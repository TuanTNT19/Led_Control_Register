#ifndef DMA_H
#define DMA_H

#include "stdint.h"
void DMA_Init();
void DMA1_Config(uint32_t srcAdd, uint32_t destAdd, uint16_t size);

#endif