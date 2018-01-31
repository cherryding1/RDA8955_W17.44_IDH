//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/// @file spp_fcchDetect.S
///
/// Header for private SPP functions and definitions. 
///
/// @author Laurent Bastide
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "regdef.h"

//------------------------------------------------------------------------------
// Defines 
//------------------------------------------------------------------------------

#define FCCH_THRES_HIGH 16
#define FCCH_THRES_LOW  6
#define MAX_SEARCH_WIN  70
#define FCCH_WIN_SIZE  142


//=============================================================================
// spp_FcchDetectAsm
//-----------------------------------------------------------------------------
// FCCH detection optimized code
//
// @param Ptr_old UINT32*. ?? See spp_fcchDetect.S 
// @param Ptr_new UINT32*. ?? See spp_fcchDetect.S 
// @param Params SPP_FCCH_PARAM_T*. ?? See spp_fcchDetect.S 
// @param Res SPP_FCCH_RESULT_T*. ?? See spp_fcchDetect.S 
//
// @return BOOL.
//
// @author Laurent Bastide 
//
//=============================================================================


// t0 : global->Acc_Energy
// t1 : global->Acc_Corr_I
// t2 : global->Acc_Corr_Q
// t3 : AbsI
// t4 : AbsQ
// t5 : Abs_Corr
// t6 : 
// t7 : loop counter
// t8 : global->counter_high
// t9 : global->counter_low
// a0 : old ptr
// a1 : new ptr
// a2 : global ptr
// a3 : sp_FchResult ptr     ///i ptr
// gp : tempo reg

// load global values
.globl    spp_FcchDetectAsm
.ent    spp_FcchDetectAsm
spp_FcchDetectAsm:
lw        t0,0(a2)
lw        t1,4(a2)
lw        t2,8(a2)
lh        t6,24(a2) // load samples_left
lbu        t8,29(a2)
lbu        t9,30(a2)

// init loop counter
move    t7,zero

// if (global->FCCH_detected)
lbu        gp,28(a2) // FCCH_detected ?
beq        gp,zero,__B_loop_start
__A_loop_start: //-----------------------------------
// update Energy
lw        gp,0(a1)
addu    t0,t0,gp
lw        gp,0(a0)
subu    t0,t0,gp
// update Corr_I
lw        gp,(3*4*FCCH_WIN_SIZE)(a1)
addu    t1,t1,gp
lw        gp,(3*4*FCCH_WIN_SIZE)(a0)
subu    t1,t1,gp
// update Corr_I
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a1)
addu    t2,t2,gp
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a0)
subu    t2,t2,gp

// AbsI = Abs(Acc_Corr_I)
move    t3,t1
bgez    t3,__A_no_invertI
negu    t3,t3
__A_no_invertI:

// AbsQ = Abs(Acc_Corr_Q)
move    t4,t2
bgez    t4,__A_no_invertQ
negu    t4,t4
__A_no_invertQ:

// AbsI = Max(AbsI,AbsQ)
// AbsQ = Min(AbsI,AbsQ)
slt        gp,t4,t3
bgtz    gp,__A_good_order
move    gp,t4
move    t4,t3
move    t3,gp
__A_good_order:

// Abs_Corr = 3/8 Min
sll        t5,t4,0x1
addu    t5,t5,t4
sra        t5,t5,0x3

// Abs_Corr += Max
addu    t5,t3

// load Max_Abs_Corr
lw        t3,12(a2)
// if (Abs_Corr>(global->Max_Abs_Corr))
slt        gp,t3,t5
beq        gp,zero,__A_no_new_max
sw        t5,12(a2) // Max_Abs_Corr
sw        t1,16(a2) // Corr_order_I
sw        t2,20(a2) // Corr_order_Q
sh        t7,26(a2) // max_index
__A_no_new_max:
addiu    t7,t7,0x1 // loop counter ++
addiu    a0,a0,0x4 // old ptr ++
addiu    a1,a1,0x4 // new ptr ++
sltu    gp,t7,t6
bnez    gp,__A_loop_start

// if (global->max_index > samples left)
lh        t7,26(a2) // load max_index
sltu    gp,t6,t7
beq        gp,zero,__A_no_offset
addiu    t7,t7,(-FCCH_WIN_SIZE)
__A_no_offset:
lw        t1,16(a2) // load Corr_order_I
lw        t2,20(a2) // load Corr_order_Q
move    gp,zero
li        gp, 0x5430    //  const=270833/(4*PI)
div        zero,t1,gp        //    Corr_I / const
mflo    gp
sh        t7,2(a3) // save TOf
addiu    v0,zero,0x1 // return TRUE
div        zero,t2,gp        //    Corr_Q / (Corr_I / const)
mflo    gp
sh        gp,0(a3) // save FOf
jr        ra

__B_loop_start: //-------------------------------------
// update Energy
lw        gp,0(a1)
addu    t0,t0,gp
lw        gp,0(a0)
subu    t0,t0,gp

// update Corr_I
lw        gp,(3*4*FCCH_WIN_SIZE)(a1)
addu    t1,t1,gp
lw        gp,(3*4*FCCH_WIN_SIZE)(a0)
subu    t1,t1,gp

// update Corr_I
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a1)
addu    t2,t2,gp
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a0)
subu    t2,t2,gp

// AbsI = Abs(Acc_Corr_I)
move    t3,t1
bgez    t3,__B_no_invertI
negu    t3,t3
__B_no_invertI:

// AbsQ = Abs(Acc_Corr_Q)
move    t4,t2
bgez    t4,__B_no_invertQ
negu    t4,t4
__B_no_invertQ:

// AbsI = Max(AbsI,AbsQ)
// AbsQ = Min(AbsI,AbsQ)
slt        gp,t4,t3
bgtz    gp,__B_good_order
move    gp,t4
move    t4,t3
move    t3,gp
__B_good_order:

// Abs_Corr = 3/8 Min
sll        t5,t4,0x1
addu    t5,t5,t4
sra        t5,t5,0x3

// Abs_Corr += Max
addu    t5,t3

// if (Abs_Corr > 0.6*Acc_Energy)
// (6/8)   = 0.7500
// (11/16) = 0.6875
// (5/8)   = 0.6250
// (9/16)  = 0.5625
li        gp,6
mult    gp,t0
sll        t4,t5,0x3
mflo    gp
sltu    gp,gp,t4
beq        gp,zero,__B_no_detect

// then
addiu    t8,t8,0x1 // cnt_high++
// load Max_Abs_Corr
lw        t3,12(a2)
// if (Abs_Corr>(global->Max_Abs_Corr))
sltu    gp,t3,t5
beq        gp,zero,__B_no_new_max
sw        t5,12(a2) // Max_Abs_Corr
sw        t1,16(a2) // Corr_order_I
sw        t2,20(a2) // Corr_order_Q
sh        t7,26(a2) // max_index
__B_no_new_max:
//if cnt_high > const1
sltiu    gp,t8,FCCH_THRES_HIGH // cnt_high>15 == not(cnt_high<16)
bnez    gp,__B_thres_not_passed
//if Acc_Energy>threshold_energy
sra        gp,t0,17
beq        gp,zero,__B_no_FCCH_Detected
addiu    gp,zero,1
sb        gp,28(a2) // set FCCH_detected
j        __FCCH_detected// break
__B_no_FCCH_Detected:
move    t8,zero
sw        zero,12(a2) // Max_Abs_Corr
sh        zero,26(a2) // max_index
__B_thres_not_passed:
j        __B_no_cnt_reset

// else
__B_no_detect:
blez    t8,__B_no_inc_cnt_low
addiu    t9,t9,0x1 // cnt_low++
__B_no_inc_cnt_low:
//if cnt_low > const2
sltiu    gp,t9,FCCH_THRES_LOW // cnt_low>5 == not(cnt_low<6)
bnez    gp,__B_no_cnt_reset
move    t8,zero
move    t9,zero
__B_no_cnt_reset: // Also the end of loop
addiu    t7,t7,0x1 // loop counter ++
addiu    a0,a0,0x4 // old ptr ++
addiu    a1,a1,0x4 // new ptr ++
sltiu    gp,t7,FCCH_WIN_SIZE
bnez    gp,__B_loop_start
// exit the loop B
// if we get there -> we didn't detect FCCH
j        __end_on_FALSE

__FCCH_detected: //-------------------------------------------
addiu    gp,t7,(MAX_SEARCH_WIN-FCCH_WIN_SIZE+1)    // samples_left = i + 70 - 156
sh        zero,24(a2) // reset samples_left
addiu    t6,t7,MAX_SEARCH_WIN    // end = i+70
blez    gp,__C_no_samples_left
lbu        t3,31(a2)    // load lastIT
bnez    t3,__C_lastIT
addiu    t6,zero,(FCCH_WIN_SIZE-1)    // end = 156
sh        gp,24(a2)    // store samples_left
j        __C_no_new_max // usefull if t7 is already at 155
__C_lastIT:
addiu    t6,zero,(FCCH_WIN_SIZE-1)    // end = 156
__C_no_samples_left:
__C_loop_start: //-----------------------------------
addiu    t7,t7,0x1 // loop counter ++
addiu    a0,a0,0x4 // old ptr ++
addiu    a1,a1,0x4 // new ptr ++
// update Energy
lw        gp,0(a1)
addu    t0,t0,gp
lw        gp,0(a0)
subu    t0,t0,gp
// update Corr_I
lw        gp,(3*4*FCCH_WIN_SIZE)(a1)
addu    t1,t1,gp
lw        gp,(3*4*FCCH_WIN_SIZE)(a0)
subu    t1,t1,gp
// update Corr_I
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a1)
addu    t2,t2,gp
lw        gp,(2*3*4*FCCH_WIN_SIZE)(a0)
subu    t2,t2,gp

// AbsI = Abs(Acc_Corr_I)
move    t3,t1
bgez    t3,__C_no_invertI
negu    t3,t3
__C_no_invertI:

// AbsQ = Abs(Acc_Corr_Q)
move    t4,t2
bgez    t4,__C_no_invertQ
negu    t4,t4
__C_no_invertQ:

// AbsI = Max(AbsI,AbsQ)
// AbsQ = Min(AbsI,AbsQ)
slt        gp,t4,t3
bgtz    gp,__C_good_order
move    gp,t4
move    t4,t3
move    t3,gp
__C_good_order:

// Abs_Corr = 3/8 Min
sll        t5,t4,0x1
addu    t5,t5,t4
sra        t5,t5,0x3

// Abs_Corr += Max
addu    t5,t3

// load Max_Abs_Corr
lw        t3,12(a2)
// if (Abs_Corr>(global->Max_Abs_Corr))
slt        gp,t3,t5
beq        gp,zero,__C_no_new_max
sw        t5,12(a2) // Max_Abs_Corr
sw        t1,16(a2) // Corr_order_I
sw        t2,20(a2) // Corr_order_Q
sh        t7,26(a2) // max_index
__C_no_new_max:
sltu    gp,t7,t6
bnez    gp,__C_loop_start

// if(samples_left==0)
lh        gp,24(a2)    // load samples_left
bnez    gp,__end_on_FALSE
// if (global->max_index > loop cnt)
lh        t6,26(a2) // load max_index
sltu    gp,t7,t6
beq        gp,zero,__C_no_offset
addiu    t6,t6,(-FCCH_WIN_SIZE)
__C_no_offset:
lw        t1,16(a2) // load Corr_order_I
lw        t2,20(a2) // load Corr_order_Q
move    gp,zero
li        gp, 0x5430    //  const=270833/(4*PI)
div        zero,t1,gp        //    Corr_I / const
mflo    gp
sh        t6,2(a3) // save TOf
addiu    v0,zero,0x1 // return TRUE
div        zero,t2,gp        //    Corr_Q / (Corr_I / const)
mflo    gp
sh        gp,0(a3) // save FOf
jr        ra

__end_on_FALSE: //-------------------------------------------
sw        t0,0(a2)
sw        t1,4(a2)
sw        t2,8(a2)
sb        t8,29(a2)
sb        t9,30(a2)
move    v0,zero // return FALSE
jr        ra

.end    spp_FcchDetectAsm 
