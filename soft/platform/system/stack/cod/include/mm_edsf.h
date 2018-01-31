/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __mm_edsf_h
#define __mm_edsf_h
#include "ed_lib.h"
#include "mm_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "InformationIE_t".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_InformationIE_t (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_InformationIE_t* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_InformationIE_t (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_InformationIE_t* ED_CONST Destin, long Length));


#ifdef __cplusplus
	};
#endif
#endif

