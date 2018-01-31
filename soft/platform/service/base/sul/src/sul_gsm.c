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
#include <string.h>

//
//GSMBCD码,如果转化奇数个数字的话，将会在对应高位补F
//支持'*''#''p'
//13811189836 --> 0x31 0x18 0x11 0x98 0x38 0xF6
//
UINT16 SUL_AsciiToGsmBcd(
    INT8 *pNumber,  // input
    UINT8 nNumberLen,
    UINT8 *pBCD  // output should >= nNumberLen/2+1
)
{
    UINT8 Tmp;
    UINT32 i;
    UINT32 nBcdSize = 0;
    UINT8 *pTmp = pBCD;

    if(pNumber == (CONST INT8 *)NULL || pBCD == (UINT8 *)NULL)
        return FALSE;

    SUL_MemSet8(pBCD, 0, nNumberLen >> 1);
    for(i = 0; i < nNumberLen; i++)
    {
        switch(*pNumber)
        {
            case '*':
                Tmp = (INT8)0x0A;
                break;
            case '#':
                Tmp = (INT8)0x0B;
                break;
            case 'p':
                Tmp = (INT8)0x0C;
                break;
            //--------------------------------------------
            //Modify by lixp 20070626 begin
            //--------------------------------------------
            case 'w':
                Tmp = (INT8)0x0D;
                break;
            case '+':
                Tmp = (INT8)0x0E;
                break;
            //--------------------------------------------
            //Modify by lixp 20070626 end
            //--------------------------------------------
            default:
                Tmp = (INT8)(*pNumber - '0');
                break;
        }
        if(i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (INT8)(Tmp & 0x0F);
        }
        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;
    if(i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}

//
// 该函数会去掉最后的F，同时将A转化为‘*’将B转化为‘#’将C转化为‘p’
//  --> 13811189836
//
UINT16 SUL_GsmBcdToAscii (
    UINT8 *pBcd,  // input
    UINT8 nBcdLen,
    UINT8 *pNumber  // output
)
{
    UINT8 i;
    UINT8 *pTmp = pNumber;
    UINT16 nSize = nBcdLen * 2;

    for(i = 0; i < nBcdLen; i++)
    {
        if((pBcd[i] & 0x0F) == 0x0A)
        {
            *pTmp = '*';
        }
        else if((pBcd[i] & 0x0F) == 0x0B)
        {
            *pTmp = '#';
        }
        else if((pBcd[i] & 0x0F) == 0x0C)
        {
            *pTmp = 'p';
        }
        // --------------------------------------------
        // Modify by lixp 20070626 begin
        // --------------------------------------------

        else if ((pBcd[i] & 0x0F) == 0x0D)
        {
            *pTmp = 'w';
        }
        else if ((pBcd[i] & 0x0F) == 0x0E)
        {
            *pTmp = '+';
        }
        // --------------------------------------------
        // Modify by lixp 20070626 end
        // --------------------------------------------

        else
        {
            *pTmp = (pBcd[i] & 0x0F) + 0x30;
        }
        pTmp ++;
        if((pBcd[i]) >> 4 == 0x0A)
        {
            *pTmp = '*';
        }
        else if((pBcd[i]) >> 4 == 0x0B)
        {
            *pTmp = '#';
        }
        else if((pBcd[i]) >> 4 == 0x0C)
        {
            *pTmp = 'p';
        }
        // --------------------------------------------
        // Modify by lixp 20070626 begin
        // --------------------------------------------

        else if((pBcd[i]) >> 4 == 0x0D)
        {
            *pTmp = 'w';
        }
        else if((pBcd[i]) >> 4 == 0x0E)
        {
            *pTmp = '+';
        }
        // --------------------------------------------
        // Modify by lixp 20070626 end
        // --------------------------------------------
        else
        {
            *pTmp = (pBcd[i] >> 4) + 0x30;
        }
        pTmp++;
    }

    pTmp--;
    if(*pTmp == 0x3f)
    {
        *pTmp = 0x00;
        nSize -= 1;
    }

    return nSize;
}


UINT16 SUL_AsciiToGsmBcdEx(
    INT8 *pNumber,  // input
    UINT8 nNumberLen,
    UINT8 *pBCD  // output should >= nNumberLen/2+1
)
{
    UINT8 Tmp;
    UINT32 i;
    UINT32 nBcdSize = 0;
    UINT8 *pTmp = pBCD;

    if(pNumber == (CONST INT8 *)NULL || pBCD == (UINT8 *)NULL)
        return FALSE;

    SUL_MemSet8(pBCD, 0, nNumberLen >> 1);

    for(i = 0; i < nNumberLen; i++)
    {
        switch(*pNumber)
        {
            case 0x41:
            case 0x61:
                Tmp = (INT8)0x0A;
                break;
            case 0x42:
            case 0x62:
                Tmp = (INT8)0x0B;
                break;
            case 0x43:
            case 0x63:
                Tmp = (INT8)0x0C;
                break;
            //--------------------------------------------
            //Modify by lixp 20070626 begin
            //--------------------------------------------
            case 0x44:
            case 0x64:
                Tmp = (INT8)0x0D;
                break;
            case 0x45:
            case 0x65:
                Tmp = (INT8)0x0E;
                break;
            case 0x46:
            case 0x66:
                Tmp = (INT8)0x0F;
                break;
            //--------------------------------------------
            //Modify by lixp 20070626 end
            //--------------------------------------------
            default:
                Tmp = (INT8)(*pNumber - '0');
                break;
        }
        if(i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (INT8)(Tmp & 0x0F);
        }
        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if(i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}


UINT16 SUL_GsmBcdToAsciiEx (
    UINT8 *pBcd,  // input
    UINT8 nBcdLen,
    UINT8 *pNumber  // output
)
{
    UINT8 i;
    UINT8 *pTmp = pNumber;
    UINT16 nSize = nBcdLen * 2;

    for(i = 0; i < nBcdLen; i++)
    {
        if((pBcd[i] & 0x0F) == 0x0A)
        {
            *pTmp = 0x41;
        }
        else if((pBcd[i] & 0x0F) == 0x0B)
        {
            *pTmp = 0x42;
        }
        else if((pBcd[i] & 0x0F) == 0x0C)
        {
            *pTmp = 0x43;
        }
        // --------------------------------------------
        // Modify by lixp 20070626 begin
        // --------------------------------------------

        else if ((pBcd[i] & 0x0F) == 0x0D)
        {
            *pTmp = 0x44;
        }
        else if ((pBcd[i] & 0x0F) == 0x0E)
        {
            *pTmp = 0x45;
        }
        // --------------------------------------------
        // Modify by lixp 20070626 end
        // --------------------------------------------
        else if ((pBcd[i] & 0x0F) == 0x0F)
        {
            *pTmp = 0x46;
        }

        else
        {
            *pTmp = (pBcd[i] & 0x0F) + 0x30;
        }

        pTmp ++;
        if((pBcd[i]) >> 4 == 0x0A)
        {
            //*pTmp = 0x61;
            *pTmp = 0x41;
        }
        else if((pBcd[i]) >> 4 == 0x0B)
        {
            //*pTmp =0x62;
            *pTmp = 0x42;
        }
        else if((pBcd[i]) >> 4 == 0x0C)
        {
            //*pTmp =0x63;
            *pTmp = 0x43;
        }
        // --------------------------------------------
        // Modify by lixp 20070626 begin
        // --------------------------------------------

        else if((pBcd[i]) >> 4 == 0x0D)
        {
            //*pTmp =0x64;
            *pTmp = 0x44;
        }
        else if((pBcd[i]) >> 4 == 0x0E)
        {
            //*pTmp =0x65;
            *pTmp = 0x45;
        }
        else if((pBcd[i]) >> 4 == 0x0F)
        {
            //*pTmp =0x66;
            *pTmp = 0x46;
        }
        // --------------------------------------------
        // Modify by lixp 20070626 end
        // --------------------------------------------

        else
        {
            *pTmp = (pBcd[i] >> 4) + 0x30;
        }
        pTmp++;
    }

    pTmp--;
    if(*pTmp == 0x3f)
    {
        *pTmp = 0x00;
        nSize -= 1;
    }

    return nSize;
}


UINT16 SUL_gsmbcd2ascii (
    UINT8 *pBcd,  // input
    UINT8 nBcdLen,
    UINT8 *pNumber  // output
)
{
    UINT8 i;
    UINT8 *pOutput = pNumber;
    UINT8 nTable[]= {'*','#','p','w','+'};

    for(i = 0; i < nBcdLen; i++)
    {
        UINT8 low4bits = pBcd[i] & 0x0F;
        UINT8 high4bits = pBcd[i] >> 4;
        if(low4bits < 0x0A)         //0 - 0x09
        {
            *pOutput++ = low4bits + '0';
        }
        else if(low4bits < 0x0F)        //0x0A - 0x0E
        {
            *pOutput++ = nTable[low4bits - 0x0A];
        }

        if(high4bits < 0x0A)            //0 - 0x09
        {
            *pOutput++ = high4bits + '0';
        }
        else if(high4bits < 0x0F)       //0x0A - 0x0E
        {
            *pOutput++ = nTable[high4bits - 0x0A];
        }
    }
    return pOutput - pNumber;
}

/*
    UINT8 *pBcd,             Input data
    UINT16 nBcdLen,          Number of input data in byte
    UINT8 *pNumber,          Output buffer, Its size should be twice nBcdLen at least.
*/
UINT16 SUL_gsmbcd2asciiEx(
    UINT8 *pBcd,
    UINT16 nBcdLen,
    UINT8 *pNumber
)
{
    UINT16 i;
    UINT8 *pTmp = pNumber;
    for(i = 0; i < nBcdLen; i++)
    {
        UINT8 high4bits = pBcd[i] >> 4;
        UINT8 low4bits = pBcd[i] & 0x0F;

        if(low4bits < 0x0A)
            *pTmp++ = low4bits + '0';   // 0 - 0x09
        else if(low4bits < 0x0F)        // 0x0A - 0x0F
            *pTmp++ = low4bits - 0x0A + 'A';

        if(high4bits < 0x0A)
            *pTmp++ = high4bits + '0';  // 0 - 9
        else if(high4bits < 0x0F)       // 0x0A - 0x0F
            *pTmp++ = high4bits - 0x0A + 'A';
    }
    return pTmp - pNumber;
}


UINT8 SUL_hex2ascii(UINT8* pInput, UINT16 nInputLen, UINT8* pOutput)
{
    UINT16 i;
    UINT8* pBuffer = pOutput;
    for(i = 0; i < nInputLen; i++)
    {
        UINT8 high4bits = pInput[i] >> 4;
        UINT8 low4bits = pInput[i] & 0x0F;

        if(high4bits < 0x0A)
            *pOutput++ = high4bits + '0';   // 0 - 9
        else                            // 0x0A - 0x0F
            *pOutput++ = high4bits - 0x0A + 'A';

        if(low4bits < 0x0A)
            *pOutput++ = low4bits + '0';    // 0 - 0x09
        else                            // 0x0A - 0x0F
            *pOutput++ = low4bits - 0x0A + 'A';
    }
    return pOutput -pBuffer;
}

UINT8 SUL_ascii2hex(UINT8* pInput, UINT8* pOutput)
{
    UINT16 i = 0;
    UINT16 length = strlen(pInput);
    UINT16 j = length;

    UINT8 nData;
    UINT8* pBuffer = pOutput;
    UINT8* pIn = pInput;

    if((length & 0x01) == 0x01)
    {
        nData  = pIn[0];
        if((nData >= '0') && (nData <= '9'))
            *pBuffer = nData - '0';
        else if((nData >= 'a') && (nData <= 'f'))
            *pBuffer = nData - 'a' + 10;
        else if((nData >= 'A') && (nData <= 'F'))
            *pBuffer = nData - 'A' + 10;
        else
            return 0;
        length--;
        pBuffer++;
        pIn++;
    }

    for(i = 0; i < length; i++)
    {
        if((i & 0x01 )  == 0x01)            // even is high 4 bits, it should be shift 4 bits for left.
            *pBuffer = (*pBuffer) << 4;
        else
            *pBuffer = 0;

        nData = pIn[i];
        if((nData >= '0') && (nData <= '9'))
            *pBuffer |= nData - '0';
        else if((nData >= 'a') && (nData <= 'f'))
            *pBuffer |= nData - 'a' + 10;
        else if((nData >= 'A') && (nData <= 'F'))
            *pBuffer |= nData - 'A' + 10;
        else
            return 0;
        if((i & 0x01 )  == 0x01)
            pBuffer++;
    }
    if((j & 0x01 )  == 0x01)
        length++;
    return (length + 1) >> 1;
}

