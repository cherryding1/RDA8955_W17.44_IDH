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



#ifndef  _HALP_SPI_H_
#define  _HALP_SPI_H_

#include "hal_spi.h"




// svn propset svn:keywords "HeadURL Author Date Revision" dummy.h



// =============================================================================
//  MACROS
// =============================================================================

/// Those defines are directly related to the ways the macros representing
/// the transferts are defined : IRQ needing ones are odd. DMA needing ones
/// are 3 or 4 (second bit is then one ...)
#define NEED_IRQ(val) (val & 0x1)
#define IS_DMA(val) (val & 0x2)



// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_SpiIrqHandler
// -----------------------------------------------------------------------------
/// Handler called in case of SPI interrupt
///
/// @param interruptId  Will be spi Hw module interrupt
/// id when used by the IRQ driver.
// =============================================================================
PROTECTED VOID hal_SpiIrqHandler(UINT8 interruptId);

// =============================================================================
// hal_SpiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the SPI divider so that the maximum frequency defined for the currently
/// active CS is not overpassed.
/// @param id Identifier of the SPI for which the function is called.
/// @param sysFreq Newly applied system frequency
// =============================================================================
PROTECTED VOID hal_SpiUpdateDivider(HAL_SPI_ID_T id, HAL_SYS_FREQ_T sysFreq);

#endif //_HALP_SPI_H_


