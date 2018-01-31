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

#ifdef AT_FTP_SUPPORT
#include <csw.h>
#include "sxs_io.h"
#include "string.h"
#include "at_cmd_ftp.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "at_module.h"
#include "sockets.h"
#include "at_utils.h"
#include "at_dispatch.h"
#include "at_data_engine.h"

#define FTP_BUFFER_LEN (1024 * 10)
int32_t api_FTPLogout();
int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd);
extern AT_DATA_ENGINE_T *at_DispatchGetDataEngine(AT_DISPATCH_T *th);
extern UINT32 CFW_GethostbynameEX(char *hostname, ip_addr_t *addr, UINT8 nCid, CFW_SIM_ID nSimID,
                                  COS_CALLBACK_FUNC_T func, VOID *param);
static void ftp_commandComplete(uint8_t succ, uint8_t *data);
static void ftp_get_disconnect();
static void ftp_TransferStart();

CFW_FTP_CTX_T gFTPCtx;
static struct FTP_Param *gFTP_Param = NULL;

typedef struct
{
    uint8_t data[FTP_SEND_MAX_SIZE];
    uint8_t *cur;
    uint8_t *end;
} FTP_TXBUFF_T;

static void ftp_put_tx_work_start(FTP_TXBUFFER_T *tx)
{
    tx->tx_work = true;
}

static void ftp_put_tx_work_stop(FTP_TXBUFFER_T *tx)
{
    tx->tx_work = false;
}

static bool ftp_put_tx_work(FTP_TXBUFFER_T *tx)
{
    return tx->tx_work;
}

int32_t ftp_txbufpush(FTP_TXBUFFER_T *tx, uint8_t *data, uint16_t size)
{
    if (size + tx->len > tx->request)
        size = tx->request - tx->len;

    memcpy(tx->buffer + tx->len, data, size);
    tx->len += size;
    return tx->request - tx->len;
}

static void ftp_data_mode_stop(uint8_t nDlc)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDlc);
    at_DispatchEndDataMode(dispatch);
}

static void ftp_txbufferinit(FTP_TXBUFFER_T *tx, uint32_t request)
{
    tx->len = 0;
    tx->sended = 0;
    tx->request = request;
}

static void ftp_reportFTPState(uint8_t *data)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(gFTPCtx.nDLCI);
    if (data)
        at_CmdRespInfoText(engine, data);
}

static void ftp_send_txbuf_finish(FTP_TXBUFFER_T *tx)
{
    ftp_txbufferinit(tx, 0);
    ftp_data_mode_stop(gFTPCtx.nDLCI);
    ftp_commandComplete(0, NULL);
}

static void ftp_send_txbuf(FTP_TXBUFFER_T *tx)
{
    AT_TC(g_sw_GPRS, "FTP# in ftp_send_txbuf");
    if (tx->sended == tx->request)
    {
        ftp_send_txbuf_finish(tx);
        return;
    }

    int32_t ret = CFW_TcpipSocketSend(gFTPCtx.dataSocket, tx->buffer + tx->sended,
                                      tx->request - tx->sended, 0);
    ftp_put_tx_work_start(tx);
    if (ret > 0)
        tx->sended += ret;

    if (ret < 0)
    {
        ftp_send_txbuf_finish(tx);
        AT_TC(g_sw_GPRS, "FTP# ftp_send_txbuf error");
        return;
    }

    at_StartCallbackTimer(1000, (COS_CALLBACK_FUNC_T)ftp_send_txbuf, (void *)tx);
}

static int ftp_UartRecv(void *param, uint8_t *data, unsigned length)
{
    AT_TC(g_sw_GPRS, "FTP# ftp_UartRecv =%d", length);
    int32_t left = ftp_txbufpush(&gFTPCtx.tx_buf, data, length);
    if (left == 0)
        ftp_send_txbuf(&gFTPCtx.tx_buf);
    return length;
}

static void ftp_data_mode_start(uint8_t nDlc)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDlc);
    at_DispatchSetDataMode(dispatch);
    at_DataSetBypassMode(at_DispatchGetDataEngine(dispatch), (AT_DATA_BYPASS_CALLBACK_T)ftp_UartRecv, NULL);
}

void ftp_MainProcess(COS_EVENT ev);

CFW_FTP_CMD ftp_cmd[] =
    {
        {FTP_CONNECT, "CONNECT", "220"},
        {FTP_USER, "USER", "331"},
        {FTP_PASS, "PASS", "230"},
        {FTP_SIZE, "SIZE", "213"},
        {FTP_TYPE, "TYPE", "200"},
        {FTP_REST, "REST", "350"},
        {FTP_PASV, "PASV", "227"},
        {FTP_RETR, "RETR", "150,125"},
        {FTP_LIST, "LIST", "150,125"},
        {FTP_CWD, "CWD", "250"},
        {FTP_PWD, "PWD", "257"},
        {FTP_DATA_TRANSFER, "FTP_DATA_TRANSFER", "226"},
        {FTP_STOR, "STOR", "150"},
        {FTP_ABOR, "ABOR", "426,225"},
        {FTP_PORT, "PORT", "200"},
        {FTP_QUIT, "QUIT", "221"},
};

static void ftp_dnsReq_callback(COS_EVENT *ev)
{
    uint8_t nDLCI = (uint8_t)ev->nParam3;
    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND)
    {
        ip_addr_t *ipaddr = (ip_addr_t *)ev->nParam1;
        api_FTPLogin(ipaddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd);
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, nDLCI);
    }
    else if (ev->nEventId == EV_CFW_DNS_RESOLV_ERR_IND)
    {
        AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLCI);
    }
    AT_FREE(ev);
}

static void ftp_FTPTimmerHandler(void *param)
{
    AT_TC(g_sw_GPRS, "FTP# ftp_FTPTimmerHandler\n");
    api_FTPLogout();
    gFTPCtx.ftpState = FTP_IDLE;
}

static void ftp_killFTPTimmer()
{
    AT_TC(g_sw_GPRS, "FTP# ftp_killFTPTimmer\n");
    if (gFTPCtx.bFTPTimer)
    {
        at_StopCallbackTimer(ftp_FTPTimmerHandler, NULL);
        gFTPCtx.bFTPTimer = false;
    }
}

static void ftp_setFTPTimmer()
{
    ftp_killFTPTimmer();
    at_StartCallbackTimer(gFTP_Param->Tout * 1000, ftp_FTPTimmerHandler, NULL);
    gFTPCtx.bFTPTimer = true;
    AT_TC(g_sw_GPRS, "FTP# ftp_setFTPTimmer\n");
}

static void ftp_setFTPIdle()
{
    gFTPCtx.ftpState = FTP_IDLE;
    ftp_setFTPTimmer();
}

static void ftp_commandComplete(uint8_t succ, uint8_t *data)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(gFTPCtx.nDLCI);
    if (succ == 0)
    {
        if (data)
            at_CmdRespInfoText(engine, data);
        at_CmdRespOK(engine);
    }
    else
    {
        if (data)
            at_CmdRespErrorText(engine, data);
        at_CmdRespError(engine);
    }
}

void ftp_Init(void)
{
    memset(&gFTPCtx, 0x00, sizeof(CFW_FTP_CTX_T));
    gFTPCtx.dataSocket = 0xFF;
    gFTPCtx.listenSocket = 0xFF;
    gFTPCtx.ftpState = FTP_LOGOUT;
    gFTPCtx.ctrlSocket = 0xFF;
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
    ftp_txbufferinit(&gFTPCtx.tx_buf, 0);
}

int32_t ftp_sendCommand(SOCKET nSocket, uint8_t *cmd)
{
    AT_TC(g_sw_GPRS, "FTP# FTP CMD:%s\n", cmd);
    strcat(cmd, "\r\n");
    return CFW_TcpipSocketSend(nSocket, cmd, strlen(cmd), 0);
}

static void ftp_msg_callback(void *param)
{
    COS_EVENT *pEvent = (COS_EVENT *)param;
    ftp_MainProcess(*pEvent);
    COS_FREE(pEvent);
    AT_ZERO_PARAM1(pEvent);
}

uint32_t ftp_ctrl_connect(CFW_FTP_CTX_T *ftp_ptr)
{
    uint32_t ret = 0;

#if LWIP_IPV4
    if (IP_IS_V4(&(ftp_ptr->serverIp)))
    {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&(ftp_ptr->ser_ip);
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        to4->sin_port = htons(ftp_ptr->port);
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(ftp_ptr->serverIp)));
        ftp_ptr->ctrlSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
    }
#endif
#if LWIP_IPV6
    if (IP_IS_V6(&(ftp_ptr->serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(ftp_ptr->serverIp))))
    {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(ftp_ptr->ser_ip);
        to6->sin6_len = sizeof(to6);
        to6->sin6_family = AF_INET6;
        to6->sin6_port = htons(ftp_ptr->port);
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(ftp_ptr->serverIp)));
        ftp_ptr->ctrlSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
    }
#endif /* LWIP_IPV6 */

    ret = CFW_TcpipSocketConnect(ftp_ptr->ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)&ftp_ptr->ser_ip, sizeof(ftp_ptr->ser_ip));
    ftp_ptr->ftp_command = FTP_CONNECT;
    return ret;
}

/* when download file, if we set offset use REST and then
 * use RETR to get file. the response of RETR and the file data
 * came not ordered. so when we get one of them we will report CONNECT
 */
static bool g_waitDataConnect = 0;
void ftpget_setWaitDataConnect()
{
    g_waitDataConnect = true;
}

bool ftpget_WaitDataConnect()
{
    return g_waitDataConnect;
}

void ftpget_clearWaitDataConnect()
{
    g_waitDataConnect = false;
}

void ftp_put_connect_finish()
{
    ftp_reportFTPState("^FTPPUT:1,3072");
    gFTPCtx.connected_socket = FTP_SOCKET_PUT;
}

static void ftp_get_connect_error()
{
    ftp_reportFTPState("^FTPGET:1,0");
    gFTPCtx.connected_socket = FTP_SOCKET_GET;
    ftp_get_disconnect();
}

void ftp_get_connect_finish()
{
    ftp_reportFTPState("^FTPGET:1,1");
    gFTPCtx.connected_socket = FTP_SOCKET_GET;
}

static uint8_t ftp_data_connected()
{
    return gFTPCtx.connected_socket;
}

void ftp_put_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

void ftp_put_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    ftp_reportFTPState("^FTPPUT:2,0");
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

void ftp_get_disconnect_finish()
{
    gFTPCtx.dataSocket = 0xFF;
    ftp_reportFTPState("^FTPGET:2,0");
    gFTPCtx.connected_socket = FTP_SOCKET_NULL;
}

static void ftp_get_disconnect()
{
    CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    gFTPCtx.ftp_command = FTP_ABOR;
}

int32_t ftp_get_connect(uint8_t *filename)
{
    uint8_t command[200] = {
        0,
    };
    strcat(command, "RETR ");
    strcat(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
    ftpget_setWaitDataConnect();
    gFTPCtx.ftp_command = FTP_RETR;
    return 0;
}

static int32_t ftp_ctrl_connected()
{
    return gFTPCtx.connect;
}

static int32_t ftp_set_ctrl_connect(bool connect)
{
    return gFTPCtx.connect = connect;
}

int32_t ftp_size(uint8_t *filename)
{
    uint8_t command[200] = {
        0,
    };
    strcat(command, "SIZE ");
    strcat(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
    gFTPCtx.ftp_command = FTP_SIZE;
    return 0;
}

static bool ftp_GetStart(CFW_FTP_CTX_T *ctx)
{
    if (ctx->getpath == NULL)
        return false;
    if (ftp_data_connected() != FTP_SOCKET_NULL)
        return false;
    uint8_t command[200] = {
        0,
    };
    strcat(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, (char *)command);
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = GET_FILE;
    return true;
}

static int32_t ftp_PutStart(CFW_FTP_CTX_T *ctx)
{
    uint8_t command[200] = {
        0,
    };

    if (ctx->putpath == NULL)
        return false;
    if (ftp_data_connected() != FTP_SOCKET_NULL)
        return false;
    ctx->ftp_command = FTP_TYPE;
    ctx->ftpState = PUT_FILE;
    strcat(command, "TYPE I");
    ftp_sendCommand(ctx->ctrlSocket, (char *)command);
    return true;
}

int32_t ftp_isFTPTransferingMux(uint8_t nDLCI)
{
    return (gFTPCtx.ftpState == FTP_WAIT_DATA) && (nDLCI == gFTPCtx.nDLCI);
}

int32_t ftp_isFTPTransfering(void)
{
    return gFTPCtx.ftpState == FTP_WAIT_DATA;
}

int32_t ftp_put_connect(uint8_t *filename)
{
    uint8_t command[200] = {
        0,
    };
    strcat(command, "STOR ");
    strcat(command, filename);
    ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
    gFTPCtx.ftp_command = FTP_STOR;
    return 0;
}

uint8_t ftp_listenPort(struct sockaddr_storage *localaddr)
{
    uint32_t ret;
    INT32 addrlen = sizeof(struct sockaddr_storage);
#if LWIP_IPV4
    if (IP_IS_V4(&(gFTPCtx.serverIp)))
    {
        struct sockaddr_in *addr = (struct sockaddr_in *)localaddr;
        gFTPCtx.listenSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
        CFW_TcpipSocketGetsockname(gFTPCtx.ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        addr->sin_port = 0;
        ret = CFW_TcpipSocketBind(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, sizeof(struct sockaddr_in));
        CFW_TcpipSocketGetsockname(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        CFW_TcpipSocketListen(gFTPCtx.listenSocket, 0);
        AT_TC(g_sw_GPRS, "FTP# FTP local addr=%s,port:%d", inet_ntoa(addr->sin_addr), addr->sin_port);
        return 0;
    }
#endif
#if LWIP_IPV6
    if (IP_IS_V6(&(gFTPCtx.serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(gFTPCtx.serverIp))))
    {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)localaddr;
        gFTPCtx.listenSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
        CFW_TcpipSocketGetsockname(gFTPCtx.ctrlSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        addr->sin6_port = 0;
        ret = CFW_TcpipSocketBind(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, sizeof(struct sockaddr_in6));
        CFW_TcpipSocketGetsockname(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)addr, &addrlen);
        CFW_TcpipSocketListen(gFTPCtx.listenSocket, 0);
        AT_TC(g_sw_GPRS, "FTP# FTP local addr=%s,port:%d", inet6_ntoa(addr->sin6_addr), addr->sin6_port);
        return 1;
    }
#endif /* LWIP_IPV6 */

    return 0;
}

static int32_t ftp_get_data_ind()
{
    uint8_t buffer[FTP_BUFFER_LEN] = {
        0x00,
    };
    uint32_t output = 0;

    uint32_t rec_len = CFW_TcpipSocketRecv(gFTPCtx.dataSocket, buffer, FTP_BUFFER_LEN, 0);
    AT_TC(g_sw_GPRS, "FTP# rec_len =%d", rec_len);
    if (rec_len > 0)
    {
        if (gFTPCtx.req_getsize > 0)
        {
            if (gFTPCtx.req_getsize >= (gFTPCtx.getsize + rec_len))
                output = rec_len;
            else
                output = gFTPCtx.req_getsize - gFTPCtx.getsize;

            gFTPCtx.getsize += output;

            if (gFTPCtx.getsize == gFTPCtx.req_getsize)
                ftp_get_disconnect();
        }
        else
            output = rec_len;

        AT_TC(g_sw_GPRS, "FTP# output=%d", output);
        uint8_t rsp[50] = {
            0,
        };
        AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(gFTPCtx.nDLCI);
        AT_Sprintf(rsp, "^FTPGET:2,%d", output);
        at_CmdRespInfoText(engine, rsp);
        at_CmdWrite(engine, buffer, output);
    }
    else if (rec_len == 0)
        AT_TC(g_sw_GPRS, "FTP# rec_len == 0, ignore");
    else
    {
        AT_TC(g_sw_GPRS, "FTP# rec_len < 0, transfer complete, abort transefer");
        ftp_get_disconnect();
    }
    return 0;
}

uint8_t ftp_errorHandler(enum FTP_COMMAND cmd)
{
    AT_TC(g_sw_GPRS, "FTP# ftp_errorHandler cmd=%d\n", cmd);

    switch (cmd)
    {
    case FTP_USER:
    case FTP_PASS:
        ftp_commandComplete(1, NULL);
        if (ftp_ctrl_connected())
        {
            CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        }

        break;
    case FTP_RETR:
        AT_TC(g_sw_GPRS, "FTP# error ftp_data_connected()=%d", ftp_data_connected());
        if (ftpget_WaitDataConnect())
            ftpget_clearWaitDataConnect();
        ftp_get_connect_error();
        break;
    default:
        ftp_commandComplete(1, NULL);
        ftp_setFTPIdle();
        break;
    }
    return 0;
}

static void ftp_setOffset(uint16_t offset)
{
    AT_TC(g_sw_GPRS, "FTP# ftp_setOffset");
    uint8_t command[200] = {
        0,
    };

    AT_Sprintf(command, "REST %d", offset);
    ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
    gFTPCtx.ftp_command = FTP_REST;
}

static int32_t ftp_ctrl_data_ind(void)
{
    uint32_t recv_num = 0;
    uint8_t command[200] = {
        0,
    };
    uint8_t *ptr1;
    uint8_t *ptr2;
    uint16_t i;
    uint16_t j;
    uint8_t numBuf[8] = {
        0,
    };
    uint8_t gRecBuf[1024] = {
        0,
    };

    recv_num = CFW_TcpipSocketRecv(gFTPCtx.ctrlSocket, (char *)gRecBuf, 1024, 0);
    if (recv_num < 3)
    {
        AT_TC(g_sw_GPRS, "FTP# warning!!! ctrl recv < 3 byte");
        return 0;
    }

    AT_TC(g_sw_GPRS, "FTP# REP:%s", gRecBuf);
    if ((strncmp(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst, 3) == 0) || (strncmp(gRecBuf, ftp_cmd[gFTPCtx.ftp_command - 1].cmd_exp_rst + 4, 3) == 0))
    {
        AT_TC(g_sw_GPRS, "FTP# CMD RSP OK");
    }
    else if (strncmp(gRecBuf, "226", 3) == 0)
    {
        /* trans finished report to ap */
        if (ftp_put_tx_work(&gFTPCtx.tx_buf))
        {
            ftp_put_tx_work_stop(&gFTPCtx.tx_buf);
            ftp_put_disconnect_finish();
        }
    }
    else
    {
        AT_TC(g_sw_GPRS, "FTP# CMD RSP ERROR rec=%d !!!", recv_num);
        SXS_DUMP(_CSW, "%2x", gRecBuf, recv_num);
        return ftp_errorHandler(gFTPCtx.ftp_command);
    }

    switch (gFTPCtx.ftp_command)
    {
    case FTP_CONNECT:
        strcat(command, "USER ");
        strcat(command, gFTPCtx.userName);
        ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
        gFTPCtx.ftp_command = FTP_USER;
        break;
    case FTP_USER:
        strcat(command, "PASS ");
        strcat(command, gFTPCtx.passWord);
        ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
        gFTPCtx.ftp_command = FTP_PASS;
        break;

    case FTP_PASS:
        gFTPCtx.logined = true;
        /* FTPOPEN */
        ftp_commandComplete(0, NULL);
        gFTPCtx.ftpState = FTP_LOGIN;
        ftp_setFTPIdle();
        break;
    case FTP_STOR:
        AT_TC(g_sw_GPRS, "FTP# FTP_STOR\n");
        gFTPCtx.ftpState = FTP_WAIT_DATA;
        ftp_killFTPTimmer();
        gFTPCtx.ftp_command = FTP_DATA_TRANSFER;
        ftp_put_connect_finish();
        break;

    case FTP_SIZE:
        AT_TC(g_sw_GPRS, "FTP# FTP_SIZE END\n");
        uint8_t ftpsize[50] = {
            0,
        };
        AT_Sprintf(ftpsize, "^FTPSIZE:%s", gRecBuf + 4);
        /* FTPSIZE */
        ftp_commandComplete(0, ftpsize);
        ftp_setFTPIdle();
        break;

    case FTP_TYPE:
        if ((gFTPCtx.ftpState == GET_FILE) || (gFTPCtx.ftpState == PUT_FILE))
        {
            if (gFTP_Param->mode == 1)
            {
                strcat(command, "PASV ");
                ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
                gFTPCtx.ftp_command = FTP_PASV;
            }
            else
            {
                struct sockaddr_storage localaddr;
                uint8_t *p = NULL;
                uint8_t isIpv6 = 0;
                isIpv6 = ftp_listenPort(&localaddr);
                if (!isIpv6)
                {
                    struct sockaddr_in *addr_in = (struct sockaddr_in *)&localaddr;
                    AT_Sprintf(command, "PORT %s,%d,%d",
                               inet_ntoa(addr_in->sin_addr),
                               addr_in->sin_port % 256, addr_in->sin_port / 256);
                }
                else
                {
                    struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *)&localaddr;
                    AT_Sprintf(command, "PORT %s,%d,%d",
                               inet_ntoa(addr_in->sin6_addr),
                               addr_in->sin6_port % 256, addr_in->sin6_port / 256);
                }

                p = command + 6;
                while (*p != '\0')
                {
                    if (*p == '.')
                        *p = ',';
                    p++;
                }

                ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
                gFTPCtx.ftp_command = FTP_PORT;
            }
        }
        break;

    case FTP_ABOR:
        AT_TC(g_sw_GPRS, "FTP# FTP_ABOR END\n");
        /* ignore reponse OK in data socket close response */
        //ftp_commandComplete(0, NULL);
        ftp_setFTPIdle();
        break;

    case FTP_REST:
        ftp_TransferStart();
        break;

    case FTP_PASV:
        ptr1 = gRecBuf + strlen(gRecBuf);
        while (*(ptr1) != ')')
        {
            ptr1--;
        }
        ptr2 = ptr1;

        while (*(ptr2) != ',')
        {
            ptr2--;
        }
        memset(numBuf, 0x00, 8);
        strncpy(numBuf, ptr2 + 1, ptr1 - ptr2 - 1);
        AT_TC(g_sw_GPRS, "FTP#  pasv receive:   %s \n\r", numBuf);

        j = atoi(numBuf);

        ptr2--;
        ptr1 = ptr2;
        ptr2++;

        while (*(ptr1) != ',')
        {
            ptr1--;
        }
        memset(numBuf, 0x00, 8);

        strncpy(numBuf, ptr1 + 1, ptr2 - ptr1 - 1);
        AT_TC(g_sw_GPRS, "FTP#  pasv receive:   %s \n\r", numBuf);

        i = atoi(numBuf);
        AT_TC(g_sw_GPRS, "FTP#  pasv receive:   %s      j: %d, i: %d\n\r", gRecBuf, i, j);

        gFTPCtx.data_port = i * 256 + j;

#if LWIP_IPV4
        if (IP_IS_V4(&(gFTPCtx.serverIp)))
        {
            struct sockaddr_in *to4 = (struct sockaddr_in *)&(gFTPCtx.ser_ip_data);
            to4->sin_len = sizeof(to4);
            to4->sin_family = AF_INET;
            to4->sin_port = htons(gFTPCtx.data_port);
            inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&(gFTPCtx.serverIp)));
            gFTPCtx.dataSocket = CFW_TcpipSocketEX(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
        }
#endif
#if LWIP_IPV6
        if (IP_IS_V6(&(gFTPCtx.serverIp)) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&(gFTPCtx.serverIp))))
        {
            struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&(gFTPCtx.ser_ip_data);
            to6->sin6_len = sizeof(to6);
            to6->sin6_family = AF_INET6;
            to6->sin6_port = htons(gFTPCtx.data_port);
            inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&(gFTPCtx.serverIp)));
            gFTPCtx.dataSocket = CFW_TcpipSocketEX(AF_INET6, CFW_TCPIP_SOCK_STREAM, 0, ftp_msg_callback, NULL);
        }
#endif /* LWIP_IPV6 */

        CFW_TcpipSocketConnect(gFTPCtx.dataSocket, (CFW_TCPIP_SOCKET_ADDR *)&gFTPCtx.ser_ip_data, sizeof(gFTPCtx.ser_ip_data));
        break;

    case FTP_PORT:
        AT_TC(g_sw_GPRS, "FTP# FTP PORT command complete");
        if (gFTPCtx.req_getoffset > 0)
            ftp_setOffset(gFTPCtx.req_getoffset);
        else
            ftp_TransferStart();

        break;
    case FTP_RETR:
        gFTPCtx.ftp_command = FTP_DATA_TRANSFER;
        gFTPCtx.ftpState = FTP_WAIT_DATA;
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_finish();
            ftpget_clearWaitDataConnect();
        }
        ftp_killFTPTimmer();
        break;

    case FTP_DATA_TRANSFER:
        ftp_setFTPIdle();
        break;
    case FTP_QUIT:
        /* report OK until ctrl socket close response, ignore this response */
        break;
    default:
        ftp_setFTPIdle();
        ftp_commandComplete(1, NULL);
        break;
    }
    return 0;
}

bool ftp_isHandleEvent(COS_EVENT ev)
{
    if ((EV_CFW_TCPIP_REV_DATA_IND <= ev.nEventId) && (ev.nEventId <= EV_CFW_TCPIP_ACCEPT_IND))
    {
        return true;
    }
    else if ((EV_CFW_TCPIP_SOCKET_CONNECT_RSP <= ev.nEventId) && (ev.nEventId <= EV_CFW_TCPIP_SOCKET_SEND_RSP))
    {
        return true;
    }
    else
        return false;
}

static void ftp_TransferStart()
{
    AT_TC(g_sw_GPRS, "FTP# ftp_TransferStart");
    if (GET_FILE == gFTPCtx.ftpState)
        ftp_get_connect(gFTPCtx.getpath);
    else if (PUT_FILE == gFTPCtx.ftpState)
        ftp_put_connect(gFTPCtx.putpath);
    gFTPCtx.ftpState = FTP_WAIT_DATA;
    ftp_killFTPTimmer();
}

int32_t ftp_data_msg_handler(COS_EVENT ev)
{
    if (EV_CFW_TCPIP_SOCKET_CONNECT_RSP == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_SOCKET_CONNECT_RSP data\n");
        if (gFTPCtx.req_getoffset > 0)
            ftp_setOffset(gFTPCtx.req_getoffset);
        else
            ftp_TransferStart();
    }
    else if (EV_CFW_TCPIP_REV_DATA_IND == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_REV_DATA_IND data\n");
        if (ftpget_WaitDataConnect())
        {
            ftp_get_connect_finish();
            ftpget_clearWaitDataConnect();
        }
        ftp_get_data_ind();
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP data exit data mode\n");
        /* exit data mode */
        gFTPCtx.ftpState = FTP_IDLE;
        if (AT_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, gFTPCtx.nDLCI))
        {
            ftp_commandComplete(0, NULL);
            AT_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, gFTPCtx.nDLCI);
            /* FTPCLOSE */
            ftp_setFTPIdle();
        }
        if (ftp_data_connected() == FTP_SOCKET_GET)
            ftp_get_disconnect_finish();
        else if (ftp_data_connected() == FTP_SOCKET_PUT)
        {
            if (!ftp_put_tx_work(&gFTPCtx.tx_buf))
                ftp_put_disconnect_finish();
        }
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.nEventId)
    {
        /* get file complete */
        //ftp_commandComplete(0, NULL);
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_CLOSE_IND data\n");
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    }
    return 0;
}

int32_t ftp_ctrl_msg_handler(COS_EVENT ev)
{
    if (EV_CFW_TCPIP_SOCKET_CONNECT_RSP == ev.nEventId)
    {
        ftp_set_ctrl_connect(true);
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_SOCKET_CONNECT_RSP ctrl\n");
    }
    else if (EV_CFW_TCPIP_REV_DATA_IND == ev.nEventId)
    {
        ftp_ctrl_data_ind();
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP ctrl\n");
        gFTPCtx.ctrlSocket = 0xFF;
        gFTPCtx.ftpState = FTP_LOGOUT;
        if (AT_isWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, gFTPCtx.nDLCI))
        {
            ftp_commandComplete(0, NULL);
            AT_DelWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nSimID, gFTPCtx.nDLCI);
        }
        ftp_reportFTPState("^URCFTP:0");
        ftp_killFTPTimmer();
        ftp_set_ctrl_connect(false);
    }
    else if (EV_CFW_TCPIP_CLOSE_IND == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_CLOSE_IND ctrl\n");
        CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
        ftp_killFTPTimmer();
        gFTPCtx.ftpState = FTP_LOGOUT;
    }
    else
        AT_TC(g_sw_GPRS, "FTP# event=%d", ev.nEventId);
    return 0;
}

int32_t ftp_listen_msg_handler(COS_EVENT ev)
{
    if (EV_CFW_TCPIP_ACCEPT_IND == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_ACCEPT_IND listen\n");

        CFW_TCPIP_SOCKET_ADDR from;
        int32_t addrlen = sizeof(CFW_TCPIP_SOCKET_ADDR);
        gFTPCtx.dataSocket = CFW_TcpipSocketAccept(gFTPCtx.listenSocket, (CFW_TCPIP_SOCKET_ADDR *)&from, (UINT32 *)&addrlen);
        CFW_TcpipSocketClose(gFTPCtx.listenSocket);
    }
    else if (EV_CFW_TCPIP_SOCKET_CLOSE_RSP == ev.nEventId)
    {
        AT_TC(g_sw_GPRS, "FTP# EV_CFW_TCPIP_SOCKET_CLOSE_RSP listen\n");
        gFTPCtx.listenSocket = 0xFF;
    }
    return 0;
}

void ftp_MainProcess(COS_EVENT ev)
{
    if (!ftp_isHandleEvent(ev))
    {
        AT_TC(g_sw_GPRS, "FTP# invalid event=%d", ev.nEventId);
        return;
    }

    SOCKET sock_id = ev.nParam1;
    if (sock_id == gFTPCtx.dataSocket)
        ftp_data_msg_handler(ev);
    else if (sock_id == gFTPCtx.ctrlSocket)
        ftp_ctrl_msg_handler(ev);
    else if (sock_id == gFTPCtx.listenSocket)
        ftp_listen_msg_handler(ev);
    else
        return;
}

int32_t api_FTPLogin(ip_addr_t *serIp, uint16_t serPort, uint8_t *user, uint8_t *pswd)
{

    if (gFTPCtx.logined)
        return ERR_DEVICE_NOT_FOUND;
    strcpy(gFTPCtx.userName, user);
    strcpy(gFTPCtx.passWord, pswd);
    gFTPCtx.serverIp = *serIp;
    gFTPCtx.port = serPort;
    ftp_ctrl_connect(&gFTPCtx);
    return ERR_SUCCESS;
}

int32_t api_FTPLogout()
{

    uint8_t command[200] = {
        0,
    };
    strcat(command, "QUIT");
    ftp_sendCommand(gFTPCtx.ctrlSocket, (char *)command);
    if (gFTPCtx.dataSocket != 0xFF)
        CFW_TcpipSocketClose(gFTPCtx.dataSocket);
    CFW_TcpipSocketClose(gFTPCtx.ctrlSocket);
    gFTPCtx.ftp_command = FTP_QUIT;
    gFTPCtx.ftpState = FTP_LOGOUT;
    return ERR_SUCCESS;
}

uint8_t ftp_getChannel(void)
{
    return gFTPCtx.nDLCI;
}

void ftp_setChannel(uint8_t nDLCI)
{
    gFTPCtx.nDLCI = nDLCI;
}

char *ftp_strstr(const char *s1, const char *s2)
{
    uint8_t len2;
    if (!(len2 = AT_StrLen(s2))) // 此种情况下s2不能指向空，否则strlen无法测出长度，这条语句错误
        return (char *)s1;

    for (; *s1; ++s1)
    {
        if (*s1 == *s2 && AT_StrNCmp(s1, s2, len2))
            return (char *)s1;
    }
    return NULL;
}

struct FTP_Param *ftp_getFTPParam()
{
    return gFTP_Param;
}

static bool ftp_setPUTINFO(CFW_FTP_CTX_T *ctx, const uint8_t *path, uint16_t length)
{
    if (ctx->putpath != NULL)
        AT_FREE(ctx->putpath);

    ctx->putpath = (uint8_t *)AT_MALLOC(length + 1);
    if (ctx->putpath == NULL)
    {
        AT_TC(g_sw_GPRS, "FTP# malloc failed");
        return false;
    }
    memset(ctx->putpath, 0x00, length + 1);
    strncpy(ctx->putpath, path, length);
    AT_TC(g_sw_GPRS, "FTP# ftp_setPUTINFO %s", path);
    return true;
}

static bool ftp_setGETINFO(CFW_FTP_CTX_T *ctx, const uint8_t *path, uint16_t length, uint32 offset, uint32_t size)
{
    if (length > 255)
        return false;

    if (ctx->getpath != NULL)
        AT_FREE(ctx->getpath);

    ctx->getpath = (uint8_t *)AT_MALLOC(length + 1);
    if (ctx->getpath == NULL)
    {
        AT_TC(g_sw_GPRS, "FTP# malloc failed");
        return false;
    }
    memset(ctx->getpath, 0x00, length + 1);
    strncpy(ctx->getpath, path, length);
    ctx->req_getoffset = offset;
    ctx->req_getsize = size;
    ctx->getsize = 0;
    AT_TC(g_sw_GPRS, "FTP# ftp_setGETINFO path=%s, offset=%d,size=%d", path, offset, size);
    return true;
}

static void ftp_gethostport(uint8_t *url, uint8_t *host, uint16_t *port)
{
    uint8_t *p;
    ip_addr_t addr;

    if (ipaddr_aton(url, &addr))
    {
        if (IP_IS_V6(&addr))
        {
            strcpy(host, url);
            *port = 21;
        }
    }
    else
    {
        p = strstr(url, ":");
        if (p != NULL)
        {
            strncpy(host, url, p - url);
            *port = atoi(p + 1);
        }
        else
        {
            strcpy(host, url);
            *port = 21;
        }
    }
}

void AT_GPRS_CmdFunc_FTPOPEN(AT_CMD_PARA *pParam)
{
    uint8_t uParamCount = 0;
    uint32_t err;
    int32_t iResult = 0;
    uint8_t tmpString[60] = {
        0x00,
    };
    uint8_t url[FTP_STRING_SIZE] = {
        0,
    };
    uint8_t idx = 0;
    uint16_t uSize = 0;
    ip_addr_t nIpAddr;
    struct FTP_Param ftp_param = {{}, 0, {}, {}, 1, 30, 0};
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    AT_Gprs_CidInfo *nStaAtGprsCidInfo = g_staAtGprsCidInfo_e[nSim];

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        if (ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 6))
        {
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        uSize = FTP_STRING_SIZE;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &url, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get url error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        ftp_gethostport(url, ftp_param.host, &ftp_param.port);
        idx++;
        uSize = FTP_STRING_SIZE;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &ftp_param.username, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get username error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        idx++;
        uSize = FTP_STRING_SIZE;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_STRING, &ftp_param.passwd, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get password error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        idx++;
        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &ftp_param.mode, &uSize);
        if (iResult != ERR_SUCCESS || (ftp_param.mode != 0 && ftp_param.mode != 1))
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get mode error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        idx++;
        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &ftp_param.Tout, &uSize);
        if (iResult != ERR_SUCCESS || (ftp_param.Tout < 5 || ftp_param.Tout > 180))
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get timeout error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        idx++;
        uSize = 1;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, idx, AT_UTIL_PARA_TYPE_UINT8, &ftp_param.FTPtype, &uSize);
        if (iResult != ERR_SUCCESS || (ftp_param.FTPtype != 0 && ftp_param.FTPtype != 1))
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN get ftp type error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (gFTP_Param == NULL)
        {
            gFTP_Param = (struct FTP_Param *)AT_MALLOC(sizeof(struct FTP_Param));
        }

        if (gFTPCtx.ftpState != FTP_LOGOUT)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN ftp have login");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_MemCpy(gFTP_Param, &ftp_param, sizeof(struct FTP_Param));
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %s", gFTP_Param->host);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %d", gFTP_Param->port);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %s", gFTP_Param->username);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %s", gFTP_Param->passwd);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %d", gFTP_Param->mode);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %d", gFTP_Param->Tout);
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN %d", gFTP_Param->FTPtype);

        ftp_Init();
        gFTPCtx.nSimID = nSim;
        ftp_setChannel(pParam->nDLCI);
        err = CFW_GethostbynameEX(gFTP_Param->host, &nIpAddr, nStaAtGprsCidInfo[0].uCid, nSim, (COS_CALLBACK_FUNC_T)ftp_dnsReq_callback, NULL);
        if (err == RESOLV_QUERY_INVALID)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPOPEN gethost by name error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
        else if (err == RESOLV_COMPLETE)
        {
            api_FTPLogin(&nIpAddr, gFTP_Param->port, gFTP_Param->username, gFTP_Param->passwd);
            AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else if (err == RESOLV_QUERY_QUEUED)
        {
            AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
    }
    break;
    case AT_CMD_READ:
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN READ");
        if (gFTPCtx.ftpState == FTP_LOGOUT)
        {
            strcpy(tmpString, "^FTPOPEN:0");
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI, nSim);
        }
        else
        {
            strcpy(tmpString, "^FTPOPEN:1");
            AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI, nSim);
        }

        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "^FTPOPEN:<url>,<username>,<password>,<mode>,<tout>,<type>");
        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, tmpString, AT_StrLen(tmpString), pParam->nDLCI, nSim);
        break;
    default:
        AT_TC(g_sw_GPRS, "FTP# FTPOPEN ERROR");
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
}

void AT_GPRS_CmdFunc_FTPSIZE(AT_CMD_PARA *pParam)
{
    uint8_t uParamCount = 0;
    int32_t iResult = 0;
    uint16_t uSize = 0;
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    uint8_t filename[FTP_STRING_SIZE] = {
        0x00,
    };

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (!ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        iResult = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
        if ((iResult != ERR_SUCCESS) || (uParamCount != 1))
        {
            AT_TC(g_sw_GPRS, "FTP# FTPSIZE get param count error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        uSize = FTP_STRING_SIZE;
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &filename, &uSize);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "FTP# FTPSIZE get filename error");
            AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        ftp_size(filename);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    default:
        AT_TC(g_sw_GPRS, "FTP# FTPSIZE ERROR");
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
}

void AT_GPRS_CmdFunc_FTPGETSET(AT_CMD_PARA *pParam)
{
    uint32_t offset = 0;
    uint32_t size = 0;

    if (AT_CMD_SET == pParam->iType)
    {
        if (!ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        bool paramok = true;
        const uint8_t *filepath = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (pParam->paramCount >= 2)
            offset = at_ParamUint(pParam->params[1], &paramok);
        if (pParam->paramCount >= 3)
            size = at_ParamUint(pParam->params[2], &paramok);

        if (ftp_setGETINFO(&gFTPCtx, filepath, strlen(filepath), offset, size))
            at_CmdRespOK(pParam->engine);
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        uint8_t info[50] = {
            0x00,
        };
        if (gFTPCtx.getpath != NULL)
        {
            AT_Sprintf(info, "^FTPGETSET:%s,%d,%d", gFTPCtx.getpath,
                       gFTPCtx.req_getoffset, gFTPCtx.req_getsize);
            at_CmdRespInfoText(pParam->engine, info);
            at_CmdRespOK(pParam->engine);
        }
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "^FTPGETSET:<filename>, [offset, [size]]");
        at_CmdRespOK(pParam->engine);
        return;
    }
    return;
}

void AT_GPRS_CmdFunc_FTPGET(AT_CMD_PARA *pParam)
{
    bool paramok = true;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (!ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (pParam->paramCount > 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        uint32_t mode = at_ParamUint(pParam->params[0], &paramok);
        if (mode == 1)
        {
            if (ftp_GetStart(&gFTPCtx))
                at_CmdRespOK(pParam->engine);
            else
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        else if (mode == 2)
        {
            uint32_t reqlength = 0;
            if (pParam->paramCount >= 2)
                reqlength = at_ParamUint(pParam->params[1], &paramok);
            else
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

            if (!paramok)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

            if (reqlength == 0)
            {
                if (ftp_data_connected() == FTP_SOCKET_GET)
                {
                    AT_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nDLCI, gFTPCtx.nSimID);
                    ftp_get_disconnect();
                }
                else
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            else
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        break;
    default:
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
}

void AT_GPRS_CmdFunc_FTPPUTSET(AT_CMD_PARA *pParam)
{

    if (gFTPCtx.ftpState != FTP_IDLE || gFTPCtx.nDLCI != pParam->nDLCI)
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    if (AT_CMD_SET == pParam->iType)
    {
        if (!ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        const uint8_t *filepath = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (ftp_setPUTINFO(&gFTPCtx, filepath, strlen(filepath)))
            at_CmdRespOK(pParam->engine);
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        uint8_t info[50] = {
            0x00,
        };
        if (gFTPCtx.putpath != NULL)
        {
            AT_Sprintf(info, "^FTPPUTSET:%s", gFTPCtx.putpath);
            at_CmdRespInfoText(pParam->engine, info);
            at_CmdRespOK(pParam->engine);
        }
        else
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespInfoText(pParam->engine, "^FTPPUTSET:<filename>");
        at_CmdRespOK(pParam->engine);
        return;
    }
    return;
}

void AT_GPRS_CmdFunc_FTPPUT(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    AT_TC(g_sw_GPRS, "FTP# in FTPPUT gFTPCtx.ftpState %d", gFTPCtx.ftpState);

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (!ftp_ctrl_connected())
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (pParam->paramCount > 2)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        uint32_t mode = at_ParamUint(pParam->params[0], &paramok);
        if (mode == 1)
        {
            AT_TC(g_sw_GPRS, "FTP# mode=%d", mode);
            if (ftp_PutStart(&gFTPCtx))
                at_CmdRespOK(pParam->engine);
            else
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        else if (mode == 2)
        {
            uint32_t req_length = at_ParamUintInRange(pParam->params[1], 0, FTP_SEND_MAX_SIZE, &paramok);

            if (!paramok)
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            if (req_length == 0)
            {
                if (ftp_data_connected() == FTP_SOCKET_PUT)
                {
                    AT_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nDLCI, gFTPCtx.nSimID);
                    ftp_put_disconnect();
                    return;
                }
                else
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            ftp_txbufferinit(&gFTPCtx.tx_buf, req_length);
            ftp_data_mode_start(pParam->nDLCI);
        }
        break;
    case AT_CMD_READ:
        at_CmdRespOK(pParam->engine);
        break;
    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, "^FTPPUT: mode[,<reqlength>]");
        at_CmdRespOK(pParam->engine);
        break;
    default:
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
}

void AT_GPRS_CmdFunc_FTPCLOSE(AT_CMD_PARA *pParam)
{
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);

    if (gFTPCtx.ftpState == FTP_LOGOUT || gFTPCtx.nDLCI != pParam->nDLCI)
    {
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        api_FTPLogout();
        AT_AddWaitingEvent(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, gFTPCtx.nDLCI, gFTPCtx.nSimID);
        AT_GPRS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        break;
    default:
        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
}

#endif
