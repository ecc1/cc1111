#ifndef _DMA_H
#define _DMA_H

#include <stddef.h>

#include "cc1111.h"

void dma_init(void);

void dma_set_receive(void *buf, size_t len);

void dma_set_transmit(void *buf, size_t len);

void dma_start(void);

#endif /* _DMA_H */
