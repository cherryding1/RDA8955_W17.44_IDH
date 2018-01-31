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












#ifndef __SUL_PRV_H__
#define __SUL_PRV_H__
#include "chip_id.h"
#include <csw_mem_prv.h>
//
// dm alarm define
//
#define TM_ALM_TEXT_LEN   32

#define TIM_BASE_DEFAULT   0x386D4380

typedef struct _TM_PARAMETERS
{
    UINT32 iBase;
    UINT16 iFormat;
    INT8  iZone;
    UINT8  padding;
    UINT32 iRtcBase;
} TM_PARAMETERS;

/*
* the internal alarm structure,defined for internal
* use
*/
typedef struct _TIM_ALARM_EX
{
    UINT8   pText[TM_ALM_TEXT_LEN];
    UINT32  nFileTime;
    UINT16  nIndex;
    UINT16  nType;
    UINT8   nTextLength;
    UINT8   nRecurrent;
    UINT8   padding[2];
} TIM_ALARM_EX;

/*
* the main information of alarm
*/
typedef struct _TIM_MAIN_ALARM_INFO
{
    UINT16   validIdxBit;
    UINT8       alarmNum;
    UINT8   minAlarmIndx;
    UINT16   totalTxtlen;
    UINT8     padding[2];
} TIM_MAIN_ALARM_INFO;


#define DEBUG_PRINT_SWITCH BASE_TM_TS_ID //only to test , to debug, this is print switch
#define TIM_ALARM_INDEX_MAX 15

#define TIM_POWERON_WRITE_FLASH_TIMER (50 * 16384)

BOOL TIM_WriteRegData(INT8 *pText,
                      UINT8 *dataBuf,
                      UINT32 dataLen);
#define NOTEST  1

#define SELF_DEFINE_INDEX        17

#define RTC_ALARM_TIMERSET_LIMIT  16416000

#define ALARM_FOR_RESPONSE_FLAG  0xFF
//the alarm response except week end,0x3E,bit means:00000000, from mon to sun
#define ALARM_FOR_EVERY_DAY      0x00
//the alarm response only once,0x01,bit means:00000001
#define ALARM_FOR_ONCE           0x01
//the alarm response except week end,0x3E,bit means:00111110, from mon to fri
#define ALARM_EXCEPT_WEEKEND     0x3E
#define ALARM_ALL_WEEK_DAY       0xfe

#define ALARM_MON_OF_WEEK        0x02 // 00000010
#define ALARM_TUE_OF_WEEK        0x04 // 00000100
#define ALARM_WED_OF_WEEK        0x08 // 00001000
#define ALARM_THU_OF_WEEK        0x10 // 00010000
#define ALARM_FRI_OF_WEEK        0x20 // 00100000
#define ALARM_SAT_OF_WEEK        0x40 // 01000000
#define ALARM_SUN_OF_WEEK        0x80 // 10000000

#define TIM_CLOCKS_PER_SECOND 256

#define TIM_TIME_FORMAT_POS    4
#define TIM_TIME_BASE_POS      8
#define TIM_TIME_FORMAT_OR_BASE_LEN 4
#define TIM_TIME_ZONE_POS      12
#define TIM_BAK_IDX_BIT_POS  698

#define TIM_TIME_INFO_LEN 16

#define TIM_DATA_TOL_LEN 700
//compile switch
#define TIM_REG_FFS_USED
//======mon, tue, wed, thu, fri, sta, sun==============
#define MON                      0
#define TUE                      1
#define WED                      2
#define THU                      3
#define FRI                      4
#define SAT                      5
#define SUN                      6

#define TM_STARTYEAR            2000
#define TM_ENDYEAR              2127

UINT8 TM_GetDayOfWeekEX(TM_FILETIME FileTime);
BOOL TIM_SetTimePara(TM_PARAMETERS *pTM_Parameters);
BOOL TIM_GetTimePara( TM_PARAMETERS *pTM_Parameters);
BOOL TIM_ResetAlarm();
VOID TIM_SetLastErr(UINT32);

BOOL TIM_OpenKey(HANDLE *pHkey);
BOOL TIM_CloseKey(HANDLE hkey);

BOOL TIM_WriteBinSet(HANDLE hkey,
                     INT8  *Ptext,
                     UINT8 *stringtxt,
                     UINT32 length);
BOOL TIM_ReadBinSet(HANDLE hkey,
                    INT8 *Ptext,
                    UINT8 *stringtxt,
                    UINT32 *plength);

BOOL TIM_ReadRegData(INT8 *pText,
                     UINT8 *dataBuf,
                     UINT32 *pDataLen);
BOOL TIM_WriteAlarmSet(UINT8 index);
BOOL TIM_ReadAlarmSet(UINT8 index);



#define TIM_ERR_RETURN(errcode)\
do { \
    TIM_SetLastErr(errcode);\
    return FALSE;\
}while(0)

#if NOTEST
#define ONE_DAY_SECONDS          86400
#define WEEK_SECONDS             7*ONE_DAY_SECONDS
#else
#define ONE_DAY_SECONDS          24
#define WEEK_SECONDS             7*ONE_DAY_SECONDS
#endif

//=========================Macro define==================================
#ifndef max
#define max(a,b) ((a>b) ? a : b)
#endif
#ifndef min
#define min(a,b) ((a < b) ? a : b )
#endif

//bychenwf
#endif // _H_

