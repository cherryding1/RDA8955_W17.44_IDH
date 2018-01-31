
//=============================================================================
// FCH filter function
//=============================================================================
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "regdef.h"

//------------------------------------------------------------------------------
// Defines 
//------------------------------------------------------------------------------

#define BB_FCCH_LENGTH 70

//=============================================================================
// spal_XcorFchFilterAsm
//-----------------------------------------------------------------------------
// @param UINT32*  Ptr 
// @param UINT32.  Cnt_flag
// @param UINT32*. Xn Ptr
// @param UINT32*. Yn Ptr

// a0 : ptr
// a1 : Cnt_flag
// a2 : Xn ptr
// a3 : Yn ptr

// t0 : Xn
// t1 : Yn
// t7 : loop counter
// t8 : 0xffff0000
// t9 : 0xffff

// load global values
.globl	spal_XcorFchFilterAsm
.ent	spal_XcorFchFilterAsm
spal_XcorFchFilterAsm:

lw		t0,0(a2)										//  t0 = Xn
lw		t1,0(a3)										//	t1 = Yn

li		t9, 0xffff									// 	t9 = 0xffff
sll		t8,t9,0x10									//	t8 = 0xffff0000

li		t7, BB_FCCH_LENGTH					// 	t7 = 70 ,loop counter

beq		a1,zero,__B_xcorloop_start			// if Cnt_flag ==zero, goto __B_loop_start

__A_xcorloop_start:
//for (i=BB_FCCH_LENGTH;i>0;i--)

//		1th sample
//		FltIn  = *pt0;
//		Xn = Xn - (((FltIn<<16) + Xn)>>3);
//		Yn = Yn - (((FltIn & 0xffff0000) + Yn)>>3);
//		FltIn = (Yn & 0xffff0000) | (Xn>>16);
//		*pt0 = FltIn;

lw		t2,0(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

addu  t3,t3,t0										//	t3 = FltIn<<16 + Xn
addu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 + Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 + Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 + Yn)>>3

subu  t0,t0,t3										//	Xn = Xn - (FltIn<<16 + Xn)>>3
subu  t1,t1,t2										//	Yn = Yn - (FltIn & 0xffff0000 + Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,0(a0)										//  *FltIn = t2

//		2th sample
//		FltIn  = *(pt0+1);
//		Xn = Xn - (((FltIn<<16) + Xn)>>3);
//		Yn = Yn - (((FltIn & 0xffff0000) + Yn)>>3);
//		FltIn = (Yn & 0xffff0000) | (Xn>>16);
//		*(pt0+1) = FltIn;
lw		t2,4(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

addu  t3,t3,t0										//	t3 = FltIn<<16 + Xn
addu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 + Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 + Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 + Yn)>>3

subu  t0,t0,t3										//	Xn = Xn - (FltIn<<16 + Xn)>>3
subu  t1,t1,t2										//	Yn = Yn - (FltIn & 0xffff0000 + Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,4(a0)										//  *FltIn = t2

addiu	a0,a0,0x8 									//  ADDRESS + = 8
subu	t7,t7,0x1 									//  loop counter --

bne	  t7,zero,__A_xcorloop_start		// if loop cnt!=zero, goto __A_loop_start

//last sample
lw		t2,0(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

addu  t3,t3,t0										//	t3 = FltIn<<16 + Xn
addu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 + Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 + Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 + Yn)>>3

subu  t0,t0,t3										//	Xn = Xn - (FltIn<<16 + Xn)>>3
subu  t1,t1,t2										//	Yn = Yn - (FltIn & 0xffff0000 + Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,0(a0)										//  *FltIn = t2

sw		t0,0(a2)										//  t0 = Xn
sw		t1,0(a3)										//	t1 = Yn
jr		ra



__B_xcorloop_start:
//for (i=BB_FCCH_LENGTH;i>0;i--)
//		FltIn  = *pt0;
//		Xn = Xn + (((FltIn<<16) - Xn)>>3);
//		Yn = Yn + (((FltIn & 0xffff0000) - Yn)>>3);
//		FltIn = (Yn & 0xffff0000) | (Xn>>16);
//		*pt0++ = FltIn;

//1th sample
lw		t2,0(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

subu  t3,t3,t0										//	t3 = FltIn<<16 - Xn
subu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 - Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 - Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 - Yn)>>3

addu  t0,t0,t3										//	Xn = Xn + (FltIn<<16 - Xn)>>3
addu  t1,t1,t2										//	Yn = Yn + (FltIn & 0xffff0000 - Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,0(a0)										//  *FltIn = t2

//2th sample
lw		t2,4(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

subu  t3,t3,t0										//	t3 = FltIn<<16 - Xn
subu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 - Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 - Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 - Yn)>>3

addu  t0,t0,t3										//	Xn = Xn + (FltIn<<16 - Xn)>>3
addu  t1,t1,t2										//	Yn = Yn + (FltIn & 0xffff0000 - Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,4(a0)										//  *FltIn = t2

addiu	a0,a0,0x8 									//  ADDRESS + = 8
subu	t7,t7,0x1 									//  loop counter --

bne 	t7,zero,__B_xcorloop_start	// if loop cnt!=zero, goto __B_loop_start


//last sample
lw		t2,0(a0)										//  t2 = *FltIn

sll		t3,t2,0x10									//	t3 = FltIn<<16
and		t2,t2,t8										//	t2 = FltIn & 0xffff0000

subu  t3,t3,t0										//	t3 = FltIn<<16 - Xn
subu  t2,t2,t1										//  t2 = FltIn & 0xffff0000 - Yn

sra		t3,t3,0x2										//	t3 = (FltIn<<16 - Xn)>>3
sra		t2,t2,0x2  									//  t2 = (FltIn & 0xffff0000 - Yn)>>3

addu  t0,t0,t3										//	Xn = Xn + (FltIn<<16 - Xn)>>3
addu  t1,t1,t2										//	Yn = Yn + (FltIn & 0xffff0000 - Yn)>>3

srl		t2,t0,0x10									//	t2 = Xn>>16
and		t3,t1,t8										//	t3 = Yn & 0xffff0000

or    t2,t2,t3
sw		t2,0(a0)										//  *FltIn = t2

sw		t0,0(a2)										//  t0 = Xn
sw		t1,0(a3)										//	t1 = Yn
jr		ra


.end	spal_XcorFchFilterAsm 
