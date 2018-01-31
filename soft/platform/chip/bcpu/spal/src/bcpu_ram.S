#include "regdef.h"
#include "cp0.h"

//#################################################################################
// Global variables in RAM
//#################################################################################
.data
.section BB_SRAM_GLOBALS
// Bcpu variables
.globl bcpu_error_code
.globl bcpu_error_status
.globl bcpu_sp_context
.globl bcpu_stack_base
.globl bcpu_main_entry


bcpu_error_code:    .word 0xc0010000
bcpu_error_status:  .word 0xc001c001
bcpu_sp_context:    .word 0             // FIXME used to be __bb_gdb_context
                                        // Set at this value by the hal_SysStartBcpu function
bcpu_stack_base:    .word 0xbad0c001
bcpu_main_entry:    .word 0x0           // location of main bcpu program 
                                        // now passed as a hal_SysStartBcpu parameter

