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

#if 1
#include "bt.h"
#include "rdabt_main.h"
#include    "kal_release.h"
#include    "stack_common.h"
#include    "stack_msgs.h"
#include    "uart_sw.h"
#include    "app_ltlcom.h"
#include    "bluetooth_struct.h"
#include    "rdabt_spp.h"
//#include    "che_api.h"
#include "spp.h"
#include "hfp.h"
#include "kal_release.h"
#include "mmi_trace.h"
#include "sdp.h"
//#include "bt_hfg_struct.h"
//#include "bt_types.h"
#include "bluetooth_struct.h"
#include "rdabt.h"


typedef struct
{
    t_bdaddr address;
    u_int8 port;
    u_int8 dlci;
    u_int16 max_frame_size;

} rdabt_spp_struct;

rdabt_spp_struct rdabt_spps_struct[2],rdabt_sppc_struct[2];


#define CIRCULAR_BUF_LEN  16384
#ifdef __SUPPORT_SPP_SYNC__

struct  CircularBuf
{
    UINT8 Buf[CIRCULAR_BUF_LEN];
    UINT16 Get;
    UINT16 Put;
    UINT16 Buf_len;
    UINT8 NotEmpty;
    UINT8  Name[10];
};

typedef enum _CircularBuffer_Ctrl
{
    READ_ONLY,
    READ_AND_GET,
} CircularBuffer_Ctrl;

struct CircularBuf rdabt_spp_tx_buff= {0,};
struct CircularBuf rdabt_spp_rx_buff= {0,};

#define MOD_BUFF_LEN(_val, _length)  ((_val) & (_length-1))
#define GET_DATA_BLOCK_LEN(_start,_end,_cycle) ((_start<=_end)?(_end-_start):(_end+_cycle - _start) )

PRIVATE UINT16 get_CircularBuffer_len(struct  CircularBuf *pCirBuff)
{
    UINT16 Block_Len;
    Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

    if ((Block_Len == 0) && (pCirBuff->NotEmpty))
        Block_Len = pCirBuff->Buf_len;
    return Block_Len;

}

PRIVATE UINT16 put_data_to_CircularBuffer(struct  CircularBuf *pCirBuff, UINT8 *pBuff,UINT16 uDataSize)
{
    UINT16 Block_Len, Remain_Len, real_Len, First_half, Second_half;

    Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

    if ((Block_Len == 0) && (pCirBuff->NotEmpty))
    {
        Block_Len = pCirBuff->Buf_len;
    }

    Remain_Len = pCirBuff->Buf_len - Block_Len;
    if(Remain_Len==0)
    {

        kal_prompt_trace(0, "%s  buffer overflow!!",pCirBuff->Name );
        hal_DbgAssert("buffer overflow!!");
        return 0;
    }
    if (uDataSize > Remain_Len)
        hal_DbgAssert("buffer overflow!!");
    real_Len = (uDataSize < Remain_Len) ? uDataSize : Remain_Len;

    if (real_Len > 0)/*circular buffer not full*/
    {
        if (pCirBuff->Put < pCirBuff->Get)
        {
            memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff, real_Len);
            pCirBuff->Put = MOD_BUFF_LEN(pCirBuff->Put + real_Len,pCirBuff->Buf_len);
        }
        else
        {
            First_half = pCirBuff->Buf_len - pCirBuff->Put;

            if (real_Len < First_half)
            {
                memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff, real_Len);
                pCirBuff->Put = MOD_BUFF_LEN(pCirBuff->Put + real_Len, pCirBuff->Buf_len);
            }
            else
            {
                memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff , First_half);
                Second_half = real_Len - First_half;
                pCirBuff->Put = MOD_BUFF_LEN((pCirBuff->Put + First_half),pCirBuff->Buf_len);

                memcpy(&pCirBuff->Buf[pCirBuff->Put], &pBuff[First_half], Second_half);
                pCirBuff->Put = MOD_BUFF_LEN((pCirBuff->Put + Second_half),pCirBuff->Buf_len);

            }

        }

        pCirBuff->NotEmpty = 1;

    }
    else
    {
        kal_prompt_trace(0,"warning put len %x",real_Len);

    }

    return Remain_Len -real_Len;
}

PRIVATE BOOL Is_CircularBuffer_Empty(struct  CircularBuf *pCirBuff)
{
    return (!pCirBuff->NotEmpty);
}

PRIVATE UINT16 get_data_from_CircularBuffer(struct  CircularBuf *pCirBuff, UINT8 *pBuff,UINT16 uDataSize,UINT16 fun)
{
    UINT16 Block_Len, First_half, Second_half, real_len, real_sent_len, tmp_Get;
    HANDLE h = 0;

    real_sent_len = 0;

    Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

    if ((Block_Len == 0) && (pCirBuff->NotEmpty))
        Block_Len = pCirBuff->Buf_len;

    real_len = (Block_Len < uDataSize) ? Block_Len :  uDataSize;
    if (real_len > 0)/*have data to send*/
    {

        if (pCirBuff->Get < pCirBuff->Put)
        {
            memcpy(pBuff, &pCirBuff->Buf[pCirBuff->Get], real_len);
            tmp_Get = MOD_BUFF_LEN(pCirBuff->Get + real_len,pCirBuff->Buf_len);
        }
        else
        {

            First_half = pCirBuff->Buf_len  - pCirBuff->Get;

            if (real_len < First_half)
            {
                memcpy(pBuff, &(pCirBuff->Buf[pCirBuff->Get]), real_len);
                tmp_Get = MOD_BUFF_LEN(pCirBuff->Get + real_len,pCirBuff->Buf_len);
            }
            else
            {
                memcpy(pBuff, &(pCirBuff->Buf[pCirBuff->Get]), First_half);
                Second_half = real_len - First_half;
                tmp_Get = MOD_BUFF_LEN(pCirBuff->Get + First_half,pCirBuff->Buf_len);

                memcpy(&pBuff[First_half], &(pCirBuff->Buf[tmp_Get]), Second_half);
                tmp_Get = MOD_BUFF_LEN(tmp_Get + Second_half,pCirBuff->Buf_len);

            }
        }

        if(READ_ONLY == fun)
        {
            ;
        }
        else if(READ_AND_GET == fun)
        {
            pCirBuff->Get  = tmp_Get;
            if (pCirBuff->Get == pCirBuff->Put)
                pCirBuff->NotEmpty = 0;
        }

    }
    else
    {
        //SXS_TRACE(TSTDOUT,"Tx  buffer empty!!");
    }

    return real_len;
}

#endif

void rdabt_spp_active(void)
{
    kal_prompt_trace(0, "rdabt_spp_active called" );

    //SPP_Active_Req();
#ifdef __SUPPORT_SPP_SYNC__
    memset(&rdabt_spp_tx_buff,0,sizeof(rdabt_spp_tx_buff));
    rdabt_spp_tx_buff.Buf_len=CIRCULAR_BUF_LEN;
    memset(&rdabt_spp_rx_buff,0,sizeof(rdabt_spp_rx_buff));
    rdabt_spp_rx_buff.Buf_len=CIRCULAR_BUF_LEN;
#endif

}

void rdabt_spp_connect_req(t_bdaddr device_addr)
{
    // memcpy(rdabt_sppc_struct.address.bytes,device_addr.bytes,SIZE_OF_BDADDR);
    //SPP_Connect_Req( device_addr);

}
void rdabt_spp_send_req(u_int8* data, u_int16 length, u_int16 spp_port)
{
    SPP_Send_Data(data,  length,spp_port);

}

void rdabt_spp_connect_ind(void *pArgs)
{
    t_spp_msg *msg = (t_spp_msg*)pArgs;
    kal_prompt_trace(0, "rdabt_spp_connect_cnf result=%d", msg->result );
#ifdef __SUPPORT_SPP_SYNC__
    memset(&rdabt_spp_tx_buff,0,sizeof(rdabt_spp_tx_buff));
    rdabt_spp_tx_buff.Buf_len=CIRCULAR_BUF_LEN;
    memset(&rdabt_spp_rx_buff,0,sizeof(rdabt_spp_rx_buff));
    rdabt_spp_rx_buff.Buf_len=CIRCULAR_BUF_LEN;
#endif
    if(msg->result  ==RDABT_NOERROR)
    {
        if(rdabt_spps_struct[0].max_frame_size==0)
        {
            char * string_test="I AM D8 EBOOK~! server1";

            memcpy(rdabt_spps_struct[0].address.bytes,msg->address.bytes,SIZE_OF_BDADDR);
            rdabt_spps_struct[0].dlci=msg->dlci;
            rdabt_spps_struct[0].port=msg->port;
            rdabt_spps_struct[0].max_frame_size=msg->max_frame_size;
            rdabt_spp_send_req(string_test,strlen(string_test), rdabt_spps_struct[0].port);
        }
        else
        {
            char * string_test="I AM D8 EBOOK~! server2";

            memcpy(rdabt_spps_struct[1].address.bytes,msg->address.bytes,SIZE_OF_BDADDR);
            rdabt_spps_struct[1].dlci=msg->dlci;
            rdabt_spps_struct[1].port=msg->port;
            rdabt_spps_struct[1].max_frame_size=msg->max_frame_size;

            rdabt_spp_send_req(string_test,strlen(string_test), rdabt_spps_struct[1].port);
        }


    }
    else
    {
        //fail
    }


}


void rdabt_spp_connect_cnf(void *pArgs)
{
    t_spp_msg *msg = (t_spp_msg*)pArgs;
    kal_prompt_trace(0, "rdabt_spp_connect_cnf result=%d,msg->max_frame_size=%d", msg->result,msg->max_frame_size );
    if(msg->result  ==RDABT_NOERROR)
    {
        if(rdabt_sppc_struct[0].max_frame_size==0)
        {
            char * string_test="I AM D8 EBOOK~! client1";
            memcpy(rdabt_sppc_struct[0].address.bytes,msg->address.bytes,SIZE_OF_BDADDR);

            rdabt_sppc_struct[0].dlci=msg->dlci;
            rdabt_sppc_struct[0].port=msg->port;
            rdabt_sppc_struct[0].max_frame_size=msg->max_frame_size;
            rdabt_spp_send_req(string_test,strlen(string_test), rdabt_sppc_struct[0].port);
        }
        else
        {
            char * string_test="I AM D8 EBOOK~! client2";
            memcpy(rdabt_sppc_struct[1].address.bytes,msg->address.bytes,SIZE_OF_BDADDR);

            rdabt_sppc_struct[1].dlci=msg->dlci;
            rdabt_sppc_struct[1].port=msg->port;
            rdabt_sppc_struct[1].max_frame_size=msg->max_frame_size;
            rdabt_spp_send_req(string_test,strlen(string_test), rdabt_sppc_struct[1].port);
        }
    }
    else
    {
        //fail
    }
}

#ifdef __SUPPORT_SPP_SYNC__
UINT8 *memstr(UINT8 *haystack, UINT8 *needle, UINT32 nHayStackSize,UINT32 nNeedleSize)
{
    UINT8 *p;
    UINT8 needlesize = nNeedleSize;

    for (p = haystack; p <= (haystack-needlesize+nHayStackSize); p++)
    {
        if (memcmp(p, needle, needlesize) == 0)
            return p; /* found */
    }
    return NULL;
}

typedef enum _rdabt_sppphone_proc_mode_t
{
    RDABT_SPPPHONE_PROC_MODE_COMMAND,
    RDABT_SPPPHONE_PROC_MODE_AUDIO,
    RDABT_SPPPHONE_PROC_MODE_DATA,
    RDABT_SPPPHONE_PROC_MODE_RSV,
    RDABT_SPPPHONE_PROC_MODE_INVALID_,  // invalid value of this enum.
} rdabt_sppphone_proc_mode_t;

rdabt_sppphone_proc_mode_t sppphone_proc_mode=RDABT_SPPPHONE_PROC_MODE_COMMAND;


CONST UINT8 STR_5PLUS[4]  = {0x2b,0x2b,0x2b,0x2b};  //+++++
CONST UINT8 STR_5MINUS[4] = {0x2d,0x2d,0x2d,0x2d};  //-----
CONST UINT8 STR_5EQUAL[4] = {0x3d,0x3d,0x3d,0x3d};  //=====

CONST UINT8 RDA_SPP_PHONE_CMD_RX_BEGIN[]= {0xAA,0xBB };
CONST UINT8 RDA_SPP_PHONE_PCM_RX_BEGIN[]= {0xAA,0xCC };
CONST UINT8 RDA_SPP_PHONE_DATA_RX_BEGIN[]= {0xAA,0xDD };


#endif

#ifdef __SUPPORT_SPP_SYNC__
extern HANDLE g_hSPPTask;
VOID Bt_Spp_Tx_Processor(VOID);

BOOL Send_Rx_Msg2SPPTask(COS_EVENT* pev)
{
    BOOL nRet=0;
    pev->nEventId = EV_SPP_RX_MSG;
    nRet=COS_SendEvent(g_hSPPTask, pev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return nRet;
}
BOOL Send_Tx_Msg2SPPTask(COS_EVENT* pev)
{
    BOOL nRet=0;
    pev->nEventId = EV_SPP_TX_MSG;
    nRet=COS_SendEvent(g_hSPPTask, pev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return nRet;
}

void BAL_SPPTask(void *pData)
{
    COS_EVENT event = { 0 };

    TS_OutputText(1, "Enter BAL_EcallTask()...");
    SUL_ZeroMemory32(&event, sizeof(COS_EVENT));

    /* 创建Media Task的消息循环 */

    hal_HstSendEvent(0xeca11000);

    for (;;)
    {
        COS_Free((VOID*)event.nParam1); // Clear the memory of the event.nParam1, then it will do the getting value...
        TS_OutputText(1, "in BAL_SPPTask()..");
        COS_WaitEvent(g_hSPPTask, &event, COS_WAIT_FOREVER);


        switch (event.nEventId)
        {
            case EV_SPP_RX_MSG:
            {
                Bt_Spp_Rx_Processor();
            }
            break;
            case EV_SPP_TX_MSG:
            {
                Bt_Spp_Tx_Processor();
            }
            break;
            default:
                break;
        }
    }

}

#endif


void rdabt_spp_data_ind(void *pArgs)
{
    t_spp_msg *msg = (t_spp_msg*)pArgs;

    hal_HstSendEvent(0xdead8888);
    hal_HstSendEvent(msg->data->bufLen);

    t_DataBuf *rx_data=msg->data;
    u_int16 rxlength=rx_data->dataLen;

    COS_EVENT EV = { 0 ,};

    kal_prompt_trace(0, "rdabt_spp_data_ind rxlength=%d" ,rx_data->dataLen );
#ifdef __SUPPORT_SPP_SYNC__

    put_data_to_CircularBuffer(&rdabt_spp_rx_buff,rx_data->buf,rxlength);

    Send_Rx_Msg2SPPTask(&EV);
#endif
    RSE_FreeWriteBuffer(rx_data);
    //do the job you need
}


#ifdef __SUPPORT_SPP_SYNC__
#include "sxr_tim.h"
void rdabt_spp_PutTxDataToSendFiFo(u_int8* data, u_int16 length);
//UINT8 temp_show[320]={0};
//char msg_connect[]={"BEGIN:BMSG\r\nVERSION:1.0\r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:CC\r\nTEL:+8613812345678\r\nEND:VCARD\r\nBEGIN:BENV\r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:RDA\r\nTEL:00000000\r\nEND:VCARD\r\nBEGIN:BBODY\r\nDATETIME:20130329T094950\r\nCHARSET:UTF-8\r\nLENGTH:11\r\nBEGIN:MSG\r\nABCDEF\r\nENDMSG\r\nEND:BBODY\r\nEND:BENV\r\nEND:BMSG\r\n"};


/*待保存的短sms的数量*/
UINT32 g_spp_sms_recv = 0;
/*已经保存的短sms的数量*/
UINT32 g_spp_sms_usedSlot = 0;
/*最大能保存的短sms的数量*/
UINT32 g_spp_sms_totalSlot = 0;
/*sms ack 的缓存数据*/
char sms_ack_data[50] = {0};

/*已经保存的pbk 的数量*/
UINT32 g_spp_pbk_usedSlot = 0;
/*最大能保存的pbk 的数量*/
UINT32 g_spp_pbk_totalSlot = 0;
/*pbk ack 的缓存数据*/
char pbk_ack_data[50] = {0};

/*data ack 的缓存数据*/
char data_ack_data[50] = {0};
/*data acd 的错误号*/
UINT32 g_spp_data_ack_error = 0;

/*一秒的tick*/
#define HAL_TICK1S 16384
/*1毫秒的tick*/
#define MS_WAITING      *  HAL_TICK1S / 1000

/*操作短信的时候，锁sms ack*/
UINT8 g_spp_send_sms_ack_locked = FALSE;

/*操作电话本的时候，锁pbk ack*/
UINT8 g_spp_send_pbk_ack_locked = FALSE;

/*操作用户数据的时候，锁data ack*/
UINT8 g_spp_send_data_ack_locked = FALSE;

#ifdef __SPP_SMS_DEBUG_MALLOC__
UINT32 g_apk_debug = 0;
void begin_spp_sms(void)
{
    if(1 == g_apk_debug)
    {
        /*走到这个assert 表示apk 发送完一条sms 之后没有等到btphone 回复ack 就发送下一条sms 了*/
        hal_DbgAssert(0);
    }
    g_apk_debug = 1;
    spp_sms_reset_all();
}
void end_spp_sms(void)
{
    spp_sms_free_all();
    g_apk_debug = 0;
}
#endif
void spp_send_sms_ack(void)
{
    if(TRUE == g_spp_send_sms_ack_locked)
    {
        /*延时一段时间，给MMI用来处理收到短信*/
        sxr_StopFunctionTimer(spp_send_sms_ack);
        sxr_StartFunctionTimer(200 MS_WAITING, spp_send_sms_ack, 0,0);
    }
    else
    {
        UINT16 data_length = strlen(sms_ack_data);/*因为apk那边处理的时候，不希望带\0，所以用了strlen*/
        rdabt_spp_PutTxDataToSendFiFo(sms_ack_data,data_length);
#ifdef __SPP_SMS_DEBUG_MALLOC__
        end_spp_sms();
#endif
    }
}
void spp_send_pbk_ack(void)
{
    if(TRUE == g_spp_send_pbk_ack_locked)
    {
        /*延时一段时间，给MMI用来处理收到短信*/
        sxr_StopFunctionTimer(spp_send_pbk_ack);
        sxr_StartFunctionTimer(200 MS_WAITING, spp_send_pbk_ack, 0,0);
    }
    else
    {
        UINT16 data_length = strlen(pbk_ack_data);/*因为apk那边处理的时候，不希望带\0，所以用了strlen*/
        rdabt_spp_PutTxDataToSendFiFo(pbk_ack_data,data_length);
    }
}
void spp_send_data_ack(void)
{
    if(TRUE == g_spp_send_data_ack_locked)
    {
        /*延时一段时间，给MMI用来处理收到的用户命令*/
        sxr_StopFunctionTimer(spp_send_data_ack);
        sxr_StartFunctionTimer(200 MS_WAITING, spp_send_data_ack, 0,0);
    }
    else
    {
        UINT16 data_length = strlen(data_ack_data);/*因为apk那边处理的时候，不希望带\0，所以用了strlen*/
        rdabt_spp_PutTxDataToSendFiFo(data_ack_data,data_length);
    }
}

void spp_send_sms_ack_save_new_sms(UINT32 delay_ms)
{

    /*每收完一个短短信，这个值就减1，减到0说明spp传过来的短信分割成的短短信都收完整了，于是发生sms ack给apk*/
    g_spp_sms_recv --;

    memset(sms_ack_data,0,sizeof(sms_ack_data));
    sprintf(sms_ack_data,"%s%03d/%03d","sms ack ok ",g_spp_sms_usedSlot,g_spp_sms_totalSlot);

    if(0 == g_spp_sms_recv)
    {
        /*延时一段时间，给MMI用来处理收到短信*/
        sxr_StopFunctionTimer(spp_send_sms_ack);
        sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_sms_ack, 0,0);
    }
}
void spp_send_sms_ack_full_for_sms(UINT32 delay_ms)
{

    /*如果短信满了，那么就不可能收完整这个短信了，所以强制认为收完了这个短信，这样才能发送sma ack给apk*/
    g_spp_sms_recv = 0;

    memset(sms_ack_data,0,sizeof(sms_ack_data));
    sprintf(sms_ack_data,"%s%03d/%03d","sms ack full ",g_spp_sms_usedSlot,g_spp_sms_totalSlot);

    if(0 == g_spp_sms_recv)
    {
        /*延时一段时间，给MMI用来处理收到短信*/\
        sxr_StopFunctionTimer(spp_send_sms_ack);
        sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_sms_ack, 0,0);
    }
}

void spp_send_pbk_ack_save_new_pbk(UINT32 delay_ms)
{
    memset(pbk_ack_data,0,sizeof(pbk_ack_data));
    sprintf(pbk_ack_data,"%s%04d/%04d","pbk ack ok ",g_spp_pbk_usedSlot,g_spp_pbk_totalSlot);

    /*延时一段时间，给MMI用来处理收到的联系人*/\
    sxr_StopFunctionTimer(spp_send_pbk_ack);
    sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_pbk_ack, 0,0);
}
void spp_send_pbk_ack_full_for_pbk(UINT32 delay_ms)
{
    memset(pbk_ack_data,0,sizeof(pbk_ack_data));
    sprintf(pbk_ack_data,"%s%04d/%04d","pbk ack full ",g_spp_pbk_usedSlot,g_spp_pbk_totalSlot);

    /*延时一段时间，给MMI用来处理收到的联系人*/\
    sxr_StopFunctionTimer(spp_send_pbk_ack);
    sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_pbk_ack, 0,0);
}

void spp_send_data_ack_success(UINT32 delay_ms)
{
    memset(data_ack_data,0,sizeof(data_ack_data));
    sprintf(data_ack_data,"%s","data ack ok");

    /*延时一段时间，给MMI用来处理收到的联系人*/\
    sxr_StopFunctionTimer(spp_send_data_ack);
    sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_data_ack, 0,0);
}
void spp_send_data_ack_fail(UINT32 delay_ms)
{
    memset(data_ack_data,0,sizeof(data_ack_data));
    sprintf(data_ack_data,"%s%04d","data ack error ",g_spp_data_ack_error);

    /*延时一段时间，给MMI用来处理收到的联系人*/\
    sxr_StopFunctionTimer(spp_send_data_ack);
    sxr_StartFunctionTimer(delay_ms MS_WAITING, spp_send_data_ack, 0,0);
}


/*spp 回复短信*/
char g_spp_sms_reply[2048] = {0};
S32 ucs2_to_utf8 (U8 *utf8, U16 ucs2)
{
    int count;
    if (ucs2 < 0x80)
        count = 1;
    else if (ucs2 < 0x800)
        count = 2;
    else
        count = 3;
    switch (count)
    {
        case 3: utf8[2] = 0x80 | (ucs2 & 0x3f); ucs2 = ucs2 >> 6; ucs2 |= 0x800;
        case 2: utf8[1] = 0x80 | (ucs2 & 0x3f); ucs2 = ucs2 >> 6; ucs2 |= 0xc0;
        case 1: utf8[0] = (U8)ucs2;
    }
    return count;
}

S32 ucs2_to_utf8_len(U16 ucs2)
{
    if (ucs2 < 0x80)
        return 1;
    else if (ucs2 < 0x800)
        return 2;
    else
        return 3;
}

S32 ucs2_to_utf8_string(U8 *dest,S32 dest_size,U8* src)
{
    S32 pos = 0;

    /*大小端转换用*/
    U8 temp = 0;

    while( src[0] || src[1])
    {
        /*大小端转换*/
        temp = src[0];
        src[0] = src[1];
        src[1] = temp;

        if (pos + ucs2_to_utf8_len(*(U16*)src) >= dest_size) // leave space of '\0'
        {
            break;
        }

        pos += ucs2_to_utf8(dest+pos,(*(U16*)src));
        src += 2;
        if(pos>=dest_size-1) break;
    }
    if(pos>=dest_size)
    {
        dest[dest_size-1] = 0;
    }
    else
        dest[pos] = 0;
    return pos+1;
}
#include "smsal_l4c_enum.h"
void SPP_SmsSendMessage(UINT8* pNumber,UINT8 numberLength,UINT8* data,UINT16 dataLength,UINT8 dcs)
{
    UINT16 datalength_begin = 0;
    UINT16 data_begin = 0;
    UINT16 utf8_datalength = 0;
    UINT8 is_all_english = FALSE;

    /*检查是不是纯英文*/
    if(SMSAL_DEFAULT_DCS == dcs)
    {
        /*如果是全英文*/
        is_all_english = TRUE;
    }

    /*准备需要发送的数据*/
    memset(g_spp_sms_reply,0,sizeof(g_spp_sms_reply));
    sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s","BEGIN:BMSG\r\nVERSION:1.0\r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:CC\r\nTEL:");
    /*如果长度是13位，说明是 +86开头的，需要这边加一个 "+"  */
    if((13 == numberLength) && ('8' == pNumber[0]) && ('6' == pNumber[1]))
    {
        sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s","+");
    }
    sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s",pNumber);
    sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s","\r\nEND:VCARD\r\nBEGIN:BENV\r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:RDA\r\nTEL:00000000\r\nEND:VCARD\r\nBEGIN:BBODY\r\nDATETIME:20130329T094950\r\nCHARSET:UTF-8\r\nLENGTH:");

    datalength_begin = strlen(g_spp_sms_reply);
    sprintf(g_spp_sms_reply+datalength_begin,"%4d",dataLength);
    sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s","\r\nBEGIN:MSG\r\n");

    /*unicode 转utf8*/
    data_begin = strlen(g_spp_sms_reply);
    if(TRUE == is_all_english)
    {
        sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s",data);
    }
    else
    {
        ucs2_to_utf8_string(g_spp_sms_reply+data_begin,sizeof(g_spp_sms_reply)-strlen(g_spp_sms_reply),data);
    }
    utf8_datalength = strlen(g_spp_sms_reply) - data_begin;

    sprintf(g_spp_sms_reply+strlen(g_spp_sms_reply),"%s","\r\nENDMSG\r\nEND:BBODY\r\nEND:BENV\r\nEND:BMSG\r\n");

    /*写入utf8_length*/
    {
        char temp_length[10]= {0};
        sprintf(temp_length,"%4d",utf8_datalength);
        memcpy(&g_spp_sms_reply[datalength_begin],&temp_length[0],4);
    }
    rdabt_spp_PutTxDataToSendFiFo(g_spp_sms_reply,strlen(g_spp_sms_reply));

}


/*spp发送的流控*/
BOOL gflow=0;

void rdabt_spp_PutTxDataToSendFiFo(u_int8* data, u_int16 length)
{
    COS_EVENT EV = { 0 ,};
    BOOL nRet=0;
    UINT16 Remain_Len;

    if (!rdabt_spps_struct[0].max_frame_size)
        return;

    if (gflow)
        return;

    Remain_Len=put_data_to_CircularBuffer(&rdabt_spp_tx_buff,data,length);


    if (Remain_Len<700) gflow=1;

    Send_Tx_Msg2SPPTask(&EV);
}

VOID Bt_Spp_Tx_Processor(VOID)
{
    UINT8 i=0;
    UINT16 buffLen=0;
    UINT8 *data=NULL;
    COS_EVENT EV = { 0 ,};

    buffLen=get_CircularBuffer_len(&rdabt_spp_tx_buff);
    if (buffLen==0)
        return;

    data=(unsigned char*)COS_MALLOC(rdabt_spps_struct[0].max_frame_size);

    if (NULL==data)
        hal_DbgAssert("pls gdb view");

    for (i=0; i<(buffLen/rdabt_spps_struct[0].max_frame_size); i++)
    {
        get_data_from_CircularBuffer(&rdabt_spp_tx_buff,data,rdabt_spps_struct[0].max_frame_size,READ_AND_GET);
        if (SPP_Send_Data(data,rdabt_spps_struct[0].max_frame_size,0)==0xFF)
        {
            COS_FREE(data);
            hal_DbgAssert(0);/*buffer满了*/
        }

    }
    if ((buffLen%rdabt_spps_struct[0].max_frame_size)!=0)
    {
        get_data_from_CircularBuffer(&rdabt_spp_tx_buff,data,buffLen%rdabt_spps_struct[0].max_frame_size,READ_AND_GET);
        if (SPP_Send_Data(data,(buffLen%rdabt_spps_struct[0].max_frame_size),0)==0xFF)
        {
            COS_FREE(data);
            hal_DbgAssert(0);/*buffer满了*/
        }
    }

    COS_FREE(data);

    return;
}

/*处理用户数据*/
void deal_user_data_for_spp(char *buffer,UINT16 length)
{
    ;
}

void Bt_Spp_Rx_Processor(void)
{
    rdabt_sppphone_proc_mode_t sppphone_proc_mode=RDABT_SPPPHONE_PROC_MODE_INVALID_;
    u_int16 rxLength=0;
    u_int16 payloadLength=0;
    u_int8* pPayloadBuff=NULL;
    u_int8* rxData = NULL;
    u_int8* rxDataStart =NULL;
    u_int8* pOffset = NULL;
    u_int8* pOffset_CMD=NULL;
    u_int8* pOffset_PCM=NULL;
    u_int8* pOffset_DATA=NULL;
    COS_EVENT EV = { 0 ,};

    rxLength=get_CircularBuffer_len(&rdabt_spp_rx_buff);

    if(0==rxLength)return;

    rxData = (unsigned char*)COS_MALLOC(rxLength);
    rxDataStart = rxData;
    memset(rxData,0,rxLength);

    get_data_from_CircularBuffer(&rdabt_spp_rx_buff,rxData,rxLength,READ_ONLY);

    pOffset_CMD=memstr(rxData,RDA_SPP_PHONE_CMD_RX_BEGIN,rxLength,sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN));
    pOffset_PCM=memstr(rxData,RDA_SPP_PHONE_PCM_RX_BEGIN,rxLength,sizeof(RDA_SPP_PHONE_PCM_RX_BEGIN));
    pOffset_DATA=memstr(rxData,RDA_SPP_PHONE_DATA_RX_BEGIN,rxLength,sizeof(RDA_SPP_PHONE_DATA_RX_BEGIN));


    /*找到位置最靠前的Offset 赋值给pOffset*/
    {
        pOffset = NULL;
        if(NULL != pOffset_CMD)
        {
            pOffset = pOffset_CMD;
        }
        if(NULL != pOffset_PCM)
        {
            pOffset = pOffset_PCM;
        }
        if(NULL != pOffset_DATA)
        {
            pOffset = pOffset_DATA;
        }

        /*如果找到了0xAA 0xBB 或者0xAA 0xCC 或者0xAA 0xDD*/
        if(NULL != pOffset)
        {
            /*找到最小的Offset赋值给pOffset*/
            if((NULL != pOffset_CMD)&&(pOffset_CMD<pOffset))
            {
                pOffset = pOffset_CMD;
            }
            if((NULL != pOffset_PCM)&&(pOffset_PCM<pOffset))
            {
                pOffset = pOffset_PCM;
            }
            if((NULL != pOffset_DATA)&&(pOffset_DATA<pOffset))
            {
                pOffset = pOffset_DATA;
            }
        }
    }

    if(NULL == pOffset)
    {
        COS_FREE(rxDataStart);
        return;
    }

    /*length has not income already*/
    if((pOffset - rxData + 4)>rxLength)
    {
        COS_FREE(rxDataStart);
        return;
    }

    payloadLength = *(pOffset+sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN)) |
                    (*(pOffset+sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN)+1))<<8;

    //hal_HstSendEvent(0xdead9999);
    //hal_HstSendEvent(pOffset - rxData);
    //hal_HstSendEvent(payloadLength);
    if (payloadLength<=(rxLength - (pOffset - rxData)-sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN)-2))
    {
        memset(rxData,0,rxLength);
        get_data_from_CircularBuffer(&rdabt_spp_rx_buff,rxData, pOffset - rxData + sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN) + 2 + payloadLength,READ_AND_GET);
        pPayloadBuff= (pOffset+sizeof(RDA_SPP_PHONE_CMD_RX_BEGIN)+2);

        if(*(pOffset+1) == (unsigned char)0xBB)
        {
            sppphone_proc_mode = RDABT_SPPPHONE_PROC_MODE_COMMAND;
        }
        else if(*(pOffset+1) == (unsigned char)0xCC)
        {
            sppphone_proc_mode = RDABT_SPPPHONE_PROC_MODE_AUDIO;
        }
        else if(*(pOffset+1) == (unsigned char)0xDD)
        {
            sppphone_proc_mode = RDABT_SPPPHONE_PROC_MODE_DATA;
        }

        switch(sppphone_proc_mode)
        {
            case RDABT_SPPPHONE_PROC_MODE_COMMAND:
#ifdef __SUPPORT_BT_SYNC_MAP__
#ifdef __SPP_SMS_DEBUG_MALLOC__
                begin_spp_sms();
#endif
                deal_msg_data_for_spp(pPayloadBuff);
#endif
                break;
            case RDABT_SPPPHONE_PROC_MODE_AUDIO:
#ifdef __SUPPORT_BT_SYNC_PHB__
            {
                deal_pbap_data_for_spp(pPayloadBuff);
            }
#endif
            break;
            case RDABT_SPPPHONE_PROC_MODE_DATA:
            {
                deal_user_data_for_spp(pPayloadBuff,payloadLength);
            }
            break;
        }


    }
    COS_FREE(rxDataStart);
    return;
    //do the job you need
}

#endif

UINT32 rdabt_spp_GetRxData(UINT8 *pData)
{
    UINT32 dataLen=0;

    return dataLen;
}
void rdabt_spp_data_cnf(void *pArgs)//after call SPP_Send_Data(),if finished send will call rdabt_spp_data_cnf
{
    t_spp_msg *msg = (t_spp_msg*)pArgs;
    kal_prompt_trace(0, "rdabt_spp_data_cnf result=%d " ,msg->result );
}


void rdabt_spp_disconnect_ind(void *pArgs)
{
    kal_prompt_trace(0, "rdabt_spp_disconnect_ind" );
    memset(rdabt_spps_struct,0,sizeof(rdabt_spp_struct)*2);
    memset(rdabt_sppc_struct,0,sizeof(rdabt_spp_struct)*2);

}

void rdabt_spp_disconnect_cnf(void *pArgs)
{
    kal_prompt_trace(0, "rdabt_spp_disconnect_cnf" );
    memset(rdabt_spps_struct,0,sizeof(rdabt_spp_struct)*2);
    memset(rdabt_sppc_struct,0,sizeof(rdabt_spp_struct)*2);

}


void rdabt_spp_msg_hdler(ilm_struct *message)
{
    switch(message->msg_id)
    {
        case MSG_ID_BT_SPP_ACTIVATE_REQ:
        {
            SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
            SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);//register two port so active two times

            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_SPP_ACTIVATE_CNF, NULL, NULL);
            memset(rdabt_spps_struct,0,sizeof(rdabt_spp_struct)*2);
            memset(rdabt_sppc_struct,0,sizeof(rdabt_spp_struct)*2);

        }
        break;
        case MSG_ID_BT_SPP_CONNECT_REQ:
        {
            bt_spp_connect_req_struct *msg_req=(bt_spp_connect_req_struct *)message->local_para_ptr;
            t_bdaddr bd_addr;
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->lap, msg_req->uap, msg_req->nap);
            kal_prompt_trace(0, "rdabt_spp_msg_hdler MSG_ID_BT_SPP_CONNECT_REQ" );

            SPP_Connect_Req(bd_addr, SDP_SCLASS_UUID_SERIAL, NULL);
            break;
        }
        case MSG_ID_BT_SPP_SCO_CONNECT_REQ:
            break;
        case MSG_ID_BT_SPP_SEND_DATA_REQ:
            break;
        case  MSG_ID_BT_SPP_GET_DATA_REQ:
            break;
        case  MSG_ID_BT_SPP_DEACTIVATE_REQ:
        {
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_SPP_DEACTIVATE_CNF, NULL, NULL);
        }
        break;
        case   MSG_ID_BT_SPP_DISCONNECT_REQ:
            break;
        case   MSG_ID_BT_SPP_AUDIO_CONNECT_REQ:
            break;
        case   MSG_ID_BT_SPP_AUDIO_DISCONNECT_REQ:
            break;
        //case   MSG_ID_BT_SPP_CONNECT_IND_RES:
        //   break;
        case   MSG_ID_BT_SPP_AUTH_RSP:
            break;
        case   MSG_ID_BT_SPP_UART_OWNER_CNF:
            break;
        case   MSG_ID_BT_SPP_UART_PLUGOUT_CNF:
            break;
        //case   MSG_ID_BT_DUN_CONNECT_REQ:
        //  break;
        case   MSG_ID_BT_DUN_ACTIVATE_REQ:
            break;
        case   MSG_ID_BT_DUN_DEACTIVATE_REQ:
            break;
        case   MSG_ID_BT_DUN_DISCONNECT_REQ:
            break;
        // case   MSG_ID_BT_DUN_CONNECT_IND_RES:
        //  break;
        case   MSG_ID_BT_DUN_AUTH_RSP:
            break;
        case   MSG_ID_BT_FAX_ACTIVATE_REQ:
            break;
        case   MSG_ID_BT_FAX_DEACTIVATE_REQ:
            break;
        case   MSG_ID_BT_FAX_DISCONNECT_REQ:
            break;
        // case   MSG_ID_BT_FAX_CONNECT_IND_RES:
        // break;
        case   MSG_ID_BT_FAX_AUTH_RSP:
            break;
        default:
            break;

    }

}



const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} spp_msg_hdlr_table[] =
{
    {SPP_CONNECT_IND,rdabt_spp_connect_ind},
    {SPP_CONNECT_CNF,   rdabt_spp_connect_cnf},
    {SPP_DATA_IND,rdabt_spp_data_ind},
    {SPP_DATA_CNF,rdabt_spp_data_cnf},
    {SPP_DISCONNECT_IND,rdabt_spp_disconnect_ind},
    {SPP_DISCONNECT_CNF,rdabt_spp_disconnect_cnf},

};

void rdabt_spp_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(spp_msg_hdlr_table)/sizeof(spp_msg_hdlr_table[0]);
    for(I=0; I<n; I++)
    {
        if( mi==spp_msg_hdlr_table[I].type )
        {
            spp_msg_hdlr_table[I].handler( mv );
            break;
        }
    }
}
#endif
