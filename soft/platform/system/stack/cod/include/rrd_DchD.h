/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_DchD_h
#define __rrd_DchD_h
#include "ed_lib.h"
#include "rrd_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ASS_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_ASS_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ASS_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ASS_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_ASS_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ASS_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CHN_MOD_MODIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_CHN_MOD_MODIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CHN_MOD_MODIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CHN_MOD_MODIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_CHN_MOD_MODIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CHN_MOD_MODIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CHN_REL".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_CHN_REL (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CHN_REL* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CHN_REL".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_CHN_REL (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CHN_REL* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CIP_MOD_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_CIP_MOD_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CIP_MOD_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CIP_MOD_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_CIP_MOD_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CIP_MOD_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "FRQ_RDF".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_FRQ_RDF (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FRQ_RDF* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "FRQ_RDF".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_FRQ_RDF (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FRQ_RDF* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "HOV_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_HOV_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_HOV_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "HOV_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_HOV_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_HOV_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PHY_INFO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_c_PHY_INFO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PHY_INFO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PHY_INFO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, DECODE_BODY_c_PHY_INFO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PHY_INFO* ED_CONST Destin, long Length));

int Match_c_ASS_CMD (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CHN_MOD_MODIFY (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CHN_REL (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CIP_MOD_CMD (const char* Buffer, ED_CONST long BitOffset);
int Match_c_FRQ_RDF (const char* Buffer, ED_CONST long BitOffset);
int Match_c_HOV_CMD (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PHY_INFO (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

