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

#define VPP_LZMADEC_CFG_STRUCT                      (9216 + RAM_X_BEGIN_ADDR)
#define VPP_LZMADEC_STATUS_STRUCT                   (9222 + RAM_X_BEGIN_ADDR)
#define lzma_header                                 (9226 + RAM_X_BEGIN_ADDR)

#endif
