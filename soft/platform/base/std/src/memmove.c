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





#include "string.h"

#define    wsize    sizeof(unsigned long)
#define    wmask    (wsize - 1)

/*-
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 */

typedef    long memmoveword;        /* "memmoveword" used for optimal copy speed */

/*
 * Macros: loop-t-times; and loop-t-times, t>0
 */
#define    TLOOP(s) if (t) TLOOP1(s)
#define    TLOOP1(s) do { s; } while (--t)

/*
 * Copy a block of memory, handling overlap.
 */
void *
memmove(dst0, src0, length)
void *dst0;
const void *src0;
size_t length;
{
    char *dst = dst0;
    const char *src = src0;
    size_t t;

    if (length == 0 || dst == src)        /* nothing to do */
        goto mvdone;

    if ((unsigned long)dst < (unsigned long)src)
    {
        /*
         * Copy forward.
         */
        t = (long)src;    /* only need low bits */
        if ((t | (long)dst) & wmask)
        {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (long)dst) & wmask || length < wsize)
                t = length;
            else
                t = wsize - (t & wmask);
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(memmoveword *)dst = *(memmoveword *)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    }
    else
    {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (long)src;
        if ((t | (long)dst) & wmask)
        {
            if ((t ^ (long)dst) & wmask || length <= wsize)
                t = length;
            else
                t &= wmask;
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(memmoveword *)dst = *(memmoveword *)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }

mvdone:
    return (dst0);
}


