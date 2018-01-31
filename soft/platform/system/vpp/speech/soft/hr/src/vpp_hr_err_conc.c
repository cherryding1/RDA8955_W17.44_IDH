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







#include "vpp_hr_mathhalf.h"
#include "vpp_hr_sp_dec.h"
#include "vpp_hr_err_conc.h"
#include "vpp_hr_mathhalf_macro.h"



//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*_________________________________________________________________________
 |                                                                         |
 |                              Local Defines                              |
 |_________________________________________________________________________|
*/
#define  MIN_MUTE_LEVEL -45

/*_________________________________________________________________________
 |                                                                         |
 |                         State variables (globals)                       |
 |_________________________________________________________________________|
*/

INT32 plSubfrEnergyMem[4];
INT16 swLevelMem[4],
      lastR0,
      pswLastGood[18],
      swState,
      swLastFlag;


/*****************************************************************************
 *
 *   FUNCTION NAME: para_conceal_speech_decoder
 *
 *     This subroutine performs concealment on parameter level. If the
 *     badframe flag (swErrorFlag[0]) has been set in the channel decoder
 *     parameter repetition is performed.
 *     If the average frame energy R0 shows an abnormal increase between two
 *     subsequent frames the badframe flag is also set and parameter
 *     repetition is performed.
 *     If R0 shows an important increase muting is permitted in the signal
 *     concealment unit. There the level of the synthesized speech signal is
 *     controlled and corrected if necessary.
 *
 *     In table "psrR0RepeatThreshold[]" the maximum allowed
 *     increase of R0 for badframe setting is stored. The table
 *     is controlled by the value of R0 of the last frame.
 *     If e.g. the previous R0 is 10 the allowed maximum increase
 *     is 9 (psrR0RepeatThreshold[10]).
 *     The figures in psrR0RepeatThreshold[] have been determined
 *     by measuring the R0 statistics of an error free speech
 *     signal. In approximately 95 % of the frames the increase of
 *     R0 is less than the defined figures for error free speech.
 *     If the level increase is higher than the determined limit
 *     then the badframe flag is set.
 *
 *     In table "psrR0MuteThreshold[]" the maximum allowed
 *     increase of R0 for muting is stored.
 *     The table is controlled by the value of R0 of the last frame
 *     If e.g. the previous R0 is 10 the allowed maximum increase
 *     is 7 (psrR0MuteThreshold[10]).
 *     The figures in psrR0MuteThreshold[] have been determined
 *     by measuring the R0 statistics of an error free speech
 *     signal. In approximately 85 % of the frames the increase of
 *     R0 is less than the defined figures for error free speech.
 *     If the level increase is higher than the determined limit
 *     then muting is allowed.
 *
 *     Input:     pswErrorFlag[0]   badframe flag from channel decoder
 *                pswErrorFlag[1]   unreliable frame flag from channel decoder
 *                pswSpeechPara[]   unconcealed speech parameters
 *     Output:    pswSpeechPara[]   concealed speech parameters
 *                swMutePermit      flag, indicating whether muting is
 *                                  permitted
 *
 *     Constants: psrR0RepeatThreshold[32]  maximum allowed R0 difference
 *                                          before frame is repeated
 *                psrR0MuteThreshold[32]    maximum allowed R0 difference
 *                                          before muting is permitted
 *
 *
 ****************************************************************************/

void   para_conceal_speech_decoder(INT16 pswErrorFlag[],
                                   INT16 pswSpeechPara[], INT16 *pswMutePermit)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */
    static const INT16 psrR0RepeatThreshold[32] =
    {
        15, 15, 15, 12, 12, 12, 12, 11,
        10, 10, 9, 9, 9, 9, 8, 8,
        7, 6, 5, 5, 5, 4, 4, 3,
        2, 2, 2, 2, 2, 2, 10, 10
    };
    static const INT16 psrR0MuteThreshold[32] =
    {
        14, 12, 11, 9, 9, 9, 9, 7,
        7, 7, 7, 7, 6, 6, 6, 5,
        5, 4, 3, 3, 3, 3, 3, 2,
        1, 1, 1, 1, 1, 1, 10, 10
    };

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 swLastLag,
          swR0,
          swLag,
          r0_diff,
          i;


    //VPP_HR_PROFILE_FUNCTION_ENTER(para_conceal_speech_decoder );


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialise mute permission flag */
    /* ------------------------------- */
    *pswMutePermit = 0;

    /* Determine R0-difference to last frame */
    /* ------------------------------------- */
    //r0_diff = sub(pswSpeechPara[0], lastR0);
    r0_diff = SUB(pswSpeechPara[0], lastR0);

    /* If no badframe has been declared, but the frame is unreliable then */
    /* check whether there is an abnormal increase of R0                  */
    /* ------------------------------------------------------------------ */
    if ((pswErrorFlag[0] == 0) && (pswErrorFlag[1] > 0))
    {

        /* Check if difference exceeds the maximum allowed threshold. */
        /* If yes, set badframe flag                                  */
        /* ---------------------------------------------------------- */
        if ( r0_diff >= psrR0RepeatThreshold[lastR0] )
        {
            pswErrorFlag[0] = 1;
        }
        else
        {
            /* Allow muting if R0 >= 30 */
            /* ------------------------ */
            if ( pswSpeechPara[0]>= 30  )
                *pswMutePermit = 1;
        }
    }

    /* If no badframe has been declared, but the frame is unreliable then */
    /* check whether there is an important increase of R0                 */
    /* ------------------------------------------------------------------ */
    if ((pswErrorFlag[1] > 0) && (pswErrorFlag[0] == 0))
    {

        /* Check if difference exceeds a threshold.                   */
        /* If yes, allow muting in the signal concealment unit        */
        /* ---------------------------------------------------------- */
        if (  r0_diff >= psrR0MuteThreshold[lastR0] )
        {
            *pswMutePermit = 1;
        }
    }


    /* Perform parameter repetition, if necessary (badframe handling) */
    /* -------------------------------------------------------------- */

    if (pswErrorFlag[0] > 0)
    {
        //swState = add(swState, 1);         /* update the bad frame
        /*masking state */
        swState = ADD(swState, 1);         /* update the bad frame
                                        * masking state */
        if ( swState > 6  )
            swState = 6;
    }
    else
    {
        if ( swState < 6)
            swState = 0;
        else if (swLastFlag == 0)
            swState = 0;
    }

    swLastFlag = pswErrorFlag[0];

    /* if the decoded frame is good, save it */
    /* ------------------------------------- */
    if (swState == 0)
    {
        for (i = 0; i < 18; i++)
            pswLastGood[i] = pswSpeechPara[i];
    }

    /* if the frame is bad, attenuate and repeat last good frame */
    /* --------------------------------------------------------- */
    else
    {
        if ( ( swState >= 3) && ( swState <= 5 ))
        {
            swR0 = (pswLastGood[0]- 2);   /* attenuate by 4 dB */
            if (swR0 < 0)
                swR0 = 0;
            pswLastGood[0] = swR0;
        }

        if ( swState >= 6)          /* mute */
            pswLastGood[0] = 0;

        /* If the last good frame is unvoiced, use its energy, voicing mode, lpc
         * coefficients, and soft interpolation.   For gsp0, use only the gsp0
         * value from the last good subframe.  If the current bad frame is
         * unvoiced, use the current codewords.  If not, use the codewords from
         * the last good frame.               */
        /* -------------------------------------------------------------- */
        if (pswLastGood[5] == 0)
        {
            /* unvoiced good frame */
            if (pswSpeechPara[5] == 0)
            {
                /* unvoiced bad frame */
                for (i = 0; i < 5; i++)
                    pswSpeechPara[i] = pswLastGood[i];
                for (i = 0; i < 4; i++)
                    pswSpeechPara[3 * i + 8] = pswLastGood[17];
            }
            else
            {
                /* voiced bad frame */
                for (i = 0; i < 18; i++)
                    pswSpeechPara[i] = pswLastGood[i];
                for (i = 0; i < 3; i++)
                    pswSpeechPara[3 * i + 8] = pswLastGood[17];
            }
        }

        /* If the last good frame is voiced, the long term predictor lag at the
         * last subframe is used for all subsequent subframes. Use the last good
         * frame's energy, voicing mode, lpc coefficients, and soft
         * interpolation.  For gsp0 in all subframes, use the gsp0 value from the
         * last good subframe.  If the current bad frame is voiced, use the
         * current codewords.  If not, use the codewords from the last good
         * frame.                              */
        /* ---------------------------------------------------------------- */
        else
        {
            /* voiced good frame */
            swLastLag = pswLastGood[6];      /* frame lag */
            for (i = 0; i < 3; i++)
            {
                /* each delta lag */
                //swLag = sub(pswLastGood[3 * i + 9], 0x8);       /* biased around 0 */
                swLag = SUB(pswLastGood[3 * i + 9], 0x8);       /* biased around 0 */
                //swLag = ADD_SAT16(swLag, swLastLag); /* reconstruct pitch */
                swLag = ADD(swLag, swLastLag); /* reconstruct pitch */
                if ( swLag > 0x00ff)
                {
                    /* limit, as needed */
                    swLastLag = 0x00ff;
                }
                else if (swLag < 0)
                {
                    swLastLag = 0;
                }
                else
                    swLastLag = swLag;
            }
            pswLastGood[6] = swLastLag;      /* saved frame lag */
            pswLastGood[9] = 0x8;            /* saved delta lags */
            pswLastGood[12] = 0x8;
            pswLastGood[15] = 0x8;

            if (pswSpeechPara[5] != 0)
            {
                /* voiced bad frame */
                for (i = 0; i < 6; i++)
                    pswSpeechPara[i] = pswLastGood[i];
                for (i = 0; i < 4; i++)
                    pswSpeechPara[3 * i + 6] = pswLastGood[3 * i + 6];
                for (i = 0; i < 4; i++)
                    pswSpeechPara[3 * i + 8] = pswLastGood[17];
            }
            else
            {
                /* unvoiced bad frame */
                for (i = 0; i < 18; i++)
                    pswSpeechPara[i] = pswLastGood[i];
                for (i = 0; i < 3; i++)
                    pswSpeechPara[3 * i + 8] = pswLastGood[17];
            }
        }

    }                                    /* end of bad frame */


    /* Update last value of R0 */
    /* ----------------------- */
    lastR0 = pswSpeechPara[0];


//  VPP_HR_PROFILE_FUNCTION_EXIT(para_conceal_speech_decoder );



}


/****************************************************************************
 *
 *   FUNCTION NAME: level_estimator
 *
 *     This subroutine determines the mean level and the maximum level
 *     of the last four speech sub-frames. These parameters are the basis
 *     for the level estimation in signal_conceal_sub().
 *
 *     Input:     swUpdate      = 0: the levels are determined
 *                              = 1: the memory of the level estimator
 *                                   is updated
 *                pswDecodedSpeechFrame[]  synthesized speech signal
 *
 *     Output:    swLevelMean   mean level of the last 4 sub-frames
 *                swLevelMax    maximum level of the last 4 sub-frames
 *
 ***************************************************************************/

void   level_estimator(INT16 update, INT16 *pswLevelMean,
                       INT16 *pswLevelMax,
                       INT16 pswDecodedSpeechFrame[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 i,
          tmp,
          swLevelSub;
    INT32 L_sum;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    //VPP_HR_PROFILE_FUNCTION_ENTER(level_estimator );


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if (update == 0)
    {

        /* Determine mean level of the last 4 sub-frames: */
        /* ---------------------------------------------- */
        for (i = 0, L_sum = 0; i < 4; ++i)
        {
            L_sum =L_ADD(L_sum, plSubfrEnergyMem[i]);
        }
        *pswLevelMean = level_calc(1, &L_sum);

        /* Determine maximum level of the last 4 sub-frames: */
        /* ------------------------------------------------- */
        *pswLevelMax = -72;
        for (i = 0; i < 4; ++i)
        {
            if ( swLevelMem[i] > *pswLevelMax)
                *pswLevelMax = swLevelMem[i];
        }

    }
    else
    {
        /* Determine the energy of the synthesized speech signal: */
        /* ------------------------------------------------------ */
        for (i = 0, L_sum = 0; i < S_LEN; ++i)
        {
            tmp = SHR(pswDecodedSpeechFrame[i], 3);
            s_lo=0;
            //L_sum = L_mac(L_sum, tmp, tmp);
            VPP_MLA16(s_hi, s_lo, tmp, tmp);

        }
        L_sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
        swLevelSub = level_calc(0, &L_sum);

        /* Update memories of level estimator: */
        /* ----------------------------------- */
        for (i = 0; i < 3; ++i)
            plSubfrEnergyMem[i] = plSubfrEnergyMem[i + 1];
        plSubfrEnergyMem[3] = L_sum;

        for (i = 0; i < 3; ++i)
            swLevelMem[i] = swLevelMem[i + 1];
        swLevelMem[3] = swLevelSub;
    }


    //VPP_HR_PROFILE_FUNCTION_EXIT( level_estimator);



}


/*****************************************************************************
 *
 *   FUNCTION NAME: signal_conceal_sub
 *
 *     This subroutine performs concealment on subframe signal level.
 *     A test synthesis is performed and the level of the synthesized speech
 *     signal is compared to the estimated level. Depending on the control
 *     flag "swMutePermit" a muting factor is determined.
 *     If muting is permitted (swMutePermit=1) and the actual sub-frame level
 *     exceeds the maximum level of the last four sub-frames "swLevelMax" plus
 *     an allowed increase "psrLevelMaxIncrease[]" then the synthesized speech
 *     signal together with the signal memories is muted.
 *     In table "psrLevelMaxIncrease[]" the maximum allowed increase
 *     of the maximum sub-frame level is stored. The table is controled by the
 *     mean level "swMeanLevel".
 *     If e.g. the level is in the range between -30 and -35 db
 *     the allowed maximum increase is 4 db (psrLevelMaxIncrease[6]).
 *     The figures in psrLevelMaxIncrease[] have been determined
 *     by measuring the level statistics of error free synthesized speech.
 *
 *     Input:     pswPPFExcit[]            excitation signal
 *                pswSynthFiltState[]      state of LPC synthesis filter
 *                ppswSynthAs[]            LPC coefficients
 *                pswLtpStateOut[]         state of long term predictor
 *                pswPPreState[]           state of pitch prefilter
 *                swLevelMean              mean level
 *                swLevelMax               maximum level
 *                swUFI                    unreliable frame flag
 *                swMuteFlagOld            last muting flag
 *                pswMuteFlag              actual muting flag
 *                swMutePermit             mute permission
 *
 *     Output:    pswPPFExcit[]            muted excitation signal
 *                pswSynthFiltState[]      muted state of LPC synthesis filter
 *                pswLtpStateOut[]         muted state of long term predictor
 *                pswPPreState[]           muted state of pitch prefilter
 *
 *     Constants: psrConceal[0:15]         muting factors
 *                psrLevelMaxIncrease[0:7] maximum allowed level increase
 *
 *
 ****************************************************************************/

void   signal_conceal_sub(INT16 pswPPFExcit[],
                          INT16 ppswSynthAs[], INT16 pswSynthFiltState[],
                          INT16 pswLtpStateOut[], INT16 pswPPreState[],
                          INT16 swLevelMean, INT16 swLevelMax,
                          INT16 swUFI, INT16 swMuteFlagOld,
                          INT16 *pswMuteFlag, INT16 swMutePermit)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */
    static const INT16 psrConceal[15] = {29205, 27571, 24573, 21900,
                                         19519, 17396, 15504, 13818, 12315, 10976, 9783, 8719, 7771, 6925, 6172
                                        };
    static const INT16 psrLevelMaxIncrease[16] =
    {0, 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16};

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 swMute,
          swLevelSub,
          i,
          swIndex;
    INT16 swTmp,
          pswStateTmp[10],
          swOutTmp[40],
          swPermitMuteSub;
    INT32 L_sum;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    register INT32  m_hi=0;
    register UINT32 m_lo=0;

    //VPP_HR_PROFILE_FUNCTION_ENTER(signal_conceal_sub );


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Test synthesis filter: */
    /* ---------------------- */
    for (i = 0; i < 10; ++i)
        pswStateTmp[i] = pswSynthFiltState[i];

    lpcIir(pswPPFExcit, ppswSynthAs, pswStateTmp, swOutTmp);


    /* Determine level in db of synthesized signal: */
    /* -------------------------------------------- */
    //L_sum = 0;
    s_lo=0;
    for (i = 0; i < S_LEN; ++i)
    {
        swTmp = SHR(swOutTmp[i], 2);
        //L_sum = L_mac(L_sum, swTmp, swTmp);
        VPP_MLA16(s_hi, s_lo, swTmp, swTmp);
    }
    L_sum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
    swLevelSub = level_calc(0, &L_sum);


    /* Determine index to table, specifying the allowed level increase: */
    /* level [ 0 ..  -5] --> swIndex = 0       */
    /* level [-5 .. -10] --> swIndex = 1  etc. */
    /*---------------------------------------------*/
    swIndex = MULT(NEGATE(swLevelMean), 1638);

    if ( swIndex > 15)
        swIndex = 15;

    /* Muting is permitted, if it is signalled from the parameter concealment */
    /* unit or if muting has been performed in the last frame                */
    /*-----------------------------------------------------------------------*/
    swPermitMuteSub = swMutePermit;
    if (swMuteFlagOld > 0)
        swPermitMuteSub = 1;

    if (swPermitMuteSub > 0)
    {
        /* Muting is not permitted if the sub-frame level is less than */
        /* MIN_MUTE_LEVEL                                              */
        /* ------------------------------------------------------------ */
        if ( swLevelSub <= MIN_MUTE_LEVEL )
            swPermitMuteSub = 0;

        /* Muting is not permitted if the sub-frame level is less than */
        /* the maximum level of the last 4 sub-frames plus the allowed */
        /* increase                                                    */
        /* ------------------------------------------------------------ */
        //swMute = sub(swLevelSub, ADD_SAT16(swLevelMax, psrLevelMaxIncrease[swIndex]));
        swMute = SUB(swLevelSub, ADD(swLevelMax, psrLevelMaxIncrease[swIndex]));
        if (swMute <= 0)
            swPermitMuteSub = 0;
    }


    /* Perform muting, if allowed */
    /* -------------------------- */
    if (swPermitMuteSub > 0)
    {

        if ( swMute > (INT16) 15 )
            swMute = 15;

        /* Keep information that muting occured for next frame */
        /* --------------------------------------------------- */
        if (swUFI > 0)
            *pswMuteFlag = 1;


        /* Mute excitation signal: */
        /* ----------------------- */
        for (i = 0; i < 10; ++i)
            pswSynthFiltState[i] = MULT_R(pswSynthFiltState[i], psrConceal[swMute - 1]);
        /*VPP_MULT_R(m_hi, m_lo, pswSynthFiltState[i], psrConceal[swMute - 1]);
        pswSynthFiltState[i] = EXTRACT_H( L_MLA_SAT(m_hi, m_lo) );*/

        m_hi=0;
        m_lo=0;
        for (i = 0; i < S_LEN; ++i)
            pswPPFExcit[i] = MULT_R(pswPPFExcit[i], psrConceal[swMute - 1]);//
        /*VPP_MULT_R(m_hi, m_lo, pswPPFExcit[i], psrConceal[swMute - 1]);
        pswPPFExcit[i] = EXTRACT_H( L_MLA_SAT(m_hi, m_lo) );*/

        m_hi=0;
        m_lo=0;
        /* Mute pitch memory: */
        /* ------------------ */
        for (i = 0; i < S_LEN; ++i)
            pswLtpStateOut[i] = MULT_R(pswLtpStateOut[i], psrConceal[swMute - 1]);
        /*VPP_MULT_R(m_hi, m_lo, pswLtpStateOut[i], psrConceal[swMute - 1]);
        pswLtpStateOut[i] = EXTRACT_H( L_MLA_SAT(m_hi, m_lo) );*/


        m_hi=0;
        m_lo=0;
        /* Mute pitch prefilter memory: */
        /* ---------------------------- */
        for (i = 0; i < S_LEN; ++i)
            pswPPreState[i] = MULT_R(pswPPreState[i], psrConceal[swMute - 1]);
        /*VPP_MULT_R(m_hi, m_lo, pswPPreState[i], psrConceal[swMute - 1]);
        pswPPreState[i]= EXTRACT_H( L_MLA_SAT(m_hi, m_lo) );*/
    }


    //VPP_HR_PROFILE_FUNCTION_EXIT(signal_conceal_sub );



}


/****************************************************************************
 *
 *   FUNCTION NAME: level_calc
 *
 *     This subroutine calculates the level (db) from the energy
 *     of a speech sub-frame (swInd=0) or a speech frame (swInd=1):
 *     The level of a speech subframe is:
 *       swLevel =  10 * lg(EN/(40.*4096*4096))
 *               =   3 * ld(EN) - 88.27
 *               = (3*4*ld(EN) - 353)/4
 *               = (3*(4*POS(MSB(EN)) + 2*BIT(MSB-1) + BIT(MSB-2)) - 353)/4
 *
 *     Input:     pl_en      energy of the speech subframe or frame
 *                           The energy is multiplied by 2 because of the
 *                           MAC routines !!
 *                swInd      = 0: EN is the energy of one subframe
 *                           = 1: EN is the energy of one frame
 *
 *     Output:    swLevel    level in db
 *
 *
 ***************************************************************************/

INT16 level_calc(INT16 swInd, INT32 *pl_en)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 swPos,
          swLevel;
    INT32 L_tmp;


    //VPP_HR_PROFILE_FUNCTION_ENTER( level_calc);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if (*pl_en != 0)
        //swPos = sub((INT16) 29, norm_l(*pl_en));
        swPos = SUB(((INT16) 29), norm_l(*pl_en));

    else
        swPos = 0;

    /* Determine the term: 4*POS(MSB(EN)): */
    /* ----------------------------------- */
    swLevel = SHL(swPos, 2);

    /* Determine the term: 2*BIT(MSB-1): */
    /* --------------------------------- */
    if (swPos >= 0)
    {
        //L_tmp = L_shl((INT32) 1, swPos);
        L_tmp = SHL((INT32) 1, swPos);
        if ((*pl_en & L_tmp) != 0)
            swLevel += 2;
    }

    /* Determine the term: BIT(MSB-2): */
    /* ------------------------------- */
    if (--swPos >= 0)
    {
        //L_tmp = L_shl((INT32) 1, swPos);
        L_tmp = SHL((INT32) 1, swPos);
        if ((*pl_en & L_tmp) != 0)
            ++swLevel;
    }

    /* Multiply by 3: */
    /* -------------- */
    swLevel += SHL(swLevel, 1);
    swLevel -= (swInd == 0) ? 353 : 377;
    swLevel = MULT_R(swLevel, 0X2000);   /* >> 2 */

    if ( swLevel < -72 )
    {
        swLevel = -72;
        *pl_en = (swInd == 0) ? 80 : 320;
    }


    //VPP_HR_PROFILE_FUNCTION_EXIT(level_calc );


    return (swLevel);
}
