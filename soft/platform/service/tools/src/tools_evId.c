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















#include <cswtype.h>
#include <ts.h>
#include <event.h>
#include "api_msg.h"
#include "csw_mem_prv.h"

#define EVID_CASE(nEvId) case nEvId: return #nEvId

PCSTR TS_GetSIMFileName(UINT32 nEvId)
{
    switch (nEvId)
    {
            EVID_CASE(API_SIM_EF_ADN       );
            EVID_CASE(API_SIM_EF_FDN       );
            EVID_CASE(API_SIM_EF_SMS       );
            EVID_CASE(API_SIM_EF_CCP       );
            EVID_CASE(API_SIM_EF_MSISDN    );
            EVID_CASE(API_SIM_EF_SMSP      );
            EVID_CASE(API_SIM_EF_SMSS      );
            EVID_CASE(API_SIM_EF_LND       );
            EVID_CASE(API_SIM_EF_SMSR      );
            EVID_CASE(API_SIM_EF_SDN       );
            EVID_CASE(API_SIM_EF_EXT1      );
            EVID_CASE(API_SIM_EF_EXT2      );
            EVID_CASE(API_SIM_EF_EXT3      );
            EVID_CASE(API_SIM_EF_BDN       );
            EVID_CASE(API_SIM_EF_EXT4      );
            EVID_CASE(API_SIM_EF_LP        );
            EVID_CASE(API_SIM_EF_IMSI      );
            EVID_CASE(API_SIM_EF_KC        );
            EVID_CASE(API_SIM_EF_PLMNSEL   );
            EVID_CASE(API_SIM_EF_HPLMN     );
            EVID_CASE(API_SIM_EF_ACMMAX    );
            EVID_CASE(API_SIM_EF_SST       );
            EVID_CASE(API_SIM_EF_ACM       );
            EVID_CASE(API_SIM_EF_GID1      );
            EVID_CASE(API_SIM_EF_GID2      );
            EVID_CASE(API_SIM_EF_PUCT      );
            EVID_CASE(API_SIM_EF_CBMI      );
            EVID_CASE(API_SIM_EF_SPN       );
            EVID_CASE(API_SIM_EF_CBMID     );
            EVID_CASE(API_SIM_EF_BCCH      );
            EVID_CASE(API_SIM_EF_ACC             );
            EVID_CASE(API_SIM_EF_FPLMN           );
            EVID_CASE(API_SIM_EF_LOCI            );
            EVID_CASE(API_SIM_EF_AD              );
            EVID_CASE(API_SIM_EF_PHASE           );
            EVID_CASE(API_SIM_EF_VGCS            );
            EVID_CASE(API_SIM_EF_VGCSS           );
            EVID_CASE(API_SIM_EF_VBS             );
            EVID_CASE(API_SIM_EF_VBSS            );
            EVID_CASE(API_SIM_EF_EMLPP           );
            EVID_CASE(API_SIM_EF_AAEM            );
            EVID_CASE(API_SIM_EF_ECC             );
            EVID_CASE(API_SIM_EF_CBMIR           );
            EVID_CASE(API_SIM_EF_NIA             );
            EVID_CASE(API_SIM_EF_KCGPRS          );
            EVID_CASE(API_SIM_EF_LOCIGPRS        );
            EVID_CASE(API_SIM_EF_SUME            );
            EVID_CASE(API_SIM_EF_PLMNWACT        );
            EVID_CASE(API_SIM_EF_OPLMNWACT       );
            EVID_CASE(API_SIM_EF_HPLMNACT        );
            EVID_CASE(API_SIM_EF_CPBCCH          );
            EVID_CASE(API_SIM_EF_INVSCAN         );
            EVID_CASE(API_SIM_EF_RPLMNAC         );
            EVID_CASE(API_SIM_EF_ICCID           );
            EVID_CASE(API_SIM_EF_ELP             );
            EVID_CASE(API_SIM_EF_IMG             );
            EVID_CASE(API_SIM_EF_MBDN            );
            EVID_CASE(API_USIM_EF_LI               );
            EVID_CASE(API_USIM_EF_ARR            );
            EVID_CASE(API_USIM_EF_IMSI           );
            EVID_CASE(API_USIM_EF_KEYS           );
            EVID_CASE(API_USIM_EF_KEYS_PS        );
            EVID_CASE(API_USIM_EF_DCK                );
            EVID_CASE(API_USIM_EF_HPLMN          );
            EVID_CASE(API_USIM_EF_CNL                );
            EVID_CASE(API_USIM_EF_ACM_MAX        );
            EVID_CASE(API_USIM_EF_UST            );
            EVID_CASE(API_USIM_EF_ACM            );
            EVID_CASE(API_USIM_EF_FDN            );
            EVID_CASE(API_USIM_EF_SMS            );
            EVID_CASE(API_USIM_EF_GID1           );
            EVID_CASE(API_USIM_EF_GID2           );
            EVID_CASE(API_USIM_EF_MSISDN         );
            EVID_CASE(API_USIM_EF_PUCT           );
            EVID_CASE(API_USIM_EF_SMSP           );
            EVID_CASE(API_USIM_EF_SMSS           );
            EVID_CASE(API_USIM_EF_CBMI           );
            EVID_CASE(API_USIM_EF_SPN            );
            EVID_CASE(API_USIM_EF_SMSR           );
            EVID_CASE(API_USIM_EF_CBMID          );
            EVID_CASE(API_USIM_EF_SDN            );
            EVID_CASE(API_USIM_EF_EXT2           );
            EVID_CASE(API_USIM_EF_EXT3           );
            EVID_CASE(API_USIM_EF_BDN            );
            EVID_CASE(API_USIM_EF_EXT5           );
            EVID_CASE(API_USIM_EF_CBMIR          );
            EVID_CASE(API_USIM_EF_EXT4           );
            EVID_CASE(API_USIM_EF_EST            );
            EVID_CASE(API_USIM_EF_ACL            );
            EVID_CASE(API_USIM_EF_CMI            );
            EVID_CASE(API_USIM_EF_START_HFN      );
            EVID_CASE(API_USIM_EF_THRESHOLD      );
            EVID_CASE(API_USIM_EF_PLMNWACT       );
            EVID_CASE(API_USIM_EF_OPLMNWACT      );
            EVID_CASE(API_USIM_EF_HPLMNWACT      );
            EVID_CASE(API_USIM_EF_EHPLMN         );
            EVID_CASE(API_USIM_EF_PS_LOCI        );
            EVID_CASE(API_USIM_EF_ACC            );
            EVID_CASE(API_USIM_EF_FPLMN          );
            EVID_CASE(API_USIM_EF_LOCI           );
            EVID_CASE(API_USIM_EF_ICI            );
            EVID_CASE(API_USIM_EF_OCI            );
            EVID_CASE(API_USIM_EF_ICT            );
            EVID_CASE(API_USIM_EF_OCT            );
            EVID_CASE(API_USIM_EF_AD                   );
            EVID_CASE(API_USIM_EF_EMLPP          );
            EVID_CASE(API_USIM_EF_AAEM             );
            EVID_CASE(API_USIM_EF_ECC                );
            EVID_CASE(API_USIM_EF_HIDDENKEY      );
            EVID_CASE(API_USIM_EF_NETPAR         );
            EVID_CASE(API_USIM_EF_PNN            );
            EVID_CASE(API_USIM_EF_OPL            );
            EVID_CASE(API_USIM_EF_MBDN           );
            EVID_CASE(API_USIM_EF_EXT6           );
            EVID_CASE(API_USIM_EF_MBI            );
            EVID_CASE(API_USIM_EF_MWIS           );
            EVID_CASE(API_USIM_EF_CFIS           );
            EVID_CASE(API_USIM_EF_EXT7           );
            EVID_CASE(API_USIM_EF_SPDI             );
            EVID_CASE(API_USIM_EF_MMSN             );
            EVID_CASE(API_USIM_EF_EXT8             );
            EVID_CASE(API_USIM_EF_MMSICP           );
            EVID_CASE(API_USIM_EF_MMSUP          );
            EVID_CASE(API_USIM_EF_MMSUCP           );
            EVID_CASE(API_USIM_EF_NIA                );
            EVID_CASE(API_USIM_EF_VGCS             );
            EVID_CASE(API_USIM_EF_VGCSS          );
            EVID_CASE(API_USIM_EF_VBS                );
            EVID_CASE(API_USIM_EF_VBSS             );
            EVID_CASE(API_USIM_EF_VGCSCA           );
            EVID_CASE(API_USIM_EF_VBSCA          );
            EVID_CASE(API_USIM_EF_GBAP             );
            EVID_CASE(API_USIM_EF_MSK                );
            EVID_CASE(API_USIM_EF_MUK                );
            EVID_CASE(API_USIM_EF_GBANL          );
            EVID_CASE(API_USIM_EF_EHPLMNPI       );
            EVID_CASE(API_USIM_EF_LRPLMNSI       );
            EVID_CASE(API_USIM_EF_NAFKCA           );
            EVID_CASE(API_USIM_EF_MEXE_ST        );
            EVID_CASE(API_USIM_EF_ARPK           );
            EVID_CASE(API_USIM_EF_TPRK           );
            EVID_CASE(API_USIM_EF_ORPK           );
            EVID_CASE(API_USIM_EF_KC             );
            EVID_CASE(API_USIM_EF_KC_GPRS        );
            EVID_CASE(API_USIM_EF_CPBCCH         );
            EVID_CASE(API_USIM_EF_INVSCAN        );
            EVID_CASE(API_USIM_EF_DIR            );
            EVID_CASE(API_USIM_EF_ARR_MF             );
            EVID_CASE(API_USIM_EF_GB_PBR             );
            EVID_CASE(API_USIM_EF_GB_IAP             );
            EVID_CASE(API_USIM_EF_GB_IAP1            );
            EVID_CASE(API_USIM_EF_GB_ADN             );
            EVID_CASE(API_USIM_EF_GB_ADN1            );
            EVID_CASE(API_USIM_EF_GB_EXT1            );
            EVID_CASE(API_USIM_EF_GB_PBC             );
            EVID_CASE(API_USIM_EF_GB_PBC1            );
            EVID_CASE(API_USIM_EF_GB_GRP             );
            EVID_CASE(API_USIM_EF_GB_GRP1            );
            EVID_CASE(API_USIM_EF_GB_AAS             );
            EVID_CASE(API_USIM_EF_GB_GAS             );
            EVID_CASE(API_USIM_EF_GB_ANRA            );
            EVID_CASE(API_USIM_EF_GB_ANRB            );
            EVID_CASE(API_USIM_EF_GB_ANRC            );
            EVID_CASE(API_USIM_EF_GB_ANRA1       );
            EVID_CASE(API_USIM_EF_GB_ANRB1       );
            EVID_CASE(API_USIM_EF_GB_ANRC1       );
            EVID_CASE(API_USIM_EF_GB_SNE             );
            EVID_CASE(API_USIM_EF_GB_SNE1            );
            EVID_CASE(API_USIM_EF_GB_CCP1            );
            EVID_CASE(API_USIM_EF_GB_UID             );
            EVID_CASE(API_USIM_EF_GB_UID1            );
            EVID_CASE(API_USIM_EF_GB_PSC             );
            EVID_CASE(API_USIM_EF_GB_CC            );
            EVID_CASE(API_USIM_EF_GB_PUID            );
            EVID_CASE(API_USIM_EF_GB_EMAIL       );
            EVID_CASE(API_USIM_EF_GB_EMAIL1      );
            EVID_CASE(API_USIM_EF_PBR                  );
            EVID_CASE(API_USIM_EF_IAP                  );
            EVID_CASE(API_USIM_EF_IAP1               );
            EVID_CASE(API_USIM_EF_ADN                  );
            EVID_CASE(API_USIM_EF_ADN1             );
            EVID_CASE(API_USIM_EF_EXT1             );
            EVID_CASE(API_USIM_EF_PBC                  );
            EVID_CASE(API_USIM_EF_PBC1             );
            EVID_CASE(API_USIM_EF_GRP                  );
            EVID_CASE(API_USIM_EF_GRP1             );
            EVID_CASE(API_USIM_EF_AAS                  );
            EVID_CASE(API_USIM_EF_GAS                  );
            EVID_CASE(API_USIM_EF_ANRA             );
            EVID_CASE(API_USIM_EF_ANRB             );
            EVID_CASE(API_USIM_EF_ANRC             );
            EVID_CASE(API_USIM_EF_ANRA1            );
            EVID_CASE(API_USIM_EF_ANRB1            );
            EVID_CASE(API_USIM_EF_ANRC1            );
            EVID_CASE(API_USIM_EF_SNE                  );
            EVID_CASE(API_USIM_EF_SNE1             );
            EVID_CASE(API_USIM_EF_CCP1             );
            EVID_CASE(API_USIM_EF_UID                  );
            EVID_CASE(API_USIM_EF_UID1             );
            EVID_CASE(API_USIM_EF_PSC                  );
            EVID_CASE(API_USIM_EF_CC                   );
            EVID_CASE(API_USIM_EF_PUID             );
            EVID_CASE(API_USIM_EF_EMAIL          );
            EVID_CASE(API_USIM_EF_EMAIL1           );
        default:
            return "Unknown Event name";
    }
}

PCSTR TS_GetEventName(UINT32 nEvId)
{
    switch (nEvId)
    {
            EVID_CASE(EV_DM_POWER_ON_IND                );
            EVID_CASE(EV_DM_POWER_OFF_IND               );
            EVID_CASE(EV_TIM_ALRAM_IND               );
            EVID_CASE(EV_DM_UART_RECEIVE_DATA_IND               );
            EVID_CASE(EV_DM_EXTI_IND               );
            EVID_CASE(EV_DM_GPIO_IND               );
            EVID_CASE(EV_DM_GPADC_IND               );
            EVID_CASE(EV_TIM_SET_TIME_IND               );
            EVID_CASE(EV_DM_RECORD_END_IND               );
            EVID_CASE(EV_KEY_DOWN               );
            EVID_CASE(EV_KEY_PRESS               );
            EVID_CASE(EV_KEY_UP               );
            EVID_CASE(EV_TIMER               );
            EVID_CASE(EV_PM_BC_IND               );
            EVID_CASE(EV_CFW_INIT_IND               );
            EVID_CASE(EV_CFW_SRV_STATUS_IND               );
            EVID_CASE(EV_CFW_NW_SIGNAL_QUALITY_IND               );
            EVID_CASE(EV_CFW_NW_REG_STATUS_IND               );
            EVID_CASE(EV_CFW_CC_SPEECH_CALL_IND);
            EVID_CASE(EV_CFW_CC_CRSSINFO_IND);
            EVID_CASE(EV_CFW_NEW_SMS_IND);
            EVID_CASE(EV_CFW_SMS_INFO_IND);
            EVID_CASE(EV_CFW_CC_CALL_INFO_IND);
            EVID_CASE(EV_CFW_SIM_STATUS_IND);
            EVID_CASE(EV_CFW_SS_USSD_IND);
            EVID_CASE(EV_CFW_CC_RELEASE_CALL_IND);
            EVID_CASE(EV_CFW_CC_ERROR_IND);
            EVID_CASE(EV_CFW_SAT_CMDTYPE_IND);
            EVID_CASE(EV_CFW_PBK_IND);
            EVID_CASE(EV_CFW_NW_NETWORKINFO_IND);
            EVID_CASE(EV_CFW_GPRS_CXT_ACTIVE_IND);
            EVID_CASE(EV_CFW_GPRS_CXT_DEACTIVE_IND);
            EVID_CASE(EV_CFW_GPRS_MOD_IND);
            EVID_CASE(EV_CFW_GPRS_STATUS_IND);
            EVID_CASE(EV_CFW_GPRS_DATA_IND);
            EVID_CASE(EV_CFW_NW_SET_BAND_RSP);
            EVID_CASE(EV_CFW_NW_GET_IMEI_RSP);
            EVID_CASE(EV_CFW_NW_GET_SIGNAL_QUALITY_RSP);
            EVID_CASE(EV_CFW_NW_SET_REGISTRATION_RSP);
            EVID_CASE(EV_CFW_NW_DEREGISTER_RSP);
            EVID_CASE(EV_CFW_NW_GET_AVAIL_OPERATOR_RSP);
            EVID_CASE(EV_CFW_NW_ABORT_LIST_OPERATORS_RSP);
            EVID_CASE(EV_CFW_GPRS_CTX_MODIFY_ACC_RSP);
            EVID_CASE(EV_CFW_GPRS_ATT_RSP);
            EVID_CASE(EV_CFW_GPRS_ACT_RSP);
            EVID_CASE(EV_CFW_GPRS_CXT_ACTIVE_RSP);
            EVID_CASE(EV_CFW_GPRS_MOD_RSP);
            EVID_CASE(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP    );
            EVID_CASE(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP  );
            EVID_CASE(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP  );
            EVID_CASE(EV_CFW_CC_RELEASE_CALL_RSP          );
            EVID_CASE(EV_CFW_SS_QUERY_CLIP_RSP            );
            EVID_CASE(EV_CFW_SS_SET_CALL_WAITING_RSP      );
            EVID_CASE(EV_CFW_SS_QUERY_CALL_WAITING_RSP    );
            EVID_CASE(EV_CFW_SS_SET_CALL_FORWARDING_RSP   );
            EVID_CASE(EV_CFW_SS_QUERY_CALL_FORWARDING_RSP );
            EVID_CASE(EV_CFW_SS_SET_FACILITY_LOCK_RSP     );
            EVID_CASE(EV_CFW_SS_GET_FACILITY_LOCK_RSP     );
            EVID_CASE(EV_CFW_SS_CHANGE_PWD_RSP            );
            EVID_CASE(EV_CFW_SS_QUERY_FACILITY_LOCK_RSP   );
            EVID_CASE(EV_CFW_SS_QUERY_CLIR_RSP            );
            EVID_CASE(EV_CFW_SS_QUERY_COLP_RSP            );
            EVID_CASE(EV_CFW_SS_SEND_USSD_RSP             );
            EVID_CASE(EV_CFW_SS_TERMINATE_RSP             );
            EVID_CASE(EV_CFW_SS_QUERY_COLR_RSP            );
            EVID_CASE(EV_CFW_SIM_ADD_PBK_RSP                );
            EVID_CASE(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP       );
            EVID_CASE(EV_CFW_SIM_GET_PBK_ENTRY_RSP          );
            EVID_CASE(EV_CFW_SIM_LIST_PBK_ENTRY_RSP         );
            EVID_CASE(EV_CFW_SIM_GET_PBK_STRORAGE_RSP       );
            EVID_CASE(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP  );
            EVID_CASE(EV_CFW_SIM_GET_PROVIDER_ID_RSP        );
            EVID_CASE(EV_CFW_SIM_CHANGE_PWD_RSP             );
            EVID_CASE(EV_CFW_SIM_GET_AUTH_STATUS_RSP        );
            EVID_CASE(EV_CFW_SIM_ENTER_AUTH_RSP             );
            EVID_CASE(EV_CFW_SIM_SET_FACILITY_LOCK_RSP      );
            EVID_CASE(EV_CFW_SIM_GET_FACILITY_LOCK_RSP      );
            EVID_CASE(EV_CFW_SIM_DELETE_MESSAGE_RSP         );
            EVID_CASE(EV_CFW_SIM_WRITE_MESSAGE_RSP          );
            EVID_CASE(EV_CFW_SIM_READ_MESSAGE_RSP           );
            EVID_CASE(EV_CFW_SIM_LIST_MESSAGE_RSP           );
            EVID_CASE(EV_CFW_SIM_GET_SMS_STORAGE_INFO_RSP   );
            EVID_CASE(EV_CFW_SIM_READ_BINARY_RSP            );
            EVID_CASE(EV_CFW_SIM_UPDATE_BINARY_RSP          );
            EVID_CASE(EV_CFW_SIM_GET_SMS_PARAMETERS_RSP     );
            EVID_CASE(EV_CFW_SIM_SET_SMS_PARAMETERS_RSP     );
            EVID_CASE(EV_CFW_SIM_GET_MR_RSP                 );
            EVID_CASE(EV_CFW_SIM_SET_MR_RSP                 );
            EVID_CASE(EV_CFW_SIM_COMPOSE_PDU_RSP            );
            EVID_CASE(EV_CFW_SAT_ACTIVATION_RSP             );
            EVID_CASE(EV_CFW_SAT_GET_STATUS_RSP             );
            EVID_CASE(EV_CFW_SAT_RESPONSE_RSP               );
            EVID_CASE(EV_CFW_SAT_GET_INFORMATION_RSP        );
            EVID_CASE(EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP );
            EVID_CASE(EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP );
            EVID_CASE(EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP );
            EVID_CASE(EV_CFW_SIM_SET_PBKMODE_RSP              );
            EVID_CASE(EV_CFW_SIM_GET_PBKMODE_RSP              );
            EVID_CASE(EV_CFW_SMS_DELETE_MESSAGE_RSP      );
            EVID_CASE(EV_CFW_SMS_GET_STORAGE_RSP         );
            EVID_CASE(EV_CFW_SMS_SET_STORAGE_RSP         );
            EVID_CASE(EV_CFW_SMS_LIST_MESSAGE_RSP        );
            EVID_CASE(EV_CFW_SMS_READ_MESSAGE_RSP        );
            EVID_CASE(EV_CFW_SMS_SEND_MESSAGE_RSP        );
            EVID_CASE(EV_CFW_SMS_SET_CENTER_ADDR_RSP     );
            EVID_CASE(EV_CFW_SMS_GET_CENTER_ADDR_RSP     );
            EVID_CASE(EV_CFW_SMS_SET_TEXT_MODE_PARAM_RSP );
            EVID_CASE(EV_CFW_SMS_GET_TEXT_MODE_PARAM_RSP );
            EVID_CASE(EV_CFW_SMS_WRITE_MESSAGE_RSP       );
            EVID_CASE(EV_CFW_SMS_LIST_MESSAGE_HRD_RSP    );
            EVID_CASE(EV_CFW_SMS_COPY_MESSAGE_RSP        );
            EVID_CASE(EV_CFW_SMS_SET_UNREAD2READ_RSP     );
            EVID_CASE(EV_CFW_SMS_SET_UNSENT2SENT_RSP     );
            EVID_CASE(EV_CFW_PBK_ADD_RSP                        );
            EVID_CASE(EV_CFW_PBK_UPDATE_ENTRY_RSP               );
            EVID_CASE(EV_CFW_PBK_DELETE_RSP                     );
            EVID_CASE(EV_CFW_PBK_DELETE_BATCH_ENTRIES_RSP       );
            EVID_CASE(EV_CFW_PBK_FIND_ENTRY_RSP                 );
            EVID_CASE(EV_CFW_PBK_LIST_ENTRIES_RSP               );
            EVID_CASE(EV_CFW_PBK_GET_ENTRY_RSP                  );
            EVID_CASE(EV_CFW_PBK_COPY_ENTRIES_RSP               );
            EVID_CASE(EV_CFW_PBK_ADD_ENTRY_ITEM_RSP             );
            EVID_CASE(EV_CFW_PBK_GET_ENTRY_ITEMS_RSP            );
            EVID_CASE(EV_CFW_PBK_DELETE_ENTRY_ITEM_RSP          );
            EVID_CASE(EV_CFW_PBK_ADD_CALLLOG_ENTTRY_RSP         );
            EVID_CASE(EV_CFW_PBK_GET_CALLLOG_ENTRY_RSP          );
            EVID_CASE(EV_CFW_PBK_LIST_CALLLOG_ENTRIES_RSP       );
            EVID_CASE(EV_CFW_PBK_DELETE_CALLLOG_ALL_ENTRIES_RSP );
            EVID_CASE(EV_CFW_PBK_CANCEL_RSP                     );
            EVID_CASE(EV_CFW_ME_SET_FACILITY_LOCK_RSP );
            EVID_CASE(EV_CFW_ME_GET_FACILITY_LOCK_RSP );
            EVID_CASE(EV_CFW_ME_CHANGE_PWD_RSP        );
            EVID_CASE(EV_CFW_TCPIP_SOCKET_CONNECT_RSP);
            EVID_CASE(EV_CFW_TCPIP_SOCKET_SEND_RSP);
            EVID_CASE(EV_CFW_TCPIP_SOCKET_CLOSE_RSP);
            EVID_CASE(EV_CFW_TCPIP_REV_DATA_IND);
            EVID_CASE(EV_CFW_ICMP_DATA_IND);
            EVID_CASE(EV_CFW_DNS_RESOLV_ERR_IND);
            EVID_CASE(EV_CFW_DNS_RESOLV_SUC_IND);
            EVID_CASE(EV_CFW_TCPIP_CLOSE_IND);
            EVID_CASE(EV_CFW_TCPIP_ERR_IND);
#ifdef LTE_NBIOT_SUPPORT
	    EVID_CASE(EV_CFW_GPRS_EDRX_IND        );
	    EVID_CASE(EV_CFW_GPRS_CCIOTOPT_IND        );
	    EVID_CASE(EV_CFW_GPRS_APNCR_IND        );
	    EVID_CASE(EV_CFW_ERRC_CONNSTATUS_IND        );
#endif
        default:
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_API_TS_ID)|TDB|TNB_ARG(1), TSTR("Get Msg, %d\n",0x08600001), nEvId);
            return "Undefine Event name";
        }
    }
}

