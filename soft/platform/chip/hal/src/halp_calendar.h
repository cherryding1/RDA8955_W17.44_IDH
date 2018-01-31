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



#ifndef _HALP_CALENDAR_H_
#define _HALP_CALENDAR_H_

// =============================================================================
// hal_TimRtcIrqHandler
// -----------------------------------------------------------------------------
/// This function is the IRQ handler for the IRQ module called when a RTC/calendar
/// IRQ occurs. It clears the IRQ and calls the user handler if available.
// =============================================================================
PROTECTED VOID hal_TimRtcIrqHandler(UINT8 interruptId);



// =============================================================================
// hal_TimRtcAlarmReached
// -----------------------------------------------------------------------------
/// @return \c TRUE if the Alarm has been reached.
// =============================================================================
PROTECTED BOOL hal_TimRtcAlarmReached(VOID);


#endif // _HALP_CALENDAR_H_

