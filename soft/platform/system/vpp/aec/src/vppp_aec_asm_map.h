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

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                                    2758
#define COMMON_GLOBAL_Y_SIZE                                    62
#define HI_GLOBAL_X_SIZE                                        0
#define HI_GLOBAL_Y_SIZE                                        0
#define BYE_GLOBAL_X_SIZE                                       2
#define BYE_GLOBAL_Y_SIZE                                       2
#define HI_LOCAL_X_SIZE                                         0
#define HI_LOCAL_Y_SIZE                                         0
#define BYE_LOCAL_X_SIZE                                        4
#define BYE_LOCAL_Y_SIZE                                        2
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define HI_GLOBAL_X_BEGIN_ADDR                                  (2758 + RAM_X_BEGIN_ADDR)
#define HI_GLOBAL_Y_BEGIN_ADDR                                  (62 + RAM_Y_BEGIN_ADDR)
#define BYE_GLOBAL_X_BEGIN_ADDR                                 (2758 + RAM_X_BEGIN_ADDR)
#define BYE_GLOBAL_Y_BEGIN_ADDR                                 (62 + RAM_Y_BEGIN_ADDR)
#define HI_LOCAL_X_BEGIN_ADDR                                   (2760 + RAM_X_BEGIN_ADDR)
#define HI_LOCAL_Y_BEGIN_ADDR                                   (64 + RAM_Y_BEGIN_ADDR)
#define BYE_LOCAL_X_BEGIN_ADDR                                  (2760 + RAM_X_BEGIN_ADDR)
#define BYE_LOCAL_Y_BEGIN_ADDR                                  (64 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (2764 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (66 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define VPP_HELLO_MODE_NO                                   (-1) //alias
#define VPP_HELLO_MODE_HI                                   (0) //alias
#define VPP_HELLO_MODE_BYE                                  (1) //alias
#define VPP_HELLO_ERROR_NO                                  (0) //alias
#define VPP_HELLO_ERROR_YES                                 (-1) //alias
#define VPP_HELLO_SAYS_HI                                   (0x0111) //alias
#define VPP_HELLO_SAYS_BYE                                  (0x0B1E) //alias
#define PassThrough                                         (0) //alias
#define AdaptiveFilter                                      (1) //alias
#define AdaptiveFilterPostFilter                            (2) //alias
#define NoiseSuppression                                    (3) //alias
#define LowDelayOpenLoopAF                                  (4) //alias
#define TwoStepNoiseSuppression                             (5) //alias
#define AdaptiveFilterPostFilterEnhanced                    (6) //alias
#define ECHO_COHF                                           (0) //alias
#define ECHO_TF                                             (1) //alias
//VPP_HELLO_CONST_STRUCT
#define VPP_HELLO_CONST_STRUCT                              (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define VPP_HELLO_CONST_SAYS_HI                             (0 + VPP_HELLO_CONST_STRUCT) //1 word
#define VPP_HELLO_CONST_SAYS_BYE                            (2 + VPP_HELLO_CONST_STRUCT) //1 word

#define GLOBAL_INPUT_MIC_PCM_ADDR                           (4 + COMMON_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_INPUT_SPK_PCM_ADDR                           (164 + COMMON_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_OUTPUT_MIC_PCM_ADDR                          (324 + COMMON_GLOBAL_X_BEGIN_ADDR) //160 short
#define r_SpkAnaFilterDelayLine                             (484 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_MicAnaFilterDelayLine                             (612 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_OutSynFilterDelayLine                             (740 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define g_FilterBank_MulBuf                                 (868 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define g_SpkAnaFilterOutBufI                               (996 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_SpkAnaFilterOutBufQ                               (1012 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufI                               (1028 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufQ                               (1044 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufI                                (1060 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufQ                                (1076 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 short
#define r_AF_SpkSigBufI                                     (1092 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 short
#define r_AF_SpkSigBufQ                                     (1164 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 short
#define r_AF_FilterBufI                                     (1236 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_AF_FilterBufQ                                     (1380 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 word
#define g_AF_FilterOutBufI                                  (1524 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve00_addr                                      (1533 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_FilterOutBufQ                                  (1534 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve01_addr                                      (1543 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufI                                 (1544 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve02_addr                                      (1553 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufQ                                 (1554 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve03_addr                                      (1563 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufI                                    (1564 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve04_addr                                      (1573 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufQ                                    (1574 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve05_addr                                      (1583 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ShortFilterBufI                                (1584 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_ShortFilterBufQ                                (1592 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_tempCmplxBufI                                  (1600 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 word
#define g_AF_tempCmplxBufQ                                  (1616 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 word
#define r_AF_SpkSigPower                                    (1632 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_SpkSigSmoothPower                              (1650 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkI                                       (1668 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkQ                                       (1686 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_AF_StepSize                                       (1704 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve06_addr                                      (1713 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_SmoothErrPower                                 (1714 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoisePowerEstBuf                               (1732 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoiseFloorDevCnt                               (1750 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_DTD_ch                                            (1759 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_PostSToNRatio                                  (1760 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define CLChnl                                              (1769 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_PrevSigPowerNBuf                               (1770 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSigPowerEBuf                               (1788 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_NoiseWeight                                    (1806 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_EC_ErrToOutRatio                                  (1815 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_NoiseAmpWeight                                 (1816 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define m_AF_FrameCnt                                       (1825 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define m_Taps                                              (1826 + COMMON_GLOBAL_X_BEGIN_ADDR) //4 short
#define m_Registers                                         (1830 + COMMON_GLOBAL_X_BEGIN_ADDR) //32 short
#define TEMP_r_AF_SpkSigBufIQ_addr_addr                     (1862 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_FilterBufIQ_addr_addr                     (1864 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_FilterOutBufIQ_addr_addr                  (1866 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_OldSpkDataBufIQ_addr_addr                 (1868 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_ErrDataBufIQ_addr_addr                    (1870 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_MicAnaFilterOutBufIQ_addr_addr               (1872 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_SpkAnaFilterOutBufIQ_addr_addr               (1874 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_SpkSigPower_addr_addr                     (1876 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define AnaFilterCoef                                       (1878 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define SynFilterCoef                                       (2006 + COMMON_GLOBAL_X_BEGIN_ADDR) //128 short
#define wnreal                                              (2134 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 short
#define wnimag                                              (2142 + COMMON_GLOBAL_X_BEGIN_ADDR) //8 short
#define AdaptTable                                          (2150 + COMMON_GLOBAL_X_BEGIN_ADDR) //32 short
#define g_counter_subframe                                  (2182 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_ch                                                (2183 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define real_int                                            (2184 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 word
#define imag_int                                            (2216 + COMMON_GLOBAL_X_BEGIN_ADDR) //16 word
#define r_PF_CL_MicSmoothPower                              (2248 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoise                                    (2252 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkSmoothPower                              (2256 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkNoise                                    (2260 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoiseFloorDevCnt                         (2264 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_SpkNoiseFloorDevCnt                         (2266 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifMicSigOn                                  (2268 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifSpkSigOn                                  (2270 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ChnlState                                   (2272 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_State                                       (2274 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_DTCountDown                                 (2275 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_NoisePowerSmoothEstBuf                         (2276 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_PriorSToNRatio                                 (2294 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_PF_PriorSToERatio                                 (2303 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_SpkPowerBuf                                    (2312 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CCErrSpkVecBufI                                (2330 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_CCErrSpkVecBufQ                                (2474 + COMMON_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_EchoPowerEstBuf                                (2618 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_EchoWeight                                     (2636 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_CL_DTCountUp                                   (2645 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_ErrSmoothPower                              (2646 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_AF_FilterAmpI                                     (2650 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_AF_FilterAmpQ                                     (2659 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_PrevEchoPowerEst                               (2668 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSpkPower                                   (2686 + COMMON_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CL_DTDXcorrMicErrI                             (2704 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicErrQ                             (2708 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoI                            (2712 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoQ                            (2716 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicSmoothPower                           (2720 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDErrSmoothPower                           (2724 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDEchoSmoothPower                          (2728 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoisePower                            (2732 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoiseFloorDevCnt                      (2736 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDDecision                                 (2740 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTD2ndDecision                              (2744 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 word
#define pInstantSNR_t                                       (2748 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define pInstantSNR_f                                       (2749 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define SumMicPower                                         (2750 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumSpkPower                                         (2752 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumErrPower                                         (2754 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 word
#define PFCLDTDOut                                          (2756 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short

//GLOBAL_VARS
//VPP_HELLO_CFG_STRUCT
#define VPP_HELLO_CFG_STRUCT                                (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_MODE                                         (0 + VPP_HELLO_CFG_STRUCT) //1 short
#define GLOBAL_RESET_ADDR                                   (1 + VPP_HELLO_CFG_STRUCT) //1 short

//VPP_HELLO_STATUS_STRUCT
#define VPP_HELLO_STATUS_STRUCT                             (2 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_USED_MODE                                    (0 + VPP_HELLO_STATUS_STRUCT) //1 short
#define GLOBAL_ERR_STATUS                                   (1 + VPP_HELLO_STATUS_STRUCT) //1 short
#define GLOBAL_HELLO_STATUS                                 (2 + VPP_HELLO_STATUS_STRUCT) //1 word

//VPP_HELLO_CODE_CFG_STRUCT
#define VPP_HELLO_CODE_CFG_STRUCT                           (6 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_HI_CODE_PTR                                  (0 + VPP_HELLO_CODE_CFG_STRUCT) //1 word
#define GLOBAL_BYE_CODE_PTR                                 (2 + VPP_HELLO_CODE_CFG_STRUCT) //1 word
#define GLOBAL_CONST_PTR                                    (4 + VPP_HELLO_CODE_CFG_STRUCT) //1 word

//m_AECProcPara_ADDR
#define m_AECProcPara_ADDR                                  (12 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define m_AECProcPara_ProcessMode                           (0 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_InitRegularFactor                     (1 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFUpdateFactor                        (2 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFCtrlSpeedUpFactor                   (3 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFFilterAmp                           (4 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoEstMethod                         (5 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseUpdateFactor                     (6 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRUpdateFactor                       (7 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRLimit                              (8 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseOverEstFactor                    (9 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseGainLimit                        (10 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoUpdateFactor                      (11 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERUpdateFactor                       (12 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERLimit                              (13 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoOverEstFactor                     (14 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoGainLimit                         (15 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_CNGFactor                             (16 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpGain                               (17 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseMin                              (18 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFPrevEchoEstLimit                    (20 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFDTEchoOverEstFactor                 (22 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_pPFEchoGainLimitAmp                   (23 + m_AECProcPara_ADDR) //9 short
#define m_AECProcPara_NoiseGainLimitStep                    (32 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpThr_ADDR                           (33 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDataSmoothFactor                  (34 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLNoiseUpdateFactor                 (35 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrSigDet                         (36 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDT                             (37 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_DataSmoothFactor                      (38 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLChanlGain                         (39 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrNT                             (40 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDTDiff                         (41 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTHoldTime                        (42 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTStartTime                       (43 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDUpdateFactor                   (44 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDThreshold                      (45 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTD2ndThreshold                   (46 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_StrongEchoFlag                        (47 + m_AECProcPara_ADDR) //1 short
#define AECProcPara_PFCLDTDThrRatio                         (48 + m_AECProcPara_ADDR) //1 short
#define AECProcPara_reserve                                 (49 + m_AECProcPara_ADDR) //1 short


/*****************
 **  HI_GLOBAL  **
 *****************/

/******************
 **  BYE_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define GLOBAL_BYE                                          (0 + BYE_GLOBAL_X_BEGIN_ADDR) //1 word

//GLOBAL_VARS
#define GLOBAL_BYE2                                         (0 + BYE_GLOBAL_Y_BEGIN_ADDR) //1 short

/****************
 **  HI_LOCAL  **
 ****************/

/*****************
 **  BYE_LOCAL  **
 *****************/

//vpp_HelloBye
#define vpp_HelloBye_X_BEGIN_ADDR                           (0 + BYE_LOCAL_X_BEGIN_ADDR)
#define LOCAL_BYE                                           (0 + vpp_HelloBye_X_BEGIN_ADDR) //4 short

//vpp_HelloBye
#define vpp_HelloBye_Y_BEGIN_ADDR                           (0 + BYE_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_BYE2                                          (0 + vpp_HelloBye_Y_BEGIN_ADDR) //1 word

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x0acc   RAM_Y: size 0x2800, used 0x0042

#endif
