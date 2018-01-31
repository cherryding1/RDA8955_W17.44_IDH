/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : cmn_dec.h 
--------------------------------------------------------------------------------

  Scope      : Defintion of Constants and Macro for Air Interface messages
               decoding

  History    :
--------------------------------------------------------------------------------
  Oct 06 03  |  PFA   | Creation            
  Apr 19 04  |  MCE   | Addition of CMN_TST_PD
================================================================================
*/
#ifndef __CMN_DEC_H__
#define __CMN_DEC_H__

/*
** Definition of L or H for certain Csn.1 Ie
** Return 0 if L, 1 if H
*/
#define CMN_LH(Val, Pos)  ( ((1<<Pos) & (Val)) == ((1<<Pos) & (0x2b) ) ? 0 : 1)
#define CMN_LH_BIT(Bit, Pos) ( (((0x2b >> (7-Pos)) & 1) == Bit) ? 0 : 1 )

/*
** Definition of MASK for differents len 
*/
#define CMN_VBITMASK(Len) ((1<<(Len)) -1)

#define CMN_MASKL1  CMN_VBITMASK(1)
#define CMN_MASKL2  CMN_VBITMASK(2)
#define CMN_MASKL3  CMN_VBITMASK(3)
#define CMN_MASKL4  CMN_VBITMASK(4)
#define CMN_MASKL5  CMN_VBITMASK(5)
#define CMN_MASKL6  CMN_VBITMASK(6)
#define CMN_MASKL7  CMN_VBITMASK(7)
#define CMN_MASKL8  CMN_VBITMASK(8)
#define CMN_MASKL9  CMN_VBITMASK(9)
#define CMN_MASKL10 CMN_VBITMASK(10)
#define CMN_MASKL11 CMN_VBITMASK(11)
#define CMN_MASKL12 CMN_VBITMASK(12)
#define CMN_MASKL13 CMN_VBITMASK(13)
#define CMN_MASKL14 CMN_VBITMASK(14)
#define CMN_MASKL15 CMN_VBITMASK(15)
#define CMN_MASKL16 CMN_VBITMASK(16)
#define CMN_MASKL17 CMN_VBITMASK(17)
#define CMN_MASKL18 CMN_VBITMASK(18)
#define CMN_MASKL19 CMN_VBITMASK(19)
#define CMN_MASKL20 CMN_VBITMASK(20)
#define CMN_MASKL21 CMN_VBITMASK(21)
#define CMN_MASKL22 CMN_VBITMASK(22)
#define CMN_MASKL23 CMN_VBITMASK(23)
#define CMN_MASKL24 CMN_VBITMASK(24)
#define CMN_MASKL25 CMN_VBITMASK(25)
#define CMN_MASKL26 CMN_VBITMASK(26)
#define CMN_MASKL27 CMN_VBITMASK(27)
#define CMN_MASKL28 CMN_VBITMASK(28)
#define CMN_MASKL29 CMN_VBITMASK(29)
#define CMN_MASKL30 CMN_VBITMASK(30)
#define CMN_MASKL31 CMN_VBITMASK(31)
#define CMN_MASKL32 CMN_VBITMASK(32)


/*
** Extract ((u8) value) of  len bits from an u8 (Byte),
**     Fisrt bit is at position "Pos" 
**     Last  bit is at position "Pos+ Len" 
*/

#define CMN_EXTRACTU8(byte, pos, mask) ((byte& (mask<<(pos) )) >> (pos))
#define CMN_GETU8(byte, pos, mask)     CMN_EXTRACTU8(byte, pos, mask)

/*
** Extract len bits from val
**     Fisrt bit is at position "Pos" 
**     Last  bit is at position "Pos+ Len" 
*/

#define CMN_EXTRACT(val, pos, mask)  ( (val & (mask<<(pos))) >> (pos))
#define CMN_GET(val, pos, mask)      CMN_EXTRACT(val, pos, mask)

/*
** Put len bits (val) in an octet
**     Fisrt bit is at position "Pos" 
**     Last  bit is at position "Pos+ Len" 
*/     
#define CMN_PUT(val, pos, mask)  ( (val & mask) << pos ) 

/*
** Protocol Descr and Msg Type            
*/
#define CMN_GET_PD(byte)          CMN_EXTRACTU8(byte, 0, CMN_MASKL4)
#define CMN_GET_MT(byte)          CMN_EXTRACTU8(byte, 0, CMN_MASKL6)
#define CMN_GET_LEN(byte)         CMN_EXTRACTU8(byte, 2, CMN_MASKL6)

#define CMN_CC_PD                3     // CC Protocol Discriminator 
#define CMN_MM_PD                5     // MM Protocol Discriminator 
#define CMN_RR_PD                6     // RR Protocol Discriminator          
#define CMN_MMP_PD               8     // GMM Protocol Discriminator 
#define CMN_SMS_PD               9     // SMS Protocol Discriminator 
#define CMN_SM_PD               10     // SM Protocol Discriminator 
#define CMN_SS_PD               11     // SS Protocol Discriminator 
#define CMN_TST_PD              15     // Test procedures described in TS 04.14


#endif // __CMN_DEC_H__
