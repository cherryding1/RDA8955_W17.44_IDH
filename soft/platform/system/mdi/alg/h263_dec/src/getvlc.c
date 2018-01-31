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










#ifndef MEDIA_VOCVID_SUPPORT
//#include <stdio.h>

#include "h263_config.h"
#include "tmndec.h"
#include "h263_global.h"
#include "getvlc.h"
#ifndef SHEEN_VC_DEBUG
#include "mcip_debug.h"
#endif
#ifdef MP4_3GP_SUPPORT

#if 0
//only for getvlc.c below tables.shenh
#ifdef USE_INTERNAL_SRAM
static VLCtab *TMNMVtab0;
static VLCtab TMNMVtab0_list[] =
#else
static VLCtab TMNMVtab0[] =
#endif
{
    {3,4}, {61,4}, {2,3}, {2,3}, {62,3}, {62,3},
    {1,2}, {1,2}, {1,2}, {1,2}, {63,2}, {63,2}, {63,2}, {63,2}
};//56 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *TMNMVtab1;
static VLCtab TMNMVtab1_list[] =
#else
static VLCtab TMNMVtab1[] =
#endif
{
    {12,10}, {52,10}, {11,10}, {53,10}, {10,9}, {10,9},
    {54,9}, {54,9}, {9,9}, {9,9}, {55,9}, {55,9},
    {8,9}, {8,9}, {56,9}, {56,9}, {7,7}, {7,7},
    {7,7}, {7,7}, {7,7}, {7,7}, {7,7}, {7,7},
    {57,7}, {57,7}, {57,7}, {57,7}, {57,7}, {57,7},
    {57,7}, {57,7}, {6,7}, {6,7}, {6,7}, {6,7},
    {6,7}, {6,7}, {6,7}, {6,7}, {58,7}, {58,7},
    {58,7}, {58,7}, {58,7}, {58,7}, {58,7}, {58,7},
    {5,7}, {5,7}, {5,7}, {5,7}, {5,7}, {5,7},
    {5,7}, {5,7}, {59,7}, {59,7}, {59,7}, {59,7},
    {59,7}, {59,7}, {59,7}, {59,7}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {60,6}, {60,6},{60,6},{60,6},
    {60,6},{60,6},{60,6},{60,6},{60,6},{60,6},
    {60,6},{60,6},{60,6},{60,6},{60,6},{60,6}
};//384 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *TMNMVtab2;
static VLCtab TMNMVtab2_list[] =
#else
static VLCtab TMNMVtab2[] =
#endif
{
    {32,12}, {31,12}, {33,12}, {30,11}, {30,11}, {34,11},
    {34,11}, {29,11}, {29,11}, {35,11}, {35,11}, {28,11},
    {28,11}, {36,11}, {36,11}, {27,11}, {27,11}, {37,11},
    {37,11}, {26,11}, {26,11}, {38,11}, {38,11}, {25,11},
    {25,11}, {39,11}, {39,11}, {24,10}, {24,10}, {24,10},
    {24,10}, {40,10}, {40,10}, {40,10}, {40,10}, {23,10},
    {23,10}, {23,10}, {23,10}, {41,10}, {41,10}, {41,10},
    {41,10}, {22,10}, {22,10}, {22,10}, {22,10}, {42,10},
    {42,10}, {42,10}, {42,10}, {21,10}, {21,10}, {21,10},
    {21,10}, {43,10}, {43,10}, {43,10}, {43,10}, {20,10},
    {20,10}, {20,10}, {20,10}, {44,10}, {44,10}, {44,10},
    {44,10}, {19,10}, {19,10}, {19,10}, {19,10}, {45,10},
    {45,10}, {45,10}, {45,10}, {18,10}, {18,10}, {18,10},
    {18,10}, {46,10}, {46,10}, {46,10}, {46,10}, {17,10},
    {17,10}, {17,10}, {17,10}, {47,10}, {47,10}, {47,10},
    {47,10}, {16,10}, {16,10}, {16,10}, {16,10}, {48,10},
    {48,10}, {48,10}, {48,10}, {15,10}, {15,10}, {15,10},
    {15,10}, {49,10}, {49,10}, {49,10}, {49,10}, {14,10},
    {14,10}, {14,10}, {14,10}, {50,10}, {50,10}, {50,10},
    {50,10}, {13,10}, {13,10}, {13,10}, {13,10}, {51,10},
    {51,10}, {51,10}, {51,10}
};//492 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *MCBPCtab0;
static VLCtab MCBPCtab0_list[] =
#else
static VLCtab MCBPCtab0[] =
#endif
{
    {-1,0},
    {255,9}, {52,9}, {36,9}, {20,9}, {49,9}, {35,8}, {35,8}, {19,8}, {19,8},
    {50,8}, {50,8}, {51,7}, {51,7}, {51,7}, {51,7}, {34,7}, {34,7}, {34,7},
    {34,7}, {18,7}, {18,7}, {18,7}, {18,7}, {33,7}, {33,7}, {33,7}, {33,7},
    {17,7}, {17,7}, {17,7}, {17,7}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {48,6}, {48,6}, {48,6}, {48,6}, {48,6}, {48,6},
    {48,6}, {48,6}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5},
    {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3},
};//1024 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *MCBPCtab1;
static VLCtab MCBPCtab1_list[] =
#else
static VLCtab MCBPCtab1[] =
#endif
{
    {5,11}, {5,11},  {5,11}, {5,11}, {21,13}, {21,13}, {37,13}, {53,13},
};//32 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *MCBPCtabintra;
static VLCtab MCBPCtabintra_list[] =
#else
static VLCtab MCBPCtabintra[] =
#endif
{
    {-1,0},
    {20,6}, {36,6}, {52,6}, {4,4}, {4,4}, {4,4},
    {4,4}, {19,3}, {19,3}, {19,3}, {19,3}, {19,3},
    {19,3}, {19,3}, {19,3}, {35,3}, {35,3}, {35,3},
    {35,3}, {35,3}, {35,3}, {35,3}, {35,3}, {51,3},
    {51,3}, {51,3}, {51,3}, {51,3}, {51,3}, {51,3},
    {51,3},
};//128 bytes


#ifdef USE_INTERNAL_SRAM
static VLCtab *CBPYtab;
static VLCtab CBPYtab_list[48] =
#else
static VLCtab CBPYtab[48] =
#endif
{
    {-1,0}, {-1,0}, {9,6}, {6,6}, {7,5}, {7,5}, {11,5}, {11,5},
    {13,5}, {13,5}, {14,5}, {14,5}, {15,4}, {15,4}, {15,4}, {15,4},
    {3,4}, {3,4}, {3,4}, {3,4}, {5,4},{5,4},{5,4},{5,4},
    {1,4}, {1,4}, {1,4}, {1,4}, {10,4}, {10,4}, {10,4}, {10,4},
    {2,4}, {2,4}, {2,4}, {2,4}, {12,4}, {12,4}, {12,4}, {12,4},
    {4,4}, {4,4}, {4,4}, {4,4}, {8,4}, {8,4}, {8,4}, {8,4},
};//192 bytes
#else
//only for getvlc.c below tables.shenh
const static VLCtab TMNMVtab0[] =
{
    {3,4}, {61,4}, {2,3}, {2,3}, {62,3}, {62,3},
    {1,2}, {1,2}, {1,2}, {1,2}, {63,2}, {63,2}, {63,2}, {63,2}
};//56 bytes

const static VLCtab TMNMVtab1[] =
{
    {12,10}, {52,10}, {11,10}, {53,10}, {10,9}, {10,9},
    {54,9}, {54,9}, {9,9}, {9,9}, {55,9}, {55,9},
    {8,9}, {8,9}, {56,9}, {56,9}, {7,7}, {7,7},
    {7,7}, {7,7}, {7,7}, {7,7}, {7,7}, {7,7},
    {57,7}, {57,7}, {57,7}, {57,7}, {57,7}, {57,7},
    {57,7}, {57,7}, {6,7}, {6,7}, {6,7}, {6,7},
    {6,7}, {6,7}, {6,7}, {6,7}, {58,7}, {58,7},
    {58,7}, {58,7}, {58,7}, {58,7}, {58,7}, {58,7},
    {5,7}, {5,7}, {5,7}, {5,7}, {5,7}, {5,7},
    {5,7}, {5,7}, {59,7}, {59,7}, {59,7}, {59,7},
    {59,7}, {59,7}, {59,7}, {59,7}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {60,6}, {60,6},{60,6},{60,6},
    {60,6},{60,6},{60,6},{60,6},{60,6},{60,6},
    {60,6},{60,6},{60,6},{60,6},{60,6},{60,6}
};//384 bytes

const static VLCtab TMNMVtab2[] =
{
    {32,12}, {31,12}, {33,12}, {30,11}, {30,11}, {34,11},
    {34,11}, {29,11}, {29,11}, {35,11}, {35,11}, {28,11},
    {28,11}, {36,11}, {36,11}, {27,11}, {27,11}, {37,11},
    {37,11}, {26,11}, {26,11}, {38,11}, {38,11}, {25,11},
    {25,11}, {39,11}, {39,11}, {24,10}, {24,10}, {24,10},
    {24,10}, {40,10}, {40,10}, {40,10}, {40,10}, {23,10},
    {23,10}, {23,10}, {23,10}, {41,10}, {41,10}, {41,10},
    {41,10}, {22,10}, {22,10}, {22,10}, {22,10}, {42,10},
    {42,10}, {42,10}, {42,10}, {21,10}, {21,10}, {21,10},
    {21,10}, {43,10}, {43,10}, {43,10}, {43,10}, {20,10},
    {20,10}, {20,10}, {20,10}, {44,10}, {44,10}, {44,10},
    {44,10}, {19,10}, {19,10}, {19,10}, {19,10}, {45,10},
    {45,10}, {45,10}, {45,10}, {18,10}, {18,10}, {18,10},
    {18,10}, {46,10}, {46,10}, {46,10}, {46,10}, {17,10},
    {17,10}, {17,10}, {17,10}, {47,10}, {47,10}, {47,10},
    {47,10}, {16,10}, {16,10}, {16,10}, {16,10}, {48,10},
    {48,10}, {48,10}, {48,10}, {15,10}, {15,10}, {15,10},
    {15,10}, {49,10}, {49,10}, {49,10}, {49,10}, {14,10},
    {14,10}, {14,10}, {14,10}, {50,10}, {50,10}, {50,10},
    {50,10}, {13,10}, {13,10}, {13,10}, {13,10}, {51,10},
    {51,10}, {51,10}, {51,10}
};//492 bytes

const static VLCtab MCBPCtab0[] =
{
    {-1,0},
    {255,9}, {52,9}, {36,9}, {20,9}, {49,9}, {35,8}, {35,8}, {19,8}, {19,8},
    {50,8}, {50,8}, {51,7}, {51,7}, {51,7}, {51,7}, {34,7}, {34,7}, {34,7},
    {34,7}, {18,7}, {18,7}, {18,7}, {18,7}, {33,7}, {33,7}, {33,7}, {33,7},
    {17,7}, {17,7}, {17,7}, {17,7}, {4,6}, {4,6}, {4,6}, {4,6}, {4,6},
    {4,6}, {4,6}, {4,6}, {48,6}, {48,6}, {48,6}, {48,6}, {48,6}, {48,6},
    {48,6}, {48,6}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5},
    {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5}, {3,5},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {32,4},
    {32,4}, {32,4}, {32,4}, {32,4}, {32,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4}, {16,4},
    {16,4}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3}, {1,3},
    {1,3}, {1,3}, {1,3},
};//1024 bytes

const static VLCtab MCBPCtab1[] =
{
    {5,11}, {5,11},  {5,11}, {5,11}, {21,13}, {21,13}, {37,13}, {53,13},
};//32 bytes

const static VLCtab MCBPCtabintra[] =
{
    {-1,0},
    {20,6}, {36,6}, {52,6}, {4,4}, {4,4}, {4,4},
    {4,4}, {19,3}, {19,3}, {19,3}, {19,3}, {19,3},
    {19,3}, {19,3}, {19,3}, {35,3}, {35,3}, {35,3},
    {35,3}, {35,3}, {35,3}, {35,3}, {35,3}, {51,3},
    {51,3}, {51,3}, {51,3}, {51,3}, {51,3}, {51,3},
    {51,3},
};//128 bytes


const static VLCtab CBPYtab[48] =
{
    {-1,0}, {-1,0}, {9,6}, {6,6}, {7,5}, {7,5}, {11,5}, {11,5},
    {13,5}, {13,5}, {14,5}, {14,5}, {15,4}, {15,4}, {15,4}, {15,4},
    {3,4}, {3,4}, {3,4}, {3,4}, {5,4},{5,4},{5,4},{5,4},
    {1,4}, {1,4}, {1,4}, {1,4}, {10,4}, {10,4}, {10,4}, {10,4},
    {2,4}, {2,4}, {2,4}, {2,4}, {12,4}, {12,4}, {12,4}, {12,4},
    {4,4}, {4,4}, {4,4}, {4,4}, {8,4}, {8,4}, {8,4}, {8,4},
};//192 bytes
#endif


int getTMNMV ()
{
    unsigned int code;

    _showbits (13,code);
    _flushbits (1);

    if (code>>12)
    {
        return 0;
    }

    code = code & 0xfff;
    if (code >= 512)
    {
        code = (code >> 8) - 2;
        _flushbits (TMNMVtab0[code].len);

        return TMNMVtab0[code].val;
    }
    if (code >= 128)
    {
        code = (code >> 2) - 32;
        _flushbits (TMNMVtab1[code].len);

        return TMNMVtab1[code].val;
    }
    if ((code -= 5) < 0)
    {
        if (!quiet)
            //fprintf (stderr, "Invalid motion_vector code\n");
            diag_printf ("Invalid motion_vector code\n");
        fault = 1;
        return 0;
    }
    _flushbits (TMNMVtab2[code].len);

    return TMNMVtab2[code].val;
}

/**********************************************************************
 *
 *      Name:        getRVLC
 *      Description: extracts RVLC for motion vectors in Annex D
 *
 *      Input:
 *
 *      Returns:     motion vector component
 *      Side effects:
 *
 *      Date: 971026 Author: Guy Cote -- guyc@ee.ubc.ca
 *
 ***********************************************************************/

int getRVLC ()
{

    int code = 0, signID;
    unsigned int codetmp;
    int bit_cnt =0;

    _showbits(24,codetmp);

    if ((codetmp>>23)&1)
    {
        _flushbits (1);
        return 0;
    }

    code = 2 + ((codetmp>>22)&1);
    bit_cnt =3;

    while (((codetmp>>21)&1))
    {
        code <<= 1;
        codetmp <<=1;
        code += (codetmp>>21)&1;
        bit_cnt++;
    }

    _flushbits(bit_cnt);
    signID = code & 1;
    code >>= 1;
    return (signID) ? -code : code;
}


int getMCBPC ()
{
    unsigned int code;
    _showbits (13,code);

    if (code >> 4 == 1)
    {
        /* macroblock stuffing */
        _flushbits (9);
        return 255;
    }
    if (code == 0)
    {
        if (!quiet)
            //fprintf (stderr, "Invalid MCBPC code\n");
            diag_printf ("Invalid MCBPC code\n");
        fault = 1;
        return 0;
    }
    if (code >= 4096)
    {
        _flushbits (1);
        return 0;
    }
    if (code >= 16)
    {
        _flushbits (MCBPCtab0[code >> 4].len);
        return MCBPCtab0[code >> 4].val;
    }
    else
    {
        _flushbits (MCBPCtab1[code - 8].len);
        return MCBPCtab1[code - 8].val;
    }
}

/**********************************************************************
 *
 *  Name:        getMBTYPE
 *  Description: extracts annex O MBTYPE information from bitstream
 *               and whether or not CBP or Quant fields
 *               follow
 *
 *  Input:       pointers for cbp and dquant (to be filled in from
 *               MBTYPE table in annex O).
 *
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102 Author: Michael Gallant --- mikeg@ee.ubc.ca
 *
 ***********************************************************************/
#if 0
int getMBTYPE (int *cbp_present, int *quant_present)
{
    int code;


    code = showbits (9);

    if (code == 1)
    {
        /* macroblock stuffing */
        flushbits (9);
        *cbp_present = *quant_present = NO;
        return B_EI_EP_STUFFING;

    }

    switch (pict_type)
    {
        case PCT_B:

            if (code < 4)
            {
                if (!quiet)
                    //fprintf (stderr, "Invalid MBTYPE code\n");
                    diag_printf ( "Invalid MBTYPE code\n");
                fault = 1;
                *cbp_present = *quant_present = -1;
                return INVALID_MBTYPE;
            }

            code >>= 2;

            if (code >= 96)
            {
                flushbits (2);
                *cbp_present = YES;
                *quant_present = NO;
                return B_DIRECT_PREDICTION;
            }
            else
            {
                flushbits (MBTYPEtabB[code].len);
                *cbp_present = CBP_present_B[MBTYPEtabB[code].val];
                *quant_present = QUANT_present_B[MBTYPEtabB[code].val];
                return PRED_type_B[MBTYPEtabB[code].val];
            }

            break;

        case PCT_EP:

            if (code < 2)
            {
                if (!quiet)
                    //fprintf (stderr, "Invalid MBTYPE code\n");
                    diag_printf ( "Invalid MBTYPE code\n");
                fault = 1;
                *cbp_present = *quant_present = -1;
                return INVALID_MBTYPE;
            }

            code >>= 1;

            if (code >= 128)
            {
                flushbits (1);
                *cbp_present = YES;
                *quant_present = NO;
                return EP_FORWARD_PREDICTION;
            }
            else
            {
                flushbits (MBTYPEtabEP[code].len);
                *cbp_present = CBP_present_EP[MBTYPEtabEP[code].val];
                *quant_present = QUANT_present_EP[MBTYPEtabEP[code].val];
                return PRED_type_EP[MBTYPEtabEP[code].val];
            }

            break;

        case PCT_EI:

            /* In the case of EI pictures, we use cbp_present to return the chrominance
             * coded block pattern */
            if (code < 2)
            {
                if (!quiet)
                    diag_printf ("Invalid MBTYPE code\n");
                fault = 1;
                *cbp_present = *quant_present = -1;
                return INVALID_MBTYPE;
            }

            code >>= 1;

            if (8 == code)
            {
                if (!quiet)
                    //fprintf (stderr, "Invalid MBTYPE code\n");
                    diag_printf ( "Invalid MBTYPE code\n");
                fault = 1;
                *cbp_present = *quant_present = -1;
                return INVALID_MBTYPE;
            }

            if (code >= 128)
            {
                flushbits (1);
                *cbp_present = 0;
                *quant_present = NO;
                return EI_EP_UPWARD_PREDICTION;
            }
            else
            {
                flushbits (MBTYPEtabEI[code].len);
                *cbp_present = CBPC_pattern_EI[MBTYPEtabEI[code].val];
                *quant_present = QUANT_present_EI[MBTYPEtabEI[code].val];
                return PRED_type_EI[MBTYPEtabEI[code].val];
            }

            break;

        default:

            break;

    }
}
#endif

#if 0
int getMODB ()
{
    int code;
    int MODB;


    if (pict_type == PCT_IPB)
    {
        code = getbits (1);
        if (code == 0)
        {
            MODB = PBMODE_BIDIR_PRED;
        }
        else
        {
            code=getbits (1);
            if (code==0)
            {
                MODB = PBMODE_CBPB_BIDIR_PRED;
            }
            else
            {
                code=getbits (1);
                if (code==0)
                {
                    MODB = PBMODE_FRW_PRED;
                }
                else
                {
                    code=getbits (1);
                    if (code==0)
                    {
                        MODB = PBMODE_CBPB_FRW_PRED;
                    }
                    else
                    {
                        code=getbits (1);
                        if (code==0)
                        {
                            MODB = PBMODE_BCKW_PRED;
                        }
                        else
                        {
                            MODB = PBMODE_CBPB_BCKW_PRED;
                        }
                    }
                }
            }
        }
    }
    else
    {
        code = showbits (2);

        if (code < 2)
        {
            MODB = 0;
            flushbits (1);
        }
        else if (code == 2)
        {
            MODB = 1;
            flushbits (2);
        }
        else
        {
            /* code == 3 */
            MODB = 2;
            flushbits (2);
        }
    }

    return MODB;

}
#endif


int getMCBPCintra ()
{
    unsigned int code;
    _showbits (9,code);

    if (code == 1)
    {
        /* macroblock stuffing */
        _flushbits (9);
        return 255;

    }
    if (code < 8)
    {
        if (!quiet)
            //fprintf (stderr, "Invalid MCBPCintra code\n");
            diag_printf ( "Invalid MCBPCintra code\n");
        fault = 1;
        return 0;
    }
    code >>= 3;

    if (code >= 32)
    {
        _flushbits (1);
        return 3;
    }
    _flushbits (MCBPCtabintra[code].len);

    return MCBPCtabintra[code].val;
}

/* extract vlc representing true B CBPC --- mikeg@ee.ubc.ca */
#if 0
int getscalabilityCBPC ()
{
    int code;


    code = showbits (1);

    if (0 == code)
    {
        flushbits (1);
        return 0;
    }
    else
    {
        code = showbits (2);

        if (2 == code)
        {
            flushbits (2);
            return 1;
        }
        else
        {
            code = showbits (3);

            if (6 == code)
            {
                flushbits (3);
                return 3;
            }
            else if (7 == code)
            {
                flushbits (3);
                return 2;
            }
            else
            {
                if (!quiet)
                    diag_printf ("Invalid chromiance CBP in getscalabilityCBPC (getvlc.c).\n");
                fault = 1;
                return 0;
            }
        }
    }
}
#endif

int getCBPY ()
{
    unsigned int code;
    _showbits (6,code);
    if (code < 2)
    {
        if (!quiet)
            //fprintf (stderr, "Invalid CBPY code\n");
            diag_printf ( "Invalid CBPY code\n");
        fault = 1;
        return -1;
    }
    if (code >= 48)
    {
        _flushbits (2);
        return 0;
    }
    _flushbits (CBPYtab[code].len);

    return CBPYtab[code].val;
}

#if 0
void init_vlcTab()//add shenh
{
#if 1
    MCBPCtabintra=(VLCtab*)MCBPCTABINTRA_START;
    CBPYtab=(VLCtab*)CBPYTAB_START;

    TMNMVtab0=(VLCtab*)TMNMVTAB0_START;
    TMNMVtab1=(VLCtab*)TMNMVTAB1_START;
    TMNMVtab2=(VLCtab*)TMNMVTAB2_START;

    MCBPCtab0=(VLCtab*)MCBPCTAB0_START;
    MCBPCtab1=(VLCtab*)MCBPCTAB1_START;

    memcpy(MCBPCtabintra, MCBPCtabintra_list, 128);
    memcpy(CBPYtab, CBPYtab_list, 192);

    memcpy(TMNMVtab0, TMNMVtab0_list, 56);
    memcpy(TMNMVtab1, TMNMVtab1_list, 384);
    memcpy(TMNMVtab2, TMNMVtab2_list, 492);

    memcpy(MCBPCtab0, MCBPCtab0_list, 1024);
    memcpy(MCBPCtab1, MCBPCtab1_list, 32);
#else
    MCBPCtabintra=MCBPCtabintra_list;
    CBPYtab= CBPYtab_list;

    TMNMVtab0= TMNMVtab0_list;
    TMNMVtab1= TMNMVtab1_list;
    TMNMVtab2= TMNMVtab2_list;

    MCBPCtab0= MCBPCtab0_list;
    MCBPCtab1= MCBPCtab1_list;
#endif

}
#endif

#if 0
void create_tab_for_voc(void)
{
#include "getvlc.h"
    int i;
    FILE *pH;
    char *p;

    const short idct_const_number[64] =
    {
        1108,3784,1568,565,2276,3406,
        2408,799,4017,181,4,32,
        63,56,31,0x80,0,0x2000,0,
    };
    /*
    extern VLCtab MCBPCtabintra[];
    extern VLCtab MCBPCtab0[];
    extern VLCtab MCBPCtab1[];
    extern VLCtab TMNMVtab0[];
    extern VLCtab TMNMVtab1[];
    extern VLCtab TMNMVtab2[];
    extern VLCtab CBPYtab[];
    */
    extern VLCtab DCT3Dtab0[];
    extern VLCtab DCT3Dtab1[];
    extern VLCtab DCT3Dtab2[];
    extern unsigned char zig_zag_scan[64];

    pH=fopen("E:\\projects\\gallite_hx_simu\\platform_simu\\voc\\simus\\audiojpegH263\\vpp_h263_dec_const_y.tab","wb");

    fprintf(pH, "/* MCBPCtabintra */ \n");
    for(i=0; i<16; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)(MCBPCtabintra[2*i+1].val));
        fprintf(pH, "%02hx",MCBPCtabintra[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)(MCBPCtabintra[2*i].val));
        fprintf(pH, "%02hx,",MCBPCtabintra[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* MCBPCtab0 */ \n");
    for(i=0; i<128; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)MCBPCtab0[2*i+1].val);
        fprintf(pH, "%02hx",MCBPCtab0[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)MCBPCtab0[2*i].val);
        fprintf(pH, "%02hx,",MCBPCtab0[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* MCBPCtab1 */ \n");
    for(i=0; i<4; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)MCBPCtab1[2*i+1].val);
        fprintf(pH, "%02hx",MCBPCtab1[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)MCBPCtab1[2*i].val);
        fprintf(pH, "%02hx,",MCBPCtab1[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* TMNMVtab0 */ \n");
    for(i=0; i<7; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)TMNMVtab0[2*i+1].val);
        fprintf(pH, "%02hx",TMNMVtab0[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)TMNMVtab0[2*i].val);
        fprintf(pH, "%02hx,",TMNMVtab0[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* TMNMVtab1 */ \n");
    for(i=0; i<48; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)TMNMVtab1[2*i+1].val);
        fprintf(pH, "%02hx",TMNMVtab1[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)TMNMVtab1[2*i].val);
        fprintf(pH, "%02hx,",TMNMVtab1[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* TMNMVtab2 */ \n");
    for(i=0; i<61; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)TMNMVtab2[2*i+1].val);
        fprintf(pH, "%02hx",TMNMVtab2[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)TMNMVtab2[2*i].val);
        fprintf(pH, "%02hx,",TMNMVtab2[2*i].len);
        fprintf(pH, "\n");
    }

    fprintf(pH, "0x%04hx",idct_const_number[0]);
    fprintf(pH, "%02hx",(unsigned char)TMNMVtab2[122].val);
    fprintf(pH, "%02hx,",TMNMVtab2[122].len);

    fprintf(pH, "\n\n");

    fprintf(pH, "/* idct_const_number [1]start  */ \n");
    for(i=0; i<9; i++)
    {
        fprintf(pH, "0x%04hx",idct_const_number[2*i+2]);
        fprintf(pH, "%04hx,",idct_const_number[2*i+1]);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* CBPYtab */ \n");
    for(i=0; i<24; i++)
    {
        fprintf(pH, "0x%02hx",(unsigned char)CBPYtab[2*i+1].val);
        fprintf(pH, "%02hx",CBPYtab[2*i+1].len);
        fprintf(pH, "%02hx",(unsigned char)CBPYtab[2*i].val);
        fprintf(pH, "%02hx,",CBPYtab[2*i].len);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* DCT3Dtab0 */ \n");
    for(i=0; i<112; i++)
    {
        fprintf(pH, "0x%04hx",DCT3Dtab0[i].len);
        fprintf(pH, "%04hx,",DCT3Dtab0[i].val);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* DCT3Dtab1 */ \n");
    for(i=0; i<96; i++)
    {
        fprintf(pH, "0x%04hx",DCT3Dtab1[i].len);
        fprintf(pH, "%04hx,",DCT3Dtab1[i].val);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* DCT3Dtab2 */ \n");
    for(i=0; i<121; i++)
    {
        fprintf(pH, "0x%04hx",DCT3Dtab2[i].len);
        fprintf(pH, "%04hx,",DCT3Dtab2[i].val);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fprintf(pH, "/* zig_zag_scan */ \n");
    for(i=0; i<32; i++)
    {
        fprintf(pH, "0x%04hx",zig_zag_scan[2*i+1]);
        fprintf(pH, "%04hx,",zig_zag_scan[2*i]);
        fprintf(pH, "\n");
    }
    fprintf(pH, "\n\n");

    fflush(pH);
    fclose(pH);


    return;
}
#endif

#endif

#endif// MEDIA_VOCVID_SUPPORT
