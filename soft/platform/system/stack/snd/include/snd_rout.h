//------------------------------------------------------------------------------
//  $Log: snd_rout.h,v $
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
  File       : SND_ROUT.H
--------------------------------------------------------------------------------

  Scope      : prototypes for SNDCP routing functions

  History    :
--------------------------------------------------------------------------------
  Jul 18 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_ROUT_H__
#define __SND_ROUT_H__

extern void snd_RouteQueueDataReq  (void);
extern void snd_RouteDataInd       (void);
extern void snd_RouteUnitDataInd   (void);
extern void snd_RouteActivateInd   (void);
extern void snd_RouteDeactivateInd (void);
extern void snd_RouteEstabCnf      (void);
extern void snd_RouteEstabInd      (void);
extern void snd_RouteDataCnf       (void);
extern void snd_RouteReleaseCnf    (void);
extern void snd_RouteReleaseInd    (void);
extern void snd_RouteModifyInd     (void);
extern void snd_RouteResetInd      (void);
extern void snd_RouteXidInd        (void);
extern void snd_RouteSequenceInd   (void);
extern void snd_RouteXidCnf        (void);
#endif // __SND_ROUT_H__

