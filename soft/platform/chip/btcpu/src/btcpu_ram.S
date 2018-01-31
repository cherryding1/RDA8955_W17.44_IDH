#include "regdef.h"
#include "cp0.h"

//#################################################################################
// Global variables in RAM
//#################################################################################
.data
// Btcpu variables
.globl btcpu_error_code
.globl btcpu_error_status
.globl btcpu_sp_context
.globl btcpu_stack_base
.globl btcpu_main_entry
.globl btcpu_int_entry


btcpu_error_code:     .word 0xc0020000
btcpu_error_status:   .word 0xc002c002
btcpu_sp_context:     .word 0xdeb9deb9

// Set the following values by the hal_SysStartBtcpu function
btcpu_stack_base:     .word 0xbad0c002
btcpu_main_entry:     .word 0x0         // location of main btcpu program 
btcpu_int_entry:      .word 0x0         // location of btcpu irq handler


