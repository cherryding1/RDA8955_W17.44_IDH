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


#include "cs_types.h"
#include "imgs_m.h"
#include "imgsp_debug.h"

// =============================================================================
// imgs_SimilarizeImage
// -----------------------------------------------------------------------------
/// Similarize a raw picture. It resizes a window in a frame buffer window, described as
/// a region of interest, into the region of interest of another frame buffer window.
/// A part or the whole source picture can then be fitted into a part or the
/// whole destination frame buffer window.
///
/// @param inFbw Input picture. Its region of interest is the source picture.
/// @param outFbw Output picture. Its region of interest will receive the picture.
/// @param rotation Rotate the image while similarizing: number of trigonometrical
/// rotation in Pi/2 rad.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
///
/// @todo What if the buffer are the same ? Enforce them to be different ?.
/// @todo Use a callback to warn the user about the end of the decoding ?
// =============================================================================
PUBLIC IMGS_ERR_T imgs_SimilarizeImage(IMGS_FBW_T* inFbw, IMGS_FBW_T* outFbw,
                                       UINT32 rotation)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_SimilarizeImage);
    // TODO Support transformat as well
    IMGS_ASSERT(inFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565
                && outFbw->fb.colorFormat == LCDD_COLOR_FORMAT_RGB_565,
                "img_SimilarizeImage only handle RGB565 color"
                "coding to date.");

    UINT32 outX = 0;
    UINT32 outY = 0;
    UINT32 inX = 0;
    UINT32 inY = 0;

    UINT16* outPtr = outFbw->fb.buffer;
    UINT16* inPtr = inFbw->fb.buffer;
    UINT16 inPxlData = 0;
    UINT16 outPxlData = 0;
    UINT32 xRatio   = inFbw->roi.width / outFbw->roi.width;
    UINT32 yRatio   = inFbw->roi.height / outFbw->roi.height;



    // Very crude and rough algorithm, improvable with
    // bilinear interpolation etc once I get a grip on
    // fixed point calculous (TODO: get a grip on
    // fixed point calculous)
    // Just an 'homothetie', with a translation from one
    // roi origin to another.
    if (rotation == 0)
    {
        for (outY=outFbw->roi.y; outY <outFbw->roi.y+outFbw->roi.height; outY++)
        {
            for (outX=outFbw->roi.x; outX <outFbw->roi.x+outFbw->roi.width; outX++)
            {
                // Implicit rounding, as we are using integers, is doing the interplation
                // for us. Like a nearest neighbour, but restricting to the toplefter
                // neighbour.
                inX = inFbw->roi.x + (outX - outFbw->roi.x) * xRatio;
                inY = inFbw->roi.y + (outY - outFbw->roi.y) * yRatio;
                inPxlData = inPtr[inX + inY * inFbw->fb.width];

                outPxlData = inPxlData;
                outPtr[outX + outY * outFbw->fb.width] = outPxlData;
            }
            // We are wrapping to the next line:
        }
    }
    else
    {
        // Flip ?
        UINT16 xOrg = inFbw->roi.x + inFbw->roi.width - 1;
        UINT16 yOrg = inFbw->roi.y + inFbw->roi.height - 1;
        UINT16 outXOffset = 0;
        UINT16 outYOffset = 0;
#define IN_X_SHIFT(offOutX, offOutY) (-1 * (offOutY * inFbw->roi.width) / outFbw->roi.height)
#define IN_Y_SHIFT(offOutX, offOutY) (-1 * (offOutX * inFbw->roi.height) / outFbw->roi.width)

        for (outY=outFbw->roi.y; outY <outFbw->roi.y+outFbw->roi.height; outY++)
        {
            for (outX=outFbw->roi.x; outX <outFbw->roi.x+outFbw->roi.width; outX++)
            {
                outXOffset = outX - outFbw->roi.x;
                outYOffset = outY - outFbw->roi.y;
                // Implicit rounding, as we are using integers, is doing the interplation
                // for us. Like a nearest neighbour, but restricting to the toplefter
                // neighbour.
                inX = xOrg + IN_X_SHIFT(outXOffset, outYOffset);
                inY = yOrg + IN_Y_SHIFT(outXOffset, outYOffset);
                inPxlData = inPtr[inX + inY * inFbw->fb.width];

                outPxlData = inPxlData;
                outPtr[outX + outY * outFbw->fb.width] = outPxlData;
            }
            // We are wrapping to the next line:
        }
    }

    IMGS_PROFILE_FUNCTION_EXIT(imgs_SimilarizeImage);
    return IMGS_ERR_NO;
}




