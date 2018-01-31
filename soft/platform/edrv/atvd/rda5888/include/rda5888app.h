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

#ifndef __RDA5888APP_H__
#define __RDA5888APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rda5888drv.h"

typedef struct rdamtv_channel_s
{
    int     chn;
    uint32  freq;
    int     bw;
    int     chstd;
} rdamtv_channel_t, *p_rdamtv_channel_t;

// ---------------------------------------------------------------------------
// RDAMTV WORK MODES
// ---------------------------------------------------------------------------
typedef enum
{
    RDAMTV_MODE_ANALOG_TV,          // Analog Television mode
    RDAMTV_MODE_FM_RADIO,           // FM Radio mode
    RDAMTV_MODE_NONE                // No Mode specified
} rdamtv_mode_t;

// ---------------------------------------------------------------------------
// TV AREA DEFINISTIONS
// ---------------------------------------------------------------------------
typedef enum
{
    RDAMTV_AREA_CHINA         = 0,
    RDAMTV_AREA_CHINA_SHENZHEN,
    RDAMTV_AREA_CHINA_TAIWAN,
    RDAMTV_AREA_WESTERNEUROP,
    RDAMTV_AREA_AMERICA,
    RDAMTV_AREA_CHINA_MACAO,
    RDAMTV_AREA_INDIA,
    RDAMTV_AREA_PAKISTAN,
    RDAMTV_AREA_INDONESIA,
    RDAMTV_AREA_CHINA_HONGKONG,
    RDAMTV_AREA_YEMEN,  // 10
    RDAMTV_AREA_BAHRAIN,
    RDAMTV_AREA_ABU_DHABI,
    RDAMTV_AREA_KUWAIT,
    RDAMTV_AREA_THAI,
    RDAMTV_AREA_PORTUGAL,
    RDAMTV_AREA_SPAIN,
    RDAMTV_AREA_PHILIPINES,
    RDAMTV_AREA_MALAYSIA,
    RDAMTV_AREA_VIETNAM,
    RDAMTV_AREA_BRAZIL,  // 20
    RDAMTV_AREA_UK,
    RDAMTV_AREA_SOUTH_AFRICA,
    RDAMTV_AREA_TURKEY,
    RDAMTV_AREA_SINGAPORE,
    RDAMTV_AREA_CAMBODIA,
    RDAMTV_AREA_LAOS,
    RDAMTV_AREA_AFGHANISTA,
    RDAMTV_AREA_CANADA,
    RDAMTV_AREA_KOREA,
    RDAMTV_AREA_MEXICO,    // 30
    RDAMTV_AREA_CHILE,
    RDAMTV_AREA_VENEZUELA,
    RDAMTV_AREA_JAPAN,
    RDAMTV_AREA_ARGENTINA,
    RDAMTV_AREA_BURMA,     // 35
    RDAMTV_AREA_RUSSIA,

    RDAMTV_AREA_FACTORYMODE,
    RDAMTV_AREA_NUM
} rdamtv_area_t;

// -------------------------------------
// global variables
// -------------------------------------
extern int g_nCurChCnt;  // current channel counter.
extern rdamtv_ctx_t g_stRdamtvCtx;
extern const rdamtv_channel_t *p_rdamtv_cur_map;

#ifdef RDA5888_VIDENHANCE_ENABLE
extern uint8 g_nVideoEnhanceEnable;
extern uint8 g_nDiscardFrameNum;
extern uint32 g_nFrameBufSize;
extern uint8 g_nFirstBufferFlag;
#endif

// ----------------------------------------------------------------------------
// This function sets up the internal state of this component.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_Init(void);

// ----------------------------------------------------------------------------
// This function performs cyclic processing for signal check, soft blending,
// and other operations. ths system want to call this function periodic.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_TimerHandler(void);

// ----------------------------------------------------------------------------
// This function sets up channel map, works mode, and video standard.
//
// Params:
//  pstChMap:  channel map to set.
//  nWorkMode: current work mode.
//  nVidStd: current video standard.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetChannelMap(const rdamtv_channel_t *pstChMap, uint32 nWorkMode, uint32 nVidStd);

// ----------------------------------------------------------------------------
// This function sets current country or area.
//
// Params:
//  cTVArea: Area to set.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetTVArea(uint32 cTVArea);

// ----------------------------------------------------------------------------
// This function gets current country or area.
//
// Params:
//  none.
//
// Return:
//  current area.
// ----------------------------------------------------------------------------
uint32 RDAAPP_GetTVArea(void);

// ----------------------------------------------------------------------------
// This function sets a channel or scan a channel.
//
// Params:
//  nChannel: channel index to set.
//  nFlag:    0: set channel, 1: scan channel.
//
// Return:
//  TRUE: channel search done, FALSE: channel search fail.
// ----------------------------------------------------------------------------
uint8 RDAAPP_SetChannel(uint32 nChannel, uint32 nFlag);

// ----------------------------------------------------------------------------
// This function sets audio mute enable.
//
// Params:
//  bMute: 0: unmute, 1: mute.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetMute(bool bMute);

// ----------------------------------------------------------------------------
// This function sets the brightness for current channel being processed.
//
// Params:
//  nLevel: 0~15
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetBright(uint32 nLevel);

// ----------------------------------------------------------------------------
// This function sets volume value for audio output.
//
// Params:
//  nLevel: 0~40
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_SetVolume(uint16 nValue);

// ----------------------------------------------------------------------------
// This function gets current signal level.
//
// Params:
//  none.
//
// Return:
//  current signal level.
// ----------------------------------------------------------------------------
uint8 RDAAPP_GetSignalLevel(void);

// ----------------------------------------------------------------------------
// This function gets total number of channels for current frequencies table.
//
// Params:
//  none.
//
// Return:
//  total number of channels.
// ----------------------------------------------------------------------------
uint8 RDAAPP_GetChannelCount(void);

// ----------------------------------------------------------------------------
// This function exit and clear internal state of this component.
//
// Params:
//  none.
//
// Return:
//  none.
// ----------------------------------------------------------------------------
void RDAAPP_Exit(void);

#ifdef RDA5888_VIDENHANCE_ENABLE
void RDAAPP_VideoEnhance(unsigned int buffer1, unsigned int buffer2, unsigned int size);
void RDAAPP_SetVideoEnhance(bool t);
uint8 RDAAPP_GetVideoEnhance(void);
#endif
uint16 RDAAPP_GetTPMode(void);

// return 1: 625 lines, 0: 525 lines
uint8 RDAAPP_Is625Lines(void);

#ifdef __cplusplus
}
#endif

#endif
