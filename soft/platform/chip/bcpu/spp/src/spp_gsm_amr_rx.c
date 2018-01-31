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






#include "spal_xcor.h"
#include "spal_vitac.h"
#include "spal_cap.h"
// spp public header
#include "spp_gsm.h"
// spp private header
#include "sppp_gsm_private.h"

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//=============================================================================
// spp_GetAmrIc
//-----------------------------------------------------------------------------
// This function decodes inband data for AFS or AHS
// deinterleaved softbit frames (SPEECH, SIDUPDATE, RATSCCH)
//
// @param softBit UINT8*. Pointer to the decoded data.
// @param mode UINT8. Type of frame: 0 -> AFS, 1 -> AHS, 2 -> SIDUPDATE/RATSCCH
//
// @return UINT8. Inband Data.
//
//=============================================================================
PUBLIC UINT8 spp_GetAmrIc(UINT8* softBit, UINT8 mode)
{
    UINT8 i, j, J;
    UINT8 ic = 0;
    UINT16* d;
    INT16 distance;
    INT16 MaxDistance = (INT16)0x8000;
    INT16 distance_max=0;

    if (mode==0)
    {
        // AFS
        J = 8;
        d = (UINT16*)g_sppIcAfsSpeech;
    }
    else if (mode==1)
    {
        // AHS
        J = 4;
        d = (UINT16*)g_sppIcAhsSpeech;
    }
    else
    {
        // SID UPDATE, RATSCCH
        J = 16;
        d = (UINT16*)g_sppIcSidRatscch;
    }

    for (j = 0; j < J; j++) distance_max+=(softBit[j]&0x7F);

    for (i = 0; i < 4; i++)
    {
        distance = 0;
        for (j = 0; j < J; j++)
        {

            if (((d[i] >> j) & 1) ^ ((*softBit >> 7) & 1))
                distance -= (*softBit & 0x7f);
            else
                distance += (*softBit & 0x7f);

            softBit++;
        }
        if (distance > MaxDistance)
        {
            MaxDistance = distance;
            ic = i;
        }
        softBit -= J;
    }


    if((MaxDistance*16)>(distance_max*14)) return ic;
    else return 0xFF;

}




//======================================================================
// Search AMR frame markers
//----------------------------------------------------------------------
// This function searches AMR frame markers and returns the most
// likely frame type. It decode also the associated in band data.
//
// @param BufferIn UINT32*. Pointer to the deinterleaved decoded data.
// @param id1 UINT8*. Pointer to ther first decoded in band data.
// @param id2 UINT8*. Pointer to the second decoded in band data.
// @param mode UINT8. TCH rate: 0 -> AFS, 1 -> AHS.
//
// @return UINT8. Most likely frame type.
//======================================================================
PUBLIC UINT8 spp_SearchAmrMarker(UINT32* BufferIn, UINT8* id1, UINT8* id2, UINT8 mode)
{
    UINT32 tmp_word1[8];
    UINT32 tmp_word2[57];
    UINT32 *ptr_word;
    UINT16 nb_diff=0;
    UINT16 ic1=0;
    UINT8 *ptr_byte1;
    UINT8 *ptr_byte2;
    UINT8 i=0;

    ////////////////////////
    // search for RATSCCH //
    ////////////////////////
    for (i=0; i<7; i++) tmp_word1[i]=g_sppRatscchMarker[i];
    ptr_word=BufferIn+4;
    spal_XcorBiterror(
        53,     // number of softvalue words to be treated
        ptr_word,// sofvalues_ptr,
        tmp_word1, // hardvalues_ptr,
        &nb_diff);
    if (nb_diff<=20)
    {
        if (mode==0)
        {
            // AFS RATSCCH
            *id1=spp_GetAmrIc((UINT8*)BufferIn, 2);
            *id2=spp_GetAmrIc(((UINT8*)BufferIn)+228, 2);
            return SPP_AFS_RATSCCH;
        }
        else
        {
            // AHS RATSCCH MARKER
            *id1=spp_GetAmrIc((UINT8*)BufferIn, 2);
            return SPP_AHS_RATSCCH_MARKER;
        }
    }

    ///////////////////////////
    // search for SID_UPDATE //
    ///////////////////////////
    if (mode==0)
    {
        for (i=0; i<7; i++) tmp_word1[i]=g_sppAfsSidMarker[i];
        for (i=0; i<53; i++) tmp_word2[i]=BufferIn[8+2*i+1];
    }
    else
    {
        for (i=0; i<7; i++) tmp_word1[i]=g_sppAhsSidMarker[i];
        for (i=0; i<53; i++) tmp_word2[i]=BufferIn[4+i];
    }
    nb_diff=0;
    spal_XcorBiterror(
        53,     // number of softvalue words to be treated
        tmp_word2,// sofvalues_ptr,
        tmp_word1,// hardvalues_ptr,
        &nb_diff);
    if (nb_diff<=20)
    {
        if (mode==0)
        {
            // AFS SIDUPDATE
            for (i=0; i<4; i++) tmp_word2[i]=BufferIn[2*i+1];
            *id1=spp_GetAmrIc((UINT8*)tmp_word2, 2);
            return SPP_AFS_SID_UPDATE;
        }
        else
        {
            // AHS SIDUPDATE MARKER
            *id1=spp_GetAmrIc((UINT8*)BufferIn, 2);
            return SPP_AHS_SID_UPDATE;
        }
    }
    if (mode==1)
    {
        ///////////////////////////////
        // search for SID_UPDATE_INH //
        ///////////////////////////////
        nb_diff=0;
        for(i=0; i<7; i++) tmp_word1[i]=((g_sppAfsSidMarker[i])&0xAAAAAAAA) | ((g_sppAhsSidMarker[i])&0x55555555);
        spal_XcorBiterror(
            53,     // number of softvalue words to be treated
            ptr_word,// sofvalues_ptr,
            tmp_word1,// hardvalues_ptr,
            &nb_diff);
        if (nb_diff<=20)
        {
            *id1=spp_GetAmrIc((UINT8*)BufferIn,2);
            return SPP_AHS_SID_UPDATE_INH;

        }
    }

    //////////////////////////
    // search for SID_FIRST //
    //////////////////////////
    for (i=0; i<7; i++) tmp_word1[i]=g_sppAfsSidMarker[i];
    if (mode==0)
    {
        for (i=0; i<53; i++) tmp_word2[i]=BufferIn[8+2*i];
    }
    else
    {
        for (i=0; i<53; i++) tmp_word2[i]=BufferIn[4+i];
    }
    nb_diff=0;
    spal_XcorBiterror(
        53,     // number of softvalue words to be treated
        tmp_word2,// sofvalues_ptr,
        tmp_word1,// hardvalues_ptr,
        &nb_diff);
    if (nb_diff<=20)
    {
        if (mode==0)
        {
            // AFS SID_FISRT
            for (i=0; i<4; i++) tmp_word2[i]=BufferIn[2*i];
            *id1=spp_GetAmrIc((UINT8*)tmp_word2, 2);
            return SPP_AFS_SID_FIRST;
        }
        else
        {
            // AHS SID_FIRST_P1
            *id1=spp_GetAmrIc((UINT8*)BufferIn, 2);
            return SPP_AHS_SID_FIRST_P1;
        }
    }
    if (mode==1)
    {
        //////////////////////////////
        // search for SID_FIRST_INH //
        //////////////////////////////
        for(i=0; i<7; i++) tmp_word1[i]=((g_sppAhsSidMarker[i])&0xAAAAAAAA) | ((g_sppAfsSidMarker[i])&0x55555555);
        nb_diff=0;
        spal_XcorBiterror(
            53,     // number of softvalue words to be treated
            ptr_word,// sofvalues_ptr,
            tmp_word1,// hardvalues_ptr,
            &nb_diff);
        if (nb_diff<=20)
        {
            *id1=spp_GetAmrIc((UINT8*)BufferIn, 2);
            return SPP_AHS_SID_FIRST_INH;
        }
    }

    ////////////////////////
    //  search for ONSET  //
    ////////////////////////
    if (mode==0)
    {
        for (i=0; i<4; i++) tmp_word2[i]=BufferIn[2*i+1];
        *id1=spp_GetAmrIc((UINT8*)tmp_word2, 2);
        ic1=g_sppIcSidRatscch[*id1];
        for (i=0; i<8; i++) tmp_word1[i] = ic1<<16 | ic1;
        tmp_word1[7]&=0xF; // only 4 bits on the last word
        for (i=0; i<57; i++) tmp_word2[i]=BufferIn[2*i+1];

        nb_diff=0;
        spal_XcorBiterror(
            57,     // number of softvalue words to be treated
            tmp_word2,// sofvalues_ptr,
            tmp_word1 ,// hardvalues_ptr,
            &nb_diff);
        if (nb_diff<=20)
        {
            return SPP_AFS_ONSET;
        }
    }
    else
    {
        ptr_byte1=(UINT8*)tmp_word2;
        ptr_byte2=(UINT8*)BufferIn;
        for (i=0; i<16; i++)
        {
            ptr_byte1[i]=ptr_byte2[2*i+1];
        }
        *id1=spp_GetAmrIc(ptr_byte1, 2);
        ic1=g_sppIcSidRatscch[*id1];
        for (i=0; i<4; i++) tmp_word1[i] = ic1<<16 | ic1;
        tmp_word1[3]&=0x0003FFFF; // only 18 bits on the last word
        for (i=0; i<114; i++)
        {
            ptr_byte1[i]=ptr_byte2[2*i+1];
        }
        tmp_word2[28]&=0x0000FFFF;
        nb_diff=0;
        spal_XcorBiterror(
            29,     // number of softvalue words to be treated
            tmp_word2,// sofvalues_ptr,
            tmp_word1,// hardvalues_ptr,
            &nb_diff);
        if (nb_diff<=10)
        {
            return SPP_AHS_ONSET;
        }
    }

    ////////////////////////
    //       SPEECH       //
    ////////////////////////
    if(mode==0)
    {
        *id1=spp_GetAmrIc((UINT8*)BufferIn,0);
        return SPP_AFS_SPEECH;
    }
    else
    {
        *id1=spp_GetAmrIc((UINT8*)BufferIn,1);
        return SPP_AHS_SPEECH;
    }

}

//=============================================================================
// spp_GetAmrMetric
//-----------------------------------------------------------------------------
// This function return amr metric
// @param softBit UINT8*. Pointer to the decoded data.
// @param rate FR/HR
// @param ic inband data
// @param biterror BER
//=============================================================================
PUBLIC UINT8 spp_GetAmrMetric(UINT8* softBit, UINT8 rate, UINT8 ic, UINT8 *biterror)
{
    UINT8 i=0, j, J,sht;
    UINT16* d;
    INT16 distance;

    if (rate==0)
    {
        // AFS
        J = 8;
        sht= 3;
        d = (UINT16*)g_sppIcAfsSpeech;
    }
    else if (rate==1)
    {
        // AHS
        J = 4;
        sht = 2;
        d = (UINT16*)g_sppIcAhsSpeech;
    }
    else
    {
        // SID UPDATE, RATSCCH
        J = 16;
        sht = 4;
        d = (UINT16*)g_sppIcSidRatscch;
    }

    if( ic >= 4 )
    {
        *biterror = J;
        return 0;
    }

    distance = 0;

    for (j = 0; j < J; j++)
    {

        if (((d[ic] >> j) & 1) ^ ((*softBit >> 7) & 1))
        {
            distance -= (*softBit & 0x7f);
            i++;
        }
        else
        {
            distance += (*softBit & 0x7f);
        }

        softBit++;
    }

    *biterror = i;

    if (distance < 0)
    {
        distance = 0;
    }

    return distance>>sht;

}


