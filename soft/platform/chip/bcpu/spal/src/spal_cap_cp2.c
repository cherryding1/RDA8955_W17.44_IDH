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



#if (!CHIP_CAP_ON_APB)

#include "cs_types.h"
#include "spal_cap.h"
#include "spalp_private.h"
#include "bb_cp2.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

// ----------------------------------
// FOR CP2 IMPLEMENTATION OF CAP ONLY
// ----------------------------------

// CP2 ACCESSES
// macro for converting a constant to a string
#define CONVERT_TO_STRING(x) #x

// control register -> GPR
#define SPAL_BB_CP2_RD_CTRL_REG(regaddr, n)       asm volatile("cfc2 %0, $" CONVERT_TO_STRING(regaddr) :"=r"((n)))
//              GPR -> control register
#define SPAL_BB_CP2_WR_CTRL_REG(regaddr, n)       asm volatile("ctc2 %0, $" CONVERT_TO_STRING(regaddr) ::"r"((n)))

// general register -> GPR
// #define SPAL_BB_CP2_RD_GNRL_REG_GPR(regaddr, n)   asm volatile("mfc2 %0, $" CONVERT_TO_STRING(regaddr) :"=r"((n)))
//              GPR -> general register
// #define SPAL_BB_CP2_WR_GNRL_REG_GPR(regaddr, n)   asm volatile("mtc2 %0, $" CONVERT_TO_STRING(regaddr) ::"r"((n)))

// general register -> memory
#define SPAL_BB_CP2_RD_GNRL_REG_MEM(regaddr, out) asm volatile("swc2 $" CONVERT_TO_STRING(regaddr) ", 0(%0)"::"r"((out)))
//           memory -> general register
#define SPAL_BB_CP2_WR_GNRL_REG_MEM(regaddr, in)  asm volatile("lwc2 $" CONVERT_TO_STRING(regaddr) ", 0(%0)"::"r"((in)))

// Start convolutional encoding and puncturing encoding
#define SPAL_BB_CP2_START_CONV    asm volatile("cop2 0x1FFFFFF")  // 25 bits command

// Start CRC or Fire code parity bits encoding
#define SPAL_BB_CP2_START_CRC_E   asm volatile("cop2 0x1FFFFFE")  // 25 bits command

// Start CRC or Fire code parity bits decoding
#define SPAL_BB_CP2_START_CRC_D   asm volatile("cop2 0x1FFFFFD")  // 25 bits command
#define SPAL_BB_CP2_START_FIRE_TRAP asm volatile("cop2 0x1FFFFFC")    // 25 bits command// Length of the puncturing table
#define LENGTH_PUNCTURE_TABLE   22

// LRAM address of writing puncturing tables
#define LRAM_ADDR_PUNC          0

// ASM labels for CP2
#define set_label(labelname) asm volatile(#labelname ":")
#define asm_wait(labelname)  asm volatile("bc2f " #labelname)

// used to check if coding scheme is CS1 without passing an extra parameter
#define IS_CS_CS1(x) ((x) == 224)

// length of words to copy a bit buffer
//#define BIT2WORDLEN(x) (((x)+31)/32);
#define BIT2WORDLEN(x) ((x) % 32)? 1+((x)/32): ((x)/32);

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_CapDecodeCrc
//-----------------------------------------------------------------------------
// BBCP2 driver function to decode CRC
//
// @param param SPAL_CAP_CRC_PARAM_T. Cap module configuration for crc.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapDecodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{
    UINT8 length = BIT2WORDLEN(param.nbDecBits);

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, param.capCfg);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, param.nbDecBits);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    for (; length>0; length--)
    {
        SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
    }

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    asm volatile("nop");
    SPAL_BB_CP2_START_CRC_D;

    asm volatile("nop");
    set_label(WAIT_BBCP2_CRC_D);
    asm_wait(WAIT_BBCP2_CRC_D);
    asm volatile("nop");

    SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB, Out[0]);
    if (IS_CS_CS1(param.nbDecBits))
    {
        SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_MSB, Out[1]);
    }
}



//=============================================================================
// spal_CapEncodeCrc
//-----------------------------------------------------------------------------
// BBCP2 driver function to encode CRC
//
// @param param SPAL_CAP_CRC_PARAM_T. Cap module configuration for crc.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapEncodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out)
{
    UINT8 length = BIT2WORDLEN(param.nbEncBits);

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, param.capCfg);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, param.nbEncBits);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    for (; length>0; length--)
    {
        SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
    }

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    asm volatile("nop");
    SPAL_BB_CP2_START_CRC_E;

    asm volatile("nop");
    set_label(WAIT_BBCP2_CRC_E);
    asm_wait(WAIT_BBCP2_CRC_E);
    asm volatile("nop");
    SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB, Out[0]);
    if (IS_CS_CS1(param.nbDecBits))
    {
        SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_MSB, Out[1]);
    }
}


PUBLIC VOID spal_CapConvoluate(SPAL_CAP_CONV_PARAM_T param, UINT32* punctTable, UINT32* In, UINT32* Out)
{
    UINT8 length = BIT2WORDLEN(param.nbInBits);
    UINT32 LRAM_addr = ((16 - length) & 0x1F) | BB_CP2_DATA_LRAM;
    UINT32 i;

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, param.capCfg);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, param.nbInBits);
    // Eventual puncturing
    if (CS_IS_PUNCTURED(param.capCfg))
    {
        SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_LRAM_PUNC);
        for (i=0; i<LENGTH_PUNCTURE_TABLE; i++)
        {
            SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, punctTable++);
        }
    }
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
    for (i=0; i<length; i++)
    {
        SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
    }
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
    asm volatile("nop");

    SPAL_BB_CP2_START_CONV;

    asm volatile("nop");
    set_label(WAIT_BBCP2_CONV);
    asm_wait(WAIT_BBCP2_CONV);
    asm volatile("nop");
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    for (i=0; i<param.nbOutWords; i++)
        SPAL_BB_CP2_RD_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Out++);

}

// TODO:This is the same function as spal_CapConvoluate
//      Just uses other tables
//=============================================================================
// spal_CapConvSrc
//-----------------------------------------------------------------------------
// BBCP2 driver function to convoluate ???
//
// @param param SPAL_CAP_CONV_PARAM_T. Cap module configuration for convolution.
// @param In UINT32*. Pointer to the input buffer.
// @param Out UINT32*. Pointer to the output buffer.
//
//=============================================================================
PUBLIC VOID spal_CapConvSrc(SPAL_CAP_CONV_PARAM_T param, UINT32* In, UINT32* Out)
{
    // UINT8  tab_idx = coding_scheme - CS_NEED_SRC; // table offset
    UINT8 length = BIT2WORDLEN(param.nbInBits);
    UINT32 LRAM_addr = ((16 - length) & 0x1F) | BB_CP2_DATA_LRAM;
    UINT32 i;

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_CTRL, param.capCfg);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_BIT_NUMBER, param.nbInBits);
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
    for (i=0; i<length; i++)
    {
        SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
    }
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, LRAM_addr);
    asm volatile("nop");

    SPAL_BB_CP2_START_CONV;

    asm volatile("nop");
    set_label(WAIT_BBCP2_SRCCONV);
    asm_wait(WAIT_BBCP2_SRCCONV);
    asm volatile("nop");
    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, BB_CP2_DATA_LRAM);
    for (i=0; i<param.nbOutWords; i++)
    {
        SPAL_BB_CP2_RD_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Out++);
    }

}


//=============================================================================
// spal_CapFireCorrection
//-----------------------------------------------------------------------------
// BBCP2 driver function for FIRE code decoding ?
//
// @param Out UINT32*. Pointer to the output buffer.
//
// @return UINT8.
//
//=============================================================================
PUBLIC UINT8 spal_CapFireCorrection(UINT32* Out)
{
    UINT32 error=0;
    UINT32 position=0;
    UINT32 status=0;
    SPAL_BB_CP2_START_FIRE_TRAP;
    asm volatile("nop");
    set_label(CRC_ERROR_TRAP);
    asm_wait(CRC_ERROR_TRAP);
    asm volatile("nop");
    SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_STATUS,status);
    SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_BIT_NUMBER,position);
    if (position>11)
    {
        UINT32 idx=0;
        UINT32 rest=0;
        SPAL_BB_CP2_RD_CTRL_REG(BB_CP2_CRC_CODE_LSB,error);
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
    {
        return 1;
    }
}

#endif //  (!CHIP_CAP_ON_APB)

