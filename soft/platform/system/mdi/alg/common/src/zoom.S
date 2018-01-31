#include "regdef.h"

	.text
	.globl VidRec_bilinear_zoom
	.ent VidRec_bilinear_zoom
	.set noreorder	
#void VidRec_bilinear_zoom(char* srcbuf,int src_width,int src_height,int cut_width,int cut_height,char* decbuf,int dec_width,int dec_height)
#function: zoom video image, YUV 4:2:0 to YUV 4:2:0
#2009/00/00 created by liyongjian 
#2009/03/06 modified by liyongian fix the last row and line interpolation mistake.	
VidRec_bilinear_zoom:	
	subu 	sp, 	sp, 	64 	
	sw 	ra, 	60(sp)
	sw 	fp, 	56(sp)
	sw 	s0, 	52(sp)
	sw 	s1, 	48(sp)
	sw 	s2, 	44(sp)
	sw 	s3, 	40(sp)
	sw 	s4, 	36(sp)
	sw 	s5, 	32(sp)
	sw 	s6, 	28(sp)
	sw 	s7, 	24(sp)	
		
	lw	s0,	80(sp)			#s0 = cut_height
	lw	s1,	92(sp)
	lw	s2,	88(sp)			#s2 = dec_width
	sll	t0,	a3,	16
	sll	t1,	s0,	16
	div	t0,	s2
	subu	s0,	a2,	s0
	mflo	t0				#scaleH=(cut_width<<16)/dec_width	
	subu	a3,	a1,	a3
		
	mult	s0,	a1
	addu	a0,	a0,	a3
	mflo	s0
	addu	a0,	a0,	s0
		
	div	t1,	s1
	lw	v0,	96(sp)			#BuffA
	sll	v1,	s2,	1
	addu	v1,	v0,	v1		#BuffB
	mflo	t1				#scaleV=(cut_height<<16)/dec_height
	li	a2,	0x0			#Accumulator
	cache 	2,	0
	nop
	nop
	nop
	
	lw	s0,	84(sp)			#char* decbuf
	li	s3,	-1
	sll	a1,	a1,	1
	addiu	s1,	s1,	-1
loop_height_start:	
	sra	s4,	a2,	16		#s4 = upline
	addiu	t2,	s4,	0x1
	mult	t2,	a1
	slt	t3,	s4,	s3
	mflo	t2
	bne	t3,	zero,	row_interpolate
	addu	t2,	a0,	t2		#delay slot t0=pLine??????
	
	move 	s5,	v0			#s5 = ptempBuff
	move	v0,	v1
	move	v1,	s5
line_interpolate_start:	
	move 	s6,	s5			#s6 = pY
	move	s7,	s5			#s7 = pUV
	lw	s2,	88(sp)
	li	a3,	0x0			#Accumulator2	
	addiu	s2,	s2,	-2
		
loop_width_start:	
	sra	t3,	a3,	16
	sll	t3,	t3,	1
	addu	t4,	t3,	t2
	sll	t3,	t3,	1
	addu	t5,	t3,	t2
		
	andi	t6,	a3,	0xffff
	li	t7,	0x10000
	subu	t7,	t7,	t6
	
	lbu	t8,	0(t4)
	lbu	t9,	2(t4)
	
	mult	t8,	t7
	lbu	t3,	1(t5)
	madd	t9,	t6
	lbu	t4,	5(t5)
	mflo	t8
	sra	t8,	t8,	16
	sb	t8,	0(s6)
	
	mult	t3,	t7
	addiu	s6,	s6,	0x2
	madd	t4,	t6
	lbu	t8,	3(t5)
	mflo	t3
	sra	t3,	t3,	16
	lbu	t9,	7(t5)
	sb	t3,	1(s7)
	
	mult	t8,	t7
	addu	a3,	a3,	t0
	madd	t9,	t6
	addiu	s2,	s2,	-2
	mflo	t8
	sra	t8,	t8,	16
	sb	t8,	3(s7)
	
	bne	s2,	zero,	loop_width_start
	addiu	s7,	s7,	0x4
#for last point U V	
	sra	t3,	a3,	16
	sll	t3,	t3,	1
	addu	t4,	t3,	t2
	sll	t3,	t3,	1
	addu	t5,	t3,	t2
		
	andi	t6,	a3,	0xffff
	li	t7,	0x10000
	subu	t7,	t7,	t6
	
	lbu	t8,	0(t4)
	lbu	t9,	2(t4)	
	mult	t8,	t7
	lbu	t3,	1(t5)		#load first U
	madd	t9,	t6
	lbu	t4,	3(t5)		#load first V
	mflo	t8
	sra	t8,	t8,	16
	sb	t8,	0(s6)	
	sb	t3,	1(s7)
	sb	t4,	3(s7)
	
	lw	s2,	88(sp)
	addiu	s6,	s6,	0x2
	addu	a3,	a3,	t0	
	addiu	s2,	s2,	-2
		
loop_width_start2:		
	sra	t3,	a3,	16
	sll	t3,	t3,	1
	addu	t4,	t3,	t2
		
	andi	t6,	a3,	0xffff
	li	t7,	0x10000
	subu	t7,	t7,	t6
	
	lbu	t8,	0(t4)
	lbu	t9,	2(t4)
	
	mult	t8,	t7
	addu	a3,	a3,	t0
	madd	t9,	t6
	addiu	s2,	s2,	-2
	mflo	t8
	sra	t8,	t8,	16
	sb	t8,	0(s6)	
	
	bne	s2,	zero,	loop_width_start2
	addiu	s6,	s6,	0x2
	sb	t8,	0(s6)					#for the last point Y
	
	beq	s4,	s3,	line_interpolate_end		#if(upLine!=lastLine)
	subu	t2,	t2,	a1
	move	s3,	s4
	j	line_interpolate_start
	move 	s5,	v0
	
line_interpolate_end:
	addiu	s3,	s3,	1
row_interpolate:	
	move 	t4,	v0
	move	t5,	v1	
	lw	s2,	88(sp)
	andi	t6,	a2,	0xffff
	li	t7,	0x10000
	subu	t7,	t7,	t6
	
row_loop_start:	
	lbu	t8,	0(t4)
	lbu	t9,	0(t5)
	
	mult	t8,	t7
	lbu	s7,	1(t4)
	madd	t9,	t6
	lbu	s6,	1(t5)
	mflo	t3
	sra	t3,	t3,	16
	sb	t3,	0(s0)
	
	mult	s7,	t7
	addiu	t4,	t4,	2
	madd	s6,	t6
	addiu	t5,	t5,	2
	mflo	t3
	sra	t3,	t3,	16
	sb	t3,	1(s0)
			
	addiu	s2,	s2,	-1
	bne	s2,	zero,	row_loop_start
	addiu	s0,	s0,	2
	
	addiu	s1,	s1,	-1
	bne	s1,	zero,	loop_height_start
	addu	a2,	a2,	t1
		
loop_height_end:

	move	t5,	v0	
	lw	s2,	88(sp)
lastline_start:	
	lw	t8,	0(v0)
	lw	t9,	4(v0)
	addiu	v0,	v0,	8	
	sw	t8,	0(s0)
	sw	t9,	4(s0)			
	addiu	s2,	s2,	-4
	bne	s2,	zero,	lastline_start
	addiu	s0,	s0,	8


	lw 	ra, 	60(sp)	
	lw 	fp, 	56(sp)	
	lw 	s0, 	52(sp)	
	lw 	s1, 	48(sp)	
	lw 	s2, 	44(sp)	
	lw 	s3, 	40(sp)	
	lw 	s4, 	36(sp)	
	lw 	s5, 	32(sp)	
	lw 	s6, 	28(sp)	
	lw 	s7, 	24(sp)	
	lw	v0,	20(sp)
	lw	v1,	16(sp)
	jr 	ra 			
	addu 	sp, 	sp, 	64 	
	.set reorder
	.end VidRec_bilinear_zoom


