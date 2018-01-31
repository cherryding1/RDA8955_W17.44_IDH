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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_DEBUG

#include "uctls_m.h"
#include "uctls_callback.h"
#include "sxr_ops.h"
#include "string.h"
#include "hal_usb_descriptor.h"
#include "uat_m.h"
#include "usb_acm_cdc.h"
#include "svcp_debug.h"

PRIVATE CONST UAT_CFG_T* g_uatConfig;
#define CS_INTERFACE_USBAT 0x24
PRIVATE UINT32 usbAtFunctionalConstructor(UINT8* buffer)
{
    UINT8* p_desc;
    UINT32 if_desc_index = 0;
    UINT32 total_size;
    UINT32 lenF;

    /* class-specific VC interface descriptor */
    p_desc = sxr_Malloc(sizeof(usb_class_header_function_descriptor));
    lenF = sizeof(usb_class_header_function_descriptor);

    ((usb_class_header_function_descriptor*)p_desc)->bFunctionLength = lenF;
    ((usb_class_header_function_descriptor*)p_desc)->bDescriptorType = CS_INTERFACE_USBAT;
    ((usb_class_header_function_descriptor*)p_desc)->bDescriptorSubtype = USB_ST_HEADER;
    ((usb_class_header_function_descriptor*)p_desc)->bcdCDC = 0x0110;; /* video spec rev is 1.0 */


    total_size = sizeof(usb_class_header_function_descriptor)+sizeof(usb_class_call_management_descriptor)
                 +sizeof(usb_class_abstract_control_descriptor)+sizeof(usb_class_union_function_descriptor);

    memcpy(&buffer[if_desc_index], p_desc, lenF);
    if_desc_index += lenF;
    sxr_Free(p_desc);

    /* input (camera) terminal descriptor */
    p_desc = sxr_Malloc(sizeof(usb_class_call_management_descriptor));

    lenF = sizeof(usb_class_call_management_descriptor);
    ((usb_class_call_management_descriptor*)p_desc)->bFunctionLength = lenF;
    ((usb_class_call_management_descriptor*)p_desc)->bDescriptorType = CS_INTERFACE_USBAT;//;
    ((usb_class_call_management_descriptor*)p_desc)->bDescriptorSubtype = USB_ST_CMF;//;
    ((usb_class_call_management_descriptor*)p_desc)->bmCapabilities = 0;//;
    ((usb_class_call_management_descriptor*)p_desc)->bDataInterface = 0x01;
    memcpy(&buffer[if_desc_index ], p_desc, lenF);
    if_desc_index += lenF;
    sxr_Free(p_desc);

    /* output terminal descriptor */
    p_desc = sxr_Malloc(sizeof(usb_class_abstract_control_descriptor));
    lenF = sizeof(usb_class_abstract_control_descriptor);

    ((usb_class_abstract_control_descriptor*)p_desc)->bFunctionLength = lenF;
    ((usb_class_abstract_control_descriptor*)p_desc)->bDescriptorType = CS_INTERFACE_USBAT;
    ((usb_class_abstract_control_descriptor*)p_desc)->bDescriptorSubtype = USB_ST_ACMF;
    ((usb_class_abstract_control_descriptor*)p_desc)->bmCapabilities = 0;

    memcpy(&buffer[if_desc_index ], p_desc, lenF);
    if_desc_index += lenF;
    sxr_Free(p_desc);

    /* processing unit descriptor */
    p_desc = sxr_Malloc(sizeof(usb_class_union_function_descriptor));

    lenF = sizeof(usb_class_union_function_descriptor);
    ((usb_class_union_function_descriptor*)p_desc)->bFunctionLength = lenF;
    ((usb_class_union_function_descriptor*)p_desc)->bDescriptorType = CS_INTERFACE_USBAT;
    ((usb_class_union_function_descriptor*)p_desc)->bDescriptorSubtype = USB_ST_UF;
    ((usb_class_union_function_descriptor*)p_desc)->bMasterInterface = 0;//;
    ((usb_class_union_function_descriptor*)p_desc)->bSlaveInterface0 = 1;//;

    //((USBVideo_PU_Dscr*)p_desc)->bmVideoStandards = 0;  /* for 1.1 revision */

    memcpy(&buffer[if_desc_index ], p_desc, lenF);
    if_desc_index += lenF;
    sxr_Free(p_desc);

    return total_size;


}

#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_
PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** uat_Open(CONST UAT_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;

    USBLOGI("UAT open");
    if(config == 0)
    {
        return 0;
    }
    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*3);

    g_uatConfig = config;
    //Open Communication Control
    interfaceDesc[0] = config->controlCfg.controlCallback
                       ->open(&config->controlCfg.controlParameters);
    //Open other fucntional class
    //interfaceDesc[1] = (HAL_USB_INTERFACE_DESCRIPTOR_T*)hal_UsbDescriptorNewCSInterfaceDescriptor(usbAtFunctionalConstructor, 0);
    //Open Communication Data
    interfaceDesc[1] = config->streamCfg.streamCallback
                       ->open(&config->streamCfg.streamParameters);

    interfaceDesc[2] = 0;

    return(interfaceDesc);

}
#else

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** uat_Open(CONST UAT_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;

    USBLOGI("UAT open");
    if(config == 0)
    {
        return 0;
    }
    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*4);

    g_uatConfig = config;
    //Open Communication Control
    interfaceDesc[0] = config->controlCfg.controlCallback
                       ->open(&config->controlCfg.controlParameters);
    //Open other fucntional class
    interfaceDesc[1] = (HAL_USB_INTERFACE_DESCRIPTOR_T*)hal_UsbDescriptorNewCSInterfaceDescriptor(usbAtFunctionalConstructor, 0);
    //Open Communication Data
    interfaceDesc[2] = config->streamCfg.streamCallback
                       ->open(&config->streamCfg.streamParameters);

    interfaceDesc[3] = 0;

    return(interfaceDesc);

}
#endif
PRIVATE VOID uat_Close(VOID)
{

    // Close Transport
    USBLOGI("UAT close");
    g_uatConfig->streamCfg.streamCallback->close();
}

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_uatCallback =
{
    .open  = (HAL_USB_INTERFACE_DESCRIPTOR_T** (*)
    (CONST UCTLS_SERVICE_CFG_T*))                 uat_Open,
    .close =                                      uat_Close
};


