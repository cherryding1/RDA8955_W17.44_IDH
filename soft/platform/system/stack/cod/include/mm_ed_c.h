/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c
	http://www.Dafocus.it/
**************************************************************************/
// REVISION HISTORY:
// DMX | 20070330 | add DECODE_c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD foe EDGE
// **************************************************************************

#ifndef __mm_ed_c_h_H
#define __mm_ed_c_h_H
#include "ed_data.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "rrd_ed_c.h"
#include "rri_ed_c.h"

/* Access member 'NwkFullName' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_NwkFullName(var) var

/* Access member 'NwkFullName' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_NwkFullName(var) (&var)

/* Access member 'NwkShortName' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_NwkShortName(var) var

/* Access member 'NwkShortName' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_NwkShortName(var) (&var)

/* Access member 'LocalTimeZone' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_LocalTimeZone(var) var

/* Access member 'LocalTimeZone' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_LocalTimeZone(var) (&var)

/* Access member 'UniversalTime' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_UniversalTime(var) var

/* Access member 'UniversalTime' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_UniversalTime(var) (&var)

/* Access member 'LSAId' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_LSAId(var) var

/* Access member 'LSAId' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_LSAId(var) (&var)

/* Access member 'NwkDaylightSavingTime' of type 'c_InformationIE_t' as a variable reference */
#define VAR_c_InformationIE_t_NwkDaylightSavingTime(var) var

/* Access member 'NwkDaylightSavingTime' of type 'c_InformationIE_t' as a pointer */
#define PTR_c_InformationIE_t_NwkDaylightSavingTime(var) (&var)

/* DEFINITION OF BINARY c_InformationIE_t_NwkFullName */
typedef struct _c_InformationIE_t_NwkFullName {
	ED_BYTE value [247]; /* Variable size; bits needed 1976 */
	int usedBits;
} c_InformationIE_t_NwkFullName;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_InformationIE_t_NwkFullName(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_InformationIE_t_NwkShortName */
typedef struct _c_InformationIE_t_NwkShortName {
	ED_BYTE value [247]; /* Variable size; bits needed 1976 */
	int usedBits;
} c_InformationIE_t_NwkShortName;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_InformationIE_t_NwkShortName(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_InformationIE_t_LSAId */
typedef struct _c_InformationIE_t_LSAId {
	ED_BYTE value [3]; /* Variable size; bits needed 24 */
	int usedBits;
} c_InformationIE_t_LSAId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_InformationIE_t_LSAId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_InformationIE_t {
	ED_BYTE UniversalTime [7];
	ED_BOOLEAN NwkFullName_Present;
	ED_BOOLEAN NwkShortName_Present;
	ED_OCTET LocalTimeZone;
	ED_BOOLEAN LocalTimeZone_Present;
	ED_BOOLEAN UniversalTime_Present;
	ED_BOOLEAN LSAId_Present;
	ED_OCTET NwkDaylightSavingTime;
	ED_BOOLEAN NwkDaylightSavingTime_Present;
	c_InformationIE_t_NwkFullName NwkFullName;
	c_InformationIE_t_NwkShortName NwkShortName;
	c_InformationIE_t_LSAId LSAId;

}	c_InformationIE_t;
#define INIT_c_InformationIE_t(sp) ED_RESET_MEM ((sp), sizeof (c_InformationIE_t))
#define FREE_c_InformationIE_t(sp)
#define SETPRESENT_c_InformationIE_t_NwkFullName(sp,present) ((sp)->NwkFullName_Present = present)
#define GETPRESENT_c_InformationIE_t_NwkFullName(sp) ((sp)->NwkFullName_Present)
#define SETPRESENT_c_InformationIE_t_NwkShortName(sp,present) ((sp)->NwkShortName_Present = present)
#define GETPRESENT_c_InformationIE_t_NwkShortName(sp) ((sp)->NwkShortName_Present)
#define SETPRESENT_c_InformationIE_t_LocalTimeZone(sp,present) ((sp)->LocalTimeZone_Present = present)
#define GETPRESENT_c_InformationIE_t_LocalTimeZone(sp) ((sp)->LocalTimeZone_Present)
#define SETPRESENT_c_InformationIE_t_UniversalTime(sp,present) ((sp)->UniversalTime_Present = present)
#define GETPRESENT_c_InformationIE_t_UniversalTime(sp) ((sp)->UniversalTime_Present)
#define SETPRESENT_c_InformationIE_t_LSAId(sp,present) ((sp)->LSAId_Present = present)
#define GETPRESENT_c_InformationIE_t_LSAId(sp) ((sp)->LSAId_Present)
#define SETPRESENT_c_InformationIE_t_NwkDaylightSavingTime(sp,present) ((sp)->NwkDaylightSavingTime_Present = present)
#define GETPRESENT_c_InformationIE_t_NwkDaylightSavingTime(sp) ((sp)->NwkDaylightSavingTime_Present)

/* Access member 'FOR' of type 'c_LocUpReqType' as a variable reference */
#define VAR_c_LocUpReqType_FOR(var) var

/* Access member 'FOR' of type 'c_LocUpReqType' as a pointer */
#define PTR_c_LocUpReqType_FOR(var) (&var)

/* Access member 'spare' of type 'c_LocUpReqType' as a variable reference */
#define VAR_c_LocUpReqType_spare(var) var

/* Access member 'spare' of type 'c_LocUpReqType' as a pointer */
#define PTR_c_LocUpReqType_spare(var) (&var)

/* Access member 'LocUpType' of type 'c_LocUpReqType' as a variable reference */
#define VAR_c_LocUpReqType_LocUpType(var) var

/* Access member 'LocUpType' of type 'c_LocUpReqType' as a pointer */
#define PTR_c_LocUpReqType_LocUpType(var) (&var)


/*-----------------------------------*/
typedef struct _c_LocUpReqType {
	ED_OCTET FOR;
	ED_OCTET LocUpType;

}	c_LocUpReqType;
#define INIT_c_LocUpReqType(sp) ED_RESET_MEM ((sp), sizeof (c_LocUpReqType))
#define FREE_c_LocUpReqType(sp)

/* Access member 'FOR' of type 'c_AttachTypeIE' as a variable reference */
#define VAR_c_AttachTypeIE_FOR(var) var

/* Access member 'FOR' of type 'c_AttachTypeIE' as a pointer */
#define PTR_c_AttachTypeIE_FOR(var) (&var)

/* Access member 'AttachType' of type 'c_AttachTypeIE' as a variable reference */
#define VAR_c_AttachTypeIE_AttachType(var) var

/* Access member 'AttachType' of type 'c_AttachTypeIE' as a pointer */
#define PTR_c_AttachTypeIE_AttachType(var) (&var)


/*-----------------------------------*/
typedef struct _c_AttachTypeIE {
	ED_BOOLEAN FOR;
	ED_OCTET AttachType;

}	c_AttachTypeIE;
#define INIT_c_AttachTypeIE(sp) ED_RESET_MEM ((sp), sizeof (c_AttachTypeIE))
#define FREE_c_AttachTypeIE(sp)

/* Access member 'spare' of type 'c_CiphAlgoIE' as a variable reference */
#define VAR_c_CiphAlgoIE_spare(var) var

/* Access member 'spare' of type 'c_CiphAlgoIE' as a pointer */
#define PTR_c_CiphAlgoIE_spare(var) (&var)

/* Access member 'AlgoType' of type 'c_CiphAlgoIE' as a variable reference */
#define VAR_c_CiphAlgoIE_AlgoType(var) var

/* Access member 'AlgoType' of type 'c_CiphAlgoIE' as a pointer */
#define PTR_c_CiphAlgoIE_AlgoType(var) (&var)


/*-----------------------------------*/
typedef struct _c_CiphAlgoIE {
	ED_OCTET AlgoType;

}	c_CiphAlgoIE;
#define INIT_c_CiphAlgoIE(sp) ED_RESET_MEM ((sp), sizeof (c_CiphAlgoIE))
#define FREE_c_CiphAlgoIE(sp)

/* Access member 'SplitPGCycle' of type 'c_DRXParamIE' as a variable reference */
#define VAR_c_DRXParamIE_SplitPGCycle(var) var

/* Access member 'SplitPGCycle' of type 'c_DRXParamIE' as a pointer */
#define PTR_c_DRXParamIE_SplitPGCycle(var) (&var)

/* Access member 'CNSpeciDRXCoef' of type 'c_DRXParamIE' as a variable reference */
#define VAR_c_DRXParamIE_CNSpeciDRXCoef(var) var

/* Access member 'CNSpeciDRXCoef' of type 'c_DRXParamIE' as a pointer */
#define PTR_c_DRXParamIE_CNSpeciDRXCoef(var) (&var)

/* Access member 'SplitOnCCCH' of type 'c_DRXParamIE' as a variable reference */
#define VAR_c_DRXParamIE_SplitOnCCCH(var) var

/* Access member 'SplitOnCCCH' of type 'c_DRXParamIE' as a pointer */
#define PTR_c_DRXParamIE_SplitOnCCCH(var) (&var)

/* Access member 'NonDRXTim' of type 'c_DRXParamIE' as a variable reference */
#define VAR_c_DRXParamIE_NonDRXTim(var) var

/* Access member 'NonDRXTim' of type 'c_DRXParamIE' as a pointer */
#define PTR_c_DRXParamIE_NonDRXTim(var) (&var)


/*-----------------------------------*/
typedef struct _c_DRXParamIE {
	ED_OCTET SplitPGCycle;
	ED_OCTET CNSpeciDRXCoef;
	ED_BOOLEAN SplitOnCCCH;
	ED_OCTET NonDRXTim;

}	c_DRXParamIE;
#define INIT_c_DRXParamIE(sp) ED_RESET_MEM ((sp), sizeof (c_DRXParamIE))
#define FREE_c_DRXParamIE(sp)

/* Access member 'spare' of type 'c_IMEISVRequest' as a variable reference */
#define VAR_c_IMEISVRequest_spare(var) var

/* Access member 'spare' of type 'c_IMEISVRequest' as a pointer */
#define PTR_c_IMEISVRequest_spare(var) (&var)

/* Access member 'IMEISVRequestValue' of type 'c_IMEISVRequest' as a variable reference */
#define VAR_c_IMEISVRequest_IMEISVRequestValue(var) var

/* Access member 'IMEISVRequestValue' of type 'c_IMEISVRequest' as a pointer */
#define PTR_c_IMEISVRequest_IMEISVRequestValue(var) (&var)


/*-----------------------------------*/
typedef struct _c_IMEISVRequest {
	ED_OCTET IMEISVRequestValue;

}	c_IMEISVRequest;
#define INIT_c_IMEISVRequest(sp) ED_RESET_MEM ((sp), sizeof (c_IMEISVRequest))
#define FREE_c_IMEISVRequest(sp)

/* Access member 'Unit' of type 'c_GPRSTimerIE' as a variable reference */
#define VAR_c_GPRSTimerIE_Unit(var) var

/* Access member 'Unit' of type 'c_GPRSTimerIE' as a pointer */
#define PTR_c_GPRSTimerIE_Unit(var) (&var)

/* Access member 'Value' of type 'c_GPRSTimerIE' as a variable reference */
#define VAR_c_GPRSTimerIE_Value(var) var

/* Access member 'Value' of type 'c_GPRSTimerIE' as a pointer */
#define PTR_c_GPRSTimerIE_Value(var) (&var)


/*-----------------------------------*/
typedef struct _c_GPRSTimerIE {
	ED_OCTET Unit;
	ED_OCTET Value;

}	c_GPRSTimerIE;
#define INIT_c_GPRSTimerIE(sp) ED_RESET_MEM ((sp), sizeof (c_GPRSTimerIE))
#define FREE_c_GPRSTimerIE(sp)

/* Access member 'FOR' of type 'c_RAUTypeIE' as a variable reference */
#define VAR_c_RAUTypeIE_FOR(var) var

/* Access member 'FOR' of type 'c_RAUTypeIE' as a pointer */
#define PTR_c_RAUTypeIE_FOR(var) (&var)

/* Access member 'RAUType' of type 'c_RAUTypeIE' as a variable reference */
#define VAR_c_RAUTypeIE_RAUType(var) var

/* Access member 'RAUType' of type 'c_RAUTypeIE' as a pointer */
#define PTR_c_RAUTypeIE_RAUType(var) (&var)


/*-----------------------------------*/
typedef struct _c_RAUTypeIE {
	ED_OCTET FOR;
	ED_OCTET RAUType;

}	c_RAUTypeIE;
#define INIT_c_RAUTypeIE(sp) ED_RESET_MEM ((sp), sizeof (c_RAUTypeIE))
#define FREE_c_RAUTypeIE(sp)

/* Access member 'data' of type 'c_RxNpduNumList_NpduValue' as a variable reference */
#define VAR_c_RxNpduNumList_NpduValue_data(var) var

/* Access member 'data' of type 'c_RxNpduNumList_NpduValue' as a pointer */
#define PTR_c_RxNpduNumList_NpduValue_data(var) (&var)

/* Access member 'NpduValue' of type 'c_RxNpduNumList' as a variable reference */
#define VAR_c_RxNpduNumList_NpduValue(var) var

/* Access member 'NpduValue' of type 'c_RxNpduNumList' as a pointer */
#define PTR_c_RxNpduNumList_NpduValue(var) (&var)

/* Access member 'data' of type 'c_RxNpduNumList_Nsapi' as a variable reference */
#define VAR_c_RxNpduNumList_Nsapi_data(var) var

/* Access member 'data' of type 'c_RxNpduNumList_Nsapi' as a pointer */
#define PTR_c_RxNpduNumList_Nsapi_data(var) (&var)

/* Access member 'Nsapi' of type 'c_RxNpduNumList' as a variable reference */
#define VAR_c_RxNpduNumList_Nsapi(var) var

/* Access member 'Nsapi' of type 'c_RxNpduNumList' as a pointer */
#define PTR_c_RxNpduNumList_Nsapi(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_RxNpduNumList_NpduValue */
typedef struct _c_RxNpduNumList_NpduValue {
	ED_OCTET data [11];
	int items;
} c_RxNpduNumList_NpduValue;

/* DEFINITION OF SUB-STRUCTURE c_RxNpduNumList_Nsapi */
typedef struct _c_RxNpduNumList_Nsapi {
	ED_OCTET data [11];
	int items;
} c_RxNpduNumList_Nsapi;


/*-----------------------------------*/
typedef struct _c_RxNpduNumList {
	c_RxNpduNumList_NpduValue NpduValue;
	c_RxNpduNumList_Nsapi Nsapi;

}	c_RxNpduNumList;
#define INIT_c_RxNpduNumList(sp) ED_RESET_MEM ((sp), sizeof (c_RxNpduNumList))
#define FREE_c_RxNpduNumList(sp)

/* Access member 'MsNwkCapability' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_MsNwkCapability(var) var

/* Access member 'MsNwkCapability' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_MsNwkCapability(var) (&var)

/* Access member 'AttachType' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_AttachType(var) var

/* Access member 'AttachType' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_AttachType(var) (&var)

/* Access member 'GprsCKSN' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_GprsCKSN(var) var

/* Access member 'GprsCKSN' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_GprsCKSN(var) (&var)

/* Access member 'DRXParam' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_DRXParam(var) var

/* Access member 'DRXParam' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_DRXParam(var) (&var)

/* Access member 'MobileId' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_MobileId(var) var

/* Access member 'MobileId' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_MobileId(var) (&var)

/* Access member 'OldRAI' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_OldRAI(var) var

/* Access member 'OldRAI' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_OldRAI(var) (&var)

/* Access member 'MsRACap' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_MsRACap(var) var

/* Access member 'MsRACap' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_MsRACap(var) (&var)

/* Access member 'OldPTMSISign' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_OldPTMSISign(var) var

/* Access member 'OldPTMSISign' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_OldPTMSISign(var) (&var)

/* Access member 'ReqReadyTimerVal' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_ReqReadyTimerVal(var) var

/* Access member 'ReqReadyTimerVal' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_ReqReadyTimerVal(var) (&var)

/* Access member 'TMSIStatus' of type 'c_ATTACH_REQUEST' as a variable reference */
#define VAR_c_ATTACH_REQUEST_TMSIStatus(var) var

/* Access member 'TMSIStatus' of type 'c_ATTACH_REQUEST' as a pointer */
#define PTR_c_ATTACH_REQUEST_TMSIStatus(var) (&var)

/* DEFINITION OF BINARY c_ATTACH_REQUEST_MsNwkCapability */
typedef struct _c_ATTACH_REQUEST_MsNwkCapability {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ATTACH_REQUEST_MsNwkCapability;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ATTACH_REQUEST_MsNwkCapability(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_ATTACH_REQUEST_MobileId */
typedef struct _c_ATTACH_REQUEST_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ATTACH_REQUEST_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ATTACH_REQUEST_MobileId(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_ATTACH_REQUEST_MsRACap */
typedef struct _c_ATTACH_REQUEST_MsRACap {
	ED_BYTE value [51]; /* Variable size; bits needed 408 */
	int usedBits;
} c_ATTACH_REQUEST_MsRACap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ATTACH_REQUEST_MsRACap(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_ATTACH_REQUEST {
	ED_BYTE OldRAI [6];
	ED_BYTE OldPTMSISign [3];
	ED_OCTET GprsCKSN;
	ED_BOOLEAN OldPTMSISign_Present;
	ED_OCTET ReqReadyTimerVal;
	ED_BOOLEAN ReqReadyTimerVal_Present;
	ED_OCTET TMSIStatus;
	ED_BOOLEAN TMSIStatus_Present;
	c_ATTACH_REQUEST_MsNwkCapability MsNwkCapability;
	c_AttachTypeIE AttachType;
	c_DRXParamIE DRXParam;
	c_ATTACH_REQUEST_MobileId MobileId;
	c_ATTACH_REQUEST_MsRACap MsRACap;

}	c_ATTACH_REQUEST;
#define INIT_c_ATTACH_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_ATTACH_REQUEST))
#define FREE_c_ATTACH_REQUEST(sp)
#define SETPRESENT_c_ATTACH_REQUEST_OldPTMSISign(sp,present) ((sp)->OldPTMSISign_Present = present)
#define GETPRESENT_c_ATTACH_REQUEST_OldPTMSISign(sp) ((sp)->OldPTMSISign_Present)
#define SETPRESENT_c_ATTACH_REQUEST_ReqReadyTimerVal(sp,present) ((sp)->ReqReadyTimerVal_Present = present)
#define GETPRESENT_c_ATTACH_REQUEST_ReqReadyTimerVal(sp) ((sp)->ReqReadyTimerVal_Present)
#define SETPRESENT_c_ATTACH_REQUEST_TMSIStatus(sp,present) ((sp)->TMSIStatus_Present = present)
#define GETPRESENT_c_ATTACH_REQUEST_TMSIStatus(sp) ((sp)->TMSIStatus_Present)

/* Access member 'DetachType' of type 'c_DETACH_REQUEST_MO' as a variable reference */
#define VAR_c_DETACH_REQUEST_MO_DetachType(var) var

/* Access member 'DetachType' of type 'c_DETACH_REQUEST_MO' as a pointer */
#define PTR_c_DETACH_REQUEST_MO_DetachType(var) (&var)

/* Access member 'Spare' of type 'c_DETACH_REQUEST_MO' as a variable reference */
#define VAR_c_DETACH_REQUEST_MO_Spare(var) var

/* Access member 'Spare' of type 'c_DETACH_REQUEST_MO' as a pointer */
#define PTR_c_DETACH_REQUEST_MO_Spare(var) (&var)

/* Access member 'PTMSI' of type 'c_DETACH_REQUEST_MO' as a variable reference */
#define VAR_c_DETACH_REQUEST_MO_PTMSI(var) var

/* Access member 'PTMSI' of type 'c_DETACH_REQUEST_MO' as a pointer */
#define PTR_c_DETACH_REQUEST_MO_PTMSI(var) (&var)

/* Access member 'PTMSISign' of type 'c_DETACH_REQUEST_MO' as a variable reference */
#define VAR_c_DETACH_REQUEST_MO_PTMSISign(var) var

/* Access member 'PTMSISign' of type 'c_DETACH_REQUEST_MO' as a pointer */
#define PTR_c_DETACH_REQUEST_MO_PTMSISign(var) (&var)


/*-----------------------------------*/
typedef struct _c_DETACH_REQUEST_MO {
	ED_BYTE PTMSI [5];
	ED_BYTE PTMSISign [3];
	ED_OCTET DetachType;
	ED_BOOLEAN PTMSI_Present;
	ED_BOOLEAN PTMSISign_Present;

}	c_DETACH_REQUEST_MO;
#define INIT_c_DETACH_REQUEST_MO(sp) ED_RESET_MEM ((sp), sizeof (c_DETACH_REQUEST_MO))
#define FREE_c_DETACH_REQUEST_MO(sp)
#define SETPRESENT_c_DETACH_REQUEST_MO_PTMSI(sp,present) ((sp)->PTMSI_Present = present)
#define GETPRESENT_c_DETACH_REQUEST_MO_PTMSI(sp) ((sp)->PTMSI_Present)
#define SETPRESENT_c_DETACH_REQUEST_MO_PTMSISign(sp,present) ((sp)->PTMSISign_Present = present)
#define GETPRESENT_c_DETACH_REQUEST_MO_PTMSISign(sp) ((sp)->PTMSISign_Present)

/* Access member 'ACReferencNumber' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_ACReferencNumber(var) var

/* Access member 'ACReferencNumber' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_ACReferencNumber(var) (&var)

/* Access member 'Spare' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_Spare(var) var

/* Access member 'Spare' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_Spare(var) (&var)

/* Access member 'AuthParamResp' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_AuthParamResp(var) var

/* Access member 'AuthParamResp' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_AuthParamResp(var) (&var)

/* Access member 'IMEISV' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_IMEISV(var) var

/* Access member 'IMEISV' of type 'c_AUTHENTICATION_AND_CIPHERING_RESPONSE' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_IMEISV(var) (&var)


/*-----------------------------------*/
typedef struct _c_AUTHENTICATION_AND_CIPHERING_RSP_Auth_Ext {
	ED_BYTE value [12];
	int usedBits;
}	c_AUTHENTICATION_AND_CIPHERING_RSP_Auth_Ext;
typedef struct _c_AUTHENTICATION_AND_CIPHERING_RESPONSE {
	ED_BOOLEAN AuthParamResp_Ext_Present;
	ED_BOOLEAN AuthParamResp_Present;
	ED_BOOLEAN IMEISV_Present;

	ED_BYTE IMEISV [9];
	ED_BYTE AuthParamResp [4];
	
	c_AUTHENTICATION_AND_CIPHERING_RSP_Auth_Ext AuthParamResp_Ext;
	ED_OCTET ACReferencNumber;
	
}	c_AUTHENTICATION_AND_CIPHERING_RESPONSE;
#define INIT_c_AUTHENTICATION_AND_CIPHERING_RESPONSE(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_AND_CIPHERING_RESPONSE))
#define FREE_c_AUTHENTICATION_AND_CIPHERING_RESPONSE(sp)
#define SETPRESENT_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_AuthParamResp(sp,present) ((sp)->AuthParamResp_Present = present)
#define GETPRESENT_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_AuthParamResp(sp) ((sp)->AuthParamResp_Present)
#define SETPRESENT_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_IMEISV(sp,present) ((sp)->IMEISV_Present = present)
#define GETPRESENT_c_AUTHENTICATION_AND_CIPHERING_RESPONSE_IMEISV(sp) ((sp)->IMEISV_Present)

/* Access member 'MSId' of type 'c_PIDENTITY_RESPONSE' as a variable reference */
#define VAR_c_PIDENTITY_RESPONSE_MSId(var) var

/* Access member 'MSId' of type 'c_PIDENTITY_RESPONSE' as a pointer */
#define PTR_c_PIDENTITY_RESPONSE_MSId(var) (&var)

/* DEFINITION OF BINARY c_PIDENTITY_RESPONSE_MSId */
typedef struct _c_PIDENTITY_RESPONSE_MSId {
	ED_BYTE value [9]; /* Variable size; bits needed 72 */
	int usedBits;
} c_PIDENTITY_RESPONSE_MSId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PIDENTITY_RESPONSE_MSId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_PIDENTITY_RESPONSE {
	c_PIDENTITY_RESPONSE_MSId MSId;

}	c_PIDENTITY_RESPONSE;
#define INIT_c_PIDENTITY_RESPONSE(sp) ED_RESET_MEM ((sp), sizeof (c_PIDENTITY_RESPONSE))
#define FREE_c_PIDENTITY_RESPONSE(sp)

/* Access member 'UpdateType' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_UpdateType(var) var

/* Access member 'UpdateType' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_UpdateType(var) (&var)

/* Access member 'GprsCKSN' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_GprsCKSN(var) var

/* Access member 'GprsCKSN' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_GprsCKSN(var) (&var)

/* Access member 'OldRAI' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_OldRAI(var) var

/* Access member 'OldRAI' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_OldRAI(var) (&var)

/* Access member 'MsRACap' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_MsRACap(var) var

/* Access member 'MsRACap' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_MsRACap(var) (&var)

/* Access member 'OldPTMSISign' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_OldPTMSISign(var) var

/* Access member 'OldPTMSISign' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_OldPTMSISign(var) (&var)

/* Access member 'ReqReadyTimerVal' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_ReqReadyTimerVal(var) var

/* Access member 'ReqReadyTimerVal' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_ReqReadyTimerVal(var) (&var)

/* Access member 'DRXParam' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_DRXParam(var) var

/* Access member 'DRXParam' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_DRXParam(var) (&var)

/* Access member 'TMSIStatus' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_TMSIStatus(var) var

/* Access member 'TMSIStatus' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_TMSIStatus(var) (&var)

/* Access member 'PTMSI' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_PTMSI(var) var

/* Access member 'PTMSI' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_PTMSI(var) (&var)

/* Access member 'MsNwkCapability' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_MsNwkCapability(var) var

/* Access member 'MsNwkCapability' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_MsNwkCapability(var) (&var)

/* Access member 'PDPContextStatus' of type 'c_RAU_REQUEST' as a variable reference */
#define VAR_c_RAU_REQUEST_PDPContextStatus(var) var

/* Access member 'PDPContextStatus' of type 'c_RAU_REQUEST' as a pointer */
#define PTR_c_RAU_REQUEST_PDPContextStatus(var) (&var)

/* DEFINITION OF BINARY c_RAU_REQUEST_MsRACap */
typedef struct _c_RAU_REQUEST_MsRACap {
	ED_BYTE value [51]; /* Variable size; bits needed 408 */
	int usedBits;
} c_RAU_REQUEST_MsRACap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_RAU_REQUEST_MsRACap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_RAU_REQUEST_MsNwkCapability */
typedef struct _c_RAU_REQUEST_MsNwkCapability {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_RAU_REQUEST_MsNwkCapability;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_RAU_REQUEST_MsNwkCapability(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_RAU_REQUEST {
	ED_BYTE OldRAI [6];
	ED_BYTE PTMSI [5];
	ED_BYTE OldPTMSISign [3];
	ED_BYTE PDPContextStatus [2];
	ED_OCTET GprsCKSN;
	ED_BOOLEAN OldPTMSISign_Present;
	ED_OCTET ReqReadyTimerVal;
	ED_BOOLEAN ReqReadyTimerVal_Present;
	ED_BOOLEAN DRXParam_Present;
	ED_OCTET TMSIStatus;
	ED_BOOLEAN TMSIStatus_Present;
	ED_BOOLEAN PTMSI_Present;
	ED_BOOLEAN MsNwkCapability_Present;
	ED_BOOLEAN PDPContextStatus_Present;
	c_RAUTypeIE UpdateType;
	c_RAU_REQUEST_MsRACap MsRACap;
	c_DRXParamIE DRXParam;
	c_RAU_REQUEST_MsNwkCapability MsNwkCapability;

}	c_RAU_REQUEST;
#define INIT_c_RAU_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_RAU_REQUEST))
#define FREE_c_RAU_REQUEST(sp)
#define SETPRESENT_c_RAU_REQUEST_OldPTMSISign(sp,present) ((sp)->OldPTMSISign_Present = present)
#define GETPRESENT_c_RAU_REQUEST_OldPTMSISign(sp) ((sp)->OldPTMSISign_Present)
#define SETPRESENT_c_RAU_REQUEST_ReqReadyTimerVal(sp,present) ((sp)->ReqReadyTimerVal_Present = present)
#define GETPRESENT_c_RAU_REQUEST_ReqReadyTimerVal(sp) ((sp)->ReqReadyTimerVal_Present)
#define SETPRESENT_c_RAU_REQUEST_DRXParam(sp,present) ((sp)->DRXParam_Present = present)
#define GETPRESENT_c_RAU_REQUEST_DRXParam(sp) ((sp)->DRXParam_Present)
#define SETPRESENT_c_RAU_REQUEST_TMSIStatus(sp,present) ((sp)->TMSIStatus_Present = present)
#define GETPRESENT_c_RAU_REQUEST_TMSIStatus(sp) ((sp)->TMSIStatus_Present)
#define SETPRESENT_c_RAU_REQUEST_PTMSI(sp,present) ((sp)->PTMSI_Present = present)
#define GETPRESENT_c_RAU_REQUEST_PTMSI(sp) ((sp)->PTMSI_Present)
#define SETPRESENT_c_RAU_REQUEST_MsNwkCapability(sp,present) ((sp)->MsNwkCapability_Present = present)
#define GETPRESENT_c_RAU_REQUEST_MsNwkCapability(sp) ((sp)->MsNwkCapability_Present)
#define SETPRESENT_c_RAU_REQUEST_PDPContextStatus(sp,present) ((sp)->PDPContextStatus_Present = present)
#define GETPRESENT_c_RAU_REQUEST_PDPContextStatus(sp) ((sp)->PDPContextStatus_Present)

/* Access member 'RxNPDUNumberList' of type 'c_RAU_COMPLETE' as a variable reference */
#define VAR_c_RAU_COMPLETE_RxNPDUNumberList(var) var

/* Access member 'RxNPDUNumberList' of type 'c_RAU_COMPLETE' as a pointer */
#define PTR_c_RAU_COMPLETE_RxNPDUNumberList(var) (&var)


/*-----------------------------------*/
typedef struct _c_RAU_COMPLETE {
	ED_BOOLEAN RxNPDUNumberList_Present;
	c_RxNpduNumList RxNPDUNumberList;

}	c_RAU_COMPLETE;
#define INIT_c_RAU_COMPLETE(sp) ED_RESET_MEM ((sp), sizeof (c_RAU_COMPLETE))
#define FREE_c_RAU_COMPLETE(sp)
#define SETPRESENT_c_RAU_COMPLETE_RxNPDUNumberList(sp,present) ((sp)->RxNPDUNumberList_Present = present)
#define GETPRESENT_c_RAU_COMPLETE_RxNPDUNumberList(sp) ((sp)->RxNPDUNumberList_Present)

/* Access member 'AuthParam' of type 'c_AUTHENTICATION_RESPONSE' as a variable reference */
#define VAR_c_AUTHENTICATION_RESPONSE_AuthParam(var) var

/* Access member 'AuthParam' of type 'c_AUTHENTICATION_RESPONSE' as a pointer */
#define PTR_c_AUTHENTICATION_RESPONSE_AuthParam(var) (&var)


/*-----------------------------------*/
typedef struct _c_AUTH_RSP_AuthParam_Ext {      
	ED_BYTE value [12]; /* Variable size; bits needed 96 */
	int usedBits;
}	c_AUTH_RSP_AuthParam_Ext;

typedef struct _c_AUTHENTICATION_RESPONSE {
	ED_BYTE AuthParam [4];
	ED_BOOLEAN AuthParam_ext_present;
	c_AUTH_RSP_AuthParam_Ext AuthParam_Ext;

}	c_AUTHENTICATION_RESPONSE;
#define INIT_c_AUTHENTICATION_RESPONSE(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_RESPONSE))
#define FREE_c_AUTHENTICATION_RESPONSE(sp)


typedef struct _c_AUTH_FAILURE_PARA {
  ED_BYTE value[14];
  int usedBits;
} c_AUTH_FAILURE_PARA;

typedef struct _c_AUTHENTICATION_FAILURE {
  ED_BYTE Cause;
  bool AuthFailure_Para_Present;
  c_AUTH_FAILURE_PARA AuthFailure_Para;
} c_AUTHENTICATION_FAILURE;
#define INIT_c_AUTHENTICATION_FAILURE(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_FAILURE))
#define FREE_c_AUTHENTICATION_FAILURE(sp)


/* Access member 'CKSN' of type 'c_CM_REESTABLISHMENT_REQUEST' as a variable reference */
#define VAR_c_CM_REESTABLISHMENT_REQUEST_CKSN(var) var

/* Access member 'CKSN' of type 'c_CM_REESTABLISHMENT_REQUEST' as a pointer */
#define PTR_c_CM_REESTABLISHMENT_REQUEST_CKSN(var) (&var)

/* Access member 'Spare' of type 'c_CM_REESTABLISHMENT_REQUEST' as a variable reference */
#define VAR_c_CM_REESTABLISHMENT_REQUEST_Spare(var) var

/* Access member 'Spare' of type 'c_CM_REESTABLISHMENT_REQUEST' as a pointer */
#define PTR_c_CM_REESTABLISHMENT_REQUEST_Spare(var) (&var)

/* Access member 'MsClassmark2' of type 'c_CM_REESTABLISHMENT_REQUEST' as a variable reference */
#define VAR_c_CM_REESTABLISHMENT_REQUEST_MsClassmark2(var) var

/* Access member 'MsClassmark2' of type 'c_CM_REESTABLISHMENT_REQUEST' as a pointer */
#define PTR_c_CM_REESTABLISHMENT_REQUEST_MsClassmark2(var) (&var)

/* Access member 'MobileId' of type 'c_CM_REESTABLISHMENT_REQUEST' as a variable reference */
#define VAR_c_CM_REESTABLISHMENT_REQUEST_MobileId(var) var

/* Access member 'MobileId' of type 'c_CM_REESTABLISHMENT_REQUEST' as a pointer */
#define PTR_c_CM_REESTABLISHMENT_REQUEST_MobileId(var) (&var)

/* Access member 'LAI' of type 'c_CM_REESTABLISHMENT_REQUEST' as a variable reference */
#define VAR_c_CM_REESTABLISHMENT_REQUEST_LAI(var) var

/* Access member 'LAI' of type 'c_CM_REESTABLISHMENT_REQUEST' as a pointer */
#define PTR_c_CM_REESTABLISHMENT_REQUEST_LAI(var) (&var)

/* DEFINITION OF BINARY c_CM_REESTABLISHMENT_REQUEST_MobileId */
typedef struct _c_CM_REESTABLISHMENT_REQUEST_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_CM_REESTABLISHMENT_REQUEST_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_CM_REESTABLISHMENT_REQUEST_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_CM_REESTABLISHMENT_REQUEST {
	ED_BYTE LAI [5];
	ED_BYTE MsClassmark2 [3];
	ED_OCTET CKSN;
	ED_BOOLEAN LAI_Present;
	c_CM_REESTABLISHMENT_REQUEST_MobileId MobileId;

}	c_CM_REESTABLISHMENT_REQUEST;
#define INIT_c_CM_REESTABLISHMENT_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_CM_REESTABLISHMENT_REQUEST))
#define FREE_c_CM_REESTABLISHMENT_REQUEST(sp)
#define SETPRESENT_c_CM_REESTABLISHMENT_REQUEST_LAI(sp,present) ((sp)->LAI_Present = present)
#define GETPRESENT_c_CM_REESTABLISHMENT_REQUEST_LAI(sp) ((sp)->LAI_Present)


/*-----------------------------------*/
typedef struct _c_CM_SERVICE_ABORT {
	int __Dummy_Field__;

}	c_CM_SERVICE_ABORT;
#define INIT_c_CM_SERVICE_ABORT(sp) ED_RESET_MEM ((sp), sizeof (c_CM_SERVICE_ABORT))
#define FREE_c_CM_SERVICE_ABORT(sp)

/* Access member 'ServiceType' of type 'c_CM_SERVICE_REQUEST' as a variable reference */
#define VAR_c_CM_SERVICE_REQUEST_ServiceType(var) var

/* Access member 'ServiceType' of type 'c_CM_SERVICE_REQUEST' as a pointer */
#define PTR_c_CM_SERVICE_REQUEST_ServiceType(var) (&var)

/* Access member 'CKSN' of type 'c_CM_SERVICE_REQUEST' as a variable reference */
#define VAR_c_CM_SERVICE_REQUEST_CKSN(var) var

/* Access member 'CKSN' of type 'c_CM_SERVICE_REQUEST' as a pointer */
#define PTR_c_CM_SERVICE_REQUEST_CKSN(var) (&var)

/* Access member 'MsClassmark2' of type 'c_CM_SERVICE_REQUEST' as a variable reference */
#define VAR_c_CM_SERVICE_REQUEST_MsClassmark2(var) var

/* Access member 'MsClassmark2' of type 'c_CM_SERVICE_REQUEST' as a pointer */
#define PTR_c_CM_SERVICE_REQUEST_MsClassmark2(var) (&var)

/* Access member 'MobileId' of type 'c_CM_SERVICE_REQUEST' as a variable reference */
#define VAR_c_CM_SERVICE_REQUEST_MobileId(var) var

/* Access member 'MobileId' of type 'c_CM_SERVICE_REQUEST' as a pointer */
#define PTR_c_CM_SERVICE_REQUEST_MobileId(var) (&var)

/* DEFINITION OF BINARY c_CM_SERVICE_REQUEST_MobileId */
typedef struct _c_CM_SERVICE_REQUEST_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_CM_SERVICE_REQUEST_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_CM_SERVICE_REQUEST_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_CM_SERVICE_REQUEST {
	ED_BYTE MsClassmark2 [3];
	ED_OCTET ServiceType;
	ED_OCTET CKSN;
	c_CM_SERVICE_REQUEST_MobileId MobileId;

}	c_CM_SERVICE_REQUEST;
#define INIT_c_CM_SERVICE_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_CM_SERVICE_REQUEST))
#define FREE_c_CM_SERVICE_REQUEST(sp)

/* Access member 'MobileId' of type 'c_IDENTITY_RESPONSE' as a variable reference */
#define VAR_c_IDENTITY_RESPONSE_MobileId(var) var

/* Access member 'MobileId' of type 'c_IDENTITY_RESPONSE' as a pointer */
#define PTR_c_IDENTITY_RESPONSE_MobileId(var) (&var)

/* DEFINITION OF BINARY c_IDENTITY_RESPONSE_MobileId */
typedef struct _c_IDENTITY_RESPONSE_MobileId {
	ED_BYTE value [9]; /* Variable size; bits needed 72 */
	int usedBits;
} c_IDENTITY_RESPONSE_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IDENTITY_RESPONSE_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_IDENTITY_RESPONSE {
	c_IDENTITY_RESPONSE_MobileId MobileId;

}	c_IDENTITY_RESPONSE;
#define INIT_c_IDENTITY_RESPONSE(sp) ED_RESET_MEM ((sp), sizeof (c_IDENTITY_RESPONSE))
#define FREE_c_IDENTITY_RESPONSE(sp)

/* Access member 'MsClassmark1' of type 'c_IMSI_DETACH_INDICATION' as a variable reference */
#define VAR_c_IMSI_DETACH_INDICATION_MsClassmark1(var) var

/* Access member 'MsClassmark1' of type 'c_IMSI_DETACH_INDICATION' as a pointer */
#define PTR_c_IMSI_DETACH_INDICATION_MsClassmark1(var) (&var)

/* Access member 'MobileId' of type 'c_IMSI_DETACH_INDICATION' as a variable reference */
#define VAR_c_IMSI_DETACH_INDICATION_MobileId(var) var

/* Access member 'MobileId' of type 'c_IMSI_DETACH_INDICATION' as a pointer */
#define PTR_c_IMSI_DETACH_INDICATION_MobileId(var) (&var)

/* DEFINITION OF BINARY c_IMSI_DETACH_INDICATION_MobileId */
typedef struct _c_IMSI_DETACH_INDICATION_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_IMSI_DETACH_INDICATION_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IMSI_DETACH_INDICATION_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_IMSI_DETACH_INDICATION {
	ED_OCTET MsClassmark1;
	c_IMSI_DETACH_INDICATION_MobileId MobileId;

}	c_IMSI_DETACH_INDICATION;
#define INIT_c_IMSI_DETACH_INDICATION(sp) ED_RESET_MEM ((sp), sizeof (c_IMSI_DETACH_INDICATION))
#define FREE_c_IMSI_DETACH_INDICATION(sp)

/* Access member 'LocUpRequestType' of type 'c_LOCATION_UPDATE_REQUEST' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REQUEST_LocUpRequestType(var) var

/* Access member 'LocUpRequestType' of type 'c_LOCATION_UPDATE_REQUEST' as a pointer */
#define PTR_c_LOCATION_UPDATE_REQUEST_LocUpRequestType(var) (&var)

/* Access member 'CKSN' of type 'c_LOCATION_UPDATE_REQUEST' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REQUEST_CKSN(var) var

/* Access member 'CKSN' of type 'c_LOCATION_UPDATE_REQUEST' as a pointer */
#define PTR_c_LOCATION_UPDATE_REQUEST_CKSN(var) (&var)

/* Access member 'LAI' of type 'c_LOCATION_UPDATE_REQUEST' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REQUEST_LAI(var) var

/* Access member 'LAI' of type 'c_LOCATION_UPDATE_REQUEST' as a pointer */
#define PTR_c_LOCATION_UPDATE_REQUEST_LAI(var) (&var)

/* Access member 'MsClassmark1' of type 'c_LOCATION_UPDATE_REQUEST' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REQUEST_MsClassmark1(var) var

/* Access member 'MsClassmark1' of type 'c_LOCATION_UPDATE_REQUEST' as a pointer */
#define PTR_c_LOCATION_UPDATE_REQUEST_MsClassmark1(var) (&var)

/* Access member 'MobileId' of type 'c_LOCATION_UPDATE_REQUEST' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REQUEST_MobileId(var) var

/* Access member 'MobileId' of type 'c_LOCATION_UPDATE_REQUEST' as a pointer */
#define PTR_c_LOCATION_UPDATE_REQUEST_MobileId(var) (&var)

/* DEFINITION OF BINARY c_LOCATION_UPDATE_REQUEST_MobileId */
typedef struct _c_LOCATION_UPDATE_REQUEST_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_LOCATION_UPDATE_REQUEST_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_LOCATION_UPDATE_REQUEST_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_LOCATION_UPDATE_REQUEST {
	ED_BYTE LAI [5];
	ED_OCTET CKSN;
	ED_OCTET MsClassmark1;
	c_LocUpReqType LocUpRequestType;
	c_LOCATION_UPDATE_REQUEST_MobileId MobileId;

}	c_LOCATION_UPDATE_REQUEST;
#define INIT_c_LOCATION_UPDATE_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_LOCATION_UPDATE_REQUEST))
#define FREE_c_LOCATION_UPDATE_REQUEST(sp)


/*-----------------------------------*/
typedef struct _c_TMSI_REALLOCATION_COMPLETE {
	int __Dummy_Field__;

}	c_TMSI_REALLOCATION_COMPLETE;
#define INIT_c_TMSI_REALLOCATION_COMPLETE(sp) ED_RESET_MEM ((sp), sizeof (c_TMSI_REALLOCATION_COMPLETE))
#define FREE_c_TMSI_REALLOCATION_COMPLETE(sp)

/* Access member 'CKSN' of type 'c_PAGING_RESPONSE' as a variable reference */
#define VAR_c_PAGING_RESPONSE_CKSN(var) var

/* Access member 'CKSN' of type 'c_PAGING_RESPONSE' as a pointer */
#define PTR_c_PAGING_RESPONSE_CKSN(var) (&var)

/* Access member 'Spare' of type 'c_PAGING_RESPONSE' as a variable reference */
#define VAR_c_PAGING_RESPONSE_Spare(var) var

/* Access member 'Spare' of type 'c_PAGING_RESPONSE' as a pointer */
#define PTR_c_PAGING_RESPONSE_Spare(var) (&var)

/* Access member 'MsClassmark2' of type 'c_PAGING_RESPONSE' as a variable reference */
#define VAR_c_PAGING_RESPONSE_MsClassmark2(var) var

/* Access member 'MsClassmark2' of type 'c_PAGING_RESPONSE' as a pointer */
#define PTR_c_PAGING_RESPONSE_MsClassmark2(var) (&var)

/* Access member 'MobileId' of type 'c_PAGING_RESPONSE' as a variable reference */
#define VAR_c_PAGING_RESPONSE_MobileId(var) var

/* Access member 'MobileId' of type 'c_PAGING_RESPONSE' as a pointer */
#define PTR_c_PAGING_RESPONSE_MobileId(var) (&var)

/* DEFINITION OF BINARY c_PAGING_RESPONSE_MobileId */
typedef struct _c_PAGING_RESPONSE_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PAGING_RESPONSE_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PAGING_RESPONSE_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_PAGING_RESPONSE {
	ED_BYTE MsClassmark2 [3];
	ED_OCTET CKSN;
	c_PAGING_RESPONSE_MobileId MobileId;

}	c_PAGING_RESPONSE;
#define INIT_c_PAGING_RESPONSE(sp) ED_RESET_MEM ((sp), sizeof (c_PAGING_RESPONSE))
#define FREE_c_PAGING_RESPONSE(sp)

/* Access member 'AttachResult' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_AttachResult(var) var

/* Access member 'AttachResult' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_AttachResult(var) (&var)

/* Access member 'ForceToStdby' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_ForceToStdby(var) (&var)

/* Access member 'RAUpdTimer' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_RAUpdTimer(var) var

/* Access member 'RAUpdTimer' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_RAUpdTimer(var) (&var)

/* Access member 'RadPrioSMS' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_RadPrioSMS(var) var

/* Access member 'RadPrioSMS' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_RadPrioSMS(var) (&var)

/* Access member 'Spare' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_Spare(var) var

/* Access member 'Spare' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_Spare(var) (&var)

/* Access member 'RAI' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_RAI(var) var

/* Access member 'RAI' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_RAI(var) (&var)

/* Access member 'PTMSISign' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_PTMSISign(var) var

/* Access member 'PTMSISign' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_PTMSISign(var) (&var)

/* Access member 'NegReadyTimerVal' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_NegReadyTimerVal(var) var

/* Access member 'NegReadyTimerVal' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_NegReadyTimerVal(var) (&var)

/* Access member 'AllocPTMSI' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_AllocPTMSI(var) var

/* Access member 'AllocPTMSI' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_AllocPTMSI(var) (&var)

/* Access member 'MSId' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_MSId(var) var

/* Access member 'MSId' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_MSId(var) (&var)

/* Access member 'GMMCause' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_GMMCause(var) (&var)

/* Access member 'T3302Val' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_T3302Val(var) var

/* Access member 'T3302Val' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_T3302Val(var) (&var)

/* Access member 'CellNotif' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_CellNotif(var) var

/* Access member 'CellNotif' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_CellNotif(var) (&var)

/* Access member 'EquivalentPLMN' of type 'c_ATTACH_ACCEPT' as a variable reference */
#define VAR_c_ATTACH_ACCEPT_EquivalentPLMN(var) var

/* Access member 'EquivalentPLMN' of type 'c_ATTACH_ACCEPT' as a pointer */
#define PTR_c_ATTACH_ACCEPT_EquivalentPLMN(var) (&var)

/* DEFINITION OF BINARY c_ATTACH_ACCEPT_MSId */
typedef struct _c_ATTACH_ACCEPT_MSId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ATTACH_ACCEPT_MSId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ATTACH_ACCEPT_MSId(sp,bits) (sp)->usedBits = 0

// for R99 15, for R6 45, test in IDEA network 201609
/* DEFINITION OF BINARY c_ATTACH_ACCEPT_EquivalentPLMN */
typedef struct _c_ATTACH_ACCEPT_EquivalentPLMN {
	ED_BYTE value [45]; /* Variable size; bits needed 120 */
	int usedBits;
} c_ATTACH_ACCEPT_EquivalentPLMN;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ATTACH_ACCEPT_EquivalentPLMN(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_ATTACH_ACCEPT {
	ED_BYTE RAI [6];
	ED_BYTE AllocPTMSI [5];
	ED_BYTE PTMSISign [3];
	ED_OCTET AttachResult;
	ED_OCTET ForceToStdby;
	ED_OCTET RadPrioSMS;
	ED_BOOLEAN PTMSISign_Present;
	ED_BOOLEAN NegReadyTimerVal_Present;
	ED_BOOLEAN AllocPTMSI_Present;
	ED_BOOLEAN MSId_Present;
	ED_OCTET GMMCause;
	ED_BOOLEAN GMMCause_Present;
	ED_BOOLEAN T3302Val_Present;
	ED_BOOLEAN CellNotif_Present;
	ED_BOOLEAN EquivalentPLMN_Present;
	c_GPRSTimerIE RAUpdTimer;
	c_GPRSTimerIE NegReadyTimerVal;
	c_ATTACH_ACCEPT_MSId MSId;
	c_GPRSTimerIE T3302Val;
	c_ATTACH_ACCEPT_EquivalentPLMN EquivalentPLMN;

}	c_ATTACH_ACCEPT;
#define INIT_c_ATTACH_ACCEPT(sp) ED_RESET_MEM ((sp), sizeof (c_ATTACH_ACCEPT))
#define FREE_c_ATTACH_ACCEPT(sp)
#define SETPRESENT_c_ATTACH_ACCEPT_PTMSISign(sp,present) ((sp)->PTMSISign_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_PTMSISign(sp) ((sp)->PTMSISign_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_NegReadyTimerVal(sp,present) ((sp)->NegReadyTimerVal_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_NegReadyTimerVal(sp) ((sp)->NegReadyTimerVal_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_AllocPTMSI(sp,present) ((sp)->AllocPTMSI_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_AllocPTMSI(sp) ((sp)->AllocPTMSI_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_MSId(sp,present) ((sp)->MSId_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_MSId(sp) ((sp)->MSId_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_GMMCause(sp,present) ((sp)->GMMCause_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_GMMCause(sp) ((sp)->GMMCause_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_T3302Val(sp,present) ((sp)->T3302Val_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_T3302Val(sp) ((sp)->T3302Val_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_CellNotif(sp,present) ((sp)->CellNotif_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_CellNotif(sp) ((sp)->CellNotif_Present)
#define SETPRESENT_c_ATTACH_ACCEPT_EquivalentPLMN(sp,present) ((sp)->EquivalentPLMN_Present = present)
#define GETPRESENT_c_ATTACH_ACCEPT_EquivalentPLMN(sp) ((sp)->EquivalentPLMN_Present)

/* Access member 'GMMCause' of type 'c_ATTACH_REJECT' as a variable reference */
#define VAR_c_ATTACH_REJECT_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_ATTACH_REJECT' as a pointer */
#define PTR_c_ATTACH_REJECT_GMMCause(var) (&var)

/* Access member 'T3302Val' of type 'c_ATTACH_REJECT' as a variable reference */
#define VAR_c_ATTACH_REJECT_T3302Val(var) var

/* Access member 'T3302Val' of type 'c_ATTACH_REJECT' as a pointer */
#define PTR_c_ATTACH_REJECT_T3302Val(var) (&var)


/*-----------------------------------*/
typedef struct _c_ATTACH_REJECT {
	ED_OCTET GMMCause;
	ED_BOOLEAN T3302Val_Present;
	c_GPRSTimerIE T3302Val;

}	c_ATTACH_REJECT;
#define INIT_c_ATTACH_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_ATTACH_REJECT))
#define FREE_c_ATTACH_REJECT(sp)
#define SETPRESENT_c_ATTACH_REJECT_T3302Val(sp,present) ((sp)->T3302Val_Present = present)
#define GETPRESENT_c_ATTACH_REJECT_T3302Val(sp) ((sp)->T3302Val_Present)

/* Access member 'DetachType' of type 'c_DETACH_REQUEST_MT' as a variable reference */
#define VAR_c_DETACH_REQUEST_MT_DetachType(var) var

/* Access member 'DetachType' of type 'c_DETACH_REQUEST_MT' as a pointer */
#define PTR_c_DETACH_REQUEST_MT_DetachType(var) (&var)

/* Access member 'ForceToStdby' of type 'c_DETACH_REQUEST_MT' as a variable reference */
#define VAR_c_DETACH_REQUEST_MT_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_DETACH_REQUEST_MT' as a pointer */
#define PTR_c_DETACH_REQUEST_MT_ForceToStdby(var) (&var)

/* Access member 'GMMCause' of type 'c_DETACH_REQUEST_MT' as a variable reference */
#define VAR_c_DETACH_REQUEST_MT_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_DETACH_REQUEST_MT' as a pointer */
#define PTR_c_DETACH_REQUEST_MT_GMMCause(var) (&var)


/*-----------------------------------*/
typedef struct _c_DETACH_REQUEST_MT {
	ED_OCTET DetachType;
	ED_OCTET ForceToStdby;
	ED_OCTET GMMCause;
	ED_BOOLEAN GMMCause_Present;

}	c_DETACH_REQUEST_MT;
#define INIT_c_DETACH_REQUEST_MT(sp) ED_RESET_MEM ((sp), sizeof (c_DETACH_REQUEST_MT))
#define FREE_c_DETACH_REQUEST_MT(sp)
#define SETPRESENT_c_DETACH_REQUEST_MT_GMMCause(sp,present) ((sp)->GMMCause_Present = present)
#define GETPRESENT_c_DETACH_REQUEST_MT_GMMCause(sp) ((sp)->GMMCause_Present)

/* Access member 'ForceToStdby' of type 'c_DETACH_ACCEPT' as a variable reference */
#define VAR_c_DETACH_ACCEPT_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_DETACH_ACCEPT' as a pointer */
#define PTR_c_DETACH_ACCEPT_ForceToStdby(var) (&var)

/* Access member 'Spare' of type 'c_DETACH_ACCEPT' as a variable reference */
#define VAR_c_DETACH_ACCEPT_Spare(var) var

/* Access member 'Spare' of type 'c_DETACH_ACCEPT' as a pointer */
#define PTR_c_DETACH_ACCEPT_Spare(var) (&var)


/*-----------------------------------*/
typedef struct _c_DETACH_ACCEPT {
	ED_OCTET ForceToStdby;

}	c_DETACH_ACCEPT;
#define INIT_c_DETACH_ACCEPT(sp) ED_RESET_MEM ((sp), sizeof (c_DETACH_ACCEPT))
#define FREE_c_DETACH_ACCEPT(sp)

/* Access member 'AllocPTMSI' of type 'c_PTMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_PTMSI_REALLOCATION_COMMAND_AllocPTMSI(var) var

/* Access member 'AllocPTMSI' of type 'c_PTMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_PTMSI_REALLOCATION_COMMAND_AllocPTMSI(var) (&var)

/* Access member 'RAI' of type 'c_PTMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_PTMSI_REALLOCATION_COMMAND_RAI(var) var

/* Access member 'RAI' of type 'c_PTMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_PTMSI_REALLOCATION_COMMAND_RAI(var) (&var)

/* Access member 'ForceToStdby' of type 'c_PTMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_PTMSI_REALLOCATION_COMMAND_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_PTMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_PTMSI_REALLOCATION_COMMAND_ForceToStdby(var) (&var)

/* Access member 'Spare' of type 'c_PTMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_PTMSI_REALLOCATION_COMMAND_Spare(var) var

/* Access member 'Spare' of type 'c_PTMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_PTMSI_REALLOCATION_COMMAND_Spare(var) (&var)

/* Access member 'PTMSISign' of type 'c_PTMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_PTMSI_REALLOCATION_COMMAND_PTMSISign(var) var

/* Access member 'PTMSISign' of type 'c_PTMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_PTMSI_REALLOCATION_COMMAND_PTMSISign(var) (&var)


/*-----------------------------------*/
typedef struct _c_PTMSI_REALLOCATION_COMMAND {
	ED_BYTE RAI [6];
	ED_BYTE AllocPTMSI [5];
	ED_BYTE PTMSISign [3];
	ED_OCTET ForceToStdby;
	ED_BOOLEAN PTMSISign_Present;

}	c_PTMSI_REALLOCATION_COMMAND;
#define INIT_c_PTMSI_REALLOCATION_COMMAND(sp) ED_RESET_MEM ((sp), sizeof (c_PTMSI_REALLOCATION_COMMAND))
#define FREE_c_PTMSI_REALLOCATION_COMMAND(sp)
#define SETPRESENT_c_PTMSI_REALLOCATION_COMMAND_PTMSISign(sp,present) ((sp)->PTMSISign_Present = present)
#define GETPRESENT_c_PTMSI_REALLOCATION_COMMAND_PTMSISign(sp) ((sp)->PTMSISign_Present)

/* Access member 'CipheringAlgorithm' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_CipheringAlgorithm(var) var

/* Access member 'CipheringAlgorithm' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_CipheringAlgorithm(var) (&var)

/* Access member 'IMEISVRequest' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_IMEISVRequest(var) var

/* Access member 'IMEISVRequest' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_IMEISVRequest(var) (&var)

/* Access member 'ForceToStdby' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_ForceToStdby(var) (&var)

/* Access member 'ACReferenceNumber' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_ACReferenceNumber(var) var

/* Access member 'ACReferenceNumber' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_ACReferenceNumber(var) (&var)

/* Access member 'AuthenticationParameterRAND' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterRAND(var) var

/* Access member 'AuthenticationParameterRAND' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterRAND(var) (&var)

/* Access member 'GPRSCipheringKeySequence' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_GPRSCipheringKeySequence(var) var

/* Access member 'GPRSCipheringKeySequence' of type 'c_AUTHENTICATION_AND_CIPHERING_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_AND_CIPHERING_REQUEST_GPRSCipheringKeySequence(var) (&var)


/*-----------------------------------*/
typedef struct _c_AUTHENTICATION_AND_CIPHERING_REQUEST {
	ED_BYTE AuthenticationParameterRAND [16];
	ED_BYTE AuthenticationParameterAUTN[16];
	ED_OCTET ForceToStdby;
	ED_OCTET ACReferenceNumber;
	ED_BOOLEAN AuthenticationParameterRAND_Present;
	ED_BOOLEAN AuthenticationParameterAUTN_Present;
	ED_OCTET GPRSCipheringKeySequence;
	ED_BOOLEAN GPRSCipheringKeySequence_Present;
	c_CiphAlgoIE CipheringAlgorithm;
	c_IMEISVRequest IMEISVRequest;

}	c_AUTHENTICATION_AND_CIPHERING_REQUEST;
#define INIT_c_AUTHENTICATION_AND_CIPHERING_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_AND_CIPHERING_REQUEST))
#define FREE_c_AUTHENTICATION_AND_CIPHERING_REQUEST(sp)
#define SETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterRAND(sp,present) ((sp)->AuthenticationParameterRAND_Present = present)
#define GETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterRAND(sp) ((sp)->AuthenticationParameterRAND_Present)
#define SETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_GPRSCipheringKeySequence(sp,present) ((sp)->GPRSCipheringKeySequence_Present = present)
#define GETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_GPRSCipheringKeySequence(sp) ((sp)->GPRSCipheringKeySequence_Present)
#define SETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterAUTN(sp,present) ((sp)->AuthenticationParameterAUTN_Present = present)
#define GETPRESENT_c_AUTHENTICATION_AND_CIPHERING_REQUEST_AuthenticationParameterAUTN(sp) ((sp)->AuthenticationParameterAUTN_Present)


/*-----------------------------------*/
typedef struct _c_AUTHENTICATION_AND_CIPHERING_REJECT {
	int __Dummy_Field__;

}	c_AUTHENTICATION_AND_CIPHERING_REJECT;
#define INIT_c_AUTHENTICATION_AND_CIPHERING_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_AND_CIPHERING_REJECT))
#define FREE_c_AUTHENTICATION_AND_CIPHERING_REJECT(sp)

/* Access member 'IdType' of type 'c_PIDENTITY_REQUEST' as a variable reference */
#define VAR_c_PIDENTITY_REQUEST_IdType(var) var

/* Access member 'IdType' of type 'c_PIDENTITY_REQUEST' as a pointer */
#define PTR_c_PIDENTITY_REQUEST_IdType(var) (&var)

/* Access member 'ForceToStdby' of type 'c_PIDENTITY_REQUEST' as a variable reference */
#define VAR_c_PIDENTITY_REQUEST_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_PIDENTITY_REQUEST' as a pointer */
#define PTR_c_PIDENTITY_REQUEST_ForceToStdby(var) (&var)


/*-----------------------------------*/
typedef struct _c_PIDENTITY_REQUEST {
	ED_OCTET IdType;
	ED_OCTET ForceToStdby;

}	c_PIDENTITY_REQUEST;
#define INIT_c_PIDENTITY_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_PIDENTITY_REQUEST))
#define FREE_c_PIDENTITY_REQUEST(sp)

/* Access member 'ForceToStdby' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_ForceToStdby(var) (&var)

/* Access member 'UpdateResult' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_UpdateResult(var) var

/* Access member 'UpdateResult' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_UpdateResult(var) (&var)

/* Access member 'RAUpdTimer' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_RAUpdTimer(var) var

/* Access member 'RAUpdTimer' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_RAUpdTimer(var) (&var)

/* Access member 'RAI' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_RAI(var) var

/* Access member 'RAI' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_RAI(var) (&var)

/* Access member 'PTMSISign' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_PTMSISign(var) var

/* Access member 'PTMSISign' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_PTMSISign(var) (&var)

/* Access member 'AllocPTMSI' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_AllocPTMSI(var) var

/* Access member 'AllocPTMSI' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_AllocPTMSI(var) (&var)

/* Access member 'MSId' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_MSId(var) var

/* Access member 'MSId' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_MSId(var) (&var)

/* Access member 'RxNPDUNumberList' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_RxNPDUNumberList(var) var

/* Access member 'RxNPDUNumberList' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_RxNPDUNumberList(var) (&var)

/* Access member 'NegReadyTimerVal' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_NegReadyTimerVal(var) var

/* Access member 'NegReadyTimerVal' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_NegReadyTimerVal(var) (&var)

/* Access member 'GMMCause' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_GMMCause(var) (&var)

/* Access member 'T3302Val' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_T3302Val(var) var

/* Access member 'T3302Val' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_T3302Val(var) (&var)

/* Access member 'CellNotif' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_CellNotif(var) var

/* Access member 'CellNotif' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_CellNotif(var) (&var)

/* Access member 'EquivalentPLMN' of type 'c_RAU_ACCEPT' as a variable reference */
#define VAR_c_RAU_ACCEPT_EquivalentPLMN(var) var

/* Access member 'EquivalentPLMN' of type 'c_RAU_ACCEPT' as a pointer */
#define PTR_c_RAU_ACCEPT_EquivalentPLMN(var) (&var)

/* DEFINITION OF BINARY c_RAU_ACCEPT_MSId */
typedef struct _c_RAU_ACCEPT_MSId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_RAU_ACCEPT_MSId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_RAU_ACCEPT_MSId(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_RAU_ACCEPT_EquivalentPLMN */
typedef struct _c_RAU_ACCEPT_EquivalentPLMN {
    // for R99 15, for R6 45, test in IDEA network 201609
	ED_BYTE value [45]; /* Variable size; bits needed 120 */
	int usedBits;
} c_RAU_ACCEPT_EquivalentPLMN;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_RAU_ACCEPT_EquivalentPLMN(sp,bits) (sp)->usedBits = 0

// DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
typedef struct _c_RAU_ACCEPT_PDPContextStatus {
	ED_BYTE value [2];
	int usedBits;
} c_RAU_ACCEPT_PDPContextStatus;


/*-----------------------------------*/
typedef struct _c_RAU_ACCEPT {
	ED_BYTE RAI [6];
	ED_BYTE AllocPTMSI [5];
	ED_BYTE PTMSISign [3];
	ED_OCTET ForceToStdby;
	ED_OCTET UpdateResult;
	ED_BOOLEAN PTMSISign_Present;
	ED_BOOLEAN AllocPTMSI_Present;
	ED_BOOLEAN MSId_Present;
	ED_BOOLEAN RxNPDUNumberList_Present;
	ED_BOOLEAN NegReadyTimerVal_Present;
	ED_OCTET GMMCause;
	ED_BOOLEAN GMMCause_Present;
	ED_BOOLEAN T3302Val_Present;
	ED_BOOLEAN CellNotif_Present;
	ED_BOOLEAN EquivalentPLMN_Present;
	ED_BOOLEAN PDPContextStatus_Present;  // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
	c_GPRSTimerIE RAUpdTimer;
	c_RAU_ACCEPT_MSId MSId;
	c_RxNpduNumList RxNPDUNumberList;
	c_GPRSTimerIE NegReadyTimerVal;
	c_GPRSTimerIE T3302Val;
	c_RAU_ACCEPT_EquivalentPLMN EquivalentPLMN;
	c_RAU_ACCEPT_PDPContextStatus PDPContextStatus;  // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
 
}	c_RAU_ACCEPT;
#define INIT_c_RAU_ACCEPT(sp) ED_RESET_MEM ((sp), sizeof (c_RAU_ACCEPT))
#define FREE_c_RAU_ACCEPT(sp)
#define SETPRESENT_c_RAU_ACCEPT_PTMSISign(sp,present) ((sp)->PTMSISign_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_PTMSISign(sp) ((sp)->PTMSISign_Present)
#define SETPRESENT_c_RAU_ACCEPT_AllocPTMSI(sp,present) ((sp)->AllocPTMSI_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_AllocPTMSI(sp) ((sp)->AllocPTMSI_Present)
#define SETPRESENT_c_RAU_ACCEPT_MSId(sp,present) ((sp)->MSId_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_MSId(sp) ((sp)->MSId_Present)
#define SETPRESENT_c_RAU_ACCEPT_RxNPDUNumberList(sp,present) ((sp)->RxNPDUNumberList_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_RxNPDUNumberList(sp) ((sp)->RxNPDUNumberList_Present)
#define SETPRESENT_c_RAU_ACCEPT_NegReadyTimerVal(sp,present) ((sp)->NegReadyTimerVal_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_NegReadyTimerVal(sp) ((sp)->NegReadyTimerVal_Present)
#define SETPRESENT_c_RAU_ACCEPT_GMMCause(sp,present) ((sp)->GMMCause_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_GMMCause(sp) ((sp)->GMMCause_Present)
#define SETPRESENT_c_RAU_ACCEPT_T3302Val(sp,present) ((sp)->T3302Val_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_T3302Val(sp) ((sp)->T3302Val_Present)
#define SETPRESENT_c_RAU_ACCEPT_CellNotif(sp,present) ((sp)->CellNotif_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_CellNotif(sp) ((sp)->CellNotif_Present)
#define SETPRESENT_c_RAU_ACCEPT_EquivalentPLMN(sp,present) ((sp)->EquivalentPLMN_Present = present)
#define GETPRESENT_c_RAU_ACCEPT_EquivalentPLMN(sp) ((sp)->EquivalentPLMN_Present)

/* Access member 'GMMCause' of type 'c_RAU_REJECT' as a variable reference */
#define VAR_c_RAU_REJECT_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_RAU_REJECT' as a pointer */
#define PTR_c_RAU_REJECT_GMMCause(var) (&var)

/* Access member 'ForceToStdby' of type 'c_RAU_REJECT' as a variable reference */
#define VAR_c_RAU_REJECT_ForceToStdby(var) var

/* Access member 'ForceToStdby' of type 'c_RAU_REJECT' as a pointer */
#define PTR_c_RAU_REJECT_ForceToStdby(var) (&var)

/* Access member 'Spare' of type 'c_RAU_REJECT' as a variable reference */
#define VAR_c_RAU_REJECT_Spare(var) var

/* Access member 'Spare' of type 'c_RAU_REJECT' as a pointer */
#define PTR_c_RAU_REJECT_Spare(var) (&var)

/* Access member 'T3302Val' of type 'c_RAU_REJECT' as a variable reference */
#define VAR_c_RAU_REJECT_T3302Val(var) var

/* Access member 'T3302Val' of type 'c_RAU_REJECT' as a pointer */
#define PTR_c_RAU_REJECT_T3302Val(var) (&var)


/*-----------------------------------*/
typedef struct _c_RAU_REJECT {
	ED_OCTET GMMCause;
	ED_OCTET ForceToStdby;
	ED_BOOLEAN T3302Val_Present;
	c_GPRSTimerIE T3302Val;

}	c_RAU_REJECT;
#define INIT_c_RAU_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_RAU_REJECT))
#define FREE_c_RAU_REJECT(sp)
#define SETPRESENT_c_RAU_REJECT_T3302Val(sp,present) ((sp)->T3302Val_Present = present)
#define GETPRESENT_c_RAU_REJECT_T3302Val(sp) ((sp)->T3302Val_Present)

/* Access member 'GMMCause' of type 'c_PSTATUS' as a variable reference */
#define VAR_c_PSTATUS_GMMCause(var) var

/* Access member 'GMMCause' of type 'c_PSTATUS' as a pointer */
#define PTR_c_PSTATUS_GMMCause(var) (&var)


/*-----------------------------------*/
typedef struct _c_PSTATUS {
	ED_OCTET GMMCause;

}	c_PSTATUS;
#define INIT_c_PSTATUS(sp) ED_RESET_MEM ((sp), sizeof (c_PSTATUS))
#define FREE_c_PSTATUS(sp)

/* Access member 'InformationIE' of type 'c_GMM_INFORMATION' as a variable reference */
#define VAR_c_GMM_INFORMATION_InformationIE(var) var

/* Access member 'InformationIE' of type 'c_GMM_INFORMATION' as a pointer */
#define PTR_c_GMM_INFORMATION_InformationIE(var) (&var)


/*-----------------------------------*/
typedef struct _c_GMM_INFORMATION {
	c_InformationIE_t InformationIE;

}	c_GMM_INFORMATION;
#define INIT_c_GMM_INFORMATION(sp) ED_RESET_MEM ((sp), sizeof (c_GMM_INFORMATION))
#define FREE_c_GMM_INFORMATION(sp)

/* Access member 'PduDesc' of type 'c_GPRS_TEST_MODE_CMD' as a variable reference */
#define VAR_c_GPRS_TEST_MODE_CMD_PduDesc(var) var

/* Access member 'PduDesc' of type 'c_GPRS_TEST_MODE_CMD' as a pointer */
#define PTR_c_GPRS_TEST_MODE_CMD_PduDesc(var) (&var)

/* Access member 'ModeFlag' of type 'c_GPRS_TEST_MODE_CMD' as a variable reference */
#define VAR_c_GPRS_TEST_MODE_CMD_ModeFlag(var) var

/* Access member 'ModeFlag' of type 'c_GPRS_TEST_MODE_CMD' as a pointer */
#define PTR_c_GPRS_TEST_MODE_CMD_ModeFlag(var) (&var)


/*-----------------------------------*/
typedef struct _c_GPRS_TEST_MODE_CMD {
	ED_BYTE PduDesc [2];
	ED_OCTET ModeFlag;

}	c_GPRS_TEST_MODE_CMD;
#define INIT_c_GPRS_TEST_MODE_CMD(sp) ED_RESET_MEM ((sp), sizeof (c_GPRS_TEST_MODE_CMD))
#define FREE_c_GPRS_TEST_MODE_CMD(sp)


// add by dingmx 20070330 for EDGE
#ifdef __EGPRS__
typedef struct _c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD
{

  	ED_OCTET ModeFlag;
}
c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD;

#define INIT_c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD(sp) ED_RESET_MEM ((sp), sizeof (c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD))
#define FREE_c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD(sp)

#endif

/*-----------------------------------*/
typedef struct _c_AUTHENTICATION_REJECT {
	int __Dummy_Field__;

}	c_AUTHENTICATION_REJECT;
#define INIT_c_AUTHENTICATION_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_REJECT))
#define FREE_c_AUTHENTICATION_REJECT(sp)

/* Access member 'CKSN' of type 'c_AUTHENTICATION_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_REQUEST_CKSN(var) var

/* Access member 'CKSN' of type 'c_AUTHENTICATION_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_REQUEST_CKSN(var) (&var)

/* Access member 'Spare' of type 'c_AUTHENTICATION_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_REQUEST_Spare(var) var

/* Access member 'Spare' of type 'c_AUTHENTICATION_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_REQUEST_Spare(var) (&var)

/* Access member 'RAND' of type 'c_AUTHENTICATION_REQUEST' as a variable reference */
#define VAR_c_AUTHENTICATION_REQUEST_RAND(var) var

/* Access member 'RAND' of type 'c_AUTHENTICATION_REQUEST' as a pointer */
#define PTR_c_AUTHENTICATION_REQUEST_RAND(var) (&var)


/*-----------------------------------*/
typedef struct _c_AUTHENTICATION_REQUEST {
	ED_BYTE RAND [16];
	ED_OCTET CKSN;
	ED_BOOLEAN AUTN_Present;
	ED_BYTE AUTN_Length;
	ED_BYTE AUTN[16];     

}	c_AUTHENTICATION_REQUEST;
#define INIT_c_AUTHENTICATION_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_REQUEST))
#define FREE_c_AUTHENTICATION_REQUEST(sp)


/*-----------------------------------*/
typedef struct _c_CM_SERVICE_ACCEPT {
	int __Dummy_Field__;

}	c_CM_SERVICE_ACCEPT;
#define INIT_c_CM_SERVICE_ACCEPT(sp) ED_RESET_MEM ((sp), sizeof (c_CM_SERVICE_ACCEPT))
#define FREE_c_CM_SERVICE_ACCEPT(sp)

/* Access member 'RejectCause' of type 'c_CM_SERVICE_REJECT' as a variable reference */
#define VAR_c_CM_SERVICE_REJECT_RejectCause(var) var

/* Access member 'RejectCause' of type 'c_CM_SERVICE_REJECT' as a pointer */
#define PTR_c_CM_SERVICE_REJECT_RejectCause(var) (&var)


/*-----------------------------------*/
typedef struct _c_CM_SERVICE_REJECT {
	ED_OCTET RejectCause;

}	c_CM_SERVICE_REJECT;
#define INIT_c_CM_SERVICE_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_CM_SERVICE_REJECT))
#define FREE_c_CM_SERVICE_REJECT(sp)

/* Access member 'RejectCause' of type 'c_ABORT' as a variable reference */
#define VAR_c_ABORT_RejectCause(var) var

/* Access member 'RejectCause' of type 'c_ABORT' as a pointer */
#define PTR_c_ABORT_RejectCause(var) (&var)


/*-----------------------------------*/
typedef struct _c_ABORT {
	ED_OCTET RejectCause;

}	c_ABORT;
#define INIT_c_ABORT(sp) ED_RESET_MEM ((sp), sizeof (c_ABORT))
#define FREE_c_ABORT(sp)

/* Access member 'IdentityType' of type 'c_IDENTITY_REQUEST' as a variable reference */
#define VAR_c_IDENTITY_REQUEST_IdentityType(var) var

/* Access member 'IdentityType' of type 'c_IDENTITY_REQUEST' as a pointer */
#define PTR_c_IDENTITY_REQUEST_IdentityType(var) (&var)

/* Access member 'Spare' of type 'c_IDENTITY_REQUEST' as a variable reference */
#define VAR_c_IDENTITY_REQUEST_Spare(var) var

/* Access member 'Spare' of type 'c_IDENTITY_REQUEST' as a pointer */
#define PTR_c_IDENTITY_REQUEST_Spare(var) (&var)


/*-----------------------------------*/
typedef struct _c_IDENTITY_REQUEST {
	ED_OCTET IdentityType;

}	c_IDENTITY_REQUEST;
#define INIT_c_IDENTITY_REQUEST(sp) ED_RESET_MEM ((sp), sizeof (c_IDENTITY_REQUEST))
#define FREE_c_IDENTITY_REQUEST(sp)

/* Access member 'LAI' of type 'c_LOCATION_UPDATE_ACCEPT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_ACCEPT_LAI(var) var

/* Access member 'LAI' of type 'c_LOCATION_UPDATE_ACCEPT' as a pointer */
#define PTR_c_LOCATION_UPDATE_ACCEPT_LAI(var) (&var)

/* Access member 'MobileId' of type 'c_LOCATION_UPDATE_ACCEPT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_ACCEPT_MobileId(var) var

/* Access member 'MobileId' of type 'c_LOCATION_UPDATE_ACCEPT' as a pointer */
#define PTR_c_LOCATION_UPDATE_ACCEPT_MobileId(var) (&var)

/* Access member 'FollowOnProceed' of type 'c_LOCATION_UPDATE_ACCEPT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_ACCEPT_FollowOnProceed(var) var

/* Access member 'FollowOnProceed' of type 'c_LOCATION_UPDATE_ACCEPT' as a pointer */
#define PTR_c_LOCATION_UPDATE_ACCEPT_FollowOnProceed(var) (&var)

/* Access member 'CTSPermission' of type 'c_LOCATION_UPDATE_ACCEPT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_ACCEPT_CTSPermission(var) var

/* Access member 'CTSPermission' of type 'c_LOCATION_UPDATE_ACCEPT' as a pointer */
#define PTR_c_LOCATION_UPDATE_ACCEPT_CTSPermission(var) (&var)

/* Access member 'EquivalentPLMN' of type 'c_LOCATION_UPDATE_ACCEPT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN(var) var

/* Access member 'EquivalentPLMN' of type 'c_LOCATION_UPDATE_ACCEPT' as a pointer */
#define PTR_c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN(var) (&var)

/* DEFINITION OF BINARY c_LOCATION_UPDATE_ACCEPT_MobileId */
typedef struct _c_LOCATION_UPDATE_ACCEPT_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_LOCATION_UPDATE_ACCEPT_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_LOCATION_UPDATE_ACCEPT_MobileId(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN */
typedef struct _c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN {
    // for R99 15, for R6 45, test in IDEA network 201609
	ED_BYTE value [45]; /* Variable size; bits needed 120 */
	int usedBits;
} c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_LOCATION_UPDATE_ACCEPT {
	ED_BYTE LAI [5];
	ED_BOOLEAN MobileId_Present;
	ED_BOOLEAN FollowOnProceed_Present;
	ED_BOOLEAN CTSPermission_Present;
	ED_BOOLEAN EquivalentPLMN_Present;
	c_LOCATION_UPDATE_ACCEPT_MobileId MobileId;
	c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN EquivalentPLMN;

}	c_LOCATION_UPDATE_ACCEPT;
#define INIT_c_LOCATION_UPDATE_ACCEPT(sp) ED_RESET_MEM ((sp), sizeof (c_LOCATION_UPDATE_ACCEPT))
#define FREE_c_LOCATION_UPDATE_ACCEPT(sp)
#define SETPRESENT_c_LOCATION_UPDATE_ACCEPT_MobileId(sp,present) ((sp)->MobileId_Present = present)
#define GETPRESENT_c_LOCATION_UPDATE_ACCEPT_MobileId(sp) ((sp)->MobileId_Present)
#define SETPRESENT_c_LOCATION_UPDATE_ACCEPT_FollowOnProceed(sp,present) ((sp)->FollowOnProceed_Present = present)
#define GETPRESENT_c_LOCATION_UPDATE_ACCEPT_FollowOnProceed(sp) ((sp)->FollowOnProceed_Present)
#define SETPRESENT_c_LOCATION_UPDATE_ACCEPT_CTSPermission(sp,present) ((sp)->CTSPermission_Present = present)
#define GETPRESENT_c_LOCATION_UPDATE_ACCEPT_CTSPermission(sp) ((sp)->CTSPermission_Present)
#define SETPRESENT_c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN(sp,present) ((sp)->EquivalentPLMN_Present = present)
#define GETPRESENT_c_LOCATION_UPDATE_ACCEPT_EquivalentPLMN(sp) ((sp)->EquivalentPLMN_Present)

/* Access member 'RejectCause' of type 'c_LOCATION_UPDATE_REJECT' as a variable reference */
#define VAR_c_LOCATION_UPDATE_REJECT_RejectCause(var) var

/* Access member 'RejectCause' of type 'c_LOCATION_UPDATE_REJECT' as a pointer */
#define PTR_c_LOCATION_UPDATE_REJECT_RejectCause(var) (&var)


/*-----------------------------------*/
typedef struct _c_LOCATION_UPDATE_REJECT {
	ED_OCTET RejectCause;

}	c_LOCATION_UPDATE_REJECT;
#define INIT_c_LOCATION_UPDATE_REJECT(sp) ED_RESET_MEM ((sp), sizeof (c_LOCATION_UPDATE_REJECT))
#define FREE_c_LOCATION_UPDATE_REJECT(sp)

/* Access member 'InformationIE' of type 'c_MM_INFORMATION' as a variable reference */
#define VAR_c_MM_INFORMATION_InformationIE(var) var

/* Access member 'InformationIE' of type 'c_MM_INFORMATION' as a pointer */
#define PTR_c_MM_INFORMATION_InformationIE(var) (&var)


/*-----------------------------------*/
typedef struct _c_MM_INFORMATION {
	c_InformationIE_t InformationIE;

}	c_MM_INFORMATION;
#define INIT_c_MM_INFORMATION(sp) ED_RESET_MEM ((sp), sizeof (c_MM_INFORMATION))
#define FREE_c_MM_INFORMATION(sp)

/* Access member 'LAI' of type 'c_TMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_TMSI_REALLOCATION_COMMAND_LAI(var) var

/* Access member 'LAI' of type 'c_TMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_TMSI_REALLOCATION_COMMAND_LAI(var) (&var)

/* Access member 'MobileId' of type 'c_TMSI_REALLOCATION_COMMAND' as a variable reference */
#define VAR_c_TMSI_REALLOCATION_COMMAND_MobileId(var) var

/* Access member 'MobileId' of type 'c_TMSI_REALLOCATION_COMMAND' as a pointer */
#define PTR_c_TMSI_REALLOCATION_COMMAND_MobileId(var) (&var)

/* DEFINITION OF BINARY c_TMSI_REALLOCATION_COMMAND_MobileId */
typedef struct _c_TMSI_REALLOCATION_COMMAND_MobileId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_TMSI_REALLOCATION_COMMAND_MobileId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_TMSI_REALLOCATION_COMMAND_MobileId(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_TMSI_REALLOCATION_COMMAND {
	ED_BYTE LAI [5];
	c_TMSI_REALLOCATION_COMMAND_MobileId MobileId;

}	c_TMSI_REALLOCATION_COMMAND;
#define INIT_c_TMSI_REALLOCATION_COMMAND(sp) ED_RESET_MEM ((sp), sizeof (c_TMSI_REALLOCATION_COMMAND))
#define FREE_c_TMSI_REALLOCATION_COMMAND(sp)

/* Access member 'CN' of type 'c_Packet_Paging_Request_Page_info_data' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data_CN(var) var

/* Access member 'CN' of type 'c_Packet_Paging_Request_Page_info_data' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data_CN(var) (&var)

/* Access member 'Length' of type 'c_Packet_Paging_Request_Page_info_data' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data_Length(var) var

/* Access member 'Length' of type 'c_Packet_Paging_Request_Page_info_data' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data_Length(var) (&var)

/* Access member 'MsId' of type 'c_Packet_Paging_Request_Page_info_data' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data_MsId(var) var

/* Access member 'MsId' of type 'c_Packet_Paging_Request_Page_info_data' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data_MsId(var) (&var)

/* Access member 'data' of type 'c_Packet_Paging_Request_Page_info_data_TMSI' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data_TMSI_data(var) var

/* Access member 'data' of type 'c_Packet_Paging_Request_Page_info_data_TMSI' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data_TMSI_data(var) (&var)

/* Access member 'TMSI' of type 'c_Packet_Paging_Request_Page_info_data' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data_TMSI(var) var

/* Access member 'TMSI' of type 'c_Packet_Paging_Request_Page_info_data' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data_TMSI(var) (&var)

/* Access member 'data' of type 'c_Packet_Paging_Request_Page_info' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info_data(var) var

/* Access member 'data' of type 'c_Packet_Paging_Request_Page_info' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info_data(var) (&var)

/* Access member 'Page_info' of type 'c_Packet_Paging_Request' as a variable reference */
#define VAR_c_Packet_Paging_Request_Page_info(var) var

/* Access member 'Page_info' of type 'c_Packet_Paging_Request' as a pointer */
#define PTR_c_Packet_Paging_Request_Page_info(var) (&var)

/* DEFINITION OF BINARY c_Packet_Paging_Request_Page_info_data_MsId */
typedef struct _c_Packet_Paging_Request_Page_info_data_MsId {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Packet_Paging_Request_Page_info_data_MsId;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Packet_Paging_Request_Page_info_data_MsId(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Packet_Paging_Request_Page_info_data_TMSI */
typedef struct _c_Packet_Paging_Request_Page_info_data_TMSI {
	ED_OCTET data [4];
	int items;
} c_Packet_Paging_Request_Page_info_data_TMSI;

/* DEFINITION OF SUB-STRUCTURE c_Packet_Paging_Request_Page_info_data */
typedef struct _c_Packet_Paging_Request_Page_info_data {
	ED_OCTET CN;
	ED_BOOLEAN CN_Present;
	ED_OCTET Length;
	ED_BOOLEAN Length_Present;
	ED_BOOLEAN MsId_Present;
	c_Packet_Paging_Request_Page_info_data_MsId MsId;
	c_Packet_Paging_Request_Page_info_data_TMSI TMSI;

} c_Packet_Paging_Request_Page_info_data;

/* DEFINITION OF SUB-STRUCTURE c_Packet_Paging_Request_Page_info */
typedef struct _c_Packet_Paging_Request_Page_info {
	c_Packet_Paging_Request_Page_info_data data [4];
	int items;
} c_Packet_Paging_Request_Page_info;


/*-----------------------------------*/
typedef struct _c_Packet_Paging_Request {
	c_Packet_Paging_Request_Page_info Page_info;

}	c_Packet_Paging_Request;
#define INIT_c_Packet_Paging_Request(sp) ED_RESET_MEM ((sp), sizeof (c_Packet_Paging_Request))
#define FREE_c_Packet_Paging_Request(sp)

/* Access member 'Pkt_Page_Ind_1' of type 'c_P1_Rest_Octets' as a variable reference */
#define VAR_c_P1_Rest_Octets_Pkt_Page_Ind_1(var) var

/* Access member 'Pkt_Page_Ind_1' of type 'c_P1_Rest_Octets' as a pointer */
#define PTR_c_P1_Rest_Octets_Pkt_Page_Ind_1(var) (&var)

/* Access member 'Pkt_Page_Ind_2' of type 'c_P1_Rest_Octets' as a variable reference */
#define VAR_c_P1_Rest_Octets_Pkt_Page_Ind_2(var) var

/* Access member 'Pkt_Page_Ind_2' of type 'c_P1_Rest_Octets' as a pointer */
#define PTR_c_P1_Rest_Octets_Pkt_Page_Ind_2(var) (&var)


/*-----------------------------------*/
typedef struct _c_P1_Rest_Octets {
	ED_BOOLEAN Pkt_Page_Ind_1;
	ED_BOOLEAN Pkt_Page_Ind_1_Present;
	ED_BOOLEAN Pkt_Page_Ind_2;
	ED_BOOLEAN Pkt_Page_Ind_2_Present;

}	c_P1_Rest_Octets;
#define INIT_c_P1_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_P1_Rest_Octets))
#define FREE_c_P1_Rest_Octets(sp)
#define SETPRESENT_c_P1_Rest_Octets_Pkt_Page_Ind_1(sp,present) ((sp)->Pkt_Page_Ind_1_Present = present)
#define GETPRESENT_c_P1_Rest_Octets_Pkt_Page_Ind_1(sp) ((sp)->Pkt_Page_Ind_1_Present)
#define SETPRESENT_c_P1_Rest_Octets_Pkt_Page_Ind_2(sp,present) ((sp)->Pkt_Page_Ind_2_Present = present)
#define GETPRESENT_c_P1_Rest_Octets_Pkt_Page_Ind_2(sp) ((sp)->Pkt_Page_Ind_2_Present)

/* Access member 'CN3' of type 'c_P2_Rest_Octets' as a variable reference */
#define VAR_c_P2_Rest_Octets_CN3(var) var

/* Access member 'CN3' of type 'c_P2_Rest_Octets' as a pointer */
#define PTR_c_P2_Rest_Octets_CN3(var) (&var)

/* Access member 'Pkt_Page_Ind_3' of type 'c_P2_Rest_Octets' as a variable reference */
#define VAR_c_P2_Rest_Octets_Pkt_Page_Ind_3(var) var

/* Access member 'Pkt_Page_Ind_3' of type 'c_P2_Rest_Octets' as a pointer */
#define PTR_c_P2_Rest_Octets_Pkt_Page_Ind_3(var) (&var)


/*-----------------------------------*/
typedef struct _c_P2_Rest_Octets {
	ED_OCTET CN3;
	ED_BOOLEAN CN3_Present;
	ED_BOOLEAN Pkt_Page_Ind_3;
	ED_BOOLEAN Pkt_Page_Ind_3_Present;

}	c_P2_Rest_Octets;
#define INIT_c_P2_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_P2_Rest_Octets))
#define FREE_c_P2_Rest_Octets(sp)
#define SETPRESENT_c_P2_Rest_Octets_CN3(sp,present) ((sp)->CN3_Present = present)
#define GETPRESENT_c_P2_Rest_Octets_CN3(sp) ((sp)->CN3_Present)
#define SETPRESENT_c_P2_Rest_Octets_Pkt_Page_Ind_3(sp,present) ((sp)->Pkt_Page_Ind_3_Present = present)
#define GETPRESENT_c_P2_Rest_Octets_Pkt_Page_Ind_3(sp) ((sp)->Pkt_Page_Ind_3_Present)

/* Access member 'CN3' of type 'c_P3_Rest_Octets' as a variable reference */
#define VAR_c_P3_Rest_Octets_CN3(var) var

/* Access member 'CN3' of type 'c_P3_Rest_Octets' as a pointer */
#define PTR_c_P3_Rest_Octets_CN3(var) (&var)

/* Access member 'CN4' of type 'c_P3_Rest_Octets' as a variable reference */
#define VAR_c_P3_Rest_Octets_CN4(var) var

/* Access member 'CN4' of type 'c_P3_Rest_Octets' as a pointer */
#define PTR_c_P3_Rest_Octets_CN4(var) (&var)


/*-----------------------------------*/
typedef struct _c_P3_Rest_Octets {
	ED_OCTET CN3;
	ED_BOOLEAN CN3_Present;
	ED_OCTET CN4;
	ED_BOOLEAN CN4_Present;

}	c_P3_Rest_Octets;
#define INIT_c_P3_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_P3_Rest_Octets))
#define FREE_c_P3_Rest_Octets(sp)
#define SETPRESENT_c_P3_Rest_Octets_CN3(sp,present) ((sp)->CN3_Present = present)
#define GETPRESENT_c_P3_Rest_Octets_CN3(sp) ((sp)->CN3_Present)
#define SETPRESENT_c_P3_Rest_Octets_CN4(sp,present) ((sp)->CN4_Present = present)
#define GETPRESENT_c_P3_Rest_Octets_CN4(sp) ((sp)->CN4_Present)

/* SETITEMS/ADDITEMS commands for type 'c_RxNpduNumList_NpduValue'. */
void SETITEMS_c_RxNpduNumList_NpduValue (c_RxNpduNumList_NpduValue* sequence, int desiredItems);
#define ADDITEMS_c_RxNpduNumList_NpduValue(sequence, itemsToBeAdded) SETITEMS_c_RxNpduNumList_NpduValue (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RxNpduNumList_Nsapi'. */
void SETITEMS_c_RxNpduNumList_Nsapi (c_RxNpduNumList_Nsapi* sequence, int desiredItems);
#define ADDITEMS_c_RxNpduNumList_Nsapi(sequence, itemsToBeAdded) SETITEMS_c_RxNpduNumList_Nsapi (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Packet_Paging_Request_Page_info'. */
void SETITEMS_c_Packet_Paging_Request_Page_info (c_Packet_Paging_Request_Page_info* sequence, int desiredItems);
#define ADDITEMS_c_Packet_Paging_Request_Page_info(sequence, itemsToBeAdded) SETITEMS_c_Packet_Paging_Request_Page_info (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Packet_Paging_Request_Page_info_data_CN(sp,present) ((sp)->CN_Present = present)
#define GETPRESENT_c_Packet_Paging_Request_Page_info_data_CN(sp) ((sp)->CN_Present)
#define SETPRESENT_c_Packet_Paging_Request_Page_info_data_Length(sp,present) ((sp)->Length_Present = present)
#define GETPRESENT_c_Packet_Paging_Request_Page_info_data_Length(sp) ((sp)->Length_Present)
#define SETPRESENT_c_Packet_Paging_Request_Page_info_data_MsId(sp,present) ((sp)->MsId_Present = present)
#define GETPRESENT_c_Packet_Paging_Request_Page_info_data_MsId(sp) ((sp)->MsId_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Packet_Paging_Request_Page_info_data_TMSI'. */
void SETITEMS_c_Packet_Paging_Request_Page_info_data_TMSI (c_Packet_Paging_Request_Page_info_data_TMSI* sequence, int desiredItems);
#define ADDITEMS_c_Packet_Paging_Request_Page_info_data_TMSI(sequence, itemsToBeAdded) SETITEMS_c_Packet_Paging_Request_Page_info_data_TMSI (sequence, (sequence)->items+itemsToBeAdded)


typedef struct _c_AUTH_AND_CIPH_FAILURE_PARA {
  ED_BYTE value[14];
  int usedBits;
} c_AUTH_AND_CIPH_FAILURE_PARA;

typedef struct _c_AUTHENTICATION_AND_CIPHERING_FAILURE {
  ED_BYTE GmmCause;
  BOOL Para_Present;
  c_AUTH_AND_CIPH_FAILURE_PARA AuthCiphFailure_Para;
} c_AUTHENTICATION_AND_CIPHERING_FAILURE;
#define INIT_c_AUTHENTICATION_AND_CIPHERING_FAILURE(sp) ED_RESET_MEM ((sp), sizeof (c_AUTHENTICATION_AND_CIPHERING_FAILURE))
#define FREE_c_AUTHENTICATION_AND_CIPHERING_FAILURE(sp)

#ifdef __cplusplus
};
#endif

#endif

