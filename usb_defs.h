#ifndef _USB_DEFS_H
#define _USB_DEFS_H

#if BOARD == BOARD_TI_DONGLE

#define enable_usb_pullup()		\
	do {				\
		P1SEL &= ~(1 << 0);	\
		P1_0 = 1;		\
		P1DIR |= (1 << 0);	\
	} while (0)

#define disable_usb_pullup()		\
	do {				\
		P1_0 = 0;		\
	} while (0)

#elif BOARD == BOARD_SRF_STICK

#define enable_usb_pullup()		\
	do {				\
		P2SEL &= ~(1 << 0);	\
		P2_0 = 1;		\
		P2DIR |= (1 << 0);	\
	} while (0)

#define disable_usb_pullup()		\
	do {				\
		P2_0 = 0;		\
	} while (0)

#else
#error "unknown BOARD"
#endif

extern __xdata uint8_t usb_descriptors[];

#define USB_INTERFACE_CLASS	0x02

#define USB_SETUP_DIR_MASK	(0x01 << 7)
#define USB_SETUP_TYPE_MASK	(0x03 << 5)
#define USB_SETUP_RECIP_MASK	(0x1f)

#define USB_DIR_OUT		0
#define USB_DIR_IN		(1 << 7)

#define USB_TYPE_STANDARD	0
#define USB_TYPE_CLASS		(1 << 5)
#define USB_TYPE_VENDOR		(2 << 5)
#define USB_TYPE_RESERVED	(3 << 5)

#define USB_RECIP_DEVICE	0
#define USB_RECIP_INTERFACE	1
#define USB_RECIP_ENDPOINT	2
#define USB_RECIP_OTHER		3

// standard requests
#define	USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C

#define USB_DESC_DEVICE			1
#define USB_DESC_CONFIGURATION		2
#define USB_DESC_STRING			3
#define USB_DESC_INTERFACE		4
#define USB_DESC_ENDPOINT		5
#define USB_DESC_DEVICE_QUALIFIER	6
#define USB_DESC_OTHER_SPEED		7
#define USB_DESC_INTERFACE_POWER	8

#define USB_GET_DESC_TYPE(x)	(((x) >> 8) & 0xFF)
#define USB_GET_DESC_INDEX(x)	((x) & 0xFF)

#define USB_CONTROL_EP		0
#define USB_CONTROL_SIZE	32

#define USB_INT_EP		1
#define USB_INT_SIZE		8

#define USB_OUT_EP		4
#define USB_IN_EP		5

// USB bulk packets can only come in 8, 16, 32 and 64
// byte sizes, so we'll use 64 for everything
#define USB_IN_SIZE		64
#define USB_OUT_SIZE		64

#define USB_EP0_IDLE		0
#define USB_EP0_DATA_IN		1
#define USB_EP0_DATA_OUT	2
#define USB_EP0_STALL		3

// CDC definitions
#define USB_CS_INTERFACE      		0x24
#define USB_CS_ENDPOINT     	  	0x25

#define USB_SET_LINE_CODING		0x20
#define USB_GET_LINE_CODING		0x21
#define USB_SET_CONTROL_LINE_STATE	0x22

// Maximum power in mA
#define USB_MAX_POWER	50

#endif /* _USB_DEFS_H */
