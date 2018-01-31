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

















































#include "beginhdr.h"
#include "rvtypes.h"

/*--------------------------------------------------------------------------; */
/* */
/*  The algorithm fills in (1) the bottom (not including corners), */
/*  then (2) the sides (including the bottom corners, but not the */
/*  top corners), then (3) the top (including the top */
/*  corners) as shown below, replicating the outermost bytes */
/*  of the original frame outward: */
/* */
/*               ---------------------------- */
/*              |                            | */
/*              |            (3)             | */
/*              |                            | */
/*              |----------------------------| */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |    original    |     | */
/*              |     |     frame      |     | */
/*              |     |                |     | */
/*              | (2) |                | (2) | */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |----------------|     | */
/*              |     |                |     | */
/*              |     |      (1)       |     | */
/*              |     |                |     | */
/*               ---------------------------- */
/* */
/*--------------------------------------------------------------------------; */
void RV_CDECL ExpandPlane
(
    U8 *StartPtr,
    U32 uFrameWidth,
    U32 uFrameHeight,
    U32 uPitch,
    U32 uPels
)
{
    U32 row, col;
    U8 uLeftFillVal;
    U8 uRightFillVal;
    PU32 pSrc;
    PU32 pDst;
    PU8 pByteSrc;


    /* section 1 at bottom */
    /* obtain pointer to start of bottom row of original frame */
    pSrc = (PU32)StartPtr + ((uFrameHeight*uPitch - uPitch)>>2);
    pDst = pSrc + (uPitch>>2);
    for (row=0; row<uPels; row++, pDst += (uPitch>>2))
        for (col=0; col<uFrameWidth>>2; col++)
            pDst[col] = pSrc[col];


    /* section 2 on left and right */
    /* obtain pointer to start of first row of original frame */
    pByteSrc = StartPtr;
    for (row=0; row<(uFrameHeight + uPels); row++, pByteSrc += uPitch)
    {
        /* get fill values from left and right columns of original frame */
        uLeftFillVal = *pByteSrc;
        uRightFillVal = *(pByteSrc + uFrameWidth - 1);

        /* fill all bytes on both edges */
        for (col=0; col<uPels; col++)
        {
            *(pByteSrc - uPels + col) = uLeftFillVal;
            *(pByteSrc + uFrameWidth + col) = uRightFillVal;
        }
    }

    /* section 3 at top */
    /* obtain pointer to top row of original frame, less expand pels */
    pSrc = (PU32)StartPtr - (uPels>>2);
    pDst = pSrc - (uPitch>>2);
    for (row=0; row<uPels; row++, pDst -= (uPitch>>2))
        for (col=0; col<(uFrameWidth+uPels+uPels)>>2; col++)
            pDst[col] = pSrc[col];

}   /* end ExpandPlane */

#ifdef INTERLACED_CODE
void RV_CDECL ExpandPlanei
(
    U8 *StartPtr,
    U32 uFrameWidth,
    U32 uFrameHeight,
    U32 uPitch,
    U32 uPels
)
{
    U32 row, col;
    U8 uLeftFillVal;
    U8 uRightFillVal;
    PU32 pSrc, pSrc2;
    PU32 pDst, pDst2;
    PU8 pByteSrc;


    // section 1 at bottom
    // obtain pointer to start of bottom row of original frame
    pSrc = (PU32)StartPtr + ((uFrameHeight*uPitch - uPitch)>>2);
    pSrc2 = pSrc - (uPitch>>2);
    pDst = pSrc + 2*(uPitch>>2);
    pDst2 = pSrc + (uPitch>>2);
    for (row=0; row<uPels; row +=2, pDst += 2*(uPitch>>2), pDst2 += 2*(uPitch>>2))
        for (col=0; col<uFrameWidth>>2; col++)
        {
            pDst[col] = pSrc[col];
            pDst2[col] = pSrc2[col];
        }


    // section 2 on left and right
    // obtain pointer to start of first row of original frame
    pByteSrc = StartPtr;
    for (row=0; row<(uFrameHeight + uPels); row++, pByteSrc += uPitch)
    {
        // get fill values from left and right columns of original frame
        uLeftFillVal = *pByteSrc;
        uRightFillVal = *(pByteSrc + uFrameWidth - 1);

        // fill all bytes on both edges
        for (col=0; col<uPels; col++)
        {
            *(pByteSrc - uPels + col) = uLeftFillVal;
            *(pByteSrc + uFrameWidth + col) = uRightFillVal;
        }
    }

    // section 3 at top
    // obtain pointer to top row of original frame, less expand pels
    pSrc = (PU32)StartPtr - (uPels>>2);
    pSrc2 = pSrc + (uPitch>>2);
    pDst = pSrc - 2*(uPitch>>2);
    pDst2 = pSrc - (uPitch>>2);

    for (row=0; row<uPels; row += 2, pDst -= 2*(uPitch>>2), pDst2 -= 2*(uPitch>>2))
        for (col=0; col<(uFrameWidth+uPels+uPels)>>2; col++)
        {
            pDst[col] = pSrc[col];
            pDst2[col] = pSrc2[col];
        }


//  fwrite(pDst, 1, uPitch *  (uFrameHeight+uPels*2), g_predfile);
//  U8 bb[1];
//  bb[0] = 128;
//  for (I32 i = 0; i < uPitch *  (uFrameHeight+uPels*2) / 2; i++)
//      fwrite(bb,1,  1,  g_predfile);
}   // end ExpandPlane
#endif
