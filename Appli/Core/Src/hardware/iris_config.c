#include "iris_config.h"
#include "stm32_hal_legacy.h"
#include "stm32n6xx.h"
#include "stm32n6xx_hal_gpio.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static SPI_HandleTypeDef hspi5;
static uint32_t iris_frame_index = 0;

static void iris_transmit_raw(const uint8_t *data, uint16_t size)
{
    HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi5, (uint8_t *)data, size, ~0);
    HAL_Delay(1);
    HAL_GPIO_WritePin(CS_GPIO_PORT, CS_PIN, GPIO_PIN_SET);
}


/**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(hspi->Instance==SPI5)
    {


        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI5;
        PeriphClkInitStruct.Spi5ClockSelection = RCC_SPI5CLKSOURCE_PCLK2;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            while (1) {}
        }

        /* Peripheral clock enable */
        __HAL_RCC_SPI5_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        /**SPI5 GPIO Configuration
        PE15     ------> SPI5_SCK
        PH8     ------> SPI5_MISO
        PG2     ------> SPI5_MOSI
        PA3     ------> SPI5_NSS
        */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = CS_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        // GPIO_InitStruct.Alternate = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(CS_GPIO_PORT, &GPIO_InitStruct);

        printf("gggggg\n");

        /* USER CODE BEGIN SPI5_MspInit 1 */

        /* USER CODE END SPI5_MspInit 1 */
    }

}

void iris_config()
{
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi5.Init.NSS = SPI_NSS_SOFT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 0x7;
    hspi5.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;    // SPI_NSS_PULSE_ENABLE
    hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi5.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi5.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi5.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi5.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi5) != HAL_OK)
    {
        while(1) {}
    }
}


void iris_transmit(const uint8_t *data, uint32_t size)
{
    if ((data == NULL) || (size == 0U))
    {
        return;
    }
    uint32_t packets_per_frame = (size + IRIS_PACKET_PAYLOAD_SIZE - 1U) / IRIS_PACKET_PAYLOAD_SIZE;
    printf("total_size: %lu bytes, packets: %lu\n", size, packets_per_frame);
    iris_packet_t packet;

    for (uint32_t packet_index = 0; packet_index < packets_per_frame; packet_index++)
    {
        uint32_t offset = packet_index * IRIS_PACKET_PAYLOAD_SIZE;
        uint32_t chunk_size = size - offset;
        if (chunk_size > IRIS_PACKET_PAYLOAD_SIZE)
        {
            chunk_size = IRIS_PACKET_PAYLOAD_SIZE;
        }

        packet.frame_index = iris_frame_index;
        packet.packet_index = packet_index;
        packet.packets_per_frame = packets_per_frame;
        memset(packet.payload, 0, sizeof(packet.payload));
        memcpy(packet.payload, data + offset, chunk_size);  
        printf("transmitting frame %lu, packet %lu/%lu, chunk size: %lu bytes, %u\n", packet.frame_index, packet.packet_index, packet.packets_per_frame, chunk_size, sizeof(packet));
        iris_transmit_raw((const uint8_t *)&packet, (uint16_t)sizeof(packet));
        HAL_Delay(50);
    }

    iris_frame_index++;
}