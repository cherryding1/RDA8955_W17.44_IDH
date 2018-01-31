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

#ifndef CFW_USIM_H
#define CFW_USIM_H

#include "cfw_sim.h"

#define COMBINE_TWO_BYTES(high,low)     ((((INT16)high) << 8) | (low))

typedef struct _CFW_UsimEfStatus
{
    UINT8                        efStructure;   // 1:Transparent ; 2: Linear Fixed ; 6: Cyclic
    INT16                      fileId;
    INT16                      fileSize;
    UINT8                       recordLength;
    UINT8                       numberOfRecords;
}
CFW_UsimEfStatus;

// Application Id that resides on the UICC see TS 31.110 for coding
typedef struct CFW_UsimAidTag
{
    UINT8       data[16];           // AID value
    UINT8       length;                 // AID length
} CFW_UsimAid;

typedef struct CFW_UsimAidLabelTag
{
    UINT16          applicationType;
    UINT8           length;                     // label length
    UINT8           data[32];               // Application label
} CFW_UsimAidLabel;

/*
typedef struct CFW_UiccApplicationListTag
{
  INT8                            numUsimApplications;
  CFW_UsimAidLabel        applicationLabel [5];
} CFW_UsimApplicationList;
*/
typedef enum CFW_UsimKeyRefValueTag
{
    CFW_USIM_NO_PIN           = 0x00,
    CFW_USIM_PIN1_APP1     = 0x01,
    CFW_USIM_PIN1_APP2     = 0x02,
    CFW_USIM_PIN1_APP3     = 0x03,
    CFW_USIM_PIN1_APP4     = 0x04,
    CFW_USIM_PIN1_APP5     = 0x05,
    CFW_USIM_PIN1_APP6     = 0x06,
    CFW_USIM_PIN1_APP7     = 0x07,
    CFW_USIM_PIN1_APP8     = 0x08,
    CFW_USIM_UNIVERSAL_PIN = 0x11,
    CFW_USIM_PIN2_APP1     = 0x81,
    CFW_USIM_PIN2_APP2     = 0x82,
    CFW_USIM_PIN2_APP3     = 0x83,
    CFW_USIM_PIN2_APP4     = 0x84,
    CFW_USIM_PIN2_APP5     = 0x85,
    CFW_USIM_PIN2_APP6     = 0x86,
    CFW_USIM_PIN2_APP7     = 0x87,
    CFW_USIM_PIN2_APP8     = 0x88,
    CFW_USIM_ADM1              = 0x0a,
    CFW_USIM_ADM2              = 0x0b,
    CFW_USIM_ADM3              = 0x0c,
    CFW_USIM_ADM4              = 0x0d,
    CFW_USIM_ADM5              = 0x0e,
    CFW_USIM_ADM6              = 0x8a,
    CFW_USIM_ADM7              = 0x8b,
    CFW_USIM_ADM8              = 0x8c,
    CFW_USIM_ADM9              = 0x8d,
    CFW_USIM_ADM10            = 0x8e
}
CFW_UsimKeyRefValue;

typedef struct CFW_SimPinStatusTag
{
    UINT8            numRemainingRetrys;
    BOOL             secretCodeInitialised;
    BOOL             enabled;
    BOOL             verified;                   // Has PIN been verified
    BOOL             used;                       // usage qualifier specifies whether the PIN should be used or not
    CFW_UsimKeyRefValue  alternativeKeyReference;    // alternative key reference
} CFW_UsimPinStatus;

typedef struct CFW_UsimSpecificDataTag
{
    CFW_UsimKeyRefValue    applicationPin;
    CFW_UsimPinStatus         pinStatus;                // status of the application PIN
    CFW_UsimKeyRefValue    localPin;
    CFW_UsimPinStatus         pin2Status;               // status of the PIN2 (local PIN)
    CFW_UsimPinStatus         unblockPin2Status;        // status of the UNBLOCK PIN2 (local PIN)
    //    CFW_UsimState               fileState;
    //    UsimDirFreeSpace_t   dirFreeSpace;
}
CFW_UsimSpecificData;

typedef struct CFW_UsimPinTag
{
    CFW_UsimKeyRefValue   pin1;               // this determines the application PIN key reference to use.
    CFW_UsimPinStatus        pinAppStatus[8];         /* status of the application PINs (valid accross the SIM) */
    CFW_UsimSpecificData   usimSpecificData;
    CFW_UsimPinStatus        UniversalPin;
    BOOL                            UniversalPinSupportedByCard;    // does the card support universal PIN ?
} CFW_UsimPin;


UINT8 CFW_UsimDecodeDataLength (UINT8 *dataLgth,  UINT8 *data);
INT16  CFW_UsimDecFileSize (UINT8  *offset,  UINT8 *respData);
void CFW_UsimDecEfFileDescriptor (UINT8  *offset,  UINT8 *respData, CFW_UsimEfStatus *pSimEfStatus);
VOID CFW_UsimDecodeEFFcp (UINT8 *RespData, CFW_UsimEfStatus *pSimEfStatus);
UINT32 CFW_UsimDecodeEfDirRecord (UINT8 *RespData, CFW_UsimAid *aid, CFW_UsimAidLabel *label);
VOID CFW_GetAdnAlphaIdLength ( UINT8 *data,    INT16  *length );
BOOL CFW_UsimDecodeDFFcp ( UINT8 *RespData, CFW_UsimPin *UsimPin);
VOID CFW_UsimDecPinStatusTemplateDO  (UINT8  *offset,  UINT8 *respData, CFW_UsimPin *UsimPin);
VOID CFW_UsimUsageQualifier (UINT8  *offset,  UINT8 *respData, BOOL *usePin);
VOID CFW_UsimDecKeyReference (UINT8  *offset,  UINT8 *respData, BOOL usePin, BOOL enabled, CFW_UsimPin *UsimPin);
void CFW_UsimSetPinStatus (UINT8 keyReference, BOOL  usePin,  BOOL  enabled, CFW_UsimPin *UsimPin);
UINT32 CFW_CfgUSimGetPinParam(CFW_UsimPin **pUSimPinParam, CFW_SIM_ID nSimID);
UINT32 CFW_USimInitPinStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, CFW_UsimPin *UsimPin, CFW_SIM_ID nSimID);
UINT32 CFW_USimParsePinStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, UINT8 PinFlag, CFW_SIM_ID nSimID);
UINT32 CFW_USimParsePukStatus(api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf, SIM_CHV_PARAM *pG_Chv_Param, UINT8 PukFlag, CFW_SIM_ID nSimID);

#endif //CFW_SIM_UICC_H
