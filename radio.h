#ifndef _RADIO_H_
#define _RADIO_H_

// Configure radio for Medtronic RF

void radio_init(void);

// Poll for incoming packet

int radio_receive(uint8_t *buf, int buf_len);

#endif  // _RADIO_H_
