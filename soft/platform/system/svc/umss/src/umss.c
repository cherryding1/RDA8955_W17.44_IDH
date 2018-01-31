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

#include "umss_m.h"
#include "uctls_m.h"
#include "uctls_callback.h"
#include "sxr_ops.h"


PRIVATE VOID*             g_umssStorageHandler[UMSS_STORAGE_MAX];

PRIVATE CONST UMSS_CFG_T* g_umssConfig;

UFLASH_WRTBLK_FUNC_P  gpf_uflash_write_block = NULL;
UFLASH_RDBLK_FUNC_P    gpf_uflash_read_block = NULL;

void init_uFlash_Func (UFLASH_WRTBLK_FUNC_P  pf_wrtblk_func,
                       UFLASH_RDBLK_FUNC_P pf_rdblk_func)
{
    gpf_uflash_write_block = pf_wrtblk_func;
    gpf_uflash_read_block = pf_rdblk_func;
}
PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** umss_Open(CONST UMSS_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;
    int                              i;

    if(config == 0)
    {
        return 0;
    }

    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*3);

    g_umssConfig = config;
    // Open storage
    for(i = 0; i < UMSS_STORAGE_MAX; ++i)
    {
        if(config->storageCfg[i].storageCallback != 0)
        {
            g_umssStorageHandler[i] = config->storageCfg[i].storageCallback
                                      ->open(&config->storageCfg[i].storageParameters);
        }
    }
    // Open transport
    interfaceDesc[0] = config->transportCfg.transportCallback
                       ->open(&config->transportCfg.transportParameters,
                              config->storageCfg, g_umssStorageHandler);
    interfaceDesc[1] = 0;
    interfaceDesc[2] = 0;
    return interfaceDesc;
}

PRIVATE VOID umss_Close(VOID)
{
    UINT8 i;
    // Close transport
    g_umssConfig->transportCfg.transportCallback->close();
    // Close storage
    for(i = 0; i < UMSS_STORAGE_MAX; ++i)
    {
        if(g_umssConfig->storageCfg[i].storageCallback != 0)
        {
            g_umssConfig->storageCfg[i].storageCallback
            ->close(g_umssStorageHandler[i]);
        }
    }
}

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_umssCallback =
{
    .open  = (HAL_USB_INTERFACE_DESCRIPTOR_T** (*)
    (CONST UCTLS_SERVICE_CFG_T*))                 umss_Open,
    .close =                                      umss_Close
};

