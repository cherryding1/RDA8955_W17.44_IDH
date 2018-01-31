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











#include "cswtype.h"
#include "ts.h"
#include "errorcode.h"
#include "event.h"
#include "cfw.h"
#include "drv_prv.h"
#include "api_msg.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_queu.h"
#include "sxr_mem.h"
#include "string.h"
#include "cfw_prv.h"
#include "dm_audio.h"

#ifdef _DEBUG
#define PRINT(fmt, ...)  do {} while(0)
#else
#define PRINT(fmt, ...) do {} while(0)
#endif
#ifdef __221_UNIT_TEST__
//Lixp add for debug
extern CFW_SIM_ID gSimID;
#endif

CFW_SIM_OC_STATUS SIMOCStatus[CFW_SIM_COUNT] = {CFW_SIM_OC_IDLE, CFW_SIM_OC_IDLE};
//
// Hardware Related functions
//
PRIVATE PFN_AUD_START_SPK_MIC pfnAudStartSpkMic = NULL;
PRIVATE PFN_AUD_STOP_SPK_MIC  pfnAudStopSpkMic  = NULL;

extern PVOID Stack_MsgMalloc(UINT32 nSize);
extern VOID *SUL_MemCopy8 (VOID  *dest, CONST VOID *src, UINT32 count);
extern UINT32 CFW_NwInit();
extern UINT32 SRVAPI CFW_SmsMtInit();
extern UINT32 SRVAPI CFW_SmsCbInit();
extern UINT32 CFW_SsInit();
extern UINT32 CFW_CcInit();
extern UINT32 CFW_GprsInit();



static UINT32 _SendSclMessage(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID)
{

    if((UINT32 *)pMessageBuf != NULL)
    {
        UINT32 *p = (UINT32 *)pMessageBuf;

        *--p = nSimID;
        *--p = nMsgId;

        PRINT("Send Message 2: %x\n", p);
        sxr_Send (p, API_MBX, SXR_SEND_MSG);
    }
    else
    {

        UINT32  nCallerAdd;
        COS_GET_RA(&nCallerAdd);
        UINT32 *p = (UINT32 *)Stack_MsgMalloc( sizeof(UINT32) + sizeof(UINT32) );
        if(p)
        {
            COS_UpdateMemCallAdd(p, nCallerAdd);

            * p  = nMsgId;
            *++p = nSimID;
            PRINT("Send Message 1: %x\n", (p - 1));
            sxr_Send (--p, API_MBX, SXR_SEND_MSG);


        }
        else
            return ERR_CME_MEMORY_FULL;
    }

    return ERR_SUCCESS;
}

//
//8955 chip in the 52M clock scene:
//If the two sim card protocol stack is open,the bandwidth of output trace is not enough,
//here we have to add a special judgment,if the two sim cards are open,then close output
//trace,if not open,or open one sim card protocol stack,then open output trace.
//

BOOL g_CFWStartStopStackFlag[CFW_SIM_COUNT] = {FALSE,FALSE};
extern VOID hal_SysSetActiveSimCount(UINT8 count);

VOID CFW_SetActiveSimCount()
{
    UINT8 i = 0x00;
    UINT8 ActiveSimCount = 0x00;
    for(i = CFW_SIM_0; i < CFW_SIM_COUNT; i++)
    {
        ActiveSimCount+=g_CFWStartStopStackFlag[i];
    }
    hal_SysSetActiveSimCount(ActiveSimCount);
}

UINT32 CFW_SendStartStackREQ(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID)
{
    g_CFWStartStopStackFlag[nSimID] = TRUE;
    CFW_SetActiveSimCount();
    return _SendSclMessage(nMsgId, pMessageBuf, nSimID);
}

VOID CFW_ReceiveStopStackCNF(CFW_SIM_ID nSimID)
{
    g_CFWStartStopStackFlag[nSimID] = FALSE;
    CFW_SetActiveSimCount();
    return ;
}

static UINT32 _SendSIMOpenCloseMessage(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID)
{
    switch(SIMOCStatus[nSimID])
    {
        case CFW_SIM_OC_IDLE:
            if ( API_SIM_OPEN_IND == nMsgId )
            {
                SIMOCStatus[nSimID] = CFW_SIM_OC_OPEN;
            }
            else
            {
                return ERR_SUCCESS;
            }
            break;

        case CFW_SIM_OC_OPEN:
            if ( API_SIM_CLOSE_IND == nMsgId )
            {
                SIMOCStatus[nSimID] = CFW_SIM_OC_CLOSE;
            }
            else
            {
                return ERR_SUCCESS;
            }
            break;

        case CFW_SIM_OC_CLOSE:
            if ( API_SIM_OPEN_IND == nMsgId )
            {
                SIMOCStatus[nSimID] = CFW_SIM_OC_OPEN;
            }
            else
            {
                return ERR_SUCCESS;
            }
            break;

        default:
            return ERR_INVALID_PARAMETER;
            break;
    }

    CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(1), TSTXT(TSTR("_SendSIMOpenCloseMessage nSimID:0x%x \n",0x08100129)), nSimID);

    return _SendSclMessage(nMsgId, pMessageBuf, nSimID);
}
#include "dbg.h"

#ifndef __221_UNIT_TEST__
//extern BOOL gSimDropInd[];

UINT32 CFW_SendSclMessage(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID)
{
    {
        //CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SendSclMessage nSimID:0x%x \n",0x0810012a)), nSimID);
        // COS_Sleep(10);
        CSW_TRACE(_CSW|TLEVEL(1)|TDB|TNB_ARG(3), TSTXT(TSTR("CFW_SendSclMessage nSimID:0x%x  0x%x 0x%x\n",0x0810012b)), nSimID , nMsgId, API_SIM_OPEN_IND);

        if((CFW_SIM_COUNT < nSimID) || (CFW_SIM_0 > nSimID))
        {
            DBG_ASSERT(0, "CFW_SendSclMessage %d", nSimID);
        }
    }
    if( ( API_SIM_OPEN_IND == nMsgId ) || ( API_SIM_CLOSE_IND == nMsgId ))
    {
        return _SendSIMOpenCloseMessage(nMsgId, pMessageBuf, nSimID);
    }
    else if(  API_START_REQ == nMsgId )
    {
        CFW_SendStartStackREQ(nMsgId, pMessageBuf, nSimID);
    }
    else
    {
        //if( gSimDropInd[nSimID] && (nMsgId != API_SIM_RESET_REQ))
        //  return ERR_INVALID_TYPE;
        return _SendSclMessage(nMsgId, pMessageBuf, nSimID);
    }
    return ERR_SUCCESS;
}

//
// Allocate memory buffer used by CFW_SendSclMessage.
// Only use this function for CFW_SendSclMessage.
// Don't free this memory buffer, it will automatically free by received task(API Task).
//
PVOID CFW_MemAllocMessageStructure (
    UINT16 nSize // specified allocated block size bytes long
)
{
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    if(nSize == 0)
        return NULL;
    else
    {

        PUINT32 p = (PUINT32)Stack_MsgMalloc(nSize + 4 + 4);
        COS_UpdateMemCallAdd(p, nCallerAdd);

        PRINT("Allocate Buffer: %x\n", p);
        p ++;
        p ++;

        return p;
    }
}

//UINT8 nNsapi, the NSAPI of the data
//UINT8 nQueueId, the index of the queue that stack used
//BOOL bMode, the mode of the data:
// Possible values for Mode
// API_PDP_ACK   0
//API_PDP_UNACK 1
//UINT16 nDataSize the data size
//UINT8 *pData point to the message data

//#define CMN_SET_SIM_ID(EventId, SimId)      ((EventId) | ((SimId) << 29))
UINT32 CFW_SendSclData(
    UINT8 nNsapi,
    UINT8 nQueueId,
    BOOL bMode,
    UINT16 nDataSize,
    UINT8 *pData,
    CFW_SIM_ID nSimID
)
{
    // For internal use
    //#define sxr_PopIn(Data, Queue) sxr_QueueLast(Data, Queue)
    //#define sxr_PopOut(Queue)      sxr_QueueRmvFirst(Queue)

    //Msg_t           *psMsg     = NULL;
    UINT32 *psMsg = NULL;

    api_PSDataReq_t *psDataMsg = NULL;
    UINT32 Signal[4] = {0, };

    Signal[0] = CMN_SET_SIM_ID(API_PS_QUEUEDATA_REQ, nSimID);
    API_PSALLOC_DATAREQ(psMsg, nDataSize);

    // Prepare and send a packet to SNDCP
    *psMsg++ = SND_SEND_REQ;
    *psMsg++ = nSimID;

    //psDataMsg           = (api_PSDataReq_t*)&(psMsg->B);
    psDataMsg             = (api_PSDataReq_t *)psMsg;
    psDataMsg->DataLen    = API_PSDATA_LEN(nDataSize);
    psDataMsg->Ctrl.Mode  = bMode;
    psDataMsg->Ctrl.NSAPI = nNsapi;

    SUL_MemCopy8(&psDataMsg->UserData[psDataMsg->DataOffset], pData, nDataSize);

    psMsg -= 2;

    sxr_QueueLast(psMsg, nQueueId);
    sxr_Send ((void *)Signal, API_MBX, SXR_SEND_EVT);

    return ERR_SUCCESS;
}

UINT32 CFW_BalSendMessage(
    UINT32 nMailBoxId,
    UINT32 nEventId,
    VOID *pData,
    UINT32 nSize,
    CFW_SIM_ID nSimID
)

{
    UINT32 *p = NULL;
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    p = Stack_MsgMalloc( 4 + 4 + nSize );
    if(p)
    {
        COS_UpdateMemCallAdd(p, nCallerAdd);

        p[0] = nEventId;
        p[1] = nSimID;
        if(pData && nSize)
        {
            memcpy((char *)&p[2], pData, nSize);
        }
        sxr_Send ((void *)p, nMailBoxId, 0); //SXR_SEND_MSG
        return ERR_SUCCESS;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("\n malloc fail: size=%d \n\n",0x0810012c), 4 + nSize);
        return ERR_NO_MORE_MEMORY;
    }
}
#else

UINT32 CFW_SendSclMessage(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID)
{
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    gSimID = nSimID;
    CSW_TRACE(_CSW|TLEVEL(30)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SendSclMessage gSimID:0x%x \n",0x0810012d)), gSimID);
    if((UINT32 *)pMessageBuf != NULL)
    {
        UINT32 *p = (UINT32 *)pMessageBuf;

        *--p = nSimID;
        *--p = nMsgId;

        PRINT("Send Message 2: %x\n", p);
        sxr_Send (p, API_MBX, SXR_SEND_MSG);
    }
    else
    {
        UINT32 *p = (UINT32 *)Stack_MsgMalloc( sizeof(UINT32) + sizeof(UINT32) );
        if(p)
        {
            COS_UpdateMemCallAdd(p, nCallerAdd);

            * p  = nMsgId;
            *++p = nSimID;
            PRINT("Send Message 1: %x\n", (p - 1));
            sxr_Send (--p, API_MBX, SXR_SEND_MSG);


        }
        else
            return ERR_CME_MEMORY_FULL;


    }

    return ERR_SUCCESS;
}

//
// Allocate memory buffer used by CFW_SendSclMessage.
// Only use this function for CFW_SendSclMessage.
// Don't free this memory buffer, it will automatically free by received task(API Task).
//
PVOID CFW_MemAllocMessageStructure (
    UINT16 nSize // specified allocated block size bytes long
)
{
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    if(nSize == 0)
        return NULL;
    else
    {
        PUINT32 p = (PUINT32)Stack_MsgMalloc(nSize + 4 + 4);

        COS_UpdateMemCallAdd(p, nCallerAdd);

        PRINT("Allocate Buffer: %x\n", p);
        p ++;
        p ++;

        return p;
    }
}

//UINT8 nNsapi, the NSAPI of the data
//UINT8 nQueueId, the index of the queue that stack used
//BOOL bMode, the mode of the data:
// Possible values for Mode
// API_PDP_ACK   0
//API_PDP_UNACK 1
//UINT16 nDataSize the data size
//UINT8 *pData point to the message data


UINT32 CFW_SendSclData(
    UINT8 nNsapi,
    UINT8 nQueueId,
    BOOL bMode,
    UINT16 nDataSize,
    UINT8 *pData
)
{
    // For internal use
#define sxr_PopIn(Data, Queue) sxr_QueueLast(Data, Queue)
#define sxr_PopOut(Queue)      sxr_QueueRmvFirst(Queue)

    //Msg_t           *psMsg     = NULL;
    UINT32 *psMsg = NULL;

    api_PSDataReq_t *psDataMsg = NULL;
    UINT32 Signal[4] = {0, };

    Signal[0] = API_PS_QUEUEDATA_REQ;

    API_PSALLOC_DATAREQ(psMsg, nDataSize);

    // Prepare and send a packet to SNDCP
    *psMsg++ = SND_SEND_REQ;

    //psDataMsg           = (api_PSDataReq_t*)&(psMsg->B);
    psDataMsg             = (api_PSDataReq_t *)psMsg;
    psDataMsg->DataLen    = API_PSDATA_LEN(nDataSize);
    psDataMsg->Ctrl.Mode  = bMode;
    psDataMsg->Ctrl.NSAPI = nNsapi;

    SUL_MemCopy8(&psDataMsg->UserData[psDataMsg->DataOffset], pData, nDataSize);

    psMsg --;

    sxr_PopIn(psMsg, nQueueId);
    sxr_Send ((void *)Signal, API_MBX, SXR_SEND_EVT);

    return ERR_SUCCESS;
}

UINT32 CFW_BalSendMessage(
    UINT32 nMailBoxId,
    UINT32 nEventId,
    VOID *pData,
    UINT32 nSize,
    CFW_SIM_ID nSimID
)

{
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    gSimID = nSimID;
    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_BalSendMessage gSimID:0x%x \n",0x0810012e)), gSimID);
    UINT32 *p = NULL;

    p = Stack_MsgMalloc( 4 + 4 + nSize );
    if(p)
    {
        COS_UpdateMemCallAdd(p, nCallerAdd);

        p[0] = nEventId;
        p[1] = nSimID;
        if(pData && nSize)
        {
            memcpy((char *)&p[2], pData, nSize);
        }
        sxr_Send ((void *)p, nMailBoxId, 0); //SXR_SEND_MSG
        return ERR_SUCCESS;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("\n malloc fail: size=%d \n\n",0x0810012f), 4 + nSize);
        return ERR_NO_MORE_MEMORY;
    }
}
#endif
//
// pMessagBuf should be used via CFW_MemAllocMessageStructure(..).
// CAUTION: Don't used any others funtion to allocate memory for this parameter!
//


UINT32 CFW_BaseInit()
{
#ifdef MF32_X86_PLATFORM
    BS32_Init();
#endif
    return 0;
}

BOOL CFW_BalAudCallStart()
{
    if(pfnAudStartSpkMic == NULL)
    {
        DM_StartAudio(); // Default Audio operations.
    }
    else if( (UINT32)pfnAudStartSpkMic == 0xFFFFFFFF) // Send Event to upper-layer APP.
    {
        return FALSE;
    }
    else
    {
        pfnAudStartSpkMic();
    }

    return TRUE;
}

BOOL CFW_BalAudCallStop()
{
    if(pfnAudStopSpkMic == NULL)
    {
        DM_StopAudio(); // Default Audio operations.
    }
    else if( (UINT32)pfnAudStopSpkMic == 0xFFFFFFFF) // Send Event to upper-layer APP.
    {
        return FALSE;
    }
    else
    {
        pfnAudStopSpkMic();
    }

    return TRUE;
}

//
// Initialize all Services of CFW.
//
extern VOID CFW_CbInit(CFW_SIM_ID nSimID);
UINT32 CFW_ServicesInit()
{
    CFW_SIM_ID nSimID;
#ifdef LTE_NBIOT_SUPPORT
    //CFW_SIM_ID nSimID;
    CFW_EQOS nEqos = {0,0,0,0,0};
    //for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID ++)
    //{
	CFW_SimInit(1, CFW_SIM_0);
	COS_Sleep(100);
	//CFW_GprsSendCtx(1/*cid*/,nEqos,CFW_SIM_0);
    CFW_GprsSendDefaultCtx(1/*cid*/,nEqos,CFW_SIM_0);
    //}
	CFW_NwInit();
	
#else   
    CFW_NwInit();


#ifdef DISABLE_SECOND_SIM
    hal_HstSendEvent(0xB0070000 | DEFAULT_SIM_SLOT);
    CFW_SimInit(1, DEFAULT_SIM_SLOT);
#else
    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID ++)
    {
        CFW_SimInit(1, nSimID);
    }
#endif
#endif //LTE_NBIOT_SUPPORT
    CFW_SmsMtInit();
#ifdef CB_SUPPORT

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        CFW_CbInit(nSimID);
    }
#endif
    //CFW_SmsCbInit();
    CFW_SsInit();
    CFW_CcInit();

#ifdef CFW_GPRS_SUPPORT
    CFW_GprsInit();
#endif
    return ERR_SUCCESS;
}

//
// Exit all Services of CFW.
//
UINT32 CFW_ServicesExit()
{
    pfnAudStartSpkMic = NULL;
    pfnAudStopSpkMic  = NULL;
    return TRUE;
}

//
// BAL Interface for CFW.
//

//
// Setup HW related Callback functions.
//
BOOL CFW_BalSetupHwCbs(CFW_PFN_AUD_CBS *pHwFun)
{
    if(pHwFun == (CFW_PFN_AUD_CBS *)0xFFFFFFFF)
    {

    }
    else if ( pHwFun == (CFW_PFN_AUD_CBS *)NULL )
    {

    }
    else
    {
        pfnAudStartSpkMic = pHwFun->m_pfnAudStartSpkMic;
        pfnAudStopSpkMic  = pHwFun->m_pfnAudStopSpkMic;
    }

    return TRUE;
}

#ifdef LTE_NBIOT_SUPPORT

extern UINT16 appUlNumDiscard;
extern UINT32 appUlSizeDiscard;
extern UINT16 appTtlUlNumDiscard;
extern UINT32 appTtlUlSizeDiscard;

extern void tgl00_165ApplyUlIpData(u8 **po, u16 size, u8 UseRsvFlag);
extern void tgl00_163ApplyUlNode(T_ComPduLte **pt);
UINT32 CFW_SendPduData(
    UINT8 nNsapi,
    UINT16 nDataSize,
    UINT8 *pData,
    UINT8 rai,
    UINT16 type_of_user_data,
    CFW_SIM_ID nSimID,
    UINT8 v_dataNum
)
{
    T_ComPduLte        *p_ulDataNode = NULL;
    T_ComPduLte        *p_ulDataListHead = NULL;
    T_ComPduLte        *p_ulDataListTail = NULL;
    u8                 *p_ulData = NULL;

    api_PSDataReqLte_t *pOutMsg = NULL;

   // CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), "CFW_SendPduData: nNsapi=%d,nDataSize=%d, rai=%d",nNsapi,nDataSize,rai);

    tgl00_165ApplyUlIpData(&(p_ulData), nDataSize, FALSE); 
    if (NULL == p_ulData)
    {
    
        appUlNumDiscard++;
        appUlSizeDiscard += nDataSize;
        appTtlUlNumDiscard++;
        appTtlUlSizeDiscard += nDataSize;
        
        //CSW_TRACE(_CSW|TLEVEL(CSW_TS_ID)|TDB|TNB_ARG(1), TSTR("flow ctrol, discard ul ip data, size=%d",0x0810012c), nDataSize);
        return ERR_SUCCESS;
    }
    
    tgl00_163ApplyUlNode(&p_ulDataNode);

    //currently v_dataNum should be 1, to be optimized in CSW later
    while (v_dataNum)
    {
        if(NULL == p_ulDataListHead)
        {
          p_ulDataListHead = p_ulDataNode;
        }
        else
        {
          p_ulDataListTail->pNext = p_ulDataNode;
        }
        p_ulDataListTail = p_ulDataNode;    
        p_ulDataNode->pFreeQE = p_ulData;
        p_ulDataNode->pdcpHeaderLen = DBA_RESERVED_UL_OFFSET;
        p_ulDataNode->pPduData = p_ulData + DBA_RESERVED_UL_OFFSET;
        p_ulDataNode->pduLen = nDataSize;
	    p_ulDataNode->rai = rai;
	    p_ulDataNode->type_of_user_data = type_of_user_data;
        p_ulDataNode->pNext = NULL;
        v_dataNum--;
        SUL_MemCopy8(p_ulDataNode->pPduData, pData, nDataSize);
    }
    pOutMsg = (api_PSDataReqLte_t *)CFW_MemAllocMessageStructure(SIZEOF(api_PSDataReqLte_t));
    pOutMsg->nsapi = nNsapi;
    pOutMsg->pUserData = p_ulDataListHead;


    // send the message to stack
    CFW_SendSclMessage(API_PSDATA_REQ_LTE, pOutMsg, nSimID);
    return ERR_SUCCESS;
   
}

#endif

