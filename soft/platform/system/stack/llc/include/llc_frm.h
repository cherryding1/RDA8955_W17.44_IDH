//------------------------------------------------------------------------------
//  $Log: llc_frm.h,v $
//  Revision 1.4  2006/05/23 10:19:27  fch
//  Change to set array of pointer in ROM
//  Update Copyright
//
//  Revision 1.3  2006/01/20 10:16:20  fch
//  Suppress FCS error trace when dummy frame (2B)
//
//  Revision 1.2  2005/12/29 17:38:46  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : LLC_FRM.H
--------------------------------------------------------------------------------

  Scope      : Defintions about LLC frames

  History    :
--------------------------------------------------------------------------------
  Mar 25 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __LLC_FRM_H__
#define __LLC_FRM_H__



#ifdef __LLC_FRM_C__
#define DefExtern
#else
#define DefExtern extern
#endif


//============================================================================
//  FRAME STRUCTURE :
//
//   |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//
//    -----------------------------------------------
//   | PD  | C/R |  X  |  X  |         SAPI          |  ADDRESS Field
//    -----------------------------------------------
//   |                   ....                        |
//   |        Variable (1 to 36 bytes)               |  CONTROL Field
//   |                   ....                        |
//    -----------------------------------------------
//   |                   ....                        |
//   |        Variable (max N201 bytes)              |  INFORMATION Field
//   |                   ....                        |
//    -----------------------------------------------
//   |                                               |
//   |                  3 bytes                      |  FCS Field
//   |                                               |
//    -----------------------------------------------
//
//============================================================================


//============================================================================
//  CONSTANTS
//============================================================================

#define LLC_VALID_FRM       0
#define LLC_INVALID_FRM     0xFF


      //-------------------------------------------------------
      //  Constants for the address and control fields of a frame
      //-------------------------------------------------------

  // Offset for the control field from the frame beginning
#define LLC_CTRL_FIELD_OFF      1

  // S function bits in I frame are in control field [2] 
#define LLC_I_FRM_S_BIT_OFF     LLC_CTRL_FIELD_OFF+2

  // S function bits in S frame are in control field [1] 
#define LLC_S_FRM_S_BIT_OFF     LLC_CTRL_FIELD_OFF+1


  // Frame fields minimum size definition
#define LLC_ALL_FRM_ADR_SIZE      1

#define LLC_ALL_FRM_FCS_SIZE      3

DefExtern const u8 llc_DummyFcs[LLC_ALL_FRM_FCS_SIZE]
#ifdef __LLC_FRM_C__
 = { 0x2B, 0x2B, 0x2B }
#endif
;


#define LLC_I_FRM_CTRL_SIZE       3
#define LLC_S_FRM_CTRL_SIZE       2
#define LLC_UI_FRM_CTRL_SIZE      2
#define LLC_U_FRM_CTRL_SIZE       1

#define LLC_UI_FRM_HEADER_SIZE    (LLC_ALL_FRM_ADR_SIZE + LLC_UI_FRM_CTRL_SIZE)
#define LLC_U_FRM_HEADER_SIZE     (LLC_ALL_FRM_ADR_SIZE + LLC_U_FRM_CTRL_SIZE)
#define LLC_I_FRM_HEADER_SIZE     (LLC_ALL_FRM_ADR_SIZE + LLC_I_FRM_CTRL_SIZE)
#define LLC_S_FRM_HEADER_SIZE     (LLC_ALL_FRM_ADR_SIZE + LLC_S_FRM_CTRL_SIZE)


  // Frame fields maximum size definition

    // For U frame, this is the case of the response to an XID command 
    // (without L3 because in this case L3 alloactes memory)
#define LLC_U_FRM_MAX_SIZE        (LLC_U_FRM_HEADER_SIZE + 15 +LLC_ALL_FRM_FCS_SIZE)




  // PD bit of the address field
#define LLC_PD_MSK      0x80    // bit 7 of the address field
#define LLC_PD_VAL      0

  // Bit C/R of the address field
#define LLC_CR_MSK            0x40    // bit 6 of the address field
#define LLC_CMD_SGSN_TO_MS    0x40
#define LLC_CMD_MS_TO_SGSN    0x00
#define LLC_RSP_SGSN_TO_MS    0x00
#define LLC_RSP_MS_TO_SGSN    0x40

  // SAPI information of the address field
#define LLC_SAPI_MSK    0X0F


  // Frame format identification masks
#define LLC_I_FRM_FORMAT_MSK    0x80
#define LLC_S_FRM_FORMAT_MSK    0x40
#define LLC_UI_FRM_FORMAT_MSK   0x20

  // Frame format identification values
#define LLC_I_FRM_FORMAT_ID     0x00
#define LLC_S_FRM_FORMAT_ID     0x80
#define LLC_UI_FRM_FORMAT_ID    0xC0
#define LLC_U_FRM_FORMAT_ID     0xE0

  // Mask for the S & M function bits
#define LLC_FRM_S_BIT_MSK       0x03


      //-------------------------------------------------------
      //  Constants for the I and S Frame type
      //-------------------------------------------------------
#define LLC_RR      0x00
#define LLC_ACK     0x01
#define LLC_RNR     0x02
#define LLC_SACK    0x03

#define LLC_I_FORMAT    0x10
#define LLC_S_FORMAT    0x20

#define LLC_FRM_TYP_MSK     0x03
#define LLC_FRM_FORMAT_MSK  0x30

// Supervisory frame name
DefExtern const ascii * const SupervisoryFrmName[]
#ifdef __LLC_FRM_C__
 =
{

  "RR",
  "ACK",
  "RNR",
  "SACK",
}
#endif
;

#define LLC_I_FRM_A_BIT_MSK     0X40  // Mask to get the A bit in an I frame
#define LLC_S_FRM_A_BIT_MSK     0X20  // Mask to get the A bit in an S frame


  // Constant to process sequence number N(S)
#define LLC_I_FRM_NS_MSB_OFF    1


      //-------------------------------------------------------
      //  Constants for the I SACK Frame type
      //-------------------------------------------------------
  // Offset to find the K field (bitmap length) from the frame beginning
#define LLC_I_FRM_K_OFF   4
#define LLC_K_MSK         0x1F  // 5 lsb of the byte


      //-------------------------------------------------------
      //  Constants for the S Frame type
      //-------------------------------------------------------
  // Max size of a S frame (SACK)
#define LLC_S_FRM_MAX_SIZE  LLC_S_FRM_HEADER_SIZE + 32 + LLC_ALL_FRM_FCS_SIZE


      //-------------------------------------------------------
      //  Constants for the UI Frame type
      //-------------------------------------------------------
  // Offset to find the PM field (protected mode) from the frame beginning
#define LLC_UI_FRM_PM_OFF       2
#define LLC_PM_MSK              0x01  // lsb of the byte
#define LLC_UI_FRM_PROTECTED    0x01  // PM = 1 => frame protected on all the info field

  // Offset to find the E field (encryption bit) from the frame beginning
#define LLC_UI_FRM_E_OFF        2
#define LLC_E_MSK               0x02  // 2nd bit of the byte
#define LLC_UI_FRM_CIPHERED     0x02  // E = 1 => frame ciphered

  // Constants to process sequence number N(U)
#define LLC_UI_FRM_NU_MSB_OFF   1
#define LLC_NU_MSB_MSK          0x07  // 3 lsb bits of the byte
#define LLC_UI_FRM_NU_LSB_OFF   2
#define LLC_NU_LSB_MSK          0xFC  // 6 msb bits of the byte
#define LLC_NU_LSB_NB           6     // 6 N(U) bits in the second byte


      //-------------------------------------------------------
      //  Constants for the U Frame type
      //-------------------------------------------------------
#define LLC_NULL    0x00
#define LLC_DM      0x01
#define LLC_DISC    0x04
#define LLC_UA      0x06
#define LLC_SABM    0x07
#define LLC_FRMR    0x08
#define LLC_XID     0x0B


DefExtern const ascii * const llc_UFrmName[]
#ifdef __LLC_FRM_C__
 =
{
  "NULL",
  "DM",
  "",
  "",
  "DISC",
  "",
  "UA",
  "SABM",
  "FRMR",
  "",
  "",
  "XID",
}
#endif
;


  // Information field length for a U FRMR frame type is 10 bytes
#define LLC_U_FRM_FRMR_INFO_LEN   10

  // Mask to get the M bits
#define LLC_FRM_M_BIT_MSK       0x0F

  // Offset to find the P/F field from the frame beginning
#define LLC_U_FRM_PF_OFF        1
#define LLC_PF_MSK              0x10  // 5th bit of the byte

  // Minimum necessary size for an EstabReq including LLC XID 
  // Adr + Ctrl + 3 bytes for T200 + 2 for N200 + 2 bytes max for the
  // L3Xid type & length
#define LLC_U_SABM_OFF_MIN      LLC_ALL_FRM_ADR_SIZE + LLC_U_FRM_CTRL_SIZE + 5 + 2

#define LLC_UA_XID_MAX          21
#define LLC_U_UA_MAX_SIZE       LLC_ALL_FRM_ADR_SIZE + LLC_U_FRM_CTRL_SIZE + LLC_UA_XID_MAX + LLC_ALL_FRM_FCS_SIZE

  //------------------------
  // Constants for XID frame
  //------------------------
    
    // Offset to find the first parameter type for an XID frame
#define LLC_XID_1ST_PAR_OFF   2

    // Parameter types possible value
#define LLC_XID_VERS        0
#define LLC_XID_IOV_UI      1
#define LLC_XID_IOV_I		2
#define LLC_XID_T200		3
#define LLC_XID_N200		4
#define LLC_XID_N201_U      5
#define LLC_XID_N201_I      6
#define LLC_XID_MD          7
#define LLC_XID_MU          8
#define LLC_XID_KD          9
#define LLC_XID_KU          10
#define LLC_XID_L3          11
#define LLC_XID_RESET       12
#define LLC_XID_TYPE_MAX    LLC_XID_RESET

#ifdef __LLC_DEBUG__
DefExtern const ascii * const llc_XidParName[]
#ifdef __LLC_FRM_C__
 =
{
  "Version",
  "IOV-UI",
  "IOV-I",
  "T200",
  "N200",
  "N201_U",
  "N201_I",
  "mD",
  "mU",
  "kD",
  "kU",
  "L3",
  "Reset",
}
#endif
;
#endif // __LLC_DEBUG__


#define LLC_XID_XL_MSK      0x80
#define LLC_XID_TYPE_MSK    0x7C

#define LLC_XID_VALID       TRUE
#define LLC_XID_INVALID     FALSE

  //-------------------------
  // Constants for FRMR frame
  //-------------------------
#define LLC_W1    ( 1 << 0 )
#define LLC_W2    ( 1 << 1 )
#define LLC_W3    ( 1 << 2 )
#define LLC_W4    ( 1 << 3 )


//============================================================================
//  !!!!  BE CARREFUL, DO NOT CHANGE THE FOLLOWING ENUM  !!!!
//============================================================================
enum
{
    // I frame types
  I_FRM,
  
    // S frame types
  S_FRM,
  
    // UI frame types
  UI,
  
    // U frame types
  U_DM,
  U_DISC,
  U_UA,
  U_SABM,
  U_FRMR,
  U_XID,
  U_UNDEFINED,
  
  LLC_NB_FRM_TYP
};



//============================================================================
//  MACROS
//============================================================================
  // Retrieve the K field (bitmap length indicator) from an I SACK frame type
#define LLC_FRM_GET_K(_AdrPtr)     (*((_AdrPtr) + LLC_I_FRM_K_OFF) & LLC_K_MSK)

  // Retrieve the PM (protected mode) bit from an UI frame
#define LLC_FRM_GET_PM(_AdrPtr)    (*((_AdrPtr) + LLC_UI_FRM_PM_OFF) & LLC_PM_MSK)

  // Retrieve the E bit (Encryption)from an UI frame
#define LLC_FRM_GET_E(_AdrPtr)     (*((_AdrPtr) + LLC_UI_FRM_E_OFF) & LLC_E_MSK)

  // Modulus on Sequence Number is 512
#define LLC_SN_MOD(_x)    ( (_x) & 511 )

  // Get the C/R bit from a frame
#define LLC_FRM_GET_CR(_AdrPtr)    (*(_AdrPtr) & LLC_CR_MSK )

  // Get the P/F bit from an U frame control field
#define LLC_FRM_GET_PF(_AdrPtr)    (*((_AdrPtr) + LLC_U_FRM_PF_OFF) & LLC_PF_MSK)

  // There is at least one XID parameter and the first one is reset  
#define LLC_IS_XID_RST_FIRST(_AdrPtr)  ( *((_AdrPtr) + LLC_XID_1ST_PAR_OFF) == (LLC_XID_RESET << 2) )
                                
  // Get the Type of a XID parameter in a byte
#define LLC_GET_XID_TYPE(_Ptr)  ( ( (*_Ptr) & LLC_XID_TYPE_MSK) >> 2)

  // Inform if XL field is setted in the XID parameter header
#define LLC_IS_XL(_Hdr)         ( (_Hdr) & LLC_XID_XL_MSK )


#undef DefExtern

#endif // endif __LLC_FRM_H__
