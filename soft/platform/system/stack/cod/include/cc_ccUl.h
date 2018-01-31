/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __cc_ccUl_h
#define __cc_ccUl_h
#include "ed_lib.h"
#include "cc_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ALERTING_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ALERTING_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ALERTING_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ALERTING_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ALERTING_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ALERTING_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ALERTING_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ALERTING_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ALERTING_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ALERTING_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CALL_CONFIRMED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CALL_CONFIRMED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CALL_CONFIRMED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CALL_CONFIRMED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CALL_CONFIRMED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CALL_CONFIRMED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CALL_CONFIRMED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CALL_CONFIRMED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CALL_CONFIRMED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CALL_CONFIRMED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CONNECT_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CONNECT_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONNECT_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CONNECT_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONNECT_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CONNECT_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CONNECT_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONNECT_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CONNECT_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONNECT_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DISCONNECT_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DISCONNECT_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DISCONNECT_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DISCONNECT_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DISCONNECT_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "DISCONNECT_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_DISCONNECT_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DISCONNECT_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_DISCONNECT_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DISCONNECT_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "EMERGENCY_SETUP".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_EMERGENCY_SETUP (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_EMERGENCY_SETUP* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_EMERGENCY_SETUP (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_EMERGENCY_SETUP* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "EMERGENCY_SETUP".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_EMERGENCY_SETUP (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_EMERGENCY_SETUP* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_EMERGENCY_SETUP (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_EMERGENCY_SETUP* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "FACILITY_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_FACILITY_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_FACILITY_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_FACILITY_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FACILITY_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "FACILITY_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_FACILITY_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_FACILITY_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_FACILITY_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FACILITY_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "MODIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_MODIFY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_MODIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "MODIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_MODIFY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_MODIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "MODIFY_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_MODIFY_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_MODIFY_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "MODIFY_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_MODIFY_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_MODIFY_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "MODIFY_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_MODIFY_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_MODIFY_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "MODIFY_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_MODIFY_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MODIFY_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_MODIFY_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MODIFY_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RELEASE_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RELEASE_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RELEASE_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RELEASE_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RELEASE_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RELEASE_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RELEASE_COMPLETE_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RELEASE_COMPLETE_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_COMPLETE_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RELEASE_COMPLETE_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_COMPLETE_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RELEASE_COMPLETE_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RELEASE_COMPLETE_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_COMPLETE_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RELEASE_COMPLETE_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_COMPLETE_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "SETUP_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_SETUP_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_SETUP_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_SETUP_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_SETUP_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "SETUP_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_SETUP_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_SETUP_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_SETUP_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_SETUP_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "START_DTMF".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_START_DTMF (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_START_DTMF (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "START_DTMF".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_START_DTMF (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_START_DTMF (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF* ED_CONST Destin, long Length));

int Match_c_ALERTING_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CALL_CONFIRMED (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CONNECT_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_DISCONNECT_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_EMERGENCY_SETUP (const char* Buffer, ED_CONST long BitOffset);
int Match_c_FACILITY_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_MODIFY (const char* Buffer, ED_CONST long BitOffset);
int Match_c_MODIFY_COMPLETE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_MODIFY_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RELEASE_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RELEASE_COMPLETE_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_SETUP_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_START_DTMF (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

