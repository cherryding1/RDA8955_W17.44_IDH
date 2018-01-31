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


#ifndef _TGT_DDR_FREQ_CFG_H_
#define _TGT_DDR_FREQ_CFG_H_

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)

#ifndef TGT_DDR_TIMING
// the vcore is 8 bits.  The low 0- 7 bits set the volt value , and  the 7 bit set the mode.   0 means LDO mode , 1 means DCDC mode.
//

//Vcore   LDO ACTIVE             LDO LP_MODE              DCDC ACTIVE         DCDC LP MODE
//4Bh[11:8]                      4Bh[15:12]               PWM 2Fh[15:12]      PFM 2Fh[11:8]
//vbuck1_ldo_bit_act             vbuck1_ldo_bit_lp        Vbuck1_ibit_nlp     Vbuck1_ibit_lp
//0000    1.630                   1.600                   0.602               0.737
//0001    1.563                   1.535                   0.738               0.893 (8809e2 recommend)
//0010    1.497                   1.470                   0.878               1.056
//0011    1.430                   1.404                   0.951               1.143
//0100    1.361                   1.337     (default)     1.024               1.231
//0101    1.295                   1.271                   1.093               1.309
//0110    1.229                   1.206                   1.162               1.390
//0111    1.162                   1.141                   1.253               1.495
//1000    1.092   (default)       1.072                   1.332  default      1.586
//1001    1.025                   1.007                   1.432               1.707
//1010    0.959                   0.942                   1.494               1.780
//1011    0.893                   0.877 (recommed)        1.565               1.862
//1100    0.823                   0.809                   1.649               1.958
//1101    0.757                   0.743                   1.697               2.013
//1110    0.691                   0.679                   1.749               2.073
//1111    0.624                   0.613                   1.807               2.139

//----__PWM            ----------- PFM_LP_MODE
//***** U01*********U02******* U01*********U 02
//0     0.602       0.751   0.737       0.907
//*1    0.738       0.851   0.893       1.023
//*2    0.878       0.901   1.056       1.08
//*3    0.951       1.077   1.143       1.281
//*4    1.024       1.102   1.231       1.31
//*5    1.093       1.126   1.309       1.338
//*6    1.162       1.151   1.39        1.366
//*7    1.253       1.176   1.495       1.396
//*8    1.332       1.201   1.586       1.427
//*9    1.432       1.226   1.707       1.457
//*10   1.494       1.252   1.78        1.486
//*11   1.565       1.276   1.862       1.514
//*12   1.649       1.301   1.958       1.544
//*13   1.697       1.327   2.013       1.573
//*14   1.749       1.402   2.073       1.659
//*15   1.807       1.476   2.139       1.745

#ifdef CHIP_DIE_8955_U01
#include "tgt_ddr_freq_cfg_8955_u01.h"
#else
#include "tgt_ddr_freq_cfg_8955_u02.h"
#endif // CHIP_DIE_8955_U01

#define TGT_DDR_USE_FREQ(m, n)      m ## _ ## n

#ifdef CHIP_DIE_8955_U01
#define TGT_DDR_TIMING \
{ \
    TGT_DDR_USE_FREQ(TGT_DDR_LDO7, 52), \
    TGT_DDR_USE_FREQ(TGT_DDR_LDO6, 156), \
    TGT_DDR_USE_FREQ(TGT_DDR_DCDC6, 52), \
    TGT_DDR_USE_FREQ(TGT_DDR_DCDC8, 156), \
}
#else
#define TGT_DDR_TIMING \
{ \
    TGT_DDR_USE_FREQ(TGT_DDR_LDO7, 52), \
    TGT_DDR_USE_FREQ(TGT_DDR_LDO6, 156), \
    TGT_DDR_USE_FREQ(TGT_DDR_DCDC6, 52), \
    TGT_DDR_USE_FREQ(TGT_DDR_DCDC12, 156), \
}
#endif // CHIP_DIE_8955_U01

#endif // !TGT_DDR_TIMING
#endif // CHIP_DIE_8955

#endif //_TGT_APP_CFG_H_

