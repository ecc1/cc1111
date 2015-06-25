#include "arch.h"
#include "usb_defs.h"

// USB descriptors in one giant block of bytes
__xdata uint8_t usb_descriptors[] = {
	// Device descriptor
	18,
	USB_DESC_DEVICE,
	LE_WORD(0x0110),	//  bcdUSB
	0x02,			//  bDeviceClass
	0x00,			//  bDeviceSubClass
	0x00,			//  bDeviceProtocol
	USB_CONTROL_SIZE,	//  bMaxPacketSize
	LE_WORD(0xECC1),	//  idVendor
	LE_WORD(0xECC1),	//  idProduct
	LE_WORD(0x0100),	//  bcdDevice
	0x01,			//  iManufacturer
	0x02,			//  iProduct
	0x03,			//  iSerialNumber
	0x01,			//  bNumConfigurations

	// Configuration descriptor
	9,
	USB_DESC_CONFIGURATION,
	LE_WORD(67),		//  wTotalLength
	0x02,			//  bNumInterfaces
	0x01,			//  bConfigurationValue
	0x00,			//  iConfiguration
	0xC0,			//  bmAttributes
	USB_MAX_POWER >> 1,	//  bMaxPower, 2mA units

	// Control class interface
	9,
	USB_DESC_INTERFACE,
	0x00,			//  bInterfaceNumber
	0x00,			//  bAlternateSetting
	0x01,			//  bNumEndPoints
	USB_INTERFACE_CLASS,	//  bInterfaceClass
	0x02,			//  bInterfaceSubClass
	0x01,			//  bInterfaceProtocol
	0x00,			//  iInterface

	// Header functional descriptor
	5,
	USB_CS_INTERFACE,
	0x00,			//  bDescriptor SubType Header
	LE_WORD(0x0110),	//  CDC version 1.1

	// Call management functional descriptor
	5,
	USB_CS_INTERFACE,
	0x01,			// bDescriptor SubType Call Management
	0x01,			// bmCapabilities = device handles call management
	0x01,			// bDataInterface call management interface number

	// ACM functional descriptor
	4,
	USB_CS_INTERFACE,
	0x02,			// bDescriptor SubType Abstract Control Management
	0x02,			// bmCapabilities = D1 (Set_line_Coding, Set_Control_Line_State, Get_Line_Coding and Serial_State)

	// Union functional descriptor
	5,
	USB_CS_INTERFACE,
	0x06,			// bDescriptor SubType Union Functional descriptor
	0x00,			// bMasterInterface
	0x01,			// bSlaveInterface0

	// Notification EP
	7,
	USB_DESC_ENDPOINT,
	USB_INT_EP | 0x80,	// bEndpointAddress
	0x03,			// bmAttributes = intr
	LE_WORD(8),		// wMaxPacketSize
	0xFF,			// bInterval

	// Data class interface descriptor
	9,
	USB_DESC_INTERFACE,
	0x01,			// bInterfaceNumber
	0x00,			// bAlternateSetting
	0x02,			// bNumEndPoints
	0x0A,			// bInterfaceClass = data
	0x00,			// bInterfaceSubClass
	0x00,			// bInterfaceProtocol
	0x00,			// iInterface

	// Data EP OUT
	7,
	USB_DESC_ENDPOINT,
	USB_OUT_EP,		// bEndpointAddress
	0x02,			// bmAttributes = bulk
	LE_WORD(USB_OUT_SIZE),	// wMaxPacketSize
	0x00,			// bInterval

	// Data EP in
	7,
	USB_DESC_ENDPOINT,
	USB_IN_EP | 0x80,	// bEndpointAddress
	0x02,			// bmAttributes = bulk
	LE_WORD(USB_IN_SIZE),	// wMaxPacketSize
	0x00,			// bInterval

	// String descriptors
	4,
	USB_DESC_STRING,
	LE_WORD(0x0409),	// EN-US

	// iManufacturer
	24,
	USB_DESC_STRING,
	LE_WORD('e'),
	LE_WORD('c'),
	LE_WORD('c'),
	LE_WORD('@'),
	LE_WORD('c'),
	LE_WORD('m'),
	LE_WORD('u'),
	LE_WORD('.'),
	LE_WORD('e'),
	LE_WORD('d'),
	LE_WORD('u'),

	// iProduct
	14,
	USB_DESC_STRING,
	LE_WORD('C'),
	LE_WORD('C'),
	LE_WORD('1'),
	LE_WORD('1'),
	LE_WORD('1'),
	LE_WORD('1'),

	// iSerial
	8,
	USB_DESC_STRING,
	LE_WORD('0'),
	LE_WORD('0'),
	LE_WORD('1'),

	// Terminating zero
	0
};
