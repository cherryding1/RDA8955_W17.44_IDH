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





#ifndef __PAL_FLHR_H__
#define __PAL_FLHR_H__

#include "sxs_type.h"
#include "sxs_flsh.hp"
#include "sxs_lib.h"
#include "sxr_ops.h"

#define PAL_RAM_FLASH_NB_SECT   6
#define PAL_RAM_FLASH_SECT_SIZE 2048
#define PAL_RAM_FLASH_SIZE      (PAL_RAM_FLASH_NB_SECT * PAL_RAM_FLASH_SECT_SIZE)


UINT16 *pal_RamFlash;

#define PAL_SECTOR_TO_ADD(Sect) (pal_RamFlash + ((Sect * PAL_RAM_FLASH_SECT_SIZE) / 2))


void pal_RamFlhWrite (UINT16 *Address, UINT16 Data);
void pal_RamFlhEraseSector (void *Address);
void pal_RamFlhUnLock (UINT8 *SectorAddress);
void pal_RamFlhLock (UINT8 *SectorAddress);
void pal_RamFlhIdentification (sxs_FlashId_t *Flash);



UINT16 *pal_RamSectorToAdd (UINT8 Sect)
{
    return PAL_SECTOR_TO_ADD(Sect);
}


sxs_FlashDesc_t pal_RamFlashDesc =

{
    {
        0,
        0,
    },

    pal_RamSectorToAdd,

    pal_RamFlhIdentification,
    pal_RamFlhLock,
    pal_RamFlhUnLock,
    pal_RamFlhEraseSector,
    pal_RamFlhWrite,

    PAL_RAM_FLASH_NB_SECT,

    {
        5,
        1,
        0,
        PAL_RAM_FLASH_SECT_SIZE,
        0, // PAL_SECTOR_TO_ADD(5),
        0, // PAL_SECTOR_TO_ADD(6) - 1
    },
    {
        {
            4,
            1,
            PAL_RAM_FLASH_SECT_SIZE - 256,
            PAL_RAM_FLASH_SECT_SIZE,
            0, // PAL_SECTOR_TO_ADD(4),
            0, // PAL_SECTOR_TO_ADD(5) - 1
        },
        {
            3,
            1,
            PAL_RAM_FLASH_SECT_SIZE - 256,
            PAL_RAM_FLASH_SECT_SIZE,
            0, // PAL_SECTOR_TO_ADD(3),
            0, // PAL_SECTOR_TO_ADD(4) - 1
        }
    },
    {
        0,
        3,
        100,
        3 * PAL_RAM_FLASH_SECT_SIZE,
        0, // PAL_SECTOR_TO_ADD(0),
        0  // PAL_SECTOR_TO_ADD(3) - 1
    }
};



/*
==============================================================================
   Function   : pal_RamFlhIdentification
 -----------------------------------------------------------------------------
   Scope      : Flash type detection
   Parameters : Pointer on sxs_FlashId_t.
   Return     : Structure sxs_FlashId_t is filled with found data.
==============================================================================
*/
void pal_RamFlhIdentification (sxs_FlashId_t *Flash)
{
    if (pal_RamFlash == NIL)
        pal_RamFlash = (UINT16 *)sxr_HMalloc (PAL_RAM_FLASH_SIZE);

    pal_RamFlashDesc.StaticBnk.StartAdd      = PAL_SECTOR_TO_ADD(5);
    pal_RamFlashDesc.StaticBnk.LastAdd       = PAL_SECTOR_TO_ADD(6) - 1;

    pal_RamFlashDesc.DynamicBnk [0].StartAdd = PAL_SECTOR_TO_ADD(4);
    pal_RamFlashDesc.DynamicBnk [0].LastAdd  = PAL_SECTOR_TO_ADD(5) - 1;

    pal_RamFlashDesc.DynamicBnk [1].StartAdd = PAL_SECTOR_TO_ADD(3);
    pal_RamFlashDesc.DynamicBnk [1].LastAdd  = PAL_SECTOR_TO_ADD(4) - 1;

    pal_RamFlashDesc.StackedBnk.StartAdd     = PAL_SECTOR_TO_ADD(0);
    pal_RamFlashDesc.StackedBnk.LastAdd      = PAL_SECTOR_TO_ADD(3) - 1;

    Flash -> ManufCode = 0;
    Flash -> DeviceId  = 0;
}


/*
==============================================================================
   Function   : pal_RamFlhLock
 -----------------------------------------------------------------------------
   Scope      : Lock a sector.
   Parameters : Sector address.
   Return     : none
==============================================================================
*/
void pal_RamFlhLock (UINT8 *SectorAddress)
{
}


/*
==============================================================================
   Function   : pal_RamFlhUnLock
 -----------------------------------------------------------------------------
   Scope      : UnLock a sector.
   Parameters : Sector address.
   Return     : none
==============================================================================
*/
void pal_RamFlhUnLock (UINT8 *SectorAddress)
{
}


/*
==============================================================================
   Function   : pal_RamFlhEraseSector
 -----------------------------------------------------------------------------
   Scope      : Erase a sector.
   Parameters : Sector address.
   Return     : none
==============================================================================
*/
void pal_RamFlhEraseSector (void *Address)
{
    memset (Address, 0xff, PAL_RAM_FLASH_SECT_SIZE);
}


/*
==============================================================================
   Function   : pal_RamFlhWrite
 -----------------------------------------------------------------------------
   Scope      : Write a word data at the defined address.
   Parameters : Address and Word data.
   Return     : none
==============================================================================
*/
void pal_RamFlhWrite (UINT16 *Address, UINT16 Data)
{
    *Address = Data;
}


#endif

