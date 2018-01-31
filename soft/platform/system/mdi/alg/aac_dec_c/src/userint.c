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





#include "wrapper.h"

#if defined (_WIN32) && !defined (_WIN32_WCE)

#include <conio.h>  /* for kbhit function */

uiSignal PollHardware(void)
{
    int kbChar;

    /* abort on keypress */
    if (_kbhit())
    {
        kbChar = _getch();
        return Stop;
    }
    else
    {
        return Play;
    }
}

#else

uiSignal PollHardware(void)
{
    return Play;
}

#endif

