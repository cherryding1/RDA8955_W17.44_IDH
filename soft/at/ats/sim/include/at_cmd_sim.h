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



#ifndef __AT_CMD_SIM_H__
#define __AT_CMD_SIM_H__

struct COS_EVENT;

struct AT_CMD_PARA;

#include "csw.h"
#include "at_module.h"
#include "at_common.h"
#include "at_utility.h"
#include "at_define.h"

// AT_20071114_CAOW_B
VOID AT_SIM_CmdFunc_CRSM(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CAMM(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_SIMIF(AT_CMD_PARA *pParam);
// AT_20071114_CAOW_E

//Add by XP 201311129
VOID AT_SIM_CmdFunc_CRSML(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CRESET(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CPIN(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CPIN2(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CPINC(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CLCK(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CPWD(AT_CMD_PARA *pParam);
VOID AT_SIM_AsyncEventProcess(COS_EVENT *pEvent);
VOID AT_SIM_CmdFunc_CSST(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_SIM(AT_CMD_PARA *pParam);
VOID AT_SIM_CmdFunc_CPIN_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPIN2_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPIN2_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPWD_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CLCK_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CLCK_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPINC_Exe_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPINC_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CPIN1_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);

UINT32 CFW_SimReadBinary(UINT8 nFileId, UINT8 nOffset, UINT8 nBytesToRead, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_SimUpdateBinary(UINT8 nFileId,
                           UINT8 nOffset, UINT8 *pData, UINT8 nBytesToWrite, UINT16 nUTI, CFW_SIM_ID nSimID);
VOID AT_FDN_UpdateRecord(UINT8 nIdx, UINT8 nRecordSize, UINT8 *pData, UINT8 nSim);

UINT32 CFW_SimGetFileStatus( UINT8 nFileID, UINT16 nUTI , CFW_SIM_ID nSimID);
UINT32 CFW_SimUpdateRecord (
    UINT8    nFileID,
    UINT8    nRecordNum,
    UINT8    nRecordSize,
    UINT8   *pData,
    UINT16   nUTI,
    CFW_SIM_ID nSimID
);

VOID AT_SIM_CmdFunc_CRSM_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_SIM_CmdFunc_CRSML_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult);
VOID AT_FDN_SetStatus(UINT8 nSim);
VOID AT_SIMID_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime,
                        UINT8 *pBuffer, UINT16 nDataSize, UINT8 nUTI);
UINT32 CFW_SimReset(UINT16 nUTI, CFW_SIM_ID nSimID);
UINT8 CFW_GetSSTStatus(UINT8 nServiceNum, CFW_SIM_ID nSimID);
VOID AT_SIM_CmdFunc_SETSIM(AT_CMD_PARA *pParam);
#define CRSML_STATUS_IDLE       0x01
#define CRSML_STATUS_FILE_1 0x02
#define CRSML_STATUS_FILE_2 0x03
#define CRSML_STATUS_FILE_END   0x04

UINT32 CFW_GetPBKTotalNum(CFW_SIM_ID nSimID);
UINT32 CFW_GetPBKRecordNum(CFW_SIM_ID nSimID);

extern UINT8 CFW_GetSimStatus(CFW_SIM_ID nSimID);
extern UINT32 CFW_SimGetACMMax(UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimReadRecordWithLen (
    UINT8 nFileID,
    UINT8 nRecordNum,
    UINT16 nLen,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);
BOOL CFW_GetSimType(CFW_SIM_ID nSimID);
u8 CFW_GetSimADNType(CFW_SIM_ID nSimID);

#define CRMPDATALENGTH      120
#define CAMMLENGTH          20
#define PIN2CODELENGTH      12

// add for crsm begin
#define  API_SIM_EF_ADN       0
#define  API_SIM_EF_FDN       1
#define  API_SIM_EF_SMS       2
#define  API_SIM_EF_CCP       3
#define  API_SIM_EF_MSISDN    4
#define  API_SIM_EF_SMSP      5
#define  API_SIM_EF_SMSS      6
#define  API_SIM_EF_LND       7
#define  API_SIM_EF_SMSR      8
#define  API_SIM_EF_SDN       9
#define  API_SIM_EF_EXT1      10
#define  API_SIM_EF_EXT2      11
#define  API_SIM_EF_EXT3      12
#define  API_SIM_EF_BDN       13
#define  API_SIM_EF_EXT4      14
#define  API_SIM_EF_LP        15
#define  API_SIM_EF_IMSI      16
#define  API_SIM_EF_KC        17
#define  API_SIM_EF_PLMNSEL   18
#define  API_SIM_EF_HPLMN     19
#define  API_SIM_EF_ACMMAX    20
#define  API_SIM_EF_SST       21
#define  API_SIM_EF_ACM       22  // 
#define  API_SIM_EF_GID1      23
#define  API_SIM_EF_GID2      24
#define  API_SIM_EF_PUCT      25
#define  API_SIM_EF_CBMI      26
#define  API_SIM_EF_SPN       27
#define  API_SIM_EF_CBMID     28
#define  API_SIM_EF_BCCH      29
#define  API_SIM_EF_ACC       30
#define  API_SIM_EF_FPLMN     31
#define  API_SIM_EF_LOCI      32
#define  API_SIM_EF_AD        33
#define  API_SIM_EF_PHASE     34
#define  API_SIM_EF_VGCS      35
#define  API_SIM_EF_VGCSS     36
#define  API_SIM_EF_VBS       37
#define  API_SIM_EF_VBSS      38
#define  API_SIM_EF_EMLPP     39
#define  API_SIM_EF_AAEM      40
#define  API_SIM_EF_ECC       41
#define  API_SIM_EF_CBMIR     42
#define  API_SIM_EF_NIA       43  // 
#define  API_SIM_EF_KCGPRS    44
#define  API_SIM_EF_LOCIGPRS  45
#define  API_SIM_EF_SUME      46
#define  API_SIM_EF_PLMNWACT  47
#define  API_SIM_EF_OPLMNWACT 48
#define  API_SIM_EF_HPLMNACT  49
#define  API_SIM_EF_CPBCCH    50
#define  API_SIM_EF_INVSCAN   51
#define  API_SIM_EF_RPLMNAC   52
#define  API_SIM_EF_ICCID     53
#define  API_SIM_EF_ELP       54
#define  API_SIM_EF_IMG       55  // 
#define  API_SIM_EF_MBDN      56


// add for 3G USIM
#define    API_USIM_EF_LI           57
#define    API_USIM_EF_ARR              58
#define    API_USIM_EF_IMSI             59
#define    API_USIM_EF_KEYS             60
#define    API_USIM_EF_KEYS_PS          61
#define    API_USIM_EF_DCK             62
#define    API_USIM_EF_HPLMN            63
#define    API_USIM_EF_CNL             64
#define    API_USIM_EF_ACM_MAX      65
#define    API_USIM_EF_UST              66
#define    API_USIM_EF_ACM              67
#define    API_USIM_EF_FDN              68
#define    API_USIM_EF_SMS              69
#define    API_USIM_EF_GID1             70
#define    API_USIM_EF_GID2             71
#define    API_USIM_EF_MSISDN       72
#define    API_USIM_EF_PUCT             73
#define    API_USIM_EF_SMSP             74
#define    API_USIM_EF_SMSS             75
#define    API_USIM_EF_CBMI             76
#define    API_USIM_EF_SPN              77
#define    API_USIM_EF_SMSR             78
#define    API_USIM_EF_CBMID            79
#define    API_USIM_EF_SDN              80
#define    API_USIM_EF_EXT2             81
#define    API_USIM_EF_EXT3             82
#define    API_USIM_EF_BDN              83
#define    API_USIM_EF_EXT5             84
#define    API_USIM_EF_CBMIR            85
#define    API_USIM_EF_EXT4             86
#define    API_USIM_EF_EST              87
#define    API_USIM_EF_ACL              88
#define    API_USIM_EF_CMI              89
#define    API_USIM_EF_START_HFN    90
#define    API_USIM_EF_THRESHOLD    91
#define    API_USIM_EF_PLMNWACT      92
#define    API_USIM_EF_OPLMNWACT    93
#define    API_USIM_EF_HPLMNWACT    94
#define    API_USIM_EF_EHPLMN              95
#define    API_USIM_EF_PS_LOCI      96
#define    API_USIM_EF_ACC              97
#define    API_USIM_EF_FPLMN        98
#define    API_USIM_EF_LOCI             99
#define    API_USIM_EF_ICI              100
#define    API_USIM_EF_OCI              101
#define    API_USIM_EF_ICT              102
#define    API_USIM_EF_OCT              103
#define    API_USIM_EF_AD           104
#define    API_USIM_EF_EMLPP            105
#define    API_USIM_EF_AAEM            106
#define    API_USIM_EF_ECC             107
#define    API_USIM_EF_HIDDENKEY    108
#define    API_USIM_EF_NETPAR       109
#define    API_USIM_EF_PNN              110
#define    API_USIM_EF_OPL              111
#define    API_USIM_EF_MBDN             112
#define    API_USIM_EF_EXT6             113
#define    API_USIM_EF_MBI              114
#define    API_USIM_EF_MWIS             115
#define    API_USIM_EF_CFIS             116
#define    API_USIM_EF_EXT7             117
#define    API_USIM_EF_SPDI            118
#define    API_USIM_EF_MMSN            119
#define    API_USIM_EF_EXT8            120
#define    API_USIM_EF_MMSICP          121
#define    API_USIM_EF_MMSUP           122
#define    API_USIM_EF_MMSUCP          123
#define    API_USIM_EF_NIA             124
#define    API_USIM_EF_VGCS            125
#define    API_USIM_EF_VGCSS            126
#define    API_USIM_EF_VBS             127
#define    API_USIM_EF_VBSS            128
#define    API_USIM_EF_VGCSCA          129
#define    API_USIM_EF_VBSCA            130
#define    API_USIM_EF_GBAP            131
#define    API_USIM_EF_MSK             132
#define    API_USIM_EF_MUK             133
#define    API_USIM_EF_GBANL            134
#define    API_USIM_EF_EHPLMNPI         135
#define    API_USIM_EF_LRPLMNSI        136
#define    API_USIM_EF_NAFKCA          137

//files under DF MEXE
#define    API_USIM_EF_MEXE_ST      138
#define    API_USIM_EF_ARPK             139
#define    API_USIM_EF_TPRK             140
#define    API_USIM_EF_ORPK                141

//files under DF GSM ACCESS
#define    API_USIM_EF_KC               142
#define    API_USIM_EF_KC_GPRS      143
#define    API_USIM_EF_CPBCCH       144
#define    API_USIM_EF_INVSCAN      145

// file under MF
#define    API_USIM_EF_DIR                 146
#define    API_USIM_EF_ARR_MF       147

// phonebook under DF_TELECOM, USIM globle phonebook
#define    API_USIM_EF_GB_PBR       148
#define    API_USIM_EF_GB_IAP       149
#define    API_USIM_EF_GB_IAP1      150
#define    API_USIM_EF_GB_ADN       151
#define    API_USIM_EF_GB_ADN1      152
#define    API_USIM_EF_GB_EXT1      153
#define    API_USIM_EF_GB_PBC       154
#define    API_USIM_EF_GB_PBC1      155
#define    API_USIM_EF_GB_GRP       156
#define    API_USIM_EF_GB_GRP1      157
#define    API_USIM_EF_GB_AAS       158
#define    API_USIM_EF_GB_GAS       159
#define    API_USIM_EF_GB_ANRA      160
#define    API_USIM_EF_GB_ANRB      161
#define    API_USIM_EF_GB_ANRC      162
#define    API_USIM_EF_GB_ANRA1 163
#define    API_USIM_EF_GB_ANRB1     164
#define    API_USIM_EF_GB_ANRC1     165
#define    API_USIM_EF_GB_SNE       166
#define    API_USIM_EF_GB_SNE1      167
#define    API_USIM_EF_GB_CCP1      168
#define    API_USIM_EF_GB_UID       169
#define    API_USIM_EF_GB_UID1      170
#define    API_USIM_EF_GB_PSC       171
#define    API_USIM_EF_GB_CC        172
#define    API_USIM_EF_GB_PUID      173
#define    API_USIM_EF_GB_EMAIL     174
#define    API_USIM_EF_GB_EMAIL1    175

// phonebook under ADF USIM, USIM local phonebook
#define    API_USIM_EF_PBR          176
#define    API_USIM_EF_IAP          177
#define    API_USIM_EF_IAP1         178
#define    API_USIM_EF_ADN          179
#define    API_USIM_EF_ADN1         180
#define    API_USIM_EF_EXT1         181
#define    API_USIM_EF_PBC          182
#define    API_USIM_EF_PBC1         183
#define    API_USIM_EF_GRP          184
#define    API_USIM_EF_GRP1         185
#define    API_USIM_EF_AAS          186
#define    API_USIM_EF_GAS          187
#define    API_USIM_EF_ANRA         188
#define    API_USIM_EF_ANRB         189
#define    API_USIM_EF_ANRC         190
#define    API_USIM_EF_ANRA1        191
#define    API_USIM_EF_ANRB1        192
#define    API_USIM_EF_ANRC1        193
#define    API_USIM_EF_SNE          194
#define    API_USIM_EF_SNE1         195
#define    API_USIM_EF_CCP1         196
#define    API_USIM_EF_UID          197
#define    API_USIM_EF_UID1         198
#define    API_USIM_EF_PSC          199
#define    API_USIM_EF_CC           200
#define    API_USIM_EF_PUID         201
#define    API_USIM_EF_EMAIL        202
#define    API_USIM_EF_EMAIL1       203


#endif
