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












#include <cswtype.h>
#include <event.h>
#include <cos.h>
#include <errorcode.h>
#include "chip_id.h"
#include "hal_timers.h"
#include "hal_host.h"

#include "csw_debug.h"
#include "csw_csp.h"
#include <dm.h>
#include <tm.h>
#include <sul.h>
#include <ts.h>
#include <base_prv.h>
#include "tm_prv.h"
#include "pmd_m.h"

#ifdef TIM_REG_FFS_USED

#else
#include <block_flash_op_api.h>
#endif

#define RTC_reset_value 154828800//一周 070626 yyj
#define TIM_DEL_SINGLE_ALARM

//time alarm global information
extern TM_PARAMETERS G_TM_Parameters;
extern UINT8 TIM_Glo_Infor_Buf[TIM_DATA_TOL_LEN] ;
extern UINT32  TIM_LastErrorCode;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
extern VOID VDS_CacheFlushAll(VOID);
#endif

PRIVATE BOOL PowerOnInitDataFlag = FALSE;
BOOL TIM_SetGloAlarmInfo(UINT16 alarmNum, TIM_ALARM_EX *pAlarmEX);
BOOL TIM_GetGloAlarmInfo(UINT8 index, TIM_ALARM_EX *pAlarmEX);
BOOL TIM_ReadGloTolData(UINT8 *destBuf);
BOOL TIM_WriteGloTolData(UINT8 *srcBuf);

PRIVATE void inline TIM_AlarmGetDayOfWeek(
    UINT32 Nowtime,
    INT8 *weekdaynow,
    UINT32 Alarmtime,
    INT8 *weekdayalarm,
    INT8 *daysBetween);


PRIVATE BOOL TIM_SetFutureAlmRTC(VOID);
PRIVATE BOOL TIM_KillFlashAlarm(UINT8 nIndex);
PRIVATE BOOL TIM_GetAllAlmIdxAndNmm(TIM_MAIN_ALARM_INFO *pAlarmMainInfo);
PRIVATE BOOL TIM_ResetAlarmTime();
PRIVATE BOOL TIM_PowerOnInitData(UINT8 *pDefaultErr);
PRIVATE BOOL TIM_KillAlarmEX(UINT8 nIndex);
PRIVATE BOOL TIM_SetAlarmEX(UINT8 writeFlashSwitch,
                            TIM_ALARM_EX *pAlarmEX,
                            UINT8 *pAlarmNum);

PRIVATE VOID TIM_UpdateAlarmTime ( TIM_ALARM_EX *pAlarmEX,
                                   UINT32 NowTime,
                                   UINT8 TodayDay);


/*
*Description:
*  change external alarm information to internal alarm information.
*Parameter:
*  pAlarmEX[out]:the internal alarm structure pointer.
*  pAlarm[in]:the external structure pointer.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_AlarmInfoToEX(TIM_ALARM_EX *pAlarmEX,
                               CONST TM_ALARM *pAlarm)
{
    if(pAlarmEX == NULL || pAlarm == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    pAlarmEX->nFileTime   = pAlarm->nFileTime;
    pAlarmEX->nIndex      = pAlarm->nIndex;
    pAlarmEX->nRecurrent  = pAlarm->nRecurrent;
    pAlarmEX->nTextLength = pAlarm->nTextLength;
    pAlarmEX->nType       = pAlarm->nType;
    pAlarmEX->padding[0]  = pAlarm->padding[0];
    pAlarmEX->padding[1]  = pAlarm->padding[1];

    if(pAlarmEX->nTextLength > 0)
    {
        SUL_MemSet8(pAlarmEX->pText, 0, TM_ALM_TEXT_LEN);
        if(pAlarm->pText == NULL)
        {
            TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
        }

        SUL_MemCopy8(pAlarmEX->pText, pAlarm->pText, pAlarm->nTextLength);
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_AlarmInfoToEX()==OK==\n",0x080000e0));
    return TRUE;
}

/*
*Description:
*  change internal alarm information to external alarm information.
*Parameter:
*  pAlarmEX[in]:the internal alarm structure pointer.
*  pAlarm[out]:the external structure pointer.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_EXInfoToAlarm(TM_ALARM *pAlarm,
                               CONST TIM_ALARM_EX *pAlarmEX)
{
    if(pAlarmEX == NULL || pAlarm == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    pAlarm->nFileTime   = pAlarmEX->nFileTime;
    pAlarm->nIndex      = pAlarmEX->nIndex;
    pAlarm->nRecurrent  = pAlarmEX->nRecurrent;
    pAlarm->nTextLength = pAlarmEX->nTextLength;
    pAlarm->nType       = pAlarmEX->nType;
    pAlarm->padding[0]  = pAlarmEX->padding[0];
    pAlarm->padding[1]  = pAlarmEX->padding[1];

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_EXInfoToAlarm()before SUL_MemCopy8 length = %d\n",0x080000e1), pAlarmEX->nTextLength);
    if(pAlarmEX->nTextLength > 0)
    {
        if(pAlarm->pText == NULL)
        {
            TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
        }
        SUL_MemCopy8(pAlarm->pText, pAlarmEX->pText, pAlarm->nTextLength);
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_EXInfoToAlarm()after SUL_MemCopy8\n",0x080000e2));

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_EXInfoToAlarm()==OK==\n",0x080000e3));

    return TRUE;
}

/*
*Description:
*   According to the seconds time, call this function to know the day,Monday to
*Sunday.
*Parameter:
*  Nowtime[in]:current time.
*  weekdaynow[out]:week day of now.
*  alarmTime[in]:alarm time
*  weekdayalarm[out]:
*  daysBetween[out]:
*Return:
*  void.
*/
static void inline TIM_AlarmGetDayOfWeek(
    UINT32 Nowtime,
    INT8 *weekdaynow,
    UINT32 Alarmtime,
    INT8 *weekdayalarm,
    INT8 *daysBetween)
{
    TM_FILETIME tm;
    if(weekdaynow    == NULL
            || weekdayalarm == NULL
            || daysBetween  == NULL)
    {
        return;
    }

    tm.DateTime = Nowtime;
    * weekdaynow   = TM_GetDayOfWeekEX(tm);
    tm.DateTime = Alarmtime;
    * weekdayalarm = TM_GetDayOfWeekEX(tm);
    * daysBetween = (INT8)((Alarmtime - Nowtime) / ONE_DAY_SECONDS);

    return;
}

//underside function:
//TIM_BakToDelSingleIdx,TIM_ClearBakSingleIdx,TIM_GetBakSingleIdx
//TIM_KillSingleAlmMem,TIM_OnlyDelSingleAlarmMem, only design
//to sovle the problem of power on alarm or power on to kill single alarm
//
/*
*bake to del sigle alarm indx,only when power on larm
*/
PRIVATE BOOL TIM_BakToDelSingleIdx(UINT16 singleIdxBits)
{
    UINT16 singleBake = 0;

    //here to protect TIM_Glo_Infor_Buf bak idx,because it is poosible
    //to del single index twice during timer(50seconds)
    SUL_MemCopy8((UINT8 *)(&singleBake), &TIM_Glo_Infor_Buf[TIM_BAK_IDX_BIT_POS],
                 SIZEOF(UINT16));
    singleBake = singleIdxBits | singleBake;

    SUL_MemCopy8(&TIM_Glo_Infor_Buf[TIM_BAK_IDX_BIT_POS], (UINT8 *)(&singleBake),
                 SIZEOF(UINT16));

    return TRUE;
}

/*
*if timer has del flash single alarm, so  clear bake to del index
* when power on larm
*/
PRIVATE BOOL TIM_ClearBakSingleIdx(VOID)
{
    SUL_MemSet8(&TIM_Glo_Infor_Buf[TIM_BAK_IDX_BIT_POS], 0, SIZEOF(UINT16));

    return TRUE;
}

/*
*get bake to del sigle alarm indx,only
*when write flash timer is reach
*/
PRIVATE BOOL TIM_GetBakSingleIdx(UINT16 *pSingleIdxBits)
{
    if(pSingleIdxBits == NULL)
    {
        return FALSE;
    }

    //here to protect TIM_Glo_Infor_Buf bak idx,because it is poosible
    //to del single index twice during timer(50seconds)
    SUL_MemCopy8((UINT8 *)pSingleIdxBits, &TIM_Glo_Infor_Buf[TIM_BAK_IDX_BIT_POS],
                 SIZEOF(UINT16));

    TIM_ClearBakSingleIdx();

    return TRUE;
}

/*
*only to del single alarm indx in mem,only when power on larm
*/
PRIVATE BOOL TIM_KillSingleAlmMem(UINT8 nIndex)
{
    TIM_ALARM_EX alarmStruEX;


    SUL_MemSet8((UINT8 *)(&alarmStruEX), 0, SIZEOF(alarmStruEX));

    if(!TIM_SetGloAlarmInfo(nIndex, &alarmStruEX))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_KillAlarmEX);
        return FALSE;
    }

    if(!TIM_BakToDelSingleIdx((UINT16)(0x1 << nIndex)))
    {
        return FALSE;
    }

    return TRUE;
}

PRIVATE BOOL TIM_DelOverdueAlarm(VOID)
{
    TM_FILETIME   FileTime;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmEx;
    UINT8 i = 0;
    UINT16 bakeSingleIdx = 0;

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DelOverdueAlarm() begin\n",0x080000e4));

    // Get system's time in seconds.
    if(!TM_GetSystemFileTime(&FileTime))
    {
        return FALSE;
    }

    // Get index of valid configured alarns.
    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        return FALSE;
    }

    /*
    * here is to recover single alarm index bit in alarmMainInfo.validIdxBit
    * before timer we have clear mem, now shoul clear flash overdue single
    * alarm
    */
    if(!TIM_GetBakSingleIdx(&bakeSingleIdx))
    {
        return FALSE;
    }

    //alarmMainInfo.validIdxBit = bakeSingleIdx | alarmMainInfo.validIdxBit;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(2), TSTR("TIM_DelOverdueAlarm() bakeSingleIdx = 0x%x,validIdxBit = 0x%x\n",0x080000e5), bakeSingleIdx, alarmMainInfo.validIdxBit);

    bakeSingleIdx = bakeSingleIdx & (~(bakeSingleIdx & alarmMainInfo.validIdxBit));

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        if(bakeSingleIdx & (0x1 << i))
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DelOverdueAlarm() find a sigle bake alm\n",0x080000e6));
            if(TRUE != TIM_KillFlashAlarm(i))
            {
                return FALSE;
            }
        }

        if(alarmMainInfo.validIdxBit & (0x1 << i))
        {
            if(!TIM_GetGloAlarmInfo(i, &alarmEx))
            {
                return FALSE;
            }

            if((alarmEx.nFileTime <= FileTime.DateTime)
                    && (alarmEx.nRecurrent == 1))
            {
                CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DelOverdueAlarm() find a sigle bit alm\n",0x080000e7));
                if(TRUE != TIM_KillFlashAlarm(alarmEx.nIndex))
                {
                    return FALSE;
                }
            }
        }
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DelOverdueAlarm() end\n",0x080000e8));
    return TRUE;
}

//找出离现在最近的闹钟
PRIVATE BOOL TIM_GetFutureAlarm(UINT8 *pIndex, UINT32 *pTime)
{
    TM_FILETIME   FileTime;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmEx;
    UINT8 i = 0;
    UINT32 futureTm = 0;
    UINT8 futureIdx = 0;

    if(pIndex == NULL || pTime == NULL)
    {
        return FALSE;
    }

    // Find the System's time in Seconds.
    if(!TM_GetSystemFileTime(&FileTime))
    {
        return FALSE;
    }

    // Get the index of the valid configured alarmes.
    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        return FALSE;
    }

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        // If the "i" alarm is configured.
        if(alarmMainInfo.validIdxBit & (0x1 << i))
        {
            // Get its configuration.
            if(!TIM_GetGloAlarmInfo(i, &alarmEx))
            {
                return FALSE;
            }

            // If its response time is bigger than the current time.
            if(FileTime.DateTime < alarmEx.nFileTime)
            {
                // Check if it is the next one to be triggered.
                if((futureTm == 0) || (futureTm > alarmEx.nFileTime))
                {
                    futureTm = alarmEx.nFileTime;
                    futureIdx = i;
                }
            }
        }
    }

    *pIndex = futureIdx;
    *pTime = futureTm;

    return TRUE;
}

PRIVATE BOOL TIM_SetHalAlmRTC(UINT8 idx, UINT32 time)
{

    if(idx > TIM_ALARM_INDEX_MAX)
    {
        return FALSE;
    }

    HAL_TIM_RTC_TIME_T AlarmTime;
    TM_SYSTEMTIME SystemTime;
    TM_FILETIME TimeSec;

    TimeSec.DateTime = time;


    // Convert time in seconds to the system_time_structure
    TM_FileTimeToSystemTime (TimeSec, &SystemTime);

    AlarmTime.year  = (UINT8) (SystemTime.uYear - 2000);
    AlarmTime.month = SystemTime.uMonth ;
    AlarmTime.wDay  = SystemTime.uDayOfWeek;
    AlarmTime.day   = SystemTime.uDay ;
    AlarmTime.hour  = SystemTime.uHour;
    AlarmTime.min   = SystemTime.uMinute ;
    AlarmTime.sec   = SystemTime.uSecond;

    if (hal_TimRtcSetAlarm (&AlarmTime) != HAL_ERR_NO)
    {
        return FALSE;
    }
    else
    {
        hal_TimRtcAlarmEnable();
    }

    return TRUE;
}

/*
*Description:
*  get the min alarm information.得到当前所有闹钟信息并找出最近闹钟
*Parameter:
*  pAlarmMainInor[out]:this structure pointer includes main information
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_GetAllAlmIdxAndNmm(TIM_MAIN_ALARM_INFO *pAlarmMainInfo)
{
    UINT8 i = 0, AlarmIdx = 0;
    UINT32 MinTime       = 0;
    TIM_ALARM_EX alarmEX;
    UINT8 alarmNumb      = 0;
    UINT16 validIdxBit   = 0;
    UINT16 textTotallen  = 0;

    if(pAlarmMainInfo == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++) //查询所有闹铃INDEX并找出最近闹铃
    {
        if(!TIM_GetGloAlarmInfo(i, &alarmEX))//从全局中把指定INDEX的闹铃结构读出来
        {
            return FALSE;
        }

        if(alarmEX.nFileTime)
        {
            if(!MinTime)
            {
                CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_GetAllAlmIdxAndNmm()alarmEX.nIndex = %d\n",0x080000e9), alarmEX.nIndex);
                MinTime  = alarmEX.nFileTime;
                AlarmIdx = i;
            }
            else
            {
                MinTime  = MIN(MinTime, alarmEX.nFileTime);
                if(MinTime == alarmEX.nFileTime)
                {
                    AlarmIdx = i;
                    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_GetAllAlmIdxAndNmm()SHITAlarmIdx = %d\n",0x080000ea), AlarmIdx);

                }
            }

            validIdxBit = (validIdxBit | (0x1 << i));
            alarmNumb++;
            textTotallen = textTotallen + alarmEX.nTextLength;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(3), TSTR("TIM_GetAllAlmIdxAndNmm()AlarmIdx = %d,alarmEX.nIndex = %d,alarmEX.nRecurrent = %d\n",0x080000eb),
                      AlarmIdx, alarmEX.nIndex, alarmEX.nRecurrent);

        }

        /*  if(alarmEX.nFileTime != 0 && MinTime == 0)//第一次比较
          {
              CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1),TSTR("TIM_GetAllAlmIdxAndNmm()alarmEX.nIndex = %d\n",0x080000ec),alarmEX.nIndex);
              MinTime  = alarmEX.nFileTime;
              AlarmIdx = i;
              validIdxBit  = (validIdxBit | (0x1<<i));
              alarmNumb++;
              textTotallen = textTotallen+alarmEX.nTextLength;
              CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(3),TSTR("TIM_GetAllAlmIdxAndNmm()AlarmIdx = %d,alarmEX.nIndex = %d,alarmEX.nRecurrent = %d\n",0x080000ed),
                            AlarmIdx,alarmEX.nIndex,alarmEX.nRecurrent);
          }
          else if(alarmEX.nFileTime != 0 && MinTime != 0)//后几次比较
          {
              MinTime  = MIN(MinTime,alarmEX.nFileTime);

              if(MinTime == alarmEX.nFileTime)
              {
                  AlarmIdx = i;
                  CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1),TSTR("TIM_GetAllAlmIdxAndNmm()SHITAlarmIdx = %d\n",0x080000ee),AlarmIdx);

              }
              validIdxBit = (validIdxBit | (0x1<<i));
              alarmNumb++;
              textTotallen = textTotallen+alarmEX.nTextLength;
              CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(3),TSTR("TIM_GetAllAlmIdxAndNmm()AlarmIdx = %d,alarmEX.nIndex = %d,alarmEX.nRecurrent = %d\n",0x080000ef),
                            AlarmIdx,alarmEX.nIndex,alarmEX.nRecurrent);


          }

          else
         {
              continue;
         }*/
    }

    pAlarmMainInfo->alarmNum     = alarmNumb;
    pAlarmMainInfo->minAlarmIndx = AlarmIdx;
    pAlarmMainInfo->totalTxtlen  = textTotallen;
    pAlarmMainInfo->validIdxBit  = validIdxBit;

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_GetAllAlmIdxAndNmm()pAlarmMainInfo->minAlarmIndx = %d\n",0x080000f0), pAlarmMainInfo->minAlarmIndx);

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_GetAllAlmIdxAndNmm()==OK==\n",0x080000f1));

    return TRUE;
}

/*
*Description:
*  when the alarm is responsing, need to ensure the up-to-date time, then add the
*least day.
*Parameter:
*  weekday[in]:week day now(0 is MON,1 is TUE.....).
*  nRecurrent[in]:responsed alarm recurrent.
*  pNumDays[out]:the days of to response.
*Return:
*  void.
*/
static void TIM_CheckAlarmOfWeek(UINT8 weekday, UINT8 nRecurrent, UINT8 *n)
{
    UINT8 i = 1;
    //UINT8 alarmsetofweek = 0;
    UINT8 alarmweek = 0;
    //UINT8 recurrent = nRecurrent;

    CSW_PROFILE_FUNCTION_ENTER(TIM_CheckAlarmOfWeek);

    switch(weekday)
    {
        case MON:
            //  alarmsetofweek = ALARM_MON_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is MON \n",0x080000f2));
            break;
        case TUE:
            //  alarmsetofweek = ALARM_TUE_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is TUE \n",0x080000f3));
            break;
        case WED:
            //  alarmsetofweek = ALARM_WED_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is WED \n",0x080000f4));
            break;

        case THU:
            //  alarmsetofweek = ALARM_THU_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is THU \n",0x080000f5));
            break;
        case FRI:
            //  alarmsetofweek = ALARM_FRI_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is FRI \n",0x080000f6));
            break;
        case SAT:
            //  alarmsetofweek = ALARM_SAT_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is SAT \n",0x080000f7));
            break;
        case SUN:
            //  alarmsetofweek = ALARM_SUN_OF_WEEK;
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("In Fun: TIM_CheckAlarmOfWeek NOW is SUN \n",0x080000f8));
            break;
        default:
            break;
    }

    alarmweek =  1 << (weekday + 1);

    //alarmweek = alarmsetofweek;
    for (i = 1; i <= 6; i++)
    {
        if (alarmweek == ALARM_SUN_OF_WEEK)
        {
            alarmweek = ALARM_MON_OF_WEEK;
        }
        else
        {
            alarmweek = alarmweek * 2;
        }

        //if ((alarmweek & recurrent) != alarmweek)
        //if ((alarmweek & nRecurrent) != alarmweek)
        //{
        //  continue;
        //}
        //break;
        if ((alarmweek & nRecurrent) == alarmweek)
        {
            break;
        }

    }
    *n = i;// i days later alarm need response

    CSW_PROFILE_FUNCTION_EXIT(TIM_CheckAlarmOfWeek);

    return;
}

UINT8 TIM_GetSingleValidIdx(UINT16 CurIdxBit, UINT8 index)
{
    if(CurIdxBit == 0 || index == 0
            || index > TIM_ALARM_INDEX_MAX)
    {
        return 0;
    }

    if(CurIdxBit & (1 << index))
    {
        return index;
    }
    /* else
     {
         return 0;
     }*/

    return 0;
}

//找到第一个合法的INDEX
UINT8 TIM_GetValidIdx(UINT16 CurIdxBit)
{
    UINT8 i = 0;

    /* for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
     {
         if(TIM_GetSingleValidIdx(CurIdxBit, i))
         {
             break;
         }
     }

     if(i < TIM_ALARM_INDEX_MAX + 1)
     {
         return i;
     }

     return 0;*/

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        if(TIM_GetSingleValidIdx(CurIdxBit, i))
        {
            return i;
        }
    }

    return 0;
}

//得到与当前时间相等的所有闹钟INDEX
BOOL TIM_GetEqualTimeAlmIdx(UINT16 *pAlarmIndex)
{
    TM_FILETIME   alarmFileTime;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmEx;
    UINT8 i = 0;
    TM_SYSTEMTIME  nowSystemTime, alarmSysTime;

    if(pAlarmIndex == NULL)
    {
        return FALSE;
    }

    *pAlarmIndex = 0;

    if(!TM_GetSystemTime(&nowSystemTime))
    {
        return FALSE;
    }

    nowSystemTime.uSecond       = 0;//不比较秒
    nowSystemTime.uMilliseconds = 0;//不比较毫秒
    nowSystemTime.uDayOfWeek = (nowSystemTime.uDayOfWeek == 0) ? 7 : nowSystemTime.uDayOfWeek;

    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        return FALSE;
    }

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        if(alarmMainInfo.validIdxBit & (0x1 << i))
        {
            if(!TIM_GetGloAlarmInfo(i, &alarmEx))
            {
                return FALSE;
            }

            alarmFileTime.DateTime = alarmEx.nFileTime;

            if(!TM_FileTimeToSystemTime(alarmFileTime, &alarmSysTime))
            {
                return FALSE;
            }
            alarmSysTime.uSecond       = 0;//不比较秒
            alarmSysTime.uMilliseconds = 0;//不比较毫秒

            if((alarmSysTime.uHour == nowSystemTime.uHour)
                    && (alarmSysTime.uMinute == nowSystemTime.uMinute))//是否时分相同
            {
                if(alarmEx.nRecurrent == 1)//如果单次,比较年月日
                {
                    if((nowSystemTime.uYear == alarmSysTime.uYear)
                            && (nowSystemTime.uMonth == alarmSysTime.uMonth)
                            && (nowSystemTime.uDay == alarmSysTime.uDay))
                    {
                        *pAlarmIndex = (*pAlarmIndex) | (1 << i);
                    }
                }
                else if(alarmEx.nRecurrent == ALARM_FOR_EVERY_DAY
                        || alarmEx.nRecurrent == ALARM_ALL_WEEK_DAY)//如果每日,不用再比较
                {
                    *pAlarmIndex = (*pAlarmIndex) | (1 << i);
                }
                else if(alarmEx.nRecurrent & (1 << nowSystemTime.uDayOfWeek))//如果非每日,比较星期数
                {
                    *pAlarmIndex = (*pAlarmIndex) | (1 << i);
                }
            }
        }
    }

    return TRUE;
}

UINT32 DM_AlmRegTimerServ(VOID)
{
    TIM_DelOverdueAlarm();

    return ERR_SUCCESS;
}


/*
*Description:
*  1.this function is called after the alarm hardware sends the EVENT of alarm response.
*  2.this function disposes responded alarm according to its RECURRENT type.
*  3.reset the new alarm if need.
*  4.send EVENT to other module(not implemented because system reason)
*  5.if needed, the function should transmit the information of responded alarm.(not
*implemented because not confirmation, maybe do this by function parameter pointer)
*/
UINT32 DM_AlarmDisposeInEvt( UINT8 nCause,
                             COS_EVENT *pEvent) // output
{
    COS_EVENT ev;
    UINT32 savedSec = 0;
    TM_ALARM *PalarmStru1 = NULL;
    TIM_ALARM_EX alarmEX;
    TM_PARAMETERS TM_para;
    UINT8 PowerOnFlag = FALSE;
    UINT8 flashERR = 0;
    UINT16 alarmIdxBit = 0;

    CSW_PROFILE_FUNCTION_ENTER(dm_AlarmDisposeInEvt);

    //CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1),TSTR("dm_AlarmDisposeInEvt()alarmMainInfo.minAlarmIndx = %d\n",0x080000f9),alarmMainInfo.minAlarmIndx);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt() Begin",0x080000fa));

    if(pEvent == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
        return FALSE;
    }

    //here need to judge flag and read alarm info to global
    if(PowerOnInitDataFlag == FALSE)
    {
        if(!TIM_PowerOnInitData(&flashERR))
        {
            CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
            return FALSE;
        }


        PowerOnFlag = TRUE;//设置开机闹钟标志
    }

    SUL_ZeroMemory8( (UINT8 *)(&ev), SIZEOF(COS_EVENT) );
    SUL_MemSet8((UINT8 *)(&alarmEX), 0, SIZEOF(TIM_ALARM_EX));
    TIM_GetTimePara(&TM_para);//得到所存基准时间
    savedSec = TM_para.iBase;

    if(!TIM_GetEqualTimeAlmIdx(&alarmIdxBit))//找是哪些闹钟响了
    {
        CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
        return FALSE;
    }
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070428
    if((alarmIdxBit != 0) && PowerOnFlag)
    {
        //Change the timer name
        if(!COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY),
                           PRV_TM_POWERON_ALM_TIMER_ID, COS_TIMER_MODE_SINGLE,
                           TIM_POWERON_WRITE_FLASH_TIMER))
        {
            return FALSE;
        }
    }
#endif

    if(alarmIdxBit == 0)//为自定义闹钟
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("dm_AlarmDisposeInEvt() savedSec = 0x%x!",0x080000fb), savedSec);

        if(PowerOnFlag)
        {
            PalarmStru1 = (TM_ALARM *)CSW_TIM_MALLOC(SIZEOF(TM_ALARM));
            if(PalarmStru1 == NULL)
            {
                TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
            }
            //CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),TSTR("In DM_AlarmDisposeInEvt() only a define alarm! power on\n",0x080000fc));
            PalarmStru1->nIndex = 17;
            pEvent->nParam3 = (UINT32)PalarmStru1;
        }
    }
    else//真实闹钟
    {
        UINT8 index = 0;

        index = TIM_GetValidIdx(alarmIdxBit);//找到响铃闹钟的INDEX

        if(index == 0)
        {
            CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
            return FALSE;
        }

        if(!TIM_GetGloAlarmInfo(index, &alarmEX))//得到响铃闹钟的信息
        {
            CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
            return FALSE;
        }
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(4)|TSMAP(1), TSTR("dm_AlarmDisposeInEvt()Alarmtext = %s,alarmEX.nIndex = %d,alarmEX.nRecurrent = %d len = %d\n",0x080000fd),
                  alarmEX.pText, alarmEX.nIndex, alarmEX.nRecurrent, alarmEX.nTextLength);

        if(PowerOnFlag)//如果是开机闹钟
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(2), TSTR("dm_AlarmDisposeInEvt()defore malloc%d %d\n",0x080000fe), SIZEOF(TM_ALARM) + alarmEX.nTextLength, alarmEX.nTextLength);
            PalarmStru1 = (TM_ALARM *)CSW_TIM_MALLOC(SIZEOF(TM_ALARM) + alarmEX.nTextLength);
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt()after malloc\n",0x080000ff));

            if(PalarmStru1 == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt() malloc the pointer PalarmStru1 faild!",0x08000100));
                CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
                return FALSE;
            }
            PalarmStru1->pText = (UINT8 *)((UINT8 *)PalarmStru1 + SIZEOF(TM_ALARM));
            //PalarmStru1->pText  = CSW_TIM_MALLOC(Alarm.nTextLength * 2);//the first para is the LOW,second is the HI
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt()before TIM_EXInfoToAlarm\n",0x08000101));
            TIM_EXInfoToAlarm(PalarmStru1, &alarmEX); //给ALARM结构赋值
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt()after TIM_EXInfoToAlarm\n",0x08000102));

            pEvent->nParam3 = (UINT32)PalarmStru1;  //这里不用发事件,只需填充事件结构就行
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt() normal alarm power on!\n",0x08000103));
        }
        else//正常闹钟
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt() normal alarm!\n",0x08000104));
            pEvent->nParam3  = MAKEUINT32(0, 0);
            ev.nEventId = EV_TIM_ALRAM_IND;
            if(alarmEX.nTextLength != 0)
            {
                ev.nParam1  = (UINT32)CSW_TIM_MALLOC(alarmEX.nTextLength);//the first para is the LOW,second is the HI
                if((UINT8 *)(ev.nParam1) == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt() malloc the pointer ev.nParam1 faild!",0x08000105));
                    CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
                    return FALSE;
                }
                SUL_MemCopy8((UINT8 *)(ev.nParam1), alarmEX.pText, alarmEX.nTextLength);
            }
            ev.nParam2  = MAKEUINT32(alarmEX.nType, alarmEX.nTextLength);
            ev.nParam3  = MAKEUINT32(alarmEX.nRecurrent, alarmEX.nIndex);//填充完事件结构后发送给MMI
            DM_SendArlarmMessage(&ev);
        }
    }
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070428
    if(!PowerOnFlag)
    {
        TIM_DelOverdueAlarm();
    }
#endif
    if(!TIM_ResetAlarmTime())
    {
        CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
        return FALSE;
    }

    if(!TIM_SetFutureAlmRTC())
    {
        CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
        return FALSE;
    }
    if((PalarmStru1 != NULL) && (PalarmStru1->nIndex == 17))
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("There is someting wrong with RTC,now SHUT_DOWN!!!!!!!!\n",0x08000106));
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
        VDS_CacheFlushAll();
#endif
        hal_HstSendEvent(0xdeada1a5);
        hal_ShutDown();
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("dm_AlarmDisposeInEvt()==OK==\n",0x08000107));

    CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);

    return TRUE;
}

/*
* Function description:
* only one example to respond Event
*/
BOOL tm_AlramProc(COS_EVENT *pEv)
{
    UINT32 EvtId;
    UINT32 ret;
    COS_EVENT *pEvent = (COS_EVENT *)pEv;
    COS_EVENT Event;

    if(pEv == NULL)
    {
        return FALSE;
    }

    EvtId = pEvent->nEventId;
    ret = ERR_SUCCESS;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("Alarm Reach! \n",0x08000108));

    if(EvtId == EV_TIM_ALRAM_IND)
    {
        ret = DM_AlarmDisposeInEvt(DM_POWRN_ON_CAUSE_ALARM, &Event);
    }
    else if(EvtId == EV_TIMER
            && pEvent->nParam1 == PRV_TM_POWERON_ALM_TIMER_ID)//dispose REG timer
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("Time REG Timer ID! \n",0x08000109));
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070428
        DM_AlmRegTimerServ();
#endif
    }

    return ret;
}

/*
* Description:
*    1. set the alarm to dummy and real flash
*    2. find the min alarm
*    3. set the alarm to hard ware.
*/
PRIVATE BOOL TIM_SetAlarmEX(UINT8 writeFlashSwitch, TIM_ALARM_EX *pAlarmEX, UINT8 *pAlarmNum)
{
    //UINT32 savedSec = 0;
    //UINT32 nowSec   = 0;
    UINT8 i = 0;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmStruEX;

    CSW_PROFILE_FUNCTION_ENTER(TIM_SetAlarmEX);

    if (pAlarmEX == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    /*check the index of alarm*/ //Already checked
    /*if (pAlarmEX->nIndex > TIM_ALARM_INDEX_MAX || pAlarmEX->nIndex <= 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }*/

    /* if(((pAlarmEX->nRecurrent & ALARM_ALL_WEEK_DAY) != 0) Move one level up.
        &&((pAlarmEX->nRecurrent & 0x1) != 0))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        TIM_ERR_RETURN(TM_ALARM_RECURRENT_ERROR);
    }*/

    //TM_PARAMETERS TM_para;
    // TIM_GetTimePara(&TM_para);
    // savedSec = TM_para.iBase;
    //nowSec = hal_timRTC_GetTime() / TIM_CLOCKS_PER_SECOND + savedSec;

    //The same thing with the above commands is done through tte function.
    TM_FILETIME   FileTime;

    // Get
    if(TRUE != TM_GetSystemFileTime(&FileTime))
    {
        return FALSE;
    }

    //check the recurrent,here Recurrent check is not needed.
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(2), TSTR("TIM_SetAlarmEX() nFileTime = %ld, nowSec = %ld \n",0x0800010a), pAlarmEX->nFileTime, FileTime.DateTime);
    //check the alarm time

    //updated by yyj 070428 from here
    //if(pAlarmEX->nFileTime <= nowSec)
    if(pAlarmEX->nFileTime <= FileTime.DateTime)
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_SetAlarmEX()  pAlarmEX->nFileTime <= FileTime.DateTime",0x0800010b));
        //  TM_FILETIME   FileTime;
        UINT8 nDay = 0;

        nDay = TM_GetDayOfWeekEX(FileTime);

        // If the alarm time has passed and the alarm is programmed once, error the alarm wont set.
        if((pAlarmEX->nRecurrent & 0x1) == 1)
        {
            CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
            TIM_ERR_RETURN(TM_ALARM_TIME_MIN_ERROR);
        }

        else
        {

            TIM_UpdateAlarmTime (pAlarmEX, FileTime.DateTime, nDay);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TIM_SetAlarmEX() Now nFileTime = %ld",0x0800010c), pAlarmEX->nFileTime);
    //to here

    // Check to see if there is another alarm programmed to ring on the same time
    if(!TIM_GetAllAlmIdxAndNmm( &alarmMainInfo))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        return FALSE;
    }

    if(alarmMainInfo.alarmNum != 0)
    {
        // dispose the equal time of alarm of different alarm index
        for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
        {
            if(alarmMainInfo.validIdxBit & (0x1 << i))
            {
                if(!TIM_GetGloAlarmInfo(i, &alarmStruEX))
                {
                    CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
                    return FALSE;
                }

                if((alarmStruEX.nFileTime == pAlarmEX->nFileTime)
                        && (i != pAlarmEX->nIndex))
                {
                    CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
                    TIM_ERR_RETURN(TM_ALARM_TIME_EQUAL_ERROR);
                }
            }
        }
    }

    // add 2006.8.25 chenwf
    TIM_ALARM_EX alarmEXBak;

    if(!TIM_GetGloAlarmInfo(pAlarmEX->nIndex, &alarmEXBak))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);

        return FALSE;
    }

    if(!TIM_SetGloAlarmInfo(pAlarmEX->nIndex, pAlarmEX))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        return FALSE;
    }
    if(writeFlashSwitch == TRUE)
    {
#ifdef TIM_REG_FFS_USED
        if(!TIM_WriteAlarmSet(pAlarmEX->nIndex))
        {
            TIM_SetGloAlarmInfo(pAlarmEX->nIndex, &alarmEXBak);

            CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
            return FALSE;
        }
#else
        if(!TIM_WriteFlash())
        {
            TIM_SetGloAlarmInfo(pAlarmEX->nIndex, &alarmEXBak);

            CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
            return FALSE;
        }
#endif
    }

    if(!TIM_SetFutureAlmRTC())
    {
        CSW_PROFILE_FUNCTION_EXIT(dm_AlarmDisposeInEvt);
        return FALSE;
    }

    if(!TIM_GetAllAlmIdxAndNmm( &alarmMainInfo))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
        return FALSE;
    }

    * pAlarmNum = alarmMainInfo.alarmNum;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_SetAlarmEX()==OK==\n",0x0800010d));
    CSW_PROFILE_FUNCTION_EXIT(TIM_SetAlarmEX);
    return TRUE;
}

/*
*Description:
*  this is user api, it set an alarm. and give the alarm number.
*Parameter:
*  pAlarm[in]:the alarm structure pointer.
*  pAlarmNum[out]:the alarm number pointer.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TM_SetAlarm(TM_ALARM *pAlarm, UINT8 *pAlarmNum)
{
    TIM_ALARM_EX alarmEx;
    UINT8 alarmNum = 0;//writeFlashSwitch = FALSE;
    UINT8 writeFlashSwitch = TRUE;

    CSW_PROFILE_FUNCTION_ENTER(TM_SetAlarm);
    if(pAlarm == NULL || pAlarmNum == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(pAlarm->nTextLength > TM_ALM_TEXT_LEN)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        TIM_ERR_RETURN(TM_ALARM_TEXT_LEN_ERROR);
    }

    if(pAlarm->nIndex > TIM_ALARM_INDEX_MAX || pAlarm->nIndex <= 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    if(((pAlarm->nRecurrent & ALARM_ALL_WEEK_DAY) != 0)
            && ((pAlarm->nRecurrent & 0x1) != 0))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        TIM_ERR_RETURN(TM_ALARM_RECURRENT_ERROR);
    }

    if(!TIM_AlarmInfoToEX(&alarmEx, pAlarm))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        return FALSE;
    }

    //writeFlashSwitch = TRUE;
    if(!TIM_SetAlarmEX(writeFlashSwitch, &alarmEx, &alarmNum))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
        return FALSE;
    }

    * pAlarmNum = alarmNum;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_SetAlarm()==OK==\n",0x0800010e));
    CSW_PROFILE_FUNCTION_EXIT(TM_SetAlarm);
    return TRUE;
}

/*
*Description:
*  this is user api. user should free the parameter if the alarm number
*is not zero.it will list all the alarms.
*Parameter:
*  pArrayCnt[out]:the alarm number pointer.
*  pAlarmArray[out]:the alarm information structure.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TM_ListAlarm (UINT8 *pArrayCnt, TM_ALARM **pAlarmArray)
{
    INT8 i = 0;
    INT8 j = 0;
    TM_ALARM *palarmStru     = NULL;
    TM_ALARM *palarmStruStar = NULL;
    UINT8 *ptrFlag           = NULL;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX        alarmStruEX;

    CSW_PROFILE_FUNCTION_ENTER(TM_ListAlarm);
    *pAlarmArray = NULL;

    if (pArrayCnt == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_ListAlarm);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    // Get index of valid configured alarms.
    if(!TIM_GetAllAlmIdxAndNmm( &alarmMainInfo))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_ListAlarm);
        return FALSE;
    }

    if(alarmMainInfo.alarmNum)
    {
        *pArrayCnt = alarmMainInfo.alarmNum;
        palarmStruStar = (TM_ALARM *)CSW_TIM_MALLOC(alarmMainInfo.alarmNum * SIZEOF(TM_ALARM) + alarmMainInfo.totalTxtlen);

        if(palarmStruStar == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_ListAlarm() malloc the pointer palarmStruStar faild!",0x0800010f));
            CSW_PROFILE_FUNCTION_EXIT(TM_ListAlarm);
            return FALSE;
        }

        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(2), TSTR("TM_ListAlarm()alarmMainInfo.totalTxtlen = %d alarmMainInfo.alarmNum = %d\n",0x08000110), alarmMainInfo.totalTxtlen, alarmMainInfo.alarmNum);

        palarmStru     = palarmStruStar;
        ptrFlag        = (UINT8 *)((UINT8 *)palarmStruStar + alarmMainInfo.alarmNum * SIZEOF(TM_ALARM));

        //get the saved alarm information
        for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
        {
            if(alarmMainInfo.validIdxBit & (0x1 << i))
            {
                // Get infos on the alarm.
                if(!TIM_GetGloAlarmInfo(i, &alarmStruEX))
                {
                    if(palarmStruStar != NULL)
                    {
                        CSW_TIM_FREE(palarmStruStar);
                    }
                    CSW_PROFILE_FUNCTION_EXIT(TM_ListAlarm);
                    return FALSE;
                }
                else
                {
                    palarmStru = (TM_ALARM *)((UINT8 *)palarmStruStar + (j++) * SIZEOF(TM_ALARM));
                    if (alarmStruEX.nTextLength > 0)
                    {
                        palarmStru->pText = (UINT8 *)ptrFlag;
                        ptrFlag = ptrFlag + alarmStruEX.nTextLength;
                        SUL_MemCopy8(palarmStru->pText , alarmStruEX.pText, alarmStruEX.nTextLength);
                    }

                    palarmStru->nFileTime   = alarmStruEX.nFileTime;
                    palarmStru->nTextLength = alarmStruEX.nTextLength;
                    palarmStru->nRecurrent  = alarmStruEX.nRecurrent;
                    palarmStru->nIndex      = alarmStruEX.nIndex;
                    palarmStru->nType       = alarmStruEX.nType;
                }
            }
        }

        *pAlarmArray = palarmStruStar;
    }
    // There are no configured alarms.
    else
    {
        * pArrayCnt = 0;
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_ListAlarm()==OK==\n",0x08000111));
    CSW_PROFILE_FUNCTION_EXIT(TM_ListAlarm);

    return TRUE;
}

PRIVATE BOOL TIM_KillFlashAlarm(UINT8 nIndex)
{
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmStruEX;


    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        return FALSE;
    }

    if((alarmMainInfo.validIdxBit & (0x1 << nIndex)) == 0)
    {
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    SUL_MemSet8((UINT8 *)(&alarmStruEX), 0, SIZEOF(alarmStruEX));

    TIM_ALARM_EX alarmEXBak;
    if(!TIM_GetGloAlarmInfo(nIndex, &alarmEXBak))
    {
        return FALSE;
    }
    if(!TIM_SetGloAlarmInfo(nIndex, &alarmStruEX))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_KillAlarmEX);
        return FALSE;
    }

#ifdef TIM_REG_FFS_USED
    if(!TIM_WriteAlarmSet(nIndex))
    {
        TIM_SetGloAlarmInfo(nIndex, &alarmEXBak);
        return FALSE;
    }
#else
    if(!TIM_WriteFlash())
    {
        TIM_SetGloAlarmInfo(nIndex, &alarmEXBak);
        return FALSE;
    }
#endif

    return TRUE;
}

//设置RTC的闹钟
PRIVATE BOOL TIM_SetFutureAlmRTC(VOID)
{
    UINT8 futureIdx = 0;
    UINT32 futureTm = 0;

    if(!TIM_GetFutureAlarm(&futureIdx, &futureTm))
    {
        return FALSE;
    }

    if(!TIM_SetHalAlmRTC(futureIdx, futureTm))
    {
        return FALSE;
    }

    return TRUE;
}
/*
*Description:
*  kill specified alarm.it will be called in user API
*TM_KillAlarm
*Parameter:
*  nIndex[in]: the alarm index to delete.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_KillAlarmEX (UINT8 nIndex)
{
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    CSW_PROFILE_FUNCTION_ENTER(TIM_KillAlarmEX);


    // Get the index of the valid configured alarms.
    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        return FALSE;
    }

    // Is the alarm to kill one of the configured one?
    if((alarmMainInfo.validIdxBit & (0x1 << nIndex)) == 0)
    {
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    // Kill it from tha flash.
    if(!TIM_KillFlashAlarm(nIndex))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_KillAlarmEX);
        return FALSE;
    }

    // Set next alarm.
    if(!TIM_SetFutureAlmRTC())
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_KillAlarmEX);
        return FALSE;
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_KillAlarmEX()==OK==\n",0x08000112));
    CSW_PROFILE_FUNCTION_EXIT(TIM_KillAlarmEX);

    return TRUE;
}

/*
*Description:
*  the user API.it kill specified alarm.
*Parameter:
*  nIndex[in]: the alarm index to delete.
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TM_KillAlarm (UINT8 nIndex)
{
    CSW_PROFILE_FUNCTION_ENTER(TM_KillAlarm);

    if (nIndex > TIM_ALARM_INDEX_MAX || nIndex < 1) // judge the index
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_KillAlarm);
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    if(!TIM_KillAlarmEX (nIndex))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_KillAlarm);
        return FALSE;
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_KillAlarm()==OK==\n",0x08000113));
    CSW_PROFILE_FUNCTION_EXIT(TM_KillAlarm);
    return TRUE;
}

/*
*Description:
*  get specified index alarm.it will be called in user API.
*Parameter:
*  nIndex[in]: the alarm index to get.
*  pAlarmEX[out]:alarm information structure
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_GetAlarmEX (UINT8 nIndex, TIM_ALARM_EX *pAlarmEX)
{
    TIM_ALARM_EX AlarmEx;
    TIM_MAIN_ALARM_INFO alarmMainInfo;

    CSW_PROFILE_FUNCTION_ENTER(TIM_GetAlarmEX);
    if(pAlarmEX == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    // this check is already done.
    /*  if (nIndex > TIM_ALARM_INDEX_MAX || nIndex < 1) // judge the index
      {
          CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
          TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
      }*/

    // Get the index of the valid conigured alarms.
    if(!TIM_GetAllAlmIdxAndNmm( &alarmMainInfo))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
        return (FALSE);
    }

    if((alarmMainInfo.validIdxBit & (0x1 << nIndex)) == 0)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    // Get infos of the alarm.
    if(!TIM_GetGloAlarmInfo(nIndex,  &AlarmEx))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
        return FALSE;
    }

    (* pAlarmEX) = AlarmEx;

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_GetAlarmEX()==OK==\n",0x08000114));
    CSW_PROFILE_FUNCTION_EXIT(TIM_GetAlarmEX);
    return TRUE;
}

/*
*Description:
*  this is user API. get the alarm information.
*Parameter:
*  nIndex[in]: the alarm index to delete.
*  pAlarm[out]:the alarm information structure pointer
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TM_GetAlarm (UINT8 nIndex, TM_ALARM *pAlarm)
{
    TIM_ALARM_EX alarmEX;

    CSW_PROFILE_FUNCTION_ENTER(TM_GetAlarm);

    if(pAlarm == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetAlarm);
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if (nIndex > TIM_ALARM_INDEX_MAX || nIndex < 1) // judge the index
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetAlarm);
        TIM_ERR_RETURN(TM_ALARM_INDEX_ERROR);
    }

    if(!TIM_GetAlarmEX(nIndex, &alarmEX))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetAlarm);
        return FALSE;
    }

    if(!TIM_EXInfoToAlarm(pAlarm, &alarmEX))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_GetAlarm);
        return FALSE;
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_GetAlarm()==OK==\n",0x08000115));
    CSW_PROFILE_FUNCTION_EXIT(TM_GetAlarm);

    return TRUE;
}

/*
*Description:
*  when power on, read data from flash
*Parameter:
*  PdfaultErr :  1<<2 mean falsh ERR
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
PRIVATE BOOL TIM_PowerOnInitData(UINT8 *pDefaultErr)
{
    UINT8 i = 0;
    UINT32 defaultTime = 0;

#ifdef TIM_REG_FFS_USED
    UINT32 datalen = 0;
    CSW_PROFILE_FUNCTION_ENTER(TIM_PowerOnInitData);
    if(pDefaultErr == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++) //把15个闹铃信息从REG中读到全局中
    {
        if(!TIM_ReadAlarmSet(i))
        {
            CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
            return FALSE;
        }
    }

    datalen = 4;
    if(NV_GetTmFormat(&TIM_Glo_Infor_Buf[4],&datalen))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }
    datalen = 4;
    if(NV_GetTmBase(&TIM_Glo_Infor_Buf[8] , &datalen))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }
    SUL_MemCopy8((UINT8 *)(&defaultTime), (UINT8 *)(&TIM_Glo_Infor_Buf[8]), 4);
    if(defaultTime == TIM_BASE_DEFAULT)//判断是否FLASH重置过或RTC是否掉电
    {
        * pDefaultErr = (* pDefaultErr) | (1 << 2);
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1),
              TSTR("TIM_PowerOnInitData()==the time = 0x%x==\n",0x08000116), defaultTime);
    datalen = 1;
    if(NV_GetTmZone(&TIM_Glo_Infor_Buf[12], &datalen))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }
    datalen = 1;
    if(NV_GetTmAlarm(&TIM_Glo_Infor_Buf[13], &datalen))
    {
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }
#else
    CSW_PROFILE_FUNCTION_ENTER(TIM_PowerOnInitData);
    if(!TIM_ReadFlash(TIM_Glo_Infor_Buf, TIM_DATA_TOL_LEN))
    {
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),
                  TSTR("TIM_PowerOnInitData()==read flash ERR==\n",0x08000117));
        CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
        return FALSE;
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1)|TSMAP(1),
              TSTR("TIM_PowerOnInitData() 111%s\n",0x08000118), TIM_Glo_Infor_Buf);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1)|TSMAP(1),
              TSTR("TIM_PowerOnInitData() 111%s\n",0x08000119), &TIM_Glo_Infor_Buf[694]);
    SUL_MemCopy8(tempBuf, TIM_Glo_Infor_Buf, 4);

    if(SUL_MemCompare(tempBuf, "CII.", 4) == 0)
    {
        SUL_MemCopy8(tempBuf, &TIM_Glo_Infor_Buf[TIM_DATA_TOL_LEN - 6], 6);

        if(SUL_MemCompare(tempBuf, "chenwf", 6) != 0)
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),
                      TSTR("TIM_PowerOnInitData()==flash data ERR==\n",0x0800011a));

            SUL_MemSet8(TIM_Glo_Infor_Buf, 0, TIM_DATA_TOL_LEN);
            SUL_MemCopy8(TIM_Glo_Infor_Buf, "CII.", 4);
            SUL_MemCopy8(&TIM_Glo_Infor_Buf[TIM_DATA_TOL_LEN - 6], "chenwf", 6);

            if(!TIM_WriteFlash())
            {
                CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),
                          TSTR("TIM_PowerOnInitData()==Write flash ERR==\n",0x0800011b));
                CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
                return FALSE;
            }
        }
    }
    else
    {
        SUL_MemSet8(TIM_Glo_Infor_Buf, 0, TIM_DATA_TOL_LEN);
        SUL_MemCopy8(TIM_Glo_Infor_Buf, "CII.", 4);
        SUL_MemCopy8(&TIM_Glo_Infor_Buf[TIM_DATA_TOL_LEN - 6], "chenwf", 6);
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),
                  TSTR("TIM_PowerOnInitData()==flash no Init==\n",0x0800011c));

        if(!TIM_WriteFlash())
        {
            CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0),
                      TSTR("TIM_PowerOnInitData()==Write flash ERR==\n",0x0800011d));
            CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);
            return FALSE;
        }
    }
#endif

    PowerOnInitDataFlag = TRUE;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1)|TSMAP(1),
              TSTR("TIM_PowerOnInitData() %s\n",0x0800011e), TIM_Glo_Infor_Buf);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1)|TSMAP(1),
              TSTR("TIM_PowerOnInitData() %s\n",0x0800011f), &TIM_Glo_Infor_Buf[694]);
    CSW_PROFILE_FUNCTION_EXIT(TIM_PowerOnInitData);

    return TRUE;
}

/*
*Description:
*  when power on, call this function
*Parameter:
*  rtcLostPower:
*  1<<1 mean lost power,need to renew RTC.
*  1<<2 mean falsh ERR
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*
*/
BOOL TM_PowerOn()
{
    BOOL rtcProgram = 0;
    COS_EVENT ev = {0x00,};
    UINT8 RtcLostPower = 0;

    ev.nEventId = EV_TIM_SET_TIME_IND;

    CSW_PROFILE_FUNCTION_ENTER(TM_PowerOn);
    //if the RTC has never been programmed

    HAL_TIM_RTC_TIME_T  RtcTime;
    if((rtcProgram = hal_TimRtcNotProgrammed()))
    {
        // FIXME Give a default valid value to the RTC calendar: 0:0:0 - 1/1/2000
        RtcTime.sec = TM_DEFAULT_TIME_SECOND;
        RtcTime.min = TM_DEFAULT_TIME_MINUTE;
#ifndef __QURAN_SUPPORT__
        RtcTime.hour = TM_DEFAULT_TIME_HOUR;
#else
        RtcTime.hour = 0;
        extern void QuranPowerOnResetState(void);
        QuranPowerOnResetState();
#endif
        RtcTime.day = TM_DEFAULT_DATE_DAY;
        RtcTime.month = TM_DEFAULT_DATE_MONTH;
        RtcTime.year = TM_DEFAULT_DATE_YEAR-2000; //Change default year is 2017.
        RtcTime.wDay = TM_DEFAULT_DATE_WEEKDAY; // It's a Sunday.
        hal_TimRtcSetTime(&RtcTime);
        DM_SendArlarmMessage(&ev);//告诉MMI重设时间
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_PowerOn()==SET THE TIME PLEASE==\n",0x08000120));
    }
    if(hal_TimRtcGetTime(&RtcTime) != HAL_ERR_NO)
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_PowerOn);
        return FALSE;
    }
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC YEAR NOW IS ========0x%x",0x08000121), RtcTime.year + 2000);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC MONTH NOW IS ========0x%x",0x08000122), RtcTime.month);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC DAY NOW IS ========0x%x",0x08000123), RtcTime.day);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC DAY OF WEEK NOW IS ========0x%x",0x08000124), RtcTime.wDay);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC HOUR NOW IS ========0x%x",0x08000125), RtcTime.hour);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC MINUTES NOW IS ========0x%x",0x08000126), RtcTime.min);
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("RTC SECONDS NOW IS ========0x%x",0x08000127), RtcTime.sec);

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TM_PowerOn()==rtcProgram ==%d\n",0x08000128), rtcProgram);

    if(PowerOnInitDataFlag == FALSE)//判断是否初始化过
    {
        if(!TIM_PowerOnInitData(&RtcLostPower))//初试化
        {
            CSW_PROFILE_FUNCTION_EXIT(TM_PowerOn);
            return FALSE;
        }
        CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(1), TSTR("TM_PowerOn()==RtcLostPower %d==\n",0x08000129), RtcLostPower);

        if(RtcLostPower != 0)//FLASH有错误,发MMI消息
        {
            DM_SendArlarmMessage(&ev);
        }

        if(!TIM_ResetAlarmTime())//更新当前所有闹钟
        {
            return FALSE;
        }

        if(!TIM_SetFutureAlmRTC())//重新设置RTC闹钟
        {
            return FALSE;
        }
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070612
        if(!COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY),
                           PRV_TM_POWERON_ALM_TIMER_ID, COS_TIMER_MODE_SINGLE,
                           TIM_POWERON_WRITE_FLASH_TIMER))
        {
            return FALSE;
        }
#endif
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_PowerOn()==OK==\n",0x0800012a));
    CSW_PROFILE_FUNCTION_EXIT(TM_PowerOn);

    return TRUE;
}

/*
*Description:
*  when power off, call this function
*Parameter:
*  none
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TM_PowerOff(void)
{
    return TRUE;
}


/*
*Description:
*  use for change sysytem time or other system time changed
*situation.
*Parameter:
*  none
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/
BOOL TIM_ResetAlarm(VOID)
{
    if(!TIM_ResetAlarmTime())
    {
        return FALSE;
    }

    if(!TIM_SetFutureAlmRTC())
    {
        return FALSE;
    }
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070428
    TIM_DelOverdueAlarm();
#endif
    return TRUE;
}

//
// Reset the  "alarm"
// 维护和更新当前所有闹铃
PRIVATE BOOL TIM_ResetAlarmTime()
{
    UINT8 i = 0;
    TM_FILETIME   FileTime;
    UINT8 nDay = 0;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    TIM_ALARM_EX alarmEx, alarmEXZero;
    //UINT8 nDay = 0,writeFlashSwitch = TRUE;

    CSW_PROFILE_FUNCTION_ENTER(TIM_ResetAlarm);
    SUL_MemSet8((UINT8 *)(&alarmEXZero), 0, SIZEOF(TIM_ALARM_EX));

    // Find System time in seconds
    if(TRUE != TM_GetSystemFileTime(&FileTime))//取当前FILETIME
    {
        return FALSE;
    }

    // Get the index of the valid configured alarms.
    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))//得到所有闹铃信息
    {
        return FALSE;
    }

    // Find the day in the week.
    nDay   = TM_GetDayOfWeekEX(FileTime);//得到当前日期是星期几

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        if(alarmMainInfo.validIdxBit & (0x1 << i)) //判断此INDEX的闹铃是否存在
        {
            // Get the infos on the "i" alarm.
            if(!TIM_GetGloAlarmInfo(i, &alarmEx))
            {
                return FALSE;
            }

            // If it is not a repetive one.
            if((alarmEx.nRecurrent & 0x01) == 0x01)//删除单次闹铃
            {
                //One time alarm
#ifdef TIM_DEL_SINGLE_ALARM //yyj 070428
                // If the time of the alarm is passed, kill it.
                if((alarmEx.nFileTime <= FileTime.DateTime))
                {
                    if(TRUE != TIM_KillSingleAlmMem(alarmEx.nIndex))
                    {
                        return FALSE;
                    }
                }
#endif
            }
            else // If recurrent alarm.
            {
                TIM_UpdateAlarmTime (&alarmEx, FileTime.DateTime, nDay);

                if(!TIM_SetGloAlarmInfo(alarmEx.nIndex, &alarmEx))//把更新后的闹钟存入全局
                {
                    return FALSE;
                }
            }

        }//end if
    }//end for

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_ResetAlarmTime()==OK==\n",0x0800012b));

    return TRUE;
}

/*
*Description:
*  clear all the alarms and set a self define alarm.
*Parameter:
*  none
*Return:
*  BOOL type.TRUE is success,FALSE if fail.
*/

BOOL TM_ClearAllAlarm (VOID)
{
    TIM_ALARM_EX alarmEX;
    TIM_MAIN_ALARM_INFO alarmMainInfo;
    UINT8 i = 0;
    UINT8 *pBakBuf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(TM_ClearAllAlarm);

    SUL_MemSet8((UINT8 *)(&alarmEX), 0, SIZEOF(TIM_ALARM_EX));

    // Get the index of the valid configured alarms.
    if(!TIM_GetAllAlmIdxAndNmm(&alarmMainInfo))
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
        return FALSE;
    }


    pBakBuf = CSW_TIM_MALLOC(TIM_DATA_TOL_LEN);

    if(pBakBuf == NULL)
    {
        TIM_ERR_RETURN(TM_PARA_NULL_ERROR);
    }

    if(!TIM_ReadGloTolData(pBakBuf))
    {
        CSW_TIM_FREE(pBakBuf);
        return FALSE;
    }

    for(i = 1; i <= TIM_ALARM_INDEX_MAX; i++)
    {
        if(alarmMainInfo.validIdxBit & (0x1 << i))
        {
            if(!TIM_SetGloAlarmInfo(i, &alarmEX))
            {
                TIM_WriteGloTolData(pBakBuf);

                CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
                return FALSE;
            }

#ifdef TIM_REG_FFS_USED
            if(!TIM_WriteAlarmSet(i))
            {
                TIM_WriteGloTolData(pBakBuf);

                CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
                return FALSE;
            }
#else
            if(!TIM_WriteFlash())
            {
                TIM_WriteGloTolData(pBakBuf);
                CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
                return FALSE;
            }
#endif
        }
    }

    CSW_TIM_FREE(pBakBuf);// add 2006.8.25 chenwf

    if(!TIM_SetFutureAlmRTC())
    {
        CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
        return FALSE;
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TM_ClearAllAlarm()==OK==\n",0x0800012c));

    CSW_PROFILE_FUNCTION_EXIT(TM_ClearAllAlarm);
    return TRUE;
}


//======================================================================
// TIM_UpdateAlarmTime
//----------------------------------------------------------------------
/// This function is used to update the time of the alarm.
//
/// @param pAlarmEX    TIM_ALARM_EX* Input/Output Pointer to the alarm info structure
/// @param NowTime     UINT32        INPUT.       The now System's time in seconds.
/// @param TodayDay    UINT8         INPUT.       The today's day.

//======================================================================
PRIVATE VOID TIM_UpdateAlarmTime (TIM_ALARM_EX *pAlarmEX,  UINT32 NowTime, UINT8 TodayDay)
{

    // If it is an everyday alarm.
    if(   pAlarmEX->nRecurrent == ALARM_FOR_EVERY_DAY
            || pAlarmEX->nRecurrent == ALARM_ALL_WEEK_DAY)
    {

        // If the time of the alarm hasnt passed yet.
        if((pAlarmEX->nFileTime % ONE_DAY_SECONDS) > NowTime % ONE_DAY_SECONDS)
        {
            // Update the alarm time to ring in the current day.
            pAlarmEX->nFileTime = NowTime - NowTime % ONE_DAY_SECONDS \
                                  + pAlarmEX->nFileTime % ONE_DAY_SECONDS;
        }
        else
        {
            // Programm it to the next day.
            pAlarmEX->nFileTime = NowTime - NowTime % ONE_DAY_SECONDS \
                                  + pAlarmEX->nFileTime % ONE_DAY_SECONDS \
                                  + ONE_DAY_SECONDS;
        }
    }
    // If the alarm is configured to respond today.
    else if((pAlarmEX->nRecurrent & (1 << (TodayDay + 1))) != 0x00)
    {
        // If the time of the alarm hasnt passed yet.
        if((pAlarmEX->nFileTime % ONE_DAY_SECONDS) > NowTime % ONE_DAY_SECONDS)
        {
            // Update the alarm time to ring in the current day.
            pAlarmEX->nFileTime = NowTime - NowTime % ONE_DAY_SECONDS \
                                  + pAlarmEX->nFileTime % ONE_DAY_SECONDS;
        }
        else
        {
            UINT8 NextDay = 0;

            // Find next recurrent day and programm accordingly the alarm.
            TIM_CheckAlarmOfWeek(TodayDay, pAlarmEX->nRecurrent, &NextDay);

            pAlarmEX->nFileTime = NowTime - NowTime % ONE_DAY_SECONDS \
                                  + pAlarmEX->nFileTime % ONE_DAY_SECONDS \
                                  + NextDay * ONE_DAY_SECONDS;
        }
    }
    // If the alarm is not configured to respond today.
    else
    {
        UINT8 NextDay = 0;

        //Find next recurrent day and programm accordingly the alarm.
        TIM_CheckAlarmOfWeek(TodayDay, pAlarmEX->nRecurrent, &NextDay);

        pAlarmEX->nFileTime = NowTime - NowTime % ONE_DAY_SECONDS \
                              + pAlarmEX->nFileTime % ONE_DAY_SECONDS \
                              + NextDay * ONE_DAY_SECONDS;
    }

}



/*
*here, it is the debug function to change RTC
*/
#undef TM_MANUAL_CHANGE_DEBUG

#ifdef TM_MANUAL_CHANGE_DEBUG

/*for test::
*timeNow:the now time
*timeAlarm:the time should be responsing
*timesec:the desired secondtime to response from now(maybe need to estimate the time )
*        which programer want to wait so long?:)
*/
BOOL TIM_DbgManualUpRTC( TM_SYSTEMTIME *pTimeNow,
                         TM_SYSTEMTIME *pTimeAlarm,
                         UINT32 timeSec)
{
    TM_FILETIME nowSecontime;
    TM_FILETIME alarmSecontime;
    UINT32   timeToset = 0;

    TM_SystemTimeToFileTime (pTimeNow, &nowSecontime);
    TM_SystemTimeToFileTime (pTimeAlarm, &alarmSecontime);
    timeToset = alarmSecontime.DateTime - nowSecontime.DateTime - timeSec;

    HAL_TIM_RTC_TIME_T RtcTime;
    TM_SYSTEMTIME SystemTime;

    if(!TM_FileTimeToSystemTime((TM_FILETIME *)&timeToset, &SystemTime))
    {
        return FALSE;
    }

    RtcTime.year  = (UINT8)(SystemTime.uYear - 2000);
    RtcTime.month = SystemTime.uMonth ;
    RtcTime.wDay  = SystemTime.uDayOfWeek
                    RtcTime.day   = SystemTime.uDay ;
    RtcTime.hour  = SystemTime.uHour;
    RtcTime.min   = SystemTime.uMinute ;
    RtcTime.sec   = SystemTime.uSecond;

    if  (hal_TimRtcSetTime (&RtcTime) != HAL_ERR_NO)
    {
        return FALSE;
    }

    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DbgManualUpRTC()==OK==\n",0x0800012d));
    return TRUE;
}


/*only for test 190 limits:
*timesec:the desired secondtime to response from now(maybe need to estimate the time )
*        which programer want to wait so long?:)
*/
VOID TIM_DbgManualTest190RTC(UINT32 timeSec)
{
    UINT32 timeToset = 0;

    timeToset = RTC_ALARM_TIMERSET_LIMIT - timeSec;
    CSW_TRACE(_CSW|TLEVEL(DEBUG_PRINT_SWITCH)|TDB|TNB_ARG(0), TSTR("TIM_DbgManualTest190RTC()==OK==\n",0x0800012e));

    HAL_TIM_RTC_TIME_T RtcTime;
    TM_SYSTEMTIME SystemTime;

    if(!TM_FileTimeToSystemTime((TM_FILETIME *)&timeToset, &SystemTime))
    {
        return FALSE;
    }

    RtcTime.year  = (UINT8)(SystemTime.uYear - 2000);
    RtcTime.month = SystemTime.uMonth ;
    RtcTime.wDay  = SystemTime.uDayOfWeek
                    RtcTime.day   = SystemTime.uDay ;
    RtcTime.hour  = SystemTime.uHour;
    RtcTime.min   = SystemTime.uMinute ;
    RtcTime.sec   = SystemTime.uSecond;

    if  (hal_TimRtcSetTime (&RtcTime) != HAL_ERR_NO)
    {
        return FALSE;
    }
}

#endif

//END _C_


