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



#ifndef FFMPEG_IMGCONVERT_H
#define FFMPEG_IMGCONVERT_H

#include "avcodec.h"

int ff_fill_linesize(AVPicture *picture, int pix_fmt, int width);

int ff_fill_pointer(AVPicture *picture, uint8_t *ptr, int pix_fmt, int height);

int ff_get_plane_bytewidth(enum PixelFormat pix_fmt, int width, int plane);

#endif /* FFMPEG_IMGCONVERT_H */
