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

























#include "pal_gsm.h"
#include "myl1.h"
#include "calib_m.h"

#include "sxs_io.h"
#include "sxr_mem.h"



/* Global variable used to keep track of the equalization status. */
VOLATILE BOOL    g_calibEquRdyFlag = FALSE;



/* ----------------- Initialize PAL with the selected band. */

VOID myl1_BandInit(UINT8 band)
{
    switch (band)
    {
        case CALIB_STUB_BAND_GSM850:
            pal_Initialisation (PAL_INIT_ASYNCHRO, PAL_GSM850 | PAL_DCS1800);
            break;

        case CALIB_STUB_BAND_GSM900:
            pal_Initialisation (PAL_INIT_ASYNCHRO, PAL_GSM900 | PAL_DCS1800);
            break;

        case CALIB_STUB_BAND_DCS1800:
            pal_Initialisation (PAL_INIT_ASYNCHRO, PAL_GSM900 | PAL_DCS1800);
            break;

        case CALIB_STUB_BAND_PCS1900:
            pal_Initialisation (PAL_INIT_ASYNCHRO, PAL_GSM900 | PAL_PCS1900);
            break;

        default:
            pal_Initialisation (PAL_INIT_ASYNCHRO, PAL_GSM900 | PAL_DCS1800);
    }
}



/* ----------------- Monitor the power on a specified ARFCN and Band. */

BOOL myl1_Monit(UINT16 arfcn, UINT8 Band, UINT8 ExpPower, UINT8 t2,
                UINT8 *MonPower, UINT8 *NBPower)
{
    BOOL res = 0;
    PRIVATE pal_RxWin_t MonWin;
    PRIVATE pal_NBurstRxWin_t NBurstRxWin;

    /* Prog. Monitorings. */
    MonWin.Arfcn  = arfcn;
    MonWin.Start  = (UINT16)(PAL_NB_MAX_WIN_PER_FRAME-1) *
                    (PAL_PLL_LOCK_TIME + PAL_MON_DURATION);
    MonWin.WinIdx = 1;
    MonWin.Gain   = PAL_AGC;

    g_calibStubGlobalCtxPtr->monBitmap |= 1;

    /* Normal burst window. */
    NBurstRxWin.Arfcn    = arfcn;
    NBurstRxWin.Gain     = ExpPower;
    NBurstRxWin.WinIdx   = 0;
    NBurstRxWin.BufIdx   = 0;
    NBurstRxWin.FOf      = 0;
    NBurstRxWin.Tsc      = 0;
    NBurstRxWin.RxBitMap = 1;
    NBurstRxWin.Start    = 0;
    NBurstRxWin.BstIdx   = 0;

    pal_SetNBurstRxWin (&NBurstRxWin);
    pal_SetMonRxWin (&MonWin);

    /* Get Monitoring Results. */
    if ( (g_calibStubGlobalCtxPtr->monBitmap >> 2) & 0x1 )
    {
        pal_NBurstResult_t NBurstResult;
        UINT16 pow = 0;
        UINT8 dcoAccCount = g_calibStubGlobalCtxPtr->dcoAccCount % CALIB_DCO_ACC_COUNT;

        /* Monitoring result. */
        MonWin.Arfcn  = arfcn;
        MonWin.Start  = (UINT16)(PAL_NB_MAX_WIN_PER_FRAME-1) *
                        (PAL_PLL_LOCK_TIME + PAL_MON_DURATION);
        MonWin.WinIdx = 1;

        pow += pal_GetMonResult (&MonWin);
        *MonPower = pow;

        /* Normal burst result. */
        pal_GetNBurstResult (0, arfcn, &NBurstResult);
        *NBPower = NBurstResult.Pwr;

        /* Record the DC offset values and count to do an average. */
        pal_CalibGetDcOffset(    (INT16*)&g_calibStubGlobalCtxPtr->dcoI[dcoAccCount],
                                 (INT16*)&g_calibStubGlobalCtxPtr->dcoQ[dcoAccCount]);

        /* The count of DC offset measurements starts from 0, goes to
         * CALIB_DCO_ACC_COUNT and then stay between CT_CALIB_DCO_ACC_COUNT
         * and 2 * CALIB_DCO_ACC_COUNT. This allows to know that the number
         * of measure for the average is above the minimum required to do
         * an average (i.e. CALIB_DCO_ACC_COUNT). */
        if (g_calibStubGlobalCtxPtr->dcoAccCount >= 2 * CALIB_DCO_ACC_COUNT - 1)
        {
            g_calibStubGlobalCtxPtr->dcoAccCount = CALIB_DCO_ACC_COUNT;
        }
        else
        {
            g_calibStubGlobalCtxPtr->dcoAccCount++;
        }

        /* Power measured. */
        res = 1;
    }

    g_calibStubGlobalCtxPtr->monBitmap <<= 1;
    return res;
}



/* ----------------- myl1_Synch. */

UINT32 myl1_Synch(void)
{
    PRIVATE UINT8 SynchState = MY1_SYNC_NO_STATE;

    const INT16 TabfOf[8] = {0,1*500,-2*500,3*500,-4*500,5*500,-6*500,7*500};
    PRIVATE UINT8 SearchIdx = 0;
    PRIVATE UINT8 FchFound;
    PRIVATE UINT8 FrameCount;
    PRIVATE UINT8 NbFramesToSch;
    PRIVATE pal_RxWin_t RxWin;
    PRIVATE pal_FchResult_t FchResult;
    UINT32 result = MY1_SYNC_SET_PRE_STATE(SynchState);

    switch (SynchState)
    {
        case MY1_SYNC_NO_STATE:

            // Set a new frequency offset for the search.
            pal_FrequencyTuning(TabfOf[SearchIdx++],
                                g_calibStubLocalCtx.monArfcn);

            RxWin.Arfcn  = g_calibStubLocalCtx.monArfcn;
            RxWin.Gain   = (UINT8)g_calibStubLocalCtx.monExpPow;
            RxWin.Start  = L1_FCH_WIN_QB_POS;

            // Open FCH window
            pal_SetFchRxWin(&RxWin);

            FrameCount = 0;
            FchFound = FALSE;
            SynchState = MY1_SYNC_FCH_SEARCH_STATE;
            break;

        case MY1_SYNC_FCH_SEARCH_STATE:

            FrameCount++;
            SynchState = MY1_SYNC_FCH_READ_STATE;
            break;

        case MY1_SYNC_FCH_READ_STATE:

            FchFound = pal_GetFchResult (L1S_FCH_WIN_IDX, &FchResult);

            if (!FchFound) // FCCH Not Found
            {
                FrameCount++;
            }
            else // FCCH Found
            {
                s32 FchQb    = (FchResult.TOf * 4) + L1_FCH_WIN_QB_POS;
                UINT16 FchFrame = FchQb / 5000;

                pal_CloseFchRxWin(0);

                RxWin.Start  =
                    g_calibStubGlobalCtxPtr->qbOf = FchQb - (FchFrame * 5000);

                pal_FrequencyTuning(FchResult.FOf, g_calibStubLocalCtx.monArfcn);

                NbFramesToSch = 10 - (FrameCount - FchFrame);
                sxs_fprintf (_L1S|TDB|TNB_ARG(3),TSTR("Fch found for 0x%x Qb %i Sch expected in %i frames\n",0x00080007),
                             g_calibStubLocalCtx.monArfcn, g_calibStubGlobalCtxPtr->qbOf, NbFramesToSch);
                pal_CloseFchRxWin(0);

                SynchState = MY1_SYNC_SCH_STATE;
            }

            if (FrameCount > 12) // We sould already have found it
            {
                SXS_TRACE(TSTDOUT, "Fch detection failure\n");
                pal_CloseFchRxWin(0);
                SynchState = MY1_SYNC_NO_STATE;
            }
            break;

        case MY1_SYNC_SCH_STATE:
        {
            PRIVATE BOOL WaitForSch = FALSE;
            PRIVATE BOOL SchFound = FALSE;
            PRIVATE UINT8 SchCnt = 0;
            pal_SchResult_t schResult;

            // Skip frames until SCH one
            if (--NbFramesToSch > 0)
            {
                result |= MY1_SYNC_SET_SCHCNT(SchCnt);
                return result;
            }

            // Prog. SCH window
            if (WaitForSch == FALSE)
            {
                pal_SetSchRxWin(&RxWin, 0);
                WaitForSch = TRUE;
                NbFramesToSch = 3;

                SchCnt++;
            }
            else // Get result & prepare for next search if needed
            {
                NbFramesToSch = 8;

                SchFound = pal_GetSchResult((UINT8)(RxWin.WinIdx & 1),
                                            &schResult);

                WaitForSch = FALSE;
            }

            if (SchFound) // We Found it
            {
                // Extract Counters
                UINT16 t1 =    ((schResult.Data [0] & 3) << 9) |
                               (schResult.Data [1] << 1) |
                               ((schResult.Data [2] >> 7) & 1);
                UINT8  t2 =    (schResult.Data [2] >> 2) & 0x1F;
                UINT8  t3 =    ((((schResult.Data [2] & 3) << 1) |
                                 (schResult.Data [3] & 1)) * 10) + 1;

                g_calibStubGlobalCtxPtr->bsic = (schResult.Data [0] >> 2) & 0x3F;
                g_calibStubGlobalCtxPtr->fn   =  myl1_Fn(t1, t2, t3) + 2;
                g_calibStubGlobalCtxPtr->t2   = (t2 + 2) % 26;
                g_calibStubGlobalCtxPtr->t3   = t3 + 2;
                g_calibStubGlobalCtxPtr->state       = CALIB_STUB_IDLE_STATE;
                sxs_fprintf (_L1S, "Sch found on 0x%x. Bsic %i T3 %i TOf=%d FOf=%d\n",
                             g_calibStubLocalCtx.monArfcn, g_calibStubGlobalCtxPtr->bsic, g_calibStubGlobalCtxPtr->t3,
                             schResult.TOf, schResult.FOf);
                // Correct the freq. offset
                pal_FrequencyTuning(schResult.FOf, g_calibStubLocalCtx.monArfcn);

                // MModify the timing offset
                g_calibStubGlobalCtxPtr->qbOf +=  schResult.TOf;
            }
            else // We didn't find the SCH -> we have another chance
            {
                if (SchCnt == 3) // ...the second time neither -> bad...
                    SXS_TRACE(TSTDOUT, "Failed to receive Sch !!");
            }

            result |= MY1_SYNC_SET_SCHCNT(SchCnt);

            if ((SchFound) || (SchCnt == 3))
            {
                if(SchFound)
                {
                    result |= MY1_SYNC_SET_SCHFOUND(1);
                }
                SynchState = MY1_SYNC_NO_STATE;
                SearchIdx = 0;
                WaitForSch = FALSE;
                SchCnt = 0;
                SchFound = FALSE;
            }
        }
        break;
    }
    result |= MY1_SYNC_SET_NXT_STATE(SynchState);
    return result;
}



/* ----------------- Function to get the fOf, using a FSM. */

myFOf_t myl1_GetFOf(BOOL init, UINT16 arfcn, UINT8 Band, UINT8 ExpPower, s32 *MeanfOf)
{
    enum {INIT, FCH_SEARCH_STATE, FCH_READ_STATE, SCH_STATE};
    PRIVATE UINT8 GetfOfState = INIT;

    PRIVATE UINT8 FchFound;
    PRIVATE UINT8 FrameCount;
    PRIVATE pal_RxWin_t RxWin;
    PRIVATE pal_FchResult_t FchResult;
#define GET_FOF_DEPTH 8
    PRIVATE s32 Fof_tab[GET_FOF_DEPTH];
    PRIVATE INT32 index;

    /* Init the FSM. */
    if (init)
    {
        GetfOfState = INIT;
        myl1_BandInit(Band);
        return FOF_SEARCH;
    }

    switch (GetfOfState)
    {
        case INIT:
            RxWin.Arfcn  = arfcn;
            RxWin.Gain   = ExpPower;
            RxWin.Start  = L1_FCH_WIN_QB_POS;

            // Open FCH window
            pal_SetFchRxWin(&RxWin);

            FrameCount = 0;
            FchFound = FALSE;
            GetfOfState = FCH_SEARCH_STATE;
            break;

        case FCH_SEARCH_STATE:
            FrameCount++;
            GetfOfState = FCH_READ_STATE;
            break;

        case FCH_READ_STATE:
            FchFound = pal_GetFchResult(L1S_FCH_WIN_IDX, &FchResult);

            if (!FchFound) // FCCH Not Found
            {
                FrameCount++;
            }
            else // FCCH Found
            {
                pal_CloseFchRxWin(0);

                Fof_tab[index] = FchResult.FOf;
                index++;
                if (index == GET_FOF_DEPTH)
                {
                    s32 mean_fof = 0;
                    INT32 i;

                    for (i=0; i<GET_FOF_DEPTH; i++)
                    {
                        mean_fof += Fof_tab[i];
                    }
                    mean_fof /= GET_FOF_DEPTH;
                    *MeanfOf = mean_fof;
                    index = 0;
                    return FOF_FOUND;
                }
                return FOF_SEARCH;
            }

            if (FrameCount > 12) // We should already have found it
            {
                pal_CloseFchRxWin(0);
                FrameCount = 0;
                GetfOfState = INIT;
            }
            break;
    }
    return FOF_NOT_FOUND;
}



/* ----------------- myl1_Idle */

const UINT8 M51BCCH[10] =
{
    0,
    SL1_BST(0),
    SL1_BST(1),
    SL1_BST(2),
    SL1_BST(3),
    0,
    SL1_DC_TC,
    0, 0, 0
};

UINT8 const *M51 = M51BCCH;

VOID myl1_Idle (void)
{
    enum {INIT, RX_STATE};
    PRIVATE UINT8 IdleState = INIT;

    PRIVATE pal_NBurstRxWin_t NBurstRxWin;
    PRIVATE UINT8 NBBurstMap = 0;
    PRIVATE BOOL DecodingAllowed = FALSE;

    UINT8 T3Mod10 = g_calibStubGlobalCtxPtr->t3 % 10;
    UINT8 T3p = (g_calibStubGlobalCtxPtr->t3 - 1) / 10;

    switch (IdleState)
    {
        case INIT:
            if (g_calibStubGlobalCtxPtr->qbOf != 0)
            {
                /* Set MS time aligned with cell time. */
                pal_SynchronizationChange (g_calibStubGlobalCtxPtr->qbOf);
                g_calibStubGlobalCtxPtr->qbOf = 0;
            }

            NBurstRxWin.Arfcn    = g_calibStubLocalCtx.monArfcn;
            NBurstRxWin.Gain     = (UINT8)g_calibStubLocalCtx.monExpPow;

            NBurstRxWin.WinIdx   = 0;
            NBurstRxWin.BufIdx   = 0;
            NBurstRxWin.FOf      = 0;
            NBurstRxWin.Tsc      = g_calibStubGlobalCtxPtr->bsic & 0x7;
            NBurstRxWin.RxBitMap = 1;

            IdleState = RX_STATE;

            break;

        case RX_STATE:

            /* Program a normal burst reception window. */
            if ((M51 [T3Mod10] & SL1_BST(0)) && (T3p==0))
            {
                /* Program NB reception window. */
                NBurstRxWin.Start  = g_calibStubGlobalCtxPtr->qbOf;
                NBurstRxWin.BstIdx = M51 [T3Mod10] & 0x03;
                NBurstRxWin.Gain   = (UINT8)g_calibStubLocalCtx.monExpPow;

                if (NBurstRxWin.BstIdx == 3)
                    DecodingAllowed = TRUE;

                NBBurstMap |= 1;
                pal_SetNBurstRxWin(&NBurstRxWin);
            }

            if (NBBurstMap & (1 << 2))
            {
                pal_NBurstResult_t NBurstResult;

                /* Equalisation results should be ready. */
                while (g_calibEquRdyFlag == FALSE);
                pal_GetNBurstResult(0, g_calibStubLocalCtx.monArfcn, &NBurstResult);
                myl1_GetNBEqualisationResult(&NBurstResult);
            }

            if (    (g_calibStubGlobalCtxPtr->t3 != 50) &&
                    (M51[T3Mod10]&SL1_DC_TC) &&
                    (DecodingAllowed) && (T3p==0))
            {
                /* Time for block decoding. */
                pal_NBlockResult_t NBlockResult;
                pal_GetNBlockResult(NBurstRxWin.BufIdx, 0, &NBlockResult);
                g_calibStubGlobalCtxPtr->bitError = NBlockResult.BitError;

                if (NBlockResult.Bfi == FALSE)
                {
#define BCCH_SIZE 23
                    CHAR my_string[2*BCCH_SIZE];
                    my_msg2str((UINT8*)(NBlockResult.Data[0]),my_string,BCCH_SIZE);
                    sxr_Free(NBlockResult.Data [0]);
                }
                else
                {
                    //SXS_TRACE(TSTDOUT, "IDLE: Bfi on BCCH\n");
                }
            }

            NBBurstMap <<= 1;
            break;
    }
}



/* ----------------- myl1_GetNBEqualisationResult */

VOID myl1_GetNBEqualisationResult (pal_NBurstResult_t *NBurstResult)
{
#define TAB_DEPTH 8
    PRIVATE UINT16 snR_tab[TAB_DEPTH];
    UINT32 snR_calc;
    PRIVATE UINT8 Power_tab[TAB_DEPTH];
    UINT32 Power_calc;

    PRIVATE INT32 tab_index = 0;
    PRIVATE BOOL tab_full = FALSE;
    INT32 i;

    Power_tab[tab_index]=NBurstResult->Pwr;
    snR_tab[tab_index]=NBurstResult->Snr;
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
        g_calibStubGlobalCtxPtr->nbPower = (UINT8)NBurstResult->Pwr;
        g_calibStubGlobalCtxPtr->snR = (UINT16)NBurstResult->Snr;
    }

    tab_index++;
    if (tab_index == TAB_DEPTH)
    {
        tab_index = 0;
        tab_full = TRUE;
    }

    g_calibStubGlobalCtxPtr->pFactor += NBurstResult->Snr;
    g_calibStubGlobalCtxPtr->tOf  += (NBurstResult->TOf * NBurstResult->Snr);
    g_calibStubGlobalCtxPtr->fOf  += (NBurstResult->FOf * NBurstResult->Snr);

    if (g_calibStubGlobalCtxPtr->pFactor >= PAL_SNR_FACTOR_THRES)
    {
        pal_TimeTuning((INT8)(g_calibStubGlobalCtxPtr->tOf / g_calibStubGlobalCtxPtr->pFactor));
        pal_FrequencyTuning((INT16)(g_calibStubGlobalCtxPtr->fOf /
                                    g_calibStubGlobalCtxPtr->pFactor),
                            g_calibStubLocalCtx.monArfcn);

        g_calibStubGlobalCtxPtr->pFactor = 0;
        g_calibStubGlobalCtxPtr->tOf  = 0;
        g_calibStubGlobalCtxPtr->fOf  = 0;
    }
}



/* ----------------- Send bursts of modulated stuff. */

UINT32 seed = 0xFFFFFFFF;
VOLATILE UINT8    FINT_counter = 0;

VOID myl1_SendBurst(UINT16 arfcn, UINT8 Pcl, UINT8 Band, UINT32 Tsc)
{
    pal_TxWin_t            TxWin;
    pal_NBlockDesc_t    NBlockDesc;
    UINT16 i;
    UINT32 seed_tmp = 0;
    UINT8 FillFrame_rand[25] = {    0,0,0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0
                               };

    /* Pseudo-Random Binary Sequence (PRBS) by a Linear-Feedback Shift */
    /* Non-zero coefficients are: c32 c28 c27 c1 */
    /* see http://www.eecircle.com/applets/009/LFSR.html */
    for (i=0; i <200; i++)
    {
        seed_tmp = ((seed >> 31) & 0x1) ^ ((seed >> 5) & 0x1) ^
                   ((seed >> 4) & 0x1) ^ ((seed >> 0) & 0x1);
        seed = seed >> 1;
        seed = (seed & 0x7FFFFFFF ) | ((seed_tmp<<31) & 0x80000000) ;

        FillFrame_rand[i/8] = FillFrame_rand[i/8] |
                              ((seed & 0x00000001)<<(i%8));
    }

    /* Init the frequency adjustment with 0. */
    pal_FrequencyTuning(0, arfcn);

    /* Send the burst. */
    if ((FINT_counter & 0x3) == 0)
    {
        NBlockDesc.Tsc    = Tsc;
        NBlockDesc.Cs     = CS4;
        NBlockDesc.Data   = FillFrame_rand;
        NBlockDesc.BufIdx = L1S_SACCH0_BUFFER;
        NBlockDesc.ChannelType = PAL_CCH_CHN;
        pal_NblockEncoding (&NBlockDesc);
    }


    /* Prog Tx win. */
    TxWin.Arfcn = arfcn;
    if(Pcl > 63)
    {
        TxWin.Start = 625*0;
    }
    else
    {
        TxWin.Start = 625*3;
    }
    //    TxWin.Start = 625*8*(FINT_counter & 0x3);
    FINT_counter = (FINT_counter + 1) % 51;
    TxWin.WinIdx = 0;
    TxWin.BstIdx = (FINT_counter & 0x3);
    TxWin.Tsc = Tsc;
    TxWin.TA = 0;
    if(Pcl > 63)
        TxWin.TxBitMap = 0xf;
    else
        TxWin.TxBitMap = 0x1;
    TxWin.RouteMap[0] = L1S_SACCH0_BUFFER;
    if((Pcl & 0xc0) == 0x40)
    {
        TxWin.TxPower[0] = (Pcl & 0x3f);
        TxWin.TxPower[1] = (Pcl & 0x3f)+4;
        TxWin.TxPower[2] = (Pcl & 0x3f)+8;
        TxWin.TxPower[3] = (Pcl & 0x3f)+12;
    }
    else if((Pcl & 0xc0) == 0x80)
    {
        TxWin.TxPower[0] = (Pcl & 0x3f);
        TxWin.TxPower[1] = (Pcl & 0x3f)+1;
        TxWin.TxPower[2] = (Pcl & 0x3f)+8;
        TxWin.TxPower[3] = (Pcl & 0x3f)+15;
    }
    else
    {
        TxWin.TxPower[0] = Pcl;
    }
    pal_SetTxWin(&TxWin);
}



/* ----------------- Send bursts of modulated zeros, at Dac power. */

VOID myl1_SendBurstDac(UINT16 arfcn, UINT16 DacId, UINT8 Band, UINT32 Tsc)
{
    pal_TxWin_t            TxWin;
    pal_NBlockDesc_t    NBlockDesc;
    UINT16 i;
    UINT32 seed_tmp = 0;
    UINT8 FillFrame_rand[25] = {    0,0,0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0
                               };

    /* Pseudo-Random Binary Sequence (PRBS) by a Linear-Feedback Shift */
    /* Non-zero coefficients are: c32 c28 c27 c1 */
    /* see http://www.eecircle.com/applets/009/LFSR.html */
    for (i=0; i<200; i++)
    {
        seed_tmp = ((seed >> 31) & 0x1) ^ ((seed >> 5) & 0x1) ^
                   ((seed >> 4) & 0x1) ^ ((seed >> 0) & 0x1);
        seed = seed >> 1;
        seed = (seed & 0x7FFFFFFF ) | ((seed_tmp<<31) & 0x80000000) ;

        FillFrame_rand[i/8]= FillFrame_rand[i/8] |
                             ((seed & 0x00000001)<<(i%8));
    }

    /* Init the frequency adjustment with 0. */
    pal_FrequencyTuning(0, arfcn);

    /* Send the burst. */
    if ((FINT_counter & 0x3) == 0)
    {
        NBlockDesc.Tsc    = Tsc;
        NBlockDesc.Cs     = CS4;
        NBlockDesc.Data   = FillFrame_rand;
        NBlockDesc.BufIdx = L1S_SACCH0_BUFFER;
        NBlockDesc.ChannelType = PAL_CCH_CHN;
        pal_NblockEncoding (&NBlockDesc);
    }

    FINT_counter = (FINT_counter + 1) % 51;

    /* Prog Tx win. */
    TxWin.Arfcn = arfcn;
    if((DacId & 0x3ff) < 5)
        TxWin.Start = 625*0;
    else
        TxWin.Start = 625*3;
    TxWin.WinIdx = 0;
    TxWin.BstIdx = (FINT_counter & 0x3);
    TxWin.Tsc = Tsc;
    TxWin.TA = 0;
    if((DacId & 0x3ff) < 5)
        TxWin.TxBitMap = 0xf;
    else
        TxWin.TxBitMap = 0x1;
    TxWin.RouteMap[0] = L1S_SACCH0_BUFFER;
    TxWin.TxPower[0] = 0;

    pal_CalibSetTxWin(&TxWin, DacId);
}



/* ----------------- myl1_Fn. */

UINT32 myl1_Fn(UINT16 t1, UINT8 t2, UINT8 t3)
{
    UINT8 mod26 = (t3 < t2) ? (26 + t3 - t2) : (t3 - t2);

    if (mod26 >= 26) mod26 -= 26;

    return ((51 * mod26) + t3 + (51 * 26 * t1));
}



/* ----------------- itoa. */

PRIVATE CHAR itoa(UINT8 input)
{
    UINT8 tmp;

    if (input<=9)
        tmp = '0' + input;
    else
        tmp = 'A' + input - 10;

    return((CHAR)tmp);
}



/* ----------------- my_msg2str. */

VOID my_msg2str(UINT8* msgp, CHAR* strp, UINT8 size)
{
    INT32 i;

    for (i=0; i<size-1; i++)
    {
        UINT8 tmp_msg;
        tmp_msg = ((*(msgp)) >> 4) & 0xF;
        *(strp++) = itoa(tmp_msg);
        tmp_msg = *(msgp++) & 0xF;
        *(strp++) = itoa(tmp_msg);
    }
    *strp = '\0';
}


