#ifndef __SETTING_H
#define __SETTING_H

typedef struct
{
    uint16_t version;
    uint16_t channel;
    uint16_t powerIndex;
    uint16_t videoFormat;
    uint16_t vref_init;
    uint16_t magic;
} openosdx_setting_t;

void setting_update(void);
openosdx_setting_t* setting(void);
void setting_init(void);


#endif
