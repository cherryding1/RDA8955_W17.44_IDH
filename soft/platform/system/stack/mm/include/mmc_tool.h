//================================================================================
// File       : mmc_tool.h
//--------------------------------------------------------------------------------
//  Scope      : Header file for the generic Circuit Switch MM functions
//  History    :
//--------------------------------------------------------------------------------
// 20131119 | DMX | Add mmc_ReleaseSavedEstabReq
// 20060116 | OTH |  Add the AbandonFOR (Follow on Proceed) procedure
// 20030906 | OTH | Creation            
//================================================================================


#ifndef __MMC_TOOL_H__
#define __MMC_TOOL_H__

// System includes
#include "sxs_type.h"

// Interface includes
#include "itf_rr.h"

void mmc_UpdRandAndRes( u8*, u8* );

void mmc_LocUp               ( u8   );
void mmc_LocUpFailure        ( bool );
void mmc_AskForRrCx          ( u8   );
void mmc_CellIndGenProc      ( void );
void mmc_HandleT3212         ( void );
void mmc_ImsiDetach          ( bool );
void mmc_PrepareCmServReq    ( u8, bool );
void mmc_ResetLocUpParam     ( u8, bool );
void mmc_BuildTmsiReallocCplt( void );
void mmc_SdLRStatus          ( u8, bool, u8, u8* );
u8   mmc_FillUpMobId         ( u8*  );
void mmc_SdMmStatus          ( u8   );
void mmc_HandlePaging        ( bool );
void mmc_HandleCMAbort       ( void );
void mmc_ReleaseCM           ( u8 );
void mmc_CheckEncode         ( s16, u8, u8 );
#define MMC_RRCNX_NEEDED 0
#define MMC_RR_TRANSFER  1
#define MMC_NO_ACTION    2
void mmc_HandleOtherPD       ( void );
void mmc_StopLocUp           ( void );
void mmc_SdSelfLocUp         ( u8   );
void mmc_ActivateCnx         ( void );
u8   mmc_SdStopCnf           ( void );
void mmc_AbandonFOR          ( void );

// MMC macros
// This macro is waiting for a PDTI
#define WHICH_MBX_PDTI( _PDTI )                                 \
( _PDTI & 0x0F ) == MM_CC_PD  ? CC_MBX  :                       \
( _PDTI & 0x0F ) == MM_SMS_PD ? SMS_MBX : SS_MBX

#define WHICH_MBX_PD( _PD )                     \
 _PD == MM_CC_PD  ? CC_MBX  :                   \
 _PD == MM_SMS_PD ? SMS_MBX : SS_MBX

#define MMC_GET_TI( _PDTI ) (_PDTI & 0xF0) >> 4
#define MMC_GET_PD( _PDTI ) (_PDTI & 0x0F)


// Capabilities variables
extern bool e2p_HRSupported;
#define MMC_GET_HR_SUPPORTED e2p_HRSupported

// dmx,add,20140326
void mmc_AddCxNb(u8 PDTI);
void mmc_ReduceCxNb(u8 PDTI);
void mmc_CleanCxNb(void );

void mmc_ReleaseSavedEstabReq(); //  add dmx 20131119

u8 mmc_SendApiCnf();  // add dmx 20140424

BOOL mmc_CheckMbxForCellInd(u32 TimerID);  // add dmx 20148028
void mmc_SendMMEstabErr(u8 PdTi,u8 cause); //__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

#endif // __MMC_TOOL_H__
