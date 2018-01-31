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



# ifndef PLAYER_H
# define PLAYER_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "mad.h"

typedef enum mp3_frame_dec
{
    MP3_FRAME_STOP = 0x0000,      /* stop decoding normally */
    MP3_FRAME_BREAK = 0x0001, /* stop decoding and signal an error */
    MP3_FRAME_CONTINUE = 0x0002,  /* continue normally */
    MP3_FRAME_GETDATA = 0x0003    /* input data and continue */
} DEC_FRAME;


enum
{
    PLAYER_OPTION_SHUFFLE      = 0x0001,
    PLAYER_OPTION_DOWNSAMPLE   = 0x0002,
    PLAYER_OPTION_IGNORECRC    = 0x0004,

    PLAYER_OPTION_SKIP         = 0x0010,
    PLAYER_OPTION_TIMED        = 0x0020,
    PLAYER_OPTION_TTYCONTROL   = 0x0040,
    PLAYER_OPTION_STREAMID3    = 0x0080,

    PLAYER_OPTION_FADEIN       = 0x0100,
    PLAYER_OPTION_FADEOUT      = 0x0200,
    PLAYER_OPTION_GAP          = 0x0400,
    PLAYER_OPTION_CROSSFADE    = 0x0800,

# if defined(EXPERIMENTAL)
    PLAYER_OPTION_EXTERNALMIX  = 0x1000,
    PLAYER_OPTION_EXPERIMENTAL = 0x2000
# endif
};

enum player_control
{
    PLAYER_CONTROL_DEFAULT,
    PLAYER_CONTROL_NEXT,
    PLAYER_CONTROL_PREVIOUS,
    PLAYER_CONTROL_REPLAY,
    PLAYER_CONTROL_STOP
};

enum player_channel
{
    PLAYER_CHANNEL_DEFAULT = 0,
    PLAYER_CHANNEL_LEFT    = 1,
    PLAYER_CHANNEL_RIGHT   = 2,
    PLAYER_CHANNEL_MONO    = 3,
    PLAYER_CHANNEL_STEREO  = 4
};

enum stats_show
{
    STATS_SHOW_OVERALL,
    STATS_SHOW_CURRENT,
    STATS_SHOW_REMAINING
};

struct audio_stats
{
    unsigned long clipped_samples;
    mad_fixed_t peak_clipping;
    mad_fixed_t peak_sample;
};

enum audio_mode
{
    AUDIO_MODE_ROUND,
    AUDIO_MODE_DITHER
};


struct player
{
    struct mad_decoder decoder;


    int verbosity;

    int options;
    int repeat;

    enum player_control control;

    struct playlist
    {
        char const **entries;
        int length;
        int current;
    } playlist;

    mad_timer_t global_start;
    mad_timer_t global_stop;

    mad_timer_t fade_in;
    mad_timer_t fade_out;
    mad_timer_t gap;

    struct input
    {
        char const *path;

        int fd;

        unsigned char *data;
        unsigned long length;

        int eof;

//    struct xing xing;
    } input;

    struct output
    {
        enum audio_mode mode;

        mad_fixed_t attenuation;
        struct filter *filters;

        unsigned int channels_in;
        unsigned int channels_out;
        enum player_channel select;

        unsigned int speed_in;
        unsigned int speed_out;

        unsigned int precision_in;
        unsigned int precision_out;

        char const *path;
//    audio_ctlfunc_t *command;

//    struct resample_state resample[2];
//    mad_fixed_t (*resampled)[2][MAX_NSAMPLES];
    } output;

    struct stats
    {
        enum stats_show show;
        char const *label;

        unsigned long total_bytes;
        mad_timer_t total_time;

        mad_timer_t global_timer;
        mad_timer_t absolute_timer;
        mad_timer_t play_timer;

        unsigned long global_framecount;
        unsigned long absolute_framecount;
        unsigned long play_framecount;

        unsigned long error_frame;
        unsigned long mute_frame;

        int vbr;
        unsigned int bitrate;
        unsigned long vbr_frames;
        unsigned long vbr_rate;

        signed long nsecs;

        struct audio_stats audio;
    } stats;
};

void player_init(struct player *);
void player_finish(struct player *);

int player_run(struct player *, int, char const *[]);

# endif
