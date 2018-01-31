
#include "regdef.h"


//=============================================================================
// extern void mmc_atvIntraFrameAveAsm(UINT32* imgBuf, UINT32 srcwidth, UINT32 cutwidth, UINT32 cutheight)
//-----------------------------------------------------------------------------

// a0 : imgBuf
// a1 : srcwidth
// a2 : cutwidth
// a3 : cutheight

.globl	mmc_atvIntraFrameAveAsm
.ent	mmc_atvIntraFrameAveAsm
.set noreorder
mmc_atvIntraFrameAveAsm:

  li 		t3, 0xfe00
  move  t2, t3
  sll 	t3, t3, 16
	or		t3,	t3,	t2		//t3 = 0xfe00fe00
  
  li 		t4, 0xff
  move  t2, t4
  sll 	t4, t4, 16
	or		t4,	t4,	t2		//t4 = 0x00ff00ff
	
	srl   a1, a1, 1			//a1 = srcwidth>>1
	srl   a2, a2, 1			//a2 = cutwidth>>1
	srl 	a3, a3, 1			//a3 = cutheight>>1
	
	move  t0, a0				//t0 = (UINT32*)imgBuf; 
	sll   t1, a1, 2			//t1 = 4* (srcwidth>>1)
	addu  t1, t0, t1		//t1 = imgBuf + srcwidth>>1
	
__atvIntraFrame_Ave_loop0:	
	move  t2, a2				//t2 = cutwidth>>1,inner loop counter
	
__atvIntraFrame_Ave_loop1:	
	lw    t8, 0(t0)			//t8 = *buf1
	lw    t9, 0(t1)			//t9 = *buf2
	
	and 	t6, t8, t3		//t6 = *buf1 & 0xfe00fe00
	and 	t7, t9, t3		//t7 = *buf2 & 0xfe00fe00
	
	srl   t6, t6, 1			//t6 = (*buf1 & 0xfe00fe00)>>1
	srl   t7, t7, 1			//t7 = (*buf2 & 0xfe00fe00)>>1
	
	addu  t5, t6, t7		//t5 = t6 + t7
	
	and 	t6, t8, t4		//t6 = *buf1 & 0x00ff00ff
	and 	t7, t9, t4		//t7 = *buf2 & 0x00ff00ff
	
	or    t6, t6, t5
	or    t7, t7, t5
	
	sw    t6, 0(t0)			//*buf1 = t6
	sw    t7, 0(t1)			//*buf2 = t7
	
	subu	t2, t2, 1
	addu  t0, t0, 4
	
	bne	t2,	zero,	__atvIntraFrame_Ave_loop1
	addu  t1, t1, 4
									
	sll   t5, a1, 1			//t5 = srcwidth 
	subu	t5, t5, a2		//t5 = srcwidth - (cutwidth >> 1)
	
	sll 	t5, t5, 2			//4*(srcwidth - (cutwidth >> 1))
	addu  t0, t0, t5		//buf1 = buf1 + srcwidth - (cutwidth >> 1)
	subu	a3, a3, 1
	
	bne	a3,	zero,	__atvIntraFrame_Ave_loop0
	addu  t1, t1, t5		//buf2 = buf2 + srcwidth - (cutwidth >> 1)
	
	jr    ra
	nop
	
.set reorder
.end    mmc_atvIntraFrameAveAsm 	
  
