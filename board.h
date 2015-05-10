#ifndef _BOARD_H_
#define _BOARD_H_

#define _port_bit(port, pin)	P##port##_##pin
#define port_bit(port, pin)	_port_bit(port, pin)

#define _dir_register(port)	P##port##DIR
#define dir_register(port)	_dir_register(port)

#define BOARD_TI_DONGLE		1
#define BOARD_SRF_STICK		2

#if BOARD==BOARD_TI_DONGLE

#define LED_PORT	1
#define LED_PIN		1

#elif BOARD==BOARD_SRF_STICK

#define LED_PORT	1
#define LED_PIN		7

#else
#error "BOARD is undefined"
#endif

#endif  // _BOARD_H_
