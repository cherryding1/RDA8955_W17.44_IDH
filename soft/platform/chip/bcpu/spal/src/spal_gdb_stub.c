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


#include "cs_types.h"


#include "global_macros.h"
#include "sys_ctrl.h"
#include "sys_irq.h"
#include "bb_irq.h"
#include "debug_host.h"
#include "spal_debug.h"
#include "mem_bridge.h"

#include "spalp_private.h"
#include "spalp_gdb_stub.h"
#include "spalp_debug.h"
#include "spal_mem.h"

#include "cp0.h"
#undef Cause
// to fix conflict between cp0 Cause reg and Irq Cause reg !!

// FIXME Remove SPAL_ROMULATOR flag at it is
// meaningless and not implemented.


#define EXL_FATAL 1

#define SPAL_GDB_BCPU_EVENT  0x9db10000

// ****************************************************************************
//                             LOCAL CONSTANTS
// ****************************************************************************

#define SPAL_GDB_OPCODE_STEP      0x0005000d
#define SPAL_GDB_16_OPCODE_STEP   0xe8a5
#define SPAL_GDB_SIGTRAP          5
#define SPAL_GDB_SIGINT           2
#define SPAL_GDB_SIGSEGV          11
#define SPAL_GDB_SIGILL           4
#define SPAL_GDB_SIGBUS           10
#define SPAL_GDB_SIGKILL          9
#define SPAL_GDB_SIGFPE           8


// ****************************************************************************
//                               LOCAL MACROS
// ****************************************************************************


//  Manage 16/32 bits addressing modes
#define INC_STEP ((spal_GdbRegs[37] & 0x01) ? 2 : 4)
#define MASK_ONE(addr) ((addr)&(~1))

#define SPAL_GDB_GET_OFFSET(op)   (((((op) & 0x00008000) == 0x00008000) ? \
        ((op) | 0xffff0000) : ((op) & 0x0000ffff)) << 2)

#define SPAL_GDB_OPCODE_J(op)     (((op) & 0xfc000000) == 0x08000000)
#define SPAL_GDB_OPADDR_J(op)     ((((op) & 0x03ffffff) << 2) | (spal_GdbRegs[37] & 0xf0000000))
#define SPAL_GDB_OPCODE_JAL(op)   (((op) & 0xfc000000) == 0x0c000000)
#define SPAL_GDB_OPADDR_JAL(op)   ((((op) & 0x03ffffff) << 2) | (spal_GdbRegs[37] & 0xf0000000))
#define SPAL_GDB_OPCODE_JALX(op)  ((((op) & 0xfc000000) == 0x74000000))
#define SPAL_GDB_OPADDR_JALX(op)  ((((op) & 0x03ffffff) << 2) | (spal_GdbRegs[37] & 0xf0000000) | 1)
#define SPAL_GDB_OPCODE_JR(op)    (((op) & 0xfc1ff83f) == 0x00000008)
#define SPAL_GDB_OPADDR_JR(op)    (spal_GdbRegs[((op) & 0x03e00000) >> 21])
#define SPAL_GDB_OPCODE_JALR(op)  (((op) & 0xfc1f003f) == 0x00000009)
#define SPAL_GDB_OPADDR_JALR(op)  (spal_GdbRegs[((op) & 0x03e00000) >> 21])
#define SPAL_GDB_OPCODE_BEQ(op)   (((op) & 0xfc000000) == 0x10000000)
#define SPAL_GDB_OPADDR_BEQ(op)   ((spal_GdbRegs[((op) & 0x03e00000) >> 21] == spal_GdbRegs[((op)   \
        & 0x001f0000) >> 16]) ? (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BNE(op)   (((op) & 0xfc000000) == 0x14000000)
#define SPAL_GDB_OPADDR_BNE(op)   ((spal_GdbRegs[((op) & 0x03e00000) >> 21] != spal_GdbRegs[((op)   \
        & 0x001f0000) >> 16]) ? (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BGEZ(op)  (((op) & 0xfc1f0000) == 0x04010000)
#define SPAL_GDB_OPADDR_BGEZ(op)  (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] >= 0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BGEZAL(op)    (((op) & 0xfc1f0000) == 0x04110000)
#define SPAL_GDB_OPADDR_BGEZAL(op)    (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] >= 0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BGTZ(op)  (((op) & 0xfc1f0000) == 0x1c000000)
#define SPAL_GDB_OPADDR_BGTZ(op)  (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] >  0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BLEZ(op)  (((op) & 0xfc1f0000) == 0x18000000)
#define SPAL_GDB_OPADDR_BLEZ(op)  (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] <= 0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BLTZ(op)  (((op) & 0xfc1f0000) == 0x04000000)
#define SPAL_GDB_OPADDR_BLTZ(op)  (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] <  0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))
#define SPAL_GDB_OPCODE_BLTZAL(op)    (((op) & 0xfc1f0000) == 0x04100000)
#define SPAL_GDB_OPADDR_BLTZAL(op)    (((INT32)spal_GdbRegs[((op) & 0x03e00000) >> 21] <  0) ?   \
        (spal_GdbRegs[37]+4+SPAL_GDB_GET_OFFSET(op)) : (spal_GdbRegs[37]+4))


//  We pass op as a pointer for mips16 macros, because we may need to access the next 16bits chunk

//  Perform 32bits wide sign extension and left shift for various offsets
#define SPAL_GDB_16_GET_11_OFFSET(op)     ( ( (*(op) & 0x0400) ? \
                ((*(op)) | 0xfffff800) : ((*(op)) & 0x7ff) ) << 1 )
#define SPAL_GDB_16_GET_8_OFFSET(op)      ( ( (*(op) & 0x0080) ? \
                ((*(op)) | 0xffffff00) : ((*(op)) & 0x00ff) ) << 1 )

/// Cf mips16 doc: register encoding in mips16 is a fucking mess.
/// Please refer to the spec or trust the fact that:
/// 0 --> r16
/// 1 --> r17
/// 2 --> r2
/// ..
/// 7 --> r7
#define SPAL_GDB_16_MAP_REGS(n)  (((n)<2)?((n)+16):(n))
#define SPAL_GDB_16_REGS(n)      spal_GdbRegs[SPAL_GDB_16_MAP_REGS(n)]

//  Helper for messy offsets on extended branches
#define SPAL_GDB_16_ASSEMBLE_EXT_OFFSET(op) \
                 ( (*((op)+1) & 0x001f) + ((*(op)&0x07e0))  + ((*(op) & 0x001f) << 11) )
//  GNU C only, but much cleaner...
#define SPAL_GDB_16_GET_EXT_OFFSET(op) \
        ({ UINT32 _off = SPAL_GDB_16_ASSEMBLE_EXT_OFFSET((op)); (_off & 0x8000) ? ((_off | 0xffff0000)<<1) : ((_off & 0x0000ffff)<<1); })

#define SPAL_GDB_16_OPCODE_JAL(op)    ((*(op) & 0xfc00) == 0x1800)
#define SPAL_GDB_16_OPADDR_JAL(op)    ((0x80000001) | ( ((spal_GdbRegs[37] + 2)&0xe0000000) + ( ( *((op)+1) + ((*(op) & 0x001f) << 21) \
        + ((*(op) & 0x03e0) << 11) ) << 2 )) )
#define SPAL_GDB_16_OPCODE_JALX(op)   ((*(op) & 0xfc00) == 0x1c00)
#define SPAL_GDB_16_OPADDR_JALX(op)   ((0x80000000) | (( (spal_GdbRegs[37] + 2)&0xe0000000) + ( ( *((op)+1) + ((*(op) & 0x001f) << 21) \
        + ((*(op) & 0x03e0) << 11 ) ) << 2 )) )
#define SPAL_GDB_16_OPCODE_JALR(op)   ((*(op) & 0xf85f) == 0xe840)
#define SPAL_GDB_16_OPADDR_JALR(op)   (SPAL_GDB_16_REGS((*(op) & 0x0700) >> 8))
#define SPAL_GDB_16_OPCODE_JR(op)     ((*(op) & 0xf85f) == 0xe800)
#define SPAL_GDB_16_OPADDR_JR(op)     ( (*(op) & 0x0020) ? \
        spal_GdbRegs[31] : SPAL_GDB_16_REGS((*(op) & 0x0700) >> 8) )
#define SPAL_GDB_16_OPCODE_B(op)      ((*(op) & 0xf800) == 0x1000)
#define SPAL_GDB_16_OPADDR_B(op)      (spal_GdbRegs[37]+2+SPAL_GDB_16_GET_11_OFFSET(op))
#define SPAL_GDB_16_OPCODE_BEQZ(op)   ((*(op) & 0xf800) == 0x2000)
#define SPAL_GDB_16_OPADDR_BEQZ(op)   ((SPAL_GDB_16_REGS((*(op) & 0x0700) >> 8) == 0 ) ? \
        spal_GdbRegs[37]+2+SPAL_GDB_16_GET_8_OFFSET(op) : spal_GdbRegs[37]+2)
#define SPAL_GDB_16_OPCODE_BNEZ(op)   ((*(op) & 0xf800) == 0x2800)
#define SPAL_GDB_16_OPADDR_BNEZ(op)   ((SPAL_GDB_16_REGS((*(op) & 0x0700) >> 8) != 0 ) ? \
        spal_GdbRegs[37]+2+SPAL_GDB_16_GET_8_OFFSET(op) : spal_GdbRegs[37]+2)
#define SPAL_GDB_16_OPCODE_BTEQZ(op)  ((*(op) & 0xff00) == 0x6000)
#define SPAL_GDB_16_OPADDR_BTEQZ(op)  ((spal_GdbRegs[24] == 0 ) ? \
        spal_GdbRegs[37]+2+SPAL_GDB_16_GET_8_OFFSET(op) : spal_GdbRegs[37]+2)
#define SPAL_GDB_16_OPCODE_BTNEZ(op)  ((*(op) & 0xff00) == 0x6100)
#define SPAL_GDB_16_OPADDR_BTNEZ(op)  ((spal_GdbRegs[24] != 0 ) ? \
        spal_GdbRegs[37]+2+SPAL_GDB_16_GET_8_OFFSET(op) : spal_GdbRegs[37]+2)

//  Separate test for extended insns
#define SPAL_GDB_16_OPCODE_EXTENDED(op)   ((*(op) & 0xf800) == 0xf000)



//  Only valid if already tested as extended
#define SPAL_GDB_16_OPCODE_B_EXT(op)      ((*(&*(op)+1) & 0xf800) == 0x1000)
#define SPAL_GDB_16_OPADDR_B_EXT(op)      ( spal_GdbRegs[37] + 4 + SPAL_GDB_16_GET_EXT_OFFSET(op) )
#define SPAL_GDB_16_OPCODE_BEQZ_EXT(op)       ((*((op)+1) & 0xf800) == 0x2000)
#define SPAL_GDB_16_OPADDR_BEQZ_EXT(op)       ((SPAL_GDB_16_REGS((*((op)+1) & 0x0700) >> 8) == 0) ? \
        ( spal_GdbRegs[37] + 4 + SPAL_GDB_16_GET_EXT_OFFSET(op) ) : spal_GdbRegs[37] + 4 )
#define SPAL_GDB_16_OPCODE_BNEZ_EXT(op)       ((*((op)+1) & 0xf800) == 0x2800)
#define SPAL_GDB_16_OPADDR_BNEZ_EXT(op)       ((SPAL_GDB_16_REGS((*((op)+1) & 0x0700) >> 8) != 0) ? \
        ( spal_GdbRegs[37] + 4 + SPAL_GDB_16_GET_EXT_OFFSET(op) ) : spal_GdbRegs[37] + 4 )
#define SPAL_GDB_16_OPCODE_BTEQZ_EXT(op)      ((*((op)+1) & 0xff00) == 0x6000)
#define SPAL_GDB_16_OPADDR_BTEQZ_EXT(op)      ((spal_GdbRegs[24] == 0) ? \
        ( spal_GdbRegs[37] + 4 + SPAL_GDB_16_GET_EXT_OFFSET(op) ) : spal_GdbRegs[37] + 4 )
#define SPAL_GDB_16_OPCODE_BTNEZ_EXT(op)      ((*((op)+1) & 0xff00) == 0x6100)
#define SPAL_GDB_16_OPADDR_BTNEZ_EXT(op)      ((spal_GdbRegs[24] != 0) ? \
        ( spal_GdbRegs[37] + 4 + SPAL_GDB_16_GET_EXT_OFFSET(op) ) : spal_GdbRegs[37] + 4 )





/// Flag to store a mips16 opcode in spal_GdbGData.opCode
/// and make it different of 0. SPAL_GDB_MIPS16_OPCODE_MASK is used
/// to restore the proper opcode, discarding the flag.
#define SPAL_GDB_MIPS16_OPCODE_FLAG      0xdb900000
#define SPAL_GDB_MIPS16_OPCODE_MASK      0x0000FFFF

/// Patch number used for the one step allowed in ROM
#define SPAL_GDB_PATCH_NUMBER         (CHIP_NUMBER_OF_PATCHES-2)


// =============================================================================
//  TYPES
// =============================================================================
/// GDB data type
typedef struct
{
    UINT32 stepAddr;
    UINT32 opCode;
} SPAL_GDB_DATA_T;


// =============================================================================
// SPAL_DBG_ERROR_CODE_T
// -----------------------------------------------------------------------------
/// GDB Exception word
/// Those are the error detected by the error handler
// =============================================================================
typedef enum
{
    SPAL_DBG_ERROR_NO =           0xC0010000,  ///< no error
    SPAL_DBG_ERROR_IT =           0x1717,      ///< cpu exception: value of cause palced id status
    SPAL_DBG_ERROR_ASSERT =       0xA55E,      ///< assert (n/a with os)
    SPAL_DBG_ERROR_GDB =          0x06DB,      ///< gdb (breakpoint/softbreak)
    SPAL_DBG_ERROR_PAGE =         0xFA9E,      /**< page exception: status build
                                        with: (address &0x01FFFFFF) |
                                        ((master&0xF)<<28) |
                                        (page<<26) */

    SPAL_DBG_ERROR_WDOG =         0xAD09,      ///< watchdog (n/a no gdb restoration)
    SPAL_DBG_ERROR_DIV0 =         0xD1B0,      ///< divide by 0
    SPAL_DBG_ERROR_RAISE =        0xA15E,      ///< Raise (stack asserts or pal asserts with os)
    SPAL_DBG_ERROR_BCPU =         0xBBBB,      ///< BCPU triggered exception (internal use)
} SPAL_DBG_ERROR_CODE_T;

// ****************************************************************************
//                             GLOBAL VARIABLES
// ****************************************************************************

SPAL_GDB_DATA_T   spal_GdbGData SPAL_BBSRAM_GLOBAL= {0,0};
extern UINT32 *    bcpu_sp_context;
extern UINT32      bcpu_error_code;
extern UINT32      bcpu_error_status;


// ****************************************************************************
//                          LOCAL FUNCTION PROTOTYPES
// ****************************************************************************
PRIVATE VOID spal_GdbHandle(VOLATILE UINT32 * regs);
PRIVATE VOID spal_GdbStep (UINT32 nAddr, VOLATILE UINT32 * spal_GdbRegs);
PRIVATE VOID spal_GdbUndoStep (VOLATILE UINT32 * spal_GdbRegs);
PRIVATE VOID spal_GdbSetBreakpoint(VOLATILE UINT32* spal_GdbRegs);
PRIVATE VOID spal_GdbUnsetBreakpoint(VOLATILE UINT32* spal_GdbRegs);

PRIVATE UINT32      spal_GdbStrLen(UINT8* pcStr);
PROTECTED VOID      spal_DbgGdbHstSendEvent(UINT32 ch);
PROTECTED UINT32    spal_Gdb16Step (UINT32 nAddr, VOLATILE UINT32 * spal_GdbRegs);



#define SPAL_DBG_STEPFORBIDDEN_FUNCTION_COUNT 1
PRIVATE CONST UINT32* CONST SPAL_DBG_STEPFORBIDDEN_FUNCTIONS [SPAL_DBG_STEPFORBIDDEN_FUNCTION_COUNT] =
{(UINT32*)spal_DbgGdbHstSendEvent};



// *****************************************************************************
//                         FUNCTION IMPLEMENTATION
// *****************************************************************************
// That is very nice to have a dedicated leaf function to send events, since
// a breakpoint (for conventional debugging) in a function called by that
// #spal_DbgGdbHstSendEvent function, would call GDB into GDB and lead to
// a system collapse.
PROTECTED VOID spal_DbgGdbHstSendEvent(UINT32 ch)
{
    UINT32 count = 0;
    // Enter critical section
    UINT32 status = hwp_bbIrq->SC;

    // Wait for semaphore to be free
    while ((hwp_debugHost->event & 1) == 0 && count++ < 10000);
    hwp_debugHost->event = ch;

    // Exit critical section
    hwp_bbIrq->SC = status;
}



// =============================================================================
// spal_GdbOpen
// -----------------------------------------------------------------------------
/// Initializes the variables used by GDB.
// =============================================================================
VOID spal_GdbOpen(VOID)
{
    spal_GdbGData.stepAddr  = 0;
    spal_GdbGData.opCode    = 0;
    bcpu_sp_context         = (UINT32*)0;
    bcpu_error_code         = 0xc0010000;
    bcpu_error_status       = 0xc001c001;
}

// =============================================================================
// spal_GdbIsAddressStepForbidden
// -----------------------------------------------------------------------------
// Helper function to tell if a function is step-into-able or not. If it's not,
// we will cheat and set the breakpoint after the func call.
// =============================================================================

PRIVATE BOOL spal_GdbIsAddressStepForbidden(UINT32 newAddr)
{
    int i = 0;

    for(i = 0; i<SPAL_DBG_STEPFORBIDDEN_FUNCTION_COUNT; i++)
    {
        if((UINT32)SPAL_DBG_STEPFORBIDDEN_FUNCTIONS[i] == newAddr)
        {
            return TRUE;
        }
    }

    return FALSE;
}

// =============================================================================
// spal_GdbBreaks
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_GdbBreaks(VOID)
{
    VOLATILE UINT32 * spal_GdbRegs;
    spal_GdbRegs = bcpu_sp_context;

    spal_GdbRegs[37]+=INC_STEP;
    return;
}

// =============================================================================
// spal_GdbBreakPrint
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbBreakPrint(VOID)
{
    UINT32     nLen;
    UINT8 * str;
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;

    spal_GdbRegs = bcpu_sp_context;


    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);

    str = (UINT8*)(spal_GdbRegs[4]); // a0 before break


    nLen  = spal_GdbStrLen(str);
//  prepare command
    v_regs[SPAL_GDB_CMD_OFF]=SPAL_GDB_CMD_PRINT;
    v_regs[SPAL_GDB_CMDPARAM_OFF]=nLen;

//  trigger command
    spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT);

//  wait for the debug server to complete
    while(v_regs[SPAL_GDB_CMD_OFF]!=SPAL_GDB_CMD_DONE);

    spal_GdbRegs[37]+=INC_STEP;
//  restart the watchdog
    return;
}

// =============================================================================
// spal_GdbPrepare
// -----------------------------------------------------------------------------
/// @return GDB context (cached)
// =============================================================================
PROTECTED VOLATILE UINT32 *spal_GdbPrepare(UINT32 error_code, UINT32 error_status, BOOL fatal)
{
    VOLATILE UINT32 * spal_GdbRegs;

    spal_GdbRegs = bcpu_sp_context;
    bcpu_error_code = error_code;
    bcpu_error_status = error_status;

    return spal_GdbRegs;
}

// =============================================================================
// spal_GdbSendcmd
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_GdbSendcmd(VOLATILE UINT32 * spal_GdbRegs, UINT32 signal, BOOL fatal)
{
    // prepare command
    spal_GdbRegs[SPAL_GDB_CMD_OFF] = SPAL_GDB_CMD_BREAKPOINT;
    spal_GdbRegs[SPAL_GDB_CMDPARAM_OFF] = signal;
    // trigger command
    spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT);
    // trigger XCPU soft irq
    if (fatal)
    {
        hwp_sysIrq->NonMaskable = SYS_IRQ_DEBUG_IRQ;
    }
}

// =============================================================================
// spal_GdbLoop
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_GdbLoop(VOLATILE UINT32 * v_regs, BOOL fatal)
{
//  wait for the debug server to complete
    while(v_regs[SPAL_GDB_CMD_OFF]!=SPAL_GDB_CMD_DONE);
    spal_GdbUndoStep(v_regs);
    spal_GdbHandle(v_regs);
    if(fatal)
    {
        SPAL_PROFILE_PULSE(BCPU_ERROR);
    }
    return;
}

// =============================================================================
// spal_GdbBreakDivBy0
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbBreakDivBy0(VOID)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;
    spal_GdbRegs = spal_GdbPrepare(SPAL_DBG_ERROR_DIV0, 0, TRUE);

    spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT+0xd0);
    spal_GdbRegs[37]+=INC_STEP;

    // trigger GDB on /0 for both CPUs

    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);

//  send command
    spal_GdbSendcmd(spal_GdbRegs, SPAL_GDB_SIGFPE, TRUE);

    spal_GdbLoop(v_regs, TRUE);
}

// =============================================================================
// spal_GdbBreakBreakPoint
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbBreakBreakPoint(VOID)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;

    spal_GdbRegs = spal_GdbPrepare(SPAL_DBG_ERROR_GDB, 0, FALSE);
    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);

//  send command
    spal_GdbSendcmd(spal_GdbRegs, SPAL_GDB_SIGTRAP, FALSE);

    spal_GdbLoop(v_regs, FALSE);
}

// =============================================================================
// spal_GdbBreakSoftBreakPoint
// -----------------------------------------------------------------------------
/// same as breakpoint but skip the break instruction as it is placed in code not by gdb
// =============================================================================
VOID spal_GdbBreakSoftBreakPoint(VOID)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;
    spal_GdbRegs = spal_GdbPrepare(SPAL_DBG_ERROR_GDB, 0, FALSE);
    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);
    v_regs[37]+=INC_STEP;

//  send command
    spal_GdbSendcmd(spal_GdbRegs, SPAL_GDB_SIGTRAP, FALSE);

    spal_GdbLoop(v_regs, FALSE);
}

// =============================================================================
// spal_GdbBreakRaise
// -----------------------------------------------------------------------------
/// same as breakpoint but skip the break instruction as it is placed in code not by gdb
/// used by stack for raises
// =============================================================================
VOID spal_GdbBreakRaise(VOID)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;
    spal_GdbRegs = spal_GdbPrepare(SPAL_DBG_ERROR_RAISE, 0, TRUE);
    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);
    v_regs[37]+=INC_STEP;

//  send command
    spal_GdbSendcmd(spal_GdbRegs, SPAL_GDB_SIGKILL, TRUE);

    spal_GdbLoop(v_regs, TRUE);
}


// =============================================================================
// spal_GdbIrqDebug
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbIrqDebug(UINT32 cause)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;
    UINT32 error_code = SPAL_DBG_ERROR_GDB;
    BOOL fatal = FALSE;

    hwp_bbIrq->NonMaskable = 0;

    spal_GdbRegs = spal_GdbPrepare(error_code, 0, fatal);
    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);

    if(!fatal)
    {
        // don't actually send command if the BCPU triggered the IT
        spal_GdbSendcmd(spal_GdbRegs, SPAL_GDB_SIGINT, FALSE);
    }

    spal_GdbLoop(v_regs, fatal);
}


// =============================================================================
// spal_GdbException
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbException(UINT32 cause)
{
    VOLATILE UINT32 * spal_GdbRegs;
    VOLATILE UINT32 * v_regs;
    UINT32 signal;
    spal_GdbRegs = spal_GdbPrepare(SPAL_DBG_ERROR_IT, cause, TRUE);

    v_regs=(UINT32*)KSEG1((UINT32)spal_GdbRegs);

//  send command
    switch(cause)
    {
        case ExcCode_IBE:
        case ExcCode_DBE:
            signal = SPAL_GDB_SIGBUS;
            break;
        case ExcCode_RI:
            signal = SPAL_GDB_SIGILL;
            break;
        default:
            signal = SPAL_GDB_SIGSEGV;
            break;
    }
    spal_GdbSendcmd(spal_GdbRegs, signal, TRUE);

    spal_GdbLoop(v_regs, TRUE);
}



// =============================================================================
// spal_GdbHandle
// -----------------------------------------------------------------------------
// =============================================================================
VOID spal_GdbHandle(VOLATILE UINT32 * regs)
{
    //  Disable page_spy now, or it will SIGKILL us. *
//      * Re-enable on exit from the critical section.
    while (1)
    {
        switch(regs[SPAL_GDB_CMD_OFF])
        {
            case SPAL_GDB_CMD_FLUSH_CACHE:
                spal_InvalidateCache();
                regs[SPAL_GDB_CMD_OFF] = SPAL_GDB_CMD_DONE;
                spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT);
                break;

            case SPAL_GDB_SET_BREAKPOINT :
                spal_GdbSetBreakpoint(regs);
                regs[SPAL_GDB_CMD_OFF] = SPAL_GDB_CMD_DONE;
                spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT);
                break;

            case SPAL_GDB_UNSET_BREAKPOINT :
                spal_GdbUnsetBreakpoint(regs);
                regs[SPAL_GDB_CMD_OFF] = SPAL_GDB_CMD_DONE;
                spal_DbgGdbHstSendEvent(SPAL_GDB_BCPU_EVENT);
                break;

            case SPAL_GDB_CMD_STEP:
                spal_GdbStep(0, regs);
                return;

            case SPAL_GDB_CMD_CONTINUE:
                return ;
            default:
                break;
        }
    }
}

// =============================================================================
// spal_GdbSetBreakpoint
// -----------------------------------------------------------------------------
///  Manage the setting of a breakpoint when asked by the GDB server. This basi-
///  cally means to squash the current opcode by the break opcode. The opcode
///  is memorized by the GDB server for it to be restored.
// =============================================================================
PROTECTED VOID spal_GdbSetBreakpoint(VOLATILE UINT32* regs)
{
    UINT32 addr = regs[SPAL_GDB_CMDPARAM_OFF];

#ifdef SPAL_ROMULATOR
    BOOL    flashWriteEnable = 0;
    // Take special action if the instruction to change
    // is in flash or in
    if (addr & REG_CS0_BASE)
    {
        // This address is in flash
        flashWriteEnable = spal_EbcFlashWriteEnabled();
        spal_EbcFlashWriteEnable(TRUE);
    }
#endif // SPAL_ROMULATOR

#ifdef FPGA
    // Special Rom
    // Unlock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#else
    if ((addr & 0x0FF00000) == REG_INT_ROM_BASE)
    {
        // This instruction is in internal ROM
        // We need to copy the whole line to patch
        // it. The line being patched become writtable
        UINT32 wordIdx = 0;
        // Copy Rom line into patch memory
        for (wordIdx = 0 ; wordIdx<4; wordIdx++)
        {
            *((UINT32*)(KSEG0(REG_INT_SRAM_BASE)) + 4*SPAL_GDB_PATCH_NUMBER + wordIdx) =
                *((UINT32*)(KSEG0(((addr & MEM_BRIDGE_BLOCK_ADDR_MASK)| REG_INT_ROM_BASE))) + wordIdx);
        }

        // Enable patch
#if !defined(CHIP_DIE_8955)
        hwp_memBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_ENABLE
                | (addr & MEM_BRIDGE_BLOCK_ADDR_MASK);
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
        hwp_bbMemBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_ENABLE
                | (addr & MEM_BRIDGE_BLOCK_ADDR_MASK);
#endif
    }
#endif

    if(addr & 1) //MIPS16
    {
        *(UINT16*)(addr&~1) = SPAL_GDB_16_OPCODE_STEP;
    }
    else         //MIPS32
    {
        *(VOLATILE UINT32*)(addr) = SPAL_GDB_OPCODE_STEP;
    }

// Take special action if the instruction to change
// is in flash or in
#ifdef SPAL_ROMULATOR
    if (addr & REG_CS0_BASE)
    {
        // Reprotect flash
        // This address was in flash
        spal_EbcFlashWriteEnable(flashWriteEnable);
    }
#endif // SPAL_ROMULATOR

#ifdef FPGA
    // Special Rom
    // Lock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#else
    // We need to keep the patch enabled until the breakpoint is removed.
#endif
}

// =============================================================================
// spal_GdbUnsetBreakpoint
// -----------------------------------------------------------------------------
///  Manage the unsetting of a breakpoint when asked by the GDB server. This basi-
///  cally means to squash the current break opcode by the true opcode. The opcode
///  was memorized by the GDB server for it to be restored.
// =============================================================================
PROTECTED VOID spal_GdbUnsetBreakpoint(VOLATILE UINT32* regs)
{
    UINT32 addr         = regs[SPAL_GDB_CMDPARAM_OFF];
    // The opcode to be restored (memorized by the GDB server).
    UINT32 theOpCode    = regs[SPAL_GDB_CMDPARAM2_OFF]; //Because it's the one

#ifdef SPAL_ROMULATOR
    BOOL    flashWriteEnable = 0;
    // Take special action if the instruction to change
    // is in flash or in
    if (addr & REG_CS0_BASE)
    {
        // This address is in flash
        flashWriteEnable = spal_EbcFlashWriteEnabled();
        spal_EbcFlashWriteEnable(TRUE);
    }
#endif // SPAL_ROMULATOR

#ifdef FPGA
    // Special Rom
    // Unlock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#else
    // We only use one patch page to handle GDB in ROM
    // code. So we can safely assume that the patch
    // was enabled to set the breakpoint we are to remove.
#endif

    if(addr & 1) //MIPS16
    {
        *(UINT16*)(addr&~1) = (UINT16)theOpCode;
    }
    else         //MIPS32
    {
        *(UINT32*)(addr) = theOpCode;
    }

// Take special action if the instruction to change
// is in flash or in
#ifdef SPAL_ROMULATOR
    if (addr & REG_CS0_BASE)
    {
        // Reprotect flash
        // This address was in flash
        spal_EbcFlashWriteEnable(flashWriteEnable);
    }
#endif // SPAL_ROMULATOR

#ifdef FPGA
    // Special Rom
    // Lock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#else
    // Disabling the patch would restore the opcode again.
    // It would also remove any step in progress, that
    // would anyhow have changed the patched area, restoring
    // de facto the code before the set breakpoint.
#endif
}




// =============================================================================
// spal_Gdb16Step
// -----------------------------------------------------------------------------
///  Uses a TRAP to generate an exception after we run the next instruction.
// =============================================================================
PROTECTED UINT32 spal_Gdb16Step (UINT32 nAddr, VOLATILE UINT32 * spal_GdbRegs)
{
    UINT32     nNewAddr;
    UINT16     *nOp=(UINT16*)MASK_ONE(spal_GdbRegs[37]);

    if     (SPAL_GDB_16_OPCODE_JALX(nOp))     nNewAddr=SPAL_GDB_16_OPADDR_JALX(nOp);
    else if(SPAL_GDB_16_OPCODE_JAL(nOp))      nNewAddr=SPAL_GDB_16_OPADDR_JAL(nOp);
    else if(SPAL_GDB_16_OPCODE_JALR(nOp))     nNewAddr=SPAL_GDB_16_OPADDR_JALR(nOp);
    else if(SPAL_GDB_16_OPCODE_JR(nOp))       nNewAddr=SPAL_GDB_16_OPADDR_JR(nOp);
    else if(SPAL_GDB_16_OPCODE_B(nOp))        nNewAddr=SPAL_GDB_16_OPADDR_B(nOp);
    else if(SPAL_GDB_16_OPCODE_BEQZ(nOp))     nNewAddr=SPAL_GDB_16_OPADDR_BEQZ(nOp);
    else if(SPAL_GDB_16_OPCODE_BNEZ(nOp))     nNewAddr=SPAL_GDB_16_OPADDR_BNEZ(nOp);
    else if(SPAL_GDB_16_OPCODE_BTEQZ(nOp))    nNewAddr=SPAL_GDB_16_OPADDR_BTEQZ(nOp);
    else if(SPAL_GDB_16_OPCODE_BTNEZ(nOp))    nNewAddr=SPAL_GDB_16_OPADDR_BTNEZ(nOp);
    else if(SPAL_GDB_16_OPCODE_EXTENDED(nOp))
    {
        if     (SPAL_GDB_16_OPCODE_B_EXT(nOp))        nNewAddr=SPAL_GDB_16_OPADDR_B_EXT(nOp);
        else if(SPAL_GDB_16_OPCODE_BEQZ_EXT(nOp))
        {
            nNewAddr=SPAL_GDB_16_OPADDR_BEQZ_EXT(nOp);
        }
        else if(SPAL_GDB_16_OPCODE_BNEZ_EXT(nOp))     nNewAddr=SPAL_GDB_16_OPADDR_BNEZ_EXT(nOp);
        else if(SPAL_GDB_16_OPCODE_BTEQZ_EXT(nOp))    nNewAddr=SPAL_GDB_16_OPADDR_BTEQZ_EXT(nOp);
        else if(SPAL_GDB_16_OPCODE_BTNEZ_EXT(nOp))    nNewAddr=SPAL_GDB_16_OPADDR_BTNEZ_EXT(nOp);
//- V: Uh, what are we doing in there ?
//- J, M & B: Don't know but add 4 instead of 2 because the instruction is extended to four bytes.
        else                                        nNewAddr=spal_GdbRegs[37]+4;
    }
    else
    {
        nNewAddr=spal_GdbRegs[37]+2;
    }


    if(spal_GdbIsAddressStepForbidden(nNewAddr))
    {
        if(SPAL_GDB_16_OPCODE_JAL(nOp)
                || SPAL_GDB_16_OPCODE_JALX(nOp))     // Bypass the step-into function (think about the delay slot).
        {
            nNewAddr = spal_GdbRegs[37]+6;
        }
        else if(SPAL_GDB_16_OPCODE_EXTENDED(nOp) // Bypass the step-to branch (extended, no delay slot).
                || SPAL_GDB_16_OPCODE_JALR(nOp)      // (think about the delay slot). And the kittens.
                || SPAL_GDB_16_OPCODE_JR(nOp))       // (think about the delay slot). And the kittens.
        {
            nNewAddr = spal_GdbRegs[37]+4;
        }
        else                                    // Should never happen.
        {
            nNewAddr = spal_GdbRegs[37]+2;
        }
    }

    return nNewAddr;
}


// =============================================================================
// spal_Gdb32Step
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED UINT32 spal_Gdb32Step (UINT32 nAddr, VOLATILE UINT32 * spal_GdbRegs)
{
    UINT32     nNewAddr;
    UINT32     nOp=*(UINT32*)spal_GdbRegs[37];

    if     (SPAL_GDB_OPCODE_J(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_J(nOp);
    }
    else if(SPAL_GDB_OPCODE_JAL(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_JAL(nOp);
    }
    else if(SPAL_GDB_OPCODE_JALX(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_JALX(nOp);
    }
    else if(SPAL_GDB_OPCODE_JR(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_JR(nOp);
    }
    else if(SPAL_GDB_OPCODE_JALR(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_JALR(nOp);
    }
    else if(SPAL_GDB_OPCODE_BEQ(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BEQ(nOp);
    }
    else if(SPAL_GDB_OPCODE_BNE(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BNE(nOp);
    }
    else if(SPAL_GDB_OPCODE_BGEZ(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BGEZ(nOp);
    }
    else if(SPAL_GDB_OPCODE_BGEZAL(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BGEZAL(nOp);
    }
    else if(SPAL_GDB_OPCODE_BGTZ(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BGTZ(nOp);
    }
    else if(SPAL_GDB_OPCODE_BLEZ(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BLEZ(nOp);
    }
    else if(SPAL_GDB_OPCODE_BLTZ(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BLTZ(nOp);
    }
    else if(SPAL_GDB_OPCODE_BLTZAL(nOp))
    {
        nNewAddr=SPAL_GDB_OPADDR_BLTZAL(nOp);
    }
    else
    {
        nNewAddr=spal_GdbRegs[37]+4;
    }

    if(spal_GdbIsAddressStepForbidden(nNewAddr))
    {
        if(TRUE)
        {
            nNewAddr = spal_GdbRegs[37]+8;
        }
        else    //This should not happened, 'cause we should always be delay-sloted in such cases.
        {
            nNewAddr = spal_GdbRegs[37]+4;
        }

    }

    return nNewAddr;
}

// =============================================================================
// spal_Gdb16EndStep
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_Gdb16EndStep (UINT32 nAddr)
{
    // Flag to prevent mips16 opcode to look like 0
    // and be discared improperly by spal_GdbUndoStep, which
    // test opCode against 0 to check if there is a step to
    // undo.
    //  save the target opcode, replace with SPAL_GDB_16_OPCODE_STEP
    spal_GdbGData.opCode = *(UINT16*)MASK_ONE(nAddr) | SPAL_GDB_MIPS16_OPCODE_FLAG;
    spal_GdbGData.stepAddr = nAddr;
    *(UINT16*)MASK_ONE(nAddr) = SPAL_GDB_16_OPCODE_STEP;
}

// =============================================================================
// spal_Gdb32EndStep
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_Gdb32EndStep (UINT32 nAddr)
{
    while(*(UINT32*)nAddr == 0)
    {
        nAddr+=4;
    }

//  save the target opcode, replace with SPAL_GDB_OPCODE_STEP
    spal_GdbGData.opCode = *(UINT32*)nAddr;
    spal_GdbGData.stepAddr = nAddr;
    *(UINT32*)nAddr = SPAL_GDB_OPCODE_STEP;
}

// =============================================================================
// spal_GdbStep
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_GdbStep (UINT32 nAddr, VOLATILE UINT32 * spal_GdbRegs)
{
    UINT32 nNewAddr;
#ifdef SPAL_ROMULATOR
    BOOL    flashWriteEnable = 0;
#endif
    if (spal_GdbRegs[37] & 0x01)
        nNewAddr=spal_Gdb16Step(nAddr, spal_GdbRegs);
    else
        nNewAddr=spal_Gdb32Step(nAddr, spal_GdbRegs);

    // Take special action if the instruction to change
    // is in flash or in Rom
#ifdef SPAL_ROMULATOR

    if (nNewAddr & REG_CS0_BASE)
    {
        // This address is in flash
        flashWriteEnable = spal_EbcFlashWriteEnabled();
        spal_EbcFlashWriteEnable(TRUE);
    }
#endif // SPAL_ROMULATOR
#ifdef FPGA
    // Special Rom
    // Unlock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#else
    if ((nNewAddr & 0x0FF00000) == REG_INT_ROM_BASE)
    {
        // This instruction is in internal ROM
        // We need to copy the whole line to patch
        // it. The line being patched become writtable
        UINT32 wordIdx = 0;

        // Copy Rom line into patch memory
        for (wordIdx = 0 ; wordIdx<4; wordIdx++)
        {
            *((UINT32*)(KSEG0(REG_INT_SRAM_BASE)) + 4*SPAL_GDB_PATCH_NUMBER + wordIdx) =
                *((UINT32*)(KSEG0(((nNewAddr & MEM_BRIDGE_BLOCK_ADDR_MASK)| REG_INT_ROM_BASE))) + wordIdx);
        }

        // Enable patch
#if !defined(CHIP_DIE_8955)
        hwp_memBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_ENABLE
                | (nNewAddr & MEM_BRIDGE_BLOCK_ADDR_MASK);
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
        hwp_bbMemBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_ENABLE
                | (nNewAddr & MEM_BRIDGE_BLOCK_ADDR_MASK);
#endif
    }
#endif

    if (nNewAddr & 0x01)
    {
        spal_Gdb16EndStep(nNewAddr);
    }
    else
    {
        spal_Gdb32EndStep(nNewAddr);
    }
    spal_InvalidateCache();

#ifdef SPAL_ROMULATOR
    // Reprotect flash
    if (nNewAddr & REG_CS0_BASE)
    {
        // This address was in flash
        spal_EbcFlashWriteEnable(flashWriteEnable);
    }
#endif // SPAL_ROMULATOR
#ifdef FPGA
    // Special Rom
    // Lock
#if !defined(CHIP_DIE_8955)
    hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#endif

}

// =============================================================================
// spal_GdbUndoStep
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID spal_GdbUndoStep (VOLATILE UINT32 * spal_GdbRegs)
{
    UINT32     nNewAddr = spal_GdbGData.stepAddr;
    UINT32     isMips16 = (spal_GdbRegs[37] & 0x01);


#ifdef SPAL_ROMULATOR
    BOOL    flashWriteEnable = 0;
#endif

//  if we're stepping, then undo the step
    if (spal_GdbGData.opCode )
    {

#ifdef SPAL_ROMULATOR
        if (nNewAddr & REG_CS0_BASE)
        {
            // This address is in flash
            flashWriteEnable = spal_EbcFlashWriteEnabled();
            spal_EbcFlashWriteEnable(TRUE);
        }
#endif // SPAL_ROMULATOR
#ifdef FPGA
        // Special Rom
        // Unlock
#if !defined(CHIP_DIE_8955)
        hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
        hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_ENABLE;
#endif
#else
        // The corresponding instruction is patchable since
        // the previous passage through spal_GdbStep.
#endif

        if(nNewAddr == spal_GdbRegs[37])
        {
            if(!isMips16)
                *(UINT32*)spal_GdbRegs[37] = spal_GdbGData.opCode;
            else
                *(UINT16*)MASK_ONE(spal_GdbRegs[37]) = (UINT16)spal_GdbGData.opCode  & SPAL_GDB_MIPS16_OPCODE_MASK;
        }
        else
        {
            if(!isMips16)
                *(UINT32*)nNewAddr=spal_GdbGData.opCode;
            else
                *(UINT16*)MASK_ONE(nNewAddr)=(UINT16)spal_GdbGData.opCode & SPAL_GDB_MIPS16_OPCODE_MASK;
        }

        spal_InvalidateCache();
// Take special action if the instruction to change
// is in flash or in Rom
#ifdef SPAL_ROMULATOR
        if (nNewAddr & REG_CS0_BASE)
        {
            // Reprotect flash
            // This address was in flash
            spal_EbcFlashWriteEnable(flashWriteEnable);
        }
#endif // SPAL_ROMULATOR
#ifdef FPGA
        // Special Rom
        // Lock
#if !defined(CHIP_DIE_8955)
        hwp_memBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
        hwp_bbMemBridge->Rom_Patch[0] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#else
        // If the nNewAddr is in Rom, disabling the patch is equivalent to
        // the previous behaviour
        if ((nNewAddr & 0x0FF00000) == REG_INT_ROM_BASE)
        {
            // The patch should be enabled
            // Disable the patch set in spal_GdbStep.
#if !defined(CHIP_DIE_8955)
            hwp_memBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_DISABLE;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
            hwp_bbMemBridge->Rom_Patch[SPAL_GDB_PATCH_NUMBER] = MEM_BRIDGE_PATCH_DISABLE;
#endif
        }
#endif

        spal_GdbGData.opCode = 0;
    }
}


// =============================================================================
// spal_GdbStrLen
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 spal_GdbStrLen(UINT8* pcStr)
{
    UINT32 i=0;
    while((*pcStr++) != '\0') i++;
    return i;
}



