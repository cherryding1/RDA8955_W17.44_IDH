//------------------------------------------------------------------------------
//  $Log: lap_rout.h,v $
//  Revision 1.3  2006/05/22 12:46:12  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:38:45  pca
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
  File       : LAP_ROUT.H
--------------------------------------------------------------------------------

  Scope      : prototypes for LAP routing functions

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __LAP_ROUT_H__
#define __LAP_ROUT_H__

void lap_L1DataInd (void)     ;
void lap_TestIFrame(void)     ;
void lap_TestUIFrame(void)    ;
void lap_TestRRFrame(void)    ;
void lap_TestREJFrame(void)   ;
void lap_TestUAFrame(void)    ;
void lap_TestSABMFrame(void)  ;
void lap_TestDMFrame(void)    ;
void lap_TestDISCFrame(void)  ;


#endif // __LAP_ROUT_H__

