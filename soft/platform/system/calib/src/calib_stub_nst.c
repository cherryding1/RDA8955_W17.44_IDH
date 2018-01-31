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

#ifdef NUMBER_OF_SIM
#undef NUMBER_OF_SIM
#endif
#define NUMBER_OF_SIM 1

#include "cs_types.h"
#include "stdio.h"
#include "chip_id.h"
#include "string.h"

#include "calib_m.h"

#include "rfd_defs.h"
#include "rfd_xcv.h"
#include "rfd_pa.h"

#include "calibp_debug.h"
#include "calibp_stub.h"

#include "sxs_io.h"
#include "sxs_rmt.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"
#include "myl1.h"

#include "hal_clk.h"
#include "pal_gsm.h"
#include "calib_stub_nst.h"
#include "itf_calib.h"
// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  extern function.
// =============================================================================

extern VOID sxr_Sleep (UINT32 Period);

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#define CALIB_UNCACHED_BSS __attribute__((section(".ucbss")))

static const char *ACQUIRE_FSM_STAT_STRING[] =
{
    FOREACH_FSM_STATE(GENERATE_STRING)
};

PROTECTED VOLATILE CALIB_STUB_NST_CTX_T CALIB_UNCACHED_BSS g_calibStubGlobalNstCtx;

PROTECTED VOLATILE CALIB_STUB_NST_CTX_T *  CALIB_UNCACHED_BSS  g_calibStubGlobalNstCtxPtr;

const UINT8 sl1_TchFSchd [26] =
{
    /*  0 */  SL1_BST(1),
    /*  1 */  SL1_BST(2),
    /*  2 */  SL1_BST(3),
    /*  3 */  SL1_BST(0) | SL1_EC_FA,
    /*  4 */  SL1_BST(1) | SL1_DC_TC | SL1_DC_FA,
    /*  5 */  SL1_BST(2),
    /*  6 */  SL1_BST(3),
    /*  7 */  SL1_BST(0) | SL1_EC_FA,
    /*  8 */  SL1_BST(1) | SL1_DC_TC | SL1_DC_FA,
    /*  9 */  SL1_BST(2) | SL1_EC_SA,
    /* 10 */  SL1_BST(3),
    /* 11 */  SL1_BST(0),
    /* 12 */  SL1_BST(0) | SL1_DC_TC | SL1_DC_FA | SL1_EC_FA,
    /* 13 */  SL1_BST(1) | SL1_DC_SA,
    /* 14 */  SL1_BST(2),
    /* 15 */  SL1_BST(3),
    /* 16 */  SL1_BST(0) | SL1_EC_FA,
    /* 17 */  SL1_BST(1) | SL1_DC_TC | SL1_DC_FA,
    /* 18 */  SL1_BST(2),
    /* 19 */  SL1_BST(3),
    /* 20 */  SL1_BST(0) | SL1_EC_FA,
    /* 21 */  SL1_BST(1) | SL1_DC_TC | SL1_DC_FA,
    /* 22 */  SL1_BST(2) /* | L1_EC_FA */, // TBC
    /* 23 */  SL1_BST(3),
    /* 24 */  0,
    /* 25 */  SL1_BST(0) | SL1_DC_TC | SL1_DC_FA | SL1_EC_FA,
};

/* Position of the downlink SUB frames (Sacch + SID) for the different Tch */
/* configurations expressed in FN mod 104 with an anticipation of one.     */
/* Decoding positions of SID frames are indicated with define L1_SID_BLOCK */
/* with an anticipation of at least one.                                   */
/* Decoding position of Sacch which depends of allocated slot is not       */
/* indicated.                                                              */
#define L1_SID_BLOCK (1 << 7)
const u8 sl1_TchSubFrames [3][16] =
{
    // Sa 11 37 63 89 - 24 50 76 102
    { 11, 24, 37, 50, 51, 52, 53, 54, 55, 56, 57, 58 | L1_SID_BLOCK, 63, 76, 89, 102},                  // Tch/F
    {103,  1,  3,  5 | L1_SID_BLOCK, 11, 24, 37, 50, 51, 53, 55, 57 | L1_SID_BLOCK, 63,  76, 89, 102},  // Tch/H0
    { 11, 13, 15, 17, 19 | L1_SID_BLOCK, 24, 37, 50, 63, 65, 67, 69, 71 | L1_SID_BLOCK, 76, 89, 102}   // Tch/H1
};

/* Gsm L2 filling frame. */
u8 sl1_L2FillFrame [25] =
{
    0,    0,    0x01, 0x03, 0x49, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B
};
//  Pointer to the context, uncached.
EXPORT CONST UINT16 g_sppMatrixFrInv[260];
/// DSP/EQUALIZATION READY.
EXPORT VOLATILE UINT8 g_calibEquRdyFlag;

// =============================================================================
//  FUNCTIONS
// =============================================================================
GSM_RFBAND_T palGetBand(UINT16 arfcn);

#ifndef ED_REVERSED_BITS
#define POS(bit) (1 << (7-(bit & 7)))
#else
#define POS(bit) (1 << (bit & 7))
#endif
#define SRC (*(((const unsigned char*) Source)+(SourceOffset >> 3)))
#define DST (*(((unsigned char*)Destination)+(DestOffset >> 3)))


/*------------------------------------------------------------------------------
  This function copies 'Count' less significant bits of 'Source' integer to
  'Destination' ad offset 'Offset'.
  ------------------------------------------------------------------------------*/
PRIVATE VOID EDIntToBits (void* Destination, long DestOffset, int Source, int Count)
{
#ifndef ED_REVERSED_INTEGER
    int i;
    for (i=Count-1; i>=0; i--)
    {
        if ((Source & (1 << i)) != 0) DST |= POS(DestOffset);
        else DST &= ~POS(DestOffset);
        DestOffset++;
    }
#else
    int i;
    DestOffset+=Count-1;
    for (i=Count-1; i>=0; i--)
    {
        if ((Source & (1 << i)) != 0) DST |= POS(DestOffset);
        else DST &= ~POS(DestOffset);
        DestOffset--;
    }
#endif
}

// =============================================================================
// InitPrbs
// -----------------------------------------------------------------------------
/// PRBS array initialize
///
///@param poly: The polynomial for the Pseudo Random Binary Sequence(PRBS) seed initialize
/// could be:
/// G(15) = X^15+X^14+1(6000h)
/// G(9) = X^9+X^5+1(110h)
///@param seed:Initial state of poly register
///@param len: Polynomial length
/// @param invert: Invert PN sequence
// =============================================================================
PRIVATE VOID InitPrbs(UINT16 poly, UINT16 seed, UINT16 len, UINT8 invert)
{
    UINT16 L=(1<<len)-1;
    UINT32 i,j;
    UINT16 registers;
    UINT16 tmp,sum;

    if(invert)
    {
        seed ^= L;
    }
    g_calibStubGlobalNstCtx.prbs.array = (UINT16*)sxr_HMalloc((1<<len)*sizeof(registers));
    g_calibStubGlobalNstCtx.prbs.array2pos = (UINT16*)sxr_HMalloc((1<<len)*sizeof(registers));
    g_calibStubGlobalNstCtx.prbs.RegLen = len;
    g_calibStubGlobalNstCtx.prbs.poly = poly;

    registers = seed & L;
    for(i=0; i<L; i++)
    {

        g_calibStubGlobalNstCtx.prbs.array[i] = registers;
        g_calibStubGlobalNstCtx.prbs.array2pos[registers] = i;
        tmp = poly&registers;

        if(len == 15)
        {
            sum = ((tmp>>14)&1)+((tmp>>13)&1)+invert;
        }
        else if(len == 9)
        {
            sum = ((tmp>>8)&1)+((tmp>>4)&1)+invert;
        }
        else
        {
            sum=0;
            for(j=0; j<len; j++)
            {
                sum += (tmp>>j)&1;
            }
            sum += invert;
        }
        sum %= 2;
        registers <<= 1;
        registers |= sum;

        registers &= L;
    }

}

// =============================================================================
// GenPrbs
// -----------------------------------------------------------------------------
/// Prbs sequence generator
///
/// @param prbs: PRBS context
/// @param output: Pointer to PN sequence output
/// @param reg: Register initial value
/// @param offset: Offset in the input sequence
/// @param period: Length of PN sequence
// =============================================================================
PRIVATE VOID GenPrbs(prbs_ctx_t *prbs, UINT8 *output, UINT16 reg,  UINT32 offset, UINT32 period)
{
    UINT32 i, pos=0;
    INT32 modPos;
    UINT16 L=(1<<prbs->RegLen)-1;

    UINT16 *array = (UINT16 *)HAL_SYS_GET_CACHED_ADDR(prbs->array);

    pos = prbs->array2pos[reg]+1;

    for(i=0; i<period; i++)
    {
        if(offset+i < period)
        {
            modPos = pos++%L;
            output[offset+i] = array[modPos]&1;
        }
        else
        {
            modPos = pos++ - period;
            modPos = modPos<0?modPos+L:modPos;
            modPos %= L;
            output[offset+i-period] = array[modPos]&1;
        }
    }

    modPos = pos-offset-1;
    modPos = modPos<0?modPos+L:modPos;
    modPos %= L;
    prbs->PosRtAligned = modPos;
    prbs->RegRtAligned = array[modPos];

}


PRIVATE UINT32 computeBer(UINT8 *s1, UINT8*s2, UINT32 n)
{
    UINT32  Cnt=0;
    if (n != 0)
    {
        do
        {
            if (*s1++ != *s2++)
                Cnt++;
        }
        while (--n != 0);
    }
    return (Cnt);

}

// =============================================================================
// acquire_fsm
// -----------------------------------------------------------------------------
/// acquire capture fsm, verify data on the seed, if no bit error was detected
/// on successive 2 poly length, and no more than 1 bit error on the third poly
/// length, announce ACQUIRE_DONE in fsm state.
///
/// @param prbs: PRBS context
/// @param seed: Register initial value
/// @param input: Pointer to input sequence
/// @param offset: Offset in the input sequence
/// @param fsm: Acquire fsm
/// be copied.
// =============================================================================
PRIVATE VOID acquire_fsm(prbs_ctx_t *prbs, UINT16 seed,  UINT8 *input, UINT32 offset, acquire_fsm_t *fsm)
{
    UINT32 Ber;
    UINT8 PrbsOut[16]; // Max register length SHOULD be below 15
    UINT16 L=(1<<prbs->RegLen)-1;
    PRIVATE ACQUIRE_FSM_STAT_T acquire_state = ACQUIRE_INIT;
    PRIVATE UINT32 offsetInit, seedInit;
    PRIVATE UINT32 acquireTimeOut= 0; //bandwidth control

    if((acquire_state != ACQUIRE_DONE) &&
            (((seed == 0) && (prbs->invert==0)) ||
             ((seed == L) && (prbs->invert)))
      )
    {
        char trcBuf[64];
        snprintf(trcBuf, sizeof(trcBuf), "acq[%s, seed %x]=>ACQUIRE_ABORT", ACQUIRE_FSM_STAT_STRING[acquire_state],seed);
        SXS_TRACE(_L1S|TNB_ARG(0),trcBuf);
        acquire_state = ACQUIRE_ABORT;
    }

    fsm->state = acquire_state;

    switch(acquire_state)
    {
        case ACQUIRE_INIT:

            offsetInit = offset;
            seedInit = seed;
            GenPrbs(prbs,PrbsOut, seed, 0, prbs->RegLen); //offset=0
            Ber = computeBer(PrbsOut, input+offset, prbs->RegLen);
            if(Ber == 0)
            {
                SXS_TRACE(_L1S|TNB_ARG(0), "acq[ACQUIRE_INIT,Ber 0]=>ACQUIRE_VERIFY");
                acquire_state = ACQUIRE_VERIFY;
            }
            if(acquireTimeOut >= prbs->acq_timeout)
            {
                acquire_state = ACQUIRE_ABORT;
                SXS_TRACE(_L1S|TNB_ARG(1), "acq[ACQUIRE_INIT,Retry %d]=>ACQUIRE_ABORT", acquireTimeOut);
            }
            acquireTimeOut++;
            break;

        case ACQUIRE_VERIFY:

            GenPrbs(prbs, PrbsOut, seed, 0, prbs->RegLen);
            Ber = computeBer(PrbsOut, input+offset, prbs->RegLen);

            if(Ber <= 1)
            {
                SXS_TRACE(_L1S|TNB_ARG(1), "acq[ACQUIRE_VERIFY,Ber %d]=>ACQUIRE_DONE", Ber);
                acquire_state = ACQUIRE_DONE;
            }
            else
            {
                SXS_TRACE(_L1S|TNB_ARG(1), "acq[ACQUIRE_VERIFY,Ber %d]=>ACQUIRE_INIT", Ber);
                acquire_state = ACQUIRE_INIT;
            }
            break;

        case ACQUIRE_DONE:
            fsm->seed = seedInit;
            fsm->offset = offsetInit;
            SXS_TRACE(_L1S|TNB_ARG(1), "acq[ACQUIRE_DONE,Retry %d]=>ACQUIRE_INIT",acquireTimeOut);
            acquireTimeOut = 0;
            acquire_state = ACQUIRE_INIT;
            break;

        case ACQUIRE_ABORT:
            SXS_TRACE(_L1S|TNB_ARG(1), "acq[ACQUIRE_ABORT,Retry %d]=>ACQUIRE_INIT",acquireTimeOut);
            acquireTimeOut = 0;
            acquire_state = ACQUIRE_INIT;
            break;
    }

}

// =============================================================================
// acquire_run
// -----------------------------------------------------------------------------
/// run acquire capture block by block, return acquire fsm state.If ACQUIRE_DONE step is reached, Bit error is calculated.
///
/// @param input: Pointer to input sequence
/// @param len: Input sequence length
/// @param Ber: Bit error
///
// =============================================================================
PRIVATE ACQUIRE_FSM_STAT_T acquire_run(UINT8 *input, UINT32 len, UINT32 *Ber)
{
    // Big tables for FR/EFR
    UINT32 i;
    UINT16 seed=0;
    UINT16 period = len; // TODO: FS:260, HS:112
    UINT32 offset = g_calibStubGlobalNstCtx.prbs.RegLen;
    acquire_fsm_t fsm= {.state=ACQUIRE_INIT};

    while((offset + g_calibStubGlobalNstCtx.prbs.RegLen) < period)
    {

        for(i=0; i<g_calibStubGlobalNstCtx.prbs.RegLen; i++)
        {
            seed |= input[offset-1-i]<<i;
        }

        acquire_fsm((prbs_ctx_t *)&g_calibStubGlobalNstCtx.prbs, seed, input, offset, (acquire_fsm_t*)&fsm);

        if((fsm.state == ACQUIRE_DONE) ||(fsm.state == ACQUIRE_ABORT))
        {
            break;
        }

        offset += g_calibStubGlobalNstCtx.prbs.RegLen;
        seed = 0;
    }

    if(fsm.state == ACQUIRE_DONE)
    {
        //compute BER
        GenPrbs((prbs_ctx_t*)&g_calibStubGlobalNstCtx.prbs, (UINT8*)g_calibStubGlobalNstCtx.PrbsOut, fsm.seed, fsm.offset, period);
        *Ber = computeBer(input, (UINT8*)g_calibStubGlobalNstCtx.PrbsOut,period);
    }
    else if(fsm.state != ACQUIRE_ABORT)
    {
        acquire_fsm((prbs_ctx_t *)&g_calibStubGlobalNstCtx.prbs, 0, input, offset, (acquire_fsm_t*)&fsm);//reset fsm with seed 0
    }

    return fsm.state;
}

// =============================================================================
// Ber_Caculate
// -----------------------------------------------------------------------------
///
/// Bit error calulator in BER_CACULATION_STATE state.
/// @param input: Pointer to input sequence
/// @param len: Input sequence length
/// @param Ber: Bit error
///
// =============================================================================
PRIVATE VOID Ber_Caculate(UINT8 *input, UINT32 len, UINT32 *Ber)
{
    UINT16 period = len; // TODO: FS:260, HS:112
    GenPrbs((prbs_ctx_t*)&g_calibStubGlobalNstCtx.prbs, (UINT8*)g_calibStubGlobalNstCtx.PrbsOut, g_calibStubGlobalNstCtx.prbs.RegRtAligned, 0, period);
    *Ber = computeBer(input, (UINT8*)g_calibStubGlobalNstCtx.PrbsOut,period);
}



PRIVATE UINT8 sl1_Power (UINT16 Arfcn)
{
    u32 i;
    static pal_RxWin_t MonWin;
    static u16 PwrSum = 0;
    static u16 FrameCnt = 0;
    static u8  NbMeas = 0;

    switch (FrameCnt++)
    {
        case 0:
            /* Wake up RF. */
            pal_RfWakeUp ();
            break;
        case 1:
            break;
        /* Program Max number of monitoring windows in the next frame. */
        case 2:
        case 3:
            for (i=0; i<PAL_NB_MAX_WIN_PER_FRAME; i++)
            {
                MonWin.Arfcn  = Arfcn;
                MonWin.Start  = (u16)i * (PAL_PLL_LOCK_TIME + PAL_MON_DURATION);
                MonWin.WinIdx = (u8)i;
                MonWin.Gain   = (u8)PAL_AGC;

                pal_SetMonRxWin (&MonWin);
            }
            break;

        case 4:
        case 5:
            /* Get monitoring results. */
        {
            u8 Pwr;

            for (i=0; i<PAL_NB_MAX_WIN_PER_FRAME; i++)
            {
                MonWin.Arfcn  = Arfcn;
                MonWin.Start  = (u16)i * (PAL_PLL_LOCK_TIME + PAL_MON_DURATION);
                MonWin.WinIdx = (u8)i;

                Pwr = pal_GetMonResult (&MonWin);
                PwrSum += Pwr;
                NbMeas++;
                sxs_fprintf (_L1S|TSTDOUT|TNB_ARG(2), "Power measure is -%i dBm for 0x%x", Pwr, MonWin.Arfcn);
            }
        }
        break;

        case 6:
        {
            u8 Agc=PwrSum / NbMeas;
            FrameCnt = 0;
            NbMeas = 0;
            PwrSum = 0;
            sxs_fprintf (_L1S|TSTDOUT|TNB_ARG(2), "Average Power measure is -%i dBm for 0x%x",Agc, MonWin.Arfcn);
            return Agc;
        }
        break;

        default:
            break;
    }

    return 0;

}


PRIVATE VOID sl1_PowerReq (GSM_RFBAND_T band)
{
    UINT32 i;
    g_calibStubGlobalNstCtxPtr->powerReq.Nb = 0;
    g_calibStubGlobalNstCtxPtr->powerReq.Index = 0;
    if((band == GSM_BAND_PCS1900) || (band == GSM_BAND_DCS1800))
    {
        for(i=g_calibStubGlobalNstCtxPtr->powerReq.Nb; i<g_calibStubGlobalNstCtxPtr->powerReq.Nb+298; i++)
        {
            g_calibStubGlobalNstCtxPtr->powerReq.List[i].Arfcn = (UINT16)(i - g_calibStubGlobalNstCtxPtr->powerReq.Nb + 512);
        }
        g_calibStubGlobalNstCtxPtr->powerReq.Nb += 298;

    }

    if(band == GSM_BAND_GSM900)
    {
        for(i=g_calibStubGlobalNstCtxPtr->powerReq.Nb; i<g_calibStubGlobalNstCtxPtr->powerReq.Nb+124; i++)
        {
            g_calibStubGlobalNstCtxPtr->powerReq.List[i].Arfcn = (UINT16)(i - g_calibStubGlobalNstCtxPtr->powerReq.Nb + 1);
        }
        g_calibStubGlobalNstCtxPtr->powerReq.Nb += 124;
    }

}


PRIVATE VOID sl1_MonitorReq(UINT16 Arfcn)
{
    g_calibStubGlobalNstCtxPtr->powerReq.List[0].Arfcn= Arfcn;
    g_calibStubGlobalNstCtxPtr->powerReq.Nb = 1;
    g_calibStubGlobalNstCtxPtr->powerReq.Index = 0;
}


PRIVATE VOID sl1_GetNBEqualisationResult (u8 MeasType)
{
#define TAB_DEPTH 8
    PRIVATE UINT16 snR_tab[TAB_DEPTH];
    UINT32 snR_calc;
    PRIVATE UINT8 Power_tab[TAB_DEPTH];
    UINT32 Power_calc;

    PRIVATE INT32 tab_index = 0;
    PRIVATE BOOL tab_full = FALSE;
    INT32 i;

    pal_NBurstResult_t NBurstResult;

    /* Equalisation results should be ready. */
    while (g_calibEquRdyFlag == FALSE)
    {
    }
    pal_GetNBurstResult(0, g_calibStubGlobalCtxPtr->txArfcn, &NBurstResult);

    Power_tab[tab_index]=NBurstResult.Pwr;
    snR_tab[tab_index]=NBurstResult.Snr;
    if(tab_full)
    {
        snR_calc = 0;
        Power_calc = 0;
        for(i=0; i<TAB_DEPTH; i++)
        {
            snR_calc += snR_tab[i];
            Power_calc += Power_tab[i];
        }
        snR_calc /= TAB_DEPTH;
        Power_calc /= TAB_DEPTH;
        g_calibStubGlobalCtxPtr->nbPower = (UINT8)Power_calc;
        g_calibStubGlobalCtxPtr->snR = (UINT16)snR_calc;
    }
    else
    {
        g_calibStubGlobalCtxPtr->nbPower = (UINT8)NBurstResult.Pwr;
        g_calibStubGlobalCtxPtr->snR = (UINT16)NBurstResult.Snr;
    }

    tab_index++;
    if (tab_index == TAB_DEPTH)
    {
        tab_index = 0;
        tab_full = TRUE;
    }

    g_calibStubGlobalCtxPtr->pFactor += NBurstResult.Snr;
    g_calibStubGlobalCtxPtr->tOf  += (NBurstResult.TOf * NBurstResult.Snr);
    g_calibStubGlobalCtxPtr->fOf  += (NBurstResult.FOf * NBurstResult.Snr);

    if (g_calibStubGlobalCtxPtr->pFactor >= PAL_SNR_FACTOR_THRES)
    {
        int tof=g_calibStubGlobalCtxPtr->tOf / g_calibStubGlobalCtxPtr->pFactor;
#define PAL_TIME_SYNC_MARGIN            20
        tof = (ABS(tof) <= (PAL_TIME_SYNC_MARGIN - 1)) ? tof:tof<0?-(PAL_TIME_SYNC_MARGIN - 1) :(PAL_TIME_SYNC_MARGIN - 1) ;
        pal_TimeTuning((INT8)(tof));
        pal_FrequencyTuning((INT16)(g_calibStubGlobalCtxPtr->fOf /
                                    g_calibStubGlobalCtxPtr->pFactor),
                            g_calibStubLocalCtx.monArfcn);

        g_calibStubGlobalCtxPtr->pFactor = 0;
        g_calibStubGlobalCtxPtr->tOf  = 0;
        g_calibStubGlobalCtxPtr->fOf  = 0;
    }

    /* Take burst measurement into account when required. */
    if ((MeasType & SL1_GET_MEAS))
    {
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumul += NBurstResult.Pwr;
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeas++;

    }

    if (MeasType & SL1_MEAS_SUB)
    {
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumulSub += NBurstResult.Pwr;
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeasSub++;

    }

}



PRIVATE VOID sl1_GetNBlockResult(pal_NBurstRxWin_t *NBurstRxWin, u8* bfi, u8 MeasType)
{
    pal_NBlockResult_t NBlockResult;

    pal_GetNBlockResult(NBurstRxWin->BufIdx, 0, &NBlockResult);
    g_calibStubGlobalCtxPtr->bitError = NBlockResult.BitError;
    *bfi=NBlockResult.Bfi;
    if (MeasType & SL1_GET_MEAS)
    {
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumul += NBlockResult.BitError;
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBit      += NBlockResult.BitTotal;
    }
    if (MeasType & SL1_MEAS_SUB)
    {
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumulSub += NBlockResult.BitError;
        g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBitSub      += NBlockResult.BitTotal;
    }
}

/*
   ==============================================================================
Function   : sl1_RxQual
----------------------------------------------------------------------------
Scope      : Compute the RxQual value.
Parameters : Number of erronous bits for the considered period.
Number total of bits for the considered period.
Return     : RxQual value.
==============================================================================
*/
PRIVATE u8 sl1_RxQual (u32 NbErrorBits, u32 NbBitsTotal)
{
    if (NbBitsTotal != 0)
    {
        u32 Ratio  = (NbErrorBits << 10) / NbBitsTotal;

        u8  RxQual = (u8)(Ratio >= ((128 << 10) / 1000) ? 7 :
                          Ratio >= (( 64 << 10) / 1000) ? 6 :
                          Ratio >= (( 32 << 10) / 1000) ? 5 :
                          Ratio >= (( 16 << 10) / 1000) ? 4 :
                          Ratio >= ((  8 << 10) / 1000) ? 3 :
                          Ratio >= ((  4 << 10) / 1000) ? 2 :
                          Ratio >= ((  2 << 10) / 1000) ? 1 :
                          0);
        return RxQual;
    }

    return 0;

}

/*
   ==============================================================================
Function   :
----------------------------------------------------------------------------
Scope      :
Parameters :
Return     : None.
==============================================================================
*/
PRIVATE VOID sl1_MeasReport()
{
    u8   Rla = 110;

    if (g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeas)
    {
        Rla = g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumul / g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeas;
    }

    g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxLevFull = Rla >= 110 ? 0 :
            Rla <= 47 ? 63 :
            110 - Rla;

    if (g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeasSub)
    {
        Rla = g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumulSub / g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeasSub;
    }

    g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxLevSub = Rla >= 110 ? 0 :
            Rla <= 47 ? 63 :
            110 - Rla;

    g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxQualFull =
        sl1_RxQual (g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumul,g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBit);

    g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxQualSub  =
        sl1_RxQual (g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumulSub, g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBitSub);

    g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBit         = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumul    = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeas           = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.NbMeasSub        = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumul        = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.MeasCumulSub     = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.TotalBitSub      = 0;
    g_calibStubGlobalNstCtxPtr->ServMeasCfg.WrongBitCumulSub = 0;
}

/*
   ================================================================================
Function   : sl1_DoNormReport
--------------------------------------------------------------------------------

Scope      : Prepare and Send the Meas Report message to the Network
Parameters :
Return     :
================================================================================
*/
PRIVATE VOID sl1_DoNormReport(u8* Data)
{
    u8        * Buffer     ;
    int         CurrOfs    ;   // Do not Change Name cod reserved
    u8        * RadioData  ;
    RadioData = &Data[2];
    RadioData [3] = 6;// RR_PD
    RadioData [4] = 0x15;// Meas Report Type
    memset (&RadioData[5], 0, 16);

    Buffer  = &RadioData[5]  ;
    CurrOfs = 0              ;
    /*
     ** Ieis of the measurment report msg
     */
    PUSH_INT (0, 1)  ; // BaInd
    PUSH_INT (0, 1)  ; // Dtx Used
    PUSH_INT (g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxLevFull,6) ; // RxLevFull
    PUSH_INT (0, 1) ; // 3G Ba Used

    PUSH_INT (0, 1)  ; // Meas Valid
    PUSH_INT (g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxLevSub, 6) ; // RxLevSub
    PUSH_INT (0, 1)   ; // Spare
    PUSH_INT (g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxQualFull, 3); // RxQualFull
    PUSH_INT (g_calibStubGlobalNstCtxPtr->ServMeasCfg.RxQualSub, 3); // RxQualSub
    PUSH_INT (0, 3);  // Number of Cells

}



PRIVATE UINT8 calib_StubNstChangeState(CALIB_NST_STAT_T state)
{
    switch(g_calibStubGlobalNstCtxPtr->NstState)
    {
        case NST_NO_STATE:
            if(state == NST_SYNC_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_IDLE_STATE;
            }
            else if(state == NST_SIMU_LOG_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_SIMU_LOG_STATE;
            }
            break;

        case NST_RX_STATE:
            if(state == NST_ACQUIRE_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_ACQUIRE_STATE;
                break;
            }
            else if(state == NST_DISCONN_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_DISCONN_STATE;
                break;
            }
        //don't break here
        case NST_BER_CACULATION_STATE:
            if(state == NST_ACQUIRE_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_ACQUIRE_STATE;
                break;
            }
            else if(state == NST_DISCONN_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_DISCONN_STATE;
                break;
            }
        //don't break here
        case NST_ACQUIRE_STATE:

            if(state == NST_SYNC_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_STOP_RX_SYNC_STATE;
            }
            else if(state == NST_DISCONN_STATE)
            {
                g_calibStubGlobalNstCtxPtr->NstState = NST_DISCONN_STATE;
                break;
            }
            break;

    }
    return g_calibStubGlobalNstCtxPtr->NstState - state;
}

/*
   ==============================================================================
Function   :calib_StubNstProcess
----------------------------------------------------------------------------
Scope      :general nst processing stub
Parameters :
Return     : None.
==============================================================================
*/
VOID calib_StubNstProcess()
{
    PRIVATE pal_NBurstRxWin_t NBurstRxWin;
    PRIVATE  pal_TxWin_t TxWin;
    PRIVATE UINT8 NBBurstMap = 0;
    PRIVATE UINT8 syncFailCnt = 0, rxDoneFlag=0;
    PRIVATE UINT8 bfiCnt=0;
    PRIVATE UINT32 state=0xff;
    PRIVATE UINT8  SubFrameBitMap = 0, SubFrameIdx = 0;// Index in TchSubFrame array
    UINT8  SubBlock    = FALSE; // Boolean indicating if the block to be decoded is a SID block
    UINT8  SubFrameIdxMax;      // Number of Sub frames / multiframe 104.
    const UINT8 *TchSubFrames;
    pal_NBlockDesc_t NBlockDesc;
    pal_GsmCounters_t GsmCounters;
    const UINT8 *TchSchd = sl1_TchFSchd;
    UINT16  RFn;
    UINT8  C104;
    UINT8  SaIdx;
    UINT8 T2;
    UINT32 Tof;
    static UINT32 OldBer = 0;
    UINT32 i, j, k, result;
    UINT32 *pRxBuffer;
    UINT8 *NstState = (UINT8 *)&g_calibStubGlobalNstCtxPtr->NstState;
    if(state != *NstState)
    {
        SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(2), "calib_Nst:%d->%d", state, *NstState);
        state = *NstState;
    }

    switch (*NstState)
    {
        case NST_NO_STATE:
            break;

        case NST_IDLE_STATE:

            myl1_BandInit(g_calibStubLocalCtx.monBand);

            if(0xffff == g_calibStubGlobalCtxPtr->monArfcn)
            {
                sl1_PowerReq(g_calibStubLocalCtx.monBand);
                *NstState = NST_POWER_STATE;
            }
            else
            {
                if(0xff == g_calibStubGlobalCtxPtr->monExpPow)
                {
                    sl1_MonitorReq(g_calibStubGlobalCtxPtr->monArfcn);

                    *NstState = NST_POWER_STATE;
                }
                else
                {
                    *NstState = NST_SYNC_STATE;
                }
            }

            break;

        case NST_POWER_STATE:
        {

            UINT16 Arfcn;
            UINT16 Index = g_calibStubGlobalNstCtxPtr->powerReq.Index;
            UINT8 Power;
            if(Index < g_calibStubGlobalNstCtxPtr->powerReq.Nb)
            {
                Arfcn = g_calibStubGlobalNstCtxPtr->powerReq.List[Index].Arfcn;
                if((Power = sl1_Power(Arfcn)) != 0)
                {
                    g_calibStubGlobalNstCtxPtr->powerReq.List[Index].Rssi = Power;
                    g_calibStubGlobalNstCtxPtr->powerReq.Index++;
                }
            }
            else
            {
                for(i=0; i<g_calibStubGlobalNstCtxPtr->powerReq.Nb; i++)
                {
                    Power = g_calibStubGlobalNstCtxPtr->powerReq.List[i].Rssi;
                    Arfcn = g_calibStubGlobalNstCtxPtr->powerReq.List[i].Arfcn;
                    if(g_calibStubGlobalCtxPtr->power[0] > Power)
                    {
                        g_calibStubGlobalCtxPtr->power[0] = Power;
                        g_calibStubGlobalCtxPtr->arfcn[0] = Arfcn;
                    }

                }
                if(0xff == g_calibStubGlobalCtxPtr->monExpPow)
                {
                    g_calibStubGlobalCtxPtr->monExpPow = g_calibStubGlobalCtxPtr->power[0];
                }
                if(0xffff == g_calibStubGlobalCtxPtr->monArfcn)
                {
                    g_calibStubGlobalCtxPtr->monArfcn = g_calibStubGlobalCtxPtr->arfcn[0];
                }
                g_calibStubGlobalCtxPtr->firstFint = 1; //inform calib stub to update

                *NstState = NST_SYNC_STATE;
            }

        }
        break;

        case NST_SYNC_STATE:
            /// Initialize PAL with the selected band.

            result = myl1_Synch();
            //fail count
            if(MY1_SYNC_GET_NXT_STATE(result) == MY1_SYNC_NO_STATE)
            {
                if(MY1_SYNC_GET_PRE_STATE(result) == MY1_SYNC_SCH_STATE)
                {
                    if(MY1_SYNC_GET_SCHFOUND(result))
                    {
                        syncFailCnt = 0;
                        *NstState = NST_OFFSET_STATE;
                    }
                    else if(MY1_SYNC_GET_SCHCNT(result) == 3)
                    {
                        syncFailCnt++;
                    }
                }
                else if(MY1_SYNC_GET_PRE_STATE(result) == MY1_SYNC_FCH_READ_STATE)
                {
                    syncFailCnt++;
                }

                if(syncFailCnt >= 20 )
                {
                    // report fail response and return to idle
                    g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_CONNECT_RSP;
                    g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_FAILED;
                    syncFailCnt = 0;
                    *NstState = NST_NO_STATE;
                    SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(2), "calib_Nst: sch can not found!");
                }


            }

            break;

        case NST_OFFSET_STATE:

            /* Set MS time aligned with cell time. */

            Tof = g_calibStubGlobalCtxPtr->tn*625+g_calibStubGlobalCtxPtr->qbOf;
            sxs_fprintf (_L1S|TNB_ARG(3), "Tof(%d)=%d+%d", Tof, g_calibStubGlobalCtxPtr->tn*625,g_calibStubGlobalCtxPtr->qbOf);

            pal_SynchronizationChange (Tof);

            g_calibStubGlobalCtxPtr->qbOf = 0;

            if(g_calibStubGlobalCtxPtr->nbPower == 0xff)
            {
                g_calibStubGlobalCtxPtr->nbPower = g_calibStubGlobalCtxPtr->monExpPow;
            }

            //TxWin.Arfcn          = g_calibStubGlobalCtxPtr->txArfcn;
            TxWin.TxBitMap = 1;
            TxWin.RouteMap [0] = L1S_TCH0_BUFFER;
            TxWin.TxPower [0] = g_calibStubGlobalCtxPtr->txPcl; // TODO: max power
            //TxWin.Tsc = g_calibStubGlobalCtxPtr->txTsc; // TODO: tsc
            TxWin.TA             = 0;
            TxWin.Start          = 3*625;
            TxWin.WinIdx         = 1;

            TxWin.Arfcn =
                NBurstRxWin.Arfcn    = g_calibStubGlobalCtxPtr->txArfcn;
            NBurstRxWin.Gain     = g_calibStubGlobalCtxPtr->nbPower;// TODO: agc
            NBurstRxWin.Start = 0;
            NBurstRxWin.WinIdx   = 0;

            NBurstRxWin.BufIdx   = L1S_TCH0_BUFFER;

            NBurstRxWin.FOf      = 0;

            TxWin.Tsc=
                NBurstRxWin.Tsc =  (g_calibStubGlobalCtxPtr->txTsc==0xff)?(g_calibStubGlobalCtxPtr->bsic & 0x7): g_calibStubGlobalCtxPtr->txTsc; // TODO: tsc
            NBurstRxWin.RxBitMap = 1;

            GsmCounters.T1 = (u16)((g_calibStubGlobalCtxPtr->fn + 1) / 1326); //next frame numer
            GsmCounters.T2 = (g_calibStubGlobalCtxPtr->t2 == 25) ? 0 : g_calibStubGlobalCtxPtr->t2 + 1;
            GsmCounters.T3 = (g_calibStubGlobalCtxPtr->t3 == 50) ? 0 : g_calibStubGlobalCtxPtr->t3 + 1;

            pal_SetGsmCounters(&GsmCounters);
            hal_SpeechFifoReset();
            pal_SetDtx(FALSE);
            pal_StopCiphering();

            pal_StartTch (PAL_SPEECH_CHANNEL|PAL_FULL_RATE_CHANNEL|PAL_SPEECH_ALGO_FR, 0, NIL); // TODO: mode:PAL_SPEECH_ALGO_FR:PAL_SPEECH_ALGO_HR:PAL_SPEECH_ALGO_EFR

            pal_RxBufferCfg_t RxBufferCfg;

            RxBufferCfg.BufferIdx    = L1S_TCH0_BUFFER;
            RxBufferCfg.ChannelType  = PAL_TCHF_CHN;  // TODO: channel type: PAL_TCHF_CHN : PAL_TCHH_CHN
            RxBufferCfg.InterruptReq = PAL_NO_INTERRUPT;

            pal_RxBufferCfg (&RxBufferCfg);
            NBlockDesc.Cs     = CS1;
            NBlockDesc.Data   = sl1_L2FillFrame;
            NBlockDesc.BufIdx = L1S_TCH0_BUFFER;
            NBlockDesc.ChannelType = PAL_TCHF_CHN;
            pal_NblockEncoding (&NBlockDesc);

            //  tch loop mode
            if(g_calibStubGlobalNstCtxPtr->loopback != 0)
            {
                pal_SetTchLoop(g_calibStubGlobalNstCtxPtr->loopback);
            }

            g_calibStubGlobalNstCtxPtr->speech_enc.encMode = HAL_FR;
            g_calibStubGlobalNstCtxPtr->speech_enc.encFrameType = 0;
            g_calibStubGlobalNstCtxPtr->speech_enc.sp = 1;

            if(g_calibStubGlobalNstCtxPtr->syncPhase == NST_SYNC_SCH_FOUND)
            {
                g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_CONNECT_RSP;
                g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            }

            *NstState = NST_RX_STATE;
            break;

        case NST_RX_STATE:
        case NST_ACQUIRE_STATE:
        case NST_BER_CACULATION_STATE:
        case NST_STOP_RX_SYNC_STATE:
        case NST_DISCONN_STATE:
        {

            if(*NstState == NST_STOP_RX_SYNC_STATE)
            {
                bfiCnt = 0;
                NBBurstMap = 0;
                rxDoneFlag = 0;
                SubFrameIdx = 0;
                SubFrameBitMap = 0;
                pal_StopTch();
                *NstState = NST_IDLE_STATE;
                break;
            }
            if(*NstState == NST_DISCONN_STATE)
            {
                bfiCnt = 0;
                NBBurstMap = 0;
                rxDoneFlag = 0;
                SubFrameIdx = 0;
                SubFrameBitMap = 0;
                pal_StopTch();
                *NstState = NST_NO_STATE;
                break;
            }

            //T2 = g_calibStubGlobalCtxPtr->t2;
            RFn         = (u16)(g_calibStubGlobalCtxPtr->fn % 42432);   //51*8*104=42432
            C104 = (u8)(RFn % 104);
            T2 = C104 + 104 - 13*g_calibStubGlobalCtxPtr->tn;
            SubFrameIdxMax=16;
            TchSubFrames = sl1_TchSubFrames [0];

            if (C104 == 103)
            {
                SubFrameIdx = 0;
            }
            else
            {
                if (TchSubFrames [0] == 103)
                {
                    SubFrameIdx = 1;
                }
                while (C104 > TchSubFrames [SubFrameIdx])
                {
                    if (++SubFrameIdx >= SubFrameIdxMax)
                    {
                        SubFrameIdx = 0;
                    }
                }
            }

            if (T2 >= 104)   T2 -= 104;
            SaIdx = T2 / 26;
            T2    = T2 % 26;
            if (T2 == 25)  SaIdx = (SaIdx + 1) & 3;


            if ((TchSubFrames [SubFrameIdx] & ~L1_SID_BLOCK) == C104)
            {
                SubFrameBitMap |= 1;

                if (TchSubFrames [SubFrameIdx] & L1_SID_BLOCK)  SubBlock = TRUE;

                if (++SubFrameIdx >= SubFrameIdxMax)  SubFrameIdx = 0;
            }

            /* -------------------- Time to encode a Sacch ? ------------------------ */
            if((TchSchd [T2] & SL1_EC_SA) && (SaIdx == 0) )
            {
                u8 *Data =sl1_L2FillFrame;

                sl1_DoNormReport(Data);

                Data [0] = g_calibStubGlobalCtxPtr->txPcl; //txPwr
                Data [1] = 0; //TA

                NBlockDesc.Cs          = CS1;
                NBlockDesc.Data        = Data;
                NBlockDesc.BufIdx = L1S_SACCH0_BUFFER;
                NBlockDesc.ChannelType = PAL_CCH_CHN;
                pal_NblockEncoding (&NBlockDesc);
                SXS_TRACE (_L1S|TLEVEL(2)|TDB,TSTR("Encode Sacch.\n",0x0007001b));
            }
            /* ------- Time to program a reception and a transmission burst ? ------- */
            if (TchSchd [T2] & SL1_BST(0))
            {
                if ((T2 == 11) || (T2 == 24))
                {
                    TxWin.BstIdx = SaIdx;
                    TxWin.RouteMap [0] = L1S_SACCH0_BUFFER;
                }
                else
                {
                    TxWin.BstIdx = SL1_GET_BST_IDX(TchSchd [T2] );
                    TxWin.RouteMap [0] = L1S_TCH0_BUFFER;
                }

                NBurstRxWin.Start = 0;
                TxWin.TxPower[0] = g_calibStubGlobalCtxPtr->txPcl;


                NBurstRxWin.BstIdx = SL1_GET_BST_IDX(TchSchd [T2] );
                NBurstRxWin.Gain   = g_calibStubGlobalCtxPtr->nbPower;// Agc
                TxWin.Tsc =
                    NBurstRxWin.Tsc =  (g_calibStubGlobalCtxPtr->txTsc==0xff)?(g_calibStubGlobalCtxPtr->bsic & 0x7): g_calibStubGlobalCtxPtr->txTsc; // TODO: tsc
                TxWin.Arfcn =
                    NBurstRxWin.Arfcn = g_calibStubGlobalCtxPtr->txArfcn;

                //   SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "HAL_SPEECH_FRAME_SIZE_COD_BUF");
                memcpy((UINT8*)g_calibStubGlobalNstCtxPtr->speech_enc.encOutBuf,(UINT8*)g_calibStubGlobalNstCtxPtr->speech_dec.decInBuf, HAL_SPEECH_FRAME_SIZE_COD_BUF);

                pal_SetTxWin(&TxWin);

                NBurstRxWin.BufIdx   = L1S_TCH0_BUFFER;

                NBBurstMap |= 1;

                pal_SetNBurstRxWin (&NBurstRxWin);

            }

            /* --------------- Time to read equalisation results ? ----------------- */
            if (NBBurstMap & (1 << 2))
            {
                sl1_GetNBEqualisationResult((SubFrameBitMap & (1 << 2) ) ?SL1_GET_MEAS | SL1_MEAS_SUB : SL1_GET_MEAS);
            }

            /* --------------------- Time to decode a Tch ? ------------------------- */
            if ((TchSchd [T2] & SL1_DC_TC)   // It is time to decode a Tch block.
                    &&  ((NBBurstMap&0x3c) == 0x3c))          // Decode only when 4 bursts were received.
            {
                /* Time for block decoding. */
                UINT32 nbBits = 260;
                UINT8 bfi=0;

                sl1_GetNBlockResult(&NBurstRxWin,  &bfi, (u8)(SubBlock ? SL1_GET_MEAS | SL1_MEAS_SUB: SL1_GET_MEAS));

                hal_SpeechPopRxPushTx((HAL_SPEECH_DEC_IN_T*)(&g_calibStubGlobalNstCtxPtr->speech_dec),(HAL_SPEECH_ENC_OUT_T*)(&g_calibStubGlobalNstCtxPtr->speech_enc));

                if(*NstState == NST_RX_STATE)
                {

                    bfiCnt += bfi;
                    if(g_calibStubGlobalCtxPtr->bitError == 0)
                    {
                        if(bfiCnt>=2)
                            bfiCnt -= 2;
                        else
                            bfiCnt = 0;
                    }
                    g_calibStubGlobalNstCtxPtr->sync_numerrors = bfiCnt;
                    SXS_TRACE(_L1S|TNB_ARG(4), "sync_numerrors  = %d", g_calibStubGlobalNstCtxPtr->sync_numerrors);
                    if(g_calibStubGlobalNstCtxPtr->syncPhase == NST_SYNC_DECODE_DONE)
                    {

                        //report fail response and return to idle
                        if(bfiCnt > 20)
                        {
                            // report fail response and return to idle

                            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_CONNECT_RSP;
                            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_FAILED;
                            bfiCnt = 0;
                            NBBurstMap = 0;
                            rxDoneFlag = 0;
                            *NstState = NST_NO_STATE;
                            pal_StopTch();
                        }
                        else if((bfiCnt == 0) && (g_calibStubGlobalCtxPtr->bitError <10) && (rxDoneFlag == 0))
                        {
                            rxDoneFlag = 1;

                            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_CONNECT_RSP;
                            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
                        }
                    }
                }
                else  // NST_ACQUIRE_STATE) or NST_BER_CACULATION_STATE
                {

                    UINT32 tmp;
                    UINT32 Ber=nbBits;
                    ACQUIRE_FSM_STAT_T acquireState;

                    // Reordering
                    pRxBuffer = (UINT32 *)g_calibStubGlobalNstCtxPtr->speech_dec.decInBuf;

                    // if frame type is PAL_TCHF_CHN,order=0 in SE_BER_LOOP_AB_ORDER_REQ
                    if(g_calibStubGlobalNstCtxPtr->order == 0)
                    {
                        for (i=0; i<8; i++)
                        {
                            tmp = pRxBuffer[i];
                            k = i*32;
                            for (j=0; j<32; j++)
                            {
                                g_calibStubGlobalNstCtxPtr->ReorderOut[g_sppMatrixFrInv[k+j]] = (UINT8)(tmp & 1);
                                tmp = tmp >> 1;
                            }
                        }
                        tmp = pRxBuffer[8];
                        for (j=0; j<4; j++)
                        {
                            g_calibStubGlobalNstCtxPtr->ReorderOut[g_sppMatrixFrInv[256+j]] = (UINT8)(tmp & 1);
                            tmp = tmp >> 1;
                        }
                    }
                    else
                    {
                        for (i=0; i<8; i++)
                        {
                            tmp = pRxBuffer[i];
                            k = i*32;
                            for (j=0; j<32; j++)
                            {
                                g_calibStubGlobalNstCtxPtr->ReorderOut[k+j] = (UINT8)(tmp & 1);
                                tmp = tmp >> 1;
                            }
                        }
                        tmp = pRxBuffer[8];
                        for (j=0; j<4; j++)
                        {
                            g_calibStubGlobalNstCtxPtr->ReorderOut[256+j] = (UINT8)(tmp & 1);
                            tmp = tmp >> 1;
                        }
                    }

                    if(*NstState == NST_ACQUIRE_STATE)
                    {
                        if((acquireState=acquire_run((UINT8*)g_calibStubGlobalNstCtxPtr->ReorderOut, nbBits, &Ber)) == ACQUIRE_DONE)
                        {
                            SXS_TRACE(_L1S|TNB_ARG(1), "ACQUIRE_DONE,Ber=%d", Ber);
                            if(Ber < (nbBits - g_calibStubGlobalNstCtxPtr->prbs.acq_threshold))
                            {
                                //check ber_mode
                                if(g_calibStubGlobalNstCtxPtr->prbs.ber_mode == 3)  //AUTO
                                {
                                    *NstState = NST_BER_CACULATION_STATE;
                                }
                            }
                        }
                        else
                        {
                            SXS_TRACE(_L1S|TNB_ARG(1), "ACQUIRE_DONE,Ber111=%d", Ber);
                        }
                    }
                    else   //NST_BER_CACULATION_STATE
                    {
                        Ber_Caculate((UINT8*)g_calibStubGlobalNstCtxPtr->ReorderOut, nbBits, &Ber);
                    }
                    if(Ber > g_calibStubGlobalNstCtxPtr->prbs.acq_lost_threshold)
                    {
                        SXS_TRACE(_L1S|TNB_ARG(1), "ACQUIRE_LOST,Ber=%d", Ber);
                        *NstState = NST_ACQUIRE_STATE;
                    }
                    if((Ber > 80)&&(Ber/((OldBer == 0)?4:OldBer)) > 10)
                    {
                        SXS_TRACE(_L1S|TNB_ARG(4), "Ber_Caculate error,Ber=%d bits = %d old = %d fn = %d", Ber,nbBits, OldBer,g_calibStubGlobalCtxPtr->fn);
                    }
                    else
                    {
                        if(TRUE == g_calibStubGlobalNstCtxPtr->logEnable)
                        {
                            g_calibStubGlobalNstCtxPtr->measuretimes += 1;
                            g_calibStubGlobalNstCtxPtr->num_bits += nbBits;
                            SXS_TRACE(_L1S|TNB_ARG(4), "Ber_Caculate,Ber=%d bits = %d  fn = %d", Ber,nbBits, g_calibStubGlobalCtxPtr->fn);
                            g_calibStubGlobalNstCtxPtr->num_errors += Ber;
                            OldBer = Ber;
                        }
                    }

                    //log
                }

                if (SaIdx == 0)
                {
                    sl1_MeasReport ();
                }
            }

            SubFrameBitMap <<= 1;
            NBBurstMap <<= 1;
            break;
        }
        default:
            break;
    }

}



/*
   ==============================================================================
Function   :calib_NstParse
----------------------------------------------------------------------------
Scope      :nst stub message parser
Parameters :
Return     : None.
==============================================================================
*/
VOID calib_StubNstParse()
{
    switch (g_calibStubGlobalNstCtxPtr->NstReqState)
    {
        case CALIB_NST_SE_BER_CONNECT_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_CONNECT_REQ");

            g_calibStubGlobalCtxPtr->state = CALIB_STUB_NST_STATE;
            g_calibStubGlobalCtxPtr->firstFint = 1;
            calib_StubNstChangeState(NST_SYNC_STATE);// TODO: NST_POWER_STATE when no bcchArfcn
            break;
        }

        case CALIB_NST_SE_BER_DISCONNECT_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_DISCONNECT_REQ");

            g_calibStubGlobalNstCtxPtr->logEnable = FALSE;
            calib_StubNstChangeState(NST_DISCONN_STATE);
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_DISCONNECT_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            break;
        }

        case CALIB_NST_SE_BER_CONFIGRURE_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_CONFIGRURE_REQ");
            if(g_calibStubGlobalNstCtxPtr->pn_type == 1)
            {
                g_calibStubGlobalNstCtxPtr->prbs.poly = 0x110;
                g_calibStubGlobalNstCtxPtr->prbs.RegLen = 9;
            }
            else if(g_calibStubGlobalNstCtxPtr->pn_type == 3)
            {
                g_calibStubGlobalNstCtxPtr->prbs.poly = 0x6000;
                g_calibStubGlobalNstCtxPtr->prbs.RegLen = 15;
            }
            g_calibStubGlobalNstCtxPtr->prbs.acq_timeout = 260/g_calibStubGlobalNstCtxPtr->prbs.RegLen - 2;
            g_calibStubGlobalNstCtxPtr->prbs.ber_mode = g_calibStubGlobalNstCtxPtr->ber_mode;
            g_calibStubGlobalNstCtxPtr->prbs.invert = g_calibStubGlobalNstCtxPtr->invert;
            g_calibStubGlobalNstCtxPtr->prbs.acq_threshold = g_calibStubGlobalNstCtxPtr->acq_threshold;
            g_calibStubGlobalNstCtxPtr->prbs.acq_lost_threshold = g_calibStubGlobalNstCtxPtr->acq_lost_threshold;
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_CONFIGRURE_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            InitPrbs(g_calibStubGlobalNstCtxPtr->prbs.poly, 0xffff, g_calibStubGlobalNstCtxPtr->prbs.RegLen, g_calibStubGlobalNstCtxPtr->invert);
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "==CALIB_NST_SE_BER_CONFIGRURE_REQ");
            calib_StubNstChangeState(NST_ACQUIRE_STATE);

            break;
        }

        case CALIB_NST_SE_BER_START_LOG_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_START_LOG_REQ");

            // report ok response and return to idle
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_START_LOG_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            break;
        }

        case CALIB_NST_SE_BER_STOP_LOG_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_STOP_LOG_REQ");
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_STOP_LOG_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            break;
        }

        case CALIB_NST_SE_BER_LOOP_AB_ORDER_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SE_BER_LOOP_AB_ORDER_REQ");
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SE_BER_LOOP_AB_ORDER_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            break;
        }

        case CALIB_NST_SIMU_LOG_REQ:
        {
            SXS_TRACE(_L1S|TSTDOUT|TNB_ARG(0), "<=CALIB_NST_SIMU_LOG_REQ");
            g_calibStubGlobalCtxPtr->state = CALIB_STUB_NST_STATE;
            g_calibStubGlobalCtxPtr->firstFint = 1;
            g_calibStubGlobalNstCtxPtr->logEnable = TRUE;

            calib_StubNstChangeState(NST_SIMU_LOG_STATE);
            // report ok response and return to idle
            g_calibStubGlobalNstCtxPtr->rspcId = CALIB_NST_SIMU_LOG_RSP;
            g_calibStubGlobalNstCtxPtr->status = CALIB_NST_STATUS_SUCCESS;
            break;
        }

    }

}

void calib_SetNststateIdle()
{
    g_calibStubGlobalNstCtxPtr->NstState = NST_STOP_RX_SYNC_STATE;
    g_calibStubGlobalNstCtxPtr->NstReqCurrState = 0x0;
    g_calibStubGlobalNstCtxPtr->num_bits = 0;
    g_calibStubGlobalNstCtxPtr->measuretimes = 0;
    g_calibStubGlobalNstCtxPtr->num_errors = 0;
}
UINT32 calib_GetReqState()
{
    return g_calibStubGlobalNstCtxPtr->NstReqState;
}
UINT32 calib_GetReqCurrState()
{
    return g_calibStubGlobalNstCtxPtr->NstReqCurrState;
}

VOID calib_SetReqCurrState(UINT32 state)
{
    g_calibStubGlobalNstCtxPtr->NstReqCurrState = state;
}
/*
   ==============================================================================
Function   :calib_StubNstOpen
----------------------------------------------------------------------------
Scope      :nst stub open
Parameters :
Return     : None.
==============================================================================
*/

VOID calib_StubNstOpen()
{
    // This doesn't release at all.
    hal_SwRequestClk(FOURCC_CALIB, HAL_SYS_FREQ_104M);
    g_calibStubGlobalNstCtxPtr = (CALIB_STUB_NST_CTX_T*)&g_calibStubGlobalNstCtx;
    g_calibStubGlobalNstCtxPtr->NstState = NST_NO_STATE;
    g_calibStubGlobalNstCtxPtr->num_bits = 0;
    g_calibStubGlobalNstCtxPtr->measuretimes = 0;
    g_calibStubGlobalNstCtxPtr->num_errors = 0;
    g_calibStubGlobalNstCtxPtr->NstReqState = 0x0;
    g_calibStubGlobalNstCtxPtr->NstReqCurrState = 0x0;
    g_calibStubGlobalCtxPtr->txTsc = 0xff; //use the same tsc as bch
    g_calibStubGlobalNstCtxPtr->order = 0; //channel codec
    g_calibStubGlobalNstCtxPtr->prbs.ber_mode = 3; //AUTO
    g_calibStubGlobalNstCtxPtr->prbs.poly = 0x6000;
    g_calibStubGlobalNstCtxPtr->prbs.RegLen = 15;//PRBS15
    g_calibStubGlobalNstCtxPtr->prbs.acq_threshold=250;//error free bit
    g_calibStubGlobalNstCtxPtr->prbs.acq_lost_threshold=200; //error bit
    g_calibStubGlobalNstCtxPtr->logEnable = 0;
    g_calibStubGlobalNstCtxPtr->powerReq.Nb = 0;
    g_calibStubGlobalNstCtxPtr->syncPhase=1;
}


