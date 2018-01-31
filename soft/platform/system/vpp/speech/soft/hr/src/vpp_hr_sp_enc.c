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





#include "hal_speech.h"
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_mathdp31.h"
#include "vpp_hr_sp_rom.h"
#include "vpp_hr_sp_dec.h"
#include "vpp_hr_sp_frm.h"
#include "vpp_hr_sp_sfrm.h"
#include "vpp_hr_sp_enc.h"
#include "vpp_hr_host.h"
#include "vpp_hr_vad.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*_________________________________________________________________________
 |                                                                         |
 |                              Local Defines                              |
 |_________________________________________________________________________|
*/

#define CG_INT_MACS       6            /* Number of Multiply-Accumulates in */
/* one interpolation                 */
#define ASCALE       0x0800
#define LMAX            142            /* largest lag (integer sense) */
#define LSMAX           (LMAX+ CG_INT_MACS/2)   /* Lag Search Array Length */
#define NUM_CLOSED        3            /* Maximum number of lags searched */
/* in closed loop.                 */

#define LPCSTARTINDEX     25           /* Where the LPC analysis window
                                        * starts */
#define INBUFFSZ          LPCSTARTINDEX + A_LEN /* Input buffer size  */
#define NUMSTARTUPSMP     INBUFFSZ - F_LEN      /* Number of samples needed */
/* at start up              */
#define NUMSTARTUPSMP_P1  INBUFFSZ - F_LEN + 1
#define HPFSHIFT          1            /* no right shifts high pass shifts
                                        * speech */

/*_________________________________________________________________________
 |                                                                         |
 |                     State variables (globals)                           |
 |_________________________________________________________________________|
*/

INT16 swOldR0;
INT16 swOldR0Index;

struct NormSw psnsWSfrmEngSpace[2 * N_SUB];

INT16 pswHPFXState[4];
INT16 pswHPFYState[8];
INT16 pswOldFrmKs[NP];
INT16 pswOldFrmAs[NP];
INT16 pswOldFrmSNWCoefs[NP];
INT16 pswWgtSpeechSpace[F_LEN + LMAX + CG_INT_MACS / 2];

INT16 pswSpeech[INBUFFSZ];         /* input speech */

INT16 swPtch = 1;

/*_________________________________________________________________________
 |                                                                         |
 |                         Global DTX variables                            |
 |_________________________________________________________________________|
*/

INT16 swTxGsHistPtr = 0;

INT16 pswCNVSCode1[N_SUB],
      pswCNVSCode2[N_SUB],
      pswCNGsp0Code[N_SUB],
      pswCNLpc[3],
      swCNR0;


extern INT32 pL_GsHist[];
extern INT32 ppLr_gsTable[4][32];


#define SHRW(nb, shift)                \
    ( shift== 0 ) ? (nb&0xffff) >>  0: \
    ( shift== 4 ) ? (nb&0xfff0) >>  4: \
    ( shift== 8 ) ? (nb&0xff00) >>  8: \
    (nb&0xf000) >> 12


/*static void writeBit(INT16 *tableCompressedOut, INT16 *pBit, INT16 NumberToCompress, INT16 NBBITS, INT16 *Idx)
{


    INT16 j=(*Idx), tmp;

    if (  NBBITS <= (16-(*pBit))  )
    {
        tmp=(*pBit);
        (tableCompressedOut[j])|=(NumberToCompress<<(*pBit));
        (*pBit)=((*pBit)+NBBITS)%16;
        if ( (*pBit)<=tmp )
            (*Idx)++;
    }
    else
    {
        tmp=16-(*pBit);
        (tableCompressedOut[j])|=(NumberToCompress<<((*pBit)));
        (*pBit)=0;
        (*Idx)++;

        NumberToCompress = SHRW(NumberToCompress,tmp);
        writeBit(tableCompressedOut, pBit, NumberToCompress   , NBBITS-tmp, Idx);
    }

}*/





/***************************************************************************
 *
 *   FUNCTION NAME: speechEncoder
 *
 *   PURPOSE:
 *
 *     Performs GSM half-rate speech encoding on frame basis (160 samples).
 *
 *   INPUTS:
 *
 *     pswSpeechIn[0:159] - input speech samples, 160 new samples per frame
 *
 *   OUTPUTS:
 *
 *     pswFrmCodes[0:19] - output parameters, 18 speech parameters plus
 *                         VAD and SP flags
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   IMPLEMENTATION:
 *
 *     n/a
 *
 *   REFERENCES: Sub-clause 4.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: speechcoder, analysis
 *
 *************************************************************************/

void   vpp_hr_encode(HAL_SPEECH_PCM_HALF_BUF_T EncInput, HAL_SPEECH_ENC_OUT_T* EncOutput)
//void   speechEncoder(INT16 pswSpeechIn[], INT16 pswFrmCodes[])
{
    INT16 pswFrmCodes[7];
    INT16 pswSpeechIn[160];
    /*_________________________________________________________________________
     |                                                                         |
     |                            Static Variables                             |
     |_________________________________________________________________________|
    */

    /* 1st point in analysis window */
    static INT16 *pswLpcStart = &pswSpeech[LPCSTARTINDEX];

    /* 1st point of new frame other than 1st */
    static INT16 *pswNewSpeech = &pswSpeech[NUMSTARTUPSMP];

    /* sample 0 of weighted speech */
    static INT16 *pswWgtSpeech = &pswWgtSpeechSpace[LSMAX];

    static struct NormSw *psnsWSfrmEng = &psnsWSfrmEngSpace[N_SUB];

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    iVoicing,                     /* bitAlloc */
           iR0,                          /* bitAlloc and aflat */
           piVq[3],                      /* bitAlloc */
           iSi,                          /* bitAlloc */
           piLagCode[N_SUB],             /* bitAlloc */
           piVSCode1[N_SUB],             /* bitAlloc */
           piVSCode2[N_SUB],             /* bitAlloc */
           piGsp0Code[N_SUB];            /* bitAlloc */

    short int siUVCode,
          siSi,
          i,
          j;

    INT16 swR0,
          pswLagCode[N_SUB],
          pswVSCode1[N_SUB],
          pswVSCode2[N_SUB],
          pswGsp0Code[N_SUB],
          *pswLagListPtr,
          pswFrmKs[NP],
          pswFrmAs[NP],
          pswFrmSNWCoefs[NP],
          pswLagList[N_SUB * NUM_CLOSED],
          pswNumLagList[N_SUB],
          pswPitchBuf[N_SUB],
          pswHNWCoefBuf[N_SUB],
          ppswSNWCoefAs[N_SUB][NP],
          ppswSynthAs[N_SUB][NP];

    INT16 swSP,
          pswVadLags[4],                /* VAD Parameters */
          swVadFlag;                    /* flag indicating voice activity
                                        * detector state.  1 = speech or
                                        * speech/signal present */
    struct NormSw
        psnsSqrtRs[N_SUB];


    /*INT16   pBit=0, Idx=0;
    INT16 EncOutput[15];
    FILE * fg1 = fopen("EncOutput_f.dat","a+");*/

    VPP_HR_PROFILE_FUNCTION_ENTER(vpp_hr_encode );

    for (i = 0; i < F_LEN; i++)
    {
        pswSpeechIn[i] = EncInput[i] ;//
    }

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Speech frame processing     */
    /* High pass filter the speech */
    /* ---------------------------- */

    filt4_2nd(psrHPFCoefs, pswSpeechIn, pswHPFXState, pswHPFYState, F_LEN, HPFSHIFT);

    /* copy high passed filtered speech into encoder's speech buff */
    /*-------------------------------------------------------------*/

    /*for (i = 0; i < F_LEN; i++)
      pswNewSpeech[i] = EncInput[i];*/

    for (i = 0; i < F_LEN; i++)
        pswNewSpeech[i] = pswSpeechIn[i];


    /* Calculate and quantize LPC coefficients */
    /* --------------------------------------- */

    aflat(pswLpcStart, &iR0, pswFrmKs, piVq,
          swPtch, &swVadFlag, &swSP);


    /* Lookup frame energy r0 */
    /* ---------------------- */

    swR0 = psrR0DecTbl[iR0 * 2];         /* lookupR0 */

    /* Generate the direct form coefs */
    /* ------------------------------ */

    if (!rcToADp(ASCALE, pswFrmKs, pswFrmAs))
    {

        getNWCoefs(pswFrmAs, pswFrmSNWCoefs);
    }
    else
    {

        for (i = 0; i < NP; i++)
        {
            pswFrmKs[i] = pswOldFrmKs[i];
            pswFrmAs[i] = pswOldFrmAs[i];
            pswFrmSNWCoefs[i] = pswOldFrmSNWCoefs[i];
        }
    }

    /* Interpolate, or otherwise get sfrm reflection coefs */
    /* --------------------------------------------------- */

    getSfrmLpcTx(swOldR0, swR0,
                 pswOldFrmKs, pswOldFrmAs,
                 pswOldFrmSNWCoefs,
                 pswFrmKs, pswFrmAs,
                 pswFrmSNWCoefs,
                 pswSpeech,
                 &siSi,
                 psnsSqrtRs,
                 ppswSynthAs, ppswSNWCoefAs);

    /* loose once bitAlloc done */
    iSi = siSi;

    /* Weight the entire speech frame */
    /* ------------------------------ */

    weightSpeechFrame(pswSpeech, ppswSynthAs[0], ppswSNWCoefAs[0],
                      pswWgtSpeechSpace);

    /* Perform open-loop lag search, get harmonic-noise-weighting parameters */
    /* --------------------------------------------------------------------- */

    openLoopLagSearch(&pswWgtSpeechSpace[LSMAX],
                      swOldR0Index,
                      (INT16) iR0,
                      &siUVCode,
                      pswLagList,
                      pswNumLagList,
                      pswPitchBuf,
                      pswHNWCoefBuf,
                      &psnsWSfrmEng[0],
                      pswVadLags,
                      swSP);

    iVoicing = siUVCode;


    /* Using open loop LTP data to calculate swPtch */      /* DTX mode */
    /* parameter                                    */      /* DTX mode */
    /* -------------------------------------------- */      /* DTX mode */

    periodicity_update(pswVadLags, &swPtch);                /* DTX mode */


    /* Subframe processing loop */
    /* ------------------------ */

    pswLagListPtr = pswLagList;

    for (giSfrmCnt = 0; giSfrmCnt < N_SUB; giSfrmCnt++)
    {

        if (swSP == 0)                                        /* DTX mode */
        {
            /* DTX mode */
            pswVSCode1[giSfrmCnt] = pswCNVSCode1[giSfrmCnt];    /* DTX mode */
            pswVSCode2[giSfrmCnt] = pswCNVSCode2[giSfrmCnt];    /* DTX mode */
            pswGsp0Code[giSfrmCnt] = pswCNGsp0Code[giSfrmCnt];  /* DTX mode */
        }                                                     /* DTX mode */

        sfrmAnalysis(&pswWgtSpeech[giSfrmCnt * S_LEN],
                     siUVCode,
                     psnsSqrtRs[giSfrmCnt],
                     ppswSNWCoefAs[giSfrmCnt],
                     pswLagListPtr,
                     pswNumLagList[giSfrmCnt],
                     pswPitchBuf[giSfrmCnt],
                     pswHNWCoefBuf[giSfrmCnt],
                     &pswLagCode[giSfrmCnt], &pswVSCode1[giSfrmCnt],
                     &pswVSCode2[giSfrmCnt], &pswGsp0Code[giSfrmCnt],
                     swSP);

        pswLagListPtr = &pswLagListPtr[pswNumLagList[giSfrmCnt]];

    }


    /* copy comfort noise parameters, */                     /* DTX mode */
    /* update GS history              */                     /* DTX mode */
    /* ------------------------------ */                     /* DTX mode */

    if (swSP == 0)                                           /* DTX mode */
    {
        /* DTX mode */

        /* copy comfort noise frame parameter */               /* DTX mode */
        /* ---------------------------------- */               /* DTX mode */

        iR0 = swCNR0; /* quantized R0 index */                 /* DTX mode */
        for (i=0; i < 3; i++)                                  /* DTX mode */
            piVq[i] = pswCNLpc[i];                               /* DTX mode */

    }                                                        /* DTX mode */
    else                                                     /* DTX mode */
    {
        /* DTX mode */

        /* if swSP != 0, then update the GS history */         /* DTX mode */
        /* -----------------------------------------*/         /* DTX mode */

        for (i=0; i < N_SUB; i++)                              /* DTX mode */
        {
            pL_GsHist[swTxGsHistPtr] =                           /* DTX mode */
                ppLr_gsTable[siUVCode][pswGsp0Code[i]];            /* DTX mode */
            swTxGsHistPtr++;                                     /* DTX mode */
            if (swTxGsHistPtr > ((OVERHANG-1)*N_SUB)-1)          /* DTX mode */
                swTxGsHistPtr=0;                                   /* DTX mode */
        }                                                      /* DTX mode */

    }                                                        /* DTX mode */


    /* End of frame processing, update frame based parameters */
    /* ------------------------------------------------------ */

    for (i = 0; i < N_SUB; i++)
    {
        piLagCode[i] = pswLagCode[i];
        piVSCode1[i] = pswVSCode1[i];
        piVSCode2[i] = pswVSCode2[i];
        piGsp0Code[i] = pswGsp0Code[i];
    }

    swOldR0Index = (INT16) iR0;
    swOldR0 = swR0;

    for (i = 0; i < NP; i++)
    {
        pswOldFrmKs[i] = pswFrmKs[i];
        pswOldFrmAs[i] = pswFrmAs[i];
        pswOldFrmSNWCoefs[i] = pswFrmSNWCoefs[i];
    }

    /* Insert SID Codeword */                                /* DTX mode */
    /* ------------------- */                                /* DTX mode */

    if (swSP == 0)                                           /* DTX mode */
    {
        /* DTX mode */
        iVoicing = 0x0003;            /* 2 bits */             /* DTX mode */
        iSi = 0x0001;                 /* 1 bit  */             /* DTX mode */
        for (i=0; i < N_SUB; i++)                              /* DTX mode */
        {
            /* DTX mode */
            piVSCode1[i] =  0x01ff;     /* 9 bits */             /* DTX mode */
            piGsp0Code[i] = 0x001f;     /* 5 bits */             /* DTX mode */
        }
        piLagCode[0] = 0x00ff;        /* 8 bits */             /* DTX mode */
        piLagCode[1] = 0x000f;        /* 4 bits */             /* DTX mode */
        piLagCode[2] = 0x000f;        /* 4 bits */             /* DTX mode */
        piLagCode[3] = 0x000f;        /* 4 bits */             /* DTX mode */
    }                                                        /* DTX mode */


    /* Generate encoded parameter array */
    /* -------------------------------- */

    fillBitAlloc(iVoicing, iR0, piVq, iSi, piLagCode,
                 piVSCode1, piVSCode2,
                 piGsp0Code, swVadFlag, swSP, pswFrmCodes);

    for  (i = 0; i < 7; ++i)
    {
        EncOutput->encOutBuf[i]=pswFrmCodes[i];
    }
    EncOutput->echoSkipEncFrame =swVadFlag;
    EncOutput->sp =swSP;


    /*for  (i = 0; i < 20; ++i)
    {
        EncOutput[i]=0;
    }
    for  (i = 0; i < 20; ++i)
    {
        writeBit(EncOutput, &pBit, pswFrmCodes[i]&0x0fff, 12, &Idx);
        //fprintf(fg1,"0x%04x,\n",EncOutput[i]);
    }
    for (i = 0 ; i < 15; i++)
    {
        fprintf(fg1,"0x%04x,\n",EncOutput[i]);
    }
    fprintf(fg1,"\n\n" );*/

    /* delay the input speech by 1 frame */
    /*-----------------------------------*/

    for (i = 0, j = F_LEN; j < INBUFFSZ; i++, j++)
    {
        pswSpeech[i] = pswSpeech[j];
    }


    VPP_HR_PROFILE_FUNCTION_EXIT(vpp_hr_encode );


}
