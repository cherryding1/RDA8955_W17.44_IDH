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





















#if !defined(__CSW_MEM_PRV_H__)
#define __CSW_MEM_PRV_H__

#include <ts.h>
#include <base_prv.h>
#include "sxs_io.h"

#define CSW_DRV_MALLOC(size)    CSW_MALLOC(CFW_DRV_TS_ID, size)
#define CSW_DM_MALLOC(size)     CSW_MALLOC(DM_TS_ID, size)
#define CSW_PM_MALLOC(size)     CSW_MALLOC(CFW_PM_TS_ID, size)
#define CSW_BAL_MALLOC(size)    CSW_MALLOC(BASE_BAL_TS_ID, size)
#define CSW_COS_MALLOC(size)    CSW_MALLOC(BASE_COS_TS_ID, size)
#define CSW_VFS_MALLOC(size)    CSW_MALLOC(BASE_VFS_TS_ID, size)
#define CSW_FFS_MALLOC(size)    CSW_MALLOC(BASE_FFS_TS_ID, size)
#define CSW_FAT_MALLOC(size)    CSW_MALLOC(BASE_FAT_TS_ID, size)
#define CSW_DB_MALLOC(size)     CSW_MALLOC(BASE_DB_TS_ID, size)
#define CSW_REG_MALLOC(size)    CSW_MALLOC(BASE_REG_TS_ID, size)
#define CSW_SUL_MALLOC(size)    CSW_MALLOC(CFW_AOM_TS_ID, size)
#define CSW_TIM_MALLOC(size)    CSW_MALLOC(BASE_TM_TS_ID, size)
#define CSW_AOM_MALLOC(size)    CSW_MALLOC(CFW_AOM_TS_ID, size)
#define CSW_CFG_MALLOC(size)    CSW_MALLOC(CFW_CFG_TS_ID, size)
#define CSW_NW_MALLOC(size)     CSW_MALLOC(CFW_NW_TS_ID, size)
#define CSW_GPRS_MALLOC(size)   CSW_MALLOC(CFW_GPRS_TS_ID, size)
#define CSW_CSD_MALLOC(size)    CSW_MALLOC(CFW_CSD_TS_ID, size)
#define CSW_SIM_MALLOC(size)    CSW_MALLOC(CFW_SIM_TS_ID, size)
#define CSW_CC_MALLOC(size)     CSW_MALLOC(CFW_CC_TS_ID, size)
#define CSW_SS_MALLOC(size)     CSW_MALLOC(CFW_SS_TS_ID, size)
#define CSW_SMS_MALLOC(size)    CSW_MALLOC(CFW_SMS_TS_ID, size)
#define CSW_PBK_MALLOC(size)    CSW_MALLOC(CFW_PBK_TS_ID, size)
#define CSW_TCPIP_MALLOC(size)  CSW_MALLOC(CFW_TCPIP_TS_ID, size)
#define CSW_SHL_MALLOC(size)    CSW_MALLOC(CFW_SHELL_TS_ID, size)
#define CSW_ML_MALLOC(size)    CSW_MALLOC(CFW_ML_TS_ID, size)
#define CSW_TMS_MALLOC(size)    CSW_MALLOC(CFW_AOM_TS_ID, size)
#define CSW_AT_MALLOC(size)     CSW_MALLOC(CFW_AOM_TS_ID, size)

#define CSW_DRV_FREE(p)    CSW_FREE(CFW_DRV_TS_ID, p)
#define CSW_DM_FREE(p)     CSW_FREE(DM_TS_ID, p)
#define CSW_PM_FREE(p)     CSW_FREE(CFW_PM_TS_ID, p)
#define CSW_BAL_FREE(p)    CSW_FREE(BASE_BAL_TS_ID, p)
#define CSW_COS_FREE(p)    CSW_FREE(BASE_COS_TS_ID, p)
#define CSW_VFS_FREE(p)    CSW_FREE(BASE_VFS_TS_ID, p)
#define CSW_FFS_FREE(p)    CSW_FREE(BASE_FFS_TS_ID, p)
#define CSW_FAT_FREE(p)    CSW_FREE(BASE_FAT_TS_ID, p)
#define CSW_DB_FREE(p)     CSW_FREE(BASE_DB_TS_ID, p)
#define CSW_REG_FREE(p)    CSW_FREE(BASE_REG_TS_ID, p)
#define CSW_SUL_FREE(p)    CSW_FREE(BASE_SUL_TS_ID, p)
#define CSW_TIM_FREE(p)    CSW_FREE(BASE_TM_TS_ID, p)
#define CSW_AOM_FREE(p)    CSW_FREE(CFW_AOM_TS_ID, p)
#define CSW_CFG_FREE(p)    CSW_FREE(CFW_CFG_TS_ID, p)
#define CSW_NW_FREE(p)     CSW_FREE(CFW_NW_TS_ID, p)
#define CSW_GPRS_FREE(p)   CSW_FREE(CFW_GPRS_TS_ID, p)
#define CSW_CSD_FREE(p)    CSW_FREE(CFW_CSD_TS_ID, p)
#define CSW_SIM_FREE(p)    CSW_FREE(CFW_SIM_TS_ID, p)
#define CSW_CC_FREE(p)     CSW_FREE(CFW_CC_TS_ID, p)
#define CSW_SS_FREE(p)     CSW_FREE(CFW_SS_TS_ID, p)
#define CSW_SMS_FREE(p)    CSW_FREE(CFW_SMS_TS_ID, p)
#define CSW_PBK_FREE(p)    CSW_FREE(CFW_PBK_TS_ID, p)
#define CSW_TCPIP_FREE(p)  CSW_FREE(CFW_TCPIP_TS_ID, p)
#define CSW_SHL_FREE(p)    CSW_FREE(CFW_SHELL_TS_ID, p)
#define CSW_ML_FREE(p)    CSW_FREE(CFW_ML_TS_ID, p)
#define CSW_TMS_FREE(p)    CSW_FREE(TMS_TS_ID, p)
#define CSW_AT_FREE(p)     CSW_FREE(AT_TS_ID, p)

#endif  // H

