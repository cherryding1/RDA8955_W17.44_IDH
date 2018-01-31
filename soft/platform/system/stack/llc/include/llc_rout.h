//------------------------------------------------------------------------------
//  $Log: llc_rout.h,v $
//  Revision 1.3  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : LLC_ROUT.H
--------------------------------------------------------------------------------

  Scope      : prototypes for LLC routing functions

  History    :
--------------------------------------------------------------------------------
  Mar 23 04  |  FCH   | Creation
================================================================================
*/
#ifndef __LLC_ROUT_H__
#define __LLC_ROUT_H__

void llc_MmpTlliInd           (void);
void llc_MmpCiphParamInd      (void);
void llc_RldDataInd           (void);
void llc_RluQueueCreateInd    (void);
void llc_MmpSuspendReq        (void);
void llc_MmpResumeReq         (void);
void llc_MmpTriggerReq        (void);



#endif // __LLC_ROUT_H__

