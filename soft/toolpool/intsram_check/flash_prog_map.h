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
    FPC_PROGRAM                                 = 0x00000001,
    FPC_ERASE_SECTOR                            = 0x00000002,
    FPC_ERASE_CHIP                              = 0x00000003,
    FPC_END                                     = 0x00000004,
    FPC_CHECK_FCS                               = 0x00000005,
    FPC_GET_FINALIZE_INFO                       = 0x00000006,
    FPC_RESTART                                 = 0x00000007,
    FPC_CHECK_CALIBRFPART_CRC                   = 0x00000008,
    FPC_DONE                                    = 0xFFFFFF9C,
    FPC_ERROR                                   = 0xFFFFFF9D,
    FPC_FCS_ERROR                               = 0xFFFFFF9E,
    FPC_FLASH_NOT_AT_FF                         = 0xFFFFFF9F
} FPC_COMMAND_TYPE_T;

#define EVENT_FLASH_PROG_READY                   (0XF0)
#define EVENT_FLASH_PROG_ERROR                   (0XE0)
#define EVENT_FLASH_PROG_UNKNOWN                 (0XD0)
#define EVENT_FLASH_PROG_MEM_RESET               (0XC0)
#define EVENT_FLASH_PROG_MEM_ERROR               (0XCE)
#define EVENT_CALIB_RFCRC_ERROR                  (0XB0)
/// Size of one of the three available data buffers
#define FPC_BUFFER_SIZE                          (32*1024)
#define FPC_PROTOCOL_MAJOR                       (0XFA01)
#define FPC_PROTOCOL_MINOR                       (0XFB04)

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

