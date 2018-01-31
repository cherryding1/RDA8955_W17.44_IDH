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
#include <ts.h>
#include <sul.h>
#include <cos.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_chv.h"

UINT32 CFW_SimGetAuthenticationStatus(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hSimGetAuthAo = 0;

    CFW_GETAUTH *pSimGetAuthData = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetAuthenticationStatus);

    UINT32 ret = ERR_SUCCESS;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100b97), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenticationStatus);
        return ret;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100b98));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenticationStatus);
        return ERR_CFW_UTI_EXIST;
    }
#endif
    pSimGetAuthData = (CFW_GETAUTH *)CSW_SIM_MALLOC(SIZEOF(CFW_GETAUTH));
    if (pSimGetAuthData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100b99));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenticationStatus);
        return ERR_NO_MORE_MEMORY;
    }
    hSimGetAuthAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimGetAuthData, CFW_SimGetAuthenStatusProc, nSimID);
    CFW_SetUTI(hSimGetAuthAo, nUTI, 0);

    pSimGetAuthData->nSm_GetAuth_prevStatus = SM_GETAUTH_INIT;
    pSimGetAuthData->nSm_GetAuth_currStatus = SM_GETAUTH_IDLE;
    CFW_SetAoProcCode(hSimGetAuthAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetAuthenticationStatus);
    return (ERR_SUCCESS);
}

UINT8 gTempPin[12] = {1,2,3,4,};
UINT32 CFW_SimSetTempPin(UINT8 *pPin,UINT8 nPinSize)
{
    UINT8 i = 0;
    for(i = 0; i < nPinSize; i++)
    {
        if((pPin[i] < '0') || (pPin[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        gTempPin[i] = pPin[i];
    }
    return nPinSize;
}
#ifdef _DROPCARD_AUTOPIN_

UINT32 CFW_SimEnterAuthenticationEX(UINT8 *pPin,
                                    UINT8 nPinSize,
                                    UINT8 *pNewPin, UINT8 nNewPinSize, UINT8 nOption, UINT16 nUTI, CFW_SIM_ID nSimID)
{

    UINT8 i             = 0;
    HAO hSimEnterAuthAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimEnterAuthentication);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Invoking CFW_SimEnterAuthentication \n",0x08100b9a));

    UINT32 ret = ERR_SUCCESS;
    //CFW_SimSetTempPin(pPin,nPinSize);

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100b9b), ret);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ret;
    }

    CFW_ENTERAUTH *pSimEnterAuthData = NULL;
    if(pNewPin == NULL)
        nNewPinSize = 0;
    else if(nNewPinSize > 8)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("Error Parameter pPin %x,nPinSize %d,nNewPinSize %d\n",0x08100b9c), pPin, nPinSize, nNewPinSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((pPin == NULL) || (nPinSize > 8))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("Error Parameter pPin %x,nPinSize %d,nNewPinSize %d\n",0x08100b9d), pPin, nPinSize, nNewPinSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }


    if (nOption > 2)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nOption 0x%x \n",0x08100b9e), nOption);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100b9f));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_UTI_EXIST;
    }
#endif
    pSimEnterAuthData = (CFW_ENTERAUTH *)CSW_SIM_MALLOC(SIZEOF(CFW_ENTERAUTH));
    if (pSimEnterAuthData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ba0));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_NO_MORE_MEMORY;
    }
    pSimEnterAuthData->nOption = nOption;
    for(i = 0; i < nPinSize; i++)
    {
        if((pPin[i] < '0') || (pPin[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        pSimEnterAuthData->pPinPwd[i] = pPin[i];
    }

    for (i = nPinSize; i < 8; i++)
        pSimEnterAuthData->pPinPwd[i] = 0xFF;

    if(nNewPinSize != 0)
    {
        for(i = 0; i < nNewPinSize; i++)
        {
            if((pNewPin[i] < '0') || (pNewPin[i] > '9'))
                return ERR_CFW_INVALID_PARAMETER;
            pSimEnterAuthData->pNewPwd[i] = pNewPin[i];
        }

        for (i = nNewPinSize; i < 8; i++)
            pSimEnterAuthData->pNewPwd[i] = 0xFF;
    }
    else
        pSimEnterAuthData->pNewPwd[0] = 0xFF;

    hSimEnterAuthAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimEnterAuthData, CFW_SimEnterAuthenticationProc, nSimID);
    CFW_SetSpecialAo(hSimEnterAuthAo);
    CFW_SetUTI(hSimEnterAuthAo, nUTI, 0);

    pSimEnterAuthData->nSm_EntAuth_prevStatus = SM_ENTAUTH_INIT;
    pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_IDLE;
    CFW_SetAoProcCode(hSimEnterAuthAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);

    return (ERR_SUCCESS);
}
#endif
UINT32 CFW_SimEnterAuthentication(UINT8 *pPin,
                                  UINT8 nPinSize,
                                  UINT8 *pNewPin, UINT8 nNewPinSize, UINT8 nOption, UINT16 nUTI, CFW_SIM_ID nSimID)
{

    UINT8 i             = 0;
    HAO hSimEnterAuthAo = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimEnterAuthentication);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Invoking CFW_SimEnterAuthentication \n",0x08100ba1));
    hal_HstSendEvent(0xfa061401);
    hal_HstSendEvent(csw_TMGetTick());
    UINT32 ret = ERR_SUCCESS;
//  CFW_SimSetTempPin(pPin,nPinSize);

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100ba2), ret);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ret;
    }

    CFW_ENTERAUTH *pSimEnterAuthData = NULL;
    if(pNewPin == NULL)
        nNewPinSize = 0;
    else if(nNewPinSize > 8)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("Error Parameter pPin %x,nPinSize %d,nNewPinSize %d\n",0x08100ba3), pPin, nPinSize, nNewPinSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((pPin == NULL) || (nPinSize > 8))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("Error Parameter pPin %x,nPinSize %d,nNewPinSize %d\n",0x08100ba4), pPin, nPinSize, nNewPinSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }


    if (nOption > 2)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nOption 0x%x \n",0x08100ba5), nOption);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100ba6));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_CFW_UTI_EXIST;
    }
#endif
    pSimEnterAuthData = (CFW_ENTERAUTH *)CSW_SIM_MALLOC(SIZEOF(CFW_ENTERAUTH));
    if (pSimEnterAuthData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ba7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);
        return ERR_NO_MORE_MEMORY;
    }
    pSimEnterAuthData->nOption = nOption;
    for(i = 0; i < nPinSize; i++)
    {
        if((pPin[i] < '0') || (pPin[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        pSimEnterAuthData->pPinPwd[i] = pPin[i];
    }

    for (i = nPinSize; i < 8; i++)
        pSimEnterAuthData->pPinPwd[i] = 0xFF;

    if(nNewPinSize != 0)
    {
        for(i = 0; i < nNewPinSize; i++)
        {
            if((pNewPin[i] < '0') || (pNewPin[i] > '9'))
                return ERR_CFW_INVALID_PARAMETER;
            pSimEnterAuthData->pNewPwd[i] = pNewPin[i];
        }

        for (i = nNewPinSize; i < 8; i++)
            pSimEnterAuthData->pNewPwd[i] = 0xFF;
    }
    else
        pSimEnterAuthData->pNewPwd[0] = 0xFF;

    hSimEnterAuthAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimEnterAuthData, CFW_SimEnterAuthenticationProc, nSimID);
    CFW_SetUTI(hSimEnterAuthAo, nUTI, 0);

    pSimEnterAuthData->nSm_EntAuth_prevStatus = SM_ENTAUTH_INIT;
    pSimEnterAuthData->nSm_EntAuth_currStatus = SM_ENTAUTH_IDLE;
    CFW_SetAoProcCode(hSimEnterAuthAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimEnterAuthentication);

    return (ERR_SUCCESS);
}

UINT32 CFW_SimChangePassword(UINT16 nFac,
                             UINT8 *pOldPwd,
                             UINT8 nOldPwdSize, UINT8 *pNewPwd, UINT8 nNewPwdSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    UINT8 i = 0;
    HAO hSimChangePwAo;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimChangePassword);

    UINT32 ret = ERR_SUCCESS;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100ba8), ret);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);
        return ret;
    }

    CFW_CHANGEPW *pSimChangePwData = NULL;

    if ((pOldPwd == NULL) || (pNewPwd == NULL) ||
            (nOldPwdSize > 8) || (nNewPwdSize > 8))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100ba9));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);

        return ERR_CFW_UTI_EXIST;
    }

    pSimChangePwData = (CFW_CHANGEPW *)CSW_SIM_MALLOC(SIZEOF(CFW_CHANGEPW));
    if (pSimChangePwData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100baa));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);
        return ERR_NO_MORE_MEMORY;
    }
    pSimChangePwData->nFac = nFac;
    for(i = 0; i < nOldPwdSize; i++)
    {
        if((pOldPwd[i] < '0') || (pOldPwd[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        pSimChangePwData->pOldPwd[i] = pOldPwd[i];
    }
    for (i = nOldPwdSize; i < 8; i++)
        pSimChangePwData->pOldPwd[i] = 0xFF;

    for(i = 0; i < nNewPwdSize; i++)
    {
        if((pNewPwd[i] < '0') || (pNewPwd[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        pSimChangePwData->pNewPwd[i] = pNewPwd[i];
    }

    for (i = nNewPwdSize; i < 8; i++)
        pSimChangePwData->pNewPwd[i] = 0xFF;

    if (nFac == CFW_STY_FAC_TYPE_SC)
        pSimChangePwData->nCHVNumber = CHV1;
    else if (nFac == CFW_STY_FAC_TYPE_P2)
        pSimChangePwData->nCHVNumber = CHV2;
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nFac value 0x%x\n",0x08100bab), nFac);
        CSW_SIM_FREE(pSimChangePwData);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);
        return ERR_CFW_INVALID_PARAMETER;
    }

    hSimChangePwAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimChangePwData, CFW_SimChangePasswordProc, nSimID);
    CFW_SetUTI(hSimChangePwAo, nUTI, 0);

    pSimChangePwData->nSm_ChgPw_prevStatus = SM_CHGPW_INIT;
    pSimChangePwData->nSm_ChgPw_currStatus = SM_CHGPW_IDLE;
    CFW_SetAoProcCode(hSimChangePwAo, CFW_AO_PROC_CODE_CONTINUE); // state change to read state.
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimChangePassword);

    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetFacilityLock(UINT16 nFac, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hSimGetFacilityAo;

    CFW_GETFACILITY *pSimGetFAcilityData;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetFacilityLock);

    UINT32 ret = ERR_SUCCESS;

    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100bac), ret);

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityLock);
        return ret;
    }

    if ((nFac != CFW_STY_FAC_TYPE_SC) && (nFac != CFW_STY_FAC_TYPE_FD))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nFac value 0x%x\n",0x08100bad), nFac);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100bae));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityLock);
        return ERR_CFW_UTI_EXIST;
    }
#endif
    pSimGetFAcilityData = (CFW_GETFACILITY *)CSW_SIM_MALLOC(SIZEOF(CFW_GETFACILITY));
    if (pSimGetFAcilityData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100baf));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityLock);
        return ERR_NO_MORE_MEMORY;
    }
    pSimGetFAcilityData->nFac = nFac;

    hSimGetFacilityAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimGetFAcilityData, CFW_SimGetFacilityProc, nSimID);
    CFW_SetUTI(hSimGetFacilityAo, nUTI, 0);

    pSimGetFAcilityData->nSm_GetFac_prevStatus = SM_GETFAC_INIT;
    pSimGetFAcilityData->nSm_GetFac_currStatus = SM_GETFAC_IDLE;
    CFW_SetAoProcCode(hSimGetFacilityAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFacilityLock);

    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetFacilityLock(UINT16 nFac, UINT8 *pBufPwd, UINT8 nPwdSize, UINT8 nMode, // 0 for unlock(=Disable); 1 for lock(=Eanble)
                              UINT16 nUTI, CFW_SIM_ID nSimID)
{
    UINT8 i = 0;
    HAO hSimSetFacilityAo;

    CFW_SETFACILITY *pSimSetFAcilityData = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetFacilityLock);

    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100bb0), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityLock);
        return ret;
    }

    if ((nFac != CFW_STY_FAC_TYPE_SC) && (nFac != CFW_STY_FAC_TYPE_FD))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nFac value 0x%x\n",0x08100bb1), nFac);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((nMode > 1) || (nPwdSize > 8))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("Error Parameter nMode %x,nPwdSize %d,nPwdSize %d\n",0x08100bb2), nMode, nPwdSize, nPwdSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityLock);
        return ERR_CFW_INVALID_PARAMETER;
    }
    pSimSetFAcilityData = (CFW_SETFACILITY *)CSW_SIM_MALLOC(SIZEOF(CFW_SETFACILITY));
    if (pSimSetFAcilityData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100bb3));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityLock);
        return ERR_NO_MORE_MEMORY;
    }
    pSimSetFAcilityData->nFac     = nFac;
    pSimSetFAcilityData->nMode    = nMode;
    pSimSetFAcilityData->nPwdSize = nPwdSize;
    for(i = 0; i < nPwdSize; i++)
    {
        if((pBufPwd[i] < '0') || (pBufPwd[i] > '9'))
            return ERR_CFW_INVALID_PARAMETER;
        pSimSetFAcilityData->pBufPwd[i] = pBufPwd[i];
    }

    for (i = nPwdSize; i < 8; i++)
        pSimSetFAcilityData->pBufPwd[i] = 0xFF;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("   pBufPwd  = 0x%x,%x,%x,%x \n",0x08100bb4), pBufPwd[0], pBufPwd[1], pBufPwd[2], pBufPwd[3]);
    hSimSetFacilityAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimSetFAcilityData, CFW_SimSetFacilityProc, nSimID);
    CFW_SetUTI(hSimSetFacilityAo, nUTI, 0);

    pSimSetFAcilityData->nSm_SetFac_prevStatus = SM_SETFAC_INIT;
    pSimSetFAcilityData->nSm_SetFac_currStatus = SM_SETFAC_IDLE;
    CFW_SetAoProcCode(hSimSetFacilityAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("OK:CFW_SimSetFacilityLock\n",0x08100bb5));
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetFacilityLock);
    return (ERR_SUCCESS);
}

