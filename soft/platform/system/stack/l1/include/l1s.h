/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File         l1s.h
--------------------------------------------------------------------------------

  Scope      : Layer one synchrous domain header file

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  | ADA   | Creation
================================================================================
*/
#ifndef __L1S_H__
#define __L1S_H__

#include "sxs_type.h"
#include "sxr_ops.h"
#include "l1a_msg.h"
#include "l1.h"

#include "stack.h"  //20060922 by wangxu

#ifdef __L1S_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif
#ifndef STK_SINGLE_SIM 
#define L1S_CAP_PCS1900 (l1_Ctx.BandMap[0] & PAL_PCS1900)
#define L1S_CAP_DCS1800 (l1_Ctx.BandMap[0] & PAL_DCS1800)
#else
#define L1S_CAP_PCS1900 (l1_Ctx.BandMap & PAL_PCS1900)
#define L1S_CAP_DCS1800 (l1_Ctx.BandMap & PAL_DCS1800)
#endif

#define L1_TA_MAX_VALUE(c)     63

#define L1S_MULTISLOT_CAP_TTA_BIT_MAP   0x03L
#define L1S_MULTISLOT_CAP_TTA           2
#define L1S_MULTISLOT_CAP_TTB_BIT_MAP   0x01L
#define L1S_MULTISLOT_CAP_TTB           1
#define L1S_MULTISLOT_CAP_TRA_BIT_MAP   0x03L
#define L1S_MULTISLOT_CAP_TRA           2
#define L1S_MULTISLOT_CAP_TRB_BIT_MAP   0x01L
#define L1S_MULTISLOT_CAP_TRB           1

#define L1S_MULTISLOT_CAP_RX_BIT_MAP    0x0FL
#define L1S_MULTISLOT_CAP_RX            4
#define L1S_MULTISLOT_CAP_TX_BIT_MAP    0x0FL
#define L1S_MULTISLOT_CAP_TX            4

#define L1S_MULTISLOT_CAP_SUM   5

#define L1S_T200_SAPI0_SDCCH   45
#define L1S_T200_SAPI0_FACCHF  34
#define L1S_T200_SAPI0_FACCHH  40

#define L1S_T200_SAPI3_SDCCH   96
#define L1S_T200_SAPI3_FACCHF  40
#define L1S_T200_SAPI3_FACCHH  44
#define L1S_T200_SAPI3_SACCH   416     //195 new one suggested by lin chaoqing to fix bug6268


#define L1_GEN_TIMER_ID(Tim, NoJob) ((Tim << 8) | NoJob)
#define L1_GET_TIMER_ID(Id)         ((Id >> 8) & 0x3F)
#define L1_TIMER_JOB_MSK            0xFF
#define L1_RELEASE_PARAM_TIMER      (1 << 5)


#define L1S_FREQ_PARAM_TIMER_ID        1
#define L1S_TOF_CTRL_TIMER_ID          2
#define L1S_TXPWR_CTRL_TIMER_ID        3

#define L1S_PS_ENCODE_TIMER_ID        (5 | L1_RELEASE_PARAM_TIMER)
#define L1S_CCH_JOBS_RESUME_TIMER_ID   6
#define L1S_PS_DATA_IND_TIMER          7
#define L1S_CIPHER_TIMER_ID            8
#define L1S_PS_END_TIMER_ID            9
#ifndef STK_SINGLE_SIM
#define L1S_NEAR_TIMER_ID(a)           (10 + a) //a =0 or a=1
#else
#define L1S_NEAR_TIMER_ID              4
#endif

#define L1S_POWER_JOB_ID               1
//#define L1S_MONITORING_JOB_ID          2
#define L1S_NEAR_JOB_ID                3
//#define L1S_CCH_JOB_ID                 4
#define L1S_PCCH_JOB_ID                5
//#define L1S_BCCH_JOB_ID                6
#define L1S_PBCCH_JOB_ID               7
#define L1S_RACH_JOB_ID                8
#define L1S_SDCCH_JOB_ID               9
#define L1S_TCH_JOB_ID                 10
#define L1S_HO_ACCESS_JOB_ID           11
#define L1S_TBF_SETUP_JOB_ID           12
#define L1S_PDCH_JOB_ID                13
#define L1S_TBF_RELEASE_JOB_ID         14
#define L1S_PS_JOB_ID                  15
#define L1S_PTCCH_JOB_ID               16
//#define L1S_CBCH_JOB_ID                17

#ifndef STK_SINGLE_SIM 
#define L1S_CCHCONTROL_JOB_ID          18
#define L1S_CCHA_JOB_ID                19
#define L1S_CCHB_JOB_ID                20
#define L1S_MONITORING_A_JOB_ID        21
#define L1S_MONITORING_B_JOB_ID        22
#define L1S_BCCHA_JOB_ID               23
#define L1S_BCCHB_JOB_ID               24
#if (STK_MAX_SIM >= 3)
 #define L1S_CCHC_JOB_ID                25
 #define L1S_BCCHC_JOB_ID               26
 #define L1S_MONITORING_C_JOB_ID        27
#endif
#if (STK_MAX_SIM >= 4)
 #define L1S_CCHD_JOB_ID                28
 #define L1S_BCCHD_JOB_ID               29
 #define L1S_MONITORING_D_JOB_ID        30
#endif
#define L1S_CBCHA_JOB_ID                31
#define L1S_CBCHB_JOB_ID                32 
#else
#define L1S_MONITORING_JOB_ID          2
#define L1S_CCH_JOB_ID                 4
#define L1S_BCCH_JOB_ID                6
#define L1S_CBCH_JOB_ID                17
#endif
#define L1S_POWER_JOB_STACK_SIZE       250 //200
//#define L1S_MONITORING_JOB_STACK_SIZE  240//200 modified by zhanghy 20070124
#define L1S_NEAR_JOB_STACK_SIZE        250 //200
//#define L1S_CCH_JOB_STACK_SIZE         200 //200 modified by alex 2006-11-1
#define L1S_PCCH_JOB_STACK_SIZE        250
//#define L1S_BCCH_JOB_STACK_SIZE        200
#define L1S_PBCCH_JOB_STACK_SIZE       250
#define L1S_RACH_JOB_STACK_SIZE        250 //200//128 modified by alex 2006-09-29
#define L1S_SDCCH_JOB_STACK_SIZE       250 //200 modified by alex 2007-3-21
#define L1S_TCH_JOB_STACK_SIZE         250
#define L1S_HO_ACCESS_JOB_STACK_SIZE   250 //128 modified by alex 2006-11-1
#define L1S_TBF_SETUP_JOB_STACK_SIZE   250 //200
#define L1S_PDCH_JOB_STACK_SIZE        250
#define L1S_TBF_RELEASE_JOB_STACK_SIZE 250 //150
#define L1S_PS_JOB_STACK_SIZE          128
#define L1S_PTCCH_JOB_STACK_SIZE       250 //200 modified according to GPRS validation team, 2009-04-10
#define L1S_CBCH_JOB_STACK_SIZE        250

#ifndef STK_SINGLE_SIM
#define L1S_CCHCONTROL_JOB_STACK_SIZE      250 //  200
#endif
#define L1S_MONITORING_JOB_STACK_SIZE  250 //200 modified by zhanghy 20070124
#define L1S_CCH_JOB_STACK_SIZE         250 //200 modified by alex 2006-11-1
#define L1S_BCCH_JOB_STACK_SIZE        250 //200

#define L1S_POWER_JOB_PRIO             137
#define L1S_MONITORING_JOB_PRIO        137
#define L1S_NEAR_JOB_PRIO              130
//#define L1S_CCH_JOB_PRIO               110
#define L1S_PCCH_JOB_PRIO              118
#define L1S_BCCH_JOB_PRIO              115
#define L1S_PBCCH_JOB_PRIO             116
#define L1S_RACH_JOB_PRIO              112
#define L1S_SDCCH_JOB_PRIO             109
#define L1S_TCH_JOB_PRIO               109
#define L1S_HO_ACCESS_JOB_PRIO         109
#define L1S_TBF_SETUP_JOB_PRIO         100
#define L1S_PDCH_JOB_PRIO              122
#define L1S_TBF_RELEASE_JOB_PRIO       120
#define L1S_PS_JOB_PRIO                 99
#define L1S_PTCCH_JOB_PRIO             121
#define L1S_CBCH_JOB_PRIO              119

#ifndef STK_SINGLE_SIM 
#define L1S_CCHCONTROL_JOB_PRIO               110
#define L1S_CCH_JOB_PRIO               111
#else
#define L1S_CCH_JOB_PRIO               110
#endif

#ifndef STK_SINGLE_SIM
void l1s_PowerJob       (Msg_t *Msg);
void l1s_ExitPowerJob    (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_MonitoringJob   (u32 SimIdx);
void l1s_ExitMonitoring (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_ExitMonitoringA (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_ExitMonitoringB (Msg_t *Msg, u8 JobNb, u8 Deletion);
#if (STK_MAX_SIM >= 3)
void l1s_ExitMonitoringC (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
#if (STK_MAX_SIM >= 4)
void l1s_ExitMonitoringD (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
void l1s_NearJob        (void);
void l1s_ExitNearJob    (void);
void l1s_CchControlJob       (u32 SimId);
void l1s_RxCchCOMJob    (u32 SimIdx);
void l1s_ExitCchControl  (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_ExitRxCch      (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_ExitRxCchA      (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_ExitRxCchB      (Msg_t *Msg, u8 JobNb, u8 Deletion);
#if (STK_MAX_SIM >= 3)
void l1s_ExitRxCchC      (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
#if (STK_MAX_SIM >= 4)
void l1s_ExitRxCchD      (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
#else
void l1s_PowerJob       (Msg_t *Msg);
void l1s_ExitPowerJob   (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_MonitoringJob  (void);
void l1s_ExitMonitoring (void *Msg, u8 JobNb, u8 Deletion);
void l1s_NearJob        (void);
void l1s_ExitNearJob    (void);
void l1s_RxCchJob       (Msg_t *Msg);
void l1s_ExitRxCch      (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif

#ifndef __NO_GPRS__
void l1s_RxPCchJob      (Msg_t *Msg);
void l1s_ExitRxPCch     (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
void l1s_RxBcchJob      (Msg_t *Msg);
void l1s_ExitRxBcch     (Msg_t *Msg, u8 JobNb, u8 Deletion);
#ifndef __NO_GPRS__
void l1s_RxPBcchJob     (Msg_t *Msg);
void l1s_ExitRxPBcch    (Msg_t *Msg, u8 JobNb, u8 Deletion);
#endif
void l1s_RachJob        (Msg_t *Msg);
void l1s_ExitRach       (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_SdcchJob       (Msg_t *Msg);
void l1s_ExitSdcch      (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_TchJob         (Msg_t *Msg);
void l1s_ExitTch        (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_HoAccessJob    (l1_ConnectReq_t *ConnectReq);
void l1s_ExitHoAccess   (l1_ConnectReq_t *ConnectReq, u8 JobNb, u8 Deletion);
#ifndef __NO_GPRS__
void l1s_TBFSetUpJob    (Msg_t *Msg);
void l1s_TBFSetUpExit   (Msg_t *Msg, u8 JobNb, u8 Deletion);
void l1s_PdchJob        (void);
void l1s_ExitPdch       (void *Msg, u8 JobNb, u8 Deletion);
void l1s_TBFReleaseJob  (u8 TBFBitMap);
void l1s_TBFReleaseExit (u8 TBFBitMap, u8 JobNb, u8 Deletion);
void l1s_PSJob          (Msg_t *Msg);
void l1s_ExitPS         (void *Msg, u8 JobNb, u8 Deletion);
void l1s_PTCchJob       (void);
void l1s_PDataQueuePurgeReq (void);
u8 l1s_Allocation (l1_TBFCfgReq_t *TBFCfgReq, /* u8 TBFHeld, */ l1_TBFAlloc_t *TBFAlloc);
#endif
void l1s_RxCbchJob      (Msg_t *Msg);
void l1s_ExitRxCbch     (Msg_t *Msg, u8 JobNb, u8 Deletion);

#ifdef __L1S_VAR__
const sxr_JbDesc_t l1s_PowerJbDesc =
{
 (void (*)(void *))l1s_PowerJob,
 (void (*)(void *, u8, u8))l1s_ExitPowerJob,
 "Initial power measures",
 L1S_POWER_JOB_ID,
 L1S_POWER_JOB_STACK_SIZE,
 L1S_POWER_JOB_PRIO,
};
const sxr_JbDesc_t l1s_NearJbDesc =
{
 (void (*)(void *))l1s_NearJob,
 (void (*)(void *, u8, u8))l1s_ExitNearJob,
 "Near job",
 L1S_NEAR_JOB_ID,
 L1S_NEAR_JOB_STACK_SIZE,
 L1S_NEAR_JOB_PRIO,
};
#ifndef STK_SINGLE_SIM 
const sxr_JbDesc_t l1s_MonitoringAJbDesc =
{
 (void (*)(void *))l1s_MonitoringJob,
 (void (*)(void *, u8, u8))l1s_ExitMonitoringA,
 "Monitoring A job",
 L1S_MONITORING_A_JOB_ID,
 L1S_MONITORING_JOB_STACK_SIZE,
 L1S_MONITORING_JOB_PRIO,
};
const sxr_JbDesc_t l1s_MonitoringBJbDesc =
{
 (void (*)(void *))l1s_MonitoringJob,
 (void (*)(void *, u8, u8))l1s_ExitMonitoringB,
 "Monitoring B job",
 L1S_MONITORING_B_JOB_ID,
 L1S_MONITORING_JOB_STACK_SIZE,
 L1S_MONITORING_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchControlJbDesc =
{
 (void (*)(void *))l1s_CchControlJob,
 (void (*)(void *, u8, u8))l1s_ExitCchControl,
 "Cch control job",
 L1S_CCHCONTROL_JOB_ID,
 L1S_CCHCONTROL_JOB_STACK_SIZE,
 L1S_CCHCONTROL_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchAJbDesc =
{
 (void (*)(void *))l1s_RxCchCOMJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCchA,
 "CchA job",
 L1S_CCHA_JOB_ID,
 L1S_CCH_JOB_STACK_SIZE,
 L1S_CCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchBJbDesc =
{
 (void (*)(void *))l1s_RxCchCOMJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCchB,
 "CchB job",
 L1S_CCHB_JOB_ID,
 L1S_CCH_JOB_STACK_SIZE,
 L1S_CCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_BcchAJbDesc =
{
 (void (*)(void *))l1s_RxBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxBcch,
 "BcchA job",
 L1S_BCCHA_JOB_ID,
 L1S_BCCH_JOB_STACK_SIZE,
 L1S_BCCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_BcchBJbDesc =
{
 (void (*)(void *))l1s_RxBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxBcch,
 "BcchB job",
 L1S_BCCHB_JOB_ID,
 L1S_BCCH_JOB_STACK_SIZE,
 L1S_BCCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CbchAJbDesc =
{
	(void (*)(void *))l1s_RxCbchJob,
	(void (*)(void *, u8, u8))l1s_ExitRxCbch,
	"CbchA job",
	L1S_CBCHA_JOB_ID,
	L1S_CBCH_JOB_STACK_SIZE,
	L1S_CBCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CbchBJbDesc =
{
	(void (*)(void *))l1s_RxCbchJob,
	(void (*)(void *, u8, u8))l1s_ExitRxCbch,
	"CbchB job",
	L1S_CBCHB_JOB_ID,
	L1S_CBCH_JOB_STACK_SIZE,
	L1S_CBCH_JOB_PRIO,
};

#if (STK_MAX_SIM >= 3)
const sxr_JbDesc_t l1s_MonitoringCJbDesc =
{
 (void (*)(void *))l1s_MonitoringJob,
 (void (*)(void *, u8, u8))l1s_ExitMonitoringC,
 "Monitoring C job",
 L1S_MONITORING_C_JOB_ID,
 L1S_MONITORING_JOB_STACK_SIZE,
 L1S_MONITORING_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchCJbDesc =
{
 (void (*)(void *))l1s_RxCchCOMJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCchC,
 "CchC job",
 L1S_CCHC_JOB_ID,
 L1S_CCH_JOB_STACK_SIZE,
 L1S_CCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_BcchCJbDesc =
{
 (void (*)(void *))l1s_RxBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxBcch,
 "BcchC job",
 L1S_BCCHC_JOB_ID,
 L1S_BCCH_JOB_STACK_SIZE,
 L1S_BCCH_JOB_PRIO,
};
#endif
#if (STK_MAX_SIM >= 4)
const sxr_JbDesc_t l1s_MonitoringDJbDesc =
{
 (void (*)(void *))l1s_MonitoringJob,
 (void (*)(void *, u8, u8))l1s_ExitMonitoringD,
 "Monitoring D job",
 L1S_MONITORING_D_JOB_ID,
 L1S_MONITORING_JOB_STACK_SIZE,
 L1S_MONITORING_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchDJbDesc =
{
 (void (*)(void *))l1s_RxCchCOMJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCchD,
 "CchD job",
 L1S_CCHD_JOB_ID,
 L1S_CCH_JOB_STACK_SIZE,
 L1S_CCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_BcchDJbDesc =
{
 (void (*)(void *))l1s_RxBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxBcch,
 "BcchD job",
 L1S_BCCHD_JOB_ID,
 L1S_BCCH_JOB_STACK_SIZE,
 L1S_BCCH_JOB_PRIO,
};
#endif
#else
const sxr_JbDesc_t l1s_MonitoringJbDesc =
{
 (void (*)(void *))l1s_MonitoringJob,
 (void (*)(void *, u8, u8))l1s_ExitMonitoring,
 "Monitoring job",
 L1S_MONITORING_JOB_ID,
 L1S_MONITORING_JOB_STACK_SIZE,
 L1S_MONITORING_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CchJbDesc =
{
 (void (*)(void *))l1s_RxCchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCch,
 "Cch job",
 L1S_CCH_JOB_ID,
 L1S_CCH_JOB_STACK_SIZE,
 L1S_CCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_BcchJbDesc =
{
 (void (*)(void *))l1s_RxBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxBcch,
 "Bcch job",
 L1S_BCCH_JOB_ID,
 L1S_BCCH_JOB_STACK_SIZE,
 L1S_BCCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_CbchJbDesc =
{
 (void (*)(void *))l1s_RxCbchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxCbch,
 "Cbch job",
 L1S_CBCH_JOB_ID,
 L1S_CBCH_JOB_STACK_SIZE,
 L1S_CBCH_JOB_PRIO,
};
#endif

#ifndef __NO_GPRS__
const sxr_JbDesc_t l1s_PCchJbDesc =
{
 (void (*)(void *))l1s_RxPCchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxPCch,
 "PCch job",
 L1S_PCCH_JOB_ID,
 L1S_PCCH_JOB_STACK_SIZE,
 L1S_PCCH_JOB_PRIO,
};
#endif
#ifndef __NO_GPRS__
const sxr_JbDesc_t l1s_PBcchJbDesc =
{
 (void (*)(void *))l1s_RxPBcchJob,
 (void (*)(void *, u8, u8))l1s_ExitRxPBcch,
 "PBcch job",
 L1S_PBCCH_JOB_ID,
 L1S_PBCCH_JOB_STACK_SIZE,
 L1S_PBCCH_JOB_PRIO,
};
#endif
const sxr_JbDesc_t l1s_RachJbDesc =
{
 (void (*)(void *))l1s_RachJob,
 (void (*)(void *, u8, u8))l1s_ExitRach,
 "Rach job",
 L1S_RACH_JOB_ID,
 L1S_RACH_JOB_STACK_SIZE,
 L1S_RACH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_SdcchJbDesc =
{
 (void (*)(void *))l1s_SdcchJob,
 (void (*)(void *, u8, u8))l1s_ExitSdcch,
 "Sdcch job",
 L1S_SDCCH_JOB_ID,
 L1S_SDCCH_JOB_STACK_SIZE,
 L1S_SDCCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_TchJbDesc =
{
 (void (*)(void *))l1s_TchJob,
 (void (*)(void *, u8, u8))l1s_ExitTch,
 "Tch job",
 L1S_TCH_JOB_ID,
 L1S_TCH_JOB_STACK_SIZE,
 L1S_TCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_HoAccessJbDesc =
{
 (void (*)(void *))l1s_HoAccessJob,
 (void (*)(void *, u8, u8))l1s_ExitHoAccess,
 "Ho access job",
 L1S_HO_ACCESS_JOB_ID,
 L1S_HO_ACCESS_JOB_STACK_SIZE,
 L1S_HO_ACCESS_JOB_PRIO,
};
#ifndef __NO_GPRS__
const sxr_JbDesc_t l1s_TBFSetUpJbDesc =
{
 (void (*)(void *))l1s_TBFSetUpJob,
 (void (*)(void *, u8, u8))l1s_TBFSetUpExit,
 "TBF setup job",
 L1S_TBF_SETUP_JOB_ID,
 L1S_TBF_SETUP_JOB_STACK_SIZE,
 L1S_TBF_SETUP_JOB_PRIO,
};
const sxr_JbDesc_t l1s_PdchJbDesc =
{
 (void (*)(void *))l1s_PdchJob,
 (void (*)(void *, u8, u8))l1s_ExitPdch,
 "Pdch job",
 L1S_PDCH_JOB_ID,
 L1S_PDCH_JOB_STACK_SIZE,
 L1S_PDCH_JOB_PRIO,
};
const sxr_JbDesc_t l1s_TBFReleaseJbDesc =
{
 (void (*)(void *))l1s_TBFReleaseJob,
 (void (*)(void *, u8, u8))l1s_TBFReleaseExit,
 "TBF release job",
 L1S_TBF_RELEASE_JOB_ID,
 L1S_TBF_RELEASE_JOB_STACK_SIZE,
 L1S_TBF_RELEASE_JOB_PRIO,
};
const sxr_JbDesc_t l1s_PSJbDesc =
{
 (void (*)(void *))l1s_PSJob,
 (void (*)(void *, u8, u8))l1s_ExitPS,
 "Ps job",
 L1S_PS_JOB_ID,
 L1S_PS_JOB_STACK_SIZE,
 L1S_PS_JOB_PRIO,
};
const sxr_JbDesc_t l1s_PTCchJbDesc =
{
 (void (*)(void *))l1s_PTCchJob,
 NIL,
 "PTCch job",
 L1S_PTCCH_JOB_ID,
 L1S_PTCCH_JOB_STACK_SIZE,
 L1S_PTCCH_JOB_PRIO,
};
#endif
#else
extern const sxr_JbDesc_t l1s_PowerJbDesc;
extern const sxr_JbDesc_t l1s_NearJbDesc;

#ifndef STK_SINGLE_SIM
extern const sxr_JbDesc_t l1s_CchControlJbDesc;
extern const sxr_JbDesc_t l1s_CchAJbDesc;
extern const sxr_JbDesc_t l1s_CchBJbDesc;
extern const sxr_JbDesc_t l1s_BcchAJbDesc;
extern const sxr_JbDesc_t l1s_BcchBJbDesc;
extern const sxr_JbDesc_t l1s_CbchAJbDesc;
extern const sxr_JbDesc_t l1s_CbchBJbDesc; 
#if (STK_MAX_SIM >=3)
extern const sxr_JbDesc_t l1s_CchCJbDesc;
extern const sxr_JbDesc_t l1s_BcchCJbDesc;
#endif
#if (STK_MAX_SIM >=4)
extern const sxr_JbDesc_t l1s_CchDJbDesc;
extern const sxr_JbDesc_t l1s_BcchDJbDesc;
#endif

#else
extern const sxr_JbDesc_t l1s_CchJbDesc;
extern const sxr_JbDesc_t l1s_BcchJbDesc;
extern const sxr_JbDesc_t l1s_CbchJbDesc;
#endif

#ifndef __NO_GPRS__
extern const sxr_JbDesc_t l1s_PCchJbDesc;
#endif
#ifndef __NO_GPRS__
extern const sxr_JbDesc_t l1s_PBcchJbDesc;
#endif
extern const sxr_JbDesc_t l1s_RachJbDesc;
extern const sxr_JbDesc_t l1s_SdcchJbDesc;
extern const sxr_JbDesc_t l1s_TchJbDesc;
extern const sxr_JbDesc_t l1s_HoAccessJbDesc;
#ifndef __NO_GPRS__
extern const sxr_JbDesc_t l1s_TBFSetUpJbDesc;
extern const sxr_JbDesc_t l1s_PdchJbDesc;
extern const sxr_JbDesc_t l1s_TBFReleaseJbDesc;
extern const sxr_JbDesc_t l1s_PSJbDesc;
extern const sxr_JbDesc_t l1s_PTCchJbDesc;
#endif
#endif

#define L1_BCCH_SCH_COUNT    9
#define L1_POND_FACTOR_THRES PAL_POND_FACTOR_THRES

#define __L1_PREDICTIVE_GAIN__


void l1s_DspIrq (u32 Status);
void l1s_FrameInterrupt (void);
void l1s_FrameBoundary (u8 Fint);


#undef DefExtern

#endif

