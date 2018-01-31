//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/// @file spal_irq_handler.S
///
/// This file contains the source code of the IRQ handler. The IRQ handler is 
/// written in assembly language.
///
/// @date 16/05/07
/// @author Laurent Bastide
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------


#include "global_macros.h"
#include "regdef.h"
#include "cp0.h"
#include "spalp_gdb_stub.h"
#include "bb_irq_asm.h"

.globl bcpu_sp_context
//------------------------------------------------------------------------------
// Defines 
//------------------------------------------------------------------------------

/// @todo maybe find a better place for these definitions
#define HIP_FIRST_LINE  (0x400)
#define BCPU_HIP_MAIN_IRQ   (HIP_FIRST_LINE << BCPU_MAIN_IRQ_LINE)
//#define BCPU_HIP_PAGE_SPY   (HIP_FIRST_LINE << BCPU_PAGE_SPY_IRQ_LINE)
#define BCPU_HIP_DEBUG      (HIP_FIRST_LINE << BCPU_DEBUG_IRQ_LINE)
#define BCPU_HIP_HOST       (HIP_FIRST_LINE << BCPU_HOST_IRQ_LINE)

#define    REG_AT    0x00
#define REG_V0    0x04
#define REG_V1    0x08
#define REG_A0  0x0c 
#define    REG_A1    0x10
#define    REG_A2    0x14
#define    REG_A3    0x18
#define    REG_T0    0x1c
#define    REG_T1    0x20
#define REG_T2    0x24
#define REG_S0    0x28
#define REG_S1    0x2c
#define REG_T8    0x30
#define REG_T9    0x34
//PC Not needed: EPC, and no reentry nor tasks
#define    REG_RA    0x38
//SP Not needed: calculated
#define REG_LO    0x3c
#define    REG_HI    0x40
                
// number of register rounded up to even number x4
#define CONTEXT_SIZE  0x48 // Room for the full context

//--------------------
// INTERRUPT TREATMENT 
//--------------------
    .text

    // internally defined global references
    .globl    __bb_int_handler
    .ent    __bb_int_handler
    .frame    sp, 0, ra

__bb_int_handler:

    // read cause in CPU 
    mfc0    k0, Cause
    
    // read cause register 
    andi    k1, k0, (BCPU_HIP_DEBUG | Cause_ExcCode)    // extract exception code + debug irq
    
    bnez    k1,_bb_go_to_gdb 

    // none of the previous we have an external interrupt 
    
    // make room on stack 
    subu    sp, sp, CONTEXT_SIZE
    // save regs allowed for use in bb_irq_handler.c 
    sw        t0, REG_T0(sp)
    sw        t1, REG_T1(sp)
    sw        t2, REG_T2(sp)
    sw        t8, REG_T8(sp)
    sw        t9, REG_T9(sp)
    sw        a0, REG_A0(sp)
    sw        a1, REG_A1(sp)
    sw        a2, REG_A2(sp)
    sw        a3, REG_A3(sp)
    sw        v0, REG_V0(sp)
    sw        v1, REG_V1(sp)
    sw        s0, REG_S0(sp)
    sw        s1, REG_S1(sp)
    .set noat
    sw        r1, REG_AT(sp) 
    .set at
    sw        ra, REG_RA(sp)
    mflo t0
    mfhi t1
    sw      t0, REG_LO(sp)
    sw      t1, REG_HI(sp)

    // go to C handler 
    subu    sp, sp, 16    
    jal        spal_IrqHandler
    addu    sp, sp, 16

    // restore context
    // Restore HI and LOW registers
    lw      t0, REG_LO(sp)
    lw      t1, REG_HI(sp)
    mtlo t0
    mthi t1
    // Restore all regs
    lw        t0, REG_T0(sp)
    lw        t1, REG_T1(sp)
    lw        t2, REG_T2(sp)
    lw        t8, REG_T8(sp)
    lw        t9, REG_T9(sp)
    lw        a0, REG_A0(sp)
    lw        a1, REG_A1(sp)
    lw        a2, REG_A2(sp)
    lw        a3, REG_A3(sp)
    lw        v0, REG_V0(sp)
    lw        v1, REG_V1(sp)
    lw        s0, REG_S0(sp)
    lw        s1, REG_S1(sp)
    .set noat
    lw        r1, REG_AT(sp) 
    .set at
    lw        ra, REG_RA(sp)

    // get the return address
    mfc0    k1, EPC

    // free stack 
    addu    sp, sp, CONTEXT_SIZE

    // return 
    .set noreorder
    jr        k1
    rfe
    .set reorder
    
/* ********************************************************** */
/*              END INTERRUPT TREATMENT                       */
/* ********************************************************** */
//------------------------
// END INTERRUPT TREATMENT 
//------------------------

//
//  GDB  
#define GDB_REG00 0x00
#define GDB_REG01 0x04
#define GDB_REG02 0x08
#define GDB_REG03 0x0c
#define GDB_REG04 0x10
#define GDB_REG05 0x14
#define GDB_REG06 0x18
#define GDB_REG07 0x1c
#define GDB_REG08 0x20
#define GDB_REG09 0x24
#define GDB_REG10 0x28
#define GDB_REG11 0x2c
#define GDB_REG12 0x30
#define GDB_REG13 0x34
#define GDB_REG14 0x38
#define GDB_REG15 0x3c
#define GDB_REG16 0x40
#define GDB_REG17 0x44
#define GDB_REG18 0x48
#define GDB_REG19 0x4c
#define GDB_REG20 0x50
#define GDB_REG21 0x54
#define GDB_REG22 0x58
#define GDB_REG23 0x5c
#define GDB_REG24 0x60
#define GDB_REG25 0x64
#define GDB_REG26 0x68
#define GDB_REG27 0x6c
#define GDB_REG28 0x70
#define GDB_REG29 0x74
#define GDB_REG30 0x78
#define GDB_REG31 0x7c
#define GDB_REGSR 0x80
#define GDB_REGLO 0x84
#define GDB_REGHI 0x88
#define GDB_REGBAD 0x8c
#define GDB_REGCAUSE 0x90
#define GDB_REGEPC 0x94

_bb_go_to_gdb:
    // address for context on k1 
    // lw        k1, bcpu_sp_context
    // make room on stack 
    subu    sp, sp, SPAL_GDB_CONTEXT_SIZE
    // Enough place is saved on the stack to save the full context
    // save context 
    sw        r0,  GDB_REG00(sp) // this is always 0 but we want it for gdb
    .set noat
    sw        r1,  GDB_REG01(sp) 
    .set at
    sw        r2,  GDB_REG02(sp)
    sw        r3,  GDB_REG03(sp)
    sw        r4,  GDB_REG04(sp)
    sw        r5,  GDB_REG05(sp)
    sw        r6,  GDB_REG06(sp)
    sw        r7,  GDB_REG07(sp)
    sw        r8,  GDB_REG08(sp)
    sw        r9,  GDB_REG09(sp)
    sw        r10, GDB_REG10(sp)
    sw        r11, GDB_REG11(sp)
    sw        r12, GDB_REG12(sp)
    sw        r13, GDB_REG13(sp)
    sw        r14, GDB_REG14(sp)
    sw        r15, GDB_REG15(sp)
    sw        r16, GDB_REG16(sp)
    sw        r17, GDB_REG17(sp)
    sw        r18, GDB_REG18(sp)
    sw        r19, GDB_REG19(sp)
    sw        r20, GDB_REG20(sp)
    sw        r21, GDB_REG21(sp)
    sw        r22, GDB_REG22(sp)
    sw        r23, GDB_REG23(sp)
    sw        r24, GDB_REG24(sp)
    sw        r25, GDB_REG25(sp)
    // sp and k1 are destroyed saved 0 for gdb 
    sw        r0, GDB_REG26(sp)
    sw        r0, GDB_REG27(sp)
    sw        r28, GDB_REG28(sp)
    sw        r29, GDB_REG29(sp)
    sw        r30, GDB_REG30(sp)
    sw        r31, GDB_REG31(sp)
    // store HI and LO registers 
    mflo    t0
    mfhi    t1
    sw        t0, GDB_REGLO(sp)
    sw        t1, GDB_REGHI(sp)
    // store Status, BadVAddr 
    mfc0    t0, Status
    mfc0    t1, BadVAddr
    sw      t0, GDB_REGSR(sp)
    sw      t1, GDB_REGBAD(sp)
    
    // store Cause and EPC registers 
    mfc0    t1, EPC
    sw      k0, GDB_REGCAUSE(sp)
    sw      t1, GDB_REGEPC(sp)

    // store context address in bcpu_sp_context
    sw      sp, bcpu_sp_context

    // frame for paramters 
    subu    sp, sp, 16
    
    andi    t0, k0, Cause_ExcCode     // extract exception code 
    bne        t0, ExcCode_BREAK, $L_skip_break

    // branch to breakpoint
    andi    t0, t1, 1                // check for mips16 ISA 
    beqz    t0, $L_isa32_bp
$L_isa16_bp:
    lhu        t0, -1(t1)                // t1 holds &EPC+1 in mips16 
    srl        t0, 3                    
    j         $L_isa_common
$L_isa32_bp:
    // branch to breakpoint
    lw      t0,    0(t1)                // t1 was still epc, get instruction 
    srl     t0,    14
$L_isa_common:
    la      t1,    spal_GdbGBreak        // get break table address 
    andi    t0,    t0,    0x3c
    addu    t0, t0,    t1                    // add break number 
    lw        t0,    0(t0)
    jalr     t0                        // call the implementation function 
    j        $L_skip_irq
$L_skip_break:
    beqz    t0, $L_skip_exception        // an exception? 
    move    a1, k0                    // cause 
//    la        t0, spal_GdbException
//    jalr    t0
    jal     spal_GdbException
    j        $L_skip_irq
$L_skip_exception:
    la        t0, spal_GdbIrqDebug
    jalr    t0
$L_skip_irq:

    // frame for paramters 
    addu    sp, sp, 16

    // restore context 

    // restore HI and LO registers 
    lw  t0, GDB_REGLO(sp)
    mtlo t0
    lw  t0, GDB_REGHI(sp)
    mthi t0
    // restore status 
    lw    t0, GDB_REGSR(sp)
    mtc0    t0, Status
    // restore all registers 
    lw    r31, GDB_REG31(sp)
    lw    r30, GDB_REG30(sp)
    // sp is not restored
    // lw    r29, GDB_REG29(sp)
    lw    r28, GDB_REG28(sp)
    // k0 and sp are not restored
    // lw    r27, GDB_REG27(sp)
    // lw    r26, GDB_REG26(sp)
    lw    r25, GDB_REG25(sp)
    lw    r24, GDB_REG24(sp)
    lw    r23, GDB_REG23(sp)
    lw    r22, GDB_REG22(sp)
    lw    r21, GDB_REG21(sp)
    lw    r20, GDB_REG20(sp)
    lw    r19, GDB_REG19(sp)
    lw    r18, GDB_REG18(sp)
    lw    r17, GDB_REG17(sp)
    lw    r16, GDB_REG16(sp)
    lw    r15, GDB_REG15(sp)
    lw    r14, GDB_REG14(sp)
    lw    r13, GDB_REG13(sp)
    lw    r12, GDB_REG12(sp)
    lw    r11, GDB_REG11(sp)
    lw    r10, GDB_REG10(sp)
    lw    r9,  GDB_REG09(sp)
    lw    r8,  GDB_REG08(sp)
    lw    r7,  GDB_REG07(sp)
    lw    r6,  GDB_REG06(sp)
    lw    r5,  GDB_REG05(sp)
    lw    r4,  GDB_REG04(sp)
    lw    r3,  GDB_REG03(sp)
    lw    r2,  GDB_REG02(sp)
    .set noat
    lw    r1,  GDB_REG01(sp) 
    .set at

    lw  k1, GDB_REGEPC(sp)
    addu    sp, sp, SPAL_GDB_CONTEXT_SIZE
    .set noreorder
    j   k1                 // go back
    rfe
    .set reorder

    .end    __bb_int_handler

// =============================================================================
// spal_BbGetSp
// -----------------------------------------------------------------------------
/// This function returns the current value of the stack register
/// @return The current value of the stack pointer.
// =============================================================================
//UINT32 spal_BbGetSp(void)
//{
    .globl  spal_BbGetSp
    .globl  spal_BbGetSp
    .ent    spal_BbGetSp
spal_BbGetSp:
spal_BbGetSp:    
    .frame sp, 0, ra

    move    v0,sp

    j    ra

    .type   spal_BbGetSp,@function
    .size   spal_BbGetSp,.-spal_BbGetSp

    .end spal_BbGetSp
//}


//-------------------------------------
// VOID spal_IrqInit(BOOL irqInRom)
//-------------------------------------
    .text
    .globl    spal_IrqInit
    .ent    spal_IrqInit
    .frame    sp, 0, ra
spal_IrqInit:

    // keep the BEV bit for this version which running from the rom
    li  r27, Status_BEV | Status_IEc  |Status_IntMASK | 0x40000000

    bnez a0, $L_irq_in_rom
    nop
    li  r27, Status_IEc | Status_IntMASK | 0x40000000
    
    $L_irq_in_rom:
    mtc0    zero, Cause
    mtc0     r27, Status
    
//    li  t0, KSEG1(REG_BB_IRQ_BASE_HI)
//    li    t1, 1
//    sw  t1, REG_BB_IRQ_SC(t0)
//    li    t1, SYS_IRQ_MASK_SET_MASK
//    sw  t1, REG_BB_IRQ_MASK_SET(t0)
    jr ra
    .type    spal_IrqInit,@function
    .size    spal_IrqInit,.-spal_IrqInit

    .end    spal_IrqInit


