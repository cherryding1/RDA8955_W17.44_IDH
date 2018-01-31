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


#define STRINGIFY_VALUE(s) STRINGIFY(s)
#define STRINGIFY(s) #s

const char bcpu_romCsString[] =
    STRINGIFY_VALUE(CT_ASIC_CAPD) "\n"
    "Copyright RDA Microelectronics 2012\n";


const char bcpu_romDT[] =
    "RDA\n";



