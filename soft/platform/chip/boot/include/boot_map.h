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


#ifndef _BOOT_MAP_H_
#define _BOOT_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// BOOT_MAIN_TAG_T
// -----------------------------------------------------------------------------
/// Magic tags used by the boot loader to switch between boot modes.
// =============================================================================
typedef enum
{
    BOOT_MAIN_TAG_NORMAL                        = 0x00000000,
    BOOT_MAIN_TAG_BREAKBOOT                     = 0xB4EAB007,
    BOOT_MAIN_TAG_CALIB                         = 0xCA1BCA1B
} BOOT_MAIN_TAG_T;


// ============================================================================
// BOOT_SECTOR_CMD_T
// -----------------------------------------------------------------------------
/// List of Boot Sector commands.
// =============================================================================
typedef enum
{
/// The Boot Sector must jump into the ROMed Boot Monitor.
    BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR          = 0x4E6A821C,
/// The Boot Sector must jump into the calib daemon.
    BOOT_SECTOR_CMD_ENTER_CALIB                 = 0x4E6A8215
} BOOT_SECTOR_CMD_T;

/// Value written in the Boot Sector structure to say that the EBC configuration
/// present in this structure is valid and can be programmed. Used for the field
/// ebcConfigValidTag.
#define BOOT_SECTOR_EBC_VALID_TAG                (0XB0075EC7)
/// Value written in the Boot Sector structure to say that the EBC extension configuration
/// present in this structure is valid and can be programmed. Used for the field
/// ebcConfigValidTag.
#define BOOT_SECTOR_EBC_EXT_VALID_TAG            (0XEBC00E28)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BOOT_SECTOR_STRUCT_T
// -----------------------------------------------------------------------------
/// Structure type used by the Boot Sector to determine in which mode it must boot
/// and how the EBC RAM must be configured (this is useful in case of burst mode
/// RAM). This structure contains information written by HAL during the normal execution
/// of the code and used by the Boot Sector when the phone boots.
// =============================================================================
typedef struct
{
    BOOT_SECTOR_CMD_T              command;                      //0x00000000
    UINT32                         ebcConfigValidTag;            //0x00000004
    UINT32                         ebcConfigRamTimings;          //0x00000008
    UINT32                         ebcConfigRamMode;             //0x0000000C
    UINT32                         ebcExtConfigValidTag;         //0x00000010
    UINT32                         ebcFreq;                      //0x00000014
    UINT8                          ramId;                        //0x00000018
    UINT8                          reserved[2];                  //0x00000019
    UINT8                          ebcConfigVersion;             //0x0000001B
} BOOT_SECTOR_STRUCT_T; //Size : 0x1C



// ============================================================================
// BOOT_MAP_GLOBALS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
} BOOT_MAP_GLOBALS_T; //Size : 0x0





#endif

