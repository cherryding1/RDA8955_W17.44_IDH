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

#ifndef __AT_FTP_H__
#define __AT_FTP_H__

#include "at.h"
#include "sockets.h"
#define FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(_start, _end, _cycle) ((_start <= _end) ? (_end - _start) : (_end + _cycle - _start))
#define FTP_MOD_BUFF_LEN(_val, _length) ((_val) & (_length - 1))
/* current not use this buf, feature maybe remove it */
//#define FTP_RXBUFFER_LEN   (1024*500)
#define FTP_RXBUFFER_LEN (1)
#define FTP_GET_TIMEOUT_DUR 360
#define FTP_STRING_SIZE 256
#define FTP_SEND_MAX_SIZE 3072

enum FTP_COMMAND
{
    FTP_CONNECT = 1,
    FTP_USER,
    FTP_PASS,
    FTP_SIZE,
    FTP_TYPE,
    FTP_REST,
    FTP_PASV,
    FTP_RETR,
    FTP_LIST,
    FTP_CWD,
    FTP_PWD,
    FTP_DATA_TRANSFER,
    FTP_STOR,
    FTP_ABOR,
    FTP_PORT,
    FTP_QUIT,
};

enum FTP_STATE
{
    FTP_IDLE,
    CHANGEING_CUR_DIR,
    GET_FILE,
    PUT_FILE,
    LIST_DIR,
    LIST_FILE,
    FTP_LOGIN,
    FTP_LOGOUT,
    FTP_TRANSFER,
    FTP_WAIT_DATA,
    FTP_WAIT_DATA_PAUSE,
};

typedef struct
{
    uint8_t buffer[FTP_SEND_MAX_SIZE];
    uint16_t len;
    uint16_t sended;
    uint16_t request;
    uint16_t tx_work;
} FTP_TXBUFFER_T;

typedef enum FTP_SOCKET_CLOSE {
    FTP_SOCKET_NULL,
    FTP_SOCKET_GET,
    FTP_SOCKET_PUT,
} FTP_SOCKET_ID_T;

typedef struct _CFW_FTP_CTX
{
    enum FTP_STATE ftpState;
    enum FTP_COMMAND ftp_command;
    SOCKET ctrlSocket, dataSocket, listenSocket;
    UINT8 userName[50];
    UINT8 passWord[50];
    struct sockaddr_storage ser_ip;
    struct sockaddr_storage ser_ip_data;
    ip_addr_t serverIp;
    UINT8 *getpath;
    uint32_t req_getoffset, req_getsize, getsize;
    UINT8 *putpath;
    FTP_TXBUFFER_T tx_buf;
    bool connect;
    FTP_SOCKET_ID_T connected_socket;
    UINT16 port;
    UINT16 data_port;
    BOOL logined;
    BOOL bFTPTimer;
    UINT8 nDLCI;
    UINT8 nSimID;
} CFW_FTP_CTX_T;

typedef struct _CFW_FTP_CMD
{
    enum FTP_COMMAND cmd_id;
    UINT8 cmd_str[30];
    UINT8 cmd_exp_rst[30];
} CFW_FTP_CMD;

typedef struct
{
    UINT8 CircuBuf[FTP_RXBUFFER_LEN];
    UINT32 Buf_len;
    UINT32 Get;
    UINT32 Put;
    UINT8 NotEmpty;
} FTP_RXBUFF_T;

struct FTP_Param
{
    UINT8 host[FTP_STRING_SIZE];
    UINT16 port;
    UINT8 username[FTP_STRING_SIZE];
    UINT8 passwd[FTP_STRING_SIZE];
    UINT8 mode;
    UINT8 Tout;
    UINT8 FTPtype;
};

VOID AT_GPRS_CmdFunc_FTPOPEN(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPCLOSE(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPSIZE(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPGET(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPPUT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPPUTSET(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_FTPGETSET(AT_CMD_PARA *pParam);

#endif /* __CFW_FTP_H__ */
#endif
