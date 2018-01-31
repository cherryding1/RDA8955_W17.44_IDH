/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rri_ed_c_h_H
#define __rri_ed_c_h_H
#include "ed_data.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "rrd_ed_c.h"

/* Access member 'BreakInd' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_BreakInd(var) var

/* Access member 'BreakInd' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_BreakInd(var) (&var)

/* Access member 'Cbq' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Cbq(var) var

/* Access member 'Cbq' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Cbq(var) (&var)

/* Access member 'Cro' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Cro(var) var

/* Access member 'Cro' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Cro(var) (&var)

/* Access member 'Ecsc' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Ecsc(var) var

/* Access member 'Ecsc' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Ecsc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_PenaltyTime(var) (&var)

/* Access member 'PwrOffset' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_PwrOffset(var) var

/* Access member 'PwrOffset' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_PwrOffset(var) (&var)

/* Access member 'RaColour' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_RaColour(var) var

/* Access member 'RaColour' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_RaColour(var) (&var)

/* Access member 'Si13Pos' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Si13Pos(var) var

/* Access member 'Si13Pos' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Si13Pos(var) (&var)

/* Access member 'Si2QPos' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Si2QPos(var) var

/* Access member 'Si2QPos' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Si2QPos(var) (&var)

/* Access member 'Si2TInd' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_Si2TInd(var) var

/* Access member 'Si2TInd' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_Si2TInd(var) (&var)

/* Access member 'TmpOffset' of type 'c_Si3_4Ro' as a variable reference */
#define VAR_c_Si3_4Ro_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_Si3_4Ro' as a pointer */
#define PTR_c_Si3_4Ro_TmpOffset(var) (&var)


/*-----------------------------------*/
typedef struct _c_Si3_4Ro {
	ED_BOOLEAN BreakInd;
	ED_BOOLEAN BreakInd_Present;
	ED_BOOLEAN Cbq;
	ED_BOOLEAN Cbq_Present;
	ED_OCTET Cro;
	ED_BOOLEAN Cro_Present;
	ED_BOOLEAN Ecsc;
	ED_BOOLEAN Ecsc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PwrOffset;
	ED_BOOLEAN PwrOffset_Present;
	ED_OCTET RaColour;
	ED_BOOLEAN RaColour_Present;
	ED_BOOLEAN Si13Pos;
	ED_BOOLEAN Si13Pos_Present;
	ED_BOOLEAN Si2QPos;
	ED_BOOLEAN Si2QPos_Present;
	ED_BOOLEAN Si2TInd;
	ED_BOOLEAN Si2TInd_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

}	c_Si3_4Ro;
#define INIT_c_Si3_4Ro(sp) ED_RESET_MEM ((sp), sizeof (c_Si3_4Ro))
#define FREE_c_Si3_4Ro(sp)
#define SETPRESENT_c_Si3_4Ro_BreakInd(sp,present) ((sp)->BreakInd_Present = present)
#define GETPRESENT_c_Si3_4Ro_BreakInd(sp) ((sp)->BreakInd_Present)
#define SETPRESENT_c_Si3_4Ro_Cbq(sp,present) ((sp)->Cbq_Present = present)
#define GETPRESENT_c_Si3_4Ro_Cbq(sp) ((sp)->Cbq_Present)
#define SETPRESENT_c_Si3_4Ro_Cro(sp,present) ((sp)->Cro_Present = present)
#define GETPRESENT_c_Si3_4Ro_Cro(sp) ((sp)->Cro_Present)
#define SETPRESENT_c_Si3_4Ro_Ecsc(sp,present) ((sp)->Ecsc_Present = present)
#define GETPRESENT_c_Si3_4Ro_Ecsc(sp) ((sp)->Ecsc_Present)
#define SETPRESENT_c_Si3_4Ro_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_Si3_4Ro_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_Si3_4Ro_PwrOffset(sp,present) ((sp)->PwrOffset_Present = present)
#define GETPRESENT_c_Si3_4Ro_PwrOffset(sp) ((sp)->PwrOffset_Present)
#define SETPRESENT_c_Si3_4Ro_RaColour(sp,present) ((sp)->RaColour_Present = present)
#define GETPRESENT_c_Si3_4Ro_RaColour(sp) ((sp)->RaColour_Present)
#define SETPRESENT_c_Si3_4Ro_Si13Pos(sp,present) ((sp)->Si13Pos_Present = present)
#define GETPRESENT_c_Si3_4Ro_Si13Pos(sp) ((sp)->Si13Pos_Present)
#define SETPRESENT_c_Si3_4Ro_Si2QPos(sp,present) ((sp)->Si2QPos_Present = present)
#define GETPRESENT_c_Si3_4Ro_Si2QPos(sp) ((sp)->Si2QPos_Present)
#define SETPRESENT_c_Si3_4Ro_Si2TInd(sp,present) ((sp)->Si2TInd_Present = present)
#define GETPRESENT_c_Si3_4Ro_Si2TInd(sp) ((sp)->Si2TInd_Present)
#define SETPRESENT_c_Si3_4Ro_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_Si3_4Ro_TmpOffset(sp) ((sp)->TmpOffset_Present)

/* Access member 'Alpha' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Alpha(var) var

/* Access member 'Alpha' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Alpha(var) (&var)

/* Access member 'Arfcn' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Arfcn(var) var

/* Access member 'Arfcn' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Arfcn(var) (&var)

/* Access member 'BcchChgeMark' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_BcchChgeMark(var) var

/* Access member 'BcchChgeMark' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_BcchChgeMark(var) (&var)

/* Access member 'AccBurstType' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_AccBurstType(var) var

/* Access member 'AccBurstType' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_AccBurstType(var) (&var)

/* Access member 'BsCvMax' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_BsCvMax(var) var

/* Access member 'BsCvMax' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_BsCvMax(var) (&var)

/* Access member 'BssPagCoord' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_BssPagCoord(var) var

/* Access member 'BssPagCoord' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_BssPagCoord(var) (&var)

/* Access member 'CtrlAckType' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_CtrlAckType(var) var

/* Access member 'CtrlAckType' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_CtrlAckType(var) (&var)

/* Access member 'DrxTimerMax' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_DrxTimerMax(var) var

/* Access member 'DrxTimerMax' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_DrxTimerMax(var) (&var)

/* Access member 'Extension_Len' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_Extension_Len(var) var

/* Access member 'Extension_Len' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_Extension_Len(var) (&var)

/* Access member 'Message_escape' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_Message_escape(var) var

/* Access member 'Message_escape' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_Message_escape(var) (&var)

/* Access member 'Nmo' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_Nmo(var) var

/* Access member 'Nmo' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_Nmo(var) (&var)

/* Access member 'PanDec' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_PanDec(var) var

/* Access member 'PanDec' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_PanDec(var) (&var)

/* Access member 'PanInc' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_PanInc(var) var

/* Access member 'PanInc' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_PanInc(var) (&var)

/* Access member 'PanMax' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_PanMax(var) var

/* Access member 'PanMax' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_PanMax(var) (&var)

/* Access member 'PfcFeatMode' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_PfcFeatMode(var) var

/* Access member 'PfcFeatMode' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_PfcFeatMode(var) (&var)

/* Access member 'T3168' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_T3168(var) var

/* Access member 'T3168' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_T3168(var) (&var)

/* Access member 'T3192' of type 'c_Si13Ro_GprsCellOpt' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt_T3192(var) var

/* Access member 'T3192' of type 'c_Si13Ro_GprsCellOpt' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt_T3192(var) (&var)

/* Access member 'GprsCellOpt' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_GprsCellOpt(var) var

/* Access member 'GprsCellOpt' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_GprsCellOpt(var) (&var)

/* Access member 'data' of type 'c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx_data(var) var

/* Access member 'data' of type 'c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx' as a pointer */
#define PTR_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx_data(var) (&var)

/* Access member 'ArfcnIdx' of type 'c_Si13Ro_GprsMa_ArfcnIdxList' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx(var) var

/* Access member 'ArfcnIdx' of type 'c_Si13Ro_GprsMa_ArfcnIdxList' as a pointer */
#define PTR_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx(var) (&var)

/* Access member 'ArfcnIdxList' of type 'c_Si13Ro_GprsMa' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_ArfcnIdxList(var) var

/* Access member 'ArfcnIdxList' of type 'c_Si13Ro_GprsMa' as a pointer */
#define PTR_c_Si13Ro_GprsMa_ArfcnIdxList(var) (&var)

/* Access member 'Hsn' of type 'c_Si13Ro_GprsMa' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_Hsn(var) var

/* Access member 'Hsn' of type 'c_Si13Ro_GprsMa' as a pointer */
#define PTR_c_Si13Ro_GprsMa_Hsn(var) (&var)

/* Access member 'MaBitMap' of type 'c_Si13Ro_GprsMa' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_MaBitMap(var) var

/* Access member 'MaBitMap' of type 'c_Si13Ro_GprsMa' as a pointer */
#define PTR_c_Si13Ro_GprsMa_MaBitMap(var) (&var)

/* Access member 'MaLen' of type 'c_Si13Ro_GprsMa' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_MaLen(var) var

/* Access member 'MaLen' of type 'c_Si13Ro_GprsMa' as a pointer */
#define PTR_c_Si13Ro_GprsMa_MaLen(var) (&var)

/* Access member 'data' of type 'c_Si13Ro_GprsMa_RflNbList_RflNb' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_RflNbList_RflNb_data(var) var

/* Access member 'data' of type 'c_Si13Ro_GprsMa_RflNbList_RflNb' as a pointer */
#define PTR_c_Si13Ro_GprsMa_RflNbList_RflNb_data(var) (&var)

/* Access member 'RflNb' of type 'c_Si13Ro_GprsMa_RflNbList' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_RflNbList_RflNb(var) var

/* Access member 'RflNb' of type 'c_Si13Ro_GprsMa_RflNbList' as a pointer */
#define PTR_c_Si13Ro_GprsMa_RflNbList_RflNb(var) (&var)

/* Access member 'RflNbList' of type 'c_Si13Ro_GprsMa' as a variable reference */
#define VAR_c_Si13Ro_GprsMa_RflNbList(var) var

/* Access member 'RflNbList' of type 'c_Si13Ro_GprsMa' as a pointer */
#define PTR_c_Si13Ro_GprsMa_RflNbList(var) (&var)

/* Access member 'GprsMa' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_GprsMa(var) var

/* Access member 'GprsMa' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_GprsMa(var) (&var)

/* Access member 'Maio' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Maio(var) var

/* Access member 'Maio' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Maio(var) (&var)

/* Access member 'NAvgI' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_NAvgI(var) var

/* Access member 'NAvgI' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_NAvgI(var) (&var)

/* Access member 'NwkCtrlOrder' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_NwkCtrlOrder(var) var

/* Access member 'NwkCtrlOrder' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_NwkCtrlOrder(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_PSi1RepPeriod(var) (&var)

/* Access member 'Pb' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Pb(var) var

/* Access member 'Pb' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Pb(var) (&var)

/* Access member 'PcMeasChan' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_PcMeasChan(var) var

/* Access member 'PcMeasChan' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_PcMeasChan(var) (&var)

/* Access member 'PrioAccThr' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_PrioAccThr(var) var

/* Access member 'PrioAccThr' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_PrioAccThr(var) (&var)

/* Access member 'Rac' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Rac(var) var

/* Access member 'Rac' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Rac(var) (&var)

/* Access member 'Sgsnr' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Sgsnr(var) var

/* Access member 'Sgsnr' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Sgsnr(var) (&var)

/* Access member 'Si13ChgeMark' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Si13ChgeMark(var) var

/* Access member 'Si13ChgeMark' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Si13ChgeMark(var) (&var)

/* Access member 'SiChgeField' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_SiChgeField(var) var

/* Access member 'SiChgeField' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_SiChgeField(var) (&var)

/* Access member 'SpgcCcchSup' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_SpgcCcchSup(var) var

/* Access member 'SpgcCcchSup' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_SpgcCcchSup(var) (&var)

/* Access member 'TAvgT' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_TAvgT(var) var

/* Access member 'TAvgT' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_TAvgT(var) (&var)

/* Access member 'TAvgW' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_TAvgW(var) var

/* Access member 'TAvgW' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_TAvgW(var) (&var)

/* Access member 'Tn' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Tn(var) var

/* Access member 'Tn' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Tn(var) (&var)

/* Access member 'Tsc' of type 'c_Si13Ro' as a variable reference */
#define VAR_c_Si13Ro_Tsc(var) var

/* Access member 'Tsc' of type 'c_Si13Ro' as a pointer */
#define PTR_c_Si13Ro_Tsc(var) (&var)

/* DEFINITION OF BINARY c_Si13Ro_GprsCellOpt_Message_escape */
typedef struct _c_Si13Ro_GprsCellOpt_Message_escape {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Si13Ro_GprsCellOpt_Message_escape;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Si13Ro_GprsCellOpt_Message_escape(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsCellOpt */
typedef struct _c_Si13Ro_GprsCellOpt {
	ED_BOOLEAN AccBurstType;
	ED_OCTET BsCvMax;
	ED_BOOLEAN BssPagCoord;
	ED_BOOLEAN BssPagCoord_Present;
	ED_BOOLEAN CtrlAckType;
	ED_OCTET DrxTimerMax;
	ED_OCTET Extension_Len;
	ED_BOOLEAN Extension_Len_Present;
	ED_BOOLEAN Message_escape_Present;
	ED_OCTET Nmo;
	ED_OCTET PanDec;
	ED_BOOLEAN PanDec_Present;
	ED_OCTET PanInc;
	ED_BOOLEAN PanInc_Present;
	ED_OCTET PanMax;
	ED_BOOLEAN PanMax_Present;
	ED_BOOLEAN PfcFeatMode;
	ED_BOOLEAN PfcFeatMode_Present;
	ED_OCTET T3168;
	ED_OCTET T3192;
	c_Si13Ro_GprsCellOpt_Message_escape Message_escape;

} c_Si13Ro_GprsCellOpt;

/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx */
typedef struct _c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx {
	ED_OCTET data [20];
	int items;
} c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx;

/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsMa_ArfcnIdxList */
typedef struct _c_Si13Ro_GprsMa_ArfcnIdxList {
	c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx ArfcnIdx;

} c_Si13Ro_GprsMa_ArfcnIdxList;

/* DEFINITION OF BINARY c_Si13Ro_GprsMa_MaBitMap */
typedef struct _c_Si13Ro_GprsMa_MaBitMap {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_Si13Ro_GprsMa_MaBitMap;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_Si13Ro_GprsMa_MaBitMap(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsMa_RflNbList_RflNb */
typedef struct _c_Si13Ro_GprsMa_RflNbList_RflNb {
	ED_OCTET data [20];
	int items;
} c_Si13Ro_GprsMa_RflNbList_RflNb;

/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsMa_RflNbList */
typedef struct _c_Si13Ro_GprsMa_RflNbList {
	c_Si13Ro_GprsMa_RflNbList_RflNb RflNb;

} c_Si13Ro_GprsMa_RflNbList;

/* DEFINITION OF SUB-STRUCTURE c_Si13Ro_GprsMa */
typedef struct _c_Si13Ro_GprsMa {
	ED_BOOLEAN ArfcnIdxList_Present;
	ED_OCTET Hsn;
	ED_BOOLEAN MaBitMap_Present;
	ED_OCTET MaLen;
	ED_BOOLEAN MaLen_Present;
	ED_BOOLEAN RflNbList_Present;
	c_Si13Ro_GprsMa_ArfcnIdxList ArfcnIdxList;
	c_Si13Ro_GprsMa_MaBitMap MaBitMap;
	c_Si13Ro_GprsMa_RflNbList RflNbList;

} c_Si13Ro_GprsMa;


/*-----------------------------------*/
typedef struct _c_Si13Ro {
	ED_SHORT Arfcn;
	ED_OCTET Alpha;
	ED_BOOLEAN Alpha_Present;
	ED_BOOLEAN Arfcn_Present;
	ED_OCTET BcchChgeMark;
	ED_BOOLEAN BcchChgeMark_Present;
	ED_BOOLEAN GprsCellOpt_Present;
	ED_BOOLEAN GprsMa_Present;
	ED_OCTET Maio;
	ED_BOOLEAN Maio_Present;
	ED_OCTET NAvgI;
	ED_BOOLEAN NAvgI_Present;
	ED_OCTET NwkCtrlOrder;
	ED_BOOLEAN NwkCtrlOrder_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET Pb;
	ED_BOOLEAN Pb_Present;
	ED_BOOLEAN PcMeasChan;
	ED_BOOLEAN PcMeasChan_Present;
	ED_OCTET PrioAccThr;
	ED_BOOLEAN PrioAccThr_Present;
	ED_OCTET Rac;
	ED_BOOLEAN Rac_Present;
	ED_BOOLEAN Sgsnr;
	ED_BOOLEAN Sgsnr_Present;
	ED_OCTET Si13ChgeMark;
	ED_BOOLEAN Si13ChgeMark_Present;
	ED_OCTET SiChgeField;
	ED_BOOLEAN SiChgeField_Present;
	ED_BOOLEAN SpgcCcchSup;
	ED_BOOLEAN SpgcCcchSup_Present;
	ED_OCTET TAvgT;
	ED_BOOLEAN TAvgT_Present;
	ED_OCTET TAvgW;
	ED_BOOLEAN TAvgW_Present;
	ED_OCTET Tn;
	ED_BOOLEAN Tn_Present;
	ED_OCTET Tsc;
	ED_BOOLEAN Tsc_Present;
	c_Si13Ro_GprsCellOpt GprsCellOpt;
	c_Si13Ro_GprsMa GprsMa;

}	c_Si13Ro;
#define INIT_c_Si13Ro(sp) ED_RESET_MEM ((sp), sizeof (c_Si13Ro))
#define FREE_c_Si13Ro(sp)
#define SETPRESENT_c_Si13Ro_Alpha(sp,present) ((sp)->Alpha_Present = present)
#define GETPRESENT_c_Si13Ro_Alpha(sp) ((sp)->Alpha_Present)
#define SETPRESENT_c_Si13Ro_Arfcn(sp,present) ((sp)->Arfcn_Present = present)
#define GETPRESENT_c_Si13Ro_Arfcn(sp) ((sp)->Arfcn_Present)
#define SETPRESENT_c_Si13Ro_BcchChgeMark(sp,present) ((sp)->BcchChgeMark_Present = present)
#define GETPRESENT_c_Si13Ro_BcchChgeMark(sp) ((sp)->BcchChgeMark_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt(sp,present) ((sp)->GprsCellOpt_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt(sp) ((sp)->GprsCellOpt_Present)
#define SETPRESENT_c_Si13Ro_GprsMa(sp,present) ((sp)->GprsMa_Present = present)
#define GETPRESENT_c_Si13Ro_GprsMa(sp) ((sp)->GprsMa_Present)
#define SETPRESENT_c_Si13Ro_Maio(sp,present) ((sp)->Maio_Present = present)
#define GETPRESENT_c_Si13Ro_Maio(sp) ((sp)->Maio_Present)
#define SETPRESENT_c_Si13Ro_NAvgI(sp,present) ((sp)->NAvgI_Present = present)
#define GETPRESENT_c_Si13Ro_NAvgI(sp) ((sp)->NAvgI_Present)
#define SETPRESENT_c_Si13Ro_NwkCtrlOrder(sp,present) ((sp)->NwkCtrlOrder_Present = present)
#define GETPRESENT_c_Si13Ro_NwkCtrlOrder(sp) ((sp)->NwkCtrlOrder_Present)
#define SETPRESENT_c_Si13Ro_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_Si13Ro_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_Si13Ro_Pb(sp,present) ((sp)->Pb_Present = present)
#define GETPRESENT_c_Si13Ro_Pb(sp) ((sp)->Pb_Present)
#define SETPRESENT_c_Si13Ro_PcMeasChan(sp,present) ((sp)->PcMeasChan_Present = present)
#define GETPRESENT_c_Si13Ro_PcMeasChan(sp) ((sp)->PcMeasChan_Present)
#define SETPRESENT_c_Si13Ro_PrioAccThr(sp,present) ((sp)->PrioAccThr_Present = present)
#define GETPRESENT_c_Si13Ro_PrioAccThr(sp) ((sp)->PrioAccThr_Present)
#define SETPRESENT_c_Si13Ro_Rac(sp,present) ((sp)->Rac_Present = present)
#define GETPRESENT_c_Si13Ro_Rac(sp) ((sp)->Rac_Present)
#define SETPRESENT_c_Si13Ro_Sgsnr(sp,present) ((sp)->Sgsnr_Present = present)
#define GETPRESENT_c_Si13Ro_Sgsnr(sp) ((sp)->Sgsnr_Present)
#define SETPRESENT_c_Si13Ro_Si13ChgeMark(sp,present) ((sp)->Si13ChgeMark_Present = present)
#define GETPRESENT_c_Si13Ro_Si13ChgeMark(sp) ((sp)->Si13ChgeMark_Present)
#define SETPRESENT_c_Si13Ro_SiChgeField(sp,present) ((sp)->SiChgeField_Present = present)
#define GETPRESENT_c_Si13Ro_SiChgeField(sp) ((sp)->SiChgeField_Present)
#define SETPRESENT_c_Si13Ro_SpgcCcchSup(sp,present) ((sp)->SpgcCcchSup_Present = present)
#define GETPRESENT_c_Si13Ro_SpgcCcchSup(sp) ((sp)->SpgcCcchSup_Present)
#define SETPRESENT_c_Si13Ro_TAvgT(sp,present) ((sp)->TAvgT_Present = present)
#define GETPRESENT_c_Si13Ro_TAvgT(sp) ((sp)->TAvgT_Present)
#define SETPRESENT_c_Si13Ro_TAvgW(sp,present) ((sp)->TAvgW_Present = present)
#define GETPRESENT_c_Si13Ro_TAvgW(sp) ((sp)->TAvgW_Present)
#define SETPRESENT_c_Si13Ro_Tn(sp,present) ((sp)->Tn_Present = present)
#define GETPRESENT_c_Si13Ro_Tn(sp) ((sp)->Tn_Present)
#define SETPRESENT_c_Si13Ro_Tsc(sp,present) ((sp)->Tsc_Present = present)
#define GETPRESENT_c_Si13Ro_Tsc(sp) ((sp)->Tsc_Present)

/* Access member 'AccBurstType' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_AccBurstType(var) var

/* Access member 'AccBurstType' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_AccBurstType(var) (&var)

/* Access member 'BsCvMax' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_BsCvMax(var) var

/* Access member 'BsCvMax' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_BsCvMax(var) (&var)

/* Access member 'BssPagCoord' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_BssPagCoord(var) var

/* Access member 'BssPagCoord' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_BssPagCoord(var) (&var)

/* Access member 'CtrlAckType' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_CtrlAckType(var) var

/* Access member 'CtrlAckType' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_CtrlAckType(var) (&var)

/* Access member 'DrxTimerMax' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_DrxTimerMax(var) var

/* Access member 'DrxTimerMax' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_DrxTimerMax(var) (&var)

/* Access member 'Extension_Len' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_Extension_Len(var) var

/* Access member 'Extension_Len' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_Extension_Len(var) (&var)

/* Access member 'Message_escape' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_Message_escape(var) var

/* Access member 'Message_escape' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_Message_escape(var) (&var)

/* Access member 'Nmo' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_Nmo(var) var

/* Access member 'Nmo' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_Nmo(var) (&var)

/* Access member 'PanDec' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_PanDec(var) var

/* Access member 'PanDec' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_PanDec(var) (&var)

/* Access member 'PanInc' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_PanInc(var) var

/* Access member 'PanInc' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_PanInc(var) (&var)

/* Access member 'PanMax' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_PanMax(var) var

/* Access member 'PanMax' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_PanMax(var) (&var)

/* Access member 'PfcFeatMode' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_PfcFeatMode(var) var

/* Access member 'PfcFeatMode' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_PfcFeatMode(var) (&var)

/* Access member 'T3168' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_T3168(var) var

/* Access member 'T3168' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_T3168(var) (&var)

/* Access member 'T3192' of type 'c_GprsCellOptIE' as a variable reference */
#define VAR_c_GprsCellOptIE_T3192(var) var

/* Access member 'T3192' of type 'c_GprsCellOptIE' as a pointer */
#define PTR_c_GprsCellOptIE_T3192(var) (&var)

/* DEFINITION OF BINARY c_GprsCellOptIE_Message_escape */
typedef struct _c_GprsCellOptIE_Message_escape {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_GprsCellOptIE_Message_escape;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_GprsCellOptIE_Message_escape(sp,bits) (sp)->usedBits = 0



/*-----------------------------------*/
typedef struct _c_GprsCellOptIE {
	ED_BOOLEAN AccBurstType;
	ED_OCTET BsCvMax;
	ED_BOOLEAN BssPagCoord;
	ED_BOOLEAN BssPagCoord_Present;
	ED_BOOLEAN CtrlAckType;
	ED_OCTET DrxTimerMax;
	ED_OCTET Extension_Len;
	ED_BOOLEAN Extension_Len_Present;
	ED_BOOLEAN Message_escape_Present;
	ED_OCTET Nmo;
	ED_OCTET PanDec;
	ED_BOOLEAN PanDec_Present;
	ED_OCTET PanInc;
	ED_BOOLEAN PanInc_Present;
	ED_OCTET PanMax;
	ED_BOOLEAN PanMax_Present;
	ED_BOOLEAN PfcFeatMode;
	ED_BOOLEAN PfcFeatMode_Present;
	ED_OCTET T3168;
	ED_OCTET T3192;
	c_GprsCellOptIE_Message_escape Message_escape;

}	c_GprsCellOptIE;
#define INIT_c_GprsCellOptIE(sp) ED_RESET_MEM ((sp), sizeof (c_GprsCellOptIE))
#define FREE_c_GprsCellOptIE(sp)
#define SETPRESENT_c_GprsCellOptIE_BssPagCoord(sp,present) ((sp)->BssPagCoord_Present = present)
#define GETPRESENT_c_GprsCellOptIE_BssPagCoord(sp) ((sp)->BssPagCoord_Present)
#define SETPRESENT_c_GprsCellOptIE_Extension_Len(sp,present) ((sp)->Extension_Len_Present = present)
#define GETPRESENT_c_GprsCellOptIE_Extension_Len(sp) ((sp)->Extension_Len_Present)
#define SETPRESENT_c_GprsCellOptIE_Message_escape(sp,present) ((sp)->Message_escape_Present = present)
#define GETPRESENT_c_GprsCellOptIE_Message_escape(sp) ((sp)->Message_escape_Present)
#define SETPRESENT_c_GprsCellOptIE_PanDec(sp,present) ((sp)->PanDec_Present = present)
#define GETPRESENT_c_GprsCellOptIE_PanDec(sp) ((sp)->PanDec_Present)
#define SETPRESENT_c_GprsCellOptIE_PanInc(sp,present) ((sp)->PanInc_Present = present)
#define GETPRESENT_c_GprsCellOptIE_PanInc(sp) ((sp)->PanInc_Present)
#define SETPRESENT_c_GprsCellOptIE_PanMax(sp,present) ((sp)->PanMax_Present = present)
#define GETPRESENT_c_GprsCellOptIE_PanMax(sp) ((sp)->PanMax_Present)
#define SETPRESENT_c_GprsCellOptIE_PfcFeatMode(sp,present) ((sp)->PfcFeatMode_Present = present)
#define GETPRESENT_c_GprsCellOptIE_PfcFeatMode(sp) ((sp)->PfcFeatMode_Present)

/* Access member 'RepTyp' of type 'c_MeasParDesc' as a variable reference */
#define VAR_c_MeasParDesc_RepTyp(var) var

/* Access member 'RepTyp' of type 'c_MeasParDesc' as a pointer */
#define PTR_c_MeasParDesc_RepTyp(var) (&var)

/* Access member 'ServBandRep' of type 'c_MeasParDesc' as a variable reference */
#define VAR_c_MeasParDesc_ServBandRep(var) var

/* Access member 'ServBandRep' of type 'c_MeasParDesc' as a pointer */
#define PTR_c_MeasParDesc_ServBandRep(var) (&var)


/*-----------------------------------*/
typedef struct _c_MeasParDesc {
	ED_BOOLEAN RepTyp;
	ED_OCTET ServBandRep;

}	c_MeasParDesc;
#define INIT_c_MeasParDesc(sp) ED_RESET_MEM ((sp), sizeof (c_MeasParDesc))
#define FREE_c_MeasParDesc(sp)

/* Access member 'IdxStartRtd12' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_IdxStartRtd12(var) var

/* Access member 'IdxStartRtd12' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_IdxStartRtd12(var) (&var)

/* Access member 'IdxStartRtd6' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_IdxStartRtd6(var) var

/* Access member 'IdxStartRtd6' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_IdxStartRtd6(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_Rtd12L_data_Rtd12' as a variable reference */
#define VAR_c_RtdDesc_Rtd12L_data_Rtd12_data(var) var

/* Access member 'data' of type 'c_RtdDesc_Rtd12L_data_Rtd12' as a pointer */
#define PTR_c_RtdDesc_Rtd12L_data_Rtd12_data(var) (&var)

/* Access member 'Rtd12' of type 'c_RtdDesc_Rtd12L_data' as a variable reference */
#define VAR_c_RtdDesc_Rtd12L_data_Rtd12(var) var

/* Access member 'Rtd12' of type 'c_RtdDesc_Rtd12L_data' as a pointer */
#define PTR_c_RtdDesc_Rtd12L_data_Rtd12(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_Rtd12L' as a variable reference */
#define VAR_c_RtdDesc_Rtd12L_data(var) var

/* Access member 'data' of type 'c_RtdDesc_Rtd12L' as a pointer */
#define PTR_c_RtdDesc_Rtd12L_data(var) (&var)

/* Access member 'Rtd12L' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_Rtd12L(var) var

/* Access member 'Rtd12L' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_Rtd12L(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_Rtd6L_data_Rtd6' as a variable reference */
#define VAR_c_RtdDesc_Rtd6L_data_Rtd6_data(var) var

/* Access member 'data' of type 'c_RtdDesc_Rtd6L_data_Rtd6' as a pointer */
#define PTR_c_RtdDesc_Rtd6L_data_Rtd6_data(var) (&var)

/* Access member 'Rtd6' of type 'c_RtdDesc_Rtd6L_data' as a variable reference */
#define VAR_c_RtdDesc_Rtd6L_data_Rtd6(var) var

/* Access member 'Rtd6' of type 'c_RtdDesc_Rtd6L_data' as a pointer */
#define PTR_c_RtdDesc_Rtd6L_data_Rtd6(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_Rtd6L' as a variable reference */
#define VAR_c_RtdDesc_Rtd6L_data(var) var

/* Access member 'data' of type 'c_RtdDesc_Rtd6L' as a pointer */
#define PTR_c_RtdDesc_Rtd6L_data(var) (&var)

/* Access member 'Rtd6L' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_Rtd6L(var) var

/* Access member 'Rtd6L' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_Rtd6L(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_StartRtd12L_Rtd12' as a variable reference */
#define VAR_c_RtdDesc_StartRtd12L_Rtd12_data(var) var

/* Access member 'data' of type 'c_RtdDesc_StartRtd12L_Rtd12' as a pointer */
#define PTR_c_RtdDesc_StartRtd12L_Rtd12_data(var) (&var)

/* Access member 'Rtd12' of type 'c_RtdDesc_StartRtd12L' as a variable reference */
#define VAR_c_RtdDesc_StartRtd12L_Rtd12(var) var

/* Access member 'Rtd12' of type 'c_RtdDesc_StartRtd12L' as a pointer */
#define PTR_c_RtdDesc_StartRtd12L_Rtd12(var) (&var)

/* Access member 'StartRtd12L' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_StartRtd12L(var) var

/* Access member 'StartRtd12L' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_StartRtd12L(var) (&var)

/* Access member 'data' of type 'c_RtdDesc_StartRtd6L_Rtd6' as a variable reference */
#define VAR_c_RtdDesc_StartRtd6L_Rtd6_data(var) var

/* Access member 'data' of type 'c_RtdDesc_StartRtd6L_Rtd6' as a pointer */
#define PTR_c_RtdDesc_StartRtd6L_Rtd6_data(var) (&var)

/* Access member 'Rtd6' of type 'c_RtdDesc_StartRtd6L' as a variable reference */
#define VAR_c_RtdDesc_StartRtd6L_Rtd6(var) var

/* Access member 'Rtd6' of type 'c_RtdDesc_StartRtd6L' as a pointer */
#define PTR_c_RtdDesc_StartRtd6L_Rtd6(var) (&var)

/* Access member 'StartRtd6L' of type 'c_RtdDesc' as a variable reference */
#define VAR_c_RtdDesc_StartRtd6L(var) var

/* Access member 'StartRtd6L' of type 'c_RtdDesc' as a pointer */
#define PTR_c_RtdDesc_StartRtd6L(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd12L_data_Rtd12 */
typedef struct _c_RtdDesc_Rtd12L_data_Rtd12 {
	ED_SHORT data [3];
	int items;
} c_RtdDesc_Rtd12L_data_Rtd12;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd12L_data */
typedef struct _c_RtdDesc_Rtd12L_data {
	c_RtdDesc_Rtd12L_data_Rtd12 Rtd12;

} c_RtdDesc_Rtd12L_data;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd12L */
typedef struct _c_RtdDesc_Rtd12L {
	c_RtdDesc_Rtd12L_data data [31];
	int items;
} c_RtdDesc_Rtd12L;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd6L_data_Rtd6 */
typedef struct _c_RtdDesc_Rtd6L_data_Rtd6 {
	ED_OCTET data [3];
	int items;
} c_RtdDesc_Rtd6L_data_Rtd6;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd6L_data */
typedef struct _c_RtdDesc_Rtd6L_data {
	c_RtdDesc_Rtd6L_data_Rtd6 Rtd6;

} c_RtdDesc_Rtd6L_data;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_Rtd6L */
typedef struct _c_RtdDesc_Rtd6L {
	c_RtdDesc_Rtd6L_data data [31];
	int items;
} c_RtdDesc_Rtd6L;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_StartRtd12L_Rtd12 */
typedef struct _c_RtdDesc_StartRtd12L_Rtd12 {
	ED_SHORT data [3];
	int items;
} c_RtdDesc_StartRtd12L_Rtd12;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_StartRtd12L */
typedef struct _c_RtdDesc_StartRtd12L {
	c_RtdDesc_StartRtd12L_Rtd12 Rtd12;

} c_RtdDesc_StartRtd12L;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_StartRtd6L_Rtd6 */
typedef struct _c_RtdDesc_StartRtd6L_Rtd6 {
	ED_OCTET data [3];
	int items;
} c_RtdDesc_StartRtd6L_Rtd6;

/* DEFINITION OF SUB-STRUCTURE c_RtdDesc_StartRtd6L */
typedef struct _c_RtdDesc_StartRtd6L {
	c_RtdDesc_StartRtd6L_Rtd6 Rtd6;

} c_RtdDesc_StartRtd6L;


/*-----------------------------------*/
typedef struct _c_RtdDesc {
	ED_OCTET IdxStartRtd12;
	ED_BOOLEAN IdxStartRtd12_Present;
	ED_OCTET IdxStartRtd6;
	ED_BOOLEAN IdxStartRtd6_Present;
	ED_BOOLEAN StartRtd12L_Present;
	ED_BOOLEAN StartRtd6L_Present;
	c_RtdDesc_Rtd12L Rtd12L;
	c_RtdDesc_Rtd6L Rtd6L;
	c_RtdDesc_StartRtd12L StartRtd12L;
	c_RtdDesc_StartRtd6L StartRtd6L;

}	c_RtdDesc;
#define INIT_c_RtdDesc(sp) ED_RESET_MEM ((sp), sizeof (c_RtdDesc))
#define FREE_c_RtdDesc(sp)
#define SETPRESENT_c_RtdDesc_IdxStartRtd12(sp,present) ((sp)->IdxStartRtd12_Present = present)
#define GETPRESENT_c_RtdDesc_IdxStartRtd12(sp) ((sp)->IdxStartRtd12_Present)
#define SETPRESENT_c_RtdDesc_IdxStartRtd6(sp,present) ((sp)->IdxStartRtd6_Present = present)
#define GETPRESENT_c_RtdDesc_IdxStartRtd6(sp) ((sp)->IdxStartRtd6_Present)
#define SETPRESENT_c_RtdDesc_StartRtd12L(sp,present) ((sp)->StartRtd12L_Present = present)
#define GETPRESENT_c_RtdDesc_StartRtd12L(sp) ((sp)->StartRtd12L_Present)
#define SETPRESENT_c_RtdDesc_StartRtd6L(sp,present) ((sp)->StartRtd6L_Present = present)
#define GETPRESENT_c_RtdDesc_StartRtd6L(sp) ((sp)->StartRtd6L_Present)

/* Access member 'data' of type 'c_Rtd6Struct_Rtd6' as a variable reference */
#define VAR_c_Rtd6Struct_Rtd6_data(var) var

/* Access member 'data' of type 'c_Rtd6Struct_Rtd6' as a pointer */
#define PTR_c_Rtd6Struct_Rtd6_data(var) (&var)

/* Access member 'Rtd6' of type 'c_Rtd6Struct' as a variable reference */
#define VAR_c_Rtd6Struct_Rtd6(var) var

/* Access member 'Rtd6' of type 'c_Rtd6Struct' as a pointer */
#define PTR_c_Rtd6Struct_Rtd6(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Rtd6Struct_Rtd6 */
typedef struct _c_Rtd6Struct_Rtd6 {
	ED_OCTET data [3];
	int items;
} c_Rtd6Struct_Rtd6;


/*-----------------------------------*/
typedef struct _c_Rtd6Struct {
	c_Rtd6Struct_Rtd6 Rtd6;

}	c_Rtd6Struct;
#define INIT_c_Rtd6Struct(sp) ED_RESET_MEM ((sp), sizeof (c_Rtd6Struct))
#define FREE_c_Rtd6Struct(sp)

/* Access member 'data' of type 'c_Rtd12Struct_Rtd12' as a variable reference */
#define VAR_c_Rtd12Struct_Rtd12_data(var) var

/* Access member 'data' of type 'c_Rtd12Struct_Rtd12' as a pointer */
#define PTR_c_Rtd12Struct_Rtd12_data(var) (&var)

/* Access member 'Rtd12' of type 'c_Rtd12Struct' as a variable reference */
#define VAR_c_Rtd12Struct_Rtd12(var) var

/* Access member 'Rtd12' of type 'c_Rtd12Struct' as a pointer */
#define PTR_c_Rtd12Struct_Rtd12(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_Rtd12Struct_Rtd12 */
typedef struct _c_Rtd12Struct_Rtd12 {
	ED_SHORT data [3];
	int items;
} c_Rtd12Struct_Rtd12;


/*-----------------------------------*/
typedef struct _c_Rtd12Struct {
	c_Rtd12Struct_Rtd12 Rtd12;

}	c_Rtd12Struct;
#define INIT_c_Rtd12Struct(sp) ED_RESET_MEM ((sp), sizeof (c_Rtd12Struct))
#define FREE_c_Rtd12Struct(sp)

/* Access member 'BaIdxStartBsic' of type 'c_BsicDesc' as a variable reference */
#define VAR_c_BsicDesc_BaIdxStartBsic(var) var

/* Access member 'BaIdxStartBsic' of type 'c_BsicDesc' as a pointer */
#define PTR_c_BsicDesc_BaIdxStartBsic(var) (&var)

/* Access member 'data' of type 'c_BsicDesc_Bsic' as a variable reference */
#define VAR_c_BsicDesc_Bsic_data(var) var

/* Access member 'data' of type 'c_BsicDesc_Bsic' as a pointer */
#define PTR_c_BsicDesc_Bsic_data(var) (&var)

/* Access member 'Bsic' of type 'c_BsicDesc' as a variable reference */
#define VAR_c_BsicDesc_Bsic(var) var

/* Access member 'Bsic' of type 'c_BsicDesc' as a pointer */
#define PTR_c_BsicDesc_Bsic(var) (&var)

/* Access member 'BsicFirst' of type 'c_BsicDesc' as a variable reference */
#define VAR_c_BsicDesc_BsicFirst(var) var

/* Access member 'BsicFirst' of type 'c_BsicDesc' as a pointer */
#define PTR_c_BsicDesc_BsicFirst(var) (&var)

/* Access member 'data' of type 'c_BsicDesc_FreqScrol' as a variable reference */
#define VAR_c_BsicDesc_FreqScrol_data(var) var

/* Access member 'data' of type 'c_BsicDesc_FreqScrol' as a pointer */
#define PTR_c_BsicDesc_FreqScrol_data(var) (&var)

/* Access member 'FreqScrol' of type 'c_BsicDesc' as a variable reference */
#define VAR_c_BsicDesc_FreqScrol(var) var

/* Access member 'FreqScrol' of type 'c_BsicDesc' as a pointer */
#define PTR_c_BsicDesc_FreqScrol(var) (&var)

/* Access member 'NbRemainBsic' of type 'c_BsicDesc' as a variable reference */
#define VAR_c_BsicDesc_NbRemainBsic(var) var

/* Access member 'NbRemainBsic' of type 'c_BsicDesc' as a pointer */
#define PTR_c_BsicDesc_NbRemainBsic(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_BsicDesc_Bsic */
typedef struct _c_BsicDesc_Bsic {
	ED_OCTET data [20];
	int items;
} c_BsicDesc_Bsic;

/* DEFINITION OF SUB-STRUCTURE c_BsicDesc_FreqScrol */
typedef struct _c_BsicDesc_FreqScrol {
	ED_BOOLEAN data [20];
	int items;
} c_BsicDesc_FreqScrol;


/*-----------------------------------*/
typedef struct _c_BsicDesc {
	ED_OCTET BaIdxStartBsic;
	ED_BOOLEAN BaIdxStartBsic_Present;
	ED_OCTET BsicFirst;
	ED_OCTET NbRemainBsic;
	c_BsicDesc_Bsic Bsic;
	c_BsicDesc_FreqScrol FreqScrol;

}	c_BsicDesc;
#define INIT_c_BsicDesc(sp) ED_RESET_MEM ((sp), sizeof (c_BsicDesc))
#define FREE_c_BsicDesc(sp)
#define SETPRESENT_c_BsicDesc_BaIdxStartBsic(sp,present) ((sp)->BaIdxStartBsic_Present = present)
#define GETPRESENT_c_BsicDesc_BaIdxStartBsic(sp) ((sp)->BaIdxStartBsic_Present)

/* Access member 'NbCells' of type 'c_RepPrioDesc' as a variable reference */
#define VAR_c_RepPrioDesc_NbCells(var) var

/* Access member 'NbCells' of type 'c_RepPrioDesc' as a pointer */
#define PTR_c_RepPrioDesc_NbCells(var) (&var)

/* Access member 'data' of type 'c_RepPrioDesc_RepPrio' as a variable reference */
#define VAR_c_RepPrioDesc_RepPrio_data(var) var

/* Access member 'data' of type 'c_RepPrioDesc_RepPrio' as a pointer */
#define PTR_c_RepPrioDesc_RepPrio_data(var) (&var)

/* Access member 'RepPrio' of type 'c_RepPrioDesc' as a variable reference */
#define VAR_c_RepPrioDesc_RepPrio(var) var

/* Access member 'RepPrio' of type 'c_RepPrioDesc' as a pointer */
#define PTR_c_RepPrioDesc_RepPrio(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_RepPrioDesc_RepPrio */
typedef struct _c_RepPrioDesc_RepPrio {
	ED_BOOLEAN data [96];
	int items;
} c_RepPrioDesc_RepPrio;


/*-----------------------------------*/
typedef struct _c_RepPrioDesc {
	ED_OCTET NbCells;
	c_RepPrioDesc_RepPrio RepPrio;

}	c_RepPrioDesc;
#define INIT_c_RepPrioDesc(sp) ED_RESET_MEM ((sp), sizeof (c_RepPrioDesc))
#define FREE_c_RepPrioDesc(sp)

/* Access member 'NcNonDrxPer' of type 'c_NcMeasParStruct' as a variable reference */
#define VAR_c_NcMeasParStruct_NcNonDrxPer(var) var

/* Access member 'NcNonDrxPer' of type 'c_NcMeasParStruct' as a pointer */
#define PTR_c_NcMeasParStruct_NcNonDrxPer(var) (&var)

/* Access member 'NcRepPerI' of type 'c_NcMeasParStruct' as a variable reference */
#define VAR_c_NcMeasParStruct_NcRepPerI(var) var

/* Access member 'NcRepPerI' of type 'c_NcMeasParStruct' as a pointer */
#define PTR_c_NcMeasParStruct_NcRepPerI(var) (&var)

/* Access member 'NcRepPerT' of type 'c_NcMeasParStruct' as a variable reference */
#define VAR_c_NcMeasParStruct_NcRepPerT(var) var

/* Access member 'NcRepPerT' of type 'c_NcMeasParStruct' as a pointer */
#define PTR_c_NcMeasParStruct_NcRepPerT(var) (&var)

/* Access member 'NwkCtrlOrder' of type 'c_NcMeasParStruct' as a variable reference */
#define VAR_c_NcMeasParStruct_NwkCtrlOrder(var) var

/* Access member 'NwkCtrlOrder' of type 'c_NcMeasParStruct' as a pointer */
#define PTR_c_NcMeasParStruct_NwkCtrlOrder(var) (&var)


/*-----------------------------------*/
typedef struct _c_NcMeasParStruct {
	ED_OCTET NcNonDrxPer;
	ED_BOOLEAN NcNonDrxPer_Present;
	ED_OCTET NcRepPerI;
	ED_BOOLEAN NcRepPerI_Present;
	ED_OCTET NcRepPerT;
	ED_BOOLEAN NcRepPerT_Present;
	ED_OCTET NwkCtrlOrder;

}	c_NcMeasParStruct;
#define INIT_c_NcMeasParStruct(sp) ED_RESET_MEM ((sp), sizeof (c_NcMeasParStruct))
#define FREE_c_NcMeasParStruct(sp)
#define SETPRESENT_c_NcMeasParStruct_NcNonDrxPer(sp,present) ((sp)->NcNonDrxPer_Present = present)
#define GETPRESENT_c_NcMeasParStruct_NcNonDrxPer(sp) ((sp)->NcNonDrxPer_Present)
#define SETPRESENT_c_NcMeasParStruct_NcRepPerI(sp,present) ((sp)->NcRepPerI_Present = present)
#define GETPRESENT_c_NcMeasParStruct_NcRepPerI(sp) ((sp)->NcRepPerI_Present)
#define SETPRESENT_c_NcMeasParStruct_NcRepPerT(sp,present) ((sp)->NcRepPerT_Present = present)
#define GETPRESENT_c_NcMeasParStruct_NcRepPerT(sp) ((sp)->NcRepPerT_Present)

/* Access member 'data' of type 'c_NcFrqListDesc_ABsic' as a variable reference */
#define VAR_c_NcFrqListDesc_ABsic_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_ABsic' as a pointer */
#define PTR_c_NcFrqListDesc_ABsic_data(var) (&var)

/* Access member 'ABsic' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_ABsic(var) var

/* Access member 'ABsic' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_ABsic(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_ABsicFirst' as a variable reference */
#define VAR_c_NcFrqListDesc_ABsicFirst_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_ABsicFirst' as a pointer */
#define PTR_c_NcFrqListDesc_ABsicFirst_data(var) (&var)

/* Access member 'ABsicFirst' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_ABsicFirst(var) var

/* Access member 'ABsicFirst' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_ABsicFirst(var) (&var)

/* Access member 'CbAcc2' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_NcFrqListDesc_ACSelPar_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_NcFrqListDesc_ACSelPar_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data_TmpOffset(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_ACSelPar' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_ACSelPar' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar_data(var) (&var)

/* Access member 'ACSelPar' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelPar(var) var

/* Access member 'ACSelPar' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelPar(var) (&var)

/* Access member 'CbAcc2' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_NcFrqListDesc_ACSelParFirst_data' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data_TmpOffset(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_ACSelParFirst' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_ACSelParFirst' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst_data(var) (&var)

/* Access member 'ACSelParFirst' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_ACSelParFirst(var) var

/* Access member 'ACSelParFirst' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_ACSelParFirst(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqDiff' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqDiff_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqDiff' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqDiff_data(var) (&var)

/* Access member 'AFrqDiff' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqDiff(var) var

/* Access member 'AFrqDiff' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqDiff(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqDiffLen' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqDiffLen_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqDiffLen' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqDiffLen_data(var) (&var)

/* Access member 'AFrqDiffLen' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqDiffLen(var) var

/* Access member 'AFrqDiffLen' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqDiffLen(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqFirst' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqFirst_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_AFrqFirst' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqFirst_data(var) (&var)

/* Access member 'AFrqFirst' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_AFrqFirst(var) var

/* Access member 'AFrqFirst' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_AFrqFirst(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_NbAFrq' as a variable reference */
#define VAR_c_NcFrqListDesc_NbAFrq_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_NbAFrq' as a pointer */
#define PTR_c_NcFrqListDesc_NbAFrq_data(var) (&var)

/* Access member 'NbAFrq' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_NbAFrq(var) var

/* Access member 'NbAFrq' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_NbAFrq(var) (&var)

/* Access member 'NbRFrq' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_NbRFrq(var) var

/* Access member 'NbRFrq' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_NbRFrq(var) (&var)

/* Access member 'data' of type 'c_NcFrqListDesc_RFrqIdx' as a variable reference */
#define VAR_c_NcFrqListDesc_RFrqIdx_data(var) var

/* Access member 'data' of type 'c_NcFrqListDesc_RFrqIdx' as a pointer */
#define PTR_c_NcFrqListDesc_RFrqIdx_data(var) (&var)

/* Access member 'RFrqIdx' of type 'c_NcFrqListDesc' as a variable reference */
#define VAR_c_NcFrqListDesc_RFrqIdx(var) var

/* Access member 'RFrqIdx' of type 'c_NcFrqListDesc' as a pointer */
#define PTR_c_NcFrqListDesc_RFrqIdx(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ABsic */
typedef struct _c_NcFrqListDesc_ABsic {
	ED_OCTET data [32];
	int items;
} c_NcFrqListDesc_ABsic;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ABsicFirst */
typedef struct _c_NcFrqListDesc_ABsicFirst {
	ED_OCTET data [20];
	int items;
} c_NcFrqListDesc_ABsicFirst;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ACSelPar_data */
typedef struct _c_NcFrqListDesc_ACSelPar_data {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_NcFrqListDesc_ACSelPar_data;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ACSelPar */
typedef struct _c_NcFrqListDesc_ACSelPar {
	c_NcFrqListDesc_ACSelPar_data data [32];
	int items;
} c_NcFrqListDesc_ACSelPar;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ACSelParFirst_data */
typedef struct _c_NcFrqListDesc_ACSelParFirst_data {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_NcFrqListDesc_ACSelParFirst_data;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_ACSelParFirst */
typedef struct _c_NcFrqListDesc_ACSelParFirst {
	c_NcFrqListDesc_ACSelParFirst_data data [20];
	int items;
} c_NcFrqListDesc_ACSelParFirst;

/* DEFINITION OF BINARY c_NcFrqListDesc_AFrqDiff_data */
typedef struct _c_NcFrqListDesc_AFrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_NcFrqListDesc_AFrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_NcFrqListDesc_AFrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_AFrqDiff */
typedef struct _c_NcFrqListDesc_AFrqDiff {
	c_NcFrqListDesc_AFrqDiff_data data [32]; /* Static, variable size; bits needed 64 */
	int items;
} c_NcFrqListDesc_AFrqDiff;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_AFrqDiffLen */
typedef struct _c_NcFrqListDesc_AFrqDiffLen {
	ED_OCTET data [20];
	int items;
} c_NcFrqListDesc_AFrqDiffLen;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_AFrqFirst */
typedef struct _c_NcFrqListDesc_AFrqFirst {
	ED_SHORT data [20];
	int items;
} c_NcFrqListDesc_AFrqFirst;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_NbAFrq */
typedef struct _c_NcFrqListDesc_NbAFrq {
	ED_OCTET data [20];
	int items;
} c_NcFrqListDesc_NbAFrq;

/* DEFINITION OF SUB-STRUCTURE c_NcFrqListDesc_RFrqIdx */
typedef struct _c_NcFrqListDesc_RFrqIdx {
	ED_OCTET data [32];
	int items;
} c_NcFrqListDesc_RFrqIdx;


/*-----------------------------------*/
typedef struct _c_NcFrqListDesc {
	ED_OCTET NbRFrq;
	ED_BOOLEAN NbRFrq_Present;
	c_NcFrqListDesc_ABsic ABsic;
	c_NcFrqListDesc_ABsicFirst ABsicFirst;
	c_NcFrqListDesc_ACSelPar ACSelPar;
	c_NcFrqListDesc_ACSelParFirst ACSelParFirst;
	c_NcFrqListDesc_AFrqDiff AFrqDiff;
	c_NcFrqListDesc_AFrqDiffLen AFrqDiffLen;
	c_NcFrqListDesc_AFrqFirst AFrqFirst;
	c_NcFrqListDesc_NbAFrq NbAFrq;
	c_NcFrqListDesc_RFrqIdx RFrqIdx;

}	c_NcFrqListDesc;
#define INIT_c_NcFrqListDesc(sp) ED_RESET_MEM ((sp), sizeof (c_NcFrqListDesc))
#define FREE_c_NcFrqListDesc(sp)
#define SETPRESENT_c_NcFrqListDesc_NbRFrq(sp,present) ((sp)->NbRFrq_Present = present)
#define GETPRESENT_c_NcFrqListDesc_NbRFrq(sp) ((sp)->NbRFrq_Present)

/* Access member 'data' of type 'c_AddedFrqListStruct_ABsic' as a variable reference */
#define VAR_c_AddedFrqListStruct_ABsic_data(var) var

/* Access member 'data' of type 'c_AddedFrqListStruct_ABsic' as a pointer */
#define PTR_c_AddedFrqListStruct_ABsic_data(var) (&var)

/* Access member 'ABsic' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_ABsic(var) var

/* Access member 'ABsic' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_ABsic(var) (&var)

/* Access member 'ABsicFirst' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_ABsicFirst(var) var

/* Access member 'ABsicFirst' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_ABsicFirst(var) (&var)

/* Access member 'CbAcc2' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_AddedFrqListStruct_ACSelPar_data' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_AddedFrqListStruct_ACSelPar_data' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data_TmpOffset(var) (&var)

/* Access member 'data' of type 'c_AddedFrqListStruct_ACSelPar' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar_data(var) var

/* Access member 'data' of type 'c_AddedFrqListStruct_ACSelPar' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar_data(var) (&var)

/* Access member 'ACSelPar' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelPar(var) var

/* Access member 'ACSelPar' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelPar(var) (&var)

/* Access member 'CbAcc2' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_AddedFrqListStruct_ACSelParFirst' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_AddedFrqListStruct_ACSelParFirst' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst_TmpOffset(var) (&var)

/* Access member 'ACSelParFirst' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_ACSelParFirst(var) var

/* Access member 'ACSelParFirst' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_ACSelParFirst(var) (&var)

/* Access member 'data' of type 'c_AddedFrqListStruct_AFrqDiff' as a variable reference */
#define VAR_c_AddedFrqListStruct_AFrqDiff_data(var) var

/* Access member 'data' of type 'c_AddedFrqListStruct_AFrqDiff' as a pointer */
#define PTR_c_AddedFrqListStruct_AFrqDiff_data(var) (&var)

/* Access member 'AFrqDiff' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_AFrqDiff(var) var

/* Access member 'AFrqDiff' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_AFrqDiff(var) (&var)

/* Access member 'AFrqDiffLen' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_AFrqDiffLen(var) var

/* Access member 'AFrqDiffLen' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_AFrqDiffLen(var) (&var)

/* Access member 'AFrqFirst' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_AFrqFirst(var) var

/* Access member 'AFrqFirst' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_AFrqFirst(var) (&var)

/* Access member 'NbAFrq' of type 'c_AddedFrqListStruct' as a variable reference */
#define VAR_c_AddedFrqListStruct_NbAFrq(var) var

/* Access member 'NbAFrq' of type 'c_AddedFrqListStruct' as a pointer */
#define PTR_c_AddedFrqListStruct_NbAFrq(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_AddedFrqListStruct_ABsic */
typedef struct _c_AddedFrqListStruct_ABsic {
	ED_OCTET data [32];
	int items;
} c_AddedFrqListStruct_ABsic;

/* DEFINITION OF SUB-STRUCTURE c_AddedFrqListStruct_ACSelPar_data */
typedef struct _c_AddedFrqListStruct_ACSelPar_data {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_AddedFrqListStruct_ACSelPar_data;

/* DEFINITION OF SUB-STRUCTURE c_AddedFrqListStruct_ACSelPar */
typedef struct _c_AddedFrqListStruct_ACSelPar {
	c_AddedFrqListStruct_ACSelPar_data data [32];
	int items;
} c_AddedFrqListStruct_ACSelPar;

/* DEFINITION OF SUB-STRUCTURE c_AddedFrqListStruct_ACSelParFirst */
typedef struct _c_AddedFrqListStruct_ACSelParFirst {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_AddedFrqListStruct_ACSelParFirst;

/* DEFINITION OF BINARY c_AddedFrqListStruct_AFrqDiff_data */
typedef struct _c_AddedFrqListStruct_AFrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_AddedFrqListStruct_AFrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_AddedFrqListStruct_AFrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_AddedFrqListStruct_AFrqDiff */
typedef struct _c_AddedFrqListStruct_AFrqDiff {
	c_AddedFrqListStruct_AFrqDiff_data data [32]; /* Static, variable size; bits needed 64 */
	int items;
} c_AddedFrqListStruct_AFrqDiff;


/*-----------------------------------*/
typedef struct _c_AddedFrqListStruct {
	ED_SHORT AFrqFirst;
	ED_OCTET ABsicFirst;
	ED_BOOLEAN ACSelParFirst_Present;
	ED_OCTET AFrqDiffLen;
	ED_OCTET NbAFrq;
	c_AddedFrqListStruct_ABsic ABsic;
	c_AddedFrqListStruct_ACSelPar ACSelPar;
	c_AddedFrqListStruct_ACSelParFirst ACSelParFirst;
	c_AddedFrqListStruct_AFrqDiff AFrqDiff;

}	c_AddedFrqListStruct;
#define INIT_c_AddedFrqListStruct(sp) ED_RESET_MEM ((sp), sizeof (c_AddedFrqListStruct))
#define FREE_c_AddedFrqListStruct(sp)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst(sp,present) ((sp)->ACSelParFirst_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst(sp) ((sp)->ACSelParFirst_Present)

/* Access member 'CbAcc2' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_CellSelStruct' as a variable reference */
#define VAR_c_CellSelStruct_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_CellSelStruct' as a pointer */
#define PTR_c_CellSelStruct_TmpOffset(var) (&var)


/*-----------------------------------*/
typedef struct _c_CellSelStruct {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

}	c_CellSelStruct;
#define INIT_c_CellSelStruct(sp) ED_RESET_MEM ((sp), sizeof (c_CellSelStruct))
#define FREE_c_CellSelStruct(sp)
#define SETPRESENT_c_CellSelStruct_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_CellSelStruct_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_CellSelStruct_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_CellSelStruct_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_CellSelStruct_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_CellSelStruct_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_CellSelStruct_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_CellSelStruct_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_CellSelStruct_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_CellSelStruct_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_CellSelStruct_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_CellSelStruct_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_CellSelStruct_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_CellSelStruct_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_CellSelStruct_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_CellSelStruct_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_CellSelStruct_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_CellSelStruct_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_CellSelStruct_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_CellSelStruct_TmpOffset(sp) ((sp)->TmpOffset_Present)

/* Access member 'data' of type 'c_ExtMeasParamStruct_ExtFrqList_FrqDiff' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data(var) var

/* Access member 'data' of type 'c_ExtMeasParamStruct_ExtFrqList_FrqDiff' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data(var) (&var)

/* Access member 'FrqDiff' of type 'c_ExtMeasParamStruct_ExtFrqList' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList_FrqDiff(var) var

/* Access member 'FrqDiff' of type 'c_ExtMeasParamStruct_ExtFrqList' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList_FrqDiff(var) (&var)

/* Access member 'FrqDiffLen' of type 'c_ExtMeasParamStruct_ExtFrqList' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList_FrqDiffLen(var) var

/* Access member 'FrqDiffLen' of type 'c_ExtMeasParamStruct_ExtFrqList' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList_FrqDiffLen(var) (&var)

/* Access member 'FrqFirst' of type 'c_ExtMeasParamStruct_ExtFrqList' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList_FrqFirst(var) var

/* Access member 'FrqFirst' of type 'c_ExtMeasParamStruct_ExtFrqList' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList_FrqFirst(var) (&var)

/* Access member 'NbFrq' of type 'c_ExtMeasParamStruct_ExtFrqList' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList_NbFrq(var) var

/* Access member 'NbFrq' of type 'c_ExtMeasParamStruct_ExtFrqList' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList_NbFrq(var) (&var)

/* Access member 'ExtFrqList' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtFrqList(var) var

/* Access member 'ExtFrqList' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtFrqList(var) (&var)

/* Access member 'ExtMeasOrder' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtMeasOrder(var) var

/* Access member 'ExtMeasOrder' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtMeasOrder(var) (&var)

/* Access member 'ExtRepPer' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtRepPer(var) var

/* Access member 'ExtRepPer' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtRepPer(var) (&var)

/* Access member 'ExtRepType' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_ExtRepType(var) var

/* Access member 'ExtRepType' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_ExtRepType(var) (&var)

/* Access member 'IntFrq' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_IntFrq(var) var

/* Access member 'IntFrq' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_IntFrq(var) (&var)

/* Access member 'NccPerm' of type 'c_ExtMeasParamStruct' as a variable reference */
#define VAR_c_ExtMeasParamStruct_NccPerm(var) var

/* Access member 'NccPerm' of type 'c_ExtMeasParamStruct' as a pointer */
#define PTR_c_ExtMeasParamStruct_NccPerm(var) (&var)

/* DEFINITION OF BINARY c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data */
typedef struct _c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_ExtMeasParamStruct_ExtFrqList_FrqDiff */
typedef struct _c_ExtMeasParamStruct_ExtFrqList_FrqDiff {
	c_ExtMeasParamStruct_ExtFrqList_FrqDiff_data data [20]; /* Static, variable size; bits needed 64 */
	int items;
} c_ExtMeasParamStruct_ExtFrqList_FrqDiff;

/* DEFINITION OF SUB-STRUCTURE c_ExtMeasParamStruct_ExtFrqList */
typedef struct _c_ExtMeasParamStruct_ExtFrqList {
	ED_SHORT FrqFirst;
	ED_OCTET FrqDiffLen;
	ED_OCTET NbFrq;
	c_ExtMeasParamStruct_ExtFrqList_FrqDiff FrqDiff;

} c_ExtMeasParamStruct_ExtFrqList;


/*-----------------------------------*/
typedef struct _c_ExtMeasParamStruct {
	ED_BOOLEAN ExtFrqList_Present;
	ED_OCTET ExtMeasOrder;
	ED_OCTET ExtRepPer;
	ED_BOOLEAN ExtRepPer_Present;
	ED_OCTET ExtRepType;
	ED_BOOLEAN ExtRepType_Present;
	ED_OCTET IntFrq;
	ED_BOOLEAN IntFrq_Present;
	ED_OCTET NccPerm;
	ED_BOOLEAN NccPerm_Present;
	c_ExtMeasParamStruct_ExtFrqList ExtFrqList;

}	c_ExtMeasParamStruct;
#define INIT_c_ExtMeasParamStruct(sp) ED_RESET_MEM ((sp), sizeof (c_ExtMeasParamStruct))
#define FREE_c_ExtMeasParamStruct(sp)
#define SETPRESENT_c_ExtMeasParamStruct_ExtFrqList(sp,present) ((sp)->ExtFrqList_Present = present)
#define GETPRESENT_c_ExtMeasParamStruct_ExtFrqList(sp) ((sp)->ExtFrqList_Present)
#define SETPRESENT_c_ExtMeasParamStruct_ExtRepPer(sp,present) ((sp)->ExtRepPer_Present = present)
#define GETPRESENT_c_ExtMeasParamStruct_ExtRepPer(sp) ((sp)->ExtRepPer_Present)
#define SETPRESENT_c_ExtMeasParamStruct_ExtRepType(sp,present) ((sp)->ExtRepType_Present = present)
#define GETPRESENT_c_ExtMeasParamStruct_ExtRepType(sp) ((sp)->ExtRepType_Present)
#define SETPRESENT_c_ExtMeasParamStruct_IntFrq(sp,present) ((sp)->IntFrq_Present = present)
#define GETPRESENT_c_ExtMeasParamStruct_IntFrq(sp) ((sp)->IntFrq_Present)
#define SETPRESENT_c_ExtMeasParamStruct_NccPerm(sp,present) ((sp)->NccPerm_Present = present)
#define GETPRESENT_c_ExtMeasParamStruct_NccPerm(sp) ((sp)->NccPerm_Present)

/* Access member 'data' of type 'c_ExtFrqListDesc_FrqDiff' as a variable reference */
#define VAR_c_ExtFrqListDesc_FrqDiff_data(var) var

/* Access member 'data' of type 'c_ExtFrqListDesc_FrqDiff' as a pointer */
#define PTR_c_ExtFrqListDesc_FrqDiff_data(var) (&var)

/* Access member 'FrqDiff' of type 'c_ExtFrqListDesc' as a variable reference */
#define VAR_c_ExtFrqListDesc_FrqDiff(var) var

/* Access member 'FrqDiff' of type 'c_ExtFrqListDesc' as a pointer */
#define PTR_c_ExtFrqListDesc_FrqDiff(var) (&var)

/* Access member 'FrqDiffLen' of type 'c_ExtFrqListDesc' as a variable reference */
#define VAR_c_ExtFrqListDesc_FrqDiffLen(var) var

/* Access member 'FrqDiffLen' of type 'c_ExtFrqListDesc' as a pointer */
#define PTR_c_ExtFrqListDesc_FrqDiffLen(var) (&var)

/* Access member 'FrqFirst' of type 'c_ExtFrqListDesc' as a variable reference */
#define VAR_c_ExtFrqListDesc_FrqFirst(var) var

/* Access member 'FrqFirst' of type 'c_ExtFrqListDesc' as a pointer */
#define PTR_c_ExtFrqListDesc_FrqFirst(var) (&var)

/* Access member 'NbFrq' of type 'c_ExtFrqListDesc' as a variable reference */
#define VAR_c_ExtFrqListDesc_NbFrq(var) var

/* Access member 'NbFrq' of type 'c_ExtFrqListDesc' as a pointer */
#define PTR_c_ExtFrqListDesc_NbFrq(var) (&var)

/* DEFINITION OF BINARY c_ExtFrqListDesc_FrqDiff_data */
typedef struct _c_ExtFrqListDesc_FrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_ExtFrqListDesc_FrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_ExtFrqListDesc_FrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_ExtFrqListDesc_FrqDiff */
typedef struct _c_ExtFrqListDesc_FrqDiff {
	c_ExtFrqListDesc_FrqDiff_data data [20]; /* Static, variable size; bits needed 64 */
	int items;
} c_ExtFrqListDesc_FrqDiff;


/*-----------------------------------*/
typedef struct _c_ExtFrqListDesc {
	ED_SHORT FrqFirst;
	ED_OCTET FrqDiffLen;
	ED_OCTET NbFrq;
	c_ExtFrqListDesc_FrqDiff FrqDiff;

}	c_ExtFrqListDesc;
#define INIT_c_ExtFrqListDesc(sp) ED_RESET_MEM ((sp), sizeof (c_ExtFrqListDesc))
#define FREE_c_ExtFrqListDesc(sp)

/* Access member '_3GNCellL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg__3GNCellL(var) var

/* Access member '_3GNCellL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg__3GNCellL(var) (&var)

/* Access member 'Arfcn' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_Arfcn(var) var

/* Access member 'Arfcn' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_Arfcn(var) (&var)

/* Access member 'BaInd' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_BaInd(var) var

/* Access member 'BaInd' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_BaInd(var) (&var)

/* Access member 'Bsic' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_Bsic(var) var

/* Access member 'Bsic' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_Bsic(var) (&var)

/* Access member 'BaIdxStartBsic' of type 'c_MeasParamMsg_BsicL' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_BaIdxStartBsic(var) var

/* Access member 'BaIdxStartBsic' of type 'c_MeasParamMsg_BsicL' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_BaIdxStartBsic(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_BsicL_Bsic' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_Bsic_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_BsicL_Bsic' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_Bsic_data(var) (&var)

/* Access member 'Bsic' of type 'c_MeasParamMsg_BsicL' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_Bsic(var) var

/* Access member 'Bsic' of type 'c_MeasParamMsg_BsicL' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_Bsic(var) (&var)

/* Access member 'BsicFirst' of type 'c_MeasParamMsg_BsicL' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_BsicFirst(var) var

/* Access member 'BsicFirst' of type 'c_MeasParamMsg_BsicL' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_BsicFirst(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_BsicL_FreqScrol' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_FreqScrol_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_BsicL_FreqScrol' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_FreqScrol_data(var) (&var)

/* Access member 'FreqScrol' of type 'c_MeasParamMsg_BsicL' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_FreqScrol(var) var

/* Access member 'FreqScrol' of type 'c_MeasParamMsg_BsicL' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_FreqScrol(var) (&var)

/* Access member 'NbRemainBsic' of type 'c_MeasParamMsg_BsicL' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL_NbRemainBsic(var) var

/* Access member 'NbRemainBsic' of type 'c_MeasParamMsg_BsicL' as a pointer */
#define PTR_c_MeasParamMsg_BsicL_NbRemainBsic(var) (&var)

/* Access member 'BsicL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_BsicL(var) var

/* Access member 'BsicL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_BsicL(var) (&var)

/* Access member 'C2000CellL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_C2000CellL(var) var

/* Access member 'C2000CellL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_C2000CellL(var) (&var)

/* Access member 'ChgeMark' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_ChgeMark(var) var

/* Access member 'ChgeMark' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_ChgeMark(var) (&var)

/* Access member 'Count' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_Count(var) var

/* Access member 'Count' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_Count(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data(var) (&var)

/* Access member 'FrqDiff' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff(var) var

/* Access member 'FrqDiff' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff(var) (&var)

/* Access member 'FrqDiffLen' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiffLen(var) var

/* Access member 'FrqDiffLen' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiffLen(var) (&var)

/* Access member 'FrqFirst' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqFirst(var) var

/* Access member 'FrqFirst' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqFirst(var) (&var)

/* Access member 'NbFrq' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_NbFrq(var) var

/* Access member 'NbFrq' of type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList_NbFrq(var) (&var)

/* Access member 'ExtFrqList' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtFrqList(var) var

/* Access member 'ExtFrqList' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtFrqList(var) (&var)

/* Access member 'ExtMeasOrder' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtMeasOrder(var) var

/* Access member 'ExtMeasOrder' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtMeasOrder(var) (&var)

/* Access member 'ExtRepPer' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtRepPer(var) var

/* Access member 'ExtRepPer' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtRepPer(var) (&var)

/* Access member 'ExtRepType' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_ExtRepType(var) var

/* Access member 'ExtRepType' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_ExtRepType(var) (&var)

/* Access member 'IntFrq' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_IntFrq(var) var

/* Access member 'IntFrq' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_IntFrq(var) (&var)

/* Access member 'NccPerm' of type 'c_MeasParamMsg_ExtMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar_NccPerm(var) var

/* Access member 'NccPerm' of type 'c_MeasParamMsg_ExtMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar_NccPerm(var) (&var)

/* Access member 'ExtMeasPar' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_ExtMeasPar(var) var

/* Access member 'ExtMeasPar' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_ExtMeasPar(var) (&var)

/* Access member 'Idx' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_Idx(var) var

/* Access member 'Idx' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_Idx(var) (&var)

/* Access member 'ImmRel' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_ImmRel(var) var

/* Access member 'ImmRel' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_ImmRel(var) (&var)

/* Access member 'InvBsicRep' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_InvBsicRep(var) var

/* Access member 'InvBsicRep' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_InvBsicRep(var) (&var)

/* Access member 'RepTyp' of type 'c_MeasParamMsg_MeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_MeasPar_RepTyp(var) var

/* Access member 'RepTyp' of type 'c_MeasParamMsg_MeasPar' as a pointer */
#define PTR_c_MeasParamMsg_MeasPar_RepTyp(var) (&var)

/* Access member 'ServBandRep' of type 'c_MeasParamMsg_MeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_MeasPar_ServBandRep(var) var

/* Access member 'ServBandRep' of type 'c_MeasParamMsg_MeasPar' as a pointer */
#define PTR_c_MeasParamMsg_MeasPar_ServBandRep(var) (&var)

/* Access member 'MeasPar' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_MeasPar(var) var

/* Access member 'MeasPar' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_MeasPar(var) (&var)

/* Access member 'MultiBandRep' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_MultiBandRep(var) var

/* Access member 'MultiBandRep' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_MultiBandRep(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ABsic' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ABsic_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ABsic' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ABsic_data(var) (&var)

/* Access member 'ABsic' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ABsic(var) var

/* Access member 'ABsic' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ABsic(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ABsicFirst' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ABsicFirst_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ABsicFirst' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ABsicFirst_data(var) (&var)

/* Access member 'ABsicFirst' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ABsicFirst(var) var

/* Access member 'ABsicFirst' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ABsicFirst(var) (&var)

/* Access member 'CbAcc2' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelPar_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data_TmpOffset(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ACSelPar' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ACSelPar' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar_data(var) (&var)

/* Access member 'ACSelPar' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelPar(var) var

/* Access member 'ACSelPar' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelPar(var) (&var)

/* Access member 'CbAcc2' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_CbAcc2(var) var

/* Access member 'CbAcc2' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_CbAcc2(var) (&var)

/* Access member 'ExcAcc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_ExcAcc(var) var

/* Access member 'ExcAcc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_ExcAcc(var) (&var)

/* Access member 'HcsThr' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_HcsThr(var) var

/* Access member 'HcsThr' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_HcsThr(var) (&var)

/* Access member 'MsTxPMaxCcch' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_MsTxPMaxCcch(var) var

/* Access member 'MsTxPMaxCcch' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_MsTxPMaxCcch(var) (&var)

/* Access member 'PSi1RepPeriod' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PSi1RepPeriod(var) var

/* Access member 'PSi1RepPeriod' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PSi1RepPeriod(var) (&var)

/* Access member 'PbcchLoc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PbcchLoc(var) var

/* Access member 'PbcchLoc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PbcchLoc(var) (&var)

/* Access member 'PenaltyTime' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PenaltyTime(var) var

/* Access member 'PenaltyTime' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PenaltyTime(var) (&var)

/* Access member 'PrioClass' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PrioClass(var) var

/* Access member 'PrioClass' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PrioClass(var) (&var)

/* Access member 'RslOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RslOffset(var) var

/* Access member 'RslOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RslOffset(var) (&var)

/* Access member 'RxLevAccMin' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RxLevAccMin(var) var

/* Access member 'RxLevAccMin' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RxLevAccMin(var) (&var)

/* Access member 'SameRaAsServCell' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_SameRaAsServCell(var) var

/* Access member 'SameRaAsServCell' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_SameRaAsServCell(var) (&var)

/* Access member 'Si13Loc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_Si13Loc(var) var

/* Access member 'Si13Loc' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_Si13Loc(var) (&var)

/* Access member 'TmpOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_TmpOffset(var) var

/* Access member 'TmpOffset' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst_data' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_TmpOffset(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_ACSelParFirst' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst_data(var) (&var)

/* Access member 'ACSelParFirst' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_ACSelParFirst(var) var

/* Access member 'ACSelParFirst' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_ACSelParFirst(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqDiff' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqDiff_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqDiff' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqDiff_data(var) (&var)

/* Access member 'AFrqDiff' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqDiff(var) var

/* Access member 'AFrqDiff' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqDiff(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqDiffLen' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqDiffLen_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqDiffLen' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqDiffLen_data(var) (&var)

/* Access member 'AFrqDiffLen' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqDiffLen(var) var

/* Access member 'AFrqDiffLen' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqDiffLen(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqFirst' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqFirst_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_AFrqFirst' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqFirst_data(var) (&var)

/* Access member 'AFrqFirst' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_AFrqFirst(var) var

/* Access member 'AFrqFirst' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_AFrqFirst(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_NbAFrq' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_NbAFrq_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_NbAFrq' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_NbAFrq_data(var) (&var)

/* Access member 'NbAFrq' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_NbAFrq(var) var

/* Access member 'NbAFrq' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_NbAFrq(var) (&var)

/* Access member 'NbRFrq' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_NbRFrq(var) var

/* Access member 'NbRFrq' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_NbRFrq(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_RFrqIdx' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_RFrqIdx_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_NcFrqL_RFrqIdx' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_RFrqIdx_data(var) (&var)

/* Access member 'RFrqIdx' of type 'c_MeasParamMsg_NcFrqL' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL_RFrqIdx(var) var

/* Access member 'RFrqIdx' of type 'c_MeasParamMsg_NcFrqL' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL_RFrqIdx(var) (&var)

/* Access member 'NcFrqL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_NcFrqL(var) var

/* Access member 'NcFrqL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_NcFrqL(var) (&var)

/* Access member 'NcNonDrxPer' of type 'c_MeasParamMsg_NcMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_NcMeasPar_NcNonDrxPer(var) var

/* Access member 'NcNonDrxPer' of type 'c_MeasParamMsg_NcMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_NcMeasPar_NcNonDrxPer(var) (&var)

/* Access member 'NcRepPerI' of type 'c_MeasParamMsg_NcMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_NcMeasPar_NcRepPerI(var) var

/* Access member 'NcRepPerI' of type 'c_MeasParamMsg_NcMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_NcMeasPar_NcRepPerI(var) (&var)

/* Access member 'NcRepPerT' of type 'c_MeasParamMsg_NcMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_NcMeasPar_NcRepPerT(var) var

/* Access member 'NcRepPerT' of type 'c_MeasParamMsg_NcMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_NcMeasPar_NcRepPerT(var) (&var)

/* Access member 'NwkCtrlOrder' of type 'c_MeasParamMsg_NcMeasPar' as a variable reference */
#define VAR_c_MeasParamMsg_NcMeasPar_NwkCtrlOrder(var) var

/* Access member 'NwkCtrlOrder' of type 'c_MeasParamMsg_NcMeasPar' as a pointer */
#define PTR_c_MeasParamMsg_NcMeasPar_NwkCtrlOrder(var) (&var)

/* Access member 'NcMeasPar' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_NcMeasPar(var) var

/* Access member 'NcMeasPar' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_NcMeasPar(var) (&var)

/* Access member 'NccPerm' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_NccPerm(var) var

/* Access member 'NccPerm' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_NccPerm(var) (&var)

/* Access member 'PmoInd' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_PmoInd(var) var

/* Access member 'PmoInd' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_PmoInd(var) (&var)

/* Access member 'NbCells' of type 'c_MeasParamMsg_PrioL' as a variable reference */
#define VAR_c_MeasParamMsg_PrioL_NbCells(var) var

/* Access member 'NbCells' of type 'c_MeasParamMsg_PrioL' as a pointer */
#define PTR_c_MeasParamMsg_PrioL_NbCells(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_PrioL_RepPrio' as a variable reference */
#define VAR_c_MeasParamMsg_PrioL_RepPrio_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_PrioL_RepPrio' as a pointer */
#define PTR_c_MeasParamMsg_PrioL_RepPrio_data(var) (&var)

/* Access member 'RepPrio' of type 'c_MeasParamMsg_PrioL' as a variable reference */
#define VAR_c_MeasParamMsg_PrioL_RepPrio(var) var

/* Access member 'RepPrio' of type 'c_MeasParamMsg_PrioL' as a pointer */
#define PTR_c_MeasParamMsg_PrioL_RepPrio(var) (&var)

/* Access member 'PrioL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_PrioL(var) var

/* Access member 'PrioL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_PrioL(var) (&var)

/* Access member 'Pwrc' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_Pwrc(var) var

/* Access member 'Pwrc' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_Pwrc(var) (&var)

/* Access member 'R3GCellL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_R3GCellL(var) var

/* Access member 'R3GCellL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_R3GCellL(var) (&var)

/* Access member 'RepOff1800' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepOff1800(var) var

/* Access member 'RepOff1800' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepOff1800(var) (&var)

/* Access member 'RepOff1900' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepOff1900(var) var

/* Access member 'RepOff1900' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepOff1900(var) (&var)

/* Access member 'RepOff400' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepOff400(var) var

/* Access member 'RepOff400' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepOff400(var) (&var)

/* Access member 'RepOff850' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepOff850(var) var

/* Access member 'RepOff850' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepOff850(var) (&var)

/* Access member 'RepOff900' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepOff900(var) var

/* Access member 'RepOff900' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepOff900(var) (&var)

/* Access member 'RepRate' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepRate(var) var

/* Access member 'RepRate' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepRate(var) (&var)

/* Access member 'RepThr1800' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepThr1800(var) var

/* Access member 'RepThr1800' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepThr1800(var) (&var)

/* Access member 'RepThr1900' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepThr1900(var) var

/* Access member 'RepThr1900' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepThr1900(var) (&var)

/* Access member 'RepThr400' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepThr400(var) var

/* Access member 'RepThr400' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepThr400(var) (&var)

/* Access member 'RepThr850' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepThr850(var) var

/* Access member 'RepThr850' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepThr850(var) (&var)

/* Access member 'RepThr900' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepThr900(var) var

/* Access member 'RepThr900' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepThr900(var) (&var)

/* Access member 'RepTyp' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RepTyp(var) var

/* Access member 'RepTyp' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RepTyp(var) (&var)

/* Access member 'IdxStartRtd12' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_IdxStartRtd12(var) var

/* Access member 'IdxStartRtd12' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_IdxStartRtd12(var) (&var)

/* Access member 'IdxStartRtd6' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_IdxStartRtd6(var) var

/* Access member 'IdxStartRtd6' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_IdxStartRtd6(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12_data(var) (&var)

/* Access member 'Rtd12' of type 'c_MeasParamMsg_RtdL_Rtd12L_data' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12(var) var

/* Access member 'Rtd12' of type 'c_MeasParamMsg_RtdL_Rtd12L_data' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd12L' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd12L_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd12L' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd12L_data(var) (&var)

/* Access member 'Rtd12L' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd12L(var) var

/* Access member 'Rtd12L' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd12L(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6_data(var) (&var)

/* Access member 'Rtd6' of type 'c_MeasParamMsg_RtdL_Rtd6L_data' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6(var) var

/* Access member 'Rtd6' of type 'c_MeasParamMsg_RtdL_Rtd6L_data' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd6L' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd6L_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_Rtd6L' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd6L_data(var) (&var)

/* Access member 'Rtd6L' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_Rtd6L(var) var

/* Access member 'Rtd6L' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_Rtd6L(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_StartRtd12L_Rtd12' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_StartRtd12L_Rtd12' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12_data(var) (&var)

/* Access member 'Rtd12' of type 'c_MeasParamMsg_RtdL_StartRtd12L' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12(var) var

/* Access member 'Rtd12' of type 'c_MeasParamMsg_RtdL_StartRtd12L' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12(var) (&var)

/* Access member 'StartRtd12L' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd12L(var) var

/* Access member 'StartRtd12L' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd12L(var) (&var)

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_StartRtd6L_Rtd6' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6_data(var) var

/* Access member 'data' of type 'c_MeasParamMsg_RtdL_StartRtd6L_Rtd6' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6_data(var) (&var)

/* Access member 'Rtd6' of type 'c_MeasParamMsg_RtdL_StartRtd6L' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6(var) var

/* Access member 'Rtd6' of type 'c_MeasParamMsg_RtdL_StartRtd6L' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6(var) (&var)

/* Access member 'StartRtd6L' of type 'c_MeasParamMsg_RtdL' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL_StartRtd6L(var) var

/* Access member 'StartRtd6L' of type 'c_MeasParamMsg_RtdL' as a pointer */
#define PTR_c_MeasParamMsg_RtdL_StartRtd6L(var) (&var)

/* Access member 'RtdL' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_RtdL(var) var

/* Access member 'RtdL' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_RtdL(var) (&var)

/* Access member 'ScaleOrd' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_ScaleOrd(var) var

/* Access member 'ScaleOrd' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_ScaleOrd(var) (&var)

/* Access member 'ServBandRep' of type 'c_MeasParamMsg' as a variable reference */
#define VAR_c_MeasParamMsg_ServBandRep(var) var

/* Access member 'ServBandRep' of type 'c_MeasParamMsg' as a pointer */
#define PTR_c_MeasParamMsg_ServBandRep(var) (&var)

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_BsicL_Bsic */
typedef struct _c_MeasParamMsg_BsicL_Bsic {
	ED_OCTET data [20];
	int items;
} c_MeasParamMsg_BsicL_Bsic;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_BsicL_FreqScrol */
typedef struct _c_MeasParamMsg_BsicL_FreqScrol {
	ED_BOOLEAN data [20];
	int items;
} c_MeasParamMsg_BsicL_FreqScrol;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_BsicL */
typedef struct _c_MeasParamMsg_BsicL {
	ED_OCTET BaIdxStartBsic;
	ED_BOOLEAN BaIdxStartBsic_Present;
	ED_OCTET BsicFirst;
	ED_OCTET NbRemainBsic;
	c_MeasParamMsg_BsicL_Bsic Bsic;
	c_MeasParamMsg_BsicL_FreqScrol FreqScrol;

} c_MeasParamMsg_BsicL;

/* DEFINITION OF BINARY c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data */
typedef struct _c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff */
typedef struct _c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff {
	c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff_data data [20]; /* Static, variable size; bits needed 64 */
	int items;
} c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_ExtMeasPar_ExtFrqList */
typedef struct _c_MeasParamMsg_ExtMeasPar_ExtFrqList {
	ED_SHORT FrqFirst;
	ED_OCTET FrqDiffLen;
	ED_OCTET NbFrq;
	c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff FrqDiff;

} c_MeasParamMsg_ExtMeasPar_ExtFrqList;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_ExtMeasPar */
typedef struct _c_MeasParamMsg_ExtMeasPar {
	ED_BOOLEAN ExtFrqList_Present;
	ED_OCTET ExtMeasOrder;
	ED_OCTET ExtRepPer;
	ED_BOOLEAN ExtRepPer_Present;
	ED_OCTET ExtRepType;
	ED_BOOLEAN ExtRepType_Present;
	ED_OCTET IntFrq;
	ED_BOOLEAN IntFrq_Present;
	ED_OCTET NccPerm;
	ED_BOOLEAN NccPerm_Present;
	c_MeasParamMsg_ExtMeasPar_ExtFrqList ExtFrqList;

} c_MeasParamMsg_ExtMeasPar;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_MeasPar */
typedef struct _c_MeasParamMsg_MeasPar {
	ED_BOOLEAN RepTyp;
	ED_OCTET ServBandRep;

} c_MeasParamMsg_MeasPar;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ABsic */
typedef struct _c_MeasParamMsg_NcFrqL_ABsic {
	ED_OCTET data [32];
	int items;
} c_MeasParamMsg_NcFrqL_ABsic;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ABsicFirst */
typedef struct _c_MeasParamMsg_NcFrqL_ABsicFirst {
	ED_OCTET data [20];
	int items;
} c_MeasParamMsg_NcFrqL_ABsicFirst;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ACSelPar_data */
typedef struct _c_MeasParamMsg_NcFrqL_ACSelPar_data {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_MeasParamMsg_NcFrqL_ACSelPar_data;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ACSelPar */
typedef struct _c_MeasParamMsg_NcFrqL_ACSelPar {
	c_MeasParamMsg_NcFrqL_ACSelPar_data data [32];
	int items;
} c_MeasParamMsg_NcFrqL_ACSelPar;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ACSelParFirst_data */
typedef struct _c_MeasParamMsg_NcFrqL_ACSelParFirst_data {
	ED_BOOLEAN CbAcc2;
	ED_BOOLEAN ExcAcc;
	ED_OCTET HcsThr;
	ED_BOOLEAN HcsThr_Present;
	ED_OCTET MsTxPMaxCcch;
	ED_BOOLEAN MsTxPMaxCcch_Present;
	ED_OCTET PSi1RepPeriod;
	ED_BOOLEAN PSi1RepPeriod_Present;
	ED_OCTET PbcchLoc;
	ED_BOOLEAN PbcchLoc_Present;
	ED_OCTET PenaltyTime;
	ED_BOOLEAN PenaltyTime_Present;
	ED_OCTET PrioClass;
	ED_BOOLEAN PrioClass_Present;
	ED_OCTET RslOffset;
	ED_BOOLEAN RslOffset_Present;
	ED_OCTET RxLevAccMin;
	ED_BOOLEAN RxLevAccMin_Present;
	ED_BOOLEAN SameRaAsServCell;
	ED_BOOLEAN Si13Loc;
	ED_BOOLEAN Si13Loc_Present;
	ED_OCTET TmpOffset;
	ED_BOOLEAN TmpOffset_Present;

} c_MeasParamMsg_NcFrqL_ACSelParFirst_data;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_ACSelParFirst */
typedef struct _c_MeasParamMsg_NcFrqL_ACSelParFirst {
	c_MeasParamMsg_NcFrqL_ACSelParFirst_data data [20];
	int items;
} c_MeasParamMsg_NcFrqL_ACSelParFirst;

/* DEFINITION OF BINARY c_MeasParamMsg_NcFrqL_AFrqDiff_data */
typedef struct _c_MeasParamMsg_NcFrqL_AFrqDiff_data {
	ED_BYTE value [8]; /* Variable size; bits needed 64 */
	int usedBits;
} c_MeasParamMsg_NcFrqL_AFrqDiff_data;
/* Binary allocation macro (static version - does nothing) */
#define ALLOC_c_MeasParamMsg_NcFrqL_AFrqDiff_data(sp,bits) (sp)->usedBits = 0


/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_AFrqDiff */
typedef struct _c_MeasParamMsg_NcFrqL_AFrqDiff {
	c_MeasParamMsg_NcFrqL_AFrqDiff_data data [32]; /* Static, variable size; bits needed 64 */
	int items;
} c_MeasParamMsg_NcFrqL_AFrqDiff;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_AFrqDiffLen */
typedef struct _c_MeasParamMsg_NcFrqL_AFrqDiffLen {
	ED_OCTET data [20];
	int items;
} c_MeasParamMsg_NcFrqL_AFrqDiffLen;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_AFrqFirst */
typedef struct _c_MeasParamMsg_NcFrqL_AFrqFirst {
	ED_SHORT data [20];
	int items;
} c_MeasParamMsg_NcFrqL_AFrqFirst;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_NbAFrq */
typedef struct _c_MeasParamMsg_NcFrqL_NbAFrq {
	ED_OCTET data [20];
	int items;
} c_MeasParamMsg_NcFrqL_NbAFrq;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL_RFrqIdx */
typedef struct _c_MeasParamMsg_NcFrqL_RFrqIdx {
	ED_OCTET data [32];
	int items;
} c_MeasParamMsg_NcFrqL_RFrqIdx;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcFrqL */
typedef struct _c_MeasParamMsg_NcFrqL {
	ED_OCTET NbRFrq;
	ED_BOOLEAN NbRFrq_Present;
	c_MeasParamMsg_NcFrqL_ABsic ABsic;
	c_MeasParamMsg_NcFrqL_ABsicFirst ABsicFirst;
	c_MeasParamMsg_NcFrqL_ACSelPar ACSelPar;
	c_MeasParamMsg_NcFrqL_ACSelParFirst ACSelParFirst;
	c_MeasParamMsg_NcFrqL_AFrqDiff AFrqDiff;
	c_MeasParamMsg_NcFrqL_AFrqDiffLen AFrqDiffLen;
	c_MeasParamMsg_NcFrqL_AFrqFirst AFrqFirst;
	c_MeasParamMsg_NcFrqL_NbAFrq NbAFrq;
	c_MeasParamMsg_NcFrqL_RFrqIdx RFrqIdx;

} c_MeasParamMsg_NcFrqL;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_NcMeasPar */
typedef struct _c_MeasParamMsg_NcMeasPar {
	ED_OCTET NcNonDrxPer;
	ED_BOOLEAN NcNonDrxPer_Present;
	ED_OCTET NcRepPerI;
	ED_BOOLEAN NcRepPerI_Present;
	ED_OCTET NcRepPerT;
	ED_BOOLEAN NcRepPerT_Present;
	ED_OCTET NwkCtrlOrder;

} c_MeasParamMsg_NcMeasPar;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_PrioL_RepPrio */
typedef struct _c_MeasParamMsg_PrioL_RepPrio {
	ED_BOOLEAN data [96];
	int items;
} c_MeasParamMsg_PrioL_RepPrio;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_PrioL */
typedef struct _c_MeasParamMsg_PrioL {
	ED_OCTET NbCells;
	c_MeasParamMsg_PrioL_RepPrio RepPrio;

} c_MeasParamMsg_PrioL;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12 */
typedef struct _c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12 {
	ED_SHORT data [3];
	int items;
} c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd12L_data */
typedef struct _c_MeasParamMsg_RtdL_Rtd12L_data {
	c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12 Rtd12;

} c_MeasParamMsg_RtdL_Rtd12L_data;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd12L */
typedef struct _c_MeasParamMsg_RtdL_Rtd12L {
	c_MeasParamMsg_RtdL_Rtd12L_data data [31];
	int items;
} c_MeasParamMsg_RtdL_Rtd12L;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6 */
typedef struct _c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6 {
	ED_OCTET data [3];
	int items;
} c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd6L_data */
typedef struct _c_MeasParamMsg_RtdL_Rtd6L_data {
	c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6 Rtd6;

} c_MeasParamMsg_RtdL_Rtd6L_data;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_Rtd6L */
typedef struct _c_MeasParamMsg_RtdL_Rtd6L {
	c_MeasParamMsg_RtdL_Rtd6L_data data [31];
	int items;
} c_MeasParamMsg_RtdL_Rtd6L;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_StartRtd12L_Rtd12 */
typedef struct _c_MeasParamMsg_RtdL_StartRtd12L_Rtd12 {
	ED_SHORT data [3];
	int items;
} c_MeasParamMsg_RtdL_StartRtd12L_Rtd12;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_StartRtd12L */
typedef struct _c_MeasParamMsg_RtdL_StartRtd12L {
	c_MeasParamMsg_RtdL_StartRtd12L_Rtd12 Rtd12;

} c_MeasParamMsg_RtdL_StartRtd12L;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_StartRtd6L_Rtd6 */
typedef struct _c_MeasParamMsg_RtdL_StartRtd6L_Rtd6 {
	ED_OCTET data [3];
	int items;
} c_MeasParamMsg_RtdL_StartRtd6L_Rtd6;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL_StartRtd6L */
typedef struct _c_MeasParamMsg_RtdL_StartRtd6L {
	c_MeasParamMsg_RtdL_StartRtd6L_Rtd6 Rtd6;

} c_MeasParamMsg_RtdL_StartRtd6L;

/* DEFINITION OF SUB-STRUCTURE c_MeasParamMsg_RtdL */
typedef struct _c_MeasParamMsg_RtdL {
	ED_OCTET IdxStartRtd12;
	ED_BOOLEAN IdxStartRtd12_Present;
	ED_OCTET IdxStartRtd6;
	ED_BOOLEAN IdxStartRtd6_Present;
	ED_BOOLEAN StartRtd12L_Present;
	ED_BOOLEAN StartRtd6L_Present;
	c_MeasParamMsg_RtdL_Rtd12L Rtd12L;
	c_MeasParamMsg_RtdL_Rtd6L Rtd6L;
	c_MeasParamMsg_RtdL_StartRtd12L StartRtd12L;
	c_MeasParamMsg_RtdL_StartRtd6L StartRtd6L;

} c_MeasParamMsg_RtdL;


/*-----------------------------------*/
typedef struct _c_MeasParamMsg {
	ED_BYTE _3GNCellL [8];
	ED_BYTE C2000CellL [8];
	ED_BYTE R3GCellL [8];
	ED_SHORT Arfcn;
	ED_BOOLEAN _3GNCellL_Present;
	ED_BOOLEAN Arfcn_Present;
	ED_BOOLEAN BaInd;
	ED_BOOLEAN BaInd_Present;
	ED_OCTET Bsic;
	ED_BOOLEAN Bsic_Present;
	ED_BOOLEAN BsicL_Present;
	ED_BOOLEAN C2000CellL_Present;
	ED_OCTET ChgeMark;
	ED_BOOLEAN ChgeMark_Present;
	ED_OCTET Count;
	ED_BOOLEAN Count_Present;
	ED_BOOLEAN ExtMeasPar_Present;
	ED_OCTET Idx;
	ED_BOOLEAN Idx_Present;
	ED_BOOLEAN ImmRel;
	ED_BOOLEAN ImmRel_Present;
	ED_BOOLEAN InvBsicRep;
	ED_BOOLEAN InvBsicRep_Present;
	ED_BOOLEAN MeasPar_Present;
	ED_OCTET MultiBandRep;
	ED_BOOLEAN MultiBandRep_Present;
	ED_BOOLEAN NcFrqL_Present;
	ED_BOOLEAN NcMeasPar_Present;
	ED_OCTET NccPerm;
	ED_BOOLEAN NccPerm_Present;
	ED_BOOLEAN PmoInd;
	ED_BOOLEAN PmoInd_Present;
	ED_BOOLEAN PrioL_Present;
	ED_BOOLEAN Pwrc;
	ED_BOOLEAN Pwrc_Present;
	ED_BOOLEAN R3GCellL_Present;
	ED_OCTET RepOff1800;
	ED_BOOLEAN RepOff1800_Present;
	ED_OCTET RepOff1900;
	ED_BOOLEAN RepOff1900_Present;
	ED_OCTET RepOff400;
	ED_BOOLEAN RepOff400_Present;
	ED_OCTET RepOff850;
	ED_BOOLEAN RepOff850_Present;
	ED_OCTET RepOff900;
	ED_BOOLEAN RepOff900_Present;
	ED_BOOLEAN RepRate;
	ED_BOOLEAN RepRate_Present;
	ED_OCTET RepThr1800;
	ED_BOOLEAN RepThr1800_Present;
	ED_OCTET RepThr1900;
	ED_BOOLEAN RepThr1900_Present;
	ED_OCTET RepThr400;
	ED_BOOLEAN RepThr400_Present;
	ED_OCTET RepThr850;
	ED_BOOLEAN RepThr850_Present;
	ED_OCTET RepThr900;
	ED_BOOLEAN RepThr900_Present;
	ED_BOOLEAN RepTyp;
	ED_BOOLEAN RepTyp_Present;
	ED_BOOLEAN RtdL_Present;
	ED_OCTET ScaleOrd;
	ED_BOOLEAN ScaleOrd_Present;
	ED_OCTET ServBandRep;
	ED_BOOLEAN ServBandRep_Present;
	c_MeasParamMsg_BsicL BsicL;
	c_MeasParamMsg_ExtMeasPar ExtMeasPar;
	c_MeasParamMsg_MeasPar MeasPar;
	c_MeasParamMsg_NcFrqL NcFrqL;
	c_MeasParamMsg_NcMeasPar NcMeasPar;
	c_MeasParamMsg_PrioL PrioL;
	c_MeasParamMsg_RtdL RtdL;

}	c_MeasParamMsg;
#define INIT_c_MeasParamMsg(sp) ED_RESET_MEM ((sp), sizeof (c_MeasParamMsg))
#define FREE_c_MeasParamMsg(sp)
#define SETPRESENT_c_MeasParamMsg__3GNCellL(sp,present) ((sp)->_3GNCellL_Present = present)
#define GETPRESENT_c_MeasParamMsg__3GNCellL(sp) ((sp)->_3GNCellL_Present)
#define SETPRESENT_c_MeasParamMsg_Arfcn(sp,present) ((sp)->Arfcn_Present = present)
#define GETPRESENT_c_MeasParamMsg_Arfcn(sp) ((sp)->Arfcn_Present)
#define SETPRESENT_c_MeasParamMsg_BaInd(sp,present) ((sp)->BaInd_Present = present)
#define GETPRESENT_c_MeasParamMsg_BaInd(sp) ((sp)->BaInd_Present)
#define SETPRESENT_c_MeasParamMsg_Bsic(sp,present) ((sp)->Bsic_Present = present)
#define GETPRESENT_c_MeasParamMsg_Bsic(sp) ((sp)->Bsic_Present)
#define SETPRESENT_c_MeasParamMsg_BsicL(sp,present) ((sp)->BsicL_Present = present)
#define GETPRESENT_c_MeasParamMsg_BsicL(sp) ((sp)->BsicL_Present)
#define SETPRESENT_c_MeasParamMsg_C2000CellL(sp,present) ((sp)->C2000CellL_Present = present)
#define GETPRESENT_c_MeasParamMsg_C2000CellL(sp) ((sp)->C2000CellL_Present)
#define SETPRESENT_c_MeasParamMsg_ChgeMark(sp,present) ((sp)->ChgeMark_Present = present)
#define GETPRESENT_c_MeasParamMsg_ChgeMark(sp) ((sp)->ChgeMark_Present)
#define SETPRESENT_c_MeasParamMsg_Count(sp,present) ((sp)->Count_Present = present)
#define GETPRESENT_c_MeasParamMsg_Count(sp) ((sp)->Count_Present)
#define SETPRESENT_c_MeasParamMsg_ExtMeasPar(sp,present) ((sp)->ExtMeasPar_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar(sp) ((sp)->ExtMeasPar_Present)
#define SETPRESENT_c_MeasParamMsg_Idx(sp,present) ((sp)->Idx_Present = present)
#define GETPRESENT_c_MeasParamMsg_Idx(sp) ((sp)->Idx_Present)
#define SETPRESENT_c_MeasParamMsg_ImmRel(sp,present) ((sp)->ImmRel_Present = present)
#define GETPRESENT_c_MeasParamMsg_ImmRel(sp) ((sp)->ImmRel_Present)
#define SETPRESENT_c_MeasParamMsg_InvBsicRep(sp,present) ((sp)->InvBsicRep_Present = present)
#define GETPRESENT_c_MeasParamMsg_InvBsicRep(sp) ((sp)->InvBsicRep_Present)
#define SETPRESENT_c_MeasParamMsg_MeasPar(sp,present) ((sp)->MeasPar_Present = present)
#define GETPRESENT_c_MeasParamMsg_MeasPar(sp) ((sp)->MeasPar_Present)
#define SETPRESENT_c_MeasParamMsg_MultiBandRep(sp,present) ((sp)->MultiBandRep_Present = present)
#define GETPRESENT_c_MeasParamMsg_MultiBandRep(sp) ((sp)->MultiBandRep_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL(sp,present) ((sp)->NcFrqL_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL(sp) ((sp)->NcFrqL_Present)
#define SETPRESENT_c_MeasParamMsg_NcMeasPar(sp,present) ((sp)->NcMeasPar_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcMeasPar(sp) ((sp)->NcMeasPar_Present)
#define SETPRESENT_c_MeasParamMsg_NccPerm(sp,present) ((sp)->NccPerm_Present = present)
#define GETPRESENT_c_MeasParamMsg_NccPerm(sp) ((sp)->NccPerm_Present)
#define SETPRESENT_c_MeasParamMsg_PmoInd(sp,present) ((sp)->PmoInd_Present = present)
#define GETPRESENT_c_MeasParamMsg_PmoInd(sp) ((sp)->PmoInd_Present)
#define SETPRESENT_c_MeasParamMsg_PrioL(sp,present) ((sp)->PrioL_Present = present)
#define GETPRESENT_c_MeasParamMsg_PrioL(sp) ((sp)->PrioL_Present)
#define SETPRESENT_c_MeasParamMsg_Pwrc(sp,present) ((sp)->Pwrc_Present = present)
#define GETPRESENT_c_MeasParamMsg_Pwrc(sp) ((sp)->Pwrc_Present)
#define SETPRESENT_c_MeasParamMsg_R3GCellL(sp,present) ((sp)->R3GCellL_Present = present)
#define GETPRESENT_c_MeasParamMsg_R3GCellL(sp) ((sp)->R3GCellL_Present)
#define SETPRESENT_c_MeasParamMsg_RepOff1800(sp,present) ((sp)->RepOff1800_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepOff1800(sp) ((sp)->RepOff1800_Present)
#define SETPRESENT_c_MeasParamMsg_RepOff1900(sp,present) ((sp)->RepOff1900_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepOff1900(sp) ((sp)->RepOff1900_Present)
#define SETPRESENT_c_MeasParamMsg_RepOff400(sp,present) ((sp)->RepOff400_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepOff400(sp) ((sp)->RepOff400_Present)
#define SETPRESENT_c_MeasParamMsg_RepOff850(sp,present) ((sp)->RepOff850_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepOff850(sp) ((sp)->RepOff850_Present)
#define SETPRESENT_c_MeasParamMsg_RepOff900(sp,present) ((sp)->RepOff900_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepOff900(sp) ((sp)->RepOff900_Present)
#define SETPRESENT_c_MeasParamMsg_RepRate(sp,present) ((sp)->RepRate_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepRate(sp) ((sp)->RepRate_Present)
#define SETPRESENT_c_MeasParamMsg_RepThr1800(sp,present) ((sp)->RepThr1800_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepThr1800(sp) ((sp)->RepThr1800_Present)
#define SETPRESENT_c_MeasParamMsg_RepThr1900(sp,present) ((sp)->RepThr1900_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepThr1900(sp) ((sp)->RepThr1900_Present)
#define SETPRESENT_c_MeasParamMsg_RepThr400(sp,present) ((sp)->RepThr400_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepThr400(sp) ((sp)->RepThr400_Present)
#define SETPRESENT_c_MeasParamMsg_RepThr850(sp,present) ((sp)->RepThr850_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepThr850(sp) ((sp)->RepThr850_Present)
#define SETPRESENT_c_MeasParamMsg_RepThr900(sp,present) ((sp)->RepThr900_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepThr900(sp) ((sp)->RepThr900_Present)
#define SETPRESENT_c_MeasParamMsg_RepTyp(sp,present) ((sp)->RepTyp_Present = present)
#define GETPRESENT_c_MeasParamMsg_RepTyp(sp) ((sp)->RepTyp_Present)
#define SETPRESENT_c_MeasParamMsg_RtdL(sp,present) ((sp)->RtdL_Present = present)
#define GETPRESENT_c_MeasParamMsg_RtdL(sp) ((sp)->RtdL_Present)
#define SETPRESENT_c_MeasParamMsg_ScaleOrd(sp,present) ((sp)->ScaleOrd_Present = present)
#define GETPRESENT_c_MeasParamMsg_ScaleOrd(sp) ((sp)->ScaleOrd_Present)
#define SETPRESENT_c_MeasParamMsg_ServBandRep(sp,present) ((sp)->ServBandRep_Present = present)
#define GETPRESENT_c_MeasParamMsg_ServBandRep(sp) ((sp)->ServBandRep_Present)

#define SETPRESENT_c_Si13Ro_GprsCellOpt_BssPagCoord(sp,present) ((sp)->BssPagCoord_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_BssPagCoord(sp) ((sp)->BssPagCoord_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_Extension_Len(sp,present) ((sp)->Extension_Len_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_Extension_Len(sp) ((sp)->Extension_Len_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_Message_escape(sp,present) ((sp)->Message_escape_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_Message_escape(sp) ((sp)->Message_escape_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_PanDec(sp,present) ((sp)->PanDec_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_PanDec(sp) ((sp)->PanDec_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_PanInc(sp,present) ((sp)->PanInc_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_PanInc(sp) ((sp)->PanInc_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_PanMax(sp,present) ((sp)->PanMax_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_PanMax(sp) ((sp)->PanMax_Present)
#define SETPRESENT_c_Si13Ro_GprsCellOpt_PfcFeatMode(sp,present) ((sp)->PfcFeatMode_Present = present)
#define GETPRESENT_c_Si13Ro_GprsCellOpt_PfcFeatMode(sp) ((sp)->PfcFeatMode_Present)
#define SETPRESENT_c_Si13Ro_GprsMa_ArfcnIdxList(sp,present) ((sp)->ArfcnIdxList_Present = present)
#define GETPRESENT_c_Si13Ro_GprsMa_ArfcnIdxList(sp) ((sp)->ArfcnIdxList_Present)
#define SETPRESENT_c_Si13Ro_GprsMa_MaBitMap(sp,present) ((sp)->MaBitMap_Present = present)
#define GETPRESENT_c_Si13Ro_GprsMa_MaBitMap(sp) ((sp)->MaBitMap_Present)
#define SETPRESENT_c_Si13Ro_GprsMa_MaLen(sp,present) ((sp)->MaLen_Present = present)
#define GETPRESENT_c_Si13Ro_GprsMa_MaLen(sp) ((sp)->MaLen_Present)
#define SETPRESENT_c_Si13Ro_GprsMa_RflNbList(sp,present) ((sp)->RflNbList_Present = present)
#define GETPRESENT_c_Si13Ro_GprsMa_RflNbList(sp) ((sp)->RflNbList_Present)
/* SETITEMS/ADDITEMS commands for type 'c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx'. */
void SETITEMS_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx (c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx* sequence, int desiredItems);
#define ADDITEMS_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx(sequence, itemsToBeAdded) SETITEMS_c_Si13Ro_GprsMa_ArfcnIdxList_ArfcnIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Si13Ro_GprsMa_RflNbList_RflNb'. */
void SETITEMS_c_Si13Ro_GprsMa_RflNbList_RflNb (c_Si13Ro_GprsMa_RflNbList_RflNb* sequence, int desiredItems);
#define ADDITEMS_c_Si13Ro_GprsMa_RflNbList_RflNb(sequence, itemsToBeAdded) SETITEMS_c_Si13Ro_GprsMa_RflNbList_RflNb (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_Rtd12L'. */
void SETITEMS_c_RtdDesc_Rtd12L (c_RtdDesc_Rtd12L* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_Rtd12L(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_Rtd12L (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_Rtd12L_data_Rtd12'. */
void SETITEMS_c_RtdDesc_Rtd12L_data_Rtd12 (c_RtdDesc_Rtd12L_data_Rtd12* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_Rtd12L_data_Rtd12(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_Rtd12L_data_Rtd12 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_Rtd6L'. */
void SETITEMS_c_RtdDesc_Rtd6L (c_RtdDesc_Rtd6L* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_Rtd6L(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_Rtd6L (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_Rtd6L_data_Rtd6'. */
void SETITEMS_c_RtdDesc_Rtd6L_data_Rtd6 (c_RtdDesc_Rtd6L_data_Rtd6* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_Rtd6L_data_Rtd6(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_Rtd6L_data_Rtd6 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_StartRtd12L_Rtd12'. */
void SETITEMS_c_RtdDesc_StartRtd12L_Rtd12 (c_RtdDesc_StartRtd12L_Rtd12* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_StartRtd12L_Rtd12(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_StartRtd12L_Rtd12 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RtdDesc_StartRtd6L_Rtd6'. */
void SETITEMS_c_RtdDesc_StartRtd6L_Rtd6 (c_RtdDesc_StartRtd6L_Rtd6* sequence, int desiredItems);
#define ADDITEMS_c_RtdDesc_StartRtd6L_Rtd6(sequence, itemsToBeAdded) SETITEMS_c_RtdDesc_StartRtd6L_Rtd6 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Rtd6Struct_Rtd6'. */
void SETITEMS_c_Rtd6Struct_Rtd6 (c_Rtd6Struct_Rtd6* sequence, int desiredItems);
#define ADDITEMS_c_Rtd6Struct_Rtd6(sequence, itemsToBeAdded) SETITEMS_c_Rtd6Struct_Rtd6 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_Rtd12Struct_Rtd12'. */
void SETITEMS_c_Rtd12Struct_Rtd12 (c_Rtd12Struct_Rtd12* sequence, int desiredItems);
#define ADDITEMS_c_Rtd12Struct_Rtd12(sequence, itemsToBeAdded) SETITEMS_c_Rtd12Struct_Rtd12 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_BsicDesc_Bsic'. */
void SETITEMS_c_BsicDesc_Bsic (c_BsicDesc_Bsic* sequence, int desiredItems);
#define ADDITEMS_c_BsicDesc_Bsic(sequence, itemsToBeAdded) SETITEMS_c_BsicDesc_Bsic (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_BsicDesc_FreqScrol'. */
void SETITEMS_c_BsicDesc_FreqScrol (c_BsicDesc_FreqScrol* sequence, int desiredItems);
#define ADDITEMS_c_BsicDesc_FreqScrol(sequence, itemsToBeAdded) SETITEMS_c_BsicDesc_FreqScrol (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_RepPrioDesc_RepPrio'. */
void SETITEMS_c_RepPrioDesc_RepPrio (c_RepPrioDesc_RepPrio* sequence, int desiredItems);
#define ADDITEMS_c_RepPrioDesc_RepPrio(sequence, itemsToBeAdded) SETITEMS_c_RepPrioDesc_RepPrio (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_ABsic'. */
void SETITEMS_c_NcFrqListDesc_ABsic (c_NcFrqListDesc_ABsic* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_ABsic(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_ABsic (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_ABsicFirst'. */
void SETITEMS_c_NcFrqListDesc_ABsicFirst (c_NcFrqListDesc_ABsicFirst* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_ABsicFirst(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_ABsicFirst (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_ACSelPar'. */
void SETITEMS_c_NcFrqListDesc_ACSelPar (c_NcFrqListDesc_ACSelPar* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_ACSelPar(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_ACSelPar (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelPar_data_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelPar_data_TmpOffset(sp) ((sp)->TmpOffset_Present)
/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_ACSelParFirst'. */
void SETITEMS_c_NcFrqListDesc_ACSelParFirst (c_NcFrqListDesc_ACSelParFirst* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_ACSelParFirst(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_ACSelParFirst (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_NcFrqListDesc_ACSelParFirst_data_TmpOffset(sp) ((sp)->TmpOffset_Present)
/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_AFrqDiff'. */
void SETITEMS_c_NcFrqListDesc_AFrqDiff (c_NcFrqListDesc_AFrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_AFrqDiff(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_AFrqDiff (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_AFrqDiffLen'. */
void SETITEMS_c_NcFrqListDesc_AFrqDiffLen (c_NcFrqListDesc_AFrqDiffLen* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_AFrqDiffLen(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_AFrqDiffLen (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_AFrqFirst'. */
void SETITEMS_c_NcFrqListDesc_AFrqFirst (c_NcFrqListDesc_AFrqFirst* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_AFrqFirst(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_AFrqFirst (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_NbAFrq'. */
void SETITEMS_c_NcFrqListDesc_NbAFrq (c_NcFrqListDesc_NbAFrq* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_NbAFrq(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_NbAFrq (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_NcFrqListDesc_RFrqIdx'. */
void SETITEMS_c_NcFrqListDesc_RFrqIdx (c_NcFrqListDesc_RFrqIdx* sequence, int desiredItems);
#define ADDITEMS_c_NcFrqListDesc_RFrqIdx(sequence, itemsToBeAdded) SETITEMS_c_NcFrqListDesc_RFrqIdx (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_AddedFrqListStruct_ABsic'. */
void SETITEMS_c_AddedFrqListStruct_ABsic (c_AddedFrqListStruct_ABsic* sequence, int desiredItems);
#define ADDITEMS_c_AddedFrqListStruct_ABsic(sequence, itemsToBeAdded) SETITEMS_c_AddedFrqListStruct_ABsic (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_AddedFrqListStruct_ACSelPar'. */
void SETITEMS_c_AddedFrqListStruct_ACSelPar (c_AddedFrqListStruct_ACSelPar* sequence, int desiredItems);
#define ADDITEMS_c_AddedFrqListStruct_ACSelPar(sequence, itemsToBeAdded) SETITEMS_c_AddedFrqListStruct_ACSelPar (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelPar_data_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelPar_data_TmpOffset(sp) ((sp)->TmpOffset_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_AddedFrqListStruct_ACSelParFirst_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_AddedFrqListStruct_ACSelParFirst_TmpOffset(sp) ((sp)->TmpOffset_Present)
/* SETITEMS/ADDITEMS commands for type 'c_AddedFrqListStruct_AFrqDiff'. */
void SETITEMS_c_AddedFrqListStruct_AFrqDiff (c_AddedFrqListStruct_AFrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_AddedFrqListStruct_AFrqDiff(sequence, itemsToBeAdded) SETITEMS_c_AddedFrqListStruct_AFrqDiff (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_ExtMeasParamStruct_ExtFrqList_FrqDiff'. */
void SETITEMS_c_ExtMeasParamStruct_ExtFrqList_FrqDiff (c_ExtMeasParamStruct_ExtFrqList_FrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_ExtMeasParamStruct_ExtFrqList_FrqDiff(sequence, itemsToBeAdded) SETITEMS_c_ExtMeasParamStruct_ExtFrqList_FrqDiff (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_ExtFrqListDesc_FrqDiff'. */
void SETITEMS_c_ExtFrqListDesc_FrqDiff (c_ExtFrqListDesc_FrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_ExtFrqListDesc_FrqDiff(sequence, itemsToBeAdded) SETITEMS_c_ExtFrqListDesc_FrqDiff (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_BsicL_BaIdxStartBsic(sp,present) ((sp)->BaIdxStartBsic_Present = present)
#define GETPRESENT_c_MeasParamMsg_BsicL_BaIdxStartBsic(sp) ((sp)->BaIdxStartBsic_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_BsicL_Bsic'. */
void SETITEMS_c_MeasParamMsg_BsicL_Bsic (c_MeasParamMsg_BsicL_Bsic* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_BsicL_Bsic(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_BsicL_Bsic (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_BsicL_FreqScrol'. */
void SETITEMS_c_MeasParamMsg_BsicL_FreqScrol (c_MeasParamMsg_BsicL_FreqScrol* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_BsicL_FreqScrol(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_BsicL_FreqScrol (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtFrqList(sp,present) ((sp)->ExtFrqList_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtFrqList(sp) ((sp)->ExtFrqList_Present)
#define SETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtRepPer(sp,present) ((sp)->ExtRepPer_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtRepPer(sp) ((sp)->ExtRepPer_Present)
#define SETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtRepType(sp,present) ((sp)->ExtRepType_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar_ExtRepType(sp) ((sp)->ExtRepType_Present)
#define SETPRESENT_c_MeasParamMsg_ExtMeasPar_IntFrq(sp,present) ((sp)->IntFrq_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar_IntFrq(sp) ((sp)->IntFrq_Present)
#define SETPRESENT_c_MeasParamMsg_ExtMeasPar_NccPerm(sp,present) ((sp)->NccPerm_Present = present)
#define GETPRESENT_c_MeasParamMsg_ExtMeasPar_NccPerm(sp) ((sp)->NccPerm_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff'. */
void SETITEMS_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff (c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_ExtMeasPar_ExtFrqList_FrqDiff (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_NcFrqL_NbRFrq(sp,present) ((sp)->NbRFrq_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_NbRFrq(sp) ((sp)->NbRFrq_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_ABsic'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_ABsic (c_MeasParamMsg_NcFrqL_ABsic* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_ABsic(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_ABsic (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_ABsicFirst'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_ABsicFirst (c_MeasParamMsg_NcFrqL_ABsicFirst* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_ABsicFirst(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_ABsicFirst (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_ACSelPar'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_ACSelPar (c_MeasParamMsg_NcFrqL_ACSelPar* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_ACSelPar(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_ACSelPar (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelPar_data_TmpOffset(sp) ((sp)->TmpOffset_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_ACSelParFirst'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_ACSelParFirst (c_MeasParamMsg_NcFrqL_ACSelParFirst* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_ACSelParFirst(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_ACSelParFirst (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_HcsThr(sp,present) ((sp)->HcsThr_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_HcsThr(sp) ((sp)->HcsThr_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_MsTxPMaxCcch(sp,present) ((sp)->MsTxPMaxCcch_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_MsTxPMaxCcch(sp) ((sp)->MsTxPMaxCcch_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PSi1RepPeriod(sp,present) ((sp)->PSi1RepPeriod_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PSi1RepPeriod(sp) ((sp)->PSi1RepPeriod_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PbcchLoc(sp,present) ((sp)->PbcchLoc_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PbcchLoc(sp) ((sp)->PbcchLoc_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PenaltyTime(sp,present) ((sp)->PenaltyTime_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PenaltyTime(sp) ((sp)->PenaltyTime_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PrioClass(sp,present) ((sp)->PrioClass_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_PrioClass(sp) ((sp)->PrioClass_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RslOffset(sp,present) ((sp)->RslOffset_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RslOffset(sp) ((sp)->RslOffset_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RxLevAccMin(sp,present) ((sp)->RxLevAccMin_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_RxLevAccMin(sp) ((sp)->RxLevAccMin_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_Si13Loc(sp,present) ((sp)->Si13Loc_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_Si13Loc(sp) ((sp)->Si13Loc_Present)
#define SETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_TmpOffset(sp,present) ((sp)->TmpOffset_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcFrqL_ACSelParFirst_data_TmpOffset(sp) ((sp)->TmpOffset_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_AFrqDiff'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_AFrqDiff (c_MeasParamMsg_NcFrqL_AFrqDiff* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_AFrqDiff(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_AFrqDiff (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_AFrqDiffLen'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_AFrqDiffLen (c_MeasParamMsg_NcFrqL_AFrqDiffLen* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_AFrqDiffLen(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_AFrqDiffLen (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_AFrqFirst'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_AFrqFirst (c_MeasParamMsg_NcFrqL_AFrqFirst* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_AFrqFirst(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_AFrqFirst (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_NbAFrq'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_NbAFrq (c_MeasParamMsg_NcFrqL_NbAFrq* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_NbAFrq(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_NbAFrq (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_NcFrqL_RFrqIdx'. */
void SETITEMS_c_MeasParamMsg_NcFrqL_RFrqIdx (c_MeasParamMsg_NcFrqL_RFrqIdx* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_NcFrqL_RFrqIdx(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_NcFrqL_RFrqIdx (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_NcMeasPar_NcNonDrxPer(sp,present) ((sp)->NcNonDrxPer_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcMeasPar_NcNonDrxPer(sp) ((sp)->NcNonDrxPer_Present)
#define SETPRESENT_c_MeasParamMsg_NcMeasPar_NcRepPerI(sp,present) ((sp)->NcRepPerI_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcMeasPar_NcRepPerI(sp) ((sp)->NcRepPerI_Present)
#define SETPRESENT_c_MeasParamMsg_NcMeasPar_NcRepPerT(sp,present) ((sp)->NcRepPerT_Present = present)
#define GETPRESENT_c_MeasParamMsg_NcMeasPar_NcRepPerT(sp) ((sp)->NcRepPerT_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_PrioL_RepPrio'. */
void SETITEMS_c_MeasParamMsg_PrioL_RepPrio (c_MeasParamMsg_PrioL_RepPrio* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_PrioL_RepPrio(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_PrioL_RepPrio (sequence, (sequence)->items+itemsToBeAdded)

#define SETPRESENT_c_MeasParamMsg_RtdL_IdxStartRtd12(sp,present) ((sp)->IdxStartRtd12_Present = present)
#define GETPRESENT_c_MeasParamMsg_RtdL_IdxStartRtd12(sp) ((sp)->IdxStartRtd12_Present)
#define SETPRESENT_c_MeasParamMsg_RtdL_IdxStartRtd6(sp,present) ((sp)->IdxStartRtd6_Present = present)
#define GETPRESENT_c_MeasParamMsg_RtdL_IdxStartRtd6(sp) ((sp)->IdxStartRtd6_Present)
#define SETPRESENT_c_MeasParamMsg_RtdL_StartRtd12L(sp,present) ((sp)->StartRtd12L_Present = present)
#define GETPRESENT_c_MeasParamMsg_RtdL_StartRtd12L(sp) ((sp)->StartRtd12L_Present)
#define SETPRESENT_c_MeasParamMsg_RtdL_StartRtd6L(sp,present) ((sp)->StartRtd6L_Present = present)
#define GETPRESENT_c_MeasParamMsg_RtdL_StartRtd6L(sp) ((sp)->StartRtd6L_Present)
/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_Rtd12L'. */
void SETITEMS_c_MeasParamMsg_RtdL_Rtd12L (c_MeasParamMsg_RtdL_Rtd12L* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_Rtd12L(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_Rtd12L (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12'. */
void SETITEMS_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12 (c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_Rtd12L_data_Rtd12 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_Rtd6L'. */
void SETITEMS_c_MeasParamMsg_RtdL_Rtd6L (c_MeasParamMsg_RtdL_Rtd6L* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_Rtd6L(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_Rtd6L (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6'. */
void SETITEMS_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6 (c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_Rtd6L_data_Rtd6 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_StartRtd12L_Rtd12'. */
void SETITEMS_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12 (c_MeasParamMsg_RtdL_StartRtd12L_Rtd12* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_StartRtd12L_Rtd12 (sequence, (sequence)->items+itemsToBeAdded)

/* SETITEMS/ADDITEMS commands for type 'c_MeasParamMsg_RtdL_StartRtd6L_Rtd6'. */
void SETITEMS_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6 (c_MeasParamMsg_RtdL_StartRtd6L_Rtd6* sequence, int desiredItems);
#define ADDITEMS_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6(sequence, itemsToBeAdded) SETITEMS_c_MeasParamMsg_RtdL_StartRtd6L_Rtd6 (sequence, (sequence)->items+itemsToBeAdded)

#ifdef __cplusplus
};
#endif

#endif

