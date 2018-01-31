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



#ifndef RA_FORMAT_INFO_H
#define RA_FORMAT_INFO_H

#include "helix_types.h"

/*
 * ra_format_info struct
 *
 */
typedef struct ra_format_info_struct
{
    UINT32 ulSampleRate;
    UINT32 ulActualRate;
    UINT16 usBitsPerSample;
    UINT16 usNumChannels;
    UINT16 usAudioQuality;
    UINT16 usFlavorIndex;
    UINT32 ulBitsPerFrame;
    UINT32 ulGranularity;
    UINT32 ulOpaqueDataSize;
    BYTE*  pOpaqueData;
} ra_format_info;

#endif /* #ifndef RA_FORMAT_INFO_H */
