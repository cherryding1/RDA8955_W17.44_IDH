/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/





















#ifndef XCV_H
#define XCV_H

#include "drv_xcv_calib.h"

//--------------------------------------------------------------------//
//                                                                    //
//                    GPIO and TCO Assignement                        //
//                                                                    //
//--------------------------------------------------------------------//

//--------------------------------------------------------------------//
//                                                                    //
//                   RF SPI Commands Related                          //
//                                                                    //
//--------------------------------------------------------------------//
// General bitfield access macros
#define RDA6220_MASK(field)         (RDA6220_ ## field ## _MASK)
#define RDA6220_OFFSET(field)       (RDA6220_ ## field ## _OFFSET)
#define RDA6220_FIELD(dword, field) (((dword) >> RDA6220_OFFSET(field)) & RDA6220_MASK(field))
#define RDA6220_SET_FIELD(dword, field, value)      \
                    dword = (((dword) & ~(RDA6220_MASK(field) << RDA6220_OFFSET(field)))    \
                    | (((value) & RDA6220_MASK(field)) << RDA6220_OFFSET(field)))
#define RDA6220_WRITE_FMT(addr, data) (((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET) |\
                                       ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET))

#define RDA6220_READ_FMT(addr, data)  ( ((addr & RDA6220_ADDR_MASK) << RDA6220_ADDR_OFFSET)|\
                                        ((data & RDA6220_DATA_MASK) << RDA6220_DATA_OFFSET)|\
                                                                                     1<<31);


// Rda6220 RF Transceiver:
//  1 write command is:
//  1 operand bit = "0" for write + 6 bits for address + 18 bits for data = 25 bits
//           0AAAAAAD | DDDDDDDD | DDDDDDDD | D0000000
//      MSB    Byte0     Byte 1     Byte 2      Byte 3  LSB
//
//  1 read command is:
//  1 operand bit = "1" for read + 6 bits for address + 18 bits for data = 25 bits
//           1AAAAAAD | DDDDDDDD | DDDDDDDD | D0000000
//      MSB    Byte0     Byte 1       Byte 2     Byte 3  LSB
//

#define RDA6220_ONE_CMD_BYTE_QTY        4
#define RDA6220_ADDR_MASK               0x3F
#define RDA6220_ADDR_OFFSET             25
#define RDA6220_DATA_MASK               0x3FFFF
#define RDA6220_DATA_OFFSET             7

#define BYTE_SIZE                       8
#define BYTE_MASK                       0xff


// RF commands context structure
typedef struct
{
    UINT32 freqCtrl;       // @ = set arfcn and band
    UINT32 gain;  // @ = set analog & digital gain
    UINT32 digAfc;      // @ = set digital Afc
} XCV_COMMANDS_T;


// RDA6220 Registers
#define RDA6220_RX_GAIN_REG                   0x01
#define RDA6220_FREQ_CTRL_REG                 0x08
#define RDA6220_CDAC_REG                      0x19
#define RDA6220_AFC_REG                       0x1f
#define RDA6220_SYS_CTRL_REG                  0x30

#define RDA6220_TX_RAMP_0_REG                 0x03
#define RDA6220_TX_RAMP_1_REG                 0x03
#define RDA6220_TX_RAMP_2_REG                 0x03
#define RDA6220_TX_RAMP_3_REG                 0x03

#define RDA6220_GSM_RAMP_1_REG                0x10
#define RDA6220_GSM_RAMP_2_REG                0x11
#define RDA6220_GSM_RAMP_3_REG                0x12

#define RDA6220_RAMP_CURVE_MASK               0x07
#define RDA6220_RAMP_CURVE_OFFSET               12

#define RDA6220_RAMP_FACTOR_MASK             0x3ff
#define RDA6220_RAMP_FACTOR_OFFSET               0


#define RDA6220_ARFCN_MASK                   0x3ff
#define EGSM_ARFCN_QTY                         174
#define DCS_ARFCN_QTY                          374
#define PCS_ARFCN_QTY                          299

#define RDA6220_MIN_IN_POWER                   110 // in -dBm
#define RDA6220_MAX_IN_POWER                    16  // in -dBm
#define XCVER_AGC_TARGET                        75

// AFC Parameters
#define RDA6220_CDAC_DEFAULT       (XCV_PARAM(XTAL))
#define RDA6220_CDAC_MAX            0xff
#define RDA6220_CDAC_MIN            0x0
#define RDA6220_CDAC_OFFSET         0x18

// MID value is changed from 0x2000 to 0x1500 to allow more space for temperature increasing
#define RDA6220_AFC_MID             0x1500 // 0x2000
#define RDA6220_AFC_HRANGE          0xa00
#define RDA6220_AFC_MAX             0x3d00
#define RDA6220_AFC_MIN             0x200
//#define RDA6220_AFC_HRANGE          0x1fff
//#define RDA6220_AFC_MAX             0x3fff
//#define RDA6220_AFC_MIN             0x0

#define RDA6220_AFC_MASK            0x3fff
#define RDA6220_AFC_BOUND_HIGH     (RDA6220_AFC_MID+RDA6220_AFC_HRANGE)
#define RDA6220_AFC_BOUND_LOW      (RDA6220_AFC_MID-RDA6220_AFC_HRANGE)

/* -----------------------------------------------------------------
 | From  To(dbm) |  Tab[]  |  LNA + AGain  |Digital |TotalVoltGain |
 ------------------------------------------------
 |     -109      | Tab[0] |     xxxxxx     |  xxx   |  xxxxxxxxxx  |
 ------------------------------------------------
 |     -108      | Tab[1] |     xxxxxx     |  xxx   |  xxxxxxxxxx  |
 -------------------------------------------------------------------
 |     ......      ......       .....        .....     ..........  |
 |                                             |
 |     ......      ......       .....        .....     ..........  |
 -------------------------------------------------------------------
 |     -16       | Tab[93]|     xxxxxx     |  xxx   |  xxxxxxxxxx  |
 ------------------------------------------------
 |     -15       | Tab[94]|     xxxxxx     |  xxx   |  xxxxxxxxxx  |
 ------------------------------------------------ ------------------*/
// AGC table
#define AGC_QTY 95
// APC table
#define APCLB_QTY 15 // GSM850 & GSM900: PCL 5 - 29 (5 - 33 dB)
#define APCHB_QTY 16 // DCS1800 & PCS1900: PCL 0 - 15 (0 - 30 dB)


typedef struct
{
    UINT8    analogGain;
    UINT8    digGain;
    UINT16   totGain;
} XCV_RX_GAIN_T;


#define EGSM_AGC_DEFAULT {\
{0x0 , 0x16 ,  5},\
{0x0 , 0x18 ,  6},\
{0x0 , 0x1a ,  7},\
{0x0 , 0x20 ,  8},\
{0x0 , 0x22 ,  9},\
{0x0 , 0x24 , 10},\
{0x0 , 0x26 , 11},\
{0x1 , 0x05 , 12},\
{0x1 , 0x07 , 13},\
{0x1 , 0x09 , 14},\
{0x1 , 0x0b , 15},\
{0x1 , 0x11 , 16},\
{0x1 , 0x13 , 17},\
{0x1 , 0x15 , 18},\
{0x1 , 0x17 , 19},\
{0x1 , 0x19 , 20},\
{0x1 , 0x1b , 21},\
{0x1 , 0x21 , 22},\
{0x1 , 0x23 , 23},\
{0x1 , 0x25 , 24},\
{0x2 , 0x01 , 25},\
{0x2 , 0x03 , 26},\
{0x2 , 0x05 , 27},\
{0x2 , 0x07 , 28},\
{0x2 , 0x09 , 29},\
{0x2 , 0x0b , 30},\
{0x2 , 0x11 , 31},\
{0x2 , 0x13 , 32},\
{0x2 , 0x15 , 33},\
{0x3 , 0x12 , 34},\
{0x3 , 0x14 , 35},\
{0x3 , 0x16 , 36},\
{0x3 , 0x18 , 37},\
{0x3 , 0x1a , 38},\
{0x3 , 0x20 , 39},\
{0x3 , 0x22 , 40},\
{0x3 , 0x24 , 41},\
{0x3 , 0x26 , 42},\
{0x3 , 0x28 , 43},\
{0x3 , 0x2a , 44},\
{0x4 , 0x04 , 45},\
{0x4 , 0x06 , 46},\
{0x4 , 0x08 , 47},\
{0x4 , 0x0a , 48},\
{0x4 , 0x10 , 49},\
{0x4 , 0x12 , 50},\
{0x4 , 0x14 , 51},\
{0x4 , 0x16 , 52},\
{0x4 , 0x18 , 53},\
{0x4 , 0x1a , 54},\
{0x4 , 0x20 , 55},\
{0x4 , 0x22 , 56},\
{0x4 , 0x24 , 57},\
{0x4 , 0x26 , 58},\
{0x4 , 0x28 , 59},\
{0x5 , 0x03 , 60},\
{0x5 , 0x05 , 61},\
{0x5 , 0x07 , 62},\
{0x5 , 0x09 , 63},\
{0x5 , 0x0b , 64},\
{0x5 , 0x11 , 65},\
{0x5 , 0x13 , 66},\
{0x5 , 0x15 , 67},\
{0x5 , 0x17 , 68},\
{0x5 , 0x19 , 69},\
{0x5 , 0x1b , 70},\
{0x5 , 0x21 , 71},\
{0x5 , 0x23 , 72},\
{0x5 , 0x25 , 73},\
{0x6 , 0x13 , 74},\
{0x6 , 0x15 , 75},\
{0x6 , 0x17 , 76},\
{0x6 , 0x19 , 77},\
{0x6 , 0x1b , 78},\
{0x6 , 0x21 , 79},\
{0x7 , 0x0a , 80},\
{0x7 , 0x10 , 81},\
{0x7 , 0x12 , 82},\
{0x7 , 0x14 , 83},\
{0x7 , 0x16 , 84},\
{0x7 , 0x18 , 85},\
{0x7 , 0x1a , 86},\
{0x7 , 0x20 , 87},\
{0x7 , 0x22 , 88},\
{0x7 , 0x24 , 89},\
{0x7 , 0x26 , 90},\
{0x7 , 0x28 , 91},\
{0x7 , 0x2a , 92},\
{0x7 , 0x30 , 93},\
{0x7 , 0x32 , 94},\
{0x7 , 0x34 , 95},\
{0x7 , 0x36 , 96},\
{0x7 , 0x38 , 97},\
{0x7 , 0x3a , 98},\
{0x7 , 0x40 , 99}\
}

#define DCS_AGC_DEFAULT {\
{0x0 , 0x1a , 5 },\
{0x0 , 0x20 , 6 },\
{0x0 , 0x22 , 7 },\
{0x0 , 0x24 , 8 },\
{0x0 , 0x26 , 9 },\
{0x0 , 0x28 , 10},\
{0x0 , 0x2a , 11},\
{0x1 , 0x05 , 12},\
{0x1 , 0x07 , 13},\
{0x1 , 0x09 , 14},\
{0x1 , 0x0b , 15},\
{0x1 , 0x11 , 16},\
{0x1 , 0x13 , 17},\
{0x1 , 0x15 , 18},\
{0x1 , 0x17 , 19},\
{0x1 , 0x19 , 20},\
{0x1 , 0x1b , 21},\
{0x1 , 0x21 , 22},\
{0x1 , 0x23 , 23},\
{0x1 , 0x25 , 24},\
{0x2 , 0x08 , 25},\
{0x2 , 0x0a , 26},\
{0x2 , 0x10 , 27},\
{0x2 , 0x12 , 28},\
{0x2 , 0x14 , 29},\
{0x2 , 0x16 , 30},\
{0x2 , 0x18 , 31},\
{0x2 , 0x1a , 32},\
{0x2 , 0x20 , 33},\
{0x3 , 0x0a , 34},\
{0x3 , 0x10 , 35},\
{0x3 , 0x12 , 36},\
{0x3 , 0x14 , 37},\
{0x3 , 0x16 , 38},\
{0x3 , 0x18 , 39},\
{0x3 , 0x1a , 40},\
{0x3 , 0x20 , 41},\
{0x3 , 0x22 , 42},\
{0x3 , 0x24 , 43},\
{0x3 , 0x26 , 44},\
{0x3 , 0x28 , 45},\
{0x4 , 0x0a , 46},\
{0x4 , 0x10 , 47},\
{0x4 , 0x12 , 48},\
{0x4 , 0x14 , 49},\
{0x4 , 0x16 , 50},\
{0x4 , 0x18 , 51},\
{0x4 , 0x1a , 52},\
{0x4 , 0x20 , 53},\
{0x4 , 0x22 , 54},\
{0x4 , 0x24 , 55},\
{0x4 , 0x26 , 56},\
{0x4 , 0x28 , 57},\
{0x4 , 0x2a , 58},\
{0x4 , 0x30 , 59},\
{0x4 , 0x32 , 60},\
{0x5 , 0x03 , 61},\
{0x5 , 0x05 , 62},\
{0x5 , 0x07 , 63},\
{0x5 , 0x09 , 64},\
{0x5 , 0x0b , 65},\
{0x5 , 0x11 , 66},\
{0x5 , 0x13 , 67},\
{0x5 , 0x15 , 68},\
{0x5 , 0x17 , 69},\
{0x6 , 0x04 , 70},\
{0x6 , 0x06 , 71},\
{0x6 , 0x08 , 72},\
{0x6 , 0x0a , 73},\
{0x6 , 0x10 , 74},\
{0x6 , 0x12 , 75},\
{0x6 , 0x14 , 76},\
{0x6 , 0x16 , 77},\
{0x6 , 0x18 , 78},\
{0x6 , 0x1a , 79},\
{0x7 , 0x02 , 80},\
{0x7 , 0x04 , 81},\
{0x7 , 0x06 , 82},\
{0x7 , 0x08 , 83},\
{0x7 , 0x0a , 84},\
{0x7 , 0x10 , 85},\
{0x7 , 0x12 , 86},\
{0x7 , 0x14 , 87},\
{0x7 , 0x16 , 88},\
{0x7 , 0x18 , 89},\
{0x7 , 0x1a , 90},\
{0x7 , 0x20 , 91},\
{0x7 , 0x22 , 92},\
{0x7 , 0x24 , 93},\
{0x7 , 0x26 , 94},\
{0x7 , 0x28 , 95},\
{0x7 , 0x2a , 96},\
{0x7 , 0x30 , 97},\
{0x7 , 0x32 , 98},\
{0x7 , 0x34 , 99}\
}

typedef struct
{
    UINT16    rampSet;
} XCV_APC_T;

// ramp factore for PCL 5 - 29 (33 - 5 dBm)
#define EGSM_APC_DEFAULT {\
{0x354},\
{0x2c8},\
{0x258},\
{0x200},\
{0x1bc},\
{0x185},\
{0x15c},\
{0x13a},\
{0x11f},\
{0x109},\
{0x0f6},\
{0x0e7},\
{0x0da},\
{0x0cf},\
{0x0c6},\
}

// ramp factore for PCL 0 - 15 (31 - 0 dBm)
#define DCS_APC_DEFAULT {\
{0x360},\
{0x290},\
{0x1f9},\
{0x1b3},\
{0x183},\
{0x158},\
{0x138},\
{0x11c},\
{0x107},\
{0x0f4},\
{0x0e6},\
{0x0da},\
{0x0d0},\
{0x0c8},\
{0x0c0},\
{0x0ba},\
}

#endif // XCV_H
