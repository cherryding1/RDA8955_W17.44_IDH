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


#include <cfw_config_prv.h>

#if (CFW_NW_DUMP_ENABLE==0) && (CFW_NW_SRV_ENABLE==1)

#include "cfw_nw_data.h"
#include "cfw_nw_tool.h"

#include <ts.h>
//
//convert API Plmn format to array: nOperatorId[6]
//[in] pLAI
//[out] nOperatorId
//
/*
3GPP 0408 10.5.1.3  Location Area Identification
   8     7     6     5     4     3     2     1
+-----------------------------------------------+
│     │    Location Area Identification IEI     │ octet 1
+-----------------------------------------------│
│                       │                       │
│      MCC digit 2      │      MCC digit 1      │ octet 2
+-----------------------+-----------------------│
│  1     1     1     1  │                       │
│                       │      MCC digit 3      │ octet 3
+-----------------------+-----------------------│
│                       │                       │
│      MNC digit 2      │      MNC digit 1      │ octet 4
+-----------------------------------------------│
│                                               │
│                      LAC                      │ octet 5
+-----------------------------------------------│
│                                               │
│                  LAC (continued)              │ octet 6
+-----------------------------------------------+
*/
VOID PlmnApi2Array(UINT8 *pLAI, UINT8 *nOperatorId)
{
    nOperatorId[0] = pLAI[0] & 0x0F;
    nOperatorId[1] = (pLAI[0] & 0xF0) >> 4;
    nOperatorId[2] = pLAI[1] & 0x0F;
    nOperatorId[3] = pLAI[2] & 0x0F;
    nOperatorId[4] = (pLAI[2] & 0xF0) >> 4;
    nOperatorId[5] = (pLAI[1] & 0xF0) >> 4;
}

//
//convert array: nOperatorId[6] to api plmn format
//[in] nOperatorId
//[out] pLAI
//40580f   4   f5   8
VOID plmnArray2Api(UINT8 *nOperatorId, UINT8 *pLAI)
{
    pLAI[0] = (nOperatorId[0] & 0x0F) + ((nOperatorId[1] << 4) & 0xF0);
    pLAI[1] = (nOperatorId[2] & 0x0F) + ((nOperatorId[5] << 4) & 0xF0);
    pLAI[2] = (nOperatorId[3] & 0x0F) + ((nOperatorId[4] << 4) & 0xF0);
}

// convert API net status to AT status refer: AT+CREG
// [in] bStackStop: the stack is stop or not
// [in] nStatusApi: status from api
// [in] bRoaming : bRoaming from api
// [out] pStatusAt : the net status refer: AT+CREG
VOID NetStatusApi2At(UINT8 nStatusApi, BOOL bRoaming, UINT8 *pStatusAt)
{

    if (nStatusApi == API_NW_NO_SVCE)    // no service
    {
        *pStatusAt = CFW_NW_STATUS_NOTREGISTERED_SEARCHING;
    }
    else if (nStatusApi == API_NW_LTD_SVCE)  // limited service
    {
        *pStatusAt = CFW_NW_STATUS_REGISTRATION_DENIED;
    }
    else if (nStatusApi == API_NW_FULL_SVCE)  // full service
    {
        if (bRoaming)   // roaming
        {
            *pStatusAt = CFW_NW_STATUS_REGISTERED_ROAMING;
        }
        else // home
        {
            *pStatusAt = CFW_NW_STATUS_REGISTERED_HOME;
        }
    }
    else
    {
        *pStatusAt = CFW_NW_STATUS_UNKNOW;
    }
}

//// have not finished
VOID GprsStatusApi2At(UINT8 nStatusApi, BOOL bRoaming, UINT8 *pStatusAt
	#ifdef LTE_NBIOT_SUPPORT
	,CFW_SIM_ID nSimID
	#endif
	)
{
    if (nStatusApi == API_NW_NO_SVCE)    // no service
    {
        *pStatusAt = CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING;
    }
    else if (nStatusApi == API_NW_FULL_SVCE)  // full service
    {

        if (bRoaming)   // roaming
        {
            *pStatusAt = CFW_NW_STATUS_REGISTERED_ROAMING;
        }
        else // home
        {
            *pStatusAt = CFW_NW_STATUS_REGISTERED_HOME;
        }
    }
    else if (nStatusApi == API_NW_REG_PENDING)   //Outcome of registn not known yet; PS only
    {
        *pStatusAt = CFW_NW_STATUS_NOTREGISTERED_SEARCHING;
    }
    else if(nStatusApi == API_NW_LTD_SVCE)
    {
    #ifdef LTE_NBIOT_SUPPORT
        if(CFW_NWGetRat(nSimID) == CFW_RAT_NBIOT_ONLY)
            *pStatusAt = CFW_NW_STATUS_REGISTRATION_DENIED;
        else
    #endif
            *pStatusAt = CFW_NW_STATUS_UNKNOW;
    }
    else
    {
        *pStatusAt = CFW_NW_STATUS_UNKNOW;
    }
}


#endif // ENABLE 
