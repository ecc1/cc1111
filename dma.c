#include "dma.h"

#include "arch.h"

__xdata struct cc_dma_channel dma_config;

void dma_init(void)
{
	// This is the same for both transmit and receive
	dma_config.cfg0 =
		DMA_CFG0_WORDSIZE_8 |
		DMA_CFG0_TMODE_SINGLE |
		DMA_CFG0_TRIGGER_RADIO;

	set_word(DMA0CFG, &dma_config);
}

#define set_high_low(name, val)	do { \
	name##_high = ((uint16_t) (val)) >> 8;	\
	name##_low = ((uint16_t) (val)) & 0xFF;	\
} while (0)

void dma_set_receive(void *buf, size_t len)
{
	set_high_low(dma_config.src, &RFDXADDR);	// from: RF data
	set_high_low(dma_config.dst, buf);		// to: user buffer
	set_high_low(dma_config.len, len);		// fixed-length packet

	// constant src, auto-increment dst by 1
	dma_config.cfg1 =
		DMA_CFG1_SRCINC_0 |
		DMA_CFG1_DESTINC_1 |
		DMA_CFG1_PRIORITY_HIGH;
}

void dma_set_transmit(void *buf, size_t len)
{
	set_high_low(dma_config.src, buf);		// from: user buffer
	set_high_low(dma_config.dst, &RFDXADDR);	// to: RF data
	set_high_low(dma_config.len, len);		// fixed-length packet

	// auto-increment src by 1, constant dst
	dma_config.cfg1 =
		DMA_CFG1_SRCINC_1 |
		DMA_CFG1_DESTINC_0 |
		DMA_CFG1_PRIORITY_HIGH;
}

void dma_start(void)
{
	DMAARM = DMAARM_DMAARM0;
}
