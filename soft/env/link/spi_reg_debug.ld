    /* ------------------------------------------------ */
    /* SPI REG DEBUG - Ext RAM UNCACHED */
    /* ------------------------------------------------ */

#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) /* GALLITE or 8808 */
#define ABB_REG_SIZE 0x100
#define PMU_REG_SIZE 0x100
#elif (CHIP_ASIC_ID == 9) || (CHIP_ASIC_ID == 11) /* 8809 or 8809P */
#define ABB_REG_SIZE 0x140
#define PMU_REG_SIZE 0x140
#elif (CHIP_ASIC_ID == 16)
#define ABB_REG_SIZE 0x200
#define PMU_REG_SIZE 0x240
#else /* 8810 or later */
#define ABB_REG_SIZE 0x200
#define PMU_REG_SIZE 0x180
#endif


#ifdef FPGA
#define FM_REG_SIZE  0x0
#else
#if (CHIP_ASIC_ID == 6) /* GALLITE */
#define FM_REG_SIZE  0x100
#elif (CHIP_DIE_8955)
#define FM_REG_SIZE  0x200
#else /* 8808 or later */
#define FM_REG_SIZE  0x0
#endif
#endif

#ifdef FPGA
#define XCV_REG_SIZE 0x300
#else /* !FPGA */
#if (CHIP_ASIC_ID == 6) /* GALLITE */
#if defined(GALLITE_IS_8805)
#define XCV_REG_SIZE 0x300
#else /* 8806 */
#define XCV_REG_SIZE 0x400
#endif
#elif (CHIP_ASIC_ID == 8) || (CHIP_ASIC_ID == 9) /* 8808 or 8809 */
#define XCV_REG_SIZE 0x500
#elif (CHIP_ASIC_ID == 15)||(CHIP_ASIC_ID == 13)||(CHIP_ASIC_ID == 14) /* 8850, 8820,8850e or later WCDMA chips */
#define XCV_REG_SIZE 0x400
#else /* 8810, 8809P or later GSM chips */
#define XCV_REG_SIZE 0x200
#endif
#endif /* !FPGA */

#ifdef CHIP_HAS_AP
#define AP_REG_SIZE 0x280
#else
#define AP_REG_SIZE 0x0
#endif

#ifdef LINK_BCPU_IMAGE

    /* Just leave a hole for spi reg debug */
    . += XCV_REG_SIZE + ABB_REG_SIZE + FM_REG_SIZE + PMU_REG_SIZE + AP_REG_SIZE;

#else

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    _ram_spi_reg_debug_uncached_start = .;

    .spi_reg_debug (_ram_spi_reg_debug_uncached_start) (NOLOAD) :
        AT (ADDR(.spi_reg_debug))
    {
        _spi_reg_debug_start = .;
        *(.xcv_reg_value)
        . = _spi_reg_debug_start + XCV_REG_SIZE;
        *(.abb_reg_value)
        . = _spi_reg_debug_start + XCV_REG_SIZE + ABB_REG_SIZE;
#if (FM_REG_SIZE > 0)
        *(.fmd_reg_value)
        . = _spi_reg_debug_start + XCV_REG_SIZE + ABB_REG_SIZE + FM_REG_SIZE;
#endif
        *(.pmu_reg_value)
        . = _spi_reg_debug_start + XCV_REG_SIZE + ABB_REG_SIZE + FM_REG_SIZE + PMU_REG_SIZE;
        *(.ap_reg_value)
        . = _spi_reg_debug_start + XCV_REG_SIZE + ABB_REG_SIZE + FM_REG_SIZE + PMU_REG_SIZE + AP_REG_SIZE;
    }

    _ram_spi_reg_debug_uncached_end = .;
    _ram_spi_reg_debug_uncached_size = _ram_spi_reg_debug_uncached_end - _ram_spi_reg_debug_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

#endif

