/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2004-2006

================================================================================
*/

/*
================================================================================
 File          l1.h
--------------------------------------------------------------------------------

  Scope      : L1 Header file.

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  |  ADA  | Creation
  Aug 31 04  |  ADA  | Equalisation results in l1_Ctx.
  Oct 13 04  |  ADA  | Add Tfi in TBF ctx.
  Feb 07 06  |  JFN  | Add NextTxDiscardReq for simultaneous request
  Feb 07 06  |  JFN  | Add NbPDAckNackBeforePMR in TBF ctx.
  Mar 15 06  |  JFN  | Add NcMeasState
================================================================================
*/

#ifndef __L1_H__
#define __L1_H__

#include "sxs_type.h"
#include "l1a_msg.h"
#include "pal_gsm.h"
#include "l1s_spy.h"
#include "cmn_defs.h"

#ifdef __L1_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define L1_ENCODE_ANTICIPATION 1
#define L1_CELL_EQ_CHECK

#define L1_NO_STATE        0
#define L1_CELL_SEL_STATE  (1 << 0)
#define L1_IDLE_STATE      (1 << 1)
#define L1_CONNECTED_STATE (1 << 2)
#define L1_TRANSFER_STATE  (1 << 3)

#if defined( STK_MAX_SIM ) && (STK_MAX_SIM >1) 
#define L1_SIMID_MASK   0x3
#define L1_SIMID_POSITION  13
#define L1_SIMID_TAG (1<<15)
#define SET_SIMID(id)  ((u32)(L1_SIMID_TAG | ((id)&L1_SIMID_MASK)<< L1_SIMID_POSITION))
#define GET_SIMID( a )    ((u32)((a & (L1_SIMID_MASK<< L1_SIMID_POSITION)) >> L1_SIMID_POSITION))

#define L1_NC_MEA_SIMID_MSK   ((u32)3)
#define L1_NC_MEA_TIM_ID ((u32)(1 << 31))
#endif

#ifdef __L1_DUMMY_DETECTION__
#define L1_DUMMY_DETECT_QBOF  PAL_DUMMY_DETECT_QBOF
#if (STK_MAX_SIM < 3)
#define L1_DUMMY_DETECT_TOLERANCE  100  //with tolerance of 100Qb
#else
#define L1_DUMMY_DETECT_TOLERANCE  0  //with tolerance of 100Qb
#endif

#define L1_DUMMY_DETECT_MAX_QBOF PAL_DUMMY_DETECT_MAX_QBOF + L1_DUMMY_DETECT_TOLERANCE// less than [5000 - 1000(equ+dummy detect)-625-450 = 2900]
#endif

#ifdef __L1_CCH_DEC_DONE_IRQ_EN__
#define L1_DEC_DONE_IRQ_MAX_QBOF  100//change to very small, to resolve crash when dec done irq is later and L1 can not finish other jobs before next FINT, 20161228
#endif
#define L1_RESET_ALL_SUB_STATE      ((u16)0)
#define L1_NO_SUB_STATE             ((u16)0)
#define L1_INITIAL_POWER_SUB_STATE  ((u16)(1 << 1))
#define L1_FULL_CCCH_SUB_STATE      ((u16)(1 << 2))
#define L1_PAGING_REORG_SUB_STATE   ((u16)(1 << 3))
#define L1_RACH_SUB_STATE           (u16)(((u16)(1 << 4)) | L1_FULL_CCCH_SUB_STATE)
#define L1_SDCCH_SUB_STATE          ((u16)(1 << 5))
#define L1_TCH_SUB_STATE            ((u16)(1 << 6))
#define L1_P_PAGING_REORG_SUB_STATE ((u16)(1 << 7))
#define L1_NON_DRX_SUB_STATE        ((u16)(1 << 8))
#define L1_P_ACCESS_SUB_STATE       ((u16)(1 << 9))
#define L1_T_SINGLE_SUB_STATE       ((u16)(1 << 9))
#define L1_PACKET_SUB_STATE         ((u16)(1 << 15))


#define L1_FN_MAX  2715648
#define L1_RFN_MAX   42432

#define L1_NB_MAX_NEAR_CELLS  6
#define L1_NB_MAX_OLD_CELLS   0
#define L1_NB_MAX_PLMN_CELLS  12
#define L1_NB_MAX_CELLS  (L1_NB_MAX_NEAR_CELLS + L1_NB_MAX_OLD_CELLS + L1_NB_MAX_PLMN_CELLS + 1)


/* Defines for cell Status field. */

#define L1_CELL_FREE    0         // Cell is free...
#define L1_NEAR_CELL    (1 << 0)  // Near cell: Sch + Bcch sy 3 or 4 due periodically.
#define L1_NO_BCCH_CELL (1 << 1)  // Near cell: Sch only due periodically.
#define L1_EXT_BCCH     (1 << 2)  // For a near or a Plmn cell, when Bcch sy 4 is received, received the associated extended Bcch (sy 7 or 8).
#define L1_PLMN_CELL    (1 << 3)  // Plmn cell: Sch + Bcch 3 or 4 due one time.
#define L1_OLD_CELL     (1 << 4)  // Old cell: nothing is due, just maintain cel time as long as possible.
#define L1_SERVING_CELL (1 << 5)  // Serving cell: action depend on state and RR requirement.

#define L1_SCH_UPDATE_REQ (1 << 6) //non base cell Qbof and FnOf update. added by zhanghy. 2010-01-07

/* Defines for cell Update field. */

#define L1_NEAR_UPDATED     0     // Near cell is updated, no action required.
#define L1_NEAR_ACTION_REQ  1     // Action on cell is required.
#define L1_NEAR_PRIORITY    2     // Action on cell is required with priority.

#define L1_NB_MEAS_SERVING      16  // Must be a power of 2. Max (5s, 5 Pch) = 5 / (2*235ms) = 10   (BsPaMfrms = 2, T = 5 s, 1 measure / Pch)
#define L1_NB_MEAS_SERVING_MSK  (L1_NB_MEAS_SERVING - 1)

/* Defines for near management purpose. */

#define L1_FREE_IDLE_FRAME  NIL          // The coming Idle frame is free for near action.
#define L1_BUSY_IDLE_FRAME  ((void *)1)  // The coming Idle frame is busy.
#define L1_PTCCH_FRAME      ((void *)2)  // The coming Ptcch frame is free for near action.

#define L1_NEAR_CELL_FOF_THRESHOLD_UPPER (2000)
#define L1_NEAR_CELL_FOF_THRESHOLD_LOWER (-2000)
#define L1_NEAR_CELL_FOF_SMALL_POSITIVE  (200)
#define L1_NEAR_CELL_FOF_SMALL_NEGTIVE  (-200)
typedef struct
{
 u8  WrIdx;
// u8  RdIdx;
 u8  Pad [3];
 u32 Pwr [2];
 u32 Snr [2];
 u32 TOf [2];
 u32 Agc [2];
 s16 FOf [2][4];
 s16 Arfcn [2][4];
} l1_EquRes_t;

typedef struct
{
 u16 Arfcn;
 u16 Agc;
 u16 PAgc;      // Packet Agc. Use for Pdch in packet transfer mode only (no dlw pwr ctrl).
 u8  Bsic;
 u8  Rla;
#if defined( STK_MAX_SIM ) && (STK_MAX_SIM >1) 
 u32 SimId;     // dedicated for 2C2D1T prj.
#endif

 s8  RTOf;      // Residual Timing offset
 u16 PFct;      // Ponderation factor acumulator
 s32 TOf;       // Timing offset acumulator
 s32 FOf;       // Frequency offset acumulator

 s16 QbOf;      // Quarter bit offset
 s32 FnOf;      // Frame number offset

 u8  RLChn;     // Channel for Radio link management.
 u8  RLInit;    // Radio link counter initial value.
 u8  RLCnt;     // Radio link counter.
 u8  RLUp;      // Up value for radio link counter.
 u8  RLDwn;     // Down value for radio link counter.

 u8  EvtCnt;    // Generic counter (Sch successful reception for near cell,..)
 u8  Status;    // Cell status (serving, near, plmn, old).
 u8  Update;    // For a near cell, update indicates if an update of FCH/SCH/BCCH is to be done with the priority.
 u8  SynchUpdtPending;


 u8  Idx;       // Link purpose data.
 u8  Next;
 u8  SpyIdx;    // Near spy index for debug purpose.

} l1_Cell_t;



typedef struct
{
 l1_Cell_t *Cell;
 u8 BcchSdcchComb;

} l1_Serving_t;


#define L1_NO_CELL 0xFF

#define L1_NEAR_LIST 0
#define L1_OLD_LIST  1


// convert abs dB power into RxLev value
#define L1_PWR2RXLEV(Pwr)  ( ((Pwr) >= 110) ? 0 : ((Pwr) <= 47) ? 63 : 110 - (Pwr) )

#if defined( STK_MAX_SIM ) && (STK_MAX_SIM >1) 
enum
{
 L1_POWER_JOB,
 L1_MONITORING_JOB,
 L1_MONITORING_B_JOB,
#if (STK_MAX_SIM >= 3)
 L1_MONITORING_C_JOB, 
#endif
#if (STK_MAX_SIM >= 4)
 L1_MONITORING_D_JOB,
#endif
 L1_NEAR_JOB,
 L1_CCHCONTROL_JOB,
 L1_CCH_JOB,
 L1_CCHB_JOB,
#if (STK_MAX_SIM >= 3)
 L1_CCHC_JOB,
#endif
#if (STK_MAX_SIM >= 4)
 L1_CCHD_JOB,
#endif
 L1_PCCH_JOB,
 L1_BCCH_JOB,
 L1_BCCHB_JOB,
#if (STK_MAX_SIM >= 3)
 L1_BCCHC_JOB,
#endif
#if (STK_MAX_SIM >= 4)
 L1_BCCHD_JOB,
#endif
 L1_PBCCH_JOB,
 L1_RACH_JOB,
 L1_CONNECT_JOB,
 L1_HO_JOB,
 L1_HO_ACCESS_JOB,
 L1_UL_TBF_SETUP_JOB,
 L1_DL_TBF_SETUP_JOB,
 L1_PDCH_JOB,
 L1_TBF_RELEASE_JOB,
 L1_PTCCH_JOB,
 L1_CBCH_JOB,				// 20
 L1_CBCHB_JOB,				// 21
 L1_JOB_LAST
};
#else
enum
{
 L1_POWER_JOB,
 L1_MONITORING_JOB,
 L1_NEAR_JOB,
 L1_CCH_JOB,
 L1_PCCH_JOB,
 L1_BCCH_JOB,
 L1_PBCCH_JOB,
 L1_RACH_JOB,
 L1_CONNECT_JOB,
 L1_HO_JOB,
 L1_HO_ACCESS_JOB,
 L1_UL_TBF_SETUP_JOB,
 L1_DL_TBF_SETUP_JOB,
 L1_PDCH_JOB,
 L1_TBF_RELEASE_JOB,
 L1_PTCCH_JOB,
 L1_CBCH_JOB,
 L1_JOB_LAST
};
#endif

#define L1_NEAR_MODE_FULL   0  // Used in Selection, Idle and Sdcch.
#define L1_NEAR_MODE_SLICED 1  // Used in Tch and Transfer mode.
#define L1_NEAR_MODE_SUSPENDED 0xFF

#define L1_PBCCH_CURRENT_CFG 0 // Last PBcch request provided by RR.
#define L1_PBCCH_SERVING_CFG 1 // PBcch Cfg memorized for the serving cell.

// left shift for stored values for computation resolution
#define L1_CVALUE_SHIFT 15
#define L1_ALPHA_SHIFT  10
#define L1_INT_SHIFT    8
#define L1_AGC_SHIFT    4
#ifdef __EGPRS__
#define L1_BEP_NVALID 0xFF
#define L1_BEP_SHIFT 14		//for Mean_Bep shit
#define L1_E_SHIFT 10		//for e, Rn

typedef struct _u64 
{
	unsigned int high;
	unsigned int low;
} u64;
#endif
typedef struct
{
 u8 Previous;
 u8 Current;
 u8 Next;
} l1s_TACtx_t;

typedef struct
{
 u8 Previous;
 u8 Current;
 u8 Target;
 u8 Next;
} l1s_TxPwrCtx_t;

#define L1_NB_MAX_STORED_MEAS 1100

typedef struct
{
 u16  NbMeasTgt;   // Number of measures per cell to be done for each cycle.
 u16  NbMeasMax;   // Number of measures Maximum per cell = capacity of Meas array.
 u16  ReadIdx;     // Start index for measure results analyse.
 u16  RunningAverageThres; // Nb of total measurement before each average
 u8   NbCells;     // Number of cells to be considered.
 u8   NbMonPerFr;  // Number of monitoring windows per frame.
 u8   PacketMeas;  // Boolean indicating when the packet measure must be handled.
 u8   NbExtraMon;  // Number of monitoring windows to add per (P)Pch period.
// u8   PcMeasBcch;  // Power control measurement on Bcch (PcMeasChan = false).
 u8   ServingIdx;  // Index of the serving cell in the BA list.
 bool Tag;         // RR measure Tag.
 bool ExtendedMeas;// Indicates if the context corresponds to the extended list.
#ifndef __NO_GPRS__
 u8   NcMeasState;
 u8   NcRPIdle;    // NC measurement Reporting Period for Idle state. Expressed in 480 ms period [1..128].
 u8   NcRPTransfer;// NC measurement Reporting Period for Transfer state. Expressed in 480 ms period [1..128].
 u8   NcCurrentRp; // NC measurement Reporting Period in use. Expressed in 480 ms period [1..128].
 u8   LastNcInTransfer;
#endif
 u32  FrmSchd;     // Frame schedule for monitoring set by the main
 u16 *ArfcnL;      // Pointer to Arfcn list of MonitoringReq message.
 u16 *WriteIdx;    // Current write index for each cell.
#ifndef __NO_GPRS__
 u32 *NcMeasCumul; // Array containing Nc cumul measurement for each Nc cell.
 u16 *NbNcMeas;    // Array containing the number of measures in NcMeas array.
#endif
 u8  *Meas;        // Array where intermediate measure results are stored for each cell.
} l1_MeasCfg_t;

typedef struct
{
 u8  PwrC;             // Power control is active (GSM connection).
 u8  NbMeas;           // Nb Measures held in current MeasFull accumulator
 u8  NbMeasSub;        // Nb Measures held in current MeasSub accumulator
 u8  MeasMax;
 u8  MeasMin;
 u8  MeasSubMax;
 u8  MeasSubMin;
 u16 MeasCumul;        // Measure full accumulator.
 u16 MeasCumulSub;     // Measure Sub accumulator, Connected state only.

 u16 SNRCumul;
  
 u16 TotalBit;         // Number of received bits accumulator for RxQualFull calculation.
 u16 TotalBitSub;      // Number of received bits accumulator for RxQualSub calculation.
 u16 WrongBitCumul;    // Number of wrong bits received accumultor for RxQualFull calculation.
 u16 WrongBitCumulSub; // Number of wrong bits received accumultor for RxQualSub calculation.

 u8  NbMeasTgt;        // Target number of measures.
 u8  ReadIdx;          // Start index for measure results analyse.
 u8  WriteIdx;         // Current write index position in Meas array.
 u8  Meas [L1_NB_MEAS_SERVING]; // Intermediate measurement storage for serving cell.

 u16 BlkMeasCumul;     // Burst by burst cumul for C value calculation.
 u8  NbBstMeas;        // Number of samples cumulated in BlkMeas.
 u8  NbBstBcch;        // Number of samples taken on serving Bcch carrier.
 u8  BfiBitMap;        // Bad frame indicator bit map for the last received multislot blocks

 u8  PcMeasBcch;       // Power control measurement on Bcch (PcMeasChan = false).
 u16 n;                // Iteration for C value calculation.
 u16 F;                // Forgetting factor for C value calculation (a, b or c depending of the state). * (1 << 10)
 u16 vF;               // 1/F for C calculation in Idle.
 u32 C;                // C value. * (1 << 10)

 u8  TxPower [8];      // Tx Power to be applied on each burst of the cell.
 u8  TA  ;             // new TA in TCH, SDCCH job, to report to RR   

 u8  BstMeas [4];      // Measure for block variance calculation.
 u16 BlVarCumul;       // Block variance cumul.
 u8  NbBlVar;          // Number of samples in block variance cumul.

#ifndef __NO_GPRS__
 u16 Int[8];            // Interference value in abs dBm * 2^(L1_INT_SHIFT)
 u8  Intn[8];           // Interference number of iteration for initial forgetting factor
 u16 IntF;              // Interference forgetting factor * 2^(L1_INT_SHIFT)
 u8  IntvF;             // real Navg_I for initial filter threshold
 bool ExtIntRequest;    // Extended Int to be done
 bool NextExtInt;       // Next measurement on Extended Arfcn
 u16 ExtIntArfcn;       // Arfcn where Extended interference measurement should be done
 u16 ExtInt [8];        // Extended Interference value in abs dBm * 2^(L1_INT_SHIFT)
 u8  ExtIntn [8];       // Extended Interference number of iteration for initial forgetting factor.
 u8  IntMeas [2 * 4];   // Storage for the current interference mesaures.
 u16 IntArfcn [2];      // Arfcns used for the last interference measures.
#ifdef __EGPRS__
u8 MBepChValidBitMap; 
u8 MBepChTypeBitMap;
 
u16 BepE;			// the forgetting factor, E
u16 BepRn[2][8];			// Rn, the reliability of the filtered quality parameters.
u32 MeanBepTn[2][8];		// Gmsk mode MeanBep value averaged per channel (timeslot)
u16 CvBepTn[2][8];		//Gmsk CvBep value averaged per channel (timeslot)
u16 NbBlockTn[2][8];		// Record the number of blocks received per channel.
						//NbBlockTn[0] is for Gmsk, NbBlockTn[1] is for 8psk.
#endif //EGPRS
#endif //GPRS
} l1_ServMeasCfg_t;

typedef struct
{
 u8  RxAlloc;
 u8  TxAlloc;
 u8  UlRxAlloc;
 u8  NbRx;
 u8  NbTx;
 u8  NbRxTx;
 u8  FirstRx;
 u8  FirstTx;
 u8  LastRx;
 u8  LastTx;
 u16 NbBlocks;
} l1_TBFAlloc_t;

enum {L1_PDCH_IDX, L1_PACCH_IDX, L1_PSDATA_IDX, L1_NB_PBLOCK_TYPE};

typedef struct
{
 u8  Tn [4];                    // Timelsot number value for each buffer allocated for RRBP.
 u8  NbBlk [L1_NB_PBLOCK_TYPE]; // Number of blocks allocated for RRBP, PACCH, PDCH.
 u8  Idx;                       // First buffer available.
} l1_TxBuffer_t;

typedef struct
{
 u8  BlkIndex;
 u8  TsOverRide;   // Used to over ride the current timeslot allocation.

 u8  Repeat;       // Repeat the allocation ?
} l1_FixedUpdt_t;

#define L1_PURGE_ALL_PDATA  (1<<0)
#define L1_PURGE_TX_DISCARD (1<<1)

typedef struct
{
 u8             DataQueue [L1_NB_PBLOCK_TYPE - 1];
 u8             TsAlloc [2];   // For Ul and Dl.
 u8             MacMode;
 u8            UsfDone;
#ifdef __EGPRS__
 u8 		TbfMode[2];	// used in EGPRS
 u8		IrBuffStatus;	// used in EGPRS mode. indicates the status of the buffer for IR. added by zhanghy. 2006-11-14
 u8          DecodeDone;
#endif
 u8             Tfi[2];
 u8             Tsc;
 u8             FixedAllocSuspended;  // Indicates that the fixed allocation is suspended.
 u8             PDAckNackTransmitted; // Indicates if the last polling answer was an Ack/Nack.
 u8             NbPDAckNackBeforePMR; // Indicates how many PDAN should send before PMR
 u8             DlPwrCtrlAllowed;     // Indicates when if the downling power control is allowed in transfer mode.
 u8             TxPdchCnt [4]; // For extended dynamic Mac, block to be transmitted.
                               // For Fixed Mac, Cnt 0 is used to index the block period in the allocation.
 u8  GprsTestMode;
                               
 bool PDCHRestartCause; // a flag to indicate to pdch the cause to start or restart pdch.
 #define FIRST_START 1
 #define RESET 2
 u8 PAgcUpdatedNum; // a flag to indicate whether the current Pagc is updated or not.
 
#if defined( STK_MAX_SIM ) && (STK_MAX_SIM >1) 
 u32 SimId;     // dedicated for 2C2D1T prj.
#endif

#ifndef __L1_NO_PDCH_OPTIMIZATION__
 u8   SendUsfInd;
 u8   NbTxMsg;
 struct
 {
  void *Msg;
  u8    Tn;
  u8    QueueId;
 } TxMsgBuffer [4];

#endif
 l1_MacDyn_t    MacDyn;
 l1_MacFixed_t  MacFixed;
 l1_FixedUpdt_t FixedUpdt;
 l1_FreqParam_t FreqParam;
 l1_GPTimAdv_t  GPTimAdv;
 l1_TBFAlloc_t  TBFAlloc;
 l1_TxBuffer_t  TxBuffer;
 l1_PwrCtrl_t   PwrCtrl;
 l1_DlPwrCtrl_t DlPwrCtrl;
} l1_TBFCtx_t;

#if defined( STK_MAX_SIM ) && (STK_MAX_SIM >1) 
#define L1_BOX_ABLE 1
#define L1_BOX_UNABLE 0
#define BCCH_JOB 0
#define CCCH_JOB 1
#define CBCH_JOB 2
#define MAXJOB 3
typedef struct 
{
u8 Owner; // indicate who locks the box.
u32 AimFn [STK_MAX_SIM][MAXJOB]; // the next PCH Position.
//s16 FailRound [STK_MAX_SIM]; // record failed times of receiving.
}l1_Combox_t;

/*end*/

typedef struct
{
 u8  State [STK_MAX_SIM];
 u8  AMR;                           // Current vocoder is AMR.
 u16 SubState [STK_MAX_SIM];
 u32 FrameLoadBitMap;               // Window load in frame
#ifdef __L1_GENERATE_EQ_INT__
 u32 FrmSchd;                       // Global frame schedule.
#endif
 l1_Cell_t Cell [STK_MAX_SIM][L1_NB_MAX_CELLS];
 l1_Serving_t Serving[STK_MAX_SIM];              // Serving cell context.
// l1_Serving_t Serving2;              // Serving cell2 context.

 l1_Combox_t    ComBox;     //Combox used to resolve contention between two CCH Rx jobs

 Msg_t             *MonitorReq [STK_MAX_SIM];     // Current monitoring configuration.
 Msg_t             *MemMonitorReq [STK_MAX_SIM];  // Memorized monitoring configuration.
 Msg_t             *ExtdMonitorReq [STK_MAX_SIM]; // Extended monitoring configuration.
 Msg_t             *TxDiscardReq;   // GPRS Transfer mode only.
 Msg_t             *NextTxDiscardReq;   // GPRS Transfer mode only.
 l1_PMeasCtrlReq_t *PMeasCtrlReq[STK_MAX_SIM];   // Packet Measure Control configuration.
 l1_CchReq_t       *CchReq [STK_MAX_SIM];         // Current Paging configuration.
 l1_PCchReq_t      *PCchReq;        // Current Packet Paging configuration.
 Msg_t             *PBcchReq [2];   // Current PBcch configuration.
 //Msg_t             *NcMonitorReq;   // To be taken into account at the beginning of the next RP.
 l1_RachOnReq_t    *RachOnReq;      // Current Rach context.
 l1_ConnectReq_t   *ConnectReq;     // Current Connection Request.
 l1s_TACtx_t       *TACtx;          // Timing advance current Ctx.
 l1_TBFCtx_t       *TBFCtx;         // TBF configuration for transfer state.
 pal_AMRCfg_t      *AMRCfg;         // Last AMR cfg.

 l1_MeasCfg_t       MeasCfg [STK_MAX_SIM];        // Context for measure on BA.
 l1_ServMeasCfg_t   ServMeasCfg [STK_MAX_SIM];    // Context for measure on serving cell.

 l1_CbchReq_t      *CbchReq[STK_MAX_SIM];        // Current Cbch configuration.
 u8  BcchMode [STK_MAX_SIM];    //bcch mode: must be Read or not.
#ifdef __L1_DUMMY_DETECTION__
 u8  DummyResult ;
#endif
 u16 CchQbOf[STK_MAX_SIM];
 u32 CchFnOf[STK_MAX_SIM];
 u8                 CbchSkipCnt[STK_MAX_SIM] [2];// Number of Tb period to be skipped.
 u8                 CbchSkipBitMap[STK_MAX_SIM]; // Bitmap indicating the skipped counter to be taken into account.

 u8                 OldTA;          // Used to memorize the TA before leaving a cell for a HO. The valueis used in case of resume.
 
 #ifndef __NO_GPRS__
 u8                 PSTxBitMap;     // Tx Bit Map used for packet synchronized block transmission.
 u8                 NbPSJob[STK_MAX_SIM];        // Number of instances of PSJob currently active.
 u8                 PDataPurgeReq;  // Purge Packet Data queue required by RR (CS change).
 #endif
 
 u8  FreeCell [STK_MAX_SIM];
 u8  ActiveCell [STK_MAX_SIM];
#ifdef __L1_MULTI_AFCDAC__
 int16  CDacOffset[STK_MAX_SIM];
 int16  AfcOffset[STK_MAX_SIM]; 
 u8  AfcLocked[STK_MAX_SIM];
#else
 u8  AfcIdx;
 u8  AfcLocked;
#endif 
 u8  GainOffset;    // Gain offset for AGC setting.
 u8  Job [L1_JOB_LAST];

 u8  CellList [STK_MAX_SIM][3];

 volatile u32 Fn;
 volatile u16 RFn;
 volatile u8  T1R;
 volatile u8  T2;
 volatile u8  T3;
 volatile u8  C52;
 volatile u8  C102;
 volatile u8  C104;

 struct
 {
#ifdef __L1_DUMMY_DETECTION__
  u8 Win [PAL_NB_MAX_WIN_PER_FRAME + PAL_NB_EXTR_WIN_PER_FRAME];
#else
  u8 Win [PAL_NB_MAX_WIN_PER_FRAME];
#endif
  u8 Idx;
  u8 NbWin;                   // Number of windows programmed in the current frame.
 } WinAlloc;

 u8  Tn;                      // Current timeslot number.
 u8  Sleep;                   // TRUE when Fint is suspended.
 u8  SleepEnable;
 u8  AdjReport[STK_MAX_SIM]; //if report is on, do not suspend FINT IRQ.
 u8  NearMode;                // Near mode: Full or Sliced.
 u8  DataBlockSize;           // Used in TCH data mode: block size depending of current channel type and mode.
 u8  BandMap [STK_MAX_SIM];                 // Map of the frequency bands currently used.
 u8  InitDone[STK_MAX_SIM];      //flag of init is done before state change.
 bool  SamePCS;               //two sim card selects the same high band, i.e. 1800 or 1900.
 bool  SameBA;
 
 l1_Cell_t *BaseCell;        //the cell that synchro updated to.
 Msg_t * MemPwrReq[STK_MAX_SIM - 1];
 u32   NbPwrReq;
 
 u8 MemPwrBitmap;
 bool SkipFrameEndEvt[STK_MAX_SIM -1];         //skip frame end event for monitor job 2,3,4...
 u8 TCHTestMode; //added to distinguish Lab test and real network situation 2011-12-24
#ifdef __L1S_SPY__
 u8  SpyServingId;
 u8  SpyNearId;
#endif

#ifdef L1_CELL_EQ_CHECK
 l1_EquRes_t EquRes [5];  // Equalization results memorization.
// u8  BstIdx [5];
#endif
#ifdef __L1_CCH_RX_OPT__
u8 RxQulity [STK_MAX_SIM];
u8 BlockSnr [STK_MAX_SIM];
u8 GoodBlockCount[STK_MAX_SIM];
#endif
bool CalibMode;

#ifdef __L1_CCH_DEC_DONE_IRQ_EN__
u8 WaitDecdone;
#endif

u8 NewHighBand[STK_MAX_SIM];
} l1_Ctx_t;

#else

typedef struct
{
 u8  State;
 u8  AMR;                           // Current vocoder is AMR.
 u16 SubState;
 u32 FrameLoadBitMap;               // Window load in frame
#ifdef __L1_GENERATE_EQ_INT__
 u32 FrmSchd;                       // Global frame schedule.
#endif
 l1_Cell_t Cell [L1_NB_MAX_CELLS];
 l1_Serving_t Serving;              // Serving cell context.

 Msg_t             *MonitorReq;     // Current monitoring configuration.
 Msg_t             *MemMonitorReq;  // Memorized monitoring configuration.
 Msg_t             *ExtdMonitorReq; // Extended monitoring configuration.
 Msg_t             *TxDiscardReq;   // GPRS Transfer mode only.
 Msg_t             *NextTxDiscardReq;   // GPRS Transfer mode only.
 l1_PMeasCtrlReq_t *PMeasCtrlReq;   // Packet Measure Control configuration.
 l1_CchReq_t       *CchReq;         // Current Paging configuration.
 l1_PCchReq_t      *PCchReq;        // Current Packet Paging configuration.
 Msg_t             *PBcchReq [2];   // Current PBcch configuration.
 //Msg_t             *NcMonitorReq;   // To be taken into account at the beginning of the next RP.
 l1_RachOnReq_t    *RachOnReq;      // Current Rach context.
 l1_ConnectReq_t   *ConnectReq;     // Current Connection Request.
 l1s_TACtx_t       *TACtx;          // Timing advance current Ctx.
 l1_TBFCtx_t       *TBFCtx;         // TBF configuration for transfer state.
 pal_AMRCfg_t      *AMRCfg;         // Last AMR cfg.

 l1_MeasCfg_t       MeasCfg;        // Context for measure on BA.
 l1_ServMeasCfg_t   ServMeasCfg;    // Context for measure on serving cell.

 l1_CbchReq_t      *CbchReq;        // Current Cbch configuration.
 u8                 CbchSkipCnt [2];// Number of Tb period to be skipped.
 u8                 CbchSkipBitMap; // Bitmap indicating the skipped counter to be taken into account.

 u8                 OldTA;          // Used to memorize the TA before leaving a cell for a HO. The valueis used in case of resume.
 u8                 PSTxBitMap;     // Tx Bit Map used for packet synchronized block transmission.
 u8                 NbPSJob;        // Number of instances of PSJob currently active.
 u8                 PDataPurgeReq;  // Purge Packet Data queue required by RR (CS change).
 u8  FreeCell;
 u8  ActiveCell;
#ifdef __L1_DUMMY_DETECTION__
 u8  DummyResult ;
#endif
#ifdef __L1_MULTI_AFCDAC__
 int16  CDacOffset;
 int16  AfcOffset; 
#else
 u8  AfcIdx; 
#endif 
u8  AfcLocked;
 u8  GainOffset;    // Gain offset for AGC setting.
 u8  Job [L1_JOB_LAST];

 u8  CellList [3];

 volatile u32 Fn;
 volatile u16 RFn;
 volatile u8  T1R;
 volatile u8  T2;
 volatile u8  T3;
 volatile u8  C52;
 volatile u8  C102;
 volatile u8  C104;

 struct
 {
#ifdef __L1_DUMMY_DETECTION__
  u8 Win [PAL_NB_MAX_WIN_PER_FRAME + PAL_NB_EXTR_WIN_PER_FRAME];
#else
  u8 Win [PAL_NB_MAX_WIN_PER_FRAME];
#endif
  u8 Idx;
  u8 NbWin;                   // Number of windows programmed in the current frame.
 } WinAlloc;

 u8  Tn;                      // Current timeslot number.
 u8  Sleep;                   // TRUE when Fint is suspended.
 u8  SleepEnable;
 u8  NearMode;                // Near mode: Full or Sliced.
 u8  DataBlockSize;           // Used in TCH data mode: block size depending of current channel type and mode.
 u8  BandMap;                 // Map of the frequency bands currently used.
 u8  InitDone;
 u8  BcchMode;
 u8 TCHTestMode; //added to distinguish Lab test and real network situation 2011-12-24
#ifdef __L1S_SPY__
 u8  SpyServingId;
 u8  SpyNearId;
#endif

#ifdef L1_CELL_EQ_CHECK
 l1_EquRes_t EquRes [5];  // Equalization results memorization.
// u8  BstIdx [5];
#endif
#ifdef __L1_CCH_RX_OPT__
u8 RxQulity;
u8 BlockSnr;
u8 GoodBlockCount;
#endif

#ifdef __L1_CCH_DEC_DONE_IRQ_EN__
u8 WaitDecdone;
#endif

u8 NewHighBand;
} l1_Ctx_t;
#endif


DefExtern l1_Ctx_t l1_Ctx;

#undef DefExtern

#endif

