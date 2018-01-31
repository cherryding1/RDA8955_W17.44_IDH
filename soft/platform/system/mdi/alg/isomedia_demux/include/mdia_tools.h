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

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _GF_MDIA_TOOLS_H_
#define _GF_MDIA_TOOLS_H_

#include "setup.h"
/*track dumper*/

enum
{
    /*track dumper types are formatted as flags for conveniency for
    authoring tools, but never used as a OR'ed set*/
    /*native format (JPG, PNG, MP3, raw aac or raw cmp) if supported*/
    GF_EXPORT_NATIVE = 1,
    /*raw samples (all except OD)*/
    GF_EXPORT_RAW_SAMPLES = (1<<1),
    /*NHNT format (any MPEG-4 media)*/
    GF_EXPORT_NHNT = (1<<2),
    /*AVI (MPEG4 video and AVC tracks only)*/
    GF_EXPORT_AVI = (1<<3),
    /*MP4 (all except OD)*/
    GF_EXPORT_MP4 = (1<<4),
    /*AVI->RAW to dump video (trackID=1) or audio (trackID>=2)*/
    GF_EXPORT_AVI_NATIVE = (1<<5),

    /*following ones are real flags*/
    /*used bfor MP4 extraction, indicates track should be added to dest file if any*/
    GF_EXPORT_MERGE_TRACKS = (1<<10),
    /*indicates QCP file format possible as well as native (EVRC and SMV audio only)*/
    GF_EXPORT_USE_QCP = (1<<11),
    /*ony probes extraction format*/
    GF_EXPORT_PROBE_ONLY = (1<<30),
    /*when set by user during export, will abort*/
    GF_EXPORT_DO_ABORT = (1<<31),
};



#endif
#endif
