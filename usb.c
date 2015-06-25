#include "usb.h"

#include "arch.h"
#include "background.h"
#include "board.h"
#include "usb_defs.h"
#include "set_stdio.h"

static volatile uint8_t usb_iif;
volatile uint8_t usb_running;

static void usb_set_interrupts(void)
{
	// IN interrupts on the control and IN endpoints
	USBIIE = (1 << USB_CONTROL_EP) | (1 << USB_IN_EP);

	// OUT interrupts on the OUT endpoint
	USBOIE = (1 << USB_OUT_EP);

	// Only care about reset
	USBCIE = USBCIE_RSTIE;
}

// This interrupt is shared with port 2

void usb_isr(void) __interrupt P2INT_VECTOR
{
	USBIF = 0;
	usb_iif |= USBIIF;
	if (USBCIF & USBCIF_RSTIF)
		usb_set_interrupts();
}

struct usb_setup {
	uint8_t dir_type_recip;
	uint8_t request;
	uint16_t value;
	uint16_t index;
	uint16_t length;
} usb_setup;

uint8_t usb_ep0_state;
uint8_t *usb_ep0_in_data;
uint8_t usb_ep0_in_len;
uint8_t usb_ep0_in_buf[2];
uint8_t usb_ep0_out_len;
uint8_t *usb_ep0_out_data;
uint8_t usb_configuration;

// Send an IN data packet

static void usb_ep0_flush(void)
{
	uint8_t this_len;
	uint8_t cs0;

	// If the IN packet hasn't been picked up, just return
	USBINDEX = 0;
	cs0 = USBCS0;
	if (cs0 & USBCS0_INPKT_RDY)
		return;

	this_len = usb_ep0_in_len;
	if (this_len > USB_CONTROL_SIZE)
		this_len = USB_CONTROL_SIZE;
	cs0 = USBCS0_INPKT_RDY;
	if (this_len != USB_CONTROL_SIZE) {
		cs0 = USBCS0_INPKT_RDY | USBCS0_DATA_END;
		usb_ep0_state = USB_EP0_IDLE;
	}
	usb_ep0_in_len -= this_len;
	while (this_len--)
		USBFIFO[0] = *usb_ep0_in_data++;
	USBINDEX = 0;
	USBCS0 = cs0;
}

// Data structure for GET_LINE_CODING / SET_LINE_CODING class requests

static struct usb_line_coding {
	uint32_t rate;
	uint8_t char_format;
	uint8_t parity;
	uint8_t data_bits;
} usb_line_coding = {
	115200,
	0,	// 1 stop bit
	0,	// no parity
	8,
};

// Walk through the list of descriptors and find a match

static void usb_get_descriptor(uint16_t value)
{
	uint8_t *descriptor;
	uint8_t type = value >> 8;
	uint8_t index = value & 0xFF;

	descriptor = usb_descriptors;
	while (descriptor[0] != 0) {
		if (descriptor[1] == type && index-- == 0) {
			if (type == USB_DESC_CONFIGURATION)
				usb_ep0_in_len = descriptor[2];
			else
				usb_ep0_in_len = descriptor[0];
			usb_ep0_in_data = descriptor;
			break;
		}
		descriptor += descriptor[0];
	}
}

// Read data from the ep0 OUT fifo

static void usb_ep0_fill(void)
{
	uint8_t len;

	USBINDEX = 0;
	len = USBCNT0;
	if (len > usb_ep0_out_len)
		len = usb_ep0_out_len;
	usb_ep0_out_len -= len;
	while (len--)
		*usb_ep0_out_data++ = USBFIFO[0];
}

static void usb_ep0_queue_byte(uint8_t a)
{
	usb_ep0_in_buf[usb_ep0_in_len++] = a;
}

static void usb_set_address(uint8_t address)
{
	usb_running = 1;
	USBADDR = address;
}

static void usb_set_configuration(void)
{
	// Set the IN max packet size, double buffered
	USBINDEX = USB_IN_EP;
	USBMAXI = USB_IN_SIZE >> 3;
	USBCSIH |= USBCSIH_IN_DBL_BUF;

	// Set the OUT max packet size, double buffered
	USBINDEX = USB_OUT_EP;
	USBMAXO = USB_OUT_SIZE >> 3;
	USBCSOH = USBCSOH_OUT_DBL_BUF;
}

static void usb_ep0_setup(void)
{
	// Pull the setup packet out of the fifo
	usb_ep0_out_data = (uint8_t *) &usb_setup;
	usb_ep0_out_len = 8;
	usb_ep0_fill();
	if (usb_ep0_out_len != 0)
		return;

	usb_ep0_in_data = usb_ep0_in_buf;
	usb_ep0_in_len = 0;
	switch(usb_setup.dir_type_recip & USB_SETUP_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		switch(usb_setup.dir_type_recip & USB_SETUP_RECIP_MASK) {
		case USB_RECIP_DEVICE:
			switch(usb_setup.request) {
			case USB_REQ_GET_STATUS:
				usb_ep0_queue_byte(0);
				usb_ep0_queue_byte(0);
				break;
			case USB_REQ_SET_ADDRESS:
				usb_set_address(usb_setup.value);
				break;
			case USB_REQ_GET_DESCRIPTOR:
				usb_get_descriptor(usb_setup.value);
				break;
			case USB_REQ_GET_CONFIGURATION:
				usb_ep0_queue_byte(usb_configuration);
				break;
			case USB_REQ_SET_CONFIGURATION:
				usb_configuration = usb_setup.value;
				usb_set_configuration();
				break;
			}
			break;
		case USB_RECIP_INTERFACE:
			#pragma disable_warning 110
			switch(usb_setup.request) {
			case USB_REQ_GET_STATUS:
				usb_ep0_queue_byte(0);
				usb_ep0_queue_byte(0);
				break;
			case USB_REQ_GET_INTERFACE:
				usb_ep0_queue_byte(0);
				break;
			case USB_REQ_SET_INTERFACE:
				break;
			}
			break;
		case USB_RECIP_ENDPOINT:
			switch(usb_setup.request) {
			case USB_REQ_GET_STATUS:
				usb_ep0_queue_byte(0);
				usb_ep0_queue_byte(0);
				break;
			}
			break;
		}
		break;
	case USB_TYPE_CLASS:
		switch (usb_setup.request) {
		case USB_SET_LINE_CODING:
			usb_ep0_out_len = 7;
			usb_ep0_out_data = (uint8_t *) &usb_line_coding;
			break;
		case USB_GET_LINE_CODING:
			usb_ep0_in_len = 7;
			usb_ep0_in_data = (uint8_t *) &usb_line_coding;
			break;
		case USB_SET_CONTROL_LINE_STATE:
			break;
		}
		break;
	}

	// Figure out how to ACK the setup packet and the next state

	USBINDEX = 0;
	if (usb_ep0_in_len) {
		// Sending data back to the host
		usb_ep0_state = USB_EP0_DATA_IN;
		USBCS0 = USBCS0_CLR_OUTPKT_RDY;
		if (usb_setup.length < usb_ep0_in_len)
			usb_ep0_in_len = usb_setup.length;
		usb_ep0_flush();
	} else if (usb_ep0_out_len) {
		// Receiving data from the host
		usb_ep0_state = USB_EP0_DATA_OUT;
		USBCS0 = USBCS0_CLR_OUTPKT_RDY;
	} else if (usb_setup.length) {
		// Uh-oh, the host expected to send or receive data
		// and we don't know what to do
		usb_ep0_state = USB_EP0_STALL;
		USBCS0 = USBCS0_CLR_OUTPKT_RDY | USBCS0_SEND_STALL;
	} else {
		// Simple setup packet with no data
		usb_ep0_state = USB_EP0_IDLE;
		USBCS0 = USBCS0_CLR_OUTPKT_RDY | USBCS0_DATA_END;
	}
}

static int usb_poll_ep0(void) __critical
{
	if (usb_iif & (1 << USB_CONTROL_EP)) {
		usb_iif &= ~(1 << USB_CONTROL_EP);
		return 1;
	}
	return 0;
}

// End point 0 receives all of the control messages.

static void usb_endpoint0(void)
{
	uint8_t cs0;

	if (!usb_poll_ep0())
		return;
	USBINDEX = 0;
	cs0 = USBCS0;
	if (cs0 & USBCS0_SETUP_END) {
		USBCS0 = USBCS0_CLR_SETUP_END;
		usb_ep0_state = USB_EP0_IDLE;
	}
	if (cs0 & USBCS0_SENT_STALL) {
		USBCS0 = 0;
		usb_ep0_state = USB_EP0_IDLE;
	}
	if (usb_ep0_state == USB_EP0_DATA_IN &&
	    (cs0 & USBCS0_INPKT_RDY) == 0) {
		usb_ep0_flush();
	}
	if (cs0 & USBCS0_OUTPKT_RDY) {
		switch (usb_ep0_state) {
		case USB_EP0_IDLE:
			usb_ep0_setup();
			break;
		case USB_EP0_DATA_OUT:
			usb_ep0_fill();
			USBINDEX = 0;
			if (usb_ep0_out_len == 0) {
				usb_ep0_state = USB_EP0_IDLE;
				USBCS0 = USBCS0_CLR_OUTPKT_RDY | USBCS0_DATA_END;
			} else {
				USBCS0 = USBCS0_CLR_OUTPKT_RDY;
			}
			break;
		}
	}
}

// Wait for a free IN buffer

static void usb_in_wait(void)
{
	for (;;) {
		USBINDEX = USB_IN_EP;
		if ((USBCSIL & USBCSIL_INPKT_RDY) == 0)
			break;
		await_interrupt();
	}
}

static uint16_t usb_in_bytes;
static uint16_t usb_in_bytes_last;

// Send the current IN packet

static void usb_in_send(void)
{
	USBINDEX = USB_IN_EP;
	USBCSIL |= USBCSIL_INPKT_RDY;
	usb_in_bytes_last = usb_in_bytes;
	usb_in_bytes = 0;
}

void usb_flush(void) __critical
{
	if (!usb_running)
		return;

	// If there are pending bytes, or if the last packet was full,
	// send another IN packet
	if (usb_in_bytes || (usb_in_bytes_last == USB_IN_SIZE)) {
		usb_in_wait();
		usb_in_send();
	}
}

void usb_putc(char c) __critical
{
	if (!usb_running)
		return;

	usb_in_wait();

	// Queue a byte, sending the packet when full
	USBFIFO[USB_IN_EP << 1] = c;
	if (++usb_in_bytes == USB_IN_SIZE)
		usb_in_send();
}

int usb_pollc(void) __critical
{
	static uint16_t count;
	uint8_t c;

	USBINDEX = USB_OUT_EP;
	if (count == 0) {
		if ((USBCSOL & USBCSOL_OUTPKT_RDY) == 0)
			return -1;
		count = (USBCNTH << 8) | USBCNTL;
		if (count == 0) {
			USBCSOL &= ~USBCSOL_OUTPKT_RDY;
			return -1;
		}
	}
	c = USBFIFO[USB_OUT_EP << 1];
	if (--count == 0)
		USBCSOL &= ~USBCSOL_OUTPKT_RDY;
	return c;
}

char usb_getc(void)
{
	int c;

	while ((c = usb_pollc()) == -1)
		idle();
	return c;
}

void usb_enable(void)
{
	// Turn on the USB controller
	SLEEP |= SLEEP_USB_EN;

	usb_set_configuration();

	usb_set_interrupts();

	// Enable USB interrupts
	IEN2 |= IEN2_USBIE;

	// Clear any pending interrupts
	USBCIF = 0;
	USBOIF = 0;
	USBIIF = 0;

	enable_usb_pullup();
}

void usb_disable(void)
{
	// Disable USB interrupts
	USBIIE = 0;
	USBOIE = 0;
	USBCIE = 0;
	IEN2 &= ~IEN2_USBIE;

	disable_usb_pullup();

	// Clear any pending interrupts
	USBCIF = 0;
	USBOIF = 0;
	USBIIF = 0;

	// Turn off the USB controller
	SLEEP &= ~SLEEP_USB_EN;
}

void usb_init(void)
{
	usb_enable();
	add_background_task(usb_endpoint0);
}

void use_usb_stdio(void)
{
	usb_init();
	set_stdio(usb_getc, usb_putc);
}
