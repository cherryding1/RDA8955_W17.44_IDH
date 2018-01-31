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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif
#ifdef MP4_3GP_SUPPORT

#ifndef _MEM_TRANSFER_H
#define _MEM_TRANSFER_H

/*****************************************************************************
 * transfer16to8 API
 ****************************************************************************/

typedef void (TRANSFER_16TO8COPY) (uint8_t * dst,
                                   int16_t * src,
                                   uint16_t stride);
typedef TRANSFER_16TO8COPY *TRANSFER_16TO8COPY_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_16TO8COPY_PTR transfer_16to8copy;

/* Implemented functions */
extern TRANSFER_16TO8COPY transfer_16to8copy_c;

/*****************************************************************************
 * transfer16to8 + addition op API
 ****************************************************************************/

typedef void (TRANSFER_16TO8ADD) (uint8_t * dst,
                                  int16_t * src,
                                  uint32_t stride);

typedef TRANSFER_16TO8ADD *TRANSFER_16TO8ADD_PTR;

/* Our global function pointer - Initialized in xvid.c */
extern TRANSFER_16TO8ADD_PTR transfer_16to8add;

/* Implemented functions */
extern TRANSFER_16TO8ADD transfer_16to8add_c;

#endif
#endif

