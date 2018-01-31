////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_def.c
// 
// DESCRIPTION:
//   General defines for CC
// 
// REVISION HISTORY:
//  NAME | DATE     | REMAKS
// DMX | 20081024 | Add dual Sim support
// DMX | 2006-00-0 | Add Timer time check
// DMX | 2006-11-30 | Add out macro max_size check.
// DMX | 2006-09-26 | Add Timer expriod value chenck in sap_StackCfg.
////////////////////////////////////////////////////////////////////////////////


#ifndef __CC_DEF_H__
#define __CC_DEF_H__

//#include "sap_cust.h"

#define CC_MSG_MAX_SIZE CMN_L3_MAX_LENGTH

#define CC_INVALID_TI       0xFF

#define CC_SETUPMT_MT       0x05
#define CC_EMERG_SETUPMT_MT 0x0E
#define CC_CONNECTACK_MT    0x0F
#define CC_HOLD_MT          0x18
#define CC_HOLDACK_MT       0x19
#define CC_RTRV_MT          0x1C
#define CC_RTRVACK_MT       0x1D
#define CC_RELCMPLT_MT      0x2A
#define CC_STOPDTMF_MT      0x31
#define CC_STOPDTMFACK_MT   0x32
#define CC_STATUSENQUIRY_MT 0x34
#define CC_NOTIFY_MT        0x3E

//
// Timers duration
#define CC_T303_DURATION  ( 30 SECONDS )
#define CC_T305_DURATION  ( 5 SECONDS )
#define CC_T308_DURATION  ( 1 SECONDS )
#define CC_T310_DURATION  ( 30 SECONDS )
#define CC_T313_DURATION  ( 30 SECONDS )
#define CC_T323_DURATION  ( 30 SECONDS )
#define CC_T336_DURATION  ( 10 SECONDS )
#define CC_T337_DURATION  ( 10 SECONDS )
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
#define CC_TRETRY_DURATION  ( 11 SECONDS )
#define CC_WAIT_LU_DURATION ( 30 SECONDS )
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

//
// Trace levels definition (5 first levels used for generic)
#define CC_ENDEC_TRC  (_CC | TLEVEL(8) )
#define CC_MMITF_TRC  (_CC | TLEVEL(10))
#define CC_APIITF_TRC (_CC | TLEVEL(11))
#define CC_TIMER_TRC  (_CC | TLEVEL(12))
#define CC_PROT_TRC   (_CC | TLEVEL(13))
#define CC_TI_TRC     (_CC | TLEVEL(14))
#define CC_CRSS_TRC   (_CC | TLEVEL(15))

#define CC_RRITF_TRC  (_CC | TLEVEL(9) )
#define CC_DUAL_TRC  (_CC | TLEVEL(9) )



// output macro after the encoding
//  add max_size check by dingmx 20061130

#define CC_CHECK_ENCODE( _Msg )                                               \
	if (( Len < 0 )|((Len/8) > CC_MSG_MAX_SIZE ))                         \
      {                                                                       \
      sxs_Raise( _CC|TSMAP(0x1)|TABORT, "%s encoding error %d\n", #_Msg,Len );\
      }                                                                       \
    else                                                                      \
      {                                                                       \
      gp_ccCurData->OMsg->B.IL.DataLen = (Len >> 3) + gp_ccCurData->OMsg->B.IL.DataOffset;  \
      SXS_TRACE( CC_ENDEC_TRC|TSMAP(0x1), "%s encoded propely :\n", #_Msg );\
      cc_SendMsg( MM_MBX );                                                   \
      }
// TI_Flag and TI_Value extraction macros + TI Release macro
//
#define CC_GET_TI_FLAG( _TI )  (_TI & 0x08) >> 3
#define CC_GET_TI_VAL( _TI )   (_TI & 0x07)
#define CC_RELEASE_TI( _TI )   gp_ccCurData->TIBitmap &= ~( 1 << _TI )

// Timer macros


#if !(STK_MAX_SIM==1)

#define CC_START_TIMER( _Duration, _Expiration )                        \
  sxs_StartTimer( _Duration, CMN_SET_SIM_ID(_Expiration,gp_ccCurData->SimIndex), (void*)((u32)gp_ccCurData->CurTI),   \
                  FALSE, CC_MBX );

#define CC_STOP_TIMER( _Expiration )                                    \
  sxs_StopTimer( CMN_SET_SIM_ID(_Expiration,gp_ccCurData->SimIndex), (void*)((u32)gp_ccCurData->CurTI), CC_MBX );

#define CC_EXIST_TIMER( _Expiration )                                    \
  sxs_ExistTimer( CMN_SET_SIM_ID(_Expiration,gp_ccCurData->SimIndex) )

#else
#define CC_START_TIMER( _Duration, _Expiration )                        \
  sxs_StartTimer( _Duration, _Expiration, (void*)((u32)gp_ccCurData->CurTI),   \
                  FALSE, CC_MBX );

#define CC_STOP_TIMER( _Expiration )                                    \
  sxs_StopTimer( _Expiration, (void*)((u32)gp_ccCurData->CurTI), CC_MBX );

#define CC_EXIST_TIMER( _Expiration )                                    \
	  sxs_ExistTimer( _Expiration )

#endif

// changed by dingmx

//#define CC_GET_TMPTY_DURATION sap_StackCfg.Tmpty
//#define CC_GET_TECT_DURATION  sap_StackCfg.Tect
//#define CC_GET_TS10_DURATION  sap_StackDynCfg.TS10

#define CC_GET_TMPTY_DURATION ((gp_ccCurData->sap_StackCfg->Tmpty)?gp_ccCurData->sap_StackCfg->Tmpty:10)
#define CC_GET_TECT_DURATION  ((gp_ccCurData->sap_StackCfg->Tect)? gp_ccCurData->sap_StackCfg->Tect:10)

#define CC_GET_TS10_DURATION   200
//#define CC_GET_TS10_DURATION  ((gp_ccCurData->psap_StackCfg->TS10>200)?gp_ccCurData->psap_StackCfg->TS10:200)

// Decoding macros
#define CC_PROGDESC_MASK 0x7F

// Cause defines

#define CC_GSM_PLMN_COD_STD 3
#define CC_USER_LOCATION    0

#define CC_USER_BUSY           17
#define CC_RESP_STAT_ENQ       30
#define CC_BEARERCAP_NOT_AUTHO 57
#define CC_MAX_ACM             68
#define CC_INVAL_TI            81
#define CC_INVAL_MANDA_INFO    96
#define CC_MT_UNKNOWN          97
#define CC_MTINCOMP_STATE      98
#define CC_CONDIE_ERROR        100
#define CC_INCOMP_STATE        101
#define CC_RECOV_TMR_EXP       102
#define CC_PROTOCOL_ERROR_UNSP 111

// Facility defines

// Component tag
#define CC_INVOKE_COMP 0xA1

// Operation code
#define CC_NOTIFY_SS   0x10

// Congestion Level

#define CC_RCVR_READY     0x00
#define CC_RCVR_NOT_READY 0x0F

#endif // __CC_DEF_H__
