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

#include "hal_sys.h"

#include "singen.h"
#include "sinref.h"



/* -------------------------------------------------- */
/* ----- Destination buffer. */

#define AMPL_1            255
#define FN                f / SINREF_F



// =============================================================================
// sinGen
// -----------------------------------------------------------------------------
/// Generate a sine wave
/// @param f Frequency
/// @param ampl Amplitude
/// @param sinBuf Pointer to the buffer where the sinwave is stored.
/// @param bufSize Size of the buffer, in bytes.
// =============================================================================
PROTECTED VOID sinGen(UINT16 f, UINT8 ampl, UINT32 *sinBuf, UINT32 bufSize)
{
    /* fn and nn are the normalized versions of f and n. */
    UINT32 n, nn;
    /* fn = n / SINREF_F */
    UINT32 tmpSpl;
    UINT16    *sinPtr = (UINT16*)HAL_SYS_GET_UNCACHED_ADDR(sinBuf);

    /* Generates the sinwave. */
    for (n = 0; n < bufSize / 2; n++)
    {
        /* Scale the samples on a period of a sine. */
        nn = ( (n * f) / SINREF_F ) % (SINREF_SIZE * 4);

        /* Scale the samples on a quarter of a period of a sine. */
        if (nn >= SINREF_SIZE * 3)
        {
            /* Sample calculation, with good amplitude. */
            tmpSpl = (((UINT32)sinRef[(SINREF_SIZE * 4) - nn - 1]) *ampl) /AMPL_1;

            /* Saturation check on 15 bits. */
            if (tmpSpl > 0x7fff) tmpSpl = 0x7fff;

            sinPtr[n] = ~((UINT16)tmpSpl) + 1;
        }
        else if (nn >= SINREF_SIZE * 2)
        {
            /* Sample calculation, with good amplitude. */
            tmpSpl = (((UINT32)sinRef[nn - (SINREF_SIZE * 2)]) *ampl) /AMPL_1;

            /* Saturation check. */
            if (tmpSpl > 0x7fff) tmpSpl = 0x7fff;

            sinPtr[n] = ~((UINT16)tmpSpl) + 1;
        }
        else if (nn >= SINREF_SIZE)
        {
            /* Sample calculation, with good amplitude. */
            tmpSpl = (((UINT32)sinRef[(SINREF_SIZE * 2) - nn - 1]) *ampl) /AMPL_1;

            /* Saturation check. */
            if (tmpSpl > 0x7fff) tmpSpl = 0x7fff;

            sinPtr[n] = (UINT16)tmpSpl;
        }
        else
        {
            /* Sample calculation, with good amplitude. */
            tmpSpl = (((UINT32)sinRef[nn]) *ampl) /AMPL_1;

            /* Saturation check. */
            if (tmpSpl > 0x7fff) tmpSpl = 0x7fff;

            sinPtr[n] = (UINT16)tmpSpl;
        }
    }
}



