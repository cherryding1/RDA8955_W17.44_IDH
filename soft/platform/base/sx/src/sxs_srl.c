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




#include "sxs_type.h"
#include "sxr_ops.h"
#include "sxs_rmt.h"
#include "sxr_csp.h"
#include "sxs_srl.h"
#define __SXS_SRL_VAR__
#include "sxs_srl.hp"
#undef __SXS_SRL_VAR__
#include "sxs_io.h"

#if (SXS_NB_SERIAL_PORTS > 0)

#ifdef __MULTI_RAT__
#include "hal_ap_mailbox.h"
#include "hal_comregs.h"

EXPORT HAL_HST_TRACE_PORT_T g_HalTracePort;
EXPORT HAL_AP_MBX_TRACE_T* HAL_DATA_INTERNAL g_halApMbxTrace;
#endif

#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef EARLY_TRACE_OPEN
#endif


PRIVATE BOOL g_sxsFlowCtrlLocked = FALSE;

PRIVATE UINT8 g_sxsCurFlowCtrl;


#ifdef EARLY_TRACE_OPEN
#include "hal_timers.h"

extern void sxr_TkSdInit();

u16 sxs_ProcessRmtDataDummy(u8 *DataIn, u16 Size, u8 RmtCpuIdx)
{
    return Size;
}

// Note: The host trace will be initialized once again when OS is started
void sxs_HostTraceEarlyOpen (void)
{
    // To avoid problems when running sxs_CheckStkTk() during a crash
    sxr_TkSdInit();

    u16 SetUart = SXS_SET_NB_DATA (SXS_8_DATA)
                  | SXS_SET_NB_STOP (SXS_1_STOP)
                  | SXS_SET_PARITY (SXS_PARITY_DISABLE)
                  | SXS_SET_RATE (SXS_R460800);

    u32 Status = sxr_EnterSc ();

    sxs_SerialDesc [PAL_DBG_PORT_IDX].Status = SXS_SERIAL_ON;

    sxs_SerialDesc [PAL_DBG_PORT_IDX].WIdxRx = 0;
    sxs_SerialDesc [PAL_DBG_PORT_IDX].RIdxRx = 0;
    sxs_SerialDesc [PAL_DBG_PORT_IDX].WIdxTx = 0;
    sxs_SerialDesc [PAL_DBG_PORT_IDX].RIdxTx = 0;

    pal_OpenSerial (PAL_DBG_PORT_IDX);
    sxs_SetSerialCfg (SetUart, PAL_DBG_PORT_IDX);

    sxs_SetSerialRoute (PAL_DBG_PORT_IDX, sxs_ReadTxRmtData, sxs_ProcessRmtDataDummy, sxs_GetRmtBufferStatus, SXS_RMT_DBG_CPU_IDX);

    sxr_ExitSc (Status);

#ifdef __TARGET__
    sxs_GetRmtBuffer ();
#endif
    sxs_RmtInit (FALSE);

    // Init trace bitmap
    sxs_IoCtx.TraceBitMap [TGET_ID(_HAL)] = 0xFFFF;
    sxs_IoCtx.TraceBitMap [TGET_ID(_PAL)] = 0xFFFF;
}

void sxs_HostTraceEarlyOpenDone (void)
{
    // Wait for 1000 MS at most
    const u32 MAX_EARLY_TRACE_OUTPUT_TIME = (1000 MS_WAITING);

    UINT32 startTime = hal_TimGetUpTime();
    while (sxs_SerialFlush() &&
            (hal_TimGetUpTime() - startTime) < MAX_EARLY_TRACE_OUTPUT_TIME);
    while (sxs_Flush(PAL_DBG_PORT_IDX) &&
            (hal_TimGetUpTime() - startTime) < MAX_EARLY_TRACE_OUTPUT_TIME);
    pal_SleepSerial(/*PortIdx*/);
    pal_CloseSerial(PAL_DBG_PORT_IDX);
}
#endif // EARLY_TRACE_OPEN

// =============================================================================
// sxs_SetSerialRoute
// -----------------------------------------------------------------------------
/// Configure the user Tx, Status buffer and Rx management functions.
/// @param PortIdx      Port index.
/// @param ReadTxData   User Tx buffer function (manage the buffer that contains
///                     user's Tx data)
/// @param WriteRxData  User Rx function (manage the data received from remote
///                     computer.)
/// @param GetBufStatus User Tx buffer status function (return the amount of
///                     data bytes to be transmitted in the Tx buffer).
/// @param RmtCpuIdx    Remote CPU index.
// =============================================================================
void sxs_SetSerialRoute (u8  PortIdx,
                         u16 (*ReadTxData) (u8 *BufOut, u16 Size, u8 RmtCpuIdx),
                         u16 (*WriteRxData) (u8 *BufIn, u16 Size, u8 RmtCpuIdx),
                         u16 (*GetBufStatus) (u8 RmtCpuIdx),
                         u8  RmtCpuIdx)
{
    sxs_SerialDesc [PortIdx].ReadTxData   = ReadTxData;
    sxs_SerialDesc [PortIdx].WriteRxData  = WriteRxData;
    sxs_SerialDesc [PortIdx].GetBufStatus = GetBufStatus;
    sxs_SerialDesc [PortIdx].RmtCpuIdx    = RmtCpuIdx;
}


// =============================================================================
// sxs_GetSerialStatus
// -----------------------------------------------------------------------------
/// Return the status of the specified uart.
/// @param PortIdx  Port index.
/// @return         Current Status.
// =============================================================================
u8 sxs_GetSerialStatus (u8 PortIdx)
{
    return sxs_SerialDesc [PortIdx].Status;
}

// =============================================================================
// sxs_SetSerialStatus
// -----------------------------------------------------------------------------
/// Set the status of the specified uart.
/// @param Status
/// @param PortIdx  Port index.
/// @return         null.
// =============================================================================
void sxs_SetSerialStatus (u8 Status, u8 PortIdx)
{
    sxs_SerialDesc [PortIdx].Status = Status;
}

// =============================================================================
// sxs_GetSerialCfg
// -----------------------------------------------------------------------------
/// Return the configuration of the specified uart.
/// @param PortIdx  Port index.
/// @return         Current setting.
// =============================================================================
u16 sxs_GetSerialCfg (u8 PortIdx)
{
    return sxs_SerialDesc [PortIdx].Setting;
}


// =============================================================================
// sxs_SetSerialCfg
// -----------------------------------------------------------------------------
/// Set serial configuration.
/// @param Setting
/// @param PortIdx serial port index.
// =============================================================================
void sxs_SetSerialCfg (u16 Setting, u8 PortIdx)
{
    pal_SetSerialCfg (Setting, PortIdx);
    sxs_SerialDesc [PortIdx].Setting = Setting;
}


// =============================================================================
// sxs_InitSerial
// -----------------------------------------------------------------------------
/// Serial port initialzation.
/// @param Setting  Settings.
/// @param PortIdx  Port index.
/// @param Route    Remote CPU index for serial route.
// =============================================================================
void sxs_InitSerial (u16 Setting, u8 PortIdx, u8 Route)
{
    u32 Status = sxr_EnterSc ();

#ifndef __SXR_SRL_F__
    sxs_SerialDesc [PortIdx].Status = PAL_TX_ENABLED(PortIdx) ?
                                      0 :
                                      SXS_TX_DISABLED;
#else
    sxs_SerialDesc [PortIdx].Status = 0;
#endif

    sxs_SerialDesc [PortIdx].Status |= SXS_SERIAL_ON;

    sxs_SerialDesc [PortIdx].WIdxRx = 0;
    sxs_SerialDesc [PortIdx].RIdxRx = 0;
    sxs_SerialDesc [PortIdx].WIdxTx = 0;
    sxs_SerialDesc [PortIdx].RIdxTx = 0;

    if (!Setting)
    {
        Setting = SXS_SET_NB_DATA (SXS_8_DATA)
                  | SXS_SET_NB_STOP (SXS_1_STOP)
                  | SXS_SET_PARITY (SXS_PARITY_DISABLE)
                  | SXS_SET_RATE (SXS_R115200);
    }

    pal_OpenSerial (PortIdx);
    sxs_SetSerialCfg (Setting, PortIdx);

    sxs_SetSerialRoute (PortIdx, sxs_ReadTxRmtData, sxs_WriteRxRmtData, sxs_GetRmtBufferStatus, Route);
    sxs_ResetTxRmtData ();
    sxs_ResetRxRmtData ();

    sxr_ExitSc (Status);
}

// =============================================================================
// sxs_CloseSerial
// -----------------------------------------------------------------------------
/// Serial port Close.
/// @param PortIdx  Port index.
// =============================================================================
void sxs_CloseSerial (u8 PortIdx)
{
    u32 Status = sxr_EnterSc ();
    sxs_SerialDesc [PortIdx].Status = 0;
    sxr_ExitSc (Status);
}

// =============================================================================
// sxs_SerialFlush
// -----------------------------------------------------------------------------
/// Move data from user Tx buffer to the serial Tx buffer,
///  fill the Tx FIFO if necessary.
///  Release data received to the user.
///  Check for flow control release.
///  Must be called from Idle task.
/// @return         TRUE when at least one user buffer is not empty
///                 for an inactive port. (must not go to sleep).
// =============================================================================
u8 FunctionLocked = FALSE;
u8 sxs_SerialFlush (void)
{
    //static u8 FunctionLocked = FALSE;
    u16 BufferLoad = 0;
    u16 DistToBufferEnd;
    u8  PortIdx, Size;
    u8 ModemStatus;
#ifndef __SXR_SRL_F__
    u8  PortActive = FALSE;
#endif

    if (FunctionLocked)
    {
        return FALSE;
    }

    FunctionLocked = TRUE;

    for (PortIdx=0; PortIdx<SXS_NB_SERIAL_PORTS; PortIdx++)
    {
#ifdef _TTCN_TEST_
        if(1)
        {
            if(1)
            {
#else
        if (sxs_SerialDesc [PortIdx].Status & SXS_SERIAL_ON
#if defined (__MULTI_RAT__) && defined (CHIP_HAS_AP)
                && !(g_HalTracePort == HAL_HST_TRACE_PORT_AP)
#endif
           )
        {
            if (!(sxs_SerialDesc [PortIdx].Status & SXS_TX_DISABLED))
            {
#endif
                do
                {
                    Size = (sxs_SerialDesc [PortIdx].RIdxTx - sxs_SerialDesc [PortIdx].WIdxTx - 1) & 0xFF;
                    DistToBufferEnd = SXS_TX_BUFFER_SIZE - sxs_SerialDesc [PortIdx].WIdxTx;

                    if (Size > 0)
                    {
                        if (Size > DistToBufferEnd)
                        {
                            Size = (u8)DistToBufferEnd;
                        }

                        Size = (u8)sxs_SerialDesc [PortIdx].ReadTxData (
                                   &sxs_SerialDesc [PortIdx].TxBuffer [sxs_SerialDesc [PortIdx].WIdxTx],
                                   Size,
                                   sxs_SerialDesc [PortIdx].RmtCpuIdx);

                        sxs_SerialDesc [PortIdx].WIdxTx = sxs_SerialDesc [PortIdx].WIdxTx + Size;
                    }
                }
                while (Size);

#ifndef __SXR_SRL_F__
                if (PAL_IS_TX_EMPTY(PortIdx))
                {
                    PortActive = sxs_Flush (PortIdx);
                }
#else
                sxs_Flush (PortIdx);
#endif
            }

#ifdef __SXR_SRL_F__
#ifdef _TTCN_TEST_
            if(1)
#else
            if (!(sxs_SerialDesc [PortIdx].Status & SXS_RX_DISABLED))
#endif
            {
                u8 TimeOut = 20;
                // Reset Plus search.
                sxs_SerialDesc [PortIdx].PlusCnt = 0;

                do
                {
                    Size = (sxs_SerialDesc [PortIdx].RIdxRx - sxs_SerialDesc [PortIdx].WIdxRx - 1) & 0xFF;
                    DistToBufferEnd = SXS_RX_BUFFER_SIZE - sxs_SerialDesc [PortIdx].WIdxRx;

                    if (Size > 0)
                    {
                        if (Size > DistToBufferEnd)
                        {
                            Size = (u8)DistToBufferEnd;
                        }

                        Size = (u8)pal_Read (PortIdx, &sxs_SerialDesc [PortIdx].RxBuffer [sxs_SerialDesc [PortIdx].WIdxRx], Size);

                        // Look for 3 isolated plus in a row.
                        if (Size <= (3-sxs_SerialDesc [PortIdx].PlusCnt))
                        {
                            u32 k;

                            for (k=0; k<Size; k++)
                            {
                                if (sxs_SerialDesc [PortIdx].RxBuffer [sxs_SerialDesc [PortIdx].WIdxRx + k] == '+')
                                {
                                    // Notify the reception of 3 plus in a row.
                                    if ((++sxs_SerialDesc [PortIdx].PlusCnt == 3)
                                            &&  (sxs_SerialDesc [PortIdx].StatusNotification != NIL))
                                    {
                                        sxs_SerialDesc [PortIdx].StatusNotification (PortIdx, SXS_RX_3PLUS, sxs_SerialDesc [PortIdx].LastModemStatus);
                                    }
                                }
                                else
                                {
                                    // Stop searching: the 3 + in a row must be isolated from any data.
                                    sxs_SerialDesc [PortIdx].PlusCnt = 0;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            sxs_SerialDesc [PortIdx].PlusCnt = 0;
                        }

                        // Manage software echo.
                        if ((sxs_SerialDesc [PortIdx].Status & SXS_ECHO_SET)
                                &&  (Size))
                        {
                            u16 WrSize = 0;

                            while (Size != WrSize)
                            {
                                WrSize += (u8)pal_Write (PortIdx, &sxs_SerialDesc [PortIdx].RxBuffer [sxs_SerialDesc [PortIdx].WIdxRx + WrSize], Size - WrSize);
                            }
                        }

                        sxs_SerialDesc [PortIdx].WIdxRx += Size;
                    }
                }
                while ((Size) || ((sxs_SerialDesc [PortIdx].PlusCnt != 0) && (TimeOut--)));
            }
#endif

            do
            {
                Size = (sxs_SerialDesc [PortIdx].WIdxRx - sxs_SerialDesc [PortIdx].RIdxRx) & 0xFF;
                DistToBufferEnd = (SXS_RX_BUFFER_SIZE - sxs_SerialDesc [PortIdx].RIdxRx);

                if (Size > 0)
                {
                    if (Size > DistToBufferEnd)
                    {
                        Size = (u8)DistToBufferEnd;
                    }

                    Size = (u8)sxs_SerialDesc [PortIdx].WriteRxData (
                               &sxs_SerialDesc [PortIdx].RxBuffer [sxs_SerialDesc [PortIdx].RIdxRx],
                               Size,
                               sxs_SerialDesc [PortIdx].RmtCpuIdx);

                    sxs_SerialDesc [PortIdx].RIdxRx = sxs_SerialDesc [PortIdx].RIdxRx + Size;
                }
            }
            while (Size);

#ifndef __SXR_SRL_F__
#ifdef _TTCN_TEST_
            if(1)
#else
            if (sxs_SerialDesc [PortIdx].Status & SXS_RX_DISABLED)
#endif
            {
                u32 Status = sxr_EnterSc ();

                if (((sxs_SerialDesc [PortIdx].RIdxRx -
                        sxs_SerialDesc [PortIdx].WIdxRx - 1) & 0xFF) >= PAL_RX_FIFO_SIZE(PortIdx))
                {
                    PAL_ENABLE_RX(PortIdx);
                    sxs_SerialDesc [PortIdx].Status &= ~SXS_RX_DISABLED;
                }

                sxr_ExitSc (Status);
            }
#endif
        }

        ModemStatus = pal_GetModemStatus (PortIdx) & sxs_SerialDesc [PortIdx].NotificationMsk;
        if (sxs_SerialDesc [PortIdx].ModemStatus != ModemStatus)
        {
            sxs_SerialDesc [PortIdx].LastModemStatus = sxs_SerialDesc [PortIdx].ModemStatus;
            sxs_SerialDesc [PortIdx].ModemStatus = ModemStatus;

            if (sxs_SerialDesc [PortIdx].StatusNotification != NIL)
            {
                sxs_SerialDesc [PortIdx].StatusNotification (PortIdx, ModemStatus, sxs_SerialDesc [PortIdx].LastModemStatus);
            }
        }

        // The above code can call functions that will add new traces.
        // Especially sxs_SerialDesc [PortIdx].WriteRxData() will call
        // sxs_Rmt [RmtCpuIdx].UserPrcsRmtData() or
        // sxs_PrcsRmtDataArray [RmtCpuIdx]().
        // Status to allow sleep must take that into account.

        if ((!(sxs_SerialDesc [PortIdx].Status & SXS_TX_DISABLED))
#ifndef __SXR_SRL_F__
                && !PortActive     // only when port is inactive
#endif
#if defined (__MULTI_RAT__) && defined (CHIP_HAS_AP)
                && !(g_HalTracePort == HAL_HST_TRACE_PORT_AP)
#endif
                && (sxs_SerialDesc [PortIdx].GetBufStatus) )
        {
            BufferLoad += sxs_SerialDesc [PortIdx].GetBufStatus (sxs_SerialDesc [PortIdx].RmtCpuIdx);
        }

        BufferLoad += sxs_GetTxBufAvailability(PortIdx);

    }



    FunctionLocked = FALSE;

    return (BufferLoad > 0);
}


// =============================================================================
// sxs_Flush
// -----------------------------------------------------------------------------
/// Fill the FIFO with Tx buffer data when any.
/// @param PortIdx  Port index.
/// @return         Active status of the port.
// =============================================================================
u8 sxs_Flush (u8 PortIdx)
{
    u8  TraceEnabled = FALSE;
    u32 Status = sxr_EnterSc ();
#ifndef __SXR_SRL_F__
    // Size of data bytes available for transmission.
    u8  Size   = sxs_SerialDesc [PortIdx].WIdxTx - sxs_SerialDesc [PortIdx].RIdxTx;

    if ((sxs_SerialDesc [PortIdx].Status & SXS_TX_DISABLED)
            &&  (PAL_TX_ENABLED(PortIdx)))
    {
        sxs_SerialDesc [PortIdx].Status &= ~SXS_TX_DISABLED;
    }

    if ((Size != 0)
            &&  (!(sxs_SerialDesc [PortIdx].Status & SXS_TX_DISABLED)))
    {
        u8 i;

        if (Size > PAL_TX_FIFO_SIZE (PortIdx))
        {
            Size = PAL_TX_FIFO_SIZE (PortIdx);
        }

        for (i=0; i<Size; i++)
        {
            pal_WriteData (sxs_SerialDesc [PortIdx].TxBuffer [sxs_SerialDesc [PortIdx].RIdxTx++], PortIdx);
        }
    }
#else
    u8  Size;
    u16 DistToBufferEnd;

//    do
    {
        Size = sxs_SerialDesc [PortIdx].WIdxTx - sxs_SerialDesc [PortIdx].RIdxTx;
        DistToBufferEnd = SXS_TX_BUFFER_SIZE - sxs_SerialDesc [PortIdx].RIdxTx;

        if (Size > DistToBufferEnd)
        {
            Size = (u8)DistToBufferEnd;
        }

        if (Size > 0)
        {
            // Get channel if needed
            TraceEnabled = pal_WakeUpSerial(/*PortIdx*/);

            Size = (u8)pal_Write (PortIdx, &sxs_SerialDesc [PortIdx].TxBuffer [sxs_SerialDesc [PortIdx].RIdxTx], Size);
        }
        else
        {
#ifndef _TTCN_TEST_
            pal_SleepSerial(/*PortIdx*/);
#endif
            TraceEnabled = FALSE;
        }

        sxs_SerialDesc [PortIdx].RIdxTx += Size;
    } //while (Size);

#endif
    sxr_ExitSc (Status);
    return TraceEnabled;
}

#ifdef __MULTI_RAT__
u8 sxs_ApFlush (void)
{
    u16 Size, DistToBufferEnd, BufferLoad = 0;
    u8   SendIrq = 0;
    u8 RmtCpuIdx = 0;
    HAL_COMREGS_IRQ_MASK_T mask;

    //u32 Status = sxr_EnterSc ();
    do
    {
        Size = (g_halApMbxTrace->apRIdx - g_halApMbxTrace->modemWIdx -1)&AP_MBX_TRACE_M2A_IDX_MASK;
        DistToBufferEnd = AP_MBX_TRACE_M2A_SIZE - g_halApMbxTrace->modemWIdx;

        if (Size > 0)
        {
            if (Size > DistToBufferEnd)
            {
                Size = DistToBufferEnd;
            }

            Size =  sxs_ReadTxRmtData(&g_halApMbxTrace->m2aBuf[g_halApMbxTrace->modemWIdx], Size, RmtCpuIdx);

            // Update the write index
            g_halApMbxTrace->modemWIdx = (g_halApMbxTrace->modemWIdx + Size) & AP_MBX_TRACE_M2A_IDX_MASK;
        }

        if(Size  > 0)
        {
            // data has sent to AP, send Irq
            SendIrq = 1;
        }
    }
    while (Size);

    // Tell AP some trace has been sent
    if(SendIrq > 0)
    {
        mask.mask = (1<<4); //AP_COMM_COM1_TRACE_ARRIVED
        hal_ApComregsApIrqTriggerLight(HAL_APCOMREGS_AP_IRQ_SLOT_1, mask);
    }
    //sxr_ExitSc (Status);

    BufferLoad = 1; //sxs_GetRmtBufferStatus(RmtCpuIdx);
    return (BufferLoad >0);
}
#endif

/*
===============================================================================
   Function   : sxs_PutRxData
 -----------------------------------------------------------------------------
   Scope      : A maximum of Size bytes are copied from the location pointed
                by the pointer Data.
   Parameters : Pointer where to get the data,
                Number of bytes available,
                Serial port number.
   Return     : The function returns the real number of bytes retrieved by SX.
===============================================================================
*/
u8 sxs_PutRxData (u8 *Data, u8 Size, u8 PortIdx)
{
    u32 i;

    for (i=0; i<Size; i++)
    {
        sxs_SerialDesc [PortIdx].RxBuffer [sxs_SerialDesc [PortIdx].WIdxRx++] = Data [i];
    }

    return i;
}

/*
===============================================================================
   Function   : sxs_GetTxBufAvailability
 -----------------------------------------------------------------------------
   Scope      : Check the number of available bytes in the Tx Buffer associated
                the identifed serial port.
   Parameters : Serial port number.
   Return     : Number of avalaible bytes.
===============================================================================
*/
u16 sxs_GetTxBufAvailability (u8 PortIdx)
{
    return ((sxs_SerialDesc [PortIdx].WIdxTx - sxs_SerialDesc [PortIdx].RIdxTx) & 0xFF);
}


/*
===============================================================================
   Function   : sxs_GetRxBufAvailability
 -----------------------------------------------------------------------------
   Scope      : Check the number of available bytes in the Rx Buffer associated
                the identifed serial port.
   Parameters : Serial port number.
   Return     : Number of avalaible bytes.
===============================================================================
*/
u16 sxs_GetRxBufAvailability (u8 PortIdx)
{
    return ((sxs_SerialDesc [PortIdx].RIdxRx - sxs_SerialDesc [PortIdx].WIdxRx - 1) & 0xFF);
}


// =============================================================================
// sxs_SetEcho
// -----------------------------------------------------------------------------
/// Configure the echo.
/// @param Setting  #SXS_ECHO_ON / #SXS_ECHO_OFF
/// @param PortIdx  Port index.
// =============================================================================
void sxs_SetEcho (u8 Setting, u8 PortIdx)
{
    u32 i;
    if (Setting == SXS_ECHO_ON)
    {
        if (!pal_SetEcho (PAL_ECHO_ON, PortIdx))
        {
            sxs_SerialDesc [PortIdx].Status |= SXS_ECHO_SET;
        }
    }
    else
    {
        i = pal_SetEcho (PAL_ECHO_OFF, PortIdx);
        sxs_SerialDesc [PortIdx].Status &= ~SXS_ECHO_SET;
    }
}

// =============================================================================
// sxs_IsSerialOpened
// -----------------------------------------------------------------------------
/// Report the open status of the serial port.
/// @param PortIdx  Port index.
/// @return         \c TRUE if opened.
// =============================================================================
u8 sxs_IsSerialOpened (u8 PortIdx)
{
    return (sxs_SerialDesc [PortIdx].Status & SXS_SERIAL_ON) != 0;
}

// =============================================================================
// sxs_SetModemStatusNotification
// -----------------------------------------------------------------------------
/// Set the modem satus notification function. Such a function is called each
/// time a change is detected on a signal set in the mask.
/// @param PortIdx  Port index.
/// @param Mask     a combination of #SXS_DSR_ON, #SXS_CTS_ON and #SXS_RING_ON.
/// @param StatusNotification   Pointer on the notification function.
// =============================================================================
void sxs_SetModemStatusNotification (u8 PortIdx, u8 Mask, void (*StatusNotification)(u8 PortIdx, u8 Status, u8 LastStatus))
{
    sxs_SerialDesc [PortIdx].NotificationMsk    = Mask;
    sxs_SerialDesc [PortIdx].StatusNotification = StatusNotification;
}

// =============================================================================
// sxs_LockFlowControl
// -----------------------------------------------------------------------------
/// Lock the flow control to specified mode
/// @param PortIdx  Port index.
/// @param Status   #SXS_HOLD_RX / #SXS_RELEASE_RX
// =============================================================================
void sxs_LockFlowControl (u8 PortIdx, u8 Status)
{
    g_sxsCurFlowCtrl = sxs_GetSerialStatus(PortIdx);
    sxs_SetFlowControl(PortIdx, Status);
    g_sxsFlowCtrlLocked = TRUE;
}

// =============================================================================
// sxs_UnlockFlowControl
// -----------------------------------------------------------------------------
/// Unlock and restore the flow control
/// @param PortIdx  Port index.
// =============================================================================
void sxs_UnlockFlowControl (u8 PortIdx)
{
    g_sxsFlowCtrlLocked = FALSE;
    sxs_SetFlowControl(PortIdx, g_sxsCurFlowCtrl);
}

// =============================================================================
// sxs_SetFlowControl
// -----------------------------------------------------------------------------
/// Configure the flow control
/// @param PortIdx  Port index.
/// @param Status   #SXS_HOLD_RX / #SXS_RELEASE_RX
// =============================================================================
void sxs_SetFlowControl (u8 PortIdx, u8 Status)
{
    u8 stat;

    if (g_sxsFlowCtrlLocked)
    {
        stat = g_sxsCurFlowCtrl;
    }
    else
    {
        stat = sxs_GetSerialStatus(PortIdx);
    }

    if (Status & SXS_HOLD_RX)
    {
        stat |= SXS_RX_DISABLED;
    }
    else if (Status & SXS_RELEASE_RX)
    {
        stat &= ~SXS_RX_DISABLED;
    }

    if (Status & SXS_HOLD_TX)
    {
        stat |= SXS_TX_DISABLED;
    }
    else if (Status & SXS_RELEASE_TX)
    {
        stat &= ~SXS_TX_DISABLED;
    }

    if (g_sxsFlowCtrlLocked)
    {
        g_sxsCurFlowCtrl = stat;
    }
    else
    {
        sxs_SetSerialStatus(stat, PortIdx);
    }
}

// =============================================================================
// sxs_GetFlowControlStatus
// -----------------------------------------------------------------------------
/// Get current flow control status
/// @param PortIdx  Port index.
/// @return Flow control status
// =============================================================================
u8 sxs_GetFlowControlStatus (u8 PortIdx)
{
    u8 status = 0;
    u8 stat = sxs_GetSerialStatus(PortIdx);

    if (stat & SXS_RX_DISABLED)
    {
        status |= SXS_HOLD_RX;
    }
    else
    {
        status |= SXS_RELEASE_RX;
    }

    if (stat & SXS_TX_DISABLED)
    {
        status |= SXS_HOLD_TX;
    }
    else
    {
        status |= SXS_RELEASE_TX;
    }

    return status;
}

// =============================================================================
// sxs_SetModemStatus
// -----------------------------------------------------------------------------
///
/// @param Status   a combination of #SXS_DSR_ON / #SXS_DSR_OFF,
//                  #SXS_CTS_ON / #SXS_CTS_OFF, #SXS_RING_ON / #SXS_RING_OFF and
//                  #SXS_DCD_ON / #SXS_DCD_OFF.
/// @param PortIdx  Port index.
// =============================================================================
void sxs_SetModemStatus (u8 Status, u8 PortIdx)
{
    if (Status & SXS_DSR_ON)
    {
        pal_SetDsr (1, PortIdx);
    }
    else if (Status & SXS_DSR_OFF)
    {
        pal_SetDsr (0, PortIdx);
    }

    if (Status & SXS_CTS_ON)
    {
        pal_SetCts (1, PortIdx);
    }
    else if (Status & SXS_CTS_OFF)
    {
        pal_SetCts (0, PortIdx);
    }

    if (Status & SXS_RING_ON)
    {
        pal_SetRing (1, PortIdx);
    }
    else if (Status & SXS_RING_OFF)
    {
        pal_SetRing (0, PortIdx);
    }

    if (Status & SXS_DCD_ON)
    {
        pal_SetDcd (1, PortIdx);
    }
    else if (Status & SXS_DCD_OFF)
    {
        pal_SetDcd (0, PortIdx);
    }
}


// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================

#endif // SXS_NB_SERIAL_PORTS