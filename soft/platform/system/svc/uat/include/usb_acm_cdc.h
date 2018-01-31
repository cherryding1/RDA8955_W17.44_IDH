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

#ifndef _USBAT_CDC_H_
#define _USBAT_CDC_H_

#include "cs_types.h"

/*
 * communications class types
 *
 * c.f. CDC  USB Class Definitions for Communications Devices
 * c.f. WMCD USB CDC Subclass Specification for Wireless Mobile Communications Devices
 *
 */

#define CLASS_BCD_VERSION       0x0110

/* c.f. CDC 4.1 Table 14 */
#define COMMUNICATIONS_DEVICE_CLASS 0x02

/* c.f. CDC 4.2 Table 15 */
#define COMMUNICATIONS_INTERFACE_CLASS_CONTROL  0x02
#define COMMUNICATIONS_INTERFACE_CLASS_DATA     0x0A
#define COMMUNICATIONS_INTERFACE_CLASS_VENDOR   0x0FF

/* c.f. CDC 4.3 Table 16 */
#define COMMUNICATIONS_NO_SUBCLASS      0x00
#define COMMUNICATIONS_DLCM_SUBCLASS    0x01
#define COMMUNICATIONS_ACM_SUBCLASS     0x02
#define COMMUNICATIONS_TCM_SUBCLASS     0x03
#define COMMUNICATIONS_MCCM_SUBCLASS    0x04
#define COMMUNICATIONS_CCM_SUBCLASS     0x05
#define COMMUNICATIONS_ENCM_SUBCLASS    0x06
#define COMMUNICATIONS_ANCM_SUBCLASS    0x07

/* c.f. WMCD 5.1 */
#define COMMUNICATIONS_WHCM_SUBCLASS    0x08
#define COMMUNICATIONS_DMM_SUBCLASS     0x09
#define COMMUNICATIONS_MDLM_SUBCLASS    0x0a
#define COMMUNICATIONS_OBEX_SUBCLASS    0x0b

/* c.f. CDC 4.4 Table 17 */
#define COMMUNICATIONS_NO_PROTOCOL      0x00
#define COMMUNICATIONS_V25TER_PROTOCOL  0x01    /*Common AT Hayes compatible*/

/* c.f. CDC 4.5 Table 18 */
#define DATA_INTERFACE_CLASS        0x0a

/* c.f. CDC 4.6 No Table */
#define DATA_INTERFACE_SUBCLASS_NONE    0x00    /* No subclass pertinent */

/* c.f. CDC 4.7 Table 19 */
#define DATA_INTERFACE_PROTOCOL_NONE    0x00    /* No class protcol required */


/* c.f. CDC 5.2.3 Table 24 */
#define CS_INTERFACE        0x24
#define CS_ENDPOINT         0x25


typedef struct
{
    u8 bFunctionLength;
    u8 bDescriptorType;
    u8 bDescriptorSubtype;  /* 0x00 */
    u16 bcdCDC;
} __attribute__ ((packed))usb_class_header_function_descriptor;



typedef struct
{
    u8 bFunctionLength;
    u8 bDescriptorType;
    u8 bDescriptorSubtype;  /* 0x01 */
    u8 bmCapabilities;
    u8 bDataInterface;
}  __attribute__ ((packed))usb_class_call_management_descriptor;

typedef struct
{
    u8 bFunctionLength;
    u8 bDescriptorType;
    u8 bDescriptorSubtype;  /* 0x02 */
    u8 bmCapabilities;
}  __attribute__ ((packed))usb_class_abstract_control_descriptor;

typedef struct
{
    u8 bFunctionLength;
    u8 bDescriptorType;
    u8 bDescriptorSubtype;  /* 0x06 */
    u8 bMasterInterface;
    /* u8 bSlaveInterface0[0]; */
    u8 bSlaveInterface0;
}  __attribute__ ((packed))usb_class_union_function_descriptor;



#define USB_ST_HEADER       0x00
#define USB_ST_CMF          0x01
#define USB_ST_ACMF         0x02
#define USB_ST_DLMF         0x03
#define USB_ST_TRF          0x04
#define USB_ST_TCLF         0x05
#define USB_ST_UF           0x06
#define USB_ST_CSF          0x07
#define USB_ST_TOMF         0x08
#define USB_ST_USBTF        0x09
#define USB_ST_NCT          0x0a
#define USB_ST_PUF          0x0b
#define USB_ST_EUF          0x0c
#define USB_ST_MCMF         0x0d
#define USB_ST_CCMF         0x0e
#define USB_ST_ENF          0x0f
#define USB_ST_ATMNF        0x10

#define USB_ST_WHCM         0x11
#define USB_ST_MDLM         0x12
#define USB_ST_MDLMD        0x13
#define USB_ST_DMM          0x14
#define USB_ST_OBEX         0x15
#define USB_ST_CS           0x16
#define USB_ST_CSD          0x17
#define USB_ST_TCM          0x18


#endif // _UVIDEOSP_DEBUG_H_


