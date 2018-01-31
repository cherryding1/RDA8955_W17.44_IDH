/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-recog
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __cc_edre_h
#define __cc_edre_h
#include "ed_c.h"
#include "cc_ed_c.h"

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
int cc_SetDecode_ccUl (const char* buffer, TccUl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int cc_Decode_ccUl (const char* buffer, int len, TccUl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int cc_SetDecode_ccDl (const char* buffer, TccDl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int cc_Decode_ccDl (const char* buffer, int len, TccDl_Data* data);
#endif

#ifdef __cplusplus
	};
#endif
#endif

