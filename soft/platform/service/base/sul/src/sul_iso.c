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

#include <sul.h>
enum
{
    ISO_8859_UNKNOW,
    ISO_8859_TYPE1,
    ISO_8859_TYPE2,
    ISO_8859_TYPE3,
    ISO_8859_TYPE4,
    ISO_8859_TYPE5,
    ISO_8859_TYPE6,
    ISO_8859_TYPE7

};
UINT8 ISO8859_GSM_Table1[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0xA3,
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0xA5,
    /*GSM 0x04*/ 0xE8,
    /*GSM 0x05*/ 0xE9,
    /*GSM 0x06*/ 0xF9,
    /*GSM 0x07*/ 0xEC,
    /*GSM 0x08*/ 0xF2,
    /*GSM 0x09*/ 0xC7,
    /*GSM 0x0a*/ 0x0a, //not contianed in ISO8859_1
    /*GSM 0x0b*/ 0xD8,
    /*GSM 0x0c*/ 0xF8,
    /*GSM 0x0d*/ 0x0D,//not contianed in ISO8859_1
    /*GSM 0x0e*/ 0xC5,
    /*GSM 0x0f*/ 0xE5,
    /*GSM 0x10*/ 0x10,//not in ISO8859_1
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0x12,//not in ISO8859_1
    /*GSM 0x13*/ 0x13,//not in ISO8859_1
    /*GSM 0x14*/ 0x14,//not in ISO8859_1
    /*GSM 0x15*/ 0x15,//not in ISO8859_1
    /*GSM 0x16*/ 0x16,//not in ISO8859_1
    /*GSM 0x17*/ 0x17,//not in ISO8859_1
    /*GSM 0x18*/ 0x18,//not in ISO8859_1
    /*GSM 0x19*/ 0x19,//not in ISO8859_1
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_1
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0xC6,
    /*GSM 0x1d*/ 0xE6,
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0xC9,


    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0xC4,
    /*GSM 0x5c*/ 0xD6,
    /*GSM 0x5d*/ 0xD1,
    /*GSM 0x5e*/ 0xDC,
    /*GSM 0x5f*/ 0xA7,

    /*GSM 0x7b*/ 0xE4,
    /*GSM 0x7c*/  0xF6,
    /*GSM 0x7d*/ 0xF1,
    /*GSM 0x7e*/ 0xFC,
    /*GSM 0x7f*/  0xE0,
    /*end*/0xff
};
UINT8 ISO8859_GSM_Table2[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0x01,//not  in ISO8859_2
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not  in ISO8859_2
    /*GSM 0x04*/ 0x04,//not  in ISO8859_2
    /*GSM 0x05*/ 0xE9,
    /*GSM 0x06*/ 0x06,//not  in ISO8859_2
    /*GSM 0x07*/ 0x07,//not  in ISO8859_2
    /*GSM 0x08*/ 0x08,//not  in ISO8859_2
    /*GSM 0x09*/ 0xC7,
    /*GSM 0x0a*/ 0x0a, //not  in ISO8859_2
    /*GSM 0x0b*/ 0x0b,//not  in ISO8859_2
    /*GSM 0x0c*/ 0x0c,//not  in ISO8859_2
    /*GSM 0x0d*/ 0x0D,//not  in ISO8859_2
    /*GSM 0x0e*/ 0xC3,
    /*GSM 0x0f*/ 0xE3,
    /*GSM 0x10*/ 0x10,//not in ISO8859_2
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0x12,//not in ISO8859_2
    /*GSM 0x13*/ 0x13,//not in ISO8859_2
    /*GSM 0x14*/ 0x14,//not in ISO8859_2
    /*GSM 0x15*/ 0x15,//not in ISO8859_2
    /*GSM 0x16*/ 0x16,//not in ISO8859_2
    /*GSM 0x17*/ 0x17,//not in ISO8859_2
    /*GSM 0x18*/ 0x18,//not in ISO8859_2
    /*GSM 0x19*/ 0x19,//not in ISO8859_2
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_2
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0x1c,//not  in ISO8859_2
    /*GSM 0x1d*/ 0x1d,//not  in ISO8859_2
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0xC9,

    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0xC4,
    /*GSM 0x5c*/ 0xD6,
    /*GSM 0x5d*/ 0xD1,// to be confirm
    /*GSM 0x5e*/ 0xDC,
    /*GSM 0x5f*/ 0xA7,

    /*GSM 0x7b*/ 0xE4,
    /*GSM 0x7c*/  0xF6,
    /*GSM 0x7d*/ 0xF1,
    /*GSM 0x7e*/ 0xFC,
    /*GSM 0x7f*/  0x7f,//not contian in ISO8859_2
    /*end*/0xff
};

UINT8 ISO8859_GSM_Table3[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0xA3,
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not  in ISO8859_3
    /*GSM 0x04*/ 0xE8,
    /*GSM 0x05*/ 0xE9,
    /*GSM 0x06*/ 0xF9,
    /*GSM 0x07*/ 0xEC,
    /*GSM 0x08*/ 0xF2,
    /*GSM 0x09*/ 0xC7,
    /*GSM 0x0a*/ 0x0a, //not  in ISO8859_3
    /*GSM 0x0b*/ 0x0b,//not  in ISO8859_3
    /*GSM 0x0c*/ 0x0c,//not  in ISO8859_3
    /*GSM 0x0d*/ 0x0D,//not  in ISO8859_3
    /*GSM 0x0e*/ 0x0e,//not  in ISO8859_3
    /*GSM 0x0f*/ 0x0f,//not  in ISO8859_3
    /*GSM 0x10*/ 0x10,//not in ISO8859_3
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0x12,//not in ISO8859_3
    /*GSM 0x13*/ 0x13,//not in ISO8859_3
    /*GSM 0x14*/ 0x14,//not in ISO8859_3
    /*GSM 0x15*/ 0x15,//not in ISO8859_3
    /*GSM 0x16*/ 0x16,//not in ISO8859_3
    /*GSM 0x17*/ 0x17,//not in ISO8859_3
    /*GSM 0x18*/ 0x18,//not in ISO8859_3
    /*GSM 0x19*/ 0x19,//not in ISO8859_3
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_3
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0x1c,//not  in ISO8859_3
    /*GSM 0x1d*/ 0x1d,//not  in ISO8859_3
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0xC9,

    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0xC4,
    /*GSM 0x5c*/ 0xD6,
    /*GSM 0x5d*/ 0xD1,// to be confirm
    /*GSM 0x5e*/ 0xDC,
    /*GSM 0x5f*/ 0xA7,

    /*GSM 0x7b*/ 0xE4,
    /*GSM 0x7c*/  0xF6,
    /*GSM 0x7d*/ 0xF1,
    /*GSM 0x7e*/ 0xFC,
    /*GSM 0x7f*/  0xE1,
    /*end*/0xff
};

UINT8 ISO8859_GSM_Table4[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0x01,//not in ISO8859_4
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not in ISO8859_4
    /*GSM 0x04*/ 0x04,//not in ISO8859_4
    /*GSM 0x05*/ 0xE9,
    /*GSM 0x06*/ 0x06,//not in ISO8859_4
    /*GSM 0x07*/ 0x07,//not in ISO8859_4
    /*GSM 0x08*/ 0x08,//not in ISO8859_4
    /*GSM 0x09*/ 0x09,//not in ISO8859_4
    /*GSM 0x0a*/ 0x0a, //not in ISO8859_4
    /*GSM 0x0b*/ 0xD8,
    /*GSM 0x0c*/ 0xF8,
    /*GSM 0x0d*/ 0x0D,//not  in ISO8859_4
    /*GSM 0x0e*/ 0xC5,
    /*GSM 0x0f*/ 0xE5,
    /*GSM 0x10*/ 0x10,//not in ISO8859_4
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0x12,//not in ISO8859_4
    /*GSM 0x13*/ 0x13,//not in ISO8859_4
    /*GSM 0x14*/ 0x14,//not in ISO8859_4
    /*GSM 0x15*/ 0x15,//not in ISO8859_4
    /*GSM 0x16*/ 0x16,//not in ISO8859_4
    /*GSM 0x17*/ 0x17,//not in ISO8859_4
    /*GSM 0x18*/ 0x18,//not in ISO8859_4
    /*GSM 0x19*/ 0x19,//not in ISO8859_4
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_4
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0xC6,
    /*GSM 0x1d*/ 0xE6,
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0xC9,

    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0xC4,
    /*GSM 0x5c*/ 0xD6,
    /*GSM 0x5d*/ 0x5D,//not in ISO8859_4
    /*GSM 0x5e*/ 0xDC,
    /*GSM 0x5f*/ 0xA7,

    /*GSM 0x7b*/ 0xE4,
    /*GSM 0x7c*/  0xF6,
    /*GSM 0x7d*/ 0x7d,//not in ISO8859_4
    /*GSM 0x7e*/ 0xFC,
    /*GSM 0x7f*/  0x7f,//not in ISO8859_4
    /*end*/0xff
};

UINT8 ISO8859_GSM_Table5[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0x01,//not in ISO8859_5
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not in ISO8859_5
    /*GSM 0x04*/ 0x04,//not in ISO8859_5
    /*GSM 0x05*/ 0x05,//not in ISO8859_5
    /*GSM 0x06*/ 0x06,//not in ISO8859_5
    /*GSM 0x07*/ 0x07,//not in ISO8859_5
    /*GSM 0x08*/ 0x08,//not in ISO8859_5
    /*GSM 0x09*/ 0x09,//not in ISO8859_5
    /*GSM 0x0a*/ 0x0a, //not in ISO8859_5
    /*GSM 0x0b*/ 0x0b,//not in ISO8859_5
    /*GSM 0x0c*/ 0x0c,//not in ISO8859_5
    /*GSM 0x0d*/ 0x0D,//not  in ISO8859_5
    /*GSM 0x0e*/ 0x0e,//not in ISO8859_5
    /*GSM 0x0f*/ 0x0f,//not in ISO8859_5
    /*GSM 0x10*/ 0x10,//not in ISO8859_5
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0xC4,
    /*GSM 0x13*/ 0xB3,
    /*GSM 0x14*/ 0x14,//not in ISO8859_5
    /*GSM 0x15*/ 0x15,//not in ISO8859_5
    /*GSM 0x16*/ 0xBF,
    /*GSM 0x17*/ 0x17,//not in ISO8859_5
    /*GSM 0x18*/ 0x18,//not in ISO8859_5
    /*GSM 0x19*/ 0x19,//not in ISO8859_5
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_5
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0x1C,//not in ISO8859_5
    /*GSM 0x1d*/ 0x1d,//not in ISO8859_5
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0x1f,//not in ISO8859_5


    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0x5b,//not in ISO8859_5
    /*GSM 0x5c*/ 0x5c,//not in ISO8859_5
    /*GSM 0x5d*/ 0x5d,//not in ISO8859_5
    /*GSM 0x5e*/ 0x5e,//not in ISO8859_5
    /*GSM 0x5f*/ 0xFD,

    /*GSM 0x7b*/ 0x7b,//not in ISO8859_5
    /*GSM 0x7c*/  0x7c,//not in ISO8859_5
    /*GSM 0x7d*/ 0x7d,//not in ISO8859_5
    /*GSM 0x7e*/ 0x7e,//not in ISO8859_5
    /*GSM 0x7f*/  0x7f,//not in ISO8859_5
    /*end*/0xff
};

UINT8 ISO8859_GSM_Table6[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0x01,//not in ISO8859_6
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not in ISO8859_6
    /*GSM 0x04*/ 0x04,//not in ISO8859_6
    /*GSM 0x05*/ 0x05,//not in ISO8859_6
    /*GSM 0x06*/ 0x06,//not in ISO8859_6
    /*GSM 0x07*/ 0x07,//not in ISO8859_6
    /*GSM 0x08*/ 0x08,//not in ISO8859_6
    /*GSM 0x09*/ 0x09,//not in ISO8859_6
    /*GSM 0x0a*/ 0x0a, //not in ISO8859_6
    /*GSM 0x0b*/ 0x0b,//not in ISO8859_6
    /*GSM 0x0c*/ 0x0c,//not in ISO8859_6
    /*GSM 0x0d*/ 0x0D,//not  in ISO8859_6
    /*GSM 0x0e*/ 0x0e,//not in ISO8859_6
    /*GSM 0x0f*/ 0x0f,//not in ISO8859_6
    /*GSM 0x10*/ 0x10,//not in ISO8859_6
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0x12,//not in ISO8859_6
    /*GSM 0x13*/ 0x13,//not in ISO8859_6
    /*GSM 0x14*/ 0x14,//not in ISO8859_6
    /*GSM 0x15*/ 0x15,//not in ISO8859_6
    /*GSM 0x16*/ 0x16,//not in ISO8859_6
    /*GSM 0x17*/ 0x17,//not in ISO8859_6
    /*GSM 0x18*/ 0x18,//not in ISO8859_6
    /*GSM 0x19*/ 0x19,//not in ISO8859_6
    /*GSM 0x1a*/ 0x1a,//not in ISO8859_6
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0x1C,//not in ISO8859_5
    /*GSM 0x1d*/ 0x1d,//not in ISO8859_5
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0x1f,//not in ISO8859_5


    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0x5b,//not in ISO8859_6
    /*GSM 0x5c*/ 0x5c,//not in ISO8859_6
    /*GSM 0x5d*/ 0x5d,//not in ISO8859_6
    /*GSM 0x5e*/ 0x5e,//not in ISO8859_6
    /*GSM 0x5f*/ 0x5f,//not in ISO8859_6

    /*GSM 0x7b*/ 0x7b,//not in ISO8859_6
    /*GSM 0x7c*/  0x7c,//not in ISO8859_6
    /*GSM 0x7d*/ 0x7d,//not in ISO8859_6
    /*GSM 0x7e*/ 0x7e,//not in ISO8859_6
    /*GSM 0x7f*/  0x7f,//not in ISO8859_6
    /*end*/0xff
};
UINT8 ISO8859_GSM_Table7[] =
{
    /*GSM 0x00*/ 0x40,
    /*GSM 0x01*/ 0xA3,
    /*GSM 0x02*/ 0x24,
    /*GSM 0x03*/ 0x03,//not in ISO8859_7
    /*GSM 0x04*/ 0x04,//not in ISO8859_7
    /*GSM 0x05*/ 0x05,//not in ISO8859_7
    /*GSM 0x06*/ 0x06,//not in ISO8859_7
    /*GSM 0x07*/ 0x07,//not in ISO8859_7
    /*GSM 0x08*/ 0x08,//not in ISO8859_7
    /*GSM 0x09*/ 0x09,//not in ISO8859_7
    /*GSM 0x0a*/ 0x0a, //not in ISO8859_7
    /*GSM 0x0b*/ 0x0b,//not in ISO8859_7
    /*GSM 0x0c*/ 0x0c,//not in ISO8859_7
    /*GSM 0x0d*/ 0x0D,//not in ISO8859_7
    /*GSM 0x0e*/ 0x0e,//not in ISO8859_7
    /*GSM 0x0f*/ 0x0e,//not in ISO8859_7
    /*GSM 0x10*/ 0x10,//not in ISO8859_7
    /*GSM 0x11*/ 0x5F,//not sure about this
    /*GSM 0x12*/ 0xD6,
    /*GSM 0x13*/ 0xC3,
    /*GSM 0x14*/ 0xCB,
    /*GSM 0x15*/ 0xD9,
    /*GSM 0x16*/ 0xD0,
    /*GSM 0x17*/ 0xD8,
    /*GSM 0x18*/ 0xD3,
    /*GSM 0x19*/ 0xC8,
    /*GSM 0x1a*/ 0xCE,
    /*GSM 0x1b*/ 0x1b,// to be extend
    /*GSM 0x1c*/ 0x1C,//not in ISO8859_7
    /*GSM 0x1d*/ 0x1d,//not in ISO8859_7
    /*GSM 0x1e*/ 0x1e,//not sure
    /*GSM 0x1f*/ 0x1f,//not in ISO8859_7


    /*The next part not continuous with first part*/

    /*GSM 0x5b*/ 0x5b,//not in ISO8859_7
    /*GSM 0x5c*/ 0x5c,//not in ISO8859_7
    /*GSM 0x5d*/ 0x5d,//not in ISO8859_7
    /*GSM 0x5e*/ 0x5e,//not in ISO8859_7
    /*GSM 0x5f*/ 0xA7,

    /*GSM 0x7b*/ 0x7b,//not in ISO8859_7
    /*GSM 0x7c*/  0x7c,//not in ISO8859_7
    /*GSM 0x7d*/ 0x7d,//not in ISO8859_7
    /*GSM 0x7e*/ 0x7e,//not in ISO8859_7
    /*GSM 0x7f*/  0x7f,//not in ISO8859_7
    /*end*/0xff
};

INT32 SUL_GET_ISOStrlen_FROM_GSM(
    PCSTR  pGSM,        // The given GSM string.
    UINT16 nSrcSize,    // the given string's length.
    UINT8 ISO_type
)
{
    INT32 i, count;
    i = 0;
    count = 0;
    PCSTR temp = pGSM;
    while(i < nSrcSize)
    {
        if(*temp == 0x1B)
        {
            i++;
            temp++;
            if((*temp == 0x65 && ISO_type != ISO_8859_TYPE7) || *temp == 0x0a)
                count ++;
            continue;
        }
        else
            count++;
        i++;
        temp++;
    }

    return count;
}

INT32 SUL_GET_GSMStrlen_FROM_ISO(
    PCSTR  pISO,        // The given ISO string.
    UINT16 nSrcSize,    // the given string's length.
    UINT8 ISO_type
)
{
    PCSTR pSrcTmp  = pISO;
    UINT16 i = 0, count = 0;
    while(i < nSrcSize)
    {
        if(*pSrcTmp == 0x7B)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x7C)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x7D)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x7E)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x5B)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x5C)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x5D)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0x5E)
            count = (UINT16)(count + 2);
        else if(*pSrcTmp == 0xa4)
        {
            if(ISO_type == ISO_8859_TYPE7)
                count = (UINT16)(count + 2);
            else
                count++;
        }
        else
            count++;
        i++;
        pSrcTmp++;
    }
    return count;
}

PCSTR SUL_GET_TABLE_ADDRESS(UINT8 ISO_type)
{
    switch(ISO_type)
    {
        case ISO_8859_TYPE1:
            return ISO8859_GSM_Table1;
        case ISO_8859_TYPE2:
            return ISO8859_GSM_Table2;
        case ISO_8859_TYPE3:
            return ISO8859_GSM_Table3;
        case ISO_8859_TYPE4:
            return ISO8859_GSM_Table4;
        case ISO_8859_TYPE5:
            return ISO8859_GSM_Table5;
        case ISO_8859_TYPE6:
            return ISO8859_GSM_Table6;
        case ISO_8859_TYPE7:
            return ISO8859_GSM_Table7;
        default:
            return NULL;
    }
}
UINT8 SUL_IS_ISO_IN_TABLE(UINT8 ISO_Char, PCSTR Table)
{
    UINT8 i, temp;
    i = 0;
    while(Table[i] != 0xff)
    {
        if(Table[i] == ISO_Char)
        {
            if(i < 0x20)
                temp = i;
            else if(0x20 <= i && i <= 0x24)
                temp = i + 0x3b;
            else if(0x24 <= i && i <= 0x28)
                temp = i + 0x5b;
            else
                temp = i;

            return temp;
        }
        i++;
        continue;
    }
    return 0xff;
}
/******************************************************************************/
/* Function name:    SUL_ISO8859_x2GSM                                                      */
/* Description:                                                                                                     */
/*                   This function change the ISO 8859 string to gsm string                             */
/* Parameters:                                                                                              */
/*                  pSrc           The given ISO string.                                                */
/*            nSrcSize  the given string's length.                                                  */
/*            pDest         Output translation string.                                                  */
/*            ISO_type    Determine which subset of ISO8859 to be used                      */
/*                   ISO_8859_TYPE1 (1) corresponding ISO8859_1                         */
/*                   ISO_8859_TYPE2 (2) corresponding ISO8859_2                         */
/*                   ISO_8859_TYPE3 (3) corresponding ISO8859_3                         */
/*                   ISO_8859_TYPE4 (4) corresponding ISO8859_4                         */
/*                   ISO_8859_TYPE5 (5) corresponding ISO8859_5                         */
/*                   ISO_8859_TYPE6 (6) corresponding ISO8859_6                         */
/*                   ISO_8859_TYPE7 (7) corresponding ISO8859_7                         */
/* Return Values:                                                                                               */
/* Return Values:                                                                                               */
/*                  If the function succeeds, the return value is the length                            */
/*                  of Encoded string, and the pDest string will be filled                                  */
/*                  with encoded string.If the function fails,the return value                          */
/*                  is -1. To get extended error information, call                                       */
/*                  SUL_GetLastError.                                                                            */
/******************************************************************************/

INT32 SUL_ISO8859_x2GSM(
    PCSTR  pSrc,            // The given ISO string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest  ,     // Output translation string.
    UINT8 ISO_type
)
{
    PCSTR ISO_GSM_Table, pISO_Str;
    PSTR pGSM_Str;
    INT32 i;
    UINT8 temp;
    pISO_Str = pSrc;
    pGSM_Str = pDest;
    if(pDest == NULL || pSrc == NULL)
        return -1;
    if((ISO_GSM_Table = SUL_GET_TABLE_ADDRESS(ISO_type)) == NULL)
        return -1;
    i = 0;
    while(i < nSrcSize)
    {

        if((0x5b <= *pISO_Str && *pISO_Str <= 0x5e) || (0x7b <= *pISO_Str && *pISO_Str <= 0x7e))
        {
            //gsm extend part
            switch(*pISO_Str)
            {
                case 0x5b:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x3C;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x5c:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x2F;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x5d:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x3E;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x5e:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x14;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x7b:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x28;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x7c:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x40;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x7d:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x29;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                case 0x7e:
                    *pGSM_Str = 0x1B;
                    *(++pGSM_Str)  = 0x3D;
                    pGSM_Str++;
                    pISO_Str++;
                    break;
                default:
                    *pGSM_Str++ = *pISO_Str++;
                    break;
            }
            i++;
            continue;
        }
        else if((temp = SUL_IS_ISO_IN_TABLE(*pISO_Str, ISO_GSM_Table)) != 0xff)
        {
            *pGSM_Str++ = temp;
            pISO_Str++;
            i++;
            continue;
        }
        else if(*pISO_Str == 0xa4)
        {
            if(ISO_type == ISO_8859_TYPE7)
            {
                *pGSM_Str = 0x1B;
                *(++pGSM_Str)  = 0x65;
                pGSM_Str++;
                pISO_Str++;
                i++;
                continue;
            }
            else
            {
                *pGSM_Str++ = 0x24;
                pISO_Str++;
                i++;
                continue;

            }
        }
        else
        {
            *pGSM_Str++ = *pISO_Str++;
            i++;
            continue;
        }
    }
    return SUL_GET_GSMStrlen_FROM_ISO(pSrc, nSrcSize, ISO_type);

}
/******************************************************************************/
/* Function name:    SUL_GSM2ISO8859_x                                                      */
/* Description:                                                                                                     */
/*                   This function change the gsm to ISO 8859 string                                    */
/* Parameters:                                                                                              */
/*                  pSrc           The given GSM string.                                                */
/*            nSrcSize  the given string's length.                                                  */
/*            pDest         Output translation string.                                                  */
/*            ISO_type    Determine which subset of ISO8859 to be used                      */
/*                   ISO_8859_TYPE1 (1) corresponding ISO8859_1                         */
/*                   ISO_8859_TYPE2 (2) corresponding ISO8859_2                         */
/*                   ISO_8859_TYPE3 (3) corresponding ISO8859_3                         */
/*                   ISO_8859_TYPE4 (4) corresponding ISO8859_4                         */
/*                   ISO_8859_TYPE5 (5) corresponding ISO8859_5                         */
/*                   ISO_8859_TYPE6 (6) corresponding ISO8859_6                         */
/*                   ISO_8859_TYPE7 (7) corresponding ISO8859_7                         */
/* Return Values:                                                                                               */
/*                  If the function succeeds, the return value is the length                            */
/*                  of Encoded string, and the pDest string will be filled                                  */
/*                  with encoded string.If the function fails,the return value                          */
/*                  is -1. To get extended error information, call                                       */
/*                  SUL_GetLastError.                                                                            */
/******************************************************************************/

INT32 SUL_GSM2ISO8859_x(
    PCSTR  pSrc,        // The given GSM string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest ,      // Output translation string.
    UINT8 ISO_type
)
{
    PCSTR ISO_GSM_Table, pGSM_Str;
    PSTR  pISO_Str;
    INT32 i;
    pGSM_Str = pSrc;
    pISO_Str = pDest;
    if(pDest == NULL || pSrc == NULL)
        return -1;

    if((ISO_GSM_Table = SUL_GET_TABLE_ADDRESS(ISO_type)) == NULL)
        return -1;
    i = 0;
    while(i < nSrcSize)
    {
        //the gsm extend part
        if(*pGSM_Str == 0x1b)
        {
            pGSM_Str++;
            switch(*pGSM_Str)
            {
                case 0x14:
                    *pISO_Str = 0x5E;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x28:
                    *pISO_Str = 0x7B;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x29:
                    *pISO_Str = 0x7D;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x2f:
                    *pISO_Str = 0x5C;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x3C:
                    *pISO_Str = 0x5B;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x3D:
                    *pISO_Str = 0x7E;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x3E:
                    *pISO_Str = 0x5D;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x40:
                    *pISO_Str = 0x7C;
                    pISO_Str++;
                    pGSM_Str++;
                    break;
                case 0x65:
                {
                    if(ISO_type == ISO_8859_TYPE7)
                    {
                        *pISO_Str++ = 0xA4;
                        pGSM_Str++;
                    }
                    else
                    {
                        *pISO_Str++ = 0x1b;
                        *pISO_Str++ = *pGSM_Str++;
                    }
                }
                break;
                default:
                    *pISO_Str = *(--pGSM_Str);
                    pISO_Str++;
                    pGSM_Str++;
            }
            i++;
            continue;
        }
        //  else if((*pGSM_Str<0x20)&&(0<=*pGSM_Str))
        else if((*pGSM_Str < 0x20))
        {
            *pISO_Str = ISO_GSM_Table[*pGSM_Str];
            pISO_Str++;
            pGSM_Str++;
            i++;
            continue;
        }
        else if(0x5b <= *pGSM_Str && *pGSM_Str < 0x5f)
        {
            *pISO_Str = ISO_GSM_Table[*pGSM_Str - 0x3b];
            pISO_Str++;
            pGSM_Str++;
            i++;
            continue;
        }
#if 0
        else if(0x5b <= *pGSM_Str && *pGSM_Str < 0x5f)
        {
            *pISO_Str = ISO_GSM_Table[*pGSM_Str - 0x5b];
            pISO_Str++;
            pGSM_Str++;
            i++;
            continue;
        }
#endif
        else if(ISO_type != ISO_8859_TYPE7 && *pGSM_Str == 0x24)
        {
            *pISO_Str = 0xa4;
            pISO_Str++;
            pGSM_Str++;
            i++;
            continue;
        }
        else
            *pISO_Str++ = *pGSM_Str++;
        i++;
        continue;
    }
    return SUL_GET_ISOStrlen_FROM_GSM(pSrc, nSrcSize, ISO_type);
}

