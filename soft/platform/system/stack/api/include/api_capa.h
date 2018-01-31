//================================================================================
//  File       : api_capa.h
//--------------------------------------------------------------------------------
//  Scope      : Capabilities configuration
//  History    :
//--------------------------------------------------------------------------------
//  Dec 31 04  |  FCH   | Creation
//================================================================================

#ifndef __API_CAPA_H__
#define __API_CAPA_H__

#ifdef __EGPRS__
#define RR_TRANS 0
#define MM_TRANS 1
#endif
void api_ComputeMsNwkCap    ( void );
u16  api_ComputeMsRACap     ( u8 );
#ifdef __EGPRS__
u16 api_ComputeMsRACapFun ( u8 *DstPara, u8 Dstlen ,u8 * PriorAccessTechType,u8 length, bool module );
#endif

//==================================================================
// Common Constants
//==================================================================
#define API_ME_R98_OLDER  0
#define API_ME_R99_LATER  1


//==================================================================
// Constants for MS Network Capability (24.008)
//==================================================================
#define API_MSNWKCAP_SIZE         9

#define API_SMS_VIA_GPRS_CHAN     0x20
#define API_SMS_VIA_DEDI_CHAN     0x40


//==================================================================
// Constants for MS Radio Access Capability (24.008)
//==================================================================
#define API_MSRACAP_SIZE      52

  // Access Technology Type
#define API_CAPA_GSM_900P  0
#define API_CAPA_GSM_900E  1
#define API_CAPA_GSM_900R  2
#define API_CAPA_DCS_1800  3
#define API_CAPA_PCS_1900  4
#define API_CAPA_GSM_450   5
#define API_CAPA_GSM_480   6
#define API_CAPA_GSM_850   7

  // Controlled Early classmark Sending
#define API_ES_SUPPORTED      1
#define API_ES_NOTSUPPORTED   0

  // Pseudo Synchronization
#define API_PS_SUPPORTED      1
#define API_PS_NOTSUPPORTED   0

  // VGCS
#define API_VGCS_SUPPORTED      1
#define API_VGCS_NOTSUPPORTED   0

  // VBS
#define API_VBS_SUPPORTED      1
#define API_VBS_NOTSUPPORTED   0

  // HSCSD Multi Slot Class 
#define API_HSCSD_MULTISLOT_SUPPORTED      1
#define API_HSCSD_MULTISLOT_NOTSUPPORTED   0

  // GPRS Extended Dynamic Allocation Capability
#define API_GPRS_EXT_DYN_ALLOC_SUPPORTED      1
#define API_GPRS_EXT_DYN_ALLOC_NOTSUPPORTED   0

  // ECSD Multi Slot Class 
#define API_ECSD_MULTISLOT_SUPPORTED      1
#define API_ECSD_MULTISLOT_NOTSUPPORTED   0

  // EGPRS Multi Slot Class 
#define API_EGPRS_MULTISLOT_SUPPORTED      1
#define API_EGPRS_MULTISLOT_NOTSUPPORTED   0

#ifdef __EGPRS__
// GPRS Extended Dynamic Allocation Capability
#define API_EGPRS_EXT_DYN_ALLOC_SUPPORTED      1
#define API_EGPRS_EXT_DYN_ALLOC_NOTSUPPORTED   0
#endif

  // DTM GPRS
#define API_DTM_GPRS_SUPPORTED      1
#define API_DTM_GPRS_NOTSUPPORTED   0

  // DTM EGPRS
#define API_DTM_EGPRS_SUPPORTED      1
#define API_DTM_EGPRS_NOTSUPPORTED   0

  // 8PSK power capability
#define API_8PSK_UL_SUPPORTED      1
#define API_8PSK_UL_NOTSUPPORTED   0

  // COMPACT Interference Measurement Capability
#define API_COMPACT_INTF_MEAS_SUPPORTED      1
#define API_COMPACT_INTF_MEAS_NOTSUPPORTED   0

  // UMTS FDD Radio Access Technology Capability
#define API_UMTS_FDD_SUPPORTED      1
#define API_UMTS_FDD_NOTSUPPORTED   0

  // UMTS 3.84 Mcps TDD Radio Access Technology Capability
#define API_UMTS_TDD_SUPPORTED      1
#define API_UMTS_TDD_NOTSUPPORTED   0
        
  // CDMA 2000 Radio Access Technology Capability
#define API_CDMA2000_SUPPORTED      1
#define API_CDMA2000_NOTSUPPORTED   0


//UMTS 1.28 Mcps TDD Radio Access Technology Capability
#define API_UMTS128_SUPPORTED      1
#define API_UMTS128_NOTSUPPORTED   0

//GERAN Feature Package 1
#define API_GERAN1_SUPPORTED      1
#define API_GERAN1_NOTSUPPORTED   0

//Modulation based multislot class support
#define API_MODULATION_MULTISLOT_SUPPORT   1
#define API_MODULATION_MULTISLOT_NOTSUPPORT   0



#ifdef __API_CAPA_C__
#define DefExtern
#else
#define DefExtern extern
#endif

extern bool sxs_IsExtULTBFSupported(void);
#if !(STK_MAX_SIM==1)
DefExtern u8  api_MsNwkCap[STK_MAX_SIM][API_MSNWKCAP_SIZE];   // 24.008
DefExtern u8  api_MsRACap[STK_MAX_SIM][API_MSRACAP_SIZE];     // 24.008
DefExtern u16 api_LenMsRACap[STK_MAX_SIM];
#else
DefExtern u8  api_MsNwkCap[API_MSNWKCAP_SIZE];   // 24.008
DefExtern u8  api_MsRACap[API_MSRACAP_SIZE];     // 24.008
DefExtern u16 api_LenMsRACap;
#endif

#undef DefExtern

#endif
