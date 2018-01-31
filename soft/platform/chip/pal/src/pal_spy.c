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

















#include "pal_spy.h"


/**********************
 ** Global variables **
 **********************/
CHAR *SpyPalText = "PAL";
const CHAR *SpyPalItem[PALSPY_QTY]=
{
    "FCPU  %d",       /* CPU free time (in idle task) */
    "CPUIT %d",       /* CPU load (in interrupt task) */
    "wrAdj %d",       /* PAL TCU wrap adjust quantity */
    "TcuMx %d",       /* Maximum TCU number used */
    "ToTof %d",       /* Total time offset from reset */
    "ToFof %d",       /* Total freq offset from reset */
    "MonQt %d",       /* Monitoring windows quantity */
    "MonPw %d",       /* Last monitoring value */
    "FccQt %d",       /* FCCH windows quantity */
    "FcFof %d",       /* Last FCCH FOf value */
    "FcTof %d",       /* Last FCCH TOf value */
    "FcOk  %d",       /* FCCH OK quantity */
    "FcKo  %d",       /* FCCH KO quantity */
    "SchQt %d",       /* SCH windows quantity */
    "SchFo %d",       /* Last SCH FOf value */
    "SchTo %d",       /* Last SCH TOf value */
    "SchSn %d",       /* Last SCH SNR value */
    "SchOk %d",       /* SCH OK quantity */
    "SchKo %d",       /* SCH KO quantity */
    "IntQt %d",       /* Interference windows quantity */
    "NbQt  %d",       /* Normal burst windows quantity */
    "NbFof %d",       /* Last Normal burst FOf value */
    "NbTof %d",       /* Last Normal burst TOf value */
    "NbSnr %d",       /* Last Normal burst SNR value */
    "NbPwr %d",       /* Last Normal burst power value */
    "NbBer %d",       /* Last Normal burst BER value */
    "NbOk  %d",       /* Normal burst OK quantity */
    "NbKo  %d",       /* Normal burst KO quantity */
    "TxAcc %d",       /* Tx Access burst encoding quantity */
    "TxNbl %d",       /* Tx Normal burst encoding quantity */
    "TxQty %d"        /* Tx windows quantity */
};

INT16 SpyPalValues[PALSPY_QTY];
