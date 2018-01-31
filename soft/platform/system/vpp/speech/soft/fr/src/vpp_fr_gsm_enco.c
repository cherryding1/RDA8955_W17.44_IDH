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







#include "vpp_fr_private.h"
#include "vpp_fr_gsm.h"
#include "vpp_fr_proto.h"

#include "vpp_fr_dtx.h"
#include "vpp_fr_vad.h"

#include "hal_speech.h"
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

//extern FILE *pfileParaFile;



extern  int frame;
//extern FILE *pfileVAD;


extern INT16 LARc_old_tx[8],xmaxc_old_tx;
extern INT16 txdtx_ctrl;



void gsm_encode P2((source, c), HAL_SPEECH_PCM_HALF_BUF_T source, HAL_SPEECH_ENC_OUT_T* encout)
{
    INT16           LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];
    int i,j;

    int  SID_flag=0;
    extern struct gsm_state g_r;
    gsm_byte* c = (INT8*)(encout->encOutBuf);

    VPP_FR_PROFILE_FUNCTION_ENTER(gsm_encode);


    Gsm_Coder(&g_r, source, LARc, Nc, bc, Mc, xmaxc, xmc);


    //SID frame
    /* Write comfort noise parameters into the parameter frame.
    Use old parameters in case SID frame is not to be updated */
    if ((txdtx_ctrl & TX_SP_FLAG) == 0)
    {
        SID_flag=1;      //used for debug!
        if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
        {
            for(i=0; i<8; i++) LARc_old_tx[i] = LARc[i];
            xmaxc_old_tx = xmaxc[0];
        }
        if ((txdtx_ctrl & TX_USE_OLD_SID) != 0)
        {
            for(i=0; i<8; i++) LARc[i] = LARc_old_tx[i];
            for(i=0; i<4; i++)xmaxc[i]  = xmaxc_old_tx;
            //for(i=0;i<4;i++) xmaxc[i] = xmaxc_old_tx;
        }

        for(i=0; i<4; i++)                               //add by Cui 2004.11.24
        {
            for(j=0; j<13; j++)
                xmc[i*13+j]=0;
        }                                                     //add end

    }


    if (0)
    {
        *c++ =   ((GSM_MAGIC & 0xF) << 4)               /* 1 */
                 | ((LARc[0] >> 2) & 0xF);
        *c++ =   ((LARc[0] & 0x3) << 6)
                 | (LARc[1] & 0x3F);
        *c++ =   ((LARc[2] & 0x1F) << 3)
                 | ((LARc[3] >> 2) & 0x7);
        *c++ =   ((LARc[3] & 0x3) << 6)
                 | ((LARc[4] & 0xF) << 2)
                 | ((LARc[5] >> 2) & 0x3);
        *c++ =   ((LARc[5] & 0x3) << 6)
                 | ((LARc[6] & 0x7) << 3)
                 | (LARc[7] & 0x7);
        *c++ =   ((Nc[0] & 0x7F) << 1)
                 | ((bc[0] >> 1) & 0x1);
        *c++ =   ((bc[0] & 0x1) << 7)
                 | ((Mc[0] & 0x3) << 5)
                 | ((xmaxc[0] >> 1) & 0x1F);
        *c++ =   ((xmaxc[0] & 0x1) << 7)
                 | ((xmc[0] & 0x7) << 4)
                 | ((xmc[1] & 0x7) << 1)
                 | ((xmc[2] >> 2) & 0x1);
        *c++ =   ((xmc[2] & 0x3) << 6)
                 | ((xmc[3] & 0x7) << 3)
                 | (xmc[4] & 0x7);
        *c++ =   ((xmc[5] & 0x7) << 5)                  /* 10 */
                 | ((xmc[6] & 0x7) << 2)
                 | ((xmc[7] >> 1) & 0x3);
        *c++ =   ((xmc[7] & 0x1) << 7)
                 | ((xmc[8] & 0x7) << 4)
                 | ((xmc[9] & 0x7) << 1)
                 | ((xmc[10] >> 2) & 0x1);
        *c++ =   ((xmc[10] & 0x3) << 6)
                 | ((xmc[11] & 0x7) << 3)
                 | (xmc[12] & 0x7);
        *c++ =   ((Nc[1] & 0x7F) << 1)
                 | ((bc[1] >> 1) & 0x1);
        *c++ =   ((bc[1] & 0x1) << 7)
                 | ((Mc[1] & 0x3) << 5)
                 | ((xmaxc[1] >> 1) & 0x1F);
        *c++ =   ((xmaxc[1] & 0x1) << 7)
                 | ((xmc[13] & 0x7) << 4)
                 | ((xmc[14] & 0x7) << 1)
                 | ((xmc[15] >> 2) & 0x1);
        *c++ =   ((xmc[15] & 0x3) << 6)
                 | ((xmc[16] & 0x7) << 3)
                 | (xmc[17] & 0x7);
        *c++ =   ((xmc[18] & 0x7) << 5)
                 | ((xmc[19] & 0x7) << 2)
                 | ((xmc[20] >> 1) & 0x3);
        *c++ =   ((xmc[20] & 0x1) << 7)
                 | ((xmc[21] & 0x7) << 4)
                 | ((xmc[22] & 0x7) << 1)
                 | ((xmc[23] >> 2) & 0x1);
        *c++ =   ((xmc[23] & 0x3) << 6)
                 | ((xmc[24] & 0x7) << 3)
                 | (xmc[25] & 0x7);
        *c++ =   ((Nc[2] & 0x7F) << 1)                  /* 20 */
                 | ((bc[2] >> 1) & 0x1);
        *c++ =   ((bc[2] & 0x1) << 7)
                 | ((Mc[2] & 0x3) << 5)
                 | ((xmaxc[2] >> 1) & 0x1F);
        *c++ =   ((xmaxc[2] & 0x1) << 7)
                 | ((xmc[26] & 0x7) << 4)
                 | ((xmc[27] & 0x7) << 1)
                 | ((xmc[28] >> 2) & 0x1);
        *c++ =   ((xmc[28] & 0x3) << 6)
                 | ((xmc[29] & 0x7) << 3)
                 | (xmc[30] & 0x7);
        *c++ =   ((xmc[31] & 0x7) << 5)
                 | ((xmc[32] & 0x7) << 2)
                 | ((xmc[33] >> 1) & 0x3);
        *c++ =   ((xmc[33] & 0x1) << 7)
                 | ((xmc[34] & 0x7) << 4)
                 | ((xmc[35] & 0x7) << 1)
                 | ((xmc[36] >> 2) & 0x1);
        *c++ =   ((xmc[36] & 0x3) << 6)
                 | ((xmc[37] & 0x7) << 3)
                 | (xmc[38] & 0x7);
        *c++ =   ((Nc[3] & 0x7F) << 1)
                 | ((bc[3] >> 1) & 0x1);
        *c++ =   ((bc[3] & 0x1) << 7)
                 | ((Mc[3] & 0x3) << 5)
                 | ((xmaxc[3] >> 1) & 0x1F);
        *c++ =   ((xmaxc[3] & 0x1) << 7)
                 | ((xmc[39] & 0x7) << 4)
                 | ((xmc[40] & 0x7) << 1)
                 | ((xmc[41] >> 2) & 0x1);
        *c++ =   ((xmc[41] & 0x3) << 6)                 /* 30 */
                 | ((xmc[42] & 0x7) << 3)
                 | (xmc[43] & 0x7);
        *c++ =   ((xmc[44] & 0x7) << 5)
                 | ((xmc[45] & 0x7) << 2)
                 | ((xmc[46] >> 1) & 0x3);
        *c++ =   ((xmc[46] & 0x1) << 7)
                 | ((xmc[47] & 0x7) << 4)
                 | ((xmc[48] & 0x7) << 1)
                 | ((xmc[49] >> 2) & 0x1);
        *c++ =   ((xmc[49] & 0x3) << 6)
                 | ((xmc[50] & 0x7) << 3)
                 | (xmc[51] & 0x7);

    }
    else
    {
        *c++ =((LARc[0] ) & 0x3F)                   /* 1 */
              | ((LARc[1] & 0x3)<<6 );
        *c++ =   ((LARc[1] & 0x3C)>>2)
                 | ((LARc[2] & 0xF) << 4);
        *c++ =  ((LARc[2] & 0x10) >> 4)
                | ((LARc[3] & 0x1F) <<1)
                |((LARc[4] & 0x3) << 6);
        *c++ =   ((LARc[4] & 0xc) >>2)
                 | ((LARc[5] & 0xF) << 2)
                 | ((LARc[6] & 0x3) <<6);
        *c++ =   ((LARc[6] & 0x4) >>2)
                 | ((LARc[7] & 0x7) << 1)
                 | ((Nc[0] & 0xF)<<4);
        *c++ =   ((Nc[0] & 0x70) >>4)
                 | ((bc[0] & 0x3) <<3)
                 | ((Mc[0] & 0x3) << 5)
                 |((xmaxc[0] & 1) <<7);
        *c++ =  ((xmaxc[0] & 0x3E) >>1)
                | ((xmc[0] & 0x7) << 5);

        *c++ =  ((xmc[1] & 0x7) )
                | ((xmc[2] & 0x7)<<3)
                | ((xmc[3] & 0x3)<<6);
        *c++ =  ((xmc[3] & 0x4) >> 2)
                | ((xmc[4] & 0x7)<<1)
                |((xmc[5] & 0x7) << 4)
                |((xmc[6] & 0x1) << 7);
        *c++ =                  /* 10 */
            ((xmc[6] & 0x6) >> 1)
            | ((xmc[7] & 0x7) <<2)
            | ((xmc[8] & 0x7) << 5);
        *c++ =  ((xmc[9] & 0x7) )
                | ((xmc[10]& 0x7) <<3)
                | ((xmc[11] & 0x3) << 6);
        *c++ =  ((xmc[11] & 0x4) >>2)
                | ((xmc[12] & 0x7)<<1)
                |((Nc[1] & 0x0F) << 4);
        *c++ =   ((Nc[1] & 0x70) >>4)
                 | ((bc[1] &0x3) <<3)
                 |((Mc[1] & 0x3) << 5)
                 |((xmaxc[1] & 0x1) <<7);
        *c++ = ((xmaxc[1] & 0x3E)>>1)
               |((xmc[13] & 0x7) << 5);


        *c++ = (xmc[14] & 0x7)
               | ((xmc[15] & 0x7)<<3)
               |((xmc[16] & 0x3) << 6);
        *c++ =   ((xmc[16] & 0x4) >>2)
                 | ((xmc[17] & 0x7)<<1)
                 |((xmc[18] & 0x7) << 4)
                 |((xmc[19] & 0x1) << 7);
        *c++ =  ((xmc[19] & 0x6)>>1)
                |((xmc[20] & 0x7) << 2)
                | ((xmc[21] & 0x7) << 5);
        *c++ =  (xmc[22] & 0x7)
                | ((xmc[23] & 0x7) <<3)
                |((xmc[24] & 0x3) << 6);
        *c++ =   ((xmc[24] & 0x4) >>2)
                 | ((xmc[25] & 0x7)<<1)
                 |((Nc[2] & 0x0F) << 4);
        *c++ =   ((Nc[2] & 0x70) >> 4)                  /* 20 */
                 | ((bc[2] & 0x3) <<3)
                 | ((Mc[2] & 0x3) << 5)
                 |((xmaxc[2] & 0x1) <<7);
        *c++ =  ((xmaxc[2] & 0x3E) >>1)
                |((xmc[26] & 0x7) << 5);

        *c++ =  (xmc[27] & 0x7)
                | ((xmc[28] & 0x7) <<3)
                | ((xmc[29] & 0x3) << 6);
        *c++ =   ((xmc[29] & 0x4) >>2)
                 | ((xmc[30] & 0x7)<<1)
                 |((xmc[31] & 0x7) << 4)
                 |((xmc[32] & 0x1) << 7);
        *c++ =  ((xmc[32] & 0x6) >>1)
                | ((xmc[33] & 0x7) <<2)
                |((xmc[34] & 0x7) << 5);
        *c++ =  (xmc[35] & 0x7)
                | ((xmc[36] & 0x7) <<3)
                |((xmc[37] & 0x3) << 6);
        *c++ =  ((xmc[37] & 0x4) >>2)
                | ((xmc[38] & 0x7)<<1)
                | ((Nc[3] & 0x0F) << 4);
        *c++ =   ((Nc[3] & 0x70) >> 4)
                 | ((bc[3] & 0x3) << 3)
                 |((Mc[3] & 0x3) << 5)
                 |((xmaxc[3] & 0x1) << 7);
        *c++ =  ((xmaxc[3] & 0x3E) >> 1)
                |((xmc[39] & 0x7) << 5);

        *c++ = (xmc[40] & 0x7)
               | ((xmc[41] & 0x7) <<3)
               | ((xmc[42] & 0x3) << 6) ;
        *c++ =                  /* 30 */
            ((xmc[42] & 0x4) >> 2)
            | ((xmc[43] & 0x7)<<1)
            |((xmc[44] & 0x7) << 4)
            | ((xmc[45] & 0x1) << 7);
        *c++ =  ((xmc[45] & 0x6) >>1)
                | ((xmc[46] & 0x7) <<2)
                |((xmc[47] & 0x7) << 5);
        *c++ =  (xmc[48] & 0x7)
                | ((xmc[49] & 0x7) <<3)
                |((xmc[50] & 0x3) << 6);
        *c++ = ((xmc[50] & 0x4) >>2)
               | ((xmc[51] & 0x7)<<1);

    }

    VPP_FR_PROFILE_FUNCTION_EXIT(gsm_encode);

}
