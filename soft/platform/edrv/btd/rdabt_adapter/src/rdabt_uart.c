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



#include "kal_release.h"        /* basic data type */
#include "stack_common.h"          /* message and module ids */
#include "stack_msgs.h"            /* enum for message ids */
#include "app_ltlcom.h"         /* task message communiction */
//#include "syscomp_config.h"         /* type of system module components */
//#include "task_config.h"          /* task creation */
#include "stacklib.h"              /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* event scheduler */
#include "stack_timer.h"        /* stack timer */
#include "stdio.h"                  /* basic c i/o functions */
//#include "eint.h"                   /* external interrupt */

#include "uart_sw.h"               /* header file for uart */
#include "device_drv.h"
//#include "bt_hw_define.h"
#include "rdabt.h"
//#include "rdabt_platform.h"
#include "bt.h"
#include "base_prv.h"
//#define  UART_TEST
#include "mmi_trace.h"
#include "rdabt_main.h"

//#define BT_UART_PORT  1

#if pDEBUG

static const char* LOGLAYER[]=
{
    "pLOGSYSTEM",
    "pLOGHCI",
    "pLOGMANAGER",
    "pLOGL2CAP",
    "pLOGRFCOMM",
    "pLOGSDP",
    "pLOGOBEX",
    "pLOGTCS",
    "pLOGA2DP",
    "pALWAYS"
};

static const char* LOGLEVEL[]=
{
    "pLOGDATA",
    "pLOGFATAL",
    "pLOGCRITICAL",
    "pLOGERROR",
    "pLOGWARNING",
    "pLOGNOTICE",
    "pLOGDEBUG"
};
#endif
extern rdabt_context_struct rdabt_cntx;
void (*uart_rx_cb) (kal_uint8 *buff, kal_uint32 length);

extern kal_int32 EINT_SW_Debounce_Modify(kal_uint8 eintno, kal_uint8 debounce_time);
void rdabt_uart_unsleep(void);

extern u_int8 LogLayer_Encode(u_int8 loglayer);

#ifdef BT_UART_BREAK_INT_WAKEUP
extern VOID UART_Register_Wakeup_Cb(void (*cb)());
#endif

BOOL g_btdUartFlowCtrlEnabled;
UINT32 g_btdUartBaud;
/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_stop
* DESCRIPTION
*   Stop uart
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_stop(void)
{
    UART_Close(BT_UART_PORT);
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_start
* DESCRIPTION
*   Start UART for 5868
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_start(void)
{


    if (!UART_Open (BT_UART_PORT, MOD_BT))
//     asm("break 1");
        kal_prompt_trace(1, "UART open fail!!!!!\n");
}

/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_configure
* DESCRIPTION
*   Configure baud rate of UART
* PARAMETERS
*   baud    IN    baud rate of UART port
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_adp_uart_configure(unsigned baud,char flow_control)
{
    UARTDCBStruct dcb =
    {
        UART_BAUD_921600,    /* init */
        len_8,                 /* dataBits; */
        sb_1,                 /* stopBits; */
        pa_none,              /* parity; */
        fc_none,
        0x11,                 /* xonChar; */
        0x13,                 /* xoffChar; */
        KAL_FALSE
    };

    dcb.baud = (UART_baudrate)baud;

    if(flow_control)
        dcb.flowControl=fc_hw;
    else
        dcb.flowControl=fc_none;

    g_btdUartBaud = baud;
    g_btdUartFlowCtrlEnabled = flow_control;

    UART_SetDCBConfig(BT_UART_PORT, &dcb);
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_rx
* DESCRIPTION
*   Read Rx buffer in UART when receive READY_TO_READ_IND or DATA_TO_READ_IND
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
UINT32 total_rx_length=0;

volatile UINT32 total_realen=0;
volatile UINT32 total_uDatasize=0;

static UINT8 bt_uart_rx_buff[RDABT_RX_BUFF_SIZE];

kal_uint32 rdabt_adp_uart_rx(void)
{
    kal_uint32 rx_length;
    kal_uint8 status;
//   rdabt_uart_unsleep();
    rx_length = UART_GetBytes(BT_UART_PORT, bt_uart_rx_buff, RDABT_RX_BUFF_SIZE, &status, MOD_BT);
    total_rx_length += rx_length;
// if(rx_length >0)
    //kal_prompt_trace(0, "rdabt_adp_uart_rx rx_length: %d, total_rx_length=%d\n",rx_length,total_rx_length);

    if(rx_length >0)
        rdabt_uart_rx_data(bt_uart_rx_buff,rx_length);

    return rx_length;
}


/*****************************************************************************
* FUNCTION
*   rdabt_adp_uart_tx
* DESCRIPTION
*   Send data to Tx buffer
* PARAMETERS
*   buf            IN      buffer of data to be sent
*   num_to_send   IN       number of bytes to be sent
*   num_send      OUT     buffer to store actual size of data sent.
* RETURNS
*   TRUE if all bytes sent; otherwise, FALSE.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
unsigned char rdabt_adp_uart_tx(kal_uint8 *buf, unsigned short num_to_send, unsigned short *num_send)
{

    //rdabt_uart_unsleep();
    *num_send = (kal_uint16)UART_PutBytes(BT_UART_PORT, (kal_uint8*)buf, (kal_uint16)num_to_send, MOD_BT);

    //kal_prompt_trace(0, "num_to_send: %d, num_send: %d,data[0~3]=%x %x %x %x", num_to_send, *num_send,buf[0],buf[1],buf[2],buf[3]);

    if(*num_send == num_to_send)
        return KAL_TRUE;
    else
    {
        return KAL_FALSE;
    }
}


/*****************************************************************************
* FUNCTION
*   rdabt_host_wake_up_timeout
* DESCRIPTION
*   Timeout function for host wake up -- enable sleep mode again
* PARAMETERS
*   mi   IN  no use
*   mv  IN  no use
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
/*void rdabt_host_wake_up_timeout(void)
{
   if(rdabt_cntx.chip_knocked == KAL_FALSE)
   {

  //    L1SM_SleepEnable(rdabt_cntx.l1_handle);
      rdabt_cntx.host_wake_up = KAL_FALSE;
   }
   else
   {
      rdabt_cntx.chip_knocked = KAL_FALSE;
      rdabt_timed_event_in((RDABT_WAKE_UP_TIME+100)*1000, rdabt_host_wake_up_timeout, 0, NULL);
   }
    rdabt_send_notify();
}*/

/*****************************************************************************
* FUNCTION
*   rdabt_uart_unsleep
* DESCRIPTION
*   Disable UART sleep mode
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
/*void rdabt_uart_unsleep(void)
{
   if(!rdabt_cntx.host_wake_up)
   {

      L1SM_SleepDisable(rdabt_cntx.l1_handle);
      rdabt_cntx.host_wake_up = KAL_TRUE;
      rdabt_timed_event_in((RDABT_WAKE_UP_TIME+100)*1000, rdabt_host_wake_up_timeout, 0, NULL);
   }
}*/

/*****************************************************************************
* FUNCTION
*   rdabt_uart_eint_hdlr
* DESCRIPTION
*   Enternal internal handler
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/

void rdabt_uart_eint_hdlr(void)
{
    ilm_struct *msg;
    kal_prompt_trace(0, "rdabt_uart_eint_hdlr called");

    msg = allocate_ilm(MOD_UART3_HISR);
    msg->src_mod_id = MOD_UART3_HISR;
    msg->dest_mod_id = MOD_BT;
    msg->sap_id = INVALID_SAP;
    msg->msg_id = MSG_ID_BT_HOST_WAKE_UP_IND;
    msg->local_para_ptr = NULL;
    msg->peer_buff_ptr = NULL;
    // hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
    msg_send_ext_queue_to_head(msg);
}

/*****************************************************************************
* FUNCTION
*   rdabt_uart_host_wake_up
* DESCRIPTION
*   Handle of host wake up routine
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
/*void rdabt_uart_host_wake_up(void)
{
   if(!rdabt_cntx.host_wake_up)
   {

      L1SM_SleepDisable(rdabt_cntx.l1_handle);
      rdabt_cntx.host_wake_up = KAL_TRUE;
      rdabt_timed_event_in((RDABT_WAKE_UP_TIME+100)*1000, rdabt_host_wake_up_timeout, 0, NULL);
   }
   else
   {
      rdabt_cntx.chip_knocked = KAL_TRUE;
   }
}
*/
/*****************************************************************************
* FUNCTION
*   rdabt_uart_register_eint
* DESCRIPTION
*   Register external interrupt handler
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_uart_register_eint(void)
{
    //kal_uint8 bt_chan = custom_eint_get_channel(bt_eint_chann);
    //EINT_Registration(bt_chan, KAL_TRUE, RDABT_WAKE_UP_INT_LEVEL, rdabt_uart_eint_hdlr, KAL_TRUE);
    //GPIO_INT_Registration(1,0,rdabt_uart_eint_hdlr);
    //EINT_SW_Debounce_Modify(bt_chan,4);
}

/*****************************************************************************
* FUNCTION
*   rdabt_uart_register_eint
* DESCRIPTION
*   Register external interrupt handler
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
/*void rdabt_uart_deregister_eint(void)
{
   kal_uint8 bt_chan = custom_eint_get_channel(bt_eint_chann);
   EINT_Mask(bt_chan);
}
*/
/*****************************************************************************
* FUNCTION
*   RDA5868_uart_rx_cb
* DESCRIPTION
*   Callback function when RX interrupt comes
* PARAMETERS
*   port    IN UART power number
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
extern volatile int8  rdabt_recive_msg_count;

void rdabt_uart_rx_cb(UART_PORT port)
{
    TASK_HANDLE * hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);
    //mmi_trace(g_sw_BT,"rdabt_uart_rx_cb called1");

    //if(!COS_IsEventAvailable(hTask))
    {
        ilm_struct *msg;

        //mmi_trace(g_sw_BT,"rdabt_uart_rx_cb called2");
        rdabt_recive_msg_count++;
        msg = allocate_ilm(MOD_UART3_HISR);
        msg->src_mod_id = MOD_UART3_HISR;
        msg->dest_mod_id = MOD_BT;
        msg->sap_id = INVALID_SAP;
        msg->msg_id = MSG_ID_UART_READY_TO_READ_IND;
        msg->local_para_ptr = NULL;
        msg->peer_buff_ptr = NULL;
        msg_send_ext_queue_to_head(msg);
    }
}



typedef void (*UART_RX_CB_FUNC)(kal_uint8 *buff,kal_uint32 len) ;


void rdabt_uart_register_cb(void)
{

#ifdef BT_UART_BREAK_INT_WAKEUP
    UART_Register_Wakeup_Cb(rdabt_uart_eint_hdlr);
#endif

    UART_Register_RX_cb(BT_UART_PORT, MOD_BT, rdabt_uart_rx_cb);

}

/*kal_uint8 LogLayer_Encode(kal_uint8 loglayer)
{
    kal_uint8 i=0;
    if(loglayer==0)
        return 0;//error
    while(loglayer)
    {
         loglayer=loglayer>>1;
         i++;
    }
    return (i-1);

}*/
//#define __RDABT_INITALL_BY_IIC__ 1

void rdabt_wirte_memory(unsigned int addr,const unsigned int *data,unsigned char len,unsigned char memory_type)
{
#ifdef __RDABT_INITALL_BY_IIC__
    if(memory_type == 0x00)
    {
        rdabt_iic_core_write_data(addr,data,len);
    }
    else
#endif
    {
        unsigned short num_send,num_to_send;
        unsigned char i;
        unsigned char data_to_send[256];
        unsigned int address_convert;
        data_to_send[0] = 0x01;
        data_to_send[1] = 0x02;
        data_to_send[2] = 0xfd;
        data_to_send[3] = len*4+6;
        data_to_send[4] = memory_type;
        if(memory_type == 0x10)
            data_to_send[4] = 0x00;//        data_to_send[4] = 0x80;
        // else
        // data_to_send[4] = 0x81;
        data_to_send[5] = len;
        if(memory_type == 0x01)
        {
            address_convert = addr*4+0x200;
            data_to_send[6] = address_convert;
            data_to_send[7] = address_convert>>8;
            data_to_send[8] = address_convert>>16;
            data_to_send[9] = address_convert>>24;
        }
        else
        {
            data_to_send[6] = addr;
            data_to_send[7] = addr>>8;
            data_to_send[8] = addr>>16;
            data_to_send[9] = addr>>24;
        }
        for(i=0; i<len; i++,data++)
        {
            unsigned char j=10+i*4;
            data_to_send[j] = *data;
            data_to_send[j+1] = *data>>8;
            data_to_send[j+2] = *data>>16;
            data_to_send[j+3] = *data>>24;
        }
        num_to_send = 4+data_to_send[3];
        rdabt_adp_uart_tx(data_to_send,num_to_send,&num_send);
    }
}

void rdabt_write_pskey(unsigned char id, const unsigned int *data,unsigned char len)
{
    //  EDRV_TRACE(EDRV_BTD_TRC, 0, "rdabt_write_pskey id=%x, len=%d",id,len);
    {
        unsigned short num_send,num_to_send;
        unsigned char i;
        unsigned char data_to_send[20];
        unsigned int address_convert;
        data_to_send[0] = 0x01;
        data_to_send[1] = 0x05;
        data_to_send[2] = 0xfd;
        data_to_send[3] = len+1;
        data_to_send[4] = id;

        num_to_send = 5;
        rdabt_adp_uart_tx(data_to_send,num_to_send,&num_send);
//    RDABT_DELAY(2);
        sxr_Sleep(16384/100);
        num_to_send = len;
        rdabt_adp_uart_tx(data,num_to_send,&num_send);
    }
}

