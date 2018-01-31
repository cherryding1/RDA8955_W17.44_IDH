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





#include "cs_types.h"
#include "gsm.h"
#include "global_macros.h"
// SPAL PUBLIC HEADERS
#include "spal_vitac.h"
#include "spal_xcor.h"
#include "spal_cap.h"
#include "spal_itlv.h"
#include "spal_irq.h"
#include "spal_ifc.h"
#include "spal_rfif.h"
// SPP
#include "spp_gsm.h"
#include "spp_profile_codes.h"
#include "sppp_gsm_private.h"
#include "sppp_debug.h"

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spp_ReorderEFR
//-----------------------------------------------------------------------------
/// EFR reordering, CRC encoding, etc
///
/// @param TxBuffer UINT32*. ?? See spp_tx.c
/// @param ReorderOut UINT8*. ?? See spp_tx.c
///
//=============================================================================
PRIVATE VOID spp_ReorderEFR(UINT32* TxBuffer, UINT8* ReorderOut)
{
    UINT16 i, j, k;
    register UINT32 tmp_buf;
    UINT32 dividend[3];    // for CRC
    UINT32 crccode[2];         // results of CRC

    // Reordering 2 - without CRC code
    //--------------------------------
    // Reordering
    for (i=0; i<7; i++)
    {
        tmp_buf = TxBuffer[i];
        k = i*32;
        for (j=0; j<32; j++)
        {
            ReorderOut[g_sppMatrixEfr2Inv[k+j]] = (UINT8)(tmp_buf&1);
            tmp_buf = tmp_buf >> 1;
        }
    }
    tmp_buf = TxBuffer[7];
    k = i*32;
    for (j=0; j<20; j++)
    {
        ReorderOut[g_sppMatrixEfr2Inv[k+j]] = (UINT8)(tmp_buf&1);
        tmp_buf = tmp_buf >> 1;
    }

    // Prepare data for CRC code encoding
    for (i=0; i<2; i++)
    {
        k = i*32;
        dividend[i]=0;
        for (j=0; j<32; j++)
            dividend[i] |= ReorderOut[g_sppMatrixEfr1Inv[k+j]] << j;
    }
    dividend[2] = ReorderOut[g_sppMatrixEfr1Inv[64]];

    // CRC code encoding
    spal_CapEncodeCrc(g_sppCsCrcParam[CS_EFR],dividend,crccode);

    // Repetition bits
    ReorderOut[183] = ReorderOut[182] = !!(TxBuffer[2]&0x00000020);  // 69
    ReorderOut[186] = ReorderOut[185] = !!(TxBuffer[3]&0x00800000);  // 119
    ReorderOut[189] = ReorderOut[188] = !!(TxBuffer[5]&0x00001000);  // 172
    ReorderOut[192] = ReorderOut[191] = !!(TxBuffer[6]&0x40000000);  // 222

    // copy CRC code
    for (i=65; i<73; i++)
    {
        ReorderOut[i] = (UINT8)(crccode[0]&1);
        crccode[0] = crccode[0] >> 1;
    }
}

#ifdef CHIP_DIE_8955
//======================================================================
// spp_EncodeABurst
//----------------------------------------------------------------------
// Access Burst encoding
//
// @param BufferIn UINT32*. Pointer to the data to be encoded.
// @param Bsic UINT8. Base Station BSIC.
// @param size UINT8. 8 or 11 bits Access Burst.
// @param ts UINT8. ts:0,1,2
// @param BufferOut UINT32*. Encoded data output pointer.
//======================================================================
PUBLIC VOID spp_EncodeABurst(UINT32* BufferIn, UINT8 Bsic, UINT8 size, UINT8 ts, UINT32* BufferOut)
{
    UINT32 crccode[2];
    UINT16 i;
    UINT8 Bsic_inv = 0;
    UINT32 ConvOut[2];
    UINT32* ConvOut_uptr = MEM_ACCESS_UNCACHED(ConvOut);
    SPP_CODING_SCHEME_T coding_scheme = (size==8)?CS_RACH:CS_PRACH;
    UINT32* AburstTscVec[3] = {(UINT32*)g_sppGsmTscAburst, (UINT32*)g_sppGsmTscAburst_1, (UINT32*)g_sppGsmTscAburst_2};
    UINT32* ATsc = AburstTscVec[ts];

    SPP_PROFILE_FUNCTION_ENTRY(spp_EncodeABurst);

    ConvOut_uptr[0] = ConvOut_uptr[1] = 0;


    // CRC encoding
    spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],BufferIn,crccode);

    // Burst building (1)
#if 0 // Old
    BufferOut[0] = (0x5C << 4) | (0xf);  // Extended tail bits and 4 guard bits
    BufferOut[0]|= (g_sppGsmTscAburst[0] & 0x000FFFFF) << 12;   // Sync Seq Bits(0-19)
    BufferOut[1] = (g_sppGsmTscAburst[0] & 0xFFF00000) >> 20;   // Sync Seq Bits(20-31)
    BufferOut[1]|= (g_sppGsmTscAburst[1] & 0x000001FF) << 12;   // Sync Seq Bits(32-40)
#else

    BufferOut[0] = (ATsc[0] << 8 )  | 0x5C;
    BufferOut[1] = ATsc[0] >> 24;
    BufferOut[1]|= (ATsc[1] & 0x000001FF) << 8;
#endif

    for (i=0; i<6; i++)
        Bsic_inv = Bsic_inv | (((Bsic>>i) & 0x1) <<(5-i));

    crccode[0] = crccode[0] ^ Bsic_inv;

    if (coding_scheme == CS_PRACH)
        BufferIn[0] =  BufferIn[0] & 0x000007FF ;
    else
        BufferIn[0] =  BufferIn[0] & 0x000000FF ;

    BufferIn[0] |=  (crccode[0] & 0x3F) << size;

    spal_CapConvoluate( g_sppCsConvParam[coding_scheme],
                        (UINT32*) g_sppCsPuncturingTable[coding_scheme],
                        BufferIn,
                        ConvOut_uptr);

    // Burst building (2)
#if 0 // Old
    BufferOut[1]|= (ConvOut_uptr[0] & 0x000007FF) << 21; // Encrypted Bits(0-10)
    BufferOut[2] = (ConvOut_uptr[0] & 0xFFFFF800) >> 11; // Encrypted Bits(11-31)
    BufferOut[2]|= (ConvOut_uptr[1] & 0x0000000F) << 21; // Encrypted Bits(32-35)+tail bits
    BufferOut[2]|= 0xF0000000; // Extended guard period
    BufferOut[3] = 0xFFFFFFFF; // Extended guard period
    BufferOut[4] = 0xFFFFFFFF; // Extended guard period
#else
    BufferOut[1]|= (ConvOut_uptr[0] & 0x00007FFF) << 17;
    BufferOut[2] = (ConvOut_uptr[0] & 0xFFFF8000) >> 15;
    BufferOut[2]|= (ConvOut_uptr[1] & 0x0000000F) << 17;
    BufferOut[2]|= 0xFF000000; // useless
    BufferOut[3] = 0xFFFFFFFF; // useless
    BufferOut[4] = 0xFFFFFFFF; // useless
#endif

    //for (i=0;i<5;i++) debugBuf[idx++] = BufferOut[i];
    SPP_PROFILE_FUNCTION_EXIT(spp_EncodeABurst);
}
#else
//======================================================================
// spp_EncodeABurst
//----------------------------------------------------------------------
// Access Burst encoding
//
// @param BufferIn UINT32*. Pointer to the data to be encoded.
// @param Bsic UINT8. Base Station BSIC.
// @param size UINT8. 8 or 11 bits Access Burst.
// @param BufferOut UINT32*. Encoded data output pointer.
//======================================================================
PUBLIC VOID spp_EncodeABurst(UINT32* BufferIn, UINT8 Bsic, UINT8 size, UINT32* BufferOut)
{
    UINT32 crccode[2];
    UINT16 i;
    UINT8 Bsic_inv = 0;
    UINT32 ConvOut[2];
    UINT32* ConvOut_uptr = MEM_ACCESS_UNCACHED(ConvOut);
    SPP_CODING_SCHEME_T coding_scheme = (size==8)?CS_RACH:CS_PRACH;

    SPP_PROFILE_FUNCTION_ENTRY(spp_EncodeABurst);
    ConvOut_uptr[0] = ConvOut_uptr[1] = 0;


    // CRC encoding
    spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],BufferIn,crccode);

    // Burst building (1)
#if 0 // Old
    BufferOut[0] = (0x5C << 4) | (0xf);  // Extended tail bits and 4 guard bits
    BufferOut[0]|= (g_sppGsmTscAburst[0] & 0x000FFFFF) << 12;   // Sync Seq Bits(0-19)
    BufferOut[1] = (g_sppGsmTscAburst[0] & 0xFFF00000) >> 20;   // Sync Seq Bits(20-31)
    BufferOut[1]|= (g_sppGsmTscAburst[1] & 0x000001FF) << 12;   // Sync Seq Bits(32-40)
#else
    BufferOut[0] = (g_sppGsmTscAburst[0] << 8 )  | 0x5C;
    BufferOut[1] = g_sppGsmTscAburst[0] >> 24;
    BufferOut[1]|= (g_sppGsmTscAburst[1] & 0x000001FF) << 8;
#endif

    for (i=0; i<6; i++)
        Bsic_inv = Bsic_inv | (((Bsic>>i) & 0x1) <<(5-i));

    crccode[0] = crccode[0] ^ Bsic_inv;

    if (coding_scheme == CS_PRACH)
        BufferIn[0] =  BufferIn[0] & 0x000007FF ;
    else
        BufferIn[0] =  BufferIn[0] & 0x000000FF ;

    BufferIn[0] |=  (crccode[0] & 0x3F) << size;

    spal_CapConvoluate( g_sppCsConvParam[coding_scheme],
                        (UINT32*) g_sppCsPuncturingTable[coding_scheme],
                        BufferIn,
                        ConvOut_uptr);

    // Burst building (2)
#if 0 // Old
    BufferOut[1]|= (ConvOut_uptr[0] & 0x000007FF) << 21; // Encrypted Bits(0-10)
    BufferOut[2] = (ConvOut_uptr[0] & 0xFFFFF800) >> 11; // Encrypted Bits(11-31)
    BufferOut[2]|= (ConvOut_uptr[1] & 0x0000000F) << 21; // Encrypted Bits(32-35)+tail bits
    BufferOut[2]|= 0xF0000000; // Extended guard period
    BufferOut[3] = 0xFFFFFFFF; // Extended guard period
    BufferOut[4] = 0xFFFFFFFF; // Extended guard period
#else
    BufferOut[1]|= (ConvOut_uptr[0] & 0x00007FFF) << 17;
    BufferOut[2] = (ConvOut_uptr[0] & 0xFFFF8000) >> 15;
    BufferOut[2]|= (ConvOut_uptr[1] & 0x0000000F) << 17;
    BufferOut[2]|= 0xFF000000; // useless
    BufferOut[3] = 0xFFFFFFFF; // useless
    BufferOut[4] = 0xFFFFFFFF; // useless
#endif
    //for (i=0;i<5;i++) debugBuf[idx++] = BufferOut[i];
    SPP_PROFILE_FUNCTION_EXIT(spp_EncodeABurst);
}
#endif

//======================================================================
// spp_EncodeBlock
//----------------------------------------------------------------------
// Normal Burst encoding
//
// @param InTxBuffer UINT32*. Pointer to the data to be encoded.
// @param ChMode UINT16. Channel Type (PAL API format).
// @param AmrFrameType UINT8. Type of AMR frame (set to 0 excepted for AMR).
// @param AmrMode UINT8. Codec Mode (set to 0 excepted for AMR).
// @param AmrInBandData UINT8*. AMR in Band Data (set to NULL excepted for AMR).
// @param BurstOffset UINT8. Offset position for interleaving. Interleaving
// is performed over a number of Tx bursts. The Offset position corresponds to
// the offset number of bursts in the interleaving buffer. For example, in
// FR mode, BurstOffset can be 0 or 4 as one coded block is interleaved over
// 8 consecutive bursts, and each coded block has 4 bursts worth of payload.
// @param InterleaverType UINT8. INPUT. InterleaverType may take the following values:
// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
// TCH/AFS (speech, ratscch, sid_first).
// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
// - 2 : TYPE_1C. Valid for FACCH/H.
// - 3 : TYPE_2A. Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
// - 4 : TYPE_2B. Valid for TCH/AHS (sid_update).
// - 5 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
// and TCH/H2.4.
// .
// @param BufferOut UINT32*. Encoded data output pointer. The function
// stores the output (the interleaved and encoded bits) in the Tx interleaver buffer.
// BufferOut should point to the beginning of the Tx interleaver buffer.
//======================================================================
PUBLIC VOID spp_EncodeBlock(UINT32* InTxBuffer,
                            UINT16 ChMode,
                            UINT8 AmrFrameType,
                            UINT8 AmrMode,
                            UINT8* AmrInBandData,
                            UINT8 BurstOffset,
                            UINT8 InterleaverType,
                            UINT32* BufferOut)
{
    UINT32 ConvOut_tab[15];
    UINT32* ConvOut = MEM_ACCESS_UNCACHED(ConvOut_tab);
    // c is the input for interleaving (456 bits)
    // c = Convolutional code encoding output + encoded in-band data
    UINT32 c[15];
    UINT32 u[15];
    UINT16 i, j, k;
    UINT32 crccode[2];
    UINT16 ic[2];
    UINT32 TxBuffer[14]; // AVOID corruption of the original buffer
    UINT8 LengthWord, ReorderBitLeft;
    UINT32 * PunctTable;
    SPP_CODING_SCHEME_T coding_scheme;

    // TODO : work this out...
    // Big tables for FR/EFR
    UINT8 ReorderOut[260];

    SPP_PROFILE_FUNCTION_ENTRY(spp_EncodeBlock);

    coding_scheme = spp_GetChannelType(ChMode,AmrFrameType,AmrMode);

    for (i=0; i<14; i++)
        TxBuffer[i] = InTxBuffer[i];

    PunctTable  = (UINT32*) g_sppCsPuncturingTable[coding_scheme];

    // Main Switch
    switch (coding_scheme)
    {
        case CS_CS1:
            // Fire code encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);

            TxBuffer[5] &= 0x00FFFFFF;
            TxBuffer[5] |= (crccode[0]&0x000000FF) << 24;
            TxBuffer[6]  = (crccode[0]&0xFFFFFF00) >> 8;
            TxBuffer[6] |= (crccode[1]&0xFF) << 24;
            TxBuffer[7]  = 0;

            // Convolutional encoding
            spal_CapConvoluate(g_sppCsConvParam[CS_CS1], PunctTable, TxBuffer, ConvOut);

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_CS2:
            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);

            // Usf encoding
            u[0] = g_sppUsfCs2Cs3[(TxBuffer[0]&0x7)];
            u[0] |= (TxBuffer[0]&0x1FFFFFF8) << 3;
            for (i=1; i<8; i++)
            {
                u[i]  = (TxBuffer[i-1]&0xE0000000) >> 29;
                u[i] |= (TxBuffer[i]  &0x1FFFFFFF) << 3;
            }
            u[8]  = (TxBuffer[7]&0xE0000000) >> 29;
            u[8] |= (TxBuffer[8]&0x00007FFF) << 3;
            u[8] |= (crccode[0]&0x00003FFF) << 18;
            u[9]  = (crccode[0]&0x0000C000) >> 14;

            // Convolutional encoding
            spal_CapConvoluate(g_sppCsConvParam[CS_CS2], PunctTable, u, ConvOut);
            ConvOut[14] &= 0x000000FF;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_CS3:
            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);

            // Usf encoding
            u[0] = g_sppUsfCs2Cs3[(TxBuffer[0]&0x7)];
            u[0] |= (TxBuffer[0]&0x1FFFFFF8) << 3;
            for (i=1; i<9; i++)
            {
                u[i]  = (TxBuffer[i-1]&0xE0000000) >> 29;
                u[i] |= (TxBuffer[i]  &0x1FFFFFFF) << 3;
            }
            u[9]  = (TxBuffer[8]&0xE0000000) >> 29;
            u[9] |= (TxBuffer[9]&0x07FFFFFF) << 3;
            u[9] |= (crccode[0]&0x00000003) << 30;
            u[10] = (crccode[0]&0x0000FFFC) >> 2;

            // Convolutional encoding
            spal_CapConvoluate(g_sppCsConvParam[CS_CS3], PunctTable, u, ConvOut);
            ConvOut[14] &= 0x000000FF;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_CS4:
            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);

            // Usf encoding
            u[0] = g_spp_UsfCs4[(TxBuffer[0]&0x7)];
            u[0] |= (TxBuffer[0]&0x007FFFF8) << 9;
            for (i=1; i<13; i++)
            {
                u[i]  = (TxBuffer[i-1]&0xFF800000) >> 23;
                u[i] |= (TxBuffer[i]  &0x007FFFFF) << 9;
            }
            u[13]  = (TxBuffer[12]&0xFF800000) >> 23;
            u[13] |= (TxBuffer[13]&0x00007FFF) << 9;
            u[13] |= (crccode[0]&0x000000FF) << 24;
            u[14] = (crccode[0]&0x0000FF00) >> 8;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, u);
            break;

        case CS_HR:
        {
            // TODO :see if this could be factorized somehow spp_decode stuff

            UINT8 mode = (UINT8)((TxBuffer[1]&0x0000000C) >> 2); // Its value could be 0,1,2,3
            UINT32 CrcIn; // Temp buffer for CRC decoding
            UINT32 *ReorderOutHr = (UINT32*) ReorderOut;
            for (i=0; i<4; i++)
            {
                ReorderOutHr[i] = 0;
            }
            // Reordering
            mode = (mode)?1:0;  // chose the reordering table
            for (i=0; i<3; i++)
            {
                k = i*32;
                for (j=0; j<32; j++)
                    ReorderOutHr[i] |= (!!(TxBuffer[g_sppMatrixHr[mode][k+j]/32] &
                                           (1<<(g_sppMatrixHr[mode][k+j]%32)))) << j;
            }
            for (j=0; j<16; j++)
                ReorderOutHr[3] |= (!!(TxBuffer[g_sppMatrixHr[mode][96+j]/32] &
                                       (1<<(g_sppMatrixHr[mode][96+j]%32)))) << j;
            // Data for CRC code encoding
            CrcIn = (ReorderOutHr[2] & 0x7ffffe00) >> 9;

            // CRC decoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],&CrcIn,crccode);

            u[0] = ReorderOutHr[0];
            u[1] = ReorderOutHr[1];
            u[2] = ReorderOutHr[2];
            u[2] &= 0x7FFFFFFF;
            u[3] = 0;
            u[2] |= (crccode[0] & 0x1) << 31;
            u[3] |= (crccode[0] & 0x6) >> 1;

            // Convolutional encoding
            spal_CapConvoluate(g_sppCsConvParam[CS_HR], PunctTable, u, ConvOut);

            ConvOut[7]  = (ReorderOutHr[3]&0x0000F000) >> 12;
            ConvOut[6] &= 0x0007FFFF;

            ConvOut[6] |= (ReorderOutHr[2]&0x80000000) >> 12; // class 2
            ConvOut[6] |= (ReorderOutHr[3]&0x00000FFF) << 20;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, ConvOut);
        }
        break;

        case CS_EFR:
        case CS_FR:
            if(coding_scheme == CS_EFR)
            {
                // Reordering & 1st stage CRC code encoding
                spp_ReorderEFR(TxBuffer, ReorderOut);
            }
            else // (coding_scheme == CS_FR)
            {
                // Reordering
                UINT32 tmp;
                for (i=0; i<8; i++)
                {
                    tmp = TxBuffer[i];
                    k = i*32;
                    for (j=0; j<32; j++)
                    {
                        ReorderOut[g_sppMatrixFrInv[k+j]] = (UINT8)(tmp & 1);
                        tmp = tmp >> 1;
                    }
                }
                tmp = TxBuffer[8];
                for (j=0; j<4; j++)
                {
                    ReorderOut[g_sppMatrixFrInv[256+j]] = (UINT8)(tmp & 1);
                    tmp = tmp >> 1;
                }
            }
            // Prepare data for 2nd stage CRC encoding (50 bits)
            TxBuffer[0] = ReorderOut[0] & 1;
            for (j=1; j<32; j++)
                TxBuffer[0] |= (ReorderOut[j] & 1) << j;
            TxBuffer[1] = ReorderOut[32] & 1;
            for (j=1; j<18; j++)
                TxBuffer[1] |= (ReorderOut[j+32] & 1) << j;

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[CS_FR],TxBuffer,crccode);

            // Tailing bits and reordering
            for (i=0; i<6; i++)
                u[i] = 0;
            for (i = 0; i < 91; i++)
            {
                j = i * 2;
                u[i / 32] |= (ReorderOut[j]&1) << (i%32);
                j++;
                k = 184 - i;
                u[k / 32] |= (ReorderOut[j]&1) << (k%32);
            }
            for (i = 91; i < 94; i++)
            {
                u[i / 32] |= (crccode[0]&1) << (i%32);
                crccode[0] = crccode[0] >> 1;
            }

            // Convolutional encoding
            spal_CapConvoluate(g_sppCsConvParam[CS_FR], PunctTable, u, ConvOut);

            // Padding of class 2
            ConvOut[12] = ReorderOut[188]&1;
            for (i=189; i<220; i++)
                ConvOut[12] |= (ReorderOut[i]&1) << (i-188);
            ConvOut[13] = ReorderOut[220]&1;
            for (i=221; i<252; i++)
                ConvOut[13] |= (ReorderOut[i]&1) << (i-220);
            ConvOut[14] = ReorderOut[252]&1;
            for (i=253; i<260; i++)
                ConvOut[14] |= (ReorderOut[i]&1) << (i-252);

            for (i=182; i<188; i++)
                ConvOut[11] |= (ReorderOut[i]&1) << (i-156);

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_F96:
        case CS_H48:
            TxBuffer[7] &= 0x0000FFFF;
            // Convolutional encoder
            spal_CapConvoluate(g_sppCsConvParam[CS_F96], PunctTable, TxBuffer, ConvOut);
            ConvOut[14] &= 0x000000FF;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_F48:
        case CS_H24:
            if (coding_scheme == CS_F48)
            {
                u[0]  = TxBuffer[0]&0x00007FFF;
                u[0] |= (TxBuffer[0]&0x0FFF8000) << 4;
                u[1]  = (TxBuffer[0]&0x30000000) >> 28;
                u[1] |= (TxBuffer[0]&0xC0000000) >> 24;
                u[1] |= (TxBuffer[1]&0x00001FFF) << 8;
                u[1] |= (TxBuffer[1]&0x000FE000) << 12;
                // u[2]  = (TxBuffer[1]&0x0F000000) >> 20;
                u[2]  = (TxBuffer[1]&0x0FF00000) >> 20; // Todo : fix of reordering pb. Replace previous line.
                u[2] |= (TxBuffer[1]&0xF0000000) >> 16;
                u[2] |= (TxBuffer[2]&0x000007FF) << 16;
                u[2] |= (TxBuffer[2]&0x00000800) << 20;
                u[3]  = (TxBuffer[2]&0x03FFF000) >> 12;
                u[3] |= (TxBuffer[2]&0xFC000000) >> 8;
                u[3] |= (TxBuffer[3]&0x000000FF) << 24;
                u[4]  = (TxBuffer[3]&0x00000100) >> 8;
                u[4] |= (TxBuffer[3]&0x00FFFE00) >> 4;
            }
            else // (coding_scheme == CS_H24)
            {
                u[0]  = TxBuffer[0];
                u[1]  = TxBuffer[1];
                u[2]  = TxBuffer[2]&0x000000FF;
                u[2] |= (TxBuffer[2]&0x0FFFFF00) << 4;
                u[3]  = (TxBuffer[2]&0xF0000000) >> 28;
                u[3] |= (TxBuffer[3]&0x0FFFFFFF) << 4;
                u[4]  = (TxBuffer[3]&0xF0000000) >> 28;
                u[4] |= (TxBuffer[4]&0x0000FFFF) << 4;
            }

            // Convolutional encoder
            spal_CapConvoluate(g_sppCsConvParam[CS_F48], PunctTable, u, ConvOut);
            ConvOut[14] &= 0x000000FF;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_F24:
            TxBuffer[2] &= 0x000000FF;

            // Convolutional encoder
            spal_CapConvoluate(g_sppCsConvParam[CS_F24], PunctTable, TxBuffer, ConvOut);
            ConvOut[14] &= 0x000000FF;


            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_F144:
            TxBuffer[9] &= 0x00000003;

            // Convolutional encoder
            spal_CapConvoluate(g_sppCsConvParam[CS_F144], PunctTable, TxBuffer, ConvOut);
            ConvOut[14] &= 0x000000FF;

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, ConvOut);
            break;

        case CS_AFS_SID_UPDATE:
        {
            // For the mapping of ConvOut & g_sppImAfsSid on output c
            UINT16 *ConvOut_16 = (UINT16*)ConvOut;
            UINT16 *IM_AFS_SID_16 = (UINT16*)g_sppImAfsSid;
            UINT16 mask;
            // Coding of in-band data
            for (i=0; i<15; i++)
                c[i] = 0;
            ic[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            ic[1] = g_sppIcSidRatscch[AmrInBandData[1]];
            for (i=0; i<4; i++)
            {
                c[0] |= (ic[0] & 0xF) << 8*i;
                c[0] |= (ic[1] & 0xF) << (8*i+4);
                ic[0] = ic[0] >> 4;
                ic[1] = ic[1] >> 4;
            }

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);
            TxBuffer[1] &= 0x00000007;
            TxBuffer[1] |= crccode[0] << 3;

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,TxBuffer,ConvOut);

            // Mapping coded data to c
            // g_sppImAfsSid: repetition of "0 1111 0010"
            for (i=1; i<14; i++)
            {
                mask = 0x000F;
                k = i-1;
                for (j=0; j<4; j++)
                {
                    c[i] |= (ConvOut_16[k] & 0xF) << 8*j;
                    c[i] |= (IM_AFS_SID_16[k] & mask) << (4*(j+1));
                    ConvOut_16[k] = ConvOut_16[k] >> 4;
                    mask = mask << 4;
                }
            }
            c[14] = ConvOut_16[13] & 0xF;
            c[14] |= (IM_AFS_SID_16[13] & 0xF) << 4;
            // Interleaving
            spal_ItlvItlv(InterleaverType, 0, 456, BufferOut+BurstOffset*4, c);
        }
        break;

        case CS_AFS_SID_FIRST:
        {
            // For mapping of IM on output c
            UINT16 *IM_AFS_SID_16 = (UINT16*)g_sppImAfsSid;
            UINT16 mask;
            for (i=0; i<15; i++)
                c[i] = 0;
            // Coding of in-band data
            ic[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            for (i=0; i<4; i++)
            {
                c[0] |= (ic[0] & 0xF) << 8*i;
                ic[0] = ic[0] >> 4;
            }
            // Identification maker
            // g_sppImAfsSid: repetition of "0 1111 0010"
            for (i=1; i<14; i++)
            {
                mask = 0x000F;
                k = i-1;
                for (j=0; j<4; j++)
                {
                    c[i] |= (IM_AFS_SID_16[k] & mask) << (4*j);
                    mask = mask << 4;
                }
            }
            c[14] = IM_AFS_SID_16[13] & 0xF;
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, c);
        }
        break;

        case CS_AFS_ONSET:
        {
            UINT16 mask = 0x000F;
            // Mapping on c
            c[0] = 0;
            // Form the fist word
            for (i=1; i<5; i++)
            {
                c[0] |= (g_sppIcSidRatscch[AmrInBandData[1]] & mask) << (4*i);
                mask = mask << 4;
            }
            // Repeat the first word
            for (i=1; i<15; i++)
                c[i] = c[0];
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, c);
        }
        break;

        case CS_AFS_SPEECH_122:
        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_795:
        case CS_AFS_SPEECH_74:
        case CS_AFS_SPEECH_67:
        case CS_AFS_SPEECH_59:
        case CS_AFS_SPEECH_515:
        case CS_AFS_SPEECH_475:
        {
            // Input: 244, 204, 159, 148, 134, 118, 103, 95 bits
            // output: 456 bits = 8 encoded in-band data + 448 convolutional code
            // Output of reordering; number of bits varies from 244 to 95
            UINT32 *ReorderOutAfs= (UINT32*) ReorderOut;
            UINT8  *ReorderTab = (UINT8 *) (g_sppAfsReorderTables[AmrMode]);

            // Reordering according to table 7-14 in 45.003
            for (i=0; i<8; i++)
                ReorderOutAfs[i] = 0;
            LengthWord = g_sppInputLengthBitAfs[AmrMode]/32;
            ReorderBitLeft = g_sppInputLengthBitAfs[AmrMode]%32;
            for (i=0; i<LengthWord; i++)
            {
                k = i*32;
                for (j=0; j<32; j++)
                    ReorderOutAfs[i] |= (!!(TxBuffer[ReorderTab[k+j]/32] &
                                            (1<<(ReorderTab[k+j]%32)))) << j;
            }

            k = i*32;
            for (j=0; j<ReorderBitLeft; j++)
                ReorderOutAfs[i] |= (!!(TxBuffer[ReorderTab[k+j]/32] &
                                        (1<<(ReorderTab[k+j]%32)))) << j;

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],ReorderOutAfs,crccode);

            // Insert CRC code
            switch (AmrMode)
            {
                case SPP_MR122_MODE://AFS_12_2:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1];
                    u[2] = ReorderOutAfs[2] & 0x0001FFFF;
                    u[2] |= crccode[0] << 17;
                    u[2] |= (ReorderOutAfs[2] & 0x03FE0000) << 6;
                    for (i=3; i<8; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case SPP_MR102_MODE://AFS_10_2:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1];
                    u[2] = ReorderOutAfs[2] & 0x00000001;
                    u[2] |= crccode[0] << 1;
                    u[2] |= (ReorderOutAfs[2] & 0x03FFFFFE) << 6;
                    for (i=3; i<7; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case SPP_MR795_MODE://AFS_7_95:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1];
                    u[2] = ReorderOutAfs[2] & 0x000007FF;
                    u[2] |= crccode[0] << 11;
                    u[2] |= (ReorderOutAfs[2] & 0x03FFF800) << 6;
                    for (i=3; i<6; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case SPP_MR74_MODE://AFS_7_4:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1] & 0x1FFFFFFF;
                    u[1] |= crccode[0] << 29;
                    u[2] = (crccode[0]&0x38) >> 3;
                    u[2] |= (ReorderOutAfs[1] & 0xE0000000) >> 26;
                    u[2] |= ReorderOutAfs[2] << 6;
                    for (i=3; i<5; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case  SPP_MR67_MODE://AFS_6_7:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1] & 0x007FFFFF;
                    u[1] |= crccode[0] << 23;
                    u[1] |= (ReorderOutAfs[1] & 0x03800000) << 6;
                    for (i=2; i<5; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case  SPP_MR59_MODE://AFS_5_9:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1] & 0x007FFFFF;
                    u[1] |= crccode[0] << 23;
                    u[1] |= (ReorderOutAfs[1] & 0x03800000) << 6;
                    for (i=2; i<4; i++)
                    {
                        u[i] = ReorderOutAfs[i-1] >> 26;
                        u[i] |= ReorderOutAfs[i] << 6;
                    }
                    break;
                case  SPP_MR515_MODE://AFS_5_15:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1] & 0x0001FFFF;
                    u[1] |= crccode[0] << 17;
                    u[1] |= (ReorderOutAfs[1] & 0x03FE0000) << 6;
                    u[2] = ReorderOutAfs[1] >> 26;
                    u[2] |= ReorderOutAfs[2] << 6;
                    u[3] = ReorderOutAfs[2] >> 26;
                    u[3] |= ReorderOutAfs[3] << 6;
                    break;
                case  SPP_MR475_MODE://AFS_4_75:
                    u[0] = ReorderOutAfs[0];
                    u[1] = ReorderOutAfs[1] & 0x0000007F;
                    u[1] |= crccode[0] << 7;
                    u[1] |= (ReorderOutAfs[1] & 0x03FFFF80) << 6;
                    u[2] = ReorderOutAfs[1] >> 26;
                    u[2] |= ReorderOutAfs[2] << 6;
                    u[3] = ReorderOutAfs[2] >> 26;
                    break;
                default:
                    break;
            }

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,u,ConvOut);

            c[0] = g_sppIcAfsSpeech[AmrInBandData[0]];
            for (i=0; i<14; i++)
            {
                c[i] |= ConvOut[i] << 8;
                c[i+1] = ConvOut[i] >> 24;
            }

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, c);
        }
        break;

        case CS_AFS_RATSCCH:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[1]];
            c[7] = g_sppIcSidRatscch[AmrInBandData[0]] << 4;

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);
            TxBuffer[1] &= 0x00000007;
            TxBuffer[1] |= (crccode[0]) << 3;

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,TxBuffer,ConvOut);

            // Mapping coded data to c
            // IM
            c[0] |= g_sppImRatscch[0];
            for (i=1; i<7; i++)
                c[i] = g_sppImRatscch[i];
            c[7] |= g_sppImRatscch[7];
            // Convolutional code
            for (i=7; i<14; i++)
            {
                c[i] |= ConvOut[i-7] << 20;
                c[i+1] = ConvOut[i-7] >> 12;
            }
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 456, BufferOut, c);
        }
        break;

        case CS_AHS_SID_UPDATE:
        {
            // Instead of mapping on 456 bits consecutively,
            // we put the first 228 bits in 8 Dwords and
            // the last 228 bits in another 8 Dwords
            // with 28 unused bits at the end
            // c[8]: first 228 bits; for the 1st interleaving
            // c2[8]: last 228 bits; for the 2nd interleaving
            UINT32 c2[8];
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[1]];
            c2[0] = g_sppIcSidRatscch[AmrInBandData[0]];

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);
            TxBuffer[1] &= 0x00000007;
            TxBuffer[1] |= crccode[0] << 3;

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,TxBuffer,ConvOut);

            // Mapping coded data to c (1)
            // g_sppImAhsSid (repetition of "1 0000 1101")
            c[0] |= g_sppImAhsSid[0];
            for (i=1; i<8; i++)
                c[i] = g_sppImAhsSid[i];

            // Mapping coded data to c (2)
            // Convolutional code
            for (i=0; i<7; i++)
            {
                c2[i] |= ConvOut[i] << 16;
                c2[i+1] = ConvOut[i] >> 16;
            }
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
            spal_ItlvItlv(ITLV_TYPE_2B, BurstOffset, 228, BufferOut, c2);
        }
        break;

        case CS_AHS_SID_UPDATE_INH:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[1]];
            // Identification maker
            // g_sppImAfsSid: repetition of "0 1111 0010"
            for (i=0; i<7; i++)
            {
                c[i] |= g_sppImAfsSid[i] << 16;
                c[i+1] = g_sppImAfsSid[i] >> 16;
            }
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_SID_FIRST_P1:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            // Identification maker
            // g_sppImAfsSid: repetition of "0 1111 0010"
            for (i=0; i<7; i++)
            {
                c[i] |= g_sppImAfsSid[i] << 16;
                c[i+1] = g_sppImAfsSid[i] >> 16;
            }
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_SID_FIRST_P2:
        {
            UINT32 ic1[4];
            // Repetition of ic1
            ic1[0] = g_sppIcSidRatscch[AmrInBandData[0]] | (g_sppIcSidRatscch[AmrInBandData[0]] << 16);
            for (i=1; i<4; i++)
                ic1[i] = ic1[0];
            ic1[3] &= 0x0003FFFF;  // discard the last 14 bits
            // Mapping on c
            for (i=0; i<8; i++)
                c[i] = 0;
            for (i=0; i<7; i++)
            {
                k = i >> 1;
                for (j=0; j<16; j++)
                {
                    c[i] |= (ic1[k] & 1) << (j*2);
                    ic1[k] = ic1[k] >> 1;
                }
            }
            c[7] |= ic1[3] & 1;
            ic1[3] = ic1[3] >> 1;
            c[7] |= (ic1[3] & 1) << 2;
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_SID_FIRST_INH:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            // Identification maker
            // g_sppImAhsSid: repetition of "1 0000 1101"
            c[0] |= g_sppImAhsSid[0];
            for (i=1; i<8; i++)
                c[i] = g_sppImAhsSid[i];
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_ONSET:
        {
            UINT32 ic1[4];
            // Repetition of ic1
            ic1[0] = g_sppIcSidRatscch[AmrInBandData[1]] | (g_sppIcSidRatscch[AmrInBandData[1]] << 16);
            for (i=1; i<4; i++)
                ic1[i] = ic1[0];
            ic1[3] &= 0x0003FFFF;  // discard the last 14 bits
            // Mapping on c
            for (i=0; i<8; i++)
                c[i] = 0;
            for (i=0; i<7; i++)
            {
                k = i >> 1;
                for (j=0; j<16; j++)
                {
                    c[i] |= (ic1[k] & 1) << (j*2+1);
                    ic1[k] = ic1[k] >> 1;
                }
            }
            c[7] |= (ic1[3] & 1) << 1;
            ic1[3] = ic1[3] >> 1;
            c[7] |= (ic1[3] & 1) << 3;
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
        case CS_AHS_SPEECH_515:
        case CS_AHS_SPEECH_475:
        {
            // Output of reordering; number of bits varies from 244 to 95
            UINT32 *ReorderOutAhs = (UINT32*) ReorderOut;
            UINT8  *ReorderTab = (UINT8 *) (g_sppAhsReorderTables[AmrMode]);
            // Reordering according to table 9-14 in 45.003
            for (i=0; i<5; i++)
                ReorderOutAhs[i] = 0;
            LengthWord = g_sppInputLengthBitAhs[AmrMode]/32;
            ReorderBitLeft = g_sppInputLengthBitAhs[AmrMode]%32;
            for (i=0; i<LengthWord; i++)
            {
                k = i*32;
                for (j=0; j<32; j++)
                    ReorderOutAhs[i] |= (!!(TxBuffer[ReorderTab[k+j]/32] &
                                            (1<<(ReorderTab[k+j]%32)))) << j;
            }

            k = i*32;
            for (j=0; j<ReorderBitLeft; j++)
                ReorderOutAhs[i] |= (!!(TxBuffer[ReorderTab[k+j]/32] &
                                        (1<<(ReorderTab[k+j]%32)))) << j;

            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],ReorderOutAhs,crccode);

            // Insert CRC code
            switch (AmrMode)
            {
                case SPP_MR795_MODE://AHS_7_95:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1];
                    u[2] = ReorderOutAhs[2] & 0x00000007;
                    u[2] |= crccode[0] << 3;
                    u[2] |= (ReorderOutAhs[2] & 0x03FFFFF8) << 6;
                    u[3] = ReorderOutAhs[2] >> 26;
                    u[3] |= ReorderOutAhs[3] << 6;
                    u[4] = ReorderOutAhs[3] >> 26;
                    break;
                case SPP_MR74_MODE://AHS_7_4:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1] & 0x1FFFFFFF;
                    u[1] |= crccode[0] << 29;
                    u[2] = crccode[0] >> 3;
                    u[2] |= (ReorderOutAhs[1] & 0xE0000000) >> 26;
                    u[2] |= ReorderOutAhs[2] << 6;
                    u[3] = ReorderOutAhs[2] >> 26;
                    u[3] |= ReorderOutAhs[3] << 6;
                    u[4] = ReorderOutAhs[3] >> 26;
                    u[4] |= ReorderOutAhs[4] << 6;
                    break;
                case SPP_MR67_MODE://AHS_6_7:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1] & 0x007FFFFF;
                    u[1] |= crccode[0] << 23;
                    u[1] |= (ReorderOutAhs[1] & 0x03800000) << 6;
                    for (i=2; i<5; i++)
                    {
                        u[i] = ReorderOutAhs[i-1] >> 26;
                        u[i] |= ReorderOutAhs[i] << 6;
                    }
                    break;
                case SPP_MR59_MODE://AHS_5_9:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1] & 0x007FFFFF;
                    u[1] |= crccode[0] << 23;
                    u[1] |= (ReorderOutAhs[1] & 0x03800000) << 6;
                    for (i=2; i<4; i++)
                    {
                        u[i] = ReorderOutAhs[i-1] >> 26;
                        u[i] |= ReorderOutAhs[i] << 6;
                    }
                    break;
                case SPP_MR515_MODE://AHS_5_15:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1] & 0x0001FFFF;
                    u[1] |= crccode[0] << 17;
                    u[1] |= (ReorderOutAhs[1] & 0x03FE0000) << 6;
                    u[2] = ReorderOutAhs[1] >> 26;
                    u[2] |= ReorderOutAhs[2] << 6;
                    u[3] = ReorderOutAhs[2] >> 26;
                    u[3] |= ReorderOutAhs[3] << 6;
                    break;
                case SPP_MR475_MODE://AHS_4_75:
                    u[0] = ReorderOutAhs[0];
                    u[1] = ReorderOutAhs[1] & 0x0000007F;
                    u[1] |= crccode[0] << 7;
                    u[1] |= (ReorderOutAhs[1] & 0x03FFFF80) << 6;
                    u[2] = ReorderOutAhs[1] >> 26;
                    u[2] |= ReorderOutAhs[2] << 6;
                    u[3] = ReorderOutAhs[2] >> 26;
                    break;
                default:
                    break;
            }

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,u,ConvOut);

            // Coding of in-band data
            c[0] = g_sppIcAhsSpeech[AmrInBandData[0]];
            // Mapping coded bits to c
            LengthWord = (g_sppConvOutLengthBitAhs[AmrMode]+4)/32+1;
            for (i=0; i<LengthWord; i++)
            {
                c[i] |= ConvOut[i] << 4;
                c[i+1] = ConvOut[i] >> 28;
            }
            // Concatenation of Class 2
            switch (AmrMode)
            {
                case SPP_MR795_MODE://AHS_7_95:
                    c[6] = ReorderOutAhs[3] >> 27;
                    c[6] |= ReorderOutAhs[4] << 5;
                    c[7] = ReorderOutAhs[4] >> 27;
                    break;
                case SPP_MR74_MODE://AHS_7_4:
                    c[6] &= 0x000000FF;
                    c[6] |= (ReorderOutAhs[3]&0xFF000000) >> 16;
                    c[6] |= ReorderOutAhs[4] << 16;
                    c[7] = ReorderOutAhs[4] >> 16;
                    break;
                case SPP_MR67_MODE://AHS_6_7:
                    c[6] &= 0x00001FFF;
                    c[6] |= (ReorderOutAhs[3] & 0xFFFFC000) >> 2;
                    c[6] |= ReorderOutAhs[4] << 30;
                    c[7] = ReorderOutAhs[4] >> 2;
                    break;
                case SPP_MR59_MODE://AHS_5_9:
                    c[6] &= 0x000FFFFF;
                    c[6] |= (ReorderOutAhs[3] & 0xFFFFFFC0) << 14;
                    c[7] = ReorderOutAhs[3] >> 18;
                    break;
                case SPP_MR515_MODE://AHS_5_15:
                    c[6] &= 0x00FFFFFF;
                    c[6] |= (ReorderOutAhs[2] & 0xF8000000) >> 3;
                    c[6] |= ReorderOutAhs[3] << 29;
                    c[7] = ReorderOutAhs[3] >> 3;
                    break;
                case SPP_MR475_MODE://AHS_4_75:
                    c[6] &= 0x00FFFFFF;
                    c[6] |= (ReorderOutAhs[2] & 0xFFF80000) << 5;
                    c[7] = ReorderOutAhs[2] >> 27;
                    break;
                default:
                    break;
            }

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_RATSCCH_MARKER:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            // Identification maker
            // g_sppImRatscch: repetition of "100 0110 1001" (already shifted)
            c[0] |= g_sppImRatscch[0];
            for (i=1; i<8; i++)
                c[i] = g_sppImRatscch[i];

            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        case CS_AHS_RATSCCH_DATA:
        {
            // Coding of in-band data
            c[0] = g_sppIcSidRatscch[AmrInBandData[0]];
            // CRC encoding
            spal_CapEncodeCrc(g_sppCsCrcParam[coding_scheme],TxBuffer,crccode);
            TxBuffer[1] &= 0x00000007;
            TxBuffer[1] |= crccode[0] << 3;

            // Convolutional code encoding
            spal_CapConvoluate(g_sppCsConvParam[coding_scheme],PunctTable,TxBuffer,ConvOut);

            // Mapping coded data to c
            for (i=0; i<7; i++)
            {
                c[i] |= ConvOut[i] << 16;
                c[i+1] = ConvOut[i] >> 16;
            }
            // Interleaving
            spal_ItlvItlv(InterleaverType, BurstOffset, 228, BufferOut, c);
        }
        break;

        default: break;
    }
    SPP_PROFILE_FUNCTION_EXIT(spp_EncodeBlock);
}

//======================================================================
// spp_BuildNBurst
//----------------------------------------------------------------------
// Normal Burst building and mapping
//
// @param TxBuffer UINT32*. Pointer to the encoded data.
// @param Tsc UINT8. Training sequence number.
// @param StealFlag UINT8. Stealing Flags.
// @param BufferOut UINT32*. Burst output pointer.
//======================================================================
PUBLIC VOID spp_BuildNBurst(UINT32* TxBuffer,
                            UINT8 Tsc,
                            UINT8 StealFlag,
                            UINT32* BufferOut)
{
    SPP_PROFILE_FUNCTION_ENTRY(spp_BuildNBurst);
#if 0 // Old version
    // 4 guard bits, 3 tail bits, Encrypted bits(0-24 of word0)
    BufferOut[0]  = ((TxBuffer[0] & 0x01ffffff) << 7) | 0xf ;

    // Encrypted bits(25-31 of word0), Encrypted bits(0-24 of word1)
    BufferOut[1]  = ((TxBuffer[1] & 0x01ffffff) << 7) | ((TxBuffer[0] & 0xfe000000) >> 25);

    // Stealing bit 1, TsNb[0-25], Stealing bit 2, Encrypted bits(25-28 of word1)
    BufferOut[2] = ((TxBuffer[1] & 0x1e000000) << 3) | (((StealFlag>>1) & 0x1) << 27) |
                   ((g_sppGsmTscNburst[Tsc] & 0x03ffffff)<< 1) | (StealFlag & 0x1) ;

    // Encrypted bits(29-31 of word1), Encrypted bits(0-28 of word2)
    BufferOut[3] = ((TxBuffer[2] & 0x1fffffff) << 3) | ((TxBuffer[1] & 0xe0000000) >> 29) ;

    // Encrypted bits(29-31 of word2), Encrypted bits(0-17 of word3), 3 tail bits, 4 guard bits
    BufferOut[4] = (0xf<<24) | ((TxBuffer[3] & 0x0003ffff) << 3) | ((TxBuffer[2] & 0xe0000000) >> 29) ;
#else
    //     29           3
    // Word0[28:0] | 3 tails
    BufferOut[0]  = TxBuffer[0] << 3;

    //     3            1          25            3
    // TsNb[2:0] | steal1 | Word1[24:0] | Word0[31:29]
    BufferOut[1]  =     (g_sppGsmTscNburst[Tsc] << 29) |
                        ((StealFlag & 0x1)<<28) |
                        ((TxBuffer[1]&0x01ffffff) << 3) |
                        (TxBuffer[0] >> 29);

    //      1            7          1        23
    //  Word2[0] | Word1[31:25]| steal2 | TsNb[25:3]
    BufferOut[2] =  (TxBuffer[2] << 31) |
                    ((TxBuffer[1] & 0xfe000000) >> 1) |
                    (((StealFlag>>1) & 0x1) << 23) |
                    ((g_sppGsmTscNburst[Tsc] & 0x03ffffff)>> 3);

    //      1          31
    //  Word3[0] | Word2[31:1]
    BufferOut[3] = (TxBuffer[2] >> 1) | (TxBuffer[3] << 31);

    //     3            17
    //  3 tails | Word3[17:1]
    BufferOut[4] = (TxBuffer[3] & 0x0003ffff) >> 1;
#endif

    SPP_PROFILE_FUNCTION_EXIT(spp_BuildNBurst);
}


//======================================================================
// spp_Modulation
//----------------------------------------------------------------------
/// This function handles the modulation of the (multiple) burst(s)
/// constituting the full Tx window.
/// The following is true for Granite TC and Greenstone :
/// resulting modulated structure :
/// - Preamble
/// - 1st burst (156 bits)
/// - eventual 2nd burst (156 bits)
/// - eventual 3nd burst (156 bits)
/// - eventual 4nd burst (156 bits)
/// - Postamble (enough for 32 bits)
///
/// Preamble are filled with '1'
/// Postamble are filled with '1'
/// Eventual "Holes" in burst Bitmap are filled with '1' because
/// multislot transmission is continuous from GMSK point of view
///
/// For Emerald:
/// depending on modulation one symbol is either 1 or 3 bits.
/// Modulation can change for each slot. 'Holes' are not sent.
/// resulting modulated structure if burst N is the first active
/// burst or if burst N-1 is inactive:
///
/// - bstN definition for RF_IF
/// - Preamble symb                 |
/// - bstN active part (148 symb)   |
///                                 | in burst N modulation
/// then if bstN+1 is active:       |
/// - (8-guardStart) symb           |
/// - bstN+1 definition for RF_IF
/// - guardStart symb               |
/// - bstN+1 active part (148 symb) | in burst N+1 modulation
/// - ... and so on until inactive burst
///
/// else if bstN+1 is inactive:
/// - Postamble symb                | in burst N modulation
/// - End of transmit for RF_IF
///
/// @param BufferIn pointer to the mapped burst(s) data
/// @param Bitmap Tx window bitmap allocation (first active bit is
/// considered as 1st burst)
/// @param preamble Burst Mapping parameters
/// @param postamble Burst Mapping parameters
/// @param guardStart Burst Mapping parameters
/// @param BufferOut Pointer to the output sw fifo (if relevant
//         for HW implementation)
/// @param BstMod Type of the modulation, 8PSK or GMSK.
/// @param NormalBst 0:Access Burst 1:Normal Burst
/// @todo  change modulation field to a bitmap to handle multislot.
/// @todo  do a structure for Burst Mapping parameters
/// @todo  include the burst type (AB/NB) in the modulation bitmap...
//======================================================================

PUBLIC VOID spp_Modulation(UINT32* BufferIn,
                           UINT8 Bitmap,
                           UINT16 preamble,
                           UINT16 postamble,
                           UINT16 guardStart,
                           UINT32* BufferOut,
                           SPP_MODULATION_TYPE_T BstMod,
                           BOOL NormalBst)
{
#if (!CHIP_IFC_RF_TX_CHANNEL)
    // As now BufferIn only includes active part of the burst (148bits)
    // we have to cope for guard bits that used to be included :
    // The 4 at the beginning of the first burst are added via the '+4'
    // on preamble, then letting NB_BITS_PER_BUFFER to 156 allow to fill
    // the 8 bits gap between eventual consecutive bursts.
#define NB_BITS_PER_BUFFER  156
#define BUFFER_REMINDER     (32 - (NB_BITS_PER_BUFFER % 32))
#define LAST_WORD_MASK      (0xFFFFFFFFU >> BUFFER_REMINDER)

    UINT8 FifoIdx = 0;

#if 0 //debug
    UINT8 Tsc = 0;
    UINT8 StealFlag = 0;
    volatile UINT32 critical_sec;

    critical_sec = spal_IrqEnterCriticalSection();
    spal_IfcTxFifoReset();
    spal_RfIfTxFifoReset();
    spal_IrqExitCriticalSection(critical_sec);

    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xefffefff);
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xefffefff);

    // Stealing bit 1, TsNb[0-25], Stealing bit 2, Encrypted bits(25-28 of word1)
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], (0x1e000000 << 3) |
                         (((StealFlag>>1) & 0x1) << 27) |
                         ((g_sppGsmTscNburst[Tsc] & 0x03ffffff)<< 1) |
                         (StealFlag & 0x1)) ;

    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xffffffff);
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xffffffff);
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xffffffff);
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xffffffff);
    spal_IfcTxFifoSetup(BufferOut, FifoIdx);
#else
    INT32 i;
    UINT8 Lshift,Rshift,nbWords;
    UINT8 BufIdx =0;
    UINT32 LastBuf;

    volatile UINT32 critical_sec;

    // enter CS
    critical_sec = spal_IrqEnterCriticalSection();

    // Reset the modulator's bit pointer
    spal_IfcTxFifoReset();
    spal_RfIfTxFifoReset();

    // exit CS
    spal_IrqExitCriticalSection(critical_sec);


    while (!(Bitmap & 0x1))
        Bitmap >>= 1;
    if      (Bitmap & 0x8) nbWords = 21;
    else if (Bitmap & 0x4) nbWords = 16;
    else if (Bitmap & 0x2) nbWords = 11;
    else                   nbWords = 6;
    if (preamble == 32)
    {
        spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xFFFFFFFF);
        preamble = 0;
    }

    Lshift = preamble;
    Rshift = 32 - Lshift;
    LastBuf = 0xFFFFFFFF;

    // Damn' tricky loop...but does the job
    for (i=0; i < nbWords; i++)
    {
        UINT32 InBuf = (Bitmap & 1)?BufferIn[BufIdx++]:0xFFFFFFFF;
        if ((i==4)||(i==9)||(i==14)||(i==19)) // Last word of a burst
        {
            UINT8 newLshift = (Lshift + NB_BITS_PER_BUFFER) % 32;
            InBuf &= LAST_WORD_MASK;
            Bitmap >>= 1;
            if (Lshift < BUFFER_REMINDER)
            {
                UINT32 TmpBuf = (Bitmap & 1)?BufferIn[BufIdx++]:0xFFFFFFFF;

                spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],
                                     SPP_SAFE_LSHIFT(TmpBuf,newLshift) |
                                     SPP_SAFE_LSHIFT(InBuf,Lshift) |
                                     SPP_SAFE_RSHIFT(LastBuf,Rshift));
                LastBuf = TmpBuf;
                Rshift = 32 - newLshift;
                i++;
            }
            else
            {
                spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],
                                     SPP_SAFE_LSHIFT(InBuf,Lshift) |
                                     SPP_SAFE_RSHIFT(LastBuf,Rshift));
                LastBuf = InBuf;
            }
            Lshift = newLshift;
        }
        else
        {
            spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],
                                 SPP_SAFE_LSHIFT(InBuf,Lshift) |
                                 SPP_SAFE_RSHIFT(LastBuf,Rshift));
            Rshift = 32 - Lshift;
            LastBuf = InBuf;
        }
    }
    // Cope for large postamble
    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], 0xFFFFFFFF);
    // Setup IFC transfer
    spal_IfcTxFifoSetup(BufferOut, FifoIdx);
#endif

#else // (CHIP_IFC_RF_TX_CHANNEL)

#define STUFFING_WORD   (0xFFFFFFFFU)
#define PREV_SLOT(n) ((n) & 0x1)
#define CURR_SLOT(n) ((n) & 0x2)
#define NEXT_SLOT(n) ((n) & 0x4)

#if 0 // Debug
    UINT8 FifoIdx = 0;
    volatile UINT32 critical_sec;

    // enter CS
    critical_sec = spal_IrqEnterCriticalSection();
    // Reset/flush Rf_If and its Ifc channel
    spal_IfcTxFifoReset();
    spal_RfIfTxFifoReset();
    // exit CS
    spal_IrqExitCriticalSection(critical_sec);
    spal_RfIfTxFifoSetBurst(0, 156, BstMod, 1);
    BufferOut[FifoIdx++] = 0x3FFFFFFF;
    BufferOut[FifoIdx++] = 0x2AAAAAAA;
    BufferOut[FifoIdx++] = 0x30f0f0f0;
    BufferOut[FifoIdx++] = 0x3FFFFFFF;
    BufferOut[FifoIdx++] = 0x3FFFFFFF;
    BufferOut[FifoIdx++] = 0x3FFFFFFF;
    spal_IfcTxFifoSetup(BufferOut, FifoIdx);
#else
    INT32 remBits,bst;
    UINT8 Lshift,Rshift;
    UINT8 BufIdx =0;
    UINT32 LastBuf;
    UINT8 activeBstNb = 0;
    UINT8 FifoIdx = 0;
    //volatile UINT32* pBufferOut = BufferOut;
    volatile UINT32 critical_sec;

    // enter CS
    critical_sec = spal_IrqEnterCriticalSection();
    // Reset/flush Rf_If and its Ifc channel
    spal_IfcTxFifoReset();
    spal_RfIfTxFifoReset();
    // exit CS
    spal_IrqExitCriticalSection(critical_sec);

    Bitmap  <<= 1;
    for (bst = 0; bst < 4; bst++) // loop on bursts
    {
        if (CURR_SLOT(Bitmap)) // Active slot
        {
            UINT16 bstSize = (NormalBst)?148:88;
            UINT32 InBuf;

            // derive pre/post-ambles sizes from bitmap allocation
            UINT8 pre  = (!PREV_SLOT(Bitmap))?preamble:guardStart;
            UINT8 post = (!NEXT_SLOT(Bitmap))?postamble:(8-guardStart);

            // Up to here bstSize is in symbols units
            spal_RfIfTxFifoSetBurst(activeBstNb++,
                                    pre + bstSize + post,
                                    BstMod,
                                    !NEXT_SLOT(Bitmap));

            // Translate into bits
            if ( BstMod == SPP_8PSK_MOD)
            {
                bstSize *= 3;
                pre     *= 3;
                post    *= 3;
            }

            while (pre  >= 30) // Pop large preambles
            {
                spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++], STUFFING_WORD);
                pre  -= 30;
            }

            // Loop init
            remBits = bstSize + pre;
            Lshift = pre;
            Rshift = 32 - Lshift;
            LastBuf = STUFFING_WORD;

            // this loop fills all the burst but the last word of data
            for ( ; remBits > 30 ; remBits-=30 )
            {
                if (Lshift >= 30) // enough bits, skip the reload
                {
                    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],
                                         SPP_SAFE_RSHIFT(LastBuf,Rshift));
                    Lshift -= 30;
                    Rshift += 30;
                }
                else
                {
                    InBuf = BufferIn[BufIdx++];
                    spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],
                                         SPP_SAFE_LSHIFT(InBuf,Lshift) |
                                         SPP_SAFE_RSHIFT(LastBuf,Rshift));
                    Lshift += 2;
                    Rshift = 32 - Lshift;
                    LastBuf = InBuf;
                }
            }

            // Handle Last word of data and postambles/guardEnd bits
            InBuf = (Lshift >= remBits) ? 0 : // All data bits already in LastBuf
                    BufferIn[BufIdx++];

            remBits += post; // add postamble
            while (remBits > 0)
            {
                UINT32 data;

                data = SPP_SAFE_LSHIFT(InBuf,Lshift) |
                       SPP_SAFE_RSHIFT(LastBuf,Rshift);

                if(remBits > post)
                    data |= SPP_SAFE_LSHIFT(STUFFING_WORD, (remBits - post));

                spal_RfIfTxFifoWrite(&BufferOut[FifoIdx++],data);

                Lshift += 2;
                Rshift = 32 - Lshift;
                LastBuf = STUFFING_WORD;
                InBuf = STUFFING_WORD;
                remBits -= 30;
            }

        } // active slot
        Bitmap >>= 1;
    } // loop on bursts
    // Setup IFC transfer
    spal_IfcTxFifoSetup(BufferOut, FifoIdx);
#endif

#endif // (!CHIP_IFC_RF_TX_CHANNEL)
}

