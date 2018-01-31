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

#ifndef _VLC_CODES_H_
#define _VLC_CODES_H_

#include "portab.h"

#define VLC_ERROR   (-1)

#define ESCAPE  3
#define ESCAPE1 6
#define ESCAPE2 14
#define ESCAPE3 15

typedef struct
{
    uint32_t code;
    uint8_t len;
}
VLC;

typedef struct
{
    uint8_t last;
    uint8_t run;
    int8_t level;
}
EVENT;

typedef struct
{
    uint8_t len;
    EVENT event;
}
REVERSE_EVENT;

typedef struct
{
    VLC vlc;
    EVENT event;
}
VLC_TABLE;


/******************************************************************
 * common tables between encoder/decoder                          *
 ******************************************************************/

extern VLC const dc_lum_tab[];
//extern short const dc_threshold[];
extern VLC_TABLE const coeff_tab[2][102];
extern uint8_t const max_level[2][2][64];
extern uint8_t const max_run[2][2][64];
extern VLC const mcbpc_intra_table[64];
extern VLC const mcbpc_inter_table[257];
extern VLC const cbpy_table[64];
extern VLC const TMNMVtab0[];
extern VLC const TMNMVtab1[];
extern VLC const TMNMVtab2[];

#endif /* _VLC_CODES_H */
#endif

