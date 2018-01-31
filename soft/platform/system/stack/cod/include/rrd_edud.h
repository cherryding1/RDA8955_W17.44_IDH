/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_edud_h
#define __rrd_edud_h
#include "ed_lib.h"
#include "rrd_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TCellDesc".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TCellDesc (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TCellDesc* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TCellDesc (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TCellDesc* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TChDesc".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TChDesc (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TChDesc* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TChDesc (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TChDesc* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TCipherModeSetting".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TCipherModeSetting (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TCipherModeSetting* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TCipherModeSetting (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TCipherModeSetting* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TFrqChnSeq".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TFrqChnSeq (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TFrqChnSeq* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TFrqChnSeq (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TFrqChnSeq* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TDedicatedModeOrTbf".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TDedicatedModeOrTbf (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TDedicatedModeOrTbf* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TDedicatedModeOrTbf (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TDedicatedModeOrTbf* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TPowerCommand".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TPowerCommand (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TPowerCommand* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TPowerCommand (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TPowerCommand* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TPowerCommandAccType".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TPowerCommandAccType (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TPowerCommandAccType* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TPowerCommandAccType (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TPowerCommandAccType* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TReqRef".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TReqRef (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TReqRef* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TReqRef (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TReqRef* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TStartingTime".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TStartingTime (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TStartingTime* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TStartingTime (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TStartingTime* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TSynchInd".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TSynchInd (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TSynchInd* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TSynchInd (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TSynchInd* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TSkip".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TSkip (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TSkip* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TSkip (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TSkip* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TFrqList".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TFrqList (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TFrqList* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TFrqList (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TFrqList* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "FrequencyList".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_FrequencyList (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_FrequencyList* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_FrequencyList (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FrequencyList* ED_CONST Destin, long Length));


#ifdef __cplusplus
	};
#endif
#endif

