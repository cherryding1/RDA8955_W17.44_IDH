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

#include "vppp_amjr_asm_map.h"
#include "vppp_amjr_asm_sections.h"
#include "voc2_library.h"
#include "voc2_define.h"


#define AMR_ENC_ConstX_size (288/2)
#define AMR_ENC_ConstY_size (4988/2)


#define MAX_32                                               0x7fffffffL
#define MIN_32                                               0x80000000L
#define MAX_16                                               0x7fff
#define MIN_16                                               0x8000
#define _1_2                                                 (32768L/2)
#define _1_4                                                 (32768L/4)
#define _1_8                                                 (32768L/8)
#define _1_16                                                (32768L/16)
#define _1_32                                                (32768L/32)
#define _1_64                                                (32768L/64)
#define _1_128                                               (32768L/128)
#define PIT_MIN                                               20       /* Minimum pitch lag (all other modes)      */
#define PIT_MAX                                              143       /* Maximum pitch lag                        */
#define MAX_PRM_SIZE                                          57       /* max. num. of params                      */
#define MAX_SERIAL_SIZE                                      244       /* max. num. of serial bits                 */
#define L_WINDOW                                             240       /* Window size in LP analysis               */
#define L_FRAME                                              160       /* Frame size                               */
#define L_FRAME_BY2                                           80       /* Frame size divided by 2                  */
#define L_SUBFR                                               40       /* Subframe size                            */
#define L_CODE                                                40       /* codevector length                        */
#define NB_TRACK                                               5       /* number of tracks                         */
#define STEP                                                   5       /* codebook step size                       */
#define NB_TRACK_MR102                                         4       /* number of tracks mode mr102              */
#define STEP_MR102                                             4       /* codebook step size mode mr102            */
#define M                                                     10       /* Order of LP filter                       */
#define MP1                                                 (M+1)      /* Order of LP filter + 1                   */
#define LSF_GAP                                              205       /* Minimum distance between LSF after quan-tization; 50 Hz = 205                    */
#define NC                                                   M/2
#define PITCH_OL_M475_LOOP_NUM                              (L_FRAME+PIT_MAX)
#define PITCH_OL_M795_LOOP_NUM                              (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_WGH_LOOP_NUM                               (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_M122_LOOP_NUM                              (L_FRAME_BY2+PIT_MAX)
#define DICO1_SIZE                                          128
#define DICO2_SIZE                                          256
#define DICO3_SIZE                                          256
#define DICO4_SIZE                                          256
#define DICO5_SIZE                                           64


// VoC_directive: PARSER_OFF

void vpp_AmjrAmrEncode(void);
void CII_NORM_L_ACC0(void);
void CII_DIV_S(void);
void CII_encoder_homing_frame_test(void);;
void CII_Copy_M(void);
void CII_scale(void);
void CII_copy_xy(void);
void CII_SEARCH_COPY(void);
void CII_encoder_reset(void);
void CII_DIV_32(void);
void CII_Inv_sqrt(void);
void CII_Lag_window (void);
void CII_Levinson (void);
void CII_Get_lsp_pol(void);
void CII_Lsp_Az(void);
void CII_Lsf_lsp(void);
void CII_Lsp_lsf(void);
void CII_Convolve(void);
void CII_Log2(void);
void CII_Lsf_wt(void);
void CII_Residu_new (void);
void CII_subframePostProc(void);
void CII_Weight_Ai(void);
void CII_Int_lpc_only(void);
void CII_Pow2(void);
void CII_gc_pred_update(void);
void CII_Autocorr(void);
void CII_Chebps(void);
void CII_Az_lsp(void);
void CII_cbseach_subr1(void);
void CII_cor_h_x(void);
void CII_cor_h( void);
void CII_G_pitch(void);
void CII_Pre_Process(void);
void CII_subframePreProc(void);
void CII_build_code_com2(void);
void CII_Pred_lt_3or6(void);
void CII_Syn_filt(void);
void CII_Reorder_lsf (void);
void CII_Vq_subvec(void);
void CII_prm2bits_amr_efr(void);
void CII_PRM2_BITS_POST_PROCESS(void);
void CII_check_lsp(void);
void CII_RESET_AMR_CONSTANT(void);
void CII_COD_AMR_122(void);
void CII_set_sign12k2_122(void);
void CII_code_10i40_35bits(void);
void CII_build_code_M122(void);
void CII_search_M122(void);
void CII_COMPUTE_RRV_M122(void);
void CII_COMPUTE_ALP1_M122(void);
void CII_SEARCH_LOOP_M122(void);
void CII_q_gain_code (void);
void CII_lpc_M122(void);
void CII_lsp_M122(void);
void CII_Lag_max_M122(void);
void CII_Pitch_ol_M122(void);
void CII_Q_plsf_5(void);
void CII_Int_lpc_1and3(void);
void CII_gainQuant_M122(void);
void CII_lsp_122(void);


// VoC_directive: PARSER_ON
