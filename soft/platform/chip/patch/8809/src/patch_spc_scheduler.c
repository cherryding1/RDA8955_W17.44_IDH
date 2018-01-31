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


#ifdef ENABLE_PATCH_SPC_SCHEDULERMAIN

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
// TOP
#include "cs_types.h"
#include "gsm.h"
#include "baseband_defs.h"
#include "chip_id.h"
// SPC
#include "spc_ctx.h"
#include "spc_private.h"
#include "spc_mailbox.h"
#include "spcp_debug.h"
#include "spcp_spp_cfg.h"
#include "spc_profile_codes.h"
// SPAL
#include "spal_comregs.h"
#include "spal_irq.h"
#include "spal_rfif.h"
// CHIP
#include "global_macros.h"
#include "sys_ctrl.h"
#include "sys_irq.h"
#include "bb_irq.h"
#include "bb_ifc.h"
#include "tcu.h"
#include "rf_if.h"

#include "spal_mem.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
// interleaver buffer
extern PROTECTED SPC_ITLV_BUFFERS_T g_spcItlvBufTab;
// Global Context Structure statically placed in BB_SRAM
// Should be referenced by a pointer placed in a reserved register
// to ease address calculation and reduce code size.
extern PUBLIC SPC_CONTEXT_T g_spcCtxStruct;
// static RF samples reception buffers (uncached data section)
extern PROTECTED SPC_STATIC_BUFFERS_T g_spcStaticBufTab;

extern BOOL g_spcIfc2Done;

// Rx slot number to identify whether multislot window programming is finished.
// TODO:
// Move the variable to SPC context structure and named it as rx_slot_nb,
// and rename the original rxSlotNb as sched_rx_slot_nb.
extern VOLATILE UINT32 g_spcRxSlotNb;

#ifdef CHIP_HAS_EXTRA_WINDOWS
extern SPC_CONTEXT_EXT_T g_spcCtxStructExt;
#endif

extern VOLATILE BOOL SPAL_DATA_INTERNAL g_spcRxCountTcu;
//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
PROTECTED VOID spc_GnrlReset(VOID);

//=============================================================================
// spc_SchedulerMain()
//-----------------------------------------------------------------------------
/// This function performs the scheduling for Signal Processing Control.
//=============================================================================

PROTECTED VOID SPAL_FUNC_INTERNAL patch_spc_SchedulerMain(VOID)
{
    // Place Global Context address in a reserved register.
    //----------------------------------------------------------
    // CAUTION
    //----------------------------------------------------------
    // This must be done at the entrance (before g_spcCtx is referenced) of
    // each task and each IRQ handler that are using g_spcCtx.
    //----------------------------------------------------------
    g_spcCtx = &g_spcCtxStruct;

    // general reset for context variables & buffers
    spc_GnrlReset();

    // main loop
    while (1)
    {
#if (SPC_IF_VER != 0)
        //--------------------------
        //    Call back hook
        //--------------------------
        // Call a function defined by the XCPU, when the BCPU
        // doesn't have anything to do. This can be useful to add
        // things to do to the BCPU.
        // We do that only when there is no pending IRQ.
        if (g_spcCtx->schedulerHook != NULL &&
                g_spcCtx->bcpuIrqCause == 0)
        {
            ((VOID(*)(VOID))g_spcCtx->schedulerHook)();
        }
#endif // (SPC_IF_VER != 0)

        //--------------------------
        //    check sleep mode
        //--------------------------
        volatile UINT32 critical_sec;
        critical_sec = spal_IrqEnterCriticalSection();
        if (g_spcCtx->bcpuIrqCause == 0)
        {
            // shutdown if nothing to do
            SPC_PROFILE_WINDOW_EXIT(BCPU_RUNNING);
            UINT32 clk_bb_disable;
            spal_IrqWriteCpuSleep((UINT32) BB_IRQ_SLEEP);
            // Flush the write buffer
            clk_bb_disable = spal_IrqReadCpuSleep();
            SPC_PROFILE_WINDOW_ENTER(BCPU_RUNNING);
        }
        spal_IrqExitCriticalSection(critical_sec);
        //flush write buffer
        critical_sec=spal_ComregsGetSnap();

        //----------------------------------------------------------------
        //                          FINT
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        // IFC2 service must be prioritary on Fint service for late Rx Window treatment
        if (  (g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_FRAME) &&
                (!(g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_IFC2)||(g_spcCtx->frame_over==1)))
        {

            //HAL_W2SET(W2_FINT_SERV);
            SPC_PROFILE_WINDOW_ENTER(FRAME);

            critical_sec = spal_IrqEnterCriticalSection();
            g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_FRAME;
            spal_IrqExitCriticalSection(critical_sec);

            if (g_spcCtx->sendEqItr == 1)
            {
                g_spcCtx->sendEqItr = 0;
                spal_ComregsSetIrq(MBOX_DSP_EQUALIZATION_COMPLETE);
                SPC_PROFILE_PULSE(DSP_EQUALIZATION_COMPLETE);
            }

            spc_UpdateOnFint();

            if (g_spcCtx->txQty > 0)
            {
                spc_TxProcess();
            }

            if (g_spcCtx->rxQty > 0)
            {
#if (SPC_IF_VER < 5)
                g_spcCtx->rxslotNb = (UINT8)g_spcRxSlotNb;
#else
                g_spcCtx->sched_rx_slot_nb = g_spcCtx->rx_slot_nb;
#endif
            }
#if (CHIP_EDGE_SUPPORTED == 1)
            else
            {
                if(g_spcCtx->egprs_rx_buffer_bmp)
                {
                    UINT8 i;
                    UINT8 j;

                    // if we decode the third or the fourth block we need to recopy
                    // the fourth burst softbits previously saved at N+1
                    // if we execute those lines we are at N+2
                    for(i=0; i<2; i++)
                    {
                        UINT32* tmp1=&(g_Egprs_Softbit_Buff[87*i]);
                        UINT32* tmp2=g_spcCtx->ItlvBufRxCtrl+(i+2)*SPC_ITLV_RX_BUFF_OFFSET;
                        for(j=0; j<87; j++)
                        {
                            tmp2[j]=tmp1[j];
                        }
                    }
                    for(i=0; i<2; i++)
                    {
                        for (j=0; j<g_spcCtx->sched_rx_slot_nb; j++)
                        {
                            if ((g_spcCtx->egprs_rx_buffer_bmp)&(0x1<<j))
                            {
                                break;
                            }
                        }
                        spc_EgprsBlockDecode(j,i);
                        // reset the decoded buffer in the bitmap
                        g_spcCtx->egprs_rx_buffer_bmp &= ~(0x1<<j);
                        // send an it to the XCPU for the end of Block Decoding if the last
                        // buffer has been decoded
                        if (g_spcCtx->egprs_rx_buffer_bmp==0)
                        {
#ifdef __USE_NB_DECODE_IRQ__
                            if(g_spcCtx->statWinRxCfg[j].itr &
                                    MBOX_DSP_NB_DECODING_COMPLETE)
                            {
                                spal_ComregsSetIrq(MBOX_DSP_NB_DECODING_COMPLETE);
                            }
#endif
                            break;
                        }
                    }
                }
            }
#endif
            //HAL_W2CLR(W2_FINT_SERV);
            SPC_PROFILE_WINDOW_EXIT(FRAME);

        }//endif FINT

        //----------------------------------------------------------------
        //        IFC2 TCU0
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        if (g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_TCU1)
        {
            SPC_PROFILE_WINDOW_ENTER(TCU1);
            g_spcCtx->sched_rx_count = g_spcRxCountTcu;
            g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_TCU1;
            SPC_PROFILE_WINDOW_EXIT(TCU1);
        }
        //----------------------------------------------------------------
        //        IFC2 RF RX
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        if (g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_IFC2)
        {

            //HAL_W2SET(W2_IFC2_SERV);
            SPC_PROFILE_WINDOW_ENTER(RF_IFC);

            critical_sec = spal_IrqEnterCriticalSection();
            // AVOID missing an IFC2 IT if 2 (or more) of them happened
            // during last IFC2 servicing
            if(!(-- g_spcCtx->ifc2_pending))
                g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_IFC2;
            spal_IrqExitCriticalSection(critical_sec);

            spc_RxProcess();

            SPC_PROFILE_WINDOW_EXIT(RF_IFC);
            //HAL_W2CLR(W2_IFC2_SERV);
        } //end if IFC
    } //end while(1)
} //end main


#endif // ENABLE_PATCH_SPC_SCHEDULERMAIN
