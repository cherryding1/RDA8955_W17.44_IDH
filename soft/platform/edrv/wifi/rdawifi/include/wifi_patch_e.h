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

#ifndef __WIFI_PATCH_E_H__
#define __WIFI_PATCH_E_H__

const UINT8 wifi_core_data_patch_e[][2] =
{
    { 0x28, 0x1a} ,
    { 0x29, 0x0d},
    { 0x35, 0x1e},
    { 0x4c, 0x90},
    { 0x4d, 0x38},
    { 0x39, 0x07},
    { 0xe4, 0xf5},
    { 0x21, 0x00},
    //default 0
    { 0x23, 0x10},
    { 0x48, 0x0e},
    { 0x25, 0x00},
    { 0x20, 0xa8},
    { 0x3f, 0x05},
    { 0x41, 0x37},
    { 0x42, 0x40},
    { 0x5b, 0xa9},
};




//#define   WF_PAT_CFG_2012_04_15
//#define  WF_PAT_CFG_2012_05_19
#define WF_PAT_CFG_2012_11_13
const UINT32 wifi_core_notch_patch_e[][2] =
{
#if 0
#ifdef WF_PAT_CFG_2012_04_15    /*pta config*/
    {0x50000800,0xFC003E05}, //tx_pri hi bits ctrl&mgmt package
    {0x50000804,0x00000000}, //tx_pri hi bits                                                                                                                as hi pri
    {0x50000808,0xA500001B}, //sig_mode and protect time
    {0x5000080c,0x000001C0}, //sigWire mode
    {0x50000810,0xFFCC0F01}, //Lut bt
    {0x50000814,0xFFFF0F33}, //Lut wf
    {0x50000818,0x00FF0001}, //antSel0 for wl_rx
    {0x5000081C,0xFF000F00}, //antSel1 for wl_tx
    {0x50000820,0xFF000F00}, //antSel2 for wl_pa
    //{0x50000838,0xFFFFFFFF}, //rx_pri low bits as high pri
    //{0x5000083C,0xFFFFFFFF}, //rx_pri high  bits as high pri
#endif

    /*end pta config*/

    // miaodefang 2012-05-19
    //{  0x00106b6c, 0x00000002 }, // scan channel 13
    // {  0x30010004, 0x0000f77c },    //intn config
    // {  0x30010010, 0x00007dff },    //intn config
    //item111:ver_b_wf_dig_2011_10_09
    {  0x30010000, 0x780369AF },   //disable tports wait  100ms;
    {  0x30000010, 0x7000FFFF },//wait 500ms;
    // miaodefang 2012-05-19

    {  0x5000050c, 0x00008000 },// for association power save

    //{  0x50000808, 0x65000013 }, // disable prerx_priority;pta config
    //{  0x50000810, 0xFFCD0F01 },  //rx beacon priority

#else
//add 2012-09-14
//add 2012-11-23 by houzhen
#ifdef WF_PAT_CFG_2012_11_13
    {0x50000800, 0xFC003E05},
    {0x50000804, 0x00000000},
    {0x50000808, 0xA5000013},
    {0x5000080c, 0x000001C0},
    {0x50000810, 0xFFCC0F01},
    {0x50000814, 0xFFFF0F33},
    {0x50000818, 0x00FF3033},
    {0x5000081C, 0xFF000F00},
    {0x50000820, 0xFF000F00},
    {0x50000824, 0x0000C0CC},
    {0x50000828, 0x00100F10},
    {0x50000834, 0x00003032},
    {0x50000838, 0xFFFFFFFF},
    {0x5000083C, 0xFFFFFFFF},
#endif


#ifdef WF_PAT_CFG_2012_05_19
    {0x50000800,0xFC003E05},
    {0x50000804,0x00000000},
    {0x50000808,0xA5000013},   //no pre_active protect
    {0x5000080c,0x000001C0},
    {0x50000810,0xFFCC0F01},
    {0x50000814,0xFFFF0F03},   //0xFFFF0F33
    {0x50000818,0x00FF0001},
    {0x5000081C,0xFF000F00},
    {0x50000820,0xFF000F00},
    {0x50000824,0x0000F0FE},
    {0x50000828,0x00100F10},
    {0x50000838,0xFFFFFFFF},
    {0x5000083C,0xFFFFFFFF},
#endif

#ifdef  FORCE_WF
    {0x50000800,0xFC003E05},
    {0x50000804,0x00000000},
    {0x50000838,0xF8003f2A},
    {0x5000083c,0x00000003},
    {0x50000808,0xfe00001b},
    {0x50000810,0x00000000},
    {0x50000814,0x00000000},
    {0x50000818,0x00000000},
    {0x5000081C,0x00000000},
    {0x50000820,0x00000000},
    {0x50000824,0xffffffff},
    {0x50000828,0x00100F10},
#endif

#ifdef WF_PAT_CFG_2012_04_15    /*pta config*/
    {0x50000800,0xFC003E05}, //tx_pri hi bits ctrl&mgmt package
    {0x50000804,0x00000000}, //tx_pri hi bits                                                                                                                as hi pri
    {0x50000808,0xA500001B}, //sig_mode and protect time
    {0x5000080c,0x000001C0}, //sigWire mode
    {0x50000810,0xFFCC0F01}, //Lut bt
    {0x50000814,0xFFFF0F33}, //Lut wf
    {0x50000818,0x00FF0001}, //antSel0 for wl_rx
    {0x5000081C,0xFF000F00}, //antSel1 for wl_tx
    {0x50000820,0xFF000F00}, //antSel2 for wl_pa
    //{0x50000838,0xFFFFFFFF}, //rx_pri low bits as high pri
    //{0x5000083C,0xFFFFFFFF}, //rx_pri high  bits as high pri
#endif

    {  0x30010004, 0x0000f77c },    //intn config
    {  0x30010010, 0x00007dff },    //intn config
    //item111:ver_b_wf_dig_2011_10_09
    {  0x30010000, 0x780369AF },   //disable tports wait  100ms;
    {  0x30000010, 0x7000FFFF },//wait 500ms;
    // {  0x5000050c, 0x00008000 },// for association power save
    {  0x5000050c, 0x00003900 },//2012-12-19, match with core_init_data from miaodf 2012-11-28
#endif
//add notch patch
    // miaodefang 2012-05-19
// For Verion E
    // For Verion E
    //ch 1
    {0x001007CC, 0x50090040},
    {0x001007D0, 0x057213a2},
    {0x001007D4, 0x50090044},
    {0x001007D8, 0x10000000},
    //ch 2
    {0x001007FC, 0x50090040},
    {0x00100800, 0x10000000},
    {0x00100804, 0x50090044},
    {0x00100808, 0x10000000},
    //ch 3
    {0x0010082C, 0x50090040},
    {0x00100830, 0x10000000},
    {0x00100834, 0x50090044},
    {0x00100838, 0x10000000},
    //ch 4
    {0x0010085C, 0x50090040},
    {0x00100860, 0x10000000},
    {0x00100864, 0x50090044},
    {0x00100868, 0x10000000},
    //ch 5
    {0x0010088C, 0x50090040},
    {0x00100890, 0x056794b4},
    {0x00100894, 0x50090044},
    {0x00100898, 0x10000000},
    //ch 6
    {0x001008BC, 0x50090040},
    {0x001008C0, 0x057c71de},
    {0x001008C4, 0x50090044},
    {0x001008C8, 0x10000000},
    //ch 7
    {0x001008EC, 0x50090040},
    {0x001008F0, 0x057e7140},
    {0x001008F4, 0x50090044},
    {0x001008F8, 0x10000000},
    //ch 8
    {0x0010091C, 0x50090040},
    {0x00100920, 0x057c7e22},
    {0x00100924, 0x50090044},
    {0x00100928, 0x10000000},
    //ch 9
    {0x0010094C, 0x50090040},
    {0x00100950, 0x10000000},
    {0x00100954, 0x50090044},
    {0x00100958, 0x10000000},
    //ch 10
    {0x0010097C, 0x50090040},
    {0x00100980, 0x10000000},
    {0x00100984, 0x50090044},
    {0x00100988, 0x10000000},
    //ch 11
    {0x001009AC, 0x50090040},
    {0x001009B0, 0x10000000},
    {0x001009B4, 0x50090044},
    {0x001009B8, 0x10000000},
    //ch 12
    {0x001009DC, 0x50090040},
    {0x001009E0, 0x05764310},
    {0x001009E4, 0x50090044},
    {0x001009E8, 0x10000000},
    //ch 13
    {0x00100A0C, 0x50090040},
    {0x00100A10, 0x056794b4},
    {0x00100A14, 0x50090044},
    {0x00100A18, 0x10000000},
    //ch 14
    {0x00100A3C, 0x50090040},
    {0x00100A40, 0x0579c279},
    {0x00100A44, 0x50090044},
    {0x00100A4c, 0x0579cd87},



    //{ 0x3001003c, 0x2e00a100 } ,
    // { 0x00106b6c, 0x00000002} , //patch for chanel 13

};


u32 wifi_notch_data_E[][2] =
{
    // For Verion E
    //ch 1
    {0x001007CC, 0x50090040},
    {0x001007D0, 0x057213a2},
    {0x001007D4, 0x50090044},
    {0x001007D8, 0x10000000},
    //ch 2
    {0x001007FC, 0x50090040},
    {0x00100800, 0x10000000},
    {0x00100804, 0x50090044},
    {0x00100808, 0x10000000},
    //ch 3
    {0x0010082C, 0x50090040},
    {0x00100830, 0x10000000},
    {0x00100834, 0x50090044},
    {0x00100838, 0x10000000},
    //ch 4
    {0x0010085C, 0x50090040},
    {0x00100860, 0x10000000},
    {0x00100864, 0x50090044},
    {0x00100868, 0x10000000},
    //ch 5
    {0x0010088C, 0x50090040},
    {0x00100890, 0x056794b4},
    {0x00100894, 0x50090044},
    {0x00100898, 0x10000000},
    //ch 6
    {0x001008BC, 0x50090040},
    {0x001008C0, 0x057c71de},
    {0x001008C4, 0x50090044},
    {0x001008C8, 0x10000000},
    //ch 7
    {0x001008EC, 0x50090040},
    {0x001008F0, 0x057e7140},
    {0x001008F4, 0x50090044},
    {0x001008F8, 0x10000000},
    //ch 8
    {0x0010091C, 0x50090040},
    {0x00100920, 0x057c7e22},
    {0x00100924, 0x50090044},
    {0x00100928, 0x10000000},
    //ch 9
    {0x0010094C, 0x50090040},
    {0x00100950, 0x10000000},
    {0x00100954, 0x50090044},
    {0x00100958, 0x10000000},
    //ch 10
    {0x0010097C, 0x50090040},
    {0x00100980, 0x10000000},
    {0x00100984, 0x50090044},
    {0x00100988, 0x10000000},
    //ch 11
    {0x001009AC, 0x50090040},
    {0x001009B0, 0x10000000},
    {0x001009B4, 0x50090044},
    {0x001009B8, 0x10000000},
    //ch 12
    {0x001009DC, 0x50090040},
    {0x001009E0, 0x05764310},
    {0x001009E4, 0x50090044},
    {0x001009E8, 0x10000000},
    //ch 13
    {0x00100A0C, 0x50090040},
    {0x00100A10, 0x056794b4},
    {0x00100A14, 0x50090044},
    {0x00100A18, 0x10000000},
    //ch 14
    {0x00100A3C, 0x50090040},
    {0x00100A40, 0x0579c279},
    {0x00100A44, 0x50090044},
    {0x00100A4c, 0x0579cd87},
};


#endif

