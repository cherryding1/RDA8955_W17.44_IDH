/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __edud_h
#define __edud_h
#include "ed_lib.h"
#include "ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

#ifdef rrd_USES_MSGSET_UserDefinedDataTypes
#include "rrd_edud.h"
#endif

#ifdef rri_USES_MSGSET_UserDefinedDataTypes
#include "rri_edud.h"
#endif

#ifdef mm_USES_MSGSET_UserDefinedDataTypes
#include "mm_edud.h"
#endif

#ifdef cc_USES_MSGSET_UserDefinedDataTypes
#include "cc_edud.h"
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TSparePadding".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TSparePadding (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TSparePadding* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TSparePadding (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TSparePadding* ED_CONST Destin, long Length));


#ifdef __cplusplus
	};
#endif
#endif

