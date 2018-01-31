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

#include "uctls_m.h"
#include "uctls_callback.h"
#include "sxr_ops.h"
#include "string.h"
#include "hal_usb_descriptor.h"

PRIVATE CONST UWIFI_CFG_T* g_uwifiConfig;

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** uwifi_Open(CONST UWIFI_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;

    if(config == 0)
    {
        return 0;
    }
    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*2);

    g_uwifiConfig = config;

    // Open transport
    interfaceDesc[0] = config->wtransportCfg.wtransportCallback
                       ->open(&config->wtransportCfg.wtransportParameters);
    interfaceDesc[1] = 0;

    return interfaceDesc;
}

PRIVATE VOID uwifi_Close(VOID)
{

    // Close Transport
    g_uwifiConfig->wtransportCfg.wtransportCallback->close();
}

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_uwifiCallback =
{
    .open  = (HAL_USB_INTERFACE_DESCRIPTOR_T** (*)
    (CONST UCTLS_SERVICE_CFG_T*))                 uwifi_Open,
    .close =                                      uwifi_Close
};

