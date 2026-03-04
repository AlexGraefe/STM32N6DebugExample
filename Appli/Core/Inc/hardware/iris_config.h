#ifndef SPI_CONFIG_H
#define SPI_CONFIG_H

#include "stm32n6xx_hal.h"

#define CS_PIN GPIO_PIN_3
#define CS_GPIO_PORT GPIOA

void iris_config();
void iris_handshake_blocking();
void iris_transmit(const uint8_t *data, uint32_t size);

#endif /* SPI_CONFIG_H */