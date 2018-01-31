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
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <ts.h>
#include <sul.h>

#include "cfw_sim.h"
#include "csw_debug.h"
#include "api_msg.h"

PCSTR TS_GetSIMFileName(UINT32 nEvId);

extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT];
// b0  == FDN
// b1  == BDN
// b2  == CC
// 0: Not Support,1: Support

CFW_PROFILE nMEProfileSIM =
    {
        0x07,
        19,
        {0xFF,
         0xFF,
#ifdef CHIP_HAS_AP
         0x7F,
#else
         0xFF,
#endif
         0x7F,
         0x00,

         0x00,
         0x00,
         0x00,
         0x7F,
         0x00,

         0x00,
         0x00,
         0x23,
         0x09,
         0x09,

         0x06,
         0x03,
         0x00,
         0x0F}};

CFW_PROFILE nMEProfileUSIM =
    {
        0x07,
        33,
        {
            0xFF,
            0xFF,
#ifdef CHIP_HAS_AP
            0x7F,
#else
            0xFF,
#endif
            0x7F,
            0x00,

            0x00,
            0x00,
            0x00,
            0x51,
            0x00,

            0x00,
            0x00,
            0x33,
            0x09,
            0x09,

            0x06,
            0x03,
            0x00,
            0x0F,
            0x00,

            0x00,
            0x00,
            0x00,
            0x00,
            0x00,

            0x00, //six group
            0x00,
            0x00,
            0x00,
            0x00,

            0x00,
            0x00,
            0x00,
#if 0 //For testing
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF
#endif
        }};

UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID)
{
    if (g_cfw_sim_status[nSimID].UsimFlag)
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("STK ProfileDownload for USIM\n", 0x08100a29));
    else
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("STK ProfileDownload for SIM\n", 0x08100a2a));

    if (g_cfw_sim_status[nSimID].UsimFlag)
        *pMeProfile = &nMEProfileUSIM;
    else
        *pMeProfile = &nMEProfileSIM;
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=================================================");
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, (*pMeProfile)->pSat_ProfileData, (*pMeProfile)->nSat_ProfileLen, 16);
    return ERR_SUCCESS;
}

UINT32 CFW_SimSetSTKProfile(UINT8 *pProfile, UINT8 Length, CFW_SIM_ID nSimID)
{
    if (g_cfw_sim_status[nSimID].UsimFlag)
    {
        if (Length > 33)
            return ERR_CFW_INVALID_PARAMETER;
        nMEProfileUSIM.nSat_ProfileLen = Length;
        memcpy(nMEProfileUSIM.pSat_ProfileData, pProfile, Length);
    }
    else
    {
        if (Length > 19)
            return ERR_CFW_INVALID_PARAMETER;
        nMEProfileSIM.nSat_ProfileLen = Length;
        memcpy(nMEProfileSIM.pSat_ProfileData, pProfile, Length);
    }
    return ERR_SUCCESS;
}

//This function is obsolete, propose using  CFW_SimSetSTKProfile.
UINT32 CFW_SimSetMeProfile(UINT8 *pProfile, UINT8 Length)
{
    if (Length > 19)
        return ERR_CFW_INVALID_PARAMETER;
    nMEProfileSIM.nSat_ProfileLen = Length;
    SUL_MemCopy8(nMEProfileSIM.pSat_ProfileData, pProfile, Length);
    return ERR_SUCCESS;
}

UINT32 Sim_ParseSW1SW2(UINT8 SW1, UINT8 SW2, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Function:Sim_ParseSW1SW2\n", 0x08100a2b));

    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(Sim_ParseSW1SW2);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("SIM parse SW1(2) 0x%x,0x%x\n", 0x08100a2c), SW1, SW2);

    if ((SW1 == 0x92) && (SW2 == 0x40))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_MEMORY_FAILURE memory problem\n", 0x08100a2d));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_MEMORY_FAILURE;
    }
    else if ((SW1 == 0x93) && (SW2 == 0x00))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0),
                  TSTR("Error ERR_CME_SIM_STK_BUSY SIM Application Toolkit is busy. Command cannot be executed at present, further normal commands are allowed.\n", 0x08100a2e));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_STK_BUSY;
    }
    else if (SW1 == 0x94)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_NOEF_SELECTED no EF selected\n", 0x08100a2f));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NOEF_SELECTED;
        }
        else if (SW2 == 0x02)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_INVALID_INDEX out of range (invalid address)\n", 0x08100a30));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_INVALID_INDEX;
        }
        else if (SW2 == 0x04)
        {
            if (pG_Chv_Param->nFunID == SIM_SEEK)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_PATTERN_NOT_FOUNDpattern not found\n", 0x08100a31));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_PATTERN_NOT_FOUND;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_PATTERN_NOT_FOUND file ID not found\n", 0x08100a32));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_FILEID_NOT_FOUND;
            }
        }
        else if (SW2 == 0x08)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_FILE_UNMATCH_COMMAND file is inconsistent with the command\n", 0x08100a33));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_FILE_UNMATCH_COMMAND;
        }
    }
    else if (SW1 == 0x98)
    {
        if (SW2 == 0x02)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_CHV_UNINIT no CHV initialized\n", 0x08100a34));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_CHV_UNINIT; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x04)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error  	access condition not fulfilled \n", 0x08100a35));

            if (pG_Chv_Param->nFunID == SIM_UNBLOCKCHV)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNBLOCK_FAIL\n", 0x08100a36));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_UNBLOCK_FAIL;
            }
            else if (pG_Chv_Param->nFunID == SIM_VERIFYCHV)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_VERIFY_FAIL\n", 0x08100a37));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_VERIFY_FAIL;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_CONDITION_NO_FULLFILLED\n", 0x08100a38));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_CONDITION_NO_FULLFILLED;
            }
        }
        else if (SW2 == 0x08)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_CONTRADICTION_CHV in contradiction with CHV status\n", 0x08100a39));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_CONTRADICTION_CHV;
        }
        else if (SW2 == 0x10)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0),
                      TSTR("Error ERR_CME_SIM_CONTRADICTION_INVALIDATION in contradiction with invalidation status\n", 0x08100a3a));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_CONTRADICTION_INVALIDATION;
        }
        else if (SW2 == 0x40)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error  	unsuccessful CHV verification, no attempt left \n", 0x08100a3b));

            if (pG_Chv_Param->nFunID == SIM_UNBLOCKCHV)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT\n", 0x08100a3c));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);

                // pG_Chv_Param->nPin1Status = CHV_BLOCKED;
                return ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT;
            }
            else if (pG_Chv_Param->nFunID == SIM_VERIFYCHV)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_VERIFY_FAIL_NO_LEFT\n", 0x08100a3d));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_VERIFY_FAIL_NO_LEFT;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_CHV_NEED_UNBLOCK\n", 0x08100a3e));
                CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
                return ERR_CME_SIM_CHV_NEED_UNBLOCK;
            }
        }
        else if (SW2 == 0x50)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_MAXVALUE_REACHED increase cannot be performed, Max value reached\n", 0x08100a3f));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MAXVALUE_REACHED;
        }
        else if (SW2 == 0x62)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Authentication error, application specific", 0x08100a40));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_AUTH_ERROR_APPLICATION_SPECIFIC;
        }
        else if (SW2 == 0x63)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Security session or association expired", 0x08100a41));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SECURITY_SESSION_ASSOCIATION_EXPIRED;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a42));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }

    else if (SW1 == 0x62)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("No information given, state of non volatile memory unchanged", 0x08100a43));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x81)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Part of returned data may be corrupted", 0x08100a44));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_RETURN_DATA_CORRUPTED;
        }
        else if (SW2 == 0x82)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("End of file/record reached before reading Le bytes", 0x08100a45));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_LESS_DATA;
        }
        else if (SW2 == 0x83)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Selected file invalidated", 0x08100a46));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SELECT_FILE_INVALIDATED;
        }
        else if (SW2 == 0x85)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Selected file in termination state", 0x08100a47));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SELECT_FILE_INVALIDATED;
        }
        else if (SW2 == 0xf1)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("More data available", 0x08100a48));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MORE_DATA;
        }
        else if (SW2 == 0xf2)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("More data available and proactive command pending", 0x08100a49));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MORE_DATA_PROACTIVE_COMMAND_PENDING;
        }
        else if (SW2 == 0xf3)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Response data available", 0x08100a4a));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_RESPONSE_DATA_AVAILABLE;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a4b));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }
    else if (SW1 == 0x63)
    {
        if (SW2 == 0xf1)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("More data expected", 0x08100a4c));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MORE_DATA_EXPECTED;
        }
        else if (SW2 == 0xf2)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("More data expected and proactive command pending", 0x08100a4d));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MORE_DATA_EXPECTED_PROACTIVE_COMMAND_PENDING;
        }
        else if ((SW2 & 0xF0) == 0xC0)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Command successful but after using an internal update retry routine '%d' times Verification failed, '%d' retries remaining", 0x08100a4e),
                      SW2 & 0x0F, SW2 & 0x0F);
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_SUCCESS_VERIFICATION_FAIL;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a4f));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }

    else if (SW1 == 0x64)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("No information given, state of non volatile memory unchanged", 0x08100a50));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a51));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }
    else if (SW1 == 0x65)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("No information given, state of non volatile memory changed", 0x08100a52));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x81)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Memory problem", 0x08100a53));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_MEMORY_PROBLEM;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a54));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }
    else if (SW1 == 0x68)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("No information given", 0x08100a55));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x81)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Logical channel not supported", 0x08100a56));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_LOGIC_CHANNEL_NO_SUPPORTED;
        }
        else if (SW2 == 0x82)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Secure messaging not supported", 0x08100a57));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SECURE_MESSAGING_NO_SUPPORTED;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a58));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }
    else if (SW1 == 0x69)
    {
        if (SW2 == 0x00)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("No information given", 0x08100a59));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x81)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Command incompatible with file structure", 0x08100a5a));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_IMCOMPATIBLE;
        }
        else if (SW2 == 0x82)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Security status not satisfied", 0x08100a5b));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SECURITY_STATUS_NO_SATISFIED;
        }
        else if (SW2 == 0x83)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Authentication/PIN method blocked", 0x08100a5c));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_LOGIC_CHANNEL_NO_SUPPORTED;
        }
        else if (SW2 == 0x84)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Referenced data invalidated", 0x08100a5d));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_REFERENCED_DATA_INVALIDATED;
        }
        else if (SW2 == 0x85)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Conditions of use not satisfied", 0x08100a5e));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_USE_CONDITIONS_NOT_SATISFIED;
        }
        else if (SW2 == 0x86)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Command not allowed (no EF selected)", 0x08100a5f));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_NOT_ALLOWED;
        }
        else if (SW2 == 0x89)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Command not allowed - secure channel - security not satisfied", 0x08100a60));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_NOT_ALLOWED_SECURITY_NOT_SATISFIED;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a61));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }
    else if (SW1 == 0x6A)
    {
        if (SW2 == 0x80)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Incorrect parameters in the data field", 0x08100a62));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_NO_INFORMATION; // ERR_CME_SIM_PUK_REQUIRED;
        }
        else if (SW2 == 0x81)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Function not supported", 0x08100a63));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_IMCOMPATIBLE;
        }
        else if (SW2 == 0x82)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("File not found", 0x08100a64));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_SECURITY_STATUS_NO_SATISFIED;
        }
        else if (SW2 == 0x83)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Record not found", 0x08100a65));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_LOGIC_CHANNEL_NO_SUPPORTED;
        }
        else if (SW2 == 0x84)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Not enough memory space", 0x08100a66));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_REFERENCED_DATA_INVALIDATED;
        }
        else if (SW2 == 0x86)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Incorrect parameters P1 to P2", 0x08100a67));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_USE_CONDITIONS_NOT_SATISFIED;
        }
        else if (SW2 == 0x87)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Lc inconsistent with P1 to P2", 0x08100a68));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_NOT_ALLOWED;
        }
        else if (SW2 == 0x88)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Referenced data not found", 0x08100a69));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_COMMAND_NOT_ALLOWED_SECURITY_NOT_SATISFIED;
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW\n", 0x08100a6a));
            CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
            return ERR_CME_SIM_UNKNOW;
        }
    }

    else if (SW1 == 0X67)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_INVALID_PARAMETER incorrect parameter P3\n", 0x08100a6b));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_INVALID_PARAMETER;
    }
    else if (SW1 == 0X6B)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_INVALID_PARAMETER incorrect parameter P1 or P2\n", 0x08100a6c));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_INVALID_PARAMETER;
    }
    else if (SW1 == 0X6D)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_UNKNOW_COMMAND unknown instruction code given in the command\n", 0x08100a6d));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_UNKNOW_COMMAND;
    }
    else if (SW1 == 0X6E)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_WRONG_CLASS wrong instruction class given in the command\n", 0x08100a6e));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_WRONG_CLASS;
    }
    else if (SW1 == 0X6F)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error ERR_CME_SIM_TECHNICAL_PROBLEM technical problem with no diagnostic given\n", 0x08100a6f));
        CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
        return ERR_CME_SIM_TECHNICAL_PROBLEM;
    }
    else
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Error No process Sim_ParseSW1SW2 SW1 0x%x, SW2 0x%x\n", 0x08100a70), SW1, SW2);

        // TODO..
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Error Sim_ParseSW1SW2 SW1 0x%x, SW2 0x%x\n", 0x08100a71), SW1, SW2);

    CSW_PROFILE_FUNCTION_EXIT(Sim_ParseSW1SW2);
    return ERR_CME_SIM_UNKNOW;
}

UINT32 SimParseDedicatedStatus(UINT8 *pDedicatedData, CFW_SIM_ID nSimID)
{
    UINT8 nPin1Remain = 0;
    UINT8 nPuk1Remain = 0;
    UINT8 nPin2Remain = 0;
    UINT8 nPuk2Remain = 0;

    UINT8 *pData = pDedicatedData;

    SIM_CHV_PARAM *pG_Chv_Param;

    CSW_PROFILE_FUNCTION_ENTER(SimParseDedicatedStatus);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    // Update Remain number of CHV
    nPin1Remain = pData[18] & 0x0F;
    nPuk1Remain = pData[19] & 0x0F;
    nPin2Remain = pData[20] & 0x0F;
    nPuk2Remain = pData[21] & 0x0F;
    pG_Chv_Param->nPin1Remain = nPin1Remain;
    pG_Chv_Param->nPuk1Remain = nPuk1Remain;
    pG_Chv_Param->nPin2Remain = nPin2Remain;
    pG_Chv_Param->nPuk2Remain = nPuk2Remain;
    // Set the status of the PIN1
    if ((pData[18] & 0x80) == 0) // b8=0: secret code not initialised; b8=1: secret code initialised
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CHV Uninit\n", 0x08100a72));
        pG_Chv_Param->nPin1Status = CHV_UNINIT;
    }
    else if ((pData[13] & 0x80) == 0) // b8=0: CHV1 enabled; b8=1: CHV1 disabled
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CHV NOVERIFY\n", 0x08100a73));
        if (pG_Chv_Param->nPin1Status == CHV_UNKNOW)
            pG_Chv_Param->nPin1Status = CHV_NOVERIFY;
    }
    else
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CHV DISABLE\n", 0x08100a74));
        pG_Chv_Param->nPin1Status = CHV_DISABLE;
    }

    // Set the status of the PUK1
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pData[19]&0x80 Value %x\n", 0x08100a75), pData[19] & 0x80);

    if ((pData[19] & 0x80) == 0)
        pG_Chv_Param->nPuk1Status = CHV_UNINIT;
    else if (pG_Chv_Param->nPuk1Status == CHV_UNKNOW)
        pG_Chv_Param->nPuk1Status = CHV_INIT;

    // Set the status of the PIN2
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pData[20]&0x80 Value %x\n", 0x08100a76), pData[19] & 0x80);
    if ((pData[20] & 0x80) == 0)
        pG_Chv_Param->nPin2Status = CHV_UNINIT;
    else if (pG_Chv_Param->nPin2Status == CHV_UNKNOW)
        pG_Chv_Param->nPin2Status = CHV_NOVERIFY;

    // Set teh status of the PUK2
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("pData[21]&0x80 Value %x\n", 0x08100a77), pData[19] & 0x80);
    if ((pData[21] & 0x80) == 0)
        pG_Chv_Param->nPuk2Status = CHV_UNINIT;
    else if (pG_Chv_Param->nPuk2Status == CHV_UNKNOW)
        pG_Chv_Param->nPuk2Status = CHV_INIT;

    if (pG_Chv_Param->nPin1Status == CHV_UNINIT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CHV_UNINIT\n", 0x08100a78));
        pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE;
    }
    else if (pG_Chv_Param->nPin1Status == CHV_NOVERIFY)
    {
        if (pG_Chv_Param->nPuk1Status == CHV_UNINIT)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("nPin1Status: CHV_NOVERIFY, nPuk1Status: CHV_UNINIT\n", 0x08100a79));
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nPin1Remain: %d\n", 0x08100a7a), nPin1Remain);
            if (nPin1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK;
            else
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1; // need PIN1.
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nPin1Status: CHV_NOVERIFY, nPuk1Status: %d\n", 0x08100a7b), pG_Chv_Param->nPuk1Status);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nPin1Remain: %d\n", 0x08100a7c), nPuk1Remain);
            if (nPuk1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK; // PIN1 block.
            else if (nPin1Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Need input PIN1\n", 0x08100a7d));
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1; // need PIN1
            }
        }
    }
    else if (pG_Chv_Param->nPin1Status == CHV_VERIFY)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nPin1Status: CHV_VERIFY, nPuk1Remain: %d\n", 0x08100a7e), nPuk1Remain);
        if (nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK; // PIN1 block.
        else if (nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1READY; // PIN1 ready.
    }
    else if (pG_Chv_Param->nPin1Status == CHV_DISABLE)
    {
        // This process to report puk1 status after power on
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("nPin1Status: CHV_DISABLE, nPuk1Remain: %d\n", 0x08100a7f), nPuk1Remain);

        if (nPuk1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1BLOCK; // PIN1 block.
        else if (nPin1Remain == 0)
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK1; // need PUK1.
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("PIN1 Disable\n", 0x08100a80));
            pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN1DISABLE; // PIN1 disable.
        }
    }

    if (((pG_Chv_Param->nPin1Status == CHV_VERIFY) ||
         (pG_Chv_Param->nPin1Status == CHV_UNINIT) ||
         (pG_Chv_Param->nPin1Status == CHV_DISABLE)) &&
        (pG_Chv_Param->bCHV2VerifyReq == TRUE))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR("nPuk2Status: %d, nPin2Remain: %d nPuk2Remain: %d\n", 0x08100a81), pG_Chv_Param->nPuk2Status,
                  nPin2Remain, nPuk2Remain);

        if (pG_Chv_Param->nPuk2Status == CHV_UNINIT)
        {
            if (nPin2Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2BLOCK;
            else
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2; // need PIN2.
        }
        else
        {
            if (nPuk2Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2BLOCK; // PIN2 block.
            else if (nPin2Remain == 0)
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PUK2; // need PUK2.
            else
                pG_Chv_Param->nCHVAuthen = CFW_SIM_CARD_PIN2; // need PIN2
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(SimParseDedicatedStatus);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(4), TSTR("nPuk2Remain %d nPin2Remain %d nPuk1Remain %d nPin1Remain %d\n", 0x08100a82),
              nPuk2Remain, nPin2Remain, nPuk1Remain, nPin1Remain);

    return (((UINT16)nPuk2Remain << 12) | ((UINT16)nPin2Remain << 8) | (nPuk1Remain << 4) | nPin1Remain);
}

UINT32 SimResetReq(CFW_SIM_ID nSimID)
{
    api_SimResetReq_t *pSimResetReq = NULL;

    pSimResetReq = (api_SimResetReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimResetReq_t));
    if (pSimResetReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a83));
        CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
        return ERR_NO_MORE_MEMORY;
    }

    pSimResetReq->UsimFlag = sxs_UsimSupported();

    return CFW_SendSclMessage(API_SIM_RESET_REQ, pSimResetReq, nSimID);
}

#ifdef _DROPCARD_AUTOPIN_
UINT32 SimRetryReq(CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("API_SIM_RETRY_REQ\n", 0x08100a84));
    api_SimRetryReq_t *pSimRetryReq = NULL; //waiting for stack code

    pSimRetryReq = (api_SimRetryReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimPollReq_t));
    if (pSimRetryReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a85));
        CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
        return ERR_NO_MORE_MEMORY;
    }

    return CFW_SendSclMessage(API_SIM_RETRY_REQ, pSimRetryReq, nSimID); //waiting for stack code
    return 0;
}
#endif

UINT32 SimReadBinaryReq(UINT8 nFile, UINT16 nOffset, UINT8 nLen, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "ReadBinary nOffset %d nLen %d\n", nOffset, nLen);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(1), TSTXT(TSTR("SimReadBinaryReq nFile[%s] nSim[%d]\n", 0x08100a86)), TS_GetSIMFileName(nFile), nSimID);
    api_SimReadBinaryReq_t *pSimReadBinaryReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimReadBinaryReq);

    UINT32 nRet = CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

    if (nRet != ERR_SUCCESS)
    {
        return nRet;
    }

    pG_Chv_Param->nFunID = SIM_READBINARY;

    pSimReadBinaryReq = (api_SimReadBinaryReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimReadBinaryReq_t));
    if (pSimReadBinaryReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a87));
        CSW_PROFILE_FUNCTION_EXIT(SimReadBinaryReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimReadBinaryReq->File = nFile;
    pSimReadBinaryReq->Offset = nOffset;
    pSimReadBinaryReq->NbBytes = nLen;
    CSW_PROFILE_FUNCTION_EXIT(SimReadBinaryReq);
    return CFW_SendSclMessage(API_SIM_READBINARY_REQ, pSimReadBinaryReq, nSimID);
}

UINT32 SimUpdateBinaryReq(UINT8 nFile, UINT16 nOffset, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID)
{
    api_SimUpdateBinaryReq_t *pSimUpdateBinaryReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimUpdateBinaryReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_UPDATEBINARY;

    pSimUpdateBinaryReq = (api_SimUpdateBinaryReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimUpdateBinaryReq_t));
    if (pSimUpdateBinaryReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a88));

        CSW_PROFILE_FUNCTION_EXIT(SimUpdateBinaryReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimUpdateBinaryReq->NbBytes = nLen;
    pSimUpdateBinaryReq->Offset = nOffset;
    pSimUpdateBinaryReq->File = nFile;
    SUL_MemCopy8(pSimUpdateBinaryReq->Data, pData, nLen);
    CSW_PROFILE_FUNCTION_EXIT(SimUpdateBinaryReq);
    return CFW_SendSclMessage(API_SIM_UPDATEBINARY_REQ, pSimUpdateBinaryReq, nSimID);
}

UINT32 SimElemFileStatusReq(UINT8 nFile, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(1), TSTXT(TSTR("SimElemFileStatusReq nFile[%s] nSim[%d]\n", 0x08100a89)), TS_GetSIMFileName(nFile), nSimID);

    api_SimElemFileStatusReq_t *pSimElemFileStatusReq = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimElemFileStatusReq);

    pSimElemFileStatusReq = (api_SimElemFileStatusReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimElemFileStatusReq_t));
    if (pSimElemFileStatusReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a8a));

        CSW_PROFILE_FUNCTION_EXIT(SimElemFileStatusReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimElemFileStatusReq->File = nFile;
    CSW_PROFILE_FUNCTION_EXIT(SimElemFileStatusReq);
    return CFW_SendSclMessage(API_SIM_ELEMFILESTATUS_REQ, pSimElemFileStatusReq, nSimID);
}

UINT32 SimDedicFileStatusReq(UINT8 nFile, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Invoking SimDedicFileStatusReq\n", 0x08100a8b));

    api_SimDedicFileStatusReq_t *pSimDedicFileStatusReq = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimDedicFileStatusReq);
    pSimDedicFileStatusReq =
        (api_SimDedicFileStatusReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimDedicFileStatusReq_t));
    if (pSimDedicFileStatusReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a8c));

        CSW_PROFILE_FUNCTION_EXIT(SimDedicFileStatusReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimDedicFileStatusReq->File = nFile;
    CSW_PROFILE_FUNCTION_EXIT(SimDedicFileStatusReq);
    return CFW_SendSclMessage(API_SIM_DEDICFILESTATUS_REQ, pSimDedicFileStatusReq, nSimID);
}

UINT32 SimSelectApplicationReq(UINT8 *AID, UINT8 AIDLength,
                               CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Invoking SimApplicationSelectReq\n", 0x08100a8d));

    api_SimSelectApplicationReq_t *pSimSelectApplicationReq = NULL;

    pSimSelectApplicationReq = (api_SimSelectApplicationReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimSelectApplicationReq_t));
    if (pSimSelectApplicationReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a8e));

        return ERR_NO_MORE_MEMORY;
    }

    SUL_MemCopy8(pSimSelectApplicationReq->AID, AID, AIDLength);
    pSimSelectApplicationReq->AIDLength = AIDLength;

    return CFW_SendSclMessage(API_SIM_SELECTAPPLICATION_REQ, pSimSelectApplicationReq, nSimID);
}

UINT32 SimSetUsimPbk(UINT8 *Data, UINT8 DataLength, UINT8 RecordNum, UINT8 flag,
                     CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(0), TSTR("Invoking SimSetUsimPbk", 0x08100a8f));

    api_SimSetUsimPbkReq_t *pSimSetUsimPbkReq = NULL;

    pSimSetUsimPbkReq = (api_SimSetUsimPbkReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimSetUsimPbkReq_t));
    if (pSimSetUsimPbkReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a90));
        return ERR_NO_MORE_MEMORY;
    }

    SUL_MemCopy8(pSimSetUsimPbkReq->Data, Data, DataLength);
    pSimSetUsimPbkReq->DataLength = DataLength;
    pSimSetUsimPbkReq->RecordNum = RecordNum;
    pSimSetUsimPbkReq->flag = flag;

    return CFW_SendSclMessage(API_SIM_SETUSIMPBK_REQ, pSimSetUsimPbkReq, nSimID);
}

UINT32 SimReadRecordReq(UINT8 nFile, UINT8 nRecordNb, UINT8 nMode, UINT8 nLen, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(4) | TSMAP(1), TSTXT(TSTR("ReadRecordReq nFile[%s]nMode[%d]nRecordNb[%d]nLen[%d]\n", 0x08100a91)), TS_GetSIMFileName(nFile), nMode, nRecordNb,
              nLen);

    api_SimReadRecordReq_t *pSimReadRecordReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimReadRecordReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    if (pG_Chv_Param)
    {
        pG_Chv_Param->nFunID = SIM_READRECORD;

        pSimReadRecordReq = (api_SimReadRecordReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimReadRecordReq_t));
        if (pSimReadRecordReq == NULL)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a92));

            CSW_PROFILE_FUNCTION_EXIT(SimReadRecordReq);
            return ERR_NO_MORE_MEMORY;
        }
        pSimReadRecordReq->File = nFile;
        pSimReadRecordReq->Mode = nMode;
        pSimReadRecordReq->NbByte = nLen;
        pSimReadRecordReq->RecordNb = nRecordNb;
        CSW_PROFILE_FUNCTION_EXIT(SimReadRecordReq);

        return CFW_SendSclMessage(API_SIM_READRECORD_REQ, pSimReadRecordReq, nSimID);
    }
    else
    {
        CSW_PROFILE_FUNCTION_EXIT(SimReadRecordReq);
        return ERR_CME_SIM_WRONG;
    }
}

UINT32 SimUpdateRecordReq(UINT8 nFile, UINT8 nRecordNb, UINT8 nMode, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(4) | TSMAP(1), TSTXT(TSTR("UpdateRecordReq nFile[%s]nMode[%d]nRecordNb[%d]nLen[%d]\n", 0x08100a93)), TS_GetSIMFileName(nFile), nMode, nRecordNb,
              nLen);
    CSW_TC_MEMBLOCK(CFW_API_TS_ID, (UINT8 *)pData, nLen, 16);

    api_SimUpdateRecordReq_t *pSimUpdateRecordReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(SimUpdateRecordReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_UPDATERECORD;

    pSimUpdateRecordReq = (api_SimUpdateRecordReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimUpdateRecordReq_t));
    if (pSimUpdateRecordReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a94));

        CSW_PROFILE_FUNCTION_EXIT(SimUpdateRecordReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimUpdateRecordReq->File = nFile;
    pSimUpdateRecordReq->Mode = nMode;
    pSimUpdateRecordReq->NbByte = nLen;
    pSimUpdateRecordReq->RecordNb = nRecordNb;
    SUL_MemCopy8(pSimUpdateRecordReq->Data, pData, nLen);
    CSW_PROFILE_FUNCTION_EXIT(SimUpdateRecordReq);
    return CFW_SendSclMessage(API_SIM_UPDATERECORD_REQ, pSimUpdateRecordReq, nSimID);
}

UINT32 SimSeekReq(UINT8 nFile, UINT8 nTypeMode, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID)
{
    api_SimSeekReq_t *pSimSeekReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    UINT32 nRet = ERR_SUCCESS;

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3) | TSMAP(1), TSTXT(TSTR("SeekReq nFile[%s]nTypeMode[%d]nLen[%d]\n", 0x08100a95)), TS_GetSIMFileName(nFile), nTypeMode, nLen);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pData, nLen, 16);

    CSW_PROFILE_FUNCTION_ENTER(SimSeekReq);

    nRet = CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    if (nRet != ERR_SUCCESS)
    {
        return nRet;
    }
    pG_Chv_Param->nFunID = SIM_SEEK;

    pSimSeekReq = (api_SimSeekReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimSeekReq_t));
    if (pSimSeekReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a96));

        CSW_PROFILE_FUNCTION_EXIT(SimSeekReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimSeekReq->File = nFile;
    pSimSeekReq->TypeMode = nTypeMode;
    pSimSeekReq->PatternNbByte = nLen;
    SUL_MemCopy8(pSimSeekReq->Patern, pData, nLen);
    CSW_PROFILE_FUNCTION_EXIT(SimSeekReq);
    return CFW_SendSclMessage(API_SIM_SEEK_REQ, pSimSeekReq, nSimID);
}

UINT32 SimSeekAllReq(UINT8 nFile, UINT8 nTypeMode, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(3) | TSMAP(1), TSTXT(TSTR("SeekReq nFile[%s]nTypeMode[%d]nLen[%d]\n", 0x08100a95)), TS_GetSIMFileName(nFile), nTypeMode, nLen);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "SeekAllReq nFile[%s]nTypeMode[%d]nLen[%d]", TS_GetSIMFileName(nFile), nTypeMode, nLen);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pData, nLen, 16);
    CSW_PROFILE_FUNCTION_ENTER(SimSeekAllReq);

    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    UINT32 nRet = CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    if (nRet != ERR_SUCCESS)
    {
        return nRet;
    }
    pG_Chv_Param->nFunID = SIM_SEEK;

    api_SimSeekAllReq_t *pSimSeekAllReq = (api_SimSeekAllReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimSeekAllReq_t));
    if (pSimSeekAllReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a96));
        CSW_PROFILE_FUNCTION_EXIT(SimSeekAllReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimSeekAllReq->File = nFile;
    pSimSeekAllReq->TypeMode = nTypeMode;
    pSimSeekAllReq->PatternNbByte = nLen;
    SUL_MemCopy8(pSimSeekAllReq->Patern, pData, nLen);
    CSW_PROFILE_FUNCTION_EXIT(SimSeekAllReq);
    return CFW_SendSclMessage(API_SIM_SEEK_ALL_REQ, pSimSeekAllReq, nSimID);
}

UINT32 SimVerifyCHVReq(UINT8 *pCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("VerifyCHVReq nCHVNumber[%d]\n", 0x08100a97)), nCHVNumber);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pCHVValue, 8, 16);

    api_SimVerifyCHVReq_t *pSimVerifyCHVReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;

    CSW_PROFILE_FUNCTION_ENTER(SimVerifyCHVReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_VERIFYCHV;

    pSimVerifyCHVReq = (api_SimVerifyCHVReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimVerifyCHVReq_t));
    if (pSimVerifyCHVReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a98));

        CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimVerifyCHVReq->CHVNumber = nCHVNumber;
    SUL_MemCopy8(pSimVerifyCHVReq->CHVValue, pCHVValue, 8);
    CSW_PROFILE_FUNCTION_EXIT(SimVerifyCHVReq);
    return CFW_SendSclMessage(API_SIM_VERIFYCHV_REQ, pSimVerifyCHVReq, nSimID);
}

UINT32 SimChangeCHVReq(UINT8 *pOldCHVValue, UINT8 *pNewCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID)
{
    api_SimChangeCHVReq_t *pSimChangeCHVReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("ChangeCHVReq nCHVNumber[%d]\n", 0x08100a99)), nCHVNumber);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pOldCHVValue, 8, 16);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pNewCHVValue, 8, 16);

    CSW_PROFILE_FUNCTION_ENTER(SimChangeCHVReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_CHANGECHV;

    pSimChangeCHVReq = (api_SimChangeCHVReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimChangeCHVReq_t));
    if (pSimChangeCHVReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a9a));

        CSW_PROFILE_FUNCTION_EXIT(SimChangeCHVReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimChangeCHVReq->CHVNumber = nCHVNumber;
    SUL_MemCopy8(pSimChangeCHVReq->OldCHVValue, pOldCHVValue, 8);
    SUL_MemCopy8(pSimChangeCHVReq->NewCHVValue, pNewCHVValue, 8);
    CSW_PROFILE_FUNCTION_EXIT(SimChangeCHVReq);
    return CFW_SendSclMessage(API_SIM_CHANGECHV_REQ, pSimChangeCHVReq, nSimID);
}

UINT32 SimDisableCHVReq(UINT8 *pCHVValue, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(1), TSTXT(TSTR("SimDisableCHVReq pCHVValue[%s] nSim[%d]\n", 0x08100a9b)), pCHVValue, nSimID);

    api_SimDisableCHVReq_t *pSimDisableCHVReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pCHVValue, 8, 16);

    CSW_PROFILE_FUNCTION_ENTER(SimDisableCHVReq);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_DISABLECHV;

    pSimDisableCHVReq = (api_SimDisableCHVReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimDisableCHVReq_t));
    if (pSimDisableCHVReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a9c));

        CSW_PROFILE_FUNCTION_EXIT(SimDisableCHVReq);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_MemCopy8(pSimDisableCHVReq->CHVValue, pCHVValue, 8);
    CSW_PROFILE_FUNCTION_EXIT(SimDisableCHVReq);
    return CFW_SendSclMessage(API_SIM_DISABLECHV_REQ, pSimDisableCHVReq, nSimID);
}

UINT32 SimEnableCHVReq(UINT8 *pCHVValue, CFW_SIM_ID nSimID)
{
    api_SimEnableCHVReq_t *pSimEnableCHVReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;

    CSW_PROFILE_FUNCTION_ENTER(SimEnableCHVReq);

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pCHVValue, 8, 16);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_ENABLECHV;

    pSimEnableCHVReq = (api_SimEnableCHVReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimEnableCHVReq_t));
    if (pSimEnableCHVReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a9d));

        CSW_PROFILE_FUNCTION_EXIT(SimEnableCHVReq);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_MemCopy8(pSimEnableCHVReq->CHVValue, pCHVValue, 8);
    CSW_PROFILE_FUNCTION_EXIT(SimEnableCHVReq);
    return CFW_SendSclMessage(API_SIM_ENABLECHV_REQ, pSimEnableCHVReq, nSimID);
}

UINT32 SimUnblockCHVReq(UINT8 *pUnblockCHVValue, UINT8 *pNewCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID)
{
    api_SimUnblockCHVReq_t *pSimUnblockCHVReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;

    CSW_PROFILE_FUNCTION_ENTER(SimUnblockCHVReq);

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pUnblockCHVValue, 8, 16);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID | C_DETAIL, (UINT8 *)pNewCHVValue, 8, 16);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("UnblockCHVReq nCHVNumber[%d]\n", 0x08100a9e)), nCHVNumber);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    pG_Chv_Param->nFunID = SIM_UNBLOCKCHV;

    pSimUnblockCHVReq = (api_SimUnblockCHVReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimUnblockCHVReq_t));
    if (pSimUnblockCHVReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100a9f));

        CSW_PROFILE_FUNCTION_EXIT(SimUnblockCHVReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimUnblockCHVReq->CHVNumber = nCHVNumber;
    SUL_MemCopy8(pSimUnblockCHVReq->UnblockCHVValue, pUnblockCHVValue, 8);
    SUL_MemCopy8(pSimUnblockCHVReq->NewCHVValue, pNewCHVValue, 8);
    CSW_PROFILE_FUNCTION_EXIT(SimUnblockCHVReq);

    return CFW_SendSclMessage(API_SIM_UNBLOCKCHV_REQ, pSimUnblockCHVReq, nSimID);
}

UINT32 SimInvalidateReq(UINT8 nFile, CFW_SIM_ID nSimID)
{
    api_SimInvalidateReq_t *pSimInvalidateReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1) | TSMAP(1), TSTXT(TSTR("InvalidateReq nFile[%s]\n", 0x08100aa0)), TS_GetSIMFileName(nFile));

    CSW_PROFILE_FUNCTION_ENTER(SimInvalidateReq);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

    pG_Chv_Param->nFunID = SIM_INVALIDATE;

    pSimInvalidateReq = (api_SimInvalidateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimInvalidateReq_t));
    if (pSimInvalidateReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100aa1));

        CSW_PROFILE_FUNCTION_EXIT(SimInvalidateReq);
        return ERR_NO_MORE_MEMORY;
    }
    pSimInvalidateReq->File = nFile;
    CSW_PROFILE_FUNCTION_EXIT(SimInvalidateReq);

    return CFW_SendSclMessage(API_SIM_INVALIDATE_REQ, pSimInvalidateReq, nSimID);
}

UINT32 SimRehabilitateReq(UINT8 nFile, CFW_SIM_ID nSimID)
{
    api_SimRehabilitateReq_t *SimReHabilitateReq = NULL;
    SIM_CHV_PARAM *pG_Chv_Param = NULL;
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1) | TSMAP(1), TSTXT(TSTR("RehabilitateReq nFile[%s]\n", 0x08100aa2)), TS_GetSIMFileName(nFile));
    CSW_PROFILE_FUNCTION_ENTER(SimRehabilitateReq);

    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

    pG_Chv_Param->nFunID = SIM_REHABILITATE;

    SimReHabilitateReq = (api_SimRehabilitateReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimRehabilitateReq_t));
    if (SimReHabilitateReq == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100aa3));

        CSW_PROFILE_FUNCTION_EXIT(SimReHabilitateReq);
        return ERR_NO_MORE_MEMORY;
    }
    SimReHabilitateReq->File = nFile;
    CSW_PROFILE_FUNCTION_EXIT(SimRehabilitateReq);

    return CFW_SendSclMessage(API_SIM_REHABILITATE_REQ, SimReHabilitateReq, nSimID);
}

VOID cfw_IMSItoASC(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen)
{
    UINT8 i;

    CSW_PROFILE_FUNCTION_ENTER(cfw_IMSItoASC);

    OutPut[0] = ((InPut[1] & 0xF0) >> 4) + 0x30;
    for (i = 2; i < InPut[0] + 1; i++)
    {
        OutPut[2 * (i - 1) - 1] = (InPut[i] & 0x0F) + 0x30;
        OutPut[2 * (i - 1)] = ((InPut[i] & 0xF0) >> 4) + 0x30;
    }
    OutPut[2 * InPut[0] - 1] = 0x00;
    *OutLen = 2 * InPut[0] - 1;
    CSW_PROFILE_FUNCTION_EXIT(cfw_IMSItoASC);
}

VOID cfw_PLMNtoBCD(UINT8 *pIn, UINT8 *pOut, UINT8 *nInLength)
{
    UINT8 i = 0;
    UINT8 j = 0;
    UINT8 k = 0;
    CSW_PROFILE_FUNCTION_ENTER(cfw_PLMNtoBCD);

    for (i = 0; i < *nInLength; i += 3)
    {
        if (0xFF != pIn[i])
        {
            j++;
            pOut[2 * k] = pIn[i] & 0x0F;         // MCC digit 1
            pOut[2 * k + 1] = pIn[i] >> 4;       // MCC digit 2
            pOut[2 * k + 2] = pIn[i + 1] & 0x0F; // MNC digit 3
            pOut[2 * k + 5] = pIn[i + 1] >> 4;   // MCC digit 3
            pOut[2 * k + 3] = pIn[i + 2] & 0x0F; // MNC digit 2
            pOut[2 * k + 4] = pIn[i + 2] >> 4;   // MNC digit 1
            k += 3;
        }
        else if (0xFF == pIn[i])
        {
            j++;
            pOut[2 * k] = 0x0F;     // MCC digit 1
            pOut[2 * k + 1] = 0x0F; // MCC digit 2
            pOut[2 * k + 2] = 0x0F; // MNC digit 3
            pOut[2 * k + 5] = 0x0F; // MCC digit 3
            pOut[2 * k + 3] = 0x0F; // MNC digit 2
            pOut[2 * k + 4] = 0x0F; // MNC digit 1
            k += 3;
        }
    }
    *nInLength = j;

    CSW_PROFILE_FUNCTION_EXIT(cfw_PLMNtoBCD);
}

VOID cfw_BCDtoPLMN(UINT8 *pIn, UINT8 *pOut, UINT8 nInLength)
{
    UINT8 i;

    CSW_PROFILE_FUNCTION_ENTER(cfw_BCDtoPLMN);

    for (i = 0; i < nInLength / 2; i++)
    {
        if (i % 3 == 0)
        {
            pOut[i] = (UINT8)(pIn[2 * i] & 0x0F) + (UINT8)((pIn[2 * i + 1] & 0x0F) << 4);
        }
        else if (i % 3 == 1)
        {
            pOut[i] = (UINT8)(pIn[2 * i] & 0x0F) + (UINT8)((pIn[2 * i + 3] & 0x0F) << 4);
        }
        else
        {
            pOut[i] = (UINT8)(pIn[2 * i - 1] & 0x0F) + (UINT8)(((pIn[2 * i] & 0x0F) << 4));
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(cfw_BCDtoPLMN);
}

CFW_SIM_ID CFW_GetSimCardID(HAO hAO)
{
    CFW_SIM_ID nSimID;

    UINT32 nRet = CFW_GetSimID(hAO, &nSimID);

    if (ERR_SUCCESS != nRet)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_GetSimID return 0x%x \n", 0x08100aa4), nRet);

        COS_SetLastError(nRet);
    }

    return nSimID;
}

/*
================================================================================
  Function   : CFW_PLMNtoBCDEX
--------------------------------------------------------------------------------

  Scope      : Change PLMN to BCD
  Parameters :
  Return     :
================================================================================
*/
VOID CFW_PLMNtoBCDEX(UINT8 *pIn, UINT8 *pOut, UINT8 *nInLength)
{
    UINT8 i = 0x00;
    UINT8 j = 0x00;

    CSW_PROFILE_FUNCTION_ENTER(cfw_PLMNtoBCD);

    for (i = 0; i < *nInLength; i += 3)
    {
        j++;
        if (0xFF != pIn[i])
        {
            pOut[2 * i] = pIn[i] & 0x0F;         // MCC digit 1
            pOut[2 * i + 1] = pIn[i] >> 4;       // MCC digit 2
            pOut[2 * i + 2] = pIn[i + 1] & 0x0F; // MNC digit 3
            pOut[2 * i + 5] = pIn[i + 1] >> 4;   // MCC digit 3
            pOut[2 * i + 3] = pIn[i + 2] & 0x0F; // MNC digit 2
            pOut[2 * i + 4] = pIn[i + 2] >> 4;   // MNC digit 1
        }
        else
        {
            pOut[2 * i] = 0xff;     // MCC digit 1
            pOut[2 * i + 1] = 0xff; // MCC digit 2
            pOut[2 * i + 2] = 0xff; // MNC digit 3
            pOut[2 * i + 5] = 0xff; // MCC digit 3
            pOut[2 * i + 3] = 0xff; // MNC digit 2
            pOut[2 * i + 4] = 0xff; // MNC digit 1
        }
    }
    *nInLength = j;

    CSW_PROFILE_FUNCTION_EXIT(cfw_PLMNtoBCD);
}
