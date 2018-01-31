#include "cs_types.h"
#include "cos.h"
#include "string.h"

#ifdef AT_MQTT_SUPPORT
#include "at_module.h"
#include "mqtt.h"
#include "at_cmd_mqttx.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#include "sxs_io.h"
#include "sockets.h"
#include "at_cmd_gprs.h"
#include "aliyun_iot_device.h"
#if LWIP_ALTCP && LWIP_ALTCP_TLS
#include "lwip/altcp_tls.h"
#endif

typedef enum {
    AT_MQTT_INIT,
    AT_MQTT_AUTH,
    AT_MQTT_CONNECTED,
} AT_MQTT_STATUS;

typedef struct at_mqttx_event{
    UINT32 event;
    INT32 result_code;
} AT_MQTTX_EVENT;

static mqtt_client_t s_at_mqtt_client;

static UINT8 s_topic[MQTT_TOPIC_LEN + 1];
static int s_msg_count = 0;
static UINT8 s_dlci = 0;
static UINT8 s_status = AT_MQTT_INIT;
static BOOL s_oper_err = FALSE;

static err_t at_mqttx_sub_def_topic(int qos);

static void at_mqttx_on_connect_result(INT32 result) {
    AT_TC(g_sw_GPRS, "at_mqttx_on_connect_result() - result code:%d", result);
    if (result == 0) {
        s_status = AT_MQTT_CONNECTED;
        if (at_mqttx_sub_def_topic(1) != 0) {
            AT_TC(g_sw_GPRS, "at_mqttx_on_connect_result() - fail to subscrib default topic");
            return;
        }
    }
}

static VOID AT_MQTTX_AsyncEventProcess(void *param)
{
    AT_MQTTX_EVENT* event = (AT_MQTTX_EVENT*)param;
    INT32 rc = event->result_code;
    UINT8 ev = event->event;
    INT8 resp = 0;
    AT_TC(g_sw_GPRS, "AT_MQTT_AsyncEventProcess() - event:%d result code:%d", ev, rc);
    switch (ev) {
    case EV_CFW_MQTT_CONN_RSP:
        AT_TC(g_sw_GPRS, "EV_CFW_MQTT_CONN_RSP");
        at_mqttx_on_connect_result(rc);
        break;
    case EV_CFW_MQTT_SUB_RSP:
        AT_TC(g_sw_GPRS, "EV_CFW_MQTT_SUB_UNSUB_RSP");
        break;
    case EV_CFW_MQTT_UNSUB_RSP:
        AT_TC(g_sw_GPRS, "EV_CFW_MQTT_UNSUB_RSP");
        break;
    case EV_CFW_MQTT_PUB_RSP:
        AT_TC(g_sw_GPRS, "EV_CFW_MQTT_PUB_RSPs");
        break;
    case EV_CFW_MQTT_DISCONN_RSP:
        AT_TC(g_sw_GPRS, "EV_CFW_MQTT_DISCONN_RSP");
        break;
    default:
        resp = 1;
        rc = 0;
        AT_TC(g_sw_GPRS, "AT_MQTTX_AsyncEventProcess unhandle event");
        break;
    }

    if (rc == 0) {
        s_oper_err = FALSE;
    } else {
        s_oper_err = TRUE;
        resp = -1;
    }
    if (resp = -1) {
        // response operation fail
        at_CmdRespCmeError(at_CmdGetByChannel(s_dlci), ERR_AT_CME_EXE_FAIL);
    } else if (resp == 0) {
        // response operation succuss
        at_CmdRespOK(at_CmdGetByChannel(s_dlci));
    } else {
        // do nothing
    }
    COS_FREE(event);
}

static VOID MQTTX_PostEvent(UINT32 nEventId, UINT32 response)
{    
    AT_MQTTX_EVENT* param = (AT_MQTTX_EVENT*)COS_MALLOC(sizeof(AT_MQTTX_EVENT));
    param->event =  nEventId;
    param->result_code= response;
    COS_TaskCallback(COS_GetDefaultATTaskHandle(), AT_MQTTX_AsyncEventProcess, param);
}

static VOID at_mqttx_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    MQTTX_PostEvent(EV_CFW_MQTT_CONN_RSP, status);
    AT_TC(g_sw_GPRS, "at_mqttx_connection_cb() - status = %d\n", status);
}

static err_t at_mqttx_connect(struct mqtt_connect_client_info_t *clientInfo, const ip_addr_t *server_ip, UINT16 port)
{
    err_t err;

#if LWIP_ALTCP && LWIP_ALTCP_TLS
    if (MQTT_PORT != port) {
        clientInfo->tls_config = altcp_tls_create_config_client(cre_ca_pem2,cre_cert_pem2,cre_key_pem2);
    } else {
        clientInfo->tls_config = NULL;
        AT_TC(g_sw_ATE,"the port is 1883 set the tls config to null\n");
    }
#endif
    err = mqtt_client_connect(&s_at_mqtt_client, server_ip, port, at_mqttx_connection_cb, 0, clientInfo);

    if (err != ERR_OK)
    {
        AT_TC(g_sw_GPRS, "at_mqttx_connect() - err = %d\n", err);
    }
    return err;
}

static VOID at_mqttx_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    SUL_ZeroMemory8(s_topic, sizeof(s_topic));
    strcpy(s_topic, topic);
}

static VOID at_mqttx_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    s_msg_count++;

    PAT_CMD_RESULT pResult = NULL;
    UINT8 urc_str[MQTT_INCOMING_MSG_LEN] = { 0 };

    AT_Sprintf(urc_str, "+CLOUDPUBLISH: %d, %s, %d, %s", s_msg_count, s_topic, strlen(data), (char *) data);
    at_CmdRespUrcText(at_CmdGetByChannel(s_dlci), urc_str);
}

static VOID at_mqttx_sub_request_cb(void *arg, err_t result)
{
    AT_TC(g_sw_GPRS, "at_mqttx_sub_request_cb() - result = %d\n", result);
    MQTTX_PostEvent(EV_CFW_MQTT_SUB_RSP, 0x0);
}

static VOID at_mqttx_unsub_request_cb(void *arg, err_t result)
{
    AT_TC(g_sw_GPRS, "at_mqttx_unsub_request_cb() - result = %d\n", result);
    MQTTX_PostEvent(EV_CFW_MQTT_UNSUB_RSP, result);
}

/**
 * sub 1 for subscribe, 0 for unsubscribe
 */
static err_t at_mqttx_sub_unsub(UINT8 *topic, int sub, int qos)
{
    AT_TC(g_sw_GPRS, "at_mqttx_sub_unsub() - sub = %d\n", sub);

    err_t err;
    if (sub)
    {
        s_msg_count = 0;

        /* Setup callback for incoming publish requests */
        mqtt_set_inpub_callback(&s_at_mqtt_client, at_mqttx_incoming_publish_cb, at_mqttx_incoming_data_cb, 0);

        err = mqtt_subscribe(&s_at_mqtt_client, topic, qos, at_mqttx_sub_request_cb, 0);
    }
    else
    {
        err = mqtt_unsubscribe(&s_at_mqtt_client, topic, at_mqttx_unsub_request_cb, 0);
    }

    if (err != ERR_OK)
    {
        AT_TC(g_sw_GPRS, "at_mqttx_sub_unsub() - err = %d\n", err);
    }
    return err;
}

static err_t at_mqttx_sub_def_topic(int qos) {
    aliot_device_info_pt dev_info = aliyun_iot_get_device_info();
    UINT8 topic[1024] = {0};
    AT_Sprintf(topic, "/%s/%s/get", dev_info->product_key, dev_info->device_name);
    return at_mqttx_sub_unsub(topic, 1, qos);
}

static VOID at_mqttx_pub_request_cb(void *arg, err_t result)
{
    AT_TC(g_sw_GPRS, "at_mqtt_pub_request_cb() - result: %d\n", result);
    MQTTX_PostEvent(EV_CFW_MQTT_PUB_RSP, result);
}

static err_t at_mqttx_publish(UINT8 *topic, UINT8 *pub_payload, UINT8 dup, UINT8 qos, UINT8 retain)
{
    err_t err;
    err = mqtt_publish(&s_at_mqtt_client, topic, pub_payload, strlen(pub_payload), dup, qos, retain, at_mqttx_pub_request_cb, NULL);
    if (err != ERR_OK)
    {
        AT_TC(g_sw_GPRS, "at_mqttx_publish() - err: %d\n", err);
    }
    return err;
}

static err_t at_mqttx_default_publish(UINT8 *payload, UINT8 dup, UINT8 qos, UINT8 retain) {
    err_t err;
    aliot_device_info_pt dev_info = aliyun_iot_get_device_info();
    UINT8 topic[1024] = {0};
    AT_Sprintf(topic, "/%s/%s/update", dev_info->product_key, dev_info->device_name);
    return at_mqttx_publish(topic, payload, dup, qos, retain);
}
static INT32 at_mqttx_auth(UINT8* pkey, UINT8* did, UINT8* dname, UINT8* dsecret, UINT8* host) {
    /* initialize device info */
    aliyun_iot_device_init();

    if (0 != aliyun_iot_set_device_info(pkey, dname, did, dsecret))
    {
        AT_TC(g_sw_TCPIP, "run aliyun_iot_set_device_info() error!");
        return -1;
    }

    /* Device AUTH */
    if (0 != aliyun_iot_auth(aliyun_iot_get_device_info(), aliyun_iot_get_user_info()))
    {
        AT_TC(g_sw_TCPIP, "run aliyun_iot_auth() error!");
        return -1;
    }
    s_status = AT_MQTT_AUTH;
    return 0;
}

static void at_mqttx_build_client_info(struct mqtt_connect_client_info_t *clientInfo,
    UINT8 **host, UINT16* port, UINT16 keepAlive, UINT8 cleanSession) {
    aliot_user_info_pt userInfo = aliyun_iot_get_user_info();
    memset(clientInfo, 0, sizeof(struct mqtt_connect_client_info_t));
    clientInfo->client_id = userInfo->client_id;
    clientInfo->client_user = userInfo->user_name;
    clientInfo->client_pass = userInfo->password;
    clientInfo->keep_alive = keepAlive;
    clientInfo->clean_session = cleanSession;
    *host = userInfo->host_name;
    *port = userInfo->port;
}

typedef struct _mqttx_dns_req_t {
    struct mqtt_connect_client_info_t clientInfo;
    UINT16 port;
    UINT8 dlci;
} MQTT_DNS_REQ_PARAM;

static void mqttx_dnsReq_cb(void* param) {
    COS_EVENT* ev = (COS_EVENT*)param;
    MQTT_DNS_REQ_PARAM *dns_req = (MQTT_DNS_REQ_PARAM*)ev->nParam3;
    UINT8 nSim = AT_SIM_ID(dns_req->dlci);

    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND) {
        ip_addr_t *ipaddr = (ip_addr_t *)ev->nParam1;
        struct mqtt_connect_client_info_t* clientInfo = &dns_req->clientInfo;
        UINT16 port = dns_req->port;

        if (at_mqttx_connect(clientInfo, ipaddr, port) != 0) {
            AT_TC(g_sw_TCPIP, "mqttx_dnsReq_cb at_mqttx_connect fail");
            at_CmdRespCmeError(at_CmdGetByChannel(dns_req->dlci), ERR_AT_CME_EXE_FAIL);
            return;
        }
        at_CmdRespOK(at_CmdGetByChannel(dns_req->dlci));
    } else {
        AT_TC(g_sw_TCPIP, "mqttx_dnsReq_cb invalid dns state:%d", ev->nEventId);
        at_CmdRespCmeError(at_CmdGetByChannel(dns_req->dlci), ERR_AT_CME_EXE_FAIL);
    }
    AT_TC(g_sw_TCPIP, "mqttx_dnsReq_cb dns state:%d", ev->nEventId);
    AT_FREE(dns_req);
    AT_FREE(ev);
}

static INT32 at_mqttx_connect_server(UINT8 dlci, UINT16 keepAlive, UINT8 clean, UINT8 version) {
    INT32 ret = 0;
    UINT8* host = NULL;
    ip_addr_t ipaddr;
    MQTT_DNS_REQ_PARAM* dns_req = AT_MALLOC(sizeof(MQTT_DNS_REQ_PARAM));
    if (dns_req == NULL) {
        AT_TC(g_sw_TCPIP, "at_mqttx_connect_server fail to malloc");
        return -1;
    }
    AT_MemZero(dns_req, sizeof(MQTT_DNS_REQ_PARAM));
    dns_req->dlci = dlci;
    mqtt_set_protocol_level(version);
    at_mqttx_build_client_info(&dns_req->clientInfo, &host, &dns_req->port, keepAlive, clean);
    if (host == NULL) {
        AT_TC(g_sw_TCPIP, "at_mqttx_connect_server fail to get host address");
        return -1;
    }
    ret = (INT32)CFW_GethostbynameEX(host, &ipaddr, 0, AT_SIM_ID(dlci), mqttx_dnsReq_cb, dns_req);
    if(ret == RESOLV_QUERY_INVALID) {
        AT_TC(g_sw_GPRS, "at_mqttx_connect_server CFW_GethostbynameEX fail");
        AT_FREE(dns_req);
        return -1;
    } else if (ret == RESOLV_COMPLETE) {
        AT_TC(g_sw_TCPIP, "at_mqttx_connect_server, CFW_GethostbynameEX() RESOLV_COMPLETE\n");
        if (at_mqttx_connect(&dns_req->clientInfo, &ipaddr, dns_req->port) != 0) {
            AT_TC(g_sw_GPRS, "at_mqttx_connect_server at_mqttx_connect fail");
            AT_FREE(dns_req);
            return -1;
        }
        AT_FREE(dns_req);
        return 0;
    } else if (ret == RESOLV_QUERY_QUEUED) {
        AT_TC(g_sw_TCPIP, "at_mqttx_connect_server, RESOLV_QUERY_QUEUED");
        return 1;
    }
    // wrong return code from getHostByName
    AT_FREE(dns_req);
    return -1;
}

VOID AT_MQTT_CmdFunc_CLOUDAUTH(AT_CMD_PARA *pParam)
{
    UINT8 param_count = pParam->paramCount;
    bool paramRet = TRUE;
    UINT8 *rspstr;
    UINT8 index = 0;
    UINT8* pkey;
    UINT8* did;
    UINT8* dname;
    UINT8* dsecret;
    UINT8* host;

    if (AT_CMD_SET == pParam->iType) {
        if (param_count != 5) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get product key
        pkey = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(pkey) > PRODUCT_KEY_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH pamameter len overflow PRODUCT_KEY_LEN %d", PRODUCT_KEY_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get device name
        dname = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(dname) > DEVICE_NAME_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH pamameter len overflow DEVICE_NAME_LEN %d", DEVICE_NAME_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get device id
        did = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(did) > DEVICE_ID_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH pamameter len overflow DEVICE_ID_LEN %d", DEVICE_ID_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get device secret
        dsecret = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(dsecret) > DEVICE_SECRET_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH pamameter len overflow DEVICE_SECRET_LEN %d", DEVICE_SECRET_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get Host
        host = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(host) > HOST_ADDRESS_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH pamameter len overflow DEVICE_SECRET_LEN %d", DEVICE_SECRET_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (at_mqttx_auth(pkey, did, dname, dsecret, host) != 0) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDAUTH at_mqttx_auth fail");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    } else if (AT_CMD_TEST == pParam->iType) {
        rspstr = "+CLOUDAUTH: <Product Key>,<Device Name>,<Device ID>,<Device Secret Key>,<Host>";
        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, rspstr));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }

}

VOID AT_MQTT_CmdFunc_CLOUDCONN(AT_CMD_PARA *pParam)
{
    UINT8 param_count = pParam->paramCount;
    bool paramRet = TRUE;
    UINT8* rspstr;
    UINT8 index = 0;
    s_dlci = pParam->nDLCI;
    UINT16 keepAlive;
    UINT8 cleanSession;
    UINT8 version;
    INT32 ret;

    if (s_status != AT_MQTT_AUTH && !s_oper_err) {
        AT_TC(g_sw_TCPIP, "AT+CLOUDCONN Wrong status:%d", s_status);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    if (AT_CMD_SET == pParam->iType) {
        if (param_count != 3) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDCONN invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        keepAlive = at_ParamUintInRange(pParam->params[index++], 60, 180, &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDCONN fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        cleanSession = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDCONN fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        version = at_ParamUintInRange(pParam->params[index++], 3, 4, &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDCONN fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        ret = at_mqttx_connect_server(s_dlci, keepAlive, cleanSession, version);
        if (ret < 0) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDCONN at_mqttx_connect_server fail");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        } else if (ret == 0) {
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
        }
    } else if (AT_CMD_TEST == pParam->iType) {
        rspstr = "+CLOUDCONN: <Keep Alive>,<Clean Session>,<Version>";
        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, rspstr));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}


VOID AT_MQTT_CmdFunc_CLOUDPUB(AT_CMD_PARA *pParam)
{
    UINT8 param_count = pParam->paramCount;
    bool paramRet = TRUE;
    UINT8* rspstr;
    UINT8 index = 0;
    UINT8* topic = NULL;
    UINT8* msg;
    UINT8 qos;
    UINT8 duplicate = 0;
    UINT8 retain = 0;
    err_t ret;
    if (s_status != AT_MQTT_CONNECTED) {
        AT_TC(g_sw_TCPIP, "AT+CLOUDPUB Wrong status:%d", s_status);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    if (AT_CMD_SET == pParam->iType) {
        // param count may be [2 - 5]
        if (param_count < 2 && param_count > 5) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDPUB invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (param_count == 3 || param_count == 5) {
            // get Topic
            topic = at_ParamStr(pParam->params[index++], &paramRet);
            if (!paramRet) {
                AT_TC(g_sw_TCPIP, "AT+CLOUDPUB fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            if (AT_StrLen(topic) > MQTT_TOPIC_LEN) {
                AT_TC(g_sw_TCPIP, "AT+CLOUDPUB Topic param overflow %d", MQTT_TOPIC_LEN);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }
        // get QoS
        qos = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDPUB fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        // get Message
        msg = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDPUB fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(msg) > MQTT_MSG_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDPUB Message param overflow %d", MQTT_MSG_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (param_count >= 4) {
            // get dup if input
            duplicate = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramRet);
            if (!paramRet) {
                AT_TC(g_sw_TCPIP, "AT+CLOUDPUB fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            // get retain if input
            retain = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramRet);
            if (!paramRet) {
                AT_TC(g_sw_TCPIP, "AT+CLOUDPUB fail to get param %d", index);
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }

        if (topic == NULL) {
            ret = at_mqttx_default_publish(msg , duplicate, qos, retain);
        } else {
            ret = at_mqttx_publish(topic, msg, duplicate, qos, retain);
        }
        if (ret != 0) {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    } else if (AT_CMD_TEST == pParam->iType) {
        rspstr = "+CLOUDPUB: Topic,QoS,Message,[duplicate],[retain]";
        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, rspstr));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

VOID AT_MQTT_CmdFunc_CLOUDSUB(AT_CMD_PARA *pParam)
{
    UINT8 param_count = pParam->paramCount;
    bool paramRet = TRUE;
    UINT8* rspstr;
    UINT8 index = 0;
    UINT8* topic;
    INT32 qos;
    if (s_status != AT_MQTT_CONNECTED) {
        AT_TC(g_sw_TCPIP, "AT+CLOUDSUB Wrong status:%d", s_status);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }

    if (AT_CMD_SET == pParam->iType) {
        if (param_count != 2) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDSUB invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        topic = at_ParamStr(pParam->params[index++], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDSUB fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(topic) > MQTT_TOPIC_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDSUB topic len overflow %d", MQTT_TOPIC_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        qos = at_ParamUintInRange(pParam->params[index++], 0, 1, &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDSUB fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (at_mqttx_sub_unsub(topic, 1, qos) != 0) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDSUB at_mqttx_sub_unsub FAIL");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    } else if (AT_CMD_TEST == pParam->iType) {
        rspstr = "+CLOUDSUB: Topic, QoS(0,1)";
        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, rspstr));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }

}

VOID AT_MQTT_CmdFunc_CLOUDUNSUB(AT_CMD_PARA *pParam)
{
    UINT8 param_count = pParam->paramCount;
    bool paramRet = TRUE;
    UINT8* rspstr;
    UINT8 index = 0;
    UINT8* topic;

    if (s_status != AT_MQTT_CONNECTED) {
        AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB Wrong status:%d", s_status);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }

    if (AT_CMD_SET == pParam->iType) {
        if (param_count != 1) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB invalid param_count:%d", param_count);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        topic = at_ParamStr(pParam->params[index], &paramRet);
        if (!paramRet) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB fail to get param %d", index);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (AT_StrLen(topic) > MQTT_TOPIC_LEN) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB topic len overflow %d", MQTT_TOPIC_LEN);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (at_mqttx_sub_unsub(topic, 0, 0) != 0) {
            AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB at_mqttx_sub_unsub FAIL");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
    } else if (AT_CMD_TEST == pParam->iType) {
        rspstr = "+CLOUDUNSUB: topic";
        AT_CMD_RETURN(at_CmdRespOKText(pParam->engine, rspstr));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }

}

VOID AT_MQTT_CmdFunc_CLOUDDISCONN(AT_CMD_PARA *pParam)
{
    if (s_status != AT_MQTT_CONNECTED) {
        AT_TC(g_sw_TCPIP, "AT+CLOUDUNSUB Wrong status:%d", s_status);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    if (AT_CMD_EXE == pParam->iType) {
        mqtt_disconnect(&s_at_mqtt_client);
        s_status = AT_MQTT_AUTH;
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    } else {
        AT_TC(g_sw_TCPIP, "exe in %s(), command type not allowed", __func__);
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

#endif    // end of AT_MQTT_SUPPORT 

