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



#ifndef FFMPEG_PARSER_H
#define FFMPEG_PARSER_H

#include "avcodec.h"

typedef struct ParseContext
{
    uint8_t *buffer;
    int index;
    int last_index;
    unsigned int buffer_size;
    uint32_t state;             ///< contains the last few bytes in MSB order
    int frame_start_found;
    int overread;               ///< the number of bytes which where irreversibly read from the next frame
    int overread_index;         ///< the index into ParseContext.buffer of the overread bytes
} ParseContext;

struct MpegEncContext;

typedef struct ParseContext1
{
    ParseContext pc;
    /* XXX/FIXME PC1 vs. PC */
    /* MPEG-2-specific */
    AVRational frame_rate;
    int progressive_sequence;
    int width, height;

    /* XXX: suppress that, needed by MPEG-4 */
    struct MpegEncContext *enc;
    int first_picture;
} ParseContext1;

#define END_NOT_FOUND (-100)

int ff_combine_frame(ParseContext *pc, int next, const uint8_t **buf, int *buf_size);
int ff_mpeg4video_split(AVCodecContext *avctx, const uint8_t *buf,
                        int buf_size);
void ff_parse_close(AVCodecParserContext *s);
void ff_parse1_close(AVCodecParserContext *s);

/**
 * Fetches timestamps for a specific byte within the current access unit.
 * @param off byte position within the access unit
 * @param remove Found timestamps will be removed if set to 1, kept if set to 0.
 */
void ff_fetch_timestamp(AVCodecParserContext *s, int off, int remove);

#endif /* FFMPEG_PARSER_H */
