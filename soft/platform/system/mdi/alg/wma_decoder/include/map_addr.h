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

#ifndef MAP_ADDR_H
#define MAP_ADDR_H

/* This file defines all the variables as memory addresses.
   It's created by voc_map automatically.*/

#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000
#define RAM_I_BEGIN_ADDR               0x10000

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                                    0
#define COMMON_GLOBAL_Y_SIZE                                    0
#define WMADECODE_GLOBAL_X_SIZE                                 9556
#define WMADECODE_GLOBAL_Y_SIZE                                 10030
#define WMADECODE_LOCAL_X_SIZE                                  514
#define WMADECODE_LOCAL_Y_SIZE                                  0
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define WMADECODE_GLOBAL_X_BEGIN_ADDR                           (0 + RAM_X_BEGIN_ADDR)
#define WMADECODE_GLOBAL_Y_BEGIN_ADDR                           (0 + RAM_Y_BEGIN_ADDR)
#define WMADECODE_LOCAL_X_BEGIN_ADDR                            (9556 + RAM_X_BEGIN_ADDR)
#define WMADECODE_LOCAL_Y_BEGIN_ADDR                            (10030 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (9556 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (10030 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

/************************
 **  WMADECODE_GLOBAL  **
 ************************/

//GLOBAL_VARS
//GLOBAL_STRUCT_BUFFER_ADDR
#define GLOBAL_STRUCT_BUFFER_ADDR                           (0 + WMADECODE_GLOBAL_X_BEGIN_ADDR)
//GLOBAL_INPUT_BUFFER_ADDR
#define GLOBAL_INPUT_BUFFER_ADDR                            (0 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_INPUT_DATA_ADDR                              (0 + GLOBAL_INPUT_BUFFER_ADDR) //2048 short
#define GLOBAL_INPUT_OFFSET_ADDR                            (2048 + GLOBAL_INPUT_BUFFER_ADDR) //2 short
#define GLOBAL_INPUT_DATALEN_ADDR                           (2050 + GLOBAL_INPUT_BUFFER_ADDR) //1 short

#define GLOBAL_STRUCT_BUFFER_ALIGN1                         (2051 + GLOBAL_STRUCT_BUFFER_ADDR) //1 short
//GLOBAL_tWMAFileStateInternal_ADDR
#define GLOBAL_tWMAFileStateInternal_ADDR                   (2052 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_cbPayloadLeft_ADDR                           (0 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_cbBlockLeft_ADDR                             (1 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_parse_state_ADDR                             (2 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_bBlockStart_ADDR                             (3 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_cbPayloadOffset_ADDR                         (4 + GLOBAL_tWMAFileStateInternal_ADDR) //2 short
#define GLOBAL_pCallBackBuffer_ADDR                         (6 + GLOBAL_tWMAFileStateInternal_ADDR) //128 short
#define GLOBAL_bHasDRM_ADDR                                 (134 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_bDecInWaitState_ADDR                         (135 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_iPayload_ADDR                                (136 + GLOBAL_tWMAFileStateInternal_ADDR) //2 short
#define GLOBAL_wPayStart_ADDR                               (138 + GLOBAL_tWMAFileStateInternal_ADDR) //2 short
#define GLOBAL_bAllFramesDone_ADDR                          (140 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short
#define GLOBAL_bFirst_ADDR                                  (141 + GLOBAL_tWMAFileStateInternal_ADDR) //1 short

//GLOBAL_tWMAFileHdrStateInternal_ADDR
#define GLOBAL_tWMAFileHdrStateInternal_ADDR                (2194 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_currPacketOffset_ADDR                        (0 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short
#define GLOBAL_nextPacketOffset_ADDR                        (2 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short
#define GLOBAL_cbPacketSize_ADDR                            (4 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short
#define GLOBAL_cbLastPacketOffset_ADDR                      (6 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short
#define GLOBAL_nBlockAlign_ADDR                             (8 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short
#define GLOBAL_wAudioStreamId_ADDR                          (10 + GLOBAL_tWMAFileHdrStateInternal_ADDR) //2 short

//GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR
#define GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR                    (2206 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_bIsCompressedPayload_ADDR                    (0 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_bStreamId_ADDR                               (1 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_cbPacketOffset_ADDR                          (2 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_cbTotalSize_ADDR                             (3 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_bObjectId_ADDR                               (4 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_cbPayloadSize_ADDR                           (5 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_cbObjectOffset_ADDR                          (6 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //2 short
#define GLOBAL_cbObjectSize_ADDR                            (8 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //2 short
#define GLOBAL_msObjectPres_ADDR                            (10 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //2 short
#define GLOBAL_cbRepData_ADDR                               (12 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_bSubPayloadState_ADDR                        (13 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //1 short
#define GLOBAL_wBytesRead_ADDR                              (14 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //2 short
#define GLOBAL_wTotalDataBytes_ADDR                         (16 + GLOBAL_PAYLOAD_MAP_ENTRY_EX_ADDR) //2 short

//GLOBAL_PACKET_PARSE_INFO_EX_ADDR
#define GLOBAL_PACKET_PARSE_INFO_EX_ADDR                    (2224 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_fParityPacket_ADDR                           (0 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_fEccPresent_ADDR                             (1 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bECLen_ADDR                                  (2 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bPacketLenType_ADDR                          (3 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_cbParseOffset_ADDR                           (4 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cbPacketLenTypeOffset_ADDR                   (6 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_bPadLenType_ADDR                             (8 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bSequenceLenType_ADDR                        (9 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_fMultiPayloads_ADDR                          (10 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bOffsetBytes_ADDR                            (11 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bOffsetLenType_ADDR                          (12 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_wDuration_ADDR                               (13 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_cbPacketLenOffset_ADDR                       (14 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cbExplicitPacketLength_ADDR                  (16 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cbSequenceOffset_ADDR                        (18 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_dwSequenceNum_ADDR                           (20 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cbPadLenOffset_ADDR                          (22 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cbPadding_ADDR                               (24 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_dwSCR_ADDR                                   (26 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_bPayLenType_ADDR                             (28 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_bPayBytes_ADDR                               (29 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //1 short
#define GLOBAL_cbPayLenTypeOffset_ADDR                      (30 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short
#define GLOBAL_cPayloads_ADDR                               (32 + GLOBAL_PACKET_PARSE_INFO_EX_ADDR) //2 short

//GLOBAL_CWMAInputBitStream_ADDR
#define GLOBAL_CWMAInputBitStream_ADDR                      (2258 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_m_dwHeaderBuf_ADDR                           (0 + GLOBAL_CWMAInputBitStream_ADDR) //2 short
#define GLOBAL_m_pBufferBegin_ADDR                          (2 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_pBuffer_ADDR                               (3 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_cbBuflenBegin_ADDR                         (4 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_dwBitsLeft_ADDR                            (5 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_dwDot_ADDR                                 (6 + GLOBAL_CWMAInputBitStream_ADDR) //2 short
#define GLOBAL_m_iPrevPacketNum_ADDR                        (8 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_cbBuflen_ADDR                              (9 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_fAllowPackets_ADDR                         (10 + GLOBAL_CWMAInputBitStream_ADDR) //1 short
#define GLOBAL_m_fSuppressPacketLoss_ADDR                   (11 + GLOBAL_CWMAInputBitStream_ADDR) //1 short

//GLOBAL_CAudioObjectDecoder_ADDR
#define GLOBAL_CAudioObjectDecoder_ADDR                     (2270 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_m_fPacketLoss_ADDR                           (0 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_cFrmInPacket_ADDR                          (1 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_decsts_ADDR                                (2 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_subfrmdecsts_ADDR                          (3 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_hdrdecsts_ADDR                             (4 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_rlsts_ADDR                                 (5 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_iChannel_ADDR                              (6 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_iBand_ADDR                                 (7 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_fNoMoreData_ADDR                           (8 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short
#define GLOBAL_m_fLastSubFrame_ADDR                         (9 + GLOBAL_CAudioObjectDecoder_ADDR) //1 short

//GLOBAL_CAudioObject_ADDR
#define GLOBAL_CAudioObject_ADDR                            (2280 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_m_iPacketCurr_ADDR                           (0 + GLOBAL_CAudioObject_ADDR) //2 short
#define GLOBAL_m_cBitPackedFrameSize_ADDR                   (2 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iVersion_ADDR                              (3 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cRunOfZeros_ADDR                           (4 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iLevel_ADDR                                (5 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fNoiseSub_ADDR                             (6 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iMaxEscSize_ADDR                           (7 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iSign_ADDR                                 (8 + GLOBAL_CAudioObject_ADDR) //2 short
#define GLOBAL_m_iHighCutOffCurr_ADDR                       (10 + GLOBAL_CAudioObject_ADDR) //2 short
#define GLOBAL_m_iNextBarkIndex_ADDR                        (12 + GLOBAL_CAudioObject_ADDR) //2 short
#define GLOBAL_m_codecStatus_ADDR                           (14 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fSeekAdjustment_ADDR                       (15 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fPacketLossAdj_ADDR                        (16 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cChannel_ADDR                              (17 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iSamplingRate_ADDR                         (18 + GLOBAL_CAudioObject_ADDR) //2 short
#define GLOBAL_m_cSubband_ADDR                              (20 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fAllowSuperFrame_ADDR                      (21 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fAllowSubFrame_ADDR                        (22 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fV5Lpc_ADDR                                (23 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCurrReconCoef_ADDR                        (24 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCurrSubFrame_ADDR                         (25 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fHeaderReset_ADDR                          (26 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_stereoMode_ADDR                            (27 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iSubFrameSizeWithUpdate_ADDR               (28 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cPossibleWinSize_ADDR                      (29 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iMaxSubFrameDiv_ADDR                       (30 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cMinSubFrameSampleHalf_ADDR                (31 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iIncr_ADDR                                 (32 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cSubFrameSample_ADDR                       (33 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cSubFrameSampleHalf_ADDR                   (34 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cSubFrameSampleQuad_ADDR                   (35 + GLOBAL_CAudioObject_ADDR) //1 short
//GLOBAL_m_subfrmconfigPrev_ADDR
#define GLOBAL_m_subfrmconfigPrev_ADDR                      (36 + GLOBAL_CAudioObject_ADDR)
#define GLOBAL_m_subfrmconfigPrev_cSubFrame_ADDR            (0 + GLOBAL_m_subfrmconfigPrev_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN2                         (1 + GLOBAL_m_subfrmconfigPrev_ADDR) //1 short
#define GLOBAL_m_subfrmconfigPrev_rgiSubFrameSize_ADDR          (2 + GLOBAL_m_subfrmconfigPrev_ADDR) //16 short
#define GLOBAL_m_subfrmconfigPrev_rgiSubFrameStart_ADDR         (18 + GLOBAL_m_subfrmconfigPrev_ADDR) //16 short
#define GLOBAL_m_subfrmconfigPrev_rgfMaskUpdate_ADDR            (34 + GLOBAL_m_subfrmconfigPrev_ADDR) //16 short

//GLOBAL_m_subfrmconfigCurr_ADDR
#define GLOBAL_m_subfrmconfigCurr_ADDR                      (86 + GLOBAL_CAudioObject_ADDR)
#define GLOBAL_m_subfrmconfigCurr_cSubFrame_ADDR            (0 + GLOBAL_m_subfrmconfigCurr_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN3                         (1 + GLOBAL_m_subfrmconfigCurr_ADDR) //1 short
#define GLOBAL_m_subfrmconfigCurr_rgiSubFrameSize_ADDR          (2 + GLOBAL_m_subfrmconfigCurr_ADDR) //16 short
#define GLOBAL_m_subfrmconfigCurr_rgiSubFrameStart_ADDR         (18 + GLOBAL_m_subfrmconfigCurr_ADDR) //16 short
#define GLOBAL_m_subfrmconfigCurr_rgfMaskUpdate_ADDR            (34 + GLOBAL_m_subfrmconfigCurr_ADDR) //16 short

//GLOBAL_m_subfrmconfigNext_ADDR
#define GLOBAL_m_subfrmconfigNext_ADDR                      (136 + GLOBAL_CAudioObject_ADDR)
#define GLOBAL_m_subfrmconfigNext_cSubFrame_ADDR            (0 + GLOBAL_m_subfrmconfigNext_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN4                         (1 + GLOBAL_m_subfrmconfigNext_ADDR) //1 short
#define GLOBAL_m_subfrmconfigNext_rgiSubFrameSize_ADDR          (2 + GLOBAL_m_subfrmconfigNext_ADDR) //16 short
#define GLOBAL_m_subfrmconfigNext_rgiSubFrameStart_ADDR         (18 + GLOBAL_m_subfrmconfigNext_ADDR) //16 short
#define GLOBAL_m_subfrmconfigNext_rgfMaskUpdate_ADDR            (34 + GLOBAL_m_subfrmconfigNext_ADDR) //16 short

#define GLOBAL_m_cBitsSubbandMax_ADDR                       (186 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cFrameSample_ADDR                          (187 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cFrameSampleHalf_ADDR                      (188 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cLowCutOff_ADDR                            (189 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cHighCutOff_ADDR                           (190 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cLowCutOffLong_ADDR                        (191 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cHighCutOffLong_ADDR                       (192 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iWeightingMode_ADDR                        (193 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iEntropyMode_ADDR                          (194 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iQuantStepSize_ADDR                        (195 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_cValidBarkBand_ADDR                        (196 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_rgiBarkIndex_ADDR                          (197 + GLOBAL_CAudioObject_ADDR) //1 short
//GLOBAL_m_qstQuantStep_ADDR
#define GLOBAL_m_qstQuantStep_ADDR                          (198 + GLOBAL_CAudioObject_ADDR)
#define GLOBAL_m_Quant_iFracBits_ADDR                       (0 + GLOBAL_m_qstQuantStep_ADDR) //2 short
#define GLOBAL_m_Quant_iFraction_ADDR                       (2 + GLOBAL_m_qstQuantStep_ADDR) //2 short

#define GLOBAL_m_iSizePrev_ADDR                             (202 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iSizeCurr_ADDR                             (203 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iSizeNext_ADDR                             (204 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCoefRecurQ1_ADDR                          (205 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCoefRecurQ2_ADDR                          (206 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCoefRecurQ3_ADDR                          (207 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iCoefRecurQ4_ADDR                          (208 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iDiscardSilence_ADDR                       (209 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_fltFirstNoiseFreq_ADDR                     (210 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iFirstNoiseBand_ADDR                       (211 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iFirstNoiseIndex_ADDR                      (212 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_iFrameNumber_ADDR                          (213 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_tRandState_ADDR                            (214 + GLOBAL_CAudioObject_ADDR) //5 short
#define GLOBAL_STRUCT_BUFFER_ALIGN5                         (219 + GLOBAL_CAudioObject_ADDR) //1 short
#define GLOBAL_m_piSinForRecon_ADDR                         (220 + GLOBAL_CAudioObject_ADDR) //2 short

//GLOBAL_LeftChannelInfo_ADDR
#define GLOBAL_LeftChannelInfo_ADDR                         (2502 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_Lm_rgiCoefQ_ADDR                             (0 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiMaskQ_ADDR                             (1 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgbBandNotCoded_ADDR                      (2 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgffltSqrtBWRatio_ADDR                    (3 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiNoisePower_ADDR                        (4 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiHuffDecTbl_ADDR                        (5 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiRunEntry_ADDR                          (6 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiLevelEntry_ADDR                        (7 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_cSubbandActual_ADDR                       (8 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN6                         (9 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_iPower_ADDR                               (10 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN7                         (11 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_iActualPower_ADDR                         (12 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN8                         (13 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rguiWeightFactor_ADDR                     (14 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiCoefRecon_ADDR                         (15 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_iMaxMaskQ_ADDR                            (16 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN9                         (17 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_iCurrGetPCM_SubFrame_ADDR                 (18 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_rgiCoefReconCurr_ADDR                     (19 + GLOBAL_LeftChannelInfo_ADDR) //1 short
#define GLOBAL_Lm_piPrevOutputCurr_ADDR                     (20 + GLOBAL_LeftChannelInfo_ADDR) //2 short

#define GLOBAL_STRUCT_BUFFER_ALIGN10                        (2524 + GLOBAL_STRUCT_BUFFER_ADDR) //16 short
#define GLOBAL_EQtype_ADDR                                  (2540 + GLOBAL_STRUCT_BUFFER_ADDR) //2 short
//GLOBAL_RightChannelInfo_ADDR
#define GLOBAL_RightChannelInfo_ADDR                        (2542 + GLOBAL_STRUCT_BUFFER_ADDR)
#define GLOBAL_Rm_rgiCoefQ_ADDR                             (0 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiMaskQ_ADDR                             (1 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgbBandNotCoded_ADDR                      (2 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgffltSqrtBWRatio_ADDR                    (3 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiNoisePower_ADDR                        (4 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiHuffDecTbl_ADDR                        (5 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiRunEntry_ADDR                          (6 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiLevelEntry_ADDR                        (7 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_cSubbandActual_ADDR                       (8 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN11                        (9 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_iPower_ADDR                               (10 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN12                        (11 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_iActualPower_ADDR                         (12 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN13                        (13 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rguiWeightFactor_ADDR                     (14 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiCoefRecon_ADDR                         (15 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_iMaxMaskQ_ADDR                            (16 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_STRUCT_BUFFER_ALIGN14                        (17 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_iCurrGetPCM_SubFrame_ADDR                 (18 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_rgiCoefReconCurr_ADDR                     (19 + GLOBAL_RightChannelInfo_ADDR) //1 short
#define GLOBAL_Rm_piPrevOutputCurr_ADDR                     (20 + GLOBAL_RightChannelInfo_ADDR) //2 short


//GLOBAL_BUFFER_X_ADDR
#define GLOBAL_BUFFER_X_ADDR                                (2564 + WMADECODE_GLOBAL_X_BEGIN_ADDR)
#define GLOBAL_FFTcostabExAddr_ADDR                         (0 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_m_piSinForSaveHistory_ADDR                   (2 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_m_piSinForRecon_EXT_ADDR                     (4 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_OutPcmDMAExAddr_ADDR                         (6 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_PrevOutputDMAExAddr_ADDR                     (8 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_HuffDecTblDMAExAddr_ADDR                     (10 + GLOBAL_BUFFER_X_ADDR) //2 short
#define GLOBAL_FILE_POINTER_ADDR                            (12 + GLOBAL_BUFFER_X_ADDR) //2 short
//GLOBAL_m_rgiBarkIndexOrig_ADDR
#define GLOBAL_m_rgiBarkIndexOrig_ADDR                      (14 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_m_rgiBarkIndexOrig_BUFFER                    (0 + GLOBAL_m_rgiBarkIndexOrig_ADDR) //130 short

//GLOBAL_m_rgcValidBarkBand_ADDR
#define GLOBAL_m_rgcValidBarkBand_ADDR                      (144 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_m_rgcValidBarkBand_BUFFER                    (0 + GLOBAL_m_rgcValidBarkBand_ADDR) //5 short

#define GLOBAL_m_rgiFirstNoiseBand_ADDR                     (149 + GLOBAL_BUFFER_X_ADDR) //5 short
#define GLOBAL_Lm_rgbBandNotCoded_BUFFER                    (154 + GLOBAL_BUFFER_X_ADDR) //25 short
#define GLOBAL_Rm_rgbBandNotCoded_BUFFER                    (179 + GLOBAL_BUFFER_X_ADDR) //25 short
#define GLOBAL_Lm_rgiNoisePower_BUFFER                      (204 + GLOBAL_BUFFER_X_ADDR) //50 short
#define GLOBAL_Rm_rgiNoisePower_BUFFER                      (254 + GLOBAL_BUFFER_X_ADDR) //50 short
#define GLOBAL_Lm_rgffltSqrtBWRatio_BUFFER                  (304 + GLOBAL_BUFFER_X_ADDR) //100 short
#define GLOBAL_Rm_rgffltSqrtBWRatio_BUFFER                  (404 + GLOBAL_BUFFER_X_ADDR) //100 short
//GLOBAL_Lm_rgiMaskQ_BUFFER
#define GLOBAL_Lm_rgiMaskQ_BUFFER                           (504 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_L_m_rgiMaskQ_ADDR                            (0 + GLOBAL_Lm_rgiMaskQ_BUFFER) //25 short

//GLOBAL_Rm_rgiMaskQ_BUFFER
#define GLOBAL_Rm_rgiMaskQ_BUFFER                           (529 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_R_m_rgiMaskQ_ADDR                            (0 + GLOBAL_Rm_rgiMaskQ_BUFFER) //25 short

#define GLOBAL_PHdrParse_BUFFER_ADDR                        (554 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_1_ADDR                      (555 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_2_ADDR                      (556 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_3_ADDR                      (557 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_4_ADDR                      (558 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_5_ADDR                      (559 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_6_ADDR                      (560 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_PHdrParse_OFFSET_7_ADDR                      (561 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_wmaErr_ADDR                                  (562 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_wmaResult_ADDR                               (563 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_ChannelSelect_ADDR                           (564 + GLOBAL_BUFFER_X_ADDR) //1 short
#define GLOBAL_ChannelInfo_ADDR                             (565 + GLOBAL_BUFFER_X_ADDR) //1 short
//GLOBAL_ReLoadLPC_EADDR
#define GLOBAL_ReLoadLPC_EADDR                              (566 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_VOC_CFG_DMA_LADDR                            (0 + GLOBAL_ReLoadLPC_EADDR) //1 short
#define GLOBAL_VOC_CFG_DMA_SIZE                             (1 + GLOBAL_ReLoadLPC_EADDR) //1 short

//GLOBAL_VOC_CFG_DMA_EADDR
#define GLOBAL_VOC_CFG_DMA_EADDR                            (568 + GLOBAL_BUFFER_X_ADDR)
#define GLOBAL_ReLoadIDCT_EADDR                             (0 + GLOBAL_VOC_CFG_DMA_EADDR) //2 short


//GLOBAL_ROM_BUFFER_X_ADDR
#define GLOBAL_ROM_BUFFER_X_ADDR                            (3134 + WMADECODE_GLOBAL_X_BEGIN_ADDR)
#define TABLE_g_rgiHuffDecTblNoisePower_ADDR                (0 + GLOBAL_ROM_BUFFER_X_ADDR) //76 short
#define TABLE_g_rgiLsfReconLevel_ADDR                       (76 + GLOBAL_ROM_BUFFER_X_ADDR) //320 short
#define TABLE_g_sct64_ADDR                                  (396 + GLOBAL_ROM_BUFFER_X_ADDR) //34 short
#define TABLE_g_sct128_ADDR                                 (430 + GLOBAL_ROM_BUFFER_X_ADDR) //26 short
#define TABLE_g_sct256_ADDR                                 (456 + GLOBAL_ROM_BUFFER_X_ADDR) //34 short
#define TABLE_g_sct512_ADDR                                 (490 + GLOBAL_ROM_BUFFER_X_ADDR) //26 short
#define TABLE_g_sct1024_ADDR                                (516 + GLOBAL_ROM_BUFFER_X_ADDR) //34 short
#define TABLE_g_sct2048_ADDR                                (550 + GLOBAL_ROM_BUFFER_X_ADDR) //26 short
#define TABLE_g_InvQuadRootFraction_ADDR                    (576 + GLOBAL_ROM_BUFFER_X_ADDR) //514 short
#define TABLE_g_InvQuadRootExponent_ADDR                    (1090 + GLOBAL_ROM_BUFFER_X_ADDR) //130 short
//CONST_GLOBAL_ADDR
#define CONST_GLOBAL_ADDR                                   (1220 + GLOBAL_ROM_BUFFER_X_ADDR)
#define GLOBAL_g_uWMALCA_BUFFER                             (0 + CONST_GLOBAL_ADDR) //13 short
#define GLOBAL_g_uWMALCC_BUFFER                             (13 + CONST_GLOBAL_ADDR) //3 short
#define LPC_gLZLTable_ADD                                   (16 + CONST_GLOBAL_ADDR) //128 short

//CONST_VAR_ADDR
#define CONST_VAR_ADDR                                      (1364 + GLOBAL_ROM_BUFFER_X_ADDR)
#define CONST_SHORT_0_ADDR                                  (0 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_1_ADDR                                  (1 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_2_ADDR                                  (2 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_3_ADDR                                  (3 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_4_ADDR                                  (4 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_5_ADDR                                  (5 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_6_ADDR                                  (6 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_7_ADDR                                  (7 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_8_ADDR                                  (8 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_9_ADDR                                  (9 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_10_ADDR                                 (10 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_12_ADDR                                 (11 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_15_ADDR                                 (12 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_16_ADDR                                 (13 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_20_ADDR                                 (14 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_24_ADDR                                 (15 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_26_ADDR                                 (16 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_32_ADDR                                 (17 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_48_ADDR                                 (18 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_64_ADDR                                 (19 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_93_ADDR                                 (20 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_96_ADDR                                 (21 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_128_ADDR                                (22 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_192_ADDR                                (23 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_256_ADDR                                (24 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_512_ADDR                                (25 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_1024_ADDR                               (26 + CONST_VAR_ADDR) //1 short
#define CONST_SHORT_2048_ADDR                               (27 + CONST_VAR_ADDR) //1 short
#define CONST_WORD_1_ADDR                                   (28 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_15_ADDR                                  (30 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_0X7FFFFFFF_ADDR                          (32 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_0XFFFFFFFF_ADDR                          (34 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_0_ADDR                                   (36 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_64_ADDR                                  (38 + CONST_VAR_ADDR) //2 short
#define CONST_WORD_0X00007FFF_ADDR                          (40 + CONST_VAR_ADDR) //2 short
#define CONST_0XFF000000_ADDR                               (42 + CONST_VAR_ADDR) //2 short

#define TABLE_fftCostab_ADDR                                (1408 + GLOBAL_ROM_BUFFER_X_ADDR) //848 short
#define TABLE_g_rgiHuffDecTbl16ssOb_ADDR                    (TABLE_fftCostab_ADDR)
#define TABLE_g_rgiHuffDecTbl16smOb_ADDR                    (TABLE_fftCostab_ADDR)
#define TABLE_g_rgiHuffDecTbl44ssQb_ADDR                    (TABLE_fftCostab_ADDR)
#define TABLE_g_rgiHuffDecTbl44smQb_ADDR                    (TABLE_fftCostab_ADDR)
#define TABLE_g_rgiHuffDecTbl44ssOb_ADDR                    (TABLE_fftCostab_ADDR)
#define TABLE_g_rgiHuffDecTbl44smOb_ADDR                    (TABLE_fftCostab_ADDR)
#define LOCAL_OutputPCMBuff_ADDR                            (TABLE_fftCostab_ADDR)
#define TABLE_gRun16ssOb_ADDR                               (2256 + GLOBAL_ROM_BUFFER_X_ADDR) //24 short
#define TABLE_gRun16smOb_ADDR                               (2280 + GLOBAL_ROM_BUFFER_X_ADDR) //193 short
#define TABLE_gLevel16ssOb_ADDR                             (2473 + GLOBAL_ROM_BUFFER_X_ADDR) //39 short
#define TABLE_gRun44ssQb_ADDR                               (2512 + GLOBAL_ROM_BUFFER_X_ADDR) //5 short
#define TABLE_gLevel16smOb_ADDR                             (2517 + GLOBAL_ROM_BUFFER_X_ADDR) //135 short
#define TABLE_gRun44smQb_ADDR                               (2652 + GLOBAL_ROM_BUFFER_X_ADDR) //413 short
#define TABLE_gLevel44ssQb_ADDR                             (3065 + GLOBAL_ROM_BUFFER_X_ADDR) //251 short
#define TABLE_gLevel44smQb_ADDR                             (3316 + GLOBAL_ROM_BUFFER_X_ADDR) //94 short
#define TABLE_gRun44ssOb_ADDR                               (3410 + GLOBAL_ROM_BUFFER_X_ADDR) //846 short
#define TABLE_gRun44smOb_ADDR                               (4256 + GLOBAL_ROM_BUFFER_X_ADDR) //223 short
#define TABLE_gLevel44ssOb_ADDR                             (4479 + GLOBAL_ROM_BUFFER_X_ADDR) //444 short
#define TABLE_gLevel44smOb_ADDR                             (4923 + GLOBAL_ROM_BUFFER_X_ADDR) //1333 short
#define TABLE_BandWeightToEQtype_ADDR                       (6256 + GLOBAL_ROM_BUFFER_X_ADDR) //80 short
#define TABLE_BandFreqToSamplingrate_ADDR                   (6336 + GLOBAL_ROM_BUFFER_X_ADDR) //70 short

#define GLOBAL_Mixer_flag_ADDR                              (9540 + WMADECODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_adjust_eq_curve_enable_ADDR                  (9541 + WMADECODE_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_adjust_eq_curve_reset_ADDR                   (9542 + WMADECODE_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_adjust_eq_curve_tab_ADDR                     (9544 + WMADECODE_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_Digital_gain_ADDR                            (9554 + WMADECODE_GLOBAL_X_BEGIN_ADDR) //2 short

//GLOBAL_VARS
//GLOBAL_BUFFER_Y_ADDR
#define GLOBAL_BUFFER_Y_ADDR                                (0 + WMADECODE_GLOBAL_Y_BEGIN_ADDR)
//GLOBAL_m_rgiCoefReconOrig_ADDR
#define GLOBAL_m_rgiCoefReconOrig_ADDR                      (0 + GLOBAL_BUFFER_Y_ADDR)
#define GLOBAL_Lm_rgiCoefReconOrig_BUFFER                   (0 + GLOBAL_m_rgiCoefReconOrig_ADDR) //4096 short

//GLOBAL_Rm_rgiCoefReconOrig_ADDR
#define GLOBAL_Rm_rgiCoefReconOrig_ADDR                     (4096 + GLOBAL_BUFFER_Y_ADDR)
//GLOBAL_Rm_rgiCoefReconOrig_BUFFER
#define GLOBAL_Rm_rgiCoefReconOrig_BUFFER                   (0 + GLOBAL_Rm_rgiCoefReconOrig_ADDR)
#define GLOBAL_STRUCT_BUFFER_ALIGN17                        (0 + GLOBAL_Rm_rgiCoefReconOrig_BUFFER) //1000 short
//LOCAL_ChLeft_pWF_ADDR
#define LOCAL_ChLeft_pWF_ADDR                               (1000 + GLOBAL_Rm_rgiCoefReconOrig_BUFFER)
#define GLOBAL_Lm_rguiWeightFactor_BUFFER                   (0 + LOCAL_ChLeft_pWF_ADDR) //1024 short

//LOCAL_ChRight_pWF_ADDR
#define LOCAL_ChRight_pWF_ADDR                              (2024 + GLOBAL_Rm_rgiCoefReconOrig_BUFFER)
#define GLOBAL_Rm_rguiWeightFactor_BUFFER                   (0 + LOCAL_ChRight_pWF_ADDR) //1024 short




#define GLOBAL_STRUCT_BUFFER_ALIGN19                        (7144 + WMADECODE_GLOBAL_Y_BEGIN_ADDR) //1048 short
//GLOBAL_ROM_BUFFER_Y_ADDR
#define GLOBAL_ROM_BUFFER_Y_ADDR                            (8192 + WMADECODE_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_getMask_ADDR                                  (0 + GLOBAL_ROM_BUFFER_Y_ADDR) //66 short
#define TABLE_g_rgiHuffDecTblMsk_ADDR                       (66 + GLOBAL_ROM_BUFFER_Y_ADDR) //226 short
#define TABLE_rgSinCosTables_ADDR                           (292 + GLOBAL_ROM_BUFFER_Y_ADDR) //18 short
#define TABLE_rgiMaskMinusPower10_ADDR                      (310 + GLOBAL_ROM_BUFFER_Y_ADDR) //144 short
#define TABLE_rgiMaskPlusPower10_ADDR                       (454 + GLOBAL_ROM_BUFFER_Y_ADDR) //100 short
#define TABLE_rgDBPower10_ADDR                              (554 + GLOBAL_ROM_BUFFER_Y_ADDR) //256 short
#define TABLE_g_InverseFraction_ADDR                        (810 + GLOBAL_ROM_BUFFER_Y_ADDR) //514 short
#define TABLE_g_SqrtFraction_ADDR                           (1324 + GLOBAL_ROM_BUFFER_Y_ADDR) //514 short


/***********************
 **  WMADECODE_LOCAL  **
 ***********************/

//CII_auSaveHistoryMono_Tbl_VOC
#define CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR          (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_ChannelInfo_ADDR                              (0 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_pcSampleGet_ADDR                              (1 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iEnd_ADDR                                     (2 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iSegEnd_ADDR                                  (3 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_i_ADDR                                        (4 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_BUFF_ALIGN2                                   (5 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_RecTblBuff_ADDR                               (6 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //30 short
#define LOCAL_RecTblBuffEnd_ADDR                            (36 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_BUFF_ALIGN3                                   (37 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //17 short
#define LOCAL_WORD_0XFFFF8000_ADDR                          (54 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_RecTblDMAExAddr_ADDR                          (56 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_cSamplesLeft_ADDR                             (58 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_BUFF_ALIGN4                                   (59 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //39 short
#define LOCAL_PrevOutputBuff_ADDR                           (98 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //16 short
#define LOCAL_PrevOutputDMAExAddr_ADDR                      (114 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_PrevOutputCurrDMAExAddr_ADDR                  (116 + CII_auSaveHistoryMono_Tbl_VOC_X_BEGIN_ADDR) //2 short

//CII_prvDecodeFrameHeader_VOC
#define CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR           (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_cBitsNeed_ADDR                                (0 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iSizePrev_ADDR                                (1 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iSizeCurr_ADDR                                (2 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iSizeNext_ADDR                                (3 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_fSkipAll_ADDR                                 (4 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_fUpdateMask_ADDR                              (5 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_rgiMaskQ_ADDR                                 (6 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_m_iMaxMaskQ_ADDR                              (7 + CII_prvDecodeFrameHeader_VOC_X_BEGIN_ADDR) //1 short

//CII_auReconCoefficentsHighRate_VOC
#define CII_auReconCoefficentsHighRate_VOC_X_BEGIN_ADDR         (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_m_ctCoefRecon_ADDR                            (0 + CII_auReconCoefficentsHighRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_temp_qfltQuantizertiFracBits_ADDR             (2 + CII_auReconCoefficentsHighRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_temp_qfltQuantizeriFraction_ADDR              (4 + CII_auReconCoefficentsHighRate_VOC_X_BEGIN_ADDR) //2 short

//CII_prvInverseQuantizeMidRate
#define CII_prvInverseQuantizeMidRate_X_BEGIN_ADDR          (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_BUFF_ALIGN7                                   (0 + CII_prvInverseQuantizeMidRate_X_BEGIN_ADDR) //12 short
//LOCAL_LEVEL3_Y_BEGIN_ADDR
#define LOCAL_LEVEL3_Y_BEGIN_ADDR                           (12 + CII_prvInverseQuantizeMidRate_X_BEGIN_ADDR)
#define LOCAL_BUFF_ALIGN8                                   (0 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //218 short
#define LOCAL_iBark_ADDR                                    (218 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iCoefQ_ADDR                                   (219 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iRecon_ADDR                                   (220 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iUBLimitOniRecon_ADDR                         (221 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_uiInvMaxWeight_ADDR                           (222 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_uiQuantStepXInvMaxWeight_ADDR                 (224 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_cQSIMWFracBits_ADDR                           (226 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_cMaxWeightFracBits_ADDR                       (228 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_uiMaxWeight_ADDR                              (230 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_qrand_ADDR                                    (232 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_uiWeightFactor_ADDR                           (234 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_cWFFracBits_ADDR                              (236 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_QuantStepXMaxWeightXWeightFactor_ADDR         (238 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_cFracBits_ADDR                                (240 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_iCoef_ADDR                                    (242 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_iCoefScaled_ADDR                              (244 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_iNoiseQuant_ADDR                              (246 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_QuantStepXMaxWeightXWeightFactorXDither_ADDR          (248 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_rgfMaskNeededForBark_BUFF                     (250 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //25 short
#define LOCAL_MaskResampleInfo_ADDR                         (275 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //3 short
#define LOCAL_rgiBarkIndexResampled_ADDR                    (278 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_rgiBarkIndex_ADDR                             (279 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iBarkResampled_ADDR                           (280 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iReconTarget_ADDR                             (281 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_qfltQuantizer_ADDR                            (282 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //4 short
#define LOCAL_iShift_ADDR                                   (286 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_uiNoisePowerXinvMaxWeight_ADDR                (288 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_iNoiseRand_ADDR                               (290 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_iNoiseBand_ADDR                               (292 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iCurrBand_ADDR                                (293 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iMaskBand_ADDR                                (294 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iRsmpBand_ADDR                                (295 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_cMaskHighCutOff_ADDR                          (296 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //2 short
#define LOCAL_rgiMaskQ4BandNotCoded_BUFF                    (298 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //10 short
#define LOCAL_cNoiseBand_ADDR                               (308 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iLoopMax_ADDR                                 (309 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_fflt_temp_ADDR                                (310 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //4 short
#define LOCAL_ffltNoisePower_ADDR                           (314 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //4 short
#define LOCAL_fAllBandsSynced_ADDR                          (318 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iCurrStart_ADDR                               (319 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iCurrEnd_ADDR                                 (320 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_iMaskEnd_ADDR                                 (321 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //1 short
#define LOCAL_ffltBandWeight_BUFF                           (322 + LOCAL_LEVEL3_Y_BEGIN_ADDR) //40 short


//CII_prvGetBandWeightLowRate_VOC
#define CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR            (8 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_m_rgfltBandWeight_ADDR                        (0 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //100 short
#define LOCAL_temp_ffltiFracBits_ADDR                       (100 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_temp_ffltiFraction_ADDR                       (102 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_m_fflt_iFracBits_ADDR                         (104 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_m_fflt_iFraction_ADDR                         (106 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_m_cNoiseBand_ADDR                             (108 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_m_rgbBandNotCoded_ADDR                        (109 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_m_rgxWeightFactor_ADDR                        (110 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_m_iBand_ADDR                                  (111 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_iEnd2Start_ADDR                               (112 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_cNoiseBandsub1_ADDR                           (113 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_m_rgffltSqrtBWRatio_ADDR                      (114 + CII_prvGetBandWeightLowRate_VOC_X_BEGIN_ADDR) //1 short

//CII_auLsp2lpc
#define CII_auLsp2lpc_X_BEGIN_ADDR                          (8 + WMADECODE_LOCAL_X_BEGIN_ADDR)
//LOCAL_LEVEL1_Y_BEGIN_ADDR
#define LOCAL_LEVEL1_Y_BEGIN_ADDR                           (0 + CII_auLsp2lpc_X_BEGIN_ADDR)
#define LOCAL_BUFF_ALIGN9                                   (0 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //4 short
#define LOCAL_shortSeq_ADDR                                 (4 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //6 short
#define LOCAL_longSeq_ADDR                                  (10 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //200 short
#define LOCAL_shortlength_ADD                               (210 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //1 short
#define LOCAL_longlength_ADD                                (211 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //1 short
#define LOCAL_order_ADD                                     (212 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //1 short
#define LOCAL_BUFF_ALIGN5                                   (213 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //1 short
#define LOCAL_lsfQ_ADD                                      (214 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //10 short
#define LOCAL_lpc_ADD                                       (224 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //20 short
#define LOCAL_p_ADD                                         (244 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //20 short
#define LOCAL_q_ADD                                         (264 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //20 short
#define LOCAL_ChannelCh_ADDR                                (284 + LOCAL_LEVEL1_Y_BEGIN_ADDR) //1 short


//CII_prvGetNextRunDEC_VOC
#define CII_prvGetNextRunDEC_VOC_X_BEGIN_ADDR               (6 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define GLOBAL_Temp16_ADDR                                  (0 + CII_prvGetNextRunDEC_VOC_X_BEGIN_ADDR) //1 short
#define GLOBAL_Temp16_2_ADDR                                (1 + CII_prvGetNextRunDEC_VOC_X_BEGIN_ADDR) //1 short
#define GLOBAL_Temp32_ADDR                                  (2 + CII_prvGetNextRunDEC_VOC_X_BEGIN_ADDR) //2 short
#define GLOBAL_Temp16_3_ADDR                                (4 + CII_prvGetNextRunDEC_VOC_X_BEGIN_ADDR) //1 short

//CII_prvFFT4DCT_VOC
#define CII_prvFFT4DCT_VOC_X_BEGIN_ADDR                     (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_DMA_BUFFER                                    (0 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //256 short
#define LOCAL_pData_ADDR                                    (256 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_n_ADDR                                    (257 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_i_ADDR                                    (258 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_j_ADDR                                    (259 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_CR_ADDR                                   (260 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_SI_ADDR                                   (261 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_t_ADDR                                    (262 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_32divt_ADDR                               (263 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_ur_ADDR                                   (264 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_VAR_ui_ADDR                                   (266 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_VAR_ndivt_ADDR                                (268 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_pxk_ADDR                                  (269 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_K2I_ADDR                                  (270 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_tHalf_ADDR                                (271 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_pData32_ADDR                                  (272 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short
#define LOCAL_VAR_Nlogt_ADDR                                (273 + CII_prvFFT4DCT_VOC_X_BEGIN_ADDR) //1 short

//CII_INBF_VOC
#define CII_INBF_VOC_X_BEGIN_ADDR                           (0 + WMADECODE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_CI_ADDR                                       (0 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_CR_ADDR                                       (2 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_CI1_ADDR                                      (4 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_CR1_ADDR                                      (6 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_SimStack32_ADDR                               (8 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_iTr_ADDR                                      (10 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_iBi_ADDR                                      (12 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_BUFF_ALIGN1                                   (14 + CII_INBF_VOC_X_BEGIN_ADDR) //48 short
#define LOCAL_STEP_ADDR                                     (62 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_CR2_ADDR                                      (64 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short
#define LOCAL_CI2_ADDR                                      (66 + CII_INBF_VOC_X_BEGIN_ADDR) //2 short

//CII_prvConvolve
#define CII_prvConvolve_X_BEGIN_ADDR                        (294 + WMADECODE_LOCAL_X_BEGIN_ADDR)
//LOCAL_LEVEL2_Y_BEGIN_ADDR
#define LOCAL_LEVEL2_Y_BEGIN_ADDR                           (0 + CII_prvConvolve_X_BEGIN_ADDR)
#define LOCAL_ret_ADDR                                      (0 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //16 short
#define LOCAL_uiFrac1_ADDR                                  (16 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_iMSF8_ADDR                                    (18 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CmS_ADDR                                      (20 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CpS_ADDR                                      (22 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T2_ADDR                                       (24 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T4_ADDR                                       (26 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T6_ADDR                                       (28 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T7_ADDR                                       (30 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T8_ADDR                                       (32 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_T9_ADDR                                       (34 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_TA_ADDR                                       (36 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_TB_ADDR                                       (38 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C2_ADDR                                       (40 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S2_ADDR                                       (42 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C3_ADDR                                       (44 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S3_ADDR                                       (46 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C1_ADDR                                       (48 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S1_ADDR                                       (50 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_k_lpc_ADDR                                    (52 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //1 short
#define LOCAL_BUFF_ALIGN6                                   (53 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //1 short
#define LOCAL_pTmp_ADDR                                     (54 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //64 short
#define LOCAL_SL16_ADDR                                     (118 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CL16_ADDR                                     (120 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_SL8_ADDR                                      (122 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CL8_ADDR                                      (124 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_SL3by16_ADDR                                  (126 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CL3by16_ADDR                                  (128 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_SL4_ADDR                                      (130 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CL4_ADDR                                      (132 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_t1pO7_ADDR                                    (134 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO1pO9_ADDR                                   (136 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO0pO8_ADDR                                   (138 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO4pO6_ADDR                                   (140 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO4mO6_ADDR                                   (142 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_t1pO7pO3_ADDR                                 (144 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_t1pO7mO3_ADDR                                 (146 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO1pO5pO9_ADDR                                (148 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tO0pO2pO4pO6pO8_ADDR                          (150 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_D_ADDR                                        (152 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_E_ADDR                                        (154 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_F_ADDR                                        (156 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_G_ADDR                                        (158 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_H_ADDR                                        (160 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_I_ADDR                                        (162 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_J_ADDR                                        (164 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_iSizeBy2_ADDR                                 (166 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //1 short
#define LOCAL_interval_ADDR                                 (167 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //1 short
#define LOCAL_S4_ADDR                                       (168 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C4_ADDR                                       (170 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_STEP1_ADDR                                    (172 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_STEP4_ADDR                                    (174 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S4p_ADDR                                      (176 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C4p_ADDR                                      (178 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S1p_ADDR                                      (180 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C1p_ADDR                                      (182 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CpS1_ADDR                                     (184 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CmS1_ADDR                                     (186 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S8_ADDR                                       (188 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C8_ADDR                                       (190 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CmS2_ADDR                                     (192 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CpS2_ADDR                                     (194 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tCpS1x_ADDR                                   (196 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tCpS2x_ADDR                                   (198 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tCmS2x_ADDR                                   (200 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_tCmS1x_ADDR                                   (202 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_S_ADDR                                        (204 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_C_ADDR                                        (206 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CLS1_ADDR                                     (208 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_SLS1_ADDR                                     (210 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_SLC1_ADDR                                     (212 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_CLC1_ADDR                                     (214 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_wtLpcSpecMax_ADDR                             (216 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //2 short
#define LOCAL_ChannelInfo_Select_ADDR                       (218 + LOCAL_LEVEL2_Y_BEGIN_ADDR) //1 short


/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x2756   RAM_Y: size 0x2800, used 0x272e

#endif
