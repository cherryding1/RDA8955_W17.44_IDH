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

#include "error_id.h"
#include "dsm_cf.h"
#include "sms_dm_cd.h"
#include "sms_dm_local.h"
#include "vds_api.h"
#include "dsm_dbg.h"

//------------------------------------------------------------------------------------
//description: this function output the start index and end index for searching loop by input SIM ID.
//-------------------------------------------------------------------------------------
//author:  Hameina   || date: 2009.1.22
//-------------------------------------------------------------------------------------
//parameter:        nSimId      [in]     SIM ID
//              nStartIndex [out] output start index to loop
//              nEndIndex   [out] output end index to loop
//-----------------------------------------------------------------------------------
//return value: success     ERR_SUCCESS
//              fail            ERR_SMS_DM_INVALID_PARAMETER
//-----------------------------------------------------------------------------------
INT32 SMS_DM_GetRange(
    CFW_SIM_ID nSimId,
    UINT16 *nStartIndex,
    UINT16 *nEndIndex)
{
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
    *nStartIndex = (UINT16)0;
    *nEndIndex = (UINT16)SMS_DM_MAX_REC_COUNT;
#else
    *nStartIndex = (nSimId - CFW_SIM_0) * (SMS_DM_MAX_REC_COUNT / CFW_SIM_COUNT);
    *nEndIndex = (nSimId - CFW_SIM_0 + 1) * (SMS_DM_MAX_REC_COUNT / CFW_SIM_COUNT);
#endif
    return ERR_SUCCESS;
}
