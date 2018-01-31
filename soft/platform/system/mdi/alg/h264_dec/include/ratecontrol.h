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



#ifndef FFMPEG_RATECONTROL_H
#define FFMPEG_RATECONTROL_H

/**
 * @file ratecontrol.h
 * ratecontrol header.
 */


#include "eval.h"
#include "common.h"
#ifdef SHEEN_VC_DEBUG
#include <stdio.h>
#endif

typedef struct Predictor
{
    double coeff;
    double count;
    double decay;
} Predictor;

typedef struct RateControlEntry
{
    int pict_type;
    float qscale;
    int mv_bits;
    int i_tex_bits;
    int p_tex_bits;
    int misc_bits;
    int header_bits;
    uint64_t expected_bits;
    int new_pict_type;
    float new_qscale;
    int mc_mb_var_sum;
    int mb_var_sum;
    int i_count;
    int skip_count;
    int f_code;
    int b_code;
} RateControlEntry;

/**
 * rate control context.
 */
typedef struct RateControlContext
{
    FILE *stats_file;
    int num_entries;              ///< number of RateControlEntries
    RateControlEntry *entry;
    double buffer_index;          ///< amount of bits in the video/audio buffer
    Predictor pred[5];
    double short_term_qsum;       ///< sum of recent qscales
    double short_term_qcount;     ///< count of recent qscales
    double pass1_rc_eq_output_sum;///< sum of the output of the rc equation, this is used for normalization
    double pass1_wanted_bits;     ///< bits which should have been outputed by the pass1 code (including complexity init)
    double last_qscale;
    double last_qscale_for[5];    ///< last qscale for a specific pict type, used for max_diff & ipb factor stuff
    int last_mc_mb_var_sum;
    int last_mb_var_sum;
    uint64_t i_cplx_sum[5];
    uint64_t p_cplx_sum[5];
    uint64_t mv_bits_sum[5];
    uint64_t qscale_sum[5];
    int frame_count[5];
    int last_non_b_pict_type;

    void *non_lavc_opaque;        ///< context for non lavc rc code (for example xvid)
    float dry_run_qscale;         ///< for xvid rc
    int last_picture_number;      ///< for xvid rc
    AVEvalExpr * rc_eq_eval;
} RateControlContext;

struct MpegEncContext;

/* rate control */
int ff_rate_control_init(struct MpegEncContext *s);
float ff_rate_estimate_qscale(struct MpegEncContext *s, int dry_run);
void ff_write_pass1_stats(struct MpegEncContext *s);
void ff_rate_control_uninit(struct MpegEncContext *s);
int ff_vbv_update(struct MpegEncContext *s, int frame_size);
void ff_get_2pass_fcode(struct MpegEncContext *s);

int ff_xvid_rate_control_init(struct MpegEncContext *s);
void ff_xvid_rate_control_uninit(struct MpegEncContext *s);
float ff_xvid_rate_estimate_qscale(struct MpegEncContext *s, int dry_run);

#endif /* FFMPEG_RATECONTROL_H */

