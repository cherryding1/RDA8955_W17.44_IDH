/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __mm_edud_h
#define __mm_edud_h
#include "ed_lib.h"
#include "mm_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "LocUpReqType".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_LocUpReqType (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LocUpReqType* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_LocUpReqType (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LocUpReqType* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AttachTypeIE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AttachTypeIE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AttachTypeIE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AttachTypeIE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AttachTypeIE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CiphAlgoIE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CiphAlgoIE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CiphAlgoIE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CiphAlgoIE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CiphAlgoIE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DRXParamIE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DRXParamIE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DRXParamIE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DRXParamIE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DRXParamIE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IMEISVRequest".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_IMEISVRequest (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IMEISVRequest* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_IMEISVRequest (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMEISVRequest* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "GPRSTimerIE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_GPRSTimerIE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GPRSTimerIE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_GPRSTimerIE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GPRSTimerIE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RAUTypeIE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RAUTypeIE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAUTypeIE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RAUTypeIE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAUTypeIE* ED_CONST Destin, long Length));


#ifdef __cplusplus
	};
#endif
#endif

