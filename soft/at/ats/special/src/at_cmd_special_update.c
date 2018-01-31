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

#ifdef AT_FOTA_SUPPORT

#include "at_common.h"
#include "at_module.h"
#include "at_cmd_engine.h"
#include "at_utils.h"
#include "fota/fota.h"
#include "fs/sf_api.h"
#include "app_http.h"
#include "http_api.h"

#define FOTA_PACKAGE_MAX_SIZE (0x50000)

static unsigned gFotaSize;
static unsigned gFotaGotSize;
static uint8_t *gFotaData;
static FOTA_CONTEXT_T *gFotaCtx;
static unsigned char *gUpgradeData;
extern AT_CMD_PARA *gHttpEngine;
static UINT32 offset = 0;
static BOOL mal_flag = FALSE;

static bool at_FotaWriteData(const void *data, unsigned size)
{
    fotaInvalidate(gFotaCtx);
    sf_file fh = sf_open(FOTA_PACK_DEFAULT_FILENAME, SF_RDWR | SF_CREAT | SF_TRUNC);
    if (fh < 0)
        return false;

    sf_write(fh, data, size);
    sf_close(fh);

    return fotaDownloadFinished(gFotaCtx);
}

static void process_FotaUpgradeData(const unsigned char *data, size_t len)
{
    AT_CMD_ENGINE_T *engine = (AT_CMD_ENGINE_T *)gHttpEngine->engine;
    ATLOGI("AT FOTA received len=%d", len);

    if (mal_flag == FALSE)
    {
        gUpgradeData = (unsigned char *)malloc(gFotaSize);
        mal_flag = TRUE;
    }
    int i = 0;
    for (i = 0; i < len; i++)
    {
        gUpgradeData[i + offset] = data[i];
    }
    offset += len;

    if (strcmp(data,"EOF") == 0)
    {
        FOTA_ENV_T fenv = {
            .packFname = FOTA_PACK_DEFAULT_FILENAME,
            .idxFname = FOTA_INDEX_DEFAULT_FILENAME,
        };

        //fota init
        gFotaCtx = (FOTA_CONTEXT_T *)at_malloc(fotaContextSize());
        if (gFotaCtx == NULL)
            return;
        if (!fotaInit(gFotaCtx, &fenv))
            return;
        //fota invalidate
        fotaInvalidate(gFotaCtx);
        //fota write pack data

        bool result = at_FotaWriteData(gUpgradeData, offset);
        ATLOGI("AT FOTA write result=%d", result);

        at_free(gFotaCtx);
        if (mal_flag == TRUE)
        {
            mal_flag = FALSE;
            free(gUpgradeData);
        }
        at_CmdSetLineMode(engine);
        if (result)
        {
            at_CmdRespOK(engine);
            hal_SysRestart();
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
        offset = 0;
    }
}

static void fota_memory_Free_Http(Http_info *http_info)
{
    cg_net_uri_delete((http_info->cg_http_api)->url);
    cg_http_request_delete((http_info->cg_http_api)->g_httpReq);
    free(http_info->cg_http_api);
    free(http_info);
}

static int at_FotaReceiveData(void *param, const uint8_t *data, unsigned length)
{
    AT_CMD_ENGINE_T *engine = (AT_CMD_ENGINE_T *)param;
    if (length == 0)
        return 0;

    if (length + gFotaGotSize > gFotaSize)
        length = gFotaSize - gFotaGotSize;

    memcpy(gFotaData + gFotaGotSize, data, length);
    gFotaGotSize += length;

    if (gFotaGotSize >= gFotaSize)
    {
        bool result = at_FotaWriteData(gFotaData, gFotaSize);

        at_free(gFotaData);
        at_free(gFotaCtx);
        at_CmdSetLineMode(engine);

        if (result)
        {
            at_CmdRespOK(engine);
            hal_SysRestart();
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    return length;
}

static BOOL at_FotaInit(UINT32 size)
{
    FOTA_ENV_T fenv = {
        .packFname = FOTA_PACK_DEFAULT_FILENAME,
        .idxFname = FOTA_INDEX_DEFAULT_FILENAME,
    };

    gFotaSize = size;
    gFotaGotSize = 0;
    if (size > FOTA_PACKAGE_MAX_SIZE)
        return FALSE;

    gFotaData = at_malloc(size);
    if (gFotaData == NULL)
        goto failed;

    gFotaCtx = (FOTA_CONTEXT_T *)at_malloc(fotaContextSize());
    if (gFotaCtx == NULL)
        goto failed_1;

    if (!fotaInit(gFotaCtx, &fenv))
        goto failed_2;

    fotaInvalidate(gFotaCtx);
    return TRUE;

failed_2:
    at_free(gFotaCtx);
failed_1:
    at_free(gFotaData);
failed:
    return FALSE;
}

VOID AT_FOTA_CmdFunc_UPDATE(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint32_t size = at_ParamUint(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_FotaInit(size))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        at_CmdSetBypassMode(pParam->engine, at_FotaReceiveData, pParam->engine);
        // Command not finished
    }
    else
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
    }
}

//upgrade fota by http
VOID AT_FOTA_CmdFunc_UPGRADE(AT_CMD_PARA *pParam)
{
    UINT8 uParamCount = 0;
    INT32 iResult = 0;
    UINT8 fotasize[255] = {0x00};
    UINT16 uSize = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 url[255] = {0x00};
    UINT8 tmpString[60] = {0x00};
    CgHttpApi *cg_http_api;
    Http_info *http_info1;
    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
    gHttpEngine = pParam;
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 2))
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        uSize = 255;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &url, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "HTTPDOWNLOAD download URL error");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &fotasize, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "gFotaSize error");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        gFotaSize = atoi(fotasize);
        AT_TC(g_sw_GPRS, "gFotaSize : %d", gFotaSize);
        AT_TC(g_sw_GPRS, "HTTPDOWNLOAD download URL : %s", url);
        http_info1 = (Http_info *)malloc(sizeof(Http_info));
        cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
        cg_http_api->url = cg_net_uri_new();
        cg_http_api->g_httpReq = cg_http_request_new();
        if (cg_http_api == NULL || http_info1 == NULL)
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        else
        {
            http_info1->url = url;
            http_info1->cg_http_api = cg_http_api;
            if ((app_WPHTTPDownLoad(http_info1, process_FotaUpgradeData, fota_memory_Free_Http) == TRUE))
            {
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {
                at_CmdRespInfoText(pParam->engine, "Task is already running, please wait");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+UPGRADE:<url>");
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI);
        break;

    default:
        AT_TC(g_sw_GPRS, "FOTA UPGRADE ERROR");
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        break;
    }
}

#endif
