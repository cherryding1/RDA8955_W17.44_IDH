//------------------------------------------------------------------------------
//  $Log: rrp_enc.h,v $
//  Revision 1.4  2006/05/22 14:39:08  oth
//  Update the copyright
//
//  Revision 1.3  2006/02/08 08:50:47  oth
//  Addition for the handling of the PCCO
//
//  Revision 1.2  2005/12/29 17:39:12  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rrp_enc.h
--------------------------------------------------------------------------------

  Scope      : Encoding structures

  History    :
--------------------------------------------------------------------------------
  Jan 11 05  |  OTH   | Creation            
================================================================================
*/

#ifndef __RRP_ENC_H__
#define __RRP_ENC_H__

// System includes
#include "sxs_type.h"




#define RRP_PCCFAIL_MT 0
#define RRP_PRESREQ_MT 5




typedef struct
{
 bool IsUplink;
 u8   Value;
} rrp_GTFI_t;




typedef struct
{
 bool IsPresent;
 u8   Value;
} rrp_TN_t;




typedef struct
{
 u8  Value[52];
 u16 BitLen;
} rrp_MsRACap_t;


#ifdef __EGPRS__
typedef struct
{
 bool Tn_Present;
 bool Module_Present; /*FALSE:GPSK; TRUE:8PSK*/
 u8   Value;
} rrp_BepTn_t;


typedef struct
{
	rrp_BepTn_t BepTn[8];
}rrp_EgprsBepTsQualMeas_BepMeas_t;// rrp_EgprsBepTsQualMeas_t;

typedef struct
{
    bool Gmsk_Present;
    bool Psk8_Present;	
    u8  GmskMeanBep;   /* GMSK_MEAN_BEP*/ 
    u8  GmskCvBep;   /* GMSK_CV_BEP */ 
    u8	Psk8MeanBep;	   /* 8PSK _MEAN_BEP:bit7-bit3*/
    u8	Psk8CvBep;	   /* 8PSK _CV_BEP */
} rrp_EgprsBepQualMeas_t;
typedef struct
{
 bool          TLLI_Present;
 bool          MsRACap2_Present;
 u32           TLLI;
 rrp_GTFI_t    GTFI;
 rrp_MsRACap_t MsRACap2;
} rrp_AMsRACap_t;
 //< Additional MS Radio Access Capabilities message content > ::=
 //	{ 0 < Global TFI : < Global TFI IE > > 1 < TLLI : < TLLI IE > > }
 //	< MS Radio Access 2 Capability : < MS Radio Access Capability 2 IE > >
 //  MS Radio Access 2 Capability will cope from  API

//c_Additional_MS_Radio_Access_Capabilities_Msg
s16 ENCODE_c_Additional_MS_Radio_Access_Capabilities_Msg( u8 * Buffer, const u32 BitOffset,
                         const rrp_AMsRACap_t* Source );
#endif

//	| 1		-- Additional contents for Release 1999
//	{ 0 | 1 < EGPRS BEP Link Quality Measurements :< EGPRS BEP Link Quality Measurements IE >> }
//  { 0 | 1 < EGPRS Timeslot Link Quality Measurements :< EGPRS Timeslot Link Quality Measurements IE >>}
//	{ 0 | 1 < PFI: bit(7) > }	bool PFI_Present;	u8 PFI;
//	< ADDITIONAL MS RAC INFORMATION AVAILABLE : bit (1) >
//	< RETRANSMISSION OF PRR : bit (1) >
//	< padding bits > } ;
typedef struct
{
 bool          AccessType_Present;
 bool          TLLI_Present;
 bool          MsRACap2_Present;
 bool          ChangeMark_Present;
 bool          SignVar_Present;
 bool          PFI_Present;
 #ifdef __EGPRS__
	bool Egprs_Present;
 bool	   EgprsBepQualMeas_Present;
bool	   EgprsBepTsQualMeas_Present;
bool    EgprsTsQualMeasILevelTN_Present;
u8 	   AMsRacAvail;	/**/
u8 	   PrrRetran;
 #endif
 u8            AccessType;
 u8            ChangeMark;
 u8            CValue;
 u8            SignVar;
 u8            PFI;
 u8            ChannelReqDesc [3];
 u32           TLLI;
 rrp_GTFI_t    GTFI;
 rrp_TN_t      ILevelTN[8];
#ifdef __EGPRS__

 rrp_EgprsBepQualMeas_t EgprsBepQualMeas;
 rrp_EgprsBepTsQualMeas_BepMeas_t EgprsBepTsQualMeas;
 
    

//	bool EGPRS_BEP_Link_Quality_Present;
//	c_EGPRS_BEP_Link_Quality_Meas_IE EGPRS_BEP_Link_Quality;
//	bool EGPRS_Timeslot_Link_Quality_Present;
//	c_EGPRS_Timeslot_Link_Quality_Meas_IE EGPRS_Timeslot_Link_Quality; 
//	bool	ADD_MS_RAC_INFOR_AVAILABLE_Present;
//	u8 ADD_MS_RAC_INFOR_AVAILABLE;
//	bool RETRANSMISSION_OF_PRR_Present;
//	u8 RETRANSMISSION_OF_PRR;
#endif
 rrp_MsRACap_t MsRACap2;

} rrp_PResReq_t;

s16 rrp_EncodePResReq( u8 * Buffer, const u32 BitOffset,
                       const rrp_PResReq_t* Source );




#define RRP_FREQ_NOT_IMPLEMENTED   0
#define RRP_NO_RESP_ON_TARGET_CELL 1
#define RRP_ACC_REJ_ON_TARGET_CELL 2
#define RRP_STDBY_STATE            5
#define RRP_FORCE_2_STDBY          6




typedef struct
{
 u8  Cause;
 u8  Bsic;
 u16 Arfcn;
 u32 TLLI;

} rrp_PCCFail_t;

s16 rrp_EncodePCCFail( u8 * Buffer, const u32 BitOffset,
                       const rrp_PCCFail_t* Source );

#endif // __RRP_ENC_H__
