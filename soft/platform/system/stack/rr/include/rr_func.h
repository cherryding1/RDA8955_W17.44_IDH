//------------------------------------------------------------------------------
//  $Log: rr_func.h,v $
//  Revision 1.13  2006/06/12 19:59:51  oth
//  Update from Bob
//
//  Revision 1.12  2006/05/22 14:39:23  oth
//  - Copyright update
//  - Sync with Bob
//
//  Revision 1.11  2006/05/11 13:03:49  oth
//  The rrp_HdleFreqParam is now returning an error index to determine if the frequencies are not in one band.
//
//  Revision 1.10  2006/04/21 14:45:29  oth
//  Update from Bob
//
//  Revision 1.9  2006/03/27 07:58:16  oth
//  Update from Bob DEV
//
//  Revision 1.8  2006/03/06 13:00:53  oth
//  Send RR_RESELECTION to RLU in case of release for Reselection (from RRI or from PCCO)
//
//  Revision 1.7  2006/02/13 06:17:37  oth
//  Change prototype of rrd_RequestResel to return a boolean to go back to idle
//
//  Revision 1.6  2006/02/08 11:24:10  oth
//  Update from Bob
//
//  Revision 1.5  2006/02/08 08:47:43  oth
//  Addition for the handling of the PCCO
//
//  Revision 1.4  2006/01/16 15:27:51  oth
//  Updates from Bob Dev
//
//  Revision 1.3  2006/01/12 10:31:21  oth
//  Evolution from the Dev of Bob
//
//  Revision 1.2  2005/12/29 17:39:12  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/
/*
================================================================================
  File       : rr_func.h
--------------------------------------------------------------------------------

  Scope      : Protoypes of rr functions

  History    :
--------------------------------------------------------------------------------
  Dec 21 05  |  MCE   | Created rri_StorePcco
  Dec 15 05  |  MCE   | Created rri_StoreDyn98AddMeasPar (used by PMO and PCCO)
             |        | Changed prototype of rri_Skip3GNCellDesc ( used by PMO 
             |        | and PCCO )
  Dec 02 05  |  MCE   | Created rri_StoreNcMeasParams() and 
             |        | rri_StoreDynNcMeasPars
  Nov 18 05  |  MCE   | suppression of rri_GoThruBcch
  Oct 26 05  |  MCE   | rri_SetNeedPTcInfo(): changed NeededPsi to an u32
  Oct 17 05  |  MCE   | changed prototype of rri_PrepCompleteBcch 
  Oct 17 05  |  MCE   | Changed prototype of rri_SetSiNotFresh()
  Sep 27 05  |  MCE   | Addition of rri_RrWantsCbch
  Aug 18 05  |  MCE   | Addition of rrg_SndL1CellOpt 
  Apr 07 05  |  MCE   | Addition of CBCH related prototypes : 
             |        | rri_CleanCbchCfg, rri_CleanTmpCbchCfg, 
             |        | rri_ActivateCbch, rri_StopCbch, rri_UpdL1CbchCfg, 
             |        | rri_ForgetCbch
  Aug 12 04  |  MCE   | Addition of rr_L1UpdRachParam()  
  Aug 20 03  |  PFA   | Creation 
================================================================================
*/

#ifndef __RR_FUNC_H__
#define __RR_FUNC_H__

// System includes
#include "sxs_type.h"
#include "sxr_ops.h"

// Interfaces
#include "itf_sim.h"     // EfName_t

// RR includes
#include "rr_cell.h"     // Cell definition
#include "rr_dec.h"

// EnDec includes
#include "rrd_ed_c.h"

// RRD includes
//#ifdef __RRD__
#include "rrd_data.h"   
//#endif
#ifdef __RRP__
#include "rrp_data.h"   
#endif

// RRI includes
#ifdef __RRI_C__
#include "rri_cmon.h"    // BaElem_t
#ifdef __EGPRS__
#include "rri_psel.h"
#endif
#include "rri_defs.h"
#endif

#ifdef __EGPRS__
#ifdef __RRI_P__
#include "rri_psel.h"
#endif
#else
#include "rri_psel.h"
#endif


/*
================================================================================
  Common prototypes for RR 
--------------------------------------------------------------------------------

  Scope      : Functions used by core RR task

================================================================================
*/

//           rr_route  
void         rr_l1DataInd     (void);
void         rr_lapDataInd    (void);


//           rr_evt
InEvtDsc_t * rr_GiveDsc       ( u32 MsgId );


//           rr_out	
void         rr_PrepMsg       ( OutEvtDsc_t * Evt, u16 DataLen ) ;
void         rr_SendMsg       ( u8 Mbx )      ;
            

//           rr_sim 
void         rri_ReqSimData   ( u8 EF, u16 Ofst, u8 Nb );
void         rri_UpdSimData   ( u8 EF, u16 Ofst, u8 Nb, u8 * Data );
void         rri_SavSimData   ( u8 EF ) ;
void         rri_SavUSimData   ( u8 EF ) ;
void         rri_ReqSimStatus ( u8 EF ) ;
void         rr_NewSimData    ( void )  ;
void         rr_NewSimRecord(void);
void         rr_ReadSimFileSt ( void )  ;
void         rri_ReqSimRecord ( u8 EF, u8 RecordNb, u8 Mode, u8 NbByte );
u8           rr_DecodeDataLength (u8 *dataLgth,  u8 *data);



//         rr_dfreq
u16        rr_ArfcnInFrqList( u16 Arfcn, u16 * FrqList, u16 NbFrq )     ;
s16        rr_DecFrqList    ( u16 * Dst, c_TFrqList * Src )             ; 
u16        DecodeBitMap0    ( u8 * BitMap, u8 Offset, u16 * OutList )   ;
u16        DecodeVarBitMap  ( u16 OrigArfcn, u8 * VarBitMap , u8 Offset ,
                              u8 Len, u16 * OutList )                   ;
u8         rr_GiveBand      ( u16 Arfcn )                               ;
bool       rr_SameBand      ( u8 Band, u16 Arfcn )                      ;

rr_DetailedFrqBand_t rr_GiveDetailedBand  ( u16 Arfcn )                            ;
bool                 rr_SameDetailedBand  ( rr_DetailedFrqBand_t Band, u16 Arfcn ) ;
bool                 rr_IsFrqBandSupported( rr_DetailedFrqBand_t Band )            ;

//         rr_dec
u8         rr_FindRfl       ( rr_ServPar_t *ServP, u8 RflNb )           ;
u8         rr_AllocRfl      ( rr_ServPar_t *ServP, u8 RflNb )           ;
u8         rr_CombineRfl    ( u16 *FreqL, ed_GprsMa_t * Ma )            ;
u8         rr_CombineRfl2   ( u16 *FreqL, u8 * IdxList, u8 NbIdx )      ;
u8         rr_DecMa         ( u16 * Dest, u8 * Src, u8 SrcSize, 
                              u16 * RefList, u8 NbRefFrq )              ;
u8         rr_DecMa2        ( u16 * Dst, u8 * Src, u16 * RefL,
                              u8 NbRefFrq, u8 Ofs, u8 Size )            ;
u8         rr_FindGMa       ( rr_ServPar_t *ServP, u8 MaNb )            ;
void       rr_BuildGMa      ( rr_GprsMa_t * DstGMa, ed_GprsMa_t * SrcGMa,
                              u8 MaNb, rr_ServPar_t *ServP )            ;
void       rr_BuildGMa2     ( rr_ServPar_t * ServP, rr_GprsMa_t * GMa   ,
                              rr_RawGMa_t  * RawGMa )                   ;
u8         rr_GetGMa        ( rr_ServPar_t *ServP, u8 MaNb )            ;
u8         rr_AllocGMa      ( rr_ServPar_t *ServP, u8 MaNb )            ;
u8         rr_StoreGMa      ( rr_ServPar_t *ServP, ed_GprsMa_t * Ma     ,
                              u8 MaNb)                                  ;
u8         rr_StoreGMa2     ( rr_ServPar_t * ServP, u8 * Buffer         ,
                              int * NewOfs, u8 MaNb )                   ;
void       rr_SkipGMa       ( u8 * Buffer, int * NewOfs )               ;
u8         rr_StoreFrqList2 ( u8 * Src, int * Ofs, u16 * Dst, u8 Len )  ;

//         rr_util
void       rr_DumpIlIn           ( InEvtDsc_t * Dsc, u8 Offset )         ;
s16        rr_ArfcnCmp           ( void *a , void *b )                   ;
void       rr_ComputeMsClassMark ( void )                                ;
void       rr_UpdateMsCm2        ( u16 Arfcn )                           ;
u16       rr_UpdateMsRACap (void);
bool       rr_CheckPlmn          ( rr_Cell_t * Cell, u8 * Plmn )         ;
bool       rr_CheckCountry       ( rr_Cell_t * Cell, u8 * Mcc )          ;
void       rr_StopL1Chans        ( u8 CfgBitMap )                        ;
u8         rr_GetS               ( void )                                ;
void       rr_PrepLapRrIl        ( u8 Len )                              ;
void       rr_SndStatus          ( void )                                ;
void       rr_SetPag             ( rr_Cell_t * Cell, u8 Mode, bool TS0 ) ;
void       rr_SndL1CcchReq       ( rr_Cell_t * Cell, u8 Mode, bool TS0 ) ;
bool       rr_SetPPag            ( rr_Cell_t * Cell, u8 Mode )           ;
bool       rr_SndL1PccchReq      ( rr_Cell_t * Cell, u8 Mode )           ;
void       rr_SortCa             ( u16 * Ca, u16 NbFreq )                ;
void       rr_L1UpdRachParam     ( void )                                ;
void       rr_CleanCa            ( void )                                ;
void       rr_CtxClean           ( void )                                ;
bool       rri_CellCmp           ( u16 Arfcn,  rr_Cell_t * Cell )        ;
rr_Cell_t *rri_FindCell          ( u16 Arfcn )                           ;
rr_Cell_t * rri_FindScanCellByArfcn(u16 Arfcn);
void rri_SetScanCellNull(rr_Cell_t *Cell);
BOOL rri_HasScanCell(void);
BOOL rri_IsScanCell(rr_Cell_t * cell);
BOOL rri_NeedWaitScanResult(void);


// added by leiming 20081006(begin)
void rr_StartTimer   (u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx);
u8   rr_StopTimer    (u32 Id, void *Data, u8 Mbx);
u8   rr_StopAllTimer (u32 Id, void *Data, u8 Mbx);
u32  rr_ExistTimer   (u32 Id);
// added by leiming 20081006(end)


//         rr_spy
void       rr_InitSpy       ( void )                           ;
void       rr_UpdateSpy     ( void )                           ;

/*
================================================================================
  Prototypes for RRI 
--------------------------------------------------------------------------------

  Scope      : Functions used by RRI    

================================================================================
*/


//          rri_sort
typedef     s16 comparF      ( void*, void* )   ;
void        qSort            ( void *baseP, int nElem, int width, 
                               comparF *compar );

void        rri_Init               ( void )                   ;
void        rri_InitVar            ( void )                   ;
void        rri_CopyList     ( ArfRssi_t * DstLst, ArfRssi_t * SrcLst, u16 Nb );
//added by leiming 20070807(begin)
//added it for warning
rr_APlmn_t * rri_GiveAPlmnPtr ( u8 * PlmnId );
//added by leiming 20070807(end)
void        rri_InsAvailPlmn   ( rr_Cell_t * Cell )                     ;
void        rri_DelAPlmnQ      ( void )                                 ;
u8          rri_PlmnCmp        ( u8 * PlmnId, rr_APlmn_t * FoundPlmn )  ;
u8          rri_PlmnPurge      ( void * PlmnId, void * FoundPlmn )      ;


#ifdef __RRI_P__

u8          rri_LaiCmp         ( u8 * LaiId, u8 * FoundLai )            ;

bool        rri_InvalidPlmn    ( u8 * PlmnId )                          ;

void        rri_AddFPlmn       ( u8 * PlmnId )                          ;

void        rr_StartLTDTimer    (void);


void        rri_AddAvailCell   ( rr_APlmn_t * PlmnPtr, rr_Cell_t * Cell);

void        rri_InsFLai        ( u8 * Lai )                             ;
void        rri_InsFLaiRegPro ( u8 * Lai )                             ; 
void        rri_InsFPlmn       ( u8 * Plmn )                            ;
void        rri_RmvFPlmn       ( u8 * PlmnId )                          ;
void        rri_FPlmnQ2L       ( u8 * FPlmnList )                       ;
u8 *        rri_GiveFPlmnPtr   ( u8 * PlmnId )                          ;
u8 *        rri_GiveFLaiPtr    ( u8 * Lai)                              ;
u8 *        rri_GiveFLaiRegProPtr    ( u8 * Lai)                              ;
bool        rri_CheckLai       ( rr_Cell_t * Cell )                     ;
void        rri_SendSearchPlmn ( u8 * Plmn )                            ;      

u8          rri_CopyPlmnL      ( u8 * SrcList, u8 * DstList, u8 Nb,u8 A);
u8          rri_FindPlmn       ( u8 * PlmnId,  u8 * PlmnL,   u8 Nb )    ;
u8          rri_MakeWPlmnL     ( u8 * FirstId, u8 * SkipId,  u8 * Lst ) ;
void        rri_SearchFirstPlmn( void )                                 ;
u8        * rri_FirstPlmn      ( void )                                 ;
void        rri_SearchFirstPlmn( void )                                 ;
u8        * rri_NextPlmn       ( u8 * PlmnL, u8 PlmnNb )                ;
u8          rri_NextOrNoSvce   ( void )                                 ;
void        rri_GoNoSvce       ( void )                                 ;
bool        rri_HigherPrio     ( u8 * PlmnId )                          ; 
void        rri_AddPSelProc    (u8 PSelProc )                           ;
void        rri_EndPSelProc    (u8 PSelProc )                           ;
void        rri_PSClean        (u8 PSelProc )                           ;
void        rri_SetReqBand ( u8 Band , u8 TriBand)                      ;

//          rri_api
void        rri_SendApiReg    ( u8 Status )    ;
void        rri_SendApiPsReg  ( u8 Status,  u8 Cause )    ;
void        rri_SendAPlmnList ( void )         ; 
void        rri_Send0PlmnList ( void )         ; 
void        rri_SendListAbort ( u8 Cause )         ;

//u8          rri_NoSvceIncr         ( u8 Dur )                 ; // [-][dingmx][20091208] not use it any more
void        rri_SendNasInfo        ( u8 Service, u8 NasUpdType ) ;
void        rri_StoreBaLToCsw();
void        rri_GetBaLFromCsw();
bool        rri_IsChinaMobile(u8* Plmn);
#endif

#ifdef __RRI_C__
             
//          rri_rssi
void        rri_DispRssi     ( void )                           ;
void        rri_OrderARssi   ( void )                           ;
void        rri_ReOrderARssi ( bool AcceptLow )                 ;
void        rri_AddList      ( u8 Band )                        ;
void        rri_SetARssiL    ( u8 ListType )                    ;
void        rri_SndARssiL    ( void )                           ;
s16         rri_RssiCmp      ( void *a , void *b )              ;
u16         rri_IsStored     ( u16 Arfcn )                      ;
void        rri_BoostStored  ( s16 Val )                        ;
s16         rri_ValidCells   ( ArfRssi_t * List,  u16 ArfcnNb ) ;

bool        rri_SameBand     ( u8 Band, u16 Arfcn )             ;
u8          rri_GiveBand     ( u16 Arfcn )                      ;
void        rri_NoiseKnown   ( void )                           ;
           
//          rri_scan
void        rri_DoScan       ( void )             ;
void        rri_EndScan      ( void )             ;
void        rri_VerifyScan   ( rr_Cell_t * Cell ) ;
           
//          rri_pag  
void        rri_GoIdle              ( rr_Cell_t * Cell, u8 Mode, bool TS0 ) ;
void        rri_UpdL1PagCfg         ( rr_Cell_t * Cell, u8 Mode )           ;
           
//          rri_bcch 
void        rri_StopTc              ( u8 StopTcNorm, u8 StopTcExt )           ;
void        rri_ResetBaRcvTcInfo    ( u16 Si )                                  ;
void        rri_ResetNeedTcInfo     ( rri_TcInfo_t * NeededTc )               ;
void        rri_ResetReqTc          ( void )                                  ;
void        rri_SetNeedTcInfo       ( rri_TcInfo_t * NeededTc, u16 NeededSi ) ;
void        rri_AddNeedTcInfo       ( rri_TcInfo_t * TcInfo, u16 AddedSi )    ;
void        rri_RmvNeedTcInfo       ( rri_TcInfo_t * TcInfo, u16 RmvedSi )    ;
void        rri_SendL1BcchCfg       ( rri_TcInfo_t * NeededTc )               ;
void        rri_RefreshBcchCfg      ( void )                                  ;
void        rri_SetNewNeedNotRcvReq ( void )                                  ;
u16         rri_WhichSi             ( u8 SiChgeField )                        ;
void        rri_PrepCompleteBcch    ( rr_Cell_t * Cell )                      ;
u8          rri_Switch2Bcch         ( void )                                  ;
           
//          rri_pbch
void        rri_ResetNeedPTcInfo    ( rri_PTcInfo_t * NeedPTc )               ;
void        rri_SetNeedPTcInfo      ( rri_PTcInfo_t * PTcInfo, u32 NeededPsi );
void        rri_SendL1PBcchCfg      ( rr_Cell_t * Cell, 
                                      rri_PTcInfo_t * NeededPTc )             ;
void        rri_StartPBcch          ( rr_Cell_t * Cell )                      ;
void        rri_StopPBcch           ( void )                                  ;
void        rri_RefreshPBcchCfg     ( void )                                  ; 
void        rri_SetNewNeedPsi       ( u8 NewChgeMark, u8 ChgeField )          ;
void        rri_ResetBaRcvPTcInfo   ( void )                                  ;
void        rri_PrepCompletePBcch   ( rr_Cell_t * Cell )                      ;
void        rri_Switch2PBcch        ( void )                                  ;

//          rri_csel
bool        rri_NextSynchro  ( void )            ;
void        rri_C1Eval       ( rr_Cell_t * Cell) ;
bool        rri_TestMode     ( rr_Cell_t * Cell) ;
bool        rri_CheckZones   ( rr_Cell_t * Cell );
bool        rri_C1BCrit      ( rr_Cell_t * Cell );
void        rri_CSClean      ( void )            ;
void        rri_ForwardPagExp( void )            ;
           

//          rri_rsel
void        rri_HandoverInd          ( u16 Arfcn, u8 Bsic )           ;
void        rri_C232Eval             ( rr_Cell_t * Cell, bool Strong );
void        rri_C31Eval              ( rr_Cell_t * Cell)              ;
void        rri_ReqReselGrant        ( u8 Cause )                     ;
void        rri_ReselReq             ( u8 Cause )                     ;
u8          rri_Reselection          ( u8 Cause )                     ;
void        rri_GuardRslExp          ( void )                         ;
void        rri_RecntRslExp          ( void )                         ;
rr_Cell_t * rri_BestC232             ( bool NoRet )                   ;
rr_Cell_t * rri_GiveReselCell        ( u8 Cause )                     ;
u8          rri_PreResel             ( u8 ReselCause )                ;
void        rri_CheckStartPnltyTimer ( rr_Cell_t * Cell )             ;
           
           



//        rr_cell
void        rri_FreeRawGMa     ( rr_ServPar_t * ServP, u8 i )           ;

//        rri_cell
rr_Cell_t * rri_NewCell        ( bool Main, u16 Arfcn, u8 Rla, 
                                 u8   IdxBa , bool ReselP    )          ;
void        rri_CleanGprsServ  ( rr_ServPar_t * ServP )                 ;
void        rri_CleanGprsCell  ( rr_Cell_t * Cell )                     ;
void        rri_CleanTmpCbchCfg( rr_ServPar_t * ServP )                 ;
void        rri_CleanCbchCfg   ( rr_ServPar_t * ServP )                 ;
void        rri_CleanGMa       ( rr_ServPar_t * ServP, u8 Idx )         ;
void        rri_CleanGMaArray  ( rr_ServPar_t * ServP )                 ;
void        rri_CleanRfl       ( rr_ServPar_t * ServP )                 ;
void        rri_FreeCell       ( rr_Cell_t * Cell       )               ;
void        rri_FreeServP      ( rr_ServPar_t * ServP   )               ;
void        rri_ReselParInit   ( rr_ReselPar_t * ReselP )               ;
void        rri_ReselParCopy   ( rr_ReselPar_t * D, rr_ReselPar_t * S ) ;
void        rri_ServParInit    ( rr_ServPar_t * ServP   )               ;
void        rri_CellParInit    ( rr_Cell_t * Cell       )               ;
void        rri_RstBcchSch     ( rr_Cell_t * Cell       )               ;
void        rri_SetSiNotFresh  ( rr_Cell_t * Cell       )               ;
void        rri_ResetReqInfo   ( rr_Cell_t * Cell       )               ;
           
//          rri_inst
void        rri_SaveInst          (rri_PsiInst_t * PsiInst, u8 InstIdx)   ;
void        rri_NextInst          (rri_PsiInst_t * PsiInst, u8 InstIdx    ,  
                                   u8 InstCount )                         ;
void        rri_InitPsiInst       ( rri_PsiInst_t * PsiInst, u8 ChgeMark  ,
                                    rr_ServPar_t  * ServP )               ;
void        rri_ResetSi2QInfo     ( void  * ServP )               ;
void        rri_ResetPSi2Info     ( void  * ServP )               ;
void        rri_ResetPSi3Info     ( void  * ServP )               ;
void        rri_ResetPSi5Info     ( void  * ServP )               ;
void        rri_ResetPmoInfo      ( void  * ServP )               ;
bool        rri_NewSetOfInst      ( rri_PsiInst_t * PsiInst, u8 ChgeMark  ,
                                    rr_ServPar_t  * ServP )               ;
bool        rri_CheckInst         ( u8 SavedChgeMark, u8 RecChgeMark      ,
                                    u8 InstIdx, u8 InstCount )            ;
bool        rri_InstReceived      ( rri_PsiInst_t * PsiInst, u8 InstIdx ) ;

//          rri_dec
void        rri_StoreSiPsi13 ( rr_Cell_t * Cell,int * NewOfs,u8 * Buffer,
                               u8 Len,u8 MT )                           ;
void        rri_StoreSi25N   ( rr_Cell_t *Cell,u8 *Data,bool RachP )    ;
void        rri_StoreSi25B   ( rr_Cell_t *Cell,u8 *Data,bool RachP )    ;
void        rri_StoreSi25T   ( rr_Cell_t *Cell,u8 *Data )               ;
u8          rri_StoreNCellDsc( u8 * NCellDsc, u16 * OutArfcnList )      ;
u8          rri_StoreFrqList ( u8 * Src, int Ofs, u16 * Dst, u8 Len )   ;
void        rri_StoreSi13    ( rr_Cell_t *Cell,u8 *Data )               ;
void        rri_StoreBsi     ( void )                                   ;
void        rri_StoreNcMeasParams ( int * NewOfs, u8 * Buffer, 
                                    rr_Cell_t * Cell )                  ;

//          rri_pdec
void        rri_StoreCellSelStruct      ( int * NewOfs, u8 * Buffer,  
                                          rr_ReselPar_t    * ReselP )         ;
void        rri_SkipExtMeasParStruct    ( int * NewOfs, u8 * Buffer )         ;
void        rri_SkipLsaParams           ( int * NewOfs, u8 * Buffer )         ;
void        rri_Skip3GNCellDesc         ( int * NewOfs,u8 * Buffer, bool Pmo );
void        rri_StoreGprsCellOptIE      ( int * NewOfs, u8 * Buffer,
                                          rr_Cell_t * Cell )                  ;
void        rri_StorePRachCtrlParams    ( int * NewOfs, u8 * Buffer,
                                          rr_Cell_t * Cell )                  ;
void        rri_StorePccchOrgParams     ( int * NewOfs, u8 * Buffer ,
                                          rr_Cell_t * Cell )                  ;
void        rri_StoreGlbPwrCtrlPar      ( int * NewOfs,u8 * B,u32* A, u32* R  ,
                                          rr_Cell_t * Cell )                  ;
void        rri_StoreCellIdentification ( int * NewOfs, u8 * Buffer, rr_Cell_t* Cell )         ;
void        rri_StoreNonGprsCellOpt     ( int * NewOfs, u8 * Buffer, rr_Cell_t* Cell )         ;
void        rri_StoreRefFreqLists       ( int * NewOfs, u8 * Buffer, u8 Len, rr_Cell_t* Cell );
void        rri_StoreCaLists            ( int * NewOfs, u8 * Buffer, u8 Len, rr_Cell_t* Cell  );
void        rri_StoreGprsMaLists        ( int * NewOfs, u8 * Buffer, u8 Len, rr_Cell_t* Cell  );
void        rri_StorePccchDscLists      ( int * NewOfs, u8 * Buffer, u8 Len, rr_Cell_t* Cell  );
void        rri_StoreAdditionalPsiMsg   ( int * NewOfs,u8 * B,u32* A, u32* R, rr_Cell_t* Cell );
void        rri_StoreDynNcMeasPars      ( int * NewOfs, u8 * Buffer, u8 Len, 
                                          u8 PmoIdx, rri_DynMeasPar_t * Param );
void        rri_StoreDyn98AddMeasPar    ( int * NewOfs, u8 * Buffer, u8 Len,
                                          u8 PmoIdx, rri_DynMeasPar_t * Param );

bool        rri_StorePsi2               ( rr_Cell_t * Cell )                  ;
bool        rri_StorePsi3               ( rr_Cell_t * Cell )                  ;
bool        rri_StorePsi3Bis            ( rr_Cell_t * Cell )                  ;
bool        rri_StorePsi5               ( rr_Cell_t * Cell )                  ;
bool        rri_StorePcco               ( void )                              ;
void        rri_StorePsi                ( void )                              ;
bool        rri_StorePmo                ( void )                              ;


//          rri_ba
void        rri_DispBa             ( void )                   ;
void        rri_DispFullSynchQ     ( rr_ReselPar_t * ReselP ) ;
void        rri_DispGsmNCellL      ( void )                   ;
void        rri_BaNotRec           ( void )                   ;
void        rri_ComputeBa          ( void )                   ;
void        rri_ComputeBaGprs      ( void )                   ;
void        rri_GsmNCellNotRec     ( void )                   ;
void        rri_CheckBaCmplte      ( rr_Cell_t * Cell )       ;
void        rri_SendBaCmplte       ( bool Psi )               ;
void        rri_MarkBsicAllowed    ( void )                   ;
void        rri_BuildSi2QGsmNCellL ( void )                   ;
void        rri_BuildPsi3GsmNCellL ( void )                   ;
void        rri_ResetFreqList      ( void )                   ;
void        rri_FreeGsmNCellL      ( void )                   ;
bool        rri_BuildPmoGsmNCellL  ( void )                   ;
bool        rri_BuildOrderGsmNCellL( bool Pmo )               ;

u8          rri_GiveIdxBa      ( u16 Arfcn, rri_BaElem_t * BaL, u8 BaNb )  ;
u8          rri_GiveIdxMon (u8 IdxBa, rri_MonElem_t * MonL, u8 MonNb );
u8          rri_GiveIdxNCell   ( rr_Cell_t * Cell, rri_BaElem_t * BaL,
                                 rri_GsmNCellElem_t * NCellL, u8 NCellNb ) ;
rr_Cell_t * rri_GiveCellFromBa ( u16 Arfcn, rri_BaElem_t * BaL,
                                 u8 BaNb,   u32* Idx )                      ;


//          rri_cmon
void        rri_DispSynchQ     ( void )                           ;
void        rri_DispMonL       ( rri_MonElem_t * MonL, u8 MonNb ) ;
void        rri_BuildMonList   ( void )                           ;
void        rri_BuildExtMonList( rr_Cell_t * Cell, u8* Data )     ;
void        rri_SendNcMonReq   ( bool Start )                     ;
void        rri_SendPMeasCtrl  ( void )                           ;
s16         rri_MonCmp         ( void *a , void *b )              ;
s16         rri_RxLCmp         ( void *a , void *b )              ;
void        rri_StopSynch      ( rr_Cell_t * Cell )               ;
void        rri_UpdateSynch    ( rr_Cell_t * Cell, bool Bcch, bool ExtBcch );
rr_Cell_t * rri_NewSynch       ( u8 IdxBa, u8 RxLev, bool RSel)   ; 
rr_Cell_t * rri_WeakestSynch   ( u8 Band )                        ;
void        rri_AddPreSynch    ( void )                           ;
void        rri_AddFirstSynch  ( void )                           ;
void        rri_DelSynchQ      ( void )                           ;
void        rri_ResetMonStatus ( void )                           ;
u8          rri_Synchronized   ( u8 Band )                        ;
bool        rri_NeighPending   ( void )                           ;
void        rri_Serv2Neigh     ( rr_Cell_t * OldMain )            ;
void        rri_Neigh2Serv     ( rr_Cell_t * NewMain )            ;

//          rri_meas
void        rri_SaveQualMod       ( void )                 ; 
void        rri_SaveCurCMod       ( void )                 ;
void        rri_SaveAdjCMod       ( void )                 ;
void        rri_SaveSyncMode      (void)                 ;
void        rri_SendQualReport    ( void )                 ; 
void        rri_SendCurCReport    ( void )                 ; 
void        rri_SendAdjCReport    ( void )                 ; 
void        rri_HandleMonitorInd  ( bool Report )          ;
void        rri_HandleMonitorInd2 ( bool Report )          ;
void        rri_HandleNCMonitorInd( u8   Report )          ;
bool        rri_EnhRepPossible    ( void )                 ;
void        rri_DoNormReport      ( void )                 ;
void        rri_DoPckReport       ( void )                 ;
void        rri_DoExtReport       ( void )                 ;
void        rri_DoEnhReport       ( void )                 ;
void        rri_InitMeasReport    ( void )                 ;
void        rri_StufMeasReport    ( void )                 ;
void        rri_GetMonResults     ( void )                 ;
bool        rri_EnhRepPossible    ( void )                 ;
void        rri_ResetNcMeasPar    ( void )                 ;
void        rri_RetrvPsi5Params   ( rr_ServPar_t * ServP ) ;
void        rri_ResetPccoInfo     ( void )                 ;
void        rri_CheckPcco         ( void )                 ;

void        rri_PutDynMeasPar     ( rri_DynMeasPar_t * MeasPar, rr_ServPar_t * ServP );
void        rri_TakeDynMeasPar    ( rr_ServPar_t * ServP, rri_DynMeasPar_t * MeasPar );
void        rri_ActOrderMeasPar   ( rr_Cell_t * Cell, rri_DynMeasPar_t * MeasPar );

//          rri_util

void        rri_ClearNCellParam    (rri_NCellParList_t * NCPL);
void        rri_ClearRmvNcFrqL     (rri_RmvNcFrqList_t * RmvL);

u8          rri_CellSleepIncr      ( u8 Dur )                 ;
bool        rri_UpdNonDrxMode      ( u8 Pos, bool Start )     ;
u8          rri_HandleInfoSchedule ( void )                   ;
void        rri_CheckSi13GMaCmplte ( rr_ServPar_t * ServP )   ;
void        rri_StoreInfo          ( void )                   ;
void        rri_SndPlmnFound       ( void )                   ;
void        rri_SndPlmnNasUpd      ( u8 NasUpdType )          ;
void        rri_SndPccoResult      ( u8 Result )              ;
bool        rri_RrWantsCbch        ( u8 CbchStatus )          ;
void        rri_ActivateCbch       ( void )                   ;
void        rri_StopCbch           ( rr_Cell_t * Cell )       ;
bool        rri_UpdL1CbchCfg       ( rr_Cell_t * Cell )       ;




//          rri_mm
void        rri_SendMmCell ( u8 Service )      ;
bool        rri_ResetXcv();
#endif

/*
================================================================================
  Prototypes for RRD 
--------------------------------------------------------------------------------

  Scope      : Functions used by RRD    

================================================================================
*/
#ifdef __RRD__
//            rrd_data
void          rrd_Init                ( void )                                 ;
void          rrd_InitAccess          ( u8 Type )                              ;
void          rrd_FreeL3InitMsg       ( void )                                 ;
void          rrd_FreeAccess          ( void )                                 ;

//            rrg_data
void          rrg_SetCurrFrqParam     ( l1_FreqParam_t ** ParamArray, u8 Idx ) ;
void          rrg_FreeL1Con           ( rrd_L1Con_t ** Ctx)                    ;

//            rrd_util 
void          rrd_SaveReqRef          ( u16 Ref, u16 RFN )                     ;
//add by lincq on 2007.10.10 begin
#ifdef __GPRS__
bool rrd_IsExRefForMe( c_TReqRef ReqRef , bool ExtRAPresent, u8 ExtRA);
#ifdef __EGPRS__
bool rrd_IsImmAssForMe ( c_IMM_ASS*ImmAss );
#endif
#endif
bool          rrd_IsRefForMe          ( c_TReqRef ReqRef )                     ;
//add by lincq on 2007.10.10 end
bool          rrd_IsImmAssRejForMe    ( void )                                 ;
bool          rr_FrqListOk            ( u16 * FrqList, u8 NbFreq )             ;
u8            rrd_ProcessImmAss       ( u8 RefIdx, bool TBF )                  ;
bool          rrd_RequestResel        ( u8 Cause )                             ;
void          rrd_SndRrCon            ( u8 ConnState )                         ;
void          rrd_SndRrNotCon         ( bool WithSysInfo )                     ;
void          rrd_L1RachOff           ( void )                                 ;
u8            rrd_StartRach           ( u8 IdleType )                          ;
void          rrd_EndRach             ( void )                                 ;
bool          rrd_IsSAccClassOK       ( u8 Ac )                                ;
bool          rrd_SimAccCtrlOK        ( u8 Ac[2] )                             ;
void          rrd_EndRrAccess         ( u8 MmCause )                           ;
void          rrd_EndRrAccessNormal(u8 MmCause);

/*add by lincq for the issue that t3168 exp evt can't be detected in case t3182 exp but happen in mailbox  on 2007.05.24 begin*/
bool 	      rrd_IsTimerExpEvtinMailbox (u32 Id, void *Data, u8 Mbx)	;
/*add by lincq for the issue that t3168 exp evt can't be detected in case t3182 exp but happen in mailbox  on 2007.05.24 end*/


//            rrg_util
void          rrg_TrcAMRCfg           ( l1_AMRCfg_t * AMRCfg )                 ;
void          rrg_TrcL1ConReq         ( l1_ConnectReq_t * Cmd )                ;
bool          rrg_AccCtrlOK           ( void )                                 ;
void          rrg_L1Release           ( void )                                 ;
void          rrg_LapRelease          ( u8 Sapi, bool NormalRelease )          ;
void          rrg_LapSuspend          ( void )                                 ;
void          rrg_LapResume           ( void )                                 ;
void          rrg_LapReconnect        ( void )                                 ;
void          rrg_SndHovFail          ( bool IntraHov )                        ;
void          rrg_SndCellChangeInd    ( rrd_L1Con_t * L1Cnx )                  ;
void          rrg_SndCmChange         ( void )                                 ;
void          rrg_SndMmRelInd         ( u8 MmCause )                           ;
void          rrg_SndMmCmkInd         ( void )                                 ;
void          rrg_SndChanMode2Api     ( u8 Mode )                              ;
void          rrg_SndChanMode2Cc      ( u8 Mode )                              ;
void          rrg_SndL1CellOpt        ( void )                                 ;
u8            rrg_LapSapiChan         ( u8 Sapi, u8 L1ChanType )               ;
void          rrg_UpdateNV_SD         ( u8 PD, u8 * MTAddr )                   ;
void          rrg_ForwardDataReq      ( Msg_t * DReq, u8 Sapi, u8 PD )         ;
bool          rrg_IsChanModeSupported ( u8 ChanMode )                          ;
void          rrg_PutCellOptions      ( l1_CellOpt_t * OutCmd, bool Tchf )     ;
void          rrg_PrepL1ConnectReq    ( void )                                 ;
rrd_L1Con_t * rrg_PrepImmAss          ( u8 RefIdx )                            ;
rrd_L1Con_t * rrg_PrepAssCmd          ( void )                                 ;
rrd_L1Con_t * rrg_PrepHovCmd          ( void )                                 ;
u8            rrg_EndRrConnection     ( u8 MmCause )                           ;
bool          rrg_IsCipAlgoSupported  ( u8 Algo )                              ;
void          rrg_SaveL1Param         ( l1_ConnectReq_t * Ctx )                ;
void          rrg_SavCA               ( c_TFrqList * CellChDesc )              ;
void          rrg_RetrieveCa          ( rr_CaL_t * CaSave )                    ;
u8            rrg_DecideHov           ( bool IntraHov )                        ;
bool          rrg_StoreSi6            ( rr_Cell_t * Cell, u8* Data )           ;
#ifdef __GPRS__
void          rrg_SuspendGprs         ( void )                                 ;
#endif

#ifdef __RRP__
u8            rrpVoid_TrsT3168Exp    (void)                                  ;
//            rrp_util
void          rrp_AbnormalRel         ( u8 Type )                              ;
u8            rrp_SendUlEstab         ( bool IsConnected )                     ;
bool          rrp_AccCtrlOK           ( u16 IsPccchPres, u8 RadioPrio )        ;
void          rrp_AccessFailure       ( void )                                 ;
u8            rrp_DlRelease           ( void )                                 ;
u8            rrp_UlRelease           ( bool RluRel )                          ;
u8            rrp_FdIdx               ( void * RefData, void * QueueData )     ;
bool          rrp_IsRefForMe          ( c_PReqRef * PReqRef )                  ;
void          rrp_SIATimerExp         ( void )                                 ;
u8            rrp_PurgeQ              ( void * Unused, void * Block )          ;
u8            rrp_BuildAllocBitmap    ( rrp_FixedBitmap_t * Cfg, bool ConI )   ;
void          rrp_SdPccchReqEvt       ( void )                                 ;
void          rrp_StartPRach          ( u8 RadioPrio, u8 Cause )               ;
void          rrp_PrepPL1ConI         ( void )                                 ;
void          rrp_SendReleaseInd      ( u8 MMCause )                           ;
void          rrp_FwdPcco             ( Msg_t* PccoMsg )                       ;
u8            rrp_SendUlEstabN        ( void )                                 ;
bool          rrp_LaunchRachProc      ( void )                                 ;
void          rrp_PCCOBack            ( void )                                 ;
void          rrp_ReleaseTBFs         ( u8 RluCause )                          ;
#ifdef __EGPRS__
void          rrp_StartEPRach          ( u8 RadioPrio, u8 Cause, u8 IdleType  )               ;
u8 		  rrp_GetMainAccTechType();
#endif
//            rrp_msg
 /*add by lincq 2007/02/06 for multislot cap check begin*/
u8 		  rrp_Allocation (l1_TBFCfgReq_t *TBFCfgReq);
 /*add by lincq 2007/02/06 for multislot cap check end*/

bool          rrp_HdlePImmAssDl       ( void )                                 ;
bool          rrp_FillCmnAssgt        ( l1_CmnAssgt_t * , u8, bool )           ;
bool          rrp_FillDlAssgt         ( void )                                 ;
bool          rrp_FillUlAssgt         ( bool )                                 ;
#ifdef __EGPRS__
bool          rrp_FillEgprsUlAssgt         ( bool )                                 ;
void          rrp_PreAMsCapMsg(bool IsReTx);
void 	  rrp_PrepPResReq ( bool TwoPhAcc , bool EgprsTbf);
#else
void          rrp_PrepPResReq         ( bool TwoPhAcc )                        ;
#endif
u8            rrp_HdleFreqParam       ( c_Freq_Param_IE *, l1_CmnAssgt_t * )   ;
#ifdef __EGPRS__
bool 	  rrp_HdlePUlAss ( bool ConI, bool Connected ,bool *Ignore);
#else
bool          rrp_HdlePUlAss          ( bool )                                 ;
#endif
bool          rrp_HdlePDlAss          ( void )                                 ;
void          rrp_HdleST              ( c_SFN_Desc_IE *, l1_CmnAssgt_t * )     ;
bool          rrp_HdlePTSReconf       ( void )                                 ;
void          rrp_HdleDynamic         ( c_Dynamic_Alloc_struct *, 
                                        l1_TBFCfgReq_t *, bool )               ;
bool          rrp_HdleGPTimAdv        ( c_GPTiming_Advance_IE *,
                                        l1_GPTimAdv_t * )                      ;
bool          rrp_HdlePTimAdv         ( c_PTiming_Advance_IE *,
                                        l1_GPTimAdv_t *, u8 )                  ;
void          rrp_HdlePwrCtrlParam    ( c_Pwr_Ctrl_Param_IE *, l1_PwrCtrl_t * );
void          rrp_HdleFixed           ( c_Fixed_Alloc_struct *, 
                                        l1_TBFCfgReq_t *, bool )               ;
void          rrp_SendPCCFail         ( u16, u8, u8 )                          ;
bool          rrp_HdlePCCO            ( bool )                                 ;

//            rrp_tim
void          rr_L1USFInd             ( void )                                 ;
void          rrp_RejPsTimExp         ( void )                                 ;
void          rrp_SIATimerExp         ( void )                                 ;

//            rrp_dec
u8            rr_StoreMa              ( ed_GprsMa_t *, u8 )                    ;
u8            rrp_StoreMa2            ( c_Freq_Param_IE_Dir_enc_2* )           ;
u8            rrp_FindIdxGMa          ( u8 )                                   ;

void          rrp_TestModeReq();

#endif

//            rrd_dec
u16           rrd_AiTime2Rfn          ( c_TStartingTime Time )                 ;
bool          rrd_FrqListOk           ( u16 * FrqList, u8 NbFreq )             ;
void          rrd_Acci2Wcni           ( void )                                 ;

//            rrg_dec
s8            rrg_DecChanType         ( l1_ConnectReq_t * OutCmd, u8 ChanType );
s8            rrg_DecMa               ( u16 * Dest, u8 * Src, u8 SrcSize )     ;
s8            rrg_DecFrqChnSeq        ( u16 * Dst, c_TFrqChnSeq * Src )        ;
bool          rrg_DecAMRCfg           ( l1_AMRCfg_t *Dsc, u8 *Src, u8 SrcSize  ,
                                        u8 ChanType ); 
#endif

// for single mode, the nSimId should be zero
extern u32 api_CheckTestSim(u32 nSimId);
extern bool sxs_SimFastAtr (void);
extern bool sxs_IsExtULTBFSupported(void);
#endif
