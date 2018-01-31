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




#include "vppp_lzmadec_asm_common.h"
#include "vppp_lzmadec_asm_map.h"
#include "vppp_lzmadec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
extern FILE *TestFileHandle;
// VoC_directive: PARSER_ON

#if 0


voc_short GLOBAL_INBUF[1024],x
voc_short probs_p[8192],x


voc_short writer_buffer[8192],y


voc_struct VPP_LZMADEC_CFG_STRUCT __attribute__((export)),x
// ----------------
// common config
// ----------------
// voc_short GLOBAL_MODE
// voc_short GLOBAL_RESET

voc_word  GLOBAL_IN_PTR
voc_word  GLOBAL_OUT_PTR
voc_word  GLABAL_OUT_PARA

voc_struct_end


voc_struct VPP_LZMADEC_STATUS_STRUCT __attribute__((export)),x

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
// ----------------
// hello status
// ----------------
voc_short  GLOBAL_LZMADEC_STATUS


voc_word GLOBAL_OUTSIZE

voc_struct_end



/*  struct lzma_header
{
    uint8_t pos;
    uint32_t dict_size;
    uint64_t dst_size;
};*/


voc_struct lzma_header __attribute__((export)),x

voc_short lzma_header_pos;
voc_short lzma_header_reserve;
voc_word  lzma_header_dict_size;
voc_word  lzma_header_dst_size[2];

voc_struct_end

/*
struct cstate {
    int state;
    uint32_t rep0, rep1, rep2, rep3;
};*/

voc_struct cstate ,x

voc_short cstate_state;
voc_short cstate_reserve;
voc_word cstate_rep0;
voc_word cstate_rep1;
voc_word cstate_rep2;
voc_word cstate_rep3;

voc_struct_end

/*struct writer {
    uint8_t *buffer;
    uint8_t previous_byte;
    size_t buffer_pos;
    int bufsize;
    size_t global_pos;
    int(*flush)(void*, unsigned int);
    struct lzma_header *header;
};*/


voc_struct writer ,x

voc_short writer_previous_byte;
voc_short writer_reserve;
voc_word writer_buffer_pos;
voc_word writer_bufsize;

voc_struct_end



/*
struct rc {
    int (*fill)(void*, unsigned int);
    uint8_t *ptr;
    uint8_t *buffer;
    uint8_t *buffer_end;
    int buffer_size;
    uint32_t code;
    uint32_t range;
    uint32_t bound;
};
*/

voc_struct rc ,x

voc_short rc_ptr;
voc_short rc_reserve;

voc_word rc_code;
voc_word rc_range;
voc_word rc_bound;

voc_struct_end




voc_word LeftShift_1_RC_TOP_BITS,x
voc_short pos_state_mask,x
voc_short literal_pos_mask,x

voc_short GLOBAL_lc,x
voc_short num_probs,x

voc_short pos_state,x
voc_short prob,x

voc_word OutputDMASize,x;

voc_short WriteBufP,x;
voc_short BITCACHE_VALUE,x;

voc_short prob_len,x
voc_short CONST_0x100_ADDR,x




#endif


// ----------------------------
// fct : vpp_LzmaDecMain
// in  :
// out :
// not modified :
// used : all
// ----------------------------

void vpp_LzmaDecMain(void)
{


    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

vpp_LzmaDecMain_L0:


    // set the stack pointers
    VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);

    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
    // Disable the SOF0 event, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    VoC_jal(Coolsand_LzmaDec);

    // Enable the SOF0 event only
    VoC_cfg_lg(CFG_WAKEUP_SOF0,CFG_WAKEUP_MASK);
    // Stall the execution and generate an interrupt
    VoC_cfg(CFG_CTRL_STALL|CFG_CTRL_IRQ_TO_XCPU);

// VoC_directive: PARSER_OFF
    // used for simulation
    VoC_cfg_stop;
// VoC_directive: PARSER_ON

    VoC_NOP();
    VoC_NOP();

    VoC_jump(vpp_LzmaDecMain_L0);

}


/**************************************************************************************
 * Function:    Coolsand_LzmaDec
 *
 * Description: lzma decoder
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/21/2011
 **************************************************************************************/
void Coolsand_LzmaDec(void)
{

// VoC_directive: PARSER_OFF
    printf("Coolsand_LzmaDec\n");
// VoC_directive: PARSER_ON

    VoC_push16(RA,DEFAULT);

    VoC_jal(Coolsand_LzmaDecReset);

    VoC_lw16i(REG4,GLOBAL_INBUF/2);
    VoC_lw16i(REG5,1024/2);
    VoC_lw32_d(ACC0,GLOBAL_IN_PTR);

    VoC_sw16_d(REG4,CFG_DMA_LADDR);
    VoC_sw16_d(REG5,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_shr32_ri(REG45,16,DEFAULT);

    VoC_shr32_ri(REG45,-2,DEFAULT);

    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG0,GLOBAL_INBUF,1);

    VoC_sw32_d(REG45,GLOBAL_IN_PTR);

    VoC_lw16i_short(WRAP0,10,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    //save lzma_header_dict_size,lzma_header_dst_size and lzma_header_dict_size+2
    VoC_lw16i_set_inc(REG2,lzma_header_dict_size,2);

    VoC_sw16_d(REG4,lzma_header_pos);

    VoC_loop_i(1,3);

    VoC_lw16i_set_inc(REG1,0,-8);

    VoC_loop_i_short(4,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_startloop0
    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_shru32_rr(REG45,REG1,DEFAULT);

    VoC_or32_rr(ACC0,REG45,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_lbinc_p(REG0);
    VoC_endloop0

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_endloop1

//  if (header.pos >= (9 * 5 * 5))
//      printf("bad header!");

    VoC_lw16i(REG1,225);
    VoC_bnlt16_rd(Coolsand_LzmaDec_L0,REG1,lzma_header_pos)
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_jump(Coolsand_LzmaDec_End);
Coolsand_LzmaDec_L0:

    VoC_lw16i_short(REG6,9,DEFAULT);

    VoC_lw16d_set_inc(REG1,lzma_header_pos,-9);
    VoC_lw16i_set_inc(REG2,0,1);

Coolsand_LzmaDec_L1:
    VoC_bgt16_rr(Coolsand_LzmaDec_L2,REG6,REG1)

    VoC_inc_p(REG2,DEFAULT);//mi++;
    VoC_inc_p(REG1,IN_PARALLEL);//lc -= 9;

    VoC_jump(Coolsand_LzmaDec_L1);
Coolsand_LzmaDec_L2:

    VoC_lw16i_short(INC2,-5,DEFAULT);
    VoC_lw16i_short(REG6,5,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,0,1);

Coolsand_LzmaDec_L3:
    VoC_bgt16_rr(Coolsand_LzmaDec_L4,REG6,REG2)

    VoC_inc_p(REG3,DEFAULT);//pb++;
    VoC_inc_p(REG2,IN_PARALLEL);//lp -= 5;

    VoC_jump(Coolsand_LzmaDec_L3);
Coolsand_LzmaDec_L4:
    //REG1:lp;
    //REG2:lc;
    //REG3:pb;

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG4,REG3,DEFAULT);//-pb
    VoC_sub16_rr(REG4,REG4,REG2,IN_PARALLEL);//-lp


    //pos_state_mask = (1 << pb) - 1;
    //literal_pos_mask = (1 << lp) - 1;

    VoC_shr16_rr(REG6,REG3,DEFAULT);
    VoC_shr16_rr(REG7,REG4,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);

    VoC_sw16_d(REG1,GLOBAL_lc);

    VoC_sw16_d(REG6,pos_state_mask);
    VoC_sw16_d(REG7,literal_pos_mask);

    //  if (header.dict_size == 0)
    //      header.dict_size = 1;

    VoC_bnez16_d(Coolsand_LzmaDec_L5,lzma_header_pos)
    VoC_sw16_d(REG5,lzma_header_pos);
Coolsand_LzmaDec_L5:

    VoC_add16_rr(REG5,REG1,REG2,DEFAULT);//(lc + lp)
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_lw16i(REG6,LZMA_LITERAL);
    VoC_lw16i(REG7,LZMA_LIT_SIZE);

    VoC_sub16_rr(REG5,REG4,REG5,DEFAULT);

    VoC_shr16_rr(REG7,REG5,DEFAULT);

    VoC_add16_rr(REG1,REG6,REG7,DEFAULT);//     num_probs = LZMA_LITERAL + (LZMA_LIT_SIZE << (lc + lp));
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_lw16i(REG6,0x400);
    VoC_lw16i_set_inc(REG2,probs_p,1);


Coolsand_LzmaDec_L6:
    VoC_bngtz16_r(Coolsand_LzmaDec_L7,REG1)

    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_jump(Coolsand_LzmaDec_L6);

Coolsand_LzmaDec_L7:

    //  rc_init_code(&rc);

    VoC_loop_i_short(5,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_shru32_ri(ACC1,-8,DEFAULT); //(rc->code << 8)

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_or32_rr(ACC1,REG45,DEFAULT);//rc_code

    VoC_lbinc_p(REG0);
    VoC_endloop0

    //I fix the variables to these regs. by Xuml
    VoC_lw32_d(REG67,rc_range);
    VoC_sw32_d(ACC1,rc_code);
    VoC_lw32_d(RL7,rc_bound);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);//write cache

    VoC_lw16i(STATUS,SHRU_ON);

    VoC_lw16i(STATUS,CARRY_ON);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(FORMATX,0-15,IN_PARALLEL);





Coolsand_LzmaDec_L8:
    //  while (get_pos(&wr) < header.dst_size)


// VoC_directive: PARSER_OFF


    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound



    VoC_lw32_d(REG45,writer_buffer_pos);
    VoC_lw32_d(REG23,lzma_header_dst_size);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();

    {
        int pos_xuml=REGS[5].reg<<16|(REGS[4].reg&0xffff);
        int dst_size=REGS[3].reg<<16|(REGS[2].reg&0xffff);



        //  if ((dst_size-pos_xuml)%0x100==0)
        {
            //printf("writer_buffer_pos:0x%8x\n",dst_size-pos_xuml);
            printf("writer_buffer_pos:0x%8x\n",pos_xuml);

        }

        if (pos_xuml==0x12535)
        {
            printf("ok");

        }

    }


    VoC_lw16_d(REG4,cstate_state);
    VoC_lw32_d(REG23,cstate_rep0);
    VoC_NOP();
    VoC_NOP();
    VoC_NOP();
    {
        int range_src=REGS[7].reg<<16|(REGS[6].reg&0xffff);
        int code_src=REGL[1];
        int bound_src=REGL[3];
        int state_src=REGS[4].reg;
        int rep0_src=REGS[3].reg<<16|(REGS[2].reg&0xffff);

        int range_dst=0;
        int code_dst=0;
        int bound_dst=0;
        int state_dst=0;
        int rep0_dst=0;


        fread(&(range_dst),1,4,TestFileHandle);
        fread(&(code_dst),1,4,TestFileHandle);
        fread(&(bound_dst),1,4,TestFileHandle);
        fread(&(state_dst),1,4,TestFileHandle);
        fread(&(rep0_dst),1,4,TestFileHandle);

        if (range_src!=range_dst)
        {
            printf("range error!:range_src:0x%8x.range_dst:0x%8x\n",range_src,range_dst);
        }


        if (code_src!=code_dst)
        {
            printf("code error!:code_src:0x%8x.code_dst:0x%8x\n",code_src,code_dst);
        }

        if (bound_src!=bound_dst)
        {
            printf("bound error!:bound_src:0x%8x.bound_dst:0x%8x\n",range_src,range_dst);
        }


        if (state_src!=state_dst)
        {
            printf("state error!:state_src:0x%8x.state_dst:0x%8x\n",state_src,state_dst);
        }

        if (rep0_src!=rep0_dst)
        {
            printf("rep0 error!:range_src:0x%8x.range_dst:0x%8x\n",rep0_src,rep0_dst);
        }
    }



// VoC_directive: PARSER_ON
    VoC_lw32_d(REG45,writer_buffer_pos);

    VoC_bnlt32_rd(Coolsand_LzmaDec_L10,REG45,lzma_header_dst_size)

    //  int pos_state = get_pos(&wr) & pos_state_mask;
    VoC_and16_rd(REG4,pos_state_mask);//pos_state

    //  uint16_t *prob = p + LZMA_IS_MATCH +(cst.state << LZMA_NUM_POS_BITS_MAX) + pos_state;

    VoC_lw16i(REG1,probs_p+LZMA_IS_MATCH);

    VoC_lw16_d(REG5,cstate_state);

    VoC_shr16_ri(REG5,-LZMA_NUM_POS_BITS_MAX,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG5,IN_PARALLEL);

    VoC_sw16_d(REG4,pos_state);//pos_state

    //  REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);

    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    //if rc->code < rc->bound ture else false;

    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(REG23,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_LzmaDec_L9);

    //REG4:prob
    VoC_jal(Coolsand_process_bit0);

    VoC_jump(Coolsand_LzmaDec_L8);
Coolsand_LzmaDec_L9:

    //REG4:prob
    VoC_jal(Coolsand_process_bit1);

    VoC_jump(Coolsand_LzmaDec_L8);
Coolsand_LzmaDec_L10:

    VoC_lw16i_short(REG7,0,DEFAULT);

Coolsand_LzmaDec_End:

    //flush the rest data.

    VoC_lw16i(STATUS,CARRY_OFF);
    VoC_lw16i(STATUS,SHRU_OFF);

    VoC_lw32_d(REG23,writer_buffer_pos);

    VoC_sub32_rd(REG45,REG23,OutputDMASize);

    VoC_sw32_d(REG23,GLOBAL_OUTSIZE);

    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_and16_rr(REG2,REG6,DEFAULT);
    VoC_shr32_ri(REG45,2,IN_PARALLEL);

    VoC_bez16_r(Coolsand_LzmaDec_L11,REG2)

    //fflush ACC0

    VoC_sub16_rr(REG1,REG6,REG2,DEFAULT);

    VoC_shr16_ri(REG1,-3,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_shru32_rr(ACC0,REG1,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_lw16_d(REG2,WriteBufP);

    VoC_add32_rr(REG45,REG45,REG01,DEFAULT);

    VoC_sw32_p(ACC0,REG2,DEFAULT);


    /*  VoC_lw16i_short(REG0,1,DEFAULT);
        VoC_lw16i_short(REG1,0,IN_PARALLEL);

        VoC_add32_rr(REG45,REG45,REG01,DEFAULT);*/

Coolsand_LzmaDec_L11:

    VoC_bngtz32_r(Coolsand_LzmaDec_L12,REG45)

    VoC_lw16i(REG6,writer_buffer/2);

    VoC_lw32_d(REG23,GLOBAL_OUT_PTR)

    VoC_add32_rd(REG23,REG23,OutputDMASize);

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG23,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

Coolsand_LzmaDec_L12:

    VoC_lw16i(REG6,VPP_LZMADEC_STATUS_STRUCT/2);
    VoC_lw16i(REG4,2);
    VoC_lw32_d(REG23,GLABAL_OUT_PARA);

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG23,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG7,GLOBAL_LZMADEC_STATUS);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_LzmaDecReset
 *
 * Description: lzma decoder reset
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/21/2011
 **************************************************************************************/
void Coolsand_LzmaDecReset(void)
{

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    //clear wrap regs
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    //clear DMA reg
    VoC_sw16_d(REG0,CFG_DMA_WRAP);

    //clear status reg
    VoC_lw16i(STATUS,STATUS_CLR);

    //reset cstate
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sw32_d(REG01,cstate_state);

    VoC_sw32_d(REG67,cstate_rep0);
    VoC_sw32_d(REG67,cstate_rep1);
    VoC_sw32_d(REG67,cstate_rep2);
    VoC_sw32_d(REG67,cstate_rep3);

    //reset writer
    VoC_sw16_d(REG0,writer_previous_byte);
    VoC_sw32_d(REG01,writer_buffer_pos);
    //VoC_sw32_d(REG01,writer_global_pos);

    VoC_lw16i_short(REG2,-1,DEFAULT);
    VoC_lw16i_short(REG3,-1,IN_PARALLEL);

    //reset rc
    VoC_sw32_d(REG01,rc_code);
    VoC_sw32_d(REG23,rc_range);

    //reset dam size
    VoC_sw32_d(REG01,OutputDMASize);

    //reset outsize
    VoC_sw32_d(REG23,GLOBAL_OUTSIZE);

    //init const
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_lw16i(REG2,writer_buffer);

    VoC_lw16i(REG4,0x100);

    VoC_shr32_ri(REG01,-(RC_TOP_BITS-1),DEFAULT);
    VoC_sw16_d(REG2,WriteBufP);
    VoC_sw32_d(REG01,LeftShift_1_RC_TOP_BITS);

    VoC_sw16_d(REG4,CONST_0x100_ADDR);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_rc_is_bit_0
 *
 * Description: rc bit
 *
 * Inputs:      REG3:prob
 *
 * Outputs:     REG3:prob
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/23/2011
 **************************************************************************************/
void Coolsand_rc_is_bit_0(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    //  uint32_t t = rc_is_bit_0_helper(rc, p);
    //rc_normalize(rc);
    //  if (rc->range < (1 << RC_TOP_BITS))
    VoC_shru32_ri(REG67,1,DEFAULT);
    VoC_movreg32(REG45,REG67,IN_PARALLEL);

    VoC_bnlt32_rd(Coolsand_rc_is_bit_0_L0,REG67,LeftShift_1_RC_TOP_BITS)
    //rc_do_normalize(rc);
    VoC_shru32_ri(REG45,-8,DEFAULT);//  rc->range <<= 8;
    VoC_shru32_ri(ACC1,-8,IN_PARALLEL); //(rc->code << 8)

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_or32_rr(ACC1,REG67,DEFAULT);//rc_code

    VoC_bnez16_r(Coolsand_rc_is_bit_0_L0,REG0)

    VoC_sw16_d(BITCACHE,BITCACHE_VALUE);
    VoC_lw16i_short(REG6,16,DEFAULT);

    VoC_blt16_rd(Coolsand_rc_is_bit_0_L0,REG6,BITCACHE_VALUE)

    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_jal(Coolsand_LzmaDmaInData);

    VoC_shr32_ri(REG67,16,DEFAULT);

    VoC_shr32_ri(REG67,-2,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_add32_rr(REG67,REG67,ACC0,IN_PARALLEL);

    VoC_pop32(ACC0,DEFAULT);


    VoC_sw32_d(REG67,GLOBAL_IN_PTR);

Coolsand_rc_is_bit_0_L0:

    VoC_shru32_ri(REG45,RC_MODEL_TOTAL_BITS-1,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_multf32_rp(ACC0,REG4,REG3,IN_PARALLEL);

    VoC_macX_rp(REG5,REG3,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_pop32(ACC0,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);//rc_bound

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_process_bit0
 *
 * Description: process bit 0
 *
 * Inputs:      REG4: prob
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/23/2011
 **************************************************************************************/
void Coolsand_process_bit0(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    //  rc_update_bit_0(rc, prob);

//  VoC_lw16_d(REG4,prob);

//  VoC_jal(Coolsand_rc_update_bit_0);

    //rc->range = rc->bound;
    //  *p += ((1 << RC_MODEL_TOTAL_BITS) - *p) >> RC_MOVE_BITS;

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);
    VoC_lw16i_short(REG2,8,IN_PARALLEL);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL;
    VoC_add16_rp(REG5,REG5,REG4,IN_PARALLEL);

    //  prob = (p + LZMA_LITERAL +(LZMA_LIT_SIZE * (((get_pos(wr) & literal_pos_mask) << lc)
    //          + (wr->previous_byte >> (8 - lc)))));

    VoC_lw16_d(REG4,writer_buffer_pos);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sw16_p(REG5,REG4,IN_PARALLEL);

    VoC_lw16_d(REG5,GLOBAL_lc);

    VoC_and16_rd(REG4,literal_pos_mask);//pos_state_mask

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG5,IN_PARALLEL);//must be IN_PARALLEL

    VoC_lw16i_short(REG1,1,DEFAULT);//  int mi = 1;
    VoC_sub16_rr(REG5,REG1,REG5,IN_PARALLEL);//must be IN_PARALLEL

    VoC_lw16_d(REG3,writer_previous_byte);

    VoC_shru16_rr(REG4,REG5,DEFAULT);
    VoC_shru16_rr(REG3,REG2,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_lw16i(REG2,LZMA_LIT_SIZE);

    VoC_mult16_rr(REG3,REG3,REG2,DEFAULT);

    VoC_lw16i(REG4,probs_p+LZMA_LITERAL);

    VoC_lw16i_short(REG5,LZMA_NUM_LIT_STATES,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_lw16_d(REG2,cstate_state);

    VoC_sw16_d(REG3,prob);

    //  if (cst->state >= LZMA_NUM_LIT_STATES)

    VoC_bgt16_rr(Coolsand_process_bit0_L3,REG5,REG2)

    //REG45:cst->rep0
    VoC_lw32_d(REG45,cstate_rep0);

    VoC_jal(Coolsand_peek_old_byte);
    //return REG4;match_byte

    VoC_lw16i(REG5,0x100);
Coolsand_process_bit0_L0:

    VoC_shru16_ri(REG4,-1,DEFAULT);

    VoC_and16_rr(REG4,REG5,DEFAULT);//bit = match_byte & 0x100;
    VoC_add16_rr(REG3,REG1,REG5,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);//match_byte

    VoC_add16_rd(REG4,REG4,prob);

    //prob_lit = prob + 0x100 + bit + mi;
    VoC_add16_rr(REG3,REG4,REG3,DEFAULT);
    VoC_push16(REG4,IN_PARALLEL);//bit

    //REG4:prob_lit

    VoC_jal(Coolsand_rc_get_bit);
    //REG2;return
    //REG1:mi

    VoC_pop16(REG3,DEFAULT);//bit
    VoC_pop32(REG45,IN_PARALLEL);//match_byte

    VoC_bez16_r(Coolsand_process_bit0_L1,REG2)

    //if (!bit) break;
    VoC_bez16_r(Coolsand_process_bit0_L3,REG3)

    VoC_jump(Coolsand_process_bit0_L2)
Coolsand_process_bit0_L1:

    //if (bit)  break;
    VoC_bnez16_r(Coolsand_process_bit0_L3,REG3)

Coolsand_process_bit0_L2:

//      VoC_lw16i(REG5,0x100);

    VoC_blt16_rd(Coolsand_process_bit0_L0,REG1,CONST_0x100_ADDR);


Coolsand_process_bit0_L3:

    //REG1:mi
//  VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);


//  VoC_lw16i(REG5,0x100);

    VoC_bnlt16_rd(Coolsand_process_bit0_L4,REG1,CONST_0x100_ADDR)

    VoC_add16_rd(REG3,REG1,prob);

    //REG1:mi
    //REG4:prob_lit
    VoC_jal(Coolsand_rc_get_bit);
    //REG2;return
    //REG1:mi

    VoC_jump(Coolsand_process_bit0_L3);

Coolsand_process_bit0_L4:

    //REG1:mi
    VoC_jal(Coolsand_write_byte);


    VoC_lw16i_short(REG5,4,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);



    VoC_bngt16_rd(Coolsand_process_bit0_L5,REG5,cstate_state)

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_jump(Coolsand_process_bit0_End);
Coolsand_process_bit0_L5:
    VoC_bngt16_rd(Coolsand_process_bit0_L6,REG3,cstate_state)

    VoC_lw16i_short(REG4,3,DEFAULT);

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_sub16_dr(REG4,cstate_state,REG4);

    VoC_jump(Coolsand_process_bit0_End);
Coolsand_process_bit0_L6:

    VoC_lw16i_short(REG4,6,DEFAULT);

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_sub16_dr(REG4,cstate_state,REG4);

Coolsand_process_bit0_End:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG4,cstate_state);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_process_bit1
 *
 * Description: process bit 1
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/27/2011
 **************************************************************************************/
void Coolsand_process_bit1(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound


//  VoC_lw16_d(REG4,prob);
//  VoC_jal(Coolsand_rc_update_bit_1);

    VoC_lw16_p(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG3,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG3,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG4,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG3,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG4,IN_PARALLEL);
    //VoC_lw16_d(REG3,prob);

    //  prob = p + LZMA_IS_REP + cst->state;

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i(REG3,probs_p+LZMA_IS_REP);

    VoC_add16_rd(REG3,REG3,cstate_state);

    //REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);

    //if rc->code < rc->bound ture else false;
    VoC_lw16i(STATUS,CARRY_SET);

    //VoC_sw16_d(REG3,prob);//save prob

    VoC_sub32_rr(REG23,ACC1,RL7,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_bncz16_r(Coolsand_process_bit1_L1);

    //  VoC_lw16_d(REG4,prob);
    //REG4:prob


    //cst->rep3 = cst->rep2;
    //cst->rep2 = cst->rep1;
    //cst->rep1 = cst->rep0;

    VoC_lw32_d(REG23,cstate_rep2);
    VoC_lw32_d(REG23,cstate_rep1);
    VoC_sw32_d(REG23,cstate_rep3);
    VoC_sw32_d(REG23,cstate_rep2);

    //VoC_jal(Coolsand_rc_update_bit_0);

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);
    VoC_lw16i_short(REG2,LZMA_NUM_LIT_STATES,IN_PARALLEL);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL;
    VoC_add16_rp(REG5,REG5,REG4,IN_PARALLEL);

    VoC_lw32_d(REG45,cstate_rep0);

    VoC_sw16_p(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_sw32_d(REG45,cstate_rep1);

    //cst->state = cst->state < LZMA_NUM_LIT_STATES ? 0 : 3;

    VoC_bngt16_rd(Coolsand_process_bit1_L0,REG2,cstate_state)

    VoC_lw16i_short(REG4,0,DEFAULT);
    //  VoC_NOP();
Coolsand_process_bit1_L0:

    //prob = p + LZMA_LEN_CODER;
    VoC_lw16i(REG3,probs_p+LZMA_LEN_CODER);

    VoC_sw16_d(REG4,cstate_state);

    VoC_sw16_d(REG3,prob);

    VoC_jump(Coolsand_process_bit1_L6);
Coolsand_process_bit1_L1:

    //  VoC_lw16_d(REG4,prob);
    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_1);
    VoC_lw16_p(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG3,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG3,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG4,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG3,DEFAULT);
    //  prob = p + LZMA_IS_REP_G0 + cst->state;
    VoC_lw16i(REG3,probs_p+LZMA_IS_REP_G0);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG4,IN_PARALLEL);

    VoC_add16_rd(REG3,REG3,cstate_state);

    //REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_sw16_d(REG3,prob);

    VoC_bncz16_r(Coolsand_process_bit1_L4);

    //VoC_jal(Coolsand_rc_update_bit_0);

    VoC_lw16_d(REG4,cstate_state);

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);
    VoC_shr16_ri(REG4,-LZMA_NUM_POS_BITS_MAX,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    //  prob = (p + LZMA_IS_REP_0_LONG+ (cst->state <<LZMA_NUM_POS_BITS_MAX) +  pos_state);
    VoC_lw16i(REG3,probs_p+LZMA_IS_REP_0_LONG);

    VoC_sw16_p(REG5,REG3,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_add16_rd(REG3,REG3,pos_state);

    //REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_process_bit1_L3);

    //      VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    //      VoC_movreg16(REG4,REG3,IN_PARALLEL);

    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_0);

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);

    VoC_lw16i_short(REG4,LZMA_NUM_LIT_STATES,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    //cst->state = cst->state < LZMA_NUM_LIT_STATES ? 9 : 11;
    //copy_byte(wr, cst->rep0);

    VoC_bngt16_rd(Coolsand_process_bit1_L2,REG4,cstate_state)

    VoC_lw16i_short(REG2,9,DEFAULT);
    //VoC_NOP();
Coolsand_process_bit1_L2:

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    VoC_sw16_d(REG2,cstate_state);
    VoC_lw32_d(REG45,cstate_rep0);

    //  VoC_jal(Coolsand_copy_byte);
    VoC_jal(Coolsand_peek_old_byte);
    //return REG4;match_byte

    VoC_movreg16(REG1,REG4,DEFAULT);

    VoC_jal(Coolsand_write_byte);

    VoC_jump(Coolsand_process_bit1_End);
Coolsand_process_bit1_L3:
    //REG3:prob

    //VoC_jal(Coolsand_rc_update_bit_1);

    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    VoC_jump(Coolsand_process_bit1_L5);

Coolsand_process_bit1_L4:

    //REG3:prob
    //VoC_jal(Coolsand_rc_update_bit_1);
    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    //  prob = p + LZMA_IS_REP_G1 + cst->state;
    VoC_lw16i(REG3,probs_p+LZMA_IS_REP_G1);

    VoC_add16_rd(REG3,REG3,cstate_state);

    //REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_process_bit1_Lx0);

    //REG4:prob
    //  VoC_jal(Coolsand_rc_update_bit_0);
    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw32_d(REG23,cstate_rep1);  //distance = cst->rep1;

    VoC_sw16_p(REG5,REG3,DEFAULT);

    VoC_jump(Coolsand_process_bit1_Lx1);
Coolsand_process_bit1_Lx0:
    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_1);
    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);
    //  prob = p + LZMA_IS_REP_G2 + cst->state;
    VoC_lw16i(REG3,probs_p+LZMA_IS_REP_G2);

    VoC_add16_rd(REG3,REG3,cstate_state);

    //REG3:prob
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob

    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_process_bit1_Lx2);
    //REG3:prob
    //VoC_jal(Coolsand_rc_update_bit_0);
    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL;
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw32_d(REG23,cstate_rep2);  //distance = cst->rep1;

    VoC_sw16_p(REG5,REG3,DEFAULT);

    VoC_jump(Coolsand_process_bit1_Lx3);
Coolsand_process_bit1_Lx2:
    //REG3:prob
    //VoC_jal(Coolsand_rc_update_bit_1);
    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    //distance = cst->rep3;
    //cst->rep3 = cst->rep2;
    VoC_lw32_d(REG45,cstate_rep2);
    VoC_lw32_d(REG23,cstate_rep3);
    VoC_sw32_d(REG45,cstate_rep3);

Coolsand_process_bit1_Lx3:
    //cst->rep2 = cst->rep1;
    VoC_lw32_d(REG45,cstate_rep1);
    VoC_NOP();
    VoC_sw32_d(REG45,cstate_rep2);

Coolsand_process_bit1_Lx1:
    //cst->rep1 = cst->rep0;
    //cst->rep0 = distance;

    VoC_lw32_d(REG45,cstate_rep0);
    VoC_sw32_d(REG23,cstate_rep0);
    VoC_sw32_d(REG45,cstate_rep1);

Coolsand_process_bit1_L5:
    //cst->state = cst->state < LZMA_NUM_LIT_STATES ? 8 : 11;


    VoC_lw16i_short(REG4,LZMA_NUM_LIT_STATES,DEFAULT);
    VoC_lw16i_short(REG5,11,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_process_bit1_L7,REG4,cstate_state)

    VoC_lw16i_short(REG5,8,DEFAULT);
    //VoC_NOP();
Coolsand_process_bit1_L7:

    //prob = p + LZMA_REP_LEN_CODER;

    VoC_lw16i(REG3,probs_p+LZMA_REP_LEN_CODER);

    VoC_sw16_d(REG5,cstate_state);

    VoC_sw16_d(REG3,prob);

Coolsand_process_bit1_L6:

    //  prob_len = prob + LZMA_LEN_CHOICE;

    VoC_lw16i_short(REG3,LZMA_LEN_CHOICE,DEFAULT);

    VoC_add16_rd(REG3,REG3,prob);

    //REG3:prob_len
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob_len
    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_process_bit1_L8);

    //REG3:prob
    //  VoC_jal(Coolsand_rc_update_bit_0);
    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw16_d(REG2,pos_state);

    VoC_sw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG3,LZMA_LEN_NUM_LOW_BITS,IN_PARALLEL);////num_bits = LZMA_LEN_NUM_LOW_BITS;

    //prob_len = (prob + LZMA_LEN_LOW+ (pos_state <<LZMA_LEN_NUM_LOW_BITS));
    //offset = 0;
    //num_bits = LZMA_LEN_NUM_LOW_BITS;

    VoC_shr16_ri(REG2,-LZMA_LEN_NUM_LOW_BITS,DEFAULT);
    VoC_lw16i_short(REG4,LZMA_LEN_LOW,IN_PARALLEL)

    VoC_add16_rr(REG4,REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);//offset=0

    VoC_add16_rd(REG4,REG4,prob);

    VoC_jump(Coolsand_process_bit1_L9);
Coolsand_process_bit1_L8:
    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_1);
    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_lw16i_short(REG3,LZMA_LEN_CHOICE_2,IN_PARALLEL);


    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    //  prob_len = prob + LZMA_LEN_CHOICE_2;
    VoC_add16_rd(REG3,REG3,prob);

    //REG3:prob_len
    VoC_jal(Coolsand_rc_is_bit_0);
    //REG3:prob_len

    VoC_lw16i(STATUS,CARRY_SET);
    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);

    VoC_bncz16_r(Coolsand_process_bit1_L10);

    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_0);

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw16_d(REG2,pos_state);

    VoC_sw16_p(REG5,REG3,DEFAULT);

    //prob_len = (prob + LZMA_LEN_MID + (pos_state << LZMA_LEN_NUM_MID_BITS));
    //offset = 1 << LZMA_LEN_NUM_LOW_BITS;
    //num_bits = LZMA_LEN_NUM_MID_BITS;

    VoC_shr16_ri(REG2,-LZMA_LEN_NUM_MID_BITS,DEFAULT);
    VoC_lw16i_short(REG3,LZMA_LEN_NUM_MID_BITS,IN_PARALLEL);////num_bits = LZMA_LEN_NUM_MID_BITS;

    VoC_lw16i(REG4,LZMA_LEN_MID);

    VoC_add16_rr(REG4,REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG1,1 << LZMA_LEN_NUM_LOW_BITS,IN_PARALLEL);//offset = 1 << LZMA_LEN_NUM_LOW_BITS;

    VoC_add16_rd(REG4,REG4,prob);


    VoC_jump(Coolsand_process_bit1_L9);

Coolsand_process_bit1_L10:

    //REG4:prob
    //VoC_jal(Coolsand_rc_update_bit_1);

    VoC_lw16_p(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG4,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG4,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG4,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    //prob_len = prob + LZMA_LEN_HIGH;
    //offset = ((1 << LZMA_LEN_NUM_LOW_BITS)+ (1 << LZMA_LEN_NUM_MID_BITS));
    //num_bits = LZMA_LEN_NUM_HIGH_BITS;

    VoC_lw16i(REG4,LZMA_LEN_HIGH);

    //num_bits = LZMA_LEN_NUM_HIGH_BITS;
    VoC_lw16i_short(REG3,LZMA_LEN_NUM_HIGH_BITS,DEFAULT);
    //offset = ((1 << LZMA_LEN_NUM_LOW_BITS)+ (1 << LZMA_LEN_NUM_MID_BITS));
    VoC_lw16i_short(REG1,(1 << LZMA_LEN_NUM_LOW_BITS)+ (1 << LZMA_LEN_NUM_MID_BITS),IN_PARALLEL);

    //prob_len = prob + LZMA_LEN_HIGH;
    VoC_add16_rd(REG4,REG4,prob);

Coolsand_process_bit1_L9:

    //REG4:prob_len;
    //REG3:num_bits;
    //REG1:offset;


//  rc_bit_tree_decode(rc, prob_len, num_bits, &len);
//  len += offset;

    VoC_push16(REG1,DEFAULT);//REG1:offset;

    VoC_jal(Coolsand_rc_bit_tree_decode);

    VoC_pop16(REG2,DEFAULT);//REG1:offset;
    VoC_lw16i_short(REG4,LZMA_NUM_LIT_STATES,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);//len += offset;
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    //if (cst->state < 4)

    VoC_lw16_d(REG3,cstate_state);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);//save len


    VoC_bngt16_rr(Coolsand_process_bit1_L11,REG2,REG3);
    //  cst->state += LZMA_NUM_LIT_STATES;

    VoC_lw16i_short(REG5,LZMA_NUM_LEN_TO_POS_STATES,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    //  prob =p + LZMA_POS_SLOT +((len < LZMA_NUM_LEN_TO_POS_STATES ? len : LZMA_NUM_LEN_TO_POS_STATES - 1) << LZMA_NUM_POS_SLOT_BITS);
    VoC_lw16i(REG2,probs_p+LZMA_POS_SLOT);

    VoC_sw16_d(REG3,cstate_state);

    VoC_lw16i_short(REG3,LZMA_NUM_LEN_TO_POS_STATES - 1,DEFAULT);

    VoC_bngt16_rr(Coolsand_process_bit1_L12,REG5,REG1)
    VoC_movreg16(REG3,REG1,DEFAULT);
Coolsand_process_bit1_L12:
    VoC_shr16_ri(REG3,-LZMA_NUM_POS_SLOT_BITS,DEFAULT);

    VoC_add16_rr(REG4,REG2,REG3,DEFAULT);
    VoC_lw16i_short(REG3,LZMA_NUM_POS_SLOT_BITS,IN_PARALLEL)

    //REG4:prob;
    //REG3:num_bits;

    //rc_bit_tree_decode(rc, prob, LZMA_NUM_POS_SLOT_BITS,  &pos_slot);

    VoC_jal(Coolsand_rc_bit_tree_decode);
    //REG1:pos_slot


    VoC_lw16i_short(REG2,LZMA_START_POS_MODEL_INDEX,DEFAULT);

    VoC_bgt16_rr(Coolsand_process_bit1_L13,REG2,REG1)

    //int i, mi;
    //num_bits = (pos_slot >> 1) - 1;
    //cst->rep0 = 2 | (pos_slot & 1);

    VoC_movreg16(REG2,REG1,DEFAULT);//pos_slot;
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_shru16_ri(REG1,1,IN_PARALLEL);

    VoC_and16_rr(REG2,REG4,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);//save pos_slot

    VoC_or16_rr(REG2,REG5,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);////num_bits=(pos_slot >> 1) - 1;

    VoC_movreg16(REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i_short(REG5,LZMA_END_POS_MODEL_INDEX,DEFAULT);

    //if (pos_slot < LZMA_END_POS_MODEL_INDEX)
    VoC_bngt16_rr(Coolsand_process_bit1_L15,REG5,REG4)
    //cst->rep0 <<= num_bits;
    //prob = p + LZMA_SPEC_POS +cst->rep0 - pos_slot - 1;

    VoC_lw16i(REG5,probs_p+LZMA_SPEC_POS);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG4,IN_PARALLEL);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL
    VoC_sub16_rr(REG5,REG5,REG4,IN_PARALLEL);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL
    VoC_sub16_rr(REG4,REG3,REG1,IN_PARALLEL);//0-numbits

    VoC_shru32_rr(REG23,REG4,DEFAULT);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);

    VoC_jump(Coolsand_process_bit1_L16);
Coolsand_process_bit1_L15:

    //num_bits -= LZMA_NUM_ALIGN_BITS;

    //while (num_bits--)
    //  cst->rep0 = (cst->rep0 << 1) | rc_direct_bit(rc);


    VoC_lw16i_short(REG5,LZMA_NUM_ALIGN_BITS,DEFAULT);

    VoC_movreg16(REG2,REG2,DEFAULT);//only for IN_PARALLEL
    VoC_sub16_rr(REG1,REG1,REG5,IN_PARALLEL);

    VoC_bez16_r(Coolsand_process_bit1_L17,REG1)

    VoC_loop_r_short(REG1,DEFAULT)
    VoC_startloop0

    VoC_jal(Coolsand_rc_direct_bit);

    VoC_endloop0

Coolsand_process_bit1_L17:

    //prob = p + LZMA_ALIGN;
    //cst->rep0 <<= LZMA_NUM_ALIGN_BITS;
    //num_bits = LZMA_NUM_ALIGN_BITS;

    VoC_lw16i(REG5,probs_p+LZMA_ALIGN);

    VoC_shru32_ri(REG23,-LZMA_NUM_ALIGN_BITS,DEFAULT);
    VoC_lw16i_short(REG1,LZMA_NUM_ALIGN_BITS,IN_PARALLEL);


Coolsand_process_bit1_L16:

    VoC_bez16_r(Coolsand_process_bit1_L14,REG1)

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_push16(REG3,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);//i

    VoC_sw16_d(REG5,prob_len);

    VoC_loop_r_short(REG1,DEFAULT)
    VoC_lw16i_short(REG1,1,IN_PARALLEL);//mi
    VoC_startloop0

    VoC_add16_rd(REG3,REG1,prob_len);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_jal(Coolsand_rc_get_bit);

    VoC_pop32(REG23,DEFAULT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);

    VoC_bez16_r(Coolsand_process_bit1_L18,REG5)

    VoC_or16_rr(REG2,REG3,DEFAULT);//   cst->rep0 |= i;

Coolsand_process_bit1_L18:
    VoC_shru16_ri(REG3,-1,DEFAULT);

    VoC_endloop0

    VoC_pop16(REG3,DEFAULT);


    VoC_jump(Coolsand_process_bit1_L14);


Coolsand_process_bit1_L13:
    //  cst->rep0 = pos_slot;
    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

Coolsand_process_bit1_L14:

    //      if (++(cst->rep0) == 0)
    //  return;

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL
    VoC_add32_rr(REG23,REG23,REG45,IN_PARALLEL);

    VoC_NOP();

    VoC_sw32_d(REG23,cstate_rep0);

    VoC_bnez32_r(Coolsand_process_bit1_L11,REG23)

    VoC_pop16(REG1,DEFAULT);//load len
    VoC_jump(Coolsand_process_bit1_End);

Coolsand_process_bit1_L11:

    //  len += LZMA_MATCH_MIN_LEN;

    VoC_pop16(REG1,DEFAULT);//load len
    VoC_lw16i_short(REG2,LZMA_MATCH_MIN_LEN,IN_PARALLEL);

    VoC_movreg16(REG0,REG0,DEFAULT);//only for IN_PARALLEL
    VoC_add16_rr(REG1,REG1,REG2,IN_PARALLEL);

    //REG1:len;
    //REG45:cst->rep0

//  VoC_jal(Coolsand_copy_bytes);



Coolsand_process_bit1_L19:

    VoC_lw32_d(REG45,cstate_rep0);

    VoC_push16(REG1,DEFAULT);

    VoC_jal(Coolsand_peek_old_byte);
    //return REG4;match_byte

    VoC_movreg16(REG1,REG4,DEFAULT);

    VoC_jal(Coolsand_write_byte);

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);


    VoC_bnez16_r(Coolsand_process_bit1_L19,REG1);



Coolsand_process_bit1_End:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_rc_update_bit_0
 *
 * Description: update bit 0
 *
 * Inputs:      REG4:prob
 *
 * Outputs:     REG4:prob
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/23/2011
 **************************************************************************************/
void Coolsand_rc_update_bit_0(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    //rc->range = rc->bound;
    //  *p += ((1 << RC_MODEL_TOTAL_BITS) - *p) >> RC_MOVE_BITS;

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG5,RC_MOVE_BITS,DEFAULT);

    VoC_movreg16(REG1,REG1,DEFAULT);//only for IN_PARALLEL;
    VoC_add16_rp(REG5,REG5,REG4,IN_PARALLEL);

    VoC_NOP();

    VoC_sw16_p(REG5,REG4,DEFAULT);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_rc_update_bit_1
 *
 * Description: update bit 1
 *
 * Inputs:      REG4:prob
 *
 * Outputs:     REG4:prob
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/23/2011
 **************************************************************************************/
void Coolsand_rc_update_bit_1(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_lw16_p(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG3,CARRY_SET,IN_PARALLEL);

    VoC_movreg16(STATUS,REG3,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_sub16_pr(REG5,REG4,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_movreg16(STATUS,REG3,DEFAULT);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_sw16_p(REG5,REG4,IN_PARALLEL);

    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_peek_old_byte
 *
 * Description: update bit 1
 *
 * Inputs:      REG45:offs
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/26/2011
 **************************************************************************************/
void Coolsand_peek_old_byte(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound
    /*
    Coolsand_peek_old_byte_L0:
    //      while (offs > wr->header->dict_size)
    VoC_bngt32_rd(Coolsand_peek_old_byte_L1,REG45,lzma_header_dict_size)
        VoC_lw16i(STATUS,CARRY_SET);
        //offs -= wr->header->dict_size;
        VoC_sub32_rd(REG45,REG45,lzma_header_dict_size);
        VoC_jump(Coolsand_peek_old_byte_L0);
    Coolsand_peek_old_byte_L1:*/

    VoC_lw16i(STATUS,CARRY_SET);

    //pos = wr->buffer_pos - offs;
    VoC_sub32_dr(REG23,writer_buffer_pos,REG45);

    VoC_lw32_d(REG45,OutputDMASize);

    //return wr->buffer[pos]；

    VoC_bgt32_rr(Coolsand_peek_old_byte_L2,REG45,REG23)

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sub32_rr(REG45,REG23,REG45,IN_PARALLEL);

    VoC_lw16i(REG5,writer_buffer);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_movreg16(REG3,REG4,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rr(REG2,REG5,REG4,IN_PARALLEL);

    VoC_and16_ri(REG3,0x0001);

    VoC_blt16_rd(Coolsand_peek_old_byte_L3,REG2,WriteBufP)

    VoC_lw16_d(REG5,WriteBufP);

    VoC_lw16i_short(REG4,3,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG5,IN_PARALLEL);

    VoC_shru16_ri(REG2,-1,DEFAULT);


    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG3,IN_PARALLEL);


    VoC_lw16_d(REG3,writer_buffer_pos);

    VoC_and16_rr(REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_sub16_rr(REG3,REG4,REG3,IN_PARALLEL);

    VoC_shr16_ri(REG2,-3,DEFAULT);
    VoC_shr16_ri(REG3,-3,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG3,DEFAULT);

    VoC_shru32_rr(REG45,REG2,DEFAULT);

    VoC_jump(Coolsand_peek_old_byte_End);
Coolsand_peek_old_byte_L3:

    VoC_lw16_p(REG4,REG2,DEFAULT);
    VoC_shr16_ri(REG3,-3,IN_PARALLEL);

    VoC_shru16_rr(REG4,REG3,DEFAULT);

    VoC_jump(Coolsand_peek_old_byte_End);
Coolsand_peek_old_byte_L2:

    // create single read pointer
    VoC_lw32_d(REG45,GLOBAL_OUT_PTR);

    VoC_movreg16(REG2,REG2,DEFAULT);////only for IN_PARALLEL
    VoC_add32_rr(REG45,REG23,REG45,IN_PARALLEL);

    VoC_and16_ri(REG4,0xfffc);

    VoC_and16_ri(REG5,0x0fff);

    VoC_or16_ri(REG5,(DMA_SINGLE >> 16));

    VoC_and16_ri(REG2,0x0003);
    // single read
    VoC_sw32_d(REG45,CFG_DMA_EADDR);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_shr16_ri(REG2,-3,IN_PARALLEL);

    VoC_lw32_d(REG45,CFG_DMA_DATA_SINGLE);

    VoC_shru32_rr(REG45,REG2,DEFAULT);

Coolsand_peek_old_byte_End:

    VoC_and16_ri(REG4,0x00ff);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_rc_get_bit
 *
 * Description:
 *
 * Inputs:      REG4:prob; REG1:mi
 *
 * Outputs:     REG2:return; REG4:prob; REG1:mi
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/26/2011
 **************************************************************************************/
void Coolsand_rc_get_bit(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    //VoC_jal(Coolsand_rc_is_bit_0);

    //  uint32_t t = rc_is_bit_0_helper(rc, p);
    //rc_normalize(rc);
    //  if (rc->range < (1 << RC_TOP_BITS))

    VoC_shru32_ri(REG67,1,DEFAULT);
    VoC_movreg32(REG45,REG67,IN_PARALLEL);

    VoC_bnlt32_rd(Coolsand_rc_get_bit_Coolsand_rc_is_bit_0_L0,REG67,LeftShift_1_RC_TOP_BITS)
    //rc_do_normalize(rc);
    VoC_shru32_ri(REG45,-8,DEFAULT);//  rc->range <<= 8;
    VoC_shru32_ri(ACC1,-8,IN_PARALLEL); //(rc->code << 8)

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_or32_rr(ACC1,REG67,DEFAULT);//rc_code

    VoC_bnez16_r(Coolsand_rc_get_bit_Coolsand_rc_is_bit_0_L0,REG0)

    VoC_sw16_d(BITCACHE,BITCACHE_VALUE);
    VoC_lw16i_short(REG6,16,DEFAULT);

    VoC_blt16_rd(Coolsand_rc_get_bit_Coolsand_rc_is_bit_0_L0,REG6,BITCACHE_VALUE)

    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_jal(Coolsand_LzmaDmaInData);

    VoC_shr32_ri(REG67,16,DEFAULT);

    VoC_shr32_ri(REG67,-2,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_add32_rr(REG67,REG67,ACC0,IN_PARALLEL);

    VoC_pop32(ACC0,DEFAULT);

    VoC_sw32_d(REG67,GLOBAL_IN_PTR);

Coolsand_rc_get_bit_Coolsand_rc_is_bit_0_L0:

    VoC_shru32_ri(REG45,RC_MODEL_TOTAL_BITS-1,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);

    VoC_push32(ACC0,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_multf32_rp(ACC0,REG4,REG3,IN_PARALLEL);

    VoC_macX_rp(REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG2,CARRY_SET,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_movreg16(STATUS,REG2,DEFAULT);
    VoC_movreg32(RL7,ACC0,IN_PARALLEL);//rc_bound

    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound
    //if rc->code < rc->bound ture else false;

    VoC_sub32_rr(REG45,ACC1,RL7,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);

    VoC_bncz16_r(Coolsand_rc_get_bit_L0);

//      VoC_jal(Coolsand_rc_update_bit_0);

    VoC_lw16i(REG5,1<<RC_MODEL_TOTAL_BITS);

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_add16_rp(REG5,REG5,REG3,IN_PARALLEL);

    VoC_jump(Coolsand_rc_get_bit_End);
Coolsand_rc_get_bit_L0:

//  VoC_jal(Coolsand_rc_update_bit_1);

    VoC_movreg16(STATUS,REG2,DEFAULT);
    VoC_lw16_p(REG5,REG3,IN_PARALLEL);

    VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);//must be DEFAULT
    VoC_shru16_ri(REG5,RC_MOVE_BITS,IN_PARALLEL);

    VoC_movreg16(STATUS,REG2,DEFAULT);
    VoC_sub16_pr(REG5,REG3,REG5,IN_PARALLEL);   //must be IN_PARALLEL

    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_sub32_rr(REG67,REG67,RL7,DEFAULT);//must be DEFAULT
    VoC_add16_rr(REG1,REG1,REG2,IN_PARALLEL);

Coolsand_rc_get_bit_End:

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sw16_p(REG5,REG3,IN_PARALLEL);

    VoC_return;//REG2:
}


/**************************************************************************************
 * Function:    Coolsand_write_byte
 *
 * Description: write a byte to buffer
 *
 * Inputs:      REG1:cbyte
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/26/2011
 **************************************************************************************/
void Coolsand_write_byte(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_and16_ri(REG1,0xff);

    VoC_lw32_d(REG23,writer_buffer_pos);

    VoC_sw16_d(REG1,writer_previous_byte);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_movreg16(REG4,REG1,DEFAULT);
    VoC_add32_rr(REG23,REG45,REG23,IN_PARALLEL);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_shru32_ri(REG45,-24,IN_PARALLEL);

    VoC_or32_rr(ACC0,REG45,DEFAULT);
    VoC_lw16i_short(REG1,0x3,IN_PARALLEL);

    VoC_sw32_d(REG23,writer_buffer_pos);

    VoC_and16_rr(REG2,REG1,DEFAULT);
    VoC_movreg32(REG45,REG23,IN_PARALLEL);

    VoC_bnez16_r(Coolsand_write_byte_L1,REG2)

    VoC_lw16d_set_inc(REG1,WriteBufP,2);

    VoC_and16_ri(REG4,0x3fff);//8192*2=16384;16384-1=16383=0x3fff;

    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_sw16_d(REG1,WriteBufP);

    VoC_bnez16_r(Coolsand_write_byte_L2,REG4);

    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);

    VoC_lw16i(REG5,writer_buffer/2);
    VoC_lw16i(REG4,8192/2);

    VoC_lw32_d(REG23,GLOBAL_OUT_PTR)

    VoC_add32_rd(REG23,REG23,OutputDMASize);

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG23,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shr16_ri(REG4,-2,IN_PARALLEL);

    VoC_lw16i(REG2,writer_buffer);

    VoC_add32_rd(REG45,REG45,OutputDMASize);

    VoC_sw16_d(REG2,WriteBufP);

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG45,OutputDMASize);

Coolsand_write_byte_L1:
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);


Coolsand_write_byte_L2:

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_copy_byte
 *
 * Description:
 *
 * Inputs:      REG45:offs.
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/27/2011
 **************************************************************************************/
void Coolsand_copy_byte(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_push16(RA,DEFAULT);

    VoC_jal(Coolsand_peek_old_byte);
    //return REG4;match_byte

    VoC_movreg16(REG1,REG4,DEFAULT);

    VoC_jal(Coolsand_write_byte);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_rc_bit_tree_decode
 *
 * Description:
 *
 * Inputs:      REG4:prob_len;
 *              REG3:num_bits;
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/27/2011
 **************************************************************************************/
void Coolsand_rc_bit_tree_decode(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_push16(RA,DEFAULT)
    VoC_push32(REG23,IN_PARALLEL);

    VoC_sw16_d(REG4,prob_len);

    VoC_loop_r_short(REG3,DEFAULT)
    VoC_lw16i_short(REG1,1,IN_PARALLEL);
    VoC_startloop0
    //REG1:mi
    //REG4:prob_lit

    VoC_add16_rd(REG3,REG1,prob_len);

    VoC_jal(Coolsand_rc_get_bit);

    //REG2;return
    //REG1:mi
    VoC_endloop0

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_sub16_rr(REG3,REG5,REG3,IN_PARALLEL);

    VoC_shr16_rr(REG5,REG3,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    //*symbol -= 1 << num_levels;
    VoC_lw16i_short(STATUS,CARRY_CLR,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG5,IN_PARALLEL);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_copy_bytes
 *
 * Description:
 *
 * Inputs:          REG1:len;
 *                  REG45:cst->rep0
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/28/2011
 **************************************************************************************/
void Coolsand_copy_bytes(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_push16(RA,DEFAULT);

    VoC_loop_r_short(REG1,DEFAULT);
    VoC_startloop0

    VoC_push32(REG45,DEFAULT);

    VoC_jal(Coolsand_peek_old_byte);
    //return REG4;match_byte

    VoC_movreg16(REG1,REG4,DEFAULT);

    VoC_jal(Coolsand_write_byte);

    VoC_pop32(REG45,DEFAULT);

    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_rc_direct_bit
 *
 * Description:
 *
 * Inputs:      REG23:cst->rep0
 *
 *
 * Outputs:     REG45:return;REG23:cst->rep0
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       12/28/2011
 **************************************************************************************/
void Coolsand_rc_direct_bit(void)
{
    //fix the variables to these regs
    //REG67:rc_range
    //ACC1:rc_code
    //RL7:rc_bound

    VoC_shru32_ri(REG67,1,DEFAULT);
    VoC_movreg32(REG45,REG67,IN_PARALLEL);

    VoC_bnlt32_rd(Coolsand_rc_direct_bit_L0,REG67,LeftShift_1_RC_TOP_BITS)
    //rc_do_normalize(rc);
    VoC_shru32_ri(REG45,-8,DEFAULT);//  rc->range <<= 8;
    VoC_shru32_ri(ACC1,-8,IN_PARALLEL); //(rc->code << 8)

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_or32_rr(ACC1,REG67,DEFAULT);//rc_code

    VoC_bnez16_r(Coolsand_rc_direct_bit_L0,REG0)

    VoC_sw16_d(BITCACHE,BITCACHE_VALUE);
    VoC_lw16i_short(REG6,16,DEFAULT);

    VoC_blt16_rd(Coolsand_rc_direct_bit_L0,REG6,BITCACHE_VALUE)

    VoC_push16(RA,DEFAULT);
    VoC_push32(ACC0,IN_PARALLEL);

    VoC_jal(Coolsand_LzmaDmaInData);

    VoC_shr32_ri(REG67,16,DEFAULT);


    VoC_shr32_ri(REG67,-2,DEFAULT);

    VoC_pop16(RA,DEFAULT);
    VoC_add32_rr(REG67,REG67,ACC0,IN_PARALLEL);

    VoC_pop32(ACC0,DEFAULT);

    VoC_sw32_d(REG67,GLOBAL_IN_PTR);

Coolsand_rc_direct_bit_L0:
    VoC_movreg32(REG67,REG45,DEFAULT);

//  if (rc->code >= rc->range)
    VoC_lbinc_p(REG0);

    VoC_lw16i(STATUS,CARRY_SET);

    VoC_shru32_ri(REG67,1,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_sub32_rr(REG45,ACC1,REG67,DEFAULT);
    VoC_shru32_ri(REG23,-1,IN_PARALLEL);

    VoC_bcz16_r(Coolsand_rc_direct_bit_L1)

    VoC_or16_rr(REG2,REG1,DEFAULT);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);

Coolsand_rc_direct_bit_L1:


    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_LzmaDmaInData
 *
 * Description:
 *
 * Inputs:      REG23:cst->rep0
 *
 *
 * Outputs:     REG45:return;REG23:cst->rep0
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       01/06/2021
 **************************************************************************************/
void Coolsand_LzmaDmaInData(void)
{
    VoC_lw16i(REG6,GLOBAL_INBUF/2);
    VoC_lw16i(REG7,1024/2);
    VoC_lw32_d(ACC0,GLOBAL_IN_PTR);

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}

