/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-c-recog
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __edre_h
#define __edre_h
#include "ed_c.h"

#include "ed_user.h"
#define ED_MAXSIZE 600
#include "edcf.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group DchD
--------------------------------------------------------------------*/
typedef enum {
	ID_DchD_Unrecognized = 0
#ifdef rrd_ENUM_TDchD_Type
		rrd_ENUM_TDchD_Type
#endif


#ifdef rri_ENUM_TDchD_Type
		rri_ENUM_TDchD_Type
#endif


#ifdef mm_ENUM_TDchD_Type
		mm_ENUM_TDchD_Type
#endif


#ifdef cc_ENUM_TDchD_Type
		cc_ENUM_TDchD_Type
#endif


} TDchD_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group DchD
--------------------------------------------------------------------*/
typedef struct {
	TDchD_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TDchD_Type
		rrd_UNION_TDchD_Type
#endif

#ifdef rri_UNION_TDchD_Type
		rri_UNION_TDchD_Type
#endif

#ifdef mm_UNION_TDchD_Type
		mm_UNION_TDchD_Type
#endif

#ifdef cc_UNION_TDchD_Type
		cc_UNION_TDchD_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TDchD_Data;
#define INIT_TDchD_Data(sp) (ED_RESET_MEM ((sp), sizeof (TDchD_Data)), (sp)->Type=ID_DchD_Unrecognized)
void FREE_TDchD_Data (TDchD_Data* sp);

void SETPRESENT_TDchD_Data(TDchD_Data* sp, TDchD_Type toBeSetPresent);

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group mmUl
--------------------------------------------------------------------*/
typedef enum {
	ID_mmUl_Unrecognized = 0
#ifdef rrd_ENUM_TmmUl_Type
		rrd_ENUM_TmmUl_Type
#endif


#ifdef rri_ENUM_TmmUl_Type
		rri_ENUM_TmmUl_Type
#endif


#ifdef mm_ENUM_TmmUl_Type
		mm_ENUM_TmmUl_Type
#endif


#ifdef cc_ENUM_TmmUl_Type
		cc_ENUM_TmmUl_Type
#endif


} TmmUl_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group mmUl
--------------------------------------------------------------------*/
typedef struct {
	TmmUl_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TmmUl_Type
		rrd_UNION_TmmUl_Type
#endif

#ifdef rri_UNION_TmmUl_Type
		rri_UNION_TmmUl_Type
#endif

#ifdef mm_UNION_TmmUl_Type
		mm_UNION_TmmUl_Type
#endif

#ifdef cc_UNION_TmmUl_Type
		cc_UNION_TmmUl_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TmmUl_Data;
#define INIT_TmmUl_Data(sp) (ED_RESET_MEM ((sp), sizeof (TmmUl_Data)), (sp)->Type=ID_mmUl_Unrecognized)
void FREE_TmmUl_Data (TmmUl_Data* sp);

void SETPRESENT_TmmUl_Data(TmmUl_Data* sp, TmmUl_Type toBeSetPresent);

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group mmDl
--------------------------------------------------------------------*/
typedef enum {
	ID_mmDl_Unrecognized = 0
#ifdef rrd_ENUM_TmmDl_Type
		rrd_ENUM_TmmDl_Type
#endif


#ifdef rri_ENUM_TmmDl_Type
		rri_ENUM_TmmDl_Type
#endif


#ifdef mm_ENUM_TmmDl_Type
		mm_ENUM_TmmDl_Type
#endif


#ifdef cc_ENUM_TmmDl_Type
		cc_ENUM_TmmDl_Type
#endif


} TmmDl_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group mmDl
--------------------------------------------------------------------*/
typedef struct {
	TmmDl_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TmmDl_Type
		rrd_UNION_TmmDl_Type
#endif

#ifdef rri_UNION_TmmDl_Type
		rri_UNION_TmmDl_Type
#endif

#ifdef mm_UNION_TmmDl_Type
		mm_UNION_TmmDl_Type
#endif

#ifdef cc_UNION_TmmDl_Type
		cc_UNION_TmmDl_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TmmDl_Data;
#define INIT_TmmDl_Data(sp) (ED_RESET_MEM ((sp), sizeof (TmmDl_Data)), (sp)->Type=ID_mmDl_Unrecognized)
void FREE_TmmDl_Data (TmmDl_Data* sp);

void SETPRESENT_TmmDl_Data(TmmDl_Data* sp, TmmDl_Type toBeSetPresent);

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group ccUl
--------------------------------------------------------------------*/
typedef enum {
	ID_ccUl_Unrecognized = 0
#ifdef rrd_ENUM_TccUl_Type
		rrd_ENUM_TccUl_Type
#endif


#ifdef rri_ENUM_TccUl_Type
		rri_ENUM_TccUl_Type
#endif


#ifdef mm_ENUM_TccUl_Type
		mm_ENUM_TccUl_Type
#endif


#ifdef cc_ENUM_TccUl_Type
		cc_ENUM_TccUl_Type
#endif


} TccUl_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group ccUl
--------------------------------------------------------------------*/
typedef struct {
	TccUl_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TccUl_Type
		rrd_UNION_TccUl_Type
#endif

#ifdef rri_UNION_TccUl_Type
		rri_UNION_TccUl_Type
#endif

#ifdef mm_UNION_TccUl_Type
		mm_UNION_TccUl_Type
#endif

#ifdef cc_UNION_TccUl_Type
		cc_UNION_TccUl_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TccUl_Data;
#define INIT_TccUl_Data(sp) (ED_RESET_MEM ((sp), sizeof (TccUl_Data)), (sp)->Type=ID_ccUl_Unrecognized)
void FREE_TccUl_Data (TccUl_Data* sp);

void SETPRESENT_TccUl_Data(TccUl_Data* sp, TccUl_Type toBeSetPresent);

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group ccDl
--------------------------------------------------------------------*/
typedef enum {
	ID_ccDl_Unrecognized = 0
#ifdef rrd_ENUM_TccDl_Type
		rrd_ENUM_TccDl_Type
#endif


#ifdef rri_ENUM_TccDl_Type
		rri_ENUM_TccDl_Type
#endif


#ifdef mm_ENUM_TccDl_Type
		mm_ENUM_TccDl_Type
#endif


#ifdef cc_ENUM_TccDl_Type
		cc_ENUM_TccDl_Type
#endif


} TccDl_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group ccDl
--------------------------------------------------------------------*/
typedef struct {
	TccDl_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TccDl_Type
		rrd_UNION_TccDl_Type
#endif

#ifdef rri_UNION_TccDl_Type
		rri_UNION_TccDl_Type
#endif

#ifdef mm_UNION_TccDl_Type
		mm_UNION_TccDl_Type
#endif

#ifdef cc_UNION_TccDl_Type
		cc_UNION_TccDl_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TccDl_Data;
#define INIT_TccDl_Data(sp) (ED_RESET_MEM ((sp), sizeof (TccDl_Data)), (sp)->Type=ID_ccDl_Unrecognized)
void FREE_TccDl_Data (TccDl_Data* sp);

void SETPRESENT_TccDl_Data(TccDl_Data* sp, TccDl_Type toBeSetPresent);

/*--------------------------------------------------------------------
	Enum representing a tag for every message in group Ccch
--------------------------------------------------------------------*/
typedef enum {
	ID_Ccch_Unrecognized = 0
#ifdef rrd_ENUM_TCcch_Type
		rrd_ENUM_TCcch_Type
#endif


#ifdef rri_ENUM_TCcch_Type
		rri_ENUM_TCcch_Type
#endif


#ifdef mm_ENUM_TCcch_Type
		mm_ENUM_TCcch_Type
#endif


#ifdef cc_ENUM_TCcch_Type
		cc_ENUM_TCcch_Type
#endif


} TCcch_Type;

/*--------------------------------------------------------------------
	Union containing data for every message in group Ccch
--------------------------------------------------------------------*/
typedef struct {
	TCcch_Type Type;
	int ProtocolDiscriminator;
	int MessageType;
	union {
#ifdef rrd_UNION_TCcch_Type
		rrd_UNION_TCcch_Type
#endif

#ifdef rri_UNION_TCcch_Type
		rri_UNION_TCcch_Type
#endif

#ifdef mm_UNION_TCcch_Type
		mm_UNION_TCcch_Type
#endif

#ifdef cc_UNION_TCcch_Type
		cc_UNION_TCcch_Type
#endif

		char Dummy; /* Avoids empty unions! */
	} Data;
} TCcch_Data;
#define INIT_TCcch_Data(sp) (ED_RESET_MEM ((sp), sizeof (TCcch_Data)), (sp)->Type=ID_Ccch_Unrecognized)
void FREE_TCcch_Data (TCcch_Data* sp);

void SETPRESENT_TCcch_Data(TCcch_Data* sp, TCcch_Type toBeSetPresent);


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_DchD (const char* buffer, TDchD_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_DchD (const char* buffer, int len, TDchD_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_mmUl (const char* buffer, TmmUl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_mmUl (const char* buffer, int len, TmmUl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_mmDl (const char* buffer, TmmDl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_mmDl (const char* buffer, int len, TmmDl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_ccUl (const char* buffer, TccUl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_ccUl (const char* buffer, int len, TccUl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_ccDl (const char* buffer, TccDl_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_ccDl (const char* buffer, int len, TccDl_Data* data);
#endif


/*-------------------------------------------------------------------------------
	Returns number of consumed bits (>=0) if recognized. <0 otherwise.
	Parameter 'len' is expressed in octets, while 'bitLen' is in bits.

-------------------------------------------------------------------------------*/
int SetDecode_Ccch (const char* buffer, TCcch_Data* data, int bitLen);

/* Maintains compatibility with Encodix 1.0.33 and before */
#ifndef ED_EXCLUDE_1_0_33_COMPATIBILITY
	int Decode_Ccch (const char* buffer, int len, TCcch_Data* data);
#endif

#include "rrd_edre.h"

#include "rri_edre.h"

#include "mm_edre.h"

#include "cc_edre.h"

#ifdef __cplusplus
	};
#endif
#endif

