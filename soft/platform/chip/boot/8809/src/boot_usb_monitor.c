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

#include "hal_sys.h"
#include "boot.h"
#include "boot_usb.h"
#include "global_macros.h"
#include "sys_irq.h"
#include "bootp_debug.h"
#include "boot_usb_monitor.h"
#include "boot_host.h"
#include "debug_host_internals.h"
#include "boot_host.h"
#include "bootp_host.h"
#include "sys_ctrl.h"
#include "cfg_regs.h"
#include "bootp_usb.h"
#include "timer.h"
#include "tcu.h"
#include "debug_host.h"


#include <string.h>

// =============================================================================
// MACROS
// =============================================================================



#ifndef BOOT_USB_SEND_EVENT_DELAY
#define BOOT_USB_SEND_EVENT_DELAY    8192 // 0.5s
#endif /* BOOT_USB_SEND_EVENT_DELAY */

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// BOOT_HOST_USB_HEADER_T
// -----------------------------------------------------------------------------
/// Header of flow
// =============================================================================
typedef struct
{
    UINT16 size;
    UINT8  flowId;
} PACKED BOOT_HOST_USB_HEADER_T;

// =============================================================================
// BOOT_HOST_USB_CMD_RW_T
// -----------------------------------------------------------------------------
/// RW Command value
// =============================================================================
typedef enum
{
    BOOT_HOST_USB_CMD_RW_WRITE = 1,
    BOOT_HOST_USB_CMD_RW_READ  = 0
} BOOT_HOST_USB_CMD_RW_T;

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

// Those variables will not be initialized as they are used in the Boot code,
// and the init of the variables is done in the Boot Loader.
// They are initialized by the code which will use them.

// Callback
PRIVATE BOOT_HOST_USB_CALLBACK_T           g_BootHostUsbCallbackOs;

PRIVATE BOOT_HOST_USB_CALLBACK_H2PSTATUS_T g_BootHostUsbCallbackH2PStatus;

PRIVATE BOOT_HOST_USB_MONITOR_VAR_T        g_BootHostUsbVar;

// This variable will be initialized because it is const and will stay in ROM.
PROTECTED CONST BOOT_HOST_USB_MONITOR_VAR_T CONST *
__attribute__((section (".usbmonitorFixptr"),used))
g_BootHostUsbFixPtr = &g_BootHostUsbVar;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE BOOT_MONITOR_OP_STATUS_T boot_HostUsbDecode(UINT8 *data, UINT32 size);
PRIVATE HAL_USB_CALLBACK_RETURN_T
boot_HostUsbCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup);
PRIVATE VOID boot_HostUsbGenerateReadPacket(VOID);
PRIVATE VOID boot_HostUsbSendReadPacket(VOID);
PUBLIC VOID boot_HostUsbInit(VOID);
PUBLIC VOID boot_HostUsbEvent(UINT32 event);

// ============================================================================
// boot_HostUsbDecode
// -----------------------------------------------------------------------------
/// This function decode the host protocol.
/// For read access this function store all read command in FIFO. If fifo is
/// full RecvState is switch to #BOOT_HOST_USB_RECV_STATE_WAIT.
///
/// For write access this function write directly in memory.
///
/// For execution this function emulates H2P_STATUS and CTRL internal register
/// host and lauches the target executor.
///
/// @param data Data you want decode
/// @param size Size of this data
/// @return Monitor operation status: either #BOOT_MONITOR_OP_STATUS_EXIT or
/// #BOOT_MONITOR_OP_STATUS_CONTINUE, since this function is only called when
/// data have been received on the USB.
// =============================================================================
PRIVATE BOOT_MONITOR_OP_STATUS_T boot_HostUsbDecode(UINT8 *data, UINT32 size)
{
    BOOT_PROFILE_FUNCTION_ENTER(boot_HostUsbDecode);

    BOOT_HOST_USB_HEADER_T*            header;
    UINT32                             pos;
    UINT8                              fifoUse;
    UINT32                             regTmp;
    UINT8                              indexW;
    BOOT_HOST_USB_MONITOR_FIFO_ELEM_T* curFifoElem;
    // Since this function is only called when command have been received on the USB,
    // we will continue monitoring unless the #BOOT_HST_MONITOR_END_CMD is received,
    // in which case the #BOOT_MONITOR_OP_STATUS_EXIT will be returned.
    BOOT_MONITOR_OP_STATUS_T           exitStatus = BOOT_MONITOR_OP_STATUS_CONTINUE;

    fifoUse = g_BootHostUsbVar.WritePt-g_BootHostUsbVar.ReadPt;

    pos                        = 0;
    g_BootHostUsbVar.Residue   = 0;
    g_BootHostUsbVar.RecvState = BOOT_HOST_USB_RECV_STATE_RUN;

    while(pos + g_BootHostUsbVar.Residue < size)
    {
        // Read command Fifo full.
        if(fifoUse == BOOT_HOST_USB_FIFO_READ_SIZE)
        {
            g_BootHostUsbVar.Residue   = size-pos;
            g_BootHostUsbVar.RecvState = BOOT_HOST_USB_RECV_STATE_WAIT;
            break;
        }

        switch(g_BootHostUsbVar.State)
        {
            default:
            case BOOT_HOST_USB_STATE_IDLE:
                if(data[pos] == 0x0AD)
                {
                    g_BootHostUsbVar.State = BOOT_HOST_USB_STATE_HEADER;
                }
                pos++;
                break;

            case BOOT_HOST_USB_STATE_HEADER:
                if((size - pos) < 3)
                {
                    // Not enough data, wait for the next USB packet.
                    // (Exit the while loop).
                    g_BootHostUsbVar.Residue    = size - pos;
                }
                else
                {
                    header                      = (BOOT_HOST_USB_HEADER_T*) &data[pos];
                    g_BootHostUsbVar.DataSize   = HAL_ENDIAN_BIG_16(header->size);
                    if(header->flowId == 0xFF)
                    {
                        g_BootHostUsbVar.State  = BOOT_HOST_USB_STATE_SXS_CMD;
                    }
                    else
                    {
                        // Unsupported data are in the packet.
                        g_BootHostUsbVar.State  = BOOT_HOST_USB_STATE_DATA;
                    }
                    pos                        += 3;
                }
                break;

            case BOOT_HOST_USB_STATE_DATA:
                // Unsupported data remain in the USB packet.

                // Do a drawing to understand
                if((size - pos) < g_BootHostUsbVar.DataSize)
                {
                    // Data in several packets.
                    g_BootHostUsbVar.DataSize -= size - pos;
                    pos                        = size;
                }
                else
                {
                    // All data in one USB packet.
                    pos                       += g_BootHostUsbVar.DataSize;
                    g_BootHostUsbVar.DataSize  = 0;
                    g_BootHostUsbVar.State     = BOOT_HOST_USB_STATE_IDLE;
                }
                break;

            case BOOT_HOST_USB_STATE_SXS_CMD:
                // Get the command type.
                g_BootHostUsbVar.Cmd          = *((BOOT_HOST_USB_CMD_T*)&data[pos]);
                // Command type on one byte.
                pos++;
                g_BootHostUsbVar.DataSize--;
                g_BootHostUsbVar.State        = BOOT_HOST_USB_STATE_SXS_ADDR;
                break;

            case BOOT_HOST_USB_STATE_SXS_ADDR:
                // Get the address.
                if((size - pos) < 4)
                {
                    g_BootHostUsbVar.Residue  = size - pos;
                }
                else
                {
                    memcpy(&g_BootHostUsbVar.Addr, (VOID*)&data[pos],
                           sizeof(UINT32));
                    if(!g_BootHostUsbVar.Cmd.internal ||
                            g_BootHostUsbVar.Cmd.rw != BOOT_HOST_USB_CMD_RW_WRITE)
                    {
                        g_BootHostUsbVar.Addr = KSEG0(g_BootHostUsbVar.Addr);
                    }

                    pos        += 4;

                    switch(g_BootHostUsbVar.Cmd.rw)
                    {
                        case BOOT_HOST_USB_CMD_RW_WRITE:
                            g_BootHostUsbVar.State     = BOOT_HOST_USB_STATE_SXS_DATA;
                            g_BootHostUsbVar.DataSize -= 4;
                            break;

                        case BOOT_HOST_USB_CMD_RW_READ:
                            g_BootHostUsbVar.State     = BOOT_HOST_USB_STATE_SXS_RID;
                            g_BootHostUsbVar.DataSize -= 4;
                            break;
                    }
                }
                break;

            case BOOT_HOST_USB_STATE_SXS_DATA:
                // Write the data received in the packet.
                if(g_BootHostUsbVar.Cmd.internal &&
                        g_BootHostUsbVar.Cmd.rw == BOOT_HOST_USB_CMD_RW_WRITE)
                {
                    // Internal Write
                    BOOT_PROFILE_FUNCTION_ENTER(boot_HostUsbWriteInternal);

                    // We need the CRC, in addition to the data to write.
                    if((size - pos) < ((UINT32)g_BootHostUsbVar.DataSize - 1))
                    {
                        // Not enough data.
                        g_BootHostUsbVar.Residue = size - pos;
                    }
                    else
                    {
                        // All data available.
                        g_BootHostUsbVar.State     = BOOT_HOST_USB_STATE_DATA;

                        // Emulate H2P_STATUS and CTRL_SET internal register
                        switch(g_BootHostUsbVar.Addr)
                        {
                            case (UINT32)(&hwp_intRegDbgHost->CTRL_SET)&0x1FFFFFFF:
                                regTmp  = data[pos];
                                // Move pos to the CRC byte.
                                pos    += g_BootHostUsbVar.DataSize - 1;
                                g_BootHostUsbVar.DataSize = 1;
                                if(regTmp & DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_RESET)
                                {
                                    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_UNLOCK;
                                    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SOFT_RST;
                                }
                                break;

                            case (UINT32)(&hwp_intRegDbgHost->H2P_STATUS)&0x1FFFFFFF:
                                // Writing in this register means we are doing
                                // monitor stuff.

                                // Value to virtually write into the register.
                                regTmp                    = data[pos];
                                pos                      += g_BootHostUsbVar.DataSize-1;
                                g_BootHostUsbVar.Residue  = size - pos;

                                // Ask Lilian about that.
                                // Allow to call the callback before a potential
                                // reset, for example.
                                // Interest is dubious.
                                if(g_BootHostUsbCallbackOs != 0)
                                {
                                    g_BootHostUsbCallbackOs(data, pos);
                                }

                                // Save current state
                                // Prepare the next packet in case of execution command
                                // (eg fastpf)
                                // Copy residue at the beginning of the buffer, as the USB
                                // context (detectionm, etc) won't change.
                                if(g_BootHostUsbVar.Residue)
                                {
                                    memmove((UINT32*)HAL_SYS_GET_UNCACHED_ADDR(g_BootHostUsbVar.Context->BufferOut),
                                            (UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&g_BootHostUsbVar.Context->BufferOut[pos]),
                                            g_BootHostUsbVar.Residue);
                                }

                                // We discard the CRC, last byte of the packet,
                                // when we are in the next BOOT_HOST_USB_STATE_DATA state.
                                g_BootHostUsbVar.DataSize  = 1;
                                pos                        = 0;

                                // Extended Handler for H2P, called with what is written
                                // in the H2P register.
                                if(g_BootHostUsbCallbackH2PStatus)
                                {
                                    g_BootHostUsbCallbackH2PStatus(regTmp);
                                }
                                else
                                {
                                    switch(regTmp)
                                    {
                                        case BOOT_HST_MONITOR_START_CMD:
                                            // That command used to be used to enter into the monitor.
                                            // We now use a boot mode for that, so this command is
                                            // just used to send the BOOT_HST_MONITOR_START event
                                            // to acknowledge to the host (eg Remote PC's coolwatcher)
                                            // that we actually are in the monitor.
                                            boot_HostUsbEvent(BOOT_HST_MONITOR_START);
                                            break;

                                        case BOOT_HST_MONITOR_END_CMD:
                                            // We are required to leave the monitor.
                                            boot_HostUsbEvent(BOOT_HST_MONITOR_END);
                                            exitStatus = BOOT_MONITOR_OP_STATUS_EXIT;
                                            break;

                                        case BOOT_HST_MONITOR_X_CMD:
                                            boot_HostUsbEvent(BOOT_HST_START_CMD_TREATMENT);

                                            // There is no need to flush the cache here as the
                                            // boot_HstMonitorXCtx is written by the CPU (We are
                                            // in the USB monitor).

                                            switch(boot_HstMonitorXCtx.cmdType)
                                            {
                                                case BOOT_HST_MONITOR_X_CMD:
                                                    // Execute at the PC in the execution context with the provided SP
                                                    boot_HstMonitorX();
                                                    break;

                                                default:
                                                    if (boot_HstMonitorExtendedHandler.
                                                            ExtendedCmdHandler != 0)
                                                    {
                                                        ((VOID(*)(VOID*))boot_HstMonitorExtendedHandler.
                                                         ExtendedCmdHandler)((VOID*)&boot_HstMonitorXCtx);
                                                    }
                                            }
                                            boot_HostUsbEvent(BOOT_HST_END_CMD_TREATMENT);
                                            break;

                                        default:
                                            boot_HostUsbEvent(BOOT_HST_UNSUPPORTED_CMD);
                                            break;
                                    }
                                }
                            default:
                                // Unsupported internal register address.
                                pos += g_BootHostUsbVar.DataSize - 1;
                                g_BootHostUsbVar.DataSize = 1;
                                break;
                        }

                        // Restore current state
                        g_BootHostUsbVar.Residue = 0;
                    }
                    BOOT_PROFILE_FUNCTION_EXIT(boot_HostUsbWriteInternal);
                }
                else
                {
                    // Normal RW
                    if((size - pos) < ((UINT32)g_BootHostUsbVar.DataSize - 1))
                    {
                        memcpy((VOID*)g_BootHostUsbVar.Addr, &data[pos],
                               size - pos);
                        g_BootHostUsbVar.Addr     += size - pos;
                        g_BootHostUsbVar.DataSize -= size - pos;
                        // End of the packet reached.
                        pos                        = size;
                    }
                    else
                    {
                        memcpy((VOID*)g_BootHostUsbVar.Addr, &data[pos],
                               g_BootHostUsbVar.DataSize-1);

                        // Discard CRC. (Through the BOOT_HOST_USB_STATE_DATA state)
                        pos                       += g_BootHostUsbVar.DataSize-1;
                        g_BootHostUsbVar.DataSize  = 1;
                        g_BootHostUsbVar.State     = BOOT_HOST_USB_STATE_DATA;
                    }
                }
                break;

            case BOOT_HOST_USB_STATE_SXS_RID:
                // Read.
                if(data[pos] == 0) // Event
                {
                    // Discarded.
                    pos++;
                    g_BootHostUsbVar.DataSize--;
                    g_BootHostUsbVar.State = BOOT_HOST_USB_STATE_DATA;
                    break;
                }

                // Index where to write the command in the read Fifo.
                indexW      = g_BootHostUsbVar.WritePt%BOOT_HOST_USB_FIFO_READ_SIZE;
                curFifoElem = &g_BootHostUsbVar.Context->Fifo[indexW];
                curFifoElem->Rid  = data[pos];
                curFifoElem->Addr = g_BootHostUsbVar.Addr;
                switch(g_BootHostUsbVar.Cmd.access)
                {
                    case BOOT_HOST_USB_CMD_ACCESS_BYTE     :
                        curFifoElem->Size = 1;
                        break;

                    case BOOT_HOST_USB_CMD_ACCESS_HALF_WORD:
                        curFifoElem->Size = 2;
                        break;

                    case BOOT_HOST_USB_CMD_ACCESS_WORD     :
                        curFifoElem->Size = 4;
                        break;

                    case BOOT_HOST_USB_CMD_ACCESS_BLOCK    :
                        g_BootHostUsbVar.State = BOOT_HOST_USB_STATE_SXS_READ_SIZE;
                        break;
                }

                if(g_BootHostUsbVar.Cmd.access != BOOT_HOST_USB_CMD_ACCESS_BLOCK)
                {
                    // Discard CRC
                    g_BootHostUsbVar.State = BOOT_HOST_USB_STATE_DATA;

                    // Next slot in the Fifo.
                    fifoUse++;
                    g_BootHostUsbVar.WritePt++;
                }

                pos++;
                g_BootHostUsbVar.DataSize--;
                break;


            case BOOT_HOST_USB_STATE_SXS_READ_SIZE:
                // Get the size for a read block.
                indexW      = g_BootHostUsbVar.WritePt%BOOT_HOST_USB_FIFO_READ_SIZE;
                curFifoElem = &g_BootHostUsbVar.Context->Fifo[indexW];

                if((size - pos) < 2)
                {
                    // Get the remaining data in the next packet.
                    g_BootHostUsbVar.Residue = size - pos;
                }
                else
                {
                    curFifoElem->Size = HAL_ENDIAN_BIG_16(*((UINT16 *)&data[pos]));
                    if(curFifoElem->Size <= BOOT_HOST_USB_BUFFER_IN_SIZE-6)
                    {
                        // Read size fits in the send buffer to the remote PC.
                        // This is a new command in the Fifo, increment pointers.
                        fifoUse++;
                        g_BootHostUsbVar.WritePt++;
                    }

                    // we just read 2 bytes.
                    pos                       += 2;
                    g_BootHostUsbVar.DataSize -= 2;
                    g_BootHostUsbVar.State = BOOT_HOST_USB_STATE_DATA;
                }
                break;
        }

        if(exitStatus == BOOT_MONITOR_OP_STATUS_EXIT)
        {
            // This come form a H2P_Status command.
            break;
        }
    }

    // Residue represent data that are going to be sent in the next run,
    // concatenated with the (next) new received packet.
    if(g_BootHostUsbCallbackOs != 0)
    {
        g_BootHostUsbCallbackOs(data, size-g_BootHostUsbVar.Residue);
    }

    if(g_BootHostUsbVar.Residue != 0 && pos != 0)
    {
        // Copy residue at the beginning.
        memmove((UINT32*)HAL_SYS_GET_UNCACHED_ADDR(g_BootHostUsbVar.Context->BufferOut),
                (UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&g_BootHostUsbVar.Context->BufferOut[pos]),
                g_BootHostUsbVar.Residue);
    }
    BOOT_PROFILE_FUNCTION_EXIT(boot_HostUsbDecode);

    return(exitStatus);
}


PRIVATE HAL_USB_CALLBACK_RETURN_T
boot_HostUsbCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            return(HAL_USB_CALLBACK_RETURN_OK);
            break;
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            boot_HostUsbInit();
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            break;
    }
    return(HAL_USB_CALLBACK_RETURN_KO);
}

// ============================================================================
// boot_HostUsbGenerateReadPacket
// -----------------------------------------------------------------------------
/// This function generate host packet resulting from a read request.
/// The read Fifo describes a read access. Those data are read, a USB packet
/// is built with those data.
// =============================================================================
PRIVATE VOID boot_HostUsbGenerateReadPacket(VOID)
{
    BOOT_HOST_USB_HEADER_T*            header;
    UINT8*                             pos;
    UINT16                             i;
    UINT8                              sum;
    UINT8                              indexR;
    BOOT_HOST_USB_MONITOR_FIFO_ELEM_T* curFifoElem;

    BOOT_PROFILE_FUNCTION_ENTER(boot_HostUsbGenerateReadPacket);

    while(g_BootHostUsbVar.ReadPt != g_BootHostUsbVar.WritePt)
    {
        // We have read commands.
        indexR      = g_BootHostUsbVar.ReadPt%BOOT_HOST_USB_FIFO_READ_SIZE;
        curFifoElem = &g_BootHostUsbVar.Context->Fifo[indexR];

        if(curFifoElem->Size+1+5 >
                BOOT_HOST_USB_BUFFER_IN_SIZE-g_BootHostUsbVar.PositionBufferIn)
        {
            // Not enough room for the next read packet.
            break;
        }

        // Position in the send buffer used to send this packet.
        pos  = &g_BootHostUsbVar.Context
               ->BufferIn[g_BootHostUsbVar.CurrentBufferIn]
               .Buffer[g_BootHostUsbVar.PositionBufferIn];

        // Host packet header.
        *pos            = 0xAD;
        pos++;
        header          = (BOOT_HOST_USB_HEADER_T*) pos;
        header->size    = HAL_ENDIAN_BIG_16(curFifoElem->Size+2);
        header->flowId  = 0xFF;
        pos            += sizeof(BOOT_HOST_USB_HEADER_T);
        *pos            = curFifoElem->Rid;

        // Read data.
        memcpy(pos+1,
               (VOID*)HAL_SYS_GET_UNCACHED_ADDR(curFifoElem->Addr),
               curFifoElem->Size);

        // Generate checksum
        for(i = 0, sum = 0; i < curFifoElem->Size+1; ++i)
        {
            sum ^= *pos;
            pos++;
        }

        // Xor with the flow Id (0xFF)
        *pos                               = ~sum;

        // Increment position in the send buffer.
        g_BootHostUsbVar.PositionBufferIn += curFifoElem->Size+5+1;

        // Go to the next command.
        g_BootHostUsbVar.ReadPt++;
    }

    BOOT_PROFILE_FUNCTION_EXIT(boot_HostUsbGenerateReadPacket);
}


// ============================================================================
// boot_HostUsbSendReadPacket
// -----------------------------------------------------------------------------
/// Send packet generated by boot_HostUsbGenerateReadPacket and switch output
/// swap buffer
// =============================================================================
PRIVATE VOID boot_HostUsbSendReadPacket(VOID)
{
    BOOT_PROFILE_FUNCTION_ENTER(boot_HostUsbSendReadPacket);

    boot_UsbSend(1, g_BootHostUsbVar.Context
                 ->BufferIn[g_BootHostUsbVar.CurrentBufferIn].Buffer,
                 g_BootHostUsbVar.PositionBufferIn, 0);

    // Use the other swap buffer ...
    g_BootHostUsbVar.CurrentBufferIn++;
    g_BootHostUsbVar.CurrentBufferIn  %= 2;

    // ... From the beginning of the buffer.
    g_BootHostUsbVar.PositionBufferIn  = 0;

    BOOT_PROFILE_FUNCTION_EXIT(boot_HostUsbSendReadPacket);
}


// ============================================================================
// hal_HostUsbInit
// -----------------------------------------------------------------------------
/// Init usb host. Clear FIFO and host variables
// =============================================================================
PUBLIC VOID boot_HostUsbInit(VOID)
{
    BOOT_PROFILE_FUNCTION_ENTER(boot_HostUsbInit);
    g_BootHostUsbVar.State             = BOOT_HOST_USB_STATE_IDLE;
    g_BootHostUsbVar.ReadPt            = 0;
    g_BootHostUsbVar.WritePt           = 0;
    g_BootHostUsbVar.CurrentBufferIn   = 0;
    g_BootHostUsbVar.PositionBufferIn  = 0;
    g_BootHostUsbVar.Residue           = 0;
    g_BootHostUsbVar.RecvState         = BOOT_HOST_USB_RECV_STATE_RUN;
    boot_UsbEpEnableInterrupt(HAL_USB_EP_DIRECTION_IN (1), FALSE);
    boot_UsbEpEnableInterrupt(HAL_USB_EP_DIRECTION_OUT(1), FALSE);
    boot_UsbRecv(1, g_BootHostUsbVar.Context->BufferOut,
                 BOOT_HOST_USB_BUFFER_OUT_SIZE, 0);

    BOOT_PROFILE_FUNCTION_EXIT(boot_HostUsbInit);
}

PRIVATE CONST HAL_USB_EP_DESCRIPTOR_T boot_HostUsbEp1In =
{
    .ep        = HAL_USB_EP_DIRECTION_IN(1),
    .type      = HAL_USB_EP_TYPE_BULK,
    .interval  = 1,
    .callback  = 0
};

PRIVATE  CONST HAL_USB_EP_DESCRIPTOR_T boot_HostUsbEp1Out =
{
    .ep        = HAL_USB_EP_DIRECTION_OUT(1),
    .type      = HAL_USB_EP_TYPE_BULK,
    .interval  = 1,
    .callback  = 0
};

// =============================================================================
// HAL_USB_EP_DESCRIPTOR_CONST_PTR_T
// -----------------------------------------------------------------------------
/// constante ep ep list
// =============================================================================
typedef CONST HAL_USB_EP_DESCRIPTOR_T * HAL_USB_EP_DESCRIPTOR_CONST_PTR_T;

PRIVATE CONST HAL_USB_EP_DESCRIPTOR_CONST_PTR_T boot_HostUsbEpList[] =
{
    &boot_HostUsbEp1In,
    &boot_HostUsbEp1Out,
    0
};

PUBLIC CONST HAL_USB_INTERFACE_DESCRIPTOR_T boot_HostUsbInterface =
{
    .usbClass    = 0xFF,
    .usbSubClass = 0x66,
    .usbProto    = 0x66,
    .interfaceIdx = 0x00,
    .epList      = (HAL_USB_EP_DESCRIPTOR_T**) boot_HostUsbEpList,
    .description  = (UINT8*)"Coolsand Usb",
    .callback    = boot_HostUsbCallback
};

// =============================================================================
// HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T
// -----------------------------------------------------------------------------
/// constante interface config
// =============================================================================
typedef CONST HAL_USB_INTERFACE_DESCRIPTOR_T *
HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T;

PRIVATE CONST HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T
boot_HostUsbInterfaceList[] =
{
    &boot_HostUsbInterface,
    0
};

PRIVATE CONST HAL_USB_CONFIG_DESCRIPTOR_T boot_HostUsbConfig =
{
    .attrib         =  0,
    .maxPower       = 50,
    .interfaceList  = (HAL_USB_INTERFACE_DESCRIPTOR_T**)
    boot_HostUsbInterfaceList,
    .description    = (UINT8*)"Coolsand default",
    .configIdx      = 1
};

// =============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T
// -----------------------------------------------------------------------------
/// Constant usb config
// =============================================================================
typedef CONST HAL_USB_CONFIG_DESCRIPTOR_T *
HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T;


PRIVATE CONST HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T boot_HostUsbConfigList[] =
{
    &boot_HostUsbConfig,
    0
};

PRIVATE CONST HAL_USB_DEVICE_DESCRIPTOR_T boot_HostUsbDesc =
{
    .usbClass     = 0,
    .usbSubClass  = 0,
    .usbProto     = 0,
    .vendor       = 0,
    .product      = 0,
    .version      = 0,
    .configList   = (HAL_USB_CONFIG_DESCRIPTOR_T**) boot_HostUsbConfigList,
    .description  = (UINT8*)"Coolsand",
    .serialNumber = (UINT8*)"Host USB",
    .usbMode      = 0
};

// =============================================================================
// boot_HostUsbDeviceCallback
// -----------------------------------------------------------------------------
/// Callback use to define the host usb descriptor. It's call by USB driver
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* boot_HostUsbDeviceCallback(BOOL speedEnum)
{
    return (HAL_USB_DEVICE_DESCRIPTOR_T*) &boot_HostUsbDesc;
}

// =============================================================================
// boot_HostUsbEvent
// -----------------------------------------------------------------------------
/// Send USB host event with a timeout
///
/// @param event Event number, you want to send.
// =============================================================================
PUBLIC VOID boot_HostUsbEvent(UINT32 event)
{
    BOOT_HOST_USB_HEADER_T*            header;
    UINT8                              trame[10];
    UINT32                             startTime = hwp_timer->HWTimer_CurVal;

    header = (BOOT_HOST_USB_HEADER_T*) &trame[1];

    // Generate Event packet
    trame[0]               = 0xAD;
    header->size           = HAL_ENDIAN_BIG_16(0x06);
    header->flowId         = 0xFF;
    trame[4]               = 0;
    memcpy(&trame[5], &event, sizeof(UINT32));
    trame[9]               = ~(trame[4]^trame[5]^trame[6]^trame[7]^trame[8]);

    // Wait the last packet
    while(boot_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == FALSE)
    {
        if((hwp_timer->HWTimer_CurVal - startTime) >= BOOT_USB_SEND_EVENT_DELAY)
        {
            BOOT_PROFILE_PULSE(BOOT_USB_MONITOR_EVENT_TIMEOUT);
            return;
        }
    }
    boot_UsbSend(1, trame, sizeof(trame), 0);
    while(boot_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == FALSE)
    {
        if((hwp_timer->HWTimer_CurVal - startTime) >= BOOT_USB_SEND_EVENT_DELAY)
        {
            BOOT_PROFILE_PULSE(BOOT_USB_MONITOR_EVENT_TIMEOUT);
            break;
        }
    }
}

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// boot_HostUsbClockEnable
// -----------------------------------------------------------------------------
/// Enable the USB clock.
/// The USB clock directly derives (is divided by 13 from) the
/// (fast) Pll's 624 Mhz clock.
/// FIXME What should we do in case of no pll lock?
/// Need a timeout?
// =============================================================================
PUBLIC VOID boot_HostUsbClockEnable(VOID)
{
    hwp_sysCtrl->REG_DBG        = SYS_CTRL_PROTECT_UNLOCK;

    // Turning off the Pll and reset of the Lock detector.
    // (Normally, we are just out of reset here, so there
    // is no real need to put the Pll in power down, as
    // that is its reset state.)
    hwp_sysCtrl->Pll_Ctrl =     SYS_CTRL_PLL_ENABLE_POWER_DOWN
                                | SYS_CTRL_PLL_BYPASS_PASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_RESET;

    // Configuration of the PLL's dividers.
    // Here, we should write PLL settings through SPI in the ABB.
    // Normally, the PLL is properly configured by default in the ABB registers.

    // Enable ABB's PLL, through the Sys Ctrl, via the PLL_PD line.
    hwp_sysCtrl->Pll_Ctrl =     SYS_CTRL_PLL_ENABLE_ENABLE
                                | SYS_CTRL_PLL_BYPASS_PASS
                                | SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE
                                | SYS_CTRL_PLL_LOCK_RESET_NO_RESET;

    // Wait PLL lock
    while ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0);

    // Enable of the output of the PLL's Fast clock.
    hwp_sysCtrl->Pll_Ctrl  |= SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE;

    hwp_sysCtrl->REG_DBG    = SYS_CTRL_PROTECT_LOCK;
}

// =============================================================================
// boot_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
/// @param context Define the context use. The context containt read request
/// fifo and buffer for read and write on USB
/// @param callback This callback is use to change the parsing process of host
/// protocol
// =============================================================================
PUBLIC VOID boot_HostUsbOpen(BOOT_HOST_USB_MONITOR_CTX_T*       context,
                             BOOT_HOST_USB_CALLBACK_T           callback,
                             BOOT_HOST_USB_CALLBACK_H2PSTATUS_T callbackH2pstatus)
{
    g_BootHostUsbVar.Context       = context;
    g_BootHostUsbCallbackOs        = callback;
    g_BootHostUsbCallbackH2PStatus = callbackH2pstatus;
    // Enable Usb Interrupt
    hwp_sysIrq->Mask_Set           = SYS_IRQ_SYS_IRQ_USBC;
    boot_HostUsbInit();
}

// =============================================================================
// boot_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb (disable USB interrupt)
// =============================================================================
PUBLIC VOID boot_HostUsbClose(VOID)
{
    // Disable Usb Interrupt
    hwp_sysIrq->Mask_Clear = SYS_IRQ_SYS_IRQ_USBC;
}

// =============================================================================
// boot_HostUsbSend
// -----------------------------------------------------------------------------
/// Generate packet must be send and send it
/// @return \c TRUE If a transfer is in progress.
// =============================================================================
PUBLIC BOOL boot_HostUsbSend(VOID)
{
    if(g_BootHostUsbVar.ReadPt != g_BootHostUsbVar.WritePt) // FIFO not empty
    {
        boot_HostUsbGenerateReadPacket();
    }

    // Previous packet send & new packet ready to be send ?
    if(boot_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == TRUE &&
            g_BootHostUsbVar.PositionBufferIn != 0)
    {
        boot_HostUsbSendReadPacket();
    }

    if(boot_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1) == FALSE))
    {
        return(TRUE);
    }
    return(FALSE);
}

// =============================================================================
// boot_HostUsbRecv
// -----------------------------------------------------------------------------
/// Receive USB packet and parse it
///
/// @return 1 if the exit command (#BOOT_HST_MONITOR_END_CMD) is received.
// =============================================================================
PUBLIC BOOT_MONITOR_OP_STATUS_T boot_HostUsbRecv(VOID)
{
    UINT8  *data;
    UINT32  size;
    UINT8   fifoUse;
    UINT8   startNewReception   = 0;
    BOOT_MONITOR_OP_STATUS_T exitStatus = BOOT_MONITOR_OP_STATUS_NONE;

    // Write Ptr and read Ptr go above fifo size,
    // but the modulo BOOT_HOST_USB_FIFO_READ_SIZE is used
    // when the Ptr are accessed.
    // It is enforced somewhere else that write - read
    // is always within fifo range.
    fifoUse = g_BootHostUsbVar.WritePt-g_BootHostUsbVar.ReadPt;

    data = (UINT8*) HAL_SYS_GET_UNCACHED_ADDR(g_BootHostUsbVar.Context->BufferOut);

    switch(g_BootHostUsbVar.RecvState)
    {
        case BOOT_HOST_USB_RECV_STATE_RUN:
            if(boot_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_OUT(1)))
            {
                // We received a command: we are not waiting anymore but actually acting
                // as a monitor.
                hwp_debugHost->p2h_status   = BOOT_HST_STATUS_NONE;

                // If a command is split on several packets, the beginning
                // is in the residue.
                size = boot_UsbEpRxTransferedSize(1)+g_BootHostUsbVar.Residue;

                // Decode received packet.
                exitStatus = boot_HostUsbDecode(data, size);
                startNewReception       = 1;
            }
            break;

        // Fifo Full
        case BOOT_HOST_USB_RECV_STATE_WAIT:
            if(fifoUse != BOOT_HOST_USB_FIFO_READ_SIZE)
            {
                // boot_HostUsbDecode can change state to BOOT_HOST_USB_RECV_STATE_RUN
                // Packet may remains to be decoded in the Fifo.
                exitStatus = boot_HostUsbDecode(data, g_BootHostUsbVar.Residue);
                startNewReception       = 1;
            }
            break;
    }

    if(g_BootHostUsbVar.RecvState == BOOT_HOST_USB_RECV_STATE_RUN &&
            startNewReception == 1)
    {
        // Fetch new usb packet
        boot_UsbRecv(1, &g_BootHostUsbVar.Context->BufferOut[g_BootHostUsbVar.Residue],
                     BOOT_HOST_USB_BUFFER_OUT_SIZE-g_BootHostUsbVar.Residue, 0);
    }
    return(exitStatus);
}

