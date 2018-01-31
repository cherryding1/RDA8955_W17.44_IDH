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



#ifndef _ASICSIMU_M_H_
#define _ASICSIMU_M_H_

#include "cs_types.h"
#include "global_macros.h"
#include "tester_chip.h"

/// @mainpage ASICSimu
/// @author Coolsand Technologies, Inc.
/// @version $Revision: 32439 $
/// @date $Date: 2016-06-07 16:38:14 +0800 (Tue, 07 Jun 2016) $
///
/// This document describes the characteristics of the ASIC Simulation Driver Interface
///
/// The ASICSimu module gives the interface to access ASIC Simulation specific
/// drivers (mainly the tester_chip hardware module)

/// @defgroup ASICSimu
/// @{
///


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// asicsimu_Open
// -----------------------------------------------------------------------------
/// This function configure the access to the tester chip.
/// It must be called BEFORE starting the BCPU to ensure the init is correct.
// =============================================================================
PUBLIC VOID asicsimu_Open(VOID);

// =============================================================================
// asicsimu_Print
// -----------------------------------------------------------------------------
/// This function Prints a string on the simulator output terminal.
/// @param string the message to print (NO FORMATING)
/// NOTE: the print in multi cpu safe (it can safely be called by XCPU or BCPU)
/// however it is not Interrupt (Task&job) safe: once a print is started,
/// starting a new print with the same CPU will result in an endless loop !
/// either use critical section arround it or make sure the interrupt code is
/// not using it.
// =============================================================================
PUBLIC VOID asicsimu_Print(CHAR * string);

// =============================================================================
// asicsimu_Stop
// -----------------------------------------------------------------------------
/// This function stop the simulation
/// @param success whether or not the simulation is succesfull
// =============================================================================
PUBLIC VOID asicsimu_Stop(BOOL success);


// =============================================================================
// asicsimu_Fatal
// -----------------------------------------------------------------------------
/// This function Prints a string on the simulator output terminal.
/// @param string the message to print (NO FORMATING)
/// Note: - This function is not CPU safe (as #asicsimu_Print is)
///       - This function will force #asicsimu_Open if not done
///       - This fucntion will steal the print resource to ensure the message
///         is printed
///       - This function will ternimate on error as asicsimu_Stop(FALSE)
///       .
// =============================================================================
PUBLIC VOID asicsimu_Fatal(CHAR * string);


// =============================================================================
// asicsimu_SetWatch
// -----------------------------------------------------------------------------
/// This function set the Watch register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetWatch(UINT16 value);

// =============================================================================
// asicsimu_GetWatch
// -----------------------------------------------------------------------------
/// This function Get the Watch register value
/// @return the read value
// =============================================================================
PUBLIC UINT16 asicsimu_GetWatch(VOID);

// =============================================================================
// asicsimu_SetW0
// -----------------------------------------------------------------------------
/// This function set the W0 register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetW0(UINT32 value);

// =============================================================================
// asicsimu_GetW0
// -----------------------------------------------------------------------------
/// This function Get the W0 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW0(VOID);


// =============================================================================
// asicsimu_SetW1
// -----------------------------------------------------------------------------
/// This function set the W1 register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetW1(UINT32 value);

// =============================================================================
// asicsimu_GetW1
// -----------------------------------------------------------------------------
/// This function Get the W1 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW1(VOID);


// =============================================================================
// asicsimu_SetW2
// -----------------------------------------------------------------------------
/// This function set the bit to 1 in W2 register
/// @param value the bits to set to 1
// =============================================================================
PUBLIC VOID asicsimu_SetW2(UINT32 value);

// =============================================================================
// asicsimu_ClrW2
// -----------------------------------------------------------------------------
/// This function clear the bit to 0 in W2 register
/// @param value the bits to clear to 0
// =============================================================================
PUBLIC VOID asicsimu_ClrW2(UINT32 value);

// =============================================================================
// asicsimu_GetW2
// -----------------------------------------------------------------------------
/// This function Get the W2 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW2(VOID);

///  @} <- End of the ASICSimu group

#endif // _ASICSIMU_M_H_

