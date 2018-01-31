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



#include "cs_types.h"
#include "chip_id.h"
#include "string.h"
#include "patch_m.h"

#include "global_macros.h"
#include "mem_bridge.h"


/// Only used by patch_GetLocation().
PRIVATE UINT32 g_patchUsed = 0;


// =============================================================================
// patch_GetLocation
// -----------------------------------------------------------------------------
/// This function allocate a hardware patch, it then need to be enabled with
/// #patch_Enable.
/// Use this function to get a patch location when #patch_ReplaceFunction cannot
/// be used for your patch (patch data or patch assembly code)
/// @param patchArea    pointer to recieve the address of the patch area
///                     It can be null.
/// @return             the number of the hardware patch or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_GetLocation(UINT32** patchArea)
{
    UINT32 patch = g_patchUsed;

    // reserve one patch for GDB
    // and reserve the last patch for the Boot Sector structure.
    if (CHIP_NUMBER_OF_PATCHES-1 < g_patchUsed)
    {
        return PATCH_FAILED;
    }

    if(NULL != patchArea)
    {
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
        *patchArea = (UINT32*)(KSEG0(REG_BB_PATCH_BASE)) + 4*patch;
#else // 8805 or 8806, 8810
        *patchArea = (UINT32*)(KSEG0(REG_INT_SRAM_BASE)) + 4*patch;
#endif
    }

    g_patchUsed = g_patchUsed + 1;

    return patch;
}


// =============================================================================
// patch_Enable
// -----------------------------------------------------------------------------
/// This function enables a hardware patch.
/// Once the replacement code is placed in internal sram, use this function to
/// enable the hardware patch.
/// @param patchNumber  the number of the patch as returned by #patch_GetLocation
/// @param patchAddress the address in the rom to patch (will be 16 bytes aligned)
// =============================================================================
PUBLIC VOID patch_Enable(UINT32 patchNumber, UINT32 patchAddress)
{
    // assert patchAddress is in rom !
    // ((patchAddress & 0x00F00000) == REG_INT_ROM_BASE)
    //
    // assert patch exist
    // (patchNumber < NB_ROM_PACHT)

    // enable patch
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || (CHIP_HAS_BCPU_ROM)
    hwp_bbMemBridge->Rom_Patch[patchNumber] = MEM_BRIDGE_PATCH_ENABLE
            | ((UINT32)patchAddress & MEM_BRIDGE_BLOCK_ADDR_MASK);// tcj
#else // 8805 or 8806, 8810
    hwp_memBridge->Rom_Patch[patchNumber] = MEM_BRIDGE_PATCH_ENABLE
                                            | ((UINT32)patchAddress & MEM_BRIDGE_BLOCK_ADDR_MASK);
#endif
}


// =============================================================================
// patch_ReplaceFunction
// -----------------------------------------------------------------------------
/// This function install and enable a patch to replace a function.
/// The function in ROM must be aligned on 16 bytes boundaries.
/// The two functions must have the exact same prototype !
/// The patch will actually replace \p oldFunction by a direct jump
/// to \p newFunction.
/// @param oldFunction  the address/symbol of the function in ROM to replace.
/// @param newFunction  the address/symbol of the new implementation of the
///                     function.
/// @return             the number of the allocated hardware patch
///                     or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_ReplaceFunction(CONST VOID (*oldFunction)(VOID),
                                   CONST VOID (*newFunction)(VOID))
{
    UINT32  oldFunctionPtr = (UINT32)oldFunction;
    UINT32  newFunctionPtr = (UINT32)newFunction;

    UINT32 *patchArea = NULL;
    INT32   patch;

    patch = patch_GetLocation(&patchArea);

    if(PATCH_FAILED == patch)
    {
        return PATCH_FAILED;
    }

    // assert oldFunction is in rom !
    // ((oldFunctionPtr & 0x00F00000) == REG_INT_ROM_BASE)
    //
    // assert oldFunction must be alligned on 16 bytes boundaries (patch area)
    // ((oldFunctionPtr & 0xe) == 0)

    // newFunction can be on any ISA Mode (Mips 16 or Mips 32)
    // as we use a jr in the patch code

    if ((oldFunctionPtr & 1) == 1)
    {
        // mips16 old function

        // BUG in 8808s/8809:
        // hwp_bbMemBridge->Rom_Patch cannot be written in 16-bit or 8-bit mode
#if 1
        // assemble: lw v0, 12(pc)
        // assemble: nop (lw delay before v0 use)
        patchArea[0] = 0x6500b203;
        // assemble: jr v0
        // assemble: nop (jr delay slot)
        patchArea[1] = 0x6500ea00;
#else
        // assemble: lw v0, 12(pc)
        ((UINT16*)patchArea)[0]=0xb203;
        // assemble: nop (lw delay before v0 use)
        ((UINT16*)patchArea)[1]=0x6500;
        // assemble: jr v0
        ((UINT16*)patchArea)[2]=0xea00;
        // assemble: nop (jr delay slot)
        ((UINT16*)patchArea)[3]=0x6500;
#endif
        // place data for lw instruction
        patchArea[3] = newFunctionPtr;

    }
    else
    {
        // mips32 old function

        // assemble: lui at, hi(newFunctionPtr)
        patchArea[0] = 0x3c010000 + (newFunctionPtr >> 16);
        // assemble: ori at, at, lo(newFunctionPtr)
        patchArea[1] = 0x34210000 + (newFunctionPtr & 0xffff);
        // assemble: jr at
        patchArea[2] = 0x00200008;
        // assemble: nop
        patchArea[3] = 0;

    }
    // enable patch
    patch_Enable(patch, oldFunctionPtr);

    return patch;
}


// =============================================================================
// patch_ReplaceData
// -----------------------------------------------------------------------------
/// This function install and enable a patch to replace some rom data.
/// The data in ROM must be aligned on 16 bytes boundaries.
/// @param oldDataAddr  the address of the data in ROM to replace.
/// @param newData      pointing to the new data.
/// @param len          the length of the new data in bytes and must be 16.
/// @return             the number of the allocated hardware patch
///                     or \c PATCH_FAILED
// =============================================================================
PUBLIC INT32 patch_ReplaceData(CONST VOID *oldDataAddr,
                               CONST VOID *newData,
                               UINT32 len)
{
    UINT32 *patchArea = NULL;
    INT32   patch;
    CONST UINT8 *data = (UINT8 *)newData;

    // One patch will replace 16 bytes exactly
    if (len != 16)
    {
        return PATCH_FAILED;
    }

    patch = patch_GetLocation(&patchArea);

    if(PATCH_FAILED == patch)
    {
        return PATCH_FAILED;
    }

    // assert oldDataAddr is in rom !
    // ((oldDataAddr & 0x00F00000) == REG_INT_ROM_BASE)
    //
    // assert oldDataAddr must be alligned on 16 bytes boundaries (patch area)
    // ((oldFunctionPtr & 0xe) == 0)

    // 32-bit writes
    patchArea[0] = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);
    data += 4;
    patchArea[1] = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);
    data += 4;
    patchArea[2] = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);
    data += 4;
    patchArea[3] = data[0]|(data[1]<<8)|(data[2]<<16)|(data[3]<<24);

    // enable patch
    patch_Enable(patch, (UINT32)oldDataAddr);

    return patch;
}

