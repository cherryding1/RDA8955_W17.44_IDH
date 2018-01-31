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


#define MP3_ENC_ConstX_size 909
#define MP3_ENC_ConstY_size 981


// VoC_directive: PARSER_OFF
void count_bit(void);
void new_choose_table(void);
void quantize(void);
void calc_runlen(void);
void count1_bitcount(void);
void subdivide(void);
void bigv_tab_select(void);
void bigv_bitcount(void);
void bin_search_StepSize(void);
void inner_loop(void);
void ResvMaxBits (void);
void ResvAdjust(void);
void ResvFrameEnd(void);
void cii_mdct18(void);
void L3_mdct_sub(void);
void L3_window_filter_subband(void);
void L3_iteration_loop(void);
void CS_L3_format_bitstream(void);
void CS_encodeSideInfo(void);
void CS_encodeMainData(void);
void CS_Huffmancodebits(void);
void CS_HuffmanCode(void);
void CS_L3_huffman_coder_count1(void);
void CS_BF_BitstreamFrame(void);
void CS_store_side_info(void);
void CS_main_data(void);
void CS_WriteMainDataBits(void);
void CS_write_side_info(void);
void CS_putbits(void);
void CS_writePartMainData(void);
void MP3_reset(void);

void vpp_AmjrMp3Encode(void);
void vpp_AmjrGenerateStepTabi(void);
void vpp_AmjrDecompressVssAdpcmTable(void);


// VoC_directive: PARSER_ON
