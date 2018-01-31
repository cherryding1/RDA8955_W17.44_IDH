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
#if (USB_HOST_SUPPORT ==1)
#include "pmd_m.h"
#include "sxr_ops.h"
#include "halp_sys.h"
#include "usb.h"
#include "halp_usb.h"
#include "halp_debug.h"
#include "halp_version.h"
#include "hal_usb.h"
#include "usb_hcd.h"
#include "string.h"
#include "cfg_regs.h"
#include "global_macros.h"
#include "usbc.h"
#include "sys_ctrl.h"
#include "usbhcd.h"
#define usb_HstSendEvent

#if 0
#define RDA_USB_MAX_END     16
#define USB_END0_MAX_BUF    64

#define RXFIFOSIZE            64
#define TXFIFOSIZE            48

// endpoint 0 stage
#define USB_END0_STAGE_IDLE            0
#define USB_END0_STAGE_SETUP        1
#define USB_END0_STAGE_IN            2
#define USB_END0_STAGE_OUT            3
#define USB_END0_STAGE_STATUS_IN    4
#define USB_END0_STAGE_STATUS_OUT    5

// endpoint 0 dir
#define USB_END0_DIRIN            0x80
// endpoint type defined by synopsys
#define USB_END_CONTROL        0
#define USB_END_ISO            1
#define USB_END_BULK        2
#define USB_END_INTERRUPT    3

// usb internal dma mode , AHB Master burst type:
#define USB_IDMA_SINGLE        0
#define USB_IDMA_INCR        1
#define USB_IDMA_INCR4        3
#define USB_IDMA_INCR8        5
#define USB_IDMA_INCR16        7

//host channel n interrupt mask
#define HCNI_XFERCOMPL_MSK        (1<<0)
#define HCNI_XFERCOMPL            (1<<0)
#define HCNI_CHHLTD_MSK            (1<<1)
#define HCNI_CHHLTD                (1<<1)
#define HCNI_AHBERR_MSK            (1<<2)
#define HCNI_AHBERR                (1<<2)
#define HCNI_STALL_MSK            (1<<3)
#define HCNI_STALL                (1<<3)
#define HCNI_NAK_MSK            (1<<4)
#define HCNI_NAK                (1<<4)
#define HCNI_ACK_MSK            (1<<5)
#define HCNI_ACK                 (1<<5)
#define HCNI_NYET_MSK            (1<<6)
#define HCNI_NYET                (1<<6)
#define HCNI_XACTERR_MSK        (1<<7)
#define HCNI_XACTERR            (1<<7)
#define HCNI_BBLERR_MSK            (1<<8)
#define HCNI_BBLERR                (1<<8)
#define    HCNI_FRMOVRUN_MSK        (1<<9)
#define    HCNI_FRMOVRUN            (1<<9)
#define HCNI_DTGLERR_MSK        (1<<10)
#define HCNI_DTGLERR             (1<<10)

//HPRT bitmap
#define USBH_PRTCONNSTS        (1<<0)
#define USBH_PRTCONNDET        (1<<1)
#define USBH_PRTENA            (1<<2)
#define USBH_PRTENCHNG        (1<<3)
#define USBH_PRTOVRCURRACT    (1<<4)
#define USBH_PRTOVRCURRCHNG    (1<<5)
#define USBH_PRTRESUME        (1<<6)
#define USBH_PRTSUSP        (1<<7)
#define USBH_PRTRST            (1<<8)
#define USBH_PRTPWR            (1<<12)

//HCCHARn bitmap
#define USBH_CHENA        (1<<31)
#define USBH_CHDIS        (1<<30)
#define USBH_ODDFRM        (1<<29)
#define USBH_DEVADDR(n)    ((n&0x3f)<<22)
#define USBH_IN            (1<<15)
#define USBH_EPTYPE(n)    ((n&0x03)<<18)
#define USBH_LSPDDEV    (1<<17)
#define USBH_EPDIRIN    (1<<15)
#define USBH_EPDIROUT    ()
#define USBH_EPNUM(n)    ((n&0x0f)<<11)
#define USBH_MPS(n)        (n&0x07ff)

//HCTSIZn bitmap
#define USBH_PKTCNT(n)        ((n&0x03ff)<<19)
#define USBH_XFERSIZE(n)    ((n&0x7ffff))
#define USBH_PID(n)            ((n&0x03)<<29)

//according to synopsys's datasheet
#define USB_DATA0        0
#define USB_DATA1        2
#define USB_DATA2        1
#define USB_MDATA        3

typedef struct tagUSBHostReg
{
    REG32    GOTGCTL;                      //0x00000000
    REG32   GOTGINT;                      //0x00000004
    REG32   GAHBCFG;                      //0x00000008
    REG32   GUSBCFG;                      //0x0000000C
    REG32   GRSTCTL;                      //0x00000010
    REG32   GINTSTS;                      //0x00000014
    REG32   GINTMSK;                      //0x00000018
    REG32   GRXSTSR;                      //0x0000001C
    REG32   GRXSTSP;                      //0x00000020
    REG32   GRXFSIZ;                      //0x00000024
    REG32   GNPTXFSIZ;                    //0x00000028
    REG32   GNPTXSTS;                     //0x0000002C
    REG32   GI2CCTL;                      //0x00000030
    REG32   GPVNDCTL;                     //0x00000034
    REG32   GGPIO;                        //0x00000038
    REG32   GUID;                         //0x0000003C
    REG32   GSNPSID;                      //0x00000040
    REG32   GHWCFG1;                      //0x00000044
    REG32   GHWCFG2;                      //0x00000048
    REG32   GHWCFG3;                      //0x0000004C
    REG32   GHWCFG4;                      //0x00000050
    REG32     Reserved_00000054[43];        //0x00000054
    REG32   HPTXFSIZ;                     //0x00000100
    REG32   Reserved_00000110[191];       //0x00000104 - 3FF
    //host part
    REG32     HCFG;                          /*0x00000400*/
    REG32    HFIR;                          /*0x00000404*/
    REG32    HFNUM;                          /*0x00000408*/
    REG32    Reserved_0000040c;              /*0x0000040c*/
    REG32    HPTXSTS;                      /*0x00000410*/
    REG32    HAINT;                          /*0x00000414*/
    REG32    HAINTMSK;                      /*0x00000418*/
    REG32    Reserved_0000041c[9];          /*0x0000041C*/
    REG32    HPRT;                          /*0x00000440*/
    REG32    Reserved_00000444[47];          /*0x00000444*/
    struct
    {
        REG32    HCCHARn;                /*0x00000500+n*0x20*/
        REG32    HCSPLTn;                /*0x00000504+n*0x20*/
        REG32    HCINTn;                    /*0x00000508+n*0x20*/
        REG32    HCINTMSKn;                /*0x0000050c+n*0x20*/
        REG32    HCTSIZn;                /*0x00000510+n*0x20*/
        REG32    HCDMAn;                    /*0x00000504+n*0x20*/
        REG32    Reserved_0000000n[2];
    } HCSR[16];
} USBHOSTREG;

typedef struct tagUSBENDPOINT
{
    uint8    Epaddress;
    uint8     type;
    uint16     nMaxPktSize;
    uint16    nCurTransferred;
    uint8     DataToggle;
    uint8     pktCnt;
    USBBULKREQUEST    *pBulkReq;
} USBENDPOINT;

typedef struct tagUSBHOSTINST
{
    uint8     dmaEnabled;
    uint8     end0Stage;
    uint8     endNum;        /*zero based, 0 for control channel*/
    uint8     UsbState;
    uint8     portEnabled;
    uint8    deviceSpeed;
    uint8     DataToggle;
    uint8     address;                /*device address set by host*/
    USBCONTROLREQUEST * pCtrlReq;
    USBENDPOINT epArray[RDA_USB_MAX_END];
    void (*InitCompleteCb)(void *pData, int status);
} USBHOSTINST;
#endif

static USBHOSTINST gUsbHostInst;

/**/
int RDAUsb_PacketTransfer(int chn)
{
    USBENDPOINT *pEnd = &gUsbHostInst.epArray[chn];
    uint8     pktSize = 0;
    uint8     *pBuf;

    if (pEnd->pktCnt == 0 || pEnd->nCurTransferred == pEnd->pBulkReq->nBufLen)
    {
        return 1;
    }

    if (pEnd->pktCnt > 1)
    {
        pktSize = pEnd->nMaxPktSize;
        //pBuf = pEnd->pBulkReq->pBuf + pEnd->nCurTransferred;
    }
    else
    {
        pktSize = pEnd->pBulkReq->nBufLen - pEnd->nCurTransferred;
        //pBuf = pEnd->pBulkReq->pBuf + pEnd->nCurTransferred;
    }

    pBuf = pEnd->pBulkReq->pBuf + pEnd->nCurTransferred;

    if (pEnd->Epaddress&USB_END0_DIRIN)
    {
        gUsbRegs->HCSR[chn].HCINTMSKn = 0x7ef;
    }
    else
    {
        gUsbRegs->HCSR[chn].HCINTMSKn = 0x7ff;
    }

    usb_HstSendEvent(0x09150002);
    usb_HstSendEvent(pBuf);
    usb_HstSendEvent(pktSize);
    usb_HstSendEvent(0x22555558); usb_HstSendEvent(pktSize);
    gUsbRegs->HCSR[chn].HCDMAn = (uint32)pBuf;
    gUsbRegs->HCSR[chn].HCTSIZn = USBH_XFERSIZE(pktSize) | USBH_PKTCNT(1) | USBH_PID(pEnd->DataToggle);
    gUsbRegs->HCSR[chn].HCCHARn |= USBH_CHENA;
    return 0;
}

/*host part init*/
void rda_UsbHostInitPart2()
{
    uint32 addr = 0;
    gUsbRegs->GINTMSK |= USBC_PRTINT_MASK;
    gUsbRegs->HCFG |= 0x05;                                /* full/low speed, 48 MHZ */
    gUsbRegs->HPRT |= USBH_PRTPWR;                        /* power on */
    sxr_Sleep(20 MS_WAITING);                            /* wait for HPRT connDet interrupt */
    gUsbHostInst.deviceSpeed = (gUsbRegs->HPRT>>17)&0x03;
    gUsbRegs->HFIR = 48000;

    if (gUsbRegs->GHWCFG2 & USBC_DYNFIFOSIZING)
    {
        gUsbRegs->GRXFSIZ  = RXFIFOSIZE;
        addr  = RXFIFOSIZE;
        hal_HstSendEvent(0x99001234); hal_HstSendEvent((gUsbRegs->GHWCFG3>>16));  //dma fifo size
        //gUsbRegs->GNPTXFSIZ  = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(0x100); //RXFIFOSIZE
        gUsbRegs->GNPTXFSIZ  = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(64); //the usb fifo not so larger . 64*4 bytes  must  <= 64*4
        addr += 0x100;    //RXFIFOSIZE;
        //gUsbRegs->HPTXFSIZ      = USBC_NPTXFSTADDR(addr) | USBC_NPTXFDEPS(0x100);  //HPTXFSIZ
        //addr += 0x100;
    }

    /*flush fifo*/
    gUsbRegs->GRSTCTL |= USBC_TXFFLSH | USBC_RXFFLSH;
}


int RDAUsb_BulkTransfer(USBBULKREQUEST * pReq)
{
    USBENDPOINT * pEnd;

    if (pReq == NULL || gUsbHostInst.portEnabled==0)
    {
        return USBHCD_ERROR_FAILED;
    }

    pEnd =&gUsbHostInst.epArray[pReq->pipe];
    pEnd->pBulkReq = pReq;
    pEnd->nCurTransferred = 0;
    pEnd->pktCnt = pReq->nBufLen / pEnd->nMaxPktSize + 1;
    RDAUsb_PacketTransfer(pReq->pipe);
    return USB_NO_ERROR;
}

USB_PIPE RDAUsb_OpenPipe(USBENDPOINTDESC *pEndDes)
{
    int i = 0;

    if (((gUsbHostInst.endNum+1) > ((gUsbRegs->GHWCFG2>>14)&0x0F)) || pEndDes == NULL)
    {
        return NULL;
    }

    i = ++gUsbHostInst.endNum;
    gUsbHostInst.epArray[i].Epaddress = pEndDes->bEndpointAddress;
    gUsbHostInst.epArray[i].nMaxPktSize = pEndDes->wMaxPacketSize;
    gUsbHostInst.epArray[i].type = pEndDes->bmAttributes;
    gUsbHostInst.epArray[i].DataToggle = USB_DATA0;    /*first packect should be USB_DATA0*/
    gUsbRegs->HAINTMSK |= (1<<i);
    //gUsbRegs->HCSR[i].HCINTMSKn = 0x7ff;    /*unmask all kind of channel's interrupt*/
    gUsbRegs->HCSR[i].HCCHARn = USBH_MPS(pEndDes->wMaxPacketSize) | USBH_EPNUM(pEndDes->bEndpointAddress) \
                                | USBH_EPTYPE(pEndDes->bmAttributes) | USBH_DEVADDR(gUsbHostInst.address);

    if (pEndDes->bEndpointAddress & 0x80)
    {
        gUsbRegs->HCSR[i].HCCHARn |= USBH_EPDIRIN;
    }

    return (USB_PIPE)gUsbHostInst.endNum;
}

int RDAUsb_ControlTransfer(USBCONTROLREQUEST * pReq)
{
    if (pReq == NULL || gUsbHostInst.portEnabled == 0)
    {
        return USBHCD_ERROR_FAILED;
    }

    gUsbHostInst.pCtrlReq = pReq;
    gUsbHostInst.end0Stage = USB_END0_STAGE_SETUP;
    usb_HstSendEvent(0x09060005);
    gUsbRegs->HAINTMSK |= 1;  /*bit 0 for channel 0*/
    gUsbRegs->GINTMSK |= USBC_HCHINT_MASK;    /*unmask host channel interrupt*/
    /* unmask interrupt, transfer Complete, AHB error, stall response, halted channel, transaction error */
    gUsbRegs->HCSR[0].HCINTMSKn = 0x7ff; //HCNI_XFERCOMPL_MSK | HCNI_CHHLTD_MSK | HCNI_AHBERR_MSK | HCNI_STALL_MSK | HCNI_XACTERR_MSK;
    gUsbRegs->HCSR[0].HCTSIZn = USBH_PID(USB_MDATA); /**/
    gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize);    /*max packet size*/
    gUsbRegs->HCSR[0].HCDMAn = (uint32)&pReq->stdRequest;
    gUsbRegs->HCSR[0].HCTSIZn |=  USBH_XFERSIZE(sizeof(USBSTDREQUEST)) | USBH_PKTCNT(1);
    gUsbRegs->HCSR[0].HCCHARn |= USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
    gUsbHostInst.DataToggle = USB_DATA0;
    usb_HstSendEvent(0x09060015);
    return USB_NO_ERROR;
}


int RDA_UsbReset(void (*CompleteCb)(void* pData, uint8 status))
{
    int count = 0;

    do
    {
        gUsbRegs->HPRT |= USBH_PRTRST;                            /* start reset */
        sxr_Sleep(50 MS_WAITING);
        gUsbRegs->HPRT &= ~USBH_PRTRST;                            /* complete reset */
        sxr_Sleep(200 MS_WAITING);
        count++;
    }
    while (count < 3);  // && !gUsbHostInst.portEnabled);

    if (CompleteCb)
    {
        CompleteCb(NULL, !gUsbHostInst.portEnabled);
    }

    usb_HstSendEvent(0x09170003);
    usb_HstSendEvent(gUsbHostInst.portEnabled);
    return !gUsbHostInst.portEnabled;
}

/*usb core init*/
void RDA_UsbCoreInit()
{
    uint32     AHBCfg = 0;
    usb_HstSendEvent(0x8815777); usb_HstSendEvent((u32)gUsbRegs);
    //flush fifo
    gUsbRegs->GRSTCTL  =  USBC_TXFNUM(0x10) | USBC_TXFFLSH | USBC_RXFFLSH;
    usb_HstSendEvent(0x1815aaa2);

    /*core initialization*/
    if (gUsbRegs->GHWCFG2&(3<<3))    // external DMA or Internal DMA impl
    {
        usb_HstSendEvent(0xdddddaa3);
        //AHBCfg |= USBC_DMAEN | USB_IDMA_INCR<<1;  // send date one bytes by bytes
        AHBCfg |= USBC_DMAEN | USB_IDMA_INCR4<<1;
        gUsbHostInst.dmaEnabled = 1;
    }

    AHBCfg |= USBC_GLBLINTRMSK;
    gUsbRegs->GAHBCFG |= AHBCfg;
    gUsbRegs->GUSBCFG |= USBC_PHYIF | USBC_USBTRDTIM(5); //USBC_USBTRDTIM(9)
    gUsbRegs->GINTMSK =  USBC_MODEMIS_MASK | USBC_DISCONNINT_MASK; // | USBC_SOF_MASK;    /*unmask port status interrupt bit*/
    usb_HstSendEvent(0x1815aaa3);
    /*host initialization*/
    rda_UsbHostInitPart2();
    usb_HstSendEvent(0xccccaaa4);
}

int RDA_UsbHostInit(USBOPERATION *pUsbOps, void (*InitCb)(void*pData, int status))
{
    if (pUsbOps != NULL)
    {
        pUsbOps->BulkTransfer = RDAUsb_BulkTransfer;
        pUsbOps->ControlTransfer = RDAUsb_ControlTransfer;
        pUsbOps->OpenPipe = RDAUsb_OpenPipe;
        //pUsbOps->ResetDefaultEndpoint = RDA_UsbReset;
    }

    memset(&gUsbHostInst, 0, sizeof(gUsbHostInst));
    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;
    gUsbHostInst.InitCompleteCb = InitCb;
    gUsbHostInst.epArray[0].nMaxPktSize = 0x40;    /*preset to 64 byte, update then we got device descriptor*/
    usb_HstSendEvent(0x0815bbb3);
    RDA_UsbCoreInit();
    usb_HstSendEvent(0x0815aaa1);
    //if( InitCb )
    {
        //InitCb(NULL, 0);
    }
    return 0;
}

#if 0  // dma maybe must size mutiple of 4  bug
// ytt
u8 dma_buffer_setup[512];

void rda_ProcessControlInterrupt()
{
    int ints = gUsbRegs->HCSR[0].HCINTn;
    u8 * dma_bf_setup;
    dma_bf_setup = MEM_ACCESS_UNCACHED(dma_buffer_setup);
    gUsbRegs->HCSR[0].HCINTn = 0x7ff;
    ints &= HCNI_XFERCOMPL | HCNI_CHHLTD | HCNI_AHBERR | HCNI_STALL | HCNI_XACTERR_MSK ;
    usb_HstSendEvent(0x09163333);
    usb_HstSendEvent(ints);

    if (ints&HCNI_CHHLTD)
    {
        if (ints&HCNI_XFERCOMPL)
        {
            usb_HstSendEvent(0x09163334);

            switch (gUsbHostInst.end0Stage)
            {
                case USB_END0_STAGE_SETUP:
                {
                    USBSTDREQUEST *pStdReq = &gUsbHostInst.pCtrlReq->stdRequest;
                    usb_HstSendEvent(0x09163335); usb_HstSendEvent((u32)pStdReq->wLength);
                    hal_TimDelay(2 MS_WAITING);

                    if (pStdReq->wLength > 0)
                    {
                        usb_HstSendEvent(0x09163336);
                        usb_HstSendEvent((UINT32)gUsbHostInst.pCtrlReq->pBuf);
                        usb_HstSendEvent((UINT32)gUsbHostInst.pCtrlReq->nBufLen);
                        memcpy(dma_bf_setup,gUsbHostInst.pCtrlReq->pBuf,gUsbHostInst.pCtrlReq->nBufLen);
                        gUsbRegs->HCSR[0].HCDMAn =dma_bf_setup;// (UINT32)gUsbHostInst.pCtrlReq->pBuf;
                        gUsbHostInst.DataToggle = gUsbHostInst.DataToggle?USB_DATA0:USB_DATA1;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(gUsbHostInst.pCtrlReq->nBufLen) | USBH_PKTCNT(1)| USBH_PID(gUsbHostInst.DataToggle);
                        usb_HstSendEvent(0x09163346);

                        if (pStdReq->bmRequestType & USB_END0_DIRIN)
                        {
                            usb_HstSendEvent(0x09163347);
                            gUsbRegs->HCSR[0].HCCHARn |= USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_IN;
                        }
                        else
                        {
                            usb_HstSendEvent(0x09163348);
                            gUsbRegs->HCSR[0].HCCHARn &= ~USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_OUT;
                        }

                        usb_HstSendEvent(0x09163349); usb_HstSendEvent(gUsbHostInst.address);
                        gUsbRegs->HCSR[0].HCCHARn |= USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);  //  ????  gHcdInst     zzz
                        //hal_HstSendEvent(0x0916334a);
                    }
                    else
                    {
                        usb_HstSendEvent(0xc9163337);
                        gUsbRegs->HCSR[0].HCDMAn = 0;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                        gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_IN | USBH_DEVADDR(gUsbHostInst.address);
                        gUsbHostInst.end0Stage = USB_END0_STAGE_STATUS_IN;
                    }
                }
                break;

                case USB_END0_STAGE_IN:
                {
                    usb_HstSendEvent(0x09160001);
                    hal_TimDelay(1 MS_WAITING);
                    gUsbHostInst.end0Stage= USB_END0_STAGE_STATUS_OUT;
                    hal_SysInvalidateCache(dma_bf_setup,gUsbHostInst.pCtrlReq->nBufLen);
                    memcpy(gUsbHostInst.pCtrlReq->pBuf,dma_bf_setup,gUsbHostInst.pCtrlReq->nBufLen);

                    if (gUsbHostInst.pCtrlReq->stdRequest.bRequest == USB_GET_DESCRIPTOR && (gUsbHostInst.pCtrlReq->stdRequest.wValue & 1<<8))
                    {
                        gUsbHostInst.epArray[0].nMaxPktSize = gUsbHostInst.pCtrlReq->pBuf[7];  //  bMaxPacketSize0
                        usb_HstSendEvent(0xa9160001); usb_HstSendEvent(gUsbHostInst.pCtrlReq->pBuf[7]);
                    }

                    /*status out*/
                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                    gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                }
                break;

                case USB_END0_STAGE_OUT:
                    break;

                case USB_END0_STAGE_STATUS_IN:
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;
                    hal_SysInvalidateCache(dma_bf_setup,gUsbHostInst.pCtrlReq->nBufLen);
                    memcpy(gUsbHostInst.pCtrlReq->pBuf,dma_bf_setup,gUsbHostInst.pCtrlReq->nBufLen);

                    if (gUsbHostInst.pCtrlReq->stdRequest.bRequest == USB_SET_ADDRESS)
                    {
                        gUsbHostInst.address = gUsbHostInst.pCtrlReq->stdRequest.wValue;
                        usb_HstSendEvent(0x09163359); usb_HstSendEvent(gUsbHostInst.pCtrlReq->stdRequest.wValue);
                    }

                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_STATUS_OUT:
                    usb_HstSendEvent(0x09160002);
                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_IDLE:
                    break;
            }
        }
        else  if (!(ints & 0x7fd) || (ints&HCNI_NAK))
        {
            usb_HstSendEvent(0x09280001);

            if (gUsbHostInst.end0Stage== USB_END0_STAGE_STATUS_OUT)
            {
                hal_TimDelay(1 MS_WAITING);
                /*status out*/
                gUsbRegs->HCSR[0].HCDMAn = 0;
                gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
            }
        }
        else
        {
            if (gUsbHostInst.pCtrlReq->completeCb)
            {
                gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 1);
                gUsbHostInst.pCtrlReq->completeCb = NULL;
            }
        }
    }
    else
    {
        //hal_HstSendEvent(0x09160004);
        //hal_HstSendEvent(ints);
    }
}

#else



#ifdef _TRAS_DATA_GREATER_64_SIZE


void rda_ProcessControlInterrupt()
{
    USBCONTROLREQUEST *pCtrlReq = gUsbHostInst.pCtrlReq;
    USBENDPOINT *pEnd = &gUsbHostInst.epArray[0];
    int ints = gUsbRegs->HCSR[0].HCINTn;
    gUsbRegs->HCSR[0].HCINTn = 0x7ff;
    ints &= HCNI_XFERCOMPL | HCNI_CHHLTD | HCNI_AHBERR | HCNI_STALL | HCNI_XACTERR_MSK ;

    //hal_HstSendEvent(SYS_EVENT,0x09163333);
    //hal_HstSendEvent(SYS_EVENT,ints);

    if (ints&HCNI_CHHLTD)
    {
        if (ints&HCNI_XFERCOMPL)
        {
            switch (gUsbHostInst.end0Stage)
            {
                case USB_END0_STAGE_SETUP:
                {
                    USBSTDREQUEST *pStdReq = &pCtrlReq->stdRequest;
                    sxr_Sleep(2 MS_WAITING);

                    if (pStdReq->wLength > 0)
                    {
                        int size = pCtrlReq->nBufLen > 0x40?0x40:pCtrlReq->nBufLen;
                        gUsbRegs->HCSR[0].HCDMAn = (UINT)pCtrlReq->pBuf;
                        gUsbHostInst.DataToggle = gUsbHostInst.DataToggle?USB_DATA0:USB_DATA1;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(size) | USBH_PKTCNT(1)| USBH_PID(gUsbHostInst.DataToggle);

                        if (pStdReq->bmRequestType & USB_END0_DIRIN)
                        {
                            gUsbRegs->HCSR[0].HCCHARn |= USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_IN;
                        }
                        else
                        {
                            gUsbRegs->HCSR[0].HCCHARn &= ~USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_OUT;
                        }

                        pEnd->nCurTransferred = size;
                        gUsbRegs->HCSR[0].HCCHARn |= USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                    }
                    else
                    {
                        gUsbRegs->HCSR[0].HCDMAn = 0;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                        gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(pEnd->nMaxPktSize) | USBH_CHENA | USBH_IN | USBH_DEVADDR(gUsbHostInst.address);
                        gUsbHostInst.end0Stage = USB_END0_STAGE_STATUS_IN;
                    }
                }
                break;

                case USB_END0_STAGE_IN:
                {
                    //hal_HstSendEvent(SYS_EVENT,0x09160001);
                    sxr_Sleep(1 MS_WAITING);

                    if (pCtrlReq->stdRequest.bRequest == USB_GET_DESCRIPTOR && (pCtrlReq->stdRequest.wValue & 1<<8))
                    {
                        pEnd->nMaxPktSize = pCtrlReq->pBuf[7];
                    }

                    if (pEnd->nCurTransferred < pCtrlReq->nBufLen)
                    {
                        int size = (pCtrlReq->nBufLen - pEnd->nCurTransferred);
                        size = size > 0x40 ? 0x40:size;
                        gUsbRegs->HCSR[0].HCDMAn = (UINT)pCtrlReq->pBuf+pEnd->nCurTransferred;
                        gUsbHostInst.DataToggle = gUsbHostInst.DataToggle?USB_DATA0:USB_DATA1;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(size) | USBH_PKTCNT(1)| USBH_PID(gUsbHostInst.DataToggle);
                        gUsbRegs->HCSR[0].HCCHARn |= USBH_IN|USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                        pEnd->nCurTransferred +=size;
                    }
                    else
                    {
                        /*status out*/
                        gUsbHostInst.end0Stage= USB_END0_STAGE_STATUS_OUT;
                        gUsbRegs->HCSR[0].HCDMAn = 0;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                        gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(pEnd->nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                    }
                }
                break;

                case USB_END0_STAGE_OUT:
                {
                    if (pEnd->nCurTransferred < pCtrlReq->nBufLen)
                    {
                        int size = (pCtrlReq->nBufLen - pEnd->nCurTransferred);
                        size = size > 0x40 ? 0x40:size;
                        gUsbRegs->HCSR[0].HCDMAn = (UINT)pCtrlReq->pBuf+pEnd->nCurTransferred;
                        gUsbHostInst.DataToggle = gUsbHostInst.DataToggle?USB_DATA0:USB_DATA1;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(size) | USBH_PKTCNT(1)| USBH_PID(gUsbHostInst.DataToggle);
                        gUsbRegs->HCSR[0].HCCHARn &= ~USBH_IN;
                        gUsbRegs->HCSR[0].HCCHARn |= USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                        pEnd->nCurTransferred += size;
                    }
                    else
                    {
                        gUsbHostInst.end0Stage= USB_END0_STAGE_STATUS_IN;
                    }
                }
                break;

                case USB_END0_STAGE_STATUS_IN:
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;

                    if (gUsbHostInst.pCtrlReq->stdRequest.bRequest == USB_SET_ADDRESS)
                    {
                        gUsbHostInst.address = gUsbHostInst.pCtrlReq->stdRequest.wValue;
                    }

                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_STATUS_OUT:
                    //hal_HstSendEvent(SYS_EVENT,0x09160002);
                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_IDLE:
                    break;
            }
        }
        else  if (!(ints & 0x7fd) || (ints&HCNI_NAK))
        {
            //hal_HstSendEvent(SYS_EVENT,0x09280001);
            if (gUsbHostInst.end0Stage== USB_END0_STAGE_STATUS_OUT)
            {
                sxr_Sleep(1 MS_WAITING);
                /*status out*/
                gUsbRegs->HCSR[0].HCDMAn = 0;
                gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
            }
        }
        else
        {
            if (gUsbHostInst.pCtrlReq->completeCb)
            {
                gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 1);
                gUsbHostInst.pCtrlReq->completeCb = NULL;
            }
        }
    }
    else
    {
        //hal_HstSendEvent(SYS_EVENT,0x09160004);
        //hal_HstSendEvent(SYS_EVENT,ints);
    }
}

#else

// ytt
void rda_ProcessControlInterrupt()
{
    int ints = gUsbRegs->HCSR[0].HCINTn;
    gUsbRegs->HCSR[0].HCINTn = 0x7ff;
    ints &= HCNI_XFERCOMPL | HCNI_CHHLTD | HCNI_AHBERR | HCNI_STALL | HCNI_XACTERR_MSK ;
    usb_HstSendEvent(0x09163333);
    usb_HstSendEvent(ints);

    if (ints&HCNI_CHHLTD)
    {
        if (ints&HCNI_XFERCOMPL)
        {
            usb_HstSendEvent(0x09163334);

            switch (gUsbHostInst.end0Stage)
            {
                case USB_END0_STAGE_SETUP:
                {
                    USBSTDREQUEST *pStdReq = &gUsbHostInst.pCtrlReq->stdRequest;
                    usb_HstSendEvent(0x09163335); usb_HstSendEvent((u32)pStdReq->wLength);
                    hal_TimDelay(2 MS_WAITING);

                    if (pStdReq->wLength > 0)
                    {
                        usb_HstSendEvent(0x09163336);
                        usb_HstSendEvent((UINT32)gUsbHostInst.pCtrlReq->pBuf);
                        usb_HstSendEvent((UINT32)gUsbHostInst.pCtrlReq->nBufLen);
                        gUsbRegs->HCSR[0].HCDMAn = (UINT32)gUsbHostInst.pCtrlReq->pBuf;
                        gUsbHostInst.DataToggle = gUsbHostInst.DataToggle?USB_DATA0:USB_DATA1;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(gUsbHostInst.pCtrlReq->nBufLen) | USBH_PKTCNT(1)| USBH_PID(gUsbHostInst.DataToggle);
                        usb_HstSendEvent(0x09163346);

                        if (pStdReq->bmRequestType & USB_END0_DIRIN)
                        {
                            usb_HstSendEvent(0x09163347);
                            gUsbRegs->HCSR[0].HCCHARn |= USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_IN;
                        }
                        else
                        {
                            usb_HstSendEvent(0x09163348);
                            gUsbRegs->HCSR[0].HCCHARn &= ~USBH_IN;
                            gUsbHostInst.end0Stage = USB_END0_STAGE_OUT;
                        }

                        usb_HstSendEvent(0x09163349); usb_HstSendEvent(gUsbHostInst.address);
                        gUsbRegs->HCSR[0].HCCHARn |= USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);    //    ????  gHcdInst       zzz
                        //usb_HstSendEvent(0x0916334a);
                    }
                    else
                    {
                        usb_HstSendEvent(0xc9163337);
                        gUsbRegs->HCSR[0].HCDMAn = 0;
                        gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                        gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_IN | USBH_DEVADDR(gUsbHostInst.address);
                        gUsbHostInst.end0Stage = USB_END0_STAGE_STATUS_IN;
                    }
                }
                break;

                case USB_END0_STAGE_IN:
                {
                    usb_HstSendEvent(0x09160001);
                    hal_TimDelay(1 MS_WAITING);
                    gUsbHostInst.end0Stage= USB_END0_STAGE_STATUS_OUT;
                    hal_SysInvalidateCache(gUsbHostInst.pCtrlReq->pBuf,gUsbHostInst.pCtrlReq->nBufLen);

                    if (gUsbHostInst.pCtrlReq->stdRequest.bRequest == USB_GET_DESCRIPTOR && (gUsbHostInst.pCtrlReq->stdRequest.wValue & 1<<8))
                    {
                        gUsbHostInst.epArray[0].nMaxPktSize = gUsbHostInst.pCtrlReq->pBuf[7];    //    bMaxPacketSize0
                        usb_HstSendEvent(0xa9160001); usb_HstSendEvent(gUsbHostInst.pCtrlReq->pBuf[7]);
                    }

                    /*status out*/
                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                    gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
                }
                break;

                case USB_END0_STAGE_OUT:
                    break;

                case USB_END0_STAGE_STATUS_IN:
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;

                    if (gUsbHostInst.pCtrlReq->stdRequest.bRequest == USB_SET_ADDRESS)
                    {
                        gUsbHostInst.address = gUsbHostInst.pCtrlReq->stdRequest.wValue;
                        usb_HstSendEvent(0x09163359); usb_HstSendEvent(gUsbHostInst.pCtrlReq->stdRequest.wValue);
                    }

                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_STATUS_OUT:
                    usb_HstSendEvent(0x09160002);
                    gUsbRegs->HCSR[0].HCDMAn = 0;
                    gUsbRegs->HCSR[0].HCTSIZn = 0;
                    gUsbRegs->HCSR[0].HCCHARn = 0;
                    gUsbHostInst.end0Stage = USB_END0_STAGE_IDLE;

                    if (gUsbHostInst.pCtrlReq->completeCb)
                    {
                        gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 0);
                        gUsbHostInst.pCtrlReq->completeCb = NULL;
                    }

                    break;

                case USB_END0_STAGE_IDLE:
                    break;
            }
        }
        else  if (!(ints & 0x7fd) || (ints&HCNI_NAK))
        {
            usb_HstSendEvent(0x09280001);

            if (gUsbHostInst.end0Stage== USB_END0_STAGE_STATUS_OUT)
            {
                hal_TimDelay(1 MS_WAITING);
                /*status out*/
                gUsbRegs->HCSR[0].HCDMAn = 0;
                gUsbRegs->HCSR[0].HCTSIZn = USBH_XFERSIZE(0) | USBH_PKTCNT(1)| USBH_PID(USB_DATA1);
                gUsbRegs->HCSR[0].HCCHARn = USBH_MPS(gUsbHostInst.epArray[0].nMaxPktSize) | USBH_CHENA | USBH_DEVADDR(gUsbHostInst.address);
            }
        }
        else
        {
            if (gUsbHostInst.pCtrlReq->completeCb)
            {
                gUsbHostInst.pCtrlReq->completeCb(gUsbHostInst.pCtrlReq->pBuf, 1);
                gUsbHostInst.pCtrlReq->completeCb = NULL;
            }
        }
    }
    else
    {
        //hal_HstSendEvent(0x09160004);
        //hal_HstSendEvent(ints);
    }
}
#endif
#endif
void(*g_pfDisconnCb)(BOOL isHost)    = NULL;

void hal_UsbSetDisconnCb(void (*Cb)(BOOL isHost))
{
    g_pfDisconnCb = Cb;
}

// main ineterrupt
void hal_UsbIRQServiceHost(UINT8 interruptId)
{
    uint32 reg;
    uint32 intsts;
    interruptId = interruptId;
    intsts = gUsbRegs->GINTSTS;
    intsts &= USBC_PRTINT | USBC_HCHINT | USBC_DISCONNINT | USBC_MODEMIS | USBC_CONLDSTSCHNG | USBC_SOF;
    gUsbRegs->GINTSTS |= intsts ;
    usb_HstSendEvent(0xaaaa0001);

    if (intsts & USBC_PRTINT)
    {
        //usb_HstSendEvent(0xaaaa0002);
        reg = gUsbRegs->HPRT;

        if (reg & USBH_PRTENCHNG)
        {
            usb_HstSendEvent(0x09170001);
            gUsbHostInst.portEnabled = (reg&0x04) >> 2;
            reg &= (~USBH_PRTENA);        /* can't clear USBH_PRTENA*/
            reg |= USBH_PRTENCHNG;
            gUsbRegs->HPRT = reg; //USBH_PRTENCHNG;    /*clear this bit */
            //let's wait a moment here.
            //    sxr_Sleep(1 MS_WAITING);
            hal_TimDelay(20);
            usb_HstSendEvent(0xaaaa0011);    usb_HstSendEvent(gUsbRegs->HPRT);

            if (!(gUsbRegs->HPRT & 1))
            {
                //extern void(*g_pfDisconnCb)(BOOL isHost);
                int i = 1;
                usb_HstSendEvent(0xaaaa0002);    usb_HstSendEvent(gUsbRegs->HPRT);
                gUsbRegs->GAHBCFG = 0;
                gUsbRegs->GINTMSK = 0;
                gUsbHostInst.portEnabled = 0;

                for (; i<gUsbHostInst.endNum+1; i++)
                {
                    if (gUsbHostInst.epArray[i].pBulkReq->completeCb)
                    {
                        usb_HstSendEvent(0x10140003);
                        gUsbHostInst.epArray[i].pBulkReq->pstatus = USBHCD_DEVICE_DISCONNECTED;
                        gUsbHostInst.epArray[i].pBulkReq->completeCb(NULL, 0);
                        break;
                    }
                }

                if (g_pfDisconnCb != NULL)
                {
                    g_pfDisconnCb(1);
                }
            }
        }
        else if (reg & USBH_PRTCONNDET)
        {
            usb_HstSendEvent(0x09170002);
            gUsbRegs->HPRT |= USBH_PRTCONNDET;

            if (gUsbHostInst.InitCompleteCb)
            {
                gUsbHostInst.InitCompleteCb(NULL, 0);
                gUsbHostInst.InitCompleteCb = NULL;
            }
        }
    }

    /*host channels interrupt*/
    if (intsts & USBC_HCHINT)
    {
        //hal_HstSendEvent(0xaaaa0006);
        int i = 1;
        uint32 haint = gUsbRegs->HAINT;
        uint32 haintMsk = gUsbRegs->HAINTMSK;
        reg = 1;

        //endpoint 0 interrupt.
        if ((reg&haint)& haintMsk)
        {
            //hal_HstSendEvent(0xaaaa0016);
            rda_ProcessControlInterrupt();
        }

        // data endpoints' interrupt
        for (; i<gUsbHostInst.endNum+1; i++)
        {
            reg <<= 1;

            if (reg&haint)
            {
                usb_HstSendEvent(0xaaaa0026);
                int hcint = gUsbRegs->HCSR[i].HCINTn;
                USBENDPOINT *pEnd = &gUsbHostInst.epArray[i];
                gUsbRegs->HCSR[i].HCINTn = 0x07ff;

                if (hcint&HCNI_CHHLTD)
                {
                    if ((hcint & HCNI_XFERCOMPL))
                    {
                        usb_HstSendEvent(0xc1234567);

                        if (pEnd->pktCnt > 1)
                        {
                            pEnd->nCurTransferred += pEnd->nMaxPktSize;
                        }
                        else
                        {
                            pEnd->nCurTransferred = pEnd->pBulkReq->nBufLen;
                        }

                        pEnd->pktCnt--;
                        pEnd->DataToggle = pEnd->DataToggle==USB_DATA0?USB_DATA1:USB_DATA0;
                        usb_HstSendEvent(0xaaaa0036);

                        if (RDAUsb_PacketTransfer(i) > 0)
                        {
                            //usb_HstSendEvent(0x55000000+hcint+(reg<<16));
                            //usb_HstSendEvent(pEnd->nCurTransferred);
                            usb_HstSendEvent(0xaaaa0007);
                            *pEnd->pBulkReq->pstatus = USB_NO_ERROR;
                            *pEnd->pBulkReq->pnByteTransferred = pEnd->nCurTransferred;

                            if (pEnd->pBulkReq->completeCb)
                            {
                                pEnd->pBulkReq->completeCb(pEnd->pBulkReq->handle, USB_NO_ERROR);
                            }
                        }
                    }
                    else
                    {
                        usb_HstSendEvent(0xaaaa0046);

                        if (hcint & HCNI_NAK)
                        {
                            //try again
                            *pEnd->pBulkReq->pstatus = USB_RECEIVED_NAK;
                            *pEnd->pBulkReq->pnByteTransferred = 0;
                            usb_HstSendEvent(0xc1234566);
                        }
                        else if (hcint & HCNI_ACK)
                        {
                            *pEnd->pBulkReq->pstatus = USB_RECEIVED_ACK;
                            *pEnd->pBulkReq->pnByteTransferred = pEnd->nCurTransferred;
                            usb_HstSendEvent(0xc1234565);
                        }
                        else if (hcint & HCNI_STALL)
                        {
                            *pEnd->pBulkReq->pstatus = USB_STALL_ERROR;
                            usb_HstSendEvent(0xc1234564); usb_HstSendEvent((u32)(pEnd->pBulkReq->pstatus));
                        }

                        if (pEnd->pBulkReq->completeCb)
                        {
                            //usb_HstSendEvent(0xc1234563);
                            pEnd->pBulkReq->completeCb(pEnd->pBulkReq->handle, 0);
                        }
                    }
                }
                else if (hcint & HCNI_ACK)
                {
                }
                else if ((hcint & HCNI_NAK) && (pEnd->Epaddress&USB_END0_DIRIN))
                {
                    //agony experience, there are too many IN NAK interrupt result for system dead.
                    gUsbRegs->HCSR[i].HCINTMSKn &= ((~HCNI_NAK_MSK)&0x7ff);
                }
            }
        }
    }
}


#endif
#endif

