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

#ifndef _XVID_DECRAW_H_
#define _XVID_DECRAW_H_

#include "xvid.h"

#define MIN_USEFUL_BYTES 1
int dec_init(int use_assembler, int debug_level , int lowres);
int dec_main(uint8 *istream,
             uint8 *ostream,
             int istream_size,
             xvid_dec_stats_t *xvid_dec_stats,
             zoomPar *pZoom,
             bool isAviFile);
int dec_stop();
char * dec_get_cur_y_end();


#endif
#endif

