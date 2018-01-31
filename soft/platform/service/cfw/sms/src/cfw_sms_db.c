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
















#include <sul.h>
#include <ts.h>
#include <cfw.h>
#include <cfw_prv.h>
#include "csw_debug.h"

#include "sms_dm.h"
#include "cfw_sms_db.h"
//=================================================================

INT32 SMS_DM_GetRange(
    CFW_SIM_ID nSimId,
    UINT16 *nStartIndex,
    UINT16 *nEndIndex
);
UINT32  g_cfw_sms_max_num = CFW_SMS_MAX_REC_COUNT;
//UINT32  g_cfw_sms_max_num = SMS_MAX_NUMBER;

typedef struct
{
    UINT8  nStatus;
    UINT8  nDate[175];
    UINT32 nTime;
    UINT32 nAppInt32;
    //UINT8  nPading[CFW_SMS_REC_DATA_SIZE - 184];
} CFW_ME_SMS_PARAM;


//=================================================================
//Create_SMS_DBEX
//
//=================================================================

INT32 Create_SMS_DBEX()
{
    return ERR_SUCCESS ;
}


//=================================================================
//Remove_SMS_DBEX
//
//=================================================================

INT32 Remove_SMS_DBEX()
{
    return SMS_DM_Clear();
}


//=================================================================
//SMS_DeleteFromMeByStatusEX
//
//=================================================================
INT32 SMS_DeleteFromMeByStatusEX (
    const UINT8 nStatus,
    CFW_SIM_ID nSimId
)
{
    UINT16 nIndexBuf[CFW_SMS_MAX_REC_COUNT];
    UINT16 nSMSNumber = 0x0000;
    UINT16 nTmp       = 0x0000;
    INT32 nRet;

    SUL_ZeroMemory32(nIndexBuf, SIZEOF(nIndexBuf));
    nSMSNumber = SMS_DM_GetIndexByStatus(nIndexBuf, (UINT8)nStatus, nSimId);

    for (nTmp = 0x0000; nTmp < nSMSNumber; nTmp++)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The nIndexBuf[nTmp] is %d\r\n",0x08100fb2), nIndexBuf[nTmp]);
        if (ERR_SUCCESS != (nRet = SMS_DM_Delete(nIndexBuf[nTmp])))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("The nIndexBuf[nTmp] %d nRet 0x%x\r\n",0x08100fb3), nIndexBuf[nTmp], nRet);
            return nRet;
        }
    }

    return ERR_SUCCESS;
}


//=================================================================
//SMS_DeleteFromMeByIndexEX
//
//=================================================================
INT32 SMS_DeleteFromMeByIndexEX(const UINT16 nIndex, UINT16 nLocation)
{
    return( SMS_DM_Delete( nIndex - 1));
}


//=================================================================
//SMS_AppendEX
//
//=================================================================
INT32 SMS_AppendEX(
    UINT8 *pData,
    CFW_SIM_ID nSimId,
    UINT16 *nIndex,
    UINT32 nTime,
    UINT32 nAppInt32
)
{
    //
    //pData 第一个字节是状态
    //
    CFW_ME_SMS_PARAM nME_SMS_PARAM;
    UINT32 nRet;


    nME_SMS_PARAM.nStatus   = *pData;
    nME_SMS_PARAM.nTime     = nTime;
    nME_SMS_PARAM.nAppInt32 = nAppInt32;

    SUL_MemCopy8( nME_SMS_PARAM.nDate, &(pData[1]), 175);
    if(ERR_SUCCESS != ( nRet = SMS_DM_Add( nIndex, (PVOID)( &nME_SMS_PARAM), nSimId)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_AppendEX nret: %d\r\n",0x08100fb4), nRet);
        return nRet;
    }

    (*nIndex)++;
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("in SMS_AppendEX, output nIndex is %d\n",0x08100fb5), (*nIndex));

    return ERR_SUCCESS;
}


//=================================================================
//SMS_WriteEX
//
//=================================================================
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
INT32 SMS_WriteEX(UINT16 nIndex, UINT8* pData,  UINT16 nLocation,
                  UINT32 nTime, UINT32 nAppInt32,CFW_SIM_ID nSimId)
#else
INT32 SMS_WriteEX(UINT16 nIndex, UINT8* pData, UINT16 nLocation,
                  UINT32 nTime, UINT32 nAppInt32)
#endif
{
    //
    //pData 第一个字节是状态
    //
    CFW_ME_SMS_PARAM nME_SMS_PARAM;
    nLocation = nLocation;

    SUL_ZeroMemory32( &nME_SMS_PARAM, SIZEOF(CFW_ME_SMS_PARAM));

    nME_SMS_PARAM.nStatus    =  *pData;
    nME_SMS_PARAM.nTime      =  nTime;
    nME_SMS_PARAM.nAppInt32  =  nAppInt32;

    SUL_MemCopy8( nME_SMS_PARAM.nDate, &(pData[1]), 175);
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
    return SMS_DM_OverWrite( nIndex-1,(PVOID)(&nME_SMS_PARAM), (UINT8)nSimId);
#else
    return SMS_DM_OverWrite( nIndex-1,(PVOID)(&nME_SMS_PARAM));
#endif
}


//=================================================================
//SMS_ReadEX
//
//=================================================================

INT32 SMS_ReadEX(UINT16 nIndex, UINT8 *pData, UINT16 nLocation)
{
    UINT8   nDate[CFW_SMS_REC_DATA_SIZE];
    UINT32  nRet;
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
    UINT8 getSimId = 0x00;
#endif
    SUL_ZeroMemory8( nDate, SIZEOF(nDate));

#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE

    if(ERR_SUCCESS != ( nRet = SMS_DM_Read( nIndex-1, nDate, &getSimId)))
#else
    if(ERR_SUCCESS != ( nRet = SMS_DM_Read( nIndex-1, nDate)))
#endif
    {
        return nRet;
    }

    SUL_MemCopy8( pData, nDate, CFW_SMS_REC_DATA_SIZE);

    *pData = SMS_SFUser2SimEX( *pData );
    return ERR_SUCCESS;

}


//=================================================================
//SMS_Read2EX
//
//=================================================================
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
INT32 SMS_Read2EX(UINT16 nIndex, UINT8* nStatus, UINT8* pData,
                  UINT16 nLocation, UINT8* getSimId)
#else
INT32 SMS_Read2EX(UINT16 nIndex, UINT8* nStatus, UINT8* pData,
                  UINT16 nLocation)
#endif
{
    UINT8   Date[CFW_SMS_REC_DATA_SIZE];
    UINT32  nRet;
    SUL_ZeroMemory8( Date, SIZEOF(Date));
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
    if(ERR_SUCCESS != ( nRet = SMS_DM_Read( nIndex-1, Date, getSimId)))
#else
    if(ERR_SUCCESS != ( nRet = SMS_DM_Read( nIndex-1, Date)))
#endif
    {
        return nRet;
    }

    *nStatus = Date[0];
    *nStatus = SMS_SFUser2SimEX( *nStatus );
    SUL_MemCopy8( pData, &(Date[1]), (CFW_SMS_REC_DATA_SIZE - 1));
    return ERR_SUCCESS;
}


//=================================================================
//SMS_AppendEX
//
//=================================================================

INT32 SMS_GetStorageInfoEX(UINT16 *pUsedSlot, UINT16 *pMaxSlot,
                           CFW_SIM_ID nSimId,
                           UINT8 nStatus)
{
    UINT16  nIndexBuf[CFW_SMS_MAX_REC_COUNT];
    UINT16  nSMSNumber = 0x0000;
    SUL_ZeroMemory32( nIndexBuf, SIZEOF(nIndexBuf));
    nSMSNumber = SMS_DM_GetIndexByStatus( nIndexBuf, nStatus, nSimId);

    *pUsedSlot = nSMSNumber;
    *pMaxSlot  = CFW_SMS_MAX_REC_COUNT / CFW_SIM_COUNT;
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("used=%d, total is %d\r\n",0x08100fb6), nSMSNumber, CFW_SMS_MAX_REC_COUNT);

    return ERR_SUCCESS;
}


//=================================================================
//SMS_HaveFreeSlotEX
//
//=================================================================
INT32 SMS_HaveFreeSlotEX(
    CFW_SIM_ID nSimId
)
{
    UINT16 nIndexBuf[CFW_SMS_MAX_REC_COUNT];
    UINT16 nSMSNumber = 0x0000;
    SUL_ZeroMemory32( nIndexBuf, SIZEOF(nIndexBuf));
    nSMSNumber = SMS_DM_GetIndexByStatus( nIndexBuf, 0xff, nSimId);


    if( nSMSNumber != CFW_SMS_MAX_REC_COUNT)
    {
        return ERR_SUCCESS;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_HaveFreeSlotEX, error nSMSNumber= 0x%x\n",0x08100fb7), nSMSNumber);

        return SMS_NO_FREE_SLOT;
    }
}


//=================================================================
//SMS_AppendEX
//
//=================================================================

UINT32 sms_db_InitEX(void)
{
    CSW_PROFILE_FUNCTION_ENTER( sms_db_Init);
    CSW_PROFILE_FUNCTION_EXIT( sms_db_Init);

    return ERR_SUCCESS ;//(UINT32)SMS_DM_Init();
}

UINT8 SMS_SFSim2UserEX(UINT8 state)
{

    UINT8 nState;
    CSW_PROFILE_FUNCTION_ENTER(SMS_SFSim2UserEX);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_SFSim2UserEX, state = 0x%x\n",0x08100fb8), state);
    switch(state)
    {
        case 0x00:
            nState = 0x00;
            break;
        case 0x03:
            nState = 0x01;
            break;
        case 0x01:
            nState = 0x02;
            break;
        case 0x07:
            nState = 0x04;
            break;
        case 0x05:
            nState = 0x08;
            break;
        case 0x0D:
            nState = 0x10;
            break;
        case 0x15:
            nState = 0x20;
            break;
        case 0x1D:
            nState = 0x40;
            break;
        case 0xA0:
            nState = 0x80;
            break;
        default:
            nState = 0xff;
            break;
    }

    CSW_PROFILE_FUNCTION_EXIT(SMS_SFSim2UserEX);
    return nState;
}

UINT8 SMS_SFUser2SimEX(UINT8 state)
{
    UINT8 nState;
    CSW_PROFILE_FUNCTION_ENTER(SMS_SFUser2SimEX);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_SFUser2SimEX, state = 0x%x\n",0x08100fb9), state);
    switch(state)
    {
        case 0x00:
            nState = 0x00;
            break;
        case 0x01:
            nState = 0x03;
            break;
        case 0x02:
            nState = 0x01;
            break;
        case 0x04:
            nState = 0x07;
            break;
        case 0x08:
            nState = 0x05;
            break;
        case 0x10:
            nState = 0x0D;
            break;
        case 0x20:
            nState = 0x15;
            break;
        case 0x40:
            nState = 0x1D;
            break;
        case 0x80:
            nState = 0xA0;
            break;
        default:
            nState = 0xff;
            break;
    }

    CSW_PROFILE_FUNCTION_EXIT(SMS_SFUser2SimEX);
    return nState;
}





///////////////////////////////////////////////////////////////////////////////
//                         Local ME DB Operation API                         //
///////////////////////////////////////////////////////////////////////////////

// Return success, but invalid index and Flash operation exception.
UINT32 CFW_MeDeleteMessage (
    CFW_SIM_ID nSimId,
    UINT16 nIndex,
    UINT8 nStatus,
    CFW_EV *pEvent
)
{
    UINT32 err_code;
    CSW_PROFILE_FUNCTION_ENTER(CFW_MeDeleteMessage);

    pEvent->nEventId = EV_CFW_SMS_DELETE_MESSAGE_RSP;
    pEvent->nType = 0;
    pEvent->nParam2 = nIndex;
    if(nIndex > (nSimId + 1) * (SMS_ME_MAX_INDEX / (CFW_SIM_COUNT)))
    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CME_INVALID_INDEX;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeDeleteMessage);
        return ERR_SUCCESS;
    }
    else if(nIndex == 0) //Delete the short message(s) pointed by status
    {
        if(ERR_SUCCESS != (err_code = SMS_DeleteFromMeByStatusEX(nStatus, nSimId)))
        {
            if(ERR_DB_NOT_ACCURATE_LOCATE == err_code)
            {
                pEvent->nType = 0x0;
                //pEvent->nParam1 = ERR_CME_ME_FAILURE;
            }
            else
            {
                pEvent->nType = 0xf0;
                pEvent->nParam1 = ERR_CME_ME_FAILURE;
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeDeleteMessage);
            return ERR_SUCCESS;
        }
    }
    else //Delete the short the message pointed by index
    {
        if(ERR_SUCCESS != (err_code = SMS_DeleteFromMeByIndexEX(nIndex, CFW_SMS_STORAGE_ME)))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_MeDeleteMessage, error err_code= 0x%x\n",0x08100fba), err_code);
            if(ERR_DB_NOT_ACCURATE_LOCATE == err_code)
            {
                pEvent->nType = 0x0;
                //pEvent->nParam1 = ERR_CME_ME_FAILURE;
            }
            else
            {
                pEvent->nType = 0xf0;
                pEvent->nParam1 = ERR_CME_ME_FAILURE;
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeDeleteMessage);
            return ERR_SUCCESS;
        }
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_MeDeleteMessage);
    return ERR_SUCCESS;
}

UINT32 CFW_MeWriteMessage (
    CFW_SIM_ID nSimId,
    UINT16 nIndex,
    UINT8 *pData,
    UINT8 nDataSize,
    UINT32 nTime,
    UINT32 nAppInt32,
    CFW_EV *pEvent
)
{
    UINT32 err_code = 0x0;
    UINT16 index = nIndex;
    CSW_PROFILE_FUNCTION_ENTER(CFW_MeWriteMessage);
    CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_MeWriteMessage nIndex= 0x%x\n",0x08100fbb), nIndex);
    nDataSize = nDataSize;

    pEvent->nEventId = EV_CFW_SMS_WRITE_MESSAGE_RSP;
    pEvent->nType = 0;
    pEvent->nParam2 = nIndex;
    if(nIndex > (nSimId + 1) * (SMS_ME_MAX_INDEX / (CFW_SIM_COUNT)))

    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CME_INVALID_INDEX;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeWriteMessage);
        return ERR_SUCCESS;
    }
    else if(nIndex == 0) //Find a free space to save the short message
    {
        if(ERR_SUCCESS != (err_code = SMS_AppendEX(pData, nSimId, &index, nTime, nAppInt32)))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_MeWriteMessage, error err_code= 0x%x\n",0x08100fbc), err_code);
            if(ERR_SMS_DB_SPACE_NULL == err_code)
            {
                pEvent->nType = 0xf0;
                pEvent->nParam1 = ERR_CME_ME_SPACE_FULL;
            }
            else
            {
                pEvent->nType = 0xf0;
                pEvent->nParam1 = ERR_CME_ME_FAILURE;
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeWriteMessage);
            return ERR_SUCCESS;
        }
        if(ERR_SUCCESS == (err_code = SMS_HaveFreeSlotEX(nSimId)))
        {
            pEvent->nParam2 = 0;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_HaveFreeSlotEX, error err_code= 0x%x\n",0x08100fbd), err_code);
            pEvent->nParam2 = 1;
        }
    }
    else //Write the short the message pointed by index
    {
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
        if(ERR_SUCCESS != SMS_WriteEX(nIndex, pData, CFW_SMS_STORAGE_ME, nTime, nAppInt32, nSimId))
#else
        if(ERR_SUCCESS != SMS_WriteEX(nIndex, pData, CFW_SMS_STORAGE_ME, nTime, nAppInt32))
#endif
        {
            pEvent->nType = 0xf0;
            pEvent->nParam1 = ERR_CME_ME_FAILURE;
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeWriteMessage);
            return ERR_SUCCESS;
        }
        if(ERR_SUCCESS == (err_code = SMS_HaveFreeSlotEX(nSimId)))
        {
            pEvent->nParam2 = 0;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SMS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SMS_HaveFreeSlotEX, error err_code= 0x%x\n",0x08100fbe), err_code);
            pEvent->nParam2 = 1;
        }
    }

    pEvent->nParam1 = index;
    CSW_PROFILE_FUNCTION_EXIT(CFW_MeWriteMessage);
    return ERR_SUCCESS;
}

UINT32 CFW_MeReadMessage (
    UINT16 nLocation,
    UINT16 nIndex,
    CFW_EV *pEvent
)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_MeReadMessage);
    pEvent->nEventId = EV_CFW_SMS_READ_MESSAGE_RSP;
    pEvent->nType = 0;
    //pEvent->nParam2 = nIndex;
    if(nIndex <= 0 || nIndex > SMS_ME_MAX_INDEX)
    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CME_INVALID_INDEX;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeReadMessage);
        return ERR_SUCCESS;
    }
    if(ERR_SUCCESS != SMS_ReadEX(nIndex, (UINT8 *)pEvent->nParam1, nLocation))
    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CME_ME_FAILURE;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeReadMessage);
        return ERR_SUCCESS;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_MeReadMessage);
    return ERR_SUCCESS;
}

//----------------------------------------------------------------------------
//parameter:
//            nCount:[in]    0,         only return the count filtered by the status and nSimId
//                  others,     SMS data will be saved in the last parameter pEvent.nParam1
//       nStatus:[in]    this value can be conbinated
//       pEvent:[out]   nParam1 is used to save the output PDU, note that nParam1[0] is the status of
//                   SMS, it's value same as the input param nStatus. this is the difference between the SIM SMS
//-------------------------------------------------------------------------
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
UINT8 g_current_SMS_simID = 0;
#endif
UINT32 CFW_MeListMessage (
    CFW_SIM_ID nSimId,
    UINT8 nStatus,
    UINT16 nCount,
    UINT16 nIndex,
    CFW_EV *pEvent
)
{
    UINT8  status = 0x0;
    UINT8  count = 0;
    UINT16 index = 0;
    UINT8  data[SMS_DEFAULT_SIZE_EX];
    UINT8  *outp = NULL;
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
    UINT8  getSimId=0x00;
#endif

    CSW_PROFILE_FUNCTION_ENTER(CFW_MeListMessage);

    outp =  (UINT8 *)pEvent->nParam1;
    pEvent->nEventId = EV_CFW_SMS_LIST_MESSAGE_RSP;
    pEvent->nType = 0;
    pEvent->nParam2 = 0;

    if(nCount > SMS_ME_MAX_INDEX / CFW_SIM_COUNT)
    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CMS_INVALID_STATUS;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeListMessage);
        return ERR_SUCCESS;
    }

    index = nIndex;

    //      count = SMS_DM_GetIndexByStatus(tmpIndex, nStatus, nSimId);
    //#endif
    UINT16 nStartIndex = 0,  nEndIndex = 0;
    SMS_DM_GetRange(nSimId, &nStartIndex, &nEndIndex) ;
    // index += nStartIndex;
    if(nCount == 0)
    {
        //#ifndef CFW_MULTI_SIM
        UINT8 i = nStartIndex;
        for(i = nStartIndex; i < nEndIndex; i++)
        {
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
            if(ERR_SUCCESS == SMS_Read2EX((UINT16)(i+1), &status, &data[1], CFW_SMS_STORAGE_ME,&getSimId))
#else
            if(ERR_SUCCESS == SMS_Read2EX((UINT16)(i+1), &status, &data[1], CFW_SMS_STORAGE_ME))
#endif
            {
                //status = SMS_SFSim2User(status);
                if((UINT8)0 != (status & nStatus)) // is the required status
                {
                    count++;
                }
            }
        }
        //#endif
        pEvent->nType = 0;
        //pEvent->nParam1 = 0;
        pEvent->nParam2 = count;
        CSW_PROFILE_FUNCTION_EXIT(CFW_MeListMessage);
        return ERR_SUCCESS;
    }
    else
    {
        while((count < nCount) && (index <= nEndIndex))
        {
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
            if(ERR_SUCCESS == SMS_Read2EX(index, &status, &data[1], CFW_SMS_STORAGE_ME,  &getSimId))
#else
            if(ERR_SUCCESS == SMS_Read2EX(index, &status, &data[1], CFW_SMS_STORAGE_ME))
#endif
            {
                UINT8 nTmpStatus = 0x0;

                nTmpStatus = status;
                status = SMS_SFSim2UserEX(status);
                if((UINT8)0 != (status & nStatus)) // is the required status
                {
                    data[0] = nTmpStatus;
                    SUL_MemCopy8((UINT8 *)((UINT8 *)outp + SMS_DEFAULT_SIZE_EX * count), data, SMS_DEFAULT_SIZE_EX);
                    count++;
                }
            }
            index++;
        }
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
        g_current_SMS_simID = getSimId;
#endif

        if(index > (nSimId + 1) * (SMS_ME_MAX_INDEX / (CFW_SIM_COUNT)))
        {
            pEvent->nType = 0;
            //pEvent->nParam1 = 0;
            pEvent->nParam2 = 0x00010000 | count;
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeListMessage);
            return ERR_SUCCESS;
        }
        else
        {
            pEvent->nType = 0;
            // pEvent->nParam1  has been set at the beginning of the function
            // with the code: outp = (UINT8 *)pEvent->nParam1;
            pEvent->nParam2 = index - 1;
            CSW_PROFILE_FUNCTION_EXIT(CFW_MeListMessage);
            return ERR_SUCCESS;
        }
    }
}


UINT32  CFW_MeGetStorageInfo (
    CFW_SIM_ID nSimId,
    UINT8 nStatus,
    CFW_EV *pEvent
)
{
    CFW_SMS_STORAGE_INFO *storageinfo = (CFW_SMS_STORAGE_INFO *)pEvent->nParam1;
    UINT16 usedSlot = 0x0;
    UINT16 maxSlot = 0x0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_MeGetStorageInfo);

    pEvent->nEventId = EV_CFW_SMS_GET_STORAGE_RSP;
    pEvent->nType = 0;

    if(CFW_SMS_STORED_STATUS_STORED_ALL == nStatus)
    {
        nStatus = 0xff; // List all of the records.
    }
    if(ERR_SUCCESS != SMS_GetStorageInfoEX(&usedSlot, &maxSlot, nSimId, nStatus))
    {
        pEvent->nType = 0xf0;
        pEvent->nParam1 = ERR_CMS_UNKNOWN_ERROR;
    }
    else
    {
        storageinfo->storageId = CFW_SMS_STORAGE_ME;
        storageinfo->totalSlot = maxSlot;
        storageinfo->usedSlot = usedSlot;
        pEvent->nType = 0x0;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_MeGetStorageInfo);
    return ERR_SUCCESS;
}

//#endif //#if USING_FLASH_API

