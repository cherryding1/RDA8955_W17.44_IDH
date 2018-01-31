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

















#ifndef VPP_ANALOGTV_H
#define VPP_ANALOGTV_H


#include "cs_types.h"
#include "hal_voc.h"

typedef struct
{
    //Input stream buffer address
    INT32 *inStreamBufAddr;

    //Output stream buffer address
    INT32 * outStreamBufAddr;

    //Blend buffer address
    INT32 * blendBufAddr;

    //The width of source picture.
    UINT16  src_w;
    //The height of source picture.
    UINT16  src_h;

    //The valid width.
    UINT16  cut_w;
    //The valid height.
    UINT16  cut_h;

    //The width of output picture.
    UINT16  dec_w;
    //The height of output picture.
    UINT16  dec_h;

    //the flag of reset.(1:reset;0:no reset)
    UINT16  reset;

    //the flag of rotate.(1:rotate;0:no rotate)
    UINT16  rotate;

} VPP_ANALOGTV_INPUT_CFG_T;


#define COMMON_CODE_SIZE    1200



#define PREVIEW_DATA_START   2

#define STREAM_IN_ADDR       PREVIEW_DATA_START
#define STREAM_OUT_ADDR      PREVIEW_DATA_START+120*320*3



//=============================================================================
// vpp_AnalogTVOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP Analog TV code and environment variables.
//=============================================================================
PUBLIC HAL_ERR_T vpp_AnalogTVOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler);

//=============================================================================
// vpp_AnalogTVClose function
//-----------------------------------------------------------------------------
/// Close VPP Analong TV, clear VoC wakeup masks. This function is
/// called each time Analog TV is stopped.
//=============================================================================
PUBLIC VOID vpp_AnalogTVClose(VOID);

//=============================================================================
// vpp_AnalogTVScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule one picture frame.
//=============================================================================
PUBLIC HAL_ERR_T vpp_AnalogTVScheduleOneFrame(VPP_ANALOGTV_INPUT_CFG_T *pEncIn);



#endif  // VPP_ANALOGTV_H

