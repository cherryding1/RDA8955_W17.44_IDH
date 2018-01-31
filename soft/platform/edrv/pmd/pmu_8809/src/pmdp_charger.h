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


#include "pmd_config.h"
#include "pmd_m.h"
#include "pmdp_charger_common.h"


// ============================================================================
//  MACROS
// ============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_BG_LP_MODE_USER_ID_T
// -----------------------------------------------------------------------------
/// BG LP mode users.
// =============================================================================
typedef enum
{
    PMD_BG_LP_MODE_USER_CHARGER,
    PMD_BG_LP_MODE_USER_KEYPAD_LED,
    PMD_BG_LP_MODE_USER_LED0,
    PMD_BG_LP_MODE_USER_LED1,
    PMD_BG_LP_MODE_USER_LED2,
    PMD_BG_LP_MODE_USER_LED3,
    PMD_BG_LP_MODE_USER_FM,
    PMD_BG_LP_MODE_USER_TSC_KEY,

    PMD_BG_LP_MODE_USER_ID_QTY
} PMD_BG_LP_MODE_USER_ID_T;


// =============================================================================
// PMD_CLK4M_AVDD3_USER_ID_T
// -----------------------------------------------------------------------------
/// Clock 4M and AVDD3 users.
// =============================================================================
typedef enum
{
    PMD_CLK4M_AVDD3_USER_CHARGER,
    PMD_CLK4M_AVDD3_USER_KEYPAD_LED,
    PMD_CLK4M_AVDD3_USER_LED0,
    PMD_CLK4M_AVDD3_USER_LED1,
    PMD_CLK4M_AVDD3_USER_LED2,
    PMD_CLK4M_AVDD3_USER_LED3,
    PMD_CLK4M_AVDD3_USER_TSC_KEY,

    PMD_CLK4M_AVDD3_USER_ID_QTY
} PMD_CLK4M_AVDD3_USER_ID_T;


// ============================================================================
//  FUNCTIONS
// ============================================================================

PROTECTED VOID pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_ID_T user, BOOL on);


PROTECTED VOID pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_ID_T user,
        BOOL on);


