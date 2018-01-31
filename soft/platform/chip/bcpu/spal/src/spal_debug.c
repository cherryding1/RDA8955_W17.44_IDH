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

#include "bb_irq.h"
#include "debug_host.h"
#include "timer.h"
#include "global_macros.h"
#if ((!defined CHIP_HAS_AP) && (!defined CHIP_DIE_8955))
#include "debug_port.h"
#endif
#include "spal_debug.h"
#include "spal_map.h"
#include "spal_irq.h"


#define SPAL_DBG_HST_SEND_EVENT_DELAY 2


extern SPAL_PROFILE_CONTROL_T g_spalProfileControl;


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// =============================================================================
// spal_DbgPxtsSendCode
// -----------------------------------------------------------------------------
/// Profiling tool (reserved for Platform profiling). To use this feature,
/// the PXTS device must have been enabled on the debug bus (spal_DbgPortSetup)
/// This function is reserved for platform profiling. It is used to send
/// a serie of event to a profiling tool (like CSBJ SA Tool) through the
/// romulator. \n
/// When this function is called, it sends an event represented by its \c code
/// to the PXTS Fifo. This Fifo is read by en external tool. \n
/// \n
/// (Nota: SA tool and Coolprofile cannot be used at the same time)
///
/// @param code The event code to send to the profiling tool
/// @param level PXTS level on which to send the code.
// =============================================================================
INLINE VOID spal_DbgPxtsSendCode(SPAL_DBG_PXTS_LEVEL_T level,
                                 UINT16 code)
{
    UINT32 config = 0;
    if (level == SPAL_DBG_PXTS_SPAL)
    {
        config = SPAL_PROFILE_CONTROL_SPAL;
    }
    else if (level == SPAL_DBG_PXTS_SPP)
    {
        config = SPAL_PROFILE_CONTROL_SPP;
    }
    else if (level == SPAL_DBG_PXTS_SPC)
    {
        config = SPAL_PROFILE_CONTROL_SPC;
    }
    else if (level == SPAL_DBG_PXTS_BB_IRQ)
    {
        config = SPAL_PROFILE_CONTROL_BB_IRQ;
    }
    else if (level == SPAL_DBG_PXTS_BB_RX_WIN)
    {
        config = SPAL_PROFILE_CONTROL_BB_RX_WIN;
    }
    else if (level == SPAL_DBG_PXTS_BB_IFC2_SYM_CNT)
    {
        config = SPAL_PROFILE_CONTROL_BB_IFC2_SYM_CNT;
    }

    if ((config&g_spalProfileControl.config) != 0)
    {
        UINT32 critical_sec = spal_IrqEnterCriticalSection();

        UINT32 time = hwp_timer->HWTimer_CurVal;
        if(g_spalProfileControl.buffer_pos >= g_spalProfileControl.buffer_size)
        {
            g_spalProfileControl.buffer_pos = 0;
        }

        g_spalProfileControl.buffer_start[g_spalProfileControl.buffer_pos++] = ((time & 0xffff) << 16) | code;

        spal_IrqExitCriticalSection(critical_sec);
    }
}


// =============================================================================
// spal_DbgPxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{
    spal_DbgPxtsSendCode(level, (functionId & CPMASK) | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{
    spal_DbgPxtsSendCode(level, (functionId & CPMASK) | CPEXITFLAG | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window.
///
/// @param windowId Code representing the window which has been entered in.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{
    spal_DbgPxtsSendCode(level, (windowId & CPMASK) | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window.
///
/// @param windowId Code representing the window which has been exited.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{
    spal_DbgPxtsSendCode(level, (windowId & CPMASK) | CPEXITFLAG | CPBCPU);
}



// =============================================================================
// spal_DbgPxtsProfileTaskEnter
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code corresponding to scheduled task Id.
///
/// @param taskId Task id of the scheduled task.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfileTask(SPAL_DBG_PXTS_LEVEL_T level,
                                    UINT16 taskId)
{
    spal_DbgPxtsSendCode(level, CPTASKSPACESTART | ((taskId) & CPTASKSPACEMASK) | CPBCPU);
}


// =============================================================================
// spal_DbgPxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse.
///
/// @param code Code representing the pulse
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_LEVEL_T level,
                                     UINT16 code)
{
    spal_DbgPxtsSendCode(level, (code & CPMASK) | CPBCPU);
}


#if ((!defined CHIP_HAS_AP) && (!defined CHIP_DIE_8955))
//======================================================================
// spal_DbgPxtsDump
//----------------------------------------------------------------------
// Send data buffer to host
//======================================================================
PUBLIC VOID spal_DbgPxtsDump(UINT16* StartAdrr, UINT16 length, UINT16 Marker)
{
    UINT16* data=StartAdrr;
    UINT16 i;

    // write the Marker twice
#ifndef CHIP_HAS_PXTS_LEVELS // Granite test chip
    hwp_debugPort->Pxts_tag_cfg = Marker;
    hwp_debugPort->Pxts_tag_cfg = Marker;
#else
    hwp_debugPort->Pxts_tag[SPAL_DBG_PXTS_DUMP] = Marker;
    hwp_debugPort->Pxts_tag[SPAL_DBG_PXTS_DUMP] = Marker;
#endif

    for (i=0; i<length; i++)
    {
#ifndef CHIP_HAS_PXTS_LEVELS // Granite test chip
        hwp_debugPort->Pxts_tag_cfg = *data;
#else
        hwp_debugPort->Pxts_tag[SPAL_DBG_PXTS_DUMP] = *data;
#endif
        data++;
    }

    // write the Marker twice
#ifndef CHIP_HAS_PXTS_LEVELS // Granite test chip
    hwp_debugPort->Pxts_tag_cfg = Marker;
    hwp_debugPort->Pxts_tag_cfg = Marker;
#else
    hwp_debugPort->Pxts_tag[SPAL_DBG_PXTS_DUMP] = Marker;
    hwp_debugPort->Pxts_tag[SPAL_DBG_PXTS_DUMP] = Marker;
#endif

}
#endif


//======================================================================
// spal_DbgHstSendEvent
//----------------------------------------------------------------------
// Send event to host
//======================================================================
PUBLIC BOOL spal_DbgHstSendEvent(UINT32 ch)
{
    UINT32 status       =   hwp_bbIrq->SC;
    UINT32 startTime    =   hwp_timer->HWTimer_CurVal;
    BOOL   timeOut      =   FALSE;

    //  wait for host sema to be free or timeout
    while(hwp_debugHost->event == 0 && !timeOut)
    {
        if ((hwp_timer->HWTimer_CurVal - startTime) >= SPAL_DBG_HST_SEND_EVENT_DELAY)
        {
            timeOut = TRUE;
        }
    }

    //  Write the byte in the host event register if possible
    if (!timeOut)
    {
        hwp_debugHost->event = ch;
    }

    hwp_bbIrq->SC = status;
    return (!timeOut);
}


