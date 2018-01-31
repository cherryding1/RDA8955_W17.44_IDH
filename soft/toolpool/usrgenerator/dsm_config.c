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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs_types.h"

#include "memd_m.h"
#include "memdp.h"
#include "dsm_dev_driver.h"
#include "dsm_config.h"
#include "vds_local.h"
#include "vds_cd.h"
#include "tgt_dsm_cfg.h"

/* name conversion */
#define DSM_MemSet memset
#define DSM_MemCpy memcpy
#define DSM_StrCpy strcpy
#define DSM_MAlloc malloc

PROTECTED CONST DSM_CONFIG_T g_tgtDsmCfg = TGT_DSM_CONFIG;

PUBLIC CONST DSM_CONFIG_T* tgt_GetDsmCfg(VOID)
{
    return &g_tgtDsmCfg;
}
