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
#include <sul.h>
#include "fs/sf_api.h"
#include "cfw_cfg.h"

#define CFW_NV_FILE "cfw_nv.bin"

const CFW_NV g_DefCfwNv =
    {
        //version
        CFW_NV_VERSION,
        //tm_format
        {DEF_TM_FAORMAT_LEN, {0x1a, 0x02, 0x00, 0x00}},
        //tm_zone
        {DEF_TM_ZONE_LEN, {0x00}},
        //tm_alarm
        {DEF_TM_ALARMS_LEN, {0x01}},
        //tm_base
        {DEF_TM_BASE_LEN, {0x80, 0x43, 0x6d, 0x38}},
        //tm_time
        {DEF_TM_TIME_LEN, {0x80, 0x43, 0x6d, 0x38}},
        //tm_alarml[]
        {
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }},
            {DEF_TM_ALARML_LEN, {
                                    0,
                                }}},
        //fs_pstatus
        0,
        //sms_param[]
        {DEF_SMS_PARAM_LEN, {
                                0,
                            }},
        //cm_public
        3,
        //cm_ccwa_sim[]
        {1, 1},
        //cm_clir_sim[]
        {0, 0},
        //cm_colp_sim[]
        {0, 0},
        //gen_mem_info[]
        {DEF_MEM_INFO_LEN, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        //gen_ind_event
        0xffff,
        //gen_imsi_sim[]
        {{DEF_GEN_IMSI_LEN, {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}, {DEF_GEN_IMSI_LEN, {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}},
        //nw_sim_work_mode[]
        {
            0,
        },
        //nw_sim_flight_mode[]
        {1, 1},
        //nw_sim_freq_band[]
        {236, 236},
        //nw_sim_utms_band[]
        {
            0,
        },
        //nw_sim_plmn[]
        {{DEF_NW_PLMN_LEN, {
                               0,
                           }},
         {DEF_NW_PLMN_LEN, {
                               0,
                           }}},
        //nw_sim_ba_count[]
        {
            0,
        },
        //nw_sim_arfce_list[]
        {{DEF_ARFCN_LIST_LEN, {
                                  0,
                              }},
         {DEF_ARFCN_LIST_LEN, {
                                  0,
                              }}},
        //nw_sim_store_last[]
        {
            0,
        },
        // ipr
        115200,
        //detect_mbs
        0x00,
        //gprs_up_sum_sim[]
        {
            0,
        },
        //gprs_dwn_sum_sim[]
        {
            0,
        },
        //sim_card_switch
        0,
        //sim_soft_reset
        0,
        //sim_is_fst_pon
        1,
        //sms_storage[]
        {2, 2},
        //sms_sim_over_ind
        {
            0,
        },
        //sms_full_for_new
        {
            0,
        },
        //sms_def_param_ind
        {
            0,
        },
        //sms_sim_paraml[][]
        {{{DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }}},
         {{DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }},
          {DEF_SMS_SIM_PARAM_LEN, {
                                      0xad,
                                  }}}},
        //sms_sim_option[]
        {4, 4},
        //sms_sim_optstor[]
        {32, 32},
        //emod_param
        0x00,
        //emod_nwnm[]
        {DEF_EMOD_NWNM_LEN, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
        //emod_sign
        0,
        //emod_simnml[]
        {{DEF_EMOD_SIMNML_LEN, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}, {DEF_EMOD_SIMNML_LEN, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}},
        //tcpip_dns_adrl
        {{DEF_DNS_ADRL_LEN, "211.136.17.107"}, {DEF_DNS_ADRL_LEN, "211.136.17.107"}},
        //jmetoc_apn
        {DEF_JMETOC_APN_LEN, "cmnet"},
        //jmetoc_user
        {DEF_JMETOC_USER_LEN, "wap"},
        //jmetoc_pwd
        {DEF_JMETOC_PWD_LEN, "wap"},
        //jmetoc_proxy_adr[]
        {DEF_JMETOC_PROXY_ADR_LEN, {0x0a, 0x00, 0x00, 0xac}},
        //jmetoc_port
        0x50,
        //jmetoc_apn_num
        0x00,
        //dbs_seed
        {DEF_DBS_SEED_LEN, {
                               0,
                           }},
        //wifi_auto_open
        0x0,
        //wifi_auto_join
        0x0,
        //wifi_ssid
        {
            {DEF_WIFI_SSID_LEN, {
                                    0x0,
                                }},
        },
        //wifi_pwd
        {
            {DEF_WIFI_PWD_LEN, {
                                   0x0,
                               }},
        },
        //sequence
        0x00000000,
        //crc
        0x00000000};

CFW_NV g_CfwNv;

typedef struct _nv_info
{
    UINT8 is_inited;
    UINT8 is_dirty;
    COS_SEMA sem;
} NV_INFO;

NV_INFO g_CfwNVInfo = {0, 0, COS_SEMA_UNINIT};

static VOID nv_SemInit(VOID)
{
    COS_SemaInit(&g_CfwNVInfo.sem, 1);
}

static VOID nv_SemWaitFor(VOID)
{
    COS_SemaTake(&g_CfwNVInfo.sem);
}

static VOID nv_SemRelease(VOID)
{
    COS_SemaRelease(&g_CfwNVInfo.sem);
}
/*
static BOOL nv_check(UINT8* buff,UINT32 len)
{
    buff = buff;
    len= len;
    return TRUE;
}
*/
static BOOL nv_IsInited(VOID)
{
    return g_CfwNVInfo.is_inited == 0 ? FALSE : TRUE;
}

static void nv_Writeback(void)
{
    UINT32 crc_offs;
    void *data = COS_MALLOC(sizeof(CFW_NV));
    if (!data)
    {
        NV_ASSERT(0, "malloc fail!size = %d", sizeof(CFW_NV));
    }
    nv_SemWaitFor();
    g_CfwNv.sequence++;
    crc_offs = FIELD_OFFSETOF(CFW_NV, crc);
    g_CfwNv.crc = NV_CRC32(&g_CfwNv, crc_offs);
    SUL_MemCopy8(data, &g_CfwNv, sizeof(CFW_NV));
    nv_SemRelease();

    sf_safefile_write(CFW_NV_FILE, data, sizeof(CFW_NV));
    COS_FREE(data);
}

static void nv_UpdateNv(VOID)
{
    COS_TaskCallbackNotif(sf_async_task_handle(), (COS_CALLBACK_FUNC_T)nv_Writeback, NULL);
}

INT32 NV_Init(VOID)
{
    BOOL bresult;
    INT32 result;
    const UINT8 *cfg_path = CFW_NV_FILE;
    UINT32 crc;
    UINT32 crc_offs = FIELD_OFFSETOF(CFW_NV, crc);

    nv_SemInit();
    SUL_MemSet8((UINT8 *)&g_CfwNv, 0, sizeof(CFW_NV));
    // Read NV data from file.
    bresult = sf_safefile_init(cfg_path);
    if (!bresult)
    {
        goto _set_to_default;
    }
    else
    {
        result = sf_safefile_read(cfg_path, (void *)&g_CfwNv, sizeof(CFW_NV));
        if (result != sizeof(CFW_NV))
        {
            goto _set_to_default;
        }
        crc = NV_CRC32((void *)&g_CfwNv, crc_offs);
        if (g_CfwNv.crc != crc || g_CfwNv.version != CFW_NV_VERSION)
        {
            goto _set_to_default;
        }
    }

    g_CfwNVInfo.is_inited = 1;
    return ERR_SUCCESS;

_set_to_default:
    SUL_MemCopy8(&g_CfwNv, &g_DefCfwNv, sizeof(CFW_NV));
    nv_UpdateNv();
    g_CfwNVInfo.is_inited = 1;
    return ERR_SUCCESS;
}

// 1-0. tm_format
INT32 NV_GetTmFormat(UINT8 *tm_format, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.tm_format.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tm_format, (UINT8 *)g_CfwNv.tm_format.format, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 1-1. tm_format
INT32 NV_SetTmFormat(UINT8 *tm_format, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > TM_FAORMAT_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_format.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_format.format, tm_format, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 2-0. tm_zone
INT32 NV_GetTmZone(UINT8 *tm_zone, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.tm_zone.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tm_zone, (UINT8 *)g_CfwNv.tm_zone.zone, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 2-1. tm_zone
INT32 NV_SetTmZone(UINT8 *tm_zone, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > TM_ZONE_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_zone.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_zone.zone, tm_zone, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 3-0. tm_alarms
INT32 NV_GetTmAlarm(UINT8 *tm_alarm, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.tm_alarms.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8((UINT8 *)tm_alarm, (UINT8 *)g_CfwNv.tm_alarms.alarms, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 3-1. tm_alarms
INT32 NV_SetTmAlarm(UINT8 *tm_alarm, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > TM_ALARMS_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_alarms.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_alarms.alarms, tm_alarm, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 4-0. tm_base
INT32 NV_GetTmBase(UINT8 *tm_base, UINT32 *plen)
{
    UINT32 len;
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.tm_base.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tm_base, (UINT8 *)g_CfwNv.tm_base.base, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 4-1. tm_base
INT32 NV_SetTmBase(UINT8 *tm_base, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > TM_BASE_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_base.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_base.base, tm_base, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 5-0. tm_base
INT32 NV_GetTmTime(UINT8 *tm_time, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.tm_time.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tm_time, (UINT8 *)g_CfwNv.tm_time.time, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 5-1. tm_base
INT32 NV_SetTmTime(UINT8 *tm_time, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > TM_TIME_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_time.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_time.time, tm_time, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 6-0. tm_alarml
INT32 NV_GetTmAlarml(UINT8 index, UINT8 *tm_alarm, UINT32 *plen)
{
    UINT32 len;
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= TM_ALARM_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.tm_alarml[index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tm_alarm, (UINT8 *)g_CfwNv.tm_alarml[index].alaml, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 6-1. tm_alarml
INT32 NV_SetTmAlarml(UINT8 index, UINT8 *tm_alarm, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= TM_ALARM_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > TM_ALARML_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tm_alarml[index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tm_alarml[index].alaml, tm_alarm, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 7-0. fs_pstatus
INT32 NV_GetFsStatus(UINT8 *fs_status)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *fs_status = g_CfwNv.fs_pstatus;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 7-1. fs_pstatus
INT32 NV_SetFsStatus(UINT8 fs_status)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.fs_pstatus = fs_status;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 8-0. sms_param
INT32 NV_GetSmsParam(UINT8 *sms_param, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.sms_param.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(sms_param, (UINT8 *)g_CfwNv.sms_param.param, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 8-1. sms_param
INT32 NV_SetSmsParam(UINT8 *sms_param, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > SMS_PARAM_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_param.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.sms_param.param, sms_param, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 9-0. cm_public
INT32 NV_GetCmPublic(UINT32 *cm_public)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *cm_public = g_CfwNv.cm_public;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 9-1. cm_public
INT32 NV_SetCmPublic(UINT32 cm_public)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.cm_public = cm_public;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 10-0. cm_ccwa_sim
INT32 NV_GetCmCcwaSim(UINT8 sim_id, UINT8 *cm_ccwa_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *cm_ccwa_sim = g_CfwNv.cm_ccwa_sim[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 10-1. cm_ccwa_sim
INT32 NV_SetCmCcwaSim(UINT8 sim_id, UINT8 cm_ccwa_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.cm_ccwa_sim[sim_id] = cm_ccwa_sim;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 11-0. cm_clir_sim
INT32 NV_GetCmClirSim(UINT8 sim_id, UINT8 *cm_clir_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *cm_clir_sim = g_CfwNv.cm_clir_sim[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 11-1. cm_clir_sim
INT32 NV_SetCmClirSim(UINT8 sim_id, UINT8 cm_clir_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.cm_clir_sim[sim_id] = cm_clir_sim;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 12-0. cm_colp_sim
INT32 NV_GetCmColpSim(UINT8 sim_id, UINT8 *cm_colp_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *cm_colp_sim = g_CfwNv.cm_colp_sim[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 12-1. cm_colp_sim
INT32 NV_SetCmColpSim(UINT8 sim_id, UINT8 cm_colp_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.cm_colp_sim[sim_id] = cm_colp_sim;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 13-0. gen_mem_info
INT32 NV_GetGenMemInfo(UINT8 *gen_mem_info, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.gen_mem_info.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(gen_mem_info, (UINT8 *)g_CfwNv.gen_mem_info.info, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 13-1. gen_mem_info
INT32 NV_SetGenMemInfo(UINT8 *gen_mem_info, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > MEM_INFO_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.gen_mem_info.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.gen_mem_info.info, gen_mem_info, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 14-0. gen_ind_event
INT32 NV_GetGenIndEvent(UINT16 *gen_ind_event)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *gen_ind_event = g_CfwNv.gen_ind_event;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 14-1. gen_ind_event
INT32 NV_SetGenIndEvent(UINT16 gen_ind_event)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.gen_ind_event = gen_ind_event;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 15-0. gen_imsi_sim
INT32 NV_GetGenImsiSim(UINT8 sim_id, UINT8 *gen_imsi_sim, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.gen_imsi_sim[sim_id].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(gen_imsi_sim, (UINT8 *)g_CfwNv.gen_imsi_sim[sim_id].imsi, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 15-1. gen_imsi_sim
INT32 NV_SetGenImsiSim(UINT8 sim_id, UINT8 *gen_imsi_sim, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > GEN_IMSI_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.gen_imsi_sim[sim_id].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.gen_imsi_sim[sim_id].imsi, gen_imsi_sim, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 16-0. nw_sim_work_mode
INT32 NV_GetNwSimWorkMode(UINT8 sim_id, UINT8 *nw_sim_work_mode)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_work_mode = g_CfwNv.nw_sim_work_mode[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 16-1. nw_sim_work_mode
INT32 NV_SetNwSimWorkMode(UINT8 sim_id, UINT8 nw_sim_work_mode)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_work_mode[sim_id] = nw_sim_work_mode;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 17-0. nw_sim_flight_mode
INT32 NV_GetNwSimFlightMode(UINT8 sim_id, UINT32 *nw_sim_flight_mode)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_flight_mode = g_CfwNv.nw_sim_flight_mode[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 17-1. nw_sim_flight_mode
INT32 NV_SetNwSimFlightMode(UINT8 sim_id, UINT32 nw_sim_flight_mode)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_flight_mode[sim_id] = nw_sim_flight_mode;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 18-0. nw_sim_freq_band
INT32 NV_GetNwSimFregBand(UINT8 sim_id, UINT8 *nw_sim_freq_band)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_freq_band = g_CfwNv.nw_sim_freq_band[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 18-1. nw_sim_freq_band
INT32 NV_SetNwSimFregBand(UINT8 sim_id, UINT8 nw_sim_freq_band)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_freq_band[sim_id] = nw_sim_freq_band;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 19-0. nw_sim_umts_band
INT32 NV_GetNwSimUmtsBand(UINT8 sim_id, UINT8 *nw_sim_umts_band)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_umts_band = g_CfwNv.nw_sim_umts_band[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 19-1. nw_sim_umts_band
INT32 NV_SetNwSimUmtsBand(UINT8 sim_id, UINT8 nw_sim_umts_band)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_umts_band[sim_id] = nw_sim_umts_band;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 20-0. nw_sim_plmn
INT32 NV_GetNwSimPlmn(UINT8 sim_id, UINT8 *nw_sim_plmn, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.nw_sim_plmn[sim_id].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(nw_sim_plmn, (UINT8 *)g_CfwNv.nw_sim_plmn[sim_id].plmn, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 20-1. nw_sim_plmn
INT32 NV_SetNwSimPlmn(UINT8 sim_id, UINT8 *nw_sim_plmn, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > NW_PLMN_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_plmn[sim_id].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.nw_sim_plmn[sim_id].plmn, nw_sim_plmn, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 21-0. nw_sim_ba_count
INT32 NV_GetNwSimBaCount(UINT8 sim_id, UINT8 *nw_sim_ba_count)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_ba_count = g_CfwNv.nw_sim_ba_count[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 21-1. nw_sim_ba_count
INT32 NV_SetNwSimBaCount(UINT8 sim_id, UINT8 nw_sim_ba_count)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_ba_count[sim_id] = nw_sim_ba_count;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 22-0. nw_sim_arfce_list
INT32 NV_GetNwSimArfceList(UINT8 sim_id, UINT8 *nw_sim_arfce_list, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.nw_sim_arfce_list[sim_id].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(nw_sim_arfce_list, (UINT8 *)g_CfwNv.nw_sim_arfce_list[sim_id].arfcn, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 22-1. nw_sim_arfce_list
INT32 NV_SetNwSimArfceList(UINT8 sim_id, UINT8 *nw_sim_arfce_list, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > ARFCN_LIST_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_arfce_list[sim_id].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.nw_sim_arfce_list[sim_id].arfcn, nw_sim_arfce_list, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 23-0. nw_sim_store_last
INT32 NV_GetNwSimStoreLast(UINT8 sim_id, UINT16 *nw_sim_store_last)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_sim_store_last = g_CfwNv.nw_sim_store_last[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 23-1. nw_sim_store_last
INT32 NV_SetNwSimStoreLast(UINT8 sim_id, UINT16 nw_sim_store_last)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_sim_store_last[sim_id] = nw_sim_store_last;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 26-0. nw_ipr
INT32 NV_GetNwIpr(UINT32 *nw_ipr)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *nw_ipr = g_CfwNv.nw_ipr;
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 26-1. nw_ipr
INT32 NV_SetNwIpr(UINT32 nw_ipr)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_ipr = nw_ipr;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//25-0. detect_mbs
INT32 NV_GetNwDetectMBS(UINT8 *detect_mbs)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *detect_mbs = g_CfwNv.nw_detect_mbs;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//25-1. detect_mbs
INT32 NV_SetNwDetectMBS(UINT8 detect_mbs)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.nw_detect_mbs = detect_mbs;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 26-0. gprs_up_sum_sim
INT32 NV_GetGprsUpSumSim(UINT8 sim_id, INT32 *gprs_up_sum_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *gprs_up_sum_sim = g_CfwNv.gprs_up_sum_sim[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 26-1. gprs_up_sum_sim
INT32 NV_SetGprsUpSumSim(UINT8 sim_id, INT32 gprs_up_sum_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.gprs_up_sum_sim[sim_id] = gprs_up_sum_sim;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 27-0. gprs_dwn_sum_sim
INT32 NV_GetGprsDwnSumSim(UINT8 sim_id, INT32 *gprs_dwn_sum_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *gprs_dwn_sum_sim = g_CfwNv.gprs_dwn_sum_sim[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 27-1. gprs_dwn_sum_sim
INT32 NV_SetGprsDwnSumSim(UINT8 sim_id, INT32 gprs_dwn_sum_sim)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.gprs_dwn_sum_sim[sim_id] = gprs_dwn_sum_sim;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 28-0. sim_card_switch
INT32 NV_GetSimCardSwitch(UINT8 *sim_card_switch)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sim_card_switch = g_CfwNv.sim_card_switch;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 28-1. sim_card_switch
INT32 NV_SetSimCardSwitch(UINT8 sim_card_switch)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sim_card_switch = sim_card_switch;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 29-0. sim_soft_reset
INT32 NV_GetSimSoftReset(UINT8 *sim_soft_reset)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sim_soft_reset = g_CfwNv.sim_soft_reset;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 29-1. sim_soft_reset
INT32 NV_SetSimSoftReset(UINT8 sim_soft_reset)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sim_soft_reset = sim_soft_reset;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

// 30-0. sim_is_fst_pon
INT32 NV_GetSimFstPon(UINT8 *sim_is_fst_pon)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sim_is_fst_pon = g_CfwNv.sim_is_fst_pon;

    nv_SemRelease();
    return ERR_SUCCESS;
}

// 30-1. sim_is_fst_pon
INT32 NV_SetSimFstPon(UINT8 sim_is_fst_pon)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sim_is_fst_pon = sim_is_fst_pon;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//31-0. sms_storage
INT32 NV_GetSmsStorage(UINT8 index, UINT8 *sms_storage)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= SMS_STORAGE_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    *sms_storage = g_CfwNv.sms_storage[index];

    nv_SemRelease();
    return ERR_SUCCESS;
}

//31-1. sms_storage
INT32 NV_SetSmsStorage(UINT8 index, UINT8 sms_storage)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= SMS_STORAGE_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_storage[index] = sms_storage;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//32-0. sms_sim_over_ind
INT32 NV_GetSmsOverInd(UINT8 sim_id, UINT8 *sms_sim_over_ind)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sms_sim_over_ind = g_CfwNv.sms_sim_over_ind[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

//32-1. sms_sim_over_ind
INT32 NV_SetSmsOverInd(UINT8 sim_id, UINT8 sms_sim_over_ind)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_sim_over_ind[sim_id] = sms_sim_over_ind;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//33-0. sms_full_for_new
INT32 NV_GetSmsFullForNew(UINT8 sim_id, UINT8 *sms_full_for_new)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sms_full_for_new = g_CfwNv.sms_full_for_new[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

//33-1. sms_full_for_new
INT32 NV_SetSmsFullForNew(UINT8 sim_id, UINT8 sms_full_for_new)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_full_for_new[sim_id] = sms_full_for_new;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//34-0. sms_def_param_ind
INT32 NV_GetSmsDefParamInd(UINT8 sim_id, UINT8 *sms_def_param_ind)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sms_def_param_ind = g_CfwNv.sms_def_param_ind[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

//34-1. sms_def_param_ind
INT32 NV_SetSmsDefParamInd(UINT8 sim_id, UINT8 sms_def_param_ind)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_def_param_ind[sim_id] = sms_def_param_ind;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//35-0. sms_sim_paraml
INT32 NV_GetSmsSimParaml(UINT8 sim_id, UINT8 index, UINT8 *sms_sim_param, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= SMS_SIM_PARAM_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.sms_sim_paraml[sim_id][index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(sms_sim_param, (UINT8 *)g_CfwNv.sms_sim_paraml[sim_id][index].sim_param, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//35-1. sms_sim_paraml
INT32 NV_SetSmsSimParaml(UINT8 sim_id, UINT8 index, UINT8 *sms_sim_param, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= SMS_SIM_PARAM_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > SMS_SIM_PARAM_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_sim_paraml[sim_id][index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.sms_sim_paraml[sim_id][index].sim_param, sms_sim_param, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//36-0. sms_sim_option
INT32 NV_GetSmsSimOption(UINT8 sim_id, UINT8 *sms_sim_option)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sms_sim_option = g_CfwNv.sms_sim_option[sim_id];
    nv_SemRelease();
    return ERR_SUCCESS;
}

//36-1. sms_sim_option
INT32 NV_SetSmsSimOption(UINT8 sim_id, UINT8 sms_sim_option)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_sim_option[sim_id] = sms_sim_option;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//37-0. sms_sim_optstor
INT32 NV_GetSmsSimOptStor(UINT8 sim_id, UINT8 *sms_sim_optstor)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *sms_sim_optstor = g_CfwNv.sms_sim_optstor[sim_id];

    nv_SemRelease();
    return ERR_SUCCESS;
}

//37-1. sms_sim_optstor
INT32 NV_SetSmsSimOptStor(UINT8 sim_id, UINT8 sms_sim_optstor)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.sms_sim_optstor[sim_id] = sms_sim_optstor;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//38-0. emod_param
INT32 NV_GetEmodParam(UINT8 *emod_param)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *emod_param = g_CfwNv.emod_param;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//38-1. emod_param
INT32 NV_SetEmodParam(UINT8 emod_param)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.emod_param = emod_param;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//39-0. emod_nwnm
INT32 NV_GetEmodNwnm(UINT8 *emod_nwnm, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.emod_nwnm.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(emod_nwnm, (UINT8 *)g_CfwNv.emod_nwnm.nwnm, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//39-1. emod_nwnm
INT32 NV_SetEmodNwnm(UINT8 *emod_nwnm, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > EMOD_NWNM_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.emod_nwnm.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.emod_nwnm.nwnm, emod_nwnm, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//40-0. emod_sign
INT32 NV_GetEmodSign(UINT8 *emod_sign)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *emod_sign = g_CfwNv.emod_sign;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//40-1. emod_sign
INT32 NV_SetEmodSign(UINT8 emod_sign)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.emod_sign = emod_sign;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//41-0. emod_simnml
INT32 NV_GetEmodSimnml(UINT8 index, UINT8 *emod_simnm, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= EMOD_SIMNML_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.emod_simnml[index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(emod_simnm, (UINT8 *)g_CfwNv.emod_simnml[index].simnm, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//41-1. emod_simnml
INT32 NV_SetEmodSimnml(UINT8 index, UINT8 *emod_simnm, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= EMOD_SIMNML_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > EMOD_SIMNML_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.emod_simnml[index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.emod_simnml[index].simnm, emod_simnm, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//42-0. tcpip_dns_adrl
INT32 NV_GetTcpipDnsAdrl(UINT8 index, UINT8 *tcpip_dns_adrl, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= DNS_ADRL_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.tcpip_dns_adrl[index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(tcpip_dns_adrl, (UINT8 *)g_CfwNv.tcpip_dns_adrl[index].adrl, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//42-1. tcpip_dns_adrl
INT32 NV_SetTcpipDnsAdrl(UINT8 index, UINT8 *tcpip_dns_adrl, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= DNS_ADRL_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > DNS_ADRL_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.tcpip_dns_adrl[index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.tcpip_dns_adrl[index].adrl, tcpip_dns_adrl, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//43-0. jmetoc_apn
INT32 NV_GetJmetocApn(UINT8 *jmetoc_apn, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.jmetoc_apn.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(jmetoc_apn, (UINT8 *)g_CfwNv.jmetoc_apn.apn, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//43-1. jmetoc_apn
INT32 NV_SetJmetocApn(UINT8 *jmetoc_apn, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > JMETOC_APN_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_apn.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.jmetoc_apn.apn, jmetoc_apn, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//44-0. jmetoc_user
INT32 NV_GetJmetocUser(UINT8 *jmetoc_user, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.jmetoc_user.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(jmetoc_user, (UINT8 *)g_CfwNv.jmetoc_user.user, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//44-1. jmetoc_user
INT32 NV_SetJmetocUser(UINT8 *jmetoc_user, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > JMETOC_USER_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_user.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.jmetoc_user.user, jmetoc_user, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//45-0. jmetoc_pwd
INT32 NV_GetJmetocPwd(UINT8 *jmetoc_pwd, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.jmetoc_pwd.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(jmetoc_pwd, (UINT8 *)g_CfwNv.jmetoc_pwd.pwd, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//45-1. jmetoc_pwd
INT32 NV_SetJmetocPwd(UINT8 *jmetoc_pwd, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > JMETOC_PWD_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_pwd.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.jmetoc_pwd.pwd, jmetoc_pwd, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//46-0. jmetoc_proxy_adr
INT32 NV_GetJmetocAdr(UINT8 *jmetoc_proxy_adr, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.jmetoc_proxy_adr.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(jmetoc_proxy_adr, (UINT8 *)g_CfwNv.jmetoc_proxy_adr.proxy, len);
    *plen = len;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//46-1. jmetoc_proxy_adr
INT32 NV_SetJmetocAdr(UINT8 *jmetoc_proxy_adr, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > JMETOC_PROXY_ADR_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_proxy_adr.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.jmetoc_proxy_adr.proxy, jmetoc_proxy_adr, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//47-0. jmetoc_port
INT32 NV_GetJmetocPort(UINT16 *jmetoc_port)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *jmetoc_port = g_CfwNv.jmetoc_port;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//47-1. jmetoc_port
INT32 NV_SetJmetocPort(UINT16 jmetoc_port)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_port = jmetoc_port;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//48-0. jmetoc_apn_num
INT32 NV_GetJmetocApnNum(UINT8 *jmetoc_apn_num)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *jmetoc_apn_num = g_CfwNv.jmetoc_apn_num;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//48-1. jmetoc_apn_num
INT32 NV_SetJmetocApnNum(UINT8 jmetoc_apn_num)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.jmetoc_apn_num = jmetoc_apn_num;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//49-0. dbs_seed
INT32 NV_GetDbsSeed(UINT8 *dbs_seed, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    len = g_CfwNv.dbs_seed.len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(dbs_seed, (UINT8 *)g_CfwNv.dbs_seed.seed, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//49-1. dbs_seed
INT32 NV_SetDbsSeed(UINT8 *dbs_seed, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (len > DBS_SEED_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.dbs_seed.len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.dbs_seed.seed, dbs_seed, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//50-0. wifi_auto_open
INT32 NV_GetWifiAutoOpen(UINT8 *wifi_auto_open)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *wifi_auto_open = g_CfwNv.wifi_auto_open;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//50-1. wifi_auto_open
INT32 NV_SetWifiAutoOpen(UINT8 wifi_auto_open)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.wifi_auto_open = wifi_auto_open;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//51-0. wifi_auto_join
INT32 NV_GetWifiAutoJoin(UINT8 *wifi_auto_join)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    *wifi_auto_join = g_CfwNv.wifi_auto_join;

    nv_SemRelease();
    return ERR_SUCCESS;
}

//51-1. wifi_auto_join
INT32 NV_SetWifiAutoJoin(UINT8 wifi_auto_join)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    nv_SemWaitFor();
    g_CfwNv.wifi_auto_join = wifi_auto_join;
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//52-0. wifi_ssid
INT32 NV_GetWifiSsid(UINT8 index, UINT8 *wifi_ssid, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= WIFI_SSID_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.wifi_ssid[index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(wifi_ssid, (UINT8 *)g_CfwNv.wifi_ssid[index].ssid, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//52-1. wifi_ssid
INT32 NV_SetWifiSsid(UINT8 index, UINT8 *wifi_ssid, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= WIFI_SSID_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > WIFI_SSID_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.wifi_ssid[index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.wifi_ssid[index].ssid, wifi_ssid, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

//53-0. wifi_pwd
INT32 NV_GetWifiPwd(UINT8 index, UINT8 *wifi_pwd, UINT32 *plen)
{
    UINT32 len;

    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= WIFI_SSID_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    len = g_CfwNv.wifi_pwd[index].len;
    if (*plen < len)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    SUL_MemCopy8(wifi_pwd, (UINT8 *)g_CfwNv.wifi_pwd[index].pwd, len);
    *plen = len;
    nv_SemRelease();
    return ERR_SUCCESS;
}

//53-1. wifi_pwd
INT32 NV_SetWifiPwd(UINT8 index, UINT8 *wifi_pwd, UINT32 len)
{
    if (!nv_IsInited())
    {
        return ERR_NV_UNINIT;
    }
    if (index >= WIFI_SSID_NUM)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (len > WIFI_PWD_LEN)
    {
        return ERR_NV_PARAM_ERROR;
    }
    nv_SemWaitFor();
    g_CfwNv.wifi_pwd[index].len = len;
    SUL_MemCopy8((UINT8 *)g_CfwNv.wifi_pwd[index].pwd, wifi_pwd, len);
    nv_UpdateNv();
    nv_SemRelease();
    return ERR_SUCCESS;
}

UINT32 NV_GetNvSize(VOID)
{
    return sizeof(CFW_NV);
}
INT32 NV_Read(UINT32 offs, UINT8 *buff, UINT32 to_read, UINT32 *pread)
{
    UINT32 len;

    if (!buff)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (offs >= sizeof(CFW_NV))
        return ERR_NV_PARAM_ERROR;

    if (to_read == 0)
    {
        *pread = 0;
        return ERR_SUCCESS;
    }

    len = (offs + to_read) < sizeof(CFW_NV) ? to_read : (sizeof(CFW_NV) - offs);
    SUL_MemCopy8(buff, ((UINT8 *)&g_CfwNv) + offs, len);
    *pread = len;
    return ERR_SUCCESS;
}

INT32 NV_Write(UINT32 offs, UINT8 *buff, UINT32 to_write, UINT32 *pwitten)
{
    UINT32 len;

    if (!buff)
    {
        return ERR_NV_PARAM_ERROR;
    }
    if (offs >= sizeof(CFW_NV))
        return ERR_NV_PARAM_ERROR;

    if (to_write == 0)
    {
        *pwitten = 0;
        return ERR_SUCCESS;
    }

    len = (offs + to_write) < sizeof(CFW_NV) ? to_write : (sizeof(CFW_NV) - offs);
    SUL_MemCopy8(((UINT8 *)&g_CfwNv) + offs, buff, len);
    *pwitten = len;
    return ERR_SUCCESS;
}
