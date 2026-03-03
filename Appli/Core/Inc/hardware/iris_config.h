#ifndef SPI_CONFIG_H
#define SPI_CONFIG_H

#include "stm32n6xx_hal.h"

#define CS_PIN GPIO_PIN_3
#define CS_GPIO_PORT GPIOA

#define IRIS_PACKET_PAYLOAD_SIZE 1024U

typedef struct __attribute__((packed))
{
    uint32_t frame_index;
    uint32_t packet_index;
    uint32_t packets_per_frame;
    uint8_t payload[IRIS_PACKET_PAYLOAD_SIZE];
} iris_packet_t;

void iris_config();
void iris_transmit(const uint8_t *data, uint32_t size);

#endif /* SPI_CONFIG_H */