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

static unsigned long int g_currentRandValue = 1;

// ============================================================================
// srand
// ----------------------------------------------------------------------------
/// Initializes the generator
// ============================================================================
void srand(unsigned int init)
{
    g_currentRandValue = init;
}

// ============================================================================
// rand
// ----------------------------------------------------------------------------
/// Generates a new value
// ============================================================================
int rand()
{
    //Based on Knuth "The Art of Computer Programming"
    g_currentRandValue = g_currentRandValue * 1103515245 + 12345;
    return ( (unsigned int) (g_currentRandValue / 65536) % (RAND_MAX+1) );
}
