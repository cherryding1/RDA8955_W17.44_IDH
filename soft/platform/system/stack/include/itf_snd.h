//------------------------------------------------------------------------------
//  $Log: itf_snd.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
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
  File       : ITF_SND.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by SNDCP
  History    :
--------------------------------------------------------------------------------
  Jun 29 04  |  ABA   | Creation
================================================================================
*/

#ifndef __ITF_SND_H__
#define __ITF_SND_H__

#include "sxs_type.h"
#include "cmn_defs.h"

/*
** SND Internal msg Start at INTRABASE + 0
*/
#define SND_INTRA_BASE ( HVY_SND + 0x8000 )

/*
** Primitive ID definitions
*/
#define SND_SEND_REQ                    ( HVY_SND + 1  )
#define SND_DATA_IND                    ( HVY_SND + 2  )
#define SND_QUEUECREATE_IND             ( HVY_SND + 3  )
#define SND_QUEUEDATA_REQ               ( HVY_SND + 4 +  SXS_TIMER_EVT_ID)

/*
================================================================================
  Structure  : snd_SendReq_t
--------------------------------
  Direction  : API -> SND
  Scope      : API asks for the sending of a N_PDU
  Comment    : Inter layer message
================================================================================
*/
/*
================================================================================
  Structure  : snd_DataInd_t
--------------------------------
  Direction  : SND -> API
  Scope      : SND sends a N_PDU to API
  Comment    : Inter layer message
================================================================================
*/
#define SND_ACK_MODE   0
#define SND_UNACK_MODE 1
typedef struct
{
  u8 NSapi;
  bool Mode;
}snd_SendReq_t,
 snd_DataInd_t;

/*
================================================================================
  Structure  : snd_QueueCreateInd_t
--------------------------------
  Direction  : SND -> API
  Scope      : SND indicate the creation of a queue with the associated queue identifier
               API uses this queue to send SND_SEND_REQ messages
  Comment    :
================================================================================
*/
typedef struct
{
  u8 QueueId;
}snd_QueueCreateInd_t;

/*
================================================================================
  Structure  : snd_QueueDataReq_t
--------------------------------
  Direction  : API -> SND
  Scope      : API sends an indication that it has inserted SendReq
               in the shared queue
  Comment    : no parameter
================================================================================
*/


#endif  // __ITF_SND_H__
