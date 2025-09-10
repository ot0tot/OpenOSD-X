#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "char_canvas.h"
#include "rtc6705.h"
#include "log.h"
#include "vtx.h"


#define PLL_SETTLE_TIME_MS      500
#define ADC_CONV_TIME_MS        100         // ms
#define VREF_INIT_MV 1800                   // mV

typedef enum {
    VTX_STATE_INIT = 0,
    VTX_STATE_INIT_RTC6705,
    VTX_STATE_PLL_SETTLE,
    VTX_STATE_IDLE,
    VTX_STATE_CALIB_START,
    VTX_STATE_CALIB_DELAY,
    VTX_STATE_ADC_ENABLE,
    VTX_STATE_ADC_CONVERSION,
    VTX_STATE_DISABLE,

}VTX_STATE;

char * vtxstate_str[] =
{
    "VTX_STATE_INIT",
    "VTX_STATE_INIT_RTC6705",
    "VTX_STATE_PLL_SETTLE",
    "VTX_STATE_IDLE",
    "VTX_STATE_CALIB_START",
    "VTX_STATE_CALIB_DELAY",
    "VTX_STATE_ADC_ENABLE",
    "VTX_STATE_ADC_CONVERSION",
    "VTX_STATE_DISABLE"
};

#ifdef TARGET_BREAKOUTBOARD

#define CAL_FREQ_SIZE 9
#define CAL_DBM_SIZE 2
typedef struct vpd_table_def {
    char magic[4];
    uint16_t calFreqs[CAL_FREQ_SIZE];
    uint8_t calDBm[CAL_DBM_SIZE];
    uint16_t calVpd[CAL_DBM_SIZE][CAL_FREQ_SIZE];
} vpd_table_t;

__attribute__((section(".vpdtable")))
const volatile vpd_table_t adj_vpdtable;
const vpd_table_t vpd_table = {
    .calFreqs = {5600,	5650,	5700,	5750,	5800,	5850,	5900, 5950, 6000},
    .calDBm = {14, 20},
    .calVpd = {
        {1300,1330,1345,1400,1480,1590,1670,1710,1760},
        {1910,1970,1980,2120,2270,2430,2540,2620,2750}
    }
};
#endif


vpd_table_t *vpdt = (void*)&vpd_table;
uint16_t target_vpd = 0;
uint16_t vpd = 0;
int32_t vref = 0;
int32_t vpd_error;
uint16_t vtx_freq = 0;
uint32_t next_state_timer = 0;
VTX_STATE vtx_state = VTX_STATE_INIT;





void initVtx(void)
{
    target_vpd = 0;
    vtx_freq = 0;

    // Check if there is an adjustment table.
    if ( adj_vpdtable.magic[0] == 'V' && 
        adj_vpdtable.magic[1] == 'P' && 
        adj_vpdtable.magic[2] == 'D' && 
        adj_vpdtable.magic[3] == 'T'){
            vpdt = (void*)&adj_vpdtable;
    }

    vtx_state = VTX_STATE_INIT;
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
    rtc6705PowerAmpOff();
    initRtc6705();
}


void setVtx_vpd(uint16_t freq, uint16_t vpd)
{

    if (vtx_freq != freq || target_vpd != vpd){
        vtx_freq = freq;
        target_vpd = vpd;
        rtc6705PowerAmpOff();
        initRtc6705();
        vtx_state = VTX_STATE_INIT_RTC6705;
        DEBUG_PRINTF("vtx_state:VTX_STATE_INIT_RTC6705");
    }
}

uint16_t bilinearInterpolation(uint16_t freq, uint8_t dB)
{
    uint8_t calFreqsIndex = 0;
    uint8_t calDBmIndex = 0;

    freq = (freq < 5600) ? 5600 : freq;
    freq = (freq > 6000) ? 6000 : freq;

    for (calFreqsIndex = 0; calFreqsIndex < (CAL_FREQ_SIZE - 1); calFreqsIndex++){
        if (freq < vpdt->calFreqs[calFreqsIndex + 1]){
            break;
        }
    }

    for (calDBmIndex = 0; calDBmIndex < CAL_DBM_SIZE; calDBmIndex++){
        if (dB == vpdt->calDBm[calDBmIndex]){
            break;
        }
    }
    if (calDBmIndex >= CAL_DBM_SIZE){
        calDBmIndex = 0;    // default
    }

    float y = freq;
    float y1 = vpdt->calFreqs[calFreqsIndex];
    float y2 = vpdt->calFreqs[calFreqsIndex + 1];

    float fxy1 = vpdt->calVpd[calDBmIndex][calFreqsIndex];
    float fxy2 = vpdt->calVpd[calDBmIndex][calFreqsIndex + 1];

    uint16_t fxy = fxy1 * (y2 - y) / (y2 - y1) + fxy2 * (y - y1) / (y2 - y1);

    DEBUG_PRINTF("freq:%d dB:%d vpd_target:%d", freq, dB, fxy);

    return fxy;
}

void setVtx(uint16_t freq, uint8_t dB)
{
    if (dB < 10){
        setVtx_vpd(freq, 0);
    }else if (dB >= 26){
        setVtx_vpd(freq, 3000);
    }else{
        setVtx_vpd(freq, bilinearInterpolation(freq, dB));
    }
}

uint16_t getVtxFreq(void)
{
    return vtx_freq;
}

uint16_t getVpdTarget(void)
{
    return target_vpd;
}

uint16_t getVpd(void)
{
    return vpd;
}

uint16_t getVref(void)
{
    return vref;
}

void vrefUpdate(void)
{
    LL_ADC_ClearFlag_EOC(ADC2);
    vpd = ( LL_ADC_REG_ReadConversionData12(ADC2) * 3300) / 65535;
    vpd_error = (int32_t)vpd - target_vpd;
    if (vpd_error < -100){
        vref += 10;
    }else if (vpd_error < 0){
        vref += 1;
    }else if (vpd_error == 0){
        ;
    }else if (vpd_error < 100){
        vref -= 1;
    }else{
        vref -= 10;
    }
    vref = (vref < 0) ? 0: vref;
    vref = (vref > 3300) ? 3300: vref;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, (uint32_t)(0xfff*vref)/3300);

}

void debuglogVtx(void)
{
    DEBUG_PRINTF("%s vpd:%d target:%d error:%d vref:%d vtx_freq:%d %s", 
        vtxstate_str[vtx_state], 
        vpd,  
        target_vpd, 
        vpd_error, 
        vref, 
        vtx_freq,
        (vpdt == &adj_vpdtable) ? "adj_vpdtable" : ""
        );
}

void procVtx(void)
{

    uint32_t now = HAL_GetTick();

    switch(vtx_state){
        case VTX_STATE_INIT:
            break;
        case VTX_STATE_INIT_RTC6705:
            if (initRtc6705check()){
                vref = VREF_INIT_MV;
                LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, (uint32_t)vref);
                rtc6705PowerAmpOff();
                rtc6705WriteFrequency(vtx_freq);
                next_state_timer = HAL_GetTick();
                vtx_state = VTX_STATE_PLL_SETTLE;
                DEBUG_PRINTF("vtx_state:VTX_STATE_PLL_SETTLE");
            }
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
                vtx_state = VTX_STATE_CALIB_START;
            }
            break;
        case VTX_STATE_CALIB_START:
            if ( LL_ADC_IsCalibrationOnGoing(ADC2) == 0){
                vtx_state = VTX_STATE_CALIB_DELAY;
                next_state_timer = now;
            }
            break;
        case VTX_STATE_CALIB_DELAY:
            if ( (now - next_state_timer) > 2 ){
                LL_ADC_Enable(ADC2);
                vtx_state = VTX_STATE_ADC_ENABLE;
            }
            break;
        case VTX_STATE_ADC_ENABLE:
            if (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 1){
                LL_ADC_REG_StartConversion(ADC2);
                vtx_state = VTX_STATE_ADC_CONVERSION;
            }
            break;
        case VTX_STATE_ADC_CONVERSION:
            if (LL_ADC_IsActiveFlag_EOC(ADC2) == 1){
                vrefUpdate();
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
