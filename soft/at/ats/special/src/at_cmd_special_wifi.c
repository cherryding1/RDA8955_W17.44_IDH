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



#include "at.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special_wifi.h"
#include "at_cfg.h"
#include "sockets.h"

#ifdef WIFI_SUPPORT
#include "wifi_config.h"
#ifdef WIFI_SMARTCONFIG
#include "wifi_smartconfig.h"
extern enum wland_smartlink_status rda_smartconfig_state;
#endif
#ifdef WIFI_SNIFFER
#include "wifi_sniffer.h"
#endif

extern void wifi_PowerOff(void);
extern void wifi_test_mode_close(void);
extern VOID wifi_PowerOn(UINT8 *local_mac);
extern VOID Wifi_ipAddrChangeReq(void *req);
extern VOID wifi_ScanAps(UINT8 dur, UINT8 *ssid);
extern void wifi_test_mode_set_rfregw(UINT16 reg_addr, UINT16 val);
extern void wifi_test_mode_set(UINT16 dbga, UINT16 rate, UINT16 channel, UINT16 len);
extern VOID wifi_ConnectApEx(wifi_conn_t *conn, UINT16 dur);
extern UINT8 localMac[6] ;

enum WIFI_MODE
{
    STATION_MODE = 0,
    SOFTAP_MODE,
    STATION_SOFTAP_MODE
};

enum WIFI_EXECUTING_AT_CMD
{
    CMD_IDLE = 0,
    CMD_WPOWER,
    CMD_WROLE,
    CMD_WMODE,
    CMD_WSCAN,
    CMD_WJOIN,
    CMD_WADDR,
    CMD_WLEAV,
    CMD_WMAC,
    CMD_WSTATUS,
    CMD_WSCONT,
    CMD_SMARTCONFIG,
    CMD_SNIFFER
};

UINT8 wifi_nDLCI = 0xFF;
UINT8 g_wifiState = 0x00;  //0: closed, 1: opened, 0x0f : openning, 0xff: closeing
UINT8 g_wifiAtStatus = 0x00;   //0:wifi closed, 1:wifi opened, 2:searching ap, 3:ap searching done, 4:ap connecting, 5:ap connected, 6:ap connect fail.

UINT8 g_wifiMacTest[6] = {0x00,};
RDA5890_BSS_DESC *g_ssidList;
UINT8 g_scanMode = NORMAL_SCAN;

UINT8 g_wifiAutoon = 0;
UINT8 g_wifiAutoJoin = 0;
BOOL g_nouseDhcp = 0;
UINT8 g_wifiMode = (UINT8)STATION_MODE;

UINT8 g_smartconfigStop = FALSE;

UINT8 SSID[20][IW_ESSID_MAX_SIZE];
UINT8 BSSID[20][ETH_ALEN];
BOOL mGetBSSID = FALSE;

enum WIFI_EXECUTING_AT_CMD g_executingAtCmd = CMD_IDLE;
wifi_conn_t g_wifiConn;
char g_ssid[IW_ESSID_MAX_SIZE + 1];
char g_password[64+1];

INT32 AT_WIFI_IPStringToDigit(UINT8 *pStr, UINT8 uSize, UINT8 *pNum, UINT8 *pNumSize)
{
    UINT8 i       = 0;
    UINT8 j       = 0;
    UINT8 m       = 0;
    UINT8 uBuf[4] = { 0 };
    UINT8 uDigitNum  = 0;
    UINT8 numSize = 0;

    AT_TC(g_sw_SPEC, "pStr: %s, uSize: %d", pStr, uSize);

    if ((NULL == pStr) || (0 == uSize))
    {
        return - 1;
    }

    for (i = 0; i < 4; i++)
    {
        m = 0;

        while ((pStr[j] != '.') && (j < uSize))
        {
            uBuf[m++] = pStr[j++];
        }
        uDigitNum = atoi(uBuf);
        AT_TC(g_sw_SPEC, "uBuf: %s, uDigitNum: %d, j: %d", uBuf, uDigitNum, j);

        if ((uBuf[0] != '0') && (uDigitNum < 1 || uDigitNum > 255))
        {
            return - 1;
        }

        pNum[numSize ++] = uDigitNum;
        AT_MemSet(uBuf, 0, 4);

        j++;
    }

    *pNumSize = numSize;
    return 1;

}

VOID storeBSSDESCInfo(int scan_index, char *ssid, char *bssid){
    SUL_MemCopy8(SSID[scan_index], ssid, strlen(ssid));
    SUL_MemCopy8(BSSID[scan_index], bssid, ETH_ALEN);
}

VOID getBssid(int index, char *bssid){
    SUL_MemCopy8(bssid, BSSID[index], ETH_ALEN);
}


//========================================================
//VOID AT_WIFI_AsyncEventProcess(COS_EVENT* pEvent)
//Handle asynchronous msg, called by AT_AsyncEventProcess.
//========================================================

extern BOOL ap_connected ;
extern struct bss_descriptor bss_desc_Used;
extern wifi_cxt_t wifi_context;

UINT8 ssid_string[RDA5890_MAX_NETWORK_NUM][80] = {0x00};
INT8 ssid_num = 0;
INT8 ssid_count = 0;
BOOL ssid_store(UINT8 * ssid,UINT8 * p)
{
    if(ssid_num >= (RDA5890_MAX_NETWORK_NUM))
        return FALSE;

    int i = 0;
    for(i = 0; i < ssid_num; i ++)
    {
        if(strncmp(ssid,ssid_string[i],strlen(ssid)) == 0)
        {
            return TRUE;
        }
    }

    strncpy(ssid_string[ssid_num],p,80);
    ssid_num ++;
    return TRUE;
}

BOOL ssid_get(UINT8 *p)
{
    if(ssid_count == ssid_num)
        return FALSE;

    strncpy(p,ssid_string[ssid_count],80);
    ssid_count ++;
    return TRUE;
}

INT8 ssid_numbers(void)
{
    return ssid_num;
}

void ssid_init()
{
    memset(ssid_string,0x00,sizeof(ssid_string));
    AT_TC(g_sw_SPEC, "ssid_string size is %d", sizeof(ssid_string));
    ssid_num = 0;
    ssid_count = 0;
}

VOID AT_WIFI_AsyncEventProcess(COS_EVENT *pEvent)
{
    UINT8 OutStr[80];
    memset(OutStr, 0, 50);
    AT_TC(g_sw_SPEC, "AT_WIFI_AsyncEventProcess %d", pEvent->nEventId);
	AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(wifi_nDLCI);
    switch (pEvent->nEventId)
    {
    case EV_CFW_WIFI_INIT_RSP:
    {
        if(0x00 != pEvent->nParam1)
        {
            at_CmdRespOK(engine);
            g_wifiAtStatus = 0x01;
            if(0x0f == g_wifiState)
            {
                g_wifiState = 0x01;
            }
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            if(0x0f == g_wifiState)
            {
                g_wifiState = 0x00;
            }
        }
        break;
    }
    case EV_CFW_WIFI_DEINIT_RSP:
    {
        g_wifiAtStatus = 0x00;
        at_CmdRespOK(engine);
        if(0xff == g_wifiState)
        {
            g_wifiState = 0x00;
        }
        break;
    }
    case EV_CFW_WIFI_SCAN_RSP:
    {
        UINT32 scan_cnt = pEvent->nParam2;
        UINT32 scan_index = 0;
	    UINT32 j = 0;
	    UINT8 bssid[ETH_ALEN];
	    UINT8 tmpStr[50];

        if (!mGetBSSID) {
            AT_Sprintf(OutStr, "+WSCAN: %x, cnt=%d(%d)", pEvent->nParam1, pEvent->nParam2, scan_cnt);
            at_CmdRespInfoText(engine, OutStr);
        }

        g_ssidList = (RDA5890_BSS_DESC *) pEvent->nParam1;

        for (scan_index = 0; scan_index < scan_cnt; scan_index++)
        {
           if (mGetBSSID) {
                //storeBSSDESCInfo(scan_index, (g_ssidList + scan_index)->ssid, (g_ssidList + scan_index)->bssid);
                //getBssid(scan_index, bssid);
                for (j=0; j<ETH_ALEN; j++) {
                    AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_BSSID... ... bssid[%d]= %x", j, (g_ssidList + scan_index)->bssid[j]);
                    SUL_StrPrint(tmpStr+2*j, "%02x", (g_ssidList + scan_index)->bssid[j]);
                    AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_BSSID... ... tmpStr= %s", tmpStr);
                }
                AT_Sprintf(OutStr, "%d: %s", scan_index, tmpStr);
				at_CmdRespInfoText(engine, OutStr);
            } else {
                AT_Sprintf(OutStr, "%d: %s", scan_index, (g_ssidList + scan_index)->ssid);
                at_CmdRespInfoText(engine, OutStr);
            }
        }

        at_CmdRespOK(engine);

        break;
    }
    case EV_CFW_WIFI_SCAN_SINGLE_RSP:
    {
        UINT32 j = 0;
        UINT8 index;
        UINT8 tmpStr[50];
        RDA5890_BSS_DESC * bss_des = (RDA5890_BSS_DESC *)pEvent->nParam1;
        index = (UINT8 *) pEvent->nParam2;
        AT_TC(g_sw_SPEC,  "AT EV_CFW_WIFI_SCAN_SINGLE_RSP index=%d",index);
        if(g_scanMode != SMARTCONFIG_SCAN)
        {
            if (mGetBSSID) {
                //storeBSSDESCInfo(scan_index, (g_ssidList + scan_index)->ssid, (g_ssidList + scan_index)->bssid);
                //getBssid(scan_index, bssid);
                for (j=0; j<ETH_ALEN; j++) {
                    AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_BSSID... ... bssid[%d]= %x", j, bss_des->bssid[j]);
                    SUL_StrPrint(tmpStr+2*j, "%02x", bss_des->bssid[j]);
                    AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_BSSID... ... tmpStr= %s", tmpStr);
                }
                AT_Sprintf(OutStr, "%d: %s", index, tmpStr);
                at_CmdRespInfoText(engine, OutStr);
            } else {
                AT_Sprintf(OutStr, "%s,%d,%u,%02x:%02x:%02x:%02x:%02x:%02x,%d", bss_des->ssid,bss_des->dot11i_info,(unsigned char)bss_des->rssi,
                bss_des->bssid[0], bss_des->bssid[1], bss_des->bssid[2], bss_des->bssid[3], bss_des->bssid[4], bss_des->bssid[5],bss_des->channel);
                //at_CmdRespInfoText(engine, OutStr);
                ssid_store(bss_des->ssid,OutStr);

            }
        }
        COS_FREE(bss_des);
        break;
    }
    case EV_CFW_WIFI_SCAN_COMPLETE:
    {
        g_wifiAtStatus = 0x03;
        if(g_scanMode == NORMAL_SCAN)
        {
            if(CMD_WJOIN == g_executingAtCmd)
            {
               wifi_ConnectApEx(&g_wifiConn, 50);
               //AT_SetAsyncTimerMux(engine, 0);
               return;
            }

            AT_Sprintf(OutStr,"+WSCAN:%d",ssid_numbers());
            at_CmdRespInfoText(engine, OutStr);

            INT8 i = 0;
            INT8 num = ssid_numbers();

            for(i = 0; i < num; i ++)
            {
                ssid_get(OutStr);
                at_CmdRespInfoText(engine, OutStr);
            }

            at_CmdRespOK(engine);
        }
        #ifdef WIFI_SMARTCONFIG
        else if(g_scanMode == SMARTCONFIG_SCAN)
        {
            if(rda_smartconfig_state == SMARTLINK_ENABLE)
            {
                AT_TC(g_sw_SPEC,  "ignore scan complete in sniff scan");
                return;
            }

            if(TRUE == g_smartconfigStop)
            {
                AT_TC(g_sw_SPEC,  "when stop smartconfig by timeout,ignore scan complete");
                g_smartconfigStop = FALSE;
                return;
            }

            static U8 scan_times = 0;

            char ssid[IW_ESSID_MAX_SIZE + 1];
            char password[64+1];
            u8 ipaddr[4];
            wifi_conn_t tWifiConn;
            BOOL finish = smartconfig_getssid_result(ssid, password, ipaddr);
            if(finish == TRUE)
            {
                scan_times = 0;

                tWifiConn.ssid = ssid;
                tWifiConn.ssid[AT_StrLen(ssid)] = '\0';
                tWifiConn.ssid_len = AT_StrLen(ssid);

                tWifiConn.password = password;
                tWifiConn.password[AT_StrLen(password)] = '\0';
                tWifiConn.pwd_len = AT_StrLen(password);

                wifi_ConnectApEx(&tWifiConn, 50);
            }
            else
            {
                if(scan_times++ > 5)
                {
                    scan_times = 0;
                    at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                    g_scanMode = NORMAL_SCAN;
                }
                else
                {
                    wifi_ScanAps(5, NULL);
                }
            }
        }
        #endif
        #ifdef WIFI_SNIFFER
        else if(g_scanMode == SNIFF_SCAN)
        {
            track_node *head;
            UINT8 macaddr[18] = {0};
            UINT32 track_num = wifi_sniffer_result(&head);

            AT_Sprintf(OutStr, "Sniff device count:%d", track_num);
            at_CmdRespInfoText(engine, OutStr);
            while(head)
            {
                AT_Sprintf(macaddr,"%02x:%02x:%02x:%02x:%02x:%02x",head->sa[0],head->sa[1],head->sa[2],head->sa[3],head->sa[4],head->sa[5]);
                at_CmdRespInfoText(engine, macaddr);
                head = head->next;
            }
            wifi_sniffer_end();
            at_CmdRespOK(engine);
            g_scanMode = NORMAL_SCAN;
        }
        #endif
        break;
    }
    #ifdef WIFI_SMARTCONFIG
    case EV_CFW_WIFI_SMART_CONFIG_COMPL:
    {
        if(pEvent->nParam1 == TRUE)
        {
            char ssid[IW_ESSID_MAX_SIZE + 1];
            char password[64+1];
            u8 ipaddr[4];
            BOOL finish = smartconfig_getssid_end(ssid, password, ipaddr);
            if(finish == TRUE)
            {
                wifi_conn_t tWifiConn;

                tWifiConn.ssid = ssid;
                tWifiConn.ssid[AT_StrLen(ssid)] = '\0';
                tWifiConn.ssid_len = AT_StrLen(ssid);

                tWifiConn.password = password;
                tWifiConn.password[AT_StrLen(password)] = '\0';
                tWifiConn.pwd_len = AT_StrLen(password);

                wifi_ConnectApEx(&tWifiConn, 50);

            }
        }
        else
        {
            at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
        break;
    }
    #endif
    case EV_CFW_WIFI_CONN_RSP:
    {
        UINT8 status = -1;
        status = (UINT8 *) pEvent->nParam3;
        AT_TC(g_sw_SPEC,  "EV_CFW_WIFI_CONN_RSP... ... status = %d", status);

        if(NORMAL_SCAN == g_scanMode)
        {
            if(0 == status)
            {
                g_wifiAtStatus = 0x05;
                at_CmdRespOK(engine);
            }
            else
            {
                g_wifiAtStatus = 0x06;
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }

#ifdef WIFI_SMARTCONFIG
        else if(SMARTCONFIG_SCAN == g_scanMode)
        {
            if(0 == status)
            {
                cg_wait(1000);
                int ret = send_smartconfig_response_to_host();
                if(ret == 0)
                {
                    at_CmdRespOK(engine);
                }
                else
                {
                    at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
                }

                g_scanMode = NORMAL_SCAN;
            }
            else
            {
                g_scanMode = NORMAL_SCAN;
                at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
#endif

        break;
    }

    case EV_CFW_WIFI_DISCONN_RSP:
    case EV_CFW_WIFI_DISCONNECT_IND:
    case EV_CFW_WIFI_IPADDR_UPDATE_IND:
    {
        at_CmdRespOK(engine);
        break;
    }

    default :
        break;

    }

}

//========================================================
// VOID AT_WIFI_CmdFunc_WPOWER(AT_CMD_PARA* pParam)
// Power on/off wifimodule, have one parameter, 1 mean power on,0 mean power off
//========================================================
VOID AT_WIFI_CmdFunc_WPOWER(AT_CMD_PARA *pParam)
{
    UINT16 uStrLen                   = 1;
    INT32 iRetValue                 = 0;
    static UINT8 siOpenCmd = (UINT8)NO_TEST;
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WPOWER;

    if (NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {
        AT_TC(g_sw_SPEC, "WPOWER Param->iType %d", pParam->iType);

        if(NULL == pParam->pPara)
        {
            goto iOpen_ERROR;
        }

        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto iOpen_ERROR;
            }
            if (1 != uNumOfParam)
            {
                goto iOpen_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &siOpenCmd, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
                at_CmdRespCmeError(pParam->engine,ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            I_AM_HERE();

            if(siOpenCmd == 1)
            {
                wifi_PowerOn(g_wifiMacTest);
                g_wifiState = 0x0f;
            }
            else
            {
                wifi_PowerOff();
                g_wifiState = 0xff;
            }

            wifi_nDLCI = pParam->nDLCI;
            AT_SetAsyncTimerMux(pParam->engine, 10);

            return;
        }

        case AT_CMD_TEST:
        {
            UINT8 arrStr[] = "iOpen: NO_TEST, RX_TEST, TX_TEST, OTHER ";
            at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);

            return;
        }

        case AT_CMD_READ:
        {
            UINT8 *p[] = {"NO_TEST", "RX_TEST", "TX_TEST", "OTHER"};
            UINT8 arrStr[30] = {0x00,};

			AT_Sprintf(arrStr, "+WPOWER: %d", siOpenCmd);
			at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);

            return;
        }
        default:
        {
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}

//========================================================
// VOID AT_WIFI_CmdFunc_WPOWER(AT_CMD_PARA* pParam)
// Set wifi mode , have one parameter, 0 mean station , 1 mean softAP , 2 mean staion+softAP.
// In the current version , only station mode is supported.
//========================================================
VOID AT_WIFI_CmdFunc_WROLE(AT_CMD_PARA *pParam)
{
    UINT16 uStrLen                   = 1;
    INT32 iRetValue                 = 0;
    UINT8 param = 0;
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WROLE;

    if (NULL == pParam)
    {
        goto ROLE_ERROR;
    }
    else
    {

        AT_TC(g_sw_SPEC, "WROLE Param->iType %d", pParam->iType);

        if(NULL == pParam->pPara)
        {
            goto ROLE_ERROR;
        }

        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto ROLE_ERROR;
            }
            if (1 != uNumOfParam)
            {
                goto ROLE_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &param, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
				at_CmdRespCmeError(pParam->engine,ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            I_AM_HERE();

            if(param != (UINT8)STATION_MODE)
            {
                AT_TC(g_sw_SPEC, "WROLE : only station mode is supported");
				goto ROLE_ERROR;
			}
            g_wifiMode = param;

			wifi_nDLCI = pParam->nDLCI;
            at_CmdRespOK(pParam->engine);

			return;
        }

        case AT_CMD_TEST:
        {
			UINT8 arrStr[50] = {0x00,};

            AT_Sprintf(arrStr, "+WROLE: \r\n %d \r\n %d \r\n %d \r\n", (UINT8)STATION_MODE, (UINT8)SOFTAP_MODE, (UINT8)STATION_SOFTAP_MODE);
			at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);
            return;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[30] = {0x00,};

            AT_Sprintf(arrStr, "+WROLE: %d", g_wifiMode);
            at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);

            return;
        }
        default:
        {
            goto ROLE_ERROR;
        }
        }
    }

ROLE_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_WMODE(AT_CMD_PARA *pParam)
{

    UINT16 uStrLen                   = 1;
    INT32 iRetValue                 = 0;
	UINT8 param = 0;
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WMODE;

    if (NULL == pParam)
    {
        goto MODE_ERROR;
    }
    else
    {

        AT_TC(g_sw_SPEC, "WROLE Param->iType %d", pParam->iType);

        if(NULL == pParam->pPara)
        {
            goto MODE_ERROR;
        }

        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto MODE_ERROR;
            }
            if (2 != uNumOfParam)
            {
                goto MODE_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &param, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
				at_CmdRespCmeError(pParam->engine,ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            if((param != 0) && (param != 1))
            {
                AT_TC(g_sw_SPEC, "WMODE : auto on : only 0 or 1 is supported");
				goto MODE_ERROR;
			}

            g_wifiAutoon = param;

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &param, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
				at_CmdRespCmeError(pParam->engine,ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            if((param != 0) && (param != 1))
            {
                AT_TC(g_sw_SPEC, "WMODE : auto join : only 0 or 1 is supported");
				goto MODE_ERROR;
			}

            g_wifiAutoJoin= param;

			wifi_nDLCI = pParam->nDLCI;
            at_CmdRespOK(pParam->engine);

			return;
        }

        case AT_CMD_TEST:
        {
			UINT8 arrStr[50] = {0x00,};

            AT_Sprintf(arrStr, "+WMODE: %d,%d", g_wifiAutoon,g_wifiAutoJoin);
			at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);
            return;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[50] = {0x00,};

            AT_Sprintf(arrStr, "+WMODE: %d,%d", g_wifiAutoon,g_wifiAutoJoin);
			at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);
            return;
        }
        default:
        {
            goto MODE_ERROR;
        }
        }
    }

MODE_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

//========================================================
//VOID AT_WIFI_CmdFunc_WSCAN(AT_CMD_PARA* pParam)
//Scan hotspot list,without parmeter
//========================================================
VOID AT_WIFI_CmdFunc_WSCAN(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WSCAN;

    if (NULL == pParam )
    {
        goto iOpen_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;

        AT_TC(g_sw_SPEC, "WSCAN Param->iType %d", Param->iType);

        if (NULL == Param->pPara)
        {
            goto iOpen_ERROR;
        }

        switch (Param->iType)
        {
        case AT_CMD_EXE:
        {
            ssid_init();
            wifi_nDLCI = pParam->nDLCI;
            g_scanMode = NORMAL_SCAN;
            wifi_ScanAps(5, NULL);
            g_wifiAtStatus = 0x02;

            AT_SetAsyncTimerMux(pParam->engine, 30);

            return;
        }
        case AT_CMD_TEST:
        {
            at_CmdRespOK(pParam->engine);
            return;
        }
        default:
        {
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

	at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}


//========================================================
//VOID AT_WIFI_CmdFunc_WJOIN(AT_CMD_PARA* pParam)
//Connect with hotspot, two parameters: num, ssid
//========================================================
VOID AT_WIFI_CmdFunc_WJOIN(AT_CMD_PARA *pParam)
{
    INT32 iRet = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WJOIN;

    if (NULL == pParam)
    {
        goto WJOIN_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto WJOIN_ERROR;

        if(0x01 != g_wifiState)
        {
            goto WJOIN_ERROR;
        }

        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 uParaCount;
            UINT8 arrCharacterSet[60] = {0,};
            UINT8 arrCharacterSet1[33] = {0,};
            UINT16 uSize ;

            iRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);

            if(ERR_SUCCESS != iRet)
                goto WJOIN_ERROR;

            if (uParaCount < 1)
                goto WJOIN_ERROR;

	        uSize  = SIZEOF(arrCharacterSet1);
	        AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, arrCharacterSet1, &uSize);
            memcpy(g_ssid,arrCharacterSet1,uSize);
            g_wifiConn.ssid = g_ssid;
            g_wifiConn.ssid[uSize] = '\0';
            g_wifiConn.ssid_len = uSize;

            if(2 == uParaCount)
            {
                uSize  = SIZEOF(arrCharacterSet);
                AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, arrCharacterSet, &uSize);
                memcpy(g_password,arrCharacterSet,uSize);

                g_wifiConn.password = g_password;
                g_wifiConn.password[uSize] = '\0';
                g_wifiConn.pwd_len = uSize;
            }
            else
            {
                g_wifiConn.password = NULL;
                g_wifiConn.pwd_len = 0;
            }

            wifi_nDLCI = pParam->nDLCI;
            g_wifiAtStatus = 0x04;
            g_executingAtCmd = CMD_WJOIN;
            wifi_ScanAps(5, g_wifiConn.ssid);
            //wifi_ConnectApEx(&tWifiConn, 50);
            //AT_SetAsyncTimerMux(pParam->engine, 0);
            return;
        }
        case AT_CMD_TEST:
        {
            if(TRUE == ap_connected)
            {
                UINT8 arrStr[30] = {0x00,};
                AT_Sprintf(arrStr, "+WJOIN: %s,%s,%02x:%02x:%02x:%02x:%02x:%02x",wifi_context.cur_ssid,wifi_context.cur_pwd,
                                              wifi_context.cur_bssid[0],wifi_context.cur_bssid[1],wifi_context.cur_bssid[2],
                                              wifi_context.cur_bssid[3],wifi_context.cur_bssid[4],wifi_context.cur_bssid[5]);

                at_CmdRespInfoText(pParam->engine, arrStr);
            }

            at_CmdRespOK(pParam->engine);
            return;
        }
        case AT_CMD_READ:
        {
            if(TRUE == ap_connected)
            {
                UINT8 arrStr[30] = {0x00,};

                AT_Sprintf(arrStr, "+WJOIN: %s,%02x:%02x:%02x:%02x:%02x:%02x,%u,%d",bss_desc_Used.data.ssid,
                bss_desc_Used.data.bssid[0],bss_desc_Used.data.bssid[1],bss_desc_Used.data.bssid[2],bss_desc_Used.data.bssid[3],bss_desc_Used.data.bssid[4],bss_desc_Used.data.bssid[5],
                (unsigned char)bss_desc_Used.data.rssi,bss_desc_Used.data.channel);

                at_CmdRespInfoText(pParam->engine, arrStr);
                at_CmdRespOK(pParam->engine);
            }
            else
            {
                goto WJOIN_ERROR;
            }

            return;
        }

        default:
            break;
        }
    }

WJOIN_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

//========================================================
//VOID AT_WIFI_CmdFunc_WADDR(AT_CMD_PARA* pParam)
//Get ip address,without parameter
//========================================================
VOID AT_WIFI_CmdFunc_WADDR(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;
    struct netif *netif;
    g_executingAtCmd = CMD_WADDR;

    if(NULL == pParam)
    {
        goto WADDR_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto WADDR_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_SET:
        {
            ipaddr_change_req_struct ip_req;
            UINT8 uParamCount  = 0;
            UINT8 ip_addr[50];
            UINT8 gateway[50];
            UINT8 ntMsk[50];
            UINT8 dns_addr[50];
            UINT8 uIndex = 0;
            UINT16 uSize;
            ip4_addr_t   ipaddr, netmask, gw, dns;

            netif = getEtherNetIf(0x11);
            if (netif ==NULL) {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

            iRet = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
            AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR... ... uParamCount= %d", uParamCount);
            // need at least <type> <ip> <netmask> <gateway>
            if ((iRet != ERR_SUCCESS) || (uParamCount != 4))
            {
                AT_TC(g_sw_SPEC,
                      "AT_WIFI_CmdFunc_WADDR, parameters error or parameter number not satisfied\n");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            //get <type>
            uIndex  = 0;
            uSize = 1;
            iRet = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_UINT8, &g_nouseDhcp, &uSize);
            AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR... ... use_dhcp= %d", g_nouseDhcp);
            if (iRet != ERR_SUCCESS ||g_nouseDhcp !=1)
            {
                AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR, get parameter error1\n");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            // get <ip>
            uIndex++;
            uSize = SIZEOF(ip_addr);
            iRet = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, ip_addr, &uSize);
            AT_TC(g_sw_AT_NW, "AT_WIFI_CmdFunc_WADDR... ... ip_addr= %s", ip_addr);
            if (iRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR, get parameter error2\n");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            } else {
                iRet = AT_WIFI_IPStringToDigit(ip_addr,  uSize, ip_req.ip_addr, (UINT8 *)&uSize);
            }

            // get <netmask>
            uIndex++;
            uSize = SIZEOF(ntMsk);
            iRet = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, ntMsk, &uSize);
            AT_TC(g_sw_AT_NW, "AT_WIFI_CmdFunc_WADDR... ... ntMsk= %s", ntMsk);
            if (iRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR, get parameter error3\n");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            } else {
                iRet = AT_WIFI_IPStringToDigit(ntMsk,  uSize, ip_req.netmask, (UINT8 *)&uSize);
            }

            //get<gateway>
            uIndex++;
            uSize = SIZEOF(gateway);
            iRet = AT_Util_GetParaWithRule(pParam->pPara, uIndex, AT_UTIL_PARA_TYPE_STRING, gateway, &uSize);
            AT_TC(g_sw_AT_NW, "AT_WIFI_CmdFunc_WADDR... ... gateway= %s", gateway);
            if (iRet != ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_WADDR, get parameter error4\n");
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            } else {
                iRet = AT_WIFI_IPStringToDigit(gateway,  uSize, ip_req.gateway, (UINT8 *)&uSize);
            }

            dhcp_stop(netif);
            etharp_cleanup_netif(netif);

            ipaddr.addr = (ip_req.ip_addr[3] << 24) | (ip_req.ip_addr[2] << 16) | (ip_req.ip_addr[1] << 8) | ip_req.ip_addr[0];
            gw.addr = (ip_req.gateway[3] << 24) | (ip_req.gateway[2] << 16) | (ip_req.gateway[1] << 8) | ip_req.gateway[0];
            netmask.addr = (ip_req.netmask[3] << 24) | (ip_req.netmask[2] << 16) | (ip_req.netmask[1] << 8) | ip_req.netmask[0];
            netif_set_addr(netif,&ipaddr,&netmask,&gw);
            at_CmdRespOK(pParam->engine);
            return;
        }

        case AT_CMD_EXE:
        {
            UINT8 outStr[50];

            netif = getEtherNetIf(0x11);
            if (netif ==NULL) {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

	        ip4_addr_t *gateway = netif_ip4_gw(netif);
            AT_Sprintf(outStr, "getway: %s",inet_ntoa(*gateway));
            at_CmdRespInfoText(pParam->engine, outStr);

	        ip4_addr_t *ip_addr = netif_ip4_addr(netif);
            AT_Sprintf(outStr, "ipaddr: %s",inet_ntoa(*ip_addr));
            at_CmdRespInfoText(pParam->engine, outStr);

	        ip4_addr_t *netmask = netif_ip4_netmask(netif);
            AT_Sprintf(outStr, "netmask: %s",inet_ntoa(*netmask));
            at_CmdRespInfoText(pParam->engine, outStr);

            ip4_addr_t *pri_dns_addr = ip_2_ip4(dns_getserver(0));
            AT_Sprintf(outStr, "dns: %s",inet_ntoa(*pri_dns_addr));
            at_CmdRespInfoText(pParam->engine, outStr);
#if 0
#if LWIP_IPV6
            ip6_addr_t *ip6_addr = netif_ip6_addr(netif,0);
            if (ip6_addr != NULL) {
                AT_Sprintf(outStr, "ip6 addr0: %s",inet6_ntoa(*ip6_addr));
                at_CmdRespInfoText(pParam->engine, outStr);
            }
            ip6_addr = netif_ip6_addr(netif,1);
            if (ip6_addr != NULL) {
                AT_Sprintf(outStr, "ip6 addr1: %s",inet6_ntoa(*ip6_addr));
                at_CmdRespInfoText(pParam->engine, outStr);
            }
#endif
#endif
            at_CmdRespOK(pParam->engine);
            return;
        }

        case AT_CMD_TEST:
        {
			UINT8 arrStr[50] = {0x00,};

            AT_Sprintf(arrStr, "+WADDR:%d\n",g_nouseDhcp);
			at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);
            return;
        }

        case AT_CMD_READ:
        {
            UINT8 outStr[50];
            netif = getEtherNetIf(0x11);
            if (netif ==NULL) {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

            ip4_addr_t *ip_addr = netif_ip4_addr(netif);
            AT_Sprintf(outStr, "+WADDR: %s",inet_ntoa(*ip_addr));
            at_CmdRespInfoText(pParam->engine, outStr);
#if 0
#if LWIP_IPV6
            ip6_addr_t *ip6_addr = netif_ip6_addr(netif,0);
            if (ip6_addr != NULL) {
                AT_Sprintf(outStr, "ip6 addr0: %s",inet6_ntoa(*ip6_addr));
                at_CmdRespInfoText(pParam->engine, outStr);
            }
            ip6_addr = netif_ip6_addr(netif,1);
            if (ip6_addr != NULL) {
                AT_Sprintf(outStr, "ip6 addr1: %s",inet6_ntoa(*ip6_addr));
                at_CmdRespInfoText(pParam->engine, outStr);
            }
#endif
#endif
            at_CmdRespOK(pParam->engine);
            return;
        }
        default:
        {
            goto WADDR_ERROR;
        }

        }
    }

WADDR_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}

//========================================================
//VOID AT_WIFI_CmdFunc_WLEAV(AT_CMD_PARA* pParam)
//Disconnect
//========================================================
VOID AT_WIFI_CmdFunc_WLEAV(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WLEAV;

    if (NULL == pParam)
    {
        goto WLEAV_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto WLEAV_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_EXE:
        {
            g_wifiAtStatus = 0x01;
            wifi_DisconnectAp();
            wifi_nDLCI = pParam->nDLCI;
            AT_SetAsyncTimerMux(pParam->engine, 10);
            return;
        }

        case AT_CMD_TEST:
        {
            at_CmdRespOK(pParam->engine);
            return;
        }
        default:
        {
            goto WLEAV_ERROR;
        }

        }
    }

WLEAV_ERROR:
    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

//========================================================
//VOID AT_WIFI_CmdFunc_WPRT(AT_CMD_PARA* pParam)
//Set or test type
//========================================================
VOID AT_WIFI_CmdFunc_WPRT(AT_CMD_PARA *pParam)
{
    INT32 iRet = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    UINT8 wifi_type;
    UINT16 uStrLen = 1;

    if (NULL == pParam)
    {
        goto WPRT_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto WPRT_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto WPRT_ERROR;
            }

            if (1 != uNumOfParam)
            {
                goto WPRT_ERROR;
            }

            iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &wifi_type, &uStrLen);

            if (ERR_SUCCESS != iRet)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            if(wifi_type != WIFI_TYPE_INFRA)
            {
                goto WPRT_ERROR;
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
            return;
        }
        case AT_CMD_TEST:
        {

            UINT8 support_type[2];
            AT_MemZero(support_type, 2);
            AT_Sprintf(support_type,"%d",WIFI_TYPE_INFRA);
            at_CmdRespInfoText(pParam->engine, support_type);
            at_CmdRespOK(pParam->engine);

            return;
        }

        default:
        {
            goto WPRT_ERROR;
        }

        }
    }

WPRT_ERROR:
    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

//========================================================
//VOID AT_WIFI_CmdFunc_QMAC(AT_CMD_PARA* pParam)
//test mac addr
//========================================================
VOID AT_WIFI_CmdFunc_QMAC(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WMAC;

    if (NULL == pParam)
    {
        goto QMAC_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto QMAC_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_EXE:
        {
            UINT8 mac_string[30] = {0x00,};
            AT_Sprintf(mac_string,"+WMAC:%02x%02x%02x%02x%02x%02x",localMac[0],localMac[1],localMac[2],localMac[3],localMac[4],localMac[5]);
            at_CmdRespInfoText(pParam->engine, mac_string);
            at_CmdRespOK(pParam->engine);

            return;
        }
        case AT_CMD_TEST:
        {
            at_CmdRespOK(pParam->engine);
            return;
        }
        default:
        {
            goto QMAC_ERROR;
        }

        }
    }

QMAC_ERROR:
    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_WSTATUS(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WSTATUS;

    if (NULL == pParam )
    {
        goto WSTATUS_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;

        AT_TC(g_sw_SPEC, "WSCAN Param->iType %d", Param->iType);

        if (NULL == Param->pPara)
        {
            goto WSTATUS_ERROR;
        }

        switch (Param->iType)
        {
            case AT_CMD_EXE:
            {
                UINT8 arrStr[60];

                if(0x05 == g_wifiAtStatus)
                {
                    AT_Sprintf(arrStr, "+WSTATUS:%d, %s,%02x:%02x:%02x:%02x:%02x:%02x,%u,%d",g_wifiAtStatus,bss_desc_Used.data.ssid,
                        bss_desc_Used.data.bssid[0],bss_desc_Used.data.bssid[1],bss_desc_Used.data.bssid[2],bss_desc_Used.data.bssid[3],bss_desc_Used.data.bssid[4],bss_desc_Used.data.bssid[5],
                        (unsigned char)bss_desc_Used.data.rssi,bss_desc_Used.data.channel);

                    at_CmdRespInfoText(pParam->engine, arrStr);
                }
                else
                {
                    AT_Sprintf(arrStr, "+WSTATUS: %d", g_wifiAtStatus);
                    at_CmdRespInfoText(pParam->engine, arrStr);
                }

                at_CmdRespOK(pParam->engine);

                return;
            }
            case AT_CMD_TEST:
            {
                at_CmdRespOK(pParam->engine);
                return;
            }
            default:
            {
                goto WSTATUS_ERROR;
            }
        }
    }

WSTATUS_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_WSCONT(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    g_executingAtCmd = CMD_WSCONT;

    if (NULL == pParam )
    {
        goto WSCONT_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;

        AT_TC(g_sw_SPEC, "WSCAN Param->iType %d", Param->iType);

        if (NULL == Param->pPara)
        {
            goto WSCONT_ERROR;
        }

        switch (Param->iType)
        {
            case AT_CMD_EXE:
            {
                at_CmdRespOK(pParam->engine);
                return;
            }
            case AT_CMD_READ:
            {
                UINT8 outStr[50];
                UINT8 ipStr[15];
                UINT8 gatewayStr[15];
                UINT8 netmaskStr[15];

                struct netif *netif;

                netif = getEtherNetIf(0x11);
                if (netif ==NULL) {
                    AT_Sprintf(outStr, "+WSCONT:%d,%d,%d,%d\n",g_wifiAutoon,g_wifiAutoJoin,g_wifiMode,g_nouseDhcp);
                    at_CmdRespInfoText(pParam->engine, outStr);
                    at_CmdRespOK(pParam->engine);
                    return;
                }

	            ip4_addr_t *gateway = netif_ip4_gw(netif);
                AT_Sprintf(gatewayStr,"%s",inet_ntoa(*gateway));

                ip4_addr_t *ip_addr = netif_ip4_addr(netif);
                AT_Sprintf(ipStr,"%s",inet_ntoa(*ip_addr));

                ip4_addr_t *netmask = netif_ip4_netmask(netif);
                AT_Sprintf(netmaskStr,"%s",inet_ntoa(*netmask));

                AT_Sprintf(outStr, "+WSCONT:%d,%d,%d,%d,%s,%s,%s\n",g_wifiAutoon,g_wifiAutoJoin,g_wifiMode,g_nouseDhcp,
                    ipStr,gatewayStr,netmaskStr);

                at_CmdRespInfoText(pParam->engine, outStr);
                at_CmdRespOK(pParam->engine);
                return;
            }
            default:
            {
                goto WSCONT_ERROR;
            }
        }
    }

WSCONT_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}


VOID AT_WIFI_CmdFunc_BSSID(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;
    UINT8 uParamCount  = 0;
    UINT8 bssid[ETH_ALEN];

    if(NULL == pParam)
    {
        goto BSSID_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto BSSID_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_READ:
        {
            int index;
            UINT16 uSize;
	     UINT32 j = 0;
            UINT8 OutStr[20];
            mGetBSSID = TRUE;
            wifi_ScanAps(5, NULL);
            AT_SetAsyncTimerMux(pParam->engine, 10);
            return;
        }

        default:
        {
            goto BSSID_ERROR;
        }

        }
    }

BSSID_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}

VOID AT_WIFI_CmdFunc_LKSTT(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;

    if(NULL == pParam)
    {
        goto LKSTT_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto LKSTT_ERROR;
        }

        switch (pParam->iType)
        {

        case AT_CMD_READ:
        {
            UINT8 OutStr[6];
            BOOL status = wifi_getConnectStatus();
            AT_TC(g_sw_SPEC,  "AT_WIFI_CmdFunc_LKSTT status = %d", status);
            AT_Sprintf(OutStr, "%d", status);
            at_CmdRespInfoText(pParam->engine, OutStr);
            at_CmdRespOK(pParam->engine);

            return;
        }
        default:
        {
            goto LKSTT_ERROR;
        }

        }
    }

LKSTT_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_SSID(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRet = ERR_SUCCESS;

    if(NULL == pParam)
    {
        goto SSID_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto SSID_ERROR;
        }

        switch (pParam->iType)
        {
            case AT_CMD_READ:
            {
		        UINT8 ssid[IW_ESSID_MAX_SIZE];
                wifi_GetSSID(ssid);
                at_CmdRespOK(pParam->engine);
                return;
            }
            case AT_CMD_SET:
            {
                UINT8 uNumOfParam = 0;
                UINT8 uLen        = 0;
                UINT8 arrBuf[IW_ESSID_MAX_SIZE];
    
                if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
                {
                    goto SSID_ERROR;
                }
    
                AT_TC(g_sw_SPEC, "NumOfParam %d", uNumOfParam);
    
                if (uNumOfParam != 1)
                {
                    goto SSID_ERROR;
                }
    
                // Get first parameter
                uLen = 1;
                if (ERR_SUCCESS != (iRet = AT_Util_GetPara(pParam->pPara, 0, arrBuf, &uLen))) // Get first parameter.
                {
                    AT_TC(g_sw_SPEC, "AT_Util_GetPara iRet Err 0x%x", iRet);
                    goto SSID_ERROR;
                }
                test_rda5890_set_ssid (arrBuf, IW_ESSID_MAX_SIZE);
                at_CmdRespOK(pParam->engine);
                return;
            }
            default:
            {
                goto SSID_ERROR;
            }
    
         }
    }

SSID_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;
}

VOID AT_TCPIP_CmdFunc_WRSSI(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;

    if(NULL == pParam)
    {
        goto RSSI_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto RSSI_ERROR;
        }

        switch (pParam->iType)
        {

        case AT_CMD_READ:
        {
            UINT8 rssi;
            UINT8 OutStr[50];

            rssi = check_wifi_rssi();
            AT_Sprintf(OutStr, "%d", rssi);
            AT_TC(g_sw_SPEC, "AT_TCPIP_CmdFunc_WRSSI rssi %d", rssi);
            at_CmdRespInfoText(pParam->engine, OutStr);
            at_CmdRespOK(pParam->engine);

            return;
        }
        default:
        {
            goto RSSI_ERROR;
        }

        }
    }

RSSI_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}

#ifdef WIFI_SMARTCONFIG
VOID AT_WIFI_CmdFunc_SMARTCONFIG(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    UINT16 uStrLen                   = 1;
    INT32 iRetValue                 = 0;
    static UINT8 smartconfigMode = 1;
    g_executingAtCmd = CMD_SMARTCONFIG;

    if(NULL == pParam)
    {
        goto iScan_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto iScan_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_EXE:
        {
            g_smartconfigStop = FALSE;
            int ret = smartconfig_getssid_start();
            if(ret == 0)
            {
                g_scanMode = SMARTCONFIG_SCAN;
                wifi_nDLCI = pParam->nDLCI;
                //AT_SetAsyncTimerMux(pParam->engine, 60);
                return;
            }
            else
            {
                goto iScan_ERROR;
            }
        }
        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto iScan_ERROR;
            }
            if (1 != uNumOfParam)
            {
                goto iScan_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &smartconfigMode, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
                at_CmdRespCmeError(pParam->engine,ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            if((smartconfigMode > 3)||(smartconfigMode < 0))
            {
                smartconfigMode = 1;
                goto iScan_ERROR;
            }

            g_smartconfigStop = FALSE;

            int ret = smartconfig_getssid_start();

            if(ret == 0)
            {
                g_scanMode = SMARTCONFIG_SCAN;
                wifi_nDLCI = pParam->nDLCI;
                //AT_SetAsyncTimerMux(pParam->engine, 60);
                return;
            }
            else
            {
                goto iScan_ERROR;
            }
        }
        case AT_CMD_READ:
        {
            UINT8 arrStr[30] = {0x00,};

            AT_Sprintf(arrStr, "+WSMARTCONFIG: %d", smartconfigMode);
            at_CmdRespInfoText(pParam->engine, arrStr);
            at_CmdRespOK(pParam->engine);

            return;
        }
        default:
        {
            goto iScan_ERROR;
        }

        }
    }

iScan_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_STOPSMARTCONFIG(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;

    if(NULL == pParam)
    {
        goto iScan_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto iScan_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_EXE:
        {
            g_scanMode = NORMAL_SCAN;
            wifi_nDLCI = pParam->nDLCI;
            smartconfig_stop();
            at_CmdRespOK(pParam->engine);
        }

        default:
        {
            goto iScan_ERROR;
        }

        }
    }

iScan_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}

VOID AT_WIFI_CmdFunc_SMARTCONFIG_RSP(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;

    if(NULL == pParam)
    {
        goto RSP_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto RSP_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_EXE:
        {
            int ret = send_smartconfig_response_to_host();
            if(ret == 0)
            {
                wifi_nDLCI = pParam->nDLCI;
                at_CmdRespOK(pParam->engine);
                return;
            }
            else
            {
                goto RSP_ERROR;
            }
        }
        default:
        {
            goto RSP_ERROR;
        }

        }
    }

RSP_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

    return;

}

#endif

#ifdef WIFI_SNIFFER
VOID AT_WIFI_CmdFunc_SNIFFER(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    INT32 iRetValue                 = 0;
    UINT16 uStrLen                   = 4;
    UINT32 SnifferTime = 0;

    if(NULL == pParam)
    {
        goto iSniffer_ERROR;
    }
    else
    {
        if (pParam->pPara == NULL)
        {
            goto iSniffer_ERROR;
        }

        switch (pParam->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                goto iSniffer_ERROR;
            }
            if (1 != uNumOfParam)
            {
                goto iSniffer_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT32, &SnifferTime, &uStrLen);

            if (ERR_SUCCESS != iRetValue)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
                return;
            }

            int ret = wifi_sniffer_start(SnifferTime);
            if(ret == 0)
            {
                g_scanMode = SNIFF_SCAN;
                wifi_nDLCI = pParam->nDLCI;
                at_CmdRespOK(pParam->engine);
                return;
            }
            else
            {
                goto iSniffer_ERROR;
            }
        }
        default:
        {
            goto iSniffer_ERROR;
        }

        }
    }

iSniffer_ERROR:

    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    return;

}
#endif
#endif


