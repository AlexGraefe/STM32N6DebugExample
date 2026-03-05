 /**
 ******************************************************************************
 * @file    app_config.h
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

#ifndef APP_CONFIG
#define APP_CONFIG

#include "arm_math.h"

#define USE_DCACHE

/*Defines: CMW_MIRRORFLIP_NONE; CMW_MIRRORFLIP_FLIP; CMW_MIRRORFLIP_MIRROR; CMW_MIRRORFLIP_FLIP_MIRROR;*/
#define CAMERA_FLIP CMW_MIRRORFLIP_FLIP_MIRROR

#define ASPECT_RATIO_CROP       (1) /* Crop both pipes to nn input aspect ratio; Original aspect ratio kept */
#define ASPECT_RATIO_FIT        (2) /* Resize both pipe to NN input aspect ratio; Original aspect ratio not kept */
#define ASPECT_RATIO_FULLSCREEN (3) /* Resize camera image to NN input size and display a maximized image. See Doc/Build-Options.md#aspect-ratio-mode */
#define ASPECT_RATIO_MODE ASPECT_RATIO_FIT // ASPECT_RATIO_CROP

/* Model Related Info */
#define POSTPROCESS_TYPE    POSTPROCESS_OD_ST_YOLOX_UI

#define COLOR_BGR (0)
#define COLOR_RGB (1)
#define COLOR_MODE    COLOR_RGB

/* Runtime / HW settings */
#define APP_CAMERA_FPS                     (10)
#define APP_VENC_WIDTH                     (144)
#define APP_VENC_HEIGHT                    (96)
#define APP_VENC_FRAMERATE                 (APP_CAMERA_FPS)
#define APP_SECONDARY_PIPE_BPP             (2)
#define APP_SECONDARY_PIPE_BUFFER_SIZE     (APP_VENC_WIDTH * APP_VENC_HEIGHT * APP_SECONDARY_PIPE_BPP)
#define APP_VENC_OUTPUT_BUFFER_WORDS       (APP_VENC_WIDTH * APP_VENC_HEIGHT / 8)
#define APP_IRIS_SPI_BAUDRATE_PRESCALER    SPI_BAUDRATEPRESCALER_16
#define APP_STREAM_LOOP_DELAY_MS           (10)

/* SD Card */
#define SD_CARD_ENABLE 0
#define BLOCK_SIZE         (512) /* 1 Block = 512 Bytes */
#define NB_BLOCKS_TO_WRITE (256)  /* Write 128 Blocks at once*/
#define NB_BYTES_TO_WRITE (NB_BLOCKS_TO_WRITE*BLOCK_SIZE)
#define NB_WORDS_TO_WRITE (NB_BYTES_TO_WRITE/4)
#define NB_BLOCKS_ERASED  (1024*1024) /*Erase 512 MBytes = 512*1024*1024 */

/* Classes */
#define NB_CLASSES   (1)
#define CLASSES_TABLE const char* classes_table[NB_CLASSES] = {\
   "person"}\

/* I/O configuration */
/* Postprocessing ST_YOLO_X configuration */
#define AI_OD_ST_YOLOX_PP_NB_CLASSES        (1)
#define AI_OD_ST_YOLOX_PP_L_GRID_WIDTH      (60)
#define AI_OD_ST_YOLOX_PP_L_GRID_HEIGHT     (60)
#define AI_OD_ST_YOLOX_PP_L_NB_INPUT_BOXES  (AI_OD_ST_YOLOX_PP_L_GRID_WIDTH * AI_OD_ST_YOLOX_PP_L_GRID_HEIGHT)
#define AI_OD_ST_YOLOX_PP_M_GRID_WIDTH      (30)
#define AI_OD_ST_YOLOX_PP_M_GRID_HEIGHT     (30)
#define AI_OD_ST_YOLOX_PP_M_NB_INPUT_BOXES  (AI_OD_ST_YOLOX_PP_M_GRID_WIDTH * AI_OD_ST_YOLOX_PP_M_GRID_HEIGHT)
#define AI_OD_ST_YOLOX_PP_S_GRID_WIDTH      (15)
#define AI_OD_ST_YOLOX_PP_S_GRID_HEIGHT     (15)
#define AI_OD_ST_YOLOX_PP_S_NB_INPUT_BOXES  (AI_OD_ST_YOLOX_PP_S_GRID_WIDTH * AI_OD_ST_YOLOX_PP_S_GRID_HEIGHT)
#define AI_OD_ST_YOLOX_PP_NB_ANCHORS        (3)
static const float32_t AI_OD_ST_YOLOX_PP_L_ANCHORS[2*AI_OD_ST_YOLOX_PP_NB_ANCHORS] = {30.000000, 30.000000, 4.200000, 15.000000, 13.800000, 41.999999};
static const float32_t AI_OD_ST_YOLOX_PP_M_ANCHORS[2*AI_OD_ST_YOLOX_PP_NB_ANCHORS] = {15.000000, 15.000000, 2.100000, 7.500000, 6.900000, 21.000000};
static const float32_t AI_OD_ST_YOLOX_PP_S_ANCHORS[2*AI_OD_ST_YOLOX_PP_NB_ANCHORS] = {7.500000, 7.500000, 1.050000, 3.750000, 3.450000, 10.500000};
#define AI_OD_ST_YOLOX_PP_IOU_THRESHOLD      (0.5)
#define AI_OD_ST_YOLOX_PP_CONF_THRESHOLD     (0.6)
#define AI_OD_ST_YOLOX_PP_MAX_BOXES_LIMIT    (100)

/* Display */
#define WELCOME_MSG_1         "st_yolo_x_nano_480_1.0_0.25_3_st_int8.tflite"
#define WELCOME_MSG_2         "Model Running in STM32 MCU internal memory"

#endif
