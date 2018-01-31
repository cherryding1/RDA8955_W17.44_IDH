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
  File       : RR_CAPA.H
--------------------------------------------------------------------------------

  Scope      : defines for RR capabilities

  History    :
--------------------------------------------------------------------------------
  Mar 17 06  |  MCE   | Cast RR_NONDRX_TIMER u8.Chge its name: RR_NONDRX_TIM_VAL
  Jun 14 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RR_CAPA_H__ 
#define __RR_CAPA_H__ 
#include "itf_l1.h"
#include "sap_cust.h"

/*
** Macros to determine support of features
*/
#ifndef STK_SINGLE_SIM
#define RR_SPLIT_PG_CYCLE               psap_StackCfg->SplitPgCycle
#else
#define RR_SPLIT_PG_CYCLE               sap_StackCfg.SplitPgCycle
#endif

#define RR_SPLIT_ON_CCCH                FALSE

#ifndef STK_SINGLE_SIM
#define RR_NONDRX_TIM_VAL               psap_StackCfg->NonDrxTimer
#else
#define RR_NONDRX_TIM_VAL               sap_StackCfg.NonDrxTimer
#endif

#define RR_EXT_MEAS_CAPA                FALSE
#define RR_CBCH_EXT_SUPP                FALSE

#endif // __RR_CAPA_H__ 

