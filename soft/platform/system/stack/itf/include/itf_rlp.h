/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2005

================================================================================
*/














/*
================================================================================
  File       : ITF_RLP.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by the RLP entity
  History    :
--------------------------------------------------------------------------------
  Nov 04 05  |  FCH   | Update with new rlp_UDataReq_t and rlp_DataReq_t
             |        | definition, and add RLP_ALLOC_DATAREQ, rlp_UdataInd_t
             |        | rlp_DataInd_t, rlp_FlowCtrlReq_t and rlp_FlowCtrlInd_t
  Mar 31 05  |  FCH   | Creation
================================================================================
*/

#ifndef __ITF_RLP_H__
#define __ITF_RLP_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_msgc.h"
#include "itf_defs.h"
#include "itf_api.h"



//============================================================================
// RLP Internal msg Start at INTRABASE + 0x8000
//============================================================================
#define RLP_INTRA_BASE        ( HVY_RLP + 0x8000 )



//================================================
// Primitive IDs
//================================================
enum
{
    /*
    ** To RLP
    */
  RLP_CONF_REQ          = ( HVY_RLP + 0  ),
  RLP_ATTACH_REQ        ,
  RLP_DETACH_REQ        ,
  RLP_CONNECT_REQ       ,
  RLP_DATA_REQ          ,
  RLP_UDATA_REQ         ,
  RLP_RESET_REQ         ,
  RLP_DISC_REQ          ,
  RLP_CRLP_REQ          ,
  RLP_CRLP_RD_REQ       ,
  RLP_FLOWCTRL_REQ      ,

    /*
    ** From RLP
    */
  RLP_CONNECT_CNF       ,
  RLP_CONNECT_IND       ,
  RLP_DATA_IND          ,
  RLP_UDATA_IND         ,
  RLP_RESET_IND         ,
  RLP_DISC_CNF          ,
  RLP_DISC_IND          ,
  RLP_FAIL_IND          ,
  RLP_CRLP_RD_IND       ,
  RLP_FLOWCTRL_IND      ,

};






/*
================================================================================
  Structure  : rlp_ConfReq_t
--------------------------------------------------------------------------------
  Direction  : L2R -> RLP
  Message    : RLP_CONF_REQ
  Scope      : Provides RLP with current channel coding.
               This config must be sent to RLP just after CC has received a
               CONNECT or a CONNECT_ACK
  Comment    : 
================================================================================
*/

typedef struct
{
  u8  ChanCod;      
} rlp_ConfReq_t;


  



#define RLP_CHCOD_F4_8    0
#define RLP_CHCOD_F9_6    1
#define RLP_CHCOD_F14_4   2


/*
================================================================================
  Structure  : rlp_ConnectCnf_t
--------------------------------------------------------------------------------
  Direction  : RLP -> L2R
  Message    : RLP_CONNECT_CNF
  Scope      : Provides L2R with the peer response to a ConnectReq
               
  Comment    : 
================================================================================
*/

typedef struct
{
  u8  Status;
} rlp_ConnectCnf_t;


  



#define RLP_CONNECT_NEG   0
#define RLP_CONNECT_OK    1



/*
================================================================================
  Structure  : rlp_UDataReq_t, rlp_DataReq_t
--------------------------------------------------------------------------------
  Direction  : L2R -> RLP
  Message    : RLP_UDATA_REQ, RLP_DATA_REQ
  Scope      : Provides RLP with data to be sent unacknowledged or acknowledged
  Comment    : 
================================================================================
*/
#define RLP_ITF_HDR_LEN   2
#define RLP_ITF_FCS_LEN   3

typedef struct
{
    // Fieds reserved for RLP usage
 void   *Next;
} rlp_DReqCtrl_t;


typedef struct 
{
  rlp_DReqCtrl_t   Ctrl;
  u8               Reserved[ILHDR_SPECIFIC_SIZE-sizeof(rlp_DReqCtrl_t)];
  u16              DataLen     ; // Nb of bytes of the UserData field 
                                 // !!! MUST include DataOffset 
  u8               DataOffset  ; // Data will be found at UserData[DataOffset]
  u8               Pad         ; // Do not use
  u8               UserData[1] ; // DataLen elts are allocated for this array
}rlp_UDataReq_t,
 rlp_DataReq_t;


#define RLP_ALLOC_DATAREQ(_Ptr, _DataSize)                                                  \
{                                                                                           \
  _Ptr = sxr_Malloc(sizeof(MsgHead_t) + OFFSETOF(rlp_DataReq_t,UserData) +                  \
                    RLP_ITF_HDR_LEN + (_DataSize) + RLP_ITF_FCS_LEN);                       \
  ((u8 *)_Ptr)[sizeof(MsgHead_t) + OFFSETOF(rlp_DataReq_t,DataOffset)] = RLP_ITF_HDR_LEN;   \
  *((u16 *) &(((u8 *)_Ptr)[sizeof(MsgHead_t) + OFFSETOF(rlp_DataReq_t,DataLen)])) =         \
                                                             (_DataSize) + RLP_ITF_HDR_LEN; \
}


/*
================================================================================
  Structure  : rlp_UDataInd_t, rlp_DataInd_t
--------------------------------------------------------------------------------
  Direction  : L2R -> RLP
  Message    : RLP_UDATA_IND, RLP_DATA_IND
  Scope      : Provides L2R with data received data (unacknowledged or
               acknowledged)
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8               Reserved[ILHDR_SPECIFIC_SIZE];
  u16              DataLen     ; // Nb of bytes of the UserData field 
                                 // include DataOffset 
  u8               DataOffset  ; // Data will be found at UserData[DataOffset]
  u8               Pad         ; // Not use
  u8               UserData[1] ; // DataLen elts are allocated for this array
}rlp_UDataInd_t,
 rlp_DataInd_t;


/*
================================================================================
  Structure  : rlp_FlowCtrlReq_t, rlp_FlowCtrlInd_t
--------------------------------------------------------------------------------
  Direction  : L2R -> RLP, RLP -> L2R
  Message    : RLP_FLOWCTRL_REQ, RLP_FLOWCTRL_IND
  Scope      : Flow control handling between RLP and L2R
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8    Action;
}rlp_FlowCtrlReq_t,
 rlp_FlowCtrlInd_t;

  



#define RLP_RESUME    0
#define RLP_SUSPEND   1


/*
================================================================================
  Structure  : rlp_CrlpReq_t
--------------------------------------------------------------------------------
  Direction  : API -> RLP
  Message    : RLP_CRLP_REQ
  Scope      : Provides RLP with the user RLP system parameters (see AT+CRLP)
  Comment    : 
================================================================================
*/
typedef api_CsdCrlpSetReq_t rlp_CrlpReq_t;


/*
================================================================================
  Structure  : rlp_CrlpRdInd_t
--------------------------------------------------------------------------------
  Direction  : RLP -> API
  Message    : RLP_CRLP_IND
  Scope      : Indicates current RLP system parameters settings
  Comment    : 
================================================================================
*/
typedef api_CsdCrlpRdInd_t rlp_CrlpRdInd_t;


#endif  // __ITF_RLP_H__
