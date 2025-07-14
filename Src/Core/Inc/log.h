#ifndef __LOG_H
#define __LOG_H

#if USE_RTT
    #include "segger_rtt.h"
    #include "sys_timer.h"
    #define DEBUG_INIT()                SEGGER_RTT_Init()
    #define DEBUG_PRINTF(fmt, ...)      SEGGER_RTT_printf(0, "%010ld " fmt "\n",  (uint32_t)(HAL_GetTick()), ##__VA_ARGS__)
    #define DEBUG_PRINTRAW(fmt, ...)    SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
    #define DEBUG_PRINTTIMESTAMP()      SEGGER_RTT_printf(0, "%010ld ",  (uint32_t)(HAL_GetTick()))
#else
    #define DEBUG_INIT()            ((void)0)
    #define DEBUG_PRINTF(...)       ((void)0)
    #define DEBUG_PRINTRAW(...)     ((void)0)
    #define DEBUG_PRINTTIMESTAMP()  ((void)0)
#endif

#endif
