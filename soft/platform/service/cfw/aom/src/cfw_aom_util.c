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
#include "cfw.h"
#include "sul.h"
#include "cfw_prv.h"
#include "rfm.h"
#include "ts.h"
#include "csw_debug.h"
#ifdef _URNDIS_
#include "uctls_m.h"
#include "urndis_data_m.h"
#endif
#define GET_0TO7_BITS(x)        (x & 0x000000FF)        // TI from Stack or other device
#define GET_8TO23_BITS(x)       (((x)&0x00FFFF00)>>8)   // For Internal CFW or APP

#define GET_STACK_UTI(x)    GET_0TO7_BITS(x)
#define GET_APP_CFW_UTI(x)  GET_8TO23_BITS(x)

static PFN_CFW_EV_PORC pfn_NotifyProc[CFW_SRV_EV_MAX + 4];
static void *pNotifyProcUserData[CFW_SRV_EV_MAX + 4];

extern BOOL hal_HstSendEvent(UINT32 ch);

UINT32 CallNotifyFun( CFW_EV *pEvent );

typedef struct _CB_FUN_FOR_UTI
{
    UINT32                  UTI;
    void                    *UserData;
    PFN_CFW_EV_PORC         pfnEvProc;
    struct _CB_FUN_FOR_UTI  *Next;
    struct _CB_FUN_FOR_UTI  *Prev;

} CB_FUN_FOR_UTI;


typedef struct _QUEUE_INSTANCE
{
    UINT8 _QUEUE_DATA[20];
} QUEUE_INSTANCE;

typedef struct _QUEUE_MSG_INTERNAL
{
    UINT32  msg_num;
    UINT32  in_index;
    UINT32  out_index;
    UINT32  queue_max;
    UINT8 *pMsgBuffer;
} QUEUE_MSG_INTERNAL;

#define MAX_MSG_NUM   50
PRIVATE char message_buffer[sizeof(CFW_EV) * MAX_MSG_NUM];

PRIVATE QUEUE_INSTANCE g_hCfwQueue;

static CB_FUN_FOR_UTI *UTI_CB_HEAD;
static CB_FUN_FOR_UTI *UTI_CB_TAIL;

UINT32 CFW_Init(VOID)
{
    QUEUE_MSG_INTERNAL *pQ = (QUEUE_MSG_INTERNAL *)&g_hCfwQueue;

    // Initialize System-related routines such as initialize DDI-related information when need.
    CFW_BaseInit();
    //    TM_PowerOn();

    SUL_ZeroMemory8(message_buffer, SIZEOF(CFW_EV)*MAX_MSG_NUM);
    SUL_ZeroMemory8(pQ, SIZEOF(QUEUE_MSG_INTERNAL));
    pQ->queue_max    = MAX_MSG_NUM;
    pQ->pMsgBuffer   = message_buffer;

    /*Initialize notify event proc*/
    SUL_ZeroMemory32(pfn_NotifyProc, SIZEOF(pfn_NotifyProc) );
    SUL_ZeroMemory32(pNotifyProcUserData, SIZEOF(pNotifyProcUserData) );

    // Initialize AOM.
    CFW_AoInit();

    CFW_RegSysDefaultProc(CFW_ShellProc);

    // Initialize variable UTI_CB_HEAD & UTI_CB_TAIL.
    UTI_CB_HEAD = 0;
    UTI_CB_TAIL = 0;

    // Init Each CFW Service.
    CFW_ServicesInit();
    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_Init invoked\n",0x08100063));

    return ERR_SUCCESS;
}

#ifdef LTE_NBIOT_SUPPORT
UINT32 CFW_Exit(UINT8 detachType)
#else
UINT32 CFW_Exit(VOID)
#endif
{
    UINT8 sim_index = 0;
#ifndef LTE_NBIOT_SUPPORT
    for(sim_index = 0; sim_index < CFW_SIM_COUNT; sim_index++)
#endif
    {
        CFW_NwDeRegister(255 - sim_index, 
#ifdef LTE_NBIOT_SUPPORT
			detachType,
#else
			TRUE,
#endif
		(CFW_SIM_ID)sim_index) ;
    }
    //  CFW_ServicesExit();
    //  CFW_AoExit();

    return ERR_SUCCESS;
}

BOOL CFW_GetEventQueue(CFW_EV *msg)
{
    QUEUE_MSG_INTERNAL *pQIns = (QUEUE_MSG_INTERNAL *)&g_hCfwQueue;
    CFW_EV *queue          = NULL;

    if( pQIns->msg_num == 0 )
    {
        //CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_GetEventQueue Null\n",0x08100064));
        return FALSE;
    }

    queue = (CFW_EV *)&pQIns->pMsgBuffer[sizeof(CFW_EV) * pQIns->out_index];

    SUL_MemCopy32(msg, queue, sizeof(CFW_EV));
#ifdef ENABLE_PAGESPY
    COS_PageProtectDisable(COS_PAGE_NUM_4);
#endif
    pQIns->msg_num--;
#ifdef ENABLE_PAGESPY
    COS_PageProtectSetup(COS_PAGE_NUM_4, COS_PAGE_WRITE_TRIGGER, (UINT32)(&(pQIns->msg_num)), (UINT32)((UINT8*)(&(pQIns->msg_num))+4));
    COS_PageProtectEnable(COS_PAGE_NUM_4);
#endif

    pQIns->out_index++;
    if( pQIns->out_index >= pQIns->queue_max ) /*Wrap*/
        pQIns->out_index = 0;

    return TRUE;
}

BOOL CFW_ResetEventQueue(VOID)
{
    QUEUE_MSG_INTERNAL *pQIns = (QUEUE_MSG_INTERNAL *)&g_hCfwQueue;
    pQIns->msg_num   = 0;
    pQIns->in_index  = 0;
    pQIns->out_index = 0;

    return TRUE;
}

//
//
//
static UINT8 WhichNotifyService(UINT32 ID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("WhichNotifyService ID : 0x%x \n",0x08100065), ID);

    if ( ID > EV_CFW_NW_RSP_BASE && ID < EV_CFW_NW_RSP_END_ )
    {
        // NW
        return CFW_SRV_NW_EV_RSP;
    }
    else if ( ID > EV_CFW_CC_RSP_BASE && ID < EV_CFW_CC_RSP_END_ )
    {
        // CM
        return CFW_SRV_CC_EV_RSP;
    }
    else if ( ID > EV_CFW_SS_RSP_BASE && ID < EV_CFW_SS_RSP_END_ )
    {
        // SS
        return CFW_SRV_SS_EV_RSP;

    }
    else if ( ID > EV_CFW_SIM_RSP_BASE && ID < EV_CFW_SIM_RSP_END_ )
    {
        // SIM
        return CFW_SRV_SIM_EV_RSP;
    }
    else if ( ID > EV_CFW_SMS_RSP_BASE && ID < EV_CFW_SMS_RSP_END_ )
    {
        // SMS
        return CFW_SRV_SMS_EV_RSP;
    }
    else if (( ID == EV_CFW_GPRS_DATA_IND ) ||
             ( ID == EV_CFW_GPRS_ACT_RSP ) ||
             ( ID == EV_CFW_GPRS_ATT_RSP ) ||
             (ID == EV_CFW_GPRS_CXT_DEACTIVE_IND) ||
             (EV_CFW_GPRS_CTRL_RELEASE_IND == ID))
    {
        // GPRS
        return CFW_SRV_GPRS_EV_RSP;

    }
    else if ( ID > EV_CFW_IND_BASE && ID < EV_CFW_IND_END_ )
    {
        // IND
        return CFW_SRV_EV_IND;

    }
    else
    {
        return CFW_SRV_EV_UNKNOW;
    }


}

//
//
//
UINT32 SRVAPI CFW_SetEventProc (
    UINT8 nServiceId,
    PVOID pUserData,
    PFN_CFW_EV_PORC pfnEvProc)
{
    if( nServiceId > CFW_SRV_EV_MAX )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR ! CFW_SetEventProc nServiceId out off range \n",0x08100066));
        return -1;
    }

    pfn_NotifyProc[nServiceId] = pfnEvProc;

    pNotifyProcUserData[nServiceId] = pUserData;

    return ERR_SUCCESS;
}

//=====================================================================
//
//
//=====================================================================
static CB_FUN_FOR_UTI *MatchExistingUTI(UINT16 nUTI)
{
    CB_FUN_FOR_UTI *pTmp = 0;

    if( UTI_CB_HEAD == 0 )
        return (CB_FUN_FOR_UTI *)0;

    pTmp = UTI_CB_HEAD;

    do
    {
        if(pTmp->UTI == nUTI)
            return pTmp;
        else
        {
            pTmp = pTmp->Next;
        }

    }
    while( pTmp );

    return (CB_FUN_FOR_UTI *)0;

}



//=====================================================================
//
//
//=====================================================================
UINT32 SRVAPI CFW_SetAoEventNotifyProcEx (
    UINT16             nUTI,
    PVOID              pUserData,
    PFN_CFW_EV_PORC pfnEvProc)
{
    CB_FUN_FOR_UTI *pNew = 0;
    CB_FUN_FOR_UTI *pTmp = 0;

    if((nUTI < CFW_UTI_MIN) || (nUTI > CFW_UTI_MAX))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR ! CFW_SetAoEventNotifyProcEx nUTI out off range \n",0x08100067));

        return ERR_CFW_UTI_INVALID;
    }

    //Check if there is a same nUTI number in existing CB table.
    pTmp = MatchExistingUTI(nUTI);

    if ( (pTmp) && (!pfnEvProc) )
    {
        //Delete this node
        if(pTmp->Prev)
            (pTmp->Prev)->Next = pTmp->Next;
        else
            UTI_CB_HEAD = pTmp->Next;

        if(pTmp->Next)
            (pTmp->Next)->Prev = pTmp->Prev;
        else
            UTI_CB_TAIL = pTmp->Prev;

        CSW_AOM_FREE(pTmp);

        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("FREE the UTI_CB NODE  %x\n",0x08100068)), (UINT32)pTmp);
    }
    else if( (pTmp) && (pfnEvProc) )
    {
        //Update this node
        pTmp->UTI = nUTI;
        pTmp->UserData = pUserData;
        pTmp->pfnEvProc = pfnEvProc;
    }
    else if( (!pTmp) && (pfnEvProc) )
    {
        //Must be a new one
        pNew = (CB_FUN_FOR_UTI *)CSW_AOM_MALLOC( sizeof(CB_FUN_FOR_UTI) );

        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("MALLOC the UTI_CB NODE  %x\n",0x08100069)), (UINT32)pNew);

        if(!pNew)
            return ERR_NO_MORE_MEMORY;

        pNew->UTI = nUTI;
        pNew->UserData = pUserData;
        pNew->pfnEvProc = pfnEvProc;

        pNew->Prev = UTI_CB_TAIL;
        pNew->Next = 0;

        if(UTI_CB_TAIL)
            UTI_CB_TAIL->Next = pNew;
        else
            UTI_CB_HEAD = pNew;

        UTI_CB_TAIL = pNew;
    }

    return ERR_SUCCESS;
}

UINT32 CFW_PostGPRSNotifyEvent (UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nTransId, UINT8 nType, COS_CALLBACK_FUNC_T func)
{
    UINT32 ret = ERR_SUCCESS;

    COS_EVENT Event;
    UINT16 nUTI;
    UINT8 nFlag;
    
    nUTI = (nTransId & 0x00FFFF00) >> 8;
    nFlag    = (UINT8)(nTransId >> 24);

    Event.nEventId = nEventId;
    Event.nParam1  = nParam1;
    Event.nParam2  = nParam2;
    Event.nParam3 =  MAKEUINT32(MAKEUINT16(nFlag, nType),nUTI);
    SXS_TRACE(0,"CFW_PostGPRSNotifyEvent func=%x,nEventId=%d\n", func,nEventId);
#ifdef CFW_TCPIP_SUPPORT    
    if (BAL_ApsTaskTcpipProc(&Event))
#endif
    {
        if (func == NULL) {
            COS_SendEvent(COS_GetDefaultMmiTaskHandle(nUTI), &Event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        } else {
            COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
            ev->nEventId =  nEventId;
            ev->nParam1  = nParam1;
            ev->nParam2  = nParam2;
            ev->nParam3 =  Event.nParam3;
            COS_TaskCallback(COS_GetDefaultMmiTaskHandle(nUTI),func,ev);
        }
    }
}
UINT32 CFW_PostNotifyEvent (UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nTransId, UINT8 nType)
{
    UINT32 ret = ERR_SUCCESS;
    QUEUE_MSG_INTERNAL *pQIns = (QUEUE_MSG_INTERNAL *)&g_hCfwQueue;
    CFW_EV *queue          = NULL;

    CFW_EV Event;
    CFW_EVENT   ExterEv;

    UINT32 APP_CFW_UTI   = GET_APP_CFW_UTI(nTransId);
    CSW_PROFILE_FUNCTION_ENTER(CFW_PostNotifyEvent);

    if(EV_CFW_SIM_READ_RECORD_RSP == nEventId)
    {
        UINT32 nCallerAdd;
        COS_GET_RA(&nCallerAdd);
        hal_HstSendEvent(0xfa022210);
        hal_HstSendEvent(nParam1);
        hal_HstSendEvent(nCallerAdd);
    }
    // Form event structure for function call...
    Event.nEventId = nEventId;
    Event.nParam1  = nParam1;
    Event.nParam2  = nParam2;
    Event.nTransId = nTransId;
    Event.nType    = nType;
    Event.nFlag    = (UINT8)(nTransId >> 24);


    //************************************************************************
    //Notify Event ==> Function Call
    //************************************************************************
    Event.nTransId = (Event.nTransId & 0x00FFFF00) >> 8;

    ExterEv.nEventId = Event.nEventId;
    ExterEv.nParam1  = Event.nParam1;
    ExterEv.nParam2  = Event.nParam2;
    ExterEv.nType    = Event.nType;
    ExterEv.nUTI     = (UINT16)Event.nTransId;
    ExterEv.nFlag    = Event.nFlag;
    CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_PostNotifyEvent  APP_CFW_UTI=%x\n",0x0810006a)), APP_CFW_UTI);
    if (nEventId == EV_CFW_GPRS_ATT_RSP)
        asm("break 1");
    if((APP_CFW_UTI >= CFW_UTI_MIN) && (APP_CFW_UTI <= CFW_UTI_MAX))
    {
        // Post to CFW.
        //Check if this UTI value has been registered in CB function table
        CB_FUN_FOR_UTI *pFunCBUTI = 0;
        pFunCBUTI = MatchExistingUTI((UINT16)APP_CFW_UTI);

        if(pFunCBUTI)
        {
            //Implement Event Proc(Call Back function)
            pFunCBUTI->pfnEvProc(&ExterEv, pFunCBUTI->UserData);

            //Free memory
            CSW_AOM_FREE(Event.nParam1);

            CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("FREE the Param1 of EVENT  %x\n",0x0810006b)), (UINT32)Event.nParam1);

            CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);
            return ERR_SUCCESS;

        }

        //Check if this AO has been registered in CB function table
        Event.nTransId = nTransId;
        ret = CallNotifyFun(&Event);
        if(ret == ERR_SUCCESS)
        {
            CSW_AOM_FREE(Event.nParam1);

            CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("FREE the Param1 of EVENT  %x\n",0x0810006c)), (UINT32)Event.nParam1);

            CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);
            return ERR_SUCCESS;
        }

    }
    else if((APP_CFW_UTI >= APP_UTI_MIN) && (APP_CFW_UTI <= APP_UTI_MAX))
    {
        // Post to MMI/APP.
        UINT8 nServiceId = 0;
        nServiceId = WhichNotifyService(nEventId);

        if(nServiceId != CFW_SRV_EV_UNKNOW)
        {
            #ifdef _URNDIS_
            if(nEventId == EV_CFW_GPRS_DATA_IND && uctls_rndis_is_used())
            {
                BAL_Run_Rndis(&ExterEv);
                return ERR_SUCCESS;
            }
            else
            #endif
            {
                if(pfn_NotifyProc[nServiceId])
                {
                    ret = pfn_NotifyProc[nServiceId] (&ExterEv, pNotifyProcUserData[nServiceId]);

#if 0
                    if( ( Event.nParam1 > 0x80000000 ) &&
                            ( !(ret & BAL_PROC_DONT_FREE_MEM) ) )
                    {
                        CSW_AOM_FREE(Event.nParam1);

                        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("FREE the Param1 of EVENT  %x\n",0x0810006d)), (UINT32)Event.nParam1);
                    }
#endif
                    CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);
                    return ERR_SUCCESS;
                }
            }

        }

    }
    else if((APP_CFW_UTI >= APP_EX_UTI_MIN) && (APP_CFW_UTI <= APP_EX_UTI_MAX))
    {
        ///
        //Dispatch the Notify Event to an extended APP.
        //
        CSW_AOM_FREE(Event.nParam1);

        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("FREE the Param1 of EVENT  %x\n",0x0810006e)), (UINT32)Event.nParam1);

        CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);
        return ERR_SUCCESS;
    }


    //************************************************************************
    // Otherwise, manage to process the Queue
    //************************************************************************

    if( pQIns->msg_num >= pQIns->queue_max)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_AOM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ERROR ! CFW_PostNotifyEvent queue full \n",0x0810006f));
        CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);
        return ERR_CFW_QUEUE_FULL;
    }

    queue = (CFW_EV *)&pQIns->pMsgBuffer[sizeof(CFW_EV) * pQIns->in_index];

    queue->nEventId = nEventId;
    queue->nParam1  = nParam1;
    queue->nParam2  = nParam2;
    queue->nTransId = nTransId;
    queue->nType    = nType;

    queue->nFlag = Event.nFlag ;
    TS_OutPutEvId(Event.nFlag, nEventId);//Hameina[+] 2009.4.8 , output trace
#ifdef ENABLE_PAGESPY
    COS_PageProtectDisable(COS_PAGE_NUM_4);
#endif
    (pQIns->msg_num)++;
#ifdef ENABLE_PAGESPY
    COS_PageProtectSetup(COS_PAGE_NUM_4, COS_PAGE_WRITE_TRIGGER, (UINT32)(&(pQIns->msg_num)), (UINT32)((UINT8*)(&(pQIns->msg_num))+4));
    COS_PageProtectEnable(COS_PAGE_NUM_4);
#endif
    (pQIns->in_index)++;
    if(pQIns->in_index >= pQIns->queue_max) //Wrap
        pQIns->in_index = 0;
    CSW_PROFILE_FUNCTION_EXIT(CFW_PostNotifyEvent);

    return ERR_SUCCESS;
}
