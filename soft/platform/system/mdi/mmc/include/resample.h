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

#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)

#define Pmask    ((1<<Np)-1)

/* Conversion constants */

INT32 ResampleInit(INT32 InSampleRate,INT32 OutSampleRate,INT32 FrameLength,INT32 NbChnanel);


INT32 ResampleOneFrame(INT16 *InBuf,INT16 * OutBuf);



