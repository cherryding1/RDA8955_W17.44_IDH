/* 
 * Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file lcdd_config.h                                                       //
/// That file describes the configuration of the structure used to set up     //
/// the AU driver stored in TGT.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"
#include "hal_spi.h"
#include "hal_gpio.h"


#ifndef _MCD_CONFIG_H_
#define _MCD_CONFIG_H_



// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// MCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Configuration structure for the MCD_SPI driver CS
// =============================================================================
typedef struct
{
    HAL_SPI_CS_T            usedSpiCs;
    HAL_GPIO_GPIO_ID_T      tmpGpioOverCs;
} MCD_CONFIG_T;




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//  

// =============================================================================
//  FUNCTIONS
// =============================================================================

#endif // _MCD_CONFIG_H_


