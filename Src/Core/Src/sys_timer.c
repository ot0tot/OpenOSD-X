
#include "main.h"
#include "sys_timer.h"

uint64_t systime100us = 0;
uint16_t time_old = 0;

void initSysTimer(void)
{
    systime100us = 0;
    time_old = 0;
    LL_TIM_SetCounter(TIM16, 0);
    LL_TIM_EnableCounter(TIM16);
}

void procSysTimer(void)
{
    volatile uint16_t now = (LL_TIM_GetCounter(TIM16) & 0xffff) ;
    systime100us += (uint64_t)((now + ~time_old +1) & 0xffff);
    time_old = now;
}

uint64_t getSysTimer(void)
{
    return systime100us;
}

uint32_t HAL_GetTick(void)
{
    procSysTimer();
    return (uint32_t)((systime100us/10) & 0xffffffffUL);
}

