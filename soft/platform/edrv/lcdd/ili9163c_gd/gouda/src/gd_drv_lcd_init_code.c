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





#include "cs_types.h"

#include "lcdd_config.h"
#include "lcdd_private.h"
#include "hal_timers.h"
#include "hal_gouda.h"
#include "lcddp_debug.h"

#include "pmd_m.h"
#include "sxr_tls.h"
// To get config
#include "lcdd_tgt_params_gallite.h"
#include "lcdd.h"

#include "gd_drv_lcd_init_code.h"
// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

#define LCM_WR_DATA_8(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD_8(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}


//low 8bit
#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO);}
#define LCM_WR_DATA(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}


#define main_Write_COM  LCM_WR_CMD_8
#define main_Write_DATA LCM_WR_DATA_8

#define LCDD_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define LCDD_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define LCDD_BUILD_CMD_WR_REG(c,i,r,d) do{LCDD_BUILD_CMD_WR_CMD(c,i,r); LCDD_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)
// Number of actual pixels in the display width
#define LCDD_DISP_X     128

// Number of pixels in the display height
#define LCDD_DISP_Y     160

PRIVATE LCDD_ERR_T lcddp_FillRect16_init(UINT16 bgColor)
{
    UINT16 i,j;
    LCM_WR_CMD(0x2a);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(LCDD_DISP_X+2);

    LCM_WR_CMD(0x2b);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(LCDD_DISP_Y+3);

    LCM_WR_CMD(0x2c);       // vertical RAM address position
    for(i=0; i<LCDD_DISP_X+2; i++)
    {
        for(j=0; j<LCDD_DISP_Y+3; j++)
        {
            LCM_WR_DATA((bgColor>>8)&0xff);
            LCM_WR_DATA(bgColor&0xff);
        }
    }
}

//c121 赛特 亚星 GD_ALL_WRITE
#ifdef GD_TFT_9163C_SAITE_YAXIN
VOID lcddp_Init_9163C_SAITE_YAXIN(VOID)
{
    hal_HstSendEvent(0xFFFFFF);
    hal_HstSendEvent(0xFFFF);
    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x07);
    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x10);
    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x02);
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);
    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x4C);
    LCM_WR_DATA(0x5e);
    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0x3a); //Set Color Format
    LCM_WR_DATA(0x55);
    LCM_WR_CMD(0x2a); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7f);
    LCM_WR_CMD(0x2b); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9f);
    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xc8);
    LCM_WR_CMD(0xB7); //Set Source Output Direction
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0xEC); //Set pumping clock frequence
    LCM_WR_DATA(0x0C);
    LCM_WR_CMD(0xF2); //E0h & E1h Enable/Disable
    LCM_WR_DATA(0x01);
    LCM_WR_CMD(0xe0);
    LCM_WR_DATA(0x3f);
    LCM_WR_DATA(0x22);
    LCM_WR_DATA(0x20);
    LCM_WR_DATA(0x24);
    LCM_WR_DATA(0x20);
    LCM_WR_DATA(0x0c);
    LCM_WR_DATA(0x4e);
    LCM_WR_DATA(0xb7);
    LCM_WR_DATA(0x3c);
    LCM_WR_DATA(0x19);
    LCM_WR_DATA(0x22);
    LCM_WR_DATA(0x1e);
    LCM_WR_DATA(0x02);
    LCM_WR_DATA(0x01);
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xe1);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x1b);
    LCM_WR_DATA(0x1f);
    LCM_WR_DATA(0x0b);
    LCM_WR_DATA(0x0f);
    LCM_WR_DATA(0x13);
    LCM_WR_DATA(0x31);
    LCM_WR_DATA(0x84);
    LCM_WR_DATA(0x43);
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x1d);
    LCM_WR_DATA(0x21);
    LCM_WR_DATA(0x3d);
    LCM_WR_DATA(0x3e);
    LCM_WR_DATA(0x3f);
    LCM_WR_CMD(0x29); // Display On
    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
}
#endif
//德世普 解决背光闪动问题
#ifdef GD_TFT_9163C_DESHIPU
VOID lcddp_Init_9163C_DESHIPU(VOID)
{
    LCM_WR_CMD(0xEC);
    LCM_WR_DATA(0x0C);

    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x10);//10

    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x07);


    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);

    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x3A);
    LCM_WR_DATA(0x40);//40

    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xCD);

    LCM_WR_CMD(0x3A); //Set Color Format
    LCM_WR_DATA(0x55);

    LCM_WR_CMD(0x2A); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7F);

    LCM_WR_CMD(0x2B); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9F);

    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xC8);

    LCM_WR_CMD(0xF2); //Enable Gamma bit
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0x3F);//p1
    LCM_WR_DATA(0x1D);//p2
    LCM_WR_DATA(0x1C);//p3
    LCM_WR_DATA(0x26);//p4
    LCM_WR_DATA(0x22);//p5
    LCM_WR_DATA(0x0C);//p6
    LCM_WR_DATA(0x41);//p7
    LCM_WR_DATA(0xC7);//p8
    LCM_WR_DATA(0x39);//p9
    LCM_WR_DATA(0x17);//p10
    LCM_WR_DATA(0x1A);//p11
    LCM_WR_DATA(0x00);//p12
    LCM_WR_DATA(0x00);//p13
    LCM_WR_DATA(0x00);//p14
    LCM_WR_DATA(0x00);//p15

    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0x00);//p1
    LCM_WR_DATA(0x22);//p2
    LCM_WR_DATA(0x23);//p3
    LCM_WR_DATA(0x09);//p4
    LCM_WR_DATA(0x1D);//p5
    LCM_WR_DATA(0x13);//p6
    LCM_WR_DATA(0x2D);//p7
    LCM_WR_DATA(0x38);//p8
    LCM_WR_DATA(0x46);//p9
    LCM_WR_DATA(0x08);//p10
    LCM_WR_DATA(0x25);//p11
    LCM_WR_DATA(0x3F);//p12
    LCM_WR_DATA(0x3F);//p13
    LCM_WR_DATA(0x3F);//p14
    LCM_WR_DATA(0x3F);//p15

    LCM_WR_CMD(0x29); // Display On
    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
}
#endif

//c01 康维斯
#ifdef GD_TFT_C01_KANGWEISI
VOID lcddp_Init_C01_KANGWEISI(VOID)
{
    LCM_WR_CMD(0xEC);
    LCM_WR_DATA(0x0C);

    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x12);

    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x06);

    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);

    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x3C);
    LCM_WR_DATA(0x40);

    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xC3);

    LCM_WR_CMD(0x3A); //Set Color Format
    LCM_WR_DATA(0x55);

    LCM_WR_CMD(0x2A); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7F);

    LCM_WR_CMD(0x2B); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9F);

    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xB8);

    LCM_WR_CMD(0xF2); //Enable Gamma bit
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0x3F);//p1
    LCM_WR_DATA(0x18);//p2
    LCM_WR_DATA(0x18);//p3
    LCM_WR_DATA(0x26);//p4
    LCM_WR_DATA(0x20);//p5
    LCM_WR_DATA(0x0D);//p6
    LCM_WR_DATA(0x46);//p7
    LCM_WR_DATA(0xF3);//p8
    LCM_WR_DATA(0x32);//p9
    LCM_WR_DATA(0x09);//p10
    LCM_WR_DATA(0x02);//p11
    LCM_WR_DATA(0x02);//p12
    LCM_WR_DATA(0x00);//p13
    LCM_WR_DATA(0x00);//p14
    LCM_WR_DATA(0x00);//p15

    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0x00);//p1
    LCM_WR_DATA(0x27);//p2
    LCM_WR_DATA(0x27);//p3
    LCM_WR_DATA(0x09);//p4
    LCM_WR_DATA(0x0F);//p5
    LCM_WR_DATA(0x12);//p6
    LCM_WR_DATA(0x39);//p7
    LCM_WR_DATA(0xC0);//p8
    LCM_WR_DATA(0x4D);//p9
    LCM_WR_DATA(0x16);//p10
    LCM_WR_DATA(0x1D);//p11
    LCM_WR_DATA(0x2D);//p12
    LCM_WR_DATA(0x3F);//p13
    LCM_WR_DATA(0x3F);//p14
    LCM_WR_DATA(0x3F);//p15

    LCM_WR_CMD(0x29); // Display On
    LCM_WR_CMD_8(0x2c);   //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
    //初始化之后刷全黑
    lcddp_FillRect16_init(0x0000);

}
#endif
#ifdef GD_TFT_9163C_WEITUO
VOID lcddp_Init_9163C_WEITUO(VOID)
{

    LCM_WR_CMD(0x11);  //Exit Sleep
    sxr_Sleep(20 MS_WAITING);
    LCM_WR_CMD(0x26);  //Set Default Gamma
    LCM_WR_DATA(0x04);


    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x06);//06
    LCM_WR_DATA(0x10);


    LCM_WR_CMD(0xC0);  //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x0e);//0e
    LCM_WR_DATA(0x00);


    LCM_WR_CMD(0xC1);  //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x05);


    LCM_WR_CMD(0xC5);  //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x42);//40
    LCM_WR_DATA(0x3a);//40


    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xC2);

    LCM_WR_CMD(0x3A);
    LCM_WR_DATA(0x05);


    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x07);


    LCM_WR_CMD(0x2a);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7f);


    LCM_WR_CMD(0x2b);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9f);

    LCM_WR_CMD(0x36);//Set Scanning Direction
    LCM_WR_DATA(0xC8);   //Write_Data(0x08);88 ,C8,68



    LCM_WR_CMD(0xf2);//E0h & E1h Enable/Disable
    LCM_WR_DATA(0x01);//enable

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0X3F);
    LCM_WR_DATA(0X22);
    LCM_WR_DATA(0X20);
    LCM_WR_DATA(0X30);
    LCM_WR_DATA(0X29);
    LCM_WR_DATA(0X0c);
    LCM_WR_DATA(0X4e);
    LCM_WR_DATA(0XB7);
    LCM_WR_DATA(0X3c);
    LCM_WR_DATA(0X19);
    LCM_WR_DATA(0X22);
    LCM_WR_DATA(0X1E);
    LCM_WR_DATA(0X02);
    LCM_WR_DATA(0X01);
    LCM_WR_DATA(0X00);



    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0X00);
    LCM_WR_DATA(0X1b);
    LCM_WR_DATA(0X1f);
    LCM_WR_DATA(0X0F);
    LCM_WR_DATA(0X16);
    LCM_WR_DATA(0X13);
    LCM_WR_DATA(0X31);
    LCM_WR_DATA(0X84);
    LCM_WR_DATA(0X43);
    LCM_WR_DATA(0X06);
    LCM_WR_DATA(0X1D);
    LCM_WR_DATA(0X21);
    LCM_WR_DATA(0X3D);
    LCM_WR_DATA(0X3E);
    LCM_WR_DATA(0X3F);
    LCM_WR_CMD(0x29);// Display On
    LCM_WR_CMD(0x2c);
    //初始化之后刷全黑
    lcddp_FillRect16_init(0x0000);

}
#endif

//c01d摩西//中光电
#ifdef GD_TFT_9163C_MOXI_ZHONGGUANDIAN
VOID lcddp_Init_9163C_MOXI_ZHONGGUANDIAN(VOID)
{
    LCM_WR_CMD(0x11); //Exit Sleep
    sxr_Sleep(20 MS_WAITING);

    LCM_WR_CMD(0xEC);
    LCM_WR_DATA(0x0C);

    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x0A);
    LCM_WR_DATA(0x14);

    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);

    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x3C);
    LCM_WR_DATA(0x40);

    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xC3);

    LCM_WR_CMD(0x3A); //Set Color Format
    LCM_WR_DATA(0x55);

    LCM_WR_CMD(0x2A); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7F);

    LCM_WR_CMD(0x2B); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9F);

    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xC8);

    LCM_WR_CMD(0xF2); //Enable Gamma bit
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0x3F);//p1
    LCM_WR_DATA(0x18);//p2
    LCM_WR_DATA(0x18);//p3
    LCM_WR_DATA(0x26);//p4
    LCM_WR_DATA(0x20);//p5
    LCM_WR_DATA(0x0D);//p6
    LCM_WR_DATA(0x46);//p7
    LCM_WR_DATA(0xF3);//p8
    LCM_WR_DATA(0x32);//p9
    LCM_WR_DATA(0x09);//p10
    LCM_WR_DATA(0x02);//p11
    LCM_WR_DATA(0x02);//p12
    LCM_WR_DATA(0x00);//p13
    LCM_WR_DATA(0x00);//p14
    LCM_WR_DATA(0x00);//p15

    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0x00);//p1
    LCM_WR_DATA(0x27);//p2
    LCM_WR_DATA(0x27);//p3
    LCM_WR_DATA(0x09);//p4
    LCM_WR_DATA(0x0F);//p5
    LCM_WR_DATA(0x12);//p6
    LCM_WR_DATA(0x39);//p7
    LCM_WR_DATA(0xC0);//p8
    LCM_WR_DATA(0x4D);//p9
    LCM_WR_DATA(0x16);//p10
    LCM_WR_DATA(0x1D);//p11
    LCM_WR_DATA(0x2D);//p12
    LCM_WR_DATA(0x3F);//p13
    LCM_WR_DATA(0x3F);//p14
    LCM_WR_DATA(0x3F);//p15

    LCM_WR_CMD(0x29); // Display On
    //初始化之后刷全黑
    lcddp_FillRect16_init(0x0000);
}
#endif

//c01d 艾立丰
#ifdef GD_TFT_C01D_AILIFENG
VOID lcddp_Init_C01D_AILIFENG(VOID)
{

    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB1);  //Fram rate
    LCM_WR_DATA(0x10); //10
    LCM_WR_DATA(0x14); //14

    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x10); //10
    LCM_WR_DATA(0x00);//00

    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);//02

    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VCOMH & VCOML
    LCM_WR_DATA(0x4b);// 31
    LCM_WR_DATA(0x4F);// 4f

    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xB0);//Bb

    LCM_WR_CMD(0xEC);
    LCM_WR_DATA(0x0C);

    LCM_WR_CMD(0x3a); //Set Color Format
    LCM_WR_DATA(0x05);

    LCM_WR_CMD(0x2A); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7F);

    LCM_WR_CMD(0x2B); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9F);

    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xC0);

    LCM_WR_CMD(0xB7); //Set Source Output Direction
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xf2); //Enable Gamma bit
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0x3F);//p1
    LCM_WR_DATA(0x20);//p2
    LCM_WR_DATA(0x1d);//p3
    LCM_WR_DATA(0x24);//p4
    LCM_WR_DATA(0x1d);//p5
    LCM_WR_DATA(0x0d);//p6
    LCM_WR_DATA(0x45);//p7
    LCM_WR_DATA(0xBa);//p8
    LCM_WR_DATA(0x30);//p9
    LCM_WR_DATA(0x17);//p10
    LCM_WR_DATA(0x0f);//p11
    LCM_WR_DATA(0x08);//p12
    LCM_WR_DATA(0x04);//p13
    LCM_WR_DATA(0x02);//p14
    LCM_WR_DATA(0x00);//p15

    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0x00);//p1
    LCM_WR_DATA(0x1f);//p2
    LCM_WR_DATA(0x22);//p3
    LCM_WR_DATA(0x0c);//p4
    LCM_WR_DATA(0x12);//p5
    LCM_WR_DATA(0x12);//p6
    LCM_WR_DATA(0x3a);//p7
    LCM_WR_DATA(0x45);//p8
    LCM_WR_DATA(0x4E);//p9
    LCM_WR_DATA(0x08);//p10
    LCM_WR_DATA(0x20);//p11
    LCM_WR_DATA(0x27);//p12
    LCM_WR_DATA(0x3c);//p13
    LCM_WR_DATA(0x3d);//p14
    LCM_WR_DATA(0x3F);//p15

    LCM_WR_CMD(0x29); // Display On
    LCM_WR_CMD_8(0x2c);   //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
}
#endif
//c121希而可 f29
#ifdef GD_TFT_C121_XIERKE_F29
VOID lcddp_Init_C121_XIERKE_F29(VOID)
{
    LCM_WR_CMD(0x26); //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x07);
    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x10);
    LCM_WR_CMD(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x02);
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);
    LCM_WR_CMD(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x4C);
    LCM_WR_DATA(0x5e);
    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0x3a); //Set Color Format
    LCM_WR_DATA(0x55);
    LCM_WR_CMD(0x2a); //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7f);
    LCM_WR_CMD(0x2b); //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9f);
    LCM_WR_CMD(0x36); //Set Scanning Direction
    LCM_WR_DATA(0xc8);
    LCM_WR_CMD(0xB7); //Set Source Output Direction
    LCM_WR_DATA(0x00);
    LCM_WR_CMD(0xEC); //Set pumping clock frequence
    LCM_WR_DATA(0x0C);
    LCM_WR_CMD(0xF2); //E0h & E1h Enable/Disable
    LCM_WR_DATA(0x01);
    LCM_WR_CMD(0xe0);
    LCM_WR_DATA(0x3f);
    LCM_WR_DATA(0x22);
    LCM_WR_DATA(0x20);
    LCM_WR_DATA(0x24);
    LCM_WR_DATA(0x20);
    LCM_WR_DATA(0x0c);
    LCM_WR_DATA(0x4e);
    LCM_WR_DATA(0xb7);
    LCM_WR_DATA(0x3c);
    LCM_WR_DATA(0x19);
    LCM_WR_DATA(0x22);
    LCM_WR_DATA(0x1e);
    LCM_WR_DATA(0x02);
    LCM_WR_DATA(0x01);
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xe1);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x1b);
    LCM_WR_DATA(0x1f);
    LCM_WR_DATA(0x0b);
    LCM_WR_DATA(0x0f);
    LCM_WR_DATA(0x13);
    LCM_WR_DATA(0x31);
    LCM_WR_DATA(0x84);
    LCM_WR_DATA(0x43);
    LCM_WR_DATA(0x06);
    LCM_WR_DATA(0x1d);
    LCM_WR_DATA(0x21);
    LCM_WR_DATA(0x3d);
    LCM_WR_DATA(0x3e);
    LCM_WR_DATA(0x3f);
    LCM_WR_CMD(0x29); // Display On
    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
}
#endif

//c121天利屏// 威而肯
#ifdef GD_TFT_C121_TIANLI
VOID lcddp_Init_C121_TIANLI(VOID)
{
    LCM_WR_CMD(0x11);  //Exit Sleep
    sxr_Sleep(20 MS_WAITING);
    LCM_WR_CMD(0x26);  //Set Default Gamma
    LCM_WR_DATA(0x04);

    LCM_WR_CMD(0xB1);
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x12);

    LCM_WR_CMD(0xC0);  //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x05);


    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x06);

    LCM_WR_CMD(0xC1);  //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCM_WR_DATA(0x02);

    LCM_WR_CMD(0xC5);  //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCM_WR_DATA(0x30);
    LCM_WR_DATA(0x50);

    LCM_WR_CMD(0xC7);
    LCM_WR_DATA(0xBD);

    LCM_WR_CMD(0x3a);  //Set Color Format
    LCM_WR_DATA(0x05);

    LCM_WR_CMD(0x2A);  //Set Column Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x7F);

    LCM_WR_CMD(0x2B);  //Set Page Address
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x9F);

    LCM_WR_CMD(0x36);  //Set Scanning Direction
    LCM_WR_DATA(0xc0);//c8

    LCM_WR_CMD(0xB7);  //Set Source Output Direction
    LCM_WR_DATA(0x00);

    LCM_WR_CMD(0xf2);  //Enable Gamma bit
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);
    LCM_WR_DATA(0x18);//p1
    LCM_WR_DATA(0x05);//p2
    LCM_WR_DATA(0x12);//p3
    LCM_WR_DATA(0x1d);//p4
    LCM_WR_DATA(0x28);//p5
    LCM_WR_DATA(0x15);//p6
    LCM_WR_DATA(0x42);//p7
    LCM_WR_DATA(0xB2);//p8
    LCM_WR_DATA(0x2F);//p9
    LCM_WR_DATA(0x13);//p10
    LCM_WR_DATA(0x12);//p11
    LCM_WR_DATA(0x0A);//p12
    LCM_WR_DATA(0x11);//p13
    LCM_WR_DATA(0x0B);//p14
    LCM_WR_DATA(0x06);//p15

    LCM_WR_CMD(0xE1);
    LCM_WR_DATA(0x09);//p1
    LCM_WR_DATA(0x1c);//p2
    LCM_WR_DATA(0x2d);//p3
    LCM_WR_DATA(0x09);//p4
    LCM_WR_DATA(0x1c);//p5
    LCM_WR_DATA(0x1a);//p6
    LCM_WR_DATA(0x42);//p7
    LCM_WR_DATA(0x44);//p8
    LCM_WR_DATA(0x54);//p9
    LCM_WR_DATA(0x0d);//p10
    LCM_WR_DATA(0x3D);//p11
    LCM_WR_DATA(0x30);//p12
    LCM_WR_DATA(0x38);//p13
    LCM_WR_DATA(0x38);//p14
    LCM_WR_DATA(0x38);//p15

    LCM_WR_CMD(0x29); // Display On
    lcddp_FillRect16_init(0x0000);

}
#endif

//20120420
//c01 威而肯 天利
//奇美玻璃

#ifdef GD_TFT_9163C_TIANlLI
VOID lcddp_Init_9163C_TIANLI(VOID)
{
    main_Write_COM(0x26);  //Set Default Gamma
    main_Write_DATA(0x04);

    main_Write_COM(0xB1);
    main_Write_DATA(0x08);
    main_Write_DATA(0x10);

    main_Write_COM(0xC0);  //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    main_Write_DATA(0x08);
    main_Write_DATA(0x05);

    main_Write_COM(0xC1);  //Set BT[2:0] for AVDD & VCL & VGH & VGL
    main_Write_DATA(0x02);

    main_Write_COM(0xC5);  //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x23);//30
    main_Write_DATA(0x59);//50

    main_Write_COM(0xC7);
    main_Write_DATA(0xbf);


    main_Write_COM(0x3a);  //Set Color Format
    main_Write_DATA(0x05);

    main_Write_COM(0x2A);  //Set Column Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);

    main_Write_COM(0x2B);  //Set Page Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0x36);  //Set Scanning Direction
    main_Write_DATA(0xc0);

    main_Write_COM(0xB7);  //Set Source Output Direction
    main_Write_DATA(0x00);

    main_Write_COM(0xf2);  //Enable Gamma bit
    main_Write_DATA(0x01);

    main_Write_COM(0xE0);
    main_Write_DATA(0x18);//p1
    main_Write_DATA(0x05);//p2
    main_Write_DATA(0x12);//p3
    main_Write_DATA(0x1d);//p4
    main_Write_DATA(0x28);//p5
    main_Write_DATA(0x15);//p6
    main_Write_DATA(0x42);//p7
    main_Write_DATA(0xB2);//p8
    main_Write_DATA(0x2F);//p9
    main_Write_DATA(0x13);//p10
    main_Write_DATA(0x12);//p11
    main_Write_DATA(0x0A);//p12
    main_Write_DATA(0x11);//p13
    main_Write_DATA(0x0B);//p14
    main_Write_DATA(0x06);//p15

    main_Write_COM(0xE1);
    main_Write_DATA(0x09);//p1
    main_Write_DATA(0x1c);//p2
    main_Write_DATA(0x2d);//p3
    main_Write_DATA(0x09);//p4
    main_Write_DATA(0x1c);//p5
    main_Write_DATA(0x1a);//p6
    main_Write_DATA(0x42);//p7
    main_Write_DATA(0x44);//p8
    main_Write_DATA(0x54);//p9
    main_Write_DATA(0x0d);//p10
    main_Write_DATA(0x3D);//p11
    main_Write_DATA(0x30);//p12
    main_Write_DATA(0x38);//p13
    main_Write_DATA(0x38);//p14
    main_Write_DATA(0x38);//p15

    main_Write_COM(0x29); // Display On
    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    sxr_Sleep(100 MS_WAITING);
    //初始化之后刷全黑
    lcddp_FillRect16_init(0x0000);
}
#endif

//c01d摩西//亚新屏
#ifdef GD_TFT_9163C_ZHONGGUANGDIAN
VOID lcddp_Init_9163C_zhongguangdian(VOID)
{



    main_Write_COM(0x26); //Set Default Gamma
    main_Write_DATA(0x04);

    main_Write_COM(0xB1);
    main_Write_DATA(0x08);//0e
    main_Write_DATA(0x12);//14

    main_Write_COM(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    main_Write_DATA(0x08);
    main_Write_DATA(0x00);

    main_Write_COM(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    main_Write_DATA(0x05);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x46);//0x46
    main_Write_DATA(0x40);//0x40

    main_Write_COM(0xC7);
    main_Write_DATA(0xBD);  //0xC2




    main_Write_COM(0x3a); //Set Color Format
    main_Write_DATA(0x05);

    main_Write_COM(0x2A); //Set Column Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);
    main_Write_COM(0x2B); //Set Page Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0xB4); //Set Source Output Direction
    main_Write_DATA(0x06);//00

    main_Write_COM(0xf2); //Enable Gamma bit
    main_Write_DATA(0x01);


#ifdef LCDSIZE_160_128
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xB8);  //0xc0
#else
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xC8);  //0xc0
#endif

    main_Write_COM(0xE0);
    main_Write_DATA(0x3F);//p1
    main_Write_DATA(0x26);//p2
    main_Write_DATA(0x23);//p3
    main_Write_DATA(0x30);//p4
    main_Write_DATA(0x28);//p5
    main_Write_DATA(0x10);//p6
    main_Write_DATA(0x55);//p7
    main_Write_DATA(0xB7);//p8
    main_Write_DATA(0x40);//p9
    main_Write_DATA(0x19);//p10
    main_Write_DATA(0x10);//p11
    main_Write_DATA(0x1E);//p12
    main_Write_DATA(0x02);//p13
    main_Write_DATA(0x01);//p14
    main_Write_DATA(0x00);//p15

    main_Write_COM(0xE1);
    main_Write_DATA(0x00);//p1
    main_Write_DATA(0x19);//p2
    main_Write_DATA(0x1C);//p3
    main_Write_DATA(0x0F);//p4
    main_Write_DATA(0x14);//p5
    main_Write_DATA(0x0F);//p6
    main_Write_DATA(0x2A);//p7
    main_Write_DATA(0x48);//p8
    main_Write_DATA(0x3F);//p9
    main_Write_DATA(0x06);//p10
    main_Write_DATA(0x1D);//p11
    main_Write_DATA(0x21);//p12
    main_Write_DATA(0x3D);//p13
    main_Write_DATA(0x3E);//p14
    main_Write_DATA(0x3F);//p15

    main_Write_COM(0x29); // Display On

    main_Write_COM(0x2c);   //WRITE ram Data display


    sxr_Sleep(100 MS_WAITING);
    //初始化之后刷全黑
    //lcddp_FillRect16_init(0x0000);

}
#endif
//c10d中光电//智博
#ifdef GD_TFT_9163C_ZHONGGUANGDIAN_ZHIBO
VOID lcddp_Init_9163C_zhongguangdian_zhibo(VOID)
{

    main_Write_COM(0x26); //Set Default Gamma
    main_Write_DATA(0x04);

    main_Write_COM(0xB1);
    main_Write_DATA(0x08);//0e
    main_Write_DATA(0x12);//14

    main_Write_COM(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    main_Write_DATA(0x06);
    main_Write_DATA(0x00);

    main_Write_COM(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    main_Write_DATA(0x02);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x35);//0x46
    main_Write_DATA(0x35);//0x40

    main_Write_COM(0xC7);
    main_Write_DATA(0xC3);  //0xC2

    main_Write_COM(0x3a); //Set Color Format
    main_Write_DATA(0x05);

    main_Write_COM(0x2A); //Set Column Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);
    main_Write_COM(0x2B); //Set Page Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0xB4); //Set Source Output Direction
    main_Write_DATA(0x06);//00

    main_Write_COM(0xf2); //Enable Gamma bit
    main_Write_DATA(0x01);


#ifdef LCDSIZE_160_128
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xB8);  //0xc0
#else
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xC8);  //0xc0
#endif

    main_Write_COM(0xE0);
    main_Write_DATA(0x3F);//p1
    main_Write_DATA(0x26);//p2
    main_Write_DATA(0x23);//p3
    main_Write_DATA(0x30);//p4
    main_Write_DATA(0x28);//p5
    main_Write_DATA(0x10);//p6
    main_Write_DATA(0x55);//p7
    main_Write_DATA(0xB7);//p8
    main_Write_DATA(0x40);//p9
    main_Write_DATA(0x19);//p10
    main_Write_DATA(0x10);//p11
    main_Write_DATA(0x1E);//p12
    main_Write_DATA(0x02);//p13
    main_Write_DATA(0x01);//p14
    main_Write_DATA(0x00);//p15

    main_Write_COM(0xE1);
    main_Write_DATA(0x00);//p1
    main_Write_DATA(0x19);//p2
    main_Write_DATA(0x1C);//p3
    main_Write_DATA(0x0F);//p4
    main_Write_DATA(0x14);//p5
    main_Write_DATA(0x0F);//p6
    main_Write_DATA(0x2A);//p7
    main_Write_DATA(0x48);//p8
    main_Write_DATA(0x3F);//p9
    main_Write_DATA(0x06);//p10
    main_Write_DATA(0x1D);//p11
    main_Write_DATA(0x21);//p12
    main_Write_DATA(0x3D);//p13
    main_Write_DATA(0x3E);//p14
    main_Write_DATA(0x3F);//p15

    main_Write_COM(0x29); // Display On

    main_Write_COM(0x2c);   //WRITE ram Data display


    sxr_Sleep(100 MS_WAITING);

}
#endif
#ifdef GD_TFT_C01D_9163C_TIANWEI
VOID lcddp_Init_C01D_9163C_TIANWEI(VOID)
{
//--************ Start Initial Sequence **********--//
    main_Write_COM(0x11); //Exit Sleep
    sxr_Sleep(120 MS_WAITING);

    main_Write_COM(0x26); //Set Default Gamma
    main_Write_DATA(0x04);

    main_Write_COM(0xB1);
    main_Write_DATA(0x08);//0e
    main_Write_DATA(0x12);//14

    main_Write_COM(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    main_Write_DATA(0x08);
    main_Write_DATA(0x00);

    main_Write_COM(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    main_Write_DATA(0x05);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x46);//0x46
    main_Write_DATA(0x40);//0x40

    main_Write_COM(0xC7);
    main_Write_DATA(0xBD);  //0xC2




    main_Write_COM(0x3a); //Set Color Format
    main_Write_DATA(0x05);

    main_Write_COM(0x2A); //Set Column Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);
    main_Write_COM(0x2B); //Set Page Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0xB4); //Set Source Output Direction
    main_Write_DATA(0x06);//00

    main_Write_COM(0xf2); //Enable Gamma bit
    main_Write_DATA(0x01);


#ifdef LCDSIZE_160_128
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xB8);  //0xc0
#else
    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xC8);  //0xc0
#endif

    main_Write_COM(0xE0);
    main_Write_DATA(0x3F);//p1
    main_Write_DATA(0x26);//p2
    main_Write_DATA(0x23);//p3
    main_Write_DATA(0x30);//p4
    main_Write_DATA(0x28);//p5
    main_Write_DATA(0x10);//p6
    main_Write_DATA(0x55);//p7
    main_Write_DATA(0xB7);//p8
    main_Write_DATA(0x40);//p9
    main_Write_DATA(0x19);//p10
    main_Write_DATA(0x10);//p11
    main_Write_DATA(0x1E);//p12
    main_Write_DATA(0x02);//p13
    main_Write_DATA(0x01);//p14
    main_Write_DATA(0x00);//p15

    main_Write_COM(0xE1);
    main_Write_DATA(0x00);//p1
    main_Write_DATA(0x19);//p2
    main_Write_DATA(0x1C);//p3
    main_Write_DATA(0x0F);//p4
    main_Write_DATA(0x14);//p5
    main_Write_DATA(0x0F);//p6
    main_Write_DATA(0x2A);//p7
    main_Write_DATA(0x48);//p8
    main_Write_DATA(0x3F);//p9
    main_Write_DATA(0x06);//p10
    main_Write_DATA(0x1D);//p11
    main_Write_DATA(0x21);//p12
    main_Write_DATA(0x3D);//p13
    main_Write_DATA(0x3E);//p14
    main_Write_DATA(0x3F);//p15

    main_Write_COM(0x29); // Display On

    main_Write_COM(0x2c);   //WRITE ram Data display


    sxr_Sleep(100 MS_WAITING);
    lcddp_FillRect16_init(0x0000);
}
#endif


//天威c01d---中光电
#ifdef GD_TFT_C01D_9163C_TIANWEI_ZGD
VOID lcddp_Init_C01D_9163C_TIANWEI_ZGD(VOID)
{
    LCM_WR_CMD(0xEC);
    LCM_WR_DATA(0x0C);

    LCM_WR_CMD(0xc0);
    LCM_WR_DATA(0x16);
    LCM_WR_DATA(0x05);

    LCM_WR_CMD(0xc1);
    LCM_WR_DATA(0x02);

    LCM_WR_CMD(0xc5);
    LCM_WR_DATA(0x46);
    LCM_WR_DATA(0x40);

    LCM_WR_CMD(0xc7);
    LCM_WR_DATA(0xAB);//0xb7


    LCM_WR_CMD(0x36);
    LCM_WR_DATA(0xc0);

    LCM_WR_CMD(0x3A);   //Set Pixel Format
    LCM_WR_DATA(0x55);

    LCM_WR_CMD(0xF2);
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE0);    //Set Gamma
    LCM_WR_DATA(0x3f);
    LCM_WR_DATA(0x28);
    LCM_WR_DATA(0x24);
    LCM_WR_DATA(0x2d);
    LCM_WR_DATA(0x26);
    LCM_WR_DATA(0x0b);
    LCM_WR_DATA(0x4f);
    LCM_WR_DATA(0xc4);
    LCM_WR_DATA(0x3a);
    LCM_WR_DATA(0x18);
    LCM_WR_DATA(0x19);
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x05);
    LCM_WR_DATA(0x04);
    LCM_WR_DATA(0x01);

    LCM_WR_CMD(0xE1);    //Set Gamma
    LCM_WR_DATA(0x00);
    LCM_WR_DATA(0x17);
    LCM_WR_DATA(0x1b);
    LCM_WR_DATA(0x12);
    LCM_WR_DATA(0x19);
    LCM_WR_DATA(0x14);
    LCM_WR_DATA(0x30);
    LCM_WR_DATA(0x3b);
    LCM_WR_DATA(0x45);
    LCM_WR_DATA(0x07);
    LCM_WR_DATA(0x26);
    LCM_WR_DATA(0x2e);
    LCM_WR_DATA(0x3a);
    LCM_WR_DATA(0x3b);
    LCM_WR_DATA(0x3e);



    LCM_WR_CMD(0xB4);
    LCM_WR_DATA(0x07); //0x00

    LCM_WR_CMD(0xB1);    //Set Frame Rate
    LCM_WR_DATA(0x08);
    LCM_WR_DATA(0x12);

    LCM_WR_CMD(0x11);    //Exit Sleep
    sxr_Sleep(120);
    LCM_WR_CMD(0x29);    //Display on
    lcddp_FillRect16_init(0x0000);

    return;

}

#endif

