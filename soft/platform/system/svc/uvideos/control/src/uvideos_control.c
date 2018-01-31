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
#include "uvideos_config.h"

typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;
#include "itf_msg.h"
#include "hal_usb.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "uvideos_control_m.h"
#include "uvideosp_debug.h"

PRIVATE UINT8                        g_uvideosIntrEp;

PRIVATE HAL_USB_CALLBACK_RETURN_T
videosIntrEpCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "videosIntrEpCallback\n");
    return(HAL_USB_CALLBACK_RETURN_OK);
}


PRIVATE HAL_USB_CALLBACK_RETURN_T
videosControlCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                      HAL_USB_SETUP_T*             setup)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "videosControlCallback,type=%d\n",type);
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            break;
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            break;
    }    return(HAL_USB_CALLBACK_RETURN_KO);
}


PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T*
uvideos_ControlOpen(CONST UVIDEOS_CONTROL_PARAMETERS_T*  cfg)
{
    HAL_USB_EP_DESCRIPTOR_T** epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T**)
             sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T*)*2);

    g_uvideosIntrEp = HAL_USB_EP_DIRECTION_IN(cfg->intrEpNum);

    epList[0] = uctls_NewInterruptEpDescriptor(g_uvideosIntrEp, 1, videosIntrEpCallback);
    epList[1] = 0;

    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "UVIDEOS control Open\n");
    return uctls_NewInterfaceDescriptor(0x0e, 0x01, 0x00, 0x00,
                                        epList, "Coolsand Video", videosControlCallback);
}
PRIVATE VOID uvideos_ControlClose(VOID)
{

}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UVIDEOS_CONTROL_CALLBACK_T g_uvideosControlCallback =
{
    .open  = uvideos_ControlOpen,
    .close = uvideos_ControlClose
};

