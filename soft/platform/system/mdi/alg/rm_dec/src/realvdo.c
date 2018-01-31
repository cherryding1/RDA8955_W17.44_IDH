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
































#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

#include "realvdo.h"
#include "rm_memory_default.h"



void RVDecoder_Init(struct RVDecoder *t, RV_FID fid, RV_Status *ps)
{
    U16 test_endian;
    t->m_signature = RV_SIGNATURE;
    t->m_is_big_endian = FALSE;
    t->m_picture_clock_frequency = ((F64)29.97);

    /* Just to be safe */
    RVAssert(sizeof(U8) == 1);
    RVAssert(sizeof(I8) == 1);
    RVAssert(sizeof(U16) == 2);
    RVAssert(sizeof(I16) == 2);
    RVAssert(sizeof(U32) == 4);
    RVAssert(sizeof(I32) == 4);
    RVAssert(sizeof(F32) == 4);
    RVAssert(sizeof(F64) == 8);

    test_endian = 0x1234;
    t->m_is_big_endian = (RV_Boolean)((*(U8*)&test_endian) != 0x34);

    /* sheen
    RV_Identify_CPU(&t->m_cpu);

    RVDebug((RV_TL_INIT,
        "RealVideo::RealVideo(generic implementation), architecture %ld, "
        " %s-endian",
        (U32)(t->m_cpu.architecture),
        (t->m_is_big_endian ? "big" : "little")));
    */
    t->m_coredecoder = NULL;
    t->m_coredecoder = Decoder_CreateDecoder(fid, t, ps);

    if (!t->m_coredecoder)
    {
        RVDebug((RV_TL_ALWAYS, "ERROR ::Generic_Decoder_RealVideo --"
                 " failed to allocate RV_Decoder for %d",
                 fid));
        *ps = RV_S_OUT_OF_MEMORY;
    }
    else if (*ps != RV_S_OK)
    {
        Decoder_Delete(t->m_coredecoder);
        rm_memory_default_free(0, t->m_coredecoder);
        t->m_coredecoder = NULL;
        RVDebug((RV_TL_ALWAYS, "ERROR ::Generic_Decoder_RealVideo --"
                 " failed to create RV_Decoder for %d",
                 fid));
    }
}

RV_Boolean RVDecoder_is_active(struct RVDecoder *h)
{
    return (RV_Boolean)(
               h && h->m_signature == RV_SIGNATURE);
}


RV_Boolean
RVDecoder_Load()
{
    RV_Status  ps = RV_S_OK;

    g_number_of_loads++;

    if (g_number_of_loads == 1)
    {

        ps = Decoder_Startup();
    }

    return (ps == RV_S_OK) ? TRUE : FALSE;
}

RV_Boolean
RVDecoder_Unload()
{
    RV_Status  ps = RV_S_OK;

    if (g_number_of_loads > 0)
    {
        g_number_of_loads--;

        if (g_number_of_loads == 0)
        {
            ps = Decoder_Shutdown();
        }
    }

    return (ps == RV_S_OK) ? TRUE : FALSE;
}

void RVDecoder_Delete(struct RVDecoder *t)
{

    if (t->m_coredecoder)
    {
        Decoder_Delete(t->m_coredecoder);
        rm_memory_default_free(0, t->m_coredecoder);
    }
    t->m_coredecoder = 0;

    t->m_signature = 0;
}




