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





#include "vppp_amjp_asm_map.h"
#include "vppp_amjp_asm_aac.h"

#include "voc2_library.h"
#include "voc2_define.h"

#if 0

voc_struct AAC_Dec_ConstY_start,y

voc_word  Tab_huffTabSpec_addr[621]
voc_word  Tab_Cos1Sin1Compressed_addr[70]
voc_word  Tab_Cos4Compressed_addr[49]
voc_word  Tab_Sin4Compressed_addr[143]
voc_word  Tab_SinwShortCompressed_addr[27]
voc_word  Tab_SinwLongCompressed_addr[135]
voc_word  Tab_KbdwShortCompressed_addr[46]
voc_word  Tab_KbdwLongCompressed_addr[148]
voc_word  Tab_twidOddCompressed_addr[196]
voc_word  Tab_TwidEvenCompressed_addr[143]
voc_word  Tab_bitrevtabCompressed_addr[37]
voc_word  Tab_invQuant3_addr[16]
voc_word  Tab_invQuant4_addr[16]
voc_short Tab_sfBandTotal_addr[NUM_SAMPLE_RATES]
voc_short Tab_tnsMaxBands_addr[NUM_SAMPLE_RATES]
voc_word  Tab_pow14_addr[4]
voc_word  Tab_pow43_14_addr[4*16]
voc_word  Tab_pow43_addr[48]
voc_word  Tab_poly43lo_addr[5]
voc_word  Tab_poly43hi_addr[5]
voc_short Tab_pow2exp_addr[8]
voc_word  Tab_pow2frac_addr[8]
voc_short Tab_sfBandTabLongOffset_addr[NUM_SAMPLE_RATES]
voc_word  Tab_sfBandTabLongCompressed_addr[59]
voc_word  Tab_huffTabSpecInfoCompressed_addr[44]
voc_short Tab_sfBandTabShortOffset_addr[NUM_SAMPLE_RATES]
voc_short Tab_sfBandTabShort_addr[76]
voc_short Tab_huffTabScaleFact_addr[122]
voc_short Tab_huffTabScaleFactInfo_addr[22]
voc_short Tab_channelMapTab_addr[8]
voc_word  Tab_sampRateTab_addr[12]
voc_short Tab_elementNumChans_addr[NUM_ELEMENTS]
voc_word  const_SQRT1_2_addr
voc_word  const_0x80000000_addr
voc_word  const_0x7fffffff_addr
voc_word  const_0_addr
voc_word  const_1_addr
voc_short const_2_addr
voc_short const_3_addr
voc_short const_4_addr
voc_short const_5_addr
voc_short const_6_addr
voc_short const_7_addr
voc_short const_8_addr
voc_short const_9_addr
voc_short const_10_addr
voc_short const_11_addr
voc_short const_13_addr
voc_short const_14_addr
voc_short const_15_addr
voc_short const_16_addr
voc_short const_32_addr
voc_short const_0x0fff_addr
voc_short AAC_Dec_ConstY_end

voc_struct_end

voc_struct STRUCT_AACDecInfo_addr,x

/* state information which is the same throughout whole frame */
voc_short STRUCT_AACDecInfo_PSInfoBase_nChans_addr         //xuml modified it to voc_short.
voc_short STRUCT_AACDecInfo_PSInfoBase_useImpChanMap_addr  //xuml modified it to voc_short.
voc_short STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr    //xuml modified it to voc_short.

/* block information */
voc_short STRUCT_AACDecInfo_prevBlockID_addr           //   int prevBlockID;  //xuml modified it to short
voc_short STRUCT_AACDecInfo_currBlockID_addr         // int currBlockID;      //xuml modified it to short
voc_short STRUCT_AACDecInfo_currInstTag_addr          //    int currInstTag; //xuml modified it to short
voc_word  STRUCT_AACDecInfo_sbDeinterleaveReqd_addr[MAX_NCHANS_ELEM] // int sbDeinterleaveReqd[MAX_NCHANS_ELEM];
voc_short STRUCT_AACDecInfo_adtsBlocksLeft_addr           //    int adtsBlocksLeft; //xuml modified it to short

/* user-accessible info */
voc_short STRUCT_AACDecInfo_nChans_addr             //  int nChans;//xuml modified it to short
voc_word  STRUCT_AACDecInfo_bitRate_addr                 // int bitRate;
voc_word  STRUCT_AACDecInfo_sampRate_addr               //  int sampRate;
voc_short STRUCT_AACDecInfo_profile_addr                    //  int profile;//xuml modified it to short
voc_short STRUCT_AACDecInfo_format_addr            //   int format;
voc_short STRUCT_AACDecInfo_sbrEnabled_addr           //    int sbrEnabled;//xuml modified it to short
voc_short STRUCT_AACDecInfo_tnsUsed_addr                //   int tnsUsed  //xuml modified it to short
voc_short STRUCT_AACDecInfo_pnsUsed_addr                //   int pnsUsed   //xuml modified it to short
voc_short STRUCT_AACDecInfo_frameCount_addr             //  int frameCount//not used

voc_struct_end

voc_word  STRUCT_AACDecInfo_PSInfoBase_overlap_addr[AAC_MAX_NSAMPS],x//[AAC_MAX_NCHANS][AAC_MAX_NSAMPS]
voc_word  STRUCT_AACDecInfo_PSInfoBase_overlap1_addr[AAC_MAX_NSAMPS],x
voc_short STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr,x
voc_short STRUCT_AACDecInfo_PSInfoBase_prevWinShape1_addr,x

voc_word  GLOBAL_bitbuffer_addr,x  //io
voc_short GLOBAL_bytesLeft_addr,x   //io
voc_short GLOBAL_inputbuffiled_ptr_addr,x //io
voc_short GLOBAL_inputptr_addr,x  //io
voc_short GLOBAL_BITCACHE_addr,x //io


#endif



/**************************************************************************************
 * Function:    Coolsand_AACDecode
 *
 * Description: decode AAC frame
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       08/08/2007
 *
 * Notes:       inbuf pointer and bytesLeft are not updated until whole frame is
 *                successfully decoded, so if ERR_AAC_INDATA_UNDERFLOW is returned
 *                just call AACDecode again with more data in inbuf
 **************************************************************************************/
void Coolsand_AACDecode(void)
{

#if 0
    voc_short  Tab_DMA_start_addr[2304+2304],y
    voc_alias  Tab_cos4sin4tab_addr     Tab_DMA_start_addr,y
    voc_alias  Tab_cos1sin1tab_addr     Tab_DMA_start_addr+2304,y
    voc_alias  Tab_twidTabOdd_addr      Tab_DMA_start_addr+2304,y
    voc_alias  Tab_twidTabEven_addr     Tab_DMA_start_addr+2304,y
    voc_alias  Tab_sinWindow_addr       Tab_DMA_start_addr,y
    voc_alias  Tab_kbdWindow_addr       Tab_DMA_start_addr+2304,y
    voc_alias  Tab_bitrevtab_addr       Tab_DMA_start_addr+2304,y
    voc_alias  Tab_huffTabSpecInfo_addr Tab_DMA_start_addr,y
    voc_alias  Tab_sfBandTabLong_addr   Tab_DMA_start_addr+2304,y

    voc_short  MONO_outbuf_addr[AAC_MAX_NSAMPS],y
#endif

#if 0

    voc_word   STRUCT_AACDecInfo_PSInfoBase_coef_addr[AAC_MAX_NSAMPS],x  //[MAX_NCHANS_ELEM][AAC_MAX_NSAMPS]
    voc_word   STRUCT_AACDecInfo_PSInfoBase_coef1_addr[AAC_MAX_NSAMPS],x
    voc_alias  STEREO_outbuf_addr STRUCT_AACDecInfo_PSInfoBase_coef_addr,x

    /* header information */
    voc_struct STRUCT_AACDecInfo_PSInfoBase_fhADTS_addr,x  //    ADTSHeader            fhADTS;
    /* fixed */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_id_addr                             /* MPEG bit - should be 1 */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_layer_addr                          /* MPEG layer - should be 0 */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_protectBit_addr                     /* 0 = CRC word follows, 1 = no CRC word */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_profile_addr                        /* 0 = main, 1 = LC, 2 = SSR, 3 = reserved */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_sampRateIdx_addr                    /* sample rate index range = [0, 11] */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_privateBit_addr                     /* ignore */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_channelConfig_addr                  /* 0 = implicit, >0 = use default table */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_origCopy_addr                       /* 0 = copy, 1 = original */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_home_addr                           /* ignore */

    /* variable */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_copyBit_addr                        /* 1 bit of the 72-bit copyright ID (transmitted as 1 bit per frame) */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_copyStart_addr                      /* 1 = this bit starts the 72-bit ID, 0 = it does not */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_frameLength_addr                    /* length of frame */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_bufferFull_addr                     /* number of 32-bit words left in enc buffer, 0x7FF = VBR */
    voc_short STRUCT_AACDecInfo_PSInfoBase_fhADTS_numRawDataBlocks_addr               /* number of raw data blocks in frame */

    voc_struct_end

    /* state information which can be overwritten by subsequent elements within frame */
    voc_struct STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,x  // ICSInfo  icsInfo[0];
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_icsResBit_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_winSequence_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_winShape_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_maxSFB_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_sfGroup_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_predictorDataPresent_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_numWinGroup_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo_winGroupLen_addr[MAX_WIN_GROUPS]
    voc_struct_end

    voc_struct STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,x  // ICSInfo  icsInfo[1];
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_icsResBit_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winSequence_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winShape_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_maxSFB_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_sfGroup_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_predictorDataPresent_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_numWinGroup_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_icsInfo1_winGroupLen_addr[MAX_WIN_GROUPS]
    voc_struct_end

    voc_struct STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr,x  //PulseInfo pulseInfo[MAX_NCHANS_ELEM]
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo_pulseDataPresent_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo_numPulse_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo_startSFB_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo_offset_addr[MAX_PULSES];
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo_amp_addr[MAX_PULSES];
    voc_struct_end

    voc_struct STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr,x  //PulseInfo pulseInfo[MAX_NCHANS_ELEM]
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_pulseDataPresent_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_numPulse_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_startSFB_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_offset_addr[MAX_PULSES];
    voc_short STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_amp_addr[MAX_PULSES];
    voc_struct_end

    voc_struct STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr,x  //   TNSInfo tnsInfo[MAX_NCHANS_ELEM];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_tnsDataPresent_addr;
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_numFilt_addr[MAX_TNS_FILTERS];     /* max 1 filter each for 8 short windows, or 3 filters for 1 long window */
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coefRes_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_length_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_order_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_dir_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo_coef_addr[MAX_TNS_COEFS];              /* max 3 filters * 20 coefs for 1 long window, or 1 filter * 7 coefs for each of 8 short windows */
    voc_struct_end

    voc_struct STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr,x  //   TNSInfo tnsInfo[MAX_NCHANS_ELEM];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_tnsDataPresent_addr;
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_numFilt_addr[MAX_TNS_FILTERS];     /* max 1 filter each for 8 short windows, or 3 filters for 1 long window */
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coefRes_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_length_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_order_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_dir_addr[MAX_TNS_FILTERS];
    voc_short STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_coef_addr[MAX_TNS_COEFS];              /* max 3 filters * 20 coefs for 1 long window, or 1 filter * 7 coefs for each of 8 short windows */
    voc_struct_end

    /* state info struct for baseline (MPEG-4 LC) decoding */
    voc_struct STRUCT_AACDecInfo_PSInfoBase_addr,x
    voc_short STRUCT_AACDecInfo_PSInfoBase_commonWin_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr[MAX_SF_BANDS]
    voc_short STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr[MAX_SF_BANDS]
    voc_short STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr[MAX_SF_BANDS]
    voc_short STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr[MAX_SF_BANDS]
    voc_short STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr[MAX_MS_MASK_BYTES_divs_2]
    voc_short STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr[MAX_NCHANS_ELEM]
    voc_word  STRUCT_AACDecInfo_PSInfoBase_pnsLastVal_addr
    voc_short STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr[MAX_NCHANS_ELEM]
    voc_word  STRUCT_AACDecInfo_PSInfoBase_tnsLPCBuf_addr[MAX_TNS_ORDER]
    voc_word  STRUCT_AACDecInfo_PSInfoBase_tnsWorkBuf_addr[MAX_TNS_ORDER]
    voc_short STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr[MAX_NCHANS_ELEM]
    voc_struct_end


#endif


    VoC_push16(RA,DEFAULT);
    VoC_bez16_d(AACDecode_Begin,GLOBAL_RESET)

    // init the stream pointer
    VoC_jal(vpp_AmjpCircBuffInit);

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG0,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(REG1,GLOBAL_INPUT_DATA_BUFFER);
    VoC_sw16_d(ACC0_HI,GLOBAL_RESET);
    VoC_sw16_d(ACC0_HI,GLOBAL_BITCACHE_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_format_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_adtsBlocksLeft_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_nChans_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_useImpChanMap_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr);
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr+1);

    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_overlap_addr,2);

    VoC_loop_i(1,4);
    VoC_loop_i_short(256,DEFAULT);
    VoC_startloop0;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0;
    VoC_NOP();
    VoC_endloop1;

    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_overlap1_addr,2);

    VoC_loop_i(1,4);
    VoC_loop_i_short(256,DEFAULT);
    VoC_startloop0;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0;
    VoC_NOP();
    VoC_endloop1;

    VoC_sw32_d(ACC0,STRUCT_AACDecInfo_PSInfoBase_pnsLastVal_addr);
    VoC_sw32_d(ACC0,GLOBAL_bitbuffer_addr);
    VoC_sw16_d(ACC0_HI,GLOBAL_bytesLeft_addr);
    VoC_sw16_d(REG1,GLOBAL_inputbuffiled_ptr_addr);
    VoC_sw16_d(REG1,GLOBAL_inputptr_addr);

AACDecode_Begin:
    VoC_lw16i_short(WRAP2,10,DEFAULT);

    VoC_jal(Coolsand_reload_data);

    VoC_lw16_d(BITCACHE,GLOBAL_BITCACHE_addr);
    VoC_lw32_d(RL6,GLOBAL_bitbuffer_addr);
    VoC_lw16_d(REG1,GLOBAL_bytesLeft_addr);

    VoC_shr16_ri(REG1,-3,DEFAULT);  //(*bytesLeft) << 3;
    //VoC_lw16i_set_inc(REG2,STATIC_readBuf_addr,1);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16_d(REG2,GLOBAL_inputptr_addr);

    /* first time through figure out what the file format is */
    VoC_bnez16_d(AACDecode_L0,STRUCT_AACDecInfo_format_addr)

    VoC_bnlt16_rd(AACDecode_L1,REG1,const_32_addr)//if (bitsAvail < 32)
    //VoC_lw16i_short(REG0,-1,DEFAULT);//return ERR_AAC_INDATA_UNDERFLOW;

    VoC_jump(AACDecode_Begin);
AACDecode_L1:
    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG4,16,DEFAULT);//"A","D"
    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG5,16,DEFAULT);//"I","F";

    VoC_lw16i(REG6,0x4144);//"A","D"
    VoC_lw16i(REG7,0x4946);

    /* assume ADTS by default */
    VoC_lw16i_short(REG3,1,DEFAULT);//AAC_FF_ADTS

    VoC_bne32_rr(AACDecode_L2,REG45,REG67);//if (IS_ADIF(inptr))

    //VoC_jal(Coolsand_UnpackADIFHeader);
    /* unpack ADIF header */
    VoC_lw16i_short(REG3,2,DEFAULT);//AAC_FF_ADIF
    VoC_bnez16_r(AACDecode_End,REG0)//if (err)

AACDecode_L2:
    VoC_sw16_d(REG3,STRUCT_AACDecInfo_format_addr);

    VoC_sub16_rd(REG2,REG2,const_2_addr);//REG2-2; return to buffer start point!

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    // VoC_lw32z(RL6,IN_PARALLEL);
AACDecode_L0:



    VoC_lw16i_short(REG6,1,DEFAULT);//AAC_FF_ADTS
    /* if ADTS, search for start of next frame */
    VoC_lw16_d(REG7,STRUCT_AACDecInfo_format_addr);
    VoC_bne16_rr(AACDecode_L3,REG6,REG7)//if (aacDecInfo->format == AAC_FF_ADTS)
    /* can have 1-4 raw data blocks per ADTS frame (header only present for first one) */
    VoC_bnez16_d(AACDecode_L4,STRUCT_AACDecInfo_adtsBlocksLeft_addr)//if (aacDecInfo->adtsBlocksLeft == 0)

    VoC_jal(Coolsand_AACFindSyncWord);

    VoC_bnltz16_r(AACDecode_L5,REG3);//if (offset < 0)
    //VoC_lw16i_short(REG0,-1,DEFAULT);//return ERR_AAC_INDATA_UNDERFLOW;
    VoC_jump(AACDecode_Begin);
AACDecode_L5:
    VoC_sub16_dr(REG4,GLOBAL_bytesLeft_addr,REG3);
    VoC_lw16i(REG5,1536);
    VoC_bgt16_rr(AACDecode_X1,REG4,REG5)
    //VoC_lw16i_short(REG0,-1,DEFAULT);//return ERR_AAC_INDATA_UNDERFLOW;
    VoC_jump(AACDecode_Begin); //load more data
AACDecode_X1:


    VoC_jal(Coolsand_UnpackADTSHeader);


    VoC_bnez16_r(AACDecode_End,REG0)//if (err)
    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_bne16_rd(AACDecode_L4,REG6,STRUCT_AACDecInfo_nChans_addr)
    /* figure out implicit channel mapping if necessary */
    VoC_bnez16_r(AACDecode_End,REG0)//if (err)
AACDecode_L4:
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sub16_dr(REG6,STRUCT_AACDecInfo_adtsBlocksLeft_addr,REG6); // aacDecInfo->adtsBlocksLeft--;
    VoC_NOP();
    VoC_sw16_d(REG6,STRUCT_AACDecInfo_adtsBlocksLeft_addr);
    VoC_jump(AACDecode_L6);
AACDecode_L3://else if (aacDecInfo->format == AAC_FF_RAW)
    VoC_lw16i_short(REG6,3,DEFAULT);//AAC_FF_RAW
    VoC_bne16_rd(AACDecode_L6,REG6,STRUCT_AACDecInfo_format_addr)//if (aacDecInfo->format == AAC_FF_ADTS)
    VoC_bnez16_r(AACDecode_End,REG0)//if (err)
AACDecode_L6:

    VoC_lw16i_short(REG0,-15,DEFAULT); //ERR_AAC_NCHANS_TOO_HIGH
    /* check for valid number of channels */
    VoC_lw16i_short(REG6,AAC_MAX_NCHANS,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    //if (aacDecInfo->nChans > AAC_MAX_NCHANS || aacDecInfo->nChans <= 0)
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_adtsBlocksLeft_addr);
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_nChans_addr);
    VoC_bgt16_rr(AACDecode_End,REG4,REG6);
    VoC_bngtz16_r(AACDecode_End,REG5);

    /* will be set later if active in this frame */
    VoC_lw16i_short(REG6,0,DEFAULT);//baseChan = 0;
    VoC_sw16_d(REG7,STRUCT_AACDecInfo_tnsUsed_addr); //aacDecInfo->tnsUsed = 0;
    VoC_sw16_d(REG7,STRUCT_AACDecInfo_pnsUsed_addr); //aacDecInfo->pnsUsed = 0;

AACDecode_L7:
    /* parse next syntactic element */
    VoC_lw16i_short(WRAP2,10,DEFAULT);
    VoC_push16(REG6,DEFAULT);//save baseChan;
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_jal(Coolsand_DecodeNextElement);
    VoC_bnez16_r(AACDecode_End,REG0)//if (err)

    VoC_lw16i(REG4,Tab_elementNumChans_addr);
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_currBlockID_addr);
    VoC_pop16(REG6,DEFAULT);//load basechan
    VoC_lw16i_short(REG7,AAC_MAX_NCHANS,IN_PARALLEL);
    VoC_lw16_p(REG5,REG4,DEFAULT);//elementChans;
    VoC_add16_rp(REG4,REG6,REG4,IN_PARALLEL);//baseChan + elementChans;
    VoC_bngt16_rr(AACDecode_L8,REG4,REG7)//if (baseChan + elementChans > AAC_MAX_NCHANS)

    VoC_lw16i(REG0,-15);//ERR_AAC_NCHANS_TOO_HIGH
    VoC_jump(AACDecode_End);
AACDecode_L8:

    VoC_jal(vpp_AmjpAacGenerateSfBandLong);

    VoC_lw16i_short(REG4,0,DEFAULT);//ch=0;
    VoC_push16(REG5,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);
    /* noiseless decoder and dequantizer */
AACDecode_L9:
    VoC_bngt16_rr(AACDecode_L10,REG5,REG4);

    VoC_lw16i_short(WRAP2,10,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    VoC_movreg16(REG1,REG4,IN_PARALLEL);//ch;

    VoC_jal(Coolsand_DecodeNoiselessData);

    VoC_bnez16_r(AACDecode_End,REG0)//if (err)
    VoC_lw16i_short(WRAP2,0,DEFAULT);

    VoC_lw32_sd(REG67,0,DEFAULT);//REG6:ch
    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_jal(Coolsand_Dequantize);

    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_jump(AACDecode_L9);
AACDecode_L10:


    /* mid-side and intensity stereo */
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_bne16_rd(AACDecode_L11,REG4,STRUCT_AACDecInfo_currBlockID_addr)
    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
    VoC_jal(Coolsand_StereoProcess);
    VoC_pop16(REG2,DEFAULT);

    VoC_pop32(RL6,IN_PARALLEL);
    VoC_bez16_r(AACDecode_L11,REG0)//if (err)
    VoC_lw16i_short(REG0,-10,DEFAULT);
    VoC_jump(AACDecode_End);
AACDecode_L11:

    VoC_lw16i_short(REG4,0,DEFAULT);//ch=0;
    VoC_pop16(REG6,IN_PARALLEL);//baseChan;
    VoC_pop16(REG5,DEFAULT);//elementChans;

    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
    /* PNS, TNS, inverse transform */
AACDecode_L12:
    VoC_bngt16_rr(AACDecode_L13,REG5,REG4);


    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG6,IN_PARALLEL);
    VoC_push16(REG4,DEFAULT);

    VoC_jal(vpp_AmjpAacGenerateCos4Sin4);
    VoC_jal(vpp_AmjpAacGenerateSfBandLong);

    VoC_jal(Coolsand_PNS);
    VoC_bez16_r(AACDecode_L14,REG0)//if (err)
    VoC_lw16i_short(REG0,-11,DEFAULT);
    VoC_jump(AACDecode_End);
AACDecode_L14:
    VoC_pop16(REG4,DEFAULT);

    VoC_jal(Coolsand_TNSFilter);

    VoC_lw32_sd(REG67,0,DEFAULT);//REG6:ch
    VoC_lw16_sd(REG4,0,IN_PARALLEL);//REG4:baseChan
    VoC_add16_rr(REG7,REG6,REG4,DEFAULT);//baseChan + ch

    VoC_jal(Coolsand_IMDCT);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    // copy mono to stereo buffer copy if the signal is stereo
    VoC_be16_rd(AACDecode_NoStereoCopy,REG6,STRUCT_AACDecInfo_nChans_addr)

    VoC_lw16i_set_inc(REG0,MONO_outbuf_addr,1);
    VoC_lw16i_set_inc(REG1,STEREO_outbuf_addr,2);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lw16inc_p(REG2,REG0,IN_PARALLEL);

    VoC_loop_i(1,16)

    VoC_loop_i(0,(AAC_MAX_NSAMPS/16))

    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);

    VoC_endloop0

    VoC_NOP();

    VoC_endloop1

AACDecode_NoStereoCopy:

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);

    VoC_jump(AACDecode_L12);

AACDecode_L13:

    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_lw16i_short(REG7,7,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,IN_PARALLEL);//baseChan += elementChans;
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_currBlockID_addr);

    VoC_bne16_rr(AACDecode_L7,REG7,REG4);

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i(REG7,AAC_MAX_NSAMPS/2);//int
    VoC_lw16i(REG6,MONO_outbuf_addr/2);

    VoC_be16_rd(AACDecode_NoStereoWrite,REG5,STRUCT_AACDecInfo_nChans_addr)

    VoC_lw16i(REG7,AAC_MAX_NSAMPS);//int
    VoC_lw16i(REG6,STEREO_outbuf_addr/2);

AACDecode_NoStereoWrite:

    /*****************  DMA  ************************/
    VoC_lw32_d(ACC0,GLOBAL_STRM_BUF_OUT_PTR);
    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA  ************************/

    /* byte align after each raw_data_block */
    VoC_push16(BITCACHE,DEFAULT);
    VoC_pop16(REG5,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_and16_ri(REG5,0x0007);  // BITCACHE mod 8
    VoC_rbinc_r(REG4,REG5,DEFAULT); // byte align
    VoC_sw32_d(RL6,GLOBAL_bitbuffer_addr);
    VoC_sw16_d(BITCACHE,GLOBAL_BITCACHE_addr);
    VoC_sw16_d(REG2,GLOBAL_inputptr_addr);

AACDecode_End:

    VoC_lw16_d(REG3,STRUCT_AACDecInfo_nChans_addr);
    VoC_sub16_dr(REG3,const_0_addr,REG3);
    VoC_lw16i(REG5,1024);
    VoC_shr16_rr(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG3,-1,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_shru32_ri(REG45,16,IN_PARALLEL);
    VoC_sw16_d(REG0, GLOBAL_ERR_STATUS);
    // VoC_sw32_d(REG45, GLOBAL_OUT_LEN);
    VoC_bez16_r(AACDecode_error_manage,REG0);
    VoC_be16_rr(AACDecode_error_manage,REG0,REG3);//for ERR_AAC_INDATA_UNDERFLOW
    VoC_lw16_d(REG2,GLOBAL_inputptr_addr);
    VoC_lw32_d(RL6,GLOBAL_bitbuffer_addr);
    VoC_lw16_d(BITCACHE,GLOBAL_BITCACHE_addr);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_bez16_d(AACDecode_error_manage_L0,GLOBAL_ERR_STATUS);
    VoC_sw16_d(REG4,STRUCT_AACDecInfo_adtsBlocksLeft_addr);
AACDecode_error_manage_L0:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG4,8,DEFAULT);//go advanced 8 from last syn.

    VoC_sw16_d(REG2,GLOBAL_inputptr_addr);
    VoC_sw32_d(RL6,GLOBAL_bitbuffer_addr);
    VoC_sw16_d(BITCACHE,GLOBAL_BITCACHE_addr);
AACDecode_error_manage:
    VoC_return;


}
/**************************************************************************************
 * Function:    Coosand_reload_data
 *
 * Description: reload data
 *
 * Inputs:      ?
 *
 * Outputs:     ?
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       08/27/2007
 **************************************************************************************/

void Coolsand_reload_data(void)
{

    // output:REG2:length
    VoC_lw16_d(REG7,GLOBAL_inputptr_addr);
    VoC_and16_ri(REG7,0x7ff);
    VoC_lw16_d(REG6,GLOBAL_inputbuffiled_ptr_addr);
    VoC_sw16_d(REG7,GLOBAL_inputptr_addr);

    // compute the circular buffer status
    VoC_sub16_rr(REG5,REG6,REG7,DEFAULT);

    VoC_lw16i(REG3,2048);
    VoC_bgtz16_r(aac_reload_data_temp_L0,REG5)
    VoC_add16_rr(REG5,REG5,REG3,DEFAULT);
aac_reload_data_temp_L0:

    // bytes left
    VoC_lw16i(REG3,1536);

    //VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_bytesLeft_addr);

    // check reload condition
    VoC_bgt16_rr(aac_reload_data_temp_L1,REG5,REG3)

    VoC_shr16_ri(REG6,2,DEFAULT);
    VoC_shr16_ri(REG7,2,IN_PARALLEL);
    VoC_sub16_rr(REG7,REG7,REG6,DEFAULT);

    VoC_bgtz16_r(aac_reload_data_temp_L2,REG7)
    VoC_lw16i(REG5,512);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);
aac_reload_data_temp_L2:

    // REG6 : begin_address   REG7 : Length
    VoC_and16_ri(REG7,0xfffc); // for 4 int aligned

    // wrap on 2^10 short
    VoC_lw16i_short(REG4,10,DEFAULT);
    VoC_lw16i(REG5,0x1ff);

    VoC_push16(RA,DEFAULT);

    // ----------------------------
    // fct : vpp_AmjpDmaRdCircBuff
    // in :  REG4 -> local buffer wrap exponent
    //       REG5 -> local buffer wrap mask
    //       REG6 -> local buffer address
    //       REG7 -> load size (words)
    // out : REG5 -> new local buffer address
    // not modified : REG0
    // unused : REG012, ACC0, ACC1
    // ----------------------------

    // read data from external input circular buffer
    VoC_jal(vpp_AmjpCircBuffDmaRd);

    VoC_pop16(RA,DEFAULT);

    // store new local address
    VoC_sw16_d(REG5,GLOBAL_inputbuffiled_ptr_addr);

aac_reload_data_temp_L1:

    VoC_return;

}



/**************************************************************************************
 * Function:    Coolsand_CLZ
 *
 * Description:     count leading zeros on signed integer x
 *
 * Inputs:      RL6->x;
 *
 *
 * Outputs:     REG6->return
 *
 * Used : REG6,REG7,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/17/2007
 *
 **************************************************************************************/
void Coolsand_CLZ(void)
{
    VoC_lw16i(REG6,32);
    VoC_bez32_r(CLZ_End,RL6)
    VoC_movreg32(RL7,RL6,DEFAULT);
    VoC_shru32_ri(RL6,16,IN_PARALLEL);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,16,IN_PARALLEL);

    VoC_bnez32_r(CLZ_L0,RL6)//if (!((unsigned int)x >> 16)) { numZeros += 16; x <<= 16; }
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_shru32_ri(RL7,-16,IN_PARALLEL);
CLZ_L0:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,24,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L1,RL6)//if (!((unsigned int)x >> 24)) { numZeros +=  8; x <<=  8; }
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_shru32_ri(RL7,-8,IN_PARALLEL);
CLZ_L1:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,28,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L2,RL6)//if (!((unsigned int)x >> 28)) { numZeros +=  4; x <<=  4; }
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_shru32_ri(RL7,-4,IN_PARALLEL);
CLZ_L2:
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_shru32_ri(RL6,30,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_bnez32_r(CLZ_L3,RL6)//if (!((unsigned int)x >> 30)) { numZeros +=  2; x <<=  2; }
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_shru32_ri(RL7,-2,IN_PARALLEL);
CLZ_L3:

    VoC_shru32_ri(RL7,31,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);  //numZeros -= ((unsigned int)x >> 31);

CLZ_End:
    VoC_return;


}

/**************************************************************************************
 * Function:    Coolsand_MULSHIFT32
 *
 * Description: signed multiply of two 32-bit integers (x and y), returns top 32-bits of 64-bit result
 *
 * Inputs:      REG01->x; FORMAT32=0,FORMATX=15
 *              REG67->y;
 *
 * Outputs:     ACC0->return
 *
 * Used : REG0,REG1,REG6,REG7,ACC0
 *
 * Version 1.0  Created by  Xuml       07/18/2007
 *
 **************************************************************************************/
void Coolsand_MULSHIFT32(void)
{
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_MADD64
 *
 * Description: 64-bit multiply accumulate: sum64 += (x*y)
 *
 * Inputs:      REG01->x; FORMAT32=0,FORMATX=15
 *              REG67->y;
 *              ACC1->sum64_HI
 *              ACC0->sum64_LO
 *
 * Outputs:     ACC1->sum64_HI
 *              ACC0->sum64_LO
 *
 * Used : REG0,REG1,REG4,REG5,REG6,REG7,ACC0,ACC1,RL6,RL7
 *
 * Version 1.0  Created by  Xuml       07/26/2007
 *
 **************************************************************************************/
void Coolsand_MADD64(void)
{
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_mac32_rr(REG1,REG7,IN_PARALLEL);

    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_macX_rr(REG6,REG1,IN_PARALLEL);

    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_macX_rr(REG7,REG0,IN_PARALLEL);

    VoC_lw16i(STATUS,SHRU_ON);
    VoC_lw16i_short(RL6_LO,1,DEFAULT);
    //FORMAT32=-2,FORMATX=-17

    VoC_mac32_rr(REG0,REG6,DEFAULT);
    VoC_lw16i_short(FORMAT32,-2,IN_PARALLEL);

    VoC_and16_ri(REG5,0x0001);

    VoC_bcz16_r(MADD64_L0);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L0:
    VoC_macX_rr(REG0,REG7,DEFAULT);
    VoC_lw16i_short(FORMATX,-17,IN_PARALLEL);

    VoC_and16_ri(REG4,0x0001);

    VoC_bcz16_r(MADD64_L1);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L1:

    VoC_macX_rr(REG1,REG6,DEFAULT);
    VoC_shru16_ri(REG0,-1,IN_PARALLEL)

    VoC_lw16i_short(FORMATX,15,DEFAULT);
    VoC_shru16_ri(REG6,-1,IN_PARALLEL);

    VoC_bcz16_r(MADD64_L2);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L2:

    VoC_bez16_r(MADD64_L3,REG4);
    VoC_movreg32(RL7,REG67,DEFAULT);
    VoC_shr32_ri(REG67,31,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);

    VoC_bcz16_r(MADD64_L3);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L3:


    VoC_bez16_r(MADD64_L4,REG5);
    VoC_movreg32(RL7,REG01,DEFAULT);
    VoC_shr32_ri(REG01,31,IN_PARALLEL);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,REG01,IN_PARALLEL);

    VoC_bcz16_r(MADD64_L4);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L4:
    VoC_and16_rr(REG4,REG5,DEFAULT);
    VoC_bez16_r(MADD64_L5,REG4);
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);

    VoC_bcz16_r(MADD64_L5);
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
MADD64_L5:
    VoC_lw16i(STATUS,SHRU_OFF);
    VoC_return;
}


/*********************************assembly.c end************************************************/


/*********************************dct.c begin************************************************/
/**************************************************************************************
 * Function:    Coolsand_PreMultiplyRescale
 *
 * Description: pre-twiddle stage of DCT4, with rescaling for extra guard bits
 *
 * Inputs:      REG1->zbuf1;INC1->2;
 *              REG0->es;
 *              REG3->tabidx;
 *
 * Outputs:    NONE;
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/30/2007
 *
 * Notes:       see notes on PreMultiply(), above
 **************************************************************************************/

void Coolsand_PreMultiplyRescale(void)
{
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(FORMATX,15,IN_PARALLEL);
    VoC_lw16i_short(REG6,0,DEFAULT);//cos4sin4tabOffset[tabidx]
    VoC_lw16i_short(INC2,-2,IN_PARALLEL);
    VoC_lw16i(REG5,256);//nmdct = nmdctTab[tabidx];
    VoC_lw16i(REG4,Tab_cos4sin4tab_addr);
    VoC_bez16_r(PreMultiplyRescale_L0,REG3);//if tabidx==0
    VoC_lw16i(REG6,256);//cos4sin4tabOffset[tabidx];
    VoC_lw16i(REG5,2048);//nmdct = nmdctTab[tabidx];
PreMultiplyRescale_L0:
    VoC_add16_rr(REG2,REG1,REG5,DEFAULT);//zbuf1 + nmdct
    VoC_add16_rr(REG3,REG4,REG6,IN_PARALLEL);//csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];
    VoC_shr16_ri(REG5,3,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_inc_p(REG2,DEFAULT);//zbuf2 = zbuf1 + nmdct - 1;
    /* whole thing should fit in registers - verify that compiler does this */
    VoC_loop_r(0,REG5)
    /* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
    VoC_lw32inc_p(RL6,REG3,DEFAULT);//cps2a = *csptr++;
    VoC_lw32_p(REG67,REG1,IN_PARALLEL);
    VoC_lw32inc_p(REG45,REG3,DEFAULT);//sin2a = *csptr++;
    VoC_lw32_p(ACC1,REG2,IN_PARALLEL);

    VoC_shr32_rr(REG67,REG0,DEFAULT);//ar1 = *(zbuf1 + 0) >> es;
    VoC_shr32_rr(ACC1,REG0,IN_PARALLEL);//ai1 = *(zbuf2 + 0) >> es;

    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);//ar1 + ai1
    VoC_movreg32(RL7,REG45,IN_PARALLEL);

    //t  = MULSHIFT32(sin2a, ar1 + ai1);
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);//ai1
    VoC_movreg32(REG45,RL6,DEFAULT);//cps2a
    VoC_shr32_ri(RL7,-1,IN_PARALLEL);//2*sin2a
    //z2 = MULSHIFT32(cps2a, ai1) - t;
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG7,IN_PARALLEL);
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_macX_rr(REG6,REG5,IN_PARALLEL);
    VoC_sub32_rr(REG45,RL6,RL7,DEFAULT)//cms2 = cps2a - 2*sin2a;
    VoC_macX_rr(REG7,REG4,IN_PARALLEL);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(RL7,ACC0,DEFAULT);//t
    VoC_lw32_p(REG67,REG1,IN_PARALLEL);
    VoC_shr32_rr(REG67,REG0,DEFAULT);//ar1 = *(zbuf1 + 0) >> es;
    VoC_sub32_rr(ACC1,ACC1,RL7,IN_PARALLEL)//z2 = MULSHIFT32(cps2a, ai1) - t;
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////


    VoC_lw32inc_p(RL6,REG3,IN_PARALLEL);//cps2b = *csptr++;
    VoC_sub16_rd(REG4,REG2,const_2_addr);//(zbuf2 - 1)
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//z1 = MULSHIFT32(cms2, ar1) + t;
    VoC_lw32inc_p(REG45,REG3,IN_PARALLEL);//sin2b = *csptr++;
    VoC_lw32_p(REG67,REG4,DEFAULT);//ar2 = *(zbuf2 - 1);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*zbuf1++ = z1;
    VoC_shr32_rr(REG67,REG0,IN_PARALLEL);//ar2 = *(zbuf2 - 1) >> es;

    VoC_lw32_p(ACC0,REG1,DEFAULT);

    VoC_shr32_rr(ACC0,REG0,DEFAULT);//ai2 = *(zbuf1 + 1) >> es;
    VoC_sw32inc_p(ACC1,REG1,IN_PARALLEL);//*zbuf1++ = z2;

    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);//ar2 + ai2
    VoC_push32(REG67,IN_PARALLEL)
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    //  t  = MULSHIFT32(sin2a, ar1 + ai1);
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shr32_ri(RL7,-1,IN_PARALLEL);//2*sin2b
    VoC_macX_rr(REG7,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);//ai2
    VoC_movreg32(REG45,RL6,DEFAULT);//cps2b

    //z2 = MULSHIFT32(cps2a, ai1) - t;
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rr(ACC1,REG5,REG7,IN_PARALLEL);
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_macX_rr(REG6,REG5,IN_PARALLEL);
    VoC_sub32_rr(REG45,RL6,RL7,DEFAULT);//cms2 = cps2b - 2*sin2b;
    VoC_macX_rr(REG7,REG4,IN_PARALLEL);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(RL7,ACC0,DEFAULT);//t
    VoC_pop32(REG67,DEFAULT);//ar2
    VoC_sub32_rr(ACC1,ACC1,RL7,IN_PARALLEL);//z2 = MULSHIFT32(cps2a, ai1) - t;
    //z1 = MULSHIFT32(cms2, ar1) + t;
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////

    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//z1 = MULSHIFT32(cms2, ar2) + t;
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    VoC_endloop0

    VoC_return;

}



/**************************************************************************************
 * Function:    Coolsand_PostMultiplyRescale
 *
 * Description: post-twiddle stage of DCT4, with rescaling for extra guard bits
 *
 * Inputs:      REG1->fft1;
 *              REG0->es;
 *              REG3->tabidx;
 *
 * Outputs:    NONE;
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/30/2007
 *
 * Notes:       clips output to [-2^30, 2^30 - 1], guaranteeing at least 1 guard bit
 *              see notes on PostMultiply(), above
 **************************************************************************************/
void Coolsand_PostMultiplyRescale(void)
{
    VoC_sub16_dr(REG0,const_0_addr,REG0);
    VoC_lw16i_short(REG6,30,DEFAULT);//skipFactor = postSkip[tabidx];
    VoC_lw16i_short(INC2,-2,IN_PARALLEL);
    VoC_lw16i(REG5,256);//nmdct = nmdctTab[tabidx];
    VoC_lw16i(REG4,Tab_cos4sin4tab_addr);
    VoC_bez16_r(PostMultiplyRescale_L0,REG3);//if tabidx==0
    VoC_lw16i_short(REG6,2,DEFAULT);//skipFactor = postSkip[tabidx];
    VoC_lw16i(REG5,2048);//nmdct = nmdctTab[tabidx];
PostMultiplyRescale_L0:
    VoC_add16_rr(REG2,REG1,REG5,DEFAULT);//fft1 + nmdct
    VoC_NOP();

    VoC_push32(REG01,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);

    VoC_jal(vpp_AmjpAacGenerateCos1);

    VoC_pop32(REG67,DEFAULT);
    VoC_lw16i_short(INC2,-2,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i(REG3,Tab_cos1sin1tab_addr);//csptr = cos1sin1tab;

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);//fft2 = fft1 + nmdct - 1;

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    /* load coeffs for first pass
    * cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin)
    */
    VoC_lw32inc_p(RL6,REG3,DEFAULT);//cs2 = *csptr++;
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//csptr += skipFactor;
    VoC_lw32_p(RL7,REG3,DEFAULT);//sin2 = *csptr;
    VoC_shr16_ri(REG5,3,IN_PARALLEL);//nmdct >> 2

    VoC_loop_r(0,REG5);//for (i = nmdct >> 2; i != 0; i--)

    VoC_push16(REG6,DEFAULT);
    VoC_lw32inc_p(REG45,REG1,DEFAULT);//ar1 = *(fft1 + 0);
    VoC_add32_rp(REG45,REG45,REG1,DEFAULT);//ar1 + ai1
    VoC_movreg32(REG67,RL7,IN_PARALLEL);//sin2
    //t = MULSHIFT32(sin2, ar2 + ai2);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);

/////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////

    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_lw32_p(REG45,REG1,DEFAULT);//ai1 = *(fft1 + 1);
    VoC_sub16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_movreg32(REG67,RL6,DEFAULT);//cs2
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

    //MULSHIFT32(cs2, ai1);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start /////////

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15  end ///////////
    VoC_shr32_ri(RL7,-1,IN_PARALLEL);//2*sin2

    VoC_sub32_rr(REG67,RL6,RL7,DEFAULT);//cs2 -= 2*sin2;
    VoC_lw32_p(REG45,REG1,IN_PARALLEL);//ar1 = *(fft1 + 0);

    VoC_sub32_rr(ACC0,ACC1,ACC0,DEFAULT);//z = t - MULSHIFT32(cs2, ai1);
    VoC_lw32_p(RL6,REG2,IN_PARALLEL);//ai2 = *(fft2 + 0);
    VoC_shr32_rr(ACC0,REG0,DEFAULT);//CLIP_2N_SHIFT(z, es);


    //MULSHIFT32(cs2, ar1)
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start /////////

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//*fft2-- = z;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15  end ///////////

    VoC_lw32inc_p(RL7,REG3,DEFAULT);//cs2 = *csptr++;
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//csptr += skipFactor;
    VoC_lw32_p(REG45,REG3,DEFAULT);//sin2 = *csptr;
    VoC_add32_rr(ACC0,ACC1,ACC0,IN_PARALLEL);//z = t + MULSHIFT32(cs2, ar1);
    VoC_push16(REG6,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_lw32_p(REG67,REG2,DEFAULT);//ar2 = *fft2;
    VoC_sub32_rr(ACC1,ACC1,RL6,IN_PARALLEL);//ai2 =0 -ai2;
    VoC_add32_rr(REG67,REG67,ACC1,DEFAULT);//ar2 + ai2;
    VoC_shr32_rr(ACC0,REG0,IN_PARALLEL);//CLIP_2N_SHIFT(z, es);
    VoC_movreg32(RL6,REG45,DEFAULT);//sin2
//t = MULSHIFT32(sin2, ar2 + ai2);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start /////////

    VoC_multf32_rr(ACC0,REG5,REG7,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*fft1++ = z;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);//cs2

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15  end ///////////
    VoC_movreg32(REG67,ACC1,DEFAULT);//ai2
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_movreg32(ACC1,ACC0,DEFAULT);//save t
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    //MULSHIFT32(cs2, ai2)
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start /////////

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shr32_ri(RL6,-1,IN_PARALLEL);//2*sin2
    VoC_macX_rr(REG7,REG4,DEFAULT);
    VoC_sub32_rr(REG67,RL7,RL6,IN_PARALLEL);//cs2 -= 2*sin2;

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15  end ///////////

    VoC_lw32_p(REG45,REG2,DEFAULT);//ar2 = *fft2;
    VoC_movreg32(RL6,RL7,IN_PARALLEL);

    VoC_sub32_rr(RL7,ACC1,ACC0,DEFAULT);//z = t - MULSHIFT32(cs2, ai1);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_shr32_rr(RL7,REG0,DEFAULT);//CLIP_2N_SHIFT(z, es);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    //MULSHIFT32(cs2, ar2)
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start /////////

    VoC_mac32_rr(REG5,REG7,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,IN_PARALLEL);//*fft2-- = z;
    VoC_macX_rr(REG7,REG4,DEFAULT);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15  end ///////////
    VoC_pop16(REG6,DEFAULT);
    VoC_shr32_rr(ACC0,REG0,DEFAULT);//CLIP_2N_SHIFT(z, es);
    VoC_pop32(RL7,DEFAULT);//sin2
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*fft1++ = z;
    VoC_endloop0
    VoC_return;
}

/**************************************************************************************
 * Function:    Cooland_DCT4
 *
 * Description: type-IV DCT
 *
 * Inputs:      REG1->coef;
 *              REG2->gb;
 *              REG3->tabidx;
 *
 * Outputs:    NONE;
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/30/2007
 *
 * Notes:       operates in-place
 *              if number of guard bits in input is < GBITS_IN_DCT4, the input is
 *                scaled (>>) before the DCT4 and rescaled (<<, with clipping) after
 *                the DCT4 (rare)
 *              the output has FBITS_LOST_DCT4 fewer fraction bits than the input
 *              the output will always have at least 1 guard bit (GBITS_IN_DCT4 >= 4)
 *              int bits gained per stage (PreMul + FFT + PostMul)
 *                 short blocks = (-5 + 4 + 2) = 1 total
 *                 long blocks =  (-8 + 7 + 2) = 1 total
 **************************************************************************************/
void Coolsand_DCT4(void)
{
    VoC_lw16i_short(REG0,0,DEFAULT);
    /* fast in-place DCT-IV - adds guard bits if necessary */
    VoC_lw16i_short(REG7,GBITS_IN_DCT4,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_bngt16_rr(DCT4_L0,REG7,REG2)//if (gb < GBITS_IN_DCT4)
    VoC_sub16_rr(REG0,REG7,REG2,DEFAULT);//es = GBITS_IN_DCT4 - gb;
DCT4_L0:
    //REG1:coef;REG0:es;REG3:tabidx;
    VoC_push16(REG3,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_jal(Coolsand_PreMultiplyRescale);
    VoC_lw16_sd(REG3,0,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);

    VoC_jal(Coolsand_R4FFT);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_jal(Coolsand_PostMultiplyRescale);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}

/*********************************dct.c end************************************************/




/*********************************pns.c begin************************************************/


/**************************************************************************************
 * Function:    InvRootR
 *
 * Description: use Newton's method to solve for x = 1/sqrt(r)
 *
 * Inputs:      REG67->r;
 *
 * Outputs:     REG67->x;
 *
 * Used : REG0,REG4,
 *
 * Version 1.0  Created by  Xuml       09/21/2007
 *
 * Notes:       guaranteed to converge and not overflow for any r in this range
 *
 *              xn+1  = xn - f(xn)/f'(xn)
 *              f(x)  = 1/sqrt(r) - x = 0 (find root)
 *                    = 1/x^2 - r
 *              f'(x) = -2/x^3
 *
 *              so xn+1 = xn/2 * (3 - r*xn^2)
 *
 *              NUM_ITER_INVSQRT = 3, maxDiff = 1.3747e-02
 *              NUM_ITER_INVSQRT = 4, maxDiff = 3.9832e-04
 **************************************************************************************/
void Coolsand_InvRootR(void)
{
    /* use linear equation for initial guess
    * x0 = -2*r + 3 (so x0 always >= correct answer in range [0.25, 1))
    * xn = Q29 (at every step)
    */
    VoC_lw16i_short(REG0,0xc,DEFAULT);
    VoC_shru32_ri(REG01,-28,DEFAULT);//X0_COEF_2;
    VoC_movreg32(ACC1,REG67,IN_PARALLEL);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end/////////
    VoC_lw16i_short(REG0,0x6,IN_PARALLEL);
    VoC_shru32_ri(REG01,-28,DEFAULT);//X0_OFF_2;
    VoC_shru32_ri(ACC0,-2,DEFAULT);
    VoC_add32_rr(REG01,ACC0,REG01,DEFAULT);//xn = (MULSHIFT32(r, X0_COEF_2) << 2) + X0_OFF_2;

    VoC_loop_i_short(4,DEFAULT);//for (i = 0; i < NUM_ITER_INVSQRT; i++) {
    VoC_startloop0
    VoC_movreg32(REG67,REG01,DEFAULT);//t = MULSHIFT32(xn, xn);                 /* Q26 = Q29*Q29 */
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end///////////

    VoC_movreg32(REG67,ACC1,DEFAULT);//r
    VoC_movreg32(REG01,ACC0,DEFAULT);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(REG01,RL7,DEFAULT);//xn

    VoC_lw16i_short(RL7_LO,0xc,IN_PARALLEL);

    VoC_shru32_ri(RL7,-24,DEFAULT);//Q26_3;

    VoC_shru32_ri(ACC0,-2,DEFAULT);
    VoC_sub32_rr(REG67,RL7,ACC0,DEFAULT);

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end///////////

    VoC_NOP();
    VoC_shru32_ri(ACC0,-5,DEFAULT);
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_endloop0
    /* clip to range (1.0, 2.0)
     * (because of rounding, this can converge to xn slightly > 2.0 when r is near 0.25)
     */
    VoC_shr32_ri(ACC0,30,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_bez32_r(InvRootR_L0,ACC0);
    VoC_shr32_ri(REG67,-30,DEFAULT);
    VoC_sub32_rd(REG01,REG67,const_1_addr);
InvRootR_L0:
    VoC_movreg32(REG67,REG01,DEFAULT);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ScaleNoiseVector
 *
 * Description: apply scaling to vector of noise coefficients for one scalefactor band
 *
 * Inputs:      REG4->coef;
 *              REG3->sf
 *              REG0->nVals;
 *
 * Outputs:     no
 *
 * Used : REG0,REG4,
 *
 * Version 1.0  Created by  Xuml       09/21/2007
 **************************************************************************************/
void Coolsand_ScaleNoiseVector(void)
{


    VoC_lw16i_short(FORMAT32,8,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);

    VoC_bez16_r(ScaleNoiseVector_L0,REG0);
    /* max nVals = max SFB width = 96, so energy can gain < 2^7 bits in accumulation */
    VoC_loop_r_short(REG0,DEFAULT);
    VoC_startloop0
    VoC_mac32inc_pp(REG2,REG2,DEFAULT);///* spec*spec range = (-2^30, 2^30) */
    VoC_endloop0
ScaleNoiseVector_L0:
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    /* unless nVals == 1 (or the number generator is broken...), this should not happen */
    VoC_bez32_r(ScaleNoiseVector_End,ACC0)///* coef[i] must = 0 for i = [0, nVals-1], so gbMask = 0 */

    /* pow(2, sf/4) * pow(2, FBITS_OUT_DQ_OFF) */

    VoC_push32(ACC1,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_push32(RL6,DEFAULT);
    VoC_push32(RL7,DEFAULT);
    VoC_movreg16(REG6,REG3,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);//save nVals;
    VoC_lw16i(REG2,Tab_pow14_addr);
    VoC_and16_ri(REG6,0x03);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_shr16_ri(REG3,2,IN_PARALLEL);//(sf >> 2)
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16i_short(REG6,FBITS_OUT_DQ_OFF,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);//scalei = (sf >> 2) + FBITS_OUT_DQ_OFF;
    VoC_lw32_p(ACC1,REG2,DEFAULT);//scalef = pow14[sf & 0x3];


    /* energy has implied factor of 2^-8 since we shifted the accumulator
     * normalize energy to range [0.25, 1.0), calculate 1/sqrt(1), and denormalize
     *   i.e. divide input by 2^(30-z) and convert to Q30
     *        output of 1/sqrt(i) now has extra factor of 2^((30-z)/2)
     *        for energy > 0, z is an even number between 0 and 28
     * final scaling of invSqrtEnergy:
     *  2^(15 - z/2) to compensate for implicit 2^(30-z) factor in input
     *  +4 to compensate for implicit 2^-8 factor in input
     */
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);//fro CLS input.


    // Inputs:      RL6->x;  Outputs:     REG6->return
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_rd(REG5,REG6,const_2_addr);// z = CLZ(energy) - 2;                    /* energy has at least 2 leading zeros (see acc loop) */
    VoC_and16_ri(REG5,0xfffe);
    VoC_sub16_dr(REG0,const_0_addr,REG5);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);
    VoC_shr32_rr(REG67,REG0,DEFAULT);
    VoC_jal(Coolsand_InvRootR);
    VoC_movreg32(ACC0,REG67,DEFAULT);//invSqrtEnergy = InvRootR(energy << z);   /* energy << z must be in range [0x10000000, 0x40000000] */
    VoC_lw16i_short(REG6,19,IN_PARALLEL);
    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);// scalei -= (15 - z/2 + 4);               /* nInt = 1/sqrt(energy) in Q29 */

    /* normalize for final scaling */
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);//fro CLS input.
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_dr(REG6,const_1_addr,REG6);//z = CLZ(invSqrtEnergy) - 1;
    VoC_shr32_rr(ACC0,REG6,DEFAULT);//invSqrtEnergy <<= z;
    VoC_add16_rd(REG6,REG6,const_5_addr);
    VoC_add16_rr(REG5,REG6,REG3,DEFAULT);//scalei -= (z - 3 - 2);   /* -2 for scalef, z-3 for invSqrtEnergy */

    VoC_movreg32(REG01,ACC0,DEFAULT);//load invSqrtEnergy
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);//load  scalef

/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end///////////

    VoC_lw16i_short(REG0,-31,IN_PARALLEL);
    VoC_sub16_dr(REG5,const_0_addr,REG5);//-scalei;
    VoC_bngt16_rr(ScaleNoiseVector_L1,REG0,REG5)//if(scalei<-31)scalei=-31;
    VoC_movreg16(REG5,REG0,DEFAULT);
ScaleNoiseVector_L1:

    VoC_lw32_d(RL6,LOCAL_PNS_gbMask_addr);
    VoC_pop16(REG0,DEFAULT);
    VoC_movreg16(REG2,REG4,IN_PARALLEL);//&coef[0];

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_bgtz16_r(ScaleNoiseVector_L2,REG5)
    VoC_movreg16(REG4,REG5,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
ScaleNoiseVector_L2:

    VoC_loop_r_short(REG0,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32_p(REG01,REG2,DEFAULT);
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);
    VoC_shr32_rr(REG01,REG4,DEFAULT);
    // c = MULSHIFT32(coef[i], scalef) >> scalei;
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////
    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG0,DEFAULT);
/////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 end///////////
    VoC_NOP();
    VoC_shr32_rr(ACC0,REG5,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,31,IN_PARALLEL);
    VoC_xor32_rr(REG67,ACC0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,IN_PARALLEL);//coef[i] = c;
    VoC_sub32_rr(REG67,REG67,ACC0,DEFAULT);
    VoC_or32_rr(RL6,REG67,DEFAULT);//gbMask |= FASTABS(c);
    VoC_endloop0

    VoC_pop32(RL7,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_pop32(RL6,DEFAULT);
    VoC_pop32(ACC1,DEFAULT);
    VoC_sw32_d(RL6,LOCAL_PNS_gbMask_addr);
ScaleNoiseVector_End:
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_GenerateNoiseVector
 *
 * Description: create vector of noise coefficients for one scalefactor band
 *
 * Inputs:      REG4-> coef
 *              REG0-> nVals
 *
 * Outputs:     no
 *
 * Used : REG0,REG4,
 *
 * Version 1.0  Created by  Xuml       09/20/2007
 **************************************************************************************/
void Coolsand_GenerateNoiseVector(void)
{

    VoC_bngtz16_r(GenerateNoiseVector_L0,REG0);

    VoC_push32(REG01,DEFAULT);
    VoC_push16(REG6,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);

    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
    VoC_push32(RL7,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_movreg16(REG2,REG4,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_pnsLastVal_addr,2)


    VoC_lw16i(ACC1_LO,0xf35f);//1013904223U
    VoC_lw16i(ACC1_HI,0x3c6e);


    /* use same coefs as MPEG reference code (classic LCG)
     * use unsigned multiply to force reliable wraparound behavior in C (mod 2^32)
     */

    VoC_lw16i(STATUS,CARRY_ON);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_loop_r_short(REG0,DEFAULT);
    VoC_startloop0

    VoC_lw32_p(ACC0,REG3,DEFAULT)//unsigned int r = *last;

    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_shru32_ri(ACC0,-16,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shru32_ri(RL6,-2,IN_PARALLEL);

    VoC_movreg32(RL7,RL6,DEFAULT);
    VoC_shru32_ri(RL6,-1,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shru32_ri(RL6,-7,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shru32_ri(RL6,-4,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_shru32_ri(RL6,-6,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_shr32_ri(ACC0,16,IN_PARALLEL);

    VoC_sw32_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//coef[i]
    VoC_endloop0



    VoC_lw16i_short(STATUS,CARRY_OFF,DEFAULT);

    VoC_pop32(RL7,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);
    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);
GenerateNoiseVector_L0:
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_CopyNoiseVector
 *
 * Description: copy vector of noise coefficients for one scalefactor band from L to R
 *
 * Inputs:       REG4->coef;    REG7->psi->coef[1] + (coef - psi->coef[0]);  REG0-> width
 *
 * Outputs:     no
 *
 * Used : REG2,REG3,ACC0,INC2,INC3
 *
 * Version 1.0  Created by  Xuml       09/20/2007
 **************************************************************************************/
void Coolsand_CopyNoiseVector(void)
{
    VoC_push32(REG23,DEFAULT);
    VoC_movreg16(REG2,REG4,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_bngtz16_r(CopyNoiseVecto_L0,REG0);
    VoC_loop_r_short(REG0,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0
CopyNoiseVecto_L0:
    VoC_pop32(REG23,DEFAULT);
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_PNS
 *
 * Description: apply perceptual noise substitution, if enabled (MPEG-4 only)
 *
 * Inputs:      REG1-> ch
 *
 * Outputs:     REG0->0 if successful, -1 if error
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC1,RL6_HI,RL7
 *
 * Version 1.0  Created by  Xuml       09/20/2007
 **************************************************************************************/

void Coolsand_PNS(void)
{
#if 0

    voc_short LOCAL_PNS_gp_M_maxSFB_addr,x
    voc_short LOCAL_PNS_CH_addr,x
    voc_short LOCAL_PNS_maxSFB_addr, 1, x
    voc_short LOCAL_PNS_gbMask_addr ,2 ,x
    voc_short LOCAL_PNS_numWinGroup_addr, 1, x
    voc_short LOCAL_PNS_checkCorr_addr, 1, x
    voc_short LOCAL_PNS_genNew_addr, 1, x
    voc_short LOCAL_PNS_nSamps_addr, 1, x
    voc_short LOCAL_PNS_gbCurrent_addr_addr, 1, x

#endif

    VoC_lw16i_short(FORMAT16,3,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    //icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);
    VoC_lw16_d(REG3,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr);
    VoC_bne16_rd(Coolsand_PNS_L0,REG1,const_1_addr);  //if ch == 1;
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr+1);
    VoC_be16_rd(Coolsand_PNS_L0,REG3,const_1_addr);  //if psi->commonWin == 1;
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,1);
Coolsand_PNS_L0:
    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_bnez16_r(Coolsand_PNS_L0_sub0,REG4);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_jump(Coolsand_PNS_End)
Coolsand_PNS_L0_sub0:
    VoC_sw32_d(RL6,LOCAL_PNS_gbMask_addr);

    VoC_inc_p(REG0,DEFAULT); //&icsInfo->winSequence
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_movreg16(REG6,REG1,DEFAULT);
    VoC_lw16i_short(INC0,3,IN_PARALLEL);

    VoC_lw16inc_p(REG3,REG0,DEFAULT);   //icsInfo->winSequence
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//icsInfo->maxSFB;
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);//FORMAT32=0,

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//icsInfo->numWinGroup;
    VoC_lw16i_short(FORMATX,15,IN_PARALLEL);//FORMATX=15

    VoC_sw16_d(REG4,LOCAL_PNS_maxSFB_addr);
    VoC_sw16_d(REG5,LOCAL_PNS_numWinGroup_addr);

    VoC_lw16i(REG4,Tab_sfBandTabLongOffset_addr);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabLong_addr,1);
    VoC_lw16i(REG7,NSAMPS_LONG);//nSamps = NSAMPS_LONG;

    VoC_bne16_rd(Coolsand_PNS_L1,REG3,const_2_addr); //if (icsInfo->winSequence == 2)
    VoC_lw16i(REG4,Tab_sfBandTabShortOffset_addr);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabShort_addr,1);
    VoC_lw16i(REG7,NSAMPS_SHORT);//nSamps = NSAMPS_SHORT;
Coolsand_PNS_L1:
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);//&sfBandTabLongOffset[psi->sampRateIdx]
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_add16_rp(REG1,REG1,REG4,DEFAULT); //sfbTab
    VoC_sw16_d(REG7,LOCAL_PNS_nSamps_addr);

    VoC_sw16_d(REG6,LOCAL_PNS_CH_addr);


    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);// AAC_ID_CPE

    VoC_sw16_d(REG2,LOCAL_PNS_checkCorr_addr);

    VoC_bne16_rd(Coolsand_PNS_L1_sub0,REG3,STRUCT_AACDecInfo_currBlockID_addr);
    VoC_bne16_rd(Coolsand_PNS_L1_sub0,REG3,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_sw16_d(REG3,LOCAL_PNS_checkCorr_addr);
Coolsand_PNS_L1_sub0:



    VoC_bez16_r(Coolsand_PNS_L2,REG6); //if ch

    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_coef1_addr);//coef = psi->coef[ch];
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1);//sfbCodeBook = psi->sfbCodeBook[ch];
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr,1);  //scaleFactors = psi->scaleFactors[ch];
    VoC_lw16i(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
    VoC_jump(Coolsand_PNS_L3);
Coolsand_PNS_L2:
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_coef_addr);
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr,1);
    VoC_lw16i(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
Coolsand_PNS_L3:

    VoC_movreg32(ACC1,REG23,DEFAULT);//load sfbCodeBook and scaleFactors
    VoC_sw16_d(REG6,LOCAL_Dequantize_gbCurrent_addr_addr);
    VoC_sw16_d(REG1, LOCAL_Dequantize_sfbTab_addr_addr);
    VoC_lw32z(RL7,DEFAULT);//// sfb=0 and gp=0;

Coolsand_PNS_L10:

    //for (gp = 0; gp < icsInfo->numWinGroup; gp++)

    VoC_movreg16(REG6,RL7_HI,DEFAULT); //load gp
    VoC_lw16_d(REG5,LOCAL_PNS_numWinGroup_addr);
    VoC_bngt16_rr(Coolsand_PNS_L11,REG5,REG6);


    VoC_lw16inc_p(REG5,REG0,DEFAULT); //icsInfo->winGroupLen[gp] gp++;
    VoC_push16(REG0,DEFAULT);

    VoC_loop_r(1,REG5);
    VoC_mult16_rd(REG6,REG6,LOCAL_PNS_maxSFB_addr);//gp*icsInfo->maxSFB
    VoC_lw16_d(REG1,LOCAL_Dequantize_sfbTab_addr_addr);
    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_shr16_ri(REG6,4,IN_PARALLEL);
    VoC_sw16_d(REG6,LOCAL_PNS_gp_M_maxSFB_addr);
    VoC_lw16i(REG0,STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr);
    VoC_add16_rr(REG0,REG6,REG0,DEFAULT);//msMaskPtr = psi->msMaskBits + ((gp*icsInfo->maxSFB) >> 3);
    VoC_and16_ri(REG5,0x0f);//msMaskOffset = ((gp*icsInfo->maxSFB) & 0x07);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);//(*msMaskPtr++)
    VoC_movreg16(REG2,ACC1_LO,IN_PARALLEL);//load sfbCodeBook

    VoC_shru16_rr(REG6,REG5,DEFAULT);//msMask = (*msMaskPtr++) >> msMaskOffset;
    VoC_movreg16(RL6_HI,REG5,IN_PARALLEL); //msMaskOffset
    VoC_movreg16(RL6_LO,REG0,DEFAULT);  //  msMaskPtr
    VoC_movreg16(REG2,ACC1_LO,IN_PARALLEL);
    VoC_lw16_d(REG1,LOCAL_Dequantize_sfbTab_addr_addr);

Coolsand_PNS_L5:

    VoC_movreg16(REG0,RL7_LO,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);


    VoC_lw16_d(REG5,LOCAL_PNS_maxSFB_addr);
    VoC_bngt16_rr(Coolsand_PNS_L4,REG5,REG0);//for (sfb = 0; sfb < icsInfo->maxSFB; sfb++)

    VoC_lw16inc_p(REG5,REG2,DEFAULT);//(sfbCodeBook[sfb]);
    VoC_lw16inc_p(REG0,REG1,DEFAULT);// sfbTab[sfb];
    VoC_sub16_pr(REG0,REG1,REG0,DEFAULT);//width = sfbTab[sfb+1] - sfbTab[sfb];
    VoC_push16(REG4,DEFAULT);//save coef
    VoC_push16(REG6,DEFAULT);// save msMask
    VoC_push32(REG01,IN_PARALLEL)//save width and &sfbTab[sfb+1]
    VoC_push16(REG2,DEFAULT);
    VoC_lw16i_short(REG7,13,IN_PARALLEL);


    VoC_bne16_rr(Coolsand_PNS_L6,REG5,REG7);//if (sfbCodeBook[sfb] == 13)
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_bnez16_d(Coolsand_PNS_L7,LOCAL_PNS_CH_addr);

    /* generate new vector, copy into ch 1 if it's possible that the channels will be correlated
    * if ch 1 has PNS enabled for this SFB but it's uncorrelated (i.e. ms_used == 0),
    *    the copied values will be overwritten when we process ch 1
    */

    //  REG4-> coef     REG0-> width
    VoC_jal(Coolsand_GenerateNoiseVector);


    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_lw16i(REG5,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr);
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);
    VoC_add16_rd(REG5,REG5,LOCAL_PNS_gp_M_maxSFB_addr);//psi->sfbCodeBook[1][gp*icsInfo->maxSFB + sfb]
    VoC_bez16_d(Coolsand_PNS_L4_sub0,LOCAL_PNS_checkCorr_addr);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_bne16_rd(Coolsand_PNS_L4_sub0,REG5,const_13_addr);
    VoC_lw16i(REG6,STRUCT_AACDecInfo_PSInfoBase_coef_addr);
    VoC_lw16i(REG7,STRUCT_AACDecInfo_PSInfoBase_coef1_addr);
    VoC_sub16_rr(REG6,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);  //psi->coef[1] + (coef - psi->coef[0])
    //  REG4->coef; REG7->psi->coef[1] + (coef - psi->coef[0]);  REG0-> width
    VoC_jal(Coolsand_CopyNoiseVector);
Coolsand_PNS_L4_sub0:

    VoC_jump(Coolsand_PNS_L7_sub3);
Coolsand_PNS_L7:
    /* generate new vector if no correlation between channels */

    VoC_sw16_d(REG7,LOCAL_PNS_genNew_addr);//genNew = 1;

    //if (checkCorr && psi->sfbCodeBook[0][gp*icsInfo->maxSFB + sfb] == 13)

    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_lw16i(REG5,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr);
    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);
    VoC_add16_rd(REG5,REG5,LOCAL_PNS_gp_M_maxSFB_addr);//psi->sfbCodeBook[0][gp*icsInfo->maxSFB + sfb]
    VoC_bez16_d(Coolsand_PNS_L7_sub0,LOCAL_PNS_checkCorr_addr);
    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_bne16_rd(Coolsand_PNS_L7_sub0,REG5,const_13_addr);

    //if ( (psi->msMaskPresent == 1 && (msMask & 0x01)) || psi->msMaskPresent == 2 )
    //  genNew = 0;
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,0x01);
    VoC_bez16_r(Coolsand_PNS_L7_sub1,REG6);
    VoC_bne16_rd(Coolsand_PNS_L7_sub1,REG7,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr)
    VoC_sw16_d(REG5,LOCAL_PNS_genNew_addr);

Coolsand_PNS_L7_sub1:
    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_bne16_rd(Coolsand_PNS_L7_sub0,REG7,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr);
    VoC_sw16_d(REG5,LOCAL_PNS_genNew_addr);

Coolsand_PNS_L7_sub0:
    //if (genNew) GenerateNoiseVector(coef, &psi->pnsLastVal, width);
    VoC_bez16_d(Coolsand_PNS_L7_sub3,LOCAL_PNS_genNew_addr)
    //  REG4-> coef     REG0-> width
    VoC_jal(Coolsand_GenerateNoiseVector);
Coolsand_PNS_L7_sub3:

    VoC_movreg16(REG7,RL7_LO,DEFAULT);  //sfb
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG7,DEFAULT);
    VoC_add16_rd(REG5,REG5,LOCAL_PNS_gp_M_maxSFB_addr);//$psi->scaleFactors[ch][gp*icsInfo->maxSFB + sfb]
    VoC_NOP();
    VoC_lw16_p(REG3,REG5,DEFAULT);

    //  REG4-> coef     REG0-> width   REG5:psi->scaleFactors[ch][gp*icsInfo->maxSFB + sfb]
    VoC_jal(Coolsand_ScaleNoiseVector);


Coolsand_PNS_L6:

    VoC_pop16(REG2,DEFAULT);
    VoC_pop16(REG6,DEFAULT); // load msMask
    VoC_pop32(REG01,IN_PARALLEL);  //load width and &sfbTab[sfb+1]
    VoC_pop16(REG4,DEFAULT);  // load coef
    VoC_shr16_ri(REG0,-1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);

    VoC_movreg16(REG0,RL6_LO,DEFAULT); //msMaskPtr;
    VoC_movreg16(REG5,RL6_HI,IN_PARALLEL);//msMaskOffset;
    VoC_shru16_ri(REG6,1,DEFAULT);//msMask >>= 1;
    VoC_add16_rd(REG5,REG5,const_1_addr);//++msMaskOffset

    VoC_bne16_rd(Coolsand_PNS_L8,REG5,const_16_addr);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);//msMask = *msMaskPtr++;
    VoC_lw16i_short(REG5,0,DEFAULT);//msMaskOffset = 0;
Coolsand_PNS_L8:
    VoC_movreg16(RL6_LO,REG0,DEFAULT);  //  msMaskPtr
    VoC_movreg16(RL6_HI,REG5,IN_PARALLEL); //msMaskOffset

    VoC_movreg16(REG0,RL7_LO,DEFAULT); //load sfb;
    VoC_add16_rd(REG0,REG0,const_1_addr); // sfb++;
    VoC_movreg16(RL7_LO,REG0,DEFAULT); //bake sfb;

    VoC_jump(Coolsand_PNS_L5);
Coolsand_PNS_L4:

    VoC_lw16_d(REG7,LOCAL_PNS_nSamps_addr);
    VoC_lw16i_short(RL7_LO,0,DEFAULT);  // sfb=0
    VoC_sub16_rp(REG6,REG7,REG1,IN_PARALLEL);//nSamps - sfbTab[icsInfo->maxSFB]
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//coef += (nSamps - sfbTab[icsInfo->maxSFB]);

    VoC_endloop1

    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_add16_rd(REG6,REG6,const_1_addr);//gp++
    VoC_movreg16(RL7_HI,REG6,DEFAULT);

    VoC_movreg32(ACC1,REG23,DEFAULT);//load sfbCodeBook and scaleFactors
    VoC_pop16(REG0,IN_PARALLEL);
    VoC_jump(Coolsand_PNS_L10);
Coolsand_PNS_L11:

    /* update guard bit count if necessary */

    VoC_lw16i(REG0,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
    VoC_bez16_d(Coolsand_PNS_L12,LOCAL_PNS_CH_addr);
    VoC_lw16i(REG0,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
Coolsand_PNS_L12:


    VoC_lw32_d(RL6,LOCAL_PNS_gbMask_addr);

    //RL6:gbmask REG6:return
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_rd(REG6,REG6,const_1_addr);//gb = CLZ(gbMask) - 1;


    VoC_lw16_p(REG1,REG0,DEFAULT);
    VoC_bngt16_rr(Coolsand_PNS_End,REG1,REG6)//if (psi->gbCurrent[ch] > gb)
    VoC_sw16_p(REG6,REG0,DEFAULT);
Coolsand_PNS_End:
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_return;
}


/*********************************pns.c end************************************************/











/*********************************decelmnt.c begin************************************************/

/**************************************************************************************
 * Function:    DecodeSingleChannelElement
 *
 * Description: decode one SCE
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : REG2,REG4,RL6
 *
 * Version 1.0  Create by  Xuml       08/22/2007
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
 **************************************************************************************/
void Coolsand_DecodeSingleChannelElement(void)
{
    /* read instance tag */
    //aacDecInfo->currInstTag = GetBits(bsi, NUM_INST_TAG_BITS);
    VoC_rbinc_i(REG4,NUM_INST_TAG_BITS,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_sw16_d(REG4,STRUCT_AACDecInfo_currInstTag_addr);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_DecodeChannelPairElement
 *
 * Description: decode one CPE
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : REG0,REG1,REG2,REG4,REG5,REG6,REG7,RL6
 *
 * Version 1.0  Create by  Xuml       07/06/2007
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
 **************************************************************************************/
void Coolsand_DecodeChannelPairElement(void)
{
    /* read instance tag */
    VoC_rbinc_i(REG4,NUM_INST_TAG_BITS,DEFAULT);
    VoC_rbinc_i(REG5,1,DEFAULT);
    VoC_lbinc_p(REG2);
    VoC_sw16_d(REG4,STRUCT_AACDecInfo_currInstTag_addr);  //aacDecInfo->currInstTag = GetBits(bsi, NUM_INST_TAG_BITS);
    VoC_sw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr); //psi->commonWin = GetBits(bsi, 1);

    /* read common window flag and mid-side info (if present)
     * store msMask bits in psi->msMaskBits[] as follows:
     *  long blocks -  pack bits for each SFB in range [0, maxSFB) starting with lsb of msMaskBits[0]
     *  short blocks - pack bits for each SFB in range [0, maxSFB), for each group [0, 7]
     * msMaskPresent = 0 means no M/S coding
     *               = 1 means psi->msMaskBits contains 1 bit per SFB to toggle M/S coding
     *               = 2 means all SFB's are M/S coded (so psi->msMaskBits is not needed)
     */
//  VoC_lw16i_short(REG1,1,DEFAULT);

    VoC_bez16_r(DecodeChannelPairElement_End,REG5);//if (psi->commonWin)
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);

    VoC_jal(Coolsand_DecodeICSInfo);  //DecodeICSInfo(bsi, icsInfo, psi->sampRateIdx);


    VoC_rbinc_i(REG7,2,DEFAULT); //psi->msMaskPresent = GetBits(bsi, 2);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_lbinc_p(REG2);
    VoC_sw16_d(REG7,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr); //psi->msMaskPresent = GetBits(bsi, 2);

    VoC_bne16_rd(DecodeChannelPairElement_End,REG7,const_1_addr);//if (psi->msMaskPresent == 1)

    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr,1);
    VoC_lw16_d(REG7,STRUCT_AACDecInfo_PSInfoBase_icsInfo_numWinGroup_addr);  //load icsInfo->numWinGroup
    VoC_lw16_d(REG3,STRUCT_AACDecInfo_PSInfoBase_icsInfo_maxSFB_addr);  //load icsInfo->maxSFB

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw32z(REG45,IN_PARALLEL); //maskOffset = 0;  //*maskPtr = 0;

    VoC_bngtz16_r(DecodeChannelPairElement_End,REG7);
    VoC_bngtz16_r(DecodeChannelPairElement_End,REG3);
    VoC_loop_r(1,REG7); //for (gp = 0; gp < icsInfo->numWinGroup; gp++)
    VoC_loop_r_short(REG3,DEFAULT); //for (sfb = 0; sfb < icsInfo->maxSFB; sfb++)
    VoC_startloop0
    VoC_rbinc_i(REG7,1,DEFAULT);  //currBit = (unsigned char)GetBits(bsi, 1);
    VoC_shru16_rr(REG7,REG4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);//++maskOffset;
    VoC_or16_rr(REG5,REG7,DEFAULT);  //*maskPtr |= currBit << maskOffset;
    VoC_lw16i_short(REG1,-16,IN_PARALLEL);  // short align for voc
    VoC_bne16_rr(DecodeChannelPairElement_L0,REG4,REG1);  //if (maskOffset == 16)
    VoC_lw32z(REG45,DEFAULT);  //maskOffset = 0;//*maskPtr = 0;
    VoC_sw16inc_p(REG5,REG0,IN_PARALLEL); //maskPtr++;
    VoC_lbinc_p(REG2);
DecodeChannelPairElement_L0:
    VoC_sw16_p(REG5,REG0,DEFAULT); //save previous maskPtr;
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

DecodeChannelPairElement_End:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_return;

}

/**************************************************************************************
 * Function:    DecodeLFEChannelElement
 *
 * Description: decode one LFE
 *
 * Inputs:      BitStreamInfo struct pointing to start of LFE (14496-3, table 4.4.9)
 *
 * Outputs:     updated element instance tag
 *
 * Return:      0 if successful, -1 if error
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
// **************************************************************************************/
//static int DecodeLFEChannelElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
//{
//  /* validate pointers */
//  if (!aacDecInfo || !aacDecInfo->psInfoBase)
//      return -1;
//  /* read instance tag */
//  aacDecInfo->currInstTag = GetBits(bsi, NUM_INST_TAG_BITS);
//  return 0;
//}

/**************************************************************************************
 * Function:    Coolsand_DecodeFillElement
 *
 * Description: decode one fill element
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,RL6
 *
 * Version 1.0  Create by  Xuml       07/06/2007
 *
 **************************************************************************************/

void Coolsand_DecodeFillElement(void)
{
    VoC_rbinc_i(REG4,4,DEFAULT);  //fillCount = GetBits(bsi, 4);
    VoC_lw16i_short(REG1,-1,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_bne16_rd(DecodeFillElement_L0,REG4,const_15_addr); //if (fillCount == 15)
    VoC_rbinc_i(REG5,8,DEFAULT);     //(GetBits(bsi, 8)
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);
    VoC_lbinc_p(REG2);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//fillCount += (GetBits(bsi, 8) - 1);
DecodeFillElement_L0:
    VoC_lw16i_short(REG0,0,DEFAULT);
    /* fill elements don't have instance tag */
    VoC_sw16_d(REG1,STRUCT_AACDecInfo_currInstTag_addr);//aacDecInfo->currInstTag = -1; /* fill elements don't have instance tag */

DecodeFillElement_L1:

    VoC_bngtz16_r(DecodeFillElement_End,REG4);
    VoC_rbinc_i(REG5,8,DEFAULT);  //GetBits(bsi, 8);
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);
    VoC_lbinc_p(REG2);
    //VoC_sw16inc_p(REG5,REG1,DEFAULT);  //*fillBuf++ = GetBits(bsi, 8);
    VoC_jump(DecodeFillElement_L1)
DecodeFillElement_End:

    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_DecodeNextElement
 *
 * Description: decode next syntactic element in AAC frame
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,ACC0,ACC1_lo
 *
 * Version 1.0  Create by  Xuml       07/04/2007
 *
 **************************************************************************************/

void Coolsand_DecodeNextElement(void)
{


    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);// return

    /* read element ID (save last ID for SBR purposes) */
    VoC_lw16_d(REG6,STRUCT_AACDecInfo_currBlockID_addr);
    VoC_rbinc_i(REG4,NUM_SYN_ID_BITS,DEFAULT);

    VoC_lbinc_p(REG2);
    /* set defaults (could be overwritten by DecodeXXXElement(), depending on currBlockID) */
    VoC_sw16_d(REG0,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_sw16_d(REG6,STRUCT_AACDecInfo_prevBlockID_addr);
    VoC_sw16_d(REG4,STRUCT_AACDecInfo_currBlockID_addr);

    VoC_bnez16_r(DecodeNextElement_L0,REG4);
    VoC_jal(Coolsand_DecodeSingleChannelElement);
    VoC_jump(DecodeNextElement_L6);
DecodeNextElement_L0:
    VoC_bne16_rd(DecodeNextElement_L1,REG4,const_1_addr);//AAC_ID_CPE
    VoC_jal(Coolsand_DecodeChannelPairElement);
    VoC_jump(DecodeNextElement_L6);
DecodeNextElement_L1:
    VoC_bne16_rd(DecodeNextElement_L2,REG4,const_2_addr);//AAC_ID_CCE
//  VoC_jal(Coolsand_?);
    VoC_jump(DecodeNextElement_L6);
DecodeNextElement_L2:
    VoC_lw16i_short(REG6,3,DEFAULT); // load AAC_ID_LFE
    VoC_lw16i_short(REG7,4,IN_PARALLEL); // load AAC_ID_DSE
    VoC_bne16_rr(DecodeNextElement_L3,REG4,REG6);
    //VoC_jal(Coolsand_DecodeLFEChannelElement);
    VoC_jump(DecodeNextElement_L6);
DecodeNextElement_L3:
    VoC_bne16_rr(DecodeNextElement_L4,REG4,REG7);
//  VoC_jal(Coolsand_DecodeDataStreamElement);
    VoC_jump(DecodeNextElement_L6);
DecodeNextElement_L4:
    VoC_lw16i_short(REG6,5,DEFAULT); // load AAC_ID_PCE
    VoC_lw16i_short(REG7,6,IN_PARALLEL); // load AAC_ID_FIL
    VoC_bne16_rr(DecodeNextElement_L5,REG4,REG6);
    //VoC_jal(Coolsand_DecodeProgramConfigElement);
DecodeNextElement_L5:
    VoC_bne16_rr(DecodeNextElement_L6,REG4,REG7);
    VoC_jal(Coolsand_DecodeFillElement);
DecodeNextElement_L6:

    VoC_pop16(RA,DEFAULT);

    //err judge
    VoC_bez16_r(DecodeNextElement_End,REG0);
    VoC_lw16i_short(REG0,-8,DEFAULT);//return ERR_AAC_SYNTAX_ELEMENT;
DecodeNextElement_End:
    VoC_return;

}
/*********************************decelmnt.c end************************************************/

/*********************************dequant.c begin************************************************/

#define SF_OFFSET           100

/**************************************************************************************
 * Function:    Coolsand_DequantBlock
 *
 * Description: dequantize one block of transform coefficients (in-place)
 *
 * Inputs:      REG4-> inbuf; (quantized transform coefficients, range = [0, 8191])
 *              REG6-> nSamps; (number of samples to dequantize)
 *              REG0-> scale;  (scalefactor for this block of data, range = [0, 256])
 *
 * Outputs:     REG4-> updated inbuf
 *
 * Used : all
 *
 * Version 1.0  Create by  Xuml       07/16/2007
 *
 * Notes:       applies dequant formula y = pow(x, 4.0/3.0) * pow(2, (scale - 100)/4.0)
 *                * pow(2, FBITS_OUT_DQ_OFF)
 *              clips outputs to Q(FBITS_OUT_DQ_OFF)
 *              output has no minimum number of guard bits
 **************************************************************************************/
void Coolsand_DequantBlock(void)
{

#if 0

    voc_word  LOCAL_DequantBlock_tab4_addr,4,x
    voc_short LOCAL_Dequantize_sfbTab_addr_addr,x
    voc_short LOCAL_Dequantize_gbCurrent_addr_addr,x
    voc_short LOCAL_Dequantize_numWinGroup_addr,x
    voc_short LOCAL_Dequantize_maxSFB_addr,x
    voc_word  LOCAL_Dequantize_gbMask_addr,x

#endif

    VoC_bngtz16_r(DequantBlock_End,REG6);//if (nSamps <= 0)

    VoC_lw16i(REG1,SF_OFFSET);
    VoC_push16(RA,DEFAULT);

    VoC_sub16_rr(REG0,REG0,REG1,DEFAULT);//scale -= SF_OFFSET; /* new range = [-100, 156] */
    VoC_lw16i_short(INC1,2,IN_PARALLEL);

    /* with two's complement numbers, scalei/scalef factorization works for pos and neg values of scale:
     *  [+4...+7] >> 2 = +1, [ 0...+3] >> 2 = 0, [-4...-1] >> 2 = -1, [-8...-5] >> 2 = -2 ...
     *  (-1 & 0x3) = 3, (-2 & 0x3) = 2, (-3 & 0x3) = 1, (0 & 0x3) = 0
     *
     * Example: 2^(-5/4) = 2^(-1) * 2^(-1/4) = 2^-2 * 2^(3/4)
     */

    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG2,16,IN_PARALLEL);
    VoC_and16_ri(REG0,0x0003);
    VoC_shr16_ri(REG0,-1,DEFAULT);
    VoC_lw16i(REG3,Tab_pow14_addr);
    VoC_mult16_rr(REG2,REG2,REG0,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL);
    VoC_lw16i(REG5,Tab_pow43_14_addr);
    VoC_lw32_p(ACC1,REG3,DEFAULT);//scalef = pow14[scale & 0x3];
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);//tab16 = pow43_14[scale & 0x3];
    VoC_shr16_ri(REG1,2,DEFAULT);//(scale >> 2)
    VoC_lw16i_short(REG2,FBITS_OUT_DQ_OFF,IN_PARALLEL);
    VoC_add16_rr(REG7,REG1,REG2,DEFAULT);//scalei = (scale >> 2) + FBITS_OUT_DQ_OFF;
    VoC_lw16i_short(REG0,28,IN_PARALLEL);


    /* cache first 4 values:
     * tab16[j] = Q28 for j = [0,3]
     * tab4[x] = x^(4.0/3.0) * 2^(0.25*scale), Q(FBITS_OUT_DQ_OFF)
     */

    VoC_sub16_rr(REG0,REG0,REG7,DEFAULT);//shift = 28 - scalei;
    VoC_lw16i_short(REG2,-31,IN_PARALLEL);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG1,REG5,IN_PARALLEL);//load tab16
    VoC_lw16i_set_inc(REG3,LOCAL_DequantBlock_tab4_addr,2);

    VoC_bgtz16_r(DequantBlock_L1,REG0);//else if (shift <= 0)

    VoC_bngt16_rr(DequantBlock_L2,REG2,REG0);//if (shift < -31)
    VoC_movreg16(REG0,REG2,DEFAULT);
DequantBlock_L2:
    VoC_loop_i_short(2,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);// y = tab16[x];
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_shr32_rr(ACC0,REG0,IN_PARALLEL);//  y <<= shift;;
    VoC_shr32_rr(RL6,REG0,DEFAULT);//   y <<= shift;;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);  //tab4[x] = y;
    VoC_sw32inc_p(RL6,REG3,DEFAULT);  //tab4[x] = y;
    VoC_endloop0
    VoC_jump(DequantBlock_L5);

DequantBlock_L1:   //   else
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//tab4[0] = 0;
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    VoC_shr32_rr(ACC0,REG0,DEFAULT);//tab16[1] >> shift;
    VoC_lw32inc_p(RL6,REG1,IN_PARALLEL);
    VoC_lw32inc_p(RL7,REG1,DEFAULT);
    VoC_shr32_rr(RL6,REG0,IN_PARALLEL);//tab16[2] >> shift;
    VoC_shr32_rr(RL7,REG0,DEFAULT);//tab16[3] >> shift;
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL)//tab4[1] = tab16[1] >> shift;
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//tab4[2] = tab16[2] >> shift;
    VoC_sw32_p(RL7,REG3,DEFAULT);//tab4[3] = tab16[3] >> shift;
DequantBlock_L5:
    VoC_movreg16(REG3,REG4,DEFAULT); //bake inbuf
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_lw16i(REG4,LOCAL_DequantBlock_tab4_addr);

    VoC_loop_r(0,REG6);//gbMask = 0;do {}while (--nSamps);
    VoC_lw16_p(REG1,REG3,DEFAULT);//iSamp = *inbuf;
    VoC_movreg16(RL7_LO,REG3,IN_PARALLEL); //bake inbuf;

    VoC_movreg16(RL7_HI,REG1,DEFAULT); //bake iSamp;

    //  x = FASTABS(iSamp);
    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_shr16_ri(REG1,15,IN_PARALLEL);
    VoC_xor16_rr(REG0,REG1,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG0,REG1,DEFAULT);//x = FASTABS(iSamp);

    VoC_add16_rr(REG6,REG0,REG4,DEFAULT);//&tab4[x]
    VoC_add16_rr(REG6,REG0,REG6,DEFAULT);//&tab4[x]
    VoC_bngt16_rr(DequantBlock_L6,REG2,REG0);//if (x < 4)
    VoC_lw32_p(ACC0,REG6,DEFAULT);//y = tab4[x];
    VoC_jump(DequantBlock_L16);
DequantBlock_L6:
    VoC_add16_rr(REG6,REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG2,16,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG0,IN_PARALLEL);//&tab16[x]
    VoC_bngt16_rr(DequantBlock_L7,REG2,REG0);//if (x < 16)
    /* result: y = Q25 (tab16 = Q25) */
    VoC_lw16i_short(REG3,25,DEFAULT);
    VoC_lw32_p(ACC0,REG6,DEFAULT);//y = tab16[x];
    VoC_sub16_rr(REG3,REG3,REG7,IN_PARALLEL);//shift = 25 - scalei;
    VoC_jump(DequantBlock_L13);
DequantBlock_L7:
    VoC_sub16_rr(REG6,REG0,REG2,DEFAULT);
    VoC_shr16_ri(REG2,-2,DEFAULT);//load 64
    VoC_shr16_ri(REG6,-1,IN_PARALLEL)////[x-16]*2
    VoC_bngt16_rr(DequantBlock_L8,REG2,REG0);//if (x < 64);
    /* result: y = Q21 (pow43tab[j] = Q23, scalef = Q30) */
    VoC_lw16i(REG3,Tab_pow43_addr);
    VoC_add16_rr(REG6,REG3,REG6,DEFAULT);//&pow43[x-16];

    VoC_lw16i_short(REG3,21,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);//shift = 21 - scalei;


    //  VoC_push32(REG01,DEFAULT);
    VoC_push16(REG7,DEFAULT);
    VoC_lw32_p(REG67,REG6,DEFAULT);//pow43[x-16]
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);
    VoC_jal(Coolsand_MULSHIFT32);
//          VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);
//          VoC_shru16_ri(REG6,1,IN_PARALLEL);
//          VoC_macX_rr(REG6,REG1,DEFAULT);
//          VoC_shru16_ri(REG0,1,IN_PARALLEL);
//          VoC_macX_rr(REG7,REG0,DEFAULT);
    VoC_pop16(REG7,DEFAULT);
    //  VoC_pop32(REG01,IN_PARALLEL);
    VoC_jump(DequantBlock_L13);

DequantBlock_L8:
    /* normalize to [0x40000000, 0x7fffffff]
     * input x = [64, 8191] = [64, 2^13-1]
     * ranges:
     *  shift = 7:   64 -  127
     *  shift = 6:  128 -  255
     *  shift = 5:  256 -  511
     *  shift = 4:  512 - 1023
     *  shift = 3: 1024 - 2047
     *  shift = 2: 2048 - 4095
     *  shift = 1: 4096 - 8191
     */
    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);//bake scalef

    VoC_shr32_ri(REG01,-17,DEFAULT);//x <<= 17;
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//shift = 0;

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);
    VoC_shr32_ri(ACC1,-27,DEFAULT);//0x08000000;
    VoC_lw16i_short(REG6,4,IN_PARALLEL);
    VoC_bngt32_rr(DequantBlock_L9,ACC1,REG01);//if (x < 0x08000000);
    VoC_shr32_ri(REG01,-4,DEFAULT);//x <<= 4,
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//shift += 4;
DequantBlock_L9:
    VoC_shr32_ri(ACC1,-2,DEFAULT);//0x20000000;
    VoC_lw16i_short(REG6,2,IN_PARALLEL);
    VoC_bngt32_rr(DequantBlock_L10,ACC1,REG01);//if (x < 0x20000000);
    VoC_shr32_ri(REG01,-2,DEFAULT);//x <<= 2,
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//shift += 2;
DequantBlock_L10:
    VoC_shr32_ri(ACC1,-1,DEFAULT);//0x40000000;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_bngt32_rr(DequantBlock_L11,ACC1,REG01);//if (x < 0x40000000);
    VoC_shr32_ri(REG01,-1,DEFAULT);//x <<= 1,
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//shift += 1;
DequantBlock_L11:
    //#define SQRTHALF 0x5a82799a
    VoC_lw16i_set_inc(REG2,Tab_poly43hi_addr,2);
    VoC_lw16i(ACC1_LO,0x799a);
    VoC_lw16i(ACC1_HI,0x5a82);
    VoC_bngt32_rr(DequantBlock_L12,ACC1,REG01);//coef = (x < SQRTHALF) ? poly43lo : poly43hi;
    VoC_lw16i_set_inc(REG2,Tab_poly43lo_addr,2);
DequantBlock_L12:
    VoC_push16(REG7,DEFAULT);
    VoC_movreg32(ACC1,REG01,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,IN_PARALLEL);
    //REG01:x REG67:y ACC0:return;
    VoC_jal(Coolsand_MULSHIFT32);   //y = MULSHIFT32(y, x) + coef[1];
    VoC_add32inc_rp(REG67,ACC0,REG2,DEFAULT);
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);
    VoC_jal(Coolsand_MULSHIFT32);//y = MULSHIFT32(y, x) + coef[2];
    VoC_add32inc_rp(REG67,ACC0,REG2,DEFAULT);
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);
    VoC_jal(Coolsand_MULSHIFT32);// y = MULSHIFT32(y, x) + coef[3];
    VoC_add32inc_rp(REG67,ACC0,REG2,DEFAULT);
    VoC_movreg32(REG01,ACC1,IN_PARALLEL);
    VoC_jal(Coolsand_MULSHIFT32);//y = MULSHIFT32(y, x) + coef[4];
    VoC_lw16i(REG0,Tab_pow2frac_addr);
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_movreg16(REG1,REG3,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
    VoC_add32inc_rp(REG67,ACC0,REG2,DEFAULT);
    VoC_lw32_p(REG01,REG0,DEFAULT);//pow2frac[shift]
    VoC_movreg16(REG3,REG1,IN_PARALLEL);
    VoC_jal(Coolsand_MULSHIFT32);//y = MULSHIFT32(y, pow2frac[shift]) << 3;
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);//load scalef
    VoC_movreg32(REG01,ACC1,DEFAULT);
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);
    /* fractional scale
     * result: y = Q21 (pow43tab[j] = Q23, scalef = Q30)
     */
    VoC_jal(Coolsand_MULSHIFT32);//y = MULSHIFT32(y, scalef);   /* now y is Q24 */

    VoC_lw16i(REG6,Tab_pow2exp_addr);
    VoC_pop16(REG7,DEFAULT); //load scalei
    VoC_lw16i_short(REG2,24,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG6,IN_PARALLEL);//&pow2exp[shift]

    VoC_sub16_rr(REG2,REG2,REG7,DEFAULT);//24 - scalei
    VoC_sub16_rp(REG3,REG2,REG6,DEFAULT);//shift = 24 - scalei - pow2exp[shift];

DequantBlock_L13:
    /* integer scale */
    VoC_bnltz16_r(DequantBlock_L14,REG3);//if (shift <= 0)
    VoC_lw16i_short(REG2,-31,DEFAULT);
    VoC_bngt16_rr(DequantBlock_L15,REG2,REG3)  //if (shift < -31)
    VoC_movreg16(REG3,REG2,DEFAULT);
DequantBlock_L15:
    VoC_shr32_rr(ACC0,REG3,DEFAULT);
    VoC_jump(DequantBlock_L16);

DequantBlock_L14:
    VoC_shr32_rr(ACC0,REG3,DEFAULT);

DequantBlock_L16:
    VoC_movreg16(REG1,RL7_HI,DEFAULT); //load iSamp;
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);
    VoC_movreg16(REG3,RL7_LO,DEFAULT); //load inbuf;
    /* apply sign */
    VoC_shr32_ri(REG01,31,IN_PARALLEL);//iSamp >>= 31;

    /* sign and store (gbMask used to count GB's) */
    VoC_or32_rd(RL6,LOCAL_Dequantize_gbMask_addr);//gbMask |= y;
    VoC_xor32_rr(ACC0,REG01,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    VoC_sub32_rr(ACC0,ACC0,REG01,DEFAULT);
    VoC_sw32_d(RL6,LOCAL_Dequantize_gbMask_addr);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//*inbuf++ = y;

    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG4,REG3,DEFAULT);

DequantBlock_End:
    VoC_return;
}

/**************************************************************************************
 * Function:    Dequantize
 *
 * Description: dequantize all transform coefficients for one channel
 *
 * Inputs:      REG6-> ch;
 *
 * Outputs:     REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : all
 *
 * Version 1.0  Create by  Xuml       07/16/2007
 *

 **************************************************************************************/

void Coolsand_Dequantize(void)
{
    //icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);

    VoC_lw16_d(REG3,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);
    VoC_bne16_rr(Dequantize_L0,REG6,REG4);  //if ch == 1;
    VoC_be16_rr(Dequantize_L0,REG3,REG4);  //if psi->commonWin == 1;
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,1);
Dequantize_L0:
    VoC_sw32_d(RL6,LOCAL_Dequantize_gbMask_addr);
    VoC_inc_p(REG0,DEFAULT); //&icsInfo->winSequence
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16i_short(INC0,3,DEFAULT);
    VoC_lw16i_short(REG4,5,IN_PARALLEL);


    VoC_lw16inc_p(REG3,REG0,DEFAULT);   //icsInfo->winSequence
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//icsInfo->maxSFB;
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);//FORMAT32=0,

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//icsInfo->numWinGroup;
    VoC_lw16i_short(FORMATX,15,IN_PARALLEL);//FORMATX=15

    VoC_sw16_d(REG4,LOCAL_Dequantize_maxSFB_addr);
    VoC_sw16_d(REG5,LOCAL_Dequantize_numWinGroup_addr);

    VoC_lw16i(REG4,Tab_sfBandTabLongOffset_addr);

    VoC_lw16i_set_inc(REG1,Tab_sfBandTabLong_addr,1);
    VoC_lw16i(REG7,NSAMPS_LONG);//nSamps = NSAMPS_LONG;
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_bne16_rr(Dequantize_L1,REG5,REG3); //if (icsInfo->winSequence == 2)
    VoC_lw16i(REG4,Tab_sfBandTabShortOffset_addr);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabShort_addr,1);
    VoC_lw16i(REG7,NSAMPS_SHORT);//nSamps = NSAMPS_SHORT;
Dequantize_L1:
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);//&sfBandTabLongOffset[psi->sampRateIdx]
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_add16_rp(REG1,REG1,REG4,DEFAULT); //sfbTab



    VoC_bez16_r(Dequantize_L2,REG6); //if ch
    VoC_sw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr+1);
    VoC_sw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr+1);
    VoC_lw16i(ACC0_LO,STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr+1);
    VoC_lw16i(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr+1);
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_coef1_addr);//coef = psi->coef[ch];
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1);//sfbCodeBook = psi->sfbCodeBook[ch];
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr,1);  //scaleFactors = psi->scaleFactors[ch];
    VoC_lw16i(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
    VoC_jump(Dequantize_L3);
Dequantize_L2:
    VoC_sw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr);
    VoC_sw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr);
    VoC_lw16i(ACC0_LO,STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr);
    VoC_lw16i(ACC0_HI,STRUCT_AACDecInfo_PSInfoBase_pnsUsed_addr);
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_coef_addr);
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr,1);
    VoC_lw16i(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
Dequantize_L3:

    VoC_movreg32(ACC1,REG23,DEFAULT);//load sfbCodeBook and scaleFactors
    VoC_sw16_d(REG6,LOCAL_Dequantize_gbCurrent_addr_addr);
    VoC_sw16_d(REG1, LOCAL_Dequantize_sfbTab_addr_addr);
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL7,IN_PARALLEL);//// sfb=0
    VoC_lw32z(RL6,DEFAULT)//gbMask = 0;



Dequantize_L10:

    //for (gp = 0; gp < icsInfo->numWinGroup; gp++)

    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_lw16_d(REG5,LOCAL_Dequantize_numWinGroup_addr);
    VoC_bngt16_rr(Dequantize_L11,REG5,REG6);

    //   VoC_push16(REG5,DEFAULT);
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//gp++
    // VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);//load icsInfo->maxSFB
    VoC_movreg16(RL7_HI,REG6,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL); //icsInfo->winGroupLen[gp] gp++;

    VoC_loop_r(1,REG6);

    VoC_movreg32(REG23,ACC1,DEFAULT);//load sfbCodeBook and scaleFactors
    VoC_lw16_d(REG1,LOCAL_Dequantize_sfbTab_addr_addr);

Dequantize_L5:
    /* dequantize one scalefactor band (not necessary if codebook is intensity or PNS)
    * for zero codebook, still run dequantizer in case non-zero pulse data was added
    */
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_lw16_d(REG5,LOCAL_Dequantize_maxSFB_addr);
    VoC_bngt16_rr(Dequantize_L4,REG5,REG6);//for (sfb = 0; sfb < icsInfo->maxSFB; sfb++)

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT)
    VoC_lw16inc_p(REG5,REG2,IN_PARALLEL);//cb = (int)(sfbCodeBook[sfb]);
    VoC_lw16inc_p(REG6,REG1,DEFAULT);// sfbTab[sfb];
    VoC_movreg16(RL7_LO,REG6,IN_PARALLEL); //bake sfb++;
    VoC_sub16_pr(REG6,REG1,REG6,DEFAULT);//width = sfbTab[sfb+1] - sfbTab[sfb];
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);
    //  VoC_lw16i_short(REG1,13,IN_PARALLEL);
    //  VoC_lw16i_short(REG0,11,DEFAULT);
    //  VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_lw16inc_p(REG0,REG3,DEFAULT); // load scaleFactors[sfb]
    VoC_bltz16_r(Dequantize_L6,REG5);
    VoC_bgt16_rd(Dequantize_L6,REG5,const_11_addr);
    //REG4:coef;REG6:width REG0:scaleFactors[sfb];


    VoC_push32(RL7,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
    VoC_push32(ACC0,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);
    // Inputs:      REG4-> inbuf; (quantized transform coefficients, range = [0, 8191])
    // REG6-> width; (number of samples to dequantize)
    // REG0-> scale;  (scalefactor for this block of data, range = [0, 256])
    VoC_jal(Coolsand_DequantBlock);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);
    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_jump(Dequantize_L9);
Dequantize_L6:
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);//load  psi->pnsUsed
    VoC_lw16i_short(REG0,1,IN_PARALLEL);
    VoC_bne16_rd(Dequantize_L7,REG5,const_13_addr);
    VoC_sw16_p(REG0,REG6,DEFAULT);//psi->pnsUsed[ch] = 1;
    VoC_jump(Dequantize_L9);
Dequantize_L7:
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);//load psi->intensityUsed
    //  VoC_lw16i_short(REG1,1,IN_PARALLEL);
    //  VoC_lw16i_short(REG0,14,DEFAULT);
    //  VoC_lw16i_short(REG1,15,IN_PARALLEL);
    VoC_be16_rd(Dequantize_L8,REG5,const_14_addr);
    VoC_be16_rd(Dequantize_L8,REG5,const_15_addr);
    VoC_jump(Dequantize_L9);
Dequantize_L8:
    VoC_sw16_p(REG0,REG6,DEFAULT);  //psi->intensityUsed[ch] = 1;   /* should only happen if ch == 1 */

Dequantize_L9:
    VoC_pop32(REG01,DEFAULT);
    VoC_jump(Dequantize_L5);
Dequantize_L4:

    VoC_lw16i_short(RL7_LO,0,DEFAULT);  // sfb=0
    VoC_sub16_rp(REG6,REG7,REG1,IN_PARALLEL);//nSamps - sfbTab[icsInfo->maxSFB]
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//coef += (nSamps - sfbTab[icsInfo->maxSFB]);

    VoC_endloop1
    VoC_movreg32(ACC1,REG23,DEFAULT);//load sfbCodeBook and scaleFactors

    VoC_jump(Dequantize_L10);
Dequantize_L11:
    /* set flag if PNS used for any channel */
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);//load  psi->pnsUsed
    VoC_lw16_d(REG1,LOCAL_Dequantize_gbCurrent_addr_addr);
    VoC_lw16_p(REG6,REG6,DEFAULT);
    VoC_or16_rd(REG6,STRUCT_AACDecInfo_pnsUsed_addr);//aacDecInfo->pnsUsed |= psi->pnsUsed[ch];
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_sw16_d(REG6,STRUCT_AACDecInfo_pnsUsed_addr);
    VoC_lw32_d(RL6,LOCAL_Dequantize_gbMask_addr);

    /* calculate number of guard bits in dequantized data */

    //RL6:gbmask REG6:return
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//psi->gbCurrent[ch] = CLZ(gbMask) - 1;
    VoC_pop16(RA,IN_PARALLEL);
    VoC_sw32_d(RL6,LOCAL_Dequantize_gbMask_addr);
    VoC_sw16_p(REG6,REG1,DEFAULT);
    VoC_return;


}

/*********************************dequant.c end************************************************/

/*********************************fft.c begin************************************************/

#define NUM_FFT_SIZES   2

/**************************************************************************************
 * Function:    Coolsand_BitReverse
 *
 * Description: Ken's fast in-place bit reverse, using super-small table
 *
 * Inputs:      REG1->coef;
 *              REG3->tabidx;
 *
 * Outputs:    NONE;
 *
 * Used :  all;
 *
 * Version 1.0  Created by  Xuml       07/30/2007
 *
 **************************************************************************************/

void Coolsand_BitReverse(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjpAacGenerateBitRev);
    VoC_pop16(RA,DEFAULT);


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_movreg16(REG0,REG1,DEFAULT);//part0 = inout;
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_short(REG6,0,DEFAULT);//bitrevtabOffset[tabidx]
    VoC_lw16i_short(REG7,-7,IN_PARALLEL);//nbits = nfftlog2Tab[tabidx]

    VoC_bez16_r(BitReverse_L0,REG3)
    VoC_lw16i_short(REG6,17,DEFAULT);//bitrevtabOffset[tabidx]
    VoC_lw16i_short(REG7,-10,IN_PARALLEL);//nbits = nfftlog2Tab[tabidx]
BitReverse_L0:
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG3,4,IN_PARALLEL);
    VoC_lw16i(REG5,Tab_bitrevtab_addr);
    VoC_add16_rr(REG2,REG5,REG6,DEFAULT);//tab = bitrevtab + bitrevtabOffset[tabidx];
    VoC_shr16_rr(REG4,REG7,IN_PARALLEL);//(1 << nbits)
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//part1 = inout + (1 << nbits);
    VoC_movreg32(REG67,REG01,DEFAULT);
BitReverse_L2:
    VoC_lw16inc_p(REG4,REG2,DEFAULT);//a = *tab++;
    VoC_bez16_r(BitReverse_L1,REG4);//while ((a = *tab++) != 0)
    VoC_lw16inc_p(REG5,REG2,DEFAULT);//b = *tab++;
    VoC_shr16_ri(REG4,-3,IN_PARALLEL);//4*a
    VoC_shr16_ri(REG5,-3,DEFAULT);//4*b
    VoC_add16_rr(REG0,REG6,REG4,IN_PARALLEL);//part0[4*a+0];
    VoC_add16_rr(REG1,REG6,REG5,DEFAULT);// part0[4*b+0]
    //swapcplx(part0[4*a+0], part0[4*b+0]); /* 0xxx0 <-> 0yyy0 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32_p(ACC1,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG4,IN_PARALLEL);//part0[4*a+0];
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_lw32_p(RL7,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG6,REG5,IN_PARALLEL);//part0[4*b+0]
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG5,IN_PARALLEL);//part1[4*b+0]

    //swapcplx(part0[4*a+2], part1[4*b+0]); /* 0xxx1 <-> 1yyy0 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//part0[4*a+2]
    VoC_lw32_p(ACC1,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG4,IN_PARALLEL);//part0[4*a+0];
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);//part0[4*a+2]
    VoC_lw32_p(RL7,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG5,IN_PARALLEL);//part1[4*b+0]
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32_p(RL7,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG7,REG4,IN_PARALLEL);//part1[4*a+0];
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG6,REG5,IN_PARALLEL);//part0[4*b+0]

    //swapcplx(part1[4*a+0], part0[4*b+2]); /* 1xxx0 <-> 0yyy1 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//part0[4*a+2]
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);//part0[4*b+2]
    VoC_lw32_p(ACC1,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG7,REG4,IN_PARALLEL);//part1[4*a+0];
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG6,REG5,IN_PARALLEL);//part0[4*b+0]
    VoC_lw32_p(RL7,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);//part0[4*b+2]
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG5,IN_PARALLEL);//part1[4*b+0]
    VoC_sw32inc_p(RL7,REG0,DEFAULT);

    //swapcplx(part1[4*a+2], part1[4*b+2]); /* 1xxx1 <-> 1yyy1 */
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//part0[4*a+2]
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);//part1[4*b+2]
    VoC_lw32_p(ACC1,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG7,REG4,IN_PARALLEL);//part1[4*a+0];
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);//part1[4*a+2]
    VoC_lw32_p(RL7,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG5,IN_PARALLEL);//part1[4*b+0]
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);//part1[4*b+2]
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_jump(BitReverse_L2);


BitReverse_L1:
    VoC_shr16_ri(REG4,-3,DEFAULT);//4*a
    VoC_add16_rr(REG1,REG7,REG4,DEFAULT);//part1[4*a+0];
    VoC_add16_rr(REG0,REG6,REG4,DEFAULT);//part0[4*a+0];

BitReverse_L3:  //do {
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);//part0[4*a+2]
    VoC_lw32_p(RL7,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG4,IN_PARALLEL);//part1[4*a+0]

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//part0[4*a+2]
    VoC_lw32_p(ACC1,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG4,IN_PARALLEL);//part0[4*a+0];

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);//part0[4*a+2]
    VoC_lw16inc_p(REG4,REG2,DEFAULT);//a = *tab++;
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_shr16_ri(REG4,-3,IN_PARALLEL);//4*a
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG4,IN_PARALLEL);//part1[4*a+0];
    VoC_sw32_p(RL7,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG6,REG4,IN_PARALLEL);//part0[4*a+0];
    VoC_bnez16_r(BitReverse_L3,REG4);//while ((a = *tab++) != 0)
    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_R4FirstPass
 *
 * Description: radix-4 trivial pass for decimation-in-time FFT
 *
 * Inputs:      REG1->x;
 *              REG3->bg;
 *
 * Outputs:    NONE;
 *
 * Used :  all;
 *
 * Version 1.0  Created by  Xuml       07/31/2007
 *
 * Notes:       assumes 2 guard bits, gains no integer bits,
 *                guard bits out = guard bits in - 2
 **************************************************************************************/
void Coolsand_R4FirstPass(void)
{

    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_loop_r_short(REG3,DEFAULT);//for (; bg != 0; bg--)
    VoC_startloop0
    VoC_push16(REG2,DEFAULT);
    VoC_lw32inc_p(REG01,REG2,DEFAULT);//load x[0]
    VoC_lw32inc_p(RL7,REG2,DEFAULT);//load x[1]
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//load x[2]
    VoC_lw32inc_p(REG67,REG2,DEFAULT);//load x[3]
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);//load x[4]
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);//load x[5]
    VoC_lw32inc_p(RL6,REG2,DEFAULT);//load x[6]
    VoC_lw32_p(RL7,REG2,DEFAULT);//load x[7]
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,REG45,DEFAULT);//ar = x[0] + x[2];
    VoC_sub32_rr(REG45,REG01,REG45,IN_PARALLEL);//br = x[0] - x[2];
    VoC_add32_rr(REG23,REG23,REG67,DEFAULT);//ai = x[1] + x[3];
    VoC_sub32_rr(REG67,REG23,REG67,IN_PARALLEL);//bi = x[1] - x[3];
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);//cr = x[4] + x[6];
    VoC_sub32_rr(RL6,ACC0,RL6,IN_PARALLEL);//dr = x[4] - x[6];
    VoC_add32_rr(ACC1,ACC1,RL7,DEFAULT);//ci = x[5] + x[7];
    VoC_sub32_rr(RL7,ACC1,RL7,IN_PARALLEL);//di = x[5] - x[7];

    VoC_add32_rr(REG01,REG01,ACC0,DEFAULT);//x[0] = ar + cr;
    VoC_sub32_rr(ACC0,REG01,ACC0,IN_PARALLEL);//x[4] = ar - cr;
    VoC_add32_rr(REG23,REG23,ACC1,DEFAULT);//x[1] = ai + ci;
    VoC_sub32_rr(ACC1,REG23,ACC1,IN_PARALLEL);//x[5] = ai - ci;
    VoC_add32_rr(REG45,REG45,RL7,DEFAULT);//x[2] = br + di;
    VoC_sub32_rr(RL7,REG45,RL7,IN_PARALLEL);//x[6] = br - di;


    VoC_push32(ACC1,DEFAULT);
    VoC_movreg32(ACC1,REG23,IN_PARALLEL);
    VoC_pop16(REG2,DEFAULT);

    VoC_add32_rr(RL6,REG67,RL6,DEFAULT);//x[7] = bi + dr;
    VoC_sub32_rr(REG67,REG67,RL6,IN_PARALLEL);//x[3] = bi - dr;

    VoC_sw32inc_p(REG01,REG2,DEFAULT);//save x[0];
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);//save x[1];
    VoC_pop32(ACC1,DEFAULT);
    VoC_sw32inc_p(REG45,REG2,DEFAULT);//save x[2];
    VoC_sw32inc_p(REG67,REG2,DEFAULT);//save x[3];
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//save x[4];
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);//save x[5];
    VoC_sw32inc_p(RL7,REG2,DEFAULT);//save x[6];
    VoC_sw32inc_p(RL6,REG2,DEFAULT);//save x[7];

    VoC_endloop0

    VoC_return;

}

/**************************************************************************************
* Function:    Coolsand_R8FirstPass
*
* Description: radix-8 trivial pass for decimation-in-time FFT
*
* Inputs:      REG1->x;
*              REG3->bg;
*
* Outputs:    NONE;
*
* Used :  all;
*
* Version 1.0  Created by  Xuml       07/31/2007
*
* Notes:       assumes 3 guard bits, gains 1 integer bit
*              guard bits out = guard bits in - 3 (if inputs are full scale)
*                or guard bits in - 2 (if inputs bounded to +/- sqrt(2)/2)
*              see scaling comments in code
**************************************************************************************/
void Coolsand_R8FirstPass(void)
{
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_loop_r(0,REG3);//for (; bg != 0; bg--)

    VoC_push16(REG2,DEFAULT);
    VoC_lw32inc_p(REG01,REG2,DEFAULT);//load x[0]
    VoC_lw32inc_p(RL7,REG2,DEFAULT);//load x[1]
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//load x[2]
    VoC_lw32inc_p(REG67,REG2,DEFAULT);//load x[3]
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);//load x[4]
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);//load x[5]
    VoC_lw32inc_p(RL6,REG2,DEFAULT);//load x[6]
    VoC_lw32_p(RL7,REG2,DEFAULT);//load x[7]
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,REG45,DEFAULT);//ar = x[0] + x[2];
    VoC_sub32_rr(REG45,REG01,REG45,IN_PARALLEL);//br = x[0] - x[2];
    VoC_add32_rr(REG23,REG23,REG67,DEFAULT);//ai = x[1] + x[3];
    VoC_sub32_rr(REG67,REG23,REG67,IN_PARALLEL);//bi = x[1] - x[3];
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);//cr = x[4] + x[6];
    VoC_sub32_rr(RL6,ACC0,RL6,IN_PARALLEL);//dr = x[4] - x[6];
    VoC_add32_rr(ACC1,ACC1,RL7,DEFAULT);//ci = x[5] + x[7];
    VoC_sub32_rr(RL7,ACC1,RL7,IN_PARALLEL);//di = x[5] - x[7];

    VoC_add32_rr(REG01,REG01,ACC0,DEFAULT);//sr = ar + cr;
    VoC_sub32_rr(ACC0,REG01,ACC0,IN_PARALLEL);//ur = ar - cr;
    VoC_add32_rr(REG23,REG23,ACC1,DEFAULT);//si = ai + ci;
    VoC_sub32_rr(ACC1,REG23,ACC1,IN_PARALLEL);//ui = ai - ci;
    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//tr = br - di;
    VoC_sub32_rr(REG45,REG45,RL7,IN_PARALLEL);//vr = br + di;
    VoC_add32_rr(RL6,REG67,RL6,DEFAULT);//ti = bi + dr;
    VoC_sub32_rr(REG67,REG67,RL6,IN_PARALLEL);//vi = bi - dr;

    VoC_shr32_ri(REG45,1,DEFAULT);//(tr >> 1)
    VoC_shr32_ri(RL7,1,IN_PARALLEL);//(vr >> 1)
    VoC_shr32_ri(REG01,1,DEFAULT);  //(sr >> 1)
    VoC_shr32_ri(ACC0,1,IN_PARALLEL);//(ur >> 1)
    VoC_shr32_ri(REG23,1,DEFAULT);//(si >> 1)
    VoC_shr32_ri(ACC1,1,IN_PARALLEL);//(ui >> 1)

    VoC_push32(REG45,DEFAULT);
    VoC_movreg32(REG45,REG23,IN_PARALLEL);

    VoC_lw16_sd(REG2,0,DEFAULT);

    VoC_shr32_ri(RL6,1,DEFAULT);//(ti >> 1)
    VoC_shr32_ri(REG67,1,IN_PARALLEL);//(vi >> 1)



    VoC_sw32inc_p(REG01,REG2,DEFAULT);//save sr>>1;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);//save si>>1;
    VoC_pop32(REG45,DEFAULT);
    VoC_sw32inc_p(RL7,REG2,DEFAULT);//save vr>>1;
    VoC_sw32inc_p(REG67,REG2,DEFAULT);//save vi>>1;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//save ur>>1;
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);//save ui>>1;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);//save tr>>1;
    VoC_sw32inc_p(RL6,REG2,DEFAULT);//save ti>>1;


    VoC_lw32inc_p(REG01,REG2,DEFAULT);//load x[8]
    VoC_lw32inc_p(RL7,REG2,DEFAULT);//load x[9]
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//load x[10]
    VoC_lw32inc_p(REG67,REG2,DEFAULT);//load x[11]
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);//load x[12]
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);//load x[13]
    VoC_lw32inc_p(RL6,REG2,DEFAULT);//load x[14]
    VoC_lw32_p(RL7,REG2,DEFAULT);//load x[15]
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,REG45,DEFAULT);//ar = x[8] + x[10];
    VoC_sub32_rr(REG45,REG01,REG45,IN_PARALLEL);//br = x[8] - x[10];
    VoC_add32_rr(REG23,REG23,REG67,DEFAULT);//ai = x[9] + x[11];
    VoC_sub32_rr(REG67,REG23,REG67,IN_PARALLEL);//bi = x[9] - x[11];
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);//cr = x[12] + x[14];
    VoC_sub32_rr(RL6,ACC0,RL6,IN_PARALLEL);//dr = x[12] - x[14];
    VoC_add32_rr(ACC1,ACC1,RL7,DEFAULT);//ci = x[13] + x[15];
    VoC_sub32_rr(RL7,ACC1,RL7,IN_PARALLEL);//di = x[13] - x[15];

    /* max gain of wr/wi/yr/yi vs input = 2
     *  (sum of 4 samples >> 1)
     */

    VoC_add32_rr(REG01,REG01,ACC0,DEFAULT);
    VoC_sub32_rr(ACC0,REG01,ACC0,IN_PARALLEL);
    VoC_add32_rr(REG23,REG23,ACC1,DEFAULT);
    VoC_sub32_rr(ACC1,REG23,ACC1,IN_PARALLEL);

    VoC_shr32_ri(REG01,1,DEFAULT);//wr = (ar + cr) >> 1;
    VoC_shr32_ri(ACC0,1,IN_PARALLEL);//yr = (ar - cr) >> 1;
    VoC_shr32_ri(REG23,1,DEFAULT);//wi = (ai + ci) >> 1;
    VoC_shr32_ri(ACC1,1,IN_PARALLEL);//yi = (ai - ci) >> 1;

    VoC_push32(REG45,DEFAULT);
    VoC_movreg32(REG45,REG23,IN_PARALLEL);

    VoC_push32(REG67,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    VoC_push32(RL6,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);
    VoC_push32(RL7,DEFAULT);
    VoC_lw16i_short(REG6,16,IN_PARALLEL)


    /* max gain of output vs input = 4
     *  (sum of 4 samples >> 1 + sum of 4 samples >> 1)
     */
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);//&x[8]
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_lw32_p(RL6,REG2,DEFAULT);// load sr>>1;
    VoC_add16_rr(REG6,REG7,REG2,IN_PARALLEL);

    VoC_add32_rr(RL6,RL6,REG01,DEFAULT);//x[0] = (sr >> 1) + wr;
    VoC_sub32_rr(REG01,RL6,REG01,IN_PARALLEL);//x[8] = (sr >> 1) - wr;

    VoC_lw32_p(RL7,REG6,DEFAULT);// load si>>1;
    VoC_lw16i_short(REG6,6,IN_PARALLEL);

    VoC_add32_rr(RL7,RL7,REG45,DEFAULT);//(si >> 1) + wi;
    VoC_sub32_rr(REG45,RL7,REG45,IN_PARALLEL);//(si >> 1) - wi;

    VoC_sw32inc_p(RL6,REG2,DEFAULT);//x[ 0] = (sr >> 1) + wr;
    VoC_sw32inc_p(REG01,REG3,DEFAULT);//x[ 8] = (sr >> 1) - wr;

    VoC_sw32_p(RL7,REG2,DEFAULT);//x[ 1] = (si >> 1) + wi;
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);//&x[ 4]

    VoC_sw32_p(REG45,REG3,DEFAULT);//x[ 9] = (si >> 1) - wi;
    VoC_add16_rr(REG3,REG3,REG6,IN_PARALLEL);//&x[12]


    VoC_lw32_p(RL6,REG2,DEFAULT);// load ur>> 1;
    VoC_add16_rr(REG6,REG7,REG2,IN_PARALLEL);

    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT);//x[4] = (ur >> 1) + yi;
    VoC_sub32_rr(ACC1,RL6,ACC1,IN_PARALLEL);//x[12] = (ur >> 1) - yi;

    VoC_lw32_p(RL7,REG6,DEFAULT);// load sr>>1;

    VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);//(ui >> 1) + yr;
    VoC_sub32_rr(ACC0,RL7,ACC0,IN_PARALLEL);//(ui >> 1) - yr;

    VoC_sw32inc_p(RL6,REG2,DEFAULT);//x[ 4] = (ur >> 1) + yi;
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);//x[12] = (ur >> 1) - yi;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//x[ 5] = (ui >> 1) - yr;
    VoC_sw32inc_p(RL7,REG3,DEFAULT);//x[13] = (ui >> 1) + yr;

    VoC_pop32(RL7,DEFAULT);
    VoC_lw32_d(REG01,const_SQRT1_2_addr);//SQRT1_2
    VoC_pop32(RL6,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);//for MULSHIFT32
    VoC_pop32(REG67,DEFAULT);
    VoC_pop32(REG45,DEFAULT);

    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//cr = br + di;
    VoC_sub32_rr(REG45,REG45,RL7,IN_PARALLEL);//ar = br - di;
    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);//ai = bi + dr;
    VoC_sub32_rr(RL6,REG67,RL6,IN_PARALLEL);//ci = bi - dr;

    /* max gain of xr/xi/zr/zi vs input = 4*sqrt(2)/2 = 2*sqrt(2)
     *  (sum of 8 samples, multiply by sqrt(2)/2, implicit >> 1 from Q31)
     */
    VoC_add32_rr(REG45,REG45,REG67,DEFAULT);//ar + ai;
    VoC_sub32_rr(REG67,REG45,REG67,IN_PARALLEL);//ar - ai;
    VoC_add32_rr(RL6,RL7,RL6,DEFAULT);//cr + ci;
    VoC_sub32_rr(RL7,RL7,RL6,IN_PARALLEL);//cr - ci;

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);//MULSHIFT32(SQRT1_2, ar - ai);
    VoC_multf32_rr(ACC1,REG1,REG5,IN_PARALLEL);// MULSHIFT32(SQRT1_2, ar + ai);

    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_macX_rr(REG4,REG1,IN_PARALLEL);

    VoC_macX_rr(REG7,REG0,DEFAULT);
    VoC_macX_rr(REG5,REG0,IN_PARALLEL);

    VoC_movreg32(REG45,RL6,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_movreg32(RL6,ACC0,DEFAULT);////xr = MULSHIFT32(SQRT1_2, ar - ai);
    VoC_movreg32(RL7,ACC1,IN_PARALLEL); //xi = MULSHIFT32(SQRT1_2, ar + ai);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG1,REG7,DEFAULT);//zr = MULSHIFT32(SQRT1_2, cr - ci);
    VoC_multf32_rr(ACC1,REG1,REG5,IN_PARALLEL);// zi = MULSHIFT32(SQRT1_2, cr + ci);

    VoC_macX_rr(REG6,REG1,DEFAULT);
    VoC_macX_rr(REG4,REG1,IN_PARALLEL);

    VoC_macX_rr(REG7,REG0,DEFAULT);
    VoC_macX_rr(REG5,REG0,IN_PARALLEL);
    /* max gain of output vs input = (2 + 2*sqrt(2) ~= 4.83)
     *  (sum of 4 samples >> 1, plus xr/xi/zr/zi with gain of 2*sqrt(2))
     * in absolute terms, we have max gain of appx 9.656 (4 + 0.707*8)
     *  but we also gain 1 int bit (from MULSHIFT32 or from explicit >> 1)
     */
    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_lw32_p(REG01,REG2,DEFAULT);
    VoC_add16_rr(REG6,REG2,REG7,IN_PARALLEL);

    VoC_sub32_rr(REG01,REG01,RL6,DEFAULT);// (tr >> 1) - xr;
    VoC_add32_rr(RL6,REG01,RL6,IN_PARALLEL);//(tr >> 1) + xr;

    VoC_lw32_p(REG45,REG6,DEFAULT);
    VoC_lw16i_short(REG6,10,IN_PARALLEL);

    VoC_sub32_rr(REG45,REG45,RL7,DEFAULT);//(ti >> 1) - xi;
    VoC_add32_rr(RL7,REG45,RL7,IN_PARALLEL);// (ti >> 1) + xi;

    VoC_sw32inc_p(REG01,REG2,DEFAULT);//x[ 6] = (tr >> 1) - xr;
    VoC_sw32inc_p(RL6,REG3,DEFAULT);//x[14] = (tr >> 1) + xr;
    VoC_sw32_p(REG45,REG2,DEFAULT);//x[ 7] = (ti >> 1) - xi;
    VoC_sub16_rr(REG2,REG2,REG6,IN_PARALLEL);//&x[ 2]
    VoC_sw32_p(RL7,REG3,DEFAULT);//x[15] = (ti >> 1) + xi;
    VoC_sub16_rr(REG3,REG3,REG6,IN_PARALLEL);//&x[10]

    VoC_lw32_p(REG01,REG2,DEFAULT); //(vr >> 1)
    VoC_add16_rr(REG6,REG2,REG7,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,ACC1,DEFAULT);// (vr >> 1) + zi;
    VoC_sub32_rr(ACC1,REG01,ACC1,IN_PARALLEL);//(vr >> 1) - zi;

    VoC_lw32_p(REG45,REG6,DEFAULT);//(vi >> 1)
    VoC_lw16i_short(REG6,26,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);//(vi >> 1) + zr;
    VoC_sub32_rr(ACC0,REG45,ACC0,IN_PARALLEL);// (vi >> 1) - zr;

    VoC_sw32inc_p(REG01,REG2,DEFAULT);//x[ 2] = (vr >> 1) + zi;
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);//same to x+=16;
    VoC_sw32_p(ACC0,REG2,DEFAULT);//x[ 3] = (vi >> 1) - zr;
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);// x[10] = (vr >> 1) - zi;
    VoC_sw32_p(REG45,REG3,DEFAULT);//x[11] = (vi >> 1) + zr;

    VoC_endloop0

    VoC_return;

}



/**************************************************************************************
 * Function:    Coolsand_R4Core_subfunction
 *
 * Description:
 *
 * Inputs:      ?
 *
 * Outputs:    ?
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/31/2007
 *
 **************************************************************************************/
void Coolsand_R4Core_subfunction(void)
{


    /* gain 2 int bits for br/bi, cr/ci, dr/di (MULSHIFT32 by Q30)
     * gain 1 net GB
     */
    VoC_lw32inc_p(RL6,REG0,DEFAULT);//ws = wptr[0];
    VoC_lw32inc_p(RL7,REG1,DEFAULT);//br = xptr[0];
    VoC_lw32_p(REG45,REG1,DEFAULT);//bi = xptr[1];
    VoC_lw32_p(REG23,REG0,DEFAULT);//wi = wptr[1];
    VoC_add32_rr(REG45,REG45,RL7,IN_PARALLEL);//br + bi
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////

    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG3,REG4,DEFAULT);

    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_lw32inc_p(REG23,REG0,IN_PARALLEL);//wi = wptr[1];
    VoC_shr32_ri(REG23,-1,DEFAULT);//2*wi
    VoC_movreg32(REG45,RL7,IN_PARALLEL)//br
    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_add32_rr(REG23,REG23,RL6,IN_PARALLEL);//wd = ws + 2*wi;

    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////

    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG3,REG4,DEFAULT);

    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////
    VoC_movreg32(REG23,RL6,IN_PARALLEL)//ws
    VoC_lw32_p(REG45,REG1,DEFAULT);//bi = xptr[1];
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15 start/////////

    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG3,REG4,DEFAULT);

    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15 end///////////

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//br
    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//bi
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_R4Core
 *
 * Description: radix-4 pass for decimation-in-time FFT
 *
 * Inputs:      REG0->wtab;
 *              REG1->x;
 *              REG4->bg;
 *              REG5->gp;
 *
 * Outputs:    NONE;
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/31/2007
 *
 * Notes:       gain 2 integer bits per pass (see scaling comments in code)
 *              min 1 GB in
 *              gbOut = gbIn - 1 (short block) or gbIn - 2 (long block)
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 **************************************************************************************/
void Coolsand_R4Core(void)
{
    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL)
R4Core_LX:
    VoC_bez16_r(R4Core_L0,REG4)

    VoC_movreg16(REG6,REG5,DEFAULT)
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_multf32_rr(ACC0,REG4,REG5,DEFAULT);
    VoC_shr16_ri(REG6,-2,IN_PARALLEL);//step = 2*gp;
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);//step-2;
    VoC_push16(RA,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);//save x
    VoC_push32(REG45,IN_PARALLEL);//save bg and gp;
    VoC_bez32_r(R4Core_L1,ACC0);
    /* max per-sample gain, per group < 1 + 3*sqrt(2) ~= 5.25 if inputs x are full-scale
     * do 3 groups for long block, 2 groups for short block (gain 2 int bits per group)
     *
     * very conservative scaling:
     *   group 1: max gain = 5.25,           int bits gained = 2, gb used = 1 (2^3 = 8)
     *   group 2: max gain = 5.25^2 = 27.6,  int bits gained = 4, gb used = 1 (2^5 = 32)
     *   group 3: max gain = 5.25^3 = 144.7, int bits gained = 6, gb used = 2 (2^8 = 256)
     */
    VoC_loop_r(1,REG4)//for (i = bg; i != 0; i--)
    VoC_push16(REG0,DEFAULT);//wptr = wtab;
    VoC_push16(REG5,DEFAULT);
    VoC_loop_r(0,REG5);//for (j = gp; j != 0; j--)

    VoC_lw32inc_p(ACC0,REG1,DEFAULT);//ar = xptr[0];
    VoC_lw32_p(ACC1,REG1,DEFAULT);//ai = xptr[1];
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);//xptr += step;

    VoC_push32(ACC0,DEFAULT);//ar
    VoC_push16(REG6,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);//ai
    VoC_jal(Coolsand_R4Core_subfunction);

    VoC_push32(ACC1,DEFAULT);//br
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);//xptr += step;
    VoC_push32(ACC0,DEFAULT);//bi

    VoC_jal(Coolsand_R4Core_subfunction);

    VoC_push32(ACC1,DEFAULT);//cr
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);//xptr += step;
    VoC_push32(ACC0,DEFAULT);//ci

    VoC_jal(Coolsand_R4Core_subfunction);//ACC1:dr;ACC0:di;

    VoC_pop32(RL7,DEFAULT);//ci
    VoC_lw16i_short(REG2,2,IN_PARALLEL);
    VoC_pop32(RL6,DEFAULT);//cr
    VoC_sub16_rr(REG1,REG1,REG2,DEFAULT);

    VoC_pop32(REG45,DEFAULT);//bi
    VoC_push16(REG0,IN_PARALLEL)
    VoC_pop32(REG23,DEFAULT);//br
    VoC_push16(REG1,IN_PARALLEL);
    VoC_pop32(REG67,DEFAULT);//ai
    VoC_push16(REG6,IN_PARALLEL)//
    VoC_pop32(REG01,DEFAULT);//ar
    VoC_shr32_ri(REG67,2,IN_PARALLEL);//ti>>2
    VoC_shr32_ri(REG01,2,DEFAULT);//tr>>2

    VoC_add32_rr(REG01,REG01,REG23,DEFAULT);//br = (tr >> 2) + br;
    VoC_sub32_rr(REG23,REG01,REG23,IN_PARALLEL);//ar = (tr >> 2) - br;

    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//bi = (ti >> 2) + bi;
    VoC_sub32_rr(REG45,REG67,REG45,IN_PARALLEL);//ai = (ti >> 2) - bi;

    VoC_add32_rr(RL6,RL6,ACC1,DEFAULT);//cr = tr + dr;
    VoC_sub32_rr(ACC1,RL6,ACC1,IN_PARALLEL);//dr = tr - dr;

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//di = di + ti;
    VoC_sub32_rr(RL7,ACC0,RL7,IN_PARALLEL);//ci = di - ti;


    VoC_add32_rr(REG01,REG01,RL6,DEFAULT);//xptr[0] = br + cr;
    VoC_sub32_rr(RL6,REG01,RL6,IN_PARALLEL);//xptr[0] = br - cr;

    VoC_add32_rr(REG23,REG23,RL7,DEFAULT);//xptr[0] = ar + ci;
    VoC_sub32_rr(RL7,REG23,RL7,IN_PARALLEL);//xptr[0] = ar - ci;

    VoC_add32_rr(REG45,REG45,ACC1,DEFAULT);//xptr[1] = ai + dr;
    VoC_sub32_rr(ACC1,REG45,ACC1,IN_PARALLEL);//xptr[1] = ai - dr;

    VoC_push32(REG01,DEFAULT);//br + cr;
    VoC_pop16(REG0,IN_PARALLEL);//step;
    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_pop16(REG1,DEFAULT);//xptr
    VoC_add16_rr(REG0,REG0,REG1,IN_PARALLEL);

    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);//xptr[1] = bi + di;
    VoC_sub32_rr(ACC0,REG67,ACC0,IN_PARALLEL);//xptr[1] = bi - di;

    VoC_sw32inc_p(REG23,REG1,DEFAULT);////xptr[0] = ar + ci;
    VoC_sub16_rr(REG1,REG1,REG0,IN_PARALLEL);//xptr -= step;
    VoC_sw32_p(REG45,REG1,DEFAULT);//xptr[1] = ai + dr;

    VoC_sw32inc_p(RL6,REG1,DEFAULT);//xptr[0] = br - cr;
    VoC_sub16_rr(REG1,REG1,REG0,IN_PARALLEL);//xptr -= step;
    VoC_sw32_p(ACC0,REG1,DEFAULT);//xptr[1] = bi - di;

    VoC_pop32(REG23,DEFAULT)//br + cr;

    VoC_sw32inc_p(RL7,REG1,DEFAULT);//xptr[0] = ar - ci;
    VoC_sub16_rr(REG1,REG1,REG0,IN_PARALLEL);//xptr -= step;
    VoC_sw32_p(ACC1,REG1,DEFAULT);//xptr[1] = ai - dr;

    VoC_sw32inc_p(REG23,REG1,DEFAULT);//xptr[0] = br + cr;
    VoC_lw16i_short(REG2,2,IN_PARALLEL);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);//xptr[1] = bi + di;
    VoC_lw16i_short(REG3,3,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);//step
    VoC_pop16(REG6,DEFAULT);


    VoC_endloop0
    VoC_add16_rr(REG4,REG6,REG2,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);//gp
    VoC_mult16_rr(REG4,REG4,REG3,DEFAULT);//3*step
    VoC_pop16(REG0,DEFAULT);//wptr
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//xptr += 3*step;
    VoC_endloop1
R4Core_L1:

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//wtab += 3*step;
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,2,DEFAULT);//bg >>= 2
    VoC_shr16_ri(REG5,-2,IN_PARALLEL);//gp <<= 2
    VoC_jump(R4Core_LX);
R4Core_L0:
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_R4FFT
 *
 * Description: Ken's very fast in-place radix-4 decimation-in-time FFT
 *
 * Inputs:      REG1->coef;
 *              REG3->tabidx;
 *
 * Outputs:    NONE;
 *
 * Used :
 *
 * Version 1.0  Created by  Xuml       07/30/2007
 *
 * Notes:       assumes 5 guard bits in for nfft <= 512
 *              gbOut = gbIn - 4 (assuming input is from PreMultiply)
 *              gains log2(nfft) - 2 int bits total
 *                so gain 7 int bits (LONG), 4 int bits (SHORT)
 **************************************************************************************/
void Coolsand_R4FFT(void)
{
    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    /* decimation in time */
    VoC_jal(Coolsand_BitReverse);

    VoC_lw16i_short(REG6,6,DEFAULT);//order = nfftlog2Tab[tabidx];
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_lw16i(REG7,64);//nfft = nfftTab[tabidx];

    VoC_bez16_r(R4FFT_L0,REG0)
    VoC_lw16i_short(REG6,9,DEFAULT);//order = nfftlog2Tab[tabidx];
    VoC_lw16i(REG7,512);//nfft = nfftTab[tabidx];
R4FFT_L0:

    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_movreg16(REG0,REG7,IN_PARALLEL);//nfft
    VoC_and16_ri(REG6,0x0001);//order & 0x1

    VoC_bez16_r(R4FFT_L1,REG6);//if (order & 0x1) {
    /* long block: order = 9, nfft = 512 */

    VoC_shr16_ri(REG3,3,DEFAULT);
    VoC_push32(REG01,DEFAULT);//save nfft and x;
    //REG1:x;REG3:nfft >> 3
    VoC_jal(Coolsand_R8FirstPass);

    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG67,DEFAULT);

    VoC_jal(vpp_AmjpAacGenerateTwidOdd);

    VoC_pop32(REG67,DEFAULT);
    VoC_pop32(REG23,DEFAULT);

    VoC_pop32(REG01,DEFAULT);//load nfft and x;
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_lw16i_short(REG5,8,IN_PARALLEL);
    VoC_lw16i(REG0,Tab_twidTabOdd_addr);
    VoC_shr16_ri(REG4,5,DEFAULT);//nfft >> 5
    //REG0:twidTabOdd;REG1:x;REG4:nfft >> 3;REG5:8
    VoC_jal(Coolsand_R4Core);
    VoC_jump(R4FFT_L2);
R4FFT_L1://} else {
    /* short block: order = 6, nfft = 64 */
    VoC_push32(REG01,DEFAULT);//save nfft and x;
    VoC_shr16_ri(REG3,2,IN_PARALLEL);
    //REG1:x;REG3:nfft >> 2
    VoC_jal(Coolsand_R4FirstPass);

    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG67,DEFAULT);

    VoC_jal(vpp_AmjpAacGenerateTwidEven);

    VoC_pop32(REG67,DEFAULT);
    VoC_pop32(REG23,DEFAULT);

    VoC_pop32(REG01,DEFAULT);//load nfft and x;
    VoC_movreg16(REG4,REG0,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);
    VoC_lw16i(REG0,Tab_twidTabEven_addr);
    VoC_shr16_ri(REG4,4,DEFAULT);//nfft >> 4
    //REG0:twidTabOdd;REG1:x;REG4:nfft >> 4;REG5:4
    VoC_jal(Coolsand_R4Core);
R4FFT_L2://}
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

/*********************************fft.c end************************************************/


/*********************************filefmt.c begin************************************************/
/**************************************************************************************
 * Function:    Coolsand_UnpackADTSHeader
 *
 * Description: parse the ADTS frame header and initialize decoder state
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> (0 if successful, error code (< 0) if error);
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,ACC0,ACC1_lo
 *
 * Version 1.0  Create by  Xuml       07/04/2007
 *
 * TODO:        test CRC
 *              verify that fixed fields don't change between frames
 **************************************************************************************/
void Coolsand_UnpackADTSHeader(void)
{

    /* verify that first 12 bits of header are syncword */

    VoC_rbinc_i(REG4,12,DEFAULT);
    VoC_lw16i_short(REG0,-3,IN_PARALLEL);  //   return ERR_AAC_INVALID_ADTS_HEADER;
    VoC_lw16_d(REG5,const_0x0fff_addr);
    VoC_bne16_rr(UnpackADTSHeader_End,REG4,REG5);//if (GetBits(&bsi, 12) != 0x0fff)


    VoC_lbinc_p(REG2);
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_fhADTS_id_addr,1);
    /* fixed fields - should not change from frame to frame */
    VoC_rbinc_i(REG4,1,DEFAULT);     //fhADTS->id =               GetBits(&bsi, 1);
    VoC_rbinc_i(REG5,2,DEFAULT); //fhADTS->layer =            GetBits(&bsi, 2);
    VoC_rbinc_i(REG6,1,DEFAULT);     //fhADTS->protectBit =       GetBits(&bsi, 1);
    VoC_rbinc_i(REG7,2,DEFAULT); //fhADTS->profile =          GetBits(&bsi, 2);
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);


    VoC_rbinc_i(REG4,4,DEFAULT);     //fhADTS->sampRateIdx =      GetBits(&bsi, 4);

    VoC_sw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG5,1,DEFAULT); //fhADTS->privateBit =       GetBits(&bsi, 1);
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG6,3,DEFAULT);     //fhADTS->channelConfig =    GetBits(&bsi, 3);
    VoC_sw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG7,1,DEFAULT); //fhADTS->origCopy =         GetBits(&bsi, 1);
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_lbinc_p(REG2);


    VoC_rbinc_i(REG4,1,DEFAULT);          //fhADTS->home =             GetBits(&bsi, 1);
    VoC_sw16inc_p(REG5,REG1,IN_PARALLEL);
    /* variable fields - can change from frame to frame */
    VoC_rbinc_i(REG5,1,DEFAULT);     //fhADTS->copyBit =          GetBits(&bsi, 1);

    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG6,1,DEFAULT);          //fhADTS->copyStart =        GetBits(&bsi, 1);
    VoC_sw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG7,13,DEFAULT);     //fhADTS->frameLength = GetBits(&bsi, 13);
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_lbinc_p(REG2);



    VoC_rbinc_i(REG5,11,DEFAULT);        //fhADTS->bufferFull =  GetBits(&bsi, 11);
    VoC_sw16inc_p(REG5,REG1,IN_PARALLEL);
    VoC_rbinc_i(REG6,2,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_add16_rd(REG6,REG6,const_1_addr); //fhADTS->numRawDataBlocks = GetBits(&bsi, 2) + 1;

    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_lw16i_short(REG3,NUM_DEF_CHAN_MAPS,IN_PARALLEL); //load NUM_DEF_CHAN_MAPS

    VoC_lbinc_p(REG2);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);

    VoC_bnez16_d (UnpackADTSHeader_L1,STRUCT_AACDecInfo_PSInfoBase_fhADTS_protectBit_addr); //  if (fhADTS->protectBit == 0)
    VoC_rbinc_i(REG7,16,DEFAULT);         //fhADTS->crcCheckWord = GetBits(&bsi, 16);
    VoC_lbinc_p(REG2);
//  VoC_sw16_p(REG7,REG1,DEFAULT);
UnpackADTSHeader_L1:



    /* byte align */
//  ByteAlignBitstream(&bsi);   /* should always be aligned anyway */
    VoC_push16(BITCACHE,DEFAULT);

    VoC_lw16i_short(REG6,AAC_PROFILE_LC,IN_PARALLEL); //load AAC_PROFILE_LC

    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i_short(REG7,NUM_SAMPLE_RATES,IN_PARALLEL); //NUM_SAMPLE_RATES

    VoC_and16_ri(REG0,0x0007);  // BITCACHE mod 8
    VoC_rbinc_r(REG4,REG0,DEFAULT); // byte align
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_short(REG0,-3,DEFAULT);//return ERR_AAC_INVALID_ADTS_HEADER;
    VoC_lw16i_short(REG3,8,IN_PARALLEL);//NUM_DEF_CHAN_MAPS
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_fhADTS_layer_addr);
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_fhADTS_profile_addr);
    VoC_bnez16_r(UnpackADTSHeader_End,REG4);  //if (fhADTS->layer != 0 )
    VoC_bne16_rr(UnpackADTSHeader_End,REG6,REG5); //if(fhADTS->profile != AAC_PROFILE_LC)
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_fhADTS_sampRateIdx_addr);
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_fhADTS_channelConfig_addr);

    VoC_bngt16_rr(UnpackADTSHeader_End,REG7,REG4); //if(fhADTS->sampRateIdx >= NUM_SAMPLE_RATES)
    VoC_bngt16_rr(UnpackADTSHeader_End,REG3,REG5); //if(fhADTS->channelConfig >= NUM_DEF_CHAN_MAPS)

    /* update codec info */
    VoC_lw16_d(REG0,STRUCT_AACDecInfo_PSInfoBase_fhADTS_sampRateIdx_addr);

    VoC_lw16i(REG3,Tab_channelMapTab_addr);
    VoC_add16_rd(REG3,REG3,STRUCT_AACDecInfo_PSInfoBase_fhADTS_channelConfig_addr);

    VoC_sw16_d(REG0,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr); //psi->sampRateIdx = fhADTS->sampRateIdx;

    VoC_lw16_p(REG0,REG3,DEFAULT);  //channelMapTab[fhADTS->channelConfig]
    VoC_lw16i_short(REG1,-1,IN_PARALLEL);  //load AAC_ID_INVALID;

    VoC_bnez16_d(UnpackADTSHeader_L2,STRUCT_AACDecInfo_PSInfoBase_useImpChanMap_addr);   //if (!psi->useImpChanMap)

    VoC_sw16_d(REG0,STRUCT_AACDecInfo_PSInfoBase_nChans_addr);  //psi->nChans = channelMapTab[fhADTS->channelConfig];

UnpackADTSHeader_L2:

    /* syntactic element fields will be read from bitstream for each element */
    VoC_sw16_d(REG1,STRUCT_AACDecInfo_prevBlockID_addr);  //aacDecInfo->prevBlockID = AAC_ID_INVALID;
    VoC_sw16_d(REG1,STRUCT_AACDecInfo_currBlockID_addr);  //aacDecInfo->currBlockID = AAC_ID_INVALID;

    /* fill in user-accessible data (TODO - calc bitrate, handle tricky channel config cases) */
    VoC_sw32_d(ACC0,STRUCT_AACDecInfo_bitRate_addr); //aacDecInfo->bitRate = 0;
    VoC_sw16_d(ACC0_HI,STRUCT_AACDecInfo_sbrEnabled_addr);  //aacDecInfo->sbrEnabled = 0;

    VoC_sw16_d(REG1,STRUCT_AACDecInfo_currInstTag_addr);  //aacDecInfo->currInstTag = -1;

    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_nChans_addr);
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_fhADTS_profile_addr);
    VoC_lw16_d(REG6,STRUCT_AACDecInfo_PSInfoBase_fhADTS_numRawDataBlocks_addr);

    VoC_lw16_d(REG0,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr); //psi->sampRateIdx
    VoC_shr16_ri(REG0,-1,DEFAULT); //psi->sampRateIdx*2
    VoC_lw16i(REG1,Tab_sampRateTab_addr);//sampRateTab
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT); //&sampRateTab[psi->sampRateIdx]


    VoC_sw16_d(REG4,STRUCT_AACDecInfo_nChans_addr);//aacDecInfo->nChans = psi->nChans;

    VoC_sw16_d(REG4,GLOBAL_NB_CHAN);//aacDecInfo->nChans = psi->nChans;

    VoC_sw16_d(REG5,STRUCT_AACDecInfo_profile_addr); //aacDecInfo->profile = fhADTS->profile;

    VoC_lw32_p(ACC0,REG0,DEFAULT); // sampRateTab[psi->sampRateIdx];


    VoC_sw16_d(REG6,STRUCT_AACDecInfo_adtsBlocksLeft_addr); //  aacDecInfo->adtsBlocksLeft = fhADTS->numRawDataBlocks;
    VoC_sw32_d(ACC0,STRUCT_AACDecInfo_sampRate_addr); //aacDecInfo->sampRate = sampRateTab[psi->sampRateIdx];
    VoC_sw32_d(ACC0,GLOBAL_SAMPLE_RATE);

    VoC_lw16i_short(REG0,0,DEFAULT);// for return;



UnpackADTSHeader_End:

    VoC_return

}


/*********************************filefmt.c end************************************************/



/*********************************huffman.c begin************************************************/

/**************************************************************************************
 * Function:    Coolsand_DecodeHuffmanScalar
 *
 * Description: decode one Huffman symbol from bitstream
 *
 * Inputs:      REG0-> huffTabInfo
 *              REG1-> huffTab
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG6-> value
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6,ACC1
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 *
 * Notes:       assumes canonical Huffman codes:
 *                first CW always 0, we have "count" CW's of length "nBits" bits
 *                starting CW for codes of length nBits+1 =
 *                  (startCW[nBits] + count[nBits]) << 1
 *                if there are no codes at nBits, then we just keep << 1 each time
 *                  (since count[nBits] = 0)
 **************************************************************************************/
void Coolsand_DecodeHuffmanScalar(void)
{
    VoC_lw16i_short(REG3,21,DEFAULT);
    VoC_add16_rr(REG3,REG0,REG3,DEFAULT); // &huffTabInfo->count;
    VoC_lw32z(REG67,IN_PARALLEL);// start = 0;
    VoC_inc_p(REG0,DEFAULT);///countPtr = huffTabInfo->count;
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_add16_rp(REG1,REG1,REG3,DEFAULT);//map = huffTab + huffTabInfo->offset;
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//count = 0;
DecodeHuffmanScalar_L0:
    VoC_movreg16(REG5,REG3,DEFAULT);
    VoC_shr32_ri(REG45,16,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG3,IN_PARALLEL);//map += count;
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//start += count;
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_shr32_ri(REG67,-1,DEFAULT);//start <<= 1;
    VoC_lw16inc_p(REG3,REG0,IN_PARALLEL);//count = *countPtr++;
    VoC_lbinc_p(REG2);
    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,IN_PARALLEL);  //the same to shift--;
    VoC_or32_rr(ACC0,REG45,DEFAULT);    //the same to (bitBuf >> shift)
    VoC_sub32_rr(REG45,ACC0,REG67,DEFAULT);//the same to t = (bitBuf >> shift) - start;

    VoC_bngt16_rr(DecodeHuffmanScalar_L0,REG3,REG4);// while (t >= count);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lbinc_p(REG2);
    VoC_lw16_p(REG6,REG1,DEFAULT);//*val = (signed int)map[t];
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_UnpackZeros
 *
 * Description: fill a section of coefficients with zeros
 *
 * Inputs:      REG7-> nVals
 *              REG4-> coef

 *
 * Outputs:     no
 *
 * Used : REG7,REG6,REG4,REG0
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 * Notes:       assumes nVals is always a multiple of 4 because all scalefactor bands
 *                are a multiple of 4 coefficients long
 **************************************************************************************/

void Coolsand_UnpackZeros(void)
{
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_movreg16(REG0,REG4,IN_PARALLEL);

    VoC_add16_rr(REG4,REG7,REG6,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);

    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_bez16_r(UnpackZeros_End,REG4);//while (nVals > 0)
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_endloop0

UnpackZeros_End:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_UnpackQuads
 *
 * Description: decode a section of 4-way vector Huffman coded coefficients
 *
 * Inputs:      REG4-> coef
 *              REG6-> cb
 *              REG7-> nVals
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 * Notes:       assumes nVals is always a multiple of 4 because all scalefactor bands
 *                are a multiple of 4 coefficients long
 **************************************************************************************/
void Coolsand_UnpackQuads(void)
{
    VoC_lw16i_short(REG5,HUFFTAB_SPEC_OFFSET,DEFAULT);
    VoC_lw16i_short(REG0,22,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);//cb - HUFFTAB_SPEC_OFFSET
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL); //bake coef;
    VoC_mult16_rr(REG5,REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);
    VoC_lw16i(REG0,Tab_huffTabSpecInfo_addr);
    VoC_lw16i(REG1,Tab_huffTabSpec_addr);

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT); //&huffTabSpecInfo[cb - HUFFTAB_SPEC_OFFSET]
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG7,2,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_bngtz16_r(UnpackQuads_L0,REG7);
    VoC_push16(RA,DEFAULT);
    VoC_loop_r(0,REG7);

    VoC_jal(Coolsand_DecodeHuffmanScalar);
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_movreg16(REG7,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG3,-4,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG4,-7,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG5,-10,DEFAULT);
    VoC_shru16_ri(REG6,-13,IN_PARALLEL);
    VoC_shr16_ri(REG3,13,DEFAULT);
    VoC_shr16_ri(REG4,13,IN_PARALLEL);
    VoC_shr16_ri(REG5,13,DEFAULT);
    VoC_shru16_ri(REG7,-1,IN_PARALLEL);//nSignBits = (int)GET_QUAD_SIGNBITS(val);
    VoC_shru16_ri(REG7,13,DEFAULT);
    VoC_shr16_ri(REG6,13,IN_PARALLEL);


    VoC_bez16_r(UnpackQuads_L1,REG7);

    VoC_bez16_r(UnpackQuads_L2,REG3);
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG3,REG7,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);

UnpackQuads_L2:
    VoC_bez16_r(UnpackQuads_L3,REG4);
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG4,REG7,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG7,DEFAULT);
UnpackQuads_L3:
    VoC_bez16_r(UnpackQuads_L4,REG5);
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG5,REG7,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);

UnpackQuads_L4:
    VoC_bez16_r(UnpackQuads_L1,REG6)
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);
UnpackQuads_L1:
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(ACC1_HI,REG6,IN_PARALLEL); //bake REG6;
    VoC_movreg16(ACC0_HI,REG3,DEFAULT);
    VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);//load coef;
    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_lw32z(REG67,IN_PARALLEL);

    VoC_lbinc_p(REG2);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//*coef++ = w;
    VoC_movreg16(REG7,REG4,IN_PARALLEL);
    VoC_shr32_ri(REG67,16,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_movreg16(ACC0_HI,REG5,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,DEFAULT);//*coef++ = x;
    VoC_shr32_ri(ACC0,16,IN_PARALLEL);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_movreg16(REG7,ACC1_HI,IN_PARALLEL);

    VoC_shr32_ri(REG67,16,DEFAULT);//
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//*coef++ = y;

    VoC_sw32inc_p(REG67,REG3,DEFAULT);//*coef++ = z;
    VoC_movreg32(REG01,RL7,IN_PARALLEL);
    VoC_movreg16(ACC1_LO,REG3,DEFAULT); //bake coef;

    VoC_endloop0
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,1,DEFAULT);

UnpackQuads_L0:
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_UnpackPairsNoEsc
 *
 * Description: decode a section of 2-way vector Huffman coded coefficients,
 *                using non-esc tables (5 through 10)
 *
 * Inputs:      REG4-> coef
 *              REG6-> cb
 *              REG7-> nVals
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 * Notes:       assumes nVals is always a multiple of 2 because all scalefactor bands
 *                are a multiple of 4 coefficients long
 **************************************************************************************/
void Coolsand_UnpackPairsNoEsc(void)
{
    VoC_lw16i_short(REG5,HUFFTAB_SPEC_OFFSET,DEFAULT);
    VoC_lw16i_short(REG0,22,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);//cb - HUFFTAB_SPEC_OFFSET
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL); //bake coef;
    VoC_mult16_rr(REG5,REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_lw16i(REG0,Tab_huffTabSpecInfo_addr);
    VoC_lw16i(REG1,Tab_huffTabSpec_addr); //huffTabSpec;

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT); //&huffTabSpecInfo[cb - HUFFTAB_SPEC_OFFSET]
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_bngtz16_r(UnpackPairsNoEsc_L0,REG7);
    VoC_loop_r_short(REG7,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_startloop0

    VoC_jal(Coolsand_DecodeHuffmanScalar);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG5,-6,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);
    VoC_shr16_ri(REG5,11,DEFAULT);//y = GET_PAIR_Y(val);
    VoC_shru16_ri(REG7,-4,IN_PARALLEL);//nSignBits = (int)GET_QUAD_SIGNBITS(val);
    VoC_shru16_ri(REG7,14,DEFAULT);
    VoC_shr16_ri(REG6,11,IN_PARALLEL);//z = GET_PAIR_Z(val);


    VoC_bez16_r(UnpackPairsNoEsc_L1,REG7);//if (nSignBits)


    VoC_bez16_r(UnpackPairsNoEsc_L2,REG5);
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG5,REG7,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);
UnpackPairsNoEsc_L2:
    VoC_bez16_r(UnpackPairsNoEsc_L1,REG6)
    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_shru16_ri(REG7,-15,DEFAULT);
    VoC_shr16_ri(REG7,15,DEFAULT);
    VoC_xor16_rr(REG6,REG7,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);
UnpackPairsNoEsc_L1:
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL); //bake REG6;
    VoC_shr32_ri(REG67,16,DEFAULT);
    VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);//load coef;
    VoC_movreg16(ACC0_HI,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,IN_PARALLEL);//*coef++ = y;
    VoC_lbinc_p(REG2);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//*coef++ = z;
    VoC_movreg32(REG01,RL7,IN_PARALLEL);
    VoC_movreg16(ACC1_LO,REG3,DEFAULT); //bake coef;

    VoC_endloop0
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,1,DEFAULT);

UnpackPairsNoEsc_L0:
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_UnpackPairsEsc
 *
 * Description: decode a section of 2-way vector Huffman coded coefficients,
 *                using esc table (11)
 *
 * Inputs:      REG4-> coef
 *              REG6-> cb
 *              REG7-> nVals
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 * Notes:       assumes nVals is always a multiple of 2 because all scalefactor bands
 *                are a multiple of 4 coefficients long
 **************************************************************************************/
void Coolsand_UnpackPairsEsc(void)
{
    VoC_lw16i_short(REG5,HUFFTAB_SPEC_OFFSET,DEFAULT);
    VoC_lw16i_short(REG0,22,IN_PARALLEL);
    VoC_sub16_rr(REG5,REG6,REG5,DEFAULT);//cb - HUFFTAB_SPEC_OFFSET
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL); //bake coef;
    VoC_mult16_rr(REG5,REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_lw16i(REG0,Tab_huffTabSpecInfo_addr);
    VoC_lw16i(REG1,Tab_huffTabSpec_addr); //huffTabSpec;

    VoC_add16_rr(REG0,REG0,REG5,DEFAULT); //&huffTabSpecInfo[cb - HUFFTAB_SPEC_OFFSET]
    VoC_add16_rr(REG7,REG7,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_bngtz16_r(UnpackPairsEsc_L0,REG7);
    VoC_push16(RA,DEFAULT);
    VoC_loop_r(0,REG7);

    VoC_jal(Coolsand_DecodeHuffmanScalar);
    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_shru16_ri(REG5,-4,DEFAULT);
    VoC_shru16_ri(REG6,-10,IN_PARALLEL);
    VoC_shr16_ri(REG5,10,DEFAULT);//y = GET_ESC_Y(val);
    VoC_shru16_ri(REG7,-2,IN_PARALLEL);//nSignBits = (int)GET_ESC_SIGNBITS(val);
    VoC_shru16_ri(REG7,14,DEFAULT);
    VoC_shr16_ri(REG6,10,IN_PARALLEL);//z = GET_ESC_Y(val);

    VoC_rbinc_r(REG4,REG7,DEFAULT);
    VoC_movreg16(REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG4,16,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG7,REG4,DEFAULT);
    VoC_movreg32(ACC0,REG01,IN_PARALLEL); //bake huffTabSpec and &huffTabSpecInfo[cb - HUFFTAB_SPEC_OFFSET]
    VoC_shru16_rr(REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG0,16,IN_PARALLEL);

    VoC_bne16_rr(UnpackPairsEsc_L1,REG5,REG0);
    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
UnpackPairsEsc_L2:
    VoC_rbinc_i(REG5,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_lbinc_p(REG2);
    VoC_bne16_rr(UnpackPairsEsc_L3,REG5,REG4);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//n++;
    VoC_jump(UnpackPairsEsc_L2);
UnpackPairsEsc_L3:
    VoC_sub16_rr(REG0,REG0,REG1,DEFAULT)
    VoC_rbinc_r(REG5,REG1,DEFAULT);//GetBits(bsi, n);
    VoC_shr16_rr(REG4,REG0,IN_PARALLEL);//(1 << n)
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);//y = (1 << n) + GetBits(bsi, n);
UnpackPairsEsc_L1:

    VoC_lw16i_short(REG0,16,DEFAULT);
    VoC_bne16_rr(UnpackPairsEsc_L4,REG6,REG0);
    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
UnpackPairsEsc_L5:
    VoC_rbinc_i(REG6,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_lbinc_p(REG2);
    VoC_bne16_rr(UnpackPairsEsc_L6,REG6,REG4);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//n++;
    VoC_jump(UnpackPairsEsc_L5);
UnpackPairsEsc_L6:
    VoC_sub16_rr(REG0,REG0,REG1,DEFAULT)
    VoC_rbinc_r(REG6,REG1,DEFAULT);//GetBits(bsi, n);
    VoC_shr16_rr(REG4,REG0,IN_PARALLEL);//(1 << n)
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT);//y = (1 << n) + GetBits(bsi, n);
UnpackPairsEsc_L4:

    VoC_movreg32(REG01,ACC0,DEFAULT); //load huffTabSpec and &huffTabSpecInfo[cb - HUFFTAB_SPEC_OFFSET]

    VoC_bez16_r(UnpackPairsEsc_L7,REG7);//if (nSignBits)

    VoC_bez16_r(UnpackPairsEsc_L8,REG5);//  if (y)  {APPLY_SIGN(y, bitBuf); bitBuf <<= 1;}
    VoC_movreg16(REG4,REG3,DEFAULT)
    VoC_shr16_ri(REG3,15,IN_PARALLEL);
    VoC_xor16_rr(REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);//bitBuf <<= 1;
    VoC_sub16_rr(REG5,REG5,REG3,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);
UnpackPairsEsc_L8:
    VoC_bez16_r(UnpackPairsEsc_L7,REG6);//if (z)    {APPLY_SIGN(z, bitBuf); bitBuf <<= 1;}
    VoC_shr16_ri(REG3,15,DEFAULT);
    VoC_xor16_rr(REG6,REG3,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG3,DEFAULT);
UnpackPairsEsc_L7:
    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL); //bake REG6;
    VoC_shr32_ri(REG67,16,DEFAULT);
    VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);//load coef;
    VoC_movreg16(ACC0_HI,REG4,DEFAULT);
    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_sw32inc_p(REG67,REG3,IN_PARALLEL);//*coef++ = y;
    VoC_lbinc_p(REG2);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//*coef++ = z;
    VoC_movreg32(REG01,RL7,IN_PARALLEL);
    VoC_movreg16(ACC1_LO,REG3,DEFAULT); //bake coef;

    VoC_endloop0
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,1,DEFAULT);

UnpackPairsEsc_L0:
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_DecodeSpectrumLong
 *
 * Description: decode transform coefficients for frame with one long block
 *
 * Inputs:      REG0-> icsInfo+1 (winSequence) INC0=1
 *              REG1-> ch
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6
 *
 * Version 1.0  Create by  Xuml       07/13/2007
 *
 * Notes:       adds in pulse data if present
 *              fills coefficient buffer with zeros in any region not coded with
 *                codebook in range [1, 11] (including sfb's above sfbMax)
 **************************************************************************************/
void Coolsand_DecodeSpectrumLong(void)
{
    VoC_inc_p(REG0,DEFAULT);; //&icsInfo->maxSFB;
    VoC_push16(RA,IN_PARALLEL);

    VoC_movreg16(REG6,REG1,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL); //for &icsInfo->maxSFB;
    /* decode long block */
    VoC_lw16i(REG4,Tab_sfBandTabLongOffset_addr);
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);//&sfBandTabLongOffset[psi->sampRateIdx]
    VoC_lw16i(REG5,Tab_sfBandTabLong_addr);
    VoC_add16_rp(REG0,REG5,REG4,DEFAULT);//sfbTab = sfBandTabShort + sfBandTabShortOffset[psi->sampRateIdx];
    VoC_lw16_p(REG5,REG0,IN_PARALLEL); //icsInfo->maxSFB;

    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_coef_addr,2);
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr);
    VoC_bez16_r(DecodeSpectrumLong_L0,REG6);
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_coef1_addr,2);//coef = psi->coef[ch];
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1);//sfbCodeBook = psi->sfbCodeBook[ch];
    VoC_lw16i(REG4,STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr);
DecodeSpectrumLong_L0:
    VoC_push32(REG01,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);

    VoC_bngtz16_r(DecodeSpectrumLong_L12,REG5)
    VoC_loop_r(1,REG5);//for (sfb = 0; sfb < icsInfo->maxSFB; sfb++) ;
    VoC_lw16inc_p(REG7,REG0,DEFAULT);//sfbTab[sfb];
    VoC_lw16inc_p(REG6,REG3,DEFAULT);//cb = *sfbCodeBook++;

    VoC_sub16_pr(REG7,REG0,REG7,DEFAULT);//nVals = sfbTab[sfb+1] - sfbTab[sfb];
    VoC_movreg16(REG4,REG1,IN_PARALLEL);

    //REG7:nVals; REG4:coef; REG6:cb;REG2:inptr; RL6:bitstream.
    VoC_push16(REG3,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);
    VoC_push16(REG7,DEFAULT);  //bake nVals;


    VoC_bnez16_r(DecodeSpectrumLong_L3,REG6);
    VoC_jal(Coolsand_UnpackZeros);
    VoC_jump(DecodeSpectrumLong_L8);
DecodeSpectrumLong_L3:
    VoC_lw16i_short(REG0,4,DEFAULT);
    VoC_lw16i_short(REG1,10,IN_PARALLEL);

    VoC_bgt16_rr(DecodeSpectrumLong_L4,REG6,REG0);
    VoC_jal(Coolsand_UnpackQuads);
    VoC_jump(DecodeSpectrumLong_L8);
DecodeSpectrumLong_L4:
    VoC_bgt16_rr(DecodeSpectrumLong_L5,REG6,REG1);
    VoC_jal(Coolsand_UnpackPairsNoEsc);
    VoC_jump(DecodeSpectrumLong_L8);
DecodeSpectrumLong_L5:
    // VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_bne16_rd(DecodeSpectrumLong_L6,REG6,const_11_addr);
    VoC_jal(Coolsand_UnpackPairsEsc);
    VoC_jump(DecodeSpectrumLong_L8);
DecodeSpectrumLong_L6:
    VoC_jal(Coolsand_UnpackZeros);
DecodeSpectrumLong_L8:
    VoC_pop16(REG7,DEFAULT);  //nVals
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_pop16(REG3,DEFAULT);
    VoC_shr16_ri(REG7,-1,IN_PARALLEL);
    VoC_add16_rr(REG1,REG7,REG1,DEFAULT); //coef += nVals;

    VoC_endloop1
DecodeSpectrumLong_L12:

    /* fill with zeros above maxSFB */
    VoC_lw16i(REG4,NSAMPS_LONG);
    VoC_sub16_rp(REG7,REG4,REG0,DEFAULT);//nVals = NSAMPS_LONG - sfbTab[sfb];
    VoC_movreg16(REG4,REG1,IN_PARALLEL);//coef;

    //REG7: nVals; REG4:coef;
    VoC_jal(Coolsand_UnpackZeros);

    VoC_pop16(REG4,DEFAULT);//psi->pulseInfo[ch];
    VoC_pop32(REG01,DEFAULT);//sfbTab and psi->coef[ch];
    VoC_pop16(RA,IN_PARALLEL);

    VoC_lw16inc_p(REG7,REG4,DEFAULT);//pi->pulseDataPresent)


    VoC_bez16_r(DecodeSpectrumLong_L9,REG7);
    VoC_lw16inc_p(REG7,REG4,DEFAULT);//load pi->numPulse;
    VoC_lw16i_short(REG6,4,IN_PARALLEL);
    VoC_add16inc_rp(REG0,REG0,REG4,DEFAULT);//&sfbTab[pi->startSFB];
    VoC_add16_rr(REG3,REG4,REG6,DEFAULT); //&pi->amp[0];
    VoC_lw16inc_p(REG5,REG0,DEFAULT);//offset = sfbTab[pi->startSFB];
    VoC_movreg16(REG0,REG4,IN_PARALLEL);//&pi->offset[0];

    VoC_bez16_r(DecodeSpectrumLong_L9,REG7);
    VoC_loop_r_short(REG7,DEFAULT);//for (i = 0; i < pi->numPulse; i++);
    VoC_add16inc_rp(REG5,REG5,REG0,IN_PARALLEL);//offset += pi->offset[i];
    VoC_startloop0

    VoC_shr16_ri(REG5,-1,DEFAULT);// for coef addr
    VoC_add16_rr(REG4,REG5,REG1,DEFAULT);//&coef[offset];
    VoC_lw16inc_p(REG7,REG3,IN_PARALLEL);//pi->amp[i]

    VoC_lw32_p(ACC0,REG4,DEFAULT);// coef[offset]
    VoC_shr32_ri(REG67,16,IN_PARALLEL);//(int)pi->amp[i]
    VoC_bngtz32_r(DecodeSpectrumLong_L10,ACC0);
    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//coef[offset] += pi->amp[i];
    VoC_jump(DecodeSpectrumLong_L11);
DecodeSpectrumLong_L10:
    VoC_bngtz32_r(DecodeSpectrumLong_L11,ACC0);//coef[offset] += pi->amp[i];
    VoC_sub32_rr(ACC0,ACC0,REG67,DEFAULT);//coef[offset] -= pi->amp[i];
DecodeSpectrumLong_L11:
    VoC_add16inc_rp(REG5,REG5,REG0,DEFAULT);//offset += pi->offset[i];
    VoC_sw32_p(ACC0,REG4,DEFAULT);// save coef[offset];

    VoC_endloop0
DecodeSpectrumLong_L9:
    VoC_return;
}

/**************************************************************************************
 * Function:    DecodeSpectrumShort
 *
 * Description: decode transform coefficients for frame with eight short blocks
 *
 * Inputs:      REG0-> icsInfo+1 (winSequence) INC0=1
 *              REG1-> ch
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,RL6
 *
 * Version 1.0  Create by  Xuml       07/11/2007
 *
 * Notes:       fills coefficient buffer with zeros in any region not coded with
 *                codebook in range [1, 11] (including sfb's above sfbMax)
 *              deinterleaves window groups into 8 windows
 **************************************************************************************/

void Coolsand_DecodeSpectrumShort(void)
{
    VoC_movreg16(REG4,REG1,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL); //for &icsInfo->maxSFB;
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_coef_addr,1);
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);
    VoC_bez16_r(DecodeSpectrumShort_L0,REG4);
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_coef1_addr,1);//coef = psi->coef[ch];
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1);//sfbCodeBook = psi->sfbCodeBook[ch];
DecodeSpectrumShort_L0:

    VoC_add16_rr(REG7,REG6,REG0,DEFAULT); //&icsInfo->maxSFB;
    VoC_lw16i_short(REG6,5,IN_PARALLEL);  //for &icsInfo->numWinGroup;

    VoC_add16_rr(REG0,REG6,REG0,DEFAULT); //&icsInfo->numWinGroup;
    VoC_push16(RA,IN_PARALLEL);

    /* decode short blocks, deinterleaving in-place */
    VoC_lw16i(REG4,Tab_sfBandTabShortOffset_addr);
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);//&sfBandTabLongOffset[psi->sampRateIdx]
    VoC_lw16i(REG5,Tab_sfBandTabShort_addr);
    VoC_add16_rp(REG4,REG5,REG4,DEFAULT);//sfbTab = sfBandTabShort + sfBandTabShortOffset[psi->sampRateIdx];
    VoC_lw16_p(REG5,REG7,IN_PARALLEL); //icsInfo->maxSFB;

    VoC_lw16inc_p(REG6,REG0,DEFAULT); // icsInfo->numWinGroup; REG0:icsInfo->winGroupLen[0]
    VoC_movreg32(ACC1,REG45,IN_PARALLEL); //bake sfbTab and icsInfo->maxSFB
    VoC_movreg16(ACC0_LO,REG3,DEFAULT);  //bake coef
    VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL); // load sfbTab
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);


//  VoC_loop_r(1,REG6);

    //for (gp = 0; gp < icsInfo->numWinGroup; gp++)
DecodeSpectrumShort_L10:
    VoC_bngt16_rr(DecodeSpectrumShort_L11,REG6,REG7);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT); //gp++
    VoC_movreg16(REG5,ACC1_HI,DEFAULT); //load icsInfo->maxSFB
    VoC_push32(REG67,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);
    VoC_bngtz16_r(DecodeSpectrumShort_L12,REG5)
    VoC_loop_r(1,REG5);//for (sfb = 0; sfb < icsInfo->maxSFB; sfb++)
    VoC_lw16inc_p(REG6,REG1,DEFAULT);//cb = *sfbCodeBook++;

    VoC_lw16inc_p(REG7,REG3,DEFAULT);//sfbTab[sfb]


    VoC_sub16_pr(REG7,REG3,REG7,DEFAULT);//nVals = sfbTab[sfb+1] - sfbTab[sfb];
    VoC_movreg16(ACC1_LO,REG3,IN_PARALLEL);//bake &sfbTab[sfb+1];

    VoC_lw16_p(REG5,REG0,DEFAULT); //icsInfo->winGroupLen[gp]
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//win = 0;


DecodeSpectrumShort_L2:
    VoC_bngt16_rr(DecodeSpectrumShort_L1,REG5,REG3); //if win < icsInfo->winGroupLen[gp];
    VoC_push32(REG01,DEFAULT);

    VoC_lw16i(REG4,NSAMPS_SHORT);
    VoC_mult16_rr(REG4,REG4,REG3,DEFAULT);//offset = win*NSAMPS_SHORT;
    VoC_movreg16(REG5,ACC0_LO,IN_PARALLEL); // load coef
    VoC_push16(REG5,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);

    // REG6:cb; REG7:nVals; REG4:coef+offset; REG2:inptr; RL6:bitstream.

    VoC_push16(ACC0_LO,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);  //bake  cb and nVals;

    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL); //win++

    VoC_push16(REG3,DEFAULT);
    VoC_add16_rr(REG4,REG5,REG4,IN_PARALLEL);//REG4:coef+offset;


    VoC_bnez16_r(DecodeSpectrumShort_L3,REG6);
    VoC_jal(Coolsand_UnpackZeros);
    VoC_jump(DecodeSpectrumShort_L8);
DecodeSpectrumShort_L3:
    VoC_lw16i_short(REG0,4,DEFAULT);
    VoC_lw16i_short(REG1,10,IN_PARALLEL);

    VoC_bgt16_rr(DecodeSpectrumShort_L4,REG6,REG0);
    VoC_jal(Coolsand_UnpackQuads);
    VoC_jump(DecodeSpectrumShort_L8);
DecodeSpectrumShort_L4:
    VoC_bgt16_rr(DecodeSpectrumShort_L5,REG6,REG1);
    VoC_jal(Coolsand_UnpackPairsNoEsc);
    VoC_jump(DecodeSpectrumShort_L8);
DecodeSpectrumShort_L5:
    VoC_lw16i_short(REG0,11,DEFAULT);
    VoC_bne16_rr(DecodeSpectrumShort_L6,REG6,REG0);
    VoC_jal(Coolsand_UnpackPairsEsc);
    VoC_jump(DecodeSpectrumShort_L8);
DecodeSpectrumShort_L6:
    VoC_jal(Coolsand_UnpackZeros);

DecodeSpectrumShort_L8:
    VoC_pop16(REG3,DEFAULT); //win
    VoC_pop32(REG67,IN_PARALLEL);  //nVals;
    VoC_pop16(ACC0_LO,DEFAULT);
    VoC_pop32(ACC1,IN_PARALLEL);
    VoC_pop16(REG5,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);


    VoC_jump(DecodeSpectrumShort_L2);

DecodeSpectrumShort_L1:
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_movreg16(REG6,ACC0_LO,IN_PARALLEL); // load coef
    VoC_movreg16(REG3,ACC1_LO,DEFAULT);//load &sfbTab[sfb+1];
    VoC_add16_rr(REG6,REG7,REG6,IN_PARALLEL);
    VoC_movreg16(ACC0_LO,REG6,DEFAULT); //coef += nVals*2;

    VoC_endloop1
DecodeSpectrumShort_L12:

    /* fill with zeros above maxSFB */

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);//load &sfbTab[sfb];
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL); //icsInfo->winGroupLen[gp]; gp++;
    VoC_lw16i(REG6,NSAMPS_SHORT);// load NSAMPS_SHORT;
    VoC_movreg16(REG4,ACC0_LO,DEFAULT); // load coef
    VoC_sub16_rp(REG7,REG6,REG7,IN_PARALLEL);//nVals = NSAMPS_SHORT - sfbTab[sfb];
    VoC_push16(REG0,DEFAULT);
    VoC_add16_rr(REG3,REG7,REG4,IN_PARALLEL); //coef += nVals
    VoC_add16_rr(REG3,REG7,REG3,DEFAULT); //coef += nVals
    VoC_lw16i_short(REG0,1,IN_PARALLEL);
    VoC_sub16_rr(REG0,REG5,REG0,DEFAULT); //(icsInfo->winGroupLen[gp] - 1)
    VoC_mult16_rr(REG0,REG0,REG6,DEFAULT);//(icsInfo->winGroupLen[gp] - 1)*NSAMPS_SHORT;
    VoC_lbinc_p(REG2);
    VoC_shr16_ri(REG0,-1,DEFAULT);

    VoC_add16_rr(REG3,REG3,REG0,DEFAULT);//coef += (icsInfo->winGroupLen[gp] - 1)*NSAMPS_SHORT*2;
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);

    VoC_movreg16(ACC0_LO,REG3,DEFAULT); //bake coef
    VoC_lw16i_short(REG3,0,IN_PARALLEL);


    VoC_bez16_r(DecodeSpectrumShort_L9,REG5)//for (win = 0; win < icsInfo->winGroupLen[gp]; win++)


    VoC_loop_r(1,REG5);
    VoC_mult16_rr(REG5,REG3,REG6,DEFAULT);//offset = win*NSAMPS_SHORT*2;
    VoC_push16(REG4,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL); //win++;
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//offset+coef;
    VoC_push32(REG67,IN_PARALLEL);
    //REG7: nVals; REG4:offset+coef;
    VoC_jal(Coolsand_UnpackZeros);
    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_endloop1
DecodeSpectrumShort_L9:

    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);
    VoC_pop16(REG3,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_jump(DecodeSpectrumShort_L10);

DecodeSpectrumShort_L11:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/*********************************huffman.c end************************************************/
/*********************************imdct.c begin************************************************/
#define RND_VAL     (1 << (FBITS_OUT_IMDCT-1))
/**************************************************************************************
 * Function:    Coolsand_DecWindowOverlap
 *
 * Description: apply synthesis window, do overlap-add, clip to 16-bit PCM,
 *                for winSequence LONG-LONG
 *
 * Inputs:      REG0->buf0;
 *              REG2->pcm0;
 *              REG4->over0;
 *              REG6->winTypeCurr;
 *              REG7->winTypePrev;
 *
 * Outputs:    NONE;
 *
 * Used :  all
 *
 * Version 1.0  Created by  Xuml       08/02/2007
 *
 * Notes:       this processes one channel at a time, but skips every other sample in
 *                the output buffer (pcm) for stereo interleaving
 *              this should fit in registers on ARM
 *
 * TODO:        ARM5E version with saturating overlap/add (QADD)
 *              asm code with free pointer updates, better load scheduling
 **************************************************************************************/
void Coolsand_DecWindowOverlap(void)
{

    VoC_lw16i(REG3,1024);

    VoC_add16_rr(REG1,REG0,REG3,DEFAULT);//buf0 += (1024 >> 1);
    VoC_lw16i_short(INC1,-2,IN_PARALLEL);

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16_d(REG3,STRUCT_AACDecInfo_nChans_addr);

    VoC_movreg32(RL6,REG01,DEFAULT);//save buf0 and buf1;
    VoC_inc_p(REG1,IN_PARALLEL);//buf1  = buf0  - 1;

    VoC_lw16i(REG1,1023*2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_add16_rr(REG5,REG4,REG1,IN_PARALLEL);//over1 = over0 + 1024 - 1;

    VoC_shr16_ri(REG1,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    //VoC_mult16_rr(REG1,REG1,REG3,DEFAULT);//(1024 - 1) * nChans
    //VoC_sub16_rr(REG3,REG0,REG3,IN_PARALLEL);//0-nChans;

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);//save  over0 and over1;

    VoC_add16_rr(REG3,REG2,REG1,DEFAULT);//pcm1  = pcm0 + (1024 - 1) * nChans;

    VoC_lw16i(REG1,256);

//wndPrev = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
    VoC_bne16_rd(DecWindowOverlap_L0,REG7,const_1_addr);//if winTypePrev == 1

    VoC_lw16i(REG0,Tab_kbdWindow_addr);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//wndPrev
    VoC_jump(DecWindowOverlap_L1);
DecWindowOverlap_L0:

    VoC_lw16i(REG0,Tab_sinWindow_addr);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//wndPrev

DecWindowOverlap_L1:

    VoC_bne16_rr(DecWindowOverlap_L2,REG6,REG7)//if (winTypeCurr == winTypePrev)
    /* cut window loads in half since current and overlap sections use same symmetric window */
DecWindowOverlap_L3:
    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);//save wndPrev;

    VoC_push32(REG45,DEFAULT);//save w0;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_push32(REG67,DEFAULT);//save w1;

    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_pop32(REG67,IN_PARALLEL);//load w1;

    VoC_pop32(REG45,DEFAULT);//load w2;
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

//  *over1-- = MULSHIFT32(w0, in);
//  *over0++ = MULSHIFT32(w1, in);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over0++ = MULSHIFT32(w1, in);

    VoC_pop16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_movreg32(REG67,RL7,DEFAULT);

    VoC_bgt16_rr(DecWindowOverlap_L3,REG7,REG6);//} while (over0 < over1);

    VoC_jump(DecWindowOverlap_L4);


DecWindowOverlap_L2://else

//  wndCurr = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
    VoC_bne16_rd(DecWindowOverlap_L5,REG6,const_1_addr);//if winTypePrev == 1

    VoC_lw16i(REG0,Tab_kbdWindow_addr);
    VoC_add16_rr(REG1,REG0,REG1,DEFAULT);//wndCurr
    VoC_jump(DecWindowOverlap_L6);
DecWindowOverlap_L5:

    VoC_lw16i(REG0,Tab_sinWindow_addr);
    VoC_add16_rr(REG1,REG0,REG1,DEFAULT);//wndCurr

DecWindowOverlap_L6:

    /* different windows for current and overlap parts - should still fit in registers on ARM w/o stack spill */

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save wndCurr


    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);// load wndCurr;

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//load w0;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//load w1;
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);

//  *over1-- = MULSHIFT32(w0, in);
//  *over0++ = MULSHIFT32(w1, in);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_push16(REG0,IN_PARALLEL);//save wndCurr

    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over0++ = MULSHIFT32(w1, in);

    VoC_pop16(REG1,DEFAULT);//load wndCurr;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);//load wndPrev
    VoC_movreg32(REG67,RL7,IN_PARALLEL);
    VoC_bgt16_rr(DecWindowOverlap_L6,REG7,REG6);//} while (over0 < over1);
DecWindowOverlap_L4:
    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_DecWindowOverlapLongStart
 *
 * Description: apply synthesis window, do overlap-add, clip to 16-bit PCM,
 *                for winSequence LONG-START
 *
 * Inputs:      REG0->buf0;
 *              REG2->pcm0;
 *              REG4->over0;
 *              REG6->winTypeCurr;
 *              REG7->winTypePrev;
 *
 * Outputs:    NONE;
 *
 * Used :  all
 *
 * Version 1.0  Created by  Xuml       08/02/2007
 *
 * Notes:       this processes one channel at a time, but skips every other sample in
 *                the output buffer (pcm) for stereo interleaving
 *              this should fit in registers on ARM
 *
 * TODO:        ARM5E version with saturating overlap/add (QADD)
 *              asm code with free pointer updates, better load scheduling
 **************************************************************************************/
void Coolsand_DecWindowOverlapLongStart(void)
{

    VoC_lw16i(REG3,1024);

    VoC_add16_rr(REG1,REG0,REG3,DEFAULT);//buf0 += (1024 >> 1);
    VoC_lw16i_short(INC1,-2,IN_PARALLEL);

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16_d(REG3,STRUCT_AACDecInfo_nChans_addr);

    VoC_movreg32(RL6,REG01,DEFAULT);//save buf0 and buf1;
    VoC_inc_p(REG1,IN_PARALLEL);//buf1  = buf0  - 1;

    VoC_lw16i(REG1,1023*2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_add16_rr(REG5,REG4,REG1,IN_PARALLEL);//over1 = over0 + 1024 - 1;

    VoC_shr16_ri(REG1,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    //VoC_mult16_rr(REG1,REG1,REG3,DEFAULT);//(1024 - 1) * nChans
    //VoC_sub16_rr(REG3,REG0,REG3,IN_PARALLEL);//0-nChans;

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);//save  over0 and over1;

    VoC_add16_rr(REG3,REG2,REG1,DEFAULT);//pcm1  = pcm0 + (1024 - 1) * nChans;
    VoC_push16(REG6,IN_PARALLEL);//save
    VoC_lw16i(REG1,256);

//wndPrev = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
    VoC_bne16_rd(DecWindowOverlapLongStart_L0,REG7,const_1_addr);//if winTypePrev == 1

    VoC_lw16i(REG0,Tab_kbdWindow_addr);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//wndPrev
    VoC_jump(DecWindowOverlapLongStart_L1);
DecWindowOverlapLongStart_L0:

    VoC_lw16i(REG0,Tab_sinWindow_addr);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//wndPrev


DecWindowOverlapLongStart_L1:
//  i = 448;    /* 2 outputs, 2 overlaps per loop */
//  do {} while (--i);
    VoC_loop_i(1,2);
    VoC_loop_i(0,224);

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_movreg16(REG1,RL6_HI,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_lw32inc_p(REG45,REG1,DEFAULT)//in = *buf1--;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_shr32_ri(REG45,1,IN_PARALLEL);//in >> 1;


    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over1-- = 0;        /* Wn = 0 for n = (2047, 2046, ... 1600) */
    VoC_sw32inc_p(REG45,REG0,DEFAULT);//*over0++ = in >> 1; /* Wn = 1 for n = (1024, 1025, ... 1471) */
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);//save wndPrev;
    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;

    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_pop16(REG6,DEFAULT);
//  wndCurr = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
    VoC_bne16_rd(DecWindowOverlapLongStart_L2,REG6,const_1_addr);//if winTypePrev == 1
    VoC_lw16i(REG1,Tab_kbdWindow_addr);//wndCurr
    VoC_jump(DecWindowOverlapLongStart_L3);
DecWindowOverlapLongStart_L2:
    VoC_lw16i(REG1,Tab_sinWindow_addr);//wndCurr

DecWindowOverlapLongStart_L3:

    /* do 64 more loops - 2 outputs, 2 overlaps per loop */

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save wndCurr


    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);// load wndCurr;

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//w0 = *wndCurr++;    /* W[0], W[1], ... --> W[255], W[254], ... */

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... --> W[128], W[129], ... */
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);
    //*over1-- = MULSHIFT32(w0, in);    /* Wn = short window for n = (1599, 1598, ... , 1536) */
    //*over0++ = MULSHIFT32(w1, in);    /* Wn = short window for n = (1472, 1473, ... , 1535) */


    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_push16(REG0,IN_PARALLEL);//save wndCurr

    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over1-- = MULSHIFT32(w0, in);   /* Wn = short window for n = (1599, 1598, ... , 1536) */
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over0++ = MULSHIFT32(w1, in);   /* Wn = short window for n = (1472, 1473, ... , 1535) */

    VoC_pop16(REG1,DEFAULT);//load wndCurr;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);//load wndPrev
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_bgt16_rr(DecWindowOverlapLongStart_L3,REG7,REG6);//} while (over0 < over1);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_DecWindowOverlapLongStop
 *
 * Description: apply synthesis window, do overlap-add, clip to 16-bit PCM,
 *                for winSequence LONG-STOP
 *
 * Inputs:      REG0->buf0;
 *              REG2->pcm0;
 *              REG4->over0;
 *              REG6->winTypeCurr;
 *              REG7->winTypePrev;
 *
 * Outputs:    NONE;
 *
 * Used :  all
 *
 * Version 1.0  Created by  Xuml       08/02/2007
 *
 * Notes:       this processes one channel at a time, but skips every other sample in
 *                the output buffer (pcm) for stereo interleaving
 *              this should fit in registers on ARM
 *
 * TODO:        ARM5E version with saturating overlap/add (QADD)
 *              asm code with free pointer updates, better load scheduling
 **************************************************************************************/

void Coolsand_DecWindowOverlapLongStop(void)
{
    VoC_lw16i(REG3,1024);

    VoC_add16_rr(REG1,REG0,REG3,DEFAULT);//buf0 += (1024 >> 1);
    VoC_lw16i_short(INC1,-2,IN_PARALLEL);

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16_d(REG3,STRUCT_AACDecInfo_nChans_addr);

    VoC_movreg32(RL6,REG01,DEFAULT);//save buf0 and buf1;
    VoC_inc_p(REG1,IN_PARALLEL);//buf1  = buf0  - 1;

    VoC_lw16i(REG1,1023*2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_add16_rr(REG5,REG4,REG1,IN_PARALLEL);//over1 = over0 + 1024 - 1;

    VoC_shr16_ri(REG1,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    VoC_mult16_rr(REG1,REG1,REG3,DEFAULT);//(1024 - 1) * nChans
    VoC_movreg16(REG3,REG1,IN_PARALLEL);
    //VoC_sub16_rr(REG3,REG0,REG3,IN_PARALLEL);//0-nChans;

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);//save  over0 and over1;

    VoC_add16_rr(REG3,REG2,REG3,DEFAULT);//pcm1  = pcm0 + (1024 - 1) * nChans;

    VoC_lw16i(REG4,256);
//  wndPrev = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[0] : sinWindow + sinWindowOffset[0]);
//  wndCurr = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);

    VoC_bne16_rd(DecWindowOverlapLongStop_L0,REG7,const_1_addr);//if winTypePrev == 1

    VoC_lw16i(REG0,Tab_kbdWindow_addr);//wndPrev
    VoC_jump(DecWindowOverlapLongStop_L1);
DecWindowOverlapLongStop_L0:
    VoC_lw16i(REG0,Tab_sinWindow_addr);//wndPrev
DecWindowOverlapLongStop_L1:

    VoC_bne16_rd(DecWindowOverlapLongStop_L2,REG6,const_1_addr);//if winTypeCurr == 1

    VoC_lw16i(REG1,Tab_kbdWindow_addr);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//wndCurr
    VoC_jump(DecWindowOverlapLongStop_L3);
DecWindowOverlapLongStop_L2:
    VoC_lw16i(REG1,Tab_sinWindow_addr);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//wndCurr
DecWindowOverlapLongStop_L3:

    VoC_push16(REG0,DEFAULT);//save wndPrev;

//  i = 448;    /* 2 outputs, 2 overlaps per loop */
//  do {} while (--i);
    VoC_loop_i(1,2);
    VoC_loop_i(0,224);
    /* Wn = 0 for n = (0, 1, ... 447) */
    /* Wn = 1 for n = (576, 577, ... 1023) */
    VoC_movreg16(REG0,RL6_LO,DEFAULT);// load buf0;
    VoC_push16(REG1,DEFAULT);//save wndCurr

    VoC_lw32inc_p(ACC1,REG0,DEFAULT);//in = *buf0++;
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);

    VoC_shr32_ri(ACC1,1,DEFAULT);//f1 = in >> 1;    /* scale since skipping multiply by Q31 */
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);// load wndCurr;

    VoC_add32_rp(ACC0,REG67,REG0,DEFAULT);//(in  + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//w0 = *wndCurr++;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);

    //*over1-- = MULSHIFT32(w0, in);
    //*over0++ = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_push16(REG0,IN_PARALLEL);//save wndCurr

    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over0++ = MULSHIFT32(w1, in);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_pop16(REG1,DEFAULT);//load wndCurr;


    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_pop16(REG0,DEFAULT);//load wndPrev
    VoC_NOP();

DecWindowOverlapLongStop_L4:

    /* do 64 more loops - 2 outputs, 2 overlaps per loop */

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;    /* W[0], W[1], ...W[63] */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;    /* W[127], W[126], ... W[64] */
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save wndCurr

    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f0
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);// load wndCurr;

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//w0 = *wndCurr++;    /* W[0], W[1], ... --> W[255], W[254], ... */

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... --> W[128], W[129], ... */
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG2,1,DEFAULT);

    //*over1-- = MULSHIFT32(w0, in);    /* Wn = short window for n = (1599, 1598, ... , 1536) */
    //*over0++ = MULSHIFT32(w1, in);    /* Wn = short window for n = (1472, 1473, ... , 1535) */


    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_push16(REG0,IN_PARALLEL);//save wndCurr

    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over1-- = MULSHIFT32(w0, in);   /* Wn = short window for n = (1599, 1598, ... , 1536) */
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over0++ = MULSHIFT32(w1, in);   /* Wn = short window for n = (1472, 1473, ... , 1535) */

    VoC_pop16(REG1,DEFAULT);//load wndCurr;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);//load wndPrev
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_bgt16_rr(DecWindowOverlapLongStop_L4,REG7,REG6);//} while (over0 < over1);

    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_DecWindowOverlapShort
 *
 * Description: apply synthesis window, do overlap-add, clip to 16-bit PCM,
 *                for winSequence EIGHT-SHORT (does all 8 short blocks)
 *
 * Inputs:      REG0->buf0;
 *              REG2->pcm0;
 *              REG4->over0;
 *              REG6->winTypeCurr;
 *              REG7->winTypePrev;
 *
 * Outputs:    NONE;
 *
 * Used :  all
 *
 * Version 1.0  Created by  Xuml       08/02/2007
 *
 * Notes:       this processes one channel at a time, but skips every other sample in
 *                the output buffer (pcm) for stereo interleaving
 *              this should fit in registers on ARM
 *
 * TODO:        ARM5E version with saturating overlap/add (QADD)
 *              asm code with free pointer updates, better load scheduling
 **************************************************************************************/

void Coolsand_DecWindowOverlapShort(void)
{
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);
    VoC_movreg16(REG0,REG4,IN_PARALLEL);//over0
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(RL6_LO,1,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL)
    VoC_shr32_ri(RL6,1-FBITS_OUT_IMDCT,DEFAULT);

    /* pcm[0-447] = 0 + overlap[0-447] */
    VoC_loop_i(0,224);//    i -= 2;
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);//f0 = *over0++;
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);//f1 = *over0++;

    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);//(f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);//(f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_sw16inc_p(ACC1_HI,REG2,DEFAULT);//pcm0 += nChans;

    VoC_endloop0

    VoC_movreg16(REG4,REG0,DEFAULT);//save over0
    VoC_lw16i_short(INC1,-2,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);

    /* pcm[448-575] = Wp[0-127] * block0[0-127] + overlap[448-575] */
    VoC_lw16i(REG3,128);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);//buf0 += 64;
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_sub16_rd(REG5,REG3,STRUCT_AACDecInfo_nChans_addr);//0-nChans;
    VoC_inc_p(REG1,DEFAULT);//buf1  = buf0  - 1;
    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_lw16i(REG3,(128-1)*2);
    VoC_movreg32(RL6,REG01,DEFAULT);//save buf0 and buf1;
    VoC_add16_rr(REG5,REG4,REG3,IN_PARALLEL);//over1 = over0 + 128 - 1;
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_movreg32(RL7,REG45,IN_PARALLEL);//save  over0 and over1;
    //VoC_mult16_rd(REG1,REG3,STRUCT_AACDecInfo_nChans_addr);//(128 - 1) * nChans

//  wndPrev = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[0] : sinWindow + sinWindowOffset[0]);
//  wndCurr = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[0] : sinWindow + sinWindowOffset[0]);

    VoC_add16_rr(REG3,REG2,REG3,DEFAULT);//pcm1  = pcm0 + (128 - 1) * nChans;

    VoC_bne16_rd(DecWindowOverlapShort_L0,REG7,const_1_addr);//if winTypePrev == 1

    VoC_lw16i(REG0,Tab_kbdWindow_addr);//wndPrev
    VoC_jump(DecWindowOverlapShort_L1);
DecWindowOverlapShort_L0:
    VoC_lw16i(REG0,Tab_sinWindow_addr);//wndPrev
DecWindowOverlapShort_L1:

    VoC_bne16_rd(DecWindowOverlapShort_L2,REG6,const_1_addr);//if winTypeCurr == 1
    VoC_lw16i(REG1,Tab_kbdWindow_addr);//wndCurr
    VoC_jump(DecWindowOverlapShort_L3);
DecWindowOverlapShort_L2:
    VoC_lw16i(REG1,Tab_sinWindow_addr);//wndCurr
DecWindowOverlapShort_L3:


DecWindowOverlapShort_L4:

    /* do 64 more loops - 2 outputs, 2 overlaps per loop */

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndPrev++;    /* W[0], W[1], ...W[63] */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndPrev++;    /* W[127], W[126], ... W[64] */
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save wndCurr


    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);//save buf0++

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG0,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);// load wndCurr;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);//(in - f0 + RND_VAL)
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);//w0 = *wndCurr++;    /* W[0], W[1], ... --> W[255], W[254], ... */

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... --> W[128], W[129], ... */
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);
    /* save over0/over1 for next short block, in the slots just vacated */
    //*over1-- = MULSHIFT32(w0, in);    /* Wn = short window for n = (1599, 1598, ... , 1536) */
    //*over0++ = MULSHIFT32(w1, in);    /* Wn = short window for n = (1472, 1473, ... , 1535) */


    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_push16(REG0,IN_PARALLEL);//save wndCurr

    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over1-- = MULSHIFT32(w0, in);   /* Wn = short window for n = (1599, 1598, ... , 1536) */
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over0++ = MULSHIFT32(w1, in);   /* Wn = short window for n = (1472, 1473, ... , 1535) */

    VoC_pop16(REG1,DEFAULT);//load wndCurr;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);//load wndPrev
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_bgt16_rr(DecWindowOverlapShort_L4,REG7,REG6);//} while (over0 < over1);

    /* pcm[576-703] = Wc[128-255] * block0[128-255] + Wc[0-127] * block1[0-127] + overlap[576-703]
     * pcm[704-831] = Wc[128-255] * block1[128-255] + Wc[0-127] * block2[0-127] + overlap[704-831]
     * pcm[832-959] = Wc[128-255] * block2[128-255] + Wc[0-127] * block3[0-127] + overlap[832-959]
     */
    VoC_movreg16(REG0,REG1,DEFAULT);//wndCurr
    VoC_movreg16(REG1,REG0,IN_PARALLEL);//wndPrev

    VoC_loop_i(0,3)//for (i = 0; i < 3; i++)
    VoC_lw16i(REG6,256);
    VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);//wndCurr -= 128;
    VoC_movreg32(REG45,RL6,IN_PARALLEL);//load buf0 and buf1;
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);//save wndCurr and wndPrev
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//buf0 += 64;
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//over0 += 64;

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_sub16_rr(REG5,REG4,REG7,IN_PARALLEL);//buf1 = buf0 - 1;

    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);//128-1
    VoC_movreg32(RL6,REG45,IN_PARALLEL);//save buf0 and buf1;

    VoC_movreg16(REG7,REG6,DEFAULT);//save 128-1;
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);

    VoC_add16_rr(REG1,REG0,REG6,DEFAULT);//over1 = over0 + 128 - 1;
    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_mult16_rr(REG7,REG6,REG7,DEFAULT);//(128 - 1) * nChans
    VoC_shr16_ri(REG6,-6,IN_PARALLEL);//64 * nChans;

    VoC_movreg32(RL7,REG01,DEFAULT);//save over0 and over1;
    VoC_pop32(REG01,IN_PARALLEL);//load wndCurr and wndPrev;

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);//pcm0 += 64 * nChans;
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);//pcm1 = pcm0 + (128 - 1) * nChans;


DecWindowOverlapShort_L5:
    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndCurr++;    /* W[0], W[1], ...W[63] */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... W[64] */
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_push32(REG45,IN_PARALLEL);//save w0;

    VoC_push32(REG67,DEFAULT);//save w1;
    VoC_lw32inc_p(REG23,REG0,IN_PARALLEL);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_lw16i(REG4,256);
    VoC_sub16_rr(REG4,REG0,REG4,DEFAULT);//over0 - 128)
    VoC_sub16_rr(REG5,REG1,REG4,IN_PARALLEL);//over1 - 128)

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG4,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG5,DEFAULT);//in + f1
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);


    VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);//in += *(over0 + 0);      /* from last full frame */
    VoC_add32_rr(ACC1,ACC1,REG67,IN_PARALLEL);//(in + f1 + RND_VAL)

    VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);//in += *(over1 + 0);      /* from last full frame */
    VoC_add32_rr(ACC0,ACC0,REG67,IN_PARALLEL);//(in - f0 + RND_VAL)


    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_pop32(REG67,IN_PARALLEL);//load w1;

    VoC_pop32(REG45,DEFAULT);//load w2;
    VoC_shr32_ri(ACC1,FBITS_OUT_IMDCT-16,IN_PARALLEL);//CLIPTOSHORT( (in + f1 + RND_VAL) >> FBITS_OUT_IMDCT )

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_HI,REG3,DEFAULT);//pcm1 -= nChans;


    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);

    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    /* save over0/over1 for next short block, in the slots just vacated */
    //  *over1-- = MULSHIFT32(w0, in);
    //  *over0++ = MULSHIFT32(w1, in);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over0++ = MULSHIFT32(w1, in);

    VoC_pop16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_movreg32(REG67,RL7,DEFAULT);

    VoC_bgt16_rr(DecWindowOverlapShort_L5,REG7,REG6);//} while (over0 < over1);
    VoC_endloop0

    /* pcm[960-1023] = Wc[128-191] * block3[128-191] + Wc[0-63]   * block4[0-63] + overlap[960-1023]
     * over[0-63]    = Wc[192-255] * block3[192-255] + Wc[64-127] * block4[64-127]
     */

    VoC_lw16i(REG6,256);
    VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);//wndCurr -= 128;
    VoC_movreg32(REG45,RL6,IN_PARALLEL);//load buf0 and buf1;
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);//save wndCurr and wndPrev
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;
    VoC_lw16i(REG5,832*2);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//buf0 += 64;
    VoC_sub16_rr(REG0,REG0,REG5,IN_PARALLEL);//over0 -=832;/* points at overlap[64] */

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_sub16_rr(REG5,REG4,REG7,IN_PARALLEL);//buf1 = buf0 - 1;

    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);//128-1

    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);//save buf0 and buf1;

    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_add16_rr(REG1,REG0,REG6,DEFAULT);//over1 = over0 + 128 - 1; /* points at overlap[191] */
    VoC_shr16_ri(REG7,-6,IN_PARALLEL);//64 * nChans;

    VoC_movreg32(RL7,REG01,DEFAULT);//save over0 and over1;
    VoC_pop32(REG01,IN_PARALLEL);//load wndCurr and wndPrev;

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);//pcm0 += 64 * nChans;

DecWindowOverlapShort_L6:

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndCurr++;    /* W[0], W[1], ...W[63] */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... W[64] */
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_push16(REG0,DEFAULT);//save wndCurr;
    VoC_push32(REG45,IN_PARALLEL);//save w0;


    VoC_push32(REG67,DEFAULT);//save w1;

    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;

    VoC_lw16i(REG4,768*2);
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);//in  = *(over0 + 768);    /* from last short block */
    VoC_add16_rr(REG5,REG1,REG4,IN_PARALLEL);//in  = *(over1 + 768);    /* from last short block */

    VoC_pop32(REG23,DEFAULT);// load pcm0 and pcm1;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sub32_pr(ACC0,REG4,ACC0,DEFAULT);//in - f0
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16i(REG4,896*2);
    VoC_add16_rr(REG4,REG0,REG4,DEFAULT);
    VoC_shr32_ri(REG67,1-FBITS_OUT_IMDCT,IN_PARALLEL);

    VoC_add32_rp(ACC1,ACC1,REG5,DEFAULT);//in + f1
    VoC_add32_rr(ACC0,ACC0,REG67,IN_PARALLEL);//(in - f0 + RND_VAL)
    VoC_lw16i(REG5,256);
    VoC_add32_rp(ACC0,ACC0,REG4,DEFAULT);//in += *(over0 + 896);    /* from last full frame */
    VoC_sub16_rr(REG5,REG1,REG5,IN_PARALLEL);//over1 - 128

    VoC_shr32_ri(ACC0,FBITS_OUT_IMDCT-16,DEFAULT);//CLIPTOSHORT( (in - f0 + RND_VAL) >> FBITS_OUT_IMDCT )
    VoC_pop32(REG67,IN_PARALLEL);//load w1;

    VoC_sw32inc_p(ACC1,REG5,DEFAULT);//*(over1 - 128) = in + f1;;

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//pcm0 += nChans;
    VoC_movreg16(REG1,RL6_HI,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);//load w2;

    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);
    VoC_push32(REG23,DEFAULT);  //save pcm0 and pcm1;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);

    /* save over0/over1 for next short block, in the slots just vacated */
    //  *over1-- = MULSHIFT32(w0, in);
    //  *over0++ = MULSHIFT32(w1, in);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);


    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over0++ = MULSHIFT32(w1, in);

    VoC_pop16(REG0,DEFAULT);//save wndCurr;
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_movreg32(REG67,RL7,DEFAULT);

    VoC_bgt16_rr(DecWindowOverlapShort_L6,REG7,REG6);//} while (over0 < over1);

    /* over0 now points at overlap[128] */

    /* over[64-191]   = Wc[128-255] * block4[128-255] + Wc[0-127] * block5[0-127]
     * over[192-319]  = Wc[128-255] * block5[128-255] + Wc[0-127] * block6[0-127]
     * over[320-447]  = Wc[128-255] * block6[128-255] + Wc[0-127] * block7[0-127]
     * over[448-576]  = Wc[128-255] * block7[128-255]
     */
    VoC_loop_i(0,3);//  for (i = 0; i < 3; i++) {

    VoC_movreg32(REG45,RL6,DEFAULT);//load buf0 and buf1;
    VoC_lw16i(REG6,256);
    VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);//wndCurr -= 128;
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_movreg16(REG0,RL7_LO,DEFAULT);//load over0;
    VoC_movreg16(REG1,REG6,IN_PARALLEL);//over1=256

    VoC_push32(REG01,DEFAULT);//save wndCurr and wndPrev
    VoC_shr16_ri(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//buf0 += 64;
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//over0 += 64;

    VoC_add16_rr(REG1,REG0,REG1,DEFAULT);//over1 = over0 + 128 ;
    VoC_sub16_rr(REG5,REG4,REG7,IN_PARALLEL);//buf1 = buf0 - 1;

    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);//buf1 = buf0 - 1


    VoC_movreg32(RL7,REG01,DEFAULT);//save over0 and over1;
    VoC_pop32(REG01,IN_PARALLEL);//load wndCurr and wndPrev;

    VoC_movreg32(RL6,REG45,DEFAULT);//save buf0 and buf1;

DecWindowOverlapShort_L7:

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//w0 = *wndCurr++;    /* W[0], W[1], ...W[63] */
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//w1 = *wndCurr++;    /* W[127], W[126], ... W[64] */
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);// load buf0;

    VoC_shru16_ri(REG6,1,DEFAULT);

    VoC_lw32inc_p(REG23,REG0,DEFAULT);//in = *buf0++;

    VoC_push32(REG23,DEFAULT);//save pcm0 and pcm1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);


    //f0 = MULSHIFT32(w0, in);
    //f1 = MULSHIFT32(w1, in);
    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);//load over0 and over1;
    /* from last short block */
    VoC_lw16i(REG2,256);

    VoC_sub16_rr(REG2,REG0,REG2,DEFAULT);//over0 - 128)
    VoC_sub16_rr(REG3,REG1,REG2,IN_PARALLEL);//over1 - 128)

    VoC_movreg16(REG1,RL6_HI,DEFAULT);

    VoC_sub32_pr(ACC0,REG2,ACC0,DEFAULT);//*(over0 - 128) -= f0;
    VoC_add32_rp(ACC1,ACC1,REG3,DEFAULT);//*(over1 - 128)+= f1;


    VoC_sw32_p(ACC0,REG2,DEFAULT);//*(over0 - 128) -= f0;
    VoC_lw32inc_p(REG23,REG1,DEFAULT)//in = *buf1--;

    VoC_sw32_p(ACC1,REG3,DEFAULT);//*(over0 - 128) -= f0;
    VoC_shru16_ri(REG2,1,IN_PARALLEL);



    /* save over0/over1 for next short block, in the slots just vacated */
    //  *over1-- = MULSHIFT32(w0, in);
    //  *over0++ = MULSHIFT32(w1, in);

    VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
    VoC_multf32_rr(ACC1,REG3,REG7,IN_PARALLEL);

    VoC_macX_rr(REG2,REG5,DEFAULT);
    VoC_macX_rr(REG2,REG7,IN_PARALLEL);

    VoC_macX_rr(REG3,REG4,DEFAULT);
    VoC_macX_rr(REG3,REG6,IN_PARALLEL);

    VoC_movreg32(REG01,RL7,DEFAULT);//load over0 and over1;
    VoC_movreg16(RL6_HI,REG1,IN_PARALLEL);
    VoC_pop32(REG23,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);//*over1-- = MULSHIFT32(w0, in);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);//*over0++ = MULSHIFT32(w1, in);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);//save wndPrev;
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_bgt16_rr(DecWindowOverlapShort_L7,REG7,REG6);//} while (over0 < over1);

    VoC_endloop0

    /* over[576-1024] = 0 */

    VoC_movreg16(REG0,RL7_LO,DEFAULT);//load over0;
    VoC_lw16i(REG1,128);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);//over0 += 64;
    VoC_loop_i(0,224)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_IMDCT
 *
 * Description: inverse transform and convert to 16-bit PCM
 *
 * Inputs:      REG6->ch;
 *              REG7->chOut;
 *
 * Outputs:     REG0-> 0 if successful, -1 if error
 *
 * Notes:       If AAC_ENABLE_SBR is defined at compile time then window + overlap
 *                does NOT clip to 16-bit PCM and does NOT interleave channels
 *              If AAC_ENABLE_SBR is NOT defined at compile time, then window + overlap
 *                does clip to 16-bit PCM and interleaves channels
 *              If SBR is enabled at compile time, but we don't know whether it is
 *                actually used for this frame (e.g. the first frame of a stream),
 *                we need to produce both clipped 16-bit PCM in outbuf AND
 *                unclipped 32-bit PCM in the SBR input buffer. In this case we make
 *                a separate pass over the 32-bit PCM to produce 16-bit PCM output.
 *                This inflicts a slight performance hit when decoding non-SBR files.
 **************************************************************************************/

void Coolsand_IMDCT(void)
{

    //icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);
    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG3,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_coef_addr,2);
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr,1);

    VoC_bne16_rd(IMDCT_L0,REG6,const_1_addr);  //if ch == 1;
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_coef1_addr,2);
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1,1);
    VoC_be16_rd(IMDCT_L0,REG3,const_1_addr);  //if psi->commonWin == 1;
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,1);
IMDCT_L0:
    VoC_inc_p(REG0,DEFAULT);
    VoC_lw16_p(REG2,REG2,IN_PARALLEL);//psi->gbCurrent[ch]

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//icsInfo->winSequence;
    VoC_lw16_p(REG5,REG0,DEFAULT);//icsInfo->winShape;
    VoC_push16(REG1,DEFAULT);//save psi->coef[ch];
    VoC_push16(REG7,DEFAULT);//save chOut;
    VoC_push32(REG45,IN_PARALLEL);//save icsInfo->winSequence and icsInfo->winShape;

    /* optimized type-IV DCT (operates inplace) */
    VoC_lw16i_short(REG6,2,DEFAULT);

    VoC_bne16_rr(IMDCT_L1,REG4,REG6);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

IMDCT_L2:

    /* 8 short blocks */
    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_bngt16_rr(IMDCT_L3,REG6,REG4)

    VoC_push16(REG1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//i++
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_push32(REG23,DEFAULT);
    VoC_shr16_ri(REG6,-8,IN_PARALLEL);//i*128
    VoC_push16(REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);//psi->coef[ch] + i*128;

    VoC_bltz16_d(IMDCT_short_LS1,GLOBAL_EQ_TYPE);

    // load 0x2000 in RL6
    VoC_lw16i(RL6_LO,0x2000);
    VoC_lw16i_short(RL6_HI,0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_jal(vpp_AmjpAudioEqualizer);
    VoC_NOP();

    VoC_pop32(RL6,DEFAULT);


IMDCT_short_LS1:

    VoC_jal(Coolsand_DCT4);

    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(REG23,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_jump(IMDCT_L2);

IMDCT_L1:

    VoC_bltz16_d(IMDCT_LS1,GLOBAL_EQ_TYPE);

    // load 0x10000 in RL6
    VoC_lw16i_short(RL6_HI,1,DEFAULT);
    VoC_lw16i_short(RL6_LO,0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_jal(vpp_AmjpAudioEqualizer);
    VoC_NOP();

    VoC_pop32(RL6,DEFAULT);

IMDCT_LS1:

    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,GLOBAL_DISPLAY_BARS,1);
    VoC_lw16i(REG3,STRUCT_AACDecInfo_PSInfoBase_coef_addr);
    VoC_bne16_rr(IMDCT_LS3,REG1,REG3);
    VoC_loop_i(1,16);
    VoC_loop_i_short(32,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC1,REG1,DEFAULT);
    VoC_bltz32_r(IMDCT_LS4,ACC1);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_jump(IMDCT_LS5);
IMDCT_LS4:
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
IMDCT_LS5:
    VoC_NOP();
    VoC_endloop0
    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_NOP();
    VoC_endloop1

IMDCT_LS3:

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);
    //REG1:psi->coef[ch];REG2:psi->gbCurrent[ch];REG3:tabidx
    VoC_jal(Coolsand_DCT4);
IMDCT_L3:


    /* window, overlap-add, round to PCM - optimized for each window sequence */
    VoC_pop16(REG1,DEFAULT);//load chOut
    VoC_pop16(REG0,DEFAULT);//load psi->coef[ch];
    VoC_pop32(REG45,IN_PARALLEL);//
    VoC_lw16i(REG2,STRUCT_AACDecInfo_PSInfoBase_overlap_addr);//psi->overlap[chOut]
    VoC_lw16i(REG7,STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr);//psi->prevWinShape[chOut]
    VoC_bez16_r(IMDCT_L4,REG1);
    VoC_lw16i(REG2,STRUCT_AACDecInfo_PSInfoBase_overlap1_addr);//psi->overlap[chOut]
    VoC_lw16i(REG7,STRUCT_AACDecInfo_PSInfoBase_prevWinShape_addr+1);//psi->prevWinShape[chOut]
IMDCT_L4:
    VoC_push16(REG5,DEFAULT);//icsInfo->winShape

    VoC_lw16i(REG3,MONO_outbuf_addr);//outbuf;

    VoC_push16(REG7,DEFAULT);//&prevWinShape[chOut];
    //VoC_add16_rr(REG3,REG3,REG1,IN_PARALLEL);

    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_movreg16(REG1,REG4,IN_PARALLEL);//icsInfo->winSequence;

    VoC_movreg16(REG4,REG2,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL);

    VoC_NOP();
    VoC_push32(REG01,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);

    VoC_jal(vpp_AmjpAacGenerateSinwKbdw);

    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);

    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);


    VoC_lw16i_short(REG3,3,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);//icsInfo->winShape;

    VoC_bnez16_r(IMDCT_L5,REG1);//if (icsInfo->winSequence == 0)
    VoC_jal(Coolsand_DecWindowOverlap);
    VoC_jump(IMDCT_L8);
IMDCT_L5:
    VoC_bne16_rd(IMDCT_L6,REG1,const_1_addr);//else if (icsInfo->winSequence == 1)
    VoC_jal(Coolsand_DecWindowOverlapLongStart);
    VoC_jump(IMDCT_L8);
IMDCT_L6:
    VoC_bne16_rd(IMDCT_L7,REG1,const_2_addr);//else if (icsInfo->winSequence == 2)
    VoC_jal(Coolsand_DecWindowOverlapShort);
    VoC_jump(IMDCT_L8);
IMDCT_L7:
    VoC_bne16_rr(IMDCT_L8,REG1,REG3);//else if (icsInfo->winSequence == 3)
    VoC_jal(Coolsand_DecWindowOverlapLongStop);
IMDCT_L8:
    VoC_pop16(REG7,DEFAULT);//&prevWinShape[chOut];

    //VoC_lw32z(ACC1,DEFAULT);
    VoC_pop16(REG5,DEFAULT);//icsInfo->winShape


//  VoC_pop16(REG6,DEFAULT);//ch
//  VoC_sw16_d(ACC1_HI,STRUCT_AACDecInfo_rawSampleBuf_addr);
//
//  VoC_bez16_r(IMDCT_L9,REG6);
//  VoC_sw16_d(ACC1_HI,STRUCT_AACDecInfo_rawSampleBuf_addr+1);
//
// IMDCT_L9:
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG5,REG7,DEFAULT);
    //VoC_sw32_d(ACC1,STRUCT_AACDecInfo_rawSampleBytes_addr);
//  VoC_sw32_d(ACC1,STRUCT_AACDecInfo_rawSampleFBits_addr);
    VoC_return;
}


/*********************************imdct.c end************************************************/


/*********************************noiseless.c begin************************************************/
/**************************************************************************************
 * Function:    Coolsand_DecodeICSInfo
 *
 * Description: decode individual channel stream info
 *
 * Inputs:      REG0->icsInfo    INC0=1;
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,RL7_HI
 *
 * Version 1.0  Create by  Xuml       07/06/2007
 **************************************************************************************/


void Coolsand_DecodeICSInfo(void)
{

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_rbinc_i(REG5,2,DEFAULT);

    VoC_lbinc_p(REG2);

    VoC_rbinc_i(REG6,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);//icsInfo->icsResBit =GetBits(bsi, 1);//icsInfo->winSequence =GetBits(bsi, 2);

    VoC_sw16inc_p(REG5,REG0,DEFAULT);//icsInfo->winSequence =GetBits(bsi, 2);
    VoC_lw16i_short(REG1,2,IN_PARALLEL); // load 45 short  for numWinGroup address.

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//icsInfo->winShape = GetBits(bsi, 1);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_bne16_rd(DecodeICSInfo_L0,REG5,const_2_addr);
    /* short block */

    VoC_rbinc_i(REG4,4,DEFAULT);
    VoC_rbinc_i(REG5,7,DEFAULT);

    VoC_lbinc_p(REG2);

    VoC_sw16inc_p(REG4,REG0,DEFAULT); //icsInfo->maxSFB = GetBits(bsi, 4);
    VoC_add16_rr(REG0,REG0,REG1,IN_PARALLEL);   //&icsInfo->numWinGroup

    VoC_sw16_p(REG5,REG0,DEFAULT);//icsInfo->sfGroup = GetBits(bsi, 7);
    VoC_movreg16(RL7_HI,REG5,IN_PARALLEL); //bake icsInfo->sfGroup.

    VoC_movreg16(REG1,REG0,DEFAULT); ////&icsInfo->numWinGroup;
    VoC_lw16i_short(REG4,1,IN_PARALLEL); // for l const.

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//icsInfo->numWinGroup = 1;
    VoC_lw16i_short(REG7,0x40,IN_PARALLEL);//mask = 0x40;   /* start with bit 6 */

    VoC_loop_i_short(7,DEFAULT);  //for (g = 0; g < 7; g++)
    VoC_sw16_p(REG6,REG0,IN_PARALLEL);//icsInfo->winGroupLen[0] = 1;
    VoC_startloop0

    VoC_sub16_rr(REG3,REG6,REG4,DEFAULT);//icsInfo->numWinGroup - 1
    VoC_movreg16(REG5,RL7_HI,IN_PARALLEL);

    VoC_and16_rr(REG5,REG7,DEFAULT);  //icsInfo->sfGroup & mask
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL); //&icsInfo->winGroupLen[icsInfo->numWinGroup - 1]

    VoC_bez16_r(DecodeICSInfo_L1,REG5);//if (icsInfo->sfGroup & mask)
    //VoC_lw16_p(REG5,REG3,DEFAULT);  //icsInfo->winGroupLen[icsInfo->numWinGroup - 1];
    VoC_add16_rp(REG5,REG4,REG3,DEFAULT); //icsInfo->winGroupLen[icsInfo->numWinGroup - 1]++;
    VoC_jump(DecodeICSInfo_L2);
DecodeICSInfo_L1:
    VoC_add16_rr(REG6,REG6,REG4,DEFAULT); //icsInfo->numWinGroup++;
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);//(&icsInfo->winGroupLen)++;
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_sw16_p(REG6,REG1,DEFAULT);  // save icsInfo->numWinGroup;
DecodeICSInfo_L2:
    VoC_shr16_ri(REG7,1,DEFAULT);  //mask >>= 1;
    VoC_sw16_p(REG5,REG3,IN_PARALLEL); //save icsInfo->winGroupLen[icsInfo->numWinGroup - 1];
    VoC_endloop0
    VoC_jump(DecodeICSInfo_End);
DecodeICSInfo_L0:

    /* long block */

    VoC_rbinc_i(REG4,6,DEFAULT);
    VoC_rbinc_i(REG5,1,DEFAULT);

    VoC_lbinc_p(REG2);

    VoC_sw16inc_p(REG4,REG0,DEFAULT); //icsInfo->maxSFB =GetBits(bsi, 6);
    VoC_add16_rr(REG1,REG1,REG0,IN_PARALLEL); //&icsInfo->numWinGroup;

    VoC_inc_p(REG0,DEFAULT); //&icsInfo->predictorDataPresent;
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL); // &icsInfo->winGroupLen[0]

    VoC_sw16_p(REG6,REG1,DEFAULT); //icsInfo->numWinGroup = 1;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//icsInfo->predictorDataPresent = GetBits(bsi, 1);

    VoC_sw16_p(REG6,REG1,DEFAULT); //icsInfo->winGroupLen[0] = 1;


DecodeICSInfo_End:
    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_DecodeSectionData
 *
 * Description: decode section data (scale factor band groupings and
 *                associated Huffman codebooks)
 *
 * Inputs:      INC0=1;
 *              REG0-> icsInfo
 *              REG1-> ch, index of current channel
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 *
 * Notes:       sectCB, sectEnd, sfbCodeBook, ordered by window groups for short blocks
 **************************************************************************************/


void Coolsand_DecodeSectionData(void)
{

#if 0

    voc_short LOCAL_DecodeSectionData_maxSFB,x

#endif

    VoC_inc_p(REG0,DEFAULT);  //&winSequence
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG0,DEFAULT);  //load  winSequence
    VoC_lw16i_short(REG3,2,IN_PARALLEL);

    VoC_inc_p(REG0,DEFAULT); //&maxSFB


    //sectLenBits = (winSequence == 2 ? 3 : 5);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);  // sectLenBits
    VoC_be16_rr(DecodeSectionData_L0,REG5,REG3) //if winSequence == 2
    VoC_lw16i_short(REG7,5,DEFAULT);  // sectLenBits
DecodeSectionData_L0:

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_shr16_rr(REG3,REG5,DEFAULT); //(1 << sectLenBits)
    VoC_lw16_p(REG5,REG0,IN_PARALLEL);// load maxSFB

    VoC_sub16_rr(REG6,REG3,REG6,DEFAULT);// sectEscapeVal = (1 << sectLenBits) - 1;
    VoC_add16_rr(REG0,REG0,REG4,IN_PARALLEL); //&numWinGrp



    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1); //load sfbCodeBook
    VoC_bnez16_r(DecodeSectionData_L1,REG1)
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);  //load sfbCodeBook
DecodeSectionData_L1:

    VoC_lw16_p(REG0,REG0,DEFAULT); //load numWinGrp;
    VoC_sw16_d(REG5,LOCAL_DecodeSectionData_maxSFB);


    VoC_loop_r(1,REG0);
    VoC_lw16i_short(REG0,0,DEFAULT);
DecodeSectionData_L2:
    VoC_bnlt16_rd(DecodeSectionData_End,REG0,LOCAL_DecodeSectionData_maxSFB)  //while (sfb < maxSFB)
    VoC_rbinc_i(REG4,4,DEFAULT);  //cb = GetBits(bsi, 4);   /* next section codebook */
    VoC_lw16i_short(REG1,0,IN_PARALLEL); //sectLen = 0;
DecodeSectionData_L3:
    VoC_rbinc_r(REG5,REG7,DEFAULT);  //sectLenIncr = GetBits(bsi, sectLenBits);
    VoC_lbinc_p(REG2);
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);//sectLen += sectLenIncr;
    VoC_be16_rr(DecodeSectionData_L3,REG5,REG6);
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);//sfb += sectLen;

    VoC_bez16_r(DecodeSectionData_L4,REG1);

    VoC_loop_r_short(REG1,DEFAULT);  //while (sectLen--)
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG3,DEFAULT); //*sfbCodeBook++ = (unsigned char)cb;
    VoC_endloop0
    //  VoC_lbinc_p(REG2);
    //  VoC_sw16_d(REG0,Output_ImgWidth_addr);

DecodeSectionData_L4:

    VoC_jump(DecodeSectionData_L2);
DecodeSectionData_End:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_endloop1


//  VoC_NOP();
//  VoC_bnez16_r(DecodeSectionData_L1_test,REG1)
//  VoC_sw16_d(REG2,Output_mode_addr);
//  VoC_sw16_d(BITCACHE,Output_reserv2_addr);
//  VoC_sw16_d(REG5,Output_ImgHeight_addr);
//DecodeSectionData_L1_test:
//      VoC_NOP();
//  VoC_NOP();
    VoC_return;
}
/**************************************************************************************
 * Function:    DecodeOneScaleFactor
 *
 * Description: decode one scalefactor using scalefactor Huffman codebook
 *
 * Inputs:      REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG6-> one decoded scalefactor, including index_offset of -60
 *
 *
 * Used : REG0,REG1,REG2,REG6,RL6,
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 *
 **************************************************************************************/

void Coolsand_DecodeOneScaleFactor(void)
{
    /* decode next scalefactor from bitstream */
    VoC_push16(REG3,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL);
    VoC_push32(REG01,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,Tab_huffTabScaleFactInfo_addr,1);
    VoC_lw16i_set_inc(REG1,Tab_huffTabScaleFact_addr,1);

    VoC_jal(Coolsand_DecodeHuffmanScalar);

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_pop16(REG3,DEFAULT);
    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_DecodeScaleFactors
 *
 * Description: decode scalefactors, PNS energy, and intensity stereo weights
 *
 * Inputs:      INC0=3;
 *              REG0-> icsInfo
 *              REG1-> ch, index of current channel
 *              REG2-> inptr;
 *              REG7-> globalGain;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 *
 * Notes:       sfbCodeBook, scaleFactors ordered by window groups for short blocks
 *              for section with codebook 13, scaleFactors buffer has decoded PNS
 *                energy instead of regular scalefactor
 *              for section with codebook 14 or 15, scaleFactors buffer has intensity
 *                stereo weight instead of regular scalefactor
 **************************************************************************************/
void Coolsand_DecodeScaleFactors(void)
{
    VoC_push16(RA,DEFAULT);

    /* starting values for differential coding */
    VoC_inc_p(REG0,DEFAULT);  //&maxSFB
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_lw16_p(REG5,REG0,DEFAULT);// load maxSFB
    VoC_add16_rr(REG6,REG0,REG4,IN_PARALLEL); //&numWinGrp

    VoC_lw16i(REG4,346);//90+256

    VoC_movreg16(REG3,REG1,DEFAULT);
    VoC_lw16_p(REG6,REG6,IN_PARALLEL); //load numWinGrp;

    VoC_mult16_rr(REG6,REG6,REG5,DEFAULT);//numWinGrp * maxSFB
    VoC_sub16_rr(REG5,REG7,REG4,IN_PARALLEL);////nrg = globalGain - 90 - 256;



    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1); //load sfbCodeBook
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr,1); // load scaleFactors

    VoC_bnez16_r(DecodeScaleFactors_L1,REG3)
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook_addr,1);  //load sfbCodeBook
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_scaleFactors_addr,1); // load scaleFactors
DecodeScaleFactors_L1:

    VoC_lw16i_short(REG4,1,DEFAULT);  //npf = 1;
    VoC_lw16i_short(REG3,0,IN_PARALLEL); //is = 0;

    VoC_bngtz16_r(DecodeScaleFactors_End,REG6)

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    //if (sfbCB  == 14 || sfbCB == 15)
    /* intensity stereo - differential coding */
    VoC_be16_rd(DecodeScaleFactors_L2,REG6,const_14_addr);
    VoC_be16_rd(DecodeScaleFactors_L2,REG6,const_15_addr);
    VoC_jump(DecodeScaleFactors_L3);
DecodeScaleFactors_L2:
    VoC_jal(Coolsand_DecodeOneScaleFactor);//val = DecodeOneScaleFactor(bsi);
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);  //REG6=val;//is += val;
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);//*scaleFactors++ = (short)is;
    VoC_jump(DecodeScaleFactors_L6);
DecodeScaleFactors_L3:
    //else if (sfbCB == 13)
    /* PNS - first energy is directly coded, rest are Huffman coded (npf = noise_pcm_flag) */
    VoC_bne16_rd(DecodeScaleFactors_L4,REG6,const_13_addr);
    VoC_bez16_r(DecodeScaleFactors_L4_sub1,REG4);//if (npf)
    VoC_rbinc_i(REG6,9,DEFAULT);//val = GetBits(bsi, 9);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);  //npf = 0;
    VoC_jump(DecodeScaleFactors_L4_sub2);
DecodeScaleFactors_L4_sub1:
    VoC_jal(Coolsand_DecodeOneScaleFactor);//val = DecodeOneScaleFactor(bsi);REG6=val;
DecodeScaleFactors_L4_sub2:
    VoC_add16_rr(REG5,REG6,REG5,DEFAULT);  //   nrg += val;
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG5,REG1,DEFAULT);  //*scaleFactors++ = (short)nrg;
    VoC_jump(DecodeScaleFactors_L6);
DecodeScaleFactors_L4:
    //  else if (sfbCB >= 1 && sfbCB <= 11)
    /* regular (non-zero) region - differential coding */
    VoC_blt16_rd(DecodeScaleFactors_L5,REG6,const_1_addr);
    VoC_bgt16_rd(DecodeScaleFactors_L5,REG6,const_11_addr);
    VoC_jal(Coolsand_DecodeOneScaleFactor);//val = DecodeOneScaleFactor(bsi);REG6=val;
    VoC_add16_rr(REG7,REG7,REG6,DEFAULT);//sf += val;
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);//**scaleFactors++ = (short)sf;
    VoC_jump(DecodeScaleFactors_L6);
DecodeScaleFactors_L5:
    //else
    /* inactive scalefactor band if codebook 0 */
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//*scaleFactors++ = 0;
DecodeScaleFactors_L6:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_endloop0
DecodeScaleFactors_End:
    VoC_pop16(RA,DEFAULT);
    VoC_lbinc_p(REG2);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_DecodePulseInfo
 *
 * Description: decode pulse information
 *
 * Inputs:      INC3=1;
 *              REG3-> pi+1
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 *
 * Used : REG0,REG2,REG3,REG4,REG5,REG7,RL6,INC0
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 **************************************************************************************/

void Coolsand_DecodePulseInfo(void)
{
    VoC_rbinc_i(REG4,2,DEFAULT);
    VoC_rbinc_i(REG5,6,DEFAULT);
    VoC_lbinc_p(REG2);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG7,6,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG0,DEFAULT);   /* add 1 here */
    VoC_add16_rr(REG0,REG3,REG7,IN_PARALLEL); //&pi->amp[0]
    VoC_sw16inc_p(REG4,REG3,DEFAULT);  //pi->numPulse = GetBits(bsi, 2) + 1;        /* add 1 here */
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);//pi->startSFB = GetBits(bsi, 6);

    VoC_rb_i(REG4,5,DEFAULT);  //noadvance pi->offset[i] = GetBits(bsi, 5);


    VoC_bez16_r(DecodePulseInfo_L0,REG4);
    VoC_rbinc_i(REG4,5,DEFAULT);  //pi->offset[i] = GetBits(bsi, 5);
    VoC_rb_i(REG5,4,DEFAULT);//noadvance pi->amp[i] = GetBits(bsi, 4);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG4,REG3,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_rbinc_i(REG5,4,DEFAULT);//pi->amp[i] = GetBits(bsi, 4);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_lbinc_p(REG2);
DecodePulseInfo_L0:
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_DecodeTNSInfo
 *
 * Description: decode TNS filter information
 *
 * Inputs:      REG0-> ti+1
 *              REG2-> inptr;
 *              REG7-> winSequence
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,ACC0,ACC1,INC0,INC1,INC3
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 **************************************************************************************/
void Coolsand_DecodeTNSInfo(void)
{
    VoC_lw16i_short(REG6,MAX_TNS_FILTERS,DEFAULT);
    VoC_movreg16(REG1,REG0,IN_PARALLEL); //&ti->numFilt[0]

    VoC_add16_rr(REG1,REG1,REG6,DEFAULT); //&ti->coefRes[0]
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG1,REG6,DEFAULT); //&ti->length[0]
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG3,REG6,DEFAULT); //&ti->order[0]
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_add16_rr(REG5,REG4,REG6,DEFAULT); //&ti->dir[0]
    VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL); //back &ti->order[0];

    VoC_add16_rr(REG6,REG5,REG6,DEFAULT); //&ti->coef[0]
    VoC_lw16i_short(REG4,2,IN_PARALLEL);

    VoC_bne16_rr(DecodeTNSInfo_L0,REG7,REG4);//if (winSequence == 2)
    VoC_rb_i(REG7,1,DEFAULT); // no advance;
    /* short blocks */
    VoC_loop_i(1,NWINDOWS_SHORT);//for (w = 0; w < NWINDOWS_SHORT; w++)
    VoC_rbinc_i(REG7,1,DEFAULT);//ti->numFilt[w] = GetBits(bsi, 1);
    VoC_sw16inc_p(REG7,REG0,IN_PARALLEL);
    VoC_bez16_r(DecodeTNSInfo_L1,REG7);//if (ti->numFilt[w])


    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_rbinc_i(REG4,4,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_movreg16(REG0,ACC1_HI,DEFAULT); //load &ti->order[0];
    VoC_movreg32(ACC0,REG01,IN_PARALLEL); //bake &ti->numFilt[w] and &ti->coefRes[w]

    VoC_rbinc_i(REG7,3,DEFAULT);
    VoC_sw16_p(REG7,REG1,IN_PARALLEL);//ti->coefRes[w] = GetBits(bsi, 1) + 3;

    VoC_sw16inc_p(REG4,REG3,DEFAULT);//*filtLength =    GetBits(bsi, 4);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*filtOrder =     GetBits(bsi, 3);
    VoC_movreg16(ACC1_HI,REG0,IN_PARALLEL); //bake filtOrder


    VoC_bez16_r(DecodeTNSInfo_L2,REG7);
    VoC_movreg16(REG0,REG6,DEFAULT) // load tnsCoef;

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_push16(REG7,IN_PARALLEL); //bake *filtOrder

    VoC_rbinc_i(REG7,1,DEFAULT);
    VoC_lw16_p(REG6,REG1,IN_PARALLEL);


    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);//coefBits = (int)ti->coefRes[0] - compress;   /* 2, 3, or 4 */
    VoC_movreg16(REG1,REG5,IN_PARALLEL); //load &ti->dir[0]

    VoC_lw16i_short(REG7,-4,DEFAULT); // 1111 1111 1111 1100
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT); //*filtDir++ =     GetBits(bsi, 1);
    VoC_sub16_rr(REG4,REG5,REG6,IN_PARALLEL);//-(coefBits - 2)

    VoC_shru16_rr(REG5,REG4,DEFAULT);  //s = sgnMask[coefBits - 2];
    VoC_shru16_rr(REG7,REG4,IN_PARALLEL);//n = negMask[coefBits - 2];

    VoC_pop16(REG4,DEFAULT); // load *filtOrder

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_movreg16(ACC1_LO,REG5,IN_PARALLEL);
    VoC_startloop0

    VoC_rbinc_r(REG4,REG6,DEFAULT);//c = GetBits(bsi, coefBits);

    VoC_and16_rr(REG5,REG4,DEFAULT);//c & s
    VoC_bez16_r(DecodeTNSInfo_L3,REG5)//if (c & s)
    VoC_or16_rr(REG4,REG7,DEFAULT);//c |= n;
DecodeTNSInfo_L3:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*tnsCoef++ = c;
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);
    VoC_endloop0

    VoC_movreg16(REG6,REG0,DEFAULT);        //save tnsCoef
    VoC_movreg16(REG5,REG1,IN_PARALLEL);    //save &ti->dir[0]

DecodeTNSInfo_L2:
    VoC_movreg32(REG01,ACC0,DEFAULT);//load &ti->numFilt[w] and &ti->coefRes[w]

DecodeTNSInfo_L1:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_rb_i(REG7,1,DEFAULT); // no advance;
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_movreg32(ACC0,REG01,DEFAULT);
    VoC_endloop1
    VoC_jump(DecodeTNSInfo_End);
DecodeTNSInfo_L0://else
    /* long blocks */

    VoC_rbinc_i(REG7,2,DEFAULT);
    VoC_movreg16(REG4,ACC1_HI,IN_PARALLEL);
    VoC_movreg32(ACC0,REG45,DEFAULT);  //bake filtOrder and filtDir;
    VoC_lw16i_short(REG4,3,IN_PARALLEL);
    VoC_rb_i(REG5,1,DEFAULT);    //no advance
    VoC_sw16_p(REG7,REG0,IN_PARALLEL); //ti->numFilt[0] = GetBits(bsi, 2);


    VoC_bez16_r(DecodeTNSInfo_End,REG7);//if (ti->numFilt[w])

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);//GetBits(bsi, 1) + 3;

    VoC_movreg16(ACC1_HI,REG5,DEFAULT);

    VoC_sw16_p(REG5,REG1,DEFAULT);//ti->coefRes[0] = GetBits(bsi, 1) + 3;
    VoC_movreg32(REG01,ACC0,IN_PARALLEL); //load filtOrder and filtDir;

    VoC_loop_r(1,REG7);

    VoC_rbinc_i(REG7,6,DEFAULT);
    VoC_rbinc_i(REG4,5,DEFAULT);

    VoC_lbinc_p(REG2);

    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*filtOrder =     GetBits(bsi, 5);

    VoC_sw16inc_p(REG7,REG3,DEFAULT);//*filtLength =    GetBits(bsi, 6);
    VoC_movreg16(ACC0_LO,REG0,IN_PARALLEL); //bake filtOrder

    VoC_bez16_r(DecodeTNSInfo_L4,REG4);
    VoC_movreg16(REG0,REG6,DEFAULT) // load tnsCoef;

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL); //bake *filtOrder

    VoC_rbinc_i(REG5,1,DEFAULT);
    VoC_movreg16(REG6,ACC1_HI,IN_PARALLEL);//load ti->coefRes[0]


    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//coefBits = (int)ti->coefRes[0] - compress;   /* 2, 3, or 4 */
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG5,REG6,DEFAULT);//-(coefBits - 2)
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL); //*filtDir++ =     GetBits(bsi, 1);

    VoC_lw16i_short(REG7,-4,DEFAULT); // 1111 1111 1111 1100
    VoC_shru16_rr(REG5,REG4,DEFAULT);  //s = sgnMask[coefBits - 2];

    VoC_shru16_rr(REG7,REG4,DEFAULT);////n = negMask[coefBits - 2];
    VoC_pop16(REG4,IN_PARALLEL); // load *filtOrder

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_movreg16(ACC1_LO,REG5,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_r(REG4,REG6,DEFAULT);//c = GetBits(bsi, coefBits);

    VoC_and16_rr(REG5,REG4,DEFAULT);//c & s
    VoC_bez16_r(DecodeTNSInfo_L5,REG5)//if (c & s)
    VoC_or16_rr(REG4,REG7,DEFAULT);//c |= n;
DecodeTNSInfo_L5:
    VoC_NOP();
    VoC_lbinc_p(REG2);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*tnsCoef++ = c;
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);
    VoC_endloop0

    VoC_movreg16(REG6,REG0,DEFAULT);        //save tnsCoef
    VoC_movreg16(REG0,ACC0_LO,IN_PARALLEL); //load filtOrder
DecodeTNSInfo_L4:
    VoC_NOP();
    VoC_lbinc_p(REG2);

    VoC_endloop1

DecodeTNSInfo_End:
    VoC_return;

}
/**************************************************************************************
 * Function:    Coolsand_DecodeICS
 *
 * Description: decode individual channel stream
 *
 * Inputs:      INC0=1;
 *              REG0-> icsInfo  (icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);)
 *              REG1-> ch, index of current channel
 *              REG2-> inptr;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/10/2007
 **************************************************************************************/

void Coolsand_DecodeICS(void)
{



    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);  //bake icsInfo and ch
    VoC_movreg16(ACC1_HI,REG4,DEFAULT);  // bake globalGain
    VoC_push16(RA,IN_PARALLEL);
    VoC_bnez16_d(DecodeICS_L0,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_push16(ACC1_HI,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);
    VoC_jal(Coolsand_DecodeICSInfo);
    VoC_pop32(RL7,DEFAULT);
    VoC_pop16(ACC1_HI,IN_PARALLEL);
DecodeICS_L0:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);   //load icsInfo and ch



    VoC_jal(Coolsand_DecodeSectionData);
    VoC_lw16i_short(INC0,3,DEFAULT);
    VoC_movreg32(REG01,RL7,IN_PARALLEL);   //load icsInfo and ch
    VoC_movreg16(REG7,ACC1_HI,DEFAULT);    //load globalGain
    VoC_push32(RL7,IN_PARALLEL);






    VoC_jal(Coolsand_DecodeScaleFactors);

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);  //load icsInfo and ch

    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_pulseInfo1_addr,1);  //pi = &psi->pulseInfo[ch];
    VoC_lw16i(REG0,STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr);  //ti = &psi->tnsInfo[ch];
//  VoC_lw16i(REG1,STRUCT_AACDecInfo_PSInfoBase_gainControlInfo1_addr);


    VoC_bnez16_r(DecodeICS_L1,REG7);    //if ch==1
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_pulseInfo_addr,1); //pi = &psi->pulseInfo[ch];
    VoC_lw16i(REG0,STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr);  //ti = &psi->tnsInfo[ch];
//  VoC_lw16i(REG1,STRUCT_AACDecInfo_PSInfoBase_gainControlInfo_addr);    //gi = &psi->gainControlInfo[ch];
DecodeICS_L1:

    VoC_sw16inc_p(REG4,REG3,DEFAULT);   //pi->pulseDataPresent = GetBits(bsi, 1);
    VoC_movreg32(RL7,REG01,IN_PARALLEL); //bake ti and gi
    VoC_bez16_r(DecodeICS_L2,REG4);
    VoC_jal(Coolsand_DecodePulseInfo);  //REG6 should be not used.REG6:icsInfo
DecodeICS_L2:

    VoC_rbinc_i(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT); //&icsInfo->winSequence
    VoC_movreg32(REG01,RL7,IN_PARALLEL); //load  ti and gi
    VoC_lbinc_p(REG2);
    VoC_lw16_p(REG7,REG6,DEFAULT);//icsInfo->winSequence
    VoC_sw16inc_p(REG4,REG0,DEFAULT); //ti->tnsDataPresent = GetBits(bsi, 1);
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);// bake icsInfo->winSequence
    VoC_bez16_r(DecodeICS_L3,REG4);
    VoC_jal(Coolsand_DecodeTNSInfo);
DecodeICS_L3:

    VoC_rbinc_i(REG4,1,DEFAULT);
//  VoC_movreg32(REG01,RL7,IN_PARALLEL); //load icsInfo->winSequence and gi
//  VoC_lbinc_p(REG2);
//  VoC_sw16_p(REG4,REG1,DEFAULT); //gi->gainControlDataPresent = GetBits(bsi, 1);
//  VoC_bez16_r(DecodeICS_L4,REG4);
//  VoC_lbinc_p(REG2);
//  VoC_jal(Coolsand_DecodeGainControlInfo);
// DecodeICS_L4:



    VoC_pop16(RA,DEFAULT);

    VoC_lbinc_p(REG2);
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_DecodeNoiselessData
 *
 * Description: decode noiseless data (side info and transform coefficients)
 *
  * Inputs:     REG1-> ch, index of current channel
 *              REG2-> inptr;
 *              REG4-> huffTableLoaded;
 *              RL6 -> bitstream;
 *
 * Outputs:     REG2-> updated inptr;
 *              RL6 -> updated bitstream;
 *              REG0-> 0 if successful, error code (< 0) if error;
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,RL6,RL7
 *
 * Version 1.0  Create by  Xuml       07/09/2007
 **************************************************************************************/

void Coolsand_DecodeNoiselessData(void)
{

    VoC_bnez16_r(AACDecode_DMA_L0,REG4)

    VoC_push16(RA,DEFAULT);
    VoC_jal(vpp_AmjpAacGenerateHuffTabSpecInfo);
    VoC_pop16(RA,DEFAULT);

AACDecode_DMA_L0:

    VoC_lw16i_short(REG4,1,DEFAULT);

//icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);
    VoC_lw16_d(REG3,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);

    VoC_bne16_rr(DecodeNoiselessData_L0,REG1,REG4);  //if ch == 1;
    VoC_be16_rr(DecodeNoiselessData_L0,REG3,REG4);  //if psi->commonWin == 1;
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,1);
DecodeNoiselessData_L0:
    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,DEFAULT); //bake icsInfo and ch;
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_jal(Coolsand_DecodeICS);

    VoC_pop32(REG01,DEFAULT); //load icsInfo and ch;
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);


    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_bne16_rr(DecodeNoiselessData_L1,REG4,REG3);//if (icsInfo->winSequence == 2)
    VoC_jal(Coolsand_DecodeSpectrumShort);  //DecodeSpectrumShort(psi, &bsi, ch);
    VoC_jump(DecodeNoiselessData_L2);
DecodeNoiselessData_L1:
    VoC_jal(Coolsand_DecodeSpectrumLong);  //DecodeSpectrumLong(psi, &bsi, ch);
DecodeNoiselessData_L2:

    VoC_pop16(REG1,DEFAULT);//load ch;
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_sbDeinterleaveReqd_addr,1);
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr);

    VoC_bez16_r(DecodeNoiselessData_L3,REG1);   //if ch==1
    VoC_or16_rd(REG4,STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_tnsDataPresent_addr);
    VoC_inc_p(REG3,DEFAULT);
    VoC_jump(DecodeNoiselessData_L4);
DecodeNoiselessData_L3:
    VoC_or16_rd(REG4,STRUCT_AACDecInfo_PSInfoBase_tnsInfo_tnsDataPresent_addr);
DecodeNoiselessData_L4:
    VoC_sw16_p(REG0,REG3,DEFAULT); //aacDecInfo->sbDeinterleaveReqd[ch] = 0;
    VoC_sw16_d(REG4,STRUCT_AACDecInfo_tnsUsed_addr);  //    aacDecInfo->tnsUsed |= psi->tnsInfo[ch].tnsDataPresent; /* set flag if TNS used for any channel */

    VoC_lw16i_short(REG0,0,DEFAULT);// for return;
//DecodeNoiselessData_End:

    VoC_return;
}

/*********************************noiseless.c end************************************************/



/*********************************stproc.c begin************************************************/
/**************************************************************************************
 * Function:    StereoProcessGroup
 *
 * Description: apply mid-side and intensity stereo to group of transform coefficients
 *
 * Inputs:      REG2->coefL; INC0=1,INC1=1,INC2=2,INC3=2
 *              REG3->coefR;
 *              REG4->maxSFB;
 *              REG6->msMaskOffset;
 *              RL6->cbRight and sfRight;
 *              RL7->msMaskPtr and sfbTab
 *
 * Outputs:     no
 *
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       07/24/2007
 *
 * Notes:       assume no guard bits in input
 *              gains 0 int bits
 **************************************************************************************/

void Coolsand_StereoProcessGroup(void)
{
    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_push16(RA,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);    //gbMaskL = 0;
    VoC_lw16inc_p(REG5,REG0,DEFAULT);//msMask = (*msMaskPtr++) >> msMaskOffset;
    VoC_lw32z(ACC1,IN_PARALLEL);//gbMaskR = 0;
    VoC_movreg16(RL7_LO,REG0,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);
    VoC_bngtz16_r(StereoProcessGroup_L8,REG4)
    VoC_shr16_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(FORMATX,15,IN_PARALLEL);
    VoC_loop_r(0,REG4);

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//sfbTab[sfb]
    VoC_sub16_pr(REG4,REG1,REG4,DEFAULT);//width = sfbTab[sfb+1] - sfbTab[sfb]; /* assume >= 0 (see sfBandTabLong/sfBandTabShort) */
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);//cbIdx = cbRight[sfb];

    VoC_movreg16(REG0,RL6_HI,DEFAULT);
    VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);
    VoC_movreg16(RL7_HI,REG1,DEFAULT);
    VoC_inc_p(REG0,DEFAULT)
    VoC_movreg16(RL6_HI,REG0,IN_PARALLEL);// for sf = -sfRight[sfb];
    //if (cbIdx == 14 || cbIdx == 15)
    VoC_be16_rd(StereoProcessGroup_L0,REG7,const_14_addr);
    VoC_be16_rd(StereoProcessGroup_L0,REG7,const_15_addr);
    VoC_jump(StereoProcessGroup_L1);
StereoProcessGroup_L0:
    /* intensity stereo */
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);
    VoC_and16_rr(REG0,REG5,DEFAULT);//(msMask & 0x01)
    VoC_lw16i_short(REG1,1,IN_PARALLEL);
    //if (msMaskPres == 1 && (msMask & 0x01))
    VoC_bne16_rd(StereoProcessGroup_IS_L0,REG1,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr)
    VoC_bez16_r(StereoProcessGroup_IS_L0,REG0);
    VoC_xor16_rr(REG7,REG1,DEFAULT);//cbIdx ^= 0x01;                /* invert_intensity(): 14 becomes 15, or 15 becomes 14 */
StereoProcessGroup_IS_L0:

    VoC_and16_rr(REG7,REG1,DEFAULT);//cbIdx &= 0x01;                    /* choose - or + scale factor */
    VoC_movreg16(REG0,RL6_HI,IN_PARALLEL);//sfRight

    VoC_shr16_ri(REG7,-3,DEFAULT);
    VoC_lw16i(REG1,Tab_pow14_addr);
    VoC_add16_rr(REG7,REG7,REG1,DEFAULT);
    VoC_lw16i_short(REG1,3,IN_PARALLEL);
    VoC_sub16_dp(REG0,const_0_addr,REG0);//sf = -sfRight[sfb];              /* negative since we use identity 0.5^(x) = 2^(-x) (see spec) */
    VoC_and16_rr(REG1,REG0,DEFAULT);//sf & 0x03
    VoC_shr16_ri(REG0,2,IN_PARALLEL);//(sf >> 2)
    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_add16_rr(REG1,REG7,REG1,DEFAULT);
    VoC_add16_rd(REG7,REG0,const_2_addr);  //   scalei = (sf >> 2) + 2;         /* +2 to compensate for scalef = Q30 */
    VoC_lw16i_short(REG0,-31,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_bgt16_rr(StereoProcessGroup_IS_L2,REG0,REG7);//if(scalei<-31)
    VoC_movreg16(REG7,REG0,DEFAULT);
StereoProcessGroup_IS_L2:
    VoC_lw32_p(REG01,REG1,DEFAULT);//scalef = pow14[cbIdx][sf & 0x03];
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG0,1,DEFAULT);
    VoC_push16(REG6,IN_PARALLEL);
StereoProcessGroup_IS_L3:
    VoC_lw32inc_p(REG45,REG2,DEFAULT);//cl = *coefL;
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15/////////
    VoC_multf32_rr(ACC0,REG5,REG1,DEFAULT);
    VoC_macX_rr(REG0,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG1,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15///////////
    VoC_sub16_rd(REG6,REG6,const_1_addr);
    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_shr32_ri(ACC0,31,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,IN_PARALLEL);//*coefR++ = cr;
    VoC_xor32_rr(REG45,ACC0,DEFAULT);
    VoC_sub32_rr(REG45,REG45,ACC0,DEFAULT);
    VoC_or32_rr(ACC1,REG45,DEFAULT);//gbMaskR |= FASTABS(cl);
    VoC_bnez16_r(StereoProcessGroup_IS_L3,REG6);
    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG6,DEFAULT)
    VoC_pop32(ACC0,IN_PARALLEL);
    VoC_jump(StereoProcessGroup_LX);
StereoProcessGroup_L1:
    //  else if ( cbIdx != 13 && ((msMaskPres == 1 && (msMask & 0x01)) || msMaskPres == 2) )
    VoC_lw16i_short(REG0,13,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);
    VoC_be16_rr(StereoProcessGroup_L2,REG7,REG0);
    VoC_lw16_d(REG7,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr);
    VoC_be16_rd(StereoProcessGroup_L3,REG7,const_2_addr);//if msMaskPres == 2
    VoC_bne16_rr(StereoProcessGroup_L2,REG7,REG1);//if msMaskPres == 1
    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_and16_ri(REG7,0x01);
    VoC_bez16_r(StereoProcessGroup_L2,REG7);//if (msMask & 0x01)
StereoProcessGroup_L3:
    VoC_push16(REG6,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);
StereoProcessGroup_L4:

    VoC_lw32_p(REG01,REG2,DEFAULT);//cl = *coefL;

    VoC_add32_rp(REG67,REG01,REG3,DEFAULT);//sf = cl + cr;
    VoC_sub32_rp(REG01,REG01,REG3,DEFAULT);//cl -= cr;
    VoC_movreg32(RL7,REG67,IN_PARALLEL);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);//*coefL++ = sf;
    VoC_shr32_ri(RL7,31,IN_PARALLEL);
    VoC_xor32_rr(REG67,RL7,DEFAULT);
    VoC_sw32inc_p(REG01,REG3,IN_PARALLEL);//*coefR++ = cl;
    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_or32_rr(ACC0,REG67,DEFAULT);//gbMaskL |= FASTABS(sf);
    VoC_shr32_ri(RL7,31,IN_PARALLEL);
    VoC_xor32_rr(REG01,RL7,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sub32_rr(REG01,REG01,RL7,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL)
    VoC_or32_rr(ACC1,REG01,DEFAULT);//gbMaskR |= FASTABS(cl);

    VoC_bnez16_r(StereoProcessGroup_L4,REG4);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

    VoC_jump(StereoProcessGroup_LX);
StereoProcessGroup_L2:
    /* nothing to do */
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);// coefL += width;
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);//coefR += width;
StereoProcessGroup_LX:
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);
    VoC_movreg16(REG1,RL7_LO,DEFAULT);
    VoC_lw16i_short(REG0,16,IN_PARALLEL);
    VoC_bne16_rr(StereoProcessGroup_L5,REG6,REG0)
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//msMask = *msMaskPtr++;
    VoC_lw16i_short(REG6,0,DEFAULT);//msMaskOffset = 0;
    VoC_movreg16(RL7_LO,REG1,IN_PARALLEL);
StereoProcessGroup_L5:

    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,DEFAULT);
    VoC_endloop0

StereoProcessGroup_L8:
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL)
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_bnlt16_rd(StereoProcessGroup_L6,REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
    VoC_sw16_d(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
StereoProcessGroup_L6:

    VoC_movreg32(RL6,ACC1,DEFAULT);
    VoC_jal(Coolsand_CLZ);
    VoC_sub16_rr(REG6,REG6,REG1,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_bnlt16_rd(StereoProcessGroup_L7,REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
    VoC_sw16_d(REG6,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
StereoProcessGroup_L7:

    VoC_return;



}
/**************************************************************************************
 * Function:    StereoProcess
 *
 * Description: apply mid-side and intensity stereo, if enabled
 *
 * Inputs:      no
 *
 * Outputs:     REG->0 if successful, -1 if error
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,RL6,
 *
 * Version 1.0  Created by  Xuml       07/24/2007
 *
 **************************************************************************************/
void Coolsand_StereoProcess(void)
{
    /* mid-side and intensity stereo require common_window == 1 (see MPEG4 spec, Correction 2, 2004) */
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL); //for return.
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_currBlockID_addr);
    VoC_bne16_rr(StereoProcess_End,REG1,REG4);
    VoC_bne16_rr(StereoProcess_End,REG1,REG5);

    /* nothing to do */
    VoC_bnez16_d(StereoProcess_Begin,STRUCT_AACDecInfo_PSInfoBase_msMaskPresent_addr);
    VoC_bnez16_d(StereoProcess_Begin,STRUCT_AACDecInfo_PSInfoBase_intensityUsed_addr+1);
    VoC_jump(StereoProcess_End);
StereoProcess_Begin:
    VoC_lw16i_short(REG2,2,DEFAULT);

    VoC_lw16i(REG4,Tab_sfBandTabLongOffset_addr);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabLong_addr,1);
    VoC_lw16i(REG7,NSAMPS_LONG*2);//nSamps = NSAMPS_LONG;

    VoC_bne16_rd(StereoProcess_L0,REG2,STRUCT_AACDecInfo_PSInfoBase_icsInfo_winSequence_addr); //if (icsInfo->winSequence == 2);
    VoC_lw16i(REG4,Tab_sfBandTabShortOffset_addr);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabShort_addr,1);
    VoC_lw16i(REG7,NSAMPS_SHORT*2);//nSamps = NSAMPS_SHORT;
StereoProcess_L0:
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);//&sfBandTabLongOffset[psi->sampRateIdx]
    VoC_lw16_d(REG5,STRUCT_AACDecInfo_PSInfoBase_icsInfo_numWinGroup_addr);
    VoC_add16_rp(REG1,REG1,REG4,DEFAULT); //sfbTab


    /* do fused mid-side/intensity processing for each block (one long or eight short) */
    VoC_lw16i_short(REG6,0,IN_PARALLEL);//  msMaskOffset = 0;
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_msMaskBits_addr,1)//    msMaskPtr = psi->msMaskBits;

    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_coef_addr,2);//coefL = psi->coef[0];
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_coef1_addr,2);//coefR = psi->coef[1];

    VoC_movreg32(RL7,REG01,DEFAULT);//save msMaskPtr and sfbTab;
    VoC_push16(RA,IN_PARALLEL);
    VoC_lw16_d(REG4,STRUCT_AACDecInfo_PSInfoBase_icsInfo_maxSFB_addr);

    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_sfbCodeBook1_addr,1);//&psi->sfbCodeBook[1]
    VoC_lw16i_set_inc(REG1,STRUCT_AACDecInfo_PSInfoBase_scaleFactors1_addr,1);//&psi->scaleFactors[1]

    VoC_movreg32(RL6,REG01,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);
    VoC_lw16i(REG1,STRUCT_AACDecInfo_PSInfoBase_icsInfo_winGroupLen_addr);




    VoC_loop_r(1,REG5)//for (gp = 0; gp < icsInfo->numWinGroup; gp++)

    VoC_lw16inc_p(REG5,REG1,DEFAULT);//icsInfo->winGroupLen[gp]


    //for (win = 0; win < icsInfo->winGroupLen[gp]; win++)
StereoProcess_L3:

    VoC_bngt16_rr(StereoProcess_L2,REG5,REG0);
    VoC_inc_p(REG0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);//psi->sfbCodeBook[1] + gp*icsInfo->maxSFB and psi->scaleFactors[1] + gp*icsInfo->maxSFB
    VoC_push16(REG4,DEFAULT); //icsInfo->maxSFB;
    VoC_push32(RL7,IN_PARALLEL);//msMaskPtr and sfbTab;
    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);
    VoC_push16(REG5,DEFAULT);
    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);


    //RL6:psi->sfbCodeBook[1] + gp*icsInfo->maxSFB and psi->scaleFactors[1] + gp*icsInfo->maxSFB
    //RL7:msMaskPtr and sfbTab;
    //REG4:icsInfo->maxSFB
    //REG6:msMaskOffset

    VoC_jal(Coolsand_StereoProcessGroup);
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_pop16(REG5,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);//msMaskPtr and sfbTab;
    VoC_pop16(REG4,DEFAULT); //icsInfo->maxSFB;
    VoC_pop32(RL6,IN_PARALLEL);//psi->sfbCodeBook[1] + gp*icsInfo->maxSFB and psi->scaleFactors[1] + gp*icsInfo->maxSFB and
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_jump(StereoProcess_L3);
StereoProcess_L2:
    VoC_push16(REG1,DEFAULT);
    VoC_movreg32(REG01,RL6,IN_PARALLEL);


    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//psi->scaleFactors[1] + gp*icsInfo->maxSFB
    VoC_add16_rr(REG5,REG6,REG4,IN_PARALLEL);//(msMaskOffset + icsInfo->maxSFB)

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//psi->sfbCodeBook[1] + gp*icsInfo->maxSFB
    VoC_shr16_ri(REG5,4,IN_PARALLEL);

    VoC_movreg16(REG1,RL7_LO,DEFAULT);
    VoC_movreg32(RL6,REG01,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);//msMaskPtr += (msMaskOffset + icsInfo->maxSFB) >> 3;
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);//(msMaskOffset + icsInfo->maxSFB)

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_movreg16(RL7_LO,REG1,IN_PARALLEL);//save msMaskPtr;

    VoC_pop16(REG1,DEFAULT);
    VoC_and16_ri(REG6,0x0f);//msMaskOffset = (msMaskOffset + icsInfo->maxSFB) & 0x07;


    VoC_endloop1
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
StereoProcess_End:
    VoC_return;
}
/*********************************stproc.c end************************************************/



/*********************************tns.c begin************************************************/
#define FBITS_LPC_COEFS 20

/**************************************************************************************
 * Function:    Coolsand_DecodeLPCCoefs
 *
 * Description: decode LPC coefficients for TNS
 *
 * Inputs:      REG0->res;INC1=1;
 *              REG7->order;
 *              RL7_HI->filtCoef
 *
 * Outputs:     none
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC0,
 *
 * Version 1.0  Created by  Xuml       07/25/2007
 *
 * Notes:       assumes no guard bits in input transform coefficients
 *              a[i] = Q(FBITS_LPC_COEFS), don't store a0 = 1.0
 *                (so a[0] = first delay tap, etc.)
 *              max abs(a[i]) < log2(order), so for max order = 20 a[i] < 4.4
 *                (up to 3 bits of gain) so a[i] has at least 31 - FBITS_LPC_COEFS - 3
 *                guard bits
 *              to ensure no intermediate overflow in all-pole filter, set
 *                FBITS_LPC_COEFS such that number of guard bits >= log2(max order)
 **************************************************************************************/
void Coolsand_DecodeLPCCoefs(void)
{
    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_tnsLPCBuf_addr,2);//a
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_tnsWorkBuf_addr,2);//b
    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_movreg16(REG6,REG0,IN_PARALLEL);
    VoC_lw16i_short(REG4,3,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);

    VoC_bne16_rr(DecodeLPCCoefs_L0,REG6,REG4);
    VoC_lw16i_set_inc(REG0,Tab_invQuant3_addr,-2);//if (res == 3)           invQuantTab = invQuant3;
    VoC_jump(DecodeLPCCoefs_L1);
DecodeLPCCoefs_L0:
    VoC_bne16_rr(DecodeLPCCoefs_End,REG6,REG5);
    VoC_lw16i_set_inc(REG0,Tab_invQuant4_addr,-2);  //invQuantTab = invQuant4;
DecodeLPCCoefs_L1:
    VoC_lw16i_short(REG4,0,DEFAULT);
//  VoC_lw16i_short(REG5,1,IN_PARALLEL);

DecodeLPCCoefs_L2:
    VoC_lw16_d(REG5,LOCAL_TNSFilter_order_addr);
    VoC_bngt16_rr(DecodeLPCCoefs_End,REG5,REG4);//for (m = 0; m < order; m++)
    VoC_lw16inc_p(REG5,REG1,DEFAULT);//filtCoef[m]
    VoC_and16_ri(REG5,0x000f);
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_movreg32(RL7,REG01,IN_PARALLEL);
    VoC_add16_rr(REG5,REG5,REG0,DEFAULT);//invQuantTab[filtCoef[m] & 0x0f]
    VoC_movreg32(RL6,REG23,IN_PARALLEL);
    VoC_sub16_rd(REG0,REG4,const_1_addr);//m-1
    VoC_shr16_ri(REG0,-1,DEFAULT);//for &a[m-1]
    VoC_lw32_p(ACC1,REG5,DEFAULT);//t = invQuantTab[filtCoef[m] & 0x0f];    /* t = Q31 */
    VoC_movreg16(REG1,REG4,IN_PARALLEL)//m
    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);//&a[m-1]
    VoC_movreg32(REG67,ACC1,IN_PARALLEL);//t

    VoC_bngtz16_r(DecodeLPCCoefs_L3,REG1);//for (i = 0; i < m; i++)

    VoC_loop_r_short(REG1,DEFAULT);//for (i = 0; i < m; i++)
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);  //a[m-i-1]
    VoC_startloop0
    /////////Coolsand_MULSHIFT32///FORMAT32=0,FORMATX=15/////////
    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);
    VoC_macX_rr(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);
    VoC_macX_rr(REG7,REG4,DEFAULT);
    /////////Coolsand_MULSHIFT32//FORMAT32=0,FORMATX=15///////////
    VoC_lw32inc_p(REG45,REG0,DEFAULT);  //a[m-i-1]
    VoC_shr32_ri(ACC0,-1,DEFAULT);//(MULSHIFT32(t, a[m-i-1]) << 1)
    VoC_sub32inc_pr(ACC0,REG2,ACC0,DEFAULT);//a[i] - (MULSHIFT32(t, a[m-i-1]) << 1);
    VoC_movreg32(REG67,ACC1,DEFAULT);//t
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_movreg32(REG23,RL6,DEFAULT);
    VoC_movreg16(REG4,REG1,DEFAULT);

    VoC_loop_r_short(REG4,DEFAULT);//for (i = 0; i < m; i++)
    VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);//a[i] = b[i];
    exit_on_warnings = ON;
    VoC_endloop0
DecodeLPCCoefs_L3:
    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);
    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_add16_rr(REG5,REG5,REG2,DEFAULT);//&a[m]
    VoC_shr32_ri(ACC1,31 - FBITS_LPC_COEFS,IN_PARALLEL);//t >> (31 - FBITS_LPC_COEFS)
    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//m++
    VoC_sw32_p(ACC1,REG5,DEFAULT);//a[m] = t >> (31 - FBITS_LPC_COEFS);
    VoC_jump(DecodeLPCCoefs_L2);
DecodeLPCCoefs_End:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_FilterRegion
 *
 * Description: apply LPC filter to one region of coefficients
 *
 * Inputs:      REG2->size
 *              REG3->dir
 *              REG4->audioCoef
 *
 * Outputs:     none         (filtered transform coefficients)
 *
 * Used : REG0~REG7,ACC1,RL6_HI,RL7
 *
 * Version 1.0  Created by  Xuml       07/26/2007
 *
 * Notes:       assumes no guard bits in input transform coefficients
 *              gains 0 int bits
 *              history buffer does not need to be preserved between regions
 **************************************************************************************/
void Coolsand_FilterRegion(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_movreg16(REG7,REG2,IN_PARALLEL);
    VoC_lw16i_short(INC0,-2,DEFAULT);//inc = (dir ? -1 : 1);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bnez16_r(FilterRegion_L0,REG3);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
FilterRegion_L0:

    VoC_lw16i_set_inc(REG2,STRUCT_AACDecInfo_PSInfoBase_tnsLPCBuf_addr,-2);//a
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_tnsWorkBuf_addr,2);//hist;

    VoC_lw16_d(REG1,LOCAL_TNSFilter_order_addr);
    /* init history to 0 every time */
    VoC_loop_r_short(REG1,DEFAULT);//for (i = 0; i < order; i++)
    VoC_movreg16(REG5,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);//hist[i] = 0;
    VoC_endloop0

    VoC_movreg16(REG3,REG5,DEFAULT);
    VoC_movreg16(REG0,REG4,IN_PARALLEL);
    VoC_lw16i_short(INC3,-2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

FilterRegion_L3:

    VoC_push16(REG7,DEFAULT);
    /* sum64 = a0*y[n] = 1.0*y[n] */

    VoC_lw32_p(ACC0,REG0,DEFAULT)//y = *audioCoef;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_shru32_ri(ACC0,-FBITS_LPC_COEFS,IN_PARALLEL);//sum64.r.lo32 = y << FBITS_LPC_COEFS;
    VoC_shr32_ri(ACC1,32 - FBITS_LPC_COEFS,DEFAULT);//  sum64.r.hi32 = y >> (32 - FBITS_LPC_COEFS);
    //  VoC_shru32_ri(ACC0,1,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG1,REG4,DEFAULT);//order - 1
    VoC_push32(REG23,IN_PARALLEL);
    VoC_movreg16(REG1,REG4,DEFAULT)
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);//&hist[order - 1]
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);//&a[order - 1]

    VoC_push16(REG0,DEFAULT);
    VoC_bngtz16_r(FilterRegion_L1,REG1)

    VoC_loop_r_short(REG1,DEFAULT);
    VoC_startloop0
    VoC_push16(REG3,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16i(STATUS,CARRY_ON);
    VoC_lw32inc_p(REG01,REG2,DEFAULT);
    VoC_jal(Coolsand_MADD64);
    VoC_pop16(REG4,DEFAULT);//hist[j] = hist[j-1];
    VoC_lw32_p(REG01,REG3,DEFAULT);
    VoC_lw16i_short(STATUS,CARRY_OFF,DEFAULT);
    VoC_sw32_p(REG01,REG4,DEFAULT);
    VoC_endloop0
FilterRegion_L1:

    VoC_lw32_p(REG67,REG3,DEFAULT);//&hist[0]
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw32_p(REG01,REG2,DEFAULT); //a[0]
    VoC_lw16i(STATUS,CARRY_ON);
    VoC_jal(Coolsand_MADD64);//sum64.w64 = MADD64(sum64.w64, hist[0], a[0]);
    VoC_lw16i_short(STATUS,CARRY_OFF,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);
    VoC_shr32_ri(ACC1,-(32 - FBITS_LPC_COEFS),IN_PARALLEL);
    VoC_be32_rd(FilterRegion_L2,ACC1,const_0x80000000_addr);///* clip output (rare) */
    VoC_be32_rd(FilterRegion_L2,ACC1,const_0x7fffffff_addr);
    VoC_shru32_ri(ACC0,FBITS_LPC_COEFS,DEFAULT);
    VoC_or32_rr(ACC1,ACC0,DEFAULT);
FilterRegion_L2:
    VoC_pop16(REG1,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);//for gbMask |= FASTABS(y);
    VoC_sw32_p(ACC1,REG3,DEFAULT);//hist[0] = y;
    VoC_shr32_ri(ACC0,31,IN_PARALLEL);
    VoC_xor32_rr(ACC1,ACC0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,IN_PARALLEL);//*audioCoef = y;audioCoef += inc;

    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_pop16(REG7,IN_PARALLEL);//size
    VoC_or32_rd(ACC1,LOCAL_TNSFilter_gbMask_addr);
    VoC_pop32(REG23,DEFAULT);//
    VoC_sub16_rd(REG7,REG7,const_1_addr);
    VoC_sw32_d(ACC1,LOCAL_TNSFilter_gbMask_addr);
    VoC_bgtz16_r(FilterRegion_L3,REG7);//   } while (--size);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_TNSFilter
 *
 * Description: apply temporal noise shaping, if enabled
 *
 * Inputs:      REG4-> ch
 *
 * Outputs:     REG0->0 if successful, -1 if error
 *
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,REG6,REG7,ACC1,RL6_HI,RL7
 *
 * Version 1.0  Created by  Xuml       07/25/2007
 **************************************************************************************/
void Coolsand_TNSFilter(void)
{

#if 0
    voc_short LOCAL_TNSFilter_nWindows_addr,x
    voc_short LOCAL_TNSFilter_winLen_addr,x
    voc_short LOCAL_TNSFilter_nSFB_addr,x
    voc_short LOCAL_TNSFilter_maxOrder_addr,x
    voc_short LOCAL_TNSFilter_tnsMaxBand_addr,x
    voc_short LOCAL_TNSFilter_order_addr,x
    voc_word  LOCAL_TNSFilter_gbMask_addr,x
#endif


    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);


    //icsInfo = (ch == 1 && psi->commonWin == 1) ? &(psi->icsInfo[0]) : &(psi->icsInfo[ch]);
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_icsInfo_addr,1);
    VoC_lw16_d(REG1,STRUCT_AACDecInfo_PSInfoBase_commonWin_addr);

    VoC_bne16_rr(TNSFilter_L0,REG4,REG2);  //if ch == 1;
    VoC_be16_rr(TNSFilter_L0,REG1,REG2);  //if psi->commonWin == 1;
    VoC_lw16i_set_inc(REG3,STRUCT_AACDecInfo_PSInfoBase_icsInfo1_addr,1);
TNSFilter_L0:

    VoC_lw16i(REG7,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_tnsInfo_addr,1);//ti = &psi->tnsInfo[ch];
    VoC_lw16i(RL6_HI,STRUCT_AACDecInfo_PSInfoBase_coef_addr);////audioCoef =  psi->coef[ch];

    VoC_bne16_rr(TNSFilter_LX,REG4,REG2);  //if ch == 1;
    VoC_lw16i(REG7,STRUCT_AACDecInfo_PSInfoBase_gbCurrent_addr+1);
    VoC_lw16i_set_inc(REG0,STRUCT_AACDecInfo_PSInfoBase_tnsInfo1_addr,1);//ti = &psi->tnsInfo[ch];
    VoC_lw16i(RL6_HI,STRUCT_AACDecInfo_PSInfoBase_coef1_addr);//audioCoef =  psi->coef[ch];
TNSFilter_LX:

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//ti->numFilt
    VoC_inc_p(REG3,IN_PARALLEL);//for icsInfo->winSequence
    VoC_bez16_r(TNSFilter_End,REG5);//if (!ti->tnsDataPresent)
    VoC_push16(RA,DEFAULT);
    VoC_lw16i(REG4,Tab_sfBandTotal_addr);
    VoC_lw16i(REG5,Tab_tnsMaxBands_addr);
    VoC_add16_rd(REG4,REG4,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);
    VoC_add16_rd(REG5,REG5,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);// &tnsMaxBandTab[psi->sampRateIdx];


    VoC_lw16_p(REG6,REG4,DEFAULT);
    VoC_lw16_p(REG7,REG5,DEFAULT);//tnsMaxBand = tnsMaxBandTab[psi->sampRateIdx];

    VoC_push16(REG7,DEFAULT);//save &psi->gbCurrent[ch];
    VoC_lw16inc_p(REG4,REG3,DEFAULT);//load icsInfo->winSequence;

    VoC_bne16_rd(TNSFilter_L1,REG4,const_2_addr);//if (icsInfo->winSequence == 2)
    VoC_lw16i_short(REG5,7,DEFAULT);//tnsMaxOrderShort[1]
    VoC_and16_ri(REG6,0x00ff);
    VoC_sw16_d(REG5,LOCAL_TNSFilter_maxOrder_addr);
    VoC_sw16_d(REG6,LOCAL_TNSFilter_nSFB_addr);

    VoC_lw16i_short(REG4,NWINDOWS_SHORT,DEFAULT);
    VoC_and16_ri(REG7,0x00ff);
    VoC_lw16i(REG5,NSAMPS_SHORT*2);
    VoC_sw16_d(REG4,LOCAL_TNSFilter_nWindows_addr);//nWindows = NWINDOWS_LONG;
    VoC_sw16_d(REG5,LOCAL_TNSFilter_winLen_addr);//winLen = NSAMPS_LONG;

    VoC_lw16i(REG4,Tab_sfBandTabShort_addr);
    VoC_lw16i(REG6,Tab_sfBandTabShortOffset_addr);

    VoC_jump(TNSFilter_L2);
TNSFilter_L1://else

    VoC_lw16i_short(REG5,12,DEFAULT);//tnsMaxOrderShort[1]
    VoC_shru16_ri(REG6,8,IN_PARALLEL);

    VoC_lw16i(REG6,Tab_sfBandTabLongOffset_addr);

    VoC_sw16_d(REG6,LOCAL_TNSFilter_nSFB_addr);
    VoC_sw16_d(REG5,LOCAL_TNSFilter_maxOrder_addr);

    VoC_lw16i_short(REG4,NWINDOWS_LONG,DEFAULT);
    VoC_shru16_ri(REG7,8,IN_PARALLEL);
    VoC_lw16i(REG5,NSAMPS_LONG*2);
    VoC_sw16_d(REG4,LOCAL_TNSFilter_nWindows_addr);//nWindows = NWINDOWS_LONG;
    VoC_sw16_d(REG5,LOCAL_TNSFilter_winLen_addr);//winLen = NSAMPS_LONG;

    VoC_lw16i(REG4,Tab_sfBandTabLong_addr);


TNSFilter_L2:
    VoC_movreg16(REG5,REG7,DEFAULT);

    VoC_add16_rd(REG6,REG6,STRUCT_AACDecInfo_PSInfoBase_sampRateIdx_addr);

    VoC_lw32z(ACC1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);//&icsInfo->maxSFB


    VoC_add16_rp(REG6,REG4,REG6,DEFAULT);//sfbTab
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_lw16_p(REG3,REG3,DEFAULT);//icsInfo->maxSFB
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_sw16_d(REG5,LOCAL_TNSFilter_tnsMaxBand_addr);
    VoC_bngt16_rr(TNSFilter_L3,REG5,REG3)//if (tnsMaxBand > icsInfo->maxSFB)
    VoC_sw16_d(REG3,LOCAL_TNSFilter_tnsMaxBand_addr);   //tnsMaxBand = icsInfo->maxSFB;
TNSFilter_L3:
    VoC_sw32_d(ACC1,LOCAL_TNSFilter_gbMask_addr);//gbMask = 0;


    VoC_add16_rr(REG5,REG0,REG7,DEFAULT);//filtRes =    ti->coefRes;
    VoC_add16_rr(REG2,REG5,REG7,DEFAULT);//filtLength = ti->length;
    VoC_movreg16(RL7_LO,REG5,IN_PARALLEL);//filtRes
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);//filtOrder =  ti->order;
    VoC_add16_rr(REG1,REG3,REG7,DEFAULT);//filtDir =    ti->dir;
    VoC_add16_rr(REG7,REG1,REG7,DEFAULT);//filtCoef =   ti->coef;
    VoC_movreg16(RL7_HI,REG7,DEFAULT);   //filtCoef
    VoC_movreg16(REG4,ACC1_HI,IN_PARALLEL);

TNSFilter_L4:
    VoC_lw16_d(REG5,LOCAL_TNSFilter_nWindows_addr);
    VoC_bngt16_rr(TNSFilter_L14,REG5,REG4);//for (win = 0; win < nWindows; win++)

    VoC_movreg16(ACC1_HI,REG4,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);//numFilt = ti->numFilt[win];
    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,0,IN_PARALLEL);
    VoC_lw16_d(REG5,LOCAL_TNSFilter_nSFB_addr);//bottom = nSFB;
TNSFilter_L6:
    VoC_movreg16(REG7,ACC1_LO,DEFAULT);

    VoC_bngt16_rr(TNSFilter_L5,REG4,REG7);//for (filt = 0; filt < numFilt; filt++)
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);
    VoC_movreg16(REG4,REG5,DEFAULT);//top = bottom;
    VoC_add16_rr(REG7,REG7,REG4,IN_PARALLEL);//filt++
    VoC_sub16inc_rp(REG5,REG4,REG2,DEFAULT);//bottom = top - *filtLength++;
    VoC_movreg16(ACC1_LO,REG7,IN_PARALLEL);
    VoC_bnltz16_r(TNSFilter_L7,REG5);//bottom = MAX(bottom, 0);
    VoC_lw16i_short(REG5,0,DEFAULT);
TNSFilter_L7:
    VoC_lw16inc_p(REG7,REG3,DEFAULT);
    VoC_bngt16_rd(TNSFilter_L8,REG7,LOCAL_TNSFilter_maxOrder_addr);//order = MIN(order, maxOrder);
    VoC_lw16_d(REG7,LOCAL_TNSFilter_maxOrder_addr);
TNSFilter_L8:
    VoC_push32(REG23,DEFAULT);
    VoC_movreg16(REG2,REG5,IN_PARALLEL);
    VoC_sw16_d(REG7,LOCAL_TNSFilter_order_addr);
    VoC_bez16_r(TNSFilter_L9,REG7);//if (order)

    VoC_bngt16_rd(TNSFilter_L10,REG2,LOCAL_TNSFilter_tnsMaxBand_addr);//[MIN(bottom, tnsMaxBand)];
    VoC_lw16_d(REG2,LOCAL_TNSFilter_tnsMaxBand_addr);
TNSFilter_L10:
    VoC_add16_rr(REG2,REG6,REG2,DEFAULT);
    VoC_bngt16_rd(TNSFilter_L11,REG4,LOCAL_TNSFilter_tnsMaxBand_addr);//[MIN(bottom, tnsMaxBand)];
    VoC_lw16_d(REG4,LOCAL_TNSFilter_tnsMaxBand_addr);
TNSFilter_L11:
    VoC_add16_rr(REG4,REG6,REG4,DEFAULT);
    VoC_lw16_p(REG7,REG2,DEFAULT);//start = sfbTab[MIN(bottom, tnsMaxBand)];
    VoC_lw16_p(REG4,REG4,DEFAULT);//end   = sfbTab[MIN(top, tnsMaxBand)];
    VoC_sub16_rr(REG2,REG4,REG7,DEFAULT);//size = end - start;
    VoC_bngtz16_r(TNSFilter_L12,REG2);//if (size > 0)
    VoC_lw16inc_p(REG3,REG1,DEFAULT);//dir = *filtDir++;
    VoC_bez16_r(TNSFilter_L13,REG3);//if (dir)
    VoC_sub16_rd(REG7,REG4,const_1_addr);//start = end - 1;
TNSFilter_L13:
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_movreg16(REG0,RL6_HI,IN_PARALLEL);
    VoC_add16_rr(REG4,REG7,REG0,DEFAULT);//audioCoef + start
    VoC_movreg16(REG0,RL7_LO,DEFAULT);//filtRes
    VoC_movreg16(REG1,ACC1_HI,IN_PARALLEL);//win;
    VoC_add16_rr(REG0,REG0,REG1,DEFAULT);

    VoC_push16(REG5,IN_PARALLEL);//bottom
    VoC_push16(RL6_HI,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);
    VoC_push16(REG6,DEFAULT);////sfbTab
    VoC_push32(ACC1,IN_PARALLEL)
    VoC_push16(REG4,DEFAULT);//audioCoef + start
    VoC_push32(REG23,IN_PARALLEL);//size and dir

    VoC_lw16_p(REG0,REG0,DEFAULT);

    //REG0:filtRes[win];RL7_HI:filtCoef;LOCAL_TNSFilter_order_addr:order

    VoC_jal(Coolsand_DecodeLPCCoefs);

    VoC_pop16(REG4,DEFAULT);//audioCoef + start
    VoC_pop32(REG23,IN_PARALLEL)////size and dir


    //REG2:size; REG3:dir;REG4:audioCoef + start; LOCAL_TNSFilter_order_addr:order
    VoC_jal(Coolsand_FilterRegion);


    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG6,IN_PARALLEL);//sfbTab
    VoC_pop32(RL7,DEFAULT);
    VoC_pop16(RL6_HI,IN_PARALLEL);
    VoC_pop16(REG5,DEFAULT);//bottom
    VoC_pop32(REG01,IN_PARALLEL);
TNSFilter_L12:
    VoC_movreg16(REG2,RL7_HI,DEFAULT);//filtCoef
    VoC_add16_rd(REG2,REG2,LOCAL_TNSFilter_order_addr);//filtCoef += order;
    VoC_movreg16(RL7_HI,REG2,DEFAULT);//filtCoef
TNSFilter_L9:

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_pop16(REG4,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_jump(TNSFilter_L6);
TNSFilter_L5:


    VoC_movreg16(REG7,RL6_HI,DEFAULT);
    VoC_add16_rd(REG7,REG7,LOCAL_TNSFilter_winLen_addr);
    VoC_movreg16(RL6_HI,REG7,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);
    VoC_movreg16(REG4,ACC1_HI,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//win++;
    VoC_jump(TNSFilter_L4);

TNSFilter_L14:
    VoC_pop16(REG5,DEFAULT);//load &psi->gbCurrent[ch];
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_lw32_d(RL6,LOCAL_TNSFilter_gbMask_addr);
    //RL6:gbMask; REG6:return.
    VoC_jal(Coolsand_CLZ);
    VoC_pop16(RA,DEFAULT);
    VoC_lw16_p(REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG1,IN_PARALLEL);
    VoC_bngt16_rr(TNSFilter_End,REG4,REG6);
    VoC_sw16_p(REG6,REG5,DEFAULT);
TNSFilter_End:
    VoC_return;
}



/*********************************tns.c end************************************************/


/**************************************************************************************
 * Function:    Coolsand_AACFindSyncWord
 *
 * Description: locate the next byte-alinged sync word in the raw AAC stream
 *
 * Inputs:      REG1->nBytes;
 *              REG2->inptr(or buf);
 *
 * Outputs:     REG3->offset;    (if pure VoC project is built,this parameter can be removed)
 *              REG2->inptr += offset;
 *              RL6->bitsteam;
 *
 * Used : REG0,REG1,REG2,REG3,REG5,REG6,RL6,
 *
 * Version 1.0  Created by  Xuml       07/19/2007
 * Version 1.0.1  Optimized by  Cui    08/21/2007
 **************************************************************************************/

void Coolsand_AACFindSyncWord(void)
{
    /* find byte-aligned syncword (12 bits = 0xFFF) */

    VoC_lw16_d(REG1,GLOBAL_bytesLeft_addr);
    VoC_sub16_rd(REG0,REG1,const_1_addr); //nBytes - 1;
    VoC_lw16i_set_inc(REG3,0,1);

    // for (i = 0; i < nBytes - 1; i++)
AACFindSyncWord_loop:
    VoC_bngt16_rr(AACFindSyncWord_loop_End,REG0,REG3)
    VoC_lbinc_p(REG2);
    VoC_rb_i(REG5,12,DEFAULT);
    VoC_be16_rd(AACFindSyncWord_End,REG5,const_0x0fff_addr);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);  //i++;
    VoC_jump(AACFindSyncWord_loop);
AACFindSyncWord_loop_End:
    VoC_lw16i_short(REG3,-1,DEFAULT); //return -1;
AACFindSyncWord_End:
    VoC_sw32_d(RL6,GLOBAL_bitbuffer_addr);
    VoC_sw16_d(BITCACHE,GLOBAL_BITCACHE_addr);
    VoC_sw16_d(REG2,GLOBAL_inputptr_addr);
    VoC_return;  //return offset;

}



void vpp_AmjpAacGenerateSinwKbdw(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************

    VoC_push16(RA,DEFAULT);

    // SINW (short)
    VoC_lw16i_set_inc(REG0,Tab_sinWindow_addr,2);
    VoC_lw16i_set_inc(REG1,Tab_SinwShortCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,128,-1);
    VoC_lw16i_set_inc(REG3,5,-1);
    VoC_lw16i(REG4,2);
    VoC_lw16i(REG5,64);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // SINW (long)
    VoC_lw16i_set_inc(REG0,Tab_sinWindow_addr+256,2);
    VoC_lw16i_set_inc(REG1,Tab_SinwLongCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,1024,-1);
    VoC_lw16i_set_inc(REG3,4,-1);
    VoC_lw16i(REG4,2);
    VoC_lw16i(REG5,512);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // KBDW (short)
    VoC_lw16i_set_inc(REG0,Tab_kbdWindow_addr,2);
    VoC_lw16i_set_inc(REG1,Tab_KbdwShortCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,128,-1);
    VoC_lw16i_set_inc(REG3,7,-1);
    VoC_lw16i(REG4,2);
    VoC_lw16i(REG5,64);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // KBDW (long)
    VoC_lw16i_set_inc(REG0,Tab_kbdWindow_addr+256,2);
    VoC_lw16i_set_inc(REG1,Tab_KbdwLongCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,1024,-1);
    VoC_lw16i_set_inc(REG3,4,-1);
    VoC_lw16i(REG4,2);
    VoC_lw16i(REG5,512);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



void vpp_AmjpAacGenerateCos4Sin4(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************

    VoC_push16(RA,DEFAULT);

    // COS4
    VoC_lw16i_set_inc(REG0,Tab_cos4sin4tab_addr,2);
    VoC_lw16i_set_inc(REG1,Tab_Cos4Compressed_addr,1);
    VoC_lw16i_set_inc(REG2,128,-1);
    VoC_lw16i_set_inc(REG3,5,-1);
    VoC_lw16i(REG4,4);
    VoC_lw16i(REG5,32);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // SIN4
    VoC_lw16i_set_inc(REG0,Tab_cos4sin4tab_addr+256,2);
    VoC_lw16i_set_inc(REG1,Tab_Sin4Compressed_addr,1);
    VoC_lw16i_set_inc(REG2,1024,-1);
    VoC_lw16i_set_inc(REG3,4,-1);
    VoC_lw16i(REG4,4);
    VoC_lw16i(REG5,256);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}




void vpp_AmjpAacGenerateTwidOdd(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************

    VoC_push16(RA,DEFAULT);

    // TWID ODD
    VoC_lw16i_set_inc(REG0,Tab_twidTabOdd_addr+480,2);
    VoC_lw16i_set_inc(REG1,Tab_twidOddCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,768,-1);
    VoC_lw16i_set_inc(REG3,6,-1);
    VoC_lw16i(REG4,6);
    VoC_lw16i(REG5,128);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // TWID rearrange
    VoC_pop16(RA,DEFAULT);

    // downscale by 4
    VoC_lw16i_set_inc(REG0,Tab_twidTabOdd_addr+480,2);
    VoC_lw16i_set_inc(REG1,Tab_twidTabOdd_addr+96,2);
    VoC_lw16i(REG4,((4*6-7)*2));

    VoC_loop_i(1,32)

    // copy 1 line
    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    // skip next 3 lines
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_NOP();

    VoC_endloop1

    // downscale by 4
    VoC_lw16i_set_inc(REG0,Tab_twidTabOdd_addr+96,2);
    VoC_lw16i_set_inc(REG1,Tab_twidTabOdd_addr+0,2);

    VoC_loop_i(1,8)

    // copy 1 line
    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    // skip next 3 lines
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_NOP();

    VoC_endloop1

    VoC_return;
}



void vpp_AmjpAacGenerateTwidEven(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************

    VoC_push16(RA,DEFAULT);

    // TWID EVEN
    VoC_lw16i_set_inc(REG0,Tab_twidTabEven_addr+240,2);
    VoC_lw16i_set_inc(REG1,Tab_TwidEvenCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,384,-1);
    VoC_lw16i_set_inc(REG3,7,-1);
    VoC_lw16i(REG4,6);
    VoC_lw16i(REG5,64);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // TWID rearrange
    VoC_pop16(RA,DEFAULT);

    // downscale by 4
    VoC_lw16i_set_inc(REG0,Tab_twidTabEven_addr+240,2);
    VoC_lw16i_set_inc(REG1,Tab_twidTabEven_addr+48,2);
    VoC_lw16i(REG4,((4*6-7)*2));

    VoC_loop_i(1,16)

    // copy 1 line
    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    // skip next 3 lines
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_NOP();

    VoC_endloop1

    // downscale by 4
    VoC_lw16i_set_inc(REG0,Tab_twidTabEven_addr+48,2);
    VoC_lw16i_set_inc(REG1,Tab_twidTabEven_addr+0,2);

    VoC_loop_i(1,4)

    // copy 1 line
    VoC_loop_i_short(6,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0

    // skip next 3 lines
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_NOP();

    VoC_endloop1

    VoC_return;
}



void vpp_AmjpAacGenerateCos1(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************

    VoC_push16(RA,DEFAULT);

    // COS1
    VoC_lw16i_set_inc(REG0,Tab_DMA_start_addr+2304,2);
    VoC_lw16i_set_inc(REG1,Tab_Cos1Sin1Compressed_addr,1);
    VoC_lw16i_set_inc(REG2,514,-1);
    VoC_lw16i_set_inc(REG3,4,-1);
    VoC_lw16i(REG4,2);
    VoC_lw16i(REG5,257);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}


void vpp_AmjpAacGenerateBitRev(void)
{

    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);

    VoC_lw16i_set_inc(REG0,Tab_bitrevtabCompressed_addr,1);
    VoC_lw16i_set_inc(REG1,Tab_bitrevtab_addr,2);

    VoC_loop_i(0,74)

    VoC_lw16inc_p(REG2,REG0,DEFAULT);
    VoC_lw16_p(REG3,REG0,IN_PARALLEL);

    VoC_shru16_ri(REG2,-8,DEFAULT);

    VoC_shru16_ri(REG2,8,DEFAULT);
    VoC_shru16_ri(REG3,8,IN_PARALLEL);

    VoC_NOP();

    VoC_sw32inc_p(REG23,REG1,DEFAULT);

    VoC_endloop0

    VoC_pop32(REG23,DEFAULT);
    VoC_pop32(REG01,DEFAULT);

    VoC_return;
}



void vpp_AmjpAacGenerateHuffTabSpecInfo(void)
{

    // ----------------
    // Unsigned VSS decompression
    // ----------------

    VoC_push32(RL6,DEFAULT);
    VoC_push16(BITCACHE,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_push32(REG45,DEFAULT);
    VoC_lw16i_short(REG5,0xf,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,Tab_huffTabSpecInfoCompressed_addr,1);
    VoC_lw16i_set_inc(REG0,Tab_huffTabSpecInfo_addr,1);

    VoC_lbinc_p(REG1);
    VoC_lw16i(REG3,0xff);

    VoC_loop_i(0,242)

    VoC_lbinc_p(REG1);

    // val = read next 4 bits
    VoC_rbinc_i(REG4,4,DEFAULT);

    VoC_bne16_rr(HuffTabSpecInfo_write_sample,REG4,REG5);

    // val = read next 8 bits
    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_bne16_rr(HuffTabSpecInfo_write_sample,REG4,REG3);

    // val = read next 16 bits
    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_NOP();

HuffTabSpecInfo_write_sample:

    VoC_sw16inc_p(REG4,REG0,DEFAULT);

    VoC_endloop0

    VoC_pop32(REG45,DEFAULT);
    VoC_pop32(REG01,DEFAULT);

    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(BITCACHE,IN_PARALLEL);

    VoC_return;
}



void vpp_AmjpAacGenerateSfBandLong(void)
{

    // **************************************************************************************
    //   Function:    vpp_AmjpDecompressVssAdpcmTable
    //
    //   Description: decompress a table compressed using Varible Sample Size and recursive ADPCM
    //
    //   Inputs:      REG0-> destination (decompressed table); inrc0=2
    //                REG1-> source (compressed table); inrc1=1
    //                REG2-> table size, incr2 = -1;
    //                REG3-> iterations, incr3 = -1;
    //                REG4-> step;
    //                REG5-> size/step;
    //   Outputs:
    //
    //   Used :       REG01234567, RL6
    //
    //   Version 1.0  Create by  Yordan  07/10/2008
    //
    //   *************************************************************************************


    VoC_push32(REG01,DEFAULT);
    VoC_push32(REG23,DEFAULT);
    VoC_push32(REG45,DEFAULT);
    VoC_push32(REG67,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    // SFBAND_LONG
    VoC_lw16i_set_inc(REG0,Tab_sfBandTabLong_addr,2);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabLongCompressed_addr,1);
    VoC_lw16i_set_inc(REG2,326,-1);
    VoC_lw16i_set_inc(REG3,2,-1);
    VoC_lw16i(REG4,1);
    VoC_lw16i(REG5,326);

    VoC_jal(vpp_AmjpDecompressVssAdpcmTable);

    // pack
    VoC_lw16i_set_inc(REG0,Tab_sfBandTabLong_addr,2);
    VoC_lw16i_set_inc(REG1,Tab_sfBandTabLong_addr,1);

    VoC_loop_i(0,163);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_endloop0;

    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);
    VoC_pop32(REG23,DEFAULT);
    VoC_pop32(REG01,DEFAULT);

    VoC_return
}



