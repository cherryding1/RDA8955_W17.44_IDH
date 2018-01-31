#ifndef _CFW_NV_H_
#define _CFW_NV_H_

#define FIELD_OFFSETOF(TYPE, MEMBER) ((UINT32) & ((TYPE *)0)->MEMBER)

#define NV_ASSERT(bl, format, ...)                \
    \
{                                          \
        if (!(bl))                                \
        {                                         \
            hal_DbgAssert(format, ##__VA_ARGS__); \
        \
}                                      \
    }
extern UINT32 vds_CRC32(CONST VOID *pvData, INT32 iLen);
#define NV_CRC32 vds_CRC32

#define CFW_NV_VERSION 0x20171124

#define DEF_TM_FAORMAT_LEN 4
#define DEF_TM_ZONE_LEN 1
#define DEF_TM_BASE_LEN 4
#define DEF_TM_TIME_LEN 4
#define DEF_TM_ALARMS_LEN 1
#define DEF_TM_ALARML_LEN 44
#define DEF_SMS_PARAM_LEN 38
#define DEF_MEM_INFO_LEN 28
#define DEF_GEN_IMSI_LEN 10
#define DEF_NW_PLMN_LEN 3
#define DEF_ARFCN_LIST_LEN 54
#define DEF_SMS_SIM_PARAM_LEN 22
#define DEF_EMOD_NWNM_LEN 16
#define DEF_EMOD_SIMNML_LEN 48
#define DEF_DNS_ADRL_LEN 20
#define DEF_JMETOC_APN_LEN 16
#define DEF_JMETOC_USER_LEN 16
#define DEF_JMETOC_PWD_LEN 16
#define DEF_JMETOC_PROXY_ADR_LEN 16
#define DEF_DBS_SEED_LEN 32
#define DEF_WIFI_SSID_LEN 0
#define DEF_WIFI_PWD_LEN 0
#define TM_FAORMAT_LEN sizeof(UINT32)
#define TM_ZONE_LEN sizeof(INT8)
#define TM_BASE_LEN 4
#define TM_TIME_LEN 4
#define TM_ALARMS_LEN 4
#define TM_ALARML_LEN 44
#define SMS_PARAM_LEN 40
#define MEM_INFO_LEN 34
#define GEN_IMSI_LEN 10
#define NW_PLMN_LEN 3
#define ARFCN_LIST_LEN CFW_STORE_MAX_COUNT * sizeof(u16)
#define SMS_SIM_PARAM_LEN sizeof(CFW_SMS_PARAMETER)
#define EMOD_NWNM_LEN 16
#define EMOD_SIMNML_LEN 48
#define DNS_ADRL_LEN 20
#define JMETOC_APN_LEN 16
#define JMETOC_USER_LEN 16
#define JMETOC_PWD_LEN 16
#define JMETOC_PROXY_ADR_LEN 16
#define DBS_SEED_LEN 32
#define WIFI_SSID_LEN 40
#define WIFI_PWD_LEN 64
#define SIM_NUM NUMBER_OF_SIM
#define TM_ALARM_NUM 16
#define SMS_STORAGE_NUM 2
#define SMS_SIM_PARAM_NUM 5
#define EMOD_SIMNML_NUM 2
#define DNS_ADRL_NUM 2
#define WIFI_SSID_NUM 5

typedef struct
{
    UINT32 len;
    UINT8 format[TM_FAORMAT_LEN];
} TM_FORMAT;
typedef struct
{
    UINT32 len;
    UINT8 zone[TM_ZONE_LEN];
} TM_ZONE;
typedef struct
{
    UINT32 len;
    UINT8 alarms[TM_ALARMS_LEN];
} TM_ALARMS;
typedef struct
{
    UINT32 len;
    UINT8 base[TM_BASE_LEN];
} TM_BASE;
typedef struct
{
    UINT32 len;
    UINT8 time[TM_TIME_LEN];
} TM_TIME;
typedef struct
{
    UINT32 len;
    UINT8 alaml[TM_ALARML_LEN];
} TM_ALARML;
typedef struct
{
    UINT32 len;
    UINT8 param[SMS_PARAM_LEN];
} SMS_PARAM;
typedef struct
{
    UINT32 len;
    UINT8 info[MEM_INFO_LEN];
} MEM_INFO;
typedef struct
{
    UINT32 len;
    UINT8 imsi[GEN_IMSI_LEN];
} GEN_IMSI;
typedef struct
{
    UINT32 len;
    UINT8 plmn[NW_PLMN_LEN];
} NW_PLMN;
typedef struct
{
    UINT32 len;
    UINT8 arfcn[ARFCN_LIST_LEN];
} NW_ARFCN_LIST;
typedef struct
{
    UINT32 len;
    UINT8 sim_param[SMS_SIM_PARAM_LEN];
} SMS_SIM_PARAM;
typedef struct
{
    UINT32 len;
    UINT8 nwnm[EMOD_NWNM_LEN];
} EMOD_NWNM;
typedef struct
{
    UINT32 len;
    UINT8 simnm[EMOD_SIMNML_LEN];
} EMOD_SIMNML;
typedef struct
{
    UINT32 len;
    UINT8 adrl[DNS_ADRL_LEN];
} DNS_ADRL;
typedef struct
{
    UINT32 len;
    UINT8 apn[JMETOC_APN_LEN];
} JMETOC_APN;
typedef struct
{
    UINT32 len;
    UINT8 user[JMETOC_USER_LEN];
} JMETOC_USER;
typedef struct
{
    UINT32 len;
    UINT8 pwd[JMETOC_PWD_LEN];
} JMETOC_PWD;
typedef struct
{
    UINT32 len;
    UINT8 proxy[JMETOC_PROXY_ADR_LEN];
} JMETOC_PROXY_ADR;

typedef struct
{
    UINT32 len;
    UINT8 seed[DBS_SEED_LEN];
} DBS_SEED;

typedef struct
{
    UINT32 len;
    UINT8 ssid[WIFI_SSID_LEN];
} WIFI_SSID;

typedef struct
{
    UINT32 len;
    UINT8 pwd[WIFI_PWD_LEN];
} WIFI_PWD;

typedef struct
{
    UINT32 version;
    TM_FORMAT tm_format;
    TM_ZONE tm_zone;
    TM_ALARMS tm_alarms;
    TM_BASE tm_base;
    TM_TIME tm_time;
    TM_ALARML tm_alarml[TM_ALARM_NUM];
    UINT8 fs_pstatus;
    SMS_PARAM sms_param;
    UINT32 cm_public;
    UINT8 cm_ccwa_sim[SIM_NUM];
    UINT8 cm_clir_sim[SIM_NUM];
    UINT8 cm_colp_sim[SIM_NUM];
    MEM_INFO gen_mem_info;
    UINT16 gen_ind_event;
    GEN_IMSI gen_imsi_sim[SIM_NUM];
    UINT8 nw_sim_work_mode[SIM_NUM];
    UINT8 nw_sim_flight_mode[SIM_NUM];
    UINT8 nw_sim_freq_band[SIM_NUM];
    UINT8 nw_sim_umts_band[SIM_NUM];
    NW_PLMN nw_sim_plmn[SIM_NUM];
    UINT8 nw_sim_ba_count[SIM_NUM];
    NW_ARFCN_LIST nw_sim_arfce_list[SIM_NUM];
    UINT16 nw_sim_store_last[SIM_NUM];
    INT32 nw_ipr;
    UINT8 nw_detect_mbs;
    INT32 gprs_up_sum_sim[SIM_NUM];
    INT32 gprs_dwn_sum_sim[SIM_NUM];
    UINT8 sim_card_switch;
    UINT8 sim_soft_reset;
    UINT8 sim_is_fst_pon;
    UINT8 sms_storage[SMS_STORAGE_NUM];
    UINT8 sms_sim_over_ind[SIM_NUM];
    UINT8 sms_full_for_new[SIM_NUM];
    UINT8 sms_def_param_ind[SIM_NUM];
    SMS_SIM_PARAM sms_sim_paraml[SIM_NUM][SMS_SIM_PARAM_NUM];
    UINT8 sms_sim_option[SIM_NUM];
    UINT8 sms_sim_optstor[SIM_NUM];
    UINT8 emod_param;
    EMOD_NWNM emod_nwnm;
    UINT8 emod_sign;
    EMOD_SIMNML emod_simnml[EMOD_SIMNML_NUM];
    DNS_ADRL tcpip_dns_adrl[DNS_ADRL_NUM];
    JMETOC_APN jmetoc_apn;
    JMETOC_USER jmetoc_user;
    JMETOC_PWD jmetoc_pwd;
    JMETOC_PROXY_ADR jmetoc_proxy_adr;
    UINT16 jmetoc_port;
    UINT8 jmetoc_apn_num;
    DBS_SEED dbs_seed;
    UINT8 wifi_auto_open;
    UINT8 wifi_auto_join;
    WIFI_SSID wifi_ssid[WIFI_SSID_NUM];
    WIFI_PWD wifi_pwd[WIFI_SSID_NUM];
    UINT32 sequence;
    UINT32 crc;
} CFW_NV;

INT32 NV_Init(VOID);

INT32 NV_GetTmFormat(UINT8 *tm_format, UINT32 *plen);
INT32 NV_SetTmFormat(UINT8 *tm_format, UINT32 len);
INT32 NV_GetTmZone(UINT8 *tm_zone, UINT32 *plen);
INT32 NV_SetTmZone(UINT8 *tm_zone, UINT32 len);
INT32 NV_GetTmAlarm(UINT8 *tm_alarm, UINT32 *plen);
INT32 NV_SetTmAlarm(UINT8 *tm_alarm, UINT32 len);
INT32 NV_GetTmBase(UINT8 *tm_base, UINT32 *plen);
INT32 NV_SetTmBase(UINT8 *tm_base, UINT32 len);
INT32 NV_GetTmTime(UINT8 *tm_time, UINT32 *plen);
INT32 NV_SetTmTime(UINT8 *tm_time, UINT32 len);
INT32 NV_GetTmAlarml(UINT8 index, UINT8 *tm_alarm, UINT32 *plen);
INT32 NV_SetTmAlarml(UINT8 index, UINT8 *tm_alarm, UINT32 len);
INT32 NV_GetFsStatus(UINT8 *fs_status);
INT32 NV_SetFsStatus(UINT8 fs_status);
INT32 NV_GetSmsParam(UINT8 *sms_param, UINT32 *plen);
INT32 NV_SetSmsParam(UINT8 *sms_param, UINT32 len);
INT32 NV_GetCmPublic(UINT32 *cm_public);
INT32 NV_SetCmPublic(UINT32 cm_public);
INT32 NV_GetCmCcwaSim(UINT8 sim_id, UINT8 *cm_ccwa_sim);
INT32 NV_SetCmCcwaSim(UINT8 sim_id, UINT8 cm_ccwa_sim);
INT32 NV_GetCmClirSim(UINT8 sim_id, UINT8 *cm_clir_sim);
INT32 NV_SetCmClirSim(UINT8 sim_id, UINT8 cm_clir_sim);
INT32 NV_GetCmColpSim(UINT8 sim_id, UINT8 *cm_colp_sim);
INT32 NV_SetCmColpSim(UINT8 sim_id, UINT8 cm_colp_sim);
INT32 NV_GetGenMemInfo(UINT8 *gen_mem_info, UINT32 *plen);
INT32 NV_SetGenMemInfo(UINT8 *gen_mem_info, UINT32 len);
INT32 NV_GetGenIndEvent(UINT16 *gen_ind_event);
INT32 NV_SetGenIndEvent(UINT16 gen_ind_event);
INT32 NV_GetGenImsiSim(UINT8 sim_id, UINT8 *gen_imsi_sim, UINT32 *plen);
INT32 NV_SetGenImsiSim(UINT8 sim_id, UINT8 *gen_imsi_sim, UINT32 len);
INT32 NV_GetNwSimWorkMode(UINT8 sim_id, UINT8 *nw_sim_work_mode);
INT32 NV_SetNwSimWorkMode(UINT8 sim_id, UINT8 nw_sim_work_mode);
INT32 NV_GetNwSimFlightMode(UINT8 sim_id, UINT32 *nw_sim_flight_mode);
INT32 NV_SetNwSimFlightMode(UINT8 sim_id, UINT32 nw_sim_flight_mode);
INT32 NV_GetNwSimFregBand(UINT8 sim_id, UINT8 *nw_sim_freq_band);
INT32 NV_SetNwSimFregBand(UINT8 sim_id, UINT8 nw_sim_freq_band);
INT32 NV_GetNwSimUmtsBand(UINT8 sim_id, UINT8 *nw_sim_umts_band);
INT32 NV_SetNwSimUmtsBand(UINT8 sim_id, UINT8 nw_sim_umts_band);
INT32 NV_GetNwSimPlmn(UINT8 sim_id, UINT8 *nw_sim_plmn, UINT32 *plen);
INT32 NV_SetNwSimPlmn(UINT8 sim_id, UINT8 *nw_sim_plmn, UINT32 len);
INT32 NV_GetNwSimBaCount(UINT8 sim_id, UINT8 *nw_sim_ba_count);
INT32 NV_SetNwSimBaCount(UINT8 sim_id, UINT8 nw_sim_ba_count);
INT32 NV_GetNwSimArfceList(UINT8 sim_id, UINT8 *nw_sim_arfce_list, UINT32 *plen);
INT32 NV_SetNwSimArfceList(UINT8 sim_id, UINT8 *nw_sim_arfce_list, UINT32 len);
INT32 NV_GetNwSimStoreLast(UINT8 sim_id, UINT16 *nw_sim_store_last);
INT32 NV_SetNwSimStoreLast(UINT8 sim_id, UINT16 nw_sim_store_last);
INT32 NV_GetNwIpr(UINT32 *nw_ipr);
INT32 NV_SetNwIpr(UINT32 nw_ipr);
INT32 NV_GetNwDetectMBS(UINT8 *detect_mbs);
INT32 NV_SetNwDetectMBS(UINT8 detect_mbs);
INT32 NV_GetGprsUpSumSim(UINT8 sim_id, INT32 *gprs_up_sum_sim);
INT32 NV_SetGprsUpSumSim(UINT8 sim_id, INT32 gprs_up_sum_sim);
INT32 NV_GetGprsDwnSumSim(UINT8 sim_id, INT32 *gprs_dwn_sum_sim);
INT32 NV_SetGprsDwnSumSim(UINT8 sim_id, INT32 gprs_dwn_sum_sim);
INT32 NV_GetSimCardSwitch(UINT8 *sim_card_switch);
INT32 NV_SetSimCardSwitch(UINT8 sim_card_switch);
INT32 NV_GetSimSoftReset(UINT8 *sim_soft_reset);
INT32 NV_SetSimSoftReset(UINT8 sim_soft_reset);
INT32 NV_GetSimFstPon(UINT8 *sim_is_fst_pon);
INT32 NV_SetSimFstPon(UINT8 sim_is_fst_pon);
INT32 NV_GetSmsStorage(UINT8 index, UINT8 *sms_storage);
INT32 NV_SetSmsStorage(UINT8 index, UINT8 sms_storage);
INT32 NV_GetSmsOverInd(UINT8 sim_id, UINT8 *sms_sim_over_ind);
INT32 NV_SetSmsOverInd(UINT8 sim_id, UINT8 sms_sim_over_ind);
INT32 NV_GetSmsFullForNew(UINT8 sim_id, UINT8 *sms_full_for_new);
INT32 NV_SetSmsFullForNew(UINT8 sim_id, UINT8 sms_full_for_new);
INT32 NV_GetSmsDefParamInd(UINT8 sim_id, UINT8 *sms_def_param_ind);
INT32 NV_SetSmsDefParamInd(UINT8 sim_id, UINT8 sms_def_param_ind);
INT32 NV_GetSmsSimParaml(UINT8 sim_id, UINT8 index, UINT8 *sms_sim_param, UINT32 *plen);
INT32 NV_SetSmsSimParaml(UINT8 sim_id, UINT8 index, UINT8 *sms_sim_param, UINT32 len);
INT32 NV_GetSmsSimOption(UINT8 sim_id, UINT8 *sms_sim_option);
INT32 NV_SetSmsSimOption(UINT8 sim_id, UINT8 sms_sim_option);
INT32 NV_GetSmsSimOptStor(UINT8 sim_id, UINT8 *sms_sim_optstor);
INT32 NV_SetSmsSimOptStor(UINT8 sim_id, UINT8 sms_sim_optstor);
INT32 NV_GetEmodParam(UINT8 *emod_param);
INT32 NV_SetEmodParam(UINT8 emod_param);
INT32 NV_GetEmodNwnm(UINT8 *emod_nwnm, UINT32 *plen);
INT32 NV_SetEmodNwnm(UINT8 *emod_nwnm, UINT32 len);
INT32 NV_GetEmodSign(UINT8 *emod_sign);
INT32 NV_SetEmodSign(UINT8 emod_sign);
INT32 NV_GetEmodSimnml(UINT8 index, UINT8 *emod_simnm, UINT32 *plen);
INT32 NV_SetEmodSimnml(UINT8 index, UINT8 *emod_simnm, UINT32 len);
INT32 NV_GetTcpipDnsAdrl(UINT8 index, UINT8 *tcpip_dns_adrl, UINT32 *plen);
INT32 NV_SetTcpipDnsAdrl(UINT8 index, UINT8 *tcpip_dns_adrl, UINT32 len);
INT32 NV_GetJmetocApn(UINT8 *jmetoc_apn, UINT32 *plen);
INT32 NV_SetJmetocApn(UINT8 *jmetoc_apn, UINT32 len);
INT32 NV_GetJmetocUser(UINT8 *jmetoc_user, UINT32 *plen);
INT32 NV_SetJmetocUser(UINT8 *jmetoc_user, UINT32 len);
INT32 NV_GetJmetocPwd(UINT8 *jmetoc_pwd, UINT32 *plen);
INT32 NV_SetJmetocPwd(UINT8 *jmetoc_pwd, UINT32 len);
INT32 NV_GetJmetocAdr(UINT8 *jmetoc_proxy_adr, UINT32 *plen);
INT32 NV_SetJmetocAdr(UINT8 *jmetoc_proxy_adr, UINT32 len);
INT32 NV_GetJmetocPort(UINT16 *jmetoc_port);
INT32 NV_SetJmetocPort(UINT16 jmetoc_port);
INT32 NV_GetJmetocApnNum(UINT8 *jmetoc_apn_num);
INT32 NV_SetJmetocApnNum(UINT8 jmetoc_apn_num);
INT32 NV_GetDbsSeed(UINT8 *dbs_seed, UINT32 *plen);
INT32 NV_SetDbsSeed(UINT8 *dbs_seed, UINT32 len);
INT32 NV_GetWifiAutoOpen(UINT8 *wifi_auto_open);
INT32 NV_SetWifiAutoOpen(UINT8 wifi_auto_open);
INT32 NV_GetWifiAutoJoin(UINT8 *wifi_auto_join);
INT32 NV_SetWifiAutoJoin(UINT8 wifi_auto_join);
INT32 NV_GetWifiSsid(UINT8 index, UINT8 *wifi_ssid, UINT32 *plen);
INT32 NV_SetWifiSsid(UINT8 index, UINT8 *wifi_ssid, UINT32 len);
INT32 NV_GetWifiPwd(UINT8 index, UINT8 *wifi_pwd, UINT32 *plen);
INT32 NV_SetWifiPwd(UINT8 index, UINT8 *wifi_pwd, UINT32 len);

UINT32 NV_GetNvSize(VOID);
INT32 NV_Read(UINT32 offs, UINT8 *buff, UINT32 to_read, UINT32 *pread);
INT32 NV_Write(UINT32 offs, UINT8 *buff, UINT32 to_write, UINT32 *pwitten);

#define ERR_NV_UNINIT 1001
#define ERR_NV_PARAM_ERROR 1002

#endif //_CFW_NV_H_
