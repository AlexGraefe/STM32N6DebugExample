 /**
 ******************************************************************************
 * @file    main.c
 * @author  GPM Application Team
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include <string.h>
#include <unistd.h>

#include "console.h"
#include "timer.h"
#include "iris.h"
#include "fuseprogramming.h"
#include "gpio.h"
#include "iac.h"
#include "mylcd.h"
#include "mpu.h"
#include "sd_card.h"
#include "security.h"
#include "stm32n6xx_hal.h"
#include "stm32n6xx_hal_spi.h"
#include "system_clock.h"
#include "venc.h"

#include "cmw_camera.h"
#include "stm32n6570_discovery_bus.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32n6570_discovery_xspi.h"
#include "stm32n6570_discovery_sd.h"
#include "stm32n6570_discovery.h"
#include "stm32_lcd.h"
#include "stm32_lcd_ex.h"

#include "h264encapi.h"
#include "stm32n6xx_ll_venc.h"

#include "app_camerapipeline.h"
#include "main.h"
#include <stdio.h>
#include "app_config.h"
#include "crop_img.h"
#include "stlogo.h"

CLASSES_TABLE;

#ifndef APP_GIT_SHA1_STRING
#define APP_GIT_SHA1_STRING "dev"
#endif
#ifndef APP_VERSION_STRING
#define APP_VERSION_STRING "unversioned"
#endif

uint16_t * pipe_buffer[2];
volatile uint8_t buf_index_changed = 0;
uint32_t img_addr = 0;

uint32_t output_buffer[APP_VENC_OUTPUT_BUFFER_WORDS] __NON_CACHEABLE __attribute__((aligned(8))); // __NON_CACHEABLE


volatile int32_t cameraFrameReceived;
void* pp_input;

#define ALIGN_TO_16(value) (((value) + 15) & ~15)

uint8_t secondary_pipe_buffer1[APP_SECONDARY_PIPE_BUFFER_SIZE] __NON_CACHEABLE __attribute__ ((aligned (32))); // needs to be aligned on 32 bytes for DCMIPP output buffer

uint8_t secondary_pipe_buffer2[APP_SECONDARY_PIPE_BUFFER_SIZE] __NON_CACHEABLE __attribute__ ((aligned (32))); // needs to be aligned on 32 bytes for DCMIPP output buffer

extern DCMIPP_HandleTypeDef hcamera_dcmipp;

static void Hardware_init(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  Hardware_init();

  /*** App header *************************************************************/
  printf("========================================\n");
  printf("STM32N6-GettingStarted-ObjectDetection %s (%s)\n", APP_VERSION_STRING, APP_GIT_SHA1_STRING);
  printf("Build date & time: %s %s\n", __DATE__, __TIME__);
  #if defined(__GNUC__)
  printf("Compiler: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(__ICCARM__)
  printf("Compiler: IAR EWARM %d.%d.%d\n", __VER__ / 1000000, (__VER__ / 1000) % 1000 ,__VER__ % 1000);
#else
  printf("Compiler: Unknown\n");
#endif
  printf("HAL: %lu.%lu.%lu\n", __STM32N6xx_HAL_VERSION_MAIN, __STM32N6xx_HAL_VERSION_SUB1, __STM32N6xx_HAL_VERSION_SUB2);
  printf("========================================\n\n");

  PRINTF_START("Camera Init");
  CameraPipeline_Init(&(get_lcd_lcd_bg_area()->XSize), &(get_lcd_lcd_bg_area()->YSize), APP_VENC_WIDTH, APP_VENC_HEIGHT);
  PRINTF_END("Camera Init");

  PRINTF_START("LCD Init");
  PRINTF_END("LCD Init");

  PRINTF_START("VENC Init");
  LL_VENC_Init();
  encoder_prepare(APP_VENC_WIDTH, APP_VENC_HEIGHT, APP_VENC_FRAMERATE, output_buffer);
  const uint8_t *encoded_frame = encoder_get_last_frame_data();
  uint32_t encoded_frame_size = encoder_get_last_frame_size();
  uint32_t sd_stream_offset = 0;
  
  if ((encoded_frame != NULL) && (encoded_frame_size > 0U)) {
    printf("Transmitting initial stream header with size %lu\n", (unsigned long)encoded_frame_size);
    iris_transmit(encoded_frame, encoded_frame_size);
    
    if (save_stream(sd_stream_offset, (const uint32_t *)encoded_frame, encoded_frame_size) != 0) {
      printf("Error saving initial stream header\n");
    } else {
      sd_stream_offset += encoded_frame_size;
    }
  } else {
    printf("No encoded frame data available\n");
  }

  PRINTF_END("VENC Init");

  // CameraPipeline_DisplayPipe_Start(get_lcd_bg_buffer(), CMW_MODE_CONTINUOUS);
  CameraPipeline_SecondaryPipe_Start(secondary_pipe_buffer1,secondary_pipe_buffer2, CMW_MODE_CONTINUOUS);  // secondary_pipe_buffer1, secondary_pipe_buffer2, CMW_MODE_CONTINUOUS);
  img_addr = (uint32_t) secondary_pipe_buffer1;
  printf("Waiting 5s before SPI streaming...\n");
  HAL_Delay(5000);
  printf("Starting SPI streaming\n");

  while (1)
  {
    CameraPipeline_IspUpdate();
    int asd = 0;
    while (!buf_index_changed) {
    };
    /* new frame available */
    buf_index_changed = 0;
    printf("\n");
    if (Encode_frame(img_addr) != 0) {
      printf("Error encoding frame\n");
      continue;
    }

    /* Saving/transmitting the frame */
    encoded_frame = encoder_get_last_frame_data();
    encoded_frame_size = encoder_get_last_frame_size();
    if ((encoded_frame != NULL) && (encoded_frame_size > 0U)) {
      iris_transmit(encoded_frame, encoded_frame_size);
      if (save_stream(sd_stream_offset, (const uint32_t *)encoded_frame, encoded_frame_size) != 0) {
        printf("Error saving encoded frame\n");
      } else {
        sd_stream_offset += encoded_frame_size;
      }
    } else {
      printf("No encoded frame data available\n");
    }
    // HAL_Delay(APP_STREAM_LOOP_DELAY_MS);
  }


}

static void Hardware_init(void)
{
  /* enable MPU configuration to create non cacheable sections */
  MPU_Config();

  /* Power on ICACHE */
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_ICACTIVE_Msk;

  HAL_Init();

  SCB_EnableICache();

#if defined(USE_DCACHE)
  /* Power on DCACHE */
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_DCACTIVE_Msk;
  SCB_EnableDCache();
#endif

  SystemClock_Config();

  GPIO_Config();

  CONSOLE_Config();
  printf("\n\n\n\n");

  PRINTF_START("IRIS Init");
  Timer_Config();
  PRINTF_END("IRIS Init");

  PRINTF_START("IRIS Init");
  iris_config();
  iris_handshake_blocking();
  PRINTF_END("IRIS Init");

  Fuse_Programming();

  /* External RAM */
  PRINTF_START("External Memory Init");
  BSP_XSPI_RAM_Init(0);
  BSP_XSPI_RAM_EnableMemoryMappedMode(0);

  PRINTF_END("External Memory Init");

  // init SD card
  PRINTF_START("SD Card Init");
  SD_Card_Init();
  PRINTF_END("SD Card Init");

  /* Set all required IPs as secure privileged */
  Security_Config();

  IAC_Config();
  set_clk_sleep_mode();

}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  __BKPT(0);
  while (1)
  {
  }
}

#endif
