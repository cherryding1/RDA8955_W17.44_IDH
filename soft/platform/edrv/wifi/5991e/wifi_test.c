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

﻿

#include "cs_types.h"
#include "kal_release.h"        /* basic data type */
#include <hal_debug.h>
#include "hal_uart.h"
#include "sxr_mem.h"
#include "sxr_sbx.h"
#include "sxs_io.h"
#include <string.h>
#include <stdio.h>

#include "cos.h"
#include "uart_sw.h"


#define rdawifi_uart_stop rdabt_adp_uart_stop
#define rdawifi_uart_start rdabt_adp_uart_start
#define rdawifi_uart_configure rdabt_adp_uart_configure
#define rdawifi_uart_rx rdabt_adp_uart_rx
#define rdawifi_uart_tx rdabt_adp_uart_tx
#define rdawifi_uart_set_rx_cb rdabt_adp_uart_set_rx_cb

typedef enum
{
    NO_TEST,
    RX_TEST,
    TX_TEST,
    CTA_A_TEST,
    CTA_B_TEST,
    CTA_C_TEST,
    OTHER
} WIFI_TEST_MODE;


#define WIFI_TEST_DELAY(DURATION) sxr_Sleep(DURATION  MS_WAITING)

WIFI_TEST_MODE g_testmode = 0;


#define BT_UART_PORT    1
void showString(UINT8 * str);

UINT32 wifi_test_Rx_ind(void);
UINT32 wifi_test_info_ind(UINT8* info, UINT16 len);

void wifi_test_Uart_Rx_CB(UART_PORT port)
{
    wifi_test_Rx_ind();
}
#define BUF_SIZE   300
void wifi_handle_received_data_cb(uint8 *buff,uint32 len)
{

    UINT32 len_tmp = len;
    SXS_TRACE(TSTDOUT, "wifi test mode, receive %d bytes data : ",len);
    UINT8 tmp_buf[BUF_SIZE +1];

    UINT8* ptr = NULL;

    while(len_tmp > 0)
    {

        memset(tmp_buf,0x00, BUF_SIZE+1);
        if(len_tmp > BUF_SIZE)
        {
            memcpy(tmp_buf, (buff + len - len_tmp), BUF_SIZE);

            ptr = COS_MALLOC(BUF_SIZE+1);
            memset(ptr,0x00, BUF_SIZE+1);
            memcpy(ptr, (buff + len - len_tmp), BUF_SIZE);
            wifi_test_info_ind(ptr, BUF_SIZE);

            len_tmp -=200;

        }
        else
        {
            memcpy(tmp_buf, (buff + len - len_tmp), len_tmp);

            ptr = COS_MALLOC(BUF_SIZE+1);
            memset(ptr,0x00, BUF_SIZE+1);
            memcpy(ptr, (buff + len - len_tmp), len_tmp);
            wifi_test_info_ind(ptr, len_tmp);

            len_tmp =0;

        }
        showString("wifi receive: ");
        showString(tmp_buf);

        // sxs_Dump(_MMI, 0, tmp_buf, 100);
    }

}

static const  char* wifi_test_table_tx[]=
{
    "sk\r\n",
    "setdbga 1\r\n",
    "utm 2\r\n",// set Rf mode
    "setr 1\r\n",// set rate. (range 1 ~ 54 ), 1,11,18,54..
    "setch 1\r\n",// set channel (range 1 ~14)
    "uttxlen 50\r\n", // set the length of transmit pack.(range 10 ~ 1000)

};
UINT16 g_dbga =1;
UINT16 g_rate= 54;
UINT16 g_channe=1;
UINT16 g_txlen=50;


static const  char* wifi_test_table_rx[]=
{
    "sk\r\n",
    "utm 1\r\n",// set Rf mode
    "setdbga 401\r\n",// set rate. (range 1 ~ 54 )
    "setch 1\r\n",// set channel (range 1 ~14)

};
static const  char* wifi_test_table_CTA_A[]=
{
    "sk\r\n",
    "setdbga 1\r\n",
    "utm 2\r\n",// set Rf mode
    "setr 1\r\n",// set rate. (range 1 ~ 54 ), 1,11,18,54..
    "setch 1\r\n",// set channel (range 1 ~14)
    "uttxlen 1000\r\n", // set the length of transmit pack.(range 10 ~ 1000)
    "phyregw 8c 59\r\n",

    "phyregw 8d 0d\r\n",

};
static const  char* wifi_test_table_CTA_B[]=
{
    "sk\r\n",
    "setdbga 1\r\n",
    "utm 2\r\n",// set Rf mode
    "setr 1\r\n",// set rate. (range 1 ~ 54 ), 1,11,18,54..
    "setch 1\r\n",// set channel (range 1 ~14)
    "uttxlen 1000\r\n", // set the length of transmit pack.(range 10 ~ 1000)
    "phyregw d3 40\r\n",

    "phyregw d4 44\r\n",

    "phyregw d5 80\r\n",

    "phyregw d3 c0\r\n",

};

static const  char* wifi_test_table_CTA_C[]=
{
    // "sk\r\n",
    // "setdbga 1\r\n",
    //"utm 2\r\n",// set Rf mode
    // "setr 1\r\n",// set rate. (range 1 ~ 54 ), 1,11,18,54..
    // "setch 1\r\n",// set channel (range 1 ~14)
    // "uttxlen 1000\r\n", // set the length of transmit pack.(range 10 ~ 1000)
    //  "phyregw 8c 59",

    "rfregw 22 2c9c\r\n",

    "rfregw 24 04f0\r\n",


    "rfregw 30 0249\r\n",

    "rfregw 30 024D\r\n",

};
extern void wifi_test_mode_init(void);
//extern void wifi_test_patch2(void);
//extern void wifi_test_notch(void);

void wifi_test_init(void )
{
    SXS_TRACE(TSTDOUT, "in wifi_test_init");
    wifi_test_mode_init();
    rdawifi_uart_stop();
    rdawifi_uart_set_rx_cb(wifi_handle_received_data_cb);
    rdawifi_uart_start();
    rdawifi_uart_configure(115200,FALSE);
    //  GPIO_INT_Registration(g_btdConfig->pinSleep,0,rdabt_uart_eint_hdlr);
    //  rdabt_uart_register_eint();
    //rdawifi_uart_register_cb();
    UART_Register_RX_cb(BT_UART_PORT,0,wifi_test_Uart_Rx_CB);
}
void wifi_test_rx(void)
{
    rdawifi_uart_rx();
}

void wifi_test_tx(const uint8 *buf, unsigned short num_to_send, unsigned short *num_send)
{
    UINT8 ret;
    SXS_TRACE(TSTDOUT, "len: %d,  wifi set to MAC:  ",num_to_send);
    showString(buf);

    ret = rdawifi_uart_tx(buf, num_to_send, num_send);
}

void wifi_test_RX_MODE(void)
{
    UINT16 sent_len;
    wifi_test_tx(wifi_test_table_rx[0], strlen(wifi_test_table_rx[0]), &sent_len);
    COS_Sleep(200);
    wifi_test_tx(wifi_test_table_rx[1], strlen(wifi_test_table_rx[1]), &sent_len);
    COS_Sleep(50);
    //  wifi_test_patch2();//write digital patch2
    wifi_test_tx(wifi_test_table_rx[2], strlen(wifi_test_table_rx[2]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_rx[3], strlen(wifi_test_table_rx[3]), &sent_len);
    //  do not need write notch for wifi digital do not
}
void wifi_test_TX_MODE(void)
{
    UINT16 sent_len;
    wifi_test_tx(wifi_test_table_tx[0], strlen(wifi_test_table_tx[0]), &sent_len);
    COS_Sleep(200);
    wifi_test_tx(wifi_test_table_tx[1], strlen(wifi_test_table_tx[1]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_tx[2], strlen(wifi_test_table_tx[2]), &sent_len);

    COS_Sleep(50);
    wifi_Write_MAC_Reg_I2C_rf(0x3f, 0x0000);
    wifi_Write_MAC_Reg_I2C_rf(0x36, 0x4000);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_tx[3], strlen(wifi_test_table_tx[3]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_tx[4], strlen(wifi_test_table_tx[4]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_tx[5], strlen(wifi_test_table_tx[5]), &sent_len);
}
void wifi_test_CTA_A_MODE(void)
{
    UINT16 sent_len;
    wifi_test_tx(wifi_test_table_CTA_A[0], strlen(wifi_test_table_CTA_A[0]), &sent_len);
    COS_Sleep(200);
    wifi_test_tx(wifi_test_table_CTA_A[1], strlen(wifi_test_table_CTA_A[1]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[2], strlen(wifi_test_table_CTA_A[2]), &sent_len);

    COS_Sleep(50);
    wifi_Write_MAC_Reg_I2C_rf(0x3f, 0x0000);
    wifi_Write_MAC_Reg_I2C_rf(0x36, 0x4000);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[3], strlen(wifi_test_table_CTA_A[3]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[4], strlen(wifi_test_table_CTA_A[4]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[5], strlen(wifi_test_table_CTA_A[5]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[6], strlen(wifi_test_table_CTA_A[6]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_A[7], strlen(wifi_test_table_CTA_A[7]), &sent_len);
}
void wifi_test_CTA_B_MODE(void)
{
    UINT16 sent_len;
    wifi_test_tx(wifi_test_table_CTA_B[0], strlen(wifi_test_table_CTA_B[0]), &sent_len);
    COS_Sleep(200);
    wifi_test_tx(wifi_test_table_CTA_B[1], strlen(wifi_test_table_CTA_B[1]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[2], strlen(wifi_test_table_CTA_B[2]), &sent_len);

    COS_Sleep(50);
    wifi_Write_MAC_Reg_I2C_rf(0x3f, 0x0000);
    wifi_Write_MAC_Reg_I2C_rf(0x36, 0x4000);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[3], strlen(wifi_test_table_CTA_B[3]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[4], strlen(wifi_test_table_CTA_B[4]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[5], strlen(wifi_test_table_CTA_B[5]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[6], strlen(wifi_test_table_CTA_B[6]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[7], strlen(wifi_test_table_CTA_B[7]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[8], strlen(wifi_test_table_CTA_B[8]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_B[9], strlen(wifi_test_table_CTA_B[9]), &sent_len);
}
void wifi_test_CTA_C_MODE(void)
{
    UINT16 sent_len;
    wifi_test_tx(wifi_test_table_CTA_C[0], strlen(wifi_test_table_CTA_C[0]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_C[1], strlen(wifi_test_table_CTA_C[1]), &sent_len);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_C[2], strlen(wifi_test_table_CTA_C[2]), &sent_len);

    //COS_Sleep(50);
    //wifi_Write_MAC_Reg_I2C_rf(0x3f, 0x0000);
    //wifi_Write_MAC_Reg_I2C_rf(0x36, 0x4000);
    COS_Sleep(50);
    wifi_test_tx(wifi_test_table_CTA_C[3], strlen(wifi_test_table_CTA_C[3]), &sent_len);
//  COS_Sleep(50);
//  wifi_test_tx(wifi_test_table_CTA_B[4], strlen(wifi_test_table_CTA_B[4]), &sent_len);
}
#if 1
void wifi_Write_MAC_Reg_I2C_rf(UINT8 reg, UINT16 val);

void wifi_test_mode_open(WIFI_TEST_MODE mode)
{
    SXS_TRACE(TSTDOUT, "wifi_test_mode_open,mode :%d  ",mode);

    UINT16 sent_len;
    wifi_test_init();
    g_testmode = mode;
    SXS_TRACE(TSTDOUT, "wifi_test_mode_open,mode :%d  ",mode);

    switch ( mode)
    {
        case  RX_TEST:

            /*{

                wifi_test_tx(wifi_test_table_rx[0], strlen(wifi_test_table_rx[0]), &sent_len);
                       COS_Sleep(200);
                wifi_test_tx(wifi_test_table_rx[1], strlen(wifi_test_table_rx[1]), &sent_len);
                       COS_Sleep(50);
              //  wifi_test_patch2();//write digital patch2
                wifi_test_tx(wifi_test_table_rx[2], strlen(wifi_test_table_rx[2]), &sent_len);
                       COS_Sleep(50);
                wifi_test_tx(wifi_test_table_rx[3], strlen(wifi_test_table_rx[3]), &sent_len);
                //  do not need write notch for wifi digital do not
            }*/
            wifi_test_RX_MODE();
            break;

        case TX_TEST:
            /*
                            wifi_test_tx(wifi_test_table_tx[0], strlen(wifi_test_table_tx[0]), &sent_len);
                                   COS_Sleep(200);
                            wifi_test_tx(wifi_test_table_tx[1], strlen(wifi_test_table_tx[1]), &sent_len);
                                   COS_Sleep(50);
                            wifi_test_tx(wifi_test_table_tx[2], strlen(wifi_test_table_tx[2]), &sent_len);

                                   COS_Sleep(50);
                          wifi_Write_MAC_Reg_I2C_rf(0x3f, 0x0000);
                        wifi_Write_MAC_Reg_I2C_rf(0x36, 0x4000);
                            COS_Sleep(50);
                            wifi_test_tx(wifi_test_table_tx[3], strlen(wifi_test_table_tx[3]), &sent_len);
                                   COS_Sleep(50);
                            wifi_test_tx(wifi_test_table_tx[4], strlen(wifi_test_table_tx[4]), &sent_len);
                                   COS_Sleep(50);
                            wifi_test_tx(wifi_test_table_tx[5], strlen(wifi_test_table_tx[5]), &sent_len);
            */
            wifi_test_TX_MODE();
            break;
        case CTA_A_TEST:
            wifi_test_CTA_A_MODE();
            break;
        case CTA_B_TEST:
            wifi_test_CTA_B_MODE();
            break;
        case CTA_C_TEST:
            wifi_test_CTA_C_MODE();
            break;
        default:
            break;
    }
}

void wifi_test_mode_set(UINT16 dbga,UINT16 rate,UINT16 channel,UINT16 len)
{

    UINT16 sent_len;

    char wifi_dbga[20];
    char wifi_rate[20];
    char wifi_channel[20];
    char wifi_len[20];
    SXS_TRACE(TSTDOUT, "wifi test mode set parameter dbga %d, rate %d , chan  %d , len  %d: ",dbga,rate,channel,len);
    sprintf(wifi_dbga, "setdbga %d\r\n",dbga);
    sprintf(wifi_rate, "setr %d\r\n",rate);
    sprintf(wifi_channel, "setch %d\r\n",channel);
    sprintf(wifi_len, "uttxlen %d\r\n",len);
    SXS_TRACE(TSTDOUT, "wifi_test_mode_set, g_testmode :%d  ",g_testmode);
    if(g_dbga!=dbga)
    {
        g_dbga = dbga;
        if (g_testmode == TX_TEST)
            //      wifi_test_tx(wifi_dbga, strlen(wifi_dbga), &sent_len);
            SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

        WIFI_TEST_DELAY(10);

    }
    if(g_rate!=rate)
    {
        g_rate = rate;
        if (g_testmode == TX_TEST)
            wifi_test_tx(wifi_rate, strlen(wifi_rate), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);
        WIFI_TEST_DELAY(10);

    }
    if(g_channe!=channel)
    {
        g_channe = channel;
        wifi_test_tx(wifi_channel, strlen(wifi_channel), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

        WIFI_TEST_DELAY(10);
    }
    if(g_txlen!=len)
    {
        g_txlen = len;
        if (g_testmode == TX_TEST)
            wifi_test_tx(wifi_len, strlen(wifi_len), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

    }
    return;

#if 0

    if(g_testmode == RX_TEST)
    {
        SXS_TRACE(TSTDOUT, "wifi test mode set rx dbga %d, chan  %d: ",dbga,channel);

        // wifi_test_tx(wifi_dbga, 14, &sent_len);
        // WIFI_TEST_DELAY(10);
        wifi_test_tx(wifi_channel, strlen(wifi_channel), &sent_len);
        WIFI_TEST_DELAY(10);

    }
    else if (g_testmode == TX_TEST)
    {
        SXS_TRACE(TSTDOUT, "wifi test mode set Tx parameter dbga %d, rate %d , chan  %d , len  %d: ",dbga,rate,channel,len);
        wifi_test_tx(wifi_dbga, strlen(wifi_dbga), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

        WIFI_TEST_DELAY(10);
        wifi_test_tx(wifi_channel, strlen(wifi_channel), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

        WIFI_TEST_DELAY(10);
        wifi_test_tx(wifi_len, strlen(wifi_len), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

        WIFI_TEST_DELAY(10);
        wifi_test_tx(wifi_rate, strlen(wifi_rate), &sent_len);
        SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

    }
#endif


}

void wifi_test_mode_set_rfregw(UINT16 reg_addr,UINT16 val)
{

    UINT16 sent_len;

    char wifi_rfregw[30];

    SXS_TRACE(TSTDOUT, "wifi_test_mode_set_rfregw:regaddr: %2x, val: %4x  ",reg_addr,val);
    sprintf(wifi_rfregw, "rfregw %2x %4x\r\n",reg_addr,val);

    SXS_TRACE(TSTDOUT, "write to uart :%s  ",wifi_rfregw);

    wifi_test_tx(wifi_rfregw, strlen(wifi_rfregw), &sent_len);
    SXS_TRACE(TSTDOUT, "sent  len  %d: ",sent_len);

    WIFI_TEST_DELAY(10);


}
#else

#endif
extern void wifi_PowerOff(void);

void wifi_test_mode_close(void)
{

    wifi_PowerOff( );


}




