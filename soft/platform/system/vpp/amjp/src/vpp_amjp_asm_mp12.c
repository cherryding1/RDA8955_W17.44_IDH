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



#include "vppp_amjp_asm_map.h"
#include "vppp_amjp_asm_mp3.h"
#include "vppp_amjp_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


/*
 * NAME:    I_sample()
 * INPUT:  REG0 for ptr REG4 for nb
 * OUTPUT: REG7
 * USED: REG4,REG5,REG6,REG7
 * DESCRIPTION: decode one requantized Layer I sample from a bitstream
 */
void CII_I_sample (void)
{

#if 0
    voc_short TABLE_offset_table_ADDR   ,90 ,y
    voc_short TABLE_linear_table_ADDR   ,14 ,y
    voc_short TABLE_sf_table_ADDR       ,64 ,y
    voc_word  TABLE_sbquant_table_ADDR  ,160,y
    voc_short TABLE_bitalloc_table_ADDR ,16 ,y
    voc_short TABLE_qc_table_ADDR       ,20 ,y
#endif


    VoC_rbinc_r(REG6,REG4,DEFAULT); // REG5 for sample  sample = mad_bit_read(ptr, nb);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    /* invert most significant bit, extend sign, then scale to fixed format */
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);   // REG4 for 1-nb for shr
    VoC_shru16_rr(REG5,REG4,DEFAULT);
    VoC_movreg16(ACC0_LO,REG5,DEFAULT);

    VoC_xor32_rr(REG67,ACC0,DEFAULT);   //  sample ^= 1 << (nb - 1);
    VoC_movreg32(ACC1,REG67,DEFAULT);

    VoC_and32_rr(ACC1,ACC0,DEFAULT);
    VoC_sub32_dr(ACC1,CONST_0_ADDR,ACC1);
    VoC_or32_rr(REG67,ACC1,DEFAULT);        //  sample |= -(sample & (1 << (nb - 1)));
    VoC_lw16i_short(REG5,-28,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_shru32_rr(REG67,REG5,DEFAULT);  //  sample <<= MAD_F_FRACBITS - (nb - 1);
    VoC_lw32z(ACC1,IN_PARALLEL);
    /* requantize the sample */

    /* s'' = (2^nb / (2^nb - 1)) * (s''' + 2^(-nb + 1)) */
    VoC_lw16i(ACC1_HI,0X1000);

    VoC_sub16_dr(REG4,CONST_0_ADDR,REG4);
    VoC_shr32_rr(ACC1,REG4,DEFAULT);
    VoC_add32_rr(REG67,ACC1,REG67,DEFAULT);     //  sample += MAD_F_ONE >> (nb - 1);
    VoC_add32_rd(REG67,REG67,CONST_0x00000800_ADDR);
    VoC_lw16i(REG5,TABLE_linear_table_ADDR);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_sub16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_shru16_ri(REG6,12,DEFAULT);

    VoC_multf32_rp(ACC0,REG7,REG5,DEFAULT);
    VoC_NOP();
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_mac32_rp(REG6,REG5,DEFAULT);
    VoC_return;

}

/*
 * NAME:    layer->I()
 * INPUT:   REG0 for ptr inc 1
 * OUTPUT:  REG1 for return value
 * DESCRIPTION: decode a single Layer I frame
 */
void CII_mad_layer_I(void)
{
    VoC_push16(RA,DEFAULT);
    /*****************  DMA mpeg layer 1 and 2 Const ************************/
    VoC_lw32_d(ACC0,GLOBAL_MP12_CONST_Y_PTR);
    VoC_lw16i(REG6,TABLE_offset_table_ADDR/2);//local addr
    VoC_lw16i(REG7,MP12_DEC_CONST_Y_SIZE);//const size

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA mpeg layer 1 and 2 Const ************************/


    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16i_short(FORMATX,0,DEFAULT);

    VoC_lw16i_short(REG6,2,DEFAULT);    //  nch = MAD_NCHANNELS(header);
    VoC_bnez16_d(mad_layer_I_101, STRUCT_MAD_HEADER_MODE_ADDR)
    VoC_lw16i_short(REG6,1,DEFAULT);        // REG6 for nch
mad_layer_I_101:
    VoC_lw16_d(REG5,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_LAYER_ADDR);
    VoC_and16_ri(REG5,0X1000);
    VoC_be16_rd(mad_layer_I_201,REG4,CONST_2_ADDR)
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_be16_rd(mad_layer_I_201,REG4,CONST_1_ADDR)
    VoC_bez16_r(mad_layer_I_201,REG5)
    VoC_lw16i_short(REG7,18,DEFAULT);
mad_layer_I_201:
    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_sw32_d(REG67,GLOBAL_NCH_ADDR);
    VoC_lw16i_short(REG7,2,DEFAULT);    // REG7 for MAD_MODE_JOINT_STEREO
    VoC_lw16i_short(REG5,32,IN_PARALLEL);   // REG5 for bound  bound = 32;
    VoC_bne16_rd(mad_layer_I_102,REG7, STRUCT_MAD_HEADER_MODE_ADDR)     // if (header->mode == MAD_MODE_JOINT_STEREO) {
    VoC_or16_ri(REG4,0X0100);       //  header->flags |= MAD_FLAG_I_STEREO;
    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_mult16_rd(REG3,REG2,STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR);
    VoC_sw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_add16_rr(REG5,REG2,REG3,DEFAULT);       // bound = 4 + header->mode_extension * 4;

mad_layer_I_102:

    VoC_and16_ri(REG4,0X0010);
    VoC_bez16_r(mad_layer_I_103,REG4)

    //if (header->flags & MAD_FLAG_PROTECTION) {

    /*header->crc_check =
      mad_bit_crc(start, mad_bit_length(&start, &stream->ptr),
          header->crc_check);*/
    VoC_push32(REG67,DEFAULT);
    VoC_jal(CII_mad_bit_crc);
    VoC_pop32(REG67,DEFAULT);
    /*  if (header->crc_check != header->crc_target && !(frame->options & MAD_OPTION_IGNORECRC)) {
      stream->error = MAD_ERROR_BADCRC;
      return -1;
      */
    VoC_lw32_d(REG23,STRUCT_MAD_HEADER_CRC_CHECK_ADDR);
    VoC_be16_rr(mad_layer_I_103,REG2,REG3)
    VoC_lw16_d(REG2,STRUCT_MAD_FRAME_OPTIONS_ADDR);
    VoC_and16_ri(REG2,1);
    VoC_bnez16_r(mad_layer_I_103,REG2)
    VoC_lw16i(REG4,0X0201);
    VoC_lw16i_short(REG1,-1,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_sw16_d(REG4,STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_return;
mad_layer_I_103:
    VoC_lw16i_short(REG3,31,DEFAULT);
    VoC_lw16i(REG2,1151*2);
    VoC_be16_rd(mad_layer_II_a110,REG6,CONST_1_ADDR)
    VoC_lw16i(REG3,63);
    VoC_lw16i(REG2,2303*2);
mad_layer_II_a110:
    //VoC_lw16i(REG5,layer_II_BACK_JUMP1_addr);
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,32);
    VoC_sw32_d(REG23,layer_II_BACK_JUMP1_addr);
    /* decode bit allocations */
    VoC_loop_r(1,REG5)          // for (sb = 0; sb < bound; ++sb) {
    VoC_loop_r_short(REG6,DEFAULT)  //  for (ch = 0; ch < nch; ++ch)
    VoC_startloop0
    VoC_rbinc_i(REG4,4,DEFAULT);    // REG4 for nb  nb = mad_bit_read(&stream->ptr, 4);
    VoC_bne16_rd(mad_layer_I_105,REG4,CONST_15_ADDR)    // if (nb == 15)
    VoC_lw16i(REG4,MAD_ERROR_BADBITALLOC);              // stream->error = MAD_ERROR_BADBITALLOC;
    VoC_lw16i_short(REG1,-1,DEFAULT);   //  return -1;
    VoC_pop16(RA,IN_PARALLEL);
    VoC_sw16_d(REG4,STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_return;
mad_layer_I_105:
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_bez16_r(mad_layer_I_106,REG4)       //  allocation[ch][sb] = nb ? nb + 1 : 0;
    VoC_add16_rd(REG2,REG4,CONST_1_ADDR);
mad_layer_I_106:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_endloop0
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP_addr);
    VoC_endloop1

    VoC_add16_rd(REG2,REG1,CONST_32_ADDR);
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_sub16_dr(REG3,CONST_32_ADDR,REG5);
    VoC_bngtz16_r(mad_layer_I_107,REG3)
    VoC_loop_r_short(REG3,DEFAULT)          //  for (sb = bound; sb < 32; ++sb)
    VoC_startloop0
    VoC_rbinc_i(REG4,4,DEFAULT);    // REG4 for nb  nb = mad_bit_read(&stream->ptr, 4);
    VoC_bne16_rd(mad_layer_I_108,REG4,CONST_15_ADDR)    // if (nb == 15)
    VoC_lw16i(REG5,0X211);              // stream->error = MAD_ERROR_BADBITALLOC;
    VoC_lw16i_short(REG1,0XFFFF,DEFAULT);   //  return -1;
mad_layer_I_108:
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_bez16_r(mad_layer_I_109,REG4)       //  allocation[ch][sb] = nb ? nb + 1 : 0;
    VoC_add16_rd(REG3,REG4,CONST_1_ADDR);
mad_layer_I_109:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG3,REG1,DEFAULT);   // allocation[0][sb] =allocation[1][sb] = nb ? nb + 1 : 0;
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    VoC_endloop0
mad_layer_I_107:

    /* decode scalefactors */

    VoC_movreg16(RL7_HI,REG7,DEFAULT);
    VoC_movreg16(RL7_LO,REG5,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,32);
    VoC_lw16i_set_inc(REG2,layer_II_scalefactor_addr,32);
    VoC_sw32_d(RL7,layer_II_bound_addr);
    VoC_loop_i(1,32)                    //for (sb = 0; sb < 32; ++sb)
    VoC_loop_r_short(REG6,DEFAULT)  //  for (ch = 0; ch < nch; ++ch)
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_bez16_r(mad_layer_I_110,REG3)       // if (allocation[ch][sb])
    VoC_rbinc_i(REG4,6,DEFAULT);    // REG4     scalefactor[ch][sb] = mad_bit_read(&stream->ptr, 6);
    VoC_lw16i(REG3,63);
    VoC_bne16_rr(mad_layer_I_110,REG4,REG3)
//          VoC_bne16_rd(mad_layer_I_110,REG4,CONST_63_ADDR)        // if (scalefactor[ch][sb] == 63)
    VoC_lw16i(REG4,MAD_ERROR_BADSCALEFACTOR);               // stream->error = MAD_ERROR_BADSCALEFACTOR
    VoC_lw16i_short(REG1,0XFFFF,DEFAULT);   //  return -1;
    VoC_pop16(RA,IN_PARALLEL);
    VoC_sw16_d(REG4,STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_return;
mad_layer_I_110:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_endloop0
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP_addr);
    VoC_sub16_rd(REG2,REG2,layer_II_BACK_JUMP_addr);
    VoC_endloop1


    VoC_lw16i_set_inc(REG2,STATIC_MAD_SBSAMPLE_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_MAD_SBSAMPLE_ADDR+36*32*2,2);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_loop_i(1,36)
    VoC_loop_i_short(32,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_movreg16(RL7_HI,REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
mad_layer_I_121:
    VoC_blt16_rd(no_mad_layer_I_120,REG7,CONST_12_ADDR)     //for (gr = 0; gr < 12; ++gr)
    VoC_jump(mad_layer_I_120);
no_mad_layer_I_120:
    VoC_lw16i(REG1, 64);
//      VoC_mult16_rd(REG2,REG7,CONST_64_ADDR);
    VoC_mult16_rr(REG2,REG7,REG1,DEFAULT);
    VoC_lw16i(REG1, STATIC_MAD_SBSAMPLE_ADDR);
    VoC_lw16i_set_inc(REG3,layer_II_scalefactor_addr,32);
    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_lw16i_set_inc(REG2,TABLE_sf_table_ADDR,1);
    VoC_push16(REG7,DEFAULT);//,"GR"
    VoC_sw32_d(REG23,TABLE_sf_table_ADDR_ADDR);
    VoC_lw16i_short(REG5,0,DEFAULT);    // REG5 for sb
    VoC_lw16i_set_inc(REG2,layer_II_allocation_addr,32);
mad_layer_I_123:
    VoC_blt16_rd(no_mad_layer_I_122,REG5,layer_II_bound_addr)       //for (sb = 0; sb < bound; ++sb)
    VoC_jump(mad_layer_I_122);
no_mad_layer_I_122:
    VoC_push16(REG5,DEFAULT);//,"SB"
    VoC_loop_r_short(REG6,DEFAULT)      //  for (ch = 0; ch < nch; ++ch)
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bez16_r(mad_layer_I_124,REG4)
    VoC_jal(CII_I_sample);
mad_layer_I_124:
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_add16_rd(REG5,REG5,TABLE_sf_table_ADDR_ADDR);
    VoC_lbinc_p(REG0);
    VoC_add32_rd(REG67,ACC0,CONST_0x00000800_ADDR);
    VoC_multf32_rp(ACC1,REG7,REG5,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32_rp(REG6,REG5,IN_PARALLEL);

    VoC_lbinc_p(REG0);
    VoC_lw16i(REG6,2304);
//               VoC_add16_rd(REG1,REG1,CONST_2304_ADDR);
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32_p(ACC1,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP1_addr);
    VoC_sub16_rd(REG2,REG2,layer_II_BACK_JUMP_addr);
    VoC_sub16_rd(REG3,REG3,layer_II_BACK_JUMP_addr);

    VoC_pop16(REG5,DEFAULT);
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_jump(mad_layer_I_123);

mad_layer_I_122:
    VoC_sub16_dr(REG4,CONST_32_ADDR,REG5);
    VoC_lw16i_set_inc(REG2,layer_II_allocation_addr,1);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw16i_set_inc(REG3,layer_II_scalefactor_addr,32);
    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_bngtz16_r(mad_layer_I_126,REG4)
    VoC_loop_r(1,REG4)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_bez16_r(mad_layer_I_125,REG4)
    VoC_jal(CII_I_sample);
mad_layer_I_125:
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG3,DEFAULT);
    VoC_add16_rd(REG5,REG5,TABLE_sf_table_ADDR_ADDR);
    VoC_add32_rd(REG67,ACC0,CONST_0x00000800_ADDR);

    VoC_multf32_rp(ACC1,REG7,REG5,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC1,-4,DEFAULT);
    VoC_mac32_rp(REG6,REG5,IN_PARALLEL);
    VoC_lbinc_p(REG0);
    VoC_lbinc_p(REG0);
    VoC_lw16i(REG6,2304);

    VoC_sw32_p(ACC1,REG1,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);
//              VoC_add16_rd(REG1,REG1,CONST_2304_ADDR);
    VoC_endloop0
    VoC_movreg16(REG6,RL7_HI,DEFAULT);
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP1_addr);
    VoC_sub16_rd(REG3,REG3,layer_II_BACK_JUMP_addr);

    VoC_endloop1
mad_layer_I_126:
    VoC_pop16(REG7,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_jump(mad_layer_I_121);
mad_layer_I_120:
    VoC_pop16(RA,DEFAULT);

    /*  {
            int i ;
           for (i = 0;i < 2; i++)
            {

            if (fwrite ( &RAM_Y[(STATIC_MAD_SBSAMPLE_ADDR +2304*i - RAM_Y_BEGIN_ADDR)/2], sizeof (Word32), 32*36, file_SB) != 32*36) {
                fprintf(stderr, "\nerror writing sbsample  file: \n");
                return -1;
            }
            fflush(file_SB);
            }
        }*/

    VoC_lw16i_short(WRAP0,16,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_sw32_d(RL6,STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_return;
}




/*
 * NAME:    II_samples()
 * INPUT:   REG0 for ptr, REG2 for quantclass address
 * OUTPUT:  save in layer_II_samples_addr
 * DESCRIPTION: decode three requantized Layer II samples from a bitstream
 * USED REGISTER:   REG0, REG2,REG3,REG5,REG6,REG7, ACC1_LO
 */
void CII_II_samples(void)
{
#if 0
    voc_word layer_II_samples_addr, 3, y
    voc_short layer_II_TEMP_ADDR, 2, y
    voc_short TABLE_sf_table_ADDR_ADDR, y
    voc_short layer_II_scalefactor_ADDR_ADDR, y
    voc_short layer_II_SBSAMPLE_SFTABLE_ADDR, y
    voc_short layer_II_SFTABLE_ADDR, y
    voc_short layer_II_bound_addr, y
    voc_short layer_II_sblimit_addr, y
#endif
//  REGS[1].reg = s;
    VoC_push16(RA,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);//,"RL7_II_samples"
    VoC_lw16inc_p(REG7,REG2,DEFAULT);   // REG7 for nlevels
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG2,DEFAULT);   // REG6 for quantclass->group


    VoC_movreg16(REG5,REG6,DEFAULT);
    VoC_shru16_ri(REG5,8,DEFAULT);
    VoC_and16_ri(REG6,0x00ff);
    VoC_inc_p(REG2,DEFAULT);       //xuml add

//  VoC_lw16inc_p(REG5,REG2,DEFAULT);   // REG5 for quantclass->bits
    VoC_lw16inc_p(ACC1_LO,REG2,DEFAULT);    // ACC1_LO for quantclass->C
    // REG2 for quantclass->D
    VoC_lw16i_set_inc(REG3,layer_II_samples_addr,2);
    VoC_bez16_r(CII_II_samples101,REG6)     // if ((nb = quantclass->group))
    VoC_push16(REG6,DEFAULT);       // push nb
    VoC_rbinc_r(REG6,REG5,DEFAULT);         // REG6 for c = mad_bit_read(ptr, quantclass->bits);
    VoC_loop_i_short(3,DEFAULT)         // for (s = 0; s < 3; ++s)
    VoC_startloop0
    VoC_jal(CII_div_mode);          // sample[s] = c % nlevels;  c /= nlevels;
    VoC_movreg16(RL7_LO,REG6,DEFAULT);
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);

    VoC_endloop0
    VoC_jump(CII_II_samples102);
CII_II_samples101:
    VoC_push16(REG5,DEFAULT);       // push nb
    VoC_loop_i_short(3,DEFAULT)         // for (s = 0; s < 3; ++s)
    VoC_startloop0
    VoC_rbinc_r(REG6,REG5,DEFAULT);         //  sample[s] = mad_bit_read(ptr, nb);
    VoC_movreg16(RL7_LO,REG6,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0

CII_II_samples102:
    VoC_lw16i_set_inc(REG3,layer_II_samples_addr,2);
    VoC_pop16(REG5,DEFAULT);        // push nb
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);//,"RL7_II_samples"

    VoC_sub16_dr(REG5,CONST_1_ADDR,REG5);   //REG5 for 1-nb
    VoC_push32(ACC0,DEFAULT);
    /* invert most significant bit, extend sign, then scale to fixed format */
    VoC_loop_i_short(3,DEFAULT)         // for (s = 0; s < 3; ++s)
    VoC_startloop0
    /* requantize the sample */
    VoC_lw32_p(ACC0,REG3,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_shru32_rr(REG67,REG5,DEFAULT);  // reg67 for (1 << (nb - 1))
    VoC_xor32_rr(ACC0,REG67,DEFAULT);   // ACC1 for requantized  requantized  = sample[s] ^ (1 << (nb - 1));
    VoC_and32_rr(REG67,ACC0,DEFAULT);
    VoC_sub32_dr(REG67,CONST_0_ADDR,REG67);
    VoC_or32_rr(ACC0,REG67,DEFAULT);        //  requantized |= -(requantized & (1 << (nb - 1)));
    VoC_push16(REG5,DEFAULT);//,"NB"
    VoC_add16_rd(REG5,REG5,CONST_28_ADDR);
    VoC_sub16_dr(REG5,CONST_0_ADDR,REG5);
    VoC_shr32_rr(ACC0,REG5,DEFAULT);    // REG7 for requant_16 requantized <<= MAD_F_FRACBITS - (nb - 1);

    VoC_add32_rp(ACC0,ACC0,REG2,DEFAULT);         //  requantized = requant_16 + quantclass->D;
    VoC_add32_rd(REG67,ACC0,CONST_0x00000800_ADDR);

    VoC_movreg16(REG5,ACC1_LO,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG6,REG5,DEFAULT);     // output[s] = mad_f_mul(requantized + quantclass->D, quantclass->C);
    VoC_NOP();
    VoC_pop16(REG5,DEFAULT);//,"NB"
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0
    /*   if(output[s])
        {
        requantized=1;
        }
        seems no use ,notes by Stephen
    */
    VoC_pop32(ACC0,DEFAULT);
    VoC_return;
}

/*
 * NAME:    CII_mad_layer_II()
 * INPUT:   REG0 for ptr inc 1
 * OUTPUT: REG1 for return value
 * DESCRIPTION: decode a single Layer II frame
 */
void CII_mad_layer_II(void)
{
#if 0
    voc_short layer_II_allocation_addr, 64, y
    voc_short layer_II_scfsi_addr, 64, y
    voc_short layer_II_scalefactor_addr, 192, y
    voc_short TABLE_bitalloc_table_ADDR_ADDR, 2, y
    voc_short layer_II_BACK_JUMP1_addr, y
    voc_short layer_II_BACK_JUMP_addr, y
#endif


    VoC_push16(RA,DEFAULT);

    /*****************  DMA mpeg layer 1 and 2 Const ************************/
    VoC_lw32_d(ACC0,GLOBAL_MP12_CONST_Y_PTR);
    VoC_lw16i(REG6,TABLE_offset_table_ADDR/2);//local addr
    VoC_lw16i(REG7,MP12_DEC_CONST_Y_SIZE);//const size

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG7,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /*****************  DMA mpeg layer 1 and 2 Const ************************/


    VoC_lw16i_short(WRAP0,10,DEFAULT);
    VoC_lw16i_short(REG6,2,DEFAULT);    //  nch = MAD_NCHANNELS(header);
    VoC_lw16i_short(REG7,36,IN_PARALLEL);   //  ns  = MAD_NSBSAMPLES(&frame->header);
    VoC_bnez16_d(mad_layer_II_101, STRUCT_MAD_HEADER_MODE_ADDR)
    VoC_lw16i_short(REG6,1,DEFAULT);        // REG6 for nch
mad_layer_II_101:
    VoC_lw16_d(REG5,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_LAYER_ADDR);
    VoC_and16_ri(REG5,0X1000);
    VoC_be16_rd(mad_layer_II_201,REG4,CONST_2_ADDR)
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_be16_rd(mad_layer_II_201,REG4,CONST_1_ADDR)
    VoC_bez16_r(mad_layer_II_201,REG5)
    VoC_lw16i_short(REG7,18,DEFAULT);
mad_layer_II_201:
    VoC_lw16_d(REG1,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_sw32_d(REG67,GLOBAL_NCH_ADDR);

    VoC_lw16i_short(REG7,4,DEFAULT);    // REG7 for index    index = 4;
    VoC_and16_ri(REG1,0X1000);
    VoC_bnez16_r(mad_layer_II_102,REG1)     // if (header->flags & MAD_FLAG_LSF_EXT)
    VoC_lw32_d(REG23,STRUCT_MAD_HEADER_BITRATE_ADDR); // REG2 for bitrate REG3 for samplerate
    //switch (nch == 2 ? header->bitrate / 2 : header->bitrate)
    VoC_bne16_rd(mad_layer_II_103, REG6, CONST_2_ADDR)
    VoC_shr16_ri(REG2,1,DEFAULT);
mad_layer_II_103:
    VoC_bne16_rd(mad_layer_II_104,REG2,CONST_32_ADDR)    // case 32:
    VoC_lw16i(REG7,48);
//      VoC_bne16_rd(mad_layer_II_104,REG2,CONST_48_ADDR)    // case 48:
    VoC_bne16_rr(mad_layer_II_104,REG2,REG7)    // case 48:

    VoC_lw16i_short(REG7,3,DEFAULT);
    //index = (header->samplerate == 32000) ? 3 : 2;
    VoC_lw16i(REG1,32000);
    VoC_be16_rr(mad_layer_II_102,REG3,REG1)
//      VoC_be16_rd(mad_layer_II_102,REG3,CONST_32000_ADDR)
    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_jump(mad_layer_II_102);         // break;
mad_layer_II_104:
    VoC_lw16i_short(REG7,0,DEFAULT);     // index = 0;
    VoC_lw16i(REG3,56);
    VoC_be16_rr(mad_layer_II_102,REG2,REG3)    // case 56:
    VoC_lw16i(REG3,64);
    VoC_be16_rr(mad_layer_II_102,REG2,REG3)    // case 64:
    VoC_lw16i(REG3,80);
    VoC_be16_rr(mad_layer_II_102,REG2,REG3)    // case 80:

    // default:
    //  index = (header->samplerate == 48000) ? 0 : 1;
    VoC_lw16i(REG1,48000);
    VoC_be16_rr(mad_layer_II_102,REG3,REG1)
//      VoC_be16_rd(mad_layer_II_102,REG3,CONST_48000_ADDR)
    VoC_lw16i_short(REG7,1,DEFAULT);
mad_layer_II_102:
    // REG7 for index, REG6 for nch

    //  sblimit = sbquant_table[index].sblimit;
    //  offsets = sbquant_table[index].offsets;
    VoC_mult16_rd(REG7,REG7,CONST_32_ADDR);
    VoC_lw16i_set_inc(REG3,TABLE_sbquant_table_ADDR,1);     // REG3 for sbquant_table address
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);   // REG3 for offsets address
    VoC_add16_rd(REG1,REG3,CONST_30_ADDR);  // REG1 for sblimit address
    VoC_push16(REG3,DEFAULT);

    VoC_lw16_p(REG7,REG1,DEFAULT);      // REG7 for sblimit
    VoC_lw16i_short(REG5,32,IN_PARALLEL);   // REG5 for bound  bound = 32;
    VoC_lw16_d(REG1,STRUCT_MAD_HEADER_MODE_ADDR);   // REG1 for header->mode
    VoC_bne16_rd(mad_layer_II_105,REG1,CONST_2_ADDR)    // if (header->mode == MAD_MODE_JOINT_STEREO) {

    VoC_lw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);  // REG4 for header->flag
    VoC_lw16_d(REG2,STRUCT_MAD_HEADER_MODE_EXTENSION_ADDR);
    VoC_or16_ri(REG4,0X0100);       // header->flags |= MAD_FLAG_I_STEREO;

    VoC_shr16_ri(REG2,-2,DEFAULT);
    VoC_add16_rd(REG5,REG2,CONST_4_ADDR);       // bound = 4 + header->mode_extension * 4;
    VoC_sw16_d(REG4,STRUCT_MAD_HEADER_FLAGS_ADDR);  // REG3 for header->flag
mad_layer_II_105:

    VoC_bngt16_rr(mad_layer_II_106, REG5,REG7)      // if (bound > sblimit)
    VoC_movreg16(REG5,REG7,DEFAULT);            // bound = sblimit;
mad_layer_II_106:
    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16i_set_inc(REG2,layer_II_allocation_addr,2);
    VoC_loop_i_short(32,DEFAULT);
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_endloop0
    // start = stream->ptr;
    // REG6 for nch, REG5 for bound, REG7 for sblimit
    /* decode bit allocations */
    VoC_lw16i_set_inc(REG2,layer_II_allocation_addr,1);
    VoC_lw16i(REG4,TABLE_bitalloc_table_ADDR);
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr+32,1);
    VoC_push16(REG7,DEFAULT);
    VoC_sw16_d(REG4,TABLE_bitalloc_table_ADDR_ADDR);
    VoC_loop_r_short(REG5,DEFAULT)      // for (sb = 0; sb < bound; ++sb) {
    VoC_startloop0
    //for (ch = 0; ch < nch; ++ch)
    //  allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
    //nbal = bitalloc_table[offsets[sb]].nbal;
    VoC_lw16inc_p(REG7,REG3,DEFAULT);   // REG3 for offsets[sb]
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_add16_rd(REG7,REG7,TABLE_bitalloc_table_ADDR_ADDR);
    VoC_lbinc_p(REG0);
    VoC_lw16_p(REG4,REG7,DEFAULT);      // REG4 for nbal
    VoC_rbinc_r(REG4,REG4,DEFAULT);         // allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
    VoC_NOP();
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_be16_rd(mad_layer_II_107,REG6,CONST_1_ADDR)
    VoC_lw16_p(REG4,REG7,DEFAULT);      // RL6_HI for nbal
    VoC_rbinc_r(REG4,REG4,DEFAULT);         // allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
    VoC_NOP();
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
mad_layer_II_107:
    VoC_NOP();
    VoC_endloop0
    /*  for (sb = bound; sb < sblimit; ++sb) {
        nbal = bitalloc_table[offsets[sb]].nbal;

        allocation[0][sb] =allocation[1][sb] = mad_bit_read(&stream->ptr, nbal);
      }*/
    VoC_lw16_sd(REG4,0,DEFAULT);//,"SBLIMIT"
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_bngtz16_r(mad_layer_II_108,REG4)
    VoC_add16_rd(REG1,REG2,CONST_32_ADDR);
    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG3,DEFAULT);   // REG3 for offsets[sb]
    VoC_shr16_ri(REG7,-1,DEFAULT);
    VoC_add16_rd(REG7,REG7,TABLE_bitalloc_table_ADDR_ADDR);
    VoC_lbinc_p(REG0);
    VoC_lw16_p(REG4,REG7,DEFAULT);      // RL6_HI for nbal
    VoC_rbinc_r(REG4,REG4,DEFAULT);         // allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
    VoC_NOP();
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0
mad_layer_II_108:
    /* decode scalefactor selection info */

    // REG0 for ptr, REG6 for nch, REG5 for bound, REG7 for sblimit
    /*  for (sb = 0; sb < sblimit; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
          if (allocation[ch][sb])
        scfsi[ch][sb] = mad_bit_read(&stream->ptr, 2);
        }
      } */
    VoC_pop16(REG7,DEFAULT);        // REG7 for sblimit
    VoC_lw16i_short(REG3,31,DEFAULT);
    VoC_lw16i(REG2,93);
    VoC_be16_rd(mad_layer_II_110,REG6,CONST_1_ADDR)
    VoC_lw16i(REG3,63);
    VoC_lw16i(REG2,189);
mad_layer_II_110:
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,32);
    VoC_sw32_d(REG23,layer_II_BACK_JUMP1_addr);
    VoC_lw16i_set_inc(REG2,layer_II_scfsi_addr,32);
    VoC_loop_r(1,REG7)
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_bez16_r(mad_layer_II_109,REG4)
    VoC_rbinc_i(REG4,2,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG4,REG2,DEFAULT);
mad_layer_II_109:
    VoC_add16_rd(REG2,REG2,CONST_32_ADDR);
    VoC_lbinc_p(REG0);
    VoC_endloop0
    VoC_sub16_rr(REG1,REG1,REG3,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG3,IN_PARALLEL);
    VoC_endloop1;
    /* check CRC word */
    VoC_lw16_d(REG1,STRUCT_MAD_HEADER_FLAGS_ADDR);
    VoC_and16_ri(REG1,0X0010);
    VoC_bez16_r(mad_layer_II_111,REG1)      //if (header->flags & MAD_FLAG_PROTECTION) {
    VoC_push32(REG67,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_push32(RL6,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);
    VoC_push16(BITCACHE,DEFAULT);


    /*header->crc_check =
      mad_bit_crc(start, mad_bit_length(&start, &stream->ptr),
          header->crc_check);*/
    VoC_jal(CII_mad_bit_crc);
    /*  if (header->crc_check != header->crc_target && !(frame->options & MAD_OPTION_IGNORECRC)) {
      stream->error = MAD_ERROR_BADCRC;
      return -1;
      */
    VoC_lw32_d(REG23,STRUCT_MAD_HEADER_CRC_CHECK_ADDR);
    VoC_pop16(BITCACHE,DEFAULT);
    VoC_pop32(RL6,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);
    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_be16_rr(mad_layer_II_111,REG2,REG3)
    VoC_lw16_d(REG2,STRUCT_MAD_FRAME_OPTIONS_ADDR);
    VoC_and16_ri(REG2,1);
    VoC_bez16_r(mad_layer_II_111,REG2)
    VoC_lw16i(REG4,0X0201);
    VoC_lw16i_short(REG1,-1,DEFAULT);
    VoC_sw16_d(REG4,STRUCT_MAD_STREAM_MAD_ERROR_ADDR);
    VoC_jump(mad_layer_II_end);
mad_layer_II_111:
    /* decode scalefactors */
    /*  for (sb = 0; sb < sblimit; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
          if (allocation[ch][sb]) {
        scalefactor[ch][sb][0] = mad_bit_read(&stream->ptr, 6);

        switch (scfsi[ch][sb]) {
        case 2:
          scalefactor[ch][sb][2] =
          scalefactor[ch][sb][1] =
          scalefactor[ch][sb][0];
          break;

        case 0:
          scalefactor[ch][sb][1] = mad_bit_read(&stream->ptr, 6);

        case 1:
        case 3:
          scalefactor[ch][sb][2] = mad_bit_read(&stream->ptr, 6);
        }

        if (scfsi[ch][sb] & 1)
          scalefactor[ch][sb][1] = scalefactor[ch][sb][scfsi[ch][sb] - 1];

        if (scalefactor[ch][sb][0] == 63 ||
            scalefactor[ch][sb][1] == 63 ||
            scalefactor[ch][sb][2] == 63) {
          stream->error = MAD_ERROR_BADSCALEFACTOR;
          return -1;
        }
          }
        }
      }
      */
    // REG0 for ptr, REG6 for nch, REG5 for bound, REG7 for sblimit REG3 for back jump number
    VoC_lw16i_set_inc(REG3,layer_II_scalefactor_addr,1);
    VoC_lw16i_set_inc(REG2,TABLE_sf_table_ADDR,1);
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,32);
    VoC_sw32_d(REG23,TABLE_sf_table_ADDR_ADDR);
    VoC_movreg16(RL7_HI,REG7,DEFAULT);
    VoC_movreg16(RL7_LO,REG5,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,layer_II_scfsi_addr,32);
    VoC_sw32_d(RL7,layer_II_bound_addr);
    VoC_movreg16(RL7_LO,REG6,DEFAULT);
    VoC_loop_r(1,REG7)          // for (sb = 0; sb < sblimit; ++sb)
//      VoC_movreg16(ACC1_HI,REG3,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT)      // for (ch = 0; ch < nch; ++ch)
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_bez16_r(mad_layer_II_112,REG4)      // if (allocation[ch][sb])
    VoC_rbinc_i(REG4,6,DEFAULT);        // scalefactor[ch][sb][0] = mad_bit_read(&stream->ptr, 6);
    VoC_movreg16(ACC0_HI,REG4,DEFAULT); // ACC0_HI for scalefactor[ch][sb][0]
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_lw16inc_p(REG7,REG2,DEFAULT);       // switch (scfsi[ch][sb])
    VoC_bne16_rd(mad_layer_II_113,REG7,CONST_2_ADDR)    //  case 2:
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_movreg16(ACC0_LO,REG4,IN_PARALLEL);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_jump(mad_layer_II_114);
mad_layer_II_113:
    VoC_bnez16_r(mad_layer_II_115,REG7) //  case 0:
    VoC_rbinc_i(REG4,6,DEFAULT);
    VoC_movreg16(ACC0_LO,REG4,DEFAULT); // ACC0_LO for scalefactor[ch][sb][1]
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
mad_layer_II_115:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,6,DEFAULT);
    VoC_bne16_rd(mad_layer_II_116,REG7,CONST_1_ADDR)    //  case 1:
    VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
    VoC_movreg16(ACC0_LO,ACC0_HI,IN_PARALLEL);
mad_layer_II_116:
    VoC_bne16_rd(mad_layer_II_117,REG7,CONST_3_ADDR)    //  case 3:
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_movreg16(ACC0_LO,ACC0_HI,IN_PARALLEL);
mad_layer_II_117:
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
mad_layer_II_114:
    VoC_lw16i(REG6,63);
    VoC_be16_rr(mad_layer_II_118,REG4,REG6)
//          VoC_be16_rd(mad_layer_II_118,REG4,CONST_63_ADDR)
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_be16_rr(mad_layer_II_118,REG6,REG4)
    VoC_be16_rr(mad_layer_II_118,REG7,REG4)
//          VoC_be16_rd(mad_layer_II_118,REG6,CONST_63_ADDR)
//          VoC_be16_rd(mad_layer_II_118,REG7,CONST_63_ADDR)
    VoC_jump(mad_layer_II_212);
mad_layer_II_118:
    VoC_lw16i(REG4,0X0221);
    VoC_lw16i_short(REG1,-1,DEFAULT);
    VoC_sw16_d(REG4,STRUCT_MAD_STREAM_MAD_ERROR_ADDR);

    VoC_jump(mad_layer_II_end);
mad_layer_II_112:
    VoC_add16_rd(REG3,REG3,CONST_3_ADDR);
    VoC_inc_p(REG2,DEFAULT);
mad_layer_II_212:
    VoC_NOP();
    VoC_lbinc_p(REG0);
    VoC_lw16i(REG6,93);
//      VoC_add16_rd(REG3,REG3,CONST_93_ADDR);
    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_movreg32(REG67,RL7,DEFAULT);


    VoC_endloop0
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP_addr);
    VoC_sub16_rd(REG2,REG2,layer_II_BACK_JUMP_addr);
    VoC_sub16_rd(REG3,REG3,layer_II_BACK_JUMP1_addr);
    VoC_endloop1

    VoC_lw16i_set_inc(REG2,STATIC_MAD_SBSAMPLE_ADDR,2);
    VoC_lw16i_set_inc(REG1,STATIC_MAD_SBSAMPLE_ADDR+36*32*2,2);
    VoC_lw32z(ACC1,DEFAULT);
    VoC_loop_i(1,36)
    VoC_loop_i_short(32,DEFAULT)
    VoC_startloop0
    VoC_sw32inc_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1
    /* decode samples */
    /*  for (gr = 0; gr < 12; ++gr) {
        for (sb = 0; sb < bound; ++sb) {
          for (ch = 0; ch < nch; ++ch) {
        if ((index = allocation[ch][sb])) {
          index = offset_table[bitalloc_table[offsets[sb]].offset][index - 1];

          II_samples(&stream->ptr, &qc_table[index], samples);

          for (s = 0; s < 3; ++s) {
            frame->sbsample[ch][3 * gr + s][sb] =
               //   Q28to14( L_mult(     samples[s]      ,       round(sf_table[scalefactor[ch][sb][gr / 4]] <<1)  ));
               Q28to14( L_mult(     samples[s]      ,      sf_table[scalefactor[ch][sb][gr / 4]]  ));
          }
        }
        else {
          for (s = 0; s < 3; ++s)
            frame->sbsample[ch][3 * gr + s][sb] = 0;
        }
          }
        }
    */

    VoC_lw16i_short(FORMATX,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    // RL7 for nch and sblimit
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG5,IN_PARALLEL);     // ACC0_LO for bound
mad_layer_II_121:
    VoC_blt16_rd(no_mad_layer_II_120,REG7,CONST_12_ADDR)        //for (gr = 0; gr < 12; ++gr)
    VoC_jump(mad_layer_II_120);
no_mad_layer_II_120:
    VoC_push16(REG7,DEFAULT);
    VoC_lw16i_short(REG5,0,DEFAULT);    // REG5 for sb
    VoC_lw16_sd(REG3,1,DEFAULT);//,"offsets"
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,32);
mad_layer_II_123:
    VoC_blt16_rd(no_mad_layer_II_122,REG5,layer_II_bound_addr)      //for (sb = 0; sb < bound; ++sb)
    VoC_jump(mad_layer_II_122);
no_mad_layer_II_122:

    VoC_lw16inc_p(REG4,REG3,DEFAULT);
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_add16_rd(REG4,REG4,TABLE_bitalloc_table_ADDR_ADDR);
    VoC_push16(REG5,DEFAULT);
    VoC_lw32_p(REG45,REG4,DEFAULT);     // REG4 for bitalloc_table[offsets[sb]].offset
    VoC_mult16_rd(REG4,REG5,CONST_15_ADDR);
    VoC_lw16_sd(REG7,1,DEFAULT);
    VoC_NOP();
    VoC_push16(REG4,DEFAULT);
    VoC_lw16_sd(REG5,1,DEFAULT);//SB
    VoC_lw16i(REG4,192);
    VoC_mult16_rr(REG4,REG7,REG4,DEFAULT);
//          VoC_mult16_rd(REG4,REG7,CONST_192_ADDR);
    VoC_shr16_ri(REG7,2,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);   //add 2*sb for the words

    VoC_mult16_rd(REG5,REG5,CONST_3_ADDR);
    VoC_NOP();
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_add16_rd(REG7,REG7,layer_II_scalefactor_ADDR_ADDR);
    VoC_lw16i(REG2,STATIC_MAD_SBSAMPLE_ADDR);
    VoC_lw16_p(REG5,REG7,DEFAULT);
    VoC_add16_rd(REG5,REG5,TABLE_sf_table_ADDR_ADDR);
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);   // REG4 for frame->sbsample[ch][3 * gr + s][sb] addr
    VoC_lw16_p(REG5,REG5,DEFAULT);      // REG5 for sf_table[scalefactor[ch][sb][gr / 4]]
    VoC_sw16_d(REG7,layer_II_TEMP_ADDR);
    VoC_sw32_d(REG45,layer_II_SBSAMPLE_SFTABLE_ADDR);
    VoC_movreg16(REG6,RL7_LO,DEFAULT);
    VoC_loop_r(1,REG6)      //  for (ch = 0; ch < nch; ++ch)
    VoC_lw16_sd(REG4,0,DEFAULT);//,"bitalloc_table"
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_lw16i(REG7,TABLE_offset_table_ADDR);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_lbinc_p(REG0);
    VoC_lw16_p(REG4,REG4,DEFAULT);      // REG4 for index
    VoC_bez16_r(mad_layer_II_124,REG5)
    VoC_lw16i_set_inc(REG2,TABLE_qc_table_ADDR,1);
    VoC_mult16_rd(REG4,REG4,CONST_6_ADDR);
    VoC_push16(REG3,DEFAULT);//,"ADDRESS"
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);   // REG2 for addr of &qc_table[index]
    // USED REGISTER:   REG0, REG2,REG3,REG5,REG6,REG7, ACC1_LO

    VoC_jal(CII_II_samples);
    VoC_lw16d_set_inc(REG2,layer_II_SBSAMPLE_SFTABLE_ADDR,64);
    VoC_lw16i_set_inc(REG3,layer_II_samples_addr,2);
    VoC_lw16_d(REG5,layer_II_SFTABLE_ADDR);
    VoC_lw32_d(ACC1,CONST_0x00000800_ADDR);
    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_add32inc_rp(REG67,ACC1,REG3,DEFAULT);

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0
    VoC_pop16(REG3,DEFAULT);//,"ADDRESS"
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_jump(mad_layer_II_125);
mad_layer_II_124:
    VoC_lw16i(REG7,192);
    VoC_lw16d_set_inc(REG2,layer_II_SBSAMPLE_SFTABLE_ADDR,64);
//              VoC_add16_rd(REG2,REG2,CONST_192_ADDR);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
mad_layer_II_125:

    VoC_lw16i(REG7,2112);
    //           VoC_add16_rd(REG2,REG2,CONST_2112_ADDR);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
//              VoC_lw16_d(REG7,layer_II_TEMP_ADDR);
//              VoC_add16_rd(REG7,REG7,CONST_96_ADDR);
    VoC_lw16i(REG7,96);
    VoC_add16_rd(REG7,REG7,layer_II_TEMP_ADDR);
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_add16_rd(REG7,REG7,TABLE_sf_table_ADDR_ADDR);
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_sw16_d(REG2,layer_II_SBSAMPLE_SFTABLE_ADDR);
    VoC_sw16_d(REG7,layer_II_SFTABLE_ADDR);
    VoC_endloop1
    VoC_sub16_rd(REG1,REG1,layer_II_BACK_JUMP_addr);
    VoC_pop16(REG4,DEFAULT);//,"bitalloc_table"
    VoC_pop16(REG5,DEFAULT);//,"SB"
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_jump(mad_layer_II_123);
mad_layer_II_122:
    VoC_lw16i_set_inc(REG1,layer_II_allocation_addr,1);
    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);
mad_layer_II_129:
    VoC_blt16_rd(no_mad_layer_II_126,REG5,layer_II_sblimit_addr)
    VoC_jump(mad_layer_II_126);
no_mad_layer_II_126:
    VoC_lw16inc_p(REG4,REG3,DEFAULT);
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_add16_rd(REG4,REG4,TABLE_bitalloc_table_ADDR_ADDR);
    VoC_push16(REG5,DEFAULT);//,"SB"
    VoC_lw32_p(REG45,REG4,DEFAULT);     // REG4 for bitalloc_table[offsets[sb]].offset
    VoC_mult16_rd(REG4,REG5,CONST_15_ADDR);
    VoC_lw16_sd(REG7,1,DEFAULT);//,"GR"
    VoC_NOP();
    VoC_push16(REG4,DEFAULT);//,"bitalloc_table"
    VoC_lw16i(REG4,192);
    VoC_mult16_rr(REG4,REG7,REG4,DEFAULT);
//          VoC_mult16_rd(REG4,REG7,CONST_192_ADDR);
    VoC_lw16_sd(REG5,1,DEFAULT);//,"SB"
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);   //add 2*sb for the words

    VoC_mult16_rd(REG5,REG5,CONST_3_ADDR);
    VoC_shr16_ri(REG7,2,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_add16_rd(REG7,REG7,layer_II_scalefactor_ADDR_ADDR);
    VoC_lw16i(REG2,STATIC_MAD_SBSAMPLE_ADDR);
    VoC_lw16_p(REG5,REG7,DEFAULT);
    VoC_add16_rd(REG5,REG5,TABLE_sf_table_ADDR_ADDR);
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);   // REG4 for frame->sbsample[ch][3 * gr + s][sb] addr
    VoC_lw16_p(REG5,REG5,DEFAULT);      // REG5 for sf_table[scalefactor[ch][sb][gr / 4]]
    VoC_sw16_d(REG7,layer_II_TEMP_ADDR);
    VoC_sw32_d(REG45,layer_II_SBSAMPLE_SFTABLE_ADDR);
    VoC_lw16_sd(REG4,0,DEFAULT);//,"bitalloc_table"
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_lw16i(REG7,TABLE_offset_table_ADDR);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_lbinc_p(REG0);
    VoC_lw16_p(REG4,REG4,DEFAULT);      // REG4 for index
    VoC_bez16_r(mad_layer_II_128,REG5)
    VoC_lw16i_set_inc(REG2,TABLE_qc_table_ADDR,1);
    VoC_mult16_rd(REG4,REG4,CONST_6_ADDR);
    VoC_push16(REG3,DEFAULT);//,"ADDRESS"
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);   // REG2 for addr of &qc_table[index]
    // USED REGISTER:   REG0, REG2,REG3,REG5,REG6,REG7, ACC1_LO

    VoC_jal(CII_II_samples);
    VoC_movreg16(REG6,RL7_LO,DEFAULT);
    VoC_loop_r(1, REG6)
    VoC_lw16d_set_inc(REG2,layer_II_SBSAMPLE_SFTABLE_ADDR,64);
    VoC_lw16i_set_inc(REG3,layer_II_samples_addr,2);
    VoC_lw16_d(REG5,layer_II_SFTABLE_ADDR);

    VoC_lw32_d(ACC1,CONST_0x00000800_ADDR);
    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_add32inc_rp(REG67,ACC1,REG3,DEFAULT);


    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_shru16_ri(REG6,12,DEFAULT);
    VoC_shru32_ri(ACC0,-4,DEFAULT);
    VoC_macX_rr(REG5,REG6,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG7,2112);
    //           VoC_add16_rd(REG2,REG2,CONST_2112_ADDR);
    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

//              VoC_lw16_d(REG7,layer_II_TEMP_ADDR);
//              VoC_add16_rd(REG7,REG7,CONST_96_ADDR);
    VoC_lw16i(REG7,96);
    VoC_add16_rd(REG7,REG7,layer_II_TEMP_ADDR);
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_add16_rd(REG7,REG7,TABLE_sf_table_ADDR_ADDR);
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);
    VoC_sw16_d(REG2,layer_II_SBSAMPLE_SFTABLE_ADDR);
    VoC_sw16_d(REG7,layer_II_SFTABLE_ADDR);
    VoC_endloop1
    VoC_pop16(REG3,DEFAULT);//,"ADDRESS"
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
mad_layer_II_128:
    VoC_pop16(REG4,DEFAULT);//,"bitalloc_table"
    VoC_pop16(REG5,DEFAULT);
    VoC_add16_rd(REG5,REG5,CONST_1_ADDR);
    VoC_jump(mad_layer_II_129);
mad_layer_II_126:
    VoC_pop16(REG7,DEFAULT);//,"GR"
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_jump(mad_layer_II_121);
mad_layer_II_120:
    VoC_lw16i_short(REG1,0,DEFAULT);
mad_layer_II_end:
    VoC_pop16(REG3,DEFAULT);//,"offsets"
    VoC_pop16(RA,DEFAULT);
    VoC_sw32_d(RL6,STRUCT_BITPTR_PTR_CACHE_ADDR);
    VoC_lw16i_short(WRAP0,16,DEFAULT);
    /*
        {
            int i ;
           for (i = 0;i < 2; i++)
            {

            if (fwrite ( &RAM_Y[(STATIC_MAD_SBSAMPLE_ADDR +2304*i - RAM_Y_BEGIN_ADDR)/2], sizeof (Word32), 32*36, file_SB) != 32*36) {
                fprintf(stderr, "\nerror writing sbsample  file: \n");
                return -1;
            }
            fflush(file_SB);
            }
        }

    */

    VoC_return;
}


