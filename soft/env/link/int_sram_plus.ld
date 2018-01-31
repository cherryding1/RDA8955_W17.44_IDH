    /* ------------------------------------------------ */
    /* SYS SRAM PLUS SECTION */
    /* ------------------------------------------------ */

    _sys_sram_plus_start = .;
    _sys_sram_plus_flash_start_location = _sys_sram_flash_end_location;

#if (CHIP_ASIC_ID == 10)

    .sys_sram_plus_1 _sys_sram_plus_start (NOLOAD) : AT (ADDR(.sys_sram_plus_1))
    {
        _sys_sram_plus_1_start = .;
        *spc_main.o(.intsram_plus)
        . = ALIGN(0x10);
    }
    _sys_sram_plus_1_end = .;

    .sys_sram_plus_2_text _sys_sram_plus_start : AT (_sys_sram_plus_flash_start_location)
    {
        _sys_sram_plus_2_text_start = .;
        *drv_xcv_init.o(ROTEXT() RODATA())
        . = ALIGN(0x10);
    }
    _sys_sram_plus_2_text_end = .;

    .sys_sram_plus_2_bss . (NOLOAD) : AT (SYS_SRAM_END /* Just an unused address */)
    {
        _sys_sram_plus_2_bss_start = .;
        *drv_xcv_init.o(.bss .bss.* .scommon)
        . = ALIGN(0x10);
    }

    _sys_sram_plus_2_end = .;

    _sys_sram_plus_flash_end_location = _sys_sram_plus_flash_start_location +
        (_sys_sram_plus_2_text_end - _sys_sram_plus_2_text_start);

    _sys_sram_plus_end = MAX(_sys_sram_plus_1_end, _sys_sram_plus_2_end);

#else

    _sys_sram_plus_flash_end_location = _sys_sram_plus_flash_start_location;
    _sys_sram_plus_end = .;

#endif

    . = _sys_sram_plus_end;

