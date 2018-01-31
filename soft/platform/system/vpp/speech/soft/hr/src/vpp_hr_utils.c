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







#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "hal_speech.h"
#include "vpp_hr_host.h"
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_mathhalf_macro.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



#ifdef VAX
#   define OPEN_WI  "wb","mrs=512","rfm=fix","ctx=stm"
#   define OPEN_RI  "rb","mrs=512","rfm=fix","ctx=stm"
#   define OPEN_WB  "wb","mrs=512","rfm=fix","ctx=stm"
#   define OPEN_RB  "rb","mrs=2","rfm=fix","ctx=stm"
#   define OPEN_WT  "w","mrs=256","rat=cr","rfm=var"
#   define OPEN_RT  "r","mrs=256","rat=cr","rfm=var"
#else
#   define OPEN_WB  "wb"
#   define OPEN_RB  "rb"
#   define OPEN_WI  "wb"
#   define OPEN_RI  "rb"
#   define OPEN_WT  "wt"
#   define OPEN_RT  "rt"
#endif

#define LW_SIGN (long)0x80000000       /* sign bit */
#define LW_MIN (long)0x80000000
#define LW_MAX (long)0x7fffffff
#define SW_MIN (short)0x8000           /* smallest Ram */
#define SW_MAX (short)0x7fff           /* largest Ram */

typedef char  Byte;
//typedef long int INT32;             /* 32 bit "accumulator" (L_*) */
//typedef short int INT16;           /* 16 bit "register"  (sw*) */

/*___________________________________________________________________________
 |                                                                           |
 |      local Functions                                                      |
 |___________________________________________________________________________|
*/
//static INT32 error_free (UINT16* EncoderIn, UINT16* EncoderOut)//( FILE *infile, FILE *outfile);
static void SwapBytes( INT16 buffer[], INT32 len );
static INT32 ByteOrder( void );

static INT16 swSidDetection(INT16 pswParameters[],
                            INT16 pswErrorFlag[]);
static void RandomParameters(INT16 pswParameters[]);
static INT16 getPnBits(INT16 iBits, INT32 *pL_PNSeed);

//INT32 L_shr(INT32 L_var1, INT16 var2);   /* 2 ops */
//INT32 L_shl(INT32 L_var1, INT16 var2);   /* 2 ops */
//INT16 shr(INT16 var1, INT16 var2);     /* 1 ops */
//INT16 shl(INT16 var1, INT16 var2);     /* 1 ops */

/*___________________________________________________________________________
 |                                                                           |
 |      Subroutines                                                          |
 |___________________________________________________________________________|
*/
INT32 error_free (UINT16* EncoderIn, UINT16* EncoderOut)//( FILE *infile, FILE *outfile)
{
#define SPEECH      1
#define CNIFIRSTSID 2
#define CNICONT     3
#define VALIDSID    11
#define GOODSPEECH  33

    static INT16 swDecoMode = {SPEECH};
    static INT16 swTAFCnt = {1};
    INT16 swInPara[20], i, swFrameType;
    INT16 swOutPara[22],pswErrorFlag[3];

    //if( EncoderInterface( infile, swInPara )) return( 1 );
    for (i=0; i<20; i++)
        swInPara[i]=EncoderIn[i];

    SwapBytes( swInPara, 18 );

    /* Copy input parameters to output parameters (error free transmission) */
    /* -------------------------------------------------------------------- */
    for (i=0; i<18; i++)
        swOutPara[i] = swInPara[i];

    /* Set channel status flags (error free transmission) */
    /* -------------------------------------------------- */
    swOutPara[18] = 0;     /* BFI flag */
    swOutPara[19] = 0;     /* UFI flag */

    /* Evaluate SID flag */
    /* ----------------- */
    pswErrorFlag[0] = 0;   /* BFI flag */
    pswErrorFlag[1] = 0;   /* UFI flag */
    pswErrorFlag[2] = 0;   /* BCI flag */
    swOutPara[20] = swSidDetection(swOutPara, pswErrorFlag);


    /* Evaluate TAF flag */
    /* ----------------- */
    if (swTAFCnt == 0) swOutPara[21] = 1;
    else               swOutPara[21] = 0;
    swTAFCnt = (swTAFCnt + 1) % 12;


    /* Frame classification:                                                */
    /* Since the transmission is error free, the received frames are either */
    /* valid speech or valid SID frames                                     */
    /* -------------------------------------------------------------------- */
    if      ( swOutPara[20] == 2) swFrameType = VALIDSID;
    else if ( swOutPara[20] == 0) swFrameType = GOODSPEECH;
    else
    {
        //printf( "Error in SID detection\n" );
        return( 1 );
    }

    /* Update of decoder state */
    /* ----------------------- */
    if (swDecoMode == SPEECH)
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNIFIRSTSID;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }
    else    /* comfort noise insertion mode */
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNICONT;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }


    /* Replace parameters by random data if in CNICONT-mode and TAF=0 */
    /* -------------------------------------------------------------- */
    if ((swDecoMode == CNICONT) && (swOutPara[21] == 0))
    {
        RandomParameters(swOutPara);
        /* Set flags such, that an "unusable frame" is produced */
        swOutPara[18] = 1;     /* BFI flag */
        swOutPara[19] = 1;     /* UFI flag */
        swOutPara[20] = 0;     /* SID flag */
    }

    SwapBytes( swOutPara, 18 );
    for (i=0; i<22; i++)
        EncoderOut[i]=swOutPara[i];

    /*if( outfile )
    {
        if( WriteOutputFile( swOutPara, outfile )) {
            printf( "Error writing File\n" );
            return( 1 );
        }
    }*/
    return( 0 );
}


static void SwapBytes( INT16 buffer[], INT32 len )
{
    Byte *pc, tmp;
    INT32 i;

    if( !ByteOrder())
        return;
    pc = (Byte *)buffer;
    for( i = 0; i < len; i++ )
    {
        tmp   = pc[0];
        pc[0] = pc[1];
        pc[1] = tmp;
        pc += 2;
    }
}

static INT32 ByteOrder( void )
{
    INT16 si;
    Byte *pc;

    si = 0x1234;
    pc = (Byte *)&si;
    if (pc[1] == 0x12 && pc[0] == 0x34 )
        return( 0 );
    if (pc[0] == 0x12 && pc[1] == 0x34 )
        return( 1 );
    //printf( "Error in ByteOrder: %X, %X\n", (int)pc[0], (int)pc[1] );
    //exit( 1 );
    return( 2 );
}


/*************************************************************************
 *
 *   FUNCTION NAME: getPnBits
 *
 *   PURPOSE:
 *
 *     Generate iBits pseudo-random bits using *pL_PNSeed as the
 *     pn-generators seed.
 *
 *   INPUTS:
 *
 *     iBits - integer indicating how many random bits to return.
 *     range [0,15], 0 yields 1 bit output
 *
 *     *pL_PNSeed - 32 bit seed (changed by function)
 *
 *   OUTPUTS:
 *
 *     *pL_PNSeed - 32 bit seed, modified.
 *
 *   RETURN VALUE:
 *
 *    random bits in iBits LSB's.
 *
 *
 *   IMPLEMENTATION:
 *
 *    implementation of x**31 + x**3 + 1 == PN_XOR_REG | PN_XOR_ADD a
 *    PN sequence generator using INT32s generating a 2**31 -1
 *    length pn-sequence.
 *
 *************************************************************************/

static INT16 getPnBits(INT16 iBits, INT32 *pL_PNSeed)
{

#define PN_XOR_REG (INT32)0x00000005L
#define PN_XOR_ADD (INT32)0x40000000L

    INT16 swPnBits=0;
    INT32 L_Taps,L_FeedBack;
    INT16 i;

    for (i=0; i < iBits; i++)
    {

        /* update the state */
        /********************/

        L_Taps = *pL_PNSeed & PN_XOR_REG;
        L_FeedBack = L_Taps; /* Xor tap bits to yield feedback bit */
        //L_Taps = L_shr(L_Taps,1);
        L_Taps = SHR(L_Taps,1);

        while(L_Taps)
        {
            L_FeedBack = L_FeedBack ^ L_Taps;
            //L_Taps = L_shr(L_Taps,1);
            L_Taps = SHR(L_Taps,1);

        }

        /* LSB of L_FeedBack is next MSB of PN register */

        //*pL_PNSeed = L_shr(*pL_PNSeed,1);
        *pL_PNSeed = SHR(*pL_PNSeed,1);
        if (L_FeedBack & 1)
            *pL_PNSeed = *pL_PNSeed | PN_XOR_ADD;

        /* State update complete.
           Get the output bit from the state, add/or it into output */

        swPnBits = SHL(swPnBits,1);
        swPnBits = swPnBits | (*pL_PNSeed & 1);

    }
    return(swPnBits);
}



/*___________________________________________________________________________
 |                                                                           |
 |     This subroutine calculates the 'SID flag'                             |
 |                                                                           |
 |     Input:     pswParameters[18]                                          |
 |                           input parameters of the speech decoder          |
 |                                                                           |
 |                pswErrorFlag[3]                                            |
 |                           error flags, generated by channel decoder       |
 |                                                                           |
 |     Return Value:                                                         |
 |                0:         speech frame detected                           |
 |                1:         most likely SID frame received                  |
 |                2:         SID frame detected                              |
 |                                                                           |
 |___________________________________________________________________________|
 |                                                                           |
 |     History:                                                              |
 |                                                                           |
 |     12-Oct-1994: Bug removed: error corrected in case of a mode (unvoiced/|
 |                  voiced) mismatch, if a SID frame was received as an      |
 |                  unvoiced frame                                           |
 |___________________________________________________________________________|
*/

static INT16 swSidDetection(INT16 pswParameters[],
                            INT16 pswErrorFlag[])
{
    static INT16 ppswIBit[2][18] =
    {
        {5, 11,9,8, 1, 2, 7,7,5, 7,7,5, 7,7,5, 7,7,5},  /* unvoiced */
        {5, 11,9,8, 1, 2, 8,9,5, 4,9,5, 4,9,5, 4,9,5}
    }; /* voiced */

    static INT16 ppswCL1pCL2[2][18] =
    {
        {
            0x0001, /* R0      */  /* unvoiced */
            0x00ef, /* LPC1    */
            0x003e, /* LPC2    */
            0x007f, /* LPC3    */
            0x0001, /* INT LPC */
            0x0003, /* Mode    */
            0x001f, /* Code1_1 */
            0x0072, /* Code2_1 */
            0x0012, /* GSP0_1  */
            0x003f, /* Code1_2 */
            0x007f, /* Code2_2 */
            0x0008, /* GSP0_2  */
            0x007f, /* Code1_3 */
            0x007f, /* Code2_3 */
            0x0008, /* GSP0_3  */
            0x007f, /* Code1_4 */
            0x007f, /* Code2_4 */
            0x000c
        }, /* GSP0_4  */

        {
            0x0000, /* R0      */  /* voiced */
            0x0000, /* LPC1    */
            0x0000, /* LPC2    */
            0x0000, /* LPC3    */
            0x0001, /* INT LPC */
            0x0003, /* Mode    */
            0x00ff, /* Lag_1   */
            0x01ff, /* Code_1  */
            0x001f, /* GSP0_1  */
            0x000f, /* Lag_2   */
            0x01ff, /* Code_2  */
            0x001f, /* GSP0_2  */
            0x000f, /* Lag_3   */
            0x01ff, /* Code_3  */
            0x001f, /* GSP0_3  */
            0x000f, /* Lag_4   */
            0x01ff, /* Code_4  */
            0x001f
        }
    }; /* GSP0_4 */

    static INT16 ppswCL2[2][18] =
    {
        {
            0x0000, /* R0      */ /* unvoiced */
            0x0000, /* LPC1    */
            0x0000, /* LPC2    */
            0x0000, /* LPC3    */
            0x0000, /* INT LPC */
            0x0000, /* Mode    */
            0x0000, /* Code1_1 */
            0x0000, /* Code2_1 */
            0x0000, /* GSP0_1  */
            0x0000, /* Code1_2 */
            0x0000, /* Code2_2 */
            0x0000, /* GSP0_2  */
            0x0000, /* Code1_3 */
            0x0007, /* Code2_3 */  /* 3 bits */
            0x0000, /* GSP0_3  */
            0x007f, /* Code1_4 */  /* 7 bits */
            0x007f, /* Code2_4 */  /* 7 bits */
            0x0000
        }, /* GSP0_4  */

        {
            0x0000, /* R0      */  /* voiced */
            0x0000, /* LPC1    */
            0x0000, /* LPC2    */
            0x0000, /* LPC3    */
            0x0000, /* INT LPC */
            0x0000, /* Mode    */
            0x0000, /* Lag_1   */
            0x0000, /* Code_1  */
            0x0000, /* GSP0_1  */
            0x0000, /* Lag_2   */
            0x0000, /* Code_2  */
            0x0000, /* GSP0_2  */
            0x0000, /* Lag_3   */
            0x00ff, /* Code_3  */  /* 8 bits */
            0x0000, /* GSP0_3  */
            0x0000, /* Lag_4   */
            0x01ff, /* Code_4  */  /* 9 bits */
            0x0000
        }
    }; /* GSP0_4 */

    static int first = 1;

    INT16 swMode, swBitMask;
    INT16 swSidN1, swSidN2, swSidN1pN2;
    INT16 swSid ;

    short siI, siII;


    if (first)
    {
        /* Force Sid codewords to be represented */
        /* internally in PC byte order           */
        /* ------------------------------------- */

        SwapBytes(ppswCL1pCL2[0], 18);
        SwapBytes(ppswCL1pCL2[1], 18);
        SwapBytes(ppswCL2[0], 18);
        SwapBytes(ppswCL2[1], 18);

        first = 0;
    }


    /* count transmission errors within the SID codeword      */
    /* count number of bits equal '0' within the SID codeword */
    /* ------------------------------------------------------ */

    if (pswParameters[5] == 0)
        swMode = 0;
    else
        swMode = 1;


    swSidN1pN2 = 0;         /* N1 + N2 */
    swSidN2    = 0;
    swSidN1    = 0;

    for (siI = 0; siI < 18; siI++)
    {
        swBitMask = 0x0001;
        SwapBytes(&swBitMask, 1);  /* force swBitMask to PC byte order */
        for (siII = 0; siII < ppswIBit[swMode][siI]; siII++)
        {
            if ( (pswParameters[siI] & swBitMask) == 0 )
            {
                if ( (ppswCL1pCL2[swMode][siI] & swBitMask) != 0 ) swSidN1pN2++;
                if ( (ppswCL2[swMode][siI] & swBitMask)     != 0 ) swSidN2++;
            }
            SwapBytes(&swBitMask, 1);  /* return swBitMask to native byte order */
            swBitMask = swBitMask << 1;
            SwapBytes(&swBitMask, 1);  /* force swBitMask to PC byte order */
        }
    }

    swSidN1 = swSidN1pN2 - swSidN2;


    /* frame classification */
    /* -------------------- */

    if (pswErrorFlag[2])
    {

        if (swSidN1 < 3)
            swSid = 2;
        else if (swSidN1pN2 < 16)
            swSid = 1;
        else
            swSid = 0;

        if ( (swSidN1pN2 >= 16) && (swSidN1pN2 <= 25) )
        {
            pswErrorFlag[0] = 1;
        }

    }
    else
    {

        if (swSidN1 < 3)
            swSid = 2;
        else if (swSidN1pN2 < 11)
            swSid = 1;
        else
            swSid = 0;

    }


    /* in case of a mode mismatch */
    /*----------------------------*/

    if ( (swSid == 2) && (swMode == 0) ) swSid = 1;

    return(swSid);

}


/*___________________________________________________________________________
 |                                                                           |
 |     This subroutine sets the 18 speech parameters to random values        |
 |                                                                           |
 |     Input:     pswParameters[18]                                          |
 |                           input parameters of the speech decoder          |
 |                                                                           |
 |___________________________________________________________________________|
*/

static void RandomParameters(INT16 pswParameters[])
{
    static INT16 ppswIBit[2][18] =
    {
        {5, 11,9,8, 1, 2, 7,7,5, 7,7,5, 7,7,5, 7,7,5},  /* unvoiced */
        {5, 11,9,8, 1, 2, 8,9,5, 4,9,5, 4,9,5, 4,9,5}
    }; /* voiced */

    static INT32 L_PNSeed=(INT32)0x1091988L;
    INT16  i,ind;

    /* Determine mode bit */
    /* ------------------ */
    pswParameters[5] = getPnBits(2, &L_PNSeed);

    /* Switch bit allocation accordingly */
    /* --------------------------------- */
    ind = 0;
    if (pswParameters[5] > 0) ind = 1;

    for (i=0; i < 5; i++)
    {
        pswParameters[i] = getPnBits(ppswIBit[ind][i], &L_PNSeed);
    }
    for (i=6; i < 18; i++)
    {
        pswParameters[i] = getPnBits(ppswIBit[ind][i], &L_PNSeed);
    }

    /* force random parameters to PC byte order */
    /* ---------------------------------------- */

    SwapBytes(pswParameters, 18);
}














INT32 error_free_initial(HAL_SPEECH_ENC_OUT_T EdIfaceIn, HAL_SPEECH_DEC_IN_T* EdIfaceOutput)
//INT32 error_free_initial (UINT16* EncoderIn, UINT16* EncoderOut)
{
#define SPEECH      1
#define CNIFIRSTSID 2
#define CNICONT     3
#define VALIDSID    11
#define GOODSPEECH  33

    static INT16 swDecoMode = {SPEECH};
    static INT16 swTAFCnt = {1};
    INT16 swInPara[20], i, swFrameType;//, j
    INT16 swOutPara[22],pswErrorFlag[3];

    VPP_HR_PROFILE_FUNCTION_ENTER(error_free_initial );

    //FILE * fg1 = fopen("reidcase3_out.tab","a+");


    for  (i = 0; i < 20; ++i)
    {
        swInPara[i]=0;
    }

    dePackEnc(&(EdIfaceIn.encOutBuf[0]), &swInPara[0]);
    swInPara[18]=EdIfaceIn.echoSkipEncFrame ;
    swInPara[19]=EdIfaceIn.sp ;/**/









    //if( EncoderInterface( infile, swInPara )) return( 1 );
    /*for (i=0; i<20; i++)
        swInPara[i]=EncoderIn[i];*///Newcomment

    SwapBytes( swInPara, 18 );

    /* Copy input parameters to output parameters (error free transmission) */
    /* -------------------------------------------------------------------- */
    for (i=0; i<18; i++)
        swOutPara[i] = swInPara[i];

    /* Set channel status flags (error free transmission) */
    /* -------------------------------------------------- */
    swOutPara[18] = 0;  //EdIfaceOutput->bfi=0;   /* BFI flag */
    swOutPara[19] = 0;  //EdIfaceOutput->ufi=0;   /* UFI flag */

    /* Evaluate SID flag */
    /* ----------------- */
    pswErrorFlag[0] = 0;   /* BFI flag */
    pswErrorFlag[1] = 0;   /* UFI flag */
    pswErrorFlag[2] = 0;   /* BCI flag */
    swOutPara[20] = swSidDetection(swOutPara, pswErrorFlag);




    /* Evaluate TAF flag */
    /* ----------------- */
    if (swTAFCnt == 0) swOutPara[21] = 1; //EdIfaceOutput->taf=1;
    else               swOutPara[21] = 0; //EdIfaceOutput->taf=0;
    swTAFCnt = (swTAFCnt + 1) % 12;


    /* Frame classification:                                                */
    /* Since the transmission is error free, the received frames are either */
    /* valid speech or valid SID frames                                     */
    /* -------------------------------------------------------------------- */
    if      ( swOutPara[20] == 2) swFrameType = VALIDSID;
    else if ( swOutPara[20] == 0) swFrameType = GOODSPEECH;
    else
    {
        VPP_HR_PROFILE_FUNCTION_EXIT(error_free_initial );
        //printf( "Error in SID detection\n" );
        return( 1 );
    }

    /* Update of decoder state */
    /* ----------------------- */
    if (swDecoMode == SPEECH)
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNIFIRSTSID;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }
    else    /* comfort noise insertion mode */
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNICONT;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }


    /* Replace parameters by random data if in CNICONT-mode and TAF=0 */
    /* -------------------------------------------------------------- */
    if ((swDecoMode == CNICONT) && (swOutPara[21] == 0))
    {
        RandomParameters(swOutPara);
        /* Set flags such, that an "unusable frame" is produced */
        swOutPara[18] = 1;     /* BFI flag */
        swOutPara[19] = 1;     /* UFI flag */
        swOutPara[20] = 0;     /* SID flag */
    }


    SwapBytes( swOutPara, 18 );

    /*for (i=0; i<22; i++)
      EncoderOut[i]=swOutPara[i];*/


    unPackEnc(&swOutPara[0], &(EdIfaceOutput->decInBuf[0]) );

    EdIfaceOutput->bfi=swOutPara[18];
    EdIfaceOutput->ufi=swOutPara[19];
    EdIfaceOutput->sid=swOutPara[20];
    EdIfaceOutput->taf=swOutPara[21];/**/





    VPP_HR_PROFILE_FUNCTION_EXIT(error_free_initial );


    return( 0 );
}












INT32 error_free_interface (UINT16* EncoderIn, UINT16* EncoderOut)
{
#define SPEECH      1
#define CNIFIRSTSID 2
#define CNICONT     3
#define VALIDSID    11
#define GOODSPEECH  33

    static INT16 swDecoMode = {SPEECH};
    static INT16 swTAFCnt = {1};
    INT16 swInPara[20], i, swFrameType;//, j
    INT16 swOutPara[22],pswErrorFlag[3];

    VPP_HR_PROFILE_FUNCTION_ENTER(error_free_initial );

    //FILE * fg1 = fopen("reidcase3_out.tab","a+");




    //if( EncoderInterface( infile, swInPara )) return( 1 );
    for (i=0; i<20; i++)
        swInPara[i]=EncoderIn[i];

    SwapBytes( swInPara, 18 );

    /* Copy input parameters to output parameters (error free transmission) */
    /* -------------------------------------------------------------------- */
    for (i=0; i<18; i++)
        swOutPara[i] = swInPara[i];

    /* Set channel status flags (error free transmission) */
    /* -------------------------------------------------- */
    swOutPara[18] = 0;  //EdIfaceOutput->bfi=0;   /* BFI flag */
    swOutPara[19] = 0;  //EdIfaceOutput->ufi=0;   /* UFI flag */

    /* Evaluate SID flag */
    /* ----------------- */
    pswErrorFlag[0] = 0;   /* BFI flag */
    pswErrorFlag[1] = 0;   /* UFI flag */
    pswErrorFlag[2] = 0;   /* BCI flag */
    swOutPara[20] = swSidDetection(swOutPara, pswErrorFlag);




    /* Evaluate TAF flag */
    /* ----------------- */
    if (swTAFCnt == 0) swOutPara[21] = 1; //EdIfaceOutput->taf=1;
    else               swOutPara[21] = 0; //EdIfaceOutput->taf=0;
    swTAFCnt = (swTAFCnt + 1) % 12;


    /* Frame classification:                                                */
    /* Since the transmission is error free, the received frames are either */
    /* valid speech or valid SID frames                                     */
    /* -------------------------------------------------------------------- */
    if      ( swOutPara[20] == 2) swFrameType = VALIDSID;
    else if ( swOutPara[20] == 0) swFrameType = GOODSPEECH;
    else
    {
        VPP_HR_PROFILE_FUNCTION_EXIT(error_free_initial );
        //printf( "Error in SID detection\n" );
        return( 1 );
    }

    /* Update of decoder state */
    /* ----------------------- */
    if (swDecoMode == SPEECH)
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNIFIRSTSID;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }
    else    /* comfort noise insertion mode */
    {
        if      (swFrameType == VALIDSID)   swDecoMode = CNICONT;
        else if (swFrameType == GOODSPEECH) swDecoMode = SPEECH;
    }


    /* Replace parameters by random data if in CNICONT-mode and TAF=0 */
    /* -------------------------------------------------------------- */
    if ((swDecoMode == CNICONT) && (swOutPara[21] == 0))
    {
        RandomParameters(swOutPara);
        /* Set flags such, that an "unusable frame" is produced */
        swOutPara[18] = 1;     /* BFI flag */
        swOutPara[19] = 1;     /* UFI flag */
        swOutPara[20] = 0;     /* SID flag */
    }


    SwapBytes( swOutPara, 18 );

    for (i=0; i<22; i++)
        EncoderOut[i]=swOutPara[i];/**/







    VPP_HR_PROFILE_FUNCTION_EXIT(error_free_initial );


    return( 0 );
}

