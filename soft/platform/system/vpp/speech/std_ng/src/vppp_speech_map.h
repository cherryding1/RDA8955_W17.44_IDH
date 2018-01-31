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

#ifndef MAP_EXPORT_H
#define MAP_EXPORT_H

/* This file defines the addresses of exported variables.
   It's created by voc_map automatically.*/

#define VPP_SPEECH_DEC_IN_STRUCT                    (0 + RAM_X_BEGIN_ADDR)
#define VPP_SPEECH_ENC_OUT_STRUCT                   (26 + RAM_X_BEGIN_ADDR)
#define VPP_SPEECH_ENC_IN_STRUCT                    (48 + RAM_X_BEGIN_ADDR)
#define VPP_SPEECH_DEC_OUT_STRUCT                   (368 + RAM_X_BEGIN_ADDR)
#define GLOBAL_NOTCH_FILTER_COEF_ADDR               (7000 + RAM_X_BEGIN_ADDR)
#define VPP_SPEECH_INIT_STRUCT                      (0 + RAM_Y_BEGIN_ADDR)
#define VPP_SPEECH_AUDIO_CFG_STRUCT                 (56 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_SUPPRESS_DEFAULT_TX             (220 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_SUPPRESS_DEFAULT_RX             (221 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_TX_RANGE1              (222 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_TX_RANGE2              (223 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_TX_RANGE3              (224 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_TX_RANGE4              (225 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_RX_RANGE1              (226 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_RX_RANGE2              (227 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_RX_RANGE3              (228 + RAM_Y_BEGIN_ADDR)
#define SPEEX_NOISE_PROB_MIN_RX_RANGE4              (229 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MSDF_RELOAD_FLAG                     (231 + RAM_Y_BEGIN_ADDR)
#define MIC_NONCLIP_ENABLE_FLAG_ADDR                (236 + RAM_Y_BEGIN_ADDR)
#define RECEIVER_NONCLIP_ENABLE_FLAG_ADDR           (237 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_TX         (238 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NON_CLIP_NOISE_THRESHOLD_TX          (239 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_RX         (240 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NON_CLIP_NOISE_THRESHOLD_RX          (241 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_TX_BEFORE_ENC_ADDR                   (244 + RAM_Y_BEGIN_ADDR)
#define NONCLIP_ENABLE_FLAG_ADDR                    (346 + RAM_Y_BEGIN_ADDR)
#define SCALE_MIC_ENABLE_FLAG_ADDR                  (348 + RAM_Y_BEGIN_ADDR)
#define SCALE_RECEIVER_ENABLE_FLAG_ADDR             (349 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_SCALE_IN_ADDR                    (350 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVER_SCALE_OUT_ADDR              (351 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_HIGH_PASS_FILTER3_NUM1_ADDR          (360 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_HIGH_PASS_FILTER3_NUM2_ADDR          (361 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_HIGH_PASS_FILTER3_DEN1_ADDR          (362 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_HIGH_PASS_FILTER3_DEN2_ADDR          (363 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_DIGITAL_GAIN_ADDR                (386 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR            (387 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR           (388 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR       (389 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_OUTPUTENERGY_ADDR                    (390 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_INPUTENERGY_ADDR                     (392 + RAM_Y_BEGIN_ADDR)
#define m_IIRProcPara_ADDR                          (394 + RAM_Y_BEGIN_ADDR)
#define IIR_ENABLE_FLAG_ADDR                        (436 + RAM_Y_BEGIN_ADDR)
#define m_AECExtraPara_ADDR                         (438 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AGC_CODE_ADDR                        (508 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AGC_CONSTX_ADDR                      (510 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_AGC_RAMX_BACKUP_ADDR             (512 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVER_AGC_RAMX_BACKUP_ADDR        (514 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MICAGC_RELOAD                        (516 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVERAGC_RELOAD                   (517 + RAM_Y_BEGIN_ADDR)
#define STRUCT_MicAgcConfig_addr                    (518 + RAM_Y_BEGIN_ADDR)
#define STRUCT_ReceiverAgcConfig_addr               (530 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MICFIR_ENABLE_FLAG_ADDR              (542 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVERFIR_ENABLE_FLAG_ADDR         (543 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR (552 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR    (553 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_SPEECH_RAMX_BACKUP_ADDR              (554 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_SPEECH_CODE_BACKUP_ADDR              (556 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AEC_RAMX_BACKUP_ADDR                 (558 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AEC_CODE_ADDR                        (560 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AEC_CONSTX_ADDR                      (562 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RX_ADDR                              (564 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_TX_ADDR                              (566 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NOISESUPPRESS_CODE_ADDR              (568 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_NOISESUPPRESS_CONSTX_ADDR            (570 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR   (572 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR  (574 + RAM_Y_BEGIN_ADDR)
#define m_AECProcPara_ADDR                          (576 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_SPEECH_RECORD_FLAG                   (627 + RAM_Y_BEGIN_ADDR)
#define MORPHVOICE_ENABLE_FLAG_ADDR                 (630 + RAM_Y_BEGIN_ADDR)
#define PITCH_SHIFT_ADDR                            (631 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MORPH_CODE_ADDR                      (632 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MORPH_CONSTX_ADDR                    (634 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG       (8940 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_EXTERN_PROCESS_MIC_ENABLE_FLAG       (8941 + RAM_Y_BEGIN_ADDR)
#define HIGH_PASS_FILTER_ENABLE_FLAG_ADDR           (8942 + RAM_Y_BEGIN_ADDR)
#define NOTCH_FILTER_ENABLE_FLAG_ADDR               (8943 + RAM_Y_BEGIN_ADDR)
#define MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR       (8944 + RAM_Y_BEGIN_ADDR)
#define RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR  (8945 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MORPH_RAMX_BACKUP_ADDR               (8948 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR            (8954 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR        (8955 + RAM_Y_BEGIN_ADDR)

#endif
