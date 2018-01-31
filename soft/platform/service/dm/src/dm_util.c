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
#include <base.h>
#include "chip_id.h"
#include "cfw.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_key.h"
#include "hal_gpio.h"
#include "hal_host.h"

#include "csw_csp.h"
#include <cos.h>
#include <dm.h>
#include <base_prv.h>
#include <stdkey.h>
#include <tm.h>
#include <drv.h>
#include <ts.h>
#include <sul.h>

#include "pmd_m.h"

extern BOOL  Hal_GetRestoretoShutdownFlag(void);
extern void lcd_present_CS_log(void);
#if defined(COMPRESS_3264)||defined(COMPRESS_6464) ||defined(COMPRESS_3232) ||defined(COMPRESS_3232_NO_OVERLAY)
extern void lcd_log_and_Animation(void);
#endif

#define PWR_KEY_DEBOUNCE    3

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
extern VOID VDS_CacheFlushAll(VOID);
#endif

UINT32  PowerOn_Cause = DM_POWRN_ON_CAUSE_UNKOWN;
//
// Soft Reset system.
//
BOOL  DM_Reset(VOID)
{

    hal_HstSendEvent(0xaabb1212);
#if 0
#define REG_SYS_GLOBAL_BASE    0x00e00000
    volatile UINT32 *reg = ( *(volatile UINT32 *)((REG_SYS_GLOBAL_BASE + 0x04) | (0xa0000000)) );
    *reg |= 1 << 30;
#endif
    pmd_EnablePower(PMD_POWER_SDMMC, FALSE);

    hal_SoftReset();
    while(1)
        ; // only dump loop here.
    return TRUE;
}

extern BOOL TM_PowerOff(void);
/*wangrui add    2008.12.17 begin,  for fixbug10731*/
//extern UINT32 CFW_CfgSetSoftReset(UINT8 cause);
//extern UINT32 CFW_CfgGetSoftReset(UINT8 *cause);
/*wangrui add     2008.12.17 end*/
//
// Power Off.
//

// 1 mean charging power on than plug out adapter
// other reserved
UINT8 g_powerdown_cause = 0;
VOID DM_SetPowerDownCause(UINT8 cause)
{
    g_powerdown_cause = cause;
}
BOOL DM_DeviceSwithOff (BOOL bReset)
{
    UINT8 resetCause = 2;

    //Do deattch from NW
    CFW_NwDeRegisterEX(); 
    TM_PowerOff();
    pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
#ifdef CHARGERING_CANNOT_POWEROFF
    switch(pmd_GetChargerStatus())
    {
        case PMD_CHARGER_UNKNOWN           :
        case PMD_CHARGER_UNPLUGGED         :
            resetCause = 2;
            break;
        case PMD_CHARGER_PLUGGED           :
        case PMD_CHARGER_PRECHARGE         :
        case PMD_CHARGER_FAST_CHARGE       :
        case PMD_CHARGER_PULSED_CHARGE     :
        case PMD_CHARGER_PULSED_CHARGE_WAIT:
        case PMD_CHARGER_FULL_CHARGE       :
        case PMD_CHARGER_ERROR             :
        case PMD_CHARGER_TIMED_OUT         :
        case PMD_CHARGER_TEMPERATURE_ERROR :
        case PMD_CHARGER_VOLTAGE_ERROR     :
        default                            :
            resetCause = 1;
            break;
    }
#endif

    if (g_powerdown_cause == 1)
    {
        CFW_CfgSetSoftReset(3);
    }
    else
    {
        CFW_CfgSetSoftReset(resetCause);
    }
    g_powerdown_cause = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    VDS_CacheFlushAll();
#endif

    if(TRUE == bReset)
    {
        DM_Reset();
        return TRUE;
    }
#ifdef CHARGERING_CANNOT_POWEROFF
    if (resetCause == 1)
    {
        DM_Reset();
    }
    else
#endif
    {
        hal_HstSendEvent(0xdeadd0ff);
        hal_ShutDown();
    }

    return TRUE;
}

//////////////////////////disable watchdog///////
BOOL DM_DisableWatchDog()
{
    hal_timWatchDog_Disable();
    return TRUE;
}
BOOL DM_SetWathcDogTime(UINT32 time)
{
    hal_timWatchDog_Start(time);

    return TRUE;
}
BOOL DM_SetWathcDogKeepAlive()
{
    hal_timWatchDog_KeepAlive();
    return TRUE;

}
extern UINT32 DM_AlarmDisposeInEvt( UINT8 nCause, COS_EVENT *pEvent) ;
extern BOOL DM_KeyboardInit();
extern BOOL PM_PowerOnChck(void);
extern BOOL DRV_RegSetDbgVal(UINT16 nVal);
extern UINT32 CFW_ShellControl(UINT8 nCommand);


#ifndef CHIP_HAS_AP

VOID DM_CheckPowerOnCause(VOID)
{
#ifdef PMD_TWO_BATTARY
    UINT32 i = 0;
    UINT32 keytime = 0;
#endif
    UINT8 POSSIBLY_UNUSED Causeflag;
    HAL_SYS_RESET_CAUSE_T resetCause;

    resetCause = hal_SysGetResetCause();
#if defined(__RESTORE_RESET_NORMAL_POWERON_MODE__)
    if (Hal_GetRestoretoShutdownFlag())
    {
        resetCause = HAL_SYS_RESET_CAUSE_NORMAL;
    }

#endif

    hal_HstSendEvent(0xFFFFFF00 | resetCause);

    switch (resetCause)
    {
        case HAL_SYS_RESET_CAUSE_NORMAL:

#ifndef PMD_TWO_BATTARY
#if defined(__RESTORE_RESET_NORMAL_POWERON_MODE__)
            if ((Hal_GetRestoretoShutdownFlag()) || (hal_KeyOnOffStateAtPowerOn()))
#else
            if (hal_KeyOnOffStateAtPowerOn())
#endif
            {
                PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
#if defined(COMPRESS_3264)||defined(COMPRESS_6464)  ||defined(COMPRESS_3232) ||defined(COMPRESS_3232_NO_OVERLAY)
                lcd_log_and_Animation();
#endif
            }
            else
            {
                switch(pmd_GetChargerStatus())
                {
                    case PMD_CHARGER_UNKNOWN           :
                    case PMD_CHARGER_UNPLUGGED         :
                        hal_HstSendEvent(0xdeadca65);
#if defined(__RESTORE_RESET_NORMAL_POWERON_MODE__)
                        if (Hal_GetRestoretoShutdownFlag())
                        {
                            PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
                        }
                        else
#endif
                            hal_ShutDown();
                        break;
                    case PMD_CHARGER_PLUGGED           :
                    case PMD_CHARGER_PRECHARGE         :
                    case PMD_CHARGER_FAST_CHARGE       :
                    case PMD_CHARGER_PULSED_CHARGE     :
                    case PMD_CHARGER_PULSED_CHARGE_WAIT:
                    case PMD_CHARGER_FULL_CHARGE       :
                    case PMD_CHARGER_ERROR             :
                    case PMD_CHARGER_TIMED_OUT         :
                    case PMD_CHARGER_TEMPERATURE_ERROR :
                    case PMD_CHARGER_VOLTAGE_ERROR     :
                    default:
#ifdef GCF_TEST
                        PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
#else
                        PowerOn_Cause = DM_POWRN_ON_CAUSE_CHARGE;
#endif

#if defined(COMPRESS_3264)||defined(COMPRESS_6464) ||defined(COMPRESS_3232) ||defined(COMPRESS_3232_NO_OVERLAY)
                        lcd_log_and_Animation();
#endif
                        break;
                }
            }

#else // PMD_TWO_BATTARY

            for ( i = 0; i < 2000; i++)
            {
                if(i == 1999)
                {
                    hal_HstSendEvent(0xdead03e9);
                    hal_ShutDown();
                }
                if (hal_KeyOnOffStateAtPowerOn())
                {
                    if (i == 0)
                    {
                        PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
                        i = 2000;
                    }
                    else
                    {
                        keytime ++;
                    }
                    if (keytime > 400)
                    {
                        PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
                        i = 2000;
                    }

                }
                else
                {
                    keytime = 0;
                    switch(pmd_GetChargerStatus())
                    {
                        case PMD_CHARGER_UNKNOWN           :
                        case PMD_CHARGER_UNPLUGGED         :
                            break;
                        case PMD_CHARGER_PLUGGED           :
                        case PMD_CHARGER_PRECHARGE         :
                        case PMD_CHARGER_FAST_CHARGE       :
                        case PMD_CHARGER_PULSED_CHARGE     :
                        case PMD_CHARGER_PULSED_CHARGE_WAIT:
                        case PMD_CHARGER_FULL_CHARGE       :
                        case PMD_CHARGER_ERROR             :
                        case PMD_CHARGER_TIMED_OUT         :
                        case PMD_CHARGER_TEMPERATURE_ERROR :
                        case PMD_CHARGER_VOLTAGE_ERROR     :
                        default                            :
                            PowerOn_Cause = DM_POWRN_ON_CAUSE_CHARGE;
                            i = 2000;
                            break;
                    }
                }
                hal_TimDelay(5 MS_WAITING);
            }

#endif // PMD_TWO_BATTARY

            break;

        case HAL_SYS_RESET_CAUSE_ALARM:
            PowerOn_Cause = DM_POWRN_ON_CAUSE_ALARM;
            break;

        case HAL_SYS_RESET_CAUSE_RESTART:
        case HAL_SYS_RESET_CAUSE_HOST_DEBUG:

            /*wangrui add    2008.12.17 begin,  for fixbug10731*/
#ifdef CHARGERING_CANNOT_POWEROFF
            CFW_CfgGetSoftReset(&Causeflag);
            if((Causeflag == 1) &&
                    (pmd_GetChargerStatus() != PMD_CHARGER_UNPLUGGED))
            {
                PowerOn_Cause = DM_POWRN_ON_CAUSE_CHARGE;
            }
            else
                PowerOn_Cause = DM_POWRN_ON_CAUSE_RESET;
            if(Causeflag == 2)
            {
                PowerOn_Cause = DM_POWRN_ON_CAUSE_KEY;
            }
            //if((Causeflag == 3)&&(pmd_GetChargerStatus() != PMD_CHARGER_UNPLUGGED))
            if( Causeflag == 3)
            {
                PowerOn_Cause = DM_POWRN_ON_CAUSE_CHARGE;
            }
#endif
            /*wangrui add    2008.12.17  end*/
            switch(pmd_GetChargerStatus())
            {
                case PMD_CHARGER_PLUGGED           :
                case PMD_CHARGER_PRECHARGE         :
                case PMD_CHARGER_FAST_CHARGE       :
                case PMD_CHARGER_PULSED_CHARGE     :
                case PMD_CHARGER_PULSED_CHARGE_WAIT:
                case PMD_CHARGER_FULL_CHARGE       :
                case PMD_CHARGER_ERROR             :
                case PMD_CHARGER_TIMED_OUT         :
                case PMD_CHARGER_TEMPERATURE_ERROR :
                case PMD_CHARGER_VOLTAGE_ERROR     :
                    PowerOn_Cause = DM_POWRN_ON_CAUSE_CHARGE;
#if defined(COMPRESS_3264)||defined(COMPRESS_6464) ||defined(COMPRESS_3232) ||defined(COMPRESS_3232_NO_OVERLAY)
                    lcd_log_and_Animation();
#endif
                    break;
                case PMD_CHARGER_UNKNOWN           :
                case PMD_CHARGER_UNPLUGGED         :
                default                            :
                    PowerOn_Cause = DM_POWRN_ON_CAUSE_RESET;
                    break;
            }

            break;

        case HAL_SYS_RESET_CAUSE_WATCHDOG:
        default:
            PowerOn_Cause = DM_POWRN_ON_CAUSE_EXCEPTION;
            break;
    }
}

#endif // !CHIP_HAS_AP


BOOL DM_PowerOnCheck(VOID)
{
    COS_EVENT ev;
    SUL_ZeroMemory32( (VOID *)&ev, SIZEOF(COS_EVENT) );

    ev.nEventId = EV_DM_POWER_ON_IND;

#ifdef CHIP_HAS_AP

    ev.nParam1 = DM_POWRN_ON_CAUSE_UNKOWN;
    DM_SendPowerOnMessage(&ev);

#else // !CHIP_HAS_AP

    BOOL bRet;
    UINT8 POSSIBLY_UNUSED Causeflag;

    bRet = TRUE;

    DM_CheckPowerOnCause();
    ev.nParam1 = PowerOn_Cause;

#ifdef CHARGERING_CANNOT_POWEROFF
    CFW_CfgGetSoftReset(&Causeflag);
    if (Causeflag > 0)
    {
        CFW_CfgSetSoftReset(0);
    }
#endif

#ifdef __221_UNIT_TEST__
    CFW_ShellControl(CFW_CONTROL_CMD_POWER_ON);
#endif

    ev.nEventId = EV_DM_POWER_ON_IND;
    ev.nParam1 = PowerOn_Cause;

    if (PowerOn_Cause & DM_POWRN_ON_CAUSE_ALARM)
    {
#if HAVE_TM_MODULE==1
        DM_AlarmDisposeInEvt(PowerOn_Cause, &ev);
#endif
        TM_ALARM *p = NULL;
        p = (TM_ALARM *)ev.nParam3;
        if ( p && (p->nIndex == 17) )
        {
        }
    }
    if (PowerOn_Cause & DM_POWRN_ON_CAUSE_CHARGE)
    {
        //DM_KeyboardInit() ;
    }

#if defined(__MMI_R_WATCH__) || defined(__MMI_R_WATCH_KIDS__)
#if defined(TOUCH_PANEL_SUPPORT)
	tsd_Init();
#endif
#endif

    DM_SendPowerOnMessage(&ev);

#endif // !CHIP_HAS_AP

    return TRUE;
}

BOOL DM_Delay(UINT32 nSec)
{
    hal_TimDelay(nSec * HAL_TICK1S);
    return TRUE;
}


// Very basic delay for non-critical timing
// This will wait n/256 sec +/- 1/256 sec, so
// for a delay of at _least_ m/256, use (m+1)

BOOL DM_Wait(UINT32 nTick)
{
    hal_TimDelay(nTick);
    return TRUE;
}

