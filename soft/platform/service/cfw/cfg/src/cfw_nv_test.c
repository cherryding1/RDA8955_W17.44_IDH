#if defined(CFG_NEW_MECHANISM_TEST)
#include <sul.h>
#include "cfw_cfg.h"

CFW_NV  tst_cfw_nv_r;
CFW_NV  tst_cfw_nv_w;
extern CFW_NV g_DefCfwNv;
extern CFW_NV g_CfwNv;
NW_PLMN tst_plmn[2] = {{0,}};

#define Tst_Assert(bl, format, ...)         \
{if (!(bl)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);       \
}}


UINT8 tst_index = 0;

void tst_set_0(UINT8* pfield, UINT32 size)
{
    typedef struct
    {
        UINT32 len;
        UINT8 buff[128];
    }TEMP_TYPE;

    UINT32 i;
    TEMP_TYPE* ptmp;

    ptmp = (TEMP_TYPE*)pfield;
    if(size == 0)
    {
        Tst_Assert(0,"size = 0");
    }
    else if(size == 1)
    {
        *pfield = tst_index ++ ;
    }
    else if(size == 2)
    {
        *pfield = tst_index ++ ;
        *(pfield +1) = tst_index ++ ;
    }
    else if(size == 3)
    {
        Tst_Assert(0,"size = 0");
    }
    else if(size == 4)
    {
        *pfield = tst_index ++ ;
        *(pfield + 1) = tst_index ++ ;
        *(pfield + 2) = tst_index ++ ;
        *(pfield + 3) = tst_index ++ ;
    }
    else // size > 4
    {
        ptmp->len = (size - 4);
        for(i = 0; i < (size - 4); i++)
        {
            ptmp->buff[i] = tst_index ++;
        }
    }
}
void tst_set_1(UINT8* pfield, UINT32 size, UINT32 Cnt)
{
    UINT32 i;
    UINT8* p;

    for(i = 0; i < Cnt; i++)
    {
        p = pfield + (size/Cnt) * i;
        tst_set_0(p,size);
    }
}

void tst_set_2(UINT8* pfield, UINT32 size, UINT32 Cnt1, UINT32 Cnt2)
{
    UINT32 i,j;
    UINT8* p;

    for(i = 0; i < Cnt1; i++)
    {
        for(j = 0; j < Cnt2; j++)
        {
            p = pfield + (size/Cnt1/Cnt2) * i * j;
            tst_set_0(p,size);
        }
    }
}

void tst_cfw_nv_init(void)
{
    CFW_NV* pNv;
    UINT8 tst_ind = 0;
    UINT32 i,n,m;

    pNv = &tst_cfw_nv_w;
    SUL_MemSet8((UINT8*)&tst_cfw_nv_r,0,sizeof(CFW_NV));
    SUL_MemSet8((UINT8*)&tst_cfw_nv_w,0,sizeof(CFW_NV));

    // 0. version
    pNv->version = CFW_NV_VERSION;

    tst_ind = 0;
    // 1. tm_format
    pNv->tm_format.len = TM_FAORMAT_LEN;
    for(i = 0; i < TM_FAORMAT_LEN; i++)
        pNv->tm_format.format[i] = tst_ind ++;

    // 2. tm_zone
    pNv->tm_zone.len = TM_ZONE_LEN;
    for(i = 0; i < TM_ZONE_LEN; i++)
        pNv->tm_zone.zone[i] = tst_ind ++;

    // 3. tm_alarms
    pNv->tm_alarms.len = TM_ALARMS_LEN;
    for(i = 0; i < TM_ALARMS_LEN; i++)
        pNv->tm_alarms.alarms[i] = tst_ind ++;

    // 4. tm_base
    pNv->tm_base.len = TM_BASE_LEN;
    for(i = 0; i < TM_BASE_LEN; i++)
        pNv->tm_base.base[i] = tst_ind ++;

    // 5. tm_time
    pNv->tm_time.len = TM_TIME_LEN;
    for(i = 0; i < TM_TIME_LEN; i++)
        pNv->tm_time.time[i] = tst_ind ++;

    // 6. tm_alarml
    for(n = 0; n < TM_ALARM_NUM; n++)
    {
        pNv->tm_alarml[n].len = TM_ALARML_LEN;
        for(i = 0; i < TM_ALARML_LEN; i++)
            pNv->tm_alarml[n].alaml[i] = tst_ind ++;
    }

    // 7. fs_pstatus
    pNv->fs_pstatus =  tst_ind ++;

    // 8. sms_param
    pNv->sms_param.len = SMS_PARAM_LEN;
    for(i = 0; i < SMS_PARAM_LEN; i++)
        pNv->sms_param.param[i] = tst_ind ++;

    // 9. cm_public
    pNv->cm_public  = (INT32) tst_ind ++;

    // 10. cm_ccwa_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->cm_ccwa_sim[n] = tst_ind ++;
    }

    // 11. cm_clir_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->cm_clir_sim[n] = tst_ind ++;
    }

    // 12. cm_colp_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->cm_colp_sim[n] = tst_ind ++;
    }

    // 13. gen_mem_info
    pNv->gen_mem_info.len = MEM_INFO_LEN;
    for(i = 0; i < MEM_INFO_LEN; i++)
        pNv->gen_mem_info.info[i] = tst_ind ++;

    // 14. gen_ind_event
    pNv->gen_ind_event = tst_ind ++;

    // 15. gen_imsi_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->gen_imsi_sim[n].len = GEN_IMSI_LEN;
        for(i = 0; i < GEN_IMSI_LEN; i++)
            pNv->gen_imsi_sim[n].imsi[i] = tst_ind ++;
    }

     // 16. nw_sim_work_mode
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_work_mode[n] = tst_ind ++;
    }

    // 17. nw_sim_flight_mode
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_flight_mode[n] = tst_ind ++;
    }

    // 18. nw_sim_freq_band
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_freq_band[n] = tst_ind ++;
    }

    // 19. nw_sim_umts_band
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_umts_band[n] = tst_ind ++;
    }

    // 20. nw_sim_plmn
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_plmn[n].len = NW_PLMN_LEN;
        for(i = 0; i < NW_PLMN_LEN; i++)
            pNv->nw_sim_plmn[n].plmn[i] = tst_ind ++;
    }

    // 21. nw_sim_ba_count
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_ba_count[n] = tst_ind ++;
    }

    // 22. nw_sim_arfce_list
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_arfce_list[n].len = ARFCN_LIST_LEN;
        for(i = 0; i < ARFCN_LIST_LEN; i++)
            pNv->nw_sim_arfce_list[n].arfcn[i] = tst_ind ++;
    }

    // 23. nw_sim_store_last
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->nw_sim_store_last[n] = tst_ind ++;
    }

    // 24. gprs_up_sum_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->gprs_up_sum_sim[n] = tst_ind ++;
    }

    // 25. gprs_dwn_sum_sim
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->gprs_dwn_sum_sim[n] = tst_ind ++;
    }

    // 26. nw_ipr
    pNv->nw_ipr = tst_ind ++;

    // 27. sim_card_switch
    pNv->sim_card_switch = tst_ind ++;

    // 28. sim_soft_reset
    pNv->sim_soft_reset = tst_ind ++;

    // 29. sim_is_fst_pon
    pNv->sim_is_fst_pon = tst_ind ++;

    // 30. sms_storage
    for(n = 0; n < SMS_STORAGE_NUM; n++)
    {
        pNv->sms_storage[n] = tst_ind ++;
    }

    // 31. sms_sim_over_ind
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->sms_sim_over_ind[n] = tst_ind ++;
    }

    // 32. sms_full_for_new
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->sms_full_for_new[n] = tst_ind ++;
    }

    // 33. sms_def_param_ind
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->sms_def_param_ind[n] = tst_ind ++;
    }

    // 34. sms_sim_paraml
    for(n = 0; n < SIM_NUM; n++)
    {
        for(m = 0; m < SMS_SIM_PARAM_NUM; m++)
        {
            pNv->sms_sim_paraml[n][m].len = SMS_SIM_PARAM_LEN;
            for(i = 0; i < SMS_SIM_PARAM_LEN; i++)
                pNv->sms_sim_paraml[n][m].sim_param[i] = tst_ind ++;
        }
    }

    // 35. sms_sim_option
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->sms_sim_option[n] = tst_ind ++;
    }

    // 36. sms_sim_optstor
    for(n = 0; n < SIM_NUM; n++)
    {
        pNv->sms_sim_optstor[n] = tst_ind ++;
    }
    // 37. emod_param
    pNv->emod_param = tst_ind ++;

    // 38. emod_nwnm
    pNv->emod_nwnm.len = EMOD_NWNM_LEN;
    for(i = 0; i < EMOD_NWNM_LEN; i++)
         pNv->emod_nwnm.nwnm[i] = tst_ind ++;

    // 39. emod_sign
    pNv->emod_sign = tst_ind ++;

    // 40. emod_simnml
    for(n = 0; n < EMOD_SIMNML_NUM; n++)
    {
        pNv->emod_simnml[n].len = EMOD_SIMNML_LEN;
        for(i = 0; i < EMOD_SIMNML_LEN; i++)
            pNv->emod_simnml[n].simnm[i] = tst_ind ++;
    }

    // 41. tcpip_dns_adrl
    for(n = 0; n < DNS_ADRL_NUM; n++)
    {
        pNv->tcpip_dns_adrl[n].len = DNS_ADRL_LEN;
        for(i = 0; i < DNS_ADRL_LEN; i++)
            pNv->tcpip_dns_adrl[n].adrl[i] = tst_ind ++;
    }

     // 42. jmetoc_apn
    pNv->jmetoc_apn.len = JMETOC_APN_LEN;
    for(i = 0; i < JMETOC_APN_LEN; i++)
        pNv->jmetoc_apn.apn[i] = tst_ind ++;

    // 43. jmetoc_user
    pNv->jmetoc_user.len = JMETOC_USER_LEN;
    for(i = 0; i < JMETOC_USER_LEN; i++)
        pNv->jmetoc_user.user[i] = tst_ind ++;

    // 44. jmetoc_pwd
    pNv->jmetoc_pwd.len = JMETOC_PWD_LEN;
    for(i = 0; i < JMETOC_PWD_LEN; i++)
        pNv->jmetoc_pwd.pwd[i] = tst_ind ++;

    // 45. jmetoc_proxy_adr
    pNv->jmetoc_proxy_adr.len = JMETOC_PROXY_ADR_LEN;
    for(i = 0; i < JMETOC_PROXY_ADR_LEN; i++)
        pNv->jmetoc_proxy_adr.proxy[i] = tst_ind ++;

    // 46. jmetoc_port
    pNv->jmetoc_port = tst_ind ++;

    // 47. jmetoc_apn_num
    pNv->jmetoc_apn_num = tst_ind ++;

    // 48. jmetoc_apn_num
    pNv->dbs_seed.len = DBS_SEED_LEN;
    for(i = 0; i < DBS_SEED_LEN; i++)
         pNv->dbs_seed.seed[i] = tst_ind ++;

    // 49. sequence
    pNv->sequence ++;

    // 50. crc
    pNv->crc = 0xaabbccdd;

}


void tst_send_event(UINT32 evnt)
{
    sxr_Sleep(16);
    hal_HstSendEvent(evnt);
    sxr_Sleep(16);
}

void tst_cfw_nv_read(void)
{
    INT32 result;
    CFW_NV* pNv_r;
    UINT32 i,j;
    UINT32 index = 0;

    pNv_r = &tst_cfw_nv_r;

    // tm_format
    pNv_r->tm_format.len = TM_FAORMAT_LEN;
    result = NV_GetTmFormat(pNv_r->tm_format.format,&pNv_r->tm_format.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000001);
    }

    // tm_zone
    pNv_r->tm_zone.len = TM_ZONE_LEN;
    result = NV_GetTmZone(pNv_r->tm_zone.zone,&pNv_r->tm_zone.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000002);
    }
    // tm_alarm
    pNv_r->tm_alarms.len = TM_ALARMS_LEN;
    result = NV_GetTmAlarm(pNv_r->tm_alarms.alarms,&pNv_r->tm_alarms.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000003);
    }
    pNv_r->tm_base.len = TM_BASE_LEN;
    result = NV_GetTmBase(pNv_r->tm_base.base,&pNv_r->tm_base.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000004);
    }
    pNv_r->tm_time.len = TM_TIME_LEN;
    result = NV_GetTmTime(pNv_r->tm_time.time,&pNv_r->tm_time.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000005);
    }

    for(i = 0; i < TM_ALARM_NUM; i++)
    {
        pNv_r->tm_alarml[i].len = TM_ALARML_LEN;
        result = NV_GetTmAlarml(i,pNv_r->tm_alarml[i].alaml,&pNv_r->tm_alarml[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000006);
        }
    }

    result = NV_GetFsStatus(&pNv_r->fs_pstatus);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000007);
    }
    pNv_r->sms_param.len = SMS_PARAM_LEN;
    result = NV_GetSmsParam(pNv_r->sms_param.param,&pNv_r->sms_param.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000008);
    }
    result = NV_GetCmPublic(&pNv_r->cm_public);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000009);
    }
    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetCmCcwaSim(i,&pNv_r->cm_ccwa_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000000a);
        }
    }
    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetCmClirSim(i,&pNv_r->cm_clir_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000000b);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetCmColpSim(i,&pNv_r->cm_colp_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000000c);
        }
    }

    pNv_r->gen_mem_info.len = MEM_INFO_LEN;
    result = NV_GetGenMemInfo(pNv_r->gen_mem_info.info,&pNv_r->gen_mem_info.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000000d);
    }
    result = NV_GetGenIndEvent(&pNv_r->gen_ind_event);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000000e);
    }
    for(i = 0; i < SIM_NUM; i++)
    {
        pNv_r->gen_imsi_sim[i].len = GEN_IMSI_LEN;
        result = NV_GetGenImsiSim(i,pNv_r->gen_imsi_sim[i].imsi,&pNv_r->gen_imsi_sim[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000000f);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimWorkMode(i,&pNv_r->nw_sim_work_mode[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000010);
        }
    }

   for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimFlightMode(i,&pNv_r->nw_sim_flight_mode[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000011);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimFregBand(i,&pNv_r->nw_sim_freq_band[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000012);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimUmtsBand(i,&pNv_r->nw_sim_umts_band[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000013);
        }
    }

    memset(&tst_plmn,0,sizeof(tst_plmn));
    for(i = 0; i < SIM_NUM; i++)
    {
        pNv_r->nw_sim_plmn[i].len = NW_PLMN_LEN;
        tst_plmn[i].len = NW_PLMN_LEN;
        result = NV_GetNwSimPlmn(i,tst_plmn[i].plmn,&tst_plmn[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000014);
        }

        result = NV_GetNwSimPlmn(i,pNv_r->nw_sim_plmn[i].plmn,&pNv_r->nw_sim_plmn[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000014);
        }
    }


    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimBaCount(i,&pNv_r->nw_sim_ba_count[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000015);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        pNv_r->nw_sim_arfce_list[i].len = ARFCN_LIST_LEN;
        result = NV_GetNwSimArfceList(i,pNv_r->nw_sim_arfce_list[i].arfcn,&pNv_r->nw_sim_arfce_list[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000016);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetNwSimStoreLast(i,&pNv_r->nw_sim_store_last[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000017);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetGprsUpSumSim(i,&pNv_r->gprs_up_sum_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000018);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetGprsDwnSumSim(i,&pNv_r->gprs_dwn_sum_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000019);
        }
    }

    result = NV_GetNwIpr(&pNv_r->nw_ipr);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000001a);
    }
    result = NV_GetSimCardSwitch(&pNv_r->sim_card_switch);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000001b);
    }
    result = NV_GetSimSoftReset(&pNv_r->sim_soft_reset);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000001c);
    }
    result = NV_GetSimFstPon(&pNv_r->sim_is_fst_pon);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000001d);
    }
    for(i = 0; i < SMS_STORAGE_NUM; i++)
    {
        result = NV_GetSmsStorage(i,&pNv_r->sms_storage[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000001e);
        }
    }

     for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetSmsOverInd(i,&pNv_r->sms_sim_over_ind[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x1000001f);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetSmsFullForNew(i,&pNv_r->sms_full_for_new[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000020);
        }
    }
    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetSmsDefParamInd(i,&pNv_r->sms_def_param_ind[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000021);
        }

    }

    for(i = 0; i < SIM_NUM; i++)
    {
        for(j = 0; j < SMS_SIM_PARAM_NUM; j++)
        {
            pNv_r->sms_sim_paraml[i][j].len = SMS_SIM_PARAM_LEN;
            result = NV_GetSmsSimParaml(i,j,pNv_r->sms_sim_paraml[i][j].sim_param,&pNv_r->sms_sim_paraml[i][j].len);
            if(result)
            {
                Tst_Assert(0,"Nv entry Get fail,result = %d",result);
            }
        else
        {
            tst_send_event(0x10000022);
        }

        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetSmsSimOption(i,&pNv_r->sms_sim_option[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000023);
        }

    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_GetSmsSimOptStor(i,&pNv_r->sms_sim_optstor[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000024);
        }

    }

    result = NV_GetEmodParam(&pNv_r->emod_param);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000025);
    }

    pNv_r->emod_nwnm.len = EMOD_NWNM_LEN;
    result = NV_GetEmodNwnm(pNv_r->emod_nwnm.nwnm,&pNv_r->emod_nwnm.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000026);
    }

    result = NV_GetEmodSign(&pNv_r->emod_sign);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x10000027);
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        pNv_r->emod_simnml[i].len = EMOD_SIMNML_LEN;
        result = NV_GetEmodSimnml(i,pNv_r->emod_simnml[i].simnm,&pNv_r->emod_simnml[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000028);
        }

    }

    for(i = 0; i < SIM_NUM; i++)
    {
        pNv_r->tcpip_dns_adrl[i].len = DNS_ADRL_LEN;
        result = NV_GetTcpipDnsAdrl(i,pNv_r->tcpip_dns_adrl[i].adrl,&pNv_r->tcpip_dns_adrl[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Get fail,result = %d",result);
        }
        else
        {
            tst_send_event(0x10000029);
        }

    }

    pNv_r->jmetoc_apn.len = JMETOC_APN_LEN;
    result = NV_GetJmetocApn(pNv_r->jmetoc_apn.apn,&pNv_r->jmetoc_apn.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002a);
    }

    pNv_r->jmetoc_user.len = JMETOC_USER_LEN;
    result = NV_GetJmetocUser(pNv_r->jmetoc_user.user,&pNv_r->jmetoc_user.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002b);
    }

    pNv_r->jmetoc_pwd.len = JMETOC_PWD_LEN;
    result = NV_GetJmetocPwd(pNv_r->jmetoc_pwd.pwd,&pNv_r->jmetoc_pwd.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002c);
    }

    pNv_r->jmetoc_proxy_adr.len = JMETOC_PROXY_ADR_LEN;
    result = NV_GetJmetocAdr(pNv_r->jmetoc_proxy_adr.proxy,&pNv_r->jmetoc_proxy_adr.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002d);
    }

    result = NV_GetJmetocPort(&pNv_r->jmetoc_port);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }

    result = NV_GetJmetocApnNum(&pNv_r->jmetoc_apn_num);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002e);
    }

    pNv_r->dbs_seed.len = DBS_SEED_LEN;
    result = NV_GetDbsSeed(pNv_r->dbs_seed.seed,&pNv_r->dbs_seed.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Get fail,result = %d",result);
    }
    else
    {
        tst_send_event(0x1000002f);
    }

}

void tst_cfw_nv_write(void)
{
    INT32 result;
    CFW_NV* pNv_w;
    UINT8 i,j;

    pNv_w = &tst_cfw_nv_w;

    // tm_format
    result = NV_SetTmFormat(pNv_w->tm_format.format,pNv_w->tm_format.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }
    // tm_zone
    result = NV_SetTmZone(pNv_w->tm_zone.zone,pNv_w->tm_zone.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Setfail,result = %d,field size = %d,len = %d.",result,FIELD_SIZE(CFW_NV,tm_zone),pNv_w->tm_zone.len);
    }

    // tm_alarm
    result = NV_SetTmAlarm(pNv_w->tm_alarms.alarms,pNv_w->tm_alarms.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetTmBase(pNv_w->tm_base.base,pNv_w->tm_base.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetTmTime(pNv_w->tm_time.time,pNv_w->tm_time.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    for(i = 0; i < TM_ALARM_NUM; i++)
    {
        result = NV_SetTmAlarml(i,pNv_w->tm_alarml[i].alaml,pNv_w->tm_alarml[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    result = NV_SetFsStatus(pNv_w->fs_pstatus);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetSmsParam(pNv_w->sms_param.param,pNv_w->sms_param.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetCmPublic(pNv_w->cm_public);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetCmCcwaSim(i,pNv_w->cm_ccwa_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetCmClirSim(i,pNv_w->cm_clir_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetCmColpSim(i,pNv_w->cm_colp_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    result = NV_SetGenMemInfo(pNv_w->gen_mem_info.info,pNv_w->gen_mem_info.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetGenIndEvent(pNv_w->gen_ind_event);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetGenImsiSim(i,pNv_w->gen_imsi_sim[i].imsi,pNv_w->gen_imsi_sim[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimWorkMode(i,pNv_w->nw_sim_work_mode[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

   for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimFlightMode(i,pNv_w->nw_sim_flight_mode[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimFregBand(i,pNv_w->nw_sim_freq_band[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimUmtsBand(i,pNv_w->nw_sim_umts_band[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimPlmn(i,pNv_w->nw_sim_plmn[i].plmn,pNv_w->nw_sim_plmn[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }


    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimBaCount(i,pNv_w->nw_sim_ba_count[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimArfceList(i,pNv_w->nw_sim_arfce_list[i].arfcn,pNv_w->nw_sim_arfce_list[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetNwSimStoreLast(i,pNv_w->nw_sim_store_last[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetGprsUpSumSim(i,pNv_w->gprs_up_sum_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetGprsDwnSumSim(i,pNv_w->gprs_dwn_sum_sim[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    result = NV_SetNwIpr(pNv_w->nw_ipr);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetSimCardSwitch(pNv_w->sim_card_switch);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetSimSoftReset(pNv_w->sim_soft_reset);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetSimFstPon(pNv_w->sim_is_fst_pon);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    for(i = 0; i < SMS_STORAGE_NUM; i++)
    {
        result = NV_SetSmsStorage(i,pNv_w->sms_storage[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

     for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetSmsOverInd(i,pNv_w->sms_sim_over_ind[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetSmsFullForNew(i,pNv_w->sms_full_for_new[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }
    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetSmsDefParamInd(i,pNv_w->sms_def_param_ind[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        for(j = 0; j < SMS_SIM_PARAM_NUM; j++)
        {
            result = NV_SetSmsSimParaml(i,j,pNv_w->sms_sim_paraml[i][j].sim_param,pNv_w->sms_sim_paraml[i][j].len);
            if(result)
            {
                Tst_Assert(0,"Nv entry Set fail,result = %d",result);
            }
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetSmsSimOption(i,pNv_w->sms_sim_option[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetSmsSimOptStor(i,pNv_w->sms_sim_optstor[i]);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    result = NV_SetEmodParam(pNv_w->emod_param);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetEmodNwnm(pNv_w->emod_nwnm.nwnm,pNv_w->emod_nwnm.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetEmodSign(pNv_w->emod_sign);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetEmodSimnml(i,pNv_w->emod_simnml[i].simnm,pNv_w->emod_simnml[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    for(i = 0; i < SIM_NUM; i++)
    {
        result = NV_SetTcpipDnsAdrl(i,pNv_w->tcpip_dns_adrl[i].adrl,pNv_w->tcpip_dns_adrl[i].len);
        if(result)
        {
            Tst_Assert(0,"Nv entry Set fail,result = %d",result);
        }
    }

    result = NV_SetJmetocApn(pNv_w->jmetoc_apn.apn,pNv_w->jmetoc_apn.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetJmetocUser(pNv_w->jmetoc_user.user,pNv_w->jmetoc_user.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetJmetocPwd(pNv_w->jmetoc_pwd.pwd,pNv_w->jmetoc_pwd.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }
    result = NV_SetJmetocAdr(pNv_w->jmetoc_proxy_adr.proxy,pNv_w->jmetoc_proxy_adr.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetJmetocPort(pNv_w->jmetoc_port);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetJmetocApnNum(pNv_w->jmetoc_apn_num);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }

    result = NV_SetDbsSeed(pNv_w->dbs_seed.seed,pNv_w->dbs_seed.len);
    if(result)
    {
        Tst_Assert(0,"Nv entry Set fail,result = %d",result);
    }
}

void tst_cfw_nv_reset(void)
{
    UINT8* p;
    UINT8* p1;

    p = (UINT8*) &g_DefCfwNv;
    p1 = (UINT8*) &tst_cfw_nv_w;
    SUL_MemCopy8(p1,p,sizeof(CFW_NV));
    tst_cfw_nv_write();
}

BOOL tst_FiledCmp(CFW_NV *pNv1,CFW_NV * pNv2)
{
    UINT32 i,j;

    if((pNv1->tm_format.len != pNv2->tm_format.len) ||  memcmp((UINT8*)pNv1->tm_format.format , (UINT8*)pNv2->tm_format.format, pNv1->tm_format.len)  != 0) Tst_Assert(0,"tm_format          ");
    if(pNv1->tm_zone.len != pNv2->tm_zone.len ||  memcmp(pNv1->tm_zone.zone, pNv2->tm_zone.zone, pNv1->tm_zone.len)  != 0) Tst_Assert(0,"tm_zone          ");
    if(pNv1->tm_alarms.len != pNv2->tm_alarms.len ||  memcmp(pNv1->tm_alarms.alarms, pNv2->tm_alarms.alarms, pNv1->tm_alarms.len)  != 0) Tst_Assert(0,"tm_alarms          ");
    if(pNv1->tm_base.len != pNv2->tm_base.len ||  memcmp(pNv1->tm_base.base, pNv2->tm_base.base, pNv1->tm_base.len)  != 0) Tst_Assert(0,"tm_base          ");
    if(pNv1->tm_time.len != pNv2->tm_time.len ||  memcmp(pNv1->tm_time.time, pNv2->tm_time.time, pNv1->tm_time.len)  != 0) Tst_Assert(0,"tm_time          ");

    for(i = 0; i < TM_ALARM_NUM; i++ )
	if(pNv1->tm_alarml[i].len != pNv2->tm_alarml[i].len ||  memcmp(pNv1->tm_alarml[i].alaml, pNv2->tm_alarml[i].alaml, pNv1->tm_alarml[i].len) != 0) Tst_Assert(0,"tm_alarml          ");

    if(memcmp(&pNv1->fs_pstatus         ,     &pNv2->fs_pstatus               ,sizeof(pNv1->fs_pstatus          )) != 0) Tst_Assert(0,"fs_pstatus         ");
    if(pNv1->sms_param.len != pNv2->sms_param.len ||  memcmp(pNv1->sms_param, pNv2->sms_param ,pNv1->sms_param.len) != 0) Tst_Assert(0,"sms_param          ");
    if(memcmp(&pNv1->cm_public          ,     &pNv2->cm_public                ,sizeof(pNv1->cm_public           )) != 0) Tst_Assert(0,"cm_public          ");
    if(memcmp(&pNv1->cm_ccwa_sim        ,  &pNv2->cm_ccwa_sim              ,sizeof(pNv1->cm_ccwa_sim         )) != 0) Tst_Assert(0,"cm_ccwa_sim        ");
    if(memcmp(&pNv1->cm_clir_sim        ,     &pNv2->cm_clir_sim              ,sizeof(pNv1->cm_clir_sim         )) != 0) Tst_Assert(0,"cm_clir_sim        ");
    if(memcmp(&pNv1->cm_colp_sim        ,     &pNv2->cm_colp_sim              ,sizeof(pNv1->cm_colp_sim         )) != 0) Tst_Assert(0,"cm_colp_sim        ");
    if(pNv1->gen_mem_info.len != pNv2->gen_mem_info.len ||  memcmp(pNv1->gen_mem_info.info, pNv2->gen_mem_info.info, pNv1->gen_mem_info.len)  != 0) Tst_Assert(0,"gen_mem_info          ");
    if(memcmp(&pNv1->gen_ind_event      ,     &pNv2->gen_ind_event            ,sizeof(pNv1->gen_ind_event         )) != 0) Tst_Assert(0,"gen_ind_event      ");
    for(i = 0; i < SIM_NUM; i++ )
        if(pNv1->gen_imsi_sim[i].len != pNv2->gen_imsi_sim[i].len ||  memcmp(pNv1->gen_imsi_sim[i].imsi, pNv2->gen_imsi_sim[i].imsi, pNv1->gen_imsi_sim[i].len)  != 0) Tst_Assert(0,"gen_imsi_sim          ");
    if(memcmp(&pNv1->nw_sim_work_mode   ,     &pNv2->nw_sim_work_mode         ,sizeof(pNv1->nw_sim_work_mode    )) != 0) Tst_Assert(0,"nw_sim_work_mode   ");
    if(memcmp(&pNv1->nw_sim_flight_mode ,     &pNv2->nw_sim_flight_mode       ,sizeof(pNv1->nw_sim_flight_mode    )) != 0) Tst_Assert(0,"nw_sim_flight_mode ");
    if(memcmp(&pNv1->nw_sim_freq_band   ,     &pNv2->nw_sim_freq_band         ,sizeof(pNv1->nw_sim_freq_band    )) != 0) Tst_Assert(0,"nw_sim_freq_band   ");
    if(memcmp(&pNv1->nw_sim_umts_band   ,     &pNv2->nw_sim_umts_band         ,sizeof(pNv1->nw_sim_umts_band    )) != 0) Tst_Assert(0,"nw_sim_umts_band   ");
    for(i = 0; i < SIM_NUM; i++ )
        if(pNv1->nw_sim_plmn[i].len != pNv2->nw_sim_plmn[i].len ||  memcmp(pNv1->nw_sim_plmn[i].plmn, pNv2->nw_sim_plmn[i].plmn, pNv1->nw_sim_plmn[i].len)  != 0) Tst_Assert(0,"nw_sim_plmn          ");
    if(memcmp(&pNv1->nw_sim_ba_count    ,     &pNv2->nw_sim_ba_count          ,sizeof(pNv1->nw_sim_ba_count     )) != 0) Tst_Assert(0,"nw_sim_ba_count    ");
    for(i = 0; i < SIM_NUM; i++ )
        if(pNv1->nw_sim_arfce_list[i].len != pNv2->nw_sim_arfce_list[i].len ||  memcmp(pNv1->nw_sim_arfce_list[i].arfcn, pNv2->nw_sim_arfce_list[i].arfcn, pNv1->nw_sim_arfce_list[i].len)  != 0) Tst_Assert(0,"nw_sim_arfce_list          ");

    if(SUL_MemCompare(&pNv1->nw_sim_store_last  ,     &pNv2->nw_sim_store_last        ,sizeof(pNv1->nw_sim_store_last   )) != 0) Tst_Assert(0,"nw_sim_store_last  ");
    if(SUL_MemCompare(&pNv1->gprs_up_sum_sim    ,     &pNv2->gprs_up_sum_sim          ,sizeof(pNv1->gprs_up_sum_sim     )) != 0) Tst_Assert(0,"gprs_up_sum_sim    ");
    if(SUL_MemCompare(&pNv1->gprs_dwn_sum_sim   ,     &pNv2->gprs_dwn_sum_sim         ,sizeof(pNv1->gprs_dwn_sum_sim    )) != 0) Tst_Assert(0,"gprs_dwn_sum_sim   ");
    if(SUL_MemCompare(&pNv1->nw_ipr             ,     &pNv2->nw_ipr                   ,sizeof(pNv1->nw_ipr              )) != 0) Tst_Assert(0,"nw_ipr             ");
    if(SUL_MemCompare(&pNv1->sim_card_switch    ,     &pNv2->sim_card_switch          ,sizeof(pNv1->sim_card_switch     )) != 0) Tst_Assert(0,"sim_card_switch    ");
    if(SUL_MemCompare(&pNv1->sim_soft_reset     ,     &pNv2->sim_soft_reset           ,sizeof(pNv1->sim_soft_reset      )) != 0) Tst_Assert(0,"sim_soft_reset     ");
    if(SUL_MemCompare(&pNv1->sim_is_fst_pon     ,     &pNv2->sim_is_fst_pon           ,sizeof(pNv1->sim_is_fst_pon      )) != 0) Tst_Assert(0,"sim_is_fst_pon     ");
    if(SUL_MemCompare(&pNv1->sms_storage        ,     &pNv2->sms_storage              ,sizeof(pNv1->sms_storage         )) != 0) Tst_Assert(0,"sms_storage        ");
    if(SUL_MemCompare(&pNv1->sms_sim_over_ind   ,     &pNv2->sms_sim_over_ind         ,sizeof(pNv1->sms_sim_over_ind    )) != 0) Tst_Assert(0,"sms_sim_over_ind   ");
    if(SUL_MemCompare(&pNv1->sms_full_for_new   ,     &pNv2->sms_full_for_new         ,sizeof(pNv1->sms_full_for_new    )) != 0) Tst_Assert(0,"sms_full_for_new   ");
    if(SUL_MemCompare(&pNv1->sms_def_param_ind  ,     &pNv2->sms_def_param_ind        ,sizeof(pNv1->sms_def_param_ind   )) != 0) Tst_Assert(0,"sms_def_param_ind  ");
    for(i = 0; i < SIM_NUM; i++ )
        for(j = 0; j < SMS_SIM_PARAM_NUM; j++ )
            if(pNv1->sms_sim_paraml[i][j].len != pNv2->sms_sim_paraml[i][j].len ||  memcmp(pNv1->sms_sim_paraml[i][j].sim_param, pNv2->sms_sim_paraml[i][j].sim_param, pNv1->sms_sim_paraml[i][j].len)  != 0) Tst_Assert(0,"sms_sim_paraml          ");

    if(SUL_MemCompare(&pNv1->sms_sim_option     ,     &pNv2->sms_sim_option           ,sizeof(pNv1->sms_sim_option      )) != 0) Tst_Assert(0,"sms_sim_option     ");
    if(SUL_MemCompare(&pNv1->sms_sim_optstor    ,     &pNv2->sms_sim_optstor          ,sizeof(pNv1->sms_sim_optstor     )) != 0) Tst_Assert(0,"sms_sim_optstor    ");
    if(SUL_MemCompare(&pNv1->emod_param         ,     &pNv2->emod_param               ,sizeof(pNv1->emod_param          )) != 0) Tst_Assert(0,"emod_param         ");
    if(pNv1->emod_nwnm.len != pNv2->emod_nwnm.len ||  memcmp(pNv1->emod_nwnm.nwnm, pNv2->emod_nwnm.nwnm, pNv1->emod_nwnm.len)  != 0) Tst_Assert(0,"emod_nwnm          ");
    if(SUL_MemCompare(&pNv1->emod_sign          ,     &pNv2->emod_sign                ,sizeof(pNv1->emod_sign           )) != 0) Tst_Assert(0,"emod_sign          ");
    for(i = 0; i < EMOD_SIMNML_NUM; i++ )
        if(pNv1->emod_simnml[i].len != pNv2->emod_simnml[i].len ||  memcmp(pNv1->emod_simnml[i].simnm, pNv2->emod_simnml[i].simnm, pNv1->emod_simnml[i].len)  != 0) Tst_Assert(0,"emod_simnml          ");
    for(i = 0; i < DNS_ADRL_NUM; i++ )
        if(pNv1->tcpip_dns_adrl[i].len != pNv2->tcpip_dns_adrl[i].len ||  memcmp(pNv1->tcpip_dns_adrl[i].adrl, pNv2->tcpip_dns_adrl[i].adrl, pNv1->tcpip_dns_adrl[i].len)  != 0) Tst_Assert(0,"tcpip_dns_adrl          ");
    if(pNv1->jmetoc_apn.len != pNv2->jmetoc_apn.len ||  memcmp(pNv1->jmetoc_apn.apn, pNv2->jmetoc_apn.apn, pNv1->jmetoc_apn.len)  != 0) Tst_Assert(0,"jmetoc_apn          ");
    if(pNv1->jmetoc_user.len != pNv2->jmetoc_user.len ||  memcmp(pNv1->jmetoc_user.user, pNv2->jmetoc_user.user, pNv1->jmetoc_user.len)  != 0) Tst_Assert(0,"jmetoc_user          ");
    if(pNv1->jmetoc_pwd.len != pNv2->jmetoc_pwd.len ||  memcmp(pNv1->jmetoc_pwd.pwd, pNv2->jmetoc_pwd.pwd, pNv1->jmetoc_pwd.len)  != 0) Tst_Assert(0,"jmetoc_pwd          ");
    if(pNv1->jmetoc_proxy_adr.len != pNv2->jmetoc_proxy_adr.len ||  memcmp(pNv1->jmetoc_proxy_adr.proxy, pNv2->jmetoc_proxy_adr.proxy, pNv1->jmetoc_proxy_adr.len)  != 0) Tst_Assert(0,"jmetoc_proxy_adr          ");
    if(SUL_MemCompare(&pNv1->jmetoc_port        ,     &pNv2->jmetoc_port              ,sizeof(pNv1->jmetoc_port         )) != 0) Tst_Assert(0,"jmetoc_port        ");
    if(SUL_MemCompare(&pNv1->jmetoc_apn_num     ,     &pNv2->jmetoc_apn_num           ,sizeof(pNv1->jmetoc_apn_num      )) != 0) Tst_Assert(0,"jmetoc_apn_num     ");
    if(pNv1->dbs_seed.len != pNv2->dbs_seed.len ||  memcmp(pNv1->dbs_seed.seed, pNv2->dbs_seed.seed, pNv1->dbs_seed.len)  != 0) Tst_Assert(0,"dbs_seed          ");

    return TRUE;
}


BOOL tst_check_nv_get(void)
{
    if(tst_FiledCmp(&tst_cfw_nv_r, &g_CfwNv) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL tst_check_nv_set(void)
{
    if(tst_FiledCmp(&tst_cfw_nv_w, &g_CfwNv) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void tst_cfw_nv(void)
{
   // init , set tst_cfw_nv_w
   sxr_Sleep(20);
   hal_HstSendEvent(0xaabb0001);
   tst_cfw_nv_init();
   hal_HstSendEvent(0xaabb0002);
   sxr_Sleep(20);

}


void tst_cfw_nv1(void)
{
   // check nv get
   // read nv entries form g_CfwNv call NV-APIs.
   sxr_Sleep(20);
   hal_HstSendEvent(0xaabb0003);
   tst_cfw_nv_read();
   hal_HstSendEvent(0xaabb0004);
   sxr_Sleep(20);

}

void tst_cfw_nv2(void)
{
   hal_HstSendEvent(0xaabb0005);
   if(tst_check_nv_get())
   {
        hal_HstSendEvent(0xaabb0006);
   }
   else
   {
        hal_HstSendEvent(0xaabb0007);
   }
   sxr_Sleep(20);
}

void tst_cfw_nv3(void)
{

   sxr_Sleep(20);
   hal_HstSendEvent(0xaabb0008);
   tst_cfw_nv_write();
   hal_HstSendEvent(0xaabb0009);
   sxr_Sleep(20);

}

void tst_cfw_nv4(void)
{
   sxr_Sleep(20);
   hal_HstSendEvent(0xaabb000a);
   // cmp(tst_cfw_nv_r , g_CfwNv)
    if(tst_check_nv_set())
   {
        hal_HstSendEvent(0xaabb000b);
   }
   else
   {
        hal_HstSendEvent(0xaabb000c);
   }
   sxr_Sleep(20);
}

#endif
