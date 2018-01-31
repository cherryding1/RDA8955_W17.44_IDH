/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "bl_dma.h"
#include "bl_platform.h"
#include "global_macros.h"
#include "dma.h"

uint32_t bl_dma_calc_fcs(void *address, unsigned size)
{
    if (size > 0x3ffff)
        bl_panic();
    if (!(hwp_dma->get_channel & DMA_GET_CHANNEL))
        bl_panic();

    hwp_dma->src_addr = DMA_SRC_ADDRESS((uint32_t)address);
    hwp_dma->dst_addr = 0;
    hwp_dma->xfer_size = DMA_TRANSFER_SIZE(size);
    hwp_dma->control = DMA_ENABLE | DMA_INT_DONE_CLEAR | DMA_FCS_ENABLE;

    while (!(hwp_dma->status & DMA_INT_DONE_STATUS))
        asm("nop;nop");

    uint32_t data = hwp_dma->fcs;
    uint8_t fcs0 = data & 0xff;
    uint8_t fcs1 = (data >> 8) & 0xff;
    uint8_t fcs2 = (data >> 16) & 0xff;
    return (fcs0 << 16) | (fcs1 << 8) | fcs2;
}

uint32_t bl_read_fcs(uint8_t *address)
{
    return (address[0] << 16) | (address[1] << 8) | address[2];
}
