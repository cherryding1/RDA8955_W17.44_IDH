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
#include "usbc.h"
#include "cfg_regs.h"
#include "sys_ctrl.h"

#include "bootp_version.h"
#include "bootp_debug.h"
#include "bootp_usb.h"
#include "boot_usb.h"

#include "hal_usb.h"
#include "hal_sys.h"

#include <string.h>


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PROTECTED BOOT_USB_GLOBAL_VAR_T g_BootUsbVar __attribute__((section (".boot_usb_ep_ptr")));

// Ep0 Buffer
PRIVATE UINT8  ALIGNED(4) g_BootUsbBufferEp0Out[HAL_USB_MPS] __attribute__((section (".boot_usb_ep_ptr")));
PRIVATE UINT8  ALIGNED(4) g_BootUsbBufferEp0In [HAL_USB_MPS] __attribute__((section (".boot_usb_ep_ptr")));

PRIVATE CONST BOOT_USB_GLOBAL_VAR_T CONST *
__attribute__((section (".usbFixptr"),used))
g_BootUsbFixPtr = &g_BootUsbVar;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS PROTOTYPE
// =============================================================================

PRIVATE        VOID boot_UsbConfigureEp(HAL_USB_EP_DESCRIPTOR_T* ep);
PRIVATE        VOID boot_UsbDecodeEp0Packet(VOID);
PRIVATE        VOID boot_getSetupPacket(VOID);
PRIVATE        VOID boot_UsbEnableEp(UINT8 ep, HAL_USB_EP_TYPE_T type);
PRIVATE        VOID boot_UsbDisableEp(UINT8 ep);
PRIVATE        VOID boot_UsbClrConfig(VOID);
INLINE         VOID boot_UsbStatusIn(VOID);
INLINE         VOID boot_UsbStatusOut(VOID);
INLINE         HAL_USB_CALLBACK_RETURN_T
boot_UsbCallbackEp(UINT8                       ep,
                   HAL_USB_CALLBACK_EP_TYPE_T  type,
                   HAL_USB_SETUP_T*            setup);
INLINE         HAL_USB_CALLBACK_RETURN_T
boot_UsbCallbackInterface(UINT8                       ep,
                          HAL_USB_CALLBACK_EP_TYPE_T  type,
                          HAL_USB_SETUP_T*            setup);
PRIVATE UINT16 boot_UsbGenerateDescConfig(HAL_USB_CONFIG_DESCRIPTOR_T* cfg,
        UINT8*                       buffer,
        UINT8                        num);
PRIVATE UINT16
boot_UsbGenerateDescEp(HAL_USB_EP_DESCRIPTOR_T* ep,
                       UINT8*                   buffer);
PRIVATE UINT16
boot_UsbGenerateDescInterface(HAL_USB_INTERFACE_DESCRIPTOR_T* interface,
                              UINT8*                          buffer,
                              UINT8                           num);

// =============================================================================
// boot_UsbFlushRxFifo
// -----------------------------------------------------------------------------
/// Flux reception USB fifo
// =============================================================================
PRIVATE VOID boot_UsbFlushRxFifo(VOID);

// =============================================================================
// boot_UsbFlushTxFifo
// -----------------------------------------------------------------------------
/// Flush transmition USB FIFO
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
// =============================================================================
PRIVATE VOID boot_UsbFlushTxFifo(UINT8 ep);

// =============================================================================
// boot_UsbFlushAllTxFifos
// -----------------------------------------------------------------------------
/// Flush all transmition USB FIFOS
// =============================================================================
PRIVATE VOID boot_UsbFlushAllTxFifos(VOID);

// =============================================================================
// boot_UsbAsciiToUtf8
// -----------------------------------------------------------------------------
/// Convert string to utf8
/// @param utf8 destination string
/// @param acsii source string
// =============================================================================
PRIVATE VOID boot_UsbAsciiToUtf8(UINT8 *utf8, UINT8 *acsii);

// =============================================================================
// PRIVATE FUNCTION
// =============================================================================

PRIVATE VOID boot_UsbAsciiToUtf8(UINT8 *utf8, UINT8 *acsii)
{
    while(*acsii)
    {
        *utf8 = *acsii;
        utf8++;
        *utf8 = 0;
        utf8++;
        acsii++;
    }
}

PRIVATE VOID boot_UsbFlushRxFifo(VOID)
{
    hwp_usbc->DCTL    |= USBC_SGOUTNAK;
    while(!(hwp_usbc->DCTL & USBC_GOUTNAKSTS));

    hwp_usbc->GRSTCTL |= USBC_RXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGOUTNAK;
}

PRIVATE VOID boot_UsbFlushTxFifo(UINT8 ep)
{
    UINT8 epNum;

    epNum              = HAL_USB_EP_NUM(ep);

    hwp_usbc->DCTL    |= USBC_SGNPINNAK;
    while(!(hwp_usbc->DCTL & USBC_GNPINNAKSTS));

    hwp_usbc->GRSTCTL  = USBC_TXFNUM(epNum) | USBC_TXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGNPINNAK;
}

PRIVATE VOID boot_UsbFlushAllTxFifos(VOID)
{
    hwp_usbc->DCTL    |= USBC_SGNPINNAK;
    while(!(hwp_usbc->DCTL & USBC_GNPINNAKSTS));

    hwp_usbc->GRSTCTL  =  USBC_TXFNUM(0x10) | USBC_TXFFLSH;

    hwp_usbc->DCTL    |= USBC_CGNPINNAK;
}

PROTECTED VOID boot_UsbResetTransfert(VOID)
{
    UINT8 i;

    for(i = 0; i < DIEP_NUM+1; ++i)
    {
        g_BootUsbVar.InTransfert[i].size  = 0;
    }
    for(i = 0; i < DOEP_NUM+1; ++i)
    {
        g_BootUsbVar.OutTransfert[i].size = 0;
    }
}


// =============================================================================
//
// -----------------------------------------------------------------------------
///
// =============================================================================
PROTECTED UINT8 boot_UsbStartTransfert(UINT8 ep, VOID *data, UINT16 size,
                                       UINT32 flag)
{
    UINT8               epNum;
    UINT16              nbPacket;
    REG32*              regDma;
    REG32*              regSize;
    REG32*              regCtl;

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        // In (Device->PC)
        if(g_BootUsbVar.InTransfert[epNum].size > 0)
        {
            return(1);
        }

        // Select DMA register for the EP
        if(epNum == 0)
        {
            regDma  = &hwp_usbc->DIEPDMA0;
            regSize = &hwp_usbc->DIEPTSIZ0;
            regCtl  = &hwp_usbc->DIEPCTL0;
        }
        else
        {
            regDma  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPDMA;
            regSize = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPTSIZ;
            regCtl  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        }

        g_BootUsbVar.EpFlag &= ~(1<<epNum);
        if(flag)
        {
            g_BootUsbVar.EpFlag |= 1<<epNum;
        }

        // One packet ?
        if(size < HAL_USB_MPS)
        {
            // Set size of DMA transfert
            *regSize  = USBC_IEPXFERSIZE(size) | USBC_IEPPKTCNT(1);
        }
        else
        {
            nbPacket  = size/HAL_USB_MPS;

            // Set size of DMA transfert
            *regSize  = USBC_IEPXFERSIZE(nbPacket*HAL_USB_MPS) |
                        USBC_IEPPKTCNT(nbPacket);
        }
        g_BootUsbVar.InTransfert[epNum].size = size;
    }
    else
    {
        // Out (PC->device)
        if(g_BootUsbVar.OutTransfert[epNum].size > 0)
        {
            return(1);
        }
        g_BootUsbVar.OutTransfert[epNum].size = size;

        // Select DMA register for the EP
        if(epNum == 0)
        {
            regDma  = &hwp_usbc->DOEPDMA0;
            regSize = &hwp_usbc->DOEPTSIZ0;
            regCtl  = &hwp_usbc->DOEPCTL0;
        }
        else
        {
            regDma  = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPDMA;
            regSize = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPTSIZ;
            regCtl  = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        }

        // Set size of DMA transfert
        *regSize = USBC_OEPXFERSIZE(size) | USBC_OEPPKTCNT(size/HAL_USB_MPS);
    }

    // Enable DMA transfert
    *regDma  = USB_DMA_SRC_ADDRESS((REG32) data);
    *regCtl |= USBC_EPENA | USBC_CNAK;

    return(0);
}


// =============================================================================
//
// -----------------------------------------------------------------------------
///
// =============================================================================
PROTECTED VOID boot_UsbCancelTransfert(UINT8 ep)
{
    UINT8               epNum;

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        g_BootUsbVar.InTransfert[epNum].size  = 0;
    }
    else
    {
        g_BootUsbVar.OutTransfert[epNum].size = 0;
    }
}

PROTECTED UINT8 boot_UsbContinueTransfert(UINT8 ep)
{
    UINT8               epNum;
    REG32*              regSize;
    REG32*              regCtl;

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        // In (Device->PC)
        if(g_BootUsbVar.InTransfert[epNum].size <= 0)
        {
            // End of transfert
            return(1);
        }


        if(!(g_BootUsbVar.EpFlag & (1<<epNum)))
        {
            // If is not a single packet
            if(g_BootUsbVar.InTransfert[epNum].size >= HAL_USB_MPS)
            {
                // Select DMA register for the EP
                if(epNum == 0)
                {
                    regSize = &hwp_usbc->DIEPTSIZ0;
                    regCtl  = &hwp_usbc->DIEPCTL0;
                }
                else
                {
                    regSize = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPTSIZ;
                    regCtl  = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
                }

                // Set size and enable DMA
                *regSize  = USBC_IEPXFERSIZE(g_BootUsbVar.InTransfert[epNum].size
                                             % HAL_USB_MPS) | USBC_IEPPKTCNT(1);
                *regCtl  |= USBC_EPENA | USBC_CNAK;

                g_BootUsbVar.InTransfert[epNum].size = 1;
                return(0);
            }
        }
        g_BootUsbVar.InTransfert[epNum].size  = 0;
    }
    else
    {
        // Out (PC->device)
        if(g_BootUsbVar.OutTransfert[epNum].size <= 0)
        {
            // End of transfert
            return(1);
        }

        // Finalize transfert

        // Select DMA register for the EP
        if(epNum == 0)
        {
            regSize = &hwp_usbc->DOEPTSIZ0;
        }
        else
        {
            regSize = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPTSIZ;
        }

        // g_BootUsbVar.OutTransfert[epNum].size = - nbDate receive
        g_BootUsbVar.OutTransfert[epNum].size -=
            GET_BITFIELD(*regSize, USBC_OEPXFERSIZE);
        g_BootUsbVar.OutTransfert[epNum].size  =
            -g_BootUsbVar.OutTransfert[epNum].size;

        if(g_BootUsbVar.OutTransfert[epNum].size > 0)
        {
            g_BootUsbVar.OutTransfert[epNum].size = 0;
        }
    }

    return(1);
}


PRIVATE VOID boot_UsbConfigureEp(HAL_USB_EP_DESCRIPTOR_T* ep)
{
    UINT8 epNum;

    epNum = HAL_USB_EP_NUM(ep->ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep->ep))
    {
        g_BootUsbVar.EpInCallback[epNum-1]  = ep->callback;
        boot_UsbEnableEp(HAL_USB_EP_DIRECTION_IN(epNum), ep->type);
    }
    else
    {
        g_BootUsbVar.EpOutCallback[epNum-1] = ep->callback;
        boot_UsbEnableEp(HAL_USB_EP_DIRECTION_OUT(epNum), ep->type);
    }
}

// =============================================================================
// boot_UsbInit
// -----------------------------------------------------------------------------
/// (Re)Initializes the USB state machine, clear the IRQ and stop the transfers.
// =============================================================================
PRIVATE VOID boot_UsbInit(VOID)
{
    // Flush fifo
    boot_UsbFlushAllTxFifos();
    boot_UsbFlushRxFifo();

    // Reset EP0 state
    g_BootUsbVar.Ep0State        = EP0_STATE_IDLE;
    g_BootUsbVar.NbString        = 0;

    boot_UsbClrConfig();
    g_BootUsbVar.Desc            = 0;

    boot_UsbResetTransfert();
}

PRIVATE VOID boot_UsbConfig(VOID)
{
    UINT8                i;
    UINT16               addr;
    UINT8                dir;

    // Nb EP
    g_BootUsbVar.NbEp = GET_BITFIELD(hwp_usbc->GHWCFG2, USBC_NUMDEVEPS);
    g_BootUsbVar.NbEp++;

    // Rx Fifo Size
    hwp_usbc->GRXFSIZ    = RXFIFOSIZE;
    addr                 = RXFIFOSIZE;

    // EP direction and Tx fifo configuration
    hwp_usbc->GNPTXFSIZ  = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(TXFIFOSIZE);
    addr                += TXFIFOSIZE;

    for(i = 1; i < g_BootUsbVar.NbEp; i++)
    {
        dir = EPDIR(i);
        if(dir == EPIN || dir == EPINOUT)
        {
            hwp_usbc->DIEPTXF[i-1].DIEnPTXF  =
                USBC_IENPNTXFSTADDR(addr) | USBC_INEPNTXFDEP(TXFIFOSIZE);
            addr                            += TXFIFOSIZE;
        }
    }

    // Enable DMA mode
    hwp_usbc->GAHBCFG  = USBC_DMAEN | USBC_HBSTLEN(BOOT_USB_AHB_MODE);
    hwp_usbc->GAHBCFG |= USBC_GLBLINTRMSK;
    hwp_usbc->GUSBCFG |= USBC_PHYIF | USBC_USBTRDTIM(5);

    hwp_usbc->DCFG    &= ~(USBC_DEVSPD_MASK  | USBC_PERFRINT_MASK);
    // Configure FS USB 1.1 Phy
    hwp_usbc->DCFG    |= USBC_DEVSPD(3);

    hwp_usbc->GINTMSK |= USBC_USBRST | USBC_ENUMDONE
                         | USBC_ERLYSUSP | USBC_USBSUSP;
}



// =============================================================================
// boot_UsbDisableEp
// -----------------------------------------------------------------------------
/// Disable an endpoint.
///
/// Please note that there is a particular behaviour for endpoint 0.
/// @param ep Endpoint to disable.
// =============================================================================
PRIVATE VOID boot_UsbDisableEp(UINT8 ep)
{
    UINT8   epNum;
    UINT8   offset;
    REG32*  diepctl;
    REG32*  doepctl;
    REG32*  ctl;
    REG32*  diepint;
    REG32*  doepint;
    REG32*  epint;

    epNum = HAL_USB_EP_NUM(ep);

    // Select ctl register
    if(epNum == 0)
    {
        diepctl = &hwp_usbc->DIEPCTL0;
        doepctl = &hwp_usbc->DOEPCTL0;
        diepint = &hwp_usbc->DIEPINT0;
        doepint = &hwp_usbc->DOEPINT0;
    }
    else
    {
        diepctl = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        diepint = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        doepint = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset = 0;
        ctl    = diepctl;
        epint  = diepint;
    }
    else
    {
        offset = 16;
        ctl    = doepctl;
        epint  = doepint;
    }

    // Clear the interruption cause, as we don't care
    // about the past anymore.
    if (epNum == 0)
    {
        // EP0 must always be enabled, so we don't stop
        // its transfer.
        if(*ctl & USBC_EPENA)
        {
            *ctl   = USBC_EPDIS | USBC_SNAK;
        }
        else
        {
            // set nake on EP
            *ctl   = USBC_SNAK;
        }
        *epint = USBC_XFERCOMPL | USBC_SETUPMK | USBC_TIMEOUT;
    }
    else
    {
        if(*ctl & USBC_EPENA)
        {
            // Stop current transfert
            if(HAL_USB_IS_EP_DIRECTION_IN(ep))
            {
                while(!(*diepint & USBC_INEPNAKEFF))
                {
                    *diepctl |= USBC_SNAK;
                }
            }
            else
            {
                while(!(hwp_usbc->GINTSTS & USBC_GOUTNAKEFF))
                {
                    hwp_usbc->DCTL  |= USBC_SGOUTNAK;
                }
            }
            *ctl             = USBC_EPDIS | USBC_SNAK;
            while(!(*epint & USBC_EPDISBLD));
            *epint           = USBC_EPDISBLD;
            if(HAL_USB_IS_EP_DIRECTION_IN(ep))
            {
                boot_UsbFlushTxFifo(epNum);
            }
            else
            {
                hwp_usbc->DCTL  |= USBC_CGOUTNAK;
            }
        }
        else
        {
            // set nake on EP
            *ctl             = USBC_SNAK;
        }
    }

    boot_UsbFlushTxFifo(epNum);

    boot_UsbCancelTransfert(ep);

    // We don't disable EP0 interrupt as it is driving the USB
    // stuff. Closing the USB will disable them though.
    if (epNum != 0)
    {
        // Disable interrupt mask for this EP
        hwp_usbc->DAINTMSK &= ~(1<<(epNum+offset));
        g_BootUsbVar.EpInterruptMask &= ~(1<<(epNum+offset));
    }
}

PRIVATE VOID boot_UsbEnableEp(UINT8 ep, HAL_USB_EP_TYPE_T type)
{
    UINT8   epNum;
    UINT8   offset;
    REG32*  diepctl;
    REG32*  doepctl;
    REG32*  ctl;
    REG32*  diepint;
    REG32*  doepint;
    REG32*  epint;

    epNum = HAL_USB_EP_NUM(ep);

    // Select ctl register
    if(epNum == 0)
    {
        diepctl = &hwp_usbc->DIEPCTL0;
        doepctl = &hwp_usbc->DOEPCTL0;
        diepint = &hwp_usbc->DIEPINT0;
        doepint = &hwp_usbc->DOEPINT0;
    }
    else
    {
        diepctl = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
        diepint = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        doepint = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset = 0;
        ctl    = diepctl;
        epint  = diepint;
    }
    else
    {
        offset = 16;
        ctl    = doepctl;
        epint  = doepint;
    }

    // Clear pending interrupt
    *epint = *epint;

    /*     if(*ctl & USBC_USBACTEP) */
    /*     { */
    if(type == HAL_USB_EP_TYPE_ISO)
    {
        *ctl = USBC_EPN_MPS(HAL_USB_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
               | USBC_EPTXFNUM(epNum) | USBC_NEXTEP(epNum);
    }
    else
    {
        *ctl = USBC_EPN_MPS(HAL_USB_MPS) | USBC_EPTYPE(type) | USBC_USBACTEP
               | USBC_EPTXFNUM(epNum) | USBC_SETD0PID | USBC_NEXTEP(epNum);
    }
    /*     } */

    // Enable interrupt mask for this EP
    hwp_usbc->DAINTMSK |= (1<<(epNum+offset));
    g_BootUsbVar.EpInterruptMask |= (1<<(epNum+offset));
}

INLINE         UINT8 boot_UsbAddString(UINT8 *str)
{
    UINT8 i;

    if(str == 0)
    {
        return(0);
    }
    for(i = 0; i < g_BootUsbVar.NbString; ++i)
    {
        if(g_BootUsbVar.String[i] == str)
        {
            return(i+1);
        }
    }
    g_BootUsbVar.String[g_BootUsbVar.NbString] = str;
    g_BootUsbVar.NbString++;
    return(g_BootUsbVar.NbString);
}


PRIVATE VOID boot_UsbGenerateDescDevice(HAL_USB_DEVICE_DESCRIPTOR_T* dev,
                                        UINT8*                       buffer)
{
    BOOT_USB_DEVICE_DESCRIPTOR_REAL_T* desc;
    UINT8                             i;

    desc = (BOOT_USB_DEVICE_DESCRIPTOR_REAL_T*) buffer;
    desc->size          = sizeof(BOOT_USB_DEVICE_DESCRIPTOR_REAL_T);
    // Device type = 1
    desc->type          = 1;
    desc->bcdUsb        = 0x0110;
    desc->usbClass      = dev->usbClass;
    desc->usbSubClass   = dev->usbSubClass;
    desc->usbProto      = dev->usbProto;
    desc->ep0Mps        = HAL_USB_MPS;
    if(dev->vendor == 0)
    {
        desc->vendor    = BOOT_USB_VENDOR_ID;
    }
    else
    {
        desc->vendor    = dev->vendor;
    }
    if(dev->product == 0)
    {
        desc->product   = BOOT_USB_PRODUCT_ID(dev->usbMode);
    }
    else
    {
        desc->product   = dev->product;
    }
    if(dev->version == 0)
    {
        desc->release   =  (UINT16)BOOT_USB_REVISION_ID;
    }
    else
    {
        desc->release   = dev->version;
    }
    desc->iManufacturer = boot_UsbAddString((UINT8*)"Coolsand Technologies");
    desc->iProduct      = boot_UsbAddString(dev->description);
    desc->iSerial       = boot_UsbAddString(dev->serialNumber);
    i = 0;
    if(dev->configList)
    {
        for(i = 0; dev->configList[i]; ++i);
    }
    desc->nbConfig          = i;
}

PRIVATE UINT16 boot_UsbGenerateDescConfig(HAL_USB_CONFIG_DESCRIPTOR_T* cfg,
        UINT8*                       buffer,
        UINT8                        num)
{
    BOOT_USB_CONFIG_DESCRIPTOR_REAL_T* desc;
    UINT16                             size;
    UINT8                              i;

    desc = (BOOT_USB_CONFIG_DESCRIPTOR_REAL_T*) buffer;
    desc->size         = sizeof(BOOT_USB_CONFIG_DESCRIPTOR_REAL_T);
    size               = sizeof(BOOT_USB_CONFIG_DESCRIPTOR_REAL_T);
    // Config type = 2
    desc->type         = 2;

    i = 0;
    if(cfg->interfaceList)
    {
        for(i = 0; cfg->interfaceList[i]; ++i)
        {
            size += boot_UsbGenerateDescInterface(cfg->interfaceList[i],
                                                  &buffer[size], i);
        }
    }
    desc->nbInterface  = i;
    desc->configIndex  = cfg->configIdx;
    desc->iDescription = boot_UsbAddString(cfg->description);
    desc->attrib       = cfg->attrib;
    desc->maxPower     = cfg->maxPower;
    desc->totalLength  = size;
    return(size);
}

PRIVATE UINT16
boot_UsbGenerateDescInterface(HAL_USB_INTERFACE_DESCRIPTOR_T* interface,
                              UINT8*                          buffer,
                              UINT8                           num)
{
    UINT8                                 i;
    BOOT_USB_INTERFACE_DESCRIPTOR_REAL_T* desc;
    UINT16                                size;

    desc = (BOOT_USB_INTERFACE_DESCRIPTOR_REAL_T*) buffer;

    desc->size              = sizeof(BOOT_USB_INTERFACE_DESCRIPTOR_REAL_T);
    size                    = sizeof(BOOT_USB_INTERFACE_DESCRIPTOR_REAL_T);
    // Interface type = 4
    desc->type              = 4;
    desc->interfaceIndex    = num;
    if(interface->interfaceIdx)
    {
        desc->interfaceIndex = interface->interfaceIdx;
    }
    desc->alternateSetting  = 0;
    i = 0;
    if(interface->epList)
    {
        for(i = 0; interface->epList[i]; ++i)
        {
            size           += boot_UsbGenerateDescEp(interface->epList[i],
                              &buffer[size]);
        }
    }
    desc->nbEp              = i;
    desc->usbClass          = interface->usbClass   ;
    desc->usbSubClass       = interface->usbSubClass;
    desc->usbProto          = interface->usbProto   ;
    desc->iDescription      = boot_UsbAddString(interface->description);

    return(size);
}

PRIVATE UINT16
boot_UsbGenerateDescEp(HAL_USB_EP_DESCRIPTOR_T* ep,
                       UINT8*                   buffer)
{
    BOOT_USB_EP_DESCRIPTOR_REAL_T* desc;

    desc = (BOOT_USB_EP_DESCRIPTOR_REAL_T*) buffer;

    desc->size       = sizeof(BOOT_USB_EP_DESCRIPTOR_REAL_T);
    // EP type = 5
    desc->type       = 5;
    desc->ep         = ep->ep;
    // EP Type (Cmd, Bulk, Iso, Int)
    desc->attributes = ep->type;
    desc->mps        = HAL_USB_MPS;
    desc->interval   = ep->interval;

    return(desc->size);
}

PRIVATE VOID boot_getSetupPacket(VOID)
{
    // Enable EP0 to receive a new setup packet
    hwp_usbc->DOEPTSIZ0  =  USBC_SUPCNT(3) |
                            USBC_OEPXFERSIZE0(8) | USBC_OEPPKTCNT0;
    hwp_usbc->DOEPDMA0   =  USB_DMA_SRC_ADDRESS((UINT32) g_BootUsbBufferEp0Out);
    hwp_usbc->DOEPCTL0  |= USBC_CNAK | USBC_EPENA;
}

INLINE         VOID boot_UsbStatusIn(VOID)
{
    // Enable EP0 to receive a status IN
    BOOT_PROFILE_PULSE(BOOT_USB_STATUS_IN);
    g_BootUsbVar.Ep0State = EP0_STATE_STATUS_IN;
    boot_UsbSend(0, g_BootUsbBufferEp0In, 0, 0);
    boot_getSetupPacket();
}

INLINE         VOID boot_UsbStatusOut(VOID)
{
    // Enable EP0 to receive a status OUT
    /* NEED use hwp_usbc->DCFG |= USBC_NZSTSOUTHSHK; for set negative status */
    BOOT_PROFILE_PULSE(BOOT_USB_STATUS_OUT);
    g_BootUsbVar.Ep0State = EP0_STATE_STATUS_OUT;
    boot_UsbRecv(0, g_BootUsbBufferEp0Out, 0, 0);
}


// =============================================================================
// boot_UsbClrConfig
// -----------------------------------------------------------------------------
/// Clear the USB config. Stop the transfer on the endpoint (but ep0) and clear
/// the interrupts (but ep0).
// =============================================================================
PRIVATE VOID boot_UsbClrConfig(VOID)
{
    UINT8 i;

    if(g_BootUsbVar.Desc)
    {
        if(g_BootUsbVar.Config != 0xFF  &&
                g_BootUsbVar.Desc->configList[g_BootUsbVar.Config] &&
                g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]->interfaceList)
        {
            for(i = 0;
                    g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]
                    ->interfaceList[i];
                    ++i)
            {
                if(g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]
                        ->interfaceList[i]->callback)
                {
                    g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]
                    ->interfaceList[i]->callback
                    (HAL_USB_CALLBACK_TYPE_DISABLE, 0);
                }
            }
        }

        // Ask lilian why he did that at revision 15516 in hal_usb.c.
        g_BootUsbVar.Config = 0xFF;
    }

    // Disable all EP
    for(i = 0; i < DIEP_NUM; ++i)
    {
        g_BootUsbVar.EpInCallback[i]  = 0;
        boot_UsbDisableEp(HAL_USB_EP_DIRECTION_IN(i+1));
    }
    for(i = 0; i < DOEP_NUM; ++i)
    {
        g_BootUsbVar.EpOutCallback[i] = 0;
        boot_UsbDisableEp(HAL_USB_EP_DIRECTION_OUT(i+1));
    }

    // Endpoint 0 doesn't have endpoint callbacks other
    // than the IRQ handlers...
    boot_UsbDisableEp(HAL_USB_EP_DIRECTION_IN(0));
    boot_UsbDisableEp(HAL_USB_EP_DIRECTION_OUT(0));
}

INLINE         HAL_USB_CALLBACK_RETURN_T
boot_UsbCallbackEp(UINT8                      ep,
                   HAL_USB_CALLBACK_EP_TYPE_T type,
                   HAL_USB_SETUP_T*           setup)
{
    UINT8                     epNum;

    epNum = HAL_USB_EP_NUM(ep);

    if(epNum < 16 &&
            epNum != 0)
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            if(g_BootUsbVar.EpInCallback[epNum-1])
            {
                return(g_BootUsbVar.EpInCallback[epNum-1](type, setup));
            }
        }
        else
        {
            if(g_BootUsbVar.EpOutCallback[epNum-1])
            {
                return(g_BootUsbVar.EpOutCallback[epNum-1](type, setup));
            }
        }
    }
    return(HAL_USB_CALLBACK_RETURN_KO);
}

INLINE         HAL_USB_CALLBACK_RETURN_T
boot_UsbCallbackInterface(UINT8 interface,
                          HAL_USB_CALLBACK_EP_TYPE_T type,
                          HAL_USB_SETUP_T*           setup)
{
    if(g_BootUsbVar.Desc == 0)
    {
        return(HAL_USB_CALLBACK_RETURN_KO);
    }
    if(g_BootUsbVar.Config == 0xFF)
    {
        return(HAL_USB_CALLBACK_RETURN_KO);
    }
    if(g_BootUsbVar.Desc->configList[g_BootUsbVar.Config] == 0)
    {
        return(HAL_USB_CALLBACK_RETURN_KO);
    }
    if(interface >= g_BootUsbVar.NbInterface)
    {
        return(HAL_USB_CALLBACK_RETURN_KO);
    }
    if(g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]->
            interfaceList[interface]->callback == 0)
    {
        return(HAL_USB_CALLBACK_RETURN_KO);
    }
    return(g_BootUsbVar.Desc->configList[g_BootUsbVar.Config]->
           interfaceList[interface]->
           callback(type, setup));
}

PRIVATE VOID boot_UsbSetConfig(UINT8 num)
{
    UINT8 i = 0;
    UINT8 j;

    // Disable old config
    boot_UsbClrConfig();

    g_BootUsbVar.Config      = 0xFF;
    g_BootUsbVar.NbInterface = 0;

    // Search config index
    for(i = 0; g_BootUsbVar.Desc->configList[i]; ++i)
    {
        if(g_BootUsbVar.Desc->configList[i]->configIdx == num)
        {
            break;
        }
    }
    if(g_BootUsbVar.Desc->configList[i] == 0)
    {
        return;
    }
    // Save config index
    g_BootUsbVar.Config = i;
    num                = i;

    // Configure all interface end EP of this config
    if(g_BootUsbVar.Desc->configList[num]->interfaceList)
    {
        for(i = 0; g_BootUsbVar.Desc->configList[num]->interfaceList[i]; ++i)
        {
            for(j = 0;
                    g_BootUsbVar.Desc->configList[num]->interfaceList[i]->epList[j];
                    ++j)
            {
                boot_UsbConfigureEp(g_BootUsbVar.Desc->configList[num]->
                                    interfaceList[i]->epList[j]);
            }
            if(g_BootUsbVar.Desc->configList[num]->interfaceList[i]->callback)
            {
                g_BootUsbVar.Desc->configList[num]->
                interfaceList[i]->callback(HAL_USB_CALLBACK_TYPE_ENABLE, 0);
            }
        }
        g_BootUsbVar.NbInterface = i;
    }
}

PRIVATE VOID boot_UsbDecodeEp0Packet(VOID)
{
    HAL_USB_SETUP_T* setup;
    UINT16           size;
    UINT8            setup_completed = 0;

    setup =
        (HAL_USB_SETUP_T*) HAL_SYS_GET_UNCACHED_ADDR(g_BootUsbBufferEp0Out);

    switch(g_BootUsbVar.Ep0State)
    {
        case EP0_STATE_IDLE  :
            // New setup packet

            // Change endian less
            setup->value     = HAL_ENDIAN_LITTLE_16(setup->value );
            setup->index     = HAL_ENDIAN_LITTLE_16(setup->index );
            setup->lenght    = HAL_ENDIAN_LITTLE_16(setup->lenght);

            g_BootUsbVar.RequestDesc = setup->requestDesc;
            g_BootUsbVar.Ep0Index    = setup->index&0xff;

            if(setup->lenght == 0)
            {
                // No data
                if(setup->requestDesc.requestDirection)
                {
                    g_BootUsbVar.Ep0State = EP0_STATE_STATUS_OUT;
                }
                else
                {
                    g_BootUsbVar.Ep0State = EP0_STATE_STATUS_IN;
                }
            }
            else
            {
                // Command with data
                if(setup->requestDesc.requestDirection)
                {
                    g_BootUsbVar.Ep0State = EP0_STATE_IN;
                }
                else
                {
                    g_BootUsbVar.Ep0State = EP0_STATE_OUT;
                }
            }

            switch(setup->requestDesc.requestDest)
            {
                case BOOT_USB_REQUEST_DESTINATION_DEVICE:
                    switch(setup->request)
                    {
                        case BOOT_USB_REQUEST_DEVICE_SETADDR:
                            BOOT_PROFILE_PULSE(BOOT_USB_REQUEST_DEVICE_SETADDR);
                            // Set device addr
                            hwp_usbc->DCFG |= USBC_DEVADDR(setup->value);
                            setup_completed = 1;
                            break;
                        case BOOT_USB_REQUEST_DEVICE_SETCONF:
                            // Select one config
                            if(g_BootUsbVar.Desc == 0)
                            {
                                break;
                            }


                            setup_completed = 1;
                            boot_UsbSetConfig(setup->value&0xFF);

                            break;
                        case BOOT_USB_REQUEST_DEVICE_GETDESC:
                            // Fetch device descriptor

                            if(g_BootUsbVar.Desc == 0)
                            {
                                break;
                            }
                            size = 0;
                            switch(setup->value>>8)
                            {
                                case 1: /* Device */
                                    boot_UsbGenerateDescDevice(g_BootUsbVar.Desc,
                                                               g_BootUsbBufferEp0In);
                                    size = sizeof(BOOT_USB_DEVICE_DESCRIPTOR_REAL_T);
                                    break;
                                case 2: /* Config */
                                    size = boot_UsbGenerateDescConfig(
                                               g_BootUsbVar.Desc->configList[(setup->value&0xFF)],
                                               g_BootUsbBufferEp0In, setup->value&0xFF);
                                    break;
                                case 3: /* String */
                                    if((setup->value&0xFF) == 0)
                                    {
                                        // Set caracter table to 0x0409
                                        size                    = 0x04;
                                        g_BootUsbBufferEp0In[0] = 0x04;
                                        g_BootUsbBufferEp0In[1] = 0x03;
                                        g_BootUsbBufferEp0In[2] = 0x09;
                                        g_BootUsbBufferEp0In[3] = 0x04;
                                    }
                                    else
                                    {
                                        // Select the string and send it
                                        size = 0;
                                        if((setup->value&0xFF) <= g_BootUsbVar.NbString)
                                        {
                                            size =
                                                strlen((INT8 *)g_BootUsbVar.String[(setup->value&0xFF)-1])*2
                                                + 2;
                                            g_BootUsbBufferEp0In[0] = size;
                                            g_BootUsbBufferEp0In[1] = 0x03;
                                            boot_UsbAsciiToUtf8(&g_BootUsbBufferEp0In[2],
                                                                g_BootUsbVar.String
                                                                [(setup->value&0xFF)-1]);
                                        }
                                    }
                                    break;
                            }
                            if(setup->lenght < size)
                            {
                                size = setup->lenght;
                            }
                            /* Data in */
                            boot_UsbSend(0, g_BootUsbBufferEp0In, size, 0);
                            setup_completed = 1;
                            break;
                    }
                    break;
                case BOOT_USB_REQUEST_DESTINATION_INTERFACE:
                    switch(boot_UsbCallbackInterface(setup->index&0xFF,
                                                     HAL_USB_CALLBACK_TYPE_CMD, setup))
                    {
                        case HAL_USB_CALLBACK_RETURN_OK:
                            setup_completed = 1;
                            break;
                        case HAL_USB_CALLBACK_RETURN_RUNNING:
                            setup_completed = 2;
                            break;
                        case HAL_USB_CALLBACK_RETURN_KO:
                            break;
                    }
                    break;
                case BOOT_USB_REQUEST_DESTINATION_EP:
                    switch(setup->request)
                    {
                        case BOOT_USB_REQUEST_EP_GET_STATUS   :
                            break;
                        case BOOT_USB_REQUEST_EP_CLEAR_FEATURE:
                            if(setup->value == 0 || setup->value & 0x01)
                            {
                                boot_UsbEpStall(setup->index&0xFF, FALSE);
                            }
                            setup_completed = 1;
                            break;
                        case BOOT_USB_REQUEST_EP_SET_FEATURE  :
                            break;
                        default:
                            break;
                    }
                    switch(boot_UsbCallbackEp(setup->index&0xFF,
                                              HAL_USB_CALLBACK_TYPE_CMD, setup))
                    {
                        case HAL_USB_CALLBACK_RETURN_OK:
                            setup_completed = 1;
                            break;
                        case HAL_USB_CALLBACK_RETURN_RUNNING:
                            setup_completed = 2;
                            break;
                        case HAL_USB_CALLBACK_RETURN_KO:
                            break;
                    }
                    break;
                default:
                    break;
            }
            if(setup_completed == 0 || setup_completed == 1)
            {
                // Command finish. Ready to receive status.
                if(setup->lenght == 0)
                {
                    if(setup->requestDesc.requestDirection)
                    {
                        boot_UsbStatusOut();
                    }
                    else
                    {
                        boot_UsbStatusIn();
                    }
                }
            }
            if(setup_completed == 0)
            {
                if(setup->lenght != 0)
                {
                    // Command success and data must be sent

                    if(setup->requestDesc.requestDirection)
                    {
                        boot_UsbSend(0, g_BootUsbBufferEp0In, 0, 0);
                    }
                    else
                    {
                        boot_UsbRecv(0, g_BootUsbBufferEp0Out, HAL_USB_MPS, 0);
                    }
                }
            }
            break;
        case EP0_STATE_IN    :
            /* Transfert finish */
            if(boot_UsbContinueTransfert(HAL_USB_EP_DIRECTION_IN(0)))
            {
                switch(g_BootUsbVar.RequestDesc.requestDest)
                {
                    case BOOT_USB_REQUEST_DESTINATION_DEVICE:
                        break;
                    case BOOT_USB_REQUEST_DESTINATION_INTERFACE:
                        boot_UsbCallbackInterface(g_BootUsbVar.Ep0Index&0xFF,
                                                  HAL_USB_CALLBACK_TYPE_DATA_CMD,
                                                  setup);
                        break;
                    case BOOT_USB_REQUEST_DESTINATION_EP:
                        boot_UsbCallbackEp(g_BootUsbVar.Ep0Index&0xFF,
                                           HAL_USB_CALLBACK_TYPE_DATA_CMD, setup);
                        break;
                }
                boot_UsbStatusOut();
            }
            break;
        case EP0_STATE_OUT   :
            /* Transfert finish */
            if(boot_UsbContinueTransfert(HAL_USB_EP_DIRECTION_OUT(0)))
            {
                switch(g_BootUsbVar.RequestDesc.requestDest)
                {
                    case BOOT_USB_REQUEST_DESTINATION_DEVICE:
                        break;
                    case BOOT_USB_REQUEST_DESTINATION_INTERFACE:
                        boot_UsbCallbackInterface(g_BootUsbVar.Ep0Index&0xFF,
                                                  HAL_USB_CALLBACK_TYPE_DATA_CMD,
                                                  setup);
                        break;
                    case BOOT_USB_REQUEST_DESTINATION_EP:
                        boot_UsbCallbackEp(g_BootUsbVar.Ep0Index&0xFF,
                                           HAL_USB_CALLBACK_TYPE_DATA_CMD, setup);
                        break;
                }
                boot_UsbStatusIn();
            }
            break;
        case EP0_STATE_STATUS_IN:
            boot_UsbContinueTransfert(HAL_USB_EP_DIRECTION_IN(0));
            g_BootUsbVar.Ep0State = EP0_STATE_IDLE;
            boot_getSetupPacket();
            break;
        case EP0_STATE_STATUS_OUT:
            boot_UsbContinueTransfert(HAL_USB_EP_DIRECTION_OUT(0));
            g_BootUsbVar.Ep0State = EP0_STATE_IDLE;
            boot_getSetupPacket();
            break;
    }
}

// =============================================================================
// FUNCTIONS
// =============================================================================

PUBLIC VOID boot_UsbInitVar(VOID)
{
    g_BootUsbVar.DeviceCallback = 0;
    g_BootUsbVar.Desc           = 0;
    g_BootUsbVar.NbEp           = 0;
    g_BootUsbVar.NbInterface    = 0;
    g_BootUsbVar.NbString       = 0;
    g_BootUsbVar.Ep0State       = EP0_STATE_IDLE;
    g_BootUsbVar.Config         = 0xFF;
}

PUBLIC VOID boot_UsbIrqHandler(UINT8 interruptId)
{
    UINT32 status;
    UINT32 statusEp;
    UINT32 data;
    UINT8  i;

    BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandler);

    // To keep the compiler warning (unused variable)
    interruptId = interruptId;

    // Store interrupt flag and reset it

    status             = hwp_usbc->GINTSTS;
    status            &= USBC_USBRST | USBC_ENUMDONE
                         | USBC_IEPINT | USBC_OEPINT | USBC_USBSUSP | USBC_ERLYSUSP;
    hwp_usbc->GINTSTS |= status;

    if(status & USBC_USBRST)
    {
        // Usb reset

        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerReset);
        boot_UsbInit();

        // Get usb description
        if(g_BootUsbVar.DeviceCallback != 0)
        {
            g_BootUsbVar.Desc = g_BootUsbVar.DeviceCallback(TRUE);
        }

        hwp_usbc->DCFG = USBC_DEVSPD(3);

        // Enable DMA INCR4
        hwp_usbc->GINTMSK   |= USBC_OEPINT | USBC_IEPINT;
        hwp_usbc->GAHBCFG   |= USBC_DMAEN  | USBC_HBSTLEN(BOOT_USB_AHB_MODE);

        // EP interrupt EP0 IN/OUT
        hwp_usbc->DAINTMSK   = 1 | (1<<16);

        hwp_usbc->DOEPMSK    =  USBC_SETUPMK    | USBC_XFERCOMPLMSK;
        hwp_usbc->DIEPMSK    =  USBC_TIMEOUTMSK | USBC_XFERCOMPLMSK;

        // We got a reset, and reseted the soft state machine: Discard all other
        // interrupt causes.
        status &= USBC_ENUMDONE;

        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerReset);
    }

    if(status & USBC_ENUMDONE)
    {
        // Enumeration done

        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerEnum);

        // Config max packet size
        hwp_usbc->DIEPCTL0  = USBC_EP0_MPS(0);
        hwp_usbc->DOEPCTL0  = USBC_EP0_MPS(0);

        // Config EP0
        boot_getSetupPacket();

        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerEnum);
    }

    if(status & USBC_IEPINT) // TX
    {
        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerI);
        data = hwp_usbc->DAINT & hwp_usbc->DAINTMSK;
        for(i = 0; i < g_BootUsbVar.NbEp; ++i)
        {
            // EP IN i have interrupt event
            if((data>>i)&1)
            {
                if(i == 0)
                {
                    // EP0

                    statusEp            = hwp_usbc->DIEPINT0;
                    if(statusEp & USBC_TIMEOUTMSK)
                    {
                        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerIto);

                        // Clear Timeout interupt
                        hwp_usbc->DIEPINT0  = USBC_SETUPMK;
                        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerIto);
                    }
                    if(statusEp & USBC_XFERCOMPLMSK)
                    {
                        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerIcp);
                        // Clear transfert completed interrupt
                        hwp_usbc->DIEPINT0  = USBC_XFERCOMPLMSK;
                        // Decode Ep0 command
                        boot_UsbDecodeEp0Packet();
                        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerIcp);
                    }
                }
                else
                {
                    statusEp = hwp_usbc->DIEPnCONFIG[i-1].DIEPINT;

                    // Interrupt EP mode
                    if(g_BootUsbVar.EpInterruptMask & (1<<i))
                    {
                        hwp_usbc->DIEPnCONFIG[i-1].DIEPINT = statusEp;
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {
                            if(boot_UsbContinueTransfert(
                                        HAL_USB_EP_DIRECTION_IN(i)))
                            {
                                boot_UsbCallbackEp
                                (HAL_USB_EP_DIRECTION_IN(i),
                                 HAL_USB_CALLBACK_TYPE_TRANSMIT_END, 0);
                            }
                        }
                        if(statusEp & USBC_TIMEOUTMSK)
                        {
                            // Clear Timeout interupt
                            BOOT_PROFILE_PULSE(BOOT_USB_TIMEOUT_IN);
                            hwp_usbc->DIEPnCONFIG[i].DIEPINT = USBC_SETUPMK;
                        }
                    }
                    else
                    {
                        if(statusEp & USBC_XFERCOMPLMSK || statusEp & USBC_TIMEOUTMSK)
                        {
                            hwp_usbc->DAINTMSK &= ~(1<<i);
                        }
                    }
                }
            }
        }
        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerI);
    }

    if(status & USBC_OEPINT) // RX
    {
        data = hwp_usbc->DAINT & hwp_usbc->DAINTMSK;
        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerO);
        for(i = 0; i < g_BootUsbVar.NbEp; ++i)
        {
            // EP OUT i have interrupt event
            if((data>>(i+16))&1)
            {
                if(i == 0)
                {
                    statusEp            = hwp_usbc->DOEPINT0;

                    if(statusEp & USBC_SETUPMK)
                    {
                        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerOsp);
                        // Decode Ep0 command
                        boot_UsbDecodeEp0Packet();
                        // Clear Setup interrupt
                        hwp_usbc->DOEPINT0 = USBC_SETUPMK;
                        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerOsp);
                    }
                    if(statusEp & USBC_XFERCOMPLMSK)
                    {
                        BOOT_PROFILE_FUNCTION_ENTER(boot_UsbIrqHandlerOcp);
                        // Clear transfert completed interrupt
                        hwp_usbc->DOEPINT0 = USBC_XFERCOMPLMSK;
                        // Decode Ep0 command
                        boot_UsbDecodeEp0Packet();

                        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerOcp);
                    }
                }
                else
                {
                    statusEp = hwp_usbc->DOEPnCONFIG[i-1].DOEPINT;
                    // Interrupt EP mode
                    if(g_BootUsbVar.EpInterruptMask & (1<<(i+16)))
                    {
                        hwp_usbc->DOEPnCONFIG[i-1].DOEPINT = statusEp;
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {
                            if(boot_UsbContinueTransfert(
                                        HAL_USB_EP_DIRECTION_OUT(i)))
                            {
                                boot_UsbCallbackEp
                                (HAL_USB_EP_DIRECTION_OUT(i),
                                 HAL_USB_CALLBACK_TYPE_RECEIVE_END, 0);
                            }
                        }
                    }
                    else
                    {
                        if(statusEp & USBC_XFERCOMPLMSK)
                        {
                            hwp_usbc->DAINTMSK &= ~(1<<(i+16));
                        }
                    }
                }
            }
        }
        BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandlerO);
    }

    BOOT_PROFILE_FUNCTION_EXIT(boot_UsbIrqHandler);
}

PUBLIC VOID boot_UsbOpen(HAL_USB_GETDESCRIPTOR_CALLBACK_T callback)
{
    // USB reset.
    boot_UsbInit();
    // Register callback
    g_BootUsbVar.DeviceCallback = callback;
    boot_UsbConfig();
}

PUBLIC VOID boot_UsbEpStall(UINT8 ep, BOOL stall)
{
    UINT8   epNum;
    REG32*  diepctl;
    REG32*  doepctl;

    epNum = HAL_USB_EP_NUM(ep);

    // Select ctl register
    if(epNum == 0)
    {
        diepctl = &hwp_usbc->DIEPCTL0;
        doepctl = &hwp_usbc->DOEPCTL0;
    }
    else
    {
        diepctl = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL;
        doepctl = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL;
    }

    // Select direction
    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        if(stall == TRUE)
        {
            if(*diepctl & USBC_EPENA)
            {
                *diepctl |= USBC_STALL | USBC_EPDIS;
            }
            else
            {
                *diepctl |= USBC_STALL;
            }
        }
        else
        {
            *diepctl &= ~(USBC_STALL | USBC_EPDIS | USBC_EPENA | USBC_USBACTEP);
            *diepctl |=   USBC_SETD0PID;
        }
    }
    else
    {
        if(stall == TRUE)
        {
            *doepctl |= USBC_STALL;
        }
        else
        {
            *doepctl &= ~(USBC_STALL | USBC_EPDIS | USBC_EPENA | USBC_USBACTEP);
        }
    }
}



// =============================================================================
// boot_UsbRecv
// -----------------------------------------------------------------------------
/// Recv an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the data buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag No flag you must set this parameter to zero
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 boot_UsbRecv(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    UINT8   epNum;
    UINT32  activeEp;

    epNum = HAL_USB_EP_NUM(ep);
    if(epNum == 0)
    {
        activeEp = USBC_USBACTEP;
    }
    else
    {
        activeEp = hwp_usbc->DOEPnCONFIG[epNum-1].DOEPCTL & USBC_USBACTEP;
    }

    if(activeEp &&
            boot_UsbStartTransfert(HAL_USB_EP_DIRECTION_OUT(ep), buffer, size, flag)
            == 0)
    {
        return(size);
    }
    return(-1);
}


// =============================================================================
// boot_UsbSend
// -----------------------------------------------------------------------------
/// Send an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the date buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag For partial send set the flag to 1
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 boot_UsbSend(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag)
{
    UINT8   epNum;
    UINT32  activeEp;

    epNum = HAL_USB_EP_NUM(ep);
    if(epNum == 0)
    {
        activeEp = USBC_USBACTEP;
    }
    else
    {
        activeEp = hwp_usbc->DIEPnCONFIG[epNum-1].DIEPCTL & USBC_USBACTEP;
    }

    if(activeEp &&
            boot_UsbStartTransfert(HAL_USB_EP_DIRECTION_IN(ep), buffer, size, flag)
            == 0)
    {
        return(size);
    }
    return(-1);
}

PUBLIC VOID boot_UsbCompletedCommand(VOID)
{
    switch(g_BootUsbVar.Ep0State)
    {
        case EP0_STATE_STATUS_OUT:
            boot_UsbStatusOut();
            break;
        case EP0_STATE_STATUS_IN:
            boot_UsbStatusIn();
            break;
        default:
            break;

    }
}

PUBLIC VOID boot_UsbEpEnableInterrupt(UINT8 ep, BOOL enable)
{
    UINT8   epNum;
    epNum = HAL_USB_EP_NUM(ep);

    if(enable == TRUE)
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            g_BootUsbVar.EpInterruptMask    |= (1<<(epNum+0));
        }
        else
        {
            g_BootUsbVar.EpInterruptMask    |= (1<<(epNum+16));
        }
    }
    else
    {
        if(HAL_USB_IS_EP_DIRECTION_IN(ep))
        {
            g_BootUsbVar.EpInterruptMask    &= ~(1<<(epNum+0));
        }
        else
        {
            g_BootUsbVar.EpInterruptMask    &= ~(1<<(epNum+16));
        }
    }
}

PUBLIC UINT16 boot_UsbEpRxTransferedSize(UINT8 ep)
{
    UINT8   epNum;

    epNum = HAL_USB_EP_NUM(ep);

    // When the transfer is done, the size value is
    // inverted. Consequently, when this function
    // is called after a transfer is done, size
    // must be inverted.
    return(-g_BootUsbVar.OutTransfert[epNum].size);
}

PUBLIC BOOL boot_UsbEpTransfertDone(UINT8 ep)
{
    UINT8   epNum;
    UINT8   offset;
    BOOL    value = FALSE;
    REG32*  reg;

    epNum = HAL_USB_EP_NUM(ep);

    if(HAL_USB_IS_EP_DIRECTION_IN(ep))
    {
        offset = 0;
        if(g_BootUsbVar.InTransfert[epNum].size <= 0)
        {
            return(TRUE);
        }

        if(epNum == 0)
        {
            reg = &hwp_usbc->DIEPINT0;
        }
        else
        {
            reg = &hwp_usbc->DIEPnCONFIG[epNum-1].DIEPINT;
        }
        if(*reg&USBC_TIMEOUTMSK)
        {
            *reg  = USBC_TIMEOUTMSK;
            boot_UsbCancelTransfert(ep);
            BOOT_PROFILE_PULSE(BOOT_USB_TIMEOUT_IN);
            value = TRUE;
        }
    }
    else
    {
        offset = 16;
        if(epNum == 0)
        {
            reg = &hwp_usbc->DOEPINT0;
        }
        else
        {
            reg = &hwp_usbc->DOEPnCONFIG[epNum-1].DOEPINT;
        }
    }
    if(*reg&USBC_XFERCOMPL)
    {
        *reg  = USBC_XFERCOMPL;
        value = (boot_UsbContinueTransfert(ep) == 1);
    }

    hwp_usbc->DAINTMSK |= 1<<(epNum+offset);

    return(value);
}

PUBLIC VOID boot_UsbClose(VOID)
{
    // Clear the configuration, and call the callbacks.
    boot_UsbClrConfig();
    g_BootUsbVar.Ep0State        = EP0_STATE_IDLE;
    g_BootUsbVar.NbString        = 0;
    hwp_usbc->GAHBCFG            = 0;
    hwp_usbc->GUSBCFG            = 0;
    g_BootUsbVar.DeviceCallback  = 0;
    boot_UsbResetTransfert();
    g_BootUsbVar.Desc            = 0;
}


// =============================================================================
// hal_UsbReset
// -----------------------------------------------------------------------------
/// This function is use to reset USB and/or change configuration of usb
/// For reset you must do hal_UsbReset(TRUE); sleep 0.5s; hal_UsbReset(FALSE);
/// @param reset If is true, the function is clear usb and power down usb else
/// the function power up usb
// =============================================================================
PUBLIC VOID boot_UsbReset(VOID)
{
    if(hwp_usbc->GAHBCFG & USBC_DMAEN)
    {
        hwp_usbc->GRSTCTL   = USBC_CSFTRST;
        boot_UsbInit();
        boot_UsbConfig();
    }
}



