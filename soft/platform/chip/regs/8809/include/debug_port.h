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


#ifndef _DEBUG_PORT_H_
#define _DEBUG_PORT_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'debug_port'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// DEBUG_MUX_CLOCKS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    DBG_SEL_SYS_NOGATE                          = 0x00000000,
    DBG_SEL_SYSD_SCI                            = 0x00000001,
    DBG_SEL_SYSD_SPI                            = 0x00000002,
    DBG_SEL_SYSD_SPI2                           = 0x00000003,
    DBG_SEL_SYSD_SPI3                           = 0x00000004,
    DBG_SEL_SYSD_RF_SPI                         = 0x00000005,
    DBG_SEL_SYSD_OSC                            = 0x00000006,
    DBG_SEL_SYSD_PWM                            = 0x00000007,
    DBG_SEL_XCPU                                = 0x00000008,
    DBG_SEL_SYSD_DAIS_208K                      = 0x00000009,
    DBG_SEL_HOST                                = 0x0000000A,
    DBG_SEL_AUDIO_PLL                           = 0x0000000B,
    DBG_SEL_AUDIO_BCK                           = 0x0000000C,
    DBG_SEL_CLK_DBG_PLL                         = 0x0000000D,
    DBG_SEL_CLK_DBG                             = 0x0000000E,
    DBG_SEL_UART                                = 0x0000000F,
    DBG_SEL_UART2                               = 0x00000010,
    DBG_SEL_RF_RX                               = 0x00000011,
    DBG_SEL_RF_TX                               = 0x00000012,
    DBG_SEL_MEM_BRIDGE                          = 0x00000013,
    DBG_SEL_VOC                                 = 0x00000014,
    DBG_SEL_32K                                 = 0x00000015,
    DBG_SEL_USBPHY                              = 0x00000016,
    DBG_SEL_CAMERA                              = 0x00000017,
    DBG_SEL_PIX                                 = 0x00000018,
    DBG_SEL_DIG52M                              = 0x00000019,
/// this clock is taken before clock root
    DBG_SEL_OSC                                 = 0x0000001A,
/// this clock is taken before clock root
    DBG_SEL_REF                                 = 0x0000001B,
/// this clock is taken before clock root
    DBG_SEL_SLOW                                = 0x0000001C,
/// this clock is taken before clock root
    DBG_SEL_FAST_PLL                            = 0x0000001D,
/// this clock is taken before clock root
    DBG_SEL_VOC_PLL                             = 0x0000001E,
/// this clock is taken before clock root
    DBG_SEL_SYS_156M_PLL                        = 0x0000001F,
    DBG_SEL_BB_PLL                              = 0x00000020,
    DBG_SEL_TCU                                 = 0x00000021
} DEBUG_MUX_CLOCKS_T;

#define NB_CLK_DBG_SEL                           (34)

// ============================================================================
// DEBUG_SIGNAL_SELECT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    DBG_SYS_IRQ_TCU0                            = 0x00000000,
    DBG_SYS_IRQ_TCU1                            = 0x00000001,
    DBG_SYS_IRQ_FRAME                           = 0x00000002,
    DBG_SYS_IRQ_COM0                            = 0x00000003,
    DBG_SYS_IRQ_COM1                            = 0x00000004,
    DBG_SYS_IRQ_VOC                             = 0x00000005,
    DBG_SYS_IRQ_DMA                             = 0x00000006,
    DBG_SYS_IRQ_GPIO                            = 0x00000007,
    DBG_SYS_IRQ_KEYPAD                          = 0x00000008,
    DBG_SYS_IRQ_TIMERS                          = 0x00000009,
    DBG_SYS_IRQ_OS_TIMER                        = 0x0000000A,
    DBG_SYS_IRQ_CALENDAR                        = 0x0000000B,
    DBG_SYS_IRQ_SPI1                            = 0x0000000C,
    DBG_SYS_IRQ_SPI2                            = 0x0000000D,
    DBG_SYS_IRQ_SPI3                            = 0x0000000E,
    DBG_SYS_IRQ_DEBUG_UART                      = 0x0000000F,
    DBG_SYS_IRQ_UART                            = 0x00000010,
    DBG_SYS_IRQ_UART2                           = 0x00000011,
    DBG_SYS_IRQ_I2C                             = 0x00000012,
    DBG_SYS_IRQ_I2C2                            = 0x00000013,
    DBG_SYS_IRQ_I2C3                            = 0x00000014,
    DBG_SYS_IRQ_SCI                             = 0x00000015,
    DBG_SYS_IRQ_RF_SPI                          = 0x00000016,
    DBG_SYS_IRQ_LPS                             = 0x00000017,
    DBG_SYS_IRQ_BBIFC0                          = 0x00000018,
    DBG_SYS_IRQ_BBIFC1                          = 0x00000019,
    DBG_SYS_IRQ_USBC                            = 0x0000001A,
    DBG_SYS_IRQ_GOUDA                           = 0x0000001B,
    DBG_SYS_IRQ_SDMMC                           = 0x0000001C,
    DBG_SYS_IRQ_CAMERA                          = 0x0000001D,
    DBG_SYS_IRQ_PMU                             = 0x0000001E,
    DBG_SYS_IRQ_SDMMC2                          = 0x0000001F,
    DBG_SYS_HREADY                              = 0x00000020,
    DBG_XCPU_I_FETCH                            = 0x00000021,
    DBG_XCPU_I_MISS                             = 0x00000022,
    DBG_XCPU_D_FETCH                            = 0x00000023,
    DBG_XCPU_D_MISS                             = 0x00000024,
    DBG_XCPU_MAIN_IRQ                           = 0x00000025,
    DBG_BB_IRQ_TCU0                             = 0x00000040,
    DBG_BB_IRQ_TCU1                             = 0x00000041,
    DBG_BB_IRQ_FRAME                            = 0x00000042,
    DBG_BB_IRQ_COM0                             = 0x00000043,
    DBG_BB_IRQ_COM1                             = 0x00000044,
    DBG_BB_IRQ_VOC                              = 0x00000045,
    DBG_BB_IRQ_IFC0                             = 0x00000046,
    DBG_BB_IRQ_IFC1                             = 0x00000047,
    DBG_BB_IRQ_IFC2                             = 0x00000048,
    DBG_BB_IRQ_IFC3                             = 0x00000049,
    DBG_BB_IRQ_RF_IF                            = 0x0000004A,
    DBG_BB_IRQ_ITLV                             = 0x0000004B,
    DBG_BB_IRQ_VITAC                            = 0x0000004C,
    DBG_BB_IRQ_XCOR                             = 0x0000004D,
    DBG_SYS_IRQ_PAGE_SPY                        = 0x0000004E,
    DBG_WATCH0                                  = 0x0000004F,
    DBG_WATCH1                                  = 0x00000050,
    DBG_WATCH2                                  = 0x00000051,
    DBG_WATCH3                                  = 0x00000052,
    DBG_PLL_LOCKED                              = 0x00000053,
    DBG_RX_SOC                                  = 0x00000054,
    DBG_VOC_DMA_BUSY                            = 0x00000055,
    DBG_VOC_BUSY                                = 0x00000056,
    DBG_SYS_EBC_BUSY                            = 0x00000057,
    DBG_BB_EBC_BUSY                             = 0x00000058,
    DBG_VITAC_BUSY                              = 0x00000059,
    DBG_XCOR_BUSY                               = 0x0000005A,
    DBG_ITLV_BUSY                               = 0x0000005B,
    XCOR_STALL                                  = 0x0000005C,
    ITLV_STALL                                  = 0x0000005D,
    VITAC_STALL                                 = 0x0000005E,
    DBG_BB_HREADY                               = 0x0000005F,
    DBG_BCPU_I_FETCH                            = 0x00000060,
    DBG_BCPU_I_MISS                             = 0x00000061,
    DBG_BCPU_D_FETCH                            = 0x00000062,
    DBG_BCPU_D_MISS                             = 0x00000063,
    DBG_BCPU_MAIN_IRQ                           = 0x00000064
} DEBUG_SIGNAL_SELECT_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// DEBUG_PORT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_DEBUG_PORT_BASE         0x01A0D000

typedef volatile struct
{
    /// All this register is locked by the host_lock_reg signal
    REG32                          Debug_Port_Mode;              //0x00000000
    /// All this register is locked by the host_lock_reg signal
    REG32                          Pxts_Exl_Cfg;                 //0x00000004
    REG32                          WD_Addr_cfg;                  //0x00000008
    /// <table border="1" > <!-- border should be in css--> <tr> <th>Pin</th> <th>MODE_SYS</th>
    /// <th>MODE_IPSPI</th> <th>MODE_IPSIG</th> <th>MODE_RF</th> </tr> <tr> <td>DBG_BUS_STB</td>
    /// <td colspan="4" >FInt_H</td> </tr> <tr> <td>DBG_BUS_0</td> <td colspan="3"
    /// >GPO_8</td> <td>RxTxEN</td> </tr> <tr> <td>DBG_BUS_1</td> <td>?</td> <td>PMU_SPI3_CS</td>
    /// <td>WAKEUP</td> <td>RxTxDATA</td> </tr> <tr> <td>DBG_BUS_2</td> <td>Xcpu_Main_irq_L</td>
    /// <td>ABB_SPI3_CS</td> <td>CHG MASK</td> <td>STROBE</td> </tr> <tr> <td>DBG_BUS_3</td>
    /// <td>Bcpu_Main_irq_L</td> <td>FM_SPI3_CS</td> <td>LP_MODE</td> <td>RFSPI_CLK</td>
    /// </tr> <tr> <td>DBG_BUS_4</td> <td>Sys_IntEnabled_H</td> <td>SPI3_CLK</td>
    /// <td>PLL_PD</td> <td>RFSPI_CS</td> </tr> <tr> <td>DBG_BUS_5</td> <td>BB_IntEnabled_H</td>
    /// <td>SPI3_DO</td> <td>RF_SYSCLKEN</td> <td>RFSPI_DIO</td> </tr> <tr> <td>DBG_BUS_6</td>
    /// <td colspan="3" >Signal_Select0</td> <td>RF_PDN</td> </tr> <tr> <td>DBG_BUS_7</td>
    /// <td colspan="3" >Signal_Select1</td> <td>RX_DEC_ON</td> </tr> </table>
    REG32                          Signal_Spy_Cfg;               //0x0000000C
    REG32                          Dbg_Clk_Cfg;                  //0x00000010
    REG32                          Dbg_Watch_Set;                //0x00000014
    REG32                          Dbg_Watch_Clr;                //0x00000018
    REG32 Reserved_0000001C[9];                 //0x0000001C
    REG32                          Pxts_tag[16];                 //0x00000040
} HWP_DEBUG_PORT_T;

#define hwp_debugPort               ((HWP_DEBUG_PORT_T*) KSEG1(REG_DEBUG_PORT_BASE))


//Debug_Port_Mode
#define DEBUG_PORT_MODE_CONFIG(n)   (((n)&7)<<0)
#define DEBUG_PORT_MODE_CONFIG_MASK (7<<0)
#define DEBUG_PORT_MODE_CONFIG_SHIFT (0)
#define DEBUG_PORT_MODE_CONFIG_MODE_PXTS_EXL_OFF (0<<0)
#define DEBUG_PORT_MODE_CONFIG_MODE_EXL (1<<0)
#define DEBUG_PORT_MODE_CONFIG_MODE_PXTS (2<<0)
#define DEBUG_PORT_MODE_CONFIG_MODE_PXTS_EXL (3<<0)
#define DEBUG_PORT_MODE_CONFIG_MODE_SIGNAL_SPY (4<<0)
#define DEBUG_PORT_PXTS_EXL_CLOCK_POL (1<<3)
#define DEBUG_PORT_PXTS_FIFO_EMPTY  (1<<4)
#define DEBUG_PORT_PXTS_FIFO_OVF    (1<<5)
#define DEBUG_PORT_EXL_FIFO_EMPTY   (1<<8)
#define DEBUG_PORT_EXL_FIFO_OVF     (1<<9)
#define DEBUG_PORT_OEN_DBG_CLK      (1<<12)

//Pxts_Exl_Cfg
#define DEBUG_PORT_RECORD_TRIGGER_TYPE(n) (((n)&15)<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_MASK (15<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_SHIFT (0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_RECORD_JUMANDBRANCH (0<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_RECORD_BRANCH (1<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_RECORD_BRANCHNOTTAKEN (2<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_EXL_RECORD_BRANCHTAKEN (3<<0)
#define DEBUG_PORT_RECORD_TRIGGER_TYPE_EXL_RECORD_NOJUMNOBRANCH (4<<0)
#define DEBUG_PORT_ANY_BRANCH       (1<<4)
#define DEBUG_PORT_INT_EN           (1<<5)
#define DEBUG_PORT_CPU_SELECT_BCPU  (0<<6)
#define DEBUG_PORT_CPU_SELECT_XCPU  (1<<6)
#define DEBUG_PORT_BRANCH_TYPE(n)   (((n)&31)<<8)
#define DEBUG_PORT_BRANCH_TYPE_MASK (31<<8)
#define DEBUG_PORT_BRANCH_TYPE_SHIFT (8)
#define DEBUG_PORT_BRANCH_TYPE_BLTZ (0<<8)
#define DEBUG_PORT_BRANCH_TYPE_BGEZ (1<<8)
#define DEBUG_PORT_BRANCH_TYPE_BEQ  (2<<8)
#define DEBUG_PORT_BRANCH_TYPE_BNE  (3<<8)
#define DEBUG_PORT_BRANCH_TYPE_BLEZ (4<<8)
#define DEBUG_PORT_BRANCH_TYPE_BGTZ (5<<8)
#define DEBUG_PORT_BRANCH_TYPE_BC1F (6<<8)
#define DEBUG_PORT_BRANCH_TYPE_BC1T (7<<8)
#define DEBUG_PORT_BRANCH_TYPE_BC2F (8<<8)
#define DEBUG_PORT_BRANCH_TYPE_BC2T (9<<8)
#define DEBUG_PORT_BRANCH_TYPE_NO_BRANCH (10<<8)
#define DEBUG_PORT_BRANCH_TYPE_BR   (11<<8)
#define DEBUG_PORT_BRANCH_TYPE_BEQZ (12<<8)
#define DEBUG_PORT_BRANCH_TYPE_BNEZ (13<<8)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL(n) (((n)&3)<<14)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL_MASK (3<<14)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL_SHIFT (14)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL_BASIC_STORAGE (0<<14)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL_HW_STORAGE_CTRL (1<<14)
#define DEBUG_PORT_EXL_FIFO_STORAGE_CTRL_SW_STORAGE_CTRL (2<<14)
#define DEBUG_PORT_ENABLE_PXTS_TAG_0 (1<<16)
#define DEBUG_PORT_ENABLE_PXTS_TAG_1 (1<<17)
#define DEBUG_PORT_ENABLE_PXTS_TAG_2 (1<<18)
#define DEBUG_PORT_ENABLE_PXTS_TAG_3 (1<<19)
#define DEBUG_PORT_ENABLE_PXTS_TAG_4 (1<<20)
#define DEBUG_PORT_ENABLE_PXTS_TAG_5 (1<<21)
#define DEBUG_PORT_ENABLE_PXTS_TAG_6 (1<<22)
#define DEBUG_PORT_ENABLE_PXTS_TAG_7 (1<<23)
#define DEBUG_PORT_ENABLE_PXTS_TAG_8 (1<<24)
#define DEBUG_PORT_ENABLE_PXTS_TAG_9 (1<<25)
#define DEBUG_PORT_ENABLE_PXTS_TAG_10 (1<<26)
#define DEBUG_PORT_ENABLE_PXTS_TAG_11 (1<<27)
#define DEBUG_PORT_ENABLE_PXTS_TAG_12 (1<<28)
#define DEBUG_PORT_ENABLE_PXTS_TAG_13 (1<<29)
#define DEBUG_PORT_ENABLE_PXTS_TAG_14 (1<<30)
#define DEBUG_PORT_ENABLE_PXTS_TAG_15 (1<<31)
#define DEBUG_PORT_ENABLE_PXTS_TAG(n) (((n)&0xFFFF)<<16)
#define DEBUG_PORT_ENABLE_PXTS_TAG_MASK (0xFFFF<<16)
#define DEBUG_PORT_ENABLE_PXTS_TAG_SHIFT (16)

//WD_Addr_cfg
#define DEBUG_PORT_WD_ADDRESS(n)    (((n)&0x3FFFFFF)<<0)

//Signal_Spy_Cfg
#define DEBUG_PORT_DBG_BUS_MODE(n)  (((n)&3)<<0)
#define DEBUG_PORT_DBG_BUS_MODE_MASK (3<<0)
#define DEBUG_PORT_DBG_BUS_MODE_SHIFT (0)
#define DEBUG_PORT_DBG_BUS_MODE_DEBUG_MODE_SYS (0<<0)
#define DEBUG_PORT_DBG_BUS_MODE_DEBUG_MODE_IP_SPI (1<<0)
#define DEBUG_PORT_DBG_BUS_MODE_DEBUG_MODE_IP_SIG (2<<0)
#define DEBUG_PORT_DBG_BUS_MODE_DEBUG_MODE_RF (3<<0)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0(n) (((n)&0x7F)<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_MASK (0x7F<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_SHIFT (8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_TCU0 (0x0<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_TCU1 (0x1<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_FRAME (0x2<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_COM0 (0x3<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_COM1 (0x4<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_VOC (0x5<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_DMA (0x6<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_GPIO (0x7<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_KEYPAD (0x8<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_TIMERS (0x9<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_OS_TIMER (0xA<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_CALENDAR (0xB<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_SPI1 (0xC<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_SPI2 (0xD<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_SPI3 (0xE<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_DEBUG_UART (0xF<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_UART (0x10<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_UART2 (0x11<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_I2C (0x12<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_I2C2 (0x13<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_I2C3 (0x14<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_SCI (0x15<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_RF_SPI (0x16<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_LPS (0x17<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_BBIFC0 (0x18<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_BBIFC1 (0x19<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_USBC (0x1A<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_GOUDA (0x1B<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_SDMMC (0x1C<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_CAMERA (0x1D<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_GPADC (0x1E<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_PMU (0x1F<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_HREADY (0x20<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_I_FETCH (0x21<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_I_MISS (0x22<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_D_FETCH (0x23<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_D_MISS (0x24<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCPU_MAIN_IRQ (0x25<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_TCU0 (0x40<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_TCU1 (0x41<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_FRAME (0x42<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_COM0 (0x43<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_COM1 (0x44<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_VOC (0x45<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_IFC0 (0x46<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_IFC1 (0x47<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_IFC2 (0x48<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_IFC3 (0x49<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_RF_IF (0x4A<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_ITLV (0x4B<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_VITAC (0x4C<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_IRQ_XCOR (0x4D<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_IRQ_PAGE_SPY (0x4E<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_WATCH0 (0x4F<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_WATCH1 (0x50<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_WATCH2 (0x51<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_WATCH3 (0x52<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_PLL_LOCKED (0x53<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_RX_SOC (0x54<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_VOC_DMA_BUSY (0x55<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_VOC_BUSY (0x56<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_SYS_EBC_BUSY (0x57<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_EBC_BUSY (0x58<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_VITAC_BUSY (0x59<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_XCOR_BUSY (0x5A<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_ITLV_BUSY (0x5B<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_XCOR_STALL (0x5C<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_ITLV_STALL (0x5D<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_VITAC_STALL (0x5E<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BB_HREADY (0x5F<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BCPU_I_FETCH (0x60<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BCPU_I_MISS (0x61<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BCPU_D_FETCH (0x62<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BCPU_D_MISS (0x63<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL0_DBG_BCPU_MAIN_IRQ (0x64<<8)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1(n) (((n)&0x7F)<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_MASK (0x7F<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_SHIFT (16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_TCU0 (0x0<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_TCU1 (0x1<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_FRAME (0x2<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_COM0 (0x3<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_COM1 (0x4<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_VOC (0x5<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_DMA (0x6<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_GPIO (0x7<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_KEYPAD (0x8<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_TIMERS (0x9<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_OS_TIMER (0xA<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_CALENDAR (0xB<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SPI1 (0xC<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SPI2 (0xD<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SPI3 (0xE<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_DEBUG_UART (0xF<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_UART (0x10<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_UART2 (0x11<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_I2C (0x12<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_I2C2 (0x13<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_I2C3 (0x14<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SCI (0x15<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_RF_SPI (0x16<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_LPS (0x17<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_BBIFC0 (0x18<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_BBIFC1 (0x19<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_USBC (0x1A<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_GOUDA (0x1B<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SDMMC (0x1C<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_CAMERA (0x1D<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_PMU (0x1E<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_SDMMC2 (0x1F<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_HREADY (0x20<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCPU_I_FETCH (0x21<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCPU_I_MISS (0x22<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCPU_D_FETCH (0x23<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCPU_D_MISS (0x24<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCPU_MAIN_IRQ (0x25<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_TCU0 (0x40<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_TCU1 (0x41<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_FRAME (0x42<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_COM0 (0x43<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_COM1 (0x44<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_VOC (0x45<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_IFC0 (0x46<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_IFC1 (0x47<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_IFC2 (0x48<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_IFC3 (0x49<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_RF_IF (0x4A<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_ITLV (0x4B<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_VITAC (0x4C<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_IRQ_XCOR (0x4D<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_IRQ_PAGE_SPY (0x4E<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_WATCH0 (0x4F<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_WATCH1 (0x50<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_WATCH2 (0x51<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_WATCH3 (0x52<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_PLL_LOCKED (0x53<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_RX_SOC (0x54<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_VOC_DMA_BUSY (0x55<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_VOC_BUSY (0x56<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_SYS_EBC_BUSY (0x57<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_EBC_BUSY (0x58<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_VITAC_BUSY (0x59<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_XCOR_BUSY (0x5A<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_ITLV_BUSY (0x5B<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_XCOR_STALL (0x5C<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_ITLV_STALL (0x5D<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_VITAC_STALL (0x5E<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BB_HREADY (0x5F<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BCPU_I_FETCH (0x60<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BCPU_I_MISS (0x61<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BCPU_D_FETCH (0x62<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BCPU_D_MISS (0x63<<16)
#define DEBUG_PORT_DBG_BUS_SIG_SEL1_DBG_BCPU_MAIN_IRQ (0x64<<16)

//Dbg_Clk_Cfg
#define DEBUG_PORT_SEL_SOURCE(n)    (((n)&0x3F)<<0)
#define DEBUG_PORT_SEL_SOURCE_MASK  (0x3F<<0)
#define DEBUG_PORT_SEL_SOURCE_SHIFT (0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYS_NOGATE (0x0<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_SCI (0x1<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_SPI (0x2<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_SPI2 (0x3<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_SPI3 (0x4<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_RF_SPI (0x5<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_OSC (0x6<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_PWM (0x7<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_XCPU (0x8<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYSD_DAIS_208K (0x9<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_HOST (0xA<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_AUDIO_PLL (0xB<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_AUDIO_BCK (0xC<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_CLK_DBG_PLL (0xD<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_CLK_DBG (0xE<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_UART (0xF<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_UART2 (0x10<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_RF_RX (0x11<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_RF_TX (0x12<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_MEM_BRIDGE (0x13<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_VOC (0x14<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_32K (0x15<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_USBPHY (0x16<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_CAMERA (0x17<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_PIX (0x18<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_DIG52M (0x19<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_OSC (0x1A<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_REF (0x1B<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SLOW (0x1C<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_FAST_PLL (0x1D<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_VOC_PLL (0x1E<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_SYS_156M_PLL (0x1F<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_BB_PLL (0x20<<0)
#define DEBUG_PORT_SEL_SOURCE_DBG_SEL_TCU (0x21<<0)
#define DEBUG_PORT_ENABLE_DBG_MASKED (0<<20)
#define DEBUG_PORT_ENABLE_DBG_ENABLED (1<<20)

//Dbg_Watch_Set
#define DEBUG_PORT_DBG_WATCH_S_C(n) (((n)&15)<<0)

//Dbg_Watch_Clr
//#define DEBUG_PORT_DBG_WATCH_S_C(n) (((n)&15)<<0)

//Pxts_tag
#define DEBUG_PORT_TAG_PXTS(n)      (((n)&0xFFFF)<<0)





#endif

