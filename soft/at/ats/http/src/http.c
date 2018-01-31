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
#include <csw.h>

#include "sxs_io.h"

#include "string.h"

#include "http.h"

INT32 gFileDataRrecvd = 0;

HTTP_RXBUFF_T Http_gFileData ={{0,},HTTP_RXBUFFER_LEN,0,0,0};

/*Http_gFileData.Buf_len = HTTP_RXBUFFER_LEN;

Http_gFileData.Get = 0;

Http_gFileData.Put = 0;*/


INT32 http_putDataToCircularBuffer( UINT8 *pBuff, long uDataSize)
{
    INT32 Block_Len, Remain_Len, real_Len, First_half, Second_half;
    INT32 ret = 0;

    AT_TC(g_sw_GPRS, "HTTP PUT# !!! ENTER+++\n\r");

    Block_Len = FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);
    if ((Block_Len == 0) && (Http_gFileData.NotEmpty)) //g_RxBuff.NotEmpty == 1 
    {
        Block_Len = Http_gFileData.Buf_len;
    }


    Remain_Len = Http_gFileData.Buf_len - Block_Len;//Block_Len == 4k
    if(Remain_Len == 0)
    {
        AT_TC(g_sw_GPRS, "HTTP# !!! no space in RX buffer\n\r");

        return -1;
    }
    if (uDataSize > Remain_Len)
    {
        AT_TC(g_sw_GPRS, "HTTP# !!! no enough space in RX buffer\n\r");

        return -1;
    }
 
real_Len = (uDataSize < Remain_Len) ? uDataSize : Remain_Len;

AT_TC(g_sw_GPRS, "HTTP# !!! REAL_LEN %d+++\n\r",real_Len);

if (real_Len > 0)
{
    if (Http_gFileData.Put < Http_gFileData.Get)
    {
        memcpy(&(Http_gFileData.CircuBuf[Http_gFileData.Put]), pBuff, real_Len);
        Http_gFileData.Put = FTP_MOD_BUFF_LEN(Http_gFileData.Put + real_Len, Http_gFileData.Buf_len);
        AT_TC(g_sw_GPRS, "HTTP# !!! GET < PUT+++\n\r");
    }
    else
    {
        First_half = Http_gFileData.Buf_len - Http_gFileData.Put;

        if (real_Len < First_half)
        {
            memcpy(&(Http_gFileData.CircuBuf[Http_gFileData.Put]), pBuff, real_Len);
            Http_gFileData.Put = FTP_MOD_BUFF_LEN(Http_gFileData.Put + real_Len, Http_gFileData.Buf_len);
            AT_TC(g_sw_GPRS, "HTTP# !!! REAL < FIRST+++\n\r");

        }
        else
        {
            memcpy(&(Http_gFileData.CircuBuf[Http_gFileData.Put]), pBuff , First_half);
            Second_half = real_Len - First_half;
            Http_gFileData.Put = FTP_MOD_BUFF_LEN((Http_gFileData.Put + First_half), Http_gFileData.Buf_len);
            memcpy(&(Http_gFileData.CircuBuf[Http_gFileData.Put]), &pBuff[First_half], Second_half);
            Http_gFileData.Put = FTP_MOD_BUFF_LEN((Http_gFileData.Put + Second_half), Http_gFileData.Buf_len);
        }
    }
    Http_gFileData.NotEmpty = 1;
}
else
{
    //SXS_TRACE(TSTDOUT,"warning put len %x",real_Len);
}

ret = Remain_Len - real_Len;

//_exit:

return ret;
}

INT32 http_getDataFromCircularBuffer( UINT8 *pBuff, UINT16 uDataSize)
{
    INT32 Block_Len, Remain_Len, real_Len, First_half, Second_half;
    INT32 ret = 0;

    Block_Len = FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);
    if ((Block_Len == 0) && (Http_gFileData.NotEmpty)) //g_RxBuff.NotEmpty == 1 
    {
        Block_Len = Http_gFileData.Buf_len;
    }


    if(Block_Len == 0)
    {
        AT_TC(g_sw_GPRS, "FTP# !!! no date in RX buffer\n\r");

        return -1;
    }
    if (uDataSize > Block_Len)
    {
        AT_TC(g_sw_GPRS, "FTP# !!! no enough date in RX buffer\n\r");

    }
 
    real_Len = (uDataSize < Block_Len) ? uDataSize : Block_Len;
    if (real_Len > 0)
    {
            if (Http_gFileData.Get < Http_gFileData.Put)
        {
            memcpy(pBuff, &(Http_gFileData.CircuBuf[Http_gFileData.Get]), real_Len);
            Http_gFileData.Get = FTP_MOD_BUFF_LEN(Http_gFileData.Get + real_Len, Http_gFileData.Buf_len);
        }
            else
        {
            First_half = Http_gFileData.Buf_len - Http_gFileData.Get;

            if (real_Len < First_half)
            {
                memcpy(pBuff, &(Http_gFileData.CircuBuf[Http_gFileData.Get]), real_Len);
                Http_gFileData.Get = FTP_MOD_BUFF_LEN(Http_gFileData.Get + real_Len, Http_gFileData.Buf_len);
            }
            else
            {
                memcpy(pBuff, &(Http_gFileData.CircuBuf[Http_gFileData.Get]), First_half);
                Second_half = real_Len - First_half;
                Http_gFileData.Get = FTP_MOD_BUFF_LEN((Http_gFileData.Get + First_half), Http_gFileData.Buf_len);
                memcpy(&pBuff[First_half], &(Http_gFileData.CircuBuf[Http_gFileData.Get]), Second_half);
                Http_gFileData.Get = FTP_MOD_BUFF_LEN((Http_gFileData.Get + Second_half), Http_gFileData.Buf_len);
            }
        }
            
    }
    else
    {
    //SXS_TRACE(TSTDOUT,"warning put len %x",real_Len);
    }

    ret = Block_Len - real_Len;

return ret;
}

INT32 Http_downLoadDataToPC(char *content,long contentLen)
{
    AT_TC(g_sw_GPRS, "http# %s\n\r", __func__);

    UINT8 nSendBuf_tmp[5001] = {0,};

    long left = contentLen;

    long remain_space = 0 ;

    UINT16 nSendLen = 0;

    UINT16 nPutLen = 0;

    INT32 dateLen = 0;

    INT32 beginPut = 0;
    
    remain_space = Http_gFileData.Buf_len -  FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);

    AT_TC(g_sw_GPRS, "http# %d\n\r", remain_space);

    if(remain_space > contentLen){
        
           http_putDataToCircularBuffer(content, contentLen);

           dateLen = FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);

           AT_TC(g_sw_GPRS, "http# %d\n\r", dateLen);

           while(dateLen > 0){

                if(dateLen > HTTP_SEND_MAX){

                    nSendLen = HTTP_SEND_MAX;

                }else{

                    nSendLen = dateLen;

                }
                
                http_getDataFromCircularBuffer( (UINT8 *) nSendBuf_tmp,nSendLen);

                Http_WriteUart((UINT8*)nSendBuf_tmp, nSendLen);

                dateLen -= nSendLen;
           }
           

    }

    while(left > 0){
        
        remain_space = Http_gFileData.Buf_len -  FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);

        if(remain_space > 0){

            if(left > remain_space){

                nPutLen = remain_space;
                
                left = left - remain_space;
            }else {

                nPutLen = left;

                left = 0;
            }

            http_putDataToCircularBuffer(&content[beginPut], nPutLen);

            beginPut += nPutLen;
        }

        dateLen = FTP_GET_DATA_BLOCK_LEN_IN_BUFFER(Http_gFileData.Get, Http_gFileData.Put, Http_gFileData.Buf_len);

        while(dateLen > 0){

                if(dateLen > HTTP_SEND_MAX){

                    nSendLen = HTTP_SEND_MAX;

                }else{

                    nSendLen = dateLen;

                }
                
                http_getDataFromCircularBuffer( (UINT8 *) nSendBuf_tmp,nSendLen);

                Http_WriteUart((UINT8*)nSendBuf_tmp, nSendLen);

                dateLen -= nSendLen;
           }
        }
    
    return 0;
    
}
#endif
