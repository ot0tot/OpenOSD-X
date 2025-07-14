#ifndef __LED_H
#define __LED_H

typedef enum {
    LED_OFF=0,
    LED_GREEN128,
    LED_RED128,
    LED_BLUE128
} LED_STATE;

void initLed(void);
void setLed(LED_STATE led);

#endif
