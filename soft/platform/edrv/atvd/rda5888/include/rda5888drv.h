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


#ifndef __RDA5888DRV_H__
#define __RDA5888DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CS_PLATFORM
#define CS_PLATFORM
#endif

#ifdef CS_PLATFORM
#include "cs_types.h"

#define RDAEXT_DebugPrint BT_TraceOutputText
#define RDA5888_TRACE_MOD 0,
#endif

#define RDA5888D                  4
#define RDA5888S                  5
#define RDA5888E                  6
#define RDA5888H                  7

// ---------------------------------------------------------------
// CHIP_VERSION                  NUM.
// RDA5888D                       4
// RDA5888S                       5
// RDA5888E                       6
// RDA5888H/RDA5888HE             7
// ---------------------------------------------------------------

#if defined(RDAATV_CHIPVER_5888E)
#define RDA5888_CHIP_VER          6
#elif defined(RDAATV_CHIPVER_5888HE) || defined(RDAATV_CHIPVER_5888H)
#define RDA5888_CHIP_VER          7
#else
#define RDA5888_CHIP_VER          0xff
#endif
// ----------------------------------------------------------------------------
// RDA5888 Driver Version
// Labview Ver: 3.0(RDA5888), 1.8(RDA5888S), 1.2(RDA5888E)
// Last update: 2011/01/05
// ----------------------------------------------------------------------------
#define RDA5888DRV_VER_MAJOR      3
#define RDA5888DRV_VER_MINOR      1
#define RDA5888DRV_VER_PATCH      1

// ---------------------------------------------------------------
// function definition.
// ---------------------------------------------------------------
#if (RDA5888_CHIP_VER == 4)
#define RDA5888_FM_ENABLE
#define RDA5888_GPIO_I2C_ENABLE
#define RDA5888_SOFTBLENDING_ENABLE
#define RDA5888_SYNC_CHECK_ENABLE
#define RDA5888_VIDENHANCE_ENABLE
//#define RDA5888_INCHFILTER_ENABLE
#define RDA5888_MULTIPATH_ENABLE
//#define RDA5888_LDOMODE_ENABLE
#define RDA5888_YCADJUST_ENABLE
//#define RDA5888_26MCRYSTAL_ENABLE
//#define RDA5888_IODRIVEREN_ENABLE
#elif (RDA5888_CHIP_VER == 5)
#define RDA5888_FM_ENABLE
#define RDA5888_SOFTBLENDING_ENABLE
#define RDA5888_SYNC_CHECK_ENABLE
#define RDA5888_VIDENHANCE_ENABLE
//#define RDA5888_INCHFILTER_ENABLE
#define RDA5888_SOFTAFC_ENABLE
#define RDA5888_MULTIPATH_ENABLE
#define RDA5888_FRAMECOEF_ENABLE
#define RDA5888_YCADJUST_ENABLE
//#define RDA5888_26MCRYSTAL_ENABLE
//#define RDA5888_IODRIVEREN_ENABLE
//#define RDA5888_SECAM_SOFTMUTE_ENABLE
#elif (RDA5888_CHIP_VER == 6)
#define RDA5888_FM_ENABLE
#define RDA5888_GPIO_I2C_ENABLE
//#define RDA5888_SOFTBLENDING_ENABLE
//#define RDA5888_SYNC_CHECK_ENABLE
//#define RDA5888_VIDENHANCE_ENABLE
//#define RDA5888_INCHFILTER_ENABLE
#define RDA5888_LPFSWITCH_ENABLE
#define RDA5888_LDOMODE_ENABLE
#define RDA5888_26MCRYSTAL_ENABLE
#define RDA5888_FM_26MCRYSTAL_ENABLE
//#define RDA5888_IODRIVEREN_ENABLE
//#define RDA5888_SECAM_SOFTMUTE_ENABLE
#define RDA5888_DSPINTCOEF_ENABLE
//#define RDA5888_HSYNCHOLD_ENABLE
#elif (RDA5888_CHIP_VER == 7)
#define RDA5888_FM_ENABLE
#define RDA5888_GPIO_I2C_ENABLE
//#define RDA5888_SOFTBLENDING_ENABLE
//#define RDA5888_SYNC_CHECK_ENABLE
#define RDA5888_INCHFILTER_ENABLE
#define RDA5888_LDOMODE_ENABLE
#define RDA5888_26MCRYSTAL_ENABLE
#define RDA5888_FM_26MCRYSTAL_ENABLE
//#define RDA5888_IODRIVEREN_ENABLE
//#define RDA5888_SECAM_SOFTMUTE_ENABLE
#define RDA5888_DSPINTCOEF_ENABLE
//#define RDA5888_HSYNCHOLD_ENABLE
#define RDA5888_32KFMMODE_ENALBE
#endif


// ---------------------------------------------------------------------------
// BASIC TYPES DEFINITIONS
// ---------------------------------------------------------------------------
#ifndef CS_PLATFORM
#ifndef uint8
typedef unsigned char    uint8;
#endif

#ifndef uint16
typedef unsigned short   uint16;
#endif

#ifndef uint32
typedef unsigned long int    uint32;
#endif

#ifndef int8
typedef signed char      int8;
#endif

#ifndef int16
typedef signed short     int16;
#endif

#ifndef int32
typedef signed long int          int32;
#endif

#ifndef bool
typedef unsigned char     bool;
#endif

#ifndef TRUE
#define TRUE            (1 == 1)
#endif

#ifndef FALSE
#define FALSE           (0 == 1)
#endif

#ifndef NULL
#define NULL            ((void *)0)
#endif
#endif
// ------------------------ END TYPE DEFINITIONS -----------------------------

// ---------------------------------------------------------------------------
// RDAMTV return code definitions
// ---------------------------------------------------------------------------
typedef enum
{
    RDAMTV_RT_SUCCESS = 0,
    RDAMTV_RT_IIC_ERROR,
    RDAMTV_RT_SCAN_FAIL,
    RDAMTV_RT_SCAN_DONE,
    RDAMTV_RT_ERROR
} rdamtv_rt_t;
// ---------------------------------------------------------------------------
// Commonly used video formats are included here.
// ---------------------------------------------------------------------------
typedef enum
{
    RDAMTV_VSTD_PAL_M       = 1,
    RDAMTV_VSTD_PAL_B       = 2,
    RDAMTV_VSTD_PAL_B1      = 3,
    RDAMTV_VSTD_PAL_D       = 4,
    RDAMTV_VSTD_PAL_D1      = 5,
    RDAMTV_VSTD_PAL_G       = 6,
    RDAMTV_VSTD_PAL_H       = 7,
    RDAMTV_VSTD_PAL_K       = 8,
    RDAMTV_VSTD_PAL_N       = 9,
    RDAMTV_VSTD_PAL_I       = 10,
    RDAMTV_VSTD_PAL_NC      = 11,

    RDAMTV_VSTD_NTSC_M      = 21,
    RDAMTV_VSTD_NTSC_B_G    = 22,
    RDAMTV_VSTD_NTSC_D_K    = 23,
    RDAMTV_VSTD_NTSC_I      = 24,

    RDAMTV_VSTD_SECAM_M     = 31,
    RDAMTV_VSTD_SECAM_B     = 32,
    RDAMTV_VSTD_SECAM_B1    = 33,
    RDAMTV_VSTD_SECAM_D     = 34,
    RDAMTV_VSTD_SECAM_D1    = 35,
    RDAMTV_VSTD_SECAM_G     = 36,
    RDAMTV_VSTD_SECAM_H     = 37,
    RDAMTV_VSTD_SECAM_K     = 38,
    RDAMTV_VSTD_SECAM_N     = 39,
    RDAMTV_VSTD_SECAM_I     = 40,
    RDAMTV_VSTD_SECAM_NC    = 41,
    RDAMTV_VSTD_SECAM_L     = 42,

    RDAMTV_VSTD_NONE        = 255   // No Video Standard Specified
} rdamtv_vstd_t;

typedef struct
{
    uint8 work_mode;     // ATV, FM ...
    rdamtv_vstd_t vidstd;
} rdamtv_ctx_t;

typedef enum
{
    BLENDMODE_PALBG = 0,
    BLENDMODE_PALDK = 1,
    BLENDMODE_PALI  = 2,
    BLENDMODE_PALM  = 3,
    BLENDMODE_PALN  = 4,

    BLENDMODE_NTSCBG = 5,
    BLENDMODE_NTSCDK = 6,
    BLENDMODE_NTSCI  = 7,
    BLENDMODE_NTSCM  = 8,

    BLENDMODE_INVALID
} rdamtv_blmode_t;

extern uint8 g_nBlendMode;

// get a field in a variable
#define RDA_GET_MASK(var, shift, mask) (((var) & (mask)) >> (shift))

// ----------------------------------------------------------------------------
// RDA5888_WriteReg
//
// dwRegAddr:       register to write
// wData:           data to write
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_WriteReg(uint32 dwRegAddr, uint16 wData);

// ----------------------------------------------------------------------------
// RDA5888_ReadReg
//
// dwRegAddr:        Register to read
// pwData:           Pointer for register data
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_ReadReg(uint32 dwRegAddr, uint16 *pwData);

uint32 RDA5888_FieldWriteReg(uint32 dwRegAddr, uint16 wFieldMask, uint16 wFieldData);

uint16 RDA5888_FieldReadReg(uint32 dwRegAddr, uint16 wFieldMask);

// ---------------------------------------------------------------------------
// Sets the internal registers to tune the specified frequency.
//
// Params:
//  dwVFreq    : Desired tuning frequency in hertz.   i.e. 57.75mhz  =
//              57750000.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ---------------------------------------------------------------------------
uint32 RDA5888_FreqSet(uint32 dwVFreq);

// ----------------------------------------------------------------------------
// This function sets the video decoder to a specified video format.
//
// Params:
//  eVideoStd        Video standard to set
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_VideoStdSet(rdamtv_vstd_t eVideoStd);

// ----------------------------------------------------------------------------
// This function sets the video decoder to the test pattern.
//
// Params:
//  None
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetTestPattern(void);

// ----------------------------------------------------------------------------
// This function initializes the rda5888 analog module.
//
// Params:
//  none.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_DCDCInit(void);

// ----------------------------------------------------------------------------
// This function initializes the rda5888 dsp module.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_DSPInit(void);

#ifdef RDA5888_26MCRYSTAL_ENABLE
void RDA5888_26MCrystalInit(void);
#endif

// ---------------------------------------------------------------------------
// RDA5888_ScanChannel.
//
// Params:
//  dwVFreq    : Desired tuning frequency in hertz.   i.e. 57.75mhz  =
//              57750000.
//  eVideoStd  : AnalogTV standard set.
//  nFlag      : 0: set channel, 1: scan channel.
//
// Return:
//  RDAMTV_RT_ERROR:     an error occurred communicating with the device.
//  RDAMTV_RT_SCAN_FAIL: not found channel.
//  RDAMTV_RT_SCAN_DONE: found channel.
// ---------------------------------------------------------------------------
uint32 RDA5888_ScanChannel(uint32 dwVFreq, rdamtv_vstd_t eVideoStd, uint32 nFlag);

// ----------------------------------------------------------------------------
// This function gets the rssi value for the current channel.
//
// Params:
//  None
//
// Return:
//  rssi value.
// ----------------------------------------------------------------------------
int16 RDA5888_GetRSSI(void);


// ----------------------------------------------------------------------------
// This function sets Luminance for the current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetLumaLevel(uint8 nLevel);

// ----------------------------------------------------------------------------
// This function sets chrominance for the current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetChromLevel(uint8 nLevel);

// ----------------------------------------------------------------------------
// This function sets volume value for audio output.
//
// Params:
//  nValue: volume to set.
//
// Return:
//  RDAMTV_RT_ERROR if an error occurred communicating with the device.
//  RDAMTV_RT_SUCCESS if the operation was successful.
// ----------------------------------------------------------------------------
uint32 RDA5888_SetVolume(uint16 nValue);

uint8 RDA5888_Is625Lines(void);

float RDA5888_GetAFCOut(void);

// ----------------------------------------------------------------------------
// This function gets the chip version.
//
// Params:
//  None
//
// Return:
//  current chip version.
// ----------------------------------------------------------------------------
uint8 RDA5888_GetChipVer(void);

// ----------------------------------------------------------------------------
// This function open the I2S interface.
//
// Params:
//  cfg: I2S configuration.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void  RDA5888_I2SOpen(uint8 cfg);

// ----------------------------------------------------------------------------
// This function close the I2S interface.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_I2SClose(void);

// ----------------------------------------------------------------------------
// This function power down the rda5888.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDA5888_PowerDown(void);

#ifdef __cplusplus
}
#endif

#endif /* end __RDAMTVIF_H__ */
