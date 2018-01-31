/* ======================================================================= */
/*                  TARGET CONFIGURATION SECTION                           */
/* ======================================================================= */
/* modem2g_internal_rom_master.ld */
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
/*  - PRELOADED_RAM : The external RAm is already preloaded, useful for ram_run or simu */

/* Take into account assert bug in binutils-2.15*/
#if (USE_BINUTILS_2_19 == 1)
#define LD_ASSERT(condition, small, full)   ASSERT(condition, full)
#else
#define LD_ASSERT(condition, small, full)   ASSERT(condition, small)
#endif

#define MEM_CACHED_UNCACHED_OFFSET 0x20000000

EXTERN (__xcpu_start)

EXTERN (boot_romCsString boot_Monitor g_bootRomVersionNumber)

EXTERN (_boot_IrqHandler)

EXTERN (boot_UsbOpen
        boot_UsbClose
        boot_UsbReset
        boot_UsbRecv
        boot_UsbSend
        boot_UsbEpStall
        boot_UsbCompletedCommand
        boot_UsbEpEnableInterrupt
        boot_UsbEpRxTransferedSize
        boot_UsbEpTransfertDone
        boot_UsbIrqHandler)

EXTERN (boot_HostUsbOpen
        boot_HostUsbClose
        boot_HostUsbSend
        boot_HostUsbRecv)

EXTERN (atoi
        bsearch
        _ctype
        memchr
        memcmp
        memcpy
        memmove
        memscan
        memset
        qsort
        strcat
        strchr
        strrchr
        strcmp
        strcpy
        strcspn
        strlen
        strnlen
        strncat
        strncmp
        strncpy
        strnicmp
        strsep
        strspn
        strstr
        strtok
        strtok_r
        strtol
        strtoul
        vsnprintf
        vsprintf
        sprintf
        snprintf
        vsscanf
        sscanf)

EXTERN (MutedFrame_AMR MutedFrame_HR MutedFrame_EFR MutedFrame_FR)

/* GALLITE or 8808, 8810 */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) || (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
EXTERN (__bcpu_start)

EXTERN (bcpu_error_code
        bcpu_error_status
        bcpu_sp_context
        bcpu_stack_base
        bcpu_main_entry)

EXTERN (g_mailbox)

EXTERN (spp_SearchAmrMarker  spp_CipherCountInit spp_FcchDetectAsm spp_FcchDetect
        spal_IrqSetMask spp_DecodeBlock spp_Open
        spp_EqualizeNBurst spp_SearchFcch spp_EncodeBlock
        spal_ItlvItlv spp_10log10 g_sppNbFwequStartSt
        __bb_int_handler
        )

EXTERN (spal_InvalidateDcache
        spal_Open
        spal_VitacBusy
        spal_XcorTscxcNb
        spal_DbgHstSendEvent
        )

EXTERN (spc_AmrDecode spc_UpdateAmrCfg g_mailbox
        spc_RxProcess g_spcCtxStruct AFSfilterCI
        spc_MboxToSppSpeechMode spc_IrqHandler)

EXTERN (spp_GsmFuncInit spp_IrFuncInit spp_EgprsFuncInit spc_EgprsFuncInit spp_EqualizeNBurst_Prefilter)

EXTERN (g_spalMapVersion g_spcMapVersion g_sppMapVersion)
#endif

#ifdef CHIP_HAS_BTCPU
EXTERN (btcpu_error_code btcpu_error_status btcpu_sp_context
        btcpu_stack_base btcpu_main_entry btcpu_int_entry)
EXTERN (__btcpu_start __btcpu_int_handler)
#endif

#ifdef CHIP_HAS_WCPU
EXTERN (_binary_wcpu_rom_bin_start)
#endif

#ifndef ROM_RM_VOCODER
EXTERN (g_vppAmrCode)
#endif

SECTIONS
{
    /* ========================================================= */
    /* INT ROM                                                   */
    /* ========================================================= */
    . = INT_ROM_BASE;

    __int_rom_main = .;
    _int_rom_start = .;

/* 8810 or ... */
#if (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
#define ROTEXT_CONTENT
#endif

    .internal_rom . : AT(ADDR(.internal_rom))
    {
        /* Specifies a section for the functions to put
           in ROM. */

/* TODO Improve readibility */
#if (CHIP_ASIC_ID == 6)
        #include "gallite_internal_rom_content"
#elif (CHIP_ASIC_ID == 8)
        #include "8808_internal_rom_content"
#elif (CHIP_ASIC_ID == 9) || (CHIP_ASIC_ID == 11)||(CHIP_ASIC_ID == 17)
        #include "8809_internal_rom_content"
#elif (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
        #include "8810_internal_rom_content"
#elif (CHIP_ASIC_ID == 13) || (CHIP_ASIC_ID == 14) || (CHIP_ASIC_ID == 15)
        #include "8850_internal_rom_content"
#else
#error "Unsupported Chip Id"
#endif
        . = ALIGN(0x10);
    }

/* 8810 or ... */
#if (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)

#undef ROTEXT_CONTENT
#define TCM_ROTEXT_CONTENT

    /* BCPU ROM start */
    /* must be aligned at 0xa1e01000 */
    . = INT_ROM_BASE + 0x1000;

    /* Go uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .internal_rom_tcm . : AT(ADDR(.internal_rom_tcm) - MEM_CACHED_UNCACHED_OFFSET)
    {
        /* Specifies a section for the functions to put
           in ROM TCM. */

/* TODO Improve readibility */
#if (CHIP_ASIC_ID == 10)  || (CHIP_ASIC_ID == 16)
        #include "8810_internal_rom_content"
#else
#error "Unsupported Chip Id"
#endif
        . = ALIGN(0x10);
    }

    /* Go cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

#undef TCM_ROTEXT_CONTENT
#define ROTEXT_CONTENT_PART2

    .internal_rom_part2 . : AT(ADDR(.internal_rom_part2))
    {
        /* Specifies a section for the data to put
           in ROM part2. */

#if (CHIP_ASIC_ID == 10)  || (CHIP_ASIC_ID == 16) 
        #include "8810_internal_rom_content"
#else
#error "Unsupported Chip Id"
#endif
        . = ALIGN(0x10);
    }

#undef ROTEXT_CONTENT_PART2
#define RODATA_CONTENT

    .internal_rom_rodata . : AT(ADDR(.internal_rom_rodata))
    {
        /* Specifies a section for the data to put
           in ROM. */

#if (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
        #include "8810_internal_rom_content"
#else
#error "Unsupported Chip Id"
#endif
        . = ALIGN(0x10);
    }
#endif /* 8810 or ... */

#ifdef CHIP_HAS_BTCPU
    . = BTCPU_ROM_BASE;

    .btcpu_rom . : AT(ADDR(.btcpu_rom))
    {
        /* Specifies a section for the functions to put
           in ROM. */

#if (CHIP_ASIC_ID == 11)
        #include "8809p_btcpu_rom_content"
#else
#error "Unsupported Chip Id"
#endif
        . = ALIGN(0x10);
    }
#endif

#ifdef CHIP_HAS_WCPU
    . = WD_ROM_BASE;
    .wcpu_rom . : AT(ADDR(.wcpu_rom))
    {
        __wcpu_start = .;
        KEEP(*wcpu_rom.bin.o(.rodata .rodata.*))
        . = ALIGN(0x10);
    }
#endif

    __int_rom_end = .;

    . = ROM_MISC_BASE + MEM_CACHED_UNCACHED_OFFSET;

    .rom_entries_uncached . (NOLOAD) : AT(ADDR(.rom_entries_uncached))
    {
        #include "modem2G_internal_rom_reserved_uncached_ram"
        . = ALIGN(0x4);
    }

    . -= MEM_CACHED_UNCACHED_OFFSET;

    .rom_entries_cached . (NOLOAD) : AT(ADDR(.rom_entries_cached))
    {
#if (CHIP_ASIC_ID == 6)
        #include "gallite_internal_rom_reserved_ram"
#elif (CHIP_ASIC_ID == 8)
        #include "8808_internal_rom_reserved_ram"
#elif (CHIP_ASIC_ID == 9) || (CHIP_ASIC_ID == 11)||(CHIP_ASIC_ID == 17)
        #include "8809_internal_rom_reserved_ram"
#elif (CHIP_ASIC_ID == 10)  || (CHIP_ASIC_ID == 16)
        #include "8810_internal_rom_reserved_ram"
#elif (CHIP_ASIC_ID == 13) || (CHIP_ASIC_ID == 14) || (CHIP_ASIC_ID == 15)
        #include "8850_internal_rom_reserved_ram"
#else
#error "Unsupported Chip Id"
#endif
    }

    __int_rom_reserved_end = . ;

    /* Reserved Ram for Rom overflow */
    LD_ASSERT(ABSOLUTE(__int_rom_reserved_end) <= ROM_MISC_BASE + ROM_MISC_SIZE, "RRRO", "Reserved Ram Reserved for Rom Overflowed")

/* GALLITE or 8808, 8810 */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) || (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
    /* ========================================================= */
    /* BB SRAM                                                   */
    /* ========================================================= */
    . = BB_SRAM_BASE;
    _bb_sram_romed_start = .;

    .bbsram . (NOLOAD) : AT(ADDR(.bbsram))
    {
        _bb_sram_romed_cached_start = .;
        *(BB_ITLV_SECTION)
        . = ALIGN(0x4);
        _bb_sram_romed_itlv_buf_end = .;
        *(BB_SPC_CTX_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_romed_cached_end = .;
    _bb_sram_romed_cached_size = _bb_sram_romed_cached_end - _bb_sram_romed_cached_start;

    . += MEM_CACHED_UNCACHED_OFFSET;

    .bbsramu . (NOLOAD) : AT(ADDR(.bbsramu))
    {
        _bb_sram_romed_uncached_start = .;
        . = ALIGN(0x10);
        _mbbsramu = .;
        *(BB_STATIC_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_romed_uncached_end = .;
    _bb_sram_romed_uncached_size = _bb_sram_romed_uncached_end - _bb_sram_romed_uncached_start;

    _bb_sram_romed_size = _bb_sram_romed_cached_size + _bb_sram_romed_uncached_size;
    _bb_sram_romed_end = _bb_sram_romed_start + _bb_sram_romed_size;

    .bbsram_globals (BB_SRAM_BASE + BB_SRAM_SIZE - RESERVED_PATCH_SIZE - BB_SRAM_GLOBALS_SIZE) (NOLOAD) : AT(ADDR(.bbsram_globals))
    {
        _bb_sram_romed_globals_start = .;
        *(BB_SRAM_GLOBALS)
        *(.bbsram_globals)
        _bb_sram_romed_globals_end = .;
    }

    /* BBSRAM overflow */
    LD_ASSERT(SIZEOF(.bbsram_globals) <= BB_SRAM_GLOBALS_SIZE, "BBSO", "BBSRAM Overflozed !!")
#endif

    /* ========================================================= */
    /* DUALPORT SRAM                                             */
    /* ========================================================= */
#if (CHIP_ASIC_ID == 8) /* 8808 */
    .dualport_nand_flash_boot (BB_DUALPORT_SRAM_BASE) (NOLOAD) : AT (ADDR(.dualport_nand_flash_boot))
    {
        _nand_flash_boot_start = .;
    }

    .mailbox (BB_DUALPORT_SRAM_BASE + BB_DUALPORT_SRAM_SIZE - RESERVED_MBX_SIZE + MEM_CACHED_UNCACHED_OFFSET) (NOLOAD) : AT(ADDR(.mailbox))
    {
        _dualport_sram_romed_mbx_start = .;
        *spc_mailbox.o(.mailbox)
        . = ALIGN(0x10);
    }

    /* Mailbox overflow */
    LD_ASSERT(SIZEOF(.mailbox) <= RESERVED_MBX_SIZE, "MBXO", "Mailbox Overlfowed !!")
#endif

 #if (CHIP_ASIC_ID == 16)  || (CHIP_ASIC_ID == 17)
 #if  (CHIP_HAS_TMCU == 1)
    /* ===================================================== */
    /* in TUMCU DM                                           */
    /* ===================================================== */
 
  . = TMCU_IFCBUF_ADDR;
    _bcpu_tmcudm_sram_start = .;

    .bcpu_tmcudm_sram_data . (NOLOAD) : AT(ADDR(.bcpu_tmcudm_sram_data))
    {
        _bcpu_tmcudm_sram_cached_data_start = .;
        *(.dm_ifcsram_data)
        . = ALIGN(0x4);
        _bcpu_tmcudm_sram_cached_data_end = .;
  
    }
    _bcpu_tmcudm_sram_cached_data_size = _bcpu_tmcudm_sram_cached_data_end - _bcpu_tmcudm_sram_cached_data_start;

  
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_tmcudm_sram_ucdata . :

     AT (ADDR(.bcpu_tmcudm_sram_ucdata))

    {
        _bcpu_tmcudm_sram_uncached_data_start = .;
        *(.dm_ifcsram_ucdata)
        . = ALIGN(0x10);
        _bcpu_tmcudm_sram_uncached_data_end = .;
    }
    
      
    _bcpu_tmcudm_sram_uncached_data_size = _bcpu_tmcudm_sram_uncached_data_end - _bcpu_tmcudm_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;
    
  	. = TMCU_IFCBUF_FCCHADDR;
      .bcpu_tmcudmfcch_sram_data . (NOLOAD) : AT(ADDR(.bcpu_tmcudmfcch_sram_data))
    {
        _bcpu_tmcudmfcch_sram_cached_data_start = .;
        *(.dm_fcchsram_data)
        . = ALIGN(0x4);
        _bcpu_tmcudmfcch_sram_cached_data_end = .;
  
    }
    _bcpu_tmcudmfcch_sram_cached_data_size = _bcpu_tmcudmfcch_sram_cached_data_end - _bcpu_tmcudmfcch_sram_cached_data_start;

  
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bcpu_tmcudmfcch_sram_ucdata . :

     AT (ADDR(.bcpu_tmcudmfcch_sram_ucdata))

    {
        _bcpu_tmcudmfcch_sram_uncached_data_start = .;
        *(.dm_fcchsram_ucdata)
        . = ALIGN(0x10);
        _bcpu_tmcudmfcch_sram_uncached_data_end = .;
    }
    
      
    _bcpu_tmcudmfcch_sram_uncached_data_size = _bcpu_tmcudmfcch_sram_uncached_data_end - _bcpu_tmcudmfcch_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bcpu_tmcudm_sram_end = .;
    _bcpu_tmcudm_sram_size = _bcpu_tmcudm_sram_end - _bcpu_tmcudm_sram_start;

    LD_ASSERT(ABSOLUTE(TMCU_DM_SRAM_BASE + TMCU_DM_SRAM_SIZE)
                        >= ABSOLUTE(_bcpu_tmcudm_sram_end), "BDPO", "TMCU DM SRAM overflow.")




   /* ===================================================== */
    /* in TMCU AND BB share SRAM                                           */
    /* ===================================================== */
 

    . = TMCU_SHARE_BBSRAM1_BASE;

    _bcpu_share_bbsram1_start = .;

    .bcpu_share_bbsram1_text . :

    AT (ADDR(.bcpu_share_bbsram1_text))

    {
        _bcpu_share_bbsram1_text_start = .;
        *(.share_bbsram1_text)
        *(.share_bbsram1_rodata)
        . = ALIGN(0x10);
        _bcpu_share_bbsram1_text_end = .;
    }
    _bcpu_share_bbsram1_text_size = _bcpu_share_bbsram1_text_end - _bcpu_share_bbsram1_text_start;

    .bcpu_share_bbsram1_data . :

        AT (ADDR(.bcpu_share_bbsram1_data))
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
        AT (ADDR(.bcpu_share_bbsram1_ucdata))
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


    LD_ASSERT(ABSOLUTE(TMCU_SHARE_BBSRAM1_BASE + TMCU_SHARE_BBSRAM1_SIZE)
                        >= ABSOLUTE(_bcpu_share_bbsram1_end), "BDPO", "TCPU SHare BB SRAM1 overflow.")

 

    . = TMCU_SHARE_BBSRAM2_BASE;

    _bcpu_share_bbsram2_start = .;

   
    .bcpu_share_bbsram2_data . :

    AT (ADDR(.bcpu_share_bbsram2_data))
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
    AT (ADDR(.bcpu_share_bbsram1_ucdata))
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

   LD_ASSERT(ABSOLUTE(TMCU_SHARE_BBSRAM2_BASE + TMCU_SHARE_BBSRAM2_SIZE)
                        >= ABSOLUTE(_bcpu_share_bbsram2_end), "BDPO", "TCPU SHare BB SRAM2 overflow.")
                        
                        

    . = TMCU_SHARE_PMSRAM1_BASE;

    _bcpu_share_pmsram1_start = .;


    .bcpu_share_pmsram1_data . :
    AT (ADDR(.bcpu_share_pmsram1_data))
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
    AT (ADDR(.bcpu_share_pmsram1_ucdata))
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


    LD_ASSERT(ABSOLUTE(TMCU_SHARE_PMSRAM1_BASE + TMCU_SHARE_PMSRAM1_SIZE)
                        >= ABSOLUTE(_bcpu_share_pmsram1_end), "BDPO", "TCPU SHare PM SRAM1 overflow.")
  
  
    . = TMCU_SHARE_PMSRAM2_BASE;

    _bcpu_share_pmsram2_start = .;

    .bcpu_share_pmsram2_data . :
     AT (ADDR(.bcpu_share_pmsram2_data))
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
     AT (ADDR(.bcpu_share_pmsram2_ucdata))
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

    LD_ASSERT(ABSOLUTE(TMCU_SHARE_PMSRAM2_BASE + TMCU_SHARE_PMSRAM2_SIZE)
                        >= ABSOLUTE(_bcpu_share_pmsram2_end), "BDPO", "TCPU SHare PM SRAM2 overflow.")

#endif //CHIP_HAS_TMCU
#endif
                        
    /* ========================================================= */
    /* INT SRAM                                                  */
    /* ========================================================= */
    .sram_romed_top (SYS_SRAM_END) (NOLOAD) : AT(ADDR(.sram_romed_top))
    {
        _sys_sram_non_romed_top = .;
    }

/* GALLITE or 8810 */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 10) || (CHIP_ASIC_ID == 16)
    .mailbox (SYS_SRAM_BASE + SYS_SRAM_SIZE - RESERVED_MBX_SIZE + MEM_CACHED_UNCACHED_OFFSET) (NOLOAD) : AT(ADDR(.mailbox))
    {
        _sys_sram_romed_mbx_start = .;
        *spc_mailbox.o(.mailbox)
        . = ALIGN(0x10);
    }

    /* Mailbox overflow */
    LD_ASSERT(SIZEOF(.mailbox) <= RESERVED_MBX_SIZE, "MBXO", "Mailbox Overlfowed !!")
#endif

    /* ========================================================= */
    /* Back in the INT ROM                                       */
    /* ========================================================= */

    .boot_rom_version_number (INT_ROM_END - 4) : AT(ADDR(.boot_rom_version_number))
    {
        _int_rom_version_number = .;
        *(.boot_rom_version_number)
    }

    .internal_rom.crc INT_ROM_END : AT(ADDR(.internal_rom.crc))
    {
        __int_rom_crc = . ;
        LONG(0xc001c001)
    }

    /DISCARD/ :
    {
        *(*)
    }
}

