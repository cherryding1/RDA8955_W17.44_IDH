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




#include "vppp_aec_asm_common.h"
#include "vppp_aec_asm_map.h"
#include "vppp_aec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
int g_counter=0;

extern FILE *SpkAnalysisProcessing_fileid, *MicAnalysisProcessing_fileid;
extern FILE *SynthesisProcessing_fileid, *AdaptiveFiltering_fileid;
extern FILE *UpdateSpkPower_fileid, *UpdateFilter_fileid;
extern FILE *FilterUpdateCotrol_Int_fileid;
extern FILE *ComputeNoiseWeight_DeciDirect_fileid;
extern FILE *PostFiltering_fileid;



int g_SpkAnaFilterOutBufI_test[SUBBAND_CHANNEL_NUM];        // Spk Analysis Filter Output Buffer
int g_SpkAnaFilterOutBufQ_test[SUBBAND_CHANNEL_NUM];        // Spk Analysis Filter Output Buffer
int g_MicAnaFilterOutBufI_test[SUBBAND_CHANNEL_NUM];        // Mic Analysis Filter Output Buffer
int g_MicAnaFilterOutBufQ_test[SUBBAND_CHANNEL_NUM];        // Mic Analysis Filter Output Buffer
int g_OutSynFilterInBufI_test[SUBBAND_CHANNEL_NUM];     // Out Synthesis Filter Input Buffer
int g_OutSynFilterInBufQ_test[SUBBAND_CHANNEL_NUM];     // Out Synthesis Filter Input Buffer





int g_AF_ErrDataBufI_test[SUBBANDFILTER_NUM];               // Err Data Buf for AF
int g_AF_ErrDataBufQ_test[SUBBANDFILTER_NUM];               // Err Data Buf for AF


int g_Buffer_test[2];       //for testing



#define DEBUG_STEP


int pOutData_test[8];       // Out Synthesis Filter Input Buffer

int i_xuml;

int g_OutPcmAdd;



// VoC_directive: PARSER_ON











/**************************************************************************************
 * Function:    Coolsand_AEC
 *
 * Description: Echo cancelation
 *
 * Inputs:      no
 *
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       08/13/2010
 **************************************************************************************/

void Coolsand_AEC(void)
{

    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG2,20,IN_PARALLEL);

    VoC_sw16_d(REG0,g_counter_subframe);


Coolsand_AEC_L0:

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_mult16_rd(REG7,REG7,g_counter_subframe);


    VoC_lw16i(REG0,GLOBAL_INPUT_SPK_PCM_ADDR);
    VoC_lw16i(REG1,GLOBAL_INPUT_MIC_PCM_ADDR);
    VoC_lw16i(REG2,GLOBAL_OUTPUT_MIC_PCM_ADDR);



    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

//  VoC_jal(Coolsand_PreProcessFiltering);


//      VoC_push32(REG01,DEFAULT);
//      VoC_push16(REG2,DEFAULT);

    //REG0:pSpk

    //  VoC_jal(Coolsand_NonLinearTransform);

//      VoC_pop32(REG01,DEFAULT);
//      VoC_pop16(REG2,IN_PARALLEL);






    //REG0:pSpk
    //REG1:pMic
    //REG2:IntBuf
    VoC_jal(Coolsand_EchoCancelProcessing);

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rd(REG1,REG1,g_counter_subframe)
    VoC_lw16i_short(REG2,20,DEFAULT);
    VoC_sw16_d(REG1,g_counter_subframe);

    VoC_bgt16_rd(Coolsand_AEC_L0,REG2,g_counter_subframe)

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_PreProcessFiltering
 *
 * Description: pre-filter.
 *
 * Inputs:   REG0: input addr
 *           REG2: output addr
 *
 * Outputs:
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       09/09/2010
 **************************************************************************************/
/*
void Coolsand_PreProcessFiltering(void)
{


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_loop_i(1,DEF_DATA_BLOCK_SIZE)

        VoC_lw16inc_p(REG6,REG0,DEFAULT);

        //VoC_jal(Coolsand_BackCopyVec);

        VoC_lw16i_set_inc(REG1,g_PE_PPFilterBuf+g_PE_PPFilterLen-1-1,-1);
        VoC_lw16i_set_inc(REG3,g_PE_PPFilterBuf+1+g_PE_PPFilterLen-1-1,-1);

        VoC_loop_i_short(g_PE_PPFilterLen-1,DEFAULT)
        VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);
        VoC_startloop0
            VoC_lw16inc_p(REG7,REG1,DEFAULT);
            VoC_sw16inc_p(REG7,REG3,DEFAULT);
        VoC_endloop0
            VoC_sw16_d(REG6,g_PE_PPFilterBuf);


        //  VoC_jal(Coolsand_MulVec);

        VoC_push16(REG2,DEFAULT);
        VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

        VoC_lw16i_set_inc(REG1,g_PE_PPFilterBuf,1);
        VoC_lw16i_set_inc(REG2,g_PE_PPFCoef_ADDR,1);
        VoC_lw16i_set_inc(REG3,g_PE_PPFilterMulBuf,2);

        VoC_loop_i_short(g_PE_PPFilterLen,DEFAULT)
        VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);
        VoC_startloop0
            VoC_multf32inc_rp(ACC0,REG6,REG2,DEFAULT);
            VoC_NOP();
            VoC_lw16inc_p(REG6,REG1,DEFAULT);
            VoC_sw32inc_p(ACC0,REG3,DEFAULT);
        VoC_endloop0

        //VecSum(g_PE_PPFilterMulBuf, MulData_FixFormat, &FilterOutFix, g_PE_PPFilterLen, false);
        VoC_lw16i_set_inc(REG3,g_PE_PPFilterMulBuf,2);

        VoC_loop_i_short(g_PE_PPFilterLen,DEFAULT)
        VoC_lw32z(ACC0,IN_PARALLEL);
        VoC_startloop0
            VoC_add32inc_rp(ACC0,ACC0,REG3,DEFAULT);
            VoC_shr32_ri(ACC0,-12,DEFAULT);
            VoC_shr32_ri(ACC0,12,DEFAULT);
        VoC_endloop0

        VoC_shr32_ri(ACC0,-12,DEFAULT);
        VoC_pop16(REG2,IN_PARALLEL);

        VoC_NOP();

        VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);

    VoC_endloop1

    VoC_return;
}


*/











/**************************************************************************************
 * Function:    Coolsand_EchoCancelProcessing
 *
 * Description: Echo cancel processing
 *
 * Inputs:
 *
 *   REG0:pSpk [In]
 *   REG1:pMic [In]
 *   REG2:Pout [Out]
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       08/05/2010
 * Version 2.0  Created by  Xuml       05/13/2011
 **************************************************************************************/

void Coolsand_EchoCancelProcessing(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG1,DEFAULT);

    VoC_lw16i(REG1,r_SpkAnaFilterDelayLine);
    VoC_lw16i(REG4,g_SpkAnaFilterOutBufI);
    VoC_lw16i(REG5,g_SpkAnaFilterOutBufQ);

    //REG0:pSpkData
    //REG1:r_SpkAnaFilterDelayLine
    //REG4:g_SpkAnaFilterOutBufI
    //REG5:g_SpkAnaFilterOutBufQ


    VoC_jal(Coolsand_AnalysisProcessing);

// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP


    fread(g_SpkAnaFilterOutBufI_test, 4, SUBBAND_CHANNEL_NUM, SpkAnalysisProcessing_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufI=(short*)&RAM_X[(g_SpkAnaFilterOutBufI)/2];
        if(g_SpkAnaFilterOutBufI_test[i_xuml]!=PoutbufI[i_xuml])
        {
            printf("g_SpkAnaFilterOutBufI error!ID:%d,:%d\n",i_xuml,g_SpkAnaFilterOutBufI_test[i_xuml]-PoutbufI[i_xuml]);
        }

    }
    fread(g_SpkAnaFilterOutBufQ_test, 4, SUBBAND_CHANNEL_NUM, SpkAnalysisProcessing_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufQ=(short *)&RAM_X[(g_SpkAnaFilterOutBufQ)/2];
        if(g_SpkAnaFilterOutBufQ_test[i_xuml]!=PoutbufQ[i_xuml])
        {
            printf("g_SpkAnaFilterOutBufQ error!ID:%d,:%d\n",i_xuml,g_SpkAnaFilterOutBufQ_test[i_xuml]-PoutbufQ[i_xuml]);
        }

    }

#endif

// VoC_directive: PARSER_ON


    VoC_pop16(REG0,DEFAULT);

    VoC_lw16i(REG1,r_MicAnaFilterDelayLine);
    VoC_lw16i(REG4,g_MicAnaFilterOutBufI);
    VoC_lw16i(REG5,g_MicAnaFilterOutBufQ);


    //REG0:pMicData
    //REG1:r_MicAnaFilterDelayLine
    //REG4:g_MicAnaFilterOutBufI
    //REG5:g_MicAnaFilterOutBufQ

    VoC_jal(Coolsand_AnalysisProcessing);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    fread(g_MicAnaFilterOutBufI_test, 4, SUBBAND_CHANNEL_NUM, MicAnalysisProcessing_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufI=(short*)&RAM_X[(g_MicAnaFilterOutBufI)/2];
        if(g_MicAnaFilterOutBufI_test[i_xuml]!=PoutbufI[i_xuml])
        {
            printf("g_MicAnaFilterOutBufI error!:%d\n",g_MicAnaFilterOutBufI_test[i_xuml]-PoutbufI[i_xuml]);
        }

    }
    fread(g_MicAnaFilterOutBufQ_test, 4, SUBBAND_CHANNEL_NUM, MicAnalysisProcessing_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufQ=(short *)&RAM_X[(g_MicAnaFilterOutBufQ)/2];
        if(g_MicAnaFilterOutBufQ_test[i_xuml]!=PoutbufQ[i_xuml])
        {
            printf("g_MicAnaFilterOutBufQ error!:%d\n",g_MicAnaFilterOutBufQ_test[i_xuml]-PoutbufQ[i_xuml]);
        }

    }
#endif
// VoC_directive: PARSER_ON


    VoC_lw16i_set_inc(REG0,r_AF_SpkSigBufI,1);
    VoC_lw16i_set_inc(REG1,r_AF_SpkSigBufQ,1);
    VoC_lw16i_set_inc(REG2,r_AF_FilterBufI,1);
    VoC_lw16i_set_inc(REG3,r_AF_FilterBufQ,1);
    VoC_sw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
    VoC_sw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);

    VoC_lw16i_set_inc(REG0,g_AF_ErrDataBufI,1);
    VoC_lw16i_set_inc(REG1,g_AF_ErrDataBufQ,1);
    VoC_lw16i_set_inc(REG2,g_AF_FilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_AF_FilterOutBufQ,1);
    VoC_sw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);
    VoC_sw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);

    VoC_lw16i_set_inc(REG0,g_MicAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG1,g_MicAnaFilterOutBufQ,1);
    VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);
    VoC_sw32_d(REG01,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
    VoC_lw16i_set_inc(REG0,r_AF_OldSpkDataBufI,1);
    VoC_lw16i_set_inc(REG1,r_AF_OldSpkDataBufQ,1);
    VoC_sw32_d(REG23,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

    VoC_lw16i_set_inc(REG2,r_AF_SpkSigPower,1);
    VoC_sw32_d(REG01,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);
    VoC_sw16_d(REG2,TEMP_r_AF_SpkSigPower_addr_addr);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG0,g_ch);


Coolsand_EchoCancelProcessing_L0:


    // Filter

    VoC_jal(Coolsand_AdaptiveFilteringAmp);





// VoC_directive: PARSER_OFF


#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();




    fread(g_AF_ErrDataBufI_test, 4, 1, AdaptiveFiltering_fileid);

    {
        short *PoutbufI=(short*)&RAM_X[(g_AF_ErrDataBufI)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_AF_ErrDataBufI error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufQ_test, 4, 1, AdaptiveFiltering_fileid);

    {
        short *PoutbufQ=(short *)&RAM_X[(g_AF_ErrDataBufQ)/2];
        if(g_AF_ErrDataBufQ_test[0]!=PoutbufQ[REGS[7].reg])
        {
            printf("g_AF_ErrDataBufQ error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufQ_test[0]-PoutbufQ[REGS[7].reg]);
        }

    }




    fread(g_MicAnaFilterOutBufI_test, 4, FILTER_LEN, AdaptiveFiltering_fileid);

    for(i_xuml=0; i_xuml<FILTER_LEN; i_xuml++)
    {
        short *PoutbufI=(short*)&RAM_X[(r_AF_SpkSigBufI+REGS[7].reg*FILTER_LEN)/2];
        if(g_MicAnaFilterOutBufI_test[i_xuml]!=PoutbufI[i_xuml])
        {
            printf("r_AF_SpkSigBufI error!CH:%d,:%d\n",REGS[7].reg,g_MicAnaFilterOutBufI_test[i_xuml]-PoutbufI[i_xuml]);
        }

    }
    fread(g_MicAnaFilterOutBufQ_test, 4, FILTER_LEN, AdaptiveFiltering_fileid);

    for(i_xuml=0; i_xuml<FILTER_LEN; i_xuml++)
    {
        short *PoutbufQ=(short *)&RAM_X[(r_AF_SpkSigBufQ+REGS[7].reg*FILTER_LEN)/2];
        if(g_MicAnaFilterOutBufQ_test[i_xuml]!=PoutbufQ[i_xuml])
        {
            printf("r_AF_SpkSigBufQ error!CH:%d,:%d\n",REGS[7].reg,g_MicAnaFilterOutBufQ_test[i_xuml]-PoutbufQ[i_xuml]);
        }

    }

#endif
// VoC_directive: PARSER_ON










    // Update Spk Power

    VoC_jal(Coolsand_UpdateSpkPower);






// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();


    fread(g_AF_ErrDataBufI_test, 4, 1, UpdateSpkPower_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_AF_SpkSigPower)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_AF_SpkSigPower error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
#endif
// VoC_directive: PARSER_ON








    // Filter Control

    VoC_lw16_d(REG7,g_ch);
    VoC_lw16i(REG2,g_SpkAnaFilterOutBufI);
    VoC_lw16i(REG3,g_SpkAnaFilterOutBufQ);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_lw16_d(REG6,m_AF_FrameCnt);

    VoC_lw16_p(REG2,REG2,DEFAULT);
    VoC_lw16_p(REG3,REG3,DEFAULT);


    //  REG2:  SubSpkDataI
    //  REG3:  SubSpkDataQ
    //  REG6: m_AF_FrameCnt
    //  REG7: ch

    VoC_jal(Coolsand_FilterUpdateCotrol_Int);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();


    fread(g_AF_ErrDataBufI_test, 4, 1, FilterUpdateCotrol_Int_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_AF_StepSize)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_AF_StepSize error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, FilterUpdateCotrol_Int_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_AF_SpkSigSmoothPower)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_AF_SpkSigSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
#endif

// VoC_directive: PARSER_ON




    // Update Filter

    VoC_lw16_d(REG7,g_ch);

    VoC_jal(Coolsand_UpdateFilterAmp);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    fread(g_MicAnaFilterOutBufI_test, 4, FILTER_LEN, UpdateFilter_fileid);

    for(i_xuml=0; i_xuml<FILTER_LEN; i_xuml++)
    {
        int *PoutbufI=(int*)&RAM_X[(r_AF_FilterBufI)/2+REGS[7].reg*FILTER_LEN];
        if(g_MicAnaFilterOutBufI_test[i_xuml]!=PoutbufI[i_xuml])
        {
            printf("r_AF_FilterBufI error!CH:%d,:%d\n",REGS[7].reg,g_MicAnaFilterOutBufI_test[i_xuml]-PoutbufI[i_xuml]);
        }

    }





    fread(g_MicAnaFilterOutBufQ_test, 4, FILTER_LEN, UpdateFilter_fileid);

    for(i_xuml=0; i_xuml<FILTER_LEN; i_xuml++)
    {
        int *PoutbufQ=(int *)&RAM_X[(r_AF_FilterBufQ)/2+REGS[7].reg*FILTER_LEN];
        if(g_MicAnaFilterOutBufQ_test[i_xuml]!=PoutbufQ[i_xuml])
        {
            printf("r_AF_FilterBufQ error!CH:%d,:%d\n",REGS[7].reg,g_MicAnaFilterOutBufQ_test[i_xuml]-PoutbufQ[i_xuml]);
        }

    }
#endif

// VoC_directive: PARSER_ON

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rd(REG1,REG1,g_ch);
    VoC_lw16i_short(REG2,9,DEFAULT);
    VoC_sw16_d(REG1,g_ch);

    VoC_bgt16_rr(Coolsand_EchoCancelProcessing_L0,REG2,REG1)




// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP


    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_NOP();

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_MicAnaFilterOutBufI)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_MicAnaFilterOutBufI error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_MicAnaFilterOutBufQ)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_MicAnaFilterOutBufQ error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_SpkAnaFilterOutBufI)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_SpkAnaFilterOutBufI error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_SpkAnaFilterOutBufQ)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_SpkAnaFilterOutBufQ error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
    VoC_lw16_d(REG6,m_AF_FrameCnt);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();
//          printf("m_AF_FrameCnt:%d\n",REGS[6].reg);

    if (REGS[6].reg==1591)
    {
        //      printf("ok");
    }

    printf("VoC g_counter:%d\n",g_counter);

    if (g_counter==104)
    {
        printf("ok");
    }

    g_counter++;

#endif

// VoC_directive: PARSER_ON


    //g_DTD_ch
    //CLChnl

    VoC_lw16i_short(REG0,0*2,DEFAULT);
    VoC_lw16i_short(REG1,1,DEFAULT);

    VoC_sw16_d(REG0,g_DTD_ch);
    VoC_sw16_d(REG1,CLChnl);

    VoC_lw16i_set_inc(REG2,g_MicAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_MicAnaFilterOutBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);

    VoC_movreg32(RL6,ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);

    VoC_movreg32(RL7,ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG2,g_AF_ErrDataBufI,1);
    VoC_lw16i_set_inc(REG3,g_AF_ErrDataBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);


    VoC_sw32_d(RL6,SumMicPower);
    VoC_sw32_d(RL7,SumSpkPower);
    VoC_sw32_d(ACC0,SumErrPower);

    VoC_jal(Coolsand_ControlLogicDTD);

    VoC_sw16_d(REG0,PFCLDTDOut);

    VoC_lw16i_short(REG0,1*2,DEFAULT);
    VoC_lw16i_short(REG1,3,DEFAULT);

    VoC_sw16_d(REG0,g_DTD_ch);
    VoC_sw16_d(REG1,CLChnl);

    VoC_lw16i_set_inc(REG2,g_MicAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_MicAnaFilterOutBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);

    VoC_add32_rd(RL6,ACC0,SumMicPower);

    VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
    VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);

    VoC_add32_rd(RL7,ACC0,SumSpkPower);

    VoC_lw16i_set_inc(REG2,g_AF_ErrDataBufI,1);
    VoC_lw16i_set_inc(REG3,g_AF_ErrDataBufQ,1);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_jal(Coolsand_SumInstantPower);

    VoC_add32_rd(ACC0,ACC0,SumErrPower);

    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_shr32_ri(RL7,1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,1,DEFAULT);

    VoC_push32(RL6,DEFAULT);

    VoC_push32(RL7,DEFAULT);

    VoC_push32(ACC0,DEFAULT);

    VoC_jal(Coolsand_ControlLogicDTD);

    VoC_sw16_d(REG0,PFCLDTDOut+1);

    VoC_lw16i_short(REG1,0,DEFAULT);

    VoC_bnez16_r(Coolsand_EchoCancelProcessing_L3,REG0)
    VoC_bltz32_d(Coolsand_EchoCancelProcessing_L3,r_PF_CL_DTDDecision)
    VoC_sw16_d(REG1,PFCLDTDOut);
Coolsand_EchoCancelProcessing_L3:

    VoC_pop32(ACC0,DEFAULT);
    VoC_pop32(RL7,DEFAULT);
    VoC_pop32(RL6,DEFAULT);



// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP


    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_lw16_d(REG6,m_AF_FrameCnt);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        if(g_Buffer_test[0]!=REGL[2])
        {
            printf("SumMicPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-REGL[2]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        if(g_AF_ErrDataBufI_test[0]!=REGL[3])
        {
            printf("SumSpkPower error!CH:%d, :%d\n",REGS[7].reg,REGL[3]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        if(g_AF_ErrDataBufI_test[0]!=REGL[0])
        {
            printf("SumSpkPower error!CH:%d, :%d\n",REGS[7].reg,REGL[3]);
        }
    }






    REGS[7].reg=0;

    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDXcorrMicErrI)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDXcorrMicErrI error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDXcorrMicErrQ)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDXcorrMicErrQ error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }


    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDXcorrMicEchoI)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDXcorrMicEchoI error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDXcorrMicEchoQ)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDXcorrMicEchoQ error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }


    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDMicSmoothPower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDMicSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDErrSmoothPower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDErrSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDEchoSmoothPower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDEchoSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }


    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDMicNoisePower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDMicNoisePower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDMicNoiseFloorDevCnt)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDMicNoiseFloorDevCnt error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTDDecision)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTDDecision error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_DTD2ndDecision)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_DTD2ndDecision error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }


#endif

// VoC_directive: PARSER_ON



    //RL6:SumMicPower
    //RL7:SumSpkPower
    //ACC0:SumErrPower

    VoC_movreg32(REG45,RL7,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_lw16_d(REG7,m_AECProcPara_PFCLChanlGain);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);

    VoC_macX_rr(REG5,REG7,DEFAULT);

    VoC_NOP();

    VoC_pop32(ACC0,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    VoC_shr32_ri(RL7,-8,DEFAULT);

    VoC_shr32_ri(RL7,8,DEFAULT);

    VoC_jal(Coolsand_ControlLogicMergeChnlEnhancedDTD);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_NOP();


    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_MicSmoothPower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_MicSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_MicNoise)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_MicNoise error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_MicNoiseFloorDevCnt)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_MicNoiseFloorDevCnt error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }



    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_SpkSmoothPower)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_SpkSmoothPower error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_CL_SpkNoise)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_CL_SpkNoise error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_SpkNoiseFloorDevCnt)/2];

        PoutbufI=PoutbufI;
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_SpkNoiseFloorDevCnt error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }



    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_ifMicSigOn)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_ifMicSigOn error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_ifSpkSigOn)/2];

        PoutbufI=PoutbufI;
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_ifSpkSigOn error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_ChnlState)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_ChnlState error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

#endif

// VoC_directive: PARSER_ON


    VoC_jal(Coolsand_ControlLogicSingleChnlEnhanced);



// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_NOP();

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_DTCountDown)/2];


        PoutbufI=PoutbufI+1;
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_DTCountDown error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_State)/2];
        PoutbufI=PoutbufI;
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_CL_State error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

#endif
// VoC_directive: PARSER_ON






    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG0,g_ch);


Coolsand_EchoCancelProcessing_L1:





// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP

    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    if (REGS[7].reg==1)
    {
        //      printf("ok");
    }

#endif
// VoC_directive: PARSER_ON


    VoC_lw16_d(REG7,g_ch);
    VoC_jal(Coolsand_ComputeNoiseWeight_DeciDirectCTRL);

// VoC_directive: PARSER_OFF



#ifdef DEBUG_STEP


    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    fread(g_Buffer_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *Poutbuf=(int*)&RAM_X[(r_PF_NoisePowerEstBuf)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("r_PF_NoisePowerEstBuf error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_SmoothErrPower)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_SmoothErrPower error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_PF_PostSToNRatio)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_PF_PostSToNRatio error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_PrevSigPowerNBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_PrevSigPowerNBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_PF_NoiseWeight)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_PF_NoiseWeight error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_NoiseFloorDevCnt)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_NoiseFloorDevCnt error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(r_PF_CL_State)/2];
        PoutbufI=PoutbufI;
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[0])
        {
            printf("r_PF_CL_State error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_EchoPowerEstBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_EchoPowerEstBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_AF_ErrDataBufI)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_AF_ErrDataBufI error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_AF_ErrDataBufQ)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_AF_ErrDataBufQ error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_NoisePowerSmoothEstBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_NoisePowerSmoothEstBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }


    if (g_counter==955)
    {
        //  printf("ok");
    }

#endif
// VoC_directive: PARSER_ON


    VoC_jal(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP



    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    {
        int i;

        for (i=0; i<FILTER_LEN; i++)
        {
            fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
            {
                int *PoutbufI=(int*)&RAM_X[(r_PF_CCErrSpkVecBufI)/2];

                PoutbufI=PoutbufI+REGS[7].reg*FILTER_LEN;
                if(g_AF_ErrDataBufI_test[0]!=PoutbufI[i])
                {
                    printf("r_PF_CCErrSpkVecBufI error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[i]);
                }
            }

            fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
            {
                int *PoutbufI=(int*)&RAM_X[(r_PF_CCErrSpkVecBufQ)/2];
                PoutbufI=PoutbufI+REGS[7].reg*FILTER_LEN;
                if(g_AF_ErrDataBufI_test[0]!=PoutbufI[i])
                {
                    printf("r_PF_CCErrSpkVecBufQ error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[i]);
                }
            }
        }
    }


    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_SpkPowerBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_SpkPowerBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_PF_NoiseWeight)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_PF_NoiseWeight error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_PF_PriorSToERatio)/2];

        PoutbufI=PoutbufI+1;

        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_PF_PriorSToERatio error!right:%d,error :%d\n",g_AF_ErrDataBufI_test[0],PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_PrevSigPowerEBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_PrevSigPowerEBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }


    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_EchoPowerEstBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_EchoPowerEstBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        short *PoutbufI=(short*)&RAM_X[(g_PF_EchoWeight)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("g_PF_EchoWeight error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }
    fread(g_AF_ErrDataBufI_test, 4, 1, ComputeNoiseWeight_DeciDirect_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_PrevEchoPowerEst)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_PrevEchoPowerEst error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }

#endif


// VoC_directive: PARSER_ON




    VoC_jal(Coolsand_PostFiltering);

// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP


    VoC_lw16_d(REG7,g_ch);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();







    fread(g_AF_ErrDataBufI_test, 4, 1, PostFiltering_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_PrevSigPowerNBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_PrevSigPowerNBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }


    fread(g_AF_ErrDataBufI_test, 4, 1, PostFiltering_fileid);
    {
        int *PoutbufI=(int*)&RAM_X[(r_PF_PrevSigPowerEBuf)/2];
        if(g_AF_ErrDataBufI_test[0]!=PoutbufI[REGS[7].reg])
        {
            printf("r_PF_PrevSigPowerEBuf error!CH:%d, :%d\n",REGS[7].reg,g_AF_ErrDataBufI_test[0]-PoutbufI[REGS[7].reg]);
        }
    }






    fread(g_Buffer_test, 4, 1, PostFiltering_fileid);
    {
        short *Poutbuf=(short *)&RAM_X[(g_OutSynFilterInBufI)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("g_OutSynFilterInBufI error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }


    fread(g_Buffer_test, 4, 1, PostFiltering_fileid);
    {
        short *Poutbuf=(short *)&RAM_X[(g_OutSynFilterInBufQ)/2];
        if(g_Buffer_test[0]!=Poutbuf[REGS[7].reg])
        {
            printf("g_OutSynFilterInBufQ error!CH:%d, :%d\n",REGS[7].reg,g_Buffer_test[0]-Poutbuf[REGS[7].reg]);
        }
    }

#endif

// VoC_directive: PARSER_ON



    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rd(REG1,REG1,g_ch);
    VoC_lw16i_short(REG2,9,DEFAULT);
    VoC_sw16_d(REG1,g_ch);

    VoC_bgt16_rr(Coolsand_EchoCancelProcessing_L1,REG2,REG1)


    // Output to Out Buf
    /*
            VoC_lw16_d(REG7,g_ch);

            VoC_lw16i(REG0,g_AF_ErrDataBufI);
            VoC_lw16i(REG1,g_AF_ErrDataBufQ);

            VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
            VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

            VoC_lw16i(REG2,g_OutSynFilterInBufI);
            VoC_lw16i(REG3,g_OutSynFilterInBufQ);

            VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
            VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

            VoC_lw16_p(REG4,REG0,DEFAULT);
            VoC_lw16_p(REG5,REG1,DEFAULT);
            VoC_sw16_p(REG4,REG2,DEFAULT);
            VoC_sw16_p(REG5,REG3,DEFAULT);  */


    //
    // Scale DoubleTalk Output
    //

    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_lw16_d(REG1,r_PF_CL_State);
    VoC_lw16_d(REG2,m_AECProcPara_StrongEchoFlag);

    VoC_bez16_r(Coolsand_EchoCancelProcessing_L2,REG2)
    VoC_bne16_rr(Coolsand_EchoCancelProcessing_L2,REG0,REG1)

    VoC_lw16i(REG4,g_AF_ErrDataBufI);
    VoC_lw16i(REG5,g_AF_ErrDataBufQ);

    VoC_lw16i(REG6,g_OutSynFilterInBufI);
    VoC_lw16i(REG7,g_OutSynFilterInBufQ);

    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);//g_OutSynFilterInBufI and g_OutSynFilterInBufQ
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0

    VoC_lw16_p(REG0,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG3,IN_PARALLEL);

    VoC_lw16_p(REG1,REG5,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);

    VoC_shr32_ri(RL7,-16,DEFAULT);
    VoC_lw16_p(REG0,REG6,IN_PARALLEL);

    VoC_shr32_ri(RL7,16,DEFAULT);
    VoC_lw16_p(REG1,REG7,IN_PARALLEL);

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_shr32_ri(RL6,-16,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);

    VoC_shr32_ri(RL6,16,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);

    VoC_endloop0

    VoC_lw16i_short(REG2,16,DEFAULT);

    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG3,REG7,DEFAULT);
    VoC_lw16i_short(REG5,3,IN_PARALLEL);

    VoC_shr16_rr(REG6,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT16,2-16,IN_PARALLEL);

    VoC_multf16_rr(REG6,REG6,REG5,DEFAULT);

    VoC_pop32(REG01,DEFAULT);//g_OutSynFilterInBufI and g_OutSynFilterInBufQ

    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);
    VoC_startloop0

    VoC_multf16_rp(REG2,REG6,REG0,DEFAULT);

    VoC_multf16_rp(REG3,REG6,REG1,DEFAULT);

    VoC_NOP();

    VoC_sw16inc_p(REG2,REG0,DEFAULT);

    VoC_sw16inc_p(REG3,REG1,DEFAULT);

    VoC_endloop0

Coolsand_EchoCancelProcessing_L2:


    // Conj to Symmetry Channel

    VoC_lw16i_set_inc(REG2,g_OutSynFilterInBufI+SUBBAND_CHANNEL_NUM-SUBBANDFILTER_NUM,-1);
    VoC_lw16i_set_inc(REG3,g_OutSynFilterInBufQ+SUBBAND_CHANNEL_NUM-SUBBANDFILTER_NUM,-1);

    VoC_lw16i_set_inc(REG0,g_OutSynFilterInBufI+SUBBANDFILTER_NUM,1);
    VoC_lw16i_set_inc(REG1,g_OutSynFilterInBufQ+SUBBANDFILTER_NUM,1);


    VoC_loop_i_short(7,DEFAULT)
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_startloop0

    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_sub16inc_rp(REG5,REG6,REG3,DEFAULT);

    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_endloop0

    VoC_pop16(REG3,DEFAULT);

    VoC_lw16i(REG0,g_OutSynFilterInBufI);
    VoC_lw16i(REG1,g_OutSynFilterInBufQ);
    VoC_lw16i(REG2,r_OutSynFilterDelayLine);


// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    g_OutPcmAdd=REGS[3].reg;


    fread(g_OutSynFilterInBufI_test, 4, SUBBAND_CHANNEL_NUM, AdaptiveFiltering_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufI=(short*)&RAM_X[(g_OutSynFilterInBufI)/2];

        if(PoutbufI[i_xuml]!=(short)g_OutSynFilterInBufI_test[i_xuml])
        {
            printf("g_OutSynFilterInBufI error!ID:%d value %d\n",i_xuml,PoutbufI[i_xuml]-(short)g_OutSynFilterInBufI_test[i_xuml]);
        }

        //  PoutbufI[i_xuml]=(short)g_OutSynFilterInBufI_test[i_xuml];
    }

    fread(g_OutSynFilterInBufQ_test, 4, SUBBAND_CHANNEL_NUM, AdaptiveFiltering_fileid);

    for(i_xuml=0; i_xuml<SUBBAND_CHANNEL_NUM; i_xuml++)
    {
        short *PoutbufQ=(short *)&RAM_X[(g_OutSynFilterInBufQ)/2];


        if(PoutbufQ[i_xuml]!=(short)g_OutSynFilterInBufQ_test[i_xuml])
        {
            printf("g_OutSynFilterInBufQ error!ID:%d value %d\n",i_xuml,PoutbufQ[i_xuml]-(short)g_OutSynFilterInBufQ_test[i_xuml]);
        }

        //  PoutbufQ[i_xuml]=(short)g_OutSynFilterInBufQ_test[i_xuml];
    }

#endif
// VoC_directive: PARSER_ON



    //REG0:g_OutSynFilterInBufI
    //REG1:g_OutSynFilterInBufQ
    //REG2:r_OutSynFilterDelayLine
    //REG3:pOutData

    VoC_jal(Coolsand_SynthesisProcessing);



// VoC_directive: PARSER_OFF

#ifdef DEBUG_STEP

    fread(pOutData_test, 4, 8, SynthesisProcessing_fileid);

    for(i_xuml=0; i_xuml<8; i_xuml++)
    {
        short *Poutbuf=(short*)&RAM_X[(g_OutPcmAdd)/2];
        if(pOutData_test[i_xuml]!=Poutbuf[i_xuml])
        {
            printf("pOutData_test error!:%d\n",pOutData_test[i_xuml]-Poutbuf[i_xuml]);
        }
        //  Poutbuf[i_xuml]=pOutData_test[i_xuml];
    }
#endif

// VoC_directive: PARSER_ON

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rd(REG1,REG1,m_AF_FrameCnt);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG1,m_AF_FrameCnt);
    VoC_return;
}






/**************************************************************************************
 * Function:    Coolsand_AnalysisProcessing
 *
 * Description: Analysis Filter Bank Operation
 *
 * Inputs:
 *
 *   REG0:(pInData) pointer to Input Data [In]
 *   REG1:(pTapppedDelayLine): DelayLine [In/Out]
 *
 *   REG4:(pOutDataI)pointer to Output Data [Out]
 *   REG5:(pOutDataQ)pointer to Output Data [Out]
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/

void Coolsand_AnalysisProcessing(void)
{

// VoC_directive: PARSER_OFF
    // printf("AnalysisProcessing......\n");
// VoC_directive: PARSER_ON

    VoC_push16(RA,DEFAULT);
    //pTapppedDelayLine+SUBSAMPLING_RATE
    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG6,SUBSAMPLING_RATE,IN_PARALLEL);

    VoC_add16_rr(REG2,REG1,REG6,DEFAULT);
    VoC_lw16i_short(REG7,PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE,IN_PARALLEL);



    //REG1: pTapppedDelayLine;
    //REG2: pTapppedDelayLine+SUBSAMPLING_RATE;
    //REG7: PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;

    VoC_jal(Coolsand_BackCopyVec);

    VoC_lw16i_short(REG7,SUBSAMPLING_RATE,DEFAULT);

    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);

    //REG1: pInData;
    //REG2: pTapppedDelayLine;
    //REG7: SUBSAMPLING_RATE;

    VoC_jal(Coolsand_FlipVec);

    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_lw16i_short(FORMAT16,-3,IN_PARALLEL);

    VoC_lw16i(REG2,AnaFilterCoef);
    VoC_lw16i(REG3,g_FilterBank_MulBuf);
    VoC_lw16i(REG0,PROTOTYPE_FILTER_LEN);

    //REG1: pTapppedDelayLine;
    //REG2: AnaFilterCoef;
    //REG3: g_FilterBank_MulBuf;
    //REG0: SUBSAMPLING_RATE;
    VoC_jal(Coolsand_MulVec);


    VoC_movreg16(REG1,REG4,DEFAULT);
    VoC_lw16i(REG7,SUBBAND_CHANNEL_NUM);

    //REG1: g_FilterBank_FoldSumBuf;
    //REG7: SUBBAND_CHANNEL_NUM;
    VoC_jal(Coolsand_ClearVec);


    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

    VoC_lw16i(REG1,g_FilterBank_MulBuf);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_loop_i(1,PROTOTYPE_FILTER_LEN/SUBBAND_CHANNEL_NUM)

    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);

    //REG0: pOutDataI;
    //REG1: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM;
    //REG2: pOutDataI;
    //REG7: SUBBAND_CHANNEL_NUM;

    VoC_jal(Coolsand_AddVec);

    VoC_endloop1


    VoC_movreg16(REG1,REG5,DEFAULT);
    VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

    //REG1: pOutDataQ;
    //REG7: SUBBAND_CHANNEL_NUM;
    VoC_jal(Coolsand_ClearVec);


    VoC_movreg16(REG1,REG5,DEFAULT);
    VoC_movreg16(REG0,REG4,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);


    //REG0:pOutDataI;
    //REG1:pOutDataQ;


    VoC_jal(Coolsand_IFFT16);


    //ScaleVec
    VoC_pop32(REG01,DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0

    VoC_lw32_p(REG45,REG0,DEFAULT);
    VoC_lw32_p(REG67,REG1,DEFAULT);

    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);

    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);

    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_SynthesisProcessing
 *
 * Description: Analysis Filter Bank Operation
 *
 * Inputs:
 *
 *   REG0: pInDataI: pointer to Input IData [In]
 *   REG1: pInDataQ: pointer to Input QData [In]
 *
 *   REG2: pTapppedDelayLine: DelayLine [In/Out]
 *   REG3: pOutData[out]
 *
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       07/28/2010
 **************************************************************************************/

void Coolsand_SynthesisProcessing(void)
{


// VoC_directive: PARSER_OFF
    // printf("SynthesisProcessing......\n");
// VoC_directive: PARSER_ON

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);

    //REG0:pInDataI
    //REG1:pInDataQ

    VoC_jal(Coolsand_IFFT16);

    VoC_pop32(REG01,DEFAULT);

    VoC_movreg32(RL7,REG01,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_movreg16(REG2,REG0,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    VoC_startloop0

    VoC_shr16_ri(REG4,-3,DEFAULT);
    VoC_shr16_ri(REG5,-3,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    VoC_endloop0

    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,g_FilterBank_MulBuf,2)


    VoC_loop_i(1,PROTOTYPE_FILTER_LEN/SUBBAND_CHANNEL_NUM)

    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

    //REG1: pInDataI;
    //REG2: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM;
    //REG7: SUBBAND_CHANNEL_NUM;
    VoC_jal(Coolsand_CopyVec);

    VoC_endloop1


    VoC_lw16i(REG1,g_FilterBank_MulBuf);
    VoC_lw16i(REG2,SynFilterCoef);

    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_lw16i_short(FORMAT16,-3,IN_PARALLEL);

    VoC_lw16i(REG0,PROTOTYPE_FILTER_LEN);

    //REG1: g_FilterBank_MulBuf;
    //REG2: SynFilterCoef;
    //REG3: g_FilterBank_MulBuf;
    //REG0: SUBSAMPLING_RATE;
    VoC_jal(Coolsand_MulVec);

    VoC_pop32(REG45,DEFAULT);//pTapppedDelayLine pOutData
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_movreg16(REG1,REG4,IN_PARALLEL);

    VoC_movreg16(REG2,REG4,DEFAULT);

    VoC_lw16i_set_inc(REG0,g_FilterBank_MulBuf,1);


    VoC_lw16i(REG7,PROTOTYPE_FILTER_LEN);

    //REG0: g_FilterBank_MulBuf;
    //REG1: pTapppedDelayLine;
    //REG2: pTapppedDelayLine;
    //REG7: PROTOTYPE_FILTER_LEN;
    VoC_jal(Coolsand_AddVec);

    VoC_movreg16(REG1,REG4,DEFAULT);
    VoC_movreg16(REG2,REG5,IN_PARALLEL);

    VoC_lw16i(REG7,SUBSAMPLING_RATE);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    //REG1: pTapppedDelayLine;
    //REG2: pOutData;
    //REG7: SUBSAMPLING_RATE;
    VoC_jal(Coolsand_CopyVec);

    VoC_movreg16(REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG3,SUBSAMPLING_RATE,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_lw16i(REG7,PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE);

    //REG1: pTapppedDelayLine+SUBSAMPLING_RATE;
    //REG2: pTapppedDelayLine;
    //REG7: PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;
    VoC_jal(Coolsand_CopyVec);

    VoC_movreg16(REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG3,PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

    //REG1: pTapppedDelayLine+PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;
    //REG7: SUBSAMPLING_RATE;
    VoC_jal(Coolsand_ClearVec);


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_AdaptiveFiltering
 *
 * Description: Filtering Operation
 *
 * Inputs:
 *   pMicData: pointer to Mic Data [In]
 *   pSpkData: pointer to Spk Data [In]
 *   pErrData: pointer to Output Data [Out]
 *   pSpkSigBuf: pointer to spk sig buffer [In/Out]
 *   pFilterBuf: pointer to filter [In]
 *   pFilterOut: filter output [Out]
 *   pOldSpkData: old spk data [Out]
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * NOTE:no
 *
 * Version 1.0  Created by  Guosz       07/29/2010
 **************************************************************************************/

void Coolsand_AdaptiveFilteringAmp(void)
{

    // Add new SpkData to Spk Sig Buffer

    //pOldSpkDataI->d = pSpkSigBufI[FILTER_LEN-1];
    //pOldSpkDataQ->d = pSpkSigBufQ[FILTER_LEN-1];

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,FILTER_LEN-1,IN_PARALLEL);

    VoC_lw32_d(REG23,TEMP_r_AF_SpkSigBufIQ_addr_addr);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_lw32_d(REG45,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

    VoC_movreg32(REG01,REG23,DEFAULT);

    VoC_lw16_p(REG6,REG2,DEFAULT);
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);

    VoC_lw16_p(REG7,REG3,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_sw16_p(REG6,REG4,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_sw16_p(REG7,REG5,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_lw32_d(REG67,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

//  BackCopyVec(pSpkSigBufI, pSpkSigBufI+1, FILTER_LEN-1);
//  BackCopyVec(pSpkSigBufQ, pSpkSigBufQ+1, FILTER_LEN-1);

    VoC_loop_i_short(FILTER_LEN-1,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    //pSpkSigBufI[0] = pSpkDataI->d;
    //pSpkSigBufQ[0] = pSpkDataQ->d;

    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_lw16_p(REG7,REG7,DEFAULT);

    VoC_sw16_p(REG6,REG0,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_sw16_p(REG7,REG1,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);


    // Filter
    VoC_lw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);
//  TruncateVec(pFilterBufI, Filter_FixFormat, g_AF_ShortFilterBufI, ShortFilter_FixFormat, FILTER_LEN);
//  TruncateVec(pFilterBufQ, Filter_FixFormat, g_AF_ShortFilterBufQ, ShortFilter_FixFormat, FILTER_LEN);

//  ScaleVec( g_AF_ShortFilterBufQ,  g_AF_ShortFilterBufQ, -1, FILTER_LEN);
    VoC_lw16i_set_inc(REG0,g_AF_ShortFilterBufI,1);
    VoC_lw16i_set_inc(REG1,g_AF_ShortFilterBufQ,1);

    VoC_loop_i_short(FILTER_LEN,DEFAULT);
    VoC_lw32z(REG45,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);

    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(REG67,-12,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG5,REG7,DEFAULT);

    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_sw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);

    /*  CmplxMulVec(    pSpkSigBufI, pSpkSigBufQ, SpkData_FixFormat,
                        g_AF_ShortFilterBufI, g_AF_ShortFilterBufQ, ShortFilter_FixFormat,
                        g_AF_tempCmplxBufI, g_AF_tempCmplxBufQ, FilterMul_FixFormat,
                        FILTER_LEN,
                        ifFixRoundOff,
                        g_AF_tempBuf);
    */
//CmplxMulVec(int * pSrc0I, int * pSrc0Q, FixFormat Src0Format, int * pSrc1I, int * pSrc1Q, FixFormat Src1Format,
    //int * pDstI, int *pDstQ, FixFormat DstFormat, int Len, bool ifRoundOff, int *tempBuf)
//  MulVec( pSrc0I, Src0Format, pSrc1I, Src1Format, pDstI, MulFormat, Len, ifRoundOff);

//  ScaleVec( pSrc0Q,  pSrc0Q, -1, Len);
//  MulVec( pSrc0Q, Src0Format, pSrc1Q, Src1Format, tempBuf, MulFormat, Len, ifRoundOff);
//  AddVec(pDstI, MulFormat, tempBuf, MulFormat, pDstI, DstFormat, Len, ifRoundOff);
//  ScaleVec( pSrc0Q,  pSrc0Q, -1, Len);

    VoC_lw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
    VoC_lw16i_set_inc(REG2,g_AF_ShortFilterBufI,2);
    VoC_lw16i_set_inc(REG3,g_AF_ShortFilterBufQ,2);

    VoC_lw16i(RL7_LO,g_AF_tempCmplxBufI);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_loop_i_short(FILTER_LEN/2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_sub16inc_rp(REG4,REG4,REG1,DEFAULT);
    VoC_sub16inc_rp(REG5,REG4,REG1,IN_PARALLEL);

    VoC_mac32_rr(REG6,REG4,DEFAULT);
    VoC_mac32_rr(REG7,REG5,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG3,DEFAULT);

    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);

    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

    VoC_endloop0

//  MulVec( pSrc0I, Src0Format, pSrc1Q, Src1Format, pDstQ, MulFormat, Len, ifRoundOff);
//  MulVec( pSrc0Q, Src0Format, pSrc1I, Src1Format, tempBuf, MulFormat, Len, ifRoundOff);
//  AddVec(pDstQ, MulFormat, tempBuf, MulFormat, pDstQ, DstFormat, Len, ifRoundOff);

    VoC_lw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
    VoC_lw16i_set_inc(REG2,g_AF_ShortFilterBufQ,2);
    VoC_lw16i_set_inc(REG3,g_AF_ShortFilterBufI,2);

    VoC_lw16i(RL7_LO,g_AF_tempCmplxBufQ);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_loop_i_short(FILTER_LEN/2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG3,DEFAULT);

    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);

    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

    VoC_endloop0

    VoC_sw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);

//  VecSum(g_AF_tempCmplxBufI, FilterMul_FixFormat, pFilterOutI, FILTER_LEN, ifFixRoundOff);
//  VecSum(g_AF_tempCmplxBufQ, FilterMul_FixFormat, pFilterOutQ, FILTER_LEN, ifFixRoundOff);
    VoC_lw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);
    VoC_lw16i_set_inc(REG0,g_AF_tempCmplxBufI,2);
    VoC_lw16i_set_inc(REG1,g_AF_tempCmplxBufQ,2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_loop_i_short(FILTER_LEN,DEFAULT);
    VoC_startloop0
    VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);
    VoC_shr32_ri(ACC1,11,IN_PARALLEL);

    VoC_add32inc_rp(ACC1,ACC1,REG1,DEFAULT);
    VoC_shr32_ri(ACC0,-11,IN_PARALLEL);

    VoC_shr32_ri(ACC0,11,DEFAULT);
    VoC_shr32_ri(ACC1,-11,IN_PARALLEL);

    VoC_endloop0


    VoC_lw16i_short(FORMAT16,5-16,DEFAULT);

    VoC_lw16i(REG0,r_AF_FilterAmpI);

    VoC_add16_rd(REG0,REG0,g_ch);

    VoC_shr32_ri(ACC0,-12,DEFAULT);
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
    VoC_movreg16(REG7,ACC1_HI,IN_PARALLEL);

    VoC_multf16_rp(REG6,REG6,REG0,DEFAULT);
    VoC_multf16_rp(REG7,REG7,REG0,IN_PARALLEL);

    VoC_lw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);   // pFilterOutI

    VoC_sw16inc_p(REG7,REG3,DEFAULT);   // pFilterOutQ


//  tempFixI = (*pMicDataI) -(*pFilterOutI);
//  pErrDataI->d = tempFixI.d;
//  tempFixQ = (*pMicDataQ) -(*pFilterOutQ);
//  pErrDataQ->d = tempFixQ.d;

    VoC_lw32_d(REG23,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
    VoC_sw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);

    VoC_sub16inc_pr(REG4,REG2,REG6,DEFAULT);
    VoC_sub16inc_pr(REG5,REG3,REG7,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_sw32_d(REG23,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
    VoC_pop16(RA,DEFAULT);
    VoC_sw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_UpdateSpkPower
 *
 * Description: Update Spk Power Estimation
 *
 * Inputs:
 *   pSpkSigBuf: pointer to spk sig buffer [In]
 *   pOldSpkData: old spk data [In]
 *   pSpkSigPower: spk power estimation [In/Out]
 *
 * Outputs: no.
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Guosz       07/29/2010
 **************************************************************************************/
void Coolsand_UpdateSpkPower(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    //UpdateSpkPower(   &SubSpkDataI, &SubSpkDataQ,
    //  r_AF_OldSpkDataBufI+ch, r_AF_OldSpkDataBufQ+ch, r_AF_SpkSigPower+ch);
//  CmplxCutNorm(pSpkDataI, pSpkDataQ, &NewSpkDataNorm);
    VoC_lw32_d(REG01,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16i_short(FORMAT32,6,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_multf32_rr(ACC0,REG4,REG4,IN_PARALLEL);
    VoC_multf32_rr(ACC1,REG5,REG5,DEFAULT);
    VoC_lw32_d(REG23,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);       // ACC0 for NewSpkDataNorm
//  CmplxCutNorm(pOldSpkDataI, pOldSpkDataQ, &OldSpkDataNorm);
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_shr32_ri(ACC0,-6,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_multf32_rr(RL6,REG4,REG4,IN_PARALLEL);

    VoC_multf32_rr(RL7,REG5,REG5,DEFAULT);
    VoC_shr32_ri(ACC0,6,IN_PARALLEL);

    VoC_sw32_d(REG01,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);          // rl6 for OldSpkDataNorm

    VoC_lw16d_set_inc(REG2,TEMP_r_AF_SpkSigPower_addr_addr,2);
    VoC_sw32_d(REG23,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

    VoC_shr32_ri(RL6,-6,DEFAULT);
    VoC_lw32_p(ACC1,REG2,IN_PARALLEL);


    VoC_shr32_ri(RL6,6,DEFAULT);
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);
//  TempSpkPowerFix = NewSpkDataNorm-OldSpkDataNorm;

    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);

//  TempSpkPowerFix = TempSpkPowerFix+(*pSpkSigPower);
//  Truncate(TempSpkPowerFix, pSpkSigPower->t, pSpkSigPower->f);


    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);


    VoC_shr32_ri(ACC0,1,DEFAULT);

    VoC_shr32_ri(ACC0,-2,DEFAULT);
    VoC_shr32_ri(ACC0,2,DEFAULT);

//  if (pSpkSigPower->d<0)
    VoC_bnltz32_r(Coolsand_UpdateSpkPower100,ACC0)
//      pSpkSigPower->d = 0;
    VoC_lw32z(ACC0,DEFAULT);

Coolsand_UpdateSpkPower100:
    //  pSpkSigPower->d = TempSpkPowerFix.d;
    VoC_pop16(RA,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw16_d(REG2,TEMP_r_AF_SpkSigPower_addr_addr);

    VoC_return;


}



/**************************************************************************************
 * Function:    Coolsand_FilterUpdateCotrol_Int
 *
 * Description: no
 *
 * Inputs:
 *   REG2: SubSpkDataI  mean pSpkDataI
 *   REG3: SubSpkDataQ  mean pSpkDataQ
 *               data format: Q15
 *   REG6: m_AF_FrameCnt
 *   REG7: ch
 *
 * Outputs:
 *   REG3:
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 1.1  optimized by  Cuilf     08/14/2010
 * Version 1.2  fixed by  Xuml     08/26/2010
 **************************************************************************************/

void Coolsand_FilterUpdateCotrol_Int(void)
{

    //
    // Err Data and Spk Data
    //

    //Input Data format:    pSpkDataI&pSpkDataQ , pErrDataI, pErrDataQ  are Q15
    //                      r_AF_RErrSpkI_addr  ,r_AF_RErrSpkQ_addr     are Q23
    VoC_push16(RA,DEFAULT);


    VoC_lw16i(REG0,g_AF_ErrDataBufI);
    VoC_lw16i(REG1,g_AF_ErrDataBufQ);

    VoC_movreg16(REG6,REG7,DEFAULT); //ch
    VoC_push16(REG6,IN_PARALLEL);//save m_AF_FrameCnt

    VoC_add32_rr(REG01,REG01,REG67,DEFAULT); //ErrDataI = pErrDataI->d;  //ErrDataQ = pErrDataQ->d;

    // Conj Err Data

    //
    // Compute Coupling Factor
    //
    // Update RErrSpk


    VoC_lw16i_short(FORMAT32,0,DEFAULT);//InstantCorrI = InstantCorrI>>(ErrFracBitNum+SpkFracBitNum-RErrSpkFracBitNum);
    VoC_multf32_rp(ACC0, REG2,REG0,DEFAULT); // InstantCorrI = ConjErrDataI*SpkDataI-ConjErrDataQ*SpkDataQ;
    VoC_multf32_rp(ACC1,REG3,REG0,IN_PARALLEL);//InstantCorrQ = ConjErrDataI*SpkDataQ+ConjErrDataQ*SpkDataI;

    VoC_mac32_rp(REG3,REG1,DEFAULT);    //  ConjErrDataQ = -pErrDataQ->d;
    VoC_msu32_rp(REG2,REG1,IN_PARALLEL);//  ConjErrDataQ = -pErrDataQ->d;   InstantCorrQ = InstantCorrQ>>(ErrFracBitNum+SpkFracBitNum-RErrSpkFracBitNum);

    VoC_NOP();
    VoC_shr32_ri(ACC0,7,DEFAULT);
    VoC_shr32_ri(ACC1,7,IN_PARALLEL);
    //After multf, the data format is Q23
    VoC_push16(REG7,DEFAULT);//save ch
    VoC_lw16i_short(REG5,0x0008,IN_PARALLEL);

    VoC_movreg32(RL7,ACC0,DEFAULT);//RL7 :InstCorrI_Abs;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrI_abs,ACC0)
    VoC_lw32z(RL7,DEFAULT);
    VoC_sub32_rr(RL7,RL7,ACC0,DEFAULT);//RL7 :InstCorrI_Abs;
Coolsand_FilterUpdateCotrol_Int_InstantCorrI_abs:

    VoC_bngt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCorrI,RL7,REG45)
    VoC_movreg32(ACC0,REG45,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);
    VoC_bgtz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrI,RL7)
    VoC_lw16i_short(ACC0_HI,0xfff8,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0x0,IN_PARALLEL);
Coolsand_FilterUpdateCotrol_Int_InstantCorrI:
    //ACC0  InstantCorrI

    VoC_movreg32(RL6,ACC1,DEFAULT);//RL6 :InstCorrQ_Abs;
    VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ_abs,ACC1)
    VoC_lw32z(RL6,DEFAULT);
    VoC_sub32_rr(RL6,RL6,ACC1,DEFAULT);//RL6 :InstCorrQ_Abs;
Coolsand_FilterUpdateCotrol_Int_InstantCorrQ_abs:

    VoC_bngt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ,RL6,REG45)
    VoC_movreg32(ACC1,REG45,DEFAULT);
    VoC_movreg32(RL6,ACC1,IN_PARALLEL);
    VoC_bgtz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ,RL6)
    VoC_lw16i_short(ACC1_HI,0xfff8,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0x0,IN_PARALLEL);
Coolsand_FilterUpdateCotrol_Int_InstantCorrQ:
    //ACC1  InstantCorrQ

    VoC_add16_rr(REG6,REG7,REG7,DEFAULT);

    VoC_lw16i(REG4,r_AF_RErrSpkI);
    VoC_lw16i(REG5,r_AF_RErrSpkQ);


    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);
    VoC_lw16i_short(FORMAT32,-16,IN_PARALLEL);

    VoC_lw32_p(RL7,REG4,DEFAULT);
    VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);

    VoC_lw32_p(RL6,REG5,DEFAULT);
//RL6 : pRErrSpkQ->d  == LastCorrQ
//RL7: pRErrSpkI->d  == LastCorrI
//ACC0  InstantCorrI    ACC1  InstantCorrQ
//Those all are Q23

    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_movreg32(REG23,ACC1,IN_PARALLEL);

    VoC_lw32z(REG67,DEFAULT);

    VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_L0,REG01)
    VoC_sub32_rr(REG01,REG67,REG01,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_L0:

    VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_L1,REG23)
    VoC_sub32_rr(REG23,REG67,REG23,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_L1:

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_L2,REG01,REG67)
    VoC_bngt32_rr(Coolsand_FilterUpdateCotrol_Int_L3,REG23,REG67)//        if ( InstCorrI_Abs >1 || InstCorrQ_Abs >1 )
Coolsand_FilterUpdateCotrol_Int_L2:

    VoC_lw16_d(REG6,m_AECProcPara_AFUpdateFactor);

    VoC_mult16_rd(REG6,REG6,m_AECProcPara_AFCtrlSpeedUpFactor);

    VoC_sub32_rr(REG23,ACC0,RL7,DEFAULT);//    NewValueI= InstantCorrI - pRErrSpkI->d;

    VoC_sub32_rr(REG01,ACC1,RL6,DEFAULT);    //    NewValueQ= InstantCorrQ - pRErrSpkQ->d;
    VoC_multf32_rr(ACC0,REG3,REG6,IN_PARALLEL);//AFUpdateFactor is Q15, Q15xQ23 >>8 is Q23

    //acc1 : NewValueI
    VoC_shru16_ri(REG2,1,DEFAULT); //    NewValueI= (NewValueI*(UpdateFactor.d)) >>UpdateFactor.f;
    VoC_multf32_rr(ACC1,REG1,REG6,IN_PARALLEL);//AFUpdateFactor is Q15, Q15xQ15 >>8 is Q23

    VoC_macX_rr(REG2,REG6,DEFAULT);
    VoC_shru16_ri(REG0,1,DEFAULT); // Q23->Q15 in reg3   NewValueQ= (NewValueQ*(UpdateFactor.d)) >>UpdateFactor.f;

    VoC_shr32_ri(ACC0,15,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//pRErrSpkI->d = pRErrSpkI->d +NewValueI;
    VoC_macX_rr(REG0,REG6,IN_PARALLEL);

    VoC_NOP();

    VoC_shr32_ri(ACC1,15,DEFAULT);

    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT); //    pRErrSpkQ->d = pRErrSpkQ->d +NewValueQ;

    VoC_sw32_p(ACC0,REG4,DEFAULT);

    VoC_sw32_p(ACC1,REG5,DEFAULT);


Coolsand_FilterUpdateCotrol_Int_L3:


    VoC_lw16_d(REG6,m_AECProcPara_AFUpdateFactor);

    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);

    VoC_pop32(REG23,DEFAULT);
    VoC_push32(REG45,DEFAULT);
//RL6 : pRErrSpkQ->d  == LastCorrQ
//RL7: pRErrSpkI->d  == LastCorrI

//Those all are Q23


    // Update Spk Sig Smooth Power
    VoC_lw16i(REG4,r_AF_SpkSigSmoothPower);

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);//   int tempMul0 = (SpkDataI*SpkDataI)>>(SpkFracBitNum*2-SpkSmoothPowFracBitNum);

    VoC_multf32_rr(ACC0,REG2,REG2,DEFAULT);//int tempMul1 = (SpkDataQ*SpkDataQ)>>(SpkFracBitNum*2-SpkSmoothPowFracBitNum);
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);

    VoC_mac32_rr(REG3,REG3,DEFAULT);  //     SpkDataPower = tempMul0+tempMul1;
    VoC_lw32_p(ACC1,REG4,DEFAULT);
    //ACC1:  LastSpkSmoothPower = pSpkSigSmoothPower->d;

    VoC_bez32_r(Coolsand_FilterUpdateCotrol_Int_SpkDataPower,ACC0)//   if (SpkDataPower!=0)
    VoC_lw16i_short(FORMAT32,-16,DEFAULT);
    VoC_sub32_rr(REG23,ACC0,ACC1,DEFAULT);//NewValue = SpkDataPower - pSpkSigSmoothPower->d;

    VoC_multf32_rr(ACC0,REG3,REG6,DEFAULT);//NewValue = (NewValue*(UpdateFactor.d)) >>UpdateFactor.f;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);   // Q23->Q15 in reg3

    VoC_macX_rr(REG2,REG6,DEFAULT);
    VoC_NOP();
    VoC_shr32_ri(ACC0,15,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);//pSpkSigSmoothPower->d = pSpkSigSmoothPower->d+NewValue;
    VoC_NOP();
    VoC_sw32_p(ACC0,REG4,DEFAULT);

Coolsand_FilterUpdateCotrol_Int_SpkDataPower:
    VoC_pop32(REG45,DEFAULT);
    VoC_lw32_p(ACC0,REG4,DEFAULT);
    VoC_push32(ACC1,DEFAULT);//push LastSpkSmoothPower
    VoC_push32(ACC0,DEFAULT);//push pSpkSigSmoothPower->d
    VoC_lw32_p(ACC0,REG4,DEFAULT);// ACC0: pRErrSpkI->d
    VoC_lw32_p(ACC1,REG5,DEFAULT);//ACC1 : pRErrSpkQ->d

    // Compute RErrSpk Norm

    VoC_jal(Coolsand_CmplxAppxNorm_int); //     CmplxAppxNorm( (int *) &(pRErrSpkI->d), (int *)&(pRErrSpkQ->d), &RErrSpkNorm );
    VoC_shr32_ri(ACC0,8,DEFAULT);  //     RErrSpkNorm = RErrSpkNorm >> (RErrSpkFracBitNum-CFFracBitNum);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);

    VoC_pop32(RL6,DEFAULT);//pSpkSigSmoothPower->d
    VoC_pop32(REG23,DEFAULT);//LastSpkSmoothPower

    VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm,ACC0)

// RL7:  pRErrSpkI->d  == LastCorrI
//RL6 : pRErrSpkQ->d  == LastCorrQ
    VoC_movreg32(ACC0,RL7,DEFAULT);

    VoC_jal(Coolsand_CmplxAppxNorm_int);// CmplxAppxNorm( &LastCorrI, &LastCorrQ, &RErrSpkNorm );
    VoC_shr32_ri(ACC0,8,DEFAULT);  //    RErrSpkNorm = RErrSpkNorm >> (RErrSpkFracBitNum-CFFracBitNum);
    VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm_100,ACC0)
    VoC_lw16i_short(ACC0_LO,2,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm_100:
    VoC_movreg32(RL6,REG23,DEFAULT); //LastSpkSmoothPower   SpkSigSmoothPower_r = LastSpkSmoothPower*FilterFactor;

Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm:

    VoC_movreg32(REG23,RL6,DEFAULT);
    VoC_shr32_ri(RL6,-1,DEFAULT);      //FilterFactor =3

    VoC_add32_rr(RL6,RL6,REG23,DEFAULT);// SpkSigSmoothPower_r = pSpkSigSmoothPower->d*FilterFactor;
    VoC_shr32_ri(RL6,12,DEFAULT);//  SpkSigSmoothPower_r = SpkSigSmoothPower_r>> (SpkSmoothPowFracBitNum-SpkSthPowerFracBitNum);

    // Coupling factor
    VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_SpkSigSmoothPower_r,RL6)//  if (SpkSigSmoothPower_r == 0)
    VoC_lw16i_short(RL6_LO,1,DEFAULT);                       //    SpkSigSmoothPower_r = 1;
Coolsand_FilterUpdateCotrol_Int_SpkSigSmoothPower_r:

    VoC_lw16i_short(REG2,12,DEFAULT);//xuml
    VoC_jal(Coolsand_MSBPos);  //  int SpkSmoothPowerFirstBitPos = MSBPos(SpkSigSmoothPower_r, SpkSthPowerFracBitNum);
    //REG3: SpkSmoothPowerFirstBitPos
    VoC_lw16i_short(REG2,12,DEFAULT);
    VoC_sub16_rr(REG2,REG3,REG2,DEFAULT);//  -(SpkSthPowerFracBitNum+1-SpkSmoothPowerFirstBitPos)
    VoC_shr32_rr(ACC0,REG2,DEFAULT);//    CouplingFactor = RErrSpkNorm << (SpkSthPowerFracBitNum+1-SpkSmoothPowerFirstBitPos);
    //        VoC_lw16i_short(REG5,0,IN_PARALLEL);
    //    VoC_lw16i(REG4,0x8000);

    //   VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_CouplingFactor,REG45,ACC0)  //    if (CouplingFactor >= (0x1<<CFFracBitNum) )
    //     VoC_lw32z(ACC0,DEFAULT);
    //     VoC_lw16i(ACC0_LO,0x7fff);                             //   CouplingFactor = (0x1<<CFFracBitNum)-1;
//Coolsand_FilterUpdateCotrol_Int_CouplingFactor:

    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_lw16_p(ACC1_HI,REG1,DEFAULT);//ErrDataQ = pErrDataQ->d;
    VoC_push32(ACC0,DEFAULT);//CouplingFactor

    //
    // Instant Coupling Factor
    //

    // Err Norm

    VoC_lw16_p(ACC0_HI,REG0,DEFAULT);//ErrDataI = pErrDataI->d;

    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_shr32_ri(ACC1,16,IN_PARALLEL);

    VoC_jal(Coolsand_CmplxAppxNorm_int);   //   CmplxAppxNorm(&ErrDataI, &ErrDataQ, &ErrDataNorm);
    // ACC0 :  ErrDataNorm

    // Sqrt Spk Sig Power


    VoC_lw16i(REG0,r_AF_SpkSigPower);//r_AF_SpkSigPower_addr : Q3.23
    VoC_add16_rr(REG6,REG7,REG7,DEFAULT);  //REG7:ch
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    VoC_lw16i(REG4,0x800);
    VoC_lw32_p(RL6,REG0,DEFAULT);

    VoC_shr32_ri(RL6,12,DEFAULT);//    SpkSigPower_s = pSpkSigPower->d >> (pSpkSigPower->f - SpkPowShortFracBitNum);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s,REG45,RL6)// if (SpkSigPower_s>= (0x1<<(SpkPowShortTotalBitNum-1)))
    VoC_lw16i(RL6_LO,0x7ff);                                               //   SpkSigPower_s = (0x1<<(SpkPowShortTotalBitNum-1))-1;
    VoC_lw16i_short(RL6_HI,0,DEFAULT);

Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s:
    VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s_100,RL6)  //if (SpkSigPower_s == 0)
    VoC_lw16i_short(RL6_LO,1,DEFAULT);                       //     SpkSigPower_s = 1;
Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s_100:


    VoC_lw16i_short(REG2,12,DEFAULT);//xuml
    VoC_jal(Coolsand_MSBPos); // int SpkPowerFirstBitPos = MSBPos( SpkSigPower_s,SpkPowShortTotalBitNum-1);

    // Div
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_sub16_rr(REG4,REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_and16_rr(REG4,REG6,DEFAULT);


    VoC_bez16_r(Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos,REG4)// if ((SpkPowerFirstBitPos-1-SpkPowShortFracBitNum)%2 != 0)
    VoC_lw16i_short(REG4,11,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT); //    SpkPowerShiftValue = -((SpkPowerFirstBitPos-SpkPowShortFracBitNum)/2+1);
    //REG4 : SpkPowerShiftValue

    VoC_shr32_rr(ACC0,REG4,DEFAULT);

    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG23,DEFAULT);// InstantCouplingFactor = InstantCouplingFactor*3;


    VoC_jump(Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos_end)


Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos:

    VoC_sub16_rr(REG4,REG3,REG7,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);   // SpkPowerShiftValue = -(SpkPowerFirstBitPos-1-SpkPowShortFracBitNum)/2;
    VoC_shr32_rr(ACC0,REG4,DEFAULT);//InstantCouplingFactor = ErrDataNorm << SpkPowerShiftValue;

Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos_end:
    VoC_lw16i_short(ACC1_LO,0,DEFAULT);
    VoC_lw16i(ACC1_HI,0x0010);

    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCouplingFactor,ACC1,ACC0)// if (InstantCouplingFactor>= (0x1<<(InstantCFTotalBitNum-1)) )

    VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0xf,DEFAULT);//0x000fffff                                      //    InstantCouplingFactor = (0x1<<(InstantCFTotalBitNum-1))-1;

Coolsand_FilterUpdateCotrol_Int_InstantCouplingFactor:

    VoC_pop16(REG7,DEFAULT);//restore ch


    VoC_lw16i(REG0,g_AF_StepSize);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);//fStepSize = 1.0;
    VoC_pop16(REG6,IN_PARALLEL);//restore m_AF_FrameCnt


    VoC_pop32(ACC1,DEFAULT);//CouplingFactor
    VoC_lw16i_short(REG2,8,IN_PARALLEL);

    VoC_lw16i(REG5,0x7f);//   *pVarStepSize = Double2Fix(fStepSize, pVarStepSize->t, pVarStepSize->f);


    VoC_bgt16_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end,REG2,REG6) //   if (FrameCnt<FILTER_LEN)
    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end, ACC1,ACC0)// if (CouplingFactor> ( InstantCouplingFactor<<(CFFracBitNum-InstantCFFracBitNum) ))

    //input : RL6, REG2
    //output: REG3
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG2,21,DEFAULT);//xuml
    VoC_jal(Coolsand_MSBPos);                    // int InstantCFFirstBitPos = MSBPos(InstantCouplingFactor, InstantCFTotalBitNum-1);
    VoC_lw16i_short(REG2,16,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG2,DEFAULT);//int StepShiftVal = InstantCFFracBitNum+1-InstantCFFirstBitPos;
    VoC_shr32_rr(ACC1,REG3,DEFAULT);  //    StepSize_Int = CouplingFactor >> (-StepShiftVal);

    VoC_shr32_ri(ACC1,8,DEFAULT); //    StepSize_Int = StepSize_Int >> (CFFracBitNum - pVarStepSize->f);
    VoC_movreg16(REG5,ACC1_LO,DEFAULT);
    VoC_lw16i(REG4,0x80);
    VoC_bgt16_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end,REG4,REG5)// if (pVarStepSize->d >= (0x1<<pVarStepSize->f) )
    VoC_lw16i(REG5,0x7f);   //  pVarStepSize->d = (0x1<<pVarStepSize->f)-1;
Coolsand_FilterUpdateCotrol_Int_FrameCnt_end:
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG5,REG0,DEFAULT);
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_UpdateFilter
 *
 * Description: Update Adaptive Filter
 *
 * Inputs:
 *   REG7: ch
 *
 * Outputs: no
 *
 *
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 1.1  optimized by  Cuilf     08/14/2010
 **************************************************************************************/
void Coolsand_UpdateFilterAmp(void)
{

    //Input : REG7  ch

    //
    // Compute Total Step Size
    //
    VoC_push16(RA,DEFAULT);


    VoC_lw16i(REG0,r_AF_SpkSigPower);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);//r_AF_SpkSigPower+ch
    VoC_lw16_d(ACC0_HI,m_AECProcPara_InitRegularFactor);
    VoC_add32_rp(REG45,ACC0,REG0,DEFAULT);  //  Fix tempSpkPower = *pSpkSigPower+RegularFactor;

    VoC_shr16_ri(REG5,-2,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_shr16_ri(REG5,2,DEFAULT);//     Truncate(tempSpkPower, SpkSigPower_r.t, SpkSigPower_r.f);
    VoC_lw16i_short(REG2,14,IN_PARALLEL);

    //REG5 : SpkSigPower_r.d

    VoC_bgtz16_r(Coolsand_ShrinkBit_abs,REG5)
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_sub16_rr(REG5,REG3,REG5,DEFAULT);
Coolsand_ShrinkBit_abs:


    VoC_movreg16(RL6_LO,REG5,DEFAULT);

    VoC_jal(Coolsand_MSBPos);//xuml
    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_bez16_r(Coolsand_ShrinkBit_end,REG3)

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_sub16_rr(REG3,REG4,REG3,DEFAULT);
    VoC_shr16_rr(REG4,REG3,DEFAULT);
    VoC_and16_rr(REG4,REG5,DEFAULT);   //   SpkSigPower_r.d = ShrinkBit(SpkSigPower_r.d, SpkSigPower_r.t, 1);
Coolsand_ShrinkBit_end:

    //input : RL6, REG2
    //output: REG3
    VoC_movreg16(RL6_LO,REG4,DEFAULT);
    //   VoC_lw16i_short(REG2,7,DEFAULT);
    VoC_jal(Coolsand_MSBPos);              //int SpkPowerFirstBitPos = MSBPos( SpkSigPower_r.d,SpkSigPower_r.t-1);

    VoC_lw16i(REG1,g_AF_StepSize);
    VoC_lw16i(REG2,g_AF_ErrDataBufI);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16i_short(REG4,7,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);// int StepSizeShiftVal = (SpkSigPower_r.f  - SpkPowerFirstBitPos+1-pVarStepSize->f);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);    //*pErrDataI

    VoC_lw16_p(REG1,REG1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);

    VoC_shr16_rr(REG1,REG3,DEFAULT);  //    TotalStepSize.d = pVarStepSize->d <<StepSizeShiftVal ;
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i(REG3,g_AF_ErrDataBufQ);

    //
    // Compute Update Item
    //

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_shr16_ri(REG1,-2,IN_PARALLEL);

    VoC_shr16_ri(REG1,2,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_multf32_rp(REG45,REG1,REG2,DEFAULT);//tempFixI = TotalStepSize*(*pErrDataI);
    VoC_shr16_ri(REG7,-3,IN_PARALLEL);

    VoC_msu32_rp(REG1,REG3,DEFAULT);// tempFixQ = TotalStepSize*(CongjErrDataQ);
    VoC_movreg16(REG6,REG7,IN_PARALLEL);

    VoC_lw16i(REG0,r_AF_FilterAmpI);

    VoC_add16_rd(REG0,REG0,g_ch);

    VoC_shr32_ri(REG45,10,DEFAULT);//Truncate(tempFixI,15+1+4, 15);
    VoC_shr32_ri(ACC0,10,IN_PARALLEL);//Truncate(tempFixQ,15+1+4, 15);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_movreg32(REG23,ACC0,IN_PARALLEL);//tempFixQ

    VoC_shr16_ri(REG0,5,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L1,REG0,REG1)
    VoC_lw16i(REG1,0x4000);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L1:

    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L2,REG0,REG1)
    VoC_lw16i(REG1,0x2000);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L2:

    VoC_lw16i_short(REG1,3,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L3,REG0,REG1)
    VoC_lw16i(REG1,0x1555);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L3:

    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L4,REG0,REG1)
    VoC_lw16i(REG1,0x1000);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L4:

    VoC_lw16i_short(REG1,5,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L5,REG0,REG1)
    VoC_lw16i(REG1,0xccc);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L5:

    VoC_lw16i_short(REG1,6,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L6,REG0,REG1)
    VoC_lw16i(REG1,0xaaa);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L6:

    VoC_lw16i_short(REG1,7,DEFAULT);
    VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L7,REG0,REG1)
    VoC_lw16i(REG1,0x924);
    VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L7:
    VoC_lw16i(REG1,0x800);
Coolsand_UpdateFilterAmp_End:

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i_short(FORMATX,14,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG3,REG1,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG1,IN_PARALLEL);

    VoC_macX_rr(REG2,REG1,DEFAULT);
    VoC_macX_rr(REG4,REG1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,r_AF_SpkSigBufI,1);
    VoC_lw16i_set_inc(REG1,r_AF_SpkSigBufQ,1);

    VoC_movreg32(REG45,ACC1,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

    /*
    FilterUpdateI.f = Filter_FixFormat.f;
    FilterUpdateI.t = FilterUpdateI.f+1;
    FilterUpdateQ.t = FilterUpdateI.t;
    FilterUpdateQ.f = FilterUpdateI.f;

    for(i=0; i<FILTER_LEN; i++)
    {
    SpkSigI.d = pSpkSigBufI[i];
    SpkSigQ.d = pSpkSigBufQ[i];

    CmplxMul(&SpkSigI, &SpkSigQ, &tempFixI, &tempFixQ, &FilterUpdateI, &FilterUpdateQ);
    g_AF_UpdateItemI[i] = FilterUpdateI.d;
    g_AF_UpdateItemQ[i] = FilterUpdateQ.d;
    }
    */


    VoC_lw16i_set_inc(REG2,r_AF_FilterBufI,2);
    VoC_lw16i_set_inc(REG3,r_AF_FilterBufQ,2);

    VoC_add32_rr(REG01,REG01,REG67,DEFAULT);
    VoC_add32_rr(REG23,REG23,REG67,IN_PARALLEL);

    VoC_add32_rr(REG23,REG23,REG67,DEFAULT);
    VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL);

    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw16i_short(FORMATX,11,IN_PARALLEL);


    //REG45 :tempFixI
    //RL7 : tempFixQ


    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);


    VoC_loop_i_short(8,DEFAULT) //  for(i=0; i<FILTER_LEN; i++)
    VoC_startloop0

    VoC_multf32_rp(ACC0,REG5,REG0,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG0,IN_PARALLEL);

    VoC_macX_rp(REG4,REG0,DEFAULT);
    VoC_macXinc_rp(REG6,REG0,IN_PARALLEL);

    VoC_msu32_rp(REG7,REG1,DEFAULT);
    VoC_mac32_rp(REG5,REG1,IN_PARALLEL);

    VoC_msuX_rp(REG6,REG1,DEFAULT);
    VoC_macXinc_rp(REG4,REG1,IN_PARALLEL);

    VoC_NOP();

    VoC_shr32_ri(ACC0,-12,DEFAULT);//   Truncate(tempFixI, pDstI->t, pDstI->f);
    VoC_shr32_ri(ACC1,-12,IN_PARALLEL); //  Truncate(tempFixQ, pDstI->t, pDstI->f);

    VoC_shr32_ri(ACC0,12,DEFAULT);//    Truncate(tempFixI, pDstI->t, pDstI->f);
    VoC_shr32_ri(ACC1,12,IN_PARALLEL); //   Truncate(tempFixQ, pDstI->t, pDstI->f);


    VoC_add32_rp(RL6,ACC0,REG2,DEFAULT); //     AddVec(pFilterBufI, (int*)g_AF_UpdateItemI, pFilterBufI, FILTER_LEN);
    VoC_add32_rp(RL7,ACC1,REG3,DEFAULT);  //    AddVec(pFilterBufQ, (int*)g_AF_UpdateItemQ, pFilterBufQ, FILTER_LEN);

    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);

    VoC_endloop0

    //
    // Filter Update
    //


    VoC_return

}




/**************************************************************************************
 * Function:    Coolsand_BackCopyVec
 *
 * Description: Copy Src to Dst from the last index to zero
 *
 * Inputs:
 *      REG1: (pSrc)pointer to Source Data [In]
 *      REG2: (pDst)pointer to Destination Data [Out]
 *      REG7: (Len)Data Length [In].shorts
 *
 * Outputs:     no
 *
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 *
 * NOTE:the len must be even and shorts.
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_BackCopyVec(void)
{
    VoC_lw16i_short(INC1,-2,DEFAULT);
    VoC_lw16i_short(INC2,-2,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG7,1,DEFAULT);

    VoC_inc_p(REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_lw32inc_p(ACC0,REG1,DEFAULT);

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_FlipVec
 *
 * Description: Flip Src to Dst
 *
 * Inputs:
 *      REG1: (pSrc)pointer to Source Data [In]
 *      REG2: (pDst)pointer to Destination Data [Out]
 *      REG7: (Len)Data Length [In].shorts
 *
 * Outputs:     no
 *
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 *
 * NOTE:
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_FlipVec(void)
{
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_MulVec
 *
 * Description: the multiplication of vectors. //Mul Filter Coef.
 *
 * Inputs:
 *      REG1: pTapppedDelayLine [in]
 *      REG2: AnaFilterCoef [in]
 *      REG3: g_FilterBank_MulBuf [out]
 *      REG0: filter Length [In]
 *
 * Outputs:     no
 *
 * Used : REG0,REG1,REG2,REG3,REG6,REG7,INC1,INC2,INC3
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_MulVec(void)
{

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_shr16_ri(REG0,1,IN_PARALLEL);

    VoC_loop_r_short(REG0,DEFAULT)
    VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_multf16inc_rp(REG6,REG6,REG2,DEFAULT);
    VoC_multf16inc_rp(REG7,REG7,REG2,DEFAULT);
    VoC_NOP();
    VoC_lw32inc_p(REG67,REG1,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_ClearVec
 *
 * Description: Clear vector.
 *
 * Inputs:
 *
 *      REG1: pBuf: pointer to Data [In/Out]
 *      REG7:  Len: Data Length [In]
 *
 * Outputs:     no
 *
 * Used : REG1,REG7,ACC0,INC1
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/24/2010
 **************************************************************************************/
void Coolsand_ClearVec(void)
{
    VoC_lw16i_short(INC1,2,DEFAULT);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_AddVec
 *
 * Description: the additation of vectors.
 *
 * Inputs:
 *      REG0: g_FilterBank_FoldSumBuf
 *      REG1: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM
 *      REG2: g_FilterBank_FoldSumBuf
 *      REG7: filter Length [In]
 *
 * Outputs:     no
 *
 * Used : REG0,REG1,REG2,REG6,REG7,INC0,INC1,INC2
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/24/2010
 **************************************************************************************/
void Coolsand_AddVec(void)
{

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_FFT16
 *
 * Description:  FFT N=16.
 *
 * Inputs:
 *      REG0: real
 *      REG1: imag
 *
 *
 * Outputs:     no
 *
 * Used : all
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/27/2010
 **************************************************************************************/
void Coolsand_IFFT16(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    //reverse real
    VoC_jal(Coolsand_ReverseIndex);

    VoC_pop32(REG23,DEFAULT);

    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);
    //reverse imag
    VoC_jal(Coolsand_ReverseIndex);

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,real_int,2);
    VoC_lw16i_set_inc(REG3,imag_int,2);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16i(REG6,0x7fff);

    VoC_loop_i_short(8,DEFAULT)
    VoC_push32(REG01,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//real[u],real[l]

    VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u],imag[l]
    VoC_multf32_rr(ACC0,REG5,REG6,IN_PARALLEL);

    VoC_movreg16(REG7,RL7_HI,DEFAULT);
    VoC_movreg16(RL7_HI,RL7_LO,IN_PARALLEL);

    VoC_multf32_rr(RL6,REG7,REG6,DEFAULT);
    VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL);

    VoC_shr32_ri(ACC1,16,DEFAULT);
    VoC_shr32_ri(RL7,16,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);//real[l]=real[u]-XkWnreal;
    VoC_add32_rr(ACC0,ACC1,ACC0,IN_PARALLEL);//real[u]=real[u]+XkWnreal;

    VoC_sub32_rr(RL7,RL7,RL6,DEFAULT);//imag[l]=imag[u]-XkWnimag;
    VoC_add32_rr(RL6,RL7,RL6,IN_PARALLEL);//imag[u]=imag[u]+XkWnimag;

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);

    VoC_endloop0


    VoC_lw16i_set_inc(REG0,real_int,4);
    VoC_lw16i_set_inc(REG1,imag_int,4);

    VoC_loop_i_short(4,DEFAULT)
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);
    VoC_startloop0

    VoC_push32(REG01,DEFAULT);

    VoC_lw32inc_p(RL6,REG0,DEFAULT);//real[u]
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);

    VoC_lw32_p(REG45,REG0,DEFAULT); //real[l]
    VoC_shru16_ri(REG2,1,IN_PARALLEL); //for getting 0x7fff

    VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u]
    VoC_shr32_ri(REG45,-1,IN_PARALLEL);

    VoC_lw32_p(REG67,REG1,DEFAULT); //imag[l]
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC1,REG6,REG2,DEFAULT);

    VoC_pop32(REG01,DEFAULT);
    VoC_macX_rr(REG7,REG2,IN_PARALLEL);

    VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
    VoC_add32_rr(RL6,RL6,ACC0,IN_PARALLEL);

    VoC_sub32_rr(ACC1,RL7,ACC1,DEFAULT);
    VoC_add32_rr(RL7,RL7,ACC1,IN_PARALLEL);

    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);

    VoC_endloop0


    VoC_lw16i_set_inc(REG0,real_int+2,4);
    VoC_lw16i_set_inc(REG1,imag_int+2,4);

    VoC_loop_i_short(4,DEFAULT)
    VoC_lw16i_short(FORMATX,0,IN_PARALLEL);
    VoC_startloop0

    VoC_push32(REG01,DEFAULT);

    VoC_lw32inc_p(RL6,REG0,DEFAULT);//real[u]
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);

    VoC_lw32_p(REG45,REG0,DEFAULT); //real[l]
    VoC_shru16_ri(REG2,1,IN_PARALLEL); //for getting 0x7fff

    VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u]
    VoC_shr32_ri(REG45,-1,IN_PARALLEL);

    VoC_lw32_p(REG67,REG1,DEFAULT); //imag[l]
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_macX_rr(REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC1,REG6,REG2,DEFAULT);

    VoC_pop32(REG01,DEFAULT);
    VoC_macX_rr(REG7,REG2,IN_PARALLEL);

    VoC_sub32_rr(ACC0,RL7,ACC0,DEFAULT);
    VoC_add32_rr(RL7,RL7,ACC0,IN_PARALLEL);

    VoC_add32_rr(ACC1,RL6,ACC1,DEFAULT);
    VoC_sub32_rr(RL6,RL6,ACC1,IN_PARALLEL);

    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);

    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);


    VoC_endloop0



    VoC_lw16i_set_inc(REG0,real_int,16);
    VoC_lw16i_set_inc(REG1,imag_int,16);


    //load the address of real and imag
    VoC_lw16i_set_inc(REG2,wnreal,2);
    VoC_lw16i_set_inc(REG3,wnimag,2);

    VoC_loop_i(1,4)

    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG3,DEFAULT);

    VoC_push32(REG01,DEFAULT)

    VoC_add16_rr(REG6,REG7,REG0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);

    VoC_loop_i_short(2,DEFAULT)
    VoC_push32(REG23,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32_p(REG23,REG6,DEFAULT);

    VoC_lw32_p(REG67,REG7,DEFAULT);
    VoC_shr32_ri(REG23,-1,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

    VoC_macX_rr(REG4,REG3,DEFAULT);
    VoC_macX_rr(REG5,REG3,IN_PARALLEL);

    VoC_msu32_rr(REG5,REG6,DEFAULT);
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);

    VoC_msuX_rr(REG5,REG7,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);

    VoC_pop32(REG67,DEFAULT);

    //real[l]=real[u]-XkWnreal;
    //real[u]=real[u]+XkWnreal;
    VoC_sub32_pr(RL6,REG0,ACC0,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG0,IN_PARALLEL);

    //imag[l]=imag[u]-XkWnimag;
    //imag[u]=imag[u]+XkWnimag;
    VoC_sub32_pr(RL7,REG1,ACC1,DEFAULT);
    VoC_add32_rp(ACC1,ACC1,REG1,IN_PARALLEL);

    VoC_sw32_p(RL6,REG6,DEFAULT);
    VoC_lw16i_short(REG2,8,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add16_rr(REG6,REG2,REG0,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG2,REG1,IN_PARALLEL);

    VoC_sw32_p(RL7,REG7,DEFAULT);

    VoC_endloop0

    VoC_pop32(REG23,DEFAULT);

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_add16_rr(REG0,REG5,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG5,REG1,IN_PARALLEL);

    VoC_endloop1


    VoC_lw16i_set_inc(REG0,real_int,2);
    VoC_lw16i_set_inc(REG1,imag_int,2);


    //load the address of real and imag
    VoC_lw16i_set_inc(REG2,wnreal,1);
    VoC_lw16i_set_inc(REG3,wnimag,1);

    VoC_lw16i_short(REG7,16,DEFAULT);

    VoC_add16_rr(REG6,REG7,REG0,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);

    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0

    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG3,DEFAULT);

    VoC_push32(REG23,DEFAULT);

    VoC_lw32_p(REG23,REG6,DEFAULT);

    VoC_lw32_p(REG67,REG7,DEFAULT);
    VoC_shr32_ri(REG23,-1,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

    VoC_macX_rr(REG4,REG3,DEFAULT);
    VoC_macX_rr(REG5,REG3,IN_PARALLEL);

    VoC_msu32_rr(REG5,REG6,DEFAULT);
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);

    VoC_msuX_rr(REG5,REG7,DEFAULT);
    VoC_macX_rr(REG4,REG7,IN_PARALLEL);

    VoC_pop32(REG67,DEFAULT);

    //real[l]=real[u]-XkWnreal;
    //real[u]=real[u]+XkWnreal;
    VoC_sub32_pr(RL6,REG0,ACC0,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG0,IN_PARALLEL);

    //imag[l]=imag[u]-XkWnimag;
    //imag[u]=imag[u]+XkWnimag;
    VoC_sub32_pr(RL7,REG1,ACC1,DEFAULT);
    VoC_add32_rp(ACC1,ACC1,REG1,IN_PARALLEL);

    VoC_sw32_p(RL6,REG6,DEFAULT);
    VoC_lw16i_short(REG4,16,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG0,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG1,DEFAULT);

    VoC_pop32(REG23,DEFAULT);

    VoC_sw32_p(RL7,REG7,DEFAULT);
    VoC_add16_rr(REG7,REG4,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_pop32(REG23,DEFAULT);

    VoC_lw16i_set_inc(REG0,real_int,2);
    VoC_lw16i_set_inc(REG1,imag_int,2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL)


    VoC_loop_i(1,2)

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);


    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);
    VoC_startloop0

    VoC_lw16i_short(STATUS,OVF_CLR,DEFAULT);
    VoC_shr32_ri(RL6,31,DEFAULT);

    VoC_xor32_rr(ACC0,RL6,DEFAULT);
    VoC_shr32_ri(RL7,31,IN_PARALLEL);

    VoC_xor32_rr(ACC1,RL7,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,RL6,IN_PARALLEL);


    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);
    VoC_shr32_ri(ACC0,4,IN_PARALLEL);

    VoC_xor32_rr(ACC0,RL6,DEFAULT);
    VoC_shr32_ri(ACC1,4,IN_PARALLEL);

    VoC_xor32_rr(ACC1,RL7,DEFAULT);

    VoC_boe0z16_r(Coolsand_IFFT16_L0)
    VoC_lw32z(RL7,DEFAULT);
Coolsand_IFFT16_L0:

    VoC_boe1z16_r(Coolsand_IFFT16_L1)
    VoC_lw32z(RL6,DEFAULT);
Coolsand_IFFT16_L1:

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);

    VoC_lw32inc_p(ACC1,REG0,DEFAULT);

    VoC_sw16inc_p(ACC1_HI,REG2,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);

    VoC_endloop0

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_movreg16(REG2,REG3,DEFAULT);

    VoC_endloop1


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_ReverseIndex
 *
 * Description: reverse the index when fft.
 *
 * Inputs:
 *      REG0: real or imag
 *
 * Outputs:     no
 *
 * Used : all
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/26/2010
 **************************************************************************************/
void Coolsand_ReverseIndex(void)
{


    VoC_push16(REG0,DEFAULT);

    //load data
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG0,DEFAULT);

    VoC_pop16(REG0,DEFAULT);

    //reverse the index
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_movreg16(RL7_HI,REG6,IN_PARALLEL);

    VoC_movreg16(ACC1_LO,RL6_LO,DEFAULT);
    VoC_movreg16(RL6_LO,ACC1_LO,IN_PARALLEL);

    VoC_movreg16(REG2,RL6_HI,DEFAULT);
    VoC_movreg16(RL6_HI,REG2,IN_PARALLEL);

    VoC_movreg16(REG4,ACC1_HI,DEFAULT);
    VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL);

    VoC_movreg16(REG1,ACC0_HI,DEFAULT);
    VoC_movreg16(ACC0_HI,REG1,IN_PARALLEL);

    VoC_movreg16(REG3,REG5,DEFAULT);
    VoC_movreg16(REG5,REG3,IN_PARALLEL);

    //save data
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG23,REG0,DEFAULT);
    VoC_sw16_p(REG1,REG0,DEFAULT);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_CopyVec
 *
 * Description: Copy Src to Dst
 *
 * Inputs:
 *      REG1: (pSrc)pointer to Source Data [In]
 *      REG2: (pDst)pointer to Destination Data [Out]
 *      REG7: (Len)Data Length [In].shorts
 *
 * Outputs: no
 *
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 *
 * NOTE:the len must be even and shorts.
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_CopyVec(void)
{

    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_CmplxAppxNorm_int
 *
 * Description: no
 *
 * Inputs:
 *   ACC0: pSrcI
 *   ACC1: pSrcQ
 *
 * Outputs: ACC0.
 *
 * Used : REG45,ACC0,ACC1
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 2.0  Created by  Xuml       08/25/2010
 **************************************************************************************/
void Coolsand_CmplxAppxNorm_int(void)
{

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,31,IN_PARALLEL);

    VoC_xor32_rr(REG45,ACC0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_shr32_ri(ACC1,31,IN_PARALLEL);

    VoC_xor32_rr(RL6,ACC1,DEFAULT);
    VoC_sub32_rr(ACC0,REG45,ACC0,IN_PARALLEL);

    VoC_sub32_rr(ACC1,RL6,ACC1,DEFAULT);

    VoC_bgt32_rr(Coolsand_CmplxAppxNorm_int_300,ACC0,ACC1)
    VoC_movreg32(ACC0,ACC1,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);//swap
Coolsand_CmplxAppxNorm_int_300:


    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-5,DEFAULT);//MaxVal*32
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);//MaxVal*31 == MaxVal*32 - MaxVal
    VoC_add32_rr(ACC1,ACC1,REG45,IN_PARALLEL);//MinVal*3 ==MinVal*2+MinVal

    VoC_shr32_ri(ACC0,5,DEFAULT); //(MaxVal*31)>>5
    VoC_shr32_ri(ACC1,3,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_return;



}


/**************************************************************************************
 * Function:    Coolsand_MSBPos
 *
 * Description: no
 *
 * Inputs:  RL6:Data
 *          REG2:BitNum
 *
 *
 * Outputs: REG3:return;
 *
 *
 * Used : REG2,REG3,ACC0,RL6,INC3.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 2.0  Created by  Xuml        08/25/2010
 **************************************************************************************/
void Coolsand_MSBPos(void)
{

    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_shr32_ri(RL6,31,IN_PARALLEL);

    VoC_xor32_rr(ACC0,RL6,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_loop_r_short(REG2,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_startloop0
    VoC_bez32_r(Coolsand_MSBPos_L0,ACC0)
    VoC_inc_p(REG3,DEFAULT);
    VoC_shr32_ri(ACC0,1,IN_PARALLEL);
    VoC_endloop0

Coolsand_MSBPos_L0:
    VoC_pop32(ACC0,DEFAULT);

    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_NonLinearTransform
 *
 * Description: no
 *
 * Inputs:
 *   REG0:Spk
 *
 *
 * Outputs:
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/20/2010
 **************************************************************************************/

/*
void Coolsand_NonLinearTransform(void)
{
    VoC_lw16i_short(REG6,-15,DEFAULT);
    VoC_lw16i_short(REG7,-15,IN_PARALLEL);

    VoC_sub16_rd(REG7,REG7,NonLinear_Beta_order_ADDR);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);

    VoC_shr32_rr(ACC0,REG6,DEFAULT);
    VoC_shr32_rr(ACC1,REG7,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_sub32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_shr32_ri(ACC1,1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_shr32_ri(ACC1,16,IN_PARALLEL);

    VoC_lw32z(RL6,DEFAULT);

    VoC_sw32_d(ACC0,UpLimit);
    VoC_sw32_d(ACC1,DnLimit);

    VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
    VoC_sub32_rr(ACC1,RL6,ACC1,IN_PARALLEL);


//  VoC_lw16i(REG6,NonLinearTransform_T_LO);
//  VoC_lw16i(REG7,NonLinearTransform_T_HI);

    VoC_lw32_d(REG67,NonLinear_T_ADDR)

    VoC_sw32_d(ACC0,Minus_UpLimit);
    VoC_sw32_d(ACC1,Minus_DnLimit);


    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

    VoC_shr32_ri(REG67,-1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_lw16_d(REG1,SoftLimiter_Beta_order_ADDR);

    VoC_loop_i(0,DEF_DATA_BLOCK_SIZE)

        VoC_multf32_rp(ACC0,REG6,REG0,DEFAULT);
        VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

        VoC_macX_rp(REG7,REG0,DEFAULT);
        VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);

        VoC_lw16i(REG4,cos_tab);

        VoC_bnlt32_rd(Coolsand_NonLinearTransform_L0,ACC0,Minus_UpLimit)

            VoC_lw16i_short(REG2,0,DEFAULT);
            VoC_sub16_rd(REG2,REG2,NonLinear_Scale_ADDR);
            VoC_movreg16(ACC0_HI,REG2,DEFAULT);
            //VoC_lw16i(ACC0_HI,-NonLinearTransform_Scale);
            VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L0:


        VoC_bgt32_rd(Coolsand_NonLinearTransform_L1,ACC0,Minus_DnLimit)


            VoC_shr32_ri(ACC1,-14,DEFAULT);
            VoC_lw16i_short(REG5,32,IN_PARALLEL);

            VoC_add32_rr(REG23,ACC0,ACC1,DEFAULT);
            VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

            VoC_shr32_rr(REG23,REG1,DEFAULT);
            VoC_lw32z(ACC1,IN_PARALLEL);

            VoC_sub32_rr(REG23,ACC1,REG23,DEFAULT);

            VoC_shr32_ri(REG23,15-NonLinearTransform_CosPhaseBitNum,DEFAULT);


            VoC_add16_rr(REG4,REG4,REG2,DEFAULT);
            VoC_lw16i_short(REG4,0,DEFAULT);

            VoC_lw16_p(REG5,REG4,DEFAULT);
            VoC_sub16_rr(REG4,REG4,REG1,IN_PARALLEL);

            VoC_shr16_rr(REG5,REG4,DEFAULT);

            VoC_shr32_ri(REG45,16,DEFAULT);
            VoC_sub32_rd(ACC0,ACC0,DnLimit);

            VoC_sub32_rr(REG45,ACC0,REG45,DEFAULT);
            VoC_shr32_ri(REG45,-1,DEFAULT);
            VoC_shru16_ri(REG4,1,DEFAULT);
            VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

            VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
            VoC_macX_rr(REG5,REG2,DEFAULT);
            VoC_NOP();
            VoC_shr32_ri(ACC0,-16,DEFAULT);



            VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L1:

        VoC_bnlt32_rd(Coolsand_NonLinearTransform_L2,ACC0,DnLimit)

            VoC_movreg32(REG45,ACC0,DEFAULT);
            VoC_shr32_ri(REG45,-1,DEFAULT);
            VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);

            VoC_shru16_ri(REG4,1,DEFAULT);
            VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);
            VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

            VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
            VoC_macX_rr(REG5,REG2,DEFAULT);

            VoC_lw16i_short(FORMATX,0,DEFAULT);
            VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

            VoC_shr32_ri(ACC0,-16,DEFAULT);

            VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L2:


        VoC_bgt32_rd(Coolsand_NonLinearTransform_L3,ACC0,UpLimit)


            VoC_shr32_ri(ACC1,-14,DEFAULT);
            VoC_lw16i_short(REG5,32,IN_PARALLEL);

            VoC_sub32_rr(REG23,ACC0,ACC1,DEFAULT);
            VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

            VoC_shr32_rr(REG23,REG1,DEFAULT);
            VoC_shr32_ri(REG23,15-NonLinearTransform_CosPhaseBitNum,DEFAULT);


            VoC_add16_rr(REG4,REG4,REG2,DEFAULT);
            VoC_lw16i_short(REG4,0,DEFAULT);

            VoC_lw16_p(REG5,REG4,DEFAULT);
            VoC_sub16_rr(REG4,REG4,REG1,IN_PARALLEL);

            VoC_shr16_rr(REG5,REG4,DEFAULT);

            VoC_shr32_ri(REG45,16,DEFAULT);
            VoC_add32_rd(ACC0,ACC0,DnLimit);

            VoC_add32_rr(REG45,ACC0,REG45,DEFAULT);
            VoC_shr32_ri(REG45,-1,DEFAULT);
            VoC_shru16_ri(REG4,1,DEFAULT);
            VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

            VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
            VoC_macX_rr(REG5,REG2,DEFAULT);
            VoC_NOP();
            VoC_shr32_ri(ACC0,-16,DEFAULT);
            VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L3:

        VoC_lw16_d(ACC0_HI,NonLinear_Scale_ADDR);

        VoC_NOP();
Coolsand_NonLinearTransform_End:
        VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);


    VoC_endloop0

    VoC_return;




}



/**************************************************************************************
 * Function:    Coolsand_ComputeNoiseWeight_DeciDirectCTRL
 *
 * Description:
 *
 * Inputs:
 *   REG7:
 *
 *
 * Outputs:
 *
 *
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_ComputeNoiseWeight_DeciDirectCTRL(void)
{
    VoC_push16(RA,DEFAULT);

    // Compute Instant Err Power
    VoC_lw16i(REG0,g_AF_ErrDataBufI);
    VoC_lw16i(REG1,g_AF_ErrDataBufQ);

    VoC_lw16_d(REG7,g_ch);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);

    VoC_jal(Cooland_InstantPower);

    VoC_lw16_d(REG5,g_ch);
    VoC_lw16i(REG6,r_PF_SmoothErrPower);
    VoC_lw16_d(REG7,m_AECProcPara_DataSmoothFactor);

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);

    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);

    VoC_push16(REG5,DEFAULT);


    VoC_jal(Coolsand_SmoothPeakPower);

    VoC_pop16(REG7,DEFAULT);

    VoC_lw16i(REG0,r_PF_NoisePowerEstBuf);
    VoC_lw16i(REG1,r_PF_SmoothErrPower);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_lw32_p(ACC0,REG0,DEFAULT);
    VoC_lw32_p(ACC1,REG1,DEFAULT);
    VoC_push32(REG01,DEFAULT);

    // Instant Err SNR
    VoC_jal(Coolsand_InstantSNR);

    //REG3:InstantSToNRatio

    //Spk  Noise Power Est
    VoC_jal(Coolsand_OverallNoiseEstwState);

    VoC_pop32(REG01,DEFAULT);//pNoisePowerEst

    VoC_lw16i_short(REG4,20,DEFAULT);

    VoC_lw32_p(REG23,REG0,DEFAULT);

    VoC_shr32_ri(REG23,-1,DEFAULT);//   NoisePowerEst_r = (*pNoisePowerEst)*BiasCorrectFactor;

    VoC_shr32_ri(REG23,4,DEFAULT);

    VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L0,REG23)
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L0:

    // Compute Noise Power Reverse
    VoC_jal(Coolsand_ShrinkBit);

    //ACC0:NoisePowerEst_r.d

    VoC_lw16i_short(REG4,5,DEFAULT);
    VoC_lw32_p(REG67,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);

    // Update Post SNR
    VoC_shr32_rr(REG67,REG4,DEFAULT);
    VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);

    VoC_movreg32(RL6,REG67,DEFAULT);
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

    VoC_and32_rr(RL6,RL7,DEFAULT);

    VoC_bez32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L1,RL6)
    VoC_add32_rr(REG67,REG67,RL7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L1:

    VoC_shr32_ri(REG67,-15,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    VoC_shr32_ri(RL7,-4,DEFAULT);

//  VoC_shr32_ri(REG67,-4,DEFAULT);
//  VoC_movreg16(REG5,REG6,IN_PARALLEL);

    VoC_lw16i(REG2,g_PF_PostSToNRatio);

    VoC_add16_rd(REG2,REG2,g_ch);

    VoC_sub32_pr(RL7,REG1,RL7,DEFAULT);

    VoC_sw16_p(REG7,REG2,DEFAULT);

    VoC_bnltz32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L2,RL7)
    VoC_lw32z(RL7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L2:

    VoC_lw16_d(REG2,g_ch);

    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_lw16i(REG3,r_PF_PrevSigPowerNBuf);

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);

    VoC_lw16_d(REG1,m_AECProcPara_SNRUpdateFactor);

    VoC_lw32_p(ACC1,REG3,DEFAULT);
    VoC_shr32_ri(RL7,4,IN_PARALLEL);

    // Update Prior Sig Power
//  VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

    VoC_shr32_ri(REG45,-8,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG45,7,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_macX_rr(REG5,REG1,DEFAULT);

    VoC_NOP();

    //ACC0:PriorSigPower

    VoC_pop32(RL6,DEFAULT);

    VoC_push32(ACC0,DEFAULT);
    VoC_lw16i_short(REG2,20,IN_PARALLEL);

    // Update Prior SNR
    //  NoisePowerFirstBitPos = MSBPos( NoisePowerEst_r.d,NoisePowerEst_r.t-1);
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i(REG1,g_PF_PriorSToNRatio);

    VoC_add16_rd(REG1,REG1,g_ch);

    VoC_pop32(ACC0,DEFAULT);
    VoC_lw16i_short(REG0,8,IN_PARALLEL);

    VoC_sub16_rr(REG0,REG3,REG0,DEFAULT);

    VoC_shr32_rr(ACC0,REG0,DEFAULT);

//  if ( pPriorSToNRatio->d  >= 0x1<<(pPriorSToNRatio->t-1) )
//      pPriorSToNRatio->d = (0x1<<(pPriorSToNRatio->t-1))-1;

    VoC_lw16i(REG0,g_PF_NoiseWeight);

    VoC_add16_rd(REG0,REG0,g_ch);

    VoC_shr32_ri(ACC0,-16,DEFAULT);

    VoC_lw16_p(REG3,REG0,DEFAULT);
    VoC_lw16i_short(REG4,8,IN_PARALLEL);

    VoC_shr32_ri(REG23,16,DEFAULT);
    VoC_sw16_p(ACC0_HI,REG1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save pNoiseWeight

    VoC_push16(ACC0_HI,DEFAULT);//save pPriorSToNRatio->d


    //Noise Weight
    VoC_jal(Coolsand_ShrinkBit);

    VoC_pop16(REG0,DEFAULT);//load pPriorSToNRatio->d
    VoC_shr32_ri(ACC0,-24,IN_PARALLEL);

    VoC_shr32_ri(ACC0,23,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_movreg16(REG1,ACC0_LO,DEFAULT);
    VoC_shr16_ri(REG2,-7,IN_PARALLEL);

    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L3,REG2,REG1)

    //Mul_SNRWeight = *pPriorSToNRatio*(NoiseWeight_r);
    VoC_multf16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L3:

    VoC_pop16(REG1,DEFAULT)//load pNoiseWeight

    VoC_lw16i_short(REG2,15,DEFAULT);

    VoC_add16_rp(REG0,REG0,REG1,DEFAULT);

    VoC_movreg16(RL6_HI,REG0,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save pNoiseWeight

    VoC_shr32_ri(RL6,16,DEFAULT);

    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(REG0,15,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG0,DEFAULT);
    VoC_shr16_ri(REG2,-7,IN_PARALLEL);//1.0

    VoC_lw16_d(REG1,m_AECProcPara_NoiseOverEstFactor);

    VoC_shr16_rr(REG1,REG3,DEFAULT);//TempFix.d
    VoC_pop16(REG4,IN_PARALLEL);//load pNoiseWeight

    VoC_lw16i_short(REG5,0,DEFAULT);
    //  if (Fix2Double(TempFix) >= 1.0 )
    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L4,REG2,REG1)
    VoC_sub16_rr(REG5,REG2,REG1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L4:

    VoC_lw16_d(REG0,m_AECProcPara_NoiseGainLimitStep);

    VoC_mult16_rd(REG0,REG0,g_ch);

    VoC_NOP();

    VoC_add16_rd(REG0,REG0,m_AECProcPara_NoiseGainLimit);

    VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L5,REG5,REG0)
    VoC_movreg16(REG5,REG0,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L5:

    VoC_pop16(RA,DEFAULT);

    VoC_sw16_p(REG5,REG4,DEFAULT);

    VoC_return;

}







/**************************************************************************************
 * Function:    Coolsand_ComputeNoiseWeight_DeciDirect
 *
 * Description:  Compute Noise Weight Through Decision Direct Method
 *
 * Inputs:
 *   REG7:
 *
 *
 * Outputs:
 *
 *
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/

/*
void Coolsand_ComputeNoiseWeight_DeciDirect(void)
{
    VoC_push16(RA,DEFAULT);

    // Compute Instant Err Power
    VoC_lw16i(REG0,g_AF_ErrDataBufI);
    VoC_lw16i(REG1,g_AF_ErrDataBufQ);

    VoC_lw16_d(REG7,g_ch);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);

    //REG0:pErrDataI;[in]
    //REG1:pErrDataQ;[in]
    //ACC0:InstantErrNorm[out]
    VoC_jal(Coolsand_CmplxAppxNorm);

    //InstantErrPower = InstantErrNorm*InstantErrNorm;
    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw16i_short(REG1,4,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_lw16i(REG2,r_PF_SmoothErrPower);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    VoC_lw16i(REG3,r_PF_NoisePowerEstBuf);

    VoC_lw32_p(ACC1,REG2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_push32(RL7,DEFAULT);

    //RL7:InstantErrPower[in]
    //REG1:ErrSmoothFactor[in]
    //ACC1:*pSmoothErrPower[in]

    //ACC0:*pSmoothErrPower[out]

    VoC_shr32_ri(RL7,-4,DEFAULT);

    VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    // Instant SNR
    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_lw32_p(RL6,REG3,IN_PARALLEL);

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_shr32_ri(RL6,4,IN_PARALLEL);

    VoC_push32(RL6,DEFAULT);//backup NoisePowerEst_r = *pNoisePowerEst;
    VoC_push16(REG3,IN_PARALLEL);

    VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L0,RL6)
        VoC_lw16i_short(RL6_LO,1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L0:

    //  *pSmoothErrPower = FirstOrderIIRSmooth2(pSmoothErrPower, &InstantErrPower, &ErrSmoothFactor);


    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_lw16i_short(REG2,20,DEFAULT);//xuml
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    //RL6:NoisePowerEst_r.d.[in]
    //REG2:NoisePowerEst_r.t-1.[in]

    ////REG3: NoisePowerFirstBitPos.[out]
    VoC_jal(Coolsand_MSBPos);

    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);

    VoC_shr32_rr(ACC0,REG3,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);

    //if (InstantSToNRatio.d >= (0x1<<(InstantSToNRatio.t-1)))
    //InstantSToNRatio.d = (0x1<<(InstantSToNRatio.t-1))-1;
    VoC_shr32_ri(ACC0,-24,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,24,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);

    VoC_lw16_d(REG6,m_AECProcPara_NoiseUpdateFactor);

    VoC_push16(ACC0_LO,DEFAULT);//InstantSToNRatio
    VoC_push32(RL6,IN_PARALLEL);

    VoC_push32(ACC1,DEFAULT);//PrevNoisePowerEst



    //RL6:pSmoothErrPower
    //RL7:PrevSmoothErrPower
    //ACC1:pNoisePowerEst
    //REG6:NoiseUpdateFactor
    //ACC0:InstantSToNRatio
    //REG7:&pNoisePowerEst

    VoC_jal(Coolsand_NoisePSDEst);

    VoC_pop32(ACC1,DEFAULT);//PrevNoisePowerEst

    VoC_pop32(RL6,DEFAULT);//*pSmoothErrPower
    VoC_pop16(REG0,IN_PARALLEL);//InstantSToNRatio

    VoC_bez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L1,RL6)

        VoC_lw16i(REG2,r_PF_NoiseFloorDevCnt);
        VoC_lw16i(REG3,r_PF_NoisePowerEstBuf);
        VoC_lw16_d(REG1,g_ch);

        VoC_add16_rr(REG2,REG2,REG1,DEFAULT);
        VoC_shr16_ri(REG1,-1,IN_PARALLEL);

        VoC_add16_rr(REG3,REG3,REG1,DEFAULT);

        VoC_lw16i_short(REG1,0,DEFAULT);
        VoC_lw16i_short(REG3,8*4,IN_PARALLEL);

        VoC_lw32_p(ACC0,REG3,DEFAULT);

        VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L2,REG0,REG3)
        VoC_bne32_rr  (Coolsand_ComputeNoiseWeight_DeciDirect_L2,ACC1,ACC0)

            VoC_lw16i_short(REG4,1,DEFAULT);
            VoC_add16_rp(REG1,REG4,REG2,DEFAULT);

Coolsand_ComputeNoiseWeight_DeciDirect_L2:
        VoC_lw16i(REG5,500);
        VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L3,REG1,REG5)
            //*pNoiseFloorDevCnt = 0;
            //pNoisePowerEst->d = pNoisePowerEst->d*2;
            VoC_lw16i_short(REG1,0,DEFAULT);
            VoC_shr32_ri(ACC1,-1,IN_PARALLEL);
            VoC_NOP();
            VoC_sw32_p(ACC1,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L3:
            VoC_sw16_p(REG1,REG2,DEFAULT);

Coolsand_ComputeNoiseWeight_DeciDirect_L1:

    // Bias Noise Power

    //NoisePowerEst_r = *pNoisePowerEst;
    VoC_lw32_p(ACC1,REG7,DEFAULT);

    //Truncate(NoisePowerEst_r, pNoisePowerEst->t-4, pNoisePowerEst->f-4);
    VoC_shr32_ri(ACC1,4,DEFAULT);

    //NoisePowerEst_r = NoisePowerEst_r*BiasCorrectFactor;
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_shr32_ri(ACC1,-12,DEFAULT);
    VoC_shr32_ri(ACC1,12,DEFAULT);


    VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L4,ACC1)
        VoC_lw16i_short(ACC1_LO,1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L4:
    VoC_push32(ACC1,DEFAULT);
    // Compute Noise Power Reverse

    VoC_movreg32(REG23,ACC1,DEFAULT);
    VoC_lw16i_short(REG4,21,IN_PARALLEL);

    //REG23:NoisePowerEst_r.d[in]
    //REG4:NoisePowerEst_r.t[in]
    //ACC0:NoisePowerEst_r.d[out]
    //REG3:[out]
    VoC_jal(Coolsand_ShrinkBit);

    //VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);//NoisePowerEst_r2
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_pop32(RL7,DEFAULT);//InstantErrPower
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    // Update Post SNR
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shr32_ri(RL7,-9,IN_PARALLEL);

    VoC_shr32_rr(RL7,REG3,DEFAULT);

    VoC_add32_rr(RL7,RL7,REG45,DEFAULT);
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);

    VoC_shr32_ri(RL7,1,DEFAULT);
    VoC_shr32_ri(ACC0,31,IN_PARALLEL);

    VoC_xor32_rr(RL7,ACC0,DEFAULT);

    VoC_sub32_rr(RL7,RL7,ACC0,DEFAULT);
    VoC_sub32_rr(ACC0,RL6,ACC1,IN_PARALLEL);


    VoC_bnltz32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L5,ACC0)
        VoC_lw32z(ACC0,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L5:


    VoC_shr32_ri(RL7,-16,DEFAULT);
    VoC_shr32_ri(ACC0,-12,IN_PARALLEL);

    VoC_lw16i(REG0,g_PF_PostSToNRatio);
    VoC_lw16_d(REG7,g_ch);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_lw16i(REG2,r_PF_PrevSigPowerNBuf);
    VoC_lw16_d(REG1,m_AECProcPara_SNRUpdateFactor);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_shr32_ri(ACC0,12,IN_PARALLEL);

    VoC_sw16_p(RL7_HI,REG0,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    // Update Post Sig Power

    //ACC1:pPrevSigPowerN[in]
    //RL7:PostSigPower[in]
    //REG1:SNRUpdateFactor[in]

    //ACC0:PriorSigPower[out]

    //VoC_shr32_ri(RL7,-4,DEFAULT);

//  VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    VoC_sub32_rp(REG45,RL7,REG2,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_shr32_ri(REG45,-8,DEFAULT);
    VoC_lw32_p(ACC0,REG2,IN_PARALLEL);

    VoC_shr32_ri(REG45,7,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_macX_rr(REG5,REG1,DEFAULT);
    VoC_lw16i_short(REG2,20,IN_PARALLEL);//xuml

    //RL6:NoisePowerEst_r.d.[in]
    //REG2:NoisePowerEst_r.t-1.[in]

    //REG3: NoisePowerFirstBitPos.[out]
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i(REG1,g_PF_NoiseWeight);
    VoC_lw16_d(REG7,g_ch);

    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_sub16_rr(REG0,REG3,REG4,DEFAULT);

    VoC_shr32_rr(ACC0,REG0,DEFAULT);
    VoC_lw16_p(REG3,REG1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_shr32_ri(REG23,16,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw16i_short(REG4,9,IN_PARALLEL);

    //REG23:NoiseWeight_r.d[in]
    //REG4:NoiseWeight_r.t[in]
    //ACC0:NoiseWeight_r.d[out]
    //REG3:[out]

    VoC_jal(Coolsand_ShrinkBit);

    VoC_movreg16(REG2,ACC0_LO,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    //NoiseWeight_r.d  = NoiseWeight_r.d*2;
    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shr16_ri(REG5,-7,IN_PARALLEL);

    //Mul_SNRWeight = *pPriorSToNRatio;
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L6,REG5,REG2)
        VoC_multf16_rr(REG4,REG2,REG0,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L6:

    VoC_pop16(REG2,DEFAULT);

    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);

//  VoC_lw16i_short(RL6_HI,1,DEFAULT);
//  VoC_bez16_r(Coolsand_ComputeNoiseWeight_DeciDirect_L7,REG4)
//      VoC_movreg16(RL6_HI,REG4,DEFAULT);
//Coolsand_ComputeNoiseWeight_DeciDirect_L7:

    VoC_movreg16(RL6_HI,REG4,DEFAULT);

    VoC_shr32_ri(RL6,16,DEFAULT);
    VoC_lw16i_short(REG2,16,IN_PARALLEL); //xuml

    //RL6:Mul_SNRWeight.d.[in]
    //REG2:Mul_SNRWeight.t-1.[in]

    //REG3: MulSNRWeightFirstBitPos.[out]
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(FORMAT16,-16,DEFAULT);

    VoC_lw16_d(REG7,g_ch);

    VoC_lw16_d(REG1,m_AECProcPara_NoiseOverEstFactor);

    VoC_lw16i_short(REG0,15,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);


    VoC_sub16_rr(REG0,REG3,REG0,DEFAULT);
    VoC_shr16_ri(REG2,-7,IN_PARALLEL);

    VoC_lw16i(REG3,g_PF_NoiseWeight);
    VoC_lw16i(REG5,g_PF_NoiseAmpWeight);

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    //  DynamicNoiseGainLimit = NoiseGainLimitStep*ch;
    VoC_mult16_rd(REG7,REG7,NoiseGainLimitStep_ADDR);

    VoC_shr16_rr(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_add16_rd(REG7,REG7,m_AECProcPara_NoiseGainLimit);

    VoC_shr16_ri(REG7,-8,DEFAULT);

    VoC_shr16_ri(REG7,8,DEFAULT);

    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L8,REG2,REG1)
        VoC_sub16_rr(REG4,REG2,REG1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L8:

    VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L11,REG4,REG7)
        VoC_movreg16(REG4,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L11:

    VoC_lw16_d(REG2,AmpThr_ADDR);
    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG2,REG7)
        VoC_movreg16(REG2,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:


    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_shr16_ri(REG6,-4,DEFAULT);

    VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG4,REG2)
        VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);
Coolsand_ComputeNoiseWeight_DeciDirect_L10:

*/


/*

    VoC_lw16_d(REG7,NoiseWeight_threshold_ADDR);
    VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
    VoC_shr16_ri(REG6,-4,DEFAULT);



    VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG7,REG4)
        VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);
        VoC_jump(Coolsand_ComputeNoiseWeight_DeciDirect_L10);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:

    VoC_bngt16_rd(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG4,m_AECProcPara_NoiseGainLimit)
        VoC_movreg16(REG6,REG4,DEFAULT);
        VoC_shr16_ri(REG6,-4,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L10:


    */




/*

    VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);

    VoC_bgt16_rd(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG4,m_AECProcPara_NoiseGainLimit)
        VoC_lw16_d(REG4,m_AECProcPara_NoiseGainLimit);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:

        VoC_lw16_d(REG7,m_AECProcPara_NoiseGainLimit);
        VoC_shr16_ri(REG7,-4,DEFAULT);
    VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG6,REG7)
        VoC_movreg16(REG6,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L10:

  */


/*
    // Update Post SNR
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_sw16_p(REG6,REG5,DEFAULT);
    VoC_return;
}
*/


/**************************************************************************************
 * Function:    Coolsand_CmplxAppxNorm
 *
 * Description:  no
 *
 * Inputs:
 *   REG0: *pErrDataI
 *   REG1: *pErrDataQ
 *
 * Outputs:
 *   ACC0: *pNorm
 *
 * Used : REG0,REG1,REG2,REG3,ACC0.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/
void Coolsand_CmplxAppxNorm(void)
{

    //AbsI = ABS(*pSrcI);
    //AbsQ = ABS(*pSrcQ);
    VoC_movreg32(REG23,REG01,DEFAULT);
    VoC_shr16_ri(REG0,15,IN_PARALLEL);

    VoC_xor16_rr(REG2,REG0,DEFAULT)
    VoC_shr16_ri(REG1,15,IN_PARALLEL);

    VoC_xor16_rr(REG3,REG1,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG0,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG0,31,IN_PARALLEL);

    VoC_bgt16_rr(Coolsand_CmplxAppxNorm_L0,REG2,REG3)

    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

Coolsand_CmplxAppxNorm_L0:

    VoC_lw16i_short(FORMAT32,5,DEFAULT);
    VoC_lw16i_short(FORMATX,3,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT);
    VoC_lw16i_short(REG1,3,IN_PARALLEL);

    VoC_macX_rr(REG3,REG1,DEFAULT);

    VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_FirstOrderIIRSmooth2
 *
 * Description:  no
 *
 * Inputs:
 *   ACC0: *pSmoothErrPower
 *   RL7: InstantErrPower
 *   REG1: ErrSmoothFactor
 *
 * Outputs:
 *   ACC0: *pSmoothErrPower
 *
 * Used : REG1,REG4,REG5,RL7,ACC0,ACC1.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/
void Coolsand_FirstOrderIIRSmooth2(void)
{

    VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);

    VoC_shr32_ri(REG45,-8,DEFAULT);
//  VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG45,7,DEFAULT);
    //VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    //VoC_lw16i_short(FORMATX,-4,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_macX_rr(REG5,REG1,DEFAULT);

    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_NoisePSDEst
 *
 * Description:  no
 *
 * Inputs:
 *
 *  REG0:pSigPower(address)
 *  REG1:pNoisePower(address)
 *  REG2:NoiseUpdateFactor
 *  REG3:pInstantSNR
 *  REG4:UpdateSpeed0
 *  REG5:UpdateSpeed1
 *
 *  REG6:FrameCnt
 *
 * Outputs: no
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 * Version 2.0  Created by  Xuml       04/21/2011
 **************************************************************************************/
void Coolsand_NoisePSDEst(void)
{


    VoC_push16(RA,DEFAULT);

    VoC_lw16i(REG7,50);

    VoC_lw32_p(ACC0,REG0,DEFAULT);//pSigPower
    VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower

    VoC_bngt16_rr(Coolsand_NoisePSDEst_L0,REG7,REG6)

    VoC_movreg32(ACC1,ACC0,DEFAULT);

    VoC_jump(Coolsand_NoisePSDEst_Lx);
Coolsand_NoisePSDEst_L0:

    VoC_bngt32_rr(Coolsand_NoisePSDEst_L1,ACC1,ACC0)

    VoC_bez32_r(Coolsand_NoisePSDEst_L1_sub0,ACC0)

    VoC_movreg32(ACC1,ACC0,DEFAULT);

    VoC_jump(Coolsand_NoisePSDEst_Lx);
Coolsand_NoisePSDEst_L1_sub0:

    VoC_lw16i_short(ACC1_LO,1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_jump(Coolsand_NoisePSDEst_Lx);


Coolsand_NoisePSDEst_L1:




    VoC_lw16i_short(RL6_LO,16,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_NoisePSDEst_L2,ACC1,RL6)

    VoC_movreg16(REG7,REG4,DEFAULT);
    VoC_jump(Coolsand_NoisePSDEst_L4);
Coolsand_NoisePSDEst_L2:

    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_lw16i_short(RL6_LO,32,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_NoisePSDEst_L3,RL6,ACC1)
    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_jump(Coolsand_NoisePSDEst_L4);
Coolsand_NoisePSDEst_L3:

    VoC_shr16_ri(REG3,-10,DEFAULT);
    VoC_shr16_ri(REG3,10,DEFAULT);
    VoC_lw16i(REG4,AdaptTable);
    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG7,REG4,DEFAULT);

Coolsand_NoisePSDEst_L4:


    VoC_multf16_rr(REG3,REG2,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT16,4-16,IN_PARALLEL);

    VoC_lw16_d(REG4,pInstantSNR_t);

    VoC_push32(ACC0,DEFAULT);
    VoC_shr16_ri(REG3,-4,IN_PARALLEL);

    VoC_shr32_ri(REG23,4+16,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);


    //REG23:AdptNoiseUpdateFactor.d[in]
    //REG4:AdptNoiseUpdateFactor.t[in]
    //ACC0:AdptNoiseUpdateFactor.d[out]


    VoC_jal(Coolsand_ShrinkBit);

    VoC_pop16(REG1,DEFAULT);

    VoC_pop32(RL7,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);

    VoC_bez32_r(Coolsand_NoisePSDEst_Lx,RL7)

    //ACC1:pNoisePowerEst[in]
    //RL7:pInstantErrPower[in]
    //REG7:AdptNoiseUpdateFactor[in]

    //ACC0:*pSmoothErrPower[out]

    //  VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    VoC_lw16_d(REG2,pInstantSNR_f);

    VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

    VoC_shr32_ri(REG45,-8,DEFAULT);
    VoC_lw16i_short(REG3,15,IN_PARALLEL);


    VoC_shr32_ri(REG45,7,DEFAULT);
    VoC_sub16_rr(REG3,REG2,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);

    //VoC_lw16i_short(FORMAT32,11,DEFAULT);
    VoC_movreg16(FORMAT32,REG2,DEFAULT);
    VoC_mac32_rr(REG4,REG7,IN_PARALLEL);

    //VoC_lw16i_short(FORMATX,11-15,DEFAULT);
    VoC_movreg16(FORMATX,REG3,DEFAULT);
    VoC_macX_rr(REG5,REG7,IN_PARALLEL);

Coolsand_NoisePSDEst_Lx:

    VoC_lw16i_short(RL6_LO,4,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_NoisePSDEst_END,ACC1,RL6)
    VoC_movreg32(ACC1,RL6,DEFAULT);
Coolsand_NoisePSDEst_END:

    VoC_pop16(RA,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_ShrinkBit
 *
 * Description:  no
 *
 * Inputs:
 *  REG23:
 *  REG4:
 *
 * Outputs: ACC0,REG3
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/

void Coolsand_ShrinkBit(void)
{

    VoC_movreg32(RL6,REG23,DEFAULT);
    VoC_shr32_ri(REG23,31,IN_PARALLEL);

    VoC_movreg32(RL7,REG23,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);


    //RL6:tempData
    //REG2:BitNum-1

    //REG3: FirstBitPos
    VoC_jal(Coolsand_MSBPos);

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_bez16_r(Coolsand_ShrinkBit_L0,REG3)

    VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_xor32_rr(RL6,RL7,DEFAULT);
    VoC_shr32_rr(REG45,REG2,IN_PARALLEL);

    VoC_sub32_rr(RL6,RL6,RL7,DEFAULT);

    VoC_and32_rr(REG45,RL6,DEFAULT);

    VoC_xor32_rr(REG45,RL7,DEFAULT);

    VoC_sub32_rr(ACC0,REG45,RL7,DEFAULT);

Coolsand_ShrinkBit_L0:

    VoC_return;

}



/**************************************************************************************
 * Function:    Coolsand_PostFiltering
 *
 * Description:  no
 *
 * Inputs:
 *
 *
 *
 * Outputs:no
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/25/2010
 * Version 2.0  Created by  Xuml       05/04/2011
 **************************************************************************************/

void Coolsand_PostFiltering(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG7,g_ch);

    VoC_lw16i(REG0,g_PF_EchoWeight);

    VoC_lw16i(REG1,g_PF_NoiseWeight);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_lw16_d(REG3,m_AECProcPara_NoiseGainLimitStep);

    VoC_lw16_p(REG0,REG0,DEFAULT);

    VoC_lw16_p(REG1,REG1,DEFAULT);

    VoC_mult16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_mult16_rr(REG0,REG0,REG1,IN_PARALLEL);

    VoC_lw16i(REG4,127);

    VoC_add16_rd(REG3,REG3,m_AECProcPara_NoiseGainLimit);

    VoC_bngt16_rr(Coolsand_PostFiltering_L0,REG4,REG3)

    VoC_movreg16(REG4,REG3,DEFAULT);

Coolsand_PostFiltering_L0:

    VoC_shr16_ri(REG4,-7,DEFAULT);

    VoC_bngt16_rr(Coolsand_PostFiltering_L1,REG0,REG4)

    VoC_mult16_rd(REG1,REG1,m_AECProcPara_AmpGain);//*pNoiseAmpWeight = (*pNoiseWeight)*AmpGain;
    VoC_jump(Coolsand_PostFiltering_L2) ;
Coolsand_PostFiltering_L1:
    VoC_shr16_ri(REG1,-4,DEFAULT);
Coolsand_PostFiltering_L2:

    VoC_lw16i(REG2,g_PF_NoiseAmpWeight);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

    VoC_lw16i(REG3,g_PF_EchoWeight);


    VoC_sw16_p(REG1,REG2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_lw16i(REG0,g_AF_ErrDataBufI);
    VoC_lw16i(REG1,g_AF_ErrDataBufQ);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_lw16i_short(FORMAT16,7-16,DEFAULT);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);

    VoC_multf16_rp(REG0,REG0,REG3,DEFAULT);
    VoC_multf16_rp(REG1,REG1,REG3,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);//save (*pErrDataI) and (*pErrDataQ)

    VoC_movreg32(RL7,REG01,DEFAULT);

    //REG0:pOutDataI;[in]
    //REG1:pOutDataQ;[in]
    //ACC0:OutNorm[out]

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_shr32_ri(ACC0,-16,DEFAULT);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);

    VoC_lw16i(REG1,r_PF_PrevSigPowerEBuf);

    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);

    // Noise Amp Weight
    VoC_lw16i(REG2,g_PF_NoiseAmpWeight);

    VoC_sw32_p(ACC0,REG1,DEFAULT);//    pPrevSigPowerE->d = PrevSigPower.d;

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);

    VoC_lw16i_short(FORMAT16,11-16,DEFAULT);

    VoC_multf16_rp(REG0,REG0,REG2,DEFAULT);
    VoC_multf16_rp(REG1,REG1,REG2,IN_PARALLEL);

    VoC_NOP();

    VoC_movreg32(RL7,REG01,DEFAULT);

    //REG0:pOutDataI;[in]
    //REG1:pOutDataQ;[in]
    //ACC0:OutNorm[out]

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_shr32_ri(ACC0,-16,DEFAULT);

    VoC_lw16i(REG2,g_PF_NoiseWeight);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_push16(ACC0_HI,DEFAULT);
    VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

    VoC_multf16_rp(REG0,REG0,REG2,DEFAULT);
    VoC_multf16_rp(REG1,REG1,REG2,IN_PARALLEL);

    VoC_lw16i(REG4,r_PF_PrevSigPowerNBuf);
//  VoC_lw16i(REG5,r_PF_PrevSigPowerEBuf);

    VoC_shr16_ri(REG7,-1,DEFAULT);

    //REG0:pOutDataI;[in]
    //REG1:pOutDataQ;[in]
    //ACC0:OutNorm[out]

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_lw16i(REG1,r_PF_NoisePowerSmoothEstBuf);

    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);

    VoC_lw32_p(ACC1,REG1,DEFAULT);
//  VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    //VoC_shr32_ri(ACC1,4,DEFAULT);
    VoC_movreg32(RL6,ACC1,DEFAULT);

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_lw32_p(ACC0,REG4,IN_PARALLEL)

    VoC_push16(REG4,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,11,DEFAULT);
    VoC_lw16i_short(FORMATX,-4,IN_PARALLEL);

    VoC_lw16_d(REG1,m_AECProcPara_DataSmoothFactor);

    VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_movreg32(ACC1,RL6,DEFAULT);

    VoC_sw32_p(ACC0,REG4,DEFAULT);
    VoC_shr32_ri(ACC1,4,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-4,DEFAULT);

    VoC_bgt32_rd(Coolsand_PostFiltering_L3,ACC1,m_AECProcPara_NoiseMin)
    VoC_lw32_d(ACC1,m_AECProcPara_NoiseMin);
Coolsand_PostFiltering_L3:

    VoC_lw16_d(REG2,m_AECProcPara_CNGFactor);

    VoC_movreg32(REG01,ACC1,DEFAULT);

    VoC_shr32_ri(REG01,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,-8,IN_PARALLEL);

    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_lw16i_short(FORMATX,-8-15,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG2,DEFAULT);

    VoC_macX_rr(REG1,REG2,DEFAULT);

    VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);

    VoC_shr32_ri(ACC0,8,DEFAULT);

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG2,24,IN_PARALLEL);//xuml


    //RL6:TargetNoisePower.d
    //REG2:TargetNoisePower.t-1

    //REG3: TargetNoisePowerFirstBitPos
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG1,24,IN_PARALLEL);

    VoC_bez16_r(Coolsand_PostFiltering_L5,REG3)

    VoC_sub16_rr(REG2,REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG4,23,IN_PARALLEL);


    VoC_and16_ri(REG2,0x0001);

    VoC_bez16_r(Coolsand_PostFiltering_L4,REG2)

    VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG4,9,IN_PARALLEL);

    VoC_shr16_ri(REG2,1,DEFAULT);

    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG0,3,IN_PARALLEL);

    VoC_shr16_rr(REG0,REG2,DEFAULT);

    VoC_jump(Coolsand_PostFiltering_L5);
Coolsand_PostFiltering_L4:

    VoC_sub16_rr(REG2,REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG4,11,IN_PARALLEL);

    VoC_shr16_ri(REG2,1,DEFAULT);

    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_shr16_rr(REG0,REG2,DEFAULT);
Coolsand_PostFiltering_L5:

    VoC_multf32_rr(ACC0,REG0,REG6,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i(REG2,g_OutSynFilterInBufI);
    VoC_lw16i(REG3,g_OutSynFilterInBufQ);

    VoC_lw16_d(REG7,g_ch);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_pop16(REG1,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_lw32z(ACC1,DEFAULT);

    VoC_bez32_d(Coolsand_PostFiltering_L8,m_AECProcPara_NoiseMin)
    VoC_lw16i(ACC1_LO,0x5b);
Coolsand_PostFiltering_L8:

    VoC_bngt32_rr(Coolsand_PostFiltering_L6,ACC1,ACC0)

    VoC_movreg32(ACC0,ACC1,DEFAULT);

Coolsand_PostFiltering_L6:

    VoC_shr32_ri(ACC0,-13,DEFAULT);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL);

    VoC_shr16_ri(REG0,-1,DEFAULT);
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);


    VoC_bne16_rd(Coolsand_PostFiltering_L9,REG6,r_PF_CL_State)
    VoC_bgt16_rd(Coolsand_PostFiltering_L7,REG6,g_ch)
Coolsand_PostFiltering_L9:
    VoC_bngt16_rr(Coolsand_PostFiltering_L7,REG0,REG1)

    //REG0[out]
    //REG1[out]
    VoC_jal(Coolsand_Generate);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);

    VoC_shr16_ri(REG0,-1,DEFAULT);
    VoC_shr16_ri(REG1,-1,IN_PARALLEL);

    VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG6,IN_PARALLEL);

    VoC_mult16_rr(REG4,REG4,REG0,DEFAULT);
    VoC_mult16_rr(REG5,REG4,REG1,IN_PARALLEL);

    VoC_NOP();

    VoC_jump(Coolsand_PostFiltering_End);

Coolsand_PostFiltering_L7:
    //REG0[out]
    //REG1[out]
    VoC_jal(Coolsand_Generate);

Coolsand_PostFiltering_End:
    VoC_pop16(RA,DEFAULT);

    VoC_sw16_p(REG4,REG2,DEFAULT);
    VoC_sw16_p(REG5,REG3,DEFAULT);

    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_Generate
 *
 * Description:     Generate the PN (Random) Sequence
 *
 * Inputs:
 *
 *
 *
 * Outputs:no
 *
 *
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/25/2010
 **************************************************************************************/

void Coolsand_Generate(void)
{

    VoC_lw16i_set_inc(REG0,m_Taps,1);
    VoC_lw16i_set_inc(REG1,m_Registers,1);

    VoC_loop_i_short(CNGTapLen,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_startloop0
    VoC_add16inc_rp(REG6,REG1,REG0,DEFAULT);
    VoC_NOP();
    VoC_add16_rp(REG7,REG7,REG6,DEFAULT);
    VoC_endloop0


    VoC_lw16_d(ACC1_LO,m_Registers+31);
    VoC_lw16_d(ACC1_HI,m_Registers+26);


    VoC_lw16i_set_inc(REG0,m_Registers+CNGRegLen-1,-1);
    VoC_lw16i_set_inc(REG1,m_Registers+CNGRegLen-2,-1);


    VoC_movreg16(REG6,REG7,DEFAULT);
    VoC_shr16_ri(REG7,15,IN_PARALLEL);

    VoC_and16_ri(REG6,0x0001);

    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_lw16inc_p(ACC0_LO,REG1,IN_PARALLEL);


    VoC_loop_i_short(CNGRegLen-1,DEFAULT)
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_endloop0

    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);


    VoC_return;



}




/**************************************************************************************
 * Function:    Coolsand_SumInstantPower
 *
 * Description:
 *
 * Inputs:  REG2:
 *          REG3:
 *
 *
 * Outputs:REG7
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/

void Coolsand_SumInstantPower(void)
{
    VoC_push16(RA,DEFAULT);

//  VoC_loop_i_short(1,DEFAULT);
//  VoC_lw32z(ACC1,IN_PARALLEL);
//  VoC_startloop0
    VoC_lw16inc_p(REG0,REG2,DEFAULT);
    VoC_lw16inc_p(REG1,REG3,DEFAULT);

    VoC_jal(Coolsand_InstantPower);

    //  VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);
//  VoC_endloop0

    //          SumMicPower.d = SumMicPower.d/PF_CL_CHNLNUM;
    //          SumSpkPower.d = SumSpkPower.d/PF_CL_CHNLNUM;


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_InstantPower
 *
 * Description:
 *
 * Inputs:  REG0:
 *          REG1:
 *
 *
 * Outputs:ACC0
 *
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,ACC0.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/
void Coolsand_InstantPower(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_push32(REG23,DEFAULT);

    //REG0:
    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_pop16(RA,DEFAULT);
    VoC_shr32_ri(ACC0,-16,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG4,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ControlLogicMergeChnlEnhancedDTD
 *
 * Description:
 *
 * Inputs:  REG0:
 *
 *
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 * Version 1.1  Optimized by  Xuml       11/04/2011
 **************************************************************************************/
void Coolsand_ControlLogicMergeChnlEnhancedDTD(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_jal(Coolsand_ControlLogicCalVarSub);

    VoC_pop32(RL7,DEFAULT);


    VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

    VoC_lw16i(REG6,r_PF_CL_ErrSmoothPower);

    // Smooth Spk Power
    VoC_jal(Coolsand_SmoothPeakPower);

    VoC_jal(Coolsand_ControlLogicChnlDecisionEnhanced);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_ControlLogicSingleChnlEnhanced
 *
 * Description:
 *
 * Inputs:  REG0:
 *
 *
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/22/2011
 **************************************************************************************/
void Coolsand_ControlLogicSingleChnlEnhanced(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG0,2,IN_PARALLEL);

    VoC_lw16d_set_inc(REG2,r_PF_CL_DTCountDown,-1);

    VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L1,REG0,r_PF_CL_ChnlState)

    VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
    VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

    VoC_bgt16_rr(Coolsand_ControlLogicSingleChnl_L0,REG5,REG4)

    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_movreg16(REG4,REG5,IN_PARALLEL);
    VoC_lw16_d(REG2,m_AECProcPara_PFCLDTHoldTime);

    VoC_jump(Coolsand_ControlLogicSingleChnl_End);

Coolsand_ControlLogicSingleChnl_L0:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    //VoC_lw16_d(REG1,r_PF_CL_State);



    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);

    VoC_jump(Coolsand_ControlLogicSingleChnl_End);
Coolsand_ControlLogicSingleChnl_L1:

    VoC_lw16i_short(REG0,1,DEFAULT);

    VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L4,REG0,r_PF_CL_ChnlState)


    VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
    VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

    VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L2,REG5,REG4)
    VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L2:

    VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L3,r_PF_CL_DTCountDown)
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_jump(Coolsand_ControlLogicSingleChnl_End);

Coolsand_ControlLogicSingleChnl_L3:
    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_jump(Coolsand_ControlLogicSingleChnl_End);
Coolsand_ControlLogicSingleChnl_L4:


    VoC_lw16i_short(REG0,3,DEFAULT);
    VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L7,REG0,r_PF_CL_ChnlState)

    VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
    VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

    VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L5,REG5,REG4)
    VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L5:

    VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L6,r_PF_CL_DTCountDown)
    VoC_lw16i_short(REG1,3,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_jump(Coolsand_ControlLogicSingleChnl_End);

Coolsand_ControlLogicSingleChnl_L6:
    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_jump(Coolsand_ControlLogicSingleChnl_End);
Coolsand_ControlLogicSingleChnl_L7:


    VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
    VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

    VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L8,REG5,REG4)
    VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L8:


    VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L9,r_PF_CL_DTCountDown)
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_jump(Coolsand_ControlLogicSingleChnl_End);
Coolsand_ControlLogicSingleChnl_L9:
    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

Coolsand_ControlLogicSingleChnl_End:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG1,r_PF_CL_State);
    VoC_sw16_d(REG2,r_PF_CL_DTCountDown);
    VoC_sw16_d(REG4,r_PF_CL_DTCountUp);

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_ControlLogicCalVarSub
 *
 * Description:
 *
 * Inputs:  RL6:pMicDataPower
 *          RL7:pSpkDataPower
 *
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/
void Coolsand_ControlLogicCalVarSub(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

//**********************spk****************************//

//  VoC_lw32_d(ACC0,r_PF_CL_SpkSmoothPower);

    VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

//  VoC_push32(ACC0,DEFAULT);

    VoC_lw16i(REG6,r_PF_CL_SpkSmoothPower);

    // Smooth Spk Power
    VoC_jal(Coolsand_SmoothPeakPower);

    VoC_lw32_d(ACC0,r_PF_CL_SpkNoise);
    VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

    // Instant Spk SNR
    VoC_jal(Coolsand_InstantSNR);

    //REG3:InstantSpkSNR

//  VoC_pop32(ACC0,DEFAULT);//PrevSpkSmoothPower

    //Spk  Noise Power Est
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);
    VoC_lw16i(REG0,r_PF_CL_SpkSmoothPower);
    VoC_lw16i(REG1,r_PF_CL_SpkNoise);

    VoC_sw16_d(REG7,pInstantSNR_t);
    VoC_sw16_d(REG2,pInstantSNR_f);

    VoC_lw16_d(REG6,m_AF_FrameCnt);
    VoC_lw16i(REG7,r_PF_CL_SpkNoiseFloorDevCnt);
    VoC_lw16_d(REG2,m_AECProcPara_PFCLNoiseUpdateFactor);




    VoC_jal(Coolsand_OverallNoiseEst);


//**********************Mic****************************//

    VoC_pop32(RL7,DEFAULT);

//  VoC_lw32_d(ACC0,r_PF_CL_MicSmoothPower);

    VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

//  VoC_push32(ACC0,DEFAULT);

    VoC_lw16i(REG6,r_PF_CL_MicSmoothPower);

    // Smooth Mic Power
    VoC_jal(Coolsand_SmoothPeakPower);

    VoC_lw32_d(ACC0,r_PF_CL_MicNoise);
    VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

    // Instant Mic SNR
    VoC_jal(Coolsand_InstantSNR);

    //REG3:InstantSpkSNR

//  VoC_pop32(ACC0,DEFAULT);//PrevSpkSmoothPower

    //Spk  Noise Power Est
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);

    VoC_lw16i(REG0,r_PF_CL_MicSmoothPower);
    VoC_lw16i(REG1,r_PF_CL_MicNoise);


    VoC_sw16_d(REG7,pInstantSNR_t);
    VoC_sw16_d(REG2,pInstantSNR_f);
    VoC_lw16_d(REG6,m_AF_FrameCnt);
    VoC_lw16i(REG7,r_PF_CL_MicNoiseFloorDevCnt);
    VoC_lw16_d(REG2,m_AECProcPara_PFCLNoiseUpdateFactor);

    //Mic Noise Power Est
    VoC_jal(Coolsand_OverallNoiseEst);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_ControlLogicChnlDecisionEnhanced
 *
 * Description:
 *
 * Inputs:
 *
 *
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/22/2011
 **************************************************************************************/
void Coolsand_ControlLogicChnlDecisionEnhanced(void)
{

    VoC_lw32_d(REG67,r_PF_CL_MicNoise);

    VoC_push16(RA,DEFAULT);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);

    VoC_lw16_d(REG5,m_AECProcPara_PFCLThrSigDet);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
    VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

    VoC_macX_rr(REG7,REG5,DEFAULT);

    VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

    VoC_shr32_ri(ACC1,-4,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L0,ACC1,ACC0)
    VoC_lw16i_short(REG4,1,DEFAULT);
Coolsand_ControlLogicChnlDecisionEnhanced_L0:

    VoC_lw32_d(REG67,r_PF_CL_SpkNoise);

    VoC_sw16_d(REG4,r_PF_CL_ifMicSigOn);

    VoC_shr32_ri(REG67,-1,DEFAULT);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
    VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

    VoC_macX_rr(REG7,REG5,DEFAULT);
    VoC_lw16i_short(REG0,-1,IN_PARALLEL);//*pState = -1;

    VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

    VoC_lw32_d(RL6,r_PF_CL_MicNoise);

    VoC_shr32_ri(ACC1,-4,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_lw32_d(REG67,r_PF_CL_MicNoise);

    VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L1,ACC1,ACC0)
    VoC_bnlt32_rd(Coolsand_ControlLogicChnlDecisionEnhanced_L1,REG67,r_PF_CL_SpkSmoothPower)
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_NOP();
Coolsand_ControlLogicChnlDecisionEnhanced_L1:

    VoC_sw16_d(REG4,r_PF_CL_ifSpkSigOn);

    // Compare Mic and Spk Power
    VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L2,r_PF_CL_ifSpkSigOn)
    VoC_bez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L2,r_PF_CL_ifMicSigOn)

    //Fix2Double(*pSpkSmoothPower) <Fix2Double(ThrNT*(*pErrSmoothPower))
    VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);

    VoC_lw16_d(REG5,m_AECProcPara_PFCLThrNT);

    VoC_shr32_ri(REG67,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);

    VoC_macX_rr(REG7,REG5,DEFAULT);

    VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

    VoC_shr32_ri(ACC1,-4,DEFAULT);

    VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L2,ACC0,ACC1)
    // Near End Single Talk
    VoC_lw16i_short(REG0,1,DEFAULT);//*pState = 1;
    VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);
Coolsand_ControlLogicChnlDecisionEnhanced_L2:



    VoC_bnez32_d(Coolsand_ControlLogicChnlDecisionEnhanced_L3,r_PF_CL_MicSmoothPower)
    VoC_bnez32_d(Coolsand_ControlLogicChnlDecisionEnhanced_L3,r_PF_CL_SpkSmoothPower)

    // Near End Single Talk
    VoC_lw16i_short(REG0,3,DEFAULT);//*pState = 3;
    VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);
Coolsand_ControlLogicChnlDecisionEnhanced_L3:


    VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L4,r_PF_CL_ifSpkSigOn)
    VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L4,r_PF_CL_ifMicSigOn)

    VoC_lw32_d(ACC0,r_PF_CL_MicSmoothPower);
    VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

    VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L4,ACC0,ACC1)

    // Near End Single Talk
    VoC_lw16i_short(REG0,3,DEFAULT);//*pState = 3;
    VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);
Coolsand_ControlLogicChnlDecisionEnhanced_L4:
    /*
        VoC_lw32_d(REG67,r_PF_CL_SpkSmoothPower);

        VoC_shr32_ri(REG67,-1,DEFAULT);

        VoC_lw16_d(REG5,m_AECProcPara_PFCLThrDT);

        VoC_shru16_ri(REG6,1,DEFAULT);
        VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

        VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
        VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

        VoC_macX_rr(REG7,REG5,DEFAULT);

        VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

        VoC_shr32_ri(ACC1,-4,DEFAULT);
        VoC_lw16i_short(REG0,0,IN_PARALLEL);


        VoC_bgt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L5,ACC1,ACC0)


                VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);
                VoC_lw16_d(REG5,m_AECProcPara_PFCLThrDTDiff);

                VoC_shr32_ri(REG67,-1,DEFAULT);

                VoC_shru16_ri(REG6,1,DEFAULT);
                VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

                VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
                VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

                VoC_macX_rr(REG7,REG5,DEFAULT);

                VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);
                VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);

                VoC_shr32_ri(ACC1,-4,DEFAULT);
                VoC_shr32_ri(REG67,-4,IN_PARALLEL);

                VoC_add32_rr(RL6,REG67,ACC0,DEFAULT);//ErrSmoothPower_db+DiffValue
                VoC_sub32_rr(RL7,REG67,ACC0,IN_PARALLEL);//ErrSmoothPower_db-DiffValue

            VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,RL6,ACC1);
            VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,ACC1,RL7);

                VoC_lw32_d(ACC0,r_PF_CL_SpkSmoothPower);
                VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

            VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,ACC1,ACC0);

    Coolsand_ControlLogicChnlDecisionEnhanced_L5:

            VoC_lw16i_short(REG0,2,DEFAULT);
    Coolsand_ControlLogicChnlDecisionEnhanced_End:*/


    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_End,PFCLDTDOut)
    VoC_lw16i_short(REG0,0,DEFAULT);
Coolsand_ControlLogicChnlDecisionEnhanced_End:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG0,r_PF_CL_ChnlState);
    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_SmoothPeakPower
 *
 * Description:
 *
 * Inputs:  REG6: pSmoothPower
 *          REG7: *pSmoothFactor
 *          RL7:*pInstantPower
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_SmoothPeakPower(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);

    VoC_lw32_p(ACC1,REG6,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0x40,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_SmoothPeakPower_L0,RL7,ACC1)
    VoC_bngt32_rr(Coolsand_SmoothPeakPower_L0,ACC0,RL7)

    VoC_movreg32(ACC0,RL7,DEFAULT);

    VoC_jump(Coolsand_SmoothPeakPower_L1);

Coolsand_SmoothPeakPower_L0:

    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,11,DEFAULT);
    VoC_lw16i_short(FORMATX,11-15,IN_PARALLEL);

    VoC_jal(Coolsand_FirstOrderIIRSmooth2);

Coolsand_SmoothPeakPower_L1:

    VoC_bnltz32_r(Coolsand_SmoothPeakPower_L2,ACC0)
    VoC_lw32z(ACC0,DEFAULT);
Coolsand_SmoothPeakPower_L2:

    VoC_pop16(RA,DEFAULT);
    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_InstantSNR
 *
 * Description:
 *
 * Inputs:  ACC0:pNoisePower
 *          ACC1:pSigPower
 *
 *
 * Outputs:REG3
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_InstantSNR(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_shr32_ri(ACC0,4,IN_PARALLEL);

    VoC_bnez32_r(Coolsand_InstantSNR_L0,ACC0)
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
Coolsand_InstantSNR_L0:
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG2,20,IN_PARALLEL);
    //RL6:NoisePower_r.d
    //REG2:oisePower_r.t

    //REG3:NoisePowerFirstBitPos
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(REG7,-1,DEFAULT);
//  VoC_lw16i_short(REG2,0x1,IN_PARALLEL);

    //  int InstantSNRShiftVal = (NoisePower_r.f+1-NoisePowerFirstBitPos - pSigPower->f+pInstantSNR->f);
    VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);
//  VoC_lw16i_short(REG7,-7,IN_PARALLEL);

    VoC_shr32_rr(ACC1,REG3,DEFAULT);
//  VoC_shr16_rr(REG2,REG7,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-24,DEFAULT);

    VoC_shr32_ri(ACC1,24,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_movreg16(REG3,ACC1_LO,DEFAULT);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_OverallNoiseEst
 *
 * Description:
 *
 * Inputs:  REG0:   pSigPower
 *          REG1:   pNoisePower
 *          REG2:   NoiseUpdateFactor
 *          REG3:   * pInstantSNR
 *          REG6:   FrameCnt
 *          REG7:   pNoiseFloorDevCnt
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_OverallNoiseEst(void)
{

    VoC_lw32_p(ACC0,REG1,DEFAULT);
    VoC_lw16i_short(REG5,16,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG4,16,IN_PARALLEL);

    VoC_push16(REG3,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_jal(Coolsand_NoisePSDEst);

    VoC_pop16(REG7,DEFAULT);//* pInstantSNR
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_pop16(REG3,DEFAULT);//pNoiseFloorDevCnt
    VoC_pop32(ACC0,IN_PARALLEL);//PrevNoisePower->d

    VoC_lw32_p(ACC1,REG0,DEFAULT);//pSigPower->d

    //if (pSigPower->d!=0)
    VoC_bez32_r(Coolsand_OverallNoiseEst_L2,ACC1)

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw32_p(ACC1,REG1,IN_PARALLEL);//pSigPower->d


    VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    VoC_bne32_rr(Coolsand_OverallNoiseEst_L0,ACC1,ACC0);
    VoC_bngt16_rr(Coolsand_OverallNoiseEst_L0,REG3,REG4);
    VoC_add16_rp(REG6,REG5,REG7,DEFAULT);       //*pNoiseFloorDevCnt = (*pNoiseFloorDevCnt) + 1;
Coolsand_OverallNoiseEst_L0:

    VoC_lw16i(REG4,500*8);
    VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower->d
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_bngt16_rr(Coolsand_OverallNoiseEst_L1,REG6,REG4);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);

Coolsand_OverallNoiseEst_L1:
    VoC_sw16_p(REG6,REG7,DEFAULT);//save pNoiseFloorDevCnt

Coolsand_OverallNoiseEst_L2:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_OverallNoiseEstwState
 *
 * Description:
 *
 * Inputs:  REG0:   pSigPower
 *          REG1:   pNoisePower
 *          REG2:   NoiseUpdateFactor
 *          REG3:   * pInstantSNR
 *          REG6:   FrameCnt
 *          REG7:   pNoiseFloorDevCnt
 *
 * Outputs:no
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_OverallNoiseEstwState(void)
{
    VoC_lw16_d(REG7,g_ch);

    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i(REG0,r_PF_SmoothErrPower);
    VoC_lw16i(REG1,r_PF_NoisePowerEstBuf);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_lw16i_short(REG4,3,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);

    VoC_lw32_p(ACC0,REG1,DEFAULT);

    VoC_push32(REG01,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_push32(ACC0,DEFAULT);

    VoC_bez16_d(Coolsand_OverallNoiseEstwState_L0_subL0,m_AECProcPara_StrongEchoFlag)
    VoC_bne16_rd(Coolsand_OverallNoiseEstwState_L0,REG4,r_PF_CL_State)

Coolsand_OverallNoiseEstwState_L0_subL0:


    //Spk  Noise Power Est



    VoC_lw16i_short(REG6,12,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,16,IN_PARALLEL);

    VoC_sw16_d(REG6,pInstantSNR_t);
    VoC_sw16_d(REG2,pInstantSNR_f);

    VoC_lw16_d(REG6,m_AF_FrameCnt);

    VoC_lw16_d(REG2,m_AECProcPara_NoiseUpdateFactor);

    VoC_jal(Coolsand_NoisePSDEst);

Coolsand_OverallNoiseEstwState_L0:


    VoC_pop16(REG3,DEFAULT);//* pInstantSNR
    VoC_pop32(ACC0,IN_PARALLEL);//PrevNoisePower->d

    VoC_pop32(REG01,DEFAULT);//pSigPower&pNoisePower
    VoC_pop16(REG4,DEFAULT);//g_ch

    VoC_lw16i(REG0,r_PF_NoisePowerSmoothEstBuf);//r_PF_NoisePowerSmoothEstBuf

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_lw32_p(ACC1,REG0,IN_PARALLEL);//pSigPower->d

    VoC_shr16_ri(REG4,1,DEFAULT);

    VoC_lw16i(REG7,r_PF_NoiseFloorDevCnt);//pNoiseFloorDevCnt

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    //if (pSigPower->d!=0)
    VoC_bez32_r(Coolsand_OverallNoiseEstwState_L3,ACC1)

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw32_p(ACC1,REG1,IN_PARALLEL);//pNoisePower


    VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
    VoC_lw16i_short(REG4,16,IN_PARALLEL);

    VoC_bne32_rr(Coolsand_OverallNoiseEstwState_L1,ACC1,ACC0);
    VoC_bngt16_rr(Coolsand_OverallNoiseEstwState_L1,REG3,REG4);
    VoC_add16_rp(REG6,REG5,REG7,DEFAULT);       //*pNoiseFloorDevCnt = (*pNoiseFloorDevCnt) + 1;
Coolsand_OverallNoiseEstwState_L1:

    VoC_lw16i(REG4,500*16);
    VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower->d
    VoC_shr32_ri(ACC1,-1,DEFAULT);//            pNoisePower->d = pNoisePower->d*2;
    VoC_bngt16_rr(Coolsand_OverallNoiseEstwState_L2,REG6,REG4);

    VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
    VoC_sw32_p(ACC1,REG1,IN_PARALLEL);

    VoC_sw32_p(ACC1,REG0,DEFAULT);// *pNoiseSmoothPower = *pNoisePower;

Coolsand_OverallNoiseEstwState_L2:
    VoC_sw16_p(REG6,REG7,DEFAULT);//save pNoiseFloorDevCnt

Coolsand_OverallNoiseEstwState_L3:

    //  if (FrameCnt<50)
    VoC_lw16i(REG4,50);
    VoC_lw32_p(ACC0,REG1,DEFAULT);
    VoC_bngt16_rd(Coolsand_OverallNoiseEstwState_L4,REG4,m_AF_FrameCnt)
    VoC_sw32_p(ACC0,REG0,DEFAULT);// *pNoiseSmoothPower = *pNoisePower;
Coolsand_OverallNoiseEstwState_L4:

    //if (pNoiseSmoothPower->d == 0)
    VoC_lw32_p(ACC0,REG0,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bnez32_r(Coolsand_OverallNoiseEstwState_L5,ACC0)
    VoC_sw32_p(REG45,REG0,DEFAULT);
Coolsand_OverallNoiseEstwState_L5:

    VoC_lw32_p(RL7,REG0,DEFAULT);//pNoiseSmoothPower
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower
    VoC_movreg32(REG67,RL7,IN_PARALLEL);//pNoiseSmoothPower

    VoC_bez16_d(Coolsand_OverallNoiseEstwState_L7,m_AECProcPara_StrongEchoFlag)

    VoC_bne16_rd(Coolsand_OverallNoiseEstwState_End,REG4,r_PF_CL_State)

Coolsand_OverallNoiseEstwState_L7:
    VoC_shr32_ri(REG67,-1,DEFAULT);
    VoC_bgt32_rr(Coolsand_OverallNoiseEstwState_L6,ACC1,REG67);
    VoC_movreg32(RL7,ACC1,DEFAULT);
    VoC_jump(Coolsand_OverallNoiseEstwState_End);
Coolsand_OverallNoiseEstwState_L6:

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(REG5,50,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_macX_rr(REG7,REG5,DEFAULT);
    VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

    VoC_NOP();

    VoC_bgt32_rr(Coolsand_OverallNoiseEstwState_End,ACC0,ACC1);
    VoC_movreg32(RL7,ACC1,DEFAULT);
Coolsand_OverallNoiseEstwState_End:

    VoC_pop16(RA,DEFAULT);
    VoC_sw32_p(RL7,REG0,DEFAULT);
    VoC_return;
}


/**************************************************************************************
 * Function:    Cooland_InstantPower
 *
 * Description:
 *
 * Inputs:
 *   REG0: *pDataI
 *   REG1: *pDataQ
 *
 * Outputs:
 *   ACC0: *pPower
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Cooland_InstantPower(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_jal(Coolsand_CmplxAppxNorm);

    VoC_shr32_ri(ACC0,-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_movreg16(REG0,ACC0_HI,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);//DataPower = DataNorm*DataNorm;

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL
 *
 * Description:
 *
 * Inputs:
 *
 *
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG7,g_ch);

    VoC_lw16i(REG2,g_AF_ErrDataBufI);
    VoC_lw16i(REG3,g_AF_ErrDataBufQ);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);

    VoC_lw16_p(REG0,REG2,DEFAULT);
    VoC_lw16_p(REG4,REG2,IN_PARALLEL);//save pErrDataI

    VoC_lw16_p(REG1,REG3,DEFAULT);
    VoC_lw16_p(REG5,REG3,IN_PARALLEL);//save pErrDataQ

    // Compute Instant Err Power
    VoC_jal(Coolsand_CmplxAppxNorm);

    //REG4:pErrDataI
    //REG5:pErrDataQ

    VoC_shr32_ri(ACC0,-16,DEFAULT);

    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG6,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,-8,DEFAULT);

    VoC_shr32_ri(ACC0,8,DEFAULT);

    VoC_NOP();

    VoC_push32(ACC0,DEFAULT);

    // Echo Power Estimation
    VoC_jal(Coolsand_EchoPSDEst_MFCOHFFix);

    //ACC0:pEchoPowerEst
    // Compute Echo Power Reverse

    VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);
    VoC_lw16_d(REG7,g_ch);
    VoC_lw16i(REG1,g_PF_EchoWeight);

    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);

    VoC_pop32(RL7,DEFAULT);

    VoC_lw32_p(REG23,REG0,DEFAULT);

    VoC_shr32_ri(REG23,2,DEFAULT);

    VoC_lw16i_short(REG4,22,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_jal(Coolsand_ShrinkBit);
    //ACC0:EchoPowerEst_r.d
    //REG3:FirstBitPos

    VoC_lw16i_short(REG4,26,DEFAULT);//FirstBitPos-1-21-4(for high 16 bit).

    VoC_bnez32_r(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L0,ACC0)

    VoC_lw16i_short(REG4,25,DEFAULT);//FirstBitPos-1-21-4(for high 16 bit).

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L0:


    VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    // Update Post SNR
    VoC_shr32_rr(REG67,REG4,DEFAULT);
    VoC_movreg32(RL6,REG67,IN_PARALLEL);

    VoC_shr32_ri(REG67,16,DEFAULT);  //*pPostSToNRatio
    VoC_push16(REG4,IN_PARALLEL);//EchoPowerEstRev

    VoC_shr32_ri(ACC0,-2,DEFAULT);
    VoC_shr32_ri(REG67,-2,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL6,ACC0,DEFAULT);

    VoC_bgtz32_r(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L1,RL7)
    VoC_lw16i_short(RL7_LO,1,DEFAULT);
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L1:

    VoC_lw16_d(REG2,g_ch);

    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shr32_ri(RL7,4,IN_PARALLEL);

    VoC_lw16i(REG3,r_PF_PrevSigPowerEBuf);

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_shr32_ri(RL7,-12,IN_PARALLEL);

    VoC_lw16_d(REG1,m_AECProcPara_SERUpdateFactor);

    VoC_lw32_p(ACC1,REG3,DEFAULT);
    VoC_shr32_ri(RL7,12,IN_PARALLEL);

//  VoC_jal(Coolsand_FirstOrderIIRSmooth2);
    VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

    VoC_shr32_ri(REG45,-12,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG45,11,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_macX_rr(REG5,REG1,DEFAULT);


    //ACC0:
    VoC_pop16(REG4,DEFAULT);//EchoPowerEstRev

    VoC_lw16i(REG0,g_PF_PriorSToERatio);

    VoC_add16_rd(REG0,REG0,g_ch);

    VoC_shr32_ri(ACC0,-12,DEFAULT);

    VoC_shr32_ri(ACC0,12,DEFAULT);

    VoC_shr32_rr(ACC0,REG4,DEFAULT);
    VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

    VoC_lw16i(RL7_LO,0x8000);

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_lw16i_short(REG2,16,IN_PARALLEL);

    VoC_lw16i(RL7_LO,128);

    VoC_sw16_p(ACC0_LO,REG0,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);
// VoC_directive: PARSER_OFF

//  printf("g_PF_PriorSToERatio :0x%x\n",REGL[0]);


// VoC_directive: PARSER_ON

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//pPriorSToERatio->d+(0x1<<pPriorSToERatio->f)

    VoC_jal(Coolsand_MSBPos);
    //REG3:PriorSERFirstBitPos

    VoC_lw16i_short(REG0,14,DEFAULT);

    VoC_sub16_rr(REG3,REG3,REG0,DEFAULT);

    VoC_lw16_d(REG1,m_AECProcPara_EchoOverEstFactor);

    VoC_shr16_rr(REG1,REG3,DEFAULT);

    VoC_shr16_ri(REG1,-8,DEFAULT);

    VoC_shr16_ri(REG1,8,DEFAULT);

    VoC_lw16i(REG2,128);

    VoC_sub16_rr(REG4,REG2,REG1,DEFAULT);

    VoC_lw16i(REG1,g_PF_EchoWeight);

    VoC_add16_rd(REG1,REG1,g_ch);

    VoC_bnez16_d(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L3,m_AECProcPara_StrongEchoFlag)

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_bgt16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L2,REG4,m_AECProcPara_EchoGainLimit)
    VoC_lw16_d(REG4,m_AECProcPara_EchoGainLimit);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L2:
    VoC_shr16_ri(REG4,2,DEFAULT);
    //  // Limit the weight
    //  *pEchoWeight = FixMax(*pEchoWeight, EchoGainLimit);

    VoC_bez16_d(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End,r_PF_CL_State)
    VoC_lw16i(REG4,127);
    VoC_jump(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End);

Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L3:

    //  // Limit the weight
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);//      AmpLimit = 1;

    VoC_lw16i(REG5,m_AECProcPara_pPFEchoGainLimitAmp);

    VoC_add16_rd(REG5,REG5,g_ch);

    VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L4,REG2,r_PF_CL_State)

    VoC_lw16_p(REG3,REG5,DEFAULT);//        AmpLimit = pEchoGainLimitAmp[Channel];

Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L4:

    VoC_mult16_rd(REG6,REG3,m_AECProcPara_EchoGainLimit);

    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_lw16i_short(REG7,0x2,IN_PARALLEL);

    VoC_bgt16_rr(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L5,REG4,REG6)
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//+0.5
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L5:

    VoC_shr16_ri(REG4,2,DEFAULT);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,3,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L6,REG2,r_PF_CL_State)
    VoC_lw16i(REG4,127);
    VoC_jump(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L6:
    VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End,REG3,r_PF_CL_State)
    VoC_lw16i(REG4,127);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG4,REG1,DEFAULT);
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_EchoPSDEst_MFCOHFFix
 *
 * Description:
 *
 * Inputs:  REG4:
 *          REG5:
 *          ACC0:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_EchoPSDEst_MFCOHFFix(void)
{
    VoC_push16(RA,DEFAULT);
//  VoC_push32(ACC0,IN_PARALLEL);//*pInstantErrPower

    VoC_lw16_d(REG2,g_ch);

    VoC_push32(REG45,DEFAULT);//save
    VoC_shr16_ri(REG2,-3,IN_PARALLEL);//FILTER_LEN

    VoC_lw16i(REG6,r_AF_SpkSigBufI);
    VoC_lw16i(REG7,r_AF_SpkSigBufQ);

    VoC_add16_rr(REG6,REG6,REG2,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);

    VoC_lw16_p(REG0,REG6,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16_p(REG1,REG7,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

//  VoC_shr16_ri(REG0,5,DEFAULT);
//  VoC_shr16_ri(REG1,5,IN_PARALLEL);

    // Spk Power
    VoC_jal(Coolsand_CmplxAppxNorm);

    //REG6:pSpkSigBufI
    //REG7:pSpkSigBufQ

    //ACC0:SpkDataNorm

    VoC_movreg16(REG1,ACC0_LO,DEFAULT);
    VoC_lw16i_short(FORMAT32,3,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG1,REG1,DEFAULT);

    VoC_lw16i(REG0,r_PF_SpkPowerBuf);

    VoC_lw16_d(REG1,g_ch);

    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-10,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_shr32_ri(ACC0,10,IN_PARALLEL);

    VoC_lw16_d(REG1,m_AECProcPara_EchoUpdateFactor);

    VoC_lw32_p(ACC1,REG0,DEFAULT);//r_PF_SpkPowerBuf

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_movreg32(REG23,REG67,IN_PARALLEL);//move

//  VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

    VoC_shr32_ri(REG45,-10,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG45,9,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_mac32_rr(REG4,REG1,DEFAULT);

    VoC_macX_rr(REG5,REG1,DEFAULT);

    VoC_NOP();

    //ACC0:pSpkPower

    VoC_shr32_ri(ACC0,-10,DEFAULT);
    VoC_lw16i_short(FORMAT32,5,IN_PARALLEL);


    VoC_lw32z(RL7,DEFAULT);//
    VoC_shr32_ri(ACC0,10,IN_PARALLEL);

    // Correlation between spk and err data

    //REG2:pSpkSigBufI
    //REG3:pSpkSigBufQ

    VoC_pop32(REG45,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);//save *pSpkPower

    VoC_sw32_p(ACC0,REG0,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    //REG4:*pErrDataI
    //REG5:*pErrDataQ

    //  r_PF_CCErrSpkVecBufI
    //  r_PF_CCErrSpkVecBufQ

    VoC_lw16_d(REG7,g_ch);

    VoC_sub16_rr(REG5,REG0,REG5,DEFAULT);
    VoC_shr16_ri(REG7,-4,IN_PARALLEL);//ch + FILTER_LEN -3+short to int -1

//  VoC_lw16i_short(FORMAT16,7-16,DEFAULT);

    VoC_lw16i_set_inc(REG0,r_PF_CCErrSpkVecBufI,2);
    VoC_lw16i_set_inc(REG1,r_PF_CCErrSpkVecBufQ,2);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_lw16_d(REG7,m_AECProcPara_EchoUpdateFactor);


    //  for (i=0; i<FILTER_LEN; i++)
    VoC_loop_i(0,FILTER_LEN);

    // Corr with each Err and SpkBufData

    //  VoC_jal(Coolsand_CmplxMul);
    VoC_multf32_rp(ACC0,REG4,REG2,DEFAULT);

    VoC_multf32_rp(ACC1,REG5,REG3,DEFAULT);

    VoC_shr32_ri(ACC0,-10,DEFAULT);
    VoC_shr32_ri(ACC1,-10,DEFAULT);

    VoC_shr32_ri(ACC0,10,DEFAULT);
    VoC_shr32_ri(ACC1,10,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);
    VoC_sub32_rr(RL6,ACC0,ACC1,IN_PARALLEL);//InstantCorrI

    VoC_multf32inc_rp(ACC1,REG5,REG2,DEFAULT);

    VoC_shr32_ri(ACC0,-10,DEFAULT);

    VoC_shr32_ri(ACC1,-10,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_shr32_ri(ACC0,10,DEFAULT);
    VoC_shr32_ri(ACC1,10,IN_PARALLEL);

    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);//InstantCorrQ
    VoC_push32(REG23,IN_PARALLEL);

    VoC_shr32_ri(RL6,-10,DEFAULT);
    VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

    VoC_shr32_ri(RL6,10,DEFAULT);
    VoC_shr32_ri(ACC1,10,IN_PARALLEL);

    // Smoothing
    //  VoC_jal(Coolsand_FirstOrderIIRSmooth2);
    //  VoC_jal(Coolsand_FirstOrderIIRSmooth2);

//      VoC_movreg16(REG2,RL6_LO,DEFAULT);//InstantCorrI
//      VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);//InstantCorrQ

    VoC_sub32_rp(REG23,RL6,REG0,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_sub32_rp(REG45,ACC1,REG1,DEFAULT);
    VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

    VoC_shr32_ri(REG23,-10,DEFAULT);
    VoC_shr32_ri(REG45,-10,IN_PARALLEL);

    VoC_shr32_ri(REG23,9,DEFAULT);
    VoC_shr32_ri(REG45,9,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG2,REG7,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG7,DEFAULT);
    VoC_macX_rr(REG5,REG7,IN_PARALLEL);

    VoC_NOP();

    VoC_shr32_ri(ACC0,-10,DEFAULT);
    VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

    VoC_shr32_ri(ACC0,10,DEFAULT);
    VoC_shr32_ri(ACC1,10,IN_PARALLEL);

    VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);

    VoC_shr32_ri(ACC0,-10,DEFAULT);
    VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

    VoC_shr32_ri(ACC0,10,DEFAULT);
    VoC_shr32_ri(ACC1,10,IN_PARALLEL);

    VoC_NOP();

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_shr32_ri(ACC0,5,IN_PARALLEL);

    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_shr32_ri(ACC1,5,IN_PARALLEL);

    // Corr Norm
    //  VoC_jal(Coolsand_CmplxAppxNorm_int);

    VoC_shr32_ri(ACC0,-17,DEFAULT);
    VoC_shr32_ri(ACC1,-17,IN_PARALLEL);

    VoC_shr32_ri(ACC0,17,DEFAULT);
    VoC_shr32_ri(ACC1,17,IN_PARALLEL);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,31,IN_PARALLEL);

    VoC_xor32_rr(REG45,ACC0,DEFAULT);

    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_shr32_ri(ACC1,31,IN_PARALLEL);

    VoC_xor32_rr(RL6,ACC1,DEFAULT);
    VoC_sub32_rr(ACC0,REG45,ACC0,IN_PARALLEL);

    VoC_sub32_rr(ACC1,RL6,ACC1,DEFAULT);

    VoC_shr32_ri(ACC0,-17,DEFAULT);
    VoC_shr32_ri(ACC1,-17,IN_PARALLEL);

    VoC_shr32_ri(ACC0,17,DEFAULT);
    VoC_shr32_ri(ACC1,17,IN_PARALLEL);


    VoC_bgt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L0,ACC0,ACC1)
    VoC_movreg32(ACC0,ACC1,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);//swap
Coolsand_EchoPSDEst_MFCOHFFix_L0:

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-5,DEFAULT);//MaxVal*32
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

    VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);//MaxVal*31 == MaxVal*32 - MaxVal
    VoC_add32_rr(ACC1,ACC1,REG45,IN_PARALLEL);//MinVal*3 ==MinVal*2+MinVal

    VoC_shr32_ri(ACC0,5,DEFAULT); //(MaxVal*31)>>5
    VoC_shr32_ri(ACC1,3,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_shr32_ri(ACC0,-17,DEFAULT);

    VoC_shr32_ri(ACC0,17,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_movreg16(REG6,ACC0_LO,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);
    // Sum Norm2
//      InstantNorm2 = RErrSpkNorm*RErrSpkNorm;
//      SumCorrNorm2 = SumCorrNorm2+InstantNorm2;

    VoC_mac32_rr(REG6,REG6,DEFAULT);

    VoC_pop32(REG23,DEFAULT);
    VoC_lw16i_short(FORMAT32,5,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);

    VoC_endloop0


    //RL7:SumCorrNorm2

    VoC_lw16i(REG0,r_PF_PrevEchoPowerEst);
    VoC_lw16i(REG1,r_PF_PrevSpkPower);
    VoC_lw16i(REG2,r_PF_SpkPowerBuf);


    VoC_lw16_d(REG4,g_ch);

    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_shr16_ri(REG4,-1,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);

    VoC_lw32_p(RL6,REG0,DEFAULT);
    VoC_lw32_p(ACC0,REG1,DEFAULT);
    VoC_lw32_p(ACC1,REG2,DEFAULT);

    VoC_lw16_d(REG4,r_PF_CL_State);


    VoC_bne16_rr(Coolsand_EchoPSDEst_MFCOHFFix_L4,REG3,REG4)
    VoC_bez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L4,RL6)

    //VoC_jal(Coolsand_InstantSNR);

    VoC_push32(RL7,DEFAULT);//SumCorrNorm2Val

    VoC_shr32_ri(ACC0,4,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L1,ACC0)
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
Coolsand_EchoPSDEst_MFCOHFFix_L1:
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG2,18,IN_PARALLEL);
    //RL6:NoisePower_r.d
    //REG2:oisePower_r.t

    //REG3:NoisePowerFirstBitPos
    VoC_jal(Coolsand_MSBPos);

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    //  int InstantSNRShiftVal = (NoisePower_r.f+1-NoisePowerFirstBitPos - pSigPower->f+pInstantSNR->f);
    VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);
    VoC_lw16i_short(ACC0_LO,1,IN_PARALLEL);

    VoC_shr32_rr(ACC1,REG3,DEFAULT);
    VoC_shr32_ri(ACC0,-11,IN_PARALLEL);//2048

    VoC_shr32_ri(ACC1,-9,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,9,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);//*pPrevEchoPowerEst

    VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L2,ACC1,ACC0)
    VoC_lw16i_short(REG2,8,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L2:

    VoC_lw16i_short(REG4,5,DEFAULT);

    VoC_shr32_rr(ACC1,REG2,DEFAULT);
    VoC_shr32_ri(REG01,6,IN_PARALLEL);

    VoC_shr32_ri(REG01,-16,DEFAULT);
    VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

    VoC_movreg16(REG3,ACC1_HI,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG2,IN_PARALLEL);

    VoC_multf32_rr(REG01,REG1,REG3,DEFAULT);
    VoC_movreg16(FORMAT32,REG4,IN_PARALLEL);

    VoC_lw16_d(REG3,m_AECProcPara_PFDTEchoOverEstFactor)

    VoC_shr32_ri(REG01,-8,DEFAULT);
    VoC_lw16i_short(FORMAT32,1,IN_PARALLEL);

    VoC_shr32_ri(REG01,7,DEFAULT);
    VoC_lw16i_short(FORMATX,1-15,IN_PARALLEL);

    VoC_shru16_ri(REG0,1,DEFAULT);

    VoC_multf32_rr(ACC0,REG0,REG3,DEFAULT);

    VoC_macX_rr(REG1,REG3,DEFAULT);
    // Residual Echo Power
    VoC_pop32(RL7,IN_PARALLEL);//SumCorrNorm2Val


    VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);

    VoC_lw16_d(REG4,g_ch);

    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_shr16_ri(REG4,-1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,IN_PARALLEL);

    VoC_movreg32(ACC1,ACC0,DEFAULT);

    VoC_lw32_p(RL6,REG0,DEFAULT);//pEchoPowerEst->d
    VoC_shr32_ri(ACC1,-2,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L3,RL6,ACC1)
    VoC_shr32_ri(RL6,2,DEFAULT);
    VoC_movreg32(ACC0,RL6,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L3:


    VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L7,ACC0)
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

    VoC_jump(Coolsand_EchoPSDEst_MFCOHFFix_L7);

Coolsand_EchoPSDEst_MFCOHFFix_L4:


//  VoC_pop32(RL6,DEFAULT);
    VoC_movreg32(RL6,ACC1,DEFAULT);
    //RL6:pSpkPower->d

    VoC_lw16i_short(ACC1_LO,4,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L5,RL6,ACC1);
    VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L5,RL7);
    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);
Coolsand_EchoPSDEst_MFCOHFFix_L5:


    VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L6,RL6);
    VoC_bez16_d(Coolsand_EchoPSDEst_MFCOHFFix_L6,r_PF_CL_State);
    VoC_lw32z(RL7,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L6:

    VoC_lw16i_short(REG2,22,DEFAULT);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);//pSpkPower->d

    VoC_jal(Coolsand_MSBPos);

    //REG3:SpkPowerFirstBitPos
    //RL7:SumCorrNorm2
    //ACC1:pSpkPower->d

    VoC_lw16i_short(REG4,11,DEFAULT);
//  VoC_pop32(ACC0,IN_PARALLEL);//r_PF_EchoPowerEstBuf

    VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);
    VoC_lw16_d(REG7,g_ch);

    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_shr32_rr(RL7,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);

    VoC_shr32_ri(RL7,-8,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

    VoC_shr32_ri(RL7,8,DEFAULT);
    VoC_lw16i_short(ACC0_LO,1,IN_PARALLEL);

    VoC_bez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L7,RL7)

    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_movreg32(ACC1,RL7,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-2,DEFAULT);
    VoC_lw32_p(RL6,REG0,IN_PARALLEL);//pEchoPowerEst->d

    VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L7,RL6,ACC1)
    VoC_shr32_ri(RL6,2,DEFAULT);
    VoC_movreg32(ACC0,RL6,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L7:


    // Record Prev

    VoC_lw16_d(REG7,g_ch);

    VoC_sw32_p(ACC0,REG0,DEFAULT);//pEchoPowerEst
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);

    VoC_lw16i(REG1,r_PF_SpkPowerBuf);
    VoC_lw16i(REG2,r_PF_PrevSpkPower);
    VoC_lw16i(REG3,r_PF_PrevEchoPowerEst);

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);

    VoC_lw32_d(RL6,m_AECProcPara_PFPrevEchoEstLimit);

    VoC_lw32_p(ACC1,REG1,DEFAULT);
    VoC_shr32_ri(RL6,-4,IN_PARALLEL);


    VoC_bnez16_d(Coolsand_EchoPSDEst_MFCOHFFix_End,r_PF_CL_State);
    VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_End,ACC1,RL6);
    VoC_bngt32_rd(Coolsand_EchoPSDEst_MFCOHFFix_End,ACC0,m_AECProcPara_PFPrevEchoEstLimit);

    VoC_sw32_p(ACC1,REG2,DEFAULT);//        *pPrevSpkPower = *pSpkPower;
    VoC_sw32_p(ACC0,REG3,DEFAULT);

Coolsand_EchoPSDEst_MFCOHFFix_End:

//  r_PF_CL_State//State

//  r_PF_SpkPowerBuf//pSpkPower

//  r_PF_EchoPowerEstBuf//pEchoPowerEst

//  m_AECProcPara_PFPrevEchoEstLimit//pPrevEchoEstLimit


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_ControlLogicDTD
 *
 * Description:
 *
 * Inputs:  :
 *          :
 *          :
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/24/2011
 **************************************************************************************/
void Coolsand_ControlLogicDTD(void)
{
    VoC_push16(RA,DEFAULT);

    // Instant Mic Power
    //  InstantPower(pMicDataI, pMicDataQ, &InstantMicPower);

    VoC_lw16i(REG0,g_MicAnaFilterOutBufI);
    VoC_lw16i(REG1,g_MicAnaFilterOutBufQ);

    VoC_add16_rd(REG0,REG0,CLChnl);
    VoC_add16_rd(REG1,REG1,CLChnl);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);

    VoC_jal(Cooland_InstantPower);

    VoC_movreg32(RL7,ACC0,DEFAULT);

    VoC_lw16i_short(FORMAT32,7,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);


    VoC_lw16i(REG1,r_PF_CL_DTDMicSmoothPower);

    VoC_add16_rd(REG1,REG1,g_DTD_ch);

    VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

    VoC_lw32_p(ACC0,REG1,DEFAULT);



    // Smooth Mic Power
    //  *pMicPower = FirstOrderIIRSmooth2(pMicPower, &InstantMicPower, pUpdateFactor);
    VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    //  if (pMicPower->d == 0)
    //      pMicPower->d = 1;

    VoC_bnez32_r(Coolsand_ControlLogicDTD_L0,ACC0)

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L0:
    // Instant Err Power
    //  InstantPower(pErrDataI, pErrDataQ, &InstantErrPower);


    VoC_lw16i(REG0,g_AF_ErrDataBufI);
    VoC_lw16i(REG1,g_AF_ErrDataBufQ);

    VoC_add16_rd(REG0,REG0,CLChnl);
    VoC_add16_rd(REG1,REG1,CLChnl);

    VoC_lw16_p(REG0,REG0,DEFAULT);
    VoC_lw16_p(REG1,REG1,DEFAULT);


    VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_NOP();

    VoC_sw32_p(ACC0,REG2,DEFAULT);




    VoC_jal(Cooland_InstantPower);


    // Smooth Err Power
    //  *pErrPower = FirstOrderIIRSmooth2(pErrPower, &InstantErrPower, pUpdateFactor);

    VoC_movreg32(RL7,ACC0,DEFAULT);

    VoC_lw16i_short(FORMAT32,7,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);


    VoC_lw16i(REG1,r_PF_CL_DTDErrSmoothPower);

    VoC_add16_rd(REG1,REG1,g_DTD_ch);

    VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

    VoC_lw32_p(ACC0,REG1,DEFAULT);


    VoC_jal(Coolsand_FirstOrderIIRSmooth2);
    //  if (pErrPower->d == 0)
    //      pErrPower->d = 1;
    VoC_bnez32_r(Coolsand_ControlLogicDTD_L1,ACC0)

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L1:

    // Instant Echo Power
    //  InstantPower(&EchoDataI, &ConjEchoDataQ, &InstantEchoPower);

    VoC_lw16i(REG0,g_MicAnaFilterOutBufI);
    VoC_lw16i(REG1,g_MicAnaFilterOutBufQ);

    VoC_add16_rd(REG0,REG0,CLChnl);
    VoC_add16_rd(REG1,REG1,CLChnl);

    VoC_lw16_p(REG0,REG0,DEFAULT);

    VoC_lw16_p(REG1,REG1,DEFAULT);


    VoC_lw16i(REG2,g_AF_ErrDataBufI);
    VoC_lw16i(REG3,g_AF_ErrDataBufQ);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_lw16_p(REG2,REG2,DEFAULT);

    VoC_lw16_p(REG3,REG3,DEFAULT);

    VoC_sub16_rr(REG0,REG0,REG2,DEFAULT);
    VoC_sub16_rr(REG1,REG3,REG1,IN_PARALLEL);


    VoC_lw16i(REG2,r_PF_CL_DTDErrSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_push32(REG01,DEFAULT);//EchoDataI and ConjEchoDataQ

    VoC_sw32_p(ACC0,REG2,DEFAULT);


    VoC_jal(Cooland_InstantPower);


    // Smooth Echo Power
    //  *pEchoPower = FirstOrderIIRSmooth2(pEchoPower, &InstantEchoPower, pUpdateFactor);

    VoC_movreg32(RL7,ACC0,DEFAULT);

    VoC_lw16i_short(FORMAT32,7,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_lw16i(REG1,r_PF_CL_DTDEchoSmoothPower);

    VoC_add16_rd(REG1,REG1,g_DTD_ch);

    VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

    VoC_lw32_p(ACC0,REG1,DEFAULT);

    VoC_jal(Coolsand_FirstOrderIIRSmooth2);

    //  if (pEchoPower->d == 0)
    //      pEchoPower->d = 1;
    VoC_bnez32_r(Coolsand_ControlLogicDTD_L2,ACC0)

    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L2:

    // Instant Mic SNR
    //  InstantSNR(pMicNoise, pMicPower, &InstantMicSNR);


    VoC_lw16i(REG2,r_PF_CL_DTDEchoSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_NOP();

    VoC_sw32_p(ACC0,REG2,DEFAULT);



    VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_NOP();

    VoC_lw32_p(ACC1,REG2,DEFAULT);



    VoC_lw16i(REG2,r_PF_CL_DTDMicNoisePower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_NOP();

    VoC_lw32_p(ACC0,REG2,DEFAULT);


    VoC_jal(Coolsand_InstantSNR);

    //Mic Noise Power Est
    //  OverallNoiseEst(    pMicNoise,
    //pMicPower,
    //&PrevMicPower,
    //&InstantMicSNR,
    //*pUpdateFactor,
    //pMicNoiseFloorDevCnt,
    //FrameCnt);
    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_lw16i_short(REG2,7,IN_PARALLEL);
    VoC_lw16i(REG0,r_PF_CL_DTDMicSmoothPower);
    VoC_lw16i(REG1,r_PF_CL_DTDMicNoisePower);
    VoC_add16_rd(REG0,REG0,g_DTD_ch);
    VoC_add16_rd(REG1,REG1,g_DTD_ch);

    VoC_sw16_d(REG7,pInstantSNR_t);
    VoC_sw16_d(REG2,pInstantSNR_f);

    VoC_lw16_d(REG6,m_AF_FrameCnt);
    VoC_lw16i(REG7,r_PF_CL_DTDMicNoiseFloorDevCnt);

    VoC_add16_rd(REG7,REG7,g_DTD_ch);


    VoC_lw16_d(REG2,m_AECProcPara_PFCLDTDUpdateFactor);




    VoC_jal(Coolsand_OverallNoiseEst);


    // Instant XCorr Mic Err
    // CmplxMul(pMicDataI, pMicDataQ, pErrDataI, &ConjErrDataQ, &InstantXcorrI, &InstantXcorrQ);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);


    VoC_lw16i(REG2,g_MicAnaFilterOutBufI);
    VoC_lw16i(REG3,g_MicAnaFilterOutBufQ);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_lw16_p(REG2,REG2,DEFAULT);

    VoC_lw16_p(REG3,REG3,DEFAULT);


    VoC_lw16i(REG4,g_AF_ErrDataBufI);
    VoC_lw16i(REG5,g_AF_ErrDataBufQ);

    VoC_add16_rd(REG4,REG4,CLChnl);
    VoC_add16_rd(REG5,REG5,CLChnl);

    VoC_lw16_p(REG4,REG4,DEFAULT);

    VoC_lw16_p(REG5,REG5,DEFAULT);

    VoC_sub16_rr(REG5,REG0,REG5,DEFAULT);

    VoC_jal(Coolsand_CmplxMul);

    //ACC0:InstantXcorrI
    //ACC1:InstantXcorrQ

    // Smooth XCorr
    //*pXorrMicErrI = FirstOrderIIRSmooth2(pXorrMicErrI, &InstantXcorrI, pUpdateFactor);
    //*pXorrMicErrQ = FirstOrderIIRSmooth2(pXorrMicErrQ, &InstantXcorrQ, pUpdateFactor);

    VoC_lw16i_short(FORMAT32,7,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);


    VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicErrI);
    VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicErrQ);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);
    VoC_add16_rd(REG3,REG3,g_DTD_ch);

    VoC_lw32_p(ACC0,REG2,DEFAULT);

    VoC_lw32_p(ACC1,REG3,DEFAULT);



    VoC_jal(Coolsand_Double_FirstOrderIIRSmooth2);

    // Instant XCorr Mic Echo
    //  CmplxMul(pMicDataI, pMicDataQ, &EchoDataI, &ConjEchoDataQ, &InstantXEcorrI, &InstantXEcorrQ);

    VoC_pop32(REG45,DEFAULT);////EchoDataI and ConjEchoDataQ
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicErrI);
    VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicErrQ);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);
    VoC_add16_rd(REG3,REG3,g_DTD_ch);

    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_sw32_p(ACC1,REG3,DEFAULT);

    VoC_lw16i(REG2,g_MicAnaFilterOutBufI);
    VoC_lw16i(REG3,g_MicAnaFilterOutBufQ);

    VoC_add16_rd(REG2,REG2,CLChnl);
    VoC_add16_rd(REG3,REG3,CLChnl);

    VoC_lw16_p(REG2,REG2,DEFAULT);

    VoC_lw16_p(REG3,REG3,DEFAULT);

    VoC_jal(Coolsand_CmplxMul);

    //ACC0:InstantXcorrI
    //ACC1:InstantXcorrQ

    // Smooth XCorr
    //*pXorrMicEchoI = FirstOrderIIRSmooth2(pXorrMicEchoI, &InstantXEcorrI, pUpdateFactor);
    //*pXorrMicEchoQ = FirstOrderIIRSmooth2(pXorrMicEchoQ, &InstantXEcorrQ, pUpdateFactor);

    VoC_lw16i_short(FORMAT32,7,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);



    VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicEchoI);
    VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicEchoQ);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);
    VoC_add16_rd(REG3,REG3,g_DTD_ch);

    VoC_lw32_p(ACC0,REG2,DEFAULT);

    VoC_lw32_p(ACC1,REG3,DEFAULT);

    VoC_jal(Coolsand_Double_FirstOrderIIRSmooth2);

    VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicEchoI);
    VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicEchoQ);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);
    VoC_add16_rd(REG3,REG3,g_DTD_ch);

    VoC_sw32_p(ACC0,REG2,DEFAULT);

    VoC_sw32_p(ACC1,REG3,DEFAULT);


    // Decision: >T0
    //  CmplxAppxNorm(pXorrMicEchoI, pXorrMicEchoQ, &NormXcorrMicEcho);
    VoC_jal(Coolsand_CmplxAppxNorm_int);


    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);


    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-10,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_ControlLogicDTD_L3,ACC0,ACC1)
    VoC_lw16i_short(REG7,0,DEFAULT);
Coolsand_ControlLogicDTD_L3:


    VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

//  VoC_NOP();

    VoC_lw16i(REG3,r_PF_CL_DTDMicNoisePower);

    VoC_add16_rd(REG3,REG3,g_DTD_ch);

    VoC_lw32_p(RL6,REG2,DEFAULT);

    VoC_sub32_rp(REG23,RL6,REG3,DEFAULT);


    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_movreg16(REG6,ACC0_LO,DEFAULT);
    VoC_shr32_ri(REG23,-1,IN_PARALLEL);

    VoC_multf32_rr(RL6,REG6,REG6,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    VoC_lw16_d(REG0,m_AECProcPara_PFCLDTDThreshold);

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG1,8,IN_PARALLEL);

    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG1,REG7,REG1,IN_PARALLEL);

    VoC_lw16i(REG4,r_PF_CL_DTDEchoSmoothPower);

    VoC_add16_rd(REG4,REG4,g_DTD_ch);

    VoC_NOP();

    VoC_lw32_p(REG45,REG4,DEFAULT);

    VoC_movreg16(FORMAT32,REG6,DEFAULT);
    VoC_shr32_ri(REG45,-1,IN_PARALLEL);

    VoC_movreg16(FORMATX,REG1,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG0,IN_PARALLEL);

    VoC_macX_rr(REG3,REG0,DEFAULT);
    VoC_macX_rr(REG5,REG0,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG23,-16,DEFAULT);
    VoC_shr32_ri(REG45,-16,IN_PARALLEL);

    VoC_bgtz16_r(Coolsand_ControlLogicDTD_L4,REG3)
    VoC_lw16i_short(REG3,1,DEFAULT);
Coolsand_ControlLogicDTD_L4:

    VoC_bnez16_r(Coolsand_ControlLogicDTD_L5,REG5)
    VoC_lw16i_short(REG5,1,DEFAULT);
Coolsand_ControlLogicDTD_L5:

    VoC_lw16i_short(REG6,23,DEFAULT);
    VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

    VoC_multf32_rr(REG45,REG3,REG5,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

    VoC_lw16i_short(REG7,25,DEFAULT);
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);

    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lw16_d(REG2,AECProcPara_PFCLDTDThrRatio);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);

    VoC_macX_rr(REG5,REG2,DEFAULT);

    VoC_NOP();

    VoC_movreg32(REG45,ACC0,DEFAULT);

    VoC_shr32_ri(REG45,-1,DEFAULT);

    VoC_shru16_ri(REG4,1,DEFAULT);

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);

    VoC_macX_rr(REG5,REG2,DEFAULT);

    VoC_NOP();

    VoC_sub32_rr(RL7,RL6,ACC0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

    VoC_shr32_rr(RL7,REG6,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_lw16i(REG4,r_PF_CL_DTDXcorrMicErrI);
    VoC_lw16i(REG5,r_PF_CL_DTDXcorrMicErrQ);
    VoC_lw16i(REG7,r_PF_CL_DTDDecision);

    VoC_add16_rd(REG4,REG4,g_DTD_ch);
    VoC_add16_rd(REG5,REG5,g_DTD_ch);
    VoC_add16_rd(REG7,REG7,g_DTD_ch);

    VoC_lw32_p(ACC0,REG4,DEFAULT);
    VoC_lw32_p(ACC1,REG5,DEFAULT);
    VoC_sw32_p(RL7,REG7,DEFAULT);

    // Decision: >T1
    //      CmplxAppxNorm(pXorrMicErrI, pXorrMicErrQ, &NormXcorrMicErr);
    VoC_jal(Coolsand_CmplxAppxNorm_int);


    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);


    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-10,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_bgt32_rr(Coolsand_ControlLogicDTD_L6,ACC0,ACC1)
    VoC_lw16i_short(REG7,0,DEFAULT);
Coolsand_ControlLogicDTD_L6:


    VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

    VoC_add16_rd(REG2,REG2,g_DTD_ch);

    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw32_p(REG23,REG2,DEFAULT);


    VoC_movreg16(REG6,ACC0_LO,DEFAULT);
    VoC_shr32_ri(REG23,-1,IN_PARALLEL);

    VoC_multf32_rr(RL6,REG6,REG6,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    VoC_lw16_d(REG0,m_AECProcPara_PFCLDTD2ndThreshold);

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG1,8,IN_PARALLEL);

    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG1,REG7,REG1,IN_PARALLEL);


    VoC_lw16i(REG4,r_PF_CL_DTDErrSmoothPower);

    VoC_add16_rd(REG4,REG4,g_DTD_ch);

    VoC_NOP();

    VoC_lw32_p(REG45,REG4,DEFAULT);


    VoC_movreg16(FORMAT32,REG6,DEFAULT);
    VoC_shr32_ri(REG45,-1,IN_PARALLEL);

    VoC_movreg16(FORMATX,REG1,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG0,IN_PARALLEL);

    VoC_macX_rr(REG3,REG0,DEFAULT);
    VoC_macX_rr(REG5,REG0,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG23,-16,DEFAULT);
    VoC_shr32_ri(REG45,-16,IN_PARALLEL);

    VoC_bnez16_r(Coolsand_ControlLogicDTD_L7,REG3)
    VoC_lw16i_short(REG3,1,DEFAULT);
Coolsand_ControlLogicDTD_L7:

    VoC_bnez16_r(Coolsand_ControlLogicDTD_L8,REG5)
    VoC_lw16i_short(REG5,1,DEFAULT);
Coolsand_ControlLogicDTD_L8:

    VoC_lw16i_short(REG6,23,DEFAULT);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

    VoC_lw16i_short(REG5,25,DEFAULT);
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);

    VoC_sub32_rr(ACC1,RL6,ACC0,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);

    VoC_shr32_rr(ACC1,REG6,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_lw16i(REG6,r_PF_CL_DTD2ndDecision);

    VoC_add16_rd(REG6,REG6,g_DTD_ch);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);

    VoC_sw32_p(ACC1,REG6,DEFAULT);

    VoC_bngt32_rr(Coolsand_ControlLogicDTD_L9,ACC1,RL7)
    VoC_bngt32_rr(Coolsand_ControlLogicDTD_L9,RL6,ACC0)
    VoC_lw16i_short(REG0,1,DEFAULT);
Coolsand_ControlLogicDTD_L9:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_CmplxMul
 *
 * Description:
 *
 * Inputs:  :
 *          :
 *          :
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/27/2011
 **************************************************************************************/
void Coolsand_CmplxMul(void)
{

    VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG3,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG3,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_shr32_ri(ACC1,-8,IN_PARALLEL);

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_shr32_ri(ACC1,8,IN_PARALLEL);

    VoC_shr32_ri(RL6,-8,DEFAULT);
    VoC_shr32_ri(RL7,-8,IN_PARALLEL);

    VoC_shr32_ri(RL6,8,DEFAULT);
    VoC_shr32_ri(RL7,8,IN_PARALLEL);

    VoC_sub32_rr(ACC0,RL6,RL7,DEFAULT);//InstantCorrI
    VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//InstantCorrQ

    VoC_shr32_ri(ACC0,-8,DEFAULT);
    VoC_shr32_ri(ACC1,-8,IN_PARALLEL);

    VoC_shr32_ri(ACC0,8,DEFAULT);
    VoC_shr32_ri(ACC1,8,IN_PARALLEL);

    VoC_return;


}

/**************************************************************************************
 * Function:    Coolsand_Double_FirstOrderIIRSmooth2
 *
 * Description:
 *
 * Inputs:  :
 *          :
 *          :
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/27/2011
 **************************************************************************************/
void Coolsand_Double_FirstOrderIIRSmooth2(void)
{
    VoC_sub32_rr(REG23,RL6,ACC0,DEFAULT);
    VoC_sub32_rr(REG45,RL7,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG23,-8,DEFAULT);
    VoC_shr32_ri(REG45,-8,DEFAULT);

    VoC_shr32_ri(REG23,7,DEFAULT);
    VoC_shr32_ri(REG45,7,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_mac32_rr(REG2,REG1,DEFAULT);
    VoC_mac32_rr(REG4,REG1,IN_PARALLEL);

    VoC_macX_rr(REG3,REG1,DEFAULT);
    VoC_macX_rr(REG5,REG1,IN_PARALLEL);

    VoC_return;
}