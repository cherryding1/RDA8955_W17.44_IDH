//================================================================================
//  File       : mm_def.h
//--------------------------------------------------------------------------------
//  Scope      : Header file for all the definition for MM
//  History    :
//--------------------------------------------------------------------------------
// 20131129 | DMX | Add GPRS detach type for combo detach
//  Aug 27 03  |  OTH   | Creation            
//================================================================================

#ifndef __MM_DEF_H__
#define __MM_DEF_H__

#include "itf_rr.h"

#include "mm_trc.h"

#define MM_MSG_MAX_SIZE 25
#define MM_MMC_PD       0x05

// Identity Type
#define MM_IDTYPE_MASK 0x07
#define MM_ID_IMSI     1
#define MM_ID_IMEI     2 
#define MM_ID_IMEISV   3
#define MM_ID_TMSI     4

// Identity Size
#define MM_IMSI_SIZE      9
#define MM_IMEISV_SIZE    9
#define MM_IMEI_SIZE      8

// LAI, RAI and PLMN
#define MM_PLMN_SIZE    3
#define MM_LAI_SIZE     5
#define MM_RAC_SIZE     1
#define MM_RAI_SIZE     MM_LAI_SIZE + MM_RAC_SIZE

// Network service station mask, add for support EGPRS
#define MM_NETSVC_MASK (RR_EMERGENCY_ONLY | RR_FULL_SERVICE)//0x3
// GPRS or EDGE service station mask, add for support EGPRS
#ifdef __EGPRS__
#define MM_GPRS_EDGE_SVC_MASK  (RR_EGPRS_OFFERED |RR_GPRS_OFFERED)
#else
#define MM_GPRS_EDGE_SVC_MASK  (RR_GPRS_OFFERED)
#endif

// TMSI and P-TMSI
//#define MM_TMSI_INVALID   0xFFFFFFFF
#define MM_TMSI_LENGTH      4
#define MM_TMSI_ID_LENGTH   5
#define MM_PTMSISIGN_LENGTH 3

// LocUp status
// defines compliants with 11.11
#define MM_UPDATED          0
#define MM_NOT_UPDATED      1
#define MM_PLMN_NOT_ALLOWED 2 // Not used
#define MM_LA_NOT_ALLOWED   3

// file sizes from the SIM
#define MM_LOCI_FILE_SIZE     11
#define MM_IMSI_FILE_SIZE      9
#define MM_LOCIGPRS_FILE_SIZE 14

// GSM & GPRS Authentication parameters size
#define MM_RAND_SIZE      16
#define MM_SRES_SIZE      4
#define MM_SRES_EXT_SIZE  12
#define MM_KC_SIZE        8
#define MM_KC_FILE_SIZE   9
#define MM_AUTN_SIZE    16
#define MM_IK_SIZE    16
#define MM_CK_SIZE    16
#define MM_KEYS_FILE_SIZE 33

// CKSN
#define MM_CKSN_INVALID 0x07
#define MM_CKSN_MASK    0x07
#define MM_CKSN_OFFSET  8

#define MM_USIM_KEY_SIZE  33
#define MM_USIM_KEY_PS_SIZE  33

/*
** USIM AUTN parameter
*/
#define MM_AUTN_SIZE      16


#define MM_SIM_RD_OK            0x00  
#define MM_SIM_RD_EF_NOT_FOUND  0x01  
#define MM_SIM_RD_EF_ERROR      0xFF  


// Location Update Reject Cause, CM Service Reject Cause and GMM Cause
// Only the used values are defined here
#define MM_IMSI_UNKNOWN_HLR                   0x02
#define MM_ILLEGAL_MS                         0x03
#define MM_IMSI_UNKNOWN_VLR                   0x04
#define MM_IMEI_REFUSED                       0x05
#define MM_ILLEGAL_ME                         0x06
#define MM_GPRS_SERV_NOT_ALLOWED              0x07  // GPRS
#define MM_GPRS_AND_NONGPRS_SERV_NOT_ALLOWED  0x08  // GPRS
#define MM_ID_CANNOT_BE_DERIVED               0x09  // GPRS
#define MM_IMPLICIT_DETACH                    0x0A  // GPRS
#define MM_FORBIDDEN_PLMN                     0x0B
#define MM_FORBIDDEN_LOCAREA                  0x0C
#define MM_FORBIDDEN_ROAMING                  0x0D
#define MM_GPRS_SERV_NOT_ALLOWED_IN_PLMN      0x0E  // GPRS
#define MM_NO_SUIT_CELL                       0x0F
#define MM_MSC_NOT_REACHABLE                  0x10  // GPRS
#define MM_NETWORK_FAILURE                    0x11
#define MM_CONGESTION                         0x16
#define MM_RETRY_ENTRY_NEW_CELL_FIRST         0x30  // All cause between 0x30 and 0x3F are
#define MM_RETRY_ENTRY_NEW_CELL_LAST          0x3F  // considered RETRY UPON ENTRY INTO A NEW CELL
#define MM_CONDITIONAL_IE_ERR                 0x64

#define MM_AUTH_MAC_FAILURE   0x14
#define MM_AUTH_SYNC_FAILURE  0x15
#define MM_AUTH_GSM_UNACCEPTABLE  0x17

// Reject Cause
#define MM_SEMANTICAL_INCORRECT            0x5F   //95
#define MM_INVALID_MANDA_INFO                 0x60  // 96
#define MM_MT_UNKNOWN                         0x61  // 97
#define MM_MT_UNCOMPATIBLE                    0x62  // 98
#define MM_IE_UNKNOWN                              0x63 // 99
#define MM_CONDITIONNAL_IE_ERROR              0x64  // 100
#define MM_PROTOCAL_ERROR                      0x6F //111



//==============================================
//   Circuit Switch Specific
//==============================================

// Message Type
#define MMC_MT_STATUS 0x31

// Channel Needed in Paging Request
// As to be 0,1,2... used as index in an array
#define MMC_PAGRSP_ANYCH 0
#define MMC_PAGRSP_SDCCH 1
#define MMC_PAGRSP_TCH   2

// Timer durations definitions
#define MMC_T3210_DURATION  ( 20 SECONDS )
#define MMC_T3211_DURATION  ( 15 SECONDS )
#define MMC_T3213_DURATION  (  4 SECONDS )
#define MMC_T3220_DURATION  (  5 SECONDS )
#define MMC_T3230_DURATION  ( 15 SECONDS )
#define MMC_T3240_DURATION  ( 10 SECONDS )
#define MMC_DETACH_DURATION ( 10 SECONDS ) /* Arbitrary value (has to be */
                                           /* between 5 and 20 seconds)  */

// Classmark 2
#define MM_CLASSMARK2_SIZE 3


// Shortcuts
#define MM_IMSG_IL pMMCtx->IMsg->B.IL

//==============================================
//   Packet Switch Specific
//==============================================

// RAU status
#define MMP_RAU_STATUS_SIZE             1
#define MMP_RAU_STATUS_OFFSET           13


#define MMP_ATTACH_REQ_MAX_SIZE         88
#define MMP_ID_RESP_MAX_SIZE            12
#define MMP_AUTH_AND_CIPH_RSP_MAX_SIZE  33//19
#define MMP_RAU_REQ_MAX_SIZE            92
#define MMP_RAU_CMPT_SIZE               21
#define MMP_DETACH_REQ_MAX_SIZE         15

#define MMP_ATTACH_CMPT_SIZE           2
#define MMP_ATTACH_CMPT_MT          0x03

#define MMP_DETACH_ACCEPT_SIZE           2
#define MMP_DETACH_ACCEPT_MT        0x06

#define MMP_PTMSI_REALLOC_CMPT_SIZE    2
#define MMP_PTMSI_REALLOC_CMPT_MT   0x11

#define MMP_GMM_STATUS_SIZE              3
#define MMP_GMM_STATUS_MT             0x20


//GPRS Attach Type
#define MMP_GPRS_ATT_TYPE               1
#define MMP_GPRS_ATT_IMSI_ATTACHED_TYPE 2
#define MMP_COMBINED_ATT_TYPE           3

// GPRS Attach Result (10.5.5.1)
#define MMP_ATT_RES_GPRS_ONLY   1   // GPRS only
#define MMP_ATT_RES_COMB        3   // Combined

// Update Result (10.5.5.17)
#define MMP_RAU_RES_GPRS_ONLY   0   // GPRS only
#define MMP_RAU_RES_COMB        1   // Combined

// Update type (10.5.5.18)
#define MMP_NORMAL_RA_UPDATE        0
#define MMP_COMB_RA_UPDATE          1
#define MMP_COMB_RA_UPDATE_IMSI_ATT 2
#define MMP_PERIODIC_UPDATING       3

/*
** TLLI Type
*/
#define MMP_RANDOM_TLLI  0
#define MMP_FOREIGN_TLLI 1
#define MMP_LOCAL_TLLI   2
#define MMP_NOT_ASSIGNED_TLLI 3


/*
** Network Mode of Operation (04.60 sec 12.24)
*/
#define MMP_NMO_I     RR_NMO1
#define MMP_NMO_II    RR_NMO2
#define MMP_NMO_III   RR_NMO3

/*
** Force to standby
*/
#define MMP_FORCE_TO_STDBY_IND  1


/*
** IMEISV requested
*/
#define MMP_IMEISV_REQUESTED    1

/*
** Type of Detach (MS to NW)
*/
#define MMP_GPRS_DETACH_TYP     1
#define MMP_IMSI_DETACH_TYP     2
#define MMP_COMB_DETACH_TYP     3

#define MMP_DETACH_POWER_OFF    ( 1 << 3 )

// Constants to process detach
// mod dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
#define MMP_DET_ORIGIN_MSK      0x0f
// mod dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
#define MMP_DET_POWEROFF        0   // Detach for power off
#define MMP_DET_SIM_REMOVE      1   // Detach for SIM removal
#define MMP_DET_STOP            2   // Detach for stack stop
#define MMP_DET_ALL             3   // Detach for ApiDetach
#define MMP_DET_PS              4   // Detach for ApiPsDetach
#define MMP_DET_WAIT_DETACC_CNF 5   // Wait conf. of DET_ACC sent
// add dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
#define MMP_DET_CS				7 
#define MMP_DET_UNKNOWN         0x08

#define MMP_COMB_DET_IND        ( 1 << 4 )
//]] add dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
/*
** Type of Detach (NW to MS)
*/
#define MMP_NW_RE_ATT_REQ     1
#define MMP_NW_RE_ATT_NOT_REQ 2
#define MMP_NW_IMSI_DETACH    3


/*
** GPRS Timer attributes
*/
#define MMP_T_DEACTIVATED   0x07



/*
** Timer durations definitions & default values
*/
#define MMP_T3310_DURATION ( 15 SECONDS )
#define MMP_T3311_DURATION ( 15 SECONDS )
#define MMP_T3316_DURATION ( 30 SECONDS )
#define MMP_T3318_DURATION ( 20 SECONDS )
#define MMP_T3320_DURATION ( 15 SECONDS )
#define MMP_T3321_DURATION ( 15 SECONDS )
#define MMP_T3330_DURATION ( 15 SECONDS )


  // READY timer default value is 44 seconds
#define MMP_T3314_DEF_UNIT    0         // unit of 2 seconds
#define MMP_T3314_DEF_VALUE   (44 / 2)  // Value
    
#define MMP_T3312_DEF_DURATION ( 54 MINUTES )
#define MMP_T3302_DEF_DURATION ( 12 MINUTES )

#ifdef __EGPRS__
 #define MMP_MAX_ACCTYPE 14 //added by yangyang for EDGE at 2007-03-30
#endif

#endif // __MM_DEF_H__
