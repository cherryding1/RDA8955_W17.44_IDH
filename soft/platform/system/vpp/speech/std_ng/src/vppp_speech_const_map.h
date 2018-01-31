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






#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)





#define HR_RAMX_CONSTANTS_ADDR                              (816 + COMMON_GLOBAL_X_BEGIN_ADDR)

#define TABLE_DTX_psrCNNewFactor_ADDR                       (160 + HR_RAMX_CONSTANTS_ADDR) //12 short
#define TABLE_DTX_ppLr_gsTable_ADDR                         (172 + HR_RAMX_CONSTANTS_ADDR) //256 short
#define TABLE_ERR_psrR0RepeatThreshold_ADDR                 (428 + HR_RAMX_CONSTANTS_ADDR) //32 short
#define TABLE_ERR_psrConceal_ADDR                           (460 + HR_RAMX_CONSTANTS_ADDR) //16 short
#define TABLE_DE_A_SST_psrSST_ADDR                          (476 + HR_RAMX_CONSTANTS_ADDR) //12 short
#define TABLE_ERR_psrLevelMaxIncrease_ADDR                  (488 + HR_RAMX_CONSTANTS_ADDR) //16 short
#define TABLE_HOMING_n_ADDR                                 (504 + HR_RAMX_CONSTANTS_ADDR) //18 short
#define TABLE_HOMING_dhf_mask_ADDR                          (522 + HR_RAMX_CONSTANTS_ADDR) //18 short
#define TABLE_DE_SP_psrSPFDenomWidenCf_ADDR                 (540 + HR_RAMX_CONSTANTS_ADDR) //10 short
#define TABLE_SP_FRM_pswPCoefE_ADDR                         (550 + HR_RAMX_CONSTANTS_ADDR) //4 short
#define TABLE_SP_FRM_psrSubMultFactor_ADDR                  (554 + HR_RAMX_CONSTANTS_ADDR) //6 short
#define TABLE_SP_ROM_pppsrGsp0_ADDR                         (560 + HR_RAMX_CONSTANTS_ADDR) //640 short
#define TABLE_SP_ROM_pppsrUvCodeVec_ADDR                    (1200 + HR_RAMX_CONSTANTS_ADDR) //560 short
#define TABLE_SP_ROM_pppsrVcdCodeVec_ADDR                   (1760 + HR_RAMX_CONSTANTS_ADDR) //360 short
#define TABLE_SP_ROM_psrR0DecTbl_ADDR                       (2120 + HR_RAMX_CONSTANTS_ADDR) //64 short
#define TABLE_SP_ROM_psrHPFCoefs_ADDR                       (2184 + HR_RAMX_CONSTANTS_ADDR) //10 short
#define TABLE_SP_ROM_psrNWCoefs_ADDR                        (2194 + HR_RAMX_CONSTANTS_ADDR) //20 short
#define TABLE_SP_ROM_pL_rFlatSstCoefs_ADDR                  (2214 + HR_RAMX_CONSTANTS_ADDR) //20 short
#define TABLE_SP_ROM_psrOldCont_ADDR                        (2234 + HR_RAMX_CONSTANTS_ADDR) //4 short
#define TABLE_SP_ROM_psrNewCont_ADDR                        (2238 + HR_RAMX_CONSTANTS_ADDR) //4 short
#define TABLE_SFRM_pswUpdateIndexV_pswBitIndexV_ADDR        (2242 + HR_RAMX_CONSTANTS_ADDR) //256 short
#define TABLE_SFRM_pswUpdateIndexUn_pswBitIndexUn_ADDR      (2498 + HR_RAMX_CONSTANTS_ADDR) //64 short
#define TABLE_SFRM_pswModNextBitV_ADDR                      (2562 + HR_RAMX_CONSTANTS_ADDR) //10 short
#define TABLE_SFRM_pswModNextBitUV_ADDR                     (2572 + HR_RAMX_CONSTANTS_ADDR) //8 short
#define TABLE_HOMING_n1_ADDR                                (2580 + HR_RAMX_CONSTANTS_ADDR) //18 short




//HR_RAMY_CONSTANTS_ADDR
#define HR_RAMY_CONSTANTS_ADDR                              (776 + COMMON_GLOBAL_Y_BEGIN_ADDR)//(792 + COMMON_GLOBAL_Y_BEGIN_ADDR)
#define TABLE_SP_ROM_psrQuant1_ADDR                         (0 + HR_RAMY_CONSTANTS_ADDR) //3072 short
#define TABLE_SP_ROM_psrQuant2_ADDR                         (3072 + HR_RAMY_CONSTANTS_ADDR) //768 short
#define TABLE_SP_ROM_psrQuant3_ADDR                         (3840 + HR_RAMY_CONSTANTS_ADDR) //512 short
#define TABLE_SP_ROM_psrPreQ1_ADDR                          (4352 + HR_RAMY_CONSTANTS_ADDR) //96 short
#define TABLE_SP_ROM_psrPreQ2_ADDR                          (4448 + HR_RAMY_CONSTANTS_ADDR) //48 short
#define TABLE_SP_ROM_psrPreQ3_ADDR                          (4496 + HR_RAMY_CONSTANTS_ADDR) //32 short
#define TABLE_SP_ROM_psrQuantSz_ADDR                        (4528 + HR_RAMY_CONSTANTS_ADDR) //4 short
#define TABLE_SP_ROM_psrPreQSz_ADDR                         (4532 + HR_RAMY_CONSTANTS_ADDR) //4 short
#define TABLE_SP_ROM_psrSQuant_ADDR                         (4536 + HR_RAMY_CONSTANTS_ADDR) //256 short
#define TABLE_SP_ROM_psvqIndex_ADDR                         (4792 + HR_RAMY_CONSTANTS_ADDR) //12 short
#define TABLE_SP_ROM_ppsrSqrtP0_ADDR                        (4804 + HR_RAMY_CONSTANTS_ADDR) //96 short
#define TABLE_SP_ROM_ppsrPVecIntFilt_ADDR                   (4900 + HR_RAMY_CONSTANTS_ADDR) //60 short
#define TABLE_SP_ROM_psrLagTbl_ADDR                         (4960 + HR_RAMY_CONSTANTS_ADDR) //256 short
#define TABLE_SP_ROM_ppsrCGIntFilt_ADDR                     (5216 + HR_RAMY_CONSTANTS_ADDR) //36 short
#define hr_reset_value_ADDR                                 (5252 + HR_RAMY_CONSTANTS_ADDR) //10 short



