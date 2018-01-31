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




#ifndef _FMG_M_H_
#define _FMG_M_H_

/// @mainpage Font Manager API
/// The FMG Font Manager Service handles the display of basic text
/// on the mobile phone screen, mainly in order of debug and test improved
/// conviviality.
///
/// It will never aim at providing much more than a printf. For more advanced
/// stuff, please go and see the MMI stuff and guys.
///
/// The FMG group is there: @ref fmg
///
/// @defgroup fmg Font Manager (FMG) API description.
///
/// @par This document describes the functions available to basically display
/// text on the phone screen. This should work with any LCD driver, provided
/// that there's a LCD screen configured somewhere.
///
///
/// @{
///

#include "cs_types.h"

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// fmg_PrintfInit
// -----------------------------------------------------------------------------
/// Initializes the printf mechanisms.
/// @param rstX Initial x coordinate - Beginning of any new line.
/// @param rstY Initial y coordinate - First line of a new screen.
/// @param graphX Printable zone last pixel x coordinate. This and graphY are the
/// only coordinates expressed in pixels. All the other ones are in characters.
/// @param graphY Printable zone last pixel y coordinate. This and graphX are the
/// only coordinates expressed in pixels. All the other ones are in characters.
// =============================================================================
PUBLIC UINT32 fmg_PrintfInit(UINT16 rstX, UINT16 rstY, UINT16 graphX, UINT16 graphY);



// =============================================================================
// fmg_PutChar
// -----------------------------------------------------------------------------
/// Display one char at the current cursor position.
/// @param chr Charater to display.
// =============================================================================
PUBLIC VOID fmg_PutChar(UINT8 chr);



// =============================================================================
// fmg_Printf
// -----------------------------------------------------------------------------
/// Classical printf function.
// =============================================================================
PUBLIC UINT32 fmg_Printf(CONST CHAR* fmt, ...);



// =============================================================================
// fmg_PrintfString
// -----------------------------------------------------------------------------
/// Print a string starting at the current cursor position.
/// @param pString String to print.
// =============================================================================
PUBLIC UINT32 fmg_PrintfString(UINT8* pString);



// =============================================================================
// fmg_PrintfSetXY
// -----------------------------------------------------------------------------
/// Set the cursor to the (xPos, yPos) point. (Coordinates are expressed as number
/// of characters).
/// @param xPos X coordinate.
/// @param yPos Y coordinate.
// =============================================================================
PUBLIC VOID fmg_PrintfSetXY(UINT8 xPos, UINT8 yPos);



// =============================================================================
// fmg_PrintfClrScr
// -----------------------------------------------------------------------------
/// Clear the screen used to display the printfed text.
// =============================================================================
PUBLIC VOID fmg_PrintfClrScr(VOID);


/// @} <-- End of fmg group
#endif // _FMG_M_H_ 


