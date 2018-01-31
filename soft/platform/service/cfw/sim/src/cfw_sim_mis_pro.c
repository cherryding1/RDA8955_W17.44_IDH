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

#include <cswtype.h>
#include <errorcode.h>
// #include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include <base_prv.h>

#include "cfw_sim.h"
#include "cfw_sim_boot.h"
#include "cfw_sim_mis.h"
#include "cfw_usim.h"

#define CFW_CHV_ET /*CHV Early treatment*/
extern CFW_SIM_OC_STATUS SIMOCStatus[];
extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT];
BOOL g_GETIMSI[CFW_SIM_COUNT] = {FALSE, FALSE};

extern BOOL hal_HstSendEvent(UINT32 ch);

UINT32 CFW_SimWritePreferPLMNProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 result = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimWritePreferPLMNProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    struct write_prefer_plmn* plmn = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> next status = %d", plmn->status);

    switch(plmn->status)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            plmn->EF = API_SIM_EF_PLMNSEL;
        else // USIM
            plmn->EF = API_USIM_EF_PLMNWACT;

        result = SimElemFileStatusReq(plmn->EF, nSimID);
        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c56), result);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
            plmn->status = CFW_SIM_MIS_READELEMENT;
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_IDLE");
        break;
    }
    case CFW_SIM_MIS_READELEMENT:
    {
        api_SimElemFileStatusCnf_t* pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)pEvent->nParam1;
        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "plmn->index = %d", plmn->index);
            if(plmn->index != 0)    //update plmn
            {
                UINT16 offset = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                {
                    offset = (plmn->index - 1) * 3;    //the index value of PLMN in (U)SIM is from 0.
                    plmn->file_size = (pSimElemFileStatusCnf->Data[2] << 8) | pSimElemFileStatusCnf->Data[3];
                    if(offset + 3 > plmn->file_size)
                    {
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, ERR_INVALID_INDEX, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The index is larger than the maximum index in the USIM(offset = %d, file size = %d)", offset, plmn->file_size);
                    }
                }
                else // USIM
                {
                    CFW_UsimEfStatus SimEfStatus;
                    CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                    plmn->file_size = SimEfStatus.fileSize;
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "plmn->file_size = %d", plmn->file_size);
                    offset = (plmn->index - 1) * 5;    //the index value of PLMN in (U)SIM is from 0.
                    if(offset + 5 > plmn->file_size)
                    {
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, ERR_INVALID_INDEX, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The index is larger than the maximum index in the USIM(offset = %d, file size = %d)", offset, SimEfStatus.fileSize);
                    }
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "offset = %d, plmn = %x%x%x", offset, plmn->PLMN[0], plmn->PLMN[1], plmn->PLMN[2]);

                result = SimUpdateBinaryReq(plmn->EF, offset, 3, plmn->PLMN, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimUpdateBinaryReq return(0x%x)", result);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                else
                    plmn->status = CFW_SIM_MIS_UPDATEBINARY;
            }
            else    //try to find a free location
            {
                if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)  //SIM
                {
                    plmn->file_size = (pSimElemFileStatusCnf->Data[2] << 8) | pSimElemFileStatusCnf->Data[3];
                    plmn->size = plmn->file_size;
                }
                else if(g_cfw_sim_status[nSimID].UsimFlag == TRUE) // USIM
                {
                    CFW_UsimEfStatus SimEfStatus;
                    CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                    plmn->file_size = SimEfStatus.fileSize;
                    plmn->size = plmn->file_size;
                    if(plmn->file_size > 255)
                    {
                        plmn->size = 255;
                    }
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "plmn->size = %d", plmn->size);

                result = SimReadBinaryReq(plmn->EF, 0, plmn->size, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimReadBinaryReq return(0x%x)", result);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                }
                else
                    plmn->status = CFW_SIM_MIS_READBINARY;
            }
        }
        else
        {
            result = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, result, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_READELEMENT");
        break;
    }
    case CFW_SIM_MIS_UPDATEBINARY:
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> recevied CFW_SIM_MIS_UPDATEBINARY");
        api_SimUpdateBinaryCnf_t* pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)pEvent->nParam1;
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "(sw1=%x,sw2=%x)", pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2);
        if (((0x90 == pSimUpdateBinaryCnf->Sw1) && (0x00 == pSimUpdateBinaryCnf->Sw2)) || (0x91 == pSimUpdateBinaryCnf->Sw1))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> 0");
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, 0, 0, nUTI | (nSimID << 24), 0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> 1");
        }
        else
        {
            result = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> 2");
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_UPDATEBINARY");
        break;
    }
    case CFW_SIM_MIS_READBINARY:
    {
        api_SimReadBinaryCnf_t* pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)pEvent->nParam1;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadBinaryCnf->Data, plmn->file_size, 16);

            if(g_cfw_sim_status[nSimID].UsimFlag == TRUE) // USIM
                plmn->step = 5;
            else
                plmn->step = 3;
            INT16 index = 0;
            for(index = 0; index < plmn->size; index += plmn->step)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "index = %d, %x, %x, %x", index, pSimReadBinaryCnf->Data[index],
                           pSimReadBinaryCnf->Data[index + 1], pSimReadBinaryCnf->Data[index + 2]);
                if((pSimReadBinaryCnf->Data[index] == 0xFF) &&
                   (pSimReadBinaryCnf->Data[index + 1] == 0xFF) &&
                   (pSimReadBinaryCnf->Data[index + 2] == 0xFF))
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "found an index = %d", plmn->index + index);
                    result = SimUpdateBinaryReq(plmn->EF, plmn->index + index, 3, plmn->PLMN, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimUpdateBinaryReq return(0x%x)", result);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    }
                    else
                        plmn->status = CFW_SIM_MIS_UPDATEBINARY;
                    return ERR_SUCCESS;
                }
            }
            if(g_cfw_sim_status[nSimID].UsimFlag == TRUE) // USIM, try to find another part of file whose size is larger than 255
            {
                if((plmn->file_size > 255) && (plmn->size == 255))
                {
                    int offset = plmn->size;
                    plmn->size = plmn->file_size - offset;
                    plmn->index = 255;
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "read data2: offset = %d, plmn->size = %d", offset, plmn->size);
                    result = SimReadBinaryReq(plmn->EF,offset , plmn->size, nSimID);
                    if(ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimReadBinaryReq return(0x%x)", result);
                    }
                    else
                    {
                        return ERR_SUCCESS;
                    }

                }
                else
                {
                    result = ERR_NO_MORE_MEMORY;
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "There is no free location to save PLMN");
                }
            }
            else
            {
                result = ERR_NO_MORE_MEMORY;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "There is no free location to save PLMN");
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        else
        {
            result = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_READBINARY");
        break;
    }
    default:
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> default case, unexpected!");
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMNProc);
    return ERR_SUCCESS;
}


UINT32 CFW_SimReadPreferPLMNProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 result = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadPreferPLMNProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    struct read_prefer_plmn* plmn = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> next status = %d", plmn->status);

    switch(plmn->status)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            plmn->EF = API_SIM_EF_PLMNSEL;
        else                                            // USIM
            plmn->EF = API_USIM_EF_PLMNWACT;

        result = SimElemFileStatusReq(plmn->EF, nSimID);
        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c56), result);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
            plmn->status = CFW_SIM_MIS_READELEMENT;
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_IDLE");
        break;
    }
    case CFW_SIM_MIS_READELEMENT:
    {
        api_SimElemFileStatusCnf_t* pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)pEvent->nParam1;
        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                plmn->file_size = (pSimElemFileStatusCnf->Data[2] << 8) | pSimElemFileStatusCnf->Data[3];
                plmn->size = plmn->file_size;
                plmn->offset = 0;
            }
            else                                            // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                plmn->file_size = SimEfStatus.fileSize;
                plmn->size = plmn->file_size;
                plmn->offset = 0;
                if(plmn->file_size > 255)
                {
                    plmn->size = 255;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "plmn->file_size = %d, plmn->size = %d", plmn->file_size, plmn->size);
            }

            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "read data1: plmn->file_size = %d", plmn->file_size);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "read data1: plmn->offset = %d, plmn->size = %d", plmn->offset, plmn->size);
            result = SimReadBinaryReq(plmn->EF, plmn->offset, plmn->size, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimReadBinaryReq return(0x%x)", result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
            else
                plmn->status = CFW_SIM_MIS_READBINARY;
        }
        else
        {
            result = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, result, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_READELEMENT");
        break;
    }

    case CFW_SIM_MIS_READBINARY:
    {
        api_SimReadBinaryCnf_t* pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)pEvent->nParam1;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadBinaryCnf->Data, plmn->file_size, 16);
            if(plmn->ret_data == 0)
            {
                if(g_cfw_sim_status[nSimID].UsimFlag == TRUE) // USIM
                    plmn->step = 5;
                else
                    plmn->step = 3;

                plmn->ret_data = (UINT8*)CSW_SIM_MALLOC((plmn->file_size / plmn->step) * 3);
                if(plmn->ret_data == NULL)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimReadBinaryReq return(0x%x)", result);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "data size = %d", (plmn->file_size / plmn->step) * 3);
            }
#if 1
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "copy data: plmn->offset = %d, plmn->size = %d", plmn->offset, plmn->size);
            if(g_cfw_sim_status[nSimID].UsimFlag == false)
            {
                plmn->offset = plmn->size;
                memcpy(plmn->ret_data, pSimReadBinaryCnf->Data, plmn->size);
            }
            else
            {
                for(UINT16 i = 0; i < plmn->size; i += 5)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "copy data: i = %d, plmn->offset = %d", i, plmn->offset);
                    plmn->ret_data[plmn->offset++] = pSimReadBinaryCnf->Data[i];
                    plmn->ret_data[plmn->offset++] = pSimReadBinaryCnf->Data[i + 1];
                    plmn->ret_data[plmn->offset++] = pSimReadBinaryCnf->Data[i + 2];
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "last data size: plmn->offset = %d", plmn->offset);
            }
#else

            INT16 index = 0;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "plmn->index = %d, plmn->size = %d, index = %d", plmn->index, plmn->size, index);
            for(index = 0; index < plmn->size; index += plmn->step)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "plmn->index = %d, %x, %x, %x, index = %d", plmn->index, pSimReadBinaryCnf->Data[index],
                           pSimReadBinaryCnf->Data[index + 1], pSimReadBinaryCnf->Data[index + 2], index);
                if((pSimReadBinaryCnf->Data[index] != 0xFF) &&
                   (pSimReadBinaryCnf->Data[index + 1] != 0xFF) &&
                   (pSimReadBinaryCnf->Data[index + 2] != 0xFF))
                {
                    int i = plmn->index;

                    plmn->ret_data[i] = (pSimReadBinaryCnf->Data[index] & 0x0F) + '0';
                    plmn->ret_data[i + 1] = (pSimReadBinaryCnf->Data[index] >> 4) + '0';
                    plmn->ret_data[i + 2] = (pSimReadBinaryCnf->Data[index + 1] & 0x0F) + '0';
                    plmn->ret_data[i + 5] = (pSimReadBinaryCnf->Data[index + 1] >> 4) + '0';
                    plmn->ret_data[i + 3] = (pSimReadBinaryCnf->Data[index + 2] & 0xF) + '0';
                    plmn->ret_data[i + 4] = (pSimReadBinaryCnf->Data[index + 2] >> 4) + '0';


                    plmn->index += 6;
                }
            }
#endif

            if(g_cfw_sim_status[nSimID].UsimFlag == TRUE) // USIM, try to search part of file size larger than 255
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "plmn->offset = %d, plmn->file_size = %d", plmn->offset, plmn->file_size);
                if((plmn->file_size > 255) && (plmn->size == 255))
                {
                    plmn->size = plmn->file_size - 255;
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "read data2: plmn->offset = %d, plmn->size = %d", plmn->offset, plmn->size);
                    result = SimReadBinaryReq(plmn->EF, 255, plmn->size, nSimID);
                    if(ERR_SUCCESS != result)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID), "Error SimReadBinaryReq return(0x%x)", result);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    return ERR_SUCCESS;
                }
            }
            //plmn->index -= 6;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "plmn->offset =%d", plmn->offset);
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, plmn->ret_data, plmn->offset, 16);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, plmn->ret_data, plmn->offset, nUTI | (nSimID << 24), 0x0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
        {
            result = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> CFW_SIM_MIS_READBINARY");
        break;
    }
    default:
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "Prefer PLMN -> default case, unexpected!");
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadPreferPLMNProc);
    return ERR_SUCCESS;
}


UINT32 CFW_SimMisSetPrefListProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;
    CFW_EV ev;

    SIM_SM_INFO_SETPREFOPREATORLIST *pMisSetPrefList = NULL;
    api_SimUpdateBinaryCnf_t *pSimUpdateBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisSetPrefListProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisSetPrefList = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pMisSetPrefList->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            result = SimElemFileStatusReq(API_SIM_EF_PLMNSEL, nSimID);
        else // USIM
            result = SimElemFileStatusReq(API_USIM_EF_PLMNWACT, nSimID);

        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c56), result);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                pMisSetPrefList->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                result = SimUpdateBinaryReq(API_SIM_EF_PLMNSEL, 0, pMisSetPrefList->nSize, pMisSetPrefList->nOperatorList, nSimID);
            }
            else // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                pMisSetPrefList->nSize = SimEfStatus.fileSize;
                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisSetPrefList->nSize, nSimID);
            }

            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Prelist Size 0x%x \n", 0x08100c57), pMisSetPrefList->nSize);
            //CSW_TC_MEMBLOCK(CFW_SIM_TS_ID,pMisSetPrefList->nOperatorList,pMisSetPrefList->nSize,16);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c58), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
            pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_IDLE;
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_UPDATEBINARY;
            else
                pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_PREREADBINARY;
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

    case CFW_SIM_MIS_UPDATEBINARY:
        pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)nEvParam;
        if (((0x90 == pSimUpdateBinaryCnf->Sw1) && (0x00 == pSimUpdateBinaryCnf->Sw2)) || (0x91 == pSimUpdateBinaryCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                result = SimReadBinaryReq(API_SIM_EF_PLMNSEL, 0, pMisSetPrefList->nSize, nSimID);
            else
                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisSetPrefList->nSize, nSimID);

            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100c59), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_UPDATEBINARY;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_READBINARY;
        break;

    case CFW_SIM_MIS_READBINARY:
        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        ;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Save Data Size 0x%x \n", 0x08100c5a), pMisSetPrefList->nSize);
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pMisSetPrefList->nOperatorList, pMisSetPrefList->nSize, 16);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Sim Data Size 0x%x \n", 0x08100c5b), pMisSetPrefList->nSize);
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadBinaryCnf->Data, pMisSetPrefList->nSize, 16);

            UINT32 nCompResult = 0x00;

            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                nCompResult = SUL_MemCompare(pSimReadBinaryCnf->Data, pMisSetPrefList->nOperatorList, pMisSetPrefList->nSize);
            else
                nCompResult = SUL_MemCompare(pSimReadBinaryCnf->Data, pMisSetPrefList->nOperatorList, 3);

            if (0x00 == nCompResult)
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0x00);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP Success\n", 0x08100c5c)));
            }
            else
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP Failed Write failed\n", 0x08100c5d)));
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP ErrorCode %x\n", 0x08100c5e)), ErrorCode);
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    case CFW_SIM_MIS_PREREADBINARY: // only used by USIM
        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        CFW_GetUTI(hAo, &nUTI);
        UINT8 UsimPlmnwact[255] = {0};
        pMisSetPrefList->nSize = pMisSetPrefList->nSize / 5 * 5;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            SUL_MemCopy8(UsimPlmnwact, pSimReadBinaryCnf->Data, pMisSetPrefList->nSize);
            for (UINT8 i = 0; i < pMisSetPrefList->nSize / 5; i++)
            {
                SUL_MemCopy8(&(UsimPlmnwact[i * 5]), &(pMisSetPrefList->nOperatorList[i * 3]), 3);
            }

            result = SimUpdateBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisSetPrefList->nSize, UsimPlmnwact, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100c5f), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }

        pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_PREREADBINARY;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_UPDATEBINARY;
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetPrefListProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimMisGetPrefListProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;
    CFW_EV ev;

    SIM_SM_INFO_GETPREFOPREATORLIST *pMisGetPreflist = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisGetPrefListProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisGetPreflist = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pMisGetPreflist->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            result = SimElemFileStatusReq(API_SIM_EF_PLMNSEL, nSimID);
        else // USIM
            result = SimElemFileStatusReq(API_USIM_EF_PLMNWACT, nSimID);

        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c60), result);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        pMisGetPreflist->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READBINARY:
        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        UINT8 UsimPlmn[153] = {0};

        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            // ------------------------------------------------------------
            // Add by lixp for Plmn raise 2007-06-25 begin
            // ------------------------------------------------------------
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                pMisGetPreflist->nSize = pMisGetPreflist->nSize / 3 * 3;
            else // USIM
            {
                pMisGetPreflist->nSize = pMisGetPreflist->nSize / 5 * 3;
                for (UINT8 i = 0; i < pMisGetPreflist->nSize / 3; i++)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Plmn Num %d\n", 0x08100c61), i);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, &(pSimReadBinaryCnf->Data[i * 5]), 5, 16);
                    SUL_MemCopy8(&(UsimPlmn[i * 3]), &(pSimReadBinaryCnf->Data[i * 5]), 3);
                }
                SUL_MemCopy8(pSimReadBinaryCnf->Data, UsimPlmn, pMisGetPreflist->nSize);
            }
            // ------------------------------------------------------------
            // Add by lixp for Plmn raise 2007-06-25 endSS
            // ------------------------------------------------------------
            UINT8 *pPLMN = (UINT8 *)CSW_SIM_MALLOC(2 * (pMisGetPreflist->nSize));
            if (pPLMN == NULL)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c62));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
            }
            SUL_ZeroMemory8(pPLMN, (2 * (pMisGetPreflist->nSize)));
            cfw_PLMNtoBCD(pSimReadBinaryCnf->Data, pPLMN, &(pMisGetPreflist->nSize));
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pMisGetPreflist->nSize 0x%x \n", 0x08100c63), pMisGetPreflist->nSize);

            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadBinaryCnf->Data, pMisGetPreflist->nSize, 16);
            // CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pPLMN, 6*pMisGetPreflist->nSize, 16);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)pPLMN, pMisGetPreflist->nSize,
                                nUTI | (nSimID << 24), 0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1),
                      TSTXT(TSTR("EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP Success data nSize=%d\n", 0x08100c64)),
                      pMisGetPreflist->nSize);
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP ErrorCode 0x%x\n", 0x08100c65)), ErrorCode);
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                pMisGetPreflist->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                result = SimReadBinaryReq(API_SIM_EF_PLMNSEL, 0, pMisGetPreflist->nSize, nSimID);
            }
            else // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                pMisGetPreflist->nSize = SimEfStatus.fileSize;
                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisGetPreflist->nSize, nSimID);
            }
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100c66), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        pMisGetPreflist->nState.nCurrState = CFW_SIM_MIS_READELEMENT;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_READBINARY;
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProc);
    return ERR_SUCCESS;
}

// ------------------------------------------------------------
// Add by wulc  2009-10-25 begin
// ------------------------------------------------------------

#define CFW_SIM_SPN_LEN 16
UINT32 CFW_SimGetServiceProviderNameProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;
    CFW_EV ev;

    SIM_SM_INFO_GETSERVERPROVIDERNAME *pMisGetSPN = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetServiceProviderNameProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisGetSPN = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisGetSPN->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pMisGetSPN->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag)
            result = SimElemFileStatusReq(API_USIM_EF_SPN, nSimID);
        else
            result = SimElemFileStatusReq(API_SIM_EF_SPN, nSimID);

        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c67), result);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_SPN_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            return ERR_SUCCESS;
        }
        pMisGetSPN->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisGetSPN->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READBINARY:
        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            CFW_SPN *SPN = (CFW_SPN *)CSW_SIM_MALLOC(SIZEOF(CFW_SPN));
            SPN->flag = pSimReadBinaryCnf->Data[0];
            SUL_MemCopy8(SPN->spn, pSimReadBinaryCnf->Data + 1, CFW_SIM_SPN_LEN);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_SPN_RSP, (UINT32)SPN, pMisGetSPN->nSize, nUTI | (nSimID << 24), 0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("pSimReadBinaryCnf->Data Success data nSize=%d\n", 0x08100c68)), pSimReadBinaryCnf->Data);
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_SPN_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_GET_SPN_RSP ErrorCode 0x%x\n", 0x08100c69)), ErrorCode);
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag)
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                pMisGetSPN->nSize = SimEfStatus.fileSize;
            }
            else
                pMisGetSPN->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];

            if (g_cfw_sim_status[nSimID].UsimFlag)
                result = SimReadBinaryReq(API_USIM_EF_SPN, 0, pMisGetSPN->nSize, nSimID);
            else
                result = SimReadBinaryReq(API_SIM_EF_SPN, 0, pMisGetSPN->nSize, nSimID);

            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100c6a), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_SPN_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                return ERR_SUCCESS;
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_SPN_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            return ERR_SUCCESS;
        }
        pMisGetSPN->nState.nCurrState = CFW_SIM_MIS_READELEMENT;
        pMisGetSPN->nState.nNextState = CFW_SIM_MIS_READBINARY;
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    return ERR_SUCCESS;
}

// ------------------------------------------------------------
// Add by wulc  2009-10-25 end
// ------------------------------------------------------------
UINT32 CFW_SimMisGetPrefListMAXNumProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;
    CFW_EV ev;

    SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM *pMisGetPreflistMax = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisGetPrefListMAXNumProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisGetPreflistMax = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisGetPreflistMax->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pMisGetPreflistMax->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            result = SimElemFileStatusReq(API_SIM_EF_PLMNSEL, nSimID);
        else // USIM
            result = SimElemFileStatusReq(API_USIM_EF_PLMNWACT, nSimID);

        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c6b), result);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
        }
        pMisGetPreflistMax->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisGetPreflistMax->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            UINT16 Totalsize;
            UINT32 nNumber;
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                Totalsize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                nNumber = Totalsize / 3;
            }
            else // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                Totalsize = SimEfStatus.fileSize;
                nNumber = Totalsize / 5;
            }
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP, (UINT32)nNumber, 0, nUTI | (nSimID << 24), 0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP Success\n", 0x08100c6c)));
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListMAXNumProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimMisGetACMProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisGetACMProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    SIM_SM_INFO_GETACM *pGetACM = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pGetACM->nState.nCurrState =%d\n", 0x08100c6d), pGetACM->nState.nCurrState);

    switch (pGetACM->nState.nCurrState)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            UINT8 ef = 0;
            if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                ef = API_USIM_EF_ACM;
            else
                ef = API_SIM_EF_ACM;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Current EF = %d\n", 0x08100c6e), ef);

            result = SimReadRecordReq(ef, 0x01, 0x04, 0x03, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("ERROR: SimReadRecordReq return 0x%x \n", 0x08100c6f), result);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
                return result;
            }

            pGetACM->nState.nPreState = CFW_SIM_MIS_IDLE;
            pGetACM->nState.nCurrState = CFW_SIM_MIS_READRECORD;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post SimReadRecordReq Success! \n", 0x08100c70)));
        }
        else
        {
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, ERR_NOT_SUPPORT, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
            return ERR_NOT_SUPPORT;
        }
    }
    break;

    case CFW_SIM_MIS_READRECORD:
    {
        api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)pEvent->nParam1;
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR(" sw1=%x,sw2=%x,event = %x \n", 0x08100c71), pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, pEvent);
        if (pEvent->nEventId == API_SIM_READRECORD_CNF)
        {
            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)pEvent->nParam1;
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR("--pSimReadRecordCnf->Data= 0x%x%x%x\n", 0x08100c72),
                          pSimReadRecordCnf->Data[0], pSimReadRecordCnf->Data[1], pSimReadRecordCnf->Data[2]);
                UINT32 nACMData = (pSimReadRecordCnf->Data[2] << 16) + (pSimReadRecordCnf->Data[1] << 8) + (pSimReadRecordCnf->Data[0]);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, nACMData, 0, nUTI | (nSimID << 24), 0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
                return ErrorCode;
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
                return ErrorCode;
            }
        }
        else
        {
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, ERR_NOT_SUPPORT, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
            return ERR_NOT_SUPPORT;
        }
    }
    break;

    default:
    {
        CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACM_RSP, ERR_NOT_SUPPORT, 0, nUTI | (nSimID << 24), 0xF0);
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetACMProc);
        return ERR_NOT_SUPPORT;
        break;
    }
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimMisSetACMProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nErrCode = 0;
    UINT32 nUTI = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisSetACMProc);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    SIM_SM_INFO_SETACM *pMisSetACM = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    switch (pMisSetACM->nState.nCurrState)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == (UINT32)(-1))
        {
            SIM_CHV_PARAM *pSimCHV;
            CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("CFW_SimMisSetACMProc: pSimCHV->nPin2Status = %d\n", 0x08100c73)), pSimCHV->nPin2Status);
            if (pSimCHV->nPin2Status != CHV_VERIFY)
            {
                nErrCode = SimVerifyCHVReq(pMisSetACM->nPIN2, CHV2, nSimID);
                pSimCHV->bCHV2VerifyReq = TRUE;
                pMisSetACM->nState.nCurrState = CFW_SIM_MIS_VERIFY;
            }
            else // if(pSimCVH->nPin2Status == CHV_VERIFY)
            {
                UINT8 ef = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    ef = API_USIM_EF_ACM;
                else
                    ef = API_SIM_EF_ACM;
                UINT8 *pData = (UINT8 *)&pMisSetACM->iACMValue;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pMisSetACM->iACMValue = 0x%x \n", 0x08100c74), pMisSetACM->iACMValue);
                nErrCode = SimUpdateRecordReq(ef, 0x0, 0x03, 0x03, pData, nSimID);
                pMisSetACM->nState.nCurrState = CFW_SIM_MIS_UPDATERECORD;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("OK:CFW_SimMisSetACMProc SimUpdateRecordReq\n", 0x08100c75)));
            }
            if (ERR_SUCCESS != nErrCode)
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0XF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR: CFW_SimMisSetACMProc CFW_SIM_MIS_IDLE\n", 0x08100c76)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
                return nErrCode;
            }
            pMisSetACM->nState.nPreState = CFW_SIM_MIS_IDLE;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("OK: CFW_SimMisSetACMProc CFW_SIM_MIS_IDLE\n", 0x08100c77)));
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    case CFW_SIM_MIS_VERIFY:
    {
        if (pEvent->nEventId == API_SIM_VERIFYCHV_CNF)
        {
            api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("EV_CFW_SIM_SET_ACM_RSP SW1= 0x%x,SW2=0X%X\n", 0x08100c78)), pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);
            if (((0x90 == pSimVerifyCHVCnf->Sw1) && (0x00 == pSimVerifyCHVCnf->Sw2)) || (0x91 == pSimVerifyCHVCnf->Sw1))
            {
                UINT8 ef = 0;
                SIM_CHV_PARAM *pSimCHV;
                CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
                if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    pSimCHV->nPin2Remain = pSimCHV->nPin2Max;
                    pSimCHV->nPuk2Remain = pSimCHV->nPuk2Max;
                    ef = API_USIM_EF_ACM;
                }
                else
                    ef = API_SIM_EF_ACM;
                pSimCHV->nPin2Status = CHV_VERIFY;
                pSimCHV->bCHV2VerifyReq = TRUE;
                UINT8 *pData = (UINT8 *)&pMisSetACM->iACMValue;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pMisSetACM->iACMValue = 0x%x \n", 0x08100c79), pMisSetACM->iACMValue);
                nErrCode = SimUpdateRecordReq(ef, 0x0, 0x03, 0x03, pData, nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("ERROR: SimUpdateRecordReq return 0x%x \n", 0x08100c7a), nErrCode);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
                    return nErrCode;
                }

                pMisSetACM->nState.nPreState = CFW_SIM_MIS_VERIFY;
                pMisSetACM->nState.nCurrState = CFW_SIM_MIS_UPDATERECORD;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("OK: CFW_SimMisSetACMProc CFW_SIM_MIS_VERIFY\n", 0x08100c7b)));
            }
            else //if((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
            {
                SIM_CHV_PARAM *pG_Chv_Param;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, CHV2, nSimID);
                nErrCode = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR: EV_CFW_SIM_SET_ACM_RSP ErrorCode\n", 0x08100c7c)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
                return nErrCode;
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    case CFW_SIM_MIS_UPDATERECORD:
    {
        if (pEvent->nEventId == API_SIM_UPDATERECORD_CNF)
        {
            api_SimUpdateRecordCnf_t *pSimUpdateRecordCnf = (api_SimUpdateRecordCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR("CFW_SIM_MIS_UPDATERECORD(ACM): sw1=%x,sw2=%x,event = %x \n", 0x08100c7d), pSimUpdateRecordCnf->Sw1, pSimUpdateRecordCnf->Sw2, pEvent);
            if (((0x90 == pSimUpdateRecordCnf->Sw1) && (0x00 == pSimUpdateRecordCnf->Sw2)) || (0x91 == pSimUpdateRecordCnf->Sw1))
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, (UINT32)pMisSetACM->nMark, 0, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_SET_ACM_RSP Success! \n", 0x08100c7e)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
                return ERR_SUCCESS;
            }
            else
            {
                nErrCode = Sim_ParseSW1SW2(pSimUpdateRecordCnf->Sw1, pSimUpdateRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("EV_CFW_SIM_SET_ACM_RSP ErrorCode 0x%x \n", 0x08100c7f)), nErrCode);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
                return nErrCode;
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    default:
        CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACM_RSP, ERR_CME_SIM_WRONG, 0, nUTI | (nSimID << 24), 0xF0);
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_SET_ACM_RSP default return\n", 0x08100c80)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
        break;
    }
    // CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetACMProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimSetACMMaxProc(HAO hAo, CFW_EV *pEvent)
{

    UINT32 nErrCode = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetACMMaxProc);

    UINT32 nUTI;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    ACM_INFO_SETACMMAX *pSetACMMax = CFW_GetAoUserData(hAo);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("CFW_SimSetACMMaxProc: pSetACMMax->n_CurrStatus = %d\n", 0x08100c81)), pSetACMMax->n_CurrStatus);

    switch (pSetACMMax->n_CurrStatus)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if (pSetACMMax->nTryCount >= 1)
        {
            if ((UINT32)pEvent == (UINT32)(-1))
            {
                SIM_CHV_PARAM *pSimCHV;
                CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("CFW_SimSetACMMaxProc: pSimCHV->nPin2Status = %d\n", 0x08100c82)), pSimCHV->nPin2Status);
                if (pSimCHV->nPin2Status != CHV_VERIFY)
                {
                    pSimCHV->bCHV2VerifyReq = TRUE;
                    nErrCode = SimVerifyCHVReq(pSetACMMax->pPin2, CHV2, nSimID);
                    if (ERR_SUCCESS != nErrCode)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0XF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error SimElemFileStatusReq return", 0x08100c83));
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                        return nErrCode;
                    }
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("pSetACMMax->n_CurrStatus= CFW_SIM_MIS_VERIFY\n", 0x08100c84)));
                    pSetACMMax->n_PrevStatus = pSetACMMax->n_CurrStatus;
                    pSetACMMax->n_CurrStatus = CFW_SIM_MIS_VERIFY;
                }
                else // if(pSimCVH->nPin2Status == CHV_VERIFY)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--pSetACMMax->iACMMaxValue--= 0x%x\n", 0x08100c85), pSetACMMax->iACMMaxValue);
                    UINT8 ef = 0;
                    if (g_cfw_sim_status[nSimID].UsimFlag)
                        ef = API_USIM_EF_ACM_MAX;
                    else
                        ef = API_SIM_EF_ACMMAX;
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("---pSetACMMax->iACMMaxValue---= 0x%x,ef = %d\n", 0x08100c86), pSetACMMax->iACMMaxValue, ef);
                    UINT8 *pData = (UINT8 *)&pSetACMMax->iACMMaxValue;
                    nErrCode = SimUpdateBinaryReq(ef, 0, 3, pData, nSimID);
                    if (ERR_SUCCESS != nErrCode)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0XF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n", 0x08100c87), nErrCode);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                        return nErrCode;
                    }
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("pSimCVH->nPin2Status == CHV_VERIFY)\n", 0x08100c88)));
                    pSetACMMax->n_PrevStatus = pSetACMMax->n_CurrStatus;
                    pSetACMMax->n_CurrStatus = CFW_SIM_MIS_UPDATEBINARY;
                }
            }
            else
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        else
        {
            //Never reach ...
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)ERR_CFW_INVALID_PARAMETER, 0, nUTI | (nSimID << 24), 0XF0);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("ERROR: nTryCount = 1\n", 0x08100c89));
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
            return ERR_CFW_INVALID_PARAMETER;
        }
    }
    break;
    case CFW_SIM_MIS_VERIFY:
    {
        if (pEvent->nEventId == API_SIM_VERIFYCHV_CNF)
        {
            SIM_CHV_PARAM *pSimCHV;
            CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
            api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF, pSimVerifyCHVCnf->Sw1 = %x, \
pSimVerifyCHVCnf->Sw2 = %x",
                                                                                       0x08100c8a),
                      pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);
            if (((0x90 == pSimVerifyCHVCnf->Sw1) && (0x00 == pSimVerifyCHVCnf->Sw2)) || (0x91 == pSimVerifyCHVCnf->Sw1))
            {
                pSimCHV->bCHV2VerifyReq = FALSE;
                pSimCHV->nPin2Status = CHV_VERIFY;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                {
                    pSimCHV->nPin2Remain = pSimCHV->nPin2Max;
                    pSimCHV->nPuk2Remain = pSimCHV->nPuk2Max;
                }
                UINT8 ef = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                    ef = API_USIM_EF_ACM_MAX;
                else
                    ef = API_SIM_EF_ACMMAX;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("---pSetACMMax->iACMMaxValue---= 0x%x,ef = %d\n", 0x08100c8b), pSetACMMax->iACMMaxValue, ef);
                UINT8 *pData = (UINT8 *)&pSetACMMax->iACMMaxValue;
                nErrCode = SimUpdateBinaryReq(ef, 0, 3, pData, nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0XF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error SimElemFileStatusReq return\n", 0x08100c8c));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                    return nErrCode;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("pEvent->nEventId == API_SIM_VERIFYCHV_CNF", 0x08100c8d));
                pSetACMMax->n_PrevStatus = pSetACMMax->n_CurrStatus;
                pSetACMMax->n_CurrStatus = CFW_SIM_MIS_UPDATEBINARY;
            }
            else
            {
                SIM_CHV_PARAM *pG_Chv_Param;
                CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, CHV2, nSimID);
                nErrCode = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);

                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("ERROR: EV_CFW_SIM_SET_ACM_RSP nErrCode = %d\n", 0x08100c8e)), nErrCode);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                return nErrCode;
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;
    case CFW_SIM_MIS_UPDATEBINARY:
    {
        if (pEvent->nEventId == API_SIM_UPDATEBINARY_CNF)
        {
            api_SimUpdateBinaryCnf_t *pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)pEvent->nParam1;
            if ((0x90 == pSimUpdateBinaryCnf->Sw1) && (00 == pSimUpdateBinaryCnf->Sw2))
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_ACM_SET_ACMMAX_RSP Success! \n", 0x08100c8f)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                return ERR_SUCCESS;
            }
            else
            {
                nErrCode = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_ACMMAX_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR(" Error EV_CFW_ACM_SET_ACMMAX_RSP SW1 0x%x SW2 0x%x\n", 0x08100c90)), pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMaxProc);
                return nErrCode;
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimGetACMMaxProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 ErrorCode = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetACMMaxProc);
    UINT32 nUTI = 0;
    CFW_GetUTI(hAo, &nUTI);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    ACM_INFO_GETACMMAX *pGetACMMax = CFW_GetAoUserData(hAo);

    switch (pGetACMMax->n_CurrStatus)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pGetACMMax->nTryCount >= 0x01)
            {
                UINT8 ef = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                    ef = API_USIM_EF_ACM_MAX;
                else
                    ef = API_SIM_EF_ACMMAX;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("---CFW_SimGetACMMaxProc--- ef = %d\n", 0x08100c91), ef);
                ErrorCode = SimReadBinaryReq(ef, 0, 3, nSimID);
                if (ERR_SUCCESS != ErrorCode)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACMMAX_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0XF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100c92), ErrorCode);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMaxProc);
                    return ErrorCode;
                }

                CFW_SetAoState(hAo, CFW_SM_SIM_STATE_WRITE);
                pGetACMMax->n_PrevStatus = pGetACMMax->n_CurrStatus;
                pGetACMMax->n_CurrStatus = CFW_SIM_MIS_READBINARY;
                // pGetMR->nTryCount --;
            }
            else
            {
                //Never reach ...
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACMMAX_RSP, (UINT32)ERR_CFW_INVALID_PARAMETER, 0, nUTI | (nSimID << 24), 0XF0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("ERROR: nTryCount = 1\n", 0x08100c93));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMaxProc);
                return ERR_CFW_INVALID_PARAMETER;
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;

    case CFW_SIM_MIS_READBINARY:
    {
        if (pEvent->nEventId == API_SIM_READBINARY_CNF)
        {
            api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)pEvent->nParam1;
            if ((0x90 == pSimReadBinaryCnf->Sw1) && (00 == pSimReadBinaryCnf->Sw2))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR("pSimReadBinaryCnf->Data= 0x%x%x%x\n", 0x08100c94),
                          pSimReadBinaryCnf->Data[0], pSimReadBinaryCnf->Data[1], pSimReadBinaryCnf->Data[2]);
                UINT32 nACMMaxValue = (pSimReadBinaryCnf->Data[2] << 16) + (pSimReadBinaryCnf->Data[1] << 8) + (pSimReadBinaryCnf->Data[0]);

                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nACMMaxValue= 0x%x\n", 0x08100c95), nACMMaxValue);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACMMAX_RSP, nACMMaxValue, 0, nUTI | (nSimID << 24), 0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_ACM_GET_ACMMAX_RSP Success!\n", 0x08100c96)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMaxProc);
                return ERR_SUCCESS;
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ACMMAX_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_ACM_GET_ACMMAX_RSP failed!\n", 0x08100c97)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMaxProc);
                return ErrorCode;
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimGetPUCTProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nErrCode = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPUCTProc);
    HAO hSimGetPUCTAo = hAo;

    UINT32 nUTI = 0;
    CFW_GetUTI(hSimGetPUCTAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimGetPUCTAo);
    CFW_SIMGETPUCT *pSimGetPUCTData = CFW_GetAoUserData(hSimGetPUCTAo);

    switch (pSimGetPUCTData->nSm_GetPUCT_currStatus)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            UINT8 ef = 0;
            if (g_cfw_sim_status[nSimID].UsimFlag)
                ef = API_USIM_EF_PUCT;
            else
                ef = API_SIM_EF_PUCT;

            nErrCode = SimReadBinaryReq(ef, 0, 5, nSimID);
            if (ERR_SUCCESS != nErrCode)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_RSP fail:0x%x\r\n", 0x08100c98)), nErrCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PUCT_RSP, nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetPUCTAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCTProc);
                return nErrCode;
            }
            pSimGetPUCTData->nSm_GetPUCT_prevStatus = pSimGetPUCTData->nSm_GetPUCT_currStatus;
            pSimGetPUCTData->nSm_GetPUCT_currStatus = CFW_SIM_MIS_READBINARY;
            //            CFW_SetAoProcCode(hSimGetPUCTAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else
            CFW_SetAoProcCode(hSimGetPUCTAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;
    case CFW_SIM_MIS_READBINARY:
    {
        if (pEvent->nEventId == API_SIM_READBINARY_CNF)
        {
            api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)pEvent->nParam1;
            if ((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
            {
                UINT16 iEPPU = 0;
                INT8 iEX = 0;        // mabo changed from INT8 to UINT8
                UINT8 iSignOfEX = 0; // mabo added

                // Data[3]存储EPPU的高八位(bit12~bit5),Data[4]存储EPPU的低四位(bit4~bit1)
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_PROC Data[3] = %x,Data[4] = %x\n", 0x08100c99)),
                          pSimReadBinaryCnf->Data[3], pSimReadBinaryCnf->Data[4]);
                iEPPU = (((UINT16)pSimReadBinaryCnf->Data[3] << 4) | ((UINT16)(pSimReadBinaryCnf->Data[4] & 0x0F)));
                iEX = (UINT8)(pSimReadBinaryCnf->Data[4] >> 5); // Data[4]的高三位存储EX
                if (pSimReadBinaryCnf->Data[4] & 0x10)          // Data[4]的bit5为EX的符号位，1表示负号
                {
                    iSignOfEX = 1;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_PROC iEPPU = %x,iEX = %x\n", 0x08100c9a)), iEPPU, iEX);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_PROC iSignOfEX = %d\r\n", 0x08100c9b)), iSignOfEX);
                CFW_SIM_PUCT_INFO *pPUCTData = (CFW_SIM_PUCT_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SIM_PUCT_INFO));
                if (pPUCTData == NULL)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR: ERR_NO_MORE_MEMORY\n", 0x08100c9c)));
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PUCT_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCTProc);
                    return ERR_NO_MORE_MEMORY;
                }
                pPUCTData->iCurrency[0] = pSimReadBinaryCnf->Data[0];
                pPUCTData->iCurrency[1] = pSimReadBinaryCnf->Data[1];
                pPUCTData->iCurrency[2] = pSimReadBinaryCnf->Data[2];
                pPUCTData->iEPPU = iEPPU;
                pPUCTData->iEX = iEX;
                pPUCTData->iSign = iSignOfEX;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_RSP success:Currency = %c%c%c\r\n", 0x08100c9d)),
                          pPUCTData->iCurrency[0], pPUCTData->iCurrency[1], pPUCTData->iCurrency[2]);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3),
                          TSTXT(TSTR("EV_CFW_SIM_GET_PUCT_RSP success:iEPPU = %d,iEX = %d,iSign = %d\r\n", 0x08100c9e)),
                          pPUCTData->iEPPU, pPUCTData->iEX, pPUCTData->iSign);

                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PUCT_RSP, (UINT32)pPUCTData, SIZEOF(CFW_SIM_PUCT_INFO),
                                    nUTI | (nSimID << 24), 0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetPUCTAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCTProc);
                return ERR_SUCCESS;
            }
            else // 操作失败的状态，向mmi返回error code
            {
                // UINT32 nErrCode = 0;
                nErrCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_GET_PUCT_RSP 0x%x\r\n", 0x08100c9f)), nErrCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PUCT_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimGetPUCTAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCTProc);
                return nErrCode;
            }
        }
        else // 此event不是本函数当前想要处理的event
        {
            CFW_SetAoProcCode(hSimGetPUCTAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    return ERR_SUCCESS;
}

#if defined(__MMI_LOCK_PROVIDER_SIM__)
extern void SimProviderIdProc(void *IdDataPtr, UINT32 IdLenth, UINT8 SimId);
#endif
UINT32 CFW_SimSetPUCTProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimSetPUCTAo = hAo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetPUCTProc);

    UINT32 nUTI = 0;
    CFW_GetUTI(hSimSetPUCTAo, &nUTI);
    UINT32 nErrCode = 0;
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimSetPUCTAo);
    CFW_SIMSETPUCT *pSimSetPUCTData = CFW_GetAoUserData(hSimSetPUCTAo);

    switch (pSimSetPUCTData->nSm_SetPUCT_currStatus)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            SIM_CHV_PARAM *pSimCHV;
            CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("CFW_SimSetPUCTProc: pSimCHV->nPin2Status = %d\n", 0x08100ca0)), pSimCHV->nPin2Status);
            if (pSimCHV->nPin2Status != CHV_VERIFY)
            {
                pSimCHV->bCHV2VerifyReq = TRUE;
                nErrCode = SimVerifyCHVReq(pSimSetPUCTData->pPin2, CHV2, nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0XF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error SimElemFileStatusReq return", 0x08100ca1));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                    return nErrCode;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" pSimSetPUCTData->nSm_SetPUCT_currStatus = CFW_SIM_MIS_VERIFY\n", 0x08100ca2)));
                pSimSetPUCTData->nSm_SetPUCT_prevStatus = pSimSetPUCTData->nSm_SetPUCT_currStatus;
                pSimSetPUCTData->nSm_SetPUCT_currStatus = CFW_SIM_MIS_VERIFY;
            }
            else
            {
                UINT8 ef = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                    ef = API_USIM_EF_PUCT;
                else
                    ef = API_SIM_EF_PUCT;
                // EFpuct文件 第五个字节 :
                // |---bit8~bit6---|--- bit5 ---|---bit4~bit1---|
                // |---    EX      ---|-sign of EX -|---   EPPU   ---|

                UINT8 ex = 0;
                UINT8 nData[5];
                SUL_MemCopy8(nData, pSimSetPUCTData->iCurrency, 3);
                nData[3] = pSimSetPUCTData->iEPPU >> 4;
                nData[4] = pSimSetPUCTData->iEPPU & 0x0F;
                if (pSimSetPUCTData->iEX < 0)
                {
                    nData[4] |= 0x10;
                    ex = -(pSimSetPUCTData->iEX);
                }
                else
                    ex = pSimSetPUCTData->iEX;
                nData[4] |= ex << 5;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("pSimSetPUCTData->iEPPU = %x,iEX= %x\n", 0x08100ca3)),
                          pSimSetPUCTData->iEPPU, pSimSetPUCTData->iEX);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("pSimSetPUCTData->nData[3] = %x,nData[4]= %x\n", 0x08100ca4)),
                          nData[3], nData[4]);
                nErrCode = SimUpdateBinaryReq(ef, 0, 5, nData, nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_GET_PUCT_RSP 0x%x\n", 0x08100ca5)), nErrCode);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetPUCTAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                    return nErrCode;
                }
                pSimSetPUCTData->nSm_SetPUCT_prevStatus = pSimSetPUCTData->nSm_SetPUCT_currStatus;
                pSimSetPUCTData->nSm_SetPUCT_currStatus = CFW_SIM_MIS_UPDATEBINARY;
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;
    case CFW_SIM_MIS_VERIFY:
    {
        if (pEvent->nEventId == API_SIM_VERIFYCHV_CNF)
        {
            SIM_CHV_PARAM *pSimCHV;
            CFW_CfgSimGetChvParam(&pSimCHV, nSimID);
            api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF, pSimVerifyCHVCnf->Sw1 = %x, \
pSimVerifyCHVCnf->Sw2 = %x",
                                                                                       0x08100ca6),
                      pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);
            if (((0x90 == pSimVerifyCHVCnf->Sw1) && (0x00 == pSimVerifyCHVCnf->Sw2)) || (0x91 == pSimVerifyCHVCnf->Sw1))
            {
                pSimCHV->bCHV2VerifyReq = FALSE;
                pSimCHV->nPin2Status = CHV_VERIFY;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                {
                    pSimCHV->nPin2Remain = pSimCHV->nPin2Max;
                    pSimCHV->nPuk2Remain = pSimCHV->nPuk2Max;
                }
                UINT8 ef = 0;
                if (g_cfw_sim_status[nSimID].UsimFlag)
                    ef = API_USIM_EF_PUCT;
                else
                    ef = API_SIM_EF_PUCT;
                // EFpuct文件 第五个字节 :
                // |---bit8~bit6---|--- bit5 ---|---bit4~bit1---|
                // |---    EX      ---|-sign of EX -|---   EPPU   ---|

                UINT8 ex = 0;
                UINT8 nData[5];
                SUL_MemCopy8(nData, pSimSetPUCTData->iCurrency, 3);
                nData[3] = pSimSetPUCTData->iEPPU >> 4;
                nData[4] = pSimSetPUCTData->iEPPU & 0x0F;
                if (pSimSetPUCTData->iEX < 0)
                {
                    nData[4] |= 0x10;
                    ex = -(pSimSetPUCTData->iEX);
                }
                else
                    ex = pSimSetPUCTData->iEX;
                nData[4] |= ex << 5;
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("pSimSetPUCTData->iEPPU = %x,iEX= %x\n", 0x08100ca7)),
                          pSimSetPUCTData->iEPPU, pSimSetPUCTData->iEX);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("pSimSetPUCTData->nData[3] = %x,nData[4]= %x\n", 0x08100ca8)),
                          nData[3], nData[4]);
                nErrCode = SimUpdateBinaryReq(ef, 0, 5, nData, nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("Error EV_CFW_SIM_GET_PUCT_RSP 0x%x\n", 0x08100ca9)), nErrCode);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetPUCTAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                    return nErrCode;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("pSimCVH->nPin2Status == CHV_VERIFY)\n", 0x08100caa)));
                pSimSetPUCTData->nSm_SetPUCT_prevStatus = pSimSetPUCTData->nSm_SetPUCT_currStatus;
                pSimSetPUCTData->nSm_SetPUCT_currStatus = CFW_SIM_MIS_UPDATEBINARY;
            }
            else
            {
                if (g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    CFW_USimParsePinStatus(pSimVerifyCHVCnf, pSimCHV, CHV2, nSimID);
                nErrCode = Sim_ParseSW1SW2(pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("ERROR: EV_CFW_SIM_GET_PUCT_RSP nErrCode = %d\n", 0x08100cab)), nErrCode);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                return nErrCode;
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;
    case CFW_SIM_MIS_UPDATEBINARY:
    {
        if (pEvent->nEventId == API_SIM_UPDATEBINARY_CNF)
        {
            api_SimUpdateBinaryCnf_t *pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)pEvent->nParam1;
            if ((0x90 == pSimUpdateBinaryCnf->Sw1) && (0x00 == pSimUpdateBinaryCnf->Sw2)) // 操作成功的状态
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("OK: EV_CFW_SIM_SET_PUCT_RSP success\n", 0x08100cac)));
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, 0, 0, nUTI | (nSimID << 24), 0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetPUCTAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                return ERR_SUCCESS;
            }
            else
            {
                nErrCode = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("ERROR: EV_CFW_SIM_GET_PUCT_RSP  0x%x\n", 0x08100cad)), nErrCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PUCT_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimSetPUCTAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCTProc);
                return nErrCode;
            }
        }
        else // 此event不是本函数当前想要处理的event
        {
            CFW_SetAoProcCode(hSimSetPUCTAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimMisGetProviderIdProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;
    CFW_EV ev;
    UINT8 IMSIAscLen = 0;
    UINT8 *pIMSIAscData = NULL;
    SIM_SM_INFO_GETPROVIDERID *pMisGetProviderID = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisGetProviderIdProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisGetProviderID = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisGetProviderID->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimMisGetProviderIdProc()\r\n", 0x08100cae)));
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("pGetACM->nState.nCurrState = 0x%x,nSimID = %d \n", 0x08100caf), pMisGetProviderID->nState.nNextState, nSimID);
    switch (pMisGetProviderID->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag)
            result = SimReadBinaryReq(API_USIM_EF_IMSI, 0, 0x09, nSimID);
        else
            result = SimReadBinaryReq(API_SIM_EF_IMSI, 0, 0x09, nSimID);
        if (ERR_SUCCESS != result)
        {
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetProviderIdProc);
            return ERR_SUCCESS;
        }
        pMisGetProviderID->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisGetProviderID->nState.nNextState = CFW_SIM_MIS_READBINARY;
        break;

    case CFW_SIM_MIS_READBINARY:
        if (nEvtId == API_SIM_READBINARY_CNF) // 用event id来鉴别此event是否为本函数当前想要处理的event
        {
            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                if ((pSimReadBinaryCnf->Data[0] > 9) || (pSimReadBinaryCnf->Data[0] == 0))
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xf0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_CME_UNKNOWN;
                }
                if (FALSE == g_GETIMSI[nSimID])
                {
                    UINT8 nPreIMSI[12];
                    SUL_ZeroMemory8(nPreIMSI, 12);
                    UINT32 nRet = CFW_CfgGetIMSI(nPreIMSI, nSimID);
                    if (ERR_SUCCESS != nRet)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("The CFW_CfgGetIMSI nRet  is 0x%x\n", 0x08100cb0), nRet);
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xf0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_CME_UNKNOWN;
                    }
                    if (0x00 == SUL_MemCompare(pSimReadBinaryCnf->Data, nPreIMSI, 9))
                    {
                        nPreIMSI[9] = 0x00;
                        nRet = CFW_CfgSetIMSI(nPreIMSI, nSimID);
                        if (ERR_SUCCESS != nRet)
                        {
                            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("The CFW_CfgSetIMSI nRet  is 0x%x\n", 0x08100cb1), nRet);
                            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                            return ERR_CME_UNKNOWN;
                        }
                    }
                    else
                    {
                        nPreIMSI[9] = 0x01;
                        SUL_MemCopy8(nPreIMSI, pSimReadBinaryCnf->Data, 9);
                        nRet = CFW_CfgSetIMSI(nPreIMSI, nSimID);
                        if (ERR_SUCCESS != nRet)
                        {
                            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, ERR_CME_UNKNOWN, 0, nUTI | (nSimID << 24), 0xf0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                            return ERR_CME_UNKNOWN;
                        }
                        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("The nPreIMSI[9] is %d \n", 0x08100cb2), nPreIMSI[9]);
                    }
                    g_GETIMSI[nSimID] = TRUE;
                }
                pIMSIAscData = (UINT8 *)CSW_SIM_MALLOC(16);
                if (pIMSIAscData == NULL)
                {
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetProviderIdProc);
                    return ERR_SUCCESS;
                }
                cfw_IMSItoASC(pSimReadBinaryCnf->Data, pIMSIAscData, &IMSIAscLen);
#if defined(__MMI_LOCK_PROVIDER_SIM__)
                SimProviderIdProc(pIMSIAscData, IMSIAscLen, nSimID);
#endif
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, (UINT32)pIMSIAscData, IMSIAscLen, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PROVIDER_ID_RSP Success! \n", 0x08100cb3)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PROVIDER_ID_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetProviderIdProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimGetICCIDProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId, nUTI, ErrorCode;
    VOID *nEvParam;

    nEvtId = 0;
    nUTI = 0;
    ErrorCode = 0;
    nEvParam = (VOID *)NULL;
    SIM_SM_INFO_GETICCID *pGetICCID = CFW_GetAoUserData(hAo);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pGetICCID->n_CurrStatus)
    {
    case CFW_SIM_ICCID_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pGetICCID->nTryCount >= 0x01)
            {
                ErrorCode = SimReadBinaryReq(API_SIM_EF_ICCID, 0, 10, nSimID);
                if (ERR_SUCCESS != ErrorCode)
                {
                    CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_ICCID_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0XF0);
                    TS_OutputText(CFW_SIM_TS_ID, TSTXT("++++====>>>>SimReadBinaryReq err,The ErrCode is %d\n"), ErrorCode);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ErrorCode;
                }

                CFW_SetAoState(hAo, CFW_AO_PROC_CODE_CONSUMED);
                pGetICCID->n_PrevStatus = pGetICCID->n_CurrStatus;
                pGetICCID->n_CurrStatus = CFW_SIM_ICCID_READBINARY;
            }
            else
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetICCIDProc);
                return ERR_SUCCESS;
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;

    case CFW_SIM_ICCID_READBINARY:
    {
        if (pEvent->nEventId == API_SIM_READBINARY_CNF)
        {
            api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
            if ((0x90 == pSimReadBinaryCnf->Sw1) && (00 == pSimReadBinaryCnf->Sw2))
            {
                UINT8 *nICCID = (UINT8 *)CSW_SIM_MALLOC(10);
                SUL_MemCopy8(nICCID, pSimReadBinaryCnf->Data, 10);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ICCID_RSP, (UINT32)nICCID, 0, nUTI | (nSimID << 24), 0);
                TS_OutputText(CFW_SIM_TS_ID, TSTXT("post EV_CFW_GET_ICCID_RSP Success! nUTI is %d\n"), nUTI);
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_ICCID_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xf0);
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    return ERR_SUCCESS;
}

//#ifdef CFW_EXTENDED_API
#if 1

// ================================================================================
//  Function   : cfw_SimBinaryDataParserDefault
// --------------------------------------------------------------------------------
//
//  Scope      : Read binary EF and send back the whole data content
//  Parameters : hAo -> Ao
//             : pValue -> pointer to the returned data (CFW's event param1)
//             : pResult -> binary read result structure
//             : pUserData -> the user (auxiliary) data containing the file Id & size
//             : simId -> SIM Id (dual SIM only)
//  Return     : none
// ================================================================================

VOID cfw_SimBinaryDataParserDefault(HAO hAo, UINT8 *pValue, api_SimReadBinaryCnf_t *pResult, SIM_SM_INFO_EF *pUserData, CFW_SIM_ID simId)
{
    // copy and sent result
    SUL_ZeroMemory8(pValue, pUserData->fileSize);
    SUL_MemCopy8(pValue, pResult->Data, pUserData->fileSize);
    CFW_SIM_SEND_RESULT(pValue, pUserData->fileSize | (pUserData->fileId << 8), EV_CFW_SIM_READ_EF_RSP);
}

// ================================================================================
//  Function   : cfw_SimReadUpdateElementaryFileProc
// --------------------------------------------------------------------------------
//
//  Scope      : Generic binary EF read & update function
//  Parameters : hAo -> Ao
//             : pEvent -> CFW event
//  Return     : CFW error
// ================================================================================
UINT32 CFW_SimReadUpdateElementaryFileProc(HAO hAo, CFW_EV *pEvent)
{
    SIM_SM_INFO_EF *pUserData = CFW_GetAoUserData(hAo);
    CFW_SIM_ID simId = CFW_GetSimCardID(hAo);
    SIM_DATA_PARSER_EF_T simDataParser;

    switch (pUserData->fileId)
    {
    case API_SIM_EF_PUCT:
        simDataParser = cfw_SimBinaryDataParserDefault;
        break;
    default:
        simDataParser = cfw_SimBinaryDataParserDefault;
        break;
    }

    switch (pUserData->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            // check remaining number of tries
            CFW_SIM_ASSERT_ERROR((pUserData->nTryCount < 1), ERR_CME_SIM_FAILURE, EV_CFW_SIM_READ_EF_RSP);

            // get file information
            UINT32 errorCode = SimElemFileStatusReq(CFW_WITH_SIM_ID(pUserData->fileId, simId));
            CFW_SIM_ASSERT_ERROR(errorCode, errorCode, EV_CFW_SIM_READ_EF_RSP);

            pUserData->nState.nCurrState = pUserData->nState.nNextState;
            pUserData->nState.nNextState = CFW_SIM_MIS_READELEMENT;

            // update read state
            CFW_SetAoState(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case CFW_SIM_MIS_READELEMENT:
    {
        if (pEvent->nEventId == API_SIM_ELEMFILESTATUS_CNF)
        {
            api_SimElemFileStatusCnf_t *pResult = ((UINT32)pEvent != 0xFFFFFFFF) ? (api_SimElemFileStatusCnf_t *)pEvent->nParam1 : NULL;

            // check SIM read status
            UINT32 errorCode = Sim_ParseSW1SW2(pResult->Sw1, CFW_WITH_SIM_ID(pResult->Sw2, simId));
            CFW_SIM_ASSERT_ERROR(CFW_SIM_ERROR(pResult), errorCode, EV_CFW_SIM_READ_EF_RSP);

            // print file size
            pUserData->fileSize = ((pResult->Data[2] << 8) & 0xff00) + pResult->Data[3];
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("CFW SIM : file size is %d", 0x08100cb4)), pUserData->fileSize);

            // read SIM binary file
            errorCode = SimReadBinaryReq(pUserData->fileId, 0, CFW_WITH_SIM_ID(pUserData->fileSize, simId));
            CFW_SIM_ASSERT_ERROR(errorCode, errorCode, EV_CFW_SIM_READ_EF_RSP);

            // goto update state if required
            pUserData->nState.nCurrState = pUserData->nState.nNextState;
            pUserData->nState.nNextState = (pUserData->update) ? CFW_SIM_MIS_UPDATEBINARY : CFW_SIM_MIS_READBINARY;
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case CFW_SIM_MIS_READBINARY:
    {
        if (pEvent->nEventId == API_SIM_READBINARY_CNF)
        {
            api_SimReadBinaryCnf_t *pResult = ((UINT32)pEvent != 0xFFFFFFFF) ? (api_SimReadBinaryCnf_t *)pEvent->nParam1 : NULL;

            // check SIM read status
            UINT32 errorCode = Sim_ParseSW1SW2(pResult->Sw1, CFW_WITH_SIM_ID(pResult->Sw2, simId));
            CFW_SIM_ASSERT_ERROR(CFW_SIM_ERROR(pResult), errorCode, EV_CFW_SIM_READ_EF_RSP);

            // allocate result message
            UINT8 *pValue = CSW_SIM_MALLOC(pUserData->fileSize);
            CFW_SIM_ASSERT_ERROR((pValue == NULL), ERR_NO_MORE_MEMORY, EV_CFW_SIM_READ_EF_RSP);

            // parse the SIM data and send the results
            simDataParser(hAo, pValue, pResult, CFW_WITH_SIM_ID(pUserData, simId));
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case CFW_SIM_MIS_UPDATEBINARY:
        break;

    default:
        break;
    }
    return ERR_SUCCESS;
}

/*
================================================================================
  Function   : CFW_SimMisSetPrefListProcEX
--------------------------------------------------------------------------------

  Scope      : The process for setting the PLMNs in SIM
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimMisSetPrefListProcEX(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0;
    UINT32 nEvtId = 0;
    UINT32 ErrorCode = 0;
    UINT32 result = 0;
    VOID *nEvParam = NULL;

    CFW_EV ev;
    SIM_SM_INFO_SETPREFOPREATORLIST *pMisSetPrefList = NULL;
    api_SimUpdateBinaryCnf_t *pSimUpdateBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisSetPrefListProc);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisSetPrefList = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimMisSetPrefListProc()\r\n", 0x08100cb5)));

    switch (pMisSetPrefList->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            result = SimElemFileStatusReq(API_SIM_EF_PLMNSEL, nSimID);
        else
            result = SimElemFileStatusReq(API_USIM_EF_PLMNWACT, nSimID);
        if (ERR_SUCCESS != result)
        {
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetPrefListProcEX);
            return ERR_SUCCESS;
        }
        pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                pMisSetPrefList->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                result = SimUpdateBinaryReq(API_SIM_EF_PLMNSEL, 0, pMisSetPrefList->nSize, pMisSetPrefList->nOperatorList, nSimID);
            }
            else // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                pMisSetPrefList->nSize = SimEfStatus.fileSize;
                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisSetPrefList->nSize, nSimID);
            }
            if (ERR_SUCCESS != result)
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetPrefListProcEX);
                return ERR_SUCCESS;
            }
            pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_IDLE;
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
                pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_UPDATEBINARY;
            else
                pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_PREREADBINARY;
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetPrefListProcEX);
            return ERR_SUCCESS;
        }
        break;

    case CFW_SIM_MIS_UPDATEBINARY:
        pSimUpdateBinaryCnf = (api_SimUpdateBinaryCnf_t *)nEvParam;
        if (((0x90 == pSimUpdateBinaryCnf->Sw1) && (0x00 == pSimUpdateBinaryCnf->Sw2)) || (0x91 == pSimUpdateBinaryCnf->Sw1))
        {
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0);
            CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP Success! \n", 0x08100cb6)));
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimUpdateBinaryCnf->Sw1, pSimUpdateBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP failure! \n", 0x08100cb7)));
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    case CFW_SIM_MIS_PREREADBINARY:
        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        ;
        UINT8 UsimPlmnwact[255] = {0};
        pMisSetPrefList->nSize = pMisSetPrefList->nSize / 5 * 5;

        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            SUL_MemCopy8(UsimPlmnwact, pSimReadBinaryCnf->Data, pMisSetPrefList->nSize);
            for (UINT8 i = 0; i < pMisSetPrefList->nSize / 5; i++)
            {
                SUL_MemCopy8(&(UsimPlmnwact[i * 5]), &(pMisSetPrefList->nOperatorList[i * 3]), 3);
            }
            result = SimUpdateBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisSetPrefList->nSize, UsimPlmnwact, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n", 0x08100cb8), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        pMisSetPrefList->nState.nCurrState = CFW_SIM_MIS_PREREADBINARY;
        pMisSetPrefList->nState.nNextState = CFW_SIM_MIS_UPDATEBINARY;
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisSetPrefListProc);
    return ERR_SUCCESS;
}

/*
================================================================================
  Function   : CFW_SimMisGetPrefListProcEX
--------------------------------------------------------------------------------
  Scope      : The process for Getting the PLMNs in SIM
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimMisGetPrefListProcEX(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI = 0x00;
    UINT32 nEvtId = 0x00;
    UINT32 ErrorCode = 0x00;
    UINT32 result = 0x00;
    VOID *nEvParam = NULL;

    CFW_EV ev;
    SIM_SM_INFO_GETPREFOPREATORLIST *pMisGetPreflist = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimMisGetPrefListProcEX);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pMisGetPreflist = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);
    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimMisGetPrefListProcEX()\r\n", 0x08100cb9)));
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " UsimFlag %d\n", g_cfw_sim_status[nSimID].UsimFlag);
    switch (pMisGetPreflist->nState.nNextState)
    {
    case CFW_SIM_MIS_IDLE:
        if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            result = SimElemFileStatusReq(API_SIM_EF_PLMNSEL, nSimID);
        else // USIM
            result = SimElemFileStatusReq(API_USIM_EF_PLMNWACT, nSimID);
        if (ERR_SUCCESS != result)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimMisGetPrefListProcEX SimElemFileStatusReq failed \r\n", 0x08100cba)));
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
            return ERR_SUCCESS;
        }
        pMisGetPreflist->nState.nCurrState = CFW_SIM_MIS_IDLE;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_READELEMENT;
        break;

    case CFW_SIM_MIS_READBINARY:

        pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
        UINT8 *UsimPlmn = NULL;
        if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
        {
            //CSW_TC_MEMBLOCK(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL),pSimReadBinaryCnf->Data,250,16);
            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                pMisGetPreflist->nSize = pMisGetPreflist->nSize / 5 * 3;

                UINT8 *pPLMN = (UINT8 *)CSW_SIM_MALLOC(2 * (pMisGetPreflist->nSize));
                if (pPLMN == NULL)
                {
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                    return ERR_SUCCESS;
                }
                CFW_PLMNtoBCDEX(pSimReadBinaryCnf->Data, pPLMN, &(pMisGetPreflist->nSize));
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)pPLMN, pMisGetPreflist->nSize, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP success! \n", 0x08100cbb)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                return ERR_SUCCESS;
            }
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " nTotalTimes %d nReadTimes %d\n", pMisGetPreflist->nTotalTimes, pMisGetPreflist->nReadTimes);
            //For USIM
            if (pMisGetPreflist->nReadTimes == pMisGetPreflist->nTotalTimes)
            {

                if (1 == pMisGetPreflist->nReadTimes)
                {
                    UINT8 *pPLMN = (UINT8 *)CSW_SIM_MALLOC(pMisGetPreflist->nSize);
                    if (pPLMN == NULL)
                    {
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                        return ERR_SUCCESS;
                    }
                    SUL_ZeroMemory8(pPLMN, pMisGetPreflist->nSize);
                    pMisGetPreflist->pData = pPLMN;
                    SUL_MemCopy8(pMisGetPreflist->pData, pSimReadBinaryCnf->Data, pMisGetPreflist->nSize);
                }
                else
                {

                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " nSize %d nReadTimes %d\n", pMisGetPreflist->nSize, pMisGetPreflist->nReadTimes);
                    //CSW_TC_MEMBLOCK(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL),pMisGetPreflist->pData,pMisGetPreflist->nSize,16);
                    SUL_MemCopy8(pMisGetPreflist->pData + pMisGetPreflist->nSize % 250 + 250 * (pMisGetPreflist->nReadTimes - 2), pSimReadBinaryCnf->Data, 250);
                    //CSW_TC_MEMBLOCK(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL),pMisGetPreflist->pData,pMisGetPreflist->nSize,16);
                }

                UsimPlmn = (UINT8 *)CSW_SIM_MALLOC(pMisGetPreflist->nSize);
                if (!UsimPlmn)
                {
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                    return ERR_SUCCESS;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " pMisGetPreflist->nSize %d \n", pMisGetPreflist->nSize);
                SUL_ZeroMemory8(UsimPlmn, pMisGetPreflist->nSize);
                //pMisGetPreflist->nSize = pMisGetPreflist->nSize / 5 * 3;
                for (UINT8 i = 0; i < pMisGetPreflist->nSize / 5; i++)
                {
                    SUL_MemCopy8(&(UsimPlmn[i * 3]), &(pMisGetPreflist->pData[i * 5]), 3);
                }
                pMisGetPreflist->nSize = pMisGetPreflist->nSize / 5 * 3;
                SUL_MemCopy8(pMisGetPreflist->pData, UsimPlmn, pMisGetPreflist->nSize);
                CSW_SIM_FREE(UsimPlmn);
                UINT8 *pPLMN = (UINT8 *)CSW_SIM_MALLOC(2 * (pMisGetPreflist->nSize));
                if (pPLMN == NULL)
                {
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                    return ERR_SUCCESS;
                }
                SUL_ZeroMemory8(pPLMN, 2 * (pMisGetPreflist->nSize));
                CFW_PLMNtoBCDEX(pMisGetPreflist->pData, pPLMN, &(pMisGetPreflist->nSize));
                CSW_TC_MEMBLOCK(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), pMisGetPreflist->pData, pMisGetPreflist->nSize, 16);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)pPLMN, pMisGetPreflist->nSize, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP success! \n", 0x08100cbb)));
            }
            else
            {
                if (1 == pMisGetPreflist->nReadTimes)
                {
                    UINT8 *pPLMN = (UINT8 *)CSW_SIM_MALLOC(pMisGetPreflist->nSize);
                    if (pPLMN == NULL)
                    {
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                        return ERR_SUCCESS;
                    }
                    SUL_ZeroMemory8(pPLMN, pMisGetPreflist->nSize);
                    pMisGetPreflist->pData = pPLMN;
                    SUL_MemCopy8(pMisGetPreflist->pData, pSimReadBinaryCnf->Data, (pMisGetPreflist->nSize % 250)
                                                                                      ? (pMisGetPreflist->nSize % 250)
                                                                                      : 250);
                }
                else
                {
                    SUL_MemCopy8(pMisGetPreflist->pData + pMisGetPreflist->nSize % 250 + 250 * (pMisGetPreflist->nReadTimes - 2), pSimReadBinaryCnf->Data, 250);
                }

                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, pMisGetPreflist->nSize % 250 + 250 * (pMisGetPreflist->nReadTimes - 1),
                                          250, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                    return ERR_SUCCESS;
                }
                pMisGetPreflist->nReadTimes++;
                return ERR_SUCCESS;
            }
        }
        else
        {
            ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP failure! \n", 0x08100cbc)));
        }
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;

    case CFW_SIM_MIS_READELEMENT:
        pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
        if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
        {

            if (g_cfw_sim_status[nSimID].UsimFlag == FALSE) // SIM
            {
                pMisGetPreflist->nSize = ((pSimElemFileStatusCnf->Data[2] << 8) & 0xFF00) + pSimElemFileStatusCnf->Data[3];
                result = SimReadBinaryReq(API_SIM_EF_PLMNSEL, 0, pMisGetPreflist->nSize, nSimID);
            }
            else // USIM
            {
                CFW_UsimEfStatus SimEfStatus;
                CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &SimEfStatus);
                pMisGetPreflist->nSize = SimEfStatus.fileSize;
                pMisGetPreflist->nTotalTimes = (pMisGetPreflist->nSize % 250) ? (pMisGetPreflist->nSize / 250 + 1) : (pMisGetPreflist->nSize / 250);
                pMisGetPreflist->nReadTimes = 1;
                result = SimReadBinaryReq(API_USIM_EF_PLMNWACT, 0, pMisGetPreflist->nSize % 250, nSimID);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " First read data length= %d \n", pMisGetPreflist->nSize % 250);
            }

            if (ERR_SUCCESS != result)
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProcEX);
                return ERR_CFW_INVALID_PARAMETER;
            }
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" SIM GET PREFOPREATORLIST ERROR! \n", 0x08100cbd)));
            ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
            CFW_PostNotifyEvent(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            return ERR_CFW_INVALID_PARAMETER;
        }
        pMisGetPreflist->nState.nCurrState = CFW_SIM_MIS_READELEMENT;
        pMisGetPreflist->nState.nNextState = CFW_SIM_MIS_READBINARY;
        break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimMisGetPrefListProc);
    return ERR_SUCCESS;
}

/*
================================================================================
  Function   : CFW_SimGetFileStatusProc
--------------------------------------------------------------------------------

  Scope      :
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimGetFileStatusProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId, nUTI, ErrorCode;
    VOID *nEvParam;
    GET_FILE_STATUS_INFO *pGetFileStatus;

    nEvtId = 0;
    nUTI = 0;
    ErrorCode = 0;
    nEvParam = (VOID *)NULL;
    pGetFileStatus = (GET_FILE_STATUS_INFO *)NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetFileStatusProc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pGetFileStatus = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    switch (pGetFileStatus->n_CurrStatus)
    {
    case CFW_SIM_MIS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pGetFileStatus->nTryCount >= 0x01)
            {
                ErrorCode = SimElemFileStatusReq(pGetFileStatus->nFileID, nSimID);
                if (ERR_SUCCESS != ErrorCode)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FILE_STATUS_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0XF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("SimElemFileStatusReq err,The ErrCode is %d\n", 0x08100cbe)), ErrorCode);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatusProc);
                    return ErrorCode;
                }

                CFW_SetAoState(hAo, CFW_SM_SIM_STATE_WRITE);
                pGetFileStatus->n_PrevStatus = pGetFileStatus->n_CurrStatus;
                pGetFileStatus->n_CurrStatus = CFW_SIM_MIS_READELEMENT;
            }
            else
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;

    case CFW_SIM_MIS_READELEMENT:
    {
        if (pEvent->nEventId == API_SIM_ELEMFILESTATUS_CNF)
        {
            api_SimElemFileStatusCnf_t *pSimEFStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
            if (((0x90 == pSimEFStatusCnf->Sw1) && (00 == pSimEFStatusCnf->Sw2)) || (0x91 == pSimEFStatusCnf->Sw1))
            {
/* 3gpp11.11 page50
                            Structure of file
                            '00'  transparent;
                            '01'  linear fixed;
                            '03'  cyclic.

                            Type of File
                            '00'  RFU;
                            '01'  MF;
                            '02'  DF;
                            '04'  EF.
                            */
#if 0
                    //
                    //参数P1低16位为文件长度，高16位为文件类型。
                    //
                    UINT32 p1 = ((pSimEFStatusCnf->Data[2] << 8) & 0xFF00) + pSimEFStatusCnf->Data[3];
                    p1 += (UINT32)(pSimEFStatusCnf->Data[6] << 16);
                    //
                    //参数P2低三个字节是ACCESS CONDITIONS,高1个字节是文件状态。
                    //
                    UINT32 p2 = (UINT32) ((pSimEFStatusCnf->Data[8] << 16) + (pSimEFStatusCnf->Data[9] << 8)
                                          + pSimEFStatusCnf->Data[10] + (pSimEFStatusCnf->Data[11] << 24));
                    CFW_PostNotifyEvent (EV_CFW_SIM_GET_FILE_STATUS_RSP, (UINT32)p1, p2, nUTI | (nSimID << 24), 0);
                    CSW_TRACE(_CSW|TLEVEL(33)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_FILE_STATUS_RSP Success! \n",0x08100cbf)));
#else
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, pSimEFStatusCnf->Data, 50, 16);
                if (pSimEFStatusCnf->Data[0] == 0x62)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FILE_STATUS_RSP, (UINT32)pSimEFStatusCnf->Data, pSimEFStatusCnf->Data[1] + 2, nUTI | (nSimID << 24), 0x00);
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_FILE_STATUS_RSP, (UINT32)pSimEFStatusCnf->Data, 256, nUTI | (nSimID << 24), 0x00);
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("post EV_CFW_SIM_GET_FILE_STATUS_RSP Success! \n", 0x08100cc0)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatusProc);
                return ERR_SUCCESS;
#endif
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimEFStatusCnf->Sw1, pSimEFStatusCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_FILE_STATUS_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xf0);
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatusProc);
    return ERR_SUCCESS;
}

/*
================================================================================
  Function   : CFW_SimReadRecordProc
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimReadRecordProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId = 0;
    UINT32 nUTI = 0;
    UINT32 nErrCode = 0;
    VOID *nEvParam = (VOID *)NULL;
    SIM_INFO_READRECORD *pReadSimInfo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadRecordProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pReadSimInfo = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    switch (pReadSimInfo->n_CurrStatus)
    {
    case SIM_STATUS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pReadSimInfo->nTryCount >= 0x01)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cc1), __LINE__, __FILE__);
                nErrCode = SimElemFileStatusReq((UINT8)(pReadSimInfo->nFileId), nSimID);
                if (ERR_SUCCESS != nErrCode)
                {
                    CFW_GetUTI(hAo, &nUTI);
                    hal_HstSendEvent(0xfa022201);
                    hal_HstSendEvent(nErrCode);
                    CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)nErrCode,
                                        0, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("SimElemFileStatusReq Failed in CFW_SimReadRecordProc\n", 0x08100cc2)));
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                    return nErrCode;
                }
                pReadSimInfo->n_PrevStatus = pReadSimInfo->n_CurrStatus;
                pReadSimInfo->n_CurrStatus = SIM_STATUS_ELEMFILE;
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;

    case SIM_STATUS_ELEMFILE:
    {
        if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
        {
            api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
            if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cc3), __LINE__, __FILE__);
                if (g_cfw_sim_status[nSimID].UsimFlag == FALSE)
                {
                    pReadSimInfo->nRecordSize = pSimElemFileStatusCnf->Data[14];
                }
                else
                {
                    CFW_UsimEfStatus gUsim;
                    CFW_UsimDecodeEFFcp(pSimElemFileStatusCnf->Data, &gUsim);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("gUsim.recordLength %d\n", 0x08100cc4)), gUsim.recordLength);
                    pReadSimInfo->nRecordSize = gUsim.recordLength; // transparent file: 0.  else file: one record length
                }
                UINT32 ErrorCode = SimReadRecordReq(pReadSimInfo->nFileId, pReadSimInfo->nRecordNum, SIM_MODE_ABSOLUTE, pReadSimInfo->nRecordSize, nSimID);
                if (ErrorCode != ERR_SUCCESS)
                {
                    hal_HstSendEvent(0xfa022202);
                    hal_HstSendEvent(nErrCode);
                    CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("SimReadRecordReq Failed in CFW_SimReadRecordProc\n", 0x08100cc5)));
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                    return nErrCode;
                }

                pReadSimInfo->n_PrevStatus = pReadSimInfo->n_CurrStatus;
                pReadSimInfo->n_CurrStatus = SIM_STATUS_READRECORD;
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)pSimElemFileStatusCnf->Sw1, (UINT8)pSimElemFileStatusCnf->Sw2, nSimID);
                hal_HstSendEvent(0xfa022203);
                hal_HstSendEvent(ErrorCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xf0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR(" SIM read ReadRecord Error CFW_SimReadRecordProc !\n", 0x08100cc6)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                return ErrorCode;
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case SIM_STATUS_READRECORD:
    {
        if (pEvent->nEventId == API_SIM_READRECORD_CNF)
        {
            api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("The sw1 is %d,    The sw2 is %d\n", 0x08100cc7), pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2);
            if (((0x90 == pSimReadRecordCnf->Sw1) && (00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cc8), __LINE__, __FILE__);
                UINT8 *pGetRecord = (UINT8 *)CSW_SIM_MALLOC(pReadSimInfo->nRecordSize + 1);
                if (pGetRecord == NULL)
                {
                    hal_HstSendEvent(0xfa022204);
                    hal_HstSendEvent(ERR_NO_MORE_MEMORY);
                    CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)ERR_NO_MORE_MEMORY,
                                        0, nUTI | (nSimID << 24), 0xf0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimReadMessage-->SIM_STATUS_READRECORD----> ERR_NO_MORE_MEMORY! \n", 0x08100cc9)));
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                    return ERR_NO_MORE_MEMORY;
                }
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, pSimReadRecordCnf->Data, 50, 16);

                SUL_MemCopy8(pGetRecord, pSimReadRecordCnf->Data, pReadSimInfo->nRecordSize);
                SUL_MemSet8(pGetRecord + pReadSimInfo->nRecordSize, 0xf0, 1);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("In CFW_SimReadRecordProc pGetRecord = %d\n", 0x08100cca), pGetRecord);
                hal_HstSendEvent(0xfa022205);
                hal_HstSendEvent((UINT32)pGetRecord);
                CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)pGetRecord, pReadSimInfo->nRecordSize, nUTI | (nSimID << 24), 0x00);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("post EV_CFW_SIM_READ_RECORD_RSP Success! \n", 0x08100ccb)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                return ERR_SUCCESS;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100ccc), __LINE__, __FILE__);
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)(pSimReadRecordCnf->Sw1), (UINT8)(pSimReadRecordCnf->Sw2), nSimID);
                hal_HstSendEvent(0xfa022206);
                hal_HstSendEvent(ErrorCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xf0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("SIM read ReadRecord Error! \n", 0x08100ccd)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                return ErrorCode;
            }
            //CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
    return ERR_SUCCESS;
}

UINT32 CFW_SimReadRecordWithLenProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId = 0;
    UINT32 nUTI = 0;
    UINT32 nErrCode = 0;
    VOID *nEvParam = (VOID *)NULL;
    SIM_INFO_READRECORD *pReadSimInfo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadRecordProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pReadSimInfo = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    switch (pReadSimInfo->n_CurrStatus)
    {
    case SIM_STATUS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pReadSimInfo->nTryCount >= 0x01)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cce), __LINE__, __FILE__);
                UINT32 ErrorCode = SimReadRecordReq(pReadSimInfo->nFileId, pReadSimInfo->nRecordNum, SIM_MODE_ABSOLUTE,
                                                    pReadSimInfo->nRecordSize, nSimID);
                if (ErrorCode != ERR_SUCCESS)
                {
                    hal_HstSendEvent(0xfa022207);
                    hal_HstSendEvent(nErrCode);
                    CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)nErrCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("SimReadRecordReq Failed in CFW_SimReadRecordProc\n", 0x08100ccf)));
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                    return nErrCode;
                }

                pReadSimInfo->n_PrevStatus = pReadSimInfo->n_CurrStatus;
                pReadSimInfo->n_CurrStatus = SIM_STATUS_READRECORD;
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;
    case SIM_STATUS_READRECORD:
    {
        if (pEvent->nEventId == API_SIM_READRECORD_CNF)
        {
            api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("The sw1 is %d,    The sw2 is %d\n", 0x08100cd0), pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2);

            if (((0x90 == pSimReadRecordCnf->Sw1) && (00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cd1), __LINE__, __FILE__);
                UINT8 *pGetRecord = (UINT8 *)CSW_SIM_MALLOC(pReadSimInfo->nRecordSize + 1);
                if (pGetRecord == NULL)
                {
                    hal_HstSendEvent(0xfa022208);
                    hal_HstSendEvent(ERR_NO_MORE_MEMORY);
                    CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)ERR_NO_MORE_MEMORY, 0,
                                        nUTI | (nSimID << 24), 0xf0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimReadMessage-->SIM_STATUS_READRECORD----> ERR_NO_MORE_MEMORY! \n", 0x08100cd2)));
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                    return ERR_NO_MORE_MEMORY;
                }

                SUL_MemCopy8(pGetRecord, pSimReadRecordCnf->Data, pReadSimInfo->nRecordSize);
                SUL_MemSet8(pGetRecord + pReadSimInfo->nRecordSize, 0xf0, 1);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("In CFW_SimReadRecordProc pGetRecord = %d\n", 0x08100cd3), pGetRecord);
                hal_HstSendEvent(0xfa022209);
                hal_HstSendEvent((UINT32)pGetRecord);
                CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)pGetRecord, pReadSimInfo->nRecordSize, nUTI | (nSimID << 24), 0x00);

                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("post EV_CFW_SIM_READ_RECORD_RSP Success! \n", 0x08100cd4)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                return ERR_SUCCESS;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cd5), __LINE__, __FILE__);
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)(pSimReadRecordCnf->Sw1), (UINT8)(pSimReadRecordCnf->Sw2), nSimID);
                hal_HstSendEvent(0xfa02220a);
                hal_HstSendEvent(ErrorCode);
                CFW_PostNotifyEvent(EV_CFW_SIM_READ_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xf0);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("SIM read ReadRecord Error! \n", 0x08100cd6)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
                return ErrorCode;
            }
            //CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecordProc);
    return ERR_SUCCESS;
}

/*
================================================================================
  Function   : CFW_SimUpdateRecordProc
--------------------------------------------------------------------------------

  Scope      :
  Parameters :
  Return     :
================================================================================
*/

UINT32 CFW_SimUpdateRecordProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId, nUTI;
    VOID *nEvParam = (VOID *)NULL;
    SIM_INFO_UPDATERECORD *pUpdateRecord;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimUpdateRecordProc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pUpdateRecord = CFW_GetAoUserData(hAo);
    CFW_GetUTI(hAo, &nUTI);

    switch (pUpdateRecord->n_CurrStatus)
    {
    case SIM_STATUS_IDLE:
    {
        if ((UINT32)pEvent == 0xFFFFFFFF)
        {
            if (pUpdateRecord->nTryCount >= 0x01)
            {
                //Add by lixp at 2007-12-06
                //Remove for At issue
                /*
                    if(( CFW_SIM_FILE_FDN   == pUpdateRecord->nFileId) ||
                       ( CFW_SIM_FILE_EXT2  == pUpdateRecord->nFileId) ||
                       ( CFW_SIM_FILE_BDN   == pUpdateRecord->nFileId) ||
                       ( CFW_SIM_FILE_EXT4  == pUpdateRecord->nFileId) )//Need pin2
                    {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(2)|TSMAP(2), TSTR("The line is %d,the file is %s\n",0x08100cd7),__LINE__,__FILE__);
                      nErrCode = SimElemFileStatusReq((UINT8)(pUpdateRecord->nFileId));
                      if(ERR_SUCCESS != nErrCode)
                      {
                          CFW_GetUTI(hAo, &nUTI);
                          CFW_PostNotifyEvent (EV_CFW_SIM_UPDATE_RECORD_RSP ,(UINT32)nErrCode, 0,nUTI, 0xF0);
                          CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SimElemFileStatusReq Failed in CFW_SimReadRecordProc\n",0x08100cd8)));
                          CFW_UnRegisterAO(CFW_SIM_SRV_ID,hAo);
                          hAo = HNULL;
                          CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecordProc );
                          return nErrCode;
                      }
                      pUpdateRecord->n_PrevStatus = pUpdateRecord->n_CurrStatus;
                      pUpdateRecord->n_CurrStatus = SIM_STATUS_ELEMFILE;
                      CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    }*/
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cd9), __LINE__, __FILE__);
                UINT32 result = SimUpdateRecordReq(pUpdateRecord->nFileId, pUpdateRecord->nRecordNum, pUpdateRecord->nMode,
                                                   pUpdateRecord->nRecordSize, pUpdateRecord->pData, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cda), __LINE__, __FILE__);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecordProc);
                }
                pUpdateRecord->n_PrevStatus = pUpdateRecord->n_CurrStatus;
                pUpdateRecord->n_CurrStatus = SIM_STATUS_UPDATERECORD;
            }
            else
            {
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
    }
    break;

    case SIM_STATUS_ELEMFILE:
    {
        if (pEvent->nEventId == API_SIM_ELEMFILESTATUS_CNF)
        {
            api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
            if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2)) || (0x91 == pSimElemFileStatusCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cdb), __LINE__, __FILE__);
                if (((pSimElemFileStatusCnf->Data[8]) & 0x0F) == 1)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cdc), __LINE__, __FILE__);
                    UINT32 result = SimUpdateRecordReq(pUpdateRecord->nFileId, pUpdateRecord->nRecordNum, pUpdateRecord->nMode,
                                                       pUpdateRecord->nRecordSize, pUpdateRecord->pData, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecordProc);
                    }
                    pUpdateRecord->n_PrevStatus = pUpdateRecord->n_CurrStatus;
                    pUpdateRecord->n_CurrStatus = SIM_STATUS_UPDATERECORD;
                }
                else if (((pSimElemFileStatusCnf->Data[8]) & 0x0F) == 2)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cdd), __LINE__, __FILE__);
                    //UINT32 result=SimVerifyCHVReq(pUpdateRecord->nPIN2,CHV2);
                    //Add by lixp at 2007-12-06
                    //Remove for At issue
                    /*
                        if(ERR_SUCCESS!=result)
                        {
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID,hAo);
                            hAo = HNULL;
                            CFW_PostNotifyEvent (EV_CFW_SIM_UPDATE_RECORD_RSP,(UINT32)result,0,nUTI, 0xF0);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecordProc);
                        }*/
                    pUpdateRecord->n_PrevStatus = pUpdateRecord->n_CurrStatus;
                    pUpdateRecord->n_CurrStatus = SIM_STATUS_VERIFY;
                }
                else
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cde), __LINE__, __FILE__);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    //CFW_PostNotifyEvent (EV_CFW_SIM_UPDATE_RECORD_RSP,(UINT32)11,0,nUTI, 0xF0);
                }
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100cdf), __LINE__, __FILE__);
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)pSimElemFileStatusCnf->Sw1, (UINT8)pSimElemFileStatusCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR("post EV_CFW_SIM_UPDATE_RECORD_RSP! \n", 0x08100ce0)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case SIM_STATUS_VERIFY:
    {
        if (pEvent->nEventId == API_SIM_VERIFYCHV_CNF)
        {
            api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
            if (((0x90 == pSimVerifyCHVCnf->Sw1) && (0x00 == pSimVerifyCHVCnf->Sw2)) || (0x91 == pSimVerifyCHVCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100ce1), __LINE__, __FILE__);
                UINT32 result = SimUpdateRecordReq(pUpdateRecord->nFileId, pUpdateRecord->nRecordNum, pUpdateRecord->nMode,
                                                   pUpdateRecord->nRecordSize, pUpdateRecord->pData, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }

                pUpdateRecord->n_PrevStatus = pUpdateRecord->n_CurrStatus;
                pUpdateRecord->n_CurrStatus = SIM_STATUS_UPDATERECORD;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100ce2), __LINE__, __FILE__);
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)pSimVerifyCHVCnf->Sw1, (UINT8)pSimVerifyCHVCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_UPDATE_RECORD_RSP failure!\n", 0x08100ce3)));
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    case SIM_STATUS_UPDATERECORD:
    {
        if (pEvent->nEventId == API_SIM_UPDATERECORD_CNF)
        {
            api_SimUpdateRecordCnf_t *pSimUpdateRecordCnf = (api_SimUpdateRecordCnf_t *)nEvParam;
            if (((0x90 == pSimUpdateRecordCnf->Sw1) && (0x00 == pSimUpdateRecordCnf->Sw2)) || (0x91 == pSimUpdateRecordCnf->Sw1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100ce4), __LINE__, __FILE__);
                CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)0, 0, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR("  post EV_CFW_SIM_UPDATE_RECORD_RSP Success! \n", 0x08100ce5)));
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100ce6), __LINE__, __FILE__);
                UINT32 ErrorCode = Sim_ParseSW1SW2((UINT8)pSimUpdateRecordCnf->Sw1, (UINT8)pSimUpdateRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_UPDATE_RECORD_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW | TLEVEL(33) | TDB | TNB_ARG(0), TSTXT(TSTR("  post EV_CFW_SIM_UPDATE_RECORD_RSP failure! \n", 0x08100ce7)));
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s \n", 0x08100ce8), __LINE__, __FILE__);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
    }
    break;

    default:
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        break;
    }

    CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecordProc);
    return ERR_SUCCESS;
}
#endif
