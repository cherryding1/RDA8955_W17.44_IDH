// **************************************************************************
// Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
//	EnDec version 1.0.75
//	GENERATOR: ed-c-recog
//	http://www.Dafocus.it/
//**************************************************************************
// REVISION HISTORY:
// DMX | 20070330 | add DECODE_c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD foe EDGE
// **************************************************************************

#ifndef __mm_mmDl_h
#define __mm_mmDl_h
#include "ed_lib.h"
#include "mm_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ATTACH_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ATTACH_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ATTACH_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ATTACH_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ATTACH_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ATTACH_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ATTACH_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ATTACH_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ATTACH_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ATTACH_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ATTACH_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ATTACH_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DETACH_REQUEST_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DETACH_REQUEST_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_REQUEST_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DETACH_REQUEST_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_REQUEST_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "DETACH_REQUEST_MT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_DETACH_REQUEST_MT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_REQUEST_MT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_DETACH_REQUEST_MT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_REQUEST_MT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DETACH_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DETACH_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DETACH_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "DETACH_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_DETACH_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_DETACH_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PTMSI_REALLOCATION_COMMAND".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PTMSI_REALLOCATION_COMMAND (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PTMSI_REALLOCATION_COMMAND* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PTMSI_REALLOCATION_COMMAND (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PTMSI_REALLOCATION_COMMAND* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PTMSI_REALLOCATION_COMMAND".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PTMSI_REALLOCATION_COMMAND (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PTMSI_REALLOCATION_COMMAND* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PTMSI_REALLOCATION_COMMAND (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PTMSI_REALLOCATION_COMMAND* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_AND_CIPHERING_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_AND_CIPHERING_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_AND_CIPHERING_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_AND_CIPHERING_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_AND_CIPHERING_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_AND_CIPHERING_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_AND_CIPHERING_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_AND_CIPHERING_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_AND_CIPHERING_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_AND_CIPHERING_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_AND_CIPHERING_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_AND_CIPHERING_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PIDENTITY_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PIDENTITY_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PIDENTITY_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PIDENTITY_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PIDENTITY_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PIDENTITY_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PIDENTITY_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PIDENTITY_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PIDENTITY_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PIDENTITY_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RAU_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RAU_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RAU_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RAU_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RAU_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RAU_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RAU_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RAU_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RAU_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RAU_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RAU_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RAU_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PSTATUS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PSTATUS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PSTATUS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PSTATUS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PSTATUS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PSTATUS".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PSTATUS (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PSTATUS* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PSTATUS (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PSTATUS* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "GMM_INFORMATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_GMM_INFORMATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GMM_INFORMATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_GMM_INFORMATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GMM_INFORMATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "GMM_INFORMATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_GMM_INFORMATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GMM_INFORMATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_GMM_INFORMATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GMM_INFORMATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "GPRS_TEST_MODE_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_GPRS_TEST_MODE_CMD (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GPRS_TEST_MODE_CMD* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_GPRS_TEST_MODE_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GPRS_TEST_MODE_CMD* ED_CONST Destin, long Length));

// add by dingmx 20070330 for EDGE
#ifdef __EGPRS__
ED_EXPORT_CALL (long, DECODE_c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD* ED_CONST Destin, long Length));
#endif
/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "GPRS_TEST_MODE_CMD".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_GPRS_TEST_MODE_CMD (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_GPRS_TEST_MODE_CMD* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_GPRS_TEST_MODE_CMD (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_GPRS_TEST_MODE_CMD* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CM_SERVICE_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CM_SERVICE_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CM_SERVICE_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CM_SERVICE_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CM_SERVICE_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CM_SERVICE_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CM_SERVICE_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CM_SERVICE_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CM_SERVICE_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CM_SERVICE_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CM_SERVICE_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CM_SERVICE_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ABORT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ABORT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ABORT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ABORT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ABORT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ABORT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ABORT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ABORT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ABORT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ABORT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IDENTITY_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_IDENTITY_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IDENTITY_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_IDENTITY_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IDENTITY_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IDENTITY_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_IDENTITY_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IDENTITY_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_IDENTITY_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IDENTITY_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "LOCATION_UPDATE_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_LOCATION_UPDATE_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_LOCATION_UPDATE_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "LOCATION_UPDATE_ACCEPT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_LOCATION_UPDATE_ACCEPT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_ACCEPT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_LOCATION_UPDATE_ACCEPT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_ACCEPT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "LOCATION_UPDATE_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_LOCATION_UPDATE_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_LOCATION_UPDATE_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "LOCATION_UPDATE_REJECT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_LOCATION_UPDATE_REJECT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_REJECT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_LOCATION_UPDATE_REJECT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_REJECT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "MM_INFORMATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_MM_INFORMATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MM_INFORMATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_MM_INFORMATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MM_INFORMATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "MM_INFORMATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_MM_INFORMATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_MM_INFORMATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_MM_INFORMATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_MM_INFORMATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TMSI_REALLOCATION_COMMAND".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TMSI_REALLOCATION_COMMAND (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TMSI_REALLOCATION_COMMAND* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TMSI_REALLOCATION_COMMAND (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TMSI_REALLOCATION_COMMAND* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "TMSI_REALLOCATION_COMMAND".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_TMSI_REALLOCATION_COMMAND (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TMSI_REALLOCATION_COMMAND* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_TMSI_REALLOCATION_COMMAND (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TMSI_REALLOCATION_COMMAND* ED_CONST Destin, long Length));

int Match_c_ATTACH_ACCEPT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_ATTACH_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_DETACH_REQUEST_MT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_DETACH_ACCEPT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PTMSI_REALLOCATION_COMMAND (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_AND_CIPHERING_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_AND_CIPHERING_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PIDENTITY_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RAU_ACCEPT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RAU_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PSTATUS (const char* Buffer, ED_CONST long BitOffset);
int Match_c_GMM_INFORMATION (const char* Buffer, ED_CONST long BitOffset);
int Match_c_GPRS_TEST_MODE_CMD (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CM_SERVICE_ACCEPT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CM_SERVICE_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_ABORT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_IDENTITY_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_LOCATION_UPDATE_ACCEPT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_LOCATION_UPDATE_REJECT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_MM_INFORMATION (const char* Buffer, ED_CONST long BitOffset);
int Match_c_TMSI_REALLOCATION_COMMAND (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

