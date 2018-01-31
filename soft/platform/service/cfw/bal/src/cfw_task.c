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










#include <cfw.h>
#include <event.h>
#include <cfw_prv.h>
#include "api_msg.h"
#include <event_prv.h>
#include "sxs_cfg.h"
#include "cmn_defs.h"
#include "sul.h"
#include "dm.h"

//martin add(should remove to correct H file later)
#define HVY_TIMER_IN             ( 0xFFFFF000          )
//UINT32 CSW_SAT_Init(void* pEvt);
UINT32 CSW_SAT_InitEX(CFW_EV *pEV);
//
// CFW Task entry.
//
UINT8 nQueueid[CFW_SIM_COUNT];


extern UINT32 CFW_Init(VOID);
extern BOOL g_SAT_INIT_COMPLETE;
extern BOOL g_SAT_IND;
extern BOOL Stack_MsgFree (PVOID pMemBlock);

//
// COS_SendRMCMessage
// send message to remote PC
// para: msg, message to send
// para: size, message size
//
VOID COS_SendRMCMessage(u8 *msg, u32 size)
{
    u8 *MsgToSend;

    MsgToSend = (u8 *)CSW_Malloc(size);

    SUL_MemCopy8(MsgToSend, msg, size);

    sxr_Send( MsgToSend, CMN_STTPC, SXR_SEND_MSG );
}


static void cfw_ParseEvt ( void *pEvt )
{
    UINT32 *p   = (UINT32 *)pEvt;

    if(p[0] == EV_CFW_INIT_REQ )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR(" CFW_Init invoked\n",0x08100130));
        CFW_Init();        
#ifdef CFW_TCPIP_SUPPORT
        tcpip_init(NULL,NULL);
#endif
        p[0] = 0;
    }

    if(p[0] == API_PS_QUEUECREATE_IND)
    {
        //CFW_CfgNwSetPsQueueId(psInMsg->QueueId);

        api_PSQueueCreateInd_t *psInMsg  = (api_PSQueueCreateInd_t *)(&p[2]);

        CFW_SIM_ID nSimID = p[1];
        nQueueid[nSimID] = psInMsg->QueueId;
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("API_PS_QUEUECREATE_IND = 0x%x\n",0x08100131), nQueueid[nSimID] );
    }
    else if(p[0] == EV_RMT_STTPC_REQ)
    {
        COS_EVENT ev;
        UINT32 Size = sxr_GetMemSize (pEvt) - sizeof(UINT32);
        UINT8 *pRmc = (UINT8 *)CSW_Malloc(Size);
        if(pRmc != NULL)
        {
            SUL_MemCopy8(pRmc, &p[1], Size);
            ev.nEventId = EV_RMT_APP_REQ;
            ev.nParam1  = (UINT32)pRmc;
            ev.nParam2  = Size;
            ev.nParam3  = 0;
            COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        }
    }
#ifdef CFW_TCPIP_SUPPORT
    else if(p[0] == EV_INTER_APS_TCPIP_REQ)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("aps: nEventId == EV_INTER_APS_TCPIP_REQ\n",0x081000f9)));
        struct tcpip_msg *msg;
        msg = (struct tcpip_msg *)p[2];
        tcpip_thread(msg);
    }
    else if(p[0] == PRV_CFW_IP_TIMER_ID)
    {
        sys_dotimeout((void *)p[2]);
    }
#endif
    else
    {
        CFW_AoScheduler(pEvt);
    }
#if 0
    if(p[0] == API_SIM_FETCH_IND)
    {
        if(!g_SAT_INIT_COMPLETE && g_SAT_IND)            //to wait the fetchind message
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("Func CSW_SAT_InitEX() ",0x08100132));

            UINT32 *pE   = (UINT32 *)pEvt;
            CFW_EV   Event;
            Event.nEventId = *pE++;
            if( EV_TIMER != Event.nEventId)
            {
                Event.nFlag = (UINT8) * pE++;
                Event.nParam1 = (UINT32)pE;
                CSW_SAT_InitEX(&Event);
            }
        }
    }
#endif
}
static void cfw_ParseMsg ( void *pMsg )
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_CSW, CSW_SYS_FREQ_104M);
    cfw_ParseEvt(pMsg);

    Stack_MsgFree(pMsg);
    csw_SetResourceActivity(CSW_LP_RESOURCE_CSW, CSW_SYS_FREQ_32K);

}

static void cfw_ParseSignal ( UINT32 *Signal )
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_CSW, CSW_SYS_FREQ_104M);

    UINT32 *p   = Signal;
    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2), TSTR("cfw_ParseSignal 1 Signal[0]  = 0x%x, signal[1] = 0x%x\n",0x08100133), Signal[0], Signal[1]);
    if(p[0] >= HVY_TIMER_IN )
    {
        p[1] = p[0] - HVY_TIMER_IN;
        p[0] = EV_TIMER;
    }
    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2), TSTR("cfw_ParseSignal 2 Signal[0]  = 0x%x, signal[1] = 0x%x\n",0x08100134), Signal[0], Signal[1]);
    CFW_AoScheduler(Signal);
    csw_SetResourceActivity(CSW_LP_RESOURCE_CSW, CSW_SYS_FREQ_32K);

}

static void cfw_Init (void)
{
}


const sxs_TaskParse_t cfw_TaskParse =
{
    cfw_Init,
    cfw_ParseMsg,
    cfw_ParseSignal,
    MMI_MBX
};

