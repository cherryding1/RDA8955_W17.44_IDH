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





#ifndef FFMPEG_H264_PARSER_H
#define FFMPEG_H264_PARSER_H

#include "h264.h"

/**
 * finds the end of the current frame in the bitstream.
 * @return the position of the first byte of the next frame, or -1
 */
int ff_h264_find_frame_end(H264Context *h, const uint8_t *buf, int buf_size);

#endif /* FFMPEG_H264_PARSER_H */
