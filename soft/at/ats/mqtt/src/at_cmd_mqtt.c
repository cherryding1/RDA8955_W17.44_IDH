#include "cs_types.h"
#include "cos.h"
#include "string.h"

#ifdef AT_MQTT_SUPPORT
#include "at_cmd_mqtt.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#include "sxs_io.h"
#include "sockets.h"
#include "at_cmd_gprs.h"
#if LWIP_ALTCP && LWIP_ALTCP_TLS
#include "lwip/altcp_tls.h"
#endif


mqtt_client_t at_static_client;

extern HANDLE g_hCosATTask;

static UINT8 g_Topic[MQTT_STRING_SIZE];
static int g_msg_count = 0;
static UINT8 nDlc = 0;
static VOID AT_MQTT_AsyncEventProcess(COS_EVENT *pEvent)
{
    UINT32 response = pEvent->nParam1;
    sxs_fprintf(TSTDOUT, "AT_MQTT_AsyncEventProcess() - pEvent->nEventId = %d\n", pEvent->nEventId);
    switch (pEvent->nEventId)
    {
        case EV_CFW_MQTT_CONN_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_CONN_RSP, connect success");
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDlc);
            break;
        case EV_CFW_MQTT_SUB_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_SUB_UNSUB_RSP, subscribe topic success");
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDlc);
            break;
        case EV_CFW_MQTT_UNSUB_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_UNSUB_RSP, unsubscribe topic success");
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDlc);
            break;
        case EV_CFW_MQTT_PUB_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_PUB_RSP, publish(0) topic success");
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDlc);
            break;
        case EV_CFW_MQTT_DISCONN_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_DISCONN_RSP, disconnect success");
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, nDlc);
            break;
        case EV_CFW_MQTT_ERROR_RSP:
            AT_TC(g_sw_GPRS, "EV_CFW_MQTT_ERROR_RSP, operation error");
            AT_TCPIP_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, nDlc);
            break;
        default:
            break;
    }
    COS_FREE(pEvent);
}

VOID CFW_MQTTPostEvent(UINT32 nEventId, UINT32 response)
{    
    COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
    ev->nEventId =  nEventId;
    ev->nParam1  = response;
    COS_TaskCallback(g_hCosATTask,AT_MQTT_AsyncEventProcess,ev);
}

static VOID at_mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    CFW_MQTTPostEvent(EV_CFW_MQTT_CONN_RSP, 0x0);
    sxs_fprintf(TSTDOUT, "at_mqtt_connection_cb() - status = %d\n", status);
}

VOID at_mqtt_connect(UINT8 *clientid, UINT16 aliveSeconds, const ip_addr_t *server_ip, UINT16 port, UINT8 cleansession)
{
    struct mqtt_connect_client_info_t ci;
    err_t err;

    ci.client_id = clientid;
    ci.keep_alive = aliveSeconds;
    ci.clean_session = cleansession;
    ci.client_user = NULL;
    ci.client_pass = NULL;
    ci.will_topic= NULL;

    if (1883 != port) {
        ci.tls_config = altcp_tls_create_config_client(cre_ca_pem2,cre_cert_pem2,cre_key_pem2);
    } else {
        ci.tls_config = NULL;
        AT_TC(g_sw_ATE,"the port is 1883 set the tls config to null\n");
    }
    err = mqtt_client_connect(&at_static_client, server_ip, port, at_mqtt_connection_cb, 0, &ci);

    if (err != ERR_OK)
    {
        sxs_fprintf(TSTDOUT, "at_mqtt_connect() - err = %d\n", err);
    }
}

static VOID at_mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    if (g_Topic)
    {
        SUL_ZeroMemory8(g_Topic, sizeof(g_Topic));
    }
    strcpy(g_Topic, topic);
}

static VOID at_mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    g_msg_count++;

    PAT_CMD_RESULT pResult = NULL;
    UINT8 urc_str[MQTT_STRING_SIZE] = { 0 };

    AT_Sprintf(urc_str, "+MQTTPUBLISH: %d, %s, %d, %s", g_msg_count, g_Topic, strlen(data), (char *) data);
    pResult = AT_CreateRC(CSW_IND_NOTIFICATION,
            CMD_RC_CR,
            CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, urc_str, (UINT16)AT_StrLen(urc_str), 0);
    AT_TC(g_sw_SA, "send an AT command to uart=%s", urc_str);
    AT_Notify2ATM(pResult, 1);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    if (g_Topic)
    {
        SUL_ZeroMemory8(g_Topic, sizeof(g_Topic));
    }
}

static VOID at_mqtt_sub_request_cb(void *arg, err_t result)
{
    sxs_fprintf(TSTDOUT, "at_mqtt_sub_request_cb() - result = %d\n", result);
    CFW_MQTTPostEvent(EV_CFW_MQTT_SUB_RSP, 0x0);
}

static VOID at_mqtt_unsub_request_cb(void *arg, err_t result)
{
    sxs_fprintf(TSTDOUT, "at_mqtt_unsub_request_cb() - result = %d\n", result);
    CFW_MQTTPostEvent(EV_CFW_MQTT_UNSUB_RSP, 0x0);
}

/**
 * sub 1 for subscribe, 0 for unsubscribe
 */
VOID at_mqtt_sub_unsub(UINT8 *topic, int sub, int qos)
{
    sxs_fprintf(TSTDOUT, "at_mqtt_sub_unsub() - sub = %d\n", sub);

    err_t err;
    if (sub)
    {
        g_msg_count = 0;

        /* Setup callback for incoming publish requests */
        mqtt_set_inpub_callback(&at_static_client, at_mqtt_incoming_publish_cb, at_mqtt_incoming_data_cb, 0);

        err = mqtt_subscribe(&at_static_client, topic, qos, at_mqtt_sub_request_cb, 0);
    }
    else
    {
        err = mqtt_unsubscribe(&at_static_client, topic, at_mqtt_unsub_request_cb, 0);
    }

    if (err != ERR_OK)
    {
        sxs_fprintf(TSTDOUT, "at_mqtt_sub_unsub() - err = %d\n", err);
    }
}

VOID at_mqtt_pub_request_cb(void *arg, err_t result)
{
    sxs_fprintf(TSTDOUT, "at_mqtt_pub_request_cb() - result: %d\n", result);
    CFW_MQTTPostEvent(EV_CFW_MQTT_PUB_RSP, 0x0);
}

void at_mqtt_publish(UINT8 *topic, UINT8 *pub_payload, UINT8 dup, UINT8 qos, UINT8 remain)
{
    err_t err;
    err = mqtt_publish(&at_static_client, topic, pub_payload, strlen(pub_payload), dup, qos, remain, at_mqtt_pub_request_cb, 0);
    if (err != ERR_OK)
    {
        sxs_fprintf(TSTDOUT, "at_mqtt_publish() - err: %d\n", err);
    }
}

typedef struct _mqtt_dns_req_t {
    UINT8 *clientid;
    UINT8 cleansession;
    UINT16 aliveSeconds;
    UINT16 port;
    AT_CMD_PARA *pParam;
} MQTT_REQ_T;

static dnsReq_callback(COS_EVENT *ev) {
    MQTT_REQ_T *callback_param = (MQTT_REQ_T*)ev->nParam3;
    AT_CMD_PARA *pParam = callback_param->pParam;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    
    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND) {
        ip_addr_t *server_ip = (ip_addr_t *)ev->nParam1;
        UINT8 *clientid = callback_param->clientid;
        UINT16 port = callback_param->port;
        UINT16 aliveSeconds = callback_param->aliveSeconds;
        UINT8 cleansession = callback_param->cleansession;
        
        at_mqtt_connect(clientid, aliveSeconds, server_ip, port,cleansession);
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        AT_FREE(clientid);
    } else {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }    
    AT_FREE(callback_param);
    AT_FREE(ev);
}
VOID AT_GPRS_CmdFunc_MQTTCONN(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 idx = 0;
    UINT16 uSize = 0;
    UINT32 err;

    UINT8 hostname[MQTT_STRING_SIZE] = { 0,};
    UINT8 *clientid = NULL;
    MQTT_REQ_T *callback_param = NULL;
    ip_addr_t server_ip;
    UINT16 port;
    UINT16 aliveSeconds;
    UINT8 cleansession;

    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if ((iResult != ERR_SUCCESS) || (uParamCount != 5))
            {
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = MQTT_STRING_SIZE;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &hostname, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTCONN get hostname error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            idx++;
            uSize = sizeof(UINT16);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT16, &port, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTCONN get port error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
            clientid = AT_MALLOC(MQTT_STRING_SIZE);
            if (clientid == NULL)
                goto error;
            idx++;
            uSize = MQTT_STRING_SIZE;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, clientid, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTCONN get clientid error");
                goto error;
            }

            idx++;
            uSize = sizeof(UINT16);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT16, &aliveSeconds, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTCONN get aliveSeconds error");
                goto error;
            }

            idx++;
            uSize = sizeof(UINT8);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &cleansession, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTCONN get cleansession error");
                goto error;
            }
            callback_param = (MQTT_REQ_T *)AT_MALLOC(sizeof(MQTT_REQ_T));
            if (callback_param == NULL) {
                goto error;
            }
            callback_param->clientid = clientid;
            callback_param->cleansession = cleansession;
            callback_param->aliveSeconds = aliveSeconds;
            callback_param->port = port;
            callback_param->pParam = pParam;
            err = CFW_GethostbynameEX(hostname, &server_ip, -1, -1,dnsReq_callback,callback_param);
            if(err == RESOLV_QUERY_INVALID) {
                AT_TC(g_sw_ATE, "dns query invalid!!!\n");
                goto error;
            } else if(err == RESOLV_COMPLETE){
                at_mqtt_connect(clientid, aliveSeconds, &server_ip, port,cleansession);
                AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                AT_FREE(clientid);
                AT_FREE(callback_param);
                return;
            }
            return;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "MQTTCONN ERROR");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
error:
    if (clientid != NULL)
        AT_FREE(clientid);
    if (callback_param != NULL)
        AT_FREE(callback_param);
    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
}


VOID AT_GPRS_CmdFunc_MQTTPUB(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 idx = 0;
    UINT16 uSize = 0;
    UINT32 err;

    UINT8 topic[MQTT_STRING_SIZE] = { 0,};
    UINT8 message[MQTT_STRING_SIZE] = { 0,};
    UINT8 qos;
    UINT8 dup;
    UINT8 remain;

    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if ((iResult != ERR_SUCCESS) || (uParamCount < 4))
            {
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (uParamCount == 5)
            {
                uSize = MQTT_STRING_SIZE;
                iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &topic, &uSize);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "MQTTPUB get topic error");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }
            else
            {
                strcpy(topic, "/pk/devicename/update");
            }

            idx++;
            uSize = MQTT_STRING_SIZE;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &message, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTPUB get message error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            idx++;
            uSize = sizeof(UINT8);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &qos, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTPUB get Qos error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            idx++;
            uSize = sizeof(UINT8);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &dup, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTPUB get Qos error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            idx++;
            uSize = sizeof(UINT8);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &remain, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTPUB get Qos error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }


            at_mqtt_publish(topic, message, qos, dup, remain);
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "MQTTSUB ERROR");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
}

VOID AT_GPRS_CmdFunc_MQTT_SUB_UNSUB(AT_CMD_PARA *pParam)
{
    INT32 iResult = 0;
    UINT8 uParamCount = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 idx = 0;
    UINT16 uSize = 0;
    UINT32 err;

    UINT8 topic[MQTT_STRING_SIZE] = { 0,};
    UINT8 sub;
    UINT8 qos = 0;

    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            if (iResult != ERR_SUCCESS)
            {
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            uSize = MQTT_STRING_SIZE;
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &topic, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTSUB get topic error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            idx++;
            uSize = sizeof(UINT8);
            iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &sub, &uSize);
            if (iResult != ERR_SUCCESS)
            {
                AT_TC(g_sw_GPRS, "MQTTSUB get isSubscribe error");
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }

            if (sub)
            {
                idx++;
                uSize = sizeof(UINT8);
                iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &qos, &uSize);
                if (iResult != ERR_SUCCESS)
                {
                    AT_TC(g_sw_GPRS, "MQTTSUB get isSubscribe error");
                    AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
            }

            at_mqtt_sub_unsub(topic, sub, qos);
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "MQTTSUB ERROR");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
}

VOID AT_GPRS_CmdFunc_MQTTDISCONN(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    nDlc = pParam->nDLCI;
    switch (pParam->iType)
    {
        case AT_CMD_EXE:
            g_msg_count = 0;
            if (g_Topic)
            {
                SUL_ZeroMemory8(g_Topic, sizeof(g_Topic));
            }
            mqtt_disconnect(&at_static_client);
            CFW_MQTTPostEvent(EV_CFW_MQTT_DISCONN_RSP, 0x0);
            break;
        default:
            AT_TC(g_sw_GPRS, "MQTT# DISCONN ERROR");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
    }
}

#endif

