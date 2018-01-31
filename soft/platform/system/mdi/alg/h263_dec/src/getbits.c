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










#include "h263_config.h"
#include "tmndec.h"
#include "h263_global.h"
//#include "mpeg4_bitstream.h"//for msk[33]
//#ifdef WIN32
//#include <io.h>
//#endif

#ifdef MP4_3GP_SUPPORT

/* to mask the n least significant bits of an integer */
#if 0
static unsigned int msk[33] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
    0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff
};
#endif

/* initialize buffer, call once before first getbits or showbits */

void initbits (unsigned char *pInbuf)
{
    //int i;
    errorstate=0;
    ld=&base;
    ld->rdbfr=pInbuf;
    ld->rdptr = ld->rdbfr ;
    ld->bitcnt = 0;

#ifdef USE_INTERNAL_SRAM
    ld->inbfr=(unsigned char*)INBFR_START;
    ld->block=(short*)BLOCK_START;
    ld->block_temp=(short*)BLOCK_TEMP_START;
#else
    if(sizeof(short)*64*12>mmc_MemGetFree()-32)
    {
        errorstate=-1;
        return;
    }
    //ld->block[0]=NULL;
    //ld->block[0]=(short*)mmc_MemMalloc(sizeof(short)*64*13);//13 for temp block
    ld->block=NULL;
    ld->block=(short*)mmc_MemMalloc(sizeof(short)*64*13);//13 for temp block

    //if(!(ld->block[0]))
    if(!(ld->block))
    {
        //exit(-1);
        errorstate=-1;
        return;
    }

    ld->block_temp=ld->block+12*64;
#endif

}

void resetbits (unsigned char *pInbuf, unsigned int datalen)//add shenh
{
    ld->rdptr = ld->rdbfr=pInbuf ;
    ld->datalen=datalen;
    ld->bitcnt = 0;
}


void flushbits (int n)
{
    ld->bitcnt += n;
    ld->rdptr += (ld->bitcnt)>>3;
    ld->bitcnt = ld->bitcnt & 0x7;
}

/* return next n bits (right adjusted) without advancing */

unsigned int showbits (int n)
{
    unsigned int b;

    b = *(ld->rdptr);
    b = (b<<8) | *(ld->rdptr + 1);
    b = (b<<8) | *(ld->rdptr + 2);
    b = (b<<8) | *(ld->rdptr + 3);
    b = b <<(ld->bitcnt);
    b = b >>(32-n);

    return b;
}


/* return next n bits (right adjusted) */

unsigned int  getbits (int n)
{
    unsigned int b;

    b = *(ld->rdptr);
    b = (b<<8) | *(ld->rdptr + 1);
    b = (b<<8) | *(ld->rdptr + 2);
    b = (b<<8) | *(ld->rdptr + 3);
    b = b <<(ld->bitcnt);
    b = b >>(32-n);

    ld->bitcnt += n;
    ld->rdptr += (ld->bitcnt)>>3;
    ld->bitcnt = ld->bitcnt & 0x7;

    return b;
}

#endif

