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







#include        "vpp_fr_config.h"


#ifdef  HAS_STDLIB_H
#include        <stdlib.h>
#else
#       include "proto.h"
extern char     * memcpy P((char *, char *, int));
#endif

#include        "vpp_fr_private.h"
#include        "vpp_fr_proto.h"

#include "vpp_fr_vad.h"
#include "vpp_fr_dtx.h"
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

/*
 *  4.2 FIXED POINT IMPLEMENTATION OF THE RPE-LTP CODER
 */

//fts add
//INT16 Nc_old_tx;
char subfrm;//add fts
// INT16 xmax_aver_tx;




extern INT16 sof[160], tone;
extern char dtx_mode;
extern INT16 txdtx_ctrl;
extern INT16 xmax_SID_tx[DTX_HANGOVER][4];

extern  void tone_detection (INT16 * sig_of, INT16 *);
//add end



void Gsm_Coder P8((S,s,LARc,Nc,bc,Mc,xmaxc,xMc),

                  struct gsm_state        * S,

                  INT16   * s,    /* [0..159] samples                     IN      */

                  /*
                   * The RPE-LTD coder works on a frame by frame basis.  The length of
                   * the frame is equal to 160 samples.  Some computations are done
                   * once per frame to produce at the output of the coder the
                   * LARc[1..8] parameters which are the coded LAR coefficients and
                   * also to realize the inverse filtering operation for the entire
                   * frame (160 samples of signal d[0..159]).  These parts produce at
                   * the output of the coder:
                   */

                  INT16   * LARc, /* [0..7] LAR coefficients              OUT     */

                  /*
                   * Procedure 4.2.11 to 4.2.18 are to be executed four times per
                   * frame.  That means once for each sub-segment RPE-LTP analysis of
                   * 40 samples.  These parts produce at the output of the coder:
                   */

                  INT16   * Nc,   /* [0..3] LTP lag                       OUT     */
                  INT16   * bc,   /* [0..3] coded LTP gain                OUT     */
                  INT16   * Mc,   /* [0..3] RPE grid selection            OUT     */
                  INT16   * xmaxc,/* [0..3] Coded maximum amplitude       OUT     */
                  INT16   * xMc   /* [13*4] normalized RPE samples        OUT     */
                 )
{
    int     k;
    INT16   * dp  = S->dp0 + 120;   /* [ -120...-1 ] */
    INT16   * dpp = dp;             /* [ 0...39 ]    */

//      INT16           ep[40];
    static INT16    e [50] = {0};

    INT16   so[160];

    long L_Temp,L_Temp1;
    int i,j  ,itest;

    INT16   * xmaxc_temp;
    INT16   * Nc_temp;

    INT16 temp, xmax_aver;

    INT16   exp;

    VPP_FR_PROFILE_FUNCTION_ENTER(Gsm_Coder);

    xmaxc_temp=xmaxc;

    /*  printf("\nsignal input:\n ");
        for(k=0;k<10;k++)
        {
            printf("s[%d]= 0x%x ", k,s[k]);
        }
    */
    Gsm_Preprocess                  (S, s, so);
    /*
        printf("\npreprocess output:\n");
        for(k=0;k<10;k++)
        {
            printf("so[%d]= 0x%x ", k,so[k]);

        }
    */
    Gsm_LPC_Analysis                (S, so, LARc);
//  for(k=0;k<8;k++)
//  {
    //if(k%8 == 0) printf("\n");
//      printf("LARc[%d]= 0x%x ", k,LARc[k]);

//  }

    Gsm_Short_Term_Analysis_Filter  (S, LARc, so);



    subfrm = 0;//add fts
//      xmaxc_temp = xmaxc;
    Nc_temp = Nc;
    for (k = 0; k <= 3; k++, xMc += 13)
    {

        Gsm_Long_Term_Predictor ( S,
                                  so+k*40, /* d      [0..39] IN  */
                                  dp,      /* dp  [-120..-1] IN  */
                                  e + 5,    /* e      [0..39] OUT */
                                  dpp,      /* dpp    [0..39] OUT */
                                  Nc++,
                                  bc++);


        Gsm_RPE_Encoding        ( S,
                                  e + 5,  /* e      ][0..39][ IN/OUT */
                                  xmaxc++, Mc++, xMc );

        /*
         * Gsm_Update_of_reconstructed_short_time_residual_signal
         *                      ( dpp, e + 5, dp );
         */

        {
            register int reg_i;
            register INT32 ltmp;
            for (reg_i = 0; reg_i <= 39; reg_i++)
                dp[ reg_i ] = (INT16) (GSM_SATADD( e[5 + reg_i], dpp[reg_i] ));
        }
        dp  += 40;
        dpp += 40;
        subfrm++;//add fts
    }




//add fts
    if ((txdtx_ctrl & TX_SP_FLAG) == 0)//SID frame
    {
        register UINT32 utmp;       /* for L_ADD */
        //xmax averaging
        L_Temp = 8;
        for(i=0; i<4; i++) //4 sub frame
        {
            for(j=0; j<DTX_HANGOVER; j++)
            {
                L_Temp1 =(long) xmax_SID_tx[j][i];
                L_Temp = GSM_L_ADD(L_Temp,L_Temp1);
            }
        }
        L_Temp = L_Temp >> 4;
        xmax_aver = (INT16)(L_Temp);


        /*  Qantizing and coding of xmax_aver to get xmaxc.
         */

        exp   = 0;
        temp  = SASR( xmax_aver ,  9 );
        itest = 0;

        for (i = 0; i <= 5; i++)
        {

            itest |= (temp <= 0);
            temp = SASR( temp, 1 );

            if (itest == 0) exp++;          /* exp = add (exp, 1) */
        }

        temp = exp + 5;

        temp = GSM_ADD( (INT16) SASR(xmax_aver, temp), (INT16) (exp << 3) );

        for(i=0; i<4; i++) *xmaxc_temp++  = temp;

    }
//add end



//add fts

    if (dtx_mode == 1)
    {
        periodicity_update (Nc_temp);//, &ptch);
        tone_detection (sof, &tone);
        //tone = 0;
    }

    /*
            if((txdtx_ctrl & TX_SID_UPDATE) != 0)
            {
                for(k=0;k<4;k++)
                        * xmaxc_temp ++ =  xmax_aver_tx;
            }
    */
//add end

    (void)memcpy( (char *)S->dp0, (char *)(S->dp0 + 160),
                  120 * sizeof(*S->dp0) );

    VPP_FR_PROFILE_FUNCTION_EXIT(Gsm_Coder);

}
