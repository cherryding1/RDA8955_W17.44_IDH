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























#ifndef __TM_H__
#define __TM_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "cswtype.h"

typedef struct _FILETIME
{
    UINT32 DateTime;
} TM_FILETIME, *TM_PFILETIME;

typedef struct _TM_SYSTEMTIME
{
    UINT16 uYear;
    UINT8  uMonth;
    UINT8  uDayOfWeek;
    UINT8  uDay;
    UINT8  uHour;
    UINT8  uMinute;
    UINT8  uSecond;
    UINT16 uMilliseconds;
} TM_SYSTEMTIME, *TM_PSYSTEMTIME;

typedef struct _TM_ALARM
{
    UINT32 nFileTime;
    UINT8 *pText;
    UINT8 nTextLength;
    UINT8 nRecurrent;
    UINT8 padding[2];
    UINT16 nIndex;
    UINT16 nType;
} TM_ALARM;

typedef struct _TM_SMS_TIME_STAMP
{
    UINT16 uYear;
    UINT8 uMonth;
    UINT8 uDay;
    UINT8 uHour;
    UINT8 uMinute;
    UINT8 uSecond;
    INT8 iZone;
} TM_SMS_TIME_STAMP;
#define  TM_SET_ALARM_RECUR(mon, tue, wed, thu, fri, sta, sun ) \
 ((1<<mon|1<<tue|1<<wed|1<<thu|1<<fri|1<<sta|1<<sun)&0xFE)

#define TM_FMT_DOT_TIME         (1<<0)  //0000000000000001--->10.33
#define TM_FMT_COLON_TIME       (1<<1)  //0000000000000010--->10:33

#define TM_FMT_TWELVE_TIME      (1<<2)  //0000000000000100--->2:33pm
#define TM_FMT_TWENTYFOUR_TIME  (1<<3)  //0000000000001000--->14:33 

#define TM_FMT_DMY_DATE         (1<<4)  //0000000000010000--->13/06/2005
#define TM_FMT_MDY_DATE         (1<<5)  //0000000000100000--->06/13/2005
#define TM_FMT_YMD_DATE         (1<<6)  //0000000001000000--->2005/06/13

#define TM_FMT_DOT_DATE         (1<<7)  //0000000010000000--->2005.26.13
#define TM_FMT_COLON_DATE       (1<<8)  //0000000100000000--->2005:26:13
#define TM_FMT_SLASH_DATE       (1<<9)  //0000001000000000--->2005/06/13
#define TM_FMT_BASELINE_DATE    (1<<10) //0000010000000000--->2005-06-13

#define TM_MODULE_BASE_ERR      0x0
//read register error
#define TM_OPEN_REG_ERROR             (TM_MODULE_BASE_ERR+1)
#define TM_CLOSE_REG_ERROR            (TM_MODULE_BASE_ERR+2)
#define TM_READ_REG_ERROR             (TM_MODULE_BASE_ERR+3)
#define TM_WRITE_REG_ERROR            (TM_MODULE_BASE_ERR+4)
#define TM_PARA_NULL_ERROR            (TM_MODULE_BASE_ERR+5)
#define TM_ALARM_TIME_MIN_ERROR       (TM_MODULE_BASE_ERR+6)
#define TM_ALARM_TIME_EQUAL_ERROR     (TM_MODULE_BASE_ERR+7)
#define TM_ALARM_RECURRENT_ERROR      (TM_MODULE_BASE_ERR+8)
#define TM_ALARM_INDEX_ERROR          (TM_MODULE_BASE_ERR+9)
#define TM_ALARM_TEXT_LEN_ERROR       (TM_MODULE_BASE_ERR+10)
#define TM_WRITE_FLASH_ERROR          (TM_MODULE_BASE_ERR+11)
#define TM_READ_FLASH_ERROR           (TM_MODULE_BASE_ERR+12)
#define TM_ALARM_UNKNOWN_ERROR        (TM_MODULE_BASE_ERR+13)
#define TM_NOT_VALID_YEAR_ERROR       (TM_MODULE_BASE_ERR+14)
#define TM_NOT_VALID_MONTH_ERROR      (TM_MODULE_BASE_ERR+15)
#define TM_NOT_VALID_DAY_ERROR        (TM_MODULE_BASE_ERR+16)
#define TM_NOT_VALID_HOUR_ERROR       (TM_MODULE_BASE_ERR+17)
#define TM_NOT_VALID_MINNUTE_ERROR    (TM_MODULE_BASE_ERR+19)
#define TM_NOT_VALID_SECOND_ERROR     (TM_MODULE_BASE_ERR+20)
#define TM_NOT_VALID_FORMATE_ERROR    (TM_MODULE_BASE_ERR+21)

#define TM_DEFAULT_DATE_YEAR      		2017
#define TM_DEFAULT_DATE_MONTH  		1
#define TM_DEFAULT_DATE_DAY       		1
#define TM_DEFAULT_DATE_WEEKDAY 	7
#define TM_DEFAULT_TIME_HOUR     		10
#define TM_DEFAULT_TIME_MINUTE  		0
#define TM_DEFAULT_TIME_SECOND         	0

BOOL SRVAPI TM_SystemTimeToFileTime(
    CONST TM_SYSTEMTIME *pSystemTime,
    TM_FILETIME *pFileTime
);

BOOL SRVAPI TM_FileTimeToSystemTime (
    CONST TM_FILETIME nFileTime,
    TM_SYSTEMTIME *pSystemTime
);

BOOL SRVAPI TM_FormatSystemTime(
    CONST TM_SYSTEMTIME *pSystemTime,
    UINT16 uFormat,
    RESID nResFormatID,
    PSTR pDateString,
    UINT8 uDateStringSize,
    PSTR pTimeString,
    UINT8 uTimeStringSize
);



BOOL SRVAPI TM_FormatFileTime(
    CONST TM_FILETIME nFileTime,
    UINT16 uFormat,
    RESID nResFormatID,
    PSTR pDateString,
    UINT8 uDateStringSize,
    PSTR pTimeString,
    UINT8 uTimeStringSize
);

UINT32 SRVAPI TM_GetTime (
    VOID
);

BOOL SRVAPI TM_SetSystemTime(
    TM_SYSTEMTIME *pSystemTime
);

BOOL SRVAPI TM_GetSystemTime(
    TM_SYSTEMTIME *pSystemTime
);

BOOL SRVAPI TM_SetTimeFormat(
    UINT16 nTimeFormat    //The time format parameter to be set.
);
UINT16  SRVAPI TM_GetTimeFormat(void);

INT8 SRVAPI TM_GetTimeZone(
    VOID
);

BOOL SRVAPI TM_SetTimeZone(
    INT8 TimeZone
);

BOOL SRVAPI  TM_GetSystemFileTime(
    TM_FILETIME *pFileTime
);

BOOL SRVAPI TM_FormatDateTime(
    TM_FILETIME *pFileTime,
    UINT16 uFormat,
    RESID nResFormatID,
    PSTR pDateString,
    UINT8 uDateStringSize,
    PSTR pTimeString,
    UINT8 uTimeStringSize
);

BOOL SRVAPI TM_SetLocalTime(
    TM_SYSTEMTIME *pSystemTime
);
BOOL SRVAPI TM_GetLocalTime (
    TM_SYSTEMTIME *pSystemTime
);
BOOL SRVAPI TM_FormatDateTimeEx(
    TM_FILETIME *pFileTime,
    PSTR pString
);

BOOL SRVAPI TM_FormatFileTimeToStringEx(
    TM_FILETIME FileTime,
    UINT8 String[18]
);


UINT8 SRVAPI TM_SmsTimeStampToFileTime(
    TM_SMS_TIME_STAMP ts,
    TM_FILETIME *pFileTime
);

BOOL SRVAPI TM_ListAlarm (
    UINT8 *pArrayCnt,
    TM_ALARM **pAlarmArray
);

BOOL SRVAPI TM_SetAlarm(
    TM_ALARM *pAlarm,
    UINT8 *pAlarmNum
);

BOOL SRVAPI TM_KillAlarm (
    UINT8 nIndex
);

BOOL SRVAPI TM_GetAlarm (
    UINT8 nIndex,
    TM_ALARM *pAlarm
);
BOOL SRVAPI TM_ClearAllAlarm (
    VOID
);
void SRVAPI TM_GetLastErr(UINT32 *pErrorCode);


// =============================================================================
// TM_GetDayNoInYear
// -----------------------------------------------------------------------------
/// Get current day's number in current year
///
/// @param tmYear   Specifies the year.
/// @param tmMonth  Specifies the month.
/// @param tmDay    Specifies the day.
/// @return The return values is the total days number of specifies day
/// in specifies year.
// =============================================================================
PUBLIC UINT32 TM_GetDayNoInYear(UINT16 tmYear, UINT8 tmMonth, UINT8 tmDay);


// =============================================================================
// csw_TMGetTick
// -----------------------------------------------------------------------------
/// Return the up time of the system in 16384Hz ticks.
/// The uptime is given as a 32 bits numbers, therefore the user of the
/// function must take care of the count wrap, that will occurs every
/// three days.
///
/// @return Up time, in 16384Hz ticks.
// =============================================================================
PUBLIC UINT32 csw_TMGetTick(VOID);

#ifdef __cplusplus
}
#endif

#endif // _H_

