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

#ifndef _MB_CODING_H_
#define _MB_CODING_H_

#include "portab.h"
#include "mpeg4_global.h"
#include "vlc_codes.h"
#include "mpeg4_bitstream.h"

//void init_vlc_tables(void);

int check_resync_marker(Bitstream * bs, int addbits);
int bs_get_spritetrajectory(Bitstream * bs);

int get_mcbpc_intra(Bitstream * bs);
int get_mcbpc_inter(Bitstream * bs);
int get_cbpy(Bitstream * bs,
             int intra);
int32_t dec_mv(int fcode,VECTOR *retMV,VECTOR * predMV,Bitstream *bs);

int get_dc_dif(Bitstream * bs,
               uint32_t dc_size);
int get_dc_size_lum(Bitstream * bs);
int get_dc_size_chrom(Bitstream * bs);

void get_intra_block(Bitstream * bs,
                     int16_t * block,
                     int direction,
                     int coeff);
void get_inter_block_h263(
    Bitstream * bs,
    int16_t * block,
    int direction,
    const int quant);
void get_inter_block_mpeg(
    Bitstream * bs,
    int16_t * block,
    int direction,
    const int quant,
    const uint16_t *matrix);


#endif                          /* _MB_CODING_H_ */
#endif

