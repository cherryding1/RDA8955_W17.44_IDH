//------------------------------------------------------------------------------
//  $Log: mac_data.h,v $
//  Revision 1.5  2006/05/22 14:36:19  oth
//  Update the copyright
//
//  Revision 1.4  2006/04/03 13:23:48  oth
//  Correction in the handling of the contention resolution
//
//  Revision 1.3  2006/02/27 10:37:17  oth
//  Use the new TFI only at expiration of the starting time
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

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : mac_data.h
--------------------------------------------------------------------------------

  Scope      : General context for the GPRS MAC

  History    :
--------------------------------------------------------------------------------
  Oct 13 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __MAC_DATA_H__
#define __MAC_DATA_H__

// System includes
#include "sxs_type.h"

// Interface includes

// EnDec includes

// MAC internal includes


/*
================================================================================
  Data       : mac_Data_t
--------------------------------------------------------------------------------

  Scope      : Global MAC context

================================================================================
*/
typedef struct
{
 u8  PollingStatus;             /* Used to answer to the polling */
 u8  Flags;
 // Do not change the position of the retry bit
#define MAC_RETRY_BIT       ( 1 << 0 )
#define MAC_COUNTDOWN_OK    ( 1 << 1 )
#define MAC_CONTRES_RUNNING ( 1 << 2 )
#define MAC_CONTRES_ONE_PH  ( 1 << 3 )
#define MAC_CONTRES_TWO_PH  ( 1 << 4 )
#define MAC_CONTRES_MASK       0x1C
#define MAC_TLLI_CHANGING   ( 1 << 5 )
#define MAC_RLU_SUSPENDED   ( 1 << 6 )
#define MAC_ULRLS_IN_PROG   ( 1 << 7 )
 u8  N3104;
 u8  NbUlAllocTs;
 u8  NbDlAllocTs;
 u8  RlcMode;                   /* See defines in itf RR */
 u8  RrState;
#define MAC_RR_IDLE_MODE   0
#define MAC_RR_PACKET_MODE 1
 u8  TlliIdx;                   /* Current index for the TLLI */
 u8  TlliIdxBeg;                /* Tlli used during the cont resolution */
 u8  DlTFI;                     /* Downlink TFI */
 u8  UlTFI;                     /* Uplink TFI   */
 u8  DlTFINext;                 /* Next Downlink TFI */
 u8  UlTFINext;                 /* Next Uplink TFI   */
#define MAC_INVALID_TFI 0xFF
 u8  DlPollCter;                /* Counter for the polling to avoid to      */
                                /* release a TBF before the emission of all */
 u8  UlPollCter;                /* the Polling response (Packet Ctrl Ack and*/
                                /* Packet Downlink Ack/Nack)                */
 u8  CtrlQ;                     /* Queue of mac_CtrlQ_t                     */
 u8  PollQ;                     /* Queue of msg_t, answer to polling        */
 u8  PollQIdx;                  /* Index of the polling block in the queue  */
 u8  T3200FreeIdx;              /* Bitfield of the free T3200Idx, 0:free    */
 u8  RadioPrio;                 /* Memorize the radio priority              */
 u8  PersLev;                   /* Memorize the persistence level           */
 u8  PCCODataOff;               /* Data offset of the PCCO                  */
 u16 TQI;
 u32 TLLI[2];
 u32 newTLLI[2];
#define MAC_INVALID_TLLI 0xFFFFFFFF
 l1_ChnQualityInd_t* ChnQualReport; /* Channel Quality Report to be included*/
                                    /* in the next PDlk Ack/Nack            */
 Msg_t* PCCOMsg;
} mac_Data_t ;


/*
** Timer duration
*/
#define MAC_T3166_DURATION ( 5 SECONDS )

/*
** Shortcut defines
*/
#define MAC_CTRL_HD ((c_MacCtrlHder*)pRRCtx->DecOut)
#define MAC_PULACK  ((c_PUplinkAckNack*)pRRCtx->DecOut)

/*
** Control queue size
*/
#define MAC_IDLE_QUEUE   8
#define MAC_PACKET_QUEUE 2

/*
** Suspend/Resume threshold
*/
#define MAC_DATAQ_THRESH_MIN 2

#define MAC_FILL_NBALLOCTS( _Bitmap, _Res )                  \
{                                                            \
  u8 i;                                                      \
  _Res = 0;                                                  \
  for ( i=0 ; i<8 ; i++ )                                    \
  {                                                          \
    if ( (_Bitmap >> i) & 1 )                                \
      _Res ++;                                               \
  }                                                          \
}


/*
** PCtrlAck config used to know if there is already one planned
*/
typedef struct
{
 u16 ST;
 u8  TN;
} mac_CACfg_t;

/*
================================================================================
  Data       : mac_Data_t
--------------------------------------------------------------------------------

  Scope      : Global MAC context

================================================================================
*/
typedef struct
{
 u8     T3200Idx;
 u8     RTI;
 Msg_t* SvMsg;
} mac_CtrlQ_t;

typedef struct
{
 u8 RTI;
 u8 TN;
} mac_RefQ_t;
#endif // __MAC_DATA_H__
