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



#ifndef _TGT_H_
#define _TGT_H_

#include "cs_types.h"
#include "factory.h"
#include "tgt_pmd_cfg.h"
// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// TGT_IMEI_T
// -----------------------------------------------------------------------------
/// Type used to represent a IMEI number.
// =============================================================================
typedef UINT8 TGT_IMEI_T[9];

// =============================================================================
// TGT_MOBILE_ID_T
// -----------------------------------------------------------------------------
/// Define the mobile identification structure
/// This structure must be included at the beginning of the factory settings sector
/// The rest of the factory settings sector can be customized to meet the customer needs
// =============================================================================
typedef struct
{
    // Hold a valid status bit for the IMEI, check with a CRC, and markers
    // for the factory setting configuration step status.
    UINT32 validMarker;
    // IMEISV for the sims of the phone. (Hardcoded at most 4 sims.)
    TGT_IMEI_T imeiSv[4];
} TGT_MOBILE_ID_T;

// =============================================================================
// TGT_MOBILE_SN_T
// -----------------------------------------------------------------------------
/// The structure contains the mobile serial numbers
// =============================================================================
typedef struct
{
    // Mother board serial numbers.
    UINT8 mbSn[50];
    // Mobile phone serial numbers.
    UINT8 mpSn[50];
} TGT_MOBILE_SN_T;

// =============================================================================
// TGT_MODULE_ID_T
// -----------------------------------------------------------------------------
/// Define the module identification structure
/// Module IDs are used to speed up LCD and Camera init.
// =============================================================================
typedef struct
{
    UINT8 lcd_id;
    UINT8 cam_id[2];
} TGT_MODULE_ID_T;

// =============================================================================
// TGT_SALES_TRACK_INFO_T
// -----------------------------------------------------------------------------
/// The structure contains the sales tracking info
// =============================================================================
typedef struct
{
    UINT8  WORKMODE;
    UINT8  SWITCH;
    UINT8  SENDONCE;
    UINT8  BATLOSE;
    UINT32 INSIM_TIME;
    UINT32 NORMAL_TIME_LIMT;
    UINT32 TESTMOD_TIME_LIMT;
} TGT_SALES_TRACK_INFO_T;

// =============================================================================
// TGT_HW_MODULE_T
// -----------------------------------------------------------------------------
/// The structure contains the hardware module names
// =============================================================================
typedef struct
{
    CONST CHAR *target;
    CONST CHAR *chipAsic;
    CONST CHAR *chipDie;
    CONST CHAR *rfPa;
    CONST CHAR *flash;
    CONST CHAR *lcd;
    CONST CHAR *cam;
    CONST CHAR *bt;
    CONST CHAR *fm;
    CONST CHAR *atv;
} TGT_HW_MODULE_T;


// =============================================================================
//  FUNCTIONS
// =============================================================================


#if !defined(_T_UPGRADE_PROGRAMMER) && defined(REDUNDANT_DATA_REGION)
PUBLIC UINT32 tgt_get_factory_setting_size(VOID);
#endif

// =============================================================================
// tgt_GetMobileId
// -----------------------------------------------------------------------------
/// This function is used to get the Mobile ID description.
// =============================================================================
PUBLIC INT32 tgt_GetImeiSv(UINT8 simId,UINT8 *imeiSv);

// =============================================================================
// tgt_SetMobileId
// -----------------------------------------------------------------------------
/// This function is used to update the Mobile ID description.
// =============================================================================
PUBLIC INT32 tgt_SetImeiSv(UINT8 simId,UINT8 *imeiSv);

// =============================================================================
// tgt_GetMbSn
// -----------------------------------------------------------------------------
/// This function is used to get the MbSn.
// =============================================================================
PUBLIC INT32 tgt_GetMbSn(UINT8 *mbSn);

// =============================================================================
// tgt_SetMbSn
// -----------------------------------------------------------------------------
/// This function is used to update the MbSn.
// =============================================================================
PUBLIC INT32 tgt_SetMbSn(UINT8 *mbSn);

// =============================================================================
// tgt_GetMpSn
// -----------------------------------------------------------------------------
/// This function is used to get the MpSn.
// =============================================================================

PUBLIC INT32 tgt_GetMpSn(UINT8 *mpSn);

// =============================================================================
// tgt_SetMpSn
// -----------------------------------------------------------------------------
/// This function is used to update the MpSn.
// =============================================================================
PUBLIC INT32 tgt_SetMpSn(UINT8 *mpSn);


// =============================================================================
// tgt_GetModuleId
// -----------------------------------------------------------------------------
/// This function is used to get the Module ID description.
// =============================================================================
PUBLIC INT32 tgt_GetModuleId(TGT_MODULE_ID_T *moduleId);

// =============================================================================
// tgt_SetModuleId
// -----------------------------------------------------------------------------
/// This function is used to update the Module ID description.
// =============================================================================
PUBLIC INT32 tgt_SetModuleId(TGT_MODULE_ID_T *moduleId);

// =============================================================================
// tgt_GetEncryptedUid
// -----------------------------------------------------------------------------
/// This function is used to get the encrypted UID.
// =============================================================================
PUBLIC INT32 tgt_GetEncryptedUid(UINT32 *uid);

// =============================================================================
// tgt_GetBtInfo
// -----------------------------------------------------------------------------
/// This function is used to get the BT information.
// =============================================================================
PUBLIC INT32 tgt_GetBtInfo(TGT_BT_INFO_T *btInfo);

// =============================================================================
// tgt_SetBtInfo
// -----------------------------------------------------------------------------
/// This function is used to update the BT information.
// =============================================================================
PUBLIC INT32 tgt_SetBtInfo(TGT_BT_INFO_T *btInfo);

// =============================================================================
// tgt_GetSalesTrackInfo
// -----------------------------------------------------------------------------
/// This function is used to get the Sales Track information.
// =============================================================================
PUBLIC INT32 tgt_GetSalesTrackInfo(TGT_SALES_TRACK_INFO_T *trackInfo);

// =============================================================================
// tgt_SetSalesTrackInfo
// -----------------------------------------------------------------------------
/// This function is used to update the Sales Track information.
// =============================================================================
PUBLIC INT32 tgt_SetSalesTrackInfo(TGT_SALES_TRACK_INFO_T *trackInfo);

// =============================================================================
// tgt_GetWifiInfo
// -----------------------------------------------------------------------------
/// This function is used to get the WIFI information.
// =============================================================================
PUBLIC INT32 tgt_GetWifiInfo(TGT_WIFI_INFO_T *wifiInfo);

// =============================================================================
// tgt_SetWifiInfo
// -----------------------------------------------------------------------------
/// This function is used to update the WIFI information.
// =============================================================================
PUBLIC INT32 tgt_SetWifiInfo(TGT_WIFI_INFO_T *wifiInfo);
// =============================================================================
// tgt_GetStationName
// -----------------------------------------------------------------------------
/// This function is used to get the StationName.
// =============================================================================
PUBLIC INT32 tgt_GetStationName(UINT8 stationid,UINT8* stationname);
// =============================================================================
// tgt_SyncGetStationName
// -----------------------------------------------------------------------------
/// This function is used to Sync get the StationName.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationName(UINT8 stationid,UINT8* stationNames);
// =============================================================================
// tgt_SetStationName
// -----------------------------------------------------------------------------
/// This function is used to set the StationName.
// =============================================================================
PUBLIC INT32 tgt_SetStationName(UINT8 stationid,UINT8* stationname);
// =============================================================================
// tgt_GetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_GetStationPerformed(UINT16 *stationperformed);// =============================================================================
// tgt_SyncGetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_GetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationPerformed(UINT16 *stationPerformed);
// =============================================================================
// tgt_SetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_SetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SetStationPerformed(UINT16 *stationperformed);
// =============================================================================
// tgt_SyncGetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to sync set the tgt_GetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SyncSetStationPerformed(UINT16 *stationPerformed);
// =============================================================================
// tgt_SetStationPerformedAndPassedByName
// -----------------------------------------------------------------------------
/// This function is used to set the Station Perform and pass bit By Station Name.
// =============================================================================
PUBLIC BOOL tgt_SetStationPerformedAndPassedByName(UINT8* stationNames, BOOL bProferm, BOOL bPass);
// =============================================================================
// tgt_SyncSetStationPerformedAndPassedByName
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Perform and pass bit By Station Name.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPerformedAndPassedByName(UINT8* stationNames, BOOL bProferm, BOOL bPass);
// =============================================================================
// tgt_GetStationIDByName
// -----------------------------------------------------------------------------
/// This function is used to get the Station Perform bit Index By Station Name.
// =============================================================================
PUBLIC UINT8 tgt_GetStationIDByName(UINT8* stationName) ;
//=============================================================================
// tgt_SyncGetStationIDByName
// -----------------------------------------------------------------------------
/// This function is used to sync get the Station Perform bit Index By Station Name.
// =============================================================================
PUBLIC UINT8 tgt_SyncGetStationIDByName(UINT8* stationName);
// =============================================================================
// tgt_SetStationPerformedByID
// -----------------------------------------------------------------------------
/// This function is used to set the Station Perform bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SetStationPerformedByID(UINT8 stationIdx, BOOL bVal);
// =============================================================================
// tgt_SyncSetStationPerformedByID
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Perform bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPerformedByID(UINT8 stationIdx, BOOL bVal);
// =============================================================================
// tgt_SetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to set the Station Pass bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SetStationPassedByID(UINT8 stationIdx, BOOL bVal);
// =============================================================================
// tgt_SyncSetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Pass bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPassedByID(UINT8 stationIdx, BOOL bVal);
// =============================================================================
// tgt_GetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to get the Station Pass bit By Index.
/// -1: invalid   0: FAIL  1: OK
// =============================================================================
PUBLIC char tgt_GetStationPassedByID(UINT8 stationIdx);

// =============================================================================
// tgt_GetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_GetStationPassed(UINT16 *stationpassed);
// =============================================================================
// tgt_SyncGetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_GetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationPassed(UINT16 *stationPassed);
// =============================================================================
// tgt_SetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_SetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SetStationPassed(UINT16 *stationpassed);
// =============================================================================
// tgt_SyncSetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_SetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SyncSetStationPassed(UINT16 *stationPassed);
// =============================================================================
// tgt_GetStationLastDesc
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationLastDesc.
// =============================================================================
PUBLIC INT32 tgt_GetStationLastDesc(UINT8 *StationLastDesc);
// =============================================================================
// tgt_SetStationLastDesc
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_SetStationLastDesc.
// =============================================================================
PUBLIC INT32 tgt_SetStationLastDesc(UINT8 *StationLastDesc);
// =============================================================================
// tgt_FlushFactorySettings
// -----------------------------------------------------------------------------
/// Wait previous factory setting write are done.
// =============================================================================
PUBLIC VOID tgt_FlushFactorySettings(VOID);



#ifdef AT_USER_DBS
PUBLIC INT32 CFW_GetDBSSeed(INT8 *seed);
PUBLIC INT32 TGT_SetDBSSeed(INT8 *seed);

#endif

// =============================================================================
// tgt_GetKeyCode
// -----------------------------------------------------------------------------
/// This function returns the code of a key from its column number. It
/// implements thus the key mapping, between the key detected and the
/// keyboard of a given target.
/// @param key Key number as given by the HAL Key driver
/// @return The key code as mapped for a specifc target.
// =============================================================================
PUBLIC UINT8 tgt_GetKeyCode(UINT8 key);

// =============================================================================
// tgt_GetKeyMapIndex
// -----------------------------------------------------------------------------
/// This function returns the index of a key in the key map.
/// @param keyCode The key code as mapped for a specifc target.
/// @return The index in the key map.
// =============================================================================
PUBLIC UINT8 tgt_GetKeyMapIndex(UINT8 keyCode);

// =============================================================================
// tgt_GetBootDownloadKeyList
// -----------------------------------------------------------------------------
/// This function returns the list of boot download keys.
/// @param ppList Point to the pointer of the key list.
/// @param pLen Point to the list length.
// =============================================================================
PUBLIC VOID tgt_GetBootDownloadKeyList(CONST UINT8 **ppList, UINT32 *pLen);

// =============================================================================
// tgt_GetBootUsbDownloadKeyList
// -----------------------------------------------------------------------------
/// This function returns the list of usb boot download keys.
/// @param ppList Point to the pointer of the key list.
/// @param pLen Point to the list length.
// =============================================================================
PUBLIC VOID tgt_GetBootUsbDownloadKeyList(CONST UINT8 **ppList, UINT32 *pLen);

// =============================================================================
// tgt_GetBootBbatKeyList
// -----------------------------------------------------------------------------
/// This function returns the list of quick entry bbat mode keys.
/// @param ppList Point to the pointer of the key list.
/// @param pLen Point to the list length.
// =============================================================================
PUBLIC VOID tgt_GetBootBbatKeyList(CONST UINT8 **ppList, UINT32 *pLen);
// =============================================================================
// tgt_GetBuildVerNo
// -----------------------------------------------------------------------------
/// This function is used to access the version number of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildVerNo(VOID);

// =============================================================================
// tgt_GetBuildDateTime
// -----------------------------------------------------------------------------
/// This function is used to access the date time of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildDateTime(VOID);

// =============================================================================
// tgt_GetBuildRelease
// -----------------------------------------------------------------------------
/// This function is used to access the CT_RELEASE of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildRelease(VOID);

// =============================================================================
// tgt_GetHwModule
// -----------------------------------------------------------------------------
/// This function is used to access the hardware module names on this target.
// =============================================================================
PUBLIC CONST TGT_HW_MODULE_T* tgt_GetHwModule(VOID);

// =============================================================================
// tgt_GetMobileBoardSn
// -----------------------------------------------------------------------------
/// This function is used to get the mobile board SN.
// =============================================================================
PUBLIC VOID tgt_GetMobileBoardSn(UINT8 *pSn, UINT8 *pLen);

// =============================================================================
// tgt_GetMobilePhoneSn
// -----------------------------------------------------------------------------
/// This function is used to get the mobile phone SN.
// =============================================================================
PUBLIC VOID tgt_GetMobilePhoneSn(UINT8 *pSn, UINT8 *pLen);

// =============================================================================
// tgt_RegisterAllModules
// -----------------------------------------------------------------------------
/// This function call the register functions for each modules of platform.
// =============================================================================
PUBLIC VOID tgt_RegisterAllModules(VOID);

// =============================================================================
// tgt_RegisterMonitorModules
// -----------------------------------------------------------------------------
/// This function call the register functions for modules for boot monitor.
// =============================================================================
PUBLIC VOID tgt_RegisterMonitorModules(VOID);

// =============================================================================
// tgt_GetUsbDownloadByMicDet
// -----------------------------------------------------------------------------
/// This function is used to get the status of Usb detect by Mic Detect Key
// =============================================================================
PUBLIC BOOL tgt_GetUsbDownloadByMicDet(UINT32 *lo, UINT32 *hi);

// =============================================================================
// TGT_FACTORY_SETTINGS_T
// -----------------------------------------------------------------------------
/// Define the factory settings structure for this target
// =============================================================================
typedef struct
{
    UINT32 magic;
    UINT32 len;
    TGT_PMD_VOLT_CAP_T curve[48];
} TGT_BATT_CURVE_T;

// =============================================================================
// TGT_DBS_INFO_T
// -----------------------------------------------------------------------------
/// Define the dbs seed structure for this target
// =============================================================================
#ifdef AT_USER_DBS
typedef struct
{
    INT8   dbsSeed[63];
} TGT_DBS_INFO_T;
#endif

// =============================================================================
// TGT_FACTORY_SETTINGS_T
// -----------------------------------------------------------------------------
/// Define the factory settings structure for this target
// =============================================================================
typedef struct
{
    // module id
    TGT_MODULE_ID_T moduleId;
    // encrypted checksum
    UINT32          uid;
    // battery curve
    TGT_BATT_CURVE_T battCurve;

#ifdef SALES_TRACK_APP
    TGT_SALES_TRACK_INFO_T trackInfo;
#endif

#ifdef AT_USER_DBS
    TGT_DBS_INFO_T dbs_info;
#endif
} TGT_FACTORY_CFG_T;

STATIC_ASSERT((sizeof(TGT_FACTORY_CFG_T) <= FACT_SUPPLEMENTARY_LEN),
              "Error: Factory structure too large.");


#endif // _TGT_H_
