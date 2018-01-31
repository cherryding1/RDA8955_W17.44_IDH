/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-encodeco
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __mm_mmUl_h
#define __mm_mmUl_h
#include "ed_lib.h"
#include "mm_ed_c.h"
#include "ed_user.h"
#include "edcf.h"


#ifdef __cplusplus
	extern "C" {
#endif

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "ATTACH_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_ATTACH_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_ATTACH_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "ATTACH_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_ATTACH_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_ATTACH_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_ATTACH_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_ATTACH_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "DETACH_REQUEST_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_DETACH_REQUEST_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_REQUEST_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_DETACH_REQUEST_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_REQUEST_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "DETACH_REQUEST_MO".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_DETACH_REQUEST_MO (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_DETACH_REQUEST_MO* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_DETACH_REQUEST_MO (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_DETACH_REQUEST_MO* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_AND_CIPHERING_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_AND_CIPHERING_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_AND_CIPHERING_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_AND_CIPHERING_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_AND_CIPHERING_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_AND_CIPHERING_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_AND_CIPHERING_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PIDENTITY_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PIDENTITY_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PIDENTITY_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PIDENTITY_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PIDENTITY_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PIDENTITY_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PIDENTITY_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PIDENTITY_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PIDENTITY_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PIDENTITY_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RAU_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RAU_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RAU_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RAU_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RAU_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RAU_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "RAU_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_RAU_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_RAU_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "RAU_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_RAU_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_RAU_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_RAU_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_RAU_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_AUTHENTICATION_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "AUTHENTICATION_FAILURE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_FAILURE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_FAILURE* ED_CONST Source));

ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_FAILURE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_FAILURE* ED_CONST Source));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "AUTHENTICATION_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_AUTHENTICATION_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_AUTHENTICATION_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CM_REESTABLISHMENT_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CM_REESTABLISHMENT_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_REESTABLISHMENT_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CM_REESTABLISHMENT_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_REESTABLISHMENT_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CM_REESTABLISHMENT_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CM_REESTABLISHMENT_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_REESTABLISHMENT_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CM_REESTABLISHMENT_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_REESTABLISHMENT_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CM_SERVICE_ABORT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CM_SERVICE_ABORT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_ABORT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CM_SERVICE_ABORT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_ABORT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CM_SERVICE_ABORT".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CM_SERVICE_ABORT (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_ABORT* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CM_SERVICE_ABORT (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_ABORT* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "CM_SERVICE_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_CM_SERVICE_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_CM_SERVICE_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "CM_SERVICE_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_CM_SERVICE_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_CM_SERVICE_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_CM_SERVICE_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_CM_SERVICE_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IDENTITY_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_IDENTITY_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IDENTITY_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_IDENTITY_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IDENTITY_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IDENTITY_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_IDENTITY_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IDENTITY_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_IDENTITY_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IDENTITY_RESPONSE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "IMSI_DETACH_INDICATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_IMSI_DETACH_INDICATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IMSI_DETACH_INDICATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_IMSI_DETACH_INDICATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMSI_DETACH_INDICATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "IMSI_DETACH_INDICATION".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_IMSI_DETACH_INDICATION (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_IMSI_DETACH_INDICATION* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_IMSI_DETACH_INDICATION (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_IMSI_DETACH_INDICATION* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "LOCATION_UPDATE_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_LOCATION_UPDATE_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_LOCATION_UPDATE_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "LOCATION_UPDATE_REQUEST".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_LOCATION_UPDATE_REQUEST (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_LOCATION_UPDATE_REQUEST* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_LOCATION_UPDATE_REQUEST (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_LOCATION_UPDATE_REQUEST* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "TMSI_REALLOCATION_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_TMSI_REALLOCATION_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TMSI_REALLOCATION_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_TMSI_REALLOCATION_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TMSI_REALLOCATION_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "TMSI_REALLOCATION_COMPLETE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_TMSI_REALLOCATION_COMPLETE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_TMSI_REALLOCATION_COMPLETE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_TMSI_REALLOCATION_COMPLETE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_TMSI_REALLOCATION_COMPLETE* ED_CONST Destin, long Length));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for "PAGING_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_c_PAGING_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PAGING_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_c_PAGING_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PAGING_RESPONSE* ED_CONST Destin, long Length));
ED_EXPORT_CALL (long, ENCODE_BODY_c_AUTHENTICATION_AND_CIPHERING_FAILURE(char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_FAILURE* ED_CONST Source));

/*----------------------------------------------------------------------------
	Encoding/decoding functions for body of "PAGING_RESPONSE".
	It returns number of bits used or <0 in case of error.
----------------------------------------------------------------------------*/
ED_EXPORT_CALL (long, ENCODE_BODY_c_PAGING_RESPONSE (char* ED_CONST Buffer, ED_CONST long BitOffset, const c_PAGING_RESPONSE* ED_CONST Source));

ED_EXPORT_CALL (long, DECODE_BODY_c_PAGING_RESPONSE (const char* ED_CONST Buffer, ED_CONST long BitOffset, c_PAGING_RESPONSE* ED_CONST Destin, long Length));

ED_EXPORT_CALL (long, ENCODE_c_AUTHENTICATION_AND_CIPHERING_FAILURE(char* ED_CONST Buffer, ED_CONST long BitOffset, const c_AUTHENTICATION_AND_CIPHERING_FAILURE* ED_CONST Source));

int Match_c_ATTACH_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_DETACH_REQUEST_MO (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_AND_CIPHERING_RESPONSE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PIDENTITY_RESPONSE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RAU_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_RAU_COMPLETE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_AUTHENTICATION_RESPONSE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CM_REESTABLISHMENT_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CM_SERVICE_ABORT (const char* Buffer, ED_CONST long BitOffset);
int Match_c_CM_SERVICE_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_IDENTITY_RESPONSE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_IMSI_DETACH_INDICATION (const char* Buffer, ED_CONST long BitOffset);
int Match_c_LOCATION_UPDATE_REQUEST (const char* Buffer, ED_CONST long BitOffset);
int Match_c_TMSI_REALLOCATION_COMPLETE (const char* Buffer, ED_CONST long BitOffset);
int Match_c_PAGING_RESPONSE (const char* Buffer, ED_CONST long BitOffset);

#ifdef __cplusplus
	};
#endif
#endif

