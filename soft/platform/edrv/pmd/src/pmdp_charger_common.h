/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#include "pmd_m.h"
#include "pmd_map.h"


// ============================================================================
//  MACROS
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// RDA_PMU_CHR_STATUS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    CHR_STATUS_UNKNOWN,
    CHR_STATUS_AC_OFF,
    CHR_STATUS_AC_ON,
} RDA_PMU_CHR_STATUS_T;


// ============================================================================
// PMD_CHARGING_STAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the stages during charging phase
// ============================================================================
typedef enum
{
    PMD_CHARGING_STAGE_OFF,
    PMD_CHARGING_STAGE_PULSE,
    PMD_CHARGING_STAGE_ON,
    PMD_CHARGING_STAGE_ERROR,
} PMD_CHARGING_STAGE_T;


// ============================================================================
// PMD_CHARGER_VFB_T
// ----------------------------------------------------------------------------
/// This enum lists the valid VFB adjustment values
// ============================================================================
typedef enum
{
    PMD_CHARGER_VFB_DEC,
    PMD_CHARGER_VFB_INC,
    PMD_CHARGER_VFB_MIN,
    PMD_CHARGER_VFB_MAX,
} PMD_CHARGER_VFB_T;


// ============================================================================
// PMD_MAX_CURRENT_USER_T
// ----------------------------------------------------------------------------
/// This enum lists the valid users to set max charge current
// ============================================================================
typedef enum
{
    PMD_MAX_CURRENT_CHR_VOLT,
    PMD_MAX_CURRENT_CHR_TYPE,

    PMD_MAX_CURRENT_USER_QTY,
} PMD_MAX_CURRENT_USER_T;


// ============================================================================
// PMD_CHARGER_PROC_CFG_T
// ----------------------------------------------------------------------------
/// This structure describes the internal configuration during charge process
// ============================================================================
typedef struct
{
    /// The VFB value will be decreased to enlarge the charge current
    /// when the battery level is larger than the following offset plus
    /// g_pmdConfig->batteryLevelChargeTermMV
    INT16 battOffsetVfbUpperLimit;
    /// The VFB value will be increased to reduce the charge current
    /// when the battery level is smaller than the following offset plus
    /// g_pmdConfig->batteryLevelChargeTermMV
    INT16 battOffsetVfbLowerLimit;
    /// The VFB lower limit in small current mode
    INT16 battOffsetVfbLowerLimitSmallCurrent;
    /// The compensation value to the above VFB limit window when the
    /// system is in high activity state
    INT16 highActVfbWindowOffset;
    /// The small current mode will be applied when the battery level is
    /// larger than the following offset plus
    /// g_pmdConfig->batteryLevelChargeTermMV
    INT16 battOffsetSmallCurrent;
    /// The interval to check the real battery level
    UINT32 intervalBattLevelCheck;
} PMD_CHARGER_PROC_CFG_T;


// ============================================================================
// PMD_CHARGER_PROC_OPS_T
// ----------------------------------------------------------------------------
/// This structure describes the callback functions during charge process
// ============================================================================
typedef struct
{
    /// Tell whether to proceed with the battery temperature check
    BOOL (*checkTemperatureValidity)(VOID);
    /// Tell whether to proceed with the battery temperature check
    BOOL (*checkChargerVoltageValidity)(VOID);
    /// Configure h/w registers to achieve the charge current change
    VOID (*setChargerCurrent)(PMD_CHARGE_CURRENT_T current);
    /// Special handling for different charger types
    VOID (*setChargerType)(PMD_CHARGER_TYPE_T type);
    /// Charger plug-in callback
    VOID (*chargerPlugin)(VOID);
    /// Charger plug-out callback
    VOID (*chargerPlugout)(VOID);
    /// Tell whether to proceed with the charger manager process
    /// (returning 0 to proceed, otherwise the ticks to wait before retry)
    UINT32 (*chargerManager)(VOID);
    /// Charger state control callback
    VOID (*chargerChargingState)(UINT32 meanBattVolt);
    /// Charger monitor callback (to arrange temperature and charger level monitor, etc)
    VOID (*chargerMonitor)(VOID);
    /// Get the battery offset compensation for VFB window
    INT16 (*getVfbWindowOffset)(VOID);
    /// Initialize charger for individual chip types
    RDA_PMU_CHR_STATUS_T (*initCharger)(VOID);
    // Configure h/w registers to start/stop charging
    VOID (*stopCharging)(BOOL stop);
    /// Charger enable/disable callback
    VOID (*enableCharger)(BOOL enable);
    /// Callback after application gets the battery level
    VOID (*postGetBatteryLevel)(VOID);
} PMD_CHARGER_PROC_OPS_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

/// Charger process configurations
EXPORT PROTECTED CONST PMD_CHARGER_PROC_CFG_T gc_pmdChargerProcCfg;

/// Charger process callbacks
EXPORT PROTECTED CONST PMD_CHARGER_PROC_OPS_T gc_pmdChargerProcOps;


// ============================================================================
//  FUNCTIONS
// ============================================================================

// ============================================================================
// pmd_DcOnHandler
// ----------------------------------------------------------------------------
/// Registerred handler for GPIO_0 used as DC-ON Detect.
///
// ============================================================================
PROTECTED VOID pmd_DcOnHandler(BOOL on);

// ============================================================================
// pmd_SetChargeCurrent
// ----------------------------------------------------------------------------
/// Set the current for the charge.
// ============================================================================
PROTECTED VOID pmd_SetChargeCurrent(PMD_CHARGE_CURRENT_T current);

PROTECTED PMD_CHARGE_CURRENT_T pmd_GetChargeCurrent(VOID);
PROTECTED VOID pmd_SetMaxChargeCurrent(PMD_MAX_CURRENT_USER_T user, PMD_CHARGE_CURRENT_T current);
PROTECTED VOID pmd_InitMaxChargeCurrent(VOID);

PROTECTED VOID pmd_InitCharger(VOID);
PROTECTED RDA_PMU_CHR_STATUS_T pmd_GetChargerHwStatus(VOID);
PROTECTED VOID pmd_StopCharging(BOOL stop);
PROTECTED VOID pmd_TuneCharger(VOID);

PROTECTED VOID pmd_RestoreChargerAtPowerOff(VOID);

PROTECTED UINT32 pmd_GetTemperatureStableCount(VOID);
PROTECTED INT32 pmd_CalculateMeanTemperature(VOID);
PROTECTED INT16 pmd_TemperatureVolt2Degree(UINT16 volt);

PROTECTED UINT32 pmd_GetChargerVoltageStableCount(VOID);
PROTECTED UINT32 pmd_CalculateMeanChargerLevel(VOID);

PROTECTED UINT32 pmd_GetAverageGpadcBatteryLevel(UINT32 count);
PROTECTED UINT32 pmd_CalculateMeanBatteryLevel(VOID);

PROTECTED PMD_CHARGING_STAGE_T pmd_GetChargingStage(VOID);

PROTECTED BOOL pmd_SetChargerVfb(PMD_CHARGER_VFB_T vfb);
PROTECTED VOID pmd_ChargerSmallCurrent(BOOL enable);
PROTECTED INT16 pmd_GetHighActivityVoltOffset(VOID);

