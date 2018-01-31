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

#ifndef _DEBUG_PORT_ASM_H_
#define _DEBUG_PORT_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'debug_port'."
#endif


#define DBG_SEL_SYS_NOGATE                       (0)
#define DBG_SEL_SYSD_SCI                         (1)
#define DBG_SEL_SYSD_SPI                         (2)
#define DBG_SEL_SYSD_SPI2                        (3)
#define DBG_SEL_SYSD_SPI3                        (4)
#define DBG_SEL_SYSD_RF_SPI                      (5)
#define DBG_SEL_SYSD_OSC                         (6)
#define DBG_SEL_SYSD_PWM                         (7)
#define DBG_SEL_XCPU                             (8)
#define DBG_SEL_SYSD_DAIS_208K                   (9)
#define DBG_SEL_HOST                             (10)
#define DBG_SEL_AUDIO_PLL                        (11)
#define DBG_SEL_AUDIO_BCK                        (12)
#define DBG_SEL_CLK_DBG_PLL                      (13)
#define DBG_SEL_CLK_DBG                          (14)
#define DBG_SEL_UART                             (15)
#define DBG_SEL_UART2                            (16)
#define DBG_SEL_RF_RX                            (17)
#define DBG_SEL_RF_TX                            (18)
#define DBG_SEL_MEM_BRIDGE                       (19)
#define DBG_SEL_VOC                              (20)
#define DBG_SEL_32K                              (21)
#define DBG_SEL_USBPHY                           (22)
#define DBG_SEL_CAMERA                           (23)
#define DBG_SEL_PIX                              (24)
#define DBG_SEL_DIG52M                           (25)
/// this clock is taken before clock root
#define DBG_SEL_OSC                              (26)
/// this clock is taken before clock root
#define DBG_SEL_REF                              (27)
/// this clock is taken before clock root
#define DBG_SEL_SLOW                             (28)
/// this clock is taken before clock root
#define DBG_SEL_FAST_PLL                         (29)
/// this clock is taken before clock root
#define DBG_SEL_VOC_PLL                          (30)
/// this clock is taken before clock root
#define DBG_SEL_SYS_156M_PLL                     (31)
#define DBG_SEL_BB_PLL                           (32)
#define DBG_SEL_TCU                              (33)
#define NB_CLK_DBG_SEL                           (34)
#define DBG_SYS_IRQ_TCU0                         (0)
#define DBG_SYS_IRQ_TCU1                         (1)
#define DBG_SYS_IRQ_FRAME                        (2)
#define DBG_SYS_IRQ_COM0                         (3)
#define DBG_SYS_IRQ_COM1                         (4)
#define DBG_SYS_IRQ_VOC                          (5)
#define DBG_SYS_IRQ_DMA                          (6)
#define DBG_SYS_IRQ_GPIO                         (7)
#define DBG_SYS_IRQ_KEYPAD                       (8)
#define DBG_SYS_IRQ_TIMERS                       (9)
#define DBG_SYS_IRQ_OS_TIMER                     (10)
#define DBG_SYS_IRQ_CALENDAR                     (11)
#define DBG_SYS_IRQ_SPI1                         (12)
#define DBG_SYS_IRQ_SPI2                         (13)
#define DBG_SYS_IRQ_SPI3                         (14)
#define DBG_SYS_IRQ_DEBUG_UART                   (15)
#define DBG_SYS_IRQ_UART                         (16)
#define DBG_SYS_IRQ_UART2                        (17)
#define DBG_SYS_IRQ_I2C                          (18)
#define DBG_SYS_IRQ_I2C2                         (19)
#define DBG_SYS_IRQ_I2C3                         (20)
#define DBG_SYS_IRQ_SCI                          (21)
#define DBG_SYS_IRQ_RF_SPI                       (22)
#define DBG_SYS_IRQ_LPS                          (23)
#define DBG_SYS_IRQ_BBIFC0                       (24)
#define DBG_SYS_IRQ_BBIFC1                       (25)
#define DBG_SYS_IRQ_USBC                         (26)
#define DBG_SYS_IRQ_GOUDA                        (27)
#define DBG_SYS_IRQ_SDMMC                        (28)
#define DBG_SYS_IRQ_CAMERA                       (29)
#define DBG_SYS_IRQ_PMU                          (30)
#define DBG_SYS_IRQ_SDMMC2                       (31)
#define DBG_SYS_HREADY                           (32)
#define DBG_XCPU_I_FETCH                         (33)
#define DBG_XCPU_I_MISS                          (34)
#define DBG_XCPU_D_FETCH                         (35)
#define DBG_XCPU_D_MISS                          (36)
#define DBG_XCPU_MAIN_IRQ                        (37)
#define DBG_BB_IRQ_TCU0                          (64)
#define DBG_BB_IRQ_TCU1                          (65)
#define DBG_BB_IRQ_FRAME                         (66)
#define DBG_BB_IRQ_COM0                          (67)
#define DBG_BB_IRQ_COM1                          (68)
#define DBG_BB_IRQ_VOC                           (69)
#define DBG_BB_IRQ_IFC0                          (70)
#define DBG_BB_IRQ_IFC1                          (71)
#define DBG_BB_IRQ_IFC2                          (72)
#define DBG_BB_IRQ_IFC3                          (73)
#define DBG_BB_IRQ_RF_IF                         (74)
#define DBG_BB_IRQ_ITLV                          (75)
#define DBG_BB_IRQ_VITAC                         (76)
#define DBG_BB_IRQ_XCOR                          (77)
#define DBG_SYS_IRQ_PAGE_SPY                     (78)
#define DBG_WATCH0                               (79)
#define DBG_WATCH1                               (80)
#define DBG_WATCH2                               (81)
#define DBG_WATCH3                               (82)
#define DBG_PLL_LOCKED                           (83)
#define DBG_RX_SOC                               (84)
#define DBG_VOC_DMA_BUSY                         (85)
#define DBG_VOC_BUSY                             (86)
#define DBG_SYS_EBC_BUSY                         (87)
#define DBG_BB_EBC_BUSY                          (88)
#define DBG_VITAC_BUSY                           (89)
#define DBG_XCOR_BUSY                            (90)
#define DBG_ITLV_BUSY                            (91)
#define XCOR_STALL                               (92)
#define ITLV_STALL                               (93)
#define VITAC_STALL                              (94)
#define DBG_BB_HREADY                            (95)
#define DBG_BCPU_I_FETCH                         (96)
#define DBG_BCPU_I_MISS                          (97)
#define DBG_BCPU_D_FETCH                         (98)
#define DBG_BCPU_D_MISS                          (99)
#define DBG_BCPU_MAIN_IRQ                        (100)

//==============================================================================
// Debug_Mux_Clocks
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// Debug_Signal_Select
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// debug_port
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_DEBUG_PORT_BASE         0x01A0D000

#define REG_DEBUG_PORT_BASE_HI      BASE_HI(REG_DEBUG_PORT_BASE)
#define REG_DEBUG_PORT_BASE_LO      BASE_LO(REG_DEBUG_PORT_BASE)

#define REG_DEBUG_PORT_DEBUG_PORT_MODE REG_DEBUG_PORT_BASE_LO + 0x00000000
#define REG_DEBUG_PORT_PXTS_EXL_CFG REG_DEBUG_PORT_BASE_LO + 0x00000004
#define REG_DEBUG_PORT_WD_ADDR_CFG  REG_DEBUG_PORT_BASE_LO + 0x00000008
#define REG_DEBUG_PORT_SIGNAL_SPY_CFG REG_DEBUG_PORT_BASE_LO + 0x0000000C
#define REG_DEBUG_PORT_DBG_CLK_CFG  REG_DEBUG_PORT_BASE_LO + 0x00000010
#define REG_DEBUG_PORT_DBG_WATCH_SET REG_DEBUG_PORT_BASE_LO + 0x00000014
#define REG_DEBUG_PORT_DBG_WATCH_CLR REG_DEBUG_PORT_BASE_LO + 0x00000018
#define REG_DEBUG_PORT_PXTS_TAG_0   REG_DEBUG_PORT_BASE_LO + 0x00000040
#define REG_DEBUG_PORT_PXTS_TAG_1   REG_DEBUG_PORT_BASE_LO + 0x00000044
#define REG_DEBUG_PORT_PXTS_TAG_2   REG_DEBUG_PORT_BASE_LO + 0x00000048
#define REG_DEBUG_PORT_PXTS_TAG_3   REG_DEBUG_PORT_BASE_LO + 0x0000004C
#define REG_DEBUG_PORT_PXTS_TAG_4   REG_DEBUG_PORT_BASE_LO + 0x00000050
#define REG_DEBUG_PORT_PXTS_TAG_5   REG_DEBUG_PORT_BASE_LO + 0x00000054
#define REG_DEBUG_PORT_PXTS_TAG_6   REG_DEBUG_PORT_BASE_LO + 0x00000058
#define REG_DEBUG_PORT_PXTS_TAG_7   REG_DEBUG_PORT_BASE_LO + 0x0000005C
#define REG_DEBUG_PORT_PXTS_TAG_8   REG_DEBUG_PORT_BASE_LO + 0x00000060
#define REG_DEBUG_PORT_PXTS_TAG_9   REG_DEBUG_PORT_BASE_LO + 0x00000064
#define REG_DEBUG_PORT_PXTS_TAG_10  REG_DEBUG_PORT_BASE_LO + 0x00000068
#define REG_DEBUG_PORT_PXTS_TAG_11  REG_DEBUG_PORT_BASE_LO + 0x0000006C
#define REG_DEBUG_PORT_PXTS_TAG_12  REG_DEBUG_PORT_BASE_LO + 0x00000070
#define REG_DEBUG_PORT_PXTS_TAG_13  REG_DEBUG_PORT_BASE_LO + 0x00000074
#define REG_DEBUG_PORT_PXTS_TAG_14  REG_DEBUG_PORT_BASE_LO + 0x00000078
#define REG_DEBUG_PORT_PXTS_TAG_15  REG_DEBUG_PORT_BASE_LO + 0x0000007C

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
