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

#ifndef _MMC_H_
#define _MMC_H_

#include "cs_types.h"
#include"cos.h"
#include"mci.h"

typedef struct _PCMSetting
{
    uint16 SampleRate;  //48k,44.1k,32k,22.05k,24k,16k,12k,11.025k,8k
    uint8  Bits;        //16bit or 8bit
    uint8  Channel;     //stero or mono
    uint32 *Buffer;     //Pointer for PCM buffer
    uint32 *Buffer_head;
    uint32 Size;        //Size of PCM buffer
    void (*OnPCM)(int32 half);//PCM1 handler
} PCMSETTING;

#define MMC_MODE_IDLE       0
#define MMC_MODE_AUDIO      1
#define MMC_MODE_VIDEO      2
#define MMC_MODE_RECORD     3
#define MMC_MODE_FM_RECORD  4
#define MMC_MODE_CAMERA     5
#define MMC_MODE_VIDREC     6
#define MMC_MODE_IMAGE      7
#define MMC_MODE_ANALOGTV     8
#define MMC_MODE_APBS     9

#define MMC_MODE_INVALID    10
#define MMC_MODE_PRE_CP    11
#define MMC_MODE_VDOCHAT  12
#define MMC_MODE_VDO_AUDIO_ONLY 13

#define MMC_RECEIVE_WRONG_COMMAND -1

typedef enum
{
    MMC_MP3_ENCODE = 0,
    MMC_MP3_DECODE,
    MMC_AMR_ENCODE,
    MMC_AMR_DECODE,
    MMC_MJPEG_ENCODE,
    MMC_MJPEG_DECODE,
    MMC_WMA_ENCODE,
    MMC_WMA_DECODE,
    MMC_AAC_ENCODE,
    MMC_AAC_DECODE,
    MMC_WAV_ENCODE,
    MMC_WAV_DECODE,
    MMC_H263_ZOOM,
    MMC_SBC_ENC,
    MMC_RM_DECODE,
    MMC_H264_DECODE,
    MMC_MPEG4_DECODE,
    MMC_H263_DECODE,
    MMC_JPEG_DECODE
} Codecmode;

////////////////////////////////////////////////////////////////////////////////
// INTERRUPT TYPE
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    INT_VOC_JPEGENC = 0,
    INT_VOC_AMRSAVE,

    INT_CPU_PCM2AMR,
    INT_VA_INTEGRATE,
    INT_PRE_INTEGRATE

} INT_TYPE_STATE;


typedef struct _Audio_Selector_
{
    int32 (*Play) (int32, HANDLE,int16,int32);
    int32 (*Stop) (void);
    int32 (*Pause) (void);
    int32 (*Resume) (HANDLE fileHandle);
    int32 (*GetID3) (char *);
    int32 (*UserMsg) (int32);
    int32 (*GetPlayInformation) (MCI_PlayInf *);
} Audio_Selector;
typedef struct _Video_Selector_
{
    int32 (*Open) (int32, int32, char*);
    int32 (*Play) (void);
    int32 (*Stop) (void);
    int32 (*Close) (void);
    int32 (*Pause) (void);
    int32 (*Resume) (void);
    int32 (*Seek) (int32, int32);
    int32 (*UserMsg) (COS_EVENT*);
} Video_Selector;

typedef struct _Record_Selector_
{
    int32 (*Start) (int32, int32, char*);
    int32 (*Stop) (void);
    int32 (*Pause) (void);
    int32 (*Resume) (void);
    int32 (*Sample) (char * pSample , int32 nSample);
    int32 (*UserMsg) (int32);
} Record_Selector;


typedef  enum
{
    NoneStatus,
    AudioPlayStatus,
    AudioPauseStatus,
    AudioStopStatus,

    VideoPlayStatus,
    VideoPauseStatus,
    VideoStopStatus,

    AudioRecordStatus,
    AudioRecordPauseStatus,
    AudioRecordStopStatus,

    VidrecPreviewStatus,
    VidrecRecordStatus,
    VidrecPauseStatus,
    VidrecStopStatus,

    CameraPreviewStatus,
    CameraCaptureStatus,

    ImagePlayStatus,
} mmcModeStatus;

extern boolean mmc_MemInit(void);
extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeAll(void);
extern char * mmc_MemMallocBack(int32);
extern void mmc_MemFreeAllBack();
extern int mmc_MemGetFree();
extern int32 mmc_SendEvent(HANDLE hTask, uint32 nEventId);
extern int32 mmc_SetCurrMode(int32 mode);
extern int32 mmc_GetCurrMode();
extern void mmc_EntryMode(int32 mode, int32 arg);
extern void mmc_ExitMode();
extern int32 mmc_SendMsg(HANDLE hTask, uint32 nEventId, uint32 param1, uint32 param2, uint32 param3);
extern void mmc_MemFreeLast(char * pbuf);
VOID mmc_SetModeStatus(mmcModeStatus status);
#endif
