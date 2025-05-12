#ifndef __LOG_H
#define __LOG_H

#if USE_RTT
    #include "segger_rtt.h"
    #include "sys_timer.h"
    #define DEBUG_INIT()        SEGGER_RTT_Init()
    #define DEBUG_PRINTF(fmt, ...)   SEGGER_RTT_printf(0, "%010ld " fmt "\n",  (uint32_t)(getSysTimer()/10), ##__VA_ARGS__)
    #define DEBUG_PRINTF2(fmt, ...)   SEGGER_RTT_printf(0, fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_INIT()        ((void)0)
    #define DEBUG_PRINTF(...)   ((void)0)
#endif

#endif
