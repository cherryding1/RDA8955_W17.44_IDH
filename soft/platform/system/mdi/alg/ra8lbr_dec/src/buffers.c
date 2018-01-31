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





#include "stdlib.h"
#include "coder.h"

#ifndef SHEEN_VC_DEBUG
extern char * mmc_MemMalloc(int32 nsize);
#endif

/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
static void ClearBuffer(void *buf, int nBytes)
{
    int i;
    unsigned char *cbuf = (unsigned char *)buf;

    for (i = 0; i < nBytes; i++)
        cbuf[i] = 0;

    return;
}

/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the RA8 decoder
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to Gecko2Info structure, set to all 0's
 **************************************************************************************/
Gecko2Info *AllocateBuffers(void)
{
    Gecko2Info *gi;

    /* create new Gecko2Info structure */
#ifdef SHEEN_VC_DEBUG
    gi = (Gecko2Info *)malloc(sizeof(Gecko2Info));
#else
    gi= (Gecko2Info *)mmc_MemMalloc(sizeof(Gecko2Info));
#endif
    if (!gi)
        return 0;
    ClearBuffer(gi, sizeof(Gecko2Info));

    //printf("malloc size=%d\n",sizeof(Gecko2Info));

    return gi;
}

#ifdef SHEEN_VC_DEBUG

#define SAFE_FREE(x)    {if (x) free(x);    (x) = 0;}   /* helper macro */

#else

#define SAFE_FREE(x)

#endif

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: free all the memory used by the RA8 decoder
 *
 * Inputs:      pointer to initialized Gecko2Info structure
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void FreeBuffers(Gecko2Info *gi)
{
    if (!gi)
        return;

    SAFE_FREE(gi);
    //printf("free\n");
}
