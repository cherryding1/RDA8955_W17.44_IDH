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

#ifndef _RDABT_SPP_H_
#define _RDABT_SPP_H_


void SPP_Uart_Init(void);


kal_bool SPP_Open(UART_PORT port, module_type ownerid);
void SPP_Close(UART_PORT port, module_type ownerid);


kal_uint16 SPP_GetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid);


kal_uint16 SPP_PutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);


kal_uint16 SPP_GetRxAvail(UART_PORT port);


kal_uint16 SPP_GetTxAvail(UART_PORT port);


kal_uint16 SPP_PutISRBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);


kal_uint16 SPP_GetISRTxAvail(UART_PORT port);


void SPP_Purge(UART_PORT port, UART_buffer dir, module_type ownerid);


void SPP_SetOwner(UART_PORT port, module_type ownerid);


void SPP_SetFlowCtrl(UART_PORT port, kal_bool XON, module_type ownerid);


void SPP_ConfigEscape(UART_PORT port, kal_uint8 EscChar, kal_uint16 ESCGuardtime, module_type ownerid);


void SPP_SetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config, module_type ownerid);


void SPP_CtrlDCD(UART_PORT port, IO_level SDCD, module_type ownerid);


void SPP_CtrlBreak(UART_PORT port, IO_level SBREAK, module_type ownerid);


void SPP_ClrRxBuffer(UART_PORT port, module_type ownerid);


void SPP_ClrTxBuffer(UART_PORT port, module_type ownerid);


void SPP_SetBaudRate(UART_PORT port, UART_baudrate baudrate, module_type ownerid);


kal_uint16 SPP_SendISRData(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length,kal_uint8 mode, kal_uint8 escape_char, module_type ownerid);


kal_uint16 SPP_SendData(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length,kal_uint8 mode,kal_uint8 escape_char, module_type ownerid);


module_type SPP_GetOwnerID(UART_PORT port);


void SPP_SetAutoBaud_Div(UART_PORT port, module_type ownerid);


void SPP_Register_TX_cb(UART_PORT port, module_type ownerid, UART_TX_FUNC func);


void SPP_Register_RX_cb(UART_PORT port, module_type ownerid, UART_RX_FUNC func);


kal_uint8 SPP_GetUARTByte(UART_PORT port);


void SPP_PutUARTByte(UART_PORT port, kal_uint8 data);


void SPP_PutUARTBytes(UART_PORT port, kal_uint8 *data, kal_uint16 len);


void SPP_ReadDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config);


void SPP_CtrlRI(UART_PORT port, IO_level SRI, module_type ownerid);


void SPP_CtrlDTR(UART_PORT port, IO_level SDTR, module_type ownerid)    ;


void SPP_ReadHWStatus(UART_PORT port, IO_level *SDSR, IO_level *SCTS);


void rdabt_spp_int_msg_hdlr( kal_uint16 mi,void *mv );
void rdabt_spp_msg_hdler(ilm_struct *message);

u_int32 rdabt_spp_get_tx_data(u_int8 port, u_int8*data, u_int16 length);
u_int8 rdabt_spp_set_rx_data(u_int8 port, u_int8 *data, u_int32 length);
u_int8 rdabt_spp_register_port(u_int8 profile);
u_int8 rdabt_spp_deregister_port(u_int8 port);
#endif
