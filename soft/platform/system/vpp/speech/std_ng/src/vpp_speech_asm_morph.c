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




#include "vppp_speech_common.h"


#if 0

voc_short For_MORPH_align   ,816,x



voc_short gInFIFO_ADDR[256],x
voc_short gOutFIFO_ADDR[256],x
voc_short ingLastPhase_ADDR[130],x
voc_short ingOutputAccum_ADDR[512],x





voc_short FFT_INPUT_ADDRESS[256],x
voc_short FFT_OUTPUT_ADDRESS[256],x
voc_short ingFFTworksp_ADDRESS[1024],x

voc_short sign_ADDRESS[2],x
voc_short TEMP_UR_ADDRESS,x
voc_short TEMP_UI_ADDRESS,x
voc_short TEMP_LE2_ADDRESS,x
voc_short TEMP_LE_ADDRESS,x
voc_short WR_ADDRESS,x
voc_short WI_ADDRESS,x
voc_short TEMP_ADDR_ADDRESS,x
voc_short TEMP1_ADDR_ADDRESS,x
voc_short ingAnaMagn_ADDRESS[512],x
voc_short ingAnaFreq_ADDRESS[256],x
voc_short ingSynFreq_ADDR[256],x
voc_short ingSynMagn_ADDRESS[512],x
voc_short GINOUT_ADDR_ADDRESS[2],x


#endif


void CII_smbPitchShift(void)
{

    VoC_push16(RA,DEFAULT);


    //morph voice const
    VoC_lw32_d(REG45,GLOBAL_MORPH_CONSTX_ADDR);

    VoC_lw16i(REG2,COS_TABLE_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_MORPHVOICE_CONST_SIZE);
    VoC_jal(Rda_RunDma);

    VoC_jal(Coolsand_MorphVoiceReset);


    VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);
    VoC_lw16i_set_inc(REG1,FFT_INPUT_ADDRESS,2);

    VoC_lw16i(REG2,3);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_loop_i(0,80)
    VoC_shr16_rr(REG4,REG2,DEFAULT);
    VoC_shr16_rr(REG5,REG2,IN_PARALLEL);


    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_endloop0




    VoC_lw16i_short(REG4,0,DEFAULT);                // REG4 for i
    VoC_lw16i_short(REG5,80,IN_PARALLEL);           // REG5 for loop

    VoC_lw16i_set_inc(REG2,gInFIFO_ADDR,2);         // gInFIFO[gRover]
    VoC_lw16i_set_inc(REG0,FFT_OUTPUT_ADDRESS,2);   // outdata[i]
    VoC_lw16i_set_inc(REG3,gOutFIFO_ADDR,2);    // gOutFIFO[gRover-inFifoLatency]
    VoC_lw16_d(REG6,gRover_ADDR);                   // REG6 for gRover
    VoC_lw16i(REG7,192);

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_sub16_rr(REG7,REG6,REG7,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,FFT_INPUT_ADDRESS,2);    // indata[i]

    VoC_sw32_d(REG23,GINOUT_ADDR_ADDRESS);          // save gInFIFO[gRover] and gOutFIFO[gRover-inFifoLatency] addr
CII_smbPitchShift102:
    VoC_bngt16_rr(CII_smbPitchShift101, REG5, REG4) //  for (i = 0; i < numSampsToProcess; i++)

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_lw32inc_p(ACC1,REG3,DEFAULT);   // outdata[i] = gOutFIFO[gRover-inFifoLatency];
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_lw32inc_p(ACC0,REG1,DEFAULT);   // gInFIFO[gRover] = indata[i];
    VoC_add16_rr(REG6,REG6,REG7,IN_PARALLEL);   // gRover ++

    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    VoC_lw16i(REG7,256);

    VoC_bgt16_rr(CII_smbPitchShift103,REG7,REG6)

    VoC_lw16i_short(REG7,2,DEFAULT);

    VoC_push32(REG01,DEFAULT);      // save outdata[i] and indata[i] addr
    VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);      // save i
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,ingFFTworksp_ADDRESS,2); // ingFFTworksp[2*k]
    VoC_lw16i_set_inc(REG1,COS_TABLE_ADDR,1);   // in_costable[k]
    VoC_lw16i_set_inc(REG2,gInFIFO_ADDR,1); // gInFIFO[k]
    VoC_lw16i(REG7,(COS_TABLE_ADDR+128));
    VoC_lw16i(REG6,0X2000);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);           //  in_windows = 0x2000 - (in_costable[k]>>1);  // Q2.14

    VoC_multf32_rr(RL7,REG4,REG5,DEFAULT);

    VoC_loop_i(1,255)               // for (k = 0; k < fftFrameSize;k++)
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw32inc_p(RL7,REG0,DEFAULT);        //      ingFFTworksp[2*k] = (long)(gInFIFO[k] * in_windows);

    VoC_bne16_rr(CII_smbPitchShift120,REG1,REG7)
    VoC_lw16i_short(INC1,-1,DEFAULT);
CII_smbPitchShift120:

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);           //  in_windows = 0x2000 - (in_costable[k]>>1);  // Q2.14
    VoC_multf32_rr(RL7,REG4,REG5,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);        //      ingFFTworksp[2*k+1] = 0;
    VoC_endloop1;

    VoC_lw16i_short(REG4,-1,DEFAULT);
    VoC_sw32inc_p(RL7,REG0,DEFAULT);        //      ingFFTworksp[2*k] = (long)(gInFIFO[k] * in_windows);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);        //      ingFFTworksp[2*k+1] = 0;
    VoC_sw16_d(REG4,sign_ADDRESS);

    VoC_jal(CS_intsmbFft);

    VoC_lw16i_set_inc(REG0,ingFFTworksp_ADDRESS,2); // ingFFTworksp[2*k]
    VoC_lw16i_set_inc(REG1,ingAnaMagn_ADDRESS,2);   // ingAnaMagn[k]
    VoC_lw16i_set_inc(REG2,ingLastPhase_ADDR,1);    // ingLastPhase
    VoC_lw16i_set_inc(REG3,ingAnaFreq_ADDRESS,1);   // ingAnaFreq[k]
    VoC_lw16i_short(REG7,0,DEFAULT);                // REG7 for k

    VoC_loop_i(1,129)               // for (k = 0; k <= fftFrameSize2; k++)
    VoC_lw32inc_p(REG45,REG0,DEFAULT);          // REG4 for inreal
    VoC_push32(REG67,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);          // REG6 for inimag
    VoC_shr32_ri(REG45,3,IN_PARALLEL);

    VoC_shr32_ri(REG67,3,DEFAULT);
    VoC_multf32_rr(RL6,REG4,REG4,IN_PARALLEL);

    VoC_multf32_rr(RL7,REG6,REG6,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);    //  tmp = 0;

    VoC_lw16i(REG3,0X4000);                 // flag = 0x4000;
    VoC_lw16i_short(REG2,0,DEFAULT)

    VoC_loop_i_short(15,DEFAULT)        // for (flag1 = 0;flag1 < 16; flag1++)
    VoC_add32_rr(RL6,RL6,RL7,IN_PARALLEL);  // temp = inreal*inreal + inimag*inimag;
    VoC_startloop0
    VoC_add16_rr(REG5,REG2,REG3,DEFAULT);   // tmp = tmp + flag;
    VoC_multf32_rr(RL7,REG5,REG5,DEFAULT);  // temp1 = tmp * tmp;
    VoC_NOP();
    VoC_bgt32_rr(CII_smbPitchShift104,RL7,RL6)      // if(temp1 > temp)
    VoC_movreg16(REG2,REG5,DEFAULT);            // tmp = tmp-flag;
CII_smbPitchShift104:
    VoC_shr16_ri(REG3,1,DEFAULT);       // flag = flag >>1;
    VoC_endloop0

    VoC_shr32_ri(REG23,-4,DEFAULT);         //  inmagn = 16 * tmp;
    VoC_lw16i_short(REG7,0,IN_PARALLEL);            //  REG7 sign = 1;

    VoC_bnltz16_r(CII_smbPitchShift105,REG6)    // if( inimag < 0)
    VoC_sub16_rr(REG6,REG7,REG6,DEFAULT);       // inimag = -inimag;
    VoC_lw16i_short(REG7,1,DEFAULT);            // sign = -1;
CII_smbPitchShift105:
    VoC_sw32inc_p(REG23,REG1,DEFAULT);          //      ingAnaMagn[k] = inmagn;
    VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);            // REG5 flag = 0;

    VoC_bnltz16_r(CII_smbPitchShift106,REG4)    // if( inimag < 0)
    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);       //  inreal = - inreal;
    VoC_lw16i_short(REG5,1,DEFAULT);            //  flag = 1;
CII_smbPitchShift106:
    VoC_lw16i_short(REG0,0,DEFAULT);            // REG0 flag1 = 0;

    VoC_bngt16_rr(CII_smbPitchShift107,REG6,REG4)   //  if (inimag > inreal)
    VoC_movreg16(REG2,REG4,DEFAULT);            // tmp = inimag;
    VoC_movreg16(REG4,REG6,DEFAULT);            //  inimag = inreal;

    VoC_movreg16(REG6,REG2,DEFAULT);            // inreal = tmp;
    VoC_lw16i_short(REG0,1,IN_PARALLEL);            // flag1 = 1;

CII_smbPitchShift107:
    VoC_lw16i_set_inc(REG2,0,1);                    // REG2 for inphase = tmp;
    VoC_lw16i_set_inc(REG3,TAN_TABLE_ADDR,1);

    VoC_loop_i_short(33,DEFAULT)
    VoC_startloop0
    VoC_multf32inc_rp(ACC1,REG4,REG3,DEFAULT);  //  temp =(short) ((inreal * in_tantable[tmp])>>14);
    VoC_NOP();
    VoC_movreg16(REG1,ACC1_LO,DEFAULT);
    VoC_bngt16_rr(CII_smbPitchShift108,REG6,REG1)       // if( temp >= inimag)
    VoC_inc_p(REG2,DEFAULT);
    VoC_endloop0

CII_smbPitchShift108:
    VoC_lw16i(REG4,64);             // if(flag1)

    VoC_bez16_r(CII_smbPitchShift109,REG0)
    VoC_sub16_rr(REG2,REG4,REG2,DEFAULT);       //  inphase = 64 - inphase;
CII_smbPitchShift109:

    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);                // if(flag1)

    VoC_bez16_r(CII_smbPitchShift110,REG5)          //  if(flag)
    VoC_sub16_rr(REG2,REG4,REG2,DEFAULT);       //  inphase = 128 - inphase;
CII_smbPitchShift110:
    VoC_bez16_r(CII_smbPitchShift111,REG7)
    VoC_sub16_rr(REG2,REG6,REG2,DEFAULT);       //      inphase = sign * inphase;
CII_smbPitchShift111:
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_pop32(REG23,DEFAULT);
    VoC_pop32(REG67,DEFAULT);               // REG7 for k
    VoC_sub16_rp(REG5,REG4,REG2,DEFAULT);   //  REG5 temp = inphase - ingLastPhase[k];

    VoC_sw16inc_p(REG4,REG2,DEFAULT);       // ingLastPhase[k] = inphase;
    VoC_movreg16(REG6,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG6,-6,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG6,DEFAULT);   //  temp -= k * inexpct;
    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_shr16_ri(REG4,6,DEFAULT);           // qpd = temp/64;
    VoC_bnltz16_r(CII_smbPitchShift118,REG5)
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_and16_ri(REG6,0X003F);
    VoC_bez16_r(CII_smbPitchShift118,REG6)
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
CII_smbPitchShift118:
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_and16_ri(REG6,1);

    VoC_bez16_r(CII_smbPitchShift113,REG6)
    VoC_bltz16_r(CII_smbPitchShift112,REG4) // if (qpd >= 0)
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);   // qpd += qpd&1;
    VoC_jump(CII_smbPitchShift113);
CII_smbPitchShift112:
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);   // qpd -= qpd&1;
CII_smbPitchShift113:

    VoC_shr16_ri(REG4,-6,DEFAULT);          // qpd = temp*64;
    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);   //      temp -= qpd * 64;

    VoC_shr16_ri(REG5,-2,DEFAULT);          //  temp = osamp * temp;
    VoC_movreg16(REG6,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG6,-7,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);   //  temp = k * 128 + temp;
    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);       // ingAnaFreq[k] = temp;
    VoC_endloop1;


    VoC_lw16i_set_inc(REG0,ingSynMagn_ADDRESS,2);   // ingSynMagn[index]
    VoC_lw16i_set_inc(REG1,ingSynFreq_ADDR,1);  // ingSynFreq
    VoC_lw16i_set_inc(REG2,ingAnaMagn_ADDRESS,2);   // ingAnaMagn[k]
    VoC_lw16i_set_inc(REG3,ingAnaFreq_ADDRESS,1);   // ingAnaFreq[k]
    VoC_sw32_d(REG01,TEMP_ADDR_ADDRESS);
    VoC_lw32z(ACC0,DEFAULT);
//  memset(ingSynMagn, 0, fftFrameSize*sizeof(long));
//  memset(ingSynFreq, 0, fftFrameSize*sizeof(short));

    VoC_loop_i(1,129)
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop1

    VoC_lw16i_short(REG7,0,DEFAULT);                // REG7 for k
    VoC_lw16i(REG6,128);                // REG7 for k

    VoC_loop_i(1,129)               // for (k = 0; k <= fftFrameSize2; k++)
    VoC_multf32_rd(REG45,REG7,PITCH_SHIFT_ADDR);    //  index = (k*pitchShift)>>14;
    VoC_lw16inc_p(REG0,REG3,DEFAULT);       // reg0 for ingAnaFreq[k]

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);       // ACC0 for ingAnaMagn[k]

    VoC_bgt16_rr(CII_smbPitchShift114,REG4,REG6)
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_add16_rd(REG5,REG5,TEMP_ADDR_ADDRESS);
    VoC_multf32_rd(REG01,REG0,PITCH_SHIFT_ADDR);//  ingSynFreq[index] = (ingAnaFreq[k] * pitchShift)>>14;
    VoC_add16_rd(REG4,REG4,TEMP1_ADDR_ADDRESS);
    VoC_sw32_p(ACC0,REG5,DEFAULT);      //  ingSynMagn[index] = ingAnaMagn[k];
    VoC_sw16_p(REG0,REG4,DEFAULT);

CII_smbPitchShift114:
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);
    VoC_endloop1;

    VoC_lw16i_set_inc(REG0,ingSynMagn_ADDRESS,2);   // ingSynMagn[index]
    VoC_lw16i_set_inc(REG1,ingFFTworksp_ADDRESS,2); // ingFFTworksp[k]
    VoC_lw16i_set_inc(REG2,ingSynFreq_ADDR,1);  // ingSynFreq
    VoC_lw16i_set_inc(REG3,ingSumPhase_ADDR,1); // ingSumPhase[k]
    VoC_lw16i_short(REG7,0,DEFAULT);                // REG7 for k

    VoC_loop_i(1,129)               // for (k = 0; k <= fftFrameSize2; k++)
    /* subtract bin mid frequency */
    VoC_lw16inc_p(REG5,REG2,DEFAULT);   //  temp = ingSynFreq[k];
    VoC_movreg16(REG6,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG6,-7,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG6,DEFAULT);   //  temp -= k * 128;

    /* take osamp into account */
    VoC_shr16_ri(REG6,2,DEFAULT);       //  temp = temp / osamp;
    /* add the overlap phase advance back in */
    VoC_movreg16(REG5,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG5,-6,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);   //  temp += k * inexpct;
    VoC_add16_rp(REG6,REG6,REG3,DEFAULT);   //  ingSumPhase[k] += temp;
    VoC_and16_ri(REG6,0X00FF);              //  ingSumPhase[k] = ingSumPhase[k] & 0X00FF;

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_push16(REG6,DEFAULT);

    VoC_lw16i(REG4,128);
    VoC_bnltz16_r(CII_smbPitchShift115,REG6)    //  if (temp1 < 0)
    VoC_sub16_rr(REG6,REG5,REG6,DEFAULT);   //  temp1 = - temp1;
CII_smbPitchShift115:
    VoC_bngt16_rr(CII_smbPitchShift121,REG6,REG4)
    VoC_shr16_ri(REG4,-1,DEFAULT);      //if (temp1 > 128)
    VoC_sub16_rr(REG6,REG4,REG6,DEFAULT);   //  temp1 =256 - temp1;
CII_smbPitchShift121:
    VoC_lw16i(REG5,COS_TABLE_ADDR);

    VoC_add16_rr(REG4,REG5,REG6,DEFAULT);   //in_costable[temp1] ADDR
    VoC_lw32inc_p(REG23,REG0,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);          // save address
    VoC_shr32_ri(REG23,-2,IN_PARALLEL);

    VoC_shru16_ri(REG2,2,DEFAULT);
    VoC_multf32_rp(ACC0,REG3,REG4,DEFAULT);
    VoC_multf32_rp(ACC1,REG2,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);   //  ingFFTworksp[2*k] = (inmagn*in_costable[temp1])>>14;

    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,64,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG5,REG6,DEFAULT);   //  temp1 = 64 - temp1;
    VoC_sw32inc_p(ACC0,REG1,IN_PARALLEL);

    VoC_shr16_ri(REG5,-2,DEFAULT);
    VoC_bnltz16_r(CII_smbPitchShift116,REG6)    //  if (temp1 < 0)
    VoC_sub16_rr(REG6,REG4,REG6,DEFAULT);   //  temp1 = - temp1;
CII_smbPitchShift116:
    VoC_bngt16_rr(CII_smbPitchShift117,REG6,REG5)   //  if (temp1 > 256)
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);       //      temp1 = temp1 - 256;
CII_smbPitchShift117:
    VoC_lw16i(REG4,128);
    VoC_bngt16_rr(CII_smbPitchShift122,REG6,REG4)       //if (temp1 > 128)
    VoC_sub16_rr(REG6,REG5,REG6,DEFAULT);   //  temp1 =256 - temp1;
CII_smbPitchShift122:
    VoC_lw16i(REG5,COS_TABLE_ADDR);
    VoC_add16_rr(REG4,REG5,REG6,DEFAULT);   //in_costable[temp1] ADDR
    VoC_NOP();
    VoC_multf32_rp(ACC0,REG3,REG4,DEFAULT);
    VoC_multf32_rp(ACC1,REG2,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);   //  ingFFTworksp[2*k+1] = (inmagn*in_costable[temp1])>>14;
    VoC_endloop1;


    VoC_lw16i_set_inc(REG1,ingFFTworksp_ADDRESS,2); // ingFFTworksp[k]
    VoC_lw16i_set_inc(REG0,ingFFTworksp_ADDRESS,2); // ingFFTworksp[k]

    VoC_loop_i(1,86)                // for (k = 0; k <= fftFrameSize2; k++)
    VoC_lw32inc_p(REG23,REG1,DEFAULT);

    VoC_lw32inc_p(REG45,REG1,DEFAULT);
    VoC_shr32_ri(REG23,2,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG1,DEFAULT);
    VoC_shr32_ri(REG45,2,IN_PARALLEL);

    VoC_sw32inc_p(REG23,REG0,DEFAULT);
    VoC_shr32_ri(REG67,2,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop1;

    VoC_lw32z(REG45,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_loop_i(1,254)               // for (k = 0; k <= fftFrameSize2; k++)
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_endloop1;
    //  VoC_sw32inc_p(REG45,REG0,DEFAULT);

    VoC_sw16_d(REG1,sign_ADDRESS);

    VoC_jal(CS_intsmbFft);

    VoC_lw16i_set_inc(REG3,ingFFTworksp_ADDRESS,2); // ingFFTworksp[k]
    VoC_lw16i_set_inc(REG2,ingFFTworksp_ADDRESS,2); // ingFFTworksp[2*k]

    VoC_loop_i(1,2)             //  for (k = 0; k < fftFrameSize*2;k++)
    VoC_loop_i(0,128)
    VoC_lw32inc_p(REG45,REG3,DEFAULT);

    VoC_lw32inc_p(REG01,REG3,DEFAULT);
    VoC_shr32_ri(REG45,-2,IN_PARALLEL);

    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_shr32_ri(REG01,-2,IN_PARALLEL);

    VoC_lw32inc_p(ACC1,REG3,DEFAULT);
    VoC_shr32_ri(ACC0,-2,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    VoC_shr32_ri(ACC1,-2,IN_PARALLEL);

    VoC_sw32inc_p(REG01,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);

    VoC_endloop0
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_endloop1;


    VoC_lw16i_set_inc(REG0,ingOutputAccum_ADDR,2);  // ingOutputAccum[k]
    VoC_lw16i_set_inc(REG1,COS_TABLE_ADDR,1);
    VoC_lw16i_set_inc(REG2,ingFFTworksp_ADDRESS,4); // ingFFTworksp[2*k]
    VoC_lw16i(REG6,(COS_TABLE_ADDR+128));
    VoC_lw16i(REG7,0X800);

    VoC_loop_i(1,255)               //  for(k=0; k < fftFrameSize; k++)
    VoC_lw16inc_p(REG3,REG1,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_shr16_ri(REG3,3,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG7,REG3,DEFAULT);   //  in_windows = 0x800 - (in_costable[k]>>3);
    VoC_shr32_ri(REG45,-2,IN_PARALLEL);

    VoC_shru16_ri(REG4,2,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);
    VoC_bne16_rr(CII_smbPitchShift123,REG1,REG6)
    VoC_lw16i_short(INC1,-1,DEFAULT);
CII_smbPitchShift123:
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);   //  ingOutputAccum[k] += in_windows*ingFFTworksp[2*k] >>10;
    VoC_endloop1;

    VoC_lw16inc_p(REG3,REG1,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_shr16_ri(REG3,3,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG7,REG3,DEFAULT);   //  in_windows = 0x800 - (in_costable[k]>>3);
    VoC_shr32_ri(REG45,-2,IN_PARALLEL);

    VoC_shru16_ri(REG4,2,DEFAULT);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(ACC1,2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);
    VoC_bne16_rr(CII_smbPitchShift123_lastone,REG1,REG6)
    VoC_lw16i_short(INC1,-1,DEFAULT);
CII_smbPitchShift123_lastone:
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);   //  ingOutputAccum[k] += in_windows*ingFFTworksp[2*k] >>10;


    VoC_lw16i_set_inc(REG0,ingOutputAccum_ADDR,2);  // ingOutputAccum[k]
    VoC_lw16i_set_inc(REG1,gOutFIFO_ADDR,1);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_loop_i(1,64)
    VoC_shr32_ri(REG45,10,DEFAULT);     //  gOutFIFO[k] = (short)(ingOutputAccum[k] >>10);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop1;

//          memmove(ingOutputAccum, ingOutputAccum+stepSize, (fftFrameSize-stepSize)*sizeof(long));
    VoC_lw16i_set_inc(REG0,ingOutputAccum_ADDR,2);  // ingOutputAccum
    VoC_lw16i(REG7,128);    // ingOutputAccum+stepSize

    VoC_add16_rr(REG3,REG7,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_loop_i(1,96)                //  for (k = 0; k < fftFrameSize*2;k++)
    VoC_lw32inc_p(REG45,REG3,DEFAULT);
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop1;

//          memset(ingOutputAccum+(fftFrameSize-stepSize), 0, stepSize*sizeof(long));
    VoC_loop_i(1,64)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop1

    /* move input FIFO */
    VoC_lw16i_set_inc(REG0,gInFIFO_ADDR,2); // ingOutputAccum
    VoC_lw16i(REG7,64);
    VoC_add16_rr(REG3,REG7,REG0,DEFAULT);

    VoC_loop_i_short(48,DEFAULT)        //  for (k = 0; k < inFifoLatency; k++)
    VoC_startloop0
    VoC_lw32inc_p(REG45,REG3,DEFAULT);      //  gInFIFO[k] = gInFIFO[k+stepSize];
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_set_inc(REG2,gInFIFO_ADDR,2);         // gInFIFO[gRover]
    VoC_lw16i_set_inc(REG3,gOutFIFO_ADDR,2);    // gOutFIFO[gRover-inFifoLatency]
    VoC_lw16i(REG6,192);            // gRover = inFifoLatency;

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

CII_smbPitchShift103:

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);

    VoC_jump(CII_smbPitchShift102);

CII_smbPitchShift101:

    VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,2);
    VoC_lw16i_set_inc(REG0,FFT_OUTPUT_ADDRESS,2);

    VoC_lw16i(REG2,-3);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_loop_i(0,80)
    VoC_shr16_rr(REG4,REG2,DEFAULT);
    VoC_shr16_rr(REG5,REG2,IN_PARALLEL);


    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG6,gRover_ADDR);                   // REG6 for gRover
    VoC_return;
}
void CS_intsmbFft(void)
/*
    FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
    Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
    time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
    and returns the cosine and sine parts in an interleaved manner, ie.
    fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
    must be a power of 2. It expects a complex input signal (see footnote 2),
    ie. when working with 'common' audio signals our input signal has to be
    passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
    of the frequencies of interest is in fftBuffer[0...fftFrameSize].
*/

{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(FORMAT32,2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,2,2);        // REG2 for i
    VoC_lw16i_set_inc(REG3,510,2);
    VoC_lw16i(REG7,ingFFTworksp_ADDRESS);
CS_intsmbFft102:
    VoC_bngt16_rr(CS_intsmbFft101, REG3, REG2)  //for (i = 2; i < 2*fftFrameSize-2; i += 2)
    VoC_push32(REG23,DEFAULT);      // REG2 for i
    VoC_lw16i_short(REG0,2,DEFAULT);    // REG0 for bitm = 2
    VoC_lw16i_set_inc(REG1,0,1);    // REG1 for j = 0

    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_and16_rr(REG4,REG0,DEFAULT);
    VoC_bez16_r(CS_intsmbFft103,REG4)
    VoC_inc_p(REG1,DEFAULT);
CS_intsmbFft103:
    VoC_shr16_ri(REG1,-1,DEFAULT);      // j <<= 1;
    VoC_shr16_ri(REG0,-1,DEFAULT);      // bitm <<= 1
    VoC_endloop0

    VoC_bngt16_rr(CS_intsmbFft104,REG1,REG2)    //if (i < j)

    VoC_shr16_ri(REG1,-1,DEFAULT);      // j
    VoC_shr16_ri(REG2,-1,IN_PARALLEL);      // i

    VoC_add16_rr(REG2,REG7,REG2,DEFAULT);       // p1 = fftBuffer+i;
    VoC_add16_rr(REG3,REG7,REG1,DEFAULT);       // p2 = fftBuffer+j;
    VoC_lw32_p(RL6,REG2,DEFAULT);               //temp = *p1;
    VoC_lw32_p(RL7,REG3,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);            //*(p1++) = *p2;
    VoC_sw32inc_p(RL7,REG2,DEFAULT);            //*(p2++) = temp;
    VoC_lw32_p(RL6,REG2,DEFAULT);               //temp = *p1;
    VoC_lw32_p(RL7,REG3,DEFAULT);
    VoC_sw32_p(RL6,REG3,DEFAULT);           // *p1 = *p2;
    VoC_sw32_p(RL7,REG2,DEFAULT);           // *p2 = temp;
CS_intsmbFft104:
    VoC_pop32(REG23,DEFAULT);
    VoC_NOP();
    VoC_inc_p(REG2,DEFAULT);
    VoC_jump(CS_intsmbFft102);

CS_intsmbFft101:

    VoC_lw16i_short(REG5,2,DEFAULT);    // REG5 for le = 2

    VoC_loop_i(1,8)

    VoC_movreg16(REG4,REG5,DEFAULT);    // REG4 for le2 = le>>1;
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);

    VoC_lw16i(REG2,0X4000);             // ur = 0x4000;

    VoC_lw16i_short(REG3,0,DEFAULT);    // ui = 0;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);//REG6 for le2>>1

    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_sw32_d(REG45,TEMP_LE2_ADDRESS);     // save le2 and le

    VoC_lw16i_set_inc(REG0,128,2);  // REG0 for temp1

    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_bez16_r(CS_intsmbFft105,REG6)
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_endloop0
CS_intsmbFft105:
    VoC_lw16i(REG6,COS_TABLE_ADDR);
    VoC_add16_rr(REG1,REG6,REG0,DEFAULT);
    VoC_sw32_d(REG23,TEMP_UR_ADDRESS);          // SAVE ur, ui
    VoC_lw16_p(REG2,REG1,DEFAULT);          // REG2 for wr
    VoC_lw16i_set_inc(REG1,64,2);

    VoC_sub16_rr(REG1,REG1,REG0,DEFAULT);       // temp1 = 64 - temp1;
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bnltz16_r(CS_intsmbFft106,REG1)         // if(temp1 < 0)
    VoC_sub16_rr(REG1,REG5,REG1,DEFAULT);   // temp1 = -temp1;
CS_intsmbFft106:

    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG6,0,DEFAULT);        // REG6 for j in loop
    VoC_lw16_p(REG3,REG1,DEFAULT);      // REG3 for wi

    VoC_bnltz16_d(CS_intsmbFft107,sign_ADDRESS)
    VoC_sub16_rr(REG3,REG6,REG3,DEFAULT);
CS_intsmbFft107:
    VoC_NOP();
    VoC_sw32_d(REG23,WR_ADDRESS);           // SAVE wr, wi
CS_intsmbFft111:
    VoC_bngt16_rr(CS_intsmbFft108,REG4,REG6)    //for (j = 0; j < le2; j += 2)

    VoC_movreg16(REG5,REG6,DEFAULT);    // REG5 for i
    VoC_push32(REG67,IN_PARALLEL);      // save j and buffer addr

    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_shr16_ri(REG4,-1,IN_PARALLEL);

    VoC_add16_rr(REG0,REG7,REG6,DEFAULT);   // REG0 for p1r
    VoC_lw16i_short(FORMAT32,-12,IN_PARALLEL);

    VoC_add16_rr(REG1,REG0,REG4,DEFAULT);   // REG1 for p2r
    VoC_lw16i_short(FORMATX,2,IN_PARALLEL);

CS_intsmbFft110:
    VoC_lw16i(REG7,512);

    VoC_bngt16_rr(CS_intsmbFft109,REG7,REG5)

    VoC_lw32inc_p(REG23,REG1,DEFAULT);  //pr2r

    VoC_push16(REG5,DEFAULT);
    VoC_shr32_ri(REG23,-2,IN_PARALLEL);

    VoC_lw16_d(REG6,TEMP_UR_ADDRESS)

    VoC_multf32_rr(ACC1,REG3,REG6,DEFAULT);
    VoC_shru16_ri(REG2,2,IN_PARALLEL);

    VoC_lw32_p(REG45,REG1,DEFAULT);     //pr2i
    VoC_macX_rr(REG2,REG6,IN_PARALLEL);

    VoC_lw16_d(REG7,TEMP_UI_ADDRESS);

    //  VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_shr32_ri(REG45,-2,DEFAULT);

    VoC_shru16_ri(REG4,2,DEFAULT);
    VoC_msu32_rr(REG5,REG7,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG3,REG7,DEFAULT);
    VoC_msuX_rr(REG4,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG7,DEFAULT);
    VoC_lw32_p(RL6,REG0,IN_PARALLEL);   //pr1r

    VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_shr32_ri(ACC1,12,IN_PARALLEL);      // ACC0 for ti

    VoC_macX_rr(REG4,REG6,DEFAULT);
    VoC_add32_rr(RL7,ACC1,RL6,IN_PARALLEL); // RL7 for *p1r += tr;

    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_sub32_rr(RL6,RL6,ACC1,DEFAULT);// RL6 for *p2r = *p1r - tr;

    VoC_shr32_ri(ACC0,12,DEFAULT);      // ACC1 for tr
    VoC_sw32inc_p(RL7,REG0,IN_PARALLEL);        // *p1r += tr;

    VoC_lw32_p(RL7,REG0,DEFAULT);       //pr1i
    VoC_sub16_rr(REG1,REG1,REG2,IN_PARALLEL);

    VoC_add32_rr(ACC1,RL7,ACC0,DEFAULT);

    VoC_lw16_d(REG3,TEMP_LE_ADDRESS);   // REG3 for le

    VoC_pop16(REG5,DEFAULT);
    VoC_lw16i_short(REG2,2,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_shr16_ri(REG3,-1,IN_PARALLEL);

    VoC_sw32_p(ACC1,REG0,DEFAULT);      // *p1i += ti;
    VoC_sub32_rr(RL7,RL7,ACC0,IN_PARALLEL);

    VoC_sw32inc_p(RL6,REG1,DEFAULT);        //*p2r = *p1r - tr;
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);   // REG0 for p1r

    VoC_sw32_p(RL7,REG1,DEFAULT);       //*p2i = *p1i - ti;
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);   // REG1 for p2r

    VoC_sub16_rr(REG0,REG0,REG2,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG2,IN_PARALLEL);

    VoC_jump(CS_intsmbFft110);

CS_intsmbFft109:
    VoC_lw16i_short(FORMAT32,2,DEFAULT);

    VoC_lw32_d(REG23,WR_ADDRESS);           // LOAD wr, wi
    VoC_lw32_d(REG45,TEMP_UR_ADDRESS);          // load ur, ui

    VoC_multf32_rr(ACC0,REG2,REG4,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG4,IN_PARALLEL);

    VoC_msu32_rr(REG3,REG5,DEFAULT);
    VoC_mac32_rr(REG2,REG5,IN_PARALLEL);

    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);       // load j and buffer addr

    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_shr32_ri(ACC1,12,IN_PARALLEL);

    VoC_movreg16(ACC0_HI,ACC1_LO,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,IN_PARALLEL);

    VoC_lw16_d(REG4,TEMP_LE2_ADDRESS);      // load le2

    VoC_sw32_d(ACC0,TEMP_UR_ADDRESS);           // SAVE ur, ui
    VoC_jump(CS_intsmbFft111);
CS_intsmbFft108:
    VoC_lw16_d(REG5,TEMP_LE_ADDRESS);
    VoC_endloop1

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_MorphVoiceReset
 *
 * Description: Reset the AEC.
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       09/15/2010
 **************************************************************************************/
void Coolsand_MorphVoiceReset(void)
{
    VoC_bez16_d(HELLO_LABEL_MAIN_RESET,GLABAL_MORPH_RESET_ADDR)

    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG0,gInFIFO_ADDR,2);
    VoC_lw16i_set_inc(REG1,gOutFIFO_ADDR,2);
    VoC_lw16i_set_inc(REG2,ingOutputAccum_ADDR,2);
    VoC_lw16i_set_inc(REG3,ingFFTworksp_ADDRESS,2);
    VoC_sw16_d(ACC0_HI,GLABAL_MORPH_RESET_ADDR);

    VoC_loop_i(0,128)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG0,ingLastPhase_ADDR,1);
    VoC_loop_i(0,130)
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1,ingSumPhase_ADDR,1);
    VoC_loop_i(0,129)
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG5,0x00C0);
    VoC_NOP();
    VoC_sw16_d(REG5,gRover_ADDR);

HELLO_LABEL_MAIN_RESET:
    VoC_return;
}