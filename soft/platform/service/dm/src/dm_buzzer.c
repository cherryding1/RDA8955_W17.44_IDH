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


#include "base_prv.h"
#include "drv.h"
#include "errorcode.h"
#include "chip_id.h"
#include "hal_pwm.h"
#include "csw_csp.h"
#include "csw_mem_prv.h"
#include "ts.h"
#include "cswtype.h"
#include "dm.h"

#ifdef USE_BUZZER

DM_BUZZER_SET g_BUZZER_set = DM_BUZZER_SET_EN;
DM_BUZZER_VIBRATING g_VIBRATING_state;
DM_BUZZER_FLASH g_FLASH_state;

extern UINT8 uLcdLevel;
extern UINT8 uKeyLevel;

UINT8 GPIO_VIBRATING = 0xFF; //Set No Vibrating function

//#define PAL_TICK1S      16384       // 16384 ticks/s
//#define MILLI_SECOND    * PAL_TICK1S /100
#define BZ_DELAY        *16384/100
//define for the callback function type
BUZZERCALLBACK gmycallback;

// buzzer play state
typedef enum
{
    BUZZER_PLAYING,
    BUZZER_IDLE
} BUZZER_STATE;

BUZZER_STATE Playstate = BUZZER_PLAYING;

UINT8 BzVolume = 25;

UINT32 PlayLenTemp = 0;
UINT32 PlayLen;
UINT32 BzDuration;
UINT8 *BzBuffer;
UINT8 BzPlayState = 0;

// FIXME This table might not be identical for
// Jade and Granite based chips, to investigate.
UINT16 DM_Note_Table[48] =
{
    1556,// 261,  //C
    1472,// 276,  //
    1386,// 293,  //D
    1310,// 310,  //
    1235,// 329,  //E
    1167,// 348,  //F
    1101,// 369,  //
    1039,// 391,  //G
    981, // 414,  //
    925, // 439,  //A
    874, // 465,  //
    826, // 492,  //B
    778, // 522,  //C
    736, // 552,  //
    693, // 586,  //D
    655, // 620,  //
    617, // 658,  //E
    584, // 696,  //F
    550, // 738,  //
    519, // 782,  //G
    491, // 828,  //
    463, // 878,  //A
    437, // 930,  //
    413, // 984,  //B
    389, // 1044, //C
    368, // 1104, //
    347, // 1172, //D
    327, // 1240, //
    309, // 1316, //E
    292, // 1392, //F
    275, // 1476, //
    260, // 1564, //G
    245, // 1656, //
    231, // 1756, //A
    218, // 1860, //
    206, // 1968, //B
    194, // 2088, //C
    184, // 2208, //
    173, // 2344, //D
    164, // 2480, //
    154, // 2632, //E
    146, // 2784, //F
    137, // 2952, //
    130, // 3128, //G
    123, // 3312, //
    116, // 3512, //A
    109, // 3720, //
    103  // 3936  //B
};

// delay N mS
VOID DM_BuzzerDelayNmS(UINT32 nMillisecondes)
{
    COS_Sleep(nMillisecondes);
}

// open buzzer device, PWT and output
UINT8 DM_BuzzerOpen(VOID)
{
    DRV_GpioSetMode(DM_DEVID_GPIO_20, 0);
    return(ERR_SUCCESS);
}

// stop buzzer playing
UINT8 DM_BuzzerStop(VOID)
{
    hal_pwt_Stop();
    DRV_GpioSetMode(DM_DEVID_GPIO_20, 1);
    DRV_GpioSetDirection(DM_DEVID_GPIO_20, DRV_GPIO_OUT);
    DRV_GpioSetLevel(DM_DEVID_GPIO_20, DRV_GPIO_LOW);
    return(ERR_SUCCESS);
}

UINT8 DM_BuzzerPlay(DM_PWT_OCT Oct, DM_PWT_NOTE Note, UINT8 Vol)
{
    UINT16 A, B;
    UINT8 tempPWL;
    if(Oct > DM_PWT_OCTS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("the wrong type buzzer OCTS! \n",0x08200001)));
        return(ERR_INVALID_PARAMETER);
    }
    if(Note > DM_PWT_NOTES)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("the wrong type buzzer NOTES! \n",0x08200002)));
        return(ERR_INVALID_PARAMETER);
    }
    if(Vol > 50)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("the wrong type buzzer VOLS! \n",0x08200003)));
        return(ERR_INVALID_PARAMETER);
    }

    //add PWL FLASH part, Longman 2006.12.04
    tempPWL = (Oct * 60) + (Note * 5) + 10; //tempPWL= 10-245
    if(g_FLASH_state == DM_BUZZER_FLASH_KEY)
    {
        hal_pwl1_Threshold(tempPWL); //KEY_LIGHTON
    }
    if(g_FLASH_state == DM_BUZZER_FLASH_LCD)
    {
        hal_pwl0_Threshold(tempPWL); //LCD_LIGHTON
    }
    if(g_FLASH_state == DM_BUZZER_FLASH_BOTH)
    {
        hal_pwl1_Threshold(tempPWL); //KEY_LIGHTON
        hal_pwl0_Threshold(tempPWL); //LCD_LIGHTON
    }

    if(Oct == DM_PWT_OCTS)
    {
        DM_BuzzerStop();
        return(ERR_SUCCESS);
    }
    if(Note == DM_PWT_NOTES)
    {
        DM_BuzzerStop();
        return(ERR_SUCCESS);
    }

    if(g_BUZZER_set == DM_BUZZER_SET_IDLE)
    {
        DM_BuzzerStop();
        return(ERR_SUCCESS);
    }
    if(g_BUZZER_set == DM_BUZZER_SET_EN)
    {
        A = (DM_Note_Table[12 * Oct + Note]);
        if(Vol <= 50)
        {
            if(0 == Vol)
                B = 0;
            else
                B = (A * Vol) / 100;
        }
        CSW_TRACE(_CSW|TLEVEL(100)|TDB|TNB_ARG(2), TSTXT(TSTR("                    A: %d B: %d \n",0x08200004)), A, B);
        DM_BuzzerOpen(); // add 12.18
        hal_pwt_Start(A, B);
        return(ERR_SUCCESS);
    }
    return ERR_SUCCESS;
}

// Set the buzzer volume for the buzzer data playing.
UINT8 DM_BuzzerVolumeSet(UINT8 Vol)
{
    if(Vol > 50)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("the wrong type buzzer VOLS! \n",0x08200005)));
        return(ERR_INVALID_PARAMETER);
    }
    BzVolume = Vol;
    return(ERR_SUCCESS);
}
// Set the buzzer Vibrating gpio
UINT8 DM_BuzzerVibratingSet(UINT8 nGpio)
{
    if(nGpio > DRV_GPIO_31)
    {
        CSW_TRACE(_CSW|TLEVEL(100)|TDB|TNB_ARG(0), TSTXT(TSTR("INVALID GPIO parameter! \n",0x08200006)));
        return FALSE;
    }
    GPIO_VIBRATING = nGpio;
    return(TRUE);
}

UINT8 DM_BuzzerVibratingOn(VOID)
{
    if(GPIO_VIBRATING == 0xFF)
    {
        CSW_TRACE(_CSW|TLEVEL(100)|TDB|TNB_ARG(0), TSTXT(TSTR("No Vibrating function! \n",0x08200007)));
        return FALSE;
    }
    DRV_GpioSetMode(GPIO_VIBRATING, 1);
    DRV_GpioSetDirection(GPIO_VIBRATING, DRV_GPIO_OUT);
    DRV_GpioSetLevel(GPIO_VIBRATING, DRV_GPIO_HIGH);
    return(TRUE);
}

UINT8 DM_BuzzerVibratingOff(VOID)
{
    if(GPIO_VIBRATING == 0xFF)
    {
        CSW_TRACE(_CSW|TLEVEL(100)|TDB|TNB_ARG(0), TSTXT(TSTR("No Vibrating function! \n",0x08200008)));
        return FALSE;
    }
    DRV_GpioSetMode(GPIO_VIBRATING, 1);
    DRV_GpioSetDirection(GPIO_VIBRATING, DRV_GPIO_OUT);
    DRV_GpioSetLevel(GPIO_VIBRATING, DRV_GPIO_LOW);
    return(TRUE);
}

// The buzzer play song file data
UINT8 DM_BuzzerSongStart(UINT8 *pBuffer, UINT32 nBufferLen, UINT32 duration, DM_BUZZER_SET Btemp, DM_BUZZER_VIBRATING Vtemp, DM_BUZZER_FLASH Ftemp, BUZZERCALLBACK pbuzzer_callback)
{
    COS_EVENT  mybuzzer;
    if(pBuffer == (void *)0)
    {
        return(ERR_INVALID_PARAMETER);
    }
    if(nBufferLen == 0)
    {
        return(ERR_INVALID_PARAMETER);
    }
    if(Btemp > DM_BUZZER_SET_EN)
    {
        return(ERR_INVALID_PARAMETER);
    }
    if(Vtemp > DM_BUZZER_VIBRATING_EN)
    {
        return(ERR_INVALID_PARAMETER);
    }
    if(Ftemp > DM_BUZZER_FLASH_BOTH)
    {
        return(ERR_INVALID_PARAMETER);
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("buzzer song START \n",0x08200009)));
    CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(5), TSTXT(TSTR("nBufferLen: %d,  duration: %d,  Btemp: %d,  Vtemp: %d,  Ftemp: %d\n",0x0820000a)), nBufferLen, duration, Btemp, Vtemp, Ftemp);

    mybuzzer.nEventId = SYS_EV_MASK_BUZZER_FILE;
    mybuzzer.nParam1 = (UINT32)pBuffer;
    mybuzzer.nParam2 = nBufferLen >> 2; //PlayLen
    mybuzzer.nParam3 = duration;

    g_BUZZER_set = Btemp;
    g_VIBRATING_state = Vtemp;
    g_FLASH_state = Ftemp;

    // if other song is playing, so first stopping
    if(BzPlayState == 1)
    {
        DM_BuzzerStop();
        COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID);
    }

    COS_SendEvent(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), &mybuzzer, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    BzPlayState = 1;    //Set buzzer song play state

    gmycallback = pbuzzer_callback;

    return(ERR_SUCCESS);
}

// add this function up to the bal_tasks.c
UINT8 DM_BuzzerSongMsg( COS_EVENT *pEvent)
{
    if(pEvent->nEventId == SYS_EV_MASK_BUZZER_FILE)
    {
        BzBuffer = (UINT8 *)pEvent->nParam1;
        PlayLen = pEvent->nParam2;
        BzDuration = pEvent->nParam3;

        Playstate = BUZZER_PLAYING;
        PlayLenTemp = 0;
        COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID, COS_TIMER_MODE_SINGLE, 100);
        return(ERR_CONTINUE);
    }
    else if(pEvent->nParam1 == PRV_DM_BUZZER_TIMER_ID)
    {
        if(PlayLenTemp < PlayLen)
        {
            if(Playstate == BUZZER_PLAYING)
            {
                if(g_VIBRATING_state == DM_BUZZER_VIBRATING_EN)
                {
                    //add GPIO Vibrating part, Longman 2006.12.04
                    if((PlayLenTemp % 2) == 1)
                    {
                        DM_BuzzerVibratingOff();
                    }
                    else
                    {
                        if(DM_PWT_OCTS == BzBuffer[(PlayLenTemp << 2)])
                        {
                            DM_BuzzerVibratingOff();
                        }
                        else
                        {
                            DM_BuzzerVibratingOn();
                        }
                    }
                }

                DM_BuzzerPlay(BzBuffer[PlayLenTemp << 2], BzBuffer[(PlayLenTemp << 2) + 1], BzVolume);
                if(ERR_INVALID_PARAMETER == DM_BuzzerPlay(BzBuffer[PlayLenTemp << 2], BzBuffer[(PlayLenTemp << 2) + 1], BzVolume))
                {
                    return(ERR_INVALID_PARAMETER);
                }
                Playstate = BUZZER_IDLE;
                COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID, COS_TIMER_MODE_SINGLE, ((BzBuffer[(PlayLenTemp << 2) + 2])BZ_DELAY));
                return(ERR_CONTINUE);
            }
            if(Playstate == BUZZER_IDLE)
            {
                DM_BuzzerStop();
                hal_pwl0_Threshold(0);
                hal_pwl1_Threshold(0);

                COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID, COS_TIMER_MODE_SINGLE, ((BzBuffer[(PlayLenTemp << 2) + 3])BZ_DELAY));
                Playstate = BUZZER_PLAYING;
                PlayLenTemp++;
                return(ERR_CONTINUE);
            }
        }
        else
        {
            BzDuration--;
            if(BzDuration > 0)
            {
                PlayLenTemp = 0;
                Playstate = BUZZER_PLAYING;
                COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID, COS_TIMER_MODE_SINGLE, 16384);
                return(ERR_CONTINUE);
            }
        }
    }
    BzPlayState = 0;    //clean buzzer song play state

    DM_BuzzerStop();
    hal_pwl0_Threshold(uLcdLevel); //release the former LCD backlight
    hal_pwl1_Threshold(uKeyLevel); //release the former KEY backlight
    DM_BuzzerVibratingOff(); //set vibrating function off at any case.
    DM_BuzzerDelayNmS(100); //add 070316 wangzw

    CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("buzzer song FINISH \n",0x0870000b)));
    if(gmycallback != NULL)
    {
        gmycallback();
    }
    return(ERR_SUCCESS);
}

// using a key can stop the song playing
UINT8 DM_BuzzerSongStop(VOID)
{
    // if other song is playing, so first stopping
    if(BzPlayState == 1)
    {
        COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_DM_BUZZER_TIMER_ID);
    }

    DM_BuzzerStop();
    hal_pwl0_Threshold(uLcdLevel); //release the former LCD backlight
    hal_pwl1_Threshold(uKeyLevel); //release the former KEY backlight
    DM_BuzzerVibratingOff(); //set vibrating function off at any case.
    CSW_TRACE(_CSW|TLEVEL(CFW_DM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("buzzer song STOP \n",0x0870000c)));
    DM_BuzzerDelayNmS(100); //add 070316 wangzw

    gmycallback = NULL;

    return(ERR_SUCCESS);
}
#endif
