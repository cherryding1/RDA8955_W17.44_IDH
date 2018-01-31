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


#if (CHIP_HAS_USB == 1)

#ifdef HAL_USB_DRIVER_DONT_USE_ROMED_CODE

// =============================================================================
// HEADERS
// =============================================================================

#include "cs_types.h"

#include "global_macros.h"
#include "sys_irq.h"
#include "debug_host_internals.h"

#include "cfg_regs.h"

#include "sys_ctrl.h"
#include "timer.h"

#include "hal_sys.h"
#include "hal_usb.h"
#include "hal_host.h"
#include "hal_trace.h"
#include "boot_sector_driver.h"
#include "halp_sys.h"
#include "halp_usb_monitor.h"
#include "halp_debug.h"
#include "halp_gdb_stub.h"
#include "hal_usb_descriptor.h"

#include <string.h>

#include "boot.h"
#include "boot_host.h"
#include "boot_usb_monitor.h"

extern CONST BOOT_HOST_USB_MONITOR_VAR_T CONST * g_BootHostUsbFixPtr;

// =============================================================================
// MACROS
// =============================================================================


#ifndef HAL_USB_SEND_EVENT_DELAY
#define HAL_USB_SEND_EVENT_DELAY    8192 // 0.5s
#endif /* HAL_USB_SEND_EVENT_DELAY */


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_HOST_USB_HEADER_T
// -----------------------------------------------------------------------------
/// Header of flow
// =============================================================================
typedef struct
{
    UINT16 size;
    UINT8  flowId;
} PACKED HAL_HOST_USB_HEADER_T;


// =============================================================================
// HAL_HOST_USB_CMD_RW_T
// -----------------------------------------------------------------------------
/// RW Command value
// =============================================================================
typedef enum
{
    HAL_HOST_USB_CMD_RW_READ  = 0,
    HAL_HOST_USB_CMD_RW_WRITE = 1,
} HAL_HOST_USB_CMD_RW_T;


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

// Callback
PRIVATE HAL_HOST_USB_CALLBACK_T     g_HalHostUsbCallbackOs        = 0;

PRIVATE HAL_HOST_USB_MONITOR_VAR_T** g_HalHostUsbVar =
    (HAL_HOST_USB_MONITOR_VAR_T**)&g_BootHostUsbFixPtr;

PRIVATE HAL_HOST_USB_MONITOR_CTX_T USB_UCBSS_INTERNAL  g_halHostUsbContext;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE UINT8 hal_HostUsbDecode(UINT8 *data, UINT32 size);
PRIVATE VOID hal_HostUsbGenerateReadPacket(VOID);
PRIVATE VOID hal_HostUsbSendReadPacket(VOID);
PRIVATE VOID hal_HostUsbInit(VOID);
PUBLIC VOID hal_HostUsbEvent(UINT32 event);

// ============================================================================
// hal_HostUsbDecode
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
// =============================================================================
PRIVATE UINT8 hal_HostUsbDecode(UINT8 *data, UINT32 size)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_HostUsbDecode);

    HAL_HOST_USB_HEADER_T*             header;
    UINT32                             pos;
    UINT8                              fifoUse;
    UINT32                             regTmp;
    UINT8                              indexW;
    HAL_HOST_USB_MONITOR_FIFO_ELEM_T*  curFifoElem;
    UINT8                              exitStatus = 0;

    fifoUse = (*g_HalHostUsbVar)->WritePt-(*g_HalHostUsbVar)->ReadPt;

    pos                        = 0;
    (*g_HalHostUsbVar)->Residue   = 0;
    (*g_HalHostUsbVar)->RecvState = HAL_HOST_USB_RECV_STATE_RUN;

    while(pos + (*g_HalHostUsbVar)->Residue < size)
    {
        // Read command Fifo full.
        if(fifoUse == HAL_HOST_USB_FIFO_READ_SIZE)
        {
            (*g_HalHostUsbVar)->Residue   = size-pos;
            (*g_HalHostUsbVar)->RecvState = HAL_HOST_USB_RECV_STATE_WAIT;
            break;
        }
        switch((*g_HalHostUsbVar)->State)
        {
            default:
            case HAL_HOST_USB_STATE_IDLE:
                if(data[pos] == 0x0AD)
                {
                    (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_HEADER;
                }
                pos++;
                break;
            case HAL_HOST_USB_STATE_HEADER:
                if((size - pos) < 3)
                {
                    // Not enough data, wait for the next USB packet.
                    // (Exit the while loop).
                    (*g_HalHostUsbVar)->Residue = size - pos;
                }
                else
                {
                    header                   = (HAL_HOST_USB_HEADER_T*) &data[pos];
                    (*g_HalHostUsbVar)->DataSize   = HAL_ENDIAN_BIG_16(header->size);
                    if(header->flowId == 0xFF)
                    {
                        (*g_HalHostUsbVar)->State  = HAL_HOST_USB_STATE_SXS_CMD;
                    }
                    else
                    {
                        // Unsupported data are in the packet.
                        (*g_HalHostUsbVar)->State  = HAL_HOST_USB_STATE_DATA;
                    }
                    pos                        += 3;
                }
                break;
            case HAL_HOST_USB_STATE_DATA:
                // Unsupported data remain in the USB packet.

                // Do a drawing to understand
                if((size - pos) < (*g_HalHostUsbVar)->DataSize)
                {
                    // Data in several packets.
                    (*g_HalHostUsbVar)->DataSize -= size - pos;
                    pos                        = size;
                }
                else
                {
                    // All data in one USB packet.
                    pos                       += (*g_HalHostUsbVar)->DataSize;
                    (*g_HalHostUsbVar)->DataSize  = 0;
                    (*g_HalHostUsbVar)->State     = HAL_HOST_USB_STATE_IDLE;
                }
                break;
            case HAL_HOST_USB_STATE_SXS_CMD:
                // Get the command type.
                (*g_HalHostUsbVar)->Cmd          = *((HAL_HOST_USB_CMD_T*)&data[pos]);
                // Command type on one byte.
                pos++;
                (*g_HalHostUsbVar)->DataSize--;
                (*g_HalHostUsbVar)->State        = HAL_HOST_USB_STATE_SXS_ADDR;
                break;
            case HAL_HOST_USB_STATE_SXS_ADDR:
                // Get the address.
                if((size - pos) < 4)
                {
                    (*g_HalHostUsbVar)->Residue  = size - pos;
                }
                else
                {
                    memcpy(&(*g_HalHostUsbVar)->Addr, (VOID*)&data[pos],
                           sizeof(UINT32));
                    if(!(*g_HalHostUsbVar)->Cmd.internal ||
                            (*g_HalHostUsbVar)->Cmd.rw != HAL_HOST_USB_CMD_RW_WRITE)
                    {
                        (*g_HalHostUsbVar)->Addr |= 0x80000000;
                    }
                    pos += 4;
                    (*g_HalHostUsbVar)->DataSize -= 4;
                    (*g_HalHostUsbVar)->Residue = 0;
                    switch((*g_HalHostUsbVar)->Cmd.rw)
                    {
                        case HAL_HOST_USB_CMD_RW_READ:
                            (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_RID;
                            break;
                        case HAL_HOST_USB_CMD_RW_WRITE:
                            if ((*g_HalHostUsbVar)->Cmd.internal != 0)
                            {
                                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_WINT;
                            }
                            else if ((*g_HalHostUsbVar)->Cmd.access == HAL_HOST_USB_CMD_ACCESS_BYTE)
                            {
                                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_W8;
                            }
                            else if ((*g_HalHostUsbVar)->Cmd.access == HAL_HOST_USB_CMD_ACCESS_HALF_WORD &&
                                     ((*g_HalHostUsbVar)->Addr & 0x1) == 0 &&
                                     ((*g_HalHostUsbVar)->DataSize - 1) <= 2)
                            {
                                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_W16;
                            }
                            else if ((*g_HalHostUsbVar)->Cmd.access == HAL_HOST_USB_CMD_ACCESS_WORD &&
                                     ((*g_HalHostUsbVar)->Addr & 0x3) == 0 &&
                                     ((*g_HalHostUsbVar)->DataSize - 1) <= 4)
                            {
                                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_W32;
                            }
                            else
                            {
                                // Block write, or invalid half-word / word write
                                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_WDATA;
                            }
                            break;
                    }
                }
                break;
            case HAL_HOST_USB_STATE_SXS_WINT:
                // Internal Write
                HAL_PROFILE_FUNCTION_ENTER(hal_HostUsbWriteInternal);
                if((size - pos) < ((UINT32)(*g_HalHostUsbVar)->DataSize - 1))
                {
                    (*g_HalHostUsbVar)->Residue = size - pos;
                }
                else
                {
                    // All data available.
                    (*g_HalHostUsbVar)->State     = HAL_HOST_USB_STATE_DATA;

                    // Emulate H2P_STATUS and CTRL_SET internal register
                    switch((*g_HalHostUsbVar)->Addr)
                    {
                        case (UINT32)(&hwp_intRegDbgHost->CTRL_SET)&0x1FFFFFFF:
                            regTmp  = data[pos];
                            // Move pos to the CRC byte.
                            pos    += (*g_HalHostUsbVar)->DataSize - 1;
                            (*g_HalHostUsbVar)->DataSize = 1;

                            if(regTmp & DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_RESET)
                            {
                                hal_SysRestart();
                            }
                            break;
                        case (UINT32)(&hwp_intRegDbgHost->H2P_STATUS)&0x1FFFFFFF:
                            // Writing in this register means we are doing
                            // monitor stuff.

                            // Value to virtually write into the register.
                            regTmp                    = data[pos];
                            pos                      += (*g_HalHostUsbVar)->DataSize-1;
                            (*g_HalHostUsbVar)->Residue  = size - pos;

                            // Save current state
                            // Prepare the next packet in case of execution command
                            // (eg fastpf)
                            // Copy residue at the beginning of the buffer, as the USB
                            // context (detectionm, etc) won't change.
                            if((*g_HalHostUsbVar)->Residue)
                            {
                                memmove((UINT32*)HAL_SYS_GET_UNCACHED_ADDR((*g_HalHostUsbVar)->Context->BufferOut),
                                        (UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&(*g_HalHostUsbVar)->Context->BufferOut[pos]),
                                        (*g_HalHostUsbVar)->Residue);
                            }

                            // We discard the CRC, last byte of the packet,
                            // when we are in the next BOOT_HOST_USB_STATE_DATA state.
                            (*g_HalHostUsbVar)->DataSize  = 1;
                            pos                           = 0;
                            switch(regTmp)
                            {
                                case BOOT_HST_MONITOR_ENTER:
                                    // To restart in the Boot Monitor, by going through
                                    // the Boot Sector and we power cycle the USB.
                                    boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);
                                    hal_SysRestart();
                                    break;

                                case BOOT_HST_CALIB_ENTER:
                                    // To restart in the Boot Monitor, by going through
                                    // the Boot Sector and we power cycle the USB.
                                    boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_CALIB);
                                    hal_SysRestart();
                                    break;

                                case BOOT_HST_MONITOR_X_CMD:
                                    hal_HostUsbEvent(BOOT_HST_START_CMD_TREATMENT);
                                    switch(boot_HstMonitorXCtx.cmdType)
                                    {
                                        case BOOT_HST_MONITOR_X_CMD:
                                            // Execute at the PC in the execution context with
                                            // the provided SP.
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
                                    hal_HostUsbEvent(BOOT_HST_END_CMD_TREATMENT);
                                    break;

                                default:
                                    hal_HostUsbEvent(BOOT_HST_UNSUPPORTED_CMD);
                                    break;
                            }
                        default:
                            pos += (*g_HalHostUsbVar)->DataSize - 1;
                            (*g_HalHostUsbVar)->DataSize = 1;
                            break;
                    }
                    // Restore current state
                    (*g_HalHostUsbVar)->Residue = 0;
                }
                HAL_PROFILE_FUNCTION_EXIT(hal_HostUsbWriteInternal);
                break;
            case HAL_HOST_USB_STATE_SXS_W8:
                // Write the 8-bit data received in the packet.
                *(UINT8 *)(*g_HalHostUsbVar)->Addr = data[pos];
                // Discard CRC. (Through the BOOT_HOST_USB_STATE_DATA state)
                pos++;
                (*g_HalHostUsbVar)->DataSize--;
                (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_DATA;
                break;
            case HAL_HOST_USB_STATE_SXS_W16:
            case HAL_HOST_USB_STATE_SXS_W32:
                // Write the 16-bit/32-bit data received in the packet.
                if((size - pos) < ((UINT32)(*g_HalHostUsbVar)->DataSize - 1))
                {
                    (*g_HalHostUsbVar)->Residue = size - pos;
                }
                else
                {
                    regTmp = 0;
                    for (int i=0; i<(*g_HalHostUsbVar)->DataSize-1; i++)
                    {
                        regTmp |= data[pos+i]<<(8*i);
                    }
                    if ((*g_HalHostUsbVar)->State == HAL_HOST_USB_STATE_SXS_W16)
                    {
                        *(UINT16 *)(*g_HalHostUsbVar)->Addr = (UINT16)regTmp;
                    }
                    else
                    {
                        *(UINT32 *)(*g_HalHostUsbVar)->Addr = regTmp;
                    }
                    // Discard CRC. (Through the BOOT_HOST_USB_STATE_DATA state)
                    pos                          += (*g_HalHostUsbVar)->DataSize-1;
                    (*g_HalHostUsbVar)->DataSize  = 1;
                    (*g_HalHostUsbVar)->State     = HAL_HOST_USB_STATE_DATA;
                }
                break;
            case HAL_HOST_USB_STATE_SXS_WDATA:
                // Write the data received in the packet.
                if((size - pos) < ((UINT32)(*g_HalHostUsbVar)->DataSize - 1))
                {
                    memcpy((VOID*)(*g_HalHostUsbVar)->Addr, &data[pos],
                           size - pos);
                    (*g_HalHostUsbVar)->Addr     += size - pos;
                    (*g_HalHostUsbVar)->DataSize -= size - pos;
                    // End of the packet reached.
                    pos                           = size;
                }
                else
                {
                    memcpy((VOID*)(*g_HalHostUsbVar)->Addr, &data[pos],
                           (*g_HalHostUsbVar)->DataSize-1);

                    // Discard CRC. (Through the BOOT_HOST_USB_STATE_DATA state)
                    pos                          += (*g_HalHostUsbVar)->DataSize-1;
                    (*g_HalHostUsbVar)->DataSize  = 1;
                    (*g_HalHostUsbVar)->State     = HAL_HOST_USB_STATE_DATA;
                }
                break;
            case HAL_HOST_USB_STATE_SXS_RID:
                // Read.
                if(data[pos] == 0) // Event
                {
                    // Discarded.
                    pos++;
                    (*g_HalHostUsbVar)->DataSize--;
                    (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_DATA;
                    break;
                }

                // Index where to write the command in the read Fifo.
                indexW      = (*g_HalHostUsbVar)->WritePt%HAL_HOST_USB_FIFO_READ_SIZE;
                curFifoElem = &(*g_HalHostUsbVar)->Context->Fifo[indexW];
                curFifoElem->Rid  = data[pos];
                curFifoElem->Addr = (*g_HalHostUsbVar)->Addr;
                switch((*g_HalHostUsbVar)->Cmd.access)
                {
                    case HAL_HOST_USB_CMD_ACCESS_BYTE     :
                        curFifoElem->Size = 1;
                        break;
                    case HAL_HOST_USB_CMD_ACCESS_HALF_WORD:
                        curFifoElem->Size = 2;
                        break;
                    case HAL_HOST_USB_CMD_ACCESS_WORD     :
                        curFifoElem->Size = 4;
                        break;
                    case HAL_HOST_USB_CMD_ACCESS_BLOCK    :
                        (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_SXS_READ_SIZE;
                        break;
                }

                if((*g_HalHostUsbVar)->Cmd.access != HAL_HOST_USB_CMD_ACCESS_BLOCK)
                {
                    // Discard CRC
                    (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_DATA;

                    // Next slot in the Fifo.
                    fifoUse++;
                    (*g_HalHostUsbVar)->WritePt++;
                }
                pos++;
                (*g_HalHostUsbVar)->DataSize--;
                break;

            case HAL_HOST_USB_STATE_SXS_READ_SIZE:
                // Get the size for a read block.
                indexW      = (*g_HalHostUsbVar)->WritePt%HAL_HOST_USB_FIFO_READ_SIZE;
                curFifoElem = &(*g_HalHostUsbVar)->Context->Fifo[indexW];

                if((size - pos) < 2)
                {
                    // Get the remaining data in the next packet.
                    (*g_HalHostUsbVar)->Residue = size - pos;
                }
                else
                {
                    curFifoElem->Size      =
                        HAL_ENDIAN_BIG_16(*((UINT16 *)&data[pos]));
                    if(curFifoElem->Size <= HAL_HOST_USB_BUFFER_IN_SIZE-6)
                    {
                        // Read size fits in the send buffer to the remote PC.
                        // This is a new command in the Fifo, increment pointers.
                        fifoUse++;
                        (*g_HalHostUsbVar)->WritePt++;
                    }

                    // we just read 2 bytes.
                    pos                       += 2;
                    (*g_HalHostUsbVar)->DataSize -= 2;
                    (*g_HalHostUsbVar)->State = HAL_HOST_USB_STATE_DATA;
                }
                break;
        }
        if(exitStatus)
        {
            // This come form a H2P_Status command.
            break;
        }
    }

    // OS cannot handle the packets if GDB is in place
    if(!hal_GdbIsInGdbLoop())
    {
        if(g_HalHostUsbCallbackOs != 0)
        {
            g_HalHostUsbCallbackOs(data, size-(*g_HalHostUsbVar)->Residue);
        }
    }

    if((*g_HalHostUsbVar)->Residue != 0 && pos != 0)
    {
        // Copy residue at the beginning.
        memmove((UINT32*)HAL_SYS_GET_UNCACHED_ADDR((*g_HalHostUsbVar)->Context->BufferOut),
                (UINT32*)HAL_SYS_GET_UNCACHED_ADDR(&(*g_HalHostUsbVar)->Context->BufferOut[pos]),
                (*g_HalHostUsbVar)->Residue);
    }
    HAL_PROFILE_FUNCTION_EXIT(hal_HostUsbDecode);

    return(exitStatus);
}


PUBLIC HAL_USB_CALLBACK_RETURN_T
hal_HostUsbCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
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
            hal_HostUsbInit();
            hal_SysUsbHostEnable(TRUE);
            hal_HstTraceSwitch(HAL_HST_TRACE_PORT_USB);
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            hal_SysUsbHostEnable(FALSE);
            hal_HstTraceSwitch(HAL_HST_TRACE_PORT_HOST);
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

// ============================================================================
// boot_HostUsbGenerateReadPacket
// -----------------------------------------------------------------------------
/// This function generate host packet resulting from a read request.
/// The read Fifo describes a read access. Those data are read, a USB packet
/// is built with those data.
// =============================================================================
PRIVATE VOID hal_HostUsbGenerateReadPacket(VOID)
{
    HAL_HOST_USB_HEADER_T*             header;
    UINT8*                             pos;
    UINT16                             i;
    UINT8                              sum;
    UINT8                              indexR;
    HAL_HOST_USB_MONITOR_FIFO_ELEM_T*  curFifoElem;

    HAL_PROFILE_FUNCTION_ENTER(hal_HostUsbGenerateReadPacket);

    while((*g_HalHostUsbVar)->ReadPt != (*g_HalHostUsbVar)->WritePt)
    {
        // We have read commands.
        indexR      = (*g_HalHostUsbVar)->ReadPt%HAL_HOST_USB_FIFO_READ_SIZE;
        curFifoElem = &(*g_HalHostUsbVar)->Context->Fifo[indexR];

        if(curFifoElem->Size+1+5 >
                HAL_HOST_USB_BUFFER_IN_SIZE-(*g_HalHostUsbVar)->PositionBufferIn)
        {
            // Not enough room for the next read packet.
            break;
        }

        // Position in the send buffer used to send this packet.
        pos  = &(*g_HalHostUsbVar)->Context
               ->BufferIn[(*g_HalHostUsbVar)->CurrentBufferIn]
               .Buffer[(*g_HalHostUsbVar)->PositionBufferIn];

        // Host packet header.
        *pos            = 0xAD;
        pos++;
        header          = (HAL_HOST_USB_HEADER_T*) pos;
        header->size    = HAL_ENDIAN_BIG_16(curFifoElem->Size+2);
        header->flowId  = 0xFF;
        pos            += sizeof(HAL_HOST_USB_HEADER_T);
        *pos            = curFifoElem->Rid;

        // Read data.
        memcpy(pos+1,
               (VOID*)HAL_SYS_GET_UNCACHED_ADDR(curFifoElem->Addr),
               curFifoElem->Size);
        // Generate checksum
        for(i = 0, sum = 0;
                i < curFifoElem->Size+1;
                ++i)
        {
            sum ^= *pos;
            pos++;
        }

        // Xor with the flow Id (0xFF)
        *pos                               = ~sum;

        // Increment position in the send buffer.
        (*g_HalHostUsbVar)->PositionBufferIn += curFifoElem->Size+5+1;

        // Go to the next command.
        (*g_HalHostUsbVar)->ReadPt++;
    }

    HAL_PROFILE_FUNCTION_EXIT(hal_HostUsbGenerateReadPacket);
}


// ============================================================================
// boot_HostUsbSendReadPacket
// -----------------------------------------------------------------------------
/// Send packet generated by boot_HostUsbGenerateReadPacket and switch output
/// swap buffer
// =============================================================================
PRIVATE VOID hal_HostUsbSendReadPacket(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_HostUsbSendReadPacket);

    hal_UsbSend(1, (*g_HalHostUsbVar)->Context
                ->BufferIn[(*g_HalHostUsbVar)->CurrentBufferIn].Buffer,
                (*g_HalHostUsbVar)->PositionBufferIn, 0);
    // Use the other swap buffer ...
    (*g_HalHostUsbVar)->CurrentBufferIn++;
    (*g_HalHostUsbVar)->CurrentBufferIn  %= 2;

    // ... From the beginning of the buffer.
    (*g_HalHostUsbVar)->PositionBufferIn  = 0;
    HAL_PROFILE_FUNCTION_EXIT(hal_HostUsbSendReadPacket);
}


// ============================================================================
// hal_HostUsbInit
// -----------------------------------------------------------------------------
/// Init usb host. Clear FIFO and host variables
// =============================================================================
PUBLIC VOID hal_HostUsbInit(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_HostUsbInit);
    (*g_HalHostUsbVar)->State             = HAL_HOST_USB_STATE_IDLE;
    (*g_HalHostUsbVar)->ReadPt            = 0;
    (*g_HalHostUsbVar)->WritePt           = 0;
    (*g_HalHostUsbVar)->CurrentBufferIn   = 0;
    (*g_HalHostUsbVar)->PositionBufferIn  = 0;
    (*g_HalHostUsbVar)->Residue           = 0;
    (*g_HalHostUsbVar)->RecvState         = HAL_HOST_USB_RECV_STATE_RUN;
    hal_UsbEpEnableInterrupt(HAL_USB_EP_DIRECTION_IN (1), FALSE);
    hal_UsbEpEnableInterrupt(HAL_USB_EP_DIRECTION_OUT(1), FALSE);
    hal_UsbRecv(1, (*g_HalHostUsbVar)->Context->BufferOut,
                HAL_HOST_USB_BUFFER_OUT_SIZE, 0);

    HAL_PROFILE_FUNCTION_EXIT(hal_HostUsbInit);
}


PRIVATE CONST HAL_USB_EP_DESCRIPTOR_T hal_HostUsbEp1In =
{
    .ep        = HAL_USB_EP_DIRECTION_IN(1),
    .type      = HAL_USB_EP_TYPE_BULK,
    .interval  = 1,
    .callback  = 0
};


PRIVATE  CONST HAL_USB_EP_DESCRIPTOR_T hal_HostUsbEp1Out =
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

PRIVATE CONST HAL_USB_EP_DESCRIPTOR_CONST_PTR_T hal_HostUsbEpList[] =
{
    &hal_HostUsbEp1In,
    &hal_HostUsbEp1Out,
    0
};


PUBLIC CONST HAL_USB_INTERFACE_DESCRIPTOR_T g_halHostUsbInterface =
{
    .usbClass     = 0xFF,
    .usbSubClass  = 0x66,
    .usbProto     = 0x66,
    .interfaceIdx = 0x00,
    .epList       = (HAL_USB_EP_DESCRIPTOR_T**) hal_HostUsbEpList,
    .description  = (UINT8*)"RDA Usb",
    .callback     = hal_HostUsbCallback
};


// =============================================================================
// HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T
// -----------------------------------------------------------------------------
/// constante interface config
// =============================================================================
typedef CONST HAL_USB_INTERFACE_DESCRIPTOR_T *
HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T;

PRIVATE CONST HAL_USB_INTERFACE_DESCRIPTOR_CONST_PTR_T
hal_HostUsbInterfaceList[] =
{
    &g_halHostUsbInterface,
    0
};


PRIVATE CONST HAL_USB_CONFIG_DESCRIPTOR_T hal_HostUsbConfig =
{
    .attrib        =  0,
    .maxPower      = 50,
    .interfaceList = (HAL_USB_INTERFACE_DESCRIPTOR_T**)
    hal_HostUsbInterfaceList,
    .description   = (UINT8*)"RDA default",
    .configIdx     =  1,
};


// =============================================================================
// HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T
// -----------------------------------------------------------------------------
/// Constant usb config
// =============================================================================
typedef CONST HAL_USB_CONFIG_DESCRIPTOR_T *
HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T;


PRIVATE CONST HAL_USB_CONFIG_DESCRIPTOR_CONST_PTR_T hal_HostUsbConfigList[] =
{
    &hal_HostUsbConfig,
    0
};


PRIVATE CONST HAL_USB_DEVICE_DESCRIPTOR_T hal_HostUsbDesc =
{
    .usbClass     = 0,
    .usbSubClass  = 0,
    .usbProto     = 0,
    .vendor       = 0,
    .product      = 0,
    .version      = 0,
    .configList   = (HAL_USB_CONFIG_DESCRIPTOR_T**) hal_HostUsbConfigList,
    .description  = (UINT8*)"RDA",
    .serialNumber = (UINT8*)"Host USB"
};

// =============================================================================
// hal_HostUsbDeviceCallback
// -----------------------------------------------------------------------------
/// Callback use to define the host usb descriptor. It's call by USB driver
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* hal_HostUsbDeviceCallback(BOOL speedEnum)
{
    return (HAL_USB_DEVICE_DESCRIPTOR_T*) &hal_HostUsbDesc;
}


// ============================================================================
// hal_HostUsbEvent
// -----------------------------------------------------------------------------
/// Send event on usb host
///
/// @param event Event number, you want send
// =============================================================================
PUBLIC VOID hal_HostUsbEvent(UINT32 event)
{
    HAL_HOST_USB_HEADER_T*             header;
    UINT8                              trame[10];
    UINT32                             startTime = hwp_timer->HWTimer_CurVal;

    header = (HAL_HOST_USB_HEADER_T*) &trame[1];

    // Generate Event packet
    trame[0]               = 0xAD;
    header->size           = HAL_ENDIAN_BIG_16(0x06);
    header->flowId         = 0xFF;
    trame[4]               = 0;
    memcpy(&trame[5], &event, sizeof(UINT32));
    trame[9]               = ~(trame[4]^trame[5]^trame[6]^trame[7]^trame[8]);

    // Wait the last packet
    while(hal_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == FALSE)
    {
        if((hwp_timer->HWTimer_CurVal - startTime) >= HAL_USB_SEND_EVENT_DELAY)
        {
            HAL_PROFILE_PULSE(HAL_USB_MONITOR_EVENT_TIMEOUT);
            return;
        }
    }
    hal_UsbSend(1, trame, sizeof(trame), 0);
    while(hal_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == FALSE)
    {
        if((hwp_timer->HWTimer_CurVal - startTime) >= HAL_USB_SEND_EVENT_DELAY)
        {
            HAL_PROFILE_PULSE(HAL_USB_MONITOR_EVENT_TIMEOUT);
            break;
        }
    }
}


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// hal_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T* interfaceDesc = NULL;

    g_halHostUsbContext.MonitorMode = HAL_HOST_USB_MODE_SX;

    (*g_HalHostUsbVar)->Context = &g_halHostUsbContext;

    if(!hal_GdbIsInGdbLoop())
    {
        interfaceDesc = hal_UsbDescriptorCopyInterfaceDescriptor(&g_halHostUsbInterface);

#ifdef HAL_TRACE_FORCE_HOST
        g_HalHostUsbCallbackOs      = NULL;
#else
        g_HalHostUsbCallbackOs      = hal_TraceUsbCallback;
#endif // HAL_TRACE_FORCE_HOST
    }

    hal_HostUsbInit();

    return interfaceDesc;
}

// =============================================================================
// hal_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb
// =============================================================================
PUBLIC VOID hal_HostUsbClose(VOID)
{
    hal_SysUsbHostEnable(FALSE);
}


// ============================================================================
// hal_HostUsbSend
// -----------------------------------------------------------------------------
/// Poll the output data in host
///
/// @return \c TRUE If a transfer is in progress.
// =============================================================================
PUBLIC BOOL hal_HostUsbSend(VOID)
{
    if((*g_HalHostUsbVar)->ReadPt != (*g_HalHostUsbVar)->WritePt) // FIFO not empty
    {
        hal_HostUsbGenerateReadPacket();
    }

    // Previous packet send & new packet ready to be send ?
    if(hal_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == TRUE &&
            (*g_HalHostUsbVar)->PositionBufferIn != 0)
    {
        hal_HostUsbSendReadPacket();
    }

    if(hal_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_IN(1)) == FALSE)
    {
        return(TRUE);
    }
    return(FALSE);
}


// =============================================================================
// hal_HostUsbRecv
// -----------------------------------------------------------------------------
/// Receive USB packet and parse it
///
/// @return 1 if new data is receive
// =============================================================================
PUBLIC UINT8 hal_HostUsbRecv(VOID)
{
    UINT8  *data;
    UINT32  size;
    UINT8   fifoUse;
    UINT8   startNewReception   = 0;
    UINT8   exitStatus = 0;

    // Write Ptr and read Ptr go above fifo size,
    // but the modulo HAL_HOST_USB_FIFO_READ_SIZE is used
    // when the Ptr are accessed.
    // It is enforced somewhere else that write - read
    // is always within fifo range.
    fifoUse = (*g_HalHostUsbVar)->WritePt-(*g_HalHostUsbVar)->ReadPt;

    data = (UINT8*) HAL_SYS_GET_UNCACHED_ADDR(
               (*g_HalHostUsbVar)->Context->BufferOut);

    switch((*g_HalHostUsbVar)->RecvState)
    {
        case HAL_HOST_USB_RECV_STATE_RUN:
            if(hal_UsbEpTransfertDone(HAL_USB_EP_DIRECTION_OUT(1)))
            {
                // If a command is split on several packets, the beginning
                // is in the residue.
                size = hal_UsbEpRxTransferedSize(1)+(*g_HalHostUsbVar)->Residue;

                // Decode received packet.
                exitStatus = hal_HostUsbDecode(data, size);
                startNewReception       = 1;
            }
            break;

        // Fifo Full
        case HAL_HOST_USB_RECV_STATE_WAIT:
            if(fifoUse != HAL_HOST_USB_FIFO_READ_SIZE)
            {
                // hal_HostUsbDecode can change state to HAL_HOST_USB_RECV_STATE_RUN
                // Packet may remains to be decoded in the Fifo.
                exitStatus = hal_HostUsbDecode(data, (*g_HalHostUsbVar)->Residue);
                startNewReception       = 1;
            }
            break;
    }

    if((*g_HalHostUsbVar)->RecvState == HAL_HOST_USB_RECV_STATE_RUN &&
            startNewReception == 1)
    {
        // Fetch new usb packet
        hal_UsbRecv(1, &(*g_HalHostUsbVar)->Context->BufferOut
                    [(*g_HalHostUsbVar)->Residue],
                    HAL_HOST_USB_BUFFER_OUT_SIZE-(*g_HalHostUsbVar)->Residue, 0);
    }
    return(exitStatus);
}


#else // !HAL_USB_DRIVER_DONT_USE_ROMED_CODE

#include "boot_usb_monitor.h"
#include "hal_usb.h"
#include "hal_sys.h"
#include "boot_host.h"
#include "hal_timers.h"
#include "boot_sector_driver.h"
#include "hal_usb_descriptor.h"
#include "hal_trace.h"
#include "halp_sys.h"
#include "halp_gdb_stub.h"


PRIVATE BOOT_HOST_USB_MONITOR_CTX_T g_halHostUsbContext;

/// This function overload the rom usb function. This implementation add call to
/// enable usb host in hal with function hal_SysUsbHostEnable
PUBLIC HAL_USB_CALLBACK_RETURN_T
hal_HostUsbCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
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
            hal_SysUsbHostEnable(TRUE);
            hal_HstTraceSwitch(HAL_HST_TRACE_PORT_USB);
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            hal_SysUsbHostEnable(FALSE);
            hal_HstTraceSwitch(HAL_HST_TRACE_PORT_HOST);
            break;
    }
    return(HAL_USB_CALLBACK_RETURN_KO);
}


/// Function use to implemente H2PSTATUS command
PUBLIC VOID hal_HostUsbH2pStatusCallback(UINT32 value)
{
    switch(value)
    {
        case BOOT_HST_MONITOR_ENTER:
            // To restart in the Boot Monitor, by going through
            // the Boot Sector  and we power cycle the USB.
            boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);
            hal_SysRestart();
            break;

        case BOOT_HST_MONITOR_X_CMD:
            boot_HostUsbEvent(BOOT_HST_START_CMD_TREATMENT);
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

PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T* interfaceDesc = NULL;

    g_halHostUsbContext.MonitorMode = BOOT_HOST_USB_MODE_SX;

    if(!hal_GdbIsInGdbLoop())
    {
        interfaceDesc = hal_UsbDescriptorCopyInterfaceDescriptor(&boot_HostUsbInterface);
        interfaceDesc->callback = hal_HostUsbCallback;
    }

    // FIXME: check this
#ifdef HAL_TRACE_FORCE_HOST
#define HAL_TRACEUSBCALLBACK NULL
#else
#define HAL_TRACEUSBCALLBACK hal_TraceUsbCallback
#endif
    boot_HostUsbOpen((BOOT_HOST_USB_MONITOR_CTX_T*)&g_halHostUsbContext,
                     HAL_TRACEUSBCALLBACK,
                     hal_HostUsbH2pStatusCallback);
    return interfaceDesc;
}

// =============================================================================
// hal_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb
// =============================================================================
PUBLIC VOID hal_HostUsbClose(VOID)
{
    hal_SysUsbHostEnable(FALSE);
}

#endif //! HAL_USB_DRIVER_DONT_USE_ROMED_CODE

#endif // CHIP_HAS_USB == 1

