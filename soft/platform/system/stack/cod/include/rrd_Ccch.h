/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_Ccch_h
#define __rrd_Ccch_h
#include "ed_lib.h"
#include "rrd_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IMM_ASS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IMM_ASS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IMM_ASS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_IMM_ASS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IMM_ASS_EXT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IMM_ASS_EXT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS_EXT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IMM_ASS_EXT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_IMM_ASS_EXT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS_EXT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IMM_ASS_REJ".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_IMM_ASS_REJ (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS_REJ* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IMM_ASS_REJ".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_IMM_ASS_REJ (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMM_ASS_REJ* ED_CONST Destin, long Length));

int Match_c_IMM_ASS (const char* Buffer, ED_CONST long BitOffset);
int Match_c_IMM_ASS_EXT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_IMM_ASS_REJ (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

