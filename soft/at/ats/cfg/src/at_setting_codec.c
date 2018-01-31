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

#include <at_cfg.h>
#include <at_utils.h>

#include <pb_decode.h>
#include <pb_encode.h>
#include <pb_util.h>
#include <stdint.h>

#include "at_setting_codec.h"
#include "at_setting.pb.h"

#define AT_PB_BUF_SIZE_MAX 2048

#define PB_CALLBACK(a) // placer holder for be handled by callback
#define PB_ASSIGN(a) (pbAssignLeft->a = pbAssignRight->a)

#define AT_ASSIGN()                              \
    do                                           \
    {                                            \
        PB_ASSIGN(resultCodePresent);            \
        PB_ASSIGN(resultCodeFormat);             \
        PB_ASSIGN(commandEchoMode);              \
        PB_ASSIGN(nS3);                          \
        PB_ASSIGN(nS4);                          \
        PB_ASSIGN(nS5);                          \
        PB_ASSIGN(cmee);                         \
        PB_ASSIGN(cgreg);                        \
        PB_ASSIGN(crc);                          \
        PB_ASSIGN(rxfc);                         \
        PB_ASSIGN(txfc);                         \
        PB_ASSIGN(icfFormat);                    \
        PB_ASSIGN(icfParity);                    \
        PB_ASSIGN(dtrData);                      \
        PB_ASSIGN(secUartEnable);               \
        PB_ASSIGN(secUartBaud);                  \
        PB_ASSIGN(secUartIcfFormat);             \
        PB_ASSIGN(secUartIcfParity);             \
        PB_ASSIGN(cfgInfo.ind_ctrl_mask);        \
        PB_ASSIGN(cfgInfo.sind_ctrl_mask);       \
        PB_ASSIGN(cfgInfo.te_chset);             \
        PB_ASSIGN(cfgInfo.mt_chset);             \
        PB_ASSIGN(cc_VTD);                       \
        PB_ASSIGN(cc_s0);                        \
        PB_ASSIGN(gprsAuto);                     \
        PB_ASSIGN(baudRate);                     \
        PB_CALLBACK(operFormat);                 \
        PB_ASSIGN(copsModeTemp);                 \
        PB_ASSIGN(preferredOperFormat);          \
        PB_ASSIGN(preferredOperatorIndex);       \
        PB_CALLBACK(preferredOperator);          \
        PB_ASSIGN(preferredOperatorCaller);      \
        PB_ASSIGN(pbkListEntryCaller);           \
        PB_ASSIGN(pbkCharacterSet);              \
        PB_ASSIGN(tempStorage);                  \
        PB_ASSIGN(alphabet);                     \
        PB_ASSIGN(smsSetting.csmpFo);            \
        PB_ASSIGN(smsSetting.csdhShow);          \
        PB_CALLBACK(smsSetting.smsInfo);         \
        PB_ASSIGN(smsSetting.smsFormat);         \
        PB_ASSIGN(callWaitingPresentResultCode); \
        PB_ASSIGN(ussd);                         \
        PB_ASSIGN(ucClip);                       \
        PB_ASSIGN(ucClir);                       \
        PB_ASSIGN(ucColp);                       \
        PB_ASSIGN(ucCSSU);                       \
        PB_ASSIGN(ucCSSI);                       \
        PB_ASSIGN(moniPara);                     \
        PB_ASSIGN(monpPara);                     \
        PB_ASSIGN(pmIndMark);                    \
        PB_ASSIGN(vgrVolume);                    \
        PB_ASSIGN(vgtVolume);                    \
        PB_ASSIGN(cmutMute);                     \
        PB_ASSIGN(audioTestMode);                \
        PB_ASSIGN(tcpip.cipSCONT_unSaved);       \
        PB_ASSIGN(tcpip.cipMux_multiIp);         \
        PB_ASSIGN(tcpip.cipHEAD_addIpHead);      \
        PB_ASSIGN(tcpip.cipSHOWTP_dispTP);       \
        PB_ASSIGN(tcpip.cipSRIP_showIPPort);     \
        PB_ASSIGN(tcpip.cipATS_setTimer);        \
        PB_ASSIGN(tcpip.cipATS_time);            \
        PB_ASSIGN(tcpip.cipSPRT_sendPrompt);     \
        PB_ASSIGN(tcpip.cipQSEND_quickSend);     \
        PB_ASSIGN(tcpip.cipMODE_transParent);    \
        PB_ASSIGN(tcpip.cipCCFG_NmRetry);        \
        PB_ASSIGN(tcpip.cipCCFG_WaitTm);         \
        PB_ASSIGN(tcpip.cipCCFG_SendSz);         \
        PB_ASSIGN(tcpip.cipCCFG_esc);            \
        PB_ASSIGN(tcpip.cipCCFG_Rxmode);         \
        PB_ASSIGN(tcpip.cipCCFG_RxSize);         \
        PB_ASSIGN(tcpip.cipCCFG_Rxtimer);        \
        PB_ASSIGN(tcpip.cipDPDP_detectPDP);      \
        PB_ASSIGN(tcpip.cipDPDP_interval);       \
        PB_ASSIGN(tcpip.cipDPDP_timer);          \
        PB_ASSIGN(tcpip.cipCSGP_setGprs);        \
        PB_ASSIGN(tcpip.cipRDTIMER_rdsigtimer);  \
        PB_ASSIGN(tcpip.cipRDTIMER_rdmuxtimer);  \
        PB_ASSIGN(tcpip.cipRXGET_manualy);       \
        PB_CALLBACK(tcpip.cipApn);               \
        PB_CALLBACK(tcpip.cipUserName);          \
        PB_CALLBACK(tcpip.cipPassWord);          \
    } while (0)

#define AT_SMSINFO_ASSIGN()       \
    do                            \
    {                             \
        PB_ASSIGN(sCnmi.nMode);   \
        PB_ASSIGN(sCnmi.nMt);     \
        PB_ASSIGN(sCnmi.nBm);     \
        PB_ASSIGN(sCnmi.nDs);     \
        PB_ASSIGN(sCnmi.nBfr);    \
        PB_ASSIGN(nStorage1);     \
        PB_ASSIGN(nStorage2);     \
        PB_ASSIGN(nStorage3);     \
        PB_ASSIGN(nTotal1);       \
        PB_ASSIGN(nTotal2);       \
        PB_ASSIGN(nTotal3);       \
        PB_ASSIGN(nUsed1);        \
        PB_ASSIGN(nUsed2);        \
        PB_ASSIGN(nServType);     \
        PB_ASSIGN(nOverflowCtrl); \
        PB_ASSIGN(nOverflowMode); \
    } while (0)

static bool at_sms_info_decode(pb_array_decode_param_t *param)
{
    at_sms_info_t *pbAssignLeft = &((at_sms_info_t *)param->data)[param->idx];
    PB_AtSmsSetting_AtSmsInfo *pbAssignRight = (PB_AtSmsSetting_AtSmsInfo *)param->pbdata;
    AT_SMSINFO_ASSIGN();
    return true;
}

static bool at_sms_info_encode(unsigned idx, const pb_array_encode_param_t *param)
{
    PB_AtSmsSetting_AtSmsInfo *pbAssignLeft = param->pbdata;
    at_sms_info_t *pbAssignRight = &((at_sms_info_t *)param->data)[idx];
    AT_SMSINFO_ASSIGN();
    return true;
}

bool at_setting_decode(at_setting_t *setting, uint8_t *buffer, unsigned length)
{
    PB_AtSetting *pbs = (PB_AtSetting *)at_malloc(sizeof(*pbs));
    if (!pbs)
    {
        ATLOGE("AT PB malloc failed for decode pbs");
        return false;
    }

    pb_string_decode_param_t cmerInd = {setting->cmerInd, sizeof(setting->cmerInd)};
    pbs->cmerInd.funcs.decode = pb_string_decode_callback;
    pbs->cmerInd.arg = &cmerInd;

    pb_string_decode_param_t operFormat = {setting->operFormat, sizeof(setting->operFormat)};
    pbs->operFormat.funcs.decode = pb_string_decode_callback;
    pbs->operFormat.arg = &operFormat;

    pb_string_decode_param_t preferredOperator = {setting->preferredOperator, sizeof(setting->preferredOperator)};
    pbs->preferredOperator.funcs.decode = pb_string_decode_callback;
    pbs->preferredOperator.arg = &preferredOperator;

    PB_AtSmsSetting_AtSmsInfo smsInfoData;
    pb_array_decode_param_t smsInfo = {
        .data = setting->smsSetting.smsInfo,
        .cnt = sizeof(setting->smsSetting.smsInfo) / sizeof(at_sms_info_t),
        .subfields = PB_AtSmsSetting_AtSmsInfo_fields,
        .conv = at_sms_info_decode,
        .pbdata = &smsInfoData};
    pbs->smsSetting.smsInfo.funcs.decode = pb_array_decode_callback;
    pbs->smsSetting.smsInfo.arg = &smsInfo;

    pb_string_decode_param_t cipApn = {setting->tcpip.cipApn, sizeof(setting->tcpip.cipApn)};
    pbs->tcpip.cipApn.funcs.decode = pb_string_decode_callback;
    pbs->tcpip.cipApn.arg = &cipApn;

    pb_string_decode_param_t cipUserName = {setting->tcpip.cipUserName, sizeof(setting->tcpip.cipUserName)};
    pbs->tcpip.cipUserName.funcs.decode = pb_string_decode_callback;
    pbs->tcpip.cipUserName.arg = &cipUserName;

    pb_string_decode_param_t cipPassWord = {setting->tcpip.cipPassWord, sizeof(setting->tcpip.cipPassWord)};
    pbs->tcpip.cipPassWord.funcs.decode = pb_string_decode_callback;
    pbs->tcpip.cipPassWord.arg = &cipPassWord;

    pb_istream_t is = pb_istream_from_buffer(buffer, length);
    if (!pb_decode(&is, PB_AtSetting_fields, pbs))
    {
        ATLOGSE(TSM(0), "AT PB decode failed: %s", PB_GET_ERROR(&is));
        at_free(pbs);
        return false;
    }

    at_setting_t *pbAssignLeft = setting;
    PB_AtSetting *pbAssignRight = pbs;
    AT_ASSIGN();

    at_free(pbs);
    return true;
}

bool at_setting_encode(const at_setting_t *setting, uint8_t **buffer, unsigned *length)
{
    PB_AtSetting *pbs = (PB_AtSetting *)at_malloc(sizeof(*pbs));
    if (pbs == NULL)
    {
        ATLOGE("AT PB malloc failed for encode pbs");
        return false;
    }

    pb_string_encode_param_t cmerInd = {setting->cmerInd, sizeof(setting->cmerInd)};
    pbs->cmerInd.funcs.encode = pb_string_encode_callback;
    pbs->cmerInd.arg = &cmerInd;

    pb_string_encode_param_t operFormat = {setting->operFormat, sizeof(setting->operFormat)};
    pbs->operFormat.funcs.encode = pb_string_encode_callback;
    pbs->operFormat.arg = &operFormat;

    pb_string_encode_param_t preferredOperator = {setting->preferredOperator, sizeof(setting->preferredOperator)};
    pbs->preferredOperator.funcs.encode = pb_string_encode_callback;
    pbs->preferredOperator.arg = &preferredOperator;

    PB_AtSmsSetting_AtSmsInfo smsInfoData;
    pb_array_encode_param_t smsInfo = {
        .data = setting->smsSetting.smsInfo,
        .cnt = sizeof(setting->smsSetting.smsInfo) / sizeof(at_sms_info_t),
        .subfields = PB_AtSmsSetting_AtSmsInfo_fields,
        .conv = at_sms_info_encode,
        .pbdata = &smsInfoData};
    pbs->smsSetting.smsInfo.funcs.encode = pb_array_encode_callback;
    pbs->smsSetting.smsInfo.arg = &smsInfo;

    pb_string_encode_param_t cipApn = {setting->tcpip.cipApn, sizeof(setting->tcpip.cipApn)};
    pbs->tcpip.cipApn.funcs.encode = pb_string_encode_callback;
    pbs->tcpip.cipApn.arg = &cipApn;

    pb_string_encode_param_t cipUserName = {setting->tcpip.cipUserName, sizeof(setting->tcpip.cipUserName)};
    pbs->tcpip.cipUserName.funcs.encode = pb_string_encode_callback;
    pbs->tcpip.cipUserName.arg = &cipUserName;

    pb_string_encode_param_t cipPassWord = {setting->tcpip.cipPassWord, sizeof(setting->tcpip.cipPassWord)};
    pbs->tcpip.cipPassWord.funcs.encode = pb_string_encode_callback;
    pbs->tcpip.cipPassWord.arg = &cipPassWord;

    PB_AtSetting *pbAssignLeft = pbs;
    const at_setting_t *pbAssignRight = setting;
    AT_ASSIGN();

    size_t bufsize;
    if (!pb_get_encoded_size(&bufsize, PB_AtSetting_fields, pbs))
    {
        ATLOGE("AT PB failed to get encode size");
        goto free_failed;
    }

    if (bufsize > AT_PB_BUF_SIZE_MAX)
    {
        ATLOGE("AT PB invalid encode size: %u", bufsize);
        goto free_failed;
    }

    *buffer = (uint8_t *)at_malloc(bufsize);
    if (*buffer == NULL)
    {
        ATLOGE("AT PB failed to malloc encode buffer");
        goto free_failed;
    }

    pb_ostream_t os = pb_ostream_from_buffer(*buffer, bufsize);
    if (!pb_encode(&os, PB_AtSetting_fields, pbs))
    {
        ATLOGSE(TSM(0), "AT PB encode failed: %s", PB_GET_ERROR(&os));
        goto free_failed;
    }

    ATLOGE("AT PB encode size: %d", os.bytes_written);
    at_free(pbs);
    *length = os.bytes_written;
    return true;

free_failed:
    at_free(pbs);
    return false;
}
