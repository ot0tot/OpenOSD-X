#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "char_canvas.h"
#include "rtc6705.h"
#include "log.h"
#include "vtx.h"


#define PLL_SETTLE_TIME_MS      500
#define ADC_CONV_TIME_MS        100         // ms

typedef enum {
    VTX_STATE_INIT = 0,
    VTX_STATE_PLL_SETTLE,
    VTX_STATE_IDLE,
    VTX_STATE_CALIBRATION,
    VTX_STATE_DELAY,
    VTX_STATE_ENABLE,
    VTX_STATE_CONVERSION,
    VTX_STATE_DISABLE,

}VTX_STATE;


uint16_t vtx_vpd = 0;
int32_t vref = 0;
uint16_t vtx_freq = 0;
uint32_t next_state_timer = 0;
VTX_STATE vtx_state = VTX_STATE_INIT;





void initVtx(void)
{
    vtx_vpd = 0;

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
    setVtx(5800, 0);
}


void setVtx(uint16_t freq, uint16_t vpd)
{
    vtx_freq = freq;
    vtx_vpd = vpd;

    vref = 0;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, (uint32_t)vref);
    rtc6705PowerAmpOff();
    rtc6705WriteFrequency(vtx_freq);
    vtx_state = VTX_STATE_PLL_SETTLE;
    next_state_timer = HAL_GetTick();
}

void procVtx(void)
{
    static uint8_t debug_count;

    uint32_t now = HAL_GetTick();

    switch(vtx_state){
        case VTX_STATE_INIT:
            break;
        case VTX_STATE_PLL_SETTLE:
            if ( (now - next_state_timer) >= PLL_SETTLE_TIME_MS ){
                LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, (uint32_t)vref);
                rtc6705PowerAmpOn();
                vtx_state = VTX_STATE_IDLE;
                next_state_timer = now;
            }
            break;
        case VTX_STATE_IDLE:
            if ( (now - next_state_timer) >= ADC_CONV_TIME_MS ){
                //DEBUG_PRINTF("adc_calibration");
                LL_ADC_StartCalibration(ADC2, LL_ADC_SINGLE_ENDED);
                vtx_state = VTX_STATE_CALIBRATION;
            }
            break;
        case VTX_STATE_CALIBRATION:
            if ( LL_ADC_IsCalibrationOnGoing(ADC2) == 0){
                vtx_state = VTX_STATE_DELAY;
                next_state_timer = now;
            }
            break;
        case VTX_STATE_DELAY:
            if ( (now - next_state_timer) > 2 ){
                LL_ADC_Enable(ADC2);
                vtx_state = VTX_STATE_ENABLE;
            }
            break;
        case VTX_STATE_ENABLE:
            if (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 1){
                LL_ADC_REG_StartConversion(ADC2);
                vtx_state = VTX_STATE_CONVERSION;
            }
            break;
        case VTX_STATE_CONVERSION:
            if (LL_ADC_IsActiveFlag_EOC(ADC2) == 1){
                LL_ADC_ClearFlag_EOC(ADC2);
                int16_t vpd = ( LL_ADC_REG_ReadConversionData12(ADC2) * 3300) / 65535;
                int32_t vpd_error = vtx_vpd - vpd;
                if ( vpd_error>>2 != 0){
                    vref += vpd_error>>2;
                }else if ( vpd_error>>1 != 0){
                    vref += vpd_error>>1;
                }
                vref = (vref < 0) ? 0: vref;
                LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, (uint32_t)vref);
                if (!debug_count){
                    DEBUG_PRINTF("vpd:%d target:%d error:%d vref:%d vtx_freq:%d", vpd, vtx_vpd, vpd_error, vref, vtx_freq);
                }
                debug_count = (debug_count +1) % 10;
                vtx_state = VTX_STATE_DISABLE;
                LL_ADC_Disable(ADC2);
            }
            break;
        case VTX_STATE_DISABLE:
            if (!LL_ADC_IsEnabled(ADC2)){
                vtx_state = VTX_STATE_IDLE;
            }
            break;
    }
}




