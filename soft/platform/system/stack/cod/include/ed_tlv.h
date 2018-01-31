/*******************************************************************************
  ed_tlv.h  Developed by Dafocus - http://www.dafocus.com/
*******************************************************************************/
#ifndef __ED_TLV_H
#define __ED_TLV_H
#include "ed_user.h"
#include "ed_lib.h"
#ifdef __cplusplus
	extern "C" {
#endif

/*------------------------------------------------------------------------------
	Macro invoked when we need to decode a mandatory information element
------------------------------------------------------------------------------*/
#ifndef ED_EXPECT_MANDATORY_IE
#define ED_EXPECT_MANDATORY_IE(IEI,IEISIZE,ERRID,KNOWNL3IES) \
		Tag = EDGetIE (IEI, IEISIZE, Buffer, &CurrOfs, Length+BitOffset, KNOWNL3IES, ED_KNOWN_L3_IE_COUNT, NULL);\
		if (Tag < 0) return Tag;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when we need to decode an optional information element
------------------------------------------------------------------------------*/
#ifndef ED_EXPECT_OPTIONAL_IE
#define ED_EXPECT_OPTIONAL_IE(IEI,IEISIZE,ERRID,SKIPINDEX,KNOWNL3IES) \
		Tag = EDGetIE (IEI, IEISIZE, Buffer, &CurrOfs, Length+BitOffset, KNOWNL3IES, ED_KNOWN_L3_IE_COUNT, TLVSkipItems+SKIPINDEX);\
		if (Tag < 0) return Tag;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a syntax error is found on a mandatory IE.
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_MANDATORY_IE_SYNTAX_ERROR
#define ED_HANDLE_MANDATORY_IE_SYNTAX_ERROR(IEI,ERRID,FIELD) return ED_MANDATORY_IE_SYNTAX_ERROR;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a syntax error is found on a optional IE.
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_OPTIONAL_IE_SYNTAX_ERROR
#define ED_HANDLE_OPTIONAL_IE_SYNTAX_ERROR(IEI,ERRID,FIELD,PRESENT) PRESENT=ED_FALSE;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a syntax error is found on a conditional IE.
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_CONDITIONAL_IE_SYNTAX_ERROR
#define ED_HANDLE_CONDITIONAL_IE_SYNTAX_ERROR(IEI,ERRID,FIELD,PRESENT) return ED_CONDITIONAL_IE_SYNTAX_ERROR;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when an error decoding the internal part of an IE is found.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_IE_SYNTAX_ERROR
#define ED_HANDLE_IE_SYNTAX_ERROR(ERRID) return ED_SYNTAX_ERROR;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a message is too short reading a mandatory IE .
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_MANDATORY_IE_MSG_TOO_SHORT
#define ED_HANDLE_MANDATORY_IE_MSG_TOO_SHORT(IEI,ERRID,FIELD) return ED_MANDATORY_IE_SYNTAX_ERROR;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a message is too short reading an optional IE .
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_OPTIONAL_IE_MSG_TOO_SHORT
#define ED_HANDLE_OPTIONAL_IE_MSG_TOO_SHORT(IEI,ERRID,FIELD,PRESENT) {PRESENT=ED_FALSE; CurrOfs = (BitOffset+Length);}
#endif

/*------------------------------------------------------------------------------
	Macro invoked when a message is too short reading a conditional IE .
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_CONDITIONAL_IE_MSG_TOO_SHORT
#define ED_HANDLE_CONDITIONAL_IE_MSG_TOO_SHORT(IEI,ERRID,FIELD,PRESENT) return ED_CONDITIONAL_IE_SYNTAX_ERROR;
#endif

/*------------------------------------------------------------------------------
	Macro invoked when an variable sized information element is out of range.
	IEI is -1 when the information element is not tagged.
------------------------------------------------------------------------------*/
#ifndef ED_HANDLE_IE_SIZE_ERROR
#define ED_HANDLE_IE_SIZE_ERROR(IEI,ERRID,FIELD,PRESENT) return ED_IE_SIZE_ERROR;
#endif
#ifdef __cplusplus
	};
#endif
#endif
