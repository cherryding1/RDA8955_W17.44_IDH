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


#include "vpp_audiojpegenc.h"
#include "mmc.h"
#include "cs_types.h"
#include"mcip_debug.h"

volatile int jpgEnframecount=0;
volatile int jpgEnFirstIsr=0;

void JPEGENCIsr(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]JPEGENCIsr");

    if (jpgEnFirstIsr)
        jpgEnframecount=1;
    else
        jpgEnFirstIsr=1;
}
boolean getJpegEncodeFinish(void)
{
    //MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]getJpegEncodeFinish");
    return jpgEnframecount;
}
INT32 initVocJpegEncode(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]initVocJpegEncode");
    jpgEnframecount=0;
    jpgEnFirstIsr=0;

    if(HAL_ERR_RESOURCE_BUSY == vpp_AudioJpegEncOpen((HAL_VOC_IRQ_HANDLER_T)JPEGENCIsr))
        return -1;
    else
        return 0;
}

void quitVocJpegEncode(void)
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]quitVocJpegEncode");
    vpp_AudioJpegEncClose();
}

uint32 getJpegEncodeLen(void)
{
    vpp_AudioJpeg_ENC_OUT_T  g_vppaudiojpeg_encDecStatus;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,  "[MMC_JPEGENC]getJpegEncodeLen");

    //abtain the outputlen
    vpp_AudioJpegEncStatus(&g_vppaudiojpeg_encDecStatus);
    return g_vppaudiojpeg_encDecStatus.output_len;
    //do what you want....
}

void  MMC_jpgEn (UINT16 srcw, UINT16 srch, UINT16 imgw, UINT16 imgh, UINT16 quality, char *bufin, char *bufout, UINT16 yuvmode)
{
    int timeout_num;
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0, "[MMC_JPEGENC]MMC_jpgEn width: %d;width:%d", imgw, imgh);

    vpp_AudioJpeg_ENC_IN_T g_vppaudiojpeg_encDecIn;
    //encode a picture
    g_vppaudiojpeg_encDecIn.SampleRate = ((srch<<16)|srcw);
    g_vppaudiojpeg_encDecIn.imag_width = imgw;
    g_vppaudiojpeg_encDecIn.imag_height = imgh;
    g_vppaudiojpeg_encDecIn.imag_quality = quality;
    g_vppaudiojpeg_encDecIn.inStreamBufAddr = (INT32*)bufin;
    g_vppaudiojpeg_encDecIn.outStreamBufAddr = (INT32*)(bufout + 608);
    g_vppaudiojpeg_encDecIn.mode = MMC_MJPEG_ENCODE;
    g_vppaudiojpeg_encDecIn.channel_num = yuvmode;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_JPEGENC]MMC_jpgEn......BEGIN TIME=%d",
              hal_TimGetUpTime());

    vpp_AudioJpegEncScheduleOneFrame(&g_vppaudiojpeg_encDecIn);

    timeout_num= 500;//500*5ms
    while(getJpegEncodeFinish()==0)
    {
        sxr_Sleep(80);
        timeout_num--;
        if(timeout_num<=0)
        {
            MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
                      0,
                      "[MMC_JPEGENC]ENCODING fail! Wait interrupt timeout! TIME=%d",
                      hal_TimGetUpTime());
            return;
        }
    }
    //mmi_trace(1,"voc encode a frame");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_JPEGENC]ENCODING DONE! END TIME=%d",
              hal_TimGetUpTime());

    jpgEnframecount = 0;
    VPP_WRITE_JPEGHeadr(g_vppaudiojpeg_encDecIn.imag_quality,
                        g_vppaudiojpeg_encDecIn.imag_width,
                        g_vppaudiojpeg_encDecIn.imag_height,
                        (uint8*) bufout);
    hal_HstSendEvent(0xbcbc0104);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT,
              0,
              "[MMC_JPEGENC]VPP_WRITE_JPEGHeadr DONE! END TIME=%d",
              hal_TimGetUpTime());
}


