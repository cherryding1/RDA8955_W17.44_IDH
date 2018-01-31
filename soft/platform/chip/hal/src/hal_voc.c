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
#include "chip_id.h"

#include "global_macros.h"
#include "voc_ahb.h"
#include "voc_cfg.h"
#include "voc_ram.h"
#include "sys_ctrl.h"
#ifdef CHIP_HAS_AP
#include "sys_ctrl_ap.h"
#include "hal_ebc.h"
#endif
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#include "hal_cache.h"
#endif
#include "hal_mem_map.h"
#include "hal_error.h"
#include "hal_voc.h"
#include "halp_voc.h"
#include "halp_debug.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "halp_sys.h"
#include "sxr_tls.h"
#include "reg_alias.h"

#define CACHED(addr)                ((addr)|0x80000000)
//  ---------- Internal defines ----------

// Modem uses one VOC only
#if !defined(hwp_vocAhb) && defined(hwp_voc1Ahb)
#define hwp_vocAhb hwp_voc1Ahb
#define hwp_vocCfg hwp_voc1Cfg
#define hwp_vocRam hwp_voc1Ram
#endif

// VOC core internal status is not stable even if VOC is idle,
// so the VOC sema is not reliable to control exclusivly mutual VOC access.
#define CHECK_VOC_CFG_SEMA 0

#define toVocLAddr(n)    (((INT32)(n)>>1) & VOC_AHB_DMA_LADDR_MASK)
#define toVocEAddr(n)    (((INT32)(n))    & VOC_AHB_DMA_EADDR_MASK)

//#define HAL_VOC_DEBUG

#ifdef HAL_VOC_DEBUG
#define VOCAHB_DMA_SIZE 16
typedef struct
{
    u32 dmaEaddr;
    u32 dmaLaddr;
    u32 dmacfg;
} HAL_VOCAHB_DMA_T;
#endif
//  ---------- Global variables ----------


// Global variable storing the voc irq handler
PRIVATE HAL_VOC_IRQ_HANDLER_T g_halVocRegistry   = NULL;

// Global variable containing the last loaded VoC code
PRIVATE INT32 * g_halVocLoadedCode  = NULL;

// Global variable containing the pcVal corresponding to the loaded VoC code
PRIVATE UINT16 g_halVocPcVal  = 0;

// Global variables containing the critical section range
PRIVATE UINT16 g_halVocPcValCriticalSecMin  = 0;
PRIVATE UINT16 g_halVocPcValCriticalSecMax  = 0;

// Global variable telling if the open procedure is finished
PRIVATE VOLATILE HAL_VOC_STATE_T g_halVocState  = HAL_VOC_CLOSE_DONE;
#ifdef HAL_VOC_DEBUG
HAL_VOCAHB_DMA_T g_vocAhb[VOCAHB_DMA_SIZE];
u32 g_vocAhb_pos = 0;
#endif

static HAL_CLK_T *gVocClk = NULL;
//enable access flash or not.default not.
static BOOL g_halVocEnableFlashAccess = false;
//flash start 0x88000000
#define FLASH_MEMMAP_MASK 0x08000000
//  ---------- Functions ----------


PUBLIC VOID hal_VocInit (VOID)
{
#ifdef CHIP_HAS_AP

    // Never touch REG_DBG register, which is managed by AP
    hwp_apSysCtrl->AXI_Rst_Set = SYS_CTRL_AP_SET_AXI_RST_VOC;

#else // !CHIP_HAS_AP

    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->BB_Rst_Set = SYS_CTRL_SET_RST_VOC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);

    HAL_SYS_VOC_FREQ_T vocFreq;

#ifdef FPGA
    vocFreq = HAL_SYS_VOC_FREQ_26M;
#else
    vocFreq = HAL_SYS_VOC_FREQ_78M;
#endif


    if (!hal_ClkIsEnabled(gVocClk)) {
        hal_ClkEnable(gVocClk);
        hal_ClkSetRate(gVocClk, vocFreq);
        hal_ClkDisable(gVocClk);
    }
#endif // !CHIP_HAS_AP
}


PUBLIC HAL_ERR_T hal_VocOpen (CONST HAL_VOC_CFG_T * pCfg)
{
    union
    {
        UINT32 reg;
        HAL_VOC_WAKEUP_EVENT_T bitfield;
    } event;

    union
    {
        UINT32 reg;
        HAL_VOC_IRQ_STATUS_T bitfield;
    } irq;

    HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocOpen : starting open procedure...");

    BOOL openVoc;

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DcacheFlushAll(FALSE);
#endif

    UINT32 status = hal_SysEnterCriticalSection();
    openVoc = (g_halVocState == HAL_VOC_CLOSE_DONE);
#ifdef CHIP_HAS_AP
    openVoc = (openVoc && !(hwp_apSysCtrl->AXI_Rst_Clr & SYS_CTRL_AP_CLR_AXI_RST_VOC));
#endif
    if (openVoc)
    {
        g_halVocState = HAL_VOC_OPEN_STARTED;
#ifdef CHIP_HAS_AP
        // Never touch REG_DBG register, which is managed by AP
        hwp_apSysCtrl->AXI_Rst_Clr = SYS_CTRL_AP_CLR_AXI_RST_VOC;
#else
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
        hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_VOC;
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
#endif
    }
    hal_SysExitCriticalSection(status);

    if (!openVoc)
    {
        HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0,
                  "hal_VocOpen : exiting on ERROR, voc state busy: %d",
                  g_halVocState);
        return HAL_ERR_RESOURCE_BUSY;
    }

    // -----------------------------------
    // Check if all resources are available
    // -----------------------------------
    UINT32 semaAvail = TRUE;
#if (CHECK_VOC_CFG_SEMA)
    semaAvail = hwp_vocCfg->Sema;
#elif defined(CHIP_HAS_AP)
    // MEM_SC is not usable on 8810
    //semaAvail = hal_MemScTake(HAL_MEM_SC_AP_VOC);
#endif
    if (!semaAvail)
    {
        // The VoC semaphore is used as a general semaphore
        // for the whole open procedure. If its value is 0,
        // it means that we are attempting to open the VoC
        // module while it is already opened (and not closed).

        // someone else (not XCPU) is using VOC
        // reset the software state but keep current hardware state
        g_halVocState = HAL_VOC_CLOSE_DONE;

        HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocOpen : exiting on ERROR, voc sema already taken.");
        return HAL_ERR_RESOURCE_BUSY;
    }

    // -----------------------------------
    // Start the VoC open procedure
    // -----------------------------------

    // -----------------------------------
    // Set the VoC irq mask and handler
    // -----------------------------------

    // enable voc freq
    if (!hal_ClkIsEnabled(gVocClk))
        hal_ClkEnable(gVocClk);

#ifdef CHIP_HAS_AP
    // Clear the interrupt mask and clear all BCPU cause
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_BCPU_IRQ_MASK_MASK;
    hwp_vocAhb->Irq_Status = VOC_AHB_BCPU_IRQ_STATUS_MASK;
    // Set VoC mask on BCPU irq (the unused fields should not be set)
    irq.bitfield = pCfg->irqMask;
    irq.bitfield.unused1 = 0;
    irq.bitfield.unused2 = 0;
    hwp_vocAhb->Irq_Mask_Set = (irq.reg<<8) & VOC_AHB_BCPU_IRQ_MASK_MASK;
    // VOC DMA address width is 28 bits. The upper 4 bits are determined
    // by register DMA_Addr_Offset.
    // Modem 0x8YYY-YYYY <=> AP 0x1YYY-YYYY
    hwp_vocCfg->DMA_Addr_Offset = 0x1;
#else
    // Clear the interrupt mask and clear all XCPU cause
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_XCPU_IRQ_MASK_MASK;
    hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_IRQ_STATUS_MASK;
    // Set VoC mask on XCPU irq (the unused fields should not be set)
    irq.bitfield = pCfg->irqMask;
    irq.bitfield.unused1 = 0;
    irq.bitfield.unused2 = 0;
    hwp_vocAhb->Irq_Mask_Set = (irq.reg) & VOC_AHB_XCPU_IRQ_MASK_MASK;
#endif

    // Set the user's irq handler
    g_halVocRegistry = pCfg->vocIrqHandler;


    // -----------------------------------
    // Clear the wake status
    // Set the VoC wakeup mask
    // -----------------------------------

    hwp_vocCfg->Wakeup_Status = VOC_CFG_WAKEUP_STATUS_MASK;
    event.bitfield = pCfg->eventMask;
    hwp_vocCfg->Wakeup_Mask = event.reg;

    g_halVocEnableFlashAccess = pCfg->enableFlashAccess;

    if (pCfg->vocCode == NULL)
    {
        // If the pointer is null, only the VoC DMA will be used,
        // so the semaphore will be taken.
        // The VoC DMA cannot be used if the driver is not opened.
        // Normally the VoC DMA cannot be busy at this point, but
        // this check can prevent from bad usage of the driver.
        if (hwp_vocAhb->DMA_Sema == 0)
        {
            // someone else is using VOC DMA
            g_halVocState = HAL_VOC_OPEN_FAILED;

            // disable voc freq
            hal_ClkDisable(gVocClk);

            HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocOpen : exiting on ERROR, dma sema already taken.");
            return HAL_ERR_RESOURCE_BUSY;
        }
    }

    // -----------------------------------
    // Load the VoC code (if necessary)
    // -----------------------------------

    // reload only if the new code is different.
    // If the pointer is null, only the VoC DMA will be used,
    // so there is no need to load a VoC code.
    if ((g_halVocLoadedCode != (INT32 *)pCfg->vocCode) && (pCfg->vocCode != NULL))
    {
        // update the loaded VoC code variables
        g_halVocLoadedCode          = (INT32 *)pCfg->vocCode;
        g_halVocPcVal               = pCfg->pcVal;
        g_halVocPcValCriticalSecMin = pCfg->pcValCriticalSecMin;
        g_halVocPcValCriticalSecMax = pCfg->pcValCriticalSecMax;

        // check if the semaphore is available
        if (hwp_vocAhb->DMA_Sema == 0)
        {
            // someone else is using VOC DMA
            g_halVocState = HAL_VOC_OPEN_FAILED;

            // disable voc freq
            hal_ClkDisable(gVocClk);

            HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocOpen : exiting on ERROR, dma sema already taken.");
            return HAL_ERR_RESOURCE_BUSY;
        }

        // load code in RAM_I
        hwp_vocAhb->DMA_LAddr = toVocLAddr(hwp_vocRam->voc_ram_i_base);
	if(!g_halVocEnableFlashAccess){
		HAL_ASSERT(!((INT32)pCfg->vocCode & FLASH_MEMMAP_MASK),"Voc can not access flash!(VocOpen)");
	}
        hwp_vocAhb->DMA_EAddr = toVocEAddr(pCfg->vocCode);
        hwp_vocAhb->DMA_Cfg   = VOC_AHB_DMA_RUN | VOC_AHB_DMA_SIZE(pCfg->vocCodeSize);

        if (pCfg->needOpenDoneIrq)
        {
            // set VoC DMA irq mask
#ifdef CHIP_HAS_AP
            hwp_vocAhb->Irq_Mask_Set = VOC_AHB_BCPU_DMA_IRQ_MASK;
#else
            hwp_vocAhb->Irq_Mask_Set = VOC_AHB_XCPU_DMA_IRQ_MASK;
#endif

            // return, but the open procedure is not finished
            g_halVocState = HAL_VOC_OPEN_IN_PROGRESS;

            HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocOpen : exiting, need IRQ notification to finish procedure.");
            return HAL_ERR_RESOURCE_RESET;
        }

        // TODO : set timeout
        // wait done
#ifdef CHIP_HAS_AP
        while (!(hwp_vocAhb->Irq_Status & VOC_AHB_BCPU_DMA_IRQ_STATUS))
#else
        while (!(hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_DMA_IRQ_STATUS))
#endif
        {
            //sxr_Sleep(0);
        }

        // clear status
#ifdef CHIP_HAS_AP
        hwp_vocAhb->Irq_Status = VOC_AHB_BCPU_DMA_IRQ_STATUS;
#else
        hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_DMA_IRQ_STATUS;
#endif

        // the code is loaded
        g_halVocState = HAL_VOC_OPEN_DONE;

        // but a reset is required
        HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocOpen : exiting on SUCCESS.");
        return HAL_ERR_RESOURCE_RESET;
    }

    // the code is already loaded
    g_halVocState = HAL_VOC_OPEN_DONE;

    // no further action required
    // the open procedure is finished without errors
    HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocOpen : exiting on SUCCESS.");
    return HAL_ERR_NO;
}


PUBLIC VOID hal_VocClose(VOID)
{
    BOOL closeVoc = FALSE;

    UINT32 status = hal_SysEnterCriticalSection();
    HAL_VOC_STATE_T prevVocState = g_halVocState;
    if (g_halVocState != HAL_VOC_CLOSE_DONE &&
            g_halVocState != HAL_VOC_CLOSE_STARTED &&
            g_halVocState != HAL_VOC_OPEN_STARTED)
    {
        g_halVocState = HAL_VOC_CLOSE_STARTED;
        closeVoc = TRUE;
    }
    hal_SysExitCriticalSection(status);

    if (!closeVoc)
    {
        if (prevVocState == HAL_VOC_CLOSE_DONE)
        {
            // Already closed.
            // disable voc freq
            hal_ClkDisable(gVocClk);
            HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocClose : aborting, already closed");
        }
        else
        {
            // Someone else is configuring VOC
            HAL_ASSERT(FALSE, "hal_VocClose : aborting, busy: %d", prevVocState);
            //HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocClose : aborting, busy: %d", prevVocState);
        }
        return;
    }

    // Check if the module is open
    if (prevVocState == HAL_VOC_OPEN_IN_PROGRESS)
    {
        // Closing while opening in progress...
        // If the open procedure is still ongoing,
        // it will be stopped before completion, so the code
        // state will be unknown.
        HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocClose : closing while open is in progress");
        g_halVocLoadedCode = NULL;
    }

    // Clear the variable that stores the voc irq handler.
    // No more voc irq will be handled after this point.
    g_halVocRegistry = NULL;

    // Clear the wakeup mask.
    // VoC cannot wakeup on a non-CPU event after this point.
    // This prevents from inopportune wakeup of the VoC module
    // triggered by a HW wakeup event.
    hwp_vocCfg->Wakeup_Mask = VOC_CFG_WAKEUP_MASK(0);

    // Stop the VoC module (if running) and flush its pipe.
    // VoC cannot be inopportunely woken up.
    hwp_vocCfg->Ctrl = VOC_CFG_RUN_STOP;

    // INFO : This check is normally unnecessary.
    // After this point the VoC module is stopped.
    while (hwp_vocCfg->Ctrl & VOC_CFG_RUNNING);


    // this is another possibility to stop properly the DMA ....
    // -----------------------------------------------------

    // Stop the VoC DMA (if running) without changing the DMA configuration,
    // in particular without changing a write to a read transfer,
    // which can crash the AHB.
    hwp_vocAhb->DMA_Cfg &= ~VOC_AHB_DMA_RUN;

    // Wait for the last burst (if any).
    while (hwp_vocAhb->DMA_Status & VOC_AHB_DMA_ON);

    // After this point, no interrupts can be received from VoC
    // Clear the interrupt mask and all CPU cause.
#ifdef CHIP_HAS_AP
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_BCPU_IRQ_MASK_MASK;
    hwp_vocAhb->Irq_Status = VOC_AHB_BCPU_IRQ_STATUS_MASK;
#else
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_XCPU_IRQ_MASK_MASK;
    hwp_vocAhb->Irq_Status = VOC_AHB_XCPU_IRQ_STATUS_MASK;
#endif

    // clear the wakeup status
    hwp_vocCfg->Wakeup_Status = VOC_CFG_WAKEUP_STATUS_MASK;

    // Reset g_halVocLoadedCode to NULL, so that VOC can be started next time
    // even if its PC value is invalid.
#if 0
    // if the VoC module's PC is inside the critical section corresponding
    // to the code reload function, the VoC loaded code variable is cleared
    // because the exact state of the Voc code is not known.
    if ((hwp_vocCfg->PC >= g_halVocPcValCriticalSecMin) && (hwp_vocCfg->PC < g_halVocPcValCriticalSecMax))
#endif
    {
        g_halVocLoadedCode = NULL;
    }

    // Release the semaphores (the order is important).
    hwp_vocAhb->DMA_Sema = 1;
    hwp_vocCfg->Sema = 1;

#ifdef CHIP_HAS_AP
    // Never touch REG_DBG register, which is managed by AP
    hwp_apSysCtrl->AXI_Rst_Set = SYS_CTRL_AP_SET_AXI_RST_VOC;
    hal_MemScRelease(HAL_MEM_SC_AP_VOC);
#else
    status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->BB_Rst_Set = SYS_CTRL_SET_RST_VOC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
#endif

    g_halVocEnableFlashAccess = false;
    // Clear the open procedure
    g_halVocState = HAL_VOC_CLOSE_DONE;

    HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocClose : exiting on SUCCESS");
    // disable voc freq
    hal_ClkDisable(gVocClk);

    return;
}


PUBLIC HAL_ERR_T hal_VocWakeup(HAL_VOC_WAKEUP_ID_T wakeupId)
{
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DcacheFlushAll(FALSE);
#endif

    if (g_halVocState != HAL_VOC_OPEN_DONE)
    {
        // the open procedure is not started or finished
        HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0,
                  "hal_VocWakeup : exiting on ERROR, voc not opened: %d",
                  g_halVocState);
        return HAL_ERR_RESOURCE_NOT_ENABLED;
    }

    if (wakeupId == HAL_VOC_START)
    {
        if (hwp_vocCfg->Ctrl & VOC_CFG_RUNNING)
        {
            // attempting to start VoC while it is still running
            HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0, "hal_VocWakeup : exiting on ERROR, voc is running!");
            return HAL_ERR_RESOURCE_BUSY;
        }

        // set the PC start address
        hwp_vocCfg->PC   = g_halVocPcVal;

        // set the 16-bit stack to the top of RAM_X
        hwp_vocCfg->SP16 = VOC_CFG_SP16(toVocLAddr(&(hwp_vocRam->voc_ram_x_base[VOC_RAM_X_SIZE])) - 1);

        // set the 32-bit stack to the top of RAM_Y
        hwp_vocCfg->SP32 = VOC_CFG_SP32(toVocLAddr(&(hwp_vocRam->voc_ram_y_base[VOC_RAM_Y_SIZE])) - 2);

        // start executing VoC code
        hwp_vocCfg->Ctrl = VOC_CFG_RUN_START;
    }
    else
    {
        // set a soft wakeup event
        hwp_vocCfg->Ctrl = (wakeupId == HAL_VOC_WAKEUP_EVENT_1) ? VOC_CFG_RUN_SOFTWAKEUP1 : VOC_CFG_RUN_SOFTWAKEUP0;
    }

    // HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocWakeup : exiting on SUCCESS");
    return HAL_ERR_NO;
}


PUBLIC VOID* hal_VocGetPointer(INT32 vocLocalAddr)
{
    // return an global pointer to vocLocalAddr
    return ((VOID*)(&(hwp_vocRam->voc_ram_x_base)[vocLocalAddr*2]));
}


PUBLIC INT32 * hal_VocGetDmaiPointer(INT32 * vocExternAddr, HAL_VOC_DMA_DIR_T wr1Rd0, HAL_VOC_DMA_TRANSFER_T sngl1brst0, HAL_VOC_DMA_B2S_T b2s)
{

    if(!g_halVocEnableFlashAccess){
	    HAL_ASSERT(!((INT32)vocExternAddr & FLASH_MEMMAP_MASK),"Voc can not access flash!(GetDmaiPointer)");
    }

    if (NULL == vocExternAddr)
    {
        // protect (do not create write NULL pointers)
        return NULL;
    }
    else
    {
        INT32 DmaiEAddr = ((INT32)vocExternAddr) & VOC_CFG_DMA_EADDR_MASK;

        // configure the single/burst parameter
        if (sngl1brst0 == HAL_VOC_DMA_SINGLE)
        {
            DmaiEAddr |= VOC_CFG_DMA_SINGLE;
        }

        // configure the read/write parameter
        if (wr1Rd0 == HAL_VOC_DMA_WRITE)
        {
            DmaiEAddr |= VOC_CFG_DMA_WRITE_WRITE;
        }

        // return an DMAI configuration pointer for the hwp_vocAhb->DMA_EAddr
        return (INT32 *)DmaiEAddr;
    }
}


PUBLIC HAL_ERR_T hal_VocDmaStart (CONST HAL_VOC_DMA_CFG_T * pCfg)
{
    UINT32 dmaCfg;

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DcacheFlushAddrRange(pCfg->extAddr, (UINT32)(pCfg->extAddr + pCfg->size), FALSE);
#endif

    // check if sema is available and that the open procedure is finished
    if ((hwp_vocAhb->DMA_Sema == 0) || (g_halVocState != HAL_VOC_OPEN_DONE))
    {
        HAL_TRACE(HAL_VOC_TRC|TSTDOUT, 0,
                  "hal_VocDmaStart : exiting on ERROR, voc not opened or in use: %d",
                  g_halVocState);
        return HAL_ERR_RESOURCE_BUSY;
    }

    // clear VoC DMA status & irq mask
#ifdef CHIP_HAS_AP
    hwp_vocAhb->Irq_Status   = VOC_AHB_BCPU_DMA_IRQ_STATUS;
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_BCPU_DMA_IRQ_MASK;
#else
    hwp_vocAhb->Irq_Status   = VOC_AHB_XCPU_DMA_IRQ_STATUS;
    hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_XCPU_DMA_IRQ_MASK;
#endif

    // load non_regression test
    hwp_vocAhb->DMA_LAddr = toVocLAddr(pCfg->vocLocalAddr);
    if(!g_halVocEnableFlashAccess){
	    HAL_ASSERT(!((INT32)pCfg->extAddr & FLASH_MEMMAP_MASK),"Voc can not access flash!(DmaStart)");
    }
    hwp_vocAhb->DMA_EAddr = toVocEAddr(pCfg->extAddr);

    dmaCfg = VOC_AHB_DMA_RUN | VOC_AHB_DMA_SIZE(pCfg->size);

    if (pCfg->wr1Rd0 == HAL_VOC_DMA_WRITE)
    {
        dmaCfg |= VOC_AHB_DMA_DIR_WRITE;
    }

    // if an interrupt is to be generated, enable the mask
    if (pCfg->needIrq)
    {
#ifdef CHIP_HAS_AP
        hwp_vocAhb->Irq_Mask_Set = VOC_AHB_BCPU_DMA_IRQ_MASK;
#else
        hwp_vocAhb->Irq_Mask_Set = VOC_AHB_XCPU_DMA_IRQ_MASK;
#endif
    }

    // configure the rest of the parameters and start the transfer
    hwp_vocAhb->DMA_Cfg   = dmaCfg;

    HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocDmaStart : exiting on SUCCESS");
    return HAL_ERR_NO;
}


PUBLIC BOOL hal_VocDmaDone(VOID)
{
#ifdef CHIP_HAS_AP
    return ((hwp_vocAhb->Irq_Status & VOC_AHB_BCPU_DMA_IRQ_STATUS) ? TRUE : FALSE);
#else
    return ((hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_DMA_IRQ_STATUS) ? TRUE : FALSE);
#endif
}

PUBLIC BOOL HAL_FUNC_RAM hal_GetVocDma(void)
{
#ifndef VOCCODE_IN_PSRAM
    return (g_halVocState  != HAL_VOC_CLOSE_DONE);
#else
    return FALSE;
#endif
}

// ============================================================================
// hal_VocIrqHandler
// ----------------------------------------------------------------------------
/// VoC IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by hal_VocIrqSetHandler. It also checks if the open procedure is finished
/// when the user chooses to confirm the end of the open procedure with an VoC DMA
/// interrupt
/// @param interruptId Id with which the handler is entered
// ============================================================================
PROTECTED VOID hal_VocIrqHandler(UINT8 interruptId)
{
    union
    {
        UINT32 reg;
        HAL_VOC_IRQ_STATUS_T bitfield;
    } cause;

    // read the cause
#ifdef CHIP_HAS_AP
    cause.reg = hwp_vocAhb->Irq_Status & VOC_AHB_BCPU_IRQ_CAUSE_MASK;
#else
    cause.reg = hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_IRQ_CAUSE_MASK;
#endif

    // clear all CPU cause
    hwp_vocAhb->Irq_Status = cause.reg;

    BOOL codeLoadingDone = FALSE;

    UINT32 status = hal_SysEnterCriticalSection();
    HAL_VOC_STATE_T prevVocState = g_halVocState;
    // If the open procedure is ongoing
    if (g_halVocState == HAL_VOC_OPEN_IN_PROGRESS)
    {
        // the only allowed irq during the open procedure is dmaDone
#ifdef CHIP_HAS_AP
        if (hwp_vocAhb->Irq_Status & VOC_AHB_BCPU_DMA_IRQ_STATUS)
        {
            // The code loading is done, so the open procedure is finished
            g_halVocState = HAL_VOC_OPEN_DONE;
            codeLoadingDone = TRUE;
            // Clr the VoC DMA irq mask
            hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_BCPU_DMA_IRQ_MASK;
        }
#else
        if (hwp_vocAhb->Irq_Status & VOC_AHB_XCPU_DMA_IRQ_STATUS)
        {
            // The code loading is done, so the open procedure is finished
            g_halVocState = HAL_VOC_OPEN_DONE;
            codeLoadingDone = TRUE;
            // Clr the VoC DMA irq mask
            hwp_vocAhb->Irq_Mask_Clr = VOC_AHB_XCPU_DMA_IRQ_MASK;
        }

#endif
    }
    hal_SysExitCriticalSection(status);

    if (prevVocState == HAL_VOC_OPEN_IN_PROGRESS)
    {
        if (codeLoadingDone)
        {
            HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocIrqHandler : open procedure finished on SUCCESS");
        }
    }
    else
    {

#ifdef HAL_VOC_DEBUG
        //dma is programed
        if (cause.bitfield.dmaVoc)
        {
            if(hwp_vocAhb->DMA_Cfg & VOC_AHB_DMA_DIR_WRITE)
            {
                //check overlap
                UINT32 dma_start = CACHED(hwp_vocAhb->DMA_EAddr);
                UINT32 dma_end = CACHED(hwp_vocAhb->DMA_EAddr+VOC_AHB_DMA_SIZE(hwp_vocAhb->DMA_Cfg));
                UINT32 overlap ;
                if(g_vocAhb_pos >= VOCAHB_DMA_SIZE)
                {
                    g_vocAhb_pos = 0;
                }
                g_vocAhb[g_vocAhb_pos].dmaEaddr = hwp_vocAhb->DMA_EAddr;
                g_vocAhb[g_vocAhb_pos].dmaLaddr = hwp_vocAhb->DMA_LAddr;
                g_vocAhb[g_vocAhb_pos].dmacfg = hwp_vocAhb->DMA_Cfg;
                g_vocAhb_pos++;
                overlap = MAX(dma_start, (UINT32)&_boot_ram_txt_start ) < MIN (dma_end, (UINT32)&_boot_ram_txt_end ) |
                          MAX(dma_start, (UINT32)&_ram_txt_start ) < MIN (dma_end, (UINT32)&_ram_txt_end ) ;
                HAL_ASSERT(!overlap,
                           "voc error dma:%x:%x:%x",hwp_vocAhb->DMA_LAddr,hwp_vocAhb->DMA_EAddr, hwp_vocAhb->DMA_Cfg);
            }
        }
#endif
        if (cause.bitfield.voc)
        {
            // call the user IRQ handler function.
            if (g_halVocRegistry)
            {
                // HAL_TRACE(HAL_VOC_TRC, 0, "hal_VocIrqHandler : calling user handler");
                g_halVocRegistry(&cause.bitfield);
            }
        }
    }
}


// ============================================================================
// hal_VocStateActive
// ----------------------------------------------------------------------------
/// Checks if VOC is in active state.
/// @return TRUE if VOC is active, and FALSE otherwise.
// ============================================================================
PUBLIC BOOL HAL_FUNC_RAM hal_VocStateActive(VOID)
{
    return (g_halVocState != HAL_VOC_CLOSE_DONE);
}


PUBLIC UINT32 hal_VocIntrClose(VOID)
{
    UINT32 now,voc_time;
    now = hal_TimGetUpTime();

    if(g_halVocState != HAL_VOC_OPEN_DONE)
        asm("break 7");

    UINT32 my_Wakeup_Mask = hwp_vocCfg->Wakeup_Mask;
    hwp_vocCfg->Wakeup_Mask = my_Wakeup_Mask&0xc;
    while ((hwp_vocCfg->Ctrl & VOC_CFG_RUNNING) | (hwp_vocAhb->DMA_Status & VOC_AHB_DMA_ON))
    {
        voc_time = hal_TimGetUpTime();
        if (voc_time - now >(16384>>2))
        {
            asm("break 7");
            return 0xffffffff;
        }

        if(g_halVocState != HAL_VOC_OPEN_DONE)
        {
            asm("break 7");
            return 0xffffffff;
        }
    }
    return my_Wakeup_Mask;
}


PUBLIC VOID  hal_VocIntrOpen(UINT32 my_Wakeup_Mask)
{
    if(my_Wakeup_Mask == 0xffffffff)
        return;
    UINT32 new_Wakeup_Mask = hwp_vocCfg->Wakeup_Mask;
//    hwp_vocCfg->Wakeup_Status = VOC_CFG_WAKEUP_STATUS_MASK;
    hwp_vocCfg->Wakeup_Mask = (my_Wakeup_Mask&0x33) | (new_Wakeup_Mask&0xc);
}


PUBLIC VOID hal_voc_clock_enable(VOID)
{
#ifdef CHIP_HAS_AP
    // Never touch REG_DBG register, which is managed by AP
    hwp_apSysCtrl->AXI_Rst_Clr = SYS_CTRL_AP_CLR_AXI_RST_VOC;
#else
    UINT32 status;

    status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->BB_Rst_Clr = SYS_CTRL_CLR_RST_VOC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
#endif
}

HAL_MODULE_CLK_INIT(VOC)
{
    gVocClk = hal_ClkGet(FOURCC_VOC);
    HAL_ASSERT((gVocClk!= NULL), "Clk VOC not found!");
}
