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
#include "stdlib.h"


PRIVATE  STD_COMPAR_FUNC_T*    g_stdFCmp;
PRIVATE  UINT32                g_stdQWidth;

PRIVATE VOID exchange(char  *leftP, char *rightP )
{
    INT32  i;
    CHAR  c;

    for (i = 0; i < (INT32)g_stdQWidth; i++ )
    {
        c = *rightP;
        *rightP++ = *leftP;
        *leftP++ = c;
    }
}


PRIVATE VOID  qSortHelp(char *pivotP, INT32 nElem)
{
    char     *leftP, *rightP, *pivotEnd, *pivotTemp, *leftTemp;
    UINT32  lNum;
    INT32   retval;

tailRecursion:
    if (nElem <= 2)
    {
        if (nElem == 2)
        {
            if (g_stdFCmp (pivotP, rightP = g_stdQWidth + pivotP) > 0)
                exchange(pivotP, rightP);
        }
        return;
    }

    rightP = (nElem - 1) * g_stdQWidth + pivotP;
    leftP  = (nElem >> 1) * g_stdQWidth + pivotP;


    if (g_stdFCmp (leftP, rightP) > 0)
        exchange(leftP, rightP);
    if (g_stdFCmp (leftP, pivotP) > 0)
        exchange(leftP, pivotP);
    else if (g_stdFCmp (pivotP, rightP) > 0)
        exchange(pivotP, rightP);

    if (nElem == 3)
    {
        exchange(pivotP, leftP);
        return;
    }


    leftP = pivotEnd = pivotP + g_stdQWidth;

    do
    {
        while ((retval = g_stdFCmp(leftP, pivotP)) <= 0)
        {
            if (retval == 0)
            {
                exchange(leftP, pivotEnd);
                pivotEnd += g_stdQWidth;
            }
            if (leftP< rightP)
                leftP += g_stdQWidth;
            else
                goto qBreak;
        }

        while (leftP< rightP)
        {
            if ((retval = g_stdFCmp(pivotP, rightP)) < 0)
                rightP -= g_stdQWidth;
            else
            {
                exchange(leftP, rightP);
                if (retval != 0)
                {
                    leftP += g_stdQWidth;
                    rightP -= g_stdQWidth;
                }
                break;
            }
        }
    }
    while (leftP< rightP);

qBreak:

    if (g_stdFCmp(leftP, pivotP) <= 0)
        leftP = leftP + g_stdQWidth;

    leftTemp = leftP - g_stdQWidth;

    pivotTemp = pivotP;

    while ((pivotTemp < pivotEnd) && (leftTemp >= pivotEnd))
    {
        exchange(pivotTemp, leftTemp);
        pivotTemp += g_stdQWidth;
        leftTemp -= g_stdQWidth;
    }

    lNum = (leftP - pivotEnd) / g_stdQWidth;
    nElem = ((nElem * g_stdQWidth + pivotP) - leftP)/g_stdQWidth;


    if (nElem < (INT32)lNum)
    {
        qSortHelp (leftP, nElem);
        nElem = lNum;
    }
    else
    {
        qSortHelp (pivotP, lNum);
        pivotP = leftP;
    }

    goto tailRecursion;
}



PUBLIC VOID  qsort (VOID *baseP, INT32 nElem, INT32 width, STD_COMPAR_FUNC_T *compar)
{
    if ((g_stdQWidth = width) == 0 || (nElem <= 1))
        return;

    g_stdFCmp = compar;

    qSortHelp (baseP, nElem);
}

