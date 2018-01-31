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


#ifndef _FLASH_PROG_MAP_H_
#define _FLASH_PROG_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// FPC_COMMAND_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    FPC_NONE                                    = 0x00000000,
/// Program with specified flash offset and size. Size will be truncated into
/// 16bits.
    FPC_PROGRAM                                 = 0x00000001,
/// Erase sector with specified flash offset. The effective sector start and
/// size will be determined by flash layout configured in ramrun.
    FPC_ERASE_SECTOR                            = 0x00000002,
/// Not implemented
    FPC_ERASE_CHIP                              = 0x00000003,
/// When flash boot sector is written, set boot magic. Otherwise do nothing.
    FPC_END                                     = 0x00000004,
/// Verify FCS checksum.
    FPC_CHECK_FCS                               = 0x00000005,
/// Write into ram all the magic numbers and their addresses
    FPC_GET_FINALIZE_INFO                       = 0x00000006,
/// Restart chip
    FPC_RESTART                                 = 0x00000007,
/// Check caliberation RF part CRC
    FPC_CHECK_CALIBRFPART_CRC                   = 0x00000008,
/// Dummy command. It can be used to determine whether FPC is alive, and the
/// next command index.
    FPC_PING                                    = 0x00000009,
/// Erase and program. It is useful for better pipeline. Also, caller specified
/// sector/block size will be used rather than configured flash layout. Only 4K
/// and 64K are supported. Caller should ensure alignment. "fcs" field in
/// command data will be interpreted as 16bits mask, to indicate whether each
/// 16th piece should be programmed.
    FPC_ERASE_PROGRAM                           = 0x0000000A,
/// Erase with caller specified sector/block size rather than configured flash
/// layout. Only 4K and 64K are supported. Caller should ensure alignment.
    FPC_ERASE                                   = 0x0000000B,
/// Enable camera flash. Usally, it is used to indicate FPC finish.
    FPC_LIGHT_ON                                = 0x0000000C,
/// Overwrite a portion of sector(s). Ramrun should read the sector, and just
/// replace the specified data and write back. It can save time to read sector
/// data to PC. The address and size can be un-aligned. The total size
/// (including unsignment offset) must be less than (FPC_BUFFER_SIZE - 4K).
    FPC_OVERWRITE                               = 0x0000000D,
/// Read valid factory block version 1 (0xFAC000100). It will scan valid factory
/// block based on version 1 definition. When no valid factory block is found,
/// all 0xFF block is returned. FPC will only take care of factory block
/// management, and won't look into details of factory block. Namely, only
/// crc/version/sequence are interested.
    FPC_FACTORY_READ                            = 0x0000000E,
/// Write factory block with version 1 block management. Also, block management
/// information will be built at FPC_FACTORY_READ. So, it is needed to call
/// FPC_FACTORY_READ beforehand.
    FPC_FACTORY_WRITE                           = 0x0000000F,
/// Shutdown the device once charger is plugged out.
    FPC_SHUTDOWN                                = 0x00000010,
/// Set the boot sector flash address.
    FPC_SET_BOOT_SECTOR                         = 0x00000011,
/// Unset the boot sector flash address.
    FPC_UNSET_BOOT_SECTOR                       = 0x00000012,
/// Initial goke flash tool.
    FPC_GKFT_INITIAL                            = 0x00480000,
/// Upload goke image file.
    FPC_GKFT_UPDATE_IMAGE                       = 0x00480001,
/// Verify the image file.
    FPC_GKFT_VERIFY_IMAGE                       = 0x00480002,
/// Burn the image into the chip.
    FPC_GKFT_BURN_IMAGE                         = 0x00480003,
    FPC_DONE                                    = 0xFFFFFF9C,
    FPC_ERROR                                   = 0xFFFFFF9D,
    FPC_FCS_ERROR                               = 0xFFFFFF9E,
    FPC_FLASH_NOT_AT_FF                         = 0xFFFFFF9F
} FPC_COMMAND_TYPE_T;

#define EVENT_FLASH_PROG_READY                  (0XF0)
#define EVENT_FLASH_PROG_ERROR                  (0XE0)
#define EVENT_FLASH_PROG_UNKNOWN                (0XD0)
#define EVENT_FLASH_PROG_MEM_RESET              (0XC0)
#define EVENT_FLASH_PROG_MEM_ERROR              (0XCE)
#define EVENT_CALIB_RFCRC_ERROR                 (0XB0)
/// Size of one of the three available data buffers
#define FPC_BUFFER_SIZE                         (64*1024)
#define FPC_PROTOCOL_MAJOR                      (0XFA01)
#define FPC_PROTOCOL_MINOR                      (0XFB05)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// FLASH_PROGRAMMER_GLOBALS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
} FLASH_PROGRAMMER_GLOBALS_T; //Size : 0x0



// ============================================================================
// FPC_COMMAND_DATA_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    FPC_COMMAND_TYPE_T             cmd;                          //0x00000000
    UINT8*                         flashAddr;                    //0x00000004
    UINT8*                         ramAddr;                      //0x00000008
    UINT32                         size;                         //0x0000000C
    UINT32                         fcs;                          //0x00000010
} FPC_COMMAND_DATA_T; //Size : 0x14



// ============================================================================
// FPC_PROTOCOL_VERSION_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT16                         major;                        //0x00000000
    UINT16                         minor;                        //0x00000002
} FPC_PROTOCOL_VERSION_T; //Size : 0x4



// ============================================================================
// FLASH_PROG_MAP_ACCESS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    FPC_PROTOCOL_VERSION_T         protocolVersion;              //0x00000000
    FPC_COMMAND_DATA_T             commandDescr[2];              //0x00000004
    UINT8*                         dataBufferA;                  //0x0000002C
    UINT8*                         dataBufferB;                  //0x00000030
    UINT8*                         dataBufferC;                  //0x00000034
    UINT32                         fpcSize;                      //0x00000038
} FLASH_PROG_MAP_ACCESS_T; //Size : 0x3C






#endif

