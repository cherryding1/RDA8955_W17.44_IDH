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



#ifndef _HALP_HOST_H_
#define _HALP_HOST_H_

#include "cs_types.h"



// =============================================================================
// hal_HstMonitor
// -----------------------------------------------------------------------------
/// Main host monitor function. Read the command passed to the platform through
/// the Host port and call the host command handler if appropriate.
/// It read the H2P register to execute commands
/// until the Exit command is received (BOOT_HST_MONITOR_END_CMD).
// =============================================================================
PROTECTED VOID hal_HstMonitor(VOID);



// =============================================================================
// hal_HstMonitorX
// -----------------------------------------------------------------------------
/// That function execute the function pointed by the execution context
/// #hal_HstMonitorXCtx.
// =============================================================================
PROTECTED  VOID hal_HstMonitorX(VOID);




// =============================================================================
// hal_HstRamClientIdle
// -----------------------------------------------------------------------------
/// Read the H2P exchange register to retrieve host commands. If any supported
/// one found (type #BOOT_HST_CMD_T), it is executed during the idle task.
// =============================================================================
PROTECTED VOID hal_HstRamClientIdle(VOID);

// =============================================================================
// hal_HstRamClientIrq
// -----------------------------------------------------------------------------
/// Executed on IRQ from the Host. It reads the H2P exchange register to retrieve
/// host commands. If any supported one is found (type #BOOT_HST_CMD_T), it is
/// executed immediatly. Takes care of Critical Section breaking and coherence
/// etc ...
// =============================================================================
PROTECTED VOID hal_HstRamClientIrq(VOID);

// =============================================================================
// hal_HstSleep
// -----------------------------------------------------------------------------
/// Release the Host Clock Resource so system can go to low power
// =============================================================================
PROTECTED VOID hal_HstSleep(VOID);

// =============================================================================
// hal_HstWakeup
// -----------------------------------------------------------------------------
/// Set the Host Clock Resource as active.  Request 26M clock.
// =============================================================================
PROTECTED VOID hal_HstWakeup(VOID);


#endif // _HALP_HOST_H_
