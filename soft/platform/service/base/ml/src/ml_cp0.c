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


#include "ml.h"

#ifdef CP0


//许多语言环境（例如格鲁吉亚语和印地语）都没有代码页，因为他们仅使用 Unicode 排序规则


static INT32 uni2char(UINT16 uni, UINT8 *out, UINT32 boundlen)
{
    //Add by lixp
    //Neednot check
    //
#if 0
    if( 2 != boundlen )
    {
        return ERR_ML_INVALID_CHARACTER;
    }
#endif
    UINT8 cl = uni & 0xFF;
    UINT8 ch = (uni >> 8) & 0xFF;
    if(ch == 0)
    {
        out[0] = cl;
        return 1;
    }
    else
    {
        out[0] = cl;
        out[1] = ch;
    }

    return 2;

}

static INT32 char2uni(UINT8 *rawstring, UINT32 boundlen, UINT16 *uni)
{
    //
    //Add by lixp
    //Neednot check
#if 0
    if( 2 != boundlen )
    {
        return ERR_ML_INVALID_CHARACTER;
    }
#endif
    if(rawstring[0] <= 127)
    {
        *uni = rawstring[0];
        return 1;
    }

    *uni = ( rawstring[0] << 8 ) +  rawstring[1];
    return 2;
}

static struct ML_Table table =
{
    (UINT8 *)"CP0",
    uni2char,
    char2uni,
    NULL,
    NULL,
};

UINT32 ML_InitCP0(void)
{
    return ML_RegisterTable(&table);
}

VOID ML_ExitCP0(void)
{
    ML_UnRegisterTable(&table);
}

#endif


