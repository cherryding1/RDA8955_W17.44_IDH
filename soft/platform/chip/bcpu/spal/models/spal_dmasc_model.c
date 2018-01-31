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
#include "opl_fixed.h"


#define MULAW1 1
#define MULAW2 2
#define MULAW3 3
#define MULAW4 4
#define MULAW5 5
#define MULAW6 6
#define MULAW7 7
#define TRUNC1 8
#define TRUNC2 9
#define TRUNC3 10
#define TRUNC4 11
#define TRUNC5 12
#define TRUNC6 13

#define ESPAL_DMACS_COMPRESSION MULAW1

#if (ESPAL_DMACS_COMPRESSION == MULAW1)

#define COMPRESSION_8_TO_4_BIT        {8,16,24,32,48,64,96,128,136,144,152,160,176,192,224,255}
#define DECOMPRESSION_4_TO_8_BIT    {4,12,20,28,40,56,80,112,132,140,148,156,168,184,210,240}

// [  0,  7]       0000            4
// [  8, 15]       0001           12
// [ 16, 23]       0010           20
// [ 24, 31]       0011           28
// [ 32, 47]       0100           40
// [ 48, 63]       0101           56
// [ 64, 95]       0110           80
// [ 96,127]       0111          112
// [128,135]       1000          132
// [136,143]       1001          140
// [144,151]       1010          148
// [152,159]       1011          156
// [160,175]       1100          168
// [176,191]       1101          184
// [192,223]       1110          210
// [224,255]       1111          240

#elif (ESPAL_DMACS_COMPRESSION == MULAW2)

#define COMPRESSION_8_TO_4_BIT      {2,4,6,8,24,32,96,128,130,132,134,136,152,160,224,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,2,4,6,15,27,63,111,128,130,132,134,143,155,191,239}

// [  0,  1]       0000            0
// [  2,  3]       0001            2
// [  4,  5]       0010            4
// [  6,  7]       0011            6
// [  8, 23]       0100           15
// [ 24, 31]       0101           27
// [ 32, 95]       0110           63
// [ 96,127]       0111          111
// [128,129]       1000          128
// [130,131]       1001          130
// [132,133]       1010          132
// [134,135]       1011          134
// [136,151]       1100          143
// [152,159]       1101          155
// [160,223]       1110          191
// [224,255]       1111          239

#elif (ESPAL_DMACS_COMPRESSION == MULAW3)

#define COMPRESSION_8_TO_4_BIT      {2,4,6,8,12,16,24,128,130,132,134,136,140,144,152,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,2,4,6, 9,13,19, 75,128,130,132,134,137,141,147,203}

// [  0,  1]       0000            0
// [  2,  3]       0001            2
// [  4,  5]       0010            4
// [  6,  7]       0011            6
// [  8, 11]       0100            9
// [ 12, 15]       0101           13
// [ 16, 23]       0110           19
// [ 24,127]       0111           75
// [128,129]       1000          128
// [130,131]       1001          130
// [132,133]       1010          132
// [134,135]       1011          134
// [136,139]       1100          137
// [140,143]       1101          141
// [144,151]       1110          147
// [152,255]       1111          203

#elif (ESPAL_DMACS_COMPRESSION == MULAW4)

#define COMPRESSION_8_TO_4_BIT      {2,4,6,8,12,16,48,128,130,132,134,136,140,144,176,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,2,4,6, 9,13,31, 87,128,130,132,134,137,141,159,215}

// [  0,  1]       0000            0
// [  2,  3]       0001            2
// [  4,  5]       0010            4
// [  6,  7]       0011            6
// [  8, 11]       0100            9
// [ 12, 15]       0101           13
// [ 16, 47]       0110           31
// [ 48,127]       0111           87
// [128,129]       1000          128
// [130,131]       1001          130
// [132,133]       1010          132
// [134,135]       1011          134
// [136,139]       1100          137
// [140,143]       1101          141
// [144,175]       1110          159
// [176,255]       1111          215

#elif (ESPAL_DMACS_COMPRESSION == MULAW5)

#define COMPRESSION_8_TO_4_BIT      {2,4,6,8,12,16,96,128,130,132,134,136,140,144,224,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,2,4,6, 9,13,55,111,128,130,132,134,137,141,183,239}

// [  0,  1]       0000            0
// [  2,  3]       0001            2
// [  4,  5]       0010            4
// [  6,  7]       0011            6
// [  8, 11]       0100            9
// [ 12, 15]       0101           13
// [ 16, 95]       0110           55
// [ 96,127]       0111          111
// [128,129]       1000          128
// [130,131]       1001          130
// [132,133]       1010          132
// [134,135]       1011          134
// [136,139]       1100          137
// [140,143]       1101          141
// [144,223]       1110          183
// [224,255]       1111          239

#elif (ESPAL_DMACS_COMPRESSION == MULAW6)

#define COMPRESSION_8_TO_4_BIT      {4,8,12,16,24,32,48,128,132,136,140,144,152,160,176,255}
#define DECOMPRESSION_4_TO_8_BIT    {1,5, 9,13,19,27,39, 87,129,133,137,141,147,155,167,215}

// [  0,  3]       0000            1
// [  4,  7]       0001            5
// [  8, 11]       0010            9
// [ 12, 15]       0011           13
// [ 16, 23]       0100           19
// [ 24, 31]       0101           27
// [ 32, 47]       0110           39
// [ 48,127]       0111           87
// [128,131]       1000          129
// [132,135]       1001          133
// [136,139]       1010          137
// [140,143]       1011          141
// [144,151]       1100          147
// [152,159]       1101          155
// [160,175]       1110          167
// [176,255]       1111          215

#elif (ESPAL_DMACS_COMPRESSION == MULAW7)

#define COMPRESSION_8_TO_4_BIT      {4,8,12,16,24,32,96,128,132,136,140,144,152,160,176,255}
#define DECOMPRESSION_4_TO_8_BIT    {1,5, 9,13,19,27,63,111,129,133,137,141,147,155,191,239}

// [  0,  3]       0000            1
// [  4,  7]       0001            5
// [  8, 11]       0010            9
// [ 12, 15]       0011           13
// [ 16, 23]       0100           19
// [ 24, 31]       0101           27
// [ 32, 95]       0110           63
// [ 96,127]       0111          111
// [128,131]       1000          129
// [132,135]       1001          133
// [136,139]       1010          137
// [140,143]       1011          141
// [144,151]       1100          147
// [152,159]       1101          155
// [160,223]       1110          191
// [224,255]       1111          239

#elif (ESPAL_DMACS_COMPRESSION == TRUNC1)

#define COMPRESSION_8_TO_4_BIT      {1,2,3,4,5,6,7,128,129,130,131,132,133,134,135,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,2,3,4,5,6, 67,128,129,130,131,132,133,134,195}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  2]       0010            2
// [  3,  3]       0011            3
// [  4,  4]       0100            4
// [  5,  5]       0101            5
// [  6,  6]       0110            6
// [  7,127]       0111           67
// [128,128]       0000          128
// [129,129]       0001          129
// [130,130]       0010          130
// [131,131]       0011          131
// [132,132]       0100          132
// [133,133]       0101          133
// [134,134]       0110          134
// [135,255]       0111          195

#elif (ESPAL_DMACS_COMPRESSION == TRUNC2)

#define COMPRESSION_8_TO_4_BIT      {1,2,3,4,7,10,13,128,129,130,131,132,135,138,141,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,2,3,5, 8,11, 70,128,129,130,131,133,136,139,198}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  2]       0010            2
// [  3,  3]       0011            3
// [  4,  6]       0100            5
// [  7,  9]       0101            8
// [ 10, 12]       0110           11
// [ 13,127]       0111           70
// [128,128]       0000          128
// [129,129]       0001          129
// [130,130]       0010          130
// [131,131]       0011          131
// [132,134]       0100          133
// [135,137]       0101          136
// [138,140]       0110          139
// [141,255]       0111          198

#elif (ESPAL_DMACS_COMPRESSION == TRUNC3)

#define COMPRESSION_8_TO_4_BIT      {1,2,3,4,7,10,15,128,129,130,131,132,135,138,143,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,2,3,5, 8,12, 71,128,129,130,131,133,136,140,199}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  2]       0010            2
// [  3,  3]       0011            3
// [  4,  6]       0100            5
// [  7,  9]       0101            8
// [ 10, 14]       0110           12
// [ 15,127]       0111           71
// [128,128]       0000          128
// [129,129]       0001          129
// [130,130]       0010          130
// [131,131]       0011          131
// [132,134]       0100          133
// [135,137]       0101          136
// [138,142]       0110          140
// [143,255]       0111          199

#elif (ESPAL_DMACS_COMPRESSION == TRUNC4)

#define COMPRESSION_8_TO_4_BIT      {1,2,3,4,7,12,19,128,129,130,131,132,135,140,147,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,2,3,5, 9,15, 73,128,129,130,131,133,137,143,201}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  2]       0010            2
// [  3,  3]       0011            3
// [  4,  6]       0100            5
// [  7, 11]       0101            9
// [ 12, 18]       0110           15
// [ 19,127]       0111           73
// [128,128]       0000          128
// [129,129]       0001          129
// [130,130]       0010          130
// [131,131]       0011          131
// [132,134]       0100          133
// [135,139]       0101          137
// [140,146]       0110          143
// [147,255]       0111          201

#elif (ESPAL_DMACS_COMPRESSION == TRUNC5)

#define COMPRESSION_8_TO_4_BIT      {1,2,5,8,11,14,17,128,129,130,133,136,139,142,145,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,3,6, 9,12,15, 72,128,129,131,134,137,140,143,200}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  4]       0010            3
// [  5,  7]       0011            6
// [  8, 10]       0100            9
// [ 11, 13]       0101           12
// [ 14, 16]       0110           15
// [ 17,127]       0111           72
// [128,128]       0000          128
// [129,129]       0001          129
// [130,132]       0010          131
// [133,135]       0011          134
// [136,138]       0100          137
// [139,141]       0101          140
// [142,144]       0110          143
// [145,255]       0111          200

#elif (ESPAL_DMACS_COMPRESSION == TRUNC6)

#define COMPRESSION_8_TO_4_BIT      {1,2,5,8,13,18,23,128,129,130,133,136,141,146,151,255}
#define DECOMPRESSION_4_TO_8_BIT    {0,1,3,6,10,15,20, 75,128,129,131,134,137,143,148,203}

// [  0,  0]       0000            0
// [  1,  1]       0001            1
// [  2,  4]       0010            3
// [  5,  7]       0011            6
// [  8, 12]       0100           10
// [ 13, 17]       0101           15
// [ 18, 22]       0110           20
// [ 23,127]       0111           75
// [128,128]       0000          128
// [129,129]       0001          129
// [130,132]       0010          131
// [133,135]       0011          134
// [136,140]       0100          137
// [141,145]       0101          143
// [146,150]       0110          148
// [151,255]       0111          203

#endif



CONST UINT8 Table8To4Compress[2][16] =
{
    COMPRESSION_8_TO_4_BIT,
    DECOMPRESSION_4_TO_8_BIT
};


//-----------------------------------------------------------------------------
// spal_DmascSoftCompressSave
//-----------------------------------------------------------------------------
// This function performs the 8-to-4 bit compression of the soft bits
// @param src                UINT8*.  IN            data in buffer
// @param dest                UINT8*.  OUT        data out buffer
// @param length            UINT8.   INPUT        data buffer length.
//=============================================================================

PUBLIC VOID spal_DmascSoftCompressSave(UINT8 *src, INT16 length, UINT8 *dest)
{
    INT16 i;
    INT8 j,k;

    for (i=0; i<length; i++)
    {
        UINT8 temp[2] = {15,15};

        for (j = 15; j > -1; j--)
        {
            for (k = 0; k < 2; k++)
            {
                if ((UINT8)src[2*i+k]<Table8To4Compress[0][j])
                {
                    temp[k] = j;
                }
            }
        }

        dest[i] = (temp[0] << 0) | (temp[1] << 4);
    }
}




//=============================================================================
//  espal_DmascSoftCombine
//-----------------------------------------------------------------------------
// recombine soft bits using different schemes.
//
// @param curVal UINT8. INPUT.  old data.
// @param newVal UINT8. new data.
//
// @return UINT8. recombined data.
//
// @author zhangchangchun
// @date 2007/09/18
//=============================================================================
INLINE UINT8 espal_DmascSoftCombine(UINT8 curVal, UINT8 newVal)
{

    INT8  result8;
    INT16 result16;
    INT16 curVal16;
    INT16 newVal16;

    curVal16 = (curVal & 0x80) ? (~((INT16)curVal)) | 0xff80 : (INT16)curVal;
    newVal16 = (newVal & 0x80) ? (~((INT16)newVal)) | 0xff80 : (INT16)newVal;
    result16 = newVal16 + curVal16;

    OPL_SAT_8(result16,result8);

    return ((result8 & 0x80)? (~result8) | 0x80 : result8);
}





PUBLIC VOID spal_DmascRecombine(    UINT8*  inBuf0,
                                    UINT32* inOutPps0,
                                    UINT8*  inBuf1,
                                    UINT32* inPps1,
                                    UINT16  len,
                                    UINT8*  outBuf)

{
    UINT16 j;
    UINT16 psIdx[3];

    for (j = 0, psIdx[0] = 0, psIdx[1] = 0, psIdx[2] = 0; j < len; j++)
    {
        if (inBuf1)
        {
            UINT8 temp = (psIdx[1] & 1) ? (inBuf1[psIdx[1]/2] >> 4) : inBuf1[psIdx[1]/2];
            temp  = Table8To4Compress[1][temp & 0xf];


            if ((inOutPps0[j / 32] & (1 << (j % 32))) && (inPps1[j / 32] & (1 << (j % 32))))
            {
                outBuf[psIdx[2]++] = espal_DmascSoftCombine(inBuf0[psIdx[0]++], temp);
                psIdx[1]++;
            }
            else if (inPps1[j / 32] & (1 << (j % 32)))
            {
                outBuf[psIdx[2]++] = temp;
                psIdx[1]++;
            }
            else if (inOutPps0[j / 32] & (1 << (j % 32)))
            {
                outBuf[psIdx[2]++] = inBuf0[psIdx[0]++];
            }
        }
        else if (inOutPps0[j / 32] & (1 << (j % 32)))
        {
            outBuf[psIdx[2]++] = inBuf0[psIdx[0]++];
        }
    }

    if (inPps1)
    {
        for (j = 0; j < 58; j++)
        {
            inOutPps0[j] |= inPps1[j];
        }
    }

}

