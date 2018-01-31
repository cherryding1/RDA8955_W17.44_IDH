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



#ifndef FFMPEG_OPT_H
#define FFMPEG_OPT_H

/**
 * @file opt.h
 * AVOptions
 */

#include "rational.h"

enum AVOptionType
{
    FF_OPT_TYPE_FLAGS,
    FF_OPT_TYPE_INT,
    FF_OPT_TYPE_INT64,
    FF_OPT_TYPE_DOUBLE,
    FF_OPT_TYPE_FLOAT,
    FF_OPT_TYPE_STRING,
    FF_OPT_TYPE_RATIONAL,
    FF_OPT_TYPE_BINARY,  ///< offset must point to a pointer immediately followed by an int for the length
    FF_OPT_TYPE_CONST=128,
};

/**
 * AVOption
 */
typedef struct AVOption
{
    const char *name;

    /**
     * short English help text
     * @todo What about other languages?
     */
    const char *help;
    int offset;             ///< offset to context structure where the parsed value should be stored
    enum AVOptionType type;

    double default_val;
    double min;
    double max;

    int flags;
#define AV_OPT_FLAG_ENCODING_PARAM  1   ///< a generic parameter which can be set by the user for muxing or encoding
#define AV_OPT_FLAG_DECODING_PARAM  2   ///< a generic parameter which can be set by the user for demuxing or decoding
#define AV_OPT_FLAG_METADATA        4   ///< some data extracted or inserted into the file like title, comment, ...
#define AV_OPT_FLAG_AUDIO_PARAM     8
#define AV_OPT_FLAG_VIDEO_PARAM     16
#define AV_OPT_FLAG_SUBTITLE_PARAM  32
//FIXME think about enc-audio, ... style flags
    const char *unit;
} AVOption;


const AVOption *av_find_opt(void *obj, const char *name, const char *unit, int mask, int flags);
const AVOption *av_set_string(void *obj, const char *name, const char *val);
const AVOption *av_set_double(void *obj, const char *name, double n);
const AVOption *av_set_q(void *obj, const char *name, AVRational n);
const AVOption *av_set_int(void *obj, const char *name, int64_t n);
double av_get_double(void *obj, const char *name, const AVOption **o_out);
AVRational av_get_q(void *obj, const char *name, const AVOption **o_out);
int64_t av_get_int(void *obj, const char *name, const AVOption **o_out);
const char *av_get_string(void *obj, const char *name, const AVOption **o_out, char *buf, int buf_len);
const AVOption *av_next_option(void *obj, const AVOption *last);
int av_opt_show(void *obj, void *av_log_obj);
void av_opt_set_defaults(void *s);
void av_opt_set_defaults2(void *s, int mask, int flags);

#endif /* FFMPEG_OPT_H */
