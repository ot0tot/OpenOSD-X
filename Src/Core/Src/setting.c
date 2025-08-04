#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "log.h"
#include "flash.h"
#include "mspvtx.h"
#include "setting.h"


const openosdx_setting_t openosdx_setting_default = {
        1,              // version
        27,             // channel
        2,              // powerIndex
        1,              // videoFormat
        1800,           // vref_init
        0x3c5e          // magic
};

openosdx_setting_t openosdx_setting, openosdx_setting_backup;

__attribute__((section(".setting")))
const openosdx_setting_t flash_setting = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};



void setting_print(void)
{
#ifdef DEV_MODE
    UNUSED(setting);
#endif
    DEBUG_PRINTF(" version:%d", openosdx_setting.version);
    DEBUG_PRINTF(" channel:%d (%c%d)", openosdx_setting.channel, getBandLetterByIdx(openosdx_setting.channel/8), (openosdx_setting.channel%8)+1);
    DEBUG_PRINTF(" powerIndex:%d", openosdx_setting.powerIndex);
    DEBUG_PRINTF(" videoFormat:%d", openosdx_setting.videoFormat);
    DEBUG_PRINTF(" vref_init:%d", openosdx_setting.vref_init);
    

}

void setting_update(void)
{
    static uint32_t update_time = 0;
    uint32_t now = HAL_GetTick();

    if (memcmp(&openosdx_setting_backup, &openosdx_setting, sizeof(openosdx_setting_t))){
        memcpy(&openosdx_setting_backup, &openosdx_setting, sizeof(openosdx_setting_t));
        update_time = now | 0x1;
    }
    if ( update_time ){
        if ( now - update_time > 1000){ // 1sec
            update_time = 0;
            flash_erase((uint32_t)&flash_setting, sizeof(openosdx_setting_t));
            flash_write((uint32_t)&flash_setting, (uint8_t*)&openosdx_setting, sizeof(openosdx_setting_t));
            memcpy(&openosdx_setting_backup, &flash_setting, sizeof(openosdx_setting_t));
            DEBUG_PRINTF("vtx_update");
            setting_print();
        }
    }
}

openosdx_setting_t* setting(void)
{
    return &openosdx_setting;
}

void setting_init(void)
{
    memcpy(&openosdx_setting, &flash_setting, sizeof(openosdx_setting));
    memcpy(&openosdx_setting_backup, &openosdx_setting, sizeof(openosdx_setting));

    if (    openosdx_setting.version != openosdx_setting_default.version ||
            openosdx_setting.magic != openosdx_setting_default.magic) {
                DEBUG_PRINTF("vtx_default");
                memcpy(&openosdx_setting, &openosdx_setting_default, sizeof(openosdx_setting));
                memcpy(&openosdx_setting_backup, &openosdx_setting_default, sizeof(openosdx_setting_t));
                flash_erase((uint32_t)&flash_setting, sizeof(openosdx_setting_t));
                flash_write((uint32_t)&flash_setting, (uint8_t*)&openosdx_setting, sizeof(openosdx_setting_t));
    }

    DEBUG_PRINTF("vtx_init");
    setting_print();
}

