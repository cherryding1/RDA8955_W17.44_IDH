//------------------------------------------------------------------------------
//  $Log: smr_fsm.h,v $
//  Revision 1.4  2006/06/08 13:35:08  aba
//  Abort of the MMA procedure
//
//  Revision 1.3  2006/05/22 07:06:11  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================
  File       : smr_fsm.h
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for smr

  History    :
--------------------------------------------------------------------------------
  Sep 2812 03  |  ABA   | Creation
================================================================================
*/
#include "fsm.h"

/*
=======================
  SMR state definitions
=======================
*/
#define SMR_MO_GENE                 0
#define SMR_MO_IDLE                 1
#define SMR_MO_WAIT_RP_ACK          2
#define SMR_MO_WAIT_MMA_RP_ACK      3
#define SMR_MO_WAIT_RETRANS_MMA     4

#define SMR_MT_GENE                 0
#define SMR_MT_IDLE                 1
#define SMR_MT_WAIT_API_REPORT      2

/*
** Transitions prototypes
*/
extern u8 smr_DoNothing                     (void);
// MO SMS
extern void smr_MO_Idle_Init(void );
extern u8 smr_MOAllStateExceptIdle_ReleaseInd (void);
extern u8 smr_MOAllStateExceptIdle_AbortReq (void);
extern u8 smr_MOIdle_SmsPPSendReq           (void);
extern u8 smr_MOIdle_SmsPPSendMMAReq        (void);
extern u8 smr_MOIdle_SelfSend               (void);
extern u8 smr_MOWaitRpAck_RpAck             (void);
extern u8 smr_MOWaitRpAck_TR1M              (void);
extern u8 smr_MOWaitRpAck_TR1MRe        (void); // SMR_RESEND_DELAY
extern u8 smr_MOWaitRpAck_EstabErr          (void);
extern u8 smr_MOWaitRpAck_RpError           (void);
extern u8 smr_MOWaitRpAck_Error95           (void);
extern u8 smr_MOWaitRpAck_Error97           (void);
extern u8 smr_MOWaitRpAck_RpData            (void);
extern u8 smr_MOWaitRpAck_SmcErrorInd       (void);
extern u8 smr_MOWaitMMARpAck_RpAck          (void);
extern u8 smr_MOWaitMMARpAck_TR1M           (void);
extern u8 smr_MOWaitMMARpAck_EstabErr           (void);
extern u8 smr_MOWaitMMARpAck_SmcErrorInd    (void);
extern u8 smr_MOWaitMMARpAck_RpError        (void);
extern u8 smr_MOWaitMMARpAck_Error95        (void);
extern u8 smr_MOWaitMMARpAck_Error97        (void);
extern u8 smr_MOWaitMMARpAck_RpData         (void);
extern u8 smr_MOWaitMMARpAck_AbortMMA       (void);
extern u8 smr_MOWaitRetransMMA_TRAM         (void);
extern u8 smr_MOWaitRetransMMA_SmcErrorInd  (void);
extern u8 smr_MOWaitRetransMMA_AbortMMA     (void);
// MT SMS
extern u8 smr_MTAllState_ReleaseInd         (void);
extern u8 smr_MTIdle_RpData                 (void);
extern u8 smr_MTIdle_SmcErrorInd            (void);
extern u8 smr_MTIdle_Error95       (void);
extern u8 smr_MTIdle_Error97       (void);
extern u8 smr_MTWaitApiReport_SmsPPAckReq   (void);
extern u8 smr_MTWaitApiReport_SmsPPErrorReq (void);
extern u8 smr_MTWaitApiReport_TR2M       (void);
u8 smr_MTWaitApiReport_Error95(void);
u8 smr_MTWaitApiReport_Error97(void);






