/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-recog
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __mm_edre_h
#define __mm_edre_h
#include "ed_c.h"
#include "mm_ed_c.h"

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
int mm_SetDecode_mmUl (const char* buffer, TmmUl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int mm_Decode_mmUl (const char* buffer, int len, TmmUl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int mm_SetDecode_mmDl (const char* buffer, TmmDl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int mm_Decode_mmDl (const char* buffer, int len, TmmDl_Data* data);
#endif

#ifdef __cplusplus
	};
#endif
#endif

