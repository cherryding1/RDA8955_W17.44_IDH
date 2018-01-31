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

#include "chip_id.h"

#define CONVERT_TO_STRING(x) #x


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

//LRAM_Data
#define BB_CP2_LRAM_DATA(n)         (((n)&0xFFFFFFFF)<<0)




// changing xml generated defines
#undef BB_CP2_ENABLE_PUNCTURING
#undef BB_CP2_LRAM_DATA
#undef BB_CP2_BIT_NUMBER

#define BB_CP2_ENABLE_PUNCTURING(n) (((n)&1)<<24)

/// BB_CP2 address mapping
#define BB_CP2_CTRL                              0
#define BB_CP2_BIT_NUMBER                        1
#define BB_CP2_STATUS                            2
#define BB_CP2_LRAM_ADDR                         3
#define BB_CP2_CRC_CODE_LSB                      4
#define BB_CP2_CRC_CODE_MSB                      5
#define BB_CP2_LRAM_DATA                         0
#define BB_CP2_LRAM_PUNC                         (0<<5)
#define BB_CP2_DATA_LRAM                         (1<<5)




// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPP_FCCH_RESULT_T
// -----------------------------------------------------------------------------
/// FCCH Result
// =============================================================================
typedef struct
{
    INT16                          FOf;                          //0x00000000
    INT16                          TOf;                          //0x00000002
} SPP_FCCH_RESULT_T; //Size : 0x4


// ============================================================================
// SPP_FCCH_PARAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    INT32                          Acc_Energy;                   //0x00000000
    INT32                          Acc_Corr_I;                   //0x00000004
    INT32                          Acc_Corr_Q;                   //0x00000008
    UINT32                         Max_Abs_Corr;                 //0x0000000C
    INT32                          Corr_order_I;                 //0x00000010
    INT32                          Corr_order_Q;                 //0x00000014
    INT16                          samples_left;                 //0x00000018
    INT16                          max_index;                    //0x0000001A
    UINT8                          FCCH_detected;                //0x0000001C
    UINT8                          counter_high;                 //0x0000001D
    UINT8                          counter_low;                  //0x0000001E
    UINT8                          lastIT;                       //0x0000001F
    UINT8                          debug;                        //0x00000020
} SPP_FCCH_PARAM_T; //Size : 0x24

extern BOOL spp_FcchDetectAsm_v2(UINT32* Ptr_old,
                                 UINT32* Ptr_new,
                                 SPP_FCCH_PARAM_T* Params,
                                 SPP_FCCH_RESULT_T* Res);


UINT32       __attribute__((section(".sramdata")))        val;



//added by lisc,2013_01_15
/*******************************************************/

PUBLIC  INT32 spal_LRAMTest_internal_Er(UINT32 *In, UINT32 *Out, UINT32 cmp_value, UINT32 ram_select, UINT32 length)
{

    UINT32 i;
    //UINT32 temp_int=8;
    UINT32 *local_out_pt;

    local_out_pt = Out;

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, ram_select);
    for (i=0; i<length; i++)
    {
        SPAL_BB_CP2_WR_GNRL_REG_MEM(BB_CP2_LRAM_DATA, In++);
    }

    SPAL_BB_CP2_WR_CTRL_REG(BB_CP2_LRAM_ADDR, ram_select);
    for (i=0; i<length; i++)
    {
        SPAL_BB_CP2_RD_GNRL_REG_MEM(BB_CP2_LRAM_DATA, Out++);
    }

    for (i=0; i<length; i++)
    {
        if (local_out_pt[i] != cmp_value)
            return 0;
    }

    return 1;

}

PUBLIC INT32 spal_LRAMTest_Er()
{
    UINT32 i;
    UINT32 len;
    UINT32 ret;
    UINT32 input[32], output[32];
    UINT32 test_value;
    val =0xff;

    /******* test1 :DATA_LRAM , length:16, test data=0x00000000**********/
    len = 16;
    test_value = 0;
    for (i=0; i<32; i++)
    {
        input[i] = test_value;
        output[i] = 0x0bad0bad;
    }

    ret = spal_LRAMTest_internal_Er(input, output,test_value,BB_CP2_DATA_LRAM,len);

    if(ret == 0)
    {
        val = 0x1;
        while(1);
    }

    /******* test2 :DATA_LRAM , length:16, test data=0xffffffff **********/
    len = 16;
    test_value = 0xffffffff;
    for (i=0; i<32; i++)
    {
        input[i] = test_value;
        output[i] = 0x0bad0bad;
    }

    ret = spal_LRAMTest_internal_Er(input, output,test_value,BB_CP2_DATA_LRAM,len);

    if(ret == 0)
    {
        val = 0x2;
        while(1);
    }


    /******* test3 :PUNT_LRAM , length:32, test data=0x00000000**********/
    len = 32;
    test_value = 0;
    for (i=0; i<32; i++)
    {
        input[i] = test_value;
        output[i] = 0x0bad0bad;
    }

    ret = spal_LRAMTest_internal_Er(input, output,test_value,0,len);

    if(ret == 0)
    {
        val = 0x3;
        while(1);
    }

    /******* test4 :PUNT_LRAM , length:32, test data=0xffffffff **********/
    len = 32;
    test_value = 0xffffffff;
    for (i=0; i<32; i++)
    {
        input[i] = test_value;
        output[i] = 0x0bad0bad;
    }

    ret = spal_LRAMTest_internal_Er(input, output,test_value,0,len);

    if(ret == 0)
    {
        val = 0x4;
        while(1);
    }

    val = 0xfe;
    return val;
}



#if (CHIP_HAS_BCPU_ROM == 0)
#define BCPU_MAIN_LOCATION
#else
#define BCPU_MAIN_LOCATION __attribute__((section(".bcpu_main_entry")))
#endif


/******************************************************/
/// **********************************************
/// BCPU entry point
/// This function should NOT be ROM'ed
/// **********************************************/
VOID bcpu_main2(VOID)
{
    // Check that Xcpu did initiate the handshake

    //added by lisc,2013_01_16
    /***********************************/
    val=0xF0;
    spal_LRAMTest_Er();
    if(val==0xFe)
        *(volatile UINT32 *)(0xa1b0001c)=0xAAAAAAAA;
    else
        *(volatile UINT32 *)(0xa1b0001c)=0xA9;


    /***********************************/

    //while (1); //Masked By Yangbiao 20140122
}

