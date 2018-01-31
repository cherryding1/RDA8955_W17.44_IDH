#include "regdef.h"

	.data			
	.size W,36
W:	.word 	2408		
	.word 	565		
	.word 	2276
	.word 	3406
	.word 	799
	.word 	4017
	.word 	1568
	.word 	3784
	.word 	1108

	.size trans_sat_table,384
trans_sat_table:
	.byte   0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.byte	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.byte	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.byte	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.byte	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15
	.byte	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31
	.byte	32,	33,	34,	35,	36,	37,	38,	39,	40,	41,	42,	43,	44,	45,	46,	47
	.byte	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	62,	63
	.byte	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	77,	78,	79
	.byte	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	93,	94,	95
	.byte	96,	97,	98,	99,	100, 	101, 	102, 	103, 	104, 	105, 	106, 	107, 	108, 	109, 	110, 	111
	.byte	112, 	113, 	114, 	115, 	116, 	117, 	118, 	119, 	120, 	121, 	122, 	123, 	124, 	125, 	126, 	127
        .byte	128, 	129, 	130, 	131,	132, 	133, 	134, 	135,	136, 	137, 	138, 	139,	140, 	141, 	142, 	143
	.byte	144, 	145, 	146, 	147,	148, 	149, 	150, 	151,	152, 	153, 	154, 	155,	156, 	157, 	158, 	159
	.byte	160, 	161, 	162, 	163,	164, 	165, 	166, 	167,	168, 	169, 	170, 	171,	172, 	173, 	174, 	175
	.byte	176, 	177, 	178, 	179,	180, 	181, 	182, 	183,	184, 	185, 	186, 	187,	188, 	189, 	190, 	191
	.byte	192, 	193, 	194, 	195,	196, 	197, 	198, 	199,	200, 	201, 	202, 	203,	204, 	205, 	206, 	207
	.byte	208, 	209, 	210, 	211,	212, 	213, 	214, 	215,	216, 	217, 	218, 	219,	220, 	221, 	222, 	223
	.byte	224, 	225, 	226, 	227,	228, 	229, 	230, 	231,	232, 	233, 	234, 	235, 	236, 	237, 	238, 	239
	.byte	240, 	241, 	242, 	243,	244, 	245, 	246, 	247,	248, 	249, 	250, 	251,	252, 	253, 	254, 	255
	.byte	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255
	.byte	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255
	.byte	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255
	.byte	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255, 	255

//void mpeg4_simple_idct_put(uint8_t *dest, int line_size, DCTELEM *block)	
	.text
	.globl mpeg4_simple_idct_put
	.ent mpeg4_simple_idct_put
	.set noreorder
//	.set nomacro
mpeg4_simple_idct_put:
	subu 	sp, 	sp, 	64 	
	sw 	ra, 	60(sp) 		
	#sw 	fp, 	56(sp) 		
	sw 	s0, 	52(sp) 		
	sw 	s1, 	48(sp) 		
	sw 	s2, 	44(sp) 		          
	sw 	s3, 	40(sp) 		
	sw 	s4, 	36(sp) 		
	sw 	s5, 	32(sp) 		
	sw 	s6, 	28(sp) 		
	sw 	s7, 	24(sp) 		
	
	move 	s6, 	a2 		
	move 	s5, 	a0 		
		
	li	s7,	8 		
	la	s3,	W
	la	s4,	trans_sat_table
	addi	s4,	s4, 	64
	
$loop_line_put:	
	lh  	t1, 	8(s6)
	lh  	t2, 	12(s6)
	lh  	t3, 	4(s6)
	lh  	t4, 	2(s6)
	lh  	t5, 	14(s6)
	lh  	t6, 	10(s6)
	lh  	t7, 	6(s6)
	
	or	v0,	t4,	t5
	or	v1,	t6,	t7
	or	t8,	v0,	v1
	or	v0,	t1,	t2
	or	v1,	v0,	t3
	or	s1,	t8,	v1
	
	bne	s1,	zero,	$caculate_line_stage_put
	lh	t0,	0(s6)
	
		
	sll	v0,	t0,	0x3
	sh	v0,	0(s6)
	sh	v0,	2(s6)	
	sh	v0,	4(s6)
	sh	v0,	6(s6)
	sh	v0,	8(s6)
	sh	v0,	10(s6)	
	sh	v0,	12(s6)
	
	j	$end_one_line_put
	sh	v0,	14(s6)
	

$caculate_line_stage_put:
	
	lw	v1,	4(s3)		
	add	v0,	t4,	t5
	mult	v0,	v1
	lw	v1,	8(s3) 		
	mflo	t8
	mult	v1,	t4
	lw	v1,	12(s3)		
	mflo	s0
	add	t4,	s0,	t8
	mult	v1,	t5
	add	v0,	t6,	t7
	mflo	s0 
	sub	t5,	t8,	s0
	lw	v1,	0(s3)		
	lw	s0,	20(s3)		
	mult	v1,	v0
	lw	v0,	16(s3)		
	mflo	t8
	mult	t6,	v0
	add	v0,	t0,	t1
	mflo	t6
	sub	t6,	t8,	t6
	mult	s0,	t7
	sub	s0,	t0,	t1	
	mflo	t7
	sub	t7,	t8,	t7
	
	sll	v0,	v0,	11
	sll	s0,	s0,	11
	addi	t8,	v0,	128	
	addi	t0,	s0,	128	
	
	lw	v1,	32(s3)		
	add	v0,	t3,	t2
	mult	v0,	v1
	lw	s0,	28(s3)		
	mflo	t1			
	mult	s0,	t2
	lw	s0,	24(s3)		
	mflo	s1
	mult	s0,	t3
	sub	t2,	t1,	s1	
	mflo	s2
	add	t3,	t1,	s2	
	add	t1,	t4,	t6	
	sub	t4,	t4,	t6	
	add	t6,	t5,	t7	
	sub	t5,	t5,	t7	
	
	add	t7,	t8,	t3	
	sub	t8,	t8,	t3	
	add	t3,	t0,	t2	
	sub	t0,	t0,	t2	
	li	s0,	181
	add	s1,	t4,	t5
	sub	s2,	t4,	t5
	addi	s1,	s1,	128
	addi	s2,	s2,	128
	sra	s1,	s1,	8
	mult	s0,	s1
	sra	s2,	s2,	8
	mflo	t2
	mult	s0,	s2
	add	v0,	t7,	t1	
	mflo	t4
	

	sra	s0,	v0,	8
	sh	s0,	0(s6)
	add	v0,	t3,	t2
	sra	s0,	v0,	8
	sh	s0,	2(s6)
	add	v0,	t0,	t4
	sra	s0,	v0,	8
	sh	s0,	4(s6)
	add	v0,	t8,	t6
	sra	s0,	v0,	8
	sh	s0,	6(s6)
	sub	v0,	t8,	t6
	sra	s0,	v0,	8
	sh	s0,	8(s6)
	sub	v0,	t0,	t4
	sra	s0,	v0,	8
	sh	s0,	10(s6)
	sub	v0,	t3,	t2
	sra	s0,	v0,	8
	sh	s0,	12(s6)
	sub	v0,	t7,	t1
	sra	s0,	v0,	8
	sh	s0,	14(s6)
	
	
$end_one_line_put:
	sub	s7,	s7,	1
	
	bne	s7,	zero,	$loop_line_put	
	add	s6,	s6,	16

	
	li	s7,	8 		
	move 	s6, 	a2
$loop_col_put:	
	
	lh  	t1, 	64(s6)
	lh  	t2, 	96(s6)
	lh  	t3, 	32(s6)
	lh  	t4, 	16(s6)
	lh  	t5, 	112(s6)
	lh  	t6, 	80(s6)
	lh  	t7, 	48(s6)
	
	or	v0,	t4,	t5
	or	v1,	t6,	t7
	or	t8,	v0,	v1
	or	v0,	t1,	t2
	or	v1,	v0,	t3
	or	s1,	t8,	v1
	
	bne	s1,	zero,	$caculate_col_stage_put
	lh	t0,	0(s6)
	
//	move	v1,	s5
	add	v1,	s5,	a1	
	sra	v0,	t0,	0x6
	add	v0,	v0,	s4
	lb	v0,	0(v0)
//	nop	
	add	t0,	v1,	a1
//	sb	v0,	0(v1)
	sb	v0,	0(s5)
//	add	v1,	v1,	a1
	sb	v0,	0(v1)		
//	add	v1,	v1,	a1
//	sb	v0,	0(v1)
	sb	v0,	0(t0)
//	add	v1,	v1,	a1
	add	v1,	t0,	a1
	sb	v0,	0(v1)
	add	v1,	v1,	a1
	sb	v0,	0(v1)
	add	v1,	v1,	a1
	sb	v0,	0(v1)	
	add	v1,	v1,	a1
	sb	v0,	0(v1)
	add	v1,	v1,	a1
	j	$end_one_col_put
	sb	v0,	0(v1)

$caculate_col_stage_put:
	
	lw	v1,	4(s3)		
	add	v0,	t4,	t5
	mult	v0,	v1
	lw	v1,	8(s3)		
	mflo	t8
	mult	v1,	t4
	lw	v1,	12(s3)		
	mflo	s0
	add	t4,	s0,	t8
	mult	v1,	t5
	add	v0,	t6,	t7
	mflo	s0
	sub	t5,	t8,	s0
	lw	v1,	0(s3)		
	lw	s0,	20(s3)		
	mult	v1,	v0
	lw	v0,	16(s3)		
	mflo	t8
	mult	t6,	v0
	add	v0,	t0,	t1
	mflo	t6
	sub	t6,	t8,	t6
	mult	s0,	t7
	sub	s0,	t0,	t1	
	mflo	t7
	sub	t7,	t8,	t7	
	
	sra	t4,	t4,	3
	sra	t5,	t5,	3
	sra	t6,	t6,	3
	sra	t7,	t7,	3
		
	
	
	sll	v0,	v0,	8
	sll	s0,	s0,	8
	addi	t8,	v0,	8192	
	addi	t0,	s0,	8192	
	
	lw	v1,	32(s3)		
	add	v0,	t3,	t2
	mult	v0,	v1
	lw	s0,	28(s3)		
	mflo	t1			
	mult	s0,	t2
	lw	s0,	24(s3)		
	mflo	s1
	mult	s0,	t3
	sub	t2,	t1,	s1	
	mflo	s2
	add	t3,	t1,	s2	
	add	t1,	t4,	t6	
	sub	t4,	t4,	t6	
	add	t6,	t5,	t7	
	sub	t5,	t5,	t7	
	
	
	sra	t2,	t2,	3
	sra	t3,	t3,	3

	
	add	t7,	t8,	t3	
	sub	t8,	t8,	t3	
	add	t3,	t0,	t2	
	sub	t0,	t0,	t2	
	li	s0,	181
	add	s1,	t4,	t5
	sub	s2,	t4,	t5
	addi	s1,	s1,	128
	addi	s2,	s2,	128
	sra	s1,	s1,	8
	mult	s0,	s1
	sra	s2,	s2,	8
	mflo	t2
	mult	s0,	s2
	add	v0,	t7,	t1	
	mflo	t4
	
	move	v1,	s5
	
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)
	
	
	add	v0,	t3,	t2
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	add	v0,	t0,	t4
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	add	v0,	t8,	t6
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	sub	v0,	t8,	t6
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	sub	v0,	t0,	t4
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	sub	v0,	t3,	t2
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)

	
	sub	v0,	t7,	t1
	sb	s0,	0(v1)
//	add	v1,	v1,	a1
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	lb	s0,	0(s2)
//	nop
	add	v1,	v1,	a1
	sb	s0,	0(v1)
$end_one_col_put:
	sub	s7,	s7,	1
	
	add	s6,	s6,	2
	bne	s7,	zero,	$loop_col_put	
	add	s5,	s5,	1
	
	lw 	ra, 	60(sp) 		
	lw 	s0, 	52(sp) 		
	lw 	s1, 	48(sp) 		
	lw 	s2, 	44(sp) 		
	lw 	s3, 	40(sp) 		
	lw 	s4, 	36(sp) 		
	lw 	s5, 	32(sp) 		
	lw 	s6, 	28(sp) 		
	lw 	s7, 	24(sp) 		
	
	jr 	ra 			
	addu 	sp, 	sp, 	64 	
	
	.set reorder
//	.set macro
	.end mpeg4_simple_idct_put
	
//void mpeg4_simple_idct_add(uint8_t *dest, int line_size, DCTELEM *block)	
	.text
	.globl mpeg4_simple_idct_add
	.ent mpeg4_simple_idct_add
	.set noreorder
//	.set nomacro
mpeg4_simple_idct_add:
	subu 	sp, 	sp, 	64 	
	sw 	ra, 	60(sp) 		
	
	sw 	s0, 	52(sp) 		
	sw 	s1, 	48(sp) 		
	sw 	s2, 	44(sp) 		          
	sw 	s3, 	40(sp) 		
	sw 	s4, 	36(sp) 		
	sw 	s5, 	32(sp) 		
	sw 	s6, 	28(sp) 		
	sw 	s7, 	24(sp) 		
		
	move 	s6, 	a2 		
	move 	s5, 	a0 		
	
	
	li	s7,	8 		
	la	s3,	W
	la	s4,	trans_sat_table
	addi	s4,	s4, 	64
	
$loop_line_add:	
		
	lh  	t1, 	8(s6)
	lh  	t2, 	12(s6)
	lh  	t3, 	4(s6)
	lh  	t4, 	2(s6)
	lh  	t5, 	14(s6)
	lh  	t6, 	10(s6)
	lh  	t7, 	6(s6)
	
	or	v0,	t4,	t5
	or	v1,	t6,	t7
	or	t8,	v0,	v1
	or	v0,	t1,	t2
	or	v1,	v0,	t3
	or	s1,	t8,	v1
	
	bne	s1,	zero,	$caculate_line_stage_add
	lh	t0,	0(s6)
	
		
	sll	v0,	t0,	0x3
	sh	v0,	0(s6)
	sh	v0,	2(s6)	
	sh	v0,	4(s6)
	sh	v0,	6(s6)
	sh	v0,	8(s6)
	sh	v0,	10(s6)	
	sh	v0,	12(s6)
	
	j	$end_one_line_add
	sh	v0,	14(s6)
	

$caculate_line_stage_add:
	
	lw	v1,	4(s3)		
	add	v0,	t4,	t5
	mult	v0,	v1
	lw	v1,	8(s3) 		
	mflo	t8
	mult	v1,	t4
	lw	v1,	12(s3)		
	mflo	s0
	add	t4,	s0,	t8
	mult	v1,	t5
	add	v0,	t6,	t7
	mflo	s0 
	sub	t5,	t8,	s0
	lw	v1,	0(s3)		
	lw	s0,	20(s3)		
	mult	v1,	v0
	lw	v0,	16(s3)		
	mflo	t8
	mult	t6,	v0
	add	v0,	t0,	t1
	mflo	t6
	sub	t6,	t8,	t6
	mult	s0,	t7
	sub	s0,	t0,	t1	
	mflo	t7
	sub	t7,	t8,	t7
	
	
	sll	v0,	v0,	11
	sll	s0,	s0,	11
	addi	t8,	v0,	128	
	addi	t0,	s0,	128	
	
	lw	v1,	32(s3)		
	add	v0,	t3,	t2
	mult	v0,	v1
	lw	s0,	28(s3)		
	mflo	t1			
	mult	s0,	t2
	lw	s0,	24(s3)		
	mflo	s1
	mult	s0,	t3
	sub	t2,	t1,	s1	
	mflo	s2
	add	t3,	t1,	s2	
	add	t1,	t4,	t6	
	sub	t4,	t4,	t6	
	add	t6,	t5,	t7	
	sub	t5,	t5,	t7	
	
	
	add	t7,	t8,	t3	
	sub	t8,	t8,	t3	
	add	t3,	t0,	t2	
	sub	t0,	t0,	t2	
	li	s0,	181
	add	s1,	t4,	t5
	sub	s2,	t4,	t5
	addi	s1,	s1,	128
	addi	s2,	s2,	128
	sra	s1,	s1,	8
	mult	s0,	s1
	sra	s2,	s2,	8
	mflo	t2
	mult	s0,	s2
	add	v0,	t7,	t1	
	mflo	t4

	sra	s0,	v0,	8
	sh	s0,	0(s6)
	add	v0,	t3,	t2
	sra	s0,	v0,	8
	sh	s0,	2(s6)
	add	v0,	t0,	t4
	sra	s0,	v0,	8
	sh	s0,	4(s6)
	add	v0,	t8,	t6
	sra	s0,	v0,	8
	sh	s0,	6(s6)
	sub	v0,	t8,	t6
	sra	s0,	v0,	8
	sh	s0,	8(s6)
	sub	v0,	t0,	t4
	sra	s0,	v0,	8
	sh	s0,	10(s6)
	sub	v0,	t3,	t2
	sra	s0,	v0,	8
	sh	s0,	12(s6)
	sub	v0,	t7,	t1
	sra	s0,	v0,	8
	sh	s0,	14(s6)
	
	
$end_one_line_add:
	sub	s7,	s7,	1
	
	bne	s7,	zero,	$loop_line_add	
	add	s6,	s6,	16
	
	li	s7,	8 		
	move 	s6, 	a2
	move	s5,	a0
$loop_col_add:	
		
	lh  	t1, 	64(s6)
	lh  	t2, 	96(s6)
	lh  	t3, 	32(s6)
	lh  	t4, 	16(s6)
	lh  	t5, 	112(s6)
	lh  	t6, 	80(s6)
	lh  	t7, 	48(s6)
	
	or	v0,	t4,	t5
	or	v1,	t6,	t7
	or	t8,	v0,	v1
	or	v0,	t1,	t2
	or	v1,	v0,	t3
	or	s1,	t8,	v1
	
	bne	s1,	zero,	$caculate_col_stage_add
	lh	t0,	0(s6)
	
//	move	v1,	s5
//	lbu	t9,	0(v1)		
	lbu	t9,	0(s5)		
	sra	t0,	t0,	6	
	add	s2,	t0,	s4	
	add	s2,	s2,	t9	
	lbu	s0,	0(s2)	
	
//	nop
	add	v1,	s5,	a1
//	sb	s0,	0(v1)
	sb	s0,	0(s5)
//	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

//	nop
	add	v0,	v1,	a1
	sb	s0,	0(v1)
//	add	v1,	v1,	a1
//	lbu	t9,	0(v1)
	lbu	t9,	0(v0)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

//	nop
	add	v1,	v0,	a1
//	sb	s0,	0(v1)
	sb	s0,	0(v0)
//	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
//	nop
	add	v0,	v1,	a1
	sb	s0,	0(v1)
//	add	v1,	v1,	a1
//	lbu	t9,	0(v1)
	lbu	t9,	0(v0)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)
	
//	nop
	add	v1,	v0,	a1
//	sb	s0,	0(v1)
	sb	s0,	0(v0)
//	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
//	nop
	add	v0,	v1,	a1
	sb	s0,	0(v1)
//	add	v1,	v1,	a1
//	lbu	t9,	0(v1)
	lbu	t9,	0(v0)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)
	
//	nop
	add	v1,	v0,	a1
//	sb	s0,	0(v1)
	sb	s0,	0(v0)
//	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	add	s2,	t0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)



	j	$end_one_col_add
	sb	s0,	0(v1)



$caculate_col_stage_add:
	lw	v1,	4(s3)		
	add	v0,	t4,	t5
	mult	v0,	v1
	lw	v1,	8(s3)		
	mflo	t8
	mult	v1,	t4
	lw	v1,	12(s3)		
	mflo	s0
	add	t4,	s0,	t8
	mult	v1,	t5
	add	v0,	t6,	t7
	mflo	s0
	sub	t5,	t8,	s0
	lw	v1,	0(s3)		
	lw	s0,	20(s3)		
	mult	v1,	v0
	lw	v0,	16(s3)		
	mflo	t8
	mult	t6,	v0
	add	v0,	t0,	t1
	mflo	t6
	sub	t6,	t8,	t6
	mult	s0,	t7
	sub	s0,	t0,	t1	
	mflo	t7
	sub	t7,	t8,	t7	
	sra	t4,	t4,	3
	sra	t5,	t5,	3
	sra	t6,	t6,	3
	sra	t7,	t7,	3
		
	
	sll	v0,	v0,	8
	sll	s0,	s0,	8
	addi	t8,	v0,	8192	
	addi	t0,	s0,	8192	
	
	lw	v1,	32(s3)		
	add	v0,	t3,	t2
	mult	v0,	v1
	lw	s0,	28(s3)		
	mflo	t1			
	mult	s0,	t2
	lw	s0,	24(s3)		
	mflo	s1
	mult	s0,	t3
	sub	t2,	t1,	s1	
	mflo	s2
	add	t3,	t1,	s2	
	add	t1,	t4,	t6	
	sub	t4,	t4,	t6	
	add	t6,	t5,	t7	
	sub	t5,	t5,	t7	
	
	sra	t2,	t2,	3
	sra	t3,	t3,	3

	
	add	t7,	t8,	t3	
	sub	t8,	t8,	t3	
	add	t3,	t0,	t2	
	sub	t0,	t0,	t2	
	li	s0,	181
	add	s1,	t4,	t5
	sub	s2,	t4,	t5
	addi	s1,	s1,	128
	addi	s2,	s2,	128
	sra	s1,	s1,	8
	mult	s0,	s1
	sra	s2,	s2,	8
	mflo	t2
	mult	s0,	s2
	add	v0,	t7,	t1	
	mflo	t4

	
	move	v1,	s5
	lbu	t9,	0(v1)		
	sra	s0,	v0,	14	
	add	s2,	s0,	s4	
	add	s2,	s2,	t9
	lbu	s0,	0(s2)
	
	
	add	v0,	t3,	t2
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	add	v0,	t0,	t4
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	add	v0,	t8,	t6
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	sub	v0,	t8,	t6
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	sub	v0,	t0,	t4
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	sub	v0,	t3,	t2
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)

	
	sub	v0,	t7,	t1
	sb	s0,	0(v1)
	add	v1,	v1,	a1
	lbu	t9,	0(v1)
	sra	s0,	v0,	14
	add	s2,	s0,	s4
	add	s2,	s2,	t9
	lbu	s0,	0(s2)
	nop
	sb	s0,	0(v1)
	
	
$end_one_col_add:
	sub	s7,	s7,	1
	
	add	s6,	s6,	2
	bne	s7,	zero,	$loop_col_add	
	add	s5,	s5,	1
	
	

	lw 	ra, 	60(sp) 		
	lw 	s0, 	52(sp) 		
	lw 	s1, 	48(sp) 		
	lw 	s2, 	44(sp) 		
	lw 	s3, 	40(sp) 		
	lw 	s4, 	36(sp) 		
	lw 	s5, 	32(sp) 		
	lw 	s6, 	28(sp) 		
	lw 	s7, 	24(sp) 		
	
	jr 	ra 			
	addu 	sp, 	sp, 	64 	
	
	.set reorder
//	.set macro
	.end mpeg4_simple_idct_add

