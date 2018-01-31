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


void vpp_LzmaDecMain(void);
void vpp_LzmaDecRunDma(void);
void vpp_LzmaDecCodeConstReload(void);

void Coolsand_LzmaDec(void);
void Coolsand_LzmaDecReset(void);

void vpp_LzmaDecBye(void);
void vpp_LzmaDecByeTest(void);

void Coolsand_rc_is_bit_0(void);

void Coolsand_process_bit0(void);
void Coolsand_process_bit1(void);

void Coolsand_rc_update_bit_0(void);
void Coolsand_rc_update_bit_1(void);

void Coolsand_peek_old_byte(void);
void Coolsand_rc_get_bit(void);
void Coolsand_write_byte(void);
void Coolsand_copy_byte(void);
void Coolsand_rc_bit_tree_decode(void);
void Coolsand_copy_bytes(void);

void Coolsand_rc_direct_bit(void);

void Coolsand_LzmaDmaInData(void);


#define GLOBAL_CONST_SIZE 2

// VoC_directive: PARSER_ON

#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 768

#define LZMA_NUM_POS_BITS_MAX 4

#define LZMA_LEN_NUM_LOW_BITS 3
#define LZMA_LEN_NUM_MID_BITS 3
#define LZMA_LEN_NUM_HIGH_BITS 8

#define LZMA_LEN_CHOICE 0
#define LZMA_LEN_CHOICE_2 (LZMA_LEN_CHOICE + 1)
#define LZMA_LEN_LOW (LZMA_LEN_CHOICE_2 + 1)
#define LZMA_LEN_MID (LZMA_LEN_LOW + (1 << (LZMA_NUM_POS_BITS_MAX + LZMA_LEN_NUM_LOW_BITS)))
#define LZMA_LEN_HIGH (LZMA_LEN_MID+(1 << (LZMA_NUM_POS_BITS_MAX + LZMA_LEN_NUM_MID_BITS)))
#define LZMA_NUM_LEN_PROBS (LZMA_LEN_HIGH + (1 << LZMA_LEN_NUM_HIGH_BITS))

#define LZMA_NUM_STATES 12
#define LZMA_NUM_LIT_STATES 7

#define LZMA_START_POS_MODEL_INDEX 4
#define LZMA_END_POS_MODEL_INDEX 14
#define LZMA_NUM_FULL_DISTANCES (1 << (LZMA_END_POS_MODEL_INDEX >> 1))

#define LZMA_NUM_POS_SLOT_BITS 6
#define LZMA_NUM_LEN_TO_POS_STATES 4

#define LZMA_NUM_ALIGN_BITS 4

#define LZMA_MATCH_MIN_LEN 2

#define LZMA_IS_MATCH 0
#define LZMA_IS_REP (LZMA_IS_MATCH + (LZMA_NUM_STATES << LZMA_NUM_POS_BITS_MAX))
#define LZMA_IS_REP_G0 (LZMA_IS_REP + LZMA_NUM_STATES)
#define LZMA_IS_REP_G1 (LZMA_IS_REP_G0 + LZMA_NUM_STATES)
#define LZMA_IS_REP_G2 (LZMA_IS_REP_G1 + LZMA_NUM_STATES)
#define LZMA_IS_REP_0_LONG (LZMA_IS_REP_G2 + LZMA_NUM_STATES)
#define LZMA_POS_SLOT (LZMA_IS_REP_0_LONG + (LZMA_NUM_STATES << LZMA_NUM_POS_BITS_MAX))
#define LZMA_SPEC_POS (LZMA_POS_SLOT+(LZMA_NUM_LEN_TO_POS_STATES << LZMA_NUM_POS_SLOT_BITS))
#define LZMA_ALIGN (LZMA_SPEC_POS + LZMA_NUM_FULL_DISTANCES - LZMA_END_POS_MODEL_INDEX)
#define LZMA_LEN_CODER (LZMA_ALIGN + (1 << LZMA_NUM_ALIGN_BITS))
#define LZMA_REP_LEN_CODER (LZMA_LEN_CODER + LZMA_NUM_LEN_PROBS)
#define LZMA_LITERAL (LZMA_REP_LEN_CODER + LZMA_NUM_LEN_PROBS)


#define RC_TOP_BITS 24
#define RC_MOVE_BITS 5
#define RC_MODEL_TOTAL_BITS 11
