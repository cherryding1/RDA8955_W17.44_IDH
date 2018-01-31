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



#include "bt.h"
#include "hci.h"
#include "rdabt_hcit.h"
//#include "rdabt_uart.h"
#include "papi.h"

typedef struct st_rdabt_host_buf_t rdabt_host_buf_t;

struct st_rdabt_host_buf_t
{
    /*IMPORTANT:
        The first four members of the structure should be data, transport, len, and totalsize
        in that order. The user-level struct st_t_dataBuf with only these members present.  */
    u_int8 *data;
    u_int8 transport;
    u_int16 len;

    u_int16 totalsize;
    u_int16 flags;
    u_int16 hci_handle_flags;
#if COMBINED_HOST==1
    void *qd;
#endif
#if pDEBUG==1
    /* the head and tail is for check bound when reserve/release header */
    u_int8 *head;
    u_int8 *tail;
#endif

#if L2CAP_FRAGMENT_ACL_SUPPORT==1
    /* linked list ability - flow control can use this */
    rdabt_host_buf_t *next;

    /* parent buffer and child counter */
    rdabt_host_buf_t *parent;
    u_int16 child_count;
#endif
};

enum
{
    HCI_UART_STATE_RX_TYPE,
    HCI_UART_STATE_RX_HEADER,
    HCI_UART_STATE_RX_DATA
};

#if 0
static char *transportTypes[] =
{
    "HCI Command",
    "ACL Data",
    "SCO Data",
    "HCI Event",
    "RS232 Error",
    "RS232 Negotiation"
};
#endif

#define HCI_SCO_MERGE_COUNT         3
static u_int8 hci_uart_rx_state;
static rdabt_host_buf_t *hci_uart_read_buffer;
static u_int16 hci_uart_pend_length;                  /* the length of data have received */
static u_int16 hci_uart_rx_length;                    /* the length of data need receive */
static u_int8* hci_uart_rx_buf;
static u_int8  hci_uart_rx_head_buf[4];
static u_int8  hci_uart_rx_type;
static const u_int8 hci_uart_head_length[] = {4,3,2};
u_int8 HciReset_Complete_Flag=0;

extern s_int16 *BTSco_GetRXBuffer(void);
extern s_int16 BTSco_ReceiveData(u_int32 length);
#ifdef __SUPPORT_BT_PHONE__
extern void rdabt_check_send_call_active(void);
#endif
u_int8 rdabt_hci_get_max_sco_size(void);
#define ACTIVE_PROFILE_HANDSFREE_AG       0x08
extern u_int8 rdabt_currentType ;
t_api rdabt_hcit_uart_init(void)
{
    hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;
    hci_uart_pend_length = 0;
    hci_uart_rx_length = 1;
    hci_uart_rx_buf = hci_uart_rx_head_buf;
    return RDABT_NOERROR;
}

t_api rdabt_hcit_uart_shutdown(void) 
{
    return RDABT_NOERROR;
}

void rdabt_uart_rx_data(    u_int8 *pdu,u_int32 length)
{
  // pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"rdabt_uart_rx_data begin %d , %d,%d",length,hci_uart_pend_length , hci_uart_rx_length ));

#if 1
    if(hci_uart_pend_length + length < hci_uart_rx_length)
    {
        if(hci_uart_rx_buf != NULL)
            pMemcpy(hci_uart_rx_buf + hci_uart_pend_length, pdu, length);
        hci_uart_pend_length += length;
        return;
    }
#endif

    while(hci_uart_pend_length + length >=  hci_uart_rx_length)
    {
        if(!hci_uart_pend_length&&!length&&!hci_uart_rx_length)
        {
            pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"!!!!!!!!00000!!!!!!!!!!!"));

            pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"!!!!!!!!00000!!!!!!!!!!!"));
            break;


        }
        if(hci_uart_rx_buf != NULL)
            pMemcpy(hci_uart_rx_buf + hci_uart_pend_length, pdu, hci_uart_rx_length - hci_uart_pend_length);

        pdu += hci_uart_rx_length - hci_uart_pend_length;
        length -= hci_uart_rx_length - hci_uart_pend_length;

        //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"HCI_Transport_Rx_Data hci_uart_rx_state=%d ",hci_uart_rx_state ));

        switch(hci_uart_rx_state)
        {
            case HCI_UART_STATE_RX_TYPE:
                hci_uart_rx_type = hci_uart_rx_buf[0];
                //   pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"Receive hci_uart_rx_typef %d\n",  hci_uart_rx_type));
                /* rx type error */
                if((hci_uart_rx_type < 2 || hci_uart_rx_type > 4)

#ifndef __SUPPORT_BT_PHONE__
                        ||(hci_uart_rx_type==3)
#endif
                  )
                {
                    //   pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"HCI_Transport_Rx_Data erro %d",hci_uart_rx_type ));

                    break;
                }
                hci_uart_rx_length = hci_uart_head_length[hci_uart_rx_type - 2];
                hci_uart_rx_buf = hci_uart_rx_head_buf;
                hci_uart_rx_state = HCI_UART_STATE_RX_HEADER;
                break;
            case HCI_UART_STATE_RX_HEADER:

                hci_uart_pend_length = hci_uart_rx_length;

                if(hci_uart_rx_type == 2) /* ACL data */
                {
                    hci_uart_rx_length = ((u_int16)hci_uart_rx_buf[3]<<8) |  hci_uart_rx_buf[2];
                    //  pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read 4 bytes of ACL Data PDU header: 0x%02X 0x%02X 0x%02X 0x%02X \n", hci_uart_rx_buf[0], hci_uart_rx_buf[1], hci_uart_rx_buf[2], hci_uart_rx_buf[3]));
                }
                else if(hci_uart_rx_type == 3) /* SCO data */
                {
                    hci_uart_rx_length =  hci_uart_rx_buf[2];
                    // pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"Read 3 bytes of SCO Data PDU header: 0x%02X 0x%02X 0x%02X \n", hci_uart_rx_buf[0], hci_uart_rx_buf[1], hci_uart_rx_buf[2]));
                }
                else  /* Event */
                {
                    hci_uart_rx_length =  hci_uart_rx_buf[1];
                    //  pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"Read 2 bytes of Event PDU header: 0x%02X 0x%02X\n", hci_uart_rx_buf[0], hci_uart_rx_buf[1]));
                }

                if(hci_uart_rx_type == 3) /* SCO data */
                {
#ifdef __SUPPORT_BT_PHONE__
                    hci_uart_rx_buf = BTSco_GetRXBuffer();
                    hci_uart_rx_state = HCI_UART_STATE_RX_DATA;
#else
                    rdabt_hcit_uart_init();
                    return;

#endif
                    break;
                }
                else
                {
                    /* allocate a buffer for the PDU */
                    if (RDABT_NOERROR!=HCI_GetReceiveBuffer((t_DataBuf**)&hci_uart_read_buffer, hci_uart_pend_length + hci_uart_rx_length))
                    {
                        pDebugPrintfEX((pLOGERROR,pLOGHCI,"Cannot allocate %d bytes of memory\n", hci_uart_pend_length + hci_uart_rx_length));
                        return;
                    }
                    pMemcpy(hci_uart_read_buffer->data, hci_uart_rx_buf, hci_uart_pend_length); /* copy header bytes into PDU buffer*/
                    hci_uart_read_buffer->len= hci_uart_pend_length;
                    hci_uart_rx_buf = hci_uart_read_buffer->data+ hci_uart_pend_length;
                    if (hci_uart_read_buffer != NULL)
                    {
                        hci_uart_rx_state = HCI_UART_STATE_RX_DATA;
                    }
                    else
                    {
                        hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;
                    }

                    if(hci_uart_rx_length != 0)
                    {
                        break;
                    }
                }


            /* for no payload, direct finish the packet */
            case HCI_UART_STATE_RX_DATA:
                //    pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"Read total %d bytes of hci_uart_rx_type =%d  \n", hci_uart_rx_length,hci_uart_rx_type));
                // send message to core

                if(hci_uart_rx_type == 3)
                {
#ifdef __SUPPORT_BT_PHONE__
                    rdabt_check_send_call_active();
                    BTSco_ReceiveData(hci_uart_rx_length);
#endif
                }
                else
                {
                    adp_data_msg_t data_msg;
                    //pMemcpy(hci_uart_read_buffer->data, hci_uart_rx_buf, hci_uart_pend_length); /* copy header bytes into PDU buffer*/

                    hci_uart_read_buffer->len+= hci_uart_rx_length;
                    //hci_uart_rx_buf = hci_uart_read_buffer->data+ hci_uart_pend_length;

                    hci_uart_read_buffer->transport=1;
                    data_msg.buff = hci_uart_read_buffer;
                    data_msg.type = hci_uart_rx_type;
                    // if(!num_temp)
                    {
                        //pDebugPrintfEX((pLOGDEBUG,pLOGHCI,"rdabt_uart_rx_data send ADP_DATA_IND,num_temp=%d,data_msg.type =0x%x,dataLen=0x%x,data[0~5]=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                        //  num_temp,data_msg.type, data_msg.buff->dataLen,data_msg.buff->buf[0],data_msg.buff->buf[1],data_msg.buff->buf[2],data_msg.buff->buf[3],data_msg.buff->buf[4],data_msg.buff->buf[5]));
                        //  if((hci_uart_rx_type == 2))//&&(data_msg.buff->dataLen==0x28))
                        {
                            u_int8 *buff=data_msg.buff->buf;
                            //pDebugPrintfEX((pLOGDEBUG,pLOGHFP,"ADP_DATA_IND data_msg.buff=0x%x,dataLen=%d,bufLen=%d -- hci_uart_read_buffer->len = %d",data_msg.buff,data_msg.buff->dataLen,data_msg.buff->bufLen,hci_uart_read_buffer->len));

                        }

                        if( hci_uart_rx_type==HCI_pduEVENT && data_msg.buff->buf[0]==HCI_evCOMMANDCOMPLETE && BT_READ_LE_UINT16(data_msg.buff->buf+3)==HCI_cmdRESET)
                            HciReset_Complete_Flag++;

                        //         pDebugPrintfEX((pLOGERROR,pLOGHCI,"HCI_UART_STATE_RX_DATA ---- %d \n", HciReset_Complete_Flag ));
                        if(HciReset_Complete_Flag)
                            RDABT_Send_Message(ADP_DATA_IND, RDABT_ADP, RDABT_HCI, sizeof(adp_data_msg_t), &data_msg);
                        else
                            HCI_FreeReceiveBuffer((t_DataBuf *)hci_uart_read_buffer,hci_uart_rx_type);
                        // num_temp++;
                    }
                    pWakeUpScheduler();
                }
                hci_uart_rx_state = HCI_UART_STATE_RX_TYPE;

                break;
        }

        if(hci_uart_rx_state == HCI_UART_STATE_RX_TYPE)
        {
            hci_uart_rx_buf = hci_uart_rx_head_buf;
            hci_uart_rx_length = 1;
        }
        hci_uart_pend_length = 0;
    }
    /** save the last data **/

    if((length > 0 && hci_uart_rx_buf != NULL)&&(hci_uart_rx_type!=3))
    {

        pMemcpy(hci_uart_rx_buf, pdu, length);

    }
    hci_uart_pend_length = length;
}

t_api rdabt_uart_tx_sco_data(u_int16 *pdu, u_int16 handle, u_int16 length)
{
    u_int8 pkt_head[4];
    u_int16 bytesWritten;

    //hal_HstSendEvent(SYS_EVENT,0x10280051);
    //hal_HstSendEvent(SYS_EVENT,count);
    //hal_HstSendEvent(SYS_EVENT,len);

    pkt_head[0] = 0x03;
    pkt_head[1] = handle&0xff;
    pkt_head[2] = (handle>>8)&0xff;
    pkt_head[3] = length;

    rdabt_adp_uart_tx(pkt_head, 4, &bytesWritten);
    rdabt_adp_uart_tx(pdu, length, &bytesWritten);

    //mmi_trace(g_sw_BT, " rdabt_uart_tx_sco_data return error ? %d",bytesWritten!=length);

    if(bytesWritten!=length)
        return RDABT_HCITERROR;
    else
        return RDABT_NOERROR;
}
t_api rdabt_uart_tx_pdu(t_DataBuf *pdu, u_int8 type, u_int16 flags, u_int16 hci_handle_flags)
{
    u_int16 bytesWritten;
    u_int8 *pkt_head;
    u_int16 pkt_head_len;
    u_int8  status=0;

    if (flags & RDABT_HOST_BUF_HCI_HEADER_PRESENT)
        pkt_head_len=1;
    else
        pkt_head_len=5;
    pkt_head = pdu->buf-pkt_head_len;

    pkt_head[0]=type;

    if (pkt_head_len==5)
    {
        pkt_head[1]=hci_handle_flags & 0xFF;
        pkt_head[2]=hci_handle_flags >> 8;
        pkt_head[3]=pdu->dataLen& 0xFF;
        pkt_head[4]=pdu->dataLen>> 8;
    }

    status= rdabt_adp_uart_tx(pdu->buf-pkt_head_len,pdu->dataLen+pkt_head_len, &bytesWritten);
    // if((pdu->dataLen>300)&&(type==HCI_pduACLDATA))
    // asm("break 1");
    if(!status)
        // pDebugPrintfEX((pLOGDEBUG,pLOGOBEX,"TX type= %d",pdu->dataLen));
//else
        pDebugPrintfEX((pLOGDEBUG,pLOGOBEX,"send_fail error error error error!!!!!!!!!"));

    HCI_FreeWriteBuffer(pdu, type);

    if (bytesWritten<pdu->dataLen+pkt_head_len)
    {
        return RDABT_HCITERROR;
    }
    else
        return RDABT_NOERROR;
}

t_api rdabt_uart_tx_data(u_int8 *pdu, u_int8 len)
{
    u_int16 bytesWritten;
    rdabt_adp_uart_tx(pdu,len, &bytesWritten);
    if (bytesWritten<len)
    {
        return RDABT_HCITERROR;
    }
    else
        return RDABT_NOERROR;
}


