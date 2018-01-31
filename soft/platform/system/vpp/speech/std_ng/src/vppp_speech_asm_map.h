#ifndef MAP_ADDR_H
#define MAP_ADDR_H

/* This file defines all the variables as memory addresses.
   It's created by voc_map automatically.*/

/********************
 **  SECTION_SIZE  **
 ********************/

#define COMMON_GLOBAL_X_SIZE                              		7016
#define COMMON_GLOBAL_Y_SIZE                              		8956
#define NOISESUPPRESS_GLOBAL_X_SIZE                       		6434
#define NOISESUPPRESS_GLOBAL_Y_SIZE                       		0
#define NOISESUPPRESS_LOCAL_X_SIZE                        		0
#define NOISESUPPRESS_LOCAL_Y_SIZE                        		0
#define MORPH_GLOBAL_X_SIZE                               		5054
#define MORPH_GLOBAL_Y_SIZE                               		0
#define MORPH_LOCAL_X_SIZE                                		0
#define MORPH_LOCAL_Y_SIZE                                		0
#define AGC_GLOBAL_X_SIZE                                 		1474
#define AGC_GLOBAL_Y_SIZE                                 		0
#define AGC_LOCAL_X_SIZE                                  		0
#define AGC_LOCAL_Y_SIZE                                  		0
#define AEC_GLOBAL_X_SIZE                                 		3550
#define AEC_GLOBAL_Y_SIZE                                 		0
#define AEC_LOCAL_X_SIZE                                  		0
#define AEC_LOCAL_Y_SIZE                                  		0
#define FR_GLOBAL_X_SIZE                                  		1102
#define FR_GLOBAL_Y_SIZE                                  		390
#define FR_LOCAL_X_SIZE                                   		790
#define FR_LOCAL_Y_SIZE                                   		468
#define HR_GLOBAL_X_SIZE                                  		2372
#define HR_GLOBAL_Y_SIZE                                  		124
#define HR_LOCAL_X_SIZE                                   		804
#define HR_LOCAL_Y_SIZE                                   		1020
#define AMR_EFR_GLOBAL_X_SIZE                             		2366
#define AMR_EFR_GLOBAL_Y_SIZE                             		230
#define AMR_EFR_LOCAL_X_SIZE                              		820
#define AMR_EFR_LOCAL_Y_SIZE                              		1022
#define COMMON_LOCAL_X_SIZE                               		0
#define COMMON_LOCAL_Y_SIZE                               		0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                        		(0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                        		(0 + RAM_Y_BEGIN_ADDR)
#define NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR                 		(0 + RAM_X_BEGIN_ADDR)
#define NOISESUPPRESS_GLOBAL_Y_BEGIN_ADDR                 		(0 + RAM_Y_BEGIN_ADDR)
#define NOISESUPPRESS_LOCAL_X_BEGIN_ADDR                  		(6434 + RAM_X_BEGIN_ADDR)
#define NOISESUPPRESS_LOCAL_Y_BEGIN_ADDR                  		(0 + RAM_Y_BEGIN_ADDR)
#define MORPH_GLOBAL_X_BEGIN_ADDR                         		(0 + RAM_X_BEGIN_ADDR)
#define MORPH_GLOBAL_Y_BEGIN_ADDR                         		(0 + RAM_Y_BEGIN_ADDR)
#define MORPH_LOCAL_X_BEGIN_ADDR                          		(5054 + RAM_X_BEGIN_ADDR)
#define MORPH_LOCAL_Y_BEGIN_ADDR                          		(0 + RAM_Y_BEGIN_ADDR)
#define AGC_GLOBAL_X_BEGIN_ADDR                           		(0 + RAM_X_BEGIN_ADDR)
#define AGC_GLOBAL_Y_BEGIN_ADDR                           		(0 + RAM_Y_BEGIN_ADDR)
#define AGC_LOCAL_X_BEGIN_ADDR                            		(1474 + RAM_X_BEGIN_ADDR)
#define AGC_LOCAL_Y_BEGIN_ADDR                            		(0 + RAM_Y_BEGIN_ADDR)
#define AEC_GLOBAL_X_BEGIN_ADDR                           		(0 + RAM_X_BEGIN_ADDR)
#define AEC_GLOBAL_Y_BEGIN_ADDR                           		(0 + RAM_Y_BEGIN_ADDR)
#define AEC_LOCAL_X_BEGIN_ADDR                            		(3550 + RAM_X_BEGIN_ADDR)
#define AEC_LOCAL_Y_BEGIN_ADDR                            		(0 + RAM_Y_BEGIN_ADDR)
#define FR_GLOBAL_X_BEGIN_ADDR                            		(7016 + RAM_X_BEGIN_ADDR)
#define FR_GLOBAL_Y_BEGIN_ADDR                            		(8956 + RAM_Y_BEGIN_ADDR)
#define FR_LOCAL_X_BEGIN_ADDR                             		(8118 + RAM_X_BEGIN_ADDR)
#define FR_LOCAL_Y_BEGIN_ADDR                             		(9346 + RAM_Y_BEGIN_ADDR)
#define HR_GLOBAL_X_BEGIN_ADDR                            		(7016 + RAM_X_BEGIN_ADDR)
#define HR_GLOBAL_Y_BEGIN_ADDR                            		(8956 + RAM_Y_BEGIN_ADDR)
#define HR_LOCAL_X_BEGIN_ADDR                             		(9388 + RAM_X_BEGIN_ADDR)
#define HR_LOCAL_Y_BEGIN_ADDR                             		(9080 + RAM_Y_BEGIN_ADDR)
#define AMR_EFR_GLOBAL_X_BEGIN_ADDR                       		(7016 + RAM_X_BEGIN_ADDR)
#define AMR_EFR_GLOBAL_Y_BEGIN_ADDR                       		(8956 + RAM_Y_BEGIN_ADDR)
#define AMR_EFR_LOCAL_X_BEGIN_ADDR                        		(9382 + RAM_X_BEGIN_ADDR)
#define AMR_EFR_LOCAL_Y_BEGIN_ADDR                        		(9186 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                         		(9388 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                         		(9346 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
//VPP_SPEECH_DEC_IN_STRUCT
#define VPP_SPEECH_DEC_IN_STRUCT                			(0 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define INPUT_BITS2_ADDR                        			(0 + VPP_SPEECH_DEC_IN_STRUCT) //26 short

//VPP_SPEECH_ENC_OUT_STRUCT
#define VPP_SPEECH_ENC_OUT_STRUCT               			(26 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define OUTPUT_BITS2_ADDR                       			(0 + VPP_SPEECH_ENC_OUT_STRUCT) //22 short

//VPP_SPEECH_ENC_IN_STRUCT
#define VPP_SPEECH_ENC_IN_STRUCT                			(48 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define INPUT_SPEECH_BUFFER2_ADDR               			(0 + VPP_SPEECH_ENC_IN_STRUCT) //160 short
#define INPUT_SPEECH_BUFFER1_ADDR               			(160 + VPP_SPEECH_ENC_IN_STRUCT) //160 short

//VPP_SPEECH_DEC_OUT_STRUCT
#define VPP_SPEECH_DEC_OUT_STRUCT               			(368 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define OUTPUT_SPEECH_BUFFER2_ADDR              			(0 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short
#define OUTPUT_SPEECH_BUFFER1_ADDR              			(160 + VPP_SPEECH_DEC_OUT_STRUCT) //160 short

#define GLOBAL_SDF_SAMPLS_ADDR                  			(688 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short
#define GLOBAL_MDF_SAMPLS_ADDR                  			(752 + COMMON_GLOBAL_X_BEGIN_ADDR) //64 short
//AMR_EFR_RAMX_CONSTANTS_ADDR
#define AMR_EFR_RAMX_CONSTANTS_ADDR             			(816 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define CONST_2_ADDR                            			(0 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_3_ADDR                            			(1 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_4_ADDR                            			(2 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_6_ADDR                            			(3 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_7_ADDR                            			(4 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_8_ADDR                            			(5 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_9_ADDR                            			(6 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_10_ADDR                           			(7 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_11_ADDR                           			(8 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_12_ADDR                           			(9 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_13_ADDR                           			(10 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_15_ADDR                           			(11 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_16_ADDR                           			(12 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_20_ADDR                           			(13 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_30_ADDR                           			(14 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_32_ADDR                           			(15 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_40_ADDR                           			(16 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_41_ADDR                           			(17 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_64_ADDR                           			(18 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_80_ADDR                           			(19 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_120_ADDR                          			(20 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_143_ADDR                          			(21 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_160_ADDR                          			(22 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_197_ADDR                          			(23 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_5325_ADDR                         			(24 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x1fff_ADDR                       			(25 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_5_ADDR                            			(26 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_15565_ADDR                        			(27 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x1555_ADDR                       			(28 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x2000_ADDR                       			(29 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_neg2_ADDR                         			(30 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x199a_ADDR                       			(31 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x5a82_ADDR                       			(32 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x00ff_ADDR                       			(33 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_6390_ADDR                         			(34 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_21955_ADDR                        			(35 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_3641_ADDR                         			(36 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0xf508_ADDR                       			(37 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_1260_ADDR                         			(38 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_720_ADDR                          			(39 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x3fff_ADDR                       			(40 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x7f80_ADDR                       			(41 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0x7c00_ADDR                       			(42 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_1000_ADDR                         			(43 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_32750_ADDR                        			(44 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_17578_ADDR                        			(45 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_8521_ADDR                         			(46 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0xa80_ADDR                        			(47 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_5443_ADDR                         			(48 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_1310_ADDR                         			(49 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_28160_ADDR                        			(50 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_1843_ADDR                         			(51 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_3427_ADDR                         			(52 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_6242_ADDR                         			(53 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_31821_ADDR                        			(54 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_3276_ADDR                         			(55 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_368_ADDR                          			(56 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_21298_ADDR                        			(57 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_24660_ADDR                        			(58 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_5462_ADDR                         			(59 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_13106_ADDR                        			(60 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_1500_ADDR                         			(61 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define CONST_0_ADDR                            			(62 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_1_ADDR                            			(64 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x40000000_ADDR                   			(66 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x7FFFFFFF_ADDR                   			(68 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x80000000_ADDR                   			(70 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x8000FFFF_ADDR                   			(72 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_1_4_ADDR                        			(74 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00010001_ADDR                   			(76 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00004000L_ADDR                  			(78 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00007FFF_ADDR                   			(80 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00008000_ADDR                   			(82 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x0000FFFF_ADDR                   			(84 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x70816958_ADDR                   			(86 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x69cb0014_ADDR                   			(88 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_15000_ADDR                        			(90 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x53c00_ADDR                      			(92 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00040000L_ADDR                  			(94 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_0xc800_ADDR                     			(96 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_0xf6b3_ADDR                     			(98 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_150_ADDR                        			(100 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_13849L_ADDR                       			(102 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x10000000_ADDR                   			(104 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x40000028_ADDR                   			(106 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x55550001_ADDR                   			(108 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x7fff0007_ADDR                   			(110 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_3500_ADDR                       			(112 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x04000000_ADDR                   			(114 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x80017fff_ADDR                   			(116 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x2000e000_ADDR                   			(118 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_783741L_ADDR                      			(120 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x100000_ADDR                     			(122 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_72_40_ADDR                        			(124 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_40_36_ADDR                        			(126 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_1_8_ADDR                        			(128 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_1_16_ADDR                       			(130 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_1_32_ADDR                       			(132 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_D_1_64_ADDR                       			(134 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_1_8_0x4000_ADDR                   			(136 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_1_16_1_4_ADDR                     			(138 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_1_32_1_4_ADDR                     			(140 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_1_64_1_8_ADDR                     			(142 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x00000100_ADDR                   			(144 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_40_64_ADDR                        			(146 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x7fff1fff_ADDR                   			(148 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0x8000e000_ADDR                   			(150 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_ALPHA_DOWN2_ALPHA_UP2_ADDR        			(152 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_600_32000_ADDR                    			(154 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_800_30500_ADDR                    			(156 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define CONST_0xc3da1e13_ADDR                   			(158 + AMR_EFR_RAMX_CONSTANTS_ADDR) //2 short
#define TABLE_SQRT_L_ADDR                       			(160 + AMR_EFR_RAMX_CONSTANTS_ADDR) //50 short
#define STATIC_CONST_PRMNO_ADDR                 			(210 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_CDOWN_ADDR                 			(220 + AMR_EFR_RAMX_CONSTANTS_ADDR) //8 short
#define STATIC_CONST_PDOWN_ADDR                 			(228 + AMR_EFR_RAMX_CONSTANTS_ADDR) //8 short
#define TABLE_POW2_ADDR                         			(236 + AMR_EFR_RAMX_CONSTANTS_ADDR) //34 short
#define TABLE_INV_SQRT_ADDR                     			(270 + AMR_EFR_RAMX_CONSTANTS_ADDR) //50 short
#define TABLE_LSP_LSF_ADDR                      			(320 + AMR_EFR_RAMX_CONSTANTS_ADDR) //66 short
#define TABLE_SLOPE_ADDR                        			(386 + AMR_EFR_RAMX_CONSTANTS_ADDR) //64 short
#define STATIC_CONST_dtx_log_en_adjust_ADDR     			(450 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_lsf_hist_mean_scale_ADDR   			(460 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_DGRAY_ADDR                 			(470 + AMR_EFR_RAMX_CONSTANTS_ADDR) //8 short
#define STATIC_CONST_GRAY_ADDR                  			(478 + AMR_EFR_RAMX_CONSTANTS_ADDR) //8 short
#define TABLE_LOG2_ADDR                         			(486 + AMR_EFR_RAMX_CONSTANTS_ADDR) //34 short
#define STATIC_CONST_qua_gain_code_ADDR         			(520 + AMR_EFR_RAMX_CONSTANTS_ADDR) //96 short
#define STATIC_CONST_QUA_GAIN_PITCH_ADDR        			(616 + AMR_EFR_RAMX_CONSTANTS_ADDR) //16 short
#define TABLE_GRID_ADDR                         			(632 + AMR_EFR_RAMX_CONSTANTS_ADDR) //62 short
#define STATIC_CONST_PRMNOFSF_ADDR              			(694 + AMR_EFR_RAMX_CONSTANTS_ADDR) //8 short
#define STATIC_CONST_BITNO_ADDR                 			(702 + AMR_EFR_RAMX_CONSTANTS_ADDR) //30 short
#define STATIC_CONST_bitno_MR475_ADDR           			(732 + AMR_EFR_RAMX_CONSTANTS_ADDR) //9 short
#define STATIC_CONST_bitno_MR515_ADDR           			(741 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_bitno_MR59_ADDR            			(751 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_bitno_MR67_ADDR            			(761 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_bitno_MR74_ADDR            			(771 + AMR_EFR_RAMX_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_bitno_MR795_ADDR           			(781 + AMR_EFR_RAMX_CONSTANTS_ADDR) //12 short
#define STATIC_CONST_bitno_MR102_ADDR           			(793 + AMR_EFR_RAMX_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_bitno_MRDTX_ADDR           			(813 + AMR_EFR_RAMX_CONSTANTS_ADDR) //3 short
#define STATIC_CONST_ph_imp_mid_ADDR            			(816 + AMR_EFR_RAMX_CONSTANTS_ADDR) //40 short
#define STATIC_CONST_ph_imp_low_MR795_ADDR      			(856 + AMR_EFR_RAMX_CONSTANTS_ADDR) //40 short
#define STATIC_trackTable_ADDR                  			(896 + AMR_EFR_RAMX_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_table_gain_highrates_ADDR  			(916 + AMR_EFR_RAMX_CONSTANTS_ADDR) //512 short
#define STATIC_CONST_table_gain_MR475_ADDR      			(1428 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1024 short
#define STATIC_mr515_3_lsf_compressed_ADDR      			(2452 + AMR_EFR_RAMX_CONSTANTS_ADDR) //416 short
#define TABLE_DICO1_LSF_3_compressed_ADDR       			(2868 + AMR_EFR_RAMX_CONSTANTS_ADDR) //624 short
#define TABLE_DICO3_LSF_3_compressed_ADDR       			(3492 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1664 short
#define TABLE_DICO1_LSF_compressed_ADDR         			(5156 + AMR_EFR_RAMX_CONSTANTS_ADDR) //416 short
#define ReservedX4_ADDR                         			(5572 + AMR_EFR_RAMX_CONSTANTS_ADDR) //34 short
#define GLOBAL_MIC_DIGITAL_OLD_GAIN_ADDR        			(5606 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_RECEIVER_DIGITAL_OLD_GAIN_ADDR   			(5607 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_AEC_DIGITAL_OLD_GAIN_ADDR        			(5608 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_DIGITAL_OLD_GAIN_ADDR            			(5609 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define COS_TABLE_ADDR                          			(5610 + AMR_EFR_RAMX_CONSTANTS_ADDR) //129 short
#define TAN_TABLE_ADDR                          			(5739 + AMR_EFR_RAMX_CONSTANTS_ADDR) //191 short
#define GLOBAL_NON_CLIP_TEMP_BUFFER             			(COS_TABLE_ADDR) //alias
#define GLOBAL_NON_CLIP_MIC_HISTORYDATA         			(5930 + AMR_EFR_RAMX_CONSTANTS_ADDR) //160 short
#define GLOBAL_NON_CLIP_CROSSZEROCOUNTER        			(6090 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_NON_CLIP_CROSSZEPEAK             			(6091 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_DIGITAL_GAIN_ADDR                			(6092 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_DIGITAL_MAXVALUE_ADDR            			(6093 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_NOTCH_FILTER_XN12_YN12_ADDR      			(6094 + AMR_EFR_RAMX_CONSTANTS_ADDR) //90 short
#define GLOBAL_NOTCH_FILTER_COEF_ADDR           			(6184 + AMR_EFR_RAMX_CONSTANTS_ADDR) //1 short
#define GLOBAL_NOTCH_FILTER_COEF_ADDR0          			(6185 + AMR_EFR_RAMX_CONSTANTS_ADDR) //15 short

#define CONST_0x7FFF_ADDR                       			(CONST_0x00007FFF_ADDR) //alias
#define CONST_0xFFFF_ADDR                       			(CONST_0x0000FFFF_ADDR) //alias
#define CONST_0x8000_ADDR                       			(CONST_0x00008000_ADDR) //alias
#define CONST_0x4000_ADDR                       			(CONST_0x00004000L_ADDR) //alias
#define CONST_0x5999_ADDR                       			((6+ STATIC_CONST_CDOWN_ADDR)) //alias

//GLOBAL_VARS
//VPP_SPEECH_INIT_STRUCT
#define VPP_SPEECH_INIT_STRUCT                  			(0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_LOADED_VOCODER_ADDR              			(0 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_EFR_ADDR              			(1 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_ENC_MODE_ADDR         			(2 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_LOADED_AMR_DEC_MODE_ADDR         			(3 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR    			(4 + VPP_SPEECH_INIT_STRUCT) //6 short
#define GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR    			(10 + VPP_SPEECH_INIT_STRUCT) //4 short
#define GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR			(14 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR			(30 + VPP_SPEECH_INIT_STRUCT) //16 short
#define GLOBAL_CONSTANTS_ADDR_ADDR              			(46 + VPP_SPEECH_INIT_STRUCT) //8 short
#define GLOBAL_RESET_ADDR                       			(54 + VPP_SPEECH_INIT_STRUCT) //1 short
#define GLOBAL_IRQGEN_ADDR                      			(55 + VPP_SPEECH_INIT_STRUCT) //1 short

//VPP_SPEECH_AUDIO_CFG_STRUCT
#define VPP_SPEECH_AUDIO_CFG_STRUCT             			(56 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_ECHO_ES_ON                       			(0 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_ES_VAD                      			(1 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_ES_DTD                      			(2 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_REL                      			(3 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_MU                       			(4 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ECHO_EC_MIN                      			(5 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_ENC_MICGAIN_ADDR                 			(6 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_DEC_SPKGAIN_ADDR                 			(7 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short
#define GLOBAL_SDF_ADDR                         			(8 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_MDF_ADDR                         			(10 + VPP_SPEECH_AUDIO_CFG_STRUCT) //2 short
#define GLOBAL_IF1_flag_ADDR                    			(12 + VPP_SPEECH_AUDIO_CFG_STRUCT) //1 short

#define GLOBAL_DTX_ADDR                         			(69 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RELOAD_MODE_ADDR                 			(70 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_AMR_EFR_RESET_ADDR               			(71 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_MICGAIN_ADDR                 			(72 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OLD_SPKGAIN_ADDR                 			(73 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SDF_COEFFS_ADDR                  			(74 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_BFI_ADDR                         			(138 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_UFI_ADDR                         			(139 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SID_ADDR                         			(140 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TAF_ADDR                         			(141 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_MODE_ADDR                    			(142 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_MODE_ADDR                    			(143 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_USED_MODE_ADDR               			(144 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_EFR_FLAG_ADDR                    			(145 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_RESET_FLAG_ADDR                     			(146 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_MAIN_RESET_FLAG_OLD_ADDR            			(147 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MDF_COEFFS_ADDR                  			(148 + COMMON_GLOBAL_Y_BEGIN_ADDR) //64 short
#define GLOBAL_ECHO_SUPPRESS_EXP_COUNTER        			(212 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ECHO_CANCEL_EXP_MU               			(213 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ECHO_CANCEL_DEC_PWR              			(214 + COMMON_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR      			(216 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR      			(217 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR      			(218 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR      			(219 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_SUPPRESS_DEFAULT_TX         			(220 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_SUPPRESS_DEFAULT_RX         			(221 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_TX_RANGE1          			(222 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_TX_RANGE2          			(223 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_TX_RANGE3          			(224 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_TX_RANGE4          			(225 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RX_RANGE1          			(226 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RX_RANGE2          			(227 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RX_RANGE3          			(228 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RX_RANGE4          			(229 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_SUPPRESS_DEFAULT            			(230 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MSDF_RELOAD_FLAG                 			(231 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RANGE1             			(232 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RANGE2             			(233 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RANGE3             			(234 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SPEEX_NOISE_PROB_MIN_RANGE4             			(235 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define MIC_NONCLIP_ENABLE_FLAG_ADDR            			(236 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define RECEIVER_NONCLIP_ENABLE_FLAG_ADDR       			(237 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_TX     			(238 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_NOISE_THRESHOLD_TX      			(239 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_RX     			(240 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_NOISE_THRESHOLD_RX      			(241 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_SMOOTH_THRESHOLD        			(242 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_NOISE_THRESHOLD         			(243 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TX_BEFORE_ENC_ADDR               			(244 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_DEC_OUTPUT_DELAY_BUFFER1_ADDR    			(246 + COMMON_GLOBAL_Y_BEGIN_ADDR) //50 short
#define GLOBAL_DEC_OUTPUT_DELAY_BUFFER2_ADDR    			(296 + COMMON_GLOBAL_Y_BEGIN_ADDR) //50 short
#define NONCLIP_ENABLE_FLAG_ADDR                			(346 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define NONCLIP_ENABLE_FLAG_RESERVED            			(347 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SCALE_MIC_ENABLE_FLAG_ADDR              			(348 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define SCALE_RECEIVER_ENABLE_FLAG_ADDR         			(349 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_SCALE_IN_ADDR                			(350 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_SCALE_OUT_ADDR          			(351 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER3_XN_1_ADDR      			(352 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_HIGH_PASS_FILTER3_XN_2_ADDR      			(354 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_HIGH_PASS_FILTER3_YN_1_ADDR      			(356 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_HIGH_PASS_FILTER3_YN_2_ADDR      			(358 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_HIGH_PASS_FILTER3_NUM1_ADDR      			(360 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER3_NUM2_ADDR      			(361 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER3_DEN1_ADDR      			(362 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_HIGH_PASS_FILTER3_DEN2_ADDR      			(363 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define HPF_CLIP_MAX_SHORT                      			(364 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define HPF_CLIP_MIN_SHORT                      			(366 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_COMFORTALBE_NOISE_ADDR           			(368 + COMMON_GLOBAL_Y_BEGIN_ADDR) //12 short
#define GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR  			(380 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_VAD_COUNTER                      			(381 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR  			(382 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR 			(383 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR    			(384 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR   			(385 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_DIGITAL_GAIN_ADDR            			(386 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR        			(387 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR       			(388 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR   			(389 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUTENERGY_ADDR                			(390 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_INPUTENERGY_ADDR                 			(392 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
//m_IIRProcPara_ADDR
#define m_IIRProcPara_ADDR                      			(394 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define m_IIRProcPara_band0_coeffs              			(0 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band1_coeffs              			(6 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band2_coeffs              			(12 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band3_coeffs              			(18 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band4_coeffs              			(24 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band5_coeffs              			(30 + m_IIRProcPara_ADDR) //6 short
#define m_IIRProcPara_band6_coeffs              			(36 + m_IIRProcPara_ADDR) //6 short

#define IIR_ENABLE_FLAG_ADDR                    			(436 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define IIR_INPUT_ADDR_ADDR                     			(437 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
//m_AECExtraPara_ADDR
#define m_AECExtraPara_ADDR                     			(438 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define m_AECProcPara_DelaySampleNum            			(0 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_FrameCntLimitNum          			(1 + m_AECExtraPara_ADDR) //3 short
#define m_AECProcPara_NoiseCntLimitNum          			(4 + m_AECExtraPara_ADDR) //4 short
#define m_AECProcPara_NoiseWeightSNRLimitMax    			(8 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NoiseWeightSNRLimitMin    			(9 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NoiseWeightSNRTh1         			(10 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NoiseWeightSNRTh2         			(11 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NoiseWeightSNRLimitStep   			(12 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NoiseWeightSub1DecayFactor			(13 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_ResidualEchoEnable        			(14 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_AGCRrrSpkRelation         			(15 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_AddAttDB                  			(16 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_MinAttDB                  			(17 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_MaxAttDB                  			(18 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_NPowRiseFactor            			(19 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_WorkFactFactor            			(20 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_AttExtFactor              			(21 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_AttRiseFactor             			(22 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_AttDecayFactor            			(23 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_Log2LinFactor             			(24 + m_AECExtraPara_ADDR) //1 short
#define m_AECProcPara_Reversed                  			(25 + m_AECExtraPara_ADDR) //1 short

#define GLOBAL_AECExtraPara_Reversed            			(464 + COMMON_GLOBAL_Y_BEGIN_ADDR) //44 short
#define GLOBAL_AGC_CODE_ADDR                    			(508 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AGC_CONSTX_ADDR                  			(510 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define AGC1_ConstY_ExternalAddr_addr           			(GLOBAL_AGC_CONSTX_ADDR) //alias
#define GLOBAL_MIC_AGC_RAMX_BACKUP_ADDR         			(512 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_RECEIVER_AGC_RAMX_BACKUP_ADDR    			(514 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MICAGC_RELOAD                    			(516 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVERAGC_RELOAD               			(517 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
//STRUCT_MicAgcConfig_addr
#define STRUCT_MicAgcConfig_addr                			(518 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define STRUCT_MicAgcConfig_enable_addr         			(0 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_targetLevelDbfs_addr			(1 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_compressionGaindB_addr			(2 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_limiterEnable_addr  			(3 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_upperThr_addr       			(4 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_lowerThr_addr       			(5 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_decayValue_addr     			(6 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_capacitorSlowFactor_addr			(7 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_capacitorFastFactor_addr			(8 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_NoiseCompressFactor_addr			(9 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_NoiseGateThr_addr   			(10 + STRUCT_MicAgcConfig_addr) //1 short
#define STRUCT_MicAgcConfig_NoiseStdShortTermFactor_addr			(11 + STRUCT_MicAgcConfig_addr) //1 short

//STRUCT_ReceiverAgcConfig_addr
#define STRUCT_ReceiverAgcConfig_addr           			(530 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define STRUCT_ReceiverAgcConfig_enable_addr    			(0 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_targetLevelDbfs_addr			(1 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_compressionGaindB_addr			(2 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_limiterEnable_addr			(3 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_upperThr_addr  			(4 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_lowerThr_addr  			(5 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_decayValue_addr			(6 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_capacitorSlowFactor_addr			(7 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_capacitorFastFactor_addr			(8 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_NoiseCompressFactor_addr			(9 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_NoiseGateThr_addr			(10 + STRUCT_ReceiverAgcConfig_addr) //1 short
#define STRUCT_ReceiverAgcConfig_NoiseStdShortTermFactor_addr			(11 + STRUCT_ReceiverAgcConfig_addr) //1 short

#define GLOBAL_MICFIR_ENABLE_FLAG_ADDR          			(542 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RECEIVERFIR_ENABLE_FLAG_ADDR     			(543 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_RESERVE_Y_ADDR                   			(544 + COMMON_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR			(552 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR			(553 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_SPEECH_RAMX_BACKUP_ADDR          			(554 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_SPEECH_CODE_BACKUP_ADDR          			(556 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_RAMX_BACKUP_ADDR             			(558 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_CODE_ADDR                    			(560 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_AEC_CONSTX_ADDR                  			(562 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_RX_ADDR                          			(564 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_TX_ADDR                          			(566 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_NOISESUPPRESS_CODE_ADDR          			(568 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_NOISESUPPRESS_CONSTX_ADDR        			(570 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR			(572 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR			(574 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
//m_AECProcPara_ADDR
#define m_AECProcPara_ADDR                      			(576 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define AEC_ENABLE_FLAG_ADDR                    			(0 + m_AECProcPara_ADDR) //1 short
#define AGC_ENABLE_FLAG_ADDR                    			(1 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_ProcessMode               			(2 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_InitRegularFactor         			(3 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFUpdateFactor            			(4 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFCtrlSpeedUpFactor       			(5 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AFFilterAmp               			(6 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoEstMethod             			(7 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseUpdateFactor         			(8 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRUpdateFactor           			(9 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SNRLimit                  			(10 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseOverEstFactor        			(11 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseGainLimit            			(12 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoUpdateFactor          			(13 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERUpdateFactor           			(14 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_SERLimit                  			(15 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoOverEstFactor         			(16 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_EchoGainLimit             			(17 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_CNGFactor                 			(18 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpGain                   			(19 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_NoiseMin                  			(20 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFPrevEchoEstLimit        			(22 + m_AECProcPara_ADDR) //1 word
#define m_AECProcPara_PFDTEchoOverEstFactor     			(24 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_pPFEchoGainLimitAmp       			(25 + m_AECProcPara_ADDR) //9 short
#define m_AECProcPara_NoiseGainLimitStep        			(34 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_AmpThr_ADDR               			(35 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDataSmoothFactor      			(36 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLNoiseUpdateFactor     			(37 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrSigDet             			(38 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDT                 			(39 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_DataSmoothFactor          			(40 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLChanlGain             			(41 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrNT                 			(42 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLThrDTDiff             			(43 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTHoldTime            			(44 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTStartTime           			(45 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDUpdateFactor       			(46 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTDThreshold          			(47 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_PFCLDTD2ndThreshold       			(48 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara_StrongEchoFlag            			(49 + m_AECProcPara_ADDR) //1 short
#define m_AECProcPara__PFCLDTDThrRatio          			(50 + m_AECProcPara_ADDR) //1 short

#define GLOBAL_SPEECH_RECORD_FLAG               			(627 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_AEC_RESET_ADDR                   			(628 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_MORPH_RESET_ADDR                 			(629 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define MORPHVOICE_ENABLE_FLAG_ADDR             			(630 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define PITCH_SHIFT_ADDR                        			(631 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_MORPH_CODE_ADDR                  			(632 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLOBAL_MORPH_CONSTX_ADDR                			(634 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 word
#define GLABAL_NOISESUPPRESS_RESET_ADDR         			(636 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLABAL_NOISESUPPRESS_INPUTADDR_ADDR     			(637 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ingSumPhase_ADDR                        			(638 + COMMON_GLOBAL_Y_BEGIN_ADDR) //129 short
#define gRover_ADDR                             			(767 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_VOCODER_RESET_ADDR               			(768 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_Vad_ADDR                  			(769 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_OUTPUT_SP_ADDR                   			(770 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_AMR_FRAME_TYPE_ADDR                 			(771 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_ADDR                     			(772 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_ADDR                    			(773 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define ENC_INPUT_ADDR_BAK_ADDR                 			(774 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
#define DEC_OUTPUT_ADDR_BAK_ADDR                			(775 + COMMON_GLOBAL_Y_BEGIN_ADDR) //1 short
//AMR_EFR_RAMY_CONSTANTS_ADDR
#define AMR_EFR_RAMY_CONSTANTS_ADDR             			(776 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define STATIC_CONST_F_GAMMA1_ADDR              			(0 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_F_GAMMA2_ADDR              			(10 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_F_GAMMA3_ADDR              			(20 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_F_GAMMA4_ADDR              			(30 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_gamma3_ADDR                			(40 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_gamma1_ADDR                			(50 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_WIND_200_40_compressd_ADDR 			(60 + AMR_EFR_RAMY_CONSTANTS_ADDR) //122 short
#define STATIC_CONST_INTER_6_ADDR               			(182 + AMR_EFR_RAMY_CONSTANTS_ADDR) //62 short
#define STATIC_CONST_INTER_6_25_ADDR            			(244 + AMR_EFR_RAMY_CONSTANTS_ADDR) //26 short
#define TABLE_LAG_L_ADDR                        			(270 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define TABLE_LAG_H_ADDR                        			(280 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_pred_ADDR                  			(290 + AMR_EFR_RAMY_CONSTANTS_ADDR) //4 short
#define STATIC_pred_fac_ADDR                    			(294 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_past_rq_init_ADDR                			(304 + AMR_EFR_RAMY_CONSTANTS_ADDR) //80 short
#define STATIC_MEAN_LSF_3_ADDR                  			(384 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define STATIC_CONST_lsp_init_data_ADDR         			(394 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define TABLE_MEAN_LSF_ADDR                     			(404 + AMR_EFR_RAMY_CONSTANTS_ADDR) //10 short
#define TABLE_WINDOW_160_80_compressd_ADDR      			(414 + AMR_EFR_RAMY_CONSTANTS_ADDR) //122 short
#define TABLE_WINDOW_232_8_compressd_ADDR       			(536 + AMR_EFR_RAMY_CONSTANTS_ADDR) //126 short
#define STATIC_CONST_pred_MR122_ADDR            			(662 + AMR_EFR_RAMY_CONSTANTS_ADDR) //4 short
#define STATIC_CONST_INTERP_FACTOR_ADDR         			(666 + AMR_EFR_RAMY_CONSTANTS_ADDR) //24 short
#define STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR  			(690 + AMR_EFR_RAMY_CONSTANTS_ADDR) //96 short
#define STATIC_CONST_DHF_MASK_ADDR              			(786 + AMR_EFR_RAMY_CONSTANTS_ADDR) //58 short
#define STATIC_CONST_dhf_MR122_ADDR             			(844 + AMR_EFR_RAMY_CONSTANTS_ADDR) //58 short
#define STATIC_CONST_dhf_MR475_ADDR             			(902 + AMR_EFR_RAMY_CONSTANTS_ADDR) //18 short
#define STATIC_CONST_dhf_MR515_ADDR             			(920 + AMR_EFR_RAMY_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_dhf_MR59_ADDR              			(940 + AMR_EFR_RAMY_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_dhf_MR67_ADDR              			(960 + AMR_EFR_RAMY_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_dhf_MR74_ADDR              			(980 + AMR_EFR_RAMY_CONSTANTS_ADDR) //20 short
#define STATIC_CONST_dhf_MR795_ADDR             			(1000 + AMR_EFR_RAMY_CONSTANTS_ADDR) //24 short
#define STATIC_CONST_dhf_MR102_ADDR             			(1024 + AMR_EFR_RAMY_CONSTANTS_ADDR) //40 short
#define STATIC_CONST_corrweight_ADDR            			(1064 + AMR_EFR_RAMY_CONSTANTS_ADDR) //128 short
#define STATIC_CONST_startPos_ADDR              			(1192 + AMR_EFR_RAMY_CONSTANTS_ADDR) //16 short
#define STATIC_CONST_ph_imp_low_ADDR            			(1208 + AMR_EFR_RAMY_CONSTANTS_ADDR) //40 short
#define STATIC_CONST_table_gain_lowrates_ADDR   			(1248 + AMR_EFR_RAMY_CONSTANTS_ADDR) //256 short
#define TABLE_DICO2_LSF_3_compressed_ADDR       			(1504 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1248 short
#define TABLE_DICO2_LSF_compressed_ADDR         			(2752 + AMR_EFR_RAMY_CONSTANTS_ADDR) //832 short
#define TABLE_DICO3_LSF_compressed_ADDR         			(3584 + AMR_EFR_RAMY_CONSTANTS_ADDR) //832 short
#define TABLE_DICO4_LSF_compressed_ADDR         			(4416 + AMR_EFR_RAMY_CONSTANTS_ADDR) //832 short
#define TABLE_DICO5_LSF_compressed_ADDR         			(5248 + AMR_EFR_RAMY_CONSTANTS_ADDR) //208 short
#define STATIC_mr795_1_lsf_compressed_ADDR      			(5456 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1248 short
#define TABLE_MR475_ADDR                        			(6704 + AMR_EFR_RAMY_CONSTANTS_ADDR) //96 short
#define TABLE_MR515_ADDR                        			(6800 + AMR_EFR_RAMY_CONSTANTS_ADDR) //104 short
#define TABLE_MR59_ADDR                         			(6904 + AMR_EFR_RAMY_CONSTANTS_ADDR) //118 short
#define TABLE_MR67_ADDR                         			(7022 + AMR_EFR_RAMY_CONSTANTS_ADDR) //134 short
#define TABLE_MR74_ADDR                         			(7156 + AMR_EFR_RAMY_CONSTANTS_ADDR) //148 short
#define TABLE_MR795_ADDR                        			(7304 + AMR_EFR_RAMY_CONSTANTS_ADDR) //160 short
#define TABLE_MR102_ADDR                        			(7464 + AMR_EFR_RAMY_CONSTANTS_ADDR) //204 short
#define TABLE_MR122_ADDR                        			(7668 + AMR_EFR_RAMY_CONSTANTS_ADDR) //244 short
#define Frame_header_ADDR                       			(7912 + AMR_EFR_RAMY_CONSTANTS_ADDR) //8 short
#define Bits_no_ADDR                            			(7920 + AMR_EFR_RAMY_CONSTANTS_ADDR) //8 short
#define Reserved2_ADDR                          			(7928 + AMR_EFR_RAMY_CONSTANTS_ADDR) //76 short
#define GLOBAL_NON_CLIP_SPK_HISTORYDATA         			(8004 + AMR_EFR_RAMY_CONSTANTS_ADDR) //160 short
#define GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG   			(8164 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_EXTERN_PROCESS_MIC_ENABLE_FLAG   			(8165 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define HIGH_PASS_FILTER_ENABLE_FLAG_ADDR       			(8166 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define NOTCH_FILTER_ENABLE_FLAG_ADDR           			(8167 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR   			(8168 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR			(8169 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR    			(8170 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR			(8171 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_MORPH_RAMX_BACKUP_ADDR           			(8172 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 word
#define GLOBAL_AGC_Coeff_l                      			(8174 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 word
#define GLOBAL_MIC_ENERGY_COUNTER_ADDR          			(8176 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_NOISE_DIGITAL_GAIN_ADDR          			(8177 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR        			(8178 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short
#define GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR    			(8179 + AMR_EFR_RAMY_CONSTANTS_ADDR) //1 short

#define CONST_0x4CCC_ADDR                       			((148+ STATIC_CONST_table_gain_lowrates_ADDR)) //alias

/****************************
 **  NOISESUPPRESS_GLOBAL  **
 ****************************/

//GLOBAL_VARS
#define For_noisesuppress_align                 			(0 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //816 short
#define SpxPpState_frame_size                   			(816 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_ps_size                      			(817 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_sampling_rate                			(818 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_nbands                       			(819 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_denoise_enabled              			(820 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_vad_enabled                  			(821 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_dereverb_enabled             			(822 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_reverb_decay                 			(823 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_reverb_level                 			(824 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob_start            			(825 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob_continue         			(826 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_noise_suppress               			(827 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_suppress                			(828 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_suppress_active         			(829 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_echo_state_ADDR              			(830 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_speech_prob                  			(831 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_frame                        			(832 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_frame_N                      			(912 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_ft                           			(992 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_gain                         			(1152 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain_M                       			(1232 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_prior                        			(1256 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_prior_M                      			(1336 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_post                         			(1360 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_post_M                       			(1440 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_gain2                        			(1464 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain2_M                      			(1544 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_gain_floor                   			(1568 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_gain_floor_M                 			(1648 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_zeta                         			(1672 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_zeta_M                       			(1752 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 short
#define SpxPpState_inbuf                        			(1776 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_outbuf                       			(1856 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_nb_adapt                     			(1936 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_was_speech                   			(1937 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_min_count                    			(1938 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_frame_shift                  			(1939 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_noise                        			(1940 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_noise_M                      			(2100 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_reverb_estimate              			(2148 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //104 word
#define SpxPpState_old_ps                       			(2356 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_old_ps_M                     			(2516 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_ps                           			(2564 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_ps_M                         			(2724 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_S                            			(2772 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_Smin                         			(2932 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_Stmp                         			(3092 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_update_prob                  			(3252 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_echo_noise                   			(3332 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 word
#define SpxPpState_echo_noise_M                 			(3492 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //24 word
#define SpxPpState_residual_echo                			(3540 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //104 word
#define SpxPpState_fft_in_ADDR                  			(3748 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_out_ADDR                 			(3749 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_ifft_in_ADDR                 			(3750 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_ifft_out_ADDR                			(3751 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxEchoState_y                          			(3752 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxEchoState_Y                          			(3912 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxEchoState_leak_estimate              			(4072 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxEchoState_reservered                 			(4073 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxEchoState_last_y                     			(4074 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxEchoState_last_y_N                   			(4154 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SPEEX_REF_BUF                           			(4234 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define COMPUTE_GAIN_FLOOR_NOISE_ECHO_GAIN      			(4314 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPX_KF_BFLY3_EPI3                       			(4316 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPX_KF_BFLY_SCRATCH                     			(4318 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_0                   			(SPX_KF_BFLY_SCRATCH) //alias
#define SPX_KF_BFLY_SCRATCH_0i                  			(4319 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1                   			(4320 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_1i                  			(4321 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2                   			(4322 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_2i                  			(4323 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3                   			(4324 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_3i                  			(4325 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4                   			(4326 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_4i                  			(4327 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5                   			(4328 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_5i                  			(4329 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6                   			(4330 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_6i                  			(4331 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7                   			(4332 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_7i                  			(4333 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8                   			(4334 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_8i                  			(4335 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9                   			(4336 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_9i                  			(4337 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10                  			(4338 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_10i                 			(4339 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11                  			(4340 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_11i                 			(4341 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12                  			(4342 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_SCRATCH_12i                 			(4343 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YA                          			(4344 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YAi                         			(4345 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YB                          			(4346 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_YBi                         			(4347 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_FSTRIDE                     			(4348 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_M                           			(4349 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_N                           			(4350 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_MM                          			(4351 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPX_KF_BFLY_TMP                         			(4352 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_RESERVR_01                        			(4353 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SPEEX_SPEAKER_DELAY_BUFFER              			(4354 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SPEEX_RESERVR_FOR_ALIGN                 			(4514 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //6 short
#define SpxPpState_window                       			(4520 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_bank_bank_left               			(4680 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_bank_right              			(4760 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_filter_left             			(4840 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_filter_right            			(4920 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //80 short
#define SpxPpState_bank_nb_banks                			(5000 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_bank_len                     			(5001 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define FILTERBANK_NB_BANKS                     			(SpxPpState_bank_nb_banks) //alias
#define FILTERBANK_LEN                          			(SpxPpState_bank_len) //alias
#define SpxPpState_fft_lookup_forward_substate_nfft			(5002 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup                   			(SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate  			(SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SpxPpState_fft_lookup_forward_substate_inverse			(5003 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_forward_substate_factors			(5004 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //64 short
#define SpxPpState_fft_lookup_forward_substate_twiddles			(5068 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_tmpbuf    			(5228 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_forward_super_twiddles			(5388 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_substate_nfft			(5548 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate 			(SpxPpState_fft_lookup_backward_substate_nfft) //alias
#define SpxPpState_fft_lookup_backward_substate_inverse			(5549 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_backward_substate_factors			(5550 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //64 short
#define SpxPpState_fft_lookup_backward_substate_twiddles			(5614 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_tmpbuf   			(5774 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_backward_super_twiddles			(5934 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxPpState_fft_lookup_N                 			(6094 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxPpState_fft_lookup_reserved          			(6095 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 short
#define SpxEchoState_window                     			(6096 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //160 short
#define SpxEchoState_fft_table                  			(SpxPpState_fft_lookup_forward_substate_nfft) //alias
#define SPEEX_ATAN01_M                          			(6256 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //4 word
#define SPEEX_COS_PI_2_L                        			(6264 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //4 word
#define SPEEX_SQRT_C                            			(6272 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_UPDATE_NOISE_X                    			(6276 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_HYPERGEOM_GAIN_TABLE              			(6280 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //22 short
#define SPEEX_EXP_D                             			(6302 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //4 short
#define SPEEX_CONST_0                           			(6306 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1                           			(6308 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_2                           			(6310 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_3                           			(6312 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_4                           			(6314 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_5                           			(6316 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6                           			(6318 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_7                           			(6320 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_8                           			(6322 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_14                          			(6324 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_16                          			(6326 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_17                          			(6328 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19                          			(6330 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20                          			(6332 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29                          			(6334 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_64                          			(6336 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_97                          			(6338 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_100                         			(6340 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_128                         			(6342 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_236                         			(6344 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_256                         			(6346 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_472                         			(6348 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_512                         			(6350 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_983                         			(6352 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1000                        			(6354 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_2458                        			(6356 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_3277                        			(6358 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_4915                        			(6360 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6521                        			(6362 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_6711                        			(6364 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_8192                        			(6366 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_9830                        			(6368 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_10000                       			(6370 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_10912                       			(6372 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_16384                       			(6374 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19458                       			(6376 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_19661                       			(6378 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20000                       			(6380 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_20839                       			(6382 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_21290                       			(6384 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_21924                       			(6386 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_22938                       			(6388 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_23637                       			(6390 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_24576                       			(6392 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_25736                       			(6394 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_26214                       			(6396 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29164                       			(6398 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_29458                       			(6400 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32000                       			(6402 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32766                       			(6404 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32767                       			(6406 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_32768                       			(6408 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_65535                       			(6410 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_65536                       			(6412 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_131071                      			(6414 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_131072                      			(6416 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x80000                     			(6418 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_1087164                     			(6420 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x800000                    			(6422 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_0x7fffffff                  			(6424 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG21290                    			(6426 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG15                       			(6428 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG2                        			(6430 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word
#define SPEEX_CONST_NEG32767                    			(6432 + NOISESUPPRESS_GLOBAL_X_BEGIN_ADDR) //1 word

/***************************
 **  NOISESUPPRESS_LOCAL  **
 ***************************/

/********************
 **  MORPH_GLOBAL  **
 ********************/

//GLOBAL_VARS
#define For_MORPH_align                         			(0 + MORPH_GLOBAL_X_BEGIN_ADDR) //816 short
#define gInFIFO_ADDR                            			(816 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define gOutFIFO_ADDR                           			(1072 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingLastPhase_ADDR                       			(1328 + MORPH_GLOBAL_X_BEGIN_ADDR) //130 short
#define ingOutputAccum_ADDR                     			(1458 + MORPH_GLOBAL_X_BEGIN_ADDR) //512 short
#define FFT_INPUT_ADDRESS                       			(1970 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define FFT_OUTPUT_ADDRESS                      			(2226 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingFFTworksp_ADDRESS                    			(2482 + MORPH_GLOBAL_X_BEGIN_ADDR) //1024 short
#define sign_ADDRESS                            			(3506 + MORPH_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_UR_ADDRESS                         			(3508 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_UI_ADDRESS                         			(3509 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_LE2_ADDRESS                        			(3510 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_LE_ADDRESS                         			(3511 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define WR_ADDRESS                              			(3512 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define WI_ADDRESS                              			(3513 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_ADDR_ADDRESS                       			(3514 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP1_ADDR_ADDRESS                      			(3515 + MORPH_GLOBAL_X_BEGIN_ADDR) //1 short
#define ingAnaMagn_ADDRESS                      			(3516 + MORPH_GLOBAL_X_BEGIN_ADDR) //512 short
#define ingAnaFreq_ADDRESS                      			(4028 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingSynFreq_ADDR                         			(4284 + MORPH_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingSynMagn_ADDRESS                      			(4540 + MORPH_GLOBAL_X_BEGIN_ADDR) //512 short
#define GINOUT_ADDR_ADDRESS                     			(5052 + MORPH_GLOBAL_X_BEGIN_ADDR) //2 short

/*******************
 **  MORPH_LOCAL  **
 *******************/

/******************
 **  AGC_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define For_AGC_align                           			(0 + AGC_GLOBAL_X_BEGIN_ADDR) //816 short
#define GLOBAL_NON_CLIP_CROSSZEROINDEX          			(816 + AGC_GLOBAL_X_BEGIN_ADDR) //160 short
//STRUCT_DigitalAgc_addr
#define STRUCT_DigitalAgc_addr                  			(976 + AGC_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_DigitalAgc_capacitorSlow_addr    			(0 + STRUCT_DigitalAgc_addr) //1 word
#define STRUCT_DigitalAgc_capacitorFast_addr    			(2 + STRUCT_DigitalAgc_addr) //1 word
#define STRUCT_DigitalAgc_gain_addr             			(4 + STRUCT_DigitalAgc_addr) //1 word
#define STRUCT_DigitalAgc_gainTable_addr        			(6 + STRUCT_DigitalAgc_addr) //32 word
#define STRUCT_DigitalAgc_gatePrevious_addr     			(70 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_upper_thr_addr        			(71 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_lower_thr_addr        			(72 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_decay_value_addr      			(73 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_capacitorSlowFactor_addr			(74 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_capacitorFastFactor_addr			(75 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_NoiseCompressFactor_addr			(76 + STRUCT_DigitalAgc_addr) //1 short
#define STRUCT_DigitalAgc_NoiseGateThr_addr     			(77 + STRUCT_DigitalAgc_addr) //1 short

//STRUCT_AgcVadNearend_addr
#define STRUCT_AgcVadNearend_addr               			(1054 + AGC_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_AgcVadNearend_downState_addr     			(0 + STRUCT_AgcVadNearend_addr) //8 word
#define STRUCT_AgcVadNearend_HPstate_addr       			(16 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_counter_addr       			(17 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_logRatio_addr      			(18 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_meanLongTerm_addr  			(19 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_varianceLongTerm_addr			(20 + STRUCT_AgcVadNearend_addr) //1 word
#define STRUCT_AgcVadNearend_stdLongTerm_addr   			(22 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_meanShortTerm_addr 			(23 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_varianceShortTerm_addr			(24 + STRUCT_AgcVadNearend_addr) //1 word
#define STRUCT_AgcVadNearend_stdShortTerm_addr  			(26 + STRUCT_AgcVadNearend_addr) //1 short
#define STRUCT_AgcVadNearend_NoiseStdShortTermFactor_addr			(27 + STRUCT_AgcVadNearend_addr) //1 short

//STRUCT_AgcConfig_addr
#define STRUCT_AgcConfig_addr                   			(1082 + AGC_GLOBAL_X_BEGIN_ADDR) //struct
#define STRUCT_AgcConfig_enable_addr            			(0 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_targetLevelDbfs_addr   			(1 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_compressionGaindB_addr 			(2 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_limiterEnable_addr     			(3 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_upper_thr_addr         			(4 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_lower_thr_addr         			(5 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_decay_value_addr       			(6 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_capacitorSlowFactor_addr			(7 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_capacitorFastFactor_addr			(8 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_NoiseCompressFactor_addr			(9 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_NoiseGateThr_addr      			(10 + STRUCT_AgcConfig_addr) //1 short
#define STRUCT_AgcConfig_NoiseStdShortTermFactor_addr			(11 + STRUCT_AgcConfig_addr) //1 short

#define CONST_SubFrameNum_addr                  			(1094 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define buf_addr_addr                           			(1095 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define FS_addr                                 			(1096 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_ProcessVad_dB                    			(1097 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_Sqrt_k_sqrt_2                    			(1098 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_Sqrt_sh                          			(1099 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_ProcessVad_buf1                  			(1100 + AGC_GLOBAL_X_BEGIN_ADDR) //8 short
#define Global_ProcessVad_buf2                  			(1108 + AGC_GLOBAL_X_BEGIN_ADDR) //4 short
#define Global_ProcessVad_in_cur_ptr            			(1112 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_ProcessVad_nrSamples             			(1113 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_X_digCompGaindB_addr              			(STRUCT_AgcConfig_compressionGaindB_addr) //alias
#define LOCAL_X_analogTarget_addr               			(STRUCT_AgcConfig_compressionGaindB_addr) //alias
#define LOCAL_X_limiterLvlshf_addr              			(1114 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define LOCAL_X_den_addr                        			(1116 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define LOCAL_maxGain_addr                      			(1118 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_zeroGainLvl_addr                  			(1119 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_diffGain_addr                     			(1120 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_X_constMaxGain_addr               			(1121 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_X_limiterIdx_addr                 			(1122 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_X_intPart_addr                    			(1123 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_X_fracPart_addr                   			(1124 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define APD_LOCAL_X_env_addr                    			(1125 + AGC_GLOBAL_X_BEGIN_ADDR) //20 word
#define APD_LOCAL_X_gains_addr                  			(1165 + AGC_GLOBAL_X_BEGIN_ADDR) //21 word
#define APD_LOCAL_X_7fffshf_addr                			(1207 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define APD_LOCAL_X_gain32_addr                 			(1209 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define APD_LOCAL_X_L_addr                      			(1211 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define APD_LOCAL_X_L2_addr                     			(1212 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define APD_LOCAL_X_decay_addr                  			(1213 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM0_USED_ADDR_ADDR     			(1214 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM2_USED_ADDR_ADDR     			(1215 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN1_USED_ADDR_ADDR     			(1216 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_Reversed                			(1217 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM0_ADDR               			(1218 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM1_ADDR               			(1219 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_NUM2_ADDR               			(1220 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN0_ADDR               			(1221 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN1_ADDR               			(1222 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_DEN2_ADDR               			(1223 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_XN_1_USED_ADDR_ADDR     			(1224 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_XN_2_USED_ADDR_ADDR     			(1225 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_YN_1_USED_ADDR_ADDR     			(1226 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND_YN_2_USED_ADDR_ADDR     			(1227 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_IIR_BAND0_XN_1_ADDR              			(1228 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND0_XN_2_ADDR              			(1230 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND0_YN_1_ADDR              			(1232 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND0_YN_2_ADDR              			(1234 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND1_XN_1_ADDR              			(1236 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND1_XN_2_ADDR              			(1238 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND1_YN_1_ADDR              			(1240 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND1_YN_2_ADDR              			(1242 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND2_XN_1_ADDR              			(1244 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND2_XN_2_ADDR              			(1246 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND2_YN_1_ADDR              			(1248 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND2_YN_2_ADDR              			(1250 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND3_XN_1_ADDR              			(1252 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND3_XN_2_ADDR              			(1254 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND3_YN_1_ADDR              			(1256 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND3_YN_2_ADDR              			(1258 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND4_XN_1_ADDR              			(1260 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND4_XN_2_ADDR              			(1262 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND4_YN_1_ADDR              			(1264 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND4_YN_2_ADDR              			(1266 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND5_XN_1_ADDR              			(1268 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND5_XN_2_ADDR              			(1270 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND5_YN_1_ADDR              			(1272 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND5_YN_2_ADDR              			(1274 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND6_XN_1_ADDR              			(1276 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND6_XN_2_ADDR              			(1278 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND6_YN_1_ADDR              			(1280 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define GLOBAL_IIR_BAND6_YN_2_ADDR              			(1282 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define kGenFuncTable_addr                      			(1284 + AGC_GLOBAL_X_BEGIN_ADDR) //128 short
#define CONST_kGenFuncTableSize_addr            			(1412 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_ProcessVad_kAvgDecayTime         			(1413 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_int_0_addr                    			(1414 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define agc_const_int_1_addr                    			(1416 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define agc_const_int_10_addr                   			(1418 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define agc_const_int_39_addr                   			(1420 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define CONST_int_kLog10_addr                   			(1422 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define CONST_kLog10_addr                       			(1424 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kLog10sh1_addr                    			(1425 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kLog10_2_addr                     			(1426 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kLog10_2sh1_addr                  			(1427 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kLogE_1_addr                      			(1428 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kCompRatio_addr                   			(1429 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kCompRatio_m1_addr                			(1430 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_kSoftLimiterLeft_addr             			(1431 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_LinApprox_tmp16_1_addr            			(1432 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define CONST_LinApprox_tmp16_2_addr            			(1433 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define limiterOffset_addr                      			(1434 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_1_addr                        			(1435 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_2_addr                        			(1436 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_15_addr                       			(1437 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_9_addr                        			(1438 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_8_addr                        			(1439 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define Global_DownsampleBy2_kResampleAllpass2  			(1440 + AGC_GLOBAL_X_BEGIN_ADDR) //3 word
#define Global_DownsampleBy2_kResampleAllpass1  			(1446 + AGC_GLOBAL_X_BEGIN_ADDR) //3 word
#define Global_ProcessVad_Const_WORD32_MAX      			(1452 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define Global_ProcessVad_Const_0x00007fff      			(1454 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define Global_ProcessVad_Const_0xffff8000      			(1456 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define Global_ProcessVad_Const_0x0000b504      			(1458 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define apd_const_int_1_addr                    			(1460 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define apd_const_int_0x01000000_addr           			(1462 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define apd_const_int_0x7fffff_addr             			(1464 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define apd_const_int_0x2D413CC_addr            			(1466 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define apd_const_int_0x7f_addr                 			(1468 + AGC_GLOBAL_X_BEGIN_ADDR) //1 word
#define agc_const_8000_addr                     			(1470 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define agc_const_16000_addr                    			(1471 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define apd_const_0x0fff_addr                   			(1472 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short
#define apd_const_0xf000_addr                   			(1473 + AGC_GLOBAL_X_BEGIN_ADDR) //1 short

/*****************
 **  AGC_LOCAL  **
 *****************/

/******************
 **  AEC_GLOBAL  **
 ******************/

//GLOBAL_VARS
#define For_AEC_align                           			(0 + AEC_GLOBAL_X_BEGIN_ADDR) //816 short
#define PassThrough                             			(0) //alias
#define AdaptiveFilter                          			(1) //alias
#define AdaptiveFilterPostFilter                			(2) //alias
#define NoiseSuppression                        			(3) //alias
#define LowDelayOpenLoopAF                      			(4) //alias
#define TwoStepNoiseSuppression                 			(5) //alias
#define AdaptiveFilterPostFilterEnhanced        			(6) //alias
#define ECHO_COHF                               			(0) //alias
#define ECHO_TF                                 			(1) //alias
#define r_SpkAnaFilterDelayLine                 			(816 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_MicAnaFilterDelayLine                 			(944 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define r_OutSynFilterDelayLine                 			(1072 + AEC_GLOBAL_X_BEGIN_ADDR) //128 word
#define g_FilterBank_MulBuf                     			(1328 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define g_SpkAnaFilterOutBufI                   			(1456 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_SpkAnaFilterOutBufQ                   			(1472 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufI                   			(1488 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_MicAnaFilterOutBufQ                   			(1504 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufI                    			(1520 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define g_OutSynFilterInBufQ                    			(1536 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define r_AF_SpkSigBufI                         			(1552 + AEC_GLOBAL_X_BEGIN_ADDR) //72 short
#define r_AF_SpkSigBufQ                         			(1624 + AEC_GLOBAL_X_BEGIN_ADDR) //72 short
#define r_AF_FilterBufI                         			(1696 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_AF_FilterBufQ                         			(1840 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define g_AF_FilterOutBufI                      			(1984 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve00_addr                          			(1993 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_FilterOutBufQ                      			(1994 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve01_addr                          			(2003 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufI                     			(2004 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve02_addr                          			(2013 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_AF_OldSpkDataBufQ                     			(2014 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve03_addr                          			(2023 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufI                        			(2024 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve04_addr                          			(2033 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ErrDataBufQ                        			(2034 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve05_addr                          			(2043 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_AF_ShortFilterBufI                    			(2044 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_ShortFilterBufQ                    			(2052 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define g_AF_tempCmplxBufI                      			(2060 + AEC_GLOBAL_X_BEGIN_ADDR) //8 word
#define g_AF_tempCmplxBufQ                      			(2076 + AEC_GLOBAL_X_BEGIN_ADDR) //8 word
#define r_AF_SpkSigPower                        			(2092 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_SpkSigSmoothPower                  			(2110 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkI                           			(2128 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_AF_RErrSpkQ                           			(2146 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_AF_StepSize                           			(2164 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define reserve06_addr                          			(2173 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_SmoothErrPower                     			(2174 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoisePowerEstBuf                   			(2192 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_NoiseFloorDevCnt                   			(2210 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_DTD_ch                                			(2219 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_PostSToNRatio                      			(2220 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define CLChnl                                  			(2229 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_PrevSigPowerNBuf                   			(2230 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSigPowerEBuf                   			(2248 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_NoiseWeight                        			(2266 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_EC_ErrToOutRatio                      			(2275 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_PF_NoiseAmpWeight                     			(2276 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define m_AF_FrameCnt                           			(2285 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define m_Taps                                  			(2286 + AEC_GLOBAL_X_BEGIN_ADDR) //4 short
#define m_Registers                             			(2290 + AEC_GLOBAL_X_BEGIN_ADDR) //32 short
#define TEMP_r_AF_SpkSigBufIQ_addr_addr         			(2322 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_FilterBufIQ_addr_addr         			(2324 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_FilterOutBufIQ_addr_addr      			(2326 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_OldSpkDataBufIQ_addr_addr     			(2328 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_AF_ErrDataBufIQ_addr_addr        			(2330 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_MicAnaFilterOutBufIQ_addr_addr   			(2332 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_g_SpkAnaFilterOutBufIQ_addr_addr   			(2334 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_r_AF_SpkSigPower_addr_addr         			(2336 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define g_counter_subframe                      			(2338 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define g_ch                                    			(2339 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define real_int                                			(2340 + AEC_GLOBAL_X_BEGIN_ADDR) //16 word
#define imag_int                                			(2372 + AEC_GLOBAL_X_BEGIN_ADDR) //16 word
#define AnaFilterCoef                           			(2404 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define SynFilterCoef                           			(2532 + AEC_GLOBAL_X_BEGIN_ADDR) //128 short
#define wnreal                                  			(2660 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define wnimag                                  			(2668 + AEC_GLOBAL_X_BEGIN_ADDR) //8 short
#define AdaptTable                              			(2676 + AEC_GLOBAL_X_BEGIN_ADDR) //32 short
#define r_PF_CL_MicSmoothPower                  			(2708 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoise                        			(2712 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkSmoothPower                  			(2716 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_SpkNoise                        			(2720 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_MicNoiseFloorDevCnt             			(2724 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_SpkNoiseFloorDevCnt             			(2726 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifMicSigOn                      			(2728 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ifSpkSigOn                      			(2730 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_ChnlState                       			(2732 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define r_PF_CL_State                           			(2734 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_DTCountDown                     			(2735 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_NoisePowerSmoothEstBuf             			(2736 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_PriorSToNRatio                     			(2754 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define g_PF_PriorSToERatio                     			(2763 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_SpkPowerBuf                        			(2772 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CCErrSpkVecBufI                    			(2790 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_CCErrSpkVecBufQ                    			(2934 + AEC_GLOBAL_X_BEGIN_ADDR) //72 word
#define r_PF_EchoPowerEstBuf                    			(3078 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_EchoWeight                         			(3096 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_CL_DTCountUp                       			(3105 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_PF_CL_ErrSmoothPower                  			(3106 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_AF_FilterAmpI                         			(3110 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_AF_FilterAmpQ                         			(3119 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_PrevEchoPowerEst                   			(3128 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevSpkPower                       			(3146 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_CL_DTDXcorrMicErrI                 			(3164 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicErrQ                 			(3168 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoI                			(3172 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDXcorrMicEchoQ                			(3176 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicSmoothPower               			(3180 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDErrSmoothPower               			(3184 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDEchoSmoothPower              			(3188 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoisePower                			(3192 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDMicNoiseFloorDevCnt          			(3196 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTDDecision                     			(3200 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define r_PF_CL_DTD2ndDecision                  			(3204 + AEC_GLOBAL_X_BEGIN_ADDR) //2 word
#define pInstantSNR_t                           			(3208 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define pInstantSNR_f                           			(3209 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define SumMicPower                             			(3210 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumSpkPower                             			(3212 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define SumErrPower                             			(3214 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define PFCLDTDOut                              			(3216 + AEC_GLOBAL_X_BEGIN_ADDR) //2 short
#define g_PF_pSmoothErrPowerMin1                			(3218 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_pSmoothErrPowerMin2                			(3236 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define g_PF_SmoothErrPowerSub1                 			(3254 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define Temp_gsz                                			(3256 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define m_PF_NoiseGainLimitAdj                  			(3258 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define m_PF_NoiseGainLimitAdj_reserve          			(3259 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_NON_CLIP_AEC_SPK_HISTORYDATA     			(3260 + AEC_GLOBAL_X_BEGIN_ADDR) //160 short
#define AEC_CONST_0x00007fff                    			(3420 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define AEC_CONST_0xffff8000                    			(3422 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define r_PF_pSmoothErrPowerResBuf              			(3424 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_pSmoothSpkPowerResBuf              			(3442 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevResEchoErrPowerBuf             			(3460 + AEC_GLOBAL_X_BEGIN_ADDR) //9 word
#define r_PF_PrevResEchoAttErrdBBuf             			(3478 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_PrevResEchoWorkErrdBBuf            			(3487 + AEC_GLOBAL_X_BEGIN_ADDR) //9 short
#define r_PF_SmoothErrPowerSub1Res              			(3496 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define r_PF_SmoothSpkPowerSub1Res              			(3498 + AEC_GLOBAL_X_BEGIN_ADDR) //1 word
#define r_PF_SpkSubErrdBSub1Res                 			(3500 + AEC_GLOBAL_X_BEGIN_ADDR) //1 short
#define r_HF_ROM                                			(3501 + AEC_GLOBAL_X_BEGIN_ADDR) //16 short
#define r_Log2Lin_HF_ROM                        			(3517 + AEC_GLOBAL_X_BEGIN_ADDR) //32 short

/*****************
 **  AEC_LOCAL  **
 *****************/

/*****************
 **  FR_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define STATIC_ENC_GSM_STATE_ADDR               			(0 + FR_GLOBAL_X_BEGIN_ADDR) //280 short
#define STATIC_ENC_GSM_STATE_Z1_ADDR            			(280 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_L_Z2_ADDR          			(282 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_MP_ADDR            			(284 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_ENC_GSM_STATE_U_ADDR             			(286 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_ENC_GSM_STATE_LARPP_ADDR         			(294 + FR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_ENC_GSM_STATE_J_ADDR             			(310 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_NRP_ADDR           			(311 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_V_ADDR             			(312 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_ENC_GSM_STATE_MSR_ADDR           			(321 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_VERBOSE_ADDR       			(322 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_ENC_GSM_STATE_FAST_ADDR          			(323 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ENC_LARC_ADDR                    			(324 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_ENC_NC_ADDR                      			(332 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_MC_ADDR                      			(336 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_BC_ADDR                      			(340 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_XMAXC_ADDR                   			(344 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_ENC_XMC_ADDR                     			(348 + FR_GLOBAL_X_BEGIN_ADDR) //52 short
#define STATIC_DEC_GSM_STATE_DP0_ADDR           			(400 + FR_GLOBAL_X_BEGIN_ADDR) //280 short
#define STATIC_DEC_GSM_STATE_Z1_ADDR            			(680 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_L_Z2_ADDR          			(682 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_MP_ADDR            			(684 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DEC_GSM_STATE_U_ADDR             			(686 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_DEC_GSM_STATE_LARPP_ADDR         			(694 + FR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_DEC_GSM_STATE_J_ADDR             			(710 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_NRP_ADDR           			(711 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_V_ADDR             			(712 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_DEC_GSM_STATE_MSR_ADDR           			(721 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_VERBOSE_ADDR       			(722 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DEC_GSM_STATE_FAST_ADDR          			(723 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DEC_LARC_ADDR                    			(724 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_DEC_NC_ADDR                      			(732 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_MC_ADDR                      			(736 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_BC_ADDR                      			(740 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_XMAXC_ADDR                   			(744 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DEC_XMC_ADDR                     			(748 + FR_GLOBAL_X_BEGIN_ADDR) //52 short
#define STATIC_VAD_pswRvad_ADDR                 			(800 + FR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STATIC_VAD_tone_ADDR                    			(809 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swNormRvad_ADDR              			(810 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swPt_sacf_ADDR               			(811 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swPt_sav0_ADDR               			(812 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swE_thvad_ADDR               			(813 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swM_thvad_ADDR               			(814 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swAdaptCount_ADDR            			(815 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swBurstCount_ADDR            			(816 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swHangCount_ADDR             			(817 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swOldLagCount_ADDR           			(818 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swVeryOldLagCount_ADDR       			(819 + FR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_VAD_swOldLag_ADDR                			(820 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_VAD_pL_sacf_ADDR                 			(822 + FR_GLOBAL_X_BEGIN_ADDR) //54 short
#define STATIC_VAD_pL_sav0_ADDR                 			(876 + FR_GLOBAL_X_BEGIN_ADDR) //72 short
#define STATIC_VAD_L_lastdm_ADDR                			(948 + FR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_VAD_SCALVAD_ADDR                 			(950 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_DLB_ADDR               			(954 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_QLB_ADDR               			(958 + FR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_CONST_GSM_NRFAC_ADDR             			(962 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_CONST_GSM_FAC_ADDR               			(970 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define DEC_HOMING_LPC_PARM_ADDR                			(978 + FR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_CONST_GSM_H_ADDR                 			(986 + FR_GLOBAL_X_BEGIN_ADDR) //12 short
#define STATIC_TABLE_HANN_ADDR                  			(998 + FR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STATIC_CONST_INTERP_FACTOR_FR_ADDR      			(1078 + FR_GLOBAL_X_BEGIN_ADDR) //24 short

//GLOBAL_VARS
#define STATIC_ENC_E_ADDR                       			(0 + FR_GLOBAL_Y_BEGIN_ADDR) //50 short
#define GLOBAL_dtx_begin_BLANK                  			(50 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_subfrm_ADDR                      			(51 + FR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_txdtx_ctrl_ADDR                  			(53 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_txdtx_hangover_ADDR              			(54 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_txdtx_N_elapsed_ADDR             			(55 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_lost_speech_frames_ADDR          			(56 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_aver_period_ADDR           			(57 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_N_elapsed_ADDR             			(58 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rxdtx_ctrl_ADDR                  			(59 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_rx_dtx_state_ADDR                			(60 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_prev_SID_frames_lost_ADDR        			(61 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_L_pn_seed_rx_ADDR                			(62 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_LAR_SID_tx_ADDR                  			(70 + FR_GLOBAL_Y_BEGIN_ADDR) //32 short
#define GLOBAL_xmax_SID_tx_ADDR                 			(102 + FR_GLOBAL_Y_BEGIN_ADDR) //16 short
#define GLOBAL_LARc_old_tx_ADDR                 			(118 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_tx_ADDR                			(126 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_LARc_old_rx_ADDR                 			(130 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_Nc_old_rx_ADDR                   			(138 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_Mc_old_rx_ADDR                   			(142 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_bc_old_rx_ADDR                   			(146 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmaxc_old_rx_ADDR                			(150 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmc_old_rx_ADDR                  			(154 + FR_GLOBAL_Y_BEGIN_ADDR) //52 short
#define GLOBAL_LARc_old_2_rx_ADDR               			(206 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_2_rx_ADDR              			(214 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_LARc_old_SID_rx_ADDR             			(218 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_LARc_old_2_SID_rx_ADDR           			(226 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_xmaxc_old_SID_rx_ADDR            			(234 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define GLOBAL_xmaxc_old_2_SID_rx_ADDR          			(238 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define VAD_ALGORITHM_swStat_ADDR               			(242 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swNormRav1_ADDR           			(243 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_pswRav1_ADDR              			(244 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define VAD_ALGORITHM_pL_av0_ADDR               			(254 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_ALGORITHM_pL_av1_ADDR               			(272 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_ALGORITHM_swM_pvad_ADDR             			(290 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swE_pvad_ADDR             			(291 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swM_acf0_ADDR             			(292 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_ALGORITHM_swE_acf0_ADDR             			(293 + FR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define VAD_PREDICTOR_VALUES_pswAav1_ADDR       			(294 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define VAD_PREDICTOR_VALUES_pswVpar_ADDR       			(304 + FR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define VAD_STEP_UP_pL_coef_ADDR                			(312 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_STEP_UP_pL_work_ADDR                			(330 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_COMPUTE_RAV1_pL_work_ADDR           			(348 + FR_GLOBAL_Y_BEGIN_ADDR) //18 short
#define VAD_TONE_DETECTION_pswA_ADDR            			(366 + FR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define SCHUR_RECURSION_pswPp_ADDR              			(370 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define SCHUR_RECURSION_pswKk_ADDR              			(380 + FR_GLOBAL_Y_BEGIN_ADDR) //10 short

/****************
 **  FR_LOCAL  **
 ****************/

//CII_Gsm_Coder
#define CII_Gsm_Coder_X_BEGIN_ADDR              			(0 + FR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_ENC_SO_ADDR                       			(0 + CII_Gsm_Coder_X_BEGIN_ADDR) //160 short
#define LOCAL_ENC_L_ACF_ADDR                    			(160 + CII_Gsm_Coder_X_BEGIN_ADDR) //18 short
#define LOCAL_ENC_X_ADDR                        			(178 + CII_Gsm_Coder_X_BEGIN_ADDR) //40 short
#define LOCAL_ENC_XM_ADDR                       			(218 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_ENC_XMP_ADDR                      			(232 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_DEC_XMP_ADDR                      			(246 + CII_Gsm_Coder_X_BEGIN_ADDR) //14 short
#define LOCAL_ENC_ACF_ADDR                      			(260 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short
#define LOCAL_ENC_P_ADDR                        			(270 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short
#define LOCAL_ENC_K_ADDR                        			(280 + CII_Gsm_Coder_X_BEGIN_ADDR) //10 short

//CII_Gsm_Coder
#define CII_Gsm_Coder_Y_BEGIN_ADDR              			(0 + FR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_ENC_WT_ADDR                       			(0 + CII_Gsm_Coder_Y_BEGIN_ADDR) //40 short
#define LOCAL_ENC_LARP_ADDR                     			(40 + CII_Gsm_Coder_Y_BEGIN_ADDR) //100 short

//CII_vad_computation_fr
#define CII_vad_computation_fr_X_BEGIN_ADDR     			(290 + FR_LOCAL_X_BEGIN_ADDR)
#define BIT_PARA_TEMP_ADDR                      			(0 + CII_vad_computation_fr_X_BEGIN_ADDR) //500 short

//CII_Gsm_Decoder
#define CII_Gsm_Decoder_X_BEGIN_ADDR            			(0 + FR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_DEC_WT_ADDR                       			(0 + CII_Gsm_Decoder_X_BEGIN_ADDR) //160 short
#define LOCAL_DEC_ERP_ADDR                      			(160 + CII_Gsm_Decoder_X_BEGIN_ADDR) //40 short
#define LOCAL_VAD_L_ACFH_ADDR                   			(200 + CII_Gsm_Decoder_X_BEGIN_ADDR) //10 short
#define LOCAL_VAD_SAV0_ADDR                     			(210 + CII_Gsm_Decoder_X_BEGIN_ADDR) //100 short

//CII_Gsm_Decoder
#define CII_Gsm_Decoder_Y_BEGIN_ADDR            			(0 + FR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_DEC_LARP_ADDR                     			(0 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //196 short
#define LOCAL_SOF_Y_ADDR                        			(196 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //160 short
#define LOCAL_VAD_SACF_Y_ADDR                   			(356 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //6 short
#define LOCAL_VAD_K_Y_ADDR                      			(362 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //6 short
#define LOCAL_VAD_RC_Y_ADDR                     			(368 + CII_Gsm_Decoder_Y_BEGIN_ADDR) //100 short

/*****************
 **  HR_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define HR_RESERVED1                            			(0 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_GLOBAL_BEGIN                         			(HR_RESERVED1) //alias
#define GLOBAL_DTX_swVadFrmCnt_ADDR             			(1 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_siUpdPointer_ADDR            			(2 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_swNElapsed_ADDR              			(3 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_pL_GsHist_ADDR               			(4 + HR_GLOBAL_X_BEGIN_ADDR) //56 short
#define STATIC_DTX_pL_R0Hist_ADDR               			(60 + HR_GLOBAL_X_BEGIN_ADDR) //16 short
#define STATIC_DTX_ppL_CorrHist_ADDR            			(76 + HR_GLOBAL_X_BEGIN_ADDR) //176 short
#define STATIC_DTX_swQntRefR0_ADDR              			(252 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_swRefGsIndex_ADDR            			(253 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_piRefVqCodewds_ADDR          			(254 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STATIC_DTX_swShortBurst_ADDR            			(258 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DECODER_resetflag_ADDR                  			(259 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_L_TxPNSeed_ADDR              			(260 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_DTX_swR0OldCN_ADDR               			(262 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED2                            			(263 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DTX_pL_OldCorrSeq_ADDR           			(264 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DTX_pL_NewCorrSeq_ADDR           			(286 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DTX_pL_CorrSeq_ADDR              			(308 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define GLOBAL_ERR_plSubfrEnergyMem_ADDR        			(330 + HR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_ERR_swLevelMem_ADDR              			(338 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DE_swDecoMode_ADDR               			(342 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DECODER_resetflag_old_ADDR              			(343 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ERR_swState_ADDR                 			(344 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_DE_swRxDTXState_ADDR         			(345 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED5                            			(346 + HR_GLOBAL_X_BEGIN_ADDR) //3 short
#define GLOBAL_DE_giSfrmCnt_ADDR                			(349 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DE_pswSpeechPara_ADDR            			(350 + HR_GLOBAL_X_BEGIN_ADDR) //22 short
#define STATIC_DE_pswDecodedSpeechFrame_ADDR    			(372 + HR_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_swPreBfi_ADDR                    			(532 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_swOldR0_ADDR                     			(533 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_pswSpeechPara_ADDR            			(534 + HR_GLOBAL_X_BEGIN_ADDR) //20 short
#define STATIC_EN_pswSpeechBuff_ADDR            			(554 + HR_GLOBAL_X_BEGIN_ADDR) //160 short
#define GLOBAL_ERR_pswLastGood_ADDR             			(714 + HR_GLOBAL_X_BEGIN_ADDR) //18 short
#define GLOBAL_DE_gswPostFiltAgcGain_ADDR       			(732 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_siPrevLagCode_ADDR   			(733 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_gpswPostFiltStateNum_ADDR     			(734 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_gpswPostFiltStateDenom_ADDR   			(744 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_swPostEmphasisState_ADDR      			(754 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pswSynthFiltState_ADDR        			(756 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmPFDenom_ADDR         			(766 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmKsDec_ADDR           			(776 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmAsDec_ADDR           			(786 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_pswOldFrmPFNum_ADDR           			(796 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_DE_swOldR0Dec_ADDR               			(806 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pswLtpStateBaseDec_ADDR       			(808 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_DE_pswPPreState_ADDR             			(996 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_DE_swMuteFlagOld_ADDR            			(1184 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ERR_swLastFlag_ADDR              			(1185 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_ERR_lastR0_ADDR                  			(1186 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swDtxMuting_ADDR              			(1187 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swDtxBfiCnt_ADDR              			(1188 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swOldR0IndexDec_ADDR          			(1189 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_swRxGsHistPtr_ADDR            			(1190 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_DE_pL_RxGsHist_ADDR              			(1192 + HR_GLOBAL_X_BEGIN_ADDR) //56 short
#define GLOBAL_DE_SP_swR0Dec_ADDR               			(1248 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swVoicingMode_ADDR         			(1249 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_pswVq_ADDR                 			(1250 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DE_SP_swSi_ADDR                  			(1254 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swEngyRShift_ADDR          			(1255 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DE_SP_swR0NewCN_ADDR             			(1256 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_DE_SP_swLastLag_ADDR             			(1257 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_swOldR0_ADDR                  			(1258 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_swOldR0Index_ADDR             			(1259 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_EN_psnsWSfrmEngSpace_ADDR        			(1260 + HR_GLOBAL_X_BEGIN_ADDR) //16 short
#define GLOBAL_EN_pswHPFXState_ADDR             			(1276 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_EN_pswHPFYState_ADDR             			(1280 + HR_GLOBAL_X_BEGIN_ADDR) //8 short
#define GLOBAL_EN_pswOldFrmKs_ADDR              			(1288 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswOldFrmAs_ADDR              			(1298 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswOldFrmSNWCoefs_ADDR        			(1308 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_pswWgtSpeechSpace_ADDR        			(1318 + HR_GLOBAL_X_BEGIN_ADDR) //306 short
#define GLOBAL_EN_pswSpeech_ADDR                			(1624 + HR_GLOBAL_X_BEGIN_ADDR) //196 short
#define GLOBAL_EN_swPtch_ADDR                   			(1820 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_TX_swTxGsHistPtr_ADDR        			(1821 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_DTX_TX_pswCNVSCode1_ADDR         			(1822 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNVSCode2_ADDR         			(1826 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNGsp0Code_ADDR        			(1830 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_pswCNLpc_ADDR             			(1834 + HR_GLOBAL_X_BEGIN_ADDR) //4 short
#define GLOBAL_DTX_TX_swCNR0_ADDR               			(1838 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_EN_SP_FRM_pswAnalysisState_ADDR  			(1840 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_SP_FRM_pswWStateNum_ADDR      			(1850 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_EN_SP_FRM_pswWStateDenom_ADDR    			(1860 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STATIC_EN_SP_FRM_psrTable_ADDR_ADDR     			(1870 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_RESERVED3                            			(1871 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iLow_ADDR              			(1872 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iThree_ADDR            			(1873 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iWordHalfPtr_ADDR      			(1874 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STATIC_EN_SP_FRM_iWordPtr_ADDR          			(1875 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_pswRvad_ADDR              			(1876 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define HR_STATIC_VAD_swNormRvad_ADDR           			(1886 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swPt_sacf_ADDR            			(1887 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swPt_sav0_ADDR            			(1888 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swE_thvad_ADDR            			(1889 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swM_thvad_ADDR            			(1890 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swAdaptCount_ADDR         			(1891 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swBurstCount_ADDR         			(1892 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swHangCount_ADDR          			(1893 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swOldLagCount_ADDR        			(1894 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swVeryOldLagCount_ADDR    			(1895 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define HR_STATIC_VAD_swOldLag_ADDR             			(1896 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define HR_STATIC_VAD_pL_sacf_ADDR              			(1898 + HR_GLOBAL_X_BEGIN_ADDR) //54 short
#define HR_STATIC_VAD_pL_sav0_ADDR              			(1952 + HR_GLOBAL_X_BEGIN_ADDR) //72 short
#define HR_STATIC_VAD_L_lastdm_ADDR             			(2024 + HR_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_SFRM_pswLtpStateBase_ADDR        			(2026 + HR_GLOBAL_X_BEGIN_ADDR) //188 short
#define GLOBAL_SFRM_pswHState_ADDR              			(2214 + HR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_SFRM_pswHNWState_ADDR            			(2224 + HR_GLOBAL_X_BEGIN_ADDR) //145 short
#define HR_RESERVED4                            			(2369 + HR_GLOBAL_X_BEGIN_ADDR) //1 short
#define BITS_SHORT_ADDR                         			(2370 + HR_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
#define DEC_pswSythAsSpace_ADDR                 			(0 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_pswPFNumAsSpace_ADDR                			(40 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_pswPFDenomAsSpace_ADDR              			(80 + HR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define DEC_L_RxPNSeed_ADDR                     			(120 + HR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define DEC_swRxDtxGsIndex_ADDR                 			(122 + HR_GLOBAL_Y_BEGIN_ADDR) //2 short

/****************
 **  HR_LOCAL  **
 ****************/

//CII_speechEncoder
#define CII_speechEncoder_X_BEGIN_ADDR          			(0 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_swR0_ADDR        			(0 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define MIC_FILT_ADDR                           			(LOCAL_EN_SPEECHENCODER_swR0_ADDR) //alias
#define LOCAL_ECHO_CANCEL_SAMPLS_ADDR           			(MIC_FILT_ADDR) //alias
#define LOCAL_EN_SPEECHENCODER_iR0_ADDR         			(1 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_swVadFlag_ADDR   			(2 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_swSP_ADDR        			(3 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_pswFrmKs_ADDR    			(4 + CII_speechEncoder_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswVadLags_ADDR  			(14 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswLagList_ADDR  			(18 + CII_speechEncoder_X_BEGIN_ADDR) //12 short
#define LOCAL_EN_SPEECHENCODER_piVq_ADDR        			(30 + CII_speechEncoder_X_BEGIN_ADDR) //3 short
#define LOCAL_EN_SPEECHENCODER_siUVCode_ADDR    			(33 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_piVSCode1_ADDR   			(34 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piVSCode2_ADDR   			(38 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piGsp0Code_ADDR  			(42 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_piLagCode_ADDR   			(46 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswNumLagList_ADDR			(50 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswPitchBuf_ADDR 			(54 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswHNWCoefBuf_ADDR			(58 + CII_speechEncoder_X_BEGIN_ADDR) //4 short
#define LOCAL_EN_SPEECHENCODER_pswFrmSNWCoefs_ADDR			(62 + CII_speechEncoder_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_ppswSNWCoefAs_ADDR			(72 + CII_speechEncoder_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_iSi_ADDR         			(112 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_iVoicing_ADDR    			(113 + CII_speechEncoder_X_BEGIN_ADDR) //1 short
#define LOCAL_EN_SPEECHENCODER_psnsSqrtRs_ADDR  			(114 + CII_speechEncoder_X_BEGIN_ADDR) //8 short
#define LOCAL_EN_SPEECHENCODER_pswLagListPtr_ADDR_ADDR			(122 + CII_speechEncoder_X_BEGIN_ADDR) //2 short

//CII_speechEncoder
#define CII_speechEncoder_Y_BEGIN_ADDR          			(0 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_pswFrmAs_ADDR    			(0 + CII_speechEncoder_Y_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_ppswSynthAs_ADDR 			(10 + CII_speechEncoder_Y_BEGIN_ADDR) //40 short

//CII_iir_d_opt
#define CII_iir_d_opt_X_BEGIN_ADDR              			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define CII_iir_d_temp                          			(0 + CII_iir_d_opt_X_BEGIN_ADDR) //10 short

//CII_aflat_opt
#define CII_aflat_opt_X_BEGIN_ADDR              			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_AFLAT_pL_VadAcf_ADDR              			(0 + CII_aflat_opt_X_BEGIN_ADDR) //18 short
#define LOCAL_AFLAT_pswFlatsRc_ADDR             			(18 + CII_aflat_opt_X_BEGIN_ADDR) //10 short
#define LOCAL_AFLAT_pL_CorrelSeq_ADDR           			(28 + CII_aflat_opt_X_BEGIN_ADDR) //22 short
#define LOCAL_AFLAT_pswVadRc_ADDR               			(50 + CII_aflat_opt_X_BEGIN_ADDR) //6 short
#define LOCAL_AFLAT_swVadScalAuto_ADDR          			(56 + CII_aflat_opt_X_BEGIN_ADDR) //10 short

//CII_speechEncoder_inner_0
#define CII_speechEncoder_inner_0_X_BEGIN_ADDR  			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_pswCoefTmp2_ADDR 			(0 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswCoefTmp3_ADDR 			(10 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pswVecTmp_ADDR   			(20 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_pswVecTmp2_ADDR  			(60 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //40 short
#define LOCAL_EN_SPEECHENCODER_L_Temp_ADDR      			(100 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //2 short
#define LOCAL_EN_SPEECHENCODER_pL_AutoCorTmp_ADDR			(102 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //22 short
#define LOCAL_EN_SPEECHENCODER_pswTempRc_ADDR   			(124 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_EN_SPEECHENCODER_pL_A_ADDR        			(134 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //20 short
#define LOCAL_EN_SPEECHENCODER_pL_tmp_ADDR      			(154 + CII_speechEncoder_inner_0_X_BEGIN_ADDR) //20 short

//CII_aFlatRcDp_opt
#define CII_aFlatRcDp_opt_Y_BEGIN_ADDR          			(324 + HR_LOCAL_Y_BEGIN_ADDR)
#define AFLATRCDP_pL_pjNewSpace_ADDR            			(0 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //20 short
#define AFLATRCDP_pL_pjOldSpace_ADDR            			(20 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //20 short
#define AFLATRCDP_pL_vjNewSpace_ADDR            			(40 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //38 short
#define AFLATRCDP_pL_vjOldSpace_ADDR            			(78 + CII_aFlatRcDp_opt_Y_BEGIN_ADDR) //38 short

//CII_getSfrmLpcTx_opt
#define CII_getSfrmLpcTx_opt_X_BEGIN_ADDR       			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_EN_SPEECHENCODER_interpolateCheck_pswRcTemp_ADDR			(0 + CII_getSfrmLpcTx_opt_X_BEGIN_ADDR) //10 short
#define LOCAL_compResidEnergy_pswResidual_ADDR  			(10 + CII_getSfrmLpcTx_opt_X_BEGIN_ADDR) //40 short

//CII_getSfrmLpcTx_opt
#define CII_getSfrmLpcTx_opt_Y_BEGIN_ADDR       			(282 + HR_LOCAL_Y_BEGIN_ADDR)
#define cov32_copy_ADDR                         			(0 + CII_getSfrmLpcTx_opt_Y_BEGIN_ADDR) //10 short

//CII_speechEncoder_inner_1
#define CII_speechEncoder_inner_1_X_BEGIN_ADDR  			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define weightSpeechFrame_pswScratch0_ADDR      			(0 + CII_speechEncoder_inner_1_X_BEGIN_ADDR) //305 short

//CII_lpcIir_opt_paris
#define CII_lpcIir_opt_paris_Y_BEGIN_ADDR       			(282 + HR_LOCAL_Y_BEGIN_ADDR)
#define TEMP_COPY_ADDR_Y                        			(0 + CII_lpcIir_opt_paris_Y_BEGIN_ADDR) //10 short

//CII_openLoopLagSearch_new
#define CII_openLoopLagSearch_new_X_BEGIN_ADDR  			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_OPENLOOPSEARCH_pswGFrame_ADDR     			(0 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //248 short
#define LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBuffer_ADDR			(248 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //306 short
#define LOCAL_OPENLOOPSEARCH_pswLPeaks_ADDR     			(554 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswCPeaks_ADDR     			(568 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswGPeaks_ADDR     			(582 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //14 short
#define LOCAL_OPENLOOPSEARCH_pswLArray_ADDR     			(596 + CII_openLoopLagSearch_new_X_BEGIN_ADDR) //16 short

//CII_openLoopLagSearch_new
#define CII_openLoopLagSearch_new_Y_BEGIN_ADDR  			(50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_OPENLOOPSEARCH_pswCFrame_ADDR     			(0 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //512 short
#define LOCAL_OPENLOOPSEARCH_pswSfrmEng_ADDR    			(512 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define LOCAL_OPENLOOPSEARCH_pswLIntBuf_ADDR    			(516 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define LOCAL_OPENLOOPSEARCH_pswCCThresh_ADDR   			(520 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //4 short
#define OPENLOOP_SP32_SAVE_ADDR                 			(524 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //1 short
#define OPENLOOP_SP16_SAVE_ADDR                 			(525 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //1 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajLBuf_ADDR  			(526 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajCCBuf_ADDR 			(558 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_ppswTrajGBuf_ADDR  			(590 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //32 short
#define LOCAL_OPENLOOPSEARCH_pswScaledWSpeechBufferCopy_ADDR			(622 + CII_openLoopLagSearch_new_Y_BEGIN_ADDR) //306 short

//CII_sfrmAnalysis_opt
#define CII_sfrmAnalysis_opt_X_BEGIN_ADDR       			(124 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_sfrmAnalysis_pswPVec_ADDR         			(0 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswWPVec_ADDR        			(40 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswWBasisVecs_ADDR   			(80 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //360 short
#define LOCAL_sfrmAnalysis_ppswWVselpEx_ADDR    			(440 + CII_sfrmAnalysis_opt_X_BEGIN_ADDR) //80 short

//CII_sfrmAnalysis_opt
#define CII_sfrmAnalysis_opt_Y_BEGIN_ADDR       			(50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_sfrmAnalysis_pswTempVec_ADDR      			(0 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //40 short
#define LOCAL_sfrmAnalysis_pswBitArray_ADDR     			(40 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //10 short
#define LOCAL_sfrmAnalysis_pswHNWCoefs_ADDR     			(50 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //6 short
#define LOCAL_sfrmAnalysis_pswWSVec_ADDR        			(56 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //40 short
#define sfrmAnalysis_psiVSCode1_ADDR_ADDR       			(96 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiVSCode2_ADDR_ADDR       			(97 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiGsp0Code_ADDR_ADDR      			(98 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_psiLagCode_ADDR_ADDR       			(99 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_swLag_ADDR           			(100 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_swLtpShift_ADDR      			(101 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_snsRs00_ADDR         			(102 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define LOCAL_sfrmAnalysis_snsRs11_ADDR         			(104 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define LOCAL_sfrmAnalysis_snsRs22_ADDR         			(106 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //2 short
#define sfrmAnalysis_SP16_SAVE0_ADDR            			(108 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define sfrmAnalysis_SP16_SAVE1_ADDR            			(109 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_sfrmAnalysis_ppswVselpEx_ADDR     			(110 + CII_sfrmAnalysis_opt_Y_BEGIN_ADDR) //80 short

//CII_flat_opt
#define CII_flat_opt_X_BEGIN_ADDR               			(190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_FLAT_pppL_B_ADDR                  			(0 + CII_flat_opt_X_BEGIN_ADDR) //400 short
#define LOCAL_pswInScale_ADDR                   			(400 + CII_flat_opt_X_BEGIN_ADDR) //170 short
#define LOCAL_pL_Phi_ADDR                       			(570 + CII_flat_opt_X_BEGIN_ADDR) //22 short

//CII_flat_opt
#define CII_flat_opt_Y_BEGIN_ADDR               			(50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_FLAT_pppL_C_ADDR                  			(0 + CII_flat_opt_Y_BEGIN_ADDR) //400 short
#define LOCAL_FLAT_pppL_F_ADDR                  			(400 + CII_flat_opt_Y_BEGIN_ADDR) //400 short

//CII_rcToCorrDpL_opt
#define CII_rcToCorrDpL_opt_X_BEGIN_ADDR        			(190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_rcToCorrDpL_pL_tmpSpace_ADDR      			(0 + CII_rcToCorrDpL_opt_X_BEGIN_ADDR) //20 short
#define LOCAL_rcToCorrDpL_pL_ASpace_ADDR        			(20 + CII_rcToCorrDpL_opt_X_BEGIN_ADDR) //20 short

//CII_vad_algorithm
#define CII_vad_algorithm_Y_BEGIN_ADDR          			(50 + HR_LOCAL_Y_BEGIN_ADDR)
#define HR_VAD_ALGORITHM_swStat_ADDR            			(0 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swNormRav1_ADDR        			(1 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_pswRav1_ADDR           			(2 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_VAD_ALGORITHM_pL_av0_ADDR            			(12 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_ALGORITHM_pL_av1_ADDR            			(30 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_ALGORITHM_swM_pvad_ADDR          			(48 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swE_pvad_ADDR          			(49 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swM_acf0_ADDR          			(50 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_ALGORITHM_swE_acf0_ADDR          			(51 + CII_vad_algorithm_Y_BEGIN_ADDR) //1 short
#define HR_VAD_PREDICTOR_VALUES_pswAav1_ADDR    			(52 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_VAD_PREDICTOR_VALUES_pswVpar_ADDR    			(62 + CII_vad_algorithm_Y_BEGIN_ADDR) //8 short
#define HR_VAD_STEP_UP_pL_coef_ADDR             			(70 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_STEP_UP_pL_work_ADDR             			(88 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_COMPUTE_RAV1_pL_work_ADDR        			(106 + CII_vad_algorithm_Y_BEGIN_ADDR) //18 short
#define HR_VAD_TONE_DETECTION_pswA_ADDR         			(124 + CII_vad_algorithm_Y_BEGIN_ADDR) //4 short
#define HR_SCHUR_RECURSION_pswPp_ADDR           			(128 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short
#define HR_SCHUR_RECURSION_pswKk_ADDR           			(138 + CII_vad_algorithm_Y_BEGIN_ADDR) //10 short

//CII_aflat_opt_inner_0
#define CII_aflat_opt_inner_0_X_BEGIN_ADDR      			(190 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_SWCOMFORTNOISE_pswFinalRc_ADDR    			(0 + CII_aflat_opt_inner_0_X_BEGIN_ADDR) //10 short
#define LOCAL_SWCOMFORTNOISE_pL_RefCorr_ADDR    			(10 + CII_aflat_opt_inner_0_X_BEGIN_ADDR) //22 short

//CII_lpcCorrQntz_opt
#define CII_lpcCorrQntz_opt_X_BEGIN_ADDR        			(222 + HR_LOCAL_X_BEGIN_ADDR)
#define LPCCORRQNTZ_pswPOldSpace_ADDR           			(0 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVOldSpace_ADDR           			(4 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //7 short
#define LPCCORRQNTZ_pswPNewSpace_ADDR           			(11 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVNewSpace_ADDR           			(15 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //7 short
#define LPCCORRQNTZ_pswPBar_ADDR                			(22 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //4 short
#define LPCCORRQNTZ_pswVBarSpace_ADDR           			(26 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //8 short
#define LPCCORRQNTZ_pswRc_ADDR                  			(34 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //12 short
#define LPCCORRQNTZ_pL_PBarFull_ADDR            			(46 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //20 short
#define LPCCORRQNTZ_pL_VBarFullSpace_ADDR       			(66 + CII_lpcCorrQntz_opt_X_BEGIN_ADDR) //38 short

//CII_lpcCorrQntz_opt
#define CII_lpcCorrQntz_opt_Y_BEGIN_ADDR        			(50 + HR_LOCAL_Y_BEGIN_ADDR)
#define LPCCORRQNTZ_quantList_ADDR              			(0 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //66 short
#define LPCCORRQNTZ_bestPql_ADDR                			(66 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //264 short
#define LPCCORRQNTZ_bestQl_ADDR                 			(330 + CII_lpcCorrQntz_opt_Y_BEGIN_ADDR) //264 short

//CII_aToRc_opt2
#define CII_aToRc_opt2_X_BEGIN_ADDR             			(174 + HR_LOCAL_X_BEGIN_ADDR)
#define aToRc_pswASpace_ADDR                    			(0 + CII_aToRc_opt2_X_BEGIN_ADDR) //10 short

//CII_aToRc_opt2
#define CII_aToRc_opt2_Y_BEGIN_ADDR             			(324 + HR_LOCAL_Y_BEGIN_ADDR)
#define aToRc_pswTmp_ADDR                       			(0 + CII_aToRc_opt2_Y_BEGIN_ADDR) //10 short

//CII_pitchLags_new
#define CII_pitchLags_new_X_BEGIN_ADDR          			(736 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_PITCHLAGS_pswLBuf_ADDR            			(0 + CII_pitchLags_new_X_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswCBuf_ADDR            			(12 + CII_pitchLags_new_X_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswLPeaks_ADDR          			(24 + CII_pitchLags_new_X_BEGIN_ADDR) //14 short
#define LOCAL_PITCHLAGS_pswCPeaks_ADDR          			(38 + CII_pitchLags_new_X_BEGIN_ADDR) //14 short

//CII_pitchLags_new
#define CII_pitchLags_new_Y_BEGIN_ADDR          			(978 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_PITCHLAGS_pswGBuf_ADDR            			(0 + CII_pitchLags_new_Y_BEGIN_ADDR) //12 short
#define LOCAL_PITCHLAGS_pswGPeaks_ADDR          			(12 + CII_pitchLags_new_Y_BEGIN_ADDR) //14 short
#define PITCHLAG_SP32_SAVE_ADDR                 			(26 + CII_pitchLags_new_Y_BEGIN_ADDR) //1 short
#define PITCHLAG_SP16_SAVE_ADDR                 			(27 + CII_pitchLags_new_Y_BEGIN_ADDR) //1 short

//CII_bestDelta_new
#define CII_bestDelta_new_X_BEGIN_ADDR          			(736 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_BESTDELTA_pswCBuf_ADDR            			(0 + CII_bestDelta_new_X_BEGIN_ADDR) //24 short
#define LOCAL_BESTDELTA_pswCInterp_ADDR         			(24 + CII_bestDelta_new_X_BEGIN_ADDR) //18 short

//CII_bestDelta_new
#define CII_bestDelta_new_Y_BEGIN_ADDR          			(978 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_BESTDELTA_pswGBuf_ADDR            			(0 + CII_bestDelta_new_Y_BEGIN_ADDR) //24 short
#define LOCAL_BESTDELTA_pswGInterp_ADDR         			(24 + CII_bestDelta_new_Y_BEGIN_ADDR) //18 short

//CII_sfrmAnalysis_opt_inner_0
#define CII_sfrmAnalysis_opt_inner_0_X_BEGIN_ADDR			(644 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_closedLoopLagSearch_ppswTVect_ADDR			(0 + CII_sfrmAnalysis_opt_inner_0_X_BEGIN_ADDR) //160 short

//CII_sfrmAnalysis_opt_inner_0
#define CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR			(240 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_closedLoopLagSearch_pL_CCBuf_ADDR 			(0 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //12 short
#define LOCAL_closedLoopLagSearch_pL_CGBuf_ADDR 			(12 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //12 short
#define LOCAL_closedLoopLagSearch_pswCCBuf_ADDR 			(24 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //6 short
#define LOCAL_closedLoopLagSearch_pswCGBuf_ADDR 			(30 + CII_sfrmAnalysis_opt_inner_0_Y_BEGIN_ADDR) //6 short

//CII_hnwFilt_opt
#define CII_hnwFilt_opt_X_BEGIN_ADDR            			(644 + HR_LOCAL_X_BEGIN_ADDR)
#define hnwFilt_swZeroState_ADDR                			(0 + CII_hnwFilt_opt_X_BEGIN_ADDR) //1 short

//CII_lpcZsIir_opt
#define CII_lpcZsIir_opt_Y_BEGIN_ADDR           			(276 + HR_LOCAL_Y_BEGIN_ADDR)
#define lpcZsIir_copy_ADDR                      			(0 + CII_lpcZsIir_opt_Y_BEGIN_ADDR) //80 short

//CII_decorr_opt
#define CII_decorr_opt_X_BEGIN_ADDR             			(644 + HR_LOCAL_X_BEGIN_ADDR)
#define decorr_pswGivenVect_ADDR_ADDR           			(0 + CII_decorr_opt_X_BEGIN_ADDR) //1 short

//CII_v_srch_opt
#define CII_v_srch_opt_X_BEGIN_ADDR             			(644 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_v_srch_pswCGUpdates_ADDR          			(0 + CII_v_srch_opt_X_BEGIN_ADDR) //144 short

//CII_v_srch_opt
#define CII_v_srch_opt_Y_BEGIN_ADDR             			(240 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_v_srch_pswBits_ADDR               			(0 + CII_v_srch_opt_Y_BEGIN_ADDR) //10 short
#define LOCAL_v_srch_pswWBasisVecs_copy_ADDR    			(10 + CII_v_srch_opt_Y_BEGIN_ADDR) //360 short
#define LOCAL_v_srch_pswDSpace_ADDR             			(370 + CII_v_srch_opt_Y_BEGIN_ADDR) //144 short
#define LOCAL_v_srch_pL_R_ADDR                  			(514 + CII_v_srch_opt_Y_BEGIN_ADDR) //18 short
#define LOCAL_v_srch_siBest_ADDR                			(532 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_v_srch_pswModNextBit_ADDR_ADDR    			(533 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short
#define LOCAL_v_srch_pswBits_ADDR_ADDR          			(534 + CII_v_srch_opt_Y_BEGIN_ADDR) //1 short

//CII_sfrmAnalysis_opt_inner_1
#define CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR			(644 + HR_LOCAL_X_BEGIN_ADDR)
#define g_quant_vl_swWIShift_ADDR               			(0 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //1 short
#define LOCAL_g_quant_vl_siNormMin_ADDR         			(1 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //1 short
#define LOCAL_g_quant_vl_ErrorTerm_ADDR         			(2 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //12 short
#define LOCAL_gainTweak_siNorm_ADDR             			(14 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //2 short
#define LOCAL_gainTweak_terms_ADDR              			(16 + CII_sfrmAnalysis_opt_inner_1_X_BEGIN_ADDR) //10 short

//CII_lpcCorrQntz_opt_inner_0
#define CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR			(644 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_AFLATNEWBARRECURSION_pL_POldSpace_ADDR			(0 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //20 short
#define LOCAL_AFLATNEWBARRECURSION_pL_VOldSpace_ADDR			(20 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //38 short
#define LOCAL_AFLATNEWBARRECURSION_pL_PNewSpace_ADDR			(58 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //20 short
#define LOCAL_AFLATNEWBARRECURSION_pL_VNewSpace_ADDR			(78 + CII_lpcCorrQntz_opt_inner_0_Y_BEGIN_ADDR) //38 short

//CII_aflatRecursionLoop_opt_inner_0
#define CII_aflatRecursionLoop_opt_inner_0_X_BEGIN_ADDR			(326 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_AFLATRECURSION_pswQntRcSqd_ADDR   			(0 + CII_aflatRecursionLoop_opt_inner_0_X_BEGIN_ADDR) //4 short

//CII_speechDecoder_opt
#define CII_speechDecoder_opt_X_BEGIN_ADDR      			(0 + HR_LOCAL_X_BEGIN_ADDR)
#define DEC_pswSynthFiltOut_ADDR                			(0 + CII_speechDecoder_opt_X_BEGIN_ADDR) //40 short

//CII_speechDecoder_opt
#define CII_speechDecoder_opt_Y_BEGIN_ADDR      			(0 + HR_LOCAL_Y_BEGIN_ADDR)
#define DEC_pswFrmPFDenom_ADDR                  			(0 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmKs_ADDR                       			(10 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmAs_ADDR                       			(20 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_pswFrmPFNum_ADDR                    			(30 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //20 short
#define DEC_siGsp0Code_ADDR                     			(50 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define FLAG_GETSFRMLPC_TX_ADDR                 			(51 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_psiVselpCw_ADDR                     			(52 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_pswPVec_ADDR                        			(54 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_ppswVselpEx_ADDR                    			(94 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //80 short
#define DEC_pswExcite_ADDR                      			(174 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_pswPPFExcit_ADDR                    			(214 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //40 short
#define DEC_pswBitArray_ADDR                    			(254 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //10 short
#define DEC_psnsSqrtRs_ADDR                     			(264 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //8 short
#define DEC_snsRs00_ADDR                        			(272 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_snsRs11_ADDR                        			(274 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_snsRs22_ADDR                        			(276 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //2 short
#define DEC_swMutePermit_ADDR                   			(278 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swLevelMean_ADDR                    			(279 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swLevelMax_ADDR                     			(280 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short
#define DEC_swMuteFlag_ADDR                     			(281 + CII_speechDecoder_opt_Y_BEGIN_ADDR) //1 short

//CII_a_sst_opt
#define CII_a_sst_opt_Y_BEGIN_ADDR              			(282 + HR_LOCAL_Y_BEGIN_ADDR)
#define A_SST_pL_CorrTemp_ADDR                  			(0 + CII_a_sst_opt_Y_BEGIN_ADDR) //22 short
#define A_SST_pswRCNum_ADDR                     			(22 + CII_a_sst_opt_Y_BEGIN_ADDR) //10 short
#define A_SST_pswRCDenom_ADDR                   			(32 + CII_a_sst_opt_Y_BEGIN_ADDR) //10 short

//CII_speechDecoder_opt_inner_0
#define CII_speechDecoder_opt_inner_0_Y_BEGIN_ADDR			(282 + HR_LOCAL_Y_BEGIN_ADDR)
#define LOCAL_pitchPreFilt_pswInterpCoefs_ADDR  			(0 + CII_speechDecoder_opt_inner_0_Y_BEGIN_ADDR) //10 short

//CII_speechDecoder_opt_inner_1
#define CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR			(40 + HR_LOCAL_X_BEGIN_ADDR)
#define LOCAL_signal_conceal_sub_pswStateTmp_ADDR			(0 + CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR) //10 short
#define LOCAL_signal_conceal_sub_swOutTmp_ADDR  			(10 + CII_speechDecoder_opt_inner_1_X_BEGIN_ADDR) //40 short

/**********************
 **  AMR_EFR_GLOBAL  **
 **********************/

//GLOBAL_VARS
#define STRUCT_POST_FILTERSTATE_RES2_ADDR       			(0 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_POST_FILTERSTATE_MEM_SYN_PST_ADDR			(40 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PREEMPHASISSTATE_ADDR            			(50 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_AGCSTATE_ADDR                    			(51 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_FILTERSTATE_SYNTH_BUF_ADDR  			(52 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //170 short
#define STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR     			(222 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR     			(223 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR     			(224 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR     			(225 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X0_ADDR        			(226 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_POST_PROCESSSTATE_X1_ADDR        			(227 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_RXDTX_CTRL_ADDR                  			(STRUCT_POST_PROCESSSTATE_Y2_LO_ADDR) //alias
#define STATIC_PREV_SID_FRAMES_LOST_ADDR        			(STRUCT_POST_PROCESSSTATE_Y2_HI_ADDR) //alias
#define GLOBAL_RX_DTX_STATE_ADDR                			(STRUCT_POST_PROCESSSTATE_Y1_LO_ADDR) //alias
#define STATIC_RXDTX_N_ELAPSED_ADDR             			(STRUCT_POST_PROCESSSTATE_Y1_HI_ADDR) //alias
#define STATIC_RXDTX_AVER_PERIOD_ADDR           			(STRUCT_POST_PROCESSSTATE_X0_ADDR) //alias
#define STATIC_BUF_P_RX_ADDR                    			(STRUCT_POST_PROCESSSTATE_X1_ADDR) //alias
#define STURCT_SPEECH_DECODE_FRAMESTATE_PREV_MODE_ADDR			(228 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STATIC_L_SACF_ADDR                      			(236 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //54 short
#define GLOBAL_CN_EXCITATION_GAIN_ADDR          			(290 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_GCODE0_CN_ADDR                   			(291 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define SCAL_ACF_ADDR_P                         			(292 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define VAD_FLAG_ADDR_P                         			(293 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define TX_SP_FLAG_ADDR                         			(294 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define GLOBAL_TXDTX_CTRL_ADDR                  			(295 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define PRAM4_TABLE_ADDR                        			(296 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR      			(306 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR      			(307 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR      			(308 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR      			(309 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_X0_ADDR         			(310 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PRE_PROCESSSTATE_X1_ADDR         			(311 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR     			(312 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR  			(352 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_P_WINDOW_ADDR       			(392 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SPEECH_ADDR         			(432 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR     			(472 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //161 short
#define STRUCT_COD_AMRSTATE_OLD_WSP_ADDR        			(633 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //143 short
#define STRUCT_COD_AMRSTATE_WSP_ADDR            			(776 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //160 short
#define STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR       			(936 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR    			(942 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_COD_AMRSTATE_OLD_EXC_ADDR        			(944 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_COD_AMRSTATE_EXC_ADDR            			(1098 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //161 short
#define STRUCT_COD_AMRSTATE_AI_ZERO_ADDR        			(1259 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_COD_AMRSTATE_ZERO_ADDR           			(1270 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //41 short
#define STRUCT_LEVINSONSTATE_OLD_A_ADDR         			(1311 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //11 short
#define STRUCT_LSPSTATE_LSP_OLD_ADDR            			(1322 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_LSPSTATE_LSP_OLD_Q_ADDR          			(1332 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR         			(1342 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR			(1352 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_GCODE0_ADDR			(1354 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_GCODE0_ADDR			(1355 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_TARGET_EN_ADDR			(1356 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_TARGET_EN_ADDR			(1357 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINQUANTSTATE_SF0_EXP_COEFF_ADDR			(1358 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_GAINQUANTSTATE_SF0_FRAC_COEFF_ADDR			(1364 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //5 short
#define STRUCT_GAINQUANTSTATE_GAIN_IDX_PTR_ADDR 			(1369 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDST_ADDR                   			(1370 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDST_PAST_QUA_EN_MR122_ADDR 			(1374 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STURCT_GC_PREDUNQST_ADDR                			(1378 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDUNQST_PAST_QUA_EN_MR122_ADDR			(1382 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GAINADAPTSTATE_ONSET_ADDR        			(1386 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINADAPTSTATE_PREV_ALPHA_ADDR   			(1387 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GAINADAPTSTATE_PREV_GC_ADDR      			(1388 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_GAINADAPTSTATE_LTPG_MEM_ADDR     			(1390 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PITCHOLWGHTSTATE_OLD_T0_MED_ADDR 			(1396 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PITCHOLWGHTSTATE_ADA_W_ADDR      			(1397 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PITCHOLWGHTSTATE_WGHT_FLG_ADDR   			(1398 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_TONSTABSTATE_GP_ADDR             			(1400 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //7 short
#define STRUCT_TONSTABSTATE_COUNT_ADDR          			(1407 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_BCKR_EST_ADDR          			(1408 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_AVE_LEVEL_ADDR         			(1418 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_OLD_LEVEL_ADDR         			(1428 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_SUB_LEVEL_ADDR         			(1438 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_VADSTATE1_A_DATA5_ADDR           			(1448 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_VADSTATE1_A_DATA3_ADDR           			(1454 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_VADSTATE1_BURST_COUNT_ADDR       			(1460 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_HANG_COUNT_ADDR        			(1461 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_STAT_COUNT_ADDR        			(1462 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_VADREG_ADDR            			(1463 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_PITCH_ADDR             			(1464 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_TONE_ADDR              			(1465 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HIGH_ADDR      			(1466 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_LOW_ADDR       			(1467 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_OLDLAG_COUNT_ADDR      			(1468 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_OLDLAG_ADDR            			(1469 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HANG_COUNT_ADDR			(1470 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_HANG_TIMER_ADDR			(1471 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_BEST_CORR_HP_ADDR      			(1472 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_SPEECH_VAD_DECISION_ADDR			(1473 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_COMPLEX_WARNING_ADDR   			(1474 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_SP_BURST_COUNT_ADDR    			(1475 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_VADSTATE1_CORR_HP_FAST_ADDR      			(1476 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_DTX_ADDR            			(1477 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR       			(1478 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR    			(1558 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR       			(1566 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR   			(1567 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR			(1568 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR      			(1570 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR			(1574 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR			(1575 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_COD_AMRSTATE_MEM_SYN_ADDR        			(1576 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W0_ADDR         			(1586 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_W_ADDR          			(1596 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR      			(1606 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_COD_AMRSTATE_ERROR_ADDR          			(1616 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_SHARP_ADDR          			(1656 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SPEECH_ENCODE_FRAMESTATE_DTX_ADDR			(1657 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR			(1658 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR			(1659 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_SID_HANDOVER_DEBT_ADDR			(1660 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_SID_SYNCSTATE_PREV_FT_ADDR       			(1661 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_EXC_ADDR      			(1662 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //154 short
#define STRUCT_DECOD_AMRSTATE_EXC_ADDR          			(1816 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //40 short
#define STRUCT_DECOD_AMRSTATE_LSP_OLD_ADDR      			(1856 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_MEM_SYN_ADDR      			(1866 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_SHARP_ADDR        			(1876 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_OLD_T0_ADDR       			(1877 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_BF_ADDR      			(1878 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_PREV_PDF_ADDR     			(1879 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_STATE_ADDR        			(1880 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_T0_LAGBUFF_ADDR   			(1881 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_EXCENERGYHIST_ADDR			(1882 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DECOD_AMRSTATE_INBACKGROUNDNOISE_ADDR			(1892 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_VOICEDHANGOVER_ADDR			(1893 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DECOD_AMRSTATE_LTPGAINHISTORY_ADDR			(1894 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //9 short
#define STRUCT_DECOD_AMRSTATE_NODATASEED_ADDR   			(1903 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_BGN_SCDSTATE_FRAMEENERGYHIST_ADDR			(1904 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //60 short
#define STRUCT_BGN_SCDSTATE_BGHANGOVER_ADDR     			(1964 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_CB_GAIN_AVERAGESTATE_ADDR        			(1966 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGCOUNT_ADDR			(1974 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_CB_GAIN_AVERAGESTATE_HANGVAR_ADDR			(1975 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_LSP_AVGSTATE_LSP_MEANSAVE_ADDR   			(1976 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_LSF_Q_ADDR      			(1986 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_D_PLSFSTATE_PAST_R_Q_ADDR        			(1996 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_EC_GAIN_PITCHSTATE_ADDR          			(2006 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_PITCHSTATE_PAST_GAIN_PIT_ADDR			(2012 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_PITCHSTATE_PREV_GP_ADDR  			(2013 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_ADDR           			(2014 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_EC_GAIN_CODESTATE_PAST_GAIN_CODE_ADDR			(2020 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_EC_GAIN_CODESTATE_PREV_GC_ADDR   			(2021 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_ADDR    			(2022 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_GC_PREDSTATE_PAST_QUA_EN_MR122_ADDR			(2026 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //4 short
#define STRUCT_PH_DISPSTATE_GAINMEM_ADDR        			(2030 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //6 short
#define STRUCT_PH_DISPSTATE_PREVSTATE_ADDR      			(2036 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_PREVCBGAIN_ADDR     			(2037 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_LOCKFULL_ADDR       			(2038 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_PH_DISPSTATE_ONSET_ADDR          			(2039 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SINCE_LAST_SID_ADDR 			(2040 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_TRUE_SID_PERIOD_INV_ADDR			(2041 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADDR         			(2042 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_OLD_LOG_EN_ADDR     			(2043 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_L_PN_SEED_RX_ADDR   			(2044 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STRUCT_DTX_DECSTATE_LSP_ADDR            			(2046 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSP_OLD_ADDR        			(2056 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_ADDR       			(2066 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //80 short
#define STRUCT_DTX_DECSTATE_LSF_HIST_PTR_ADDR   			(2146 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_PG_MEAN_ADDR    			(2147 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define DEC_STATIC_LSF_P_CN_ADDR                			(2148 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_RX_ADDR                  			(2158 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //70 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_ADDR    			(2228 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //8 short
#define STRUCT_DTX_DECSTATE_LOG_EN_HIST_PTR_ADDR			(2236 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_LOG_EN_ADJUST_ADDR  			(2237 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERCOUNT_ADDR			(2238 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DECANAELAPSEDCOUNT_ADDR			(2239 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_SID_FRAME_ADDR      			(2240 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_VALID_DATA_ADDR     			(2241 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXHANGOVERADDED_ADDR			(2242 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DTXGLOBALSTATE_ADDR 			(2243 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //1 short
#define STRUCT_DTX_DECSTATE_DATA_UPDATED_ADDR   			(2244 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //2 short
#define STATIC_L_SAV0_ADDR                      			(2246 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //72 short
#define GLOBAL_LSF_NEW_CN_DEC_ADDR              			(2318 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_CN_DEC_ADDR              			(2328 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //10 short
#define GLOBAL_GAIN_CODE_OLD_RX_ADDR            			(2338 + AMR_EFR_GLOBAL_X_BEGIN_ADDR) //28 short

//GLOBAL_VARS
#define STRUCT_COD_AMRSTATE_HVEC_ADDR           			(0 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STRUCT_COD_AMRSTATE_H1_ADDR             			(40 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define STATIC_SCAL_RVAD_ADDR                   			(80 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_CODE_SF0_ADDRESS                			(STATIC_SCAL_RVAD_ADDR) //alias
#define STATIC_RVAD_ADDR                        			(81 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //9 short
#define STATIC_BURSTCOUNT_ADDR                  			(90 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_HANGCOUNT_ADDR                   			(91 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_PT_SACF_ADDR                     			(92 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_PT_SAV0_ADDR                     			(93 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_L_LASTDM_ADDR                    			(94 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STATIC_ADAPTCOUNT_ADDR                  			(96 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_PTCH_ADDR                        			(97 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_THVAD_ADDR                       			(98 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //4 short
#define STATIC_LSF_P_CN_ADDR                    			(102 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define GLOBAL_LSF_OLD_TX_ADDR                  			(112 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //10 short
#define COD_AMR_XN_SF0_ADDRESS                  			(122 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //40 short
#define COD_AMR_Y2_SF0_ADDRESS                  			(162 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //20 short
#define STATIC_TXDTX_HANGOVER_ADDR              			(182 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_TXDTX_N_ELAPSED_ADDR             			(183 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_L_PN_SEED_TX_ADDR                			(184 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define STATIC_VERYOLDLAGCOUNT_ADDR             			(186 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_BUF_P_TX_ADDR                    			(187 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define STATIC_OLD_CN_MEM_TX_ADDR               			(188 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //8 short
#define GLOBAL_GAIN_CODE_OLD_TX_ADDR            			(196 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //6 short
#define COD_AMR_T0_SF0_ADDRESS                  			(202 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_T0_FRAC_SF0_ADDRESS             			(203 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_I_SUBFR_SF0_ADDRESS             			(204 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define COD_AMR_GAIN_PIT_SF0_ADDRESS            			(206 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_CODE_SF0_ADDRESS           			(207 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define Blankkk                                 			(208 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //16 short
#define GLOBAL_L_PN_SEED_RX_ADDR                			(224 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_GCODE0_CN_DEC_ADDR               			(226 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_MUTING_CN_DEC_ADDR     			(227 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_OLD_CN_DEC_ADDR        			(228 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_GAIN_CODE_NEW_CN_DEC_ADDR        			(229 + AMR_EFR_GLOBAL_Y_BEGIN_ADDR) //1 short

/*********************
 **  AMR_EFR_LOCAL  **
 *********************/

//CII_AMR_Encode
#define CII_AMR_Encode_X_BEGIN_ADDR             			(0 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COD_AMR_XN_ADDRESS                      			(0 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_XN2_ADDRESS                     			(40 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_Y1_ADDRESS                      			(80 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_Y2_ADDRESS                      			(120 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_RES_ADDRESS                     			(160 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define COD_AMR_MEM_SYN_SAVE_ADDRESS            			(200 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_MEM_W0_SAVE_ADDRESS             			(210 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_MEM_ERR_SAVE_ADDRESS            			(220 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_GCOEFF_ADDRESS                  			(230 + CII_AMR_Encode_X_BEGIN_ADDR) //4 short
#define COD_AMR_SUBFRNR_ADDRESS                 			(234 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_I_SUBFR_ADDRESS                 			(235 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_T_OP_ADDRESS                    			(236 + CII_AMR_Encode_X_BEGIN_ADDR) //2 short
#define COD_AMR_T0_ADDRESS                      			(238 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_T0_FRAC_ADDRESS                 			(239 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_PIT_ADDRESS                			(240 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GAIN_CODE_ADDRESS               			(241 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_EXC_ADDR_ADDRESS                			(242 + CII_AMR_Encode_X_BEGIN_ADDR) //2 short
#define COD_AMR_LSP_FLAG_ADDRESS                			(244 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_GP_LIMIT_ADDRESS                			(245 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_VAD_FLAG_ADDRESS                			(246 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_COMPUTE_SID_FLAG_ADDRESS        			(247 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_A_T_ADDRESS                     			(248 + CII_AMR_Encode_X_BEGIN_ADDR) //44 short
#define COD_AMR_AQ_T_ADDRESS                    			(292 + CII_AMR_Encode_X_BEGIN_ADDR) //44 short
#define COD_AMR_LSP_NEW_ADDRESS                 			(336 + CII_AMR_Encode_X_BEGIN_ADDR) //10 short
#define COD_AMR_SHARP_SAVE_ADDRESS              			(346 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_EVENSUBFR_ADDRESS               			(347 + CII_AMR_Encode_X_BEGIN_ADDR) //1 short
#define COD_AMR_CODE_ADDRESS                    			(348 + CII_AMR_Encode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_CODE_ADDRESS                    			(COD_AMR_CODE_ADDRESS) //alias

//CII_AMR_Encode
#define CII_AMR_Encode_Y_BEGIN_ADDR             			(0 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COD_AMR_ANA_ADDR                        			(0 + CII_AMR_Encode_Y_BEGIN_ADDR) //60 short
#define COD_AMR_RES3_ADDRESS                    			(60 + CII_AMR_Encode_Y_BEGIN_ADDR) //40 short
#define BLANK                                   			(100 + CII_AMR_Encode_Y_BEGIN_ADDR) //2 short

//CII_vad1
#define CII_vad1_Y_BEGIN_ADDR                   			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD1_level_ADDR                         			(0 + CII_vad1_Y_BEGIN_ADDR) //11 short
#define VAD1_decission_noise_level_ADDR         			(11 + CII_vad1_Y_BEGIN_ADDR) //1 short
#define VAD1_low_power_flag_ADDR                			(12 + CII_vad1_Y_BEGIN_ADDR) //1 short

//CII_Levinson
#define CII_Levinson_X_BEGIN_ADDR               			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define LEVINSON_ANL_ADDR                       			(0 + CII_Levinson_X_BEGIN_ADDR) //11 short

//CII_Levinson
#define CII_Levinson_Y_BEGIN_ADDR               			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Blank_LEVINSON                          			(0 + CII_Levinson_Y_BEGIN_ADDR) //3 short
#define PARA4_ADDR_ADDR                         			(3 + CII_Levinson_Y_BEGIN_ADDR) //1 short
#define LEVINSON_AL_ADDR                        			(4 + CII_Levinson_Y_BEGIN_ADDR) //22 short

//CII_subframePostProc
#define CII_subframePostProc_Y_BEGIN_ADDR       			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COD_AMR_SYNTH_ADDR                      			(0 + CII_subframePostProc_Y_BEGIN_ADDR) //160 short

//CII_Syn_filt
#define CII_Syn_filt_Y_BEGIN_ADDR               			(262 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SYN_FILT_TMP_ADDR                       			(0 + CII_Syn_filt_Y_BEGIN_ADDR) //80 short

//CII_Autocorr
#define CII_Autocorr_X_BEGIN_ADDR               			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AUTOCORR_Y_ADDR                         			(0 + CII_Autocorr_X_BEGIN_ADDR) //240 short
#define LPC_RLOW_ADDR                           			(240 + CII_Autocorr_X_BEGIN_ADDR) //12 short
#define LPC_RHIGH_ADDR                          			(252 + CII_Autocorr_X_BEGIN_ADDR) //18 short
#define LPC_RC_ADDR                             			(270 + CII_Autocorr_X_BEGIN_ADDR) //4 short

//CII_Autocorr
#define CII_Autocorr_Y_BEGIN_ADDR               			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Blamk_AUTOCORR_Y_ADDR                   			(0 + CII_Autocorr_Y_BEGIN_ADDR) //4 short
#define AUTOCORR_Y_COPY_ADDR                    			(4 + CII_Autocorr_Y_BEGIN_ADDR) //240 short
#define Windows_table_ADDR                      			(244 + CII_Autocorr_Y_BEGIN_ADDR) //240 short

//CII_Az_lsp
#define CII_Az_lsp_X_BEGIN_ADDR                 			(418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AZ_LSP_IP_ADDR                          			(0 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_COEF_ADDR                        			(1 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_GRID_J_ADDR_ADDR                 			(2 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_LOOP_CTRL_ADDR                   			(3 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XLOW_ADDR                        			(4 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YLOW_ADDR                        			(5 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XHIGH_ADDR                       			(6 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YHIGH_ADDR                       			(7 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_XMID_ADDR                        			(8 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_YMID_ADDR                        			(9 + CII_Az_lsp_X_BEGIN_ADDR) //1 short
#define AZ_LSP_F1_ADDR                          			(10 + CII_Az_lsp_X_BEGIN_ADDR) //10 short
#define AZ_LSP_F2_ADDR                          			(20 + CII_Az_lsp_X_BEGIN_ADDR) //10 short

//CII_Chebps
#define CII_Chebps_Y_BEGIN_ADDR                 			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define CHEBPS_B2_ADDR                          			(0 + CII_Chebps_Y_BEGIN_ADDR) //2 short

//CII_cor_h_x
#define CII_cor_h_x_X_BEGIN_ADDR                			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COR_H_X_Y32_ADDR                        			(0 + CII_cor_h_x_X_BEGIN_ADDR) //40 short

//CII_cor_h
#define CII_cor_h_X_BEGIN_ADDR                  			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define COR_H_H2_ADDR                           			(0 + CII_cor_h_X_BEGIN_ADDR) //40 short

//CII_cor_h
#define CII_cor_h_Y_BEGIN_ADDR                  			(962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define COR_H_H2_COPY_ADDR                      			(0 + CII_cor_h_Y_BEGIN_ADDR) //40 short
#define COPY_C1035PF_SIGN_ADDR                  			(40 + CII_cor_h_Y_BEGIN_ADDR) //20 short

//CII_G_pitch
#define CII_G_pitch_Y_BEGIN_ADDR                			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define G_PITCH_SCALED_Y1_ADDR                  			(0 + CII_G_pitch_Y_BEGIN_ADDR) //40 short
#define G_PITCH_Y1_ADDR                         			(40 + CII_G_pitch_Y_BEGIN_ADDR) //40 short

//CII_subframePreProc
#define CII_subframePreProc_X_BEGIN_ADDR        			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define PRE_BIG_AP1_ADDR                        			(0 + CII_subframePreProc_X_BEGIN_ADDR) //20 short
#define PRE_BIG_AP2_ADDR                        			(20 + CII_subframePreProc_X_BEGIN_ADDR) //20 short

//CII_AMR_Decode
#define CII_AMR_Decode_X_BEGIN_ADDR             			(0 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DEC_AMR_LSP_NEW_ADDRESS                 			(0 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_LSP_MID_ADDRESS                 			(10 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_PREV_LSF_ADDRESS                			(20 + CII_AMR_Decode_X_BEGIN_ADDR) //10 short
#define DEC_AMR_LSF_I_ADDRESS                   			(30 + CII_AMR_Decode_X_BEGIN_ADDR) //50 short
#define DEC_AMR_EXCP_ADDRESS                    			(80 + CII_AMR_Decode_X_BEGIN_ADDR) //40 short
#define DEC_AMR_EXC_ENHANCED_ADDRESS            			(120 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_T0_ADDRESS                      			(164 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_FRAC_ADDRESS                 			(165 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_ADDRESS                   			(166 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_INDEX_MR475_ADDRESS             			(167 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_PIT_ADDRESS                			(168 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_ADDRESS               			(169 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_GAIN_CODE_MIX_ADDRESS           			(170 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_SHARP_ADDRESS               			(171 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FLAG_ADDRESS                			(172 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PIT_FAC_ADDRESS                 			(173 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MIN_ADDRESS                  			(174 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_T0_MAX_ADDRESS                  			(175 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_LOW_ADDRESS           			(176 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_DELTA_FRC_RANGE_ADDRESS         			(177 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_SHIFT_ADDRESS              			(178 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP_ADDRESS                    			(179 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_L_TEMP_ADDRESS                  			(180 + CII_AMR_Decode_X_BEGIN_ADDR) //2 short
#define DEC_AMR_FLAG4_ADDRESS                   			(182 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_CAREFULFLAG_ADDRESS             			(183 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EXCENERGY_ADDRESS               			(184 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_SUBFRNR_ADDRESS                 			(185 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_EVENSUBFR_ADDRESS               			(186 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_NEWDTXSTATE_ADDRESS             			(187 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_BFI_ADDRESS                     			(188 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_PDFI_ADDRESS                    			(189 + CII_AMR_Decode_X_BEGIN_ADDR) //1 short
#define DEC_AMR_A_T_ADDRESS                     			(190 + CII_AMR_Decode_X_BEGIN_ADDR) //44 short
#define DEC_AMR_PARM_ADDRESS                    			(234 + CII_AMR_Decode_X_BEGIN_ADDR) //57 short

//CII_AMR_Decode
#define CII_AMR_Decode_Y_BEGIN_ADDR             			(0 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define AMR_DEC_BUFOUT_ADDR                     			(0 + CII_AMR_Decode_Y_BEGIN_ADDR) //160 short

//CII_bits2prm_amr_efr
#define CII_bits2prm_amr_efr_X_BEGIN_ADDR       			(292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define AMR_DEC_BUFIN_ADDR                      			(0 + CII_bits2prm_amr_efr_X_BEGIN_ADDR) //256 short
#define AMR_IF1_DEC_BUFIN_ADDR                  			(256 + CII_bits2prm_amr_efr_X_BEGIN_ADDR) //256 short

//CII_dtx_dec
#define CII_dtx_dec_X_BEGIN_ADDR                			(292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DTX_DEC_L_lsf_ADDR                      			(0 + CII_dtx_dec_X_BEGIN_ADDR) //20 short
#define DTX_DEC_lsf_ADDR                        			(20 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_LSP_INT_ADDR                    			(30 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_LSF_INT_ADDR                    			(40 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_lsf_int_variab_ADDR             			(50 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_lsp_int_variab_ADDR             			(60 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_acoeff_ADDR                     			(70 + CII_dtx_dec_X_BEGIN_ADDR) //12 short
#define dtx_dec_acoeff_variab_ADDR              			(82 + CII_dtx_dec_X_BEGIN_ADDR) //12 short
#define dtx_dec_refl_ADDR                       			(94 + CII_dtx_dec_X_BEGIN_ADDR) //10 short
#define dtx_dec_ex_ADDR                         			(104 + CII_dtx_dec_X_BEGIN_ADDR) //40 short

//CII_amr_dec_122
#define CII_amr_dec_122_X_BEGIN_ADDR            			(292 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define blank_dec                               			(0 + CII_amr_dec_122_X_BEGIN_ADDR) //106 short
#define DEC_AMR_BUFA40_ADDRESS                  			(106 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP1_ADDRESS                   			(107 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP2_ADDRESS                   			(108 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP3_ADDRESS                   			(109 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP4_ADDRESS                   			(110 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP5_ADDRESS                   			(111 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP6_ADDRESS                   			(112 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP7_ADDRESS                   			(113 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP8_ADDRESS                   			(114 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP9_ADDRESS                   			(115 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP10_ADDRESS                  			(116 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP11_ADDRESS                  			(117 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP12_ADDRESS                  			(118 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP13_ADDRESS                  			(119 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP14_ADDRESS                  			(120 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP15_ADDRESS                  			(121 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP16_ADDRESS                  			(122 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP17_ADDRESS                  			(123 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP18_ADDRESS                  			(124 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP19_ADDRESS                  			(125 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP20_ADDRESS                  			(126 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP21_ADDRESS                  			(127 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP22_ADDRESS                  			(128 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP23_ADDRESS                  			(129 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP24_ADDRESS                  			(130 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP25_ADDRESS                  			(131 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP26_ADDRESS                  			(132 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP27_ADDRESS                  			(133 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP28_ADDRESS                  			(134 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP29_ADDRESS                  			(135 + CII_amr_dec_122_X_BEGIN_ADDR) //11 short
#define DEC_AMR_D2PF_POS_ADDRESS                			(146 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS2_ADDRESS               			(148 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS4_ADDRESS               			(150 + CII_amr_dec_122_X_BEGIN_ADDR) //2 short
#define DEC_AMR_D2PF_POS6_ADDRESS               			(152 + CII_amr_dec_122_X_BEGIN_ADDR) //14 short
#define DEC_AMR_TEMP30_ADDRESS                  			(166 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP31_ADDRESS                  			(167 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP32_ADDRESS                  			(168 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP33_ADDRESS                  			(169 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP34_ADDRESS                  			(170 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP35_ADDRESS                  			(171 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP36_ADDRESS                  			(172 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP37_ADDRESS                  			(173 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP38_ADDRESS                  			(174 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short
#define DEC_AMR_TEMP39_ADDRESS                  			(175 + CII_amr_dec_122_X_BEGIN_ADDR) //1 short

//CII_prm2bits_amr_efr
#define CII_prm2bits_amr_efr_X_BEGIN_ADDR       			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define BIT_SERIAL_ADDR_RAM                     			(0 + CII_prm2bits_amr_efr_X_BEGIN_ADDR) //244 short

//CII_prm2bits_amr_efr
#define CII_prm2bits_amr_efr_Y_BEGIN_ADDR       			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define BIT_SERIAL_TRANS_ADDR                   			(0 + CII_prm2bits_amr_efr_Y_BEGIN_ADDR) //256 short

//CII_schur_recursion
#define CII_schur_recursion_Y_BEGIN_ADDR        			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD_SCHUR_RECUR_KK_ADDR_ADDR            			(0 + CII_schur_recursion_Y_BEGIN_ADDR) //9 short

//CII_compute_rav1
#define CII_compute_rav1_Y_BEGIN_ADDR           			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define VAD_COMP_RAV1_L_WORK_ADDR               			(0 + CII_compute_rav1_Y_BEGIN_ADDR) //100 short
#define VAD_COMP_RAV1_AAV1_ADDR_ADDR            			(100 + CII_compute_rav1_Y_BEGIN_ADDR) //20 short

//CII_spectral_comparison
#define CII_spectral_comparison_Y_BEGIN_ADDR    			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SPECTRAL_COMPARISON_SAV0_ADDR           			(0 + CII_spectral_comparison_Y_BEGIN_ADDR) //9 short

//CII_vad_computation
#define CII_vad_computation_X_BEGIN_ADDR        			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define VAD_COMPUTATION_ACF0_ADDR               			(0 + CII_vad_computation_X_BEGIN_ADDR) //2 short
#define VAD_COMPUTATION_PVAD_ADDR               			(2 + CII_vad_computation_X_BEGIN_ADDR) //2 short
#define VAD_COMPUTATION_L_AV0_ADDR              			(4 + CII_vad_computation_X_BEGIN_ADDR) //18 short
#define VAD_COMPUTATION_L_AV1_ADDR              			(22 + CII_vad_computation_X_BEGIN_ADDR) //18 short
#define VAD_COMPUTATION_RAV1_ADDR               			(40 + CII_vad_computation_X_BEGIN_ADDR) //9 short
#define VAD_COMPUTATION_SCAL_RAV1_ADDR          			(49 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_STAT_ADDR               			(50 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_TONE_ADDR               			(51 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_VVAD_ADDR               			(52 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_COMPUTATION_VAD_ADDR                			(53 + CII_vad_computation_X_BEGIN_ADDR) //7 short
#define VAD_PRED_VAL_VPAR_ADDR                  			(60 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_PRED_VAL_AAV1_ADDR                  			(70 + CII_vad_computation_X_BEGIN_ADDR) //20 short
#define VAD_COMPUTATION_A_ADDR                  			(90 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_STEP_UP_L_COEF_ADDR_ADDR            			(100 + CII_vad_computation_X_BEGIN_ADDR) //9 short
#define VAD_COMPUTATION_A_ADDR_END              			(109 + CII_vad_computation_X_BEGIN_ADDR) //1 short
#define VAD_SCHUR_RECUR_PP_ADDR_ADDR            			(110 + CII_vad_computation_X_BEGIN_ADDR) //10 short
#define VAD_STEP_UP_L_WORK_ADDR_ADDR            			(120 + CII_vad_computation_X_BEGIN_ADDR) //80 short

//CII_vad1_inner_0
#define CII_vad1_inner_0_X_BEGIN_ADDR           			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define FILTER_BANK_tmp_buf_ADDR                			(0 + CII_vad1_inner_0_X_BEGIN_ADDR) //160 short

//CII_dtx_enc
#define CII_dtx_enc_X_BEGIN_ADDR                			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define DTX_ENC_LSP_ADDR                        			(0 + CII_dtx_enc_X_BEGIN_ADDR) //10 short
#define DTX_ENC_LSF_ADDR                        			(10 + CII_dtx_enc_X_BEGIN_ADDR) //10 short
#define DTX_ENC_LSP_Q_ADDR                      			(20 + CII_dtx_enc_X_BEGIN_ADDR) //10 short

//CII_Q_plsf_3
#define CII_Q_plsf_3_X_BEGIN_ADDR               			(418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Q_PLSF_3_LSF1_ADDR                      			(0 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short
#define Q_PLSF_3_LSF_R1_ADDR                    			(10 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short
#define Q_PLSF_3_LSF1_Q_ADDR                    			(20 + CII_Q_plsf_3_X_BEGIN_ADDR) //20 short
#define Q_PLSF_3_PRED_INIT_I_ADDR               			(40 + CII_Q_plsf_3_X_BEGIN_ADDR) //1 short
#define Q_PLSF_3_LOOP_I_ADDR                    			(41 + CII_Q_plsf_3_X_BEGIN_ADDR) //9 short
#define Q_PLSF_3_TEMP_P_ADDR                    			(50 + CII_Q_plsf_3_X_BEGIN_ADDR) //80 short
#define Q_PLSF_3_MEAN_LSF_COPY_ADDR             			(130 + CII_Q_plsf_3_X_BEGIN_ADDR) //10 short

//CII_Q_plsf_3
#define CII_Q_plsf_3_Y_BEGIN_ADDR               			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Q_PLSF_3_LSF_P_ADDR                     			(0 + CII_Q_plsf_3_Y_BEGIN_ADDR) //10 short
#define Q_PLSF_3_WF1_ADDR                       			(10 + CII_Q_plsf_3_Y_BEGIN_ADDR) //10 short
#define Q_PLSF_3_TEMP_R1_ADDR                   			(20 + CII_Q_plsf_3_Y_BEGIN_ADDR) //80 short

//CII_lsp_122
#define CII_lsp_122_X_BEGIN_ADDR                			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define LSP_LSP_NEW_Q_ADDR                      			(0 + CII_lsp_122_X_BEGIN_ADDR) //10 short
#define LSP_LSP_MID_ADDR                        			(10 + CII_lsp_122_X_BEGIN_ADDR) //10 short
#define LSP_LSP_MID_Q_ADDR                      			(20 + CII_lsp_122_X_BEGIN_ADDR) //10 short

//CII_COD_AMR_122_inner_0
#define CII_COD_AMR_122_inner_0_X_BEGIN_ADDR    			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Pitch_fr6_max_loop_addr                 			(0 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //2 short
#define Pitch_fr6_corr_v_addr                   			(2 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //40 short
#define Pitch_fr6_t0_min_addr                   			(42 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short
#define Pitch_fr6_t0_max_addr                   			(43 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short
#define Pitch_delta_search_addr                 			(44 + CII_COD_AMR_122_inner_0_X_BEGIN_ADDR) //1 short

//CII_COD_AMR_122_inner_0
#define CII_COD_AMR_122_inner_0_Y_BEGIN_ADDR    			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define PITCH_F6_EXCF_ADDR                      			(0 + CII_COD_AMR_122_inner_0_Y_BEGIN_ADDR) //40 short

//CII_code_10i40_35bits
#define CII_code_10i40_35bits_Y_BEGIN_ADDR      			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define C1035PF_DN_ADDR                         			(0 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //40 short
#define C1035PF_RR_DIAG_ADDR                    			(40 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //40 short
#define C1035PF_RR_SIDE_ADDR                    			(80 + CII_code_10i40_35bits_Y_BEGIN_ADDR) //780 short

//CII_gainQuant_M122
#define CII_gainQuant_M122_Y_BEGIN_ADDR         			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define calc_filt_energies_Y2_ADDRESS           			(0 + CII_gainQuant_M122_Y_BEGIN_ADDR) //50 short
#define gainQuant_frac_coeff_ADDRESS            			(50 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define gainQuant_exp_coeff_ADDRESS             			(60 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_coeff_ADDRESS          			(70 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_coeff_lo_ADDRESS       			(80 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR475_gain_quant_exp_max_ADDRESS        			(90 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_frac_en_ADDRESS        			(100 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_exp_en_ADDRESS         			(110 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_g_pitch_cand_ADDRESS   			(120 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short
#define MR795_gain_quant_g_pitch_cind_ADDRESS   			(130 + CII_gainQuant_M122_Y_BEGIN_ADDR) //10 short

//CII_Lag_max_M122
#define CII_Lag_max_M122_X_BEGIN_ADDR           			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define PITCH_OL_M122_SCALED_SIGNAL2_ADDR       			(0 + CII_Lag_max_M122_X_BEGIN_ADDR) //310 short

//CII_Lag_max_M122
#define CII_Lag_max_M122_Y_BEGIN_ADDR           			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define PITCH_OL_M475_SCALED_SIGNAL_ADDR        			(0 + CII_Lag_max_M122_Y_BEGIN_ADDR) //310 short
#define PITCH_OL_M475_P_MAX1_ADDR               			(310 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX1_ADDR                 			(311 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_P_MAX2_ADDR               			(312 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX2_ADDR                 			(313 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_P_MAX3_ADDR               			(314 + CII_Lag_max_M122_Y_BEGIN_ADDR) //1 short
#define PITCH_OL_M475_MAX3_ADDR                 			(315 + CII_Lag_max_M122_Y_BEGIN_ADDR) //7 short
#define PITCH_OL_M475_CORR_ADDR                 			(322 + CII_Lag_max_M122_Y_BEGIN_ADDR) //300 short

//CII_code_10i40_35bits_inner_0
#define CII_code_10i40_35bits_inner_0_X_BEGIN_ADDR			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define SET_SIGN_EN_ADDR                        			(0 + CII_code_10i40_35bits_inner_0_X_BEGIN_ADDR) //230 short
#define C417PF_DN2_ADDR                         			(230 + CII_code_10i40_35bits_inner_0_X_BEGIN_ADDR) //40 short

//CII_search_M122
#define CII_search_M122_X_BEGIN_ADDR            			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define SEARCH_10I40_RRV_ADDR                   			(0 + CII_search_M122_X_BEGIN_ADDR) //40 short
#define SEARCH_CODE_CP_RR_ADDR                  			(40 + CII_search_M122_X_BEGIN_ADDR) //320 short
#define SEARCH_CODE_PS0_TEMP_ADDR               			(360 + CII_search_M122_X_BEGIN_ADDR) //1 short
#define SEARCH_10I40_PS0_ADDR                   			(361 + CII_search_M122_X_BEGIN_ADDR) //1 short
#define C1035PF_IPOS_ADDR                       			(362 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_POS_MAX_ADDR                    			(372 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_CODVEC_ADDR                     			(382 + CII_search_M122_X_BEGIN_ADDR) //10 short
#define C1035PF_SIGN_ADDR                       			(392 + CII_search_M122_X_BEGIN_ADDR) //40 short

//CII_code_10i40_35bits_inner_1
#define CII_code_10i40_35bits_inner_1_X_BEGIN_ADDR			(388 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define build_code_sign_ADDR                    			(0 + CII_code_10i40_35bits_inner_1_X_BEGIN_ADDR) //10 short

//CII_build_code_com2
#define CII_build_code_com2_Y_BEGIN_ADDR        			(962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define BUILD_CODE_POINT_ADDR_ADDR              			(0 + CII_build_code_com2_Y_BEGIN_ADDR) //10 short

//CII_SEARCH_LOOP_M122
#define CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR       			(962 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define SEARCH_CODE_RRV_COE_ADDR                			(0 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_RR_COE_ADDR                 			(1 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I_ADDR                     			(2 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_10I40_PSK_ADDR                   			(4 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_ALPK_ADDR                  			(5 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_2_ADDR             			(6 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_CP_RR_ADDR_ADDR             			(7 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_DN_ADDR_ADDR                			(8 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_RRV_ADDR_ADDR               			(9 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_SIDE_ADDR_ADDR               			(10 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define C1035PF_RR_DIAG_ADDR_ADDR               			(11 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_CP_RR_7_ADDR_ADDR           			(12 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define RR_SIDE_DIAG_TEMP_ADDR                  			(13 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2_TEMP_ADDR               			(14 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2I3_TEMP_ADDR             			(15 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_CODE_DN_RRV_ADDR_ADDR            			(16 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_CODE_ALP0_TEMP_ADDR              			(18 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //2 short
#define SEARCH_10I40_I0_ADDR                    			(20 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I1_ADDR                    			(21 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I2_ADDR                    			(22 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I3_ADDR                    			(23 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_M74_TRACK_ADDR                   			(24 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_M67_TRACK2_ADDR                  			(25 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I6_ADDR                    			(26 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I7_ADDR                    			(27 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I8_ADDR                    			(28 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define SEARCH_10I40_I9_ADDR                    			(29 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //1 short
#define linear_signs_ADDR                       			(30 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //4 short
#define linear_codewords_ADDR                   			(34 + CII_SEARCH_LOOP_M122_Y_BEGIN_ADDR) //4 short

//CII_Int_lpc_1and3
#define CII_Int_lpc_1and3_Y_BEGIN_ADDR          			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define INT_LPC_LSP_ADDR                        			(0 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //10 short
#define F1_ADDR                                 			(10 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //12 short
#define F2_ADDR                                 			(22 + CII_Int_lpc_1and3_Y_BEGIN_ADDR) //12 short

//CII_Q_plsf_5
#define CII_Q_plsf_5_X_BEGIN_ADDR               			(418 + AMR_EFR_LOCAL_X_BEGIN_ADDR)
#define Q_PLSF_5_LSF1_ADDR                      			(0 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF2_ADDR                      			(10 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_WF1_ADDR                       			(20 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_WF2_ADDR                       			(30 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_R1_ADDR                    			(40 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_R2_ADDR                    			(50 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF1_Q_ADDR                    			(60 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF2_Q_ADDR                    			(70 + CII_Q_plsf_5_X_BEGIN_ADDR) //10 short
#define Q_PLSF_5_LSF_AVER_ADDR                  			(80 + CII_Q_plsf_5_X_BEGIN_ADDR) //30 short
#define Q_PLSF_5_DTX_AND_SP_ADDR                			(110 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_SID_ADDR               			(111 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_HAN_ADDR               			(112 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short
#define Q_PLSF_5_DTX_AND_PREV_HAN_ADDR          			(113 + CII_Q_plsf_5_X_BEGIN_ADDR) //1 short

//CII_Q_plsf_5
#define CII_Q_plsf_5_Y_BEGIN_ADDR               			(102 + AMR_EFR_LOCAL_Y_BEGIN_ADDR)
#define Q_PLSF_5_LSF_P_ADDR                     			(0 + CII_Q_plsf_5_Y_BEGIN_ADDR) //10 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x2800, used 0x27da	RAM_Y: size 0x2800, used 0x27e0

#endif
