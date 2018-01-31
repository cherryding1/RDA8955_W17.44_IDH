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
//#include <stdlib.h>

#include "h263_config.h"
#include "tmndec.h"
#include "h263_global.h"
#ifndef SHEEN_VC_DEBUG
#include "mcip_debug.h"
#endif

#ifdef MP4_3GP_SUPPORT
//#include "zigzag.h"//for scan_tables[3][64]

#define INDICES
#include "indices.h"

#define SACTABLES
#include "sactbls.h"

#include "getvlc.h"//shenh add
/* change shenh
typedef struct
{
  char run, level, len;
} DCTtab;
typedef struct
{
  int val, len;
} VLCtabI;
*/


typedef struct
{
    int val, run, sign;
} RunCoef;

typedef struct
{
    int code, level, last, run, sign, extended_level;
} CodeCoeff;

static CodeCoeff *store_code;// size 6*4*64

//move from getvlc.h. shenh 6128 bytes
#ifdef USE_INTERNAL_SRAM
static VLCtab *DCT3Dtab0;
const static VLCtab DCT3Dtab0_list[]=
#else
const /*static*/ VLCtab DCT3Dtab0[] =
#endif
{
    {4225,7}, {4209,7}, {4193,7}, {4177,7}, {193,7}, {177,7},
    {161,7}, {4,7}, {4161,6}, {4161,6}, {4145,6}, {4145,6},
    {4129,6}, {4129,6}, {4113,6}, {4113,6}, {145,6}, {145,6},
    {129,6}, {129,6}, {113,6}, {113,6}, {97,6}, {97,6},
    {18,6}, {18,6}, {3,6}, {3,6}, {81,5}, {81,5},
    {81,5}, {81,5}, {65,5}, {65,5}, {65,5}, {65,5},
    {49,5}, {49,5}, {49,5}, {49,5}, {4097,4}, {4097,4},
    {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {17,3}, {17,3}, {17,3}, {17,3},
    {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3},
    {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3},
    {33,4}, {33,4}, {33,4}, {33,4}, {33,4}, {33,4},
    {33,4}, {33,4}, {2,4}, {2,4},{2,4},{2,4},
    {2,4}, {2,4},{2,4},{2,4},
};//448 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *DCT3Dtab1;
const static VLCtab DCT3Dtab1_list[] =
#else
const /*static*/ VLCtab DCT3Dtab1[] =
#endif
{
    {9,10}, {8,10}, {4481,9}, {4481,9}, {4465,9}, {4465,9},
    {4449,9}, {4449,9}, {4433,9}, {4433,9}, {4417,9}, {4417,9},
    {4401,9}, {4401,9}, {4385,9}, {4385,9}, {4369,9}, {4369,9},
    {4098,9}, {4098,9}, {353,9}, {353,9}, {337,9}, {337,9},
    {321,9}, {321,9}, {305,9}, {305,9}, {289,9}, {289,9},
    {273,9}, {273,9}, {257,9}, {257,9}, {241,9}, {241,9},
    {66,9}, {66,9}, {50,9}, {50,9}, {7,9}, {7,9},
    {6,9}, {6,9}, {4353,8}, {4353,8}, {4353,8}, {4353,8},
    {4337,8}, {4337,8}, {4337,8}, {4337,8}, {4321,8}, {4321,8},
    {4321,8}, {4321,8}, {4305,8}, {4305,8}, {4305,8}, {4305,8},
    {4289,8}, {4289,8}, {4289,8}, {4289,8}, {4273,8}, {4273,8},
    {4273,8}, {4273,8}, {4257,8}, {4257,8}, {4257,8}, {4257,8},
    {4241,8}, {4241,8}, {4241,8}, {4241,8}, {225,8}, {225,8},
    {225,8}, {225,8}, {209,8}, {209,8}, {209,8}, {209,8},
    {34,8}, {34,8}, {34,8}, {34,8}, {19,8}, {19,8},
    {19,8}, {19,8}, {5,8}, {5,8}, {5,8}, {5,8},
};//384 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *DCT3Dtab2;
const static VLCtab DCT3Dtab2_list[] =
#else
const /*static*/ VLCtab DCT3Dtab2[] =
#endif
{
    {4114,11}, {4114,11}, {4099,11}, {4099,11}, {11,11}, {11,11},
    {10,11}, {10,11}, {4545,10}, {4545,10}, {4545,10}, {4545,10},
    {4529,10}, {4529,10}, {4529,10}, {4529,10}, {4513,10}, {4513,10},
    {4513,10}, {4513,10}, {4497,10}, {4497,10}, {4497,10}, {4497,10},
    {146,10}, {146,10}, {146,10}, {146,10}, {130,10}, {130,10},
    {130,10}, {130,10}, {114,10}, {114,10}, {114,10}, {114,10},
    {98,10}, {98,10}, {98,10}, {98,10}, {82,10}, {82,10},
    {82,10}, {82,10}, {51,10}, {51,10}, {51,10}, {51,10},
    {35,10}, {35,10}, {35,10}, {35,10}, {20,10}, {20,10},
    {20,10}, {20,10}, {12,11}, {12,11}, {21,11}, {21,11},
    {369,11}, {369,11}, {385,11}, {385,11}, {4561,11}, {4561,11},
    {4577,11}, {4577,11}, {4593,11}, {4593,11}, {4609,11}, {4609,11},
    {22,12}, {36,12}, {67,12}, {83,12}, {99,12}, {162,12},
    {401,12}, {417,12}, {4625,12}, {4641,12}, {4657,12}, {4673,12},
    {4689,12}, {4705,12}, {4721,12}, {4737,12}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7},
};//484 bytes


/* Tables for qcoeff in advanced intra coding (Annex I) */
#ifdef USE_INTERNAL_SRAM
static VLCtab *INTRA_DCT3Dtab0;
const static VLCtab INTRA_DCT3Dtab0_list[] =
#else
const static VLCtab INTRA_DCT3Dtab0[] =
#endif
{
    {4099,7}, {4353,7}, {4481,7}, {4417,7}, {67,7}, {130,7},
    {9,7}, {321,7}, {4098,6}, {4098,6}, {4289,6}, {4289,6},
    {4225,6}, {4225,6}, {4161,6}, {4161,6}, {6,6}, {6,6},
    {7,6}, {7,6}, {8,6}, {8,6}, {257,6}, {257,6},
    {66,6}, {66,6}, {193,6}, {193,6}, {129,5}, {129,5},
    {129,5}, {129,5}, {4,5}, {4,5}, {4,5}, {4,5},
    {5,5}, {5,5}, {5,5}, {5,5}, {4097,4}, {4097,4},
    {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2},
    {1,2}, {1,2}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {2,3}, {2,3}, {2,3}, {2,3}, {2,3}, {2,3},
    {3,4}, {3,4}, {3,4}, {3,4}, {3,4}, {3,4},
    {3,4}, {3,4}, {65,4}, {65,4},{65,4},{65,4},
    {65,4}, {65,4},{65,4},{65,4},
};//448 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *INTRA_DCT3Dtab1;
const static VLCtab INTRA_DCT3Dtab1_list[] =
#else
const static VLCtab INTRA_DCT3Dtab1[] =
#endif
{
    {705,10}, {641,10},{4101,9}, {4101,9}, {4102,9}, {4102,9},
    {4162,9}, {4162,9}, {4226,9}, {4226,9},{5057,9}, {5057,9},
    {5121,9}, {5121,9}, {5249,9}, {5249,9},{5185,9}, {5185,9},
    {4993,9}, {4993,9}, {13,9}, {13,9},{14,9}, {14,9},
    {15,9}, {15,9}, {16,9}, {16,9},{17,9}, {17,9},
    {18,9}, {18,9}, {11,9}, {11,9},{12,9}, {12,9},
    {322,9}, {322,9}, {258,9}, {258,9}, {577,9}, {577,9},
    {513,9}, {513,9}, {4100,8}, {4100,8}, {4100,8}, {4100,8},
    {4545,8}, {4545,8}, {4545,8}, {4545,8}, {4609,8}, {4609,8},
    {4609,8}, {4609,8}, {4929,8}, {4929,8}, {4929,8}, {4929,8},
    {4865,8}, {4865,8}, {4865,8}, {4865,8}, {4801,8}, {4801,8},
    {4801,8}, {4801,8}, {4737,8}, {4737,8}, {4737,8}, {4737,8},
    {4673,8}, {4673,8}, {4673,8}, {4673,8}, {10,8}, {10,8},
    {10,8}, {10,8}, {385,8}, {385,8}, {385,8}, {385,8},
    {194,8}, {194,8}, {194,8}, {194,8}, {68,8}, {68,8},
    {68,8}, {68,8}, {449,8}, {449,8}, {449,8}, {449,8},
};//384 bytes

#ifdef USE_INTERNAL_SRAM
static VLCtab *INTRA_DCT3Dtab2;
const static VLCtab INTRA_DCT3Dtab2_list[] =
#else
const static VLCtab INTRA_DCT3Dtab2[] =
#endif
{
    {5313,11}, {5313,11}, {5377,11}, {5377,11}, {578,11}, {578,11},
    {259,11}, {259,11}, {4103,10}, {4103,10}, {4103,10}, {4103,10},
    {4163,10}, {4163,10}, {4163,10}, {4163,10}, {4290,10}, {4290,10},
    {4290,10}, {4290,10}, {4354,10}, {4354,10}, {4354,10}, {4354,10},
    {769,10}, {769,10}, {769,10}, {769,10}, {132,10}, {132,10},
    {132,10}, {132,10}, {514,10}, {514,10}, {514,10}, {514,10},
    {450,10}, {450,10}, {450,10}, {450,10}, {386,10}, {386,10},
    {386,10}, {386,10}, {195,10}, {195,10}, {195,10}, {195,10},
    {131,10}, {131,10}, {131,10}, {131,10}, {69,10}, {69,10},
    {69,10}, {69,10}, {833,11}, {833,11}, {70,11}, {70,11},
    {20,11}, {20,11}, {19,11}, {19,11}, {4227,11}, {4227,11},
    {4164,11}, {4164,11}, {4105,11}, {4105,11}, {4104,11}, {4104,11},
    {71,12}, {196,12}, {323,12}, {25,12}, {24,12}, {23,12},
    {22,12}, {21,12}, {5441,12}, {5505,12}, {5569,12}, {4546,12},
    {4482,12}, {4418,12}, {4291,12}, {4106,12}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7},
    {7167,7},
};//480 bytes


/* zig-zag scan */
//use shenh. only for getblk.c
#ifdef USE_INTERNAL_SRAM
unsigned char *zig_zag_scan;
const unsigned char zig_zag_scan_list[64]=
#else
const unsigned char zig_zag_scan[64]=
#endif
{
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

#ifdef USE_INTERNAL_SRAM
char *MQ_chroma_QP_table;
const char MQ_chroma_QP_table_list[32] =
#else
const char MQ_chroma_QP_table[32] =
#endif
{
    0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13,
    14, 14, 14, 14, 14, 15, 15, 15, 15, 15
};

/* local prototypes */
/*
RunCoef vlc_word_decode _ANSI_ARGS_ ((int symbol_word, int *last));
RunCoef Decode_Escape_Char _ANSI_ARGS_ ((int intra, int *last));
int DecodeTCoef _ANSI_ARGS_ ((int position, int intra));
*/



//extern VLCtabI DCT3Dtab0[], DCT3Dtab1[], DCT3Dtab2[];

/* Tables used in advanced intra coding mode */
//extern VLCtabI INTRA_DCT3Dtab0[], INTRA_DCT3Dtab1[], INTRA_DCT3Dtab2[];

//extern MQ_chroma_QP_table[];

/**********
copy 6 dct table into internal sram
***********/

void DctTabMov()//add shenh
{
#ifdef USE_INTERNAL_SRAM
    //int res;
    DCT3Dtab0=(VLCtab *)DCT3DTAB0_START;
    DCT3Dtab1=(VLCtab *)DCT3DTAB1_START;
    DCT3Dtab2=(VLCtab *)DCT3DTAB2_START;

    INTRA_DCT3Dtab0=(VLCtab *)INTRA_DCT3DTAB0_START;
    INTRA_DCT3Dtab1=(VLCtab *)INTRA_DCT3DTAB1_START;
    INTRA_DCT3Dtab2=(VLCtab *)INTRA_DCT3DTAB2_START;

    zig_zag_scan=(unsigned char *)ZIGZAG_SCAN_START;
    MQ_chroma_QP_table=(char*)MQ_CHROMA_QP_TABLE_START;

    memcpy(DCT3Dtab0, DCT3Dtab0_list, 448);
    memcpy(DCT3Dtab1, DCT3Dtab1_list, 384);
    memcpy(DCT3Dtab2, DCT3Dtab2_list, 480);

    memcpy(INTRA_DCT3Dtab0, INTRA_DCT3Dtab0_list, 448);
    memcpy(INTRA_DCT3Dtab1, INTRA_DCT3Dtab1_list, 384);
    memcpy(INTRA_DCT3Dtab2, INTRA_DCT3Dtab2_list, 480);

    memcpy(zig_zag_scan, zig_zag_scan_list, 64);
    memcpy(MQ_chroma_QP_table, MQ_chroma_QP_table_list, 32);

    store_code=(CodeCoeff*)STORE_CODE_START;
#else
    store_code=(CodeCoeff*)mmc_MemMalloc(64*sizeof(CodeCoeff));
#endif
}


void getblock (int comp, int mode, int INTRA_AC_DC, int Mode)
{
    int val, i, j, signID;
    unsigned int code;
    //VLCtabI *tab;//change shenh
    VLCtab *tab;
    short *bp;
    int run, last, level, QP;
    short *qval;
    /* store_code used in alternative inter VLC mode */
    int use_intra = 0;
    //CodeCoeff store_code[64];//shenh
    int coeff_ind;
    char ModeID;//shenh
    unsigned int code_tmp;

    errorstate=0;

#ifdef USE_INTERNAL_SRAM
    bp = ld->block+comp*64;
#else
    //bp = ld->block[comp];
    bp = ld->block+comp*64;
#endif

    /* decode AC coefficients (or all coefficients in advanced intra coding
     * mode) */
    ModeID=0;
    if (alternative_inter_VLC_mode && !(Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
    {
        coeff_ind = 0;
        use_intra = 0;
        ModeID=1;
        for (i = (mode == 0);; i++)
        {
            _showbits (13,code_tmp);
            code = code_tmp>>1;//变换系数
            store_code[coeff_ind].code = code;
            if (code >= 512)
                tab =(VLCtab *)&DCT3Dtab0[(code >> 5) - 16];
            else if (code >= 128)
                tab = (VLCtab *)&DCT3Dtab1[(code >> 2) - 32];
            else if (code >= 8)
                tab = (VLCtab *)&DCT3Dtab2[(code >> 0) - 8];
            else
            {
                if (!quiet)
                    diag_printf ("invalid Huffman code in getblock()\n");
                fault = 1;
                return;
            }

            run = (tab->val >> 4) & 255;
            last = (tab->val >> 12) & 1;

            if (tab->val == ESCAPE)
            {
                _flushbits (tab->len);
                code_tmp = getbits (15);
                last = code_tmp>>14;
                store_code[coeff_ind].last = last;

                i += run = (code_tmp>>8) & 0x3f;
                store_code[coeff_ind].run = run;

                level = code_tmp & 0xff;
                store_code[coeff_ind].level = level;

                if ( modified_quantization_mode )
                {
                    if(level == 128)
                    {
                        level = getbits (11);
                        store_code[coeff_ind].extended_level = level;
                    }
                }
            }
            else
            {
                store_code[coeff_ind].sign = (code_tmp>>(12-tab->len)) & 1;
                _flushbits (1+tab->len);
                i += run;
            }
            coeff_ind++;

            if (i >= 64)
                use_intra = 1;
            if (last)
                break;
        }
    }
    coeff_ind = 0;

    QP = quant;

    if (modified_quantization_mode )
    {
        if(comp == 4 || comp == 5)
        {
            QP = MQ_chroma_QP_table[QP];
        }
    }

    if(ModeID)
    {
        if (use_intra)
        {
            for (i = (mode == 0);; i++)
            {
                if ((store_code[coeff_ind].code) >= 512)
                    tab = (VLCtab *)&INTRA_DCT3Dtab0[((store_code[coeff_ind].code) >> 5) - 16];
                else if ((store_code[coeff_ind].code) >= 128)
                    tab = (VLCtab *)&INTRA_DCT3Dtab1[((store_code[coeff_ind].code) >> 2) - 32];
                else if ((store_code[coeff_ind].code) >= 8)
                    tab = (VLCtab *)&INTRA_DCT3Dtab2[((store_code[coeff_ind].code) >> 0) - 8];
                else
                {
                    if (!quiet)
                        diag_printf ( "invalid Huffman code in getblock()\n");
                    fault = 1;
                    return;
                }
                run = (tab->val >> 6) & 63;
                level = tab->val & 63;
                last = (tab->val >> 12) & 1;

                if (tab->val == ESCAPE)
                {
                    /* escape */
                    last = store_code[coeff_ind].last;
                    i += run = store_code[coeff_ind].run;
                    level = store_code[coeff_ind].level;

                    if ((signID = (level >= 128)))
                        val = 256 - level;
                    else
                        val = level;

                    if (level == 128 && modified_quantization_mode)
                    {
                        level = store_code[coeff_ind].extended_level;
                        level = (level >> 6 & 0x001F) | level << 5;
                        /* correctly set the sign */
                        level = (level << (sizeof (int) * 8 - 11)) >> (sizeof (int) * 8 - 11);
                        if (level >= 0)
                            signID = 0;
                        else
                            signID = 1;
                        val = ABS (level);
                    }
                }
                else
                {
                    signID = store_code[coeff_ind].sign;
                    i += run;
                    val = level;

                }
                coeff_ind += 1;

                if (i >= 64)
                {
                    if (!quiet)
                        diag_printf ( "DCT coeff index (i) out of bounds\n");
                    fault = 1;
                    return;
                }

                j = zig_zag_scan[i];//change shenh
                qval = &bp[j];
                val = val<<1;
                val = QP * (val +1) - 1 + (QP&1);
                *qval = signID ? -val : val;

                if (last)
                {
                    return;
                }
            }
        }
        else
        {
            for (i = (mode == 0);; i++)
            {
                if (store_code[coeff_ind].code >= 512)
                    tab = (VLCtab *)&DCT3Dtab0[(store_code[coeff_ind].code >> 5) - 16];
                else if (store_code[coeff_ind].code >= 128)
                    tab = (VLCtab *)&DCT3Dtab1[(store_code[coeff_ind].code >> 2) - 32];
                else if (store_code[coeff_ind].code >= 8)
                    tab =(VLCtab *)&DCT3Dtab2[(store_code[coeff_ind].code >> 0) - 8];
                else
                {
                    if (!quiet)
                        diag_printf ("invalid Huffman code in getblock()\n");
                    fault = 1;
                    return;
                }

                run = (tab->val >> 4) & 255;
                level = tab->val & 15;
                last = (tab->val >> 12) & 1;

                if (tab->val == ESCAPE)
                {
                    /* escape */
                    last = store_code[coeff_ind].last;
                    i += run = store_code[coeff_ind].run;
                    level = store_code[coeff_ind].level;

                    if ((signID = (level >= 128)))
                        val = 256 - level;
                    else
                        val = level;

                    if (level == 128 && modified_quantization_mode)
                    {
                        level = store_code[coeff_ind].extended_level;
                        level = (level >> 6 & 0x001F) | level << 5;
                        /* correctly set the sign */
                        level = (level << (sizeof (int) * 8 - 11)) >> (sizeof (int) * 8 - 11);
                        if (level >= 0)
                            signID = 0;
                        else
                            signID = 1;
                        val = ABS (level);
                    }
                }
                else
                {
                    signID = store_code[coeff_ind].sign;
                    i += run;
                    val = level;
                }
                coeff_ind += 1;

                if (i >= 64)
                {
                    if (!quiet)
                        diag_printf ( "DCT coeff index (i) out of bounds\n");
                    fault = 1;
                    return;
                }

                j = zig_zag_scan[i];
                qval = &bp[j];
                val = val<<1;
                val = QP * (val +1) - 1 + (QP &1);
                *qval = signID ? -val : val;

                if (last)
                {
                    return;
                }
            }
        }
    }
    else
    {
        for (i = (mode == 0);; i++)
        {
            _showbits (13,code_tmp);
            code = code_tmp>>1;//变换系数

            if (code >= 512)
                tab = (VLCtab *)&DCT3Dtab0[(code >> 5) - 16];
            else if (code >= 128)
                tab = (VLCtab *)&DCT3Dtab1[(code >> 2) - 32];
            else if (code >= 8)
                tab = (VLCtab *)&DCT3Dtab2[(code) - 8];
            else
            {
                if (!quiet)
                    diag_printf ( "invalid Huffman code in getblock()\n");
                fault = 1;
                return;
            }

            run = (tab->val >> 4) & 255;
            level = tab->val & 15;
            last = (tab->val >> 12) & 1;

            if (tab->val == ESCAPE)
            {
                _flushbits (tab->len);
                code_tmp = getbits (15);
                last = code_tmp>>14;
                i += run = (code_tmp>>8) & 0x3f;
                level = code_tmp & 0xff;

                if ((signID = (level >= 128)))
                    val = 256 - level;
                else
                    val = level;

                if (level == 128 && modified_quantization_mode)
                {
                    level = getbits (11);
                    level = (level >> 6 & 0x001F) | level << 5;
                    level = (level << (sizeof (int) * 8 - 11)) >> (sizeof (int) * 8 - 11);
                    if (level >= 0)
                        signID = 0;
                    else
                        signID = 1;
                    val = ABS (level);
                }
            }
            else
            {
                i += run;
                val = level;
                signID = (code_tmp>>(12-tab->len)) & 1;
                _flushbits (1+tab->len);
            }

            if (i >= 64)
            {
                if (!quiet)
                    diag_printf ( "DCT coeff index (i) out of bounds\n");
                fault = 1;
                return;
            }

            j = zig_zag_scan[i];
            qval = &bp[j];
            val = val<<1;
            val = QP * (val +1) - 1 + (QP &1);
            *qval = signID ? -val : val;

            if (last)
            {
                return;
            }
        }
    }
}



/*********************************************************************
 *
 *        Name:        get_sac_block
 *
 *  Description:    Decodes blocks of Arithmetic Encoded DCT Coeffs.
 *        and performs Run Length Decoding and Coefficient
 *        Dequantisation.
 *
 *  Input:        Picture block type and number.
 *
 *  Returns:    Nothing.
 *
 *  Side Effects:
 *
 *  Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
 *
 *********************************************************************/

#if 0
void get_sac_block (int comp, int ptype, int INTRA_AC_DC, int Mode)
{
    int position = 0;
    int TCOEF_index, symbol_word;
    int last = 0, QP, i, j;
    short *qval;
    short *bp;
    RunCoef DCTcoef;
    int intra;
    errorstate=0;

    bp = ld->block[comp];

    i = (ptype == 0);
    intra = (Mode == MODE_INTRA || Mode == MODE_INTRA_Q);

    while (!last)
    {
        /* while there are DCT coefficients
         * remaining */
        position++;                 /* coefficient counter relates to Coeff.
                                 * model */
        TCOEF_index = DecodeTCoef (position, intra);

        if (TCOEF_index == ESCAPE_INDEX)
        {
            /* ESCAPE code encountered */
            DCTcoef = Decode_Escape_Char (intra, &last);
        }
        else
        {
            symbol_word = tcoeftab[TCOEF_index];

            DCTcoef = vlc_word_decode (symbol_word, &last);
        }

        i += DCTcoef.run;
        if (advanced_intra_coding && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
        {
            switch (INTRA_AC_DC)
            {
                case INTRA_MODE_DC:
                    j = zig_zag_scan[i];//change shenh
                    // j = scan_tables[0][i];
                    break;
                case INTRA_MODE_VERT_AC:
                    j = alternate_horizontal_scan[i];
                    // j = scan_tables[1][i];
                    break;
                case INTRA_MODE_HORI_AC:
                    j = alternate_vertical_scan[i];
                    // j = scan_tables[2][i];
                    break;
                default:
                    diag_printf ("Invalid Intra_Mode in Advanced Intra Coding");
                    //  exit (-1);
                    errorstate=-1;
                    return;
                    // break;
            }
        }
        else
        {
            j = zig_zag_scan[i];//change shenh
            // j = scan_tables[0][i];
        }

        qval = &bp[j];

        i++;

        if (comp >= 6)
            QP = mmax (1, mmin (31, (bquant_tab[bquant] * quant) >> 2));
        else
            QP = quant;
        if ((advanced_intra_coding) && (Mode == MODE_INTRA || Mode == MODE_INTRA_Q))
        {
            /* Advanced intra coding dequantization */
            *qval = (DCTcoef.sign ? -(QP * (2 * DCTcoef.val)) : QP * (2 * DCTcoef.val));
        }
        else
        {

            if ((QP % 2) == 1)
                *qval = ((DCTcoef.sign) ? -(QP * (2 * (DCTcoef.val) + 1)) :
                         QP * (2 * (DCTcoef.val) + 1));
            else
                *qval = ((DCTcoef.sign) ? -(QP * (2 * (DCTcoef.val) + 1) - 1) :
                         QP * (2 * (DCTcoef.val) + 1) - 1);
        }
    }
    return;
}
#endif

/*********************************************************************
 *
 *        Name:        vlc_word_decode
 *
 *  Description:    Fills Decoder FIFO after a fixed word length
 *        string has been detected.
 *
 *  Input:        Symbol to be decoded, last data flag.
 *
 *  Returns:    Decoded Symbol via the structure DCTcoeff.
 *
 *  Side Effects:   Updates last flag.
 *
 *  Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
 *
 *********************************************************************/
#if 0
RunCoef vlc_word_decode (int symbol_word, int *last)
{
    int sign_index;
    RunCoef DCTcoef;

    *last = (symbol_word >> 12) & 01;

    DCTcoef.run = (symbol_word >> 4) & 255;

    DCTcoef.val = (symbol_word) & 15;

    sign_index = decode_a_symbol (cumf_SIGN);

    DCTcoef.sign = signtab[sign_index];

    return (DCTcoef);
}
#endif

/*********************************************************************
 *
 *        Name:        Decode_Escape_Char
 *
 *  Description:    Decodes all components for a Symbol when an
 *        ESCAPE character has been detected.
 *
 *  Input:        Picture Type and last data flag.
 *
 *  Returns:    Decoded Symbol via the structure DCTcoeff.
 *
 *  Side Effects:   Modifies last data flag.
 *
 *  Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
 *
 *********************************************************************/
#if 0
RunCoef Decode_Escape_Char (int intra, int *last)
{
    int last_index, run, run_index, level, level_index;
    RunCoef DCTcoef;

    if (intra)
    {
        last_index = decode_a_symbol (cumf_LAST_intra);
        *last = last_intratab[last_index];
    }
    else
    {
        last_index = decode_a_symbol (cumf_LAST);
        *last = lasttab[last_index];
    }

    if (intra)
        run_index = decode_a_symbol (cumf_RUN_intra);
    else
        run_index = decode_a_symbol (cumf_RUN);

    run = runtab[run_index];

    /* $if (mrun) run|=64;$ */

    DCTcoef.run = run;

    if (intra)
        level_index = decode_a_symbol (cumf_LEVEL_intra);
    else
        level_index = decode_a_symbol (cumf_LEVEL);


    level = leveltab[level_index];

    if (level > 128)
        level -= 256;

    if (level < 0)
    {
        DCTcoef.sign = 1;
        DCTcoef.val = ABS (level);
    }
    else
    {
        DCTcoef.sign = 0;
        DCTcoef.val = level;
    }

    return (DCTcoef);

}
#endif

/*********************************************************************
 *
 *        Name:        DecodeTCoef
 *
 *  Description:    Decodes a.c DCT Coefficients using the
 *        relevant arithmetic decoding model.
 *
 *  Input:        DCT Coeff count and Picture Type.
 *
 *  Returns:    Index to LUT
 *
 *  Side Effects:   None
 *
 *  Author:        Wayne Ellis <ellis_w_wayne@bt-web.bt.co.uk>
 *
 *********************************************************************/
#if 0
int DecodeTCoef (int position, int intra)
{
    int index;

    switch (position)
    {
        case 1:
        {
            if (intra)
                index = decode_a_symbol (cumf_TCOEF1_intra);
            else
                index = decode_a_symbol (cumf_TCOEF1);
            break;
        }
        case 2:
        {
            if (intra)
                index = decode_a_symbol (cumf_TCOEF2_intra);
            else
                index = decode_a_symbol (cumf_TCOEF2);
            break;
        }
        case 3:
        {
            if (intra)
                index = decode_a_symbol (cumf_TCOEF3_intra);
            else
                index = decode_a_symbol (cumf_TCOEF3);
            break;
        }
        default:
        {
            if (intra)
                index = decode_a_symbol (cumf_TCOEFr_intra);
            else
                index = decode_a_symbol (cumf_TCOEFr);
            break;
        }
    }

    return (index);
}
#endif
#endif

#endif// MEDIA_VOCVID_SUPPORT
