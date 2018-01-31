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

#ifdef AT_HTTP_SUPPORT

#ifndef __AT_HTTP_H__
#define __AT_HTTP_H__

#include "at.h"
#include "sockets.h"
#define FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(_start,_end,_cycle) ((_start<=_end)?(_end-_start):(_end+_cycle - _start) )
#define FTP_MOD_BUFF_LEN(_val, _length)  ((_val) & (_length-1))
#define HTTP_RXBUFFER_LEN   (1024*500)
#define HTTP_SEND_MAX 65534

typedef struct
{
    UINT8 CircuBuf[HTTP_RXBUFFER_LEN];
    long Buf_len;
    long Get;
    long Put;
    UINT8 NotEmpty;
} HTTP_RXBUFF_T;



INT32 http_putDataToCircularBuffer( UINT8 *pBuff, long uDataSize);
INT32 http_getDataFromCircularBuffer( UINT8 *pBuff, UINT16 uDataSize);
INT32 Http_downLoadDataToPC(char *content,long contentLen);

#endif /* __CFW_FTP_H__ */
#endif
