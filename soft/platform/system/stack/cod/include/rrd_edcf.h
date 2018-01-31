/**************************************************************************
	Generated automatically by Codegenix(TM) - (c) 2000-2004 Dafocus
	EnDec version 1.0.75
	GENERATOR: ed-multifile
	http://www.Dafocus.it/
**************************************************************************/

#ifndef __rrd_edcf_h
#define __rrd_edcf_h

#define rrd_USES_MSGSET_CSN1EnDataTypes
#define rrd_USES_MSGSET_DchD
#define rrd_USES_MSGSET_Ccch
#define rrd_USES_MSGSET_UserDefinedDataTypes
#define rrd_ENUM_TDchD_Type\
	,ID_DchD_ASS_CMD,\
	ID_DchD_CHN_MOD_MODIFY,\
	ID_DchD_CHN_REL,\
	ID_DchD_CIP_MOD_CMD,\
	ID_DchD_FRQ_RDF,\
	ID_DchD_HOV_CMD,\
	ID_DchD_PHY_INFO

#define rrd_UNION_TDchD_Type\
		c_ASS_CMD fld_c_ASS_CMD;\
		c_CHN_MOD_MODIFY fld_c_CHN_MOD_MODIFY;\
		c_CHN_REL fld_c_CHN_REL;\
		c_CIP_MOD_CMD fld_c_CIP_MOD_CMD;\
		c_FRQ_RDF fld_c_FRQ_RDF;\
		c_HOV_CMD fld_c_HOV_CMD;\
		c_PHY_INFO fld_c_PHY_INFO;

#define rrd_ENUM_TmmUl_Type

#define rrd_UNION_TmmUl_Type

#define rrd_ENUM_TmmDl_Type

#define rrd_UNION_TmmDl_Type

#define rrd_ENUM_TccUl_Type

#define rrd_UNION_TccUl_Type

#define rrd_ENUM_TccDl_Type

#define rrd_UNION_TccDl_Type

#define rrd_ENUM_TCcch_Type\
	,ID_Ccch_IMM_ASS,\
	ID_Ccch_IMM_ASS_EXT,\
	ID_Ccch_IMM_ASS_REJ

#define rrd_UNION_TCcch_Type\
		c_IMM_ASS fld_c_IMM_ASS;\
		c_IMM_ASS_EXT fld_c_IMM_ASS_EXT;\
		c_IMM_ASS_REJ fld_c_IMM_ASS_REJ;


#endif

