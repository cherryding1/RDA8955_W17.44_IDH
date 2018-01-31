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
#ifdef GPS_SUPPORT
#include "stdio.h"
#include "cfw.h"
#include "at.h"
#include "at_sa.h" // caoxh[+]2008-04-19
#include "at_module.h"
#include "at_cmd_http.h"
#include "at_cfg.h"
#include "itf_api.h"
#include "sockets.h"
#include "http_api.h"
#include "at_cmd_gps.h"
#include "at_cmd_http.h"
#include "at_cmd_nw.h"

#define GPS_TASK_STACK_SIZE (2048 * 2)
#define GPS_TASK_PRIORITY (COS_MMI_TASKS_PRIORITY_BASE + 12)
HANDLE g_GPSTask = HNULL;

struct GPS_STATE g_GPSState;
extern void memory_Free_Http(Http_info *http_info);

static BOOL http_buffer_init(UINT32 size);
static void memory_Free_Http_agps(Http_info *http_info);

HANDLE GetTaskHandle_GPS(void)
{
    return g_GPSTask;
}
void GPSTask(void *p_arg)
{
    COS_EVENT ev = {0};
    HANDLE task = g_GPSTask;

    AT_TC(g_sw_SA, "start GPSTask");
    memset(&ev, 0, sizeof(COS_EVENT));
    InitGPSState();
    while (1)
    {
        COS_WaitEvent(task, &ev, COS_WAIT_FOREVER);
        switch (ev.nEventId)
        {
        case EV_GPS_RECV_IND:
        {
            //UINT32 time = csw_TMGetTick();
            UINT8 *DataBuf = NULL;
            UINT32 DataLen = 0;

            DataBuf = (UINT8 *)ev.nParam1;
            DataLen = ev.nParam2;
            if (0 == g_GPSState.gpsbinary)
            {
                //AT_TC(g_sw_GPRS,"GPS recv:[%d]%s",DataLen,DataBuf);
                gps_Pro(DataBuf, DataLen);
            }
            else if (1 == g_GPSState.gpsbinary)
            {
                Isuartbin149(DataBuf);
                isbinright(DataBuf, DataLen);
            }
            AT_FREE(DataBuf);
        }
        break;
        default:
            break;
        }
    }
}

void GpsCreateTask(void)
{
    g_GPSTask = COS_CreateTask(GPSTask,
                               NULL, NULL,
                               GPS_TASK_STACK_SIZE,
                               GPS_TASK_PRIORITY,
                               COS_CREATE_DEFAULT, 0, "GPSTask");
}

void InitGPSState()
{
    memset(&g_GPSState, 0x00, sizeof(g_GPSState));
    //read last position  121.589399,31.20462
    g_GPSState.lat = 31.20462;
    g_GPSState.lng = 121.589399;
    //readEPHtime(&g_GPSState.epht);
}

//+GPS:ON,LBS,3150.8558,N,11711.8781,E,
void MakeGPSState2Str(UINT8 *arrStr)
{
    UINT8 lat_str[20] = {0};
    UINT8 lng_str[20] = {0};
    double2str(g_GPSState.lat, lat_str);
    double2str(g_GPSState.lng, lng_str);

    AT_Sprintf(arrStr + strlen(arrStr), "+GPS:");
    //mode
    if (0 == g_GPSState.on)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "OFF,");
    }
    else if (1 == g_GPSState.on)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "ON,");
    }
    //state
    if (0 == g_GPSState.state)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "NOPOS,");
    }
    else if (1 == g_GPSState.state)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "LBS,");
    }
    else if (2 == g_GPSState.state)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "GPS,");
    }
    //latitude
    AT_Sprintf(arrStr + strlen(arrStr), "%s,", lat_str);
    if (0 <= g_GPSState.lat)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "N,");
    }
    else
    {
        AT_Sprintf(arrStr + strlen(arrStr), "S,");
    }
    //   longitude
    AT_Sprintf(arrStr + strlen(arrStr), "%s,", lng_str);
    if (0 <= g_GPSState.lng)
    {
        AT_Sprintf(arrStr + strlen(arrStr), "E,");
    }
    else
    {
        AT_Sprintf(arrStr + strlen(arrStr), "W,");
    }
}

void double2str(double num, UINT8 *num_str)
{
    int num_left = num;
    int num_right = 10000000 * (num - num_left);
    sprintf(num_str, "%d.%07d", num_left, num_right);
    AT_TC(g_sw_GPRS, "double2str:%s", num_str);
}
/*
********0724 GPIO OUTPUT 1 
$GNGGA,001231.799,3112.2771,N,12135.3639,E,0,0,,141.7,M,8.3,M,,*6A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GLGSA,A,1,,,,,,,,,,,,,,,*02
$GPGSV,1,1,00*79
$GLGSV,1,1,00*65
$GNRMC,001231.799,V,3112.2771,N,12135.3639,E,0.000,0.00,060180,,,N*58
$GNVTG,0.00,T,,M,0.000,N,0.000,K,N*2C
********0724 GPIO  OUTPUT 0
*/

#define NEMA_START_STR ("********0724 GPIO OUTPUT 1")
#define NEMA_END_STR ("********0724 GPIO  OUTPUT 0")
#define GPS_BUFFER_SIZE (1024)
UINT8 g_gps_buffer[GPS_BUFFER_SIZE] = {0};
UINT32 g_gps_buffer_len = 0;
extern uint32 cont_length;
extern uint8 *pBody;
void gps_Pro(void *p_arg, u16 size)
{
    if (g_gps_buffer_len + size > GPS_BUFFER_SIZE)
    {
        g_gps_buffer_len = 0; //hal_DbgAssert(0);
    }
    if (0 == strncmp(p_arg, NEMA_START_STR, strlen(NEMA_START_STR)))
    {
        g_gps_buffer_len = 0;
    }
    memcpy(g_gps_buffer + g_gps_buffer_len, p_arg, size);
    g_gps_buffer_len += size;

    check_gps_rmc();
}
void check_gps_rmc()
{
    UINT8 arrStr[GPS_BUFFER_SIZE] = {
        0x00,
    };
    UINT32 arrLen = 0;
    int16 pos_content = 0;
    UINT8 *parrStr = arrStr;

    MakeGPSRDStr_Single(parrStr, &arrLen, "RMC");
    //pos_content = ht_SearchStr(parrStr,"RMC");
    AT_TC(g_sw_GPRS, "check_gps_rmc 0 [%d][RMC]", pos_content);
    if (pos_content < 0)
    {
        return;
    }
    pos_content = ht_SearchStr(parrStr, "A");
    //AT_TC(g_sw_GPRS, "check_gps_rmc 1 [%d][A]",pos_content,parrStr);
    if (pos_content < 0)
    {
        if (2 == g_GPSState.state)
        {
            g_GPSState.state = 0;
            AT_TC(g_sw_GPRS, "check_gps_rmc 2  [%d][%s]", g_GPSState.state, parrStr);
        }
    }
    else
    {
        g_GPSState.state = 2;
        AT_TC(g_sw_GPRS, "check_gps_rmc 3  [%d][%s]", g_GPSState.state, parrStr);
    }
}

void MakeGPSRDStr_ALL(UINT8 *arrStr, UINT32 *parrLen)
{
    UINT16 NEMA_start = 0;
    UINT16 NEMA_len = 0;
    UINT8 find_NEMA = 0;
    UINT16 check = 0;
    //AT_TC(g_sw_GPRS, "ALL 1 [%d][%d][%s]",find_NEMA,check,&g_gps_buffer[check]);
    while (check <= g_gps_buffer_len)
    {
        if ('$' == g_gps_buffer[check])
        {
            //AT_TC(g_sw_GPRS, "ALL 2 [%d][%d][%s]",find_NEMA,check,&g_gps_buffer[check]);
            if (0 == find_NEMA)
            {
                NEMA_start = check;
                find_NEMA = 1;
            }
        }

        if (('*' == g_gps_buffer[check]) && (1 == find_NEMA))
        {
            //AT_TC(g_sw_GPRS, "ALL 3 [%d][%d][%s]",find_NEMA,check,&g_gps_buffer[check]);
            if (0 == strncmp(&g_gps_buffer[check], NEMA_END_STR, strlen(NEMA_END_STR)))
            {
                break;
            }
        }

        check++;
        if (1 == find_NEMA)
        {
            NEMA_len++;
        }
    }
    if (NEMA_len > 2)
    {
        NEMA_len = NEMA_len - 2; //delete "\r\n"
        memcpy(arrStr, g_gps_buffer + NEMA_start, NEMA_len);
        *parrLen = NEMA_len;
    }
}

void MakeGPSRDStr_Single(UINT8 *arrStr, UINT32 *parrLen, UINT8 *NEMA_GSA)
{
    UINT16 GSA_start = 0;
    UINT16 GSA_len = 0;
    UINT8 find_GSA = 0;
    UINT16 check = 0;
    while (check <= g_gps_buffer_len)
    {
        if ('$' == g_gps_buffer[check])
        {
            //AT_TC(g_sw_GPRS, "GSA 1 [%d][%s]",check,&g_gps_buffer[check]);
            if (0 == strncmp(&g_gps_buffer[check + 3], NEMA_GSA, 3))
            {
                if (0 == find_GSA)
                {
                    GSA_start = check;
                    find_GSA = 1;
                }
            }
            else
            {
                if (1 == find_GSA)
                {
                    break;
                }
            }
        }

        if ('*' == g_gps_buffer[check])
        {
            if (0 == strncmp(&g_gps_buffer[check], NEMA_END_STR, strlen(NEMA_END_STR)))
            {
                break;
            }
        }

        check++;
        if (1 == find_GSA)
        {
            GSA_len++;
        }
    }

    if (GSA_len > 2)
    {
        GSA_len = GSA_len - 2; //delete "\r\n"
        memcpy(arrStr, g_gps_buffer + GSA_start, GSA_len);
        *parrLen = GSA_len;
    }
}

VOID AT_CmdFunc_GPSRD(AT_CMD_PARA *pParam)
{

    UINT16 uStrLen = 10; //max length of CMD string
    INT32 iRetValue = 0;
    UINT8 content[20] = {0}; // ON  OFF UTC EPH RFLOC
    PAT_CMD_RESULT pResult;

    if (NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {

        AT_TC(g_sw_GPRS, "GPSRD Param->iType %d", pParam->iType);
        if (NULL == pParam->pPara)
        {
            goto iOpen_ERROR;
        }
        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            UINT8 uNumOfParam = 0;
            UINT8 arrStr[GPS_BUFFER_SIZE] = {0x00};
            UINT32 arrLen = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uNumOfParam))
            {
                AT_TC(g_sw_GPRS, "GPSRD error 1");
                goto iOpen_ERROR;
            }
            if (1 != uNumOfParam)
            {
                AT_TC(g_sw_GPRS, "GPSRD error 2");
                goto iOpen_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &content, &uStrLen);
            if (ERR_SUCCESS != iRetValue)
            {

                AT_GPS_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            I_AM_HERE();

            if (0 == strcmp("ALL", content))
            {
                MakeGPSRDStr_ALL(arrStr, &arrLen);
            }
            else if (0 == strcmp("GGA", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "GGA");
            }
            else if (0 == strcmp("GLL", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "GLL");
            }
            else if (0 == strcmp("GSA", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "GSA");
            }
            else if (0 == strcmp("GSV", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "GSV");
            }
            else if (0 == strcmp("RMC", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "RMC");
            }
            else if (0 == strcmp("VTG", content))
            {
                MakeGPSRDStr_Single(arrStr, &arrLen, "VTG");
            }
            else
            {
                AT_TC(g_sw_GPRS, "GPSRD error 10");
                goto iOpen_ERROR;
            }
            AT_TC(g_sw_GPRS, "GPSRD arrLen = %d", arrLen);
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, arrLen /*AT_StrLen(arrStr)*/, pParam->nDLCI);

            if (pResult != NULL)
                AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }
            return;
        }

        case AT_CMD_TEST:
        {

            UINT8 arrStr[] = "+GPSRD: (\"ALL\",\"GGA\",\"GLL\",\"GSA\",\"GSV\",\"RMC\",\"VTG\") ";
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (NULL != pResult)
            {
                AT_Notify2ATM(pResult, pParam->nDLCI);
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[256] = {0x00};
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (pResult != NULL)
                AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "GPS error 11");
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_CmdFunc_GPS(AT_CMD_PARA *pParam)
{

    UINT16 uStrLen = 10; //max length of CMD string
    INT32 iRetValue = 0;
    UINT8 content[20] = {0}; // ON  OFF UTC EPH RFLOC
    PAT_CMD_RESULT pResult;

    if (NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {

        AT_TC(g_sw_GPRS, "GPS Param->iType %d", pParam->iType);
        if (NULL == pParam->pPara)
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
                AT_TC(g_sw_GPRS, "GPS error 1");
                goto iOpen_ERROR;
            }
            if (1 != uNumOfParam)
            {
                AT_TC(g_sw_GPRS, "GPS error 2");
                goto iOpen_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &content, &uStrLen);
            if (ERR_SUCCESS != iRetValue)
            {

                AT_GPS_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            I_AM_HERE();

            if (0 == strcmp("ON", content))
            {
                if (0 == g_GPSState.on)
                {

                    g_GPSState.on = 1;
                    open_GPS_power_vVIB();
                    gps_uart_open();
                }
            }
            else if (0 == strcmp("OFF", content))
            {
                if (1 == g_GPSState.on)
                {
                    close_GPS_power_vVIB();
                    gps_uart_close();
                    g_GPSState.on = 0;
                }
            }
            else if (0 == strcmp("UTC", content))
            {
                UINT8 utc_str[256] = {0};
                BOOL tmRet = 0;
                TM_SYSTEMTIME st;
                tmRet = TM_GetSystemTime(&st);
                if (!tmRet)
                {
                    AT_TC(g_sw_GPRS, "GPS error 4");
                    goto iOpen_ERROR;
                }
                if (0 == g_GPSState.on)
                {
                    AT_TC(g_sw_GPRS, "GPS error 5");
                    goto iOpen_ERROR;
                }
                //$PGKC634,(year),(moon),(day),(hour),(min),(sec)
                sprintf(utc_str, "PGKC634,%d,%d,%d,%d,%d,%d", st.uYear, st.uMonth, st.uDay, st.uHour, st.uMinute, st.uSecond);
                guoke_uart_send(utc_str);
                AT_TC(g_sw_GPRS, "GPS send %s", utc_str);
            }
            else if (0 == strcmp("RFLOC", content))
            {
                UINT8 utc_str[256] = {0};
                UINT8 lat_str[20] = {0};
                UINT8 lng_str[20] = {0};
                BOOL tmRet = 0;
                TM_SYSTEMTIME st;
                tmRet = TM_GetSystemTime(&st);
                if (!tmRet)
                {
                    AT_TC(g_sw_GPRS, "GPS error 6");
                    goto iOpen_ERROR;
                }
                if (0 == g_GPSState.on)
                {
                    AT_TC(g_sw_GPRS, "GPS error 7");
                    goto iOpen_ERROR;
                }
                //$PGKC635,(lat),(lng),(high),(year),(moon),(day),(hour),(min),(sec)
                double2str(g_GPSState.lat, lat_str);
                double2str(g_GPSState.lng, lng_str);
                sprintf(utc_str, "PGKC635,%s,%s,0,%d,%d,%d,%d,%d,%d", lat_str, lng_str, st.uYear, st.uMonth, st.uDay, st.uHour, st.uMinute, st.uSecond);
                guoke_uart_send(utc_str);
                AT_TC(g_sw_GPRS, "GPS send %s", utc_str);
            }
            else if (0 == strcmp("EPH", content))
            {
                if (0 == g_GPSState.on)
                {
                    AT_TC(g_sw_GPRS, "GPS error 8");
                    goto iOpen_ERROR;
                }

                if (ERR_SUCCESS != guoke_agps_process())
                {
                    AT_TC(g_sw_GPRS, "GPS error 9");
                    goto iOpen_ERROR;
                }
            }
            else
            {
                AT_TC(g_sw_GPRS, "GPS error 10");
                goto iOpen_ERROR;
            }
            AT_GPS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 10, NULL, 0, pParam->nDLCI);

            return;
        }

        case AT_CMD_TEST:
        {

            UINT8 arrStr[] = "+GPS: (\"ON\",\"OFF\",\"UTC\",\"EPH\",\"RFLOC\") ";
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (NULL != pResult)
            {
                AT_Notify2ATM(pResult, pParam->nDLCI);
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[256] = {0x00};
            MakeGPSState2Str(arrStr);
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (pResult != NULL)
                AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "GPS error 11");
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_CmdFunc_GPSCMD(AT_CMD_PARA *pParam)
{

    UINT16 uStrLen = 128; //max length of CMD string
    INT32 iRetValue = 0;
    UINT8 content[128] = {0}; //
    PAT_CMD_RESULT pResult;

    if (NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {

        AT_TC(g_sw_GPRS, "GPSCMD Param->iType %d", pParam->iType);
        if (NULL == pParam->pPara)
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
                AT_TC(g_sw_GPRS, "GPSCMD error 1");
                goto iOpen_ERROR;
            }
            if (1 != uNumOfParam)
            {
                AT_TC(g_sw_GPRS, "GPSCMD error 2");
                goto iOpen_ERROR;
            }

            iRetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &content, &uStrLen);
            if (ERR_SUCCESS != iRetValue)
            {

                AT_GPS_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

            I_AM_HERE();

            guoke_uart_send(content);

            AT_GPS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 10, NULL, 0, pParam->nDLCI);

            return;
        }

        case AT_CMD_TEST:
        {

            UINT8 arrStr[] = "+GPSCMD: (\"cmdstring\") ";
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (NULL != pResult)
            {
                AT_Notify2ATM(pResult, pParam->nDLCI);
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[256] = {0x00};
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (pResult != NULL)
                AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "GPSCMD error 11");
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

UINT32 utc_2_hour(TM_SYSTEMTIME *st)
{
    char days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    UINT16 year = st->uYear;
    UINT8 month = st->uMonth;
    UINT8 day = st->uDay;
    UINT8 hour = st->uHour;

    if ((year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0))) // leap year
        days[2] = 29;

    while (month > 0)
    {
        day--;
        if (day < 0)
        {
            month--;
            day = days[month];
        }
    }
}

BOOL checkEPHvalid(TM_SYSTEMTIME *st)
{
    if ((st->uYear == g_GPSState.epht.uYear) && (st->uMonth == g_GPSState.epht.uMonth) && (st->uDay == g_GPSState.epht.uDay))
    {
        UINT32 new_minutes = st->uHour * 60 + st->uMinute;
        UINT32 old_minutes = g_GPSState.epht.uHour * 60 + g_GPSState.epht.uMinute;
        if ((new_minutes >= old_minutes) && (new_minutes < (old_minutes + 360)))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    if ((st->uYear == g_GPSState.epht.uYear) && (st->uMonth == g_GPSState.epht.uMonth) && (st->uDay == (g_GPSState.epht.uDay + 1)))
    {
        UINT32 new_minutes = st->uHour * 60 + st->uMinute + 60 * 24;
        UINT32 old_minutes = g_GPSState.epht.uHour * 60 + g_GPSState.epht.uMinute;
        if ((new_minutes >= old_minutes) && (new_minutes < (old_minutes + 360)))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    return FALSE;
}
#define AGPS_BUFFER_SIZE 4096
U8 lbs_flag = 0;
extern AT_CCED gpsPosiList;
UINT8 lbs_url[HTTP_URL_SIZE] = {0x00};
AT_CMD_ENGINE_T *gLbsEngine;

VOID AT_CmdFunc_LBS(AT_CMD_PARA *pParam)
{

    UINT8 uParamCount = 0;
    INT32 iResult = 0;
    UINT16 uSize = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 content[20] = {0};
    UINT8 tmpString[60] = {0x00};
    CgHttpApi *cg_http_api;
    Http_info *http_info1;
    if (NULL == pParam)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }
    gLbsEngine = pParam->engine;
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        http_buffer_deinit();
        if (http_buffer_init(AGPS_BUFFER_SIZE) == FALSE)
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        uSize = sizeof(content); //HTTP_URL_SIZE;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &content, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "HTTPGET get URL error");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_GPRS, "HTTPGET get URL : %s", content);

        http_info1 = (Http_info *)malloc(sizeof(Http_info));

        cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));

        cg_http_api->url = cg_net_uri_new();

        cg_http_api->g_httpReq = cg_http_request_new();
        if (strcmp("EPH", content) == 0)
        {
            lbs_flag = 1;
            memset(lbs_url, 0x00, HTTP_URL_SIZE);
            //sprintf(url,"http://118.193.145.14:8000/brdcGPD.dat_rda");
            sprintf(lbs_url, "http://goke-agps.com:7777/brdcGPD.dat");
            AT_TC(g_sw_GPRS, "HTTPGET %s", lbs_url);
        }
        else if (strcmp("RFLOC", content) == 0)
        {
            lbs_flag = 2;
            memset(lbs_url, 0x00, HTTP_URL_SIZE);
            sprintf(lbs_url, "http://apilocate.amap.com/position?accesstype=0&imei=352315052834187&cmda=0&bts="
                             "%d%d%d,%d,%d,%d,%d"                                 //460,0,24591,17527,-65
                             "&output=json&key=d9c8968cf77bd9e6cbaab746a19e9505", // need rda itself KEY
                    gpsPosiList.sMcc[0],
                    gpsPosiList.sMcc[1], gpsPosiList.sMcc[2],
                    gpsPosiList.sMnc[0] * 1 + gpsPosiList.sMnc[1] * 2 + gpsPosiList.sMnc[2] * 2 * 2,
                    gpsPosiList.sLac, gpsPosiList.sCellID, (gpsPosiList.iRxLev * 2 - 113));
            AT_TC(g_sw_GPRS, "HTTPGET %s", lbs_url);
            //hal_DbgAssert(0);
        }
        else
        {
            AT_TC(g_sw_GPRS, "HTTPGET get URL error");
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        if (cg_http_api == NULL || http_info1 == NULL)

        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        else
        {

            http_info1->url = lbs_url;
            http_info1->cg_http_api = cg_http_api;

            if ((app_WPHTTPGet(http_info1, process_HTTP_agps, memory_Free_Http_agps) == TRUE))
            {

                AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
            }
            else
            {

                at_CmdRespInfoNText(pParam->engine, "Task is already running, please wait\n", 37);
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPGET:<url>");
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI);
        break;
    default:
        AT_TC(g_sw_GPRS, "HTTPGET ERROR");
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        break;
    }
}
#if 0
VOID AT_CmdFunc_LBS(AT_CMD_PARA *pParam)
{
	
       UINT8 uParamCount = 0;
       INT32 iResult = 0;
       UINT16 uSize = 0;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       UINT8 url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       UINT8 content[20] = { 0 };
       if (NULL == pParam)
       {
               AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
               return;
       }
	http_buffer_deinit();
      if( http_buffer_init(AGPS_BUFFER_SIZE)==FALSE)
	{
              AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
              return;
       }
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
                       if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
                       {
                               AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                               return;
                       }
                       uSize = 10;
                       iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &content, &uSize);
                       if (iResult != ERR_SUCCESS)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET get URL error");
                               AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                               return;
                       }
                       AT_TC(g_sw_GPRS, "HTTPGET get URL : %s",content);
			if(strcmp("EPH",content)==0)
			{
			   lbs_flag=1;
			   //sprintf(url,"http://118.193.145.14:8000/brdcGPD.dat_rda");
			   sprintf(url,"http://140.206.73.181:7777/brdcGPD.dat_rda");
			}
			else if(strcmp("RFLOC",content)==0)
			{
			    lbs_flag=2;
			    sprintf(url,"http://apilocate.amap.com/position?accesstype=0&imei=352315052834187&cmda=0&bts="
					"%d%d%d,%d%d%d,%d,%d,%d"//460,0,24591,17527,-65
					"&output=json&key=914d4ea9cbd385cd81c2e24305ffcb73",
					gpsPosiList.sMcc[0],gpsPosiList.sMcc[1],gpsPosiList.sMcc[2],
					gpsPosiList.sMnc[0],gpsPosiList.sMnc[1],gpsPosiList.sMnc[2],
					gpsPosiList.sLac,gpsPosiList.sCellID,(gpsPosiList.iRxLev*2-113));
			AT_TC(g_sw_GPRS, "HTTPGET %s",url);	
				//hal_DbgAssert(0);
			}
			else
			{
                               AT_TC(g_sw_GPRS, "HTTPGET get URL error");
                               AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                               return;
                       }
                       if((lunch_http_get((UINT8*)url,process_HTTP_agps)==TRUE)){
			   
                            AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);

                       }else{
                       
                            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            return;

                       }
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPGET:<url>");
                       AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI);
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPGET ERROR");
                       AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                       break;
       }
}
#endif
VOID AT_CmdFunc_EPH(AT_CMD_PARA *pParam)
{

    UINT16 uStrLen = 10; //max length of CMD string
    INT32 iRetValue = 0;
    UINT8 content[20] = {0}; // ON  OFF UTC EPH RFLOC
    PAT_CMD_RESULT pResult;

    if (NULL == pParam)
    {
        goto iOpen_ERROR;
    }
    else
    {

        AT_TC(g_sw_GPRS, "EPH Param->iType %d", pParam->iType);
        if (NULL == pParam->pPara)
        {
            goto iOpen_ERROR;
        }
        switch (pParam->iType)
        {

        case AT_CMD_SET:
        {
            goto iOpen_ERROR;
        }

        case AT_CMD_TEST:
        {
            goto iOpen_ERROR;
        }

        case AT_CMD_READ:
        {
            UINT8 arrStr[256] = {0x00};
            TM_SYSTEMTIME st;
            BOOL tmRet = 0;
            tmRet = TM_GetSystemTime(&st);
            if (!tmRet)
            {
                AT_TC(g_sw_GPRS, "EPH error 4");
                goto iOpen_ERROR;
            }

            if (checkEPHvalid(&st))
            {
                AT_Sprintf(arrStr + strlen(arrStr), "+EPH:VALID");
            }
            else
            {
                AT_Sprintf(arrStr + strlen(arrStr), "+EPH:INVALID");
            }
            pResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, arrStr, AT_StrLen(arrStr), pParam->nDLCI);

            if (pResult != NULL)
                AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }

            return;
        }
        default:
        {
            AT_TC(g_sw_GPRS, "EPH error 11");
            goto iOpen_ERROR;
        }
        }
    }

iOpen_ERROR:

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (NULL != pResult)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_GPS_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)

{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_GPS_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}
extern U8 lbs_flag;
extern struct GPS_STATE g_GPSState;
uint32 cont_length = 0;
uint32 http_head_length = 0;
uint8 *pBody = NULL;
uint8 *pBuffHead = NULL;
uint8 search_str[] = {0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x4c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3a, 0x00}; //"Content-Length:"
uint8 fota_package_header[] = {0x0d, 0x0a, 0x0d, 0x0a, 0x00};
static void memory_Free_Http_agps(Http_info *http_info)
{

    cg_net_uri_delete((http_info->cg_http_api)->url);

    cg_http_request_delete((http_info->cg_http_api)->g_httpReq);

    free(http_info->cg_http_api);

    free(http_info);
}
void http_buffer_deinit(void)
{
    if (NULL != pBuffHead)
    {
        AT_FREE(pBuffHead);
    }
    cont_length = 0;
    http_head_length = 0;
    pBody = NULL;
    pBuffHead = NULL;
}
static BOOL http_buffer_init(UINT32 size)
{
    BOOL result = TRUE;
    cont_length = 0;
    http_head_length = 0;
    pBody = NULL;
    pBody = (uint8 *)AT_MALLOC(size);
    if (NULL == pBody)
    {
        AT_TC(g_sw_GPRS, "http_buffer_init fail!");
        pBuffHead = NULL;
        result = FALSE;
    }
    else
    {
        AT_TC(g_sw_GPRS, "http_buffer_init OK!");
        pBuffHead = pBody;
        memset(pBody, 0x00, size);
        result = TRUE;
    }
    return result;
}
uint32 get_content_lenth(int16 pos)
{
    uint16 i, j;
    uint16 pos_temp;
    uint16 count = 0;
    uint32 cont_num[6];
    uint32 temp;

    pos_temp = pos + sizeof(search_str) / sizeof(uint8); //pos=>str true address
    while (*(pBuffHead + pos_temp) != 0x0d && *(pBuffHead + pos_temp) != 0x0a)
    {
        if (*(pBuffHead + pos_temp) >= '0' && *(pBuffHead + pos_temp) <= '9')
        {
            cont_num[count] = *(pBuffHead + pos_temp) - 0x30;
            count++;
        }
        pos_temp++;
    }

    temp = 0;
    for (i = 0; i < count; i++)
    {
        for (j = 1; j <= count - i - 1; j++)
        {
            cont_num[i] *= 10;
        }
        temp += cont_num[i];
    }

    return temp;
}

BOOL finish_HTTP_agps_getHeadBody(void)
{
    int16 len, rlen;
    int16 pos_content, pos_agps_header, pos_temp, pos_agps_header_next;
    int32 ret;
    uint8 *pheader;
    uint8 *PkgData;
    uint16 i;
    UINT8 download_state[128] = {0};

    //start search Content-length:
    if (0 == cont_length)
    {
        pos_content = ht_SearchStr(pBuffHead, search_str);
        if (pos_content < 0)
        {
            AT_TC(g_sw_GPRS, "No found Content-Length:");
            //hal_HstSendEvent(pBuffHead);
            //hal_HstSendEvent(search_str);
            //hal_DbgAssert(0);
            return FALSE;
        }
        cont_length = get_content_lenth(pos_content);
        AT_TC(g_sw_GPRS, "http_download:cont_length=%d", cont_length); //hoper
    }

    //start search { 0d 0a 0d 0a }
    if (0 == http_head_length)
    {
        pos_agps_header = ht_SearchStr(pBuffHead, fota_package_header);
        if (pos_agps_header < 0)
        {
            AT_TC(g_sw_GPRS, "No found http_head_length:");
            return FALSE;
        }
        http_head_length = pos_agps_header + 4;
        AT_TC(g_sw_GPRS, "http_download:http_head_length=%d", http_head_length);
    }
    //downloading
    if ((pBody - (pBuffHead + http_head_length)) < (cont_length))
    {
        //memset(download_state,0x00,sizeof(download_state));
        //sprintf(download_state,"[%d%]\r\n",(pBody- (pBuffHead + http_head_length))*100/(cont_length));
        //AT_TC(g_sw_GPRS,"[%d%]",(pBody- (pBuffHead + http_head_length))*100/(cont_length));
        return FALSE;
    }

    //download finish
    //memset(download_state,0x00,sizeof(download_state));
    //sprintf(download_state,"[%d%]\r\n",(pBody- (pBuffHead + http_head_length))*100/(cont_length));
    //AT_TC(g_sw_GPRS,"[%d%]",(pBody- (pBuffHead + http_head_length))*100/(cont_length));
    pBody = pBuffHead + http_head_length;
    return TRUE;
}
void process_HTTP_agps(const unsigned char *pData, size_t len)
{
    AT_TC(g_sw_GPRS, "[http_data_save][%d][%s]", len, pData);
    memcpy(pBody, pData, len);
    pBody += len;
    check_finish_HTTP_agps();
}

void check_finish_HTTP_agps(void)
{
    if (lbs_flag == 1)
    {
        if (finish_HTTP_agps_getHeadBody())
        {
            memset(g_GPSState.pEPH, 0, sizeof(g_GPSState.pEPH));
            g_GPSState.EPHlen = cont_length;
            memcpy(g_GPSState.pEPH, pBody, cont_length);
            lbs_flag = 0;
            //http_buffer_deinit();
            AT_AsyncWriteUart(gLbsEngine, "download OK\r\n", strlen("download OK\r\n"));
        }
    }
    else if (lbs_flag == 2)
    {
        if (finish_HTTP_agps_getHeadBody())
        {
            lbs_flag = 0;
            lbs_get_time();
            lbs_get_location();
            //http_buffer_deinit();
            AT_AsyncWriteUart(gLbsEngine, "download OK\r\n", strlen("download OK\r\n"));
        }
    }
}
void lbs_get_time(void)
{
    int16 pos_content = 0;
    UINT8 *ptime = pBuffHead;
    pos_content = ht_SearchStr(ptime, "Date:");
    if (pos_content < 0)
    {
        return;
    }
    //Mon Tue Wed Thu Fri Sat Sun
    ptime = ptime + pos_content + 6;
    if (0 == strncmp(ptime, "Mon", 3))
    {
        g_GPSState.epht.uDayOfWeek = 1;
    }
    else if (0 == strncmp(ptime, "Tue", 3))
    {
        g_GPSState.epht.uDayOfWeek = 2;
    }
    else if (0 == strncmp(ptime, "Wed", 3))
    {
        g_GPSState.epht.uDayOfWeek = 3;
    }
    else if (0 == strncmp(ptime, "Thu", 3))
    {
        g_GPSState.epht.uDayOfWeek = 4;
    }
    else if (0 == strncmp(ptime, "Fri", 3))
    {
        g_GPSState.epht.uDayOfWeek = 5;
    }
    else if (0 == strncmp(ptime, "Sat", 3))
    {
        g_GPSState.epht.uDayOfWeek = 6;
    }
    else if (0 == strncmp(ptime, "Sun", 3))
    {
        g_GPSState.epht.uDayOfWeek = 7;
    }
    //01 - 31
    ptime = ptime + 5;
    g_GPSState.epht.uDay = (*ptime - '0') * 10 + (*(ptime + 1) - '0');
    //Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    ptime = ptime + 3;
    if (0 == strncmp(ptime, "Jan", 3))
    {
        g_GPSState.epht.uMonth = 1;
    }
    else if (0 == strncmp(ptime, "Feb", 3))
    {
        g_GPSState.epht.uMonth = 2;
    }
    else if (0 == strncmp(ptime, "Mar", 3))
    {
        g_GPSState.epht.uMonth = 3;
    }
    else if (0 == strncmp(ptime, "Apr", 3))
    {
        g_GPSState.epht.uMonth = 4;
    }
    else if (0 == strncmp(ptime, "May", 3))
    {
        g_GPSState.epht.uMonth = 5;
    }
    else if (0 == strncmp(ptime, "Jun", 3))
    {
        g_GPSState.epht.uMonth = 6;
    }
    else if (0 == strncmp(ptime, "Jul", 3))
    {
        g_GPSState.epht.uMonth = 7;
    }
    else if (0 == strncmp(ptime, "Aug", 3))
    {
        g_GPSState.epht.uMonth = 8;
    }
    else if (0 == strncmp(ptime, "Sep", 3))
    {
        g_GPSState.epht.uMonth = 9;
    }
    else if (0 == strncmp(ptime, "Oct", 3))
    {
        g_GPSState.epht.uMonth = 10;
    }
    else if (0 == strncmp(ptime, "Nov", 3))
    {
        g_GPSState.epht.uMonth = 11;
    }
    else if (0 == strncmp(ptime, "Dec", 3))
    {
        g_GPSState.epht.uMonth = 12;
    }
    //year 2017
    ptime = ptime + 4;
    g_GPSState.epht.uYear = (*ptime - '0') * 1000 + (*(ptime + 1) - '0') * 100 + (*(ptime + 2) - '0') * 10 + (*(ptime + 3) - '0');
    //hour 0-23
    ptime = ptime + 5;
    g_GPSState.epht.uHour = (*ptime - '0') * 10 + (*(ptime + 1) - '0');
    //minute 0-59
    ptime = ptime + 3;
    g_GPSState.epht.uMinute = (*ptime - '0') * 10 + (*(ptime + 1) - '0');
    //second 0-59
    ptime = ptime + 3;
    g_GPSState.epht.uSecond = (*ptime - '0') * 10 + (*(ptime + 1) - '0');
    //setting RTC time
    TM_SetSystemTime(&g_GPSState.epht);
}

void lbs_get_location(void)
{
    int16 pos_content = 0;
    double lng = 0;
    double lat = 0;
    int dot = 0;
    double min = 0;
    UINT8 *ploc = pBody;
    UINT8 lng_str[20] = {0};
    pos_content = ht_SearchStr(ploc, "location");
    if (pos_content < 0)
    {
        return;
    }
    //lng 121.5818685
    ploc = ploc + pos_content + 11;
    dot = 0;
    min = 0.1;
    while (*ploc != ',')
    {
        if (((*ploc) == '.') && (0 == dot))
        {
            dot = 1;
        }
        if (((*ploc) >= '0') && ((*ploc) <= '9'))
        {
            if (0 == dot)
            {
                lng = lng * 10 + (*ploc) - '0';
            }
            else
            {
                lng = lng + ((*ploc) - '0') * min;
                min = min * 0.1;
            }
        }
        ploc++;
    }
    g_GPSState.lng = lng;
    //lat 31.199157
    ploc = ploc + 1;
    dot = 0;
    min = 0.1;
    while (*ploc != '\"')
    {
        if (((*ploc) == '.') && (0 == dot))
        {
            dot = 1;
        }

        if (((*ploc) >= '0') && ((*ploc) <= '9'))
        {
            if (0 == dot)
            {
                lat = lat * 10 + (*ploc) - '0';
            }
            else
            {
                lat = lat + ((*ploc) - '0') * min;
                min = min * 0.1;
            }
        }
        ploc++;
    }
    g_GPSState.lat = lat;
    AT_TC(g_sw_GPRS, "lbs_get_location 1  [%d]", g_GPSState.state);
    if (g_GPSState.state == 0)
    {
        g_GPSState.state = 1;
        AT_TC(g_sw_GPRS, "lbs_get_location 2  [%d]", g_GPSState.state);
    }
}

#endif
