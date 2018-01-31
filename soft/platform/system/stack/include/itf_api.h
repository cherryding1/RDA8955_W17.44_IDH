//================================================================================
//  File       : ITF_API.H
//--------------------------------------------------------------------------------
//  Scope      : Interface Messages provided by API
//               This file is in accordance with StackCom's Application Protocol
//               Interface Specification document, version 2.12
//               Please refer to this document if you need more information on 
//               some particular message fields. 
//  History    :
//--------------------------------------------------------------------------------
//  201301129 | DMX   | add GPRS detach type for network mod 1
//  20130902 | DMX   |  change the api_CrssNameInd_t from union to struct
//  Jun 09 06  |  ABA   | Add API_SIM_EF_ICCID and API_SIM_EF_ELP
//  Jun 02 06  |  MCE   | Added field Roaming to api_NwPsRegStatusInd_t
//  Apr 26 06  |  MCE   | changed ChanMode to an u32 in api_CcChanModeInd_t ;
//  Mar 30 06  |  MCE   | Added api_CongestInd_t and API_PS_ULCONGEST_IND 
//             |        | Not sent to MMI yet, Ul Flow Ctrl being implemented in 
//             |        | API for now
//  Mar 02 06  |  ADA   | API_CSD_QUEUE is queue 0 (created in sap_main.c)
//  Jan 27 06  |  FCH   | Added API_CSD_CRLPSET_REQ, API_CSD_CRLPRD_REQ and
//             |        | API_CSD_CRLPRD_IND
//  Jan 10 06  |  MCE   | Added API_CC_CHANMODE_IND and api_CcChanModeInd_t
//  Nov 28 05  |  OTH   | Add 2 defines for CALLHOLD and MPTY support
//  Nov 22 05  |  FCH   | MMP and MMC spys are merge together and API_MMP_SPY is
//             |        | changed to API_MM_SPY. Add API_MMSPY_T3212DUR
//  Nov 21 05  |  FCH   | Added spy fields enum for MMP, LLC and CB
//  Nov 14 05  |  MCE   | Added spy fields enum for RR and RLU
//             |        | Added API_DBG_INFO_CMD
//  Nov 14 05  |  ADA   | Debug info.
//  Nov 02 05  |  ABA   | Change Fetch length to 16 bits
//  Sep 26 05  |  MCE   | Suppressed FreqBandSupport from api_StackCfg_t
//  Sep 26 05  |  FCH   | Change order of fields in api_PSDReqCtrl_t
//             |        | Add macro API_PSALLOC_DATAREQ
//  Sep 06 05  |  FCH   | Add AutoGprsAttach in api_StackCfg_t
//  Aug 09 05  |  OTH   | Add the TS10 duration in api_StackCfg_t
//  Jun 15 05  |  PFA   | Add LIST_ABORT_IND msgs 
//  Jun 14 05  |OTH/FCH | Remove Api_NwDetachReq
//  Jun 06 05  |  ABA   | ISSUE 19: poll interval for simtoolkit
//             |        | Implementation of the simtoolkit polling 
//             |        | (API_SIM_POLL_REQ and API_SIM_POLL_OFF_REQ)
//  Jun 01 05  |  FCH   | - Changes in api_SmsCBActivateReq_t
//             |        | - Addition of API_SMSCB_PAGE_IND and the associated
//             |        |   structure (api_SmsCbPageInd_t) to provide MMI with
//             |        |   page of a SMSCB message
//  May 11 05  |  MCE   | - Added TstLlcPDuSize in api_StackCfg_t
//  Mar 25 05  |  MCE   | - Removed FrequencyBands from api_StackCfg_t
//  Mar 04 05  |  MCE   | - moved definition of StackCfg structure from sap_cust.h
//  Mar 03 05  |  OTH   | - Addition of the API_SIM_CLOSE_IND
//  Feb 23 05  |  MCE   | - Addition of the Band in api_StartReq_t                
//  Jan 20 05  |  ABA   | - Add file indication in some SIM_CNF message. File item
//             |        | is now present for all read and write,seek, increase,
//             |        | invalidate, rehabilitate, DFStatus and EFStatus.
//             |        | - Modification of the file indication in the confirm
//             |        | message (file item always follow sw1,sw2 in the cnf
//             |        | structure).
//  Dec 07 04  |  FCH   | Changes in the SMS-CB interface (api_SmsCBActivateReq_t,
//             |        | api_SmsCBDeActivateReq_t and api_SmsCBReceiveInd_t)
//  Dec 01 04  |  MCE   | - Improvement of api_NwListInd_t
//  Nov 18 04  |  MCE   | - Addition of API_NW_REG_PENDING
//  Nov 11 04  |  OTH   | - Modification of the api_CrssFacilityReqIE_t for the
//             |        |   UUS needs
//  Nov 04 04  |  OTH   | - Remove the api_CcCallWaintingInd event
//  Nov 02 04  |  ABA   | - New pdp context management (sm) interface
//  Oct 29 04  |  OTH   | - Addition of the API_STOP_CNF
//  Oct 22 04  |  FCH   | - Renamed API_NW_CSDETACH_REQ into API_NW_DETACH_REQ
//             |        | - Add field PowerOff in api_StopReq_t
//             |        | - Added API_NWKINFO_IND and api_NwkInfoInd_t
//  Oct 21 04  |  MCE   | - Renamed api_NwCsDetachReq into api_NwDetchReq      
//  Oct 12 04  |  OTH   | - Addition of the Notify Ind and Notify Req in CC-API
//  Sep 29 04  |  OTH   | - Removed Ussd specific events
//  Sep 28 04  |  MCE   | - Removed unused "Reserved" fields
//  Sep 28 04  |  MCE   | - Switched API_PDP_ACK and API_PDP_UNACK values
//  Sep 24 04  |  MCE   | - Changed api_PSDataReq_t and api_PSDataInd_t to 
//             |        |   interlayer types
//             |        | - Added the API_PSDATA_REQ_SIZE macro
//             |        | - Added the API_CSDATA_REQ_SIZE macro
//  Sep 16 04  |BBE/MCE | USSD API and update of SS API
//  Aug 13 04  |  OTH   | CRSS API
//  Jul 05 04  |  OTH   | TI handled by CC   
//  Jun 22 04  |  OTH   | SMS Api improvments
//  Jun 09 04  |  OTH   | Add CC AUDIO ON interface to indicate MMI, audio can be
//             |        | opened and APIData interface for CSD and Fax.
//  May 25 04  |  OTH   | Add CC MODIFY interface
//  May 24 04  |  OTH   | SimFileStatus split in SimElemFileStatus and
//             |        | SimDedicFileStatus.
//  Mar 15 04  |  OTH   | CC modifications
//  Mar 10 04  |  PFA   | Offset Sim from u8 to u16
//             |        | Data becomes Data[256] in Sim_FileStatusCnf
//  Feb 23 04  |  MCE   | Creation            
//================================================================================

#ifndef __ITF_API_H__
#define __ITF_API_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_defs.h"
#ifdef LTE_NBIOT_SUPPORT
#include "itf_msgc.h"
#include "../../nbiot/pub/tgl.hec"
#include "../../nbiot/pub/tgl.het"
#include "../../nbiot/ps/pub/dba/include/eas_pub_dba.h"
#endif
//#include "stk_debug.h"

#define HVY_API_GENERAL_IN       ( HVY_API + 0     )
#define HVY_API_GENERAL_OUT      ( HVY_API + 0x80  )
#define HVY_API_NW_IN            ( HVY_API + 0x100 )
#define HVY_API_NW_OUT           ( HVY_API + 0x180 )
#define HVY_API_CC_IN            ( HVY_API + 0x200 )
#define HVY_API_CC_OUT           ( HVY_API + 0x280 )
#define HVY_API_SS_IN            ( HVY_API + 0x300 )
#define HVY_API_SS_OUT           ( HVY_API + 0x380 )
#define HVY_API_PDP_IN           ( HVY_API + 0x400 )
#define HVY_API_PDP_OUT          ( HVY_API + 0x480 )
#define HVY_API_PSD_IN           ( HVY_API + 0x500 )
#define HVY_API_PSD_OUT          ( HVY_API + 0x580 )
#define HVY_API_SIM_IN           ( HVY_API + 0x600 )
#define HVY_API_SIM_OUT          ( HVY_API + 0x680 )
#define HVY_API_SMSPP_IN         ( HVY_API + 0x700 )
#define HVY_API_SMSPP_OUT        ( HVY_API + 0x780 )
#define HVY_API_SMSCB_IN         ( HVY_API + 0x800 )
#define HVY_API_SMSCB_OUT        ( HVY_API + 0x880 )
#define HVY_API_CSD_IN           ( HVY_API + 0x900 )
#define HVY_API_CSD_OUT          ( HVY_API + 0x980 )

#define HVY_API_CTR_IN           ( HVY_API + 0xa00 )
#define HVY_API_CTR_OUT          ( HVY_API + 0xa80 )
#ifdef LTE_NBIOT_SUPPORT
#define HVY_API_RABM_IN          ( HVY_API + 0xb00)
#define HVY_API_RABM_OUT         ( HVY_API + 0xb80 )
#endif
// values (HVY_API+0xFF00) and above are RESERVED
#define HVY_API_RESERVED         ( HVY_API + 0xFF00 )

/*
** Primitive ID definitions
*/
// General: MMI -> API
#define API_START_REQ            ( HVY_API_GENERAL_IN + 0 )
#define API_STOP_REQ             ( HVY_API_GENERAL_IN + 1 )
#define API_IMEI_REQ             ( HVY_API_GENERAL_IN + 2 )
#define API_QUAL_REPORT_REQ      ( HVY_API_GENERAL_IN + 3 )
#define API_NWKINFO_IND          ( HVY_API_GENERAL_IN + 4 )
#define API_DBGINFO_CMD          ( HVY_API_GENERAL_IN + 5 )
#define API_CURCELL_INFO_REQ     ( HVY_API_GENERAL_IN + 6 )
#define API_ADJCELL_INFO_REQ     ( HVY_API_GENERAL_IN + 7 )
#define API_SYNC_INFO_REQ           ( HVY_API_GENERAL_IN + 8 )

//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
#define API_NV_CHANGE_IND          ( HVY_API_GENERAL_IN + 9 )
//adapter mmc
#define API_MMC_NW_PS_REG_IND          ( HVY_API_GENERAL_IN + 0x0a )
#define API_MMC_STK_STOP_CNF           ( HVY_API_GENERAL_IN + 0x0b )
#define API_MMC_NW_LIST_IND            ( HVY_API_GENERAL_IN + 0x0c)
#define API_MMC_SIM_STATUSERROR_IND    ( HVY_API_GENERAL_IN + 0x0d )
#define API_MMC_NWKINFO_IND            ( HVY_API_GENERAL_IN + 0x0e )

//adapter gms	
#define API_CMC_SMSPP_ACK_IND          ( HVY_API_GENERAL_IN + 0x0f )
#define API_CMC_SMSPP_ERROR_IND        ( HVY_API_GENERAL_IN + 0x10 )
#define API_CMC_SMSPP_RECEIVE_IND      ( HVY_API_GENERAL_IN + 0x11 )

//adapter esm
#define API_CMC_PDP_CTXACTIVATE_CNF    ( HVY_API_GENERAL_IN + 0x12 )
#define API_CMC_PDP_CTXACTIVATE_REJ    ( HVY_API_GENERAL_IN + 0x13 )
#define API_CMC_PDP_CTXSMINDEX_IND     ( HVY_API_GENERAL_IN + 0x14 )
#define API_CMC_PDP_CTXACTIVATE_IND    ( HVY_API_GENERAL_IN + 0x15 )
#define API_CMC_PDP_CTXMODIFY_CNF      ( HVY_API_GENERAL_IN + 0x16)
#define API_CMC_PDP_CTXMODIFY_IND      ( HVY_API_GENERAL_IN + 0x17 )
#define API_CMC_PDP_CTXDEACTIVATE_CNF  ( HVY_API_GENERAL_IN + 0x18 )
#define API_CMC_PDP_CTXDEACTIVATE_IND  ( HVY_API_GENERAL_IN + 0x19 )

//adapter rabm
#define API_RABM_PSDATA_IND            ( HVY_API_GENERAL_IN + 0x1a )

//adapter errc
#define API_ERRC_CAPI_RMI_FIELD_STRENGTH_IND            ( HVY_API_GENERAL_IN + 0x1b )

//
#define API_MMC_NW_LIST_ABORT_IND    ( HVY_API_GENERAL_IN + 0x1c )
#define API_MMC_ERRC_CONNSTATUS_IND ( HVY_API_GENERAL_IN + 0x1d )
#define API_CMC_EPS_CTXALLOCATION_CNF  ( HVY_API_GENERAL_IN + 0x1e )
#define API_CMC_EPS_CTXALLOCATION_ERR  ( HVY_API_GENERAL_IN + 0x1f )
#define API_CMC_EPS_CTXACTIVATE_IND  ( HVY_API_GENERAL_IN + 0x20 )



//adapter mmc
#define API_MMC_START_REQ              ( HVY_API_GENERAL_OUT + 8 )
#define API_MMC_STOP_REQ               ( HVY_API_GENERAL_OUT + 9 )
#define API_MMC_NW_LIST_REQ            ( HVY_API_GENERAL_OUT + 0x0A )
#define API_MMC_NW_LIST_ABORT_REQ      ( HVY_API_GENERAL_OUT + 0x0B )
#define API_MMC_NW_SEL_REQ             ( HVY_API_GENERAL_OUT + 0x0C )

//adapter gms
#define API_CMC_SMSPP_SEND_REQ         ( HVY_API_GENERAL_OUT + 0x0D )
#define API_CMC_SMSPP_ACK_REQ          ( HVY_API_GENERAL_OUT + 0x0E )
#define API_CMC_SMSPP_ERROR_REQ        ( HVY_API_GENERAL_OUT + 0x0F )
#define API_CMC_SMSPP_SENDMMA_REQ      ( HVY_API_GENERAL_OUT + 0x10 )
#define API_CMC_SMSPP_ABORT_REQ        ( HVY_API_GENERAL_OUT + 0x11 )

//adapter esm
#define API_CMC_PDP_CTXACTIVATE_REQ    ( HVY_API_GENERAL_OUT + 0x12 )
#define API_CMC_PDP_CTXMODIFY_REQ      ( HVY_API_GENERAL_OUT + 0x13 )
#define API_CMC_PDP_CTXMODIFY_REJ      ( HVY_API_GENERAL_OUT + 0x14 )
#define API_CMC_PDP_CTXMODIFY_ACC      ( HVY_API_GENERAL_OUT + 0x15 )
#define API_CMC_PDP_CTXDEACTIVATE_REQ  ( HVY_API_GENERAL_OUT + 0x16 )
#define API_CMC_PDP_SETMINQOS_REQ      ( HVY_API_GENERAL_OUT + 0x17 )
#define API_CMC_DEFAULT_PDN_CONFIG_REQ ( HVY_API_GENERAL_OUT + 0x18 )

//adapter rabm
#define API_RABM_PSDATA_REQ            ( HVY_API_GENERAL_OUT + 0x19 )
#define API_MMC_NV_CHANGE_IND        ( HVY_API_GENERAL_OUT + 0x1a )
#define API_CMC_EPS_CTXALLOCATION_REQ  ( HVY_API_GENERAL_OUT + 0x1b )
#define API_CMC_EPS_CTXACTIVATE_RSP    ( HVY_API_GENERAL_OUT + 0x1c )
#define API_CMC_EPS_CTXACTIVATE_REJ    ( HVY_API_GENERAL_OUT + 0x1d )
#define API_CMC_EPS_CTXMODIFY_REJ    ( HVY_API_GENERAL_OUT + 0x1e )
#endif //#ifdef LTE_NBIOT_SUPPORT
// General: API -> MMI
#define API_IMEI_IND             ( HVY_API_GENERAL_OUT + 0 )
#define API_QUAL_REPORT_CNF      ( HVY_API_GENERAL_OUT + 1 )
#define API_STOP_CNF             ( HVY_API_GENERAL_OUT + 2 )
#define API_DBGINFO_IND          ( HVY_API_GENERAL_OUT + 3 )
#define API_CURCELL_INFO_IND     ( HVY_API_GENERAL_OUT + 4 )
#define API_ADJCELL_INFO_IND     ( HVY_API_GENERAL_OUT + 5 )
#define API_POWERLIST_IND     ( HVY_API_GENERAL_OUT + 6 )
#define API_SYNC_INFO_IND     ( HVY_API_GENERAL_OUT + 7 )


// Network services: MMI -> API
#define API_NW_SELMODE_REQ       ( HVY_API_NW_IN + 0 )
#define API_NW_LIST_REQ          ( HVY_API_NW_IN + 1 )
#define API_NW_LISTABORT_REQ     ( HVY_API_NW_IN + 2 )
#define API_NW_LISTABORT_IND     ( HVY_API_NW_IN + 3 )
#define API_NW_SEL_REQ           ( HVY_API_NW_IN + 4 )
#define API_NW_RESEL_REQ         ( HVY_API_NW_IN + 5 )
#define API_NW_PSATTACH_REQ      ( HVY_API_NW_IN + 6 )
#define API_NW_PSDETACH_REQ      ( HVY_API_NW_IN + 7 )
#define API_POWER_SCAN_REQ          ( HVY_API_NW_IN + 8 )
#define API_DETECT_MBS_REQ       ( HVY_API_NW_IN + 9 )

// Network services: API -> MMI
#define API_NW_CSREGSTATUS_IND   ( HVY_API_NW_OUT + 0 )
#define API_NW_PSREGSTATUS_IND   ( HVY_API_NW_OUT + 1 )
#define API_NW_LIST_IND          ( HVY_API_NW_OUT + 2 )
#define API_NW_STORELIST_IND (HVY_API_NW_OUT + 3)
#define API_POWER_SCAN_RSP (HVY_API_NW_OUT + 4)
#ifdef LTE_NBIOT_SUPPORT
#define API_NW_ERRC_CONNSTATUS_IND (HVY_API_NW_OUT + 5)
#endif //#ifdef LTE_NBIOT_SUPPORT
// CC: MMI -> API
#define API_CC_SETUP_REQ         ( HVY_API_CC_IN + 0    )
#define API_CC_CALLCONF_REQ      ( HVY_API_CC_IN + 1    )
#define API_CC_ALERT_REQ         ( HVY_API_CC_IN + 2    )
#define API_CC_CONNECT_REQ       ( HVY_API_CC_IN + 3    )
#define API_CC_DISC_REQ          ( HVY_API_CC_IN + 4    )
#define API_CC_RELEASE_REQ       ( HVY_API_CC_IN + 5    )
#define API_CC_RELEASECMPLT_REQ  ( HVY_API_CC_IN + 6    )
#define API_CC_STARTDTMF_REQ     ( HVY_API_CC_IN + 7    )
#define API_CC_STOPDTMF_REQ      ( HVY_API_CC_IN + 8    ) 
#define API_CC_CALLHOLD_REQ      ( HVY_API_CC_IN + 9    )
#define API_CC_CALLRETRIEVE_REQ  ( HVY_API_CC_IN + 0xA  )
#define API_CC_MODIFY_REQ        ( HVY_API_CC_IN + 0xB  )
#define API_CC_MODIFYCMPLT_REQ   ( HVY_API_CC_IN + 0xC  )
#define API_CC_MODIFYREJ_REQ     ( HVY_API_CC_IN + 0xD  )
#define API_CC_NOTIFY_REQ        ( HVY_API_CC_IN + 0xE  )
#define API_CC_FACILITY_REQ      ( HVY_API_CC_IN + 0xF  )
#define API_CC_USER_INFO_REQ     ( HVY_API_CC_IN + 0x10 )

// CC: API -> MMI
#define API_CC_TI_IND            ( HVY_API_CC_OUT + 0    )
#define API_CC_SETUP_IND         ( HVY_API_CC_OUT + 1    )
#define API_CC_CALLPROC_IND      ( HVY_API_CC_OUT + 2    )
#define API_CC_PROGRESS_IND      ( HVY_API_CC_OUT + 3    )
#define API_CC_ALERT_IND         ( HVY_API_CC_OUT + 4    )
#define API_CC_CONNECT_IND       ( HVY_API_CC_OUT + 5    )
#define API_CC_CONNECTACK_IND    ( HVY_API_CC_OUT + 6    )
#define API_CC_DISC_IND          ( HVY_API_CC_OUT + 7    )
#define API_CC_RELEASE_IND       ( HVY_API_CC_OUT + 8    )
#define API_CC_RELEASECMPLT_IND  ( HVY_API_CC_OUT + 9    )
#define API_CC_ERROR_IND         ( HVY_API_CC_OUT + 0xA  )
#define API_CC_CALLHOLD_CNF      ( HVY_API_CC_OUT + 0xB  )
#define API_CC_CALLRETRIEVE_CNF  ( HVY_API_CC_OUT + 0xC  )
#define API_CC_MODIFY_IND        ( HVY_API_CC_OUT + 0xD  )
#define API_CC_MODIFYCMPLT_IND   ( HVY_API_CC_OUT + 0xE  )
#define API_CC_MODIFYREJ_IND     ( HVY_API_CC_OUT + 0xF  )
#define API_CC_NOTIFY_IND        ( HVY_API_CC_OUT + 0x10 )
#define API_CC_AUDIOON_IND       ( HVY_API_CC_OUT + 0x11 )
#define API_CC_FACILITY_IND      ( HVY_API_CC_OUT + 0x12 )
#define API_CC_USER_INFO_IND     ( HVY_API_CC_OUT + 0x13 )
#define API_CC_CONG_CTRL_IND     ( HVY_API_CC_OUT + 0x14 )
#define API_CC_AOC_CCM_IND       ( HVY_API_CC_OUT + 0x15 )
#define API_CC_CHANMODE_IND      ( HVY_API_CC_OUT + 0x16 )

#define API_CC_REESTAB_BEGIN_IND  ( HVY_API_CC_OUT + 0x17 )
#define API_CC_REESTAB_CNF_IND    ( HVY_API_CC_OUT + 0x18 )



// SS: MMI -> API
#define API_SS_ACTION_REQ        ( HVY_API_SS_IN + 0 )

// SS: API -> MMI
#define API_SS_ACTION_CNF        ( HVY_API_SS_OUT + 0 )
#define API_SS_TI_IND            ( HVY_API_SS_OUT + 1 )
#define API_SS_ERROR_IND         ( HVY_API_SS_OUT + 2 )
#define API_SS_ACTION_IND        ( HVY_API_SS_OUT + 3 )

// PDP: MMI -> API
#define API_PDP_CTXACTIVATE_REQ   ( HVY_API_PDP_IN + 0 )
#define API_PDP_CTXMODIFY_REQ     ( HVY_API_PDP_IN + 1 )
#define API_PDP_CTXMODIFY_ACC     ( HVY_API_PDP_IN + 2 )
#define API_PDP_CTXDEACTIVATE_REQ ( HVY_API_PDP_IN + 3 )
#define API_PDP_SETMINQOS_REQ     ( HVY_API_PDP_IN + 4 )

//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT 
#define API_DEFAULT_PDN_CTXCONFIG_REQ ( HVY_API_PDP_IN + 5 )
#define API_EPS_CTXACTIVATE_RSP   ( HVY_API_PDP_IN + 6 )
#define API_EPS_CTXACTIVATE_REJ   ( HVY_API_PDP_IN + 7 )
#define API_EPS_CTXALLOCATE_REQ   ( HVY_API_PDP_IN + 8 )
#define API_EPS_CTXMODIFY_REJ    ( HVY_API_PDP_IN + 9 )
#endif //#ifdef LTE_NBIOT_SUPPORT
// PDP: API -> MMI
#define API_PDP_CTXACTIVATE_CNF   ( HVY_API_PDP_OUT + 0 )
#define API_PDP_CTXSMINDEX_IND     ( HVY_API_PDP_OUT + 1  )
#define API_PDP_CTXACTIVATE_REJ    ( HVY_API_PDP_OUT + 2  )
#define API_PDP_CTXACTIVATE_IND    ( HVY_API_PDP_OUT + 3  )
#define API_PDP_CTXMODIFY_CNF      ( HVY_API_PDP_OUT + 4  )
#define API_PDP_CTXMODIFY_REJ      ( HVY_API_PDP_OUT + 5  )
#define API_PDP_CTXMODIFY_IND      ( HVY_API_PDP_OUT + 6  )
#define API_PDP_CTXDEACTIVATE_IND  ( HVY_API_PDP_OUT + 7  )
#define API_PDP_CTXDEACTIVATE_CNF  ( HVY_API_PDP_OUT + 8  )

#ifdef LTE_NBIOT_SUPPORT
// Add for EPS Bearer Context
#define API_EPS_CTXACTIVATE_IND    ( HVY_API_PDP_OUT + 9  )
#define API_EPS_CTXALLOCATE_CNF    ( HVY_API_PDP_OUT + 10 )
#define API_EPS_CTXALLOCATE_REJ    ( HVY_API_PDP_OUT + 11 )
#endif //#ifdef LTE_NBIOT_SUPPORT
// Packet Switched Data: MMI -> API
#define API_PSDATA_REQ               ( HVY_API_PSD_IN + 0 )
#ifdef LTE_NBIOT_SUPPORT
#define API_PSDATA_REQ_LTE           ( HVY_API_PSD_IN + 1 )
#endif //#ifdef LTE_NBIOT_SUPPORT
#define API_PS_QUEUEDATA_REQ         ( HVY_API_PSD_IN + 1 + SXS_TIMER_EVT_ID ) // SX signal

// Packet Switched Data: API -> MMI
#define API_PSDATA_IND               ( HVY_API_PSD_OUT + 0 )
#define API_PS_QUEUECREATE_IND       ( HVY_API_PSD_OUT + 1 )
#define API_PS_ULCONGEST_IND         ( HVY_API_PSD_OUT + 2 ) //Not sent for now,
                                                             //UL Flow Ctrl done
                                                             //by API
#ifdef LTE_NBIOT_SUPPORT
#define API_PSDATA_IND_LTE           ( HVY_API_PSD_OUT + 3)     
#endif //#ifdef LTE_NBIOT_SUPPORT

// SIM: MMI -> API
#define API_SIM_RESET_REQ            ( HVY_API_SIM_IN + 0  )
#define API_SIM_READBINARY_REQ       ( HVY_API_SIM_IN + 1  )
#define API_SIM_UPDATEBINARY_REQ     ( HVY_API_SIM_IN + 2  )
#define API_SIM_READRECORD_REQ       ( HVY_API_SIM_IN + 3  )
#define API_SIM_UPDATERECORD_REQ     ( HVY_API_SIM_IN + 4  )
#define API_SIM_INVALIDATE_REQ       ( HVY_API_SIM_IN + 5  )
#define API_SIM_REHABILITATE_REQ     ( HVY_API_SIM_IN + 6  )
#define API_SIM_SEEK_REQ             ( HVY_API_SIM_IN + 7  )
#define API_SIM_INCREASE_REQ         ( HVY_API_SIM_IN + 8  )
#define API_SIM_VERIFYCHV_REQ        ( HVY_API_SIM_IN + 9  )
#define API_SIM_CHANGECHV_REQ        ( HVY_API_SIM_IN + 10 )
#define API_SIM_DISABLECHV_REQ       ( HVY_API_SIM_IN + 11 )
#define API_SIM_ENABLECHV_REQ        ( HVY_API_SIM_IN + 12 )
#define API_SIM_UNBLOCKCHV_REQ       ( HVY_API_SIM_IN + 13 )
#define API_SIM_ELEMFILESTATUS_REQ   ( HVY_API_SIM_IN + 14 )
#define API_SIM_DEDICFILESTATUS_REQ  ( HVY_API_SIM_IN + 15 )
#define API_SIM_TERMINALPROFILE_REQ  ( HVY_API_SIM_IN + 16 )
#define API_SIM_TERMINALENVELOPE_REQ ( HVY_API_SIM_IN + 17 )
#define API_SIM_TERMINALRESPONSE_REQ ( HVY_API_SIM_IN + 18 )
#define API_SIM_STKACTIVATE_REQ      ( HVY_API_SIM_IN + 19 )
#define API_SIM_OPEN_IND             ( HVY_API_SIM_IN + 20 )
#define API_SIM_CLOSE_IND            ( HVY_API_SIM_IN + 21 )
#define API_SIM_POLL_REQ             ( HVY_API_SIM_IN + 22 )
#define API_SIM_POLLOFF_REQ          ( HVY_API_SIM_IN + 23 )
#define API_SIM_SELECTAPPLICATION_REQ  ( HVY_API_SIM_IN + 24 )
#define API_SIM_SETUSIMPBK_REQ  ( HVY_API_SIM_IN + 25 )
#define API_SIM_RETRY_REQ            ( HVY_API_SIM_IN + 26 )
#define API_SIM_SEEK_ALL_REQ       ( HVY_API_SIM_IN + 27  )


// SIM: API -> MMI
#define API_SIM_RESET_CNF            ( HVY_API_SIM_OUT + 0  )
#define API_SIM_STATUSERROR_IND      ( HVY_API_SIM_OUT + 1  )
#define API_SIM_READBINARY_CNF       ( HVY_API_SIM_OUT + 2  )
#define API_SIM_UPDATEBINARY_CNF     ( HVY_API_SIM_OUT + 3  )
#define API_SIM_READRECORD_CNF       ( HVY_API_SIM_OUT + 4  )
#define API_SIM_UPDATERECORD_CNF     ( HVY_API_SIM_OUT + 5  )
#define API_SIM_INVALIDATE_CNF       ( HVY_API_SIM_OUT + 6  )
#define API_SIM_REHABILITATE_CNF     ( HVY_API_SIM_OUT + 7  )
#define API_SIM_SEEK_CNF             ( HVY_API_SIM_OUT + 8  )
#define API_SIM_INCREASE_CNF         ( HVY_API_SIM_OUT + 9  )
#define API_SIM_VERIFYCHV_CNF        ( HVY_API_SIM_OUT + 10 )
#define API_SIM_CHANGECHV_CNF        ( HVY_API_SIM_OUT + 11 )
#define API_SIM_DISABLECHV_CNF       ( HVY_API_SIM_OUT + 12 )
#define API_SIM_ENABLECHV_CNF        ( HVY_API_SIM_OUT + 13 )
#define API_SIM_UNBLOCKCHV_CNF       ( HVY_API_SIM_OUT + 14 )
#define API_SIM_ELEMFILESTATUS_CNF   ( HVY_API_SIM_OUT + 15 )
#define API_SIM_DEDICFILESTATUS_CNF  ( HVY_API_SIM_OUT + 16 )
#define API_SIM_TERMINALPROFILE_CNF  ( HVY_API_SIM_OUT + 17 )
#define API_SIM_TERMINALENVELOPE_CNF ( HVY_API_SIM_OUT + 18 )
#define API_SIM_TERMINALRESPONSE_CNF ( HVY_API_SIM_OUT + 19 )
#define API_SIM_FETCH_IND            ( HVY_API_SIM_OUT + 20 )
#define API_SIM_SELECTAPPLICATION_CNF   ( HVY_API_SIM_OUT + 21 )
#define API_SIM_SETUSIMPBK_CNF   ( HVY_API_SIM_OUT + 22 )
#define API_SIM_CLOSE_CNF			 ( HVY_API_SIM_OUT + 23 )  /// API_SIM_CLOSE_CNF
#define API_SIM_SEEK_ALL_CNF             ( HVY_API_SIM_OUT + 24  )

// SMS-PP: MMI -> API
#define API_SMSPP_SEND_REQ           ( HVY_API_SMSPP_IN  + 0 )
#define API_SMSPP_SENDMMA_REQ        ( HVY_API_SMSPP_IN  + 1 )
#define API_SMSPP_ACK_REQ            ( HVY_API_SMSPP_IN  + 2 )
#define API_SMSPP_ERROR_REQ          ( HVY_API_SMSPP_IN  + 3 )
#define API_SMSPP_ABORTMMA_REQ       ( HVY_API_SMSPP_IN  + 4 )
#define API_SMSPP_ABORT_REQ       ( HVY_API_SMSPP_IN  + 5 )

// SMS-PP: API -> MMI
#define API_SMSPP_ACK_IND            ( HVY_API_SMSPP_OUT + 0 )
#define API_SMSPP_ERROR_IND          ( HVY_API_SMSPP_OUT + 1 )
#define API_SMSPP_RECEIVE_IND        ( HVY_API_SMSPP_OUT + 2 )

// SMS-CB: MMI -> API
#define API_SMSCB_ACTIVATE_REQ       ( HVY_API_SMSCB_IN  + 0 )
#define API_SMSCB_DEACTIVATE_REQ     ( HVY_API_SMSCB_IN  + 1 )

// SMS-CB: API -> MMI
#define API_SMSCB_RECEIVE_IND        ( HVY_API_SMSCB_OUT + 0 )
#define API_SMSCB_PAGE_IND           ( HVY_API_SMSCB_OUT + 1 )

// Fax, Data : MMI -> API
#define API_CSD_DATA_REQ             ( HVY_API_CSD_IN + 0 )
#define API_CSD_CRLPSET_REQ          ( HVY_API_CSD_IN + 1 )
#define API_CSD_CRLPRD_REQ           ( HVY_API_CSD_IN + 2 )

// Fax, Data : API -> MMI
#define API_CSD_DATA_IND             ( HVY_API_CSD_OUT + 0 )
#define API_CSD_CRLPRD_IND           ( HVY_API_CSD_OUT + 1 )

#define API_SYS_CTR_IND               ( HVY_API_CTR_OUT + 0 )
#define API_START_STACK_ERR           (HVY_API_GENERAL_OUT + 10)
#ifdef LTE_NBIOT_SUPPORT
//API -> RABM
#define  API_RABM_DATA_REQ         ( HVY_API_RABM_OUT + 0 )

//RABM ->API
//#define  RABM_API_FLOWCTRL_IND     ( HVY_API_RABM_IN + 1 )



#endif //#ifdef LTE_NBIOT_SUPPORT

/****************************************************************
** PROTOCOL STACK CONFIGURATION STRUCTURE
****************************************************************/
/*
** Possible values for the 
** Non Drx Timer requested duration
*/
typedef enum
{
  API_NO_NDRX    ,
  API_NDRXTIM_1S ,
  API_NDRXTIM_2S ,
  API_NDRXTIM_4S ,
  API_NDRXTIM_8S ,
  API_NDRXTIM_16S,
  API_NDRXTIM_32S,
  API_NDRXTIM_64S 
} api_NonDrxTimer_t ;

/*
** Possible values for Ms Power Class
** (3GPP TS 05.05)
*/
typedef enum 
{
  API_CLASS_NOT_SUPPORTED,
  API_CLASS_1,
  API_CLASS_2,
  API_CLASS_3,
  API_CLASS_4,
  API_CLASS_5,
  API_NO_CLASS = 0xff,
} api_PwrClass_t ;


#ifdef __EGPRS__
/*
** Possible values for Ms PSK Power capability
** (3GPP TS 45.005)
*/
typedef enum 
{
  API_RESERVED = 0x00,
  API_CLASS_E1 = 0x01,
  API_CLASS_E2 = 0x10,
  API_CLASS_E3 = 0x11,
} api_PskPwrCap_t;
#endif
/*
** Possible values for the SS Screening indicator
** (3GPP TS 24.080)
*/
typedef enum
{
  API_SS_PHASE1,
  API_SS_PHASE2
} api_SsScreenInd_t ;

/*
** Possible values for Ms Multislot Class
** (3GPP TS 05.02)
*/
typedef enum
{
  API_MSCLASS_1  = 1 ,
  API_MSCLASS_2  = 2 ,
  API_MSCLASS_3  = 3 ,
  API_MSCLASS_4  = 4 ,
  API_MSCLASS_5  = 5 ,
  API_MSCLASS_6  = 6 ,
  API_MSCLASS_7  = 7 ,
  API_MSCLASS_8  = 8 ,
  API_MSCLASS_9  = 9 ,
  API_MSCLASS_10 = 10,
  API_MSCLASS_11 = 11,
  API_MSCLASS_12 = 12,
} api_SlotClass_t ;

#ifdef __EGPRS__
/*
** Possible values for Ms EGPRS Multislot Class
** (3GPP TS 05.02)
*/
typedef enum
{
  API_MSECLASS_1  = 1 ,
  API_MSECLASS_2  = 2 ,
  API_MSECLASS_3  = 3 ,
  API_MSECLASS_4  = 4 ,
  API_MSECLASS_5  = 5 ,
  API_MSECLASS_6  = 6 ,
  API_MSECLASS_7  = 7 ,
  API_MSECLASS_8  = 8 ,
  API_MSECLASS_9  = 9 ,
  API_MSECLASS_10 = 10,
  API_MSECLASS_11 = 11,
  API_MSECLASS_12 = 12,
  API_MSECLASS_13  =15 ,
  API_MSECLASS_14  =14 ,
  API_MSECLASS_15 = 15 ,
  API_MSECLASS_16  =16 ,
  API_MSECLASS_17  =17 ,
  API_MSECLASS_18  =18 ,
  API_MSECLASS_19  =19 ,
  API_MSECLASS_20  =20 ,
  API_MSECLASS_21  =21 ,
  API_MSECLASS_22 = 22,
  API_MSECLASS_23 = 23,
  API_MSECLASS_24 = 24,
  API_MSECLASS_25  =25 ,
  API_MSECLASS_26  =26 ,
  API_MSECLASS_27  =27 ,
  API_MSECLASS_28  =28 ,
  API_MSECLASS_29  =29 
} api_EgprsSlotClass_t ;

#endif

/*
** possible values for SpyFieldBitMap 
** = DbgInfo type identifiers
*/
enum
{
  API_L1_SERV_SPY,
  API_L1_NEAR_SPY,
  API_RR_SPY,
  API_RLU_SPY,
  API_MM_SPY,
  API_CC_SPY,
  API_CB_SPY,
  API_LLC_SPY,
  API_NB_SPY
};

/*
** Available spy fields for RR
*/
enum { API_RRSPY_CFSM,  API_RRSPY_PFSM,  API_RRSPY_PROC,  API_RRSPY_GFSM,
       API_RRSPY_SIM,   API_RRSPY_NEARS, API_RRSPY_MON,   API_RRSPY_SAP0, 
       API_RRSPY_SAP3,  API_RRSPY_CIPON, API_RRSPY_HOPON, API_RRSPY_DTXON,
       API_RRSPY_BANDS, API_RRSPY_LAST };


/*
** Available spy fields for RLU
*/
enum { API_RLUSPY_FSM,     API_RLUSPY_MODE,    API_RLUSPY_PRIO,    
       API_RLUSPY_CLASS,   API_RLUSPY_NBSDU,   API_RLUSPY_NBBYTES, 
       API_RLUSPY_TBC,     API_RLUSPY_CV,      API_RLUSPY_CS,    
       API_RLUSPY_STLD,    
       API_RLUSPY_LAST } ;

/*
** Available spy fields for MM
*/
enum { API_MMSPY_T3212DUR,
#ifdef __GPRS__
       API_MMSPY_ATT,        API_MMSPY_PROC,    API_MMSPY_SIM,      
       API_MMSPY_GPRS_SERV,  API_MMSPY_NMO,     API_MMSPY_PTMSI3,   
       API_MMSPY_PTMSI2,     API_MMSPY_PTMSI1,  API_MMSPY_PTMSI0,   
       API_MMSPY_ATT_CNT,    API_MMSPY_RAU_CNT, API_MMSPY_TIMEX_CNT,
       API_MMSPY_RDY_STA,    API_MMSPY_T3312,   API_MMSPY_PDPCTX,   
#endif
       API_MMSPY_LAST } ;

/*
** Available spy fields for LLC
*/
enum { API_LLCSPY_ASS,        API_LLCSPY_CIPH,    API_LLCSPY_SUSP,
       API_LLCSPY_LAST } ;

/*
** Available spy fields for CB
*/
enum { API_CBSPY_ACT,         API_CBSPY_ITF,      API_CBSPY_SUPP, 
       API_CBSPY_HIST,        API_CBSPY_CTXNB,
       API_CBSPY_LAST } ;

typedef struct 
{
 /*
 ** Plmn Selection Mode
 */
#define API_PSEL_AUTO 0 
#define API_PSEL_MANU 1 
 u8                PlmnSelMode     ; 

 /*
 ** UCS2 alphabet support
 */
 bool              Ucs2Support     ; 

 /*
 ** PFC support
 */
 bool              PfcSupport      ; // (3GPP TS 24.008)

 /*
 ** GSM ciphering algorithms
 */
#define API_A52 ( 1 << 0 )
#define API_A53 ( 1 << 1 )
#define API_A54 ( 1 << 2 )
#define API_A55 ( 1 << 3 )
#define API_A56 ( 1 << 4 )
#define API_A57 ( 1 << 5 )
 u8                A5Support       ; // Bit field. Bit = 1 means "supported"

 /*
 ** GPRS ciphering algorithms
 */
#define API_GEA1 ( 1 << 7 )
#define API_GEA2 ( 1 << 6 )
#define API_GEA3 ( 1 << 5 )
#define API_GEA4 ( 1 << 4 )
#define API_GEA5 ( 1 << 3 )
#define API_GEA6 ( 1 << 2 )
#define API_GEA7 ( 1 << 1 )
 u8                GeaSupport      ; // Bit field. Bit = 1 means "supported"

 /*
 ** MS Multislot Class
 */
 api_SlotClass_t   MSClass         ; 

 /*
 ** MS EGPRS Multislot Class
 */
 #ifdef __EGPRS__
 api_EgprsSlotClass_t   MSEGPRSClass   ; 
#endif
 /*
 ** MS power classes (GMSK)  
 */
 api_PwrClass_t    G400PwrClass  ; // Valid only if GSM4xx support 
 api_PwrClass_t    G850PwrClass  ; // Valid only if GSM850 support 
 api_PwrClass_t    G900PwrClass  ; // Valid only if GSM900x support 
 api_PwrClass_t    D1800PwrClass ; // Valid only if DCS 1800 support
 api_PwrClass_t    P1900PwrClass ; // Valid only if PCS 1900 support

 
 /*
 ** MS 8PSK power capability  
 */
 #ifdef __EGPRS__
 api_PskPwrCap_t    api_PskPwrCap  ; 
#endif

 /*
 ** Non Drx parameters
 */
 api_NonDrxTimer_t NonDrxTimer        ; // Requested duration of transfer 
                                        // non-DRX mode period
 u8                SplitPgCycle       ; // must be < 99 (cf 24.008 10.5.5.6 )

 /*
 ** Supported Channel Modes
 */ 
#define API_SIG_ONLY    ( 1 << 0 )
#define API_SPEECH_V1   ( 1 << 1 )
#define API_SPEECH_V2   ( 1 << 2 )
#define API_SPEECH_V3   ( 1 << 3 )
#ifdef __CS_DATA__
#define API_43_14_KBS   ( 1 << 4 ) //data, 43.5kbit/s dwnlnk, 14.5kbit/s uplnk
#define API_29_14_KBS   ( 1 << 5 ) //data, 29.0kbit/s dwnlnk, 14.5kbit/s uplnk
#define API_43_29_KBS   ( 1 << 6 ) //data, 43.5kbit/s dwnlnk, 29.0kbit/s uplnk
#define API_14_43_KBS   ( 1 << 7 ) //data, 14.5kbit/s dwnlnk, 43.5kbit/s uplnk
#define API_14_29_KBS   ( 1 << 8 ) //data, 14.5kbit/s dwnlnk, 29.0kbit/s uplnk
#define API_29_43_KBS   ( 1 << 9 ) //data, 29.0kbit/s dwnlnk, 43.5kbit/s uplnk
#define API_43_KBS      ( 1 << 10 )//data, 43.5kbit/s
#define API_32_KBS      ( 1 << 11 )//data, 32.0kbit/s
#define API_29_KBS      ( 1 << 12 )//data, 29.0kbit/s
#define API_14_KBS      ( 1 << 13 )//data, 14.5kbit/s
#define API_12_KBS      ( 1 << 14 )//data, 12.0kbit/s
#define API_6_KBS       ( 1 << 15 )//data, 6.0kbit/s
#define API_3_KBS       ( 1 << 16 )//data, 3.6kbit/s
#endif

// added by leiming 2006.08.17 begin 
#define API_INVALID_CHAN_MODE    0xFFFFFFFF 
//added by leiming 2006.08.17 end

 u32               ChanModeSupport ;

 /*
 ** SS screening indicator
 */
 api_SsScreenInd_t SsScreenInd   ; 

 /*
 ** CC timers 
 */
 u8                Tmpty         ; // (Timer MPTY) nb of seconds
 u8                Tect          ; // (Timer ECT ) nb of seconds

 /*
 ** CRSS and SS supported service
 */
#define API_SS_AOC      ( 1 << 0 )
#define API_SS_CALLHOLD ( 1 << 1 )
#define API_SS_MPTY     ( 1 << 2 )
 u8                SsSupported   ; // Bitfield of the supported SS

 /* 
 ** GPRS TEST MODE A : Nb of bytes in LLC PDUs
 */
 u16               TstLlcPduSize ; // [140 .. 1560 ]
 
 /* 
 ** GPRS automatic Attach at power on/switch on
 */
 bool              AutoGprsAttach;

 /*
 ** API Debug mode interface.
 */
 u32               SpyFieldBitMap [API_NB_SPY];     
 
#ifdef __EGPRS__
 bool				EgprsUl8PSK;	
#endif


} api_StackCfg_t ;


#if !(STK_MAX_SIM==1)
extern api_StackCfg_t sap_StackCfg[STK_MAX_SIM];
#else
extern api_StackCfg_t sap_StackCfg;
#endif


typedef struct 
{
 /* 
 ** GPRS Access Method (1: One phase access, 2: Two phase access)
 */
 u8 GprsAccessMeth;

#define API_GACCMETH_ONE_PHASE  1
#define API_GACCMETH_TWO_PHASES 2
 
 /*
 ** CC timers 
 */
 u8                TS10          ; // (Timer S10, Automatic disconnect delay)
                                   // tenths of a second
} api_StackDynCfg_t ;

#if !(STK_MAX_SIM==1)
extern api_StackDynCfg_t sap_StackDynCfg[STK_MAX_SIM];
#else
extern api_StackDynCfg_t sap_StackDynCfg;
#endif
/****************************************************************
** General note : wherever the TI is not significant,
**                it has been replaced by a "Reserved" u8 field
****************************************************************/

/********************************************
** API-MMI GENERAL and MISCELLANEOUS SERVICES
*********************************************/

/*
================================================================================
  Structure  : api_StartReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Starts the protocol stack
  Parameters : Bitmap of the frequency bands to activate (900|1800, or 900|1900
               etc), 
  Comment    : 
================================================================================
*/

typedef struct 
{
	u8 Band ;       // see definition below 
	u8 Plmn[3];   // for   __LOCATION_LOCK__
	u8 bLockArf;
#ifdef LTE_SUPPORT
	u8  Rat;
	u8 LteType;
	u8  bOnlyAtt;  // for NBIOT 
	u32 EUBand_FDD ;
	u32 EUBand_TDD ;
#endif
} api_StartReq_t ; 



#ifdef LTE_SUPPORT

//for command rat
#define API_RAT_AUTO               0
#define API_RAT_IRAT          (1<<0) 
#define API_RAT_GSM           (1<<1)
#define API_RAT_LTE           (1<<2)

// for camped rat
#define API_RAT_CAMP_NO            0
#define API_RAT_CAMP_IRAT          (1<<0) 
#define API_RAT_CAMP_GSM           API_RAT_GSM
#define API_RAT_CAMP_LTE           API_RAT_LTE

// for LteType
#define API_LTE_CATM              (1<<0) 
#define API_LTE_NBIOT             (1<<1) 
#define API_LTE_CAT1BIS           (1<<2) 

#endif

typedef struct
{
   u16         Arfcn ;   // [0..1024]
   u16         Rssi  ;   // [0..120] dBm after L1_LOW_RLA  it's noise only
} ArfPower_t ;

typedef struct 
{
	u8 Band ;       // see definition below
	u16         Nb                ;  // Number of Elem in Table
	ArfPower_t   List[672] ;  // List of (Arfcn, Measure)
} api_PowerScanReq_t ; 

typedef struct 
{
	u16         Nb                ;  // Number of Elem in Table
	ArfPower_t   List[672] ;  // List of (Arfcn, Measure)
} api_PowerScanRsp_t ; 

/* 
API_GSM_450, API_GSM_480, API_GSM_900R bands are not supported by STACK now, 
MMI should not set Band to these value. These macros are reserved only for compatible.
*/
#define API_GSM_450  ( 1 << 0 )
#define API_GSM_480  ( 1 << 1 )
#define API_GSM_900P ( 1 << 2 )
#define API_GSM_900E ( 1 << 3 )
#define API_GSM_900R ( 1 << 4 )
#define API_GSM_850  ( 1 << 5 )
#define API_DCS_1800 ( 1 << 6 )
#define API_PCS_1900 ( 1 << 7 )



/*
================================================================================
  Structure  : api_StopReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Stops the protocol stack
  Comment    : 
================================================================================
*/
typedef struct
{
#ifdef LTE_NBIOT_SUPPORT
 u8   detachType; //extend powerOff value from bool to u8,   0 is normal detach,  1 is poweroff, 2 is detach with sim card removed(value 2 is used for nbiot act)
 #else
 bool   PowerOff;   // StopReq for power off if TRUE
 #endif
} api_StopReq_t ;

/*
================================================================================
  Structure  : api_ImeiReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Requests the MS's IMEI
  Comment    : This message has no specific parameter
================================================================================
*/
typedef u8 api_ImeiReq_t ;

/*
================================================================================
  Structure  : api_QualReportReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Requests a radio signal quality report
  Comment    : 
================================================================================
*/
typedef struct 
{
 bool  StartReport ;     // Send report on every Measuremnt if TRUE   
} api_QualReportReq_t ;

/*
================================================================================
  Structure  : api_ImeiInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Gives the MS's IMEI
================================================================================
*/
#define API_IMEI_SIZE 14
#define API_SV_SIZE   2 
typedef struct 
{
 u8          Imei[API_IMEI_SIZE] ;
 u8          SV  [API_SV_SIZE]   ;
} api_ImeiInd_t ;

/*
================================================================================
  Structure  : api_QualReportCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : radio signal quality report
================================================================================
*/
typedef struct 
{
 u8          Rssi     ;
 u8          RxQual   ;
} api_QualReportCnf_t ;

/*
================================================================================
  Structure  : api_NwkInfoInd_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Gives informations about the network
================================================================================
*/
typedef struct 
{
 u8     LocalTimeZoneLen;       // 0 or 1
 u8     LocalTimeZone;          // valid if LocalTimeZoneLen > 0
 
 u8     UniversalTimeZoneLen;   // 0 or 7
 u8     UniversalTimeZone[7];   // valid if UniversalTimeZoneLen > 0
 
 u8     DaylightSavingTimeLen;  // 0 or 1
 u8     DaylightSavingTime;     // valid if DaylightSavingTimeLen > 0
 
 u8     NwkFullNameLen;         // 0 to 247
 u8     NwkShortNameLen;        // 0 to 247
 u8     NwkNames[247];          // The first NwkFullNameLen bytes describe the
                                // Full Name of the Network, the following
                                // NwkShortNameLen bytes describe the Short Name
                                // of the Network.
                                // NwkFullNameLen + NwkShortNameLen <= 247
} api_NwkInfoInd_t ;



/********************************
** API-NETWORK SERVICE INTERFACE
*********************************/

/*
================================================================================
  Structure  : api_NwSelModeReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : selects the network selection mode
================================================================================
*/
typedef struct
{
 u8         Mode     ;
} api_NwSelModeReq_t ;

// Mode possible values
#define API_NW_AUTOMATIC_MODE    0 
#define API_NW_MANUAL_MODE       1

/*
================================================================================
  Structure  : api_NwListReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : requests a list of the surrounding PLMNs
  Comment    : This message has no specific parameter
================================================================================
*/
typedef u8 api_NwListReq_t ;

/*
================================================================================
  Structure  : api_NwListAbortReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : requests API to abort the construction of the list of 
               surrounding PLMNs
  Comment    : This message has no specific parameter
================================================================================
*/
typedef u8 api_NwListAbortReq_t ;

/*
================================================================================
  Structure  : api_NwSelReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI selects a PLMN for registration
================================================================================
*/
typedef struct 
{
 u8          PlmnId[3] ; 
} api_NwSelReq_t ;

/*
================================================================================
  Structure  : api_NwReselReq_t 
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : requests API to launch a new network selection procedure
  Comment    : This message has no specific parameter
================================================================================
*/
typedef u8 api_NwReselReq_t ;


/*
================================================================================
  Structure  : api_NwPsAttachReq_t 
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : requests API to attempt GPRS Attach when possible
  Comment    : This message has no specific parameter
================================================================================
*/
//[[ add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
// the API using should not using the value 0
#define API_PS_ATTACH_DEFAULT     0
#define API_PS_ATTACH_GPRS     1
#define API_PS_ATTACH_IMSI     2
#define API_PS_ATTACH_COMB     3
#define API_PS_ATTACH_TYPE_LAST     3
#define API_PS_ATTACH_TYPE_MASK  0x07
#define API_PS_ATTACH_FOR    (1<<3)

typedef u8 api_NwPsAttachReq_t ;



/*
================================================================================
  Structure  : api_NwPsDetachReq_t 
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : requests API to perform a GPRS DETACH
  Comment    : This message has no specific parameter
================================================================================
*/
//[[ add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
// the API using should not using the value 0
#define API_PS_DETACH_DEFAULT     0
#define API_PS_DETACH_GPRS     1
#define API_PS_DETACH_IMSI     2
#define API_PS_DETACH_COMB     3
// ]] add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
typedef u8 api_NwPsDetachReq_t ;


/*
================================================================================
  Structure  : api_NwCsRegStatusInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : indicates the current network registration status for circuit
               switched services
================================================================================
*/
typedef struct 
{
 u8         Status     ;
 u8         LAI    [5] ; // Location Area Identifier
 u8         CellId [2] ; 
 bool       Roaming    ;
} api_NwCsRegStatusInd_t ;

// Status possible values
#define API_NW_NO_SVCE      0    // No Service
#define API_NW_FULL_SVCE    1    // Full Service
#define API_NW_LTD_SVCE     2    // Limited Service (Emergency Only); CS only
#define API_NW_NO_OUTGO     3    // No outgoing called allowed ; CS only
#define API_NW_REG_PENDING  4    // Outcome of registn not known yet; PS only

#define API_NW_GPRS     1
#define API_NW_EDGE     2
#ifdef LTE_NBIOT_SUPPORT
#define API_NW_NBIOT    4
#endif //#ifdef LTE_NBIOT_SUPPORT
/*
================================================================================
  Structure  : api_NwPsRegStatusInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : indicates the current network registration status for packet
               switched services
================================================================================
*/
typedef struct 
{
 u8         Status    ; // same possible values as for api_NwCsRegStatusInd_t
 u8         Cause;     // Tell upper layer why stack can not support PS full service. It only works when Status is API_NW_NO_SVCE.
 u8         RAI    [6]; // Routing Area Identifier
 #ifdef LTE_NBIOT_SUPPORT
 u8         CellId [4]; 
 #else
 u8         CellId [2]; 
 #endif
 bool       Roaming   ;
 u8         NwType;
#ifdef LTE_NBIOT_SUPPORT
    u8 cause_type;
    u8 activeTime;
    u8 periodicTau;
    u8 nwSupportedCiot; //[0-3]
    u8 edrxEnable;  //0 off, 1 on
    u8 edrxPtw;     //[0-15]
    u8 edrxValue;    //[0-15]
#endif
} api_NwPsRegStatusInd_t ;

#ifdef LTE_NBIOT_SUPPORT
typedef struct
{
    u8 ConnMode;
}api_NwErrcConnStatusInd_t;
#endif //#ifdef LTE_NBIOT_SUPPORT
// Cause possible values
#define API_P_USER_DETACH                       9     //Detach following user request
#define API_P_ILLEGAL_USR				12	// illegal MS, illegal ME
											// GPRS services and non GPRS services not allowed
#define API_P_SRVC_NOT_ALLOWD			13	// GPRS Service not allowed
#define API_P_FBD_PLMN					14	// PLMN not allowed
#define API_P_NO_ROAM_LA				15	// Roaming not allowed in this Location Area
#define API_P_FBD_LA					16	// Location Area not allowed
#define API_P_NO_CELL_IN_LA			17	// No suitable Cells in Location Area
#define API_P_SRVC_NOT_ALLOWD_INPLMN	18	// GPRS Service not allowed in this PLMN
#define API_P_NWK_DET					19	// Detach from the Nwk (re-attach shall be requested by MMI)
#define API_P_CS_NOT_FULL_SVC			20	// CS is not full service
#define API_P_INVALID_CAUSE			0xFF	// When Status is API_NW_FULL_SVCE or API_NW_REG_PENDING, cause should 
                                                                       		// be set to API_P_INVALID_CAUSE

/*
================================================================================
  Structure  : api_NwListInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : gives the list of surrounding PLMNs
================================================================================
*/

/*
** Restriction definitions
*/
#define API_PLMN_OK         0
#define API_PLMN_FBD        1

/*
** Maximum number of available networks
*/
#define API_MAX_AVAIL_PLMN 20

typedef struct 
{
 u8              PlmnNb      ;
  u8   PlmnL [ API_MAX_AVAIL_PLMN * 3 ] ;
  u8   Restr [ API_MAX_AVAIL_PLMN  ]    ;
} api_NwListInd_t;


/*
================================================================================
  Structure  : api_NwListAbortInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API indicates it has cancelled the scan for surrounding PLMNs
  Comment    : 
================================================================================
*/
#define API_NORMAL_LIST_ABORT 0
#define API_ABORM_LIST_ABORT   1
#define API_PSREJ_LIST_ABORT 2
typedef struct
{
    u8 Cause;
}api_NwListAbortInd_t ;

#define API_MAX_STORE_COUNT   33

typedef struct//this must be the same with CFW_StoredPlmnList
{
    u8 Plmn[3];
    u8 Count;    
    u16 ArfcnL[API_MAX_STORE_COUNT];
}api_NwStoreListInd_t;

typedef struct
{
    bool bDetectMBS;
}api_DetectMBSReq_t;

/*******************************************************************************
** API-SS INTERFACE
*******************************************************************************/

//***************************************
//* Common SS Definitions
//***************************************

//*************************************************
//* Basic Service Code Definitions and Enumerations
//*************************************************
//The following definitions include possible values of the BasicServiceCode 
//field for either Teleservices or Bearer Services
//For Teleservcies the following definitions apply: 
//Use these in an action request if ss_ServiceCodeCategory == ss_TeleserviceCode
#define API_SS_ALL_TELESERVICES            ((u8)0)
#define API_SS_ALL_SPEECH_TELESERVICES     ((u8)0x10) //16d
#define API_SS_TELEPHONY                   ((u8)0x11)
#define API_SS_EMERGENCY_CALLS             ((u8)0x12)
#define API_SS_ALL_SMS                     ((u8)0x20)
#define API_SS_SM_MT_PP                    ((u8)0x21)
#define API_SS_SM_MO_PP                    ((u8)0x22)
#define API_SS_ALL_FAX_SERVICES            ((u8)0x60)
#define API_SS_FAX_GROUP3_ALTER_SPEECH     ((u8)0x61)
#define API_SS_AUTO_FAX_GROUP3             ((u8)0x62)
#define API_SS_FAX_GROUP4                  ((u8)0x63)
#define API_SS_ALL_DATA_TELESERVICES       ((u8)0x70)
#define API_SS_ALL_TELESERVICES_XCPT_SMS   ((u8)0x80)
#define API_SS_ALL_VOICE_GRP_CALL_SERVICES ((u8)0x90)
#define API_SS_VOICE_GROUP_CALL            ((u8)0x91)
#define API_SS_VOICE_BROADCAST_CALL        ((u8)0x92)

#define API_SS_allPLMN_specificTS          ((u8)0xD0)
#define API_SS_plmn_specificTS_1           ((u8)0xD1)
#define API_SS_plmn_specificTS_2           ((u8)0xD2)
#define API_SS_plmn_specificTS_3           ((u8)0xD3)
#define API_SS_plmn_specificTS_4           ((u8)0xD4)
#define API_SS_plmn_specificTS_5           ((u8)0xD5)
#define API_SS_plmn_specificTS_6           ((u8)0xD6)
#define API_SS_plmn_specificTS_7           ((u8)0xD7)
#define API_SS_plmn_specificTS_8           ((u8)0xD8)
#define API_SS_plmn_specificTS_9           ((u8)0xD9)
#define API_SS_plmn_specificTS_A           ((u8)0xDA)
#define API_SS_plmn_specificTS_B           ((u8)0xDB)
#define API_SS_plmn_specificTS_C           ((u8)0xDC)
#define API_SS_plmn_specificTS_D           ((u8)0xDD)
#define API_SS_plmn_specificTS_E           ((u8)0xDE)
#define API_SS_plmn_specificTS_F           ((u8)0xDF)

//For Bearer the following definitions apply: 
//Use these in an action request if 
//ss_ServiceCodeCategory == ss_BearerserviceCode 
#define API_SS_AllBearerServices  ((u8)0)
#define API_SS_AllDataCDAServices ((u8)0x10)
#define API_SS_DataCDA_300bps     ((u8)0x11)
#define API_SS_DataCDA_1200bps    ((u8)0x12)
#define API_SS_DataCDA_1200_75bps ((u8)0x13)
#define API_SS_DataCDA_2400bps    ((u8)0x14)
#define API_SS_DataCDA_4800bps    ((u8)0x15)
#define API_SS_DataCDA_9600bps    ((u8)0x16)
#define API_SS_general_dataCDA    ((u8)0x17)

#define API_SS_AllDataCDSServices ((u8)0x18)
#define API_SS_DataCDS_1200bps    ((u8)0x1A)
#define API_SS_DataCDS_2400bps    ((u8)0x1C)
#define API_SS_DataCDS_4800bps    ((u8)0x1D)
#define API_SS_DataCDS_9600bps    ((u8)0x1E)
#define API_SS_general_dataCDS    ((u8)0x1F)

#define API_SS_AllPadAccessCAServices ((u8)0x20)
#define API_SS_PadAccessCA_300bps     ((u8)0x21)
#define API_SS_PadAccessCA_1200bps    ((u8)0x22)
#define API_SS_PadAccessCA_1200_75bps ((u8)0x23)
#define API_SS_PadAccessCA_2400bps    ((u8)0x24)
#define API_SS_PadAccessCA_4800bps    ((u8)0x25)
#define API_SS_PadAccessCA_9600bps    ((u8)0x26)
#define API_SS_general_padAccessCA    ((u8)0x27)

#define API_SS_ALLDataPDSServices ((u8)0x28)
#define API_SS_DataPDS_2400bps    ((u8)0x2C)
#define API_SS_DataPDS_4800bps    ((u8)0x2D)
#define API_SS_DataPDS_9600bps    ((u8)0x2E)
#define API_SS_general_dataPDS    ((u8)0x2F)

#define API_SS_allAlternateSpeech_DataCDA ((u8)0x30)
#define API_SS_allAlternateSpeech_DataCDS ((u8)0x38)
#define API_SS_allSpeechFollowedByDataCDA ((u8)0x40)
#define API_SS_allSpeechFollowedByDataCDS ((u8)0x48)
#define API_SS_allDataCircuitAsynchronous ((u8)0x50)
#define API_SS_allAsynchronousServices    ((u8)0x60)
#define API_SS_allDataCircuitSynchronous  ((u8)0x58)
#define API_SS_allSynchronousServices     ((u8)0x68)

#define API_SS_allPLMN_specificBS  ((u8)0xD0)
#define API_SS_plmn_specificBS_1   ((u8)0xD1)
#define API_SS_plmn_specificBS_2   ((u8)0xD2)
#define API_SS_plmn_specificBS_3   ((u8)0xD3)
#define API_SS_plmn_specificBS_4   ((u8)0xD4)
#define API_SS_plmn_specificBS_5   ((u8)0xD5)
#define API_SS_plmn_specificBS_6   ((u8)0xD6)
#define API_SS_plmn_specificBS_7   ((u8)0xD7)
#define API_SS_plmn_specificBS_8   ((u8)0xD8)
#define API_SS_plmn_specificBS_9   ((u8)0xD9)
#define API_SS_plmn_specificBS_A   ((u8)0xDA)
#define API_SS_plmn_specificBS_B   ((u8)0xDB)
#define API_SS_plmn_specificBS_C   ((u8)0xDC)
#define API_SS_plmn_specificBS_D   ((u8)0xDD)
#define API_SS_plmn_specificBS_E   ((u8)0xDE)
#define API_SS_plmn_specificBS_F   ((u8)0xDF)

//*****************************************
//* Error Code Definitions and Enumerations
//*****************************************
//Network Errors are returned by the network in a return error or reject  
// component or a Cause IE
#define API_SS_RETURN_ERROR     ((u8)0xA3) //Nwk returned a ReturnError Compont
#define API_SS_REJECT_COMPONENT ((u8)0xA4) //Nwk returned a Reject Component
#define API_SS_CAUSE_IE         ((u8)0xA5) //Network returned a cause IE, 
                                           //See cause IE 24.008 table 10.5.123

//Network Error Code Field definitions
#define API_SS_UNKNOWN_SUBSCRIBER                      ((u8)1)
#define API_SS_ILLEGAL_SUBSCRIBER                      ((u8)9)
#define API_SS_BEARER_SERVICE_NOT_PROVISIONED          ((u8)10)
#define API_SS_TELE_SERVICE_NOT_PROVISIONED            ((u8)11)
#define API_SS_ILLEGAL_EQUIPMENT                       ((u8)12)
#define API_SS_CALL_BARRED                             ((u8)13)
#define API_SS_ILLEGAL_SS_OPERATION                    ((u8)16)
#define API_SS_ERROR_STATUS                            ((u8)17)
#define API_SS_SS_NOT_AVAILABLE                        ((u8)18)
#define API_SS_SUBSCRIPTION_VIOLATION                  ((u8)19)
#define API_SS_INCOMPATIBILITY                         ((u8)20)
#define API_SS_FACILITY_NOT_SUPPORTED                  ((u8)21)
#define API_SS_ABSENT_SUBSCRIBER                       ((u8)27)
#define API_SS_SHORT_TERMINAL_DENIAL                   ((u8)29)
#define API_SS_LONG_TERM_DENIAL                        ((u8)30)
#define API_SS_SYSTEM_FAILURE                          ((u8)34)
#define API_SS_DATA_MISSING                            ((u8)35)
#define API_SS_UNEXPECTED_DATA_VALUE                   ((u8)36)
#define API_SS_PW_REGISTRATION_FAILURE                 ((u8)37)
#define API_SS_NEGATIVE_PW_CHECK                       ((u8)38)
#define API_SS_NUMBER_OF_PW_ATTEMTS_VIOLATION          ((u8)43)
#define API_SS_POSITION_METHOD_FAILURE                 ((u8)54)
#define API_SS_UNKNOWN_ALPHABET                        ((u8)71)
#define API_SS_USSD_BUSY                               ((u8)72)
#define API_SS_NBR_SB_EXCEEDED                         ((u8)120)
#define API_SS_REJECTED_BY_USER                        ((u8)121)
#define API_SS_REJECTED_BY_NETWORK                     ((u8)122)
#define API_SS_DEFLECTION_TO_SERVED_SUBSCRIBER         ((u8)123)
#define API_SS_SPECIAL_SERVICE_CODE                    ((u8)124)
#define API_SS_INVALID_DEFLECTED_TO_NUMBER             ((u8)125) 
#define API_SS_MAX_NUMER_OF_MPTY_PARTICIPANTS_EXCEEDED ((u8)126)
#define API_SS_RESOURCES_NOT_AVAILABLE                 ((u8)127)

//Local Error Definitions
//Local errors - causes of error generated at the MS - 0 through 9 are reserved
#define API_SS_NOT_IMPLEMENTED        ((u8)10)  //the SS has not been 
                                                //implemented
#define API_SS_OP_INVALID             ((u8)11)  //SS operation not valid for the
                                                //SS
#define API_SS_SSCODE_INVALID         ((u8)12)  //SSCode field either invalid or
                                                //not supported
#define API_SS_TI_ALLOC_FAILURE       ((u8)13)  //A TI could not be allocated 
                                                //for the transaction, try again
#define API_SS_CTX_MEM_ALLOC_FAILURE  ((u8)13)  //Memory was not available to 
                                                //support the transaction, try 
                                                //again
#define API_SS_PARAM_INVALID          ((u8)15) //Invalid parameter
#define API_SS_NO_PSWD_ERR            ((u8)16) //Call barring activation, 
                                               //deactivation, and pwd 
                                               //registration require a pwd, no
                                               //password was supplied
#define API_SS_NO_NEW_PSWD_ERR        ((u8)17) //Pwd registration requires a new
                                               //pwd, no new pwd was supplied
#define API_SS_NO_NEW_PSWD_AGAIN_ERR  ((u8)18) //Pwd registration requires the 
                                               //new pwd to be repeated, no new
                                               //pwd again was supplied
#define API_SS_NEW_PSWD_MATCH_ERR     ((u8)19) //New pwd doesn't match new pwd 
                                               //again
#define API_SS_ESTAB_ERR              ((u8)20) //Connection could not be 
                                               //established
#define API_SS_TRANSACTION_TIMEOUT    ((u8)21) //Transaction timeout
#define API_SS_CONNECTION_LOST        ((u8)22) //The connection was lost

#define API_SS_UNKNOWN_COMP_RECEIVED  ((u8)23)  //An unknown compnt was received
                                                //from the nwk, the transaction
                                                //was terminated
#define API_SS_INVALID_USSD_MSG_TYPE  ((u8)24)  //An invalid msg type was 
                                                //received for the current state
                                                //of the transaction
#define API_SS_INVALID_USSD_COMPONENT ((u8)25)  //aN invalid compnt was received
                                                //for the current state of the 
                                                //transaction
#define API_SS_USSD_ARG_LEN_TOO_LONG ((u8)26) //The USSD V2 String exceeded 
                                              //SS_USSD_ARG_STRING_MAX_LEN bytes
#define API_SS_USSD_IA5_STRING_TOO_LONG ((u8)27)
#define API_SS_USSD_INVALID_TI          ((u8)28)//The MMI/API itf. sent a ussd
                                                //action req. with an invalid TI
#define API_SS_USSD_INVALID_PROB_CODE_TAG ((u8)29) //The MMI/API sent an invalid
                                                   //problem code tag for a 
                                                   //reject message request
#define API_SS_USSD_INVALID_ERR_CODE   ((u8)30)  //The MMI/API itf. sent an 
                                                 //invalid ussd error code for a
                                                 //return error msg request
#define API_SS_USSD_INVALID_STATE      ((u8)31)
#define API_SS_INVALID_USSD_OPERATION  ((u8)32)  //Invalid USSD Operation
#define API_SS_INVALID_USSD_CODE       ((u8)33)

//Enumeration of the the set of local errors
typedef enum
{
 ss_ServiceNotImplementedError    = API_SS_NOT_IMPLEMENTED,
 ss_InvalidOperationError         = API_SS_OP_INVALID,
 ss_InvalidServiceCodeError       = API_SS_SSCODE_INVALID,
 ss_TIAllocationError             = API_SS_TI_ALLOC_FAILURE,
 ss_CtxtAllocationError           = API_SS_CTX_MEM_ALLOC_FAILURE,
 ss_InvalidParameterError         = API_SS_PARAM_INVALID,
 ss_PasswordMissingError          = API_SS_NO_PSWD_ERR,
 ss_NewPasswordMissingError       = API_SS_NO_NEW_PSWD_ERR,
 ss_NewPasswordAgainMissingError  = API_SS_NO_NEW_PSWD_AGAIN_ERR,
 ss_InvalidNewPasswordError       = API_SS_NEW_PSWD_MATCH_ERR,
 ss_MMEstablishmentError          = API_SS_ESTAB_ERR,
 ss_TransactionTimeoutError       = API_SS_TRANSACTION_TIMEOUT,
 ss_MMConnectionReleasedError     = API_SS_CONNECTION_LOST,
 ss_UnknownComponentReceivedError = API_SS_UNKNOWN_COMP_RECEIVED,
 ss_InvalidUSSDMessageTypeError   = API_SS_INVALID_USSD_MSG_TYPE,
 ss_InvalidUSSDComponentError     = API_SS_INVALID_USSD_COMPONENT,
 ss_USSDArgumentLengthTooLong     = API_SS_USSD_ARG_LEN_TOO_LONG,
 ss_USSDIA5StringTooLong          = API_SS_USSD_IA5_STRING_TOO_LONG,
 ss_InvalidUSSDProblemCodeTag     = API_SS_USSD_INVALID_PROB_CODE_TAG,
 ss_InvalidUSSDErrorCode          = API_SS_USSD_INVALID_ERR_CODE,
 ss_InvalidUSSDState              = API_SS_USSD_INVALID_STATE,
 ss_InvalidUSSDOperation          = API_SS_INVALID_USSD_OPERATION,
 ss_InvalidUSSDServiceCode        = API_SS_INVALID_USSD_CODE,
 ss_USSDBusy                      = API_SS_USSD_BUSY
} api_SsLocalErrorCodes_t;

//Problem Code Tags for Reject components
#define API_SS_GENERAL_PROBLEM_TAG        ((u8)0x80)
#define API_SS_INVOKE_PROBLEM_TAG         ((u8)0x81)
#define API_SS_RETURN_RESULT_PROBLEM_TAG  ((u8)0x82)
#define API_SS_RETURN_ERROR_PROBLEM_TAG   ((u8)0x83)

//Enumeration of the set of problem code tags
typedef enum
{
 ss_GENERAL_PROBLEM_TAG       = API_SS_GENERAL_PROBLEM_TAG,
 ss_INVOKE_PROBLEM_TAG        = API_SS_INVOKE_PROBLEM_TAG,
 ss_RETURN_RESULT_PROBLEM_TAG = API_SS_RETURN_RESULT_PROBLEM_TAG,
 ss_RETURN_ERROR_PROBLEM_TAG  = API_SS_RETURN_ERROR_PROBLEM_TAG
} api_SsProblemCodeTag_t;

//Problem Code Field Definitions for reject components having a 
//problem code tag == ss_GENERAL_PROBLEM_TAG
#define API_SS_GP_UNRECOGNIZED_COMPONENT     ((u8)0x00)
#define API_SS_GP_MISTYPED_COMPONENT         ((u8)0x01)
#define API_SS_GP_BADLY_STRUCTURED_COMPONENT ((u8)0x02)

//Enumeration of the set of allowed General Problem Codes
typedef enum
{
 ss__GP_BADLY_STRUCTURED_COMPONENT = API_SS_GP_UNRECOGNIZED_COMPONENT,
 ss_GP_MISTYPED_COMPONENT          = API_SS_GP_MISTYPED_COMPONENT,
 ss_GP_BADLY_STRUCTURED_COMPONENT  = API_SS_GP_BADLY_STRUCTURED_COMPONENT
} api_SsGeneralProblemCodes_t;

//Problem Code Field Definitions for reject components having a 
//problem code tag == ss_INVOKE_PROBLEM_TAG
#define API_SS_IP_DUPLICATE_INVOKE_ID         ((u8)0x00)
#define API_SS_IP_UNRECOGNIZED_OPERATION      ((u8)0x01)
#define API_SS_IP_MISTYPED_PARAMETER          ((u8)0x02)
#define API_SS_IP_RESOURCE_LIMITATION         ((u8)0x03)
#define API_SS_IP_INITIATING_RELEASE          ((u8)0x04)
#define API_SS_IP_UNRECOGNIZED_LINKED_ID      ((u8)0x05)
#define API_SS_IP_LINKED_RESPONSE_UNEXPECTED  ((u8)0x06)
#define API_SS_IP_UNEXPECTED_LINKED_OPERATION ((u8)0x07)

//Enumeration of the set of allowed Invoke Problem Codes
typedef enum
{
 ss_IP_DUPLICATE_INVOKE_ID         = API_SS_IP_DUPLICATE_INVOKE_ID,
 ss_IP_UNRECOGNIZED_OPERATION      = API_SS_IP_UNRECOGNIZED_OPERATION,
 ss_IP_MISTYPED_PARAMETER          = API_SS_IP_MISTYPED_PARAMETER,
 ss__IP_RESOURCE_LIMITATION        = API_SS_IP_RESOURCE_LIMITATION,
 ss_IP_INITIATING_RELEASE          = API_SS_IP_INITIATING_RELEASE,
 ss_IP_UNRECOGNIZED_LINKED_ID      = API_SS_IP_UNRECOGNIZED_LINKED_ID,
 ss_IP_LINKED_RESPONSE_UNEXPECTED  = API_SS_IP_LINKED_RESPONSE_UNEXPECTED,
 ss_IP_UNEXPECTED_LINKED_OPERATION = API_SS_IP_UNEXPECTED_LINKED_OPERATION
} api_SsInvokeProblemCodes_t;

//Problem Code Field Definitions for reject components having a 
//problem code tag == ss_RETURN_RESULT_PROBLEM_TAG
#define API_SS_RR_UNRECOGNIZED_INVOKE_ID     ((u8)0x00)
#define API_SS_RR_RETURN_RESULT_UNEXPECTED   ((u8)0x01)
#define API_SS_RR_MISTYPED_PARAMETER         ((u8)0x02)

//Enumeration of the set of allowed Return Result Problem Codes
typedef enum
{
 ss_RR_UNRECOGNIZED_INVOKE_ID   = API_SS_RR_UNRECOGNIZED_INVOKE_ID,
 ss_RR_RETURN_RESULT_UNEXPECTED = API_SS_RR_RETURN_RESULT_UNEXPECTED,
 ss_RR_MISTYPED_PARAMETER       = API_SS_RR_MISTYPED_PARAMETER
} api_SsReturnResultProblemCodes_t;

//Problem Code Field Definitions for reject components having a 
//problem code tag == ss_RETURN_ERROR_PROBLEM_TAG
#define API_SS_RE_UNRECOGNIZED_INVOKE_ID    ((u8)0x00)
#define API_SS_RE_RETURN_ERROR_UNEXPECTED   ((u8)0x01)
#define API_SS_RE_UNRECOGNIZED_ERROR        ((u8)0x02)
#define API_SS_RE_UNEXPECTED_ERROR          ((u8)0x03)
#define API_SS_RE_MISTYPED_PARAMETER        ((u8)0x04)

//Enumeration of the set of allowed Return Error Problem Codes
typedef enum
{
 ss_RE_UNRECOGNIZED_INVOKE_ID  = API_SS_RE_UNRECOGNIZED_INVOKE_ID,
 ss_RE_RETURN_ERROR_UNEXPECTED = API_SS_RE_RETURN_ERROR_UNEXPECTED,
 ss_RE_UNRECOGNIZED_ERROR      = API_SS_RE_UNRECOGNIZED_ERROR,
 ss_RE_UNEXPECTED_ERROR        =  API_SS_RE_UNEXPECTED_ERROR,
 ss_RE_MISTYPED_PARAMETER      = API_SS_RE_MISTYPED_PARAMETER
} api_SsReturnErrorProblemCodes_t;

//A union of all the possible problem codes that may returned in a
//reject component
typedef union
{
 //use this when the problem code tag == General Problem Tag
 api_SsGeneralProblemCodes_t      ss_GeneralProblemCode; 

 //use this when the problem code tag == Invoke Problem Tag
 api_SsInvokeProblemCodes_t       ss_InvokeProblemCode;  

 //Use this when the problem code tag == Return Result Problem Tag
 api_SsReturnResultProblemCodes_t ss_ReturnResultProblemCodes; 

 //Use this when the problem code tag == Return Error Problem Tag
 api_SsReturnErrorProblemCodes_t  ss_ReturnErrorProblemCodes; 
} api_SsProblemCodes_t;

//A structure used for Return Reject components.
//The Problem Code Tag indicates which field of the union is relevant
typedef struct
{
 api_SsProblemCodeTag_t ss_ProblemCodeTag; 
 api_SsProblemCodes_t   ss_ProblemCode;
} api_SsRejectComponentTag_t;

//Set of allowable errors for ss_ProcessUnstructuredSSData operations
typedef enum
{
 ss_USS_DATA_SYSTEM_FAILURE_ERR  = API_SS_SYSTEM_FAILURE,
 ss_USS_DATA_UNEXPECTED_DATA_ERR = API_SS_UNEXPECTED_DATA_VALUE
} api_SsProcessUSSDataErrors_t;

//Set of allowable errors for ss_ProcessUnstructuredSSRequest operations
typedef enum 
{
 ss_PROCESS_USS_REQUEST_SYSTEM_FAILURE_ERR       = API_SS_SYSTEM_FAILURE,
 ss_PROCESS_USS_REQUEST_DATA_MISSING_ERR         = API_SS_DATA_MISSING,
 ss_PROCESS_USS_REQUEST_UNEXPECTED_DATA_VALUE_ERR= API_SS_UNEXPECTED_DATA_VALUE,
 ss_PROCESS_USS_REQUEST_UNKNOWN_ALPHABET_ERR     = API_SS_UNKNOWN_ALPHABET,
 ss_PROCESS_USS_REQUEST_CALL_BARRED              = API_SS_CALL_BARRED
} api_SsProcessUSSRequestErrors_t;

//Set of allowable error for ss_UnstructuredSSRequest operations
typedef enum
{
 ss_USS_REQUEST_SYSTEM_FAILURE_ERR        = API_SS_SYSTEM_FAILURE,
 ss_USS_REQUEST_DATA_MISSING_ERR          = API_SS_DATA_MISSING,
 ss_USS_REQUEST_UNEXPECTED_DATA_VALUE_ERR = API_SS_UNEXPECTED_DATA_VALUE,
 ss_USS_REQUEST_ABSENT_SUBSCRIBER_ERR     = API_SS_ABSENT_SUBSCRIBER,
 ss_USS_REQUEST_ILLEGAL_SUBSCRIBER_ERR    = API_SS_ILLEGAL_SUBSCRIBER,
 ss_USS_REQUEST_ILLEGAL_EQUIPMENT         = API_SS_ILLEGAL_EQUIPMENT,
 ss_USS_REQUEST_UNKNOWN_ALPHABET_ERR      = API_SS_UNKNOWN_ALPHABET,
 ss_USS_REQUEST_USSD_BUSY                 = API_SS_USSD_BUSY
} api_SsUSSRequestErrors_t;

//Set of allowable error for ss_UnstructuredSSNotify operations
typedef enum
{
 ss_USS_NOTIFY_SYSTEM_FAILURE_ERR        = API_SS_SYSTEM_FAILURE,
 ss_USS_NOTIFY_DATA_MISSING_ERR          = API_SS_DATA_MISSING,
 ss_USS_NOTIFY_UNEXPECTED_DATA_VALUE_ERR = API_SS_UNEXPECTED_DATA_VALUE,
 ss_USS_NOTIFY_ABSENT_SUBSCRIBER_ERR     = API_SS_ABSENT_SUBSCRIBER,
 ss_USS_NOTIFY_ILLEGAL_SUBSCRIBER_ERR    = API_SS_ILLEGAL_SUBSCRIBER,
 ss_USS_NOTIFY_ILLEGAL_EQUIPMENT         = API_SS_ILLEGAL_EQUIPMENT,
 ss_USS_NOTIFY_UNKNOWN_ALPHABET_ERR      = API_SS_UNKNOWN_ALPHABET,
 ss_USS_NOTIFY_USSD_BUSY                 = API_SS_USSD_BUSY
} api_SsUSSNotifyErrors_t;

//*******************
//Supported SS-Codes
//*******************
//The codes enumerated in api_SsCode_t are supported by the SS layer.
//When invoking an SS operation, the user must provide a valid SS-Code from the
//api_SsCode_t as part of the api_SsActionReq. 
//If an invalid code is used, the SS software will send back an 
//api_SsErrorInd message with an appropriate cause.
typedef enum
{
 //Line ID Supplementary Service Codes - 3G TS 24.081
 ss_CLIP = (0x11), //(test 31.1.1)Calling Line ID Presentation, 17d 
 ss_CLIR = (0x12), //(test 31.1.2)Calling Line Identification Restriction, 18d 
 ss_COLP = (0x13), //(test 31.1.3)Connected Line Identification Presentation,19d
 ss_COLR = (0x14), //(test 31.1.4) Connected Line Identification Restriction,20d
 ss_CNAP = (0x19), //Calling Name Presentation

 //Call Forwarding Service(s)
 ss_CFA   = (0x20), //(test 31.2.1) All Forwarding SS, 32d
 ss_CFU   = (0x21), //Call Forwarding Unconditional, 33d
 ss_CD    = (0x24), //Call Deflection
 ss_CFC   = (0x28), //Call Forwarding Conditional, 40d
 ss_CFB   = (0x29), //Call Forwarding on Mobile Subscriber Busy, 41d
 ss_CFNRY = (0x2A), //Call Forwarding on Mobile Subscriber No Reply, 42d
 ss_CFNRC = (0x2B), //Call Forwarding on Mobile Subscriber Not Reachable, 43d

 // CallOffering
 ss_ECT      =   (0x31), // Explicit Call Transfer

 // Call Completion Services
 ss_CW       =   (0x41), // Call Waiting
 ss_HOLD     =   (0x42), // Call Hold

 // MultiParty Services
 ss_MPTY     =   (0x51), // Multiparty

 // Charging Services
 ss_AOCI     =   (0x71), // Advice of Charge Information
 ss_AOCC     =   (0x72), // Advice of Charge Charging

 // Additional Info Transfer
 ss_UUS1     =   (0x81), // UUS1 user-to user signalling
 ss_UUS2     =   (0x82), // UUS2 user-to user signalling
 ss_UUS3     =   (0x83), // UUS3 user-to user signalling

 // Barring Services
 ss_AllBarringSS           = (0x90), // All call barring SS
 ss_BarringOfOutgoingCalls = (0x91), // Barring of outgoing calls
 ss_BAOC                   = (0x92), // Barring of all outgoing calls
 ss_BOIC                   = (0x93), // Barring of international outgoing calls
 ss_BOIC_ExHC              = (0x94), // Barring of international outgoing calls
                                     // except those going to the home PLMN
 ss_BarringOfIncomingCalls = (0x99), // Barring of incoming calls
 ss_BAIC                   = (0x9A), // Barring of all incoming calls
 ss_BIC_ROAM               = (0x9B), // Barring of incoming calls when roaming
                                     // outside of home PLMN
 //USSD Services
 ss_USSD_V2 = (0x100),    //USSD version 2 services, supports all USSD 
                          //Operations except ss_ProcessUnstructuredSSData
 ss_USSD_V1 = (0x101),    //USSD version 1 services, supports 
                          //ss_ProcessUnstructuredSSData Operations Only
 //Invalid Code
 ss_InvalidSSCode
} api_SsCode_t;

#define API_SS_NO_CODE  ((api_SsCode_t)0)

//**********************************
//Supplementary Service  Operations
//**********************************
//The operations enumerated in api_SsOperation_t are supported by the 
//supplementary service layer.
//When invoking an SS operation, the user must provide a valid SS-operation from
//api_SsOperation_t as part of the api_SsActionReq.
//Not all operations are valid for every supplementary service.  
//See comments below and the relevant SS specification(s) for more specific info
//If an invalid operation is invoked by the api for a supplementary service, 
//the SS layer will respond with an api_SsErrorInd_t message.
typedef enum 
{
 ss_Register                     = 10, //Register a supplementary service,
                                       //Valid for Call Forwarding only
 ss_Erasure                      = 11, //Erase a supplementary service, 
                                       //Valid for Call Forwarding only
 ss_Activate                     = 12, //Activate a supplementary service, 
                                       //Valid for all SS Operations EXCEPT 
                                       //line identification services
 ss_Deactivate                   = 13, //Deactivate a supplementary service, 
                                       //Valid for all SS Operations EXCEPT 
                                       //line identification services
 ss_Interrogate                  = 14, //Interrogate a supplementary service, 
                                       //Valid for all SS operations
 ss_Notify                       = 16,
 ss_RegisterPassword             = 17, //Register a password, valid for Call 
                                       //Barring only
 ss_GetCallBarringPassword       = 18, //This operation is invoked by network 
                                       //only. If API attempts to use this
                                       //operation, SS will rspond with an error
 ss_ProcessUnstructuredSSData    = 19, //This operation is used only for USSD 
                                       //Version 1
 ss_ProcessUnstructuredSSRequest = 59, //This operation is used only for USSD 
                                       //Version 2
 ss_UnstructuredSSRequest        = 60, //This operation is used only for USSD 
                                       //Version 2
 ss_UnstructuredSSNotify         = 61, //This operation is used only for USSD 
                                       //Version 2
 callDeflection                  = 117,
 userUserService                 = 118,
 splitMPTY                       = 121,
 retrieveMPTY                    = 122,
 holdMPTY                        = 123,
 buildMPTY                       = 124,
 forwardChargeAdvice             = 125,
 explicitCT                      = 126,
 ss_InvalidSSOperation
} api_SsOperation_t;

//Enumerates the type of basic service code.  The basic service
//code is an optional parameter for action requests.  The 
//basic service code is also an optional paramenter returned 
//to the API as a result upon completion of an SS operation
typedef enum
{
 ss_TeleserviceCode           = 0x83, //131d
 ss_BearerServiceCode         = 0x82, //130d
 ss_ServicecodeNotApplicable  = 0 //To NOT include a BasicServiceCode in an SS 
                                  //transaction use this enumeration
                                  //note- in many cases this means that the
                                  //transaction applies to ALL BasicServiceCodes
} api_SsBasicServiceCodeCategory_t;

//L3 Message Types
#define API_SS_RELEASE_COMPLETE_MESSAGE_TYPE ((u8)0x2A) //upper 2 msbs are xx
#define API_SS_FACILITY_MESSAGE_TYPE         ((u8)0x3A) 
#define API_SS_REGISTER_MESSAGE_TYPE         ((u8)0x3B)

typedef enum
{
 ss_ReleaseCompleteMessageType = API_SS_RELEASE_COMPLETE_MESSAGE_TYPE,
 ss_FacilityMessageType        = API_SS_FACILITY_MESSAGE_TYPE,
 ss_RegisterMessageType        = API_SS_REGISTER_MESSAGE_TYPE,
 ss_InvalidSSMessageType
} api_SsMessageType_t;

/*
================================================================================
  Structure  : api_SsActionReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API -> SS
  Scope      : MMI-API invokes an SS operation for an SS Code with optional
               parameters to the network
================================================================================
*/
//**********************************
//* Supplementary Service  Action Request Params
//**********************************
//Data Structure containing optional parameters for use with call waiting 
//action requests
typedef struct
{
 //Include optional Basic Service Code
 api_SsBasicServiceCodeCategory_t ss_ServiceCodeCategory; //See enum definition
                                                          //for explanation
 u8 ss_ServiceCode;  //The service code, see #defines.
} api_SsCallWaitingParams_t;

//Data Structure containing optional parameters for use with call 
//forwarding action requests
#define MAX_FWD_TO_NUM_LEN      20
#define MAX_FWD_TO_SUBADDR_LEN  21
typedef struct
{
 //Include optional Basic Service Code
 api_SsBasicServiceCodeCategory_t ss_ServiceCodeCategory; //See enum definition
                                                          //for explanation
 u8 ss_ServiceCode;      //The service code, see #defines.

 //Include a forwarded to Number, these fields apply to call forwarding 
 //transactions only
 //Fields are valid if ss_ActionReqParamType == ss_CallForwarding and 
 //forwardedToNumberLen > 0
 u8 forwardedToNumber[MAX_FWD_TO_NUM_LEN];  //optional field for call forwarding
                                            //operations, set length field to 0 
                                            //in order to not include this 
                                            //field; else set length 
                                            //appropriately
 u8 forwardedToNumberLen;    //0 to 20 bytes, if 0 the field is ignored

 //Include a forwarded to subaddress, these fields apply to call 
 //forwarding transactions only
 //Fields are valid if ss_ActionReqParamType == ss_CallForwarding and 
 //forwardedToSubaddressLen > 0
 u8 forwardedtoSubaddress[MAX_FWD_TO_SUBADDR_LEN]; //optional field
 u8 forwardedToSubaddressLen;

 //Include a no reply condition time, these fields apply to call forwarding 
 //transactions only
 //Fields are valid if ss_ActionReqParamType == ss_CallForwarding and 
 //bNoReplyConditionTime == TRUE
 u8 bNoReplyConditionTimePresent; //TRUE or FALSE
 u8 noReplyConditionTime;         // only valid if bNoReplyConditionTimePresent 
                                  // == TRUE, 5 <= noReplyConditionTime <= 30
} api_SsCallForwardingParams_t;

//Data structure containing optional parameters for use with call barring 
//action requests
#define SS_NUM_PSWD_ELEMS   4
typedef struct
{
 //Include optional Basic Service Code
 api_SsBasicServiceCodeCategory_t ss_ServiceCodeCategory; //See enum definition
                                                          //for explanation
 u8 ss_ServiceCode;      //The service code, see #defines.

 //Include a Password, these fields apply to Call Barring operations only
 //Fields are valid if ss_ActionReqParamType == ss_CallBarring and 
 //bPasswordPresent == TRUE
 u8  bPasswordPresent;     //TRUE or False, must be TRUE for call barring 
                           //activation, deactivation and  password registration
 u8 ss_Password[SS_NUM_PSWD_ELEMS]; //password, mandatory for call barring 
                                    //activation,deactivation and pwd registratn

 //Register a new password, these fields apply to Call Barring Password 
 //Registration Operations only
 //Fields are valid if ss_ActionReqParamType == ss_CallBarring and 
 //bNewPasswordPresent == TRUE
 u8  bNewPasswordPresent; //TRUE or False, must be TRUE for call barring 
                          //password registration
 u8 ss_newPassword[SS_NUM_PSWD_ELEMS]; //call barring password, mandatory for 
                                       //call barring password registration
 //Fields are valid if ss_ActionReqParamType == ss_CallBarring and 
 //bNewPasswordAgainPresent == TRUE
 u8  bNewPasswordAgainPresent; //TRUE or False, must be TRUE for call barring 
                               //activation, deactivation and  pwd registration
 u8 ss_newPasswordAgain[SS_NUM_PSWD_ELEMS];  // call barring password, mandatory
                                             //for call barring activation,
                                             //deactivation, and pwd registratn
} api_SsCallBarringParams_t;

//******************************************************
//Data structures, unions, and definitions for 
//optional parameters for use with USSD action requests
//******************************************************
//Data structure defining parameters for use when invoking a 
//processUnstructuredSS-Data operation (USSD version 1 operation).
//The MMI should invoke a processUnstructuredSS-Data operation with these 
//parameters only after receiving an api_SsErrorInd 
//mailbox message having a problem code of "unrecognized operation" or a cause 
//of Facility Rejected" in response to the invocation of a 
//processUnstructuredSS-Request, unstructuredSS-Request, or an 
//unstructuredSS-NotifyUSSD operation (USSD version 2 operations).
//For a processUnstructuredSS-Data operations, the string must be encoded as an
//IA5 string.
#define SS_USSD_USER_DATA_IA5_STRING_MAX_LEN (200)
typedef struct
{
 u8 ss_IA5StringLength;
 u8 ss_IA5String[SS_USSD_USER_DATA_IA5_STRING_MAX_LEN];
} api_SsUSSDUserData_t;

//Data structure defining parameters for use when invoking an 
//processUnstructuredSS-Request or when returning results for an 
//unstructuredSS-Request operation.
#define SS_USSD_ARG_STRING_MAX_LEN (160)
typedef struct
{
 u8 ss_USSDDataCodingScheme;

 u8 ss_USSDStringLength; 
 u8 ss_USSDString[SS_USSD_ARG_STRING_MAX_LEN];
} api_SsUSSDArgs_t;

//A union of the possible USSD errrors.  Use the field appropriate to the 
//operation being used.
typedef union
{
 api_SsProcessUSSDataErrors_t    ss_ProcessUSSDataError;//Set of allowable errs
                                 //for ss_ProcessUnstructuredSSData operations
 api_SsProcessUSSRequestErrors_t ss_ProcessUSSRequestError; //Set of allowable
                                 //errors for ss_ProcessUnstructuredSSRequest 
                                 //operations.
 api_SsUSSRequestErrors_t        ss_USSRequestError; //Set of allowable error 
                                 //for ss_UnstructuredSSRequest operations
 api_SsUSSNotifyErrors_t         ss_USSNotifyError; //Set of allowable error for
                                 //ss_UnstructuredSSNotify operations
} api_SsUSSDErrorSel_t;

typedef enum
{
 ss_ProcessUSSDataErrorPresent,
 ss_ProcessUSSRequestErrorPresent,
 ss_USSRequestErrorPresent,
 ss_USSNotifyErrorPresent,
 ss_NoUSSDErrorPresent
} api_SsUSSDErrorPresent_t;

typedef struct
{
 api_SsUSSDErrorPresent_t ss_USSDErrorPresent;
 api_SsUSSDErrorSel_t ss_USSDErrorSel;
} api_SsUSSDErrors_t;

//A union of USSD parameters.
//The field to use depends upon the SS-Operation and the Component
typedef union
{
 //Use these params when the MS invokes or returns a result for a 
 //processUnstructuredSS-Data operation with ss_USSDInvoke or 
 //ss_USSDReturnResult components.
 api_SsUSSDUserData_t ss_USSDUserData;

 //Use these params when the MS invokes a processUnstructuredSS-Request, 
 //unstructuredSS-Request, or unstructuredSS-Notify operation with an 
 //ss_USSDInvoke component.
 api_SsUSSDArgs_t ss_USSDArgs;

 //Use these params for all Operations when the MS returns an error to the 
 //network with a ss_USSDReturnError component
 api_SsUSSDErrors_t  ss_USSDError;

 //Use these params for all operations when the MS returns a reject to the 
 //network with ss_USSD_ReturnReject component
 api_SsRejectComponentTag_t ss_ProblemCodeTagandCode;

} api_SsUSSDParamSet_t;

//Enumeration defining the types of USSD components available for use in USSD 
//api_SsActionReq mailbox messages.
//Note1- the component is a part of the operation currently in use as defined by
//the ss_operation field of the api_SsActionReq mailbox message.
//Note2 - the parameters used are defined by the component.
typedef enum
{
 //Used by the MMI/API to invoke a USSD operation when starting a MS initiated 
 //USSD transaction.
 //In response, MMI will receive a mailbox message of type
 //api_SsTIInd with a TI for the transaction if the transaction has been 
 //successfully started or of type api_SsErrorInd indicating the error.
 //This component uses parameters of type api_SsUSSDVersionParams_t for all USSD
 //operations
 ss_USSDInvoke, 

 //Used by the MS to return a normal (non-error) USSD result to the network.
 //This should only be used in response to the reception of a USSD message 
 //from the network.
 //This component uses parameters of type api_SsUSSDVersionParams_t for all USSD
 //operations EXCEPT ss_UnstructuredSSNotify which has no parameters.
 ss_USSDReturnResult, 

 //Used by the MS to return a USSD error to the network.
 //This should only be used in response to the reception of a USSD message from
 //the network.
 //Error Codes are dependant upon the operation.
 ss_USSDReturnError,

 //Used by the MS to return a USSD reject to the network.
 //This should only be used in response to the reception of a USSD message from 
 //the network.
 ss_USSDReturnReject,

 //Used by the MS to release a transaction (paired with Release Complete Msg)
 ss_USSDNoComponentPresent

} api_SsUSSDComponentType_t;

//Data structure containing:
//optional parameters for use with USSD Action Requests
//USSD results for api_SsActionConf mailbox messages
typedef struct
{
 //The type of message.
 //Valid Pairings for MS:
 //Register Msg - Invoke Component
 //Facility Msg - Return Result, Return Error, or Reject Component
 api_SsMessageType_t  ss_USSDMessageType;

 //The type of component for the api_SsActionReq mailbox message.
 //This depends both upon the operation and the current point in the sequence of
 //the operation.
 api_SsUSSDComponentType_t ss_USSDComponentType;

 //This field only applies to either:
 //MS responses to Network Intitiated USSD Requests OR
 //MS initiated USSD operations in which the nwk requests further information.
 //For MS responses to Network Intitiated USSD Requests, the TI is obtained in 
 //the api_SsActionConf message
 //for MS initiated USSD operations, the TI is obtained in a api_SsTIInd 
 //mailbox message
 u8 TI;

 //The USSD parameters, select from the union according to the operation and the
 //component type
 api_SsUSSDParamSet_t ss_USSDParamSel;
} api_SsUSSDParams_t;

//A union of the various action request param types used in api_SsParams_t.
//The only SS operations that have optional parameters are:
//Call Waiting, Call Forwarding, Call Barring, and USSD.
//For operations using these SS-Code make sure that you initialize the 
//appropriate field of the union!
typedef union
{
 //Use this data structure to execute a call waiting operation
 api_SsCallWaitingParams_t ss_CallWaitingParams;

 //Use this data structure to execute a call forwarding operation
 api_SsCallForwardingParams_t ss_CallForwardingParams;

 //Use this data structure to execute a call barring operation
 api_SsCallBarringParams_t ss_CallBarringParams;

 //Use this data structure to execute a USSD operation
 //Field is valid if ss_ActionReqParamType == ss_USSD
 api_SsUSSDParams_t ss_USSDParams;

} api_SsActionReqParams_t;


//This data structure is sent by the API module to the SS module to
//Initiate an SS transaction.  Not all combinations of ss_Operation, ss_Code,
//and ss_Params..  If a particular combination is invalide, the SS module
//will respond with an Error Indication
typedef struct
{
 api_SsOperation_t        ss_Operation; //The SS Operation
 api_SsCode_t             ss_Code; //The ss_code
 api_SsActionReqParams_t  ss_Params; //Optional SS transaction parameters
} api_SsActionReq_t;

/*
================================================================================
  Structure  : api_SsTIInd_t
--------------------------------------------------------------------------------
  Direction  : SS -> API -> MMI
  Scope      : Used to indicate the chosen TI for the supplementary service
               operation. Also, indicates that the transaction has started
               between the MS and the network
================================================================================
*/
typedef struct
{
 u8               TI;      //The transactionID being used for the SS transaction
 api_SsCode_t     ss_Code; //The SS Code for the SS transaction, this matches 
                           //the SS code supplied in the api_SsActionReq message
 api_SsOperation_t ss_Operation; //The SS operation for the SS transaction, this
                                 //matches the SS operation supplied in the 
                                 //api_SsActionReq message
} api_SsTIInd_t;

/*
================================================================================
  Structure  : api_SsActionCnf_t
--------------------------------------------------------------------------------
  Direction  : SS -> API -> MMI
  Scope      : Confirms that the requested action was successfully performed and
               provides API with any results returned from the network
================================================================================
*/
typedef enum
{
 Permanent                  = 0,
 TemporaryDefaultRestricted = 1,
 TemporaryDefaultAllowed    = 2
} api_SsCliRestrictionOption_t;

typedef enum
{
 ss_overrideEnabled  = 0,
 ss_overrideDisabled = 1
} api_SsOverrideCategory_t;

typedef struct
{
 api_SsBasicServiceCodeCategory_t ss_ServiceCodeCategory;
 u8                               BasicServiceCode;
} api_SsBasicServiceGroupListElement_t;

typedef struct
{
 u8 ccbsIndexPresent;                          //TRUE or FALSE
 u8 ccbsIndex;                                 //1 <= ccbsIndex <= 5
 u8 bSubcriberNumberPresent;                   //TRUE or FALSE
 u8 numberofSubscriberNumbers;                 //1 to 20
 u8 b_subscriberNumber[20]; 
 u8 bSubscriberSubaddressPresent;              //TRUE or FALSE
 u8 numberofSubscriberSubaddresses;            //1 to 21
 u8 b_subscriberSubaddress[21];
 u8 bBasicServiceGroupListElementPresent;      //TRUE or FALSE
 api_SsBasicServiceGroupListElement_t ss_BSGLE;
} api_SsCCBSFeatureListElement_t;

typedef struct
{
 u8 Status;                                     //not an optional field
 u8 bCliRestrictionOptionPresent;
 api_SsCliRestrictionOption_t cliRestrictionOption;
 u8 bMaximumEntitledPriorityPresent;
 u8 maximumEntitledPriority;
 u8 bDefaultPriorityPresent;
 u8 defaultPriority;
 u8 bCCBSFeatureListPresent;
 u8 numberOfCCBSFeatureListElements;                        //1 to 5
 api_SsCCBSFeatureListElement_t CCBSFeatureListElement[5];  //1 to 5
 u8 nbrSBPresent;
 u8 nbrSB;                                                  //2 <= nbrSB <= 7
 u8 nbrUserPresent; 
 u8 nbrUser;                                                //1 <= nbrUser <= 7
 u8 nbrSNPresent; 
 u8 nbrSN;                                                 //1 <= nbrUser <= 7
} api_SsGenericServiceInfo_t;

typedef struct
{
 u8 numberOfBasicServiceGroupElements;                    //1 to 13
 api_SsBasicServiceGroupListElement_t ss_BasicServiceGroupElement[13];
} api_SsBasicServiceGroupList_t;

//Results of a call forwarding interrogation operation
typedef struct
{
 u8     ss_bBasicServicePresent;                           //TRUE or FALSE
 api_SsBasicServiceCodeCategory_t ss_serviceCodeCatagory;
 u8     ss_basicServiceCode;

 u8     ss_bStatusPresent;                                 //TRUE or FALSE
 u8     ss_status;

 u8     ss_bForwardedToNumberPresent;                     //TRUE or FALSE
 u8     ss_numberOfForwardedToNumberElements;             //1 to 20
 u8     ss_forwardedToNumber[20];

 u8     ss_bForwardedToSubaddressPresent;                 //True or FALSE
 u8     ss_numberOfForwardedToSubaddressElements;         //1 to 21
 u8     ss_forwardedToSubaddress[21];

 u8     ss_bForwardingOptionsPresent;                     //TRUE or FALSE
 u8     ss_forwardingOptions;
 
 u8     ss_bNoReplyConditionTimePresent;                  //TRUE or FALSE
 u8     ss_noReplyConditionTime;
} api_SsForwardingFeatureListElement_t;

//Interrogation Results for call forwarding
typedef struct
{
 u8  ss_numberOfForwardingFeatureListElements; //0 to 13
 api_SsForwardingFeatureListElement_t ss_forwardingFeatureListElement[13];
} api_SsForwardingFeatureList_t;

//A union of the possible types of interrogation results
typedef union
{
 u8                              ss_Status;
 api_SsBasicServiceGroupList_t   ss_BSL;
 api_SsGenericServiceInfo_t      ss_GSI;
 api_SsForwardingFeatureList_t   ss_FFL; //Interrogation results for CF.
} api_SsIntRsltChoice_t;

//Enumerates the type of interrogation results
typedef enum
{
 ss_BasicServiceGroupListPresent  = 1,
 ss_GenericServiceInfoPresent     = 2,
 ss_ForwardingFeatureListPresent  = 3,
 ss_StatusInfoPresent             = 4,
 ss_NoIntRsltChoicesPresent       = 0
} api_SsIntRsltChoicePresence_t;

//If the Return Result is a result of an interrogation operation, this data 
//structure contains the results.  
//Different results are obtained for different operations
typedef struct
{
 //Indicates the type of interrogation results
 api_SsIntRsltChoicePresence_t ss_IntRsltInfoPresent; 

 //contains the interrogations results
 api_SsIntRsltChoice_t   ss_IntRsltChoice; 
} api_SsIntRslt_t;

//Contains the results returned from the network for call forwarding operations 
//except interrogate
typedef struct
{
 u8                            ss_bSSCodePresent;    //True or FALSE
 api_SsCode_t                  ss_code;
 u8                            ss_bForwardingFeatureListPresent; //True or FALSE
 api_SsForwardingFeatureList_t ss_ForwardingFeatureList;
} api_SsForwardingInfo_t;

//Call Barring Feature List
typedef struct
{
 u8  ss_bBasicServicePresent; //TRUE or FALSE
 api_SsBasicServiceCodeCategory_t  ss_serviceCodeCatagory;
 u8  ss_basicServiceCode;

 u8  ss_bStatusPresent;       //TRUE or FALSE
 u8  ss_status;
} api_SsCallBarringFeatureList_t;

//Contains the results returned from the network for call barring operations 
//except interrogate
typedef struct
{
 u8           ss_bSSCodePresent;     //True or FALSE
 api_SsCode_t ss_code;
 u8           ss_bPasswordPresent;   //TRUE or FALSE
 u8           ss_newPassword[SS_NUM_PSWD_ELEMS];  //New pwd, returned from nwk 
              //after successful call barring password registration
 u8           ss_bCallBarringFeatureListPresent;  //TRUE or FALSE
 u8           ss_numberOfCallBarringFeatureListElements; //1 to 13
 api_SsCallBarringFeatureList_t ss_callBarringFeatureList[13]; 
} api_SsCallBarringInfo_t;

//Contains the results returned from the network for ss codes other than call 
//barring and forwarding for all operations except interrogate
typedef struct
{
 u8  ss_bSSCodePresent;                                        //True or FALSE
 api_SsCode_t    ss_code;
 u8  ss_bSSStatusPresent;                                      //True or False
 u8  ss_Status;
 u8  ss_bCliRestrictionOptionPresent;                          //TRUE or FALSE
 api_SsCliRestrictionOption_t ss_cliRestrictionOption;
 u8  ss_OverrideCategoryPresentPresent;                        //TRUE or FALSE
 api_SsOverrideCategory_t  ss_OverrideCategory;
 u8  ss_bBasicServiceListPresent;                              // TRUE or FALSE
 u8  ss_numberOfBasicServiceListElements;                      //1 to 13
 api_SsBasicServiceGroupListElement_t basicServiceElement[13]; //only valid if 
                                            //bBasicServiceListPresent == TRUE
 u8  ss_bDefaultPriorityPresent;                               //TRUE or FALSE
 u8  ss_defaultPriority;                                       //0 to 15
 u8  ss_bNbrUserPresent;                                       //TRUE or FALSE
 u8  ss_nbrUser;                                               //1 to 7
} api_SsData_t;


//Enumerates the type of component received from the network for USSD operations
//The netork may either Invoke an operation and provide the MS with Args or
//The network may either Return Results for a MS invoked operation
typedef enum
{
 //An Invoke Component has been received from the Network for USSD Operation.
 ss_USSDNetworkInvoke,   

 //A Return Result Component has been received from the Network for a USSD 
 //Operation Invoked by the MS
 ss_USSDNetworkReturnResult,  

 //No Component was received with the L3 message (release complete only).
 ss_USSDNetworkNoComponentPresent 
} api_SsUSSDNetworkComponentType_t;

//Data structure containing results provided to the MS the Network due to the 
//invocation of a processUnstructuredSS-Request,
//unstructuredSS-Request, or an unstructuredSS-NotifyUSSD or due to the network 
//returning results to the MS as
//a result of an MS invocation of these operations.
typedef api_SsUSSDArgs_t api_SsUSSDRes_t;

//Union of results that may be returned by the network
//MMI determines field to use according to ss_Operation field of 
//mailbox messages of type api_SsActionCnf_t.
//If ss_Operation == processUnstructuredSS-Data, use the ss_USSDUserData field 
//of the union.
//If ss_Operation equals either processUnstructuredSS-Request, 
//unstructuredSS-Request, or unstructuredSS-Notify operations, 
//use the ss_USSDRes field of the union
typedef union
{
 //Use these params when the network invokes or returns a result for a 
 //processUnstructuredSS-Data operation 
 //with ss_USSDInvoke or ss_USSDReturnResult components.
 api_SsUSSDUserData_t ss_USSDUserData;

 //These params are returned when the Network invokes or returns a result for a
 //processUnstructuredSS-Request, 
 //unstructuredSS-Request, or unstructuredSS-Notify operation with an 
 //ss_USSDInvoke component.
 api_SsUSSDRes_t ss_USSDRes;
} api_SsUSSDNetworkResults_t;

//A structure encapsulating all USSD results received by the MS from the network
//when the network either invokes a USSD operation on the MS or returns a USSD 
//result to the MS in response to the MS invocation of a USSD operation 
typedef struct
{
 //Indicates the type of component received
 api_SsUSSDNetworkComponentType_t ss_USSDNetworkComponentType;

 //Indicates whether results are present (release complete messages may or may 
 //not have results)
 //Bool
 u8      bUSSDResultsPresent;

 api_SsUSSDNetworkResults_t   ss_USSDNetworkResult;
} api_SsUSSDInfo_t;

//The type of results returned from the network
typedef enum
{
 //indicates presence of results from an interrogation operation
 ss_InterrogateResultsPresent = 1,  

 //indicates presence of results from a call forwarding operation (except 
 //interrogation)
 ss_ForwardingInfoPresent     = 2,   

 //indicates presence of results from a call barring operation (except 
 //interrogation)
 ss_CallBarringInfoPresent    = 3,   

 //indicates presence of results from operations (except interrogation) for 
 //other ss codes
 ss_SSDataPresent             = 4,   

 //indicates presence of results from a USSD V2 operation (either Network 
 //initiated or in response to an MS initiated operation)
 ss_USSDV2ResultsPresent      = 5,   

 //indicates presence of results from a USSD V1 operation (either Network 
 //initiated or in response to an MS initiated operation)
 ss_USSDV1ResultsPresent      = 6,  

 // no results are available for the operation
 ss_NoRetRsltInfoPresent      = 0   
} api_SsRetRsltInfoPresence_t;

// a union of all possible data structures containing results that may be 
// returned from the network.
typedef union
{
 api_SsIntRslt_t          ss_INT; //results obtained from network for all 
                                  //interrogate operations
 api_SsForwardingInfo_t   ss_FI;  //results obtained from network for all call 
                                  //forwarding operations (except interrogate)
 api_SsCallBarringInfo_t  ss_CB;  //results obtained from network for all call 
                                  //barring operations (except interrogate)
 api_SsData_t             ss_D;   //results obtained from network for all 
                                  //operations (except interrogate) for all 
                                  //other ss codes 
 api_SsUSSDInfo_t         ss_USSD;//results obtained from network for all 
                                  //USSD operations
} api_SsRetRslt_t;

//A data structure encapsulating a union of all possible results that may be 
//returned from the network and an enum field for determining which field of 
//the union if any is applicable to the current mailbox message of type 
//api_SsActionCnf_t
typedef struct
{
 api_SsMessageType_t    ss_RxMessageType;//The Type of Message Received: 
                                         //Register, Facility, or Release 
                                         //Complete

 //Indicates the types of result received by the MS from the network
 //The value depends upon the SS-Code and SS-Operation for the transaction.
 //This value should be decoded in order to retrieve the results from the 
 //ss_RetRslt union.
 //If this field == ss_InterrogateResultsPresent, the ss_INT field of the union 
 //contains the results.
 //If this field == ss_ForwardingInfoPresent, the ss_FI field of the union 
 //contains the results.
 //If this field == ss_CallBarringInfoPresent, the ss_CB field of the union 
 //contains the results.
 //If this field == ss_SSDataPresent, the ss_D field of the union contains 
 //the results.
 //If this field == ss_USSDResultsPresent, the XXXXX field of the union contains
 //the results.
 //If this field == ss_NoRetRsltInfoPresent, the union contains the no results.
 api_SsRetRsltInfoPresence_t   ss_RetRsltInfoPresence; 

 //A union of the various data structures that may be received from the network.
 api_SsRetRslt_t    ss_RetRslt;


} api_SsReturnResultComponent_t;

//An action cnf is sent by the SS module to the API module after an SS
//transaction has received a either a return result from the network for an MS 
//initiated transaction or after the MS has received an invocation from the 
//network for USSD operations for network initiated transactions.
typedef struct
{
 u8                             TI;             // Same as in the action request
 api_SsCode_t                   ss_Code ;       // Same as in the action request
 api_SsOperation_t              ss_Operation;   // Same as in the action request
 api_SsReturnResultComponent_t  ss_ReturnResult;// any values rcvd from the nwk
} api_SsActionCnf_t ;

/*
================================================================================
  Structure  : api_SsErrorInd_t
--------------------------------------------------------------------------------
  Direction  : SS -> API -> MMI
  Scope      : indicate that a given request could not be executed
================================================================================
*/

//data structure encapsulating the results returned by the network in a Reject 
//component
typedef struct
{
 u8  ss_ProblemCodeTag; //the problem code tag received from the network
 u8  ss_ProblemCode;    //the problem code received from the network
} api_SsRejectComponentU8Tag_t;

//Cause IE
//Coding Standard Field for Cause IE, Octet 3
//Per 24.008, The mobile station need not (and does not) support any other 
//standard than GSM_PLMNS
#define API_SS_ITU_T_REC_Q_931                  (0)
#define API_SS_RESERVED_INTERNATIONAL_STANDARD  (1)
#define API_SS_NATIONAL_STANDARD                (2)
#define API_SS_GSM_PLMNS                        (3)

//Location Field for Cause IE, Octet 3
#define API_SS_User                                 (0)
#define API_SS_PRIVATE_NETWORK_SERVING_LOCAL_USER   (1)
#define API_SS_PUBLIC_NETWORK_SERVING_LOCAL_USER    (2)
#define API_SS_TRANSIT_NETWORK                      (3)
#define API_SS_PUBLIC_NETWORK_SERVING_REMOTE_USER   (4)
#define API_SS_PRIVATE_NETWORK_SERVING_REMOTE_USER  (5)
#define API_SS_INTERNATIONAL_NETWORK                (7)
#define API_SS_NETWORK_BEYOND_INTERNETWORKING_POINT (10)

//Cause field for all non GSM PLMNS encoded standards
#define API_SS_CAUSE_IE_INTERNETWORKING_UNSPECIFIED (127)
#define API_SS_CAUSE_IE_FACILITY_REJECTED           (0x9D)

//Data Structure encapsulating information received in a Cause IE
typedef struct
{
 u8    ss_CauseIECodingStandard;
 u8    ss_CauseIELocation;
 u8    ss_CauseValue;
} api_SsCauseIEInfo_t;

//A union of errors that may be returned from the network in a Return Error or 
//Reject component
typedef union
{
 //Used when the ss_ErrorSource Field of an api_SsErrorInd_t 
 //mailbox msg == ss_RejectComponent
 api_SsRejectComponentU8Tag_t ss_ProblemCodeTagandCode; 
 //Used when ss_ErrorSource Field of an api_SsErrorInd_t 
 //mailbox msg == ss_ReturnErrorComponent
 u8                           ss_NetworkErrorCode;
 //Used when the ss_ErrorSource field of an api_SsErrorInd_t 
 //mailbox msg == ss_CauseIE
 api_SsCauseIEInfo_t          ss_CauseIEInfo;
 //Used when the ss_ErrorSource field of an api_SsErrorInd_t 
 //mailbox msg == ss_LocalError
 api_SsLocalErrorCodes_t      ss_LocalErrorCode;
} api_SsErrors_t;

//Enumerates the source of the error
typedef enum
{
 //Network returned a reject component, look in ss_Errors.ss_ProblemCodeandTag
 ss_RejectComponent      = API_SS_REJECT_COMPONENT, 

 //Nwk returned a return error component, look in ss_Errors.ss_NetworkErrorCode
 ss_ReturnErrorComponent = API_SS_RETURN_ERROR, 

 //Nwk returned a Cause IE, look in ss_Errors.ss_CauseIEInfo
 ss_CauseIE = API_SS_CAUSE_IE, 

 //CISS Module returned a local error, look in ss_Errors.ss_LocalErrorCode
 ss_LocalError = 0 
} api_SsErrorSource_t;

typedef struct
{
 // TI if known, otherwise 0xFF
 u8                  TI; 

 // Same as in the api_SsActionReq mailbox msg that caused the error
 api_SsCode_t        ss_Code; 

 // Same as in the api_SsActionReq mailbox msg that caused the error
 api_SsOperation_t   ss_Operation;

 //The source of the error
 api_SsErrorSource_t ss_ErrorSource;
 
 // Network Errors depend upon the cause.
 //If ss_ErrorSource == ss_RejectComponent then use the ss_ProblemCodTagandCode
 //field of the union
 //If ss_ErrorSource == ss_ReturnErrorComponent then use the ss_ErrorCode field
 //of the union
 //If ss_ErrorSource == ss_CauseIE then use the ss_CauseIE field of the union
 //Otherwise this field is NOT relevant.
 api_SsErrors_t      ss_Errors;
} api_SsErrorInd_t ;


/*****************************
** API-CALL CONTROL INTERFACE
******************************/

/*
** Structure for the Charging Information Sequence -- See 3GPP 22.024
*/
typedef struct
{
 u16 e1;
 u16 e2;
 u16 e3;
 u16 e4;
 u16 e5;
 u16 e6;
 u16 e7;
} api_CrssChargInfo_t;
#define API_CRSS_E_NOT_VALID 0xFFFF

/*
** Structure for the Explicit Call Transfer Indicator
*/
typedef struct
{
 bool ECTCallState;             /* FALSE: Alerting, TRUE: Active */
 bool RdnPresent;               /* Is the Rdn structure valid    */
 union
 {
  bool PresentationRestricted;
  bool NberNotAvaiDueToInterworking;
  struct
  {
   u8   PANLen;                  /* Presentation Allowed Address Number       */
   u8   PASALen;                 /* Presentation Allowed Address SubAddress   */
   u8   PAN[MAX_FWD_TO_NUM_LEN];
   u8   PASA[MAX_FWD_TO_SUBADDR_LEN];
  } PAA;                        /* Presentation Allowed Address               */
  struct
  {
   u8   PRNLen;                  /* Presentation Restricted Address Number    */
   u8   PRSALen;                 /* Presentation Restricted Address SubAddress*/
   u8   PRN[MAX_FWD_TO_NUM_LEN];
   u8   PRSA[MAX_FWD_TO_SUBADDR_LEN];
  } PRA;                        /* Presentation Restricted Address            */
 } Rdn;
} api_CrssECTInd_t;

/*
** Structure for the Name Set
*/
typedef struct
{
 u32 LenInChar;                 /* Length In Characters */
 u8  NameString[160];           /* Name String          */
} api_CrssNameSet_t;

/*
** Structure for the Name Indicator
*/

//[[ [mod][dmx][20130902]
// change the api_CrssNameInd_t from union to struct
//typedef union
//{
// bool              PresentationRestricted;
// bool              NameUnavailable;
// api_CrssNameSet_t NamePresAllowed;
// api_CrssNameSet_t NamePresRestricted;
//} api_CrssNameInd_t;

typedef struct
{
 bool              PresentationRestricted;
 bool              NameUnavailable;
 bool              NamePresAllowedPresent;
 bool              NamePresRestrictedPresent;
 api_CrssNameSet_t Nameset;
} api_CrssNameInd_t;
// ]] [mod][dmx][20130902]

/*
** Structure for the Call Deflection
*/
typedef struct
{
 u8 DTNLen;                     /* DTN: Deflected To Number */
 u8 DTSALen;                    /* DTSA: Defleced To SubAddress */
 u8 DTN[MAX_FWD_TO_NUM_LEN];
 u8 DTSA[MAX_FWD_TO_SUBADDR_LEN];
} api_CrssDeflec_t;

/*
** Structure for the Call Related SS parameters
*/
typedef struct
{
 u8   ServiceNb;                /* Service 1, 2 or 3 */
 bool Required ;
} api_CrssUUS_t;

/*
** Structure for the Error indication related to CRSS
*/
typedef struct
{
 u8 Cause;          /* API_SS_REJECT_COMPONENT or API_SS_RETURN_ERROR        */
 u8 Code;           /* If cause == RejectComponent then Code = Problem Code, */
                    /* If Cause == Return Error, then code == Error Code     */
 u8 ProblemCodeTag; /* If cause == RejectComponent then                      */
                    /* ProblemCodeTag = ProblemCodeTag else not meaningful   */
} api_CrssErrorInd_t;

/*
** Generic structures for the Facility Ind IE
*/
typedef struct
{
 bool                StructValid;       /* TRUE if the struct is valid        */
 u8                  Status;
 u8                  InvokeId;         /* Id of the current Invoke proc.      */
 api_SsOperation_t   ss_Operation;
 api_SsCode_t        ss_Code     ;
 api_CrssChargInfo_t crss_ChargInfo; /* Valid if ss_Op == forwardChargeAdvice */
 api_CrssECTInd_t    crss_ECTIndicator; /* Valid if ss_Op   == ss_Notify &&   */
                                        /*          ss_Code == ss_ECT         */
 api_CrssNameInd_t   crss_NameIndcator; /* Valid if ss_Op   == ss_Notify &&   */
                                        /*          ss_Code == ss_CNAP        */
 api_CrssUUS_t       crss_UUS ;         /* Valid if ss_Op == userUserService  */
 api_CrssErrorInd_t  crss_ErrorInd;  /* Valid if Status == API_CRSS_ERROR_IND */
} api_CrssFacilityIndIE_t;

/*
** Defines for the ss_Status field
*/
#define API_CRSS_NO_STATUS                 0
#define API_CRSS_MOCALL_HAS_BEEN_FORWARDED ( 1 << 2 ) /* values from the  */
#define API_CRSS_MTCALL_HAS_BEEN_FORWARDED ( 1 << 1 ) /* ss_notification  */
#define API_CRSS_MTCALL_IS_FORWARDED_CALL  ( 1 << 0 ) /* see 24.080       */
#define API_CRSS_CALL_ON_HOLD              5
#define API_CRSS_CALL_RETRIEVED            6
#define API_CRSS_CALL_IS_WAITING           7
#define API_CRSS_CLIR_SUP_REJECTED         8
#define API_CRSS_STATUS_ACTIVE_OPERATIVE   9
#define API_CRSS_MPTY_INDICATION           10
#define API_CRSS_RETURN_RESULT             11
#define API_CRSS_ERROR_IND                 12

/*
** Generic structures for the Facility Req IE
*/
typedef struct
{
 bool                StructValid;      /* TRUE if the struct is valid         */
 u8                  InvokeId;         /* Id of the current Invoke proc.      */
 api_SsOperation_t   ss_Operation;
 api_SsCode_t        ss_Code     ;
 api_CrssDeflec_t    crss_Deflec ;     /* Valid if ss_Op == CallDeflection   */
 struct {
  u8                 Status;           /* API_CRSS_RETURN_RESULT,             */
                                       /* API_CRSS_ERROR_IND or               */
                                       /* API_CRSS_NO_STATUS                  */
  u8                 ErrorCode;        /* Valid if ReturnValue == ReturnError */
  u8                 UUSNb;            /* Indicate the nb of crss_UUS valid   */
  api_CrssUUS_t      crss_UUS[3] ; 
 } UUS;                                /* Valid if ss_Op == userUserService   */
} api_CrssFacilityReqIE_t;


/*
================================================================================
  Structure  : api_CcSetupReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Ask for the establishment of an outgoing call
================================================================================
*/
typedef struct
{
 u8   CallType              ;
 bool CLIRSuppression       ;  // TRUE means CLIR service is suppressed
 bool CLIRInvocation        ;  // TRUE means CLIR service is invoced   
 u8   CPNLen                ;  // Called Party Number field length     
 u8   CPSALen               ;  // Called Party SubAdd field length     
 u8   Bearer1Len            ;  // Bearer Cap 1 field length
 u8   Bearer2Len            ;  // Bearer Cap 2 field length
 u8   UserUserLen           ;  // UserUser field length
 u8   Bearer1[14]           ;
 u8   Bearer2[14]           ;
 u8   CalledPartyNb[41]     ;
 u8   CalledPartySubAdd[21] ;
 u8   UserUser[33]          ;
 api_CrssFacilityReqIE_t Facility;

} api_CcSetupReq_t ;

/*
** Possible values for CallType
*/
#define API_CC_REGULAR_CALL   1
#define API_CC_EMERGENCY_CALL 2

/*
================================================================================
  Structure  : api_CcCallConfReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to confirm an incoming call request
================================================================================
*/
typedef struct
{
 u8 TI          ;  
 u8 CauseLen    ;
 u8 Bearer1Len  ;  // Bearer Cap 1 field length
 u8 Bearer2Len  ;  // Bearer Cap 2 field length
 u8 Bearer1[14] ;
 u8 Bearer2[14] ;
 u8 Cause[30]   ;

} api_CcCallConfReq_t ;


/*
================================================================================
  Structure  : api_CcAlertReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Indicate that user alerting has been initiated by the MS.
================================================================================
*/
typedef struct
{
 u8                      TI ;
 u8                      UserUserLen  ;  // UserUser field length
 u8                      UserUser[129];
 api_CrssFacilityReqIE_t Facility;
} api_CcAlertReq_t ;


/*
================================================================================
  Structure  : api_CcConnectReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to inform that the incoming call has been accepted by the MS
================================================================================
*/
typedef struct
{
 u8 TI                  ;
 u8 CSALen              ;  // Connceced SubAdd field length     
 u8 UserUserLen         ;  // UserUser field length
 u8 ConnectedSubAdd[21] ;
 u8 UserUser[129]       ;
 api_CrssFacilityReqIE_t Facility; // Only used for the User-User CRSS
} api_CcConnectReq_t ;


/*
================================================================================
  Structure  : api_CcDiscReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask for the disconnection of a call
================================================================================
*/
typedef struct
{
 u8 TI           ;  
 u8 CauseLen     ;
 u8 UserUserLen  ;  // UserUser field length
 u8 Cause[30]    ;
 u8 UserUser[129];
 api_CrssFacilityReqIE_t Facility;
} api_CcDiscReq_t ;


/*
================================================================================
  Structure  : api_CcReleaseReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Indicate that the MS intends to release the TI
================================================================================
*/
typedef struct
{
 u8 TI         ;  
 u8 Cause1Len  ;
 u8 Cause2Len  ;
 u8 UserUserLen;  // UserUser field length
 u8 Cause1[30] ;
 u8 Cause2[30] ;
 u8 UserUser[129];
} api_CcReleaseReq_t ;


/*
================================================================================
  Structure  : api_CcReleaseCmpltReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Indicate that the MS has released the TI.
================================================================================
*/
typedef struct
{
 u8 TI        ;  
 u8 CauseLen  ;
 u8 UserUserLen  ;  // UserUser field length
 u8 UserUser[129];
 u8 Cause[30] ;
} api_CcReleaseCmpltReq_t ;


/*
================================================================================
  Structure  : api_CcStartDTMFReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to start a DTMF transmission
================================================================================
*/
typedef struct
{
 u8         TI         ;
 u8         KeyPressed ;
} api_CcStartDTMFReq_t ;


/*
================================================================================
  Structure  : api_CcStopDTMFReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to stop a DTMF transmission
================================================================================
*/
typedef struct
{
 u8         TI        ;
} api_CcStopDTMFReq_t ;


/*
================================================================================
  Structure  : api_CcCallHoldReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to hold a call
================================================================================
*/
typedef struct
{
 u8         TI        ;
} api_CcCallHoldReq_t ;


/*
================================================================================
  Structure  : api_CcCallRetrieveReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to retrieve a call
================================================================================
*/
typedef struct
{
 u8          TI           ;
} api_CcCallRetrieveReq_t ;


/*
================================================================================
  Structure  : api_CcModifyReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to modify an existing call
================================================================================
*/
typedef struct
{
 u8 TI         ;

} api_CcModifyReq_t ;


/*
================================================================================
  Structure  : api_CcModifyCmpltReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to send a modify complete message
================================================================================
*/
typedef struct
{
 u8 TI         ;

} api_CcModifyCmpltReq_t;


/*
================================================================================
  Structure  : api_CcModifyRejReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to send a modify reject message
================================================================================
*/
typedef struct
{
 u8 TI         ;
 u8 CauseLen   ;
 u8 Cause[30]  ;

} api_CcModifyRejReq_t;


/*
================================================================================
  Structure  : api_CcNotifyReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to send a notify message
================================================================================
*/
typedef struct
{
 u8 TI       ;
 u8 NotifyInd;
} api_CcNotifyReq_t;


/*
================================================================================
  Structure  : api_CcFacilityReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used for an action on the Call Related SS
================================================================================
*/
typedef struct
{
 u8                      TI;
 api_CrssFacilityReqIE_t Facility;
} api_CcFacilityReq_t;


/*
================================================================================
  Structure  : api_CcUserInfoReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Request the emission of a User Information message
================================================================================
*/
typedef struct
{
 u8 TI;
 u8 UserUserLen;
 bool MoreData;
 u8 UserUser[129];
} api_CcUserInfoReq_t ;


/*
================================================================================
  Structure  : api_CcTIInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate the chosen TI for the outgoing call in process
================================================================================
*/
typedef struct
{
 u8 TI;
} api_CcTIInd_t;


/*
================================================================================
  Structure  : api_CcSetupInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate an incoming call
================================================================================
*/
typedef struct
{
 u8 TI                     ;
 u8 CPNLen                 ;  // Calling Party Number field length     
 u8 CPSALen                ;  // Calling Party SubAdd field length     
 u8 RPNLen                 ;  // Redirecting Party Number field length     
 u8 RPSALen                ;  // Redirecting Party SubAdd field length     
 u8 Bearer1Len             ;  // Bearer Cap 1 field length
 u8 Bearer2Len             ;  // Bearer Cap 2 field length
 u8 Signal                 ;
 u8 Alert                  ;
 u8 CauseOfNoCLI           ;  // Cause of No Calling Line Identification
 u8 UserUserLen            ;  // UserUser field length
 u8 ProgressInd[2]         ;
 u8 Bearer1[14]            ;
 u8 Bearer2[14]            ;
 u8 CallingPartyNb[12]     ;
 u8 CallingPartySubAdd[21] ;
 u8 RedirectingPartyNb[12]     ;
 u8 RedirectingPartySubAdd[21] ;
 u8 UserUser[33];
 api_CrssFacilityIndIE_t Facility;

} api_CcSetupInd_t ;

// Signal special values
#define API_SIGNAL_INVALID 0xFF

// Alert special values
#define API_ALERT_INVALID  0xFF

// Cause of no CLI values
#define API_CAUSEOFNOCLI_UNAVAILABLE  0
#define API_CAUSEOFNOCLI_REJECTBYUSER 1
#define API_CAUSEOFNOCLI_INTERACTSERV 2
#define API_CAUSEOFNOCLI_PAYPHONE     3
#define API_CAUSEOFNOCLI_INVALID      0x80

// Progress Indicator special values
#define API_PROGIND_INVALID  0


/*
================================================================================
  Structure  : api_CcCallProcInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Nwk indicate that the requested call estab. info. has been
               received
================================================================================
*/
typedef struct
{
 u8 TI             ;
 u8 NwkCcCap       ;  // Network Call Control Capabilities
 u8 Bearer1Len     ;  // Bearer Cap 1 field length
 u8 Bearer2Len     ;  // Bearer Cap 2 field length
 u8 Bearer1[14]    ;
 u8 Bearer2[14]    ;
 u8 ProgressInd[2] ;
 api_CrssFacilityIndIE_t Facility;
} api_CcCallProcInd_t ;

#define API_NWKCCCAP_INVALID 0xFF

/*
================================================================================
  Structure  : api_CcProgressInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate the reception of a progress indicator message
================================================================================
*/
typedef struct
{
 u8 TI             ;
 u8 UserUserLen    ;  // UserUser field length
 u8 ProgressInd[2] ;
 u8 UserUser[129]  ;

} api_CcProgressInd_t ;


/*
================================================================================
  Structure  : api_CcAlertInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate that user alerting has been started by the 
               called party
================================================================================
*/
typedef struct
{
 u8 TI             ;
 u8 UserUserLen    ;  // UserUser field length
 u8 ProgressInd[2] ;
 u8 UserUser[129]  ;
 api_CrssFacilityIndIE_t Facility;
} api_CcAlertInd_t ;


/*
================================================================================
  Structure  : api_CcConnectInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used by the nwk to inform that the outgoing call has been 
               accepted
================================================================================
*/
typedef struct
{
 u8 TI                  ;
 u8 CNLen               ;  // Connected Number field length     
 u8 CSALen              ;  // Connceced SubAdd field length     
 u8 UserUserLen         ;  // UserUser field length
 u8 UserUser[129]       ;
 u8 ProgressInd[2]      ;
 u8 ConnectedNb[12]     ;  
 u8 ConnectedSubAdd[21] ;
 api_CrssFacilityIndIE_t Facility;
} api_CcConnectInd_t ;


/*
================================================================================
  Structure  : api_CcConnectAckInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used by the nwk to inform that the MS has been awarded the call
================================================================================
*/
typedef struct
{
 u8 TI ;

} api_CcConnectAckInd_t ;


/*
================================================================================
  Structure  : api_CcDiscInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate the end-to-end connection is cleared
================================================================================
*/
typedef struct
{
 u8 TI             ; 
 u8 CauseLen       ;
 u8 UserUserLen    ;  // UserUser field length
 u8 ProgressInd[2] ;
 u8 Cause[30]      ;
 u8 UserUser[129]  ;
 api_CrssFacilityIndIE_t Facility;
} api_CcDiscInd_t ;


/*
================================================================================
  Structure  : api_CcReleaseInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used by the nwk to indicate that the nwk intends to release the
               TI
================================================================================
*/
typedef struct
{
 u8 TI           ;  
 u8 Cause1Len    ;
 u8 Cause2Len    ;
 u8 UserUserLen  ;  // UserUser field length
 u8 Cause1[30]   ;
 u8 Cause2[30]   ;
 u8 UserUser[129];
 api_CrssFacilityIndIE_t Facility;
} api_CcReleaseInd_t ;


/*
================================================================================
  Structure  : api_CcReleaseCmpltInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used by the nwk to indicate that the nwk has released the TI
================================================================================
*/
typedef struct
{
 u8 TI           ;  
 u8 CauseLen     ;
 u8 UserUserLen  ;  // UserUser field length
 u8 Cause[30]    ;
 u8 UserUser[129];
 api_CrssFacilityIndIE_t Facility;
} api_CcReleaseCmpltInd_t ;


/*
================================================================================
  Structure  : api_CcErrorInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used by API to indicate that something went wrong
================================================================================
*/
typedef struct
{
 u8 TI          ;
 u8 Cause       ;
 u8 NwkCauseLen ;
 u8 NwkCause[30];

} api_CcErrorInd_t ;

// Cause possible values
#define API_NORMAL_RELEASE          0
#define API_LOW_LAYER_FAIL          1
#define API_REQUEST_IMPOSSIBLE      2
#define API_INCOMING_CALL_BARRED    3
#define API_OUTGOING_CALL_BARRED    4
#define API_CALL_HOLD_REJECTED      5
#define API_CALL_RTRV_REJECTED      6
#define API_CC_TIMER_EXPIRY         7
#define API_CC_PROTOCOL_ERROR       8
#define API_AOC_EXPIRY              9

#define API_CC_DTMF_BUFFER_FULL    10

/*
================================================================================
  Structure  : api_CcCallHoldCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to confirm the hold of a call
================================================================================
*/
typedef struct
{
 u8        TI         ;
} api_CcCallHoldCnf_t ;


/*
================================================================================
  Structure  : api_CcCallRetrieveCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to confirm the call has been retrieved
================================================================================
*/
typedef struct
{
 u8        TI             ;
} api_CcCallRetrieveCnf_t ;


/*
================================================================================
  Structure  : api_CcCallWaitingInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to inform that a second call is waiting
================================================================================
*/
typedef struct
{
 u8         TI           ;
} api_CcCallWaitingInd_t ;


/*
================================================================================
  Structure  : api_CcAudioOnInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to inform that MMI can activate the audio
================================================================================
*/
typedef u8 api_CcAudioOnInd_t ;

/*
================================================================================
  Structure  : api_CcModifyInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate an incoming MODIFY message
================================================================================
*/
typedef struct
{
 u8 TI         ;

} api_CcModifyInd_t ;


/*
================================================================================
  Structure  : api_CcModifyCmpltInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate an incoming MODIFY COMPLETE message
================================================================================
*/
typedef struct
{
 u8 TI         ;

} api_CcModifyCmpltInd_t;


/*
================================================================================
  Structure  : api_CcModifyRejInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate an incoming MODIFY REJECT message
================================================================================
*/
typedef struct
{
 u8 TI         ;
 u8 CauseLen   ;
 u8 Cause[30]  ;

} api_CcModifyRejInd_t;


/*
================================================================================
  Structure  : api_CcNotifyInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate an incoming notify message
================================================================================
*/
typedef struct
{
 u8 TI       ;
 u8 NotifyInd;
} api_CcNotifyInd_t;


/*
================================================================================
  Structure  : api_CcFacilityInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : A facility message has been received
================================================================================
*/
typedef struct
{
 u8                  TI;
 api_CrssFacilityIndIE_t Facility[2];
} api_CcFacilityInd_t;


/*
================================================================================
  Structure  : api_CcUserInfoInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : indicate the reception of a User Information message
================================================================================
*/
typedef struct
{
 u8 TI;
 u8 UserUserLen;
 bool MoreData;
 u8 UserUser[129];
} api_CcUserInfoInd_t ;


/*
================================================================================
  Structure  : api_CcCongCtrlInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : indicate the reception of a Congestion Control message
================================================================================
*/
typedef struct
{
 u8 TI;
 u8 CongLevel;                  /* 0: reciever ready, 0x0F: not ready */
 u8 CauseLen   ;
 u8 Cause[30]  ;
} api_CcCongCtrlInd_t ;

/*
================================================================================
  Structure  : api_CcAocCCMInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : indicate that AOC CCM is updated
================================================================================
*/
typedef struct
{
 u32 CCM;
} api_CcAocCCMInd_t ;

/*
================================================================================
  Structure  : api_CcChanModeInd_t
--------------------------------
  Direction  : API -> MMI
  Scope      : API gives MMI the new channel mode
================================================================================
*/
typedef struct 
{
 u32 ChanMode;  
} api_CcChanModeInd_t ;

/***************************************************************
** API-PDP CONTEXT MANAGEMENT AND PACKET DATA TRANSFER INTERFACE
****************************************************************/
typedef enum
{
  LLC_OR_SNDCP_FAILURE = 0x19,
  INSUFFICIENT_RESOURCES,
  MISSING_OR_UNKNOWN_APN,
  UNKNOWN_PDP_ADDRESS_OR_PDP_TYPE,
  USER_AUTHENTICATION_FAILED,
  ACTIVATION_REJECTED_BY_GGSN,
  ACTIVATION_REJECTED_UNSPECIFIED,
  SERVICE_OPTION_NOT_SUPPORTED,
  REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED,
  SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER,
  NSAPI_ALREADY_USED,
  REGULAR_DEACTIVATION,
  QOS_NOT_ACCEPTED,
  NETWORK_FAILURE,
  REACTIVATION_REQUIRED,
  FEATURE_NOT_SUPPORTED,
  SEMANTIC_ERROR_IN_THE_TFT_OPERATION,
  SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION,
  UNKNOWN_PDP_CONTEXT,
  PDP_CONTEXT_WITHOUT_TFT_ALREADY_ACTIVATED,
  SEMANTIC_ERRORS_IN_PACKET_FILTER,
  SYNTACTICAL_ERRORS_IN_PACKET_FILTER,
  INVALID_TRANSACTION_IDENTIFIER_VALUE = 0x51,
  SEMANTICALLY_INCORRECT_MESSAGE = 0x5F,
  INVALID_MANDATORY_INFORMATION,
  MESSAGE_TYPE_NOT_EXISTENT_OR_NOT_IMPLEMENTED,
  MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE,
  INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED,
  CONDITIONAL_IE_ERROR,
  MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE,
  PROTOCOL_ERROR_UNSPECIFIED = 0x6F
}sm_Cause_t;

/*
================================================================================
  Structure  : api_PdpCtxActivateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to activate a PDP context
================================================================================
*/
#define API_QOS_LEN_R98 3 
#define API_QOS_LEN_R99 11
#define API_QOS_LEN_MAX 16  // for R6

#ifdef LTE_NBIOT_SUPPORT
#define API_APN_LEN_MAX 100
#define API_PCO_LEN_MAX 512
#define API_PDP_ADD_LEN_MAX 18
#define API_TFT_LEN_MAX 255
#define API_EPS_QOS_LEN 13
#else //#ifdef LTE_NBIOT_SUPPORT
#define API_APN_LEN_MAX 100
#define API_PCO_LEN_MAX 251
#define API_PDP_ADD_LEN_MAX 18
#endif

typedef struct
{
  u8  SmIndex                 ; // 0xFF if no SmIndex
  u8  NSapi                   ;
  u8  QoSLen                  ;
 u8  QoS[API_QOS_LEN_MAX]              ;  
 u8  PDPAddLen            ;
  u8  PDPAddress[API_PDP_ADD_LEN_MAX]       ;
  u8  APNLen      ;
  u8  AccessPointName[API_APN_LEN_MAX] ;
 #ifdef LTE_NBIOT_SUPPORT
   u16  ProtocolCfgOptionLen    ;
 #else
  u8  ProtocolCfgOptionLen    ;
 #endif
  u8  ProtocolCfgOption[API_PCO_LEN_MAX]  ;
  //add by yanzhili
  #ifdef LTE_NBIOT_SUPPORT
  u8  nCid;
  u8  PdnType; // IPV4/V6/Non-IP
  u8  nSlpi;
  #endif
} api_PdpCtxActivateReq_t ;
//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
typedef struct
{
  u8  APNLen      ;
  u8  AccessPointName[API_APN_LEN_MAX] ;
  u16  ProtocolCfgOptionLen    ;
  u8  ProtocolCfgOption[API_PCO_LEN_MAX]  ;
  u8  PdnType; // IPV4/V6/Non-IP
} api_DefaultPdnCtxConfigReq_t ;

/*
================================================================================
  Structure  : api_EpsCtxActivateRsp
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to Accept the Dedicated context Activate requested by the network 
================================================================================
*/
typedef struct
{
 u8   SmIndex           ;
} api_EpsCtxActivateRsp_t ;

/*
================================================================================
  Structure  : api_EpsCtxActivateRej
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to Reject the Dedicated context Activate requested by the network 
================================================================================
*/
typedef struct
{
 u8   SmIndex           ;
 u8   Cause             ;
} api_EpsCtxActivateRej_t ;

/*
================================================================================
  Structure  : api_EpsCtxActivateInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to Indicate the Dedicated context Activate arrived
================================================================================
*/
//#define API_TFT_LEN_MAX 255   redefine
//#define API_EPS_QOS_LEN 13    redefine
typedef struct
{
 u8   SmIndex                 ;
 u8   Ebi                     ;
 u8   LinkedEbi               ;
 u8   actType                 ;
 u8   TFTLen                  ;
 u8   TFT[API_TFT_LEN_MAX]    ;
 u8   EpsQoSLen               ;
 u8   EpsQoS[API_EPS_QOS_LEN] ;
 u8       apnrcAer;
 u8       apnrcUnit;
 u32      apnrcRate;
} api_EpsCtxActivateInd_t ;

/*
================================================================================
  Structure  : api_EpsCtxAllocateReq
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to Request the Bearer Resource Allocation Procedure 
================================================================================
*/
typedef struct
{
 u8   SmIndex                 ;
 u8   LinkedEbi               ;
 u8   TFTLen                  ;
 u8   TFT[API_TFT_LEN_MAX]    ;
 u8   EpsQoSLen               ;
 u8   EpsQoS[API_EPS_QOS_LEN] ;
} api_EpsCtxAllocateReq_t ;

/*
================================================================================
  Structure  : api_EpsCtxAllocateCnf
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to Indicate the Allocation Procedure Completed
================================================================================
*/
typedef struct
{
 u8   SmIndex                 ;
 u8   respType                ;
} api_EpsCtxAllocateCnf_t ;

/*
================================================================================
  Structure  : api_EpsCtxAllocateRej
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to Indicate the Allocation Procedure Failure
================================================================================
*/
typedef struct
{
 u8   SmIndex                 ;
 u8   Cause                   ;
} api_EpsCtxAllocateRej_t ;


typedef struct
{
    u8 changedValue;
}api_NvParasChangeInd_t;
#endif //  #ifdef LTE_NBIOT_SUPPORT

/*
================================================================================
  Structure  : api_PdpCtxModifyReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to request a change of QoS on the active PDP context
================================================================================
*/
typedef struct
{
  u8  SmIndex                 ;
  u8  QoSLen                  ;
 u8             QoS[API_QOS_LEN_MAX] ;  
 #ifdef LTE_NBIOT_SUPPORT
  u8  TFTLen                  ;
  u8  TFT[API_TFT_LEN_MAX]    ;
  u8  EpsQoSLen               ;
  u8  EpsQoS[API_EPS_QOS_LEN] ;
#endif
} api_PdpCtxModifyReq_t ;

/*
================================================================================
  Structure  : api_PdpCtxModifyAcc_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to accept the PDP context modification requested by the network 
================================================================================
*/
typedef struct
{
 u8   SmIndex           ;
} api_PdpCtxModifyAcc_t ;

/*
================================================================================
  Structure  : api_PdpCtxDeactivateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to de-activate the active PDP context
================================================================================
*/
typedef struct
{
  u8  SmIndex     ;
 u8                   Cause ;  
#ifdef LTE_NBIOT_SUPPORT
   u8  TFTLen              ;
   u8  TFT[API_TFT_LEN_MAX];
#endif
} api_PdpCtxDeactivateReq_t ;

/*
================================================================================
  Structure  : api_PdpCtxActivateCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to confirm that the requested PDP context was successfully
               activated
================================================================================
*/
typedef struct
{
  u8    SmIndex               ;
  u8    PDPAddLen             ;
  u8    PDPAddress[API_PDP_ADD_LEN_MAX]        ;
  #ifdef LTE_NBIOT_SUPPORT
    u16  ProtocolCfgOptionLen    ;
  #else
  u8    ProtocolCfgOptionLen  ;
  #endif
  u8    ProtocolCfgOption[API_PCO_LEN_MAX];
  u8    QoSLen                ;
  u8    QoS[API_QOS_LEN_MAX]               ;
  //add by yanzhili
  #ifdef LTE_NBIOT_SUPPORT
  u8  APNLen;
  u8  AccessPointName[API_APN_LEN_MAX];
  u8  PdnType; // IPV4/V6/Non-IP
  u8    EpsQoSLen             ;
  u8    EpsQoS[API_EPS_QOS_LEN];
  u8       apnrcAer;
  u8       apnrcUnit;
  u32      apnrcRate;
  #endif
} api_PdpCtxActivateCnf_t ;

/*
================================================================================
  Structure  : api_PdpCtxSmIndexInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Indicates SmIndex after a api_PdpCtxActivateReq
================================================================================
*/
typedef struct
{
 u8 SmIndex              ;
 u8 NSapi                ;
 #ifdef LTE_NBIOT_SUPPORT
 u8 nCid                 ;
 #endif //  #ifdef LTE_NBIOT_SUPPORT
} api_PdpCtxSmIndexInd_t ;

/*
================================================================================
  Structure  : api_PdpCtxDeactivateCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to confirm that the PDP context was successfully deactivated
================================================================================
*/
typedef struct
{
  u8    SmIndex             ;
} api_PdpCtxDeactivateCnf_t ;

/*
================================================================================
  Structure  : api_PdpCtxActivateRej_t
--------------------------------------------------------------------------------
  Direction  : API <-> MMI
  Scope      : to indicate a failure during pdp context activation
================================================================================
*/
typedef struct
{
  u8    SmIndex                ;
  u8    Cause                  ;
#ifdef LTE_NBIOT_SUPPORT
  u16    ProtocolCfgOptionLen   ;  // must be 0 for uplink

#else
  u8    ProtocolCfgOptionLen   ;  // must be 0 for uplink
#endif
  u8    ProtocolCfgOption[API_PCO_LEN_MAX] ;  // Unused for uplink
} api_PdpCtxActivateRej_t ;

/*
================================================================================
  Structure  : api_PdpCtxModifyRej_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to indicate a failure during pdp context modification
================================================================================
*/
typedef struct
{
  u8    SmIndex                ;
  u8    Cause                  ;
} api_PdpCtxModifyRej_t ;

/*
================================================================================
  Structure  : api_PdpCtxActivateInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to indicate that the network wishes to activate a PDP context
================================================================================
*/
typedef struct
{
 u8     SmIndex             ;
 u8   PDPAddLen           ;
 u8   APNLen              ;
 u8   PDPAddress     [API_PDP_ADD_LEN_MAX] ;
 u8     AccessPointName[API_APN_LEN_MAX];
} api_PdpCtxActivateInd_t ;

/*
================================================================================
  Structure  : api_PdpCtxModifyCnf
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to confirm that the active PDP context was successfully modified
================================================================================
*/
typedef struct
{
 u8     SmIndex             ;
 u8     QoSLen              ;
 u8             QoS[API_QOS_LEN_MAX] ;
} api_PdpCtxModifyCnf_t ;

/*
================================================================================
  Structure  : api_PdpCtxModifyInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to indicate that the network wishes to change the QoS for the
               active PDP context
================================================================================
*/
typedef struct
{
 u8       SmIndex       ;
 u8       QoSLen        ;
 u8       QoS[API_QOS_LEN_MAX]       ;
 u8       PDPAddLen     ;
 u8       PDPAddress[API_PDP_ADD_LEN_MAX];
 #ifdef LTE_NBIOT_SUPPORT
 u16    ProtocolCfgOptionLen  ;
 u8    ProtocolCfgOption[API_PCO_LEN_MAX];
 u8       TFTLen        ;
 u8       TFT[API_TFT_LEN_MAX]; 
 u8       EpsQoSLen     ;
 u8       EpsQoS[API_EPS_QOS_LEN];
 u8       apnrcAer;
 u8       apnrcUnit;    //if 0xff, means no valid apn rate control parameter
 u32      apnrcRate;
 #endif 
} api_PdpCtxModifyInd_t ;

/*
================================================================================
  Structure  : api_PdpCtxDeactivateInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : to indicate that the PDP context has been deactivated by the
               network
================================================================================
*/
typedef struct
{
 u8          SmIndex        ;
 u8          Cause          ;
} api_PdpCtxDeactivateInd_t ;

#ifdef __MINQOS__
/*
================================================================================
  Structure  : api_PdpSetMinQoSReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to configure a minimum QoS
================================================================================
*/
typedef struct
{
  bool  Cfg              ;
  u8    QoSLen           ;
  u8    QoS[API_QOS_LEN_MAX]          ;
} api_PdpSetMinQoSReq_t;
#endif

/*
================================================================================
  Structure  : api_PSDataReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : to send a user packet data block
================================================================================
*/
typedef struct
{
 u8   NSAPI       ;
 u8   Mode        ;
} api_PSDReqCtrl_t ;

//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
typedef struct
{
 u32 nsapiMode; //NSAPI  | mode <<8;
 u8 reserved1;
 u8 dataOffset; //Data will be found at UserData[DataOffset]
 u16 dataLen; //Contain DataOffset
 u8 *userData;
} api_PSDReqNbiotCtrl_t;
#endif
// Possible values for Mode
#define API_PDP_ACK   0
#define API_PDP_UNACK 1

// ATTENTION : the nb of bytes allocated for an API_PSDATA_REQ message must be:
// API_PSDATA_REQ_SIZE(DataLen), where DataLen is the nb of radio bytes MMI 
// wants to send in the API_PSDATA_REQ message ; 
// API_PSDATA_REQ_SIZE is defined below
typedef struct
{
 api_PSDReqCtrl_t Ctrl        ;
 u8               Reserved[ILHDR_SPECIFIC_SIZE-sizeof(api_PSDReqCtrl_t)];
 u16              DataLen     ; // Nb of bytes of the UserData field 
                                // !!! MUST include DataOffset 
 u8               DataOffset  ; // Data will be found at UserData[DataOffset]
 u8               Pad         ; // Do not use
 u8               UserData[1] ; // DataLen elts are allocated for this array
} api_PSDataReq_t ;
#ifdef LTE_NBIOT_SUPPORT
typedef struct
{
 u8                 nsapi;
 T_ComPduLte*       pUserData; 
} api_PSDataReqLte_t ;
#endif
#define API_PSDATA_OFFSET             RAD_OFFS_PSDATA
#define API_PSDATA_LEN( _DataLen )    ( _DataLen + API_PSDATA_OFFSET )
#define API_PSDATA_REQ_SIZE( _DataLen ) \
        (  sizeof(MsgHead_t) + OFFSETOF(api_PSDataReq_t,UserData) \
         + API_PSDATA_LEN(_DataLen) + PSDATA_FOOTER_SIZE )

#define API_PSALLOC_DATAREQ(_Ptr, _DataLen) \
    { \
      _Ptr = sxr_Malloc(API_PSDATA_REQ_SIZE(_DataLen)); \
      ((u8 *)_Ptr)[sizeof(MsgHead_t) + OFFSETOF(api_PSDataReq_t,DataOffset)] = RAD_OFFS_PSDATA; \
    }


/*
================================================================================
  Structure  : api_PSQueueCreateInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API indicates MMI the ID of the queue to use for 
               PS data transmission
================================================================================
*/
typedef struct 
{
 u8 QueueId ;
} api_PSQueueCreateInd_t ;

/*
================================================================================
  Structure  : api_UlCongestInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Message    : API_ULCONGEST_IND
  Scope      : API informs MMI that the uplink congestion status has changed 
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8 Status ;  
} api_UlCongestInd_t ;

// Possible values for Status
#define API_PS_ULCONG_OFF 0  // No congestion on the uplink
#define API_PS_ULCONG_ON  1  // Uplink radio path congested

/*
================================================================================
  Structure  : api_PSDataInd_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : reception of a user packet data block
================================================================================
*/
typedef api_PSDReqCtrl_t api_PSDIndCtrl_t ;

typedef struct
{
 api_PSDIndCtrl_t Ctrl        ;
 u8               Reserved[ILHDR_SPECIFIC_SIZE-sizeof(api_PSDIndCtrl_t)];
 u16              DataLen     ; // Nb of bytes of the UserData field 
                                // Includes DataOffset 
 u8               DataOffset  ; // Data will be found at UserData[DataOffset]
 u8               Pad         ; // Do not use
 u8               UserData[1] ; // DataLen elts are allocated for this array
} api_PSDataInd_t ;

//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
typedef struct
{
 u8               nsapi   ;
 T_ComSduLte*     pUserData; 
} api_PSDataIndLte_t ;

typedef api_PSDReqNbiotCtrl_t api_PSDataNbiotInd_t;

#endif

/*******************
** API-SIM INTERFACE
********************/

/*
================================================================================
  Structure  : api_SimResetReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI is asking for a reset of the SIM
================================================================================
*/
// typedef u8 api_SimResetReq_t ;
typedef struct
{
 u8    Pad;			// Room for Src Mbx
 BOOL UsimFlag;		// Usim Flag
} api_SimResetReq_t ;

/*
================================================================================
  Structure  : api_SimReadBinaryReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI is asking to read a binary file in the SIM
================================================================================
*/
typedef struct
{
 u8             Pad      ; // Room for Src Mbx
 u8             File     ; // See the defines below for the possible values
 u16            Offset   ;
 u8             NbBytes  ;

} api_SimReadBinaryReq_t ;

// Possible value for File:
// Elementary file name.
#define  API_SIM_EF_ADN       0
#define  API_SIM_EF_FDN       1
#define  API_SIM_EF_SMS       2
#define  API_SIM_EF_CCP       3
#define  API_SIM_EF_MSISDN    4
#define  API_SIM_EF_SMSP      5
#define  API_SIM_EF_SMSS      6
#define  API_SIM_EF_LND       7
#define  API_SIM_EF_SMSR      8
#define  API_SIM_EF_SDN       9
#define  API_SIM_EF_EXT1      10
#define  API_SIM_EF_EXT2      11
#define  API_SIM_EF_EXT3      12
#define  API_SIM_EF_BDN       13
#define  API_SIM_EF_EXT4      14
#define  API_SIM_EF_LP        15
#define  API_SIM_EF_IMSI      16
#define  API_SIM_EF_KC        17
#define  API_SIM_EF_PLMNSEL   18
#define  API_SIM_EF_HPLMN     19
#define  API_SIM_EF_ACMMAX    20
#define  API_SIM_EF_SST       21
#define  API_SIM_EF_ACM       22
#define  API_SIM_EF_GID1      23
#define  API_SIM_EF_GID2      24
#define  API_SIM_EF_PUCT      25
#define  API_SIM_EF_CBMI      26
#define  API_SIM_EF_SPN       27
#define  API_SIM_EF_CBMID     28
#define  API_SIM_EF_BCCH      29
#define  API_SIM_EF_ACC       30
#define  API_SIM_EF_FPLMN     31
#define  API_SIM_EF_LOCI      32
#define  API_SIM_EF_AD        33
#define  API_SIM_EF_PHASE     34
#define  API_SIM_EF_VGCS      35
#define  API_SIM_EF_VGCSS     36
#define  API_SIM_EF_VBS       37
#define  API_SIM_EF_VBSS      38
#define  API_SIM_EF_EMLPP     39
#define  API_SIM_EF_AAEM      40
#define  API_SIM_EF_ECC       41
#define  API_SIM_EF_CBMIR     42
#define  API_SIM_EF_NIA       43
#define  API_SIM_EF_KCGPRS    44
#define  API_SIM_EF_LOCIGPRS  45
#define  API_SIM_EF_SUME      46
#define  API_SIM_EF_PLMNWACT  47
#define  API_SIM_EF_OPLMNWACT 48
#define  API_SIM_EF_HPLMNACT  49
#define  API_SIM_EF_CPBCCH    50
#define  API_SIM_EF_INVSCAN   51
#define  API_SIM_EF_RPLMNAC   52
#define  API_SIM_EF_ICCID     53
#define  API_SIM_EF_ELP       54
#define  API_SIM_EF_IMG       55
#define  API_SIM_EF_MBDN      56

// add for 3G USIM
#define    API_USIM_EF_LI			57
#define    API_USIM_EF_ARR         		58
#define    API_USIM_EF_IMSI        		59
#define    API_USIM_EF_KEYS        		60
#define    API_USIM_EF_KEYS_PS          61
#define    API_USIM_EF_DCK		       62   
#define    API_USIM_EF_HPLMN      		63
#define    API_USIM_EF_CNL		       64    
#define    API_USIM_EF_ACM_MAX    	65
#define    API_USIM_EF_UST        		66
#define    API_USIM_EF_ACM        		67
#define    API_USIM_EF_FDN        		68
#define    API_USIM_EF_SMS        		69
#define    API_USIM_EF_GID1       		70
#define    API_USIM_EF_GID2       		71
#define    API_USIM_EF_MSISDN     	72
#define    API_USIM_EF_PUCT       		73
#define    API_USIM_EF_SMSP       		74
#define    API_USIM_EF_SMSS       		75
#define    API_USIM_EF_CBMI       		76
#define    API_USIM_EF_SPN        		77
#define    API_USIM_EF_SMSR       		78
#define    API_USIM_EF_CBMID      		79
#define    API_USIM_EF_SDN        		80
#define    API_USIM_EF_EXT2       		81
#define    API_USIM_EF_EXT3       		82
#define    API_USIM_EF_BDN        		83
#define    API_USIM_EF_EXT5       		84
#define    API_USIM_EF_CBMIR      		85
#define    API_USIM_EF_EXT4       		86
#define    API_USIM_EF_EST        		87
#define    API_USIM_EF_ACL        		88
#define    API_USIM_EF_CMI        		89
#define    API_USIM_EF_START_HFN  	90
#define    API_USIM_EF_THRESHOLD  	91
#define    API_USIM_EF_PLMNWACT      92   
#define    API_USIM_EF_OPLMNWACT    93    
#define    API_USIM_EF_HPLMNWACT    94    
#define    API_USIM_EF_EHPLMN    	       95
#define    API_USIM_EF_PS_LOCI    	96
#define    API_USIM_EF_ACC        		97
#define    API_USIM_EF_FPLMN       	98
#define    API_USIM_EF_LOCI        		99
#define    API_USIM_EF_ICI         		100
#define    API_USIM_EF_OCI         		101
#define    API_USIM_EF_ICT         		102
#define    API_USIM_EF_OCT        		103
#define    API_USIM_EF_AD			104     	       
#define    API_USIM_EF_EMLPP	      	105
#define    API_USIM_EF_AAEM		       106    
#define    API_USIM_EF_ECC		       107   
#define    API_USIM_EF_HIDDENKEY   	108
#define    API_USIM_EF_NETPAR      	109
#define    API_USIM_EF_PNN         		110
#define    API_USIM_EF_OPL         		111
#define    API_USIM_EF_MBDN       		112
#define    API_USIM_EF_EXT6        		113
#define    API_USIM_EF_MBI         		114
#define    API_USIM_EF_MWIS       		115
#define    API_USIM_EF_CFIS       		116
#define    API_USIM_EF_EXT7       		117
#define    API_USIM_EF_SPDI		       118    	
#define    API_USIM_EF_MMSN		       119   
#define    API_USIM_EF_EXT8		       120    
#define    API_USIM_EF_MMSICP	       121    
#define    API_USIM_EF_MMSUP	       122    
#define    API_USIM_EF_MMSUCP	       123    
#define    API_USIM_EF_NIA		       124    
#define    API_USIM_EF_VGCS		       125     
#define    API_USIM_EF_VGCSS	      	126
#define    API_USIM_EF_VBS		       127   
#define    API_USIM_EF_VBSS		       128    
#define    API_USIM_EF_VGCSCA	       129    
#define    API_USIM_EF_VBSCA	      	130
#define    API_USIM_EF_GBAP		       131   
#define    API_USIM_EF_MSK		       132    
#define    API_USIM_EF_MUK		       133    
#define    API_USIM_EF_GBANL	      	134
#define    API_USIM_EF_EHPLMNPI      	135 
#define    API_USIM_EF_LRPLMNSI        136 
#define    API_USIM_EF_NAFKCA	       137    
                             
//files under DF MEXE         
#define    API_USIM_EF_MEXE_ST    	138
#define    API_USIM_EF_ARPK       		139
#define    API_USIM_EF_TPRK       		140
#define    API_USIM_EF_ORPK                141
                                   
//files under DF GSM ACCESS            
#define    API_USIM_EF_KC         		142
#define    API_USIM_EF_KC_GPRS    	143
#define    API_USIM_EF_CPBCCH     	144
#define    API_USIM_EF_INVSCAN    	145
                           
// file under MF           
#define    API_USIM_EF_DIR      	       146                                   
#define    API_USIM_EF_ARR_MF		147	     

// phonebook under DF_TELECOM, USIM globle phonebook 
#define    API_USIM_EF_GB_PBR		148                  
#define    API_USIM_EF_GB_IAP		149                  
#define    API_USIM_EF_GB_IAP1		150                
#define    API_USIM_EF_GB_ADN		151                  
#define    API_USIM_EF_GB_ADN1		152                
#define    API_USIM_EF_GB_EXT1		153                
#define    API_USIM_EF_GB_PBC		154                  
#define    API_USIM_EF_GB_PBC1		155                
#define    API_USIM_EF_GB_GRP		156                  
#define    API_USIM_EF_GB_GRP1		157                
#define    API_USIM_EF_GB_AAS		158                  
#define    API_USIM_EF_GB_GAS		159                  
#define    API_USIM_EF_GB_ANRA		160                
#define    API_USIM_EF_GB_ANRB		161                
#define    API_USIM_EF_GB_ANRC		162                
#define    API_USIM_EF_GB_ANRA1	163                
#define    API_USIM_EF_GB_ANRB1		164                
#define    API_USIM_EF_GB_ANRC1		165                
#define    API_USIM_EF_GB_SNE		166                  
#define    API_USIM_EF_GB_SNE1		167                
#define    API_USIM_EF_GB_CCP1		168                
#define    API_USIM_EF_GB_UID		169                  
#define    API_USIM_EF_GB_UID1		170                
#define    API_USIM_EF_GB_PSC		171                  
#define    API_USIM_EF_GB_CC		172                
#define    API_USIM_EF_GB_PUID		173                
#define    API_USIM_EF_GB_EMAIL		174                
#define    API_USIM_EF_GB_EMAIL1	175	               
                                                     
// phonebook under ADF USIM, USIM local phonebook    
#define    API_USIM_EF_PBR			176                  
#define    API_USIM_EF_IAP			177                  
#define    API_USIM_EF_IAP1			178                  
#define    API_USIM_EF_ADN			179                  
#define    API_USIM_EF_ADN1			180                  
#define    API_USIM_EF_EXT1			181                  
#define    API_USIM_EF_PBC			182                  
#define    API_USIM_EF_PBC1			183                  
#define    API_USIM_EF_GRP			184                  
#define    API_USIM_EF_GRP1			185                  
#define    API_USIM_EF_AAS			186                  
#define    API_USIM_EF_GAS			187                  
#define    API_USIM_EF_ANRA			188                  
#define    API_USIM_EF_ANRB			189                  
#define    API_USIM_EF_ANRC			190                  
#define    API_USIM_EF_ANRA1		191                  
#define    API_USIM_EF_ANRB1		192                
#define    API_USIM_EF_ANRC1		193                
#define    API_USIM_EF_SNE			194                  
#define    API_USIM_EF_SNE1			195                  
#define    API_USIM_EF_CCP1			196                  
#define    API_USIM_EF_UID			197                  
#define    API_USIM_EF_UID1			198                  
#define    API_USIM_EF_PSC			199                  
#define    API_USIM_EF_CC			200                    
#define    API_USIM_EF_PUID			201                  
#define    API_USIM_EF_EMAIL   		202                
#define    API_USIM_EF_EMAIL1		203                  

// esim file
#define    API_SIM_EF_EID			204

// LTE usim file
#define    API_USIM_EF_SPNI			205
#define    API_USIM_EF_PNNI			206
#define    API_USIM_EF_NCPIP			207
#define    API_USIM_EF_EPSLOCI		208
#define    API_USIM_EF_EPSNSC		209
#define    API_USIM_EF_UFC			210
#define    API_USIM_EF_UICCIARI		211
#define    API_USIM_EF_NASCONFIG	212

#define    API_USIM_EF_LP			15    // the same file as API_SIM_EF_LP


/*
================================================================================
  Structure  : api_SimUpdateBinaryReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI is asking to update a binary file in the SIM
================================================================================
*/
typedef struct
{
 u8          Pad       ; // Room for Src Mbx
 u8          File      ; // See the defines above
 u16         Offset    ;
 u8          NbBytes   ;
 u8          Data[256] ;

} api_SimUpdateBinaryReq_t ;

/*
================================================================================
  Structure  : api_SimReadRecordReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to read a record of a cyclic or linear file
================================================================================
*/
typedef struct
{
 u8          Pad      ; // Room for Src Mbx
 u8          File     ; // See the defines above
 u8          RecordNb ;
 u8          Mode     ;
 u8          NbByte   ;

} api_SimReadRecordReq_t ;

// Possible values for Mode
#define API_SIM_NEXT_RECORD  2
#define API_SIM_PREV_RECORD  3
#define API_SIM_ABS_CUR_MODE 4

/*
================================================================================
  Structure  : api_SimUpdateRecordReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to update a record of a cyclic or linear file
================================================================================
*/
typedef struct
{
 u8          Pad       ; // Room for Src Mbx
 u8          File      ; // See the defines above
 u8          RecordNb  ;
 u8          Mode      ; // See the defines above
 u8          NbByte    ;
 u8          Data[256] ;

} api_SimUpdateRecordReq_t ;

/*
================================================================================
  Structure  : api_SimInvalidateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to invalidate a SIM file
================================================================================
*/
typedef struct
{
 u8          Pad       ; // Room for Src Mbx
 u8          File      ; // See the defines above

} api_SimInvalidateReq_t ;

/*
================================================================================
  Structure  : api_SimRehabilitateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to rehabilitate a SIM file
================================================================================
*/
typedef struct
{
 u8          Pad       ; // Room for Src Mbx
 u8          File      ; // See the defines above

} api_SimRehabilitateReq_t ;

/*
================================================================================
  Structure  : api_SimSeekReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to seek in a SIM file
================================================================================
*/
typedef struct
{
 u8          Pad           ; // Room for Src Mbx
 u8          TypeMode      ; // See the defines below,
                             // bit 4 : Type, bit 1-0 : Mode
 u8          File          ; // See the defines above
 u8          PatternNbByte ;
 u8          Patern[256]   ;

} api_SimSeekReq_t ;

/*
================================================================================
  Structure  : api_SimSeekAllReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to seek in a SIM file
================================================================================
*/
typedef struct
{
 u8          Pad           ; // Room for Src Mbx
 u8          TypeMode      ; // See the defines below,
                             // bit 4 : Type, bit 1-0 : Mode
 u8          File          ; // See the defines above
 u8          PatternNbByte ;
 u8          Patern[256]   ;

} api_SimSeekAllReq_t ;

// Possible values for TypeMode
#define API_SIM_TYPE_NO_OUT     ( 0 << 4 )
#define API_SIM_TYPE_REC_NB_OUT ( 1 << 4 )
#define API_SIM_MODE_BEG_FWD      0
#define API_SIM_MODE_END_RWD      1
#define API_SIM_MODE_NEXT_LOC_FWD 2
#define API_SIM_MODE_PREV_LOC_RWD 3

/*
================================================================================
  Structure  : api_SimIncreaseReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to Increase the current record by a certain value
================================================================================
*/
typedef struct
{
 u8          Pad           ; // Room for Src Mbx
 u8          Value[3]      ;
 u8          File          ; // See the defines above

} api_SimIncreaseReq_t ;

/*
================================================================================
  Structure  : api_SimVerifyCHVReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to verify a CHV
================================================================================
*/
typedef struct
{
 u8         Pad         ; // Room for Src Mbx
 u8         CHVNumber   ; // Can be CHV Nb 1 or CHV Nb 2
 u8         CHVValue[8] ;

} api_SimVerifyCHVReq_t ;

/*
================================================================================
  Structure  : api_SimChangeCHVReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to change a CHV
================================================================================
*/
typedef struct
{
 u8         Pad            ; // Room for Src Mbx
 u8         CHVNumber      ; // Can be CHV Nb 1 or CHV Nb 2
 u8         OldCHVValue[8] ;
 u8         NewCHVValue[8] ;

} api_SimChangeCHVReq_t ;

/*
================================================================================
  Structure  : api_SimDisableCHVReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to disable the CHV 1
================================================================================
*/
typedef struct
{
 u8         Pad            ; // Room for Src Mbx
 u8         CHVValue[8]    ; // Can only be done on CHV 1

} api_SimDisableCHVReq_t ;

/*
================================================================================
  Structure  : api_SimEnableCHVReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to enable the CHV 1
================================================================================
*/
typedef struct
{
 u8         Pad            ; // Room for Src Mbx
 u8         CHVValue[8]    ; // Can only be done on CHV 1

} api_SimEnableCHVReq_t ;

/*
================================================================================
  Structure  : api_SimUnblockCHVReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to unblock a CHV
================================================================================
*/
typedef struct
{
 u8          Pad                ; // Room for Src Mbx
 u8          CHVNumber          ; // Can be CHV Nb 1 or CHV Nb 2
 u8          UnblockCHVValue[8] ;
 u8          NewCHVValue[8]     ;

} api_SimUnblockCHVReq_t ;

/*
================================================================================
  Structure  : api_SimElemFileStatusReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to get the status of an elementary file
================================================================================
*/
typedef struct
{
 u8    Pad                ; // Room for Src Mbx
 u8    File;

} api_SimElemFileStatusReq_t ;

/*
================================================================================
  Structure  : api_SimDedicFileStatusReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to get the status of a dedicated file
================================================================================
*/
#define API_DF_GSM     0
#define API_DF_TELECOM 1
typedef struct
{
 u8    Pad                ; // Room for Src Mbx
 u8    File               ; // API_DF_GSM or API_DF_TELECOM

} api_SimDedicFileStatusReq_t ;

/*
================================================================================
  Structure  : api_SimTerminalProfileReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send the UE capabilities. Only used for STK
================================================================================
*/
typedef struct
{
 u8          Pad              ; // Room for Src Mbx
 u8          NbByte           ;
 u8          ProfileData[256] ;

} api_SimTerminalProfileReq_t ;

/*
================================================================================
  Structure  : api_SimTerminalEnvelopeReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send an envelope. Only used for STK
================================================================================
*/
typedef struct
{
 u8          Pad               ; // Room for Src Mbx
 u8          NbByte            ;
 u8          EnvelopeData[256] ;

} api_SimTerminalEnvelopeReq_t ;

/*
================================================================================
  Structure  : api_SimTerminalResponseReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send a terminal response. Only used for STK
================================================================================
*/
typedef struct
{
 u8          Pad                   ; // Room for Src Mbx
 u8          NbByte                ;
 u8          TerminalRespData[256] ;

} api_SimTerminalResponseReq_t ;

/*
================================================================================
  Structure  : api_SimSelectApplicationReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send select application
================================================================================
*/
typedef struct
{
 u8    Pad      ; 	// Room for Src Mbx
 u8    AID[16]; 	// application ID
 u8    AIDLength; 	// application ID length
} api_SimSelectApplicationReq_t ;


/*
================================================================================
  Structure  : api_SimSetUsimPbkReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send usim phonebook information;
================================================================================
*/
typedef struct
{
 u8    Pad      ; 		// Room for Src Mbx
 u8    DataLength; 	// USIM Phonebook data length
 u8    RecordNum;	// USIM Phonebook record number;
 u8    flag;			// USIM Phonebook type; global phk: 0; application phb: 1;
 u8    Data[256]; 		// USIM Phonebook data
} api_SimSetUsimPbkReq_t ;



/*
================================================================================
  Structure  : api_SimToolkitActivateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to activate the SIM toolkit
================================================================================
*/
typedef struct
{
 u8          Pad      ; // Room for Src Mbx
 bool        Activate ; // TRUE for activation and FALSE for deactivation

} api_SimToolkitActivateReq_t ;

/*
================================================================================
  Structure  : api_SimPollReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to activate or modify the SIM toolkit polling
================================================================================
*/
typedef struct
{
 u8          Pad ; // Room for Src Mbx
 u32         Duration; // Unit 0.1 second
} api_SimPollReq_t ;

/*
================================================================================
  Structure  : api_SimResetCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimResetReq
================================================================================
*/
typedef struct
{
  u8 ResetResult ;
  u8 Sw1;
  u8 Sw2;
  u8 Data[256]; // Status of the DF_TELECOM
  u8 UsimFlag;  // 0: sim; 1: Usim
} api_SimResetCnf_t ;

/*
================================================================================
  Structure  : api_SimRetryReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to retry the current SIM instruction
================================================================================
*/
typedef struct
{
  u8 Pad ; // Room for Src Mbx
} api_SimRetryReq_t ;

// Possible value for Result:
#define  API_SIM_NO_RESP         0
#define  API_SIM_BAD_RESP        1
#define  API_SIM_RESET_FIRST_ATT 2
#define  API_SIM_RESET_MULT_ATT  3
#define  API_SIM_RESET_OK_FILE_FAIL  4

/*
================================================================================
  Structure  : api_SimStatusErrorInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Periodical status indicates an error
================================================================================
*/
typedef struct
{
 u8         StatusError ; // Can take value 0 and 1 from Result 

} api_SimStatusErrorInd_t ;

/*
================================================================================
  Structure  : api_SimReadBinaryCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimReadBinaryReq
================================================================================
*/
typedef struct
{
 u8          Sw1       ;
 u8          Sw2       ;
 u8          File      ; // See the defines above. File which has been read
 u8          Data[256] ;

} api_SimReadBinaryCnf_t ;

/*
================================================================================
  Structure  : api_SimUpdateBinaryCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimUpdateBinaryReq
================================================================================
*/
typedef struct
{
 u8          Sw1      ;
 u8          Sw2      ;
 u8          File     ; // See the defines above. File which has been modified

} api_SimUpdateBinaryCnf_t ;

/*
================================================================================
  Structure  : api_SimReadRecordCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimReadRecordReq
================================================================================
*/
typedef struct
{
 u8          Sw1       ;
 u8          Sw2       ;
 u8          File      ; // See the defines above. File which has been read
 u8          Data[256] ;

} api_SimReadRecordCnf_t ;

/*
================================================================================
  Structure  : api_SimUpdateRecordCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimUpdateRecordReq
================================================================================
*/
typedef struct
{
 u8          Sw1        ;
 u8          Sw2        ;
 u8          File       ; // See the defines above. File which has been modified

} api_SimUpdateRecordCnf_t ;

/*
================================================================================
  Structure  : api_SimInvalidateCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimInvalidateReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;
 u8          File        ; // See the defines above. File which has been invalidated

} api_SimInvalidateCnf_t ;

/*
================================================================================
  Structure  : api_SimRehabilitateCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimRehabilitateReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;
 u8          File        ; // See the defines above. File which has been rehabilitated

} api_SimRehabilitateCnf_t ;

/*
================================================================================
  Structure  : api_SimSeekCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimSeekReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;
 u8          File        ; // See the defines above.
 u8          RecordNb    ;

} api_SimSeekCnf_t ;

/*
================================================================================
  Structure  : api_SimSeekAllCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimSeekReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;
 u8          File        ; // See the defines above.
 u8          Record[255]    ;
 u8          length;
} api_SimSeekAllCnf_t ;

/*
================================================================================
  Structure  : api_SimIncreaseCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimIncreaseReq
================================================================================
*/
typedef struct
{
 u8          Sw1                 ;
 u8          Sw2                 ;
 u8          File                ; // See the defines above.
 u8          IncRecAddedVal[259] ; // Increased Record followed by the 3 bytes
                                   // of added value
} api_SimIncreaseCnf_t ;

/*
================================================================================
  Structure  : api_SimVerifyCHVCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimVerifyCHVReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;

} api_SimVerifyCHVCnf_t ;

/*
================================================================================
  Structure  : api_SimChangeCHVCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimChangeCHVReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;

} api_SimChangeCHVCnf_t ;

/*
================================================================================
  Structure  : api_SimDisableCHVCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimDisableCHVReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ; 
 u8          Sw2         ;

} api_SimDisableCHVCnf_t ;

/*
================================================================================
  Structure  : api_SimEnableCHVCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimEnableCHVReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;

} api_SimEnableCHVCnf_t ;

/*
================================================================================
  Structure  : api_SimUnblockCHVCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimUnblockCHVReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;

} api_SimUnblockCHVCnf_t ;

/*
================================================================================
  Structure  : api_SimElemFileStatusCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimElemFileStatusReq
================================================================================
*/
typedef struct
{
 u8          Sw1       ;
 u8          Sw2       ;
 u8          File      ;  // See the defines above. File which status has been read
 u8          Data[256] ;  // See Appendix C of the API interface document

} api_SimElemFileStatusCnf_t ;

/*
================================================================================
  Structure  : api_SimDedicFileStatusCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimDedicFileStatusReq
================================================================================
*/
typedef struct
{
 u8          Sw1       ;
 u8          Sw2       ;
 u8          File      ;  // See the defines above. File which status has been read
 u8          Data[256] ;  // See Appendix C of the API interface document

} api_SimDedicFileStatusCnf_t ;

/*
================================================================================
  Structure  : api_SimTerminalProfileCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimTerminalProfileReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ;
 u8          Sw2         ;

} api_SimTerminalProfileCnf_t ;

/*
================================================================================
  Structure  : api_SimTerminalEnvelopeCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimTerminalEnvelopeReq
================================================================================
*/
typedef struct
{
 u8          Sw1               ;
 u8          Sw2               ;
 u8          NbByte            ; // Nb of byte of the response
 u8          ResponseData[256] ;

} api_SimTerminalEnvelopeCnf_t ;

/*
================================================================================
  Structure  : api_SimTerminalResponseCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimTerminalResponseReq
================================================================================
*/
typedef struct
{
 u8          Sw1         ; 
 u8          Sw2         ;

} api_SimTerminalResponseCnf_t ;


/*
================================================================================
  Structure  : api_SimSelectApplicationCnf_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Response to the SimSelectApplicationReq
================================================================================
*/
typedef struct
{
 u8          Sw1       ;
 u8          Sw2       ;
 u8          File      ; // See the defines above. File which has been read
 u8          Data[256] ;
} api_SimSelectApplicationCnf_t ;

/*
================================================================================
  Structure  : api_SimOpenInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : The Sim is Available for Protocol       
================================================================================
*/
typedef struct
{
    api_NwStoreListInd_t StoreList[STK_MAX_SIM];
    u8 SimStatus; // 0:GSIM; 1: USIM 
}api_SimOpenInd_t ;

#define    API_GSIM        0
#define    API_USIM        1
/*
================================================================================
  Structure  : api_SimFetchInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to send a fetch command to MMI. Only used for STK
================================================================================
*/
typedef struct
{
 u16          Length     ;
 u8          Data[260]   ;

} api_SimFetchInd_t ;

/********************************
** API-SMSPP INTERFACE 
*********************************/

/*
================================================================================
  Structure  : api_SmsPPSendReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send a SMS point-to-point
================================================================================
*/
typedef struct
{
 u8          Path        ;
 bool        Concat      ;
 u8          AddressLen  ;
 u8          Address[11] ;
 u8          DataLen     ;
 u8          Data[1]     ; // In fact, DataLen elts must be allocated for this 
                           // array

} api_SmsPPSendReq_t ;

// Possible values for Path
#define API_SMS_PATH_GSM  0
#define API_SMS_PATH_GPRS 1

/*
================================================================================
  Structure  : api_SmsPPSendMMAReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to send a notification to the network that the Ms has 
               memory available to receive one or more short messages
================================================================================
*/
typedef struct
{
 u8          Path    ; // possible values as defined in api_SmsPPSendReq_t

} api_SmsPPSendMMAReq_t ;

typedef struct
{
 u8          Path    ; // possible values as defined in api_SmsPPSendReq_t

} api_SmsPPAbortReq_t ;

/*
================================================================================
  Structure  : api_SmsPPAckReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to acknowledge a previously received SMS PP.
================================================================================
*/
typedef struct
{
 u8          Path    ; // possible values as defined in api_SmsPPSendReq_t
 u8          DataLen ;
 u8          Data[1] ; // In fact, DataLen elts must be allocated for this array

} api_SmsPPAckReq_t ;

/*
================================================================================
  Structure  : api_SmsPPErrorReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to indicate the network there was an error while receiving
               an SMS PP
================================================================================
*/
typedef struct
{
 u8          Path    ; // possible values as defined in api_SmsPPSendReq_t
 u8          CauseLen;
 u8          Cause[2]; 
 u8          DataLen ;
 u8          Data[1] ; // In fact, DataLen elts must be allocated for this array

} api_SmsPPErrorReq_t ;

/*
================================================================================
  Structure  : api_SmsPPAckInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that an RP-ACK was received from the 
               network
================================================================================
*/
typedef struct
{
 u8          DataLen ;
 u8          Data[1] ; // In fact, DataLen elts must be allocated for this array

} api_SmsPPAckInd_t  ;


/*
================================================================================
  Structure  : api_SmsPPErrorInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that either 
               - an RP-ERROR was received, or 
               - a CP-ERROR was received, or 
               - a local error was detected
================================================================================
*/
typedef struct
{
 u8          ErrorType ;
 u8          CauseLen;
 u8          Cause[2]; 
 u8          DataLen ;
 u8          Data[1] ; // In fact, DataLen elts must be allocated for this array

} api_SmsPPErrorInd_t ;

// Possible values for ErrorType
#define API_SMS_LOCAL_ERR    0
#define API_SMS_PEER_SMR_ERR 1
#define API_SMS_PEER_SMC_ERR 2

// Possible local error causes
#define API_SMS_MAX_RP_RETRANS 0
#define API_SMS_MAX_CP_RETRANS 1

/*
================================================================================
  Structure  : api_SmsPPReceiveInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that an SMS-PP has been received
================================================================================
*/
typedef struct
{
 u8          AddressLen  ;
 u8          Address[11] ;
 u8          DataLen     ;
 u8          Data[1]     ; // In fact, DataLen elts must be allocated for this 
                           // array
} api_SmsPPReceiveInd_t ;

/********************************
** API-SMSCB INTERFACE 
*********************************/
/*
================================================================================
  Structure  : api_SmsCBActivateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to activate SMS CB reception for a given list of SMS CB Ids
               (MID) and for a given list of Data Coding Scheme (DCS)
               Indicate the interface MMI wants to be used to provide data of
               the messages
================================================================================
*/
#define API_SMSCB_MID_LEN   20
#define API_SMSCB_DCS_LEN   17

typedef struct
{
 u8          ItfType;   // Kind of interface wished by MMI
 u8          MIDNb   ;  // 0 to 20, 0 means listen to every SMS CB Ids
 u16         MID[API_SMSCB_MID_LEN]  ;  // SMS CB Id to listen
 u8          DCSNb   ;  // 0 to 10, 0 means listen to every DCS
 u8          DCS[API_SMSCB_DCS_LEN]  ;  // DCS to listen

} api_SmsCBActivateReq_t ;

// Possible values for ItfType
#define API_SMSCB_ITF_MSG   0 // Provides complete SMSCB message to MMI
                              // (API_SMSCB_RECEIVE_IND)
#define API_SMSCB_ITF_PAGE  1 // Provides SMSCB page to MMI (API_SMSCB_PAGE_IND)

/*
================================================================================
  Structure  : api_SmsCBDeActivateReq_t
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to de-activate SMS CB reception
  Comment    : This message has no specific parameter
================================================================================
*/
typedef struct
{
 u8          Pad;

} api_SmsCBDeActivateReq_t ;

/*
================================================================================
  Structure  : api_SmsCBReceiveInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that an SMS-CB has been received
================================================================================
*/
typedef struct
{
  u16       MId;        // Message Id the SMS-CB belongs to
  u16       DataLen;
  u8        DCS;        // Data Coding Scheme applicable to the SMS-CB message
  u8        Data[1];    // In fact, DataLen elts must be allocated for this 
                        // array
} api_SmsCBReceiveInd_t ;

/*
================================================================================
  Structure  : api_SmsCBPageInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that an SMS-CB page has been received
================================================================================
*/
typedef struct
{
  u8        DataLen;    // Page size, in byte
  u8        Data[1];    // In fact, DataLen elts must be allocated for this 
                        // array
} api_SmsCBPageInd_t ;


/********************************
** API-CSD INTERFACE 
*********************************/

#define API_CSD_MBX              CDATA_MBX      // Mbx to which received CSD is 
                                                // forwarded
#define API_CSD_QUEUE            0              // Id of CSD transmission queue

#define API_CSD_RESERVED_LEN     ILHDR_SPECIFIC_SIZE
/*
================================================================================
  Structure  : api_CsdDataReq
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to ask to send data for CS data
================================================================================
*/
// ATTENTION : the nb of bytes allocated for an API_CSDATA_REQ message must be:
// API_CSDATA_REQ_SIZE(DataLen), where DataLen is the nb of radio bytes CSD-FAX
// wants to send in the API_CSDATA_REQ message ; 
// API_CSDATA_REQ_SIZE is defined below
/*
================================================================================
  Structure  : api_CsdDataInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate the reception of data for CS data
================================================================================
*/
typedef struct 
{
 u8         Reserved[API_CSD_RESERVED_LEN];
 u16        DataLen    ; // Nb of bytes of the Data field (including DataOffset)
 u8         DataOffset ; // Data will be found at UserData[DataOffset]
 u8         Pad        ;
 u8         Data[1]    ; 
} api_CsdDataReq_t,
  api_CsdDataInd_t,
  api_Csd_t ;
#define API_CSDATA_OFFSET             0
#define API_CSDATA_LEN( _DataLen )    (_DataLen) 
#define API_CSDATA_REQ_SIZE( _DataLen ) \
        (  sizeof(MsgHead_t) + OFFSETOF(api_CsdDataReq_t,Data) + _DataLen )



/*
================================================================================
  Structure  : api_CsdCrlpSetReq
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to configure RLP system parameters by user
================================================================================
*/
typedef struct 
{
  u8  Kim;          
  u8  Kmi;          
  u8  T1;           
  u8  N2;           
  u8  Vers;         
  u8  T4;           
  u8  T2;           
} api_RlpSysPar_t;

typedef struct 
{
  u8              BitmapPar;    
  api_RlpSysPar_t UsrPar;
} api_CsdCrlpSetReq_t;


  


#define API_RLP_USRPAR_KIM    ( 1 << 0 )
#define API_RLP_USRPAR_KMI    ( 1 << 1 )
#define API_RLP_USRPAR_T1     ( 1 << 2 )
#define API_RLP_USRPAR_N2     ( 1 << 3 )
#define API_RLP_USRPAR_VERS   ( 1 << 4 )
#define API_RLP_USRPAR_T4     ( 1 << 5 )
#define API_RLP_USRPAR_T2     ( 1 << 6 )  
#define API_RLP_NB_PAR        7

/*
================================================================================
  Structure  : api_CsdCrlpRdInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Indicate current RLP system parameters settings
================================================================================
*/
typedef api_RlpSysPar_t api_CsdCrlpRdInd_t;




/***************************
** API Debug mode interface.
****************************/
/*
================================================================================
  Structure  : api_DbgInfoCmd
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : Used to start / stop system information debug mode 
================================================================================
*/
typedef struct
{
 bool Enable ;       // TRUE to start debug mode, FALSE to stop it
} api_DbgInfoCmd_t ;

/*
================================================================================
  Structure  : api_DbgInfoInd
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API reports engineering mode info
================================================================================
*/
typedef struct
{
 u16 Id;              // Field identifier 
 u16 Value;           // Field value.
} api_DbgField_t;

// Possible values for Id 
#define API_DBG_BANDS     ( (API_RR_SPY  << 8) || API_RRSPY_BANDS      )
#define API_DBG_CIPON     ( (API_RR_SPY  << 8) || API_RRSPY_CIPON      )
#define API_DBG_HOPON     ( (API_RR_SPY  << 8) || API_RRSPY_HOPON      )
#define API_DBG_DTXON     ( (API_RR_SPY  << 8) || API_RRSPY_DTXON      )
#define API_DBG_T3212DUR  ( (API_MM_SPY  << 8) || API_MMSPY_T3212DUR   )
#define API_DBG_ATTACHED  ( (API_MM_SPY  << 8) || API_MMSPY_ATT        )
#define API_DBG_PDPCTXACT ( (API_MM_SPY  << 8) || API_MMSPY_PDPCTX     )
#define API_DBG_GPRSCIPON ( (API_LLC_SPY << 8) || API_LLCSPY_CIPH      )


typedef struct
{
 u8 NbFields;         // Number of fields that follow.
 u8 Pad [2];
 api_DbgField_t Field [1]; // Fields description.
} api_DbgInfoInd_t;

/*
================================================================================
  Structure  : api_CurrCellInfo_Req
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI configures the Stack to report infos on Cells
================================================================================
*/
typedef struct
{
bool  StartReport  ;          // 0 one shot, 1 periodic

} api_CurCellInfoReq_t,
  api_AdjCellInfoReq_t;

/*
================================================================================
  Structure  : api_CurrCellInfo_Ind
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API reports Current Cell Info Indication
================================================================================
*/
typedef struct
{
 u16    Arfcn          ;       // Abs Freq Number        
 u8     Rla            ;       // Averaged measure 
 u8     Lai [5]        ;       // MCC MNC LAC
 u8     Rac            ;       // RAC
 u8     CellId[2]      ;       // Cell Identifier
 u8     Bsic           ;       // Base Station Code
 bool   BcchDec        ;       // Bcch has been decoded
 u8     Nmo            ;       // Network Mode of Operation
 u8     MTxPower       ;       // Max Tx Power
 s8     C1             ;       // C1 Param see 05.08
 u8     Ber            ;       // Quality qunatity
 u8     Ta             ;       // Timing Adv.

 //added by leiming 2006.08.16(begin)
  
 u8     CCCHconf;  //ccch configuration
 u8     BsPaMfrms; // BS-PA-MFRMS
 u8     RxQualFull;//receive quality full
 u8     RxQualSub;//receive quality sub
 s16    C2;		//C2 value
 u8     CurrBand;	//current band used,possible value see below
 u8	   CurrChanType;//Current channel type ,possible value see blow,only valid in dedicated mode
 u32    CurrChanMode;//current channel mode ,only valid in decicated mode,possible value API_?
 u8     MaxRetrans;//max number of random access retransmission
 u8     BsAgBlkRes;//the block number kept for AGCH in common channel
 
 u8	   AC[2];        //AC[1]  bit0-7 indicate access class 0-7
 				//AC[0] bit0-1 indicate access class 8-9,AC[1] bit2 is spare bit
 				//AC[0] bit3-7 indicate access class 11-15
 				// if value of bit is 0:indicate the corresponding access class is allowed to access the cell
 				// if value of bit is 1:indicate the corresponding access class is not allowed to access the cell

 u8     RxLevAccMin;//receive level access minnum
 bool  EC;//whether emergency call is allowed in the current cell,
 		//value 0 indicate allowed,value 1 indicate not allowed
 		
 bool  SI7_8;//whether SI7 and 8 are broadcast in the current cell.value 0 indicate they are not broadcasted,
                  //value 1 indicate they are broadcasted.
                  
 bool  ATT; //whether IMSI attach is allowed in the current cell.
 		   //value 0 indicate MSs in the cell are not allowed to apply IMSI attach and  detach procedure.
 		   //value 1 indicate MSs in the cell shall apply IMSI attach and detach procedure
 //added by leiming 2006.08.16(end)

} api_CurCellInfoInd_t;


/*
================================================================================
  Structure  : api_SyncInfoReq_Req
--------------------------------------------------------------------------------
  Direction  : MMI -> API
  Scope      : MMI configures the Stack to report power list and sync info report
================================================================================
*/
typedef struct
{
    bool  StartReport  ;
}api_SyncInfoReq_t;

#define API_POWER_LIST_COUNT  50
typedef struct
{
    u16  Arfcn;
	u16    Rssi;
}api_PowerInfo_t;
/*
================================================================================
  Structure  : api_PowerListInd_Ind
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API reports power list
================================================================================
*/
typedef struct
{
   api_PowerInfo_t PowerList[API_POWER_LIST_COUNT];
   u8   Count;
}api_PowerListInd_t;

/*
================================================================================
  Structure  : api_SyncInfoInd_Ind
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API reports sync info
================================================================================
*/
typedef struct
{
   u16  Arfcn;
   u8    Band; // GSM: 0, DCS: 1, PCS: 2, 850: 3, 400: 4.
}api_SyncInfoInd_t;

 //added by leiming 2006.08.16(begin)

//possible value for CCCHconf
#define 		CCCH_1_CHANNEL_WITH_SDCCH 	0
#define		CCCH_1_CHANNEL				1
#define		CCCH_2_CHANNEL				2
#define		CCCH_3_CHANNEL				3
#define		CCCH_4_CHANNEL				4
#define		INVALID_CCCH_CHANNEL_NUM 	 0xFF

 //possible value for CurrBand
#define 		BAND_GSM900 					 9
#define		BAND_DCS1800 					 18
#define		BAND_PCS1900 					 19
#define		BAND_INVALID 					 0xFF

//possible value for CurrChan

enum{
CHANNEL_TYPE_SDCCH,
CHANNEL_TYPE_TCH_H0,
CHANNEL_TYPE_TCH_H1,
CHANNEL_TYPE_TCH_F,
INVALID_CHANNEL_TYPE = 0XFF 
};

//added by leiming 2006.08.16(end)



/*
================================================================================
  Structure  : api_AdjCellInfo_Ind
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : API reports on Adjacents Info Indication
================================================================================
*/
typedef struct
{
 u16    Arfcn          ;       // Abs Freq Number        
 //added by leiming 20080718(begin)
 u8 CellId[2]           ;
 //added by leiming 20080718(end)

 u8     Rla            ;       // Averaged measure 
 u8     Bsic           ;       // Base Station Code
 bool   BcchDec        ;       // Bcch has been decoded
 u8     MTxPower       ;       // Max Tx Power
 u8     Ram            ;       // RxLev Acc Min
 s8     C1             ;       // C1 Param see 05.08
 u8     Lai[5]      ;
} api_AdjCellElem_t;

typedef struct
{
 u8                    AdjCellNb      ;       // Number of Elems        

 //modified by leiming 20080721
 api_AdjCellElem_t     AdjCellElem [10];       // Content
 
} api_AdjCellInfoInd_t;


/*
================================================================================
  Structure  : api_FlowCtrInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMII
  Scope      : Flow control status
  Comment    : 
================================================================================
*/
typedef struct
{
 u8   status;   // 1: disable serial RX; 2:release serial RX
} api_FlowCtrInd_t ;


/*
================================================================================
  Structure  : api_CloseCnf_t
--------------------------------------------------------------------------------
  Direction  : MM -> API
  Scope      : SIM close status
  Comment    : 
================================================================================
*/
typedef struct
{
 u8   status;   
  u8 CnfFsm;   // dmx 20150706,  MMP also need send SIM_CLOSE_CNF
} api_SimCloseCnf_t ;

typedef api_SimCloseCnf_t mm_SimCloseCnf_t;

extern u32 api_CheckTestSim(u32 nSimID);
//================================================================================
#define API_NETWORK_STATUS_CS_SIM_KO      	(1<<0)
#define API_NETWORK_STATUS_CS_SERVICE_MASK  (3<<1)
#define API_NETWORK_STATUS_CS_NO     		(0)
#define API_NETWORK_STATUS_CS_FULL     		(1<<1)
#define API_NETWORK_STATUS_CS_EMC     		(2<<1)

#define API_NETWORK_STATUS_PS_SIM_KO      	(1<<3)
#define API_NETWORK_STATUS_PS_SERVICE_MASK  (1<<4)
#define API_NETWORK_STATUS_PS_KO      		(0<<4)
#define API_NETWORK_STATUS_PS_OK     		(1<<4)

#define API_NETWORK_STATUS_SIMCARD_NOT_EXIST     	(1<<5)

typedef enum api_Network_State
{
	// possible return value 
	API_PLMN_STATE_CSNO = 0,
	API_PLMN_STATE_CSNO_PSOK        = API_NETWORK_STATUS_PS_OK,
	API_PLMN_STATE_CSNO_PSSIMKO = API_NETWORK_STATUS_PS_SIM_KO,			

	API_PLMN_STATE_CSNO_CSSIMKO               = API_NETWORK_STATUS_CS_SIM_KO,
	API_PLMN_STATE_CSNO_CSSIMKO_PSOK          = API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_OK,
	API_PLMN_STATE_CSNO_CSSIMKO_PSSIMKO       = API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_SIM_KO,			
	API_PLMN_STATE_CSNO_CSSIMKO_PSSIMKO_NOSIM_CARD = API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_SIM_KO|API_NETWORK_STATUS_SIMCARD_NOT_EXIST,			

	API_PLMN_STATE_STATUS_CSOK                 = API_NETWORK_STATUS_CS_FULL,
	API_PLMN_STATE_STATUS_CSOK_PSOK            = API_NETWORK_STATUS_CS_FULL|API_NETWORK_STATUS_PS_OK,
	API_PLMN_STATE_STATUS_CSOK_PSSIMKO         = API_NETWORK_STATUS_CS_FULL|API_NETWORK_STATUS_PS_SIM_KO,

	API_PLMN_STATE_STATUS_CSEMC               = API_NETWORK_STATUS_CS_EMC,
	API_PLMN_STATE_STATUS_CSEMC_PSOK          = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_PS_OK,
	API_PLMN_STATE_STATUS_CSEMC_PSSIMKO       = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_PS_SIM_KO,

	API_PLMN_STATE_STATUS_CSEMC_CSSIMKO                = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_CS_SIM_KO,
	API_PLMN_STATE_STATUS_CSEMC_CSSIMKO_PSOK           = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_OK,
	API_PLMN_STATE_STATUS_CSEMC_CSSIMKO_PSSIMKO = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_SIM_KO,
	API_PLMN_STATE_STATUS_CSEMC_CSSIMKO_PSSIMKO_NOSIM_CARD = API_NETWORK_STATUS_CS_EMC|API_NETWORK_STATUS_CS_SIM_KO|API_NETWORK_STATUS_PS_SIM_KO|API_NETWORK_STATUS_SIMCARD_NOT_EXIST,

	API_PLMN_UNDEFINE = 0xff
 }api_Network_State_t ;
 extern api_Network_State_t api_GetNetworkState(u32 nSimID);
//================================================================================
//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
//adapter mmc
typedef struct 
{
    u8 plmnMode;
    u8 airMode;
    u8 onlyAttach;
} api_MmcStartReq_t ; 

typedef struct 
{
    u8 plmnMode;
    u8 PlmnId[3];
} api_MmcNwSelReq_t ; 

typedef api_NvParasChangeInd_t api_MmcNvChangeInd_t;
typedef api_StopReq_t           api_MmcStopReq_t;
typedef api_NwListReq_t         api_MmcNwListReq_t;
typedef api_NwListAbortReq_t    api_MmcNwListAbortReq_t;
typedef api_NwPsRegStatusInd_t  api_MmcNwPsRegInd_t;
typedef u8                      api_MmcStopCnf_t;
typedef api_NwListInd_t         api_MmcNwListInd_t;
typedef api_NwListAbortInd_t  api_MmcNwListAbortInd_t;
typedef api_NwErrcConnStatusInd_t api_MmcNwErrcConnStatusInd_t;
typedef api_SimStatusErrorInd_t api_MmcSimStatusErrorInd_t;
typedef api_NwkInfoInd_t        api_MmcNwkInfoInd_t;

//adapter gms
typedef api_SmsPPSendReq_t      api_CmcSmsppSendReq_t;
typedef api_SmsPPSendMMAReq_t   api_CmcSmsPPSendMMAReq_t;
typedef api_SmsPPAbortReq_t     api_CmcSmsPPAbortReq_t;
typedef api_SmsPPAckReq_t       api_CmcSmsPPAckReq_t;
typedef api_SmsPPErrorReq_t     api_CmcSmsPPErrorReq_t;
typedef api_SmsPPAckInd_t       api_CmcSmsPPAckInd_t;
typedef api_SmsPPErrorInd_t     api_CmcSmsPPErrorInd_t;
typedef api_SmsPPReceiveInd_t   api_CmcSmsPPReceiveInd_t;

//adapter esm
typedef api_PdpCtxActivateReq_t         api_CmcPdpCtxActivateReq_t;
typedef api_DefaultPdnCtxConfigReq_t    api_CmcDefaultPdnCtxConfigReq_t;
typedef api_PdpCtxModifyReq_t           api_CmcPdpCtxModifyReq_t;
typedef api_PdpCtxModifyAcc_t           api_CmcPdpCtxModifyAcc_t;
typedef api_PdpCtxDeactivateReq_t       api_CmcPdpCtxDeactivateReq_t;
typedef api_PdpCtxActivateCnf_t         api_CmcPdpCtxActivateCnf_t;
typedef api_PdpCtxSmIndexInd_t          api_CmcPdpCtxSmIndexInd_t;
typedef api_PdpCtxDeactivateCnf_t       api_CmcPdpCtxDeactivateCnf_t;
typedef api_PdpCtxActivateRej_t         api_CmcPdpCtxActivateRej_t;
typedef api_PdpCtxModifyRej_t           api_CmcPdpCtxModifyRej_t;
typedef api_PdpCtxActivateInd_t         api_CmcPdpCtxActivateInd_t;
typedef api_PdpCtxModifyCnf_t           api_CmcPdpCtxModifyCnf_t;
typedef api_PdpCtxModifyInd_t           api_CmcPdpCtxModifyInd_t;
typedef api_PdpCtxDeactivateInd_t       api_CmcPdpCtxDeactivateInd_t;
#ifdef __MINQOS__
typedef api_PdpSetMinQoSReq_t           api_CmcPdpSetMinQoSReq_t;
#endif
typedef api_EpsCtxAllocateReq_t           api_CmcEpsCtxAllocateReq_t;
typedef api_EpsCtxActivateRsp_t           api_CmcEpsCtxActivateRsp_t;
typedef api_EpsCtxActivateRej_t           api_CmcEpsCtxActivateRej_t;

#ifdef _MSC_VER //for test tool to generate xml parsing code
//API_MMC_NW_PS_REG_IND
typedef struct api_MmcNwPsRegInd_tag
{
    MsgHead_t Header;
    api_NwPsRegStatusInd_t body;
}h_api_MmcNwPsRegInd_t;

//API_MMC_STK_STOP_CNF
typedef struct api_MmcStkStopCnf_tag
{
    MsgHead_t Header;
}h_api_MmcStkStopCnf_t;

//API_MMC_NW_LIST_IND
typedef struct api_MmcNwListInd_tag
{
    MsgHead_t Header;
    api_MmcNwListInd_t body;
}h_api_MmcNwListInd_t;

//API_MMC_SIM_STATUSERROR_IND
typedef struct api_MmcSimStatusErrorInd_tag
{
    MsgHead_t Header;
    api_MmcSimStatusErrorInd_t body;
}h_api_MmcSimStatusErrorInd_t;

//API_MMC_NWKINFO_IND
typedef struct api_MmcNwkInfoInd_tag
{
    MsgHead_t Header;
    api_MmcNwkInfoInd_t body;
}h_api_MmcNwkInfoInd_t;

//API_CMC_PDP_CTXACTIVATE_CNF
typedef struct api_PdpCtxActivateCnf_tag
{
    MsgHead_t Header;
    api_PdpCtxActivateCnf_t body;
}h_api_PdpCtxActivateCnf_t;

//API_CMC_PDP_CTXACTIVATE_REJ
typedef struct api_PdpCtxActivateRej_tag
{
    MsgHead_t Header;
    api_PdpCtxActivateRej_t body;
}h_api_PdpCtxActivateRej_t;

//API_CMC_PDP_CTXMODIFY_CNF
typedef struct api_PdpCtxModifyCnf_tag
{
    MsgHead_t Header;
    api_PdpCtxModifyCnf_t body;
}h_api_PdpCtxModifyCnf_t;

//API_CMC_PDP_CTXMODIFY_REJ
typedef struct api_PdpCtxModifyRej_tag
{
    MsgHead_t Header;
    api_PdpCtxModifyRej_t body;
}h_api_PdpCtxModifyRej_t;

//API_CMC_PDP_CTXMODIFY_IND
typedef struct api_PdpCtxModifyInd_tag
{
    MsgHead_t Header;
    api_PdpCtxModifyInd_t body;
}h_api_PdpCtxModifyInd_t;

//API_CMC_PDP_CTXDEACTIVATE_CNF
typedef struct api_PdpCtxDeactivateCnf_tag
{
    MsgHead_t Header;
    api_PdpCtxDeactivateCnf_t   body;
}h_api_PdpCtxDeactivateCnf_t;

//API_CMC_PDP_CTXDEACTIVATE_IND
typedef struct api_PdpCtxDeactivateInd_tag
{
    MsgHead_t Header;
    api_PdpCtxDeactivateInd_t   body;
}h_api_PdpCtxDeactivateInd_t;

//API_CMC_PDP_CTXSMINDEX_IND
typedef struct api_PdpCtxSmIndexInd_tag
{
    MsgHead_t Header;
    api_PdpCtxSmIndexInd_t  body;
}h_api_PdpCtxSmIndexInd_t;

//API_CMC_EPS_CTXALLOCATION_CNF
typedef struct api_EpsCtxAllocateCnf_tag
{
    MsgHead_t Header;
    api_EpsCtxAllocateCnf_t body;
}h_api_EpsCtxAllocateCnf_t;

//API_CMC_EPS_CTXACTIVATE_IND;
typedef struct api_EpsCtxActivateInd_tag
{
    MsgHead_t Header;
    api_EpsCtxActivateInd_t body;
}h_api_EpsCtxActivateInd_t;

//API_CMC_EPS_CTXALLOCATION_ERR;
typedef struct api_EpsCtxAllocateRej_tag
{
    MsgHead_t Header;
    api_EpsCtxAllocateRej_t body;
}h_api_EpsCtxAllocateRej_t;

//API_CMC_SMSPP_ACK_IND
typedef struct api_SmsPPAckInd_tag
{
    MsgHead_t Header;
    api_SmsPPAckInd_t  body;
    u8 data[175];		//#define  GMS_MAX_TPDU_LEN  175
}h_api_SmsPPAckInd_t;

//API_CMC_SMSPP_ERROR_IND
typedef struct api_SmsPPErrorInd_tag
{
    MsgHead_t Header;
    api_SmsPPErrorInd_t body;
    u8 data[175];       //#define  GMS_MAX_TPDU_LEN  175
}h_api_SmsPPErrorInd_t;

//API_MMC_NW_LIST_ABORT_IND
typedef struct api_NwListAbortInd_tag
{
    MsgHead_t Header;    
    api_NwListAbortInd_t body;
}h_api_NwListAbortInd_t;

//API_MMC_ERRC_CONNSTATUS_IND
typedef struct api_NwErrcConnStatusInd_tag
{
    MsgHead_t Header;    
    api_NwErrcConnStatusInd_t body;
}h_api_NwErrcConnStatusInd_t;

//API_CMC_SMSPP_RECEIVE_IND
typedef struct api_SmsPPReceiveInd_tag
{
    MsgHead_t Header;
    api_SmsPPReceiveInd_t body;
    u8 data[175];       //#define  GMS_MAX_TPDU_LEN  175
}h_api_SmsPPReceiveInd_t;

#endif

#endif //LTE_NBIOT_SUPPORT 
#endif
