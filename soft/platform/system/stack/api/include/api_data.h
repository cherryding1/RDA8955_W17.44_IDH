//================================================================================
//  File       : api_data.h
//--------------------------------------------------------------------------------
//  Scope      : API Context
//  History    :
//--------------------------------------------------------------------------------
//  May 12 06  |  FCH   | Added __GPRS__ flag for GSM only compilation
//  Mar 31 06  |  MCE   | Addec UlCongStatus
//  Mar 10 04  |  PCA   | Creation
//================================================================================

#ifndef __API_DATA_H__
#define __API_DATA_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

// Interface includes
#include "api_msg.h"


#ifdef __API_C__
#define DefExtern
#else
#define DefExtern extern
#endif


/*
================================================================================
  Data       : api_Ctxt_t
--------------------------------------------------------------------------------

  Scope      : Global API context

================================================================================
*/
#define API_MSG_MAX_SIZE 20
#define RAD_OFFS_API     10
#ifdef LTE_NBIOT_SUPPORT
#define PLMN_MODE_AUTO                 0x00
#define PLMN_MODE_MANUAL               0x01
#define  GSM_SUPPORT_BITMAP            0x01
#define  NBIOT_SUPPORT_BITMAP          0x08
#define RAT_INVALID                   0x00
#define GSM_ACTIVED                   0x01
#define NBIOT_ACTIVED                 0x02
#define CARD_SIM                       0x00
#define CARD_USIM                      0x01
#define CARD_INVALID                  0xFF
#endif
typedef struct
{
	Msg_t       * RecMsg       ;   // Received message
	Msg_t       * OutMsg       ;   // Message to send

	Fsm_t       * Fsm          ;   // Current selected Fsm
	InEvtDsc_t  * IEvtDsc      ;   // Incoming Event
	OutEvtDsc_t * OEvtDsc      ;   // Outgoing Event
#if !(STK_MAX_SIM==1)
	u32           SimId        ;
#endif
	bool          KeepMsg      ;   // TRUE if RecMsg must not be deleted
	u8            Flags        ;   // API flags
	u8            UlCongStatus ;   // Bit map of Uplink Congestion Status

	// [[add dmx 20140714
	bool         bWaitingMMSimCloseCnf; // api received SIM_CLOSE_IND and send to MM, waiting for SIM_CLOSE_CNF
	bool		 bWaitingMMPSimCloseCnf;  //  +  dmx 20150706,  MMP also need send SIM_CLOSE_CNF

	Msg_t       *pSavedSimOpenInd;     //  save the SIM_OPEN_IND, will send after received SIM_CLOSE_CNF

	// add dmx, 20141114, when API is waiting for SIM_CLOSE_CNF from MM, not send the new stk_start_req
	// this is for the case: the stk stoped, and then the API received SIM_CLOSE_IND, STK_START_REQ,SIM_OPEN_IND
	// for API wait SIM_CLOSE_CNF from MM, RR will received the STK_START_REQ, SIM_CLOSE_IND,SIM_OPEN_IND
	Msg_t       *pSavedStkStartReq;     
	// ]]add dmx 20140714,

	bool          bPowerOffWhenStop;   	// dmx support SIM_CLSOE 20150812
    #ifdef LTE_NBIOT_SUPPORT
    u8 supportRat; /*Bitmap0:GSM;Bitmap1:nb-iot;Bitmap2:catm. 0:no support, 1:support*/
    u8 currentRat;
    u8 simType;
    u8 plmnMode; /*0:auto; 1:manual*/
    Msg_t *pSaveSimOpenIndMsg; // Received message
    #endif
} api_Ctxt_t ;

/*
** Defines for the API flags
*/
#define API_FLG_MMC_STOPPED STK_CNFFSM_MMC
#define API_FLG_MMP_STOPPED STK_CNFFSM_MMP
#define API_FLG_RR_STOPPED STK_CNFFSM_RR

#ifdef __GPRS__
#define API_FLG_STOP_MSK_MM    ( API_FLG_MMC_STOPPED | API_FLG_MMP_STOPPED )
#else
#define API_FLG_STOP_MSK_MM    ( API_FLG_MMC_STOPPED )
#endif
#define API_FLG_STOP_MSK_STK    ( API_FLG_STOP_MSK_MM | API_FLG_RR_STOPPED)

/*
** Bit definition for UlCongStatus
*/
#define API_NO_ULCONG     0
#define API_RLU_ULCONG   (1<<0)
#define API_RLU_NOULCONG (1<<1)
#define API_MEM_ALERT    (1<<2)

#if !(STK_MAX_SIM==1)
DefExtern api_Ctxt_t apiData[STK_MAX_SIM];
DefExtern api_Ctxt_t *pAPIData;
#else
DefExtern api_Ctxt_t apiData;
DefExtern api_Ctxt_t *pAPIData;
#endif

#undef DefExtern
#endif // __API_DATA_H__
