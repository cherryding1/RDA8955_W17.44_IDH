//------------------------------------------------------------------------------
//  $Log: mm_tool.h,v $
//  Revision 1.3  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : mm_tool.h
--------------------------------------------------------------------------------

  Scope      : Header file for the generic MM functions

  History    :
--------------------------------------------------------------------------------
  Aug 11 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __MM_TOOL_H__
#define __MM_TOOL_H__

// System includes
#include "sxs_type.h"
#include "mm_msg.h"

#if !(STK_MAX_SIM==1)
typedef struct EvtSimIdUnite_t
{
    u32 Id;
    u32 SimId;
}EvtSimIdUnite_t;
#endif

#define MM_READ_SIM_ERR 0x10
#define MM_AUTH_SIM_ERR 0x11

void mm_WriteFileSim          ( u8, u8*, u8, u8 );
bool mm_ReadFileSim           ( u8, u8*, u8, u8 );
u8   mm_ReadFileSimOpt        ( u8, u8*, u8, u8 );
bool mm_ReadFileStatusSim     ( u8, u8* );

#ifdef __USING_SMS_STORE_PS_LOCI__
u8 mm_ReadRecodeFileSim( u8 SimFile, u8* Buffer, u8 Length, u8 reodeNo);
void mm_WriteRecodeFileSim( u8 SimFile, u8* Data, u8 Length, u8 reodeNo);
#endif

void mm_BackInact             ( u8 );
u8   mm_AnswerIdReq           ( u8, u8* );
void mm_BuildIMEI             ( u8* );
u8    *mm_GiveFLaiPtr         ( u8* );
void  mm_InsFLai              ( u8* );
void  mm_DelFLai              ( u8 * );
u8    mm_PurgeQueue           ( void *, void * );
Msg_t *mm_AuthenticationReq   ( u8, u8 *, u8 *, bool );
void  mm_HdleInformation      ( void );
void  mm_HandleEvt            ( void );
void mm_DelSavedCellInd       ( void );
void mm_CreateSavedCellInd    ( void );
void mm_SndSimErrInd        ( u8 cause );
void mm_SndSimStopReq         ( void );
void mm_DualSaveMsg(u32 EvtId);

#ifdef __MM_SPY__
void mm_InitSpy               ( void );
void mm_UpdateSpy             ( void );
#endif

u8 mm_EvtIdCmp ( u32 *Id, Msg_t *Msg  );
u8 *mm_DuleIsEvtInQueue ( u32 EvtId );


#if !(STK_MAX_SIM==1)
u8 *mm_DuleIsEvtSimIdInQueue ( EvtSimIdUnite_t *inpara );
bool mm_CheckCurConnPermit(void);
bool mm_CheckConnStatus(void);
bool mm_CheckOtherSimCallSta(void);
void mm_DualReSendCurrentSimMsg(void);

#endif

bool mm_StkStopingOrStoped();
bool mm_StkStoped();

#define MM_CKECK_TEST_SIM_FALSE 0
#define MM_CKECK_TEST_SIM_TRUE 1

// this function defined in API
extern u32 api_CheckTestSim(u32 nSimId);
extern u32 mm_CheckTestSim(u32 nSimId);
void mm_StartTimer (u32 Period, u32 Id, void *Data, u8 ReStart);
void  mm_StopTimer(u32 Id,void* pdata);
u32  mm_ExistTimer(u32 Id);

#define MM_CELL_CHANGE_SAME  0
#define MM_CELL_CHANGE_CELL   1
#define MM_CELL_CHANGE_RA    2
#define MM_CELL_CHANGE_LAI    3
u32 mm_CellChanged(void) ;
u32 mm_MmcCellChanged(void);
bool mm_CellAccessClassChanged(void) ;	
extern bool sxs_SimFastAtr (void);
#endif // __MM_TOOL_H__
