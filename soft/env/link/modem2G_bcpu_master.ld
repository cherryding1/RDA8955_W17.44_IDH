/* ======================================================================= */
/*                  TARGET CONFIGURATION SECTION                           */
/* ======================================================================= */
/* modem2g_bcpu_master.ld */
/* The following values are defined in ff_target.def and exported by the Makefile */
/*  FLASH_SIZE              */
/*  RAM_SIZE                */
/*  CALIB_BASE              */
/*  FACT_SETTINGS_BASE      */
/*  CODE_BASE               */
/*  USER_DATA_BASE          */
/*  USER_DATA_SIZE          */
/* ======================================================================= */
/*                  END CONFIGURATION SECTION                              */
/* ======================================================================= */
/* The linker script depends on the following defines */
/*  - CONST_IN_RAM : all constants are placed in RAM, this is useful for ram_run */
/*  - CODE_IN_RAM : All code is executed from RAM, this is useful for ram_run or code running from RAM on romulator */
/*  - PRELOADED_INT_SRAM : The internal SRAM are already preloaded, useful for ram_run */
/*  - PRELOADED_RAM : The external RAM is already preloaded, useful for ram_run or simu */
/*  - __NATIVE_JAVA__ :  The Native Java need  reload it's data and bss section every time when app start*/
/* Those are constants of the system */


/* ROM content selection: things to include
 * WARNING: the DISCARD section uses the opposite -->
 * do all inclusion before discard !*/


/* Take into account assert bug in binutils-2.15*/
#if (USE_BINUTILS_2_19 == 1)
#define LD_ASSERT(condition, small, full)   ASSERT(condition, full)
#else
#define LD_ASSERT(condition, small, full)   ASSERT(condition, small)
#endif

#define MEM_CACHED_UNCACHED_OFFSET 0x20000000


/*Mandatory to have the BB side irq handler linked */
#ifdef FPGA_NBCPU
EXTERN (nbcpu_main)
#ifdef BUILD_BCPU_PSRAM
EXTERN (g_spcMapAccess)
#endif
#else
EXTERN (bcpu_main)
EXTERN (g_spcMapAccess)
#endif

EXTERN (g_sppMapAccess)

SECTIONS
{
    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */

    . = EXT_RAM_BASE;

#ifdef SPI_REG_DEBUG
    #define LINK_BCPU_IMAGE
    #include "spi_reg_debug.ld"
#endif

    . = ALIGN(0x100);

    _bcpu_ram_available_base = .;

    /* ============================== */
    /* Main BCPU entry point */
    /* ============================== */

#ifndef RAMRUN
    . = BCPU_CODE_BASE;
#else
    /* We do NOT consider CODE_IN_RAM and CONST_IN_RAM macros for BCPU RAMRUN.
     * All the codes and constants of BCPU RAMRUN will be put in ram space */
    . = BCPU_RAMRUN_CODE_BASE;
#endif

    _bcpu_flash_start = .;

    _bcpu_flash_rotext_start = .;

    .bcpu_rotext . : AT (ADDR(.bcpu_rotext))
    {
#ifndef FPGA_NBCPU
        *spc_main.o(.bcpu_main_entry)
        *(.text .text.*)
#endif
        . = ALIGN(0x10);
    }

    _bcpu_flash_rotext_end = .;

    _bcpu_flash_rodata_start = .;

    .bcpu_rodata . : AT (ADDR(.bcpu_rodata))
    {
        *(.rodata.g_spcMapAccess)
        . = ALIGN(0x10);
    }

    _bcpu_flash_rodata_end = .;


    _bcpu_rom_flash_start_location = .;

#if defined(LOAD_ROM_ELF)
    /* ======================================== */
    /* BCPU rom in flash elf */
    /* ======================================== */
    #include "link_rom_elf.ld"
#endif

#if (defined(REWRITE_ROM_AT_BOOT) && defined(FPGA))
    _bcpu_rom_flash_end_location = .;
#else
    _bcpu_rom_flash_end_location = _bcpu_rom_flash_start_location;
#endif

    /* ================================================= */
    /* in BB SRAM                                        */
    /* ================================================= */
    _bb_sram_flash_start_location = _bcpu_rom_flash_end_location;

    . = BB_SRAM_BASE;
    _bb_sram_start = .;

#if defined(LOAD_ROM_ELF)

    _bb_sram_cached_size = 0;
    _bb_sram_uncached_size = 0;

#else /* !LOAD_ROM_ELF */

    /*------------------ */
    /* BB SRAM CACHED    */
    /*------------------ */
#ifndef PRELOADED_INT_SRAM
    .bbsram . : AT (_bb_sram_flash_start_location)
#else
    .bbsram . : AT (ADDR(.bbsram))
#endif
    {
        _bb_sram_cached_start = .;
        *(BB_ITLV_SECTION)
        . = ALIGN(0x4);
/*     _bb_sram_itlv_buf_end = .;*/
        *(BB_SPC_CTX_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_cached_end = .;
    _bb_sram_cached_size = _bb_sram_cached_end - _bb_sram_cached_start;

    /*------------------ */
    /* BB SRAM UNCACHED  */
    /*------------------ */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

#ifndef PRELOADED_INT_SRAM
    .bbsramu . : AT (_bb_sram_flash_start_location + _bb_sram_cached_size)
#else
    .bbsramu . : AT (ADDR(.bbsramu))
#endif
    {
        _bb_sram_uncached_start = .;
        . = ALIGN(0x10);
        _mbbsramu = .;
        *(BB_STATIC_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_uncached_end = .;
    _bb_sram_uncached_size = _bb_sram_uncached_end - _bb_sram_uncached_start;

#endif /* !LOAD_ROM_ELF */

    /* Read the symbol address from BCPU ROM */
    . = _bb_sram_romed_globals_end;

    .bbsram_globals_not_romed . (NOLOAD) : AT (ADDR(.bbsram_globals_not_romed))
    {
        _bb_sram_not_romed_globals_start = .;
        *(BB_SRAM_GLOBALS)
        *(.bbsram_globals)
        _bb_sram_not_romed_globals_end = .;
    }

    LD_ASSERT(ABSOLUTE(BB_SRAM_BASE + BB_SRAM_SIZE - RESERVED_PATCH_SIZE)
                        >= ABSOLUTE(_bb_sram_not_romed_globals_end),
                        "BBGS", "BBSRAM not romed globals overflow out of BBSRAM.")

    _bb_sram_var_size = _bb_sram_cached_size + _bb_sram_uncached_size;
    _bb_sram_var_end = _bb_sram_start + _bb_sram_var_size;

    /*----------------------- */
    /* BB SRAM Patch           */
    /*----------------------- */

#ifndef PRELOADED_INT_SRAM
    _bb_sram_patch_flash_start = _bb_sram_flash_start_location + _bb_sram_var_size;
#else
    _bb_sram_patch_flash_start = _bb_sram_flash_start_location;
#endif

    . = BB_SRAM_BASE + BB_SRAM_SIZE - RESERVED_PATCH_SIZE;
    _bb_sram_patch_start = .;

    .bbsram_patch_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start)
#else
        AT (ADDR(.bbsram_patch_text))
#endif
    {
        _bb_sram_patch_text_start = .;
        *(.bbsram_patch_text)
        *(.bbsram_patch_rodata)
#ifndef FPGA_NBCPU
        *(.rdata .rodata .rodata.str1.4 .rodata.* .rodata1)
#endif
        . = ALIGN(0x10);
        _bb_sram_patch_text_end = .;
    }
    _bb_sram_patch_text_size = _bb_sram_patch_text_end - _bb_sram_patch_text_start;

    .bbsram_patch_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start + _bb_sram_patch_text_size)
#else
        AT (ADDR(.bbsram_patch_data))
#endif
    {
        _bb_sram_patch_data_start = .;
        *(.bbsram_patch_data)
        . = ALIGN(0x10);
        _bb_sram_patch_data_end = .;
    }
    _bb_sram_patch_data_size = _bb_sram_patch_data_end - _bb_sram_patch_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start + _bb_sram_patch_text_size + _bb_sram_patch_data_size)
#else
        AT (ADDR(.bbsram_patch_ucdata))
#endif
    {
        _bb_sram_patch_ucdata_start = .;
        *(.bbsram_patch_ucdata)
        . = ALIGN(0x10);
        _bb_sram_patch_ucdata_end = .;
    }
    _bb_sram_patch_ucdata_size = _bb_sram_patch_ucdata_end - _bb_sram_patch_ucdata_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bb_sram_patch_end = .;

    _bb_sram_patch_size = _bb_sram_patch_end - _bb_sram_patch_start;

#ifndef PRELOADED_INT_SRAM
    _bb_sram_patch_flash_end = _bb_sram_patch_flash_start + _bb_sram_patch_size;
#else
    _bb_sram_patch_flash_end = _bb_sram_patch_flash_start;
#endif

    _bb_sram_flash_end_location = _bb_sram_patch_flash_end;

    /*----------------------- */
    /* BB SRAM Patch BSS           */
    /*----------------------- */
    _bb_sram_patch_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_ucbss . (NOLOAD) : AT (ADDR(.bbsram_patch_ucbss))
    {
        *(.bbsram_patch_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_bss . (NOLOAD) : AT (ADDR(.bbsram_patch_bss))
    {
        *(.bbsram_patch_bss)
        . = ALIGN(0x10);
    }

    _bb_sram_patch_bss_end = .;
    _bb_sram_patch_bss_size = _bb_sram_patch_bss_end - _bb_sram_patch_bss_start;

    LD_ASSERT(ABSOLUTE(BB_SRAM_BASE + BB_SRAM_SIZE) >= ABSOLUTE(_bb_sram_patch_bss_end),
                        "BBPC", "BBSRAM patch code overflow out of BBSRAM.")

#ifdef FPGA_NBCPU
    /*----------------------- */
    /* NB SRAM Patch           */
    /*----------------------- */

#ifndef PRELOADED_INT_SRAM
    _nb_sram_patch_flash_start = _bb_sram_flash_end_location;
#else
    _nb_sram_patch_flash_start = _bb_sram_flash_start_location;
#endif

    . = NB_SRAM_BASE + NB_RESERVED_PATCH_SIZE;
    
    _nb_sram_patch_start = .;

    .nbsram_patch_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_nb_sram_patch_flash_start)
#else
        AT (ADDR(.nbsram_patch_text))
#endif
    {
        _nb_sram_patch_text_start = .;
        *(.nbsram_patch_text)
        *(.nbsram_patch_rodata)
#ifndef BUILD_BCPU_PSRAM
        *spc_nb_main.o(.bcpu_main_entry)
        *(.text .text.*)
        *(.rdata .rodata .rodata.str1.4 .rodata.* .rodata1)
#endif
        . = ALIGN(0x10);
        _nb_sram_patch_text_end = .;
    }
    _nb_sram_patch_text_size = _nb_sram_patch_text_end - _nb_sram_patch_text_start;

    .nbsram_patch_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_nb_sram_patch_flash_start + _nb_sram_patch_text_size)
#else
        AT (ADDR(.nbsram_patch_data))
#endif
    {
        _nb_sram_patch_data_start = .;
        *(.nbsram_patch_data)
        . = ALIGN(0x10);
        _nb_sram_patch_data_end = .;
    }
    _nb_sram_patch_data_size = _nb_sram_patch_data_end - _nb_sram_patch_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .nbsram_patch_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_nb_sram_patch_flash_start + _nb_sram_patch_text_size + _nb_sram_patch_data_size)
#else
        AT (ADDR(.nbsram_patch_ucdata))
#endif
    {
        _nb_sram_patch_ucdata_start = .;
        *(.nbsram_patch_ucdata)
        . = ALIGN(0x10);
        _nb_sram_patch_ucdata_end = .;
    }
    _nb_sram_patch_ucdata_size = _nb_sram_patch_ucdata_end - _nb_sram_patch_ucdata_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _nb_sram_patch_end = .;

    _nb_sram_patch_size = _nb_sram_patch_end - _nb_sram_patch_start;

#ifndef PRELOADED_INT_SRAM
    _nb_sram_patch_flash_end = _nb_sram_patch_flash_start + _nb_sram_patch_size;
#else
    _nb_sram_patch_flash_end = _nb_sram_patch_flash_start;
#endif

    _nb_sram_flash_end_location = _nb_sram_patch_flash_end;

    /*----------------------- */
    /* NB SRAM Patch BSS           */
    /*----------------------- */
    _nb_sram_patch_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .nbsram_patch_ucbss . (NOLOAD) : AT (ADDR(.nbsram_patch_ucbss))
    {
        *(.nbsram_patch_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .nbsram_patch_bss . (NOLOAD) : AT (ADDR(.nbsram_patch_bss))
    {
        *(.nbsram_patch_bss)
        . = ALIGN(0x10);
    }

    _nb_sram_patch_bss_end = .;
    _nb_sram_patch_bss_size = _nb_sram_patch_bss_end - _nb_sram_patch_bss_start;

    LD_ASSERT(ABSOLUTE(NB_SRAM_BASE + NB_RESERVED_PATCH_SIZE + NB_SRAM_PATCH_SIZE) >= ABSOLUTE(_nb_sram_patch_bss_end),
                        "NBPC", "NBSRAM patch code overflow out of NBSRAM.")


    /* Read the symbol address from BCPU ROM */
    . = _nb_sram_romed_globals_end;
    
    _nb_sram_not_romed_start = .;

    .nbsram_globals_not_romed . (NOLOAD) : AT(ADDR(.nbsram_globals_not_romed))
    {
        _nb_sram_not_romed_globals_start = .;
        *(NB_SRAM_GLOBALS)
        *(.nbsram_globals)
        _nb_sram_not_romed_globals_end = .;
    }
    _nb_sram_not_romed_end = .;
    _nb_sram_not_romed_size = _nb_sram_not_romed_end - _nb_sram_not_romed_start;
    LD_ASSERT(ABSOLUTE(_nb_sram_not_romed_end) <= ABSOLUTE(NB_SRAM_BASE + NB_SRAM_SIZE), "NBGO", "NBSRAM not romed Globals Overflozed !!")
#endif
    
    /* ===================================================== */
    /* in BB DUALPORT SRAM                                           */
    /* ===================================================== */
#ifdef FPGA_NBCPU
    _bcpu_dualport_sram_flash_start_location = _nb_sram_flash_end_location;
#else
    _bcpu_dualport_sram_flash_start_location = _bb_sram_flash_end_location;
#endif
    . = BB_DUALPORT_SRAM_BASE;

    _bcpu_dualport_sram_start = .;

    .bcpu_dualport_sram_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_dualport_sram_flash_start_location)
#else
        AT (ADDR(.bcpu_dualport_sram_text))
#endif
    {
        _bcpu_dualport_sram_text_start = .;
        *(.dualport_sram_text)
        *(.dualport_sram_rodata)
        . = ALIGN(0x10);
        _bcpu_dualport_sram_text_end = .;
    }
    _bcpu_dualport_sram_text_size = _bcpu_dualport_sram_text_end - _bcpu_dualport_sram_text_start;

    .bcpu_dualport_sram_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_dualport_sram_flash_start_location + _bcpu_dualport_sram_text_size)
#else
        AT (ADDR(.bcpu_dualport_sram_data))
#endif
    {
        _bcpu_dualport_sram_cached_data_start = .;
        *(.dualport_sram_data)
        . = ALIGN(0x10);
        _bcpu_dualport_sram_cached_data_end = .;
    }
    _bcpu_dualport_sram_cached_data_size = _bcpu_dualport_sram_cached_data_end - _bcpu_dualport_sram_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_dualport_sram_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_dualport_sram_flash_start_location + _bcpu_dualport_sram_text_size + _bcpu_dualport_sram_cached_data_size)
#else
        AT (ADDR(.bcpu_dualport_sram_ucdata))
#endif
    {
        _bcpu_dualport_sram_uncached_data_start = .;
        *(.dualport_sram_ucdata)
        . = ALIGN(0x10);
        _bcpu_dualport_sram_uncached_data_end = .;
    }
    _bcpu_dualport_sram_uncached_data_size = _bcpu_dualport_sram_uncached_data_end - _bcpu_dualport_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_dualport_sram_end = .;
    _bcpu_dualport_sram_size = _bcpu_dualport_sram_end - _bcpu_dualport_sram_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_dualport_sram_flash_end_location = _bcpu_dualport_sram_flash_start_location + _bcpu_dualport_sram_size;
#else
    _bcpu_dualport_sram_flash_end_location = _bcpu_dualport_sram_flash_start_location;
#endif

    _bcpu_dualport_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_dualport_sram_ucbss . (NOLOAD) : AT (ADDR(.bcpu_dualport_sram_ucbss))
    {
        *(.dualport_sram_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_dualport_sram_bss . (NOLOAD) : AT (ADDR(.bcpu_dualport_sram_bss))
    {
        *(.dualport_sram_bss)
        . = ALIGN(0x10);
    }

    _bcpu_dualport_bss_end = .;
    _bcpu_dualport_bss_size = _bcpu_dualport_bss_end - _bcpu_dualport_bss_start;

    LD_ASSERT(ABSOLUTE(BB_DUALPORT_SRAM_BASE + BB_DUALPORT_SRAM_SIZE - RESERVED_MBX_SIZE)
                        >= ABSOLUTE(_bcpu_dualport_bss_end), "BDPO", "BCPU DUALPORT SRAM overflow.")

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */
    _bcpu_sys_sram_flash_start_location = _bcpu_dualport_sram_flash_end_location;

    . = BCPU_SYS_SRAM_BASE;
    _bcpu_sys_sram_start = .;

#ifndef PRELOADED_INT_SRAM
    .bcpu_sramtext . : AT (_bcpu_sys_sram_flash_start_location)
#else
    .bcpu_sramtext . : AT (ADDR(.bcpu_sramtext))
#endif
    {
        _bcpu_sys_sram_txt_start = .;
        *(.sramtext .sramrodata)
        . = ALIGN(0x10);
    }
    _bcpu_sys_sram_txt_end = .;
    _bcpu_sys_sram_txt_size = _bcpu_sys_sram_txt_end - _bcpu_sys_sram_start;

#ifndef PRELOADED_INT_SRAM
    .bcpu_sramdata _bcpu_sys_sram_txt_end : AT (_bcpu_sys_sram_flash_start_location + _bcpu_sys_sram_txt_size )
#else
    .bcpu_sramdata _bcpu_sys_sram_txt_end : AT (ADDR(.bcpu_sramdata))
#endif
    {
        _bcpu_sys_sram_cached_start = .;
        *(.sramdata)
        . = ALIGN(0x10);
    }
    _bcpu_sys_sram_cached_end = .;
    _bcpu_sys_sram_cached_size = _bcpu_sys_sram_cached_end - _bcpu_sys_sram_cached_start;

    /*-------------------- */
    /* SYS SRAM UNCACHED   */
    /*-------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

#ifndef PRELOADED_INT_SRAM
    .bcpu_sramucdata . : AT (_bcpu_sys_sram_flash_start_location + _bcpu_sys_sram_txt_size + _bcpu_sys_sram_cached_size)
#else
    .bcpu_sramucdata . : AT (ADDR(.bcpu_sramucdata))
#endif
    {
        _bcpu_sys_sram_uncached_start = .;
        *(.sramucdata)
        . = ALIGN(0x10);
    }
    _bcpu_sys_sram_uncached_end = .;
    _bcpu_sys_sram_uncached_size = _bcpu_sys_sram_uncached_end - _bcpu_sys_sram_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_sys_sram_end = .;
    _bcpu_sys_sram_size = _bcpu_sys_sram_end - _bcpu_sys_sram_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_sys_sram_flash_end_location = _bcpu_sys_sram_flash_start_location + _bcpu_sys_sram_size;
#else
    _bcpu_sys_sram_flash_end_location = _bcpu_sys_sram_flash_start_location;
#endif

    /*----------------------- */
    /* SYS SRAM BSS           */
    /*----------------------- */
    _bcpu_sys_sram_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_sramucbss . (NOLOAD) : AT (ADDR(.bcpu_sramucbss))
    {
        *(.sramucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_srambss . (NOLOAD) : AT (ADDR(.bcpu_srambss))
    {
        *(.srambss)
        . = ALIGN(0x10);
    }

    _bcpu_sys_sram_bss_end = .;
    _bcpu_sys_sram_bss_size = _bcpu_sys_sram_bss_end - _bcpu_sys_sram_bss_start;

    _bcpu_sys_sram_end = _bcpu_sys_sram_bss_end; 

    /* Check that the INT SRAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(SYS_SRAM_END - SYS_SRAM_IRQ_STACKWATCH_SIZE - XCPU_IRQ_STACK_SIZE)
                        > ABSOLUTE(_bcpu_sys_sram_bss_end), "BINT", "BCPU Internal SRAM overflow !")

    /*---------------- */
    /* End of SYS SRAM */
    /*---------------- */
    
 #if (CHIP_HAS_TMCU==1)
    /* ===================================================== */
    /* in TUMCU DM                                           */
    /* ===================================================== */
    _bcpu_tmcudm_ram_flash_start_location = _bcpu_sys_sram_flash_end_location;

	  . = TMCU_IFCBUF_ADDR;
 
    _bcpu_tmcudm_sram_start = .;


    .bcpu_tmcudm_sram_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_tmcudm_ram_flash_start_location)
#else
        AT (ADDR(.bcpu_tmcudm_sram_data))
#endif
    {
        _bcpu_tmcudm_sram_cached_data_start = .;
        *(.dm_sram_data)
        . = ALIGN(0x10);
        _bcpu_tmcudm_sram_cached_data_end = .;
    }
    _bcpu_tmcudm_sram_cached_data_size = _bcpu_tmcudm_sram_cached_data_end - _bcpu_tmcudm_sram_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_tmcudm_sram_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_tmcudm_ram_flash_start_location + _bcpu_tmcudm_sram_cached_data_size)
#else
        AT (ADDR(.bcpu_tmcudm_sram_ucdata))
#endif
    {
        _bcpu_tmcudm_sram_uncached_data_start = .;
        *(.dm_sram_ucdata)
        . = ALIGN(0x10);
        _bcpu_tmcudm_sram_uncached_data_end = .;
    }
    _bcpu_tmcudm_sram_uncached_data_size = _bcpu_tmcudm_sram_uncached_data_end - _bcpu_tmcudm_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_tmcudm_sram_end = .;
    _bcpu_tmcudm_sram_size = _bcpu_tmcudm_sram_end - _bcpu_tmcudm_sram_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_tmcudm_sram_flash_end_location = _bcpu_tmcudm_sram_flash_start_location + _bcpu_tmcudm_sram_size;
#else
    _bcpu_tmcudm_sram_flash_end_location = _bcpu_tmcudm_sram_flash_start_location;
#endif

    LD_ASSERT(ABSOLUTE(TMCU_DM_SRAM_BASE + TMCU_DM_SRAM_SIZE)
                        >= ABSOLUTE(_bcpu_tmcudm_sram_end), "BDPO", "TMCU DM SRAM overflow.")


	


   /* ===================================================== */
    /* in TMCU AND BB share SRAM                                           */
    /* ===================================================== */
    _bcpu_SHARE_BBsram1_flash_start_location = _bcpu_tmcudm_sram_flash_end_location;

    . = TMCU_SHARE_BBSRAM1_BASE;

    _bcpu_share_bbsram1_start = .;

    .bcpu_share_bbsram1_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram1_flash_start_location)
#else
        AT (ADDR(.bcpu_share_bbsram1_text))
#endif
    {
        _bcpu_share_bbsram1_text_start = .;
        *(.share_bbsram1_text)
        *(.share_bbsram1_rodata)
        . = ALIGN(0x10);
        _bcpu_share_bbsram1_text_end = .;
    }
    _bcpu_share_bbsram1_text_size = _bcpu_share_bbsram1_text_end - _bcpu_share_bbsram1_text_start;

    .bcpu_share_bbsram1_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram1_flash_start_location + _bcpu_share_bbsram1_text_size)
#else
        AT (ADDR(.bcpu_share_bbsram1_data))
#endif
    {
        _bcpu_share_bbsram1_cached_data_start = .;
        *(.share_bbsram1_data)
        . = ALIGN(0x10);
        _bcpu_share_bbsram1_cached_data_end = .;
    }
    _bcpu_share_bbsram1_cached_data_size = _bcpu_share_bbsram1_cached_data_end - _bcpu_share_bbsram1_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram1_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram1_flash_start_location + _bcpu_share_bbsram1_text_size + _bcpu_share_bbsram1_cached_data_size)
#else
        AT (ADDR(.bcpu_share_bbsram1_ucdata))
#endif
    {
        _bcpu_share_bbsram1_uncached_data_start = .;
        *(.share_bbsram1_ucdata)
        . = ALIGN(0x10);
        _bcpu_share_bbsram1_uncached_data_end = .;
    }
    _bcpu_share_bbsram1_uncached_data_size = _bcpu_share_bbsram1_uncached_data_end - _bcpu_share_bbsram1_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_share_bbsram1_end = .;
    _bcpu_share_bbsram1_size = _bcpu_share_bbsram1_end - _bcpu_share_bbsram1_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_share_bbsram1_flash_end_location = _bcpu_share_bbsram1_flash_start_location + _bcpu_share_bbsram1_size;
#else
    _bcpu_share_bbsram1_flash_end_location = _bcpu_sharet_bbsram1_flash_start_location;
#endif

    _bcpu_share_bbbss1_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram1_ucbss . (NOLOAD) : AT (ADDR(.bcpu_share_bbsram1_ucbss))
    {
        *(.share_bbsram1_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram1_bss . (NOLOAD) : AT (ADDR(.bcpu_share_bbsram1_bss))
    {
        *(.share_bbsram1_bss)
        . = ALIGN(0x10);
    }

    _bcpu_share_bbbss1_end = .;
    _bcpu_share_bbbss1_size = _bcpu_share_bbbss1_end - _bcpu_share_bbbss1_start;

    LD_ASSERT(ABSOLUTE(TMCU_SHARE_BBSRAM1_BASE + TMCU_SHARE_BBSRAM1_SIZE)
                        >= ABSOLUTE(_bcpu_share_bbbss1_end), "BDPO", "TCPU SHare BB SRAM1 overflow.")
                        
               
                        
   _bcpu_SHARE_BBsram2_flash_start_location = _bcpu_share_bbsram1_flash_end_location;

    . = TMCU_SHARE_BBSRAM2_BASE;

    _bcpu_share_bbsram2_start = .;

    .bcpu_share_bbsram2_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram2_flash_start_location)
#else
        AT (ADDR(.bcpu_share_bbsram2_text))
#endif
    {
        _bcpu_share_bbsram2_text_start = .;
        *(.share_bbsram2_text)
        *(.share_bbsram2_rodata)
        . = ALIGN(0x10);
        _bcpu_share_bbsram2_text_end = .;
    }
    _bcpu_share_bbsram2_text_size = _bcpu_share_bbsram2_text_end - _bcpu_share_bbsram2_text_start;

    .bcpu_share_bbsram2_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram2_flash_start_location + _bcpu_share_bbsram2_text_size)
#else
        AT (ADDR(.bcpu_share_bbsram2_data))
#endif
    {
        _bcpu_share_bbsram2_cached_data_start = .;
        *(.share_bbsram2_data)
        . = ALIGN(0x10);
        _bcpu_share_bbsram2_cached_data_end = .;
    }
    _bcpu_share_bbsram2_cached_data_size = _bcpu_share_bbsram2_cached_data_end - _bcpu_share_bbsram2_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram2_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_bbsram2_flash_start_location + _bcpu_share_bbsram2_text_size + _bcpu_share_bbsram2_cached_data_size)
#else
        AT (ADDR(.bcpu_share_bbsram1_ucdata))
#endif
    {
        _bcpu_share_bbsram2_uncached_data_start = .;
        *(.share_bbsram2_ucdata)
        . = ALIGN(0x10);
        _bcpu_share_bbsram2_uncached_data_end = .;
    }
    _bcpu_share_bbsram2_uncached_data_size = _bcpu_share_bbsram2_uncached_data_end - _bcpu_share_bbsram2_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_share_bbsram2_end = .;
    _bcpu_share_bbsram2_size = _bcpu_share_bbsram2_end - _bcpu_share_bbsram2_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_share_bbsram2_flash_end_location = _bcpu_share_bbsram2_flash_start_location + _bcpu_share_bbsram2_size;
#else
    _bcpu_share_bbsram2_flash_end_location = _bcpu_sharet_bbsram2_flash_start_location;
#endif

    _bcpu_share_bbbss2_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram2_ucbss . (NOLOAD) : AT (ADDR(.bcpu_share_bbsram2_ucbss))
    {
        *(.share_bbsram2_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_bbsram2_bss . (NOLOAD) : AT (ADDR(.bcpu_share_bbsram2_bss))
    {
        *(.share_bbsram2_bss)
        . = ALIGN(0x10);
    }

    _bcpu_share_bbbss1_end = .;
    _bcpu_share_bbbss1_size = _bcpu_share_bbbss2_end - _bcpu_share_bbbss2_start;

    LD_ASSERT(ABSOLUTE(TMCU_SHARE_BBSRAM2_BASE + TMCU_SHARE_BBSRAM2_SIZE)
                        >= ABSOLUTE(_bcpu_share_bbbss2_end), "BDPO", "TCPU SHare BB SRAM2 overflow.")
                        
                        
    _bcpu_SHARE_PMsram1_flash_start_location = _bcpu_share_bbsram2_flash_end_location;

    . = TMCU_SHARE_PMSRAM1_BASE;

    _bcpu_share_pmsram1_start = .;

    .bcpu_share_pmsram1_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram1_flash_start_location)
#else
        AT (ADDR(.bcpu_share_pmsram1_text))
#endif
    {
        _bcpu_share_pmsram1_text_start = .;
        *(.share_pmsram1_text)
        *(.share_pmsram1_rodata)
        . = ALIGN(0x10);
        _bcpu_share_pmsram1_text_end = .;
    }
    _bcpu_share_pmsram1_text_size = _bcpu_share_pmsram1_text_end - _bcpu_share_pmsram1_text_start;

    .bcpu_share_pmsram1_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram1_flash_start_location + _bcpu_share_pmsram1_text_size)
#else
        AT (ADDR(.bcpu_share_pmsram1_data))
#endif
    {
        _bcpu_share_pmsram1_cached_data_start = .;
        *(.share_pmsram1_data)
        . = ALIGN(0x10);
        _bcpu_share_pmsram1_cached_data_end = .;
    }
    _bcpu_share_pmsram1_cached_data_size = _bcpu_share_pmsram1_cached_data_end - _bcpu_share_pmsram1_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram1_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram1_flash_start_location + _bcpu_share_pmsram1_text_size + _bcpu_share_pmsram1_cached_data_size)
#else
        AT (ADDR(.bcpu_share_pmsram1_ucdata))
#endif
    {
        _bcpu_share_pmsram1_uncached_data_start = .;
        *(.share_pmsram1_ucdata)
        . = ALIGN(0x10);
        _bcpu_share_pmsram1_uncached_data_end = .;
    }
    _bcpu_share_pmsram1_uncached_data_size = _bcpu_share_pmsram1_uncached_data_end - _bcpu_share_pmsram1_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_share_pmsram1_end = .;
    _bcpu_share_pmsram1_size = _bcpu_share_pmsram1_end - _bcpu_share_pmsram1_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_share_pmsram1_flash_end_location = _bcpu_share_pmsram1_flash_start_location + _bcpu_share_pmsram1_size;
#else
    _bcpu_share_pmsram1_flash_end_location = _bcpu_sharet_pmsram1_flash_start_location;
#endif

    _bcpu_share_pmbss1_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram1_ucbss . (NOLOAD) : AT (ADDR(.bcpu_share_pmsram1_ucbss))
    {
        *(.share_pmsram1_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram1_bss . (NOLOAD) : AT (ADDR(.bcpu_share_pmsram1_bss))
    {
        *(.share_pmsram1_bss)
        . = ALIGN(0x10);
    }

    _bcpu_share_pmbss1_end = .;
    _bcpu_share_pmbss1_size = _bcpu_share_pmbss1_end - _bcpu_share_pmbss1_start;

    LD_ASSERT(ABSOLUTE(TMCU_SHARE_PMSRAM1_BASE + TMCU_SHARE_PMSRAM1_SIZE)
                        >= ABSOLUTE(_bcpu_share_pmbss1_end), "BDPO", "TCPU SHare PM SRAM1 overflow.")
  
  
   _bcpu_SHARE_PMsram2_flash_start_location = _bcpu_share_pmsram1_flash_end_location;

    . = TMCU_SHARE_PMSRAM2_BASE;

    _bcpu_share_pmsram2_start = .;

    .bcpu_share_pmsram2_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram2_flash_start_location)
#else
        AT (ADDR(.bcpu_share_pmsram2_text))
#endif
    {
        _bcpu_share_pmsram2_text_start = .;
        *(.share_pmsram1_text)
        *(.share_pmsram1_rodata)
        . = ALIGN(0x10);
        _bcpu_share_pmsram2_text_end = .;
    }
    _bcpu_share_pmsram2_text_size = _bcpu_share_pmsram2_text_end - _bcpu_share_pmsram2_text_start;

    .bcpu_share_pmsram2_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram2_flash_start_location + _bcpu_share_pmsram2_text_size)
#else
        AT (ADDR(.bcpu_share_pmsram2_data))
#endif
    {
        _bcpu_share_pmsram2_cached_data_start = .;
        *(.share_pmsram1_data)
        . = ALIGN(0x10);
        _bcpu_share_pmsram2_cached_data_end = .;
    }
    _bcpu_share_pmsram2_cached_data_size = _bcpu_share_pmsram2_cached_data_end - _bcpu_share_pmsram2_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram2_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bcpu_share_pmsram2_flash_start_location + _bcpu_share_pmsram2_text_size + _bcpu_share_pmsram2_cached_data_size)
#else
        AT (ADDR(.bcpu_share_pmsram2_ucdata))
#endif
    {
        _bcpu_share_pmsram2_uncached_data_start = .;
        *(.share_pmsram1_ucdata)
        . = ALIGN(0x10);
        _bcpu_share_pmsram2_uncached_data_end = .;
    }
    _bcpu_share_pmsram2_uncached_data_size = _bcpu_share_pmsram2_uncached_data_end - _bcpu_share_pmsram2_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_share_pmsram2_end = .;
    _bcpu_share_pmsram2_size = _bcpu_share_pmsram2_end - _bcpu_share_pmsram2_start;

#ifndef PRELOADED_INT_SRAM
    _bcpu_share_pmsram2_flash_end_location = _bcpu_share_pmsram2_flash_start_location + _bcpu_share_pmsram1_size;
#else
    _bcpu_share_pmsram2_flash_end_location = _bcpu_sharet_pmsram2_flash_start_location;
#endif

    _bcpu_share_pmbss2_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram2_ucbss . (NOLOAD) : AT (ADDR(.bcpu_share_pmsram2_ucbss))
    {
        *(.share_pmsram2_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_share_pmsram2_bss . (NOLOAD) : AT (ADDR(.bcpu_share_pmsram2_bss))
    {
        *(.share_pmsram2_bss)
        . = ALIGN(0x10);
    }

    _bcpu_share_pmbss2_end = .;
    _bcpu_share_pmbss2_size = _bcpu_share_pmbss2_end - _bcpu_share_pmbss2_start;

    LD_ASSERT(ABSOLUTE(TMCU_SHARE_PMSRAM2_BASE + TMCU_SHARE_PMSRAM2_SIZE)
                        >= ABSOLUTE(_bcpu_share_pmbss2_end), "BDPO", "TCPU SHare PM SRAM2 overflow.")
  
            
    #endif  /*#if(CHIP_HAS_TMCU==1)*/
                                          
    /* ===================================================== */
    /* in External RAM                                           */
    /* ===================================================== */
     #if (CHIP_HAS_TMCU==1)
        _bcpu_ram_flash_start_location = _bcpu_share_pmsram2_flash_end_location;
     #else
         _bcpu_ram_flash_start_location = _bcpu_sys_sram_flash_end_location;
     #endif
#if !defined(RAMRUN) && !defined(CHIP_HAS_AP)

    . = _bcpu_ram_available_base;

#else /* RAMRUN || CHIP_HAS_AP */

    /* In a RAMRUN or CHIP_HAS_AP, all the flash content will be put in the
     * ram, and _bcpu_ram_flash_start_location is in fact pointing to a ram
     * location. */
    . = _bcpu_ram_flash_start_location;

#endif /* RAMRUN || CHIP_HAS_AP */


    _bcpu_ram_start = .;

#ifndef PRELOADED_RAM
    .bcpu_ramtext . : AT (_bcpu_ram_flash_start_location)
#else
    .bcpu_ramtext . : AT (ADDR(.bcpu_ramtext))
#endif
    {
        _bcpu_ram_txt_start = .;
        *(.ram)
#ifdef BUILD_BCPU_PSRAM
        *spc_nb_main.o(.bcpu_main_entry)
        *(.text .text.*)
        *(.rdata .rodata .rodata.str1.4 .rodata.* .rodata1)
#endif
        . = ALIGN(0x10);
    }
    _bcpu_ram_txt_end = .;
    _bcpu_ram_txt_size = _bcpu_ram_txt_end - _bcpu_ram_start;

#ifndef PRELOADED_RAM
    _bcpu_ram_data_load_start = _bcpu_ram_flash_start_location + _bcpu_ram_txt_size;
#else
    _bcpu_ram_data_load_start = .;
#endif

#ifdef CHIP_HAS_AP
#ifdef BCPU_EXT_RAM_DATA_SIZE
    . += BCPU_EXT_RAM_DATA_SIZE;
#endif
#endif

    _bcpu_ram_data_start = .;

    .bcpu_data . : AT (_bcpu_ram_data_load_start)
    {
        _bcpu_ram_cached_start = .;
        *(.data .data.* .sdata .sdata.* .data1)
        . = ALIGN(0x10);
    }
    _bcpu_ram_cached_end = .;
    _bcpu_ram_cached_size = _bcpu_ram_cached_end - _bcpu_ram_cached_start;

    /*-------------------- */
    /* External RAM UNCACHED   */
    /*-------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_ucdata . : AT (_bcpu_ram_data_load_start + _bcpu_ram_cached_size)
    {
        _bcpu_ram_uncached_start = .;
        *(.ucdata)
        . = ALIGN(0x10);
    }
    _bcpu_ram_uncached_end = .;
    _bcpu_ram_uncached_size = _bcpu_ram_uncached_end - _bcpu_ram_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_ram_data_end = .;
    _bcpu_ram_data_size = _bcpu_ram_data_end - _bcpu_ram_data_start;

    _bcpu_ram_data_load_end = _bcpu_ram_data_load_start + _bcpu_ram_data_size;

#ifdef CHIP_HAS_AP
#if defined(BCPU_EXT_RAM_DATA_SIZE) && (BCPU_EXT_RAM_DATA_SIZE > 0)
    /* Check that the bcpu ram data is not over overflowed */
    LD_ASSERT(ABSOLUTE(_bcpu_ram_data_start) >= ABSOLUTE(_bcpu_ram_data_load_end), "BDOV", "BCPU ram data overflow (BCPU_EXT_RAM_DATA_SIZE too small) !")
#endif
#endif

    _bcpu_ram_end = .;
    _bcpu_ram_size = _bcpu_ram_end - _bcpu_ram_start;

#if !defined(PRELOADED_RAM) && !defined(CHIP_HAS_AP)
    _bcpu_ram_flash_end_location = _bcpu_ram_flash_start_location + _bcpu_ram_size;
#else
    _bcpu_ram_flash_end_location = _bcpu_ram_flash_start_location;
#endif

    /*----------------------- */
    /* External RAM BSS           */
    /*----------------------- */
    _bcpu_ram_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_ucbss . (NOLOAD) : AT (ADDR(.bcpu_ucbss))
    {
        *(.ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_bss . (NOLOAD) : AT (ADDR(.bcpu_bss))
    {
        *(.bss .bss.* .sbss .sbss.*)
        . = ALIGN(0x10);
    }

    _bcpu_ram_bss_end = .;
    _bcpu_ram_bss_size = _bcpu_ram_bss_end - _bcpu_ram_bss_start;

    /* Check that the External RAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(EXT_RAM_BASE + RAM_SIZE)
                        > ABSOLUTE(_bcpu_ram_bss_end), "BEXT", "BCPU External RAM overflow !")

    /*---------------- */
    /* End of External RAM */
    /*---------------- */


 

    /*========================================================= */
    /* Back in FLASH                                                 */
    /*========================================================= */

#if !defined(RAMRUN) && !defined(CHIP_HAS_AP)
    . = _bcpu_ram_flash_end_location;
#else
    . = _bcpu_ram_bss_end;
#endif

    /* WORKAROUND:
     * Keep the following section as the last section to remove the LD warning,
     * "access beyond end of merged section", when linking XCPU image. This warning might
     * be ejected due to introducing some BCPU LD symbols defined above to the XCPU image.
     */
    .bcpu_flash_end . : AT(ADDR(.bcpu_flash_end))
    {
        LONG(0)
        . = ALIGN(0x10);
    }

    _bcpu_flash_end = .;

    /* This is a rough check. XCPU image linking process will check flash/ram usage in detail. */
#if !defined(RAMRUN) && !defined(CHIP_HAS_AP)
    LD_ASSERT(ABSOLUTE(FLASH_BASE + FLASH_SIZE) > ABSOLUTE(_bcpu_flash_end),
              "BFLA",
              "BCPU Flash overflow: there is no more space available in flash memory.")
#else
    LD_ASSERT(ABSOLUTE(EXT_RAM_BASE + RAM_SIZE) > ABSOLUTE(_bcpu_flash_end),
              "BRAM",
              "BCPU RAM overflow: there is no more space available in RAM.")
#endif


    #include "discard.ld"
}
