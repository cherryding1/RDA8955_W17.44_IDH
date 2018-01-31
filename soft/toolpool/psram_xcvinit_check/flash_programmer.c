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

#include "flash_prog_map.h"

#include "tgt_memd_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"
#include "mem_bridge.h"

#include "hal_clk.h"
#include "sys_ctrl.h"

#include "hal_mem_map.h"
#include "cfg_regs.h"
#include "boot_ispi.h"
#include "spi.h"
HAL_DMA_CFG_T ram_dmaCfg;


// define the folowing to see the error code returned by the flash driver as host events
#define DEBUG_EVENTS
#ifdef DEBUG_EVENTS
#include "hal_host.h"
#endif

extern VOID mon_Event(UINT32 ch);
extern VOID boot_HostUsbEvent(UINT32 ch);

#ifdef CHIP_HAS_AP
#define hwp_ispi hwp_spi2
#define hwp_apIspi hwp_apSpi3
#else
#define hwp_ispi hwp_spi3
#endif

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
//  #define SEND_EVENT boot_HostUsbEvent
#else
//  #define SEND_EVENT mon_Event
#endif

//#define SEND_EVENT hal_HstSendEvent

#define SEND_EVENT(event)


#ifndef FLASH_USB_SEND_EVENT_DELAY
#define FLASH_USB_SEND_EVENT_DELAY    8192 // 0.5s
#endif /* FLASH_USB_SEND_EVENT_DELAY */

#define FP_DATA_BUFFER_LOCATION __attribute__((section(".ramucbss")))

BOOT_HOST_USB_MONITOR_CTX_T g_flashProgUsbContext;

//-----------------------------
// Three Data Buffers are available for CoolWatcher
//-----------------------------
#if (FPC_BUFFER_SIZE/4*4 != FPC_BUFFER_SIZE)
#error "FPC_BUFFER_SIZE should be aligned with word boundary"
#endif

PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferA[FPC_BUFFER_SIZE/4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferB[FPC_BUFFER_SIZE/4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferC[FPC_BUFFER_SIZE/4];

EXPORT PUBLIC UINT32 _boot_loader_magic_number_in_flash;

PROTECTED HAL_MAP_VERSION_T g_flashProgVersion = {0, 0x20080121, 0, "Flash Programmer Version 1"};
PUBLIC FLASH_PROG_MAP_ACCESS_T g_flashProgDescr =
{   {FPC_PROTOCOL_MAJOR,FPC_PROTOCOL_MINOR},{{FPC_NONE, (UINT32)0, 0, 0} ,{FPC_NONE, (UINT32)0, 0, 0}},
    (UINT8*)g_dataBufferA, (UINT8*)g_dataBufferB, (UINT8*)g_dataBufferC, FPC_BUFFER_SIZE
};

#if !(CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
#error "error: You can t use FASTPF_USE_USB without CHIP_HAS_USB"
#endif

// =============================================================================
// FLASH_PROGRAMMER_SPECIAL_SECTOR_T
// -----------------------------------------------------------------------------
/// Describe the special sector (use a magic number the first word)
// =============================================================================
typedef struct
{
    UINT8* address;
    UINT32 magicNumber;
} FLASH_PROGRAMMER_SPECIAL_SECTOR_T;

// =============================================================================
// HAL_RFSPI_CFG_T
// -----------------------------------------------------------------------------
/// RFSPI configuration structure, used to set the SPI configuration.
// =============================================================================
typedef enum  {
    HAL_RFSPI_HALF_CLK_PERIOD_0,
    HAL_RFSPI_HALF_CLK_PERIOD_1,
    HAL_RFSPI_HALF_CLK_PERIOD_2,
    HAL_RFSPI_HALF_CLK_PERIOD_3,

    HAL_RFSPI_HALF_CLK_PERIOD_QTY
} HAL_RFSPI_DELAY_T;
typedef struct  {
    /// Enable the RFSPI ?
    BOOL enable;
    /// Is the Chip Select active high
    BOOL csActiveHi;
    /// @todo fill
    BOOL modeDigRFRead;
    BOOL modeClockedBack2back;
    /// When set to 1 the inputs are activated, else only
    /// the output is driven and no data are stored in the receive FIFO.
    BOOL inputEn;
    /// The spi clock polarity: \n
    /// - when '0' the clock disabled level is low, and the first edge is
    /// a rising edge.
    /// - When '1' the clock disabled level is high, and the first edge
    /// is a falling edge.
    BOOL clkFallEdge;
    /// Transfert start to first edge delay
    HAL_RFSPI_DELAY_T clkDelay;
    /// Transfert start to first data out delay
    HAL_RFSPI_DELAY_T doDelay;
    /// Transfer start to first data in sample delay
    HAL_RFSPI_DELAY_T diDelay;
    /// Transfer start to chip select activation delay
    HAL_RFSPI_DELAY_T csDelay;
    /// Chip select deactivation to reactivation minimum delay
    HAL_RFSPI_DELAY_T csEndHold;
    /// @todo fill
    HAL_RFSPI_DELAY_T csEndPulse;
    /// @todo fill
    UINT8 turnAroundCycles;
    /// Frame size
    UINT32 frameSize;
    /// @todo fill
    UINT32 inputFrameSize;
} HAL_RFSPI_CFG_T;
const HAL_RFSPI_CFG_T g_spiCfg = {
    TRUE,  // Enable
    TRUE, // csActiveHi
    TRUE,  // modeDigRfRead
    FALSE, // modeClockBack2Back
    FALSE, // inputEn
    FALSE, // clkFallEdge
    HAL_RFSPI_HALF_CLK_PERIOD_1, // clkDelay
    HAL_RFSPI_HALF_CLK_PERIOD_2, // doDelay
    HAL_RFSPI_HALF_CLK_PERIOD_3, // diDelay
    HAL_RFSPI_HALF_CLK_PERIOD_2, // csDelay
    HAL_RFSPI_HALF_CLK_PERIOD_0, // csEndHold
    HAL_RFSPI_HALF_CLK_PERIOD_3, // csEndPulse
    2,  // turnAroundCycles
    24, // frameSize
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    17, // inputFrameSize
#else // 8810, 8809P or later
    15, // inputFrameSize
#endif
};
static CONST UINT32 pattern[] = { 0x4f35b7da, 0x8e354c91, };
//static CONST UINT32 pattern[] = { 0xa5a5a5a5, 0x5a5a5a5a, 0x4f35b7da, 0x8e354c91, };

UINT32 g_dataBuffer_start,psram_check_size,ret_flag=0;

// Check whether flash programmer data buffer is accessible
PROTECTED BOOL fp_CheckDataBuffer(VOID)
{
    int i, j;
    static VOLATILE UINT32* CONST dataAry[] = {
        (VOLATILE UINT32*)&g_dataBufferA[0],
        (VOLATILE UINT32*)&g_dataBufferA[FPC_BUFFER_SIZE/4 - 1],
        (VOLATILE UINT32*)&g_dataBufferB[0],
        (VOLATILE UINT32*)&g_dataBufferB[FPC_BUFFER_SIZE/4 - 1],
        (VOLATILE UINT32*)&g_dataBufferC[0],
        (VOLATILE UINT32*)&g_dataBufferC[FPC_BUFFER_SIZE/4 - 1],
    };

    for (i=0; i<sizeof(dataAry)/sizeof(dataAry[0]); i++)
    {
        for (j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        {
            *(dataAry[i]) = pattern[j];
            if (*(dataAry[i]) != pattern[j])
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}


BOOL fp_CheckPsramData(UINT32  flag, UINT32 g_dataBuffer_start,UINT32 psram_check_size);


PROTECTED BOOL fp_CheckDataBuffer_HIGH_ADDR(VOID)
{

#ifndef NO_PSRAM_HIGH_ADDR_CHECK
    typedef struct
    {
        UINT32 startAddr;
        UINT32 len;
    } PSRAM_RANGE_T;

    UINT32 psramBase = (UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);
    PSRAM_RANGE_T range[] = {
        { psramBase+0xc0000, 0x10000, },
        { psramBase+0xd0000, 0x10000, },
        { psramBase+0xe0000, 0x10000, },
        { psramBase+0xf0000, 0x10000, },
    };

    range[0].startAddr=(psram_check_size-0x100000)|range[0].startAddr;
    range[1].startAddr=(psram_check_size-0x100000)|range[1].startAddr;

    range[2].startAddr=(psram_check_size-0x100000)|range[2].startAddr;
    range[3].startAddr=(psram_check_size-0x100000)|range[3].startAddr;




    for (int i=0; i<sizeof(range)/sizeof(range[0]); i++)
    {

        for (int j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
        {
            if(!fp_CheckPsramData(pattern[j],range[i].startAddr,range[i].startAddr+range[i].len))
                return FALSE;
        }

        /*
           for (psramAddr=range[i].startAddr;
           psramAddr+4<=range[i].startAddr+range[i].len;
           psramAddr+=4)
           {
           for (int j=0; j<sizeof(pattern)/sizeof(pattern[0]); j++)
           {
         *(VOLATILE UINT32*)psramAddr = pattern[j];
         if (*(VOLATILE UINT32*)psramAddr != pattern[j])
         {
         SEND_EVENT(psramAddr);
         return FALSE;
         }
         }
         }
         */

    }
#endif
    return TRUE;
}








PROTECTED BOOL fp_CheckPsramData_noDMA(UINT32  flag, UINT32 dataBuffer_start,UINT32 dataBuffer_end)
{
    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        *(volatile UINT32 *)((UINT32)i) = flag;
    }

    for (int i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {
            SEND_EVENT(0x88500000|(flag&0xff));
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            ret_flag =1;
            *(volatile UINT32 *)(0x81c00020)=flag;
            *(volatile UINT32 *)(0x81c00024)=i;
            *(volatile UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(volatile UINT32 *)(0x81c00030)=dataBuffer_start;
            *(volatile UINT32 *)(0x81c00034)=dataBuffer_end;
            return FALSE;
        }
    }
    return TRUE;
}
#if 1
#define RDA6220_ONE_CMD_BYTE_QTY        4

#ifdef CHIP_HAS_AP
#define RDA6220_ADDR_MASK               0xFF
#define RDA6220_ADDR_OFFSET             23
#define RDA6220_DATA_MASK               0xFFFF
#define RDA6220_DATA_OFFSET             7
#else
#define RDA6220_ADDR_MASK               0x3F
#define RDA6220_ADDR_OFFSET             25
#define RDA6220_DATA_MASK               0x3FFFF
#define RDA6220_DATA_OFFSET             7
#endif

#define BYTE_SIZE                       8
#define BYTE_MASK                       0xff

#define RDA6220_WRITE_FMT(addr, data) (((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET) |\
        ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET))

#define RDA6220_READ_FMT(addr, data)  ( ((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET)|\
        ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET)|\
        1<<31);

#define REG_RF_SPI_BASE            0x01A0C000

typedef volatile struct
{
    REG32                          Ctrl;                         //0x00000000
    REG32                          Status;                       //0x00000004
    REG32                          Rx_Data;                      //0x00000008
    REG32                          Command;                      //0x0000000C
    REG32                          Cmd_Size;                     //0x00000010
    REG32                          Cmd_Data;                     //0x00000014
    REG32                          Gain_Size;                    //0x00000018
    REG32                          Gain_Data;                    //0x0000001C
    REG32                          IRQ;                          //0x00000020
    REG32                          IRQ_Mask;                     //0x00000024
    REG32                          IRQ_Threshold;                //0x00000028
    REG32                          Divider;                      //0x0000002C
} HWP_RF_SPI_T;

//Ctrl
#define RF_SPI_ENABLE              (1<<0)
#define RF_SPI_CS_POLARITY         (1<<1)
#define RF_SPI_DIGRF_READ          (1<<2)
#define RF_SPI_CLOCKED_BACK2BACK   (1<<3)
#define RF_SPI_INPUT_MODE          (1<<4)
#define RF_SPI_CLOCK_POLARITY      (1<<5)
#define RF_SPI_CLOCK_DELAY(n)      (((n)&3)<<6)
#define RF_SPI_DO_DELAY(n)         (((n)&3)<<8)
#define RF_SPI_DI_DELAY(n)         (((n)&3)<<10)
#define RF_SPI_CS_DELAY(n)         (((n)&3)<<12)
#define RF_SPI_CS_END_HOLD(n)      (((n)&3)<<14)
#define RF_SPI_FRAME_SIZE(n)       (((n)&31)<<16)
#define RF_SPI_FRAME_SIZE_MASK     (31<<16)
#define RF_SPI_FRAME_SIZE_SHIFT    (16)
#define RF_SPI_CS_END_PULSE(n)     (((n)&3)<<22)
#define RF_SPI_INPUT_FRAME_SIZE(n) (((n)&31)<<24)
#define RF_SPI_INPUT_FRAME_SIZE_MASK (31<<24)
#define RF_SPI_INPUT_FRAME_SIZE_SHIFT (24)
#define RF_SPI_TURNAROUND_TIME(n)  (((n)&3)<<30)

//Status
#define RF_SPI_ACTIVE_STATUS       (1<<0)
#define RF_SPI_ERROR_CMD           (1<<1)
#define RF_SPI_TABLE_OVF           (1<<6)
#define RF_SPI_TABLE_UDF           (1<<7)
#define RF_SPI_CMD_LEVEL(n)        (((n)&31)<<8)
#define RF_SPI_CMD_LEVEL_MASK      (31<<8)
#define RF_SPI_CMD_LEVEL_SHIFT     (8)
#define RF_SPI_CMD_OVF             (1<<14)
#define RF_SPI_CMD_UDF             (1<<15)
#define RF_SPI_CMD_DATA_LEVEL(n)   (((n)&31)<<16)
#define RF_SPI_CMD_DATA_LEVEL_MASK (31<<16)
#define RF_SPI_CMD_DATA_LEVEL_SHIFT (16)
#define RF_SPI_CMD_DATA_OVF        (1<<22)
#define RF_SPI_CMD_DATA_UDF        (1<<23)
#define RF_SPI_RX_LEVEL(n)         (((n)&7)<<24)
#define RF_SPI_RX_LEVEL_MASK       (7<<24)
#define RF_SPI_RX_LEVEL_SHIFT      (24)
#define RF_SPI_RX_OVF              (1<<30)
#define RF_SPI_RX_UDF              (1<<31)

//Rx_Data
#define RF_SPI_RX_DATA(n)          (((n)&0xFF)<<0)

//Command
#define RF_SPI_SEND_CMD            (1<<0)
#define RF_SPI_FLUSH_CMD_FIFO      (1<<8)
#define RF_SPI_FLUSH_RX_FIFO       (1<<16)
#define RF_SPI_RESTART_GAIN        (1<<24)
#define RF_SPI_RELOAD_GAIN         (1<<28)
#define RF_SPI_DRIVE_ZERO          (1<<31)

//Cmd_Size
#define RF_SPI_CMD_SIZE(n)         (((n)&0xFF)<<0)
#define RF_SPI_CMD_MARK            (1<<31)

//Cmd_Data
#define RF_SPI_CMD_DATA(n)         (((n)&0xFF)<<0)

//Gain_Size
#define RF_SPI_GAIN_SIZE(n)        (((n)&15)<<0)

//Gain_Data
#define RF_SPI_GAIN_DATA(n)        (((n)&0xFF)<<0)

//IRQ
#define RF_SPI_CMD_DATA_DMA_DONE_CAUSE (1<<0)
#define RF_SPI_CMD_FIFO_EMPTY_CAUSE (1<<2)
#define RF_SPI_CMD_THRESHOLD_CAUSE (1<<3)
#define RF_SPI_RX_FIFO_FULL_CAUSE  (1<<4)
#define RF_SPI_RX_THRESHOLD_CAUSE  (1<<5)
#define RF_SPI_ERROR_CAUSE         (1<<6)
#define RF_SPI_CMD_DATA_DMA_DONE_STATUS (1<<16)
#define RF_SPI_CMD_FIFO_EMPTY_STATUS (1<<18)
#define RF_SPI_CMD_THRESHOLD_STATUS (1<<19)
#define RF_SPI_RX_FIFO_FULL_STATUS (1<<20)
#define RF_SPI_RX_THRESHOLD_STATUS (1<<21)
#define RF_SPI_ERROR_STATUS        (1<<22)
#define RF_SPI_ALL_CAUSE(n)        (((n)&0x7D)<<0)
#define RF_SPI_ALL_CAUSE_MASK      (0x7D<<0)
#define RF_SPI_ALL_CAUSE_SHIFT     (0)
#define RF_SPI_ALL_STATUS(n)       (((n)&0x7D)<<16)
#define RF_SPI_ALL_STATUS_MASK     (0x7D<<16)
#define RF_SPI_ALL_STATUS_SHIFT    (16)

//IRQ_Mask
#define RF_SPI_CMD_DATA_DMA_DONE_MASK (1<<0)
#define RF_SPI_CMD_FIFO_EMPTY_MASK (1<<2)
#define RF_SPI_CMD_THRESHOLD_MASK  (1<<3)
#define RF_SPI_RX_FIFO_FULL_MASK   (1<<4)
#define RF_SPI_RX_THRESHOLD_MASK   (1<<5)
#define RF_SPI_ERROR_MASK          (1<<6)
#define RF_SPI_ALL_MASK(n)         (((n)&0x7D)<<0)
#define RF_SPI_ALL_MASK_MASK       (0x7D<<0)
#define RF_SPI_ALL_MASK_SHIFT      (0)

//IRQ_Threshold
#define RF_SPI_CMD_THRESHOLD(n)    (((n)&31)<<8)
#define RF_SPI_RX_THRESHOLD(n)     (((n)&3)<<24)

//Divider
#define RF_SPI_DIVIDER(n)          (((n)&0x3F)<<1)
#define RF_SPI_DIVIDER_MASK        (0x3F<<1)
#define RF_SPI_DIVIDER_SHIFT       (1)
#define RF_SPI_CLOCK_LIMITER       (1<<28)
#define RF_SPI_CLOCK_LIMITER_MASK  (1<<28)
#define RF_SPI_CLOCK_LIMITER_SHIFT (28)

#define EXP2(n) (1<<(n))

#define CMD_FIFO_LEN_BITS                       (5)
#define CMD_FIFO_LEN                            (20)
#define CMD_SIZE_BITS                           (8)
#define CMD_DATA_FIFO_LEN_BITS                  (4)
#define CMD_DATA_FIFO_LEN                       (EXP2(CMD_DATA_FIFO_LEN_BITS))
#define GAIN_TABLE_LEN_BITS                     (4)
#define GAIN_TABLE_LEN                          (15)
#define GAIN_SIZE_BITS                          (4)
#define RX_DATA_FIFO_LEN_BITS                   (2)
#define RX_DATA_FIFO_LEN                        (EXP2(RX_DATA_FIFO_LEN_BITS))
#define hwp_rfSpi                  ((HWP_RF_SPI_T*) KSEG1(REG_RF_SPI_BASE))
#define HAL_RFSPI_MIN_FRAME_SIZE         3
#define HAL_RFSPI_MAX_FRAME_SIZE         31
#undef HAL_ASSERT
#define HAL_ASSERT(cond, str, ...)      do { if (!(cond)) *(UINT32 *)0=0; } while (0)
VOID rfspiOpen(CONST HAL_RFSPI_CFG_T *cfg)
{
    //  Checkers
    HAL_ASSERT((UINT32)cfg->clkDelay < HAL_RFSPI_HALF_CLK_PERIOD_3,       "clkDelay=%d",          cfg->clkDelay        );
    HAL_ASSERT((UINT32)cfg->doDelay < HAL_RFSPI_HALF_CLK_PERIOD_3,        "doDelay=%d",           cfg->doDelay         );
    HAL_ASSERT((UINT32)cfg->diDelay < HAL_RFSPI_HALF_CLK_PERIOD_QTY,      "diDelay=%d",           cfg->diDelay         );
    HAL_ASSERT((UINT32)cfg->csDelay < HAL_RFSPI_HALF_CLK_PERIOD_QTY,      "csDelay=%d",           cfg->csDelay         );
    HAL_ASSERT((UINT32)cfg->csEndHold < HAL_RFSPI_HALF_CLK_PERIOD_QTY,    "csEndHold=%d",         cfg->csEndHold       );
    HAL_ASSERT((UINT32)cfg->csEndPulse < HAL_RFSPI_HALF_CLK_PERIOD_QTY,   "csEndPulse=%d",        cfg->csEndPulse      );
    HAL_ASSERT((UINT32)cfg->turnAroundCycles < 4,                         "turnAroundCycles=%d",  cfg->turnAroundCycles);
    HAL_ASSERT((cfg->frameSize>=HAL_RFSPI_MIN_FRAME_SIZE)
               && (cfg->frameSize<=HAL_RFSPI_MAX_FRAME_SIZE),                    "frameSize=%d",         cfg->frameSize       );
    HAL_ASSERT((!cfg->inputEn)
               || ((cfg->inputFrameSize>=HAL_RFSPI_MIN_FRAME_SIZE)
                   && (cfg->inputFrameSize<=HAL_RFSPI_MAX_FRAME_SIZE)),         "inputFrameSize=%d",    cfg->inputFrameSize  );

    //  Setter
    hwp_rfSpi->Ctrl = ((cfg->enable?RF_SPI_ENABLE:0)
                       | (cfg->csActiveHi?0:RF_SPI_CS_POLARITY)
                       | (cfg->modeDigRFRead?RF_SPI_DIGRF_READ:0)
                       | (cfg->modeClockedBack2back?RF_SPI_CLOCKED_BACK2BACK:0)
                       | (cfg->inputEn?RF_SPI_INPUT_MODE:0)
                       | (cfg->clkFallEdge?RF_SPI_CLOCK_POLARITY:0)
                       | RF_SPI_CLOCK_DELAY(cfg->clkDelay)
                       | RF_SPI_DO_DELAY(cfg->doDelay)
                       | RF_SPI_DI_DELAY(cfg->diDelay)
                       | RF_SPI_CS_DELAY(cfg->csDelay)
                       | RF_SPI_CS_END_HOLD(cfg->csEndHold)
                       | RF_SPI_CS_END_PULSE(cfg->csEndPulse)
                       | RF_SPI_FRAME_SIZE(cfg->frameSize)
                       | RF_SPI_INPUT_FRAME_SIZE(cfg->inputFrameSize)
                       | RF_SPI_TURNAROUND_TIME(cfg->turnAroundCycles));

    // To remove the "FORCE ZERO" in case of low-active CS
    hwp_rfSpi->Command = 0;

    // Max speed
    hwp_rfSpi->Divider = RF_SPI_DIVIDER(0);
}

#define REG_TIMER_BASE             0x01A02000

typedef volatile struct
{
    REG32                          OSTimer_Ctrl;                 //0x00000000
    REG32                          OSTimer_CurVal;               //0x00000004
    REG32                          WDTimer_Ctrl;                 //0x00000008
    REG32                          WDTimer_LoadVal;              //0x0000000C
    REG32                          HWTimer_Ctrl;                 //0x00000010
    REG32                          HWTimer_CurVal;               //0x00000014
    REG32                          Timer_Irq_Mask_Set;           //0x00000018
    REG32                          Timer_Irq_Mask_Clr;           //0x0000001C
    REG32                          Timer_Irq_Clr;                //0x00000020
    REG32                          Timer_Irq_Cause;              //0x00000024
    REG32                          WD_Addr_cfg;                  //0x00000028
} HWP_TIMER_T;

#define hwp_timer                  ((HWP_TIMER_T*) KSEG1(REG_TIMER_BASE))
PUBLIC VOID hal_TimDelay1(UINT32 delay)
{
    UINT32 initialValue;

    initialValue = hwp_timer->HWTimer_CurVal;

    // Wait
#ifndef SIMU
    while ((hwp_timer->HWTimer_CurVal - initialValue) < delay);
#endif
}
PUBLIC VOID hal_RfspiImmWrite(CONST UINT8 *Cmd, UINT32 CmdSize)
{
#ifndef SIMU
    // Assume RFSPI Input_Mode is OFF
    HAL_ASSERT(CmdSize<CMD_DATA_FIFO_LEN, "RF SPI command size too large");

    // Flush the Tx fifo
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO | RF_SPI_FLUSH_RX_FIFO;
    for (UINT32 loop=0; loop<CmdSize; loop++)
    {
        hwp_rfSpi->Cmd_Data = Cmd[loop];
    }
    // Set the cmd size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(CmdSize);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to start - at least one byte has been sent
    while(GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_CMD_DATA_LEVEL) >= CmdSize);
    // Wait for the SPI to finish
    while((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);
#endif
}
PROTECTED VOID rfd_XcvRegForceWriteSingle(UINT32 addr, UINT32 data)
{
    UINT8 array[RDA6220_ONE_CMD_BYTE_QTY];
    UINT32 cmdWord = RDA6220_WRITE_FMT(addr, data);

    // Hardcoded
    array[0] = (cmdWord >> (3 * BYTE_SIZE)) & BYTE_MASK;
    array[1] = (cmdWord >> (2 * BYTE_SIZE)) & BYTE_MASK;
    array[2] = (cmdWord >> (BYTE_SIZE)) & BYTE_MASK;
    array[3] =  cmdWord & BYTE_MASK;

    hal_RfspiImmWrite(array, RDA6220_ONE_CMD_BYTE_QTY);
}

#endif
PROTECTED BOOL fp_CheckPsramData(UINT32  flag, UINT32 dataBuffer_start,UINT32 dataBuffer_end)
{

    /*
       for (int i=g_dataBuffer_start; i<psram_check_size; i=i+4)
       {
     *(volatile UINT32 *)((UINT32)i) = flag;
     }
     */

    int i;

    *(volatile UINT32 *)(0x81c0002c)=flag;

    for (i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {

        //      while(!hal_DmaDone());

        if(!(hal_DmaDone()))
        {
            *(volatile UINT32 *)((UINT32)i) = flag;
        }
        else
        {
            // Clear destination buffer
            ram_dmaCfg.dstAddr=(UINT8*)i;
            ram_dmaCfg.pattern=flag;
            ram_dmaCfg.transferSize=(dataBuffer_end-i)>=0x40000?0x40000:(dataBuffer_end-i);
            ram_dmaCfg.mode = HAL_DMA_MODE_PATTERN;
            ram_dmaCfg.userHandler=NULL;

            while(hal_DmaStart(&ram_dmaCfg)!=HAL_ERR_NO)
            {
            }
            i=i+(((dataBuffer_end-i)>=0x40000)?0x40000:(dataBuffer_end-i))-4;

            // Wait until cleanup finished
            //				while(!hal_DmaDone())
            //				{
            //				}
        }
    }

    while(!hal_DmaDone())
    {
    }

    for (i=dataBuffer_start; i<dataBuffer_end; i=i+4)
    {
        if (*(volatile UINT32 *)((UINT32)i) != flag)
        {
            SEND_EVENT(0x88500000|(flag&0xff));
            SEND_EVENT((UINT32)i);
            SEND_EVENT(flag);
            ret_flag =1;
            *(volatile UINT32 *)(0x81c00020)=flag;
            *(volatile UINT32 *)(0x81c00024)=i;
            *(volatile UINT32 *)(0x81c00028)=*(UINT32 *)((UINT32)i);
            *(volatile UINT32 *)(0x81c00030)=dataBuffer_start;
            *(volatile UINT32 *)(0x81c00034)=dataBuffer_end;
            return FALSE;
        }
    }
    return TRUE;
}

PROTECTED VOID fp_CheckPsram(VOID)
{
    UINT32 elapsedTime=0,fpsStartMeasTime = hal_TimGetUpTime();

    *(volatile UINT32 *)(0x81c00000)=0x6500b203;

    fp_CheckPsramData(0x55555555,g_dataBuffer_start,psram_check_size);
    fp_CheckPsramData_noDMA(0xAAAAAAAA,g_dataBuffer_start,psram_check_size);

    elapsedTime = (hal_TimGetUpTime()-fpsStartMeasTime);

    if(ret_flag == 1)
        *(volatile UINT32 *)(0x81c00000)=0x22222222;
    else
        *(volatile UINT32 *)(0x81c00000)=0xAAAAAAA8;
    *(volatile UINT32 *)(0x81c00008)=elapsedTime;
    *(volatile UINT32 *)(0x81c00004)=hal_TimGetUpTime();
    return ;
}


#define ERAM_CHECK_PATTERN0  0xa569f0f0
#define ERAM_CHECK_PATTERN1  0x5a960f0f
#define ERAM_ADDR_NUM 6
UINT32 eram_end_addr2[ERAM_ADDR_NUM] = {0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000};
#define ERAM_CHECK_ERROR_CODE 0xdead0000
#define CRAM_DIDR 0x40000

UINT32 ERamSize_Valid_Check(REG16* csBase)
{
    // Using the CR controlled method
    // Configure DIDR
    // CRAM_DIDR[15] : ROW length
    // CRAM_DIDR[14:11] : Device Version
    // CRAM_DIDR[10:8] : Device density
    // CRAM_DIDR[7:5] : CRAM Generation
    // CRAM_DIDR[4:0] : Vendor ID
    UINT16 index=0;
    REG32* pCRE = &(hwp_memFifo->EBC_CRE);
    UINT32 is_aptic=0,aptic_size=0;

    *pCRE = 1; // MEM_FIFO_CRE
    index = *(csBase + CRAM_DIDR);
    UINT32 flush __attribute__((unused)) = *pCRE;
    *pCRE = 0;

    if((index & 0x1f) == 0xd)//aptic
    {
        index = (index >> 8) & 7;
        is_aptic=1;
        aptic_size=eram_end_addr2[index];
        hal_HstSendEvent(aptic_size);
    }

    {
        volatile UINT32 *ERam_Base = (UINT32 *)csBase;
        *ERam_Base = ERAM_CHECK_PATTERN0;

        for(index = 0; index < ERAM_ADDR_NUM; index++)
        {
            if(*(volatile UINT32 *)((UINT32)ERam_Base + eram_end_addr2[index]) == ERAM_CHECK_PATTERN0)
            {
                *ERam_Base = ERAM_CHECK_PATTERN1;
                if(*(volatile UINT32 *)((UINT32)ERam_Base + eram_end_addr2[index]) == ERAM_CHECK_PATTERN1)
                {
                    if(is_aptic)
                    {
                        if(aptic_size==eram_end_addr2[index])
                            return eram_end_addr2[index];
                        else
                        {
                            *(volatile UINT32 *)(0x81c00000)=0x11111111;
                            while(1);
                        }
                    }
                    else
                        return eram_end_addr2[index];
                }
            }
        }
    }
    return 0;
}
VOID rfd_XcvRfInit(VOID)
{
    // TO BE REMOVED
    // hal_HstSendEvent(0x88886666);


    rfd_XcvRegForceWriteSingle(0x30, 0x5182);

    hal_TimDelay1(10 MS_WAITING);  // delay 10 ms
    rfd_XcvRegForceWriteSingle(0x30, 0x5183);
    hal_TimDelay1(10 MS_WAITING);  // delay 10 ms
    //  xtal setting
    rfd_XcvRegForceWriteSingle(0xaf, 0x0001);
    // RX setting
    rfd_XcvRegForceWriteSingle(0x10, 0x009e);
    rfd_XcvRegForceWriteSingle(0x14, 0x54f4);
    rfd_XcvRegForceWriteSingle(0x16, 0xf410);
    rfd_XcvRegForceWriteSingle(0x18, 0xf880);   // 0xf080 0529
    rfd_XcvRegForceWriteSingle(0x1a, 0x8008);
    // VCO & PLL setting
    rfd_XcvRegForceWriteSingle(0x1c, 0x0088);   //0x9088  0529
    rfd_XcvRegForceWriteSingle(0x20, 0x8881);
    rfd_XcvRegForceWriteSingle(0x22, 0x4b45);
    rfd_XcvRegForceWriteSingle(0x24, 0x0388);
    rfd_XcvRegForceWriteSingle(0x26, 0x8025);
    rfd_XcvRegForceWriteSingle(0x28, 0xa020);
    rfd_XcvRegForceWriteSingle(0x2a, 0x8080);
    rfd_XcvRegForceWriteSingle(0x2c, 0x88c5);
    rfd_XcvRegForceWriteSingle(0x2e, 0xc000);   //0xff00   0529
    rfd_XcvRegForceWriteSingle(0x34, 0x24c9);
    rfd_XcvRegForceWriteSingle(0x36, 0xfe00);
    // DSP setting
    //
    rfd_XcvRegForceWriteSingle(0x44, 0x3000);
    rfd_XcvRegForceWriteSingle(0x4a, 0xb00f);
    rfd_XcvRegForceWriteSingle(0x4c, 0x4700);
    rfd_XcvRegForceWriteSingle(0x50, 0x0253);   //0xf23d  0605  0x0241 0609
    rfd_XcvRegForceWriteSingle(0x52, 0x0000);
    rfd_XcvRegForceWriteSingle(0x54, 0x0004);
    rfd_XcvRegForceWriteSingle(0x56, 0x0250);
    rfd_XcvRegForceWriteSingle(0x58, 0xbbdb);  //0xabdb   0529
    rfd_XcvRegForceWriteSingle(0x5a, 0x1108);  //0x1130   0529
    rfd_XcvRegForceWriteSingle(0x62, 0x0080);
    rfd_XcvRegForceWriteSingle(0x64, 0x0080);  //0x0800   0529
    rfd_XcvRegForceWriteSingle(0x66, 0x0c84);
    rfd_XcvRegForceWriteSingle(0x68, 0xb0da);  //0xb02a   0529
    rfd_XcvRegForceWriteSingle(0x6a, 0x12c9);  //0xacc9   0529
    rfd_XcvRegForceWriteSingle(0x6c, 0x8000);
    rfd_XcvRegForceWriteSingle(0x6e, 0xfcd0);
    rfd_XcvRegForceWriteSingle(0x7a, 0x00a0);
    rfd_XcvRegForceWriteSingle(0x7c, 0x6000);
    rfd_XcvRegForceWriteSingle(0x7e, 0x9580);
    rfd_XcvRegForceWriteSingle(0xb2, 0x1894);  //0x18b4   0529
    rfd_XcvRegForceWriteSingle(0xb4, 0x0401);
    rfd_XcvRegForceWriteSingle(0xb4, 0x0405);
    rfd_XcvRegForceWriteSingle(0xb6, 0x0471);
    //    rfd_XcvRegForceWriteSingle(0xb8, 0x0800);
    rfd_XcvRegForceWriteSingle(0xba, 0x1f00);   // 0517 8051 fast_clk
    rfd_XcvRegForceWriteSingle(0xbc, 0x9ca8);
    rfd_XcvRegForceWriteSingle(0xbe, 0x6000);  // 0xe00a 0609 for 975 sensitivity


    rfd_XcvRegForceWriteSingle(0xd4, 0x0102);
    rfd_XcvRegForceWriteSingle(0xd6, 0x060e);
    rfd_XcvRegForceWriteSingle(0xd8, 0x1d35);
    rfd_XcvRegForceWriteSingle(0xdc, 0x557b);
    rfd_XcvRegForceWriteSingle(0xdc, 0xa3c9);
    rfd_XcvRegForceWriteSingle(0xde, 0xe6f9);
    rfd_XcvRegForceWriteSingle(0xe8, 0x0000);
    rfd_XcvRegForceWriteSingle(0xe6, 0xc000);  // add 0529
    rfd_XcvRegForceWriteSingle(0xb8, 0x0450);  // add 0603  digafc
#ifdef PRE_P5_BOARD   // for p4 and p3
    rfd_XcvRegForceWriteSingle(0x06, 0x26ae);  // 0617  for gpio 06_reg rxl 7-4,rxh 3-0,07_reg txl 7-4 txh 3-0
#else // for p5 and p6
    rfd_XcvRegForceWriteSingle(0x06, 0x4c4c);  // 0617  for gpio 06_reg rxl 7-4,rxh 3-0,07_reg txl 7-4 txh 3-0
#endif
    rfd_XcvRegForceWriteSingle(0x08, 0x141a);  // 0x08 tx group on timing, 0x09 paon timing

    //item:8051 Test1

    hal_TimDelay1(10 MS_WAITING);  // delay 10 ms
    rfd_XcvRegForceWriteSingle(0x30, 0x1183);

    rfd_XcvRegForceWriteSingle(0x30, 0x0301);  //;DL=1, rst=1
    hal_TimDelay1(1 MS_WAITING);  // delay 1 ms
#if 1
    rfd_XcvRegForceWriteSingle(0xFF,0x011D);
    rfd_XcvRegForceWriteSingle(0xFF,0x0001);
    rfd_XcvRegForceWriteSingle(0xFF,0xA600);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x00E1);
    rfd_XcvRegForceWriteSingle(0xFF,0x5300);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x00E1);
    rfd_XcvRegForceWriteSingle(0xFF,0x5875);
    rfd_XcvRegForceWriteSingle(0xFF,0xD000);
    rfd_XcvRegForceWriteSingle(0xFF,0x7580);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x9000);
    rfd_XcvRegForceWriteSingle(0xFF,0x75A0);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0xB0FF);
    rfd_XcvRegForceWriteSingle(0xFF,0xE4C3);
    rfd_XcvRegForceWriteSingle(0xFF,0x7581);
    rfd_XcvRegForceWriteSingle(0xFF,0x2F75);
    rfd_XcvRegForceWriteSingle(0xFF,0xA88B);
    rfd_XcvRegForceWriteSingle(0xFF,0x75B8);
    rfd_XcvRegForceWriteSingle(0xFF,0x0A75);
    rfd_XcvRegForceWriteSingle(0xFF,0x8911);
    rfd_XcvRegForceWriteSingle(0xFF,0x7588);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x6400);
    rfd_XcvRegForceWriteSingle(0xFF,0x013F);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0111);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0211);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0311);
    rfd_XcvRegForceWriteSingle(0xFF,0x8522);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0800);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0801);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0802);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0803);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x2285);
    rfd_XcvRegForceWriteSingle(0xFF,0x0890);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0x80C2);
    rfd_XcvRegForceWriteSingle(0xFF,0xB5D2);
    rfd_XcvRegForceWriteSingle(0xFF,0xB522);
    rfd_XcvRegForceWriteSingle(0xFF,0x8508);
    rfd_XcvRegForceWriteSingle(0xFF,0x9085);
    rfd_XcvRegForceWriteSingle(0xFF,0x9009);
    rfd_XcvRegForceWriteSingle(0xFF,0x22E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x08A3);
    rfd_XcvRegForceWriteSingle(0xFF,0x00E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x09A3);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8522);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x90E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x09B4);
    rfd_XcvRegForceWriteSingle(0xFF,0xFF0C);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0111);
    rfd_XcvRegForceWriteSingle(0xFF,0x90E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x09B4);
    rfd_XcvRegForceWriteSingle(0xFF,0xFF22);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x1143);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E7);
    rfd_XcvRegForceWriteSingle(0xFF,0x0B30);
    rfd_XcvRegForceWriteSingle(0xFF,0xE60C);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0D30);
    rfd_XcvRegForceWriteSingle(0xFF,0xE40E);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x3115);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x519D);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x51C3);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x71CF);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x1143);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E7);
    rfd_XcvRegForceWriteSingle(0xFF,0x1730);
    rfd_XcvRegForceWriteSingle(0xFF,0xE618);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1730);
    rfd_XcvRegForceWriteSingle(0xFF,0xE416);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E3);
    rfd_XcvRegForceWriteSingle(0xFF,0x1530);
    rfd_XcvRegForceWriteSingle(0xFF,0xE214);
    rfd_XcvRegForceWriteSingle(0xFF,0x30E1);
    rfd_XcvRegForceWriteSingle(0xFF,0x1530);
    rfd_XcvRegForceWriteSingle(0xFF,0xE016);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x71F3);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0xF152);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0xF151);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0xF150);
    rfd_XcvRegForceWriteSingle(0xFF,0x2110);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x6432);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0804);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5711);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0805);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5811);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0806);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0838);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0x10E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0954);
    rfd_XcvRegForceWriteSingle(0xFF,0x07F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x11E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1020);
    rfd_XcvRegForceWriteSingle(0xFF,0xE30A);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5A75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0901);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x2156);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5A75);
    rfd_XcvRegForceWriteSingle(0xFF,0x091D);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x20E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0520);
    rfd_XcvRegForceWriteSingle(0xFF,0xE42A);
    rfd_XcvRegForceWriteSingle(0xFF,0x4145);
    rfd_XcvRegForceWriteSingle(0xFF,0xE512);
    rfd_XcvRegForceWriteSingle(0xFF,0x54F0);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0xE811);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0815);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0xF211);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x081E);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x2211);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x081F);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x2011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8590);
    rfd_XcvRegForceWriteSingle(0xFF,0x04AB);
    rfd_XcvRegForceWriteSingle(0xFF,0x21AD);
    rfd_XcvRegForceWriteSingle(0xFF,0xE512);
    rfd_XcvRegForceWriteSingle(0xFF,0x540F);
    rfd_XcvRegForceWriteSingle(0xFF,0xC4F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x08E8);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1575);
    rfd_XcvRegForceWriteSingle(0xFF,0x09EE);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0922);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1F75);
    rfd_XcvRegForceWriteSingle(0xFF,0x09C0);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x9004);
    rfd_XcvRegForceWriteSingle(0xFF,0xEEE5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1151);
    rfd_XcvRegForceWriteSingle(0xFF,0x48FA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x0814);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x0811);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x0816);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x8709);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1911);
    rfd_XcvRegForceWriteSingle(0xFF,0x85A3);
    rfd_XcvRegForceWriteSingle(0xFF,0xEA93);
    rfd_XcvRegForceWriteSingle(0xFF,0xF987);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x081A);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x8709);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5E11);
    rfd_XcvRegForceWriteSingle(0xFF,0x85A3);
    rfd_XcvRegForceWriteSingle(0xFF,0xEA93);
    rfd_XcvRegForceWriteSingle(0xFF,0xF987);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x085F);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EA);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x8709);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x6011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0805);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0xE509);
    rfd_XcvRegForceWriteSingle(0xFF,0x20E0);
    rfd_XcvRegForceWriteSingle(0xFF,0x0590);
    rfd_XcvRegForceWriteSingle(0xFF,0x0253);
    rfd_XcvRegForceWriteSingle(0xFF,0x4117);
    rfd_XcvRegForceWriteSingle(0xFF,0x9002);
    rfd_XcvRegForceWriteSingle(0xFF,0x787F);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x97D1);
    rfd_XcvRegForceWriteSingle(0xFF,0xF30F);
    rfd_XcvRegForceWriteSingle(0xFF,0xBF11);
    rfd_XcvRegForceWriteSingle(0xFF,0xF800);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1411);
    rfd_XcvRegForceWriteSingle(0xFF,0x90E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x09D2);
    rfd_XcvRegForceWriteSingle(0xFF,0xE7C2);
    rfd_XcvRegForceWriteSingle(0xFF,0xE6F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0911);
    rfd_XcvRegForceWriteSingle(0xFF,0x85C2);
    rfd_XcvRegForceWriteSingle(0xFF,0xE7D2);
    rfd_XcvRegForceWriteSingle(0xFF,0xE6F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0911);
    rfd_XcvRegForceWriteSingle(0xFF,0x85D1);
    rfd_XcvRegForceWriteSingle(0xFF,0xFA75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0852);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x8011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8541);
    rfd_XcvRegForceWriteSingle(0xFF,0x4600);
    rfd_XcvRegForceWriteSingle(0xFF,0x0022);
    rfd_XcvRegForceWriteSingle(0xFF,0xFB2B);
    rfd_XcvRegForceWriteSingle(0xFF,0x2B2B);
    rfd_XcvRegForceWriteSingle(0xFF,0x2B2B);
    rfd_XcvRegForceWriteSingle(0xFF,0x2B2B);
    rfd_XcvRegForceWriteSingle(0xFF,0x2200);
    rfd_XcvRegForceWriteSingle(0xFF,0x0096);
    rfd_XcvRegForceWriteSingle(0xFF,0xC697);
    rfd_XcvRegForceWriteSingle(0xFF,0x0812);
    rfd_XcvRegForceWriteSingle(0xFF,0xFF13);
    rfd_XcvRegForceWriteSingle(0xFF,0xCCB3);
    rfd_XcvRegForceWriteSingle(0xFF,0x94B3);
    rfd_XcvRegForceWriteSingle(0xFF,0xB418);
    rfd_XcvRegForceWriteSingle(0xFF,0xC818);
    rfd_XcvRegForceWriteSingle(0xFF,0xC9BD);
    rfd_XcvRegForceWriteSingle(0xFF,0xA8BD);
    rfd_XcvRegForceWriteSingle(0xFF,0x2892);
    rfd_XcvRegForceWriteSingle(0xFF,0x4992);
    rfd_XcvRegForceWriteSingle(0xFF,0xC944);
    rfd_XcvRegForceWriteSingle(0xFF,0x3044);
    rfd_XcvRegForceWriteSingle(0xFF,0xB050);
    rfd_XcvRegForceWriteSingle(0xFF,0x0250);
    rfd_XcvRegForceWriteSingle(0xFF,0x8252);
    rfd_XcvRegForceWriteSingle(0xFF,0x00AA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x96C6);
    rfd_XcvRegForceWriteSingle(0xFF,0x9708);
    rfd_XcvRegForceWriteSingle(0xFF,0x12FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x13CC);
    rfd_XcvRegForceWriteSingle(0xFF,0xB394);
    rfd_XcvRegForceWriteSingle(0xFF,0xB3B4);
    rfd_XcvRegForceWriteSingle(0xFF,0x18C8);
    rfd_XcvRegForceWriteSingle(0xFF,0x18C9);
    rfd_XcvRegForceWriteSingle(0xFF,0xBDA8);
    rfd_XcvRegForceWriteSingle(0xFF,0xBD28);
    rfd_XcvRegForceWriteSingle(0xFF,0x9249);
    rfd_XcvRegForceWriteSingle(0xFF,0x92C9);
    rfd_XcvRegForceWriteSingle(0xFF,0x4430);
    rfd_XcvRegForceWriteSingle(0xFF,0x4430);
    rfd_XcvRegForceWriteSingle(0xFF,0x5002);
    rfd_XcvRegForceWriteSingle(0xFF,0x5082);
    rfd_XcvRegForceWriteSingle(0xFF,0x5200);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x007E);
    rfd_XcvRegForceWriteSingle(0xFF,0x0090);
    rfd_XcvRegForceWriteSingle(0xFF,0x02AC);
    rfd_XcvRegForceWriteSingle(0xFF,0x1197);
    rfd_XcvRegForceWriteSingle(0xFF,0x0EBE);
    rfd_XcvRegForceWriteSingle(0xFF,0x0AFA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0022);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x1200);
    rfd_XcvRegForceWriteSingle(0xFF,0x1300);
    rfd_XcvRegForceWriteSingle(0xFF,0x1400);
    rfd_XcvRegForceWriteSingle(0xFF,0xB394);
    rfd_XcvRegForceWriteSingle(0xFF,0x18F8);
    rfd_XcvRegForceWriteSingle(0xFF,0xBDA8);
    rfd_XcvRegForceWriteSingle(0xFF,0x9249);
    rfd_XcvRegForceWriteSingle(0xFF,0x4430);
    rfd_XcvRegForceWriteSingle(0xFF,0x5002);
    rfd_XcvRegForceWriteSingle(0xFF,0x5200);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x084C);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0xE509);
    rfd_XcvRegForceWriteSingle(0xFF,0x54C0);
    rfd_XcvRegForceWriteSingle(0xFF,0xC333);
    rfd_XcvRegForceWriteSingle(0xFF,0x3333);
    rfd_XcvRegForceWriteSingle(0xFF,0xFC90);
    rfd_XcvRegForceWriteSingle(0xFF,0x039F);
    rfd_XcvRegForceWriteSingle(0xFF,0x719A);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x0869);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3EC);
    rfd_XcvRegForceWriteSingle(0xFF,0x719A);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x086A);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0711);
    rfd_XcvRegForceWriteSingle(0xFF,0x9085);
    rfd_XcvRegForceWriteSingle(0xFF,0x0912);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x3811);
    rfd_XcvRegForceWriteSingle(0xFF,0x90E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0920);
    rfd_XcvRegForceWriteSingle(0xFF,0xE505);
    rfd_XcvRegForceWriteSingle(0xFF,0x20E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x3161);
    rfd_XcvRegForceWriteSingle(0xFF,0x97E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1254);
    rfd_XcvRegForceWriteSingle(0xFF,0xF0F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x13F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x08E8);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0922);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1F75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0908);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x2D75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0945);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x6875);
    rfd_XcvRegForceWriteSingle(0xFF,0x09C0);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x615E);
    rfd_XcvRegForceWriteSingle(0xFF,0xE512);
    rfd_XcvRegForceWriteSingle(0xFF,0x540F);
    rfd_XcvRegForceWriteSingle(0xFF,0xC4F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x13F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x08E8);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0922);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1F75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0990);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x2D75);
    rfd_XcvRegForceWriteSingle(0xFF,0x09C5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x6875);
    rfd_XcvRegForceWriteSingle(0xFF,0x09B0);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7C00);
    rfd_XcvRegForceWriteSingle(0xFF,0x9003);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA11);
    rfd_XcvRegForceWriteSingle(0xFF,0x97D1);
    rfd_XcvRegForceWriteSingle(0xFF,0xF30C);
    rfd_XcvRegForceWriteSingle(0xFF,0xBC11);
    rfd_XcvRegForceWriteSingle(0xFF,0xF890);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x0811);
    rfd_XcvRegForceWriteSingle(0xFF,0x9085);
    rfd_XcvRegForceWriteSingle(0xFF,0x090B);
    rfd_XcvRegForceWriteSingle(0xFF,0xF108);
    rfd_XcvRegForceWriteSingle(0xFF,0xE513);
    rfd_XcvRegForceWriteSingle(0xFF,0xD2E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0xE8F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0911);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0809);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BF1);
    rfd_XcvRegForceWriteSingle(0xFF,0x0875);
    rfd_XcvRegForceWriteSingle(0xFF,0x0813);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x7C11);
    rfd_XcvRegForceWriteSingle(0xFF,0x8561);
    rfd_XcvRegForceWriteSingle(0xFF,0x9800);
    rfd_XcvRegForceWriteSingle(0xFF,0x0022);
    rfd_XcvRegForceWriteSingle(0xFF,0xFD2D);
    rfd_XcvRegForceWriteSingle(0xFF,0x2200);
    rfd_XcvRegForceWriteSingle(0xFF,0x00CF);
    rfd_XcvRegForceWriteSingle(0xFF,0x3BD5);
    rfd_XcvRegForceWriteSingle(0xFF,0x54DA);
    rfd_XcvRegForceWriteSingle(0xFF,0x12DD);
    rfd_XcvRegForceWriteSingle(0xFF,0xDDAA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x2EFF);
    rfd_XcvRegForceWriteSingle(0xFF,0x6BC9);
    rfd_XcvRegForceWriteSingle(0xFF,0x96A7);
    rfd_XcvRegForceWriteSingle(0xFF,0x1287);
    rfd_XcvRegForceWriteSingle(0xFF,0x134C);
    rfd_XcvRegForceWriteSingle(0xFF,0x2C18);
    rfd_XcvRegForceWriteSingle(0xFF,0x2C98);
    rfd_XcvRegForceWriteSingle(0xFF,0xBDA8);
    rfd_XcvRegForceWriteSingle(0xFF,0xBD28);
    rfd_XcvRegForceWriteSingle(0xFF,0x9249);
    rfd_XcvRegForceWriteSingle(0xFF,0x92C9);
    rfd_XcvRegForceWriteSingle(0xFF,0x4431);
    rfd_XcvRegForceWriteSingle(0xFF,0x44B1);
    rfd_XcvRegForceWriteSingle(0xFF,0x5002);
    rfd_XcvRegForceWriteSingle(0xFF,0x5082);
    rfd_XcvRegForceWriteSingle(0xFF,0x5200);
    rfd_XcvRegForceWriteSingle(0xFF,0x5280);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x007B);
    rfd_XcvRegForceWriteSingle(0xFF,0x0090);
    rfd_XcvRegForceWriteSingle(0xFF,0x03DE);
    rfd_XcvRegForceWriteSingle(0xFF,0x1197);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BBB);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0022);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x1200);
    rfd_XcvRegForceWriteSingle(0xFF,0x1300);
    rfd_XcvRegForceWriteSingle(0xFF,0x2C18);
    rfd_XcvRegForceWriteSingle(0xFF,0xBDA8);
    rfd_XcvRegForceWriteSingle(0xFF,0x9249);
    rfd_XcvRegForceWriteSingle(0xFF,0x4431);
    rfd_XcvRegForceWriteSingle(0xFF,0x5002);
    rfd_XcvRegForceWriteSingle(0xFF,0x5200);
    rfd_XcvRegForceWriteSingle(0xFF,0xE800);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x0075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0806);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0x12E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x1254);
    rfd_XcvRegForceWriteSingle(0xFF,0xF0F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x08E8);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xB131);
    rfd_XcvRegForceWriteSingle(0xFF,0xF13D);
    rfd_XcvRegForceWriteSingle(0xFF,0x9004);
    rfd_XcvRegForceWriteSingle(0xFF,0xAB91);
    rfd_XcvRegForceWriteSingle(0xFF,0x3700);
    rfd_XcvRegForceWriteSingle(0xFF,0xE512);
    rfd_XcvRegForceWriteSingle(0xFF,0x540F);
    rfd_XcvRegForceWriteSingle(0xFF,0xC4F5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0975);
    rfd_XcvRegForceWriteSingle(0xFF,0x08E8);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xB140);
    rfd_XcvRegForceWriteSingle(0xFF,0xF130);
    rfd_XcvRegForceWriteSingle(0xFF,0x9004);
    rfd_XcvRegForceWriteSingle(0xFF,0xEE91);
    rfd_XcvRegForceWriteSingle(0xFF,0x3700);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1375);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x227F);
    rfd_XcvRegForceWriteSingle(0xFF,0x00E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x9375);
    rfd_XcvRegForceWriteSingle(0xFF,0x0814);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xD2E7);
    rfd_XcvRegForceWriteSingle(0xFF,0xC2E6);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xC2E7);
    rfd_XcvRegForceWriteSingle(0xFF,0xD2E6);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x9375);
    rfd_XcvRegForceWriteSingle(0xFF,0x0811);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x9375);
    rfd_XcvRegForceWriteSingle(0xFF,0x0816);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3F1);
    rfd_XcvRegForceWriteSingle(0xFF,0x16B1);
    rfd_XcvRegForceWriteSingle(0xFF,0x4FE4);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1911);
    rfd_XcvRegForceWriteSingle(0xFF,0x90A7);
    rfd_XcvRegForceWriteSingle(0xFF,0x09A3);
    rfd_XcvRegForceWriteSingle(0xFF,0xE493);
    rfd_XcvRegForceWriteSingle(0xFF,0xF975);
    rfd_XcvRegForceWriteSingle(0xFF,0x081A);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0xA709);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3D1);
    rfd_XcvRegForceWriteSingle(0xFF,0x06E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5E11);
    rfd_XcvRegForceWriteSingle(0xFF,0x90A7);
    rfd_XcvRegForceWriteSingle(0xFF,0x09A3);
    rfd_XcvRegForceWriteSingle(0xFF,0xE493);
    rfd_XcvRegForceWriteSingle(0xFF,0xF975);
    rfd_XcvRegForceWriteSingle(0xFF,0x085F);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0xA709);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3E4);
    rfd_XcvRegForceWriteSingle(0xFF,0x93F9);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x6011);
    rfd_XcvRegForceWriteSingle(0xFF,0x90A7);
    rfd_XcvRegForceWriteSingle(0xFF,0x09A3);
    rfd_XcvRegForceWriteSingle(0xFF,0x0FBF);
    rfd_XcvRegForceWriteSingle(0xFF,0x0891);
    rfd_XcvRegForceWriteSingle(0xFF,0x2200);
    rfd_XcvRegForceWriteSingle(0xFF,0x006A);
    rfd_XcvRegForceWriteSingle(0xFF,0x010C);
    rfd_XcvRegForceWriteSingle(0xFF,0xDAD9);
    rfd_XcvRegForceWriteSingle(0xFF,0xD8D7);
    rfd_XcvRegForceWriteSingle(0xFF,0xD66A);
    rfd_XcvRegForceWriteSingle(0xFF,0x420C);
    rfd_XcvRegForceWriteSingle(0xFF,0xDFDE);
    rfd_XcvRegForceWriteSingle(0xFF,0xDDDC);
    rfd_XcvRegForceWriteSingle(0xFF,0xDB6A);
    rfd_XcvRegForceWriteSingle(0xFF,0x460C);
    rfd_XcvRegForceWriteSingle(0xFF,0xE5E4);
    rfd_XcvRegForceWriteSingle(0xFF,0xE3E2);
    rfd_XcvRegForceWriteSingle(0xFF,0xE16A);
    rfd_XcvRegForceWriteSingle(0xFF,0x461C);
    rfd_XcvRegForceWriteSingle(0xFF,0xEAE9);
    rfd_XcvRegForceWriteSingle(0xFF,0xE8E7);
    rfd_XcvRegForceWriteSingle(0xFF,0xE66A);
    rfd_XcvRegForceWriteSingle(0xFF,0x8E1C);
    rfd_XcvRegForceWriteSingle(0xFF,0xEFEE);
    rfd_XcvRegForceWriteSingle(0xFF,0xEDEC);
    rfd_XcvRegForceWriteSingle(0xFF,0xEB6A);
    rfd_XcvRegForceWriteSingle(0xFF,0xCEDC);
    rfd_XcvRegForceWriteSingle(0xFF,0xF5F4);
    rfd_XcvRegForceWriteSingle(0xFF,0xF3F2);
    rfd_XcvRegForceWriteSingle(0xFF,0xF16A);
    rfd_XcvRegForceWriteSingle(0xFF,0xDEFC);
    rfd_XcvRegForceWriteSingle(0xFF,0xFAF9);
    rfd_XcvRegForceWriteSingle(0xFF,0xF8F7);
    rfd_XcvRegForceWriteSingle(0xFF,0xF668);
    rfd_XcvRegForceWriteSingle(0xFF,0xFEFC);
    rfd_XcvRegForceWriteSingle(0xFF,0xFFFE);
    rfd_XcvRegForceWriteSingle(0xFF,0xFDFC);
    rfd_XcvRegForceWriteSingle(0xFF,0xFBAA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x5501);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CC0);
    rfd_XcvRegForceWriteSingle(0xFF,0xBAB7);
    rfd_XcvRegForceWriteSingle(0xFF,0xB6AA);
    rfd_XcvRegForceWriteSingle(0xFF,0x5542);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CA7);
    rfd_XcvRegForceWriteSingle(0xFF,0xA5A4);
    rfd_XcvRegForceWriteSingle(0xFF,0xA3A1);
    rfd_XcvRegForceWriteSingle(0xFF,0x5546);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CAF);
    rfd_XcvRegForceWriteSingle(0xFF,0xAEAD);
    rfd_XcvRegForceWriteSingle(0xFF,0xACAB);
    rfd_XcvRegForceWriteSingle(0xFF,0x5546);
    rfd_XcvRegForceWriteSingle(0xFF,0x1CB5);
    rfd_XcvRegForceWriteSingle(0xFF,0xB4B3);
    rfd_XcvRegForceWriteSingle(0xFF,0xB2B1);
    rfd_XcvRegForceWriteSingle(0xFF,0x558E);
    rfd_XcvRegForceWriteSingle(0xFF,0x1CBF);
    rfd_XcvRegForceWriteSingle(0xFF,0xBEBD);
    rfd_XcvRegForceWriteSingle(0xFF,0xBCBB);
    rfd_XcvRegForceWriteSingle(0xFF,0x55CE);
    rfd_XcvRegForceWriteSingle(0xFF,0xDCC5);
    rfd_XcvRegForceWriteSingle(0xFF,0xC4C3);
    rfd_XcvRegForceWriteSingle(0xFF,0xC2C1);
    rfd_XcvRegForceWriteSingle(0xFF,0x55DE);
    rfd_XcvRegForceWriteSingle(0xFF,0xFCCF);
    rfd_XcvRegForceWriteSingle(0xFF,0xCEC9);
    rfd_XcvRegForceWriteSingle(0xFF,0xC7C6);
    rfd_XcvRegForceWriteSingle(0xFF,0x54FE);
    rfd_XcvRegForceWriteSingle(0xFF,0xFCD5);
    rfd_XcvRegForceWriteSingle(0xFF,0xD4D3);
    rfd_XcvRegForceWriteSingle(0xFF,0xD2D1);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x007F);
    rfd_XcvRegForceWriteSingle(0xFF,0x0090);
    rfd_XcvRegForceWriteSingle(0xFF,0x0671);
    rfd_XcvRegForceWriteSingle(0xFF,0x1197);
    rfd_XcvRegForceWriteSingle(0xFF,0xD1FA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0FBF);
    rfd_XcvRegForceWriteSingle(0xFF,0x1EF8);
    rfd_XcvRegForceWriteSingle(0xFF,0x0022);
    rfd_XcvRegForceWriteSingle(0xFF,0x7F00);
    rfd_XcvRegForceWriteSingle(0xFF,0x9006);
    rfd_XcvRegForceWriteSingle(0xFF,0xB011);
    rfd_XcvRegForceWriteSingle(0xFF,0x97D1);
    rfd_XcvRegForceWriteSingle(0xFF,0xFA0F);
    rfd_XcvRegForceWriteSingle(0xFF,0xBF1E);
    rfd_XcvRegForceWriteSingle(0xFF,0xF800);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0818);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0xCB11);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x083C);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x8011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0836);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x5E11);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0819);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x8011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x0819);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0xE509);
    rfd_XcvRegForceWriteSingle(0xFF,0xFA75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0F02);
    rfd_XcvRegForceWriteSingle(0xFF,0x750E);
    rfd_XcvRegForceWriteSingle(0xFF,0xFAF1);
    rfd_XcvRegForceWriteSingle(0xFF,0x0175);
    rfd_XcvRegForceWriteSingle(0xFF,0x0888);
    rfd_XcvRegForceWriteSingle(0xFF,0x1190);
    rfd_XcvRegForceWriteSingle(0xFF,0x8509);
    rfd_XcvRegForceWriteSingle(0xFF,0xE020);
    rfd_XcvRegForceWriteSingle(0xFF,0xE710);
    rfd_XcvRegForceWriteSingle(0xFF,0xBAFF);
    rfd_XcvRegForceWriteSingle(0xFF,0x02A1);
    rfd_XcvRegForceWriteSingle(0xFF,0xA80A);
    rfd_XcvRegForceWriteSingle(0xFF,0xEA75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0819);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xA1A8);
    rfd_XcvRegForceWriteSingle(0xFF,0xBA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x02A1);
    rfd_XcvRegForceWriteSingle(0xFF,0xA81A);
    rfd_XcvRegForceWriteSingle(0xFF,0xEA75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0819);
    rfd_XcvRegForceWriteSingle(0xFF,0xF509);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50E);
    rfd_XcvRegForceWriteSingle(0xFF,0xD2D5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0FCC);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1A75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0980);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1A11);
    rfd_XcvRegForceWriteSingle(0xFF,0x90E5);
    rfd_XcvRegForceWriteSingle(0xFF,0x09FA);
    rfd_XcvRegForceWriteSingle(0xFF,0x750F);
    rfd_XcvRegForceWriteSingle(0xFF,0x0275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0EFA);
    rfd_XcvRegForceWriteSingle(0xFF,0xF101);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x8811);
    rfd_XcvRegForceWriteSingle(0xFF,0x9085);
    rfd_XcvRegForceWriteSingle(0xFF,0x09E0);
    rfd_XcvRegForceWriteSingle(0xFF,0x20E6);
    rfd_XcvRegForceWriteSingle(0xFF,0x10BA);
    rfd_XcvRegForceWriteSingle(0xFF,0xFF02);
    rfd_XcvRegForceWriteSingle(0xFF,0xA1EF);
    rfd_XcvRegForceWriteSingle(0xFF,0x0AEA);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1AF5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0911);
    rfd_XcvRegForceWriteSingle(0xFF,0x85A1);
    rfd_XcvRegForceWriteSingle(0xFF,0xEFBA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0002);
    rfd_XcvRegForceWriteSingle(0xFF,0xA1EF);
    rfd_XcvRegForceWriteSingle(0xFF,0x1AEA);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x1AF5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0911);
    rfd_XcvRegForceWriteSingle(0xFF,0x85D5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0ED2);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50F);
    rfd_XcvRegForceWriteSingle(0xFF,0xCC75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0818);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0xC911);
    rfd_XcvRegForceWriteSingle(0xFF,0x8575);
    rfd_XcvRegForceWriteSingle(0xFF,0x083C);
    rfd_XcvRegForceWriteSingle(0xFF,0x7509);
    rfd_XcvRegForceWriteSingle(0xFF,0x0011);
    rfd_XcvRegForceWriteSingle(0xFF,0x8522);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5775);
    rfd_XcvRegForceWriteSingle(0xFF,0x0940);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5875);
    rfd_XcvRegForceWriteSingle(0xFF,0x093A);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5F75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x6075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x9E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0904);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5D75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0904);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0xF130);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x5D75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x9E75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0900);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x8C11);
    rfd_XcvRegForceWriteSingle(0xFF,0x9075);
    rfd_XcvRegForceWriteSingle(0xFF,0x085E);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x8D11);
    rfd_XcvRegForceWriteSingle(0xFF,0x9075);
    rfd_XcvRegForceWriteSingle(0xFF,0x085F);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x7508);
    rfd_XcvRegForceWriteSingle(0xFF,0x8E11);
    rfd_XcvRegForceWriteSingle(0xFF,0x9075);
    rfd_XcvRegForceWriteSingle(0xFF,0x0860);
    rfd_XcvRegForceWriteSingle(0xFF,0x1185);
    rfd_XcvRegForceWriteSingle(0xFF,0x2200);
    rfd_XcvRegForceWriteSingle(0xFF,0x0038);
    rfd_XcvRegForceWriteSingle(0xFF,0xA74C);
    rfd_XcvRegForceWriteSingle(0xFF,0x07BC);
    rfd_XcvRegForceWriteSingle(0xFF,0x7C15);
    rfd_XcvRegForceWriteSingle(0xFF,0xF21E);
    rfd_XcvRegForceWriteSingle(0xFF,0x221F);
    rfd_XcvRegForceWriteSingle(0xFF,0x2090);
    rfd_XcvRegForceWriteSingle(0xFF,0x4891);
    rfd_XcvRegForceWriteSingle(0xFF,0xE040);
    rfd_XcvRegForceWriteSingle(0xFF,0xA641);
    rfd_XcvRegForceWriteSingle(0xFF,0x9342);
    rfd_XcvRegForceWriteSingle(0xFF,0x6943);
    rfd_XcvRegForceWriteSingle(0xFF,0x375A);
    rfd_XcvRegForceWriteSingle(0xFF,0x015B);
    rfd_XcvRegForceWriteSingle(0xFF,0x4012);
    rfd_XcvRegForceWriteSingle(0xFF,0xFF13);
    rfd_XcvRegForceWriteSingle(0xFF,0xCCB3);
    rfd_XcvRegForceWriteSingle(0xFF,0x94B3);
    rfd_XcvRegForceWriteSingle(0xFF,0xB418);
    rfd_XcvRegForceWriteSingle(0xFF,0xF818);
    rfd_XcvRegForceWriteSingle(0xFF,0xF9BD);
    rfd_XcvRegForceWriteSingle(0xFF,0xA8BD);
    rfd_XcvRegForceWriteSingle(0xFF,0x2892);
    rfd_XcvRegForceWriteSingle(0xFF,0x4992);
    rfd_XcvRegForceWriteSingle(0xFF,0xC944);
    rfd_XcvRegForceWriteSingle(0xFF,0x3044);
    rfd_XcvRegForceWriteSingle(0xFF,0xB050);
    rfd_XcvRegForceWriteSingle(0xFF,0x0250);
    rfd_XcvRegForceWriteSingle(0xFF,0x8252);
    rfd_XcvRegForceWriteSingle(0xFF,0x0052);
    rfd_XcvRegForceWriteSingle(0xFF,0x80AA);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x3897);
    rfd_XcvRegForceWriteSingle(0xFF,0x4C07);
    rfd_XcvRegForceWriteSingle(0xFF,0xBC7C);
    rfd_XcvRegForceWriteSingle(0xFF,0x15EE);
    rfd_XcvRegForceWriteSingle(0xFF,0x1E22);
    rfd_XcvRegForceWriteSingle(0xFF,0x1FC0);
    rfd_XcvRegForceWriteSingle(0xFF,0x9046);
    rfd_XcvRegForceWriteSingle(0xFF,0x91DF);
    rfd_XcvRegForceWriteSingle(0xFF,0x40A1);
    rfd_XcvRegForceWriteSingle(0xFF,0x41FC);
    rfd_XcvRegForceWriteSingle(0xFF,0x429F);
    rfd_XcvRegForceWriteSingle(0xFF,0x43CA);
    rfd_XcvRegForceWriteSingle(0xFF,0x5A01);
    rfd_XcvRegForceWriteSingle(0xFF,0x5B40);
    rfd_XcvRegForceWriteSingle(0xFF,0x12FF);
    rfd_XcvRegForceWriteSingle(0xFF,0x13CC);
    rfd_XcvRegForceWriteSingle(0xFF,0xB394);
    rfd_XcvRegForceWriteSingle(0xFF,0xB3B4);
    rfd_XcvRegForceWriteSingle(0xFF,0x18F8);
    rfd_XcvRegForceWriteSingle(0xFF,0x18F9);
    rfd_XcvRegForceWriteSingle(0xFF,0xBDA8);
    rfd_XcvRegForceWriteSingle(0xFF,0xBD28);
    rfd_XcvRegForceWriteSingle(0xFF,0x9249);
    rfd_XcvRegForceWriteSingle(0xFF,0x92C9);
    rfd_XcvRegForceWriteSingle(0xFF,0x4430);
    rfd_XcvRegForceWriteSingle(0xFF,0x44B0);
    rfd_XcvRegForceWriteSingle(0xFF,0x5002);
    rfd_XcvRegForceWriteSingle(0xFF,0x5082);
    rfd_XcvRegForceWriteSingle(0xFF,0x5200);
    rfd_XcvRegForceWriteSingle(0xFF,0x5280);
    rfd_XcvRegForceWriteSingle(0xFF,0xAA00);
    rfd_XcvRegForceWriteSingle(0xFF,0x00D5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BFD);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0B13);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50B);
    rfd_XcvRegForceWriteSingle(0xFF,0xFD22);
    rfd_XcvRegForceWriteSingle(0xFF,0x750B);
    rfd_XcvRegForceWriteSingle(0xFF,0xC3D5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BFD);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BEA);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50B);
    rfd_XcvRegForceWriteSingle(0xFF,0xFD22);
    rfd_XcvRegForceWriteSingle(0xFF,0xE50B);
    rfd_XcvRegForceWriteSingle(0xFF,0x850B);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CF5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BD5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BFD);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50C);
    rfd_XcvRegForceWriteSingle(0xFF,0xF822);
    rfd_XcvRegForceWriteSingle(0xFF,0x750C);
    rfd_XcvRegForceWriteSingle(0xFF,0x0A75);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BC3);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50B);
    rfd_XcvRegForceWriteSingle(0xFF,0xFDD5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CF7);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0C32);
    rfd_XcvRegForceWriteSingle(0xFF,0x750B);
    rfd_XcvRegForceWriteSingle(0xFF,0xC3D5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BFD);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50C);
    rfd_XcvRegForceWriteSingle(0xFF,0xF722);
    rfd_XcvRegForceWriteSingle(0xFF,0x750C);
    rfd_XcvRegForceWriteSingle(0xFF,0x6475);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BC3);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50B);
    rfd_XcvRegForceWriteSingle(0xFF,0xFDD5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CF7);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x0D02);
    rfd_XcvRegForceWriteSingle(0xFF,0x750C);
    rfd_XcvRegForceWriteSingle(0xFF,0xC375);
    rfd_XcvRegForceWriteSingle(0xFF,0x0BFA);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50B);
    rfd_XcvRegForceWriteSingle(0xFF,0xFDD5);
    rfd_XcvRegForceWriteSingle(0xFF,0x0CF7);
    rfd_XcvRegForceWriteSingle(0xFF,0xD50D);
    rfd_XcvRegForceWriteSingle(0xFF,0xF122);
    rfd_XcvRegForceWriteSingle(0xFF,0x2222);
    rfd_XcvRegForceWriteSingle(0xFF,0x2275);
    rfd_XcvRegForceWriteSingle(0xFF,0x8800);
    rfd_XcvRegForceWriteSingle(0xFF,0x3200);
    rfd_XcvRegForceWriteSingle(0xFF,0x7588);
    rfd_XcvRegForceWriteSingle(0xFF,0x0032);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
    rfd_XcvRegForceWriteSingle(0xFF,0x0000);
#endif
    hal_TimDelay1(1 MS_WAITING);  // delay 1 ms
    rfd_XcvRegForceWriteSingle(0x30, 0x0101);  //DL=0, rst=1
    hal_TimDelay1(1 MS_WAITING);  // delay 1 ms
    rfd_XcvRegForceWriteSingle(0x30, 0x0003);
    hal_TimDelay1(1 MS_WAITING);  // delay 10 ms
    rfd_XcvRegForceWriteSingle(0x00, 0xff7f);
    hal_TimDelay1(1 MS_WAITING);  // delay 1 ms

    rfd_XcvRegForceWriteSingle(0x00, 0xffff);

}
VOID sysSwitchTo26M(VOID)
{
    //sendEvent(0xaaaa1000);
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_OSCILLATOR
                             | SYS_CTRL_SYS_SEL_FAST_SLOW;
#ifndef SIMU
    while ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_RF_DETECTED_OK) == 0);
#endif
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_SLOW;
    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    // sendEvent(0xaaaa1001);
}
VOID sysSetupPLL(VOID)
{
    BOOL g_bootSysTcuRunningAt26M = TRUE; //boot_SysTcuClockIs26M();

    UINT32 tcuClock;
#ifdef CHIP_HAS_AP
    if (g_bootSysTcuRunningAt26M)
    {
        tcuClock = SYS_CTRL_TCU_13M_L_26M;
    }
    else
    {
        tcuClock = SYS_CTRL_TCU_13M_L_13M;
    }
#else // !CHIP_HAS_AP
    if (g_bootSysTcuRunningAt26M)
    {
        tcuClock = SYS_CTRL_SYS_SEL_TCU_26M;
    }
    else
    {
        tcuClock = SYS_CTRL_SYS_SEL_TCU_13M;
    }
#endif // !CHIP_HAS_AP

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    // Avoid power up PLL twice
    // ( If PLL must be powered up again, first the clock must be switched to 26M.
    //   Then the external RAM is considered unaccessible, meaning that the
    //   related codes should be located in internal SRAM.
    //   After that PLL can be powered down and then powered up as usual.)
    if ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0)
    {

#ifdef HAL_SYS_SETUP_CLOCK_32K

        // Turning off the Pll and reset of the Lock detector
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_PLL_BYPASS_PASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET
                                ;

#else // !HAL_SYS_SETUP_CLOCK_32K

        // Turning off the Pll and reset of the Lock detector
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_PLL_BYPASS_PASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
#ifndef CHIP_HAS_AP
                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET
#endif
                                ;

#ifdef USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_PLL_BYPASS_BYPASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET
                                | SYS_CTRL_BB_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_BB_PLL_BYPASS_BYPASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_ENABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_RESET;
#else // !USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_ENABLE
                                | SYS_CTRL_PLL_BYPASS_PASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_NO_RESET
#ifndef CHIP_HAS_AP
                                | SYS_CTRL_BB_PLL_ENABLE_ENABLE
                                | SYS_CTRL_BB_PLL_BYPASS_PASS
                                | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_BB_PLL_LOCK_RESET_NO_RESET
#endif
                                ;
#endif // !USE_DIG_CLK_ONLY_AT_IDLE

        // Wait for lock.
#ifndef SIMU
        // sendEvent(0xaaaa2000);
        while((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0);
        //sendEvent(0xaaaa2001);
#endif

        // Allow clock through
#ifndef USE_DIG_CLK_ONLY_AT_IDLE
        hwp_sysCtrl->Pll_Ctrl |= SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE
#ifndef CHIP_HAS_AP
                                 | SYS_CTRL_BB_PLL_CLK_FAST_ENABLE_ENABLE
#endif
                                 ;
#endif // !USE_DIG_CLK_ONLY_AT_IDLE

#ifdef USE_DIGITAL_CLK
        hwp_sysCtrl->Sel_Clock |= (SYS_CTRL_DIGEN_H_ENABLE | SYS_CTRL_MPMC_SYS_SAME_ENABLE);
#endif // USE_DIGITAL_CLK

#endif // !HAL_SYS_SETUP_CLOCK_32K

    } // (hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0

    //sendEvent(0xaaaa2002);
    // Select clock source
#ifdef FPGA

    // Only 39 or 52 MHz available through PLL on FPGA:
    // - 26 MHz is done with the RF clock.
    // - 39 or 52 Mhz is done with the Pll.
#ifdef HAL_SYS_SETUP_CLOCK_26M
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_SLOW
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_SLOW
#endif
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
                             | tcuClock
                             ;
#else // !HAL_SYS_SETUP_CLOCK_26M
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_FAST
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_FAST
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#endif
                             | tcuClock
                             ;
#endif // !HAL_SYS_SETUP_CLOCK_26M

#else // !FPGA

    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF
                             | SYS_CTRL_SYS_SEL_FAST_FAST
#ifdef USE_DIGITAL_CLK
                             | SYS_CTRL_DIGEN_H_ENABLE
#endif // USE_DIGITAL_CLK
#ifndef CHIP_HAS_AP
                             | SYS_CTRL_BB_SEL_FAST_FAST
                             | SYS_CTRL_MPMC_SYS_SAME_ENABLE
#endif
                             | tcuClock
                             ;

#endif // !FPGA

#ifdef SIMU
    hwp_sysCtrl->Sel_Clock |= SYS_CTRL_PLL_BYPASS_LOCK;
#endif

    // sendEvent(0xaaaa2003);

    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}
PUBLIC VOID ispiOpen(CONST BOOT_ISPI_CFG_T* spiConfigPtr)
{
    UINT32  clkDivider  = 0;
    UINT32  cfgReg      = 0;
    UINT32  ctrlReg     = 0;
    UINT32  ispiFreq    = 0;

    // When no configuration is passed, use the default
    // romed one.
    if (spiConfigPtr == NULL)
    {
        spiConfigPtr = boot_IspiGetRomedCfg();
    }

    //  Configuration setter.
    cfgReg =
        ((spiConfigPtr->csPmuActiveLow ? SPI_CS_POLARITY_0_ACTIVE_LOW:
          SPI_CS_POLARITY_0_ACTIVE_HIGH)
         | (spiConfigPtr->csAbbActiveLow ? SPI_CS_POLARITY_1_ACTIVE_LOW:
            SPI_CS_POLARITY_1_ACTIVE_HIGH)
         | (spiConfigPtr->csFmActiveLow ? SPI_CS_POLARITY_2_ACTIVE_LOW:
            SPI_CS_POLARITY_2_ACTIVE_HIGH));
    // Calculate divider.
    ispiFreq = spiConfigPtr->spiFreq;
#if 0
    // In the romed boot code, the only supported
    // system frequency is 26 MHz. In case of system frequency change, a not romed
    // function will be implemented and update the divider accordingly. (Round up)
    clkDivider = (BOOT_ISPI_ROMED_SYSTEM_FREQUENCY + (2 * ispiFreq - 1)) / ( 2 * ispiFreq);


    // There is a plus one in the register.
    clkDivider = (clkDivider) ? clkDivider-1 : 0;
#else
    clkDivider = 0;
#endif
    // Update divider in the registry image.
    cfgReg &=  ~SPI_CLOCK_DIVIDER_MASK;
    // NOTE:
    // (Not to use clock limiter any more, but 8809 ROM has been built ...)
    cfgReg |= SPI_CLOCK_DIVIDER(clkDivider);


    ctrlReg =
        (
            SPI_ENABLE
            // CS: We use the configuration from the data (tx Fifo) register.
            | SPI_CS_SEL(0)
            // Input enable: We use the configuration from the data (tx Fifo) register.
            | 0
            | (spiConfigPtr->clkFallEdge?SPI_CLOCK_POLARITY:0)
            | SPI_CLOCK_DELAY(spiConfigPtr->clkDelay )
            | SPI_DO_DELAY(spiConfigPtr->doDelay )
            | SPI_DI_DELAY(spiConfigPtr->diDelay )
            | SPI_CS_DELAY(spiConfigPtr->csDelay )
            | SPI_CS_PULSE(spiConfigPtr->csPulse )
            | SPI_FRAME_SIZE(spiConfigPtr->frameSize - 1 )
            // Normally No Output Enable Delay on ISPI, but needed for FPGA
            | SPI_OE_DELAY(spiConfigPtr->oeRatio)
            // Control comes from the data.
            | SPI_CTRL_DATA_MUX_SEL_DATA_REG_SEL
            // Input selection: We use the data register.
            | SPI_INPUT_SEL(0)
        );


    // Activate the ISPI.
    hwp_ispi->cfg  = cfgReg;
    hwp_ispi->ctrl = ctrlReg;

    // No IRQ.
    hwp_ispi->irq = 0;
}

VOID sysSwitchTo32K(VOID)
{
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_OSCILLATOR
                             | SYS_CTRL_SYS_SEL_FAST_SLOW
                             | SYS_CTRL_RF_DETECT_RESET;
    //hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}
PROTECTED VOID ispiWrite(BOOT_ISPI_CS_T csId, UINT32 regIdx, UINT32 value)
{
#ifndef SIMU
    UINT32 wrData;

    wrData = (0<<25) | ((regIdx & 0x1ff) << 16) | (value&0xffff);

    while(boot_IspiTxFifoAvail() < 1 ||
            boot_IspiSendData(csId, wrData, FALSE) == 0);

    //wait until any previous transfers have ended
    while(!boot_IspiTxFinished());
#endif
}

extern UINT32 _boot_sys_sram_bss_start;
extern UINT32 _boot_sys_sram_bss_end;
extern UINT32 _boot_ram_bss_start;
extern UINT32 _boot_ram_bss_end;
PROTECTED int main(VOID)
{
    UINT32 *dst;
    // Clear booting BSS in internal SRAM
    for (dst = (UINT32*) &_boot_sys_sram_bss_start;
            dst < (UINT32*) &_boot_sys_sram_bss_end;
            dst++)
    {
        *dst = 0;
    }
    // Clear booting BSS in External RAM
    for (dst = (UINT32*) &_boot_ram_bss_start;
            dst < (UINT32*) &_boot_ram_bss_end;
            dst++)
    {
        *dst = 0;
    }
    // Clear BSS in Internal SRAM.
    for (dst = (UINT32*) &_sys_sram_bss_start;
            dst < (UINT32*) &_sys_sram_bss_end;
            dst++)
    {
        *dst = 0;
    }
    // clear BSS in External RAM
    for (dst = (UINT32*) &_ram_bss_start;
            dst < (UINT32*) &_ram_bss_end;
            dst++)
    {
        *dst = 0;
    }

    sysSwitchTo32K();

    ispiOpen(NULL);
    rfspiOpen(&g_spiCfg);
    if (GET_BITFIELD(hwp_configRegs->CHIP_ID, CFG_REGS_METAL_ID) == 0x00)
    {

        ispiWrite(BOOT_ISPI_CS_PMU, 0x01, 0x008b);
        ispiWrite(BOOT_ISPI_CS_PMU, 0x03, 0x9FDF); // pu_vibr=0
        ispiWrite(BOOT_ISPI_CS_PMU, 0x16, 0x5080); // bypass 3p2 det
        ispiWrite(BOOT_ISPI_CS_PMU, 0x0D, 0x802D); // DDR_1V2=PWM MODE
        ispiWrite(BOOT_ISPI_CS_PMU, 0x2F, 0x8844); // vcore=8
        ispiWrite(BOOT_ISPI_CS_PMU, 0x04, 0x8d70); // v_asw 1.8V, v_lcd 2.8V

        hwp_configRegs->Alt_mux_select = CFG_REGS_LPSCO_1_LPSCO_1; // GPIO_A_7
    }
    else if (GET_BITFIELD(hwp_configRegs->CHIP_ID, CFG_REGS_METAL_ID) == 0x01)
    {
        ispiWrite(BOOT_ISPI_CS_PMU, 0x0D, 0x802D); // DDR_1V2=PWM MODE
        ispiWrite(BOOT_ISPI_CS_PMU, 0x2F, 0x8844); // vcore=8
    }
    sysSwitchTo26M();
    sysSetupPLL();
    rfd_XcvRfInit();


    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_104M);


    *(volatile UINT32 *)(0x81c0000c)=hal_TimGetUpTime();

    if (!fp_CheckDataBuffer())
    {
        *(volatile UINT32 *)(0x81c00000)=0x12344444;
        SEND_EVENT(EVENT_FLASH_PROG_MEM_RESET);
        hal_SysResetOut(TRUE);
        pmd_EnableMemoryPower(FALSE);
        hal_TimDelay(2 * HAL_TICK1S);
        pmd_EnableMemoryPower(TRUE);
        hal_TimDelay(10);
        hal_SysResetOut(FALSE);
        hal_TimDelay(10);

        //       memd_FlashOpen(tgt_GetMemdFlashConfig());
        memd_RamOpen(tgt_GetMemdRamConfig());

        if (!fp_CheckDataBuffer())
        {
            SEND_EVENT(EVENT_FLASH_PROG_MEM_ERROR);
            *(volatile UINT32 *)(0x81c00000)=0x12355555;
            while(1);
        }
    }

    // Record the communication structure into HAL Mapping Engine
    hal_MapEngineRegisterModule(HAL_MAP_ID_FLASH_PROG, &g_flashProgVersion, &g_flashProgDescr);


    *(volatile UINT32 *)(0x81c00000)=0x11223344;

    g_dataBuffer_start=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);

    psram_check_size =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) | ERamSize_Valid_Check((UINT16*)hal_EbcGetCsAddress(HAL_EBC_SRAM));


    *(volatile UINT32 *)(0x81c00014)=psram_check_size;
    *(volatile UINT32 *)(0x81c0001c)=g_dataBuffer_start;


    for(int i =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) ,j=15; i<psram_check_size; i=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<j),j++)
    {
        *(volatile UINT32 *)i=0xaaaa5555;
    }

    for(int i =(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) ,j=15; i<psram_check_size; i=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<j),j++)
    {
        *(volatile UINT32 *)i=i;
        for(int k =i,m=15; k<psram_check_size; k=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM) |(1<<m),m++)
        {
            if((*(volatile UINT32 *)k==*(volatile UINT32 *)i)&(k!=i))
            {
                hal_HstSendEvent(k);
                hal_HstSendEvent(i);
                *(volatile UINT32 *)(0x81c00000)=0x11223345;
                while(1);
            }
        }
        *(volatile UINT32 *)i=0x5555aaaa;
    }


    if(psram_check_size <= g_dataBuffer_start)
    {
        *(volatile UINT32 *)(0x81c00000)=0x11112222;
        SEND_EVENT(psram_check_size);
        SEND_EVENT(g_dataBuffer_start);
        SEND_EVENT(0x06210222);
        while(1);
    }

    g_dataBuffer_start=(g_dataBuffer_start&(~0xa0000000))|0x80000000;


    if(psram_check_size>((UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000))
    {
        psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

        if(!fp_CheckDataBuffer_HIGH_ADDR())
        {
            *(volatile UINT32 *)(0x81c00000)=0x12366666;
            while(1);
        }
        psram_check_size=(UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM)|0x00400000;
    }

    psram_check_size=(psram_check_size&(~0xa0000000))|0x80000000;

    if(*(volatile UINT32 *)(0x81c00060)==0x12345678)
    {
        if((*(volatile UINT32 *)0x81c00068 & 0xff00ffff) == 0)
            psram_check_size=(*(volatile UINT32 *)(0x81c00068)&(~0xa0000000))|0x80000000;
    }


    *(volatile UINT32 *)(0x81c00018)=psram_check_size;
    *(volatile UINT32 *)(0x81c00000)=0x55667788;
    *(volatile UINT32 *)(0x81c00010)=hal_TimGetUpTime();

    fp_CheckPsram();
    *(volatile UINT32 *)(0xa1b00020)=hal_TimGetUpTime();
    hal_HstSendEvent(*(volatile unsigned int *)0xa1a02014);
    while (1);
}

