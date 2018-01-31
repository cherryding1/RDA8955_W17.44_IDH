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

#ifdef __SUPPORT_BT_SYNC_PHB__

#include "rdabt.h"
#include "bt.h"
#include "papi.h"
#include "bluetooth_struct.h"
#include "obex.h"
#include "bt_dm_struct.h"
#include "bt_hfg_struct.h"
#include "rdabt_pbap.h"

u_int32 pb_cid = 0;
u_int32 pb_tid = 0;
t_bdaddr pbap_addr;
extern u_int16 last_client_connect;


u_int8 phonebook_step;
u_int8 history_step;


int history_size = 0;
static u_int8 *result_data;
u_int16 result_length;
u_int16 buffer_length;

u_int8 pbap_timer=0;
static u_int16 pbap_item_num=0;
static u_int16 have_got_pbap_item_num=0;
static u_int16 remain_pbap_item_num=0;
static u_int16 get_phone_book_finished=0;
struct_pbap_info pbap_info_array[10];
struct_pbap_com_history_info pbap_histoy_call_info_array[10];
static u_int8 current_pbap=0;
u_int16 current_num=0;
//extern u_int32 *inputbuf_overlay;
extern uint32 pcmbuf_overlay[4608];

void Process_pbap_data(u_int8 item_num);

void PB_Download_Timeout(void)
{
    bt_pbap_con_discon_info *param_ptr;
    BT_TraceOutputText(0,"PB_Download_Timeout called PBAP_DISCON_OPPER_NOT_ACCEPT=%d",PBAP_DISCON_OPPER_NOT_ACCEPT);
    rdabt_cancel_timer(pbap_timer);

    param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
    param_ptr->con_type=current_pbap;
    param_ptr->tid=0;
    param_ptr->status=PBAP_DISCON_OPPER_NOT_ACCEPT;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DISCON_CNF, (void *)param_ptr, NULL);

    history_step=0;
    phonebook_step=0;
    pbap_timer = 0;
}


void pbap_phone_book_con_req(t_bdaddr addr,u_int8 con_type)
{
    BT_TraceOutputText(0,"pbap_phone_book_con_req called,con_type=%d,add=%x,0x%x,0x%x,0x%x,0x%x,0x%x",con_type,addr.bytes[0],addr.bytes[1],addr.bytes[2],addr.bytes[3],addr.bytes[4],addr.bytes[5]);
    last_client_connect =SCLASS_UUID_PHONEBOOK_ACCESS_PSE;
    memcpy(pbap_addr.bytes,addr.bytes,6);
    if(con_type==0)//con phone books
    {
        phonebook_step = 2;
        current_pbap=PHONE_BOOK_PHONE_BOOK;

    }
    else
    {
        history_step=2;
        current_pbap=PHONE_BOOK_COMBINED_HISTORY;

    }

    pbap_timer =rdabt_timer_create(15000,  PB_Download_Timeout,NULL,pTIMER_ONESHOT);
    OBEX_ConnectRequest(&pbap_addr,SCLASS_UUID_PHONEBOOK_ACCESS_PSE,pcmbuf_overlay);

}

extern u_int8 get_transport_tid(t_bdaddr address);
void pbap_discon_req(void)
{
    BT_TraceOutputText(0,"pbap_discon_req con pb_tid=%x,pbap_timer=%d",pb_tid,pbap_timer);
    if(pbap_timer)
    {
        bt_pbap_con_discon_info *param_ptr;
        rdabt_cancel_timer(pbap_timer);
        OBEX_TransportDisconnect(get_transport_tid(pbap_addr));
        param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
        param_ptr->con_type=current_pbap;
        param_ptr->tid=pb_tid;
        param_ptr->status=PBAP_DISCON_SUCCSEE;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DISCON_CNF, (void *)param_ptr, NULL);
    }
    else
        OBEX_Disconnect(pb_tid);

}
void pbap_connect_cnf(obex_cli_data_ind_msg_t *msg)
{
    t_obex_header_offsets headers;
    u_int16 offset_len;
    u_int8 status;
    t_obex_digest digest;
    t_obex_nonce nonce;
    t_DataBuf *txBuffer;
    u_int32 temp_cid;
    BT_TraceOutputText(0,"pbap_connect_cnf pbap_timer=%x,%d,%d",pbap_timer,history_step,phonebook_step);
    if(pbap_timer)
        rdabt_cancel_timer(pbap_timer);
    pbap_timer=0;
    if((history_step==0)&&(phonebook_step==0))
    {
        if(msg->rx_buffer)
        {
            if(*(msg->rx_buffer)==OBEX_OK)
                OBEX_Disconnect(msg->tid);
        }
        return;
    }
    offset_len=OBEX_CONNECT_HEADER;
    current_num=0;

    if(!msg->rx_buffer)
    {
        bt_pbap_con_discon_info *param_ptr;
        BT_TraceOutputText(0,"pbap_connect_cnf con fail!!!!!!!!!!!");
        param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
        param_ptr->con_type=current_pbap;
        param_ptr->tid=0;
        param_ptr->status=PBAP_DISCON_OPPER_UNSUPPORT;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DISCON_CNF, (void *)param_ptr, NULL);
        history_step=0;
        phonebook_step=0;
        return;
    }
    OBEX_ExtractHeaders(&headers,&offset_len,msg->rx_buffer,msg->length);
    BT_TraceOutputText(0,"pbap_connect_cnf,*(msg->rx_buffer)=%x,cid=%d,au_rsp=%x,msg->tid=%d",*(msg->rx_buffer),headers.cid,headers.au_rsp,msg->tid);

    switch(*(msg->rx_buffer))
    {
        case OBEX_OK:
            pb_tid = msg->tid;
            if(headers.cid)
            {
                if(history_step == 2 || phonebook_step == 2)
                    pb_cid = ((msg->rx_buffer+headers.cid)[0] << 24) + ((msg->rx_buffer+headers.cid)[1] << 16) + ((msg->rx_buffer+headers.cid)[2] << 8) + (msg->rx_buffer+headers.cid)[3];

            }
            else
            {
                pb_cid = 0;
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

                    if(history_step == 2 || phonebook_step == 2)
                        status=OBEX_TransportDisconnect(pb_tid);
                    return;
                }

            }
            if(phonebook_step == 2 || history_step == 2)
                OBEX_SetPhoneBookFormat(pb_tid,PHONE_BOOK_FIELD_FN|PHONE_BOOK_FIELD_TEL,0,PHONE_BOOK_VCARD3_0, RDABT_PBAP_SORT_INDEX);

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
                    if(history_step == 2 || phonebook_step == 2)
                        status=OBEX_TransportDisconnect(pb_tid);
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

int UTF8_to_Unicode(u_int16 *dst, const u_int8 *src)
{
    int i = 0, ii, iii;
    u_int16  unicode = 0;
    int codeLen = 0;
    BOOL is_chinese = FALSE;
    while ( *src )
    {
        //1. UTF-8 ---> Unicode
        if(0 == (src[0] & 0x80))
        {
            // 单字节
            codeLen = 1;
            unicode = src[0];
        }
        else if(0xC0 == (src[0] & 0xE0) && 0x80 == (src[1] & 0xC0))
        {
            // 双字节
            codeLen = 2;
            unicode = (int)((((int)src[0] & 0x001F) << 6) | ((int)src[1] & 0x003F));
        }
        else if(0xE0 == (src[0] & 0xF0) && 0x80 == (src[1] & 0xC0) && 0x80 == (src[2] & 0xC0))
        {
            // 三字节
            codeLen = 3;
            is_chinese = TRUE;
            ii = (((int)src[0] & 0x000F) << 12);
            iii = (((int)src[1] & 0x003F) << 6);
            unicode = ii|iii|((int)src[2] & 0x003F);
            unicode = (int)((((int)src[0] & 0x000F) << 12) | (((int)src[1] & 0x003F) << 6) | ((int)src[2] & 0x003F));
        }
        else if(0xF0 == (src[0] & 0xF0) && 0x80 == (src[1] & 0xC0) && 0x80 == (src[2] & 0xC0) && 0x80 == (src[3] & 0xC0))
        {
            // 四字节
            codeLen = 4;
            unicode = (((int)(src[0] & 0x07)) << 18) | (((int)(src[1] & 0x3F)) << 12) | (((int)(src[2] & 0x3F)) << 6) | (src[3] & 0x3F);
        }
        else
        {
            break;
        }
        src += codeLen;
        if (unicode < 0x80)
        {
            if (i == 0 && unicode == 0x20)
            {
                continue;
            }
        }
        if(codeLen > 1)
        {
            i += 2;
        }
        else
        {
            i+=2;
        }

        //if(codeLen>1)
        {
            //*dst++ = (char)(unicode&0xff);
            //*dst++ = (char)((unicode>>8)&0xff);
            *dst++=unicode;
        }
        //else
        {
            //  *dst++ = unicode;
        }
    } // end while
    *dst = 0;
    //if(!is_chinese)
    //    i = 0;
    return i;
}





void Get_pbap_history_item_info(char *start_p,char* end_p,struct_pbap_com_history_info *info_array)
{
    char *p=NULL,*q=NULL;
    u_int8 length=0;
    //frst get name
    p=strstr(start_p,"FN");
    p=strstr(p,":");
    if(!p)
        return  ;
    q=strstr(p,"\r\n");
    p+=1;
    length=q-p;
    if(length>=PB_LENGTH)
    {
        length=PB_LENGTH;
    }
    p[length]='\0';
    info_array->name_length=UTF8_to_Unicode(info_array->name, (u_int8 *)p)/2;



    // get the phone_number

    p=q+2;
    p=strstr(p,"TEL");
    if(!p)
        return  ;
    p=strstr(p,":");
    q=strstr(p,"\r\n");
    p+=1;
    length=q-p;
    if(length>=PB_LENGTH)
    {
        length=PB_LENGTH;
        memcpy(info_array->phone_num,p,length);

        info_array->phone_num[length-1]='\0';
    }
    else
        memcpy(info_array->phone_num,p,length);


//get the call type
    p=q+2;

    p=strstr(p,"X-IRMC");
    if(!p)
        return  ;
    p=strstr(p,"=");
    p++;
    q=strstr(p,":");
    *q='\0';
    if(!memcmp(p,"DIALED",strlen("DIALED")))
        info_array->call_type=PBAP_DIALED_CALL;
    else if(!memcmp(p,"RECEIVED",strlen("RECEIVED")))
        info_array->call_type=PBAP_RECEIVED_CALL;
    else// if(!memcmp(p,"MISSED",strlen("MISSED")))
        info_array->call_type=PBAP_MISSED_CALL;



//get the data and time
    q++;
    memcpy(info_array->data_time,q,16);
    info_array->data_time[16]='\0';
    return;
}


void Get_pbap_item_info(char *start_p,char* end_p,struct_pbap_info *info_array)
{
    char *p=NULL,*q=NULL,*ind=NULL;
    u_int8 length=0, index=0;

    p=strstr(start_p,"FN:");
    if(!p)
    {
        p=strstr(start_p,"FN;");
        if(!p)
            return ;
        p+=14;

    }
    q=strstr(p,"\n");
    if(!q)
        return ;
    p+=3;
    if(*(q-1)=='\r')
        length=q-p-1;
    else
        length=q-p;
    if(length>=PB_LENGTH)
    {
        length=PB_LENGTH;
        //memcpy(info_array->name,p,length);
        //info_array->name[length-1]='\0';
    }
    //else
    //memcpy(info_array->name,p,length);
    p[length]='\0';
    info_array->name_length=UTF8_to_Unicode(info_array->name, (u_int8 *)p)/2;

    p=q+1;
    p=strstr(p,"TEL");
    if(!p)
        return ;
    p=strstr(p,":");
    if(!p)
        return ;
    q=strstr(p,"\r");
    if(!q)
        return ;
    p+=1;


    ind=p;
    index=0;
    while((ind<q)&&(index<(PB_LENGTH-1)))
    {
        if(*ind!='-')
        {
            info_array->phone_num[0][index++]=*ind;

        }
        ind++;

    }
    info_array->phone_num[0][index]='\0';



    if(*(q+1)=='\n')
        p=q+2;
    else
        p=q+1;

    p=strstr(p,"TEL");
    if(!p)
        return ;
    p=strstr(p,":");
    if(!p)
        return ;
    q=strstr(p,"\r");

    p+=1;


    ind=p;
    index=0;
    while((ind<q)&&(index<(PB_LENGTH-1)))
    {
        if(*ind!='-')
        {
            info_array->phone_num[1][index++]=*ind;

        }
        ind++;

    }
    info_array->phone_num[1][index]='\0';

    if(!q)
        return ;

    if(*(q+1)=='\n')
        p=q+2;
    else
        p=q+1;

    p=strstr(p,"TEL");
    if(!p)
        return ;
    p=strstr(p,":");
    if(!p)
        return ;
    q=strstr(p,"\r");
    if(!q)
        return ;
    p+=1;

    ind=p;
    index=0;
    while((ind<q)&&(index<(PB_LENGTH-1)))
    {
        if(*ind!='-')
        {
            info_array->phone_num[2][index++]=*ind;

        }
        ind++;

    }
    info_array->phone_num[2][index]='\0';




    return ;
}


void Process_pbap_data(u_int8 item_num)
{
    char *begin=NULL,*end;
    char *temp=(char *)result_data+5;
    u_int16 processed_num=0;
    BT_TraceOutputText(0,"Process_pbap_data called item_num!!!!!!!!!!!!!!!!=%d",item_num);
    if(  current_pbap==PHONE_BOOK_PHONE_BOOK)
        memset(pbap_info_array,0,sizeof(struct_pbap_info)*10);
    else
        memset(pbap_histoy_call_info_array,0,sizeof(struct_pbap_com_history_info)*10);
    if(0)//((result_data_current_index==result_data)&&(current_pbap==PHONE_BOOK_PHONE_BOOK))//skip first empty item
    {
        current_num++;
        begin=temp;
        end=strstr(temp,"END");
        *end='\0';
        //Get_pbap_item_info(begin,end);
        temp=end+11;
        processed_num++;
    }
    if(item_num>0)
        while(processed_num<item_num)
        {
            char *name,*num;
            begin=temp;
            end=strstr(temp,"BEGIN");
            if(!end)
            {
                end=strstr(temp,"FN:");

                BT_TraceOutputText(0,"***pro wil end,end=%x,p_num=%d",end,processed_num);

                if((processed_num==(item_num-1))||(end))
                    end=&result_data[result_length+1];// for samsung ,it's pbap last item maybe no end.
                else
                    return;

            }
            *end='\0';

            if(  current_pbap==PHONE_BOOK_PHONE_BOOK)
            {

                Get_pbap_item_info(begin,end,&pbap_info_array[processed_num]);





                BT_TraceOutputText(0,"spbap current_num=%d.num1=%s,n+%x %x %x",
                                   current_num,
                                   pbap_info_array[processed_num].phone_num[0],pbap_info_array[processed_num].name[0],pbap_info_array[processed_num].name[1],pbap_info_array[processed_num].name[2]);
                if(strlen(pbap_info_array[processed_num].phone_num[1]))
                    BT_TraceOutputText(0,"spbap  phone num2=%s\n",
                                       pbap_info_array[processed_num].phone_num[1]);
                if(strlen(pbap_info_array[processed_num].phone_num[2]))
                    BT_TraceOutputText(0,"spbap phonenum3=%s\n",
                                       pbap_info_array[processed_num].phone_num[2]);
            }
            else
            {
                Get_pbap_history_item_info(begin,end,&pbap_histoy_call_info_array[processed_num]);
                BT_TraceOutputText(0,"***histoy_call current_num=%d.name len=%d,unicoce_name[0~2]=0x%x 0x%x 0x%x ,num1_length=%d,num1=%s,data=%s",
                                   current_num,pbap_histoy_call_info_array[processed_num].name_length,
                                   pbap_histoy_call_info_array[processed_num].name[0],pbap_histoy_call_info_array[processed_num].name[1],pbap_histoy_call_info_array[processed_num].name[2],

                                   strlen(pbap_histoy_call_info_array[processed_num].phone_num),pbap_histoy_call_info_array[processed_num].phone_num
                                   ,pbap_histoy_call_info_array[processed_num].data_time);

            }




            temp=end+5;// jump "BEGIN"

            current_num++;

            processed_num++;


        }
}

void rdabt_pbap_get_next_req(void)
{

    BT_TraceOutputText(0,"rdabt_pbap_get_next_req remain_pbap_item_num=%d",remain_pbap_item_num);
    if(remain_pbap_item_num>10)
        OBEX_PullPhoneBook( pb_tid, pb_cid, current_pbap, have_got_pbap_item_num, 10); // get list content
    else if(remain_pbap_item_num>0)
    {
        OBEX_PullPhoneBook(  pb_tid, pb_cid, current_pbap, have_got_pbap_item_num, remain_pbap_item_num); // get list content
    }

}
void GetPhonBookCnf(obex_cli_data_ind_msg_t *msg)
{
    u_int16 offset;
    t_obex_header_offsets headers;
    t_DataBuf *buffer;
    u_int8 *finger = msg->rx_buffer;

    if(result_data == 0x00)
    {
        result_data = (u_int8*)pMalloc(700);
        memset(result_data,0,700);
        result_length = 0;
        buffer_length = 700;
    }

    if(msg->length + result_length+2 > buffer_length)
    {
        u_int8* temp;
        temp = (u_int8*)pMalloc(msg->length + result_length+2);//add 2 for process end.
        memset(temp,0,msg->length + result_length);
        buffer_length = result_length + msg->length;
        memcpy(temp,result_data,result_length);
        pFree(result_data);
        result_data = temp;
    }

    offset = OBEX_HEADER;
    OBEX_ExtractHeaders(&headers, &offset,finger,msg->length);
    BT_TraceOutputText(0,"GetPhonBookCnf msg->typeorstatus=%x,headers.body=%x,headers.eob=%x,headers.app=%x,%d,%d,%d",
                       msg->typeorstatus,headers.body,headers.eob,headers.app,pbap_item_num,have_got_pbap_item_num,remain_pbap_item_num);
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

            if(headers.app)
            {
                u_int8 *tmp = (finger + headers.app);

                while(*tmp != 0x08 && *tmp != 0x49)
                {
                    tmp++;
                }
                if(*tmp == 0x08)
                {
                    tmp += 2;
                    pbap_item_num = (*tmp << 8) + (*(tmp + 1));
                    BT_TraceOutputText(0,"GetPhonBookCnf pbap_item_num=%d",pbap_item_num);
                }
            }

            if(headers.eob)
            {
                u_int16 temp;
                temp = ((finger + headers.eob)[0] << 8) + (finger + headers.eob)[1] -OBEX_HEADER;
                memcpy(result_data + result_length, finger + headers.eob + 2,temp);
                result_length += temp;
                // BT_TraceOutputText(0,"temp=%d",temp);

            }

            if(get_phone_book_finished)
            {
                if(result_length>0)
                {
                    BT_TraceOutputText(0,"GetPhonBookCnf FINISHED result_length=%x ",result_length);
                    Process_pbap_data(remain_pbap_item_num);
                    {
                        bt_pbap_data_ind_struct *param_ptr;
                        param_ptr = (bt_pbap_data_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_pbap_data_ind_struct), TD_CTRL);

                        param_ptr->status=0;
                        param_ptr->tid=pb_tid;
                        param_ptr->all_item_num=pbap_item_num;
                        param_ptr->current_item_num=remain_pbap_item_num;
                        if(current_pbap==PHONE_BOOK_PHONE_BOOK)
                        {
                            param_ptr->con_type=0;
                            memcpy(param_ptr->pbap_data_array.pbap_info_array,pbap_info_array,sizeof(pbap_info_array));
                        }
                        else
                        {
                            param_ptr->con_type=1;
                            memcpy(param_ptr->pbap_data_array.history_info_array,pbap_histoy_call_info_array,sizeof(pbap_histoy_call_info_array));
                        }
                        param_ptr->is_spp_ind = 0;
                        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DATA_IND, (void *)param_ptr, NULL);

                    }
                    pFree(result_data);
                    result_data = 0x00;
                    //process
                }

                have_got_pbap_item_num=0;
                pbap_item_num=0;
                remain_pbap_item_num=0;
                get_phone_book_finished=0;

            }
            else
            {
                if(result_length>0)
                {

                    have_got_pbap_item_num+=10;
                    remain_pbap_item_num=pbap_item_num-have_got_pbap_item_num;
                    if(remain_pbap_item_num>10)
                        get_phone_book_finished=0;//OBEX_PullPhoneBook( pb_tid, pb_cid, current_pbap, have_got_pbap_item_num, 10); // get list content
                    else if(remain_pbap_item_num>0)
                    {
                        //OBEX_PullPhoneBook(  pb_tid, pb_cid, current_pbap, have_got_pbap_item_num, remain_pbap_item_num); // get list content
                        get_phone_book_finished=1;
                    }
                    //rdabt_timer_create(10,  rdabt_pbap_delay_get_timeout,NULL,pTIMER_ONESHOT);

                    Process_pbap_data(10);
                    {
                        bt_pbap_data_ind_struct *param_ptr;
                        param_ptr = (bt_pbap_data_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_pbap_data_ind_struct), TD_CTRL);
                        param_ptr->status=0;
                        param_ptr->tid=pb_tid;
                        param_ptr->current_item_num=10;
                        param_ptr->all_item_num=pbap_item_num;


                        if(current_pbap==PHONE_BOOK_PHONE_BOOK)
                        {
                            param_ptr->con_type=0;
                            memcpy(param_ptr->pbap_data_array.pbap_info_array,pbap_info_array,sizeof(pbap_info_array));
                        }
                        else
                        {
                            param_ptr->con_type=1;
                            memcpy(param_ptr->pbap_data_array.history_info_array,pbap_histoy_call_info_array,sizeof(pbap_histoy_call_info_array));
                        }
                        param_ptr->is_spp_ind = 0;
                        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DATA_IND, (void *)param_ptr, NULL);

                    }
                    //asm("break 1");

                    pFree(result_data);
                    result_data = 0x00;
                }
                else
                {
                    if(pbap_item_num>10)
                    {
                        OBEX_PullPhoneBook( pb_tid,pb_cid, current_pbap, 0, 10); // get list content
                    }
                    else if(pbap_item_num>0)
                    {
                        OBEX_PullPhoneBook( pb_tid,pb_cid, current_pbap, 0, pbap_item_num); // get list content
                        remain_pbap_item_num=pbap_item_num;
                        get_phone_book_finished=1;
                    }
                }
            }

            break;
        case OBEX_CONTINUE:
            if(headers.body)
            {
                u_int16 temp;
                temp = ((finger + headers.body)[0] << 8) + (finger + headers.body)[1] -OBEX_HEADER;
                memcpy(result_data + result_length, finger + headers.body + 2,temp);
                result_length += temp;
            }
#ifdef DOWN_ABORT_TEST
            OBEX_Abort(pb_tid);
#else
            OBEX_GetWriteBuffer(0,&buffer);
            /* I need to get the tid some how */
            OBEX_Get(pb_tid,1,0,buffer);
#endif
            break;
        case OBEX_NOT_FOUND:
        case OBEX_NOT_ACCEPTABLE:
        {
            /*bt_pbap_con_discon_info *param_ptr;

            param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
            param_ptr->con_type=current_pbap;
            param_ptr->tid=pb_tid;
            param_ptr->status=PBAP_DISCON_SUCCSEE;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DISCON_CNF, (void *)param_ptr, NULL); */
            OBEX_Disconnect(pb_tid);

            break;
        }
        default:
            pFree(result_data);
            result_data = 0x00;
            break;
    }
    return;
}

void rdabt_pbap_data_ind_callback(obex_cli_data_ind_msg_t *pbap_data_message)
{
//BT_TraceOutputText(0,"rdabt_pbap_data_ind_callback process_pending=%x,phonebook_step=%x,history_step=%x",
    //pbap_data_message->process_pending,phonebook_step,history_step);
    switch(pbap_data_message->process_pending)
    {
        case OBEX_CONNECT_CNF_ID :
            pbap_connect_cnf(pbap_data_message);
            BT_TraceOutputText(0,"rdabt_pbap_data_ind_callback pb_tid=%d",pb_tid);
            if(current_pbap == PHONE_BOOK_PHONE_BOOK)
                OBEX_SetPhoneBook(pb_tid, pb_cid, PHONE_BOOK_PHONE, PHONE_BOOK_PHONE_BOOK, 1);
            else     if(current_pbap == PHONE_BOOK_COMBINED_HISTORY)

                OBEX_SetPhoneBook(pb_tid, pb_cid, PHONE_BOOK_PHONE, PHONE_BOOK_COMBINED_HISTORY, 1);
            break;
        case OBEX_DISCONNCET_CNF_ID:
        {
            bt_pbap_con_discon_info *param_ptr;
            BT_TraceOutputText(0,"OBEX_DISCONNCET_CNF_ID %d,%d",history_step,phonebook_step);
            current_pbap=0;
            pb_tid=0;
            current_num=0;
            if(pbap_timer)
                rdabt_cancel_timer(pbap_timer);
            pbap_timer=0;
            if((history_step==0)&&(phonebook_step==0))
                return;
            history_step=0;
            phonebook_step=0;
            param_ptr = (bt_pbap_con_discon_info*)construct_local_para((kal_uint16)sizeof(bt_pbap_con_discon_info), TD_CTRL);
            param_ptr->con_type=current_pbap;
            param_ptr->tid=pb_tid;
            param_ptr->status=PBAP_DISCON_SUCCSEE;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PBAP_DISCON_CNF, (void *)param_ptr, NULL);
            OBEX_TransportDisconnect(pbap_data_message->tid);

        }


            //NOTIFY MMI DISCON CNF;
        break;
        case OBEX_ABORT_ID:
            OBEX_Disconnect(pbap_data_message->tid);
            break;
        case OBEX_PUT_ID:

            break;

        case OBEX_GET_ID   :
            GetPhonBookCnf(pbap_data_message);
            break;
        case   OBEX_SET_PATH_ID:
        {
            OBEX_PullvCardList(pb_tid,pb_cid,current_pbap,0,0);
            have_got_pbap_item_num=0;
            pbap_item_num=0;
            remain_pbap_item_num=0;
            get_phone_book_finished=0;
        }

        break;

        default:

            break;
    }
}

#endif



