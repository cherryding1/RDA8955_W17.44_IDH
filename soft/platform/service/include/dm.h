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























#ifndef __DM_H__
#define __DM_H__

#include <cos.h>
#include "hal_sys.h"

#ifndef CHIP_HAS_AP
#define USE_BUZZER
#endif

//
// nPlayCount
//
#define  DM_SND_REPEAT_ONE       1         //DM use this value by default if you use a invalid count. 
#define  DM_SND_REPEAT_INFINITE  0x7FFFFFFF //DM use this value as loop for ever. 

//
// nOption
//

#define DM_SND_OPTION_INTERRUPT  0x1  //Interrupt any currently playing sound and play a new one. By default, this option will be applied for all sound player. 
#define DM_SND_OPTION_PRIORITY   0x2  //Interrupts another, currently playing, sound if the new sound has a higher priority than the currently playing sound. 
#define DM_SND_OPTION_ASYNC      0x4  //Play sound asynchronously. 
#define DM_SND_OPTION_SYNC       0x8  //Play sound synchronously. Not supported currently 
#define DM_SND_OPTION_PRI_HIGH   0x10 //Play sound in high priority. 
#define DM_SND_OPTION_PRI_LOW    0x20 //Play sound in low priority. 

enum LCDType
{
    Coolsand_MAIN_LCD,
    Coolsand_SUB_LCD
};


// =============================================================================
// CSW_LP_RESOURCE_T
// -----------------------------------------------------------------------------
/// Describes the possible users for resource management. For those users,
/// and only through that type, can a system frequency be chosen.
// =============================================================================
typedef enum
{
    CSW_LP_RESOURCE_LCDBACKLIGHT = 0,
    CSW_LP_RESOURCE_KEYBACKLIGHT,
    CSW_LP_RESOURCE_MMIACTIVE,
    CSW_LP_RESOURCE_CAMERA,
    CSW_LP_RESOURCE_AUDIO_PLAYER,
    CSW_LP_RESOURCE_VIDEO_PLAYER,
    CSW_LP_RESOURCE_PROFILE_ACTIVE,
    CSW_LP_RESOURCE_CSW,
    CSW_LP_RESOURCE_UNUSED_1,    
    CSW_LP_RESOURCE_UNUSED_2,
    CSW_LP_RESOURCE_QTY
} CSW_LP_RESOURCE_T;


// =============================================================================
// CSW_SYS_FREQ_T
// -----------------------------------------------------------------------------
/// That type defines all the possible frequencies for the system clock.
/// This type is used to define the minimum system frequency required
/// by a resource selected among #CSW_LP_RESOURCE_T.
// =============================================================================
typedef enum
{
    CSW_SYS_FREQ_32K    = 32768,
    CSW_SYS_FREQ_13M    = 13000000,
    CSW_SYS_FREQ_26M    = 26000000,
    CSW_SYS_FREQ_39M    = 39000000,
    CSW_SYS_FREQ_52M    = 52000000,
    CSW_SYS_FREQ_78M    = 78000000,
    CSW_SYS_FREQ_104M   = 104000000,
    CSW_SYS_FREQ_139M   = 138666667,
    CSW_SYS_FREQ_156M   = 156000000,
    CSW_SYS_FREQ_178M    = 178285714, //624M/3.5
    CSW_SYS_FREQ_208M   = 208000000,
    CSW_SYS_FREQ_250M   = 249600000,
    CSW_SYS_FREQ_312M   = 312000000,
} CSW_SYS_FREQ_T;

typedef enum
{
    CSW_VOC_FREQ_13M    = 13000000,
    CSW_VOC_FREQ_26M    = 26000000,
    CSW_VOC_FREQ_39M    = 39000000,
    CSW_VOC_FREQ_52M    = 52000000,
    CSW_VOC_FREQ_78M    = 78000000,
    CSW_VOC_FREQ_104M   = 104000000,
    CSW_VOC_FREQ_156M   = 156000000,
    CSW_VOC_FREQ_208M   = 208000000,
    CSW_VOC_FREQ_250M   = 249600000,
    CSW_VOC_FREQ_312M   = 312000000
} CSW_VOC_FREQ_T;

// =============================================================================
// csw_SetResourceActivity
// -----------------------------------------------------------------------------
/// Require a minimum system frequency for a given resource.
/// To release the resource, allowing for lowpower, you have to require a
/// frequenct of 32kHz, ie: #CSW_SYS_FREQ_32K.
/// Any other requirement will take the resource.
///
/// @param resource Resource that requests a minimum system frequency.
/// @param freq Minimum system frequency required. The actual system fequency
/// may be faster.
// =============================================================================
PUBLIC VOID csw_SetResourceActivity(CSW_LP_RESOURCE_T resource, CSW_SYS_FREQ_T freq);
PUBLIC VOID csw_SetVOCFreqActivity(CSW_VOC_FREQ_T freq);


VOID DM_KeyBackLightOpen();
VOID DM_KeyBackLightClose();
VOID DM_LpwMmiActive();
VOID DM_LpwMmiInactive();
VOID DM_LpwLCDActive();
VOID DM_LpwLCDInactive();
VOID DM_LpwDeviceActive();
VOID DM_LpwDeviceInactive();

VOID DM_ClkOut_Enable(BOOL bEnable);


typedef struct _SFW_VB_DATA
{
    UINT16 nDuration;
    UINT8 nAmplitude;
    UINT8 nFrequency;
} DM_VB_DATA;

typedef struct _DM_VB_SONG
{
    DM_VB_DATA *pVibrateDataArray;
    UINT8 nArraySize;
    BOOL bRepeat;
} DM_VB_SONG;



BOOL SRVAPI DM_Reset(
    VOID
);


// 1 mean charging power on than plug out adapter
// other reserved

VOID DM_SetPowerDownCause(UINT8 cause);

BOOL SRVAPI DM_DeviceSwithOff (BOOL bReset);


#define DM_POWRN_ON_CAUSE_KEY         0x01
#define DM_POWRN_ON_CAUSE_CHARGE      0x02
#define DM_POWRN_ON_CAUSE_ALARM       0x04
#define DM_POWRN_ON_CAUSE_EXCEPTION   0x08
#define DM_POWRN_ON_CAUSE_RESET       0x10
#define DM_POWRN_ON_CAUSE_UNKOWN      0x80

#define DM_STATUS_FLASH_INIT          0x01

typedef VOID (*PFN_DEVICE_HANDLER)(COS_EVENT *);

#define DM_GPIO(n)             (1<<(n))

#define DM_DEVID_GPIO_0        DRV_GPIO_0
#define DM_DEVID_GPIO_1        DRV_GPIO_1
#define DM_DEVID_GPIO_2        DRV_GPIO_2
#define DM_DEVID_GPIO_6        DRV_GPIO_6
#define DM_DEVID_GPIO_7        DRV_GPIO_7
#define DM_DEVID_GPIO_8        DRV_GPIO_8
#define DM_DEVID_GPIO_9        DRV_GPIO_9 //reserved for power off/on key
#define DM_DEVID_GPIO_10       DRV_GPIO_10
#define DM_DEVID_GPIO_12       DRV_GPIO_12
#define DM_DEVID_GPIO_13       DRV_GPIO_13
#define DM_DEVID_GPIO_14       DRV_GPIO_14
#define DM_DEVID_GPIO_15       DRV_GPIO_15
#define DM_DEVID_GPIO_16       DRV_GPIO_16
#define DM_DEVID_GPIO_17       DRV_GPIO_17
#define DM_DEVID_GPIO_20       DRV_GPIO_20
#define DM_DEVID_GPIO_21       DRV_GPIO_21
#define DM_DEVID_GPIO_22       DRV_GPIO_22
#define DM_DEVID_GPIO_23       DRV_GPIO_23
#define DM_DEVID_GPIO_24       DRV_GPIO_24
#define DM_DEVID_GPIO_25       DRV_GPIO_25
#define DM_DEVID_GPIO_26       DRV_GPIO_26
#define DM_DEVID_GPIO_27       DRV_GPIO_27
#define DM_DEVID_GPIO_28       DRV_GPIO_28
#define DM_DEVID_GPIO_29       DRV_GPIO_29
#define DM_DEVID_GPIO_30       DRV_GPIO_30
#define DM_DEVID_GPIO_31       DRV_GPIO_31

#define DM_DEVID_EXTI          (32)
#define DM_DEVID_POWER_ON      (33)
#define DM_DEVID_KEY           (34)
#define DM_DEVID_ALARM         (35)
#define DM_DEVID_PM            (36)
#define DM_DEVID_DM_STATUS     (37)
#define DM_DEVID_GPADC         (38)
#define DM_DEVID_RECORD_END (39)
#define DM_DEVID_STREAM_END (40)
/*zj mod*/
#define DM_DEVID_SPI      (41)
#define DM_DEVID_END_          (41)

BOOL SRVAPI DM_RegisterHandler(
    UINT8 nDeviceId,
    PFN_DEVICE_HANDLER pfnDevHandler
);

//
// UART
//
typedef BOOL (*PFN_DM_UART_GET_DATA)(UINT8 *pUartBuffer, UINT8 nLen);

//
// Others API to support.
//
BOOL DM_Wait(UINT32 nTick);     // Wait in 1/256 seconds step.
BOOL DM_Delay(UINT32 nSeconds); // Wait in 1 seconds step.
BOOL DM_DisableWatchDog();
BOOL DM_SetWathcDogTime(UINT32 time);
BOOL DM_SetWathcDogKeepAlive();
#ifdef USE_BUZZER
//
// PWT Buzzer @Longman
//

// octave
typedef enum
{
    DM_PWT_OCT_0, //no use
    DM_PWT_OCT_1,
    DM_PWT_OCT_2,
    DM_PWT_OCT_3,
    DM_PWT_OCTS
} DM_PWT_OCT;

// note
typedef enum
{
    DM_PWT_NOTE_C,      // 0
    DM_PWT_NOTE_CS,     // 1
    DM_PWT_NOTE_D,      // 2
    DM_PWT_NOTE_DS,     // 3
    DM_PWT_NOTE_E,      // 4
    DM_PWT_NOTE_F,      // 5
    DM_PWT_NOTE_FS,     // 6
    DM_PWT_NOTE_G,      // 7
    DM_PWT_NOTE_GS,     // 8
    DM_PWT_NOTE_A,      // 9
    DM_PWT_NOTE_AS,     // 10
    DM_PWT_NOTE_B,      // 11
    DM_PWT_NOTES
} DM_PWT_NOTE;

typedef enum
{
    DM_BUZZER_SET_IDLE,
    DM_BUZZER_SET_EN
} DM_BUZZER_SET;


typedef enum
{
    DM_BUZZER_VIBRATING_IDLE,
    DM_BUZZER_VIBRATING_EN
} DM_BUZZER_VIBRATING;

typedef enum
{
    DM_BUZZER_FLASH_IDLE,
    DM_BUZZER_FLASH_KEY,
    DM_BUZZER_FLASH_LCD,
    DM_BUZZER_FLASH_BOTH
} DM_BUZZER_FLASH;

UINT8 DM_BuzzerVibratingSet(UINT8 nGpio);

// callback part added @11.22 by Longman
typedef void (*BUZZERCALLBACK)(void);

VOID DM_BuzzerDelayNmS(UINT32 nMillisecondes);
UINT8 DM_BuzzerOpen(VOID);
UINT8 DM_BuzzerStop(VOID);
UINT8 DM_BuzzerPlay(DM_PWT_OCT Oct, DM_PWT_NOTE Note, UINT8 Vol);

UINT8 DM_BuzzerVolumeSet(UINT8 Vol);
UINT8 DM_BuzzerSongStart(UINT8 *pBuffer, UINT32 nBufferLen, UINT32 duration, DM_BUZZER_SET Btemp, DM_BUZZER_VIBRATING Vtemp, DM_BUZZER_FLASH Ftemp, BUZZERCALLBACK pbuzzer_callback);
UINT8 DM_BuzzerSongStop(VOID);
#endif
//
//
//

#endif // _H_
