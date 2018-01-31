/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef AT_TRACE_H
#define AT_TRACE_H

// Debug Index for AT layer, and maybe for CFW, etc.
#ifdef AT_DEBUG_SUPPORT
#define DEBUG
#else
#undef DEBUG
#endif

// Trace Index for AT entry module

#define g_sw_SA         0
#define g_sw_ATE        1
#define g_sw_UART       2
#define g_sw_AT_NW      3
#define g_sw_AT_SIM     4
#define g_sw_AT_PBK     5
#define g_sw_AT_CC      6
#define g_sw_AT_SMS     7
#define g_sw_AT_SS      8
#define g_sw_GPRS       9
#define g_sw_AT_SAT     10
#define g_sw_CH         11
#define g_sw_GC         12
#define g_sw_ID         13
#define g_sw_SPEC       14
#define g_sw_TCPIP      15
#define g_sw_UTIL       16
#define g_sw_DFMS       17
#define g_sw_ATA        18

/*
FUNC:
    Intialization for MMI trace function.
**/

#define AT_TC_INIT()
#define AT_TS_ID 1

#ifndef AT_NO_TRACE

#define AT_TRACE_VERBOSE 0

/*
FUNC:
    Output trace information to UART.
PARAM:
[bSwtich]
    Output switch. TRUE, means output; FALSE, means DO NOT output.
[fmt]
    The trace information that you want to output. Refer the usage of ANSI C printf().
**/
#if (AT_TRACE_VERBOSE == 1)
#define AT_TC(nIndex, fmt, ...) \
    do \
    { \
        if ((nIndex)) \
            AT_OutputText(nIndex, "[AT]["#bSwitch"][%s][%s][%d]>>"#fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#else
#define AT_TC(nIndex, fmt, ...) \
    do \
    { \
        if ((nIndex)) \
            AT_OutputText(nIndex, "[AT]>>"#fmt, ##__VA_ARGS__); \
    } while (0)
#endif

#else

#define AT_TC(nIndex, fmt, ...)
#endif

/*
Trace一个内存块中的数据.
[bSwitch]
[buffer]
[len]
[format] 16, hexidecimal format; 10, decimal format.
**/
#ifdef DEBUG
extern void AT_TC_MEMBLOCK(UINT16 nIndex, UINT8 *buffer, UINT16 len, UINT8 radix);

#define AT_TC_ENTER_FUNC(nIndex) AT_TC(nIndex, "ENTER %s()...", __FUNCTION__)
#define AT_TC_EXIT_FUNC(nIndex) AT_TC(nIndex, "EXIT %s()...", __FUNCTION__)

/*
Because the CSW macro DS_ASSERT is invalid, so at this moment we just do it by ourselvies.

@RUBY STYLE PSEUDOCODE@
trace out the assert expression
sleep for a while to make the idle task print trace information
make system crash
**/
#define AT_ASSERT(x) if (!(x)) \
        do { \
            AT_TC(0, "[AT_ASSERT]assert expresion = %s", #x); \
            COS_Sleep(1000); \
            __asm__("break 5"); \
        } while(0)
#else
#define AT_TC_MEMBLOCK(nIndex, buffer, len, radix)
#define AT_TC_ENTER_FUNC(nIndex)
#define AT_TC_EXIT_FUNC(nIndex)
#define AT_ASSERT(x)
#endif

#endif

