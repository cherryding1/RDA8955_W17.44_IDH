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

#include "api_msg.h"
#include <cswtype.h>


//
//convert API Plmn format to array: nOperatorId[6]
//[in] pLAI
//[out] nOperatorId
//
VOID PlmnApi2Array(UINT8 *pLAI, UINT8 *nOperatorId);

//
//convert array: nOperatorId[6] to api plmn format
//[in] nOperatorId
//[out] pLAI
//
VOID plmnArray2Api(UINT8 *nOperatorId, UINT8 *pLAI);


// convert API net status to AT status refer: AT+CREG
// [in] nStatusApi
// [in] bRoaming
// [out] pStatusAt
VOID NetStatusApi2At(UINT8 nStatusApi, BOOL bRoaming, UINT8 *pStatusAt);

// convert API gprs status to AT status refer: AT+CGREG
// [in] nStatusApi
// [out] pStatusAt

VOID GprsStatusApi2At(UINT8 nStatusApi, BOOL bRoaming, UINT8 *pStatusAt
     #ifdef LTE_NBIOT_SUPPORT
	 ,CFW_SIM_ID nSimID
	 #endif
	 ) ;


