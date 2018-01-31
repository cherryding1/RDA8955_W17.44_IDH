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



#ifndef _HWPFDEFS_H_
#define _HWPFDEFS_H_

// Flags for DVPFHeader

// import video source flags
#define HX_SYSTEM_BUFFER    0x00000001  // Standard video buffer
#define HX_DMA_BUFFER       0x00000002  // DMA ready video buffer
#define HX_COPY_VIDEO       0x00000004  // Manually copy video data to hw surface

// import alpha flags
#define HX_COPY_ALPHA       0x00000008  // Manually copy alpha data to hw surface
#define HX_DISABLE_ALPHA    0x00000010  // Disable hw alpha blending

typedef struct _tagDVPFHeader
{
    INT32   lRV8Strengths[4];

    UINT32  ulFlags;

    void*   lpSurface;                  // device specific surface

} DVPFHeader;

#endif //_HWPFDEFS_H_

