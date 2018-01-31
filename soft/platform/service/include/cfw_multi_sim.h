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

#ifndef __CFW_MULTI_SIM_H__

#define __CFW_MULTI_SIM_H__

#define CFW_SIM_NUMBER NUMBER_OF_SIM

typedef enum
{
    CFW_SIM_0 = 0x00,
    CFW_SIM_1 = 0x01,
    CFW_SIM_COUNT = CFW_SIM_NUMBER,
    CFW_SIM_END = 0xFF
} CFW_SIM_ID;

#endif
