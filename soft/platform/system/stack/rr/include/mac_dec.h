//------------------------------------------------------------------------------
//  $Log: mac_dec.h,v $
//  Revision 1.6  2006/06/06 22:28:02  oth
//  Addition of the PdchDist Array to compute the exact FN for PS Data Req
//
//  Revision 1.5  2006/05/22 14:36:19  oth
//  Update the copyright
//
//  Revision 1.4  2006/04/06 09:34:08  oth
//  New change in the computation of the polling date
//
//  Revision 1.3  2006/02/27 10:35:48  oth
//  Modification for the CPUTE_RRBP for customer b
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
  File       : mac_dec.h
--------------------------------------------------------------------------------

  Scope      : Defines for decoding

  History    :
--------------------------------------------------------------------------------
  Oct 15 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __MAC_DEC_H__
#define __MAC_DEC_H__

// System includes
#include "sxs_type.h"

/*
** MAC header
*/
#define MAC_DATA_HDER_SIZE 2
#define MAC_CTRL_HDER_SIZE 1

/*
** Payload types defines
*/
#define MAC_PL_SIZE       2
#define MAC_PL_MASK       0xC0
#define MAC_DATA_PL       0
#define MAC_CTRL_PL_NOOPT 1
#define MAC_CTRL_PL_WOPT  2
#define MAC_CTRL_PL_WAC   3
#define MAC_CTRL_PL_ULK   0x01

#define MAC_AC_MASK       0x01
#define MAC_PAGEMODE_MASK 0x03

/*
** Message Type
*/
#define MAC_PCELLCHGE_ORDER       0x01
#define MAC_PDOWNLINK_ASSIGNEMENT 0x02
#define MAC_PMEASURE_ORDER        0x03
#define MAC_PPOLLING_REQ          0x04
#define MAC_PPCTA                 0x05
#define MAC_PQUEUEING_NOTIF       0x06
#define MAC_PTIMESLOT_RECONF      0x07
#define MAC_PTBF_RELEASE          0x08
#define MAC_PUPLINK_ACKNACK       0x09
#define MAC_PUPLINK_ASSIGNEMENT   0x0A
#define MAC_PACCESS_REJ           0x21
#define MAC_PPAGING_REQ           0x22
#define MAC_PPRACH_PARAM          0x24
#define MAC_PDOWNLINK_DUMMY_CTRL  0x25
// Packet Ctrl Ack Message type
#define MAC_MT11_CTRLACK    0x01F9
#define MAC_MT8_CTRLACK     0x1F
#define MAC_MTN_CTRLACK     0x01

/*
** Type of Ack in PPolling Request
*/
#define MAC_ACCESS_ACK_FORMAT 0
#define MAC_NORMAL_ACK_FORMAT 1

/*
** Packet Uplink Ack/Nack
*/
#define MAC_FAI_MASK   0x80




#define MAC_PSI_BITOFFSET 16

/*
** Control MAC header builder
*/
#define MAC_BUILD_CTRLHDER( _OctetHder )                \
  _OctetHder = (MAC_CTRL_PL_ULK << 6) | (MAC_DATA.Flags & MAC_RETRY_BIT)

/*
** Emission date computer
*/
#define MAC_CPUTE_EMDATE( _ReceivedDate, _RRBP )        \
  ( 13 + ((13 * _RRBP) / 3) + (_ReceivedDate) ) % 42432

#ifdef __MAC_CTRL_C__
const u8 mac_PdchDist [13] = {0, 3, 2, 1, 0, 3, 2, 1, 0, 4, 3, 2, 1};
#else
extern const u8 mac_PdchDist[13];
#endif

/*
** Update of the Packet Uplink Dummy Control
*/
#define MAC_UPD_ULDUM( _Tlli )                                               \
{                                                                            \
 u32 Status = sxr_EnterSc( );                                                \
 l1_PUlDumCtrl[1] = ( l1_PUlDumCtrl[1] & 0xFC ) | ( (_Tlli >> 30) & 0x03 );  \
 l1_PUlDumCtrl[2] = ( _Tlli >> 22 ) & 0xFF;                                  \
 l1_PUlDumCtrl[3] = ( _Tlli >> 14 ) & 0xFF;                                  \
 l1_PUlDumCtrl[4] = ( _Tlli >> 6  ) & 0xFF;                                  \
 l1_PUlDumCtrl[5] = ( _Tlli << 2  ) & 0xFC;                                  \
 sxr_ExitSc( Status );                                                       \
}


#endif // __MAC_DEC_H__
