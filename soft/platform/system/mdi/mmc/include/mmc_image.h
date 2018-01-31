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

#ifndef _MMC_IMAGE_H
#define _MMC_IMAGE_H
#include "cs_types.h"
//#include "lily_lcd_api.h"
#include "fs.h"
// #include "../src/mmc_gif.h"
// #include "../src/mmc_gif_codec.h"

typedef enum
{
    MSG_MMC_GIF_PLAY_CONTINUE,  // =10,
    MSG_MMC_GIF_PLAY_STOP,
    MSG_MMC_UPDATE_LCD,
    MSG_MMC_WEBCAM_MJPEG_ENCODE,
} GIF_PLAY_USER_MSG;

typedef struct
{
    char *file_name;
    FS_FILE_ATTR file_attr;
} IMAGE_FILE_ATTR;

extern uint32 g_TotalMemAllocate;

// extern void mmc_gif_set_progress_callback(mmc_gif_callback_ptr ptr);
extern int32 Lily_BmpFileShow(uint8* filename, RECT* dispRegin);
extern int32 Lily_JpegFileShow(uint8* filename, RECT* dispRegin);
extern int32 Lily_JpegFileDecodeToBuff(uint8 *filename, uint8 *destBuff, uint16 *destWidth, uint16 *destHeight);
extern int32 Lily_AnimationPlayStart(uint8* filename, RECT* dispRegin);
extern int32 Lily_AnimationPlayStop(VOID);
extern int32 Lily_InvalidFileShow(uint8* filename, RECT* dispRegin);
extern VOID Lily_SetDecodeState(CHAR state);
extern CHAR Lily_GetDecodeState(VOID);
extern VOID Lily_SetAnimationResult(int32 value);
extern int32 Lily_GetAnimationResult(VOID);
extern int32 Lily_AddImageFrame(int16 startx,int16 starty,int16 endx, int16 endy);
extern boolean IsMemEnoughForAlloc(uint32 nNeedAllocMemSize);

#endif

