//==============================================================================
//                                                                              
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.              
//                            All Rights Reserved                               
//                                                                              
//      This source code is the property of Coolsand Technologies and is        
//      confidential.  Any  modification, distribution,  reproduction or        
//      exploitation  of  any content of this file is totally forbidden,        
//      except  with the  written permission  of  Coolsand Technologies.        
//                                                                              
//==============================================================================
//                                                                              
//    THIS FILE WAS GENERATED FROM ITS CORRESPONDING XML VERSION WITH COOLXML.  
//                                                                              
//                       !!! PLEASE DO NOT EDIT !!!                             
//                                                                              
//  $HeadURL$                                                                   
//  $Author$                                                                    
//  $Date$                                                                      
//  $Revision$                                                                  
//                                                                              
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _L1_MAP_H_
#define _L1_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================
/// Must be a power of 2. Max (5s, 5 Pch) = 5 / (2*235ms) = 10 (BsPaMfrms = 2, T
/// = 5 s, 1 measure / Pch)
#define L1_NB_MEAS_SERVING                       (16)
#define L1_NB_MAX_NEAR_CELLS                     (6)
#define L1_NB_MAX_OLD_CELLS                      (0)
#define L1_NB_MAX_PLMN_CELLS                     (12)
#define L1_NB_MAX_CELLS                          ((L1_NB_MAX_NEAR_CELLS + L1_NB_MAX_OLD_CELLS + L1_NB_MAX_PLMN_CELLS + 1))

// ============================================================================
// JOB_ENUM_H_ENUM_0_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef enum
{
    L1_POWER_JOB                                = 0x00000000,
    L1_MONITORING_JOB                           = 0x00000001,
    L1_NEAR_JOB                                 = 0x00000002,
    L1_CCH_JOB                                  = 0x00000003,
    L1_PCCH_JOB                                 = 0x00000004,
    L1_BCCH_JOB                                 = 0x00000005,
    L1_PBCCH_JOB                                = 0x00000006,
    L1_RACH_JOB                                 = 0x00000007,
    L1_CONNECT_JOB                              = 0x00000008,
    L1_HO_JOB                                   = 0x00000009,
    L1_HO_ACCESS_JOB                            = 0x0000000A,
    L1_UL_TBF_SETUP_JOB                         = 0x0000000B,
    L1_DL_TBF_SETUP_JOB                         = 0x0000000C,
    L1_PDCH_JOB                                 = 0x0000000D,
    L1_TBF_RELEASE_JOB                          = 0x0000000E,
    L1_PTCCH_JOB                                = 0x0000000F,
    L1_CBCH_JOB                                 = 0x00000010,
    L1_JOB_LAST                                 = 0x00000011
} JOB_ENUM_H_ENUM_0_T;

/// Maximum number of valid Arfcn in the Mobile Allocation
#define L1_MA_SIZE                               (64)
#define L1_NO_ICM                                (0XFF)
/// This parameter indicates the number of windows that can be handle in a frame.
/// The minimum value is three (1 Rx, 1 Tx and 1 Mx or 1 Rx and 2 Mx or 3 Mx).\n
/// For performance purpose, one additional monitoring window would be a plus especially
/// in GPRS and in a multi-band context to speed up the initial power measurement.
#define PAL_NB_MAX_WIN_PER_FRAME                 (3)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// L1_CELL_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT16                         Arfcn;                        //0x00000000
    UINT16                         Agc;                          //0x00000002
    UINT16                         PAgc;                         //0x00000004
    UINT8                          Bsic;                         //0x00000006
    UINT8                          Rla;                          //0x00000007
    INT8                           RTOf;                         //0x00000008
    UINT16                         PFct;                         //0x0000000A
    INT32                          TOf;                          //0x0000000C
    INT32                          FOf;                          //0x00000010
    INT16                          QbOf;                         //0x00000014
    INT32                          FnOf;                         //0x00000018
    UINT8                          RLChn;                        //0x0000001C
    UINT8                          RLInit;                       //0x0000001D
    UINT8                          RLCnt;                        //0x0000001E
    UINT8                          RLUp;                         //0x0000001F
    UINT8                          RLDwn;                        //0x00000020
    UINT8                          EvtCnt;                       //0x00000021
    UINT8                          Status;                       //0x00000022
    UINT8                          Update;                       //0x00000023
    UINT8                          Idx;                          //0x00000024
    UINT8                          Next;                         //0x00000025
    UINT8                          SpyIdx;                       //0x00000026
} L1_CELL_T; //Size : 0x28



// ============================================================================
// L1_SERVING_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    L1_CELL_T*                     Cell;                         //0x00000000
    UINT8                          BcchSdcchComb;                //0x00000004
} L1_SERVING_T; //Size : 0x8



// ============================================================================
// L1_GPWRCTRL_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          Alpha;                        //0x00000000
    UINT8                          TAvgW;                        //0x00000001
    UINT8                          TAvgT;                        //0x00000002
    UINT8                          Pb;                           //0x00000003
    UINT8                          PCMeasChan;                   //0x00000004
    UINT8                          NAvgI;                        //0x00000005
    UINT8                          TxPwrMax;                     //0x00000006
} L1_GPWRCTRL_T; //Size : 0x7



// ============================================================================
// L1_PMEASCTRLREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef L1_GPWRCTRL_T L1_PMEASCTRLREQ_T;


// ============================================================================
// L1_CCHREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT16                         Arfcn;                        //0x00000000
    UINT8                          BcchSdcchComb;                //0x00000002
    UINT8                          BsAgBlkRes;                   //0x00000003
    UINT8                          BsPaMfrms;                    //0x00000004
    UINT8                          Group;                        //0x00000005
    UINT8                          Tn;                           //0x00000006
    UINT8                          Mode;                         //0x00000007
} L1_CCHREQ_T; //Size : 0x8



// ============================================================================
// L1_FREQPARAM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          NbArfcn;                      //0x00000000
    UINT8                          Maio;                         //0x00000001
    UINT8                          Hsn;                          //0x00000002
    UINT8                          Pad;                          //0x00000003
    UINT16                         MA[L1_MA_SIZE];               //0x00000004
} L1_FREQPARAM_T; //Size : 0x84



// ============================================================================
// L1_PCCHREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT16                         Arfcn;                        //0x00000000
    L1_FREQPARAM_T                 FreqParam;                    //0x00000002
    UINT8                          MultiframeType;               //0x00000086
    UINT8                          Tn;                           //0x00000087
    UINT8                          Tsc;                          //0x00000088
    UINT8                          AgBlkRes;                     //0x00000089
    UINT8                          PBcchBlks;                    //0x0000008A
    UINT8                          BcchSdcchComb;                //0x0000008B
    UINT8                          SplitPagingCycle;             //0x0000008C
    UINT8                          Mode;                         //0x0000008D
    UINT16                         Group;                        //0x0000008E
    UINT8                          Pb;                           //0x00000090
} L1_PCCHREQ_T; //Size : 0x92



// ============================================================================
// L1_RACHONREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT16                         RachInfo;                     //0x00000000
    UINT8                          Type;                         //0x00000002
    UINT8                          IdleType;                     //0x00000003
    UINT8                          RandSize;                     //0x00000004
    UINT8                          TxPower;                      //0x00000005
    UINT8                          S;                            //0x00000006
    UINT8                          T;                            //0x00000007
    /// 10.5.2.29, 4.60 12.14
    UINT8                          NbMaxRetrans;                 //0x00000008
    UINT8                          PersistenceLevel;             //0x00000009
    UINT8                          RadioPrio;                    //0x0000000A
    UINT8                          Pad;                          //0x0000000B
} L1_RACHONREQ_T; //Size : 0xC



// ============================================================================
// L1_CIPHPARAM_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          Kc[8];                        //0x00000000
    UINT8                          Algo;                         //0x00000008
    BOOL                           Start;                        //0x00000009
    UINT8                          Pad[2];                       //0x0000000A
} L1_CIPHPARAM_T; //Size : 0xC



// ============================================================================
// L1_CELLOPT_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    BOOL                           Pwrc;                         //0x00000000
    UINT8                          Rlt;                          //0x00000001
    UINT8                          Dtx;                          //0x00000002
    UINT8                          Pad;                          //0x00000003
} L1_CELLOPT_T; //Size : 0x4



// ============================================================================
// L1_AMRCFG_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    BOOL                           AMRCfgValid;                  //0x00000000
    UINT8                          Version;                      //0x00000001
    UINT8                          NoiseSuppression;             //0x00000002
    UINT8                          StartModeIdx;                 //0x00000003
    /// be used. When value is L1_NO_ICM initial codec selection is done according
    /// to 05.09 requirement.
    UINT8                          ACS;                          //0x00000004
    UINT8                          Threshold[3];                 //0x00000005
    UINT8                          Hysteresis[3];                //0x00000008
} L1_AMRCFG_T; //Size : 0xB



// ============================================================================
// L1_CONNECTREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    L1_FREQPARAM_T*                FreqParam[2];                 //0x00000000
    /// - immediate action only ([0]) or - later action only ([0]) or - both ([0]
    /// and [1])
    L1_CIPHPARAM_T                 CipherParam;                  //0x00000008
    L1_CELLOPT_T                   CellOptions;                  //0x00000014
    L1_AMRCFG_T                    AMRCfg;                       //0x00000018
    UINT16                         ST;                           //0x00000024
    /// L1_NO_ST = non significant
    UINT16                         Arfcn;                        //0x00000026
    UINT8                          ChanType;                     //0x00000028
    /// champs (ChanType et TdmaOffset)
    UINT8                          TdmaOffset;                   //0x00000029
    UINT8                          TN;                           //0x0000002A
    UINT8                          Tsc;                          //0x0000002B
    UINT8                          PowerLevel;                   //0x0000002C
    UINT8                          ChanMode;                     //0x0000002D
    UINT8                          TA;                           //0x0000002E
    /// Assignment command and Handover command specific fields
    UINT8                          HoType;                       //0x0000002F
    BOOL                           Ctrl;                         //0x00000030
    /// Handover command specific fields
    UINT8                          HORef;                        //0x00000031
    BOOL                           Nci;                          //0x00000032
    BOOL                           ATC;                          //0x00000033
    /// mandatory; 3GPP TS 4.18: if new cell supports extended TA and TA &amp;gt;
    /// 63 and ATC == TRUE, MS must not send HO access
    UINT8                          RTD;                          //0x00000034
} L1_CONNECTREQ_T; //Size : 0x38



// ============================================================================
// L1S_TACTX_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          Previous;                     //0x00000000
    UINT8                          Current;                      //0x00000001
    UINT8                          Next;                         //0x00000002
} L1S_TACTX_T; //Size : 0x3



// ============================================================================
// PAL_AMRCFG_T
// -----------------------------------------------------------------------------
/// AMR Configuration structure
// =============================================================================
typedef struct
{
    UINT8                          Version;                      //0x00000000
    UINT8                          NoiseSuppression;             //0x00000001
    UINT8                          StartModeIdx;                 //0x00000002
    UINT8                          NbActiveCodecSet;             //0x00000003
    UINT8                          ActiveCodecSet[4];            //0x00000004
    UINT8                          Hysteresis[3];                //0x00000008
    UINT8                          Threshold[3];                 //0x0000000B
    UINT8                          CMIPhase;                     //0x0000000E
} PAL_AMRCFG_T; //Size : 0xF



// ============================================================================
// L1_MEASCFG_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT16                         NbMeasTgt;                    //0x00000000
    UINT16                         NbMeasMax;                    //0x00000002
    UINT16                         ReadIdx;                      //0x00000004
    UINT16                         RunningAverageThres;          //0x00000006
    UINT8                          NbCells;                      //0x00000008
    UINT8                          NbMonPerFr;                   //0x00000009
    UINT8                          PacketMeas;                   //0x0000000A
    UINT8                          NbExtraMon;                   //0x0000000B
    /// u8 PcMeasBcch; Power control measurement on Bcch (PcMeasChan = false).
    UINT8                          ServingIdx;                   //0x0000000C
    BOOL                           Tag;                          //0x0000000D
    BOOL                           ExtendedMeas;                 //0x0000000E
    UINT8                          NcMeasState;                  //0x0000000F
    UINT8                          NcRPIdle;                     //0x00000010
    UINT8                          NcRPTransfer;                 //0x00000011
    UINT8                          NcCurrentRp;                  //0x00000012
    UINT8                          LastNcInTransfer;             //0x00000013
    UINT32                         FrmSchd;                      //0x00000014
    UINT16*                        ArfcnL;                       //0x00000018
    UINT16*                        WriteIdx;                     //0x0000001C
    UINT32*                        NcMeasCumul;                  //0x00000020
    UINT16*                        NbNcMeas;                     //0x00000024
    UINT8*                         Meas;                         //0x00000028
} L1_MEASCFG_T; //Size : 0x2C



// ============================================================================
// L1_SERVMEASCFG_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          PwrC;                         //0x00000000
    UINT8                          NbMeas;                       //0x00000001
    UINT8                          NbMeasSub;                    //0x00000002
    UINT8                          MeasMax;                      //0x00000003
    UINT8                          MeasMin;                      //0x00000004
    UINT8                          MeasSubMax;                   //0x00000005
    UINT8                          MeasSubMin;                   //0x00000006
    UINT16                         MeasCumul;                    //0x00000008
    UINT16                         MeasCumulSub;                 //0x0000000A
    UINT16                         TotalBit;                     //0x0000000C
    UINT16                         TotalBitSub;                  //0x0000000E
    UINT16                         WrongBitCumul;                //0x00000010
    UINT16                         WrongBitCumulSub;             //0x00000012
    UINT8                          NbMeasTgt;                    //0x00000014
    UINT8                          ReadIdx;                      //0x00000015
    UINT8                          WriteIdx;                     //0x00000016
    UINT8                          Meas[L1_NB_MEAS_SERVING];     //0x00000017
    UINT16                         BlkMeasCumul;                 //0x00000028
    UINT8                          NbBstMeas;                    //0x0000002A
    UINT8                          NbBstBcch;                    //0x0000002B
    UINT8                          BfiBitMap;                    //0x0000002C
    UINT8                          PcMeasBcch;                   //0x0000002D
    UINT16                         n;                            //0x0000002E
    UINT16                         F;                            //0x00000030
    UINT16                         vF;                           //0x00000032
    UINT32                         C;                            //0x00000034
    UINT8                          TxPower[8];                   //0x00000038
    UINT8                          BstMeas[4];                   //0x00000040
    UINT16                         BlVarCumul;                   //0x00000044
    UINT8                          NbBlVar;                      //0x00000046
    UINT16                         Int[8];                       //0x00000048
    UINT8                          Intn[8];                      //0x00000058
    UINT16                         IntF;                         //0x00000060
    UINT8                          IntvF;                        //0x00000062
    BOOL                           ExtIntRequest;                //0x00000063
    BOOL                           NextExtInt;                   //0x00000064
    UINT16                         ExtIntArfcn;                  //0x00000066
    UINT16                         ExtInt[8];                    //0x00000068
    UINT8                          ExtIntn[8];                   //0x00000078
    UINT8                          IntMeas[2*4];                 //0x00000080
    UINT16                         IntArfcn[2];                  //0x00000088
} L1_SERVMEASCFG_T; //Size : 0x8C



// ============================================================================
// L1_CBCHREQ_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          BitMap;                       //0x00000000
    /// extended to be listened.
    UINT8                          Tn;                           //0x00000001
    UINT8                          Tsc;                          //0x00000002
    UINT8                          Pad;                          //0x00000003
    L1_FREQPARAM_T                 FreqParam;                    //0x00000004
} L1_CBCHREQ_T; //Size : 0x88



// ============================================================================
// WINALLOC_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          Win[PAL_NB_MAX_WIN_PER_FRAME]; //0x00000000
    UINT8                          Idx;                          //0x00000003
    UINT8                          NbWin;                        //0x00000004
} WINALLOC_T; //Size : 0x5



// ============================================================================
// S16PAIR_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef INT16 S16PAIR_T[2];


// ============================================================================
// L1_EQURES_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          WrIdx;                        //0x00000000
    /// u8 RdIdx;
    UINT8                          Pad[3];                       //0x00000001
    UINT32                         Pwr[2];                       //0x00000004
    UINT32                         Snr[2];                       //0x0000000C
    UINT32                         TOf[2];                       //0x00000014
    UINT32                         Agc[2];                       //0x0000001C
    S16PAIR_T                      FOf[4];                       //0x00000024
    S16PAIR_T                      Arfcn[4];                     //0x00000034
} L1_EQURES_T; //Size : 0x44



// ============================================================================
// L1_CTX_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
typedef struct
{
    UINT8                          State;                        //0x00000000
    UINT8                          AMR;                          //0x00000001
    UINT16                         SubState;                     //0x00000002
    UINT32                         FrameLoadBitMap;              //0x00000004
    L1_CELL_T                      Cell[L1_NB_MAX_CELLS];        //0x00000008
    L1_SERVING_T                   Serving;                      //0x00000300
    MSG_T*                         MonitorReq;                   //0x00000308
    MSG_T*                         MemMonitorReq;                //0x0000030C
    MSG_T*                         ExtdMonitorReq;               //0x00000310
    MSG_T*                         TxDiscardReq;                 //0x00000314
    MSG_T*                         NextTxDiscardReq;             //0x00000318
    L1_PMEASCTRLREQ_T*             PMeasCtrlReq;                 //0x0000031C
    L1_CCHREQ_T*                   CchReq;                       //0x00000320
    L1_PCCHREQ_T*                  PCchReq;                      //0x00000324
    MSG_T*                         PBcchReq[2];                  //0x00000328
    /// Msg_t *NcMonitorReq; To be taken into account at the beginning of the next
    /// RP.
    L1_RACHONREQ_T*                RachOnReq;                    //0x00000330
    L1_CONNECTREQ_T*               ConnectReq;                   //0x00000334
    L1S_TACTX_T*                   TACtx;                        //0x00000338
    L1_TBFCTX_T*                   TBFCtx;                       //0x0000033C
    PAL_AMRCFG_T*                  AMRCfg;                       //0x00000340
    L1_MEASCFG_T                   MeasCfg;                      //0x00000344
    L1_SERVMEASCFG_T               ServMeasCfg;                  //0x00000370
    L1_CBCHREQ_T*                  CbchReq;                      //0x000003FC
    UINT8                          CbchSkipCnt[2];               //0x00000400
    UINT8                          CbchSkipBitMap;               //0x00000402
    UINT8                          OldTA;                        //0x00000403
    UINT8                          PSTxBitMap;                   //0x00000404
    UINT8                          NbPSJob;                      //0x00000405
    UINT8                          PDataPurgeReq;                //0x00000406
    UINT16                         NbGPMeaSkip;                  //0x00000408
    UINT8                          FreeCell;                     //0x0000040A
    UINT8                          ActiveCell;                   //0x0000040B
    UINT8                          AfcIdx;                       //0x0000040C
    UINT8                          AfcLocked;                    //0x0000040D
    UINT8                          GainOffset;                   //0x0000040E
    UINT8                          Job[L1_JOB_LAST];             //0x0000040F
    UINT8                          CellList[3];                  //0x00000420
    VOLATILE UINT32                Fn;                           //0x00000424
    VOLATILE UINT16                RFn;                          //0x00000428
    VOLATILE UINT8                 T1R;                          //0x0000042A
    VOLATILE UINT8                 T2;                           //0x0000042B
    VOLATILE UINT8                 T3;                           //0x0000042C
    VOLATILE UINT8                 C52;                          //0x0000042D
    VOLATILE UINT8                 C102;                         //0x0000042E
    VOLATILE UINT8                 C104;                         //0x0000042F
    WINALLOC_T                     WinAlloc;                     //0x00000430
    UINT8                          Tn;                           //0x00000435
    UINT8                          Sleep;                        //0x00000436
    UINT8                          SleepEnable;                  //0x00000437
    UINT8                          NearMode;                     //0x00000438
    UINT8                          DataBlockSize;                //0x00000439
    UINT8                          BandMap;                      //0x0000043A
    UINT8                          SpyServingId;                 //0x0000043B
    UINT8                          SpyNearId;                    //0x0000043C
    L1_EQURES_T                    EquRes[5];                    //0x00000440
} L1_CTX_T; //Size : 0x594





#endif

