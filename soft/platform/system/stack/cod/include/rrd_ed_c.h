/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_ed_c_h_H
#define __rrd_ed_c_h_H
#include "ed_data.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Access member 'T1' of type 'c_TStartingTime' as a variable reference */
#define VAR_c_TStartingTime_T1(var) var

/* Access member 'T1' of type 'c_TStartingTime' as a pointer */
#define PTR_c_TStartingTime_T1(var) (&var)

/* Access member 'T3' of type 'c_TStartingTime' as a variable reference */
#define VAR_c_TStartingTime_T3(var) var

/* Access member 'T3' of type 'c_TStartingTime' as a pointer */
#define PTR_c_TStartingTime_T3(var) (&var)

/* Access member 'T2' of type 'c_TStartingTime' as a variable reference */
#define VAR_c_TStartingTime_T2(var) var

/* Access member 'T2' of type 'c_TStartingTime' as a pointer */
#define PTR_c_TStartingTime_T2(var) (&var)


/*-----------------------------------*/
typedef struct _c_TStartingTime {
	ED_OCTET T1;
	ED_OCTET T3;
	ED_OCTET T2;

}	c_TStartingTime;
#define INIT_c_TStartingTime(sp) ED_RESET_MEM ((sp), sizeof (c_TStartingTime))
#define FREE_c_TStartingTime(sp)

#ifdef __EGPRS__
////////////////////////////////////////////////////////////////////////////////
//	add by dingmx begin 20061031
#define EGPRS_WIN_SIZE_IE_LEN	5
#define EGPRS_CH_COD_CMD_IE_LEN	4
// shared IE
typedef struct _c_BITMAP_64BIT {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_BITMAP_64BIT;

typedef struct _c_Global_TFI
{
    ED_BOOLEAN	UL_TFI_Prestent;
    ED_OCTET	UL_TFI;
    ED_BOOLEAN	DL_TFI_Prestent;
    ED_OCTET	DL_TFI;
}c_Global_TFI;
long DECODE_c_Global_TFI_IE( const char* const Buffer,
                            const long BitOffset,
                            c_Global_TFI* const Destin,
                            long Length );
typedef struct _c_COMPACT_reduced_MA
{
    ED_OCTET length;
    ED_OCTET bitmap[16];
    ED_BOOLEAN MAIO_2_Prestent;
    ED_OCTET MaIO_2;
}c_COMPACT_reduced_MA;
long DECODE_c_COMPACT_reduced_MA_IE( const char* const Buffer,
                                    const long BitOffset,
                                    c_COMPACT_reduced_MA* const Destin,
                                    long Length );
typedef struct _c_Starting_Frame_Number_Description
{
    ED_SHORT Relative_FN;
    ED_BOOLEAN Absolute_FN_Present;
    ED_BOOLEAN Relative_FN_Present;
    c_TStartingTime Absolute_FN;
} c_Starting_Frame_Number_Description;
long DECODE_c_Starting_Frame_Number_Description_IE( const char* const Buffer,
                                                   const long BitOffset,
                                                   c_Starting_Frame_Number_Description* const Destin,
                                                   long Length );
//	add by dingmx end
////////////////////////////////////////////////////////////////////////////////
#endif

/* Access member 'BitMap' of type 'c_FrequencyList_BitMap0' as a variable reference */
#define VAR_c_FrequencyList_BitMap0_BitMap(var) var

/* Access member 'BitMap' of type 'c_FrequencyList_BitMap0' as a pointer */
#define PTR_c_FrequencyList_BitMap0_BitMap(var) (&var)

/* Access member 'Offset' of type 'c_FrequencyList_BitMap0' as a variable reference */
#define VAR_c_FrequencyList_BitMap0_Offset(var) var

/* Access member 'Offset' of type 'c_FrequencyList_BitMap0' as a pointer */
#define PTR_c_FrequencyList_BitMap0_Offset(var) (&var)

/* Access member 'BitMap0' of type 'c_FrequencyList' as a variable reference */
#define VAR_c_FrequencyList_BitMap0(var) var

/* Access member 'BitMap0' of type 'c_FrequencyList' as a pointer */
#define PTR_c_FrequencyList_BitMap0(var) (&var)

/* Access member 'BitMap' of type 'c_FrequencyList_VarBitMap' as a variable reference */
#define VAR_c_FrequencyList_VarBitMap_BitMap(var) var

/* Access member 'BitMap' of type 'c_FrequencyList_VarBitMap' as a pointer */
#define PTR_c_FrequencyList_VarBitMap_BitMap(var) (&var)

/* Access member 'Offset' of type 'c_FrequencyList_VarBitMap' as a variable reference */
#define VAR_c_FrequencyList_VarBitMap_Offset(var) var

/* Access member 'Offset' of type 'c_FrequencyList_VarBitMap' as a pointer */
#define PTR_c_FrequencyList_VarBitMap_Offset(var) (&var)

/* Access member 'OrigARFCN' of type 'c_FrequencyList_VarBitMap' as a variable reference */
#define VAR_c_FrequencyList_VarBitMap_OrigARFCN(var) var

/* Access member 'OrigARFCN' of type 'c_FrequencyList_VarBitMap' as a pointer */
#define PTR_c_FrequencyList_VarBitMap_OrigARFCN(var) (&var)

/* Access member 'VarBitMap' of type 'c_FrequencyList' as a variable reference */
#define VAR_c_FrequencyList_VarBitMap(var) var

/* Access member 'VarBitMap' of type 'c_FrequencyList' as a pointer */
#define PTR_c_FrequencyList_VarBitMap(var) (&var)

/* Access member 'data' of type 'c_FrequencyList_Range_Wi' as a variable reference */
#define VAR_c_FrequencyList_Range_Wi_data(var) var

/* Access member 'data' of type 'c_FrequencyList_Range_Wi' as a pointer */
#define PTR_c_FrequencyList_Range_Wi_data(var) (&var)

/* Access member 'Wi' of type 'c_FrequencyList_Range' as a variable reference */
#define VAR_c_FrequencyList_Range_Wi(var) var

/* Access member 'Wi' of type 'c_FrequencyList_Range' as a pointer */
#define PTR_c_FrequencyList_Range_Wi(var) (&var)

/* Access member 'OrigARFCN' of type 'c_FrequencyList_Range' as a variable reference */
#define VAR_c_FrequencyList_Range_OrigARFCN(var) var

/* Access member 'OrigARFCN' of type 'c_FrequencyList_Range' as a pointer */
#define PTR_c_FrequencyList_Range_OrigARFCN(var) (&var)

/* Access member 'Range' of type 'c_FrequencyList' as a variable reference */
#define VAR_c_FrequencyList_Range(var) var

/* Access member 'Range' of type 'c_FrequencyList' as a pointer */
#define PTR_c_FrequencyList_Range(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_FrequencyList_BitMap0 */
typedef struct _c_FrequencyList_BitMap0 {
	ED_LONG BitMap;
	ED_LONG Offset;

} c_FrequencyList_BitMap0;

/* DEFINITION OF SUB-STRUCTURE c_FrequencyList_VarBitMap */
typedef struct _c_FrequencyList_VarBitMap {
	ED_LONG BitMap;
	ED_LONG Offset;
	ED_LONG OrigARFCN;

} c_FrequencyList_VarBitMap;

/* DEFINITION OF SUB-STRUCTURE c_FrequencyList_Range_Wi */
typedef struct _c_FrequencyList_Range_Wi {
	ED_LONG data [266];
	int items;
} c_FrequencyList_Range_Wi;

/* DEFINITION OF SUB-STRUCTURE c_FrequencyList_Range */
typedef struct _c_FrequencyList_Range {
	ED_LONG OrigARFCN;
	c_FrequencyList_Range_Wi Wi;

} c_FrequencyList_Range;


/*-----------------------------------*/
typedef enum {
	U_c_FrequencyList_NONE,
	U_c_FrequencyList_BitMap0,
	U_c_FrequencyList_VarBitMap,
	U_c_FrequencyList_Range
} TPRESENT_c_FrequencyList;

typedef struct _c_FrequencyList {
		TPRESENT_c_FrequencyList Present;
		union {
			c_FrequencyList_BitMap0 BitMap0;
		
			c_FrequencyList_VarBitMap VarBitMap;
		
			c_FrequencyList_Range Range;
		
		} u;

}	c_FrequencyList;
#define INIT_c_FrequencyList(sp) ED_RESET_MEM ((sp), sizeof (c_FrequencyList))
#define FREE_c_FrequencyList(sp)

/* SETPRESENT commands for type 'c_FrequencyList' */
#define GLOB_SETPRESENT_c_FrequencyList(sp,toBeSetPresent) (sp)->Present = toBeSetPresent
#define SETPRESENT_c_FrequencyList_BitMap0(sp) (sp)->Present = U_c_FrequencyList_BitMap0
#define GETPRESENT_c_FrequencyList_BitMap0(sp) ((sp)->Present == U_c_FrequencyList_BitMap0)
#define SETPRESENT_c_FrequencyList_VarBitMap(sp) (sp)->Present = U_c_FrequencyList_VarBitMap
#define GETPRESENT_c_FrequencyList_VarBitMap(sp) ((sp)->Present == U_c_FrequencyList_VarBitMap)
#define SETPRESENT_c_FrequencyList_Range(sp) (sp)->Present = U_c_FrequencyList_Range
#define GETPRESENT_c_FrequencyList_Range(sp) ((sp)->Present == U_c_FrequencyList_Range)
#define GETPRESENT_c_FrequencyList(sp) ((sp)->Present)


/* Access member 'data' of type 'c_TSkip' as a variable reference */
#define VAR_c_TSkip_data(var) var

/* Access member 'data' of type 'c_TSkip' as a pointer */
#define PTR_c_TSkip_data(var) (&var)


/*-----------------------------------*/
typedef struct _c_TSkip {
		ED_OCTET data [16];
		int items;

}	c_TSkip;
#define INIT_c_TSkip(sp) ED_RESET_MEM ((sp), sizeof (c_TSkip))
#define FREE_c_TSkip(sp)
/* SETITEMS/ADDITEMS commands for type 'c_TSkip'. */
void SETITEMS_c_TSkip (c_TSkip* sequence, int desiredItems);
#define ADDITEMS_c_TSkip(sequence, itemsToBeAdded) SETITEMS_c_TSkip (sequence, (sequence)->items+itemsToBeAdded)


/* Access member 'Format' of type 'c_TFrqList' as a variable reference */
#define VAR_c_TFrqList_Format(var) var

/* Access member 'Format' of type 'c_TFrqList' as a pointer */
#define PTR_c_TFrqList_Format(var) (&var)

/* Access member 'List' of type 'c_TFrqList' as a variable reference */
#define VAR_c_TFrqList_List(var) var

/* Access member 'List' of type 'c_TFrqList' as a pointer */
#define PTR_c_TFrqList_List(var) (&var)

/* Access member 'ListLength' of type 'c_TFrqList' as a variable reference */
#define VAR_c_TFrqList_ListLength(var) var

/* Access member 'ListLength' of type 'c_TFrqList' as a pointer */
#define PTR_c_TFrqList_ListLength(var) (&var)


/*-----------------------------------*/
typedef struct _c_TFrqList {
	ED_LONG Format;
	ED_LONG ListLength;
	c_FrequencyList List;

}	c_TFrqList;
#define INIT_c_TFrqList(sp) ED_RESET_MEM ((sp), sizeof (c_TFrqList))
#define FREE_c_TFrqList(sp)

/* Access member 'ArfcnH' of type 'c_TCellDesc' as a variable reference */
#define VAR_c_TCellDesc_ArfcnH(var) var

/* Access member 'ArfcnH' of type 'c_TCellDesc' as a pointer */
#define PTR_c_TCellDesc_ArfcnH(var) (&var)

/* Access member 'Bsic' of type 'c_TCellDesc' as a variable reference */
#define VAR_c_TCellDesc_Bsic(var) var

/* Access member 'Bsic' of type 'c_TCellDesc' as a pointer */
#define PTR_c_TCellDesc_Bsic(var) (&var)

/* Access member 'ArfcnL' of type 'c_TCellDesc' as a variable reference */
#define VAR_c_TCellDesc_ArfcnL(var) var

/* Access member 'ArfcnL' of type 'c_TCellDesc' as a pointer */
#define PTR_c_TCellDesc_ArfcnL(var) (&var)


/*-----------------------------------*/
typedef struct _c_TCellDesc {
	ED_OCTET ArfcnH;
	ED_OCTET Bsic;
	ED_OCTET ArfcnL;

}	c_TCellDesc;
#define INIT_c_TCellDesc(sp) ED_RESET_MEM ((sp), sizeof (c_TCellDesc))
#define FREE_c_TCellDesc(sp)

/* Access member 'ChType' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_ChType(var) var

/* Access member 'ChType' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_ChType(var) (&var)

/* Access member 'TN' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_TN(var) var

/* Access member 'TN' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_TN(var) (&var)

/* Access member 'TSC' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_TSC(var) var

/* Access member 'TSC' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_TSC(var) (&var)

/* Access member 'H' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_H(var) var

/* Access member 'H' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_H(var) (&var)

/* Access member 'MAIO' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_MAIO(var) var

/* Access member 'MAIO' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_MAIO(var) (&var)

/* Access member 'HSN' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_HSN(var) var

/* Access member 'HSN' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_HSN(var) (&var)

/* Access member 'spare' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_spare(var) var

/* Access member 'spare' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_spare(var) (&var)

/* Access member 'ARFCN' of type 'c_TChDesc' as a variable reference */
#define VAR_c_TChDesc_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_TChDesc' as a pointer */
#define PTR_c_TChDesc_ARFCN(var) (&var)


/*-----------------------------------*/
typedef struct _c_TChDesc {
	ED_SHORT ARFCN;
	ED_OCTET ChType;
	ED_OCTET TN;
	ED_OCTET TSC;
	ED_BOOLEAN H;
	ED_OCTET MAIO;
	ED_BOOLEAN MAIO_Present;
	ED_OCTET HSN;
	ED_BOOLEAN HSN_Present;
	ED_BOOLEAN spare_Present;
	ED_BOOLEAN ARFCN_Present;

}	c_TChDesc;
#define INIT_c_TChDesc(sp) ED_RESET_MEM ((sp), sizeof (c_TChDesc))
#define FREE_c_TChDesc(sp)
#define SETPRESENT_c_TChDesc_MAIO(sp,present) ((sp)->MAIO_Present = present)
#define GETPRESENT_c_TChDesc_MAIO(sp) ((sp)->MAIO_Present)
#define SETPRESENT_c_TChDesc_HSN(sp,present) ((sp)->HSN_Present = present)
#define GETPRESENT_c_TChDesc_HSN(sp) ((sp)->HSN_Present)
#define SETPRESENT_c_TChDesc_spare(sp,present) ((sp)->spare_Present = present)
#define GETPRESENT_c_TChDesc_spare(sp) ((sp)->spare_Present)
#define SETPRESENT_c_TChDesc_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_TChDesc_ARFCN(sp) ((sp)->ARFCN_Present)

/* Access member 'AlgorithmId' of type 'c_TCipherModeSetting' as a variable reference */
#define VAR_c_TCipherModeSetting_AlgorithmId(var) var

/* Access member 'AlgorithmId' of type 'c_TCipherModeSetting' as a pointer */
#define PTR_c_TCipherModeSetting_AlgorithmId(var) (&var)

/* Access member 'SC' of type 'c_TCipherModeSetting' as a variable reference */
#define VAR_c_TCipherModeSetting_SC(var) var

/* Access member 'SC' of type 'c_TCipherModeSetting' as a pointer */
#define PTR_c_TCipherModeSetting_SC(var) (&var)


/*-----------------------------------*/
typedef struct _c_TCipherModeSetting {
	ED_OCTET AlgorithmId;
	ED_BOOLEAN SC;

}	c_TCipherModeSetting;
#define INIT_c_TCipherModeSetting(sp) ED_RESET_MEM ((sp), sizeof (c_TCipherModeSetting))
#define FREE_c_TCipherModeSetting(sp)

/* Access member 'spare' of type 'c_TFrqChnSeq' as a variable reference */
#define VAR_c_TFrqChnSeq_spare(var) var

/* Access member 'spare' of type 'c_TFrqChnSeq' as a pointer */
#define PTR_c_TFrqChnSeq_spare(var) (&var)

/* Access member 'LowestARFCN' of type 'c_TFrqChnSeq' as a variable reference */
#define VAR_c_TFrqChnSeq_LowestARFCN(var) var

/* Access member 'LowestARFCN' of type 'c_TFrqChnSeq' as a pointer */
#define PTR_c_TFrqChnSeq_LowestARFCN(var) (&var)

/* Access member 'Skip' of type 'c_TFrqChnSeq' as a variable reference */
#define VAR_c_TFrqChnSeq_Skip(var) var

/* Access member 'Skip' of type 'c_TFrqChnSeq' as a pointer */
#define PTR_c_TFrqChnSeq_Skip(var) (&var)


/*-----------------------------------*/
typedef struct _c_TFrqChnSeq {
	ED_OCTET LowestARFCN;
	c_TSkip Skip;

}	c_TFrqChnSeq;
#define INIT_c_TFrqChnSeq(sp) ED_RESET_MEM ((sp), sizeof (c_TFrqChnSeq))
#define FREE_c_TFrqChnSeq(sp)

/* Access member 'force' of type 'c_TDedicatedModeOrTbf' as a variable reference */
#define VAR_c_TDedicatedModeOrTbf_force(var) var

/* Access member 'force' of type 'c_TDedicatedModeOrTbf' as a pointer */
#define PTR_c_TDedicatedModeOrTbf_force(var) (&var)

/* Access member 'TMA' of type 'c_TDedicatedModeOrTbf' as a variable reference */
#define VAR_c_TDedicatedModeOrTbf_TMA(var) var

/* Access member 'TMA' of type 'c_TDedicatedModeOrTbf' as a pointer */
#define PTR_c_TDedicatedModeOrTbf_TMA(var) (&var)

/* Access member 'Downlink' of type 'c_TDedicatedModeOrTbf' as a variable reference */
#define VAR_c_TDedicatedModeOrTbf_Downlink(var) var

/* Access member 'Downlink' of type 'c_TDedicatedModeOrTbf' as a pointer */
#define PTR_c_TDedicatedModeOrTbf_Downlink(var) (&var)

/* Access member 'TBF' of type 'c_TDedicatedModeOrTbf' as a variable reference */
#define VAR_c_TDedicatedModeOrTbf_TBF(var) var

/* Access member 'TBF' of type 'c_TDedicatedModeOrTbf' as a pointer */
#define PTR_c_TDedicatedModeOrTbf_TBF(var) (&var)


/*-----------------------------------*/
typedef struct _c_TDedicatedModeOrTbf {
	ED_BOOLEAN TMA;
	ED_BOOLEAN Downlink;
	ED_BOOLEAN TBF;

}	c_TDedicatedModeOrTbf;
#define INIT_c_TDedicatedModeOrTbf(sp) ED_RESET_MEM ((sp), sizeof (c_TDedicatedModeOrTbf))
#define FREE_c_TDedicatedModeOrTbf(sp)

/* Access member 'spare' of type 'c_TPowerCommand' as a variable reference */
#define VAR_c_TPowerCommand_spare(var) var

/* Access member 'spare' of type 'c_TPowerCommand' as a pointer */
#define PTR_c_TPowerCommand_spare(var) (&var)

/* Access member 'FPC' of type 'c_TPowerCommand' as a variable reference */
#define VAR_c_TPowerCommand_FPC(var) var

/* Access member 'FPC' of type 'c_TPowerCommand' as a pointer */
#define PTR_c_TPowerCommand_FPC(var) (&var)

/* Access member 'PowerLevel' of type 'c_TPowerCommand' as a variable reference */
#define VAR_c_TPowerCommand_PowerLevel(var) var

/* Access member 'PowerLevel' of type 'c_TPowerCommand' as a pointer */
#define PTR_c_TPowerCommand_PowerLevel(var) (&var)


/*-----------------------------------*/
typedef struct _c_TPowerCommand {
	ED_BOOLEAN FPC;
	ED_OCTET PowerLevel;

}	c_TPowerCommand;
#define INIT_c_TPowerCommand(sp) ED_RESET_MEM ((sp), sizeof (c_TPowerCommand))
#define FREE_c_TPowerCommand(sp)

/* Access member 'ATC' of type 'c_TPowerCommandAccType' as a variable reference */
#define VAR_c_TPowerCommandAccType_ATC(var) var

/* Access member 'ATC' of type 'c_TPowerCommandAccType' as a pointer */
#define PTR_c_TPowerCommandAccType_ATC(var) (&var)

/* Access member 'spare' of type 'c_TPowerCommandAccType' as a variable reference */
#define VAR_c_TPowerCommandAccType_spare(var) var

/* Access member 'spare' of type 'c_TPowerCommandAccType' as a pointer */
#define PTR_c_TPowerCommandAccType_spare(var) (&var)

/* Access member 'FPC' of type 'c_TPowerCommandAccType' as a variable reference */
#define VAR_c_TPowerCommandAccType_FPC(var) var

/* Access member 'FPC' of type 'c_TPowerCommandAccType' as a pointer */
#define PTR_c_TPowerCommandAccType_FPC(var) (&var)

/* Access member 'PowerLevel' of type 'c_TPowerCommandAccType' as a variable reference */
#define VAR_c_TPowerCommandAccType_PowerLevel(var) var

/* Access member 'PowerLevel' of type 'c_TPowerCommandAccType' as a pointer */
#define PTR_c_TPowerCommandAccType_PowerLevel(var) (&var)


/*-----------------------------------*/
typedef struct _c_TPowerCommandAccType {
	ED_BOOLEAN ATC;
	ED_BOOLEAN FPC;
	ED_OCTET PowerLevel;

}	c_TPowerCommandAccType;
#define INIT_c_TPowerCommandAccType(sp) ED_RESET_MEM ((sp), sizeof (c_TPowerCommandAccType))
#define FREE_c_TPowerCommandAccType(sp)

/* Access member 'RA' of type 'c_TReqRef' as a variable reference */
#define VAR_c_TReqRef_RA(var) var

/* Access member 'RA' of type 'c_TReqRef' as a pointer */
#define PTR_c_TReqRef_RA(var) (&var)

/* Access member 'Time' of type 'c_TReqRef' as a variable reference */
#define VAR_c_TReqRef_Time(var) var

/* Access member 'Time' of type 'c_TReqRef' as a pointer */
#define PTR_c_TReqRef_Time(var) (&var)


/*-----------------------------------*/
typedef struct _c_TReqRef {
	ED_OCTET RA;
	c_TStartingTime Time;

}	c_TReqRef;
#define INIT_c_TReqRef(sp) ED_RESET_MEM ((sp), sizeof (c_TReqRef))
#define FREE_c_TReqRef(sp)

/* Access member 'NCI' of type 'c_TSynchInd' as a variable reference */
#define VAR_c_TSynchInd_NCI(var) var

/* Access member 'NCI' of type 'c_TSynchInd' as a pointer */
#define PTR_c_TSynchInd_NCI(var) (&var)

/* Access member 'ROT' of type 'c_TSynchInd' as a variable reference */
#define VAR_c_TSynchInd_ROT(var) var

/* Access member 'ROT' of type 'c_TSynchInd' as a pointer */
#define PTR_c_TSynchInd_ROT(var) (&var)

/* Access member 'SI' of type 'c_TSynchInd' as a variable reference */
#define VAR_c_TSynchInd_SI(var) var

/* Access member 'SI' of type 'c_TSynchInd' as a pointer */
#define PTR_c_TSynchInd_SI(var) (&var)


/*-----------------------------------*/
typedef struct _c_TSynchInd {
	ED_BOOLEAN NCI;
	ED_BOOLEAN ROT;
	ED_OCTET SI;

}	c_TSynchInd;
#define INIT_c_TSynchInd(sp) ED_RESET_MEM ((sp), sizeof (c_TSynchInd))
#define FREE_c_TSynchInd(sp)

/* Access member 'FreqParamLen' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_FreqParamLen(var) var

/* Access member 'FreqParamLen' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_FreqParamLen(var) (&var)

/* Access member 'MAIO' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_MAIO(var) var

/* Access member 'MAIO' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_MAIO(var) (&var)

/* Access member 'Mobile_Allocation' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_Mobile_Allocation(var) var

/* Access member 'Mobile_Allocation' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_Mobile_Allocation(var) (&var)

/* Access member 'ALPHA' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_ALPHA(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'GAMMA' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_P0(var) var

/* Access member 'P0' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_P0(var) (&var)

/* Access member 'POLLING' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_POLLING(var) var

/* Access member 'POLLING' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_POLLING(var) (&var)

/* Access member 'PR_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_PR_MODE(var) (&var)

/* Access member 'RLC_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_RLC_MODE(var) var

/* Access member 'RLC_MODE' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_RLC_MODE(var) (&var)

/* Access member 'TA_VALID' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_TA_VALID(var) var

/* Access member 'TA_VALID' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_TA_VALID(var) (&var)

/* Access member 'TBF_STARTING_TIME' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_TBF_STARTING_TIME(var) var

/* Access member 'TBF_STARTING_TIME' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_TBF_STARTING_TIME(var) (&var)

/* Access member 'TFI_ASSIGNMENT' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_TFI_ASSIGNMENT(var) var

/* Access member 'TFI_ASSIGNMENT' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_TFI_ASSIGNMENT(var) (&var)

/* Access member 'TIMING_ADVANCE_INDEX' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_TIMING_ADVANCE_INDEX(var) var

/* Access member 'TIMING_ADVANCE_INDEX' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_TIMING_ADVANCE_INDEX(var) (&var)

/* Access member 'TLLI' of type 'c_IA_Rest_Octets_PDlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt_TLLI(var) var

/* Access member 'TLLI' of type 'c_IA_Rest_Octets_PDlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt_TLLI(var) (&var)

/* Access member 'PDlAssgt' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_PDlAssgt(var) var

/* Access member 'PDlAssgt' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_PDlAssgt(var) (&var)

/* Access member 'ALLOCATION_BITMAP' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP(var) var

/* Access member 'ALLOCATION_BITMAP' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP(var) (&var)

/* Access member 'ALLOCATION_BITMAP_LENGTH' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP_LENGTH(var) var

/* Access member 'ALLOCATION_BITMAP_LENGTH' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP_LENGTH(var) (&var)

/* Access member 'ALPHA' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_ALPHA(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_CHANNEL_CODING_COMMAND(var) var

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_CHANNEL_CODING_COMMAND(var) (&var)

/* Access member 'Extended_RA' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_Extended_RA(var) var

/* Access member 'Extended_RA' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_Extended_RA(var) (&var)

/* Access member 'GAMMA' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_P0(var) var

/* Access member 'P0' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_P0(var) (&var)

/* Access member 'POLLING' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_POLLING(var) var

/* Access member 'POLLING' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_POLLING(var) (&var)

/* Access member 'PR_MODE' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_PR_MODE(var) (&var)

/* Access member 'TBF_STARTING_TIME' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_TBF_STARTING_TIME(var) var

/* Access member 'TBF_STARTING_TIME' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_TBF_STARTING_TIME(var) (&var)

/* Access member 'TFI_ASSIGNMENT' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_TFI_ASSIGNMENT(var) var

/* Access member 'TFI_ASSIGNMENT' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_TFI_ASSIGNMENT(var) (&var)

/* Access member 'TIMING_ADVANCE_INDEX' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_TIMING_ADVANCE_INDEX(var) var

/* Access member 'TIMING_ADVANCE_INDEX' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_TIMING_ADVANCE_INDEX(var) (&var)

/* Access member 'TLLI_BLOCK_CHANNEL_CODING' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_TLLI_BLOCK_CHANNEL_CODING(var) var

/* Access member 'TLLI_BLOCK_CHANNEL_CODING' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_TLLI_BLOCK_CHANNEL_CODING(var) (&var)

/* Access member 'USF' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_USF(var) var

/* Access member 'USF' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_USF(var) (&var)

/* Access member 'USF_GRANULARITY' of type 'c_IA_Rest_Octets_PUlAssgt' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt_USF_GRANULARITY(var) var

/* Access member 'USF_GRANULARITY' of type 'c_IA_Rest_Octets_PUlAssgt' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt_USF_GRANULARITY(var) (&var)

/* Access member 'PUlAssgt' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_PUlAssgt(var) var

/* Access member 'PUlAssgt' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_PUlAssgt(var) (&var)

/* Access member 'Extended_RA' of type 'c_IA_Rest_Octets_ScndPart' as a variable reference */
#define VAR_c_IA_Rest_Octets_ScndPart_Extended_RA(var) var

/* Access member 'Extended_RA' of type 'c_IA_Rest_Octets_ScndPart' as a pointer */
#define PTR_c_IA_Rest_Octets_ScndPart_Extended_RA(var) (&var)

/* Access member 'ScndPart' of type 'c_IA_Rest_Octets' as a variable reference */
#define VAR_c_IA_Rest_Octets_ScndPart(var) var

/* Access member 'ScndPart' of type 'c_IA_Rest_Octets' as a pointer */
#define PTR_c_IA_Rest_Octets_ScndPart(var) (&var)

/* DEFINITION OF BINARY c_IA_Rest_Octets_Mobile_Allocation */
typedef struct _c_IA_Rest_Octets_Mobile_Allocation {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_IA_Rest_Octets_Mobile_Allocation;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IA_Rest_Octets_Mobile_Allocation(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_IA_Rest_Octets_PDlAssgt */
typedef struct _c_IA_Rest_Octets_PDlAssgt {
	ED_LONG TLLI;
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_OCTET GAMMA;
	ED_BOOLEAN GAMMA_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN POLLING;
	ED_BOOLEAN POLLING_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN RLC_MODE;
	ED_BOOLEAN RLC_MODE_Present;
	ED_BOOLEAN TA_VALID;
	ED_BOOLEAN TA_VALID_Present;
	ED_BOOLEAN TBF_STARTING_TIME_Present;
	ED_OCTET TFI_ASSIGNMENT;
	ED_BOOLEAN TFI_ASSIGNMENT_Present;
	ED_OCTET TIMING_ADVANCE_INDEX;
	ED_BOOLEAN TIMING_ADVANCE_INDEX_Present;
	c_TStartingTime TBF_STARTING_TIME;
#ifdef __EGPRS__
// add by dingmx 20061031 for EDGE
//{ null | L 		-- Receiver compatible with earlier release
//| H 	-- Additions for R99
//	-- indicates EGPRS TBF mode, see 3GPP TS 04.60
//	< EGPRS Window Size : < EGPRS Window Size IE >>
//	< LINK_QUALITY_MEASUREMENT_MODE : bit (2) >
//	{ 0 | 1 < BEP_PERIOD2 : bit (4) > }
//	} ;
ED_OCTET EGPRS_Window_Size;
ED_BOOLEAN EGPRS_Window_Size_Present;
ED_OCTET LINK_QUALITY_MEASUREMENT_MODE;
ED_BOOLEAN LINK_QUALITY_MEASUREMENT_MODE_Present;
ED_OCTET BEP_PERIOD2;
ED_BOOLEAN BEP_PERIOD2_Present;
#else
#endif

} c_IA_Rest_Octets_PDlAssgt;

/* DEFINITION OF BINARY c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP */
typedef struct _c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_IA_Rest_Octets_PUlAssgt */
typedef struct _c_IA_Rest_Octets_PUlAssgt {
	ED_BOOLEAN ALLOCATION_BITMAP_Present;
	ED_OCTET ALLOCATION_BITMAP_LENGTH;
	ED_BOOLEAN ALLOCATION_BITMAP_LENGTH_Present;
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_OCTET CHANNEL_CODING_COMMAND;
	ED_BOOLEAN CHANNEL_CODING_COMMAND_Present;
	ED_OCTET Extended_RA;
	ED_BOOLEAN Extended_RA_Present;
	ED_OCTET GAMMA;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN POLLING;
	ED_BOOLEAN POLLING_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN TBF_STARTING_TIME_Present;
	ED_OCTET TFI_ASSIGNMENT;
	ED_BOOLEAN TFI_ASSIGNMENT_Present;
	ED_OCTET TIMING_ADVANCE_INDEX;
	ED_BOOLEAN TIMING_ADVANCE_INDEX_Present;
	ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING;
	ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING_Present;
	ED_OCTET USF;
	ED_BOOLEAN USF_Present;
	ED_BOOLEAN USF_GRANULARITY;
	ED_BOOLEAN USF_GRANULARITY_Present;
	c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP ALLOCATION_BITMAP;
	c_TStartingTime TBF_STARTING_TIME;

} c_IA_Rest_Octets_PUlAssgt;

/* DEFINITION OF SUB-STRUCTURE c_IA_Rest_Octets_ScndPart */
typedef struct _c_IA_Rest_Octets_ScndPart {
	ED_OCTET Extended_RA;
	ED_BOOLEAN Extended_RA_Present;

} c_IA_Rest_Octets_ScndPart;

#ifdef __EGPRS__
// add by dingmx 20061031 for EDGE
typedef struct 	_c_IA_Rest_Octets_EGPRS_PUlAssgt
{
	//< EGPRS Packet Uplink Assignment > : :=
	//< Extended RA : bit (5) >
	//{ 0 | 1 < Access Technologies Request : Access Technologies Request struct > }
    //<Access Technologies Request struct> ::= 
    //    <Access Technology Type : bit (4)>{ 0 | 1 <Access Technologies Request struct> };	
	ED_OCTET Extended_RA;
	ED_BOOLEAN	Extended_RA_Present;

	ED_BOOLEAN Access_Tech_Req_Present;//add by lincq
    ED_OCTET Access_Tech_Req_num;
 	ED_OCTET Access_Tech_Req[16];  //2^4 = 16
    //{ 1
    //< TFI_ASSIGNMENT : bit (5) >< POLLING : bit >
	ED_BOOLEAN	 TFI_ASSIGNMENT_Present;
	ED_OCTET    TFI_ASSIGNMENT;
	ED_BOOLEAN	 POLLING_Present;	
    ED_OCTET    POLLING;
    //{ 0 				-- Dynamic Allocation
    //< USF: bit (3) >< USF_GRANULARITY : bit >
    //{ 0 | 1 < P0 : bit (4) >< PR_MODE : bit (1) >}
    //| 1					-- Fixed Allocation
    //< ALLOCATION_BITMAP_LENGTH : bit (5) >
    //    < ALLOCATION_BITMAP : bit (val(ALLOCATION_BITMAP_LENGTH)) >
    //   { 0 | 1 < P0 : bit (4) >< BTS_PWR_CTRL_MODE : bit (1) >< PR_MODE : bit (1) > }
    // }
    ED_BOOLEAN  	USF_Present;
    ED_OCTET    USF;
    ED_BOOLEAN  	USF_GRANULARITY_Present;
    ED_OCTET    USF_GRANULARITY;
    ED_BOOLEAN P0_Present;
    ED_OCTET    P0;
    ED_BOOLEAN PR_MODE_Present;
    ED_OCTET    PR_MODE;
    ED_BOOLEAN   ALLOCATION_BITMAP_Present;
	c_BITMAP_64BIT	ALLOCATION_BITMAP;
    ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
    ED_OCTET BTS_PWR_CTRL_MODE;

    //< EGPRS CHANNEL_CODING_COMMAND : < EGPRS Modulation and Coding IE>>
    //< TLLI_BLOCK_CHANNEL_CODING : bit (1) >
    //{ 0 | 1 < BEP_PERIOD2 : bit (4) > }
    //< RESEGMENT : bit (1) >< EGPRS Window Size : < EGPRS Window Size IE>>
    ED_BOOLEAN EGPRS_CHANNEL_CODING_COMMAND_Present;
    ED_OCTET    EGPRS_CHANNEL_CODING_COMMAND;
    ED_BOOLEAN  TLLI_BLOCK_CHANNEL_CODING_Present;
	ED_OCTET    TLLI_BLOCK_CHANNEL_CODING; 
    ED_BOOLEAN  BEP_PERIOD2_Present;
    ED_OCTET    BEP_PERIOD2;
    ED_BOOLEAN RESEGMENT_Present;
    ED_OCTET RESEGMENT;
	ED_BOOLEAN	 EGPRS_Window_Size_Present;
    ED_OCTET    EGPRS_Window_Size;
	
    //{ 0 |1 < ALPHA : bit4>}< GAMMA : bit5>{ 0 | 1 < TIMING_ADVANCE_INDEX :bit4> }
    //{ 0 | 1 < TBF_STARTING_TIME : bit (16) > }
	ED_BOOLEAN ALPHA_Present;	
	ED_OCTET    ALPHA;		
	ED_BOOLEAN GAMMA_Present;
	ED_OCTET    GAMMA;		
    ED_BOOLEAN TIMING_ADVANCE_INDEX_Present;
    ED_OCTET TIMING_ADVANCE_INDEX;
    ED_BOOLEAN  TBF_STARTING_TIME_Present;
	c_TStartingTime  TBF_STARTING_TIME; //	    ED_SHORT   
	
    //| 0			-- Multi Block Allocation
    //        { 0 | 1 < ALPHA : bit (4) > }< GAMMA : bit (5) >
    //        < TBF_STARTING_TIME : bit (16) >
    //        < NUMBER OF RADIO BLOCKS ALLOCATED : bit (2) >
    //        { 0 | 1 < P0 : bit (4) >< BTS_PWR_CTRL_MODE : bit (1) >< PR_MODE : bit (1) > }
    //    } ;
    ED_BOOLEAN  NUMBER_OF_RADIO_BLOCKS_ALLOCATED_Present;
	ED_OCTET    NUMBER_OF_RADIO_BLOCKS_ALLOCATED;
}c_IA_Rest_Octets_EGPRS_PUlAssgt;

long DECODE_c_IA_Rest_Octets_EGPRS_PUlAssgt (const char* const Buffer, 
                              const long BitOffset, 
                              c_IA_Rest_Octets_EGPRS_PUlAssgt* const Destin,
                              long Length);

#else
#endif
// add by dingmx end
/*-----------------------------------*/
typedef struct _c_IA_Rest_Octets {
	ED_OCTET FreqParamLen;
	ED_BOOLEAN FreqParamLen_Present;
	ED_OCTET MAIO;
	ED_BOOLEAN MAIO_Present;
	ED_BOOLEAN Mobile_Allocation_Present;
	ED_BOOLEAN PDlAssgt_Present;
	ED_BOOLEAN PUlAssgt_Present;
	ED_BOOLEAN ScndPart_Present;
	c_IA_Rest_Octets_Mobile_Allocation Mobile_Allocation;
	c_IA_Rest_Octets_PDlAssgt PDlAssgt;
	c_IA_Rest_Octets_PUlAssgt PUlAssgt;
	c_IA_Rest_Octets_ScndPart ScndPart;
#ifdef __EGPRS__
// add by dingmx 20061031 for EDGE
	ED_BOOLEAN EGprs_Present;
	c_IA_Rest_Octets_EGPRS_PUlAssgt EGPRS_PUlAssgt; // add by dingmx
#else
#endif

}	c_IA_Rest_Octets;
#define INIT_c_IA_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_IA_Rest_Octets))
#define FREE_c_IA_Rest_Octets(sp)
#define SETPRESENT_c_IA_Rest_Octets_FreqParamLen(sp,present) ((sp)->FreqParamLen_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_FreqParamLen(sp) ((sp)->FreqParamLen_Present)
#define SETPRESENT_c_IA_Rest_Octets_MAIO(sp,present) ((sp)->MAIO_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_MAIO(sp) ((sp)->MAIO_Present)
#define SETPRESENT_c_IA_Rest_Octets_Mobile_Allocation(sp,present) ((sp)->Mobile_Allocation_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_Mobile_Allocation(sp) ((sp)->Mobile_Allocation_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt(sp,present) ((sp)->PDlAssgt_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt(sp) ((sp)->PDlAssgt_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt(sp,present) ((sp)->PUlAssgt_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt(sp) ((sp)->PUlAssgt_Present)
#define SETPRESENT_c_IA_Rest_Octets_ScndPart(sp,present) ((sp)->ScndPart_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_ScndPart(sp) ((sp)->ScndPart_Present)

/* Access member 'Extended_RA_1' of type 'c_IAX_Rest_Octets' as a variable reference */
#define VAR_c_IAX_Rest_Octets_Extended_RA_1(var) var

/* Access member 'Extended_RA_1' of type 'c_IAX_Rest_Octets' as a pointer */
#define PTR_c_IAX_Rest_Octets_Extended_RA_1(var) (&var)

/* Access member 'Extended_RA_2' of type 'c_IAX_Rest_Octets' as a variable reference */
#define VAR_c_IAX_Rest_Octets_Extended_RA_2(var) var

/* Access member 'Extended_RA_2' of type 'c_IAX_Rest_Octets' as a pointer */
#define PTR_c_IAX_Rest_Octets_Extended_RA_2(var) (&var)


/*-----------------------------------*/
typedef struct _c_IAX_Rest_Octets {
	ED_OCTET Extended_RA_1;
	ED_BOOLEAN Extended_RA_1_Present;
	ED_OCTET Extended_RA_2;
	ED_BOOLEAN Extended_RA_2_Present;

}	c_IAX_Rest_Octets;
#define INIT_c_IAX_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_IAX_Rest_Octets))
#define FREE_c_IAX_Rest_Octets(sp)
#define SETPRESENT_c_IAX_Rest_Octets_Extended_RA_1(sp,present) ((sp)->Extended_RA_1_Present = present)
#define GETPRESENT_c_IAX_Rest_Octets_Extended_RA_1(sp) ((sp)->Extended_RA_1_Present)
#define SETPRESENT_c_IAX_Rest_Octets_Extended_RA_2(sp,present) ((sp)->Extended_RA_2_Present = present)
#define GETPRESENT_c_IAX_Rest_Octets_Extended_RA_2(sp) ((sp)->Extended_RA_2_Present)

/* Access member 'ARFCN' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_ARFCN(var) (&var)

/* Access member 'CHANGE_MARK_1' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_CHANGE_MARK_1(var) var

/* Access member 'CHANGE_MARK_1' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_CHANGE_MARK_1(var) (&var)

/* Access member 'Channel_Type' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_Channel_Type(var) var

/* Access member 'Channel_Type' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_Channel_Type(var) (&var)

/* Access member 'HSN' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_HSN(var) var

/* Access member 'HSN' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_HSN(var) (&var)

/* Access member 'MAIO' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_MAIO(var) var

/* Access member 'MAIO' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_MAIO(var) (&var)

/* Access member 'MA_NB_IND' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_MA_NB_IND(var) var

/* Access member 'MA_NB_IND' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_MA_NB_IND(var) (&var)

/* Access member 'TN' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_TN(var) var

/* Access member 'TN' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_TN(var) (&var)

/* Access member 'TSC' of type 'c_Packet_Channel_Description' as a variable reference */
#define VAR_c_Packet_Channel_Description_TSC(var) var

/* Access member 'TSC' of type 'c_Packet_Channel_Description' as a pointer */
#define PTR_c_Packet_Channel_Description_TSC(var) (&var)


/*-----------------------------------*/
typedef struct _c_Packet_Channel_Description {
	ED_SHORT ARFCN;
	ED_BOOLEAN ARFCN_Present;
	ED_OCTET CHANGE_MARK_1;
	ED_BOOLEAN CHANGE_MARK_1_Present;
	ED_OCTET Channel_Type;
	ED_OCTET HSN;
	ED_BOOLEAN HSN_Present;
	ED_OCTET MAIO;
	ED_BOOLEAN MAIO_Present;
	ED_BOOLEAN MA_NB_IND;
	ED_BOOLEAN MA_NB_IND_Present;
	ED_OCTET TN;
	ED_OCTET TSC;

}	c_Packet_Channel_Description;
#define INIT_c_Packet_Channel_Description(sp) ED_RESET_MEM ((sp), sizeof (c_Packet_Channel_Description))
#define FREE_c_Packet_Channel_Description(sp)
#define SETPRESENT_c_Packet_Channel_Description_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_Packet_Channel_Description_ARFCN(sp) ((sp)->ARFCN_Present)
#define SETPRESENT_c_Packet_Channel_Description_CHANGE_MARK_1(sp,present) ((sp)->CHANGE_MARK_1_Present = present)
#define GETPRESENT_c_Packet_Channel_Description_CHANGE_MARK_1(sp) ((sp)->CHANGE_MARK_1_Present)
#define SETPRESENT_c_Packet_Channel_Description_HSN(sp,present) ((sp)->HSN_Present = present)
#define GETPRESENT_c_Packet_Channel_Description_HSN(sp) ((sp)->HSN_Present)
#define SETPRESENT_c_Packet_Channel_Description_MAIO(sp,present) ((sp)->MAIO_Present = present)
#define GETPRESENT_c_Packet_Channel_Description_MAIO(sp) ((sp)->MAIO_Present)
#define SETPRESENT_c_Packet_Channel_Description_MA_NB_IND(sp,present) ((sp)->MA_NB_IND_Present = present)
#define GETPRESENT_c_Packet_Channel_Description_MA_NB_IND(sp) ((sp)->MA_NB_IND_Present)

/* Access member 'ChDescAT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChDescAT(var) var

/* Access member 'ChDescAT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChDescAT(var) (&var)

/* Access member 'PowerCmd' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_PowerCmd(var) var

/* Access member 'PowerCmd' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_PowerCmd(var) (&var)

/* Access member 'FrqListAT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_FrqListAT(var) var

/* Access member 'FrqListAT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_FrqListAT(var) (&var)

/* Access member 'CellChDesc' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_CellChDesc(var) var

/* Access member 'CellChDesc' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_CellChDesc(var) (&var)

/* Access member 'MultiSlotCfgDesc' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_MultiSlotCfgDesc(var) var

/* Access member 'MultiSlotCfgDesc' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_MultiSlotCfgDesc(var) (&var)

/* Access member 'ChMode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChMode(var) var

/* Access member 'ChMode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChMode(var) (&var)

/* Access member 'ChSet2Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet2Mode(var) var

/* Access member 'ChSet2Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet2Mode(var) (&var)

/* Access member 'ChSet3Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet3Mode(var) var

/* Access member 'ChSet3Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet3Mode(var) (&var)

/* Access member 'ChSet4Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet4Mode(var) var

/* Access member 'ChSet4Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet4Mode(var) (&var)

/* Access member 'ChSet5Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet5Mode(var) var

/* Access member 'ChSet5Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet5Mode(var) (&var)

/* Access member 'ChSet6Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet6Mode(var) var

/* Access member 'ChSet6Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet6Mode(var) (&var)

/* Access member 'ChSet7Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet7Mode(var) var

/* Access member 'ChSet7Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet7Mode(var) (&var)

/* Access member 'ChSet8Mode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChSet8Mode(var) var

/* Access member 'ChSet8Mode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChSet8Mode(var) (&var)

/* Access member 'SecondChDescAT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_SecondChDescAT(var) var

/* Access member 'SecondChDescAT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_SecondChDescAT(var) (&var)

/* Access member 'SecondChMode' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_SecondChMode(var) var

/* Access member 'SecondChMode' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_SecondChMode(var) (&var)

/* Access member 'MobAllocAT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_MobAllocAT(var) var

/* Access member 'MobAllocAT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_MobAllocAT(var) (&var)

/* Access member 'ST' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ST(var) var

/* Access member 'ST' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ST(var) (&var)

/* Access member 'FrqListBT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_FrqListBT(var) var

/* Access member 'FrqListBT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_FrqListBT(var) (&var)

/* Access member 'ChDescBT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_ChDescBT(var) var

/* Access member 'ChDescBT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_ChDescBT(var) (&var)

/* Access member 'SecondChDescBT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_SecondChDescBT(var) var

/* Access member 'SecondChDescBT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_SecondChDescBT(var) (&var)

/* Access member 'FrqChnSeqBT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_FrqChnSeqBT(var) var

/* Access member 'FrqChnSeqBT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_FrqChnSeqBT(var) (&var)

/* Access member 'MobAllocBT' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_MobAllocBT(var) var

/* Access member 'MobAllocBT' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_MobAllocBT(var) (&var)

/* Access member 'CipModeSetting' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_CipModeSetting(var) var

/* Access member 'CipModeSetting' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_CipModeSetting(var) (&var)

/* Access member 'VgcsTargetModeInd' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_VgcsTargetModeInd(var) var

/* Access member 'VgcsTargetModeInd' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_VgcsTargetModeInd(var) (&var)

/* Access member 'MultiRateCfg' of type 'c_ASS_CMD' as a variable reference */
#define VAR_c_ASS_CMD_MultiRateCfg(var) var

/* Access member 'MultiRateCfg' of type 'c_ASS_CMD' as a pointer */
#define PTR_c_ASS_CMD_MultiRateCfg(var) (&var)

/* DEFINITION OF BINARY c_ASS_CMD_MobAllocAT */
typedef struct _c_ASS_CMD_MobAllocAT {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ASS_CMD_MobAllocAT;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ASS_CMD_MobAllocAT(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_ASS_CMD_MobAllocBT */
typedef struct _c_ASS_CMD_MobAllocBT {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ASS_CMD_MobAllocBT;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ASS_CMD_MobAllocBT(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_ASS_CMD_MultiRateCfg */
typedef struct _c_ASS_CMD_MultiRateCfg {
	ED_BYTE value [6]; /* Variable size; bits needed 48 */
	int usedBits;
} c_ASS_CMD_MultiRateCfg;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ASS_CMD_MultiRateCfg(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_ASS_CMD {
	ED_BOOLEAN FrqListAT_Present;
	ED_BOOLEAN CellChDesc_Present;
	ED_BOOLEAN MultiSlotCfgDesc_Present;
	ED_OCTET ChMode;
	ED_BOOLEAN ChMode_Present;
	ED_BOOLEAN ChSet2Mode_Present;
	ED_BOOLEAN ChSet3Mode_Present;
	ED_BOOLEAN ChSet4Mode_Present;
	ED_BOOLEAN ChSet5Mode_Present;
	ED_BOOLEAN ChSet6Mode_Present;
	ED_BOOLEAN ChSet7Mode_Present;
	ED_BOOLEAN ChSet8Mode_Present;
	ED_BOOLEAN SecondChDescAT_Present;
	ED_BOOLEAN SecondChMode_Present;
	ED_BOOLEAN MobAllocAT_Present;
	ED_BOOLEAN ST_Present;
	ED_BOOLEAN FrqListBT_Present;
	ED_BOOLEAN ChDescBT_Present;
	ED_BOOLEAN SecondChDescBT_Present;
	ED_BOOLEAN FrqChnSeqBT_Present;
	ED_BOOLEAN MobAllocBT_Present;
	ED_BOOLEAN CipModeSetting_Present;
	ED_BOOLEAN VgcsTargetModeInd_Present;
	ED_BOOLEAN MultiRateCfg_Present;
	c_TChDesc ChDescAT;
	c_TPowerCommand PowerCmd;
	c_TFrqList FrqListAT;
	c_TFrqList CellChDesc;
	c_ASS_CMD_MobAllocAT MobAllocAT;
	c_TStartingTime ST;
	c_TFrqList FrqListBT;
	c_TChDesc ChDescBT;
	c_TFrqChnSeq FrqChnSeqBT;
	c_ASS_CMD_MobAllocBT MobAllocBT;
	c_TCipherModeSetting CipModeSetting;
	c_ASS_CMD_MultiRateCfg MultiRateCfg;

}	c_ASS_CMD;
#define INIT_c_ASS_CMD(sp) ED_RESET_MEM ((sp), sizeof (c_ASS_CMD))
#define FREE_c_ASS_CMD(sp)
#define SETPRESENT_c_ASS_CMD_FrqListAT(sp,present) ((sp)->FrqListAT_Present = present)
#define GETPRESENT_c_ASS_CMD_FrqListAT(sp) ((sp)->FrqListAT_Present)
#define SETPRESENT_c_ASS_CMD_CellChDesc(sp,present) ((sp)->CellChDesc_Present = present)
#define GETPRESENT_c_ASS_CMD_CellChDesc(sp) ((sp)->CellChDesc_Present)
#define SETPRESENT_c_ASS_CMD_MultiSlotCfgDesc(sp,present) ((sp)->MultiSlotCfgDesc_Present = present)
#define GETPRESENT_c_ASS_CMD_MultiSlotCfgDesc(sp) ((sp)->MultiSlotCfgDesc_Present)
#define SETPRESENT_c_ASS_CMD_ChMode(sp,present) ((sp)->ChMode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChMode(sp) ((sp)->ChMode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet2Mode(sp,present) ((sp)->ChSet2Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet2Mode(sp) ((sp)->ChSet2Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet3Mode(sp,present) ((sp)->ChSet3Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet3Mode(sp) ((sp)->ChSet3Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet4Mode(sp,present) ((sp)->ChSet4Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet4Mode(sp) ((sp)->ChSet4Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet5Mode(sp,present) ((sp)->ChSet5Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet5Mode(sp) ((sp)->ChSet5Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet6Mode(sp,present) ((sp)->ChSet6Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet6Mode(sp) ((sp)->ChSet6Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet7Mode(sp,present) ((sp)->ChSet7Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet7Mode(sp) ((sp)->ChSet7Mode_Present)
#define SETPRESENT_c_ASS_CMD_ChSet8Mode(sp,present) ((sp)->ChSet8Mode_Present = present)
#define GETPRESENT_c_ASS_CMD_ChSet8Mode(sp) ((sp)->ChSet8Mode_Present)
#define SETPRESENT_c_ASS_CMD_SecondChDescAT(sp,present) ((sp)->SecondChDescAT_Present = present)
#define GETPRESENT_c_ASS_CMD_SecondChDescAT(sp) ((sp)->SecondChDescAT_Present)
#define SETPRESENT_c_ASS_CMD_SecondChMode(sp,present) ((sp)->SecondChMode_Present = present)
#define GETPRESENT_c_ASS_CMD_SecondChMode(sp) ((sp)->SecondChMode_Present)
#define SETPRESENT_c_ASS_CMD_MobAllocAT(sp,present) ((sp)->MobAllocAT_Present = present)
#define GETPRESENT_c_ASS_CMD_MobAllocAT(sp) ((sp)->MobAllocAT_Present)
#define SETPRESENT_c_ASS_CMD_ST(sp,present) ((sp)->ST_Present = present)
#define GETPRESENT_c_ASS_CMD_ST(sp) ((sp)->ST_Present)
#define SETPRESENT_c_ASS_CMD_FrqListBT(sp,present) ((sp)->FrqListBT_Present = present)
#define GETPRESENT_c_ASS_CMD_FrqListBT(sp) ((sp)->FrqListBT_Present)
#define SETPRESENT_c_ASS_CMD_ChDescBT(sp,present) ((sp)->ChDescBT_Present = present)
#define GETPRESENT_c_ASS_CMD_ChDescBT(sp) ((sp)->ChDescBT_Present)
#define SETPRESENT_c_ASS_CMD_SecondChDescBT(sp,present) ((sp)->SecondChDescBT_Present = present)
#define GETPRESENT_c_ASS_CMD_SecondChDescBT(sp) ((sp)->SecondChDescBT_Present)
#define SETPRESENT_c_ASS_CMD_FrqChnSeqBT(sp,present) ((sp)->FrqChnSeqBT_Present = present)
#define GETPRESENT_c_ASS_CMD_FrqChnSeqBT(sp) ((sp)->FrqChnSeqBT_Present)
#define SETPRESENT_c_ASS_CMD_MobAllocBT(sp,present) ((sp)->MobAllocBT_Present = present)
#define GETPRESENT_c_ASS_CMD_MobAllocBT(sp) ((sp)->MobAllocBT_Present)
#define SETPRESENT_c_ASS_CMD_CipModeSetting(sp,present) ((sp)->CipModeSetting_Present = present)
#define GETPRESENT_c_ASS_CMD_CipModeSetting(sp) ((sp)->CipModeSetting_Present)
#define SETPRESENT_c_ASS_CMD_VgcsTargetModeInd(sp,present) ((sp)->VgcsTargetModeInd_Present = present)
#define GETPRESENT_c_ASS_CMD_VgcsTargetModeInd(sp) ((sp)->VgcsTargetModeInd_Present)
#define SETPRESENT_c_ASS_CMD_MultiRateCfg(sp,present) ((sp)->MultiRateCfg_Present = present)
#define GETPRESENT_c_ASS_CMD_MultiRateCfg(sp) ((sp)->MultiRateCfg_Present)

/* Access member 'ChDesc' of type 'c_CHN_MOD_MODIFY' as a variable reference */
#define VAR_c_CHN_MOD_MODIFY_ChDesc(var) var

/* Access member 'ChDesc' of type 'c_CHN_MOD_MODIFY' as a pointer */
#define PTR_c_CHN_MOD_MODIFY_ChDesc(var) (&var)

/* Access member 'ChMode' of type 'c_CHN_MOD_MODIFY' as a variable reference */
#define VAR_c_CHN_MOD_MODIFY_ChMode(var) var

/* Access member 'ChMode' of type 'c_CHN_MOD_MODIFY' as a pointer */
#define PTR_c_CHN_MOD_MODIFY_ChMode(var) (&var)

/* Access member 'VgcsTargetModeInd' of type 'c_CHN_MOD_MODIFY' as a variable reference */
#define VAR_c_CHN_MOD_MODIFY_VgcsTargetModeInd(var) var

/* Access member 'VgcsTargetModeInd' of type 'c_CHN_MOD_MODIFY' as a pointer */
#define PTR_c_CHN_MOD_MODIFY_VgcsTargetModeInd(var) (&var)

/* Access member 'MultiRateCfg' of type 'c_CHN_MOD_MODIFY' as a variable reference */
#define VAR_c_CHN_MOD_MODIFY_MultiRateCfg(var) var

/* Access member 'MultiRateCfg' of type 'c_CHN_MOD_MODIFY' as a pointer */
#define PTR_c_CHN_MOD_MODIFY_MultiRateCfg(var) (&var)

/* DEFINITION OF BINARY c_CHN_MOD_MODIFY_MultiRateCfg */
typedef struct _c_CHN_MOD_MODIFY_MultiRateCfg {
	ED_BYTE value [6]; /* Variable size; bits needed 48 */
	int usedBits;
} c_CHN_MOD_MODIFY_MultiRateCfg;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_CHN_MOD_MODIFY_MultiRateCfg(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_CHN_MOD_MODIFY {
	ED_OCTET ChMode;
	ED_BOOLEAN VgcsTargetModeInd_Present;
	ED_BOOLEAN MultiRateCfg_Present;
	c_TChDesc ChDesc;
	c_CHN_MOD_MODIFY_MultiRateCfg MultiRateCfg;

}	c_CHN_MOD_MODIFY;
#define INIT_c_CHN_MOD_MODIFY(sp) ED_RESET_MEM ((sp), sizeof (c_CHN_MOD_MODIFY))
#define FREE_c_CHN_MOD_MODIFY(sp)
#define SETPRESENT_c_CHN_MOD_MODIFY_VgcsTargetModeInd(sp,present) ((sp)->VgcsTargetModeInd_Present = present)
#define GETPRESENT_c_CHN_MOD_MODIFY_VgcsTargetModeInd(sp) ((sp)->VgcsTargetModeInd_Present)
#define SETPRESENT_c_CHN_MOD_MODIFY_MultiRateCfg(sp,present) ((sp)->MultiRateCfg_Present = present)
#define GETPRESENT_c_CHN_MOD_MODIFY_MultiRateCfg(sp) ((sp)->MultiRateCfg_Present)

/* Access member 'RRCause' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_RRCause(var) var

/* Access member 'RRCause' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_RRCause(var) (&var)

/* Access member 'BARange' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_BARange(var) var

/* Access member 'BARange' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_BARange(var) (&var)

/* Access member 'GCDesc' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_GCDesc(var) var

/* Access member 'GCDesc' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_GCDesc(var) (&var)

/* Access member 'GCKN' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_GCKN(var) var

/* Access member 'GCKN' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_GCKN(var) (&var)

/* Access member 'GPRSResumption' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_GPRSResumption(var) var

/* Access member 'GPRSResumption' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_GPRSResumption(var) (&var)

/* Access member 'BAListPref' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_BAListPref(var) var

/* Access member 'BAListPref' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_BAListPref(var) (&var)

/* Access member 'UTRANFreqList' of type 'c_CHN_REL' as a variable reference */
#define VAR_c_CHN_REL_UTRANFreqList(var) var

/* Access member 'UTRANFreqList' of type 'c_CHN_REL' as a pointer */
#define PTR_c_CHN_REL_UTRANFreqList(var) (&var)

/* DEFINITION OF BINARY c_CHN_REL_BARange */
typedef struct _c_CHN_REL_BARange {
	ED_BYTE value [598]; /* Variable size; bits needed 4784 */
	int usedBits;
} c_CHN_REL_BARange;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_CHN_REL_BARange(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_CHN_REL {
	ED_OCTET RRCause;
	ED_BOOLEAN BARange_Present;
	ED_BOOLEAN GCDesc_Present;
	ED_BOOLEAN GCKN_Present;
	ED_OCTET GPRSResumption;
	ED_BOOLEAN GPRSResumption_Present;
	ED_BOOLEAN BAListPref_Present;
	ED_BOOLEAN UTRANFreqList_Present;
	c_CHN_REL_BARange BARange;

}	c_CHN_REL;
#define INIT_c_CHN_REL(sp) ED_RESET_MEM ((sp), sizeof (c_CHN_REL))
#define FREE_c_CHN_REL(sp)
#define SETPRESENT_c_CHN_REL_BARange(sp,present) ((sp)->BARange_Present = present)
#define GETPRESENT_c_CHN_REL_BARange(sp) ((sp)->BARange_Present)
#define SETPRESENT_c_CHN_REL_GCDesc(sp,present) ((sp)->GCDesc_Present = present)
#define GETPRESENT_c_CHN_REL_GCDesc(sp) ((sp)->GCDesc_Present)
#define SETPRESENT_c_CHN_REL_GCKN(sp,present) ((sp)->GCKN_Present = present)
#define GETPRESENT_c_CHN_REL_GCKN(sp) ((sp)->GCKN_Present)
#define SETPRESENT_c_CHN_REL_GPRSResumption(sp,present) ((sp)->GPRSResumption_Present = present)
#define GETPRESENT_c_CHN_REL_GPRSResumption(sp) ((sp)->GPRSResumption_Present)
#define SETPRESENT_c_CHN_REL_BAListPref(sp,present) ((sp)->BAListPref_Present = present)
#define GETPRESENT_c_CHN_REL_BAListPref(sp) ((sp)->BAListPref_Present)
#define SETPRESENT_c_CHN_REL_UTRANFreqList(sp,present) ((sp)->UTRANFreqList_Present = present)
#define GETPRESENT_c_CHN_REL_UTRANFreqList(sp) ((sp)->UTRANFreqList_Present)

/* Access member 'CipModSetting' of type 'c_CIP_MOD_CMD' as a variable reference */
#define VAR_c_CIP_MOD_CMD_CipModSetting(var) var

/* Access member 'CipModSetting' of type 'c_CIP_MOD_CMD' as a pointer */
#define PTR_c_CIP_MOD_CMD_CipModSetting(var) (&var)

/* Access member 'CipRsp' of type 'c_CIP_MOD_CMD' as a variable reference */
#define VAR_c_CIP_MOD_CMD_CipRsp(var) var

/* Access member 'CipRsp' of type 'c_CIP_MOD_CMD' as a pointer */
#define PTR_c_CIP_MOD_CMD_CipRsp(var) (&var)


/*-----------------------------------*/
typedef struct _c_CIP_MOD_CMD {
	ED_BOOLEAN CipRsp;
	c_TCipherModeSetting CipModSetting;

}	c_CIP_MOD_CMD;
#define INIT_c_CIP_MOD_CMD(sp) ED_RESET_MEM ((sp), sizeof (c_CIP_MOD_CMD))
#define FREE_c_CIP_MOD_CMD(sp)

/* Access member 'ChDesc' of type 'c_FRQ_RDF' as a variable reference */
#define VAR_c_FRQ_RDF_ChDesc(var) var

/* Access member 'ChDesc' of type 'c_FRQ_RDF' as a pointer */
#define PTR_c_FRQ_RDF_ChDesc(var) (&var)

/* Access member 'MobAlloc' of type 'c_FRQ_RDF' as a variable reference */
#define VAR_c_FRQ_RDF_MobAlloc(var) var

/* Access member 'MobAlloc' of type 'c_FRQ_RDF' as a pointer */
#define PTR_c_FRQ_RDF_MobAlloc(var) (&var)

/* Access member 'ST' of type 'c_FRQ_RDF' as a variable reference */
#define VAR_c_FRQ_RDF_ST(var) var

/* Access member 'ST' of type 'c_FRQ_RDF' as a pointer */
#define PTR_c_FRQ_RDF_ST(var) (&var)

/* Access member 'CellChDesc' of type 'c_FRQ_RDF' as a variable reference */
#define VAR_c_FRQ_RDF_CellChDesc(var) var

/* Access member 'CellChDesc' of type 'c_FRQ_RDF' as a pointer */
#define PTR_c_FRQ_RDF_CellChDesc(var) (&var)

/* DEFINITION OF BINARY c_FRQ_RDF_MobAlloc */
typedef struct _c_FRQ_RDF_MobAlloc {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_FRQ_RDF_MobAlloc;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_FRQ_RDF_MobAlloc(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_FRQ_RDF {
	ED_BOOLEAN CellChDesc_Present;
	c_TChDesc ChDesc;
	c_FRQ_RDF_MobAlloc MobAlloc;
	c_TStartingTime ST;
	c_TFrqList CellChDesc;

}	c_FRQ_RDF;
#define INIT_c_FRQ_RDF(sp) ED_RESET_MEM ((sp), sizeof (c_FRQ_RDF))
#define FREE_c_FRQ_RDF(sp)
#define SETPRESENT_c_FRQ_RDF_CellChDesc(sp,present) ((sp)->CellChDesc_Present = present)
#define GETPRESENT_c_FRQ_RDF_CellChDesc(sp) ((sp)->CellChDesc_Present)

/* Access member 'CellDesc' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_CellDesc(var) var

/* Access member 'CellDesc' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_CellDesc(var) (&var)

/* Access member 'ChDescAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChDescAT(var) var

/* Access member 'ChDescAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChDescAT(var) (&var)

/* Access member 'HovRef' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_HovRef(var) var

/* Access member 'HovRef' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_HovRef(var) (&var)

/* Access member 'PowerCmd' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_PowerCmd(var) var

/* Access member 'PowerCmd' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_PowerCmd(var) (&var)

/* Access member 'SynchInd' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_SynchInd(var) var

/* Access member 'SynchInd' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_SynchInd(var) (&var)

/* Access member 'FrqShortListAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqShortListAT(var) var

/* Access member 'FrqShortListAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqShortListAT(var) (&var)

/* Access member 'FrqListAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqListAT(var) var

/* Access member 'FrqListAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqListAT(var) (&var)

/* Access member 'CellChDesc' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_CellChDesc(var) var

/* Access member 'CellChDesc' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_CellChDesc(var) (&var)

/* Access member 'MultiSlotCfgDesc' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_MultiSlotCfgDesc(var) var

/* Access member 'MultiSlotCfgDesc' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_MultiSlotCfgDesc(var) (&var)

/* Access member 'ChMode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChMode(var) var

/* Access member 'ChMode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChMode(var) (&var)

/* Access member 'ChSet2Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet2Mode(var) var

/* Access member 'ChSet2Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet2Mode(var) (&var)

/* Access member 'ChSet3Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet3Mode(var) var

/* Access member 'ChSet3Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet3Mode(var) (&var)

/* Access member 'ChSet4Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet4Mode(var) var

/* Access member 'ChSet4Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet4Mode(var) (&var)

/* Access member 'ChSet5Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet5Mode(var) var

/* Access member 'ChSet5Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet5Mode(var) (&var)

/* Access member 'ChSet6Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet6Mode(var) var

/* Access member 'ChSet6Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet6Mode(var) (&var)

/* Access member 'ChSet7Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet7Mode(var) var

/* Access member 'ChSet7Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet7Mode(var) (&var)

/* Access member 'ChSet8Mode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChSet8Mode(var) var

/* Access member 'ChSet8Mode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChSet8Mode(var) (&var)

/* Access member 'SecondChDescAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_SecondChDescAT(var) var

/* Access member 'SecondChDescAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_SecondChDescAT(var) (&var)

/* Access member 'SecondChMode' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_SecondChMode(var) var

/* Access member 'SecondChMode' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_SecondChMode(var) (&var)

/* Access member 'FrqChnSeqAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqChnSeqAT(var) var

/* Access member 'FrqChnSeqAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqChnSeqAT(var) (&var)

/* Access member 'MobAllocAT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_MobAllocAT(var) var

/* Access member 'MobAllocAT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_MobAllocAT(var) (&var)

/* Access member 'ST' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ST(var) var

/* Access member 'ST' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ST(var) (&var)

/* Access member 'RTD' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_RTD(var) var

/* Access member 'RTD' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_RTD(var) (&var)

/* Access member 'TimAdv' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_TimAdv(var) var

/* Access member 'TimAdv' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_TimAdv(var) (&var)

/* Access member 'FrqShortListBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqShortListBT(var) var

/* Access member 'FrqShortListBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqShortListBT(var) (&var)

/* Access member 'FrqListBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqListBT(var) var

/* Access member 'FrqListBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqListBT(var) (&var)

/* Access member 'ChDescBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_ChDescBT(var) var

/* Access member 'ChDescBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_ChDescBT(var) (&var)

/* Access member 'SecondChDescBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_SecondChDescBT(var) var

/* Access member 'SecondChDescBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_SecondChDescBT(var) (&var)

/* Access member 'FrqChnSeqBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_FrqChnSeqBT(var) var

/* Access member 'FrqChnSeqBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_FrqChnSeqBT(var) (&var)

/* Access member 'MobAllocBT' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_MobAllocBT(var) var

/* Access member 'MobAllocBT' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_MobAllocBT(var) (&var)

/* Access member 'CipModeSetting' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_CipModeSetting(var) var

/* Access member 'CipModeSetting' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_CipModeSetting(var) (&var)

/* Access member 'VgcsTargetModeInd' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_VgcsTargetModeInd(var) var

/* Access member 'VgcsTargetModeInd' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_VgcsTargetModeInd(var) (&var)

/* Access member 'MultiRateCfg' of type 'c_HOV_CMD' as a variable reference */
#define VAR_c_HOV_CMD_MultiRateCfg(var) var

/* Access member 'MultiRateCfg' of type 'c_HOV_CMD' as a pointer */
#define PTR_c_HOV_CMD_MultiRateCfg(var) (&var)

/* DEFINITION OF BINARY c_HOV_CMD_MobAllocAT */
typedef struct _c_HOV_CMD_MobAllocAT {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_HOV_CMD_MobAllocAT;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_HOV_CMD_MobAllocAT(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_HOV_CMD_MobAllocBT */
typedef struct _c_HOV_CMD_MobAllocBT {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_HOV_CMD_MobAllocBT;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_HOV_CMD_MobAllocBT(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_HOV_CMD_MultiRateCfg */
typedef struct _c_HOV_CMD_MultiRateCfg {
	ED_BYTE value [6]; /* Variable size; bits needed 48 */
	int usedBits;
} c_HOV_CMD_MultiRateCfg;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_HOV_CMD_MultiRateCfg(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_HOV_CMD {
	ED_OCTET HovRef;
	ED_BOOLEAN SynchInd_Present;
	ED_BOOLEAN FrqShortListAT_Present;
	ED_BOOLEAN FrqListAT_Present;
	ED_BOOLEAN CellChDesc_Present;
	ED_BOOLEAN MultiSlotCfgDesc_Present;
	ED_OCTET ChMode;
	ED_BOOLEAN ChMode_Present;
	ED_BOOLEAN ChSet2Mode_Present;
	ED_BOOLEAN ChSet3Mode_Present;
	ED_BOOLEAN ChSet4Mode_Present;
	ED_BOOLEAN ChSet5Mode_Present;
	ED_BOOLEAN ChSet6Mode_Present;
	ED_BOOLEAN ChSet7Mode_Present;
	ED_BOOLEAN ChSet8Mode_Present;
	ED_BOOLEAN SecondChDescAT_Present;
	ED_BOOLEAN SecondChMode_Present;
	ED_BOOLEAN FrqChnSeqAT_Present;
	ED_BOOLEAN MobAllocAT_Present;
	ED_BOOLEAN ST_Present;
	ED_OCTET RTD;
	ED_BOOLEAN RTD_Present;
	ED_OCTET TimAdv;
	ED_BOOLEAN TimAdv_Present;
	ED_BOOLEAN FrqShortListBT_Present;
	ED_BOOLEAN FrqListBT_Present;
	ED_BOOLEAN ChDescBT_Present;
	ED_BOOLEAN SecondChDescBT_Present;
	ED_BOOLEAN FrqChnSeqBT_Present;
	ED_BOOLEAN MobAllocBT_Present;
	ED_BOOLEAN CipModeSetting_Present;
	ED_BOOLEAN VgcsTargetModeInd_Present;
	ED_BOOLEAN MultiRateCfg_Present;
	c_TCellDesc CellDesc;
	c_TChDesc ChDescAT;
	c_TPowerCommandAccType PowerCmd;
	c_TSynchInd SynchInd;
	c_TFrqList FrqShortListAT;
	c_TFrqList FrqListAT;
	c_TFrqList CellChDesc;
	c_TFrqChnSeq FrqChnSeqAT;
	c_HOV_CMD_MobAllocAT MobAllocAT;
	c_TStartingTime ST;
	c_TFrqList FrqShortListBT;
	c_TFrqList FrqListBT;
	c_TChDesc ChDescBT;
	c_TFrqChnSeq FrqChnSeqBT;
	c_HOV_CMD_MobAllocBT MobAllocBT;
	c_TCipherModeSetting CipModeSetting;
	c_HOV_CMD_MultiRateCfg MultiRateCfg;

}	c_HOV_CMD;
#define INIT_c_HOV_CMD(sp) ED_RESET_MEM ((sp), sizeof (c_HOV_CMD))
#define FREE_c_HOV_CMD(sp)
#define SETPRESENT_c_HOV_CMD_SynchInd(sp,present) ((sp)->SynchInd_Present = present)
#define GETPRESENT_c_HOV_CMD_SynchInd(sp) ((sp)->SynchInd_Present)
#define SETPRESENT_c_HOV_CMD_FrqShortListAT(sp,present) ((sp)->FrqShortListAT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqShortListAT(sp) ((sp)->FrqShortListAT_Present)
#define SETPRESENT_c_HOV_CMD_FrqListAT(sp,present) ((sp)->FrqListAT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqListAT(sp) ((sp)->FrqListAT_Present)
#define SETPRESENT_c_HOV_CMD_CellChDesc(sp,present) ((sp)->CellChDesc_Present = present)
#define GETPRESENT_c_HOV_CMD_CellChDesc(sp) ((sp)->CellChDesc_Present)
#define SETPRESENT_c_HOV_CMD_MultiSlotCfgDesc(sp,present) ((sp)->MultiSlotCfgDesc_Present = present)
#define GETPRESENT_c_HOV_CMD_MultiSlotCfgDesc(sp) ((sp)->MultiSlotCfgDesc_Present)
#define SETPRESENT_c_HOV_CMD_ChMode(sp,present) ((sp)->ChMode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChMode(sp) ((sp)->ChMode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet2Mode(sp,present) ((sp)->ChSet2Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet2Mode(sp) ((sp)->ChSet2Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet3Mode(sp,present) ((sp)->ChSet3Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet3Mode(sp) ((sp)->ChSet3Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet4Mode(sp,present) ((sp)->ChSet4Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet4Mode(sp) ((sp)->ChSet4Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet5Mode(sp,present) ((sp)->ChSet5Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet5Mode(sp) ((sp)->ChSet5Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet6Mode(sp,present) ((sp)->ChSet6Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet6Mode(sp) ((sp)->ChSet6Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet7Mode(sp,present) ((sp)->ChSet7Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet7Mode(sp) ((sp)->ChSet7Mode_Present)
#define SETPRESENT_c_HOV_CMD_ChSet8Mode(sp,present) ((sp)->ChSet8Mode_Present = present)
#define GETPRESENT_c_HOV_CMD_ChSet8Mode(sp) ((sp)->ChSet8Mode_Present)
#define SETPRESENT_c_HOV_CMD_SecondChDescAT(sp,present) ((sp)->SecondChDescAT_Present = present)
#define GETPRESENT_c_HOV_CMD_SecondChDescAT(sp) ((sp)->SecondChDescAT_Present)
#define SETPRESENT_c_HOV_CMD_SecondChMode(sp,present) ((sp)->SecondChMode_Present = present)
#define GETPRESENT_c_HOV_CMD_SecondChMode(sp) ((sp)->SecondChMode_Present)
#define SETPRESENT_c_HOV_CMD_FrqChnSeqAT(sp,present) ((sp)->FrqChnSeqAT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqChnSeqAT(sp) ((sp)->FrqChnSeqAT_Present)
#define SETPRESENT_c_HOV_CMD_MobAllocAT(sp,present) ((sp)->MobAllocAT_Present = present)
#define GETPRESENT_c_HOV_CMD_MobAllocAT(sp) ((sp)->MobAllocAT_Present)
#define SETPRESENT_c_HOV_CMD_ST(sp,present) ((sp)->ST_Present = present)
#define GETPRESENT_c_HOV_CMD_ST(sp) ((sp)->ST_Present)
#define SETPRESENT_c_HOV_CMD_RTD(sp,present) ((sp)->RTD_Present = present)
#define GETPRESENT_c_HOV_CMD_RTD(sp) ((sp)->RTD_Present)
#define SETPRESENT_c_HOV_CMD_TimAdv(sp,present) ((sp)->TimAdv_Present = present)
#define GETPRESENT_c_HOV_CMD_TimAdv(sp) ((sp)->TimAdv_Present)
#define SETPRESENT_c_HOV_CMD_FrqShortListBT(sp,present) ((sp)->FrqShortListBT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqShortListBT(sp) ((sp)->FrqShortListBT_Present)
#define SETPRESENT_c_HOV_CMD_FrqListBT(sp,present) ((sp)->FrqListBT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqListBT(sp) ((sp)->FrqListBT_Present)
#define SETPRESENT_c_HOV_CMD_ChDescBT(sp,present) ((sp)->ChDescBT_Present = present)
#define GETPRESENT_c_HOV_CMD_ChDescBT(sp) ((sp)->ChDescBT_Present)
#define SETPRESENT_c_HOV_CMD_SecondChDescBT(sp,present) ((sp)->SecondChDescBT_Present = present)
#define GETPRESENT_c_HOV_CMD_SecondChDescBT(sp) ((sp)->SecondChDescBT_Present)
#define SETPRESENT_c_HOV_CMD_FrqChnSeqBT(sp,present) ((sp)->FrqChnSeqBT_Present = present)
#define GETPRESENT_c_HOV_CMD_FrqChnSeqBT(sp) ((sp)->FrqChnSeqBT_Present)
#define SETPRESENT_c_HOV_CMD_MobAllocBT(sp,present) ((sp)->MobAllocBT_Present = present)
#define GETPRESENT_c_HOV_CMD_MobAllocBT(sp) ((sp)->MobAllocBT_Present)
#define SETPRESENT_c_HOV_CMD_CipModeSetting(sp,present) ((sp)->CipModeSetting_Present = present)
#define GETPRESENT_c_HOV_CMD_CipModeSetting(sp) ((sp)->CipModeSetting_Present)
#define SETPRESENT_c_HOV_CMD_VgcsTargetModeInd(sp,present) ((sp)->VgcsTargetModeInd_Present = present)
#define GETPRESENT_c_HOV_CMD_VgcsTargetModeInd(sp) ((sp)->VgcsTargetModeInd_Present)
#define SETPRESENT_c_HOV_CMD_MultiRateCfg(sp,present) ((sp)->MultiRateCfg_Present = present)
#define GETPRESENT_c_HOV_CMD_MultiRateCfg(sp) ((sp)->MultiRateCfg_Present)

/* Access member 'TimAdv' of type 'c_PHY_INFO' as a variable reference */
#define VAR_c_PHY_INFO_TimAdv(var) var

/* Access member 'TimAdv' of type 'c_PHY_INFO' as a pointer */
#define PTR_c_PHY_INFO_TimAdv(var) (&var)


/*-----------------------------------*/
typedef struct _c_PHY_INFO {
	ED_OCTET TimAdv;

}	c_PHY_INFO;
#define INIT_c_PHY_INFO(sp) ED_RESET_MEM ((sp), sizeof (c_PHY_INFO))
#define FREE_c_PHY_INFO(sp)

/* Access member 'PageMode' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_PageMode(var) var

/* Access member 'PageMode' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_PageMode(var) (&var)

/* Access member 'DediOrTbf' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_DediOrTbf(var) var

/* Access member 'DediOrTbf' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_DediOrTbf(var) (&var)

/* Access member 'ChDesc' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_ChDesc(var) var

/* Access member 'ChDesc' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_ChDesc(var) (&var)

/* Access member 'PChDesc' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_PChDesc(var) var

/* Access member 'PChDesc' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_PChDesc(var) (&var)

/* Access member 'ReqRef' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_ReqRef(var) var

/* Access member 'ReqRef' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_ReqRef(var) (&var)

/* Access member 'TimAdv' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_TimAdv(var) var

/* Access member 'TimAdv' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_TimAdv(var) (&var)

/* Access member 'MobAlloc' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_MobAlloc(var) var

/* Access member 'MobAlloc' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_MobAlloc(var) (&var)

/* Access member 'ST' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_ST(var) var

/* Access member 'ST' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_ST(var) (&var)

/* Access member 'EndOfL2Length' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_EndOfL2Length(var) var

/* Access member 'EndOfL2Length' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_EndOfL2Length(var) (&var)

/* Access member 'IARestOctets' of type 'c_IMM_ASS' as a variable reference */
#define VAR_c_IMM_ASS_IARestOctets(var) var

/* Access member 'IARestOctets' of type 'c_IMM_ASS' as a pointer */
#define PTR_c_IMM_ASS_IARestOctets(var) (&var)

/* DEFINITION OF BINARY c_IMM_ASS_MobAlloc */
typedef struct _c_IMM_ASS_MobAlloc {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_IMM_ASS_MobAlloc;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IMM_ASS_MobAlloc(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_IMM_ASS {
	ED_BOOLEAN ChDesc_Present;
	ED_BOOLEAN PChDesc_Present;
	ED_OCTET TimAdv;
	ED_BOOLEAN ST_Present;
	c_TDedicatedModeOrTbf DediOrTbf;
	c_TChDesc ChDesc;
	c_Packet_Channel_Description PChDesc;
	c_TReqRef ReqRef;
	c_IMM_ASS_MobAlloc MobAlloc;
	c_TStartingTime ST;
	c_IA_Rest_Octets IARestOctets;

}	c_IMM_ASS;
#define INIT_c_IMM_ASS(sp) ED_RESET_MEM ((sp), sizeof (c_IMM_ASS))
#define FREE_c_IMM_ASS(sp)
#define SETPRESENT_c_IMM_ASS_ChDesc(sp,present) ((sp)->ChDesc_Present = present)
#define GETPRESENT_c_IMM_ASS_ChDesc(sp) ((sp)->ChDesc_Present)
#define SETPRESENT_c_IMM_ASS_PChDesc(sp,present) ((sp)->PChDesc_Present = present)
#define GETPRESENT_c_IMM_ASS_PChDesc(sp) ((sp)->PChDesc_Present)
#define SETPRESENT_c_IMM_ASS_ST(sp,present) ((sp)->ST_Present = present)
#define GETPRESENT_c_IMM_ASS_ST(sp) ((sp)->ST_Present)

/* Access member 'PageMode' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_PageMode(var) var

/* Access member 'PageMode' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_PageMode(var) (&var)

/* Access member 'SpareHalfOctet' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_SpareHalfOctet(var) var

/* Access member 'SpareHalfOctet' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_SpareHalfOctet(var) (&var)

/* Access member 'ChDesc1' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_ChDesc1(var) var

/* Access member 'ChDesc1' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_ChDesc1(var) (&var)

/* Access member 'ReqRef1' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_ReqRef1(var) var

/* Access member 'ReqRef1' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_ReqRef1(var) (&var)

/* Access member 'TimAdv1' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_TimAdv1(var) var

/* Access member 'TimAdv1' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_TimAdv1(var) (&var)

/* Access member 'ChDesc2' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_ChDesc2(var) var

/* Access member 'ChDesc2' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_ChDesc2(var) (&var)

/* Access member 'ReqRef2' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_ReqRef2(var) var

/* Access member 'ReqRef2' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_ReqRef2(var) (&var)

/* Access member 'TimAdv2' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_TimAdv2(var) var

/* Access member 'TimAdv2' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_TimAdv2(var) (&var)

/* Access member 'MobAlloc' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_MobAlloc(var) var

/* Access member 'MobAlloc' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_MobAlloc(var) (&var)

/* Access member 'ST' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_ST(var) var

/* Access member 'ST' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_ST(var) (&var)

/* Access member 'EndOfL2Length' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_EndOfL2Length(var) var

/* Access member 'EndOfL2Length' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_EndOfL2Length(var) (&var)

/* Access member 'IAXRestOctets' of type 'c_IMM_ASS_EXT' as a variable reference */
#define VAR_c_IMM_ASS_EXT_IAXRestOctets(var) var

/* Access member 'IAXRestOctets' of type 'c_IMM_ASS_EXT' as a pointer */
#define PTR_c_IMM_ASS_EXT_IAXRestOctets(var) (&var)

/* DEFINITION OF BINARY c_IMM_ASS_EXT_MobAlloc */
typedef struct _c_IMM_ASS_EXT_MobAlloc {
	ED_BYTE value [4]; /* Variable size; bits needed 32 */
	int usedBits;
} c_IMM_ASS_EXT_MobAlloc;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_IMM_ASS_EXT_MobAlloc(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_IMM_ASS_EXT {
	ED_OCTET TimAdv1;
	ED_OCTET TimAdv2;
	ED_BOOLEAN ST_Present;
	c_TChDesc ChDesc1;
	c_TReqRef ReqRef1;
	c_TChDesc ChDesc2;
	c_TReqRef ReqRef2;
	c_IMM_ASS_EXT_MobAlloc MobAlloc;
	c_TStartingTime ST;
	c_IAX_Rest_Octets IAXRestOctets;

}	c_IMM_ASS_EXT;
#define INIT_c_IMM_ASS_EXT(sp) ED_RESET_MEM ((sp), sizeof (c_IMM_ASS_EXT))
#define FREE_c_IMM_ASS_EXT(sp)
#define SETPRESENT_c_IMM_ASS_EXT_ST(sp,present) ((sp)->ST_Present = present)
#define GETPRESENT_c_IMM_ASS_EXT_ST(sp) ((sp)->ST_Present)

/* Access member 'PageMode' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_PageMode(var) var

/* Access member 'PageMode' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_PageMode(var) (&var)

/* Access member 'SpareHalfOctet' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_SpareHalfOctet(var) var

/* Access member 'SpareHalfOctet' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_SpareHalfOctet(var) (&var)

/* Access member 'ReqRef1' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_ReqRef1(var) var

/* Access member 'ReqRef1' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_ReqRef1(var) (&var)

/* Access member 'WaitInd1' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_WaitInd1(var) var

/* Access member 'WaitInd1' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_WaitInd1(var) (&var)

/* Access member 'ReqRef2' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_ReqRef2(var) var

/* Access member 'ReqRef2' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_ReqRef2(var) (&var)

/* Access member 'WaitInd2' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_WaitInd2(var) var

/* Access member 'WaitInd2' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_WaitInd2(var) (&var)

/* Access member 'ReqRef3' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_ReqRef3(var) var

/* Access member 'ReqRef3' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_ReqRef3(var) (&var)

/* Access member 'WaitInd3' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_WaitInd3(var) var

/* Access member 'WaitInd3' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_WaitInd3(var) (&var)

/* Access member 'ReqRef4' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_ReqRef4(var) var

/* Access member 'ReqRef4' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_ReqRef4(var) (&var)

/* Access member 'WaitInd4' of type 'c_IMM_ASS_REJ' as a variable reference */
#define VAR_c_IMM_ASS_REJ_WaitInd4(var) var

/* Access member 'WaitInd4' of type 'c_IMM_ASS_REJ' as a pointer */
#define PTR_c_IMM_ASS_REJ_WaitInd4(var) (&var)
/*-----------------------------------*/
typedef struct _c_IAR_Rest_Octets {
	ED_OCTET Extended_RA_1;
	ED_BOOLEAN Extended_RA_1_Present;
	ED_OCTET Extended_RA_2;
	ED_BOOLEAN Extended_RA_2_Present;
	ED_OCTET Extended_RA_3;
	ED_BOOLEAN Extended_RA_3_Present;
	ED_OCTET Extended_RA_4;
	ED_BOOLEAN Extended_RA_4_Present;

}	c_IAR_Rest_Octets;

#ifndef __EGPRS__
typedef struct _c_IMM_ASS_REJ {
	ED_OCTET WaitInd1;
	ED_OCTET WaitInd2;
	ED_OCTET WaitInd3;
	ED_OCTET WaitInd4;
	c_TReqRef ReqRef1;
	c_TReqRef ReqRef2;
	c_TReqRef ReqRef3;
	c_TReqRef ReqRef4;

}c_IMM_ASS_REJ;
#else
typedef struct _c_IMM_ASS_REJ {
	ED_OCTET WaitInd1;
	ED_OCTET WaitInd2;
	ED_OCTET WaitInd3;
	ED_OCTET WaitInd4;
	c_TReqRef ReqRef1;
	c_TReqRef ReqRef2;
	c_TReqRef ReqRef3;
	c_TReqRef ReqRef4;
  c_IAR_Rest_Octets IAR_Rest_Octets;
}c_IMM_ASS_REJ;
#endif
#define INIT_c_IMM_ASS_REJ(sp) ED_RESET_MEM ((sp), sizeof (c_IMM_ASS_REJ))
#define FREE_c_IMM_ASS_REJ(sp)

/* Access member 'Extended_RA_1' of type 'c_IAR_Rest_Octets' as a variable reference */
#define VAR_c_IAR_Rest_Octets_Extended_RA_1(var) var

/* Access member 'Extended_RA_1' of type 'c_IAR_Rest_Octets' as a pointer */
#define PTR_c_IAR_Rest_Octets_Extended_RA_1(var) (&var)

/* Access member 'Extended_RA_2' of type 'c_IAR_Rest_Octets' as a variable reference */
#define VAR_c_IAR_Rest_Octets_Extended_RA_2(var) var

/* Access member 'Extended_RA_2' of type 'c_IAR_Rest_Octets' as a pointer */
#define PTR_c_IAR_Rest_Octets_Extended_RA_2(var) (&var)

/* Access member 'Extended_RA_3' of type 'c_IAR_Rest_Octets' as a variable reference */
#define VAR_c_IAR_Rest_Octets_Extended_RA_3(var) var

/* Access member 'Extended_RA_3' of type 'c_IAR_Rest_Octets' as a pointer */
#define PTR_c_IAR_Rest_Octets_Extended_RA_3(var) (&var)

/* Access member 'Extended_RA_4' of type 'c_IAR_Rest_Octets' as a variable reference */
#define VAR_c_IAR_Rest_Octets_Extended_RA_4(var) var

/* Access member 'Extended_RA_4' of type 'c_IAR_Rest_Octets' as a pointer */
#define PTR_c_IAR_Rest_Octets_Extended_RA_4(var) (&var)

/* this struct shall be defined before it is called.
-----------------------------------*/
/*
typedef struct _c_IAR_Rest_Octets {
	ED_OCTET Extended_RA_1;
	ED_BOOLEAN Extended_RA_1_Present;
	ED_OCTET Extended_RA_2;
	ED_BOOLEAN Extended_RA_2_Present;
	ED_OCTET Extended_RA_3;
	ED_BOOLEAN Extended_RA_3_Present;
	ED_OCTET Extended_RA_4;
	ED_BOOLEAN Extended_RA_4_Present;

}	c_IAR_Rest_Octets;
*/
  


#define INIT_c_IAR_Rest_Octets(sp) ED_RESET_MEM ((sp), sizeof (c_IAR_Rest_Octets))
#define FREE_c_IAR_Rest_Octets(sp)
#define SETPRESENT_c_IAR_Rest_Octets_Extended_RA_1(sp,present) ((sp)->Extended_RA_1_Present = present)
#define GETPRESENT_c_IAR_Rest_Octets_Extended_RA_1(sp) ((sp)->Extended_RA_1_Present)
#define SETPRESENT_c_IAR_Rest_Octets_Extended_RA_2(sp,present) ((sp)->Extended_RA_2_Present = present)
#define GETPRESENT_c_IAR_Rest_Octets_Extended_RA_2(sp) ((sp)->Extended_RA_2_Present)
#define SETPRESENT_c_IAR_Rest_Octets_Extended_RA_3(sp,present) ((sp)->Extended_RA_3_Present = present)
#define GETPRESENT_c_IAR_Rest_Octets_Extended_RA_3(sp) ((sp)->Extended_RA_3_Present)
#define SETPRESENT_c_IAR_Rest_Octets_Extended_RA_4(sp,present) ((sp)->Extended_RA_4_Present = present)
#define GETPRESENT_c_IAR_Rest_Octets_Extended_RA_4(sp) ((sp)->Extended_RA_4_Present)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'CTRL_ACK' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_CTRL_ACK(var) var

/* Access member 'CTRL_ACK' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_CTRL_ACK(var) (&var)

/* Access member 'DL_TFI_ASSIGN' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_DL_TFI_ASSIGN(var) var

/* Access member 'DL_TFI_ASSIGN' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_DL_TFI_ASSIGN(var) (&var)

/* Access member 'ARFCN' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_ARFCN(var) (&var)

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) var

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) (&var)

/* Access member 'ArfcnIdx' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) var

/* Access member 'ArfcnIdx' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) (&var)

/* Access member 'ArfcnIdxList' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) var

/* Access member 'ArfcnIdxList' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) (&var)

/* Access member 'Hsn' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) var

/* Access member 'Hsn' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) (&var)

/* Access member 'MaBitMap' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) var

/* Access member 'MaBitMap' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) (&var)

/* Access member 'MaLen' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) var

/* Access member 'MaLen' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) (&var)

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) var

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) (&var)

/* Access member 'RflNb' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) var

/* Access member 'RflNb' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) (&var)

/* Access member 'RflNbList' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) var

/* Access member 'RflNbList' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) (&var)

/* Access member 'GPRS_MA' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) var

/* Access member 'GPRS_MA' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) (&var)

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_MAIO(var) var

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_MAIO(var) (&var)

/* Access member 'Dir_enc_1' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1(var) var

/* Access member 'Dir_enc_1' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1(var) (&var)

/* Access member 'HSN' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_HSN(var) var

/* Access member 'HSN' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_HSN(var) (&var)

/* Access member 'Len_MA_Freq_List' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) var

/* Access member 'Len_MA_Freq_List' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) (&var)

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_MAIO(var) var

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_MAIO(var) (&var)

/* Access member 'MA_Freq_List' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) var

/* Access member 'MA_Freq_List' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) (&var)

/* Access member 'Dir_enc_2' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2(var) var

/* Access member 'Dir_enc_2' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2(var) (&var)

/* Access member 'CHANGE_MARK_1' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) var

/* Access member 'CHANGE_MARK_1' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) (&var)

/* Access member 'CHANGE_MARK_2' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) var

/* Access member 'CHANGE_MARK_2' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) (&var)

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_MAIO(var) var

/* Access member 'MAIO' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_MAIO(var) (&var)

/* Access member 'MA_NB' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_MA_NB(var) var

/* Access member 'MA_NB' of type 'c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_MA_NB(var) (&var)

/* Access member 'Indir_enc' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc(var) var

/* Access member 'Indir_enc' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc(var) (&var)

/* Access member 'TSC' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param_TSC(var) var

/* Access member 'TSC' of type 'c_PDownlink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param_TSC(var) (&var)

/* Access member 'Freq_Param' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Freq_Param(var) var

/* Access member 'Freq_Param' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Freq_Param(var) (&var)

/* Access member 'MAC_MODE' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_MAC_MODE(var) var

/* Access member 'MAC_MODE' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_MAC_MODE(var) (&var)

/* Access member 'Meas_Mapping' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Meas_Mapping(var) var

/* Access member 'Meas_Mapping' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Meas_Mapping(var) (&var)

/* Access member 'P0' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_P0(var) var

/* Access member 'P0' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_P0(var) (&var)

/* Access member 'PExtended_Timing_Advance' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PExtended_Timing_Advance(var) var

/* Access member 'PExtended_Timing_Advance' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PExtended_Timing_Advance(var) (&var)

/* Access member 'PR_MODE' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PR_MODE(var) (&var)

/* Access member 'TA_INDEX' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_INDEX(var) var

/* Access member 'TA_INDEX' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_INDEX(var) (&var)

/* Access member 'TA_TS_NB' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_TS_NB(var) var

/* Access member 'TA_TS_NB' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PDownlink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_VALUE(var) (&var)

/* Access member 'PTiming_Advance' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_PTiming_Advance(var) var

/* Access member 'PTiming_Advance' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_PTiming_Advance(var) (&var)

/* Access member 'ALPHA' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_data(var) var

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA(var) (&var)

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p(var) (&var)

/* Access member 'Pwr_Ctrl_Param' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param(var) var

/* Access member 'Pwr_Ctrl_Param' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param(var) (&var)

/* Access member 'RLC_MODE' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_RLC_MODE(var) var

/* Access member 'RLC_MODE' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_RLC_MODE(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PDownlink_Assignment_Msg_TBF_ST' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PDownlink_Assignment_Msg_TBF_ST' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PDownlink_Assignment_Msg_TBF_ST' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PDownlink_Assignment_Msg_TBF_ST' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_TBF_ST(var) (&var)

/* Access member 'TS_ALLOC' of type 'c_PDownlink_Assignment_Msg' as a variable reference */
#define VAR_c_PDownlink_Assignment_Msg_TS_ALLOC(var) var

/* Access member 'TS_ALLOC' of type 'c_PDownlink_Assignment_Msg' as a pointer */
#define PTR_c_PDownlink_Assignment_Msg_TS_ALLOC(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx {
	ED_OCTET data [20];
	int items;
} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList {
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx ArfcnIdx;

} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList;

/* DEFINITION OF BINARY c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb {
	ED_OCTET data [20];
	int items;
} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList {
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb RflNb;

} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA {
	ED_BOOLEAN ArfcnIdxList_Present;
	ED_OCTET Hsn;
	ED_BOOLEAN MaBitMap_Present;
	ED_OCTET MaLen;
	ED_BOOLEAN MaLen_Present;
	ED_BOOLEAN RflNbList_Present;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList ArfcnIdxList;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap MaBitMap;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList RflNbList;

} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1 */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1 {
	ED_OCTET MAIO;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA GPRS_MA;

} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2 */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2 {
	ED_OCTET HSN;
	ED_OCTET Len_MA_Freq_List;
	ED_OCTET MAIO;
	c_TFrqList MA_Freq_List;

} c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc {
	ED_OCTET CHANGE_MARK_1;
	ED_BOOLEAN CHANGE_MARK_1_Present;
	ED_OCTET CHANGE_MARK_2;
	ED_BOOLEAN CHANGE_MARK_2_Present;
	ED_OCTET MAIO;
	ED_OCTET MA_NB;

} c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Freq_Param */
typedef struct _c_PDownlink_Assignment_Msg_Freq_Param {
	ED_SHORT ARFCN;
	ED_BOOLEAN ARFCN_Present;
	ED_BOOLEAN Dir_enc_1_Present;
	ED_BOOLEAN Dir_enc_2_Present;
	ED_BOOLEAN Indir_enc_Present;
	ED_OCTET TSC;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1 Dir_enc_1;
	c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2 Dir_enc_2;
	c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc Indir_enc;

} c_PDownlink_Assignment_Msg_Freq_Param;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_PTiming_Advance */
typedef struct _c_PDownlink_Assignment_Msg_PTiming_Advance {
	ED_OCTET TA_INDEX;
	ED_BOOLEAN TA_INDEX_Present;
	ED_OCTET TA_TS_NB;
	ED_BOOLEAN TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;

} c_PDownlink_Assignment_Msg_PTiming_Advance;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA */
typedef struct _c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p */
typedef struct _c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param */
typedef struct _c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param {
	ED_OCTET ALPHA;
	c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA GAMMA;
	c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p GAMMA_p;

} c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param;

/* DEFINITION OF SUB-STRUCTURE c_PDownlink_Assignment_Msg_TBF_ST */
typedef struct _c_PDownlink_Assignment_Msg_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PDownlink_Assignment_Msg_TBF_ST;


/*-----------------------------------*/
typedef struct _c_PDownlink_Assignment_Msg {
	ED_LONG Meas_Mapping;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_BOOLEAN CTRL_ACK;
	ED_OCTET DL_TFI_ASSIGN;
	ED_BOOLEAN DL_TFI_ASSIGN_Present;
	ED_BOOLEAN Freq_Param_Present;
	ED_OCTET MAC_MODE;
	ED_BOOLEAN Meas_Mapping_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_OCTET PExtended_Timing_Advance;
	ED_BOOLEAN PExtended_Timing_Advance_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN Pwr_Ctrl_Param_Present;
	ED_BOOLEAN RLC_MODE;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET TS_ALLOC;
	c_PDownlink_Assignment_Msg_Freq_Param Freq_Param;
	c_PDownlink_Assignment_Msg_PTiming_Advance PTiming_Advance;
	c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param Pwr_Ctrl_Param;
	c_PDownlink_Assignment_Msg_TBF_ST TBF_ST;
#ifdef __EGPRS__
	// add by dingmx 20061031 for EDGE
	ED_BOOLEAN COMPACT_reduced_MA_Present;
    c_COMPACT_reduced_MA COMPACT_reduced_MA;
    ED_BOOLEAN		EGPRS_Window_Size_Present;
    ED_OCTET		EGPRS_Window_Size;
    ED_BOOLEAN		LINK_QUALITY_MEAS_MODE_Present;
    ED_OCTET		LINK_QUALITY_MEAS_MODE;
    ED_BOOLEAN		BEP_PERIOD2_Present;
    ED_OCTET		BEP_PERIOD2;
	ED_BOOLEAN		Egprs_Present;
    ED_BOOLEAN		Gprs_R99_Present;
#endif
}	c_PDownlink_Assignment_Msg;
#ifdef __EGPRS__
// decoding function
long DECODE_c_PDownlink_Assignment_Msg(const char* const Buffer, 
                                        const long BitOffset,
                                        c_PDownlink_Assignment_Msg* const Destin,
                                        long Length);
#endif
#define INIT_c_PDownlink_Assignment_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PDownlink_Assignment_Msg))
#define FREE_c_PDownlink_Assignment_Msg(sp)
#define SETPRESENT_c_PDownlink_Assignment_Msg_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_DL_TFI_ASSIGN(sp,present) ((sp)->DL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_DL_TFI_ASSIGN(sp) ((sp)->DL_TFI_ASSIGN_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param(sp,present) ((sp)->Freq_Param_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param(sp) ((sp)->Freq_Param_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Meas_Mapping(sp,present) ((sp)->Meas_Mapping_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Meas_Mapping(sp) ((sp)->Meas_Mapping_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_PExtended_Timing_Advance(sp,present) ((sp)->PExtended_Timing_Advance_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_PExtended_Timing_Advance(sp) ((sp)->PExtended_Timing_Advance_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param(sp,present) ((sp)->Pwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param(sp) ((sp)->Pwr_Ctrl_Param_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST(sp) ((sp)->TBF_ST_Present)

/* Access member 'ALPHA' of type 'c_Pwr_Ctrl_Param_IE' as a variable reference */
#define VAR_c_Pwr_Ctrl_Param_IE_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_Pwr_Ctrl_Param_IE' as a pointer */
#define PTR_c_Pwr_Ctrl_Param_IE_ALPHA(var) (&var)

/* Access member 'data' of type 'c_Pwr_Ctrl_Param_IE_GAMMA' as a variable reference */
#define VAR_c_Pwr_Ctrl_Param_IE_GAMMA_data(var) var

/* Access member 'data' of type 'c_Pwr_Ctrl_Param_IE_GAMMA' as a pointer */
#define PTR_c_Pwr_Ctrl_Param_IE_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_Pwr_Ctrl_Param_IE' as a variable reference */
#define VAR_c_Pwr_Ctrl_Param_IE_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_Pwr_Ctrl_Param_IE' as a pointer */
#define PTR_c_Pwr_Ctrl_Param_IE_GAMMA(var) (&var)

/* Access member 'data' of type 'c_Pwr_Ctrl_Param_IE_GAMMA_p' as a variable reference */
#define VAR_c_Pwr_Ctrl_Param_IE_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_Pwr_Ctrl_Param_IE_GAMMA_p' as a pointer */
#define PTR_c_Pwr_Ctrl_Param_IE_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_Pwr_Ctrl_Param_IE' as a variable reference */
#define VAR_c_Pwr_Ctrl_Param_IE_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_Pwr_Ctrl_Param_IE' as a pointer */
#define PTR_c_Pwr_Ctrl_Param_IE_GAMMA_p(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Pwr_Ctrl_Param_IE_GAMMA */
typedef struct _c_Pwr_Ctrl_Param_IE_GAMMA {
	ED_OCTET data [8];
	int items;
} c_Pwr_Ctrl_Param_IE_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_Pwr_Ctrl_Param_IE_GAMMA_p */
typedef struct _c_Pwr_Ctrl_Param_IE_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_Pwr_Ctrl_Param_IE_GAMMA_p;


/*-----------------------------------*/
typedef struct _c_Pwr_Ctrl_Param_IE {
	ED_OCTET ALPHA;
	c_Pwr_Ctrl_Param_IE_GAMMA GAMMA;
	c_Pwr_Ctrl_Param_IE_GAMMA_p GAMMA_p;

}	c_Pwr_Ctrl_Param_IE;
#define INIT_c_Pwr_Ctrl_Param_IE(sp) ED_RESET_MEM ((sp), sizeof (c_Pwr_Ctrl_Param_IE))
#define FREE_c_Pwr_Ctrl_Param_IE(sp)

/* Access member 'TS_AVAILABLE' of type 'c_PPDCH_Release_Msg' as a variable reference */
#define VAR_c_PPDCH_Release_Msg_TS_AVAILABLE(var) var

/* Access member 'TS_AVAILABLE' of type 'c_PPDCH_Release_Msg' as a pointer */
#define PTR_c_PPDCH_Release_Msg_TS_AVAILABLE(var) (&var)


/*-----------------------------------*/
typedef struct _c_PPDCH_Release_Msg {
	ED_OCTET TS_AVAILABLE;

}	c_PPDCH_Release_Msg;
#define INIT_c_PPDCH_Release_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PPDCH_Release_Msg))
#define FREE_c_PPDCH_Release_Msg(sp)

/* Access member 'DL_TA_INDEX' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_INDEX(var) var

/* Access member 'DL_TA_INDEX' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_INDEX(var) (&var)

/* Access member 'DL_TA_TS_NB' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_TS_NB(var) var

/* Access member 'DL_TA_TS_NB' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_TA_VALUE(var) (&var)

/* Access member 'UL_TA_INDEX' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_INDEX(var) var

/* Access member 'UL_TA_INDEX' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_INDEX(var) (&var)

/* Access member 'UL_TA_TS_NB' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_TS_NB(var) var

/* Access member 'UL_TA_TS_NB' of type 'c_PPwrCtrl_TA_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_TS_NB(var) (&var)

/* Access member 'GPTiming_Advance' of type 'c_PPwrCtrl_TA_Msg' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPTiming_Advance(var) var

/* Access member 'GPTiming_Advance' of type 'c_PPwrCtrl_TA_Msg' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPTiming_Advance(var) (&var)

/* Access member 'ALPHA' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'INT_MEAS_CHANNEL_LIST_AVAIL' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_INT_MEAS_CHANNEL_LIST_AVAIL(var) var

/* Access member 'INT_MEAS_CHANNEL_LIST_AVAIL' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_INT_MEAS_CHANNEL_LIST_AVAIL(var) (&var)

/* Access member 'N_AVG_I' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_N_AVG_I(var) var

/* Access member 'N_AVG_I' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_N_AVG_I(var) (&var)

/* Access member 'PC_MEAS_CHAN' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_PC_MEAS_CHAN(var) var

/* Access member 'PC_MEAS_CHAN' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_PC_MEAS_CHAN(var) (&var)

/* Access member 'Pb' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_Pb(var) var

/* Access member 'Pb' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_Pb(var) (&var)

/* Access member 'T_AVG_T' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_T_AVG_T(var) var

/* Access member 'T_AVG_T' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_T_AVG_T(var) (&var)

/* Access member 'T_AVG_W' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_T_AVG_W(var) var

/* Access member 'T_AVG_W' of type 'c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param_T_AVG_W(var) (&var)

/* Access member 'GPwr_Ctrl_Param' of type 'c_PPwrCtrl_TA_Msg' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param(var) var

/* Access member 'GPwr_Ctrl_Param' of type 'c_PPwrCtrl_TA_Msg' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param(var) (&var)

/* Access member 'Packet_Extended_Timing_Advance' of type 'c_PPwrCtrl_TA_Msg' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Packet_Extended_Timing_Advance(var) var

/* Access member 'Packet_Extended_Timing_Advance' of type 'c_PPwrCtrl_TA_Msg' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Packet_Extended_Timing_Advance(var) (&var)

/* Access member 'ALPHA' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'data' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_data(var) var

/* Access member 'data' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA(var) (&var)

/* Access member 'data' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p(var) (&var)

/* Access member 'Pwr_Ctrl_Param' of type 'c_PPwrCtrl_TA_Msg' as a variable reference */
#define VAR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param(var) var

/* Access member 'Pwr_Ctrl_Param' of type 'c_PPwrCtrl_TA_Msg' as a pointer */
#define PTR_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PPwrCtrl_TA_Msg_GPTiming_Advance */
typedef struct _c_PPwrCtrl_TA_Msg_GPTiming_Advance {
	ED_OCTET DL_TA_INDEX;
	ED_BOOLEAN DL_TA_INDEX_Present;
	ED_OCTET DL_TA_TS_NB;
	ED_BOOLEAN DL_TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;
	ED_OCTET UL_TA_INDEX;
	ED_BOOLEAN UL_TA_INDEX_Present;
	ED_OCTET UL_TA_TS_NB;
	ED_BOOLEAN UL_TA_TS_NB_Present;

} c_PPwrCtrl_TA_Msg_GPTiming_Advance;

/* DEFINITION OF SUB-STRUCTURE c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param */
typedef struct _c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param {
	ED_OCTET ALPHA;
	ED_BOOLEAN INT_MEAS_CHANNEL_LIST_AVAIL;
	ED_OCTET N_AVG_I;
	ED_BOOLEAN PC_MEAS_CHAN;
	ED_OCTET Pb;
	ED_OCTET T_AVG_T;
	ED_OCTET T_AVG_W;

} c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param;

/* DEFINITION OF SUB-STRUCTURE c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA */
typedef struct _c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p */
typedef struct _c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param */
typedef struct _c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param {
	ED_OCTET ALPHA;
	c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA GAMMA;
	c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p GAMMA_p;

} c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param;


/*-----------------------------------*/
typedef struct _c_PPwrCtrl_TA_Msg {
	ED_BOOLEAN GPTiming_Advance_Present;
	ED_BOOLEAN GPwr_Ctrl_Param_Present;
	ED_OCTET Packet_Extended_Timing_Advance;
	ED_BOOLEAN Packet_Extended_Timing_Advance_Present;
	ED_BOOLEAN Pwr_Ctrl_Param_Present;
	c_PPwrCtrl_TA_Msg_GPTiming_Advance GPTiming_Advance;
	c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param GPwr_Ctrl_Param;
	c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param Pwr_Ctrl_Param;

}	c_PPwrCtrl_TA_Msg;
#define INIT_c_PPwrCtrl_TA_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PPwrCtrl_TA_Msg))
#define FREE_c_PPwrCtrl_TA_Msg(sp)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance(sp,present) ((sp)->GPTiming_Advance_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance(sp) ((sp)->GPTiming_Advance_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param(sp,present) ((sp)->GPwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPwr_Ctrl_Param(sp) ((sp)->GPwr_Ctrl_Param_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_Packet_Extended_Timing_Advance(sp,present) ((sp)->Packet_Extended_Timing_Advance_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_Packet_Extended_Timing_Advance(sp) ((sp)->Packet_Extended_Timing_Advance_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param(sp,present) ((sp)->Pwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param(sp) ((sp)->Pwr_Ctrl_Param_Present)

/* Access member 'DL_RELEASE' of type 'c_PTBF_Release_Msg' as a variable reference */
#define VAR_c_PTBF_Release_Msg_DL_RELEASE(var) var

/* Access member 'DL_RELEASE' of type 'c_PTBF_Release_Msg' as a pointer */
#define PTR_c_PTBF_Release_Msg_DL_RELEASE(var) (&var)

/* Access member 'TBF_RELEASE_CAUSE' of type 'c_PTBF_Release_Msg' as a variable reference */
#define VAR_c_PTBF_Release_Msg_TBF_RELEASE_CAUSE(var) var

/* Access member 'TBF_RELEASE_CAUSE' of type 'c_PTBF_Release_Msg' as a pointer */
#define PTR_c_PTBF_Release_Msg_TBF_RELEASE_CAUSE(var) (&var)

/* Access member 'UL_RELEASE' of type 'c_PTBF_Release_Msg' as a variable reference */
#define VAR_c_PTBF_Release_Msg_UL_RELEASE(var) var

/* Access member 'UL_RELEASE' of type 'c_PTBF_Release_Msg' as a pointer */
#define PTR_c_PTBF_Release_Msg_UL_RELEASE(var) (&var)


/*-----------------------------------*/
typedef struct _c_PTBF_Release_Msg {
	ED_BOOLEAN DL_RELEASE;
	ED_OCTET TBF_RELEASE_CAUSE;
	ED_BOOLEAN UL_RELEASE;

}	c_PTBF_Release_Msg;
#define INIT_c_PTBF_Release_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PTBF_Release_Msg))
#define FREE_c_PTBF_Release_Msg(sp)

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_CHANNEL_CODING_COMMAND(var) var

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_CHANNEL_CODING_COMMAND(var) (&var)

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_ALPHA(var) (&var)

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_EXTENDED_DYNAMIC_ALLOC(var) var

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_EXTENDED_DYNAMIC_ALLOC(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_P0(var) var

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_PR_MODE(var) (&var)

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(var) var

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_data(var) (&var)

/* Access member 'USF' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF(var) var

/* Access member 'USF' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF(var) (&var)

/* Access member 'USF_GRANULARITY' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_GRANULARITY(var) var

/* Access member 'USF_GRANULARITY' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_GRANULARITY(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p_data(var) (&var)

/* Access member 'USF_p' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p(var) var

/* Access member 'USF_p' of type 'c_PUplink_Assignment_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p(var) (&var)

/* Access member 'Dynamic_Alloc' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Dynamic_Alloc(var) var

/* Access member 'Dynamic_Alloc' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Dynamic_Alloc(var) (&var)

/* Access member 'AllocBitmap' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap(var) var

/* Access member 'AllocBitmap' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap(var) (&var)

/* Access member 'AllocBitmapLen' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmapLen(var) var

/* Access member 'AllocBitmapLen' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmapLen(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'BlocksOrBlockPeriods' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_BlocksOrBlockPeriods(var) var

/* Access member 'BlocksOrBlockPeriods' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_BlocksOrBlockPeriods(var) (&var)

/* Access member 'DL_CTRL_TS' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_DL_CTRL_TS(var) var

/* Access member 'DL_CTRL_TS' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_DL_CTRL_TS(var) (&var)

/* Access member 'FINAL_ALLOC' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_FINAL_ALLOC(var) var

/* Access member 'FINAL_ALLOC' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_FINAL_ALLOC(var) (&var)

/* Access member 'Meas_Mapping' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Meas_Mapping(var) var

/* Access member 'Meas_Mapping' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Meas_Mapping(var) (&var)

/* Access member 'Message_escape' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape(var) var

/* Access member 'Message_escape' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape(var) (&var)

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_P0(var) var

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_PR_MODE(var) (&var)

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(var) (&var)

/* Access member 'Pwr_Ctrl_Param' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param(var) var

/* Access member 'Pwr_Ctrl_Param' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TFI_ASSIGN(var) (&var)

/* Access member 'UL_TS_ALLOC' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TS_ALLOC(var) var

/* Access member 'UL_TS_ALLOC' of type 'c_PUplink_Assignment_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TS_ALLOC(var) (&var)

/* Access member 'Fixed_Alloc' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Fixed_Alloc(var) var

/* Access member 'Fixed_Alloc' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Fixed_Alloc(var) (&var)

/* Access member 'ARFCN' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_ARFCN(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) (&var)

/* Access member 'ArfcnIdx' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) var

/* Access member 'ArfcnIdx' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) (&var)

/* Access member 'ArfcnIdxList' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) var

/* Access member 'ArfcnIdxList' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) (&var)

/* Access member 'Hsn' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) var

/* Access member 'Hsn' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) (&var)

/* Access member 'MaBitMap' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) var

/* Access member 'MaBitMap' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) (&var)

/* Access member 'MaLen' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) var

/* Access member 'MaLen' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) (&var)

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) var

/* Access member 'data' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) (&var)

/* Access member 'RflNb' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) var

/* Access member 'RflNb' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) (&var)

/* Access member 'RflNbList' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) var

/* Access member 'RflNbList' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) (&var)

/* Access member 'GPRS_MA' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) var

/* Access member 'GPRS_MA' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) (&var)

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_MAIO(var) var

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_MAIO(var) (&var)

/* Access member 'Dir_enc_1' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1(var) var

/* Access member 'Dir_enc_1' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1(var) (&var)

/* Access member 'HSN' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_HSN(var) var

/* Access member 'HSN' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_HSN(var) (&var)

/* Access member 'Len_MA_Freq_List' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) var

/* Access member 'Len_MA_Freq_List' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) (&var)

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_MAIO(var) var

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_MAIO(var) (&var)

/* Access member 'MA_Freq_List' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) var

/* Access member 'MA_Freq_List' of type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) (&var)

/* Access member 'Dir_enc_2' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2(var) var

/* Access member 'Dir_enc_2' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2(var) (&var)

/* Access member 'CHANGE_MARK_1' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) var

/* Access member 'CHANGE_MARK_1' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) (&var)

/* Access member 'CHANGE_MARK_2' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) var

/* Access member 'CHANGE_MARK_2' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) (&var)

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_MAIO(var) var

/* Access member 'MAIO' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_MAIO(var) (&var)

/* Access member 'MA_NB' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_MA_NB(var) var

/* Access member 'MA_NB' of type 'c_PUplink_Assignment_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_MA_NB(var) (&var)

/* Access member 'Indir_enc' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc(var) var

/* Access member 'Indir_enc' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc(var) (&var)

/* Access member 'TSC' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param_TSC(var) var

/* Access member 'TSC' of type 'c_PUplink_Assignment_Msg_Freq_Param' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param_TSC(var) (&var)

/* Access member 'Freq_Param' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Freq_Param(var) var

/* Access member 'Freq_Param' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Freq_Param(var) (&var)

/* Access member 'PExtended_Timing_Advance' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_PExtended_Timing_Advance(var) var

/* Access member 'PExtended_Timing_Advance' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_PExtended_Timing_Advance(var) (&var)

/* Access member 'TA_INDEX' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_INDEX(var) var

/* Access member 'TA_INDEX' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_INDEX(var) (&var)

/* Access member 'TA_TS_NB' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_TS_NB(var) var

/* Access member 'TA_TS_NB' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PUplink_Assignment_Msg_PTiming_Advance' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_PTiming_Advance_TA_VALUE(var) (&var)

/* Access member 'PTiming_Advance' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_PTiming_Advance(var) var

/* Access member 'PTiming_Advance' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_PTiming_Advance(var) (&var)

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_ALPHA(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_P0(var) var

/* Access member 'P0' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_PR_MODE(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST(var) (&var)

/* Access member 'TS_NB' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TS_NB(var) var

/* Access member 'TS_NB' of type 'c_PUplink_Assignment_Msg_Single_Block_Alloc' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc_TS_NB(var) (&var)

/* Access member 'Single_Block_Alloc' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_Single_Block_Alloc(var) var

/* Access member 'Single_Block_Alloc' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_Single_Block_Alloc(var) (&var)

/* Access member 'TLLI_BLOCK_CHANNEL_CODING' of type 'c_PUplink_Assignment_Msg' as a variable reference */
#define VAR_c_PUplink_Assignment_Msg_TLLI_BLOCK_CHANNEL_CODING(var) var

/* Access member 'TLLI_BLOCK_CHANNEL_CODING' of type 'c_PUplink_Assignment_Msg' as a pointer */
#define PTR_c_PUplink_Assignment_Msg_TLLI_BLOCK_CHANNEL_CODING(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA */
typedef struct _c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST */
typedef struct _c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Dynamic_Alloc_USF */
typedef struct _c_PUplink_Assignment_Msg_Dynamic_Alloc_USF {
	ED_OCTET data [8];
	int items;
} c_PUplink_Assignment_Msg_Dynamic_Alloc_USF;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p */
typedef struct _c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p {
	ED_BOOLEAN data [8];
	int items;
} c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Dynamic_Alloc */
typedef struct _c_PUplink_Assignment_Msg_Dynamic_Alloc {
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN EXTENDED_DYNAMIC_ALLOC;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_OCTET RLC_DATA_BLOCKS_GRANTED;
	ED_BOOLEAN RLC_DATA_BLOCKS_GRANTED_Present;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_BOOLEAN USF_GRANULARITY;
	c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA GAMMA;
	c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST TBF_ST;
	c_PUplink_Assignment_Msg_Dynamic_Alloc_USF USF;
	c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p USF_p;

} c_PUplink_Assignment_Msg_Dynamic_Alloc;

/* DEFINITION OF BINARY c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param {
	ED_OCTET ALPHA;
	c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA GAMMA;
	c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p GAMMA_p;

} c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Fixed_Alloc */
typedef struct _c_PUplink_Assignment_Msg_Fixed_Alloc {
	ED_LONG Meas_Mapping;
	ED_BOOLEAN AllocBitmap_Present;
	ED_OCTET AllocBitmapLen;
	ED_BOOLEAN AllocBitmapLen_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_BOOLEAN BlocksOrBlockPeriods;
	ED_BOOLEAN BlocksOrBlockPeriods_Present;
	ED_OCTET DL_CTRL_TS;
	ED_BOOLEAN FINAL_ALLOC;
	ED_BOOLEAN Meas_Mapping_Present;
	ED_BOOLEAN Message_escape_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN Pwr_Ctrl_Param_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_OCTET UL_TS_ALLOC;
	ED_BOOLEAN UL_TS_ALLOC_Present;
	c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap AllocBitmap;
	c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape Message_escape;
	c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param Pwr_Ctrl_Param;
	c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST TBF_ST;

} c_PUplink_Assignment_Msg_Fixed_Alloc;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx {
	ED_OCTET data [20];
	int items;
} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList {
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx ArfcnIdx;

} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList;

/* DEFINITION OF BINARY c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb {
	ED_OCTET data [20];
	int items;
} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList {
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb RflNb;

} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA {
	ED_BOOLEAN ArfcnIdxList_Present;
	ED_OCTET Hsn;
	ED_BOOLEAN MaBitMap_Present;
	ED_OCTET MaLen;
	ED_BOOLEAN MaLen_Present;
	ED_BOOLEAN RflNbList_Present;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList ArfcnIdxList;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap MaBitMap;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList RflNbList;

} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1 */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1 {
	ED_OCTET MAIO;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA GPRS_MA;

} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2 */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2 {
	ED_OCTET HSN;
	ED_OCTET Len_MA_Freq_List;
	ED_OCTET MAIO;
	c_TFrqList MA_Freq_List;

} c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param_Indir_enc */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param_Indir_enc {
	ED_OCTET CHANGE_MARK_1;
	ED_BOOLEAN CHANGE_MARK_1_Present;
	ED_OCTET CHANGE_MARK_2;
	ED_BOOLEAN CHANGE_MARK_2_Present;
	ED_OCTET MAIO;
	ED_OCTET MA_NB;

} c_PUplink_Assignment_Msg_Freq_Param_Indir_enc;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Freq_Param */
typedef struct _c_PUplink_Assignment_Msg_Freq_Param {
	ED_SHORT ARFCN;
	ED_BOOLEAN ARFCN_Present;
	ED_BOOLEAN Dir_enc_1_Present;
	ED_BOOLEAN Dir_enc_2_Present;
	ED_BOOLEAN Indir_enc_Present;
	ED_OCTET TSC;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1 Dir_enc_1;
	c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2 Dir_enc_2;
	c_PUplink_Assignment_Msg_Freq_Param_Indir_enc Indir_enc;

} c_PUplink_Assignment_Msg_Freq_Param;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_PTiming_Advance */
typedef struct _c_PUplink_Assignment_Msg_PTiming_Advance {
	ED_OCTET TA_INDEX;
	ED_BOOLEAN TA_INDEX_Present;
	ED_OCTET TA_TS_NB;
	ED_BOOLEAN TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;

} c_PUplink_Assignment_Msg_PTiming_Advance;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST */
typedef struct _c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_PUplink_Assignment_Msg_Single_Block_Alloc */
typedef struct _c_PUplink_Assignment_Msg_Single_Block_Alloc {
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_OCTET GAMMA;
	ED_BOOLEAN GAMMA_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_OCTET TS_NB;
	c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST TBF_ST;

} c_PUplink_Assignment_Msg_Single_Block_Alloc;

//add by lincq begin

#ifdef __EGPRS__
typedef struct _c_PUplink_Assignment_msg_ContTlli {
	ED_BOOLEAN ContResTLLI_Present;
	ED_LONG ContResTLLI;
}	c_PUplink_Assignment_ContTlli;
#endif
//add by lincq end
#ifndef __EGPRS__
/*-----------------------------------*/
typedef struct _c_PUplink_Assignment_Msg {
	ED_OCTET CHANNEL_CODING_COMMAND;
	ED_BOOLEAN CHANNEL_CODING_COMMAND_Present;
	ED_BOOLEAN Dynamic_Alloc_Present;
	ED_BOOLEAN Fixed_Alloc_Present;
	ED_BOOLEAN Freq_Param_Present;
	ED_OCTET PExtended_Timing_Advance;
	ED_BOOLEAN PExtended_Timing_Advance_Present;
	ED_BOOLEAN PTiming_Advance_Present;
	ED_BOOLEAN Single_Block_Alloc_Present;
	ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING;
	ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING_Present;
	c_PUplink_Assignment_Msg_Dynamic_Alloc Dynamic_Alloc;
	c_PUplink_Assignment_Msg_Fixed_Alloc Fixed_Alloc;
	c_PUplink_Assignment_Msg_Freq_Param Freq_Param;
	c_PUplink_Assignment_Msg_PTiming_Advance PTiming_Advance;
	c_PUplink_Assignment_Msg_Single_Block_Alloc Single_Block_Alloc;

}	c_PUplink_Assignment_Msg;
#endif
#define INIT_c_PUplink_Assignment_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PUplink_Assignment_Msg))
#define FREE_c_PUplink_Assignment_Msg(sp)
#define SETPRESENT_c_PUplink_Assignment_Msg_CHANNEL_CODING_COMMAND(sp,present) ((sp)->CHANNEL_CODING_COMMAND_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_CHANNEL_CODING_COMMAND(sp) ((sp)->CHANNEL_CODING_COMMAND_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc(sp,present) ((sp)->Dynamic_Alloc_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc(sp) ((sp)->Dynamic_Alloc_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc(sp,present) ((sp)->Fixed_Alloc_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc(sp) ((sp)->Fixed_Alloc_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param(sp,present) ((sp)->Freq_Param_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param(sp) ((sp)->Freq_Param_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_PExtended_Timing_Advance(sp,present) ((sp)->PExtended_Timing_Advance_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_PExtended_Timing_Advance(sp) ((sp)->PExtended_Timing_Advance_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance(sp,present) ((sp)->PTiming_Advance_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance(sp) ((sp)->PTiming_Advance_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc(sp,present) ((sp)->Single_Block_Alloc_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc(sp) ((sp)->Single_Block_Alloc_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_TLLI_BLOCK_CHANNEL_CODING(sp,present) ((sp)->TLLI_BLOCK_CHANNEL_CODING_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_TLLI_BLOCK_CHANNEL_CODING(sp) ((sp)->TLLI_BLOCK_CHANNEL_CODING_Present)

/* Access member 'ALPHA' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_ALPHA(var) (&var)

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_EXTENDED_DYNAMIC_ALLOC(var) var

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_EXTENDED_DYNAMIC_ALLOC(var) (&var)

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_GAMMA' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_GAMMA_data(var) var

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_GAMMA' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_P0(var) var

/* Access member 'P0' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_PR_MODE(var) (&var)

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_RLC_DATA_BLOCKS_GRANTED(var) var

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_RLC_DATA_BLOCKS_GRANTED(var) (&var)

/* Access member 'Absolute_FN' of type 'c_Dynamic_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_Dynamic_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_Dynamic_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_Dynamic_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_UL_TFI_ASSIGN(var) (&var)

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_USF' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_USF_data(var) var

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_USF' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_USF_data(var) (&var)

/* Access member 'USF' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_USF(var) var

/* Access member 'USF' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_USF(var) (&var)

/* Access member 'USF_GRANULARITY' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_USF_GRANULARITY(var) var

/* Access member 'USF_GRANULARITY' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_USF_GRANULARITY(var) (&var)

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_USF_p' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_USF_p_data(var) var

/* Access member 'data' of type 'c_Dynamic_Alloc_struct_USF_p' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_USF_p_data(var) (&var)

/* Access member 'USF_p' of type 'c_Dynamic_Alloc_struct' as a variable reference */
#define VAR_c_Dynamic_Alloc_struct_USF_p(var) var

/* Access member 'USF_p' of type 'c_Dynamic_Alloc_struct' as a pointer */
#define PTR_c_Dynamic_Alloc_struct_USF_p(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Dynamic_Alloc_struct_GAMMA */
typedef struct _c_Dynamic_Alloc_struct_GAMMA {
	ED_OCTET data [8];
	int items;
} c_Dynamic_Alloc_struct_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_Dynamic_Alloc_struct_TBF_ST */
typedef struct _c_Dynamic_Alloc_struct_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_Dynamic_Alloc_struct_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_Dynamic_Alloc_struct_USF */
typedef struct _c_Dynamic_Alloc_struct_USF {
	ED_OCTET data [8];
	int items;
} c_Dynamic_Alloc_struct_USF;

/* DEFINITION OF SUB-STRUCTURE c_Dynamic_Alloc_struct_USF_p */
typedef struct _c_Dynamic_Alloc_struct_USF_p {
	ED_BOOLEAN data [8];
	int items;
} c_Dynamic_Alloc_struct_USF_p;


/*-----------------------------------*/
typedef struct _c_Dynamic_Alloc_struct {
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN EXTENDED_DYNAMIC_ALLOC;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_OCTET RLC_DATA_BLOCKS_GRANTED;
	ED_BOOLEAN RLC_DATA_BLOCKS_GRANTED_Present;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_BOOLEAN USF_GRANULARITY;
	c_Dynamic_Alloc_struct_GAMMA GAMMA;
	c_Dynamic_Alloc_struct_TBF_ST TBF_ST;
	c_Dynamic_Alloc_struct_USF USF;
	c_Dynamic_Alloc_struct_USF_p USF_p;

}	c_Dynamic_Alloc_struct;
#define INIT_c_Dynamic_Alloc_struct(sp) ED_RESET_MEM ((sp), sizeof (c_Dynamic_Alloc_struct))
#define FREE_c_Dynamic_Alloc_struct(sp)
#define SETPRESENT_c_Dynamic_Alloc_struct_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_RLC_DATA_BLOCKS_GRANTED(sp,present) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_RLC_DATA_BLOCKS_GRANTED(sp) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_TBF_ST(sp) ((sp)->TBF_ST_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)

/* Access member 'ALPHA' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_ALPHA(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'GAMMA' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_P0(var) var

/* Access member 'P0' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_PR_MODE(var) (&var)

/* Access member 'Absolute_FN' of type 'c_Single_Block_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_Single_Block_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_Single_Block_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_Single_Block_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_TBF_ST(var) (&var)

/* Access member 'TS_NB' of type 'c_Single_Block_Alloc_struct' as a variable reference */
#define VAR_c_Single_Block_Alloc_struct_TS_NB(var) var

/* Access member 'TS_NB' of type 'c_Single_Block_Alloc_struct' as a pointer */
#define PTR_c_Single_Block_Alloc_struct_TS_NB(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Single_Block_Alloc_struct_TBF_ST */
typedef struct _c_Single_Block_Alloc_struct_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_Single_Block_Alloc_struct_TBF_ST;


/*-----------------------------------*/
typedef struct _c_Single_Block_Alloc_struct {
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_OCTET GAMMA;
	ED_BOOLEAN GAMMA_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_OCTET TS_NB;
	c_Single_Block_Alloc_struct_TBF_ST TBF_ST;

}	c_Single_Block_Alloc_struct;
#define INIT_c_Single_Block_Alloc_struct(sp) ED_RESET_MEM ((sp), sizeof (c_Single_Block_Alloc_struct))
#define FREE_c_Single_Block_Alloc_struct(sp)
#define SETPRESENT_c_Single_Block_Alloc_struct_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_Single_Block_Alloc_struct_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_Single_Block_Alloc_struct_GAMMA(sp,present) ((sp)->GAMMA_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_GAMMA(sp) ((sp)->GAMMA_Present)
#define SETPRESENT_c_Single_Block_Alloc_struct_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_Single_Block_Alloc_struct_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_PR_MODE(sp) ((sp)->PR_MODE_Present)

/* Access member 'Absolute_FN' of type 'c_SFN_Desc_IE' as a variable reference */
#define VAR_c_SFN_Desc_IE_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_SFN_Desc_IE' as a pointer */
#define PTR_c_SFN_Desc_IE_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_SFN_Desc_IE' as a variable reference */
#define VAR_c_SFN_Desc_IE_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_SFN_Desc_IE' as a pointer */
#define PTR_c_SFN_Desc_IE_Relative_FN(var) (&var)


/*-----------------------------------*/
typedef struct _c_SFN_Desc_IE {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

}	c_SFN_Desc_IE;
#define INIT_c_SFN_Desc_IE(sp) ED_RESET_MEM ((sp), sizeof (c_SFN_Desc_IE))
#define FREE_c_SFN_Desc_IE(sp)
#define SETPRESENT_c_SFN_Desc_IE_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_SFN_Desc_IE_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_SFN_Desc_IE_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_SFN_Desc_IE_Relative_FN(sp) ((sp)->Relative_FN_Present)

/* Access member 'TA_INDEX' of type 'c_PTiming_Advance_IE' as a variable reference */
#define VAR_c_PTiming_Advance_IE_TA_INDEX(var) var

/* Access member 'TA_INDEX' of type 'c_PTiming_Advance_IE' as a pointer */
#define PTR_c_PTiming_Advance_IE_TA_INDEX(var) (&var)

/* Access member 'TA_TS_NB' of type 'c_PTiming_Advance_IE' as a variable reference */
#define VAR_c_PTiming_Advance_IE_TA_TS_NB(var) var

/* Access member 'TA_TS_NB' of type 'c_PTiming_Advance_IE' as a pointer */
#define PTR_c_PTiming_Advance_IE_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PTiming_Advance_IE' as a variable reference */
#define VAR_c_PTiming_Advance_IE_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PTiming_Advance_IE' as a pointer */
#define PTR_c_PTiming_Advance_IE_TA_VALUE(var) (&var)


/*-----------------------------------*/
typedef struct _c_PTiming_Advance_IE {
	ED_OCTET TA_INDEX;
	ED_BOOLEAN TA_INDEX_Present;
	ED_OCTET TA_TS_NB;
	ED_BOOLEAN TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;

}	c_PTiming_Advance_IE;
#define INIT_c_PTiming_Advance_IE(sp) ED_RESET_MEM ((sp), sizeof (c_PTiming_Advance_IE))
#define FREE_c_PTiming_Advance_IE(sp)
#define SETPRESENT_c_PTiming_Advance_IE_TA_INDEX(sp,present) ((sp)->TA_INDEX_Present = present)
#define GETPRESENT_c_PTiming_Advance_IE_TA_INDEX(sp) ((sp)->TA_INDEX_Present)
#define SETPRESENT_c_PTiming_Advance_IE_TA_TS_NB(sp,present) ((sp)->TA_TS_NB_Present = present)
#define GETPRESENT_c_PTiming_Advance_IE_TA_TS_NB(sp) ((sp)->TA_TS_NB_Present)
#define SETPRESENT_c_PTiming_Advance_IE_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PTiming_Advance_IE_TA_VALUE(sp) ((sp)->TA_VALUE_Present)

/* Access member 'ARFCN' of type 'c_Freq_Param_IE' as a variable reference */
#define VAR_c_Freq_Param_IE_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_Freq_Param_IE' as a pointer */
#define PTR_c_Freq_Param_IE_ARFCN(var) (&var)

/* Access member 'data' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) var

/* Access member 'data' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) (&var)

/* Access member 'ArfcnIdx' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) var

/* Access member 'ArfcnIdx' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) (&var)

/* Access member 'ArfcnIdxList' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) var

/* Access member 'ArfcnIdxList' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) (&var)

/* Access member 'Hsn' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_Hsn(var) var

/* Access member 'Hsn' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_Hsn(var) (&var)

/* Access member 'MaBitMap' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap(var) var

/* Access member 'MaBitMap' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap(var) (&var)

/* Access member 'MaLen' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaLen(var) var

/* Access member 'MaLen' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaLen(var) (&var)

/* Access member 'data' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) var

/* Access member 'data' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) (&var)

/* Access member 'RflNb' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) var

/* Access member 'RflNb' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) (&var)

/* Access member 'RflNbList' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList(var) var

/* Access member 'RflNbList' of type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList(var) (&var)

/* Access member 'GPRS_MA' of type 'c_Freq_Param_IE_Dir_enc_1' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA(var) var

/* Access member 'GPRS_MA' of type 'c_Freq_Param_IE_Dir_enc_1' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_GPRS_MA(var) (&var)

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Dir_enc_1' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1_MAIO(var) var

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Dir_enc_1' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1_MAIO(var) (&var)

/* Access member 'Dir_enc_1' of type 'c_Freq_Param_IE' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_1(var) var

/* Access member 'Dir_enc_1' of type 'c_Freq_Param_IE' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_1(var) (&var)

/* Access member 'HSN' of type 'c_Freq_Param_IE_Dir_enc_2' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_2_HSN(var) var

/* Access member 'HSN' of type 'c_Freq_Param_IE_Dir_enc_2' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_2_HSN(var) (&var)

/* Access member 'Len_MA_Freq_List' of type 'c_Freq_Param_IE_Dir_enc_2' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_2_Len_MA_Freq_List(var) var

/* Access member 'Len_MA_Freq_List' of type 'c_Freq_Param_IE_Dir_enc_2' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_2_Len_MA_Freq_List(var) (&var)

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Dir_enc_2' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_2_MAIO(var) var

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Dir_enc_2' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_2_MAIO(var) (&var)

/* Access member 'MA_Freq_List' of type 'c_Freq_Param_IE_Dir_enc_2' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_2_MA_Freq_List(var) var

/* Access member 'MA_Freq_List' of type 'c_Freq_Param_IE_Dir_enc_2' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_2_MA_Freq_List(var) (&var)

/* Access member 'Dir_enc_2' of type 'c_Freq_Param_IE' as a variable reference */
#define VAR_c_Freq_Param_IE_Dir_enc_2(var) var

/* Access member 'Dir_enc_2' of type 'c_Freq_Param_IE' as a pointer */
#define PTR_c_Freq_Param_IE_Dir_enc_2(var) (&var)

/* Access member 'CHANGE_MARK_1' of type 'c_Freq_Param_IE_Indir_enc' as a variable reference */
#define VAR_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_1(var) var

/* Access member 'CHANGE_MARK_1' of type 'c_Freq_Param_IE_Indir_enc' as a pointer */
#define PTR_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_1(var) (&var)

/* Access member 'CHANGE_MARK_2' of type 'c_Freq_Param_IE_Indir_enc' as a variable reference */
#define VAR_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_2(var) var

/* Access member 'CHANGE_MARK_2' of type 'c_Freq_Param_IE_Indir_enc' as a pointer */
#define PTR_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_2(var) (&var)

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Indir_enc' as a variable reference */
#define VAR_c_Freq_Param_IE_Indir_enc_MAIO(var) var

/* Access member 'MAIO' of type 'c_Freq_Param_IE_Indir_enc' as a pointer */
#define PTR_c_Freq_Param_IE_Indir_enc_MAIO(var) (&var)

/* Access member 'MA_NB' of type 'c_Freq_Param_IE_Indir_enc' as a variable reference */
#define VAR_c_Freq_Param_IE_Indir_enc_MA_NB(var) var

/* Access member 'MA_NB' of type 'c_Freq_Param_IE_Indir_enc' as a pointer */
#define PTR_c_Freq_Param_IE_Indir_enc_MA_NB(var) (&var)

/* Access member 'Indir_enc' of type 'c_Freq_Param_IE' as a variable reference */
#define VAR_c_Freq_Param_IE_Indir_enc(var) var

/* Access member 'Indir_enc' of type 'c_Freq_Param_IE' as a pointer */
#define PTR_c_Freq_Param_IE_Indir_enc(var) (&var)

/* Access member 'TSC' of type 'c_Freq_Param_IE' as a variable reference */
#define VAR_c_Freq_Param_IE_TSC(var) var

/* Access member 'TSC' of type 'c_Freq_Param_IE' as a pointer */
#define PTR_c_Freq_Param_IE_TSC(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx {
	ED_OCTET data [20];
	int items;
} c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList {
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx ArfcnIdx;

} c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList;

/* DEFINITION OF BINARY c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb {
	ED_OCTET data [20];
	int items;
} c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList {
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb RflNb;

} c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1_GPRS_MA */
typedef struct _c_Freq_Param_IE_Dir_enc_1_GPRS_MA {
	ED_BOOLEAN ArfcnIdxList_Present;
	ED_OCTET Hsn;
	ED_BOOLEAN MaBitMap_Present;
	ED_OCTET MaLen;
	ED_BOOLEAN MaLen_Present;
	ED_BOOLEAN RflNbList_Present;
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList ArfcnIdxList;
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap MaBitMap;
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList RflNbList;

} c_Freq_Param_IE_Dir_enc_1_GPRS_MA;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_1 */
typedef struct _c_Freq_Param_IE_Dir_enc_1 {
	ED_OCTET MAIO;
	c_Freq_Param_IE_Dir_enc_1_GPRS_MA GPRS_MA;

} c_Freq_Param_IE_Dir_enc_1;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Dir_enc_2 */
typedef struct _c_Freq_Param_IE_Dir_enc_2 {
	ED_OCTET HSN;
	ED_OCTET Len_MA_Freq_List;
	ED_OCTET MAIO;
	c_TFrqList MA_Freq_List;

} c_Freq_Param_IE_Dir_enc_2;

/* DEFINITION OF SUB-STRUCTURE c_Freq_Param_IE_Indir_enc */
typedef struct _c_Freq_Param_IE_Indir_enc {
	ED_OCTET CHANGE_MARK_1;
	ED_BOOLEAN CHANGE_MARK_1_Present;
	ED_OCTET CHANGE_MARK_2;
	ED_BOOLEAN CHANGE_MARK_2_Present;
	ED_OCTET MAIO;
	ED_OCTET MA_NB;

} c_Freq_Param_IE_Indir_enc;


/*-----------------------------------*/
typedef struct _c_Freq_Param_IE {
	ED_SHORT ARFCN;
	ED_BOOLEAN ARFCN_Present;
	ED_BOOLEAN Dir_enc_1_Present;
	ED_BOOLEAN Dir_enc_2_Present;
	ED_BOOLEAN Indir_enc_Present;
	ED_OCTET TSC;
	c_Freq_Param_IE_Dir_enc_1 Dir_enc_1;
	c_Freq_Param_IE_Dir_enc_2 Dir_enc_2;
	c_Freq_Param_IE_Indir_enc Indir_enc;

}	c_Freq_Param_IE;
#define INIT_c_Freq_Param_IE(sp) ED_RESET_MEM ((sp), sizeof (c_Freq_Param_IE))
#define FREE_c_Freq_Param_IE(sp)
#define SETPRESENT_c_Freq_Param_IE_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_Freq_Param_IE_ARFCN(sp) ((sp)->ARFCN_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_1(sp,present) ((sp)->Dir_enc_1_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_1(sp) ((sp)->Dir_enc_1_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_2(sp,present) ((sp)->Dir_enc_2_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_2(sp) ((sp)->Dir_enc_2_Present)
#define SETPRESENT_c_Freq_Param_IE_Indir_enc(sp,present) ((sp)->Indir_enc_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Indir_enc(sp) ((sp)->Indir_enc_Present)

/* Access member 'AllocBitmap' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_AllocBitmap(var) var

/* Access member 'AllocBitmap' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_AllocBitmap(var) (&var)

/* Access member 'AllocBitmapLen' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_AllocBitmapLen(var) var

/* Access member 'AllocBitmapLen' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_AllocBitmapLen(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'BlocksOrBlockPeriods' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_BlocksOrBlockPeriods(var) var

/* Access member 'BlocksOrBlockPeriods' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_BlocksOrBlockPeriods(var) (&var)

/* Access member 'DL_CTRL_TS' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_DL_CTRL_TS(var) var

/* Access member 'DL_CTRL_TS' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_DL_CTRL_TS(var) (&var)

/* Access member 'FINAL_ALLOC' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_FINAL_ALLOC(var) var

/* Access member 'FINAL_ALLOC' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_FINAL_ALLOC(var) (&var)

/* Access member 'Meas_Mapping' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Meas_Mapping(var) var

/* Access member 'Meas_Mapping' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Meas_Mapping(var) (&var)

/* Access member 'Message_escape' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Message_escape(var) var

/* Access member 'Message_escape' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Message_escape(var) (&var)

/* Access member 'P0' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_P0(var) var

/* Access member 'P0' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_PR_MODE(var) (&var)

/* Access member 'ALPHA' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'data' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_data(var) var

/* Access member 'data' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA(var) (&var)

/* Access member 'data' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p(var) (&var)

/* Access member 'Pwr_Ctrl_Param' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param(var) var

/* Access member 'Pwr_Ctrl_Param' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_Pwr_Ctrl_Param(var) (&var)

/* Access member 'Absolute_FN' of type 'c_Fixed_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_Fixed_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Fixed_Alloc_struct_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_Fixed_Alloc_struct_TBF_ST' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_Fixed_Alloc_struct_TBF_ST' as a pointer */
#define PTR_c_Fixed_Alloc_struct_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_UL_TFI_ASSIGN(var) (&var)

/* Access member 'UL_TS_ALLOC' of type 'c_Fixed_Alloc_struct' as a variable reference */
#define VAR_c_Fixed_Alloc_struct_UL_TS_ALLOC(var) var

/* Access member 'UL_TS_ALLOC' of type 'c_Fixed_Alloc_struct' as a pointer */
#define PTR_c_Fixed_Alloc_struct_UL_TS_ALLOC(var) (&var)

/* DEFINITION OF BINARY c_Fixed_Alloc_struct_AllocBitmap */
typedef struct _c_Fixed_Alloc_struct_AllocBitmap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Fixed_Alloc_struct_AllocBitmap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Fixed_Alloc_struct_AllocBitmap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_Fixed_Alloc_struct_Message_escape */
typedef struct _c_Fixed_Alloc_struct_Message_escape {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Fixed_Alloc_struct_Message_escape;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Fixed_Alloc_struct_Message_escape(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA */
typedef struct _c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA {
	ED_OCTET data [8];
	int items;
} c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p */
typedef struct _c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_Fixed_Alloc_struct_Pwr_Ctrl_Param */
typedef struct _c_Fixed_Alloc_struct_Pwr_Ctrl_Param {
	ED_OCTET ALPHA;
	c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA GAMMA;
	c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p GAMMA_p;

} c_Fixed_Alloc_struct_Pwr_Ctrl_Param;

/* DEFINITION OF SUB-STRUCTURE c_Fixed_Alloc_struct_TBF_ST */
typedef struct _c_Fixed_Alloc_struct_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_Fixed_Alloc_struct_TBF_ST;


/*-----------------------------------*/
typedef struct _c_Fixed_Alloc_struct {
	ED_LONG Meas_Mapping;
	ED_BOOLEAN AllocBitmap_Present;
	ED_OCTET AllocBitmapLen;
	ED_BOOLEAN AllocBitmapLen_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_BOOLEAN BlocksOrBlockPeriods;
	ED_BOOLEAN BlocksOrBlockPeriods_Present;
	ED_OCTET DL_CTRL_TS;
	ED_BOOLEAN FINAL_ALLOC;
	ED_BOOLEAN Meas_Mapping_Present;
	ED_BOOLEAN Message_escape_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN Pwr_Ctrl_Param_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_OCTET UL_TS_ALLOC;
	ED_BOOLEAN UL_TS_ALLOC_Present;
	c_Fixed_Alloc_struct_AllocBitmap AllocBitmap;
	c_Fixed_Alloc_struct_Message_escape Message_escape;
	c_Fixed_Alloc_struct_Pwr_Ctrl_Param Pwr_Ctrl_Param;
	c_Fixed_Alloc_struct_TBF_ST TBF_ST;

}	c_Fixed_Alloc_struct;
#define INIT_c_Fixed_Alloc_struct(sp) ED_RESET_MEM ((sp), sizeof (c_Fixed_Alloc_struct))
#define FREE_c_Fixed_Alloc_struct(sp)
#define SETPRESENT_c_Fixed_Alloc_struct_AllocBitmap(sp,present) ((sp)->AllocBitmap_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_AllocBitmap(sp) ((sp)->AllocBitmap_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_AllocBitmapLen(sp,present) ((sp)->AllocBitmapLen_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_AllocBitmapLen(sp) ((sp)->AllocBitmapLen_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_BlocksOrBlockPeriods(sp,present) ((sp)->BlocksOrBlockPeriods_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_BlocksOrBlockPeriods(sp) ((sp)->BlocksOrBlockPeriods_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_Meas_Mapping(sp,present) ((sp)->Meas_Mapping_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_Meas_Mapping(sp) ((sp)->Meas_Mapping_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_Message_escape(sp,present) ((sp)->Message_escape_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_Message_escape(sp) ((sp)->Message_escape_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_Pwr_Ctrl_Param(sp,present) ((sp)->Pwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_Pwr_Ctrl_Param(sp) ((sp)->Pwr_Ctrl_Param_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_UL_TS_ALLOC(sp,present) ((sp)->UL_TS_ALLOC_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_UL_TS_ALLOC(sp) ((sp)->UL_TS_ALLOC_Present)

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_CHANNEL_CODING_COMMAND(var) var

/* Access member 'CHANNEL_CODING_COMMAND' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_CHANNEL_CODING_COMMAND(var) (&var)

/* Access member 'CONTROL_ACK' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_CONTROL_ACK(var) var

/* Access member 'CONTROL_ACK' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_CONTROL_ACK(var) (&var)

/* Access member 'DL_RLC_MODE' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_DL_RLC_MODE(var) var

/* Access member 'DL_RLC_MODE' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_DL_RLC_MODE(var) (&var)

/* Access member 'DL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_DL_TFI_ASSIGN(var) var

/* Access member 'DL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_DL_TFI_ASSIGN(var) (&var)

/* Access member 'DL_TS_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_DL_TS_ALLOC(var) var

/* Access member 'DL_TS_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_DL_TS_ALLOC(var) (&var)

/* Access member 'ALPHA' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_ALPHA(var) (&var)

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_EXTENDED_DYNAMIC_ALLOC(var) var

/* Access member 'EXTENDED_DYNAMIC_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_EXTENDED_DYNAMIC_ALLOC(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA(var) (&var)

/* Access member 'P0' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_P0(var) var

/* Access member 'P0' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_PR_MODE(var) (&var)

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(var) var

/* Access member 'RLC_DATA_BLOCKS_GRANTED' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_data(var) (&var)

/* Access member 'USF' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF(var) var

/* Access member 'USF' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF(var) (&var)

/* Access member 'USF_GRANULARITY' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_GRANULARITY(var) var

/* Access member 'USF_GRANULARITY' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_GRANULARITY(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p_data(var) (&var)

/* Access member 'USF_p' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p(var) var

/* Access member 'USF_p' of type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p(var) (&var)

/* Access member 'Dynamic_Alloc' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc(var) var

/* Access member 'Dynamic_Alloc' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc(var) (&var)

/* Access member 'AllocBitmap' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap(var) var

/* Access member 'AllocBitmap' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap(var) (&var)

/* Access member 'AllocBitmapLen' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmapLen(var) var

/* Access member 'AllocBitmapLen' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmapLen(var) (&var)

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(var) var

/* Access member 'BTS_PWR_CTRL_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(var) (&var)

/* Access member 'BlocksOrBlockPeriods' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BlocksOrBlockPeriods(var) var

/* Access member 'BlocksOrBlockPeriods' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BlocksOrBlockPeriods(var) (&var)

/* Access member 'DL_CTRL_TS' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_DL_CTRL_TS(var) var

/* Access member 'DL_CTRL_TS' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_DL_CTRL_TS(var) (&var)

/* Access member 'FINAL_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_FINAL_ALLOC(var) var

/* Access member 'FINAL_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_FINAL_ALLOC(var) (&var)

/* Access member 'Meas_Mapping' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping(var) var

/* Access member 'Meas_Mapping' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping(var) (&var)

/* Access member 'Message_escape' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape(var) var

/* Access member 'Message_escape' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape(var) (&var)

/* Access member 'P0' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_P0(var) var

/* Access member 'P0' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_P0(var) (&var)

/* Access member 'PR_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_PR_MODE(var) var

/* Access member 'PR_MODE' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_PR_MODE(var) (&var)

/* Access member 'ALPHA' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_ALPHA(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(var) (&var)

/* Access member 'Pwr_Ctrl_Param' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param(var) var

/* Access member 'Pwr_Ctrl_Param' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TFI_ASSIGN(var) (&var)

/* Access member 'UL_TS_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TS_ALLOC(var) var

/* Access member 'UL_TS_ALLOC' of type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TS_ALLOC(var) (&var)

/* Access member 'Fixed_Alloc' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc(var) var

/* Access member 'Fixed_Alloc' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc(var) (&var)

/* Access member 'ARFCN' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_ARFCN(var) var

/* Access member 'ARFCN' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_ARFCN(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx_data(var) (&var)

/* Access member 'ArfcnIdx' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) var

/* Access member 'ArfcnIdx' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(var) (&var)

/* Access member 'ArfcnIdxList' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) var

/* Access member 'ArfcnIdxList' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(var) (&var)

/* Access member 'Hsn' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) var

/* Access member 'Hsn' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_Hsn(var) (&var)

/* Access member 'MaBitMap' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) var

/* Access member 'MaBitMap' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(var) (&var)

/* Access member 'MaLen' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) var

/* Access member 'MaLen' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(var) (&var)

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) var

/* Access member 'data' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb_data(var) (&var)

/* Access member 'RflNb' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) var

/* Access member 'RflNb' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(var) (&var)

/* Access member 'RflNbList' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) var

/* Access member 'RflNbList' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(var) (&var)

/* Access member 'GPRS_MA' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) var

/* Access member 'GPRS_MA' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA(var) (&var)

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_MAIO(var) var

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_MAIO(var) (&var)

/* Access member 'Dir_enc_1' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1(var) var

/* Access member 'Dir_enc_1' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1(var) (&var)

/* Access member 'HSN' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_HSN(var) var

/* Access member 'HSN' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_HSN(var) (&var)

/* Access member 'Len_MA_Freq_List' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) var

/* Access member 'Len_MA_Freq_List' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_Len_MA_Freq_List(var) (&var)

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_MAIO(var) var

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_MAIO(var) (&var)

/* Access member 'MA_Freq_List' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) var

/* Access member 'MA_Freq_List' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2_MA_Freq_List(var) (&var)

/* Access member 'Dir_enc_2' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2(var) var

/* Access member 'Dir_enc_2' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2(var) (&var)

/* Access member 'CHANGE_MARK_1' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) var

/* Access member 'CHANGE_MARK_1' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(var) (&var)

/* Access member 'CHANGE_MARK_2' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) var

/* Access member 'CHANGE_MARK_2' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(var) (&var)

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_MAIO(var) var

/* Access member 'MAIO' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_MAIO(var) (&var)

/* Access member 'MA_NB' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_MA_NB(var) var

/* Access member 'MA_NB' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_MA_NB(var) (&var)

/* Access member 'Indir_enc' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc(var) var

/* Access member 'Indir_enc' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc(var) (&var)

/* Access member 'TSC' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param_TSC(var) var

/* Access member 'TSC' of type 'c_PTimeslot_Reconfigure_Msg_Freq_Param' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param_TSC(var) (&var)

/* Access member 'Freq_Param' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_Freq_Param(var) var

/* Access member 'Freq_Param' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_Freq_Param(var) (&var)

/* Access member 'DL_TA_INDEX' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_INDEX(var) var

/* Access member 'DL_TA_INDEX' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_INDEX(var) (&var)

/* Access member 'DL_TA_TS_NB' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_TS_NB(var) var

/* Access member 'DL_TA_TS_NB' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_TA_VALUE(var) (&var)

/* Access member 'UL_TA_INDEX' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_INDEX(var) var

/* Access member 'UL_TA_INDEX' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_INDEX(var) (&var)

/* Access member 'UL_TA_TS_NB' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_TS_NB(var) var

/* Access member 'UL_TA_TS_NB' of type 'c_PTimeslot_Reconfigure_Msg_GPTiming_Advance' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_TS_NB(var) (&var)

/* Access member 'GPTiming_Advance' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance(var) var

/* Access member 'GPTiming_Advance' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance(var) (&var)

/* Access member 'PExtended_Timing_Advance' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_PExtended_Timing_Advance(var) var

/* Access member 'PExtended_Timing_Advance' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_PExtended_Timing_Advance(var) (&var)

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg' as a variable reference */
#define VAR_c_PTimeslot_Reconfigure_Msg_UL_TFI_ASSIGN(var) var

/* Access member 'UL_TFI_ASSIGN' of type 'c_PTimeslot_Reconfigure_Msg' as a pointer */
#define PTR_c_PTimeslot_Reconfigure_Msg_UL_TFI_ASSIGN(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA */
typedef struct _c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST */
typedef struct _c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF */
typedef struct _c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF {
	ED_OCTET data [8];
	int items;
} c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p */
typedef struct _c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p {
	ED_BOOLEAN data [8];
	int items;
} c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc */
typedef struct _c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc {
	ED_OCTET ALPHA;
	ED_BOOLEAN ALPHA_Present;
	ED_BOOLEAN EXTENDED_DYNAMIC_ALLOC;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_OCTET RLC_DATA_BLOCKS_GRANTED;
	ED_BOOLEAN RLC_DATA_BLOCKS_GRANTED_Present;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_BOOLEAN USF_GRANULARITY;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA GAMMA;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST TBF_ST;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF USF;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p USF_p;

} c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc;

/* DEFINITION OF BINARY c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap {
#ifdef __EGPRS__
	ED_BYTE value [16]; // changed by dingmx from 8 to 16 20061031
#else
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
#endif
	int usedBits;
} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF BINARY c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param {
	ED_OCTET ALPHA;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA GAMMA;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p GAMMA_p;

} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST;
#ifdef __EGPRS__
//	add by dingmx begin 20061031
//< Measurement Mapping struct > ::=
//	< Measurement Starting Time : < Starting Frame Number Description IE >
//	< MEASUREMENT_INTERVAL : bit (5) >
//	< MEASUREMENT_BITMAP : bit (8) > ;
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping_struct {
ED_OCTET INTERVAL ;
ED_OCTET BITMAP ;
c_Starting_Frame_Number_Description Meas_Starting_Time;
}c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping_struct;
//	add by dingmx end 20061031
#endif
/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Fixed_Alloc */
typedef struct _c_PTimeslot_Reconfigure_Msg_Fixed_Alloc {
	ED_LONG Meas_Mapping;
	ED_BOOLEAN AllocBitmap_Present;
	ED_OCTET AllocBitmapLen;
	ED_BOOLEAN AllocBitmapLen_Present;
	ED_BOOLEAN BTS_PWR_CTRL_MODE;
	ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
	ED_BOOLEAN BlocksOrBlockPeriods;
	ED_BOOLEAN BlocksOrBlockPeriods_Present;
	ED_OCTET DL_CTRL_TS;
	ED_BOOLEAN FINAL_ALLOC;
	ED_BOOLEAN Meas_Mapping_Present;
	ED_BOOLEAN Message_escape_Present;
	ED_OCTET P0;
	ED_BOOLEAN P0_Present;
	ED_BOOLEAN PR_MODE;
	ED_BOOLEAN PR_MODE_Present;
	ED_BOOLEAN Pwr_Ctrl_Param_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	ED_OCTET UL_TS_ALLOC;
	ED_BOOLEAN UL_TS_ALLOC_Present;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap AllocBitmap;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape Message_escape;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param Pwr_Ctrl_Param;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST TBF_ST;

} c_PTimeslot_Reconfigure_Msg_Fixed_Alloc;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx {
	ED_OCTET data [20];
	int items;
} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList {
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx ArfcnIdx;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList;

/* DEFINITION OF BINARY c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb {
	ED_OCTET data [20];
	int items;
} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList {
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb RflNb;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA {
	ED_BOOLEAN ArfcnIdxList_Present;
	ED_OCTET Hsn;
	ED_BOOLEAN MaBitMap_Present;
	ED_OCTET MaLen;
	ED_BOOLEAN MaLen_Present;
	ED_BOOLEAN RflNbList_Present;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList ArfcnIdxList;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap MaBitMap;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList RflNbList;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1 */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1 {
	ED_OCTET MAIO;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA GPRS_MA;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2 */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2 {
	ED_OCTET HSN;
	ED_OCTET Len_MA_Freq_List;
	ED_OCTET MAIO;
	c_TFrqList MA_Freq_List;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc {
	ED_OCTET CHANGE_MARK_1;
	ED_BOOLEAN CHANGE_MARK_1_Present;
	ED_OCTET CHANGE_MARK_2;
	ED_BOOLEAN CHANGE_MARK_2_Present;
	ED_OCTET MAIO;
	ED_OCTET MA_NB;

} c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_Freq_Param */
typedef struct _c_PTimeslot_Reconfigure_Msg_Freq_Param {
	ED_SHORT ARFCN;
	ED_BOOLEAN ARFCN_Present;
	ED_BOOLEAN Dir_enc_1_Present;
	ED_BOOLEAN Dir_enc_2_Present;
	ED_BOOLEAN Indir_enc_Present;
	ED_OCTET TSC;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1 Dir_enc_1;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2 Dir_enc_2;
	c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc Indir_enc;

} c_PTimeslot_Reconfigure_Msg_Freq_Param;

/* DEFINITION OF SUB-STRUCTURE c_PTimeslot_Reconfigure_Msg_GPTiming_Advance */
typedef struct _c_PTimeslot_Reconfigure_Msg_GPTiming_Advance {
	ED_OCTET DL_TA_INDEX;
	ED_BOOLEAN DL_TA_INDEX_Present;
	ED_OCTET DL_TA_TS_NB;
	ED_BOOLEAN DL_TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;
	ED_OCTET UL_TA_INDEX;
	ED_BOOLEAN UL_TA_INDEX_Present;
	ED_OCTET UL_TA_TS_NB;
	ED_BOOLEAN UL_TA_TS_NB_Present;

} c_PTimeslot_Reconfigure_Msg_GPTiming_Advance;

#ifndef __EGPRS__
/*-----------------------------------*/
typedef struct _c_PTimeslot_Reconfigure_Msg {
	ED_OCTET CHANNEL_CODING_COMMAND;
	ED_BOOLEAN CHANNEL_CODING_COMMAND_Present;
	ED_BOOLEAN CONTROL_ACK;
	ED_BOOLEAN CONTROL_ACK_Present;
	ED_BOOLEAN DL_RLC_MODE;
	ED_BOOLEAN DL_RLC_MODE_Present;
	ED_OCTET DL_TFI_ASSIGN;
	ED_BOOLEAN DL_TFI_ASSIGN_Present;
	ED_OCTET DL_TS_ALLOC;
	ED_BOOLEAN DL_TS_ALLOC_Present;
	ED_BOOLEAN Dynamic_Alloc_Present;
	ED_BOOLEAN Fixed_Alloc_Present;
	ED_BOOLEAN Freq_Param_Present;
	ED_BOOLEAN GPTiming_Advance_Present;
	ED_OCTET PExtended_Timing_Advance;
	ED_BOOLEAN PExtended_Timing_Advance_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc Dynamic_Alloc;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc Fixed_Alloc;
	c_PTimeslot_Reconfigure_Msg_Freq_Param Freq_Param;
	c_PTimeslot_Reconfigure_Msg_GPTiming_Advance GPTiming_Advance;

}	c_PTimeslot_Reconfigure_Msg;
#endif
#define INIT_c_PTimeslot_Reconfigure_Msg(sp) ED_RESET_MEM ((sp), sizeof (c_PTimeslot_Reconfigure_Msg))
#define FREE_c_PTimeslot_Reconfigure_Msg(sp)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_CHANNEL_CODING_COMMAND(sp,present) ((sp)->CHANNEL_CODING_COMMAND_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_CHANNEL_CODING_COMMAND(sp) ((sp)->CHANNEL_CODING_COMMAND_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_CONTROL_ACK(sp,present) ((sp)->CONTROL_ACK_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_CONTROL_ACK(sp) ((sp)->CONTROL_ACK_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_RLC_MODE(sp,present) ((sp)->DL_RLC_MODE_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_RLC_MODE(sp) ((sp)->DL_RLC_MODE_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_TFI_ASSIGN(sp,present) ((sp)->DL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_TFI_ASSIGN(sp) ((sp)->DL_TFI_ASSIGN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_TS_ALLOC(sp,present) ((sp)->DL_TS_ALLOC_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_DL_TS_ALLOC(sp) ((sp)->DL_TS_ALLOC_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc(sp,present) ((sp)->Dynamic_Alloc_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc(sp) ((sp)->Dynamic_Alloc_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc(sp,present) ((sp)->Fixed_Alloc_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc(sp) ((sp)->Fixed_Alloc_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param(sp,present) ((sp)->Freq_Param_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param(sp) ((sp)->Freq_Param_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance(sp,present) ((sp)->GPTiming_Advance_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance(sp) ((sp)->GPTiming_Advance_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_PExtended_Timing_Advance(sp,present) ((sp)->PExtended_Timing_Advance_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_PExtended_Timing_Advance(sp) ((sp)->PExtended_Timing_Advance_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)

/* Access member 'DL_TA_INDEX' of type 'c_GPTiming_Advance_IE' as a variable reference */
#define VAR_c_GPTiming_Advance_IE_DL_TA_INDEX(var) var

/* Access member 'DL_TA_INDEX' of type 'c_GPTiming_Advance_IE' as a pointer */
#define PTR_c_GPTiming_Advance_IE_DL_TA_INDEX(var) (&var)

/* Access member 'DL_TA_TS_NB' of type 'c_GPTiming_Advance_IE' as a variable reference */
#define VAR_c_GPTiming_Advance_IE_DL_TA_TS_NB(var) var

/* Access member 'DL_TA_TS_NB' of type 'c_GPTiming_Advance_IE' as a pointer */
#define PTR_c_GPTiming_Advance_IE_DL_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_GPTiming_Advance_IE' as a variable reference */
#define VAR_c_GPTiming_Advance_IE_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_GPTiming_Advance_IE' as a pointer */
#define PTR_c_GPTiming_Advance_IE_TA_VALUE(var) (&var)

/* Access member 'UL_TA_INDEX' of type 'c_GPTiming_Advance_IE' as a variable reference */
#define VAR_c_GPTiming_Advance_IE_UL_TA_INDEX(var) var

/* Access member 'UL_TA_INDEX' of type 'c_GPTiming_Advance_IE' as a pointer */
#define PTR_c_GPTiming_Advance_IE_UL_TA_INDEX(var) (&var)

/* Access member 'UL_TA_TS_NB' of type 'c_GPTiming_Advance_IE' as a variable reference */
#define VAR_c_GPTiming_Advance_IE_UL_TA_TS_NB(var) var

/* Access member 'UL_TA_TS_NB' of type 'c_GPTiming_Advance_IE' as a pointer */
#define PTR_c_GPTiming_Advance_IE_UL_TA_TS_NB(var) (&var)


/*-----------------------------------*/
typedef struct _c_GPTiming_Advance_IE {
	ED_OCTET DL_TA_INDEX;
	ED_BOOLEAN DL_TA_INDEX_Present;
	ED_OCTET DL_TA_TS_NB;
	ED_BOOLEAN DL_TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;
	ED_OCTET UL_TA_INDEX;
	ED_BOOLEAN UL_TA_INDEX_Present;
	ED_OCTET UL_TA_TS_NB;
	ED_BOOLEAN UL_TA_TS_NB_Present;

}	c_GPTiming_Advance_IE;
#define INIT_c_GPTiming_Advance_IE(sp) ED_RESET_MEM ((sp), sizeof (c_GPTiming_Advance_IE))
#define FREE_c_GPTiming_Advance_IE(sp)
#define SETPRESENT_c_GPTiming_Advance_IE_DL_TA_INDEX(sp,present) ((sp)->DL_TA_INDEX_Present = present)
#define GETPRESENT_c_GPTiming_Advance_IE_DL_TA_INDEX(sp) ((sp)->DL_TA_INDEX_Present)
#define SETPRESENT_c_GPTiming_Advance_IE_DL_TA_TS_NB(sp,present) ((sp)->DL_TA_TS_NB_Present = present)
#define GETPRESENT_c_GPTiming_Advance_IE_DL_TA_TS_NB(sp) ((sp)->DL_TA_TS_NB_Present)
#define SETPRESENT_c_GPTiming_Advance_IE_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_GPTiming_Advance_IE_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
#define SETPRESENT_c_GPTiming_Advance_IE_UL_TA_INDEX(sp,present) ((sp)->UL_TA_INDEX_Present = present)
#define GETPRESENT_c_GPTiming_Advance_IE_UL_TA_INDEX(sp) ((sp)->UL_TA_INDEX_Present)
#define SETPRESENT_c_GPTiming_Advance_IE_UL_TA_TS_NB(sp,present) ((sp)->UL_TA_TS_NB_Present = present)
#define GETPRESENT_c_GPTiming_Advance_IE_UL_TA_TS_NB(sp) ((sp)->UL_TA_TS_NB_Present)

/* Access member 'D' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_D(var) var

/* Access member 'D' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_D(var) (&var)

/* Access member 'DownlinkTFI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_DownlinkTFI(var) var

/* Access member 'DownlinkTFI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_DownlinkTFI(var) (&var)

/* Access member 'FS' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_FS(var) var

/* Access member 'FS' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_FS(var) (&var)

/* Access member 'MessageType' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_MessageType(var) var

/* Access member 'MessageType' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_MessageType(var) (&var)

/* Access member 'DlTFI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_DlTFI(var) (&var)

/* Access member 'RA' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef(var) (&var)

/* Access member 'TLLI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_TLLI(var) var

/* Access member 'TLLI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_TLLI(var) (&var)

/* Access member 'UlTFI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_UlTFI(var) var

/* Access member 'UlTFI' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_UlTFI(var) (&var)

/* Access member 'WAIT_IND' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND(var) var

/* Access member 'WAIT_IND' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND(var) (&var)

/* Access member 'WAIT_IND_SIZE' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND_SIZE(var) var

/* Access member 'WAIT_IND_SIZE' of type 'c_MacCtrlHder_PAccRej_Add_Reject_data' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND_SIZE(var) (&var)

/* Access member 'data' of type 'c_MacCtrlHder_PAccRej_Add_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject_data(var) var

/* Access member 'data' of type 'c_MacCtrlHder_PAccRej_Add_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject_data(var) (&var)

/* Access member 'Add_Reject' of type 'c_MacCtrlHder_PAccRej' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Add_Reject(var) var

/* Access member 'Add_Reject' of type 'c_MacCtrlHder_PAccRej' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Add_Reject(var) (&var)

/* Access member 'DlTFI' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_DlTFI(var) (&var)

/* Access member 'RA' of type 'c_MacCtrlHder_PAccRej_Reject_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_MacCtrlHder_PAccRej_Reject_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_MacCtrlHder_PAccRej_Reject_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_MacCtrlHder_PAccRej_Reject_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_PReqRef(var) (&var)

/* Access member 'TLLI' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_TLLI(var) var

/* Access member 'TLLI' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_TLLI(var) (&var)

/* Access member 'UlTFI' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_UlTFI(var) var

/* Access member 'UlTFI' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_UlTFI(var) (&var)

/* Access member 'WAIT_IND' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_WAIT_IND(var) var

/* Access member 'WAIT_IND' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_WAIT_IND(var) (&var)

/* Access member 'WAIT_IND_SIZE' of type 'c_MacCtrlHder_PAccRej_Reject' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject_WAIT_IND_SIZE(var) var

/* Access member 'WAIT_IND_SIZE' of type 'c_MacCtrlHder_PAccRej_Reject' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject_WAIT_IND_SIZE(var) (&var)

/* Access member 'Reject' of type 'c_MacCtrlHder_PAccRej' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej_Reject(var) var

/* Access member 'Reject' of type 'c_MacCtrlHder_PAccRej' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej_Reject(var) (&var)

/* Access member 'PAccRej' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_PAccRej(var) var

/* Access member 'PAccRej' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_PAccRej(var) (&var)

/* Access member 'PR' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_PR(var) var

/* Access member 'PR' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_PR(var) (&var)

/* Access member 'RA' of type 'c_MacCtrlHder_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_MacCtrlHder_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_MacCtrlHder_PReqRef' as a variable reference */
#define VAR_c_MacCtrlHder_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_MacCtrlHder_PReqRef' as a pointer */
#define PTR_c_MacCtrlHder_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_PReqRef(var) (&var)

/* Access member 'PayloadType' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_PayloadType(var) var

/* Access member 'PayloadType' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_PayloadType(var) (&var)

/* Access member 'data' of type 'c_MacCtrlHder_PersistenceLevel' as a variable reference */
#define VAR_c_MacCtrlHder_PersistenceLevel_data(var) var

/* Access member 'data' of type 'c_MacCtrlHder_PersistenceLevel' as a pointer */
#define PTR_c_MacCtrlHder_PersistenceLevel_data(var) (&var)

/* Access member 'PersistenceLevel' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_PersistenceLevel(var) var

/* Access member 'PersistenceLevel' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_PersistenceLevel(var) (&var)

/* Access member 'RBSN' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_RBSN(var) var

/* Access member 'RBSN' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_RBSN(var) (&var)

/* Access member 'RRBP' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_RRBP(var) var

/* Access member 'RRBP' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_RRBP(var) (&var)

/* Access member 'RTI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_RTI(var) var

/* Access member 'RTI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_RTI(var) (&var)

/* Access member 'SP' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_SP(var) var

/* Access member 'SP' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_SP(var) (&var)

/* Access member 'TFI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_TFI(var) var

/* Access member 'TFI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_TFI(var) (&var)

/* Access member 'TLLI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_TLLI(var) var

/* Access member 'TLLI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_TLLI(var) (&var)

/* Access member 'TQI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_TQI(var) var

/* Access member 'TQI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_TQI(var) (&var)

/* Access member 'data' of type 'c_MacCtrlHder_TQIInd' as a variable reference */
#define VAR_c_MacCtrlHder_TQIInd_data(var) var

/* Access member 'data' of type 'c_MacCtrlHder_TQIInd' as a pointer */
#define PTR_c_MacCtrlHder_TQIInd_data(var) (&var)

/* Access member 'TQIInd' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_TQIInd(var) var

/* Access member 'TQIInd' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_TQIInd(var) (&var)

/* Access member 'TypeOfAck' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_TypeOfAck(var) var

/* Access member 'TypeOfAck' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_TypeOfAck(var) (&var)

/* Access member 'UplinkTFI' of type 'c_MacCtrlHder' as a variable reference */
#define VAR_c_MacCtrlHder_UplinkTFI(var) var

/* Access member 'UplinkTFI' of type 'c_MacCtrlHder' as a pointer */
#define PTR_c_MacCtrlHder_UplinkTFI(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef */
typedef struct _c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej_Add_Reject_data */
typedef struct _c_MacCtrlHder_PAccRej_Add_Reject_data {
	ED_LONG TLLI;
	ED_OCTET DlTFI;
	ED_BOOLEAN DlTFI_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_BOOLEAN TLLI_Present;
	ED_OCTET UlTFI;
	ED_BOOLEAN UlTFI_Present;
	ED_OCTET WAIT_IND;
	ED_BOOLEAN WAIT_IND_Present;
	ED_BOOLEAN WAIT_IND_SIZE;
	ED_BOOLEAN WAIT_IND_SIZE_Present;
	c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef PReqRef;

} c_MacCtrlHder_PAccRej_Add_Reject_data;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej_Add_Reject */
typedef struct _c_MacCtrlHder_PAccRej_Add_Reject {
	c_MacCtrlHder_PAccRej_Add_Reject_data data [20];
	int items;
} c_MacCtrlHder_PAccRej_Add_Reject;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej_Reject_PReqRef */
typedef struct _c_MacCtrlHder_PAccRej_Reject_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_MacCtrlHder_PAccRej_Reject_PReqRef;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej_Reject */
typedef struct _c_MacCtrlHder_PAccRej_Reject {
	ED_LONG TLLI;
	ED_OCTET DlTFI;
	ED_BOOLEAN DlTFI_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_BOOLEAN TLLI_Present;
	ED_OCTET UlTFI;
	ED_BOOLEAN UlTFI_Present;
	ED_OCTET WAIT_IND;
	ED_BOOLEAN WAIT_IND_Present;
	ED_BOOLEAN WAIT_IND_SIZE;
	ED_BOOLEAN WAIT_IND_SIZE_Present;
	c_MacCtrlHder_PAccRej_Reject_PReqRef PReqRef;

} c_MacCtrlHder_PAccRej_Reject;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PAccRej */
typedef struct _c_MacCtrlHder_PAccRej {
	c_MacCtrlHder_PAccRej_Add_Reject Add_Reject;
	c_MacCtrlHder_PAccRej_Reject Reject;

} c_MacCtrlHder_PAccRej;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PReqRef */
typedef struct _c_MacCtrlHder_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_MacCtrlHder_PReqRef;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_PersistenceLevel */
typedef struct _c_MacCtrlHder_PersistenceLevel {
	ED_OCTET data [4];
	int items;
} c_MacCtrlHder_PersistenceLevel;

/* DEFINITION OF SUB-STRUCTURE c_MacCtrlHder_TQIInd */
typedef struct _c_MacCtrlHder_TQIInd {
	ED_SHORT data [2];
	int items;
} c_MacCtrlHder_TQIInd;


/*-----------------------------------*/
typedef struct _c_MacCtrlHder {
	ED_LONG TLLI;
	ED_SHORT TQI;
	ED_BOOLEAN D;
	ED_BOOLEAN D_Present;
	ED_OCTET DownlinkTFI;
	ED_BOOLEAN DownlinkTFI_Present;
	ED_BOOLEAN FS;
	ED_BOOLEAN FS_Present;
	ED_OCTET MessageType;
	ED_BOOLEAN MessageType_Present;
	ED_BOOLEAN PAccRej_Present;
	ED_OCTET PR;
	ED_BOOLEAN PR_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_OCTET PayloadType;
	ED_BOOLEAN RBSN;
	ED_BOOLEAN RBSN_Present;
	ED_OCTET RRBP;
	ED_OCTET RTI;
	ED_BOOLEAN RTI_Present;
	ED_BOOLEAN SP;
	ED_OCTET TFI;
	ED_BOOLEAN TFI_Present;
	ED_BOOLEAN TLLI_Present;
	ED_BOOLEAN TQI_Present;
	ED_BOOLEAN TypeOfAck;
	ED_BOOLEAN TypeOfAck_Present;
	ED_OCTET UplinkTFI;
	ED_BOOLEAN UplinkTFI_Present;
	c_MacCtrlHder_PAccRej PAccRej;
	c_MacCtrlHder_PReqRef PReqRef;
	c_MacCtrlHder_PersistenceLevel PersistenceLevel;
	c_MacCtrlHder_TQIInd TQIInd;

}	c_MacCtrlHder;
#define INIT_c_MacCtrlHder(sp) ED_RESET_MEM ((sp), sizeof (c_MacCtrlHder))
#define FREE_c_MacCtrlHder(sp)
#define SETPRESENT_c_MacCtrlHder_D(sp,present) ((sp)->D_Present = present)
#define GETPRESENT_c_MacCtrlHder_D(sp) ((sp)->D_Present)
#define SETPRESENT_c_MacCtrlHder_DownlinkTFI(sp,present) ((sp)->DownlinkTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_DownlinkTFI(sp) ((sp)->DownlinkTFI_Present)
#define SETPRESENT_c_MacCtrlHder_FS(sp,present) ((sp)->FS_Present = present)
#define GETPRESENT_c_MacCtrlHder_FS(sp) ((sp)->FS_Present)
#define SETPRESENT_c_MacCtrlHder_MessageType(sp,present) ((sp)->MessageType_Present = present)
#define GETPRESENT_c_MacCtrlHder_MessageType(sp) ((sp)->MessageType_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej(sp,present) ((sp)->PAccRej_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej(sp) ((sp)->PAccRej_Present)
#define SETPRESENT_c_MacCtrlHder_PR(sp,present) ((sp)->PR_Present = present)
#define GETPRESENT_c_MacCtrlHder_PR(sp) ((sp)->PR_Present)
#define SETPRESENT_c_MacCtrlHder_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_MacCtrlHder_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_MacCtrlHder_RBSN(sp,present) ((sp)->RBSN_Present = present)
#define GETPRESENT_c_MacCtrlHder_RBSN(sp) ((sp)->RBSN_Present)
#define SETPRESENT_c_MacCtrlHder_RTI(sp,present) ((sp)->RTI_Present = present)
#define GETPRESENT_c_MacCtrlHder_RTI(sp) ((sp)->RTI_Present)
#define SETPRESENT_c_MacCtrlHder_TFI(sp,present) ((sp)->TFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_TFI(sp) ((sp)->TFI_Present)
#define SETPRESENT_c_MacCtrlHder_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_MacCtrlHder_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_MacCtrlHder_TQI(sp,present) ((sp)->TQI_Present = present)
#define GETPRESENT_c_MacCtrlHder_TQI(sp) ((sp)->TQI_Present)
#define SETPRESENT_c_MacCtrlHder_TypeOfAck(sp,present) ((sp)->TypeOfAck_Present = present)
#define GETPRESENT_c_MacCtrlHder_TypeOfAck(sp) ((sp)->TypeOfAck_Present)
#define SETPRESENT_c_MacCtrlHder_UplinkTFI(sp,present) ((sp)->UplinkTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_UplinkTFI(sp) ((sp)->UplinkTFI_Present)

/* Access member 'RRBP' of type 'c_Mac_First_Octet' as a variable reference */
#define VAR_c_Mac_First_Octet_RRBP(var) var

/* Access member 'RRBP' of type 'c_Mac_First_Octet' as a pointer */
#define PTR_c_Mac_First_Octet_RRBP(var) (&var)

/* Access member 'SP' of type 'c_Mac_First_Octet' as a variable reference */
#define VAR_c_Mac_First_Octet_SP(var) var

/* Access member 'SP' of type 'c_Mac_First_Octet' as a pointer */
#define PTR_c_Mac_First_Octet_SP(var) (&var)


/*-----------------------------------*/
typedef struct _c_Mac_First_Octet {
	ED_OCTET RRBP;
	ED_BOOLEAN SP;

}	c_Mac_First_Octet;
#define INIT_c_Mac_First_Octet(sp) ED_RESET_MEM ((sp), sizeof (c_Mac_First_Octet))
#define FREE_c_Mac_First_Octet(sp)

/* Access member 'RA' of type 'c_PReqRef' as a variable reference */
#define VAR_c_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_PReqRef' as a pointer */
#define PTR_c_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_PReqRef' as a variable reference */
#define VAR_c_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_PReqRef' as a pointer */
#define PTR_c_PReqRef_ST(var) (&var)


/*-----------------------------------*/
typedef struct _c_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

}	c_PReqRef;
#define INIT_c_PReqRef(sp) ED_RESET_MEM ((sp), sizeof (c_PReqRef))
#define FREE_c_PReqRef(sp)

/* Access member 'AckNack_Desc' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_AckNack_Desc(var) var

/* Access member 'AckNack_Desc' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_AckNack_Desc(var) (&var)

/* Access member 'ChanCodCmd' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_ChanCodCmd(var) var

/* Access member 'ChanCodCmd' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_ChanCodCmd(var) (&var)

/* Access member 'ContResTLLI' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_ContResTLLI(var) var

/* Access member 'ContResTLLI' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_ContResTLLI(var) (&var)

/* Access member 'ExtLen' of type 'c_PUplinkAckNack_Extension_Bits' as a variable reference */
#define VAR_c_PUplinkAckNack_Extension_Bits_ExtLen(var) var

/* Access member 'ExtLen' of type 'c_PUplinkAckNack_Extension_Bits' as a pointer */
#define PTR_c_PUplinkAckNack_Extension_Bits_ExtLen(var) (&var)

/* Access member 'Extension_Bits' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_Extension_Bits(var) var

/* Access member 'Extension_Bits' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_Extension_Bits(var) (&var)

/* Access member 'AllocBitmap' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_AllocBitmap(var) var

/* Access member 'AllocBitmap' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_AllocBitmap(var) (&var)

/* Access member 'AllocBitmapLen' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_AllocBitmapLen(var) var

/* Access member 'AllocBitmapLen' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_AllocBitmapLen(var) (&var)

/* Access member 'BlocksOrBlockPeriods' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_BlocksOrBlockPeriods(var) var

/* Access member 'BlocksOrBlockPeriods' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_BlocksOrBlockPeriods(var) (&var)

/* Access member 'FinalAlloc' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_FinalAlloc(var) var

/* Access member 'FinalAlloc' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_FinalAlloc(var) (&var)

/* Access member 'Absolute_FN' of type 'c_PUplinkAckNack_FixedAlloc_TBF_ST' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_PUplinkAckNack_FixedAlloc_TBF_ST' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_PUplinkAckNack_FixedAlloc_TBF_ST' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_PUplinkAckNack_FixedAlloc_TBF_ST' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_TBF_ST(var) (&var)

/* Access member 'TSAlloc' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_TSAlloc(var) var

/* Access member 'TSAlloc' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_TSAlloc(var) (&var)

/* Access member 'TSOverride' of type 'c_PUplinkAckNack_FixedAlloc' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc_TSOverride(var) var

/* Access member 'TSOverride' of type 'c_PUplinkAckNack_FixedAlloc' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc_TSOverride(var) (&var)

/* Access member 'FixedAlloc' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_FixedAlloc(var) var

/* Access member 'FixedAlloc' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_FixedAlloc(var) (&var)

/* Access member 'PExtTimingAdv' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_PExtTimingAdv(var) var

/* Access member 'PExtTimingAdv' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_PExtTimingAdv(var) (&var)

/* Access member 'TA_INDEX' of type 'c_PUplinkAckNack_PTimingAdv' as a variable reference */
#define VAR_c_PUplinkAckNack_PTimingAdv_TA_INDEX(var) var

/* Access member 'TA_INDEX' of type 'c_PUplinkAckNack_PTimingAdv' as a pointer */
#define PTR_c_PUplinkAckNack_PTimingAdv_TA_INDEX(var) (&var)

/* Access member 'TA_TS_NB' of type 'c_PUplinkAckNack_PTimingAdv' as a variable reference */
#define VAR_c_PUplinkAckNack_PTimingAdv_TA_TS_NB(var) var

/* Access member 'TA_TS_NB' of type 'c_PUplinkAckNack_PTimingAdv' as a pointer */
#define PTR_c_PUplinkAckNack_PTimingAdv_TA_TS_NB(var) (&var)

/* Access member 'TA_VALUE' of type 'c_PUplinkAckNack_PTimingAdv' as a variable reference */
#define VAR_c_PUplinkAckNack_PTimingAdv_TA_VALUE(var) var

/* Access member 'TA_VALUE' of type 'c_PUplinkAckNack_PTimingAdv' as a pointer */
#define PTR_c_PUplinkAckNack_PTimingAdv_TA_VALUE(var) (&var)

/* Access member 'PTimingAdv' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_PTimingAdv(var) var

/* Access member 'PTimingAdv' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_PTimingAdv(var) (&var)

/* Access member 'ALPHA' of type 'c_PUplinkAckNack_PwrCtrlParam' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam_ALPHA(var) var

/* Access member 'ALPHA' of type 'c_PUplinkAckNack_PwrCtrlParam' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam_ALPHA(var) (&var)

/* Access member 'data' of type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_data(var) var

/* Access member 'data' of type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_data(var) (&var)

/* Access member 'GAMMA' of type 'c_PUplinkAckNack_PwrCtrlParam' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam_GAMMA(var) var

/* Access member 'GAMMA' of type 'c_PUplinkAckNack_PwrCtrlParam' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam_GAMMA(var) (&var)

/* Access member 'data' of type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA_p' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p_data(var) var

/* Access member 'data' of type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA_p' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p_data(var) (&var)

/* Access member 'GAMMA_p' of type 'c_PUplinkAckNack_PwrCtrlParam' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p(var) var

/* Access member 'GAMMA_p' of type 'c_PUplinkAckNack_PwrCtrlParam' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p(var) (&var)

/* Access member 'PwrCtrlParam' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_PwrCtrlParam(var) var

/* Access member 'PwrCtrlParam' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_PwrCtrlParam(var) (&var)

/* Access member 'TBF_EST' of type 'c_PUplinkAckNack' as a variable reference */
#define VAR_c_PUplinkAckNack_TBF_EST(var) var

/* Access member 'TBF_EST' of type 'c_PUplinkAckNack' as a pointer */
#define PTR_c_PUplinkAckNack_TBF_EST(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_Extension_Bits */
typedef struct _c_PUplinkAckNack_Extension_Bits {
	ED_OCTET ExtLen;

} c_PUplinkAckNack_Extension_Bits;

/* DEFINITION OF BINARY c_PUplinkAckNack_FixedAlloc_AllocBitmap */
typedef struct _c_PUplinkAckNack_FixedAlloc_AllocBitmap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_PUplinkAckNack_FixedAlloc_AllocBitmap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_PUplinkAckNack_FixedAlloc_AllocBitmap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_FixedAlloc_TBF_ST */
typedef struct _c_PUplinkAckNack_FixedAlloc_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_PUplinkAckNack_FixedAlloc_TBF_ST;

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_FixedAlloc */
typedef struct _c_PUplinkAckNack_FixedAlloc {
	ED_BOOLEAN AllocBitmap_Present;
	ED_OCTET AllocBitmapLen;
	ED_BOOLEAN AllocBitmapLen_Present;
	ED_BOOLEAN BlocksOrBlockPeriods;
	ED_BOOLEAN BlocksOrBlockPeriods_Present;
	ED_BOOLEAN FinalAlloc;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET TSAlloc;
	ED_BOOLEAN TSAlloc_Present;
	ED_OCTET TSOverride;
	ED_BOOLEAN TSOverride_Present;
	c_PUplinkAckNack_FixedAlloc_AllocBitmap AllocBitmap;
	c_PUplinkAckNack_FixedAlloc_TBF_ST TBF_ST;

} c_PUplinkAckNack_FixedAlloc;

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_PTimingAdv */
typedef struct _c_PUplinkAckNack_PTimingAdv {
	ED_OCTET TA_INDEX;
	ED_BOOLEAN TA_INDEX_Present;
	ED_OCTET TA_TS_NB;
	ED_BOOLEAN TA_TS_NB_Present;
	ED_OCTET TA_VALUE;
	ED_BOOLEAN TA_VALUE_Present;

} c_PUplinkAckNack_PTimingAdv;

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_PwrCtrlParam_GAMMA */
typedef struct _c_PUplinkAckNack_PwrCtrlParam_GAMMA {
	ED_OCTET data [8];
	int items;
} c_PUplinkAckNack_PwrCtrlParam_GAMMA;

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_PwrCtrlParam_GAMMA_p */
typedef struct _c_PUplinkAckNack_PwrCtrlParam_GAMMA_p {
	ED_BOOLEAN data [8];
	int items;
} c_PUplinkAckNack_PwrCtrlParam_GAMMA_p;

/* DEFINITION OF SUB-STRUCTURE c_PUplinkAckNack_PwrCtrlParam */
typedef struct _c_PUplinkAckNack_PwrCtrlParam {
	ED_OCTET ALPHA;
	c_PUplinkAckNack_PwrCtrlParam_GAMMA GAMMA;
	c_PUplinkAckNack_PwrCtrlParam_GAMMA_p GAMMA_p;

} c_PUplinkAckNack_PwrCtrlParam;
#ifdef __EGPRS__

 //		0	< EGPRS Ack/Nack Description struct >     -- This IE fills rest of message
 //		| 1	< Length L : bit (8) >				-- Value part of this IE is of length L
 //			{ < bit (val(Length L)) > & < EGPRS Ack/Nack Description struct > } ;
 //< EGPRS Ack/Nack Description struct > ::=
 //		< FINAL_ACK_INDICATION : bit (1) >
 //		< BEGINNING_OF_WINDOW : bit (1) >
 //		< END_OF_WINDOW : bit (1) >
 //		< STARTING_SEQUENCE_NUMBER : bit (11) >
 //		{ 0 | 1	< COMPRESSED_BITMAP_LENGTH: bit (7) >
 //			< COMPRESSED_BITMAP_STARTING_COLOR_CODE: bit (1) >
 //			< COMPRESSED_RECEIVED_BLOCK_BITMAP : 
 //				bit (val(COMPRESSED_BITMAP_LENGTH)) > }
 //		< UNCOMPRESSED_RECEIVED_BLOCK_BITMAP: bit** > ;
// add by dingmx 20061031 for EDGE
typedef struct _c_EGPRS_Ack_Nack_Description_IE
{
    ED_BOOLEAN  FINAL_ACK_INDICATION_Present;
    ED_OCTET FINAL_ACK_INDICATION;
    ED_BOOLEAN  BEGINNING_OF_WINDOW_Present;
    ED_OCTET 	 BEGINNING_OF_WINDOW;
    ED_BOOLEAN END_OF_WINDOW_Present;
    ED_OCTET 	 END_OF_WINDOW;
    ED_BOOLEAN STARTING_SEQUENCE_NUMBER_Present;
    ED_SHORT 	 STARTING_SEQUENCE_NUMBER;	
    ED_BOOLEAN COMPRESSED_BITMAP_Present;//add by lincq	
    ED_OCTET	COMPRESSED_BITMAP_LENGTH;
    u8  		COMPRESSED_BITMAP[16];	
    ED_BOOLEAN 	COMPRESSED_BITMAP_STARTING_COLOR_CODE_Present;
    ED_OCTET	COMPRESSED_BITMAP_STARTING_COLOR_CODE;

    u16 	UNCOMPRESSED_RECEIVED_BLOCK_BITMAP_length;
    u8  	UNCOMPRESSED_RECEIVED_BLOCK_BITMAP[44];	 // that should be enough long
}c_EGPRS_Ack_Nack_Description_IE;
long DECODE_c_EGPRS_Ack_Nack_Description_IE( const char* const Buffer,
                                             const long BitOffset,
                                             c_EGPRS_Ack_Nack_Description_IE* const Destin,
                                             long Length );
#else
#endif
/*-----------------------------------*/
typedef struct _c_PUplinkAckNack {
	ED_BYTE AckNack_Desc [9];
	ED_LONG ContResTLLI;
	ED_BOOLEAN AckNack_Desc_Present;
	ED_OCTET ChanCodCmd;
	ED_BOOLEAN ChanCodCmd_Present;
	ED_BOOLEAN ContResTLLI_Present;
	ED_BOOLEAN Extension_Bits_Present;
	ED_BOOLEAN FixedAlloc_Present;
	ED_OCTET PExtTimingAdv;
	ED_BOOLEAN PExtTimingAdv_Present;
	ED_BOOLEAN PTimingAdv_Present;
	ED_BOOLEAN PwrCtrlParam_Present;
	ED_BOOLEAN TBF_EST;
	ED_BOOLEAN TBF_EST_Present;
	c_PUplinkAckNack_Extension_Bits Extension_Bits;
	c_PUplinkAckNack_FixedAlloc FixedAlloc;
	c_PUplinkAckNack_PTimingAdv PTimingAdv;
	c_PUplinkAckNack_PwrCtrlParam PwrCtrlParam;
// add by dingmx 20061031 for EDGE    
#ifdef __EGPRS__
    ED_OCTET 	EGPRS_Channel_Coding_Command;
    ED_BOOLEAN EGPRS_Channel_Coding_Command_Present;
    ED_OCTET 	RESEGMENT;
    ED_BOOLEAN RESEGMENT_Present;
    
    
    ED_OCTET 		PRE_EMPTIVE_TRANSMISSION;
    ED_BOOLEAN 		PRE_EMPTIVE_TRANSMISSION_Present;
    ED_OCTET 		PRR_RETRANSMISSION_REQUEST;
    ED_BOOLEAN 		PRR_RETRANSMISSION_REQUEST_Present;
    ED_OCTET 		ARAC_RETRANSMISSION_REQUEST;
    ED_BOOLEAN		ARAC_RETRANSMISSION_REQUEST_Present;
    c_EGPRS_Ack_Nack_Description_IE EGPRS_Ack_Nack_Description;
    ED_BOOLEAN		EGPRS_Ack_Nack_Description_Present;

	ED_BOOLEAN		EGprs_Present;
#endif    
}	c_PUplinkAckNack;
#ifdef __EGPRS__

long DECODE_c_PUplinkAckNack( const char* const Buffer, 
                                  const long BitOffset, 
                                  c_PUplinkAckNack* const Destin,
                                  long Length );

// add by lincq begin
void DECODE_c_PUplink_Assignment_msg_ContTlli( const char* const Buffer, 
                                  const long BitOffset, 
                                  c_PUplink_Assignment_ContTlli* const Destin);
#define INIT_c_PUplink_Assignment_ContTlli(sp) ED_RESET_MEM ((sp), sizeof (c_PUplink_Assignment_ContTlli))
#endif
// add by lincq end
#define INIT_c_PUplinkAckNack(sp) ED_RESET_MEM ((sp), sizeof (c_PUplinkAckNack))
#define FREE_c_PUplinkAckNack(sp)
#define SETPRESENT_c_PUplinkAckNack_AckNack_Desc(sp,present) ((sp)->AckNack_Desc_Present = present)
#define GETPRESENT_c_PUplinkAckNack_AckNack_Desc(sp) ((sp)->AckNack_Desc_Present)
#define SETPRESENT_c_PUplinkAckNack_ChanCodCmd(sp,present) ((sp)->ChanCodCmd_Present = present)
#define GETPRESENT_c_PUplinkAckNack_ChanCodCmd(sp) ((sp)->ChanCodCmd_Present)
#define SETPRESENT_c_PUplinkAckNack_ContResTLLI(sp,present) ((sp)->ContResTLLI_Present = present)
#define GETPRESENT_c_PUplinkAckNack_ContResTLLI(sp) ((sp)->ContResTLLI_Present)
#define SETPRESENT_c_PUplinkAckNack_Extension_Bits(sp,present) ((sp)->Extension_Bits_Present = present)
#define GETPRESENT_c_PUplinkAckNack_Extension_Bits(sp) ((sp)->Extension_Bits_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc(sp,present) ((sp)->FixedAlloc_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc(sp) ((sp)->FixedAlloc_Present)
#define SETPRESENT_c_PUplinkAckNack_PExtTimingAdv(sp,present) ((sp)->PExtTimingAdv_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PExtTimingAdv(sp) ((sp)->PExtTimingAdv_Present)
#define SETPRESENT_c_PUplinkAckNack_PTimingAdv(sp,present) ((sp)->PTimingAdv_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PTimingAdv(sp) ((sp)->PTimingAdv_Present)
#define SETPRESENT_c_PUplinkAckNack_PwrCtrlParam(sp,present) ((sp)->PwrCtrlParam_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PwrCtrlParam(sp) ((sp)->PwrCtrlParam_Present)
#define SETPRESENT_c_PUplinkAckNack_TBF_EST(sp,present) ((sp)->TBF_EST_Present = present)
#define GETPRESENT_c_PUplinkAckNack_TBF_EST(sp) ((sp)->TBF_EST_Present)

/* Access member 'AllocBitmap' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_AllocBitmap(var) var

/* Access member 'AllocBitmap' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_AllocBitmap(var) (&var)

/* Access member 'AllocBitmapLen' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_AllocBitmapLen(var) var

/* Access member 'AllocBitmapLen' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_AllocBitmapLen(var) (&var)

/* Access member 'BlocksOrBlockPeriods' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_BlocksOrBlockPeriods(var) var

/* Access member 'BlocksOrBlockPeriods' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_BlocksOrBlockPeriods(var) (&var)

/* Access member 'FinalAlloc' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_FinalAlloc(var) var

/* Access member 'FinalAlloc' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_FinalAlloc(var) (&var)

/* Access member 'Absolute_FN' of type 'c_Fixed_Alloc_Ack_IE_TBF_ST' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_TBF_ST_Absolute_FN(var) var

/* Access member 'Absolute_FN' of type 'c_Fixed_Alloc_Ack_IE_TBF_ST' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_TBF_ST_Absolute_FN(var) (&var)

/* Access member 'Relative_FN' of type 'c_Fixed_Alloc_Ack_IE_TBF_ST' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_TBF_ST_Relative_FN(var) var

/* Access member 'Relative_FN' of type 'c_Fixed_Alloc_Ack_IE_TBF_ST' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_TBF_ST_Relative_FN(var) (&var)

/* Access member 'TBF_ST' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_TBF_ST(var) var

/* Access member 'TBF_ST' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_TBF_ST(var) (&var)

/* Access member 'TSAlloc' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_TSAlloc(var) var

/* Access member 'TSAlloc' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_TSAlloc(var) (&var)

/* Access member 'TSOverride' of type 'c_Fixed_Alloc_Ack_IE' as a variable reference */
#define VAR_c_Fixed_Alloc_Ack_IE_TSOverride(var) var

/* Access member 'TSOverride' of type 'c_Fixed_Alloc_Ack_IE' as a pointer */
#define PTR_c_Fixed_Alloc_Ack_IE_TSOverride(var) (&var)

/* DEFINITION OF BINARY c_Fixed_Alloc_Ack_IE_AllocBitmap */
typedef struct _c_Fixed_Alloc_Ack_IE_AllocBitmap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Fixed_Alloc_Ack_IE_AllocBitmap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Fixed_Alloc_Ack_IE_AllocBitmap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Fixed_Alloc_Ack_IE_TBF_ST */
typedef struct _c_Fixed_Alloc_Ack_IE_TBF_ST {
	ED_SHORT Relative_FN;
	ED_BOOLEAN Absolute_FN_Present;
	ED_BOOLEAN Relative_FN_Present;
	c_TStartingTime Absolute_FN;

} c_Fixed_Alloc_Ack_IE_TBF_ST;


/*-----------------------------------*/
typedef struct _c_Fixed_Alloc_Ack_IE {
	ED_BOOLEAN AllocBitmap_Present;
	ED_OCTET AllocBitmapLen;
	ED_BOOLEAN AllocBitmapLen_Present;
	ED_BOOLEAN BlocksOrBlockPeriods;
	ED_BOOLEAN BlocksOrBlockPeriods_Present;
	ED_BOOLEAN FinalAlloc;
	ED_BOOLEAN TBF_ST_Present;
	ED_OCTET TSAlloc;
	ED_BOOLEAN TSAlloc_Present;
	ED_OCTET TSOverride;
	ED_BOOLEAN TSOverride_Present;
	c_Fixed_Alloc_Ack_IE_AllocBitmap AllocBitmap;
	c_Fixed_Alloc_Ack_IE_TBF_ST TBF_ST;

}	c_Fixed_Alloc_Ack_IE;
#define INIT_c_Fixed_Alloc_Ack_IE(sp) ED_RESET_MEM ((sp), sizeof (c_Fixed_Alloc_Ack_IE))
#define FREE_c_Fixed_Alloc_Ack_IE(sp)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_AllocBitmap(sp,present) ((sp)->AllocBitmap_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_AllocBitmap(sp) ((sp)->AllocBitmap_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_AllocBitmapLen(sp,present) ((sp)->AllocBitmapLen_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_AllocBitmapLen(sp) ((sp)->AllocBitmapLen_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_BlocksOrBlockPeriods(sp,present) ((sp)->BlocksOrBlockPeriods_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_BlocksOrBlockPeriods(sp) ((sp)->BlocksOrBlockPeriods_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST(sp) ((sp)->TBF_ST_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_TSAlloc(sp,present) ((sp)->TSAlloc_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_TSAlloc(sp) ((sp)->TSAlloc_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_TSOverride(sp,present) ((sp)->TSOverride_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_TSOverride(sp) ((sp)->TSOverride_Present)

/* Access member 'AckNack_Desc' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_AckNack_Desc(var) var

/* Access member 'AckNack_Desc' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_AckNack_Desc(var) (&var)

/* Access member 'C_Value' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_C_Value(var) var

/* Access member 'C_Value' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_C_Value(var) (&var)

/* Access member 'ChanReq_Desc' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_ChanReq_Desc(var) var

/* Access member 'ChanReq_Desc' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_ChanReq_Desc(var) (&var)

/* Access member 'DlTFI' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_DlTFI(var) (&var)

/* Access member 'data' of type 'c_PDownlinkAckNack_ILevelTN' as a variable reference */
#define VAR_c_PDownlinkAckNack_ILevelTN_data(var) var

/* Access member 'data' of type 'c_PDownlinkAckNack_ILevelTN' as a pointer */
#define PTR_c_PDownlinkAckNack_ILevelTN_data(var) (&var)

/* Access member 'ILevelTN' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_ILevelTN(var) var

/* Access member 'ILevelTN' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_ILevelTN(var) (&var)

/* Access member 'data' of type 'c_PDownlinkAckNack_ILevelTN_p' as a variable reference */
#define VAR_c_PDownlinkAckNack_ILevelTN_p_data(var) var

/* Access member 'data' of type 'c_PDownlinkAckNack_ILevelTN_p' as a pointer */
#define PTR_c_PDownlinkAckNack_ILevelTN_p_data(var) (&var)

/* Access member 'ILevelTN_p' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_ILevelTN_p(var) var

/* Access member 'ILevelTN_p' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_ILevelTN_p(var) (&var)

/* Access member 'PFI' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_PFI(var) var

/* Access member 'PFI' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_PFI(var) (&var)

/* Access member 'RxQual' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_RxQual(var) var

/* Access member 'RxQual' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_RxQual(var) (&var)

/* Access member 'Sign_Var' of type 'c_PDownlinkAckNack' as a variable reference */
#define VAR_c_PDownlinkAckNack_Sign_Var(var) var

/* Access member 'Sign_Var' of type 'c_PDownlinkAckNack' as a pointer */
#define PTR_c_PDownlinkAckNack_Sign_Var(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PDownlinkAckNack_ILevelTN */
typedef struct _c_PDownlinkAckNack_ILevelTN {
	ED_OCTET data [8];
	int items;
} c_PDownlinkAckNack_ILevelTN;

/* DEFINITION OF SUB-STRUCTURE c_PDownlinkAckNack_ILevelTN_p */
typedef struct _c_PDownlinkAckNack_ILevelTN_p {
	ED_BOOLEAN data [8];
	int items;
} c_PDownlinkAckNack_ILevelTN_p;

//add by lincq begin
#ifdef __EGPRS__
typedef struct _c_PEgprsDownlinkAckNack_EgprsTsQualMeas_BepTn
{
 ED_BOOLEAN Tn_Present;
 ED_BOOLEAN Module_Present; /*FALSE:GPSK; TRUE:8PSK*/
 ED_OCTET   Value;
} c_PEgprsDownlinkAckNack_EgprsTsQualMeas_BepTn;

typedef struct _c_PEgprsDownlinkAckNack_EgprsBepTsQualMeas
{
	c_PEgprsDownlinkAckNack_EgprsTsQualMeas_BepTn BepTn[8];
} c_PEgprsDownlinkAckNack_EgprsBepTsQualMeas_BepMeas;

typedef struct _c_PEgprsDownlinkAckNack_EgprsBepQualMeas
{
    ED_BOOLEAN Gmsk_Present;
    ED_BOOLEAN Psk8_Present;	
    ED_OCTET  GmskMeanBep;   /* GMSK_MEAN_BEP*/ 
    ED_OCTET  GmskCvBep;   /* GMSK_CV_BEP */ 
    ED_OCTET	Psk8MeanBep;	   /* 8PSK _MEAN_BEP*/
    ED_OCTET	Psk8CvBep;	   /* 8PSK _CV_BEP */
} c_PEgprsDownlinkAckNack_EgprsBepQualMeas;

typedef struct _c_PEgprsDownlinkAckNack_Desc {
	ED_OCTET FAI;
	ED_OCTET BOW;
	ED_OCTET EOW;
	ED_BYTE SSN[2];
	ED_OCTET Len_comp;
	ED_OCTET Start_colour;
	ED_BYTE CRBB[20];
	ED_OCTET Len_uncom;
	ED_BYTE URBB[20];
	ED_BOOLEAN FillsRestMsg_Present;
	ED_BOOLEAN CRBB_Present;
	ED_BOOLEAN URBB_Present;
} c_PEgprsDownlinkAckNack_Desc;
#endif
// add by lincq end
typedef struct _c_PDownlinkAckNack {
	ED_BYTE AckNack_Desc [9];
	ED_BYTE ChanReq_Desc [3];
	ED_OCTET C_Value;
	ED_BOOLEAN ChanReq_Desc_Present;
	ED_OCTET DlTFI;
	ED_OCTET PFI;
	ED_BOOLEAN PFI_Present;
	ED_OCTET RxQual;
	ED_OCTET Sign_Var;
	c_PDownlinkAckNack_ILevelTN ILevelTN;
	c_PDownlinkAckNack_ILevelTN_p ILevelTN_p;
// add by lincq begin
#ifdef __EGPRS__	
	ED_OCTET OutOfMem;
	ED_BOOLEAN ILevelTN_Present;
	ED_BOOLEAN EgprsMeas_Present;
	ED_BOOLEAN EgprsBepTsQualMeas_Present;
	c_PEgprsDownlinkAckNack_EgprsBepQualMeas EgprsBepQualMeas;
	c_PEgprsDownlinkAckNack_EgprsBepTsQualMeas_BepMeas EgprsBepTsQualMeas;
	c_PEgprsDownlinkAckNack_Desc EgprsAckDesc;
#endif
// add by lincq end

}	c_PDownlinkAckNack;
#define INIT_c_PDownlinkAckNack(sp) ED_RESET_MEM ((sp), sizeof (c_PDownlinkAckNack))
#define FREE_c_PDownlinkAckNack(sp)
#define SETPRESENT_c_PDownlinkAckNack_ChanReq_Desc(sp,present) ((sp)->ChanReq_Desc_Present = present)
#define GETPRESENT_c_PDownlinkAckNack_ChanReq_Desc(sp) ((sp)->ChanReq_Desc_Present)
#define SETPRESENT_c_PDownlinkAckNack_PFI(sp,present) ((sp)->PFI_Present = present)
#define GETPRESENT_c_PDownlinkAckNack_PFI(sp) ((sp)->PFI_Present)

/* Access member 'DlTFI' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_DlTFI(var) (&var)

/* Access member 'RA' of type 'c_PAccess_Reject_Add_Reject_data_PReqRef' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_PAccess_Reject_Add_Reject_data_PReqRef' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_PAccess_Reject_Add_Reject_data_PReqRef' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_PAccess_Reject_Add_Reject_data_PReqRef' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_PReqRef(var) (&var)

/* Access member 'TLLI' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_TLLI(var) var

/* Access member 'TLLI' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_TLLI(var) (&var)

/* Access member 'UlTFI' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_UlTFI(var) var

/* Access member 'UlTFI' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_UlTFI(var) (&var)

/* Access member 'WAIT_IND' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_WAIT_IND(var) var

/* Access member 'WAIT_IND' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_WAIT_IND(var) (&var)

/* Access member 'WAIT_IND_SIZE' of type 'c_PAccess_Reject_Add_Reject_data' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data_WAIT_IND_SIZE(var) var

/* Access member 'WAIT_IND_SIZE' of type 'c_PAccess_Reject_Add_Reject_data' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data_WAIT_IND_SIZE(var) (&var)

/* Access member 'data' of type 'c_PAccess_Reject_Add_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject_data(var) var

/* Access member 'data' of type 'c_PAccess_Reject_Add_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject_data(var) (&var)

/* Access member 'Add_Reject' of type 'c_PAccess_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Add_Reject(var) var

/* Access member 'Add_Reject' of type 'c_PAccess_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Add_Reject(var) (&var)

/* Access member 'DlTFI' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_DlTFI(var) (&var)

/* Access member 'RA' of type 'c_PAccess_Reject_Reject_PReqRef' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_PAccess_Reject_Reject_PReqRef' as a pointer */
#define PTR_c_PAccess_Reject_Reject_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_PAccess_Reject_Reject_PReqRef' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_PAccess_Reject_Reject_PReqRef' as a pointer */
#define PTR_c_PAccess_Reject_Reject_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_PReqRef(var) (&var)

/* Access member 'TLLI' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_TLLI(var) var

/* Access member 'TLLI' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_TLLI(var) (&var)

/* Access member 'UlTFI' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_UlTFI(var) var

/* Access member 'UlTFI' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_UlTFI(var) (&var)

/* Access member 'WAIT_IND' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_WAIT_IND(var) var

/* Access member 'WAIT_IND' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_WAIT_IND(var) (&var)

/* Access member 'WAIT_IND_SIZE' of type 'c_PAccess_Reject_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject_WAIT_IND_SIZE(var) var

/* Access member 'WAIT_IND_SIZE' of type 'c_PAccess_Reject_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject_WAIT_IND_SIZE(var) (&var)

/* Access member 'Reject' of type 'c_PAccess_Reject' as a variable reference */
#define VAR_c_PAccess_Reject_Reject(var) var

/* Access member 'Reject' of type 'c_PAccess_Reject' as a pointer */
#define PTR_c_PAccess_Reject_Reject(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_PAccess_Reject_Add_Reject_data_PReqRef */
typedef struct _c_PAccess_Reject_Add_Reject_data_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_PAccess_Reject_Add_Reject_data_PReqRef;

/* DEFINITION OF SUB-STRUCTURE c_PAccess_Reject_Add_Reject_data */
typedef struct _c_PAccess_Reject_Add_Reject_data {
	ED_LONG TLLI;
	ED_OCTET DlTFI;
	ED_BOOLEAN DlTFI_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_BOOLEAN TLLI_Present;
	ED_OCTET UlTFI;
	ED_BOOLEAN UlTFI_Present;
	ED_OCTET WAIT_IND;
	ED_BOOLEAN WAIT_IND_Present;
	ED_BOOLEAN WAIT_IND_SIZE;
	ED_BOOLEAN WAIT_IND_SIZE_Present;
	c_PAccess_Reject_Add_Reject_data_PReqRef PReqRef;

} c_PAccess_Reject_Add_Reject_data;

/* DEFINITION OF SUB-STRUCTURE c_PAccess_Reject_Add_Reject */
typedef struct _c_PAccess_Reject_Add_Reject {
	c_PAccess_Reject_Add_Reject_data data [20];
	int items;
} c_PAccess_Reject_Add_Reject;

/* DEFINITION OF SUB-STRUCTURE c_PAccess_Reject_Reject_PReqRef */
typedef struct _c_PAccess_Reject_Reject_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_PAccess_Reject_Reject_PReqRef;

/* DEFINITION OF SUB-STRUCTURE c_PAccess_Reject_Reject */
typedef struct _c_PAccess_Reject_Reject {
	ED_LONG TLLI;
	ED_OCTET DlTFI;
	ED_BOOLEAN DlTFI_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_BOOLEAN TLLI_Present;
	ED_OCTET UlTFI;
	ED_BOOLEAN UlTFI_Present;
	ED_OCTET WAIT_IND;
	ED_BOOLEAN WAIT_IND_Present;
	ED_BOOLEAN WAIT_IND_SIZE;
	ED_BOOLEAN WAIT_IND_SIZE_Present;
	c_PAccess_Reject_Reject_PReqRef PReqRef;

} c_PAccess_Reject_Reject;


/*-----------------------------------*/
typedef struct _c_PAccess_Reject {
	c_PAccess_Reject_Add_Reject Add_Reject;
	c_PAccess_Reject_Reject Reject;

}	c_PAccess_Reject;
#define INIT_c_PAccess_Reject(sp) ED_RESET_MEM ((sp), sizeof (c_PAccess_Reject))
#define FREE_c_PAccess_Reject(sp)

/* Access member 'DlTFI' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_DlTFI(var) var

/* Access member 'DlTFI' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_DlTFI(var) (&var)

/* Access member 'RA' of type 'c_Reject_struct_PReqRef' as a variable reference */
#define VAR_c_Reject_struct_PReqRef_RA(var) var

/* Access member 'RA' of type 'c_Reject_struct_PReqRef' as a pointer */
#define PTR_c_Reject_struct_PReqRef_RA(var) (&var)

/* Access member 'ST' of type 'c_Reject_struct_PReqRef' as a variable reference */
#define VAR_c_Reject_struct_PReqRef_ST(var) var

/* Access member 'ST' of type 'c_Reject_struct_PReqRef' as a pointer */
#define PTR_c_Reject_struct_PReqRef_ST(var) (&var)

/* Access member 'PReqRef' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_PReqRef(var) var

/* Access member 'PReqRef' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_PReqRef(var) (&var)

/* Access member 'TLLI' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_TLLI(var) var

/* Access member 'TLLI' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_TLLI(var) (&var)

/* Access member 'UlTFI' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_UlTFI(var) var

/* Access member 'UlTFI' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_UlTFI(var) (&var)

/* Access member 'WAIT_IND' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_WAIT_IND(var) var

/* Access member 'WAIT_IND' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_WAIT_IND(var) (&var)

/* Access member 'WAIT_IND_SIZE' of type 'c_Reject_struct' as a variable reference */
#define VAR_c_Reject_struct_WAIT_IND_SIZE(var) var

/* Access member 'WAIT_IND_SIZE' of type 'c_Reject_struct' as a pointer */
#define PTR_c_Reject_struct_WAIT_IND_SIZE(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Reject_struct_PReqRef */
typedef struct _c_Reject_struct_PReqRef {
	ED_SHORT RA;
	c_TStartingTime ST;

} c_Reject_struct_PReqRef;


/*-----------------------------------*/
typedef struct _c_Reject_struct {
	ED_LONG TLLI;
	ED_OCTET DlTFI;
	ED_BOOLEAN DlTFI_Present;
	ED_BOOLEAN PReqRef_Present;
	ED_BOOLEAN TLLI_Present;
	ED_OCTET UlTFI;
	ED_BOOLEAN UlTFI_Present;
	ED_OCTET WAIT_IND;
	ED_BOOLEAN WAIT_IND_Present;
	ED_BOOLEAN WAIT_IND_SIZE;
	ED_BOOLEAN WAIT_IND_SIZE_Present;
	c_Reject_struct_PReqRef PReqRef;

}	c_Reject_struct;
#define INIT_c_Reject_struct(sp) ED_RESET_MEM ((sp), sizeof (c_Reject_struct))
#define FREE_c_Reject_struct(sp)
#define SETPRESENT_c_Reject_struct_DlTFI(sp,present) ((sp)->DlTFI_Present = present)
#define GETPRESENT_c_Reject_struct_DlTFI(sp) ((sp)->DlTFI_Present)
#define SETPRESENT_c_Reject_struct_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_Reject_struct_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_Reject_struct_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_Reject_struct_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_Reject_struct_UlTFI(sp,present) ((sp)->UlTFI_Present = present)
#define GETPRESENT_c_Reject_struct_UlTFI(sp) ((sp)->UlTFI_Present)
#define SETPRESENT_c_Reject_struct_WAIT_IND(sp,present) ((sp)->WAIT_IND_Present = present)
#define GETPRESENT_c_Reject_struct_WAIT_IND(sp) ((sp)->WAIT_IND_Present)
#define SETPRESENT_c_Reject_struct_WAIT_IND_SIZE(sp,present) ((sp)->WAIT_IND_SIZE_Present = present)
#define GETPRESENT_c_Reject_struct_WAIT_IND_SIZE(sp) ((sp)->WAIT_IND_SIZE_Present)

/* SETITEMS/ADDITEMS commands for type 'c_FrequencyList_Range_Wi'. */
void SETITEMS_c_FrequencyList_Range_Wi (c_FrequencyList_Range_Wi* sequence, int desiredItems);
#define ADDITEMS_c_FrequencyList_Range_Wi(sequence, itemsToBeAdded) SETITEMS_c_FrequencyList_Range_Wi (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_GAMMA(sp,present) ((sp)->GAMMA_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_GAMMA(sp) ((sp)->GAMMA_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_POLLING(sp,present) ((sp)->POLLING_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_POLLING(sp) ((sp)->POLLING_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_RLC_MODE(sp,present) ((sp)->RLC_MODE_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_RLC_MODE(sp) ((sp)->RLC_MODE_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_TA_VALID(sp,present) ((sp)->TA_VALID_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_TA_VALID(sp) ((sp)->TA_VALID_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_TBF_STARTING_TIME(sp,present) ((sp)->TBF_STARTING_TIME_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_TBF_STARTING_TIME(sp) ((sp)->TBF_STARTING_TIME_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_TFI_ASSIGNMENT(sp,present) ((sp)->TFI_ASSIGNMENT_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_TFI_ASSIGNMENT(sp) ((sp)->TFI_ASSIGNMENT_Present)
#define SETPRESENT_c_IA_Rest_Octets_PDlAssgt_TIMING_ADVANCE_INDEX(sp,present) ((sp)->TIMING_ADVANCE_INDEX_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PDlAssgt_TIMING_ADVANCE_INDEX(sp) ((sp)->TIMING_ADVANCE_INDEX_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP(sp,present) ((sp)->ALLOCATION_BITMAP_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP(sp) ((sp)->ALLOCATION_BITMAP_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP_LENGTH(sp,present) ((sp)->ALLOCATION_BITMAP_LENGTH_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALLOCATION_BITMAP_LENGTH(sp) ((sp)->ALLOCATION_BITMAP_LENGTH_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_CHANNEL_CODING_COMMAND(sp,present) ((sp)->CHANNEL_CODING_COMMAND_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_CHANNEL_CODING_COMMAND(sp) ((sp)->CHANNEL_CODING_COMMAND_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_Extended_RA(sp,present) ((sp)->Extended_RA_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_Extended_RA(sp) ((sp)->Extended_RA_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_POLLING(sp,present) ((sp)->POLLING_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_POLLING(sp) ((sp)->POLLING_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_TBF_STARTING_TIME(sp,present) ((sp)->TBF_STARTING_TIME_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_TBF_STARTING_TIME(sp) ((sp)->TBF_STARTING_TIME_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_TFI_ASSIGNMENT(sp,present) ((sp)->TFI_ASSIGNMENT_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_TFI_ASSIGNMENT(sp) ((sp)->TFI_ASSIGNMENT_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_TIMING_ADVANCE_INDEX(sp,present) ((sp)->TIMING_ADVANCE_INDEX_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_TIMING_ADVANCE_INDEX(sp) ((sp)->TIMING_ADVANCE_INDEX_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_TLLI_BLOCK_CHANNEL_CODING(sp,present) ((sp)->TLLI_BLOCK_CHANNEL_CODING_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_TLLI_BLOCK_CHANNEL_CODING(sp) ((sp)->TLLI_BLOCK_CHANNEL_CODING_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_USF(sp,present) ((sp)->USF_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_USF(sp) ((sp)->USF_Present)
#define SETPRESENT_c_IA_Rest_Octets_PUlAssgt_USF_GRANULARITY(sp,present) ((sp)->USF_GRANULARITY_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_PUlAssgt_USF_GRANULARITY(sp) ((sp)->USF_GRANULARITY_Present)
#define SETPRESENT_c_IA_Rest_Octets_ScndPart_Extended_RA(sp,present) ((sp)->Extended_RA_Present = present)
#define GETPRESENT_c_IA_Rest_Octets_ScndPart_Extended_RA(sp) ((sp)->Extended_RA_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_ARFCN(sp) ((sp)->ARFCN_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1(sp,present) ((sp)->Dir_enc_1_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1(sp) ((sp)->Dir_enc_1_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2(sp,present) ((sp)->Dir_enc_2_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_2(sp) ((sp)->Dir_enc_2_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc(sp,present) ((sp)->Indir_enc_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc(sp) ((sp)->Indir_enc_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp,present) ((sp)->ArfcnIdxList_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp) ((sp)->ArfcnIdxList_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,present) ((sp)->MaBitMap_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp) ((sp)->MaBitMap_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp,present) ((sp)->MaLen_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp) ((sp)->MaLen_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp,present) ((sp)->RflNbList_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp) ((sp)->RflNbList_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx'. */
void SETITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx* sequence, int desiredItems);
#define ADDITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(sequence, itemsToBeAdded) SETITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb'. */
void SETITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb* sequence, int desiredItems);
#define ADDITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(sequence, itemsToBeAdded) SETITEMS_c_PDownlink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp,present) ((sp)->CHANGE_MARK_1_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp) ((sp)->CHANGE_MARK_1_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp,present) ((sp)->CHANGE_MARK_2_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp) ((sp)->CHANGE_MARK_2_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_INDEX(sp,present) ((sp)->TA_INDEX_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_INDEX(sp) ((sp)->TA_INDEX_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_TS_NB(sp,present) ((sp)->TA_TS_NB_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_TS_NB(sp) ((sp)->TA_TS_NB_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_PTiming_Advance_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA'. */
void SETITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA (c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p'. */
void SETITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p (c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_PDownlink_Assignment_Msg_Pwr_Ctrl_Param_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PDownlink_Assignment_Msg_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Pwr_Ctrl_Param_IE_GAMMA'. */
void SETITEMS_c_Pwr_Ctrl_Param_IE_GAMMA (c_Pwr_Ctrl_Param_IE_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_Pwr_Ctrl_Param_IE_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_Pwr_Ctrl_Param_IE_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Pwr_Ctrl_Param_IE_GAMMA_p'. */
void SETITEMS_c_Pwr_Ctrl_Param_IE_GAMMA_p (c_Pwr_Ctrl_Param_IE_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_Pwr_Ctrl_Param_IE_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_Pwr_Ctrl_Param_IE_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_INDEX(sp,present) ((sp)->DL_TA_INDEX_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_INDEX(sp) ((sp)->DL_TA_INDEX_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_TS_NB(sp,present) ((sp)->DL_TA_TS_NB_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_DL_TA_TS_NB(sp) ((sp)->DL_TA_TS_NB_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_INDEX(sp,present) ((sp)->UL_TA_INDEX_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_INDEX(sp) ((sp)->UL_TA_INDEX_Present)
#define SETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_TS_NB(sp,present) ((sp)->UL_TA_TS_NB_Present = present)
#define GETPRESENT_c_PPwrCtrl_TA_Msg_GPTiming_Advance_UL_TA_TS_NB(sp) ((sp)->UL_TA_TS_NB_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA'. */
void SETITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA (c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p'. */
void SETITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p (c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_PPwrCtrl_TA_Msg_Pwr_Ctrl_Param_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(sp,present) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(sp) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST(sp) ((sp)->TBF_ST_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA'. */
void SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA (c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF'. */
void SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF (c_PUplink_Assignment_Msg_Dynamic_Alloc_USF* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p'. */
void SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p (c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Dynamic_Alloc_USF_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap(sp,present) ((sp)->AllocBitmap_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmap(sp) ((sp)->AllocBitmap_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmapLen(sp,present) ((sp)->AllocBitmapLen_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_AllocBitmapLen(sp) ((sp)->AllocBitmapLen_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_BlocksOrBlockPeriods(sp,present) ((sp)->BlocksOrBlockPeriods_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_BlocksOrBlockPeriods(sp) ((sp)->BlocksOrBlockPeriods_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Meas_Mapping(sp,present) ((sp)->Meas_Mapping_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Meas_Mapping(sp) ((sp)->Meas_Mapping_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape(sp,present) ((sp)->Message_escape_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Message_escape(sp) ((sp)->Message_escape_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param(sp,present) ((sp)->Pwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param(sp) ((sp)->Pwr_Ctrl_Param_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TS_ALLOC(sp,present) ((sp)->UL_TS_ALLOC_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_UL_TS_ALLOC(sp) ((sp)->UL_TS_ALLOC_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA'. */
void SETITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA (c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p'. */
void SETITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p (c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Fixed_Alloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_ARFCN(sp) ((sp)->ARFCN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1(sp,present) ((sp)->Dir_enc_1_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1(sp) ((sp)->Dir_enc_1_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2(sp,present) ((sp)->Dir_enc_2_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_2(sp) ((sp)->Dir_enc_2_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc(sp,present) ((sp)->Indir_enc_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc(sp) ((sp)->Indir_enc_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp,present) ((sp)->ArfcnIdxList_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp) ((sp)->ArfcnIdxList_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,present) ((sp)->MaBitMap_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp) ((sp)->MaBitMap_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp,present) ((sp)->MaLen_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp) ((sp)->MaLen_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp,present) ((sp)->RflNbList_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp) ((sp)->RflNbList_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx'. */
void SETITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb'. */
void SETITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb* sequence, int desiredItems);
#define ADDITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(sequence, itemsToBeAdded) SETITEMS_c_PUplink_Assignment_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp,present) ((sp)->CHANGE_MARK_1_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp) ((sp)->CHANGE_MARK_1_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp,present) ((sp)->CHANGE_MARK_2_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp) ((sp)->CHANGE_MARK_2_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_INDEX(sp,present) ((sp)->TA_INDEX_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_INDEX(sp) ((sp)->TA_INDEX_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_TS_NB(sp,present) ((sp)->TA_TS_NB_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_TS_NB(sp) ((sp)->TA_TS_NB_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_PTiming_Advance_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_GAMMA(sp,present) ((sp)->GAMMA_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_GAMMA(sp) ((sp)->GAMMA_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PUplink_Assignment_Msg_Single_Block_Alloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Dynamic_Alloc_struct_GAMMA'. */
void SETITEMS_c_Dynamic_Alloc_struct_GAMMA (c_Dynamic_Alloc_struct_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_Dynamic_Alloc_struct_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_Dynamic_Alloc_struct_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Dynamic_Alloc_struct_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_Dynamic_Alloc_struct_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_Dynamic_Alloc_struct_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Dynamic_Alloc_struct_USF'. */
void SETITEMS_c_Dynamic_Alloc_struct_USF (c_Dynamic_Alloc_struct_USF* sequence, int desiredItems);
#define ADDITEMS_c_Dynamic_Alloc_struct_USF(sequence, itemsToBeAdded) SETITEMS_c_Dynamic_Alloc_struct_USF (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Dynamic_Alloc_struct_USF_p'. */
void SETITEMS_c_Dynamic_Alloc_struct_USF_p (c_Dynamic_Alloc_struct_USF_p* sequence, int desiredItems);
#define ADDITEMS_c_Dynamic_Alloc_struct_USF_p(sequence, itemsToBeAdded) SETITEMS_c_Dynamic_Alloc_struct_USF_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Single_Block_Alloc_struct_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_Single_Block_Alloc_struct_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_Single_Block_Alloc_struct_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp,present) ((sp)->ArfcnIdxList_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp) ((sp)->ArfcnIdxList_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap(sp,present) ((sp)->MaBitMap_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaBitMap(sp) ((sp)->MaBitMap_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaLen(sp,present) ((sp)->MaLen_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_MaLen(sp) ((sp)->MaLen_Present)
#define SETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList(sp,present) ((sp)->RflNbList_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList(sp) ((sp)->RflNbList_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx'. */
void SETITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx* sequence, int desiredItems);
#define ADDITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(sequence, itemsToBeAdded) SETITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb'. */
void SETITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb (c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb* sequence, int desiredItems);
#define ADDITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb(sequence, itemsToBeAdded) SETITEMS_c_Freq_Param_IE_Dir_enc_1_GPRS_MA_RflNbList_RflNb (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_1(sp,present) ((sp)->CHANGE_MARK_1_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_1(sp) ((sp)->CHANGE_MARK_1_Present)
#define SETPRESENT_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_2(sp,present) ((sp)->CHANGE_MARK_2_Present = present)
#define GETPRESENT_c_Freq_Param_IE_Indir_enc_CHANGE_MARK_2(sp) ((sp)->CHANGE_MARK_2_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA'. */
void SETITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA (c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p'. */
void SETITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p (c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_Fixed_Alloc_struct_Pwr_Ctrl_Param_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Fixed_Alloc_struct_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_Fixed_Alloc_struct_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_Fixed_Alloc_struct_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_ALPHA(sp,present) ((sp)->ALPHA_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_ALPHA(sp) ((sp)->ALPHA_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(sp,present) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_RLC_DATA_BLOCKS_GRANTED(sp) ((sp)->RLC_DATA_BLOCKS_GRANTED_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST(sp) ((sp)->TBF_ST_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA (c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF (c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p (c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc_USF_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap(sp,present) ((sp)->AllocBitmap_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmap(sp) ((sp)->AllocBitmap_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmapLen(sp,present) ((sp)->AllocBitmapLen_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_AllocBitmapLen(sp) ((sp)->AllocBitmapLen_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(sp,present) ((sp)->BTS_PWR_CTRL_MODE_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BTS_PWR_CTRL_MODE(sp) ((sp)->BTS_PWR_CTRL_MODE_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BlocksOrBlockPeriods(sp,present) ((sp)->BlocksOrBlockPeriods_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_BlocksOrBlockPeriods(sp) ((sp)->BlocksOrBlockPeriods_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping(sp,present) ((sp)->Meas_Mapping_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Meas_Mapping(sp) ((sp)->Meas_Mapping_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape(sp,present) ((sp)->Message_escape_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Message_escape(sp) ((sp)->Message_escape_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_P0(sp,present) ((sp)->P0_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_P0(sp) ((sp)->P0_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_PR_MODE(sp,present) ((sp)->PR_MODE_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_PR_MODE(sp) ((sp)->PR_MODE_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param(sp,present) ((sp)->Pwr_Ctrl_Param_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param(sp) ((sp)->Pwr_Ctrl_Param_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TFI_ASSIGN(sp,present) ((sp)->UL_TFI_ASSIGN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TFI_ASSIGN(sp) ((sp)->UL_TFI_ASSIGN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TS_ALLOC(sp,present) ((sp)->UL_TS_ALLOC_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_UL_TS_ALLOC(sp) ((sp)->UL_TS_ALLOC_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA (c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p (c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_Pwr_Ctrl_Param_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Fixed_Alloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_ARFCN(sp,present) ((sp)->ARFCN_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_ARFCN(sp) ((sp)->ARFCN_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1(sp,present) ((sp)->Dir_enc_1_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1(sp) ((sp)->Dir_enc_1_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2(sp,present) ((sp)->Dir_enc_2_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_2(sp) ((sp)->Dir_enc_2_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc(sp,present) ((sp)->Indir_enc_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc(sp) ((sp)->Indir_enc_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp,present) ((sp)->ArfcnIdxList_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList(sp) ((sp)->ArfcnIdxList_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp,present) ((sp)->MaBitMap_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaBitMap(sp) ((sp)->MaBitMap_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp,present) ((sp)->MaLen_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_MaLen(sp) ((sp)->MaLen_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp,present) ((sp)->RflNbList_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList(sp) ((sp)->RflNbList_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_ArfcnIdxList_ArfcnIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb'. */
void SETITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb* sequence, int desiredItems);
#define ADDITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb(sequence, itemsToBeAdded) SETITEMS_c_PTimeslot_Reconfigure_Msg_Freq_Param_Dir_enc_1_GPRS_MA_RflNbList_RflNb (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp,present) ((sp)->CHANGE_MARK_1_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_1(sp) ((sp)->CHANGE_MARK_1_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp,present) ((sp)->CHANGE_MARK_2_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_Freq_Param_Indir_enc_CHANGE_MARK_2(sp) ((sp)->CHANGE_MARK_2_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_INDEX(sp,present) ((sp)->DL_TA_INDEX_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_INDEX(sp) ((sp)->DL_TA_INDEX_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_TS_NB(sp,present) ((sp)->DL_TA_TS_NB_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_DL_TA_TS_NB(sp) ((sp)->DL_TA_TS_NB_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_INDEX(sp,present) ((sp)->UL_TA_INDEX_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_INDEX(sp) ((sp)->UL_TA_INDEX_Present)
#define SETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_TS_NB(sp,present) ((sp)->UL_TA_TS_NB_Present = present)
#define GETPRESENT_c_PTimeslot_Reconfigure_Msg_GPTiming_Advance_UL_TA_TS_NB(sp) ((sp)->UL_TA_TS_NB_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MacCtrlHder_PAccRej_Add_Reject'. */
void SETITEMS_c_MacCtrlHder_PAccRej_Add_Reject (c_MacCtrlHder_PAccRej_Add_Reject* sequence, int desiredItems);
#define ADDITEMS_c_MacCtrlHder_PAccRej_Add_Reject(sequence, itemsToBeAdded) SETITEMS_c_MacCtrlHder_PAccRej_Add_Reject (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_DlTFI(sp,present) ((sp)->DlTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_DlTFI(sp) ((sp)->DlTFI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_UlTFI(sp,present) ((sp)->UlTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_UlTFI(sp) ((sp)->UlTFI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND(sp,present) ((sp)->WAIT_IND_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND(sp) ((sp)->WAIT_IND_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND_SIZE(sp,present) ((sp)->WAIT_IND_SIZE_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Add_Reject_data_WAIT_IND_SIZE(sp) ((sp)->WAIT_IND_SIZE_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_DlTFI(sp,present) ((sp)->DlTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_DlTFI(sp) ((sp)->DlTFI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_UlTFI(sp,present) ((sp)->UlTFI_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_UlTFI(sp) ((sp)->UlTFI_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_WAIT_IND(sp,present) ((sp)->WAIT_IND_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_WAIT_IND(sp) ((sp)->WAIT_IND_Present)
#define SETPRESENT_c_MacCtrlHder_PAccRej_Reject_WAIT_IND_SIZE(sp,present) ((sp)->WAIT_IND_SIZE_Present = present)
#define GETPRESENT_c_MacCtrlHder_PAccRej_Reject_WAIT_IND_SIZE(sp) ((sp)->WAIT_IND_SIZE_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MacCtrlHder_PersistenceLevel'. */
void SETITEMS_c_MacCtrlHder_PersistenceLevel (c_MacCtrlHder_PersistenceLevel* sequence, int desiredItems);
#define ADDITEMS_c_MacCtrlHder_PersistenceLevel(sequence, itemsToBeAdded) SETITEMS_c_MacCtrlHder_PersistenceLevel (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MacCtrlHder_TQIInd'. */
void SETITEMS_c_MacCtrlHder_TQIInd (c_MacCtrlHder_TQIInd* sequence, int desiredItems);
#define ADDITEMS_c_MacCtrlHder_TQIInd(sequence, itemsToBeAdded) SETITEMS_c_MacCtrlHder_TQIInd (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_AllocBitmap(sp,present) ((sp)->AllocBitmap_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_AllocBitmap(sp) ((sp)->AllocBitmap_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_AllocBitmapLen(sp,present) ((sp)->AllocBitmapLen_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_AllocBitmapLen(sp) ((sp)->AllocBitmapLen_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_BlocksOrBlockPeriods(sp,present) ((sp)->BlocksOrBlockPeriods_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_BlocksOrBlockPeriods(sp) ((sp)->BlocksOrBlockPeriods_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST(sp,present) ((sp)->TBF_ST_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST(sp) ((sp)->TBF_ST_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_TSAlloc(sp,present) ((sp)->TSAlloc_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_TSAlloc(sp) ((sp)->TSAlloc_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_TSOverride(sp,present) ((sp)->TSOverride_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_TSOverride(sp) ((sp)->TSOverride_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_PUplinkAckNack_FixedAlloc_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
#define SETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_INDEX(sp,present) ((sp)->TA_INDEX_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_INDEX(sp) ((sp)->TA_INDEX_Present)
#define SETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_TS_NB(sp,present) ((sp)->TA_TS_NB_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_TS_NB(sp) ((sp)->TA_TS_NB_Present)
#define SETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_VALUE(sp,present) ((sp)->TA_VALUE_Present = present)
#define GETPRESENT_c_PUplinkAckNack_PTimingAdv_TA_VALUE(sp) ((sp)->TA_VALUE_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA'. */
void SETITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA (c_PUplinkAckNack_PwrCtrlParam_GAMMA* sequence, int desiredItems);
#define ADDITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA(sequence, itemsToBeAdded) SETITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PUplinkAckNack_PwrCtrlParam_GAMMA_p'. */
void SETITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p (c_PUplinkAckNack_PwrCtrlParam_GAMMA_p* sequence, int desiredItems);
#define ADDITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p(sequence, itemsToBeAdded) SETITEMS_c_PUplinkAckNack_PwrCtrlParam_GAMMA_p (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST_Absolute_FN(sp,present) ((sp)->Absolute_FN_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST_Absolute_FN(sp) ((sp)->Absolute_FN_Present)
#define SETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST_Relative_FN(sp,present) ((sp)->Relative_FN_Present = present)
#define GETPRESENT_c_Fixed_Alloc_Ack_IE_TBF_ST_Relative_FN(sp) ((sp)->Relative_FN_Present)
/* SETITEMS/ADDITEMS commands for type 'c_PDownlinkAckNack_ILevelTN'. */
void SETITEMS_c_PDownlinkAckNack_ILevelTN (c_PDownlinkAckNack_ILevelTN* sequence, int desiredItems);
#define ADDITEMS_c_PDownlinkAckNack_ILevelTN(sequence, itemsToBeAdded) SETITEMS_c_PDownlinkAckNack_ILevelTN (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PDownlinkAckNack_ILevelTN_p'. */
void SETITEMS_c_PDownlinkAckNack_ILevelTN_p (c_PDownlinkAckNack_ILevelTN_p* sequence, int desiredItems);
#define ADDITEMS_c_PDownlinkAckNack_ILevelTN_p(sequence, itemsToBeAdded) SETITEMS_c_PDownlinkAckNack_ILevelTN_p (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_PAccess_Reject_Add_Reject'. */
void SETITEMS_c_PAccess_Reject_Add_Reject (c_PAccess_Reject_Add_Reject* sequence, int desiredItems);
#define ADDITEMS_c_PAccess_Reject_Add_Reject(sequence, itemsToBeAdded) SETITEMS_c_PAccess_Reject_Add_Reject (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_DlTFI(sp,present) ((sp)->DlTFI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_DlTFI(sp) ((sp)->DlTFI_Present)
#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_UlTFI(sp,present) ((sp)->UlTFI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_UlTFI(sp) ((sp)->UlTFI_Present)
#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_WAIT_IND(sp,present) ((sp)->WAIT_IND_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_WAIT_IND(sp) ((sp)->WAIT_IND_Present)
#define SETPRESENT_c_PAccess_Reject_Add_Reject_data_WAIT_IND_SIZE(sp,present) ((sp)->WAIT_IND_SIZE_Present = present)
#define GETPRESENT_c_PAccess_Reject_Add_Reject_data_WAIT_IND_SIZE(sp) ((sp)->WAIT_IND_SIZE_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_DlTFI(sp,present) ((sp)->DlTFI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_DlTFI(sp) ((sp)->DlTFI_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_PReqRef(sp,present) ((sp)->PReqRef_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_PReqRef(sp) ((sp)->PReqRef_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_TLLI(sp,present) ((sp)->TLLI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_TLLI(sp) ((sp)->TLLI_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_UlTFI(sp,present) ((sp)->UlTFI_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_UlTFI(sp) ((sp)->UlTFI_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_WAIT_IND(sp,present) ((sp)->WAIT_IND_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_WAIT_IND(sp) ((sp)->WAIT_IND_Present)
#define SETPRESENT_c_PAccess_Reject_Reject_WAIT_IND_SIZE(sp,present) ((sp)->WAIT_IND_SIZE_Present = present)
#define GETPRESENT_c_PAccess_Reject_Reject_WAIT_IND_SIZE(sp) ((sp)->WAIT_IND_SIZE_Present)
#ifdef __EGPRS__
//	add by dingmx begin 20061031
////////////////////////////////////////////////////////////////////////////////
// move here for we want used c_Freq_Param_IE Freq_Param


typedef struct _c_PTimeslot_Reconfigure_Msg {
	ED_OCTET CHANNEL_CODING_COMMAND;
	ED_BOOLEAN CHANNEL_CODING_COMMAND_Present;
	ED_BOOLEAN CONTROL_ACK;
	ED_BOOLEAN CONTROL_ACK_Present;
	ED_BOOLEAN DL_RLC_MODE;
	ED_BOOLEAN DL_RLC_MODE_Present;
	ED_OCTET DL_TFI_ASSIGN;
	ED_BOOLEAN DL_TFI_ASSIGN_Present;
	ED_OCTET DL_TS_ALLOC;
	ED_BOOLEAN DL_TS_ALLOC_Present;
	ED_BOOLEAN Dynamic_Alloc_Present;
	ED_BOOLEAN Fixed_Alloc_Present;
	ED_BOOLEAN Freq_Param_Present;
	ED_BOOLEAN GPTiming_Advance_Present;
	ED_OCTET PExtended_Timing_Advance;
	ED_BOOLEAN PExtended_Timing_Advance_Present;
	ED_OCTET UL_TFI_ASSIGN;
	ED_BOOLEAN UL_TFI_ASSIGN_Present;
	c_PTimeslot_Reconfigure_Msg_Dynamic_Alloc Dynamic_Alloc;
	c_PTimeslot_Reconfigure_Msg_Fixed_Alloc Fixed_Alloc;

    //c_PTimeslot_Reconfigure_Msg_Freq_Param Freq_Param; // shared IE
    c_Freq_Param_IE Freq_Param; 
    //c_PTimeslot_Reconfigure_Msg_GPTiming_Advance GPTiming_Advance;
    c_GPTiming_Advance_IE GPTiming_Advance;
	// add by dingmx
    ED_BOOLEAN Global_TFI_IE_Present;
    c_Global_TFI  Global_TFI;    
// add by dingmx 20061031 for EDGE	
    ED_BOOLEAN COMPACT_reduced_MA_Present;
    c_COMPACT_reduced_MA  COMPACT_reduced_MA;
    
	ED_OCTET EGPRS_Channel_Coding_Command;
	ED_BOOLEAN EGPRS_Channel_Coding_Command_Present;
	
    ED_OCTET RESEGMENT;
    
    ED_BOOLEAN  DL_EGPRS_Window_Size_Present;
    ED_OCTET	DL_EGPRS_Window_Size;
    
    ED_BOOLEAN  UL_EGPRS_Window_Size_Present;
    ED_OCTET	UL_EGPRS_Window_Size;
    
    ED_BOOLEAN  LINK_Quality_Meas_Mode_Present;
    ED_OCTET    LINK_Quality_Meas_Mode;

	ED_BOOLEAN	 Egprs_Present;
}	c_PTimeslot_Reconfigure_Msg;
//		{ 00 {	{ 0 | 1 < COMPACT reduced MA : < COMPACT reduced MA IE >> }
//		< EGPRS Channel Coding Command : < EGPRS Modulation and Coding IE >>
//		< RESEGMENT : bit (1) >
//		{ 0 | 1 < DOWNLINK EGPRS Window Size : < EGPRS Window Size IE > > }
//		{ 0 | 1 < UPLINK EGPRS Window Size : < EGPRS Window Size IE > > }
//		< LINK_QUALITY_MEASUREMENT_MODE : bit (2) >
//		< Global Packet Timing Advance : < Global Packet Timing Advance IE > >
//		{ 0 | 1 <Packet Extended Timing Advance : bit (2) > }
//		< DOWNLINK_RLC_MODE : bit (1) >
//		< CONTROL_ACK : bit (1) >
//		{ 0 | 1 < DOWNLINK_TFI_ASSIGNMENT : bit (5) > }
//		{ 0 | 1 < UPLINK_TFI_ASSIGNMENT : bit (5) > }
//		< DOWNLINK_TIMESLOT_ALLOCATION : bit (8) >
//		{ 0 | 1	< Frequency Parameters : < Frequency Parameters IE > > }
//		 { 0	< Dynamic Allocation : < Dynamic Allocation struct > >
//		 | 1	< Fixed allocation : < Fixed Allocation struct > >}
//		< padding bits >
//		! < Non-distribution part error : bit (*) = < no string > > }
// ! < Message escape : { 01| 10 | 11} bit (*) = <no string> > }} -Extended for future changes
	

long DECODE_c_PTimeslot_Reconfigure_Msg(const char* const Buffer, 
                                         const long BitOffset,
                                         c_PTimeslot_Reconfigure_Msg* const Destin,
                                         long Length);
//=============================================================================
typedef struct _c_PUplink_Assignment_Msg_Multi_Block_Allocation
{
    //< Multi Block Allocation struct > ::=
    //	< TIMESLOT_NUMBER : bit (3) >
    //	{ 0 | 1 < ALPHA : bit (4) >
    //	< GAMMA_TN : bit (5) >}
    //	{ 0 | 1 < P0 : bit (4) >
    //		< BTS_PWR_CTRL_MODE : bit (1) >
    //			< PR_MODE : bit (1) > }
    //	< TBF Starting Time : < Starting Frame Number Description IE > >
    //	< NUMBER OF RADIO BLOCKS ALLOCATED: bit (2)>;
    ED_OCTET TIMESLOT_NUMBER;
    ED_BOOLEAN ALPHA_Present ;
    ED_OCTET ALPHA;
	ED_BOOLEAN GAMMA_TN_Present ;
    ED_OCTET  GAMMA_TN;
	ED_BOOLEAN P0_Present;
    ED_OCTET  P0;
    ED_BOOLEAN BTS_PWR_CTRL_MODE_Present;
    ED_OCTET BTS_PWR_CTRL_MODE;
   	ED_BOOLEAN PR_MODE_Present;
    ED_OCTET PR_MODE;
    c_Starting_Frame_Number_Description TBF_Starting_Time;
    ED_OCTET	NUMBER_OF_RADIO_BLOCKS_ALLOCATED;
}c_PUplink_Assignment_Msg_Multi_Block_Allocation;
long DECODE_c_PUplink_Assignment_Msg_Multi_Block_Allocation(const char* const Buffer, 
                                                            const long BitOffset,
            c_PUplink_Assignment_Msg_Multi_Block_Allocation* const,
                                                                long Length);
//-----------------------------------------------------------------------
typedef struct _c_PUplink_Assignment_Msg {
    ED_OCTET CHANNEL_CODING_COMMAND;
    ED_BOOLEAN CHANNEL_CODING_COMMAND_Present;
    ED_BOOLEAN Dynamic_Alloc_Present;
    ED_BOOLEAN Fixed_Alloc_Present;
    ED_BOOLEAN Freq_Param_Present;
    ED_OCTET PExtended_Timing_Advance;
    ED_BOOLEAN PExtended_Timing_Advance_Present;
    ED_BOOLEAN PTiming_Advance_Present;
    ED_BOOLEAN Single_Block_Alloc_Present;
    ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING;
    ED_BOOLEAN TLLI_BLOCK_CHANNEL_CODING_Present;
    c_PUplink_Assignment_Msg_Dynamic_Alloc Dynamic_Alloc; // private
    c_PUplink_Assignment_Msg_Fixed_Alloc Fixed_Alloc;	 // private	
    // changed by dingmx for remove same struct
    //    c_PUplink_Assignment_Msg_Freq_Param Freq_Param;	
    c_Freq_Param_IE Freq_Param;
    //    c_PUplink_Assignment_Msg_PTiming_Advance PTiming_Advance;
    c_PTiming_Advance_IE PTiming_Advance;
    c_PUplink_Assignment_Msg_Single_Block_Alloc Single_Block_Alloc;  // private
// add by dingmx 20061031 for EDGE    
    ED_BOOLEAN CONTENTION_RESOLUTION_TLLI_Present;
    ED_LONG CONTENTION_RESOLUTION_TLLI;
    ED_BOOLEAN COMPACT_reduced_MA_Present;
    c_COMPACT_reduced_MA COMPACT_reduced_MA;
    ED_BOOLEAN EGPRS_Channel_Coding_Command_Present;
    ED_OCTET     EGPRS_Channel_Coding_Command;
	ED_BOOLEAN RESEGMENT_Present;
	ED_OCTET	RESEGMENT;
    ED_BOOLEAN EGPRS_Window_Size_Present;
    ED_OCTET EGPRS_Window_Size;

	ED_BOOLEAN Access_Tech_Req_Present;//add by lincq
    ED_OCTET Access_Tech_Req_num; // private
 	ED_OCTET Access_Tech_Req[16];  //2^4 = 16
    
    ED_BOOLEAN ARAC_RETRANSMISSION_REQ_Present;
    ED_OCTET  ARAC_RETRANSMISSION_REQ;
    
    ED_BOOLEAN BEP_PERIOD2_Present;
    ED_OCTET BEP_PERIOD2;
    
    ED_BOOLEAN Multi_Block_Allocation_Present;
    c_PUplink_Assignment_Msg_Multi_Block_Allocation Multi_Block_Allocation;   // private

	ED_BOOLEAN Egprs_Present;
}c_PUplink_Assignment_Msg;
long DECODE_c_PUplink_Assignment_Msg( const char* const Buffer, 
                                      const long BitOffset, 
                                      c_PUplink_Assignment_Msg* const Destin,
                                      long Length );

#endif
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
};
#endif

#endif

