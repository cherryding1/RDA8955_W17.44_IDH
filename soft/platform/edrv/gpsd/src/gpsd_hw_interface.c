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
#include <cos.h>

#include "hal_uart.h"
#include "hal_gpio.h"
#include "i2c_master.h"
#include "hal_i2c.h"
#include "sxr_mem.h"
#include "sxr_sbx.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "sxr_tim.h"
#include "gpsddp_debug.h"
#include "gpsd_config.h"
#include "tgt_gpsd_cfg.h"
#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#endif

#include "gpsd_hw_interface.h"
#include "pmd_m.h"

#define MOD_BUFF_LEN1(_val, _length)  ((_val) & (_length-1))

PRIVATE DEVICE Uart_Dev[MAX_UART_DEV_NUM]={NULL,};


VOID gpsd_uart_set_dcb_config(HAL_UART_ID_T id, UARTDCBStruct *pDcb);
PRIVATE VOID gpsd_uart1_irq_handler (HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error) ;
PRIVATE VOID gpsd_uart2_irq_handler (HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error) ;




BOOL static Is_CircularBuffer_Empty(struct  CircularBuf *pCirBuff)
{
    return (!pCirBuff->NotEmpty);
}

UINT16 static get_CircularBuffer_len(struct  CircularBuf *pCirBuff)
{
   UINT16 Block_Len;
    Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

	if ((Block_Len == 0) && (pCirBuff->NotEmpty))
		Block_Len = pCirBuff->Buf_len;
	return Block_Len;	
}

UINT16 static put_data_to_CircularBuffer(struct  CircularBuf *pCirBuff, UINT8 *pBuff,UINT16 uDataSize)
{
	UINT16 Block_Len, Remain_Len,Remain_Len2, real_Len, First_half, Second_half;

	UINT32 status = hal_SysEnterCriticalSection();
    	
    // do thing with uart breakint not happen

	Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

	if ((Block_Len == 0) && (pCirBuff->NotEmpty))
	{
		Block_Len = pCirBuff->Buf_len;
	}

	Remain_Len = pCirBuff->Buf_len - Block_Len;
	if(Remain_Len < STATIC_RX_BUF_LEN)
	{
		GPSD_TRACE (GPSD_INFO_TRC, 0, "%s  buffer overflow!!",pCirBuff->Name);
		hal_SysExitCriticalSection(status);
		GPSD_ASSERT(0, "%s  buffer overflow1!!",pCirBuff->Name);
		return 0;
	}

	if (uDataSize > Remain_Len)
		GPSD_ASSERT(0, "%s  buffer overflow2!!",pCirBuff->Name);

	real_Len = uDataSize;// (uDataSize < Remain_Len) ? uDataSize : Remain_Len;

	if (real_Len > 0)/*circular buffer not full*/
	{
		if (pCirBuff->Put < pCirBuff->Get)
		{
			memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff, real_Len);
			pCirBuff->Put = MOD_BUFF_LEN1(pCirBuff->Put + real_Len,pCirBuff->Buf_len);
		}
		else
		{
			First_half = pCirBuff->Buf_len - pCirBuff->Put;

			if (real_Len < First_half)
			{
				memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff, real_Len);
				pCirBuff->Put = MOD_BUFF_LEN1(pCirBuff->Put + real_Len, pCirBuff->Buf_len);
			}
			else
			{
				memcpy(&pCirBuff->Buf[pCirBuff->Put], pBuff , First_half);
				Second_half = real_Len - First_half;
				pCirBuff->Put = MOD_BUFF_LEN1((pCirBuff->Put + First_half),pCirBuff->Buf_len);

				memcpy(&pCirBuff->Buf[pCirBuff->Put], &pBuff[First_half], Second_half);
				pCirBuff->Put = MOD_BUFF_LEN1((pCirBuff->Put + Second_half),pCirBuff->Buf_len);

			}

		}
		
		pCirBuff->NotEmpty = 1;
		
	}
	else
	{
		GPSD_TRACE (GPSD_INFO_TRC, 0, "warning put len %x",real_Len);

	}
	
  	hal_SysExitCriticalSection(status);

	Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

	if ((Block_Len == 0) && (pCirBuff->NotEmpty))
	{
		Block_Len = pCirBuff->Buf_len;
	}
	Remain_Len2 = pCirBuff->Buf_len - Block_Len;
	
	return (Remain_Len -real_Len);
}

UINT16 static get_data_from_CircularBuffer(struct  CircularBuf *pCirBuff, UINT8 *pBuff,UINT16 uDataSize)
{
	UINT16 Block_Len, First_half, Second_half, real_len, real_sent_len, tmp_Get;
	
	real_sent_len = 0;		
	
    UINT32 status = hal_SysEnterCriticalSection();
   
	Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);

	if ((Block_Len == 0) && (pCirBuff->NotEmpty))
		Block_Len = pCirBuff->Buf_len;

	real_len = (Block_Len < uDataSize) ? Block_Len :  uDataSize;
	if (real_len > 0)/*have data to send*/
	{
		
		if (pCirBuff->Get < pCirBuff->Put)
		{
			memcpy(pBuff, &pCirBuff->Buf[pCirBuff->Get], real_len);
			tmp_Get = MOD_BUFF_LEN1(pCirBuff->Get + real_len,pCirBuff->Buf_len);
		}
		else
		{

			First_half = pCirBuff->Buf_len  - pCirBuff->Get;

			if (real_len < First_half)
			{
				memcpy(pBuff, &pCirBuff->Buf[pCirBuff->Get], real_len);
				tmp_Get = MOD_BUFF_LEN1(pCirBuff->Get + real_len,pCirBuff->Buf_len);
			}
			else
			{
				memcpy(pBuff, &pCirBuff->Buf[pCirBuff->Get], First_half);
				Second_half = real_len - First_half;
				tmp_Get = MOD_BUFF_LEN1(pCirBuff->Get + First_half,pCirBuff->Buf_len);			
				
				memcpy(&pBuff[First_half], &pCirBuff->Buf[tmp_Get], Second_half);
				tmp_Get = MOD_BUFF_LEN1(tmp_Get + Second_half,pCirBuff->Buf_len);				
				
			}
		}	
		
		
		pCirBuff->Get  = tmp_Get;
		  if (pCirBuff->Get == pCirBuff->Put)
				pCirBuff->NotEmpty = 0;		
		   
	}
	
  	hal_SysExitCriticalSection(status);
	
	return real_len;
}

struct CircularBuf mcu_uartWriteBuf;
struct CircularBuf mcu_uartReadBuf;
struct Uart_hw_buf mcu_uartHwBuf;

BOOL mcud_uart_open(HAL_UART_ID_T id, UARTDCBStruct *pDcb)
{		
	DEVICE *pDev=&Uart_Dev[id-1];
	if(pDev->port!=0)
	{
		//GPSD_ASSERT(0, "error! Uart port %d  have existed",id);
        return FALSE;
	}
	pDev->port=id;
	pDev->RecMbx=NULL;


	pDev->pWriteBuf = &mcu_uartWriteBuf;//(struct CircularBuf*)GPSD_MALLOC( sizeof(struct CircularBuf) );
	if(pDev->pWriteBuf == NULL)
		goto _openError;
    memset(pDev->pWriteBuf, 0 , sizeof(struct  CircularBuf));
    pDev->pWriteBuf->Buf_len=CIRCULAR_BUF_LEN;	
    sprintf(pDev->pWriteBuf->Name, "Tx ");

	pDev->pReadBuf = &mcu_uartReadBuf;//(struct CircularBuf*)GPSD_MALLOC( sizeof(struct CircularBuf) );
	if(pDev->pReadBuf == NULL)
		goto _openError;
	memset(pDev->pReadBuf, 0 , sizeof(struct  CircularBuf));
	pDev->pReadBuf->Buf_len=CIRCULAR_BUF_LEN;
	sprintf(pDev->pReadBuf->Name, "Rx ");

//	GPSD_TRACE (GPSD_INFO_TRC, 0, "UART_Open: pDev->port = %d, pDev->pWriteBuf = 0x%x, pDev->pReadBuf=0x%x\n",pDev->port,pDev->pWriteBuf,pDev->pReadBuf);				

	pDev->pUart_hw_buf = &mcu_uartHwBuf;//(struct Uart_hw_buf*)GPSD_MALLOC( sizeof(struct Uart_hw_buf) );
	if(NULL==pDev->pUart_hw_buf->rx_buf)
		goto _openError;
	memset(pDev->pUart_hw_buf, 0 , sizeof(struct  Uart_hw_buf));


	 gpsd_uart_set_dcb_config(id,pDcb);

 	 return TRUE;

_openError:
    pDev->port=0;
    if(pDev->pWriteBuf!=NULL)
    {
        GPSD_FREE(pDev->pWriteBuf);
        pDev->pWriteBuf=NULL;
    }
    if(pDev->pReadBuf!=NULL)
    {
        GPSD_FREE(pDev->pReadBuf);
        pDev->pReadBuf=NULL;
    }
    if(pDev->pUart_hw_buf!=NULL)
    {
        GPSD_FREE(pDev->pUart_hw_buf);
        pDev->pUart_hw_buf=NULL;
    }
	
    return FALSE;
}

VOID mcud_uart_register_rx_cb(HAL_UART_ID_T id,VOID(*callfunc)(HAL_UART_ID_T id))
{
    DEVICE *pDev=&Uart_Dev[id-1];
    if(pDev->port!=id)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error2! wrong uart %d",id);
        return ;
    }
    pDev->registered_func=callfunc;
    return;
}

struct CircularBuf gps_uartWriteBuf;
struct CircularBuf gps_uartReadBuf;
struct Uart_hw_buf gps_uartHwBuf;


BOOL gpsd_uart_open(HAL_UART_ID_T id, UARTDCBStruct *pDcb)
{		
	DEVICE *pDev=&Uart_Dev[id-1];
	if(pDev->port!=0)
	{
		//GPSD_ASSERT(0, "error! Uart port %d  have existed",id);
        return FALSE;
	}
	pDev->port=id;
	pDev->RecMbx=NULL;


	pDev->pWriteBuf = &gps_uartWriteBuf;//(struct CircularBuf*)GPSD_MALLOC( sizeof(struct CircularBuf) );
	if(pDev->pWriteBuf == NULL)
		goto _openError;
    memset(pDev->pWriteBuf, 0 , sizeof(struct  CircularBuf));
    pDev->pWriteBuf->Buf_len=CIRCULAR_BUF_LEN;	
    sprintf(pDev->pWriteBuf->Name, "Tx ");

	pDev->pReadBuf = &gps_uartReadBuf;//(struct CircularBuf*)GPSD_MALLOC( sizeof(struct CircularBuf) );
	if(pDev->pReadBuf == NULL)
		goto _openError;
	memset(pDev->pReadBuf, 0 , sizeof(struct  CircularBuf));
	pDev->pReadBuf->Buf_len=CIRCULAR_BUF_LEN;
	sprintf(pDev->pReadBuf->Name, "Rx ");

//	GPSD_TRACE (GPSD_INFO_TRC, 0, "UART_Open: pDev->port = %d, pDev->pWriteBuf = 0x%x, pDev->pReadBuf=0x%x\n",pDev->port,pDev->pWriteBuf,pDev->pReadBuf);				

	pDev->pUart_hw_buf = &gps_uartHwBuf;//(struct Uart_hw_buf*)GPSD_MALLOC( sizeof(struct Uart_hw_buf) );
	if(NULL==pDev->pUart_hw_buf->rx_buf)
		goto _openError;
	memset(pDev->pUart_hw_buf, 0 , sizeof(struct  Uart_hw_buf));


	 gpsd_uart_set_dcb_config(id,pDcb);

 	 return TRUE;

_openError:
    pDev->port=0;
    if(pDev->pWriteBuf!=NULL)
    {
        GPSD_FREE(pDev->pWriteBuf);
        pDev->pWriteBuf=NULL;
    }
    if(pDev->pReadBuf!=NULL)
    {
        GPSD_FREE(pDev->pReadBuf);
        pDev->pReadBuf=NULL;
    }
    if(pDev->pUart_hw_buf!=NULL)
    {
        GPSD_FREE(pDev->pUart_hw_buf);
        pDev->pUart_hw_buf=NULL;
    }
	
    return FALSE;
}

VOID gpsd_uart_set_dcb_config(HAL_UART_ID_T id, UARTDCBStruct *pDcb)
{
	VOID (*Uart_ISR[2])(HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error);
 	DEVICE *pDev=&Uart_Dev[id-1];
	if(pDev->port!=id)
	{
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error1! wrong uart %d",id);
        return;
	}
	
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) && defined(GALLITE_IS_8805)
	pDev->RxUseDMA = FALSE;
	pDev->TxUseDMA = FALSE;
#else // 8806 or later
	pDev->RxUseDMA = TRUE;
	pDev->TxUseDMA = TRUE;
#endif

    if(pDev->RxUseDMA)
    {
        pDev->mask.rxDataAvailable = 0;
        pDev->mask.rxTimeout       = 0;
        pDev->mask.rxDmaDone       = 1;
        pDev->mask.rxDmaTimeout    = 1;

        pDev->uartCfg.rx_trigger = HAL_UART_RX_TRIG_1;
        pDev->uartCfg.rx_mode = HAL_UART_TRANSFERT_MODE_DMA_IRQ;
    }
    else
    {
        pDev->mask.rxDataAvailable = 1;
        pDev->mask.rxTimeout       = 1;
        pDev->mask.rxDmaDone       = 0;
        pDev->mask.rxDmaTimeout    = 0;

        pDev->uartCfg.rx_trigger = HAL_UART_RX_TRIG_HALF;
        pDev->uartCfg.rx_mode = HAL_UART_TRANSFERT_MODE_DIRECT_IRQ;
    }

    if(pDev->TxUseDMA)
    {
        pDev->mask.txModemStatus   = 0;
        pDev->mask.txDataNeeded    = 0;
        pDev->mask.txDmaDone       = 0; //1;

        pDev->uartCfg.tx_mode = HAL_UART_TRANSFERT_MODE_DMA_IRQ;
    }
    else
    {
        pDev->mask.txModemStatus   = 0;
        pDev->mask.txDataNeeded    = 0;
        pDev->mask.txDmaDone       = 0;

        pDev->uartCfg.tx_mode = HAL_UART_TRANSFERT_MODE_DIRECT_IRQ;
    }

    pDev->uartCfg.data 			= pDcb->dataBits;
    pDev->uartCfg.stop 			= pDcb->stopBits;
    pDev->uartCfg.parity 		= pDcb->parity;

    pDev->uartCfg.tx_trigger 	= HAL_UART_TX_TRIG_EMPTY;

    pDev->uartCfg.afc 			= pDcb->flowControl;
    pDev->uartCfg.rate  		= pDcb->baud;
	
    pDev->uartCfg.irda			= HAL_UART_IRDA_MODE_DISABLE;


    Uart_ISR[0]=gpsd_uart1_irq_handler;
    Uart_ISR[1]=gpsd_uart2_irq_handler;

    hal_UartClose(pDev->port);

	hal_UartOpen(pDev->port, &pDev->uartCfg);
    // Not to flush UART FIFO in breakIntWakeup mode, as UART might receive
    // some valid data after UART break
	hal_UartClearErrorStatus(pDev->port);
	GPSD_TRACE (GPSD_INFO_TRC, 0, ",gpsd_uart_set_dcb_config \n");

	if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
	{
		hal_UartSetRts(pDev->port, TRUE);
	}

	hal_UartIrqSetHandler(pDev->port,Uart_ISR[pDev->port-1],0);

    // Unmask IRQs at last
    UINT32 scStatus = hal_SysEnterCriticalSection();
	hal_UartIrqSetMask(pDev->port, pDev->mask);
    // hal_UartGetData will unmask IRQ too.
    // It will request an IFC channel. This cannot be done twice,
    // so it is put in critial section to avoid conflicts with
    // DealWith_UartRxIrq.
    if(pDev->RxUseDMA)
    {
        hal_UartGetData (pDev->port, pDev->pUart_hw_buf->rx_buf, STATIC_RX_BUF_LEN);	
    }
    hal_SysExitCriticalSection(scStatus);

	return;
}

VOID gpsd_uart_register_rx_cb(HAL_UART_ID_T id,VOID(*callfunc)(HAL_UART_ID_T id))
{
    DEVICE *pDev=&Uart_Dev[id-1];
    if(pDev->port!=id)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error2! wrong uart %d",id);
        return ;
    }
    pDev->registered_func=callfunc;
    return;
}

VOID gpsd_uart_reset(HAL_UART_ID_T id)
{
	DEVICE *pDev=&Uart_Dev[id-1];
	VOID (*Uart_ISR[2])(HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T error);

    hal_UartClose(pDev->port);

	memset(pDev->pWriteBuf, 0 , sizeof(struct CircularBuf));
	memset(pDev->pReadBuf , 0 , sizeof(struct CircularBuf));
	memset(pDev->pUart_hw_buf, 0 , sizeof(struct  Uart_hw_buf));

	hal_UartOpen(pDev->port, &pDev->uartCfg);
    // Not to flush UART FIFO in breakIntWakeup mode, as UART might receive
    // some valid data after UART break
	hal_UartClearErrorStatus(pDev->port);
	GPSD_TRACE (GPSD_INFO_TRC, 0, ",gpsd_uart_set_dcb_config \n");

	if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
	{
		hal_UartSetRts(pDev->port, TRUE);
	}
	
	hal_UartIrqSetHandler(pDev->port,Uart_ISR[pDev->port-1],0);

	// Unmask IRQs at last
	UINT32 scStatus = hal_SysEnterCriticalSection();
	hal_UartIrqSetMask(pDev->port, pDev->mask);
	// hal_UartGetData will unmask IRQ too.
	// It will request an IFC channel. This cannot be done twice,
	// so it is put in critial section to avoid conflicts with
	// DealWith_UartRxIrq.
	if(pDev->RxUseDMA)
	{
		hal_UartGetData (pDev->port, pDev->pUart_hw_buf->rx_buf, STATIC_RX_BUF_LEN);	
	}
	hal_SysExitCriticalSection(scStatus);
}

BOOL gpsd_uart_close(HAL_UART_ID_T id)
{
	DEVICE *pDev=&Uart_Dev[id-1];
	if(pDev->port!=id)
	{
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error3! wrong uart %d",id);
        return FALSE;
	}

    // Disable UART IRQ
    HAL_UART_IRQ_STATUS_T mask = { 0, };
    hal_UartIrqSetMask(pDev->port, mask);
    // Clean UART FIFO
    hal_UartFifoFlush(pDev->port);
    // Close UART
    hal_UartClose(pDev->port);

    pDev->registered_func=NULL;
    pDev->port=0;

    if(pDev->pWriteBuf!=NULL)
    {
        GPSD_FREE(pDev->pWriteBuf);
        pDev->pWriteBuf=NULL;
    }
    if(pDev->pReadBuf!=NULL)
    {
        GPSD_FREE(pDev->pReadBuf);
        pDev->pReadBuf=NULL;
    }
    if(pDev->pUart_hw_buf!=NULL)
    {
        GPSD_FREE(pDev->pUart_hw_buf);
        pDev->pUart_hw_buf=NULL;
    }

    return TRUE;
}


PRIVATE VOID DealWith_UartRxIrq(DEVICE *pDev,BOOL  DMA_used, BOOL  Timeout)
{
    UINT32 uDataSize, real_len=0;
    UINT8 *rxDmaBufEmpty, *data;
    rxDmaBufEmpty =  pDev->pUart_hw_buf->rx_buf ;
	UINT32 rxdmalen=0;
    if( rxDmaBufEmpty == NULL ) return;

    if(DMA_used)
    {   	
        uDataSize = STATIC_RX_BUF_LEN;
        if(Timeout)
        {
            hal_UartRxDmaPurgeFifo(pDev->port);
            while(!hal_UartRxDmaFifoEmpty(pDev->port));
		 rxdmalen = hal_UartRxDmaLevel(pDev->port);
		 
		//GPSD_TRACE (GPSD_INFO_TRC, 0, ",DealWith_UartRxIrq  rxdmalen = %d\n",rxdmalen);
		if(rxdmalen <STATIC_RX_BUF_LEN)
	            uDataSize = STATIC_RX_BUF_LEN- rxdmalen;	
		else
			uDataSize = 0;
		
		//GPSD_TRACE (GPSD_INFO_TRC, 0, ",DealWith_UartRxIrq  uDataSize = %d\n",uDataSize);
		while(!hal_UartRxDmaStop(pDev->port));
        }
		
	//dbg_TraceOutputText(0,"DealWith_UartRxIrq  Timeout?%d ,uDataSize = %d,rxdmalen = 0x%x ",Timeout,uDataSize,rxdmalen);
	
        data = rxDmaBufEmpty;
        real_len = put_data_to_CircularBuffer(
                            pDev->pReadBuf,
                            (VOID*)HAL_SYS_GET_UNCACHED_ADDR((UINT32)data),
                            uDataSize);    
       // hal_UartGetData(pDev->port,rxDmaBufEmpty,STATIC_RX_BUF_LEN); 
	if (real_len <= STATIC_RX_BUF_LEN*2)
	{
		if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
		{
			hal_UartSetRts(pDev->port, FALSE);
		}	
		pDev->RxPending = 0;
		GPSD_TRACE (GPSD_INFO_TRC, 0, ",hal_UartSetRts FLASE real_len = %d\n",real_len);
	}
	else
	{
		if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
		{
			hal_UartSetRts(pDev->port, TRUE);
		}
		hal_UartGetData (pDev->port,rxDmaBufEmpty, STATIC_RX_BUF_LEN);
		pDev->RxPending = 1;
	}  
    }
    else
    {
        uDataSize = 1;
        while(uDataSize>0)
        {
            uDataSize = hal_UartGetData (pDev->port,rxDmaBufEmpty,STATIC_RX_BUF_LEN);
            data = rxDmaBufEmpty;
            if(uDataSize>0)
            {
                real_len = put_data_to_CircularBuffer(pDev->pReadBuf,data,uDataSize);
                if (real_len <= 32)
                {
                	if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
					{
                    	hal_UartSetRts(pDev->port, FALSE);
                	}	
                    GPSD_TRACE (GPSD_INFO_TRC, 0, ",hal_UartSetRts XXX real_len = %d\n",real_len);
                }
            }
            //GPSD_TRACE (GPSD_INFO_TRC, 0, "Rx laixf %x: %x,%x,%x,%x,\n",uDataSize,rxDmaBufEmpty[0],rxDmaBufEmpty[1],rxDmaBufEmpty[2],rxDmaBufEmpty[3]);
            uDataSize  = hal_UartRxFifoLevel(pDev->port);
        }
    }

    if(pDev->registered_func!=NULL)
    {
        pDev->registered_func(pDev->port);
    }	
}

PRIVATE VOID DealWith_UartTxIrq(DEVICE *pDev,UINT8 DMA_used)
{
	UINT16  real_len=0;
	if(DMA_used)
	{
	}
	else
	{
		real_len=get_data_from_CircularBuffer(pDev->pWriteBuf, pDev->pUart_hw_buf->tx_buf,UART_FIFO_LEN);

		if(real_len>0)
		{	
			hal_UartSendData(pDev->port, pDev->pUart_hw_buf->tx_buf, real_len);	
		
	 		GPSD_TRACE (GPSD_INFO_TRC, 0, "tx laixf %x,%x,Normal ",real_len,DMA_used );
		}
	 }
}

PRIVATE VOID gpsd_uart1_irq_handler (HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T reg) 
{
    DEVICE *pDev;
    pDev=&Uart_Dev[HAL_UART_1-1];
    if(pDev->port!=HAL_UART_1)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error4! wrong uart %d",HAL_UART_1);
        return ;
    }
    if(status.rxDmaDone)
    { 	       
        DealWith_UartRxIrq(pDev,TRUE,FALSE);  
    }
    else if(status.rxDmaTimeout)
    {  	
        //    GPSD_TRACE (GPSD_INFO_TRC, 0, "error4! status.rxDmaTimeout");
        if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
		{
			hal_UartSetRts(pDev->port, FALSE); 	 
        }
		DealWith_UartRxIrq(pDev,TRUE,TRUE);  
    }
    else if(status.rxTimeout)
    {
        DealWith_UartRxIrq(pDev,FALSE,TRUE); 
    }
    else if(status.rxDataAvailable)
    {
        DealWith_UartRxIrq(pDev,FALSE,FALSE);  
    }

    // TX
    if(status.txDmaDone)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txDmaDone%d",HAL_UART_1);
    // 	DealWith_UartTxIrq(pDev,TRUE);
    }
    else if(status.txDataNeeded)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txDataNeeded%d",HAL_UART_1);
        DealWith_UartTxIrq(pDev,TRUE);
    }
    else if(status.txModemStatus)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txModemStatus%d",HAL_UART_1);
    }

//add task process

}

PRIVATE VOID gpsd_uart2_irq_handler (HAL_UART_IRQ_STATUS_T status, HAL_UART_ERROR_STATUS_T reg) 
{
    DEVICE *pDev;
    pDev=&Uart_Dev[HAL_UART_2-1];
    if(pDev->port!=HAL_UART_2)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "error4! wrong uart %d",HAL_UART_2);
        return ;
    }
    if(status.rxDmaDone)
    { 	       
        DealWith_UartRxIrq(pDev,TRUE,FALSE);  
    }
    else if(status.rxDmaTimeout)
    {  	
        //    GPSD_TRACE (GPSD_INFO_TRC, 0, "error4! status.rxDmaTimeout");
        if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
		{
			hal_UartSetRts(pDev->port, FALSE); 	 
        }
		DealWith_UartRxIrq(pDev,TRUE,TRUE);  
    }
    else if(status.rxTimeout)
    {
        DealWith_UartRxIrq(pDev,FALSE,TRUE); 
    }
    else if(status.rxDataAvailable)
    {
        DealWith_UartRxIrq(pDev,FALSE,FALSE);  
    }

    // TX
    if(status.txDmaDone)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txDmaDone%d",HAL_UART_2);
    // 	DealWith_UartTxIrq(pDev,TRUE);
    }
    else if(status.txDataNeeded)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txDataNeeded%d",HAL_UART_2);
        DealWith_UartTxIrq(pDev,TRUE);
    }
    else if(status.txModemStatus)
    {
        GPSD_TRACE (GPSD_INFO_TRC, 0, "status.txModemStatus%d",HAL_UART_2);
    }

//add task process

}


PRIVATE BOOL g_UartTxFinished	=  0;
PRIVATE BOOL g_UartTxDmaDone	=  0;
UINT8 debug111[] = {0xE0 ,0xE2 ,0x04 ,0x00 ,0x01 ,0x64 ,0x01 ,0x01 ,0x65 ,0x01 ,0xF0 ,0xF3};
UINT16  GPS_UART_PutBytes(HAL_UART_ID_T id, UINT8 *pBuff, UINT16 NumToWrite)
{
 	UINT16 real_len,send_rel_len,Tx_fifo_avail;
	DEVICE * pDev=&Uart_Dev[id-1];
	if(pDev->port!=id)
	{
			GPSD_TRACE (GPSD_INFO_TRC, 0, "error6! wrong uart %d",id);
			return 0;
	}
	 if((*(pBuff+0)==debug111[0])&&(*(pBuff+1)==debug111[1])&&(*(pBuff+2)==debug111[2])&&(*(pBuff+5)==debug111[5]))
	 {
	 	//hal_DbgAssert(0);
	 }
	 if(pDev->pWriteBuf==NULL)
	 	return 0;

//qiff add for check uart DMA status
	while ( (!hal_UartTxFinished(pDev->port) )||(!hal_UartTxDmaDone( pDev->port ) )  );


       real_len=put_data_to_CircularBuffer(pDev->pWriteBuf, pBuff,NumToWrite);


	if(pDev->TxUseDMA)
	{
		while(get_CircularBuffer_len(pDev->pWriteBuf) > 0)
		{
			while ( (!(g_UartTxFinished =hal_UartTxFinished(pDev->port)) )||(!(g_UartTxDmaDone =hal_UartTxDmaDone( pDev->port ) ))  );
			send_rel_len=get_data_from_CircularBuffer(pDev->pWriteBuf, pDev->pUart_hw_buf->tx_buf,STATIC_TX_BUF_LEN);
			if(send_rel_len >0 )
			{
				NumToWrite=hal_UartSendData(pDev->port, pDev->pUart_hw_buf->tx_buf, send_rel_len);
			}
		}
	}
	else
	{
		UINT32 suspend_time = 0;
		UINT32  send_timer_out=0;

		send_timer_out = hal_TimGetUpTime();

		while(!Is_CircularBuffer_Empty(pDev->pWriteBuf))
		{
			while(!hal_UartTxFinished(pDev->port))
			{
				suspend_time = hal_TimGetUpTime();

				if (suspend_time - send_timer_out > 2*16384)
				{
					return -1;
				}
			}
			
			Tx_fifo_avail=hal_UartTxFifoAvailable(pDev->port);
			send_rel_len=get_data_from_CircularBuffer(pDev->pWriteBuf, pDev->pUart_hw_buf->tx_buf,Tx_fifo_avail);
			hal_UartSendData(pDev->port, pDev->pUart_hw_buf->tx_buf, send_rel_len);
		}
		send_timer_out=0;		
	}
       return NumToWrite;
 }

VOID GPS_uart_SendData(UINT8* pBuf, UINT16 len)
{
	GPS_UART_PutBytes(GPS_DEFAULT_UART,pBuf,len);
}

UINT16 static getbyteRemain_Len;	

UINT16  GPS_UART_GetBytes(HAL_UART_ID_T id, UINT8 *pBuff,UINT16 NumToRead,UINT8 *pstate)
{
	UINT16 real_len;	
	UINT16 Block_Len;	
	UINT16 Remain_Len;	
	DEVICE * pDev=&Uart_Dev[id-1];
	struct  CircularBuf  *pCirBuff = pDev->pReadBuf;

	if(pDev->port!=id)
	{
		GPSD_TRACE (GPSD_INFO_TRC, 0, "error7! wrong uart %d",id);
		return 0;
	}
	if(pDev->pReadBuf==NULL)
	{
		GPSD_ASSERT(0, "%s  pDev->pReadBuf is NULL!!");
		return 0;
	}
	
   	real_len=get_data_from_CircularBuffer(pDev->pReadBuf, pBuff,NumToRead);

	if(pDev->RxPending == 0)
	{
		Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);
		Remain_Len = pCirBuff->Buf_len - Block_Len;
		GPSD_TRACE (GPSD_INFO_TRC, 0, "UART_GetBytes    real_len = %d  Block_Len=  %d ,Remain_Len =%d",real_len,Block_Len,Remain_Len);
		getbyteRemain_Len = Remain_Len;
		if( Remain_Len > STATIC_RX_BUF_LEN)
		{
			pDev->RxPending = 1;
			if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
			{	
				hal_UartSetRts(pDev->port,TRUE); 
			}
			hal_UartGetData (pDev->port, pDev->pUart_hw_buf->rx_buf, STATIC_RX_BUF_LEN);
		}
	}
	return real_len;
}

UINT16 static mcu_getbyteRemain_Len;	

UINT16  mcu_UART_GetBytes(HAL_UART_ID_T id, UINT8 *pBuff,UINT16 NumToRead,UINT8 *pstate)
{
	UINT16 real_len;	
	UINT16 Block_Len;	
	UINT16 Remain_Len;	
	DEVICE * pDev=&Uart_Dev[id-1];
	struct  CircularBuf  *pCirBuff = pDev->pReadBuf;

	if(pDev->port!=id)
	{
		GPSD_TRACE (GPSD_INFO_TRC, 0, "error7! wrong uart %d",id);
		return 0;
	}
	if(pDev->pReadBuf==NULL)
	{
		GPSD_ASSERT(0, "%s  pDev->pReadBuf is NULL!!");
		return 0;
	}
	
   	real_len=get_data_from_CircularBuffer(pDev->pReadBuf, pBuff,NumToRead);

	if(pDev->RxPending == 0)
	{
		Block_Len = GET_DATA_BLOCK_LEN(pCirBuff->Get, pCirBuff->Put, pCirBuff->Buf_len);
		Remain_Len = pCirBuff->Buf_len - Block_Len;
		GPSD_TRACE (GPSD_INFO_TRC, 0, "UART_GetBytes    real_len = %d  Block_Len=  %d ,Remain_Len =%d",real_len,Block_Len,Remain_Len);
		mcu_getbyteRemain_Len = Remain_Len;
		if( Remain_Len > STATIC_RX_BUF_LEN)
		{
			pDev->RxPending = 1;
			if ( pDev->uartCfg.afc!=HAL_UART_AFC_MODE_DISABLE )
			{	
				hal_UartSetRts(pDev->port,TRUE); 
			}
			hal_UartGetData (pDev->port, pDev->pUart_hw_buf->rx_buf, STATIC_RX_BUF_LEN);
		}
	}
	return real_len;
}

