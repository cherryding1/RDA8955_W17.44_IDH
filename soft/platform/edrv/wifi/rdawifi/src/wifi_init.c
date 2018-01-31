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

#include "tgt_mcd_cfg.h"


#include <hal_debug.h>
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_sys.h"
#include "sxr_mem.h"
#include "sxr_sbx.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "sxr_tim.h"
#include <string.h>
#include <stdio.h>
#include "wifi_init_d.h"
#include "wifi_init_e.h"
//#include "fs.h"
//#include "cfw.h"

//#define RDA5990_WIFI_USE_DCDC_MODE
#define WIFI_VERSION_D  0x7

#define WIFI_VERSION_E  0x4
#define WIFI_VERSION_F  0x5
UINT8 wifi_version_flag = 0x7;






#define I2C_MASTER_ACK              (1<<0)
#define I2C_MASTER_RD               (1<<4)
#define I2C_MASTER_STO              (1<<8)
#define I2C_MASTER_WR               (1<<12)
#define I2C_MASTER_STA              (1<<16)
//PUBLIC BOOL hal_I2cIsBusy(HAL_I2C_BUS_ID_T id);
#define WIFI_RF_ADDR 0x14
#define WIFI_CORE_ADDR 0x13
HAL_I2C_BUS_ID_T g_wifiI2cBusId = HAL_I2C_BUS_ID_2;

#define WIFI_DELAY(DURATION) sxr_Sleep(DURATION  MS_WAITING)
UINT8  wifi_i2c_core_write_data(UINT32 Regiter, CONST UINT32 *pData, UINT8 datalen)
{
#ifdef I2C_BASED_ON_GPIO
    //  gpio_i2c_rdabt_core_write_data(WIFI_CORE_ADDR<<1,Regiter, pData,datalen);
    return TRUE;
#else
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
    UINT8 Data_half[4];
    UINT8 i,j;

    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_CORE_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>24,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>16,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>8,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_CORE_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);

    for(j=0; j<datalen-1; j++,pData++) //data
    {
        Data_half[0]=*pData>>24;
        Data_half[1]=*pData>>16;
        Data_half[2]=*pData>>8;
        Data_half[3]=*pData&0x00FF;
        for(i=0; i<4; i++)
        {
            halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[i],I2C_MASTER_WR);
        }
    }
    Data_half[0]=*pData>>24;
    Data_half[1]=*pData>>16;
    Data_half[2]=*pData>>8;
    Data_half[3]=*pData&0x00FF;
    for(i=0; i<3; i++)
    {
        halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[i],I2C_MASTER_WR);
    }
    halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[3],I2C_MASTER_WR|I2C_MASTER_STO);
    //halErr=hal_I2cSendData(RDABT_CORE_ADDR,Regiter,Data_half,4);
    SXS_TRACE(TSTDOUT, "write I2C: register: 0x%x, data: 0x%x, ret: %d.",Regiter,*pData,halErr);

    if(halErr==HAL_ERR_NO)
        return TRUE;
    else
        return FALSE;
#endif
}
UINT8 wifi_i2c_core_read_data(UINT32 Regiter, UINT32 *pData, UINT8 datalen)
{
//  HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
#ifdef I2C_BASED_ON_GPIO
    //  gpio_i2c_rdabt_core_read_data(RDABT_CORE_ADDR<<1,Regiter, pData,datalen);
#else
    UINT8 Data_half[4];
    UINT8 i,j;

    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_CORE_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>24,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>16,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter>>8,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,Regiter,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,((WIFI_CORE_ADDR<<1)|1),I2C_MASTER_WR | I2C_MASTER_STA);

    for(j=0; j<datalen-1; j++,pData++) //data
    {
        for(i=0; i<4; i++)
        {
            Data_half[i]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD);
        }
        (*pData)=Data_half[3];
        (*pData)|=Data_half[2]<<8;
        (*pData)|=Data_half[1]<<16;
        (*pData)|=Data_half[0]<<24;
        //   SXS_TRACE(TSTDOUT, "Read I2C: %d\n.",halErr);
    }
    for(i=0; i<3; i++)
    {
        Data_half[i]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD);
    }
    Data_half[3]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);
    (*pData)=Data_half[3];
    (*pData)|=Data_half[2]<<8;
    (*pData)|=Data_half[1]<<16;
    (*pData)|=Data_half[0]<<24;
#endif
    SXS_TRACE(TSTDOUT, "Read I2C: register: 0x%x; data:0x%x.",Regiter,*pData);

    return TRUE;
}
HAL_ERR_T wifi_i2c_rf_write_data(unsigned char regaddr, const unsigned short *data, unsigned char datalen)
{

#ifdef I2C_BASED_ON_GPIO
    //  gpio_i2c_rdabt_rf_write_data(WIFI_RF_ADDR<<1,regaddr, data,datalen);
#else

    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
    UINT8 Data_half[2];
    UINT8 i=0;

    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_RF_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);
    hal_I2cSendRawByte(g_wifiI2cBusId,regaddr,I2C_MASTER_WR);

    for(i=0; i<datalen-1; i++,data++) //data
    {
        Data_half[0]=*data>>8;
        Data_half[1]=*data&0x00FF;

        halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[0],I2C_MASTER_WR);
        halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[1],I2C_MASTER_WR);
    }
    Data_half[0]=*data>>8;
    Data_half[1]=*data&0x00FF;

    halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[0],I2C_MASTER_WR);
    halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[1],I2C_MASTER_WR|I2C_MASTER_STO);
#endif
    SXS_TRACE(TSTDOUT, "write I2C: register: 0x%x, data: 0x%x, ret: %d.",regaddr,*data,halErr);
    return halErr;
//  SXS_TRACE(TSTDOUT, "rdabt_iic_rf_write_data: %d\n.",halErr);
}
HAL_ERR_T wifi_i2c_write_U8U8(unsigned char regaddr, const unsigned char data, unsigned char datalen)
{

#ifdef I2C_BASED_ON_GPIO
    //  gpio_i2c_rdabt_rf_write_data(WIFI_RF_ADDR<<1,regaddr, data,datalen);
#else

    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;
    UINT8 Data_half;

    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_RF_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);
    hal_I2cSendRawByte(g_wifiI2cBusId,regaddr,I2C_MASTER_WR);


    Data_half=data;

    //halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half[0],I2C_MASTER_WR);
    halErr=hal_I2cSendRawByte(g_wifiI2cBusId,Data_half,I2C_MASTER_WR|I2C_MASTER_STO);
#endif
    SXS_TRACE(TSTDOUT, "write I2C: register: 0x%x, data: 0x%x, ret: %d.",regaddr,data,halErr);
    return halErr;
//  SXS_TRACE(TSTDOUT, "rdabt_iic_rf_write_data: %d\n.",halErr);
}
void wifi_i2c_rf_read_data(unsigned char regaddr, unsigned short *data, unsigned char datalen)
{
#ifdef I2C_BASED_ON_GPIO
    //  gpio_i2c_rdabt_rf_read_data((RDABT_RF_ADDR<<1),regaddr, data,datalen);
#else

    UINT8 Data_half[2];
    UINT8 i=0;

    hal_I2cSendRawByte(g_wifiI2cBusId,(WIFI_RF_ADDR<<1),I2C_MASTER_WR | I2C_MASTER_STA);
    hal_I2cSendRawByte(g_wifiI2cBusId,regaddr,I2C_MASTER_WR);
    hal_I2cSendRawByte(g_wifiI2cBusId,((WIFI_RF_ADDR<<1)|1),I2C_MASTER_WR | I2C_MASTER_STA);

    for(i=0; i<datalen-1; i++,data++) //data
    {
        Data_half[0]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD);
        Data_half[1]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD);

        (*data)=Data_half[0]<<8;
        (*data)|=Data_half[1];
    }

    Data_half[0]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD);
    Data_half[1]=hal_I2cReadRawByte(g_wifiI2cBusId,I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);

    (*data)=Data_half[0]<<8;
    (*data)|=Data_half[1];

#endif
    SXS_TRACE(TSTDOUT, "Read I2C: register: 0x%x; data:0x%x.",regaddr,*data);

//  SXS_TRACE(TSTDOUT, "rdabt_iic_rf_read_data: %d\n.");
}

void wifi_Write_core_data(void)
{
    SXS_TRACE(TSTDOUT, "wifi_Write_core_data.  wifi_version: 0x%x",wifi_version_flag);

    INT32 i;


#if 1
    UINT32* tmp_array = NULL;
    UINT32 array_len = 0;

    if(wifi_version_flag== WIFI_VERSION_D )
    {
        tmp_array = wifi_core_init_data;
        array_len = sizeof(wifi_core_init_data)/sizeof(wifi_core_init_data[0]);
    }
    else if((wifi_version_flag== WIFI_VERSION_E) ||(wifi_version_flag== WIFI_VERSION_F))
    {
        tmp_array = wifi_core_init_data_e;
        array_len = sizeof(wifi_core_init_data_e)/sizeof(wifi_core_init_data_e[0]);
    }


    for(i=0; i<array_len; i++)
    {
        //  SXS_TRACE(TSTDOUT, "wifi_Write_core_data.  array_len: 0x%x, i: %d",array_len,i);


        wifi_i2c_core_write_data(tmp_array[2*i],&tmp_array[2*i+1],1);
        WIFI_DELAY(5);
    }

#else
    for(i=t; i<sizeof(wifi_core_init_data)/sizeof(wifi_core_init_data[0]); i++)
    {
        //while(hal_I2cIsBusy(g_wifiI2cBusId)) ;

        wifi_i2c_core_write_data(wifi_core_init_data[i][0],&wifi_core_init_data[i][1],1);
        WIFI_DELAY(5);
    }
#endif
}
void wifi_Read_core_data(void)
{
    SXS_TRACE(TSTDOUT, "wifi_Read_core_data.");

    INT32 i, t;
    t = 0;
    UINT32 data_r =0x00;

    wifi_i2c_core_read_data(0x30010000,&data_r,1);
    t++;
    data_r=0x00;
    WIFI_DELAY(100);
//   return;


    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    data_r=0x00;
    WIFI_DELAY(100);

    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    WIFI_DELAY(500);
    data_r=0x00;

    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    WIFI_DELAY(500);
    data_r=0x00;

    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    WIFI_DELAY(500);
    data_r=0x00;

    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    WIFI_DELAY(500);
    data_r=0x00;

    wifi_i2c_core_read_data(wifi_core_init_data[t][0],&data_r,1);
    t++;
    WIFI_DELAY(500);
    data_r=0x00;

    for(i=t; i<sizeof(wifi_core_init_data)/sizeof(wifi_core_init_data[0]); i++)
    {

        wifi_i2c_core_read_data(wifi_core_init_data[i][0],&data_r,1);
        WIFI_DELAY(5);
        data_r=0x00;

    }
}


UINT8 reg3F7F_flag = 0x00;
void wifi_Write_rf_data(void)
{
    SXS_TRACE(TSTDOUT, "wifi_Write_rf_data.");

    INT32 i =0;
    UINT8 tmp_reg =0x00;

    HAL_ERR_T halErr;

    for(i=0; i<sizeof(wifi_rf_init_data)/sizeof(wifi_rf_init_data[0]); i++)
    {
        tmp_reg = wifi_rf_init_data[i][0];

        halErr= wifi_i2c_rf_write_data(tmp_reg, &wifi_rf_init_data[i][1],1);
        if(halErr!=0)
            return;
        WIFI_DELAY(5);


    }
}
void wifi_Read_Version(void)
{
    SXS_TRACE(TSTDOUT, "wifi_Read_Version.");

    INT32 i =0;
    UINT16 data_r =0x00;
    UINT8 tmp_reg =0x00;


    for(i=0; i<sizeof(wifi_version)/sizeof(wifi_version[0]); i++)
    {
        tmp_reg = wifi_version[i][0];
        if(0x3f==tmp_reg)
        {
            wifi_i2c_rf_write_data(tmp_reg,&wifi_version[i][1],1);
            WIFI_DELAY(10);
            //      data_r =0x00;
        }
        else
        {
            wifi_i2c_rf_read_data(tmp_reg,&data_r,1);
            SXS_TRACE(TSTDOUT," wifi version , 0x%x:  0x%x\n",tmp_reg+0x80,data_r);
            WIFI_DELAY(10);
            if(tmp_reg==0x21)
            {
                wifi_version_flag = 0x1f & data_r;
                SXS_TRACE(TSTDOUT," wifi version : 0x%x\n",wifi_version_flag);

            }
            data_r =0x00;

        }

    }
}


UINT32 wifi_Read_MAC_Reg_I2C(UINT32 reg)
{
    hal_I2cOpen(g_wifiI2cBusId);

    UINT32 data_r =0x00;
    wifi_i2c_core_read_data(reg,&data_r,1);
    //SXS_TRACE(TSTDOUT, "wifi_Read_Reg_I2C. register: 0x%x, val: 0x%x",reg,data_r);
    hal_I2cClose(g_wifiI2cBusId);
    return data_r;
}
UINT32 wifi_Write_MAC_Reg_I2C(UINT32 reg, UINT32 val)
{
    hal_I2cOpen(g_wifiI2cBusId);

    UINT32 data_r =val;
    wifi_i2c_core_write_data(reg,&data_r,1);
    SXS_TRACE(TSTDOUT, "wifi_Write_Reg_I2C. register: 0x%x, val: 0x%x",reg,data_r);
    hal_I2cClose(g_wifiI2cBusId);
    return data_r;
}


UINT16 wifi_Read_MAC_Reg_I2C_rf(UINT8 reg)
{
    hal_I2cOpen(g_wifiI2cBusId);

    UINT16 data_r =0x00;

    if(reg>0x80)
    {
        data_r =0x01;
        wifi_i2c_rf_write_data(0x3F, &data_r,1);
        data_r =0x00;
    }

    wifi_i2c_rf_read_data((reg-0x80),&data_r,1);
    //SXS_TRACE(TSTDOUT, "wifi_Read_Reg_I2C. register: 0x%x, val: 0x%x",reg,data_r);

    if(reg>0x80)
    {
        UINT16 data_tmp = 0x00;
        wifi_i2c_rf_write_data(0x3F,&data_tmp,1);

    }

    hal_I2cClose(g_wifiI2cBusId);
    return data_r;

}
void wifi_Write_MAC_Reg_I2C_rf(UINT8 reg, UINT16 val)
{
    hal_I2cOpen(g_wifiI2cBusId);

    UINT16 data_r =0x00;
    if(reg>0x80)
    {
        data_r =0x01;

        wifi_i2c_rf_write_data(0x3F, &data_r,1);

    }
    data_r =val;
    wifi_i2c_rf_write_data((reg-0x80),&data_r,1);

    /*
     data_r =0x14;
    wifi_i2c_rf_write_data((reg-0x80),&data_r,1);
    data_r =0x04;
        wifi_i2c_rf_write_data((reg-0x80),&data_r,1);
    */
    if(reg>0x80)
    {
        data_r =0x00;
        wifi_i2c_rf_write_data(0x3F, &data_r,1);

    }
    hal_I2cClose(g_wifiI2cBusId);

}

void wifi_Read_rf_data(void)
{
    SXS_TRACE(TSTDOUT, "wifi_Read_rf_data.");

    INT32 i =0;
    UINT16 data_r =0x00;
    UINT8 tmp_reg =0x00;

//   reg3F7F_flag = 0x01;

//wifi_i2c_rf_write_data(0x3f,&reg3F7F_flag,1);
//     WIFI_DELAY(50);

    for(i=0; i<sizeof(wifi_rf_init_data)/sizeof(wifi_rf_init_data[0]); i++)
    {
        tmp_reg = wifi_rf_init_data[i][0];
        if(0x3f==tmp_reg)
        {
            wifi_i2c_rf_write_data(tmp_reg,&wifi_rf_init_data[i][1],1);
            WIFI_DELAY(10);
            //      data_r =0x00;
        }
        else
        {
            wifi_i2c_rf_read_data(tmp_reg,&data_r,1);
            WIFI_DELAY(10);
            data_r =0x00;

        }



    }
}


VOID wifi_I2c_Write_U8(UINT8  (*u8_reg)[2], UINT8 num)
{
    SXS_TRACE(TSTDOUT, "write_rssi_switch_data_u8.");
    hal_I2cOpen(g_wifiI2cBusId);
    INT32 i =0;
    UINT8 tmp_reg =0x00;

    HAL_ERR_T halErr;

    for(i=0; i<num ; i++)
    {
        tmp_reg = u8_reg[i][0];
        halErr= wifi_i2c_write_U8U8(tmp_reg, u8_reg[i][1],1);
        if(halErr!=0)
            return;
        WIFI_DELAY(5);


    }
    hal_I2cClose(g_wifiI2cBusId);
}


VOID wifi_I2c_Write_U32(UINT32  (*u32_reg)[2], UINT8 num)
{
    SXS_TRACE(TSTDOUT, "write_rssi_switch_data_u32.");
    hal_I2cOpen(g_wifiI2cBusId);
    INT32 i =0;
    UINT8 tmp_reg =0x00;

    HAL_ERR_T halErr;

    for(i=0; i<num ; i++)
    {
        tmp_reg = u32_reg[i][0];

        halErr= wifi_i2c_core_read_data(tmp_reg, &u32_reg[i][1],1);
        if(halErr!=0)
            return;
        WIFI_DELAY(5);


    }
    hal_I2cClose(g_wifiI2cBusId);
}
/*
VOID write_rssi_switch_data_u32(VOID)
{
    SXS_TRACE(TSTDOUT, "write_rssi_switch_data_u32.");
  hal_I2cOpen(g_wifiI2cBusId);
    INT32 i =0;
   UINT8 tmp_reg =0x00;

HAL_ERR_T halErr;

   for(i=0;i<sizeof(rssi_switch_data_u32)/sizeof(rssi_switch_data_u32[0]);i++)
   {
    tmp_reg = rssi_switch_data_u32[i][0];

      halErr= wifi_i2c_core_read_data(tmp_reg, &rssi_switch_data_u32[i][1],1);
    if(halErr!=0)
        return;
     WIFI_DELAY(5);


   }
         hal_I2cClose(g_wifiI2cBusId);

}
VOID write_rssi_switch_data_u8(VOID)
{
SXS_TRACE(TSTDOUT, "write_rssi_switch_data_u8.");
  hal_I2cOpen(g_wifiI2cBusId);
    INT32 i =0;
   UINT8 tmp_reg =0x00;

HAL_ERR_T halErr;

   for(i=0;i<sizeof(rssi_switch_data_u8)/sizeof(rssi_switch_data_u8[0]);i++)
   {
    tmp_reg = rssi_switch_data_u8[i][0];

      halErr= wifi_i2c_rf_write_data(tmp_reg, &rssi_switch_data_u8[i][1],1);
    if(halErr!=0)
        return;
     WIFI_DELAY(5);


   }
         hal_I2cClose(g_wifiI2cBusId);

}



VOID wifi_sdio_set_notch_by_channel( UINT8 channel)
{
    int count = 0, index = 0;

    if(wifi_version_flag== WIFI_VERSION_D)
        count  = sizeof(wifi_notch_data)/(sizeof(wifi_notch_data[0]) * 4);
    else if(wifi_version_flag== WIFI_VERSION_E)
        count  = sizeof(wifi_notch_data_E)/(sizeof(wifi_notch_data_E[0]) * 4);
    channel = channel % count;


    if(channel > 1)
        channel -= 1;

   if(wifi_version_flag== WIFI_VERSION_D)
        {
            wifi_I2c_Write_U32(wifi_notch_data[4*channel],4);
        //    rda5890_set_core_init(priv, wifi_notch_data[4*channel], 4);
        wifi_I2c_Write_U8(rssi_switch_default_data_u8, 1);
           // rda5890_set_core_patch(priv, rssi_switch_default_data_u8, 1);
        }
     else if(wifi_version_flag== WIFI_VERSION_E)
        {
            wifi_I2c_Write_U32(wifi_notch_data_E[4*channel],4);
        //    rda5890_set_core_init(priv, wifi_notch_data[4*channel], 4);
        wifi_I2c_Write_U8(rssi_switch_default_data_u8, 1);
           // rda5890_set_core_patch(priv, rssi_switch_default_data_u8, 1);
        }
}
*/
void setclockmode(void);
HAL_SYS_CLOCK_OUT_ID_T g_WifiClockOutId = HAL_SYS_CLOCK_OUT_ID_QTY;
BOOL g_wifi32kClockRequested =FALSE;
BOOL g_wifi26mClockReq = FALSE;
/*
void wifi_init_ex(void)
{
    SXS_TRACE(TSTDOUT, "wifi_initEx.");
//setclockmode();
#ifdef USE_32K_CLOCK_PIN
    if (!g_wifi32kClockRequested)
    {
        hal_Sys32kClkOut(TRUE);
       g_wifi32kClockRequested = TRUE;
    }
#else
     g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
        if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == g_WifiClockOutId)
        {
         SXS_TRACE(TSTDOUT," wifi 32k clock error%d\n");
            //return FALSE;
        }

#endif

  //   g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);

  hal_I2cOpen(g_wifiI2cBusId);

    //  SXS_TRACE(TSTDOUT, "open 32k clock, ret:0x%x .",g_WifiClockOutId);

//    WIFI_DELAY(10);
  wifi_Read_rf_data();
    WIFI_DELAY(500);

  wifi_Write_rf_data();
      WIFI_DELAY(500);
  wifi_Read_rf_data();
        WIFI_DELAY(5000);

    wifi_Write_core_data();
    WIFI_DELAY(200);

 wifi_Read_core_data();
    WIFI_DELAY(200);


  WIFI_DELAY(200);
      hal_I2cClose(g_wifiI2cBusId);
      #ifndef USE_32K_CLOCK_PIN

        hal_SysClkOutClose(g_WifiClockOutId);
    #endif


}
*/
HAL_ERR_T Rda_wf_poweron(void)
{
    INT32 i =0;
    HAL_ERR_T halErr;
    SXS_TRACE(TSTDOUT, "Rda_wf_poweron start\n");
    for(i=0; i<sizeof(Rda_wf_poweron_data)/sizeof(Rda_wf_poweron_data[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(Rda_wf_poweron_data[i][0], &Rda_wf_poweron_data[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_poweron failed\n");
            return halErr;
        }
        WIFI_DELAY(10);
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_poweron finish\n");
    return 0;
}




HAL_ERR_T Rda_wf_setup_A2_reg(int enable)
{
    HAL_ERR_T ret = 0;
    UINT16 temp_data =0x0000;
    UINT16 page1_val=0x0001;
    UINT16 page0_val=0x0000;

    SXS_TRACE(TSTDOUT, "Rda_wf_setup_A2_reg start\n");

    ret=wifi_i2c_rf_write_data(0x3f,&page1_val,1);
    if(ret)
        goto err;

    if(enable)
    {
        wifi_i2c_rf_read_data(0x22,&temp_data,1);
        SXS_TRACE(TSTDOUT, "***0xA2 readback value:0x%x \n", temp_data);
        temp_data |=0x0200;   /*en reg4_pa bit*/
        ret=wifi_i2c_rf_write_data(0x22,&temp_data,1);
        if(ret)
            goto err;

    }
    else
    {
        wifi_i2c_rf_read_data(0x28,&temp_data,1);

        if(temp_data&0x8000)        // bt is on
        {
            goto out;
        }
        else
        {


            wifi_i2c_rf_read_data(0x22,&temp_data,1);
            temp_data&=0xfdff;

            ret=wifi_i2c_rf_write_data(0x22,&temp_data,1);
            if(ret)
                goto err;
        }


    }

out:
    ret=wifi_i2c_rf_write_data(0x3f,&page0_val,1);
    if(ret)
        goto err;
    SXS_TRACE(TSTDOUT, "Rda_wf_setup_A2_reg finished\n");
    return 0;


err:
    SXS_TRACE(TSTDOUT, "Rda_wf_setup_A2_reg failed\n");
    return ret;

}




HAL_ERR_T Rda_wf_init_rf(void)
{
    INT32 i =0;
    SXS_TRACE(TSTDOUT, "Rda_wf_init_rf start\n");
    HAL_ERR_T halErr;
    for(i=0; i<sizeof(wifi_rf_init_data)/sizeof(wifi_rf_init_data[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(wifi_rf_init_data[i][0], &wifi_rf_init_data[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_init_rf failed\n");
            return halErr;
        }
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_init_rf finish\n");
    return 0;
}




HAL_ERR_T Rda_wf_dc_cal(void)
{
    INT32 i =0;
    HAL_ERR_T halErr;

    SXS_TRACE(TSTDOUT, "Rda_wf_dc_cal start\n");
    for(i=0; i<sizeof(wifi_dc_cal_data)/sizeof(wifi_dc_cal_data[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(wifi_dc_cal_data[i][0], &wifi_dc_cal_data[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_dc_cal failed\n");
            return halErr;
        }
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_dc_cal finish\n");
    return 0;
}

HAL_ERR_T Rda_wf_mdll_reset(void)
{
    INT32 i =0;
    HAL_ERR_T halErr;

    SXS_TRACE(TSTDOUT, "Rda_wf_mdll_reset\n");
    for(i=0; i<sizeof(wifi_mdll_reset_data)/sizeof(wifi_mdll_reset_data[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(wifi_mdll_reset_data[i][0], &wifi_mdll_reset_data[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_mdll_reset failed\n");
            return halErr;
        }
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_mdll_reset finish\n");
    return 0;
}







HAL_ERR_T Rda_wf_dig_reset(void)
{
    INT32 i =0;
    HAL_ERR_T halErr;

    SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset start\n");
    for(i=0; i<sizeof(wifi_dig_reset_data)/sizeof(wifi_dig_reset_data[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(wifi_dig_reset_data[i][0], &wifi_dig_reset_data[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset failed\n");
            return halErr;
        }
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset finish\n");
    return 0;
}

#ifdef RDA5990_WIFI_USE_DCDC_MODE
void RDA5990bt_wf_dcdc_ldo_trans(void);
#endif



void Rda_wf_init(void)
{
    HAL_ERR_T halErr;
    SXS_TRACE(TSTDOUT, "Rda_wf_init start\n");

#ifdef RDA5990_WIFI_USE_DCDC_MODE
    RDA5990bt_wf_dcdc_ldo_trans();
#endif

    halErr=Rda_wf_poweron();
    if(halErr)
        goto err;
    WIFI_DELAY(50);
    halErr=Rda_wf_setup_A2_reg(1);
    if(halErr)
        goto err;


    WIFI_DELAY(10);

    halErr=Rda_wf_init_rf();
    if(halErr)
        goto err;

    WIFI_DELAY(50);
    halErr=Rda_wf_dc_cal();
    if(halErr)
        goto err;
    WIFI_DELAY(50);
    halErr=Rda_wf_mdll_reset();
    if(halErr)
        goto err;


    WIFI_DELAY(50);

    halErr=Rda_wf_dig_reset();
    if(halErr)
        goto err;
    WIFI_DELAY(50);
    return ;


err:
    SXS_TRACE(TSTDOUT, "Rda_wf_init failed\n");
    return ;

}

BOOL wifi_no_off = FALSE;

void wifi_init(void)
{
    wifi_no_off = TRUE;
    SXS_TRACE(TSTDOUT, "wifi_init.");
//setclockmode();
#ifdef USE_32K_CLOCK_PIN
    SXS_TRACE(TSTDOUT,"define USE_32K_CLOCK_PIN\n");

    if (!g_wifi32kClockRequested)
    {
        hal_Sys32kClkOut(TRUE);
        g_wifi32kClockRequested = TRUE;
    }
    if(g_wifi26mClockReq ==FALSE)
    {
        hal_SysAuxClkOut(TRUE);
        g_wifi26mClockReq = TRUE;
    }
#else
    SXS_TRACE(TSTDOUT,"undefine USE_32K_CLOCK_PIN\n");

    if (!g_wifi32kClockRequested)
    {
        g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
        if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == g_WifiClockOutId)
        {
            SXS_TRACE(TSTDOUT," wifi 32k clock error%d\n");
            return FALSE;
        }
        g_wifi32kClockRequested = TRUE;
    }
    if(g_wifi26mClockReq ==FALSE)
    {
        hal_SysAuxClkOut(TRUE);
        g_wifi26mClockReq = TRUE;
    }
#endif

    //g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);

    hal_I2cOpen(g_wifiI2cBusId);

    // SXS_TRACE(TSTDOUT, "open 32k clock, ret:0x%x .",g_WifiClockOutId);

//    WIFI_DELAY(10);
    //wifi_Read_rf_data();
    //WIFI_DELAY(50);
    wifi_Read_Version();
    WIFI_DELAY(50);

#if 0
    wifi_Write_rf_data();

#else
    Rda_wf_init();

#endif


    //WIFI_DELAY(50);
//  wifi_Read_rf_data();
    //WIFI_DELAY(100);

    wifi_Write_core_data();
    //WIFI_DELAY(200);

    //wifi_Read_core_data();
    //WIFI_DELAY(200);


    WIFI_DELAY(50);
    hal_I2cClose(g_wifiI2cBusId);


    if(g_wifi26mClockReq ==TRUE)
    {
        //hal_SysAuxClkOut(FALSE);
        //g_wifi26mClockReq = FALSE;
    }
#ifndef USE_32K_CLOCK_PIN

    //    hal_SysClkOutClose(g_WifiClockOutId);
#endif

}
void wifi_PowerOffRsp(void);
VOID Wifi_Sdio_Close(VOID);

void wifi_PowerOff_wifi(void)
{
    if(wifi_no_off == TRUE)
    {
        wifi_no_off = FALSE;
    }
    else
    {
        wifi_PowerOffRsp();
        return;
    }

    SXS_TRACE(TSTDOUT, "wifi_PowerOff.");
//Wifi_Sdio_Close();
    Wifi_Sdio_Close();

    hal_I2cOpen(g_wifiI2cBusId);
    UINT8 tmp_reg = 0x3F;
    UINT16 tmp_val =0x0001,data,poweroff;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    tmp_reg = 0x31;
    tmp_val =0x0140;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    wifi_i2c_rf_read_data(0x28,&data,1);
    if(data&0x8000)
    {

        tmp_val=0x0000;
        wifi_i2c_rf_write_data(0x3f, &tmp_val,1);
        tmp_val=0x2223;
        wifi_i2c_rf_write_data(0x0f, &tmp_val,1);
        hal_HstSendEvent(0xddaa3333);
    }
    else
    {
        wifi_i2c_rf_read_data(0x22,&data,1);
        poweroff = data & 0xfdff;
        wifi_i2c_rf_write_data(0x22,&poweroff,1);

        hal_HstSendEvent(0xddaa444);
    }

    hal_I2cClose(g_wifiI2cBusId);

    wifi_PowerOffRsp();
}
void wifi_PowerOff_wifi_test_mode(void)
{
    if(wifi_no_off == TRUE)
    {
        wifi_no_off = FALSE;
    }
    else
    {
        return;
    }

    SXS_TRACE(TSTDOUT, "wifi_PowerOff.");
//Wifi_Sdio_Close();
    Wifi_Sdio_Close();

    hal_I2cOpen(g_wifiI2cBusId);
    UINT8 tmp_reg = 0x3F;
    UINT16 tmp_val =0x0001,data,poweroff;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    tmp_reg = 0x31;
    tmp_val =0x0140;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    wifi_i2c_rf_read_data(0x28,&data,1);
    if(data&0x8000)
    {

        tmp_val=0x0000;
        wifi_i2c_rf_write_data(0x3f, &tmp_val,1);
        tmp_val=0x2223;
        wifi_i2c_rf_write_data(0x0f, &tmp_val,1);
        hal_HstSendEvent(0xddaa3333);
    }
    else
    {
        wifi_i2c_rf_read_data(0x22,&data,1);
        poweroff = data & 0xfdff;
        wifi_i2c_rf_write_data(0x22,&poweroff,1);

        hal_HstSendEvent(0xddaa444);
    }

    hal_I2cClose(g_wifiI2cBusId);

    //   wifi_PowerOffRsp();
}

void wifi_PowerOffEx(void)
{
    SXS_TRACE(TSTDOUT, "wifi_PowerOffEx.");
    Wifi_Sdio_Close();
    hal_I2cOpen(g_wifiI2cBusId);
    UINT8 tmp_reg = 0x3F;
    UINT16 tmp_val =0x0001;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    tmp_reg = 0x31;
    tmp_val =0x0140;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);

    hal_I2cClose(g_wifiI2cBusId);

}

void wifi_close(void)
{
    SXS_TRACE(TSTDOUT, "wifi_close.");
    Wifi_Sdio_Close();
    hal_I2cOpen(g_wifiI2cBusId);
    UINT8 tmp_reg = 0x3F;
    UINT16 tmp_val =0x0001,data,poweroff;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    tmp_reg = 0x31;
    tmp_val =0x0140;
    wifi_i2c_rf_write_data(tmp_reg, &tmp_val,1);
    wifi_i2c_rf_read_data(0x28,&data,1);
    if(data&0x8000)
    {

        tmp_val=0x0000;
        wifi_i2c_rf_write_data(0x3f, &tmp_val,1);
        tmp_val=0x2223;
        wifi_i2c_rf_write_data(0x0f, &tmp_val,1);
        hal_HstSendEvent(0xddaa3333);
    }
    else
    {
        wifi_i2c_rf_read_data(0x22,&data,1);
        poweroff = data & 0xfdff;
        wifi_i2c_rf_write_data(0x22,&poweroff,1);

        hal_HstSendEvent(0xddaa444);
    }

    hal_I2cClose(g_wifiI2cBusId);




}
void wifi_setTransMode(void)
{
    SXS_TRACE(TSTDOUT, "wifi_setTransMode.");

    hal_I2cOpen(g_wifiI2cBusId);

    UINT32 transmode =0x01;
    UINT32 transmodev =0x00;
#ifndef USE_32K_CLOCK_PIN
// g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);;
#endif
    wifi_i2c_core_read_data(0x0010190c,&transmodev,1);
    SXS_TRACE(TSTDOUT, "before write 0x0010190c, read value: 0x%x.",transmodev);
    wifi_i2c_core_write_data(0x0010190c,&transmode,1);
    WIFI_DELAY(500);
//   transmode =0x03;

//  wifi_i2c_core_write_data(0x00101E18,&transmode,1);
#ifndef USE_32K_CLOCK_PIN
    //      hal_SysClkOutClose(g_WifiClockOutId);
#endif
//  WIFI_DELAY(100);
    transmodev =0x00;
    wifi_i2c_core_read_data(0x0010190c,&transmodev,1);
    SXS_TRACE(TSTDOUT, "after write 0x0010190c, read value: 0x%x.",transmodev);
//   wifi_i2c_core_read_data(0x00101E18,&transmodev,1);
//  SXS_TRACE(TSTDOUT, "after write 0x00101E18, read value: 0x%x.",transmodev);


    hal_I2cClose(g_wifiI2cBusId);

}



HAL_ERR_T wifi_set_uart_to_wifi(void)
{
    INT32 i =0;
    HAL_ERR_T halErr;

    SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset start\n");
    for(i=0; i<sizeof(wifi_poweron_uart)/sizeof(wifi_poweron_uart[0]); i++)
    {
        halErr= wifi_i2c_rf_write_data(wifi_poweron_uart[i][0], &wifi_poweron_uart[i][1],1);
        if(halErr!=0)
        {
            SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset failed\n");
            return halErr;
        }
    }

    SXS_TRACE(TSTDOUT, "Rda_wf_dig_reset finish\n");
    return 0;
}




void wifi_test_mode_init(void)
{

    wifi_PowerOff_wifi_test_mode();
    WIFI_DELAY(200);
    wifi_no_off = TRUE;
    SXS_TRACE(TSTDOUT, "wifi test mode init.");
//setclockmode();
#ifdef USE_32K_CLOCK_PIN
    SXS_TRACE(TSTDOUT,"define WIFI_USE_32K_CLK_PIN\n");

    if (!g_wifi32kClockRequested)
    {
        hal_Sys32kClkOut(TRUE);
        g_wifi32kClockRequested = TRUE;
    }
    if(g_wifi26mClockReq ==FALSE)
    {
        hal_SysAuxClkOut(TRUE);
        g_wifi26mClockReq = TRUE;
    }
#else
    SXS_TRACE(TSTDOUT,"undefine WIFI_USE_32K_CLK_PIN\n");

    if (!g_wifi32kClockRequested)
    {
        g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
        if (HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE == g_WifiClockOutId)
        {
            SXS_TRACE(TSTDOUT," wifi 32k clock error%d\n");
            return FALSE;
        }
        g_wifi32kClockRequested = TRUE;
    }
    if(g_wifi26mClockReq ==FALSE)
    {
        hal_SysAuxClkOut(TRUE);
        g_wifi26mClockReq = TRUE;
    }
#endif

    //g_WifiClockOutId =    hal_SysClkOutOpen(HAL_SYS_CLOCK_OUT_FREQ_32K);
    hal_I2cOpen(g_wifiI2cBusId);
    WIFI_DELAY(5);
#if 0
    wifi_Write_rf_data();
#else
    Rda_wf_init();
#endif
    //wifi_Write_core_data();
    WIFI_DELAY(200);
    wifi_set_uart_to_wifi();
    WIFI_DELAY(200);
    hal_I2cClose(g_wifiI2cBusId);

    if(g_wifi26mClockReq ==TRUE)
    {
        hal_SysAuxClkOut(FALSE);
        //g_wifi26mClockReq = FALSE;
    }
}




void wifi_test_notch(void)
{

    SXS_TRACE(TSTDOUT, "wifi_Write_core_data.");

    INT32 i, t;
    t = 0;
    hal_I2cOpen(g_wifiI2cBusId); WIFI_DELAY(5);

/// t = 0;
    for(i=t; i<sizeof(wifi_core_notch_test_mode_notch)/sizeof(wifi_core_notch_test_mode_notch[0]); i++)
    {
        //while(hal_I2cIsBusy(g_wifiI2cBusId)) ;

        wifi_i2c_core_write_data(wifi_core_notch_test_mode_notch[i][0],&wifi_core_notch_test_mode_notch[i][1],1);
        WIFI_DELAY(5);
    }
    hal_I2cClose(g_wifiI2cBusId);

}







