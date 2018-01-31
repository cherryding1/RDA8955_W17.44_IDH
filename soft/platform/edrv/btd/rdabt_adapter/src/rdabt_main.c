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

#include "stacklib.h"              /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* event scheduler */
#include "stack_timer.h"        /* stack timer */
#include "gpio_sw.h"               /* for gpio */
#include "uart_sw.h"
#include "bluetooth_bm_struct.h"
#include "bluetooth_struct.h"

#include "bt_a2dp_struct.h"
#include "rda_bt_avrcp.h"
#include "bt_dm_struct.h"
#include "bt_hfg_struct.h"
#include "bt_avrcp_struct.h"

#include "bt.h"
#include "rdabt.h"
#include "rdabt_opp.h"
#include "rdabt_main.h"
#include "bt_ctrl.h"
#include "rdabt_task.h"
#include "rdabt_profile.h"
#include "rdabt_link_prim.h"
#include "manager.h"
#include "rdabt_mgr.h"


#include "mmi_trace.h"
#include "papi.h"
#include "sxr_mem.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "adp_events.h"
#include "device_drv.h"
#include "rdabt_hcit.h"


#ifdef __BT_HFG_PROFILE__
#include "rdabt_hfp.h"
#include "rdabt_hsp.h"
#endif
#ifdef  __BT_OPP_PROFILE__
#include "rdabt_opp.h"
#endif
#ifdef  __BT_FTS_PROFILE__
#endif
#ifdef  __BT_FTC_PROFILE__
#endif
#ifdef  __BT_A2DP_PROFILE__
#include "rdabt_a2dp.h"
#endif
#ifdef  __BT_AVRCP_PROFILE__
#include "rdabt_avrcp.h"
#endif
typedef UINT32 MsgBody_t;


#include "rdabt_uart.h"
#include "bt_ctrl.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "itf_msg.h"
#include "btd_config.h"
#include "tgt_btd_cfg.h"
#include "pmd_m.h"
#include "hal_gpio.h"
#define RDABT_WAKE_UP_TIME                          50      /* host wake up time 5s */

U8 rda_wakeup_timer = 0;

#ifdef AP525_TEST_ON
extern t_pTimer ap525_connect_timer;
#endif

rdabt_context_struct rdabt_cntx;
rdabt_context_struct* rdabt_p = &rdabt_cntx;
extern const unsigned char bt_gpio_setting[16];

//u_int16 rdabt_btNap = 0x0068;
//u_int8 rdabt_btUap = 0x58;
//u_int32 rdabt_btLap = 0x000006;
u_int8 Bt_init_Finished =0;
u_int8  rdabt_send_notify_pending = 0;

CONST TGT_BTD_CONFIG_T* g_btdConfig;

volatile int8  rdabt_recive_msg_count=0;

const kal_uint8 rdabt_wakeup_data[] = {0x01,0xc0,0xfc,0x00};
kal_uint8 init_flag[] = {0x01,0x02,0xfd,0x0a,0x04,0x01,0x58,0x68,0x00,0x80,0x02,0x00,0x00,0x00}; // for rda5872
static const kal_uint8 rdabt_nable_sleep_FF[3]= {0XFF,0XFF,0XFF};

extern kal_uint32 rand_num_seed;
extern void Uart1_GetStatus();
extern         pBOOL isInCall(void);
extern BOOL g_btdUartFlowCtrlEnabled;
extern UINT32 g_btdUartBaud;
extern int rdabt_poweron_init(void);
extern void rdabt_baudrate_ctrl(void);
extern void rdabt_poweronphone_init(void);
static BOOL IsInLpMode;
 BOOL IsFirst_PowerOn=TRUE;
extern  void rdabt_set_ignore_uart_rx(u_int8 flag);
void rdabt_antenna_off(void);
VOID pal_EnableAuxClkCtrl(BOOL on);

static const bt_config_t host_config =
{
    //.host_ctrl_config
    {
        20,//.max_timers
        20,//.max_buff
        50,//.max_message
        10240,//.message_length
        0,//
        256,//host_mem_size
    },
    //.hci_config
    {
        1,//flow_control_support
        3, //flow_control_timeout
        5,  //flow_control_threshold
        10,//max_acl_number
        1021, //max_acl_size
        10,//max_sco_number
        60, //   max_sco_size
        15360*2  ,//max_acl_queue_size
    },
    //.manager_config

    {
        7,//max_acl_links???????????
        50,//max_device_list
        1,//multi_connect_support
#ifdef __SUPPORT_BT_PHONE__
        0x7e041c,
#else
        0x5a0204,//0x7c2204,//device_class  0x5a0204->0x7a0204  phone ->btphone
#endif
        1,//security_support
        0x0c,//sco_packets  0x04->0X0C
#if 0//def __SUPPORT_BT_PHONE__
        0x8060,//0x8060,//voice_setting
#else
#ifdef USE_VOC_CVSD
			0xc040,//
#else
        0x0060,//0x8060,//voice_setting
#endif
#endif
        0,//sco_via_hci
#ifdef __MMI_BT_SIMPLE_PAIR__
        1,
#else
        0,//ssp_support
#endif
        1,//io_capability
        100,//sniff_timeout
        0x320,//sniff_interval
        3,//unsniff_packets
    },
    //.l2cap_config
    {
        1,    //auto_config
        1,//fragment_acl_support
        0,   //config_to_core
        0x30,  //max_channel
        0x06, //   max_links
        1021, //  max_in_mtu
        1021,  //  max_out_mtu
        0,//use_piconet_broadcast
        0x04ED,//max_broadcast_mtu
        0xef1, //broadcast_handle
        0//RDABT_L2CAP_EXTENDED_FEATURE_FLOWCONTROL_MODE|RDABT_L2CAP_EXTENDED_FEATURE_RETRANSMISSION_MODE,
    },
    //sdp_config
    {
        0x80,//mtu,
    },
    // obex_config
    {
        0x3800, // local_mtu
        3,      // server_transport
    },
    //.rfcomm_config
    {
        7,//init_credit
        255,//max_credit
        3,//min_credit
        7,//credit_issue
        990,//329,//128, // max_frame_size
    },
    //.hfp_config
    {
        0x7f,//.hf_feature
        0x1e3,//0x1FF,//.ag_feature
        0,// .handle_at_cmd
    },
    //.a2dp_config
    {
        672,//.media_mtu
        48,//.signal_mtu
#ifdef __BT_PROFILE_BQB__    //sy
        1,//.accetp_connect
#else
        0,// .accetp_connect
#endif
    },
    // spp_config
    {
        6, // max_ports
        2, // server_num
#ifdef __SUPPORT_SPP_SYNC__
        329, // max_data_len
#else
        128, // max_data_len
#endif
    },
};



/*****************************************************************************
* FUNCTION
*   rdabt_send_msg_up
* DESCRIPTION
*   Send a message to upper layer
* PARAMETERS
*   dest                IN    destination module ID
*   msg_id             IN    ID of the message
*   local_param_ptr      IN    pointer to local parameter
*   peer_buf_ptr        IN    pointer to peer buffer
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_send_msg_up(kal_uint16 dest, kal_uint32 msg_id, void *local_param_ptr, void *peer_buf_ptr)
{
    ilm_struct *msg = allocate_ilm(MOD_BT);
    msg->src_mod_id = MOD_BT;
    msg->dest_mod_id = (module_type)dest;
    msg->sap_id = BT_APP_SAP;
    msg->msg_id = msg_id;
    msg->local_para_ptr = local_param_ptr;
    msg->peer_buff_ptr = peer_buf_ptr;
    rda_msg_send_ext_queue(msg);
}







#include "hal_uart.h"
#include "hal_gpio.h"




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
void rdabt_host_wake_up_timeout(void *dummy)
{
//kal_prompt_trace(1,"rdabt_host_wake_up_timeout rdabt_cntx.chip_knocked=%d",rdabt_cntx.chip_knocked);

    if(rdabt_cntx.chip_knocked == FALSE)
    {
        /* disable sleep mode */
//      L1SM_SleepEnable(rdabt_cntx.l1_handle);
        //    hal_UartAllowSleep(TRANSPORT_UART_ID,TRUE);
        //enter sleep mode
        rdabt_EnterLpMode();
        rdabt_cntx.host_wake_up = FALSE;
    }
    else
    {
        rdabt_cntx.chip_knocked = FALSE;
        sxr_StartFunctionTimer(5*16384, rdabt_host_wake_up_timeout, 0,0);

        //rda_wakeup_timer = rdabt_timer_create(RDABT_WAKE_UP_TIME, rdabt_host_wake_up_timeout, NULL,pTIMER_ONESHOT);
    }
    // rdabt_send_notify();
}

/*****************************************************************************
* FUNCTION
*   rdabt_EnterLpMode
* DESCRIPTION
*   close uart enable bb sleep
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
extern DEVICE Uart_Dev[];

VOID rdabt_EnterLpMode(VOID)
{
	DEVICE *pDev=&Uart_Dev[PORT1];
	kal_prompt_trace(1,"0913rdabt_EnterLpMode");

	UINT32 scStatus = hal_SysEnterCriticalSection();
	if(pDev->port)
		hal_UartClose(pDev->port);
	hal_SysExitCriticalSection(scStatus);

}
/*****************************************************************************
* FUNCTION
*   rdabt_ExitLpMode
* DESCRIPTION
*   reconfigure uart and wakeup bt chip
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
VOID rdabt_ExitLpMode(VOID)
{
	unsigned short bytesWritten;
	kal_prompt_trace(1,"rdabt_ExitLpMode");

	rdabt_adp_uart_configure(g_btdUartBaud,g_btdUartFlowCtrlEnabled);
	sxr_Sleep(210);
	rdabt_adp_uart_tx((u_int8 *)rdabt_nable_sleep_FF,sizeof(rdabt_nable_sleep_FF),&bytesWritten);
	sxr_Sleep(210);
}

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
void rdabt_uart_unsleep(void)
{
//kal_prompt_trace(1,"rdabt_uart_unsleep rdabt_cntx.host_wake_up=%d",rdabt_cntx.host_wake_up);
  if(rdabt_p->state==RDABT_STATE_POWER_OFF )
    return;
    if(!rdabt_cntx.host_wake_up)
    {
        /* disable sleep mode */
//      hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
//exit sleep mode
        rdabt_ExitLpMode();
        rdabt_cntx.host_wake_up = TRUE;
        sxr_StartFunctionTimer(5*16384, rdabt_host_wake_up_timeout, 0,0);
    }
    else
    {
        rdabt_cntx.chip_knocked = TRUE;
    }
}
extern u_int8 HciReset_Complete_Flag;
void BT_Register_Modules(void)
{
    // base module
    RDABT_AddModule(RDABT_ADP, NULL, rdabt_adp_msg_dispatch);
    RDABT_AddModule(RDABT_HCI, rdabt_hci_layer_ctrl, rdabt_hci_msg_dispatch);
    RDABT_AddModule(RDABT_MANAGER, rdabt_manager_layer_ctrl, rdabt_manager_msg_dispatch);
    RDABT_AddModule(RDABT_L2CAP, rdabt_l2cap_layer_ctrl, rdabt_l2cap_msg_dispatch);
    // modules for 3.0
    //RDABT_AddModule(RDABT_PAL, rdabt_pal_layer_ctrl, rdabt_pal_msg_dispatch);
    //RDABT_AddModule(RDABT_AMP, rdabt_amp_layer_ctrl, rdabt_amp_msg_dispatch);

    // stack protocol
    RDABT_AddModule(RDABT_SDP_CLI, rdabt_sdp_cli_layer_ctrl, rdabt_sdp_cli_msg_dispatch);
    RDABT_AddModule(RDABT_SDP_SRV, rdabt_sdp_srv_layer_ctrl, rdabt_sdp_srv_msg_dispatch);
    RDABT_AddModule(RDABT_RFCOMM, rdabt_rfcomm_layer_ctrl, rdabt_rfcomm_msg_dispatch);
    RDABT_AddModule(RDABT_OBEX_CLI, rdabt_OBEX_CLI_layer_ctrl, rdabt_OBEX_CLI_msg_dispatch);
    RDABT_AddModule(RDABT_OBEX_SRV, rdabt_OBEX_SRV_layer_ctrl, rdabt_OBEX_SRV_msg_dispatch);

    // profiles
#ifndef __BT_NO_A2DP_PROFILE__
    RDABT_AddModule(RDABT_A2DP, rdabt_avdtp_layer_ctrl, rdabt_avdtp_msg_dispatch);
    RDABT_AddModule(RDABT_AVRCP, rdabt_avctp_layer_ctrl, rdabt_avctp_msg_dispatch);
#endif

    RDABT_AddModule(RDABT_HFP, rdabt_hfp_layer_ctrl, rdabt_hfp_msg_dispatch);
    //RDABT_AddModule(RDABT_HID, rdabt_hid_layer_ctrl, rdabt_hid_msg_dispatch);
    // #ifdef __SUPPORT_SPP_SYNC__
    RDABT_AddModule(RDABT_SPP, rdabt_spp_layer_ctrl, rdabt_spp_msg_dispatch);
    //#endif
    //RDABT_AddModule(RDABT_HCRP, rdabt_hcrp_layer_ctrl, rdabt_hcrp_msg_dispatch);
}

int  rdabt_antenna_on()
{
    kal_uint16 num_send;
    HciReset_Complete_Flag=0;
    mmi_trace(g_sw_BT,"Enter function  rdabt_antenna_on \n");

    pal_EnableAuxClkCtrl(TRUE);
    // Enable 26M AUX clock
    hal_SysAuxClkOut(TRUE);
    hal_GpioClr(g_btdConfig->pinReset);
    pmd_EnablePower(PMD_POWER_BT, TRUE);
    sxr_Sleep(168*50);/*10 ms*/
    hal_GpioSet(g_btdConfig->pinReset);
	RDABT_Tranport_Init();
    rdabt_set_ignore_uart_rx(TRUE);
    RDABT_Initialise(&host_config);
   BT_Register_Modules();
   rdabt_cntx.chip_knocked = TRUE;
   rdabt_cntx.host_wake_up = TRUE;
    rdabt_adp_uart_stop(); 
    rdabt_adp_uart_start();
    I2C_Open();
    rdabt_adp_uart_configure(115200,FALSE);
#ifndef BT_UART_BREAK_INT_WAKEUP
    GPIO_INT_Registration(g_btdConfig->pinSleep,0,rdabt_uart_eint_hdlr);
#endif
    rdabt_uart_register_cb(); 
    rdabt_p->timer_id= pTimerCreate_base(100,0);
	sxr_StartFunctionTimer(5*16384, rdabt_host_wake_up_timeout, 0,0);  
    if( rdabt_poweron_init() == -1 )
    {
        hal_GpioClr(g_btdConfig->pinReset);
        hal_GpioClr(g_btdConfig->pinSleep);
        pmd_EnablePower(PMD_POWER_BT, FALSE);
        rdabt_adp_uart_stop();
        I2C_Close();
        // Disable 26M AUX clock
        hal_SysAuxClkOut(FALSE);
        pal_EnableAuxClkCtrl(FALSE);
        return -1;
    }
    rdabt_baudrate_ctrl();
    sxr_Sleep(10);  
	RDABT_Add_Transport(TRANSPORT_UART_ID, RDABT_CORE_TYPE_BR_EDR);
    rdabt_adp_uart_tx(rdabt_nable_sleep_FF,sizeof(rdabt_nable_sleep_FF),&num_send);
    rdabt_send_notify_pending=0;
    I2C_Close();
    // Disable 26M AUX clock
    hal_SysAuxClkOut(FALSE);

    return 0;

}


void rdabt_antenna_off(void)
{
    kal_prompt_trace(1,"Enter function  rdabt_antenna_off \n");
 //   RDABT_Shut_Down();
  //  RDABT_Terminate(0);
  //  sxr_Sleep(160);
    hal_GpioClr(g_btdConfig->pinReset);
    pmd_EnablePower(PMD_POWER_BT, FALSE);
    rdabt_adp_uart_stop();
    pal_EnableAuxClkCtrl(FALSE);
    rdabt_p->host_wake_up = KAL_FALSE;
    rdabt_p->chip_knocked = KAL_FALSE;
    rdabt_p->state = RDABT_STATE_POWER_OFF;
    if(rdabt_p->timer_id)
        pTimerCancel_base(rdabt_p->timer_id);
    memset(rdabt_p,0,sizeof(rdabt_cntx));
    rdabt_send_notify_pending=0;
    Bt_init_Finished=0;
    sxr_StopFunctionTimer(rdabt_host_wake_up_timeout);

#ifdef AP525_TEST_ON
    if (ap525_connect_timer)
    {
        rdabt_cancel_timer(ap525_connect_timer);
        ap525_connect_timer = 0;
    }
#endif

}

void rdabt_antenna_off1(void)
{
#if defined(__RDA_CHIP_R12_5990__)
    //   ilm_struct msg;
    //   kal_uint32 msg_count;
    mmi_trace(g_sw_BT,"Enter function  rdabt_antenna_off \n");

    if(rda_on_off == TRUE)
    {
        rda_on_off = FALSE;
    }
    else
    {
        return;
    }
    RDABT_Shut_Down();
    RDABT_Terminate(0);
    sxr_Sleep(160);

    hal_GpioClr(g_btdConfig->pinReset);
    //  hal_GpioClr(g_btdConfig->pinSleep);
#if defined(__RDA_CHIP_R16_5876P__) || defined(__RDA_CHIP_R12_5876__)
    if(g_FMOpen_Flag == FALSE)
    {
        pmd_EnablePower(PMD_POWER_BT, FALSE);
    }
    else
    {
        //FM on
        RDABT_5876p_btoff_fmon_patch();
    }
    g_BTOpen_Flag = FALSE;
#else
    pmd_EnablePower(PMD_POWER_BT, FALSE);
#endif
    hal_HstSendEvent(0xdddccc11);

    RDA_5990_power_off();

    rda5868_uart_stop();
    pal_EnableAuxClkCtrl(FALSE);

    /* reset global context */
    rdabt_p->seq_num = 0;
    rdabt_p->num_int_msg = 0;
    rdabt_p->rx_cursor = 0;
    rdabt_p->rx_length = 0;
    rdabt_p->rx_length = 0;
    //rdabt_p->curr_queue = 0;
    rdabt_p->sent_rx_ind = KAL_FALSE;
    rdabt_p->sent_tx_ind = KAL_FALSE;

    rdabt_p->host_wake_up = KAL_FALSE;
    rdabt_p->chip_knocked = KAL_FALSE;

    // rdabt_mem_deinit();

    rdabt_p->state = RDABT_STATE_POWER_OFF;
#endif
}


/*****************************************************************************
* FUNCTION
*   rdabt_is_bd_addr_valid
* DESCRIPTION
*   Check if BD Address is valid
* PARAMETERS
*   bd_addr    IN    address to be checked
* RETURNS
*   TRUE if the address is valid; otherwise, FALSE.
* GLOBALS AFFECTED
*   None
*****************************************************************************/
kal_bool rdabt_is_bd_addr_valid(rdabt_bd_addr_struct *bd_addr)
{
    if(bd_addr->LAP == 0 && bd_addr->UAP == 0 && bd_addr->UAP == 0)
        return FALSE;
    else if(bd_addr->LAP>=0x9E8B00 && bd_addr->LAP<= 0x9E8B3F)
        return FALSE;
    else if(bd_addr->LAP>0xFFFFFF)
        return FALSE;
    else
        return TRUE;
}

#ifndef __COOLSAND_TARGET__
U32 rdabt_get_random_number(void)
{
    U32 temp;
    temp = hal_TimGetUpTime() & 0x003f;
    temp ^= ( (hal_TimGetUpTime() & 0x003f) << ((hal_TimGetUpTime())&0x001f));
    return temp;
}
#endif
#ifdef MMI_ON_HARDWARE_P
#include "tgt_m.h"
#define     BT_ACTIVED_FLAG 0x1d0e
BOOL bt_LoadDeviceAddrInfo(t_bdaddr *addr)
{
    TGT_BT_INFO_T btInfo;
    rdabt_bd_addr_struct bd_addr_str;
    t_bdaddr g_PairAddr;

    tgt_GetBtInfo(&btInfo);
    memcpy(g_PairAddr.bytes,btInfo.dev_addr,FACT_MAC_LEN);

    bd_addr_str.LAP= BDADDR_Get_LAP(&g_PairAddr);
    bd_addr_str.UAP= BDADDR_Get_UAP(&g_PairAddr);
    bd_addr_str.NAP = BDADDR_Get_NAP(&g_PairAddr);
    kal_prompt_trace(1,"bt_LoadDeviceAddrInfo 0 pFS->btInfo.activated=%x",btInfo.activated);
    if ((btInfo.activated == BT_ACTIVED_FLAG)&&(rdabt_is_bd_addr_valid(&bd_addr_str)))//read from factory
    {
        mmi_trace(g_sw_BT,"bt_LoadDeviceAddrInfo 1");
        memcpy(addr->bytes,btInfo.dev_addr,FACT_MAC_LEN);
    }
    else
    {
        kal_prompt_trace(1,"bt_LoadDeviceAddrInfo 2");
        srand(rdabt_get_random_number());
        while(1)
        {
            bd_addr_str.LAP= rand()&0xFFFFFF;
            if(rdabt_is_bd_addr_valid(&bd_addr_str))
                break;
        }
        while(1)
        {
            bd_addr_str.UAP= rand()&0xFF;
            if(rdabt_is_bd_addr_valid(&bd_addr_str))
                break;
        }
        while(1)
        {
            bd_addr_str.NAP= rand()&0xFFFF;
            if(rdabt_is_bd_addr_valid(&bd_addr_str))
                break;
        }
        //  factorySettings = *pFS;
        BDADDR_Set_LAP_UAP_NAP(&g_PairAddr,bd_addr_str.LAP, bd_addr_str.UAP, bd_addr_str.NAP);

        btInfo.activated = BT_ACTIVED_FLAG;
        memcpy(btInfo.dev_addr,g_PairAddr.bytes,FACT_MAC_LEN);

        tgt_SetBtInfo(&btInfo);//write to factoy.
        memcpy(addr->bytes,g_PairAddr.bytes,FACT_MAC_LEN);

    }
    kal_prompt_trace(1,"bt_LoadDeviceAddrInfo addr.bytes=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                     addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
}

void bt_GetDeviceAddr(U16* nap,U8* uap,U32* lap )
{
    t_bdaddr bt_addr;
    bt_LoadDeviceAddrInfo(&bt_addr);
    *nap = BDADDR_Get_NAP(&bt_addr);
    *uap = BDADDR_Get_UAP(&bt_addr);
    *lap = BDADDR_Get_LAP(&bt_addr);
    kal_prompt_trace(1,"rdabt_get_local_addr_req addr.bytes=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                     bt_addr.bytes[0], bt_addr.bytes[1], bt_addr.bytes[2], bt_addr.bytes[3], bt_addr.bytes[4], bt_addr.bytes[5]);
}


/*****************************************************************************
* FUNCTION
*   rdabt_get_db_address
* DESCRIPTION
*   Get previous-saved BD address to NVRAM
* PARAMETERS
*   None
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_get_local_addr_req(kal_msgqid queue_id)

{
    t_bdaddr bt_addr;
    rdabt_bd_addr_struct bt_addr_st;
    bt_bm_read_local_addr_cnf_struct *param_ptr = (bt_bm_read_local_addr_cnf_struct *)construct_local_para(
                (kal_uint16)sizeof(bt_bm_read_local_addr_cnf_struct), TD_CTRL);
    kal_prompt_trace(1,"rdabt_get_local_addr_req");
    bt_LoadDeviceAddrInfo(&bt_addr);
    kal_prompt_trace(1,"rdabt_get_local_addr_req addr.bytes=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                     bt_addr.bytes[0], bt_addr.bytes[1], bt_addr.bytes[2], bt_addr.bytes[3], bt_addr.bytes[4], bt_addr.bytes[5]);
    MGR_WriteLocalDeviceAddress(bt_addr);

    bt_addr_st.LAP= BDADDR_Get_LAP(&bt_addr);
    bt_addr_st.UAP = BDADDR_Get_UAP(&bt_addr);
    bt_addr_st.NAP = BDADDR_Get_NAP(&bt_addr);
    param_ptr->result = 0;
    param_ptr ->bd_addr.lap=  bt_addr_st.LAP;
    param_ptr ->bd_addr.uap=  bt_addr_st.UAP;

    param_ptr ->bd_addr.nap=  bt_addr_st.NAP;
    kal_prompt_trace(1,"rdabt_get_local_addr_req addr.LAP UAP NAP=0x%x 0x%x 0x%x ",  bt_addr_st.LAP,bt_addr_st.UAP,bt_addr_st.NAP);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF, param_ptr, NULL);

}

void rdabt_send_notify(void)
{
    TASK_HANDLE * hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);
    if(!COS_IsEventAvailable(hTask))
    {
        //rdabt_send_msg_up(MOD_BT, MSG_ID_BT_NOTIFY_EVM_IND, NULL, NULL);
        ilm_struct *msg = allocate_ilm(MOD_BT);
        msg->src_mod_id = MOD_BT;
        msg->dest_mod_id = (module_type)MOD_BT;
        msg->sap_id = BT_APP_SAP;
        msg->msg_id = MSG_ID_BT_NOTIFY_EVM_IND;
        msg->local_para_ptr = NULL;
        msg->peer_buff_ptr = NULL;
        msg_send_ext_queue(msg);
        //   rdabt_timed_event_in((100)*1000, rdabt_send_notify,NULL);
        rdabt_send_notify_pending++;
    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_init
* DESCRIPTION
*   Init function if bt task
* PARAMETERS
*   task_index  IN   index of the taks
* RETURNS
*   TRUE if reset successfully
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
kal_bool rdabt_init(task_indx_type task_index)
{
    mmi_trace(g_sw_BT,"rdabt_init");
    RDABT_Initialise(&host_config);
    return KAL_TRUE;
}


BOOL  reset_in_process=0;

/*****************************************************************************
* FUNCTION
*   rdabt_power_off_sched
* DESCRIPTION
*   Main loop when in power off state
* PARAMETERS
*  queue_id               IN     queue index of bt
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_power_off_sched(kal_msgqid queue_id)
{
    ilm_struct *msg=NULL;
    COS_EVENT ev;
    TASK_HANDLE * hTask=NULL;
    Msg_t *BtMsg=NULL;
    hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);
    while(1) /* main root for message processing */
    {
        SUL_ZeroMemory32(&ev, sizeof(COS_EVENT) );
        BtMsg = sxr_Wait((u32 *)&ev, COS_TaskMailBoxId(hTask));
        if (BtMsg!= NULL )
        {

            asm("break 1");
        }
        else
        {
            if (ev.nEventId == BT_TIMER_PAPI)
            {

                asm("break 1");
            }
            else
            {
                msg = (ilm_struct*)ev.nParam1;

                if(msg == NULL) {mmi_trace(0,"bt received a NULL msg"); return ;}
                mmi_trace(g_sw_BT,"rdabt_power_off_sched Get a Message ,msg id = %d\n",msg->msg_id);

                switch(msg->msg_id)
                {
                    case MSG_ID_BT_POWEROFF_REQ:
                    {
                        //rdabt_antenna_off();
                        RDABT_Shut_Down();
                        mmi_trace(g_sw_BT,"rdabt_main-msg_hdler process MSG_ID_BT_MMI_RESET_REQ");
                        break;
                    }
                    case MSG_ID_BT_MMI_RESET_REQ:
                    {
                        //  rdabt_antenna_off();
                        mmi_trace(g_sw_BT,"rdabt_main-msg_hdler process MSG_ID_BT_MMI_RESET_REQ");
                        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MMI_RESET_CNF, NULL, NULL);
                        break;
                    }
                    case MSG_ID_BT_POWERON_REQ:
                    {
                        reset_in_process = 0;
                        bt_bm_PowerON_cnf_struct *cnf_param_ptr;
                        cnf_param_ptr = (bt_bm_PowerON_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_bm_PowerON_cnf_struct), TD_CTRL);
                        IsFirst_PowerOn=TRUE;
                        if(rdabt_antenna_on())
                        {
                            cnf_param_ptr->result = FALSE;
                        }
                        else
                        {
                            cnf_param_ptr->result = TRUE;
                        }

                        /* ====================== state change ====================== */
                        /* send response back to MMI */

                        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_POWERON_CNF, cnf_param_ptr, NULL);

                        MMI_Free_msg_buf(msg);

                        if(cnf_param_ptr->result)
                        {
                            rdabt_p->state = RDABT_STATE_POWER_ON;
                            rdabt_p->original_boot_state = RDABT_STATE_POWER_ON;
                        }

                        return;
                    }
                    break;
                    case MSG_ID_BT_NOTIFY_EVM_IND:
                    {
                        if(rdabt_send_notify_pending)
                            rdabt_send_notify_pending--;
                    }
                    break;
                    default :
                        mmi_trace(g_sw_BT,"rdabt_power_off_sched recived msg id = %d\n",msg->msg_id);
                        break;
                } /* end of switch */
                MMI_Free_msg_buf(msg);
            }
        }
    }  /* end of while(1) */
}

extern void rdabt_goep_init(void) ;
void rdabt_bt_restart()
{
    reset_in_process = 1;
    rdabt_send_notify();
    rdabt_goep_init();
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_RESET_REQ_IND, NULL, NULL);
}


void rdabt_main_msg_hdler(kal_msgqid queue_id, ilm_struct *message)
{
    switch(message->msg_id)
    {
        case MSG_ID_BT_NOTIFY_EVM_IND:
        {
            if(rdabt_send_notify_pending)
                rdabt_send_notify_pending--;
            break;
        }
        case MSG_ID_UART_READY_TO_READ_IND:
        {
            rdabt_adp_uart_rx();
            rdabt_recive_msg_count--;
            break;
        }
        //process in exit lp mode
        case MSG_ID_BT_HOST_WAKE_UP_IND:
        {
            kal_uint16 num_send;
            mmi_trace(g_sw_BT, "MSG_ID_BT_HOST_WAKE_UP_IND\n");
            rdabt_adp_uart_tx(rdabt_wakeup_data,sizeof(rdabt_wakeup_data),&num_send);
            break;
        }

        case MSG_ID_BT_POWEROFF_REQ:
        {
            RDABT_Shut_Down();
            // rdabt_antenna_off();


           //sxr_StopFunctionTimer(CheckIsNeedToSleepBT);
            //rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_POWEROFF_CNF, NULL, NULL);	
            break;
        }
        case MSG_ID_BT_SDPDB_REGISTER_REQ:
        {
            bt_sdpdb_register_req_struct *msg_p = (bt_sdpdb_register_req_struct *)message->local_para_ptr;
            rdabt_bt_sdpdb_register_req(msg_p);
            break;
        }

        case     MSG_ID_BT_SDPDB_DEREGISTER_REQ:
        {
            bt_sdpdb_deregister_req_struct *msg_p = (bt_sdpdb_deregister_req_struct *)message->local_para_ptr;
            mmi_trace(g_sw_BT,"rdabt_main-msg_hdler process MSG_ID_BT_SDPDB_DEREGISTER_REQ");
            rdabt_sdpdb_deregister_req(msg_p);
            break;
        }

        case MSG_ID_BT_RESTART_REQ:
        {
            rdabt_bt_restart();
            break;
        }
        case MSG_ID_BT_MMI_RESET_REQ:
        {
            rdabt_antenna_off();
            mmi_trace(g_sw_BT,"rdabt_main-msg_hdler process MSG_ID_BT_MMI_RESET_REQ");
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_MMI_RESET_CNF, NULL, NULL);
            break;
        }
        default:
            break;
    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_adp_msg_dispatch
* DESCRIPTION
*   This function handle of internal message
* PARAMETERS
*   Message
* RETURNS
*   None
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
t_api rdabt_adp_msg_dispatch(rdabt_msg_t *message)
{
    if(message->msg_id >= MGR_CHANGE_LOCAL_NAME && message->msg_id <=SDP_SERVICE_SEARCH_CNF)
    {
        rdabt_mgr_int_msg_hdlr(message->msg_id, message->data);
    }
    else if(message->msg_id >= A2DP_SIGNAL_CONNECT_CNF && message->msg_id <=A2DP_MESSAGE_END)
    {
        rdabt_a2dp_int_msg_hdlr(message->msg_id, message->data);
    }

    else if(message->msg_id >=OBEX_CLI_PUSH_SERVER_CNF &&  message->msg_id <=OBEX_SERVER_AUTHORIZE_IND)
    {
        rdabt_obex_dispatch_event(message);
    }

    else if(message->msg_id >= AVRCP_ACTIVATE_CNF && message->msg_id <=AVRCP_MESSAGE_END)
    {
        rdabt_avrcp_int_msg_hdlr(message->msg_id, message->data);
    }
    else if(message->msg_id >= HFG_ACTIVATE_CNF && message->msg_id <=HFG_MESSAGE_END)
    {
        rdabt_hfp_int_msg_hdlr(message->msg_id, message->data);
    }
    else if(message->msg_id >= SPP_CONNECT_IND && message->msg_id <=SPP_DISCONNECT_CNF)
    {
        rdabt_spp_int_msg_hdlr(message->msg_id, message->data);
    }
    else
    {
        switch(message->msg_id)
        {
            case ADP_DATA_REQ:
            {
                adp_data_msg_t *data_msg = (adp_data_msg_t*)message->data;

                //sxr_Sleep(600);/*10 ms*/
                rdabt_uart_tx_pdu(data_msg->buff,data_msg->type,data_msg->flags,data_msg->hci_handle_flags);
            }
            break;
            case ADP_TRANSPORT_CNF:
            {
                u_int32 status = ((u_int32)(message->data));
                u_int8 transport_id;
                transport_id = status&0xf;
                status = status >> 8;
                kal_prompt_trace(1,"received ADP_TRANSPORT_CNF transport_id=%d,status=%d,IsFirst_PowerOn=%d",transport_id,status,IsFirst_PowerOn);


                if(transport_id != TRANSPORT_UART_ID || status != RDABT_NOERROR)
                {

                    kal_prompt_trace(1,"ADP_TRANSPORT_CNF power on fail IsFirst_PowerOn=%d ",IsFirst_PowerOn);
                    RDABT_Terminate(0);
                    rdabt_antenna_off();
                    if(IsFirst_PowerOn)
                    {

                        sxr_Sleep(160);
                        IsFirst_PowerOn=FALSE;
                        rdabt_antenna_on();//try to poweron again
                        rdabt_p->state = RDABT_STATE_POWER_ON;

                        rdabt_p->original_boot_state = RDABT_STATE_POWER_ON;
                    }
                    else
                    {
                        bt_bm_PowerON_cnf_struct *cnf_param_ptr;
                        cnf_param_ptr = (bt_bm_PowerON_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_bm_PowerON_cnf_struct), TD_CTRL);
                        cnf_param_ptr->result=FALSE;
                        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_POWERON_CNF, cnf_param_ptr, NULL);

                    }
                }
                else
                    RDABT_Start_Up(TRANSPORT_UART_ID);
                break;
            }
            case HOST_INIT_CNF:
                kal_prompt_trace(1,"received HOST_INIT_CNF rdabt_p->original_boot_state=%d",rdabt_p->original_boot_state);
                if(rdabt_p->original_boot_state)
                {
#if 1
                    t_MGR_HCVersion version;
                    u_int16 num_send;
//#ifndef _MMI_FTS_SUPPORT__
//adf
                    //          rdabt_sdp_replace_service(0x0010004,NULL);
//#endif
                    /* send to MMI */
                    kal_prompt_trace(1,"rdabt_adp_msg_dispatch received HOST_INIT_CNF power bt success");

                    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_READY_IND, NULL, NULL);

                    MGR_GetDeviceVersion(&version);
                    switch(version.HCIrevision)
                    {
                        case 11: // r11
                            init_flag[6] = 0xe0;
                            init_flag[7] = 0x2a;
                            break;
                        case 10: // r10
                            init_flag[6] = 0x58;
                            init_flag[7] = 0x68;
                            break;
                    }

                    rdabt_adp_uart_tx(init_flag, sizeof(init_flag), &num_send);
                    load_paired_device_info();

#endif
                }
                break;
            case HOST_SHUTDOWN_CNF:
            {
                kal_prompt_trace(1,"RDABT_Terminate ");
                RDABT_Terminate(0);
                rdabt_antenna_off();
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_POWEROFF_CNF, NULL, NULL);
                rdabt_send_notify_pending = 0;
                rdabt_send_notify();
                break;
            }
            case ADP_WAKE_IND:
            {
                kal_uint16 num_send;
                //L1SM_SleepDisable(rdabt_cntx.l1_handle);
                //GPIO_ModeSetup(77,1);
                //GPIO_WriteIO(77,1);
#ifdef __SUPPORT_BT_PHONE__
                extern u_int8 current_sco_connected;
                mmi_trace(g_sw_BT,"rdabt_adp_msg_dispatch received ADP_WAKE_IND current_sco_connected=%d",current_sco_connected);
                if(current_sco_connected)
                    RDABT_Send_Message(ADP_WAKE_IND, RDABT_ADP, RDABT_HCI, 0, message->data);
                else
#endif
                {
                    rdabt_adp_uart_tx(rdabt_wakeup_data,sizeof(rdabt_wakeup_data),&num_send);
                    //  hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
                }
            }
            break;
            case ADP_SLEEP_IND:
            {
                kal_prompt_trace(1, "Recieve ADP_SLEEP_IND, data=%d", message->data);
                if(message->data)  // host stack sleep
                {
                    pTimerCancel_base(rdabt_p->timer_id);

                    rdabt_p->timer_id = 0;
                }
                else
                {
                    if(rdabt_p->timer_id == 0)
                        rdabt_p->timer_id= pTimerCreate_base(100,0);

                }
            }
            break;
            default:
                // unknow message
                break;
        }
    }
    return RDABT_NOERROR;
}

/*****************************************************************************
* FUNCTION
*   rdabt_power_on_sched
* DESCRIPTION
*   Main loop when in power on state
* PARAMETERS
*  queue_id               IN     queue index of bt
*   taks_entry_ptr      IN    task entry
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
TASK_HANDLE * G_Bt_hTask = NULL;

void rdabt_power_on_sched(kal_msgqid queue_id)
{
   ilm_struct *ext_message=NULL;
   int status=0;
   mmi_trace(g_sw_BT,"rdabt_power_on_sched");
   COS_EVENT ev;
   Msg_t *BtMsg;
    static UINT32 count; 
	
	rdabt_set_ignore_uart_rx(FALSE);
    while (1)
    {
        /*
           Message within rdabt tasks can be separated into external and internal messages,
           and we take 1:1 ratio between them. Because there are more than 1 internal modules,
           rounb-robin machanism is applied to all the internal modules.
        */

        SUL_ZeroMemory32(&ev, sizeof(COS_EVENT) );

        rdabt_uart_unsleep();
        do
        {
            status=RDABT_Execute(2);
        }
        while (status==RDABT_RETRY);

        count = 0;
        BtMsg = sxr_Wait((u32 *)&ev, COS_TaskMailBoxId(G_Bt_hTask));

        rdabt_uart_unsleep();
        //kal_prompt_trace(1,"%x",ev.nEventId);
        if (BtMsg!= NULL )
        {
            UINT32 *msg= (UINT32*)BtMsg;
            if (*msg  == 0x12345678)
            {
                rdabt_bt_restart();
                mmi_trace(g_sw_BT,"error occur $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
            }
            else
                mmi_trace(g_sw_BT,"rdabt_power_on_sched Get a envet for BT set event\n");
            sxr_Free(BtMsg);
        }
        else
        {
            if ( (ev.nEventId == BT_TIMER_PAPI+1) || (ev.nEventId == BT_TIMER_PAPI+2))
            {
                if(ev.nEventId == BT_TIMER_PAPI+1)
                {
                    //mmi_trace(g_sw_BT,"timer");
                    RDABT_Send_Message(ADP_TIMER_IND, RDABT_ADP, RDABT_HOST_CORE, 0, NULL);
                    pTimerCancel_base(ev.nEventId-BT_TIMER_PAPI);
                    rdabt_p->timer_id=pTimerCreate_base(100,0);
                }
                else
                {
                    extern void A2dpDataSendTimerCallback(void);                            //for A2DP timer callback
                    pTimerCancel_base(ev.nEventId-BT_TIMER_PAPI);
                    A2dpDataSendTimerCallback();
                }
            }
            else
            {
                ext_message = (ilm_struct*)ev.nParam1;
                //mmi_trace(1,"rdabt_power_on_sched Get a Message id = %d,count = %d",ext_message->msg_id,count);
                if (ext_message==NULL)
                    asm("break 1");

                if( (ext_message->msg_id >=MSG_ID_BT_A2DP_ACTIVATE_REQ) && (ext_message->msg_id<= MSG_ID_BT_A2DP_STREAM_ABORT_RES))
                {
                    rdabt_a2dp_msg_hdlr( ext_message );
                }
                else if ( (ext_message->msg_id >=MSG_ID_BT_AVRCP_ACTIVATE_REQ) && (ext_message->msg_id<= MSG_ID_BT_AVRCP_CMD_FRAME_IND_RES))
                {
                    rdabt_avrcp_msg_hdlr( ext_message );
                }
                else if( (ext_message->msg_id ==MSG_ID_MEDIA_BT_AUDIO_OPEN_REQ) || (ext_message->msg_id ==MSG_ID_MEDIA_BT_AUDIO_CLOSE_REQ)
#ifdef __SUPPORT_BT_PHONE__
                         || ( ext_message->msg_id == MSG_ID_BT_PHONE_SCO_DATA_SEND_REQ)
#endif
                         || ((ext_message->msg_id >=MSG_TAG_BT_HFG_FIRST_REQ_MSG) && (ext_message->msg_id<= MSG_TAG_BT_HFG_LAST_REQ_MSG)))
                {
                    rdabt_hfg_msg_hdler(ext_message);
                }
                else if( ext_message->msg_id<=MSG_TAG_BT_HSG_LAST_REQ_MSG && ext_message->msg_id>=MSG_TAG_BT_HSG_FIRST_REQ_MSG)
                {
                    rdabt_hfg_msg_hdler(ext_message);
                }
                else if( (ext_message->msg_id >=MSG_ID_GOEP_REGISTER_SERVER_REQ) && (ext_message->msg_id<= MSG_ID_BT_RECORD_UNREQ_CNF))
                {
                    rdabt_opp_msg_hdler(ext_message);
                }
                else if( (ext_message->msg_id >=MSG_TAG_BT_BM_FIRST_REQ_MSG) && (ext_message->msg_id<= MSG_TAG_BT_BM_LAST_REQ_MSG))
                {
                    rdabt_mgr_msg_hdler(ext_message);
                }
                //  else if( (ext_message->msg_id >=MSG_TAG_BT_SPP_FIRST_REQ_MSG) && (ext_message->msg_id<= MSG_TAG_BT_SPP_LAST_REQ_MSG))
                // {
                //   rdabt_spp_msg_hdler(ext_message);
                //}
                else
                {
                    rdabt_main_msg_hdler(queue_id, ext_message);
                }
                if(ext_message)
                {
                    MMI_Free_msg_buf(ext_message);
                    ext_message  = NULL;
                }
                else
                {
                    asm("break 1");
                }
                if(rdabt_p->state == RDABT_STATE_POWER_OFF)
                    break;
            }
        }
    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_main
* DESCRIPTION
*   This function is the main function of bt task
* PARAMETERS
*   task_entry_ptr  IN   taks entry of bt
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/

void rdabt_main(void)
{
    kal_msgqid queue_id=0;//not used

    /* main loop of bchs_main for all kinds of states */
    mmi_trace(g_sw_BT,"entry rdabt_main--------------------------------------------\n");
    memset(rdabt_p,0,sizeof(rdabt_cntx));

    g_btdConfig = tgt_GetBtdConfig();
    if (g_btdConfig->pinReset.type == HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetOut(g_btdConfig->pinReset.gpioId);
    }
    if (g_btdConfig->pinWakeUp.type == HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetOut(g_btdConfig->pinWakeUp.gpioId);
    }
    if (g_btdConfig->pinSleep.type == HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetIn(g_btdConfig->pinSleep.gpioId);
    }

    rdabt_poweronphone_init();

    G_Bt_hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);

    while(1)
    {
        switch(rdabt_p->state)
        {
            case RDABT_STATE_POWER_OFF:
                rdabt_power_off_sched(queue_id);
                break;
            case RDABT_STATE_POWER_ON:
                rdabt_power_on_sched(queue_id);
                break;
            default:
                ASSERT(0);
        }
    }
}

#ifdef __SUPPORT_BT_PHONE__
int32 MMC_AudioSCO_AsynSendReq(int16* pdu,uint16 handle, uint16 length)
{
    COS_EVENT sEv = {0,};
    //AudioSCO_ASYN_EVENT sEv = {0,};
    AudioSCO_ASYN_PDU  *pParam = NULL;
    int16* pPdu = NULL;

    if(NULL == pdu )
    {
        return -1;
    }

    pParam = (AudioSCO_ASYN_PDU*)COS_MALLOC(SIZEOF(AudioSCO_ASYN_PDU));
    if(pParam == NULL)
    {
        return -1;
    }
    pPdu  = (int16*)COS_MALLOC(length);
    if(pPdu == NULL)
    {
        return -1;
    }
    memcpy(pPdu,pdu,length);
    pParam ->handle = handle;
    pParam ->pdu = pPdu;
    pParam ->length = length;


    ilm_struct *msg = allocate_ilm(MOD_MED);
    if(NULL == msg)
    {
        return -1;
    }
    msg->src_mod_id = MOD_MED;
    msg->dest_mod_id = (module_type)MOD_BT;
    msg->sap_id = BT_APP_SAP;
    msg->msg_id = MSG_ID_BT_PHONE_SCO_DATA_SEND_REQ;
    msg->local_para_ptr = pParam;
    msg->peer_buff_ptr = NULL;
    rda_msg_send_ext_queue(msg);


    return 0;
}
#endif



#endif
