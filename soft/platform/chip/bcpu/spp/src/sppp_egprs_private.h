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



#ifndef __SPP_EGPRS_PRIVATE_H__
#define __SPP_EGPRS_PRIVATE_H__


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "global_macros.h"
#include "spal_xcor.h"
#include "spal_cap.h"
#include "spp_gsm.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


// Bit Operations
#define MOVEBITESPP(a, i, b, j) b[(j)/8]|=((!!(a[(i)/8]&1<<((i)%8)))<<((j)%8))
#define CLEARBITESPP(b, j)      b[(j)/8]&=(~(1<<((j)%8)))
#define MOVEBIT2(a, i, b, k, j) b[(k)+((j)/8)]|=((!!(a[(i)/8]&1<<((i)%8)))<<((j)%8))
#define CLEARBIT2(b, i, j)      b[(i)+((j)/8)]&=(~(1<<((j)%8)))
#define RDBIT32(b,j)          ((b[(j)/32] >>  ((j)%32)) & 1)
#define CLRBIT32(b,j)           b[(j)/32] &= (~(1<<((j)%32)))
#define SETBIT32(b,j)           b[(j)/32] |= ( (1<<((j)%32)))
#define RD4BITS8(b,j)        (((b)[(j)/2] >> (4*((j)%2))) & 0xf)
#define RD2BITS8(b,j)        (((b)[(j)/4] >> (2*((j)%4))) & 0x3)
#define CLR2BITS8(b,j)         (b)[(j)/4] &= (~((0x3)<<(2*((j)%4))))
#define CLR4BITS8(b,j)         (b)[(j)/2] &= (~((0xf<<(4*((j)%2)))))
#define SET2BITS8(b,j,val)     (b)[(j)/4] &= (~((0x3)<<(2*((j)%4)))); \
                               (b)[(j)/4] |= ( ((val)<<(2*((j)%4))))
#define SET4BITS8(b,j,val)     (b)[(j)/2] &= (~((0xf)<<(4*((j)%2)))); \
                                   (b)[(j)/2] |= ( ((val)<<(4*((j)%2))))


#define SWAP_DE(pIn, a, b)    \
{ \
    UINT8 swap = pIn[a]; \
    pIn[a] = pIn[b];     \
    pIn[b] = swap;       \
}

#define SWAP_EN(pIn, a, b, ta, tb)          \
                                            \
    ta = (!!(pIn[a / 8] & 1 << (a % 8)));   \
    tb = (!!(pIn[b / 8] & 1 << (b % 8)));   \
                                            \
    if (ta^tb)                              \
    {                                       \
        if (ta)                             \
        {                                   \
            pIn[a / 8] &= ~(1 << (a % 8));  \
            pIn[b / 8] |=  (1 << (b % 8));  \
        }                                   \
        else                                \
        {                                   \
            pIn[a / 8] |=  (1 << (a % 8));  \
            pIn[b / 8] &= ~(1 << (b % 8));  \
        }                                   \
    }


// Math
#define MIN_METRIC(_in, _num, _out) { UINT8 idx; _out = 0;\
    for (idx = 0; idx < _num; idx++) \
    _out = _in[idx] < _in[_out] ? idx : _out;}

#define MAX_METRIC(_in, _num, _out) { UINT8 idx; _out = 0;\
                for (idx = 0; idx < _num; idx++) \
                _out = _in[idx] > _in[_out] ? idx : _out;}





// Prefiler defines
// ----------------
// the LPQP is the scale for LP prefilter.
// the LPSFL is the sub of LPQP and
// the LPCH  is the scale of the channel taps
#define LPQP             11
#define LPSFL             1
#define LPCH             11




// Header element

// Macro used to change CPS format
// if g_sppHeaderDecodeMcs56(1234, 789)_copy use 1,2,3 to stand for P1,2,3.
// and do not care PADDING, i.e. P1 and PADDING P1 are the same
// #define CPS_FORMAT_CHANGE(x)    (x-1)
// #define CPS_FORMAT_REVERT(x)    (x+1)
// if g_sppHeaderDecodeMcs56(1234, 789)_copy use 0,1,2 to stand for P1,2,3.
#define CPS_FORMAT_CHANGE(x)    (x)
#define CPS_FORMAT_REVERT(x)    (x)

#define IR_MOD_SNS(_val)  ((_val) & 2047)
#define ROUND_DISTANCE(_V1,_V2, _Step)     (((INT32)(_V1-_V2)>=0)?  (_V1- _V2): (_V1-_V2+_Step))
#define IR_GET_BSN1(data)  ( ((data[3]&0x1)<<10)|(data[2]<<2)|((data[1]>>6)&0x3))
#define IR_GET_BSN2(data)  (((data[4]&0x7)<<7) |((data[3]>>1)&0x7f))
#define IR_GET_TFI(data)    (((data[1]&0xF)<<1) | ((data[0]>>7)&0x1))



// Padding prefix
#define MCS6_PADDING_PREFIX         (0x2)
#define MCS6_CLR_PADDING_PREFIX(x)  (x&(MCS6_PADDING_PREFIX-1))
#define MCS3_PADDING_PREFIX         (0x4)
#define MCS3_CLR_PADDING_PREFIX(x)  (x&(MCS3_PADDING_PREFIX-1))


// Type
typedef struct
{
    UINT8 cnt;
    UINT8 num;
    UINT8 state;
} SPP_CHANNEL_8TAP_STAT_PARAM_T;


typedef struct
{
    UINT8 mcsStatiscs[9];
    UINT8 cnt;
} SPP_EGPRS_CSHIST_T;



// External declaration of tables used by SPP
EXPORT CONST UINT16        g_sppEgprsHInSPos[1];
EXPORT CONST UINT16        g_sppEgprsDInSPos[9];
EXPORT CONST UINT16        g_sppEgprsHTailLen[9];
EXPORT CONST UINT16        g_sppEgprsDTailLen[9];
EXPORT CONST UINT16        g_sppEgprsDInLng[9];
EXPORT CONST UINT16        g_sppEgprsDHalfLth[3];
EXPORT CONST UINT8         g_sppEgprsUsf36[8][5];
EXPORT CONST INT16* CONST  g_sppUsfTables[2];
EXPORT CONST SPP_COMPLEX_T g_sppEgprsInvTrain[8][11][16];
EXPORT CONST UINT8         g_sppHeaderDecodeMcs789[29][3];
EXPORT CONST UINT8         g_sppHeaderDecodeMcs56[6][2];
EXPORT CONST UINT8         g_sppHeaderDecodeMcs1234[13][2];
EXPORT CONST SPAL_CAP_CRC_PARAM_T g_sppCsCrcParam_ULHead[12];
EXPORT CONST SPAL_CAP_CRC_PARAM_T g_sppCsCrcParam_Egprs[12];
EXPORT CONST SPAL_CAP_CONV_PARAM_T g_sppCsConvParam_Edge_DL[12];
EXPORT CONST SPAL_CAP_CONV_PARAM_T g_sppCsConvParam_Edge_UL[12];
#if (CHIP_VAMOSI_SUPPORTED)
EXPORT CONST INT16 g_sppChestTabEgprs[32][8][19];
#else
EXPORT CONST INT16 g_sppChestTabEgprs[8][8][19];
#endif
EXPORT CONST UINT8 g_sppHeaderDecodeMcs789_copy[29][3];
EXPORT CONST UINT8 g_sppHeaderDecodeMcs56_copy[6][2];
EXPORT CONST UINT8 g_sppHeaderDecodeMcs1234_copy[13][2];
EXPORT CONST UINT32* CONST g_sppCsPuncturingTable_Edge[12];
EXPORT CONST UINT8 g_sppTable8To4Compress_LUT[32];
EXPORT CONST UINT32 ENC_PUNCTURE_HEAD_1[2*6];
EXPORT CONST UINT32 ENC_PUNCTURE_HEAD_2[2*6];
EXPORT CONST UINT32 ENC_PUNCTURE_HEAD_3[2*6];
EXPORT CONST UINT32 g_sppDcTabEgprs[8][9][19];


EXPORT CONST UINT32 g_sppSovaCmd[339];

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


PROTECTED UINT8 spp_Psk8DcIqEstimation(INT16*                   inOut,
                                       UINT8                    tsc,
                                       UINT8                    enableDc,
                                       UINT8                    enableIq,
                                       SPP_COMPLEX_T*           DcIqBufs,
                                       SPP_COMPLEX_T*           dc_est,
                                       SPP_COMPLEX_T*           iq_est);


PROTECTED VOID spp_EgprsFofEstimation (  UINT32* InSamples,
        UINT8* InSoftBits,
        UINT8 Tsc,
        SPP_COMPLEX_T* ChTaps,
        UINT32 NoisePwr,
        UINT32 Power,
        INT16* Fof);


PROTECTED UINT8 spp_PArraySat(SPP_EGPRS_PREFILTER_BUFS_T* PrefilterBufs);



PROTECTED UINT8 spp_PrefltCheckValid(SPP_EGPRS_PREFILTER_BUFS_T *PrefilterBufs,
                                     INT8  LogChanPwr,
                                     UINT8 LPQP_Shift);


PROTECTED VOID spp_LpNoiseCorr(SPP_EGPRS_PREFILTER_BUFS_T* PrefilterBufs, UINT8* plogNoisePwr);



PROTECTED VOID spp_LpChannelEstimation(INT16 *RLS_W,
                                       INT16 *P_Array,
                                       INT16 *H_CorArray,
                                       INT16 *A1,
                                       INT8 LogChanPwr);

PROTECTED UINT8 spp_LsePrefilter(UINT8                        tsc,
                                 SPP_EGPRS_PREFILTER_BUFS_T* PrefilterBufs,
                                 UINT32*                     NoisePower,
                                 UINT8*                      plogNoisePwr);

PROTECTED VOID spp_EqualizeDdfseBck(INT32*                  lpOut,
                                    SPP_EGPRS_DDFSE_BUFS_T* DdfseBufs,
                                    UINT8                   logNoisePwr,
                                    PUINT8                  OutSoft);

PROTECTED VOID spp_EqualizeDdfseFwd(INT32*                  lpOut,
                                    SPP_EGPRS_DDFSE_BUFS_T* DdfseBufs,
                                    UINT8                   logNoisePwr,
                                    PUINT8                  OutSoft);

PROTECTED VOID spp_EgprsSovaScale(UINT8 logNoisePwr, UINT8* pScale);


PROTECTED VOID spp_EqualizeSovaBck(INT32* lpOut,
                                   SPP_EGPRS_DDFSE_BUFS_T* DdfseBufs,
                                   UINT8 logNoisePwr,
                                   PUINT8  OutSoft);
PROTECTED VOID spp_EqualizeSovaFwd(INT32* lpOut,
                                   SPP_EGPRS_DDFSE_BUFS_T* DdfseBufs,
                                   UINT8 logNoisePwr,
                                   PUINT8  OutSoft);


//=============================================================================
// spp_EgprsItlv
//-----------------------------------------------------------------------------
// Function performing the interleaving for Egprs Uplink.
//
// @param pD             UINT8*   INPUT    Pointer to Data.
// @param pD1            UINT8*   INPUT    Pointer to Data1.
// @param pHd            UINT8*   INPUT    Pointer to Header.
// @param pUsfm          UINT8*   INPUT    Pointer to Usfm.
// @param ItLvDataOut    UINT8*   OUTPUT   Pointer to the Data after Interleaving.
// @param ItLvHeadOut    UINT8*   OUTPUT   Pointer to the Header after Interleaving.
// @param ChMod          UINT8    OUTPUT   Coding Scheme.
//
// @author Nadia Touliou
// @date 2008/02/01
//=============================================================================
PUBLIC VOID spp_EgprsItlv(UINT8* pD,
                          UINT8* pD1,
                          UINT8* pHd,
                          UINT8* ItLvDataOut,
                          UINT8* ItLvHeadOut,
                          UINT8  ChMod);



//=============================================================================
// spp_EgprsConvoluate
//-----------------------------------------------------------------------------
// conv-encoding for egprs
//
// @param cs SPP_CODING_SCHEME_T INPUT. coding scheme
// @param isDownlink BOOL. INPUT. downlink:1. uplink:0
// @param punctTable UINT32*. INPUT. Pointer to the punc table. UINT32* type
// @param In UINT32*. INPUT Pointer to the input buffer. UINT32* type
// @param Out UINT32*. OUTPUT. Pointer to the output buffer. UINT32* type
//
//=============================================================================

PROTECTED VOID spp_EgprsConvoluate(SPP_CODING_SCHEME_T cs,
                                   BOOL isDownlink,
                                   UINT32* punctTable,
                                   UINT32* In,
                                   UINT32* Out);



//=============================================================================
// spp_EgprsEncodeCrc
//-----------------------------------------------------------------------------
// encode CRC
//
// @param cs SPP_CODING_SCHEME_T. Coding scheme.
// @param In UINT8*. Pointer to the input buffer. UINT8 type used
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PROTECTED VOID spp_EgprsEncodeCrc(SPP_CODING_SCHEME_T cs, UINT32* In, UINT32* Out);


PROTECTED UINT8 spp_8tapDetect(SPP_COMPLEX_T* RLS_W, UINT32*TapPwr);


//=============================================================================
// spp_Max()
//-----------------------------------------------------------------------------
/// This function is used to search max abs value
//=============================================================================

PROTECTED UINT16 spp_Max(INT16* in, UINT16 n);

// ============================================================================
// SPP_UNCACHED_GAUSSS_T
// -----------------------------------------------------------------------------
/// buffer for gauss ce update
// =============================================================================
typedef struct
{
    UINT32                                hard_bit_back[2];             //0x00000000
    UINT32                                hard_bit_forward[3];          //0x00000008
    SPAL_COMPLEX_T                x_matrix[8];                  //0x00000014
    SPAL_COMPLEX_T                sym_value[71];                //0x00000034
    SPAL_COMPLEX_T                sym_value_conj[71];           //0x00000150
    SPAL_COMPLEX_T                q;                            //0x0000026C
    SPAL_COMPLEX_T                temp[8];                      //0x00000270
    SPAL_COMPLEX_T                k_matrix[8][8];                 //0x00000290
} SPP_UNCACHED_GAUSS_T; //Size : 0x390




#endif

