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
#include "global_macros.h"
#include "spal_cipher.h"
#include "cipher.h"
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
#include "cipher_a53.h"
#endif
//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//======================================================================
// Initialize the count register
//----------------------------------------------------------------------
/// This function initializes the count register.
///
/// @param count UINT32. Counter register.
//======================================================================
PUBLIC UINT8 spal_CipherCountInit (UINT32 count)
{
    UINT32 error;

    // Check if the cipher A5 is already processing
    error = hwp_cipher->status & CIPHER_RUNNING;
    if (error)
    {
        return cipher_err_running;
    }
    else
    {
        hwp_cipher->Count = count;
        return cipher_err_none;
    }
}

//======================================================================
// spal_CipherStart
//----------------------------------------------------------------------
/// Start the encipherment.
///
/// @param algo UINT8.
///
/// @return UINT8. Error or no error??
//======================================================================
PUBLIC UINT8 spal_CipherStart(UINT8 algo)
{
    UINT32 error;

    // Check if the cipher A5 is already processing
    error = hwp_cipher->status & CIPHER_RUNNING;
    if (error)
    {
        return cipher_err_running;
    }
    else
    {
        if (algo == CIPHER_A51) // A5/1
        {
            hwp_cipher->ctrl = CIPHER_ENABLE | CIPHER_ALGORITHM_A5_1;
        }
        else
        {
            if (algo == CIPHER_A52) // A5/2
            {
                hwp_cipher->ctrl = CIPHER_ENABLE | CIPHER_ALGORITHM_A5_2;
            }

        }
        return cipher_err_none;
    }
}


//======================================================================
// spal_CipherCipherNB
//----------------------------------------------------------------------
/// This function cipher 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherCipherNb(UINT32 *BufferIn, UINT32 *BufferOut)
{
    register UINT8 i;
    UINT32 CipherStream[4];

    // Wait for the ciphering to finish
    while(hwp_cipher->status & CIPHER_RUNNING);
    // Get data block 1
    CipherStream[0] = hwp_cipher->Data0_block2;
    CipherStream[1] = hwp_cipher->Data1_block2;
    CipherStream[2] = hwp_cipher->Data2_block2;
    CipherStream[3] = hwp_cipher->Data3_block2;
    // !! the BufferIn and BufferOut COULD be the same one !!
    for (i = 0; i < 4; i++)
        BufferOut[i] = BufferIn[i] ^ CipherStream[i];
}

//======================================================================
// spal_CipherDecipherNB
//----------------------------------------------------------------------
/// This function deciphers 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherDecipherNb(UINT32 *BufferIn, UINT32 *BufferOut)
{
    register UINT8 i;
    while(hwp_cipher->status & CIPHER_RUNNING);

    //hwp_cipher-> ctrl = CIPHER_SOFTBIT8 ;
    for (i = 0; i < 29; i++)
        BufferOut[i] = BufferIn[i] ^ (hwp_cipher->Data_block1[i].Data_block1);
}

// //======================================================================
// // spal_CipherRunning
// //----------------------------------------------------------------------
// // This functions tests whether Cipher is running or not.
// //
// // @return BOOL. 0: cipher ready. 1: cipher running.
// //======================================================================
// BOOL spal_CipherRunning()
// {
//     UINT32 blk_ready;
//     blk_ready = hwp_cipher->status & CIPHER_DATA_BLK_RDY;
//     if (blk_ready)
//         return 0;
//     else
//         return 1;
// }


// // GLTODO This function is NEVER called in the code!
// /* Get the results of Block 2*/
// //======================================================================
// // spal_CipherGetBlk2
// //----------------------------------------------------------------------
// // This functions gets the results of Block 2 ciphering.
// //
// // @param data_blk2 UINT32*. Pointer to the block2 data.
// //======================================================================
// VOID spal_CipherGetBlk2(UINT32* data_blk2)
// {
//     data_blk2[0] = hwp_cipher->Data0_block2;
//     data_blk2[1] = hwp_cipher->Data1_block2;
//     data_blk2[2] = hwp_cipher->Data2_block2;
//     data_blk2[3] = hwp_cipher->Data3_block2;
// }


// // GLTODO This function is NEVER called in the code!
// /* Get the results of Block 1*/
// //======================================================================
// // spal_CipherGetBlk1
// //----------------------------------------------------------------------
// // This functions gets the results of Block 1 ciphering.
// //
// // @param data_blk1 UINT32*. Pointer to the block1 data.
// //======================================================================
// VOID spal_CipherGetBlk1(UINT32* data_blk1)
// {
//     UINT8 i ;
//     for (i = 0; i < 29; i++)
//         data_blk1[i] = (hwp_cipher->Data_block1[i].Data_block1);
// }

#if (CHIP_CIPHER_A53_SUPPORTED == 1)

//======================================================================
// Initialize the count register
//----------------------------------------------------------------------
/// This function initializes the count register.
///
/// @param count UINT32. Counter register.
//======================================================================
PUBLIC UINT8 spal_CipherA53CountInit (UINT32 count)
{
    UINT32 error;

    int t1 = (count >> 11)&0x3ff;
    int t3 = (count >> 5)&0x3f;
    int t2 = count&0x1f;

    // Check if the cipher A5 is already processing
    error = hwp_cipherA53->CIPH_STAT & CIPHER_A53_INIT;
    if (error)
    {
        return cipher_err_running;
    }
    else
    {

        hwp_cipherA53->Key_Data3 =CIPHER_A53_SFN_10_5(t1);
        hwp_cipherA53->Key_Data4 =CIPHER_A53_T26N(t2)|CIPHER_A53_T51N(t3)|CIPHER_A53_SFN_4_0(t1);
        return cipher_err_none;
    }
}


//======================================================================
// spal_CipherA53Start
//----------------------------------------------------------------------
/// Start the encipherment.
///
/// @param algo UINT8.
///
/// @return UINT8. Error or no error??
//======================================================================
PUBLIC UINT8 spal_CipherA53Start()
{
    UINT32 error;

    // Check if the cipher A5 is already processing
    error = hwp_cipherA53->CIPH_STAT & CIPHER_A53_INIT;
    if (error)
    {
        return cipher_err_running;
    }
    else
    {
        hwp_cipherA53->CIPH_STAT = CIPHER_A53_INIT | CIPHER_A53_CACT | CIPHER_A53_A53;
        return cipher_err_none;
    }
}

//======================================================================
// spal_CipherCipherA53Nb
//----------------------------------------------------------------------
/// This function cipher 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherCipherA53Nb(UINT32 *BufferIn, UINT32 *BufferOut)
{
    register UINT32 i;
    UINT32 *CipherStream;

    // Wait for the ciphering to finish
    while(hwp_cipherA53->CIPH_STAT);
    // Get data block 1
    CipherStream = (UINT32 *)hwp_cipherA53->block2;
    // !! the BufferIn and BufferOut COULD be the same one !!
    for (i = 0; i < 4; i++)
        BufferOut[i] = BufferIn[i] ^ CipherStream[i];
}

//======================================================================
// spal_CipherDecipherA53Nb
//----------------------------------------------------------------------
/// This function deciphers 114 bits for normal burst.
///
/// @param BufferIn UINT32*. Pointer to input buffer.
/// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spal_CipherDecipherA53Nb(UINT32 *BufferIn, UINT32 *BufferOut)
{
    register UINT32 i,j;
    UINT8 *pSv,*pHv;
    UINT32 DeCipherCode;
    pSv = (UINT8 *)&DeCipherCode;
    // Wait for the ciphering to finish
    while(hwp_cipherA53->CIPH_STAT);
    pHv = (UINT8*)hwp_cipherA53->block1;
    for (i = 0; i < 29; i++)
    {
        for(j=0; j<4; j++)
        {
            pSv[j] = (((pHv[(i*4+j)/8]>>((i*4+j)%8))&1)<<7);
        }
        BufferOut[i] = BufferIn[i] ^ DeCipherCode;
    }
}

#endif
