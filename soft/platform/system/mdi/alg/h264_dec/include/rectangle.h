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





#ifndef FFMPEG_RECTANGLE_H
#define FFMPEG_RECTANGLE_H

#include "common.h"

/**
 * fill a rectangle.
 * @param h height of the rectangle, should be a constant
 * @param w width of the rectangle, should be a constant
 * @param size the size of val (1 or 4), should be a constant
 */
static av_always_inline void fill_rectangle(void *vp, int w, int h, int stride, uint32_t val, int size)
{
    uint8_t *p= (uint8_t*)vp;
    AV_ASSERT(size==1 || size==4);
    AV_ASSERT(w<=4);

    w      *= size;
    stride *= size;

    AV_ASSERT((((long)vp)&(FFMIN(w, STRIDE_ALIGN)-1)) == 0);
    AV_ASSERT((stride&(w-1))==0);
    if(w==2)
    {
        const uint16_t v= size==4 ? val : val*0x0101;
        *(uint16_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint16_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint16_t*)(p + 2*stride)= v;
        *(uint16_t*)(p + 3*stride)= v;
    }
    else if(w==4)
    {
        const uint32_t v= size==4 ? val : val*0x01010101;
        *(uint32_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint32_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint32_t*)(p + 2*stride)= v;
        *(uint32_t*)(p + 3*stride)= v;
    }
    else if(w==8)
    {
        //gcc can't optimize 64bit math on x86_32
#ifdef HAVE_FAST_64BIT
        const uint64_t v= val*0x0100000001ULL;
        *(uint64_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint64_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint64_t*)(p + 2*stride)= v;
        *(uint64_t*)(p + 3*stride)= v;
    }
    else if(w==16)
    {
        const uint64_t v= val*0x0100000001ULL;
        *(uint64_t*)(p + 0+0*stride)= v;
        *(uint64_t*)(p + 8+0*stride)= v;
        *(uint64_t*)(p + 0+1*stride)= v;
        *(uint64_t*)(p + 8+1*stride)= v;
        if(h==2) return;
        *(uint64_t*)(p + 0+2*stride)= v;
        *(uint64_t*)(p + 8+2*stride)= v;
        *(uint64_t*)(p + 0+3*stride)= v;
        *(uint64_t*)(p + 8+3*stride)= v;
#else
        *(uint32_t*)(p + 0+0*stride)= val;
        *(uint32_t*)(p + 4+0*stride)= val;
        if(h==1) return;
        *(uint32_t*)(p + 0+1*stride)= val;
        *(uint32_t*)(p + 4+1*stride)= val;
        if(h==2) return;
        *(uint32_t*)(p + 0+2*stride)= val;
        *(uint32_t*)(p + 4+2*stride)= val;
        *(uint32_t*)(p + 0+3*stride)= val;
        *(uint32_t*)(p + 4+3*stride)= val;
    }
    else if(w==16)
    {
        *(uint32_t*)(p + 0+0*stride)= val;
        *(uint32_t*)(p + 4+0*stride)= val;
        *(uint32_t*)(p + 8+0*stride)= val;
        *(uint32_t*)(p +12+0*stride)= val;
        *(uint32_t*)(p + 0+1*stride)= val;
        *(uint32_t*)(p + 4+1*stride)= val;
        *(uint32_t*)(p + 8+1*stride)= val;
        *(uint32_t*)(p +12+1*stride)= val;
        if(h==2) return;
        *(uint32_t*)(p + 0+2*stride)= val;
        *(uint32_t*)(p + 4+2*stride)= val;
        *(uint32_t*)(p + 8+2*stride)= val;
        *(uint32_t*)(p +12+2*stride)= val;
        *(uint32_t*)(p + 0+3*stride)= val;
        *(uint32_t*)(p + 4+3*stride)= val;
        *(uint32_t*)(p + 8+3*stride)= val;
        *(uint32_t*)(p +12+3*stride)= val;
#endif
    }
    else
        AV_ASSERT(0);
    AV_ASSERT(h==4);
}

#endif /* FFMPEG_RECTANGLE_H */
