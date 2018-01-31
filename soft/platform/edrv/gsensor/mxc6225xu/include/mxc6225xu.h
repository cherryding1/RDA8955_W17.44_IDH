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



#ifndef _mxc6225xu_M_H_
#define _mxc6225xu_M_H_





// ============================================================================
// mxc6225xu_REG_MAP_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    mxc6225xu_ADDR_XOUT                         = 0x00,
    mxc6225xu_ADDR_YOUT                         = 0x01,
    mxc6225xu_ADDR_STATUS                         = 0x02,
    mxc6225xu_ADDR_DETECTION                         = 0x04,
    mxc6225xu_ADDR_QTY                          = 0x05
} MMA7760FC_REG_MAP_T;


//Xout
#define mxc6225xu_XOUT(n)           (((n)&0xff)<<0)
#define mxc6225xu_XOUT_MASK         (0xff)
//Yout
#define mxc6225xu_YOUT(n)           (((n)&0xff)<<0)
#define mxc6225xu_YOUT_MASK         (0xff)

//Status
#define mxc6225xu_STATUS(n)           (((n)&0xff)<<0)
#define mxc6225xu_ZOUT_MASK         (0xff)





// status
#define mxc6225xu_INTOR0             (1<<0)
#define mxc6225xu_INTOR1             (1<<1)
#define mxc6225xu_INTOR             (3<<0)

#define mxc6225xu_INTORI0             (1<<2)
#define mxc6225xu_INTORI1             (1<<3)
#define mxc6225xu_INTORI             (3<<2)

#define mxc6225xu_TILT             (1<<4)

#define mxc6225xu_INTSH0             (1<<5)
#define mxc6225xu_INTSH1             (1<<6)
#define mxc6225xu_INTSH             (3<<5)

#define mxc6225xu_INT             (1<<7)


// detection // write_only


#define mxc6225xu_ORC0            (1<<0)
#define mxc6225xu_ORC1         (1<<1)
#define mxc6225xu_ORC       (3<<0)


#define mxc6225xu_SHC1             (1<<3)
#define mxc6225xu_SHC0           (1<<2)
#define mxc6225xu_SHC          (3<<3)


#define mxc6225xu_SHTH1             (1<<5)
#define mxc6225xu_SHTH0            (1<<4)
#define mxc6225xu_SHTH            (3<<4)

#define mxc6225xu_SHM            (1<<6)

#define mxc6225xu_PD           (1<<7)

#define mxc6225xu_adder_xadc 0
#define mxc6225xu_adder_yadc 1
#define mxc6225xu_adder_status 2
#define mxc6225xu_adder_detection 4


#define  mxc6225xu_ALERT (1<<4)

typedef struct
{
    INT8 x_adc; //   the range is -128 to +127
    INT8 y_adc; // the range is -128 to +127
    UINT8 status; //tilt status
} GSS_ACC_DATE_T;


#define ACC_0G_X        (2048)
#define ACC_1G_X        (2048+512)
#define ACC_MINUS1G_X   (2048-512)
#define ACC_0G_Y        (2048)
#define ACC_1G_Y        (2048+512)
#define ACC_MINUS1G_Y   (2048-512)
#define ACC_0G_Z            (0)
#define ACC_1G_Z            (0)
#define ACC_MINUS1G_Z   (0)

enum
{
    DTOS_CTRL_POWER = 0x80,
    DTOS_CTRL_SHAKEMODE = 0x40,
    DTOS_CTRL_SHAKETHRESHOLD = 0x30,
    DTOS_CTRL_SHAKECOUNT = 0x0C,
    DTOS_CTRL_ORIENTCOUNT = 0x03,
    DTOS_CTRL_ALL = 0xFF
};

//Acceleration sensor category
typedef enum
{
    MXC_SENSOR_UNKOWN,
    MXC_SENSOR_2D_XVMP,
    MXC_SENSOR_2D_DTOS
} mxc_sensor_category_t;


#endif








