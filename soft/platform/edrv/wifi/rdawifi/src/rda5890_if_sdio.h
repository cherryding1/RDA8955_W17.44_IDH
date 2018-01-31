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

#ifndef _RDA5890_IF_SDIO_H
#define _RDA5890_IF_SDIO_H

#define IF_SDIO_SDIO2AHB_PKTLEN_L       0x00
#define IF_SDIO_SDIO2AHB_PKTLEN_H       0x01

#define IF_SDIO_AHB2SDIO_PKTLEN_L       0x02
#define IF_SDIO_AHB2SDIO_PKTLEN_H       0x03

#define IF_SDIO_FUN1_INT_MASK   0x04
#define IF_SDIO_FUN1_INT_PEND   0x05
#define IF_SDIO_FUN1_INT_STAT   0x06

#define   IF_SDIO_INT_AHB2SDIO  0x01
#define   IF_SDIO_INT_ERROR     0x04
#define   IF_SDIO_INT_SLEEP     0x10
#define   IF_SDIO_INT_AWAKE     0x20

#define IF_SDIO_FUN1_FIFO_WR    0x07
#define IF_SDIO_FUN1_FIFO_RD    0x08

#define IF_SDIO_FUN1_INT_TO_DEV 0x09

#endif
