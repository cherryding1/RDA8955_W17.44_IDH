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

#ifndef __RDABT_UART_H__
#define __RDABT_UART_H__
#include "cs_types.h"
//typedef void (*UART_RX_CB_FUNC)(u_int8 *buff,u_int32 len) ;



void rdabt_adp_uart_configure(unsigned baud,char flow_control);


void rdabt_adp_uart_stop(void);

void rdabt_adp_uart_start(void);

unsigned int rdabt_adp_uart_rx(void);
unsigned char rdabt_adp_uart_tx(const kal_uint8 *buf, unsigned short num_to_send, unsigned short *num_send);
void rdabt_uart_eint_hdlr(void);

void rdabt_uart_rx_cb(UART_PORT port);

void rdabt_uart_register_cb(void);

#endif
