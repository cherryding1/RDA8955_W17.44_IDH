//================================================================================
//  File       : ITF_MM.H
//--------------------------------------------------------------------------------
//  Scope      : Interface Mesages provided by MM                       
//  History    :
//--------------------------------------------------------------------------------/
// 20131129 | DMX | add mm_PsDetachReq_t
//  Oct 04 05  |  OTH   | Add the mm_ReleaseInd_t struct            
//  Aug 06 03  |  OTH   | Creation            
//================================================================================


#ifndef __ITF_MM_H__
#define __ITF_MM_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "cmn_dec.h"

#include "itf_api.h"


/*
** Primitive ID definitions
** Circuit switch
*/
#define MM_ESTAB_REQ        ( HVY_MM + 1  )
#define MM_ESTAB_CNF        ( HVY_MM + 2  )
#define MM_ESTAB_IND        ( HVY_MM + 3  )
#define MM_ESTAB_ERR        ( HVY_MM + 4  )
#define MM_ABORT_REQ        ( HVY_MM + 5  )
#define MM_RELEASE_REQ      ( HVY_MM + 6  )
#define MM_RELEASE_IND      ( HVY_MM + 7  )
#define MM_REESTAB_REQ      ( HVY_MM + 8  )
#define MM_REESTAB_CNF      ( HVY_MM + 9  )
#define MM_REESTAB_ERR      ( HVY_MM + 10 )
#define MM_SERVREQ_ACK      ( HVY_MM + 11 )
#define MM_DATA_REQ         ( HVY_MM + 12 )
#define MM_DATA_IND         ( HVY_MM + 13 )
#define MM_NWKINFO_IND      ( HVY_MM + 14 )
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
#define MM_PAGINGD_BEGIN_IND      ( HVY_MM + 15 )
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
#define HVY_MM_GPRS_BASE ( HVY_MM + 0x20 )

#ifdef __GPRS__
/*
** Packet switch
*/
#define MMP_ATTACH_REQ      ( HVY_MM_GPRS_BASE + 1 )
#define MMP_DETACH_REQ      ( HVY_MM_GPRS_BASE + 2 )
#define MMP_DATA_REQ        ( HVY_MM_GPRS_BASE + 3 )
#define MMP_DATA_IND        ( HVY_MM_GPRS_BASE + 4 )
#define MMP_ABORT_IND       ( HVY_MM_GPRS_BASE + 5 )
#define MMP_PDPCTXSTATUS_IND  ( HVY_MM_GPRS_BASE + 6 )
#define MMP_SEQUENCE_IND      ( HVY_MM_GPRS_BASE + 7 )
#define MMP_SEQUENCE_RSP      ( HVY_MM_GPRS_BASE + 8 )
#define MMP_SMNONETRSP_IND              ( HVY_MM_GPRS_BASE + 9 )  // DMX add 20140625, PDP active req no respond,
#define MMP_DEL_SAVE_DATA_REQ              ( HVY_MM_GPRS_BASE + 10 )  // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ

#define MMP_RAUACC_PDP_STATUS_IND  ( HVY_MM_GPRS_BASE + 11 )  // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
#endif


#define MM_CC_PD  CMN_CC_PD
#define MM_SS_PD  CMN_SS_PD
#define MM_SMS_PD CMN_SMS_PD
#define MM_SM_PD  CMN_SM_PD

/*
** Primitive Content definitions
*/
typedef struct 
{
 u8 PDTI;
 u8 CallType;
 // These values respect the 24.008, 10.5.3.3
#define MM_REGULAR_CALL   1
#define MM_EMERGENCY_CALL 2
#define MM_SMS            4
#define MM_SS_ACTIVATION  8
} mm_EstabReq_t;

typedef struct 
{
 u8 NbReest;
} mm_ReestabReq_t;

//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
typedef struct 
{
 u8 PDTI;  // if PDTI=0xFF, Release everything
 u8 ReleaseCause;
} mm_ReleaseInd_t;

typedef struct 
{
 u8 PDTI;  // if PDTI=0xFF, Release everything
 u8 EstabErrCause; // establish error cause
} mm_EstabErrInd_t;

#define MM_CAUSE_NO 		0
// allow CC reset only for release_ind
#define MM_LOWLAYER_FAILURE 1
#define MM_OTHER_RELCAUSE   2
#define MM_COLLISION        3
// cause for ESTAB_ERROR
#define MM_ANOTHER_SIM_BUSY     4
#define MM_OTHER_ERRCAUSE       5

#define MM_CS_SIM_KO            0x40
#define MM_FORBIAN_LAI          0x80
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

/*
================================================================================
  Structure  : mm_PDTIInd_t
--------------------------------------------------------------------------------
  Direction  : MMC->CM
  Message    : MM_ESTAB_IND, MM_RELEASE_REQ, MM_ESTAB_ERR, MM_ESTAB_CNF,
               MM_SERVREQ_ACK
  Scope      : 
================================================================================
*/
typedef struct 
{
 u8 PDTI;
} mm_PDTIInd_t;

/*added start by yangyang for EDGE at 2007-04-04*/
#ifdef __EGPRS__
/*
================================================================================
Structure  : mm_StopReq_t
--------------------------------------------------------------------------------
Direction  : API -> MM
Scope      : Starts the protocol stack
================================================================================
*/
typedef api_StartReq_t mm_apiStartReq_t;
#endif
/*added end by yangyang for EDGE at 2007-04-04*/

/*
================================================================================
  Structure  : mm_StopReq_t
--------------------------------------------------------------------------------
  Direction  : API -> MM
  Scope      : Stops the protocol stack
================================================================================
*/
typedef api_StopReq_t mm_StopReq_t;

// [[add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
//================================================================================
//  Structure  : mm_PsAttachReq_t
//--------------------------------------------------------------------------------
//  Direction  : API -> MM
//  Scope      : 
//================================================================================
typedef 	api_NwPsAttachReq_t 			mm_PsAttachReq_t ;

//================================================================================
//  Structure  : mm_PsDetachReq_t
//--------------------------------------------------------------------------------
//  Direction  : API -> MM
//  Scope      : 
//================================================================================
typedef 	api_NwPsDetachReq_t 			mm_PsDetachReq_t ;
// ]]add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
/*
================================================================================
  Structure  : mm_NwkInfoInd_t
--------------------------------------------------------------------------------
  Direction  : MM -> API
  Scope      : Gives informations about the network
================================================================================
*/
typedef api_NwkInfoInd_t mm_NwkInfoInd_t;

#ifdef __GPRS__
/*
================================================================================
  Structure  : mmp_DataReq_t
--------------------------------------------------------------------------------
  Direction  : SM -> MMP or SMS -> MMP
  Message    : MMP_DATA_REQ
  Scope      : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Id;
} mmp_DataReq_t;


// Possible values for Id
// DO NOT CHANGE
// These values MUST MATCH the itf_llc.h Sapi value
#define MMP_SM_ID   1
#define MMP_SMS_ID  7


/*
================================================================================
  Structure  : mmp_DataInd_t
--------------------------------------------------------------------------------
  Direction  : MMP -> SMS or SM
  Message    : MMP_DATA_IND
  Scope      : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Pad;
} mmp_DataInd_t;
//================================================================================
typedef struct
{
	void* pMsg;
	u8  Pd;
}mmp_DelSavaDataReq_t;

/*
================================================================================
  Structure  : mmp_AbortInd_t
--------------------------------------------------------------------------------
  Direction  : MMP -> SM
  Message    : MMP_ABORT_IND
  Scope      :
================================================================================
*/
typedef struct
{
  u8  Cause;
} mmp_AbortInd_t;

/*
** Possible values for Cause
*/
  /*
      The cause value must be checked after discussion with the MMI for
      the interface between SM and MMI for the PDP deactivation due to MMP
  */
#define MMP_PDP_TO_REACTIVATE     36  // Regular PDP context deactivation from 24.008
#define MMP_PDP_NOT_TO_REACTIVATE 36  // Regular PDP context deactivation from 24.008


/*
================================================================================
  Structure  : mmp_PdpCtxStatusInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> MMP
  Message    : MMP_PDPCTXSTATUS_IND
  Scope      :
================================================================================
*/
typedef struct
{
  u8    Nsapi;    // Concerned NSAPI (5 to 15)
  bool  Status; // TRUE = Active / FALSE = Inactive
} mmp_PdpCtxStatusInd_t;


/*
================================================================================
  Structure  : mmp_SequenceInd_t
--------------------------------------------------------------------------------
  Direction  : MMP -> SNDCP
  Message    : MMP_SEQUENCE_IND
  Scope      : MMP provides SNDCP with the list of the received N-PDU number in
               case of inter SGSN routing area updating
================================================================================
*/
typedef struct
{
  u8  Nsapi;
  u8  NpduNum;
} mmp_NpduElmt_t;

typedef struct
{
  u8  MbxRsp;     // Mbx to send the response
  u8  NbItems;    // Nb elements in the list
  mmp_NpduElmt_t NpduList[11];
} mmp_SequenceInd_t;


/*
================================================================================
  Structure  : mmp_SequenceRsp_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> MMP
  Message    : MMP_SEQUENCE_RSP
  Scope      : SNDCP provides MMP with the list of received N-PDU number
               (response to the MMP_SEQUENCE_IND)
================================================================================
*/
typedef struct
{
  u8  NbItems;    // Nb elements in the list
  mmp_NpduElmt_t NpduList[11];
} mmp_SequenceRsp_t;
#endif // __GPRS__
//================================================================================
// DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
#define MMP_RAUACC_PDP_STATUE_ACTIVE 1
#define MMP_RAUACC_PDP_STATUE_INACTIVE 0
typedef struct  {
u8 StatusNum;
u8 Status[16];
} mmp_RauAccPdpStatusInd_t;


#endif
