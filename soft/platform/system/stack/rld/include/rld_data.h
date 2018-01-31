//------------------------------------------------------------------------------
//  $Log: rld_data.h,v $
//  Revision 1.4  2006/04/18 15:10:48  oth
//  Modification to have now 1 queue for the InSequence received block with BSN < Vq
//
//  Revision 1.3  2006/01/10 16:58:49  oth
//  Correct the RLD_MOD_SNS macro in case of first parameter in format (a+1)
//
//  Revision 1.2  2005/12/29 17:39:12  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : rld_data.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for rld  

  History    :
--------------------------------------------------------------------------------
  Mar 04 04  |  OTH   | Creation            
================================================================================
*/

#ifndef __RLD_DATA_H__
#define __RLD_DATA_H__

#include "rld_msg.h"
#include "itf_msg.h"

/*
** Block size depending on the CS
*/
#define RLD_DATASIZE_CS1 20
#define RLD_DATASIZE_CS2 30
#define RLD_DATASIZE_CS3 36
#define RLD_DATASIZE_CS4 50

#ifdef __RLD_C__
#define DefExtern 

#ifdef __EGPRS__
u8 rldDataSizeCS[] = { RLD_DATASIZE_CS1, RLD_DATASIZE_CS2, RLD_DATASIZE_CS3, RLD_DATASIZE_CS4 ,23,29,38,45,57,75,57,69,75} ;
#else
u8 rldDataSizeCS[4] = { RLD_DATASIZE_CS1, RLD_DATASIZE_CS2,
                        RLD_DATASIZE_CS3, RLD_DATASIZE_CS4 };

#endif
#else
#define DefExtern extern
extern u8 rldDataSizeCS[4];
#endif

typedef struct
{
 /*
 ** General global data        
 */
 #ifndef STK_SINGLE_SIM
 u32    SimIndex; // new added for dual SIM support
 #endif
 Msg_t       * RecMsg  ;   // Received message
 Msg_t       * OutMsg  ;   // LLC Message to send

 InEvtDsc_t  * IEvtDsc ;   // Incoming Event
 OutEvtDsc_t * OEvtDsc ;   // Outgoing Event

 u8            Flags   ;
 u8            RlcMode ;   // 0: Ack, 1: Unack
 u8            CS      ;
 u8            PduQ    ;
 u8            InSeqQ  ;   
 u8            LowBsnQ ;   // min of Lowest BSN of the blocks in PduQ and Vq
 u16           Ws      ;   // Window Size (will be used in EDGE)
 u16           Sns     ;   // Sequence Number Space (128 GPRS and 2048 EDGE)
 u32           Vr      ;   // BSN + 1 (next in-sequence BSN we are waiting)
 u32           Vq      ;   // Lowest BSN not yet received
 u32           TLLI    ;   // TLLI of the current TBF
 u8*           Vn      ;
 u8            TestMode;  //0xff: test mode if off; 0: test mode A; 1:test mode B
 #ifdef __EGPRS__
 u8            MCS_last;/*for egprs*/
 u8            EG_MODE;/*0 GPRS, 1 EGPRS*/
 u16          PBSN;/*for Egprs*/
 /*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
 u8            LoopBack_Mode;
 /*#Added by PENGQIANG 2007.3.19 for testmode end.*/
 u8            SplitPduQ;
 #endif
}rld_Data_t;

#ifndef STK_SINGLE_SIM
DefExtern rld_Data_t  rld_Data[STK_MAX_SIM]  ;
#else
DefExtern rld_Data_t  rld_Data  ;
#endif

DefExtern rld_Data_t *pRLDCtx;
/*
** Possible values for the flags
*/
#define RLD_FLG_KEEP_MSG  ( 1 << 0 )
#define RLD_FLG_CONNECTED ( 1 << 1 )
#define RLD_FLG_FBI       ( 1 << 2 )
#define RLD_FLG_FAI       ( 1 << 7 ) /* Do not change this value */

/*
** Possible values for the RlcMode
*/
#define RLD_ACK_MODE   0
#define RLD_UNACK_MODE 1

/*
** Possible values for the Loopback mode
*/
#ifdef __EGPRS__
/*#Added by PENGQIANG 2007.3.30 for testmode begin.*/
#define RLD_LOOPBACK_OFF  0
#define RLD_LOOPBACK_ON    1
 /*#Added by PENGQIANG 2007.3.30 for testmode begin.*/
#endif
/*
** Possible values for the CS
*/
#define RLD_CS1      0
#define RLD_CS2      1
#define RLD_CS3      2
#define RLD_CS4      3

#define RLD_LOWBSN_INVALID 0xFF

/*
** SNS values
*/
#define RLD_SNS_GPRS 128
#define RLD_SNS_EDGE 2048

/*
** Additionnal flags for mac_DlDataInd_t structure
** The shift value has to be changed if some value are added in the Bitfield
** of the interface.
*/
#define RLD_IF_END_SDU     ( 1 << 2 )
#define RLD_IF_NULL_BLOCK  ( 1 << 3 )
#define RLD_IF_PARTIAL_HDL ( 1 << 4 )

#define RLD_MOD_SNS(_val)  ((_val) & pRLDCtx->Sns)

//the difference between v1 and v2 model step
#define GET_DIFF(_V1,_V2, _Step) 	(((s16)(_V1-_V2)>=0)?  (_V1- _V2): (_V1-_V2+_Step))

/*changed by laixuefeng 2008-1-9, if the Pdu is more than 128,that is to say BSN  run a circle, we should count 
    the circle number to get the correct lenth.*/
    
/*#define RLD_LEN_COMPUTE( _BSN, _CurBSN )                \
( _BSN >= _CurBSN ) ? ((_BSN - _CurBSN + 1) * 50) :     \
  ((_BSN + 128 - _CurBSN + 1) * 50)
*/
#define RLD_LEN_COMPUTE( _BSN, _CurBSN,_cycle ) \
( _BSN >= _CurBSN ) ? ((_BSN - _CurBSN + 1+(_cycle)*(pRLDCtx->Sns+1)) * 50) :     \
  ((_BSN - _CurBSN + 1+((_cycle)+1)*(pRLDCtx->Sns+1)) * 50)
/*changed by laixuefeng 2008-1-9, ended */

/*
** Decoding macros
*/
#define RLD_BSN_OFF 2
#define RLD_GET_E( _Msg, _Off )  ( _Msg->RLD_ILD[_Off] & 0x01 )
#define RLD_GET_M( _Msg, _Off )  ((_Msg->RLD_ILD[_Off] >> 1) & 0x01)
#define RLD_GET_LI( _Msg, _Off ) ( _Msg->RLD_ILD[_Off] >> 2)
#define RLD_GET_BSN( _Msg )      (_Msg->RLD_ILD[RLD_BSN_OFF] >> 1)

#ifdef __EGPRS__
#define FPB_INC     1
#define GPRS_MODE			0
#define EGPRS_MODE			1
#define GET_LITTLE(_val1, _val2)  (_val1<_val2)? _val1 : _val2
#define GET_DIFF(_V1,_V2, _Step) 	(((s16)(_V1-_V2)>=0)?  (_V1- _V2): (_V1-_V2+_Step))

#define EGPRS_RLD_LEN_COMPUTE( _BSN, _CurBSN,_cycle ) \
( _BSN >= _CurBSN ) ? ((_BSN - _CurBSN + 1+(_cycle)*(pRLDCtx->Sns+1)) * 74) :     \
  ((_BSN - _CurBSN + 1+((_cycle)+1)*(pRLDCtx->Sns+1)) * 74)
  
#define GET_FBI(data) (( pRLDCtx->EG_MODE==EGPRS_MODE)? (data[0]&0x02) :(data[1]&0x01) )

/*#Modified by PENGQIANG 2007.9.26 for support both big endian and small endian begin.*/
/*support big endian*/
#ifdef __BIG_ENDIAN__
#define GET_DOWNLINK_BSN1(data)  ( ((data[1]&0xC0)<<3)|(data[2]<<1)|(data[3]&1))
#define GET_DOWNLINK_BSN2(data)  (((data[3]&0xFE)<<2) |(data[4]&7))
#else
#define GET_DOWNLINK_BSN1(data)  ( ((data[3]&1)<<10)|(data[2]<<2)|((data[1]&0xc0)>>6))
#define GET_DOWNLINK_BSN2(data)  (((data[4]&0x7)<<7) |((data[3]&0xfe)>>1))
#endif
/*#Modified by PENGQIANG 2007.9.26 for support both big endian and small endian end.*/

#define GET_A_PDU(Pdu,val,Len,Src,str_Pos)  \
 {	\
	Pdu = sxr_Malloc( sizeof( str_PDU_REC )+ Len );\
	Pdu->BSN=val; \
	Pdu->len=Len; \
	Pdu->BitField=0;\
	memcpy(Pdu->Data,&Src[str_Pos],Pdu->len);\
  }
typedef struct 
{
  u8 SPB;
  u8 Mcs;
  u16 BSN;
  u8 len;
  u8 Data[38];
}SPLIT_TMP_DATA;
//DefExtern SPLIT_TMP_DATA split_tmp_data;

typedef struct
{
   u8 len;
   u8 valid;
   u8 tmpData[74];
}PDU_REMAIN;
DefExtern PDU_REMAIN Pdu_remain;

typedef struct
{
   u8 BitField;
   u8 len;
   u16 BSN;
   u8 Data[1];
}str_PDU_REC;

enum SPB_TYPE
{
     NO_RETRANSMISSION = 0,
     RESERVE = 1,
     RET_FIRST_PART = 2,
     RET_SECOND_PART = 3
};

#endif
#undef DefExtern

#endif
