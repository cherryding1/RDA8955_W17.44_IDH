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


#include "sxs_io.h"
#include <string.h>

#include "hal_host.h"
#include <cswtype.h>
#include <errorcode.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
// #include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_sat.h"
#include "cfw_sim_cb.h"
#include "cfw_usim.h"
#ifdef CB_SUPPORT
#include "cfw_sim_boot.h"
#endif

#define CFW_CB_TS_ID  1
#define CFW_SIM_TS_ID_DUMP  (CFW_CB_TS_ID)
#define CFW_CB_MAX_CH 20
#define CFW_CB_MAX_DCS 17
UINT32 CFW_CbSimReadProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_CbAoProc (HAO hAo, CFW_EV *pEvent);
HAO CB_Rec_Reg(CFW_EV *pEvent);
UINT32 CFW_SimSetMidListProc(HAO hAo, CFW_EV *pEvent);
extern CFW_SIMSTATUS g_cfw_sim_status[];

extern UINT32 CFW_SimPatchEX(UINT16 nFileID, UINT16 nUTI, CFW_SIM_ID nSimID);

#define CB_DEACTIVED     0
#define CB_ACTIVED         1
#define CB_ACTIVING        2
#define CB_DEACTIVING    3

#define  API_SIM_EF_CBMI     26
#define  API_SIM_EF_CBMID     28
#define  API_SIM_EF_CBMIR     42
#define  API_SIM_EF_LP          15
#define  API_SIM_EF_ELP          54

CB_CTX_T cbCtx[CFW_SIM_COUNT];

VOID CFW_CbGetSimContext(CFW_SIM_ID nSimID)
{
    UINT8 simuti;

    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CbGetSimContext\n",0x08100f5a));

    CFW_RegisterCreateAoProc(API_SMSCB_RECEIVE_IND, (PFN_EV_IND_TRIGGER_PORC)CB_Rec_Reg);
    CFW_RegisterCreateAoProc(API_SMSCB_PAGE_IND, (PFN_EV_IND_TRIGGER_PORC)CB_Rec_Reg);
    memset(cbCtx[nSimID].Dcs, 0xFF, sizeof(cbCtx[nSimID].Dcs));
    memset(cbCtx[nSimID].Mid, 0xFFFF, sizeof(cbCtx[nSimID].Mid));

    if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
    {
        CFW_GetFreeUTI(CFW_SIM_SRV_ID, &simuti);
        CFW_CbSimRead(BINARY_READ, API_USIM_EF_CBMI, simuti, nSimID);
        CFW_GetFreeUTI(CFW_SIM_SRV_ID,&simuti);
        CFW_CbSimRead(BINARY_READ,API_USIM_EF_CBMID,simuti,nSimID);
        CFW_GetFreeUTI(CFW_SIM_SRV_ID,&simuti);
        CFW_CbSimRead(BINARY_READ,API_USIM_EF_CBMIR,simuti,nSimID);
    }
    else
    {
        CFW_GetFreeUTI(CFW_SIM_SRV_ID, &simuti);
        CFW_CbSimRead(BINARY_READ, API_SIM_EF_CBMI, simuti, nSimID);
        CFW_GetFreeUTI(CFW_SIM_SRV_ID,&simuti);
        CFW_CbSimRead(BINARY_READ,API_SIM_EF_CBMID,simuti,nSimID);
        CFW_GetFreeUTI(CFW_SIM_SRV_ID,&simuti);
        CFW_CbSimRead(BINARY_READ,API_SIM_EF_CBMIR,simuti,nSimID);
    }
    CFW_GetFreeUTI(CFW_SIM_SRV_ID, &simuti);
    CFW_CbSimRead(BINARY_READ, API_SIM_EF_LP, simuti, nSimID);
    CFW_GetFreeUTI(CFW_SIM_SRV_ID, &simuti);
    CFW_CbSimRead(BINARY_READ, API_SIM_EF_ELP, simuti, nSimID);
    //only use to test for debug code start

    return ;
}

typedef struct _CB_READFILE_
{
    UINT8   nSimIndex;
    UINT8   CbSimFileList[5];
}CB_READFILE;
CB_READFILE gCbReadFile[CFW_SIM_COUNT];

UINT32 CFW_CbReadFile(CFW_SIM_ID nSimID)
{
    UINT8 simuti;
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {	
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100b97), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenticationStatus);
        return ret;
    }

    CSW_TRACE(CFW_SMS_TS_ID, TSTXT("CFW_CbReadFile, nSimIndex:%d \n"), gCbReadFile[nSimID].nSimIndex);
    if( gCbReadFile[nSimID].nSimIndex >= 5)
    {
        CSW_TRACE(CFW_SMS_TS_ID, TSTXT("CFW_CbReadFile,error nSimIndex:%d \n"), gCbReadFile[nSimID].nSimIndex);
	return ERR_INVALID_INDEX;
    }

    CFW_GetFreeUTI(CFW_SIM_SRV_ID, &simuti);
    CFW_CbSimRead(BINARY_READ, gCbReadFile[nSimID].CbSimFileList[gCbReadFile[nSimID].nSimIndex] , simuti, nSimID);

    gCbReadFile[nSimID].nSimIndex++;

    return ERR_SUCCESS;
}

VOID CFW_CbInit(CFW_SIM_ID nSimID)
{
    UINT8 simuti;

    CSW_TRACE(CFW_CB_TS_ID, "CFW_CbInit\n");
    CFW_RegisterCreateAoProc(API_SMSCB_RECEIVE_IND, (PFN_EV_IND_TRIGGER_PORC)CB_Rec_Reg);
    
    memset(&gCbReadFile[nSimID], 0x00, sizeof( CB_READFILE));
    if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
    {
	gCbReadFile[nSimID].nSimIndex = 0;
	gCbReadFile[nSimID].CbSimFileList[0]=API_USIM_EF_CBMI;
	gCbReadFile[nSimID].CbSimFileList[1]=API_USIM_EF_CBMID;
	gCbReadFile[nSimID].CbSimFileList[2]=API_USIM_EF_CBMIR;
	gCbReadFile[nSimID].CbSimFileList[3]=API_SIM_EF_LP;
	gCbReadFile[nSimID].CbSimFileList[4]=API_SIM_EF_ELP;
    }  
    else
    {
	gCbReadFile[nSimID].nSimIndex = 0;
	gCbReadFile[nSimID].CbSimFileList[0]=API_SIM_EF_CBMI;
	gCbReadFile[nSimID].CbSimFileList[1]=API_SIM_EF_CBMID;
	gCbReadFile[nSimID].CbSimFileList[2]=API_SIM_EF_CBMIR;
	gCbReadFile[nSimID].CbSimFileList[3]=API_SIM_EF_LP;
	gCbReadFile[nSimID].CbSimFileList[4]=API_SIM_EF_ELP;
    }

    CFW_CbReadFile(nSimID);

    return ;
}

CB_CTX_T cbCtx[CFW_SIM_COUNT];

UINT32 CFW_CbSimRead( UINT8 nMode, UINT8 FileId, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO CFW_CbSimReadAo;
    CFW_CB_SIM_READ_REQ *pCbSimReadReq;

    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(2), TSTR("CFW_CbSimRead, nMode = 0x%x, fileId: %d\n",0x08100f5b), nMode, FileId);

    CSW_PROFILE_FUNCTION_ENTER(CFW_CbSimRead);
    if(nMode > 2)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CbSimRead);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CbSimRead);
        return ERR_CFW_UTI_EXIST;
    }

    pCbSimReadReq = (CFW_CB_SIM_READ_REQ *)CSW_SIM_MALLOC(SIZEOF(CFW_CB_SIM_READ_REQ));
    if(pCbSimReadReq == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CbSimRead);
        return ERR_NO_MORE_MEMORY;
    }
    pCbSimReadReq->Mode = nMode;
    pCbSimReadReq->FileId = FileId;

    CFW_CbSimReadAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pCbSimReadReq, CFW_CbSimReadProc, nSimID);
    CFW_SetUTI(CFW_CbSimReadAo, nUTI, 0);

    CFW_SetAoProcCode(CFW_CbSimReadAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CbSimRead);
    return(ERR_SUCCESS);
}


//#define  API_SIM_EF_CBMI     26
//#define  API_SIM_EF_CBMID     28
//#define  API_SIM_EF_CBMIR     42

//CbType: 0,1;
// Possible values for ItfType
//#define API_SMSCB_ITF_MSG   0 // Provides complete SMSCB message to MMI
// (API_SMSCB_RECEIVE_IND)
//#define API_SMSCB_ITF_PAGE  1 // Provides SMSCB page to MMI (API_SMSCB_PAGE_IND)
UINT32 CFW_CbAct( UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO CFW_CbActAo;
    CFW_CB_ACTIVE_REQ *pCbActReq;
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("MESSAGES_CB %s, nSimID = 0x%x\n",0x08100f5c),__FUNCTION__,nSimID);
    if (CFW_IsFreeUTI(nUTI, CFW_SMS_CB_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CbAct, UTI ERROR!\n",0x08100f5d));
        return ERR_CFW_UTI_EXIST;
    }
    pCbActReq = (CFW_CB_ACTIVE_REQ *)CSW_SIM_MALLOC(SIZEOF(CFW_CB_ACTIVE_REQ));
    if(pCbActReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CbAct, NO MEMORY ERROR!\n",0x08100f5e));
        return ERR_NO_MORE_MEMORY;
    }

    pCbActReq->CbType = API_SMSCB_ITF_MSG;
    pCbActReq->MsgId = API_SMSCB_ACTIVATE_REQ;

    CFW_RegisterCreateAoProc(API_SMSCB_RECEIVE_IND, (PFN_EV_IND_TRIGGER_PORC)CB_Rec_Reg);
    CFW_CbActAo = CFW_RegisterAo(CFW_SMS_CB_ID, pCbActReq, CFW_CbAoProc, nSimID);
    CFW_SetUTI(CFW_CbActAo, nUTI, 0);

    CFW_SetAoProcCode(CFW_CbActAo, CFW_AO_PROC_CODE_CONTINUE);

    return(ERR_SUCCESS);
}

UINT32 CFW_CbDeAct(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO CFW_CbActAo;
    CFW_CB_ACTIVE_REQ *pCbActReq;
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_CbDeAct  nSimID =%d\n",0x08100f5f),nSimID);

    if (CFW_IsFreeUTI(nUTI, CFW_SMS_CB_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CbDeAct, UTI ERROR!\n",0x08100f60));
        return ERR_CFW_UTI_EXIST;
    }

    pCbActReq = (CFW_CB_ACTIVE_REQ *)CSW_SIM_MALLOC(SIZEOF(CFW_CB_ACTIVE_REQ));
    if(pCbActReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_CbDeAct, NO MEMORY ERROR!\n",0x08100f61));
        return ERR_NO_MORE_MEMORY;
    }

    pCbActReq->CbType = API_SMSCB_ITF_MSG;
    pCbActReq->MsgId = API_SMSCB_DEACTIVATE_REQ;

    CFW_CbActAo = CFW_RegisterAo(CFW_SMS_CB_ID, pCbActReq, CFW_CbAoProc, nSimID);
    CFW_SetUTI(CFW_CbActAo, nUTI, 0);

    CFW_SetAoProcCode(CFW_CbActAo, CFW_AO_PROC_CODE_CONTINUE);

    return(ERR_SUCCESS);
}

extern CFW_UsimEfStatus gUsimEfStatus[CFW_SIM_COUNT];
extern VOID CFW_UsimDecodeEFFcp (UINT8 *RespData, CFW_UsimEfStatus *pSimEfStatus);

UINT32 CFW_CbSimReadProc (HAO hAo, CFW_EV *pEvent)
{
    HAO CFW_CbSimReadAo = hAo;

    CFW_CB_SIM_READ_REQ *pCbSimReadReq = NULL;
    UINT32 nUTI   = 0;
    UINT32 nEvtId = 0;;
    VOID *nEvParam;
    CFW_EV ev;
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTR("In CFW_CbSimReadProc, pEvent:0x%x \n",0x08100f62), pEvent);

    if((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("In CFW_CbSimReadProc \n",0x08100f63));
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
    }
    if( (hAo == 0) || (pEvent == NULL))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        CFW_SetServiceId(CFW_SIM_SRV_ID);
        nEvParam = (VOID *)pEvent->nParam1;
        pCbSimReadReq = CFW_GetAoUserData(CFW_CbSimReadAo);
        nEvtId = pEvent->nEventId;
    }

    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(CFW_CbSimReadAo);

    CFW_GetUTI(CFW_CbSimReadAo, &nUTI);
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("pCbSimReadReq->Mode: %x\n",0x08100f64)), pCbSimReadReq->Mode);
    if(nEvParam == (VOID *)0xff)
    {
        switch (pCbSimReadReq->Mode)
        {
            case BINARY_READ:
                SimElemFileStatusReq(pCbSimReadReq->FileId, nSimID);
                break;
            default:
                break;
        }
        return(ERR_SUCCESS);
    }
    else
    {
        if(API_SIM_READBINARY_CNF == nEvtId)
        {
            UINT8 i = 0;
            UINT16 tmpMid;
            api_SimReadBinaryCnf_t* pSimReadBinaryCnf = (api_SimReadBinaryCnf_t*)nEvParam;
            CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf, Sw1: 0x%x; Sw2:  0x%x ; fileId: %d; file size: %d\n"),pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2,pSimReadBinaryCnf->File,cbCtx[nSimID].SimFileSize);

            switch(pSimReadBinaryCnf->File)
            {
                case API_USIM_EF_CBMI:
                case API_SIM_EF_CBMI:
                    tmpMid = pSimReadBinaryCnf->Data[0] << 8 | pSimReadBinaryCnf->Data[1];
                    CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf1111, tmpMid: 0x%x \n"), tmpMid);
                    while(((i * 2) < cbCtx[nSimID].SimFileSize) && ((tmpMid != 0xFFFF)&&(tmpMid != 0x0)))
                        //while((tmpMid != 0xFFFF)&&(tmpMid != 0x0000))
                    {
                        if(((tmpMid>0x3EC)&&(tmpMid<0xFFF))||((tmpMid>0x1000)&&(tmpMid<0x107F))||((tmpMid>0x1100)&&(tmpMid<0x9FFF)))
                        {
                            CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf1111, tmpMid: 0x%x \n"), tmpMid);
                        }
                        else
                            cbCtx[nSimID].Mid[i] = tmpMid;
                        CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf, tmpMid: 0x%x \n"), tmpMid);
                        i++;
                        if((i * 2) >= cbCtx[nSimID].SimFileSize || i >= CFW_CB_MAX_CH)
                            break;
                        tmpMid = pSimReadBinaryCnf->Data[i * 2] << 8 | pSimReadBinaryCnf->Data[i * 2 + 1];
                    }
                    //cbCtx[nSimID].MidNb = i;
                    if(cbCtx[nSimID].MidNb != 0)
                        cbCtx[nSimID].MidNb += cbCtx[nSimID].SimFileSize/2;
                    else
                        cbCtx[nSimID].MidNb = cbCtx[nSimID].SimFileSize/2;
                    CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf,API_SIM_EF_CBMI cbCtx[nSimID].MidNb: %d \n"), cbCtx[nSimID].MidNb);
                    break;
                case API_SIM_EF_CBMID:
                case API_USIM_EF_CBMID:
                    tmpMid = pSimReadBinaryCnf->Data[0] << 8 | pSimReadBinaryCnf->Data[1];
                    //while(tmpMid != 0xFFFF)
                    while(((i * 2) < cbCtx[nSimID].SimFileSize)&&(tmpMid != 0xFFFF))
                    {
                        if(((tmpMid>0x3EC)&&(tmpMid<0xFFF))||((tmpMid>0x1000)&&(tmpMid<0x107F))||((tmpMid>0x1100)&&(tmpMid<0x9FFF)))
                        {
                            CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf1111, tmpMid: 0x%x \n"), tmpMid);
                        }//need not post set to cbctx.
                        else
                            cbCtx[nSimID].Mid[i] = tmpMid;
                        i++;
                        if((i * 2) >= cbCtx[nSimID].SimFileSize
                                || i >= CFW_CB_MAX_CH)
                        {
                            break;
                        }
                        tmpMid = pSimReadBinaryCnf->Data[i * 2] << 8 | pSimReadBinaryCnf->Data[i * 2 + 1];
                    }
                    if(cbCtx[nSimID].MidNb != 0)
                        cbCtx[nSimID].MidNb += cbCtx[nSimID].SimFileSize/2;
                    else
                        cbCtx[nSimID].MidNb = cbCtx[nSimID].SimFileSize/2;

                    CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf,API_SIM_EF_CBMID cbCtx[nSimID].MidNb: %d \n"), cbCtx[nSimID].MidNb);
                    break;
                case API_SIM_EF_CBMIR:
                case API_USIM_EF_CBMIR:
                    tmpMid = pSimReadBinaryCnf->Data[0] << 8 | pSimReadBinaryCnf->Data[1];
                    //while(tmpMid != 0xFFFF)
                    while(((i * 2) < cbCtx[nSimID].SimFileSize)/* && (tmpMid != 0xFFFF)*/)
                    {
                        cbCtx[nSimID].Mir[i] = tmpMid;
                        i++;
                        if((i * 2) >= cbCtx[nSimID].SimFileSize || i >= CFW_CB_MAX_CH)
                        {
                            break;
                        }
                        tmpMid = pSimReadBinaryCnf->Data[i * 2] << 8 | pSimReadBinaryCnf->Data[i * 2 + 1];
                    }
                    //if(cbCtx[nSimID].MidNb != 0)
                    //cbCtx[nSimID].MidNb += cbCtx[nSimID].SimFileSize/2;
                    //else
                    //cbCtx[nSimID].MidNb = cbCtx[nSimID].SimFileSize/2;
                    //cbCtx[nSimID].MirNb = i;
                    break;
                case API_SIM_EF_LP:
                    while((pSimReadBinaryCnf->Data[i] != 0xFF)&&( i < CFW_CB_MAX_DCS))
                    {
                        if(cbCtx[nSimID].Dcs[i] == 0xFF)
                        {
                            cbCtx[nSimID].Dcs[i] = pSimReadBinaryCnf->Data[i];
                            i++;
                        }
                        else
                        {
                            i++;
                            continue;
                        }
                        if(i >= cbCtx[nSimID].SimFileSize)
                        {
                            break;
                        }
                    }
                    CSW_TRACE(CFW_SMS_TS_ID, TSTXT("pSimReadBinaryCnf,API_SIM_EF_CBMID cbCtx[nSimID].DcsNb: %d \n"), cbCtx[nSimID].DcsNb);
                    cbCtx[nSimID].DcsNb = i;
                    break;
                case API_SIM_EF_ELP:
                    while((pSimReadBinaryCnf->Data[i] != 0xFF)&&( i < CFW_CB_MAX_DCS))
                    {
                        if(cbCtx[nSimID].Dcs[i] == 0xFF)
                        {
                            cbCtx[nSimID].Dcs[i] = pSimReadBinaryCnf->Data[i];
                            i++;
                        }
                        else
                        {
                            i++;
                            continue;
                        }
                        if(i >= cbCtx[nSimID].SimFileSize )
                        {
                            break;
                        }
                    }
                    cbCtx[nSimID].DcsNb = i;
                    CFW_PostNotifyEvent(EV_CFW_CB_READ_SIM_RSP, (UINT32)NULL, 0,  nUTI | (nSimID << 24), 0);           // comsumed the message
//                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
//                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    break;
                default:
                    break;
            }
      	    CFW_CbReadFile(nSimID);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else if(API_SIM_ELEMFILESTATUS_CNF == nEvtId)
        {
            api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("pSimElemFileStatusCnf :\n",0x08100f6a)));
            //SXS_DUMP(CFW_SIM_TS_ID_DUMP, 0, (UINT8*)pSimElemFileStatusCnf, 260);
            SXS_DUMP(CFW_SIM_TS_ID_DUMP, 0, (UINT8 *)pSimElemFileStatusCnf, sizeof(api_SimElemFileStatusCnf_t));
            //COS_Sleep(10);
            if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
            {
                CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                cbCtx[nSimID].SimFileSize = gUsimEfStatus[nSimID].fileSize;
            }
            else
                cbCtx[nSimID].SimFileSize = ((UINT16)pSimElemFileStatusCnf->Data[2] << 8) | (pSimElemFileStatusCnf->Data[3]);
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(2), TSTR("pSimElemFileStatusCnf :file size:%d File = %d\n",0x08100f6b), cbCtx[nSimID].SimFileSize, pSimElemFileStatusCnf->File);
            if(cbCtx[nSimID].SimFileSize > 0)
            {
                SimReadBinaryReq(pCbSimReadReq->FileId, 0, cbCtx[nSimID].SimFileSize, nSimID);
            }
            else
            {
                if(pSimElemFileStatusCnf->File == API_SIM_EF_ELP)
                {
                    CFW_PostNotifyEvent(EV_CFW_CB_READ_SIM_RSP, (UINT32)NULL, 0,  nUTI | (nSimID << 24), 0);            // comsumed the message
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("pSimElemFileStatusCnf :file size <=0\n",0x08100f6c));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
            //COS_Sleep(10);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("no process, pSimReadBinaryCnf, nEvtId: %x\n",0x08100f6d)), nEvtId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        //COS_Sleep(100);
        return(ERR_SUCCESS);
    }
}

UINT32 CFW_CbAoProc (HAO hAo, CFW_EV *pEvent)
{
    HAO CFW_CbAo = hAo;
    CFW_CB_ACTIVE_REQ *pCbActReq = NULL;
    UINT32 nUTI   = 0;
    UINT32 nEvtId = 0;;
    VOID *nEvParam;
    CFW_EV ev;
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTR("In CFW_CbAoProc, pEvent:0x%x \n",0x08100f6e), pEvent);
    if((UINT32)pEvent == 0xffffffff)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("In CFW_CbAoProc \n",0x08100f6f));
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
    }
    if( (hAo == 0) || (pEvent == NULL))
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    else
    {
        nEvParam = (VOID *)pEvent->nParam1;
        pCbActReq = CFW_GetAoUserData(CFW_CbAo);
        nEvtId = pEvent->nEventId;
    }
    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(CFW_CbAo);
    CFW_GetUTI(CFW_CbAo, &nUTI);
    if(nEvParam == (VOID *)0xff)
    {
        CSW_TRACE(_CSW|CFW_CB_TS_ID, "CFW_CbAoProc cbCtx[%d].DcsNb = %d  cbCtx[].MidNb = %d   \n",nSimID, cbCtx[nSimID].DcsNb,cbCtx[nSimID].MidNb);
        //if(pCbActReq->MsgId == API_SMSCB_ACTIVATE_REQ && cbCtx[nSimID].MidNb > 0 && cbCtx[nSimID].DcsNb > 0)
        if(pCbActReq->MsgId == API_SMSCB_ACTIVATE_REQ)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_CbAoProc, API_SMSCB_ACTIVATE_REQ nSimID %d CbType %d\n",0x08100f70)), nSimID, pCbActReq->CbType);
            api_SmsCBActivateReq_t *pOMsg = (api_SmsCBActivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SmsCBActivateReq_t));
            SUL_MemSet8((UINT8 *)pOMsg, 0x00, SIZEOF(api_SmsCBActivateReq_t));
            pOMsg->ItfType = pCbActReq->CbType;
            cbCtx[nSimID].CbType = pCbActReq->CbType;
            pOMsg->MIDNb = cbCtx[nSimID].MidNb;
            SUL_MemCopy8(pOMsg->MID, cbCtx[nSimID].Mid, pOMsg->MIDNb * 2);
            pOMsg->DCSNb = cbCtx[nSimID].DcsNb;
            SUL_MemCopy8(pOMsg->DCS, cbCtx[nSimID].Dcs, pOMsg->DCSNb == 0 ? API_SMSCB_DCS_LEN : pOMsg->DCSNb);
            int i;
            for(i = 0; i < API_SMSCB_DCS_LEN; i++)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_CbAoProc, API_SMSCB_ACTIVATE_REQ API_SMSCB_DCS_LEN :0x%x \n",0x08100f71)), pOMsg->DCS[i]&0xff);
            }
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_CbAoProc, API_SMSCB_ACTIVATE_REQ pOMsg->DCSNb:%d\n",0x08100f72)),pOMsg->DCSNb);
            CFW_SendSclMessage(API_SMSCB_ACTIVATE_REQ, pOMsg, nSimID);
            cbCtx[nSimID].CbActiveStatus = CB_ACTIVED;
            CFW_UnRegisterAO(CFW_SMS_CB_ID, hAo);
        }
        else if(pCbActReq->MsgId == API_SMSCB_DEACTIVATE_REQ)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CbAoProc, API_SMSCB_DEACTIVATE_REQ \n",0x08100f73)));
            api_SmsCBDeActivateReq_t *pOMsg = (api_SmsCBDeActivateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SmsCBDeActivateReq_t));
            SUL_MemSet8((UINT8 *)pOMsg, 0x00, SIZEOF(api_SmsCBDeActivateReq_t));
            CFW_SendSclMessage(API_SMSCB_DEACTIVATE_REQ, pOMsg, nSimID);
            cbCtx[nSimID].CbActiveStatus = CB_DEACTIVED;
            CFW_UnRegisterAO(CFW_SMS_CB_ID, hAo);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID), TSTXT("CFW_CbAoProc, ignore \n"));
            cbCtx[nSimID].CbActiveStatus = CB_DEACTIVED;
            CFW_UnRegisterAO(CFW_SMS_CB_ID, hAo);
        }
    }
    else
    {
        if(API_SMSCB_RECEIVE_IND == nEvtId)
        {
            api_SmsCBReceiveInd_t *psInMsg  = (api_SmsCBReceiveInd_t *)nEvParam;
            //ASSERT(0);
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(3), TSTXT(TSTR("receive API_SMSCB_RECEIVE_IND, Mid:0x%x, DCS: 0x%x, DataLen:%d\n",0x08100f74)), psInMsg->MId, psInMsg->DCS, psInMsg->DataLen);
            int i;
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR(" CFW_CbAoProc Start printf data!!!\n",0x08100f75)));
            for(i = 0; i < psInMsg->DataLen; i++)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("psInMsg->Data[%d]:0x%x \n",0x08100f76)), i, psInMsg->Data[i]);
            }
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR(" CFW_CbAoProc END printf data!!!\n",0x08100f77)));
            SXS_DUMP(CFW_CB_TS_ID, 0, psInMsg->Data, psInMsg->DataLen);
            CB_MSG_IND_STRUCT *pData = NULL;
            UINT16 nDataLen = 0;
            nDataLen = (UINT16)(psInMsg->DataLen);
            //pData = (CFW_GPRS_DATA *)CFW_MALLOC(nDataLen + OFFSETOF(CFW_GPRS_DATA, pData));
            pData = (CB_MSG_IND_STRUCT *)CSW_SMS_MALLOC(SIZEOF(CB_MSG_IND_STRUCT) + nDataLen + 2);
            if(pData == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x08100f78)));
                //COS_Sleep(20);//remove by wuys for gprs test 2008-9-03
                return ERR_BUSY_DEVICE;
            }
            SUL_ZeroMemory8(pData, (SIZEOF(CB_MSG_IND_STRUCT) + nDataLen + 2));
            pData->MId = psInMsg->MId;
            pData->DCS = psInMsg->DCS;
            pData->DataLen = nDataLen;
            SUL_MemCopy8(pData->Data, psInMsg->Data, psInMsg->DataLen);
            CFW_GetUTI(hAo, &nUTI);
            CFW_PostNotifyEvent(EV_CFW_CB_MSG_IND, (UINT32)pData, 0,  nUTI | (nSimID << 24), 0);// comsumed the message
            CFW_UnRegisterAO(CFW_SMS_CB_ID, hAo);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else if(API_SMSCB_PAGE_IND == nEvtId)
        {
            api_SmsCBPageInd_t *psInMsg  = (api_SmsCBPageInd_t *)nEvParam;
            //ASSERT(0);
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("receive API_SMSCB_PAGE_IND,datalen: %d\n",0x08100f79)), psInMsg->DataLen);
            SXS_DUMP(CFW_SIM_TS_ID_DUMP, 0, psInMsg->Data, psInMsg->DataLen);
            CB_PAGE_IND_STRUCT *pData = NULL;
            UINT16 nDataLen = 0;
            nDataLen = (UINT16)(psInMsg->DataLen);
            //pData = (CFW_GPRS_DATA *)CFW_MALLOC(nDataLen + OFFSETOF(CFW_GPRS_DATA, pData));
            pData = (CB_PAGE_IND_STRUCT *)CSW_SMS_MALLOC(nDataLen + 1);
            if(pData == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x08100f7a)));
                //COS_Sleep(20);//remove by wuys for gprs test 2008-9-03
                return ERR_BUSY_DEVICE;
            }
            pData->DataLen = nDataLen;
            SUL_MemCopy8(pData->Data, &(psInMsg->Data), psInMsg->DataLen);
            CFW_GetUTI(hAo, &nUTI);
            CFW_PostNotifyEvent(EV_CFW_CB_PAGE_IND, (UINT32)pData, 0,  nUTI | (nSimID << 24), 0);           // comsumed the message
            CFW_UnRegisterAO(CFW_SMS_CB_ID, hAo);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("no process, CFW_CbAoProc, nEvtId: %x\n",0x08100f7b)), nEvtId);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    return(ERR_SUCCESS);
}
HAO CB_Rec_Reg(CFW_EV *pEvent)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("MESSAGES_CB %s receive CB message. \n ",0x08100f7c),__FUNCTION__);

    UINT32 ret = 0;
    CFW_SIM_ID nSimId = CFW_SIM_END;
    nSimId = pEvent->nFlag;
    ret = CFW_CheckSimId( nSimId);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CB_Rec_Reg() ERROR!!!!, the incoming SMS CB with invalid SIM id\n",0x08100f7d));
        return ret;
    }
    if(cbCtx[nSimId].CbActiveStatus != CB_ACTIVED)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CB_Rec_Reg receive CB message but not actived \n ",0x08100f7e));
        return HNULL;
    }
    CFW_SMS_CB_INFO    *pSmsCBInfo = NULL; //Point to MT private data.
    pSmsCBInfo = (CFW_SMS_CB_INFO *)CSW_SMS_MALLOC(SIZEOF(CFW_SMS_CB_INFO));
    if(!pSmsCBInfo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("CB_Rec_Reg pSmsCBInfo malloc error!!! \n ",0x08100f7f));
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsCBInfo, SIZEOF(CFW_SMS_CB_INFO));

    pSmsCBInfo->hAo = CFW_RegisterAo(CFW_SMS_CB_ID, pSmsCBInfo, CFW_CbAoProc, nSimId);
    return pSmsCBInfo->hAo;
}

#ifdef CB_SUPPORT
UINT32 CFW_CbGetContext(VOID *CtxData , CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CbGetContext);
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CbGetContext);
        return ret;
    }
    memcpy(CtxData, &cbCtx[nSimID], sizeof(CB_CTX_T));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CbGetContext);
    return ret;
}
UINT32 CFW_CbSetContext(VOID *CtxData , CFW_SIM_ID nSimID)
{
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CbSetContext);
        return ret;
    }
    memcpy(&cbCtx[nSimID], CtxData, sizeof(CB_CTX_T));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CbSetContext);
}
UINT32 CFW_SimSetMidList(UINT16 *pMidList, UINT8 nCnt, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = HNULL;
    UINT8 i = 0;
    UINT16 tmpMid;

    CFW_SMS_CB_MID_INFO *pCFWMidList = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetMidList);
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100f80), ret);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMidList);
        return ret;
    }
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100f81));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMidList);
        return ERR_CFW_UTI_IS_BUSY;
    }
    pCFWMidList = (CFW_SMS_CB_MID_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SMS_CB_MID_INFO));
    if (pCFWMidList == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CB_TS_ID)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100f82));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMidList);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }
    SUL_ZeroMemory32(pCFWMidList, SIZEOF(CFW_SMS_CB_MID_INFO));
    pCFWMidList->nSize = nCnt * 2;
    SUL_MemSet8(pCFWMidList->nMidList, 0xFF, sizeof(pCFWMidList->nMidList));
    SUL_MemCopy8(pCFWMidList->nMidList, pMidList, pCFWMidList->nSize);

    while(i < nCnt && i < sizeof(pCFWMidList->nMidList) / sizeof(UINT16))
    {
        tmpMid = ((pMidList[i] & 0x00ff) << 8) | ((pMidList[i] & 0xff00) >> 8);

        pCFWMidList->nMidList[i] = tmpMid;

        i++;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pCFWMidList, CFW_SimSetMidListProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMidList);

    return (ERR_SUCCESS);
}
extern CFW_UsimEfStatus gUsimEfStatus[CFW_SIM_COUNT];
extern VOID CFW_UsimDecodeEFFcp (UINT8 *RespData, CFW_UsimEfStatus *pSimEfStatus);
UINT32 CFW_SimSetMidListProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI      = 0x00000000;
    UINT32 nEvtId    = 0x00000000;
    UINT32 ErrorCode = 0x00000000;
    UINT32 result    = 0x00000000;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    CFW_SMS_CB_MID_INFO *pCFWMidList = NULL;
    api_SimUpdateBinaryCnf_t *pSimUpdateBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetMidListProc);
    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    pCFWMidList = CFW_GetAoUserData(hAo);
    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                         = 0xff;
        pEvent                             = &ev;
        pCFWMidList->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pCFWMidList->nState.nNextState)
    {
        case CFW_SIM_MIS_IDLE:
            if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
            {
                result = SimElemFileStatusReq(API_USIM_EF_CBMI, nSimID);
            }
            else
                result = SimElemFileStatusReq(API_SIM_EF_CBMI, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100f83), result);

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
                CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }

            pCFWMidList->nState.nCurrState = CFW_SIM_MIS_IDLE;
            pCFWMidList->nState.nNextState = CFW_SIM_MIS_READELEMENT;
            break;

        case CFW_SIM_MIS_READELEMENT:
            pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
            CFW_GetUTI(hAo, &nUTI);

            if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2))
                    || (0x91 == pSimElemFileStatusCnf->Sw1))

            {
                if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
                {
                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                    pCFWMidList->nSize =  gUsimEfStatus[nSimID].fileSize;
                    cbCtx[nSimID].SimFileSize = gUsimEfStatus[nSimID].fileSize;
                    result = SimUpdateBinaryReq(API_USIM_EF_CBMI, 0, pCFWMidList->nSize, pCFWMidList->nMidList, nSimID);
                }
                else
                {
                    pCFWMidList->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("MID Size 0x%x \n",0x08100f84), pCFWMidList->nSize);


                    result = SimUpdateBinaryReq(API_SIM_EF_CBMI, 0, pCFWMidList->nSize, pCFWMidList->nMidList, nSimID);
                }
                int i=0;
                for(i=0; i<19; i++)
                    hal_HstSendEvent(pCFWMidList->nMidList[i]);

                CSW_TC_MEMBLOCK(CFW_CC_TS_ID, pCFWMidList->nMidList, pCFWMidList->nSize, 16);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100f85), result);

                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    hAo = HNULL;
                    CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                pCFWMidList->nState.nCurrState = CFW_SIM_MIS_IDLE;
                pCFWMidList->nState.nNextState = CFW_SIM_MIS_UPDATEBINARY;
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
            }

            break;

        case CFW_SIM_MIS_UPDATEBINARY:
            pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)nEvParam;
            CFW_GetUTI(hAo, &nUTI);
            if (((0x90 == pSimUpdateBinaryCnf->Sw1) && (0x00 == pSimUpdateBinaryCnf->Sw2))
                    || (0x91 == pSimUpdateBinaryCnf->Sw1))
            {
                if(g_cfw_sim_status[nSimID].UsimFlag == TRUE)
                    result = SimReadBinaryReq(API_USIM_EF_CBMI, 0, pCFWMidList->nSize, nSimID);
                else
                    result = SimReadBinaryReq(API_SIM_EF_CBMI, 0, pCFWMidList->nSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100f86),result);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    hAo = HNULL;
                    CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                }
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
            }

            pCFWMidList->nState.nCurrState = CFW_SIM_MIS_UPDATEBINARY;
            pCFWMidList->nState.nNextState = CFW_SIM_MIS_READBINARY;
            break;

        case CFW_SIM_MIS_READBINARY:
            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;;
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
                    || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Save Data Size 0x%x \n",0x08100f87), pCFWMidList->nSize);
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pCFWMidList->nMidList, pCFWMidList->nSize, 16);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Sim Data Size 0x%x \n",0x08100f88), pCFWMidList->nSize);
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadBinaryCnf->Data, pCFWMidList->nSize, 16);

                UINT32 nCompResult = 0x00;
                nCompResult = SUL_MemCompare(pSimReadBinaryCnf->Data, pCFWMidList->nMidList, pCFWMidList->nSize);
                if( 0x00 == nCompResult )
                {
                    CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0x00);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP Success\n",0x08100f89)));

                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP Failed Write failed\n",0x08100f8a)));
                }

            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_CB_SET_MID_SIM_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP ErrorCode %x\n",0x08100f8b)),ErrorCode);
            }

            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;

        default:
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetMidListProc);
    return ERR_SUCCESS;
}

#endif


