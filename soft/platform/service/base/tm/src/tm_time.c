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
















#include "chip_id.h"
#include <cswtype.h>
#include <sul.h>
#include <tm.h>
#include <drv.h>
#include "hal_timers.h"

#include "csw_debug.h"
#include "csw_csp.h"
#include <ts.h>
#include <drv_prv.h>
#include "tm_prv.h"


#ifdef TIM_REG_FFS_USED
#else
#include <block_flash_op_api.h>
#endif

TM_PARAMETERS G_TM_Parameters;
UINT8 TIM_Glo_Infor_Buf[TIM_DATA_TOL_LEN];
UINT32  TIM_LastErrorCode;
UINT32 gCSWPowerOnTime = 0;

UINT32 SRVAPI CFW_CfgGetTimeParam(TM_FILETIME *pTM);
UINT32 SRVAPI CFW_CfgSetTimeParam(TM_FILETIME *pTM);

//#ifdef _T_UPGRADE_PROGRAMMER
#if 0
INT32  REG_OpenKey(HANDLE hKey, PCSTR pcSubKey, HANDLE *phKey)
{
    hKey = hKey;
    pcSubKey = pcSubKey;
    phKey = phKey;
    return 0;
}


INT32 REG_CloseKey(HANDLE hKey)
{
    hKey = hKey;
    return 0;
}


INT32  REG_SetValue(
    HANDLE hKey,
    PCSTR pcValueName,
    UINT8 iType,
    PVOID pvData,
    UINT8 iDataSize
)
{
    hKey = hKey;
    pcValueName = pcValueName;
    iType = iType;
    pvData = pvData;
    iDataSize = iDataSize;
    return 0;
}


INT32 REG_GetValue(
    HANDLE hKey,
    PCSTR pcValueName,
    UINT8 *pcType,
    PVOID pvData,
    UINT8 *pDataSize
)
{
    hKey = hKey;
    pcValueName = pcValueName;
    pcType = pcType;
    pvData = pvData;
    pDataSize = pDataSize;
    return 0;
}
#endif

/*
*Description:
*  copy global buffer to protect data if operate flash
*fail.only used in TIM_KillAlarmEX
*Parameter:
*  destBuf[in]: .
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_ReadGloTolData(UINT8 *destBuf)
{
    if(destBuf == NULL)
    {
        return FALSE;
    }

    SUL_MemSet8(destBuf, 0, TIM_DATA_TOL_LEN);
    SUL_MemCopy8(destBuf, TIM_Glo_Infor_Buf, TIM_DATA_TOL_LEN);

    return TRUE;
}

/*
*Description:
*  copy bak buffer to global if operate flash
*fail.only used in TIM_KillAlarmEX
*Parameter:
*  srcBuf[in]: .
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_WriteGloTolData(UINT8 *srcBuf)
{
    if(srcBuf == NULL)
    {
        return FALSE;
    }

    SUL_MemCopy8(TIM_Glo_Infor_Buf, srcBuf, TIM_DATA_TOL_LEN);

    return TRUE;
}

/*
*Description:
*  write time parameters to global buffer,a dummy flash
*Parameter:
*  pTM_Parameters[in]: .
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_SetTimePara(TM_PARAMETERS *pTM_Parameters)
{
    UINT32 formate = 0;

    if(pTM_Parameters == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    formate = pTM_Parameters->iFormat;
    SUL_MemCopy8(&TIM_Glo_Infor_Buf[TIM_TIME_FORMAT_POS], (UINT8 *)(&formate), TIM_TIME_FORMAT_OR_BASE_LEN);
    TIM_Glo_Infor_Buf[TIM_TIME_ZONE_POS] = pTM_Parameters->iZone;
    SUL_MemCopy8(&TIM_Glo_Infor_Buf[TIM_TIME_BASE_POS], (UINT8 *)(&(pTM_Parameters->iBase)), TIM_TIME_FORMAT_OR_BASE_LEN);
    G_TM_Parameters.iRtcBase = pTM_Parameters->iRtcBase;

    return TRUE;
}

/*
*Description:
*  get time parameters from global buffer,a dummy flash
*Parameter:
*  pTM_Parameters[out]: .
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_GetTimePara( TM_PARAMETERS *pTM_Parameters)
{
    if(pTM_Parameters == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    pTM_Parameters->iBase    = *((UINT32 *)(&TIM_Glo_Infor_Buf[TIM_TIME_BASE_POS]));
    pTM_Parameters->iFormat  = (UINT16)(*((UINT32 *)(&TIM_Glo_Infor_Buf[TIM_TIME_FORMAT_POS])));
    pTM_Parameters->iZone    = TIM_Glo_Infor_Buf[TIM_TIME_ZONE_POS];
    pTM_Parameters->iRtcBase = G_TM_Parameters.iRtcBase;

    return TRUE;
}


/*
*Description:
*  set alarm information to global buffer, a dummy flash
*Parameter:
*  pAlarmEX[in]:a alarm information structure pointer.
*  alarmNum[out]:the set alarm index
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_SetGloAlarmInfo(UINT16 alarmNum, TIM_ALARM_EX *pAlarmEX)
{
    UINT8 len = SIZEOF(TIM_ALARM_EX);

    if(pAlarmEX == NULL || alarmNum > TIM_ALARM_INDEX_MAX)
    {
        return FALSE;
    }

    SUL_MemCopy8((VOID *)( &TIM_Glo_Infor_Buf[TIM_TIME_INFO_LEN + (alarmNum - 1)*len]), pAlarmEX, sizeof(TIM_ALARM_EX));

    return TRUE;
}

/*
*Description:
*  get alarm information from global buffer, a dummy flash
*Parameter:
*  pAlarmEX[out]:a alarm information structure pointer.
*  alarmNum[in]:the index of alarm to get
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_GetGloAlarmInfo(UINT8 index, TIM_ALARM_EX *pAlarmEX)
{
    UINT8 len = SIZEOF(TIM_ALARM_EX);

    if(pAlarmEX == NULL)
    {
        return FALSE;
    }

    *pAlarmEX = (*((TIM_ALARM_EX *)(&TIM_Glo_Infor_Buf[TIM_TIME_INFO_LEN + (index - 1) * len])));

    return TRUE;
}

/*
*Description:
*  get error code when use TM interface fail
*Parameter:
*  pErrorCode[out]:the pointer to save error code.
*Return:
*  void type.
*/
void TM_GetLastErr(UINT32 *pErrorCode)
{
    if(pErrorCode == NULL)
        return;

    * pErrorCode = TIM_LastErrorCode;
}

/*
*Description:
*  set error code in TIM module
*Parameter:
*  ErrorCode[in]:the error code.
*Return:
*  void type.
*/
void TIM_SetLastErr(UINT32 ErrorCode)
{
    TIM_LastErrorCode = ErrorCode;
}


/*
*Function Description:
*   check the formate is valid or not.
*Parameters:
*   firBitNum[in]: the start bit of timFormat parameter
*   lastBitNum[in]: the last bit number of timFormat parameter
*   timFormate[in]: the time format
*/
PRIVATE BOOL TIM_ChckFormat(UINT8 firBitNum,
                            UINT8 lastBitNum,
                            UINT16 timFormat)
{
    UINT8 j = 0;

    for(; firBitNum <= lastBitNum; firBitNum++)
    {
        if(timFormat & (1 << firBitNum))
        {
            j++;
        }
    }

    if(j == 0 || j > 1)
    {
        TIM_ERR_RETURN(TM_NOT_VALID_FORMATE_ERROR);
    }

    return TRUE;
}

/*
*Description:
*   get the time format
*Return Value: the got format value
*/
UINT16 TM_GetTimeFormat(void)
{
    TM_PARAMETERS TM_Parameters;

    if(!TIM_GetTimePara(&TM_Parameters))
    {
        return FALSE;
    }
    return(TM_Parameters.iFormat);
}

/*
*Description:
*  set the format to flash
*Parameter:
*  timeformat[in]: the time format
*Return: BOOL
*/
BOOL TM_SetTimeFormat(UINT16 timeformat)
{
    TM_PARAMETERS TM_Parameters;
    UINT32 timeFormat = 0;

    CSW_PROFILE_FUNCTION_ENTER(TM_SetTimeFormat);

    if((!TIM_ChckFormat(0, 1, timeformat))
            || (!TIM_ChckFormat(2, 3, timeformat))
            || (!TIM_ChckFormat(4, 6, timeformat))
            || (!TIM_ChckFormat(7, 10, timeformat)))
    {
        return FALSE;
    }

    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeFormat);
        return FALSE;
    }

    TM_PARAMETERS paraBak;
    paraBak = TM_Parameters;

    TM_Parameters.iFormat = timeformat;
    timeFormat = timeformat;

    if(!TIM_SetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeFormat);
        return FALSE;
    }

#ifdef TIM_REG_FFS_USED
    if(NV_SetTmFormat((UINT8 *)(&timeFormat), 4))
    {
        TIM_SetTimePara(&paraBak);

        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeFormat);
        return FALSE;
    }

#else
    if(!TIM_WriteFlash())
    {
        TIM_SetTimePara(&paraBak);

        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeFormat);
        return FALSE;
    }
#endif

    CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeFormat);
    return TRUE;
}


/*
* Function name:    TM_IsLeapYear
* Description:
*                   determines whether the year is a leap year
* Parameters:         TM_Year
*                          Specifies the year.
* Return Values:
*                 TRUE indicates the specifies is a leap year,
*                   FALSE indicates the specifies is not a leap year
*/
PRIVATE BOOL TM_IsLeapYear(UINT16 TM_Year)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_IsLeapYear);
    if((TM_Year % 4 == 0 && TM_Year % 100 != 0) || (TM_Year % 400) == 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_IsLeapYear);
        return TRUE;
    }
    CSW_PROFILE_FUNCTION_EXIT(TM_IsLeapYear);
    return FALSE;
}

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
PUBLIC UINT32 TM_GetDayNoInYear(UINT16 tmYear, UINT8 tmMonth, UINT8 tmDay)
{

    UINT32 DayNUM = 0, MonthNUM_TMP;
    UINT8 DaysMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    CSW_PROFILE_FUNCTION_ENTER(TM_GetDayNoInYear);

    if(TM_IsLeapYear(tmYear))
    {
        DaysMonth[1] = 29;
    }

    for(MonthNUM_TMP = 1; MonthNUM_TMP < tmMonth; MonthNUM_TMP++)
    {
        DayNUM += DaysMonth[MonthNUM_TMP - 1]; //????????????
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_GetDayNoInYear);
    return(UINT32)(DayNUM + tmDay);//???????,??

}


/*
* Function name:    TM_GetDayOfWeek
* Description:
*                   Get week number int special day
* remark:
*    ???????????
*    expressions:
*       a=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
*     If a =0 ,Than the week number is Monday;
*       If a =1 ,Than the week number is Tuesday;
*       If a =2 ,Than the week number is Wednesday;
*       If a =3 ,Than the week number is Thursday;
*       If a =4 ,Than the week number is Friday;
*       If a =5 ,Than the week number is Saturday;
*       If a =6 ,Than the week number is Sunday;
* Parameters:
*                  TM_Year   Specifies the year.
*                     TM_Month  Specifies the month.
*                  TM_Day    Specifies the day.
* Return Values:
*                  The return values is the week number(describe as up)
*                  of specifies day
*/
PRIVATE UINT8 TM_GetDayOfWeek(UINT16 TM_Year,
                              UINT8  TM_Month,
                              UINT8  TM_Day)
{
    UINT8 WeekNUM = 7; // ???????????????

    CSW_PROFILE_FUNCTION_ENTER(TM_GetDayOfWeek);

    if((TM_Month == 1) || (TM_Month == 2)) //????????????
    {
        TM_Month += 12;
        TM_Year --;
    }

    WeekNUM = (UINT8)((TM_Day + 2 * TM_Month + 3 * (TM_Month + 1) / 5 + TM_Year + TM_Year / 4 - TM_Year / 100 + TM_Year / 400) % 7);
    CSW_PROFILE_FUNCTION_EXIT(TM_GetDayOfWeek);

    return WeekNUM;
}


/*
* Function name:    TM_GetDayOfWeekEX
* Description:
*                   Get week number int special day
* remark:
*    ???????????
*    expressions:
*       a=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
*     If a =0 ,Than the week number is Monday;
*       If a =1 ,Than the week number is Tuesday;
*       If a =2 ,Than the week number is Wednesday;
*       If a =3 ,Than the week number is Thursday;
*       If a =4 ,Than the week number is Friday;
*       If a =5 ,Than the week number is Saturday;
*       If a =6 ,Than the week number is Sunday;
* Parameters:
*                  TM_Year   Specifies the year.
*                     TM_Month  Specifies the month.
*                  TM_Day    Specifies the day.
* Return Values:
*                  The return values is the week number(describe as up)
*                  of specifies day
*/
UINT8 TM_GetDayOfWeekEX(TM_FILETIME FileTime)
{
    UINT8 WeekNUM = 7; // ???????????????
    TM_SYSTEMTIME Systemtime;

    CSW_PROFILE_FUNCTION_ENTER(TM_GetDayOfWeekEX);

    TM_FileTimeToSystemTime(FileTime, &Systemtime);

    if((Systemtime.uMonth == 1) || (Systemtime.uMonth == 2)) //????????????
    {
        Systemtime.uMonth += 12;
        Systemtime.uYear --;
    }

    WeekNUM = (UINT8)((Systemtime.uDay + 2 * Systemtime.uMonth + 3 * (Systemtime.uMonth + 1) / 5 + Systemtime.uYear + Systemtime.uYear / 4 - Systemtime.uYear / 100 + Systemtime.uYear / 400) % 7);
    CSW_PROFILE_FUNCTION_EXIT(TM_GetDayOfWeekEX);

    return WeekNUM;
}

/*
* Function name:    TM_IsValidTime
* Description:
*                   determines whether the Time is a Valid time
* Parameters:         TM_SystemTime
*                          Specifies the SystemTime.
* Return Values:
*                 TRUE indicates the specifies is a Valid time,
*                   FALSE indicates the specifies is not a Valid time
*
*/

PRIVATE BOOL TM_IsValidTime(CONST TM_SYSTEMTIME *pSysTime)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_IsValidTime);

    if(pSysTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if((pSysTime->uYear < TM_STARTYEAR ) || (pSysTime->uYear > TM_ENDYEAR))
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid Year failed----\n",0x0800012f));
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_NOT_VALID_YEAR_ERROR);
    }

    if((pSysTime->uMonth < 1) || (pSysTime->uMonth > 12))
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid Year  failed----\n",0x08000130));
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_NOT_VALID_MONTH_ERROR);
    }
    if(pSysTime->uDay < 1 || pSysTime->uDay > 31)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid failed----\n",0x08000131));
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_NOT_VALID_DAY_ERROR);
    }
    else
    {
        UINT8 DaysMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if(TM_IsLeapYear(pSysTime->uYear))
        {
            DaysMonth[1] = 29;
        }

        if(pSysTime->uDay > DaysMonth[pSysTime->uMonth - 1])
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid failed----\n",0x08000132));
            CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
            TIM_ERR_RETURN(TM_NOT_VALID_DAY_ERROR);
        }
    }
    if(pSysTime->uHour >= 24)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid failed----\n",0x08000133));
        TIM_ERR_RETURN(TM_NOT_VALID_HOUR_ERROR);
    }
    if(pSysTime->uMinute >= 60)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid failed----\n",0x08000134));
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_NOT_VALID_MINNUTE_ERROR);
    }
    if(pSysTime->uSecond >= 60)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====SUL_TM Time Valid failed----\n",0x08000135));
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);
        TIM_ERR_RETURN(TM_NOT_VALID_SECOND_ERROR);
    }
    CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("=====The time is valid----\n",0x08000136));

    CSW_PROFILE_FUNCTION_EXIT(TM_IsValidTime);

    return TRUE;
}

/*
*Description:
*  judge wether the time formate is valid
*Parameter:
*  uFormat[in]: the time format.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TM_IsValidFormat(UINT16 uFormat)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_IsValidFormat);

    if((!TIM_ChckFormat(0, 1, uFormat))
            || (!TIM_ChckFormat(2, 3, uFormat))
            || (!TIM_ChckFormat(4, 6, uFormat))
            || (!TIM_ChckFormat(7, 10, uFormat)))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_IsValidFormat);
        return FALSE;
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_IsValidFormat);

    return TRUE;
}

/*
* Function name:    TM_SystemTimeToFileTime
* Description:
*                   Convert SystemTime to FileTime
*
*/
BOOL TM_SystemTimeToFileTime (CONST TM_SYSTEMTIME *pSystemTime,
                              TM_FILETIME *pFileTime)
{

    UINT32 TotalDay_Tmp = 0, Year_TMP;

    CSW_PROFILE_FUNCTION_ENTER(TM_SystemTimeToFileTime);
    if(pSystemTime == NULL || pFileTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SystemTimeToFileTime);

        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }
    if(TRUE != TM_IsValidTime(pSystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SystemTimeToFileTime);
        return FALSE;
    }
    /**********************determines from 1970 to 2070;***************************/
    if(((pSystemTime->uYear) < TM_STARTYEAR ) || ((pSystemTime->uYear) > TM_ENDYEAR))
    {
        //SUL_RETURN(ERR_INVALID_PARAMETER,(UINT32)-1);
        CSW_PROFILE_FUNCTION_EXIT(TM_SystemTimeToFileTime);
        TIM_ERR_RETURN(TM_NOT_VALID_YEAR_ERROR);
    }
    //return ERR_INVALID_PARAMETER;

    for(Year_TMP = TM_STARTYEAR; Year_TMP < (pSystemTime->uYear); Year_TMP++)
    {
        if(TM_IsLeapYear((UINT16)Year_TMP))
        {
            TotalDay_Tmp += 366;
        }
        else
        {
            TotalDay_Tmp += 365;
        }
    }

    TotalDay_Tmp += TM_GetDayNoInYear(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay);
    TotalDay_Tmp --;
    pFileTime->DateTime = (TotalDay_Tmp * 24 * 60 * 60 + (pSystemTime->uHour) * 60 * 60 + (pSystemTime->uMinute) * 60 + (pSystemTime->uSecond));

    //SUL_RETURN(ERR_SUCCESS,ERR_SUCCESS);
    //return ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_EXIT(TM_SystemTimeToFileTime);
    return TRUE;
}

/*
* Function name:    TM_FileTimeToSystemTime
* Description:
*                   Convert FileTime to SystemTime
*
*/
BOOL TM_FileTimeToSystemTime (CONST TM_FILETIME nFileTime,
                              TM_SYSTEMTIME *pSystemTime)
{
    UINT32 MonthTmp = 1, DayTmp = 1;
    UINT32 TotalSeconds;
    UINT16 YearTmp = TM_STARTYEAR;
    UINT8  DaysMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    CSW_PROFILE_FUNCTION_ENTER(TM_FileTimeToSystemTime);

    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FileTimeToSystemTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    TotalSeconds = nFileTime.DateTime;
    if(TotalSeconds < 0)
    {
        //SUL_RETURN(ERR_INVALID_PARAMETER,(UINT32)-1);
        CSW_PROFILE_FUNCTION_EXIT(TM_FileTimeToSystemTime);
        return FALSE;
    }
    //return ERR_INVALID_PARAMETER;
    /***********************GET YEAR***********************************************/
    while(1)
    {
        if(TM_IsLeapYear((UINT16)YearTmp))
        {
            if(TotalSeconds >= 366 * 24 * 60 * 60 )
            {
                TotalSeconds -= 366 * 24 * 60 * 60;
                YearTmp++;
            }
            else
            {
                break;
            }

        }
        else
        {
            if(TotalSeconds >= 365 * 24 * 60 * 60 )
            {
                TotalSeconds -= 365 * 24 * 60 * 60;
                YearTmp++;
            }
            else
            {
                break;
            }
        }
    }
    pSystemTime->uYear = (UINT16)YearTmp;
    /*********************GET MONTH************************************************/
    if(TM_IsLeapYear((UINT16)YearTmp))
    {
        DaysMonth[1] = 29;

    }

    while(1)
    {
        if(TotalSeconds >= (UINT32)(DaysMonth[MonthTmp - 1] * 24 * 60 * 60))
        {
            TotalSeconds -= DaysMonth[MonthTmp - 1] * 24 * 60 * 60;
            MonthTmp++;
        }
        else
            break;
    }
    pSystemTime->uMonth = (UINT8)MonthTmp;
    /********************GET DAY***************************************************/
    while(1)
    {
        if(TotalSeconds >= 24 * 60 * 60)
        {
            TotalSeconds -= 24 * 60 * 60;
            DayTmp++;
        }
        else
        {
            break;
        }
    }
    pSystemTime->uDay = (UINT8)DayTmp;
    /********************GET HOUR**************************************************/
    pSystemTime->uHour = (UINT8)(TotalSeconds / (60 * 60));
    TotalSeconds %= 60 * 60;

    /********************GET MINUTE************************************************/
    pSystemTime->uMinute = (UINT8)(TotalSeconds / 60);
    TotalSeconds %= 60;

    /********************GET SECOND************************************************/
    pSystemTime->uSecond = (UINT8)TotalSeconds;

    /********************GET WEEKDAY***********************************************/
    /*                     Sunday is 0, Monday is 1, and so on.                   */
    if(TM_GetDayOfWeek(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay) == 6)
    {
        pSystemTime->uDayOfWeek = 0;
    }
    else
    {
        pSystemTime->uDayOfWeek = (UINT8)(TM_GetDayOfWeek(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay) + 1);
    }
    /******************************************************************************/

    pSystemTime->uMilliseconds = 0;
    //SUL_RETURN(ERR_SUCCESS,ERR_SUCCESS);
    CSW_PROFILE_FUNCTION_EXIT(TM_FileTimeToSystemTime);
    return TRUE;
    //return ERR_SUCCESS;
}

/*
* Function name:    TM_FormatSystemTime
* Description:
*                   Formate SystemTime for Output
*
*/
BOOL TM_FormatSystemTime(CONST TM_SYSTEMTIME *pSystemTime,
                         UINT16 uFormat,
                         RESID nResFormatID,
                         PSTR pDateString,
                         UINT8 uDateStringSize,
                         PSTR pTimeString,
                         UINT8 uTimeStringSize)
{
    TCHAR TimeFirstPart[4], TimeSecondPart[4], TimeThirdPart[4], Timeseparator[2];
    TCHAR DateFirstPart[8], DateSecondPart[8], DateThirdPart[8], Dateseparator[2];
    TCHAR URL_Array[][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"}; //for res file and it can be special as you like
    UINT32 Tmp = 0;

    CSW_PROFILE_FUNCTION_ENTER(TM_FormatSystemTime);
    if(pSystemTime == NULL  ||
            pDateString == NULL  ||
            pTimeString == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }
    SUL_ZeroMemory8(TimeFirstPart, 4);
    SUL_ZeroMemory8(TimeSecondPart, 4);
    SUL_ZeroMemory8(TimeThirdPart, 4);
    SUL_ZeroMemory8(Timeseparator, 2);
    SUL_ZeroMemory8(DateFirstPart, 8);
    SUL_ZeroMemory8(DateSecondPart, 8);
    SUL_ZeroMemory8(DateThirdPart, 8);
    SUL_ZeroMemory8(Dateseparator, 2);
    if(TRUE != TM_IsValidTime(pSystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
        return FALSE;
    }
    if(TRUE != TM_IsValidFormat(uFormat))
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_FormatSystemTime fuckly format ERR!!! \n",0x08000137));
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
        return FALSE;
    }
    if(nResFormatID == 0)
        ;//?????????
    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
        return FALSE;
    }

    //Tmp = pSystemTime->uDayOfWeek;

    Tmp = (UINT8)(TM_GetDayOfWeek(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay));

    if((pDateString != NULL) && (uDateStringSize != 0))
    {
#if 0
        if(uDateStringSize < (26 + sizeof(URL_Array[Tmp][10])))//?????????????
        {
            //SUL_RETURN(ERR_INVALID_PARAMETER,(UINT32)-1);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("=============uDateStringSize < 26 failed \n",0x08000138));
            return FALSE;
        }
#endif
        //return ERR_INVALID_PARAMETER;
        /******************************************************************************/
        /*                        ?11-13? ???????(".", ":", "/" ,"_")          */
        /******************************************************************************/
        if(uFormat & TM_FMT_DOT_DATE)
        {
            SUL_StrCopy(Dateseparator, (const TCHAR *)("."));
        }
        else if(uFormat & TM_FMT_COLON_DATE)
        {
            SUL_StrCopy(Dateseparator, (const TCHAR *)(":"));
        }
        else if(uFormat & TM_FMT_SLASH_DATE)
        {
            SUL_StrCopy(Dateseparator, (const TCHAR *)"/");
        }
        else if(uFormat & TM_FMT_BASELINE_DATE)
        {
            SUL_StrCopy(Dateseparator, (const TCHAR *)"-");
        }
        /******************************************************************************/
        /*        ?14-15? ??????(????????????          */
        /******************************************************************************/

        if(uFormat & TM_FMT_DMY_DATE)
        {
            if(pSystemTime->uDay < 10)
            {
                SUL_StrPrint(DateFirstPart,  (PCSTR)"0%d", (pSystemTime->uDay));
            }
            else
            {
                SUL_StrPrint(DateFirstPart,  (PCSTR)"%d", (pSystemTime->uDay));
            }
            if(pSystemTime->uMonth < 10)
            {
                SUL_StrPrint(DateSecondPart,  (PCSTR)"%d", (pSystemTime->uMonth));
            }
            else
            {
                SUL_StrPrint(DateSecondPart,  (PCSTR)"%d", (pSystemTime->uMonth));
            }

            SUL_StrPrint(DateThirdPart,  (PCSTR)"%d", (pSystemTime->uYear));
        }
        else if(uFormat & TM_FMT_MDY_DATE)
        {
            if(pSystemTime->uMonth < 10)
            {
                SUL_StrPrint(DateFirstPart,  (PCSTR)("0%d"), (pSystemTime->uMonth));
            }
            else
            {
                SUL_StrPrint(DateFirstPart,  (PCSTR)("%d"), (pSystemTime->uMonth));
            }
            if(pSystemTime->uDay < 10)
            {
                SUL_StrPrint(DateSecondPart, (PCSTR) "0%d", (pSystemTime->uDay));
            }
            else
            {
                SUL_StrPrint(DateSecondPart, (PCSTR) "%d", (pSystemTime->uDay));
            }

            SUL_StrPrint(DateThirdPart,  (PCSTR)"%d", (pSystemTime->uYear));
        }
        else if(uFormat & TM_FMT_YMD_DATE)
        {
            SUL_StrPrint(DateFirstPart,  (PCSTR)"%d", (pSystemTime->uYear));
            if(pSystemTime->uMonth < 10)
            {
                SUL_StrPrint(DateSecondPart,  (PCSTR)"0%d", (pSystemTime->uMonth));
            }
            else
            {
                SUL_StrPrint(DateSecondPart,  (PCSTR)"%d", (pSystemTime->uMonth));
            }
            if(pSystemTime->uDay < 10)
            {
                SUL_StrPrint(DateThirdPart,  (PCSTR)"0%d", (pSystemTime->uDay));
            }
            else
            {
                SUL_StrPrint(DateThirdPart,  (PCSTR)"%d", (pSystemTime->uDay));
            }

        }
        SUL_StrCopy(pDateString, DateFirstPart);
        SUL_StrCat(pDateString, Dateseparator);
        SUL_StrCat(pDateString, DateSecondPart);
        SUL_StrCat(pDateString, Dateseparator);
        SUL_StrCat(pDateString, DateThirdPart);
        SUL_StrCat(pDateString, (const TCHAR *)" ");
        SUL_StrCat(pDateString, URL_Array[Tmp]);
    }

    if((pTimeString != NULL) && (uTimeStringSize != 0))
    {
        if(uDateStringSize < 14)//?????????????
        {
            //SUL_RETURN(ERR_INVALID_PARAMETER,(UINT32)-1);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("=============Because uDateStringSize < 14 failed \n",0x08000139));
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
            return FALSE;
        }

        //return ERR_INVALID_PARAMETER;
        /******************************************************************************/
        /*                        ?9-10? ???????(./:)                          */
        /******************************************************************************/
        if(uFormat & TM_FMT_DOT_TIME)
        {
            SUL_StrCopy(Timeseparator, (const TCHAR *)("."));
        }
        else if(uFormat & TM_FMT_COLON_TIME)
        {
            SUL_StrCopy(Timeseparator, (const TCHAR *)(":"));
        }
        /******************************************************************************/
        /*        ?16? ??????(?2??/24???                                */
        /******************************************************************************/
        if(uFormat & TM_FMT_TWELVE_TIME)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_FormatSystemTime TM_FMT_TWELVE_TIME \n",0x0800013a));

            if(pSystemTime->uHour >= 12)
            {
                SUL_StrCopy(TimeThirdPart, (const TCHAR *)(TEXT("PM")));
            }
            else
            {
                SUL_StrCopy(TimeThirdPart, (const TCHAR *)(TEXT("AM")));
            }

#if 1
            if(pSystemTime->uHour < 10)
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"0%d", pSystemTime->uHour);
            }
            else if(pSystemTime->uHour >= 10 && pSystemTime->uHour <= 12)
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"%d", pSystemTime->uHour);
            }
            else if(pSystemTime->uHour > 12 && pSystemTime->uHour < 22)
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"0%d", pSystemTime->uHour - 12);
            }
            else
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"%d", pSystemTime->uHour - 12);
            }
#endif
        }
        else if(uFormat & TM_FMT_TWENTYFOUR_TIME)
        {
            SUL_StrCopy(TimeThirdPart, (const TCHAR *)(""));
#if 1
            if(pSystemTime->uHour < 10)
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"0%d", pSystemTime->uHour);
            }
            else
            {
                SUL_StrPrint(TimeFirstPart, (PCSTR)"%d", pSystemTime->uHour);
            }
#endif
        }

        if( pSystemTime->uMinute < 10)
        {
            SUL_StrPrint(TimeSecondPart, (PCSTR) "0%d", pSystemTime->uMinute);
        }
        else
        {
            SUL_StrPrint(TimeSecondPart, (PCSTR) "%d", pSystemTime->uMinute);
        }
#if 1
        SUL_StrCopy(pTimeString, TimeFirstPart);
        SUL_StrCat(pTimeString, Timeseparator);
        SUL_StrCat(pTimeString, TimeSecondPart);
        SUL_StrCat(pTimeString, (const TCHAR *)" ");
        SUL_StrCat(pTimeString, TimeThirdPart);
#endif
    }
    if((pTimeString == NULL) && (uTimeStringSize == 0) && (pDateString == NULL) && (uDateStringSize == 0))
    {
        //SUL_RETURN(ERR_INVALID_PARAMETER,(UINT32)-1);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("=============Because pTimeString == NULL .... failed \n",0x0800013b));
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
        return FALSE;
    }
    //return ERR_INVALID_PARAMETER;
    //SUL_RETURN(ERR_SUCCESS,ERR_SUCCESS);
    CSW_PROFILE_FUNCTION_EXIT(TM_FormatSystemTime);
    return TRUE;
}


/*
* Function name:    TM_FormatFileTime
* Description:
*                   Formate Filetime for Output
*
*/
BOOL TM_FormatFileTime(CONST TM_FILETIME nFileTime,
                       UINT16 uFormat,
                       RESID nResFormatID,
                       PSTR pDateString,
                       UINT8 uDateStringSize,
                       PSTR pTimeString,
                       UINT8 uTimeStringSize)
{

    TM_SYSTEMTIME SystemTime;
    TM_SYSTEMTIME *pSystemTime;
    BOOL RET;
    CSW_PROFILE_FUNCTION_ENTER(TM_FormatFileTime);

    if(pDateString == NULL ||
            pTimeString == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }
    pSystemTime = &SystemTime;
    RET = TM_FileTimeToSystemTime (nFileTime, pSystemTime);
    if(TRUE != RET)
    {
        CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_FileTimeToSystemTime failed \n",0x0800013c));
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTime);
        return RET;
    }
    RET = TM_FormatSystemTime(pSystemTime, uFormat, nResFormatID, pDateString, uDateStringSize, pTimeString, uTimeStringSize);
    if(TRUE != RET)
    {
        CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_FormatSystemTime failed \n",0x0800013d));
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTime);
        return RET;
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTime);
    return TRUE;
}

/*
* Function name:    TM_GetTimeZone
* Description:
*                   Get the current time zone.
* Parameters
*                   NONE
*/
INT8 TM_GetTimeZone(VOID)
{

    TM_PARAMETERS TM_Parameters;

    CSW_PROFILE_FUNCTION_ENTER(TM_GetTimeZone);
    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetTimeZone);
        return FALSE;
    }
    CSW_PROFILE_FUNCTION_EXIT(TM_GetTimeZone);

    return(TM_Parameters.iZone);

}

/*
* Function name:    TM_SetTimeZone
* Description:
*                   Set the timezone
* Parameters
*      TimeZone     The timezone having a reference to GMT and localtime
*                        to be set
*/
BOOL TM_SetTimeZone(INT8 TimeZone)
{
    TM_PARAMETERS TM_Parameters, paraBak;

    CSW_PROFILE_FUNCTION_ENTER(TM_SetTimeZone);
    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeZone);
        return FALSE;
    }
    paraBak = TM_Parameters;
    TM_Parameters.iZone = TimeZone;

    if(!TIM_SetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeZone);
        return FALSE;
    }

#ifdef TIM_REG_FFS_USED
    if(NV_SetTmZone(&TimeZone,sizeof(INT8)))
    {
        TIM_SetTimePara(&paraBak);
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeZone);
        return FALSE;
    }
#else
    if(!TIM_WriteFlash())
    {
        TIM_SetTimePara(&paraBak);
        CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeZone);
        return FALSE;
    }
#endif
    CSW_PROFILE_FUNCTION_EXIT(TM_SetTimeZone);
    return TRUE;
}

/*
* Function name:    TM_GetTime
* Description:
*                   Get the current  time.
*/
UINT32 TM_GetTime (VOID)
{

    //modify at 2005-11-14
    //tmp = (1000 * hal_timRTC_GetTime ())/ DDI_CLOCKS_PER_SECOND;
    //
    //modify at 2006-05-14 by lxp
    //For the Alarm function's release
    UINT32  RET;
    TM_FILETIME FileTime;

    if(!TM_GetSystemFileTime (&FileTime) )
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetTime);
        return FALSE;
    }

    // TM_GetTime seems to be expected to return time in 256Hz
    //modify by Frank
    //RET = FileTime.DateTime*256;
    RET = FileTime.DateTime;

    CSW_PROFILE_FUNCTION_EXIT(TM_GetTime);
    return(RET);
}

/*
* Function name:    TM_GetSystemTime
* Description:
*                   Get the current system date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/

UINT32 TM_PowerOnPatch();

BOOL TM_GetSystemTime(TM_SYSTEMTIME *pSystemTime)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_GetSystemTime);

    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    BOOL RET = FALSE;

    HAL_TIM_RTC_TIME_T RtcTime;

    if(hal_TimRtcGetTime(&RtcTime) != HAL_ERR_NO)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("hal_TimRtcGetTime Error\n",0x0800013e));
        CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
        return FALSE;
    }

    pSystemTime->uYear         = ((UINT16)RtcTime.year) + 2000;
    pSystemTime->uMonth        = RtcTime.month;
    pSystemTime->uDayOfWeek    = RtcTime.wDay;
    pSystemTime->uDay          = RtcTime.day;
    pSystemTime->uHour         = RtcTime.hour;
    pSystemTime->uMinute       = RtcTime.min;
    pSystemTime->uSecond       = RtcTime.sec;

    // No information on the milliseconds is provided. Always set to 0.
    pSystemTime->uMilliseconds = 0;

    if( TRUE != TM_IsValidTime(pSystemTime) )
    {
        pSystemTime->uSecond  = RtcTime.sec   = TM_DEFAULT_TIME_SECOND;
        pSystemTime->uMinute  = RtcTime.min   = TM_DEFAULT_TIME_MINUTE;
        pSystemTime->uHour    = RtcTime.hour  = TM_DEFAULT_TIME_HOUR;
        pSystemTime->uDay     = RtcTime.day   = TM_DEFAULT_DATE_DAY;
        pSystemTime->uMonth   = RtcTime.month = TM_DEFAULT_DATE_MONTH;
        pSystemTime->uDayOfWeek = RtcTime.wDay  = TM_DEFAULT_DATE_WEEKDAY; // It's a Sunday.
        RtcTime.year  = TM_DEFAULT_DATE_YEAR-2000; //Change default year is 2017.
        pSystemTime->uYear = TM_DEFAULT_DATE_YEAR;
        if(hal_TimRtcSetTime(&RtcTime) != HAL_ERR_NO)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("hal_TimRtcSetTime Error\n",0x0800013f));

            CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
            return FALSE;
        }
    }

    TM_FILETIME nFileTime;
    RET = TM_SystemTimeToFileTime(pSystemTime, &nFileTime);
    if(TRUE != RET)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_SystemTimeToFileTime Error \n",0x08000140));
        return FALSE;
    }

    if(gCSWPowerOnTime && ((nFileTime.DateTime - gCSWPowerOnTime) > 2 * 60 * 60))
    {
        UINT32 nRet = CFW_CfgSetTimeParam(&nFileTime);
        if( ERR_SUCCESS != nRet )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetTimeParam Error 0x%x\n",0x08000141), nRet);
            return FALSE;
        }
        gCSWPowerOnTime = nFileTime.DateTime;
    }
    else if(gCSWPowerOnTime == 0)
    {
        gCSWPowerOnTime = nFileTime.DateTime;
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("nFileTime - gCSWPowerOnTime %d\n",0x08000142), (nFileTime.DateTime - gCSWPowerOnTime));
    }
    CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_GetSystemTime exit\n",0x08000143));
    CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);

    return(TRUE);
}


/*
* Function name:    TM_GetSystemFileTime
* Description:
*                   Get the current system date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/
BOOL TM_GetSystemFileTime (
    TM_FILETIME *pFileTime
)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_GetSystemFileTime);

    if(pFileTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemFileTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    HAL_TIM_RTC_TIME_T RtcTime;
    TM_SYSTEMTIME  SystemTime;

    if(hal_TimRtcGetTime(&RtcTime) != HAL_ERR_NO)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
        return FALSE;
    }

    SystemTime.uYear         = ((UINT16)RtcTime.year) + 2000;
    SystemTime.uMonth        = RtcTime.month;
    SystemTime.uDayOfWeek    = RtcTime.wDay;
    SystemTime.uDay          = RtcTime.day;
    SystemTime.uHour         = RtcTime.hour;
    SystemTime.uMinute       = RtcTime.min;
    SystemTime.uSecond       = RtcTime.sec;

    if( TRUE != TM_IsValidTime(&SystemTime) )
    {
        SystemTime.uSecond  = RtcTime.sec   = TM_DEFAULT_TIME_SECOND;
        SystemTime.uMinute  = RtcTime.min   = TM_DEFAULT_TIME_MINUTE;
        SystemTime.uHour    = RtcTime.hour  = TM_DEFAULT_TIME_HOUR;
        SystemTime.uDay     = RtcTime.day   = TM_DEFAULT_DATE_DAY;
        SystemTime.uMonth   = RtcTime.month = TM_DEFAULT_DATE_MONTH;
        SystemTime.uDayOfWeek = RtcTime.wDay  = TM_DEFAULT_DATE_WEEKDAY; // It's a Sunday.
        RtcTime.year  = TM_DEFAULT_DATE_YEAR-2000; //Change default year is 2017.
        SystemTime.uYear = TM_DEFAULT_DATE_YEAR;
        if(hal_TimRtcSetTime(&RtcTime) != HAL_ERR_NO)
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
            return FALSE;
        }
    }

    if(!TM_SystemTimeToFileTime (&SystemTime, pFileTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemTime);
        return FALSE;
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_GetSystemFileTime);
    return(TRUE);
}


//I think we have a hardware BUG, when pulling the battery, the valid bit of the RTC register may be an invalid value, leading to obtain the system time is incorrect.
//We will fix it by software, when the phone boot up, and the time stored in flash is greater than the hardware RTC time, we will restore time to the default settings,
//The same as the hardware time is more than two days for flash time to restore the default settings.

UINT32 TM_PowerOnPatch()
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    if (hal_SysGetChipId(HAL_SYS_CHIP_PROD_ID) != HAL_SYS_CHIP_PROD_ID_8808)
    {
        // The h/w issue has been fixed since 8808S
        return ERR_SUCCESS;
    }
#endif

    TM_FILETIME nHWFileTime;
    TM_FILETIME nSWFileTime;
    BOOL nTimeValidFlag = FALSE;
    UINT32 nRet = 0x00;
    BOOL bRet = TM_GetSystemFileTime(&nHWFileTime);
    if( ERR_SUCCESS == bRet )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_GetSystemFileTime Success\n",0x08000144));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("TM_GetSystemFileTime Error nRet 0x%x\n",0x08000145), nRet );
    }

    nRet = CFW_CfgGetTimeParam(&nSWFileTime);
    if( ERR_SUCCESS == nRet )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CfgGetTimeParam Success\n",0x08000146));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetTimeParam nRet 0x%x\n",0x08000147), nRet );
    }

    if( nSWFileTime.DateTime > nHWFileTime.DateTime )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("nSWFileTime > nHWFileTime\n",0x08000148));
        nTimeValidFlag = FALSE;
    }
    else if(( nHWFileTime.DateTime - nSWFileTime.DateTime ) > ( 2 * 24 * 60 * 60 ))
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("( nHWFileTime - nSWFileTime ) > ( 2*24*60*60 )\n",0x08000149));
        nTimeValidFlag = FALSE;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("nTimeValidFlag = TRUE\n",0x0800014a));
        nTimeValidFlag = TRUE;
    }

    if(!nTimeValidFlag)
    {
        TM_SYSTEMTIME nSystemTime;
        HAL_TIM_RTC_TIME_T RtcTime;
        nSystemTime.uSecond  = RtcTime.sec   = TM_DEFAULT_TIME_SECOND;
        nSystemTime.uMinute  = RtcTime.min   = TM_DEFAULT_TIME_MINUTE;
        nSystemTime.uHour    = RtcTime.hour  = TM_DEFAULT_TIME_HOUR;
        nSystemTime.uDay     = RtcTime.day   = TM_DEFAULT_DATE_DAY;
        nSystemTime.uMonth   = RtcTime.month = TM_DEFAULT_DATE_MONTH;
        nSystemTime.uDayOfWeek = RtcTime.wDay  = TM_DEFAULT_DATE_WEEKDAY; // It's a Sunday.
        RtcTime.year  = TM_DEFAULT_DATE_YEAR-2000; //Change default year is 2017.
        nSystemTime.uYear = TM_DEFAULT_DATE_YEAR;
        if(hal_TimRtcSetTime(&RtcTime) != HAL_ERR_NO)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("hal_TimRtcSetTime Error\n",0x0800014b));
            return ERR_INVALID_HANDLE;
        }

        nRet = TM_SetSystemTime(&nSystemTime);
        if( ERR_SUCCESS != nRet )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_SetSystemTime Error\n",0x0800014c));
            return ERR_INVALID_HANDLE;
        }

        TM_FILETIME nFileTime;
        if(TRUE != TM_SystemTimeToFileTime(&nSystemTime, &nFileTime))
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_SystemTimeToFileTime Error\n",0x0800014d));
            CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
            return FALSE;
        }

        nRet = CFW_CfgSetTimeParam(&nFileTime);
        if( ERR_SUCCESS != nRet )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetTimeParam Error 0x%x\n",0x0800014e), nRet);
            return ERR_INVALID_HANDLE;
        }
    }
    else
    {
        nRet = CFW_CfgSetTimeParam(&nHWFileTime);
        if( ERR_SUCCESS != nRet )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetTimeParam Error 0x%x\n",0x0800014f), nRet);
            return ERR_INVALID_HANDLE;
        }
    }

    gCSWPowerOnTime = nHWFileTime.DateTime;
#endif // gallite or 8808

    return ERR_SUCCESS;
}


/*
* Function name:    TM_SetSystemTime
* Description:
*                   Set the current system date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/


BOOL TM_SetSystemTime(TM_SYSTEMTIME *pSystemTime)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_SetSystemTime);
    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(TRUE != TM_IsValidTime(pSystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
        return FALSE;
    }

    TM_FILETIME nFileTime;
    TM_SystemTimeToFileTime(pSystemTime, &nFileTime);

    UINT32 nRet = CFW_CfgSetTimeParam(&nFileTime);
    if( ERR_SUCCESS == nRet )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CfgSetTimeParam Success\n",0x08000150));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CfgSetTimeParam nRet 0x%x\n",0x08000151), nRet );
    }

    // Set a proper day of week, ignoring the fied in the pSystemTime structure.
    // Sunday is 0, Monday is 1, and so on, when calling TM_GetDayOfWeek.
    if(TM_GetDayOfWeek(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay) == 6)
    {
        pSystemTime->uDayOfWeek = 0;
    }
    else
    {
        pSystemTime->uDayOfWeek = (UINT8)(TM_GetDayOfWeek(pSystemTime->uYear, pSystemTime->uMonth, pSystemTime->uDay) + 1);
    }

    if(pSystemTime->uYear < 2000)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    HAL_TIM_RTC_TIME_T RtcTime;

    RtcTime.year  = (UINT8) (pSystemTime->uYear - 2000);
    RtcTime.month = pSystemTime->uMonth ;
    RtcTime.wDay  = pSystemTime->uDayOfWeek;
    RtcTime.day   = pSystemTime->uDay ;
    RtcTime.hour  = pSystemTime->uHour;
    RtcTime.min   = pSystemTime->uMinute ;
    RtcTime.sec   = pSystemTime->uSecond;

    if  (hal_TimRtcSetTime (&RtcTime) != HAL_ERR_NO)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("hal_TimRtcSetTime Error\n",0x08000152));
        CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
        return FALSE;
    }

    CSW_TRACE(_CSW|TLEVEL(BASE_TM_TS_ID)|TDB|TNB_ARG(0), TSTR("TM_SetSystemTime OK\n",0x08000153));
    CSW_PROFILE_FUNCTION_EXIT(TM_SetSystemTime);
    return(TRUE);
}


/*
* Function name:    TM_GetLocalTime
* Description:
*                   Set the current system date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/
BOOL TM_GetLocalTime (
    TM_SYSTEMTIME *pSystemTime
)
{
    TM_FILETIME     nFileTime;
    TM_PARAMETERS   TM_Parameters;

    CSW_PROFILE_FUNCTION_ENTER(TM_GetLocalTime);

    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetLocalTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetLocalTime);
        return FALSE;
    }

    if(!TM_GetSystemFileTime(&nFileTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetLocalTime);
        return FALSE;
    }

    nFileTime.DateTime = nFileTime.DateTime + TM_Parameters.iZone * 60 * 60;

    if(TRUE != TM_FileTimeToSystemTime(nFileTime, pSystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetLocalTime);
        return FALSE;
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_GetLocalTime);
    return(TRUE);
}


/*
* Function name:    TM_SetLocalTime
* Description:
*                   Set the current system date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/
BOOL TM_SetLocalTime(
    TM_SYSTEMTIME *pSystemTime
)
{

    TM_FILETIME     nFileTime;
    TM_PARAMETERS   TM_Parameters;
    CSW_PROFILE_FUNCTION_ENTER(TM_SetLocalTime);
    TM_SYSTEMTIME SystemTime;

    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }
    if(TRUE != TM_IsValidTime(pSystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        return FALSE;
    }

    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        return FALSE;
    }

    if(!TM_SystemTimeToFileTime(pSystemTime, &nFileTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        return FALSE;
    }

    nFileTime.DateTime = nFileTime.DateTime - TM_Parameters.iZone * 60 * 60;
    if(!TM_FileTimeToSystemTime(nFileTime, &SystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        return FALSE;
    }

    if(!TM_SetSystemTime(&SystemTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
        return FALSE;
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_SetLocalTime);
    return(TRUE);
}

/******************************************************************************/
/* Function name:    TM_FormatDateTime                                    */
/* Description:                                                               */
/*                   Formate string to file time                               */
/*                                                                            */
/******************************************************************************/
BOOL TM_FormatDateTime(
    TM_FILETIME *pFileTime,
    UINT16 uFormat,
    RESID nResFormatID,
    PSTR pDateString,
    UINT8 uDateStringSize,
    PSTR pTimeString,
    UINT8 uTimeStringSize
)
{
    UINT32 TotalSecond, TimeTmp, YearTmp, MonthTmp;
    UINT8  Dateseparator, Timeseparator, YearLog, SecondLog, Tmp[5], *TDate, *TTime;
    //UINT16 LeapMonth[12],NLeapMonth[12];
    UINT16 LeapMonth[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    UINT16 NLeapMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(pDateString  == NULL ||
            pTimeString  == NULL ||
            pFileTime    == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    CSW_PROFILE_FUNCTION_ENTER(TM_FormatDateTime);
    YearLog   = 0;
    SecondLog = 0;
    TotalSecond = 0;
    TimeTmp  = 0;
    YearTmp  = 0;
    MonthTmp = 0;
    TDate = pDateString;
    TTime = pTimeString;
    //    SUL_ZeroMemory8(Dateseparator,2);
    //    SUL_ZeroMemory8(Timeseparator,2);
    SUL_ZeroMemory8(Tmp, 5);
    if(uDateStringSize == 10)
    {
        YearLog = 0;
    }
    else if(uDateStringSize == 8)
    {
        YearLog = 1;
    }
    if(uTimeStringSize == 8)
    {
        SecondLog = 0;
    }
    else if(uTimeStringSize == 5)
    {
        SecondLog = 1;
    }
    if((uDateStringSize == 0) || (uTimeStringSize == 0))
    {
        return(FALSE);
    }
    if(nResFormatID == 0)
        ;
    //TODO..
    if(TRUE != TM_IsValidFormat(uFormat))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return FALSE;
    }
#if 0
    if(uFormat == 0xffff)
    {
        uFormat = TM_FMT_YMD_DATE | TM_FMT_SLASH_DATE | TM_FMT_COLON_TIME;
    }
#endif

    //Firstly, dispose the date
    if(uFormat & TM_FMT_DOT_DATE)
    {
        Dateseparator = '.';
    }
    else if(uFormat & TM_FMT_COLON_DATE)
    {
        Dateseparator = ':';
    }
    else if(uFormat & TM_FMT_SLASH_DATE)
    {
        Dateseparator = '/';
    }
    else if(uFormat & TM_FMT_BASELINE_DATE)
    {
        Dateseparator = '-';
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return(FALSE);
    }
    //TRACE1("[FW_API_A | TM]The time Separator is %c",Dateseparator);
    //Secondly, dispose the time
    if(uFormat & TM_FMT_DOT_TIME)
    {
        Timeseparator = '.';
    }
    else if(uFormat & TM_FMT_COLON_TIME)
    {
        Timeseparator = ':';
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return(FALSE);
    }
    //TRACE1("[FW_API_A | TM]The time Separator is %c",Timeseparator);

    //Third, dispose the place of date.
    if(uFormat & TM_FMT_YMD_DATE)
    {
        if(YearLog == 0)//2005/04/13
        {
            SUL_StrNCopy(Tmp, TDate, 4);
            TimeTmp = SUL_StrAToI(Tmp);
            if((TimeTmp < TM_STARTYEAR) || (TimeTmp > TM_ENDYEAR))
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return(FALSE);
            }

            while(YearTmp < (TimeTmp - TM_STARTYEAR))
            {
                if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
                {
                    TotalSecond = TotalSecond + 366 * 24 * 60 * 60;
                }
                else
                {
                    TotalSecond = TotalSecond + 365 * 24 * 60 * 60;
                }
                YearTmp++;
            }

            SUL_ZeroMemory8(Tmp, 5);
            TDate = TDate + 5;
            SUL_StrNCopy(Tmp, TDate, 2);
            TimeTmp = SUL_StrAToI(Tmp);
            MonthTmp = 0;

            if((TimeTmp < 1) || (TimeTmp > 12))
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return(FALSE);
            }
            if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
            {
                while(MonthTmp < (TimeTmp - 1))
                {
                    TotalSecond = TotalSecond + LeapMonth[MonthTmp] * 24 * 60 * 60;
                    MonthTmp ++;
                }
            }
            else
            {
                while(MonthTmp < (TimeTmp - 1))
                {
                    TotalSecond = TotalSecond + NLeapMonth[MonthTmp] * 24 * 60 * 60;
                    MonthTmp++;
                }

            }

            SUL_ZeroMemory8(Tmp, 5);
            TDate = TDate + 3;
            SUL_StrNCopy(Tmp, TDate, 2);
            TimeTmp = SUL_StrAToI(Tmp);

            //
            //
            if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
            {
                if((TimeTmp <= 0) || TimeTmp > LeapMonth[MonthTmp])
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return FALSE;
                }
            }
            else
            {
                if((TimeTmp <= 0) || TimeTmp > NLeapMonth[MonthTmp])
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return FALSE;
                }
            }
            TotalSecond = TotalSecond + (TimeTmp - 1) * 24 * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            SUL_StrNCopy(Tmp, TTime, 2);
            TimeTmp = SUL_StrAToI(Tmp);

            if((TimeTmp < 0) || TimeTmp >= 24)
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return FALSE;
            }
            TotalSecond = TotalSecond + TimeTmp * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            TTime = TTime + 3;
            SUL_StrNCopy(Tmp, TTime, 2);
            TimeTmp = SUL_StrAToI(Tmp);
            if((TimeTmp < 0) || TimeTmp >= 60)
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return FALSE;
            }
            TotalSecond = TotalSecond + TimeTmp * 60;

            if(SecondLog == 0)
            {
                SUL_ZeroMemory8(Tmp, 5);
                TTime = TTime + 3;
                SUL_StrNCopy(Tmp, TTime, 2);
                TimeTmp = SUL_StrAToI(Tmp);
                if((TimeTmp < 1) || (TimeTmp > 60))
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return(FALSE);
                }
                TotalSecond = TotalSecond + TimeTmp;
            }
            pFileTime->DateTime = TotalSecond;
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
            return(TRUE);
        }
        else//05/04/13
        {
            SUL_StrNCopy(Tmp, pDateString, 2);
            TimeTmp = SUL_StrAToI(Tmp);

            YearTmp = 0;

            while(YearTmp < (TimeTmp))
            {
                if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
                {
                    TotalSecond = TotalSecond + 366 * 24 * 60 * 60;
                }
                else
                {
                    TotalSecond = TotalSecond + 365 * 24 * 60 * 60;
                }
                YearTmp++;
            }

            //TotalSecond = TotalSecond + (30 + TimeTmp)* 365 * 24 * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            TDate = TDate + 3;
            SUL_StrNCopy(Tmp, TDate, 2);
            TimeTmp = SUL_StrAToI(Tmp);
            if((TimeTmp < 1) || (TimeTmp > 12))
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return(FALSE);
            }
            MonthTmp = 0;

            if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
            {
                while(MonthTmp < (TimeTmp - 1))
                {
                    TotalSecond = TotalSecond + LeapMonth[MonthTmp] * 24 * 60 * 60;
                    MonthTmp ++;
                }
            }
            else
            {
                while(MonthTmp < (TimeTmp - 1))
                {
                    TotalSecond = TotalSecond + NLeapMonth[MonthTmp] * 24 * 60 * 60;
                    MonthTmp ++;
                }

            }
            //TotalSecond = TotalSecond + (TimeTmp - 1)* 30 * 24 * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            TDate = TDate + 3;
            SUL_StrNCopy(Tmp, TDate, 2);
            TimeTmp = SUL_StrAToI(Tmp);

            if(TM_IsLeapYear((UINT16)(YearTmp + TM_STARTYEAR)))
            {
                if((TimeTmp <= 0) || TimeTmp > LeapMonth[MonthTmp])
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return(FALSE);
                }
            }
            else
            {
                if((TimeTmp <= 0) || TimeTmp > NLeapMonth[MonthTmp])
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return(FALSE);
                }
            }
            TotalSecond = TotalSecond + (TimeTmp - 1) * 24 * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            SUL_StrNCopy(Tmp, TTime, 2);
            TimeTmp = SUL_StrAToI(Tmp);
            if((TimeTmp < 0) || (TimeTmp >= 24))
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return(FALSE);
            }
            TotalSecond = TotalSecond + TimeTmp * 60 * 60;

            SUL_ZeroMemory8(Tmp, 5);
            TTime = TTime + 3;
            SUL_StrNCopy(Tmp, TTime, 2);
            TimeTmp = SUL_StrAToI(Tmp);

            if((TimeTmp < 0) || (TimeTmp >= 60))
            {
                CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                return(FALSE);
            }
            TotalSecond = TotalSecond + TimeTmp * 60;

            if(SecondLog == 0)
            {
                SUL_ZeroMemory8(Tmp, 5);
                TTime = TTime + 3;
                SUL_StrNCopy(Tmp, TTime, 2);
                TimeTmp = SUL_StrAToI(Tmp);
                if((TimeTmp < 0) || (TimeTmp >= 60))
                {
                    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
                    return(FALSE);
                }
                TotalSecond = TotalSecond + TimeTmp;
            }

            pFileTime->DateTime = TotalSecond;
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
            return(TRUE);
        }
    }
    else if(uFormat & TM_FMT_MDY_DATE)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return(FALSE);
    }
    else if(uFormat & TM_FMT_DMY_DATE)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return(FALSE);
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
        return(FALSE);
    }

    CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTime);
    return(TRUE);
}

/*
* Function name:    TM_FormatDateTimeEx
* Description:
*                   Formate string to file time
*
*/
BOOL TM_FormatDateTimeEx(
    TM_FILETIME *pFileTime,
    PSTR pString
)
{
    UINT8 DateString[15], TimeString[15], *pTmp;

    CSW_PROFILE_FUNCTION_ENTER(TM_FormatDateTimeEx);

    if(pFileTime == NULL || pString == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }


    UINT8 len = SUL_Strlen(pString);
    SUL_ZeroMemory8(DateString, 15);
    SUL_ZeroMemory8(TimeString, 15);
    pTmp = pString;

    if(len < 14 || len > 19)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
        return FALSE;
    }
    //"yy/mm/dd,hh:mm"
    if(len == 14 || len == 16)
    {
        SUL_StrNCopy(DateString, pTmp, len - 6);
        pTmp = pTmp + len - 5;
        SUL_StrNCopy(TimeString, pTmp, 5);
        if(TM_FormatDateTime(pFileTime, TM_FMT_COLON_TIME | TM_FMT_TWENTYFOUR_TIME | TM_FMT_YMD_DATE | TM_FMT_SLASH_DATE,
                             (RESID)0, DateString, (UINT8)SUL_Strlen(DateString), TimeString, (UINT8)SUL_Strlen(TimeString)))
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
            return(TRUE);
        }
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
            return(FALSE);
        }
    }
    //"yy/mm/dd,hh:mm:ss"
    else if(len == 17 || len == 19)
    {
        SUL_StrNCopy(DateString, pTmp, len - 9);
        pTmp = pTmp + len - 8;
        SUL_StrNCopy(TimeString, pTmp, 8);
        if(TM_FormatDateTime(pFileTime, TM_FMT_COLON_TIME | TM_FMT_TWENTYFOUR_TIME | TM_FMT_YMD_DATE | TM_FMT_SLASH_DATE,
                             (RESID)0, DateString, (UINT8)SUL_Strlen(DateString), TimeString, (UINT8)SUL_Strlen(TimeString)))
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
            return(TRUE);
        }
        else
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
            return(FALSE);
        }
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatDateTimeEx);
        return(FALSE);
    }
}

/*
* Function name:    TM_FormatFileTimeToStringEx
* Description:
*                   Formate  file time to string
*
*/
BOOL TM_FormatFileTimeToStringEx(
    TM_FILETIME FileTime,
    UINT8 pString[18]
)
{
    UINT32 Tmp;
    UINT8 cDateString[50];
    UINT8 cTimeString[50], cSecond[2];
    UINT8 *pTmp;
    UINT32 err = 0;

    CSW_PROFILE_FUNCTION_ENTER(TM_FormatFileTimeToStringEx);
    //
    //Modify by Zhangcl at 20080809
    //
    SUL_ZeroMemory8(pString, 18);
    //
    //Modify end
    //
    SUL_ZeroMemory8(cDateString, 50);
    SUL_ZeroMemory8(cTimeString, 50);
    SUL_ZeroMemory8(cSecond, 2);
    err = TM_FormatFileTime(FileTime, TM_FMT_YMD_DATE | TM_FMT_SLASH_DATE | TM_FMT_TWENTYFOUR_TIME | TM_FMT_COLON_TIME, 0, cDateString, 50, cTimeString, 50);
    if(err)
    {
        pTmp = cDateString;
        pTmp = pTmp + 2;
        SUL_StrNCopy(pString, pTmp, 8);
        SUL_StrCat(pString, ",");
        SUL_StrNCat(pString, cTimeString, 15);
        Tmp = FileTime.DateTime;
        Tmp = Tmp % 60;
        if(Tmp >= 10)
        {
            SUL_StrPrint(cSecond, "%d", Tmp);
        }
        else
        {
            SUL_StrPrint(cSecond, "0%d", Tmp);
        }
        /*
                for(pTmp = pString;*pTmp!=(UINT8)'/0';pTmp++)
                {
                    ;
                }
                        *pTmp = ':';*/
        SUL_StrCat(pString, ":");
        SUL_StrCat(pString, cSecond);
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTimeToStringEx);
        return(TRUE);
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_FormatFileTimeToStringEx);
        return(FALSE);
    }
}


/*
* Function name:    TM_GetTimeZoneTime
* Description:
*                   Set the current Time Zone date and time.
* Parameters
*      PS_TM_SystemTime  Pointer to a TM_SYSTEMTIME structure to receive the
*                          current system date and time.
*/
UINT32 TM_GetTimeZoneTime (
    INT8  nTimeZone,
    TM_SYSTEMTIME *pSystemTime
)
{
    TM_PARAMETERS TM_Parameters;

    CSW_PROFILE_FUNCTION_ENTER(TM_GetTimeZoneTime);

    if(pSystemTime == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetTimeZoneTime);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(!TIM_GetTimePara(&TM_Parameters))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetTimeZoneTime);
        return FALSE;
    }

    TM_FILETIME nFileTime;

    if(!TM_GetSystemFileTime(&nFileTime))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetTimeZoneTime);
        return FALSE;
    }

    nFileTime.DateTime = nFileTime.DateTime + TM_Parameters.iZone * 60 * 60;

    return (nFileTime.DateTime);
}



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
PUBLIC UINT32 csw_TMGetTick(VOID)
{
    return hal_TimGetUpTime();
}

/*
*Description:
*  write the data to register flash
*Parameters:
*  pText[in]: the value name.
*  dataBuf[in]:the pointer to the data buffer to write
*  dataLen[in]:the data length of the buffer
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
#if 0
BOOL TIM_WriteRegData(INT8 *pText,
                      UINT8 *dataBuf,
                      UINT32 dataLen)
{
    HANDLE hkey;

    CSW_PROFILE_FUNCTION_ENTER(TIM_WriteRegData);
    if(pText == NULL || dataBuf == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteRegData);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(!TIM_OpenKey(&hkey))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteRegData);
        TIM_ERR_RETURN(TM_OPEN_REG_ERROR);
    }

    if(!TIM_WriteBinSet(hkey, pText,
                        dataBuf, dataLen))
    {
        TIM_CloseKey(hkey);
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteRegData);
        TIM_ERR_RETURN (TM_WRITE_REG_ERROR);
    }

    if(!TIM_CloseKey(hkey))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteRegData);
        TIM_ERR_RETURN (TM_CLOSE_REG_ERROR);
    }
    CSW_PROFILE_FUNCTION_EXIT(TIM_WriteRegData);
    return TRUE;
}
#endif
#ifdef TIM_REG_FFS_USED

/*
*Description:
*  open the register key
*Parameter:
*  pHkey[out]: a pointer to save the key handle
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/

BOOL TIM_OpenKey(HANDLE *pHkey)
{
    return TRUE;
}

/*
*Description:
*  close the register key
*Parameter:
*  hkey[in]: the opened handle
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/


BOOL TIM_CloseKey(HANDLE hkey)
{
    return TRUE;
}



/*
*Description:
*  write binary data to register flash
*Parameter:
*  hkey[in]: a pointer to save the opened key handle
*  Ptext[in]:the value name
*  stringtxt[in]:the buffer pointer to save the data
*  length[in]:the data length
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/

/*
BOOL TIM_WriteBinSet(HANDLE hkey,
                     INT8  *Ptext,
                     UINT8 *stringtxt,
                     UINT32 length)
{
#ifdef CHIP_HAS_AP
    INT32 offset;
    UINT32 rec_len;
#endif

    CSW_PROFILE_FUNCTION_ENTER(TIM_WriteBinSet);
    if(Ptext == NULL || stringtxt == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteBinSet);
        return FALSE;
    }

    if(REG_SetValue(hkey, TEXT(Ptext), REG_TYPE_BINARY, (void *)stringtxt, length) != ERR_SUCCESS)
    {
        UINT32 POSSIBLY_UNUSED ret = SUL_GetLastError();
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_WriteBinSet()SUL_GetLastError ret = 0x0%x\n",0x08000157), ret);
        CSW_PROFILE_FUNCTION_EXIT(TIM_WriteBinSet);
        TIM_ERR_RETURN(TM_WRITE_REG_ERROR);
    }
#ifdef CHIP_HAS_AP
    offset = REG_GetValueOffset(hkey, TEXT(Ptext));
    if(offset >= 0 )
    {
        rec_len = REG_GetLineLen();
        cfg_update_notify((UINT32)offset,rec_len);
    }
#endif
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_WriteBinSet()==OK==\n",0x08000158));
    CSW_PROFILE_FUNCTION_EXIT(TIM_WriteBinSet);

    return TRUE;
}
*/

/*
*Description:
*  read data from the register flash
*Parameter:
*  hkey[in]: the opened key handle
*  Ptext[in]:the value name.
*  stringtxt[out]:the buffer to save read data
*  plength[in]:the length of the data to read
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/

BOOL TIM_ReadBinSet(HANDLE hkey,
                    INT8 *Ptext,
                    UINT8 *stringtxt,
                    UINT32 *plength)
{
    UINT8 cType = 0;

    CSW_PROFILE_FUNCTION_ENTER(TIM_ReadBinSet);

    if(Ptext == NULL || stringtxt == NULL || plength   == NULL)
    {
        return FALSE;
    }

    if(REG_GetValue(hkey, TEXT(Ptext), &cType,
                    (void *)stringtxt, (UINT8 *)plength) != ERR_SUCCESS)
    {
        UINT32 POSSIBLY_UNUSED ret = SUL_GetLastError();
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_ReadBinSet()SUL_GetLastError ret = 0x0%x\n",0x08000159), ret);
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadBinSet);
        TIM_ERR_RETURN(TM_READ_REG_ERROR);
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_ReadBinSet()==OK==\n",0x0800015a));

    CSW_PROFILE_FUNCTION_EXIT(TIM_ReadBinSet);
    return TRUE;
}




/*
*Description:
*  read the data from register flash
*Parameters:
*  pText[in]: the value name.
*  dataBuf[out]:the pointer to the data buffer to write
*  pDataLen[in]:the pointer to data length of the buffer
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/

BOOL TIM_ReadRegData(INT8 *pText,
                     UINT8 *dataBuf,
                     UINT32 *pDataLen)
{
    HANDLE hkey;
    CSW_PROFILE_FUNCTION_ENTER(TIM_ReadRegData);

    if(pText       == NULL
            || dataBuf  == NULL
            || pDataLen == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadRegData);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(!TIM_OpenKey(&hkey))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadRegData);
        TIM_ERR_RETURN(TM_OPEN_REG_ERROR);
    }

    if(!TIM_ReadBinSet(hkey, pText, dataBuf, pDataLen))
    {
        TIM_CloseKey(hkey);
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadRegData);
        TIM_ERR_RETURN(TM_READ_REG_ERROR);
    }

    if(!TIM_CloseKey(hkey))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadRegData);
        TIM_ERR_RETURN (TM_CLOSE_REG_ERROR);
    }

    CSW_PROFILE_FUNCTION_EXIT(TIM_ReadRegData);
    return TRUE;
}

/*
*Description:
*  write an alarm information structure to register flash
*from a global buffer,a dummy flash.
*Parameter:
*  index[in]: the alarm index, it will specify the position
*of an alarm in the dummy flash.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_WriteAlarmSet(UINT8 index)
{
    UINT8  alarmindx[20] = {0};
    UINT8 len = SIZEOF(TIM_ALARM_EX);

    SUL_MemSet8(alarmindx, 0, 20);
    SUL_StrPrint(alarmindx, "TM_ALARM%d", index);
    if(NV_SetTmAlarml(index,
                         &TIM_Glo_Infor_Buf[TIM_TIME_INFO_LEN + (index - 1)*len],
                         len))
    {
        return FALSE;
    }

    return TRUE;
}

/*
*Description:
*  read an alarm information from real register flash to dummy
*flash,a global buffer.
*Parameter:
*  index[in]: the alarm index to read, it will specify the position
*in register.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_ReadAlarmSet(UINT8 index)
{
    UINT8  alarmindx[20] = {0};
    UINT32 len = SIZEOF(TIM_ALARM_EX);

    SUL_MemSet8(alarmindx, 0, 20);
    SUL_StrPrint(alarmindx, "TM_ALARM%d", index);
    if(NV_GetTmAlarml(index,
                         &TIM_Glo_Infor_Buf[TIM_TIME_INFO_LEN + (index - 1)*len],
                         &len))
    {
        return FALSE;
    }

    return TRUE;
}
#else
/*
*Description:
*  write data to real flash from dummy flash.
*Parameter:
*  none
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_WriteFlash(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(TIM_WriteFlash);
    CII_Flash_Block_Dirty();
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_WriteFlash()==111==\n",0x0800015b));

    CSW_PROFILE_FUNCTION_EXIT(TIM_WriteFlash);

    return TRUE;
}

/*
*Description:
*  read flash data to a buffer.
*Parameter:
*  Pdata[out]: a pointer to save the read data
*  bytesize[in]: the data length to read
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_ReadFlash(UINT8 *Pdata, UINT32 bytesize)
{
    CSW_PROFILE_FUNCTION_ENTER(TIM_ReadFlash);
    if(Pdata == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadFlash);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_ReadFlash()==111==\n",0x0800015c));
    if(!CII_Flash_Block_Init(TM_FLASH_DATA, Pdata, bytesize))
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_ReadFlash()==fail==\n",0x0800015d));
        CSW_PROFILE_FUNCTION_EXIT(TIM_ReadFlash);
        TIM_ERR_RETURN(TM_READ_FLASH_ERROR);
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(2)|TSMAP(1)|TSMAP(2), TSTR("TIM_ReadFlash()==OK==[%s] [%s]\n",0x0800015e), &Pdata[694], Pdata);
    CSW_PROFILE_FUNCTION_EXIT(TIM_ReadFlash);
    return TRUE;
}
#endif




//END_C




















