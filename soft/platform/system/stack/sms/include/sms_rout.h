//------------------------------------------------------------------------------
//  $Log: sms_rout.h,v $
//  Revision 1.3  2006/05/22 07:57:21  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : SMS_ROUT.H
--------------------------------------------------------------------------------

  Scope      : prototypes for SMS routing functions

  History    :
--------------------------------------------------------------------------------
  Feb 20 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SMS_ROUT_H__
#define __SMS_ROUT_H__

void sms_RouteMmDataInd  (void);
void sms_RouteMmpDataInd (void);
void sms_DataInd         (void);
void smr_DataInd         (void);
void sms_RouteReleaseInd (void);
void sms_RouteAbortInd   (void);


#endif // __SMS_ROUT_H__

