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




#ifndef BLUETOOTH_MANAGER_API_DECLARED
#define BLUETOOTH_MANAGER_API_DECLARED

#define CONN_NONE              0
#define CONN_ACTIVE             1
#define CONN_CLOSED             2
#define CONN_PENDING            3
#define CONN_RELEASEPENDING     4
#define CONN_PENDINGHOLDALLACLS 5
#define CONN_REQPENDING    6

#define MGR_NONDISCOVERABLE     0x00
#define MGR_GENERALDISCOVERY    0x01
#define MGR_LIMITEDDISCOVERY    0x03
#define MGR_NONCONNECTABLE      0x00
#define MGR_CONNECTABLE         0x04
#define MGR_DISCOVERABLE_BIT        0x1
#define MGR_LIMDISCOVERABLE_BIT     0x2
#define MGR_CONNECTABLE_BIT         0x4

typedef struct st_t_MGR_HCVersion
{
    u_int8 HCIversion;
    u_int16 HCIrevision;
    u_int8 LMPversion;
    u_int16 LMPsubversion;
    u_int16 ManufacturerId;
} t_MGR_HCVersion;


#define NUM_BYTES_LMP_FEATURES 8
#define NUM_BYTES_HOST_VERSION 12

/* Discoverability Modes */
enum _rdabt_discoverable_mode
{
    RDABT_NONDISCOVERABLE = 0,
    RDABT_GENERALDISCOVERY,
    RDABT_LIMITEDDISCOVERY
};

/* Connectability Mode */
enum _rdabt_connectablity_mode
{
    RDABT_NONCONNECTABLE = 0,
    RDABT_CONNECTABLE = 1
};

/* PAIRING MODES */
enum _rdabt_bondable_mode
{
    MGR_NONPAIRABLE  = 0,
    MGR_PAIRABLE      =1
};

enum _rdabt_io_capability
{
    MGR_IOCAP_DISPLAY_ONLY         = 0,
    MGR_IOCAP_DISPLAY_YESNO        = 1,
    MGR_IOCAP_KEYBOARD_ONLY        = 2,
    MGR_IOCAP_NOINPUT_NOOUTPUT     = 3
};

/* SECURITY MODES */
enum _rdabt_security_mode
{
    MGR_NOSECURITY   =1,
    MGR_SERVICELEVEL  = 2,
    MGR_LINKLEVEL     = 3,
    MGR_SERVICELEVEL_ENFORCED  =4
};

/* Security Level Define */
#define MGR_ACCESS_REGECT         0xff
#define MGR_MITM_ENCRYPTION         3
#define MGR_NOMITM_ENCRYPTION       2
#define MGR_NOMITM_MIN_USERIF       1
#define MGR_NOUSER_INTERACTION      0       // only for sdp


enum _rdabt_global_state
{
    RDABT_IDLE                              = 0x0000,
    RDABT_INQUIRYING                  = 0x0001,
    RDABT_BONDING                       = 0x0004,
    RDABT_HF_AUDIO_OPEN         = 0x0008,
    RDABT_A2DP_PLAY                 = 0x0010,
    RDABT_OBEX_CONN                 = 0x0020,
    RDABT_OBEX_SEND                 = 0x0040,
    RDABT_OBEX_RECEIVE            = 0x0080,
    RDABT_A2DP_CONN                 = 0x0100,

};

enum _mgr_device_list_flag
{
    MGR_DEVICE_FLAG_NAME_RECEIVED = 1,
    MGR_DEVICE_FLAG_LINKKEY_VALID    = 2,
    MGR_DEVICE_FLAG_SERVICE_RECEIVED = 4,
    MGR_DEVICE_FLAG_CONNECTED = 8,
    MGR_DEVICE_FLAG_OOB_RECEIVED = 0x10,
    MGR_DEVICE_FLAG_ALL        = 0xff
};

/* Link Key Types */
#define MGR_COMBINATIONKEY 0
#define MGR_LOCALUNITKEY   1
#define MGR_REMOTEUNITKEY  2
#define MGR_DEBUG_COMBINATION_KEY               3
#define MGR_UNAUTHENTICATED_COMBINATION_KEY     4
#define MGR_AUTHENTICATED_COMBINATION_KEY       5
#define MGR_CHANGED_COMBINATION_KEY             6

typedef struct st_t_BT_DeviceEntry
{
    /* WARNING - this structure MUST match the layout of the first part of the st_t_BT_DeviceInfo structure above */
    t_bdaddr bdAddress;
    u_int16 deviceState; /* combined with device record flags */
    u_int32 classOfDevice;
    s_int8 rssi;
    u_int8 name_length;
    u_int8 name[MGR_MAX_REMOTE_NAME_LEN];
    u_int8 linkKeyType; /* MGR_COMBINATION | MGR_LOCALUNITKEY | MGR_REMOTEUNITKEY */
    u_int8 linkKey[LINKKEY_SIZE];
} t_MGR_DeviceEntry;

typedef struct st_t_BT_ConnectionEntry
{

    u_int8 role; /* 1 = master, 0 = slave */

    u_int8 aclState; /* CONN_OPEN | CONN_CLOSED | CONN_PENDING */
    u_int16 aclHandle;

    u_int8 scoState; /* CONN_OPEN | CONN_CLOSED | CONN_PENDING */
    u_int16 scoHandle;

    u_int8 deviceMode; /* DEV_SNIFF | DEV_HOLD | DEV_PARK | DEV_ACTIVE */
    u_int8 encryptionMode; /* CRPYT_DISABLED | CRYPT_P2PONLY | CRYPT_ALL */
} t_MGR_ConnectionEntry;

typedef struct st_mgr_bond_cnf_msg
{
    u_int8 result;
    void *device;
} mgr_bond_cnf_msg_t;


typedef struct st_mgr_ssp_ind_msg
{
    u_int8 isDisplay;
    u_int32 value;
    void *device;
} mgr_ssp_ind_msg_t;

typedef struct
{
    t_bdaddr address;
    u_int16 sco_handle;
    u_int8 result;
    u_int8 rx_length;
} mgr_sco_link_t;

typedef struct
{
    t_bdaddr address;
} mgr_acl_link_ind_t;

typedef struct
{
    t_bdaddr address;
    u_int16 reason;
    u_int8 aclState;
    u_int8 linkTyp;
    u_int8 result;
} mgr_acl_disconn_ind_t;

typedef struct
{
    s_int32 offset;
    u_int8 result;
} mgr_calib_clock_ind_t;

typedef struct
{
    u_int8 mode;
    u_int16 aclhandle;
} mgr_mode_change_ind_t;

/*
    Informational functions
*/
#ifdef __cplusplus
extern "C" {
#endif

/* get local version and feature */
APIDECL1 t_api APIDECL2 MGR_GetHostVersion(char *buffer);
APIDECL1 t_api APIDECL2 MGR_GetLocalFeatures(u_int8 *outBuffer);
APIDECL1 t_api APIDECL2 MGR_GetDeviceVersion(t_MGR_HCVersion *ver);
/* local bdaddr */
APIDECL1 t_api APIDECL2 MGR_GetLocalBDAddress(t_bdaddr *address);
/* Discoverability Setting */
APIDECL1 t_api APIDECL2 MGR_SetDiscoverableMode(u_int8 mode);
APIDECL1 t_api APIDECL2 MGR_GetDiscoverableMode(u_int8 *mode);
/* Connectability Mode */
APIDECL1 t_api APIDECL2 MGR_GetConnectableMode(u_int8 *mode);
APIDECL1 t_api APIDECL2 MGR_SetConnectableMode(u_int8 mode);
/* Bondable mode */
APIDECL1 t_api APIDECL2 MGR_SetPairingMode(u_int8 mode);
APIDECL1 t_api APIDECL2 MGR_GetPairingMode(u_int8 *mode);
/* Local Device Security Settings */
APIDECL1 t_api APIDECL2 MGR_SetSecurityMode(u_int8 mode);
APIDECL1 t_api APIDECL2 MGR_GetSecurityMode(u_int8 *mode);
APIDECL1 t_api APIDECL2 MGR_GetLocalOOBData(u_int8 *oob_c, u_int8 *oob_r);
APIDECL1 t_api APIDECL2 MGR_SetSecurityLevel(u_int8 level);
/* local name */
APIDECL1 t_api APIDECL2 MGR_ChangeLocalName(u_int8 *name);
APIDECL1 t_api APIDECL2 MGR_GetLocalName(char *outBuffer, u_int8 bufLen, u_int8 *nameLen);
/* global state */
APIDECL1 t_api APIDECL2 MGR_GetState(u_int32 *state);


/* Device Database Access */
APIDECL1 t_api APIDECL2 MGR_AddDeviceRecord(t_bdaddr address,  u_int8 *linkKey,  u_int8 keyType);
APIDECL1 t_api APIDECL2 MGR_FindDeviceRecord(t_bdaddr address, t_MGR_DeviceEntry **entry);
APIDECL1 t_api APIDECL2 MGR_GetNextDeviceRecord(t_MGR_DeviceEntry **entry, u_int32 flags, u_int32 withoutflags);
APIDECL1 t_api APIDECL2 MGR_RemoveDeviceRecord(t_MGR_DeviceEntry *entry);
APIDECL1 t_api APIDECL2 MGR_GetConnectionRecord(t_bdaddr address, t_MGR_ConnectionEntry **entry);
APIDECL1 t_api APIDECL2 MGR_Set_Sniff_Interval(u_int16 handle, u_int16 min, u_int16 max, u_int16 adjust_interval, u_int8 active_sniff);

/* Inquiry */
APIDECL1 t_api APIDECL2 MGR_Inquiry(u_int8 inquiryLength, u_int32 accessCode, u_int32 cod, u_int8 maxResults);
APIDECL1 t_api APIDECL2 MGR_InquiryCancel(void);
APIDECL1 t_api APIDECL2 MGR_GetDeviceNumFound(void);
APIDECL1 t_api APIDECL2 MGR_GetRemoteName(t_bdaddr address);

/* Bonding */
APIDECL1 t_api APIDECL2 MGR_InitBonding(t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_LinkKeyResponse(t_bdaddr address, u_int8 keySupplied, u_int8 *linkkey, u_int8 keytype);
APIDECL1 t_api APIDECL2 MGR_PINResponse(t_bdaddr address, u_int8 pinSupplied, u_int8 *pinCode, u_int8 pinLen);
APIDECL1 t_api APIDECL2 MGR_OOBDataResponse(t_bdaddr address, u_int8 oobSupplied, u_int8 *OOB_R, u_int8 *OOB_C);
APIDECL1 t_api APIDECL2 MGR_PasskeyResponse(t_bdaddr address, u_int8 passkeySupplied, u_int32 passkey);
APIDECL1 t_api APIDECL2 MGR_ConfirmResponse(t_bdaddr address, u_int8 accept);

/* Sync Link Control functions */
APIDECL1 t_api APIDECL2 MGR_SetupSynchronous(t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_ReleaseSynchronous(u_int16 scoHandle);
APIDECL1 t_api APIDECL2 MGR_ConnectAcceptRsp(u_int8 result, t_bdaddr addr);


/* TCI extensions functions*/
APIDECL1 t_api APIDECL2 MGR_WriteLocalDeviceAddress(t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_EnableDUT(u_int8 transport);
APIDECL1 t_api APIDECL2 MGR_ReadPHYRegister(u_int8 flag, u_int32 address);
APIDECL1 t_api APIDECL2 MGR_WritePHYRegister(u_int8 flag, u_int32 address, u_int8 count, u_int32 *value);
APIDECL1 t_api APIDECL2 MGR_BaseBand_TXTest( u_int8 hop_frq, u_int8 msg_sco, u_int8 msg_burst,
        u_int8 msg_type, u_int8 power_level,u_int16 PRbs_init, u_int16 msg_data,
        u_int8 tx_packet_type, u_int8 EDR_mode, u_int32 tx_len,
        u_int8 am_addr, u_int32 syncword_low, u_int32 syncword_hight, u_int8 hop_enable);
APIDECL1 t_api APIDECL2 MGR_BaseBand_TestEnd(void);
APIDECL1 t_api APIDECL2 MGR_CalibClock(t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_Write_DHKey(u_int8 is_localkey, u_int8 *key_x, u_int8 *key_y);


#define RDABT_MGR_GetHostVersion(buffer)                        MGR_GetHostVersion(buffer)
#define RDABT_MGR_GetLocalFeatures(outBuffer)                                     MGR_GetLocalFeatures(outBuffer)
#define RDABT_MGR_GetDeviceVersion(ver)                                           MGR_GetDeviceVersion(ver)
#define RDABT_MGR_GetLocalBDAddress(address)                                      MGR_GetLocalBDAddress(address)
#define RDABT_MGR_GetDiscoverableMode(mode)                                       MGR_GetDiscoverableMode(mode)
#define RDABT_MGR_GetConnectableMode(mode)                                        MGR_GetConnectableMode(mode)
#define RDABT_MGR_SetPairingMode(mode)                                            MGR_SetPairingMode(mode)
#define RDABT_MGR_GetPairingMode(mode)                                            MGR_GetPairingMode(mode)
#define RDABT_MGR_GetSecurityMode(mode)                                           MGR_GetSecurityMode(mode)
#define RDABT_MGR_GetLocalOOBData(mode)                                           MGR_GetLocalOOBData(mode)
#define RDABT_MGR_GetLocalName(outBuffer, bufLen, nameLen)                        MGR_GetLocalName(outBuffer, bufLen, nameLen)
#define RDABT_MGR_GetState(state)                                                 MGR_GetState(state)
#define RDABT_MGR_AddDeviceRecord(address,  linkKey,  keyType)                    MGR_AddDeviceRecord(address,  linkKey,  keyType)
#define RDABT_MGR_FindDeviceRecord(address, entry)                                MGR_FindDeviceRecord(address, entry)
#define RDABT_MGR_GetNextDeviceRecord(entry, flags, withoutflags)                 MGR_GetNextDeviceRecord(entry, flags, withoutflags)
#define RDABT_MGR_RemoveDeviceRecord(entry)                                       MGR_RemoveDeviceRecord(entry)
#define RDABT_MGR_GetConnectionRecord(address, entry)                             MGR_GetConnectionRecord(address, entry)
#define RDABT_MGR_GetDeviceNumFound()                                             MGR_GetDeviceNumFound()
#define RDABT_MGR_WriteLocalDeviceAddress( address)                               MGR_WriteLocalDeviceAddress( address)
#define RDABT_MGR_EnableDUT()                                                     MGR_EnableDUT()
#define RDABT_MGR_ReadPHYRegister( flag,  address)                                MGR_ReadPHYRegister( flag,  address)
#define RDABT_MGR_WritePHYRegister                                                  MGR_WritePHYRegister
#define RDABT_MGR_BaseBand_TXTest                                                 MGR_BaseBand_TXTest
#define RDABT_MGR_BaseBand_TestEnd                                              MGR_BaseBand_TestEnd

/* set mode */
APIDECL1 t_api APIDECL2 RDABT_MGR_SetDiscoverableMode(u_int8 mode);
APIDECL1 t_api APIDECL2 RDABT_MGR_SetConnectableMode(u_int8 mode);
APIDECL1 t_api APIDECL2 RDABT_MGR_SetSecurityMode(u_int8 mode);
APIDECL1 t_api APIDECL2 RDABT_MGR_ChangeLocalName(u_int8 *name) ;

/* Inquiry */
APIDECL1 t_api APIDECL2 RDABT_MGR_Inquiry(u_int8 inquiryLength, u_int32 accessCode, u_int32 cod, u_int8 maxResults);
APIDECL1 t_api APIDECL2 RDABT_MGR_InquiryCancel(void);
APIDECL1 t_api APIDECL2 RDABT_MGR_GetRemoteName(t_bdaddr address);

/* Bonding */
APIDECL1 t_api APIDECL2 RDABT_MGR_InitBonding(t_bdaddr address);
APIDECL1 t_api APIDECL2 RDABT_MGR_LinkKeyResponse(t_bdaddr address, u_int8 keySupplied, u_int8 *linkkey, u_int8 keytype);
APIDECL1 t_api APIDECL2 RDABT_MGR_PINResponse(t_bdaddr address, u_int8 pinSupplied, u_int8 *pinCode, u_int8 pinLen);
APIDECL1 t_api APIDECL2 RDABT_MGR_OOBDataResponse(t_bdaddr address, u_int8 oobSupplied, u_int8 *OOB_R, u_int8 *OOB_C);
APIDECL1 t_api APIDECL2 RDABT_MGR_PasskeyResponse(t_bdaddr address, u_int8 passkeySupplied, u_int32 passkey);
APIDECL1 t_api APIDECL2 RDABT_MGR_ConfirmResponse(t_bdaddr address, u_int8 accept);
APIDECL1 t_api APIDECL2 RDABT_MGR_RemoveDevice(t_bdaddr address, u_int8 removeAll);

/* Sync Link Control functions */
APIDECL1 t_api APIDECL2 RDABT_MGR_SetupSynchronous(t_bdaddr address);
APIDECL1 t_api APIDECL2 RDABT_MGR_ReleaseSynchronous(u_int16 scoHandle);
APIDECL1 t_api APIDECL2 RDABT_MGR_ConnectAcceptRsp(u_int8 result, t_bdaddr addr);




#if RDABT_LE_SUPPORT ==1

typedef struct st_t_BT_LEDeviceEntry
{
    t_bdaddr bdAddress;
    u_int8 addr_type;
    u_int16 deviceState;
    u_int8 name_length;
    u_int8 name[MGR_MAX_REMOTE_NAME_LEN];
    u_int8 ltk[LINKKEY_SIZE];
    u_int16 ediv;
    u_int8 rand[8];
} t_MGR_LEDeviceEntry;

enum _mgr_le_device_list_flag
{
    MGR_LE_DEVICE_FLAG_NAME_RECEIVED = 1,
    MGR_LE_DEVICE_FLAG_LINKKEY_VALID    = 2,
    MGR_LE_DEVICE_FLAG_SERVICE_RECEIVED = 4,
    MGR_LE_DEVICE_FLAG_CONNECTED = 8,
    MGR_LE_DEVICE_FLAG_OOB_RECEIVED = 0x10,
    MGR_LE_DEVICE_FLAG_CSRK_VALID = 0X20,
    MGR_LE_DEVICE_FLAG_IRK_VALID = 0X40,
    MGR_LE_DEVICE_FLAG_ALL        = 0xff
};


enum _rdabt_le_security_mode
{
    SEC_MODE1 = 1,
    SEC_MODE2 = 2,
};

/* LE Security Level Define */
#define AUTH_PAIR_WITH_DATA_SIGNING     4
#define UNAUTH_PAIR_WITH_DATA_SIGNING   3
#define AUTH_PAIR_WITH_ENCRYPTION       2
#define UNAUTH_PAIR_WITH_ENCRYPTION     1
#define NO_SECURITY                     0


enum _rdabt_le_discovery_mode
{
    LE_NONDISCOVERABLE_MODE = 0,
    LE_LIMITEDDISCOVERY_MODE,
    LE_GENERALDISCOVERY_MODE,
};

#ifndef _LELC_TYPE_H_
enum _rdabt_le_adv_type
{
    ADV_IND = 0x00,
    ADV_DIRECT_IND = 0x01,
    ADV_SCAN_IND = 0x02,
    ADV_NONCONN_IND = 0x03,
};
#endif

enum _rdabt_le_bonding_mode
{
    LE_NONBONDABLE_MODE = 0,
    LE_BONDABLE_MODE
};


APIDECL1 t_api APIDECL2 MGR_GetOwnAddrType(u_int8 *type);
APIDECL1 t_api APIDECL2 MGR_SetOwnAddrType(u_int8 type);
APIDECL1 t_api APIDECL2 MGR_LeGetOwnAddress(t_bdaddr* pAddr);
APIDECL1 t_api APIDECL2 MGR_SetRandomAddress(t_bdaddr addr);
APIDECL1 t_api APIDECL2 MGR_LESetDirectAddr(u_int8 addr_type,t_bdaddr addr);
APIDECL1 t_api APIDECL2 MGR_LESetAdvertMode(u_int8 discov_mode,u_int8 conn_mode, u_int8 noBrEdr);
APIDECL1 t_api APIDECL2 MGR_LESetAdvData(u_int8 *adv_data,u_int8 length);
APIDECL1 t_api APIDECL2 MGR_LeScan(u_int8 scanType, u_int16 scanInterval,u_int16 scanWindow,
                                   u_int8 filterPolicy,u_int8 scanTimeout,u_int8 maxResults);
APIDECL1 t_api APIDECL2 MGR_LeScanStop(void);
APIDECL1 t_api APIDECL2 MGR_LECreateACL(t_bdaddr addr, u_int8 addrType);
APIDECL1 t_api APIDECL2 MGR_LEReleaseACL(u_int8 handle);
APIDECL1 t_api APIDECL2 MGR_LEReleaseACLFromAddr(t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_LESetAdvIntval(u_int16 min,u_int16 max);
APIDECL1 t_api APIDECL2 MGR_LEAddWhiteList(u_int8 type,t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_LERemoveWhiteList(u_int8 type,t_bdaddr address);
APIDECL1 t_api APIDECL2 MGR_LESetSecLevel(u_int8 level);
APIDECL1 u_int8 APIDECL2 MGR_LEGetSecLevel(void);
APIDECL1 t_api APIDECL2 MGR_EnableAdv(u_int8 type);
APIDECL1 u_int8 APIDECL2 MGR_LEGetBondMode();
APIDECL1 u_int8 APIDECL2 MGR_LEGetBondMode();
APIDECL1 u_int8 APIDECL2 MGR_LEPrivacySupport(void);
APIDECL1 t_api APIDECL2 MGR_LEStartEncryption(u_int16 handle);

#endif


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BLUETOOTH_MANAGER_API_DECLARED */


