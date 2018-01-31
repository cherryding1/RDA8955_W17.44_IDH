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





#define CONST16_START_ADDR      (0 + ROM_Z_BEGIN_ADDR)

#define CONST_0x1fff_ADDR                                   (0 + CONST16_START_ADDR) //1 short
#define CONST_5462_ADDR                                     (1 + CONST16_START_ADDR) //1 short
#define CONST_0x3fff_ADDR                                   (2 + CONST16_START_ADDR) //1 short
#define CONST_0x0800_ADDR                                   (3 + CONST16_START_ADDR) //1 short
#define CONST_21875_ADDR                                    (4 + CONST16_START_ADDR) //1 short
#define CONST_0x2b33_ADDR                                   (5 + CONST16_START_ADDR) //1 short
#define CONST_21299_ADDR                                    (6 + CONST16_START_ADDR) //1 short
#define CONST_5443_ADDR                                     (7 + CONST16_START_ADDR) //1 short

/**************************
 **  CONST32_START_ADDR  **
 **************************/

#define CONST32_START_ADDR      (8 + ROM_Z_BEGIN_ADDR)

#define CONST_0x40000000_ADDR                               (0 + CONST32_START_ADDR) //[2]words
#define CONST_0x7FFFFFFF_ADDR                               (2 + CONST32_START_ADDR) //[2]words
#define CONST_0x80000000_ADDR                               (4 + CONST32_START_ADDR) //[2]words
#define CONST_0x8000FFFF_ADDR                               (6 + CONST32_START_ADDR) //[2]words
#define CONST_0x70816958_ADDR                               (8 + CONST32_START_ADDR) //[2]words
#define CONST_D_1_4_ADDR                                    (10 + CONST32_START_ADDR) //[2]words
#define CONST_D_1_8_ADDR                                    (12 + CONST32_START_ADDR) //[2]words
#define CONST_D_1_16_ADDR                                   (14 + CONST32_START_ADDR) //[2]words
#define CONST_D_1_32_ADDR                                   (16 + CONST32_START_ADDR) //[2]words
#define CONST_D_1_64_ADDR                                   (18 + CONST32_START_ADDR) //[2]words
#define CONST_15000_ADDR                                    (20 + CONST32_START_ADDR) //[2]words
#define CONST_0x00000800L_ADDR                              (22 + CONST32_START_ADDR) //[2]words

/****************************
 **  EFR_TABLE_START_ADDR  **
 ****************************/

#define EFR_TABLE_START_ADDR        (32 + ROM_Z_BEGIN_ADDR)

#define TABLE_WINDOW_160_80_ADDR                            (0 + EFR_TABLE_START_ADDR) //[240]shorts
#define TABLE_WINDOW_232_8_ADDR                             (240 + EFR_TABLE_START_ADDR) //[240]shorts
#define TABLE_MEAN_LSF_ADDR                                 (480 + EFR_TABLE_START_ADDR) //[10]shorts
#define TABLE_SLOPE_ADDR                                    (490 + EFR_TABLE_START_ADDR) //[64]shorts
#define TABLE_LSP_LSF_ADDR                                  (554 + EFR_TABLE_START_ADDR) //[66]shorts
#define TABLE_LOG2_ADDR                                     (620 + EFR_TABLE_START_ADDR) //[34]shorts
#define TABLE_LAG_L_ADDR                                    (654 + EFR_TABLE_START_ADDR) //[10]shorts
#define TABLE_LAG_H_ADDR                                    (664 + EFR_TABLE_START_ADDR) //[10]shorts
#define TABLE_INV_SQRT_ADDR                                 (674 + EFR_TABLE_START_ADDR) //[50]shorts
#define TABLE_GRID_ADDR                                     (724 + EFR_TABLE_START_ADDR) //[62]shorts
#define TABLE_POW2_ADDR                                     (786 + EFR_TABLE_START_ADDR) //[34]shorts
#define STATIC_CONST_INTERP_FACTOR_ADDR                     (820 + EFR_TABLE_START_ADDR) //[24]shorts
#define STATIC_CONST_DHF_MASK_ADDR                          (844 + EFR_TABLE_START_ADDR) //[58]shorts
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR                    (902 + EFR_TABLE_START_ADDR) //[16]shorts
#define STATIC_CONST_F_GAMMA4_ADDR                          (918 + EFR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_F_GAMMA3_ADDR                          (928 + EFR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_INTER_6_ADDR                           (938 + EFR_TABLE_START_ADDR) //[62]shorts
#define STATIC_CONST_INTER_6_25_ADDR                        (1000 + EFR_TABLE_START_ADDR) //[26]shorts
#define STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR              (1026 + EFR_TABLE_START_ADDR) //[96]shorts
#define STATIC_CONST_CDOWN_ADDR                             (1122 + EFR_TABLE_START_ADDR) //[8]shorts
#define STATIC_CONST_PDOWN_ADDR                             (1130 + EFR_TABLE_START_ADDR) //[8]shorts
#define STATIC_CONST_DGRAY_ADDR                             (1138 + EFR_TABLE_START_ADDR) //[8]shorts
#define STATIC_CONST_F_GAMMA2_ADDR                          (1146 + EFR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_F_GAMMA1_ADDR                          (1156 + EFR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_GRAY_ADDR                              (1166 + EFR_TABLE_START_ADDR) //[8]shorts
#define TABLE_DICO1_LSF_ADDR                                (1174 + EFR_TABLE_START_ADDR) //[512]shorts
#define TABLE_DICO2_LSF_ADDR                                (1686 + EFR_TABLE_START_ADDR) //[1024]shorts
#define TABLE_DICO3_LSF_ADDR                                (2710 + EFR_TABLE_START_ADDR) //[1024]shorts
#define TABLE_DICO4_LSF_ADDR                                (3734 + EFR_TABLE_START_ADDR) //[1024]shorts
#define TABLE_DICO5_LSF_ADDR                                (4758 + EFR_TABLE_START_ADDR) //[256]shorts
#define STATIC_CONST_BITNO_ADDR                             (5014 + EFR_TABLE_START_ADDR) //[30]shorts

/****************************
 **  AMR_TABLE_START_ADDR  **
 ****************************/

#define AMR_TABLE_START_ADDR        (5076 + ROM_Z_BEGIN_ADDR)

#define STATIC_CONST_PRMNO_ADDR                             (0 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_PRMNOFSF_ADDR                          (10 + AMR_TABLE_START_ADDR) //[8]shorts
#define STATIC_CONST_bitno_MR475_ADDR                       (18 + AMR_TABLE_START_ADDR) //[9]shorts
#define STATIC_CONST_bitno_MR515_ADDR                       (27 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_bitno_MR59_ADDR                        (37 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_bitno_MR67_ADDR                        (47 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_bitno_MR74_ADDR                        (57 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_bitno_MR795_ADDR                       (67 + AMR_TABLE_START_ADDR) //[12]shorts
#define STATIC_CONST_bitno_MR102_ADDR                       (79 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_bitno_MRDTX_ADDR                       (99 + AMR_TABLE_START_ADDR) //[3]shorts
#define STATIC_CONST_BITNO_AMR_ADDR                         (102 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_startPos1_ADDR                         (112 + AMR_TABLE_START_ADDR) //[2]shorts
#define STATIC_CONST_startPos2_ADDR                         (114 + AMR_TABLE_START_ADDR) //[4]shorts
#define STATIC_CONST_startPos_ADDR                          (118 + AMR_TABLE_START_ADDR) //[16]shorts
#define STATIC_CONST_corrweight_ADDR                        (134 + AMR_TABLE_START_ADDR) //[128]shorts
#define STATIC_CONST_dhf_MR475_ADDR                         (262 + AMR_TABLE_START_ADDR) //[18]shorts
#define STATIC_CONST_dhf_MR515_ADDR                         (280 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_dhf_MR59_ADDR                          (300 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_dhf_MR67_ADDR                          (320 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_dhf_MR74_ADDR                          (340 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_dhf_MR795_ADDR                         (360 + AMR_TABLE_START_ADDR) //[24]shorts
#define STATIC_CONST_dhf_MR102_ADDR                         (384 + AMR_TABLE_START_ADDR) //[40]shorts
#define STATIC_CONST_dhf_MR122_ADDR                         (424 + AMR_TABLE_START_ADDR) //[58]shorts
#define STATIC_CONST_dhf_amr_ADDR                           (482 + AMR_TABLE_START_ADDR) //[8]shorts
#define STATIC_CONST_qua_gain_code_ADDR                     (490 + AMR_TABLE_START_ADDR) //[96]shorts
#define STATIC_CONST_lsp_init_data_ADDR                     (586 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_ph_imp_low_MR795_ADDR                  (596 + AMR_TABLE_START_ADDR) //[40]shorts
#define STATIC_CONST_ph_imp_low_ADDR                        (636 + AMR_TABLE_START_ADDR) //[40]shorts
#define STATIC_CONST_ph_imp_mid_ADDR                        (676 + AMR_TABLE_START_ADDR) //[40]shorts
#define STATIC_past_rq_init_ADDR                            (716 + AMR_TABLE_START_ADDR) //[80]shorts
#define STATIC_MEAN_LSF_3_ADDR                              (796 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_pred_fac_ADDR                                (806 + AMR_TABLE_START_ADDR) //[10]shorts
#define TABLE_DICO1_LSF_3_ADDR                              (816 + AMR_TABLE_START_ADDR) //[768]shorts
#define TABLE_DICO2_LSF_3_ADDR                              (1584 + AMR_TABLE_START_ADDR) //[1536]shorts
#define TABLE_DICO3_LSF_3_ADDR                              (3120 + AMR_TABLE_START_ADDR) //[2048]shorts
#define STATIC_mr515_3_lsf_ADDR                             (5168 + AMR_TABLE_START_ADDR) //[512]shorts
#define STATIC_mr795_1_lsf_ADDR                             (5680 + AMR_TABLE_START_ADDR) //[1536]shorts
#define STATIC_CONST_table_gain_MR475_ADDR                  (7216 + AMR_TABLE_START_ADDR) //[1024]shorts
#define STATIC_CONST_table_gain_highrates_ADDR              (8240 + AMR_TABLE_START_ADDR) //[512]shorts
#define STATIC_CONST_table_gain_lowrates_ADDR               (8752 + AMR_TABLE_START_ADDR) //[256]shorts
#define TABLE_SQRT_L_ADDR                                   (9008 + AMR_TABLE_START_ADDR) //[50]shorts
#define STATIC_CONST_WIND_200_40_ADDR                       (9058 + AMR_TABLE_START_ADDR) //[240]shorts
#define STATIC_trackTable_ADDR                              (9298 + AMR_TABLE_START_ADDR) //[20]shorts
#define STATIC_CONST_gamma1_ADDR                            (9318 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_lsf_hist_mean_scale_ADDR               (9328 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_dtx_log_en_adjust_ADDR                 (9338 + AMR_TABLE_START_ADDR) //[10]shorts
#define STATIC_CONST_pred_ADDR                              (9348 + AMR_TABLE_START_ADDR) //[4]shorts
#define STATIC_CONST_pred_MR122_ADDR                        (9352 + AMR_TABLE_START_ADDR) //[4]shorts
#define STATIC_CONST_b_60Hz_ADDR                            (9356 + AMR_TABLE_START_ADDR) //[4]shorts
#define STATIC_CONST_a_60Hz_ADDR                            (9360 + AMR_TABLE_START_ADDR) //[4]shorts
#define STATIC_CONST_gamma3_ADDR                            (9364 + AMR_TABLE_START_ADDR) //[10]shorts
#define AMR_RESET_VALUE_TABLE_ADDR                          (9374 + AMR_TABLE_START_ADDR) //[48]shorts
#define AMR_ENC_RESET_TABLE_ADDR                            (9422 + AMR_TABLE_START_ADDR) //[72]shorts
#define AMR_DEC_RESET0_TABLE_ADDR                           (9494 + AMR_TABLE_START_ADDR) //[38]shorts
#define AMR_DEC_RESET_TABLE_ADDR                            (9532 + AMR_TABLE_START_ADDR) //[82]shorts

/***************************
 **  HR_TABLE_START_ADDR  **
 ***************************/

#define HR_TABLE_START_ADDR     (14690 + ROM_Z_BEGIN_ADDR)

#define TABLE_DTX_psrCNNewFactor_ADDR                       (0 + HR_TABLE_START_ADDR) //[12]shorts
#define TABLE_DTX_ppLr_gsTable_ADDR                         (12 + HR_TABLE_START_ADDR) //[256]shorts
#define TABLE_ERR_psrR0RepeatThreshold_ADDR                 (268 + HR_TABLE_START_ADDR) //[32]shorts
#define TABLE_ERR_psrConceal_ADDR                           (300 + HR_TABLE_START_ADDR) //[16]shorts
#define TABLE_DE_A_SST_psrSST_ADDR                          (316 + HR_TABLE_START_ADDR) //[12]shorts
#define TABLE_ERR_psrLevelMaxIncrease_ADDR                  (328 + HR_TABLE_START_ADDR) //[16]shorts
#define TABLE_HOMING_n_ADDR                                 (344 + HR_TABLE_START_ADDR) //[18]shorts
#define TABLE_HOMING_dhf_mask_ADDR                          (362 + HR_TABLE_START_ADDR) //[18]shorts
#define TABLE_DE_SP_psrSPFDenomWidenCf_ADDR                 (380 + HR_TABLE_START_ADDR) //[10]shorts
#define TABLE_SP_FRM_pswPCoefE_ADDR                         (390 + HR_TABLE_START_ADDR) //[4]shorts
#define TABLE_SP_FRM_psrSubMultFactor_ADDR                  (394 + HR_TABLE_START_ADDR) //[6]shorts
#define TABLE_SP_ROM_pppsrGsp0_ADDR                         (400 + HR_TABLE_START_ADDR) //[640]shorts
#define TABLE_SP_ROM_pppsrUvCodeVec_ADDR                    (1040 + HR_TABLE_START_ADDR) //[560]shorts
#define TABLE_SP_ROM_pppsrVcdCodeVec_ADDR                   (1600 + HR_TABLE_START_ADDR) //[360]shorts
#define TABLE_SP_ROM_psrQuant1_ADDR                         (1960 + HR_TABLE_START_ADDR) //[3072]shorts
#define TABLE_SP_ROM_psrQuant2_ADDR                         (5032 + HR_TABLE_START_ADDR) //[768]shorts
#define TABLE_SP_ROM_psrQuant3_ADDR                         (5800 + HR_TABLE_START_ADDR) //[512]shorts
#define TABLE_SP_ROM_psrPreQ1_ADDR                          (6312 + HR_TABLE_START_ADDR) //[96]shorts
#define TABLE_SP_ROM_psrPreQ2_ADDR                          (6408 + HR_TABLE_START_ADDR) //[48]shorts
#define TABLE_SP_ROM_psrPreQ3_ADDR                          (6456 + HR_TABLE_START_ADDR) //[32]shorts
#define TABLE_SP_ROM_psrQuantSz_ADDR                        (6488 + HR_TABLE_START_ADDR) //[4]shorts
#define TABLE_SP_ROM_psrPreQSz_ADDR                         (6492 + HR_TABLE_START_ADDR) //[4]shorts
#define TABLE_SP_ROM_psrSQuant_ADDR                         (6496 + HR_TABLE_START_ADDR) //[256]shorts
#define TABLE_SP_ROM_psvqIndex_ADDR                         (6752 + HR_TABLE_START_ADDR) //[12]shorts
#define TABLE_SP_ROM_ppsrSqrtP0_ADDR                        (6764 + HR_TABLE_START_ADDR) //[96]shorts
#define TABLE_SP_ROM_ppsrCGIntFilt_ADDR                     (6860 + HR_TABLE_START_ADDR) //[36]shorts
#define TABLE_SP_ROM_ppsrPVecIntFilt_ADDR                   (6896 + HR_TABLE_START_ADDR) //[60]shorts
#define TABLE_SP_ROM_psrLagTbl_ADDR                         (6956 + HR_TABLE_START_ADDR) //[256]shorts
#define TABLE_SP_ROM_psrR0DecTbl_ADDR                       (7212 + HR_TABLE_START_ADDR) //[64]shorts
#define TABLE_SP_ROM_psrHPFCoefs_ADDR                       (7276 + HR_TABLE_START_ADDR) //[10]shorts
#define TABLE_SP_ROM_psrNWCoefs_ADDR                        (7286 + HR_TABLE_START_ADDR) //[20]shorts
#define TABLE_SP_ROM_pL_rFlatSstCoefs_ADDR                  (7306 + HR_TABLE_START_ADDR) //[20]shorts
#define TABLE_SP_ROM_psrOldCont_ADDR                        (7326 + HR_TABLE_START_ADDR) //[4]shorts
#define TABLE_SP_ROM_psrNewCont_ADDR                        (7330 + HR_TABLE_START_ADDR) //[4]shorts
#define TABLE_SFRM_pswUpdateIndexV_pswBitIndexV_ADDR            (7334 + HR_TABLE_START_ADDR) //[256]shorts
#define TABLE_SFRM_pswUpdateIndexUn_pswBitIndexUn_ADDR          (7590 + HR_TABLE_START_ADDR) //[64]shorts
#define TABLE_SFRM_pswModNextBitV_ADDR                      (7654 + HR_TABLE_START_ADDR) //[10]shorts
#define TABLE_SFRM_pswModNextBitUV_ADDR                     (7664 + HR_TABLE_START_ADDR) //[8]shorts
#define TABLE_HOMING_n1_ADDR                                (7672 + HR_TABLE_START_ADDR) //[18]shorts
#define hr_reset_value_ADDR                                 (7690 + HR_TABLE_START_ADDR) //[10]shorts
#define EN_RESET_ZERO_ADDR                                  (7700 + HR_TABLE_START_ADDR) //[60]shorts
#define EN_RESET_VALUE_ADDR                                 (7760 + HR_TABLE_START_ADDR) //[24]shorts
#define DE_RESET_ZERO_ADDR                                  (7784 + HR_TABLE_START_ADDR) //[24]shorts
#define DE_RESET_VALUE_ADDR                                 (7808 + HR_TABLE_START_ADDR) //[26]shorts

/*************
 **  alias  **
 *************/

#define STATIC_CONST_gray_ADDR                              STATIC_CONST_GRAY_ADDR
#define STATIC_CONST_gamma2_ADDR                            STATIC_CONST_F_GAMMA2_ADDR
#define STATIC_CONST_gamma1_12K2_ADDR                       STATIC_CONST_F_GAMMA1_ADDR
#define CONST_0x40000000L_ADDR                              CONST_0x40000000_ADDR
#define CONST_ALPHA5_ADDR                                   CONST_0x3fff_ADDR
#define CONST_1_16_ADDR                                     CONST_D_1_16_ADDR
#define CONST_0x7fffffff_ADDR                               CONST_0x7FFFFFFF_ADDR
#define CONST_1_4_ADDR                                      CONST_D_1_4_ADDR
#define CONST_0x0021_ADDR                                   TABLE_SP_ROM_pppsrGsp0_ADDR
#define TABLE_SP_ROM_psvqIndex_ADDR_MOIN_4_ADDR             ( -4 + TABLE_SP_ROM_psvqIndex_ADDR)
#define CONST_112_ADDR                                      (     40+STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR   )
#define CONST_105_ADDR                                      (     33+STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR   )
#define CONST_197_ADDR                                      (     65+TABLE_SP_ROM_psrLagTbl_ADDR              )
#define CONST_15565_ADDR                                    (     10+ STATIC_CONST_QUA_GAIN_PITCH_ADDR        )
#define CONST_31128_ADDR                                    (      257 + TABLE_SP_ROM_psrQuant3_ADDR          )
#define CONST_134_ADDR                                      (      4+ TABLE_SP_ROM_psrLagTbl_ADDR             )
#define CONST_15565_ADDR                                    (      10+ STATIC_CONST_QUA_GAIN_PITCH_ADDR       )
#define CONST_13926_ADDR                                    (       8+ STATIC_CONST_QUA_GAIN_PITCH_ADDR       )
#define CONST_19_ADDR                                       (      16+ TABLE_SP_ROM_pppsrUvCodeVec_ADDR       )
#define CONST_neg3_ADDR                                     (      29+  TABLE_SP_ROM_pppsrGsp0_ADDR           )
#define CONST_288_ADDR                                      (      117+ TABLE_SP_ROM_psrLagTbl_ADDR           )
#define CONST_144_ADDR                                      (       12+ TABLE_SP_ROM_psrLagTbl_ADDR           )
#define CONST_260_ADDR                                      (      103+ TABLE_SP_ROM_psrLagTbl_ADDR           )
#define CONST_0x177_ADDR                                    (        148+ TABLE_SP_ROM_psrLagTbl_ADDR         )
#define CONST_0x55_ADDR                                     (     10+ TABLE_SP_ROM_pppsrGsp0_ADDR         )
#define CONST_200_ADDR                                      (     68+    TABLE_SP_ROM_psrLagTbl_ADDR          )
#define CONST_48_ADDR                                       (    2+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR   )
#define CONST_102_ADDR                                      (    30+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR  )
#define CONST_3277_ADDR                                     (     1+ STATIC_CONST_QUA_GAIN_PITCH_ADDR         )
#define CONST_0x87_ADDR                                     (     33+   STATIC_CONST_INTER_6_ADDR             )
#define CONST_0x201_ADDR                                    (    69+  TABLE_SP_ROM_psrLagTbl_ADDR             )
#define CONST_NEG11_ADDR                                    (     58+ TABLE_SP_ROM_pppsrUvCodeVec_ADDR        )
#define CONST_31821_ADDR                                    (    731+ TABLE_SP_ROM_psrQuant1_ADDR             )
#define CONST_29491_ADDR                                    (    0+ STATIC_CONST_F_GAMMA1_ADDR                )
#define CONST_9830_ADDR                                     (    4+ STATIC_CONST_QUA_GAIN_PITCH_ADDR          )
#define CONST_14746_ADDR                                    (   9+  STATIC_CONST_QUA_GAIN_PITCH_ADDR          )
#define CONST_17578_ADDR                                    (   256+   TABLE_SP_ROM_psrQuant1_ADDR            )
#define CONST_0xa80_ADDR                                    (391+   TABLE_SP_ROM_pppsrGsp0_ADDR               )
#define CONST_16384_ADDR                                    (11+  STATIC_CONST_QUA_GAIN_PITCH_ADDR)
#define CONST_NEG31_ADDR                                    (41+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_210_ADDR                                      (78+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_248_ADDR                                      (97+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_264_ADDR                                      (105+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_256_ADDR                                      (101+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_neg45_ADDR                                    (322+  TABLE_SP_ROM_pppsrGsp0_ADDR)
#define CONST_neg72_ADDR                                    (243+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_142_ADDR                                      (10+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_147_ADDR                                      (15+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_160_ADDR                                      (28+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_143_ADDR                                      (11+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_184_ADDR                                      (52+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_396_ADDR                                      (155+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_26214_ADDR                                    (3+    STATIC_CONST_PDOWN_ADDR)
#define CONST_fu10_ADDR                                     (510+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_fu16_ADDR                                     (347+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_4096_ADDR                                     (56+  TABLE_SP_ROM_psrR0DecTbl_ADDR)
#define CONST_64_ADDR                                       (18+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR)
#define CONST_128_ADDR                                      (1+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_720_ADDR                                      (233+  TABLE_SP_ROM_psrLagTbl_ADDR)
#define CONST_205_ADDR                                      (80+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR)
#define CONST_66_ADDR                                       (20+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR)
#define CONST_50_ADDR                                       (4+  STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR)
#define CONST_44_ADDR                                       (178+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_0_ADDR                                        (0+TABLE_ERR_psrLevelMaxIncrease_ADDR )
#define CONST_0x00010001_ADDR                               ( 2+TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_2_ADDR                                        (4+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_3_ADDR                                        (5+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_4_ADDR                                        (6+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_6_ADDR                                        (8+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_7_ADDR                                        (9+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_8_ADDR                                        (10+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_9_ADDR                                        (11+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_10_ADDR                                       (12+   TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_12_ADDR                                       (13+  TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_14_ADDR                                       (14+  TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_16_ADDR                                       (15+  TABLE_ERR_psrLevelMaxIncrease_ADDR)
#define CONST_17_ADDR                                       (12+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_18_ADDR                                       (366+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_20_ADDR                                       (103+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_21_ADDR                                       (36+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_22_ADDR                                       (45+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_23_ADDR                                       (30+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_24_ADDR                                       (39+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_25_ADDR                                       (35+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_26_ADDR                                       (100+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_27_ADDR                                       (102+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_28_ADDR                                       (121+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_29_ADDR                                       (171+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_30_ADDR                                       (14+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_31_ADDR                                       (536+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_32_ADDR                                       (392+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_33_ADDR                                       (348+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_35_ADDR                                       (188+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_36_ADDR                                       (140+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_39_ADDR                                       (108+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_40_ADDR                                       (32+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_41_ADDR                                       (434+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_72_ADDR                                       (480+  TABLE_SP_ROM_pppsrUvCodeVec_ADDR)
#define CONST_15_ADDR                                       (0+  TABLE_ERR_psrR0RepeatThreshold_ADDR)
#define CONST_0x00020002_ADDR                               (26+  TABLE_ERR_psrR0RepeatThreshold_ADDR)
#define CONST_0x00080008_ADDR                               (14+  TABLE_ERR_psrR0RepeatThreshold_ADDR)
#define CONST_11_ADDR                                       (7+   TABLE_ERR_psrR0RepeatThreshold_ADDR)
#define CONST_13_ADDR                                       (6+  TABLE_SP_ROM_psrR0DecTbl_ADDR)
#define CONST_0xFFFFFFFF_ADDR                               (18+  TABLE_SP_ROM_pppsrGsp0_ADDR)
#define CONST_3276_ADDR                                     (2982+  TABLE_SP_ROM_psrQuant1_ADDR)
#define CONST_0x1555_ADDR                                   (3+  STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_0x2000_ADDR                                   (5+  STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_0x5000_ADDR                                   (14+  STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_0x6000_ADDR                                   (17+  STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_0x7000_ADDR                                   (20+  STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_0x2666_ADDR                                   (0+  TABLE_SP_ROM_psrNewCont_ADDR)
#define CONST_0x3333_ADDR                                   (7+  STATIC_CONST_QUA_GAIN_PITCH_ADDR)
#define CONST_0x7fc5_ADDR                                   (225+  TABLE_WINDOW_232_8_ADDR)
#define CONST_neg2_ADDR                                     (24+  TABLE_SP_ROM_pppsrGsp0_ADDR)
#define CONST_1000_ADDR                                     (344+TABLE_DICO1_LSF_ADDR)
#define CONST_80_ADDR                                       (316+TABLE_DICO1_LSF_ADDR)
#define CONST_512_ADDR                                      (252+TABLE_DICO1_LSF_ADDR)
#define CONST_1024_ADDR                                     (603+  TABLE_DICO1_LSF_3_ADDR)
#define CONST_1464_ADDR                                     (0x649 + TABLE_DICO3_LSF_3_ADDR)
#define CONST_0x199a_ADDR                                   (335+ STATIC_CONST_table_gain_MR475_ADDR)
#define CONST_0x5a82_ADDR                                   (16+ TABLE_LSP_LSF_ADDR)
#define CONST_0x00ff_ADDR                                   (484+ TABLE_DICO3_LSF_ADDR)
#define CONST_0x7ff_ADDR                                    (0x514 + TABLE_DICO2_LSF_3_ADDR)
#define CONST_1260_ADDR                                     (0x44c + TABLE_DICO2_LSF_3_ADDR)
#define CONST_572_ADDR                                      (173+ TABLE_DICO5_LSF_ADDR )
#define CONST_neg0x6676_ADDR                                (619+TABLE_SP_ROM_psrQuant2_ADDR)
#define CONST_500_ADDR                                      (356 + TABLE_DICO1_LSF_ADDR)
#define CONST_1152_ADDR                                     (620+  TABLE_DICO2_LSF_ADDR)
#define CONST_0xfff8_ADDR                                   (104 + TABLE_DICO1_LSF_ADDR)
#define CONST_576_ADDR                                      (1074+ TABLE_DICO3_LSF_3_ADDR)
#define CONST_120_ADDR                                      (440+ TABLE_DICO2_LSF_ADDR)
#define CONST_575_ADDR                                      (460+ TABLE_DICO2_LSF_ADDR)
#define CONST_ALPHA4_ADDR                                   (0xba6+ TABLE_SP_ROM_psrQuant1_ADDR)
#define CONST_ALPHA_UP2_ADDR                                (540 +   TABLE_DICO2_LSF_ADDR)
#define CONST_24576_ADDR                                    (17+STATIC_CONST_INTERP_FACTOR_ADDR)
#define CONST_124_ADDR                                      (476+  TABLE_DICO1_LSF_ADDR)
#define CONST_1311_ADDR                                     (814+  TABLE_DICO2_LSF_ADDR)
#define CONST_368_ADDR                                      (222+  TABLE_DICO1_LSF_ADDR)
#define CONST_463_ADDR                                      (830+  TABLE_DICO2_LSF_3_ADDR)
#define CONST_0x4CCC_ADDR                                   (148+  STATIC_CONST_table_gain_lowrates_ADDR)
#define CONST_0x5999_ADDR                                   (6+  STATIC_CONST_CDOWN_ADDR)
#define CONST_0xa00_ADDR                                    (923+  STATIC_mr795_1_lsf_ADDR)
#define CONST_9000_ADDR                                     (61+  STATIC_CONST_table_gain_highrates_ADDR)
#define CONST_0x235_ADDR                                    (247+  TABLE_DICO1_LSF_ADDR)
#define CONST_0x101_ADDR                                    (877+  TABLE_DICO4_LSF_ADDR)
#define CONST_97_ADDR                                       (394+  TABLE_DICO2_LSF_ADDR)
#define CONST_400_ADDR                                      (648+  TABLE_DICO4_LSF_ADDR)
#define CONST_5325_ADDR                                     (693+  STATIC_CONST_table_gain_MR475_ADDR)
#define CONST_1_ADDR                                        (24+STATIC_CONST_DHF_MASK_ADDR  )
#define CONST_0x2L_ADDR                                     (218+TABLE_SP_ROM_pppsrVcdCodeVec_ADDR  )
#define CONST_0x00004000L_ADDR                              (48+TABLE_INV_SQRT_ADDR  )
#define CONST_0x00007FFF_ADDR                               (32+TABLE_LOG2_ADDR  )
#define CONST_0x00008000_ADDR                               (64+TABLE_LSP_LSF_ADDR )
#define CONST_0x0000FFFF_ADDR                               (4+STATIC_trackTable_ADDR )
#define CONST_0x00000005L_ADDR                              (4+TABLE_SFRM_pswModNextBitUV_ADDR )
#define CONST_0xffff0000_ADDR                               (12+STATIC_trackTable_ADDR )
#define CONST_RX_LOST_SID_FRAME_ADDR                        CONST_16_ADDR
#define CONST_RX_INVALID_SID_FRAME_ADDR                     CONST_32_ADDR
#define CONST_RX_NO_TRANSMISSION_ADDR                       CONST_64_ADDR
#define CONST_RX_DTX_MUTING_ADDR                            CONST_128_ADDR
#define CONST_FU2_ADDR                                      CONST_neg2_ADDR
#define CONST_WORD32_0_ADDR                                 CONST_0_ADDR
#define CONST_0x00000001_ADDR                               CONST_1_ADDR
#define TABLE_hufftab0_ADDR                                 CONST_0x00000001_ADDR
#define CONST_0x7FFF_ADDR                                   CONST_0x00007FFF_ADDR
#define CONST_0xFFFF_ADDR                                   CONST_0x0000FFFF_ADDR
#define CONST_0x8000_ADDR                                   CONST_0x00008000_ADDR
#define CONST_0x4000_ADDR                                   CONST_0x00004000L_ADDR
#define CONST_5_ADDR                                        CONST_0x00000005L_ADDR
#define CONST_0X80008_ADDR                                  CONST_0x00080008_ADDR
#define CONST_8192_ADDR_Y                                   CONST_0x2000_ADDR
#define CONST_0x00008000L_ADDR                              CONST_0x00008000_ADDR
#define CONST_0x0010_ADDR                                   CONST_16_ADDR
#define CONST_0x0011_ADDR                                   CONST_17_ADDR
#define CONST_0x0020_ADDR                                   CONST_32_ADDR
#define CONST_00A0_ADDR                                     CONST_160_ADDR
#define CONST_0x7fff_ADDR                                   CONST_0x7FFF_ADDR
#define CONST_0x400_ADDR                                    CONST_1024_ADDR
#define CONST_0x80008_ADDR                                  CONST_0x00080008_ADDR
#define CONST_6554_ADDR                                     CONST_0x199a_ADDR
#define COSNT_1_2_ADDR                                      CONST_0x00004000L_ADDR
#define CONST_0x00007fffL_ADDR                              CONST_0x00007FFF_ADDR
#define CONST_0x0000ffffL_ADDR                              CONST_0x0000FFFF_ADDR
#define CONST_neg1_ADDR                                     CONST_0xFFFFFFFF_ADDR

//CONST16_START_ADDR                        RAM_X:        NULL      RAM_Y:       NULL       ROM_Z: 0x0000 -> 0x0007
//CONST32_START_ADDR                        RAM_X:        NULL      RAM_Y:       NULL       ROM_Z: 0x0008 -> 0x001f
//EFR_TABLE_START_ADDR                      RAM_X:        NULL      RAM_Y:       NULL       ROM_Z: 0x0020 -> 0x13d3
//AMR_TABLE_START_ADDR                      RAM_X:        NULL      RAM_Y:       NULL       ROM_Z: 0x13d4 -> 0x3961
//HR_TABLE_START_ADDR                       RAM_X:        NULL      RAM_Y:       NULL       ROM_Z: 0x3962 -> 0x57fb

//RAM_X: size 0x1000, used 0x0000   RAM_Y: size 0x0800, used 0x0000 ROM_Z: size 0x5800, used 0x57fc
