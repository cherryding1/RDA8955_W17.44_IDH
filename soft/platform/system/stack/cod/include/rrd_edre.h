/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-recog
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_edre_h
#define __rrd_edre_h
#include "ed_c.h"
#include "rrd_ed_c.h"

#include "ed_user.h"
#define ED_MAXSIZE 600
#include "edcf.h"

#ifdef __cplusplus
	extern "C" {
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int rrd_SetDecode_DchD (const char* buffer, TDchD_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int rrd_Decode_DchD (const char* buffer, int len, TDchD_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int rrd_SetDecode_Ccch (const char* buffer, TCcch_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int rrd_Decode_Ccch (const char* buffer, int len, TCcch_Data* data);
#endif

#ifdef __cplusplus
	};
#endif
#endif

