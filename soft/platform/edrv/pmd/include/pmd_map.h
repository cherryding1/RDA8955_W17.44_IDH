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


#ifndef _PMD_MAP_H_
#define _PMD_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================
#ifdef CHIP_DIE_8909
#define PMD_MAP_POWER_ID_QTY                     (15)
#else
#define PMD_MAP_POWER_ID_QTY                     (14)
#endif
#define PMD_MAP_LEVEL_ID_QTY                     (10)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PMD_MAP_CHARGER_STATE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32                         status;                       //0x00000000
    UINT32                         current;                      //0x00000004
    VOID*                          handler;                      //0x00000008
    UINT16                         batteryLevel;                 //0x0000000C
    UINT16                         pulsedOnCount;                //0x0000000E
    UINT16                         pulsedOffCount;               //0x00000010
    UINT16                         pulsedCycleCount;             //0x00000012
    UINT16                         pulsedOnMeanCount;            //0x00000014
    UINT16                         pulsedOffMeanCount;           //0x00000016
    UINT32                         startTime;                    //0x00000018
    UINT32                         lastWTime;                    //0x0000001C
    BOOL                           highActivityState;            //0x00000020
} PMD_MAP_CHARGER_STATE_T; //Size : 0x24



// ============================================================================
// PMD_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// Type used to define the accessible structures of the module.
// =============================================================================
typedef struct
{
    UINT32                         powerInfo[PMD_MAP_POWER_ID_QTY]; //0x00000000
    /// Current level value.
    UINT32                         levelInfo[PMD_MAP_LEVEL_ID_QTY]; //0x00000038
    PMD_MAP_CHARGER_STATE_T*       chargerState;                 //0x00000060
} PMD_MAP_ACCESS_T; //Size : 0x64

//powerInfo
#define PMD_MAP_ACCESS_T_POWER_SHARED_LINK_FLAG (1<<31)
#define PMD_MAP_ACCESS_T_POWER_VALUE(n) (((n)&0x7FFFFFFF)<<0)






// =============================================================================
// pmd_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID pmd_RegisterYourself(VOID);



#endif

