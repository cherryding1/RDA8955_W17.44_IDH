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



#ifndef _FACTORY_H_
#define _FACTORY_H_

#define TGT_FACTORY_BLOCK_SIZE 1024

// =============================================================================
//  MACROS
// =============================================================================
#define FACTORY_VERSION                         (0XFAC00200)
#define FACTORY_BLOCK_SIZE                    (1024)
#define FACT_SIM_COUNT                           (4)
#define FACT_IMEI_LEN                               (9)
#define FACT_SN_LEN                                  (64)
#define FACT_MAC_LEN                                (6)
#define FACT_BT_MAC_FLAG                       (0X1D0E)
#define FACT_STATION_COUNT                   (15)
#define FACT_STATION_NAME_LEN             (12)
#define FACT_STATION_DESC_LEN              (32)
#define FACT_SUPPLEMENTARY_LEN             (544)
#define FACT_DEVICENAME_LEN                   (32)
#define FACT_PINCODE_LEN                          (16)

#define FIELD_OFFSETOF(TYPE, MEMBER) ((UINT32) &((TYPE *)0)->MEMBER)
#define FIELD_SIZE(TYPE, MEMBER) (sizeof(((TYPE *)0)->MEMBER))
// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// FACT_IMEI_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT8 FACT_IMEI_T[FACT_IMEI_LEN];


// ============================================================================
// FACT_SN_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT8 FACT_SN_T[FACT_SN_LEN];


// ============================================================================
// FACT_MAC_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT16                         activated;                    //0x00000000
    UINT8                          mac[FACT_MAC_LEN];            //0x00000002
} FACT_MAC_T; //Size : 0x8



// ============================================================================
// FACT_STATION_NAME_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT8 FACT_STATION_NAME_T[FACT_STATION_NAME_LEN];

// =============================================================================
// TGT_BT_INFO_T
// -----------------------------------------------------------------------------
/// The structure contains the bluetooth device info
// =============================================================================
typedef struct
{
    UINT16 activated;
    UINT8 dev_addr[FACT_MAC_LEN];
    UINT8 dev_name[FACT_DEVICENAME_LEN];
    UINT8 pin_code[FACT_PINCODE_LEN];
} TGT_BT_INFO_T;

// =============================================================================
// TGT_WIFI_INFO_T
// -----------------------------------------------------------------------------
/// The structure contains the wifi device info
// =============================================================================
typedef struct
{
    UINT16 activated;
    UINT8  mac_addr[FACT_MAC_LEN];
    UINT8  mac_ap1[FACT_MAC_LEN];
    UINT8  mac_ap2[FACT_MAC_LEN];
    UINT8  mac_ap3[FACT_MAC_LEN];
    UINT8  reserved[2];
} TGT_WIFI_INFO_T;

// ============================================================================
// FACTORY_BLOCK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// Magic and version number. The lsb 8bits is minor version. When this structure
    /// is changed, major version must be upgraded. When supplementary content is
    /// changed, it is recommended to upgrade minor version.
    UINT32                         version;                      //0x00000000
    FACT_IMEI_T                    imeiSv[FACT_SIM_COUNT];       //0x00000004
    UINT8                          mbSn[FACT_SN_LEN];            //0x00000028
    UINT8                          mpSn[FACT_SN_LEN];            //0x00000068
    TGT_BT_INFO_T           btInfo;                                 //0x000000A8
    TGT_WIFI_INFO_T        wifiInfo;                               //0x000000E0
    FACT_STATION_NAME_T            stationNames[FACT_STATION_COUNT]; //0x000000FC
    /// Indicate whether a station is performed. 1: performed
    UINT16                         stationPerformed;             //0x000001B0
    /// Indicate whether a station is passed, only used when performed flags is 1.
    /// 1: passed, 0: failed
    UINT16                         stationPassed;                //0x000001B2
    UINT8                          stationLastDesc[FACT_STATION_DESC_LEN]; //0x000001B4
    /// Area for customized information.
    UINT8                          supplementary[FACT_SUPPLEMENTARY_LEN]; //0x000001D4
    /// Sequence number for block management. For error protection, bit [0] is reserved
    /// as zero (otherwise will be regarded as invalid). So, the initial sequence
    /// number will be 0, and increase step is 2.
    UINT32                         sequence;                     //0x000003F4
    /// CRC of the whole block, and 'crc' itself is not included. From the beginning
    /// of block, and length is BLOCK_SIZE-8.
    UINT32                         crc;                          //0x000003F8
    /// Bit inverted of CRC. It is to enhance CRC check robustness.
    UINT32                         crcInverted;                  //0x000003FC
} FACTORY_BLOCK_T; //Size : 0x400


// =============================================================================
//  APIS
// =============================================================================

INT32 FACTORY_Init(VOID);
INT32 FACTORY_ReadBlock(FACTORY_BLOCK_T *facory_blk);
INT32 FACTORY_WriteBlock(FACTORY_BLOCK_T *facory_blk);
INT32 FACTORY_ReadSupple(UINT8 *buff);
INT32 FACTORY_WriteSupple(UINT8 *buff);
INT32 FACTORY_ReadField(UINT32 offset,UINT32 size,UINT8* buff);
INT32 FACTORY_WriteField(UINT32 offset,UINT32 size,UINT8* buff);
INT32 FACTORY_SyncReadField(UINT32 offset,UINT32 size,UINT8* buff);
INT32 FACTORY_SyncWriteField(UINT32 offset,UINT32 size,UINT8* buff);

// =============================================================================
//  Error code.
// =============================================================================

#define ERR_FACTORY_SUCCESS                                 0
#define ERR_FACTORY_NOT_INIT                              1001

STATIC_ASSERT((sizeof(FACTORY_BLOCK_T) == TGT_FACTORY_BLOCK_SIZE), "Invalid TGT_FACTORY_BLOCK_T size.");
/*
#if (sizeof(FACTORY_BLOCK_T) != TGT_FACTORY_BLOCK_SIZE)
#error "Invalid TGT_FACTORY_BLOCK_T size"
#endif
*/
#endif

