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


#include <base.h>
#include <cfw.h>
#include <cos.h>
#include <stdkey.h>
#include <sul.h>
#include "api_msg.h"
#include <ts.h>
#include <base_prv.h>
#include <cfw_prv.h>
#include <event.h>

#include <event_prv.h>
#include <shell.h>
#include "csw_debug.h"
#include "cfw_config_prv.h"
#include <cfw_sim_prv.h>
#include <stack_map.h>
#include "chip_id.h"
#include "tgt_m.h"
#include "hal_host.h"
#include "string.h"
#include "calib_m.h"

// Include mandatorily that one after tgt_m.h (if inclusion needed)
// as the csw_csp.h change the meaning of the factory setting fields
// by expanding them with mobile_id., for example.
#include "csw_csp.h"

UINT32 CFW_SimPatch(UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 processFetchInd(CFW_EV *pEvent, UINT32 nUTI, CFW_SIM_ID nSimID);
extern UINT32 _ProcessLockBCCHNWRsp(CFW_SIM_ID nSimID);

extern UINT8 *_SIMPLE_TLV_DATA_OBJECTS_LOCATION_INFORMATION( UINT8 *nMCCMNC, UINT16 *nLAC, UINT16 *nCellID, UINT8 *pLen );

BOOL _SetAutoCallTimer(UINT32 nElapse, CFW_SIM_ID nSimID);

PRIVATE UINT8 g_nStatus[CFW_SIM_COUNT]  = { SHELL_STATE_WAIT, };
BOOL g_NW_IND[CFW_SIM_COUNT]            = { FALSE, };
BOOL g_IMSI_INIT[CFW_SIM_COUNT]         = { FALSE, };
BOOL g_CFW_PowerOn                      = FALSE;
BOOL g_SAT_IND[CFW_SIM_COUNT]           = { FALSE, };
BOOL g_SAT_INIT_COMPLETE[CFW_SIM_COUNT] = { FALSE, };

UINT32 g_TestMode_AutoResponse[CFW_SIM_COUNT]   = { 0x00, };
UINT32 g_TestMode_AutoEMCDial[CFW_SIM_COUNT]    = { 0x00, };
UINT32 g_TestMode_AutoEMCDialNum[CFW_SIM_COUNT] = { 0x00, };
PRIVATE UINT8 g_NW_DialEmcOK[CFW_SIM_COUNT]     = { 0x00, };
PRIVATE UINT8 g_State_No_SimCard[CFW_SIM_COUNT] = { 0x00, };
PRIVATE UINT8 g_State_TestCard[CFW_SIM_COUNT]   = { 0x00, };
CFW_SMS_PARAMETER g_SMSPInfo[CFW_SIM_COUNT] = { {0,}, };
UINT32 g_SimInit1Status[CFW_SIM_COUNT]    = { 0x00, };
UINT32 g_SimInit2Status[CFW_SIM_COUNT]    = { 0x00, };
UINT32 g_SimInit3Status[CFW_SIM_COUNT]    = { 0x00, };
UINT32 g_SimInit3Status_p2[CFW_SIM_COUNT] = { 0x00, };
typedef struct _NW_STORELIST_INFO
{
    CFW_StoredPlmnList p_StoreList[CFW_SIM_COUNT];
} CFW_STORELIST_INFO;


//Add by wcf, 2015/1/19, Process polling timer
//g_nPollingTimer hold intervals of the timers
//g_bTimerStatus present status of timer, started or not
#ifdef _SIM_HOT_PLUG_

UINT8 g_nPollingTimer[CFW_SIM_COUNT] = {30, 30};
UINT8 g_bTimerStatus[CFW_SIM_COUNT] = {FALSE, FALSE};
UINT8 g_SIM_REMOVE_IND[CFW_SIM_COUNT] = {0, 0};
VOID CFW_SetPollingTimer(UINT8 PollingInterval, CFW_SIM_ID nSimID)
{
    g_nPollingTimer[nSimID] = PollingInterval;
}
VOID CFW_GetPollingTimer(UINT8 *pPollingInterval, CFW_SIM_ID nSimID)
{
    *pPollingInterval  = g_nPollingTimer[nSimID];
}

#endif      //#ifdef _SIM_HOT_PLUG_
//End, Process polling timer

extern BOOL CFW_GetSATIndFormate(VOID);

extern UINT32 CFW_SimGetSmsTotalNum(UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimInitStage1(CFW_SIM_ID nSimID);
static UINT8 nSimOpenStatus[CFW_SIM_COUNT] = { SOS_NO_SIM, };
VOID CFW_SimUnRegisterAll(
    CFW_SIM_ID nSimID
);
extern  CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;


UINT32 CFW_GetSimOpenStatus(CFW_SIM_ID nSimID)
{
    return nSimOpenStatus[nSimID];
}

VOID CFW_SetSimOpenStatus(UINT8 nStatus, CFW_SIM_ID nSimID)
{
    nSimOpenStatus[nSimID] = nStatus;
}

//
// Setup all CFW necessary Callback function as initialization step.
// This functions should be called by system or other task if you want to used CFW TASK.
//
UINT32 BAL_CfwSetupCbs(CFW_PFN_KL_CBS *pKlFun, CFW_PFN_AUD_CBS *pHwFun)
{
    CSW_PROFILE_FUNCTION_ENTER(BAL_CfwSetupCbs);
    CFW_BalSetupHwCbs(pHwFun);
    CSW_PROFILE_FUNCTION_EXIT(BAL_CfwSetupCbs);
    return ERR_SUCCESS;
}

//
// CFW command control.
//
UINT32 CFW_ShellControl(UINT8 nCommand)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Entry CFW_ShellControl\r\n",0x081009b3)));
    CSW_PROFILE_FUNCTION_ENTER(CFW_ShellControl);
    if (nCommand == CFW_CONTROL_CMD_POWER_ON)
    {
        if (!g_CFW_PowerOn)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CONTROL_CMD_POWER_ON\r\n",0x081009b4)));

            g_CFW_PowerOn = TRUE;

            //
            // Instatll some callback functios to CSW-CFW when CFW task initialize.
            // You can Setupt this CBS in Usr-Task when initialize.
            //
            CFW_PFN_AUD_CBS pfnAudCbs = { NULL, NULL };

            CFW_ServicesExit();

            CFW_BalSetupHwCbs(&pfnAudCbs);

            //
            // Start Initialize CSW ATCOM module really.
            //

            CFW_BalSendMessage(CFW_MBX, EV_CFW_INIT_REQ, NULL, 0, CFW_SIM_END);


        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Error Call CFW_CONTROL_CMD_POWER_ON More than one time\r\n",0x081009b5)));
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Error return ERR_CFW_REPEAT_TRANSFER\r\n",0x081009b6)));

            return ERR_CFW_REPEAT_TRANSFER;
        }

    }
    else if (nCommand == CFW_CONTROL_CMD_POWER_OFF)
    {
        if (g_CFW_PowerOn)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CONTROL_CMD_POWER_OFF\r\n",0x081009b7)));
            g_CFW_PowerOn = FALSE;
            CFW_Exit(
#ifdef LTE_NBIOT_SUPPORT
				1
#endif
			);
        }
    }
    else if (nCommand == CFW_CONTROL_CMD_RESTART)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CONTROL_CMD_RESTART Not Support\r\n",0x081009b8)));
        return ERR_NOT_SUPPORT;
    }
    else if (nCommand == CFW_CONTROL_CMD_RESET)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CONTROL_CMD_RESET Not Support\r\n",0x081009b9)));
        return ERR_NOT_SUPPORT;
    }

    //
    // Add at 20060914
    //
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_ShellControl Input Invalid Parameter %x\r\n",0x081009ba)), nCommand);

        return ERR_INVALID_PARAMETER;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_ShellControl);

    return ERR_SUCCESS;
}

// #define CFW_SIM_CARD_REMOVED   (SIM_BASE+1)  //No sim card is inserted.That just only is sent after API_SIM_RESET_CNF received.
#ifdef __221_UNIT_TEST__
UINT32 CFW_SimInit(CFW_SIM_ID nSimID);
#endif
VOID SendRefreshSclMsg(UINT8 nSAT_Status, CFW_SIM_ID nSimID);


UINT32 _SimOpen(CFW_SIM_ID nSimID)
{

    CFW_COMM_MODE nFM;
    UINT32 nRet = CFW_GetComm(&nFM, nSimID);
    if (ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_GetComm failed return %x\n",0x081009bb), nRet);

        nFM = CFW_ENABLE_COMM;
    }
    if (CFW_ENABLE_COMM == nFM)
    {
        //CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));
        CFW_SIM_ID s = CFW_SIM_0;

        for( ; s < CFW_SIM_COUNT; s++ )
        {
            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
        }
        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("Enable communciation nSimID[%d]\n",0x081009bc)), nSimID);
        CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);

        g_nStatus[nSimID] = SHELL_STATE_WAIT_NW_WITHSIM;
    }

    return 0;
}//
// CFW Shell Proc entry.
//
//if SIM initialization is triggered by STK module, the variable will be set to 1, otherwise it will be set to 0.
UINT8 bSatSimInit = 0;
#ifdef CB_SUPPORT
extern VOID CFW_CbInit(CFW_SIM_ID nSimID);
#endif
UINT32 CFW_ShellProc(CFW_EV *pEvent)
{
    UINT32 ret     = ERR_SUCCESS;
    UINT32 nEvId   = 0;
    UINT32 nParam1 = 0;
    UINT32 nParam2 = 0;
    UINT8 nType    = 0;
    UINT8 nTUTI    = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_ShellProc);
    nEvId   = pEvent->nEventId;
    nParam1 = pEvent->nParam1;
    nParam2 = pEvent->nParam2;
    nType   = pEvent->nType;
    nTUTI   = pEvent->nTransId;

    CFW_SIM_ID nSimID;

    nSimID = pEvent->nFlag;

    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("CFW_ShellProc [nEvId]0x%x,[nParam1]0x%x [nSimID]%d,[nType]0%x\n",0x081009bd), nEvId, nParam1, nSimID,
              nType);
    if (nEvId == EV_CFW_SIM_STATUS_IND) // EV_CFW_SRV_STATUS_IND
    {
        if (nParam1 == CFW_SIM_CARD_INSERTED) // sim init phase_1
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Find SimCard\n",0x081009be));

            //Add by wcf, 2015/1/19, process polling timer
#ifdef _SIM_HOT_PLUG_

            g_SIM_REMOVE_IND[nSimID] = 0;
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SIM_CARD_INSERTED: g_SIM_REMOVE_IND[%d] = %d\n",0x081009bf), nSimID, g_SIM_REMOVE_IND[nSimID]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("---g_bTimerStatus[%d]:%d---",0x081009c0), nSimID, g_bTimerStatus[nSimID]);

            if( g_bTimerStatus[nSimID] == TRUE)
            {
                g_bTimerStatus[nSimID] = FALSE;
                COS_KillTimer(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM0_ID + nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("---SIM Card Removed and Inserted again --nSimID = %d---",0x081009c1), nSimID);
            }

#endif      //#ifdef _SIM_HOT_PLUG_
            //End, process polling timer

            // g_SimInit1Status = CFW_INIT_SIM_INSERTED;
            // CFW_PostNotifyEvent(EV_CFW_INIT_IND, CFW_INIT_SIM_INSERTED, nParam2, 0, CFW_INIT_SMS_STATUS);
        }
        else if (nParam1 == CFW_SIM_CARD_DROP)
        {

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Received SimCard Drop in Shell(nSimID = %d)\n",0x081009c2),nSimID);
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM_DROP);
            CFW_SimUnRegisterAll(nSimID);

            //Add by wcf, 2015/1/19, process polling timer
#ifdef _SIM_HOT_PLUG_
            if(g_bTimerStatus[nSimID] == FALSE)
            {
                g_bTimerStatus[nSimID] = TRUE;
                BOOL result = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM0_ID + nSimID, \
                                             COS_TIMER_MODE_SINGLE, g_nPollingTimer[nSimID] * 1000 MILLI_SECOND);
                if (result == FALSE)  //
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("---Start timer Fail---",0x081009c3));
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Drop---Polling timer start simID = %d -g_bTimerStatus[nSimID] = TRUE = %d",0x081009c4), nSimID, g_bTimerStatus[nSimID] = TRUE);
            }
#endif  //#ifdef _SIM_HOT_PLUG_
            //End, process polling timer

        }
        else if (nParam1 == CFW_SIM_CARD_REMOVED) // sim init phase_1
        {
            DM_KeyboardInit();
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("NO SimCard\n",0x081009c5));
            //Add by wcf, 2015/3/24, process polling timer
#ifdef _SIM_HOT_PLUG_
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Multi SIM Send -g_SIM_REMOVE_IND[%d]= %d\n",0x081009c6), nSimID, g_SIM_REMOVE_IND[nSimID]);
            if(g_SIM_REMOVE_IND[nSimID] == 0)
            {
                g_SIM_REMOVE_IND[nSimID] = 1;
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Multi SIM Send --- EV_CFW_INIT_IND --- \n",0x081009c7));
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NO_SIM);
            }
            if(g_bTimerStatus[nSimID] == FALSE)
            {
                g_bTimerStatus[nSimID] = TRUE;
                BOOL result = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM0_ID + nSimID, \
                                             COS_TIMER_MODE_SINGLE, g_nPollingTimer[nSimID] * 1000 MILLI_SECOND);
                if (result == FALSE)
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("---Start timer Fail---",0x081009c3));
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Drop---Polling timer start simID = %d -g_bTimerStatus[nSimID] = TRUE = %d",0x081009c4), nSimID, g_bTimerStatus[nSimID] = TRUE);
            }

#else
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NO_SIM);
#endif

            g_SimInit1Status[nSimID] = 0;
            g_SimInit2Status[nSimID] = 0;
            g_SimInit3Status[nSimID] = 0;
            if (0x01 == CFW_GetAutoCallCmd(nSimID))
            {
                PM_ProLPMDisable();
            }

            if ((0x01 == CFW_GetAutoCallCmd(nSimID)) && g_NW_DialEmcOK[nSimID])
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Auto Dail\n",0x081009c8));
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0x00, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_AUTODAIL);

                _SetAutoCallTimer(1500, nSimID);
            }


            else if ((!g_NW_DialEmcOK[nSimID]))
            {
                g_State_No_SimCard[nSimID] = 1;

                //Add by wcf, 2015/1/19, process polling timer
#ifdef _SIM_HOT_PLUG_

                if(   g_bTimerStatus[nSimID] == FALSE)
                {
                    g_bTimerStatus[nSimID] = TRUE;
                    BOOL result = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_SIM_POLLING_TIMER_SIM0_ID + nSimID, \
                                                 COS_TIMER_MODE_SINGLE, g_nPollingTimer[nSimID] * 1000 MILLI_SECOND);
                    if (result == FALSE)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("---Start timer Fail---",0x081009c9));

                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("REMOVED---Polling timer start simID = %d, g_bTimerStatus[nSimID] = %d--- ",0x081009ca), nSimID, g_bTimerStatus[nSimID]);
                }
#endif  //#ifdef _SIM_HOT_PLUG_
                //End, process polling timer

            }
            else
            {
                ;
            }
        }
        else if (nParam1 == ERR_NO_MORE_MEMORY)
        {
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NO_MEMORY);
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error No More Memory\n",0x081009cb));
        }
        else if (nParam1 == CFW_SIM_CARD_PIN1BLOCK) // sim init phase_1. sim card is block(pin1,or puk1?).
        {
            g_SimInit1Status[nSimID] |= CFW_INIT_SIM_CARD_BLOCK;
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit1Status[nSimID], nParam2, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM);
            g_SimInit1Status[nSimID] = 0;
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("PIN1 Block nSimID[%d]\n",0x081009cc), nSimID);

        }
        else if (nParam1 == CFW_SIM_CARD_PUK1)  // sim init phase_1, need to input puk1
        {
            DM_KeyboardInit();
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Need Input PUK1\n",0x081009cd));
            g_SimInit1Status[nSimID] |= CFW_INIT_SIM_WAIT_PUK1;
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit1Status[nSimID], nParam2, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM);
            // there should be send a message to MMI
            // CFW_SimEnterAuthentication("1234",4,"74074155",8,1,APP_UTI_SHELL);
        }
        else if (nParam1 == CFW_SIM_CARD_PIN1)  // sim init phase_1, need to input pin1
        {
            DM_KeyboardInit();
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Need Input PIN1\n",0x081009ce));
            g_SimInit1Status[nSimID] |= CFW_INIT_SIM_WAIT_PIN1;
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit1Status[nSimID], nParam2, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM);

            // there should be send a message to MMI
            // CFW_SimEnterAuthentication("1234",4,NULL,0,1,APP_UTI_SHELL);
        }
        else if (nParam1 == CFW_SIM_CARD_PHASE) // sim init phase_1, pin1 ready
        {
            if (SAT_SUPPORT && (TRUE == CFW_SimInitStage2(nSimID)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ProfileDownload OK\n",0x081009cf));
//                COS_Sleep(50);  //This timer value should be larger than 37 at least , and only for Tianjing China mobile SIM, otherwise PROFILEDOWNLOAD return 0x9000.
                if (ERR_SUCCESS != (ret = CFW_SatActivation(0, APP_UTI_SHELL, nSimID)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SatActivation failed,return 0x%x\n",0x081009d0), ret);
                    if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed,return 0x%x\n",0x081009d1), ret);
                    }
                }
            }
            else
            {
                if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed,return 0x%x\n",0x081009d2), ret);
                }
            }
        }
        else if (nParam1 == CFW_SIM_CARD_PIN1DISABLE) // sim init phase_1, pin1 ready
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("PIN1 Ready\n",0x081009d3));
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL), "bSatSimInit = %d",bSatSimInit);
            if(bSatSimInit == 0)
            {
                g_SimInit1Status[nSimID] |= CFW_INIT_SIM_PIN1_READY;
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit1Status[nSimID], nParam2, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM);
            }
            if (ERR_SUCCESS == CFW_SimInitStage1(nSimID))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SimInitStage1 OK\n",0x081009d4));

            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SimInitStage1 Error\n",0x081009d5));

                if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed,return 0x%x\n",0x081009d6), ret);
                }
            }
        }
        else if (nParam1 == CFW_SIM_CARD_BAD) // sim init phase_3
        {
            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_CARD_BAD;
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIMCARD);

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Bad SimCard\n",0x081009d7));

        }
        else if (nParam1 == CFW_SIM_CARD_ABNORMITY)
        {
            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_CARD_ABNORMITY;
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIMCARD);

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Abnormity Sim Card\n",0x081009d8));

        }
        else if (nParam1 == CFW_SIM_CARD_TEST)  // sim init phase_3
        {
            //          CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
            CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

            CFW_SIM_ID s = CFW_SIM_0;

            for( ; s < CFW_SIM_COUNT; s++ )
            {
                CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
            }
            pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Sim Card Using for test\n",0x081009d9));

            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_TEST_CARD;

            // CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], nParam2, 0 | (nSimID << 24),
            // CFW_INIT_STATUS_SIMCARD);
            if (ERR_SUCCESS != (ret = CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID)))
            {
                return (ret | BAL_PROC_DONT_FREE_MEM | BAL_PROC_COMPLETE);
            }
            g_State_TestCard[nSimID] = 0x01;
            g_nStatus[nSimID]        = SHELL_STATE_WAIT_NW_WITHSIM;
            if (0x02 == CFW_GetAutoCallCmd(nSimID))
            {
                PM_ProLPMDisable();
            }
            //g_nStatus[nSimID] = SHELL_STATE_SIM_IMSI;

        }
        else if (nParam1 == CFW_SIM_CARD_NORMAL)  // sim init phase_3,  sent by Security APIs. last event of SIM init.
        {
            // Param2.b0 == 1:AND;Param2b0 == 0:FDN
            // Param2.b1= 1:BDN enable;Param2.b1=0:BDN disable.
            g_SimInit3Status[nSimID] &= ~CFW_INIT_SIM_WAIT_PS;
            g_SimInit3Status[nSimID] &= ~CFW_INIT_SIM_WAIT_PF;
            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_NORMAL_CARD;
            g_SimInit3Status_p2[nSimID] = nParam2;

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Nomal Sim Card nSimID[%d]\n",0x081009da), nSimID);
            if (0x01 == (nParam2 & 0x01))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ADN mode\n",0x081009db));
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("FDN mode\n",0x081009dc));
            }

            if (0x01 == ((nParam2 & 0x02) >> 1))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("BDN Enable\n",0x081009dd));
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("BDN Disable\n",0x081009de));
            }

            // Add by lixp at 20080703
            CFW_COMM_MODE nFM;

            UINT32 nRet = CFW_GetComm(&nFM, nSimID);

            if (ERR_SUCCESS != nRet)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_GetComm failed return %x\n",0x081009df), nRet);

                nFM = CFW_ENABLE_COMM;
            }

            if (CFW_ENABLE_COMM == nFM)
            {
                //              CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
                CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));
                CFW_SIM_ID s = CFW_SIM_0;

                for( ; s < CFW_SIM_COUNT; s++ )
                {
                    CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                }
                pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("Enable communciation nSimID[%d]\n",0x081009e0)), nSimID);
                if (ERR_SUCCESS != (ret = CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID)))
                {
                    // CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], ret, 0 | (nSimID << 24),
                    // CFW_INIT_STATUS_SIMCARD);
                }
                else
                {
                    // CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], g_SimInit3Status_p2[nSimID], 0 | (nSimID << 24),
                    // CFW_INIT_STATUS_SIMCARD);
                }

                g_nStatus[nSimID] = SHELL_STATE_WAIT_NW_WITHSIM;
            }
            else if ((CFW_DISABLE_COMM == nFM) || (CFW_CHECK_COMM == nFM))  //
            {
                g_nStatus[nSimID] = SHELL_STATE_SIM_IMSI;
                if (CFW_DISABLE_COMM == nFM)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Disable communciation\n",0x081009e1)));
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Check status for communciation\n",0x081009e2)));
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Error Get communciation status,CommMode %d,nSimID %d\n",0x081009e3)), nFM, nSimID);
            }
            g_nStatus[nSimID] = SHELL_STATE_SIM_IMSI;
#if defined(SIMCARD_HOT_DETECT)
            if(nSimID == SIMCARD_HOT_DETECT-1)
            {
                //2 reinit IMSI
                extern UINT8 simcard_hot_detect_is_init(void);
                if(simcard_hot_detect_is_init())
                    g_IMSI_INIT[nSimID] = FALSE;
            }
#endif

        }
        else if (nParam1 == CFW_SIM_ME_PS)
        {
            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_WAIT_PS;

            // CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIMCARD);

            // Call Phone lock verify procedure
            // CFW_SimInitStage3();
        }
        else if (nParam1 == CFW_SIM_ME_PF)
        {
            g_SimInit3Status[nSimID] |= CFW_INIT_SIM_WAIT_PF;

            // CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIMCARD);

            // Call Phone lock verify procedure
            // CFW_SimInitStage3();
        }
    }
    else if (SAT_SUPPORT && (nEvId == EV_CFW_SAT_ACTIVATION_RSP))
    {
        if (nParam1 == 0)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Get EV_CFW_SAT_ACTIVATION_RSP, Wait EV_CFW_SAT_CMDTYPE_IND message\n",0x081009e4));

            // Wait EV_CFW_SAT_CMDTYPE_IND message
        }
        else
        {
            if (nParam1 == ERR_CME_SIM_PROFILE_ERROR) // when intit sat, some card may return 0x90, in this case there is a fetchind msg later
            {
                g_SAT_IND[nSimID] = TRUE;

                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Without Fetch Data\n",0x081009e5));
            }
            else
            {
                if(bSatSimInit == 0)
                {
                    g_SimInit2Status[nSimID] &= ~CFW_INIT_SIM_SAT;  // call CFW_SatGetInformation to get sap info.
                    CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit2Status[nSimID], 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SAT);
                }
            }

            if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error CFW_SimInitStage3 failed,return 0x%x nSimID %d\n",0x081009e6), ret, nSimID);
            }

        }
    }

    // else if (SAT_SUPPORT && (nEvId == EV_CFW_SAT_CMDTYPE_IND))
    else if (SAT_SUPPORT && (nEvId == EV_CFW_SAT_CMDTYPE_IND))
    {

        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Get EV_CFW_SAT_CMDTYPE_IND command(Fetch Data)\n",0x081009e7));

        BOOL bRet = CFW_GetSATIndFormate();

        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SAT Indication formate %d\n",0x081009e8), bRet);
#if 1
        UINT32 nInParam1 = 0;
        UINT32 nTransID = 0;
        g_SimInit2Status[nSimID] |= CFW_INIT_SIM_SAT; // call CFW_SatGetInformation to get sat info.
        nInParam1 = g_SimInit2Status[nSimID];
        nTransID = nSimID << 24;
        CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pEvent->nParam1, pEvent->nParam2, nTransID, pEvent->nType);
        CFW_PostNotifyEvent(EV_CFW_INIT_IND, nInParam1, pEvent->nParam1, nTransID, CFW_INIT_STATUS_SAT);
#else
        if (CFW_SAT_IND_PDU == bRet)
        {
            // g_SimInit2Status[nSimID] |= CFW_INIT_SIM_SAT; // call CFW_SatGetInformation to get sap info.
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, (pEvent->nParam1), (pEvent->nParam2), 0 | (nSimID << 24),
                                CFW_INIT_STATUS_SAT);
        }
        else
        {
            if (pEvent->nParam1 == 0x25)  // to check whether is it a SETUP MENU command
            {

                g_SimInit2Status[nSimID] |= CFW_INIT_SIM_SAT; // call CFW_SatGetInformation to get sap info.
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit2Status[nSimID], (pEvent->nParam1), 0 | (nSimID << 24),
                                    CFW_INIT_STATUS_SAT);

            }
        }

#endif
        if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed, return 0x%x\n",0x081009e9), ret);
        }
    }

    else if (SAT_SUPPORT && (nEvId == EV_CFW_SAT_RESPONSE_RSP))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("g_SAT_IND %d, g_SAT_INIT_COMPLETE %d!\n",0x081009ea), g_SAT_IND[nSimID],
                  g_SAT_INIT_COMPLETE[nSimID]);
        if ((!g_SAT_IND[nSimID]) && (nParam1 == 0) && (nType == 0))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SAT Init OK\n",0x081009eb));
            g_SAT_INIT_COMPLETE[nSimID] = TRUE;
            if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed, return 0x%x\n",0x081009ec), ret);
            }
            g_SAT_IND[nSimID] = TRUE;
        }
        else if (g_SAT_IND[nSimID] && g_SAT_INIT_COMPLETE[nSimID] && (nParam2 == 0x90) && (nType == 0)) // if init succeeds, to notify MMI
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SAT Init OK\n",0x081009ed));
            g_SimInit2Status[nSimID] |= CFW_INIT_SIM_SAT; // call CFW_SatGetInformation to get sap info.
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit2Status[nSimID], (pEvent->nParam1), 0 | (nSimID << 24),
                                CFW_INIT_STATUS_SAT);
        }
        else if (!g_SAT_IND[nSimID] && !g_SAT_INIT_COMPLETE[nSimID] && (0x00 != nType))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("g_SAT_INIT_COMPLETE***x%x\n",0x081009ee), ret);

            g_SAT_INIT_COMPLETE[nSimID] = TRUE;
            if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage3 failed,return 0x%x\n",0x081009ef), ret);
            }
            g_SAT_IND[nSimID] = TRUE;
        }
        else
        {
            // TODO
            g_SAT_INIT_COMPLETE[nSimID] = TRUE;
            if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage3 failed,return 0x%x\n",0x081009f0), ret);
            }
            g_SAT_IND[nSimID] = TRUE;
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SAT At TODO sect\n",0x081009f1));
        }
    }
    else if(nEvId == EV_CFW_SIM_CHANGE_PWD_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-----------------nParam2 = %d--------------------",0x081009f2),nParam2);
        if(((nParam1 == 0x63C0) || (nParam1 == 0x9840))&&( nParam2 == 0x01))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Deactive stack after input WRONG pin1 code!",0x081009f3));
            CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
        }
    }
    else if(nEvId == EV_CFW_SIM_SET_FACILITY_LOCK_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("-------------------------------------",0x081009f4));
        if((nParam1 == 0x63C0) || (nParam1 == 0x9840))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Deactive stack after input WRONG pin1 code!",0x081009f5));
            CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
        }
    }
    else if (nEvId == EV_CFW_SIM_ENTER_AUTH_RSP)  // MMI input pin1 or puk1 OK --> EV_CFW_SIM_ENTER_AUTH_RSP
    {
        if (nParam1 == 0)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Enter PIN OK\n",0x081009f6));
#ifdef _DROPCARD_AUTOPIN_
            //_SimOpen(nSimID);
            SimRetryReq(nSimID);
#else
            SIM_MISC_PARAM *pG_Mic_Param;
            CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
            if(pG_Mic_Param->bStartPeriod == FALSE)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Active stack after input PUK1 code!",0x081009f7));
                //_SimOpen(nSimID);
            }
#endif

            g_SimInit2Status[nSimID] &= ~CFW_INIT_SIM_WAIT_PIN1;
            g_SimInit2Status[nSimID] &= ~CFW_INIT_SIM_WAIT_PUK1;
            if (ERR_SUCCESS != CFW_SimInitStage1(nSimID))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SimInitStage1 Error \n",0x081009f8));
                if (ERR_SUCCESS != (ret = CFW_SimInitStage3(nSimID)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimInitStage3 failed, return 0x%x\n",0x081009f9), ret);
                }
            }
        }
        else
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Wrong PIN\n",0x081009fa));
    }
    else if (nEvId == EV_CFW_SIM_GET_PROVIDER_ID_RSP)
    {
        if (nType == 0)
        {
            // ------------------------------------------------------------
            // Add by lixp for  special card 2007.8.16 begin
            // ------------------------------------------------------------
            UINT8 nIMSI[10];

            SUL_ZeroMemory8(nIMSI, 10);

            UINT32 nRet = CFW_CfgGetIMSI(nIMSI, nSimID);
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgGetIMSI nIMSI[9] 0x%x \n",0x081009fb), nIMSI[9]);
            if (ERR_SUCCESS != nRet)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CfgGetIMSI return 0x%x\n",0x081009fc), nRet);
            }

            // ------------------------------------------------------------
            // Add by lixp for special card 2007.8.16 end
            // ------------------------------------------------------------

            if (nIMSI[9] == 0)
            {
                g_SMSPInfo[nSimID].bSimChange = 0;

            }
            else
            {
                g_SMSPInfo[nSimID].bSimChange = 1;
                g_SimInit3Status[nSimID] |= CFW_INIT_SIM_CARD_CHANGED;


            }
            if (pEvent->nParam1)
            {
                CSW_SMS_FREE((UINT8 *)(pEvent->nParam1));
                pEvent->nParam1 = (UINT32)NULL;
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Get IMSI 0x%x\n",0x081009fd), nParam1);
        }
        if(bSatSimInit == 1)
        {
            bSatSimInit = 0;
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL), "CFW_INIT_STATUS_SIM_COMPLTED :bSatSimInit = %d",bSatSimInit);
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_SIM_COMPLTED);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, g_SimInit3Status[nSimID], g_SimInit3Status_p2[nSimID], 0 | (nSimID << 24),
                                CFW_INIT_STATUS_SIMCARD);
        g_nStatus[nSimID] = SHELL_STATE_SMS;
    }
    else if (nEvId == EV_CFW_SIM_GET_SMS_PARAMETERS_RSP)
    {
        if (nType == 0x00)
        {
#if 1
            CFW_SIM_SMS_PARAMETERS *pSmsParam = (CFW_SIM_SMS_PARAMETERS *)pEvent->nParam1;

            //
            // Get the SMS SCA number, 12 bytes.
            //
            SUL_MemCopy8(g_SMSPInfo[nSimID].nNumber, (UINT8 *)pSmsParam->nSMSCInfo, SIZEOF(g_SMSPInfo[nSimID].nNumber));

            // ------------------------------------------------------------
            // Add by lixp for Sim sca raise 2007-06-25 begin
            // ------------------------------------------------------------

            // if(g_SMSPInfo.nNumber[0] && g_SMSPInfo.nNumber[1] && g_SMSPInfo.nNumber[2] )
            if (g_SMSPInfo[nSimID].nNumber[0])

                // ------------------------------------------------------------
                // Add by lixp for Sim sca raise 2007-06-25 end
                // ------------------------------------------------------------

            {
                // ------------------------------------------------------------
                // Add by wanghb for special sca  begin  2007.8.10
                // ------------------------------------------------------------
                if (0xFF == g_SMSPInfo[nSimID].nNumber[0])
                {
                    g_SMSPInfo[nSimID].nNumber[0] = 0x00;
                }

                // ------------------------------------------------------------
                // Add by wanghb for special sca  end  2007.8.10
                // ------------------------------------------------------------
                CFW_CfgSetDefaultSmsParam(&g_SMSPInfo[nSimID], nSimID);

                CSW_TC_MEMBLOCK(CFW_SHELL_TS_ID, (UINT8 *)g_SMSPInfo[nSimID].nNumber, 12, 16);

            }
            if (pEvent->nParam1)
            {
                CSW_SMS_FREE(((VOID *)(pEvent->nParam1)));
                pEvent->nParam1 = (UINT32)NULL;
            }
#endif
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Get Sms Parameter 0x%x",0x081009fe), nParam1);
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Start Patch",0x081009ff));

        CFW_SimPatch(APP_UTI_SHELL, nSimID);

        // CFW_SmsMoInit(APP_UTI_SHELL, nSimID);
        // CFW_SimGetSmsTotalNum(APP_UTI_SHELL, nSimID);
        g_nStatus[nSimID] = SHELL_STATE_WAIT;

    }
    else if (nEvId == EV_CFW_SIM_PATCH_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Got Patch Rsp",0x08100a00));
        // CFW_SimPatch(APP_UTI_SHELL, nSimID);
        // CFW_SmsMoInit(APP_UTI_SHELL, nSimID);
        CFW_SimGetSmsTotalNum(APP_UTI_SHELL, nSimID);
        g_nStatus[nSimID] = SHELL_STATE_WAIT;
    }
    else if (nEvId == EV_CFW_SIM_PATCHEX_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Got PatchEX Rsp",0x08100a01));
        extern BOOL gSatFreshComm[];

        if(0x20 != (0xF0 & gSatFreshComm[nSimID]))
        {
            gSatFreshComm[nSimID] = 0x00;
            CFW_NW_STATUS_INFO nStatusInfo;
            UINT32 nStatus = 0x00;
            if (ERR_SUCCESS != CFW_NwGetStatus(&nStatusInfo, nSimID))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_NwGetStatus Error\n",0x08100a02));

                return ERR_INVALID_TYPE;
            }
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTR("NWSATCellInfo nStatusInfo.nStatus  %d\n",0x08100a03), nStatusInfo.nStatus);

            if (nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_SEARCHING || nStatusInfo.nStatus == CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING ||
                    nStatusInfo.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || nStatusInfo.nStatus == CFW_NW_STATUS_UNKNOW)
            {
                nStatus = 0x06;
            }

            if(0x00 == nStatus)
                SendRefreshSclMsg(0, nSimID);
            else
                SendRefreshSclMsg(6, nSimID);
        }
        else
        {
            gSatFreshComm[nSimID] = ((gSatFreshComm[nSimID] & 0xF0) + 0x03);
            UINT8 nRUti = 0x00;

            CFW_GetFreeUTI(0, &nRUti);
            CFW_SetComm(CFW_ENABLE_COMM, 1, nRUti, nSimID);
        }

    }
    else if (nEvId == EV_CFW_SET_COMM_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Got EV_CFW_SET_COMM_RSP",0x08100a04));
        _ProcessLockBCCHNWRsp(nSimID);
    }
    else if (nEvId == EV_CFW_NW_DEREGISTER_RSP)
    {
        if (nType == 0x00)
        {
            CFW_PostNotifyEvent(EV_CFW_EXIT_IND, CFW_EXIT_STATUS_NETWORK_DETACHMENT, 0, 0 | (nSimID << 24), 0x00);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Error deregister failed 0x%x\n",0x08100a05)), pEvent->nParam1);

            CFW_PostNotifyEvent(EV_CFW_EXIT_IND, CFW_EXIT_STATUS_NETWORK_DETACHMENT, pEvent->nParam1, 0 | (nSimID << 24),
                                0xF0);
        }
    }
    else if (nEvId == EV_CFW_SIM_GET_SMS_TOTAL_NUM_RSP)
    {

        CFW_SetSimOpenStatus(SOS_SENT_SIM_OPEN, nSimID);
        if (nType == 0x00)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Sim SMS GetTotalNum Success\n",0x08100a06)));
#ifdef PHONE_SMS_STORAGE_SPACE_COMBINATE
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, ((CFW_SMS_MAX_REC_COUNT)<<16) | (nParam1 & 0xFFFF), 0 | (nSimID << 24) /*UTI*/, CFW_INIT_STATUS_SMS);
#else
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, ((CFW_SMS_MAX_REC_COUNT/CFW_SIM_COUNT)<<16) | (nParam1 & 0xFFFF), 0 | (nSimID << 24) /*UTI*/, CFW_INIT_STATUS_SMS);
#endif
            g_nStatus[nSimID] = SHELL_STATE_PBK;

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Error Sim SMS Init error\n",0x08100a07)));
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 1, 0, 0 | (nSimID << 24) /*UTI*/, CFW_INIT_STATUS_SMS);
            g_nStatus[nSimID] = SHELL_STATE_PBK;
        }
    }
    else if (nEvId == EV_CFW_SRV_STATUS_IND)
    {
        // SMS
        //
        if (nType == SMS_INIT_EV_OK_TYPE)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Sim SMS Init Success\n",0x08100a08)));
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24) /*UTI*/, CFW_INIT_STATUS_SMS);

            g_nStatus[nSimID] = SHELL_STATE_PBK;

        }
        else if (nType == SMS_INIT_EV_ERR_TYPE)
        {

            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Error Sim SMS Init error\n",0x08100a09)));
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 1, 0, 0 | (nSimID << 24) /*UTI*/, CFW_INIT_STATUS_SMS);
            g_nStatus[nSimID] = SHELL_STATE_PBK;

        }

        //
        // PBK
        //
        if (nType == PBK_INIT_EV_OK_TYPE)
        {
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_PBK);
            g_nStatus[nSimID] = SHELL_STATE_WAIT;
        }
        else if (nType == PBK_INIT_EV_ERR_TYPE)
        {
            CFW_PostNotifyEvent(EV_CFW_INIT_IND, 1, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_PBK);
            g_nStatus[nSimID] = SHELL_STATE_WAIT;
        }
        else if (nType == PBK_EXIT_EV_OK_TYPE)
        {
        }
        else if (nType == PBK_EXIT_EV_ERR_TYPE)
        {
        }
    }
    else if (nEvId == EV_CFW_CC_INITIATE_SPEECH_CALL_RSP)
    {
        if (nType == 0xF0)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Call 112 failed 0x%x\r\n",0x08100a0a)), nParam1);
            CFW_EMCDial(nSimID);

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Call 112 success\r\n",0x08100a0b)));
        }
    }
    if (g_nStatus[nSimID] == SHELL_STATE_AUTO_RESP)
    {
        if ((nEvId == EV_CFW_CC_SPEECH_CALL_IND))
        {
            UINT32 nRet;

            nRet = CFW_CcAcceptSpeechCall(nSimID);

            if (nRet != ERR_SUCCESS)
            {
                //
                // TO DO...
                //
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CcAcceptSpeechCall failed,return 0x%x",0x08100a0c), nRet);
            }

            // g_nStatus = SHELL_STATE_WAIT;
        }
    }

    //hal_HstSendEvent(0xA0CA1100);
    //hal_HstSendEvent(CFW_GetAutoCallCmd(nSimID));
    //hal_HstSendEvent(g_NW_DialEmcOK[nSimID] );
    //hal_HstSendEvent(g_State_TestCard[nSimID]);
	
    //
    // Add for nw init at 20060831
    // SHELL_STATE_WAIT_NW_WITHSIM
    //
    if ((nEvId == EV_CFW_NW_REG_STATUS_IND) && (nType == 2))
    {
        DM_KeyboardInit();
        if (g_nStatus[nSimID] == SHELL_STATE_WAIT_NW_WITHSIM)
        {
            if (nParam1 == CFW_NW_STATUS_REGISTRATION_DENIED)
            {
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, CFW_INIT_SIM_LIMITED_SERVER, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NET);
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Limited Server\n",0x08100a0d));
                g_NW_DialEmcOK[nSimID] = 1;
            }
            else if (nParam1 == CFW_NW_STATUS_REGISTERED_HOME)
            {
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, CFW_INIT_SIM_FULL_SERVER, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NET);
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Full Server\n",0x08100a0e));
                g_NW_DialEmcOK[nSimID] = 1;

            }
            else if (nParam1 == CFW_NW_STATUS_REGISTERED_ROAMING)
            {
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, CFW_INIT_SIM_ROAMING_SERVER, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NET);
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Roaming Server\n",0x08100a0f));
                g_NW_DialEmcOK[nSimID] = 1;

            }
            else
            {
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, CFW_INIT_SIM_NO_SERVER, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_NET);
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("No Server\n",0x08100a10));
                g_NW_DialEmcOK[nSimID] = 0;

            }

            // CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("the value of is CFW_GetAutoCallCmd()  is %d\r\n",0x08100a11), CFW_GetAutoCallCmd());
		hal_HstSendEvent(0xA0CA1101);

		hal_HstSendEvent(CFW_GetAutoCallCmd(nSimID));
		hal_HstSendEvent(g_NW_DialEmcOK[nSimID] );
		hal_HstSendEvent(g_State_TestCard[nSimID]);
			
            if ((0x02 == CFW_GetAutoCallCmd(nSimID)) && g_NW_DialEmcOK[nSimID] && g_State_TestCard[nSimID])
            {
                g_TestMode_AutoResponse[nSimID] = 1;
                g_nStatus[nSimID]               = SHELL_STATE_AUTO_RESP;
            }
            else if ((0x02 == CFW_GetAutoCallCmd(nSimID)) && (!g_NW_DialEmcOK[nSimID]) && g_State_TestCard[nSimID])
            {
                g_nStatus[nSimID] = SHELL_STATE_WAIT_NW_WITHSIM;
            }
            else
            {
                // CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("No Server\n",0x08100a12));

                if ((!g_NW_IND[nSimID]) && (g_SimInit3Status[nSimID] & CFW_INIT_SIM_NORMAL_CARD))
                {
                    // g_nStatus[nSimID] = SHELL_STATE_SIM_IMSI;
                    g_NW_IND[nSimID] = TRUE;
                }
            }

        }
        else
        {
            if (nParam1 == CFW_NW_STATUS_REGISTRATION_DENIED)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Limited Server\n",0x08100a13));
                g_NW_DialEmcOK[nSimID] = 1;

            }
            else if (nParam1 == CFW_NW_STATUS_REGISTERED_HOME)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Full Server\n",0x08100a14));
                g_NW_DialEmcOK[nSimID] = 1;

            }
            else if (nParam1 == CFW_NW_STATUS_REGISTERED_ROAMING)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Roaming Server\n",0x08100a15));
                g_NW_DialEmcOK[nSimID] = 1;

            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("No Server\n",0x08100a16));
                g_NW_DialEmcOK[nSimID] = 0;

            }

            if ((0x01 == CFW_GetAutoCallCmd(nSimID)) && g_State_No_SimCard[nSimID] && g_NW_DialEmcOK[nSimID])
            {
                CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0x00, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_AUTODAIL);
                _SetAutoCallTimer(1500, nSimID);

#if 0
                //
                // Auto dial 112
                //
                if (ERR_SUCCESS == (ret = CFW_CcEmcDial(NULL, 0)))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Auto dial 112 Successl!!!\n",0x08100a17));
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Auto dial 112 failed,The errorcode is 0x%x!!!\n",0x08100a18), ret);
                    while (ERR_SUCCESS != ret)
                    {
                        //
                        // Maybe need timer
                        //
                        COS_Sleep(500);
                        ret = CFW_CcEmcDial(NULL, 0);
                    }
                }
#endif

            }
            else
            {
                ;
            }
        }

    }
    if (g_nStatus[nSimID] == SHELL_STATE_SIM_IMSI)
    {
        if (FALSE == g_IMSI_INIT[nSimID])
        {
            g_nStatus[nSimID] = SHELL_STATE_WAIT;
            CFW_SimGetProviderId(APP_UTI_SHELL, nSimID);
            g_IMSI_INIT[nSimID] = TRUE;
        }
        else
        {
            // Do nothing
        }
    }

    else if (g_nStatus[nSimID] == SHELL_STATE_SMS)
    {
        g_nStatus[nSimID] = SHELL_STATE_WAIT;

        CFW_CfgGetDefaultSmsParam(&g_SMSPInfo[nSimID], nSimID);
        g_SMSPInfo[nSimID].nNumber[0] = 0x00;

        //
        // Get the SCA
        //
        // TODO... need to check the default index in SIM card for SCA.
        //
        ret = CFW_SimGetSmsParameters(0, APP_UTI_SHELL, nSimID);
        if (ret != ERR_SUCCESS)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SimGetSmsParameters failed,ret 0x%x\n",0x08100a19), ret);
            CFW_SmsMoInit(APP_UTI_SHELL, nSimID);
            g_nStatus[nSimID] = SHELL_STATE_WAIT;
        }
    }
    else if (g_nStatus[nSimID] == SHELL_STATE_PBK)
    {
        CFW_PostNotifyEvent(EV_CFW_INIT_IND, 0, 0, 0 | (nSimID << 24), CFW_INIT_STATUS_PBK);
        g_nStatus[nSimID] = SHELL_STATE_WAIT;

        // SHL_PbkPowerOn((0x04)|(0x02));
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_ShellProc);

    return (BAL_PROC_DONT_FREE_MEM | BAL_PROC_COMPLETE);
}

/*
========================================================
Flash的特性是擦除过后全部为1，所以这里认为如果标志位1
则是工位不成功，标志位为0，工位执行成功
另外两个结构体factory_settings_t，bind_info_t是连续存储
在FLASH中
========================================================
*/
// TODO I completely agree with that, we will fight in the shade.
BOOL _CheckValue(void)
{
    return TRUE;
}

BOOL CFW_GetAutoCallFlagFromRam(UINT32 *flag_p)
{
    static BOOL gotStruct             = FALSE;
    static STACK_AUTO_CALL_T autoCall = { 0, };

    if (!gotStruct)
    {
        gotStruct = TRUE;
        STACK_AUTO_CALL_T *ramPtr = stack_GetAutoCallStruct();
        if (ramPtr != NULL)
        {
            autoCall = *ramPtr;

            // Ram flag is valid for this boot only
            ramPtr->validity = 0;
            // Clear the flag and ready to receive the result
            ramPtr->flag = 0;
        }
    }

    if (autoCall.validity != STACK_AUTO_CALL_VALID)
    {
        return FALSE;
    }

    if (flag_p)
    {
        *flag_p = autoCall.flag;
    }
    return TRUE;
}

BOOL CFW_AutoCallResultAvail(VOID)
{
    if (!CFW_GetAutoCallFlagFromRam(NULL))
    {
        return FALSE;
    }

    STACK_AUTO_CALL_T *ramPtr = stack_GetAutoCallStruct();
    // Distinguish the result flag (1) from the auto call flag (>=0x10000)
    // and the null state (0).
    if (ramPtr == NULL || ramPtr->flag != 1)
    {
        return FALSE;
    }

    return TRUE;
}

VOID CFW_SetAutoCallResultSavingStatus(UINT32 status)
{
    if (!CFW_GetAutoCallFlagFromRam(NULL))
    {
        return;
    }

    STACK_AUTO_CALL_T *ramPtr = stack_GetAutoCallStruct();
    if (ramPtr == NULL)
    {
        return;
    }

    ramPtr->flag = 0x80000000 | status;

    return;
}

//
// Return value:
// 0x00:No test mode, 0x01: auto dial 112, 0x02:Auto response
//

UINT8 CFW_GetAutoCallCmd(CFW_SIM_ID nSimID)
{
    UINT32 mark = 0;

    if (FALSE == CFW_GetAutoCallFlagFromRam(&mark))
    {
        return 0xFF;
    }

    mark = ((mark >> 16) & 0x03);
    if (AUTO_CALL_SETTIGN_DIAL == mark)
    {
        // Auto dial 112
        return 0x01;
    }
    else if (AUTO_CALL_SETTING_RESP == mark)
    {
        // Auto response
        return 0x02;
    }

    return 0x00;
}

BOOL CFW_GetAutoCallFreqBand(UINT8 *pBand)
{
    UINT32 mark = 0;

    if (FALSE == CFW_GetAutoCallFlagFromRam(&mark))
    {
        return FALSE;
    }

    mark = ((mark >> 18) & 0x0F);
    if (mark)
    {
        // 第18位是GSM850 的功能位， 19位是GSM900的功能位，20是DCS1800的功能位，21是PCS1900的功能位。
        if (0x01 == (mark & 0x01))
        {
            *pBand = API_GSM_850;
        }

        if (0x01 == ((mark >> 1) & 0x01))
        {
            *pBand |= (API_GSM_900E | API_GSM_900P);
        }

        if (0x01 == ((mark >> 2) & 0x01))
        {
            *pBand |= API_DCS_1800;
        }

        if (0x01 == ((mark >> 3) & 0x01))
        {
            *pBand |= API_PCS_1900;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

UINT8 CFW_GetEMCStatus(CFW_SIM_ID nSimID)
{
    return g_NW_DialEmcOK[nSimID];
}

VOID CFW_EMCDial(CFW_SIM_ID nSimID)
{
    UINT32 ret = 0x00000000;

    //
    // Auto Dial 112
    //
    g_TestMode_AutoEMCDial[nSimID] = 1;

    if (g_TestMode_AutoEMCDialNum[nSimID] < DIAL_EMC_MAXNUM)
    {
        g_TestMode_AutoEMCDialNum[nSimID]++;
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Call 112 %x times\r\n",0x08100a1c)), g_TestMode_AutoEMCDialNum[nSimID]);
    }
    else
    {
        return;
    }

    COS_Sleep(500);

    if (ERR_SUCCESS == (ret = CFW_CcEmcDial(NULL, 0, nSimID)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Auto Call 112 Success\n",0x08100a1d));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Auto Call 112 failed,The errorcode 0x%x\n",0x08100a1e), ret);
        while (ERR_SUCCESS != ret)
        {
            //
            // Maybe need timer
            //
            COS_Sleep(500);
            ret = CFW_CcEmcDial(NULL, 0, nSimID);
        }
    }
}

extern UINT8 nCommondNum[][CFW_SIM_COUNT];
// COS_FreeEvent
//


UINT32 cfw_SatProCmdSetupMenuEX(CFW_SIM_ID nSimID, UINT8 *pData, UINT8 nLength)
{
    UINT8 i         = 0;
    UINT8 nIndex    = 0;
    UINT16 nMemsize = 0;  // memory size
    UINT8 nAlphaLen = 0;
    UINT32 nItemNum = 0;
    UINT8 *pOffset  = pData;

    CFW_SAT_MENU_RSP *pMenuItemList = NULL;
    CFW_SAT_ITEM_LIST *pItemList = NULL;
    SIM_SAT_PARAM *pG_SimSat = NULL;
    UINT32 nUTI = GENERATE_SHELL_UTI();


    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);
    nIndex = 5;

    if ((pData[nIndex] == 0x05) || (pData[nIndex] == 0x85))
    {
        if (pData[++nIndex] == 0x81)
            nIndex++;
        nAlphaLen = pData[nIndex++];
        i         = nIndex + nAlphaLen;
    }
    else
    {
        return ERR_SUCCESS;
    }
    nMemsize = SIZEOF(CFW_SAT_MENU_RSP) + nAlphaLen;

    // nLength = nLength - i;
    while (i < nLength)
    {
        if ((pOffset[i] != 0x0F) && (pOffset[i] != 0x8F))
            break;
        nItemNum++;
        nMemsize += SIZEOF(CFW_SAT_ITEM_LIST);

        // nMemsize += pOffset[++i] - 1;
        nMemsize += pOffset[++i];
        i = i + pOffset[i];
        i++;
    }

    // ??
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize: 0x%x\n",0x08100a1f), nMemsize);

    pMenuItemList = (CFW_SAT_MENU_RSP *)CSW_SIM_MALLOC(nMemsize);
    if (pMenuItemList == NULL)
    {
        return ERR_SUCCESS;
    }
    SUL_ZeroMemory8(pMenuItemList, (nMemsize));
    pMenuItemList->nComID = 0x25;

    pMenuItemList->nComQualifier = *pData;
    pMenuItemList->nAlphaLen     = nAlphaLen;
    nItemNum                     = SIZEOF(CFW_SAT_ITEM_LIST) * nItemNum;

    pMenuItemList->pAlphaStr = SIZEOF(CFW_SAT_MENU_RSP) + (UINT8 *)pMenuItemList + nItemNum;
    SUL_MemCopy8(pMenuItemList->pAlphaStr, (pOffset + nIndex), nAlphaLen);
    nItemNum                 = (UINT32)(pMenuItemList->pAlphaStr) + nAlphaLen;
    pMenuItemList->pItemList = (CFW_SAT_ITEM_LIST *)(SIZEOF(CFW_SAT_MENU_RSP) + (UINT8 *)pMenuItemList);

    pItemList = pMenuItemList->pItemList;
    i         = nIndex + nAlphaLen;

    CFW_SAT_ITEM_LIST *pTemp = NULL;
    for (;;)
    {
        if ((pOffset[i] != 0x0F) && (pOffset[i] != 0x8F))
        {
            pTemp->pNextItem = NULL;
            break;
        }
        pItemList->nItemLen = pOffset[++i] - 1; // reduce the item's id
        pItemList->nItemID  = pOffset[++i];
        pItemList->pItemStr = (UINT8 *)nItemNum;
        nItemNum += pItemList->nItemLen;

        SUL_MemCopy8(pItemList->pItemStr, (pOffset + i + 1), pItemList->nItemLen);
        pItemList->pNextItem = (CFW_SAT_ITEM_LIST *)(SIZEOF(CFW_SAT_ITEM_LIST) + (UINT8 *)pItemList);

        // CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("SM_ACT_FETCH\n",0x08100a20));
        i = i + pItemList->nItemLen;
        if (++i < nLength)
        {
            pTemp     = pItemList;
            pItemList = pItemList->pNextItem;
        }
        else
        {
            pItemList->pNextItem = NULL;
            break;
        }
    }

    if (pG_SimSat->pSetupMenu != NULL)
        CSW_SIM_FREE(pG_SimSat->pSetupMenu);
    pG_SimSat->pSetupMenu = pMenuItemList;
    g_SAT_INIT_COMPLETE[nSimID] = TRUE; // g__SAT_INIT_COMPLETE  to determine whether init is complete??

    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->pSetupMenu->nComID, 0, nUTI | (nSimID << 24), 0x0);
    return ERR_SUCCESS; // [+ ] dingmx 20080908 remove warning
}
extern BOOL gSatFreshComm[] ;

/*
Command details
Byte(s) Description Length
1   Command details tag 1
2   Length = '03'       1
3   Command number      1
4   Type of command     1
5   Command Qualifier   1
*/

typedef struct _SAT_COMMAND_DETAILS
{
    UINT8 nLen;
    UINT8 nCmdNum;
    UINT8 nTypeOfCmd;
    UINT8 nCmdQual;
} SAT_COMMAND_DETAILS;

SAT_COMMAND_DETAILS *_CheckCommandDetails(UINT8 *pData, UINT8 *pLen)
{
    if((0x01 != pData[0]) && (0x81 != pData[0]))
    {
        return NULL;
    }

    SAT_COMMAND_DETAILS *p = (SAT_COMMAND_DETAILS *)CSW_SIM_MALLOC(SIZEOF(SAT_COMMAND_DETAILS));
    SUL_ZeroMemory8(p, SIZEOF(SAT_COMMAND_DETAILS));

    p->nLen         = pData[1];
    p->nCmdNum      = pData[2];
    p->nTypeOfCmd   = pData[3];
    p->nCmdQual     = pData[4];

    *pLen = 1 + 1 + p->nLen;
    return p;
}


/*
Device identities
Byte(s) Description Length
1   Device identities tag   1
2   Length = '02'   1
3   Source device identity  1
4   Destination device identity 1
*/
typedef struct _SAT_DEVICE_IDENTITIES
{
    UINT8 nLen;
    UINT8 nSrcDevIde;
    UINT8 nDesDevIde;
} SAT_DEVICE_IDENTITIES;

SAT_DEVICE_IDENTITIES *_CheckDeviceIdentities(UINT8 *pData, UINT8 *pLen)
{
    if((0x02 != pData[0]) && (0x82 != pData[0]))
    {
        return NULL;
    }

    SAT_DEVICE_IDENTITIES *p = (SAT_DEVICE_IDENTITIES *)CSW_SIM_MALLOC(SIZEOF(SAT_DEVICE_IDENTITIES));
    SUL_ZeroMemory8(p, SIZEOF(SAT_DEVICE_IDENTITIES));

    p->nLen         = pData[1];
    p->nSrcDevIde   = pData[2];
    p->nDesDevIde   = pData[3];

    *pLen = 1 + 1 + p->nLen;
    return p;
}

/*
Alpha identifier
Byte(s) Description Length
1   Alpha identifier tag    1
2 to (Y 1)+2    Length (X)  Y1
(Y 1)+3 to (Y 1)+X+2    Alpha identifier    X

*/
typedef struct _SAT_ALPHA_IDENTIFIER
{
    UINT8 nLen;
    UINT8 pAlpIde[1];
} SAT_ALPHA_IDENTIFIER;

SAT_ALPHA_IDENTIFIER *_CheckAlphaIdentifier(UINT8 *pData, UINT8 *pLen)
{
    if((0x05 != pData[0]) && (0x85 != pData[0]))
    {
        return NULL;
    }

    SAT_ALPHA_IDENTIFIER *p = (SAT_ALPHA_IDENTIFIER *)CSW_SIM_MALLOC(SIZEOF(SAT_ALPHA_IDENTIFIER) + pData[1]);
    SUL_ZeroMemory8(p, SIZEOF(SAT_ALPHA_IDENTIFIER) + pData[1]);

    p->nLen         = pData[1];
    SUL_MemCopy8(p->pAlpIde, p, pData[1]);

    *pLen = 1 + 1 + p->nLen;

    return p;
}

/*
12.44   DTMF string
Byte(s) Description Length
1   DTMF String tag 1
2 to (Y-1)+2    Length (X)  Y
(Y-1)+3 to (Y-1)+3+X-1  DTMF string X

Contents :

*/

typedef struct _SAT_DTMF_STRING
{
    UINT8 nLen;
    UINT8 pDTMFStr[1];
} SAT_DTMF_STRING;

SAT_DTMF_STRING *_CheckDTMFString(UINT8 *pData, UINT8 *pLen)
{
    if((0x05 != pData[0]) && (0x85 != pData[0]))
    {
        return NULL;
    }

    SAT_DTMF_STRING *p = (SAT_DTMF_STRING *)CSW_SIM_MALLOC(SIZEOF(SAT_DTMF_STRING) + pData[1]);
    SUL_ZeroMemory8(p, SIZEOF(SAT_DTMF_STRING) + pData[1]);

    p->nLen         = pData[1];
    SUL_MemCopy8(p->pDTMFStr, p, pData[1]);

    *pLen = 1 + 1 + p->nLen;

    return p;
}

/*
Icon Identifier
Byte(s) Description Length
1   Icon identifier tag 1
2   Length = '02'   1
3   Icon qualifier  1
4   Icon identifier 1
*/
typedef struct _SAT_ICON_IDENTIFIER
{
    UINT8 nLen;
    UINT8 nIconQual;
    UINT8 nIconIde;
} SAT_ICON_IDENTIFIER;

SAT_ICON_IDENTIFIER *_CheckIconIdentifier(UINT8 *pData, UINT8 *pLen)
{
    if((0x1E != pData[0]) && (0x9E != pData[0]))
    {
        return NULL;
    }

    SAT_ICON_IDENTIFIER *p = (SAT_ICON_IDENTIFIER *)CSW_SIM_MALLOC(SIZEOF(SAT_ICON_IDENTIFIER));
    SUL_ZeroMemory8(p, SIZEOF(SAT_ICON_IDENTIFIER));

    p->nLen         = pData[1];
    p->nIconQual    = pData[2];
    p->nIconIde     = pData[3];

    *pLen = 1 + 1 + p->nLen;
    return p;
}

/*
SEND DTMF COMMAND

Description Section M/O Min Length
Proactive SIM command Tag   13.2    M   Y   1
Length (A+B+C+D+E)  -               M   Y   1 or 2
Command details 12.6                M   Y   A
Device Identities   12.7            M   Y   B
Alpha Identifier    12.2            O   N   C
DTMF String 12.44                   M   Y   D
Icon identifier 12.31               O   N   E

*/
UINT32 _SatProCmdSendDTMF(  UINT8 *pData,
                            UINT8 nLength
                            , CFW_SIM_ID nSimID
                         )
{
    UINT8 i         = 0;
    UINT8 nIndex    = 0;
    UINT16 nMemsize = 0;  // memory size
    UINT8 nAlphaLen = 0;
    UINT32 nItemNum = 0;
    UINT8 *pOffset  = pData;

    CFW_SAT_MENU_RSP *pMenuItemList = NULL;
    CFW_SAT_ITEM_LIST *pItemList = NULL;
    SIM_SAT_PARAM *pG_SimSat = NULL;
    UINT32 nUTI = GENERATE_SHELL_UTI();

    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);
    nIndex = 5;
    UINT8 nLen  = nLength;
    UINT8 *p = pData;
    //VOID *p     = NULL;

    SAT_COMMAND_DETAILS *pCmdDetatls = _CheckCommandDetails(p += 2, &nLen);
    if(NULL == pCmdDetatls)
    {
        return ERR_INVALID_PARAMETER;
    }

    if(0x14 != pCmdDetatls->nTypeOfCmd)
    {
        CSW_SIM_FREE(pCmdDetatls);
        return ERR_INVALID_PARAMETER;
    }

    p += nLen;

    // SAT_DEVICE_IDENTITIES *pDevIdentities = _CheckDeviceIdentities(p,&nLen);

    if ((pData[nIndex] == 0x05) || (pData[nIndex] == 0x85))
    {
        if (pData[++nIndex] == 0x81)
            nIndex++;
        nAlphaLen = pData[nIndex++];
        i         = nIndex + nAlphaLen;
    }
    else
    {
        return ERR_SUCCESS;
    }
    nMemsize = SIZEOF(CFW_SAT_MENU_RSP) + nAlphaLen;

    // nLength = nLength - i;
    while (i < nLength)
    {
        if ((pOffset[i] != 0x0F) && (pOffset[i] != 0x8F))
            break;
        nItemNum++;
        nMemsize += SIZEOF(CFW_SAT_ITEM_LIST);

        // nMemsize += pOffset[++i] - 1;
        nMemsize += pOffset[++i];
        i = i + pOffset[i];
        i++;
    }

    // ??
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize: 0x%x\n",0x08100a21), nMemsize);

    pMenuItemList = (CFW_SAT_MENU_RSP *)CSW_SIM_MALLOC(nMemsize);
    if (pMenuItemList == NULL)
    {
        return ERR_SUCCESS;
    }
    SUL_ZeroMemory8(pMenuItemList, (nMemsize));
    pMenuItemList->nComID = 0x25;

    pMenuItemList->nComQualifier = *pData;
    pMenuItemList->nAlphaLen     = nAlphaLen;
    nItemNum                     = SIZEOF(CFW_SAT_ITEM_LIST) * nItemNum;

    pMenuItemList->pAlphaStr = SIZEOF(CFW_SAT_MENU_RSP) + (UINT8 *)pMenuItemList + nItemNum;
    SUL_MemCopy8(pMenuItemList->pAlphaStr, (pOffset + nIndex), nAlphaLen);
    nItemNum                 = (UINT32)(pMenuItemList->pAlphaStr) + nAlphaLen;
    pMenuItemList->pItemList = (CFW_SAT_ITEM_LIST *)(SIZEOF(CFW_SAT_MENU_RSP) + (UINT8 *)pMenuItemList);

    pItemList = pMenuItemList->pItemList;
    i         = nIndex + nAlphaLen;

    // CFW_SAT_ITEM_LIST* pTemp = NULL;


    if (pG_SimSat->pSetupMenu != NULL)
        CSW_SIM_FREE(pG_SimSat->pSetupMenu);
    pG_SimSat->pSetupMenu = pMenuItemList;
    g_SAT_INIT_COMPLETE[nSimID] = TRUE; // g__SAT_INIT_COMPLETE  to determine whether init is complete??

    CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->pSetupMenu->nComID, 0, nUTI | (nSimID << 24), 0x0);
    return ERR_SUCCESS; // [+ ] dingmx 20080908 remove warning
}

extern UINT32 gSATTimerValue[][CFW_SIM_COUNT];
UINT8 _GetTimerIdentifier(UINT8 *p);
UINT32 _GetTimerValue(UINT8 *p);

UINT32 CSW_SAT_InitEX(CFW_EV *pEvent)
{

    SIM_SAT_PARAM *pG_SimSat = NULL;
    UINT32 POSSIBLY_UNUSED nRet;

    CFW_SIM_ID nSimID = pEvent->nFlag;
    UINT32 nUTI = GENERATE_SHELL_UTI() | (nSimID << 24) ;
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);

    if (pEvent->nEventId == API_SIM_FETCH_IND)
    {
        UINT32 nReturnValue = processFetchInd(pEvent, nUTI, nSimID);
        return nReturnValue;
    }
    return ERR_SUCCESS;
}


// position service status, according to sat specs.
#define NORMAL_SERVICE 0x0
#define LIMIT_SERVICE 0x1
#define NO_SERVICE 0x2

#ifdef CHIP_HAS_AP
extern VOID calib_DaemonSetAutoCallMode(BOOL autocall);
#endif

BOOL _SetAutoCallTimer(UINT32 nElapse, CFW_SIM_ID nSimID)
{
    static BOOL bSACFlag = FALSE;

    BOOL bRet  = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_AUTO_CALL_TIMER_SIM0_ID + ((UINT8)nSimID),
                                COS_TIMER_MODE_SINGLE,  nElapse * 16384 / 1000);

    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTR("Auto dial _SetAutoCallTiemr %d \n",0x08100a22), bRet);
    //CALIB_CALIBRATION_T *calibPtr =  calib_GetPointers();
    //calibPtr->hstSw->param[13] = 0x00;

    if( !bSACFlag )
    {
        bSACFlag = TRUE;

        //Write flag to 0
        STACK_AUTO_CALL_T *ramPtr = stack_GetAutoCallStruct();
        if (ramPtr != NULL)
        {
            ramPtr->flag = 0;
        }
    }
    return bRet;
}


VOID _AutoCallTimerProc(CFW_SIM_ID nSimID)
{
    BOOL status;

    if(CFW_AutoCallResultAvail())
    {
        //Set Auto call flag to 0,and save the calib data to flash
        //calibPtr->hstSw->param[13] = 0x00;
        CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(0), TSTR("Let's calib_DaemonUsrDataProcess \n",0x08100a23));
#ifdef CHIP_HAS_AP
        calib_DaemonSetAutoCallMode(TRUE);
#endif
        status = calib_DaemonUsrDataProcess();
        CFW_SetAutoCallResultSavingStatus(status);
    }
    else
    {
        _SetAutoCallTimer(1500, nSimID);
    }

}


#define SIM_SAT_EVENT_DOWNLOAD          0xD6
UINT8 *_SIMPLE_TLV_DATA_OBJECTS_CHANNEL_STATUS(UINT8 nChannelStatus, UINT8 *pLen);

UINT32 CFW_SAT_Location_Status(api_NwCsRegStatusInd_t *pCsRegStatus, CFW_SIM_ID nSimID)
{
    // FIXME, 256 is ok?
    UINT8 buf[100];
    INT cur_pos = 0;
    UINT8 status = 0;
    UINT8 len = 0;
    UINT8 UTI = 0;
    UINT8 nLei[3] = {0x0,};
    UINT16 nLac = 0x0;
    UINT16 nCellID = 0x0;
    UINT8 *p_pos_status = NULL, *p_pos_info = NULL;
    //mmi_nw_attach_ind_struct *local_data = (mmi_nw_attach_ind_struct *)param;
    CSW_TRACE(CFW_SHELL_TS_ID, (const U8 *)" SAT, [%s] file, [%s] func [%d] line", __FILE__, __func__, __LINE__);

    // 1. mark download event type tag
    //MARK_DOWNLOAD_EVENT_TAG(buf, SAT_SETUP_EVENT_LIST__EVT_POSITOIN_STATUS);
    buf[0] = 0x03;
    cur_pos += 1;

    // 2. position status
    CSW_TRACE(CFW_SHELL_TS_ID, (const U8 *)"terminalDownloadEventPositionStatus pCsRegStatus->status is [%x]", pCsRegStatus->Status);
    switch(pCsRegStatus->Status)
    {
        case API_NW_FULL_SVCE:
            status = NORMAL_SERVICE;
            break;
        case API_NW_NO_SVCE:
            status = NO_SERVICE;
            break;
        case API_NW_LTD_SVCE:
            status = LIMIT_SERVICE;
            break;
        default:
            CSW_TRACE(CFW_SHELL_TS_ID, (const U8 *)"terminalDownloadEventPositionStatus local_data->status unknown");
            return ERR_INVALID_PARAMETER;
    }
    p_pos_status = _SIMPLE_TLV_DATA_OBJECTS_CHANNEL_STATUS(status, &len);
    sxs_Dump(CFW_SHELL_TS_ID, 0, p_pos_status, len);
    if(len > 0)
    {
        memcpy(buf + cur_pos, p_pos_status, len);
    }
    cur_pos += len;

    // 3. position info
    if(NORMAL_SERVICE == status)
    {
        memcpy(nLei, pCsRegStatus->LAI, 3);
        nLac = pCsRegStatus->LAI[4];
        nLac = (nLac << 8) | ((UINT8)pCsRegStatus->LAI[3]);

        nCellID = pCsRegStatus->CellId[1];
        nCellID = (nCellID << 8) | ((UINT8)pCsRegStatus->CellId[0]);

        p_pos_info = _SIMPLE_TLV_DATA_OBJECTS_LOCATION_INFORMATION((UINT8 *)nLei,(UINT16 *)&nLac, (UINT16 *)&nCellID, (UINT8 *)&len);
        sxs_Dump(CFW_SHELL_TS_ID, 0, p_pos_info, len);
        if(len > 0)
        {
            memcpy(buf + cur_pos, p_pos_info, len);
        }
        cur_pos += len;
    }

    // 4. request
    CFW_GetFreeUTI(0, &UTI);
    CFW_SatResponse (SIM_SAT_EVENT_DOWNLOAD, 0, 0, buf, cur_pos, UTI, (CFW_SIM_ID)nSimID);

    // free buf
    if(p_pos_info)
        CSW_Free(p_pos_info);
    if(p_pos_status)
        CSW_Free(p_pos_status);

    return ERR_SUCCESS;
}

VOID CFW_SimUnRegisterAll(
    CFW_SIM_ID nSimID
)
{
    HAO hAo        = 0;  // ao
    UINT8 nAoCount = 0;
    UINT8 nAoIndex = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimUnRegisterAll);
    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsDeactiveAll start\n",0x08100a24)));

    nAoCount = CFW_GetAoCount(CFW_SIM_SRV_ID, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_GprsDeactiveAll nAoCount: %d\n",0x08100a25)), nAoCount);

    for (nAoIndex = 0; nAoIndex < nAoCount; nAoIndex++)
    {
        hAo = CFW_GetAoHandle(nAoIndex, CFW_SM_STATE_UNKONW, CFW_SIM_SRV_ID, nSimID);

        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SHELL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GprsDeactiveAll end\n",0x08100a26)));

    CSW_PROFILE_FUNCTION_EXIT(CFW_SimUnRegisterAll);
}



