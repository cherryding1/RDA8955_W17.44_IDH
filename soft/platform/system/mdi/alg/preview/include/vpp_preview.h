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








#ifndef vpp_PREVIEW_H
#define vpp_PREVIEW_H

#include "cs_types.h"
#include "hal_voc.h"
#include "hal_error.h"

typedef struct
{
    INT32*    inStreamBufAddr;         //Input stream buffer address
    INT32*    outStreamBufAddr;        //Output stream buffer address
    INT32*    blendBufAddr;
    INT16  src_w;
    INT16  src_h;
    INT16  cut_w;
    INT16  cut_h;
    INT16  dec_w;
    INT16  dec_h;
    INT16  reset;
    INT16  rotate;
} CutZoomInStruct;

HAL_ERR_T vpp_PreviewOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler) ;
HAL_ERR_T vpp_PreviewYUVOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler) ;
void vpp_PreviewClose(void);
HAL_ERR_T vpp_PreviewScheduleOneFrame(CutZoomInStruct *pEncIn);

#endif  // vpp_PREVIEW_H









