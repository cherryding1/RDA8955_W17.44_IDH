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

typedef    long memsetword;        /* "memsetword" used for optimal copy speed */

/*
 * This code is derived from software contributed to Berkeley by
 * Mike Hibler and Chris Torek.
 */
void *
memset(dst0, c0, length)
void *dst0;
int c0;
size_t length;
{
    size_t t;
    unsigned long c;
    unsigned char *dst;

    dst = dst0;
    /*
     * If not enough words, just fill bytes.  A length >= 2 words
     * guarantees that at least one of them is `complete' after
     * any necessary alignment.  For instance:
     *
     *    |-----------|-----------|-----------|
     *    |00|01|02|03|04|05|06|07|08|09|0A|00|
     *              ^---------------------^
     *         dst         dst+length-1
     *
     * but we use a minimum of 3 here since the overhead of the code
     * to do word writes is substantial.
     */
    if (length < 3 * wsize)
    {
        while (length != 0)
        {
            *dst++ = c0;
            --length;
        }
        return (dst0);
    }

    if ((c = (unsigned char)c0) != 0)      /* Fill the word. */
    {
        c = (c << 8) | c;    /* unsigned long is 16 bits. */
        c = (c << 16) | c;    /* unsigned long is 32 bits. */
    }
    /* Align destination by filling in bytes. */
    if ((t = (unsigned long)dst & wmask) != 0)
    {
        t = wsize - t;
        length -= t;
        do
        {
            *dst++ = c0;
        }
        while (--t != 0);
    }

    /* Fill words.  Length was >= 2*words so we know t >= 1 here. */
    t = length / wsize;
    do
    {
        *(unsigned long *)dst = c;
        dst += wsize;
    }
    while (--t != 0);

    /* Mop up trailing bytes, if any. */
    t = length & wmask;
    if (t != 0)
        do
        {
            *dst++ = c0;
        }
        while (--t != 0);
    return (dst0);
}


