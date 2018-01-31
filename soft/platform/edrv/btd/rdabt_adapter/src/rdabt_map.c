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

#ifdef __SUPPORT_BT_SYNC_MAP__
#include "rdabt.h"
#include "bt.h"
#include "papi.h"
#include "bluetooth_struct.h"
#include "obex.h"
#include "bt_dm_struct.h"
#include "bt_hfg_struct.h"
#include "rdabt_pbap.h"
#include "rdabt_map.h"
#include "queuegprot.h"
#include "protocolevents.h"

t_bdaddr bt_map_addr;
u_int32 bt_map_tid = 0;
u_int32 bt_map_cid = 0;

u_int32 bt_mnp_tid = 0;
u_int32 bt_mnp_cid = 0;

u_int8 bt_map_step = 0;
u_int8 bt_mnp_step=0;

u_int32 bt_map_size = 0;
u_int32 have_got_map_size = 0;
u_int32 remain_map_size = 0;
u_int32 finished_get_map_size = 0;

int obex_size = 0;
int obex_start = 0;

static u_int8 *result_data;
static u_int16 result_length;
static u_int16 buffer_length;
static uint32 current_array_index=0;
static u_int8 current_get_type=0;// 2 is get msg content,1 is get msg list to get handle
extern u_int16 last_client_connect;
extern uint32 pcmbuf_overlay[4608];
t_bdaddr BT_map_addr;

bt_map_message_info bt_map_msg_array[10];
u_int8 bt_map_timer=0;
void MAP_CON_Timeout(void)
{
    bt_pbap_con_discon_info *param_ptr;
    BT_TraceOutputText(0,"PB_Download_Timeout called PBAP_DISCON_OPPER_NOT_ACCEPT=%d",PBAP_DISCON_OPPER_NOT_ACCEPT);
    rdabt_cancel_timer(bt_map_timer);

    param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
    param_ptr->tid=0;
    param_ptr->status=PBAP_DISCON_OPPER_NOT_ACCEPT;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MAP_DISCON_CNF, (void *)param_ptr, NULL);

    bt_map_step=0;
    bt_map_timer = 0;
}
void bt_map_con_req(t_bdaddr addr)
{
    // bt_dm_addr_struct bt_addr;
    //t_bdaddr addr;
    //bt_addr.lap=0x3737b5;//=addr.lap
    //bt_addr.uap=0xfe;//addr.uap
    //bt_addr.nap=0xb8ff;//addr.nap
    BT_TraceOutputText(0,"bt_map_con_req");
    //BDADDR_Set_LAP_UAP_NAP(&addr, bt_addr.lap, bt_addr.uap, bt_addr.nap);

    memcpy(bt_map_addr.bytes,addr.bytes,6);

    last_client_connect=SCLASS_UUID_MESSAGE_ACCESS_MSE;
    //memcpy(bt_map_addr.bytes,addr.bytes,6);
    last_client_connect =SCLASS_UUID_MESSAGE_ACCESS_MSE;
    OBEX_ConnectRequest(&bt_map_addr,SCLASS_UUID_MESSAGE_ACCESS_MSE,pcmbuf_overlay);
    bt_map_step = 2;
    have_got_map_size=0;
    remain_map_size=0;
    bt_map_size=0;
    current_array_index=0;

    bt_map_timer =rdabt_timer_create(200,  MAP_CON_Timeout,NULL,pTIMER_ONESHOT);

}
void map_discon_req(void)
{
    BT_TraceOutputText(0,"pbap_discon_req con pb_tid=%x",bt_map_tid);
    OBEX_Disconnect(bt_map_tid);

}
void bt_map_con_cnf(obex_cli_data_ind_msg_t *msg)
{
    t_obex_header_offsets headers;
    u_int16 offset_len;
    u_int8 status;
    t_obex_digest digest;
    t_obex_nonce nonce;
    t_DataBuf *txBuffer;
    u_int32 temp_cid;
    BT_TraceOutputText(0,"bt_map_con_cnf");

    if(bt_map_timer)
        rdabt_cancel_timer(bt_map_timer);
    bt_map_timer=0;
    if(bt_map_step==0)
    {
        if(*(msg->rx_buffer)==OBEX_OK)
            OBEX_Disconnect(msg->tid);
        return;
    }
    offset_len=OBEX_CONNECT_HEADER;
    if(!msg->rx_buffer)
    {
        bt_pbap_con_discon_info *param_ptr;
        BT_TraceOutputText(0,"MAP_connect_cnf con fail!!!!!!!!!!!");
        param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
        param_ptr->tid=0;
        param_ptr->status=PBAP_DISCON_OPPER_UNSUPPORT;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MAP_DISCON_CNF, (void *)param_ptr, NULL);
        bt_map_step=0;
        return;
    }
    OBEX_ExtractHeaders(&headers,&offset_len,msg->rx_buffer,msg->length);
    BT_TraceOutputText(0,"bt_map_con_cnf 0x%x",*(msg->rx_buffer));

    switch(*(msg->rx_buffer))
    {
        case OBEX_OK:
            bt_map_tid=msg->tid;
            if(headers.cid)
            {
                if(bt_map_step == 2)
                    bt_map_cid = ((msg->rx_buffer+headers.cid)[0] << 24) + ((msg->rx_buffer+headers.cid)[1] << 16) + ((msg->rx_buffer+headers.cid)[2] << 8) + (msg->rx_buffer+headers.cid)[3];
            }
            else
            {
                bt_map_cid = 0;

            }
            if(headers.au_rsp)
            {
                status=OBEX_AuthCheck("0000", 4, nonce, msg->rx_buffer+headers.au_rsp);
                if(status)
                {
                    offset_len = OBEX_HEADER;
                    OBEX_GetWriteBuffer(100, &txBuffer);
                    temp_cid = ((msg->rx_buffer+headers.cid)[0] << 24) + ((msg->rx_buffer+headers.cid)[1] << 16) + ((msg->rx_buffer+headers.cid)[2] << 8) + (msg->rx_buffer+headers.cid)[3];
                    status=OBEX_Create4ByteHeader(OBEX_CID,temp_cid,txBuffer,&offset_len);
                    if(bt_map_step == 2)
                        status=OBEX_TransportDisconnect(bt_map_tid);
                    return;
                }
            }
            break;
        case OBEX_UNAUTHORIZED:
            if(headers.au_rsp)
            {
                status=OBEX_AuthCheck("0000", 4, nonce, msg->rx_buffer+headers.au_rsp);
                if(status)
                {
                    offset_len = OBEX_HEADER;
                    OBEX_GetWriteBuffer(100, &txBuffer);
                    temp_cid = ((msg->rx_buffer+headers.cid)[0] << 24) + ((msg->rx_buffer+headers.cid)[1] << 16) + ((msg->rx_buffer+headers.cid)[2] << 8) + (msg->rx_buffer+headers.cid)[3];
                    status=OBEX_Create4ByteHeader(OBEX_CID,temp_cid,txBuffer,&offset_len);
                    if(bt_map_step == 2)
                        status=OBEX_TransportDisconnect(bt_map_tid);
                    return;
                }

            }
            if(headers.au_cal)
            {
                OBEX_GetWriteBuffer(300, &txBuffer);
                offset_len=OBEX_CONNECT_HEADER;
                status=OBEX_AuthResponse("0000", 4, msg->rx_buffer+headers.au_cal, &offset_len,txBuffer);
            }
            else
            {
            }
            break;
        default:
            break;
    }

    return;

}

void deal_msglist_data(void)
{
    u_int8 processed_num=0,temp_data=0;
    char *p=NULL,*q=NULL,*temp=NULL;
    BT_TraceOutputText(0,"deal_msglist_data result_data=%x",result_data);
    if(result_data==0)
    {
        BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 0");
        return;
    }
    memset(bt_map_msg_array,0,sizeof(bt_map_message_info)*10);
    p=strstr(result_data,"msg handle");
    if(!p)
        return;
    if(*(p+10)==' ')
        p+=14;
    else
        p+=12;
    if(!p)
    {
        BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 1");
        return;
    }
    while(p)
    {
        bt_map_msg_array[processed_num].handle=strtol(p,&temp,16);//(const char * nptr, char * * endptr, int base)
        //BT_TraceOutputText(0,"deal list handle %x *p=%x,%x,%x,%x",bt_map_msg_array[processed_num].handle,*p,*(p+1),*(p+2),*(p+3));

        p=strstr(p,"datetime");
        if(!p)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 2");
            return;
        }
        if(*(p+8)==' ')
            p+=12;
        else
            p+=10;

        q=strstr(p,"\"");
        if(!q)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 3");
            return;
        }
        strncpy(    bt_map_msg_array[processed_num].msg_time,p,q-p);
        bt_map_msg_array[processed_num].msg_time[16]='\0';

#if 0
        p=strstr(q,"sender_name");
        if(!p)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 4");
            return;
        }
        if(*(p+11)==' ')
            p+=15;
        else
            p+=13;

        q=strstr(p,"\"");
        if(!q)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 5");
            return;
        }
#endif
        //      temp_data=*q;
        //*q='\0';
        //bt_map_msg_array[processed_num].msg_name_length=UTF8_to_Unicode(  bt_map_msg_array[processed_num].msg_name, (u_int8 *)p)/2;
        //*q=temp_data;
        //BT_TraceOutputText(0,"deal list name %x,%x,%x,%x,%x,%x",*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5));

        p=strstr(q,"sender_addressing");
        if(!p)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 6");
            return;
        }
        if(*(p+17)==' ')
            p+=21;
        else
            p+=19;
        q=strstr(p,"\"");
        if(!q)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 7");
            return;
        }
        strncpy(    bt_map_msg_array[processed_num].msg_tel,p,q-p);
        bt_map_msg_array[processed_num].msg_tel[PB_LENGTH-1]='\0';
        BT_TraceOutputText(0,"deal list  hhhhhhandle=%x number=%s",bt_map_msg_array[processed_num].handle,bt_map_msg_array[processed_num].msg_tel);

        p=strstr(q,"read");
        if(!p)
        {
            BT_TraceOutputText(0,"+++++++++++++deal_msglist_data error return 8");
            return;
        }
        if(*(p+4)==' ')
            p+=7;
        else
            p+=5;
        if(*p=='y')
            bt_map_msg_array[processed_num].readed_status=1;
        else
            bt_map_msg_array[processed_num].readed_status=0;

        p=strstr(q,"msg handle");
        if(!p)
            return;
        if(*(p+10)==' ')
            p+=14;
        else
            p+=12;

        processed_num++;

    }


}
#if 1
u_int8 assert_flag=1;
u_int16 assert_handle=0x505;

void deal_msg_data(void)
{
    char *p=NULL,*q=NULL;
    //BT_TraceOutputText(0,"deal_msg_data1 current handle=array[%d]---%x",current_array_index,bt_map_msg_array[current_array_index].handle);
    p=strstr(result_data,"BEGIN:MSG");
    if(!p)
        return;
    p=p+11;
    q=strstr(p,"\r\n");
    if(!q)
        return;
    *q='\0';
    bt_map_msg_array[current_array_index].msg_length=UTF8_to_Unicode(bt_map_msg_array[current_array_index].msg, (u_int8 *)p)/2;

    //if(assert_flag)
    //if(bt_map_msg_array[current_array_index].handle=assert_handle)
    //asm("break 1");
    BT_TraceOutputText(0,"d msg=array[%d]---handle:%x tel:%s msg_len=%d msg[0~2]%x %x %x",
                       current_array_index,bt_map_msg_array[current_array_index].handle
                       ,bt_map_msg_array[current_array_index].msg_tel
                       ,bt_map_msg_array[current_array_index].msg_length
                       ,bt_map_msg_array[current_array_index].msg[0]
                       ,bt_map_msg_array[current_array_index].msg[1]
                       ,bt_map_msg_array[current_array_index].msg[2]);

}
#else
void deal_msg_data(void)
{
    char *p=NULL,*q=NULL;
    BT_TraceOutputText(0,"deal_msg_data1 current handle=array[%d]---%x",current_array_index,bt_map_msg_array[current_array_index].handle);
    //if(bt_map_msg_array[current_array_index].handle=0x395)
    //asm("break 1");
    p=strstr(result_data,"TEL:");
    if(!p)
        return;
    q=strstr(p,"\r\n");
    if(!q)
        return;
    *q='\0';
    BT_TraceOutputText(0,"deal_msg_data2 tel=%s",p);

}
#endif
void bt_map_get_cnf(obex_cli_data_ind_msg_t *msg)
{
    u_int16 offset;
    t_obex_header_offsets headers;
    t_DataBuf *buffer;
    u_int8 *finger = msg->rx_buffer;

    if(result_data == 0x00)
    {
        result_data = (u_int8*)pMalloc(1300);
        result_length = 0;
        buffer_length = 1300;
    }

    if(msg->length + result_length > buffer_length)
    {
        u_int8* temp;
        temp = (u_int8*)pMalloc(msg->length + result_length);
        buffer_length = result_length + msg->length;
        memcpy(temp,result_data,result_length);
        pFree(result_data);
        result_data = temp;
    }

    offset = OBEX_HEADER;
    OBEX_ExtractHeaders(&headers, &offset,finger,msg->length);
    BT_TraceOutputText(0,"bt_map_get_cnf msg->typeorstatus=0x%x,0x%x,0x%x,0x%x,type=%d,index=%d",msg->typeorstatus,headers.body,headers.app,headers.eob,current_get_type,current_array_index);
    switch(msg->typeorstatus)
    {
        case OBEX_OK:
            if(headers.body)
            {
                u_int16 temp;
                temp = ((finger + headers.body)[0] << 8) + (finger + headers.body)[1] -OBEX_HEADER;
                memcpy(result_data + result_length, finger + headers.body + 2,temp);
                result_length += temp;
            }

            if(headers.app && bt_map_size == 0)
            {
                u_int8 *tmp = (finger + headers.app);

                while(*tmp != 0x12)
                {
                    tmp++;
                }
                tmp += 2;
                bt_map_size = (*tmp << 8) + (*(tmp + 1));
                BT_TraceOutputText(0,"bt_map_get_cnf,bt_map_size=%d",bt_map_size);

            }
            if(headers.eob)
            {
                u_int16 temp;
                temp = ((finger + headers.eob)[0] << 8) + (finger + headers.eob)[1] - OBEX_HEADER;
                memcpy(result_data + result_length, finger + headers.eob + 2,temp);
                result_length += temp;
                //BT_TraceOutputText(0,"bt_map_get_cnf,current_get_type=%d,current_array_index=%d",current_get_type,current_array_index);

                if(result_length>0)
                {
                    if(current_get_type==1)
                    {
                        deal_msglist_data();
                        // asm("break 1");
                        current_array_index=0;
                        OBEX_GetMsg(bt_map_tid, bt_map_cid, MSG_INBOX, bt_map_msg_array[current_array_index].handle);
                        current_get_type=2;

                    }
                    else if(current_get_type==2)
                    {
                        deal_msg_data();
                        current_array_index++;
                        if((current_array_index>=10)||((bt_map_msg_array[current_array_index].handle==0)&&!strlen(bt_map_msg_array[current_array_index].msg_tel)))
                        {
                            bt_map_data_ind_struct *param_ptr=(bt_map_data_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_map_data_ind_struct), TD_CTRL);
                            param_ptr->tid=bt_map_tid;
                            param_ptr->all_item_num=bt_map_size;
                            param_ptr->status=0;
                            memcpy(param_ptr->map_msg_array,bt_map_msg_array,sizeof(bt_map_msg_array));
                            if(finished_get_map_size)
                            {

                                BT_TraceOutputText(0,"%%%%%%%%%%%%%%%%%%%finished");
                                param_ptr->current_item_num=remain_map_size;

                                have_got_map_size=0;
                                remain_map_size=0;
                                bt_map_size=0;
                                current_array_index=0;
                                finished_get_map_size=0;
                                OBEX_Disconnect(bt_map_tid);
                            }
                            else
                            {
                                param_ptr->current_item_num=10;

                                have_got_map_size+=10;
                                remain_map_size=bt_map_size-have_got_map_size;
                                current_get_type=1;
                                current_array_index=0;
                                if(remain_map_size>10)
                                {
                                    OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, have_got_map_size, 10);
                                    BT_TraceOutputText(0,"******************bt_map_get_cnf get next 10 item have_got_map_size=%d",have_got_map_size);
                                }
                                else if (remain_map_size>0)
                                {
                                    OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, have_got_map_size, remain_map_size);
                                    //remain_map_size=bt_map_size;
                                    finished_get_map_size=1;
                                    BT_TraceOutputText(0,"******************bt_map_get_cnf get last %d item have_got_map_size=%d",remain_map_size,have_got_map_size);
                                }

                            }

                            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MAP_DATA_IND, (void *)param_ptr, NULL);


                        }
                        else
                        {
                            OBEX_GetMsg(bt_map_tid, bt_map_cid, MSG_INBOX, bt_map_msg_array[current_array_index].handle);
                            current_get_type=2;

                        }


                    }

                }
                else
                {
                    if(bt_map_size>10)
                        OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0, 10);
                    else if(bt_map_size>0)
                    {
                        OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0, bt_map_size);
                        remain_map_size=bt_map_size;
                        finished_get_map_size=1;
                    }
                    current_get_type=1;
                }
            }
            else
            {
                if(bt_map_size>10)
                    OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0, 10);
                else if(bt_map_size>0)
                {
                    OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0, bt_map_size);
                    remain_map_size=bt_map_size;
                    finished_get_map_size=1;
                }
                current_get_type=1;
            }
            pFree(result_data);
            result_data = 0x00;

            break;
        case OBEX_CONTINUE:
            if(headers.body)
            {
                u_int16 temp;
                temp = ((finger + headers.body)[0] << 8) + (finger + headers.body)[1] -OBEX_HEADER;
                memcpy(result_data + result_length, finger + headers.eob + 2,temp);
                result_length += temp;
            }
#ifdef DOWN_ABORT_TEST
            OBEX_Abort(bt_map_tid);
#else
            OBEX_GetWriteBuffer(0,&buffer);
            /* I need to get the tid some how */
            OBEX_Get(bt_map_tid,1,0,buffer);
#endif
            break;

        default:
            BT_TraceOutputText(0,"%%%%%%%%%%%%%%%%%%%FAIL %x",msg->typeorstatus);
            if(0)//(msg->typeorstatus==OBEX_NOT_FOUND)
            {
                OBEX_GetMsg(bt_map_tid, bt_map_cid, MSG_INBOX, bt_map_msg_array[current_array_index].handle);

            }
            else
            {
                have_got_map_size=0;
                remain_map_size=0;
                bt_map_size=0;
                current_array_index=0;
                OBEX_Disconnect(bt_map_tid);
                pFree(result_data);
                result_data = 0x00;
            }
            break;
    }
}

void rdabt_map_data_ind_callback(obex_cli_data_ind_msg_t *map_data_message)
{
//BT_TraceOutputText(0,"rdabt_map_data_ind_callback process_pending=%x,bt_map_step=%x",
    //map_data_message->process_pending,bt_map_step);
    switch(map_data_message->process_pending)
    {
        case OBEX_CONNECT_CNF_ID :
            bt_map_con_cnf(map_data_message);
            BT_TraceOutputText(0,"rdabt_pbap_data_ind_callback bt_map_tid=%d",bt_map_tid);
            if(bt_map_step == 2)
                OBEX_SetMap(bt_map_tid, bt_map_cid, MSG_INBOX, 1);
            break;
        case OBEX_DISCONNCET_CNF_ID:
        {
            BT_TraceOutputText(0,"OBEX_DISCONNCET_CNF_ID tid=%x",map_data_message->tid);
            OBEX_TransportDisconnect(map_data_message->tid);

            bt_pbap_con_discon_info *param_ptr;
            if(bt_map_timer)
                rdabt_cancel_timer(bt_map_timer);
            bt_map_timer=0;
            if(bt_map_step==0)
                return;
            bt_map_step=0;
            param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
            param_ptr->tid=bt_map_tid;
            param_ptr->status=PBAP_DISCON_SUCCSEE;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MAP_DISCON_CNF, (void *)param_ptr, NULL);
            OBEX_TransportDisconnect(map_data_message->tid);
        }
        break;

        case OBEX_ABORT_ID:
            OBEX_Disconnect(map_data_message->tid);
            break;

        case OBEX_PUT_ID:
            break;

        case OBEX_GET_ID   :
            if(bt_map_step > 1)
                bt_map_get_cnf(map_data_message);
            /*if(bt_map_step == 2 && bt_map_size > 0)
            {
            #if 0
                OBEX_GetMsg(mas_tid, mas_cid, MSG_INBOX, 768);
                mas_step = 3;
            #else
                OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0,
                    (bt_map_size > 4) ? 4 : bt_map_size);
                //current_get_pos = 0;
                bt_map_step = 3;
            #endif
            }*/
            break;

        case   OBEX_SET_PATH_ID:
        {
            if(bt_map_step == 2)
                OBEX_GetMsgList(bt_map_tid, bt_map_cid, MSG_INBOX, 0, 0);

        }
        break;

        default:

            break;
    }
}

#ifdef __SUPPORT_SPP_SYNC__
#ifdef __SUPPORT_BT_SYNC_PHB__
extern struct_pbap_info pbap_info_array[10];
void deal_pbap_data_for_spp(char *buffer)
{
    char *begin=NULL,*end=NULL;
    begin=strstr(buffer,"BEGIN:VCARD");
    if(!begin)
        return;
    end=strstr(buffer,"END:VCARD");
    if(!end)
        return;
    Get_pbap_item_info(begin,end,&pbap_info_array[0]);
    {
        bt_pbap_data_ind_struct *param_ptr;
        param_ptr = (bt_pbap_data_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_pbap_data_ind_struct), TD_CTRL);
        param_ptr->status=0;
        param_ptr->tid=0;
        param_ptr->all_item_num=1;
        param_ptr->current_item_num=1;
        param_ptr->con_type=0;
        param_ptr->is_spp_ind = 1;
        memcpy(param_ptr->pbap_data_array.pbap_info_array,pbap_info_array,sizeof(pbap_info_array));
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DATA_IND, (void *)param_ptr, NULL);
    }
}
#endif
void deal_msg_data_for_spp(char *buffer)//(u_int8 *buffer,bt_map_message_info *msg_info)
{
    char temp_data;
    char *p=NULL,*q=NULL,*temp=NULL;


    if(!buffer)
        return;
    p=strstr(buffer,"BEGIN:BMSG");
    if(!p)
        return;
    p=strstr(p,"BEGIN:VCARD");
    if(!p)
        return;
    q=strstr(p,"END:VCARD");
    if(!q)
        return;
    temp_data=*q;
    *q='\0';
    temp=q;

    p=strstr(p,"\r\nN:");
    if(!p)
        return;
    p+=4;
    q=strstr(p,"\r\n");
    if(!q)
        return;

    *q='\0';



    bt_map_msg_array[0].msg_name_length=UTF8_to_Unicode(    bt_map_msg_array[0].msg_name, (u_int8 *)p)/2;
    q++;
    p=strstr(q,"TEL:");
    if(!p)
        return;
    p+=4;
    q=strstr(p,"\r\n");
    if(!q)
        return;

    //清空上次的电话号码
    memset(bt_map_msg_array[0].msg_tel,0,PB_LENGTH);

    strncpy(    bt_map_msg_array[0].msg_tel,p,q-p);
    bt_map_msg_array[0].msg_tel[PB_LENGTH-1]='\0';
    *temp=temp_data;

    p=strstr(q,"DATETIME:");
    if(!p)
        return;
    q=strstr(p,"\r\n");
    if(!q)
        return;
    p+=9;
    strncpy(    bt_map_msg_array[0].msg_time,p,q-p);
    bt_map_msg_array[0].msg_time[16]='\0';

    p=strstr(q,"BEGIN:MSG");
    if(!p)
        return;
    p+=11;
    q=strstr(p,"\r\n");
    if(!q)
        return;
    *q='\0';
    bt_map_msg_array[0].msg_length=UTF8_to_Unicode(bt_map_msg_array[0].msg, (u_int8 *)p);

    {
        bt_map_data_ind_struct *param_ptr=(bt_map_data_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_map_data_ind_struct), TD_CTRL);
        param_ptr->tid=bt_map_tid;
        param_ptr->all_item_num=1;
        param_ptr->status=0;
        param_ptr->current_item_num=1;
        memcpy(param_ptr->map_msg_array,bt_map_msg_array,sizeof(bt_map_msg_array));

        mmi_btphone_push_sms_req( (void*)param_ptr);
    }
    return;

}

void mmi_btphone_push_sms_req(void* msg)
{
    MYQUEUE Message;
    module_type dest_mod_id = MOD_L4C;

    Message.oslSrcId = MOD_BT;
    Message.oslDestId = dest_mod_id;
    Message.oslMsgId = PRT_MSG_ID_BT_PUSH_SMS_REQ;
    Message.oslDataPtr = (oslParaType *)msg;
    Message.oslPeerBuffPtr = NULL;

    OslMsgSendExtQueue(&Message);
}

#endif

#endif
