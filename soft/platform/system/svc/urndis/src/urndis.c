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

#include "urndis_m.h"
#include "uctls_m.h"
#include "uctls_callback.h"
#include "sxr_ops.h"
#include "string.h"
#include "hal_usb_descriptor.h"

#define USB_DT_CS_INTERFACE 0x24

#define USB_CDC_HEADER_TYPE		0x00	/* header_desc */
#define USB_CDC_CALL_MANAGEMENT_TYPE	0x01	/* call_mgmt_descriptor */
#define USB_CDC_ACM_TYPE		0x02	/* acm_descriptor */
#define USB_CDC_UNION_TYPE		0x06	/* union_desc */

PUBLIC CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T g_urndisIad =
{
    .bLength = sizeof g_urndisIad,
    .bDescriptorType = 0x0b,//USB_DT_INTERFACE_ASSOCIATION
    .bFirstInterface = 0x00,
    .bInterfaceCount = 0x02,
    .bFunctionClass = USB_CLASS_WIRELESS_CONTROLLER,
    .bFunctionSubClass = 0x01,//RF controller
    .bFunctionProtocol = 0x03,//Remote NDIS
    .iFunction  = 2,//if not composite,control iInterface is 0,data iInterface is 1,iad iFunction is 2
};

/* "Header Functional Descriptor" from CDC spec  5.2.3.1 */
struct usb_cdc_header_desc {
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bDescriptorSubType;

	UINT16	bcdCDC;
} __attribute__ ((packed));

/* "Call Management Descriptor" from CDC spec  5.2.3.2 */
struct usb_cdc_call_mgmt_descriptor {
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bDescriptorSubType;

	UINT8	bmCapabilities;
	UINT8	bDataInterface;
} __attribute__ ((packed));

/* "Abstract Control Management Descriptor" from CDC spec  5.2.3.3 */
struct usb_cdc_acm_descriptor {
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bDescriptorSubType;

	UINT8	bmCapabilities;
} __attribute__ ((packed));

/* "Union Functional Descriptor" from CDC spec 5.2.3.8 */
struct usb_cdc_union_desc {
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bDescriptorSubType;

	UINT8	bMasterInterface0;
	UINT8	bSlaveInterface0;
	/* ... and there could be other slave interfaces */
} __attribute__ ((packed));

static struct usb_cdc_header_desc header_desc = {
	.bLength =		sizeof header_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_HEADER_TYPE,

	.bcdCDC =		0x0110,
};

static struct usb_cdc_call_mgmt_descriptor call_mgmt_descriptor = {
	.bLength =		sizeof call_mgmt_descriptor,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_CALL_MANAGEMENT_TYPE,

	.bmCapabilities =	0x00,
	.bDataInterface =	0x01,
};

static struct usb_cdc_acm_descriptor rndis_acm_descriptor = {
	.bLength =		sizeof rndis_acm_descriptor,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_ACM_TYPE,

	.bmCapabilities =	0x00,
};

static struct usb_cdc_union_desc rndis_union_desc = {
	.bLength =		sizeof(rndis_union_desc),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_UNION_TYPE,
    .bMasterInterface0 =	0x00,
	.bSlaveInterface0 =	0x01,
};

PRIVATE CONST URNDIS_CFG_T* g_urndisConfig;

PRIVATE UINT32 urndisCSdescConstructor(UINT8* buffer)
{
    UINT32 if_desc_index = 0;

    memcpy(&buffer[if_desc_index], &header_desc, sizeof(header_desc));
    if_desc_index += sizeof(header_desc);
    memcpy(&buffer[if_desc_index], &call_mgmt_descriptor, sizeof(call_mgmt_descriptor));
    if_desc_index += sizeof(call_mgmt_descriptor);
    memcpy(&buffer[if_desc_index], &rndis_acm_descriptor, sizeof(rndis_acm_descriptor));
    if_desc_index += sizeof(rndis_acm_descriptor);
    memcpy(&buffer[if_desc_index], &rndis_union_desc, sizeof(rndis_union_desc));
    if_desc_index += sizeof(rndis_union_desc);

    return if_desc_index;
}

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** urndis_Open(CONST URNDIS_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;

    if(config == 0)
    {
        return 0;
    }

    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*4);

    g_urndisConfig = config;

    //Open Control itf
    interfaceDesc[0] = config->controlCfg.controlCallback
                       ->open(&config->controlCfg.controlParameters);
    //Open other CS desc
    interfaceDesc[1] = (HAL_USB_INTERFACE_DESCRIPTOR_T*)hal_UsbDescriptorNewCSInterfaceDescriptor(urndisCSdescConstructor, 0);
    //Open Data itf
    interfaceDesc[2] = config->dataCfg.dataCallback
                       ->open(&config->dataCfg.dataParameters);
    interfaceDesc[3] = 0;

    return(interfaceDesc);
}

PRIVATE VOID urndis_Close(VOID)
{
    // Close Control
    g_urndisConfig->controlCfg.controlCallback->close();
    // Close Data
    g_urndisConfig->dataCfg.dataCallback->close();

}

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_urndisCallback =
{
    .open  = (HAL_USB_INTERFACE_DESCRIPTOR_T** (*)
    (CONST UCTLS_SERVICE_CFG_T*))                 urndis_Open,
    .close =                                      urndis_Close
};

