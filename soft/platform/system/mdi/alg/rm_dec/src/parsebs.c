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


































#include "hxtypes.h"
#if defined(_WIN32)
/* <windows.h> is needed for registry and ini file access */
//#include "hxtypes.h"
#include <windows.h>
#endif /* defined(_WIN32) */

#include "stdio.h"
#include "beginhdr.h"
#include "rvdecoder.h"
#include "tables.h"
#include "dec4x4m.h"

#ifdef SHEEN_VC_DEBUG

//added by huangx
#include "vpp_rmvb.h"
#include "vppp_rmvb_asm_map.h"
#include "voc2_define.h"
#include "hal_voc.h"
#include "vppp_rmvb_asm_common.h"

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFFFF)//(0xFFFFFF<<2)

#else

#include "mcip_debug.h"
#include "vpp_rm_dec_asm.h"
#include "hal_voc.h"
#include "mmc.h"
#include "sxr_tls.h"
#include "vpp_audiojpeg_dec.h"
#include "mdi_alg_common.h"

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFF<<2)


#endif

//VOLATILE VPP_RMVB_CFG_T *vppCfgIn;

VPP_RMVB_CFG_T  vpp_rmvb_cfg;
INT32  vpp_contex_buffer[CONTEX_BUF_SIZE];//voc contex memory.

VPP_RMVB_DATA_T *vppDataIn=NULL;
//volatile VocWorkStruct *pRmVocWorkState=NULL;
extern VocWorkStruct vocWorkState;
vpp_data_struct vpp_data_buffer[2];// pingpang for vld
volatile INT16  vpp_buffer_rd_id;
volatile INT16  vpp_buffer_wt_id;


const static UINT32 local_length_table[32]=
{
    0,0,1,1,2,2,2,2,
    3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4
};

const static UINT32 local_length_table2[32]=
{
    5,6,7,7,8,8,8,8,
    9,9,9,9,9,9,9,9,
    10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10
};

const static UINT32 local_length_table3[16]=
{
    11,12,13,13,14,14,14,14,
    15,15,15,15,15,15,15,15
};

#ifndef SHEEN_VC_DEBUG
VOLATILE U32 g_rm_step_cnt __attribute__((section (".ucdata"))) = 0 ;
VOLATILE U32 g_rm_mb_cnt __attribute__((section (".ucdata"))) = 0 ;
#endif

/* Dequantize q based on qp (should match encoder's quantization). */
/* See C_QuantAndDequant4x4 in the encoder. */
#define DEQUANT(q, qp) ( ((q) * BquantTable[(qp)] + 8) >> 4 )

/*((qval * Btrue[qp] )>>10); */

/* Prepare registers if using optimized inverse transform */
#if !defined(APK) && defined(CPK) || defined(ARM) || defined(_ARM_)
#define PRE_INVERSE_TRANSFORM(impl, x)
#else
#define PRE_INVERSE_TRANSFORM(impl, x)  \
    if (impl >= MMX_IMPL)               \
        pre_inverse_transform(x);
#endif

#define NUM_AI_MODES 9
#define LOOP_FILTER_EN

/*/////////////////////////////////////////////////////////////// */
/* Convert Length and Info to VLC code number */
#define LENGTH_INFO_TO_N(length, info) \
    ((1 << ((length)>>1)) + (info) - 1)


/*////////////////////////////////////////////////////////////////// */
/* MV_Sub2FullPels */
/* GLD 6/16/99 */
void
MV_Sub2FullPels(const I32 mv, I32* mv_intg, I32* mv_frac)
{
    *mv_intg = mv / INTERP_FACTOR;
    *mv_frac = mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }
}

void
MV_Sub2FullPelsChromaYi(const I32 mv, I32* mv_intg, I32* mv_frac)
{

    static I32 twidle_chroma_mv[15] = {-7, -4, -3, -6, -5, -2, -1, 0, 1, 4, 5, 2, 3, 6, 7};
    // xform
    I32 int_lookup = mv / (INTERP_FACTOR*2);
    I8 frac_lookup = mv - int_lookup*(INTERP_FACTOR*2);
    I32 new_mv = twidle_chroma_mv[frac_lookup+7] + int_lookup*(INTERP_FACTOR*2);

    *mv_intg = new_mv / INTERP_FACTOR;
    *mv_frac = new_mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }
}

/* B frames */
void
MV_Sub2FullPels_B(const I32 mv, I32* mv_intg, I32* mv_frac)
{
    *mv_intg = mv/INTERP_FACTOR;

#ifdef BFRAME_ALWAYS_NO_SUBPEL
    *mv_frac = 0;
#elif  defined BFRAME_ALWAYS_HALF_PEL
    *mv_frac = mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }

    if (*mv_frac == 1 || *mv_frac == 3)
        *mv_frac = 2;
#else
    *mv_frac = mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }
#endif
}

/* chroma B frames */
void
MV_Sub2FullPels_BC(const I32 mv, I32* mv_intg, I32* mv_frac)
{
    *mv_intg = mv/INTERP_FACTOR;

#if defined (BFRAME_ALWAYS_NO_SUBPEL) || defined (BFRAME_ALWAYS_NO_SUBPEL_CHROMA)
    *mv_frac = 0;
#elif defined (BFRAME_ALWAYS_HALF_PEL) || defined (BFRAME_ALWAYS_HALF_PEL_CHROMA)
    *mv_frac = mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }

    if (*mv_frac == 1 || *mv_frac == 3)
        *mv_frac = 2;
#else
    *mv_frac = mv - *mv_intg * INTERP_FACTOR;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR;
    }
#endif
}



void
MV_Sub2FullPels_RV8(const I32 mv, I32* mv_intg, I32* mv_frac)
{
    *mv_intg = mv / INTERP_FACTOR_RV8;
    *mv_frac = mv - *mv_intg * INTERP_FACTOR_RV8;

    if (*mv_frac < 0)
    {
        *mv_intg -= 1;
        *mv_frac += INTERP_FACTOR_RV8;
    }
}


#ifdef INTERLACED_CODE
const U32 uBlockPairAdjI[8] = {0, 1, 2, 3, 0, 1, 6, 7};
/*                              0, 1, X, X, 0, 1, X, X */
const U32 uModeLeftI[8] = {0, 0, 3, 0, 0, 0, 3, 0};
#endif

#if 0//hx chg
RV_Status
Decoder_decodeIntraTypes
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMB,
    const DecoderIntraType pAboveIntraTypes[4],
    DecoderIntraType *pMBIntraTypes,
    struct CRealVideoBs *pBitstream
#ifdef INTERLACED_CODE
    , Bool32 bIsInterlaced
#endif
)
{
    U32         ulength, info;
    U32         block;
    I32         iLeftMode, iAboveLeftMode, iAboveRightMode;
    I32         i, A, B;
    I32         table_number, table_index;
    const U8    *u8TabPtr;
    const U16   *u16TabPtr;

    DecoderIntraType   *pDstType = pMBIntraTypes;
    const Bool32        isTopMB  = pMB->edge_type & D_TOP_EDGE;
    const Bool32        isLeftMB = pMB->edge_type & D_LEFT_EDGE;

    if (pMB->mbtype == MBTYPE_INTRA_16x16)
    {
        info = CB_GetBits(pBitstream,2);

        if (info > 3) /* we only have 4 modes */
        {
            return RV_S_OUT_OF_RANGE;
        }
        *pDstType = (U8) info;

        for (i = 1; i < 16; i ++)
            pDstType[i] = *pDstType;
    }
    else
    {

        for (block = 0; block < 16; block += 4, pDstType += 4)
        {
            if (block == 0 && isTopMB)
            {
                /* decode 4 modes */
                u8TabPtr = aic_top_vlc;
                for (A = 0; A < 16; A++, u8TabPtr++)
                {
                    info = *u8TabPtr;
                    GET_VLC_LENGTH_INFO_CHAR(info, ulength);
                    if (CB_SearchBits(pBitstream,ulength, info, 0))
                        break;
                }
                pDstType[0] = (DecoderIntraType)((A >> 2) & 2);
                pDstType[1] = (DecoderIntraType)((A >> 1) & 2);
                pDstType[2] = (DecoderIntraType)((A) & 2);
                pDstType[3] = (DecoderIntraType)((A << 1) & 2);
                RVDebug((ptl_mb,"Mode %d Mode %d Mode %d Mode %d",
                         (U32)pDstType[0], (U32)pDstType[1], (U32)pDstType[2], (U32)pDstType[3]));
            }
            else
            {
                iLeftMode       = (isLeftMB ? -1 : pDstType[3]);
                iAboveLeftMode  = block ? pDstType[-4] : (isTopMB ? -1 : pAboveIntraTypes[0]);
                iAboveRightMode = block ? pDstType[-3] : (isTopMB ? -1 : pAboveIntraTypes[1]);

                table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                for (table_index = 0; table_index < 20; table_index++)
                    if (aic_table_index[table_index] == table_number)
                        break;

                if (table_index < 20)
                {
                    /* decode 2 modes */
                    u16TabPtr = &(aic_2mode_vlc[table_index][0]);
                    for (table_number = 0, A = 0, B = 0;
                            table_number < 81;
                            table_number++, u16TabPtr++)
                    {
                        info = *u16TabPtr;
                        GET_VLC_LENGTH_INFO_SHORT(info,ulength);
                        if (CB_SearchBits(pBitstream,ulength, info, 0))
                            break;
                        B++;
                        if (B == 9)
                        {
                            B = 0;
                            A++;
                        }
                    }
                    pDstType[0] = (DecoderIntraType)A;
                    pDstType[1] = (DecoderIntraType)B;
                    RVDebug((ptl_b,"2 Modes A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));
                }
                else
                {
                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);
                    for (A = 0; A < 9; A++, u8TabPtr++)
                    {
                        info = *u8TabPtr;

                        GET_VLC_LENGTH_INFO_CHAR(info,ulength);
                        if (ulength && CB_SearchBits(pBitstream,ulength, info, 0))
                            break;
                    }
                    pDstType[0] = (DecoderIntraType)A;
                    RVDebug((ptl_b,"1 Mode A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));

                    iLeftMode = A;
                    iAboveLeftMode = iAboveRightMode;
                    iAboveRightMode = block ? pDstType[-2] : (isTopMB ? -1 : pAboveIntraTypes[2]);

                    table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                    for (table_index = 0; table_index < 20; table_index++)
                        if (aic_table_index[table_index] == table_number)
                            break;

                    if (table_index < 20)
                    {
                        /* decode 2 modes */
                        u16TabPtr = &(aic_2mode_vlc[table_index][0]);
                        for (table_number = 0, A = 0, B = 0;
                                table_number < 81;
                                table_number++, u16TabPtr++)
                        {
                            info = *u16TabPtr;
                            GET_VLC_LENGTH_INFO_SHORT(info, ulength);
                            if (CB_SearchBits(pBitstream,ulength, info, 0))
                                break;
                            B++;
                            if (B == 9)
                            {
                                B = 0;
                                A++;
                            }
                        }
                        pDstType[1] = (DecoderIntraType)A;
                        pDstType[2] = (DecoderIntraType)B;
                        RVDebug((ptl_b,"2 Modes A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));

                        iLeftMode = B;
                        iAboveLeftMode = block ? pDstType[-1] : (isTopMB ? -1 : pAboveIntraTypes[3]);

                        /* decode 1 mode */
                        u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);
                        for (A = 0; A < 9; A++, u8TabPtr++)
                        {
                            info = *u8TabPtr;
                            GET_VLC_LENGTH_INFO_CHAR(info,ulength);
                            if (CB_SearchBits(pBitstream,ulength, info, 0))
                                break;
                        }
                        pDstType[3] = (DecoderIntraType)A;
                        RVDebug((ptl_b,"1 Mode A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));
                        RVDebug((ptl_mb,"Mode %d Mode %d Mode %d Mode %d",
                                 (U32)pDstType[0], (U32)pDstType[1], (U32)pDstType[2], (U32)pDstType[3]));
                        continue;
                    }
                    else
                    {
                        /* decode 1 mode */
                        u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);
                        for (A = 0; A < 9; A++, u8TabPtr++)
                        {
                            info = *u8TabPtr;
                            GET_VLC_LENGTH_INFO_CHAR(info,ulength);
                            if (CB_SearchBits(pBitstream,ulength, info, 0))
                                break;
                        }
                        pDstType[1] = (DecoderIntraType)A;
                        RVDebug((ptl_b,"1 Mode A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));
                    }
                }

                iLeftMode       = pDstType[1];
                iAboveLeftMode  = block ? pDstType[-2] : (isTopMB ? -1 : pAboveIntraTypes[2]);
                iAboveRightMode = block ? pDstType[-1] : (isTopMB ? -1 : pAboveIntraTypes[3]);

                table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                for (table_index = 0; table_index < 20; table_index++)
                    if (aic_table_index[table_index] == table_number)
                        break;

                if (table_index < 20)
                {
                    /* decode 2 modes */
                    u16TabPtr = &(aic_2mode_vlc[table_index][0]);
                    for (table_number = 0, A = 0, B = 0;
                            table_number < 81;
                            table_number++, u16TabPtr++)
                    {
                        info = *u16TabPtr;
                        GET_VLC_LENGTH_INFO_SHORT(info, ulength);
                        if (CB_SearchBits(pBitstream,ulength, info, 0))
                            break;
                        B++;
                        if (B == 9)
                        {
                            B = 0;
                            A++;
                        }
                    }
                    pDstType[2] = (DecoderIntraType)A;
                    pDstType[3] = (DecoderIntraType)B;
                    RVDebug((ptl_b,"2 Modes A %d B %d C %d info %d", iLeftMode, iAboveLeftMode, iAboveRightMode, info));
                }
                else
                {
                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);
                    for (A = 0; A < 9; A++, u8TabPtr++)
                    {
                        info = *u8TabPtr;
                        GET_VLC_LENGTH_INFO_CHAR(info,ulength);
                        if (CB_SearchBits(pBitstream,ulength, info, 0))
                            break;
                    }
                    pDstType[2] = (DecoderIntraType)A;

                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*A + iAboveRightMode + 11][0]);
                    for (B = 0; B < 9; B++, u8TabPtr++)
                    {
                        info = *u8TabPtr;
                        GET_VLC_LENGTH_INFO_CHAR(info,ulength);
                        if (CB_SearchBits(pBitstream,ulength, info, 0))
                            break;
                    }
                    pDstType[3] = (DecoderIntraType)B;
                }
            }
            RVDebug((ptl_mb,"Mode %d Mode %d Mode %d Mode %d",
                     (U32)pDstType[0], (U32)pDstType[1], (U32)pDstType[2], (U32)pDstType[3]));
        }

    }/*else intra16 */


    return RV_S_OK;
}
#endif


RV_Status
Decoder_decodeIntraTypes
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMB,
    const DecoderIntraType pAboveIntraTypes[4],
    DecoderIntraType *pMBIntraTypes,
    struct CRealVideoBs *pBitstream
#ifdef INTERLACED_CODE
    , Bool32 bIsInterlaced
#endif
)
{
    U32         ulength, info;
    U32         block,Dtmp32;
    I32         iLeftMode, iAboveLeftMode, iAboveRightMode;
    I32         i, A, B;
    I32         table_number, table_index;
    const U8        *u8TabPtr;
    const U32   *u32TabPtr;

    DecoderIntraType   *pDstType = pMBIntraTypes;
    const Bool32        isTopMB  = pMB->edge_type & D_TOP_EDGE;
    const Bool32        isLeftMB = pMB->edge_type & D_LEFT_EDGE;

    if (pMB->mbtype == MBTYPE_INTRA_16x16)
    {
        info = CB_GetBits(pBitstream,2);
        *pDstType = (U8) info;

        for (i = 1; i < 16; i +=4)
        {
            pDstType[i]      = *pDstType;
            pDstType[i+1] = *pDstType;
            pDstType[i+2] = *pDstType;
            pDstType[i+3] = *pDstType;
        }
    }
    else
    {

        for (block = 0; block < 16; block += 4, pDstType += 4)
        {
            if (block == 0 && isTopMB)
            {
                /* decode 4 modes */
                u8TabPtr = aic_top_vlc;

                Dtmp32 = (U32) *(pBitstream->m_pbs);
                Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                for (A = 0; A < 16; A++, u8TabPtr++)
                {
                    info = *u8TabPtr;

                    if(info<32)
                        ulength = local_length_table[info];
                    else
                        ulength = local_length_table2[info>>6];     //MAX 10

                    info <<= (32 - ulength);
                    info >>= (32 - ulength);

                    if (info ==(Dtmp32 >> (32- ulength)))
                    {
                        for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                            pBitstream->m_pbs ++;
                        pBitstream->m_bitOffset = Dtmp32;
                        break;
                    }
                }

                pDstType[0] = (DecoderIntraType)((A >> 2) & 2);
                pDstType[1] = (DecoderIntraType)((A >> 1) & 2);
                pDstType[2] = (DecoderIntraType)((A) & 2);
                pDstType[3] = (DecoderIntraType)((A << 1) & 2);
            }
            else
            {
                iLeftMode       = (isLeftMB ? -1 : pDstType[3]);
                iAboveLeftMode  = block ? pDstType[-4] : (isTopMB ? -1 : pAboveIntraTypes[0]);
                iAboveRightMode = block ? pDstType[-3] : (isTopMB ? -1 : pAboveIntraTypes[1]);

                table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                for (table_index = 0; table_index < 20; table_index++)
                    if (aic_table_index[table_index] == table_number)
                        break;

                if (table_index < 20)
                {
                    /* decode 2 modes */
                    u32TabPtr = &(aic_2mode_vlc[table_index][0]);

                    Dtmp32 = (U32) *(pBitstream->m_pbs);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                    Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);   //left align
                    Dtmp32 = Dtmp32>>16;                            //move high 16 bit to low 16 bit

                    if(Dtmp32< (u32TabPtr[40] & 0xffff))
                    {
                        if(Dtmp32< (u32TabPtr[20] & 0xffff))
                        {
                            if(Dtmp32< (u32TabPtr[10] & 0xffff))
                            {
                                table_number = 10;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 20;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                        else
                        {
                            if(Dtmp32< (u32TabPtr[30] & 0xffff))
                            {
                                table_number = 30;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 40;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }

                    }
                    else    //Dtmp32 >= (u32TabPtr[40] & 0xffff)
                    {
                        if(Dtmp32< (u32TabPtr[60] & 0xffff))
                        {
                            if(Dtmp32< (u32TabPtr[50] & 0xffff))
                            {
                                table_number = 50;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 60;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                        else
                        {
                            if(Dtmp32< (u32TabPtr[70] & 0xffff))
                            {
                                table_number = 70;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 80;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                    }

                    A = (u32TabPtr[table_number] >>28) & 0xf;
                    B = (u32TabPtr[table_number] >>24) & 0xf;
                    ulength = (u32TabPtr[table_number]>>16) & 0xff;

                    for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                        pBitstream->m_pbs ++;
                    pBitstream->m_bitOffset = Dtmp32;

                    pDstType[0] = (DecoderIntraType)A;
                    pDstType[1] = (DecoderIntraType)B;
                }
                else
                {
                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);

                    Dtmp32 = (U32) *(pBitstream->m_pbs);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                    Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                    for (A = 0; A < 9; A++, u8TabPtr++)
                    {
                        info = *u8TabPtr;

                        if(info<32)
                            ulength = local_length_table[info];
                        else
                            ulength = local_length_table2[info>>6];     //MAX 10

                        info <<= (32 - ulength);
                        info >>= (32 - ulength);

                        if (ulength && (info ==(Dtmp32 >> (32- ulength))) )
                        {
                            for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                                pBitstream->m_pbs ++;
                            pBitstream->m_bitOffset = Dtmp32;
                            break;
                        }
                    }

                    pDstType[0] = (DecoderIntraType)A;

                    iLeftMode = A;
                    iAboveLeftMode = iAboveRightMode;
                    iAboveRightMode = block ? pDstType[-2] : (isTopMB ? -1 : pAboveIntraTypes[2]);

                    table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                    for (table_index = 0; table_index < 20; table_index++)
                        if (aic_table_index[table_index] == table_number)
                            break;

                    if (table_index < 20)
                    {
                        /* decode 2 modes */
                        u32TabPtr = &(aic_2mode_vlc[table_index][0]);

                        Dtmp32 = (U32) *(pBitstream->m_pbs);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                        Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);   //left align
                        Dtmp32 = Dtmp32>>16;                            //move high 16 bit to low 16 bit

                        if(Dtmp32< (u32TabPtr[40] & 0xffff))
                        {
                            if(Dtmp32< (u32TabPtr[20] & 0xffff))
                            {
                                if(Dtmp32< (u32TabPtr[10] & 0xffff))
                                {
                                    table_number = 10;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                                else
                                {
                                    table_number = 20;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                            }
                            else
                            {
                                if(Dtmp32< (u32TabPtr[30] & 0xffff))
                                {
                                    table_number = 30;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                                else
                                {
                                    table_number = 40;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                            }

                        }
                        else    //Dtmp32 >= (u32TabPtr[40] & 0xffff)
                        {
                            if(Dtmp32< (u32TabPtr[60] & 0xffff))
                            {
                                if(Dtmp32< (u32TabPtr[50] & 0xffff))
                                {
                                    table_number = 50;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                                else
                                {
                                    table_number = 60;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                            }
                            else
                            {
                                if(Dtmp32< (u32TabPtr[70] & 0xffff))
                                {
                                    table_number = 70;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                                else
                                {
                                    table_number = 80;
                                    while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                        table_number--;
                                }
                            }
                        }

                        A = (u32TabPtr[table_number] >>28) & 0xf;
                        B = (u32TabPtr[table_number] >>24) & 0xf;
                        ulength = (u32TabPtr[table_number]>>16) & 0xff;

                        for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                            pBitstream->m_pbs ++;
                        pBitstream->m_bitOffset = Dtmp32;

                        pDstType[1] = (DecoderIntraType)A;
                        pDstType[2] = (DecoderIntraType)B;

                        iLeftMode = B;
                        iAboveLeftMode = block ? pDstType[-1] : (isTopMB ? -1 : pAboveIntraTypes[3]);

                        /* decode 1 mode */
                        u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);

                        Dtmp32 = (U32) *(pBitstream->m_pbs);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                        Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                        for (A = 0; A < 9; A++, u8TabPtr++)
                        {
                            info = *u8TabPtr;

                            if(info<32)
                                ulength = local_length_table[info];
                            else
                                ulength = local_length_table2[info>>6];     //MAX 10

                            info <<= (32 - ulength);
                            info >>= (32 - ulength);

                            if (ulength && (info ==(Dtmp32 >> (32- ulength))) )
                            {
                                for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                                    pBitstream->m_pbs ++;
                                pBitstream->m_bitOffset = Dtmp32;
                                break;
                            }
                        }

                        pDstType[3] = (DecoderIntraType)A;
                        continue;
                    }
                    else
                    {
                        /* decode 1 mode */
                        u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);

                        Dtmp32 = (U32) *(pBitstream->m_pbs);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                        Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                        Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                        for (A = 0; A < 9; A++, u8TabPtr++)
                        {
                            info = *u8TabPtr;

                            if(info<32)
                                ulength = local_length_table[info];
                            else
                                ulength = local_length_table2[info>>6];     //MAX 10

                            info <<= (32 - ulength);
                            info >>= (32 - ulength);

                            if (info ==(Dtmp32 >> (32- ulength)))
                            {
                                for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                                    pBitstream->m_pbs ++;
                                pBitstream->m_bitOffset = Dtmp32;
                                break;
                            }
                        }

                        pDstType[1] = (DecoderIntraType)A;
                    }
                }

                iLeftMode       = pDstType[1];
                iAboveLeftMode  = block ? pDstType[-2] : (isTopMB ? -1 : pAboveIntraTypes[2]);
                iAboveRightMode = block ? pDstType[-1] : (isTopMB ? -1 : pAboveIntraTypes[3]);

                table_number = 100 * iLeftMode + 10 * iAboveLeftMode + iAboveRightMode;
                for (table_index = 0; table_index < 20; table_index++)
                    if (aic_table_index[table_index] == table_number)
                        break;

                if (table_index < 20)
                {
                    /* decode 2 modes */
                    u32TabPtr = &(aic_2mode_vlc[table_index][0]);

                    Dtmp32 = (U32) *(pBitstream->m_pbs);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                    Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);   //left align
                    Dtmp32 = Dtmp32>>16;                            //move high 16 bit to low 16 bit

                    if(Dtmp32< (u32TabPtr[40] & 0xffff))
                    {
                        if(Dtmp32< (u32TabPtr[20] & 0xffff))
                        {
                            if(Dtmp32< (u32TabPtr[10] & 0xffff))
                            {
                                table_number = 10;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 20;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                        else
                        {
                            if(Dtmp32< (u32TabPtr[30] & 0xffff))
                            {
                                table_number = 30;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 40;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }

                    }
                    else    //Dtmp32 >= (u32TabPtr[40] & 0xffff)
                    {
                        if(Dtmp32< (u32TabPtr[60] & 0xffff))
                        {
                            if(Dtmp32< (u32TabPtr[50] & 0xffff))
                            {
                                table_number = 50;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 60;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                        else
                        {
                            if(Dtmp32< (u32TabPtr[70] & 0xffff))
                            {
                                table_number = 70;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                            else
                            {
                                table_number = 80;
                                while(Dtmp32< (u32TabPtr[table_number] & 0xffff))
                                    table_number--;
                            }
                        }
                    }

                    A = (u32TabPtr[table_number] >>28) & 0xf;
                    B = (u32TabPtr[table_number] >>24) & 0xf;
                    ulength = (u32TabPtr[table_number]>>16) & 0xff;

                    for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                        pBitstream->m_pbs ++;
                    pBitstream->m_bitOffset = Dtmp32;

                    pDstType[2] = (DecoderIntraType)A;
                    pDstType[3] = (DecoderIntraType)B;
                }
                else
                {
                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*iLeftMode + iAboveLeftMode + 11][0]);

                    Dtmp32 = (U32) *(pBitstream->m_pbs);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                    Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                    for (A = 0; A < 9; A++, u8TabPtr++)
                    {
                        info = *u8TabPtr;

                        if(info<32)
                            ulength = local_length_table[info];
                        else
                            ulength = local_length_table2[info>>6];     //MAX 10

                        info <<= (32 - ulength);
                        info >>= (32 - ulength);

                        if (info ==(Dtmp32 >> (32- ulength)) )
                        {
                            for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                                pBitstream->m_pbs ++;
                            pBitstream->m_bitOffset = Dtmp32;
                            break;
                        }
                    }

                    pDstType[2] = (DecoderIntraType)A;

                    /* decode 1 mode */
                    u8TabPtr = &(aic_1mode_vlc[10*A + iAboveRightMode + 11][0]);

                    Dtmp32 = (U32) *(pBitstream->m_pbs);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                    Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+2);
                    Dtmp32 = Dtmp32<<(8+pBitstream->m_bitOffset);

                    for (B = 0; B < 9; B++, u8TabPtr++)
                    {
                        info = *u8TabPtr;

                        if(info<32)
                            ulength = local_length_table[info];
                        else
                            ulength = local_length_table2[info>>6];     //MAX 10

                        info <<= (32 - ulength);
                        info >>= (32 - ulength);

                        if (info ==(Dtmp32 >> (32- ulength)) )
                        {
                            for (Dtmp32 = pBitstream->m_bitOffset + ulength; Dtmp32 >= 8; Dtmp32 -= 8)
                                pBitstream->m_pbs ++;
                            pBitstream->m_bitOffset = Dtmp32;
                            break;
                        }
                    }

                    pDstType[3] = (DecoderIntraType)B;
                }
            }
        }

    }/*else intra16 */


    return RV_S_OK;
}



RV_Status
Decoder_decodeIntraTypes_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMB,
    const DecoderIntraType pAboveIntraTypes[4],
    DecoderIntraType *pMBIntraTypes,
    struct CRealVideoBs *pBitstream
)
{
    U32     length, info, blockpair, N, A, B, prob0, prob1;
    Bool32              isLeftEdgeSubBlock, isTopEdgeSubBlock;
    DecoderIntraType   *pDstType = pMBIntraTypes;
    const Bool32        isTopMB  = pMB->edge_type & D_TOP_EDGE;
    const Bool32        isLeftMB = pMB->edge_type & D_LEFT_EDGE;
    I32 i;

    if (pMB->mbtype == MBTYPE_INTRA_16x16)
    {
        N = CB_GetBits(pBitstream,2);

        if (N > 3) /* we only have 4 modes */
        {
            return RV_S_OUT_OF_RANGE;
        }

        *pDstType = (U8) N;

        for (i = 1; i < 16; i ++)
            pDstType[i] = *pDstType;

    }
    else

        /* Loop over 8 pairs of subblocks. One pair is */
        /* decoded in each iteration. */
        for (blockpair = 0; blockpair < 8; blockpair++)
        {
            length = CB_GetVLCBits(pBitstream,&info);

            N = LENGTH_INFO_TO_N(length,info);

            /* Find out if this block is at the left or top edge */
            /* of the frame, slice or macroblock. */

            isLeftEdgeSubBlock = left_edge_tab16[blockpair << 1];
            isTopEdgeSubBlock  = top_edge_tab16[blockpair << 1];

            B = isLeftEdgeSubBlock
                ? (isLeftMB ? 0 : (1 + pDstType[3]))
                : (1 + pDstType[-1]);

            A = isTopEdgeSubBlock
                ? (isTopMB ? 0 : (1 + pAboveIntraTypes[blockpair << 1]))
                : (1 + pDstType[-4]);

            prob0 = dec_iprob_rv8[N << 1];
            prob1 = dec_iprob_rv8[(N << 1) + 1];

            *pDstType = dec_aic_prob_rv8[A][B][prob0];
            if (*pDstType >= NUM_AI_MODES)
            {
                return RV_S_OUT_OF_RANGE;
            }

            /* Block two (no need to recalc edge conditions. We know this */
            /* block is not at the left edge, and if the previous block */
            /* was at the top edge, this will be too) */

            pDstType++;
            B = 1 + pDstType[-1];

            A = isTopEdgeSubBlock
                ? (isTopMB ? 0 : (1 + pAboveIntraTypes[(blockpair << 1) + 1]))
                : (1 + pDstType[-4]);

            *pDstType = dec_aic_prob_rv8[A][B][prob1];
            if (*pDstType >= NUM_AI_MODES)
            {
                return RV_S_OUT_OF_RANGE;
            }
            pDstType++;
        }
    return RV_S_OK;
}  /* decodeintratypes_rv8 */


#if 0//hx chg
RV_Status
Decoder_decodeMacroBlockType(
    struct Decoder *t,
    struct DecoderMBInfo *pMBInfo,
    U8 quant_prev,
    struct CRealVideoBs *pBitstream,
    U32 * ulSkipModesLeft
)
{
    U32 M, N, length, info;
    U8  thisMBType;
    U8  bestMBType;

    I32 uWidthInMBs = t->m_pCurrentFrame->m_macroBlockSize.width;

#define  LAST_HIST_INDEX NUMBER_OF_MBTYPES+3
    U32 uNeighborMBHist[LAST_HIST_INDEX] =
#ifdef INTERLACED_CODE
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 22 */
#else
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 15 */
#endif

    RV_Status status = RV_S_OK;


    if (*ulSkipModesLeft == 0)
    {
        /* Read the number of skipped MBs */
        length = CB_GetVLCBits(pBitstream,&info);
        *ulSkipModesLeft = (U32) LENGTH_INFO_TO_N(length,info);

        RVDebug((RV_TL_MBLK, "Num Skipped MBs: %d (%d/%d)",
                 *ulSkipModesLeft, length, info));
    }
    else
    {
        (*ulSkipModesLeft)--;
    }

    /* If there's still more MBs to skip, then skip them. */
    if (*ulSkipModesLeft > 0)
    {
        pMBInfo->mbtype = MBTYPE_SKIPPED;

#if defined(QUANTIZER_CHANGE_ALLOWED)
        pMBInfo->QP = quant_prev;
#else
        pMBInfo->QP = t->m_pCurrentFrame->m_pictureHeader.PQUANT;
#endif

        RVDebug((RV_TL_MBLK,"MB Type: MBTYPE_SKIPPED"));
        RVDebug((RV_TL_MBLK,"QP: %d", pMBInfo->QP));

        return status;
    }
    /* left */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-1].mbtype]++;
    }
    /* left-above */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) == 0 &&
            (pMBInfo[-1].edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-1 - uWidthInMBs].mbtype]++;
    }
    /* above */
    if ((pMBInfo->edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-uWidthInMBs].mbtype]++;
    }
    /* right-above */
    if ((pMBInfo->edge_type & D_RIGHT_EDGE) == 0 &&
            (pMBInfo->edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[1 - uWidthInMBs].mbtype]++;
    }

    /* make all MBTYPE_SKIPPED be MBTYPE_INTER or MBTYPE_DIRECT */
    if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        uNeighborMBHist[MBTYPE_INTER] += uNeighborMBHist[MBTYPE_SKIPPED];
        uNeighborMBHist[MBTYPE_SKIPPED] = 0;
    }
    else if (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        uNeighborMBHist[MBTYPE_DIRECT] += uNeighborMBHist[MBTYPE_SKIPPED];
        uNeighborMBHist[MBTYPE_SKIPPED] = 0;
    }

    for (M = 0, N = 0, bestMBType = MBTYPE_SKIPPED; M < LAST_HIST_INDEX; M++)
    {
        if (uNeighborMBHist[M] > N)
        {
            N = uNeighborMBHist[M];
            bestMBType = (U8)M;
        }
    }

    RVDebug((RV_TL_MBLK, "bestMBType: %d", bestMBType));

    /*  PiFrame Mb Types
        MBTYPE_INTRA,           // 0 , mbtype=0
        MBTYPE_INTRA_16x16,     // 1 , mbtype=1
        MBTYPE_INTER,           // 2 , mbtype=2
        MBTYPE_INTER_4V,        // 3 , mbtype=3
        MBTYPE_INTER_16x8V,     // 4 , mbtype=8
        MBTYPE_INTER_8x16V,     // 5 , mbtype=9
        MBTYPE_INTER_16x16,     // 6 P_DQUANT 7, mbtype=11
        MBTYPE_INTER_16x16i,    // 7 , mbtype=12
        MBTYPE_INTER_16x8i,     // 8 , mbtype=13
        MBTYPE_INTER_8x8i,      // 9 , mbtype=14
        PI_DQUANT               // 10  */
#define P_MBTYPE_DQUANT 7
#define PI_MBTYPE_DQUANT 10

    /* Construct and write codeword */
    if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        {
            M = mbtype_p_enc_tab[bestMBType];

            for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++)
            {
                info = mbtype_pframe_vlc[M][thisMBType][0];
                length = mbtype_pframe_vlc[M][thisMBType][1];
                if (CB_SearchBits(pBitstream,length, info, 0))
                    break;
            }
            RVAssert(thisMBType <= P_MBTYPE_DQUANT);

            if (thisMBType == P_MBTYPE_DQUANT)
            {
                /* DQUANT escape code */
                for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++)
                {
                    info = mbtype_pframe_vlc[M][thisMBType][0];
                    length = mbtype_pframe_vlc[M][thisMBType][1];
                    if (CB_SearchBits(pBitstream,length, info, 0))
                        break;
                }

                RVAssert(thisMBType < P_MBTYPE_DQUANT);

                /* Get DQUANT */
                CB_GetDQUANT(pBitstream,&(pMBInfo->QP),quant_prev,TRUE);
            }
#if defined(QUANTIZER_CHANGE_ALLOWED)
            else
            {
                pMBInfo->QP = quant_prev;
            }
#endif

            pMBInfo->mbtype = mbtype_p_dec_tab[thisMBType];
        }
    }
    else
    {
        /*  BiFrame Mb Types
            MBTYPE_INTRA,           // 0 , mbtype=0
            MBTYPE_INTRA_16x16,     // 1 , mbtype=1
            MBTYPE_FORWARD,         // 2 , mbtype=4
            MBTYPE_BACKWARD,        // 3 , mbtype=5
            MBTYPE_DIRECT,          // 4 , mbtype=7
            MBTYPE_BIDIR,           // 5  B_DQUANT 6 , mbtype=10
            MBTYPE_FORWARD_16x16i,  // 6 , mbtype=15
            MBTYPE_BACKWARD_16x16i, // 7 , mbtype=16
            MBTYPE_FORWARD_16x8i,   // 8 , mbtype=17
            MBTYPE_BACKWARD_16x8i,  // 9 , mbtype=18
            BI_DQUANT               //10 */
#define B_MBTYPE_DQUANT 6
#define BI_MBTYPE_DQUANT 10

        {
            M = mbtype_b_enc_tab[bestMBType];

            for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
            {
                info = mbtype_bframe_vlc[M][thisMBType][0];
                length = mbtype_bframe_vlc[M][thisMBType][1];
                if (CB_SearchBits(pBitstream,length, info, 0))
                    break;
            }
            RVAssert(thisMBType <= B_MBTYPE_DQUANT);

            if (thisMBType == B_MBTYPE_DQUANT)
            {
                /* DQUANT escape code */
                for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
                {
                    info = mbtype_bframe_vlc[M][thisMBType][0];
                    length = mbtype_bframe_vlc[M][thisMBType][1];
                    if (CB_SearchBits(pBitstream,length, info, 0))
                        break;
                }

                RVAssert(thisMBType < B_MBTYPE_DQUANT);

                /* Get DQUANT */
                CB_GetDQUANT(pBitstream,&(pMBInfo->QP),quant_prev,TRUE);
            }
#if defined(QUANTIZER_CHANGE_ALLOWED)
            else
            {
                pMBInfo->QP = quant_prev;
            }
#endif

            pMBInfo->mbtype = mbtype_b_dec_tab[thisMBType];
        }
    }

    RVAssert(pMBInfo->mbtype != 255);

    RVDebug((RV_TL_MBLK,"MBTYPE_%s",
             pMBInfo->mbtype == MBTYPE_INTER       ? "INTER" :
             pMBInfo->mbtype == MBTYPE_INTER_4V    ? "INTER_4V" :
             pMBInfo->mbtype == MBTYPE_INTRA       ? "INTRA" :
             pMBInfo->mbtype == MBTYPE_INTRA_16x16 ? "INTRA_16x16" :
             pMBInfo->mbtype == MBTYPE_INTER_16x8V ? "INTER_16x8V" :
             pMBInfo->mbtype == MBTYPE_INTER_8x16V ? "INTER_8x16V" :
             pMBInfo->mbtype == MBTYPE_DIRECT      ? "DIRECT" :
             pMBInfo->mbtype == MBTYPE_FORWARD     ? "FORWARD" :
             pMBInfo->mbtype == MBTYPE_BIDIR       ? "BIDIR" :
             pMBInfo->mbtype == MBTYPE_BACKWARD    ? "BACKWARD" :
             pMBInfo->mbtype == MBTYPE_INTER_16x16 ? "INTER_16x16" :
             "ERROR" ));

    RVDebug((RV_TL_MBLK,"QP: %d", pMBInfo->QP));

    return status;
}
#endif


RV_Status
Decoder_decodeMacroBlockType(
    struct Decoder *t,
    struct DecoderMBInfo *pMBInfo,
    U8 quant_prev,
    struct CRealVideoBs *pBitstream,
    U32 * ulSkipModesLeft
)
{
    U32 M, N, length, info,Dtmp32;
    U8  thisMBType;
    U8  bestMBType;

    I32 uWidthInMBs = t->m_pCurrentFrame->m_macroBlockSize.width;

#define  LAST_HIST_INDEX NUMBER_OF_MBTYPES+3
    U32 uNeighborMBHist[LAST_HIST_INDEX] =
#ifdef INTERLACED_CODE
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 22 */
#else
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 15 */
#endif

    RV_Status status = RV_S_OK;


    if (*ulSkipModesLeft == 0)
    {
        /* Read the number of skipped MBs */
        length = CB_GetVLCBits(pBitstream,&info);
        *ulSkipModesLeft = (U32) LENGTH_INFO_TO_N(length,info);
    }
    else
    {
        (*ulSkipModesLeft)--;
    }

    /* If there's still more MBs to skip, then skip them. */
    if (*ulSkipModesLeft > 0)
    {
        pMBInfo->mbtype = MBTYPE_SKIPPED;

#if defined(QUANTIZER_CHANGE_ALLOWED)
        pMBInfo->QP = quant_prev;
#else
        pMBInfo->QP = t->m_pCurrentFrame->m_pictureHeader.PQUANT;
#endif
        return status;
    }

    /* left */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-1].mbtype]++;
    }
    /* left-above */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) == 0 &&
            (pMBInfo[-1].edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-1 - uWidthInMBs].mbtype]++;
    }
    /* above */
    if ((pMBInfo->edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[-uWidthInMBs].mbtype]++;
    }
    /* right-above */
    if ((pMBInfo->edge_type & D_RIGHT_EDGE) == 0 &&
            (pMBInfo->edge_type & D_TOP_EDGE) == 0)
    {
        uNeighborMBHist[pMBInfo[1 - uWidthInMBs].mbtype]++;
    }

    /* make all MBTYPE_SKIPPED be MBTYPE_INTER or MBTYPE_DIRECT */
    if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        uNeighborMBHist[MBTYPE_INTER] += uNeighborMBHist[MBTYPE_SKIPPED];
        uNeighborMBHist[MBTYPE_SKIPPED] = 0;
    }
    else if (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        uNeighborMBHist[MBTYPE_DIRECT] += uNeighborMBHist[MBTYPE_SKIPPED];
        uNeighborMBHist[MBTYPE_SKIPPED] = 0;
    }

    for (M = 0, N = 0, bestMBType = MBTYPE_SKIPPED; M < LAST_HIST_INDEX; M++)
    {
        if (uNeighborMBHist[M] > N)
        {
            N = uNeighborMBHist[M];
            bestMBType = (U8)M;
        }
    }

#define P_MBTYPE_DQUANT 7
#define PI_MBTYPE_DQUANT 10

    /* Construct and write codeword */
    if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        {
            M = mbtype_p_enc_tab[bestMBType];

            Dtmp32 = (U32) *(pBitstream->m_pbs);
            Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
            Dtmp32 = Dtmp32<<(16+pBitstream->m_bitOffset);

            for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++)
            {
                info = mbtype_pframe_vlc[M][thisMBType][0];
                length = mbtype_pframe_vlc[M][thisMBType][1];

                if (info ==(Dtmp32 >> (32- length) ))
                {
                    for (Dtmp32 = pBitstream->m_bitOffset + length; Dtmp32 >= 8; Dtmp32 -= 8)
                        pBitstream->m_pbs ++;
                    pBitstream->m_bitOffset = Dtmp32;
                    break;
                }
            }

            if (thisMBType == P_MBTYPE_DQUANT)
            {
                Dtmp32 = (U32) *(pBitstream->m_pbs);
                Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                Dtmp32 = Dtmp32<<(16+pBitstream->m_bitOffset);
                /* DQUANT escape code */
                for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++)
                {
                    info = mbtype_pframe_vlc[M][thisMBType][0];
                    length = mbtype_pframe_vlc[M][thisMBType][1];

                    if (info ==(Dtmp32 >> (32- length) ))
                    {
                        for (Dtmp32 = pBitstream->m_bitOffset + length; Dtmp32 >= 8; Dtmp32 -= 8)
                            pBitstream->m_pbs ++;
                        pBitstream->m_bitOffset = Dtmp32;
                        break;
                    }
                }
                /* Get DQUANT */
                CB_GetDQUANT(pBitstream,&(pMBInfo->QP),quant_prev,TRUE);
            }
#if defined(QUANTIZER_CHANGE_ALLOWED)
            else
            {
                pMBInfo->QP = quant_prev;
            }
#endif

            pMBInfo->mbtype = mbtype_p_dec_tab[thisMBType];
        }
    }
    else
    {
#define B_MBTYPE_DQUANT 6
#define BI_MBTYPE_DQUANT 10

        {
            M = mbtype_b_enc_tab[bestMBType];

            Dtmp32 = (U32) *(pBitstream->m_pbs);
            Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
            Dtmp32 = Dtmp32<<(16+pBitstream->m_bitOffset);

            for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
            {
                info = mbtype_bframe_vlc[M][thisMBType][0];
                length = mbtype_bframe_vlc[M][thisMBType][1];
                if (info ==(Dtmp32 >> (32- length) ))
                {
                    for (Dtmp32 = pBitstream->m_bitOffset + length; Dtmp32 >= 8; Dtmp32 -= 8)
                        pBitstream->m_pbs ++;
                    pBitstream->m_bitOffset = Dtmp32;
                    break;
                }
            }

            if (thisMBType == B_MBTYPE_DQUANT)
            {
                /* DQUANT escape code */
                Dtmp32 = (U32) *(pBitstream->m_pbs);
                Dtmp32 = (Dtmp32 << 8) | *((pBitstream->m_pbs)+1);
                Dtmp32 = Dtmp32<<(16+pBitstream->m_bitOffset);

                /* DQUANT escape code */
                for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
                {
                    info = mbtype_bframe_vlc[M][thisMBType][0];
                    length = mbtype_bframe_vlc[M][thisMBType][1];
                    if (info ==(Dtmp32 >> (32- length) ))
                    {
                        for (Dtmp32 = pBitstream->m_bitOffset + length; Dtmp32 >= 8; Dtmp32 -= 8)
                            pBitstream->m_pbs ++;
                        pBitstream->m_bitOffset = Dtmp32;
                        break;
                    }
                }

                /* Get DQUANT */
                CB_GetDQUANT(pBitstream,&(pMBInfo->QP),quant_prev,TRUE);
            }
#if defined(QUANTIZER_CHANGE_ALLOWED)
            else
            {
                pMBInfo->QP = quant_prev;
            }
#endif
            pMBInfo->mbtype = mbtype_b_dec_tab[thisMBType];
        }
    }

    return status;
}



RV_Status
Decoder_decodeMacroBlockType_RV8(struct Decoder *t,
                                 U8 *mbtype,
                                 U8 *QP,
                                 U8 quant_prev,
                                 struct CRealVideoBs *pBitstream
                                )
{
    U32 length, info;
    U8 mode;
    RV_Status status = RV_S_OK;

    length = CB_GetVLCBits(pBitstream,&info);


    mode = (U8) LENGTH_INFO_TO_N(length,info);

    if (mode > 5)
    {
        /* dquant */
        mode -= 5;

        CB_GetDQUANT(pBitstream,QP,quant_prev,TRUE);
    }

    if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        switch (mode)
        {
            case 0:
                *mbtype = MBTYPE_SKIPPED;
                break;
            case 1:
                *mbtype = MBTYPE_INTER;
                break;
            case 2:
                *mbtype = MBTYPE_INTER_4V;
                break;
            /*        case 3: */
            /*            *mbtype = MBTYPE_INTER_16V; */
            /*            break;   */
            /* there are no RV8 bitstreams with 16 motion vectors */
            case 4:
                *mbtype = MBTYPE_INTRA;
                break;
            case 5:
                *mbtype = MBTYPE_INTRA_16x16;
                break;
            default:
                RVDebug((RV_TL_ALWAYS,"ERROR: "
                         "Decoder::decodeMacroBlockType_RV8 -- "
                         "Illegal mbtype"));
                status = RV_S_ERROR;
                break;
        }
    }
    else if (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        switch (mode)
        {
            case 0:
                *mbtype = MBTYPE_SKIPPED;
                break;
            case 1:
                *mbtype = MBTYPE_DIRECT;
                break;
            case 2:
                *mbtype = MBTYPE_FORWARD;
                break;
            case 3:
                *mbtype = MBTYPE_BACKWARD;
                break;
            case 4:
                *mbtype = MBTYPE_INTRA;
                break;
            case 5:
                *mbtype = MBTYPE_INTRA_16x16;
                break;
            default:
                RVDebug((RV_TL_ALWAYS,"ERROR: "
                         "Decoder::decodeMacroBlockType_RV8 -- "
                         "Illegal mbtype"));
                status = RV_S_ERROR;
                break;
        }
    }
    else
    {
        RVDebug((RV_TL_ALWAYS,"ERROR: "
                 "Decoder::decodeMacroBlockType_RV8 -- "
                 "illegal frame type"));
    }

    RVDebug((RV_TL_MBLK,"MB Type: %d  NLI %2d %2d %2d",
             mode, mode, length, info));
    RVDebug((RV_TL_MBLK,"QP: %d", *QP));

    return status;
}  /* decodemacroblocktype_rv8 */


U32
Decoder_decodeCBP(struct Decoder *t, U32 mbtype, struct CRealVideoBs *pBitstream)
{
    U32 length, info, N;
    U32 cbp;

    length = CB_GetVLCBits(pBitstream,&info);


    N = LENGTH_INFO_TO_N(length,info);

    if (mbtype==MBTYPE_INTRA)
    {
        if (N < sizeof(dec_cbp_intra)/sizeof(dec_cbp_intra[0]))
            cbp = dec_cbp_intra[N];
        else
        {
            RVDebug((RV_TL_ALWAYS,"Decoder::decodeCBP --"
                     "ERROR decoding intra CBP"));
            cbp = 255;
        }
    }
    else if (mbtype==MBTYPE_INTRA_16x16)
    {
        if (N < sizeof(dec_cbp_intra16x16)/sizeof(dec_cbp_intra16x16[0]))
            cbp = dec_cbp_intra16x16[N];
        else
        {
            RVDebug((RV_TL_ALWAYS,"Decoder::decodeCBP --"
                     "ERROR decoding intra 16x16 CBP"));
            cbp = 255;
        }
    }
    else
    {
        if (N < sizeof(dec_cbp_inter)/sizeof(dec_cbp_inter[0]))
            cbp = dec_cbp_inter[N];
        else
        {
            RVDebug((RV_TL_ALWAYS,"Decoder::decodeCBP --"
                     "ERROR decoding inter CBP"));
            cbp = 255;
        }
    }

    RVDebug((RV_TL_BLK,"cbp %d: N L INFO %d %d %d",
             cbp, N, length, info));

    return cbp;
}

#define MEDIAN_OF_3(a, b, c) \
    (((a) > (b)) \
        ? (((b) > (c)) \
            ? (b) \
            : (((a) > (c)) ? (c) : (a))) \
        : (((a) > (c)) \
            ? (a) \
            : (((b) > (c)) ? (c) : (b))))



/*//////////////////////////////////////////////////////// */
/* Calculate motion vector predictor for a specific block. */
/* B frame backward and forward MB types */

RV_Status
Decoder_computeMotionVectorPredictorsBFrame
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy,
    const U32 mbnum
)
{
    Bool32  isRightUnavailable;
    /* Indicates whether the (above) right block, subblock or */
    /* macroblock has not had its motion vectors decoded yet. */
    /* This is not a true boolean, in that we use bitwise operations */
    /* so that any non-zero value, not just the value TRUE, */
    /* is considered true. */
    U32 mbtype;

    const struct DecoderPackedMotionVector *sl=0; /* pointer to sb to the left */
    const struct DecoderPackedMotionVector *sa=0; /* pointer to sb above */
    const struct DecoderPackedMotionVector *sr=0; /* pointer to sb above right */
    struct DecoderPackedMotionVector null;

    RV_Boolean leftSameMBType=0;
    RV_Boolean aboveSameMBType=0;
    RV_Boolean aboveRightSameMBType=0;


    isRightUnavailable = (pMBInfo->edge_type & D_RIGHT_EDGE);

    mbtype = pMBInfo->mbtype;

    if (mbtype == MBTYPE_FORWARD
#ifdef INTERLACED_CODE
            || mbtype == MBTYPE_FORWARD_16x16i
            || mbtype == MBTYPE_FORWARD_16x8i
#endif
       )
    {
        /* correct for left edge */
        /* no vector info required */
        if (pMBInfo->edge_type & D_LEFT_EDGE)
        {
            sl = &null;
            null.mvx = 0;
            null.mvy = 0;
            leftSameMBType = FALSE;
        }
        else
        {
            sl = pMV - 1;
#ifdef INTERLACED_CODE
            leftSameMBType =
                IS_FORWARD_OR_BIDIR((pMBInfo - 1)->mbtype)
                ? TRUE : FALSE;
#else
            leftSameMBType =
                (pMBInfo - 1)->mbtype == mbtype
                || (pMBInfo - 1)->mbtype == MBTYPE_BIDIR
                ? TRUE : FALSE;
#endif
        }

        /* correct for top edge */
        if (pMBInfo->edge_type & D_TOP_EDGE)
        {
            sa = sr = sl;
            aboveSameMBType = FALSE;
            aboveRightSameMBType = FALSE;
        }
        else
        {
#ifdef INTERLACED_CODE
            if(mbtype == MBTYPE_FORWARD_16x8i)
            {
                sa = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width;
            }
            else
#endif
            {
                sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
            }
#ifdef INTERLACED_CODE
            aboveSameMBType =
                IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype)
                ? TRUE : FALSE;
#else
            aboveSameMBType =
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype == mbtype
                || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype == MBTYPE_BIDIR
                ? TRUE : FALSE;
#endif

            /* correct for vectors not yet available or to the right */
            /* of the frame */
            if (isRightUnavailable)
            {
                sr = sa - 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype)
                    ? TRUE : FALSE;
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == mbtype
                    || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == MBTYPE_BIDIR
                    ? TRUE : FALSE;
#endif
            }
            else
            {
#ifdef INTERLACED_CODE
                if(mbtype == MBTYPE_FORWARD_16x8i)
                {
                    sr = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width + 4;
                }
                else
#endif
                {
                    sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + 4;
                }
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype)
                    ? TRUE : FALSE;
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype == mbtype
                    || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype == MBTYPE_BIDIR
                    ? TRUE : FALSE;
#endif
            }

        }
    }
    else if (mbtype == MBTYPE_BACKWARD
#ifdef INTERLACED_CODE
             || mbtype == MBTYPE_BACKWARD_16x16i
             || mbtype == MBTYPE_BACKWARD_16x8i
#endif
            )
    {
        struct DecoderPackedMotionVector *pMVb;
        pMVb = t->m_pBidirMotionVectors + mbnum;

        /* correct for left edge */
        /* no vector info required */
        if (pMBInfo->edge_type & D_LEFT_EDGE)
        {
            sl = &null;
            null.mvx = 0;
            null.mvy = 0;
            leftSameMBType = FALSE;
        }
        else
        {
            sl = pMVb - 1;
#ifdef INTERLACED_CODE
            leftSameMBType =
                IS_BACKWARD_OR_BIDIR((pMBInfo - 1)->mbtype)
                ? TRUE : FALSE;
            if((pMBInfo - 1)->mbtype != MBTYPE_BIDIR)
            {
                sl = pMV - 1;
            }
#else
            leftSameMBType =
                (pMBInfo - 1)->mbtype == mbtype
                || (pMBInfo - 1)->mbtype == MBTYPE_BIDIR
                ? TRUE : FALSE;
#endif
        }

        /* correct for top edge */
        if (pMBInfo->edge_type & D_TOP_EDGE)
        {
            sa = sr = sl;
            aboveSameMBType = FALSE;
            aboveRightSameMBType = FALSE;
        }
        else
        {
            sa = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width;
#ifdef INTERLACED_CODE
            aboveSameMBType =
                IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype)
                ? TRUE : FALSE;
            if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype != MBTYPE_BIDIR)
            {
                if(mbtype == MBTYPE_BACKWARD_16x8i)
                {
                    sa = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width;
                }
                else
                {
                    sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
                }
            }
#else
            aboveSameMBType =
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype == mbtype
                || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype == MBTYPE_BIDIR
                ? TRUE : FALSE;
#endif

            /* correct for vectors not yet available or to the right */
            /* of the frame */
            if (isRightUnavailable)
            {
                sr = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width - 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype)
                    ? TRUE : FALSE;
                if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype != MBTYPE_BIDIR)
                {
                    if(mbtype == MBTYPE_BACKWARD_16x8i)
                    {
                        sr = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width -1;
                    }
                    else
                    {
                        sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width -1;
                    }
                }
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == mbtype
                    || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == MBTYPE_BIDIR
                    ? TRUE : FALSE;
#endif
            }
            else
            {
                sr = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width + 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype)
                    ? TRUE : FALSE;
                if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype != MBTYPE_BIDIR)
                {
                    if(mbtype == MBTYPE_BACKWARD_16x8i)
                    {
                        sr = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width + 4;
                    }
                    else
                    {
                        sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + 4;
                    }
                }
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype == mbtype
                    || (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype == MBTYPE_BIDIR
                    ? TRUE : FALSE;
#endif
            }

        }
    }
    else
    {
        RVAssert(0);
    }


    if (leftSameMBType && aboveSameMBType && aboveRightSameMBType)
    {
        *pMVx = MEDIAN_OF_3(sl->mvx, sa->mvx, sr->mvx);
        *pMVy = MEDIAN_OF_3(sl->mvy, sa->mvy, sr->mvy);
    }
    else if (leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
    {
        *pMVx = (sl->mvx + sa->mvx) / 2;
        *pMVy = (sl->mvy + sa->mvy) / 2;
    }
    else if (leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
    {
        *pMVx = (sl->mvx + sr->mvx) / 2;
        *pMVy = (sl->mvy + sr->mvy) / 2;
    }
    else if (!leftSameMBType && aboveSameMBType && aboveRightSameMBType)
    {
        *pMVx = (sr->mvx + sa->mvx) / 2;
        *pMVy = (sr->mvy + sa->mvy) / 2;
    }
    else if (leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
    {
        *pMVx = sl->mvx;
        *pMVy = sl->mvy;
    }
    else if (!leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
    {
        *pMVx = sa->mvx;
        *pMVy = sa->mvy;
    }
    else if (!leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
    {
        *pMVx = sr->mvx;
        *pMVy = sr->mvy;
    }
    else if (!leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
    {
        *pMVx = *pMVy = 0;
    }
    else
    {
        RVAssert(0);
    }

    return RV_S_OK;

}



/*//////////////////////////////////////////////////////// */
/* Calculate motion vector predictor for a specific block. */
/* B frame backward and forward MB types */

RV_Status
Decoder_computeMotionVectorPredictorsBFrameBidir
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy,
    const U32 mbnum,
    const U32 block
)
{
    Bool32  isRightUnavailable;
    /* Indicates whether the (above) right block, subblock or */
    /* macroblock has not had its motion vectors decoded yet. */
    /* This is not a true boolean, in that we use bitwise operations */
    /* so that any non-zero value, not just the value TRUE, */
    /* is considered true. */

    const struct DecoderPackedMotionVector *sl; /* pointer to sb to the left */
    const struct DecoderPackedMotionVector *sa; /* pointer to sb above */
    const struct DecoderPackedMotionVector *sr; /* pointer to sb above right */
    struct DecoderPackedMotionVector null;
    U32 mbtype;

    RV_Boolean leftSameMBType;
    RV_Boolean aboveSameMBType;
    RV_Boolean aboveRightSameMBType;


    isRightUnavailable = (pMBInfo->edge_type & D_RIGHT_EDGE);

    mbtype = pMBInfo->mbtype;


    RVAssert (mbtype == MBTYPE_BIDIR);

    if (block == 0)
        /* FORWARD */
    {
        /* correct for left edge */
        if (pMBInfo->edge_type & D_LEFT_EDGE)
        {
            sl = &null;
            null.mvx = 0;
            null.mvy = 0;
            leftSameMBType = FALSE;
        }
        else
        {
            sl = pMV - 1;
#ifdef INTERLACED_CODE
            leftSameMBType =
                IS_FORWARD_OR_BIDIR((pMBInfo - 1)->mbtype)
                ? TRUE : FALSE;
#else
            leftSameMBType =
                (pMBInfo - 1)->mbtype == mbtype ||
                (pMBInfo - 1)->mbtype == MBTYPE_FORWARD
                ? TRUE : FALSE;
#endif
        }

        /* correct for top edge */
        if (pMBInfo->edge_type & D_TOP_EDGE)
        {
            sa = sr = sl;
            aboveSameMBType = FALSE;
            aboveRightSameMBType = FALSE;
        }
        else
        {
            sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
#ifdef INTERLACED_CODE
            aboveSameMBType =
                IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype)
                ? TRUE : FALSE;
#else
            aboveSameMBType =
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype  == mbtype ||
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype  == MBTYPE_FORWARD
                ? TRUE : FALSE;
#endif


            /* correct for vectors not yet available or to the right */
            /* of the frame */
            if (isRightUnavailable)
            {
                sr = sa - 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype)
                    ? TRUE : FALSE;
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == mbtype ||
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == MBTYPE_FORWARD
                    ? TRUE : FALSE;
#endif
            }
            else
            {
                sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + 4;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_FORWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype)
                    ? TRUE : FALSE;
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype  == mbtype ||
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype  == MBTYPE_FORWARD
                    ? TRUE : FALSE;
#endif
            }
        }

        if (leftSameMBType && aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = MEDIAN_OF_3(sl->mvx, sa->mvx, sr->mvx);
            *pMVy = MEDIAN_OF_3(sl->mvy, sa->mvy, sr->mvy);
        }
        else if (leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = (sl->mvx + sa->mvx) / 2;
            *pMVy = (sl->mvy + sa->mvy) / 2;
        }
        else if (leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = (sl->mvx + sr->mvx) / 2;
            *pMVy = (sl->mvy + sr->mvy) / 2;
        }
        else if (!leftSameMBType && aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = (sr->mvx + sa->mvx) / 2;
            *pMVy = (sr->mvy + sa->mvy) / 2;
        }
        else if (leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = sl->mvx;
            *pMVy = sl->mvy;
        }
        else if (!leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = sa->mvx;
            *pMVy = sa->mvy;
        }
        else if (!leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = sr->mvx;
            *pMVy = sr->mvy;
        }
        else if (!leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = *pMVy = 0;
        }
        else
        {
            RVAssert(0);
        }
    }

    else if (block == 1)
        /* BACKWARD */
    {
        struct DecoderPackedMotionVector *pMVb;
        pMVb = t->m_pBidirMotionVectors + mbnum;

        /* correct for left edge */
        if (pMBInfo->edge_type & D_LEFT_EDGE)
        {
            sl = &null;
            null.mvx = 0;
            null.mvy = 0;
            leftSameMBType = FALSE;
        }
        else
        {
            sl = pMVb - 1;

#ifdef INTERLACED_CODE
            leftSameMBType = IS_BACKWARD_OR_BIDIR((pMBInfo - 1)->mbtype)
                             ? TRUE : FALSE;
            if((pMBInfo - 1)->mbtype != MBTYPE_BIDIR)
            {
                sl = pMV - 1;
            }
#else
            leftSameMBType =
                (pMBInfo - 1)->mbtype == mbtype ||
                (pMBInfo - 1)->mbtype == MBTYPE_BACKWARD
                ? TRUE : FALSE;
#endif
        }

        /* correct for top edge */
        if (pMBInfo->edge_type & D_TOP_EDGE)
        {
            sa = sr = sl;
            aboveSameMBType = FALSE;
            aboveRightSameMBType = FALSE;
        }
        else
        {
            sa = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width;

#ifdef INTERLACED_CODE
            aboveSameMBType =
                IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype)
                ? TRUE : FALSE;

            if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype != MBTYPE_BIDIR)
            {
                sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
            }
#else
            aboveSameMBType =
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype  == mbtype ||
                (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width)->mbtype  == MBTYPE_BACKWARD
                ? TRUE : FALSE;
#endif

            /* correct for vectors not yet available or to the right */
            /* of the frame */
            if (isRightUnavailable)
            {
                sr = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width - 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype)
                    ? TRUE : FALSE;

                if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype != MBTYPE_BIDIR)
                {
                    sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width - 1;
                }
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == mbtype ||
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width - 1)->mbtype == MBTYPE_BACKWARD
                    ? TRUE : FALSE;
#endif

            }
            else
            {
                sr = pMVb - t->m_pCurrentFrame->m_macroBlockSize.width + 1;
#ifdef INTERLACED_CODE
                aboveRightSameMBType =
                    IS_BACKWARD_OR_BIDIR((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype)
                    ? TRUE : FALSE;
                if((pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype != MBTYPE_BIDIR)
                {
                    sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + 4;
                }
#else
                aboveRightSameMBType =
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype  == mbtype ||
                    (pMBInfo - t->m_pCurrentFrame->m_macroBlockSize.width + 1)->mbtype  == MBTYPE_BACKWARD
                    ? TRUE : FALSE;
#endif
            }

        }

        if (leftSameMBType && aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = MEDIAN_OF_3(sl->mvx, sa->mvx, sr->mvx);
            *pMVy = MEDIAN_OF_3(sl->mvy, sa->mvy, sr->mvy);
        }
        else if (leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = (sl->mvx + sa->mvx) / 2;
            *pMVy = (sl->mvy + sa->mvy) / 2;
        }
        else if (leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = (sl->mvx + sr->mvx) / 2;
            *pMVy = (sl->mvy + sr->mvy) / 2;
        }
        else if (!leftSameMBType && aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = (sr->mvx + sa->mvx) / 2;
            *pMVy = (sr->mvy + sa->mvy) / 2;
        }
        else if (leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = sl->mvx;
            *pMVy = sl->mvy;
        }
        else if (!leftSameMBType && aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = sa->mvx;
            *pMVy = sa->mvy;
        }
        else if (!leftSameMBType && !aboveSameMBType && aboveRightSameMBType)
        {
            *pMVx = sr->mvx;
            *pMVy = sr->mvy;
        }
        else if (!leftSameMBType && !aboveSameMBType && !aboveRightSameMBType)
        {
            *pMVx = *pMVy = 0;
        }
        else
        {
            RVAssert(0);
        }
    }
    else
    {
        RVAssert(0);
    }

    return RV_S_OK;

}


/*//////////////////////////////////////////////////////// */
/* Calculate motion vector predictor for a specific block. */

RV_Status
Decoder_computeMotionVectorPredictors
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    const I32 block,    /* block or subblock number, depending on mbtype */
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy
)
{
    I32     px0, px1, px2;
    I32     py0, py1, py2;
    I32     diff;

    Bool32  left_edge_block, top_edge_block;
    /* Indicate whether the block, subblock or macroblock is a leftmost */
    /* or topmost entity in its containing macroblock. */

    Bool32  isRightUnavailable;
    /* Indicates whether the (above) right block, subblock or */
    /* macroblock has not had its motion vectors decoded yet. */
    /* This is not a true boolean, in that we use bitwise operations */
    /* so that any non-zero value, not just the value TRUE, */
    /* is considered true. */


    const struct DecoderPackedMotionVector *sl; /* pointer to sb to the left */
    const struct DecoderPackedMotionVector *sa; /* pointer to sb above */
    const struct DecoderPackedMotionVector *sr; /* pointer to sb above right */
    struct DecoderPackedMotionVector null;

    switch (pMBInfo->mbtype)
    {
#ifdef SKIPPED_RV10i
        case MBTYPE_SKIPPED:
#endif
        case MBTYPE_INTER:
        case MBTYPE_INTER_16x16:
#ifdef INTERLACED_CODE
        case MBTYPE_INTER_16x16i:
#endif
            RVAssert(block == 0);
            diff = 4;
            left_edge_block = TRUE;
            top_edge_block = TRUE;
            isRightUnavailable = (pMBInfo->edge_type & D_RIGHT_EDGE);
            break;

        case MBTYPE_INTER_4V:
            RVAssert(block >= 0 && block <= 3);
            diff = 2;
            left_edge_block = left_edge_tab4[block];
            top_edge_block = top_edge_tab4[block];
            /* right is unavailable if this is block 3, or if this */
            /* is block 1 of the rightmost MB */
            isRightUnavailable = not_available8[block]
                                 | ((pMBInfo->edge_type & D_RIGHT_EDGE) && xoff8[block]);
            break;

#ifdef INTERLACED_CODE
        case MBTYPE_INTER_8x8i:
            RVAssert(block >= 0 && block <= 3);
            diff = 2;
            left_edge_block = left_edge_tab4[block];
            top_edge_block = 1;
            /* right is unavailable if this is block 1 or 3 of the rightmost MB */
            isRightUnavailable =  ((pMBInfo->edge_type & D_RIGHT_EDGE) && xoff8[block]);
            break;
#endif

        case MBTYPE_INTER_16x8V:
            RVAssert(block >= 0 && block <= 1);
            diff = 4;
            left_edge_block = TRUE;
            top_edge_block = (block ^ 1);
            isRightUnavailable = (block) | (pMBInfo->edge_type & D_RIGHT_EDGE);
            break;

#ifdef INTERLACED_CODE
        case MBTYPE_INTER_16x8i:
            RVAssert(block >= 0 && block <= 1);
            diff = 4;
            left_edge_block = TRUE;
            top_edge_block = TRUE;
            isRightUnavailable = (pMBInfo->edge_type & D_RIGHT_EDGE);
            break;
#endif
        case MBTYPE_INTER_8x16V:
            RVAssert(block >= 0 && block <= 1);
            diff = 2;
            left_edge_block = (block ^ 1);
            top_edge_block = TRUE;
            isRightUnavailable = ((pMBInfo->edge_type & D_RIGHT_EDGE) && (block));
            break;

        default:
            RVDebug((RV_TL_ALWAYS,
                     "ERROR: Decoder::computeMotionVectorPredictors -- "
                     "Illegal mbtype (%ld) encountered", (U32)pMBInfo->mbtype));
            *pMVx = 0;
            *pMVy = 0;
            return RV_S_ERROR;
    }


    sl = pMV - 1;
#ifdef INTERLACED_CODE
    if(pMBInfo->mbtype == MBTYPE_INTER_8x8i
            || pMBInfo->mbtype == MBTYPE_INTER_16x8i)
    {
        sa = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width;
        sr = pMV - 4*t->m_pCurrentFrame->m_subBlockSize.width + diff;
    }
    else
#endif
    {
        sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
        sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + diff;
    }


    /* correct for left edge */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) && left_edge_block)
    {
        sl = &null;
        null.mvx = 0;
        null.mvy = 0;
    }

    /* correct for top edge */
    if ((pMBInfo->edge_type & D_TOP_EDGE) && top_edge_block)
    {
        sa = sr = sl;
    }

    /* correct for vectors not yet available or to the right */
    /* of the frame */
    if (isRightUnavailable)
    {
        /* This is Gisle's way of setting the MV predictor above right */
        /* to the block above left if this exists, left otherwise. */
        if ((pMBInfo->edge_type & D_TOP_EDGE) && top_edge_block)
        {
            sr = sl;
            /* replace with left block */
        }
        else
        {
            if ((pMBInfo->edge_type & D_LEFT_EDGE) && left_edge_block)
            {
                sr = sl;
            }
            else
            {
                sr = sa - 1;
                /* replace with block above left */
            }
        }
    }

    px0 = sl->mvx;
    px1 = sa->mvx;
    px2 = sr->mvx;


    *pMVx = MEDIAN_OF_3(px0, px1, px2);

    py0 = sl->mvy;
    py1 = sa->mvy;
    py2 = sr->mvy;

    *pMVy = MEDIAN_OF_3(py0, py1, py2);

    return RV_S_OK;
}

/*//////////////////////////////////////////////////////// */
/* Calculate motion vector predictor for a specific block. */

RV_Status
Decoder_computeMotionVectorPredictors_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    const I32 block,    /* block or subblock number, depending on mbtype */
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy
)
{
    I32     px0, px1, px2;
    I32     py0, py1, py2;
    I32     diff;

    Bool32  left_edge_block, top_edge_block;
    /* Indicate whether the block, subblock or macroblock is a leftmost */
    /* or topmost entity in its containing macroblock. */

    Bool32  isRightUnavailable;
    /* Indicates whether the (above) right block, subblock or */
    /* macroblock has not had its motion vectors decoded yet. */
    /* This is not a true boolean, in that we use bitwise operations */
    /* so that any non-zero value, not just the value TRUE, */
    /* is considered true. */


    const struct DecoderPackedMotionVector *sl; /* pointer to sb to the left */
    const struct DecoderPackedMotionVector *sa; /* pointer to sb above */
    const struct DecoderPackedMotionVector *sr; /* pointer to sb above right */
    struct DecoderPackedMotionVector null;

    switch (pMBInfo->mbtype)
    {
        case MBTYPE_INTER:
        case MBTYPE_FORWARD:
        case MBTYPE_BACKWARD:
            RVAssert(block == 0);
            diff = 4;
            left_edge_block = TRUE;
            top_edge_block = TRUE;
            isRightUnavailable = (pMBInfo->edge_type & D_RIGHT_EDGE);
            break;

        case MBTYPE_INTER_4V:
            RVAssert(block >= 0 && block <= 3);
            diff = 2;
            left_edge_block = left_edge_tab4[block];
            top_edge_block = top_edge_tab4[block];

            /* right is unavailable if this is block 3, or if this */
            /* is block 1 of the rightmost MB */
            isRightUnavailable = not_available8[block]
                                 | ((pMBInfo->edge_type & D_RIGHT_EDGE) && xoff8[block]);
            break;


        default:
            RVDebug((RV_TL_ALWAYS,
                     "ERROR: Decoder::computeMotionVectorPredictors -- "
                     "Illegal mbtype (%ld) encountered", (U32)pMBInfo->mbtype));
            *pMVx = 0;
            *pMVy = 0;
            return RV_S_ERROR;
    }


    sl = pMV - 1;
    sa = pMV - t->m_pCurrentFrame->m_subBlockSize.width;
    sr = pMV - t->m_pCurrentFrame->m_subBlockSize.width + diff;

    /* correct for left edge */
    if ((pMBInfo->edge_type & D_LEFT_EDGE) && left_edge_block)
    {
        sl = &null;
        null.mvx = 0;
        null.mvy = 0;
    }

    /* correct for top edge */
    if ((pMBInfo->edge_type & D_TOP_EDGE) && top_edge_block)
    {
        sa = sr = sl;
    }

    /* correct for vectors not yet available or to the right */
    /* of the frame */
    if (isRightUnavailable)
    {
        /* This is Gisle's way of setting the MV predictor above right */
        /* to the block above left if this exists, left otherwise. */
        if ((pMBInfo->edge_type & D_TOP_EDGE) && top_edge_block)
        {
            sr = sl;
            /* replace with left block */
        }
        else
        {
            sr = sa - 1;
            /* replace with block above left */
        }
    }

    px0 = sl->mvx;
    px1 = sa->mvx;
    px2 = sr->mvx;

    *pMVx = MEDIAN_OF_3(px0, px1, px2);

    py0 = sl->mvy;
    py1 = sa->mvy;
    py2 = sr->mvy;

    *pMVy = MEDIAN_OF_3(py0, py1, py2);

    return RV_S_OK;
} /* computemotionvectorpredictors_rv8 */



RV_Status
Decoder_decodeMotionVectors
(
    struct Decoder *t,
    struct DecoderMBInfo *pMBInfo,
    struct DecoderPackedMotionVector *pMV,
    const U32 mbnum,
    struct CRealVideoBs *pBitstream

)
{
    I32 num_vectors, vector, mvdx, mvdy, pmvx, pmvy, i, j;
    DecoderPackedMotionVectorComponent  mvx, mvy;
    U32 length, info, N;
    RV_Status status = RV_S_OK;
    struct DecoderPackedMotionVector *pMVorig = pMV;
#ifdef DEBUG
    RV_Trace_Level ptl = RV_TL_MBLK;
#endif

    switch (pMBInfo->mbtype)
    {
        case MBTYPE_INTER:
        case MBTYPE_FORWARD:
        case MBTYPE_BACKWARD:
        case MBTYPE_INTER_16x16:
#ifdef INTERLACED_CODE
        case MBTYPE_FORWARD_16x16i:
        case MBTYPE_BACKWARD_16x16i:
        case MBTYPE_INTER_16x16i:
#endif
            num_vectors = 1;
            break;
        case MBTYPE_BIDIR:
        case MBTYPE_INTER_16x8V:
        case MBTYPE_INTER_8x16V:
#ifdef INTERLACED_CODE
        case MBTYPE_BACKWARD_16x8i:
        case MBTYPE_FORWARD_16x8i:
        case MBTYPE_INTER_16x8i:
#endif
            num_vectors = 2;
            break;
        case MBTYPE_INTER_4V:
#ifdef INTERLACED_CODE
        case MBTYPE_INTER_8x8i:
#endif
            num_vectors = 4;
            break;
        default:
            RVDebug((RV_TL_ALWAYS, "ERROR: "
                     "Decoder::decodeMotionVectors -- "
                     "Illegal mbtype %d in this function", pMBInfo->mbtype));
            status = RV_S_ERROR;
            goto done;
    }

    for (vector = 0; vector < num_vectors; vector++)
    {
        /* Read horizontal component */
        length = CB_GetVLCBits(pBitstream,&info);
        if (length > 17)
        {
            status = RV_S_ERROR;
            goto done;
        }

        N = LENGTH_INFO_TO_N(length,info);
        mvdx = (N+1)>>1;
        if (!(N&1))
            mvdx = -mvdx;

        /* Read vertical component */
        length = CB_GetVLCBits(pBitstream,&info);
        if (length > 17)
        {
            status = RV_S_ERROR;
            goto done;
        }

        N = LENGTH_INFO_TO_N(length,info);
        mvdy = (N+1)>>1;
        if (!(N&1))
            mvdy = -mvdy;

        /* Find MV predictor */
        if (t->m_bIsRV8)
        {
            status =
                Decoder_computeMotionVectorPredictors_RV8(t,pMBInfo, pMV, vector, &pmvx, &pmvy);
        }
        else if (pMBInfo->mbtype == MBTYPE_FORWARD ||
                 pMBInfo->mbtype == MBTYPE_BACKWARD
                )
        {
            status =
                Decoder_computeMotionVectorPredictorsBFrame(t,pMBInfo, pMV, &pmvx, &pmvy, mbnum);
        }
        else if (pMBInfo->mbtype == MBTYPE_BIDIR)
        {
            status =
                Decoder_computeMotionVectorPredictorsBFrameBidir(t,pMBInfo, pMV, &pmvx, &pmvy, mbnum, vector);
        }
        else
        {
            status =
                Decoder_computeMotionVectorPredictors(t,pMBInfo, pMV, vector, &pmvx, &pmvy);
        }


        mvx =  (DecoderPackedMotionVectorComponent)(mvdx + pmvx);
        mvy =  (DecoderPackedMotionVectorComponent)(mvdy + pmvy);

        RVDebug((ptl,"Block %2d: mvx = mvdx + pmvx: %3d = %3d + %3d",
                 vector, mvx, mvdx, pmvx ));
        RVDebug((ptl,"Block %2d: mvy = mvdy + pmvy: %3d = %3d + %3d",
                 vector, mvy, mvdy, pmvy ));

        /* Store motion vectors in Sub Block Data buffer */
        switch (pMBInfo->mbtype)
        {
            case MBTYPE_BIDIR:
                if (vector == 0)
                {
                    for (i = 0; i < 4; i++)
                    {
                        for (j = 0; j < 4; j++)
                        {
                            pMV[j].mvx = mvx;
                            pMV[j].mvy = mvy;
                        }
                        pMV += t->m_pCurrentFrame->m_subBlockSize.width;
                    }
                }
                else
                {
                    struct DecoderPackedMotionVector *pMVb;
                    pMVb = t->m_pBidirMotionVectors + mbnum;

                    pMVb->mvx = mvx;
                    pMVb->mvy = mvy;
                }
                pMV = pMVorig;
                break;
            case MBTYPE_INTER:
            case MBTYPE_FORWARD:
            case MBTYPE_BACKWARD:
            case MBTYPE_INTER_16x16:
                for (i = 0; i < 4; i++)
                {
                    for (j = 0; j < 4; j++)
                    {
                        pMV[j].mvx = mvx;
                        pMV[j].mvy = mvy;
                    }
                    pMV += t->m_pCurrentFrame->m_subBlockSize.width;
                }

                if (pMBInfo->mbtype == MBTYPE_BACKWARD
                   )
                {
                    struct DecoderPackedMotionVector *pMVb;
                    pMVb = t->m_pBidirMotionVectors + mbnum;

                    pMVb->mvx = mvx;
                    pMVb->mvy = mvy;
                }
                break;

            case MBTYPE_INTER_4V:
                pMV[0].mvx = mvx;
                pMV[0].mvy = mvy;
                pMV[1].mvx = mvx;
                pMV[1].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvy = mvy;
                pMV += 2;
                if (vector == 1)
                    pMV += (t->m_pCurrentFrame->m_subBlockSize.width << 1) - 4;
                break;

            case MBTYPE_INTER_16x8V:
                pMV[0].mvx = mvx;
                pMV[0].mvy = mvy;
                pMV[1].mvx = mvx;
                pMV[1].mvy = mvy;
                pMV[2].mvx = mvx;
                pMV[2].mvy = mvy;
                pMV[3].mvx = mvx;
                pMV[3].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 2].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 2].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 3].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 3].mvy = mvy;
                pMV += (t->m_pCurrentFrame->m_subBlockSize.width << 1);
                break;

            case MBTYPE_INTER_8x16V:
                pMV[0].mvx = mvx;
                pMV[0].mvy = mvy;
                pMV[1].mvx = mvx;
                pMV[1].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width].mvy = mvy;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvx = mvx;
                pMV[t->m_pCurrentFrame->m_subBlockSize.width + 1].mvy = mvy;
                pMV[2*(t->m_pCurrentFrame->m_subBlockSize.width)].mvx = mvx;
                pMV[2*(t->m_pCurrentFrame->m_subBlockSize.width)].mvy = mvy;
                pMV[2*(t->m_pCurrentFrame->m_subBlockSize.width) + 1].mvx = mvx;
                pMV[2*(t->m_pCurrentFrame->m_subBlockSize.width) + 1].mvy = mvy;
                pMV[3*(t->m_pCurrentFrame->m_subBlockSize.width)].mvx = mvx;
                pMV[3*(t->m_pCurrentFrame->m_subBlockSize.width)].mvy = mvy;
                pMV[3*(t->m_pCurrentFrame->m_subBlockSize.width) + 1].mvx = mvx;
                pMV[3*(t->m_pCurrentFrame->m_subBlockSize.width) + 1].mvy = mvy;
                pMV += 2;
                break;

        }
    }

done:


    return status;
}

#if 0//sheen
/*/////////////////////////////////////////////////// */
/* Decode run length and level for inter coefficients */
/*/////////////////////////////////////////////////// */

static void
levrun_inter(U32 length, U32 info, I32 *level, I32 *irun)
{
    U32 infoSizeMinus1, inf;

    RVAssert(length > 1);

    if (length <= 9)
    {
        infoSizeMinus1 = (length >> 1) - 1;
        inf = info >> 1;
        *level = ntab_inter[infoSizeMinus1][inf][0];
        *irun  = ntab_inter[infoSizeMinus1][inf][1];
        if (info & 1)
            *level = -*level; /* sign */
    }
    else
    {
        *irun = (info & 0x1e) >> 1;
        *level = levrun_inter_tab[*irun] + (info >> 5) + (1<<((length >> 1)-5));
        if (info & 1)
            *level = -*level;
    }
}

/*/////////////////////////////////////////////////// */
/* Decode run length and level for intra coefficients */
/*/////////////////////////////////////////////////// */

static void
levrun_intra(U32 length, U32 info, I32 *level, I32 *irun)
{
    U32 infoSizeMinus1, inf;

    RVAssert(length > 1);

    if (length <= 9)
    {
        infoSizeMinus1 = (length >> 1) - 1;
        inf = info >> 1;
        *level = ntab_intra[infoSizeMinus1][inf][0];
        *irun = ntab_intra[infoSizeMinus1][inf][1];
        if (info & 1)
            *level = -*level;
    }
    else
    {
        *irun = (info & 0x0e)>>1;
        *level = levrun_intra_tab[*irun] + (info >> 4) + (1<<((length >> 1)-4))-1;
        if (info & 1)
            *level = -*level;
    }
}
#endif


/* Decode the luma and chroma coefficients for a macroblock, placing them in VOC Buf */
#if 0//hx chg
void Decoder_decodeLumaAndChromaCoefficients_SVLC
(
    struct Decoder *t,
    U32     cbp,
    Bool32  intra,
    Bool32  intra16x16,
    Bool32  inter16x16,
    U32     QP,
    U32     EntropyQP,
    struct CRealVideoBs *pBitstream
)
{

    I16 *buf;
    U32 block, startblock, stopblock, svlc_mbtype;
    U8 qdc, qdc0;

#ifdef DEBUG
    I32 i;
#endif

    /* */
    /* Decode coefficients the SUPER_VLC way. */
    /* */
    vppDataIn->dequant_y_qp = BquantTable[QP];
    vppDataIn->mb_cbp       = cbp;

    if (intra16x16 || inter16x16)
    {
        buf = vppDataIn->vld_cof + 384;
        CB_DecodeSuperSubBlockCoefficients(
            pBitstream,
            buf,
            3, /* DC Intra 16x16 */
            FALSE, /* isChroma */
            EntropyQP
        );

        if (t->m_bIsRV8)
        {
            qdc0 = qdc = ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader)? luma_quant_DC_RV8[QP] : (U8) QP;
        }
        else
        {
            qdc = (U8)QP;
            qdc0 = intra16x16 ? luma_intra_quant_DC[QP]:luma_inter_quant_DC[QP];
        }

        vppDataIn->dequant_y_qdc0 = BquantTable[qdc0];
        vppDataIn->dequant_y_qdc  = BquantTable[qdc];

        svlc_mbtype = 2; /* DC-removed intra block */
    }
    else
        svlc_mbtype = intra ? 1: 0;

    /* decode 4x4 blocks, luma */
    for (block = 0; block < 16; block++)
    {
        buf = vppDataIn->vld_cof + block*16;
        if (cbp & CBP4x4Mask[block])
        {
            CB_DecodeSuperSubBlockCoefficients(
                pBitstream,
                buf,
                svlc_mbtype,
                FALSE /* block >= 16 */,
                EntropyQP
            );
        }
    } /* for block */

    /* */
    /* Now decode the SUPER_VLC chroma coefficients */
    /* */
    vppDataIn->dequant_c_qdc = BquantTable[chroma_quant_DC[QP]];
    vppDataIn->dequant_c_qp  = BquantTable[chroma_quant[QP]];

    startblock = (cbp & 0xf0000 ) ? 16: 20;
    stopblock  = (cbp & 0xf00000) ? 24: 20;
    /* decode 4x4 blocks, chroma */
    for (block = startblock; block < stopblock; block++)
    {
        buf = vppDataIn->vld_cof + block*16;
        if (cbp & CBP4x4Mask[block])
        {
            CB_DecodeSuperSubBlockCoefficients(
                pBitstream,
                buf,
                intra,
                TRUE /*block >=16 */,
                EntropyQP
            );
        }
    }

}   /* decodeLumaCoefficients_SVLC */
#endif

void Decoder_decodeLumaAndChromaCoefficients_SVLC
(
    struct Decoder *t,
    U32     cbp,
    Bool32  intra,
    Bool32  intra16x16,
    Bool32  inter16x16,
    U32     QP,
    U32     EntropyQP,
    struct CRealVideoBs *pBitstream
)
{

    I16 *buf;
    U32 block, startblock, stopblock, svlc_mbtype;
    U8 qdc, qdc0;
    unsigned int bitOffset;

#ifdef DEBUG
    I32 i;
#endif

    /* */
    /* Decode coefficients the SUPER_VLC way. */
    /* */
    vppDataIn->dequant_y_qp = BquantTable[QP];
    vppDataIn->mb_cbp       = cbp;

    decode_4x4_gettab(3, FALSE, EntropyQP);

    if (intra16x16 || inter16x16)
    {
        buf = vppDataIn->vld_cof + 384;
        bitOffset = pBitstream->m_bitOffset;
        decode_4x4_block((short *)buf, &pBitstream->m_pbs, &bitOffset);
        pBitstream->m_bitOffset = bitOffset;

        if (t->m_bIsRV8)
        {
            qdc0 = qdc = ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader)? luma_quant_DC_RV8[QP] : (U8) QP;
        }
        else
        {
            qdc = (U8)QP;
            qdc0 = intra16x16 ? luma_intra_quant_DC[QP]:luma_inter_quant_DC[QP];
        }

        vppDataIn->dequant_y_qdc0 = BquantTable[qdc0];
        vppDataIn->dequant_y_qdc  = BquantTable[qdc];

        svlc_mbtype = 2; /* DC-removed intra block */
    }
    else
        svlc_mbtype = intra ? 1: 0;

    decode_4x4_gettab(svlc_mbtype, FALSE, EntropyQP);

    /* decode 4x4 blocks, luma */
    for (block = 0; block < 16; block++)
    {
        buf = vppDataIn->vld_cof + block*16;
        if (cbp & CBP4x4Mask[block])
        {
            bitOffset = pBitstream->m_bitOffset;
            decode_4x4_block((short *)buf, &pBitstream->m_pbs, &bitOffset);
            pBitstream->m_bitOffset = bitOffset;
        }
    } /* for block */

    /* */
    /* Now decode the SUPER_VLC chroma coefficients */
    /* */
    vppDataIn->dequant_c_qdc = BquantTable[chroma_quant_DC[QP]];
    vppDataIn->dequant_c_qp  = BquantTable[chroma_quant[QP]];

    startblock = (cbp & 0xf0000 ) ? 16: 20;
    stopblock  = (cbp & 0xf00000) ? 24: 20;

    decode_4x4_gettab(intra, TRUE, EntropyQP);

    /* decode 4x4 blocks, chroma */
    for (block = startblock; block < stopblock; block++)
    {
        buf = vppDataIn->vld_cof + block*16;
        if (cbp & CBP4x4Mask[block])
        {
            bitOffset = pBitstream->m_bitOffset;
            decode_4x4_block((short *)buf, &pBitstream->m_pbs, &bitOffset);
            pBitstream->m_bitOffset = bitOffset;
        }
    }

}   /* decodeLumaCoefficients_SVLC */




void
Decoder_resetAllEdgeTypes(struct Decoder *t)
{
    U32 i,j;
    U32 mb_width  = t->m_pCurrentFrame->m_macroBlockSize.width;
    U32 mb_height = t->m_pCurrentFrame->m_macroBlockSize.height;
    struct DecoderMBInfo *pMBInfo = t->m_pMBInfo;

    pMBInfo->edge_type = D_LEFT_EDGE | D_TOP_EDGE;
    pMBInfo++;

    /* First do the top row */
    for (i = 1; i < mb_width; i++, pMBInfo++)
        pMBInfo->edge_type = D_TOP_EDGE;
    pMBInfo[-1].edge_type = D_TOP_EDGE | D_RIGHT_EDGE;

    for (j = 1; j < mb_height; j++)
    {
        for (i = 0; i < mb_width; i++, pMBInfo++)
        {
            U32 edge_type = 0;

            if (0 == i)
                edge_type |= D_LEFT_EDGE;
            if (mb_width - 1 == i)
                edge_type |= D_RIGHT_EDGE;
            if (mb_height - 1 == j)
                edge_type |= D_BOTTOM_EDGE;

            pMBInfo->edge_type = (U8)edge_type;
        }
    }
}

void
Decoder_setMacroBlockEdgeType
(
    struct Decoder *t,
    const U32 mb_addr_start,
    const U32 mb_addr_end,
    const U8 edge_type
)
{
    struct DecoderMBInfo *pMBInfo;
    U32 mba;

    for (mba = mb_addr_start; mba <= mb_addr_end; mba++)
    {
        pMBInfo = &t->m_pMBInfo[mba];
        pMBInfo->edge_type |= edge_type;
    }
}

void
Decoder_setSliceEdgeTypes(struct Decoder *t, const U32 mbAddress)
{

    I32 mb_width  = t->m_pCurrentFrame->m_macroBlockSize.width;
    I32 mb_height = t->m_pCurrentFrame->m_macroBlockSize.height;
    I32 mb_row, mb_start;
    I32 mbs_left_in_row;

    /* Set the first MB in the new slice to be LEFT and TOP */
    Decoder_setMacroBlockEdgeType(t,mbAddress, mbAddress, D_LEFT_EDGE | D_TOP_EDGE);

    /* Set the rest of the MBs in current row to be TOP */
    mb_row = mbAddress / mb_width;
    mbs_left_in_row = (mb_row+1) * mb_width - mbAddress - 1;
    if (mbs_left_in_row)
    {
        mb_start = mbAddress+1;
        Decoder_setMacroBlockEdgeType(t,mb_start, mb_start + mbs_left_in_row-1,
                                      D_TOP_EDGE);
    }

    /* Set the MBs beginning next row to be TOP */
    if (mb_row + 1 < mb_height)
    {
        mb_start = (mb_row+1)*mb_width;
        Decoder_setMacroBlockEdgeType(t,mb_start, mb_start+(mbAddress-mb_row*mb_width)-1,
                                      D_TOP_EDGE);
    }

}

#if 0// sheen
void
Decoder_insertMissingMacroBlocks
(
    struct Decoder *t,
    const U32   mba1,
    const U32   mba2,
    U32  *mbXOffset,
    U32  *mbYOffset,
    struct DecoderPackedMotionVector **pMV
)
{
    U8     *pDst, *pRef;
    U32     mbnum, subblock, offset;
    I32     pitch = t->m_pCurrentFrame->m_pitch;
    struct DecoderPackedMotionVector *pMVTmp;

    struct DecodedFrame* pRefFrame = t->m_pCurrentFrame->m_pPreviousFrame;
    /* Note that pRefFrame is the previously decoded non-B frame, if any. */
    /* If the current frame is an I frame, it obviously would not have */
    /* been predicted from pRefFrame, but pRefFrame is still a useful */
    /* basis for filling in missing information in the current frame. */

    /* update m_pMBInfo, mv's, etc. */
    for (mbnum = mba1; mbnum < mba2; mbnum++)
    {
        /* reset flags and assign skipped MB type */
        t->m_pMBInfo[mbnum].QP = 0; /* TBD, Huh? */
        t->m_pMBInfo[mbnum].missing = 1;
        t->m_pMBInfo[mbnum].mbtype = MBTYPE_SKIPPED;


        /* clear mv's */
        pMVTmp = *pMV;
        for (subblock = 0; subblock < 16; subblock ++)
        {
            pMVTmp->mvx = pMVTmp->mvy = 0;
            pMVTmp++;
            if ((subblock & 3) == 3)
                pMVTmp += t->m_pCurrentFrame->m_subBlockSize.width - 4;
        }

        if (pRefFrame)
        {
            /* copy luma subblocks from reference frame */
            for (subblock = 0; subblock < 16; subblock ++)
            {
                offset =  (*mbYOffset + yoff[subblock]) * pitch
                          + *mbXOffset + xoff[subblock];
                pDst = t->m_pCurrentFrame->m_pYPlane + offset;
                pRef = pRefFrame->m_pYPlane + offset;
                t->m_pCopy4x4_SrcDstPitch(pRef, pitch, pDst);
            }

            /* copy chroma blocks from reference frame */

            for (subblock = 0; subblock < 4; subblock++)
            {
                offset = ((*mbYOffset >> 1) + (yoff8[subblock] >> 1)) * (pitch>>1)
                         + (*mbXOffset >> 1) + (xoff8[subblock] >> 1);//sheen
                pDst = t->m_pCurrentFrame->m_pUPlane + offset;
                pRef = pRefFrame->m_pUPlane + offset;
                t->m_pCopy4x4_SrcDstPitch(pRef, pitch>>1, pDst);//sheen
            }

            for (subblock = 0; subblock < 4; subblock++)
            {
                offset =  ((*mbYOffset >> 1) + (yoff8[subblock] >> 1)) * (pitch>>1)
                          + (*mbXOffset >> 1) + (xoff8[subblock] >> 1);//sheen
                pDst = t->m_pCurrentFrame->m_pVPlane + offset;
                pRef = pRefFrame->m_pVPlane + offset;
                t->m_pCopy4x4_SrcDstPitch(pRef, pitch>>1, pDst);//sheen
            }
        }

        *mbXOffset += 16;
        *pMV += 4;
        if (*mbXOffset == t->m_pCurrentFrame->m_lumaSize.width)
        {
            *mbXOffset  = 0;
            *mbYOffset += 16;
            *pMV += 3 * t->m_pCurrentFrame->m_subBlockSize.width;
        }
    }
}
#endif

/* Set cbpcoef MB info for one MB */
/* The following tables contain the list of luma blocks to check motion */
/* vectors of neighbor blocks, above or to the left, dependent upon the */
/* MB type, saving time by checking the minimal number of blocks. For example */
/* if the MB type is 1V, it is known that within the MB the vectors for all */
/* of the blocks are the same, so there is no need to check all of them. */

/* left check tables */
static const I32 Left1VOnEdgeList[] = {-1};
static const I32 Left1VList[] = {0,4,8,12,-1};
static const I32 Left4VOnEdgeList[] = {2,6,10,14,-1};
static const I32 Left4VList[] = {0,2,4,6,8,10,12,14,-1};
static const I32 Left16VOnEdgeList[] = {1,2,3,5,6,7,9,10,11,13,14,15,-1};
static const I32 All16VList[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1};

/* above check tables */
static const I32 Above1VOnEdgeList[] = {-1};
static const I32 Above1VList[] = {0,1,2,3,-1};
static const I32 Above4VOnEdgeList[] = {8,9,10,11,-1};
static const I32 Above4VList[] = {0,1,2,3,8,9,10,11,-1};
static const I32 Above16VOnEdgeList[] = {4,5,6,7,8,9,10,11,12,13,14,15,-1};

/* Versions for 8x8i interlaced MBtype */
static const I32 Left2VOnEdgeList[] = {2,6,-1};
static const I32 Left2VList[] = {0,2,4,6,-1};
static const I32 Above2VOnEdgeList[] = {-1};
static const I32 Above2VList[] = {0,1,2,3,-1};

/* All 2MV Interlaced Mbtypes use 1VI cases */
static const I32 Left1VIOnEdgeList[] = {-1};
static const I32 Left1VIList[] = {0,4,-1};
static const I32 Above1VIOnEdgeList[] = {-1};
static const I32 Above1VIList[] = {0,1,2,3,-1};

/* 1MV interlaced MBTypes use 1V cases */
U32 SpLoc[16] = {0, 1, 2 , 3, 8, 9, 10, 11, 0, 1, 2, 3, 8, 9 , 10 , 11};

void
Decoder_setDeblockCoefs(struct Decoder *t,
                        struct DecoderMBInfo  *pMB, struct DecoderPackedMotionVector *pMV,
                        U32 uDirectBRefMBType)
{
    U32 uMBType;
    const I32 *pLeftBlockList;
    const I32 *pAboveBlockList;
    I32 i;
    struct DecoderPackedMotionVector* pBlock0mv ;
    //const I32 iThreshold = 3;     /* signal filter on when motion vector */
    I32 b,dx,dy;
#ifdef INTERLACED_CODE
    RV_Boolean bIsInterlaced = FALSE;
#endif

#ifdef REMOVE_INLOOP_ASSUMPTION_NO_MVDs_IN_KEYFRAME
    if (!t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough)
#else
    if (!ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader) &&
            !t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough)
#endif
    {
        /* set deblock flags for neighboring blocks with mv differences */
        /* greater than 3 subpels */
        const U32 width_b = t->m_pCurrentFrame->m_subBlockSize.width;
        //const U32 width_mb = t->m_pCurrentFrame->m_macroBlockSize.width;

        struct DecoderPackedMotionVector* mv;

        if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader) ||
                (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader) &&
                 pMB->mbtype != MBTYPE_SKIPPED &&
                 pMB->mbtype != MBTYPE_DIRECT))
        {
            /* (P frame) || (B frame and not skipped and not direct) */
            /* in this case, look at current motion vector differences */
            mv = pMV;
            uMBType = pMB->mbtype;
        }
        else
        {
            /* B frame and skipped or direct mode. */
            /* Now we look at the motion vector differences for the */
            /* frame from which they are scaled          */
            mv = (pMV - t->m_pMotionVectors) + t->m_pReferenceMotionVectors;
            uMBType = uDirectBRefMBType;
        }
#ifdef INTERLACED_CODE
        bIsInterlaced = IS_INTERLACE_MBTYPE(uMBType);
#endif
        pBlock0mv = mv;

        if (MBTYPE_INTER_4V == uMBType
                || MBTYPE_INTER_16x8V == uMBType
                || MBTYPE_INTER_8x16V == uMBType
           )
        {
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left4VList;
            else
                pLeftBlockList = Left4VOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above4VList;
            else
                pAboveBlockList = Above4VOnEdgeList;
#ifdef INTERLACED_CODE
            bIsInterlaced = FALSE;
#endif
        }
#ifdef INTERLACED_CODE
        else if(MBTYPE_INTER_8x8i == uMBType)
        {
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left2VList;
            else
                pLeftBlockList = Left2VOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above2VList;
            else
                pAboveBlockList = Above2VOnEdgeList;
        }
        else if(MBTYPE_INTER_16x8i == uMBType
                || MBTYPE_FORWARD_16x8i == uMBType
                || MBTYPE_BACKWARD_16x8i == uMBType
               )
        {
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left1VIList;
            else
                pLeftBlockList = Left1VIOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above1VIList;
            else
                pAboveBlockList = Above1VIOnEdgeList;
        }
#endif
        else
        {
            /* 1V type MB, all blocks have same MV */
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left1VList;
            else
                pLeftBlockList = Left1VOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above1VList;
            else
                pAboveBlockList = Above1VOnEdgeList;
#ifdef INTERLACED_CODE
            bIsInterlaced = FALSE;
#endif
        }

        /* left */
        i = 0;
        b = pLeftBlockList[i];
        while (b >= 0)
        {
#ifdef INTERLACED_CODE
            if(bIsInterlaced)
            {
                mv = pBlock0mv + (b & 3) + (b>>2)*width_b;

                dx = mv->mvx - (mv-1)->mvx;
                dy = mv->mvy - (mv-1)->mvy;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                {
                    pMB->mvdelta |= CBP4x4Mask[SpLoc[b]];
                    pMB->mvdelta |= CBP4x4Mask[SpLoc[b]+4];
                }
            }
            else
#endif
            {
                mv = pBlock0mv + (b & 3) + (b>>2)*width_b;

                dx = mv->mvx - (mv-1)->mvx;
                dy = mv->mvy - (mv-1)->mvy;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                {
                    pMB->mvdelta |= CBP4x4Mask[b];
                }
            }
            i++;
            b = pLeftBlockList[i];
        }

        /* above */
        i = 0;
        b = pAboveBlockList[i];
        while (b >= 0)
        {
#ifdef INTERLACED_CODE
            if(bIsInterlaced)
            {
                mv = pBlock0mv + (b & 3) + (b>>2)*width_b;
                dx = mv->mvx - (mv-width_b)->mvx;
                dy = mv->mvy - (mv-width_b)->mvy;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                {
                    pMB->mvdelta |= CBP4x4Mask[SpLoc[b]];
                    pMB->mvdelta |= CBP4x4Mask[SpLoc[b]+4];
                }
            }
            else
#endif
            {
                mv = pBlock0mv + (b & 3) + (b>>2)*width_b;
                dx = mv->mvx - (mv-width_b)->mvx;
                dy = mv->mvy - (mv-width_b)->mvy;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                {
                    pMB->mvdelta |= CBP4x4Mask[b];
                }
            }
            i++;
            b = pAboveBlockList[i];
        }
    }
}   /* setDeblockCoefs */


void
Decoder_setDeblockCoefs_RV8(struct Decoder *t,
                            struct DecoderMBInfo  *pMB,
                            struct DecoderPackedMotionVector *pMV,
                            U32 uDirectBRefMBType
                           )
{
    U32 uMBType;
    U32 active_blocks;
    const I32 *pLeftBlockList;
    const I32 *pAboveBlockList;
    I32 i,b,dx,dy;
    struct DecoderPackedMotionVector* pBlock0mv;
    //const I32 iThreshold = 3;     /* signal filter on when motion vector */
    /* difference exceeds this threshold. */

#ifdef REMOVE_INLOOP_ASSUMPTION
    if (!t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough)
#else
    if (!ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader) &&
            !t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough)
#endif
    {
        /* set deblock flags for neighboring blocks with mv differences */
        /* greater than 3 subpels */
        const U32 width_b = t->m_pCurrentFrame->m_subBlockSize.width;
        const U32 width_mb = t->m_pCurrentFrame->m_macroBlockSize.width;

        struct DecoderPackedMotionVector* mv;

        if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader) ||
                (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader) &&
                 pMB->mbtype != MBTYPE_SKIPPED &&
                 pMB->mbtype != MBTYPE_DIRECT))
        {
            /* (P frame) || (B frame and not skipped and not direct) */
            /* in this case, look at current motion vector differences */
            mv = pMV;
            uMBType = pMB->mbtype;
        }
        else
        {
            /* B frame and skipped or direct mode. */
            /* Now we look at the motion vector differences for the */
            /* frame from which they are scaled */
            mv = (pMV - t->m_pMotionVectors) + t->m_pReferenceMotionVectors;
            uMBType = uDirectBRefMBType;
        }


        active_blocks = 0;


        pBlock0mv = mv;

        if (MBTYPE_INTER_4V == uMBType)
        {
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left4VList;
            else
                pLeftBlockList = Left4VOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above4VList;
            else
                pAboveBlockList = Above4VOnEdgeList;
        }
        else
        {
            /* 1V type MB, all blocks have same MV */
            if (!(pMB->edge_type & D_LEFT_EDGE))
                pLeftBlockList = Left1VList;
            else
                pLeftBlockList = Left1VOnEdgeList;

            if (!(pMB->edge_type & D_TOP_EDGE))
                pAboveBlockList = Above1VList;
            else
                pAboveBlockList = Above1VOnEdgeList;
        }

        /* left */
        i = 0;
        b = pLeftBlockList[i];
        while (b >= 0)
        {
            mv = pBlock0mv + (b & 3) + (b>>2)*width_b;

            dx = mv->mvx - (mv-1)->mvx;
            dy = mv->mvy - (mv-1)->mvy;
            if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
            {
                active_blocks++;
                pMB->cbpcoef |= CBP4x4Mask[b];
                if (b & 3)
                    pMB->cbpcoef |= CBP4x4Mask[b - 1];
                else
                    (pMB-1)->cbpcoef |= CBP4x4Mask[b + 3];
            }

            i++;
            b = pLeftBlockList[i];
        }

        /* above */
        i = 0;
        b = pAboveBlockList[i];
        while (b >= 0)
        {
            mv = pBlock0mv + (b & 3) + (b>>2)*width_b;
            dx = mv->mvx - (mv-width_b)->mvx;
            dy = mv->mvy - (mv-width_b)->mvy;
            if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
            {
                active_blocks++;

                pMB->cbpcoef |= CBP4x4Mask[b];
                if (b > 3)
                    pMB->cbpcoef |= CBP4x4Mask[b - 4];
                else
                    (pMB-width_mb)->cbpcoef |= CBP4x4Mask[b + 12];
            }

            i++;
            b = pAboveBlockList[i];
        }

        if (active_blocks > 4)
        {
            t->m_num_active_mbs++;
        }

    }
}   /* setDeblockCoefs_RV8 */

#if 0//hx chg

/* MB copy functions */
static void CopyMBLuma(U8 *pDst, const U8 *pRef, U32 pitch)
{
    /* Just copy the reference plane data. */
    /* For performance, copy as a series of U32's. */
    /* This is safe because we are guaranteed that */
    /* the data is 4-byte aligned. */

    U32        *pDst32;
    const U32  *pRef32;
    U32         dstRow, pitch32;

    pitch32 = pitch >> 2;
    pDst32 = (U32*)pDst;
    pRef32 = (const U32*)pRef;

    for (dstRow = 0; dstRow < 16; dstRow++)
    {
        pDst32[0] = pRef32[0];
        pDst32[1] = pRef32[1];
        pDst32[2] = pRef32[2];
        pDst32[3] = pRef32[3];

        pDst32 += pitch32;
        pRef32 += pitch32;
    }
}   /* CopyMBLuma */

static void CopyMBChroma(U8 *pDst, const U8 *pRef, U32 pitch)
{
    U32        *pDst32;
    const U32  *pRef32;
    U32         dstRow, pitch32;

    pitch32 = pitch >> 2;
    pDst32 = (U32*)pDst;
    pRef32 = (const U32*)pRef;

    for (dstRow = 0; dstRow < 8; dstRow++)
    {
        pDst32[0] = pRef32[0];
        pDst32[1] = pRef32[1];

        pDst32 += pitch32;
        pRef32 += pitch32;
    }

}   /* CopyMBChroma */
#endif

void
Decoder_startNextFrame(struct Decoder *t)
{
    RVAssert(sizeof(t->m_invariantPictureHeader) == sizeof(struct PictureHeader));
    RVAssert(sizeof(t->m_pCurrentFrame->m_pictureHeader) == sizeof(struct PictureHeader));


    /*if (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader)) */
    if (t->m_pCurrentFrame->m_pictureHeader.PicCodType == INTERPIC)
    {
        DecodedFrame_expand(t->m_pCurrentFrame->m_pPreviousFrame);
    }
    else if (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        /* previous() has already been expanded, when decoding future(). */
        /* But in case there was some kind of error, lets make sure. */
        DecodedFrame_expand(t->m_pCurrentFrame->m_pPreviousFrame);
        DecodedFrame_expand(t->m_pCurrentFrame->m_pFutureFrame);

    }

    /* Since slicing will generally move the edge types around for each frame */
    /* they need to be reset before decoding the next frame. */
    Decoder_resetAllEdgeTypes(t);

#if defined(MULTI_THREADED_DECODER)
    {
        U32 i;
        t->m_bDataIsLost = FALSE;

#if defined (MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS)
        t->m_bIsWaiting[0] = FALSE;
        t->m_bIsWaiting[1] = FALSE;
#endif

        for (i = 0; i < 128; i++)
        {
            t->m_iSliceMBA[i] = -1;
            t->m_uLastGoodMBNum[i] = 0;
        }
    }
#endif
}


void vpp_RMVBDecScheduleOneBlk(VPP_RMVB_CFG_T *dec_in_ptr)
{
    VPP_RMVB_CFG_T *dec_in_ptr_voc = (VPP_RMVB_CFG_T*) hal_VocGetPointer(VPP_RMVB_CFG_STRUCT);

    *dec_in_ptr_voc=*dec_in_ptr;
    //WAKE UP VOC
#ifndef SHEEN_VC_DEBUG

    hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

#else
    VoC_set_event(CFG_WAKEUP_SOF0);

#ifdef BINARY_SIM
    VoC_execute(48);
#else
    vpp_RMVBDECMAIN();
#endif

#endif

}

extern void (*fillAudioData)();//read audio data
extern uint16 MBScale4AudRed;
extern BYTE needBackUpFileOffset;

//static INT32 rm_time1,rm_time2,rm_total1,rm_total2;//for test .sheen

RV_Status Decoder_parseBitStream(struct Decoder *t, I32 iSliceNum, I32 iThread)
{
    RV_Status       status = RV_S_OK;
    /* decoding status, if !RV_S_OK, can not continue, return error */
    RV_Status       dec_status = RV_S_OK;
    /* bitstream error status, if !RV_S_OK, find next slice */
    //VOLATILE U16            Data16;
    I32             i;
    U32             mbnum=0, cbp;
    U8              QP, mbtype, quant_prev;
    U32             uNumMissingMBs;
    struct DecoderMBInfo  *pMB;
    RV_Boolean     bFirstSlice;
    RV_Boolean     intra;
    RV_Boolean     intra16x16;
    RV_Boolean     inter16x16;
    RV_Boolean      bExitAfterRecon = FALSE;
    RV_Boolean      found_ssc;

    struct CRealVideoBs     *pBitstream;
    DecoderIntraType *pAboveSubBlockIntraTypes, *pTmp;
    DecoderIntraType *pMBIntraTypes;
    U8      *pTmp8;
    U16     *pTmp16;
    U32     *pTmp32;
    U32     Data_tmp32;
    U32     ulSkipModesLeft = 0;
    U8      *pRefYPlane, *pRefUPlane, *pRefVPlane;
    I32     iSliceMBA, iSliceGFID, iSQUANT;
    /*PictureHeader   pictureHeader; */
    I32     iRatio0=0;
    I32     iRatio1=0;
    I32         trb, trd;
    U8 EntropyQP;
    struct DecoderPackedMotionVector *pMV;
    struct DecoderPackedMotionVector *pMVR;

    RV_Boolean bnewSlice = FALSE;
    U32 mbXOffset, mbYOffset, totalMBs;
    U32 bsBitOffset;
    struct DecoderPackedMotionVector *pMVTmp;
    U32 cbptype;

    INT16  mb_exceed;
    //INT8  mb_voc_rq;
    uint16 fillAudMbScale=MBScale4AudRed;//big than qcif

    /* Frame type to local vars for better performance in MB loops */
    RV_Boolean bCurrentFrameIsPFrame = ISPFRAME(t->m_pCurrentFrame->m_pictureHeader);
    RV_Boolean bCurrentFrameIsBFrame = ISBFRAME(t->m_pCurrentFrame->m_pictureHeader);
    RV_Boolean bCurrentFrameIsKeyFrame = ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader);

    //rm_printf("PicCodType=%d\n", t->m_pCurrentFrame->m_pictureHeader.PicCodType);

    bFirstSlice = TRUE;
    t->m_num_active_mbs = 0;
    uNumMissingMBs = 0;

    pBitstream = t->m_pBitStream;
    t->m_inLoopDeblockingFilter.m_pEdgeFilter4H = NULL;//C_EdgeFilter4H; voc sheen.
    t->m_inLoopDeblockingFilter.m_pEdgeFilter4V = NULL;//C_EdgeFilter4V; voc sheen.



    pAboveSubBlockIntraTypes    = t->m_pAboveSubBlockIntraTypes[iThread];

    if (bCurrentFrameIsPFrame)
    {
        pRefYPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pYPlane;
        pRefUPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pUPlane;
        pRefVPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pVPlane;
    }
    else if (bCurrentFrameIsBFrame)
    {
        /* Reference planes need to be initialized to either previous */
        /* or future as each macroblock is processed. */
        pRefYPlane = pRefUPlane = pRefVPlane = 0;

        /* Compute ratios needed for direct mode */

        trb = (I32) t->m_pCurrentFrame->m_pictureHeader.TR - (I32) t->m_pCurrentFrame->m_pPreviousFrame->m_pictureHeader.TR;
        trd = (I32) t->m_pCurrentFrame->m_pFutureFrame->m_pictureHeader.TR - (I32) t->m_pCurrentFrame->m_pPreviousFrame->m_pictureHeader.TR;

        if (trb<0)
            trb += (I32) t->m_uTRWrap;

        if (trd<0)
            trd += (I32) t->m_uTRWrap;

        if (trb > trd)      /* just in case TR's are incorrect */
            trb=0;

        if (trd>0)
        {
            iRatio0 = (trb << TR_SHIFT) / trd;
            iRatio1 = ((trd-trb) << TR_SHIFT) / trd;
        }
        else
        {
            iRatio0= iRatio1 = 0;
        }
    }
    else if (bCurrentFrameIsKeyFrame)
    {
        /* Reference planes are not used, set to 0 for safety. */
        pRefYPlane = pRefUPlane = pRefVPlane = 0;
    }
    else
    {
        RVDebug((RV_TL_ALWAYS,"ERROR: parseBitStream -- "
                 "Unknown picture coding type"));
        return RV_S_UNSUPPORTED;
    }

    /* Set the workspace data members according to what was found in the */
    /* picture header. */
    QP = t->m_pCurrentFrame->m_pictureHeader.PQUANT;
    quant_prev = QP;
    EntropyQP = QP;

    if (!t->m_bIsRV8)
    {
        switch (t->m_pCurrentFrame->m_pictureHeader.OSVQUANT)
        {
            case 0:
                break;
            case 1:
                if (QP <= 25)
                {   EntropyQP = MIN(30,QP+5); }
                break;
            case 2:
                if (QP <= 18)
                {   EntropyQP = MIN(30,QP+10);}
                else if (QP <= 25)
                {   EntropyQP = MIN(30,QP+5); }
                break;
            default:
                RVAssert("Illegal OSVQUANT" == 0);
                break;
        }
    }

    mbXOffset = 0;
    mbYOffset = 0;

    pMV = t->m_pMotionVectors;
    /* MV store for subblock 0 of the current macroblock. */
    totalMBs = t->m_pCurrentFrame->m_macroBlockSize.width * t->m_pCurrentFrame->m_macroBlockSize.height;

#ifndef SHEEN_VC_DEBUG

//rm_time1=hal_TimGetUpTime();
    //check voc state.sheen

    //move wait before Decoder_startNextFrame.
    /*
    i=0;
    while(*(pRmVocWorkState->pVOC_VID)!=0)
    {
        i++;
        if(i>3000)
        {
            rm_printf("[RM] wait over at decode start.");
            return RV_S_ERROR;//about 200ms
        }
        sxr_Sleep(1);
    }*/

    //*(pRmVocWorkState->pVOC_WORK)=2;
    //vpp_AudioJpegDecSetCurMode(-1, -1);
    //vpp_AudioJpegDecSetInputMode(MMC_RM_DECODE);
    vpp_AVDecSetMode(MMC_RM_DECODE, 0, 0);
    //hal_VocIrqMaskClr(0xF);//clear irq.
//rm_time2=hal_TimGetUpTime();
//rm_printf("voc wait time=%d \n", (rm_time2-rm_time1)*1000/16384);

#endif
    //INITIAL ID
    vpp_buffer_rd_id=0;
    vpp_buffer_wt_id=0;
    vpp_data_buffer[0].done=0;
    vpp_data_buffer[1].done=0;
    mb_exceed   =0;
#ifndef SHEEN_VC_DEBUG
    {
        UINT32 cs_status=hal_SysEnterCriticalSection();
        *(vocWorkState.pVOC_VID)=0;
        hal_SysExitCriticalSection(cs_status);
    }
#endif

    //INTIAL RMVB STRUCT
    vpp_rmvb_cfg.reset      =   0;
    vpp_rmvb_cfg.status     =   VOC_PROCESS_RM;
    vpp_rmvb_cfg.mb_num     =   RM_PARA_BUF_NUM;
    vpp_rmvb_cfg.rv8_flag   =   t->m_bIsRV8;

    vpp_rmvb_cfg.pic_width  =   (INT16)(t->m_pCurrentFrame->m_lumaSize.width);
    vpp_rmvb_cfg.pic_height=    (INT16)(t->m_pCurrentFrame->m_lumaSize.height);
    vpp_rmvb_cfg.pic_pitch  =   (INT16)(t->m_pCurrentFrame->m_pitch);
    vpp_rmvb_cfg.contex_buf_ptr=    ((INT32)vpp_contex_buffer) & VOC_CFG_DMA_EADDR_MASK;

#if defined(LOOP_FILTER_EN)
    if (bCurrentFrameIsBFrame)
        vpp_rmvb_cfg.filter_pass =   1;
    else
        vpp_rmvb_cfg.filter_pass =   t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough;
#else
    vpp_rmvb_cfg.filter_pass =   1;
#endif

#ifndef SHEEN_VC_DEBUG


    //rm_printf("[VID] start RM MB decoding \n");
    //rm_total1=0;
    //rm_total2=0;
#endif

    //vppCfgIn = (VPP_RMVB_CFG_T*) hal_VocGetPointer(VPP_RMVB_CFG_STRUCT);
    //vppCfgIn->status  =   VOC_PROCESS_NON;

    for (mbnum = 0; mbnum < totalMBs && status == RV_S_OK; mbnum++)
    {
#ifndef SHEEN_VC_DEBUG
//rm_time1=hal_TimGetUpTime();
#endif

#if !defined(ARM) && !defined(_ARM_)
#if defined(RELIEF_CALLBACK)
#define CALLBACKINTERVAL 0xff
        /* call the relief callback function at appropriate intervals */
        /* (for now, every CALLBACKINTERVAL macroblocks */
        if ((mbnum & CALLBACKINTERVAL) == CALLBACKINTERVAL)
        {
            t->m_pReliefCallback ? t->m_pReliefCallback->Func() : 0;
        }
#endif  /*  defined(RELIEF_CALLBACK) */
#endif  /*  !defined(ARM) && !defined(_ARM_) */

        RVAssert(mbXOffset < t->m_pCurrentFrame->m_lumaSize.width);

        pMBIntraTypes               = t->m_pMBIntraTypes[iThread];

        /* check if current position resides within a valid segment */
        if (!CB_IsSegmentValid(pBitstream))
        {
            BSLR_Status lr_status;

            lr_status = CB_LossRecovery(pBitstream,TRUE);

            if (lr_status == BSLR_STATUS_ERROR)
            {
                /* unrecoverable error, quit */
                RVDebug((RV_TL_ALWAYS, "ERROR parseBitStream: Bad "
                         "bitstream"));
                status = RV_S_ERROR;
                break;
            }
            else if (lr_status == BSLR_STATUS_EOS)
            {
                if (ulSkipModesLeft > 1)
                {
                    /* If we are at the end of the bitstream for */
                    /* this frame but there are still MBs left in */
                    /* "skip" mode, skip over the slice boundary */
                    /* check and continue decoding MBs. */
                    goto skip_slice_check;
                }
                else
                {
                    /* stop if reached end of stream, */
                    /* reconstruct those MBs that were read from */
                    /* the bitstream, then stop decoding */
                    bExitAfterRecon = TRUE;
                    break;
                }
            }
        }

        if (ulSkipModesLeft > 1 && !bFirstSlice)
        {
            /* If there are skipped macroblocks at the end of a slice */
            /* left to be decoded, skip over the slice boundary */
            /* check and continue decoding MBs. */
            goto skip_slice_check;
        }

        if (!bnewSlice)
        {
            if (t->m_bIsRV8)
            {
                bnewSlice = CB_GetSSC_RV8(pBitstream) || bFirstSlice;
            }
            else
            {
                bnewSlice = CB_GetSSC(pBitstream) || bFirstSlice;
            }
        }

        if (bnewSlice)
        {
            bnewSlice = FALSE;
            bsBitOffset = CB_GetBsOffset(pBitstream);

            if (t->m_bIsRV8)
                found_ssc = CB_GetSliceHeader_RV8(pBitstream,bFirstSlice,
                                                  &iSliceMBA, &iSliceGFID, &iSQUANT,
                                                  &t->m_pCurrentFrame->m_pictureHeader);
            else
                found_ssc = CB_GetSliceHeader(pBitstream,bFirstSlice,
                                              &iSliceMBA, &iSliceGFID, &iSQUANT,
                                              &t->m_pCurrentFrame->m_pictureHeader);


            RVDebug((RV_TL_MBLK, "Slice header size: %3d bits ",
                     CB_GetBsOffset(pBitstream) - bsBitOffset));

            ulSkipModesLeft = 0;

            mbnum = (U32)iSliceMBA;
            mbYOffset = mbnum / t->m_pCurrentFrame->m_macroBlockSize.width;
            mbXOffset = mbnum % t->m_pCurrentFrame->m_macroBlockSize.width;
            pMV = t->m_pMotionVectors + mbXOffset * 4 +
                  mbYOffset * 4 * t->m_pCurrentFrame->m_subBlockSize.width;
            mbYOffset *= 16;
            mbXOffset *= 16;

            Decoder_setSliceEdgeTypes(t,iSliceMBA);

            bFirstSlice = FALSE;
            if (iSQUANT >= 0)
                QP = quant_prev = (U8)iSQUANT;

            EntropyQP = QP;

            if (!t->m_bIsRV8)
            {
                switch (t->m_pCurrentFrame->m_pictureHeader.OSVQUANT)
                {
                    case 0:
                        break;
                    case 1:
                        if (QP <= 25)
                        { EntropyQP = MIN(30,QP+5);}
                        break;
                    case 2:
                        if (QP <= 18)
                        { EntropyQP = MIN(30,QP+10);}
                        else if (QP <= 25)
                        { EntropyQP = MIN(30,QP+5);}
                        break;
                    default:
                        RVAssert("Illegal OSVQUANT" == 0);
                        break;
                }
            }
        }

skip_slice_check:

        pMB = &t->m_pMBInfo[mbnum];
        pMB->missing = 0;
#ifdef SHEEN_VC_DEBUG
        vppDataIn = (VPP_RMVB_DATA_T *)(&(vpp_data_buffer[vpp_buffer_wt_id].buffer[mb_exceed]));
#else
        vppDataIn = (VPP_RMVB_DATA_T *)((U32)(&(vpp_data_buffer[vpp_buffer_wt_id].buffer[mb_exceed]))|0x20000000);
#endif
        mb_exceed++;

#ifdef SHEEN_VC_DEBUG
        pTmp16 = (U16 *)vppDataIn;
#endif

        /* First decode the "macroblock header", e.g., macroblock type, */
        /* intra-coding types, motion vectors and CBP. */
        if (bCurrentFrameIsKeyFrame)
        {
            intra = 1;

            if (t->m_bIsRV8)
            {
                pMB->mbtype = mbtype = CB_Get1Bit(pBitstream) ? MBTYPE_INTRA_16x16 :
                                       MBTYPE_INTRA;
            }
            else
            {
                if (CB_Get1Bit(pBitstream))
                {
                    pMB->mbtype = mbtype = MBTYPE_INTRA_16x16;
                }
                else
                {
                    if (CB_Get1Bit(pBitstream))
                    {
                        pMB->mbtype = mbtype = MBTYPE_INTRA;
                    }
                    else
                    {
                        /* Get DQUANT */
#if defined(QUANTIZER_CHANGE_ALLOWED)
                        quant_prev = QP;
                        CB_GetDQUANT(pBitstream,&(QP),quant_prev,TRUE);
                        EntropyQP = QP;
                        if (!t->m_bIsRV8)
                        {
                            switch (t->m_pCurrentFrame->m_pictureHeader.OSVQUANT)
                            {
                                case 0: break;
                                case 1:
                                    if (QP <= 25)
                                    { EntropyQP = MIN(30,QP+5); }
                                    break;
                                case 2:
                                    if (QP <= 18)
                                    { EntropyQP = MIN(30,QP+10);}
                                    else if (QP <= 25)
                                    { EntropyQP = MIN(30,QP+5); }
                                    break;
                                default:
                                    RVAssert("Illegal OSVQUANT" == 0);
                                    break;
                            }
                        }
#else
                        quant_prev = pMB->QP;
                        CB_GetDQUANT(pBitstream,&(pMB->QP),quant_prev,TRUE);
#endif

                        pMB->mbtype = mbtype =
                                          CB_Get1Bit(pBitstream) ? MBTYPE_INTRA_16x16 :
                                          MBTYPE_INTRA;
                    }
                }
            }

            intra16x16 = (RV_Boolean)(mbtype == MBTYPE_INTRA_16x16);
            inter16x16 = FALSE;

            vppDataIn->intra16_16   = intra16x16;
            vppDataIn->intra        = 1;
            vppDataIn->inter16      = 0;
            vppDataIn->inter16_16   = 0;

            RVDebug((RV_TL_MBLK,"mbtype %d", mbtype));

            pMVTmp = pMV;

            /* TBD, do we need to spend CPU time initializing motion */
            /* vectors in a key frame?  Does anyone depend on these values? */
            for (i = 0; i < 4; i++)         /* zero out stored motion vectors. */
            {
                pMVTmp[0].mvx = pMVTmp[0].mvy = 0;
                pMVTmp[1].mvx = pMVTmp[1].mvy = 0;
                pMVTmp[2].mvx = pMVTmp[2].mvy = 0;
                pMVTmp[3].mvx = pMVTmp[3].mvy = 0;

                pMVTmp += t->m_pCurrentFrame->m_subBlockSize.width;
            }

            if (t->m_bIsRV8)
                dec_status = Decoder_decodeIntraTypes_RV8(t,pMB,
                             pAboveSubBlockIntraTypes + (mbXOffset >> 2),
                             pMBIntraTypes,
                             pBitstream);
            else
                dec_status = Decoder_decodeIntraTypes(t,pMB,
                                                      pAboveSubBlockIntraTypes + (mbXOffset >> 2),
                                                      pMBIntraTypes,
                                                      pBitstream);

            if (dec_status != RV_S_OK)
            {
                status = dec_status;
                break;
            }

            cbptype = intra16x16 ? 2:1;
            /* TBD, 0, 1 and 2 are magic numbers for cbptype */
            cbp = CB_GetSuperCBP(pBitstream, cbptype, EntropyQP);
            RVDebug((RV_TL_BLK, "cbp: %2x", cbp));

            pMB->QP = QP;
        }
        else if (bCurrentFrameIsPFrame || bCurrentFrameIsBFrame)
        {
            if (t->m_bIsRV8)
            {
                dec_status = Decoder_decodeMacroBlockType_RV8(t,&mbtype, &QP, quant_prev, pBitstream);
                pMB->QP = QP;
                pMB->mbtype = mbtype;
            }
            else
            {
                dec_status = Decoder_decodeMacroBlockType(t,pMB, quant_prev, pBitstream, &ulSkipModesLeft);

#if defined(QUANTIZER_CHANGE_ALLOWED)
                QP = pMB->QP;
                if(quant_prev != QP)
                {
                    EntropyQP = pMB->QP;
                    switch (t->m_pCurrentFrame->m_pictureHeader.OSVQUANT)
                    {
                        case 1:
                            if (QP <= 25) EntropyQP = MIN(30,QP+5);
                            break;
                        case 2:
                            if (QP <= 18) EntropyQP = MIN(30,QP+10);
                            else if (QP <= 25) EntropyQP = MIN(30,QP+5);
                            break;
                        default:
                            RVAssert("Illegal OSVQUANT" == 0);
                        case 0:
                            break;
                    }
                }
                quant_prev = QP;
#else
                quant_prev = pMB->QP;
#endif

                mbtype = pMB->mbtype;
            }

            if (dec_status != RV_S_OK)
            {
                status = dec_status;
                break;
            }

            intra = (RV_Boolean)(IS_INTRA_MBTYPE(mbtype));
            intra16x16 = (RV_Boolean)(mbtype == MBTYPE_INTRA_16x16);
            inter16x16 = (RV_Boolean)(mbtype == MBTYPE_INTER_16x16);

            vppDataIn->intra16_16   = intra16x16;
            vppDataIn->intra        = intra;
            vppDataIn->inter16      = (mbtype == MBTYPE_INTER);
            vppDataIn->inter16_16   = inter16x16;

            if(mbtype == MBTYPE_INTER || mbtype == MBTYPE_FORWARD || mbtype == MBTYPE_BACKWARD
                    || mbtype == MBTYPE_BIDIR || mbtype == MBTYPE_SKIPPED || mbtype == MBTYPE_DIRECT)
            {
                vppDataIn->inter16      = 1;
            }
            else
                vppDataIn->inter16      = 0;

            if (mbtype == MBTYPE_SKIPPED)
            {
#if 0//hx chg
                I32 pmvx, pmvy;

                /* skipped - copy data from previous frame */

                RVDebug((RV_TL_MBLK,"Skipped"));

                /* To treat m_pMBIntraTypes as a (U32*), it must be */
                /* 4-byte aligned. */
                RVAssert(sizeof(pMBIntraTypes[0]) == 1
                         && !((U32)pMBIntraTypes & 3));
#endif
                pMVTmp = pMV;

                //pmvx = pmvy = 0; hx chg

                for (i = 0; i < 4; i++)
                {
                    ((U32*)pMBIntraTypes)[i] = 0; /* this clears 4 elements */
#if 0//hx chg                   
                    pMVTmp[0].mvx = (I16)pmvx; pMVTmp[0].mvy = (I16)pmvy;
                    pMVTmp[1].mvx = (I16)pmvx; pMVTmp[1].mvy = (I16)pmvy;
                    pMVTmp[2].mvx = (I16)pmvx; pMVTmp[2].mvy = (I16)pmvy;
                    pMVTmp[3].mvx = (I16)pmvx; pMVTmp[3].mvy = (I16)pmvy;
#endif
                    pMVTmp[0].mvx = 0; pMVTmp[0].mvy = 0;
                    pMVTmp[1].mvx = 0; pMVTmp[1].mvy = 0;
                    pMVTmp[2].mvx = 0; pMVTmp[2].mvy = 0;
                    pMVTmp[3].mvx = 0; pMVTmp[3].mvy = 0;

                    pMVTmp += t->m_pCurrentFrame->m_subBlockSize.width;
                }

                cbp = 0;
            }
            else if (intra)
            {
                if (t->m_bIsRV8)
                    dec_status = Decoder_decodeIntraTypes_RV8(t,pMB,
                                 pAboveSubBlockIntraTypes + (mbXOffset >> 2),
                                 pMBIntraTypes,
                                 pBitstream);
                else
                    dec_status = Decoder_decodeIntraTypes(t,pMB,
                                                          pAboveSubBlockIntraTypes + (mbXOffset >> 2),
                                                          pMBIntraTypes,
                                                          pBitstream);

                if (dec_status != RV_S_OK)
                {
                    status = dec_status;
                    break;
                }

                cbptype = (mbtype == MBTYPE_INTRA);
                cbptype = intra16x16 ? 2:cbptype;
                cbp = CB_GetSuperCBP(pBitstream, cbptype, EntropyQP);
                RVDebug((RV_TL_BLK, "cbp: %2x", cbp));

                /* zero out motion vectors */
                /* Technically, we only need to zero out motion vectors */
                /* for the bottom row of subblocks, since these may be */
                /* used as predictors for the next row of macroblocks. */
                /* Other potential users of the motion vectors should */
                /* first be checking the mbtype, and not use them for */
                /* intra coded macroblocks. */

                pMVTmp = pMV;
                for (i = 0; i < 4; i++)
                {
#if 0//hx chg           
                    for (j = 0; j < 4; j++)
                        pMVTmp[j].mvx = pMVTmp[j].mvy = 0;
#endif
                    pMVTmp[0].mvx = pMVTmp[0].mvy = 0;
                    pMVTmp[1].mvx = pMVTmp[1].mvy = 0;
                    pMVTmp[2].mvx = pMVTmp[2].mvy = 0;
                    pMVTmp[3].mvx = pMVTmp[3].mvy = 0;

                    pMVTmp += t->m_pCurrentFrame->m_subBlockSize.width;
                }
            }
            else
            {
                /* zero out advanced intra coding type */
                /* Since sizeof(DecoderIntraType) is 1, just store */
                /* 4 U32's of 0.  We can only do this safely if */
                /* m_pIsMBIntraTypes is 4-byte aligned, which */
                /* we've taken steps to guarantee. */
                RVAssert(sizeof(pMBIntraTypes[0]) == 1);
                ((U32*)pMBIntraTypes)[0] = 0;
                ((U32*)pMBIntraTypes)[1] = 0;
                ((U32*)pMBIntraTypes)[2] = 0;
                ((U32*)pMBIntraTypes)[3] = 0;

                if (mbtype == MBTYPE_DIRECT)
                {
                    /* There are no motion vectors sent in Direct Mode */
                    pMVTmp = pMV;
                    for (i = 0; i < 4; i++)
                    {
                        pMVTmp[0].mvx = pMVTmp[0].mvy = 0;
                        pMVTmp[1].mvx = pMVTmp[1].mvy = 0;
                        pMVTmp[2].mvx = pMVTmp[2].mvy = 0;
                        pMVTmp[3].mvx = pMVTmp[3].mvy = 0;
                        pMVTmp += t->m_pCurrentFrame->m_subBlockSize.width;
                    }
                }
                else
                {
                    dec_status = Decoder_decodeMotionVectors(t,pMB, pMV, mbnum, pBitstream);
                    if (dec_status != RV_S_OK)
                    {
                        status = dec_status;
                        break;
                    }
                }

                if (inter16x16)
                    cbp = CB_GetSuperCBP(pBitstream, 2 /* 16x16 xform */, EntropyQP);
                else
                    cbp = CB_GetSuperCBP(pBitstream, 0 /* inter */, EntropyQP);

                RVDebug((RV_TL_BLK, "cbp: %2x", cbp));
            }

        }
        else
        {
            RVAssert(0);
            status = RV_S_UNSUPPORTED;
            break;
        }

        pMB->QP = QP;
        /* Copy the intra-coding types for the bottom row of subblocks */
        /* to m_pAboveSubBlockIntraTypes, so it can be used when decoding */
        /* the next row of macroblocks. "Extra" local var used here to "assist" */
        /* the MS compiler with generating faster code. */
        pTmp = &t->m_pAboveSubBlockIntraTypes[iThread][(mbXOffset >> 2)];
        pTmp[0] = pMBIntraTypes[0+12];
        pTmp[1] = pMBIntraTypes[1+12];
        pTmp[2] = pMBIntraTypes[2+12];
        pTmp[3] = pMBIntraTypes[3+12];

        RVDebug((RV_TL_SUBPICT, "MB %3d:  CBP  %2lx", mbnum, cbp));

        pTmp32 = (U32*)(vppDataIn->vld_cof);

        //clear vld buffer
        for(i=50; i>0; i--)
        {
            *pTmp32++ =0; *pTmp32++ =0;
            *pTmp32++ =0; *pTmp32++ =0;
        }

        Decoder_decodeLumaAndChromaCoefficients_SVLC(
            t,
            cbp,
            intra,
            intra16x16,
            inter16x16,
            QP,
            EntropyQP,
            pBitstream
        );

        /* set all bits in current cbpcoef if INTRA */
        /* otherwise, set according to cbp */
        if (IS_INTRA_MBTYPE(mbtype))
        {
            pMB->cbpcoef = 0xffffff;
            if (t->m_bIsRV8)
            {
                t->m_num_active_mbs ++;
            }
        }
        else
        {
            pMB->cbpcoef = cbp;
        }

        if (mbtype == MBTYPE_INTER_16x16)
            pMB->cbpcoef |= 0xffff;

        pMB->mvdelta = 0;

        if (t->m_bIsRV8)
            Decoder_setDeblockCoefs_RV8(t,pMB, pMV, t->m_pReferenceMBInfo[mbnum].mbtype);
        else
            Decoder_setDeblockCoefs(t,pMB, pMV, t->m_pReferenceMBInfo[mbnum].mbtype);

        if (status != RV_S_OK) break;
        /* Finally, perform motion compensation to reconstruct the YUV data */

        pMB = &t->m_pMBInfo[mbnum];
        mbtype = pMB->mbtype;
        cbp = pMB->cbpcoef;
        intra = (RV_Boolean)(IS_INTRA_MBTYPE(mbtype));
        intra16x16 = (RV_Boolean)(mbtype == MBTYPE_INTRA_16x16);
        vppDataIn->bidir_pred   = 0;
        vppDataIn->mb_pskip     = 0;
        vppDataIn->mb_offsetx   = (mbXOffset>>4);
        vppDataIn->top          = (pMB->edge_type & D_TOP_EDGE);
        vppDataIn->left         = (pMB->edge_type & D_LEFT_EDGE);
        vppDataIn->topright     = (pMB->edge_type & D_RIGHT_EDGE);

        if (bCurrentFrameIsBFrame)
        {
            /* Depending on MB type set up pointers to reference planes */
            if (mbtype == MBTYPE_BACKWARD)
            {
                /* backward prediction */
                pRefYPlane = t->m_pCurrentFrame->m_pFutureFrame->m_pYPlane;
                pRefUPlane = t->m_pCurrentFrame->m_pFutureFrame->m_pUPlane;
                pRefVPlane = t->m_pCurrentFrame->m_pFutureFrame->m_pVPlane;
            }
            else
            {
                /* forward prediction */
                pRefYPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pYPlane;
                pRefUPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pUPlane;
                pRefVPlane = t->m_pCurrentFrame->m_pPreviousFrame->m_pVPlane;
            }
        }

        if ((pMB->mbtype == MBTYPE_SKIPPED) && !(bCurrentFrameIsBFrame))
        {
            /* Just copy the reference plane data. */
            U32         pitch, offset;

            vppDataIn->mb_pskip     = 1;

            pitch = t->m_pCurrentFrame->m_pitch;
            offset = mbXOffset + (mbYOffset * pitch);
            //CopyMBLuma(t->m_pCurrentFrame->m_pYPlane + offset, pRefYPlane + offset, pitch); hx chg
            vppDataIn->interplate_yblk_addr[0] = (INT32)(pRefYPlane + offset) & VOC_CFG_DMA_EADDR_MASK;

            //offset >>= 1; sheen
            offset = (mbXOffset>>1) + ((mbYOffset>>1) * (pitch>>1));
            //CopyMBChroma(t->m_pCurrentFrame->m_pUPlane + offset, pRefUPlane + offset, pitch); hx chg
            vppDataIn->interplate_ublk_addr[0] = (INT32)(pRefUPlane + offset) & VOC_CFG_DMA_EADDR_MASK;
            //CopyMBChroma(t->m_pCurrentFrame->m_pVPlane + offset, pRefVPlane + offset, pitch); hx chg
            vppDataIn->interplate_vblk_addr[0] = (INT32)(pRefVPlane + offset) & VOC_CFG_DMA_EADDR_MASK;
        }
        else
        {
            dec_status = RV_S_OK;

            /* Reconstruct luma */
            if (((pMB->mbtype == MBTYPE_DIRECT) || (pMB->mbtype == MBTYPE_SKIPPED))&& (bCurrentFrameIsBFrame))
            {
                /* set pMVR to the corresponding motion vector in reference mv store */
                pMVR = (pMV - t->m_pMotionVectors) + t->m_pReferenceMotionVectors;

                vppDataIn->ratio0   = iRatio0;
                vppDataIn->ratio1   = iRatio1;
                vppDataIn->bidir_pred   = 1;

                if (t->m_bIsRV8)
                    dec_status = Decoder_reconstructLumaMacroblockDirect_RV8(t, pMB->mbtype,
                                 t->m_pReferenceMBInfo[mbnum].mbtype,
                                 pMVR,
                                 t->m_pCurrentFrame->m_pFutureFrame->m_pYPlane,
                                 t->m_pCurrentFrame->m_pPreviousFrame->m_pYPlane,
                                 mbXOffset,
                                 mbYOffset,
                                 iRatio0,
                                 iRatio1
                                                                            );
                else
                {
                    dec_status = Decoder_reconstructLumaMacroblockDirect(t, pMB->mbtype,
                                 t->m_pReferenceMBInfo[mbnum].mbtype,
                                 pMVR,
                                 t->m_pCurrentFrame->m_pFutureFrame->m_pYPlane,
                                 t->m_pCurrentFrame->m_pPreviousFrame->m_pYPlane,
                                 mbXOffset,
                                 mbYOffset,
                                 iRatio0,
                                 iRatio1
                                                                        );
                }
            }
            else if ((pMB->mbtype == MBTYPE_BIDIR) && bCurrentFrameIsBFrame)
            {
                struct DecoderPackedMotionVector *pMVp, *pMVf;
                pMVp = pMV;
                pMVf = t->m_pBidirMotionVectors + mbnum;

                vppDataIn->ratio0   = 8192;
                vppDataIn->ratio1   = 8192;
                vppDataIn->bidir_pred= 1;

                dec_status = Decoder_reconstructLumaMacroblockBidir(t, pMB->mbtype,
                             pMVp,
                             pMVf,
                             t->m_pCurrentFrame->m_pFutureFrame->m_pYPlane,
                             t->m_pCurrentFrame->m_pPreviousFrame->m_pYPlane,
                             mbXOffset,
                             mbYOffset);
            }
            else if (intra16x16)
            {
                vppDataIn->intra_mode[0] = pMBIntraTypes[0];
            }
            else if(intra)
            {
                pTmp16 = vppDataIn->intra_mode;

                for(i=0; i<16; i++)
                {
                    *pTmp16++ = pMBIntraTypes[i];
                }
            }
            else
            {
                if (t->m_bIsRV8)
                {
                    dec_status =
                        Decoder_reconstructLumaMacroblock_RV8(
                            t,
                            pMB->mbtype,
                            pMV,
                            pRefYPlane,
                            mbXOffset,
                            mbYOffset
                        );
                }
                else
                {
                    dec_status =
                        Decoder_reconstructLumaMacroblock(
                            t,
                            pMB->mbtype,
                            pMV,
                            pRefYPlane,
                            mbXOffset,
                            mbYOffset
                        );
                }
            }

            if (dec_status != RV_S_OK)
            {
                status = dec_status;
                break;
            }


            if ( ((pMB->mbtype == MBTYPE_DIRECT) || (pMB->mbtype == MBTYPE_SKIPPED)) && (bCurrentFrameIsBFrame) )
            {
                /* set pMVR to the corresponding motion vector in reference mv store */
                pMVR = (pMV - t->m_pMotionVectors) + t->m_pReferenceMotionVectors;

                if (t->m_bIsRV8)
                    Decoder_reconstructChromaMacroBlockDirect_RV8(
                        t,
                        pMB->mbtype,
                        t->m_pReferenceMBInfo[mbnum].mbtype,
                        pMVR,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pVPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pVPlane,
                        mbXOffset, mbYOffset, iRatio0, iRatio1);
                else
                    Decoder_reconstructChromaMacroBlockDirect (
                        t,
                        pMB->mbtype,
                        t->m_pReferenceMBInfo[mbnum].mbtype,
                        pMVR,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pVPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pVPlane,
                        mbXOffset, mbYOffset, iRatio0, iRatio1);
            }
            else if ((pMB->mbtype == MBTYPE_BIDIR) && bCurrentFrameIsBFrame)
            {
                struct DecoderPackedMotionVector *pMVp, *pMVf;
                pMVp = pMV;
                pMVf = t->m_pBidirMotionVectors + mbnum;
                Decoder_reconstructChromaMacroBlockBidir(t,
                        pMVp,
                        pMVf,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pUPlane,
                        t->m_pCurrentFrame->m_pFutureFrame->m_pVPlane,
                        t->m_pCurrentFrame->m_pPreviousFrame->m_pVPlane,
                        mbXOffset,
                        mbYOffset
                                                        );

            }
            else if (!intra16x16 && !intra)
            {
                if (t->m_bIsRV8)
                {
                    Decoder_reconstructChromaBlock_RV8(t, pMB, pMV, pRefUPlane,pRefVPlane,mbXOffset, mbYOffset);
                }
                else
                {
                    Decoder_reconstructChromaBlock(t, pMB, pMV, pRefUPlane,pRefVPlane,mbXOffset, mbYOffset);
                }
            }
        }


        Data_tmp32 = mbXOffset + (mbYOffset * t->m_pCurrentFrame->m_pitch);
        pTmp8 = (U8 *)(t->m_pCurrentFrame->m_pYPlane + Data_tmp32);
        vppDataIn->out_buffer_y_ptr = ((U32)pTmp8) & VOC_CFG_DMA_EADDR_MASK;

        //Data_tmp32 = Data_tmp32>>1; sheen
        Data_tmp32 = (mbXOffset>>1) + ((mbYOffset>>1) * (t->m_pCurrentFrame->m_pitch>>1));
        pTmp8 = (U8 *)(t->m_pCurrentFrame->m_pUPlane + Data_tmp32);
        vppDataIn->out_buffer_u_ptr = ((U32)pTmp8) & VOC_CFG_DMA_EADDR_MASK;

        pTmp8 = (U8 *)(t->m_pCurrentFrame->m_pVPlane + Data_tmp32);
        vppDataIn->out_buffer_v_ptr = ((U32)pTmp8) & VOC_CFG_DMA_EADDR_MASK;

#ifdef LOOP_FILTER_EN

        if (!t->m_pCurrentFrame->m_pictureHeader.Deblocking_Filter_Passthrough)
        {
            DeblockingFilter_EdgeFilter4x4StrongOptimizedNSM(
                &t->m_inLoopDeblockingFilter,
                //t->m_pCurrentFrame->m_pYPlane + Data_tmp32,
                //t->m_pCurrentFrame->m_pVPlane + (Data_tmp32>>1),
                //t->m_pCurrentFrame->m_pUPlane + (Data_tmp32>>1),
                t->m_pCurrentFrame->m_lumaSize.width,
                t->m_pCurrentFrame->m_lumaSize.height,
                //t->m_pCurrentFrame->m_pitch,
                &(t->m_pMBInfo[mbnum].QP),
                sizeof(struct DecoderMBInfo),
                &(t->m_pMBInfo[mbnum].cbpcoef),
                sizeof(struct DecoderMBInfo) >> 2,
                &(t->m_pMBInfo[mbnum].mbtype),
                sizeof(struct DecoderMBInfo),
                &(t->m_pMBInfo[mbnum].mvdelta),
                sizeof(struct DecoderMBInfo) >> 2,
                t->m_pCurrentFrame->m_pictureHeader.PicCodType,
                mbnum,
                mbXOffset
                //refQP
            );
            t->m_pCurrentFrame->m_isDeblocked = TRUE;
        }

#endif

#ifndef SHEEN_VC_DEBUG
//      rm_time2=hal_TimGetUpTime();
//      rm_total1+=(rm_time2-rm_time1);
#endif

        if(mb_exceed>=RM_PARA_BUF_NUM || mbnum == totalMBs-1)//STOP AND WAIT FOR VOC DECODING
        {
            U32 cs_status,irqEnable;
            //rm_printf("[RM]num=%d ", mbnum);

#ifndef SHEEN_VC_DEBUG
//rm_time1=hal_TimGetUpTime();
#endif
            vpp_data_buffer[vpp_buffer_wt_id].done=1;
            vpp_buffer_wt_id=1-vpp_buffer_wt_id;

            i=0;
            while( vpp_data_buffer[vpp_buffer_wt_id].done==1 && mbnum != (totalMBs-1) )//voc vid over
            {
                i++;
#ifndef SHEEN_VC_DEBUG

                if(i>0x2000000)
                {
                    rm_printf("[RM]wait voc fail. mbnum=%d voc_work=%d voc_vid=%d ", mbnum, *(vocWorkState.pVOC_WORK),
                              *(vocWorkState.pVOC_VID) );
                    return RV_S_ERROR;//about 60ms
                }
                //sxr_Sleep(1);

#endif
            }

#ifndef SHEEN_VC_DEBUG
            *(vocWorkState.pVOC_VID)+=mb_exceed;

            cs_status=hal_SysEnterCriticalSection();
            irqEnable=0;

            if(*(vocWorkState.pVOC_WORK)==0)//no voc audio
            {
                *(vocWorkState.pVOC_WORK)=2;

                hal_SysExitCriticalSection(cs_status);
                irqEnable=1;

                //*(pRmVocWorkState->pVOC_VID)+=mb_exceed;
                //if(vpp_AudioJpegDecGetMode()!=MMC_RM_DECODE )
                {
                    //vpp_AudioJpegDecSetCurMode(-1, -1);
                    //vpp_AudioJpegDecSetInputMode(MMC_RM_DECODE);
                    vpp_AVDecSetMode(MMC_RM_DECODE, 0, 0);
                    //rm_printf("[RM]voc reload\n");
                }
#endif
                //VOC DECODE OVER
                vpp_rmvb_cfg.para_buf_ptr= (INT32)(vpp_data_buffer[vpp_buffer_rd_id].buffer) & VOC_CFG_DMA_EADDR_MASK;
                vpp_rmvb_cfg.mb_num =   mb_exceed;
                vpp_RMVBDecScheduleOneBlk(&vpp_rmvb_cfg);
                //rm_printf("[RM]voc call");
#ifndef SHEEN_VC_DEBUG
            }

            if(irqEnable==0)
                hal_SysExitCriticalSection(cs_status);

#endif
            mb_exceed=0;

            //if(pRmVocWorkState->aud_dec)
            //  pRmVocWorkState->aud_dec();//check and decode ra.

#ifndef SHEEN_VC_DEBUG
//rm_time2=hal_TimGetUpTime();
//rm_total2+=(rm_time2-rm_time1);
#endif

        }

        mbXOffset += 16;
        pMV += 4;

        if (mbXOffset == t->m_pCurrentFrame->m_lumaSize.width)
        {
            mbXOffset  = 0;
            mbYOffset += 16;
            pMV += 3 * t->m_pCurrentFrame->m_subBlockSize.width;
        }

        /* If we're at the end of the stream, then do not continue decoding */
        if (bExitAfterRecon)
            break;

        if(fillAudioData && mbnum>=fillAudMbScale)//read audio when image big than qcif
        {
            needBackUpFileOffset=1;
            fillAudioData();
            fillAudMbScale+=MBScale4AudRed;
        }
        if(vocWorkState.aud_nframe_dec)
            vocWorkState.aud_nframe_dec();//check and decode ra.

    }   /* while mbnum */
#ifndef SHEEN_VC_DEBUG
//rm_printf("vld&rec  time=%d voc wait time=%d \n", rm_total1*1000/16384, rm_total2*1000/16384);
#endif

#if 0// voc sheen
    if (status == RV_S_OK)
    {
        /* might have lost some end packets */
        if (mbnum < totalMBs)
        {
            Decoder_insertMissingMacroBlocks(t, mbnum, totalMBs, &mbXOffset, &mbYOffset, &pMV);
            uNumMissingMBs += totalMBs - mbnum;
        }
    }
#endif

    t->m_uNumMissingMBs = uNumMissingMBs;

#ifndef SHEEN_VC_DEBUG
    /*
        i=0;
        while(*(pRmVocWorkState->pVOC_VID)!=0)//wait the last mb.
        {
            //i++;
            //if(i>640)return RV_S_ERROR;//about 40ms
            //sxr_Sleep(1);
        }

    */
    //vpp_AudioJpegDecSetCurMode(-1, -1);
    //hal_VocIrqMaskSet(0x1);//set irq.
    //pRmVocWorkState->vid_voc_isr(NULL);//callback as isr
#endif

    return status;
}

