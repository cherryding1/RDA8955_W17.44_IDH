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



#ifdef WIFI_SUPPORT
#include "rdabt_drv.h"
#include "edrvp_debug.h"

//sync btdV3.45
#define RDA5990_USE_DCDC_MODE
#define __RDA_CHIP_R12_5990__

#if defined(__RDA_CHIP_R12_5990__)

const uint16 rdabt_phone_init_12[][2] =
{
    {0x3f,0x0001},//;page 1
#if defined(GALLITE_IS_8808)
    {0x29,0x0114},// Init UART IO status
#else
    {0x29,0x0114},//;
#endif
    {0x3f,0x0000},//;page 0
};


const uint16 rda_5990_bt_off_data[][2] =
{
    {0x3f, 0x0001 }, //pageup
    {0x28, 0x00A1 }, //power off bt
    {0x3f, 0x0000 }, //pagedown
};


const uint16 rda_5990_bt_power_up_data[][2] =
{
    {0x3f, 0x0001 }, //pageup
    {0x23, 0x4EE1 }, //power off bt
    {0x24, 0x80c8 }, //pagedown
    {0x27, 0x4925 },
    {0x29, 0x0114 },
    {0x32, 0x0111 },
    {0x2c, 0x0106 },
    {0x35, 0x0106 },
    {0x3f, 0x0000 },
};



#if 0
uint16 rda_5990_bt_en_data[][2] =
{
    {0x3f, 0x0001 },      //pageup
    {0x22, 0xD3C7},       // //for ver.c 20111109, txswitch
    {0x23, 0x8fA1},       // //20111001 higher AVDD voltage to improve EVM
    {0x24, 0x80C8},       // ;//freq_osc_in[1:0]00
    {0x26, 0x47A5},       //  reg_vbit_normal_bt[2:0] =111
    {0x27, 0x4925},       // //for ver.c20111109, txswitch
    {0x29, 0x1114},       // // rden4in_ldoon_bt=1
    {0x32, 0x0114},       // set_ rdenout_ldooff_wf=0;
    {0x39, 0x0000},       //      //uart switch to bt
    {0x28, 0x80A1},      // bt en
    {0x3f, 0x0000},      //pagedown
};

#else
uint16 rda_5990_bt_en_data[][2] =
{
    {0x3f, 0x0001},       //pageup
//{0x22, 0xc7A5},     //use setup_A2 function to setup A2
    /*
        to void DCDC interference fm,  DCDC setting is configed in
            RDA5990bt_wf_dcdc_ldo_trans function
    */
//{0x23, 0xCEE1},         //   for debug

    {0x28, 0x80A1},       // //20111001 higher AVDD voltage to improve EVM
    {0x3f, 0x0000},       // ;//freq_osc_in[1:0]00
};


#endif

const uint16 rda_5990_bt_en_clk_data[][2] =
{
    {0x3f, 0x0000 }, //pageup
    {0x30, 0x0040 },// enable chip sys clk
    {0x0f, 0x2223 },//set rf switch
    {0x3f, 0x0000 }, //pagedown
};



const uint16 rda_5990_bt_dig_reset_data[][2] =
{
    {0x3f, 0x0001 }, //pageup
    {0x28, 0x86A1 },
    {0x28, 0x87A1 },
    {0x28, 0x85A1 },
    {0x3f, 0x0000 }, //pagedown
};





uint16 rdabt_rf_init_12[][2] =
{
    {0x3f,0x0000}, //page 0
    {0x01, 0x1FFF},
    {0x06, 0x07F7},
    {0x08, 0x29E7},
    {0x09, 0x0520},
    {0x0B, 0x03DF},
    {0x0C, 0x85E8},
    {0x0F, 0x0DBC},
    {0x12, 0x07F7},
    {0x13, 0x0327},
    {0x14, 0x0CCC},
    {0x15, 0x0526},
    {0x16, 0x8918},
    {0x18, 0x8800},
    {0x19, 0x10C8},
    {0x1A, 0x9078},
    {0x1B, 0x80E2},
    {0x1C, 0x361F},
    {0x1D, 0x4363},
    {0x1E, 0x303F},
    {0x23, 0x2222},
    {0x24, 0x359D},
    {0x27, 0x0011},
    {0x28, 0x124F},
    {0x39, 0xA5FC},
    {0x3f, 0x0001}, //page 1
    {0x00, 0x043F},
    {0x01, 0x467F},
    {0x02, 0x28FF},
    {0x03, 0x67FF},
    {0x04, 0x57FF},
    {0x05, 0x7BFF},
    {0x06, 0x3FFF},
    {0x07, 0x7FFF},
    {0x18, 0xF3F5},
    {0x19, 0xF3F5},
    {0x1A, 0xE7F3},
    {0x1B, 0xF1FF},
    {0x1C, 0xFFFF},
    {0x1D, 0xFFFF},
    {0x1E, 0xFFFF},
    {0x1F, 0xFFFF},
//  {0x22, 0xD3C7},
//  {0x23, 0x8fa1},
//  {0x24, 0x80c8},
//  {0x26, 0x47A5},
//  {0x27, 0x4925},
//  {0x28, 0x85a1},
//  {0x29, 0x111f},
//  {0x32, 0x0111},
//  {0x39, 0x0000},
    {0x3f, 0x0000}, //page 0
};


//sync btdV3.45
const uint32 rdabt_pskey_rf_12[][2] =
{


//x40240000,0x0004f39c}, //; SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0,0x00000021}, //; CHIP_PS PSKEY: Total number -----------------
    {0x800000C4,0x003F0000}, //;         PSKEY: Page 0
    {0x800000C8,0x00414003}, //;         PSKEY: Swch_clk_ADC
    {0x800000CC,0x004225BD}, //;         PSKEY: dig IF 625K IF  by lihua20101231
    {0x800000D0,0x004908E4}, //;         PSKEY: freq_offset_for rateconv  by lihua20101231(replace 47H)
    {0x800000D4,0x0043B074}, //;         PSKEY: AM dac gain, 20100605
    {0x800000D8,0x0044D01A}, //;         PSKEY: gfsk dac gain, 20100605//22
    {0x800000DC,0x004A0800}, //;         PSKEY: 4AH=0800
    {0x800000E0,0x0054A020}, //;         PSKEY: 54H=A020;agc_th_max=A0;agc_th_min=20
    {0x800000E4,0x0055A020}, //;         PSKEY: 55H=A020;agc_th_max_lg=A0;agc_th_min_lg=20
    {0x800000E8,0x0056A542}, //;         PSKEY: 56H=A542
    {0x800000EC,0x00574C18}, //;         PSKEY: 57H=4C18
    {0x800000F0,0x003F0001}, //;         PSKEY: Page 1
    {0x800000F4,0x00410900}, //;         PSKEY: C1=0900;Phase Delay, 20101029
    {0x800000F8,0x0046033F}, //;         PSKEY: C6=033F;modulation Index;delta f2=160KHZ,delta f1=164khz
    {0x800000FC,0x004C0000}, //;         PSKEY: CC=0000;20101108
    {0x80000100,0x004D0015}, //;         PSKEY: CD=0015;
    {0x80000104,0x004E002B}, //;         PSKEY: CE=002B;
    {0x80000108,0x004F0042}, //;         PSKEY: CF=0042
    {0x8000010C,0x0050005A}, //;         PSKEY: D0=005A
    {0x80000110,0x00510073}, //;         PSKEY: D1=0073
    {0x80000114,0x0052008D}, //;         PSKEY: D2=008D
    {0x80000118,0x005300A7}, //;         PSKEY: D3=00A7
    {0x8000011C,0x005400C4}, //;         PSKEY: D4=00C4
    {0x80000120,0x005500E3}, //;         PSKEY: D5=00E3
    {0x80000124,0x00560103}, //;         PSKEY: D6=0103
    {0x80000128,0x00570127}, //;         PSKEY: D7=0127
    {0x8000012C,0x0058014E}, //;         PSKEY: D8=014E
    {0x80000130,0x00590178}, //;         PSKEY: D9=0178
    {0x80000134,0x005A01A1}, //;         PSKEY: DA=01A1
    {0x80000138,0x005B01CE}, //;         PSKEY: DB=01CE
    {0x8000013C,0x005C01FF}, //;         PSKEY: DC=01FF
    {0x80000140,0x003F0000}, //;         PSKEY: Page 0
    {0x80000144,0x00000000}, //;         PSKEY: Page 0
    {0x80000040,0x10000000}, //;         PSKEY: Flage
//x40240000,0x0000f29c}, //; SPI2_CLK_EN PCLK_SPI2_EN
};


const uint16 rdabt_dccal_12[][2]=
{
    {0x3f, 0x0000 },
    {0x30, 0x0129 },
    {0x30, 0x012b },
    {0x3f, 0x0000 }
};

const uint16 rdabt_5876_fm_init[][2] =
{
    {0x3f,0x0001},//page 1
    {0x2D,0x002A},//page 1
    {0x3f,0x0000},//page 1
};

const uint16 rdabt_5876_fm_deinit[][2] =
{
    {0x3f,0x0001},//page 1
    {0x2D,0x00AA},//page 1
    {0x3f,0x0000},//page 1
};

void RDABT_phone_Intialization_r12(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_phone_init_12)/sizeof(rdabt_phone_init_12[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_phone_init_12[i][0],&rdabt_phone_init_12[i][1],1);
    }
}
BOOL rda_on_off = FALSE;
#ifdef RDA5990_USE_DCDC_MODE

/*add to ensure RDA5990 is using ldo mode when FM is on
*/
void RDA5990bt_wf_dcdc_ldo_trans(void)
{
    uint8 pageaddr = 0x3F;
    uint8 dcdc_ctrl_reg = 0x23;
    uint16 temp_data;
    uint16 pageupvalue = 0x0001;
    uint16 pagedownvalue = 0x0000;

    rdabt_iic_rf_write_data(pageaddr,&pageupvalue,1);

    rdabt_iic_rf_read_data(0x28,&temp_data,1);
    EDRV_TRACE(EDRV_BTD_TRC, 0,"bt power on  A8 read back = %x",temp_data);

    rdabt_iic_rf_read_data(dcdc_ctrl_reg,&temp_data,1);
    EDRV_TRACE(EDRV_BTD_TRC, 0,"bt power on A3 read back = %x",temp_data);

    rdabt_iic_rf_read_data(0x22,&temp_data,1);
    EDRV_TRACE(EDRV_BTD_TRC, 0,"bt power on  A2 read back = %x",temp_data);




    if(!(temp_data&0x8000))   // fm is on  dcdc mode should not be used
    {

        rdabt_iic_rf_write_data(pageaddr,&pagedownvalue,1);
        return;
    }

    else                     // fm is off dcdc mode can be used

    {
        EDRV_TRACE(EDRV_BTD_TRC, 0,"A2 read back = %x",temp_data);

        rdabt_iic_rf_read_data(dcdc_ctrl_reg,&temp_data,1);
        EDRV_TRACE(EDRV_BTD_TRC, 0,"fm off A3 read back = %x",temp_data);
        temp_data|=0x8000;

        rdabt_iic_rf_write_data(dcdc_ctrl_reg,&temp_data,1);
        rdabt_iic_rf_read_data(dcdc_ctrl_reg,&temp_data,1);
        EDRV_TRACE(EDRV_BTD_TRC, 0,"fm off A3 write and read back = %x",temp_data);

        rdabt_iic_rf_write_data(pageaddr,&pagedownvalue,1);
    }


}


#endif







void Rda_bt_setup_A2_reg(int enable)
{
    UINT16 temp_data =0x0000;
    UINT16 page1_val=0x0001;
    UINT16 page0_val=0x0000;

//  SXS_TRACE(TSTDOUT, "Rda_bt_setup_A2_reg start\n");
    EDRV_TRACE(EDRV_BTD_TRC, 0,"bt power on  A8 read back\n");

    rdabt_iic_rf_write_data(0x3f,&page1_val,1);

    if(enable)
    {
        rdabt_iic_rf_read_data(0x22,&temp_data,1);
        //SXS_TRACE(TSTDOUT, "***0xA2 readback value:0x%x \n", temp_data);
        EDRV_TRACE(EDRV_BTD_TRC, 0,"0xA2 readback value:0x%x \n",temp_data);

        temp_data |=0x0200;   /*en reg4_pa bit*/
        rdabt_iic_rf_write_data(0x22,&temp_data,1);

    }
    else
    {
        rdabt_iic_rf_read_data(0x31,&temp_data,1);

        if(temp_data&0x8000)        // wf is on
        {
            goto out;
        }
        else
        {


            rdabt_iic_rf_read_data(0x22,&temp_data,1);
            temp_data&=0xfdff;

            rdabt_iic_rf_write_data(0x22,&temp_data,1);
        }


    }

out:
    rdabt_iic_rf_write_data(0x3f,&page0_val,1);

    //SXS_TRACE(TSTDOUT, "Rda_bt_setup_A2_reg finished\n");
    EDRV_TRACE(EDRV_BTD_TRC, 0,"Rda_bt_setup_A2_reg finished\n");


}










void RDA_5990_power_on(void)
{


    rda_on_off = TRUE;
    uint16 i=0;
    uint16 data;
    uint8 pageaddr = 0x3f;
    uint16 pagevalue = 0x0001;
    uint16 pagevalue1 = 0x0000;

    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDA_5990_power_on\n");
    hal_HstSendEvent(0xaa551234);

#if 0
    rdabt_iic_rf_write_data(pageaddr,&pagevalue,1);
    rdabt_iic_rf_read_data(0x22,&data,1);
    rdabt_iic_rf_write_data(pageaddr,&pagevalue1,1);
    ///if(!(data & 0x8000))
    //{
    //rda_5990_bt_en_data[1][2] = rda_5990_bt_en_data[1][2] & 0x7fff;
    //}
    rda_5990_bt_en_data[1][2] = data | 0x0200;

    //rdabt_iic_rf_write_data(pageaddr,&pagevalue1,1);


#endif


    for(i =0; i<sizeof(rda_5990_bt_en_data)/sizeof(rda_5990_bt_en_data[0]); i++)
    {
        rdabt_iic_rf_write_data(rda_5990_bt_en_data[i][0],(const uint16 *)(&rda_5990_bt_en_data[i][1]),1);
    }



}

void RDA_5990_bt_dig_reset(void)
{

    uint16 i=0;

    for(i =0; i<sizeof(rda_5990_bt_dig_reset_data)/sizeof(rda_5990_bt_dig_reset_data[0]); i++)
    {
        rdabt_iic_rf_write_data(rda_5990_bt_dig_reset_data[i][0],(const uint16 *)(&rda_5990_bt_dig_reset_data[i][1]),1);
    }

    //SXS_TRACE(TSTDOUT, "RDA_5990_bt_dig_reset finish\n");
}




void RDA_5990_power_off(void)
{
    uint16 i=0;
    uint16 data;
    uint8 pageaddr = 0x3f;
    uint16 pagevalue = 0x0001;
    uint16 pagevalue1 = 0x0000;
    uint16 poweroff = 0;
    hal_HstSendEvent(0xaa55face);

    rdabt_iic_rf_write_data(pageaddr,&pagevalue,1);
    rdabt_iic_rf_read_data(0x31,&data,1);
    if(data&0x8000)
    {
        hal_HstSendEvent(0xddaa2222);
    }
    else
    {
        rdabt_iic_rf_read_data(0x22,&data,1);
        poweroff = data & 0xfdff;
        rdabt_iic_rf_write_data(0x22,&poweroff,1);
        hal_HstSendEvent(0xddaa5555);
    }

    for(i =0; i<sizeof(rda_5990_bt_off_data)/sizeof(rda_5990_bt_off_data[0]); i++)
    {
        rdabt_iic_rf_write_data(rda_5990_bt_off_data[i][0],&rda_5990_bt_off_data[i][1],1);
    }
}

void RDA_5990_power_up_setting(void)
{
    uint16 i=0;
    for(i =0; i<sizeof(rda_5990_bt_power_up_data)/sizeof(rda_5990_bt_power_up_data[0]); i++)
    {
        rdabt_iic_rf_write_data(rda_5990_bt_power_up_data[i][0],&rda_5990_bt_power_up_data[i][1],1);
    }
}

void RDA_5990_clock_en(void)
{
    uint16 i=0;
    uint16 data;
    uint8 pageaddr = 0x3f;
    uint16 pagevalue = 0x0001;
    uint16 pagevalue1 = 0x0000;


    rdabt_iic_rf_write_data(pageaddr,&pagevalue,1);
    rdabt_iic_rf_read_data(0x31,&data,1);
    rdabt_iic_rf_write_data(pageaddr,&pagevalue1,1);

    if((data & 0x8000))
    {
        hal_HstSendEvent(0xdead1234);
        return;

    }

    for(i =0; i<sizeof(rda_5990_bt_en_clk_data)/sizeof(rda_5990_bt_en_clk_data[0]); i++)
    {
        rdawifi_iic_rf_write_data(rda_5990_bt_en_clk_data[i][0],&rda_5990_bt_en_clk_data[i][1],1);
    }
}


void RDABT_5870e_5876_fm_patch_on(void)
{
    uint16 i=0;
    I2C_Open();
    for(i =0; i<sizeof(rdabt_5876_fm_init)/sizeof(rdabt_5876_fm_init[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_5876_fm_init[i][0],&rdabt_5876_fm_init[i][1],1);
    }
    RDABT_DELAY(10);
    I2C_Close();
}

void RDABT_5870e_5876_fm_patch_off(void)
{
    uint16 i=0;
    I2C_Open();
    for(i =0; i<sizeof(rdabt_5876_fm_deinit)/sizeof(rdabt_5876_fm_deinit[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_5876_fm_deinit[i][0],&rdabt_5876_fm_deinit[i][1],1);
    }
    RDABT_DELAY(10);
    I2C_Close();
}

void  RDABT_rf_Intialization_r12(void)
{
    hal_HstSendEvent(0xface1234);
    uint16 i=0;
    for(i =0; i<sizeof(rdabt_rf_init_12)/sizeof(rdabt_rf_init_12[0]); i++)
    {
        rdabt_iic_rf_write_data(rdabt_rf_init_12[i][0],&rdabt_rf_init_12[i][1],1);
    }
    RDABT_DELAY(10);
}



void Rdabt_Pskey_Write_rf_r12(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_pskey_rf_12)/sizeof(rdabt_pskey_rf_12[0]); i++)
    {

        rdabt_wirte_memory(rdabt_pskey_rf_12[i][0],&rdabt_pskey_rf_12[i][1],1,0);
        RDABT_DELAY(2);
    }
}

void rdabt_DC_write_r12(void)
{
    uint16 i;
    for(i=0; i<sizeof(rdabt_dccal_12)/sizeof(rdabt_dccal_12[0]); i++)
    {

        rdabt_iic_rf_write_data(rdabt_dccal_12[i][0],&rdabt_dccal_12[i][1],1);
        RDABT_DELAY(10);

    }
}


void RDABT_core_Intialization_r12(void)
{
#ifdef RDA_BT_debug
    EDRV_TRACE(EDRV_BTD_TRC, 0,"RDABT_core_Intialization_r12");

#endif
    rda_bt_pin_to_low();
    rda_bt_ldoon_toggle_high();
    rda_bt_reset_to_high();
    RDABT_DELAY(10); //wait for digi running
    RDABT_rf_Intialization_r12();
    Rdabt_Pskey_Write_rf_r12();
    rdabt_DC_write_r12();
    hal_HstSendEvent(0xface1234);
    Rdabt_Pskey_Write_r12();
//    Rdabt_patch_write_r12();         //houzhen for 8852c platform rda5990 bt porting
    Rdabt_unsniff_prerxon_write_r12();
    Rdabt_setfilter_r12();
//    Rdabt_trap_write_r12();  //houzhen for 8852c platform rda5990 bt porting
}

#endif

#endif





