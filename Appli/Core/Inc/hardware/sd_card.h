#ifndef SD_CARD_H
#define SD_CARD_H

#include "stm32n6xx_hal.h"

int save_stream(uint32_t offset, const uint32_t *buf, size_t size);
int flush_out_buffer(void);
int erase_enc_output(void);
void SD_Card_Init(void);

#endif /* SD_CARD_H */