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







#include <stdio.h>
#include "vpp_hr_typedefs.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*static void writeBit(INT16 *tableCompressedOut, INT16 *pBit, INT16 NumberToCompress, INT16 NBBITS, INT16 *Idx)
{


    INT16 j=(*Idx), tmp;

    if (  NBBITS <= (16-(*pBit))  )
    {
        tmp=(*pBit);
        (tableCompressedOut[j])|=(NumberToCompress<<(*pBit));
        (*pBit)=((*pBit)+NBBITS)%16;
        if ( (*pBit)<=tmp )
            (*Idx)++;
    }
    else
    {
        tmp=16-(*pBit);
        (tableCompressedOut[j])|=(NumberToCompress<<((*pBit)));
        (*pBit)=0;
        (*Idx)++;

        NumberToCompress = SHRW(NumberToCompress,tmp);
        writeBit(tableCompressedOut, pBit, NumberToCompress   , NBBITS-tmp, Idx);
    }

}*/
/***************************************************************************
 *
 *   FUNCTION NAME: DePackEnc
 *
 *   PURPOSE:
 *
 *     Depack encoder output for decoder input
 *
 *   INPUTS:
 *     In - encoder output buffer
 *
 *   OUTPUTS:
 *
 *     Out[0:17] - an array into which the coded data is moved
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   REFERENCES: Sub-clause 2.1 and 4.1.12 of GSM Recomendation 06.20
 *
 **************************************************************************/
/*void dePackEnc(UINT16* In, UINT16* Out)
{

        Out[0]= ( In[0] & 0x001f)                                     ; //RO  :  5 bits
        Out[1]= ((In[0] & 0xffe0)>>5)                                 ; //LPC1: 11 bits
        Out[2]= ( In[1] & 0x01ff)                                     ; //LPC2:  9 bits
        Out[3]= ((In[1] & 0xfe00)>>9) | ((EdIfaceIn[2] & 0x0001)<<7)  ; //LPC3:  8 bits=7+1
        Out[4]= ( In[2] & 0x0002)>>1                                  ; //INT_LPC
        Out[5]= ( In[2] & 0x000c)>>2                                  ; //MODE

    if (In[5])
    {
        Out[6] = (In[2] & 0x0ff0)>>4                                 ; //LAG_1
        Out[7] = (In[2] & 0xf000)>>12) | ((EdIfaceIn[3] & 0x001f)<<4); //CODE_1
        Out[8] = (In[3] & 0x03e0)>>5                                 ; //GSPO_1
        Out[9] = (In[3] & 0x3c00)>>10                                ; //LAG_2
        Out[10]= (In[3] & 0xc000)>>14) | ((EdIfaceIn[4] & 0x007f)<<2); //CODE_2
        Out[11]= (In[4] & 0x0f80)>>7                                 ; //GSPO_2
        Out[12]= (In[4] & 0xf000)>>12                                ; //LAG_3
        Out[13]= (In[5] & 0x01ff)                                    ; //CODE_3
        Out[14]= (In[5] & 0x3e00)>>9                                 ; //GSPO_3
        Out[15]= (In[5] & 0xc000)>>14) | ((EdIfaceIn[6] & 0x0003)<<2); //LAG_4
        Out[16]= (In[5] & 0x07fc)>>2                                 ; //CODE_4
        Out[17]= (In[5] & 0xf800)>>11                                ; //GSPO_4
    }
    else
    {
       Out[6] = ( In[2] & 0x07f0)>>4                                 ; //CODE1_1
       Out[7] = ((In[2] & 0xf800)>>11) | ((EdIfaceIn[3] & 0x0003)<<5); //CODE2_1
       Out[8] = ( In[3] & 0x007c)>>2                                 ; //GSPO_1
       Out[9] = ( In[3] & 0x3f80)>>7                                 ; //CODE1_2
       Out[10]= ((In[3] & 0xc000)>>14) | ((EdIfaceIn[4] & 0x001f)<<2); //CODE2_2
       Out[11]= ( In[4] & 0x03e0)>>5                                 ; //GSPO_2
       Out[12]= ((In[4] & 0xfc00)>>10) | ((EdIfaceIn[5] & 0x0001 )   ; //CODE1_3
       Out[13]= ( In[5] & 0x00fe)>>1                                 ; //CODE2_3
       Out[14]= ( In[5] & 0x1f00)>>8                                 ; //GSPO_3
       Out[15]= ((In[5] & 0xe000)>>13) | ((EdIfaceIn[6] & 0x000f)<<3 ; //CODE1_4
       Out[16]= ( In[6] & 0x07f0)>>4                                 ; //CODE2_4
       Out[17]= ( In[6] & 0xf800)>>11                                ; //GSPO_4
    }

}*/

/***************************************************************************
 *
 *   FUNCTION NAME: fillBitAlloc
 *
 *   PURPOSE:
 *
 *     Arrange speech parameters for encoder output
 *
 *   INPUTS:
 *
 *     The speechcoders codewords:
 *     iR0 - Frame energy
 *     piVqIndeces[0:2] - LPC vector quantizer codewords
 *     iSoftInterp - Soft interpolation bit 1 or 0
 *     iVoicing - voicing mode 0,1,2,3
 *     piLags[0:3] - Frame and delta lag codewords
 *     piCodeWrdsA[0:3] - VSELP codevector 1
 *     piCodeWrdsB[0:3] - VSELP codevector 2 (n/a for voiced modes)
 *     piGsp0s[0:3] - GSP0 codewords
 *     swVadFlag - voice activity detection flag
 *     swSP - Speech flag
 *
 *   OUTPUTS:
 *
 *     pswBAlloc[0:20] - an array into which the coded data is moved
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   REFERENCES: Sub-clause 2.1 and 4.1.12 of GSM Recomendation 06.20
 *
 **************************************************************************/

void   fillBitAlloc(int iVoicing, int iR0, int *piVqIndeces,
                    int iSoftInterp, int *piLags,
                    int *piCodeWrdsA, int *piCodeWrdsB,
                    int *piGsp0s, INT16 swVadFlag,
                    INT16 swSP, INT16 *pswBAlloc)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    i;
    //INT16 *pswNxt;
    INT16 *outAlloc; //outAlloc[7];

    //FILE * fg1 = fopen("EncOutput.dat","a+");

    //VPP_HR_PROFILE_FUNCTION_ENTER(fillBitAlloc );

    /*_________________________________________________________________________
     |                                                                         |
     |                            Executable Code                              |
     |_________________________________________________________________________|
    */

    outAlloc = pswBAlloc;

    for (i = 0; i < 7; i++)
        outAlloc[i]=0;

    outAlloc[0]|= ( ((piVqIndeces[0]&0x07ff)<<5) | (iR0           &0x001f) );//LPC1(11bits/11bits)+RO  (4bits/4bits)
    outAlloc[1]|= ( ((piVqIndeces[2]&0x007f)<<9) | (piVqIndeces[1]&0x01ff) );//LPC3(7bits/8bits)  +LPC2(9bits/9bits)
    outAlloc[2]|= ( ((iVoicing      &0x0003)<<2) | ((iSoftInterp&0x0001)<<1) | ((piVqIndeces[2]&0x0080)>>7) );//iVoicing(2bits/2bits)+iSoftInterp(1bits/1bits)  +LPC3(1bits/8bits)

    if (iVoicing)
    {
        /* voiced mode */
        outAlloc[2]|= ( ((piCodeWrdsA[0]&0x000f)<<12) | ((piLags[0]&0x00ff)<<4) );//piCodeWrdsA[0](4bits/9bits)+piLags[0](8bits/8bits)
        outAlloc[3]|= ( ((piCodeWrdsA[1]&0x0003)<<14) | ((piLags[1]&0x000f)<<10)      | ((piGsp0s[0]&0x001f)<<5)  | ((piCodeWrdsA[0]&0x01f0)>>4) );//piCodeWrdsA[1](2bits/9bits)+piLags[1](4bits/4bits)+piGsp0s(5bits/5bits)+piCodeWrdsA[0](5bits/9bits)
        outAlloc[4]|= (                                 ((piLags[2]&0x000f)<<12)      | ((piGsp0s[1]&0x001f)<<7)  | ((piCodeWrdsA[1]&0x01fc)>>2) );//piLags[2](4bits/4bits)+piGsp0s[1](5bits/5bits)+piCodeWrdsA[1](7bits/9bits)
        outAlloc[5]|= ( ((piLags[3]     &0x0003)<<14) | ((piGsp0s[2]&0x001f)<<9)      | (piCodeWrdsA[2]&0x01ff) );//piLags[3](2bits/4bits)+piGsp0s[2](5bits/5bits)+piCodeWrdsA[2](9bits/9bits)
        outAlloc[6]|= ( ((piGsp0s[3]    &0x001f)<<11) | ((piCodeWrdsA[3]&0x01ff)<<2)  | ((piLags[3]&0x000c)>>2) );//piGsp0s[3](5bits/5bits)+piCodeWrdsA[3](9bits/9bits)+piLags[3](2bits/4bits)

    }
    else
    {
        /* unvoiced frame */
        outAlloc[2]|= ( ((piCodeWrdsB[0]&0x001f)<<11) | ((piCodeWrdsA[0]&0x007f)<<4) );//piCodeWrdsB[0](5bits/7bits)+piCodeWrdsA[0](7bits/7bits)
        outAlloc[3]|= ( ((piCodeWrdsB[1]&0x0003)<<14) | ((piCodeWrdsA[1]&0x007f)<<7)  | ((piGsp0s[0]&0x001f)<<2)     | ((piCodeWrdsB[0]&0x0060)>>5) );//piCodeWrdsB[1](2bits/7bits)+piCodeWrdsA[1](7bits/7bits)+piGsp0s(5bits/5bits)+piCodeWrdsB[0](2bits/7bits)
        outAlloc[4]|= ( ((piCodeWrdsA[2]&0x003f)<<10)                                 | ((piGsp0s[1]&0x001f)<<5)     | ((piCodeWrdsB[1]&0x007c)>>2) );//piCodeWrdsA[2](6bits/7bits)+piGsp0s(5bits/5bits)+piCodeWrdsB[1](5bits/7bits)
        outAlloc[5]|= ( ((piCodeWrdsA[3]&0x0007)<<13) | ((piGsp0s[2]&0x001f)<<8)      | ((piCodeWrdsB[2]&0x007f)<<1) | ((piCodeWrdsA[2]&0x0040)>>6) );//piCodeWrdsA[3](3bits/7bits)+piCodeWrdsA[1](7bits/7bits)+piGsp0s(5bits/5bits)+piCodeWrdsA[2](5bits/7bits)
        outAlloc[6]|= (                                 ((piGsp0s[3]&0x001f)<<11)     | ((piCodeWrdsB[3]&0x007f)<<4) | ((piCodeWrdsA[3]&0x0078)>>3) );//piCodeWrdsB[3](7bits/7bits)+piGsp0s(5bits/5bits)+piCodeWrdsA[3](4bits/7bits)

    }




    /*pswNxt = pswBAlloc;
    *pswNxt++ = iR0;
    for (i = 0; i < 3; i++)
      *pswNxt++ = *piVqIndeces++;
    *pswNxt++ = iSoftInterp;
    *pswNxt++ = iVoicing;

    // check voicing mode
    if (iVoicing)
    {
      // voiced mode
      for (i = 0; i < N_SUB; i++)
      {
        *pswNxt++ = *piLags++;
        *pswNxt++ = *piCodeWrdsA++;
        *pswNxt++ = *piGsp0s++;
      }
    }
    else
    {                                    // unvoiced frame
      for (i = 0; i < N_SUB; i++)
      {
        *pswNxt++ = *piCodeWrdsA++;
        *pswNxt++ = *piCodeWrdsB++;
        *pswNxt++ = *piGsp0s++;
      }
    }
    *pswNxt++ = swVadFlag;
    *pswNxt++ = swSP;*/




    /*for (i = 0 ; i < 20; i++)//
    {
        fprintf(fg1,"0x%04x,\n",pswBAlloc[i]);
    }
    fprintf(fg1,"\n\n" );*/

// VPP_HR_PROFILE_FUNCTION_EXIT(fillBitAlloc );



}

