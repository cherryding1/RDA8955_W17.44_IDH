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



#include "spal_cap.h"
#include "spalp_private.h"
#include "spalp_common_model.h"
#include "spal_cfg.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
// Tables only for the models.


typedef struct
{
    UINT16 nbInBits;
    UINT16 nbEncBits;
    UINT16 nbDecBits;
} SIMU_CONV_CRC_PARAM_T;


// TODO : to be removed


PRIVATE CONST SIMU_CONV_CRC_PARAM_T simu_spalConvCrcParam[CS_MCS9 + 1] =
{
    //ConvInBits EncBits DecBits
    { 0,        0,       0}, // RACH
    { 0,        0,       0}, // PRACH
    { 0,        0,       0}, // SCH
    { 0,        0,       0}, // CS1
    { 0,        0,       0}, // CS2
    { 0,        0,       0}, // CS3
    { 0,        0,       0}, // CS4
    {51,       37,     162}, // HEAD_1
    {39,       25,     135}, // HEAD_2
    {42,       28,     117}, // HEAD_3
    { 0,      178,       0}, // MCS1
    { 0,      226,       0}, // MCS2
    { 0,      298,       0}, // MCS3
    { 0,      354,       0}, // MCS4
    { 0,      450,       0}, // MCS5
    { 0,      594,       0}, // MCS6
    { 0,      450,       0}, // MCS7
    { 0,      546,       0}, // MCS8
    { 0,      594,       0}, // MCS9
};


UINT16 PUNCTURE_HR[]=
{
    1,  4,  7, 10, 13, 16, 19, 22, 25, 28,
    31, 34, 37, 40, 43, 46, 49, 52, 55, 58,
    61, 64, 67, 70, 73, 76, 79, 82, 85, 88,
    91, 94, 97,100,103,106,109,112,115,118,
    121,124,127,130,133,136,139,142,145,148,
    151,154,157,160,163,166,169,172,175,178,
    181,184,187,190,193,196,199,202,205,208,
    211,214,217,220,223,226,229,232,235,238,
    241,244,247,250,253,256,259,262,265,268,
    271,274,277,280,283,295,298,301,304,307,
    310
};



UINT16 pnctrCS2[]=
{
    15, 19, 23, 27, 31, 35, 43, 47, 51, 55,
    59, 63, 67, 71, 75, 79, 83, 91, 95, 99,
    103,107,111,115,119,123,127,131,139,143,
    147,151,155,159,163,167,171,175,179,187,
    191,195,199,203,207,211,215,219,223,227,
    235,239,243,247,251,255,259,263,267,271,
    275,283,287,291,295,299,303,307,311,315,
    319,323,331,335,339,343,347,351,355,359,
    363,367,371,379,383,387,391,395,399,403,
    407,411,415,419,427,431,435,439,443,447,
    451,455,459,463,467,475,479,483,487,491,
    495,499,503,507,511,515,523,527,531,535,
    539,543,547,551,555,559,563,571,575,579,
    583,587
};
UINT16 pnctrCS3[]=
{
    15, 17, 21, 23, 27, 29, 33, 35, 39, 41,
    45, 47, 51, 53, 57, 59, 63, 65, 69, 71,
    75, 77, 81, 83, 87, 89, 93, 95, 99,101,
    105,107,111,113,117,119,123,125,129,131,
    135,137,141,143,147,149,153,155,159,161,
    165,167,171,173,177,179,183,185,189,191,
    195,197,201,203,207,209,213,215,219,221,
    225,227,231,233,237,239,243,245,249,251,
    255,257,261,263,267,269,273,275,279,281,
    285,287,291,293,297,299,303,305,309,311,
    315,317,321,323,327,329,333,335,339,341,
    345,347,351,353,357,359,363,365,369,371,
    375,377,381,383,387,389,393,395,399,401,
    405,407,411,413,417,419,423,425,429,431,
    435,437,441,443,447,449,453,455,459,461,
    465,467,471,473,477,479,483,485,489,491,
    495,497,501,503,507,509,513,515,519,521,
    525,527,531,533,537,539,543,545,549,551,
    555,557,561,563,567,569,573,575,579,581,
    585,587,591,593,597,599,603,605,609,611,
    615,617,621,623,627,629,633,635,639,641,
    645,647,651,653,657,659,663,665,669,671
};



//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_InsertBits
//-----------------------------------------------------------------------------
// Cap driver function to decode CRC
//
// @param .
// @param s1 UINT8*. Pointer to the .
// @param s2 UINT8*. Pointer to the .
//
//=============================================================================

PRIVATE void spal_BufferClear(UINT8 *cc_buffer,UINT32 len)
{
    UINT32 i;
    if(len%8)
        for(i=0; i < len/8+1; i++)
            cc_buffer[i] = 0;
    else
        for(i=0; i < len/8; i++)
            cc_buffer[i] = 0;
}

PRIVATE void spal_InsertBits(UINT8* s1, UINT8* s2, UINT16 length_s1,
                             UINT16 position_s2, UINT16 length_s2)
{
    UINT16 i, j, k;

    /*move second part s1, "length_s1-position_s2" bits,  to new position.*/
    j = length_s1 > position_s2 ? length_s1 - position_s2 : 0;
    for (i = 0; i < j; i++)
    {
        k = length_s1 + length_s2 - 1 - i;
        s1[k / 8] &= (~(1 << (k % 8)));
        s1[k / 8] |= ((!!(s1[(k - length_s2) / 8] & 1 << ((k - length_s2) % 8))) << (k % 8));
    }
    /*fill in s2*/
    for (i = 0; i < length_s2; i++)
    {
        s1[(i + position_s2) / 8] &= (~(1 << ((i + position_s2) % 8)));
        s1[(i + position_s2) / 8] |= ((!!(s2[i / 8] & 1 << (i % 8))) << ((i + position_s2) % 8));
    }

}



PRIVATE void spal_memset(output, value, len)
UINT8 * output;                 /* output block */
UINT8 value;                      /* value */
UINT32 len;               /* length of block */
{
    if(len != 0)
    {
        do
        {
            *output++ = (UINT8)value;
        }
        while(--len != 0);
    }
}




/* Secure memcpy routine */

PRIVATE void spal_memcpy(output, input, len)
UINT8 * output;                 /* output block */
UINT8 * input;                  /* input block */
UINT32 len;               /* length of blocks */
{
    if (len != 0)
    {
        do
        {
            *output++ = *input++;
        }
        while (--len != 0);
    }
}




PRIVATE void spal_PnctrPDCH(UINT8 *ctmp,UINT8 *c,UINT8 chSubMode)
{
    UINT16 *pPnctrASlctr, *pPnctrAIndx[2] = {pnctrCS2, pnctrCS3};
    UINT16 inLngth[4] = {456,588,676,456};

    /*i: process all the input bits.                                              */
    /*j: the bit whose position in s1 is pPnctrASlctr[j] should be deleted.       */
    /*k: the position of output bit.                                              */
    UINT16 i, j = 0, k = 0;

    UINT16 length = inLngth[chSubMode];

    /*CS1    0 CS2    1 CS3    2 CS4    3         */
    if((CS_CS1==chSubMode)||(CS_CS4==chSubMode))
    {
        spal_memcpy(c,ctmp,inLngth[chSubMode]);
        return;
    }

    pPnctrASlctr = pPnctrAIndx[chSubMode-1];

    spal_memset(c,0,57);

    for (i = 0; i < length; i++)
    {
        if (pPnctrASlctr[j] != i)   /*TRUE: the bit should be transmited.     */
        {
            /*otherwise, it will be deleted.          */
            c[k / 8] |= ((!!(ctmp[i / 8] & 1 << (i % 8))) << (k % 8));
            k++;
        }
        else
            j++;
    }
}



//=============================================================================
// spal_PreScal
//-----------------------------------------------------------------------------
// Function for the prescaling
//
//=============================================================================
PRIVATE VOID spal_PreScal (SPAL_CODING_SCHEME_T cs, UINT8* in, UINT8* out, UINT16 length)
{
    UINT16 i, j;
    UINT8 reg[7] = {0, 0, 0, 0, 0, 0, 0};

    j = length / 8 + 1;

    for (i = 0; i < j; i++)
    {
        out [i]= 0;
    }

    switch (cs)
    {


        case CS_AFS_SPEECH_122:
        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
            for (i = 0; i < length - 4; i++)
            {
                reg[4] = (!!(in[i / 8] & 1 << (i % 8))) ^ reg[1] ^ reg[0];
                out[i / 8]|=(reg[4] << (i % 8));
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
            }
            for (i = length - 4; i < length; i++)
            {
                CLEARBIT(out, i);
            }

            break;
        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_74:
        case CS_AFS_SPEECH_67:
        case CS_AFS_SPEECH_515:
        case CS_AHS_SPEECH_515:
            for (i = 0; i < length - 4; i++)
            {
                reg[4] = (!!(in[i / 8] & 1 << (i % 8))) ^ reg[3] ^ reg[2] ^ reg[1] ^ reg[0];
                out[i / 8]|=(reg[4] << (i % 8));
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
            }
            for (i = length - 4; i < length; i++)
            {
                CLEARBIT(out, i);
            }
            break;
        case CS_AFS_SPEECH_795:
        case CS_AHS_SPEECH_475:
            for (i = 0; i < length -6; i++)
            {
                reg[6] = (!!(in[i / 8] & 1 << (i % 8))) ^ reg[4] ^ reg[3] ^ reg[1] ^ reg[0];
                out[i / 8]|=(reg[6] << (i % 8));
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
                reg[4] = reg[5];
                reg[5] = reg[6];
            }
            for (i = length-6; i < length; i++)
            {
                CLEARBIT(out, i);
            }
            break;
        case CS_AFS_SPEECH_59:
        case CS_AFS_SPEECH_475:
            for (i = 0; i < length -6; i++)
            {
                reg[6] = (!!(in[i / 8] & 1 << (i % 8))) ^ reg[5] ^ reg[4] ^ reg[3] ^ reg[2] ^ reg[0];
                out[i / 8]|=(reg[6] << (i % 8));
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
                reg[4] = reg[5];
                reg[5] = reg[6];
            }
            for (i = length - 6; i < length; i++)
            {
                CLEARBIT(out, i);
            }

            break;
        default:
            break;
    }
}

//=============================================================================
// spal_PreScal
//-----------------------------------------------------------------------------
// Function for the puncturing
//
//=============================================================================
PRIVATE VOID spal_punct (UINT8* in, UINT8* out, UINT32* PunctTable, UINT16 length, UINT16 n)
{
    UINT16 i;
    UINT16 k=0;
    UINT8 Delete_i;

    for (i = 0; i < length*n; i++)
    {
        Delete_i = 0;                  // decision switch off
        if ((PunctTable[i / 32] & (1 << (i % 32))) == 0)
        {
            Delete_i = 1;
        }

        if (!Delete_i)
        {
            MOVEBIT(in, i, out, k);
            k++;
        }
    }
}


//=============================================================================
// spal_CapDecodeCrc
//-----------------------------------------------------------------------------
// Cap driver function to decode CRC
//
// @param coding_scheme SPAL_CODING_SCHEME_T.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PROTECTED VOID spal_CapDecodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{

    // Driver
//     UINT16 nb_bits = g_sppCsCrcParam[coding_scheme].nbDecBits;
//     UINT8 length = (nb_bits % 32)? 1+(nb_bits/32): (nb_bits/32);
//
//     SPP_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, g_sppCsCrcParam[coding_scheme].CapCfg);
//     SPP_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, nb_bits);
//     SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//     for (; length>0; length--)
//         SPP_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
//     SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//     SPP_BB_CP2_START_CRC_D;
//
//     set_label(WAIT_Cap_CRC_D);
//     asm_wait(WAIT_Cap_CRC_D);
//     asm volatile("nop");
//     SPP_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB, Out[0]);
//     if (coding_scheme == CS_CS1)
//         SPP_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_MSB, Out[1]);

    //Uint8 CC_CrcD(Uint8* dividend, enum eCrcPoly ip, Uint8 r, Uint16 length, Uint8 n)
// CC_CrcD(pcDownMid, crcFS, 1, 53, 3)
    // Software equivalent

    UINT16 nb_bits = param.nbDecBits;
    //UINT8 length = (nb_bits % 32)? 1+(nb_bits/32): (nb_bits/32);
    UINT8 reg[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    INT8 j;
    INT16 i;
    UINT8* in_8= (UINT8*) In;
    UINT8* out_8= (UINT8*) Out;

    SPAL_CODING_SCHEME_T coding_scheme = param.capCfg;

    switch (coding_scheme)
    {
        case CS_HR:
            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<3; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;

        case CS_FR:
            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<3; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;

        case CS_EFR:
            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j;
                reg[5] = reg[6] ^ j;
                reg[6] = reg[7];
                reg[7] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<8; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;


        case CS_AFS_RATSCCH:
        case CS_AFS_SID_UPDATE:

            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[ 0] = reg[ 1] ^ j;
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4];
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9] ^ j;
                reg[ 9] = reg[10];
                reg[10] = reg[11] ^ j;
                reg[11] = reg[12] ^ j;
                reg[12] = reg[13];
                reg[13] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<14; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;

        case CS_RACH:
        case CS_AFS_SPEECH_122:
        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_795:
        case CS_AFS_SPEECH_74:
        case CS_AFS_SPEECH_67:
        case CS_AFS_SPEECH_59:
        case CS_AFS_SPEECH_515:
        case CS_AFS_SPEECH_475:
        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
        case CS_AHS_SPEECH_515:
        case CS_AHS_SPEECH_475:

            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[0] = reg[1] ^ j;
                reg[1] = reg[2];
                reg[2] = reg[3] ^ j;
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j;
                reg[5] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<6; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;

// Not verified yet !!
        case CS_SCH:
            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = reg[3];
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j;
                reg[5] = reg[6] ^ j;
                reg[6] = reg[7];
                reg[7] = reg[8] ^ j;
                reg[8] = reg[9];
                reg[9] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<10; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;


        case CS_CS2:
        case CS_CS3:
        case CS_CS4:
            for (i = 0; i < nb_bits; i++)
            {
                j = reg[0];
                reg[ 0] = reg[ 1];
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4] ^ j;
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9];
                reg[ 9] = reg[10];
                reg[10] = reg[11] ^ j;
                reg[11] = reg[12];
                reg[12] = reg[13];
                reg[13] = reg[14];
                reg[14] = reg[15];
                reg[15] = j ^ !!(In[i / 32] & 1 << (i % 32));
            }
            Out[0]=0;
            for (i=0; i<16; i++)
            {
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }
            break;


        case CS_CS1:
        {
            UINT8* s;
            //UINT8* firedecode;
            /*x^(131071*23-224+12)mod genepoly*/
            UINT8 reg[40] = {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0,
                             0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1
                            };
            UINT8 poly[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
                              0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1
                             };
            /*224 bits input + 39 bits shift = 263 bits, 33 Bytes                         */
            UINT8 multi[33], s_[33];
            UINT16 i;
            UINT8 j, k, l, p;

            s=(UINT8*) In;
            //firedecode=(UINT8*) Out;


            for (i = 0; i < 33; i++)
                multi[i] = s_[i] = 0;

            //for (i = 23; i < 28; i++)
            //s[i] = ~s[i];

            /*first step : syndrome computation */

            for (i = 0; i < 40; i++)
            {
                if (reg[i])
                {
                    for (j = 0; j < i / 8 + 1; j++)
                        s_[j] = 0;
                    spal_InsertBits(s_, s, 0, i, 224);
                    for (j = 0; j < 33; j++)
                        multi[j] ^= s_[j];
                }
            }

            for (i = 0; i < 40; i++)
                reg[i] = 0;

            for (i = 0; i < 263; i++)
            {
                j = reg[0];
                reg[ 0] = reg[ 1];
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4];
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9];
                reg[ 9] = reg[10];
                reg[10] = reg[11];
                reg[11] = reg[12];
                reg[12] = reg[13];
                reg[13] = reg[14] ^ j;
                reg[14] = reg[15];
                reg[15] = reg[16];
                reg[16] = reg[17] ^ j;
                reg[17] = reg[18];
                reg[18] = reg[19];
                reg[19] = reg[20];
                reg[20] = reg[21];
                reg[21] = reg[22];
                reg[22] = reg[23] ^ j;
                reg[23] = reg[24];
                reg[24] = reg[25];
                reg[25] = reg[26];
                reg[26] = reg[27];
                reg[27] = reg[28];
                reg[28] = reg[29];
                reg[29] = reg[30];
                reg[30] = reg[31];
                reg[31] = reg[32];
                reg[32] = reg[33];
                reg[33] = reg[34];
                reg[34] = reg[35];
                reg[35] = reg[36];
                reg[36] = reg[37] ^ j;
                reg[37] = reg[38];
                reg[38] = reg[39];
                reg[39] = j ^ !!(multi[i / 8] & 1 << (i % 8));
            }

            /* second step : error trapping and correction*/
            j = 0;
            for (i = 0; i < 28; i++)
                j += reg[i];
            k = j;
            for (i = 28; i < 40; i++)
                k += reg[i];
            if (!k)/*no error detected*/
            {
                p=1;//return SUCCESS;
                Out[0]=0;
                Out[1]=0;
                for (i=0; i<40; i++)
                {
                    //firedecode[i / 8] =firedecode[i / 8] | (reg[i]<<i);
                    Out[0]=Out[0] | (((UINT32)reg[i])<<i);
                }/**/
                return;
            }
            else if (!j)
            {
                for (i = 0; i < 12; i++)
                    s[i / 8] ^= reg[28 + i] << (i % 8);
                goto check;
            }

            for (i = 0; i < 212; i++)
            {
                if (reg[0])
                {
                    for (j = 0; j < 39; j++)
                        reg[j] = reg[j + 1] ^ poly[j];
                    reg[39] = poly[39];
                }
                else
                {
                    for (j = 0; j < 39; j++)
                        reg[j] = reg[j + 1];
                    reg[39] = 0;
                }
                k = 0;
                for (l = 0; l < 28; l++)
                    k +=reg[l];
                if (!k)
                {
                    //rem(data(1,i+1:i+12)+syndrome(29:40),2)
                    for (l = 0; l < 12; l++)
                        s[(i + l + 1) / 8] ^= reg[28 + l] << ((i + l + 1) % 8);
                    goto check;
                }
            }
            p=0;//return FAIL;

check:

            for (i = 0; i < 40; i++)
                reg[i] = 0;

            for (i = 0; i < 224; i++)
            {
                j = reg[0];
                reg[ 0] = reg[ 1];
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4];
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9];
                reg[ 9] = reg[10];
                reg[10] = reg[11];
                reg[11] = reg[12];
                reg[12] = reg[13];
                reg[13] = reg[14] ^ j;
                reg[14] = reg[15];
                reg[15] = reg[16];
                reg[16] = reg[17] ^ j;
                reg[17] = reg[18];
                reg[18] = reg[19];
                reg[19] = reg[20];
                reg[20] = reg[21];
                reg[21] = reg[22];
                reg[22] = reg[23] ^ j;
                reg[23] = reg[24];
                reg[24] = reg[25];
                reg[25] = reg[26];
                reg[26] = reg[27];
                reg[27] = reg[28];
                reg[28] = reg[29];
                reg[29] = reg[30];
                reg[30] = reg[31];
                reg[31] = reg[32];
                reg[32] = reg[33];
                reg[33] = reg[34];
                reg[34] = reg[35];
                reg[35] = reg[36];
                reg[36] = reg[37] ^ j;
                reg[37] = reg[38];
                reg[38] = reg[39];
                reg[39] = j ^ !!(s[i / 8] & 1 << (i % 8));
            }

            Out[0]=0;
            Out[1]=0;
            for (i=0; i<40; i++)
            {
                //firedecode[i / 8] =firedecode[i / 8] | (reg[i]<<i);
                Out[0]=Out[0] | (((UINT32)reg[i])<<i);
            }

        }
        break;

        case CS_HEAD_1:
        case CS_HEAD_2:
        case CS_HEAD_3:

            if(spal_CfgIsUL())
            {
                nb_bits = simu_spalConvCrcParam [coding_scheme].nbDecBits;
            }

            for (i = 0; i < nb_bits/3; i++)
            {
                j = reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = reg[3];
                reg[3] = reg[4] ;
                reg[4] = reg[5] ^ j;
                reg[5] = reg[6];
                reg[6] = reg[7];
                reg[7] = j ^ !!(in_8[i / 8] & 1 << (i % 8));
            }

            // residual should be all "1"
            if (reg[0] && reg[1] && reg[2] && reg[3] && reg[4] && reg[5]
                    && reg[6]&& reg[7])
            {
                *out_8 = 0; // ok
            }
            else
            {
                *out_8 = 1; // failed
            }

            break;

        case CS_MCS1:
        case CS_MCS2:
        case CS_MCS3:
        case CS_MCS4:
        case CS_MCS5:
        case CS_MCS6:
        case CS_MCS7:
        case CS_MCS8:
        case CS_MCS9:
            for (i = 0; i < nb_bits - 6; i++)
            {
                j = reg[0];
                reg[0] = reg[1] ^ j;
                reg[1] = reg[2] ^ j;
                reg[2] = reg[3];
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5];
                reg[5] = reg[6];
                reg[6] = reg[7] ^ j;
                reg[7] = reg[8] ^ j;
                reg[8] = reg[9];
                reg[9] = reg[10];
                reg[10] = reg[11];
                reg[11] = j ^ !!(in_8[i / 8] & 1 << (i % 8));
            }

            // residual should be all "1"
            if (reg[0] && reg[1] && reg[2] && reg[3] && reg[4] && reg[5]
                    && reg[6] && reg[7] && reg[8] && reg[9] && reg[10] && reg[11])
            {
                *out_8 = 0; // ok
            }
            else
            {
                *out_8 = 1; // failed
            }

            break;

        default:
            Out[0]=0;
            break;

// Not verified yet !!


    }
}


//=============================================================================
// spal_CapEncodeCrc
//-----------------------------------------------------------------------------
// Cap driver function to encode CRC
//
// @param coding_scheme SPAL_CODING_SCHEME_T.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PROTECTED VOID spal_CapEncodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{
    // Driver
//    UINT16 nb_bits = g_sppCsCrcParam[coding_scheme].nbEncBits;
//    UINT8 length = (nb_bits % 32)? 1+(nb_bits/32): (nb_bits/32);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, g_sppCsCrcParam[coding_scheme].CapCfg);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, nb_bits);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//    for (; length>0; length--)
//        SPP_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//    SPP_BB_CP2_START_CRC_E;
//    set_label(WAIT_Cap_CRC_E);
//    asm_wait(WAIT_Cap_CRC_E);
//    asm volatile("nop");
//    SPP_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB, Out[0]);
//    if (coding_scheme == CS_CS1)
//        SPP_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_MSB, Out[1]);

//    Uint16 CC_CrcE(Uint8* dividend, enum eCrcPoly ip, Uint8 r, Uint16 length, Uint8 n)

    // Software equivalent

    UINT8 reg[72]= {0};
    UINT8  j;
    UINT16 i,k=0;
    UINT16 dLngth[4] = {184,271,315,431};    /* information data length  */
    UINT8 crcLngth[4] = {40,16,16,16};    /* crc check length */
    UINT8  r;
    UINT16 length;
    UINT16 crcStart;

    UINT8* in_8 = (UINT8*) In;
    UINT16* out_16 = (UINT16*) Out;
    UINT16 reg_16[16] = {0};

    SPAL_CODING_SCHEME_T coding_scheme = param.capCfg;

    UINT8* firecode;
    firecode=(UINT8*)((Out));


    length   = param.nbEncBits;
    crcStart = 0;

    switch (coding_scheme)
    {
        case CS_CS1:

            for(i = 0; i < 184; i++)
            {

                j = !!(In[i / 32] & 1 << (i % 32));
                j ^= reg[0];

                for(k = 0; k < 13; k++)
                {
                    reg[k] = reg[k + 1];
                }

                reg[13] = reg[14] ^ j;
                reg[14] = reg[15];
                reg[15] = reg[16];
                reg[16] = reg[17] ^ j;

                for(k = 17; k < 22; k++)
                {
                    reg[k] = reg[k + 1];
                }

                reg[22] = reg[23] ^ j;

                for(k = 23; k < 36; k++)
                {
                    reg[k] = reg[k + 1];
                }

                reg[36] = reg[37] ^ j;
                reg[37] = reg[38];
                reg[38] = reg[39];
                reg[39] = j;
            }

            /*spal_memset((UINT8*) Out, 0, 2); /* clear firecode */
            spal_memset(firecode, 0, 5); /* clear firecode */

            for(i = 0; i < 40; i++)
            {
                firecode[i / 8] |= (~reg[i] & 1) << (i % 8);
            }

            /*for(i = 0; i < 72; i++)
            {
                Out[i / 32] |= (~reg[i] & 1) << (i % 32);
            }
            for (i=0; i<2; i++)
            {
                UINT32* ptrT;
                ptrT=(UINT32*)(&((firecode)[4*i]));
                Out[i]=ptrT[0];
            }*/
            break;


        case CS_HR:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = j;
            }
            r=1;
            Out[0]=0;
            for (i = 0; i < 3; i++) //n=3 represents the number of parity bits for HR
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=~(Out[0]);
            break;


        case CS_FR:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = j;
            }
            r=1;
            Out[0]=0;
            for (i = 0; i < 3; i++) //n=3 represents the number of parity bits for FR
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=~(Out[0]);
            break;


        case CS_EFR:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j;
                reg[5] = reg[6] ^ j;
                reg[6] = reg[7];
                reg[7] = j;
            }
            r=0;
            Out[0]=0;
            for (i = 0; i < 8; i++) //n=8 represents the number of parity bits for EFR:first CRC
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            break;

        case CS_AFS_RATSCCH:
        case CS_AFS_SID_UPDATE:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[ 0] = reg[ 1] ^ j;
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4];
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9] ^ j;
                reg[ 9] = reg[10];
                reg[10] = reg[11] ^ j;
                reg[11] = reg[12] ^ j;
                reg[12] = reg[13];
                reg[13] = j;
            }
            r=0;
            Out[0]=0;
            for (i = 0; i < 14; i++)
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=~(Out[0]);
            break;

        case CS_RACH:
        case CS_AFS_SPEECH_122:
        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_795:
        case CS_AFS_SPEECH_74:
        case CS_AFS_SPEECH_67:
        case CS_AFS_SPEECH_59:
        case CS_AFS_SPEECH_515:
        case CS_AFS_SPEECH_475:
        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
        case CS_AHS_SPEECH_515:
        case CS_AHS_SPEECH_475:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[0] = reg[1] ^ j;
                reg[1] = reg[2];
                reg[2] = reg[3] ^ j;
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j ;
                reg[5] = j;
            }
            r=1;
            Out[0]=0;
            for (i = 0; i < 6; i++) //n=6 represents the number of parity bits for RACH
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=(~(Out[0])) & 0x3f;
            break;


        case CS_SCH:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[0] = reg[1];
                reg[1] = reg[2] ^ j;
                reg[2] = reg[3];
                reg[3] = reg[4] ^ j;
                reg[4] = reg[5] ^ j;
                reg[5] = reg[6] ^ j;
                reg[6] = reg[7];
                reg[7] = reg[8] ^ j;
                reg[8] = reg[9];
                reg[9] = j;
            }
            r=1;
            Out[0]=0;
            for (i = 0; i < 6; i++) //n=6 represents the number of parity bits for RACH
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=~(Out[0]);
            break;


        case CS_CS2:
        case CS_CS3:
        case CS_CS4:
            for (i = 0; i < length; i++)
            {
                j = !!(In[i / 32] & 1 << (i % 32));
                j = j ^ reg[0];
                reg[ 0] = reg[ 1];
                reg[ 1] = reg[ 2];
                reg[ 2] = reg[ 3];
                reg[ 3] = reg[ 4] ^ j;
                reg[ 4] = reg[ 5];
                reg[ 5] = reg[ 6];
                reg[ 6] = reg[ 7];
                reg[ 7] = reg[ 8];
                reg[ 8] = reg[ 9];
                reg[ 9] = reg[10];
                reg[10] = reg[11] ^ j;
                reg[11] = reg[12];
                reg[12] = reg[13];
                reg[13] = reg[14];
                reg[14] = reg[15];
                reg[15] = j;
            }
            r=1;
            Out[0]=0;
            for (i = 0; i < 16; i++) //n=16 represents the number of parity bits for CS2->CS4
            {
                Out[0] |= (((UINT32)reg[i]) << i);
            }
            Out[0]=~(Out[0]);
            break;

        case CS_HEAD_1:
        case CS_HEAD_2:
        case CS_HEAD_3:

            if(!spal_CfgIsUL())
            {
                length   = simu_spalConvCrcParam [coding_scheme].nbEncBits;
                crcStart = 0;
            }

            for (i = crcStart; i < length; i++)
            {
                k = !!(in_8[i / 8] & 1 << (i % 8));
                k = k ^ reg_16[0];
                reg_16[0] = reg_16[1];
                reg_16[1] = reg_16[2] ^ k;
                reg_16[2] = reg_16[3];
                reg_16[3] = reg_16[4];
                reg_16[4] = reg_16[5] ^ k;
                reg_16[5] = reg_16[6];
                reg_16[6] = reg_16[7];
                reg_16[7] = k;
            }

            for (i = 0; i < 8; i++)
            {
                *out_16 |= (reg_16[i] << i);
            }

            *out_16 =  ~(*out_16);

            break;

        case CS_MCS1:
        case CS_MCS2:
        case CS_MCS3:
        case CS_MCS4:
        case CS_MCS5:
        case CS_MCS6:
        case CS_MCS7:
        case CS_MCS8:
        case CS_MCS9:

            if(!spal_CfgIsUL())
            {
                length   = simu_spalConvCrcParam [coding_scheme].nbEncBits;
                crcStart = 0;
            }
            for (i = crcStart; i < length; i++)
            {
                k = !!(in_8[i / 8] & 1 << (i % 8));
                k = k ^ reg_16[0];
                reg_16[0] = reg_16[1] ^ k;
                reg_16[1] = reg_16[2] ^ k;
                reg_16[2] = reg_16[3];
                reg_16[3] = reg_16[4] ^ k;
                reg_16[4] = reg_16[5];
                reg_16[5] = reg_16[6];
                reg_16[6] = reg_16[7] ^ k;
                reg_16[7] = reg_16[8] ^ k;
                reg_16[8] = reg_16[9];
                reg_16[9] = reg_16[10];
                reg_16[10] = reg_16[11];
                reg_16[11] =  k;
            }

            for (i = 0; i < 12; i++)
            {
                *out_16 |= (reg_16[i] << i);
            }
            *out_16 = ~(*out_16);

            break;

        default:
            Out[0]=0;
            break;
    }


}

//=============================================================================
// spal_CapConvoluate
//-----------------------------------------------------------------------------
// Cap driver function to convoluate ???
//
// @param coding_scheme SPAL_CODING_SCHEME_T
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PROTECTED VOID spal_CapConvoluate(SPAL_CAP_CONV_PARAM_T param, UINT32* Ptable, UINT32* In, UINT32* Out)
{
// Driver
//    UINT16 nb_bits = g_spalCsConvParam[coding_scheme].nbInBits;
//    UINT8 length = (nb_bits % 32)? 1+(nb_bits/32): (nb_bits/32);
//    UINT32 LRAM_addr = ((16 - length) & 0x1F) | BB_CP2_DATA_LRAM;
//    UINT32 i;
//    UINT32 *Ptable = (UINT32*)g_sppCsPuncturingTable[coding_scheme];
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, g_sppCsConvParam[coding_scheme].CapCfg);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, nb_bits);
//    // Eventual puncturing
//    if (CS_IS_PUNCTURED(g_sppCsConvParam[coding_scheme].CapCfg))
//    {
//        SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_LRAM_PUNC);
//        for (i=0; i<LENGTH_PUNCTURE_TABLE; i++)
//            SPP_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Ptable++);
//    }
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
//    for (i=0; i<length; i++)
//        SPP_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
//    SPP_BB_CP2_START_CONV;
//    asm volatile("nop");
//    set_label(WAIT_Cap_CONV);
//    asm_wait(WAIT_Cap_CONV);
//    asm volatile("nop");
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//    for (i=0; i<g_sppCsConvParam[coding_scheme].nbOutWords; i++)
//        SPP_BB_CP2_RD_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Out++);



//----------------------------------------------------------------------
    // Software equivalent
//----------------------------------------------------------------------

    UINT16 inLngth = param.nbInBits;
    SPAL_CODING_SCHEME_T coding_scheme = param.capCfg;

    UINT16    n;
    UINT8*    in;
    UINT8*    out;

    // intermediary unpunctured buffer able to contain the max convolution of 338*3 bits
    UINT8   pnctIn[230] = {0};
    UINT8   PreScalOut[40] = {0};

    UINT8 m[6][79];
    UINT8 g[3][79];

    UINT8* pG[6];

    UINT32* punc_table;
    UINT16  i, j, k;

    UINT32 i1,j1,k1=0;
    UINT8 Delete_i;        //decision switch
    UINT8 CPS;

    in=(UINT8*) In;
    out=(UINT8*) Out;

    CPS = *out;
    *out = 0;

    /*m[6][38] store 6 copies of "in", but one byte bigger.                       */
    /*the number "i" in m[i] presents how many bits the bit-sequence shift left.  */
    /*In other words, how many "0" are inserted at the beginning of               */
    /*the bit-sequence. At most, k=6, so one byte is enough for left shift.       */

    /*TCH/F14.4 sends 294 bits to conv. 294/8=36.75*/
    /*CS3 sends 338 bits to conv. 338/8=42.25*/

    /*g[6] are output of the generation polynomials.                     */
    /*pG[6] are 6 output (at most) of conv.*/

    j = inLngth / 8 + 1;

    if(coding_scheme >= CS_AFS_SPEECH_122 && coding_scheme <=CS_AHS_SPEECH_475)
    {
        //Prescaling
        spal_PreScal (coding_scheme, in, PreScalOut, inLngth);
        for (i = 0; i < j; i++)
        {
            m[0][i] = m[1][i] = m[2][i] = m[3][i] = m[4][i] = m[5][i] = PreScalOut[i];
        }

    }
    else
    {
        for (i = 0; i < j; i++)
        {
            m[0][i] = m[1][i] = m[2][i] = m[3][i] = m[4][i] = m[5][i] = in[i];
        }
    }

    /*shifting m1, m3 and m4 left 1, 3 and 4 bits, respectively.                  */
    spal_InsertBits(m[0],"\0", inLngth, 0, 1);
    spal_InsertBits(m[1],"\0", inLngth, 0, 2);
    spal_InsertBits(m[2],"\0", inLngth, 0, 3);
    spal_InsertBits(m[3],"\0", inLngth, 0, 4);
    spal_InsertBits(m[4],"\0", inLngth, 0, 5);
    spal_InsertBits(m[5],"\0", inLngth, 0, 6);



    /*pTCHFS:     G0 G1                                                           */
    /*pTCHHS:     G4 G5 G6                                                        */
    /*pTCHF48:    G1 G2 G3                                                        */
    /*pTCHAFS67:  G1 G2 G3 G3                                                     */
    /*pTCHAFS59:  G4 G5 G6 G6                                                     */
    /*pTCHAFS515: G1 G1 G2 G3 G3                                                  */
    /*pTCHAFS475: G4 G4 G5 G6 G6                                                  */
    /*pTCHF24:    G1 G2 G3 G1 G2 G3                                               */

    /*G0=1 +           D^3 + D^4                                                  */
    /*G1=1 + D +       D^3 + D^4                                                  */
    /*G2=1 +     D^2 +       D^4                                                  */
    /*G3=1 + D + D^2 + D^3 + D^4                                                  */
    /*G4=1 +     D^2 + D^3 +       D^5 + D^6                                      */
    /*G5=1 + D +             D^4 +       D^6                                      */
    /*G6=1 + D + D^2 + D^3 + D^4 +       D^6                                      */
    /*G7=1 + D + D^2 + D^3 +             D^6                                      */

    switch (coding_scheme)
    {
        case CS_CS1:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            //Puncturing
            spal_memcpy(out,pnctIn,57);


            break;
        case CS_CS2:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }


            //Puncturing
            j=0;  //j: the bit whose position in s1 is pPnctrASlctr[j] should be deleted.
            k=0;  //k: the position of output bit.
            for (i = 0; i < (inLngth*2); i++)
            {
                if (pnctrCS2[j] != i)   /*TRUE: the bit should be transmited.     */
                {
                    /*otherwise, it will be deleted.          */
                    out[k / 8] |= ((!!(pnctIn[i / 8] & 1 << (i % 8))) << (k % 8));
                    k++;
                }
                else
                    j++;
            }


            break;

        case CS_CS3:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            //Puncturing
            j=0;  //j: the bit whose position in s1 is pPnctrASlctr[j] should be deleted.
            k=0;  //k: the position of output bit.
            for (i = 0; i < (inLngth*2); i++)
            {
                if (pnctrCS3[j] != i)   /*TRUE: the bit should be transmited.     */
                {
                    /*otherwise, it will be deleted.          */
                    out[k / 8] |= ((!!(pnctIn[i / 8] & 1 << (i % 8))) << (k % 8));
                    k++;
                }
                else
                    j++;
            }

            break;

        case CS_FR:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, out, n * i + j);
                }
            }

            break;

        case CS_HR:
            for (i = 0; i < j; i++)
            {
                g[1][i] = in[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];      /*G5*/
                g[2][i] = g[1][i] ^ m[1][i] ^ m[2][i];              /*G6*/
                g[0][i] = g[2][i] ^ m[0][i] ^ m[3][i] ^ m[4][i];    /*G4*/
            }
            pG[0] = g[0];           /*output G4 G5 G6*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }



            //Punctruring HS

            spal_BufferClear(out,211);

            for(i1=0; i1<104*3; i1++)          // repeat 104*3 times
            {
                Delete_i=0;        // decision switch off
                // if i in the PUNCTURE_HS then set sign Delete_i
                for(j1=0; j1<95+6; j1++)   //95 CLASSI information bits number, 6:tail bits number
                {
                    if(i1==PUNCTURE_HR[j1])
                    {
                        Delete_i=1;    //decision switch on
                    }
                }

                /*
                * if i is not in the PUNCTURE_HS then translate i
                * else i will not be translated
                * then i=i+1 and repeat the operations above
                */
                if(!Delete_i)
                {
                    MOVEBIT(pnctIn, (i1), out, k1);
                    k1++;
                }

            }


            break;
        case CS_F96:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }


            j=0;
            for (i = 0; i < 488; i++)
            {
                k = 11 + 15 * j;
                if (i != k)
                    MOVEBIT(pnctIn, i, out, (i - j));
                else
                    j++;
            }


            break;
        case CS_F48:
            for (i = 0; i < j; i++)
            {
                g[1][i] = in[i] ^ m[1][i] ^ m[3][i];                /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];              /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G1 G2 G3*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, out, n * i + j);
                }
            }
            break;
        case CS_F144:
        case CS_AFS_SID_UPDATE:
        case CS_AFS_RATSCCH:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            j=0;
            for (i = 0; i < 588; i++)
            {
                k = 18 * (j / 4);
                if ((i != 1 + k) && (i != 6 + k) && (i != 11 + k) && (i != 15 + k)
                        && (i != 577) && (i != 582) && (i != 584) && (i != 587))
                    MOVEBIT(pnctIn, i, out, (i - j));
                else
                    j++;
            }
            break;

        case CS_AFS_SPEECH_122:
            for (i = 0; i < j; i++)
            {
                g[0][i] = PreScalOut[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            j = inLngth * n / 8 + 1;

            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);
            break;

        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_74:
            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[1][i] ^ m[3][i];                /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];              /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G1 G2 G3*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            j = inLngth * n / 8 + 1;

            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;
        case CS_AFS_SPEECH_795:
            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];      /*G5*/
                g[2][i] = g[1][i] ^ m[1][i] ^ m[2][i];              /*G6*/
                g[0][i] = g[2][i] ^ m[0][i] ^ m[3][i] ^ m[4][i];    /*G4*/
            }
            pG[0] = g[0];           /*output G4 G5 G6*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);
            break;

        case CS_AFS_SPEECH_67:
            // Convolutional Code
            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[1][i] ^ m[3][i];        /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];              /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                        /*G1*/
            }
            pG[0] = g[0];                                           /*output G1 G2 G3 G3*/
            pG[1] = g[1];
            pG[2] = g[2];
            pG[3] = pG[2];
            n=4;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i <j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;
        case CS_AFS_SPEECH_59:

            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];      /*G5*/
                g[2][i] = g[1][i] ^ m[1][i] ^ m[2][i];              /*G6*/
                g[0][i] = g[2][i] ^ m[0][i] ^ m[3][i] ^ m[4][i];    /*G4*/
            }
            pG[0] = g[0];           /*output G4 G5 G6 G6*/
            pG[1] = g[1];
            pG[2] = g[2];
            pG[3] = pG[2];
            n=4;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;

        case CS_AFS_SPEECH_515:

            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[1][i] ^ m[3][i];                /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];                      /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                                /*G1*/
            }
            pG[0] = g[0];           /*output G1 G1 G2 G3 G3*/
            pG[1] = pG[0];
            pG[2] = g[1];
            pG[3] = g[2];
            pG[4] = pG[3];
            n=5;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;
        case CS_AFS_SPEECH_475:

            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];      /*G5*/
                g[2][i] = g[1][i] ^ m[1][i] ^ m[2][i];                      /*G6*/
                g[0][i] = g[2][i] ^ m[0][i] ^ m[3][i] ^ m[4][i];            /*G4*/
            }

            pG[0] = g[0];           /*output G4 G4 G5 G6 G6*/
            pG[1] = pG[0];
            pG[2] = g[1];
            pG[3] = g[2];
            pG[4] = pG[3];
            n=5;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 448);

            //Puncturing    //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;

        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
            for (i = 0; i < j; i++)
            {
                g[0][i] = PreScalOut[i] ^ m[2][i] ^ m[3][i];                /*G0*/
                g[1][i] = g[0][i] ^ m[0][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G0 G1*/
            pG[1] = g[1];
            n=2;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 208);

            //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;
        case CS_AHS_SPEECH_515:
            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[1][i] ^ m[3][i];                /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];              /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G1 G2 G3*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 212);

            //Puncturing    //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);
            break;

        case CS_AHS_SPEECH_475:
            for (i = 0; i < j; i++)
            {
                g[1][i] = PreScalOut[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];      /*G5*/
                g[2][i] = g[1][i] ^ m[1][i] ^ m[2][i];              /*G6*/
                g[0][i] = g[2][i] ^ m[0][i] ^ m[3][i] ^ m[4][i];    /*G4*/
            }
            pG[0] = g[0];           /*output G4 G5 G6*/
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            /*set out to "0". outLngth/8+1.               */
            j = inLngth * n / 8 + 1;
            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            spal_BufferClear (out, 212);

            //Puncturing    //Puncturing
            punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
            spal_punct (pnctIn,out, punc_table,inLngth ,n);
            break;

        case CS_F24:
            for (i = 0; i < j; i++)
            {
                g[1][i] = in[i] ^ m[1][i] ^ m[3][i];                /*G2*/
                g[2][i] = g[1][i] ^ m[0][i] ^ m[2][i];              /*G3*/
                g[0][i] = g[2][i] ^ m[1][i];                        /*G1*/
            }
            pG[0] = g[0];           /*output G1 G2 G3 G1 G2 G3*/
            pG[1] = g[1];
            pG[2] = g[2];
            pG[3] = pG[0];
            pG[4] = pG[1];
            pG[5] = pG[2];
            n=6;

            /*set out to "0". outLngth/8+1.               */
            //j = inLngth * n / 8 + 1;
            for (i = 0; i < 60; i++)
            {
                out[i] = 0;
            }

            /*mix pG[] up to form output, "out".     */
            for (i = 0; i < inLngth; i++)
            {
                for (j = 0;  j < n; j++)
                {
                    MOVEBIT(pG[j], i, out, n * i + j);
                }
            }
            break;


        case CS_HEAD_1:
        case CS_HEAD_2:
        case CS_HEAD_3:
            punc_table =  (UINT32*) (g_sppCsPuncturingTable [coding_scheme] + 58);

            if(!spal_CfgIsUL())
            {
                inLngth = simu_spalConvCrcParam[coding_scheme].nbInBits;
                punc_table =  (UINT32*) g_sppCsPuncturingTable [coding_scheme];
                j = inLngth / 8 + 1;

                for (i = 0; i < j; i++)
                    m[0][i] = m[1][i] = m[2][i] = m[3][i] = m[4][i] = m[5][i] = in[i];

                /*shifting m1, m3 and m4 left 1, 3 and 4 bits, respectively.                  */
                spal_InsertBits(m[0],"\0", inLngth, 0, 1);
                spal_InsertBits(m[1],"\0", inLngth, 0, 2);
                spal_InsertBits(m[2],"\0", inLngth, 0, 3);
                spal_InsertBits(m[3],"\0", inLngth, 0, 4);
                spal_InsertBits(m[4],"\0", inLngth, 0, 5);
                spal_InsertBits(m[5],"\0", inLngth, 0, 6);
            }

            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[1][i] ^ m[2][i] ^ m[4][i] ^ m[5][i];      // G4
                g[1][i] = in[i] ^ m[0][i] ^ m[1][i] ^ m[2][i] ^ m[5][i];      // G7
                g[2][i] = in[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];                // G5
            }

            pG[0] = g[0];
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            // set out to "0". outLngth/8+1.
            j = inLngth * n / 8 + 1;

            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            for (i = 6; i < inLngth; i++)
            {
                for (j = 0; j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * (i - 6) + j);
                }
            }


            // Puncturing.
            if(coding_scheme == CS_HEAD_2) // If MCS5, MCS6
            {
                for (i = 0; i < (inLngth-6)*3; i++)
                {
                    MOVEBIT(pnctIn, i, out, i);
                }

                MOVEBIT(pnctIn, (inLngth-6)*3 - 1 , out, (inLngth-6)*3);

            }
            else
            {
                spal_punct (pnctIn, out, punc_table, (UINT16)(inLngth - 6), n);
            }

            break;

        case CS_MCS1:
        case CS_MCS2:
        case CS_MCS3:
        case CS_MCS4:
        case CS_MCS5:
        case CS_MCS6:
        case CS_MCS7:
        case CS_MCS8:
        case CS_MCS9:
            for (i = 0; i < j; i++)
            {
                g[0][i] = in[i] ^ m[1][i] ^ m[2][i] ^ m[4][i] ^ m[5][i];      // G4
                g[1][i] = in[i] ^ m[0][i] ^ m[1][i] ^ m[2][i] ^ m[5][i];      // G7
                g[2][i] = in[i] ^ m[0][i] ^ m[3][i] ^ m[5][i];                // G5
            }

            pG[0] = g[0];
            pG[1] = g[1];
            pG[2] = g[2];
            n=3;

            // set out to "0". outLngth/8+1.
            j = inLngth * n / 8 + 1;

            for (i = 0; i < j; i++)
            {
                pnctIn[i] = 0;
            }

            for (i = 0; i < inLngth; i++)
            {
                for (j = 0; j < n; j++)
                {
                    MOVEBIT(pG[j], i, pnctIn, n * i + j);
                }
            }

            // Puncturing
            punc_table = (UINT32*)(g_sppCsPuncturingTable [coding_scheme] + 58 * (CPS-1)) ;
            spal_punct (pnctIn,out, punc_table,inLngth ,n);

            break;

        default:
////////////////////////////////////////////////////////////////
// TODO CHECK THAT
            //exit(1);
            break;
    }




//void CC_PunctrureHFS(UINT8 *pHFS_MID,UINT8 *pOUT)
//{
//    UINT32 i,j,k=0;
//    UINT8 Delete_i;        /* decision switch */
//
//    CC_BufferClear(pOUT,211);
//
//    for(i=0;i<TCH_HS_PRE_CONVLUTIONAL_BITS*3;i++)            /* repeat 104*3 times */
//    {
//        Delete_i=0;        /* /* decision switch off */
//        /* if i in the PUNCTURE_HS then set sign Delete_i */
//        for(j=0;j<TCH_HS_CLASSI_BITS+TCH_HS_TAIL_BITS;j++)
//        {
//            if(i==PUNCTURE_HS[j])
//            {
//                Delete_i=1;    /* decision switch on */
//            }
//        }
//
//        /*
//         * if i is not in the PUNCTURE_HS then translate i
//         * else i will not be translated
//         * then i=i+1 and repeat the operations above
//         */
//        if(!Delete_i)
//        {
//                MOVEBIT(pHFS_MID, i, pOUT, k);
//                k++;
//        }
//
//    }


}

// TODO:This is the same function as spp_CapConvoluate
//      Just uses other tables
//=============================================================================
// spal_CapConvSrc
//-----------------------------------------------------------------------------
// Post SRC (Systematic Recursive Convolutional)
//
// @param coding_scheme SPAL_CODING_SCHEME_T.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PROTECTED VOID spal_CapConvSrc(SPAL_CAP_CONV_PARAM_T param, UINT32* In, UINT32* Out)
{
    // Driver
//    UINT8  tab_idx = coding_scheme - CS_NEED_SRC; // table offset
//    UINT16 nb_bits = g_sppCsSrcParam[tab_idx].nbInBits;
//    UINT8 length = (nb_bits % 32)? 1+(nb_bits/32): (nb_bits/32);
//    UINT32 LRAM_addr = ((16 - length) & 0x1F) | BB_CP2_DATA_LRAM;
//    UINT32 i;
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, g_sppCsSrcParam[tab_idx].CapCfg); fr=14;
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, nb_bits);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
//    for (i=0; i<length; i++)
//        SPP_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
//    SPP_BB_CP2_START_CONV;
//    asm volatile("nop");
//    set_label(WAIT_Cap_SRCCONV);
//    asm_wait(WAIT_Cap_SRCCONV);
//    asm volatile("nop");
//    SPP_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
//    for (i=0; i<g_sppCsSrcParam[tab_idx].nbOutWords; i++)
//        SPP_BB_CP2_RD_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Out++);


    UINT16 i, j;
    SPAL_CODING_SCHEME_T coding_scheme = param.capCfg;
    UINT16 inLngth = param.nbInBits;

    UINT8* in=(UINT8*) In;
    UINT8* out=(UINT8*) Out;


    UINT8 reg[7] = {0, 0, 0, 0, 0, 0, 0};

    j = inLngth / 8 + 1;

    for (i = 0; i < j; i++)
    {
        out[i] = 0;
    }

    switch (coding_scheme)
    {
        case CS_AFS_RATSCCH:
            break;
        case CS_AFS_SPEECH_122:
        case CS_AHS_SPEECH_795:
        case CS_AHS_SPEECH_74:
        case CS_AHS_SPEECH_67:
        case CS_AHS_SPEECH_59:
            for (i = 0; i < inLngth; i++)
            {
                reg[4] = !!(in[i / 8] & 1 << (i % 8));
                out[i / 8]|= (reg[4] ^ reg[1] ^ reg[0])<<(i%8);
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
            }
            break;
        case CS_AFS_SPEECH_795:
        case CS_AHS_SPEECH_475:
            for (i = 0; i < inLngth; i++)
            {
                reg[6] = !!(in[i / 8] & 1 << (i % 8));
                out[i / 8]|= (reg[6] ^ reg[4] ^ reg[3] ^ reg[1] ^ reg[0])<<(i%8);
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
                reg[4] = reg[5];
                reg[5] = reg[6];
            }
            break;
        case CS_AFS_SPEECH_475:
        case CS_AFS_SPEECH_59:
            for (i = 0; i < inLngth; i++)
            {
                reg[6] = !!(in[i / 8] & 1 << (i % 8));
                out[i / 8]|= (reg[6] ^ reg[5] ^ reg[4] ^ reg[3] ^ reg[2] ^ reg[0])<<(i%8);
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
                reg[4] = reg[5];
                reg[5] = reg[6];
            }
            break;
        case CS_AFS_SPEECH_102:
        case CS_AFS_SPEECH_74:
        case CS_AFS_SPEECH_515:
        case CS_AFS_SPEECH_67:
        case CS_AHS_SPEECH_515:
            for (i = 0; i < inLngth; i++)
            {
                reg[4] = !!(in[i / 8] & 1 << (i % 8));
                out[i / 8]|= (reg[4] ^ reg[3] ^ reg[2] ^ reg[1] ^ reg[0])<<(i%8);
                reg[0] = reg[1];
                reg[1] = reg[2];
                reg[2] = reg[3];
                reg[3] = reg[4];
            }
            break;
        default:
            break;
    }


}

//=============================================================================
// spal_CapFireCorrection
//-----------------------------------------------------------------------------
// Cap driver function for FIRE code decoding ?
//
// @param Out UINT32*. Pointer to the output buffer.
//
// @return UINT8.
//
//=============================================================================
PROTECTED UINT8 spal_CapFireCorrection(UINT32* Out)
{
#ifdef FIRE_CORRECTION
    UINT32 error=0;
    UINT32 position=0;
    UINT32 status=0;
    SPP_BB_CP2_START_FIRE_TRAP;
    asm volatile("nop");
    set_label(CRC_ERROR_TRAP);
    asm_wait(CRC_ERROR_TRAP);
    asm volatile("nop");
    SPP_BB_CP2_RD_CTRL_REG(BB_CP2_STATUS,status);
    SPP_BB_CP2_RD_CTRL_REG(BB_CP2_BIT_NUMBER,position);
    if (position>11)
    {
        UINT32 idx=0;
        UINT32 rest=0;
        SPP_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB,error);
        position=224-position+12;
        idx=position>>5;
        rest=position-(idx<<5);
        if (rest<12)
        {
            Out[idx]^=error>>(12-rest);
            Out[idx-1]^=error<<(rest+20);
        }
        else
        {
            Out[idx]^=error<<(rest-12);
        }
        return  0;
    }
    else
#endif
        return 1;
}




















































































































































