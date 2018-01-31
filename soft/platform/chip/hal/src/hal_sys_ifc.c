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
#include "sys_ifc.h"
#include "mem_bridge.h"

#include "halp_sys_ifc.h"
#include "halp_debug.h"

#define HAL_IFC_CHANNEL_DEBUG


#define IFC_ASSERT(BOOL,param)  HAL_ASSERT(BOOL, "Ifc param = %d", param)

#define IFC_DDR_HIGH_FREQ_NEEDED(requestId, memAddr) ((((requestId) & 1) == 0) && ADDRESS_IN_HWP((memAddr), hwp_cs1))

/// That structure is used to memorize the owner of
/// each of the IFC channel.
PRIVATE HAL_IFC_REQUEST_ID_T g_halModuleIfcChannelOwner[SYS_IFC_STD_CHAN_NB];


#ifdef HAL_IFC_CHANNEL_DEBUG

#define HAL_IFC_CHANNEL_DEBUG_LEN 16
typedef struct
{
    HAL_IFC_REQUEST_ID_T owner;
    UINT32 start_addr;
    UINT32 tc;
    HAL_IFC_MODE_T mode;
} HAL_IFC_CHANNEL_DEBUG_T;

HAL_IFC_CHANNEL_DEBUG_T g_halModuleIfcChannelDebug[HAL_IFC_CHANNEL_DEBUG_LEN];
UINT32 g_halModuleIfcChannelDebugPos;
#endif

// =============================================================================
// hal_IfcOpen
// -----------------------------------------------------------------------------
/// Open the System IFC. Initialize some state variables.
// =============================================================================
PROTECTED VOID hal_IfcOpen(VOID)
{
    UINT8 channel;

    // Initialize the channel table with unknown requests. This is more
    // of a sanity initialization.
    for (channel = 0; channel < SYS_IFC_STD_CHAN_NB; channel++)
    {
        g_halModuleIfcChannelOwner[channel] = HAL_IFC_NO_REQWEST;
    }
}

// =============================================================================
// hal_IfcGetOwner
// -----------------------------------------------------------------------------
/// Get the owner ID of an IFC channel (i.e. the module using the channel).
/// @param channel number to check
/// @return The owner number associated with the channel
// =============================================================================
PROTECTED HAL_IFC_REQUEST_ID_T hal_IfcGetOwner(UINT8 channel)
{
    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return HAL_IFC_NO_REQWEST;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

    return g_halModuleIfcChannelOwner[channel];
}


// =============================================================================
// hal_IfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of a channel owned by a request.
///
/// The channel is only released if the specified request
/// owns the channel.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel to release
// =============================================================================
PROTECTED VOID hal_IfcChannelRelease(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel)
{
    UINT32 status;

    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

    status = hal_SysEnterCriticalSection();
    if (g_halModuleIfcChannelOwner[channel] == requestId)
    {
        // Disable this channel
        // CAUTION: It needs several cycles to disable IFC, but the FLUSH bit
        // will take effect immediately. If the FLUSH bit is cleared while
        // the DISABLE bit is set, IFC will still work for the next few cycles,
        // which might lead to data loss!
        hwp_sysIfc->std_ch[channel].control |= SYS_IFC_DISABLE;
#if 0
        // Read the status of this channel
        while((hwp_sysIfc->std_ch[channel].status & SYS_IFC_ENABLE) != 0);
        // Write the TC to 0 for next time the channel is re-enabled
        hwp_sysIfc->std_ch[channel].tc =  0;
#endif
    }
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_IfcChannelFlush
// -----------------------------------------------------------------------------
/// Empty the FIFO a specified channel.
///
/// If #requestId does not match #channel owner, does not do anything.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be clear.
// =============================================================================
PROTECTED VOID hal_IfcChannelFlush(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel)
{
    UINT32 status;

    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

    // Check that the channel is really owned by the peripheral
    // which is doing the request, it could have been release
    // automatically or by an IRQ handler.
    status = hal_SysEnterCriticalSection();
    if (g_halModuleIfcChannelOwner[channel] == requestId)
    {
        // Always flush IFC regardless of whether FIFO is currently empty.
        // In case of RX, if some data is being received, it has
        // NOT been put in FIFO yet.
        hwp_sysIfc->std_ch[channel].control |= SYS_IFC_FLUSH;
    }
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_IfcChannelIsFifoEmpty
// -----------------------------------------------------------------------------
/// Returns \c TRUE when the FIFO is empty.
///
/// If #requestId does not match #channel owner, returns TRUE.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be checked.
// =============================================================================
PROTECTED BOOL hal_IfcChannelIsFifoEmpty(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel)
{
    UINT32 status;
    BOOL fifoIsEmpty = TRUE;

    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return fifoIsEmpty;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

    // Check that the channel is really owned by the peripheral
    // which is doing the request, it could have been release
    // automatically or by an IRQ handler.
    status = hal_SysEnterCriticalSection();
    if (g_halModuleIfcChannelOwner[channel] == requestId)
    {
        fifoIsEmpty =
            (0 != (hwp_sysIfc->std_ch[channel].status & SYS_IFC_FIFO_EMPTY));
    }
    hal_SysExitCriticalSection(status);

    return fifoIsEmpty;
}


// =============================================================================
// hal_IfcTransferStart
// -----------------------------------------------------------------------------
/// Start an IFC transfer
///
/// This is a non blocking function that starts the transfer
/// and returns the hand.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param memStartAddr. Start address of the buffer where data
/// to be sent are located or where to put the data read, according
/// to the request defined by the previous parameter
/// @param xferSize Number of bytes to transfer. The maximum size
/// is 2^20 - 1 bytes.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return Channel got or HAL_UNKNOWN_CHANNEL.
// =============================================================================
PROTECTED UINT8 hal_IfcTransferStart(HAL_IFC_REQUEST_ID_T requestId, UINT8* memStartAddr, UINT32 xferSize, HAL_IFC_MODE_T ifcMode)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_IfcTransferStart);
#if (CHIP_HAS_SYS_IFC_SIZE)
    // Check buffer alignment depending on the mode
    if (ifcMode == HAL_IFC_SIZE_32_MODE_MANUAL || ifcMode == HAL_IFC_SIZE_32_MODE_AUTO)
    {
        // check word alignment
        HAL_ASSERT(((UINT32)memStartAddr%4) == 0,
                   "HAL IFC: 32 bits transfer misaligned 0x@%08X", memStartAddr);
    }
#if (CHIP_HAS_SYS_IFC_SIZE == 2)
    else if (ifcMode == HAL_IFC_SIZE_16_MODE_MANUAL || ifcMode == HAL_IFC_SIZE_16_MODE_AUTO)
    {
        // check half word alignment
        HAL_ASSERT(((UINT32)memStartAddr%2) == 0,
                   "HAL IFC: 16 bits transfer misaligned 0x@%08X", memStartAddr);
    }
#endif
    else
    {
        // ifcMode == HAL_IFC_SIZE_8, nothing to check
    }
#endif

    HAL_ASSERT(xferSize < (1<<SYS_IFC_TC_LEN),
               "HAL IFC: Transfer size too large: %d", xferSize);

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    if(((UINT32)memStartAddr & 0x20000000) == 0)
    {
        hal_DcacheFlushAddrRange((UINT32)memStartAddr, (UINT32)memStartAddr+xferSize, TRUE);
    }
#endif

    UINT32 status = hal_SysEnterCriticalSection();
    UINT8 channel;
    UINT8 i;

    // Check the requested id is not currently already used.
    for (i = 0; i < SYS_IFC_STD_CHAN_NB ; i++)
    {
        if ((GET_BITFIELD(hwp_sysIfc->std_ch[i].control, SYS_IFC_REQ_SRC) == requestId) &&
            (hwp_sysIfc->std_ch[i].status & SYS_IFC_ENABLE) &&
            (hwp_sysIfc->std_ch[i].tc != 0))
        {
            // This channel is or was used for the requestId request.
            // Check it is still in use.
            // HAL_ASSERT((hwp_sysIfc->std_ch[i].status & SYS_IFC_ENABLE) == 0,
            //            "HAL: Attempt to use the IFC to deal with a %d"
            //            " request still active on channel %d", requestId, i);
            return HAL_UNKNOWN_CHANNEL;
        }
    }

    channel = SYS_IFC_CH_TO_USE(hwp_sysIfc->get_ch) ;

    if (channel >= SYS_IFC_STD_CHAN_NB)
    {
        hal_SysExitCriticalSection(status);
        HAL_PROFILE_FUNCTION_EXIT(hal_IfcTransferStart);
        HAL_PROFILE_FUNCTION_ENTER(hal_IfcTransferStart+1);
        HAL_PROFILE_FUNCTION_EXIT(hal_IfcTransferStart+1);
        return HAL_UNKNOWN_CHANNEL;
    }

#ifdef CHIP_XTAL_CLK_IS_52M
    if (IFC_DDR_HIGH_FREQ_NEEDED(requestId, memStartAddr))
    {
        hal_SysRequestDdrHighFreq(TRUE);
    }
#endif

    g_halModuleIfcChannelOwner[channel]     = requestId;
    hwp_sysIfc->std_ch[channel].start_addr  =  (UINT32) memStartAddr;
    hwp_sysIfc->std_ch[channel].tc          =  xferSize;
    hwp_sysIfc->std_ch[channel].control     = (SYS_IFC_REQ_SRC(requestId)
            | ifcMode
            | SYS_IFC_CH_RD_HW_EXCH
            | SYS_IFC_ENABLE);
    if(g_halModuleIfcChannelDebugPos >= HAL_IFC_CHANNEL_DEBUG_LEN)
    {
        g_halModuleIfcChannelDebugPos = 0;
    }
    g_halModuleIfcChannelDebug[g_halModuleIfcChannelDebugPos].owner = requestId;
    g_halModuleIfcChannelDebug[g_halModuleIfcChannelDebugPos].start_addr = (UINT32)memStartAddr;
    g_halModuleIfcChannelDebug[g_halModuleIfcChannelDebugPos].tc = xferSize;
    g_halModuleIfcChannelDebug[g_halModuleIfcChannelDebugPos].mode = ifcMode;
    g_halModuleIfcChannelDebugPos++;
    hal_SysExitCriticalSection(status);
    HAL_PROFILE_FUNCTION_EXIT(hal_IfcTransferStart);
    return channel;
}


// =============================================================================
// hal_IfcGetTc
// -----------------------------------------------------------------------------
/// Returns the transfer count of the IFC.
/// If #requestId does not match #channel owner, returns 0.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Number of the channel whose transfer we are concerned for.
/// @return The number of bytes remaining to be transfered.
// =============================================================================
PROTECTED UINT32 hal_IfcGetTc(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel)
{
    UINT32 status;
    UINT32 tc = 0;

    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return tc;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

    // Check that the channel is really owned by the peripheral
    // which is doing the request, it could have been release
    // automatically or by an IRQ handler.
    status = hal_SysEnterCriticalSection();
    if (g_halModuleIfcChannelOwner[channel] == requestId)
    {
        tc = hwp_sysIfc->std_ch[channel].tc;
    }
    hal_SysExitCriticalSection(status);

    return tc;
}


// =============================================================================
// hal_IfcExtendTransfer
// -----------------------------------------------------------------------------
/// Extend the transfer size of the channel of
/// the number ox bytes passed as a parameter.
///
/// Should be used only when a channel has been locked.
///
/// @param xferSize Number of bytes to add to the
/// transfer size of this channel
// =============================================================================
PROTECTED VOID hal_IfcExtendTransfer(UINT8 channel, UINT16 xferSize)
{
    // Here, we consider the transfer as previously finished.
    if (channel == HAL_UNKNOWN_CHANNEL) return;

    // Channel number too big.
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);

#ifdef CHIP_XTAL_CLK_IS_52M
    HAL_IFC_REQUEST_ID_T requestId = GET_BITFIELD(hwp_sysIfc->std_ch[channel].control, SYS_IFC_REQ_SRC);
    UINT32 memStartAddr = hwp_sysIfc->std_ch[channel].start_addr;
    if (IFC_DDR_HIGH_FREQ_NEEDED(requestId, memStartAddr))
    {
        hal_SysRequestDdrHighFreq(TRUE);
    }
#endif

    // Writing in the Tc register adds the number written
    // to the counter of bytes yet to transfer
    hwp_sysIfc->std_ch[channel].tc = xferSize;
}


// =============================================================================
// hal_IfcGetChannel
// -----------------------------------------------------------------------------
/// Get an IFC channel opend in NO auto-disable mode
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return The channel number got or HAL_UNKNOWN_CHANNEL
// =============================================================================
PROTECTED UINT8 hal_IfcGetChannel(HAL_IFC_REQUEST_ID_T requestId, HAL_IFC_MODE_T ifcMode)
{
    UINT8 channel;
    // This function is only relevant/available for manual transferts.
    HAL_ASSERT( ifcMode == HAL_IFC_SIZE_8_MODE_MANUAL
#if (CHIP_HAS_SYS_IFC_SIZE)
#if (CHIP_HAS_SYS_IFC_SIZE == 2)
                || ifcMode == HAL_IFC_SIZE_16_MODE_MANUAL
#endif
                || ifcMode == HAL_IFC_SIZE_32_MODE_MANUAL
#endif
                ,
                "hal_IfcGetChannel called with an autodisable %d mode", ifcMode);

    UINT32 status = hal_SysEnterCriticalSection();

    channel = SYS_IFC_CH_TO_USE(hwp_sysIfc->get_ch) ;

    if (channel >= SYS_IFC_STD_CHAN_NB)
    {
        hal_SysExitCriticalSection(status);
        return HAL_UNKNOWN_CHANNEL;
    }

    hwp_sysIfc->std_ch[channel].tc =  0;
    hwp_sysIfc->std_ch[channel].start_addr = 0;
    g_halModuleIfcChannelOwner[channel] = requestId;
    hwp_sysIfc->std_ch[channel].control = (SYS_IFC_REQ_SRC(requestId)
                                           | ifcMode
                                           | SYS_IFC_CH_RD_HW_EXCH
                                           | SYS_IFC_ENABLE);

    hal_SysExitCriticalSection(status);

    return channel;
}


// =============================================================================
// hal_IfcSetStartAddress
// -----------------------------------------------------------------------------
/// Set the start address of a previously opened (with hal_IfcGetChannel)
/// IFC channel.
///
/// Should be used only when a channel has been locked.
///
/// @param channel Opened channel whose start address will be set.
/// @param startAddress Set start address of the channel.
// =============================================================================
PROTECTED VOID hal_IfcSetStartAddress(UINT8 channel, CONST UINT8* startAddress)
{
    // channel number too big
    IFC_ASSERT(channel < SYS_IFC_STD_CHAN_NB, channel);
    HAL_ASSERT(hwp_sysIfc->std_ch[channel].tc == 0, "Attempt to change an IFC start_address "
               "while tc != 0 (==%d), channel=%d", hwp_sysIfc->std_ch[channel].tc, channel);

    // Writing in the Tc register adds the number written
    // to the counter of bytes yet to transfer
    hwp_sysIfc->std_ch[channel].start_addr = (UINT32) startAddress;
}


