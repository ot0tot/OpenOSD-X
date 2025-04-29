#ifndef __MSP_H
#define __MSP_H

void initMsp(void);
uint8_t* getMspdata(void);
void txMsp(uint8_t cmd, uint8_t *payload, uint8_t payloadlen);
int32_t rxMsp(uint8_t data);

#endif
