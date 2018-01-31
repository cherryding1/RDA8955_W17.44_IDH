
#include "regdef.h"

//=============================================================================
// mult32_r1
//-----------------------------------------------------------------------------
// @param INT32  x 
// @param INT32  y
// @param INT32  r

// a0 : x
// a1 : y
// a2 : r

.globl	mult32_r1
.ent	mult32_r1
mult32_r1:
	mult a0,a1				//{t1,t0} = a0 * a1
	li	 t7,32
	subu t7,t7,a2			//t7 = 32 - r	
	mflo t0
	mfhi t1
	
	srl t0,t0,a2			//t0 = t0 >> r
	sll t1,t1,t7			//t0 = t0 << (32 - r)
	
	or  v0,t0,t1
	jr	ra

.end	mult32_r1 

//=============================================================================
// div_320_320
//-----------------------------------------------------------------------------
// @param INT32  divd 
// @param INT32  divs

// a0 : divd
// a1 : divs

.globl	div_320_320
.ent	div_320_320
div_320_320:
	divu v0,a0,a1		//v0 = divd/divs
	jr	ra

.end	div_320_320 