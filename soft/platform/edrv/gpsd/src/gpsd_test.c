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
#include "sxr_ops.h"
#include "sxs_io.h"
#include "cs_types.h"
#include "sxr_tls.h"
#include "gpsddp_debug.h"
#include "gpsd_config.h"
#include "gpsd_m.h"
#include "tgt_gpsd_cfg.h"
#include "hal_host.h"
#include "gpsd_hw_interface.h"
#include "cos.h"
#include "event.h"
#include "at_common.h"
#include "at_define.h"
#include "pmd_m.h"
#include "at_trace.h"

// ============================================================================
// LOCAL DEFINITIONS
// ============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================



// =============================================================================
//  FUNCTIONS
// =============================================================================
#define RDAGPS_RX_BUFF_SIZE  4096      /* maximum size of Rx buffer */
UINT8  GPSRx_buff[RDAGPS_RX_BUFF_SIZE]; /* Rx buffer in local side */

#define MCU_RX_BUFF_SIZE  4096      /* maximum size of Rx buffer */
UINT8  mcuRx_buff[MCU_RX_BUFF_SIZE]; /* Rx buffer in local side */


RAD5900MODE g_gpsMode=NEMA_MODE;



UINT32 rdaGPS_uart_rx(VOID)
{
   UINT32 rx_length;
   UINT8 status;
   memset(GPSRx_buff,0,sizeof(GPSRx_buff));
   rx_length = GPS_UART_GetBytes(GPS_DEFAULT_UART, GPSRx_buff, RDAGPS_RX_BUFF_SIZE, &status);
   if(rx_length >0)
   {

	//hal_HstSendEvent(rx_length);
#if 0

	if(rx_length > 50)
	{
		//rx_length = 50;
		AT_TC(g_sw_GPRS, "%s", GPSRx_buff);
		;//AT_WriteUart(GPSRx_buff,rx_length);
	}
#else
		UINT8* pUartData = NULL;
		COS_EVENT event = { 0 };		
		BOOL r			= FALSE;
		
		pUartData = AT_MALLOC(rx_length+2);
		if(NULL == pUartData)
		{
			hal_DbgAssert(0);
		}
		AT_MemSet(pUartData, 0, rx_length+2);

		memcpy(pUartData, GPSRx_buff, rx_length);
			
		event.nEventId                           = EV_GPS_RECV_IND;
		event.nParam1                            = (UINT32)(pUartData);
		event.nParam2                            = (UINT32)rx_length;
		r = COS_SendEvent(GetTaskHandle_GPS(), &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
#endif
   }   
   return rx_length;
}

BOOL isAT(UINT8 first,UINT8 second)
{
	BOOL result = FALSE;
	if((first == 'A' && second == 'T')||(first == 'a' && second == 't'))
	{
		result =  TRUE;
	}
	return result;
}

void gps_uart_open(void)
{
	UARTDCBStruct dcb =
	{
		HAL_UART_BAUD_RATE_115200,	/* baud */
		HAL_UART_8_DATA_BITS,		/* dataBits; */
		HAL_UART_1_STOP_BIT,		/* stopBits; */
		HAL_UART_NO_PARITY, 		/* parity; */
		HAL_UART_AFC_MODE_DISABLE,	/* flowControl*/
		0,							/* xonChar; */
		0,							/* xoffChar; */
		0,							/* DSRCheck*/
	};		
	gpsd_uart_open(GPS_DEFAULT_UART, &dcb); 		
	gpsd_uart_register_rx_cb(GPS_DEFAULT_UART,rdaGPS_uart_rx);

}

void gps_uart_close(void)
{
	UARTDCBStruct dcb =
	{
		HAL_UART_BAUD_RATE_115200,	/* baud */
		HAL_UART_8_DATA_BITS,		/* dataBits; */
		HAL_UART_1_STOP_BIT,		/* stopBits; */
		HAL_UART_NO_PARITY, 		/* parity; */
		HAL_UART_AFC_MODE_DISABLE,	/* flowControl*/
		0,							/* xonChar; */
		0,							/* xoffChar; */
		0,							/* DSRCheck*/
	};		
	gpsd_uart_close(GPS_DEFAULT_UART, &dcb); 		
	gpsd_uart_register_rx_cb(GPS_DEFAULT_UART,NULL);

}



