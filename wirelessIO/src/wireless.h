#ifndef WIRELESS_H_
#define WIRELESS_H_

void initWireless(void);
void w_send(uint8_t * buff, uint8_t len);
uint32_t w_read(uint8_t* buff, uint32_t len);

#endif /* WIRELESS_H_ */
