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

#ifndef VIDREC_API_H
#define VIDREC_API_H

#include "cpu_share.h"


int32 LILY_VidrecPreviewStartReq(MMC_VDOREC_SETTING_STRUCT *previewParam , void(*vid_rec_finish_ind)(uint16 msg_result) );


int32 LILY_VidrecAdjustSettingReq (int32 adjustItem, int32 value);

int32 LILY_VidrecPreviewStopReq(void);


int32 LILY_VidrecRecordStartReq (HANDLE filehandle) ;   //  MCI_MEDIA_PLAY_REQ,


int32 LILY_VidrecRecordPauseReq(void);


int32 LILY_VidrecRecordResumeReq(void);

int32 LILY_VidrecRecordStopReq(void);

int32 LILY_VidrecAudioSampleReadyReq(uint32 nEvent);

int32 LILY_VidrecApplyAudioSampleStop(void);

#endif


