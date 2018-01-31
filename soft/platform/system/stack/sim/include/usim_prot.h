//------------------------------------------------------------------------------
//  $Log: usim_prot.h,v $
//  Revision 1.0  2012/06/18 20:51:18  lijy
//  Update of the copyright
//
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of RDA. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright RDA (C) 2004-2013

================================================================================
*/

/*
================================================================================
  File       : usim.h
--------------------------------------------------------------------------------

  Scope      : Include of the usim software unit

  History    :
--------------------------------------------------------------------------------
  Jun 18 12  |  Lijy   | Creation
================================================================================
*/
#ifndef __USIM_PROT_H__
#define __USIM_PROT_H__

#include "sxs_type.h"


/*******************************************************************************
 *  Macros
 ******************************************************************************/
#define SIM_CATEGORY_INDICATOR_80       0x80


#define BIT_1_IS_SET( bYTE )    (BOOL) ((bYTE & 0x01)== 0x01)
#define BIT_2_IS_SET( bYTE )    (BOOL) ((bYTE & 0x02)== 0x02)
#define BIT_3_IS_SET( bYTE )    (BOOL) ((bYTE & 0x04)== 0x04)
#define BIT_4_IS_SET( bYTE )    (BOOL) ((bYTE & 0x08)== 0x08)
#define BIT_5_IS_SET( bYTE )    (BOOL) ((bYTE & 0x10)== 0x10)
#define BIT_6_IS_SET( bYTE )    (BOOL) ((bYTE & 0x20)== 0x20)
#define BIT_7_IS_SET( bYTE )    (BOOL) ((bYTE & 0x40)== 0x40)
#define BIT_8_IS_SET( bYTE )    (BOOL) ((bYTE & 0x80)== 0x80)

#define GET_HIGH_BYTE(val16)	((val16 & 0xff00) >> 8)
#define GET_LOW_BYTE(val16)  	(val16 & 0x00ff)

#define RECORD_NUM_VALID( rECORDNUM )    (BOOL) ((rECORDNUM != 0) && (rECORDNUM != 0xff))
#define TLV_OBJECT_OFFSET_ADVANCE( oFFSETpARAM, oBJdATAlENGTH ) \
                        oFFSETpARAM = oFFSETpARAM + 2 + oBJdATAlENGTH;


#define COMBINE_TWO_BYTES(high,low)     ((((INT16)high) << 8) | (low))
#define USIM_AID_MAX_SIZE                16
// Used as an offset into the LAI starting with octet 1 
#define USIM_MNC_OFFSET  1

#define LEVEL_2_KEY_REF   0x80
#define LEVEL_1_KEY_REF   0x00
// Data to encode/decode Cipher and Integrity Keys
#define CK_OFFSET                       1
#define IK_OFFSET                       17
#define EF_KEYS_FILE_SIZE               33
#define EF_KC_FILE_SIZE                 9
#define EF_KC_GPRS_FILE_SIZE            EF_KC_FILE_SIZE
#define EF_START_HFN_FILE_SIZE          6
#define EF_THRESHOLD_FILE_SIZE          3


// Tags used forthe FCP template
#define USIM_FCP_TAG                    0x62
#define USIM_FILE_DESCRIPTOR_TAG        0x82
#define USIM_FILE_ID_TAG                0x83
#define USIM_DF_NAME_TAG                0x84
#define USIM_PROPRIETARY_INFO_TAG       0xa5
#define USIM_CHARACTERISTICS_TAG        0x80
#define USIM_APP_POWER_CONSUMPTION_TAG  0x81
#define USIM_MIN_APP_CLOCK_FREQ_TAG     0x82
#define USIM_MEM_AVAIL_TAG              0x83
#define USIM_LIFE_CYCLE_STATUS_TAG      0x8a

#define USIM_COMPACT_TAG                0x8c
#define USIM_EXPANDED_TAG               0xab
#define USIM_REF_TO_EXPANDED_TAG        0x8b
#define USIM_PIN_STATUS_TEMPLATE_DO     0xc6

#define USIM_AM_DO_TAG                  0x80
#define USIM_OR_TAG                     0xa0
#define USIM_AND_TAG                    0xaf
#define USIM_SC_DO_TAG                  0xa4
#define USIM_SC_DO_ALWAYS_TAG           0x90


#define USIM_FILE_SIZE_TAG              0x80
#define USIM_TOTAL_FILE_SIZE_TAG        0x81
#define USIM_SFI_TAG                    0x88

#define USIM_PS_DO_TAG                  0x90
#define USIM_USAGE_QUAL_TAG             0x95
#define USIM_KEY_REFERENCE_TAG          0x83

#define USIM_APPLICATION_TEMPLATE_TAG   0x61
#define USIM_APPLICATION_IDENTIFIER_TAG 0x4f
#define USIM_APPLICATION_LABEL_TAG      0x50

//length of the TLV objects 
#define USIM_APP_POWER_CONSUMPTION_TLV_LENGTH   3
#define USIM_TOTAL_FILE_SIZE_TLV_LENGTH         2
#define USIM_SFI_TLV_LENGTH                     1
#define USIM_FILE_SIZE_TLV_LENGTH               2
#define USIM_FILE_ID_TLV_LENGTH                 2
#define USIM_MIN_APP_CLOCK_FREQ_TLV_LENGTH      1
#define USIM_CHARACTERISTICS_TLV_LENGTH         1
#define USIM_LIFE_CYCLE_STATUS_TLV_LENGTH       1
#define USIM_KEY_REFERENCE_TLV_LENGTH           1
#define USIM_USAGE_QUAL_TLV_LENGTH              1
#define USIM_REF_TO_EXPANDED_TLV_LENGTH         3
#define USIM_AM_DO_TLV_LENGTH                   1
#define USIM_CAMPING_FREQ_TLV_LENGTH            2
#define USIM_NEIGHBOUR_FREQ_TLV_LENGTH          2

//          file structures                
#define EF_STRUCT_T              1
#define EF_STRUCT_LF             2
#define EF_STRUCT_C              6


#define DELETE_FILE_MASK        0x40
#define TERMINATE_MASK          0x20
#define ACTIVATE_MASK           0x10
#define DEACTIVATE_MASK         0x08
#define WRITE_MASK              0x04
#define UPDATE_MASK             0x02
#define READ_AND_SEARCH_MASK    0x01
#define DELETE_FILE             0x40
#define TERMINATE               0x20
#define ACTIVATE                0x10
#define DEACTIVATE              0x08
#define WRITE                   0x04
#define UPDATE                  0x02
#define READ_AND_SEARCH         0x01

#define APN_LABEL_SEPARATOR     0x2e


// Max data that we want to get back from the SIM 
#define MAX_RESPONSE_DATA_LENGTH       255

#define TAG_ID_OFFSET  0
#define LENGTH_OFFSET  1
#define VALUE_START_OFFSET 2

#define SIM_PATH_MIN_LENGTH  2

#define SIM_CCP_RECORD_SIZE 14

#define APP_CODE_OFFSET 5
#define RID_SIZE        5
#define APP_CODE_SIZE   2

 // indicates the type of application in EF_DIR
#define  UICC_APP_CODE  0x1001
#define  USIM_APP_CODE  0x1002
#define  USAT_APP_CODE  0x1003
#define  FILE_DESCRIPTOR_MAX_OBJ_LENGTH 5
#define  FILE_DESCRIPTOR_MIN_OBJ_LENGTH 2
#define  NO_PREFERRED_LEVEL         0x01
#define  HIGH_LEVEL_ONLY            0x04
#define  HIGH_LEVEL_PREFERRED       0x05
#define  LOW_LEVEL_ONLY             0x08
#define  LOW_LEVEL_PREFERRED        0x09
#define  NEVER_ALLOWED              0x00
#define  SIM_5_VOLTS_BIT_POSITION   4
#define  SIM_3_VOLTS_BIT_POSITION   5
#define  SIM_1_8_VOLTS_BIT_POSITION 6
#define  SIM_CLOCK_STOP_MODE_MASK   0x0d


#define CLASS_A_MASK 0x01
#define CLASS_B_MASK 0x02
#define CLASS_C_MASK 0x04

#define USAGE_QUALIFIER_VALUE  0x08



#define     SIM_CHV_LENGTH                  8

#define     SIM_MAX_STORED_FDD_FREQUENCIES  4
#define     MAX_NUM_PIN_APP                 	8
#define     SIM_PIN_LENGTH                  		SIM_CHV_LENGTH
#define     MAX_PIN_ATTEMPTS                	3
#define     MAX_UNBLOCK_PIN_ATTEMPTS    10
#define     MAX_NUM_KEY_REF                 	2

#define ACTIVATION_MASK  0xfd


#define  MAX_NUM_KEY_REFERENCE         	 	5
#define  MAX_DIR_FREE_SPACE             			4
#define  MAX_NUM_VOLTAGE_CLASSES        		3
#define  MAX_NUM_PROTOCOLS              		2
#define  MAX_PBR_RECORDS                			2
#define  MAX_FILES_PER_PBR_RECORD       		15
#define  USIM_MAX_NUM_EMAIL_FILES    		4
#define  USIM_MAX_NUM_ANR_FILES      		4
#define  USIM_PBID_FIXED_PART_LENGTH 		10
#define  USIM_PSC_LENGTH              			4
#define USIM_MAX_NUM_RECORDS             		40
#define USIM_LABEL_MAX_SIZE              		32
#define SIM_MAX_LENGTH_ECC_ALPHA_ID      	30    // max length of the alpha Id of the emergency code 
#define MAX_SIM_APPLICATIONS              		5     // max sim applications we can fetch at a time


#define USIM_TYPE_1_TAG                 0xa8
#define USIM_TYPE_2_TAG                 0xa9
#define USIM_TYPE_3_TAG                 0xaa
#define USIM_ADN_TAG                    0xc0
#define USIM_IAP_TAG                    0xc1
#define USIM_EXT1_TAG                   0xc2
#define USIM_SNE_TAG                    0xc3
#define USIM_ANR_TAG                    0xc4
#define USIM_PBC_TAG                    0xc5
#define USIM_GRP_TAG                    0xc6
#define USIM_AAS_TAG                    0xc7
#define USIM_GAS_TAG                    0xc8
#define USIM_UID_TAG                    0xc9
#define USIM_EMAIL_TAG                  0xca
#define USIM_CCP1_TAG                   0xcb
#define USIM_APN_TAG                    0xdd

typedef enum UsimKeyRefValueTag
{
    USIM_ACCESS_NO_PIN        = 0x00,
    USIM_ACCESS_PIN1_APP1     = 0x01,
    USIM_ACCESS_PIN1_APP2     = 0x02,
    USIM_ACCESS_PIN1_APP3     = 0x03,
    USIM_ACCESS_PIN1_APP4     = 0x04,
    USIM_ACCESS_PIN1_APP5     = 0x05,
    USIM_ACCESS_PIN1_APP6     = 0x06,
    USIM_ACCESS_PIN1_APP7     = 0x07,
    USIM_ACCESS_PIN1_APP8     = 0x08,
    USIM_ACCESS_UNIVERSAL_PIN = 0x11,
    USIM_ACCESS_PIN2_APP1     = 0x81,
    USIM_ACCESS_PIN2_APP2     = 0x82,
    USIM_ACCESS_PIN2_APP3     = 0x83,
    USIM_ACCESS_PIN2_APP4     = 0x84,
    USIM_ACCESS_PIN2_APP5     = 0x85,
    USIM_ACCESS_PIN2_APP6     = 0x86,
    USIM_ACCESS_PIN2_APP7     = 0x87,
    USIM_ACCESS_PIN2_APP8     = 0x88,
    USIM_ACCESS_ADM1          = 0x0a,
    USIM_ACCESS_ADM2          = 0x0b,
    USIM_ACCESS_ADM3          = 0x0c,
    USIM_ACCESS_ADM4          = 0x0d,
    USIM_ACCESS_ADM5          = 0x0e,
    USIM_ACCESS_ADM6          = 0x8a,
    USIM_ACCESS_ADM7          = 0x8b,
    USIM_ACCESS_ADM8          = 0x8c,
    USIM_ACCESS_ADM9          = 0x8d,
    USIM_ACCESS_ADM10         = 0x8e
} 
UsimKeyRefValue_t;
typedef struct
{
//	BOOL                     voltageIndicationPresent; /*flag indicates whether voltage indication is present in the ATR. If so, it will be stored in simData->simVoltageCapab*/
//	BOOL                     uiccTransmissionProtocol[MAX_NUM_PROTOCOLS];  /*Transmission protocols supported by the SIM/UICC*/
//	BOOL                     globalInterfaceBytes;
	BOOL                     ApplicationSelectByFullDf;                		/*Application select by full DF allowed ? */
	BOOL                     ApplicationSelectByPartialDf;             	/*Application select by partial DF allowed ? */

	BOOL                     EfDirPresent;                  		/*EF DIR present on the SIM/UICC ?*/
	BOOL                     EfAtrPresent;                  		/*EF ATR present on the SIM/UICC ?*/
	BOOL                     DfSelectByFullDf;                		/*select by full DF allowed ? */
	BOOL                     DfSelectByPartialDf;             	/*select by partial DF allowed ? */
	BOOL                     DfSelectByPath;                  		/*select by PATH allowed ? */
	BOOL                     DfSelectByFileID;                  		/*select by FILE ID allowed ? */
	BOOL                     SfiSupport;                   		/*select by SFI allowed ? */
	BOOL                     RecordNumberSupport;          /*Record Number supported */
	BOOL                     RecordIDSupport;                   /*Record ID supported */
//	BOOL                     DfSelectBySfi;                   		/*select by SFI allowed ? */
	BOOL                     LogicalChannelSupported;       /*more than one logical channel supported ?*/
	UINT8                    TA1;                          			 /*TA1 in the ATR*/
//	BOOL                     TA1Present;                    		/*TA1 present in the ATR ?*/
//   UsimModeOfOperation      modeOfOperation;         /* Mode of operation: specific or negociable*/
	BOOL                     CategoryInd;
}SimATRPara_t;


typedef enum UsimDecodeStatusTag
{
    USIM_DEC_COMM_OK,                  /* command data OK */
    USIM_DEC_COM_DATA_NOT_UNDERSTOOD,  /* unknown tag, minimum data OK, no CR */
    USIM_DEC_COM_TYPE_NOT_UNDERSTOOD,  /* unknown Command Type in Comm Details data */
    USIM_DEC_REQ_VALUES_MISSING,       /* minimum set of data elements not in command */
    USIM_DEC_UNEXPECTED_TAG,           /* Unexpected/Unknown tag */
    USIM_DEC_BAD_LENGTH,               /* wrong length */
    USIM_DEC_SEC_ATTRIBUTES_MISSING,
    USIM_DEC_PHONEBOOK_ERROR,           /*phone book error*/
    USIM_DEC_PIN_TEMPLATE_ERROR         /*error when decoding the PIN template*/
}
UsimDecodeStatus_t;

typedef enum UsimStateTag
{
    USIM_NO_INFO_STATE     = 0x00,
    USIM_CREATION_STATE    = 0x01,
    USIM_INIT_STATE        = 0x03,
    USIM_DEACTIVATED_STATE = 0x04,
    USIM_ACTIVATED_STATE   = 0x05,
    USIM_TERMINATED_STATE  = 0x0c
} 
UsimState_t;



typedef enum SimAccessConditionTag
{
    SIM_ACCESS_ALWAYS,
    SIM_ACCESS_CHV1,
    SIM_ACCESS_CHV2,
    SIM_ACCESS_RFU,
    SIM_ACCESS_ADMIN_4,
    SIM_ACCESS_ADMIN_5,
    SIM_ACCESS_ADMIN_6,
    SIM_ACCESS_ADMIN_7,
    SIM_ACCESS_ADMIN_8,
    SIM_ACCESS_ADMIN_9,
    SIM_ACCESS_ADMIN_A,
    SIM_ACCESS_ADMIN_B,
    SIM_ACCESS_ADMIN_C,
    SIM_ACCESS_ADMIN_D,
    SIM_ACCESS_ADMIN_E,
    SIM_ACCESS_NEVER
}
SimAccessCondition_t;


typedef struct SimAccessDataTag
{
    SimAccessCondition_t    update;
    SimAccessCondition_t    readSeek;
    SimAccessCondition_t    increase;
    SimAccessCondition_t    invalidate;
    SimAccessCondition_t    rehabilitate;
}
SimAccessData_t;



typedef enum UsimSecEnvironmentTag
{
    USIM_SE00 = 0,
    USIM_SE01 = 1,
    USIM_SE_UNKNOWN
}
UsimSecEnvironment_t;


// SimSwStatusCode enum:
//
// Whenever a command is transmitted to the SIM, the SIM returns two bytes
// called SW1 and SW2. The contents and interpretation of these is command
// specfic. The codes below represent a generic set of possible return codes
// in SW1 and SW2.
//
// Note; in some cases the value in SW2 should be read in addition to the
// status below. For example, for code SIM_GS_VALID_RES_LENGTH, SW2 will
// contain the length of response data held in the SIM that may be read
// with the GET RESPONSE command. The SW1 and SW2 codes are detailed in
// section 9.4 of GSM 11.11.
 typedef enum SimSwStatusCodeTag
{                                     				// SW1  SW2     (hex values) 
    SIM_GS_NORMAL_ENDING,             	// 90   00       
    SIM_GS_VALID_RES_LENGTH,          	// 9F   length  
    SIM_GS_UPDATE_RETRIES,            	// 92   retries 
    SIM_GS_MEMORY_PROBLEM,            	// 92   40       
    SIM_GS_NO_EF_SELECTED,            	// 94   00       
    SIM_GS_INVALID_ADDRESS,           	// 94   02       
    SIM_GS_FILE_NOT_FOUND,           	// 94   04       
    SIM_GS_PATTERN_NOT_FOUND,        // 94   04       
    SIM_GS_INCONSISTENT_COMMAND,  // 94   08      
    SIM_GS_NO_CHV_INITIALISED,        // 98   02       
    SIM_GS_ACCESS_DENIED,             	// 98   04       
    SIM_GS_CHV_STAT_CONTRADICT,     // 98   08       
    SIM_GS_INVALID_CONTRADICT,        // 98   10       
    SIM_GS_CODE_BLOCKED,              	// 98   40       
    SIM_GS_MAX_VALUE_REACHED,        // 98   50       
    SIM_GS_INCORRECT_P3,              	// 67   correct length  
    SIM_GS_INCORRECT_P1_OR_P2,        // 6B   given in GSM 11.11 
    SIM_GS_UNKNOWN_INST_CODE,       // 6D   given in GSM 11.11
    SIM_GS_WRONG_INST_CLASS,          // 6E   given in GSM 11.11 
    SIM_GS_TECHNICAL_PROBLEM,         // 6F   given in GSM 11.11 

    // status codes for Phase 2+ SIM Toolkit implemenation 
    SIM_GS_NORMAL_END_WITH_SIM_INFO,  	// 91   length  
    SIM_GS_SIMAT_IS_BUSY,             			// 93   00       
    SIM_GS_UNSPECIFIED_STATUS,         		// 62 57    status not listed in spec returned by faulty SIM   
	
#ifdef UPGRADE_SAT97
    SIM_GS_DDL_ERROR_RES_LENGTH,      		// 9E   length  => Data Download Error  
#endif

//#ifdef __3G_SUPPORT__
    SIM_GS_EOF_REACHED_BEFORE_LE,     		// 62   82  
    SIM_GS_INCORRECT_DATA_PARAM,      		// 6A   80   
    SIM_GS_LC_INCONSISTENT,           			// 6A   87   
    SIM_GS_REF_DATA_NOT_FOUND,        		// 6A   88   
    SIM_GS_EXPECTED_LENGTH,           			// 6C XX   
    SIM_GS_FUNC_IN_CLA_NOT_SUP,       		// 68   00   
    SIM_GS_LOGICAL_CHANNEL_NOT_SUP,   		// 68   81   
    SIM_GS_SECURE_MSG_NOT_SUP,        		// 68   82   
    SIM_GS_COM_NOT_ALLOWED,           			// 6900, 6984, 6985   
    SIM_GS_INVALID_MAC,               			// 98   62   
    SIM_GS_UNDEFINED_APP_ERROR,       		// 98 XX   
    SIM_GS_GSM_CONTEXT_NOT_SUP,       		// 98 64   
//#endif
    SIM_GS_UNKNOWN_STATUS             // unknown SW1, SW2 combination   

}
SimSwStatusCode_t;


typedef enum SimDirIdTag
{
    SIM_DIR_MF                  		=   0x3f00,
    SIM_DIR_DF_MEXE             		=   0x5f3c, 
    SIM_DIR_DF_GRAPHICS         	=   0x5f50,
    SIM_DIR_DF_GSM              		=   0x7f20,
    SIM_DIR_DF_DCS1800          	=   0x7f21,
    SIM_DIR_DF_TELECOM          	=   0x7f10,
    // directories for additional files for enhanced network selection 
    SIM_DIR_DF_ENS              		=   0x7f66,
    SIM_DIR_DF_IRIDIUM          	=   0x5f30,
    // Viag Interkom Directory 
    SIM_DIR_VI                  			=   0x7F43,
//#ifdef __3G_SUPPORT__
    SIM_DIR_DF_PHONEBOOK        	=   0x5f3a,
    SIM_DIR_DF_GSM_ACCESS       	=   0x5f3b,
    SIM_DIR_ADF_USIM            		=   0x7fff,
//#endif
    // used when a field is uninitialised 
    SIM_DIR_INVALID             	=   0x0000
}SimDirId_t;




#define USIM_RETURN_FCP_TEMPLATE       		0x04    
#define USIM_DONT_RETURN_FCP_TEMPLATE  	0x0c    

typedef struct SimChvStatusTag
{
    UINT8                numRemainingRetrys;
    BOOL             secretCodeInitialised;
    BOOL             enabled;
    BOOL             verified;                   // Has PIN been verified 
    BOOL             used;                       // usage qualifier specifies whether the PIN should be used or not 
    UsimKeyRefValue_t  alternativeKeyReference;    // alternative key reference 
} UsimPinStatus_t;

typedef struct UsimDirFreeSpaceTag
{
    UINT8 length;
    UINT8 data[MAX_DIR_FREE_SPACE];
} 
UsimDirFreeSpace_t;


typedef struct UsimSpecificDataTag
{
    UsimKeyRefValue_t    applicationPin;
    UsimPinStatus_t         pinStatus;     			// status of the application PIN
    UsimKeyRefValue_t    localPin;
    UsimPinStatus_t         pin2Status;    			// status of the PIN2 (local PIN) 
    UsimPinStatus_t         unblockPin2Status;    	// status of the UNBLOCK PIN2 (local PIN) 
    UsimState_t               fileState;
    UsimDirFreeSpace_t   dirFreeSpace;
}
UsimSpecificData_t;


typedef struct UsimSfiTag // Short File Identifier 
{
  BOOL supported;
  UINT8    data;
}UsimSfi_t;

typedef struct SimEfInfoTag
{
    UINT8           recordLength;
    UsimSfi_t      sfi;                 // Short File Identifier 
    UINT8           numRecords;
} SimEfInfo_t;


typedef enum UsimTransmissionProtocolTag
{
   USIM_T0 = 0,
   USIM_T1 = 1,
   USIM_T_UNKNOWN

}UsimTransmissionProtocol_t;

// class of instruction 
typedef enum  SimClassByteTag
{
    SIM_3G_CLASS =      0x00,     	//used on 3G SIMs
    SIM_80_CLASS =      0x80,
    SIM_90_CLASS =      0x90,
    SIM_GSM_CLASS =     0xa0      //GSM class
} SimClassByte_t;

// logical channel 
typedef enum UsimLogicalChannelTag
{
   BASIC_CHANNEL = 0,
   LOGICAL_CHANNEL_1  = 1,
   LOGICAL_CHANNEL_2  = 2,
   LOGICAL_CHANNEL_3  = 3,
   MAX_NUM_LOGICAL_CHANNELS = 4
} UsimLogicalChannel_t;

//mode of SELECT 
typedef enum  UsimSelectModeTag
{
   SELECT_BY_FILE_ID                = 0x00,
   SELECT_CHILD_DF_OF_CURRENT_DF    = 0x01,
   SELECT_PARENT_DF_OF_CURRENT_DF   = 0x03,
   SELECT_BY_DF_NAME                = 0x04,
   SELECT_BY_PATH_FROM_MF           = 0x08,
   SELECT_BY_PATH_FROM_DF           = 0x09
} UsimSelectMode_t;



typedef enum UsimApplicationCodeTag
{
  SIM_UICC_APP_CODE,
  SIM_USIM_APP_CODE,
  SIM_USAT_APP_CODE,
  SIM_UNKNOWN_APP_CODE
} UsimApplicationCode_t;

 // Application Id that resides on the UICC see TS 31.110 for coding
typedef struct UsimAidTag
{
  UINT8       data[USIM_AID_MAX_SIZE];         	// AID value 
  UINT8       length;                          			// AID length 
} UsimAid_t;

typedef struct UsimAidLabelTag
{
  UsimApplicationCode_t          applicationType;
  UINT8                            		length;                         			// label length 
  UINT8                            		data[USIM_LABEL_MAX_SIZE];      	// Application label 
} UsimAidLabel_t;

typedef struct UsimApplicationListTag
{
  UINT8                            numUsimApplications;
  UsimAidLabel_t                 applicationLabel [MAX_SIM_APPLICATIONS];
} UsimApplicationList_t;


typedef enum UsimFileLinkingTag   //Used for EF_PBR: type of file linking
{
   USIM_TYPE_1,
   USIM_TYPE_2,
   USIM_TYPE_3,
   USIM_UNDEFINED_TYPE
} UsimFileLinking;



/*******************************************************************************
 *  Function Statement
 ******************************************************************************/


UsimDecodeStatus_t Usim_DecodeDFFcp ( UINT8* RespData);

INT8 Usim_DecodeDataLength ( INT8 *dataLgth,  INT8 *data);
UsimDecodeStatus_t Usim_DecDirFileDescriptor (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecDirId (UINT8  *offset,   UINT8  *respData);
UsimDecodeStatus_t Usim_DecDfName (UINT8    *offset, UINT8  *respData);
UsimDecodeStatus_t Usim_DecProprietaryInfo (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecUiccCharacteristics (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecAppPowerConsumption (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecMinClockFrequency (UINT8  *offset,  UINT8 *respData);
void Usim_DecMemAvailable   (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecDirSecurityAttributes (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecPinStatusTemplateDO  (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecUsageQualifier (UINT8  *offset,  UINT8 *respData, BOOL * usePin);
UsimDecodeStatus_t Usim_DecKeyReference (UINT8  *offset,  UINT8 *respData, BOOL  usePin, BOOL enabled );
void Usim_SetPinStatus (INT8 keyReference, BOOL  usePin,  BOOL  enabled);
void  Usim_DecTotalDirFileSize (UINT8  *offset,  UINT8 *respData);
UsimDecodeStatus_t Usim_DecLifeCycleStatus (UINT8  *offset,  UINT8 *respData);
BOOL Usim_DecodePbrRecord (UINT8 *data, UINT8 length, UINT8 RecordNum, UINT8 flag, UINT8 SimId);

#endif  // __USIM_PROT_H__
