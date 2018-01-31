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

#ifndef MAP_EXPORT_H
#define MAP_EXPORT_H

/* This file defines the addresses of exported variables.
   It's created by voc_map automatically.*/

#define vpp_AudioJpeg_ENC_IN_STRUCT                 (0 + RAM_Y_BEGIN_ADDR)
#define vpp_AudioJpeg_ENC_OUT_STRUCT                (14 + RAM_Y_BEGIN_ADDR)
#define VPP_MP3_Enc_Code_ExternalAddr_addr          (22 + RAM_Y_BEGIN_ADDR)
#define VPP_AMR_Enc_Code_ExternalAddr_addr          (32 + RAM_Y_BEGIN_ADDR)
#define RGBYUV_ROTATE_Code_ExternalAddr_addr        (38 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_MIC_DIGITAL_GAIN_ADDR                (40 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_xScale                               (7626 + RAM_Y_BEGIN_ADDR)
#define GLOBAL_yScale                               (7628 + RAM_Y_BEGIN_ADDR)
#define GX_RGB_ROTATE_CFG_STRUCT                    (100 + RAM_X_BEGIN_ADDR)

#endif
