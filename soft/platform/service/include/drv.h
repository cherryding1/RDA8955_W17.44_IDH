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

#ifndef _DRV_H_
#define _DRV_H_

#include <stdint.h>
#include <stdbool.h>
#include "cos.h"

#define DRV_TRACE_ID (TID(_EDRV) | TLEVEL(12))
#define DRVLOGV(...) COS_TRACE_V(DRV_TRACE_ID, __VA_ARGS__)
#define DRVLOGD(...) COS_TRACE_D(DRV_TRACE_ID, __VA_ARGS__)
#define DRVLOGI(...) COS_TRACE_I(DRV_TRACE_ID, __VA_ARGS__)
#define DRVLOGW(...) COS_TRACE_W(DRV_TRACE_ID, __VA_ARGS__)
#define DRVLOGE(...) COS_TRACE_E(DRV_TRACE_ID, __VA_ARGS__)
#define DRVLOGSV(tsmap, ...) COS_TRACE_V(DRV_TRACE_ID | (tsmap), __VA_ARGS__)
#define DRVLOGSD(tsmap, ...) COS_TRACE_D(DRV_TRACE_ID | (tsmap), __VA_ARGS__)
#define DRVLOGSI(tsmap, ...) COS_TRACE_I(DRV_TRACE_ID | (tsmap), __VA_ARGS__)
#define DRVLOGSW(tsmap, ...) COS_TRACE_W(DRV_TRACE_ID | (tsmap), __VA_ARGS__)
#define DRVLOGSE(tsmap, ...) COS_TRACE_E(DRV_TRACE_ID | (tsmap), __VA_ARGS__)
#define DRVDUMP(data, size) COS_DUMP(DRV_TRACE_ID, data, size);
#define DRVDUMP16(data, size) COS_DUMP16(DRV_TRACE_ID, data, size);
#define DRVDUMP32(data, size) COS_DUMP32(DRV_TRACE_ID, data, size);

// =============================================================================
// DRV_CIRCLE_BUF_T
// -----------------------------------------------------------------------------
/// A generic circle buffer. It is not task/interrupt safe. Caller should
/// consider it (if needed).
// =============================================================================

struct DRV_CIRCLE_BUF;
typedef struct DRV_CIRCLE_BUF DRV_CIRCLE_BUF_T;

void DRV_CBufInit(DRV_CIRCLE_BUF_T *cb, uint8_t *buf, unsigned size);
void DRV_CBufFlush(DRV_CIRCLE_BUF_T *cb);
bool DRV_CBufIsEmpty(DRV_CIRCLE_BUF_T *cb);
bool DRV_CBufIsFull(DRV_CIRCLE_BUF_T *cb);
unsigned DRV_CBufPayloadSize(DRV_CIRCLE_BUF_T *cb);
unsigned DRV_CBufRemainSize(DRV_CIRCLE_BUF_T *cb);
unsigned DRV_CBufWrite(DRV_CIRCLE_BUF_T *cb, const uint8_t *data, unsigned size);
unsigned DRV_CBufRead(DRV_CIRCLE_BUF_T *cb, uint8_t *data, unsigned size);
uint8_t *DRV_CBufLinearRead(DRV_CIRCLE_BUF_T *cb, unsigned *size);
void DRV_CBufSkipBytes(DRV_CIRCLE_BUF_T *cb, unsigned size);

// DONT access directly
struct DRV_CIRCLE_BUF
{
    uint8_t *buf;
    unsigned size;
    unsigned rd;
    unsigned payload;
};

typedef struct _DRV_SCREENINFO
{
    UINT16 width;
    UINT16 height;
    UINT8 bitdepth;
    UINT8 isMemoryAddrValid;
    UINT16 nResvered;
    VOID *pMemoryAddr;
} DRV_SCREENINFO;

typedef struct _LCD_INFO
{
    short em_x;
    short em_y;
    short ScreenWidth;
    short ScreenHeight;
    short ScreenBitDepth;
    UINT32 *pMemoryAdd;
} LCD_INFO;

//
//create 24 bit 8/8/8 format pixel (0xRRGGBB) from RGB triplet
//
#define DDI_RGB2PIXEL888(r, g, b) \
    (((r) << 16) | (((BYTE)(g)) << 8) | ((BYTE)b))

//
//create 16 bit 5/6/5 format pixel (0xRGB) from RGB triplet
//
#define DRV_RGB2PIXEL565(r, g, b) \
    ((((r)&0xf8) << 8) | (((g)&0xfc) << 3) | (((b)&0xf8) >> 3))

#define DRV_LCD_IOCTL_CONTRAST 0x01
#define DRV_LCD_IOCTL_BRIGHTNESS 0x02
#define DRV_LCD_IOCTL_STANDBY 0x03
#define DRV_LCD_IOCTL_UNSTANDBY 0x04
#define DRV_LCD_IOCTL_SLEEP 0x05
#define DRV_LCD_IOCTL_UNSLEEP 0x06
#define DRV_LCD_IOCTL_DEVINFO 0x07
#define DRV_LCD_IOCTL_BACKLIGHT 0x08

UINT32 SRVAPI DRV_LcdInit(
    VOID);

UINT32 SRVAPI DRV_LcdExit(
    VOID);

UINT32 SRVAPI DRV_LcdIoControl(
    UINT32 uCode,
    UINT32 nParamIn,
    UINT32 *pParamOut);

UINT32 SRVAPI DRV_LcdSetPixel16(
    UINT16 nXDest,
    UINT16 nYDest,
    UINT16 pPixelData);

UINT32 SRVAPI DRV_LcdBlt16(
    UINT16 nXDest,
    UINT16 nYDest,
    UINT16 nSrcWidth,
    UINT16 nSrcHeight,
    UINT16 *pPixelData);

UINT32 SRVAPI DRV_LcdBmpBlt16(
    INT16 nXDest,  // LCD position x
    INT16 nYDest,  // LCD position y
    INT16 nXSrc,   // bmp   x
    INT16 nYSrc,   // bmp   y
    INT16 nWidth,  // bmp   w
    INT16 nHeight, // bmp  h
    UINT16 *pBmpData);

UINT32 SRVAPI DRV_SubLcdInit(
    VOID);

UINT32 SRVAPI DRV_SubLcdExit(
    VOID);

UINT32 SRVAPI DRV_SubLcdIoControl(
    UINT32 uCode,
    UINT32 nParamIn,
    UINT32 *pParamOut);

UINT32 SRVAPI DRV_SubLcdSetPixel12(
    UINT16 nXDest,
    UINT16 nYDest,
    UINT16 nPixelData);

UINT32 SRVAPI DRV_SubLcdBlt(
    UINT8 nXDest,
    UINT8 nYDest,
    UINT8 nSrcWidth,
    UINT8 nSrcHeight,
    UINT16 *pRgbData);

//
// GPIO
//

#define DRV_GPIO_0 0
#define DRV_GPIO_1 1
#define DRV_GPIO_2 2
#define DRV_GPIO_6 6
#define DRV_GPIO_7 7
#define DRV_GPIO_8 8
#define DRV_GPIO_9 9
#define DRV_GPIO_10 10
#define DRV_GPIO_11 11
#define DRV_GPIO_12 12
#define DRV_GPIO_13 13
#define DRV_GPIO_14 14
#define DRV_GPIO_15 15
#define DRV_GPIO_16 16
#define DRV_GPIO_17 17
#define DRV_GPIO_18 18
#define DRV_GPIO_19 19

#define DRV_GPIO_20 20
#define DRV_GPIO_21 21
#define DRV_GPIO_22 22
#define DRV_GPIO_23 23
#define DRV_GPIO_24 24
#define DRV_GPIO_25 25
#define DRV_GPIO_26 26
#define DRV_GPIO_27 27
#define DRV_GPIO_28 28
#define DRV_GPIO_29 29
#define DRV_GPIO_30 30
#define DRV_GPIO_31 31

#define DRV_GPIO_EDGE_R 0
#define DRV_GPIO_EDGE_F 1
#define DRV_GPIO_EDGE_RF 2

#define DRV_GPIO_IN 0
#define DRV_GPIO_OUT 1

#define DRV_GPIO_LOW 0
#define DRV_GPIO_HIGH 1

UINT32 SRVAPI DRV_GpioSetDirection(
    UINT8 nGpio,
    UINT8 nDirection);

UINT32 DRV_GpioSetMode(
    UINT8 nGpio,
    UINT8 nMode);

UINT32 SRVAPI DRV_GpioGetDirection(
    UINT8 nGpio,
    UINT8 *pDirection);

UINT32 SRVAPI DRV_GpioSetLevel(
    UINT8 nGpio,
    UINT8 nLevel);

UINT32 SRVAPI DRV_GpioGetLevel(
    UINT8 nGgpio,
    UINT8 *pLevel);

UINT32 SRVAPI DRV_GpioSetEdgeIndication(
    UINT8 nGpio,
    UINT8 nEdge,
    UINT32 nDebonce);

UINT32 SRVAPI DRV_EXTISetEdgeIndication(
    UINT8 nEdge);

#define DRV_MEMORY_NO_PAGE 0
#define DRV_MEMORY_4HW_PAGE 1
#define DRV_MEMORY_8HW_PAGE 2
#define DRV_MEMORY_16HW_PAGE 3

#define DRV_MEMORY_0_CYCLE 0
#define DRV_MEMORY_1_CYCLE 1
#define DRV_MEMORY_2_CYCLE 2
#define DRV_MEMORY_4_CYCLE 3

typedef enum {
    DRV_EXTI_EDGE_NUL,
    DRV_EXTI_EDGE_R,
    DRV_EXTI_EDGE_F,
    DRV_EXTI_EDGE_RF,
} DRV_EXTI_EDGE;

typedef enum {
    DRV_MEMORY_FLASH = 0,
    DRV_MEMORY_SRAM,
    DRV_MEMORY_CS2,
    DRV_MEMORY_CS3, /* this one can be used for main FLASH too */
    DRV_MEMORY_CS4, /* this one can be used for main SRAM too */
    DRV_MEMORY_CS5,
    DRV_MEMORY_CS6,
    DRV_MEMORY_CS_QTY
} DRV_EBC_CS;

typedef struct _DRV_MEM_CFG
{
    BOOL writeAllow;
    BOOL writeByteEnable;
    BOOL waitMode;
    UINT8 pageSize;
    UINT8 tACS;
    UINT8 tCAH;
    UINT8 tCOS;
    UINT8 tOCH;
    INT32 tACC;
    INT32 tPMACC;
} DRV_MEM_CFG;

PVOID SRVAPI DRV_MemorySetupCS(
    UINT8 nCS,
    DRV_MEM_CFG *pMemCfg);

UINT32 SRVAPI DRV_MemoryDisableCS(
    UINT8 nCS);

//
// PWML
//
UINT32 SRVAPI DRV_SetPwl1(
    UINT8 nLevel);

UINT32 DRV_SetKeyBacklight(UINT8 nLevel);

UINT32 SRVAPI DRV_SetPwl0(
    UINT8 nLevel);

//UINT32 SRVAPI Test_SetPwl0(  UINT8 nLevel);  //for Dingjian test

// =============================================================================
// DRV_UART_T
// -----------------------------------------------------------------------------
/// A (not so) generic UART driver. It will always use DMA for both RX and TX.
///
/// * RX DMA buffer should be uncachable.
/// * TX circle buffer should be uncachable
/// * RX DMA buffer size: 256 for 115200 (22ms)
/// * RX polling period: 15ms for 115200
///
/// State Control:
/// * "close" will close UART completely.
/// * "sleep" will enable system sleep. However, UART can still recieve RX
///   interrupts, and UART will be woken automatically in ISR. Also, UART can
///   be woken manually. At sleep, it is very possible to lose data.
/// * "close/sleep" will wait TX finish (peer can see all TX data). During wait,
///   UART is in CLOSE_REQ/SLEEP_REQ state. And driver will close/sleep it
///   automatically. It is not needed to be handled by application.
/// * In most cases, close and sleep won't be used mixedly. If sleep/wakeup is
///   needed, close will never be called.
///
/// Baud Rate:
/// * Driver will provide divider configuration for "normal" baud rate. In case
///   non-common baud rate is needed, or tune baud rate, divider can be set from
///   configuration. (most likely, it is not needed).
/// * Chips before 8955 can'e support 1200.
// =============================================================================

struct DRV_UART;
typedef struct DRV_UART DRV_UART_T;

typedef enum {
    DRV_UART_RX_ARRIVED = (1 << 0),
    DRV_UART_RX_OVERFLOW = (1 << 1),
    DRV_UART_TX_DONE = (1 << 2),
    DRV_UART_BREAK = (1 << 3),
    DRV_UART_LINE_ERR = (1 << 4),
    DRV_UART_WAKE_UP = (1 << 5)
} DRV_UART_EVENT_T;

typedef enum {
    DRV_UART_ST_CLOSE,
    DRV_UART_ST_OPEN,
    DRV_UART_ST_SLEEP,
    DRV_UART_ST_BAUD_LOCKING,
    DRV_UART_ST_SLEEP_REQ,
    DRV_UART_ST_CLOSE_REQ
} DRV_UART_STATE_T;

typedef void (*DRV_UART_CALLBACK_T)(DRV_UART_T *uart, void *param, DRV_UART_EVENT_T evt);

enum DRV_UART_DATA_BITS
{
    DRV_UART_DATA_BITS_7 = 7,
    DRV_UART_DATA_BITS_8 = 8
};
enum DRV_UART_STOP_BITS
{
    DRV_UART_STOP_BITS_1 = 1,
    DRV_UART_STOP_BITS_2 = 2
};
enum DRV_UART_PARITY
{
    DRV_UART_NO_PARITY,    // No parity check
    DRV_UART_ODD_PARITY,   // Parity check is odd
    DRV_UART_EVEN_PARITY,  // Parity check is even
    DRV_UART_SPACE_PARITY, // Parity check is always 0 (space)
    DRV_UART_MARK_PARITY   // Parity check is always 1 (mark)
};

#define DRV_UART_DEFAULT_AUTO_FLOW_CTRL_LEVEL (4)
#define DRV_UART_DISABLE_AUTO_FLOW_CTRL (0xff)

typedef struct
{
    int baud;
    enum DRV_UART_DATA_BITS dataBits;
    enum DRV_UART_STOP_BITS stopBits;
    enum DRV_UART_PARITY parity;
    bool ctsEnabled;
    bool rtsEnabled;
    uint8_t autoFlowCtrlLevel; // 0xff for disable auto flow control
    bool autoBaudLC;           // true/at, false/AT
    uint32_t forceDivider;

    uint8_t *rxDmaBuf;
    uint8_t *rxBuf;
    uint8_t *txBuf;
    unsigned rxDmaSize;
    unsigned rxBufSize;
    unsigned txBufSize;
    unsigned rxPollPeriod;

    unsigned evtMask;
    DRV_UART_CALLBACK_T callback;
    void *callbackParam;
} DRV_UART_CFG_T;

unsigned DRV_UartStructSize(void);
bool DRV_UartInit(DRV_UART_T *uart, int uartid, DRV_UART_CFG_T *cfg);
DRV_UART_STATE_T DRV_UartState(DRV_UART_T *uart);
bool DRV_UartOpen(DRV_UART_T *uart);
void DRV_UartClose(DRV_UART_T *uart, bool forcedly);
void DRV_UartSleep(DRV_UART_T *uart, bool forcedly, bool wakeupEnabled);
void DRV_UartWakeup(DRV_UART_T *uart);
int DRV_UartWrite(DRV_UART_T *uart, const uint8_t *data, unsigned size, unsigned timeout);
int DRV_UartRead(DRV_UART_T *uart, uint8_t *data, unsigned size);
bool DRV_UartWaitTxFinish(DRV_UART_T *uart, unsigned timeout);
DRV_UART_CFG_T DRV_UartGetCfg(DRV_UART_T *uart);

// =============================================================================
// DRV_UAT_T
// -----------------------------------------------------------------------------
/// Driver for USB CDC device
// =============================================================================

struct DRV_UAT;
typedef struct DRV_UAT DRV_UAT_T;

typedef enum {
    DRV_UAT_RX_ARRIVED = (1 << 0),
    DRV_UAT_RX_OVERFLOW = (1 << 1)
} DRV_UAT_EVENT_T;

typedef void (*DRV_UAT_CALLBACK_T)(DRV_UAT_T *uat, void *param, DRV_UAT_EVENT_T evt);

typedef struct
{
    uint8_t *rxBuf;
    uint8_t *txBuf;
    unsigned rxBufSize;
    unsigned txBufSize;

    unsigned evtMask;
    DRV_UAT_CALLBACK_T callback;
    void *callbackParam;
} DRV_UAT_CFG_T;

void DRV_UatInit(DRV_UAT_T *uar, DRV_UAT_CFG_T *cfg);
bool DRV_UatOpen(DRV_UAT_T *uat);
void DRV_UatClose(DRV_UAT_T *uat);
int DRV_UatWrite(DRV_UAT_T *uat, const uint8_t *data, unsigned size);
int DRV_UatRead(DRV_UAT_T *uat, uint8_t *data, unsigned size);

// DONT access directly
struct DRV_UAT
{
    DRV_UAT_CFG_T cfg;
    DRV_CIRCLE_BUF_T rxcb;
    DRV_CIRCLE_BUF_T txcb;
    uint8_t *usbRecvBuf;
    uint8_t *usbSendBuf;
};

/*************************************************
 *** SPI
 *************************************************/

typedef enum {
    DRV_SPI_CS0 = 0, ///< Chip Select 0
    DRV_SPI_CS1,     ///< Chip Select 1
    DRV_SPI_CS_QTY
} DRV_SpiCs_t;

typedef enum {
    DRV_SPI_DIRECT_POLLING,
    DRV_SPI_DIRECT_IRQ,
    DRV_SPI_DMA_POLLING,
    DRV_SPI_DMA_IRQ,
    DRV_SPI_OFF,
    DRV_SPI_TM_QTY
} DRV_SpiTransfertMode;
typedef enum {
    DRV_SPI_HALF_CLK_PERIOD_0,
    DRV_SPI_HALF_CLK_PERIOD_1,
    DRV_SPI_HALF_CLK_PERIOD_2,
    DRV_SPI_HALF_CLK_PERIOD_3
} DRV_Spi_Delay;
typedef enum {
    DRV_SPI_RX_TRIGGER_1_BYTE,
    DRV_SPI_RX_TRIGGER_4_BYTE,
    DRV_SPI_RX_TRIGGER_8_BYTE,
    DRV_SPI_RX_TRIGGER_12_BYTE,
    DRV_SPI_RX_TRIGGER_QTY
} DRV_SpiRxTriggerCfg;
typedef enum {
    DRV_SPI_TX_TRIGGER_1_EMPTY,
    DRV_SPI_TX_TRIGGER_4_EMPTY,
    DRV_SPI_TX_TRIGGER_8_EMPTY,
    DRV_SPI_TX_TRIGGER_12_EMPTY,
    DRV_SPI_TX_TRIGGER_QTY
} DRV_SpiTxTriggerCfg;

#define DRV_SPI_MIN_FRAME_SIZE 4
#define DRV_SPI_MAX_FRAME_SIZE 32
#define DRV_SPI_MAX_CLK_DIVIDER 0xFFFFFF

#define DRV_SPI_IRQ_MSK_RX_OVF 0x1
#define DRV_SPI_IRQ_MSK_TX_TH 0x2
#define DRV_SPI_IRQ_MSK_TX_DMA 0x4
#define DRV_SPI_IRQ_MSK_RX_TH 0x8
#define DRV_SPI_IRQ_MSK_RX_DMA 0x10

typedef VOID (*DEVICE_SPI_USERVECTOR)(UINT16);

typedef struct _DRV_SpiCfg
{
    DRV_SpiCs_t enabledCS;
    BOOL polarityCS;
    unsigned char inputEn;
    unsigned char clkFallEdge;
    DRV_Spi_Delay clkDelay;
    DRV_Spi_Delay doDelay;
    DRV_Spi_Delay diDelay;
    DRV_Spi_Delay csDelay;
    DRV_Spi_Delay csPulse;
    int frameSize;
    int clkDivider;
    DRV_SpiRxTriggerCfg rxTrigger;
    DRV_SpiTxTriggerCfg txTrigger;
    DRV_SpiTransfertMode rxMode;
    DRV_SpiTransfertMode txMode;
    UINT16 irqMask;                   ///< Irq Mask for this CS
    DEVICE_SPI_USERVECTOR irqHandler; ///< Irq Handler for this CS

} DRV_SpiCfg; /*this struct to user*/

UINT32 DRV_SPIInit(DRV_SpiCfg *spicfg);

VOID DRV_SPIExit(DRV_SpiCs_t cs);

void DRV_WaitSPIRxDmaDone(
    unsigned char *datain,
    unsigned int inlen);

UINT32 DRV_SPIWrite(DRV_SpiCs_t cs, UINT8 *data, UINT16 len);

UINT32 DRV_SPIRead(DRV_SpiCs_t cs, UINT8 *outbuf, UINT16 len);

void DRV_SPISetISRmask(
    UINT16 mask);

UINT8 DRV_Spi_TxFinished(DRV_SpiCs_t cs);
UINT32 DRV_ISRSpiRead(
    unsigned char *add,
    unsigned int len);
UINT32 DRV_GetSpiIsrFifoLen(
    void);

void DRV_SPI_ChangeCs(
    DRV_SpiCfg *spicfg);

#endif // _H_
