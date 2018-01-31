/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __cc_ccDl_h
#define __cc_ccDl_h
#include "ed_lib.h"
#include "cc_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ALERTING_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ALERTING_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ALERTING_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ALERTING_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ALERTING_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ALERTING_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ALERTING_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ALERTING_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ALERTING_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ALERTING_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CALL_PROCEEDING".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CALL_PROCEEDING (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CALL_PROCEEDING* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CALL_PROCEEDING (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CALL_PROCEEDING* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CALL_PROCEEDING".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CALL_PROCEEDING (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CALL_PROCEEDING* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CALL_PROCEEDING (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CALL_PROCEEDING* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CONGESTION_CONTROL".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CONGESTION_CONTROL (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONGESTION_CONTROL* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CONGESTION_CONTROL (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONGESTION_CONTROL* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CONGESTION_CONTROL".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CONGESTION_CONTROL (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONGESTION_CONTROL* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CONGESTION_CONTROL (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONGESTION_CONTROL* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CONNECT_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CONNECT_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONNECT_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CONNECT_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONNECT_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CONNECT_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CONNECT_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CONNECT_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CONNECT_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CONNECT_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DISCONNECT_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DISCONNECT_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DISCONNECT_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DISCONNECT_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DISCONNECT_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "DISCONNECT_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_DISCONNECT_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DISCONNECT_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_DISCONNECT_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DISCONNECT_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "FACILITY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_FACILITY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_FACILITY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_FACILITY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FACILITY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "FACILITY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_FACILITY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_FACILITY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_FACILITY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_FACILITY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "HOLD_REJECTED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_HOLD_REJECTED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_HOLD_REJECTED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_HOLD_REJECTED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_HOLD_REJECTED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "HOLD_REJECTED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_HOLD_REJECTED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_HOLD_REJECTED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_HOLD_REJECTED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_HOLD_REJECTED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "NOTIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_NOTIFY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_NOTIFY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_NOTIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_NOTIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "NOTIFY".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_NOTIFY (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_NOTIFY* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_NOTIFY (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_NOTIFY* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PROGRESS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PROGRESS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PROGRESS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PROGRESS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PROGRESS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PROGRESS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PROGRESS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PROGRESS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PROGRESS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PROGRESS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RELEASE_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RELEASE_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RELEASE_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RELEASE_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RELEASE_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RELEASE_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RELEASE_COMPLETE_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RELEASE_COMPLETE_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_COMPLETE_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RELEASE_COMPLETE_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_COMPLETE_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RELEASE_COMPLETE_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RELEASE_COMPLETE_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RELEASE_COMPLETE_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RELEASE_COMPLETE_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RELEASE_COMPLETE_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RETRIEVE_REJECTED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RETRIEVE_REJECTED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RETRIEVE_REJECTED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RETRIEVE_REJECTED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RETRIEVE_REJECTED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RETRIEVE_REJECTED".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RETRIEVE_REJECTED (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RETRIEVE_REJECTED* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RETRIEVE_REJECTED (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RETRIEVE_REJECTED* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "SETUP_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_SETUP_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_SETUP_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_SETUP_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_SETUP_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "SETUP_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_SETUP_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_SETUP_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_SETUP_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_SETUP_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "START_DTMF_ACK".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_START_DTMF_ACK (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF_ACK* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_START_DTMF_ACK (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF_ACK* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "START_DTMF_ACK".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_START_DTMF_ACK (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF_ACK* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_START_DTMF_ACK (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF_ACK* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "START_DTMF_REJ".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_START_DTMF_REJ (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF_REJ* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_START_DTMF_REJ (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF_REJ* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "START_DTMF_REJ".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_START_DTMF_REJ (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_START_DTMF_REJ* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_START_DTMF_REJ (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_START_DTMF_REJ* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "STATUS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_STATUS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_STATUS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_STATUS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_STATUS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "STATUS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_STATUS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_STATUS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_STATUS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_STATUS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "USER_INFO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_USER_INFO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_USER_INFO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_USER_INFO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_USER_INFO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "USER_INFO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_USER_INFO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_USER_INFO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_USER_INFO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_USER_INFO* ED_CONST Destin, long Length));

int Match_c_ALERTING_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CALL_PROCEEDING (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CONGESTION_CONTROL (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CONNECT_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_DISCONNECT_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_FACILITY (const char* Buffer, ED_CONST long BitOffset);
int Match_c_HOLD_REJECTED (const char* Buffer, ED_CONST long BitOffset);
int Match_c_NOTIFY (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PROGRESS (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RELEASE_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RELEASE_COMPLETE_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RETRIEVE_REJECTED (const char* Buffer, ED_CONST long BitOffset);
int Match_c_SETUP_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_START_DTMF_ACK (const char* Buffer, ED_CONST long BitOffset);
int Match_c_START_DTMF_REJ (const char* Buffer, ED_CONST long BitOffset);
int Match_c_STATUS (const char* Buffer, ED_CONST long BitOffset);
int Match_c_USER_INFO (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

