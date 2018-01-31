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

#ifndef __CFW_CONFIG_PRV_H__
#define __CFW_CONFIG_PRV_H__

//
// Define the version number
//
#define CFW_NW_SRV_VER            100 // 1.0.0
#define CFW_SIM_SRV_VER           100 // 1.0.0
#define CFW_CM_SRV_VER            100 // 1.0.0
#define CFW_SS_SRV_VER            100 // 1.0.0
#define CFW_SMS_SRV_VER           100 // 1.0.0

//
// Define the service enalbe or disable ,
// 1: enalbe(use) the specified service
// 0: disable(don't use) the specified servie.
//
#define CFW_NW_SRV_ENABLE         1 // NW Service Management
#define CFW_SIM_SRV_ENABLE        1 // SIM Service Management
#define CFW_CM_SRV_ENABLE         1 // CM Service Management
#define CFW_SS_SRV_ENABLE         1 // SS Service Management
#define CFW_SMS_SRV_ENABLE        1 // SMS Service Management

#define CFW_NW_DUMP_ENABLE         0
#define CFW_CM_DUMP_ENABLE         0
#define CFW_SS_DUMP_ENABLE         0
#define CFW_SIM_DUMP_ENABLE        0
#define CFW_SMS_DUMP_ENABLE        0

//
//add by lixp
//
#define  CFW_SIM_SMS_SUPPORT_LONG_MSG       1  // 1:support long msg, 0:doesn't support
#define  g_SMS_ValueCheck                   1

//SMS support number
#define CFW_SMS_MAX_REC_COUNT                   PHONE_SMS_ENTRY_COUNT
//SMS data size
#define CFW_SMS_REC_DATA_SIZE                   184

#endif // _H_

