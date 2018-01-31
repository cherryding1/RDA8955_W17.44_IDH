////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/edrv/trunk/aud/ti/src/audp_ti.h $ //
//    $Author: jingyuli $                                                        //
//    $Date: 2016-06-07 16:37:31 +0800 (Tue, 07 Jun 2016) $                     //
//    $Revision: 32438 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file audp_ti.h                                                        //
/// That file is the private header for the ti implementation of the AUD  //
/// interface.                                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////






// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================
/// Address on the I2C bus.
#define TI_ADDR 0x1a // 0x1a // 0x1b in case of CS=1

/// Ti's register Map
#define REG_TI_LINE_VOL_L              0x00
#define TI_LRS_ON                   (1<<8)
#define TI_LRS_OFF                  (0<<8)
#define TI_LIM_ON                   (1<<7)
#define TI_LIM_OFF                  (0<<7)
#define LIV_VALUE(v)                ((v)&0x1f)

#define REG_TI_LINE_VOL_R              0x01
#define TI_RLS_ON                   (1<<8)
#define TI_RLS_OFF                  (0<<8)
#define TI_RIM_ON                   (1<<7)
#define TI_RIM_OFF                  (0<<7)
#define RIV_VALUE(v)                ((v)&0x1f)

#define REG_TI_HEAD_VOL_L              0x02
#define TI_LZC_ON                   (1<<7)
#define TI_LZC_OFF                  (0<<7)
#define TI_LHV_VALUE(v)             ((v)&0x7f)
#define TI_LHV_MUTE                  0x30

#define REG_TI_HEAD_VOL_R              0x03
#define TI_RZC_ON                   (1<<7)
#define TI_RZC_OFF                  (0<<7)
#define TI_RHV_VALUE(v)             ((v)&0x7f)
#define TI_RHV_MUTE                  0x30

#define REG_TI_ANA_AUD_CTRL            0x04
#define TI_SIDETONE_0DB             0x120
#define TI_SIDETONE_MINUS_6DB       0x20
#define TI_SIDETONE_MINUS_9DB       0x60
#define TI_SIDETONE_MINUS_12DB      0xa0
#define TI_SIDETONE_MINUS_18DB      0xe0
#define TI_SIDETONE_OFF             (0<<5)
#define TI_ANA_DAC_ON               (1<<4)
#define TI_ANA_DAC_OFF              (0<<4)
#define TI_ANA_BYP_ON               (1<<3)
#define TI_ANA_BYP_OFF              (0<<3)
#define TI_INSEL_MIC                (1<<2)
#define TI_INSEL_LINE               (0<<2)
#define TI_MICM_ON                  (1<<1)
#define TI_MICM_OFF                 (0<<1)
#define TI_MICB_ON                  (1<<0)
#define TI_MICB_OFF                 (0<<0)

#define REG_TI_DIG_AUD_CTRL            0x05
#define TI_DACM_ON                  (1<<3)
#define TI_DACM_OFF                 (0<<3)
#define TI_DEEMP_OFF                (0<<1)
#define TI_DEEMP_32KHZ              (1<<1)
#define TI_DEEMP_44KHZ              (2<<1)
#define TI_DEEMP_48KHZ              (3<<1)
#define TI_ADCHP_ON                 (0<<0)
#define TI_ADCHP_OFF                (1<<0)

#define REG_TI_POWER_CTRL              0x06
#define TI_POWER_OFF                (1<<7)
#define TI_POWER_ON                 (0<<7)
#define TI_CLK_OFF                  (1<<6)
#define TI_CLK_ON                   (0<<6)
#define TI_OSC_OFF                  (1<<5)
#define TI_OSC_ON                   (0<<5)
#define TI_OUT_OFF                  (1<<4)
#define TI_OUT_ON                   (0<<4)
#define TI_DAC_OFF                  (1<<3)
#define TI_DAC_ON                   (0<<3)
#define TI_ADC_OFF                  (1<<2)
#define TI_ADC_ON                   (0<<2)
#define TI_MIC_OFF                  (1<<1)
#define TI_MIC_ON                   (0<<1)
#define TI_LINE_OFF                 (1<<0)
#define TI_LINE_ON                  (0<<0)
#define TI_POWER_ALL_OFF            0xff

#define REG_TI_DIG_FORMAT              0x07
#define TI_MODE_MASTER              (1<<6)
#define TI_MODE_SLAVE               (0<<6)
#define TI_LRSWAP_ON                (1<<5)
#define TI_LRSWAP_OFF               (0<<5)
#define TI_LRP_R_LRCIN_LOW          (1<<4)
#define TI_LRP_R_LRCIN_HIGH         (0<<4)
#define TI_LRP_MSB_ON_2BLCK         (1<<4)
#define TI_LRP_MSB_ON_1BLCK          (0<<4)
#define TI_IWL_16BIT                (0<<2)
#define TI_IWL_20BIT                (1<<2)
#define TI_IWL_24BIT                (2<<2)
#define TI_IWL_32BIT                (3<<2)
#define TI_FOR_DSP                  (3<<0)
#define TI_FOR_I2S                  (2<<0)
#define TI_FOR_L_ALIGN              (1<<0)
#define TI_FOR_R_ALIGN              (0<<0)

#define REG_TI_SAMPLE_RATE             0x08
#define TI_CLKIN_DIV_ON                 (1<<7)
#define TI_CLKIN_DIV_OFF                (0<<7)
#define TI_CLKOUT_DIV_ON                (1<<6)
#define TI_CLKOUT_DIV_OFF               (0<<6)
#define TI_SR_VALUE(v)              (((v)&0xf)<<2)
#define TI_BOSR_FAST                (1<<1)
#define TI_BOSR_SLOW                (0<<1)
#define TI_CLK_MODE_USB             (1<<0)
#define TI_CLK_MODE_NORM            (0<<0)

#define REG_TI_DIG_ACT                 0x09
#define TI_ACT_ON                   (1<<0)
#define TI_ACT_OFF                  (0<<0)

#define REG_TI_RESET                   0x0f
#define TI_RESET                    0

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//  __attribute__((section (".hal.globalvars.c")))

// =============================================================================
//  FUNCTIONS
// =============================================================================

