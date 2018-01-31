//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_data.h,v 1.5 2006/03/27 21:20:39 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : rlu_data.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for RLC-Uplink

  History    :
--------------------------------------------------------------------------------
  Jun 30 06  |  MCE   | Added TbfParsValid
  Jun 28 06  |  MCE   | Removed PurgeReqOn
  Jun 22 06  |  MCE   | Added PurgeReqOn
  Jun 17 06  |  MCE   | Added UserDataSuspQ  
  Jun 17 06  |  MCE   | Suspended renamed TxSusp, Added GprsSusp
  May 03 06  |  MCE   | Moved Cause from rlu_Flow_t to rlu_Data_t 
  Mar 30 06  |  MCE   | Added TotalLlcBytes, TotalLlcPdus and UlCongestion
  Mar 28 06  |  MCE   | Added UsfGran
  Feb 11 06  |  MCE   | Added RLU_PDU_TO_DISCARD    
  Nov 21 05  |  MCE   | Added PfcNwkSupport  
  Apr 22 05  |  MCE   | Addition of PurgeOn
  Sep 09 04  |  MCE   | Creation of LlcPduQ 
  Feb 05 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_DATA_H__
#define __RLU_DATA_H__

// system includes
#include "fsm.h"

// RLC includes
#include "rlc_defs.h"

// RLU_includes
#include "rlu_defs.h"
#include "rlu_msg.h"










typedef struct 
{
 u8              RlcMode      ; 
 u8              Prio         ; 
 u8              Class        ; 
 u8              Pfi          ; 
 u8              PType        ; 
 u8              SduQ         ; 
 #ifdef __EGPRS__
 u32              NbSdus       ;  //modified by fengwei 20071022
 #else
 u16              NbSdus       ; 
 #endif
 u32             NbBytes      ; 
 #ifdef __EGPRS__
 u32              AddedSdus    ; //modified by fengwei 20071022
 #else
 u16              AddedSdus    ; 
 #endif
 u32             AddedBytes   ; 
                                
 
} rlu_Flow_t ;


/*
================================================================================
  Structure  : rlu_Sdu_t
--------------------------------
  Scope      : structure used to save an RLC_SDU ( = an LLC PDU )
  Comment    : 
================================================================================
*/
typedef struct 
{
  Msg_t      *Msg     ;// pointer to the SDU as received from LLC
  u8          CpBytes ;// Nb bytes to copy in the next PDU built from this SDU
  u8          Status  ;// Ack status of the SDU. When ACKED, it can be Freeed 
  bool        Done    ;
  u8          AckNb   ;// Nb of PDus that have parts of this SDU and are waiting 
                       // to be acked
  u8          TxPduNb ;// Nb of PDUs that have parts of this SDU and are 
                       // waiting to be transmitted on the air
  u8          NbPdu   ;
  u16        *Offset  ;
  rlu_Flow_t *Flow    ;
} rlu_Sdu_t ;

// rlu_Sdu_t.Status possible values
#define RLU_SDU_WAIT  0    // SDU is waiting to be processed 
#define RLU_SDU_TX    1    // SDU is currently being processed
#define RLU_SDU_WACK  2    // SDU has been completely transmitted to MAC and 
                           // is waiting for all its PDUs to be acked
#define RLU_SDU_ACKED 3    // All PDUs of this SDUs have been Acked  

/*
================================================================================
  Structure  : rlu_Pdu_t         
--------------------------------
  Scope      : gives all the information about a transmitted PDU
================================================================================
*/
typedef struct 
{
  Msg_t      *Blk       ; // pointer to radio block
  rlu_Sdu_t  *FirstSdu  ; // pointer to first SDU whose bytes are in the PDU
  rlu_Sdu_t  *LastSdu   ; // pointer to last SDU whose bytes are in the PDU
  u32         Bsn       ;
  #ifdef __EGPRS__
  u32         TxFn      ;
  #else
  u16         TxFn      ; // FN where the block was transmitted (AckMode only)
  #endif
  
  u8          State     ; // Acknowledgment state
  #ifdef __EGPRS__
  u8          Mcs    ;//added by laixuefeng for egprs
  u8          Mcs_last;//added by laixuefeng for egprs to store last transmit Mcs
  u8          Ps ;// added by laixuefeng for egprs  
  u8         Ps_last;/*added by laisuefeng 2007-10-24, for when a Pdu that is purged by RLC from L1 queue is retransmitted */
  				/* the Ps of the Pdu should be the same as its last put in the L1 queue*/
   u8		TI;//for egprs added by laixuefeng
   u8       PI;//for egprs added by laixuefeng
   u8       CV;// for egprs added by laixuefeng
   #endif
#ifdef __RLU_DOUBLE_REPEAT__
  bool        DiscReq   ; 
#endif
} rlu_Pdu_t;

// possible values for TxFn
#define RLU_NO_FN       0xffff

// possible values for State
#define RLU_PDU_NO_ACKSTATUS  0 
#define RLU_PDU_TX_PENDING    1 
#define RLU_PDU_RP_PENDING    2 
#define RLU_PDU_ACK_PENDING   3 
#define RLU_PDU_ACKED         4 
#define RLU_PDU_NACKED        5 
#define RLU_PDU_TO_DISCARD    6 

/*
================================================================================
  Structure  : rlu_CurPdu_t         
--------------------------------
  Scope      : gives all the information about the PDU currently being prepared
================================================================================
*/
typedef struct 
{
  rlu_Pdu_t * Pdu         ; // pointer to the PDU being prepared; 0 if none
  u8          NetDataLen  ; // Nb of bytes in Data field, without spare bits
  u8          NbFreeBytes ; // Nb of bytes still available for SDU and LI bytes
  u8          NbLiBytes   ; // Nb of LI bytes to be inserted in the PDU
  bool        TI          ; // TRUE if TLLI should be put in the PDU
  bool        PI          ; // TRUE if PFI should be put in the PDU
} rlu_CurPdu_t ;
#define RLU_USR_BYTES  

/*
================================================================================
  Structure  : rlu_Data_t
--------------------------------
  Scope      : RLU context
================================================================================
*/
typedef struct 
{
 /*
 ** General global data        
 */
 #ifndef STK_SINGLE_SIM
 u32	           SimIndex;//Sim Index for dual SIM support
#endif
 u32           * RecSig         ; // Received signal
 Msg_t         * RecMsg         ; // Received message
 Msg_t         * OutMsg         ; // Message to send
 InEvtDsc_t    * IEvtDsc        ; // Incoming Event
 OutEvtDsc_t   * OEvtDsc        ; // Outgoing Event
 u32             TReadyValue    ; // TREADY duration (OS ticks)
 bool            TReadyActive   ; // TRUE if TREADY is activated
 bool            TReadyOn       ; // TRUE if TREADY is running 
 bool            PfcNwkSupport  ; 
 bool            GprsSusp       ; 
 bool            TPurgeOn       ; 
 u8              LlcPduQ        ; // Shared with LLC;LLC puts Txdata in this Q
 u8              UserDataSuspQ  ; 
 u8              FlowQ          ; 

 


 bool            UlCongestOn    ; 
 u32             TotalLlcPdus   ; 
 u32             TotalLlcBytes  ; 
  
 /*
 ** ARQ context
 */
 u8              PduQ            ; // Queue of PDUs waiting for an ACK
 u32             Vs              ; 
 u32             Va              ;
 
 /*
 ** Transmission context
 */

 bool            FaiRcvd         ;

 bool            TbfParsValid    ;
 u8              TbfCause        ;
 u8              RlcMode         ;// RLC mode as defined in itf_rlu.h 
 bool            Realloc         ;
 u8              Tlli[4]         ;
 u32             TBC             ;// Total Block Count on the TBF
 u32             FillBsn         ;// Bsn of next PDU to transmit on TxFillReq
 u32             RepeatBsn       ;// Bsn of next PDU to transmit on RepeatReq
 rlu_CurPdu_t    CurPdu          ;// RLC PDU currently prepared; 
 rlu_Sdu_t      *CurSdu          ;// RLC SDU currently processed;
                                  
                                  
 u32             NbSduBytes      ;// Total nb of SDU data we want to tx on TBF
 u16             NbSdus          ;// Nb of SDUs we want to transmit on the TBF
 u16             BlockGranted    ;
 u8              UAckRepNb       ;
 bool            T3182On         ;// TRUE if T3182 is running
 bool            CloseEnded      ;
 bool            TxSusp          ;// TRUE if Rlu transmission are suspended
 bool            EarlyUlRel      ;
 u8              PurgeOn         ;
 bool            PurgeAll        ;
 u8              SduQ            ;
 u8              ContRes         ;// Contention res underway = RLU_CR_ON
 u8              BsCvMax         ;
 u8              Cv              ;
 u8              Cs              ;// Coding Scheme
 u8              TlliBlockCs     ;// RRP_TLLIBLK_CS1 or RRP_TLLIBLK_REG
 u8              Nts             ;// Nb of TS allocated for the Uplink TBF
 u8              UsfGran         ;
 
 s8              N3102           ;// set by CellInd.N3102
 u8              PanMax          ;// set by CellInd.N3102
 u8              PanDec          ;// set by CellInd.N3102
 u8              PanInc          ;// set by CellInd.N3102
 rlu_Flow_t     *CurFlow         ; 

 


 u8              TestMode        ; 
 u8 		     TestMode_UpLink_SetUp;
 u16             TestSduSize     ; 
 u16             NbReqTstSdu     ; 
 u16             NbBuiltTstSdu   ; 

 bool            KeepMsg         ; // TRUE if RecMsg must not be deleted 
 #ifdef __EGPRS__
 u8               EG_Mode ;//for EGPRS 0:GPRS; 1:EGPRS, laixuefeng add
 u16 		    WS;//for EGPRS,window size  laixuefeng add
 u8		    Resegment; //for EGPRS;whether retransmitted RLC data blocks ,laixuefeng add
 u8           Pre_emptive;
  u8          NeedfillLI0;//added by laixuefeng, for EGPRS, for secgment
  u16 		 Sns;//added by laixuefeng, for EGPRS;
  #endif
 
 bool ExtendedTBF; //support for extended uplink mode
 
} rlu_Data_t;

// possible values for ContRes
#define RLU_CR_OFF               0
#define RLU_CR_ON                1

#ifdef __EGPRS__
typedef struct
{
u16 Bsn1;
u16 Bsn2;
u8   BsnValid;
	
}t_Send_Block;
#define FIRST_SEND		0
#define SEND_BEFORE      1

#define GPRS_MODE			0
#define EGPRS_MODE			1

/*modified 070117
#define PS1     0
#define PS2    1
#define PS3    2
*/
#define ONE_PDU    	1
#define TWO_PDU		2

/*del by fengwei 070117
#define PAD  1
#define NO_PAD 0
*/

/* this is for big edian*/
/*#define SET_UPLINK_BSN1(data,BSN){\
 data[2]=BSN&0x3F;\
 data[1]=(BSN&0x07C0)>>3;\
}

#define SET_UPLINK_BSN2(data,BSN) {\
	data[3]=BSN;\
	data[2]|=(BSN&0x0300)>>2;\
}
*/
/*changed by laixuefeng2007-9-26,change big edian to little edian; begin*/
#define SET_UPLINK_BSN1(data,BSN){\
 data[1]=((BSN&0x001F)<<3);\
 data[2]=(BSN&0x07E0)>>5;\
}

#define SET_UPLINK_BSN2(data,BSN) {\
	data[2]|=(BSN&0x0003)<<6;\
	data[3]=(BSN&0x03FC)>>2;\
}

#define SET_UPLINK_HEADER1(data,BSN1,BSN2,CPS,RSB,PI){\
	SET_UPLINK_BSN1(data,BSN1);\
	SET_UPLINK_BSN2(data, BSN2);\
	data[4]=((PI&1)<<6)|((RSB&1)<<5)|(CPS&0x1F);\
}
#define SET_UPLINK_HEADER2(data,BSN,CPS,RSB,PI){\
	SET_UPLINK_BSN1(data,BSN);\
	data[2]|=((CPS&0x03)<<6);\
	data[3]=((PI&1)<<2)|((RSB&1)<<1)|((CPS&0x04)>>2);\
}
#define SET_UPLINK_HEADER3(data,BSN,CPS,SPB,RSB,PI) {\
	SET_UPLINK_BSN1(data,BSN);\
	data[2] |=((CPS&0x03)<<6);\
	data[3]=((PI&1)<<5)|((RSB&1)<<4)|((SPB&0x03)<<2)|((CPS&0x0C)>>2);\
}
/*changed by laixuefeng2007-9-26,change big edian to little edian; end*/
	
#endif
// possible values for N3102
#define RLU_N3102_NS             127


#define RLU_TST_OFF              0xFF

#ifdef __RLU_C__
#define DefExtern 
#else
#define DefExtern extern
#endif

/*
** RLU Finite State Machines
*/
#ifndef STK_SINGLE_SIM
extern Fsm_t      rlu_Fsm[]  ;//for dual sim, there are two set of FSM
#else
extern Fsm_t      rlu_Fsm  ;
#endif

#ifndef STK_SINGLE_SIM
DefExtern rlu_Data_t  rlu_Data[STK_MAX_SIM]      ;//for dual sim support, two set fo rlu data needed
#else
DefExtern rlu_Data_t  rlu_Data;
#endif

DefExtern rlu_Data_t  *pRLUCtx;//new added for dual sim support
#undef DefExtern 

#endif
