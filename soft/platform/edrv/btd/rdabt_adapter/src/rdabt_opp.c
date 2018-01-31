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
#ifndef _RDABT_OPP_C
#define _RDABT_OPP_C


/* basic system service headers */
#include "stdlib.h"
#include "kal_release.h"        /* basic data type */
#include "stack_common.h"          /* message and module ids */
#include "stack_msgs.h"            /* enum for message ids */
#include "app_ltlcom.h"         /* task message communiction */
//#include "syscomp_config.h"         /* type of system module components */
//#include "task_config.h"          /* task creation */
#include "stacklib.h"              /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* event scheduler */
#include "stack_timer.h"        /* stack timer */
//#include "stdio.h"                  /* basic c i/o functions */
#include "gpio_sw.h"               /* for gpio */

#include "cos.h"

#include "bt.h"

#include "obex.h"
#include "sdp.h"


#include "bluetooth_bm_struct.h"
#include "mmi_trace.h"

#include "rdabt.h"
#include "rdabt_opp.h"
#include "papi.h"
#include "manager.h"
#include "rdabt_pbap.h"
//#include "btmmicm
#include "sxr_tim.h"
/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/
extern S32 UCS2Strlen(const S8 *arrOut);
extern void pFree(void *buffer);

/*****************************************************************************
* Local Variable
*****************************************************************************/

rdabt_oppc_struct rdabt_oppc;
rdabt_oppc_struct* rdabt_oppcp = &rdabt_oppc;

rdabt_ftpc_struct rdabt_ftpc;
rdabt_ftpc_struct* rdabt_ftpcp = &rdabt_ftpc;

rdabt_opps_struct rdabt_opps;
rdabt_opps_struct* rdabt_oppsp = &rdabt_opps;

rdabt_ftps_struct rdabt_ftps;
rdabt_ftps_struct* rdabt_ftpsp = &rdabt_ftps;

static  u_int8 NAME_obex;
static   t_DataBuf *FTPSbuffer;
u_int32 need_user_confirm;
u_int16 last_client_connect=0;

u_int8 oppc_packetType=0;
u_int8 oppc_waite_abort=0;

u_int8 oppc_waite_discon_flag=0;
u_int8 rdabt_discon_timer=0;

static u_int8 ServerSecondConnect_dlci=0xff;
extern u_int8 get_transport_tid(t_bdaddr address);
/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_register_server_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_register_server_req(goep_register_server_req_struct *msg_p)
{
    int status;
    goep_register_server_rsp_struct *param_ptr ;
    param_ptr = (goep_register_server_rsp_struct*) construct_local_para(sizeof(goep_register_server_rsp_struct), TD_CTRL);

    kal_prompt_trace(1, "rdabt_goep_register_server_req...  20090220-Rdabt_opp!@!!!!!! msg_p ->uuid_len=%d",msg_p ->uuid_len);

    if(msg_p ->uuid_len == 0)
    {
        rdabt_oppcp->goep_tidOPPC = 0xff;//rdabt_obex_transmit_transport' tid
        rdabt_oppcp->goep_tidOPPC_mmi = 0xff;
        rdabt_oppcp->req_idOPPC = 0x00;//just for mmi  MMI
        rdabt_oppcp->oppCstatus = GOEP_Client_Idle;
        rdabt_oppsp->goep_tid_OPPS = 0xffff;//rdabt_obex_server_transport's tid, also the same as prh_obex_state's server_id
        rdabt_oppsp->OPPSstatus = GOEP_Client_Idle;
        rdabt_oppsp->req_idOPPS = 0xff;//just for mmi rdabt_goep_disconnect_req and rdabt_goep_disconnect_req rdabt_goep_disconnect_rsp_opps
        rdabt_oppsp->OPPStid = 0xffff;//// tid is rdabt_obex_transmit_transport 's tid
        rdabt_oppsp->OPPSfirst = 1;
        rdabt_oppsp->buf_sizeOPPS = msg_p ->buf_size;
        rdabt_oppsp->buf_ptrOPPS = msg_p ->buf_ptr;
        OBEX_RegisterService(0,rdabt_oppsp->buf_ptrOPPS,rdabt_oppsp->buf_sizeOPPS,SDP_SCLASS_UUID_OBEX_PUSH );
        rdabt_oppsp->goep_tid_OPPS = 1;
        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0x18;
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->req_id = msg_p ->req_id ;
        param_ptr->rsp_code = 0 ;
        param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_REGISTER_SERVER_RSP, (void *)param_ptr, NULL);
    }
    else
    {
        int lcv = 0;

        rdabt_ftpsp->goep_tid_FTPS = 0xffff;
        rdabt_ftpsp->req_idFTPS = 0xff;
        rdabt_ftpsp->FTPSstatus = GOEP_Client_Idle;
        rdabt_ftpsp->FTPStid = 0xffff;
        rdabt_ftpsp->FTPSfirst = 1;
        rdabt_ftpsp->buf_sizeFTPS = msg_p ->buf_size;
        rdabt_ftpsp->buf_ptrFTPS = msg_p ->buf_ptr;
        OBEX_RegisterService(0,rdabt_ftpsp->buf_ptrFTPS,rdabt_ftpsp->buf_sizeFTPS,SDP_SCLASS_UUID_OBEX_FTP );
        rdabt_ftpsp->goep_tid_FTPS = 2;
        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0x18;
        param_ptr->uuid_len = msg_p ->uuid_len;
        for(lcv = 0; lcv < param_ptr->uuid_len; lcv++)
        {
            param_ptr->uuid[lcv] = msg_p->uuid[lcv];
        }
        param_ptr->req_id = msg_p ->req_id ;
        param_ptr->rsp_code = 0 ;
        param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_REGISTER_SERVER_RSP, (void *)param_ptr, NULL);
        {
            void rdabt_spp_active(void);

            rdabt_spp_active();

        }
    }

    return;
}

void rdabt_goep_init_reset(void) //called when deactive profile
{
    rdabt_oppcp->goep_tidOPPC = 0xff;
    rdabt_oppcp->req_idOPPC = 0xffff;
    rdabt_oppcp->oppCstatus = GOEP_Client_Idle;

    rdabt_oppsp->goep_tid_OPPS = 0xffff;
    rdabt_oppsp->req_idOPPS = 0xff;
    rdabt_oppsp->OPPStid = 0xffff;
    rdabt_oppsp->OPPSfirst=1;
    rdabt_oppsp->OPPSstatus = GOEP_Client_Idle;

    rdabt_ftpsp->goep_tid_FTPS =0xffff;
    rdabt_ftpsp->req_idFTPS = 0xff;
    rdabt_ftpsp->FTPStid = 0xffff;
    rdabt_ftpsp->FTPSfirst=1;
    rdabt_ftpsp->FTPSstatus = GOEP_Client_Idle;

    //OBEX_Init_Variable();

}
extern   int MGR_GetACLState(t_bdaddr addr) ;
uint8 OPPC_ConnectDelayTimer=0;
uint8 OPPS_Receive_Timer=0;
uint32 Current_Receivd_len=0;
uint8 Current_lowRate_num=0;
extern uint32 rdabt_obex_get_received_length(void);

extern void rdabt_obex_clean_received_length(void);

void rdabt_goep_connect_req_delay(void * temp)
{
    kal_prompt_trace(1, "rdabt_goep_connect_req_delay current aclStatue=0x%x,rdabt_oppcp->oppCstatus=%d",MGR_GetACLState(rdabt_oppcp->OPPCaddress),rdabt_oppcp->oppCstatus);
    /*if(OPPC_ConnectDelayTimer)
       {
        rdabt_cancel_timer(OPPC_ConnectDelayTimer);
        OPPC_ConnectDelayTimer=0;

    }
       if(!MGR_GetACLState(rdabt_oppcp->OPPCaddress))
                OBEX_ConnectRequest(&(rdabt_oppcp->OPPCaddress),SCLASS_UUID_OBEX_PUSH,NULL);
                 else
                    OPPC_ConnectDelayTimer=rdabt_timer_create(10,  rdabt_goep_connect_req_delay,NULL,pTIMER_PERIODIC);*/
    OBEX_ConnectRequest(&(rdabt_oppcp->OPPCaddress),SCLASS_UUID_OBEX_PUSH,NULL);

    OPPC_ConnectDelayTimer=0;



}
void rdabt_goep_opps_recevePkt_timeout(void * temp)
{
    Current_Receivd_len=rdabt_obex_get_received_length()/2;

    kal_prompt_trace(1, "rdabt_goep_opps_recevePkt_timeout rate=%d Bps,Current_lowRate_num=%d",Current_Receivd_len,Current_lowRate_num);
    if(Current_Receivd_len<5000)
        Current_lowRate_num++;
    else if(Current_lowRate_num<5)
        Current_lowRate_num=0;
    if(Current_lowRate_num>=5)
    {
        kal_prompt_trace(1, "rdabt_goep_opps_recevePkt_timeout NOW DISCON LINK");
        OBEX_ServerTransportDisconnect(rdabt_oppsp->OPPStid);
        rdabt_goep_disconnect_rsp_opps(0, NULL, 0);
        Current_lowRate_num=0;
        if(OPPS_Receive_Timer)
        {
            rdabt_cancel_timer(OPPS_Receive_Timer);
            OPPS_Receive_Timer=0;
        }

    }
    rdabt_obex_clean_received_length();

    //OBEX_ServerTransportDisconnect(rdabt_oppsp->OPPStid);
    // rdabt_goep_disconnect_rsp_opps(0, NULL, 0);

}
/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_connect_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_connect_req(t_bdaddr address,goep_connect_req_struct *msg_p)
{
    bt_bm_link_state_ind_struct *param_ptr ;
    param_ptr = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);
    kal_prompt_trace(1, "rdabt_goep_connect_req...  20090220-Rdabt_opp!@!!!!!!");
    param_ptr->max_number = 3;
    param_ptr->current_number = 1 ;//0x2;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr, NULL);
    oppc_waite_abort=0;

    oppc_waite_discon_flag=0;
    rdabt_oppcp->current_FirstPkt=0;
    if(memcmp((char*)msg_p->uuid , (const char*)(OBEX_FTP_UUID) , 16)==0)
    {
        rdabt_ftpcp->ftpCstatus = GOEP_Client_Disconnect;
        rdabt_ftpcp->goep_tidFTPC = 0xff;
        memcpy(rdabt_ftpcp->FTPCaddress.bytes,address.bytes,6);

        //rdabt_ftpcp->FTPCaddress = address;
        rdabt_ftpcp->req_idFTPC = msg_p ->req_id;
        //OBEX_ConnectRequest(&address,SCLASS_UUID_OBEX_FTP);
        last_client_connect = SCLASS_UUID_OBEX_FTP;
    }
    else
    {
        rdabt_oppcp->oppCstatus = GOEP_Client_Connectting;//GOEP_Client_Disconnect;
        rdabt_oppcp->goep_tidOPPC=0xff;
        memcpy(rdabt_oppcp->OPPCaddress.bytes,address.bytes,6);
        //rdabt_oppcp->OPPCaddress = address;
        rdabt_oppcp->req_idOPPC = msg_p ->req_id;
        //rdabt_obexConnect_process(&address,SCLASS_UUID_OBEX_PUSH);
        last_client_connect = SCLASS_UUID_OBEX_PUSH;
        // kal_prompt_trace(1, "rdabt_goep_connect_req, rerturn directly");

        //  return;
        OPPC_ConnectDelayTimer=rdabt_timer_create(20,  rdabt_goep_connect_req_delay,NULL,pTIMER_ONESHOT);
        /*  if(0)//((rdabt_ftpsp->FTPSstatus==GOEP_Client_Disconnect)||(rdabt_ftpsp->FTPSstatus==GOEP_Client_Idle))
            {
                if(!MGR_GetACLState(rdabt_oppcp->OPPCaddress))
                 OBEX_ConnectRequest(&(rdabt_oppcp->OPPCaddress),SCLASS_UUID_OBEX_PUSH,NULL);
                  else
                     OPPC_ConnectDelayTimer=rdabt_timer_create(15,  rdabt_goep_connect_req_delay,NULL,pTIMER_PERIODIC);
            }
          else
                         OBEX_ConnectRequest(&(rdabt_oppcp->OPPCaddress),SCLASS_UUID_OBEX_PUSH,NULL);
        */
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  OBEX_Connect_callback
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
kal_uint32 goep_cidClient=0;
void rdabt_opp_discon_timeOut(void);

void OBEX_Connect_callback(kal_uint8 status,
                           kal_uint8* finger,
                           kal_uint16 length,
                           kal_uint32 oppc_tid)
{
    goep_connect_rsp_struct *param_ptr ;

    kal_prompt_trace(1, "OBEX_Connect_callback... ,status = %x,rdabt_oppcp->oppCstatus=%x",status,rdabt_oppcp->oppCstatus);

    kal_prompt_trace(1, "OBEX_Connect_callback...  last_client_connect=0x%x ftpc status=%x oppc status=%x tid=%x",last_client_connect,rdabt_ftpcp->ftpCstatus,rdabt_oppcp->oppCstatus,oppc_tid);

    if(last_client_connect == SCLASS_UUID_OBEX_FTP)
    {
        if( rdabt_ftpcp->ftpCstatus != GOEP_Client_Disconnect)
        {
            OBEX_TransportDisconnect(oppc_tid);
            return;
        }
        rdabt_ftpcp->goep_tidFTPC=oppc_tid;//obex connect process finish, here first get tid for opp_send_tid,rdabt_obex_transmit_transport' tid
    }
    else  if(last_client_connect == SCLASS_UUID_OBEX_PUSH)
    {
        rdabt_oppcp->goep_tidOPPC=oppc_tid;//obex connect process finish, here first get tid for opp_send_tid,rdabt_obex_transmit_transport' tid

        //  if(rdabt_discon_timer)
        //  rdabt_cancel_timer(rdabt_discon_timer);
        //rdabt_discon_timer=0;
        if(oppc_waite_discon_flag)
            sxr_StopFunctionTimer(rdabt_opp_discon_timeOut);

        oppc_waite_discon_flag=0;
        if( (rdabt_oppcp->oppCstatus != GOEP_Client_Connectting)&&(rdabt_oppcp->oppCstatus != GOEP_Client_Idle))
        {
            status=OBEX_TransportDisconnect(rdabt_oppcp->goep_tidOPPC);
            if(status==OBEX_OK)
                OBEX_Disconnect(oppc_tid);//, offset_len, txBuffer);//, rdabt_goep_disconnect_rsp_oppc);
            if(rdabt_oppcp->oppCstatus != GOEP_Client_Disconnect)
            {
                goep_disconnect_ind_struct *param_ptr ;
                param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);
                param_ptr->uuid[0] = 0;
                param_ptr->uuid_len = 0;
                param_ptr->req_id = rdabt_oppcp->req_idOPPC ;
                param_ptr->goep_conn_id =0xff;
                param_ptr->ref_count=0x11;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
                kal_prompt_trace(1,"MSG_ID_GOEP_DISCONNECT_IND SEND");
            }
            rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;
            return;
        }


    }
    else
        return;

    param_ptr = (goep_connect_rsp_struct*) construct_local_para(sizeof(goep_connect_rsp_struct), TD_CTRL);

    if(status==OBEX_OK)
    {
        switch(*finger)
        {
            case OBEX_OK:
            {

                t_obex_header_offsets headers;
                u_int16 offset_lenn = OBEX_CONNECT_HEADER;
                /*I need to read back the size field.*/
                param_ptr->peer_mru =(finger[5] << 8) + finger[6];
                if(param_ptr->peer_mru<GOEP_LOCAL_MTU)
                    param_ptr->peer_mru=param_ptr->peer_mru-200;
                else
                    param_ptr->peer_mru=GOEP_LOCAL_MTU-200;
                param_ptr->rsp_code = GOEP_STATUS_SUCCESS;//0;


                OBEX_ExtractHeaders(&headers, &offset_lenn,finger,length);
                if(headers.cid)
                {
                    goep_cidClient =
                        ((finger+headers.cid)[0] << 24) + ((finger+headers.cid)[1] << 16) + ((finger+headers.cid)[2] << 8) + ((finger+headers.cid)[3]);

                }
                kal_prompt_trace(1,"OBEX_Connect_callback goep_cidClient =0x%x,param_ptr->peer_mru=%d",goep_cidClient,param_ptr->peer_mru);

                break;
            }

            case 1:
            {
                param_ptr->peer_mru = 0;
                param_ptr->rsp_code = GOEP_STATUS_FAILED;//0;

                break;
            }

            default:
            {
                /*
                this was a failure I think that I will have to kill the thread.
                */
                param_ptr->rsp_code = GOEP_STATUS_BUSY;
                param_ptr->peer_mru = 0;
                kal_prompt_trace(1,"Got response of %x from a OBEX_Connect",*finger);

                break;
            }
        }
    }
    else
    {
        param_ptr->peer_mru = 0;
        param_ptr->rsp_code = GOEP_STATUS_FAILED;
    }

    param_ptr->ref_count = 1;
    param_ptr->msg_len = 0x20;
    if(last_client_connect == SCLASS_UUID_OBEX_FTP)
    {
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
        param_ptr->req_id = rdabt_ftpcp->req_idFTPC;
        param_ptr->goep_conn_id = rdabt_ftpcp->goep_tidFTPC ;//0x2;
        rdabt_ftpcp->ftpCstatus = GOEP_Client_Connect;
        rdabt_ftpcp->first = 1;
    }
    else
    {
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->req_id = rdabt_oppcp->req_idOPPC;
        param_ptr->goep_conn_id = rdabt_oppcp->goep_tidOPPC ;//0x2;
        rdabt_oppcp->oppCstatus = GOEP_Client_Connect;
    }
    param_ptr->cm_conn_id = 0;


    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_CONNECT_RSP, (void *)param_ptr, NULL);

}

/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_push_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/


void rdabt_goep_push_req(goep_push_req_struct *msg_p)
{
    u_int16 ii=0;

    obex_push_req_handle_struct   push_req_handle;
    push_req_handle.frag_len=msg_p ->frag_len;
    push_req_handle.frag_ptr=msg_p ->frag_ptr;
    push_req_handle.goep_conn_id=msg_p ->goep_conn_id;
    push_req_handle.pkt_type=msg_p ->pkt_type;
    kal_prompt_trace(1, "rdabt_goep_push_req,push_req_handle.frag_len=%d",push_req_handle.frag_len);
    rdabt_oppcp->current_FirstPkt=0;
    rdabt_oppcp->during_put=1;

    if(GOEP_FIRST_PKT==push_req_handle.pkt_type)
    {

        push_req_handle.TypeLength=(u_int32)strlen((char* )msg_p ->obj_mime_type)+1;
        push_req_handle.obj_mime_type=msg_p ->obj_mime_type;
        push_req_handle.total_obj_len=msg_p ->total_obj_len;
        push_req_handle.NameLength= (u_int16) mmi_ucs2strlen(msg_p ->obj_name);
        push_req_handle.obj_name=(u_int16*) rdabt_malloc(sizeof(u_int16)*512);
        if(!push_req_handle.obj_name)
        {
            kal_prompt_trace(1, "rdabt_goep_push_req,malloc name fail!!!!!!!!!!!!!!!!");

            rdabt_free(push_req_handle.obj_name);
            return;
        }

        if(push_req_handle.NameLength)
        {
            for(ii=0; ii<(push_req_handle.NameLength+1)*2; ii+=2)
                push_req_handle.obj_name[ii>>1]=((msg_p->obj_name[ii])<<8)+msg_p->obj_name[ii+1];
        }

        rdabt_oppcp->current_FirstPkt=1;
    }
    oppc_packetType=msg_p ->pkt_type;
    kal_prompt_trace(1, "rdabt_goep_push_req,push_req_handle.frag_len=%d,push_req_handle.obj_mime_type=%d,push_req_handle.NameLength=%d",
                     push_req_handle.frag_len,push_req_handle.obj_mime_type,push_req_handle.NameLength);
    RDABT_OPPC_PutHandle(&push_req_handle);

    rdabt_free(push_req_handle.obj_name);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_abort_req_reply
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_abort_req_reply(u_int8 status, u_int8* data, u_int16 length)
{
    goep_abort_rsp_struct *param_ptr ;
    kal_prompt_trace(1, "rdabt_goep_abort_req_reply. status = %d,last_client_connect=%x,rdabt_ftpcp->ftpCstatus=%d ",status,last_client_connect,rdabt_ftpcp->ftpCstatus );
    kal_prompt_trace(1, "!@!!!!!!,length = %d",length);
    if(last_client_connect == SCLASS_UUID_OBEX_FTP)
    {
        if(rdabt_ftpcp->ftpCstatus != GOEP_Client_Connect)
            return;
        param_ptr = (goep_abort_rsp_struct*) construct_local_para(sizeof(goep_abort_rsp_struct), TD_CTRL);

        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 16;
        param_ptr ->goep_conn_id = rdabt_ftpcp->goep_tidFTPC;
        //  rdabt_ftpcp->ftpCstatus = GOEP_Client_Disconnect;
    }
    else
    {
        if(rdabt_oppcp->oppCstatus != GOEP_Client_Connect)
            return;
        OBEX_Disconnect(rdabt_oppcp->goep_tidOPPC);
        //OBEX_TransportDisconnect(rdabt_oppcp->goep_tidOPPC)
        return;
        param_ptr = (goep_abort_rsp_struct*) construct_local_para(sizeof(goep_abort_rsp_struct), TD_CTRL);

        param_ptr ->uuid[0] = 0;
        param_ptr ->uuid_len = 0;
        param_ptr ->goep_conn_id = rdabt_oppcp->goep_tidOPPC;
        //  rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;
    }
    //status=OBEX_TransportDisconnect(rdabt_oppcp->goep_tidOPPC);
    //param_ptr ->ref_count = 1;
    //param_ptr ->msg_len = 0x18;
    param_ptr ->rsp_code = 0 ;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_ABORT_RSP, (void *)param_ptr, NULL);
}
u_int8 rdabt_abort_timer=0;
void rdabt_opp_abort_timeOut(void * temp)//for send to android phone, after peer device pop receive file msg, local presss cancel,nee wait lang time issue.
{
    kal_prompt_trace(1, "rdabt_opp_abort_timeOut");
    rdabt_abort_timer=0;
    oppc_waite_abort=0;

    //OBEX_Abort(rdabt_oppcp->goep_tidOPPC);
    rdabt_goep_disconnect_req_reply(rdabt_oppcp->req_idOPPC, rdabt_oppcp->goep_tidOPPC);
}
void rdabt_opp_discon_timeOut(void)
{
    goep_disconnect_ind_struct *param_ptr ;
    kal_prompt_trace(1, "rdabt_opp_discon_timeOut");
    rdabt_discon_timer=0;
    oppc_waite_discon_flag=0;
    param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);

    param_ptr->uuid[0] = 0;
    param_ptr->uuid_len = 0;
    param_ptr->req_id = rdabt_oppcp->req_idOPPC ;
    param_ptr->goep_conn_id =0xff;
    param_ptr->ref_count=0x22;


    rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
    kal_prompt_trace(1,"MSG_ID_GOEP_DISCONNECT_IND SEND");

}
/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_abort_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_abort_req(goep_abort_req_struct *msg_p)
{
    int status;
    kal_prompt_trace(1, " ***** rdabt_goep_abort_req con id=%d,rdabt_oppcp->current_FirstPkt=%d,rdabt_oppcp->during_put=%d ",msg_p ->goep_conn_id,rdabt_oppcp->current_FirstPkt,rdabt_oppcp->during_put);
    if(rdabt_oppcp->current_FirstPkt)
        rdabt_goep_disconnect_req_reply(rdabt_oppcp->req_idOPPC, rdabt_oppcp->goep_tidOPPC);
    else
    {
        if(rdabt_oppcp->during_put)
        {
            oppc_waite_abort=1;
            rdabt_abort_timer=rdabt_timer_create(20,  rdabt_opp_abort_timeOut,NULL,pTIMER_ONESHOT);

        }
        else
            OBEX_Abort(rdabt_oppcp->goep_tidOPPC);
    }
    return;
}



/*****************************************************************************
 * FUNCTION
 *  goep_push_callback
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void goep_push_callback(u_int8 messageType, u_int8 *data, u_int16 len)
{
    goep_push_rsp_struct *param_ptr ;
    kal_prompt_trace(1, "goep_push_callback...  20090220-Rdabt_opp!@!!!!!! messageType = %X,rdabt_abort_timer=%x,oppc_waite_abort=%x",messageType,rdabt_abort_timer,oppc_waite_abort);
    kal_prompt_trace(1, "!@!!!!!! len = %x,rdabt_oppcp->oppCstatus=%x",len,rdabt_oppcp->oppCstatus);
    rdabt_oppcp->during_put=0;
    if(oppc_waite_abort|| (rdabt_oppcp->oppCstatus == GOEP_Client_Disconnect))
    {
        oppc_waite_abort=0;
        if(rdabt_abort_timer)
            rdabt_cancel_timer(rdabt_abort_timer);
        rdabt_abort_timer=0;
        OBEX_Abort(rdabt_oppcp->goep_tidOPPC);

        return;
    }
    param_ptr = (goep_push_rsp_struct*) construct_local_para(sizeof(goep_push_rsp_struct), TD_CTRL);

    if(last_client_connect == SCLASS_UUID_OBEX_FTP)
    {
        param_ptr->goep_conn_id = rdabt_ftpcp->goep_tidFTPC;
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
    }
    else
    {
        param_ptr->goep_conn_id = rdabt_oppcp->goep_tidOPPC;
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
    }
    if((OBEX_OK == messageType)||(OBEX_CONTINUE == messageType))
    {
        param_ptr->rsp_code =0;
        param_ptr->reason =0;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_RSP, (void *)param_ptr, NULL);
    }
    else if((OBEX_DISCONNECT == messageType))
    {

        kal_prompt_trace(1, "goep_push_callback...  !@!!!!!! messageType = %d",messageType);
        param_ptr->rsp_code =1;
        param_ptr->reason =messageType;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_RSP, (void *)param_ptr, NULL);
        if(last_client_connect == SCLASS_UUID_OBEX_FTP)
            rdabt_ftpcp->ftpCstatus = GOEP_Client_Idle;
        else
            rdabt_oppcp->oppCstatus = GOEP_Client_Idle;

    }
    else //OBEX_FORBIDDEN
    {

        kal_prompt_trace(1, "goep_push_callback...  !@!!!!!! messageType = %d",messageType);
        param_ptr->rsp_code =1;
        param_ptr->reason =messageType;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_RSP, (void *)param_ptr, NULL);
    }

}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_req_reply
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_req_reply(kal_uint32 req_id,  kal_uint8 goep_conn_id)
{

    goep_disconnect_ind_struct *param_ptr ;
    bt_bm_link_state_ind_struct *param_ptr_link_state ;
    kal_prompt_trace(1,"rdabt_goep_disconnect_req_reply is called OBEX_TransportDisconnect will calleds");
    param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);
    param_ptr_link_state = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);

    OBEX_TransportDisconnect(goep_conn_id);

    param_ptr->ref_count = 0x33;
    param_ptr->msg_len = 0x18;
    param_ptr->uuid[0] = 0;
    param_ptr->uuid_len = 0;
    param_ptr->req_id = req_id ;
    param_ptr->goep_conn_id = goep_conn_id;
    /*
    I might have to keep track of lenghts here.
    */
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
    param_ptr_link_state->ref_count = 1;
    param_ptr_link_state->msg_len = 6 ;//0x2;
    param_ptr_link_state->max_number = 3;
    param_ptr_link_state->current_number = 0 ;//0x2;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr_link_state, NULL);
    rdabt_oppcp->goep_tidOPPC = 0xff;
    rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;//GOEP_Client_Idle;

}



/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_rsp_oppc
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_rsp(u_int8 status, u_int8* data, u_int16 length)
{

    goep_disconnect_ind_struct *param_ptr ;
    //bt_bm_link_state_ind_struct *param_ptr_link_state ;
    kal_prompt_trace(1,"rdabt_goep_disconnect_rsp last_client_connect=0x%x",last_client_connect);
    //param_ptr_link_state = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);
    if(last_client_connect == SCLASS_UUID_OBEX_FTP)
    {
        param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);

        status=OBEX_TransportDisconnect(rdabt_ftpcp->goep_tidFTPC);
        rdabt_ftpcp->ftpCstatus = GOEP_Client_Idle;
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 16;
        param_ptr->req_id = rdabt_ftpcp->req_idFTPC ;
        param_ptr->goep_conn_id =rdabt_ftpcp->goep_tidFTPC;// rdabt_ftpcp->goep_tidFTPC_mmi;
        rdabt_ftpcp->goep_tidFTPC = 0xff;
        last_client_connect=0;
    }
    else if(last_client_connect ==SCLASS_UUID_OBEX_PUSH)
    {
        param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);

        status=OBEX_TransportDisconnect(rdabt_oppcp->goep_tidOPPC);
        rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->req_id = rdabt_oppcp->req_idOPPC ;
        param_ptr->goep_conn_id = rdabt_oppcp->goep_tidOPPC ;//rdabt_oppcp->goep_tidOPPC_mmi;
        rdabt_oppcp->goep_tidOPPC = 0xff;
        last_client_connect=0;

    }
    else
        return;
    param_ptr->ref_count = 0x44;
    //param_ptr->msg_len = 0x18;
    /*
    I might have to keep track of lenghts here.
    */
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
    //param_ptr_link_state->ref_count = 1;
    //param_ptr_link_state->msg_len = 6 ;//0x2;
    //param_ptr_link_state->max_number = 3;
    // param_ptr_link_state->current_number = 0 ;//0x2;
    // rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr_link_state, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_req(goep_disconnect_req_struct *msg_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int status;
    u_int16 offset_len;
    t_DataBuf *txBuffer;

    kal_prompt_trace(1, "rdabt_goep_disconnect_req. 0x%x, 0x%x, 0x%x, 0x%x, 0x%x,rdabt_oppcp->oppCstatus =%d ",
                     msg_p ->goep_conn_id,rdabt_ftpcp->goep_tidFTPC, rdabt_oppcp->goep_tidOPPC,rdabt_ftpsp->goep_tid_FTPS, rdabt_oppsp->goep_tid_OPPS,rdabt_oppcp->oppCstatus  );
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((msg_p ->goep_conn_id == rdabt_ftpcp->goep_tidFTPC))
    {
        //offset_len = OBEX_HEADER;
        rdabt_ftpcp->req_idFTPC = msg_p ->req_id ;
        if(rdabt_ftpcp->ftpCstatus == GOEP_Client_Disconnect)
        {
            rdabt_ftpcp->goep_tidFTPC_mmi = msg_p ->goep_conn_id;
        }
        else
            rdabt_ftpcp->goep_tidFTPC_mmi = rdabt_ftpcp->goep_tidFTPC;

        if(msg_p ->goep_conn_id == 0xff)
        {
            rdabt_ftpcp->ftpCstatus = GOEP_Client_Disconnect;
        }

        kal_prompt_trace(1, "rdabt_goep_disconnect_req OPPC...,ftpCstatus ==%x",rdabt_ftpcp->ftpCstatus);

        if(rdabt_ftpcp->ftpCstatus == GOEP_Client_Connect)
        {
            status=OBEX_Disconnect(msg_p ->goep_conn_id);//, offset_len, txBuffer);//, rdabt_goep_disconnect_rsp_oppc);
            rdabt_ftpcp->ftpCstatus = GOEP_Client_Disconnect;
        }
        else if( rdabt_ftpcp->ftpCstatus == GOEP_Client_Disconnect)
        {
            rdabt_goep_disconnect_rsp(0, NULL, 0);
        }
        else
            rdabt_goep_disconnect_req_reply(rdabt_ftpcp->req_idFTPC, rdabt_ftpcp->goep_tidFTPC);
    }
    else if((msg_p ->goep_conn_id == rdabt_oppcp->goep_tidOPPC)||(msg_p ->goep_conn_id == 0xff))
    {
        //offset_len = OBEX_HEADER;
        rdabt_oppcp->req_idOPPC = msg_p ->req_id ;
        if(rdabt_oppcp->oppCstatus == GOEP_Client_Disconnect)
        {
            rdabt_oppcp->goep_tidOPPC_mmi = msg_p ->goep_conn_id;
        }
        else
            rdabt_oppcp->goep_tidOPPC_mmi = rdabt_oppcp->goep_tidOPPC;

        if(msg_p ->goep_conn_id == 0xff)
        {


            rdabt_oppcp->oppCstatus = GOEP_Client_pending_Disconnect;
            /// rdabt_oppcp->goep_tidOPPC=get_transport_tid(rdabt_oppcp->OPPCaddress);
            //   OBEX_TransportDisconnect(get_transport_tid(rdabt_oppcp->OPPCaddress));


            oppc_waite_discon_flag=1;
            sxr_StartFunctionTimer(11 *16384,  rdabt_opp_discon_timeOut,NULL,0);
            return;

        }

        kal_prompt_trace(1, "rdabt_goep_disconnect_req OPPC,oppCstatus==%x",rdabt_oppcp->oppCstatus);

        if(rdabt_oppcp->oppCstatus == GOEP_Client_Connect)
        {
            status=OBEX_Disconnect(msg_p ->goep_conn_id);//, offset_len, txBuffer);//, rdabt_goep_disconnect_rsp_oppc);
            rdabt_oppcp->oppCstatus = GOEP_Client_Disconnect;
        }
        else if( rdabt_oppcp->oppCstatus == GOEP_Client_Disconnect)
        {
            rdabt_goep_disconnect_rsp(0, NULL, 0);
        }
        else
            rdabt_goep_disconnect_req_reply(rdabt_oppcp->req_idOPPC, rdabt_oppcp->goep_tidOPPC);
    }
    else if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS)
    {
        offset_len = OBEX_HEADER;
        rdabt_ftpsp->req_idFTPS = msg_p ->req_id;
        kal_prompt_trace(1, "rdabt_goep_disconnect_req FTPS");

        OBEX_ServerTransportDisconnect(rdabt_ftpsp->FTPStid);
        rdabt_goep_disconnect_rsp_ftps(0, NULL, 0);
    }
    else if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS)
    {
        offset_len = OBEX_HEADER;
        rdabt_oppsp->req_idOPPS = msg_p ->req_id ;
        kal_prompt_trace(1, "rdabt_goep_disconnect_req OPPS ");

        OBEX_ServerTransportDisconnect(rdabt_oppsp->OPPStid);
        rdabt_goep_disconnect_rsp_opps(0, NULL, 0);
        if(OPPS_Receive_Timer)
        {
            rdabt_cancel_timer(OPPS_Receive_Timer);
            OPPS_Receive_Timer=0;
        }
    }
    else
        rdabt_goep_disconnect_req_reply(msg_p ->req_id,msg_p ->goep_conn_id);
    return;
}





/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_rsp_ftps
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_rsp_ftps(u_int8 status, u_int8* data, u_int16 length)
{

    goep_disconnect_ind_struct *param_ptr ;
    bt_bm_link_state_ind_struct *param_ptr_link_state ;

    param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);
    param_ptr_link_state = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);

    param_ptr->ref_count = 0x55;
//    param_ptr->msg_len = 0x18;
    memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
    param_ptr->uuid_len = 16;
    param_ptr->req_id = rdabt_ftpsp->req_idFTPS ;
    param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS;

    rdabt_ftpsp->FTPSfirst = 1;
    rdabt_ftpsp->FTPSstatus = GOEP_Client_Disconnect;

    /*
    I might have to keep track of lenghts here.
    */
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
    param_ptr_link_state->ref_count = 1;
    param_ptr_link_state->msg_len = 6 ;//0x2;
    param_ptr_link_state->max_number = 3;
    param_ptr_link_state->current_number = 0 ;//0x2;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr_link_state, NULL);

}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_rsp_opps
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_rsp_opps(u_int8 status, u_int8* data, u_int16 length)
{

    goep_disconnect_ind_struct *param_ptr ;
    bt_bm_link_state_ind_struct *param_ptr_link_state ;

    param_ptr = (goep_disconnect_ind_struct*) construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);
    param_ptr_link_state = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);

    param_ptr->ref_count = 0x66;
    param_ptr->msg_len = 0x18;
    param_ptr->uuid[0] = 0;
    param_ptr->uuid_len = 0;
    param_ptr->req_id = rdabt_oppsp->req_idOPPS ;
    param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS;

    rdabt_oppsp->OPPSfirst = 1;
    rdabt_oppsp->OPPSstatus = GOEP_Client_Disconnect;

    /*
    I might have to keep track of lenghts here.
    */
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);
    param_ptr_link_state->ref_count = 1;
    param_ptr_link_state->msg_len = 6 ;//0x2;
    param_ptr_link_state->max_number = 3;
    param_ptr_link_state->current_number = 0 ;//0x2;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr_link_state, NULL);
    if(OPPS_Receive_Timer)
    {
        rdabt_cancel_timer(OPPS_Receive_Timer);
        OPPS_Receive_Timer=0;
    }

}


//below is opp servers code


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_connect_ind
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_connect_ind(t_bdaddr address,u_int16 ftps)
{
    int status = 0;
    kal_prompt_trace(1,"rdabt_goep_connect_ind");

    if(ftps == 1)
    {
        goep_connect_ind_struct *param_ptr;
        t_MGR_DeviceEntry *device;
        kal_prompt_trace(1,"rdabt_goep_connect_ind");
        param_ptr = (goep_connect_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_connect_ind_struct), TD_CTRL);
        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0x78;
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
        param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
        param_ptr->peer_mru = rdabt_ftpsp->FTPSmax_len - 20;
        param_ptr->bd_addr.lap= BDADDR_Get_LAP(&address);
        param_ptr->bd_addr.uap = BDADDR_Get_UAP(&address);
        param_ptr->bd_addr.nap = BDADDR_Get_NAP(&address);
        memset((char*)param_ptr->dev_name , 0 , 80);
        MGR_FindDeviceRecord(address, &device);
        if( 0 != device )
            memcpy((char*)param_ptr->dev_name , (const char*)(device->name) , 80);

        param_ptr->cm_conn_id = 0;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_CONNECT_IND, (void *)param_ptr, NULL);
        memset(rdabt_ftpsp->ftps_obj_name, 0, GOEP_MAX_OBJ_NAME);
    }
    else
    {
        goep_connect_ind_struct *param_ptr;
        t_MGR_DeviceEntry *device;
        kal_prompt_trace(1,"rdabt_goep_connect_ind");
        param_ptr = (goep_connect_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_connect_ind_struct), TD_CTRL);
        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0x78;
        {
            param_ptr->uuid[0] = 0;
            param_ptr->uuid_len = 0;
            param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS ;
            param_ptr->peer_mru = rdabt_oppsp->OPPSmax_len;
        }
        param_ptr->bd_addr.lap= BDADDR_Get_LAP(&address);
        param_ptr->bd_addr.uap = BDADDR_Get_UAP(&address);
        param_ptr->bd_addr.nap = BDADDR_Get_NAP(&address);
        MGR_FindDeviceRecord(address, &device);
        memset((char*)param_ptr->dev_name , 0 , 80);
        if( 0 != device )
            memcpy((char*)param_ptr->dev_name , (const char*)(device->name) , 80);
        memset(rdabt_oppsp->opps_obj_name, 0, 80);
        param_ptr->cm_conn_id = 0;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_CONNECT_IND, (void *)param_ptr, NULL);

    }

}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_connect_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_connect_res(goep_connect_res_struct *msg_p)//MSG_ID_GOEP_CONNECT_IND
{
    t_DataBuf* buf;

    kal_prompt_trace(1,"rdabt_goep_connect_res msg_p ->rsp_code =0x%x",msg_p ->rsp_code );

    OBEX_GetWriteBuffer(100,&buf);

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS )
    {
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            rdabt_oppsp->OPPSstatus = GOEP_Client_Connect;
            OBEX_ConnectResponse(rdabt_oppsp->OPPStid,OBEX_OK,0x10,0x00,rdabt_oppsp->OPPSmax_len,rdabt_oppsp->OPPSoffsetVar,(t_DataBuf*)buf);
            if(OPPS_Receive_Timer)
            {
                rdabt_cancel_timer(OPPS_Receive_Timer);
                OPPS_Receive_Timer=0;
            }
            OPPS_Receive_Timer=rdabt_timer_create(20,  rdabt_goep_opps_recevePkt_timeout,NULL,pTIMER_PERIODIC);
            rdabt_obex_clean_received_length();

            Current_lowRate_num=0;
        }
        else
            OBEX_ConnectResponse(rdabt_oppsp->OPPStid,(msg_p ->rsp_code + 0x80),0x10,0x00,rdabt_oppsp->OPPSmax_len,rdabt_oppsp->OPPSoffsetVar,(t_DataBuf*)buf);
    }
    else if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS )
    {
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            rdabt_ftpsp->FTPSstatus = GOEP_Client_Connect;
            OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_OK,(rdabt_ftpsp->FTPS_offset_len),FTPSbuffer);
        }
        else
            OBEX_ConnectResponse(rdabt_ftpsp->FTPStid, (msg_p ->rsp_code + 0x80) ,0x10,0x00,rdabt_ftpsp->FTPSmax_len,rdabt_ftpsp->FTPSoffsetVar,(t_DataBuf*)buf);
    }
}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_connect_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_push_res(goep_push_res_struct *msg_p)//MSG_ID_GOEP_PUSH_IND
{
    t_DataBuf* buffer;

    kal_prompt_trace(1,"rdabt_goep_push_resd msg_p ->rsp_code =0x%x,msg_p ->goep_conn_id=0x%x,rdabt_oppsp->goep_tid_OPPS=0x%x",
                     msg_p ->rsp_code ,msg_p ->goep_conn_id,rdabt_oppsp->goep_tid_OPPS);

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS || msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS)
    {
        OBEX_GetWriteBuffer(0,&buffer);
    }

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS )
    {
        if(rdabt_oppsp->OPPSstatus == GOEP_Client_Disconnect)
        {
            return;
        }
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            if(rdabt_oppsp->OPPSfirst==0)
                OBEX_Response(rdabt_oppsp->OPPStid,OBEX_CONTINUE,0,buffer);
            else
                OBEX_Response(rdabt_oppsp->OPPStid,OBEX_OK,0,buffer);

        }
        else
        {
            OBEX_Response(rdabt_oppsp->OPPStid,(msg_p ->rsp_code+0x80),0,buffer);
            rdabt_oppsp->OPPSfirst = 1;
            rdabt_goep_disconnect_ind(0);
        }
    }
    else if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS )
    {
        if(rdabt_ftpsp->FTPSstatus == GOEP_Client_Disconnect)
        {
            return;
        }
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            if(rdabt_ftpsp->FTPSfirst==0)
                OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_CONTINUE,0,buffer);
            else
                OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_OK,0,buffer);
        }
        else
        {
            OBEX_Response(rdabt_ftpsp->FTPStid,(msg_p ->rsp_code+0x80),0,buffer);
            rdabt_ftpsp->FTPSfirst = 1;
        }
    }
    else
        kal_prompt_trace(1,"rdabt_goep_connect_res may be error!");


}

/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_disconnect_ind
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_disconnect_ind(u_int16 ftps)
{
    goep_disconnect_ind_struct *param_ptr;
    bt_bm_link_state_ind_struct *param_ptr2 ;

    kal_prompt_trace(1,"rdabt_goep_disconnect_ind");

    param_ptr = (goep_disconnect_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_disconnect_ind_struct), TD_CTRL);

    param_ptr->ref_count = 0x77;
    param_ptr->msg_len = 0x18;
    param_ptr->req_id = 0;
    if(ftps == 1)
    {
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
        param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
        rdabt_ftpsp->FTPSstatus = GOEP_Client_Disconnect;
    }
    else
    {
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS ;
        rdabt_oppsp->OPPSstatus = GOEP_Client_Disconnect;
    }

    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DISCONNECT_IND, (void *)param_ptr, NULL);

    param_ptr2= (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_CTRL);
    param_ptr2->ref_count  = 1;
    param_ptr2->msg_len    = 6;
    param_ptr2->max_number = 3;
    param_ptr2->current_number = 0 ;//0x2;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_STATE_IND, (void *)param_ptr2, NULL);
    if(OPPS_Receive_Timer)
    {
        rdabt_cancel_timer(OPPS_Receive_Timer);
        OPPS_Receive_Timer=0;
    }
    rdabt_obex_clean_received_length();
    Current_lowRate_num=0;
}

/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_abort_ind
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_abort_ind(u_int16 ftps)
{
    goep_abort_ind_struct *param_ptr;

    kal_prompt_trace(1,"rdabt_goep_abort_ind");

    param_ptr = (goep_abort_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_abort_ind_struct), TD_CTRL);

    param_ptr->ref_count = 1;
    param_ptr->msg_len = 0x16;
    if(ftps)
    {
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
        param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
    }
    else
    {
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS ;
    }

    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_ABORT_IND, (void *)param_ptr, NULL);

}
/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_abort_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_abort_res(goep_abort_res_struct *msg_p)
{
    t_DataBuf* buffer;

    kal_prompt_trace(1,"rdabt_goep_abort_res");

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS || msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS)
    {
        OBEX_GetWriteBuffer(0,&buffer);
    }

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS )
    {
        rdabt_oppsp->OPPSfirst = 1;
        if(rdabt_oppsp->OPPSstatus != GOEP_Client_Connect)
            return;

        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            OBEX_Response(rdabt_oppsp->OPPStid,OBEX_OK,0,buffer);
        }
        else
        {
            OBEX_Response(rdabt_oppsp->OPPStid,(msg_p ->rsp_code+0x80),0,buffer);
        }
    }
    else if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS )
    {
        rdabt_ftpsp->FTPSfirst = 1;
        if(rdabt_ftpsp->FTPSstatus != GOEP_Client_Connect)
            return;

        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_OK,0,buffer);
        }
        else
        {
            OBEX_Response(rdabt_ftpsp->FTPStid,(msg_p ->rsp_code+0x80),0,buffer);
        }
    }
}




/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_authorize_ind_ftps
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_authorize_ind(t_bdaddr address, u_int8 dlci, u_int16 uuid)
{
    int status = 0;
    kal_prompt_trace(1, "rdabt_goep_authorize_ind,schan=0x%x,dlci=0x%x,rdabt_ftpsp->goep_tid_FTPS=%d",uuid,dlci,rdabt_ftpsp->goep_tid_FTPS);
    if(uuid == SDP_SCLASS_UUID_OBEX_FTP)
    {

        goep_authorize_ind_struct *param_ptr ;
        t_MGR_DeviceEntry *device;
        memset(rdabt_ftpsp->ftps_obj_name, 0, GOEP_MAX_OBJ_NAME);
        param_ptr = (goep_authorize_ind_struct*) construct_local_para(sizeof(goep_authorize_ind_struct), TD_CTRL);
        memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
        param_ptr->uuid_len = 0x10;
        param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS;

        memcpy(rdabt_ftpsp->FTPSaddress.bytes,address.bytes,6);


        //rdabt_ftpsp->FTPSaddress = address;
        param_ptr->bd_addr.lap= BDADDR_Get_LAP(&address);
        param_ptr->bd_addr.uap = BDADDR_Get_UAP(&address);
        param_ptr->bd_addr.nap = BDADDR_Get_NAP(&address);
        memset((char*)param_ptr->dev_name , 0 , 80);
        MGR_FindDeviceRecord(address, &device);
        if( 0 != device)
            memcpy((char*)param_ptr->dev_name , (const char*)(device->name) , 80);//???? 80 512??
        memset(rdabt_ftpsp->ftps_obj_name, 0, GOEP_MAX_OBJ_NAME);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_AUTHORIZE_IND, (void *)param_ptr, NULL);
        rdabt_ftpsp->FTPSdlci = dlci;

    }
    else  if(uuid == SDP_SCLASS_UUID_OBEX_PUSH)
    {

        goep_authorize_ind_struct *param_ptr ;
        t_MGR_DeviceEntry *device;
        memset(rdabt_oppsp->opps_obj_name, 0, 80);
        param_ptr = (goep_authorize_ind_struct*) construct_local_para(sizeof(goep_authorize_ind_struct), TD_CTRL);
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
        param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS;
        memcpy(rdabt_oppsp->OPPSaddress.bytes,address.bytes,6);

        //rdabt_oppsp->OPPSaddress = address;
        param_ptr->bd_addr.lap= BDADDR_Get_LAP(&address);
        param_ptr->bd_addr.uap = BDADDR_Get_UAP(&address);
        param_ptr->bd_addr.nap = BDADDR_Get_NAP(&address);
        memset((char*)param_ptr->dev_name , 0 , 80);
        MGR_FindDeviceRecord(address, &device);
        if( 0 != device )
            memcpy((char*)param_ptr->dev_name , (const char*)(device->name) , 80);
        memset(rdabt_oppsp->opps_obj_name, 0, 80);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_AUTHORIZE_IND, (void *)param_ptr, NULL);
        rdabt_oppsp->OPPSdlci = dlci;

    }
}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_authorize_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_authorize_res(goep_authorize_res_struct *msg_p)
{
    kal_prompt_trace(1, "rdabt_goep_authorize_res,msg_p ->rsp_code =0x%x,rdabt_ftpsp->goep_tid_FTPS=%d",msg_p ->rsp_code,rdabt_ftpsp->goep_tid_FTPS );

    if(msg_p ->goep_conn_id == rdabt_oppsp->goep_tid_OPPS )
    {
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            RSE_SrvConnectResponse(rdabt_oppsp->OPPSaddress, rdabt_oppsp->OPPSdlci, 1);

        }
        else
            RSE_SrvConnectResponse(rdabt_oppsp->OPPSaddress, rdabt_oppsp->OPPSdlci, 0);
    }
    else if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS )
    {
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            RSE_SrvConnectResponse(rdabt_ftpsp->FTPSaddress, rdabt_ftpsp->FTPSdlci, 1);

        }
        else
            RSE_SrvConnectResponse(rdabt_ftpsp->FTPSaddress, rdabt_ftpsp->FTPSdlci, 0);
    }
}

/*****************************************************************************
 * FUNCTION
 *  ServiceHandler
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void OPPServiceHandler(u_int32 tid, u_int8 messageType, t_obex_headers headers, u_int8 *data,u_int16 len,void* cbref)
{
    t_DataBuf* buf;
    t_DataBuf *buffer;
    u_int16 offset_len;
    t_obex_header_offsets internal;

    // rdabt_oppsp->OPPStid = tid;// tid is rdabt_obex_transmit_transport 's tid

    kal_prompt_trace(1, "R OPP tid %x,rdabt_oppsp->OPPStid=0x%x,OPPS_Receive_Timer=%x\n",tid,rdabt_oppsp->OPPStid,OPPS_Receive_Timer);

    /*if(OPPS_Receive_Timer)
        {
        rdabt_cancel_timer(OPPS_Receive_Timer);
        OPPS_Receive_Timer=0;
        }*/


    switch(messageType)
    {
        case OBEX_CONNECT:
            kal_prompt_trace(1, "Received a Connect message\n");
            rdabt_oppsp->OPPSfirst = 1;

            rdabt_oppsp->OPPSmax_len = (data[5] << 8) + data[6];
            if(rdabt_oppsp->buf_sizeOPPS < rdabt_oppsp->OPPSmax_len)
                rdabt_oppsp->OPPSmax_len = rdabt_oppsp->buf_sizeOPPS;
            rdabt_oppsp->OPPSmax_len -=20;
            need_user_confirm = 0;
            rdabt_oppsp->OPPSoffsetVar=OBEX_CONNECT_HEADER;
            OBEX_ExtractHeaders(&internal,&(rdabt_oppsp->OPPSoffsetVar),data,len);
            rdabt_goep_connect_ind(rdabt_oppsp->OPPSaddress,0);
            rdabt_obex_clean_received_length();
            break;

        case OBEX_DISCONNECT:
            kal_prompt_trace(1, "Received a Disconnect message\n");
            OBEX_GetWriteBuffer(0,&buffer);
            OBEX_Response(tid,OBEX_OK,0,buffer);
            rdabt_goep_disconnect_ind(0);
            need_user_confirm = 0;
            break;

        case OBEX_PUT:
        {
            goep_push_ind_struct *param_ptr;
            if(data != rdabt_oppsp->buf_ptrOPPS)
            {
                kal_prompt_trace(1, "May a error!!!!!!! 0 \n");
                kal_prompt_trace(1, "data = 0x %x 0 \n",data);
                kal_prompt_trace(1, "buf_ptrOPPS = 0x %x 0 \n",rdabt_oppsp->buf_ptrOPPS);
                asm("break 1");
            }

            param_ptr = (goep_push_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_push_ind_struct), TD_CTRL);
            param_ptr->ref_count = 1;
            param_ptr->msg_len = 0x174;
            param_ptr->uuid[0] = 0;
            param_ptr->uuid_len = 0;
            param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS ;
            param_ptr->frag_ptr = rdabt_oppsp->buf_ptrOPPS;
            param_ptr->total_obj_len = 0;
            param_ptr->obj_name[0]=0;
            param_ptr->obj_mime_type[0]=0;
            param_ptr->frag_len= 0;
            param_ptr->put_type = GOEP_PUT_NORMAL ;
            //param_ptr->need_user_confirm = need_user_confirm;
            /* I will need to store data here */
            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);
            kal_prompt_trace(1, "R Put msg,rdabt_oppsp->OPPSfirst=%d,internal.name=%d,internal.type=%d,internal.len=%d,internal.body=%d,internal.eob=%d\n"
                             ,rdabt_oppsp->OPPSfirst,internal.name,internal.type,internal.len,internal.body,internal.eob);
            need_user_confirm++;
            if(rdabt_oppsp->OPPSfirst)
            {
                if(internal.name)
                {
                    memcpy((char*)param_ptr->obj_name , (const char*)((data + internal.name+2)) , (((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                    if(param_ptr->obj_name)
                        rdabt_oppsp->OPPSfirst = 0;
                    else
                    {
                        OBEX_GetWriteBuffer(0,&buffer);
                        OBEX_Response(tid,OBEX_BAD_REQUEST,0,buffer);
                        break;
                    }
                }

                if(internal.type)
                {
                    strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                    param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                }

                if(internal.len)
                    param_ptr->total_obj_len = (((data + internal.len)[0] << 24 ) + ((data + internal.len)[1] << 16 ) + ((data + internal.len)[2] << 8 ) + (data+internal.len)[3] );

                param_ptr->pkt_type = GOEP_FIRST_PKT;
                param_ptr->put_type = GOEP_PUT_NORMAL ;
            }
            else
                param_ptr->pkt_type = GOEP_NORMAL_PKT;


            if(internal.body)
            {
                param_ptr->put_type = GOEP_PUT_NORMAL ;
                param_ptr->frag_ptr= (data + internal.body + 2);
                param_ptr->frag_len= (((data + internal.body)[0] << 8) + (data + internal.body)[1] -3);
                if(param_ptr->frag_ptr != rdabt_oppsp->buf_ptrOPPS)
                    kal_prompt_trace(1, "May a error param_ptr->frag_ptr %d\n",param_ptr->frag_ptr);

            }
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
            break;
        }

        case OBEX_PUT_FINAL:
        {
            goep_push_ind_struct *param_ptr;
            kal_prompt_trace(1, "Received a Final Put message\n");
            param_ptr = (goep_push_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_push_ind_struct), TD_CTRL);
            param_ptr->ref_count = 1;
            param_ptr->msg_len = 0x174;
            param_ptr->uuid[0] = 0;
            param_ptr->uuid_len = 0;
            param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS ;
            param_ptr->frag_ptr = rdabt_oppsp->buf_ptrOPPS;
            param_ptr->total_obj_len = 0;
            param_ptr->obj_name[0]=0;
            param_ptr->obj_mime_type[0]=0;
            param_ptr->frag_len= 0;
            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);
            kal_prompt_trace(1, "R Put msg,rdabt_oppsp->OPPSfirst=%d,internal.name=%d,internal.type=%d,internal.len=%d,internal.body=%d,internal.eob=%d\n"
                             ,rdabt_oppsp->OPPSfirst,internal.name,internal.type,internal.len,internal.body,internal.eob);
            need_user_confirm ++;
            if(rdabt_oppsp->OPPSfirst)
            {
                memcpy((char*)param_ptr->obj_name , (const char*)((data + internal.name+2)) , (((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                if(param_ptr->obj_name)
                    rdabt_oppsp->OPPSfirst = 0;
                else
                {
                    OBEX_GetWriteBuffer(0,&buffer);
                    OBEX_Response(tid,OBEX_BAD_REQUEST,0,buffer);
                    break;
                }
                if(internal.type)
                {
                    strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                    param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                }
                if(internal.len)
                    param_ptr->total_obj_len = (((data + internal.len)[0] << 8 ) + (data+internal.len)[1] );
                param_ptr->pkt_type = GOEP_SINGLE_PKT;//? or GOEP_FIRST_PKT
                param_ptr->put_type = GOEP_PUT_NORMAL ;
                if(internal.eob )
                {
                    param_ptr->frag_ptr= (data + internal.eob + 2);
                    param_ptr->frag_len= (((data + internal.eob)[0] << 8) + (data + internal.eob)[1] -3);

                }

                rdabt_oppsp->OPPSfirst = 1;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
            }
            else if(internal.eob)
            {
                param_ptr->pkt_type = GOEP_FINAL_PKT;
                param_ptr->put_type = GOEP_PUT_NORMAL ;
                param_ptr->frag_ptr= (data + internal.eob + 2);
                param_ptr->frag_len= (((data + internal.eob)[0] << 8) + (data + internal.eob)[1] -3);

                if(param_ptr->frag_ptr != rdabt_oppsp->buf_ptrOPPS)
                    kal_prompt_trace(1, "May a error!!!!!!! OPPS 4 \n");

                rdabt_oppsp->OPPSfirst = 1;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
                break;
            }
            else
                kal_prompt_trace(1, "May a error!!!!!!! OPPS 5 \n");

            break;
        }


        case OBEX_ABORT:
            kal_prompt_trace(1, "Received a Abort message\n");
            rdabt_goep_abort_ind(0);
            break;

        default:
            kal_prompt_trace(1, "Received a message of type 0x%x from the Client\n",messageType);

            if((0 == messageType) && (rdabt_oppsp->OPPSstatus == GOEP_Client_Connect) && (NULL == data)&&(tid==rdabt_oppsp->OPPStid))
            {

                rdabt_goep_disconnect_rsp_opps(0, NULL, 0);
            }
            else
            {
                ;//OBEX_GetWriteBuffer(0,&buf);
                ;//OBEX_Response(tid,OBEX_NOT_IMPLEMENTED,0,(t_DataBuf*)buf);
            }
            return;
    }

}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_set_floder_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_set_floder_res(goep_set_folder_res_struct *msg_p)//MSG_ID_GOEP_SET_FOLDER_IND
{
    t_DataBuf* buf;

    kal_prompt_trace(1,"rdabt_goep_set_floder_res msg_p ->rsp_code =0x%x",msg_p ->rsp_code );
    if(msg_p ->goep_conn_id == rdabt_ftpsp->goep_tid_FTPS )
    {
        OBEX_GetWriteBuffer(0,&buf);
        if(msg_p ->rsp_code == GOEP_STATUS_SUCCESS )
        {
            OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_OK,0,buf);
        }
        else
            OBEX_Response(rdabt_ftpsp->FTPStid,(msg_p ->rsp_code + 0x80),0,buf);
    }
}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_connect_res
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_pull_res(goep_pull_res_struct *msg_p)//MSG_ID_GOEP_PULL_IND
{
    t_DataBuf* buf;
    u_int16 offset_len;
    u_int32 result = 0;
    int ret = -1;
    u_int16 file_name[128];
    u_int32 write_len;
    u_int16 type_len;
    kal_prompt_trace(1,"rdabt_goep_pull_res, msg_p ->goep_conn_id=0x%x,status=%d, ftps tid=%d,msg_p->rsp_code=%d,msg_p->pkt_type=%d,msg_p ->frag_len=%d",
                     msg_p ->goep_conn_id,rdabt_ftpsp->FTPSstatus,rdabt_ftpsp->goep_tid_FTPS,msg_p->rsp_code,msg_p->pkt_type,msg_p ->frag_len);
    if(msg_p ->goep_conn_id != rdabt_ftpsp->goep_tid_FTPS )
        return;
    if(rdabt_ftpsp->FTPSstatus == GOEP_Client_Disconnect)
        return;


    if(msg_p->rsp_code != GOEP_STATUS_SUCCESS)   //error
    {
        OBEX_GetWriteBuffer(100,&buf);
        OBEX_Response(rdabt_ftpsp->FTPStid, (msg_p->rsp_code | 0x80), 0,buf);
        return;
    }
    offset_len = 3;
    OBEX_GetWriteBuffer(msg_p ->frag_len+100,&buf);
    if(msg_p ->total_obj_len)
        OBEX_Create4ByteHeader(OBEX_LENGTH,msg_p ->total_obj_len,buf,&offset_len);
    type_len = strlen((char* )msg_p->obj_mime_type);
    if(type_len>0)
        OBEX_CreateSequenceHeader(OBEX_TYPE, type_len+1, (u_int8*)msg_p->obj_mime_type, buf, &offset_len);
    if(msg_p->pkt_type == GOEP_FINAL_PKT ||msg_p->pkt_type == GOEP_SINGLE_PKT)
    {
        kal_prompt_trace(1,"rdabt_goep_pull_res GOEP_FINAL_PKT msg_p ->frag_len=0x%x,msg_p ->frag_ptr=0x%x",msg_p ->frag_len,msg_p ->frag_ptr);
        if((msg_p ->frag_len) && (msg_p ->frag_ptr))
            OBEX_CreateSequenceHeader(OBEX_END_BODY,msg_p ->frag_len,msg_p ->frag_ptr,buf,&offset_len);
        result = OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_OK,offset_len,buf);
    }
    else
    {
        kal_prompt_trace(1,"rdabt_goep_pull_res !GOEP_FINAL_PKT");
        if((msg_p ->frag_len) && (msg_p ->frag_ptr))
            OBEX_CreateSequenceHeader(OBEX_BODY,msg_p ->frag_len,msg_p ->frag_ptr,buf,&offset_len);
        result = OBEX_Response(rdabt_ftpsp->FTPStid,OBEX_CONTINUE,offset_len,buf);
    }

}

/////////////////////////////////////OPP SERVER///////////////////////////////////////
void FTPS_worker_handler(u_int32 tid, u_int8 messageType, t_obex_headers headers, u_int8 *data,u_int16 len,void* cbref)
{
    t_DataBuf *buffer;
    u_int16 offset_incoming;
    u_int16 offset_len;
    static u_int32 len_data_recieved;
    u_int8 status;

    t_obex_header_offsets internal;
    int i =0;
    //  rdabt_ftpsp->FTPStid = tid;
    offset_incoming=OBEX_CONNECT_HEADER;

    kal_prompt_trace(1, "Received a FTPS tid %x,rdabt_ftpsp->FTPStid=0x%x,messageType=0x%x,len=%d\n",tid,rdabt_ftpsp->FTPStid,messageType,len);

    switch(messageType)
    {
        case OBEX_CONNECT:
        {
            u_int32 FTPScid;
            kal_prompt_trace(1, "Received a Connect message\n");

            rdabt_ftpsp->FTPSmax_len = (data[5] << 8) + data[6];
            if(rdabt_oppsp->buf_sizeOPPS < rdabt_ftpsp->FTPSmax_len)
                rdabt_ftpsp->FTPSmax_len = rdabt_ftpsp->buf_sizeFTPS;
            rdabt_ftpsp->FTPSmax_len -=20;

            rdabt_ftpsp->FTPSoffsetVar=OBEX_CONNECT_HEADER;
            OBEX_ExtractHeaders(&internal,&(rdabt_ftpsp->FTPSoffsetVar),data,len);

            rdabt_goep_connect_ind(rdabt_ftpsp->FTPSaddress,1);

            rdabt_ftpsp->FTPS_offset_len = OBEX_CONNECT_HEADER;

            OBEX_GetWriteBuffer(300,&(FTPSbuffer));
            OBEX_ExtractHeaders(&internal,&offset_incoming,data,len);

            if(internal.au_cal)
            {
                ; //         OBEX_AuthResponse(temp_pin_code, temp_pin_len, data+internal.au_cal, &offset_len,buffer);
            }

            OBEX_GetCID(tid,((data+internal.tar)[0] << 8) + (data+internal.tar)[1] -3,data+internal.tar + 2,&FTPScid);

            FTPSbuffer->buf[3] = data[3];
            FTPSbuffer->buf[4] = data[4];
            FTPSbuffer->buf[5] = ( rdabt_ftpsp->FTPSmax_len >> 8 )  & 0xFF;
            FTPSbuffer->buf[6] = rdabt_ftpsp->FTPSmax_len & 0xFF;
            OBEX_Create4ByteHeader(OBEX_CID, FTPScid, FTPSbuffer, &(rdabt_ftpsp->FTPS_offset_len));
            if(internal.tar)
                OBEX_CreateSequenceHeader(OBEX_WHO,((data + internal.tar)[0] << 8 ) + (data+internal.tar)[1] -3,data + internal.tar +2,FTPSbuffer,&(rdabt_ftpsp->FTPS_offset_len));

            if(internal.au_rsp)
            {
                //          status=OBEX_AuthCheck(temp_pin_code, temp_pin_len, nonce, windows->data+internal.au_rsp);
                if(status)// means the client could not authenticate
                {
                    offset_len = OBEX_CONNECT_HEADER;
                    OBEX_GetWriteBuffer(100, &buffer);

                    //should I disconnect the transport ??
                    OBEX_Response(tid,OBEX_UNAUTHORIZED,(offset_len),buffer);

                    break;

                }

            }
            else
            {

            }
            break;
        }

        case OBEX_PUT:
        {
            goep_push_ind_struct *param_ptr;
            kal_prompt_trace(1, "Received a Put message\n");
            if(data!=rdabt_ftpsp->buf_ptrFTPS)
            {
                asm("break 1");
            }
            param_ptr = (goep_push_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_push_ind_struct), TD_CTRL);

            memset(param_ptr,0 ,sizeof(goep_push_ind_struct));
            param_ptr->ref_count = 1;
            param_ptr->msg_len = 0x174;
            memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
            param_ptr->uuid_len = 0x10;
            param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
            param_ptr->frag_ptr = rdabt_ftpsp->buf_ptrFTPS;

            param_ptr->total_obj_len = 0;
            param_ptr->obj_name[0]=0;
            param_ptr->obj_mime_type[0]=0;
            param_ptr->frag_ptr = 0;
            param_ptr->frag_len= 0;
            param_ptr->put_type = GOEP_PUT_NORMAL ;

            /* I will need to store data here */
            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);
            kal_prompt_trace(1,"rdabt_ftpsp->FTPSfirst=%d,internal.name=%d,internal.type=%d,internal.len=%d,internal.body=%d,param_ptr=%x,sizeof(goep_push_ind_struct)=%x"
                             ,rdabt_ftpsp->FTPSfirst,internal.name,internal.type,internal.len,internal.body,param_ptr,sizeof(goep_push_ind_struct));


            if(rdabt_ftpsp->FTPSfirst)
            {
                if(internal.name)
                {

                    memcpy((char*)param_ptr->obj_name , (const char*)((data + internal.name+2)) , (((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                    if(param_ptr->obj_name)
                    {
                        len_data_recieved = 0;
                        rdabt_ftpsp->FTPSfirst = 0;
                    }
                    else
                    {
                        OBEX_GetWriteBuffer(0,&buffer);
                        OBEX_Response(tid,OBEX_BAD_REQUEST,0,buffer);
                        break;
                    }
                }
                if(internal.type)
                {
                    strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                    param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                }
                if(internal.len)
                    param_ptr->total_obj_len = (((data + internal.len)[0] << 24 ) + ((data + internal.len)[1] << 16 ) + ((data + internal.len)[2] << 8 ) + (data+internal.len)[3] );

                param_ptr->pkt_type = GOEP_FIRST_PKT;

            }
            else
                param_ptr->pkt_type = GOEP_NORMAL_PKT;

            if(internal.body)
            {
                len_data_recieved += ((data + internal.body)[0] << 8) + (data + internal.body)[1] -3;
                param_ptr->frag_ptr= (data + internal.body + 2);
                param_ptr->frag_len= (((data + internal.body)[0] << 8) + (data + internal.body)[1] -3);

                if(param_ptr->frag_ptr != rdabt_ftpsp->buf_ptrFTPS)
                    kal_prompt_trace(1, "May a error!!!!!!! FTPS 1\n");
            }

            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
            break;

        }
        break;

        case OBEX_DISCONNECT:
            kal_prompt_trace(1, "Received a Disconnect message\n");
            OBEX_GetWriteBuffer(0,&buffer);
            OBEX_Response(tid,OBEX_OK,0,buffer);
            rdabt_goep_disconnect_ind(1);
            break;


        case OBEX_PUT_FINAL:
        {
            goep_push_ind_struct *param_ptr;
            kal_prompt_trace(1, "Received a Final Put message\n");
            param_ptr = (goep_push_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_push_ind_struct), TD_CTRL);
            memset(param_ptr,0 ,sizeof(goep_push_ind_struct));
            param_ptr->ref_count = 1;
            param_ptr->msg_len = 0x174;
            memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
            param_ptr->uuid_len = 0x10;
            param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
            param_ptr->frag_ptr = rdabt_ftpsp->buf_ptrFTPS;

            param_ptr->total_obj_len = 0;
            param_ptr->obj_name[0]=0;
            param_ptr->obj_mime_type[0]=0;
            param_ptr->frag_ptr = 0;
            param_ptr->frag_len= 0;
            /* only here can I process data*/
            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);
            kal_prompt_trace(1,"rdabt_ftpsp->FTPSfirst=%d,internal.eob=%d, internal.body=%d,internal.type=%d,internal.len=%d，param_ptr=%x,sizeof(goep_push_ind_struct)=%x",
                             rdabt_ftpsp->FTPSfirst,internal.eob, internal.body,internal.type,internal.len,param_ptr,sizeof(goep_push_ind_struct));
            if(internal.eob || internal.body)
            {
                if(rdabt_ftpsp->FTPSfirst)
                {
                    memcpy((char*)param_ptr->obj_name , (const char*)((data + internal.name+2)) , (((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                    if(param_ptr->obj_name)
                    {
                        len_data_recieved = 0;
                        rdabt_ftpsp->FTPSfirst = 0;
                    }
                    else
                    {
                        OBEX_GetWriteBuffer(0,&buffer);
                        OBEX_Response(tid,OBEX_BAD_REQUEST,0,buffer);
                        break;
                    }

                    if(internal.type)
                    {
                        strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                        param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                    }

                    if(internal.len)
                        param_ptr->total_obj_len = (((data + internal.len)[0] << 8 ) + (data+internal.len)[1] );

                    param_ptr->pkt_type = GOEP_SINGLE_PKT;//? or GOEP_FIRST_PKT
                    param_ptr->put_type = GOEP_PUT_NORMAL ;

                    if((internal.body == 0)&&(internal.eob == 0))
                    {
                        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
                        break;
                    }
                }
                rdabt_ftpsp->FTPSfirst = 1;

                if(internal.body)
                {
                    len_data_recieved += ((data + internal.body)[0] << 8) + (data + internal.body)[1] -3;
                    param_ptr->pkt_type = GOEP_NORMAL_PKT;//or ?
                    param_ptr->put_type = GOEP_PUT_NORMAL ;
                    param_ptr->frag_ptr= (data + internal.body + 2);
                    param_ptr->frag_len= (((data + internal.body)[0] << 8) + (data + internal.body)[1] -3);

                    if(param_ptr->frag_ptr != rdabt_ftpsp->buf_ptrFTPS)
                        kal_prompt_trace(1, "May a error!!!!!!! FTPS 3 \n");

                    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
                    break;
                }

                if(internal.eob)
                {
                    len_data_recieved += ((data + internal.eob)[0] << 8) + (data + internal.eob)[1] -3;

                    param_ptr->pkt_type = GOEP_FINAL_PKT;
                    param_ptr->put_type = GOEP_PUT_NORMAL ;
                    param_ptr->frag_ptr= (data + internal.eob + 2);
                    param_ptr->frag_len= (((data + internal.eob)[0] << 8) + (data + internal.eob)[1] -3);

                    if(param_ptr->frag_ptr != rdabt_ftpsp->buf_ptrFTPS)
                        kal_prompt_trace(1, "May a error!!!!!!! FTPS 4 \n");

                    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
                    break;

                }
                kal_prompt_trace(1, "May a error!!!!!!! FTPS 5 \n");

                rdabt_ftpsp->FTPSfirst = 1;
                OBEX_GetWriteBuffer(20,&buffer);
                OBEX_Response(tid,OBEX_OK,0,buffer);
            }
            else
            {
                //delete
                if(rdabt_ftpsp->FTPSfirst)
                {
                    memcpy((char*)param_ptr->obj_name , (const char*)((data + internal.name+2)) , (((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                    if(param_ptr->obj_name)
                    {
                        len_data_recieved = 0;
                        //  rdabt_ftpsp->FTPSfirst = 0;
                    }
                    else
                    {
                        OBEX_GetWriteBuffer(0,&buffer);
                        OBEX_Response(tid,OBEX_BAD_REQUEST,0,buffer);
                        break;
                    }

                    if(internal.type)
                    {
                        strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                        param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                    }

                    if(internal.len)
                        param_ptr->total_obj_len = (((data + internal.len)[0] << 8 ) + (data+internal.len)[1] );

                    param_ptr->pkt_type = GOEP_FIRST_PKT;//? or GOEP_FIRST_PKT
                    param_ptr->put_type = GOEP_PUT_DELETE ;
                    kal_prompt_trace(1, "May a error!!!!!!! FTPS 7 %x,%x \n",param_ptr,(param_ptr+sizeof(goep_push_ind_struct)));
                    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PUSH_IND, (void *)param_ptr, NULL);
                    break;
                }
                else
                    kal_prompt_trace(1, "May a error!!!!!!! FTPS 6 \n");
            }
            break;
        }
        case OBEX_GET:
            kal_prompt_trace(1, "Received a OBEX_GET message\n");
        //break;
        case OBEX_GET_FINAL:
        {
            goep_pull_ind_struct *param_ptr;
            kal_prompt_trace(1, "Received a Final Get message\n");
            //kal_sleep_task(100);
            param_ptr = (goep_pull_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_pull_ind_struct), TD_CTRL);
            //param_ptr->ref_count = 1;
            //param_ptr->msg_len = 0x168;
            memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
            param_ptr->uuid_len = 0x10;
            param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;
            param_ptr->pkt_type = GOEP_NORMAL_PKT ;

            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);

            kal_prompt_trace(1, "headers type=%d name=%d",internal.type,internal.name);

            memset((char*)param_ptr->obj_name , 0 , GOEP_MAX_OBJ_NAME);
            memset((char*)param_ptr->obj_mime_type , 0 , GOEP_MAX_MIME_TYPE);

            if((internal.type == 0) && (internal.name == 0))
            {
                memcpy(  (char*)param_ptr->obj_mime_type , (const char*)(rdabt_ftpsp->ftps_objs_mime_type) , (GOEP_MAX_MIME_TYPE));
                memcpy(  (char*)param_ptr->obj_name , (const char*)(rdabt_ftpsp->ftps_obj_name) , (GOEP_MAX_OBJ_NAME));
            }

            if(internal.type)
            {
                kal_prompt_trace(1,"type len=%d",(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                strncpy(  (char*)param_ptr->obj_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                param_ptr->obj_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
                strncpy(  (char*)rdabt_ftpsp->ftps_objs_mime_type , (const char*)(data + internal.type + 2) , (((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3));
                rdabt_ftpsp->ftps_objs_mime_type[(((data + internal.type)[0] << 8 ) + (data+internal.type)[1] -3)] = 0;
            }
            else
            {
                memset((char*)rdabt_ftpsp->ftps_objs_mime_type , 0 , GOEP_MAX_MIME_TYPE);
            }


            if(internal.name)
            {
                kal_prompt_trace(1,"name len=%d",(((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3));
                for( i =0; i<(((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3); i++)
                {
                    param_ptr->obj_name[i]=(data + internal.name+2)[i];
                    rdabt_ftpsp->ftps_obj_name[i]=(data + internal.name+2)[i];
                }
                param_ptr->obj_name[i]=0;
                rdabt_ftpsp->ftps_obj_name[i]=0;
                param_ptr->obj_name[i+1]=0;
                rdabt_ftpsp->ftps_obj_name[i+1]=0;
            }
            else
            {
                memset((char*)rdabt_ftpsp->ftps_obj_name , 0 , GOEP_MAX_OBJ_NAME);
            }
            //kal_sleep_task(100);
            kal_prompt_trace(1,"Send message MSG_ID_GOEP_PULL_IND");
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_IND, (void *)param_ptr, NULL);
            break;
        }

        case OBEX_SETPATH:
        {
            goep_set_folder_ind_struct *param_ptr;
            param_ptr = (goep_set_folder_ind_struct*)construct_local_para((kal_uint16)sizeof(goep_set_folder_ind_struct), TD_CTRL);
            //param_ptr->ref_count = 1;
            //param_ptr->msg_len = 0x118;
            memcpy((char*)param_ptr->uuid , (const char*)(OBEX_FTP_UUID) , 16);
            param_ptr->uuid_len = 0x10;
            param_ptr->goep_conn_id = rdabt_ftpsp->goep_tid_FTPS ;

            offset_len = OBEX_HEADER;
            OBEX_ExtractHeaders(&internal,&offset_len,data,len);

            if(data[3] & 0x01)    // go back up a directory
            {
                param_ptr->setpath_flag = GOEP_BACK_FOLDER ;
            }
            else if(!(data[3] & 0x2))
            {
                param_ptr->setpath_flag = GOEP_CREATE_FOLDER ;
            }
            else if(((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3)
            {
                param_ptr->setpath_flag = GOEP_FORWARD_FOLDER ;
            }
            else
            {
                param_ptr->setpath_flag = GOEP_ROOT_FOLDER ;
            }


            memset((char*)param_ptr->folder_name , 0 , GOEP_MAX_FOLDER_NAME);

            if(internal.name)
            {
                for( i =0; i<(((data + internal.name)[0] << 8 ) + (data+internal.name)[1] -3); i++)
                {
                    param_ptr->folder_name[i]=(data + internal.name+2)[i];
                }
            }
            kal_prompt_trace(1, "Received a SetPath message param_ptr->setpath_flag=0x%x\n",param_ptr->setpath_flag);

            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_SET_FOLDER_IND, (void *)param_ptr, NULL);


            break;
        }

        case OBEX_ABORT:
            kal_prompt_trace(1, "Received a Abort message\n");
            rdabt_goep_abort_ind(1);
            break;
        default:
            kal_prompt_trace(1, "Received a message : 0x%x,rdabt_ftpsp->FTPSstatus=%d \n",messageType,rdabt_ftpsp->FTPSstatus);
            if((0 == messageType) /*&& (rdabt_ftpsp->FTPSstatus== GOEP_Client_Connect) */&& (NULL == data)&&(rdabt_ftpsp->FTPStid==tid))
            {
                //rdabt_goep_abort_ind(1);
                //rdabt_ftpsp->FTPSstatus = GOEP_Client_Disconnect;
                //OBEX_ServerTransportDisconnect(rdabt_ftpsp->FTPStid);
                rdabt_goep_disconnect_rsp_ftps(0, NULL, 0);
            }
            else
            {
                kal_prompt_trace(1, "Received a message : %x \n",messageType);
                //OBEX_GetWriteBuffer(0,&buf);
                //OBEX_Response(tid,OBEX_NOT_IMPLEMENTED,0,(t_DataBuf*)buf);
            }

            break;
    }

    return;
}



void rdabt_obex_dispatch_event(rdabt_msg_t *msg)
{


    kal_prompt_trace(1, "rdabt_obex_dispatch_event msg->msg_id=0x%x,rdabt_ftpsp->FTPStid=%d,rdabt_oppsp->OPPStid=%d",msg->msg_id,rdabt_ftpsp->FTPStid ,rdabt_oppsp->OPPStid);
    switch(msg->msg_id)
    {
        case OBEX_CLI_RF_DATA_IND:
        {
            obex_cli_data_ind_msg_t *obex_data_message = (obex_cli_data_ind_msg_t*)(msg->data ) ;
            // kal_prompt_trace(1, "rdabt_obex_dispatch_event process_pending=%d,obex_data_message->tid=0x%x,SecondConnect_dlci=0x%x, rdabt_oppcp->goep_tidOPPC=0x%x,rdabt_ftpsp->FTPStid=0x%x, rdabt_oppsp->OPPStid=0x%x,",
            //obex_data_message->process_pending,obex_data_message->tid,ServerSecondConnect_dlci, rdabt_oppcp->goep_tidOPPC,rdabt_ftpsp->FTPStid, rdabt_oppsp->OPPStid);
#ifdef __SUPPORT_BT_SYNC_PHB__
            if(last_client_connect ==SCLASS_UUID_PHONEBOOK_ACCESS_PSE)
            {
                rdabt_pbap_data_ind_callback(obex_data_message);
                break;
            }
#endif
#ifdef __SUPPORT_BT_SYNC_MAP__

            if(last_client_connect ==SCLASS_UUID_MESSAGE_ACCESS_MSE)
            {
                rdabt_map_data_ind_callback(obex_data_message);
                break;
            }
#endif
            switch(obex_data_message->process_pending)
            {
                case OBEX_CONNECT_CNF_ID :
                    OBEX_Connect_callback(obex_data_message->typeorstatus,obex_data_message->rx_buffer,obex_data_message->length,obex_data_message->tid);
                    break;
                case   OBEX_DISCONNCET_CNF_ID:
                {

                    if((obex_data_message->tid!= rdabt_oppsp->OPPStid)&&(obex_data_message->tid!=rdabt_ftpsp->FTPStid)&&(obex_data_message->dlci!=ServerSecondConnect_dlci))

                        rdabt_goep_disconnect_rsp(obex_data_message->typeorstatus,obex_data_message->rx_buffer,obex_data_message->length);
                    //ADD for ftp client 's disconnect process
                    break;
                }
                case OBEX_PUT_ID   :
                    goep_push_callback(obex_data_message->typeorstatus,obex_data_message->rx_buffer,obex_data_message->length);
                    //ADD for ftp client 's disconnect process
                    break;

                case OBEX_ABORT_ID:
                    rdabt_goep_abort_req_reply(obex_data_message->typeorstatus,obex_data_message->rx_buffer,obex_data_message->length);
                    break;
                //next is for ftp add in future.
                case OBEX_GET_ID   :
                    //rdabt_get_callback(obex_data_message);
                    break;
                case   OBEX_SET_PATH_ID:
                    //rdabt_set_path_callback(obex_data_message);
                    break;
                default:

                    break;
            }
        }
        break;


        case OBEX_SERVER_RECEIVE_DATA_IND:
        {
            obex_server_data_ind_msg_t *ser_data_ind_msg=(obex_server_data_ind_msg_t*)msg->data;
            kal_prompt_trace(1, "rdabt_obex_dispatch_event OBEX_SERVER_RECEIVE_DATA_IND 0x%x,0x%x",ser_data_ind_msg->server_tid,rdabt_oppsp->goep_tid_OPPS);
            if (ser_data_ind_msg->tid== rdabt_ftpsp->FTPStid)
                FTPS_worker_handler(ser_data_ind_msg->tid , ser_data_ind_msg->messageType , ser_data_ind_msg->headers , ser_data_ind_msg->data , ser_data_ind_msg->len , ser_data_ind_msg->cbref);
            else if((ser_data_ind_msg->tid==rdabt_oppsp->OPPStid)||(ser_data_ind_msg->messageType==OBEX_DISCONNECT))
                OPPServiceHandler(ser_data_ind_msg->tid , ser_data_ind_msg->messageType , ser_data_ind_msg->headers , ser_data_ind_msg->data , ser_data_ind_msg->len , ser_data_ind_msg->cbref);
            else
                asm("break 1");
        }
        break;
        case OBEX_SERVER_AUTHORIZE_IND:
        {
            obex_server_authorize_ind_msg_t *authorize_ind_msg=(obex_server_authorize_ind_msg_t*)( msg->data);
            kal_prompt_trace(1, "OBEX_SERVER_AUTHORIZE_IND authorize_ind_msg->tid=0x%x,rdabt_ftpsp->FTPSstatus=0x%x,rdabt_oppsp->OPPSstatus=0x%x,authorize_ind_msg->uuid=%x",
                             authorize_ind_msg->tid,rdabt_ftpsp->FTPSstatus,rdabt_oppsp->OPPSstatus,authorize_ind_msg->uuid);
            if(authorize_ind_msg->uuid==SDP_SCLASS_UUID_OBEX_FTP)
            {
                if((rdabt_ftpsp->FTPSstatus==GOEP_Client_Disconnect)||(rdabt_ftpsp->FTPSstatus==GOEP_Client_Idle))
                {
                    memset(rdabt_ftpsp->ftps_obj_name, 0, GOEP_MAX_OBJ_NAME);
                    rdabt_ftpsp->FTPStid=authorize_ind_msg->tid;
                    memcpy(rdabt_ftpsp->FTPSaddress.bytes,authorize_ind_msg->addr.bytes,6);
                }

                else
                {
                    RSE_SrvConnectResponse(authorize_ind_msg->addr, authorize_ind_msg->dlci, 0);//only support one ftp client device connected
                    ServerSecondConnect_dlci=authorize_ind_msg->dlci;
                    break;
                }

            }
            else if(authorize_ind_msg->uuid==SDP_SCLASS_UUID_OBEX_PUSH)

            {
                if((rdabt_oppsp->OPPSstatus==GOEP_Client_Disconnect)||(rdabt_oppsp->OPPSstatus==GOEP_Client_Idle))
                {
                    memset(rdabt_oppsp->opps_obj_name, 0, 80);
                    rdabt_oppsp->OPPStid=authorize_ind_msg->tid;
                    memcpy(rdabt_ftpsp->FTPSaddress.bytes,authorize_ind_msg->addr.bytes,6);
                }
                else
                {

                    RSE_SrvConnectResponse(authorize_ind_msg->addr, authorize_ind_msg->dlci, 0);//only support one opp client device connected
                    ServerSecondConnect_dlci=authorize_ind_msg->dlci;

                    break;
                }
            }
            // if( rdabt_get_device_authriose_info(authorize_ind_msg->addr))
            //    RSE_SrvConnectResponse(authorize_ind_msg->addr, authorize_ind_msg->dlci, 1);//autheriosed devices
            //else
            rdabt_goep_authorize_ind(authorize_ind_msg->addr,authorize_ind_msg->dlci,authorize_ind_msg->uuid);

        }
        break;
        default:

            break;
    }

}


//end new//end new//end new//end new//end new//end new//end new












rdabt_client_struct rdabt_client;
rdabt_client_struct* rdabt_clientp = &rdabt_client;
void rdabt_goep_init(void)
{

    rdabt_clientp->goep_tidClient = 0xff;
    rdabt_clientp->req_idClient = 0xffff;
    rdabt_clientp->ClientStatus = GOEP_Client_Idle;

    rdabt_oppsp->goep_tid_OPPS = 0xffff;
    rdabt_oppsp->req_idOPPS = 0xff;
    rdabt_oppsp->OPPStid = 0xffff;
    rdabt_oppsp->OPPSfirst=1;
    rdabt_oppsp->OPPSstatus = GOEP_Client_Idle;

    rdabt_ftpsp->goep_tid_FTPS =0xffff;
    rdabt_ftpsp->req_idFTPS = 0xff;
    rdabt_ftpsp->FTPStid = 0xffff;
    rdabt_ftpsp->FTPSfirst=1;
    rdabt_ftpsp->FTPSstatus = GOEP_Client_Idle;


}

void rdabt_FolderListing_callback(u_int8 status, u_int8* data, u_int16 length)
{
    kal_prompt_trace(1, "rdabt_FolderListing_callback...  !@!!!!!! status = %d,length = %d\n",status,length);
}

void goep_pull_callback(u_int8 status, u_int8 *finger, u_int16 length)
{
#if 0
    u_int8* result_data = 0x00;
    u_int16 result_length;
    goep_pull_rsp_struct *param_ptr = (goep_pull_rsp_struct*) construct_local_para(sizeof(goep_pull_rsp_struct), TD_CTRL);;

    u_int16 offset_len;
    t_obex_header_offsets headers;
    u_int8 retValue=1;
    u_int32 fileRxLen=0;
    kal_prompt_trace(1, "goep_pull_callback... rdabt_clientp->goep_tidClient =%d !@!!!!!! status = %d,finger= 0x%x,length = %d\n",rdabt_clientp->goep_tidClient,status,finger,length);

    param_ptr->goep_conn_id = rdabt_clientp->goep_tidClient;
    if (rdabt_clientp->isFtpClient ==0)
    {
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
    }
    else
    {
        memcpy(param_ptr->uuid,OBEX_FTP_UUID,16);
        param_ptr->uuid_len = 16;
    }

    result_data = rdabt_clientp->buf_ptrFTC;
    result_length = 0;
    offset_len = OBEX_HEADER;
    if ( status == OBEX_DISCONNECT)
    {
        param_ptr->rsp_code =GOEP_STATUS_DISCONNECT;
        param_ptr->reason =status;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_RSP, (void *)param_ptr, NULL);
        //rdabt_clientp->ClientStatus = GOEP_Client_Idle;
        //rdabt_goep_disconnect_rsp_oppc(0, NULL, 0);
        return;
    }
    retValue=OBEX_ExtractHeaders(&headers, &offset_len,finger,length);

    /* check headers were extracted ok*/
    if(retValue)
    {
        param_ptr->rsp_code =1;
        param_ptr->reason =status;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_RSP, (void *)param_ptr, NULL);
        rdabt_clientp->ClientStatus = GOEP_Client_Idle;
        return;
    }
    kal_prompt_trace(1, "goep_pull_callback...  headers.len = %d,headers.body= %d,headers.eob = %d\n",headers.len,headers.body,headers.eob);
    switch(status)
    {
        /*
        Success
        */
        case OBEX_OK:
            if(headers.len)
            {
                fileRxLen= ((finger+headers.len)[0] << 24) + ((finger+headers.len)[1] << 16) + ((finger+headers.len)[2] << 8) + ((finger+headers.len)[3]);
            }
            kal_prompt_trace(1, "OBEX_OK...  !@!!!!!! fileRxLen = %d",fileRxLen);
            if(headers.body)
            {
                u_int16 temp;
                /*
                shift 1st byte as it's the msb, add second byte to give the len.
                -OBEX_HEADER for the header id and length fields
                */
                temp = ((finger + headers.body)[0] << 8) + (finger + headers.body)[1] -OBEX_HEADER;
                pMemcpy(result_data + result_length, finger + headers.body + 2,temp);
                result_length += temp;
            }

            if(headers.eob)
            {
                u_int16 temp;
                /*
                shift 1st byte as it's the msb, add second byte to give the len.
                -OBEX_HEADER for the header id and length fields
                */
                temp = ((finger + headers.eob)[0] << 8) + (finger + headers.eob)[1] -OBEX_HEADER;
                pMemcpy(result_data + result_length, finger + headers.eob + 2,temp);
                result_length += temp;
            }
            /*
            Call the user registered callback function - OBEX_OK indicates it's finished
            so no need to put the below code in the "if(headers.eob)" path.
            */
            //prh_user_obex_ftp_genpurpose_callback(OBEX_OK,result_data,result_length);
            param_ptr->rsp_code =0;
            param_ptr->reason =0;
            param_ptr->frag_len = result_length;
            param_ptr->frag_ptr = result_data;
            param_ptr->total_obj_len = fileRxLen;
            param_ptr->to_self = 0;
            kal_prompt_trace(1, "frag_len = %d frag_ptr=0x%x total_obj_len = %d...  !@!!!!!! ",param_ptr->frag_len,param_ptr->frag_ptr,fileRxLen);
            if (rdabt_clientp->FTCfirst)
                param_ptr->pkt_type = GOEP_SINGLE_PKT;
            else
                param_ptr->pkt_type = GOEP_FINAL_PKT;
            rdabt_clientp->FTCfirst = 1;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_RSP, (void *)param_ptr, NULL);
            break;
        case OBEX_CONTINUE:
            if(headers.len)
            {
                fileRxLen= ((finger+headers.len)[0] << 24) + ((finger+headers.len)[1] << 16) + ((finger+headers.len)[2] << 8) + ((finger+headers.len)[3]);
            }
            kal_prompt_trace(1, "OBEX_CONTINUE...  !@!!!!!! fileRxLen = %d",fileRxLen);
            /*
            Means I have to send another Get with no headers. but there is can still be a body here
            so I must take the data if it exists.
            */
            if(headers.body)
            {
                /*
                shift 1st byte as it's the msb, add second byte to give the len.
                -3 for the header id and length fields
                */
                u_int16 temp;
                temp = ((finger + headers.body)[0] << 8) + (finger + headers.body)[1] -OBEX_HEADER;
                pMemcpy(result_data + result_length, finger + headers.body + 2,temp);
                result_length += temp;
            }
            param_ptr->rsp_code =0;
            param_ptr->reason =0;
            param_ptr->frag_len = result_length;
            param_ptr->frag_ptr = result_data;
            param_ptr->total_obj_len = fileRxLen;
            param_ptr->to_self = 0;
            kal_prompt_trace(1, "frag_len = %d frag_ptr=0x%x total_obj_len = %d...  !@!!!!!! ",param_ptr->frag_len,param_ptr->frag_ptr,fileRxLen);
            if (rdabt_clientp->FTCfirst)
                param_ptr->pkt_type = GOEP_FIRST_PKT;
            else
                param_ptr->pkt_type = GOEP_NORMAL_PKT;
            rdabt_clientp->FTCfirst = 0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_RSP, (void *)param_ptr, NULL);

            break;
        default:
            /* error case */
            kal_prompt_trace(1, "goep_pull_callback...  default case rdabt_clientp->ClientStatus = %d",rdabt_clientp->ClientStatus);
            param_ptr->rsp_code =1;
            param_ptr->reason =status;
            param_ptr->to_self = 0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_PULL_RSP, (void *)param_ptr, NULL);
            // rdabt_clientp->ClientStatus = GOEP_Client_Idle;
            break;
    }
    return;
#endif
}

void goep_set_folder_callback(u_int8 status, u_int8* finger, u_int16 length)
{
    u_int8 retValue;
    u_int16 offset_len;
    t_obex_header_offsets headers;
    goep_set_folder_rsp_struct *param_ptr = (goep_set_folder_rsp_struct*) construct_local_para(sizeof(goep_set_folder_rsp_struct), TD_CTRL);;
    memcpy(param_ptr->uuid,OBEX_FTP_UUID,16);
    param_ptr->uuid_len = 16;
    param_ptr->goep_conn_id = rdabt_clientp->goep_tidClient;

    offset_len=OBEX_HEADER;
    if ( status == OBEX_DISCONNECT)
    {
        param_ptr->rsp_code =GOEP_STATUS_DISCONNECT;
        param_ptr->reason =status;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_SET_FOLDER_RSP, (void *)param_ptr, NULL);
        //rdabt_clientp->ClientStatus = GOEP_Client_Idle;
        //rdabt_goep_disconnect_rsp_oppc(0, NULL, 0);
        return;
    }
    retValue=OBEX_ExtractHeaders(&headers, &offset_len,finger,length);

    /* check headers were extracted ok*/
    if(retValue)
    {
        param_ptr->rsp_code =1;
        param_ptr->reason =*finger;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_SET_FOLDER_RSP, (void *)param_ptr, NULL);
        return;
    }

    /* I will take this out later as it increases code size */

    switch(*finger)
    {
        /*
        Success - callback with the cid to the user
        */
        case OBEX_OK:

            /*
            Must be from a setpath call
            */
            param_ptr->rsp_code =0;
            param_ptr->reason =0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_SET_FOLDER_RSP, (void *)param_ptr, NULL);

            break;

        /*
        not successful - return status and data to user
        */
        default:

            param_ptr->rsp_code =1;
            param_ptr->reason =*finger;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_SET_FOLDER_RSP, (void *)param_ptr, NULL);

            break;
    }

    return;
}


void rdabt_goep_set_folder_req(goep_set_folder_req_struct *msg_p)
{
    u_int8 status;
    u_int16 t_offset;
    t_DataBuf* buff;
    u_int8 setPathFlags=0;
    u_int16 NameLength = (u_int16) UCS2Strlen(msg_p ->folder_name);

    /*
    set the offset
    */
    t_offset = OBEX_SETPATH_HEADER;

    status=OBEX_GetWriteBuffer(300,&buff); /* I will need to trim this down a bit*/
    if(status)
        return ;
    if (msg_p->setpath_flag == GOEP_FORWARD_FOLDER)
    {
        status=OBEX_Create4ByteHeader(OBEX_CID,rdabt_clientp->goep_cidClient,buff,&t_offset);
        if(status)
            return ;
        status=OBEX_CreateStringHeader(OBEX_NAME,NameLength,(uint16 *)msg_p->folder_name,buff,&t_offset);
        setPathFlags = 0X02;
    }
    else if (msg_p->setpath_flag == GOEP_BACK_FOLDER)
    {
        status=OBEX_Create4ByteHeader(OBEX_CID,rdabt_clientp->goep_cidClient,buff,&t_offset);
        if(status)
            return ;

        setPathFlags = 0X03;
    }
    else if (msg_p->setpath_flag == GOEP_ROOT_FOLDER)
    {
        status=OBEX_Create4ByteHeader(OBEX_CID,rdabt_clientp->goep_cidClient,buff,&t_offset);
        if(status)
            return ;
        status=OBEX_CreateStringHeader(OBEX_NAME,0,0,buff,&t_offset);
        setPathFlags = 0X02;
    }
    /*
    Call OBEX_SetPath();
    */
//  status=OBEX_SetPath(rdabt_clientp->goep_tidClient,setPathFlags,0,t_offset,buff,goep_set_folder_callback);

    return;
}
void rdabt_goep_pull_req(goep_pull_req_struct *msg_p)
{
    /*
        if (!strcmp(msg_p->obj_mime_type,"x-obex/folder-listing"))
            OBEX_GetFolderListingObject(rdabt_clientp->goep_tidClient,rdabt_clientp->goep_cidClient,rdabt_FolderListing_callback);
        */
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    t_DataBuf *buff;
    u_int16 Offset;
    u_int16 NameLength = (u_int16) UCS2Strlen(msg_p ->obj_name);
    u_int32 TypeLength = (u_int32)strlen((char* )msg_p ->obj_mime_type)+1;


    kal_prompt_trace(1, "rdabt_goep_pull_req...  20090220-Rdabt_opp!@!!!!!!,TypeLength =%d rdabt_clientp->goep_cidClient=%d",TypeLength,rdabt_clientp->goep_cidClient);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch( msg_p ->pkt_type )
    {
        case GOEP_FIRST_PKT:
            rdabt_clientp->FTCfirst = 1;
            Offset = 3;
            kal_prompt_trace(1, "GOEP_FIRST_PKT,NameLength =%d",NameLength);
            kal_prompt_trace(1, "GOEP_FIRST_PKT,TypeLength =%d",TypeLength);
            OBEX_GetWriteBuffer(NameLength*2+TypeLength+50,&buff); /*need to reallocate as the transport free the */
            OBEX_Create4ByteHeader(OBEX_CID,rdabt_clientp->goep_cidClient,buff,&Offset);
            OBEX_CreateStringHeader(OBEX_NAME,NameLength,(uint16 *)msg_p ->obj_name,buff,&Offset);
            OBEX_CreateSequenceHeader(OBEX_TYPE,TypeLength==1?0:TypeLength,msg_p ->obj_mime_type,buff,&Offset);
            //OBEX_Get(msg_p ->goep_conn_id,1,Offset,(t_DataBuf*)buff,goep_pull_callback);
            break;

        case GOEP_NORMAL_PKT:
            kal_prompt_trace(1, "GOEP_NORMAL_PKT");
            if (TypeLength > 1)
            {
                OBEX_GetWriteBuffer(0,&buff);
            }
            else
            {
                OBEX_GetWriteBuffer(20,&buff);
            }
            //OBEX_Get(msg_p ->goep_conn_id,1,0,buff,goep_pull_callback);
            break;

        case GOEP_FINAL_PKT:
        case GOEP_SINGLE_PKT:
            kal_prompt_trace(1, "GOEP_FINAL_PKT msg_p ->pkt_type=%d\n",msg_p ->pkt_type);
            /*
                      Offset = 3;
                            max_len = msg_p ->frag_len;
                            OBEX_GetWriteBuffer(max_len+50,&buff);
                            OBEX_CreateSequenceHeader(OBEX_END_BODY,max_len,msg_p ->frag_ptr,buff,&Offset);
                            OBEX_Get(msg_p ->goep_conn_id,1,Offset,buff,goep_pull_callback);
            */
            break;

        default:
        {
            kal_prompt_trace(1, "default msg_p ->pkt_type=%d\n",msg_p ->pkt_type);
            break;
        }
    }
    return;
}



/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_link_disallow
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/

void rdabt_goep_link_disallow(void)
{
    // kal_prompt_trace(1,"rdabt_goep_link_disallow rdabt_oppsp->OPPSstatus=%d,rdabt_ftpsp->FTPSstatus=%d ",rdabt_oppsp->OPPSstatus,rdabt_ftpsp->FTPSstatus);
    if(rdabt_oppsp->OPPSstatus == GOEP_Client_Connect)
    {
        //OBEX_ServerTransportDisconnect(rdabt_oppsp->OPPStid);
        rdabt_goep_disconnect_rsp_opps(0, NULL, 0);
    }

    if(rdabt_ftpsp->FTPSstatus == GOEP_Client_Connect)
    {
        //OBEX_ServerTransportDisconnect(rdabt_ftpsp->FTPStid);
        rdabt_goep_disconnect_rsp_ftps(0, NULL, 0);
    }
    COS_Sleep(20);

}



/*****************************************************************************
 * FUNCTION
 *  rdabt_bt_sdpdb_register_req_reply
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_bt_sdpdb_register_req_reply(kal_uint32 uuid, u_int8* data, u_int16 length)
{
    bt_sdpdb_register_cnf_struct *param_ptr ;
    param_ptr = (bt_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_register_cnf_struct), TD_CTRL);
    kal_prompt_trace(1, "rdabt_bt_sdpdb_register_req_reply...  20090220-Rdabt_opp!@!!!!!!,uuid = %d",uuid);

    param_ptr->ref_count = 1;
    param_ptr->msg_len = 0xc;
    param_ptr->result = 0;
    param_ptr-> uuid = uuid;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_SDPDB_REGISTER_CNF, (void *)param_ptr, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  rdabt_bt_sdpdb_register_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_bt_sdpdb_register_req(bt_sdpdb_register_req_struct *msg_p)
{

    kal_prompt_trace(1, "rdabt_bt_sdpdb_register_req...  20090220-Rdabt_opp!@!!!!!! ,msg_p ->uuid  =%x",msg_p ->uuid);

#if 0
    if(msg_p ->uuid == 0x1106)
        OBEX_RegisterFTPServer(RDA_FTPS_Server_Channel);
    else if(msg_p ->uuid == 0x1105)
        OBEX_RegisterObjectPushServer(RDA_OPPS_Server_Channel,(u_int8)types);
#endif
    rdabt_bt_sdpdb_register_req_reply(msg_p ->uuid, NULL, 0);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  rdabt_sdpdb_deregister_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_sdpdb_deregister_req(bt_sdpdb_deregister_req_struct *msg_p)
{
    kal_prompt_trace(1,"rdabt_sdpdb_deregister_req FTPS msg_p->uuid=0x%x" ,msg_p->uuid);

    if(msg_p->uuid == 0x1105)
    {
        /*
            goep_deregister_server_rsp_struct *param_ptr ;

            param_ptr = (goep_deregister_server_rsp_struct*) construct_local_para(sizeof(goep_deregister_server_rsp_struct), TD_CTRL);

            param_ptr->ref_count = 1;
            param_ptr->msg_len = 0x18;
            param_ptr->goep_conn_id = rdabt_oppsp->goep_tid_OPPS;
            param_ptr->rsp_code = 0;
            param_ptr->uuid[0] = 0;
            param_ptr->uuid_len = 0;

            //   kal_prompt_trace(1,"rdabt_goep_deregister_req OPPS");
        */
        bt_sdpdb_deregister_cnf_struct *param_ptr ;
        param_ptr = (bt_sdpdb_deregister_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_cnf_struct), TD_CTRL);
        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0xc;
        param_ptr->result = 0;
        param_ptr->uuid = msg_p->uuid;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_SDPDB_DEREGISTER_CNF, (void *)param_ptr, NULL);
        RSE_SrvDeregisterPort(RDA_OPPS_Server_Channel);
        //rdabt_goep_init();
    }
    else
    {
        bt_sdpdb_deregister_cnf_struct *param_ptr ;

        //   kal_prompt_trace(1,"rdabt_sdpdb_deregister_req FTPS");

        param_ptr = (bt_sdpdb_deregister_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_cnf_struct), TD_CTRL);

        param_ptr->ref_count = 1;
        param_ptr->msg_len = 0xc;
        param_ptr->result = 0;
        param_ptr->uuid = msg_p->uuid;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_SDPDB_DEREGISTER_CNF, (void *)param_ptr, NULL);
        RSE_SrvDeregisterPort(RDA_FTPS_Server_Channel);

        if(rdabt_ftpsp->FTPSstatus == GOEP_Client_Connect)
        {
            rdabt_goep_disconnect_rsp_ftps(0, NULL, 0);
        }

    }

}


/*****************************************************************************
 * FUNCTION
 *  rdabt_goep_deregister_req
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_goep_deregister_req(goep_deregister_server_req_struct *msg_p)
{
    goep_deregister_server_rsp_struct *param_ptr ;
    param_ptr = (goep_deregister_server_rsp_struct*) construct_local_para(sizeof(goep_deregister_server_rsp_struct), TD_CTRL);
    kal_prompt_trace(1,"msg_p->goep_conn_id = %d,rdabt_oppsp->goep_tid_OPPS =%d,rdabt_ftpsp->goep_tid_FTPS =%d\n",msg_p->goep_conn_id,rdabt_oppsp->goep_tid_OPPS,rdabt_ftpsp->goep_tid_FTPS );
    param_ptr->ref_count = 1;
    param_ptr->msg_len = 0x18;
    param_ptr->goep_conn_id = msg_p->goep_conn_id;
    param_ptr->rsp_code = 0;

    if( rdabt_oppsp->goep_tid_OPPS == msg_p->goep_conn_id)
    {
        param_ptr->uuid[0] = 0;
        param_ptr->uuid_len = 0;
    }

    if( rdabt_ftpsp->goep_tid_FTPS == msg_p->goep_conn_id)
    {
        int lcv = 0;
        for(lcv = 0; lcv < 16; lcv++)
        {
            param_ptr->uuid[lcv] = OBEX_FTP_UUID[lcv];
        }
        param_ptr->uuid_len = 0x10;
        rdabt_goep_init();
    }
    rdabt_send_msg_up(MOD_MMI, MSG_ID_GOEP_DEREGISTER_SERVER_RSP, (void *)param_ptr, NULL);

}

/*****************************************************************************
 * FUNCTION
 *  rdabt_opp_msg_hdler
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
void rdabt_opp_msg_hdler(ilm_struct *ext_message)
{
    switch(ext_message->msg_id)
    {
        case MSG_ID_GOEP_CONNECT_REQ:
        {
            goep_connect_req_struct *msg_p = (goep_connect_req_struct *)ext_message->local_para_ptr;
            t_bdaddr tempbd;
            BDADDR_Set_LAP(&tempbd,msg_p ->bd_addr.lap);
            BDADDR_Set_UAP(&tempbd,msg_p ->bd_addr.uap);
            BDADDR_Set_NAP(&tempbd,msg_p ->bd_addr.nap);
            rdabt_goep_connect_req(tempbd,msg_p);///PIN CODE Negative
            break;
        }

        case MSG_ID_GOEP_PUSH_REQ:
        {
            goep_push_req_struct *msg_p = (goep_push_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_push_req(msg_p);

            break;
        }

        case MSG_ID_GOEP_PULL_REQ:
        {
            goep_pull_req_struct *msg_p = (goep_pull_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_pull_req(msg_p);
            break;
        }

        case MSG_ID_GOEP_SET_FOLDER_REQ:
        {
            goep_set_folder_req_struct *msg_p = (goep_set_folder_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_set_folder_req(msg_p);
            break;
        }

        case MSG_ID_GOEP_DISCONNECT_REQ:
        case MSG_ID_GOEP_TPDISCONNECT_REQ:
        {
            goep_disconnect_req_struct *msg_p = (goep_disconnect_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_disconnect_req(msg_p);

            break;
        }

        case MSG_ID_GOEP_ABORT_REQ:
        {
            goep_abort_req_struct *msg_p = (goep_abort_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_abort_req(msg_p);

            break;
        }

        case MSG_ID_GOEP_REGISTER_SERVER_REQ:
        {
            goep_register_server_req_struct *msg_p = (goep_register_server_req_struct *)ext_message->local_para_ptr;
            rdabt_goep_register_server_req(msg_p);

            break;
        }

#if 0
        case MSG_ID_BT_SDPDB_REGISTER_REQ:
        {
            bt_sdpdb_register_req_struct *msg_p = (bt_sdpdb_register_req_struct *)ext_message->local_para_ptr;
            rdabt_bt_sdpdb_register_req(msg_p);

            break;
        }
#endif

        case MSG_ID_GOEP_AUTHORIZE_RES:
        {
            goep_authorize_res_struct *msg_p = (goep_authorize_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_authorize_res(msg_p);

            break;
        }

        case MSG_ID_GOEP_CONNECT_RES:
        {
            goep_connect_res_struct *msg_p = (goep_connect_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_connect_res(msg_p);

            break;
        }

        case MSG_ID_GOEP_PUSH_RES:
        {
            goep_push_res_struct *msg_p = (goep_push_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_push_res(msg_p);

            break;
        }

        case MSG_ID_GOEP_ABORT_RES:
        {
            goep_abort_res_struct *msg_p = (goep_abort_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_abort_res(msg_p);

            break;
        }

        case MSG_ID_GOEP_PULL_RES:
        {
            goep_pull_res_struct *msg_p = (goep_pull_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_pull_res(msg_p);

            break;
        }

        case MSG_ID_GOEP_SET_FOLDER_RES:
        {
            goep_set_folder_res_struct *msg_p = (goep_set_folder_res_struct *)ext_message->local_para_ptr;
            rdabt_goep_set_floder_res(msg_p);

            break;
        }

#if 0
        case MSG_ID_BT_SDPDB_DEREGISTER_REQ:
        {
            bt_sdpdb_deregister_req_struct *msg_p = (bt_sdpdb_deregister_req_struct *)ext_message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_SDPDB_DEREGISTER_REQ");
            rdabt_sdpdb_deregister_req(msg_p);

            break;
        }
#endif

        case MSG_ID_GOEP_DEREGISTER_SERVER_REQ:
        {
            goep_deregister_server_req_struct *msg_p = (goep_deregister_server_req_struct *)ext_message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_GOEP_DEREGISTER_SERVER_REQ");
            rdabt_goep_deregister_req(msg_p);

            break;
        }
        default :
            break;

    }
}

#endif /* _RDABT_OPP_C */

#endif

