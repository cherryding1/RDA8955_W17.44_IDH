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
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

extern INT16 rxdtx_ctrl;
extern INT16 rx_dtx_state;

extern long L_pn_seed_rx;


//extern FILE *pfileVAD2;
//extern short TAF;


//extern int   dec_SID_flag;





INT16   LARc_old_rx[8], Nc_old_rx[4], Mc_old_rx[4], bc_old_rx[4], xmaxc_old_rx[4], xmc_old_rx[13*4];
INT16    lost_speech_frames; //bad frame counter
INT16 LARc_old_2_rx[8];//,  LARc_new_CN_rx[8];
INT16 xmaxc_old_2_rx[4];//, xmaxc_new_CN_rx;

int decoder_homing_frame_test(short LARc[],short Nc[],short bc[],short Mc[],short xmaxc[],short xmc[])
{
    int i,j;
    static const short dhf_mask[76] =
    {
        0x0009, 0x0017, 0x000F, 0x0008, 0x0007, 0x0003, 0x0003, 0x0002, // LARc[8]
        0x0028, 0x0028, 0x0028, 0x0028,     // Nc[4]
        0x0000, 0x0000, 0x0000, 0x0000,     // bc[4]
        0x0000, 0x0000, 0x0000, 0x0000,     // Mc[4]
        0x0000, 0x0000, 0x0000, 0x0000,     //xmaxc[4]
        0x0004, 0x0004, 0x0004, 0x0004,     // xmc[4*13]
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0003,
        0x0004, 0x0004, 0x0004, 0x0004,
        0x0004, 0x0004, 0x0004, 0x0004
    };

    VPP_FR_PROFILE_FUNCTION_ENTER(decoder_homing);

    for (i = 0; i < 8; i++)
    {
        j = LARc[i] ^ dhf_mask[i];

        if (j)
            goto THEEND001;
    }
    for (i = 0; i < 4; i++)
    {
        j = Nc[i] ^ dhf_mask[i+8];

        if (j)
            goto THEEND001;
    }
    for (i = 0; i < 4; i++)
    {
        j = bc[i] ^ dhf_mask[i+12];

        if (j)
            goto THEEND001;
    }
    for (i = 0; i < 4; i++)
    {
        j = Mc[i] ^ dhf_mask[i+16];

        if (j)
            goto THEEND001;
    }
    for (i = 0; i < 4; i++)
    {
        j = xmaxc[i] ^ dhf_mask[i+20];

        if (j)
            goto THEEND001;
    }
    for (i = 0; i < 52; i++)
    {
        j = xmc[i] ^ dhf_mask[i+24];

        if (j)
            goto THEEND001;
    }

THEEND001:
    VPP_FR_PROFILE_FUNCTION_EXIT(decoder_homing);
    return !j;
}



int frames=0;
int gsm_decode P2((c, target), HAL_SPEECH_DEC_IN_T* DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut)
{
    INT16   LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];

    int i, temp, dec_reset;
    extern struct gsm_state g_r;
    gsm_byte *c=(INT8*) (DecIn->decInBuf);

    //INT16 bfi=0,SID_flag;//fts add

    VPP_FR_PROFILE_FUNCTION_ENTER(gsm_decode);

    frames++;


    /* GSM_MAGIC  = (*c >> 4) & 0xF; */

    //  if (((*c >> 4) & 0x0F) != GSM_MAGIC) return -1;

    if(0)
    {
        LARc[0]  = (*c++ & 0xF) << 2;           /* 1 */
        LARc[0] |= (*c >> 6) & 0x3;
        LARc[1]  = *c++ & 0x3F;
        LARc[2]  = (*c >> 3) & 0x1F;
        LARc[3]  = (*c++ & 0x7) << 2;
        LARc[3] |= (*c >> 6) & 0x3;
        LARc[4]  = (*c >> 2) & 0xF;
        LARc[5]  = (*c++ & 0x3) << 2;
        LARc[5] |= (*c >> 6) & 0x3;
        LARc[6]  = (*c >> 3) & 0x7;
        LARc[7]  = *c++ & 0x7;
        Nc[0]  = (*c >> 1) & 0x7F;
        bc[0]  = (*c++ & 0x1) << 1;
        bc[0] |= (*c >> 7) & 0x1;
        Mc[0]  = (*c >> 5) & 0x3;
        xmaxc[0]  = (*c++ & 0x1F) << 1;
        xmaxc[0] |= (*c >> 7) & 0x1;
        xmc[0]  = (*c >> 4) & 0x7;
        xmc[1]  = (*c >> 1) & 0x7;
        xmc[2]  = (*c++ & 0x1) << 2;
        xmc[2] |= (*c >> 6) & 0x3;
        xmc[3]  = (*c >> 3) & 0x7;
        xmc[4]  = *c++ & 0x7;
        xmc[5]  = (*c >> 5) & 0x7;
        xmc[6]  = (*c >> 2) & 0x7;
        xmc[7]  = (*c++ & 0x3) << 1;            /* 10 */
        xmc[7] |= (*c >> 7) & 0x1;
        xmc[8]  = (*c >> 4) & 0x7;
        xmc[9]  = (*c >> 1) & 0x7;
        xmc[10]  = (*c++ & 0x1) << 2;
        xmc[10] |= (*c >> 6) & 0x3;
        xmc[11]  = (*c >> 3) & 0x7;
        xmc[12]  = *c++ & 0x7;
        Nc[1]  = (*c >> 1) & 0x7F;
        bc[1]  = (*c++ & 0x1) << 1;
        bc[1] |= (*c >> 7) & 0x1;
        Mc[1]  = (*c >> 5) & 0x3;
        xmaxc[1]  = (*c++ & 0x1F) << 1;
        xmaxc[1] |= (*c >> 7) & 0x1;
        xmc[13]  = (*c >> 4) & 0x7;
        xmc[14]  = (*c >> 1) & 0x7;
        xmc[15]  = (*c++ & 0x1) << 2;
        xmc[15] |= (*c >> 6) & 0x3;
        xmc[16]  = (*c >> 3) & 0x7;
        xmc[17]  = *c++ & 0x7;
        xmc[18]  = (*c >> 5) & 0x7;
        xmc[19]  = (*c >> 2) & 0x7;
        xmc[20]  = (*c++ & 0x3) << 1;
        xmc[20] |= (*c >> 7) & 0x1;
        xmc[21]  = (*c >> 4) & 0x7;
        xmc[22]  = (*c >> 1) & 0x7;
        xmc[23]  = (*c++ & 0x1) << 2;
        xmc[23] |= (*c >> 6) & 0x3;
        xmc[24]  = (*c >> 3) & 0x7;
        xmc[25]  = *c++ & 0x7;
        Nc[2]  = (*c >> 1) & 0x7F;
        bc[2]  = (*c++ & 0x1) << 1;             /* 20 */
        bc[2] |= (*c >> 7) & 0x1;
        Mc[2]  = (*c >> 5) & 0x3;
        xmaxc[2]  = (*c++ & 0x1F) << 1;
        xmaxc[2] |= (*c >> 7) & 0x1;
        xmc[26]  = (*c >> 4) & 0x7;
        xmc[27]  = (*c >> 1) & 0x7;
        xmc[28]  = (*c++ & 0x1) << 2;
        xmc[28] |= (*c >> 6) & 0x3;
        xmc[29]  = (*c >> 3) & 0x7;
        xmc[30]  = *c++ & 0x7;
        xmc[31]  = (*c >> 5) & 0x7;
        xmc[32]  = (*c >> 2) & 0x7;
        xmc[33]  = (*c++ & 0x3) << 1;
        xmc[33] |= (*c >> 7) & 0x1;
        xmc[34]  = (*c >> 4) & 0x7;
        xmc[35]  = (*c >> 1) & 0x7;
        xmc[36]  = (*c++ & 0x1) << 2;
        xmc[36] |= (*c >> 6) & 0x3;
        xmc[37]  = (*c >> 3) & 0x7;
        xmc[38]  = *c++ & 0x7;
        Nc[3]  = (*c >> 1) & 0x7F;
        bc[3]  = (*c++ & 0x1) << 1;
        bc[3] |= (*c >> 7) & 0x1;
        Mc[3]  = (*c >> 5) & 0x3;
        xmaxc[3]  = (*c++ & 0x1F) << 1;
        xmaxc[3] |= (*c >> 7) & 0x1;
        xmc[39]  = (*c >> 4) & 0x7;
        xmc[40]  = (*c >> 1) & 0x7;
        xmc[41]  = (*c++ & 0x1) << 2;
        xmc[41] |= (*c >> 6) & 0x3;
        xmc[42]  = (*c >> 3) & 0x7;
        xmc[43]  = *c++ & 0x7;                  /* 30  */
        xmc[44]  = (*c >> 5) & 0x7;
        xmc[45]  = (*c >> 2) & 0x7;
        xmc[46]  = (*c++ & 0x3) << 1;
        xmc[46] |= (*c >> 7) & 0x1;
        xmc[47]  = (*c >> 4) & 0x7;
        xmc[48]  = (*c >> 1) & 0x7;
        xmc[49]  = (*c++ & 0x1) << 2;
        xmc[49] |= (*c >> 6) & 0x3;
        xmc[50]  = (*c >> 3) & 0x7;
        xmc[51]  = *c & 0x7;                    /* 33 */
    }
    else
    {
        //注意这里没有检查
        LARc[0]  = *c & 0x3F;           /* 1 */
        LARc[1]  = (*c++ & 0xC0)>>6;
        LARc[1] |= ((*c & 0xF)<<2);
        LARc[2]  = (*c++ & 0xF0) >>4;
        LARc[2] |= (*c &0x1)<<4;
        LARc[3]  = (*c & 0x3E) >>1;
        LARc[4]  = (*c++ & 0xC0) >>6;
        LARc[4] |= (*c & 0x3)<<2;
        LARc[5]  = (*c & 0x3C) >> 2;
        LARc[6]  = (*c++ & 0xC0) >>6;
        LARc[6] |= (*c & 0x1)<<2;
        LARc[7]  = (*c & 0xE)>>1;

        Nc[0]   = (*c++ & 0xF0) >>4;
        Nc[0] |= (*c & 0x7) <<4;
        bc[0]  = (*c & 0x18) >>3;
        Mc[0]  = (*c & 0x60) >>5;
        xmaxc[0]  = (*c++ & 0x80) >>7;
        xmaxc[0] |= (*c & 0x1F) <<1;
        xmc[0]  = (*c++ & 0xE0) >>5;
        xmc[1]  = *c & 0x7;
        xmc[2]  = (*c & 0x38) >>3;
        xmc[3]  = (*c++ & 0xC0) >>6;
        xmc[3] |= (*c & 0x1) << 2;
        xmc[4]  = (*c & 0xE)>>1;
        xmc[5]  = (*c & 0x70)>>4 ;
        xmc[6]  = (*c++ & 0x80) >>7;
        xmc[6] |= (*c & 0x3)<<1;
        xmc[7]  = (*c & 0x1C) >>2;          /* 10 */
        xmc[8]  = (*c++ & 0xE0) >>5;
        xmc[9]  = *c & 0x7;
        xmc[10]  = (*c & 0x38) >>3;
        xmc[11]  = (*c++ & 0xC0) >>6;
        xmc[11] |=(*c & 0x1) <<2;
        xmc[12]  = (*c & 0xE)>>1;
        Nc[1]  = (*c++ & 0xF0)>>4;
        Nc[1]  |=(*c & 0x7)<<4;
        bc[1]  = (*c & 0x18) >>3;
        Mc[1]  = (*c & 0x60) >>5;
        xmaxc[1]  = (*c++ & 0x80) >>7;
        xmaxc[1] |= (*c & 0x1F) <<1;

        xmc[13]  = (*c++ & 0xE0)>>5 ;
        xmc[14]  = (*c & 0x7)  ;
        xmc[15]  = (*c & 0x38) >>3;
        xmc[16]  = (*c++ & 0xC0) >>6;
        xmc[16] |= (*c & 0x1) <<2;
        xmc[17]  = (*c & 0xE)>>1;
        xmc[18]  = (*c & 0x70) >>4;
        xmc[19]  = (*c++  & 0x80)>>7;
        xmc[19] |= (*c & 0x3) <<1;
        xmc[20]  = (*c & 0x1C) >>2;
        xmc[21]  = (*c++ & 0xE0) >>5;
        xmc[22]  = (*c & 0x7)  ;
        xmc[23]  = (*c & 0x38) >>3;
        xmc[24]  = (*c++ & 0xC0) >>6;
        xmc[24] |= (*c & 0x1) <<2;
        xmc[25]  = (*c & 0xE)>>1;
        Nc[2]   = (*c++ & 0xF0 ) >>4;
        Nc[2]  |= (*c & 0x7)<<4;
        bc[2]  = (*c & 0x18) >>3;           /* 20 */
        Mc[2]  = (*c & 0x60) >>5;
        xmaxc[2]  = (*c++ & 0x80)>>7 ;
        xmaxc[2] |= (*c & 0x1F)<<1 ;

        xmc[26]  = (*c++ & 0xE0) >>5;
        xmc[27]  = (*c & 0x7) ;
        xmc[28]  = (*c & 0x38) >>3;
        xmc[29]  = (*c++ & 0xC0) >>6;
        xmc[29] |= (*c & 0x1) <<2;
        xmc[30]  = (*c & 0xE)>>1;
        xmc[31]  = (*c & 0x70 ) >>4;
        xmc[32]  = (*c++  & 0x80)>>7;
        xmc[32] |= (*c & 0x3)<<1;
        xmc[33]  = (*c & 0x1C) >>2;
        xmc[34]  = (*c++ & 0xE0) >>5;
        xmc[35]  = (*c & 0x7)  ;
        xmc[36]  = (*c & 0x38) >>3;
        xmc[37]  = (*c++ & 0xC8) >>6;
        xmc[37] |= (*c & 0x1) <<2;
        xmc[38]  = (*c & 0xE) >>1;
        Nc[3]  = (*c++ & 0xF0) >>4;
        Nc[3] |= (*c & 0x7) <<4;
        bc[3]  = (*c & 0x18) >>3;
        Mc[3]  = (*c & 0x60) >>5;
        xmaxc[3]  = (*c++ & 0x80)>>7 ;
        xmaxc[3] |= (*c & 0x1F)<<1 ;

        xmc[39]  = (*c++ & 0xE0) >>5;
        xmc[40]  = (*c & 0x7) ;
        xmc[41]  = (*c & 0x38) >>3;
        xmc[42]  = (*c++ & 0xC0) >>6;
        xmc[42] |= (*c & 0x1) <<2;
        xmc[43]  = (*c & 0xE)>>1;                   /* 30  */
        xmc[44]  = (*c & 0x70 ) >>4;
        xmc[45]  = (*c++ & 0x80)>>7;
        xmc[45] |= (*c & 0x3) <<1;
        xmc[46]  = (*c & 0x1C) >>2;
        xmc[47]  = (*c++ & 0xE0) >>5;
        xmc[48]  = (*c & 0x7)  ;
        xmc[49]  = (*c & 0x38) >>3;
        xmc[50]  = (*c++ & 0xC0) >>6;
        xmc[50] |= (*c & 0x1) <<2;
        xmc[51]  = (*c & 0xE) >>1;                  /* 33 */
    }

    DecIn->sid = sid_frame_detection (&xmc[0]);

    if(DecIn->sid)
    {
        i=0;        // used for debug SID_flag
    }

//fts add for dtx
    dec_reset = decoder_homing_frame_test(LARc,Nc,bc,Mc,xmaxc,xmc);
    if(dec_reset != 0)
    {
        for(i=0; i<160; i++)
            DecOut[i] = 0x0008;
        dtx_init();

        VPP_FR_PROFILE_FUNCTION_EXIT(gsm_decode);
        return 0;
    }

    /*  if(SID_flag != 0 )
        {
            SID_flag = 0;

        }
    */
    rx_dtx (&rxdtx_ctrl, DecIn->taf, DecIn->bfi, DecIn->sid);

    //store nowly parameter of good speech frame
    if((DecIn->bfi==0) && (DecIn->sid == 0))
    {
        for(i=0; i<4; i++)
        {
            Nc_old_rx[i]    =  Nc[i];
            bc_old_rx[i]    =  bc[i];
            Mc_old_rx[i]    =  Mc[i];
        }
        for(i=0; i<52; i++)  xmc_old_rx[i] = xmc[i];

        for(i=0; i<8; i++) LARc_old_rx[i]  = LARc[i];
        for(i=0; i<4; i++) xmaxc_old_rx[i] = xmaxc[i];
    }



    DecIn->sid=0;            //for debug

    //no transmission period
    if (((rxdtx_ctrl & RX_NO_TRANSMISSION) != 0)||((rxdtx_ctrl & RX_INVALID_SID_FRAME) != 0))
    {

        DecIn->sid=1;                //for debug

        for(i=0; i<4; i++)
            xmaxc[i] = interpolate_CN_param (xmaxc_old_2_rx[i], xmaxc_old_rx[i], rx_dtx_state);

        for(i=0; i<8; i++)
            LARc[i] = interpolate_CN_param (LARc_old_2_rx[i], LARc_old_rx[i], rx_dtx_state);

    }


    //silence frame parameters
    if((lost_speech_frames) || ((rxdtx_ctrl & RX_LOST_SID_FRAME) != 0))
    {
        //refer to GSM 06.11

        DecIn->sid=2;                     //for debug!!

        lost_frame_process(LARc, Nc, bc, Mc, xmaxc, xmc,&L_pn_seed_rx);

        temp = 64 ;
        for(i=0; i<4; i++)
        {
            if(xmaxc[i] < temp) temp = xmaxc[i];
        }

        /*
                    if((temp == 0) || lost_speech_frames > 3)
                    {
                        silence_frame_paramer(LARc, Nc, bc, Mc, xmaxc, xmc);               //old
                    }
        */
        if((temp <3 )  || lost_speech_frames > 16)                                         //change by Cui  2004.11.23
        {
            silence_frame_paramer(LARc, Nc, bc, Mc, xmaxc, xmc);          //The output will muted after a maximum of 320 ms.
        }

    }


    if(((rxdtx_ctrl & RX_CONT_SID_UPDATE) != 0) ||(rxdtx_ctrl & RX_FIRST_SID_UPDATE) != 0)
    {

        DecIn->sid=1;                        //for debug!

        for(i=0; i<8; i++)
        {
            LARc_old_2_rx[i]  = LARc_old_rx[i];
            LARc_old_rx[i]  = LARc[i];
        }

        for(i=0; i<4; i++)
        {
            xmaxc_old_2_rx[i] = xmaxc_old_rx[i];
            xmaxc_old_rx[i] = xmaxc[i];
        }

        CN_paramer( Nc, bc, Mc, xmc, &L_pn_seed_rx);

    }

//add end

    // fprintf(pfileVAD2, "frame   %d # ,                       %d      \n",      frame,dec_SID_flag);

    Gsm_Decoder(&g_r, LARc, Nc, bc, Mc, xmaxc, xmc, DecOut);

    VPP_FR_PROFILE_FUNCTION_EXIT(gsm_decode);

    return 0;
}


void lost_frame_process(
    INT16    * LARc,        /* [0..7]               OUT      */
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmaxc,       /* [0..3]               OUT      */
    INT16    * xmc,         /* [0..13*4]            OUT      */
    long    * seed

)
{
    int i ;

    VPP_FR_PROFILE_FUNCTION_ENTER(lost_frame_process);

    for(i=0; i<8; i++)
        LARc[i]  = LARc_old_rx[i];


    for(i=0; i<4; i++)
    {
        Nc[i]    =  Nc_old_rx[i];
        bc[i]    =  bc_old_rx[i];


        if((lost_speech_frames==1) ||((rxdtx_ctrl & RX_DTX_MUTING) == 0))        // change by Cui  2004.11.23   not mute!!!

            //  if((lost_speech_frames==1) ||(RX_LOST_SID_FRAME ==1))
        {
            xmaxc[i] =  xmaxc_old_rx[i];
        }
        else
        {
            //xmaxc[i] =  xmaxc_old_rx[i];// - 4;                                                       //old

            xmaxc[i] =  xmaxc_old_rx[i] - 4;                                                            //change by Cui  2004.11.23
            xmaxc_old_rx[i] =  xmaxc[i];
        }
    }

    //if((lost_speech_frames==1)||(RX_LOST_SID_FRAME ==1))

    if((lost_speech_frames==1)||((rxdtx_ctrl & RX_DTX_MUTING) == 0))            //   // change by Cui  2004.11.23    not mute!!!
    {
        for(i=0; i<4; i++)    Mc[i] = Mc_old_rx[i];
    }
    else
    {
        for(i=0; i<4; i++) Mc[i] = pseudonoise (seed, 2);
    }

    for(i=0; i<52; i++)
    {
        xmc[i]   = xmc_old_rx[i];
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(lost_frame_process);

}

void silence_frame_paramer(
    INT16    * LARc,        /* [0..7]               OUT      */
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmaxc,       /* [0..3]               OUT      */
    INT16    * xmc          /* [0..13*4]            OUT      */
)
{
    int i ;

    VPP_FR_PROFILE_FUNCTION_ENTER(silence_frame_paramer);

    LARc[0]  = 42;
    LARc[1]  = 39;
    LARc[2]  = 21;
    LARc[3]  = 10;
    LARc[4]  = 9;
    LARc[5]  = 4;
    LARc[6]  = 3;
    LARc[7]  = 2;

    for(i=0; i<4; i++)
    {
        Nc[i]    =  0;
        bc[i]    = 40;

        Mc[i]    =  1;
        xmaxc[i] =  0;
    }


    for(i=0; i<4; i++)
    {
        xmc[0+i*13]  = 3;
        xmc[1+i*13]  = 4;
        xmc[2+i*13]  = 3;
        xmc[3+i*13]  = 4;
        xmc[4+i*13]  = 4;
        xmc[5+i*13]  = 3;
        xmc[6+i*13]  = 3;
        xmc[7+i*13]  = 3;
        xmc[8+i*13]  = 3;
        xmc[9+i*13]  = 4;
        xmc[10+i*13] = 4;
        xmc[11+i*13] = 3;
        xmc[12+i*13] = 3;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(silence_frame_paramer);


}

void CN_paramer(
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmc,         /* [0..13*4]            OUT      */
    long    * seed
)
{
    int i ;
    INT16 temp;

    VPP_FR_PROFILE_FUNCTION_ENTER(CN_paramer);

    for(i=0; i<4; i++)
    {
        bc[i]    = 0;
    }

    Nc[0]  = 40;    Nc[1]  = 120;
    Nc[2]  = 40;    Nc[3]  = 120;

    for(i=0; i<4; i++) Mc[i] = pseudonoise (seed, 2);

    for(i=0; i<52; i++)
    {
        do
        {
            temp=pseudonoise (seed, 3);
        }
        while (temp==0||temp==7);

        xmc[i]   = temp;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(CN_paramer);

}
