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





#ifndef FFMPEG_RL_H
#define FFMPEG_RL_H

#include "h264_bitstream.h"

/* run length table */
#define MAX_RUN    64
#define MAX_LEVEL  64

/** RLTable. */
typedef struct RLTable
{
    int n;                         ///< number of entries of table_vlc minus 1
    int last;                      ///< number of values for last = 0
    const uint16_t (*table_vlc)[2];
    const int8_t *table_run;
    const int8_t *table_level;
    uint8_t *index_run[2];         ///< encoding only
    int8_t *max_level[2];          ///< encoding & decoding
    int8_t *max_run[2];            ///< encoding & decoding
    VLC vlc;                       ///< decoding only deprecated FIXME remove
    RL_VLC_ELEM *rl_vlc[32];       ///< decoding only
} RLTable;

/**
 *
 * @param static_store static uint8_t array[2][2*MAX_RUN + MAX_LEVEL + 3] which will hold
 *                     the level and run tables, if this is NULL av_malloc() will be used
 */
void init_rl(RLTable *rl, uint8_t static_store[2][2*MAX_RUN + MAX_LEVEL + 3]);
void init_vlc_rl(RLTable *rl);

#define INIT_VLC_RL(rl, static_size)\
{\
    int q;\
    static RL_VLC_ELEM rl_vlc_table[32][static_size];\
    INIT_VLC_STATIC(&rl.vlc, 9, rl.n + 1,\
             &rl.table_vlc[0][1], 4, 2,\
             &rl.table_vlc[0][0], 4, 2, static_size);\
\
    if(!rl.rl_vlc[0]){\
        for(q=0; q<32; q++)\
            rl.rl_vlc[q]= rl_vlc_table[q];\
\
        init_vlc_rl(&rl);\
    }\
}

static inline int get_rl_index(const RLTable *rl, int last, int run, int level)
{
    int index;
    index = rl->index_run[last][run];
    if (index >= rl->n)
        return rl->n;
    if (level > rl->max_level[last][run])
        return rl->n;
    return index + level - 1;
}

#endif /* FFMPEG_RL_H */
