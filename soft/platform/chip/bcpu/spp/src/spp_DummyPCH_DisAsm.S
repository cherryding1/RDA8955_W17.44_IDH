
#include "regdef.h"

#.section .sramtext, "awx", @progbits

//=============================================================================
// mean = spp_DummyPCH_DisAsm(softBit, pa, distance, cnt)
//-----------------------------------------------------------------------------
// @param INT8  *softBit 
// @param INT8  *pa
// @param INT32 *distance
// @param INT32 cnt

// a0 : softBit
// a1 : pa
// a2 : distance
// a3 : cnt

//return mean

.globl	spp_DummyPCH_DisAsm
.ent	spp_DummyPCH_DisAsm
spp_DummyPCH_DisAsm:
  li t3, 0
  li t4, 0
  
  //loop
  //t3 = mean
  //t4 = distance
__pch_dummy_loop:
  
  lb  	t1, 0(a0)			//t1 = softBit[i]
  lb  	t5, 0(a1)			//t5 = *pa
  
  sra 	t2, t1, 7			//t2 = softBit[i] >>7 = sign, 0x00000000 or 0xffffffff
  and 	t1, t1, 0x7f	//t1 = softBit[i] & 0x7f = value
  addu 	t3, t3, t1		//t3 = t3 + value
  
  //distance += (sign ^ value)*(*pa++);
  xor 	t2, t2, t1		//t2 = sign ^ value
  
  mult 	t2, t5
  addiu	a0,	a0,	1		//a0++
  addiu	a1,	a1,	1		//a1++
  mflo	t6					//t6 = (sign ^ value) * (*pa)
  add 	t4, t6, t4  //distance = distance + (sign ^ value) * (*pa)
  sub	  a3,	a3,	1		//cnt--
  
	bne	a3,	zero,	__pch_dummy_loop
	
	move  v0, t3  		//return mean
  sw  	t4, 0(a2)		//store distance
  jr    ra

.end    spp_DummyPCH_DisAsm 


