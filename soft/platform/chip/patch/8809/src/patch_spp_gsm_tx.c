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

#include "spal_mem.h"

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

#ifdef ENABLE_PATCH_SPP_BUILDNBURST

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
PUBLIC VOID SPAL_FUNC_INTERNAL patch_spp_BuildNBurst(UINT32* TxBuffer,
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

#endif // ENABLE_PATCH_SPP_BUILDNBURST


#ifdef ENABLE_PATCH_SPP_MODULATION

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

PUBLIC VOID SPAL_FUNC_INTERNAL patch_spp_Modulation(UINT32* BufferIn,
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
    //volatile UINT32* pBufferOut = BufferOut;
    volatile UINT32 critical_sec;

    // enter CS
    critical_sec = spal_IrqEnterCriticalSection();
    // Reset/flush Rf_If and its Ifc channel
    spal_IfcTxFifoReset();
    spal_RfIfTxFifoReset();
    // exit CS
    spal_IrqExitCriticalSection(critical_sec);
    spal_RfIfTxFifoSetBurst(0, 112, BstMod, 1);
    pBufferOut[FifoIdx++] = 0xAAAAAAAA;
    pBufferOut[FifoIdx++] = 0xf0f0f0f0;
    pBufferOut[FifoIdx++] = 0xFFFFFFFF;
    pBufferOut[FifoIdx++] = 0xFFFFFFFF;
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
            InBuf = (Lshift >= remBits)? 0 : // All data bits already in LastBuf
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

#endif // ENABLE_PATCH_SPP_MODULATION

