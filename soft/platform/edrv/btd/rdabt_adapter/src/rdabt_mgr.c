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



#include "sdp.h"
//#include "rdabt_main.h"

//
//#include "bt_dm_struct.h"

//#include "bt_hfg_struct.h"
//#include "bluetooth_bm_struct.h"





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



#include "bluetooth_bm_struct.h"
#include "mmi_trace.h"

#include "rdabt.h"
#include "rdabt_opp.h"
#include "papi.h"
#include "manager.h"

#include "rdabt_mgr.h"
#include "bt_dm_struct.h"

#include "bt_hfg_struct.h"
#include "hfp.h"
#include "hci.h"
#include "mci.h"

u_int8 bonding_role;    // local device initialise bonding or not
u_int8 inquiry_cancel;  // inqyiry cancel or not

bt_bm_service_search_req_struct sdp_search_sdap; // save bonding and service search config

bth_dev_info_array  g_paired_devInfo;
static module_type sco_module = MOD_MED;
static t_bdaddr sco_address;
static u_int16 sco_cid;
u_int8 current_sco_connected=0;
u_int16 sco_index = 0xffff;

#ifdef __SUPPORT_BT_PHONE__
u_int8 current_voice_change_staus=0;// 1 is during change ,0 is finished
static u_int16 sco_states = 0;//o closed,1opend
static u_int16 sco_rx_pkt_length=0;
extern u_int16 BtPhone_CurrentCallStatus;
extern u_int8 BtPhone_CallDirector;
#endif

#define RDABT_RESULT_TO_BM_RESULT(res)               ((res)?BTBM_ADP_FAILED:BTBM_ADP_SUCCESS)

void rdabt_get_local_addr_req(kal_msgqid queue_id);


btbm_bd_addr_t rdabt_bdaddr_to_bm(t_bdaddr bdaddr)
{
    btbm_bd_addr_t bm_addr;
    bm_addr.lap= BDADDR_Get_LAP(&bdaddr);
    bm_addr.uap = BDADDR_Get_UAP(&bdaddr);
    bm_addr.nap = BDADDR_Get_NAP(&bdaddr);

    return bm_addr;
}

t_bdaddr rdabt_bm_bdaddr_to_rdabt(btbm_bd_addr_t bdaddr)
{
    t_bdaddr rda_addr;
    BDADDR_Set_LAP(&rda_addr,bdaddr.lap);
    BDADDR_Set_UAP(&rda_addr,bdaddr.uap);
    BDADDR_Set_NAP(&rda_addr,bdaddr.nap);
    return rda_addr;
}



void load_paired_device_info(void)
{
    int i= 0;
    t_bdaddr        address;
    kal_prompt_trace(1,"load_paired_device_info devNum=:%d\n", g_paired_devInfo.devNum);
    for( i= 0; i<g_paired_devInfo.devNum; i++)
    {
        address=rdabt_bm_bdaddr_to_rdabt(g_paired_devInfo.devInfo_Array[i].bd_addr);
        MGR_AddDeviceRecord(address,  g_paired_devInfo.devInfo_Array[i].linkKey, g_paired_devInfo.devInfo_Array[i].KeyType);
    }
}
extern void bt_mmi_update_pairDevice_info(void);
int rdabt_get_device_authriose_info(t_bdaddr address)
{
    btbm_bd_addr_t   bd_addr_str;
    int i= 0;

    bd_addr_str.lap= BDADDR_Get_LAP(&address);
    bd_addr_str.uap= BDADDR_Get_UAP(&address);
    bd_addr_str.nap= BDADDR_Get_NAP(&address);
    bt_mmi_update_pairDevice_info();
    kal_prompt_trace(1,"rdabt_get_device_authriose_info devNum=:%d\n", g_paired_devInfo.devNum);

    for( i= 0; i<g_paired_devInfo.devNum; i++)
    {
        if(
            bd_addr_str.lap==g_paired_devInfo.devInfo_Array[i].bd_addr.lap&&
            bd_addr_str.uap==g_paired_devInfo.devInfo_Array[i].bd_addr.uap&&
            bd_addr_str.nap==g_paired_devInfo.devInfo_Array[i].bd_addr.nap
        )
            break;
    }
    if(i<g_paired_devInfo.devNum)
    {
        kal_prompt_trace(1,"rdabt_get_device_authriose_info  g_paired_devInfo.devInfo_Array[i].authorise=:%d\n",  g_paired_devInfo.devInfo_Array[i].authorise);

        return g_paired_devInfo.devInfo_Array[i].authorise;
    }
    else
        return 0;

}
int rdabt_write_paired_device_info(bth_dev_struct * pDevInfo)
{
    int i = 0;
    if(!pDevInfo)
        return 1;
    //traverse through the device array
    for( ; i<g_paired_devInfo.devNum; i++)
    {
        if( g_paired_devInfo.devInfo_Array[i].bd_addr.lap == pDevInfo->bd_addr.lap
                && g_paired_devInfo.devInfo_Array[i].bd_addr.nap == pDevInfo->bd_addr.nap
                && g_paired_devInfo.devInfo_Array[i].bd_addr.uap == pDevInfo->bd_addr.uap)
            break;
    }

    //if the device's info exgist, replace it, else add a new
    if(i<10)
    {
        memcpy(&g_paired_devInfo.devInfo_Array[i],  pDevInfo, sizeof(bth_dev_struct));

        if( i==g_paired_devInfo.devNum)
            g_paired_devInfo.devNum++;

        //bth_write_nvram();
    }
    return 0;
}


int rdabt_del_paired_device_info(btbm_bd_addr_t bd_addr,  int flag)
{
    int i = 0;
    if(flag != 0)
    {
        g_paired_devInfo.devNum = 0;
        memset(&g_paired_devInfo, 0, sizeof(bth_dev_info_array));
    }
    else
    {
        for(; i<g_paired_devInfo.devNum; i++)
        {
            if(     g_paired_devInfo.devInfo_Array[i].bd_addr.lap == bd_addr.lap
                    && g_paired_devInfo.devInfo_Array[i].bd_addr.uap== bd_addr.uap
                    && g_paired_devInfo.devInfo_Array[i].bd_addr.nap== bd_addr.nap)
            {
                memmove(&g_paired_devInfo.devInfo_Array[i], &g_paired_devInfo.devInfo_Array[i+1], sizeof(bth_dev_struct)*(g_paired_devInfo.devNum-i-1));
                g_paired_devInfo.devNum--;
                memset(&g_paired_devInfo.devInfo_Array[g_paired_devInfo.devNum], 0, sizeof(bth_dev_struct));
                break;
            }
        }
    }
    //bth_write_nvram();

    return 0;
}


void rdabt_SDP_serviceSearch(t_bdaddr bd_addr)
{
    t_SDP_SearchPattern     pattern;
    t_SDP_AttributeIDs      attributes;
    t_SDP_StopRule      rule;
    u_int32 patternUUIDs = SDP_PROTOCOL_UUID_L2CAP;
    u_int32 attributeIDs[4] = {SDP_AID_SERVICE_CLASS, SDP_AID_PROFILE_LIST, SDP_AID_PROTOCOL, SDP_AID_SUPPORTED_FEATURES};
    u_int8 aid_range=0;

//  u_int32     cod = 0;

    kal_prompt_trace(1,"--> rdabt_SDP_serviceSearch");
//   kal_prompt_trace(1,"--> rdabt_SDP_serviceSearch, cod == %d", cod);

    pattern.numItems = 1;
    pattern.patternUUIDs = &patternUUIDs;

/// if(cod & 0x200)
    attributes.numItems = 1;
//  else
//      attributes.numItems = 4;

    attributes.attributeIDs = attributeIDs;
    attributes.aid_range=&aid_range;
    rule.maxBytes = 512;
    rule.maxDuration = 7;//50;
    rule.maxItems = 10;

    SDP_DeviceServiceSearch(bd_addr,&pattern,&attributes,&rule);

}


void rdabt_ServiceSearch_cb(void *data)
{
    t_SDP_service_Parse_Results  uuid_finded;
    u_int32 i,j,k, bfinded, searched_number;
    bt_bm_service_search_result_ind_struct *result_ind;
    bt_bm_service_search_cnf_struct *service_cnf;
    t_SDP_Results *pResults = (t_SDP_Results*)data;

    memset(&uuid_finded, 0, sizeof(t_SDP_service_Parse_Results));
    if(pResults->element.dataLen)
        SDP_ServiceResultParse(pResults, &uuid_finded);

    kal_prompt_trace(1," rdabt_ServiceSearch_cb sdp_search_sdap.sdap_len %d uuid_finded.total_searched %d ",sdp_search_sdap.sdap_len,uuid_finded.total_searched);

    for(i=0,j=0, searched_number=0; j<uuid_finded.total_searched; j++)
    {
        bfinded=0;
        // kal_prompt_trace(1," rdabt_ServiceSearch_cb uuid_finded.service[%d].SC_uuid %x  ",j,uuid_finded.service[j].SC_uuid);
        for(k=0; k<sdp_search_sdap.sdap_len; k++)
        {
            //  kal_prompt_trace(1," rdabt_ServiceSearch_cb sdp_search_sdap.sdap_uuid[%d] %x  ",k,sdp_search_sdap.sdap_uuid[k]);
            if(sdp_search_sdap.sdap_uuid[k]==uuid_finded.service[j].SC_uuid)
                bfinded=1;
            i++;
        }

        if(bfinded)
        {
            kal_prompt_trace(1,"rdabt_ServiceSearch_cb bfinded=%d,searched_number=%d",bfinded,searched_number);
            result_ind = (bt_bm_service_search_result_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_service_search_result_ind_struct), TD_CTRL);
            result_ind->uuid = uuid_finded.service[j].SC_uuid;
            result_ind->bd_addr = sdp_search_sdap.bd_addr;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND, (void *)result_ind, NULL);
            searched_number ++;
        }
    }
    service_cnf = (bt_bm_service_search_cnf_struct*)construct_local_para( (kal_uint16)sizeof(bt_bm_service_search_cnf_struct), TD_CTRL);
    service_cnf->result = pResults->status;
    service_cnf->searched_number = searched_number;
    service_cnf->bd_addr = sdp_search_sdap.bd_addr;
    kal_prompt_trace(1, "rdabt_ServiceSearch_cb send MSG_ID_BT_BM_SERVICE_SEARCH_CNF");

    SDP_FreeResults(pResults);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_SERVICE_SEARCH_CNF, (void *)service_cnf, NULL);

}

void rdabt_Remote_name_cb(void *data)
{

    bt_bm_read_remote_name_cnf_struct *param_ptr = (bt_bm_read_remote_name_cnf_struct*)construct_local_para(
                (kal_uint16)sizeof(bt_bm_read_remote_name_cnf_struct), TD_CTRL);

    t_MGR_DeviceEntry*entry = (t_MGR_DeviceEntry*)data;
    kal_prompt_trace(1, "rdabt_Remote_name_cb");
    mmi_trace_hex(g_sw_BT,20,  param_ptr->name);
    memset(param_ptr,0,sizeof(bt_bm_read_remote_name_cnf_struct));
    mmi_trace_hex(g_sw_BT,20,  param_ptr->name);

    if(entry && entry->name_length)
    {
        param_ptr->result = 0;
        param_ptr->bd_addr = rdabt_bdaddr_to_bm(entry->bdAddress);
        param_ptr->cod = entry->classOfDevice;
        param_ptr->name_len = entry->name_length;
        memcpy(param_ptr->name, entry->name, entry->name_length);
    }
    else
        param_ptr->result = 1; // error
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_READ_REMOTE_NAME_CNF, (void *)param_ptr, NULL);
}
const uint8 rdabt_pskey_rf_setting_inquiry[] = {0x00,0x0a,0x00,0x0c,0x40,0x30,0xd0,0x30,0xb5,0x30,0xba,0xba};
const uint8 rdabt_pskey_rf_setting_inquiry_end[] = {0x00,0x0a,0x00,0x0c,0x40,0x30,0xb5,0x30,0xb5,0x30,0xba,0xba};


/*****************************************************************************
* FUNCTION
*  rdabt_link_discovery_result_ind
* DESCRIPTION
*   This function is to send msg to MMI task
*
* PARAMETERS
*  a  IN/OUT      first variable, used as returns
*  b  IN       second variable
* RETURNS
*  NONE.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_link_discovery_result_ind(void *data)
{
    bt_bm_discovery_result_ind_struct *param_ptr;
    t_MGR_DeviceEntry*pDevice = (t_MGR_DeviceEntry*)data;

    param_ptr = (bt_bm_discovery_result_ind_struct *) construct_local_para(sizeof(bt_bm_discovery_result_ind_struct), TD_CTRL);
    kal_prompt_trace(1, "rdabt_link_discovery_result_ind %x ,%s",pDevice->classOfDevice,pDevice->name);

    param_ptr ->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
    param_ptr ->cod = pDevice->classOfDevice;
    memset(param_ptr->name,0,BTBM_ADP_MAX_NAME_LEN);
    memcpy(param_ptr->name,pDevice->name,strlen((char*)pDevice->name));
    param_ptr ->name_len = strlen((char*)pDevice->name);

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_DISCOVERY_RESULT_IND, param_ptr, NULL);

}

void rdabt_link_discovery_cnf(void *data)
{
    bt_bm_discovery_cnf_struct *param_ptr;
    u_int32 result = (u_int32)data;
    kal_prompt_trace(1, "rdabt_link_discovery_cnf inquiry_cancel=%d,result=0x%x",inquiry_cancel ,result);
    param_ptr = (bt_bm_discovery_cnf_struct *) construct_local_para(sizeof(bt_bm_discovery_cnf_struct), TD_CTRL);
    result= RDABT_NOERROR;

    param_ptr->result = RDABT_RESULT_TO_BM_RESULT(result);
    param_ptr->total_number = MGR_GetDeviceNumFound();


    rdabt_send_msg_up(MOD_MMI, inquiry_cancel?MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:MSG_ID_BT_BM_DISCOVERY_CNF, param_ptr, NULL);
    rdabt_write_pskey(0x24, rdabt_pskey_rf_setting_inquiry_end, sizeof(rdabt_pskey_rf_setting_inquiry_end));
    sxr_Sleep(5 MS_WAITING);
}


void rdabt_send_pin_code_ind(void *data)
{
    bt_bm_pin_code_ind_struct *param_ptr;
    t_MGR_DeviceEntry*pDevice = (t_MGR_DeviceEntry*)data;
    kal_prompt_trace(1, "rdabt_send_pin_code_ind CALLED");

    param_ptr = (bt_bm_pin_code_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_pin_code_ind_struct), TD_CTRL);
    param_ptr->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
    memset(param_ptr->name,0,BTBM_ADP_MAX_NAME_LEN);
    memcpy(param_ptr ->name,pDevice->name,strlen((char*)pDevice->name));
    param_ptr ->name_len = strlen((char*)pDevice->name);
    //param_ptr ->enable_16digits_pin_code = 0;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_PIN_CODE_IND, param_ptr, NULL);
}

void rdabt_link_key_ind(void *data)
{
    t_MGR_DeviceEntry *dbEntry = (t_MGR_DeviceEntry *)data;

    MGR_LinkKeyResponse(dbEntry->bdAddress, FALSE, NULL, 0);
    kal_prompt_trace(1, "rdabt_link_key_ind CALLED");

}

void rdabt_link_pin_code_ind(void *data)
{
    t_MGR_DeviceEntry*pDevice = (t_MGR_DeviceEntry*)data;

//    if( bonding_role != 0)
    {
        rdabt_send_pin_code_ind(pDevice);
    }
    /*    else
        {
            //may be the other device has lost the security info..
            //anyway, we do not need to bond...

            bt_bm_link_connect_accept_ind_struct      * accept_ind;

            accept_ind = (bt_bm_link_connect_accept_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_link_connect_accept_ind_struct), TD_CTRL);
            accept_ind->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);

            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND, (void *)accept_ind, NULL);
        }
    */
}

void rdabt_link_passkey_ind(void *data)
{
    /*
       mgr_ssp_ind_msg_t *msg = (mgr_ssp_ind_msg_t*)data;
       t_MGR_DeviceEntry *pDevice = (t_MGR_DeviceEntry *)msg->device;
       bt_bm_security_passkey_notify_ind_struct *param_ptr;
       param_ptr = (bt_bm_security_passkey_notify_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_security_passkey_notify_ind_struct), TD_CTRL);
       param_ptr->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
       memset(param_ptr->name,0,BTBM_ADP_MAX_NAME_LEN);
       memcpy(param_ptr ->name,pDevice->name,strlen((char*)pDevice->name));
       param_ptr ->name_len = strlen((char*)pDevice->name);
       param_ptr ->numeric[0] = msg->value%10;
       param_ptr ->numeric[1] = (msg->value/10)%10;
       param_ptr ->numeric[2] = (msg->value/100)%10;
       param_ptr ->numeric[3] = (msg->value/1000)%10;
       param_ptr ->numeric[4] = (msg->value/10000)%10;
       param_ptr ->numeric[5] = (msg->value/100000)%10;

       rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND, param_ptr, NULL);
       */
    kal_prompt_trace(1, "rdabt_link_passkey_indCALLED");

}


void rdabt_link_confirm_ind(void *data)
{
#ifdef __MMI_BT_SIMPLE_PAIR__
    mgr_ssp_ind_msg_t *msg = (mgr_ssp_ind_msg_t*)data;
    t_MGR_DeviceEntry *pDevice = (t_MGR_DeviceEntry *)msg->device;
    bt_bm_security_user_confirm_ind_struct *param_ptr;
    param_ptr = (bt_bm_security_user_confirm_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_security_user_confirm_ind_struct), TD_CTRL);
    param_ptr->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
    memset(param_ptr->name,0,BTBM_ADP_MAX_NAME_LEN);
    memcpy(param_ptr ->name,pDevice->name,strlen((char*)pDevice->name));
    param_ptr ->name_len = strlen((char*)pDevice->name);
    param_ptr ->numeric[5] = '0'+(msg->value%10);
    param_ptr ->numeric[4] = '0'+((msg->value/10)%10);
    param_ptr ->numeric[3] = '0'+((msg->value/100)%10);
    param_ptr ->numeric[2] = '0'+((msg->value/1000)%10);
    param_ptr ->numeric[1] = '0'+((msg->value/10000)%10);
    param_ptr ->numeric[0] = '0'+((msg->value/100000)%10);
    param_ptr ->display_numeric = msg->isDisplay;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND, param_ptr, NULL);

    kal_prompt_trace(1, "rdabt_link_confirm_ind msg->value=0X%X,msg->isDisplay=%d",msg->value,msg->isDisplay);
    // MGR_ConfirmResponse(pDevice->bdAddress, TRUE);
#endif
}
t_bdaddr bdAddress_sdp;
void rdabt_delay_sdp_process(void * temp)
{
    kal_prompt_trace(1, "rdabt_delay_sdp_process");
    rdabt_SDP_serviceSearch(bdAddress_sdp);

}

void rdabt_bonding_cnf(t_MGR_DeviceEntry *pDevice, u_int8 status)
{
//extern u_int8 * rdabt_mgr_printLargeNumber(u_int8 *number, u_int8 length) ;
    bt_bm_bonding_result_ind_struct *result_ptr;
    bth_dev_struct  pDevInfo;

    result_ptr = (bt_bm_bonding_result_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_bonding_result_ind_struct), TD_CTRL);
#if pDEBUG
    //    kal_prompt_trace(1, "rdabt_bonding_cnf linkey=%s,pDevice=0x%x",rdabt_mgr_printLargeNumber(pDevice->linkKey,16),pDevice);
#endif
    result_ptr->result = RDABT_RESULT_TO_BM_RESULT(status);
    result_ptr->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
    result_ptr->KeyType= pDevice->linkKeyType;
    memcpy(result_ptr ->linkKey,pDevice->linkKey,16);
    result_ptr->cod= pDevice ->classOfDevice;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_BONDING_RESULT_IND, result_ptr, NULL);

    pDevInfo.bd_addr=result_ptr->bd_addr;
    pDevInfo.KeyType=result_ptr->KeyType;
    pDevInfo.cod=result_ptr->cod;
    memcpy(pDevInfo.linkKey,result_ptr->linkKey,16);
    rdabt_write_paired_device_info(&pDevInfo);

    if(bonding_role)
    {
        bt_bm_bonding_cnf_struct *param_ptr;

        param_ptr = (bt_bm_bonding_cnf_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_bonding_cnf_struct), TD_CTRL);

        param_ptr->result = RDABT_RESULT_TO_BM_RESULT(status);
        param_ptr->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
        // param_ptr->key_type = pDevice->linkKeyType;
        //memcpy(param_ptr ->link_key,pDevice->linkKey,16);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_BONDING_CNF, param_ptr, NULL);
        if(status == RDABT_NOERROR)
        {
            //   rdabt_SDP_serviceSearch(pDevice->bdAddress);
            memcpy(bdAddress_sdp.bytes,pDevice->bdAddress.bytes,6);
            rdabt_timer_create(10,  rdabt_delay_sdp_process,NULL,pTIMER_ONESHOT);
        }


    }
    else
    {

        bt_bm_bonding_cnf_struct * bonding_cnf=NULL;

        bonding_cnf = (bt_bm_bonding_cnf_struct *)construct_local_para((kal_uint16)sizeof(bt_bm_bonding_cnf_struct), TD_CTRL);
        bonding_cnf->result = status;
        bonding_cnf->bd_addr = rdabt_bdaddr_to_bm(pDevice->bdAddress);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_BE_BONDING_CNF, (void*)bonding_cnf, NULL);//zhou siyou add 200728
    }


    bonding_role = 0;
}

void rdabt_link_bonding_cnf(void *msg)
{
    mgr_bond_cnf_msg_t *bond_msg = (mgr_bond_cnf_msg_t*)msg;
    rdabt_bonding_cnf((t_MGR_DeviceEntry*)(bond_msg->device), bond_msg->result);
}


void rdabt_link_oob_data_ind(void *msg)
{
    t_MGR_DeviceEntry *pDevice = (t_MGR_DeviceEntry *)msg;
    kal_prompt_trace(1, "rdabt_link_oob_data_ind");
    MGR_OOBDataResponse(pDevice->bdAddress, FALSE, NULL, NULL);
}

void rdabt_sco_connect_cnf(u_int8 result)
{
    bt_hfg_sco_connect_cnf_struct     * connect_cnf;

    connect_cnf = (bt_hfg_sco_connect_cnf_struct*)construct_local_para((kal_uint16)sizeof(bt_hfg_sco_connect_cnf_struct), TD_CTRL);
    connect_cnf->result = result;
    connect_cnf->connection_id = sco_cid;
    //  connect_id =connect_cnf->connection_id;
    rdabt_send_msg_up(sco_module, MSG_ID_BT_HFG_SCO_CONNECT_CNF, (void *)connect_cnf, NULL);
}

void rdabt_sco_disconnect_cnf(u_int8 result)
{
    bt_hfg_sco_disconnect_cnf_struct      * connect_cnf;

    connect_cnf = (bt_hfg_sco_disconnect_cnf_struct*)construct_local_para((kal_uint16)sizeof(bt_hfg_sco_disconnect_cnf_struct), TD_CTRL);
    connect_cnf->result = result;
    connect_cnf->connection_id = sco_cid;
    //      connect_id =0;
    kal_prompt_trace(1, "rdabt_sco_disconnect_cnf");
    rdabt_send_msg_up(sco_module, MSG_ID_BT_HFG_SCO_DISCONNECT_CNF, (void *)connect_cnf, NULL);
}


#ifdef __SUPPORT_BT_PHONE__
u_int16 rdabt_sco_con_get_fshandle(void)
{
    kal_prompt_trace(1, "rdabt_sco_con_get_fshandle =0x%x",sco_index );
    return sco_index;
}
u_int16 btphone_get_voice_change_status(void)
{
    kal_prompt_trace(1, "btphone_get_voice_change_status  current_voice_change_staus=0x%x",current_voice_change_staus );
    return current_voice_change_staus;
}
u_int16 rdabt_sco_player_states(void)
{
    return sco_states;
}
u_int16 rdabt_get_sco_RxPkt_length(void)
{
    return sco_rx_pkt_length;
}
#endif


extern u_int8 rdabt_currentType;
extern  u_int16 current_cid;
void rdabt_sco_connect_ind(void *data)
{
    struct st_t_dataBuf *cmd = NULL;

    mgr_sco_link_t *msg = (mgr_sco_link_t*)data;
    kal_prompt_trace(1, "rdabt_sco_connect_ind msg->result =%d,rdabt_currentType=0x%x,msg->rx_length=%d,current_cid=%x",msg->result,rdabt_currentType,msg->rx_length ,current_cid);
    if(msg->result != RDABT_NOERROR)
        ;//  rdabt_sco_connect_cnf(msg->result);






















    else
    {
        if( RDABT_NOERROR == HCI_WriteLinkPolicySettings(&cmd, current_cid, 0x00))
            HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
        sco_index = msg->sco_handle;
        if(ACTIVE_PROFILE_HANDSFREE_AG==rdabt_currentType)
        {
            bt_hfg_sco_connect_cnf_struct     * connect_cnf;
            connect_cnf = (bt_hfg_sco_connect_cnf_struct*)construct_local_para((kal_uint16)sizeof(bt_hfg_sco_connect_cnf_struct), TD_CTRL);
            connect_cnf->result = msg->result;
            connect_cnf->connection_id = sco_cid;
            rdabt_send_msg_up(sco_module, MSG_ID_BT_HFG_SCO_CONNECT_CNF, (void *)connect_cnf, NULL);

        }
#ifdef __SUPPORT_BT_PHONE__
        else
        {
            u_int32  * sco_state;
            sco_state = (u_int32*)construct_local_para((kal_uint16)sizeof(u_int32), TD_CTRL);
            *sco_state = 0x1;
            sco_rx_pkt_length=msg->rx_length;
            if(sco_rx_pkt_length==0)
                sco_rx_pkt_length=30;
            kal_prompt_trace(1, "rdabt_sco_connect_ind BtPhone_CurrentCallStatus=%ds ",BtPhone_CurrentCallStatus);
            current_sco_connected=TRUE;
            if(current_voice_change_staus)
                current_voice_change_staus=0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_SCO_INFO_IND, (void*)sco_state, NULL);//zhou siyou add 200728
            if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_OUTGOING)
            {
                bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                rdabt_a2dp_pause_by_hfp();
                HF_Get_Number(call_info->call_number,&call_info->call_number_length);
                call_info->call_status=1;
                call_info->call_director=BtPhone_CallDirector;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
            }
            else if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_INCOMING)
            {
                rdabt_a2dp_pause_by_hfp();
            }
            else if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_ACTIVE)
            {
                bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                HF_Get_Number(call_info->call_number,&call_info->call_number_length);
                call_info->call_status=2;
                call_info->call_director=BtPhone_CallDirector;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
                BtPhone_CurrentCallStatus=BT_CALL_STATUS_ACTIVE;
            }

            sco_states = 1;
        }
#endif
    }
}

void rdabt_sco_disconnect_ind(void *data)
{
    mgr_sco_link_t *msg = (mgr_sco_link_t*)data;
    struct st_t_dataBuf *cmd = NULL;
    current_sco_connected=FALSE;
    kal_prompt_trace(1, "rdabt_sco_disconnect_ind msg->result =%d type%d sco_module%d",msg->result,rdabt_currentType,sco_module );
    if(msg->result != RDABT_NOERROR)
        rdabt_sco_disconnect_cnf(msg->result);




















    else
    {
        sco_index = 0xffff;
        if( RDABT_NOERROR == HCI_WriteLinkPolicySettings(&cmd, current_cid, 0x0f))
            HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
        if(ACTIVE_PROFILE_HANDSFREE_AG==rdabt_currentType)
        {
            bt_hfg_sco_disconnect_ind_struct      * connect_cnf;
            connect_cnf = (bt_hfg_sco_disconnect_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_hfg_sco_disconnect_ind_struct), TD_CTRL);
            connect_cnf->connection_id = sco_cid;
            rdabt_send_msg_up(sco_module, MSG_ID_BT_HFG_SCO_DISCONNECT_IND, (void *)connect_cnf, NULL);

        }
#ifdef __SUPPORT_BT_PHONE__
        else
        {
            u_int32  * sco_state;
            mmi_bt_phone_build_audio(FALSE);
            //Audio_SCOStop();
            sco_state = (u_int32*)construct_local_para((kal_uint16)sizeof(u_int32), TD_CTRL);
            *sco_state = 0x0;
            //last be used ?
            if(current_voice_change_staus)
                current_voice_change_staus=0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_SCO_INFO_IND, (void*)sco_state, NULL);//zhou siyou add 200728
#ifdef BTPHONE_RECOGNITION_CALL
            {
                extern u_int16 btphone_recognition_call;
                kal_prompt_trace(1,"rdabt_sco_disconnect_ind btphone_recognition_call=%d",btphone_recognition_call);
                if(btphone_recognition_call)
                {
                    bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                    call_info->call_status=0;
                    call_info->call_director=0;
                    btphone_recognition_call=0;
                    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
                    BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;

                }
            }
#endif
            sco_states = 0;
        }
#endif

    }
}
void rdabt_acl_connect_ind(void *data)
{
    mgr_acl_link_ind_t *msg = (mgr_acl_link_ind_t*)data;

    bt_bm_link_connect_accept_ind_struct      * connect_ind;

    connect_ind = (bt_bm_link_connect_accept_ind_struct*)construct_local_para((kal_uint16)sizeof(bt_bm_link_connect_accept_ind_struct), TD_CTRL);
    connect_ind->bd_addr = rdabt_bdaddr_to_bm(msg->address);
    kal_prompt_trace(1,"rdabt_acl_connect_ind called");

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND, (void *)connect_ind, NULL);
}

/*****************************************************************************
* FUNCTION
*   rdabt_write_scan_enable_cb
* DESCRIPTION
*   rdabt_write_scan_enable_cb
* PARAMETERS
*   task_index  IN   index of the taks
* RETURNS
*   TRUE if reset successfully
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/

void rdabt_set_discoverable(kal_uint8 flag)
{
    kal_uint8 mode = RDABT_NONDISCOVERABLE;
    bt_bm_write_scanenable_mode_cnf_struct *param_ptr;
    switch(flag)
    {
        case BTBM_ADP_P_OFF_I_OFF:
            mode = RDABT_NONDISCOVERABLE;
            break;
        case BTBM_ADP_P_OFF_I_ON:
            mode = RDABT_GENERALDISCOVERY;
            break;
        case BTBM_ADP_P_ON_I_OFF:
            mode = RDABT_NONDISCOVERABLE;
            break;
        case BTBM_ADP_P_ON_I_ON:
            mode = RDABT_GENERALDISCOVERY;
            break;
        case BTBM_ADP_P_ON_I_ON_L:
            mode = RDABT_LIMITEDDISCOVERY;
            break;
    }
    kal_prompt_trace(1,"rdabt_set_discoverable ,mode = %d, flag=%d",mode,flag);
    MGR_SetDiscoverableMode(mode);

    param_ptr = (bt_bm_write_scanenable_mode_cnf_struct *)construct_local_para(
                    (kal_uint16)sizeof(bt_bm_write_scanenable_mode_cnf_struct), TD_CTRL);
    param_ptr->result = 0;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF, param_ptr, NULL);
#ifdef BTPHONE_RECOGNITION_CALL
    {
        extern void rdabt_hfp_Voice_Recognition(void);
        rdabt_hfp_Voice_Recognition();
    }
#endif
}



void rdabt_set_SecurityMode(kal_uint8 mode)
{
    int temp = mode?MGR_SERVICELEVEL_ENFORCED:MGR_NOSECURITY;
    MGR_SetSecurityMode(temp);
}


void rdabt_mgr_msg_hdler(ilm_struct *message)
{
    switch(message->msg_id)
    {
        case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ:
        {
            bt_bm_write_scanenable_mode_req_struct *msg_p = (bt_bm_write_scanenable_mode_req_struct *)message->local_para_ptr;
            rdabt_set_discoverable(msg_p->modenotconnected);
            break;
        }
        case MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ:
        {
            rdabt_get_local_addr_req(0);
            break;
        }

        case MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ:
        {
            bt_bm_write_local_name_cnf_struct *param_ptr;

            bt_bm_write_local_name_req_struct *msg_p = (bt_bm_write_local_name_req_struct *)message->local_para_ptr;
            param_ptr = (bt_bm_write_local_name_cnf_struct *)construct_local_para(
                            (kal_uint16)sizeof(bt_bm_write_local_name_cnf_struct), TD_CTRL);
            param_ptr->result = 0;
            MGR_ChangeLocalName(msg_p->name);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF, (void *)param_ptr, NULL);

            break;
        }

        case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ:
        {
            bt_bm_write_authentication_mode_req_struct *msg_p = (bt_bm_write_authentication_mode_req_struct *)message->local_para_ptr;
            bt_bm_write_authentication_mode_cnf_struct *param_ptr = (bt_bm_write_authentication_mode_cnf_struct*)construct_local_para(
                        (kal_uint16)sizeof(bt_bm_write_authentication_mode_cnf_struct), TD_CTRL);
            rdabt_set_SecurityMode(msg_p->mode);
            param_ptr->result = 0;
            //param_ptr->security_level = msg_p->mode;//?MGR_SERVICELEVEL_ENFORCED:MGR_NOSECURITY;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF, (void *)param_ptr, NULL);
            break;
        }

        case MSG_ID_BT_BM_LINK_ALLOW_REQ:
        {

            kal_prompt_trace(1,"rdabt_mgr_msg_hdler MSG_ID_BT_BM_LINK_ALLOW_REQ process");
            MGR_SetConnectableMode(RDABT_CONNECTABLE);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_ALLOW_CNF, NULL, NULL);
            break;
        }

        case MSG_ID_BT_BM_LINK_DISALLOW_REQ:

            kal_prompt_trace(1,"MSG_ID_BT_BM_LINK_DISALLOW_REQ MSG_ID_BT_BM_LINK_ALLOW_REQ process");
            MGR_SetConnectableMode(RDABT_NONCONNECTABLE);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_DISALLOW_CNF, NULL, NULL);
            break;

        case MSG_ID_BT_BM_DISCOVERY_REQ:
        {
            extern boolean rdabt_mgr_GetAll_ACL_Status(void);
            bt_bm_discovery_req_struct *msg_p = (bt_bm_discovery_req_struct *)message->local_para_ptr;
            int  status = 0x06;

            kal_prompt_trace(1, "cod =%x, discovery_mode=%d",msg_p->cod, msg_p->discovery_mode);
            if(rdabt_mgr_GetAll_ACL_Status())

            {
                rdabt_link_discovery_cnf((void*)status);
                inquiry_cancel = 0;
                break;
            }
            rdabt_write_pskey(0x24, rdabt_pskey_rf_setting_inquiry, sizeof(rdabt_pskey_rf_setting_inquiry));
            sxr_Sleep(5 MS_WAITING);
            status = MGR_Inquiry(msg_p ->inquiry_length, 0x9E8B33, msg_p->cod, msg_p->inquiry_number);
            if( status != RDABT_NOERROR)
            {
                rdabt_link_discovery_cnf((void*)status);
            }
            inquiry_cancel = 0;
            break;
        }

        case MSG_ID_BT_BM_READ_REMOTE_NAME_REQ:
        {
            t_bdaddr address;
            bt_bm_read_remote_name_req_struct *msg_p = (bt_bm_read_remote_name_req_struct *)message->local_para_ptr;
            kal_prompt_trace(1, "receive MSG_ID_BT_BM_READ_REMOTE_NAME_REQ");

            address = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);
            MGR_GetRemoteName(address);
        }
        break;

        case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_NOT_AUTO_REQ:
            //rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_NOT_AUTO_CNF, NULL, NULL);
            break;

        case MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ:
        {
            inquiry_cancel = 1;
            kal_prompt_trace(1, "receive MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ");
            MGR_InquiryCancel();
            break;
        }

        case MSG_ID_BT_BM_BONDING_REQ:
        {
            bt_bm_bonding_req_struct *msg_p = (bt_bm_bonding_req_struct *)message->local_para_ptr;
            t_bdaddr tempbd;
            kal_uint8 i;
            bonding_role = 1;       //we sent the bonding request....
            tempbd = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);
            sdp_search_sdap.bd_addr = msg_p->bd_addr;
            sdp_search_sdap.sdap_len= msg_p->sdap_len;
            for(i= 0; i<BTBM_ADP_MAX_SDAP_UUID_NO; i++)
                sdp_search_sdap.sdap_uuid[i] = msg_p->sdap_uuid[i];

            //memcpy(&sdp_search_sdap,&msg_p->sdap_len,sizeof(sdp_search_sdap_struct));
            i = MGR_InitBonding(tempbd);//, rdabt_reply_rsp_cb);

            kal_prompt_trace(1,"MGR_InitBonding return %d", i);
            break;
        }

        case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_RSP:
        {
            bt_bm_link_connect_accept_rsp_struct* msg_p = (bt_bm_link_connect_accept_rsp_struct*)message->local_para_ptr;
            t_bdaddr address = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);
#if 1
            kal_prompt_trace(1,"MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_RSP result %d", msg_p->result);
            MGR_ConnectAcceptRsp(msg_p->result, address);
#else
            if(msg_p->result == 0)
            {
                t_MGR_DeviceEntry *devEntry;
                MGR_FindDeviceRecord(address, &devEntry);
                rdabt_send_pin_code_ind(devEntry);
            }
            else
            {
                MGR_PINResponse(address,  0, NULL, 0);
            }
#endif
            break;
        }
        /*
            case MSG_ID_BT_BM_LINK_KEY_RSP:
            {
                bt_bm_link_key_rsp_struct *msg_p = (bt_bm_link_key_rsp_struct *)message->local_para_ptr;
                t_bdaddr tempbd = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);

                MGR_LinkKeyResponse(tempbd, msg_p->key_valid, msg_p ->link_key,  msg_p ->key_type);
                break;
            }
        */
        case MSG_ID_BT_BM_PIN_CODE_RSP:
        {
            bt_bm_pin_code_rsp_struct *msg_p = (bt_bm_pin_code_rsp_struct *)message->local_para_ptr;
            t_bdaddr tempbd = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);

            if(msg_p->pin_len==0)
                MGR_PINResponse(tempbd,0, NULL,  0);///PIN CODE Negative
            else
                MGR_PINResponse(tempbd,1, msg_p ->pin_code,  msg_p ->pin_len);///PIN CODE Negative
            break;
        }

            /* case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_RSP:
             {
                 // do nothing
               ///  break;
             }

             case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_CANCEL_REQ:
             {
                 // do nothing
                 break;
             }*/
#ifdef __MMI_BT_SIMPLE_PAIR__
        case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP:
        {
            bt_bm_security_user_confirm_rsp_struct *msg_p = (bt_bm_security_user_confirm_rsp_struct *)message->local_para_ptr;
            t_bdaddr tempbd = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);
            kal_prompt_trace(1,"rdabt_mgr_msg_hdler MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP msg_p->accept=%d",msg_p->accept);
            MGR_ConfirmResponse(tempbd, msg_p->accept);
            break;
        }
#endif
        case MSG_ID_BT_BM_SERVICE_SEARCH_REQ:
        {
            int i;
            bt_bm_service_search_req_struct *msg_p = (bt_bm_service_search_req_struct *)message->local_para_ptr;
            kal_prompt_trace(1,"rdabt_mgr_msg_hdler MSG_ID_BT_BM_SERVICE_SEARCH_REQ");
            sdp_search_sdap.bd_addr= msg_p->bd_addr;
            sdp_search_sdap.sdap_len= msg_p->sdap_len;
            for(i= 0; i<BTBM_ADP_MAX_SDAP_UUID_NO; i++)
                sdp_search_sdap.sdap_uuid[i] = msg_p->sdap_uuid[i];
            rdabt_SDP_serviceSearch(rdabt_bm_bdaddr_to_rdabt(msg_p->bd_addr));
            break;
        }

        case MSG_ID_BT_BM_DELETE_TRUST_REQ:
        {
            bt_bm_delete_trust_req_struct * msg_p = (bt_bm_delete_trust_req_struct*)message->local_para_ptr;
            bt_bm_delete_trust_cnf_struct *delete_cnf;
            int status = 0;
            t_bdaddr    address;

            t_MGR_DeviceEntry *devEntry;
            delete_cnf = (bt_bm_delete_trust_cnf_struct*) construct_local_para(sizeof(bt_bm_delete_trust_cnf_struct), TD_CTRL);

            address = rdabt_bm_bdaddr_to_rdabt(msg_p ->bd_addr);

            MGR_FindDeviceRecord(address, &devEntry);
            if(devEntry)
                MGR_RemoveDeviceRecord(devEntry);
            rdabt_del_paired_device_info(msg_p->bd_addr, 0);

            delete_cnf->bd_addr = msg_p->bd_addr;
            delete_cnf->result = status;
            rdabt_send_msg_up( MOD_MMI,  MSG_ID_BT_BM_DELETE_TRUST_CNF, (void *)delete_cnf,  NULL);

            break;
        }
        default:
            break;

    }
}


const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} mgr_msg_hdlr_table[] =
{
    {MGR_DISCOVERY_RESULT_IND,  rdabt_link_discovery_result_ind},
    {MGR_DISCOVERY_CNF,rdabt_link_discovery_cnf},
    {MGR_LINK_KEY_IND,rdabt_link_key_ind},
    {MGR_PIN_CODE_IND,rdabt_link_pin_code_ind},
    {MGR_OOB_DATA_IND,rdabt_link_oob_data_ind},
    {MGR_PASSKEY_IND,rdabt_link_passkey_ind},
    {MGR_CONFIRM_IND,rdabt_link_confirm_ind},
    {MGR_BONDING_CNF,rdabt_link_bonding_cnf},
    {MGR_SCO_CONNECT_IND,rdabt_sco_connect_ind},
    {MGR_SCO_DISCONNECT_IND,rdabt_sco_disconnect_ind},
    {SDP_SERVICE_SEARCH_CNF,rdabt_ServiceSearch_cb},
    {MGR_GET_REMOTE_NAME_CNF,rdabt_Remote_name_cb},
    {MGR_ACL_CONNECT_IND,rdabt_acl_connect_ind},

};

void rdabt_mgr_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(mgr_msg_hdlr_table)/sizeof(mgr_msg_hdlr_table[0]);
    //kal_uint16 type = (mi & 0x00ff);
    for(I=0; I<n; I++)
    {

        //  kal_prompt_trace(1, "rdabt_mgr_int_msg_hdlr type=0x%x,mgr_msg_hdlr_table[I].type =0x%x",mi,mgr_msg_hdlr_table[I].type );
        if( mi==mgr_msg_hdlr_table[I].type )
        {
            mgr_msg_hdlr_table[I].handler( mv );

            break;
        }
    }

    //ASSERT( I<N ); // not supported by the avadp
}

void CONVERT_BDADDR2ARRAY(u_int8 *dest, u_int32 lap, u_int8 uap, u_int16 nap)
{
    dest[0] = (u_int8)(lap & 0xFF);
    dest[1] = (u_int8)((lap>>8) & 0xFF);
    dest[2] = (u_int8) (lap>>16);
    dest[3] = (uap);
    dest[4] = (u_int8) (nap & 0xFF);
    dest[5] = (u_int8) (nap>>8);

}

u_int8 *BTBMGetLocalStoredName(u_int8 *bdAddr)
{
    t_bdaddr bdaddr;
    t_MGR_DeviceEntry *devEntry;
    memcpy(bdaddr.bytes, bdAddr, 6);
    MGR_FindDeviceRecord(bdaddr, &devEntry);

    return devEntry->name;
}

//#ifdef __SUPPORT_BT_PHONE__
void rdabt_mgr_connect_sco(module_type module, t_bdaddr address, u_int16 cid)
{
    t_api result;
    sco_module = MOD_MED;//module;
    sco_address = address;
    sco_cid = cid;
    result = MGR_SetupSynchronous(address);
    //   result = MGR_AddSco_req(address);

    kal_prompt_trace(1, "rdabt_mgr_connect_sco result=%d,addr:%x,%x,%x,%x,MOD_MED=0x%x,module=0x%x"
                     ,result,address.bytes[0],address.bytes[1],address.bytes[2],address.bytes[3],module,MOD_MED);
    if(result != RDABT_PENDING)
    {
        rdabt_sco_connect_cnf(result);
    }
    //HF_Call_Request(HF_CALL_START,1);

}

void rdabt_mgr_disconnect_sco(module_type module, t_bdaddr address, u_int16 cid,u_int8 speed_on)
{
    t_api result;
    result = MGR_ReleaseSynchronous(sco_index);
//last be used ?
    sco_index = 0xffff;

    kal_prompt_trace(1, "rdabt_mgr_disconnect_sco MGR_ReleaseSynchronous() return resut=0x%x,speed_on=%d",result,speed_on);
    if(result != RDABT_PENDING)
    {
        rdabt_sco_disconnect_cnf(result);
    }
    if(!speed_on)
        HF_Call_Request(HF_CALL_RELEASE,1);
}
//#endif


