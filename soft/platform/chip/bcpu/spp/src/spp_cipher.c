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
#include "spc_ctx.h"
#include "spal_cipher.h"
#include "spp_gsm.h"
#include "spc_private.h"
#include "spc_mailbox.h"
#include "spal_mem.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
UINT8  (*g_sppA53CountInit)(UINT32 count) SPAL_BBSRAM_GLOBAL;
VOID  (*g_sppCipherCipherA53Nb)(UINT32 *BufferIn, UINT32 *BufferOut) SPAL_BBSRAM_GLOBAL;
VOID  (*g_sppCipherDecipherA53Nb)(UINT32 *BufferIn, UINT32 *BufferOut) SPAL_BBSRAM_GLOBAL;
#endif
//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
VOID spp_CipherA53Open(UINT8 (*CountInit) (UINT32 count),
                       VOID (*Cipher) (UINT32 *BufferIn, UINT32 *BufferOut),
                       VOID (*Decipher) (UINT32 *BufferIn, UINT32 *BufferOut))
{
    g_sppA53CountInit = CountInit;
    g_sppCipherCipherA53Nb = Cipher;
    g_sppCipherDecipherA53Nb = Decipher;
}
#endif
//======================================================================
// Initialize the count register
//----------------------------------------------------------------------
// This function initializes the count register.
//
// @param count UINT32. Counter register.
//======================================================================
PUBLIC UINT8 spp_CipherCountInit (UINT32 count)
{
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    if((g_mailbox.pal2spc.statWin.ciphMode == SPC_A51) ||
            (g_mailbox.pal2spc.statWin.ciphMode == SPC_A52) )
#endif
        return spal_CipherCountInit(count);
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    else
        return g_sppA53CountInit(count);
#endif
}

//======================================================================
// spp_CipherStart
//----------------------------------------------------------------------
// Start the encipherment.
//
// @param algo UINT8.
//
// @return UINT8. Error or no error??
//======================================================================
PUBLIC UINT8 spp_CipherStart(UINT8 algo)
{
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    if((g_mailbox.pal2spc.statWin.ciphMode == SPC_A51) ||
            (g_mailbox.pal2spc.statWin.ciphMode == SPC_A52) )
#endif
        return spal_CipherStart(algo);
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    else
        return spal_CipherA53Start();
#endif
}

//======================================================================
// spp_CipherCipherNB
//----------------------------------------------------------------------
// This function cipher 114 bits for normal burst.
//
// @param BufferIn UINT32*. Pointer to input buffer.
// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spp_CipherCipherNb(UINT32* BufferIn, UINT32* BufferOut)
{
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    if((g_mailbox.pal2spc.statWin.ciphMode == SPC_A51) ||
            (g_mailbox.pal2spc.statWin.ciphMode == SPC_A52) )
#endif
        spal_CipherCipherNb(BufferIn, BufferOut);
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    else
        g_sppCipherCipherA53Nb(BufferIn, BufferOut);
#endif
}

//======================================================================
// spp_CipherDecipherNB
//----------------------------------------------------------------------
// This function deciphers 114 bits for normal burst.
//
// @param BufferIn UINT32*. Pointer to input buffer.
// @param BufferOut UINT32*. Pointer to output buffer.
//
//======================================================================
PUBLIC VOID spp_CipherDecipherNb(UINT32* BufferIn, UINT32* BufferOut)
{
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    if((g_mailbox.pal2spc.statWin.ciphMode == SPC_A51) ||
            (g_mailbox.pal2spc.statWin.ciphMode == SPC_A52) )
#endif
        spal_CipherDecipherNb(BufferIn, BufferOut);
#if (CHIP_CIPHER_A53_SUPPORTED == 1)
    else
        g_sppCipherDecipherA53Nb(BufferIn, BufferOut);
#endif
}
