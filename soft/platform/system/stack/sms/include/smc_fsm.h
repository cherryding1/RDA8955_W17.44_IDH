//------------------------------------------------------------------------------
//  $Log: smc_fsm.h,v $
//  Revision 1.6  2006/05/22 07:05:12  aba
//  Copyrigth is updated
//
//  Revision 1.5  2006/02/28 16:15:46  aba
//  Issue 360: New SMS MO sending impossible after wrong SC address used (CM_Service_Rej) => Send of CP_ERROR during abort
//  Add PDTI in MM_RELEASE_REQ
//
//  Revision 1.4  2006/02/24 18:21:14  aba
//  Issue 300 Timer TC1M not stopped
//
//  Revision 1.3  2006/02/23 16:45:47  aba
//  Issue 355  "ERROR" always answered to the user when trying to send new SMS MO (SMC was not released if establishment failed and TR1M expired)
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================
  File       : smc_fsm.h
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for smc

  History    :
--------------------------------------------------------------------------------
  Sep 2812 03  |  ABA   | Creation
================================================================================
*/
#include "fsm.h"

/*
=======================
  SMC state definitions
=======================
*/
#define SMC_MO_GENE                  0
#define SMC_MO_IDLE                  1
#define SMC_MO_WAIT_MM_ESTAB_CNF     2
#define SMC_MO_WAIT_CP_ACK           3
#define SMC_MO_CONNECTED             4
#define SMC_MO_WAIT_NEW_MM_ESTAB_CNF 5

#define SMC_MT_GENE                  0
#define SMC_MT_IDLE                  1
#define SMC_MT_CONNECTED             2
#define SMC_MT_WAIT_CP_ACK           3


/*
** Transitions prototypes
*/
extern u8 smc_DoNothing                  (void);
// MO SMS
extern u8 smc_MOAllState_ReleaseInd      (void);
extern u8 smc_MOIdle_SmrEstabReq         (void);
extern u8 smc_MOIdle_TC1M                      (void);
extern u8 smc_MOIdle_AbortReqEvt(void);

extern u8 smc_MOWaitEstabCnf_EstabCnf    (void);
extern u8 smc_MOWaitEstabCnf_EstabErr    (void);
extern u8 smc_MOWaitEstabCnf_ReleaseReq  (void);
extern u8 smc_MOWaitEstabCnf_ReleaseInd   (void);
extern u8 smc_MOWaitCpAck_CpAck          (void);
extern u8 smc_MOWaitCpAck_TC1M           (void);
extern u8 smc_MOWaitCpAck_CpError        (void);
extern u8 smc_MOWaitCpAck_CpData         (void);
extern u8 smc_MOWaitCpAck_Error97        (void);
extern u8 smc_MOWaitCpAck_ReleaseReq     (void);
extern u8 smc_MOWaitCpAck_AbortReq       (void);
extern u8 smc_MOWaitCpAck_ReleaseInd  (void);
extern u8 smc_MOConnected_CpData         (void);
extern u8 smc_MOConnected_ReleaseReq     (void);
extern u8 smc_MOConnected_AbortReq       (void);
extern u8 smc_MOConnected_EstabReq       (void);
extern u8 smc_MOConnected_CpAck          (void);
extern u8 smc_MOConnected_CpError        (void);
extern u8 smc_MOConnected_Error97        (void);
extern u8 smc_MOConnected_DataReq        (void);
extern u8 smc_MOWaitNewEstabCnf_EstabCnf (void);
extern u8 smc_MOWaitNewEstabCnf_ReleaseReq(void);
extern  u8 smc_MOWaitNewEstabCnf_ServReqAck(void);
// add by dingmx 20070629
extern u8 smc_MOWaitNewEstabCnf_EstabErr(void);    
extern u8 smc_MOWaitNewEstabCnf_ReleaseInd(void);
extern u8 smc_MOWaitNewEstabCnf_AbortReq();

// MT SMS
extern u8 smc_MTAllState_ReleaseInd      (void);
extern u8 smc_MTIdle_Error97(void);
extern u8 smc_MTIdle_CpError(void);
extern u8 smc_MTIdle_CpData              (void);
extern u8 smc_MTIdle_DataReq(void);
extern u8 smc_MTConnected_DataReq        (void);
extern u8 smc_MTConnected_CpAck          (void);
extern u8 smc_MTConnected_CpData         (void);
extern u8 smc_MTConnected_CpError        (void);
extern u8 smc_MTConnected_Error97        (void);
extern u8 smc_MTWaitCpAck_CpAck          (void);
extern u8 smc_MTWaitCpAck_TC1M           (void);
extern u8 smc_MTWaitCpAck_CpData         (void);
extern u8 smc_MTWaitCpAck_CpError        (void);
extern u8 smc_MTWaitCpAck_Error97        (void);
extern u8 smc_MTWaitApiReport_AbortReq  (void);



