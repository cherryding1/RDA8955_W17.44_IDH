//------------------------------------------------------------------------------
//  $Log: snd_fsm.h,v $
//  Revision 1.3  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.
  
  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/
/*
================================================================================
File       : snd_fsm.h
--------------------------------------------------------------------------------

Scope      : Finite State Machine defintions for sndcp

History    :
--------------------------------------------------------------------------------
Jun 28 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_FSM_H__
#define __SND_FSM_H__

#include "fsm.h"

/*
=========================
SNDCP state definitions
========================
*/
#define SND_GENE                      0
#define SND_IDLE                      1
#define SND_WAIT_ESTAB_CNF            2
#define SND_CONNECTED                 3
#define SND_WAIT_RELEASE_CNF          4
#define SND_WAIT_ESTAB_IND            5
#define SND_WAIT_RELEASE_IND          6
#define SND_ROUTING_AREA_UPD_INTER    7

/*
** Transitions prototypes
*/
extern u8 snd_DoNothing                            (void);
extern u8 snd_AllState_UnitDataReq                 (void);
extern u8 snd_AllState_UnitDataInd                 (void);
extern u8 snd_AllState_QueueCreateInd              (void);
extern u8 snd_Idle_ActivateInd                     (void);
extern u8 snd_Idle_DeactivateInd                   (void);
extern u8 snd_Idle_ModifyInd                       (void);
extern u8 snd_Idle_ResetInd                        (void);
extern u8 snd_Idle_EstabInd                        (void);
extern u8 snd_Idle_EstabCnf                        (void);
extern u8 snd_WaitEstabCnf_EstabCnf                (void);
extern u8 snd_WaitEstabCnf_ReleaseInd              (void);
extern u8 snd_WaitEstabCnf_EstabInd                (void);
extern u8 snd_WaitEstabCnf_ModifyInd               (void);
extern u8 snd_WaitEstabCnf_ResetInd                (void);
extern u8 snd_WaitEstabCnf_DeactivateInd           (void);
extern u8 snd_Connected_DeactivateInd              (void);
extern u8 snd_Connected_XidInd                     (void);
extern u8 snd_Connected_EstabInd                   (void);
extern u8 snd_Connected_EstabCnf                   (void);
extern u8 snd_Connected_ReleaseInd                 (void);
extern u8 snd_Connected_ModifyInd                  (void);
extern u8 snd_Connected_DataReq                    (void);
extern u8 snd_Connected_DataCnf                    (void);
extern u8 snd_Connected_DataInd                    (void);
extern u8 snd_Connected_ResetInd                   (void);
extern u8 snd_WaitReleaseCnf_ReleaseCnf            (void);
extern u8 snd_WaitReleaseCnf_ReleaseInd            (void);
extern u8 snd_WaitReleaseCnf_ModifyInd             (void);
extern u8 snd_WaitReleaseCnf_ResetInd              (void);
extern u8 snd_WaitEstabInd_EstabInd                (void);
extern u8 snd_WaitEstabInd_ResetInd                (void);
extern u8 snd_WaitEstabInd_DeactivateInd           (void);
extern u8 snd_WaitReleaseInd_ReleaseInd            (void);
extern u8 snd_WaitReleaseInd_ReleaseCnf            (void);
extern u8 snd_WaitReleaseInd_ResetInd              (void);
extern u8 snd_WaitReleaseInd_DeactivateInd         (void);
extern u8 snd_RoutingAreaUpdInter_SequenceInd      (void);
extern u8 snd_RoutingAreaUpdInter_EstabInd         (void);
extern u8 snd_RoutingAreaUpdInter_DataReq          (void);
extern u8 snd_RoutingAreaUpdInter_DeactivateInd    (void);

#endif

