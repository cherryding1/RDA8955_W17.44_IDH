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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdlib.h>

#include "portab.h"
#include "mpeg4_global.h"
//#include "colorspace.h"
#include "xvid.h"

#define EDGE_SIZE  16
#define EDGE_SIZE_LOWRES_1 8
//#define SAFETY    64
#define EDGE_SIZE2  (EDGE_SIZE>>1)
#define EDGE_SIZE2_LOWRES_1  (EDGE_SIZE_LOWRES_1>>1)

void init_image(uint32_t cpu_flags);


static void __inline
image_null(IMAGE * image)
{
    image->y = image->u = image->v = NULL;
}

int32_t image_create(IMAGE * image,
                     int16_t edged_width,
                     int16_t edged_height);

void image_destroy(IMAGE * image,
                   int16_t edged_width,
                   int16_t edged_height);

void image_swap(IMAGE * image1,
                IMAGE * image2);

void image_copy(IMAGE * image1,
                IMAGE * image2,
                uint32_t edged_width,
                uint32_t height);

void image_setedges(IMAGE * image,
                    uint32_t edged_width,
                    uint32_t edged_height,
                    uint32_t width,
                    uint32_t height,
                    int bs_version,
                    uint8 low_resolution);

int image_output(IMAGE * image,
                 uint32_t width,
                 uint32_t height,
                 uint32_t edged_width,
                 uint8_t * dst[4],
                 zoomPar *pZoom);

void
image_clear(IMAGE * img, int width, int height, int edged_width,
            int y, int u, int v);

#endif                          /* _IMAGE_H_ */
#endif

