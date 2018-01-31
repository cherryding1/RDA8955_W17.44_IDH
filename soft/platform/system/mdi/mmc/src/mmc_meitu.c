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

#include "vpp_meitu_filter.h"
#include "mmc.h"
#include "cs_types.h"
#include "mcip_debug.h"
#include "mmc_meitu.h"

volatile int meituFramecount=0;
volatile int meituFirstIsr=0;

void MEITUIsr(void)
{
    //MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0, "[MMC_MEITU]MEITUIsr");

    if (meituFirstIsr)
        meituFramecount = 1;
    else
        meituFirstIsr = 1;
}

boolean getMeituFinish(void)
{
    //MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0, "[MMC_MEITU]getMeituFinish");
    return meituFramecount;
}

INT32 initVocMeitu(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_MEITU]initMeitu");
    meituFramecount=0;
    meituFirstIsr=0;

    if(HAL_ERR_RESOURCE_BUSY == vpp_MeituOpen((HAL_VOC_IRQ_HANDLER_T)MEITUIsr))
        return -1;
    else
        return 0;
}

void quitVocMeitu(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_MEITU]quitMeitu");
    vpp_MeituClose();
}

uint32 getMeituLen(void)
{
    vpp_MEITU_OUT_T  g_vppmeitu_Status;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_MEITU]getMeituLen");

    //abtain the outputlen
    vpp_MeituStatus(&g_vppmeitu_Status);
    return g_vppmeitu_Status.output_len;
    //do what you want....
}

void  MMC_Meitu (UINT16 imgw, UINT16 imgh, char *bufin, char *bufout, MeituFiltermode filter_mode, UINT16 brightness, UINT16 contrast)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0, "[MMC_MEITU]MMC_Meitu width: %d;height: %d", imgw, imgh);

    vpp_MEITU_IN_T g_vppmeitu_In;
    int i;

    //encode a picture
    g_vppmeitu_In.image_width = imgw;
    g_vppmeitu_In.image_height = imgh;
    g_vppmeitu_In.inStreamBufAddr = (INT32*)bufin;
    g_vppmeitu_In.outStreamBufAddr = (INT32*)bufout;
    if (filter_mode == MMC_MEITU_SKINSMOOTH)
    {
        g_vppmeitu_In.brightness = brightness;
        g_vppmeitu_In.contrast = contrast;
    }
    else
    {
        g_vppmeitu_In.brightness = 5;
        g_vppmeitu_In.contrast = 5;
    }

    g_vppmeitu_In.mode = filter_mode;
    g_vppmeitu_In.reset = 0;
    //g_vppmeitu_In.framecount = 0;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_MEITU]MMC_Meitu......BEGIN TIME=%d",
              hal_TimGetUpTime());

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_MEITU]MMC_Meitu......brightness=%d\tcontrast=%d\n",
              g_vppmeitu_In.brightness, g_vppmeitu_In.contrast);

    for (i=0; i<imgh; i++)
    {
        vpp_MeituScheduleOneFrame(&g_vppmeitu_In);

        while(getMeituFinish()==0)
        {
            ;
        }

        meituFramecount = 0;
    }

    //mmi_trace(1,"voc encode a frame");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_MEITU]DONE! END TIME=%d",
              hal_TimGetUpTime());

    meituFramecount = 0;
}

