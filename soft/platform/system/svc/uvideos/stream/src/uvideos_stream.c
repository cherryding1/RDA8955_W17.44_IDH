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

#include "uctls_m.h"
#include "uvideos_config.h"

typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;
#include "itf_msg.h"
#include "hal_usb.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "uvideos_stream_m.h"
#include "uvideosp_debug.h"
#include "string.h"

#include "mmc.h"
#include "mcip_debug.h"
#include "camera_m.h"
#include "pmd_m.h"
#include "vpp_audiojpegenc.h"

//test
//#include "fs.h"

CAM_SIZE_T SensorFrameSize = CAM_NPIX_QVGA;
UINT16 SensorFrameWidth = 240;
UINT16 SensorFrameHeight = 320;



PRIVATE UINT16  *g_sensorbuf = NULL;
PRIVATE UINT8  *g_jpegbuf = NULL;
PRIVATE UINT32 g_jpeg_len = 0;
PRIVATE UINT32 g_sent_jpeg_len = 0;
PRIVATE UINT8 g_frame_id = 0;
PRIVATE UINT8 g_clear_feature_flag = 0;
PRIVATE BOOL g_still_capture = FALSE;




extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeAll();


PRIVATE UINT8                        g_uvideosBoInEp;

PRIVATE sxr_TaskDesc_t               g_uvideosTask;
PRIVATE UINT8                        g_uvideosIdTask = 0;
PRIVATE UINT8                        g_uvideosMbx;
PRIVATE Msg_t                        g_uvideosCamMsg;


PRIVATE UINT8                        g_uvideosEP0Out [HAL_USB_MPS];
extern UINT8                        g_UsbBulkIn [4096];

/* payload header info offset */
#define USBVIDEO_PAYLOAD_FRAME_ID               0
#define USBVIDEO_PAYLOAD_EOF                        1
#define USBVIDEO_PAYLOAD_PTS                        2
#define USBVIDEO_PAYLOAD_SCR                        3
#define USBVIDEO_PAYLOAD_RESVD                  4
#define USBVIDEO_PAYLOAD_STILL                  5
#define USBVIDEO_PAYLOAD_ERR                        6
#define USBVIDEO_PAYLOAD_EOH                        7

#define USBVIDEO_PAYLOAD_HEADER_LENGTH          12
#define USBVIDEO_MAX_PAYLOAD_LENGTH             4096

typedef enum
{
    USBVIDEO_PROBE_NONE,
    USBVIDEO_PROBE_IN,
    USBVIDEO_PROBE_DONE,
    USBVIDEO_COMMIT_IN,
    USBVIDEO_COMMIT_DONE
} UVIDEOS_PROBE_STATE_T;

typedef enum
{
    UVIDEOS_MSG_START_PREVIEW,
    UVIDEOS_MSG_FRAME_READY,
    UVIDEOS_MSG_ENCODE_DONE,
    UVIDEOS_MSG_SEND_VIDEO_DATA,
    UVIDEOS_MSG_NONE
} UVIDEOS_MSG;

PRIVATE UVIDEOS_CALLBACK_T uvideos_Send_Msg = NULL;
PRIVATE UVIDEOS_PROBE_STATE_T g_uvideosProbeState = USBVIDEO_PROBE_NONE;
PRIVATE UVIDEOS_PROBE_STATE_T g_uvideosStillProbeState = USBVIDEO_PROBE_NONE;

/*Video Probe and Commit Controls Struct*/
struct usb_video_probe_commit_ctr_struct
{
    UINT16  bmHint;
    UINT8  bFormatIndex;
    UINT8 bFrameIndex;
    UINT32  dwFrameInterval;
    UINT16  wKeyFrameRate;
    UINT16  wPFrameRate;
    UINT16  wCompQuality;
    UINT16  wCompWindowSize;
    UINT16  wDelay;
    UINT32  dwMaxVideoFrameSize;
    UINT32  dwMaxPayloadTransferSize;
    UINT32 dwClockFrequency;
    UINT8  bmFramingInfo;
    UINT8  bPreferedVersion;
    UINT8  bMinVersion;
    UINT8  bMaxVersion;

} __attribute__ ((packed));


static struct usb_video_probe_commit_ctr_struct video_probe_commit_ctr =
{
    .bmHint = 0x0000,
    .bFormatIndex = 0x01,
    .bFrameIndex = 0x01,
    .dwFrameInterval = 0x0007a120,//50ms
    .wKeyFrameRate = 0x0001,
    .wPFrameRate = 0x0000,
    .wCompQuality = 0x0000,
    .wCompWindowSize = 0x0000,
    .wDelay = 0x0000,
    .dwMaxVideoFrameSize = 0x0000c000,
    .dwMaxPayloadTransferSize = USBVIDEO_MAX_PAYLOAD_LENGTH,//
    .dwClockFrequency = 0,
    .bmFramingInfo = 0,
    .bPreferedVersion = 0,
    .bMinVersion = 0,
    .bMaxVersion = 0,
};

static struct usb_video_probe_commit_ctr_struct video_probe_commit_ctr_recv;

struct usb_still_probe_commit_ctr_struct
{
    UINT8  bFormatIndex;
    UINT8  bFrameIndex;
    UINT8  bCompressionIndex;
    UINT32  dwMaxVideoFrameSize;
    UINT32  dwMaxPayloadTransferSize;
} __attribute__ ((packed));

static struct usb_still_probe_commit_ctr_struct still_probe_commit_ctr =
{
    .bFormatIndex = 0x01,
    .bFrameIndex = 0x01,
    .bCompressionIndex = 0x01,
    .dwMaxVideoFrameSize = 0x0000c000,
    .dwMaxPayloadTransferSize = USBVIDEO_MAX_PAYLOAD_LENGTH,
};

static struct usb_still_probe_commit_ctr_struct still_probe_commit_ctr_recv;

typedef struct
{
    UINT8       bHeaderLength;
    UINT8       bmHeaderInfo;
    UINT8       dwPTS[4];
    UINT8       dwSCR[6];
} USBVideo_Payload_Header;


PRIVATE HAL_USB_CALLBACK_RETURN_T
videosStreamCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Cmd,request=%x,value=%x,index=%d,length=%d\n",setup->request,setup->value,setup->index,setup->lenght);
            switch(setup->value)
            {
                case USBVIDEO_VS_PROBE_CONTROL:
                case USBVIDEO_VS_COMMIT_CONTROL:
                    if(setup->index == 1)
                    {
                        if((setup->request == USBVIDEO_GET_CUR)&&(setup->lenght == 26))
                            //&& ((setup->lenght == 26)||(setup->lenght == 34)) )
                        {
                            hal_UsbSend(0, (UINT8*)&video_probe_commit_ctr, 26, 0);//!!!video_probe_commit_ctr actually size is 34
                            return(HAL_USB_CALLBACK_RETURN_OK);
                        }
                        else if((setup->request == USBVIDEO_GET_MAX)||(setup->request == USBVIDEO_GET_MIN))
                        {
                        }
                        else if((setup->request == USBVIDEO_GET_INFO)&&(setup->lenght == 1))
                        {
                        }
                        else if((setup->request == USBVIDEO_SET_CUR)&&(setup->lenght == 26))
                            //&& ((setup->lenght == 26)||(setup->lenght == 34)))
                        {
                            hal_UsbRecv(0, g_uvideosEP0Out, HAL_USB_MPS, 0);

                            if(setup->value==USBVIDEO_VS_PROBE_CONTROL)
                                g_uvideosProbeState = USBVIDEO_PROBE_IN;
                            else
                                g_uvideosProbeState = USBVIDEO_COMMIT_IN;

                            return(HAL_USB_CALLBACK_RETURN_OK);
                        }
                        else
                        {
                            return(HAL_USB_CALLBACK_RETURN_KO);
                        }
                    }
                    else
                    {
                        return(HAL_USB_CALLBACK_RETURN_KO);
                    }
                    break;
                case USBVIDEO_VS_STILL_PROBE_CONTROL:
                case USBVIDEO_VS_STILL_COMMIT_CONTROL:
                    if((setup->request == USBVIDEO_GET_CUR) && (setup->index == 1)
                            && (setup->lenght == 11))
                    {
                        hal_UsbSend(0, (UINT8*)&still_probe_commit_ctr, 11, 0);
                        return(HAL_USB_CALLBACK_RETURN_OK);
                    }
                    else if((setup->request == USBVIDEO_GET_INFO) && (setup->index == 1)
                            && (setup->lenght == 1))
                    {
                    }
                    else if((setup->request == USBVIDEO_SET_CUR) && (setup->index == 1)
                            && (setup->lenght == 11))
                    {
                        hal_UsbRecv(0, g_uvideosEP0Out, HAL_USB_MPS, 0);

                        if(setup->value==USBVIDEO_VS_PROBE_CONTROL)
                            g_uvideosStillProbeState = USBVIDEO_PROBE_IN;
                        else
                            g_uvideosStillProbeState = USBVIDEO_COMMIT_IN;

                        return(HAL_USB_CALLBACK_RETURN_OK);
                    }
                    else
                    {
                        return(HAL_USB_CALLBACK_RETURN_KO);
                    }
                    break;
                case USBVIDEO_VS_STILL_IMAGE_TRIGGER_CONTROL:
                    if((setup->request == USBVIDEO_SET_CUR) && (setup->index == 1)
                            && (setup->lenght == 1))
                    {
                        /* register handler to handle the get data*/
                        hal_UsbRecv(0, g_uvideosEP0Out, HAL_USB_MPS, 0);

                        return(HAL_USB_CALLBACK_RETURN_OK);
                    }
                    else
                    {
                        return(HAL_USB_CALLBACK_RETURN_KO);
                    }
                    break;
                default:
                    break;
            }
            break;
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Data Cmd,lenght=%d\n",setup->lenght);
            if(setup->requestDesc.requestDirection)
            {
            }
            else
            {
                if(setup->lenght == 26)
                {
                    memcpy(&video_probe_commit_ctr_recv, g_uvideosEP0Out, setup->lenght);
                    //UINT8 count;
                    //for (count=0;count<setup->lenght;count++)
                    //  UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "%x",((UINT8 *)&video_probe_commit_ctr_recv)[count]);
                    if(g_uvideosProbeState == USBVIDEO_PROBE_IN)
                    {
                        g_uvideosProbeState = USBVIDEO_PROBE_DONE;
                    }
                    else if(g_uvideosProbeState == USBVIDEO_COMMIT_IN)
                    {
                        g_uvideosProbeState = USBVIDEO_COMMIT_DONE;

                        if(g_clear_feature_flag == 0)
                        {
                            uvideos_Send_Msg(UVIDEOS_MSG_MED_START_SUCCESS_CNF, 0, 0);
                        }
                        else
                        {
                            g_uvideosCamMsg.B = UVIDEOS_MSG_START_PREVIEW;
                            sxr_Send(&g_uvideosCamMsg, g_uvideosMbx, SXR_SEND_MSG);
                        }
                    }
                }
                else if(setup->lenght == 11)
                {
                    memcpy(&still_probe_commit_ctr_recv, g_uvideosEP0Out, setup->lenght);
                    //UINT8 count;
                    //for (count=0;count<setup->lenght;count++)
                    //  UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "%x",((UINT8 *)&still_probe_commit_ctr_recv)[count]);
                    if(g_uvideosStillProbeState == USBVIDEO_PROBE_IN)
                    {
                        g_uvideosStillProbeState = USBVIDEO_PROBE_DONE;
                    }
                    else if(g_uvideosStillProbeState == USBVIDEO_COMMIT_IN)
                    {
                        g_uvideosStillProbeState = USBVIDEO_COMMIT_DONE;
                    }
                }
                else if(setup->lenght == 1)
                {
                    uvideos_Send_Msg(UVIDEOS_MSG_MED_STILL_CAPTURE_IND, 0, 0);
                }
            }
            return(HAL_USB_CALLBACK_RETURN_OK);
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Receive end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Transmit end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Enable\n");
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video Stream Callback Disable\n");
            //reset clear feature flag
            g_clear_feature_flag = 0;
            break;
    }

    return(HAL_USB_CALLBACK_RETURN_KO);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T
videosCallbackEp(HAL_USB_CALLBACK_EP_TYPE_T   type,
                 HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Cmd data\n");
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Receive end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_CMD:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Cmd\n");
            if(setup->request == HAL_USB_REQUEST_EP_CLEAR_FEATURE)
            {
                uvideos_CameraStopPreview();
                uvideos_CameraPowerdown();
                g_clear_feature_flag = 1;
            }
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Transmit end\n");
            if(g_sent_jpeg_len < g_jpeg_len)
            {
                //current jpeg frame transmit not complete,continue send
                g_uvideosCamMsg.B = UVIDEOS_MSG_SEND_VIDEO_DATA;
                sxr_Send(&g_uvideosCamMsg, g_uvideosMbx, SXR_SEND_MSG);
            }
            else
            {
                UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video g_still_capture=%d\n",g_still_capture);
                if(g_still_capture == TRUE)
                {
                    //current jpeg frame transmit complete for still capture,restart video
                    uvideos_Send_Msg(UVIDEOS_MSG_MED_START_IND, 0, 0);
                }
                else
                {
                    //current jpeg frame transmit complete,get next frame from sensor
                    camera_PrepareImageCaptureL(0, SensorFrameSize, g_sensorbuf);
                    camera_CaptureImage();
                }
            }
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Enable\n");
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "--- Usb Video EP Callback Disable\n");
            break;
    }

    return(HAL_USB_CALLBACK_RETURN_OK);
}


PRIVATE VOID uvideos_CompleteJpegBuff(VOID)
{
    vpp_AudioJpeg_ENC_OUT_T VocOutputStatus;
    USBVideo_Payload_Header *pHeader;
    UINT32 time;

    time=hal_TimGetUpTime();

    vpp_AudioJpegEncStatus(&VocOutputStatus);
    g_jpeg_len = VocOutputStatus.output_len;
    g_sent_jpeg_len = 0;

    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CompleteJpegBuff,jpeg_len=%d",g_jpeg_len);

    memset(g_UsbBulkIn, 0, sizeof(USBVideo_Payload_Header));

    pHeader = (USBVideo_Payload_Header*)g_UsbBulkIn;
    pHeader->dwPTS[0] = time&0xff;
    pHeader->dwPTS[1] = (time>>8)&0xff;
    pHeader->dwPTS[2] = (time>>16)&0xff;
    pHeader->dwPTS[3] = (time>>24)&0xff;
    pHeader->bHeaderLength = sizeof(USBVideo_Payload_Header);
    pHeader->bmHeaderInfo = (1<<USBVIDEO_PAYLOAD_PTS)|(1<<USBVIDEO_PAYLOAD_SCR)|(g_frame_id<<USBVIDEO_PAYLOAD_FRAME_ID)|(g_still_capture<<USBVIDEO_PAYLOAD_STILL);
    /* dwSCR will be filled when configring HAL usb send */

    g_uvideosCamMsg.B = UVIDEOS_MSG_SEND_VIDEO_DATA;
    sxr_Send(&g_uvideosCamMsg, g_uvideosMbx, SXR_SEND_MSG);
}

PRIVATE VOID uvideos_VideoSend(VOID)
{
    USBVideo_Payload_Header *pHeader;

    UINT32 time;
    static UINT16 count;

    time=hal_TimGetUpTime();
    pHeader = (USBVideo_Payload_Header*)g_UsbBulkIn;

    pHeader->dwSCR[0] = time&0xff;
    pHeader->dwSCR[1] = (time>>8)&0xff;
    pHeader->dwSCR[2] = (time>>16)&0xff;
    pHeader->dwSCR[3] = (time>>24)&0xff;
    pHeader->dwSCR[4] = count&0xff;
    pHeader->dwSCR[5] = (count>>8)&0xff;
    count++;

    if(g_jpeg_len <= g_sent_jpeg_len + USBVIDEO_MAX_PAYLOAD_LENGTH -USBVIDEO_PAYLOAD_HEADER_LENGTH)
    {
        //the last part of current jpeg frame
        pHeader->bmHeaderInfo |= (1<<USBVIDEO_PAYLOAD_EOF);
        g_frame_id  = g_frame_id ^ 0x1;
        UINT32 jpeg_remain_len = g_jpeg_len - g_sent_jpeg_len;

        memcpy(g_UsbBulkIn + USBVIDEO_PAYLOAD_HEADER_LENGTH, g_jpegbuf + 1 + g_sent_jpeg_len, jpeg_remain_len);
        g_sent_jpeg_len = g_jpeg_len;
        hal_UsbSend(g_uvideosBoInEp, g_UsbBulkIn, jpeg_remain_len + USBVIDEO_PAYLOAD_HEADER_LENGTH, 1);
    }
    else
    {
        //not the last part of current jpeg frame
        memcpy(g_UsbBulkIn + USBVIDEO_PAYLOAD_HEADER_LENGTH, g_jpegbuf + 1 + g_sent_jpeg_len, USBVIDEO_MAX_PAYLOAD_LENGTH - USBVIDEO_PAYLOAD_HEADER_LENGTH);
        g_sent_jpeg_len += (USBVIDEO_MAX_PAYLOAD_LENGTH -USBVIDEO_PAYLOAD_HEADER_LENGTH);
        hal_UsbSend(g_uvideosBoInEp, g_UsbBulkIn, USBVIDEO_MAX_PAYLOAD_LENGTH, 1);
    }
}


PRIVATE VOID uvideos_CameraFrameReadyCB(UINT8 id,  UINT8 ActiveBufNum)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraFrameReadyCB,valid:%d",id);
    if(id == 0)
    {
        g_uvideosCamMsg.B = UVIDEOS_MSG_FRAME_READY;
        sxr_Send(&g_uvideosCamMsg, g_uvideosMbx, SXR_SEND_MSG);
    }
    else
    {
        //image corrupt,get one new frame
        camera_PrepareImageCaptureL(0, SensorFrameSize, g_sensorbuf);
        camera_CaptureImage();
    }
}


PRIVATE VOID uvideos_ImageEncodeDone(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_ImageEncodeDone");

    uvideos_CompleteJpegBuff();
}

PRIVATE UINT32 IsFirstEncodeIntr = 0;

PRIVATE VOID uvideos_ImageEncodeIsr(HAL_VOC_IRQ_STATUS_T* irqStatus)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_ImageEncodeIsr->IsFirstEncodeIntr=%d",IsFirstEncodeIntr);
    if(IsFirstEncodeIntr)
    {
        g_uvideosCamMsg.B = UVIDEOS_MSG_ENCODE_DONE;
        sxr_Send(&g_uvideosCamMsg, g_uvideosMbx, SXR_SEND_MSG);
    }
    else
    {
        IsFirstEncodeIntr = 1;
    }

}

PRIVATE VOID uvideos_ImageEncodeInit(VOID)
{
    HAL_ERR_T voc_ret = 0;
    IsFirstEncodeIntr = 0;

    voc_ret                    = vpp_AudioJpegEncOpen(uvideos_ImageEncodeIsr);

    switch (voc_ret)
    {
        case HAL_ERR_NO:
            break;

        case HAL_ERR_RESOURCE_RESET:
        case HAL_ERR_RESOURCE_BUSY:
        case HAL_ERR_RESOURCE_TIMEOUT:
        case HAL_ERR_RESOURCE_NOT_ENABLED:
        case HAL_ERR_BAD_PARAMETER:
        case HAL_ERR_UART_RX_OVERFLOW:
        case HAL_ERR_UART_TX_OVERFLOW:
        case HAL_ERR_UART_PARITY:
        case HAL_ERR_UART_FRAMING:
        case HAL_ERR_UART_BREAK_INT:
        case HAL_ERR_TIM_RTC_NOT_VALID:
        case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
        case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
        case HAL_ERR_COMMUNICATION_FAILED:
        case HAL_ERR_QTY:
            UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "fail to call vpp_AMREncOpen(), result: %d \n", voc_ret);
        default:
            break;
    }

}

PRIVATE VOID uvideos_ImageEncode(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_ImageEncode");

    vpp_AudioJpeg_ENC_IN_T VocImgEncIn;

    VocImgEncIn.SampleRate  = ((SensorFrameHeight<<16)|SensorFrameWidth);
    VocImgEncIn.imag_width  = SensorFrameWidth;
    VocImgEncIn.imag_height = SensorFrameHeight;

    VocImgEncIn.mode        = MMC_MJPEG_ENCODE;
    VocImgEncIn.imag_quality = 1;
    VocImgEncIn.channel_num = 1;//yuv


    VPP_WRITE_JPEGHeadr(VocImgEncIn.imag_quality, SensorFrameWidth,SensorFrameHeight,g_jpegbuf);

    VocImgEncIn.inStreamBufAddr  = (INT32*)g_sensorbuf;

    VocImgEncIn.outStreamBufAddr = (INT32*)(g_jpegbuf+ 608);

    vpp_AudioJpegEncScheduleOneFrame(&VocImgEncIn);

}


// =============================================================================
// PUBLIC API
// =============================================================================
VOID uvideos_CameraGetInfo (VOID)
{
    camera_Reserve(NULL);
    camera_GetImageInfo(&SensorFrameSize, &SensorFrameWidth, &SensorFrameHeight);
}

UINT32 uvideos_CameraPowerUp (VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPowerUp");

    UINT16 ispOutWidth = 0, ispOutHeight = 0;

    ispOutWidth = SensorFrameWidth;
    ispOutHeight = SensorFrameHeight;

    //initialize for the pointers for safety reason
    UINT16*  InvalideAddress = (UINT16*)convertaddr(NULL);
    UINT8*  InvalideAddressU8 = (UINT8*)convertaddr(NULL);

    g_jpegbuf = (UINT8*)convertaddr(mmc_MemMalloc(SensorFrameWidth*SensorFrameHeight*2 + 20480));
    g_sensorbuf = (UINT16*)(g_jpegbuf + 20480);

    camera_Reserve(uvideos_CameraFrameReadyCB);

    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPowerUp ispOutWidth=%d ispOutHeight=%d",ispOutWidth,ispOutHeight);
    camera_PowerOn(SensorFrameSize, ispOutWidth, ispOutHeight, 0);

    if(g_sensorbuf == InvalideAddress || g_jpegbuf == InvalideAddressU8)
    {
        camera_PowerOff();
        mmc_MemFreeAll();
        UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPowerUp no memory!");
        return  0;
    }

    if(camera_GetId()== 255)
    {
        UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPowerUp ID ERROR!");
        camera_PowerOff();
        mmc_MemFreeAll();
        return 0;
    }
    //initialize clear feature flag
    g_clear_feature_flag = 0;

    return 1;
}

VOID uvideos_CameraPowerdown (VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPowerdown");
    camera_PowerOff();
    mmc_MemFreeAll();
}

INT32 uvideos_CameraPreview(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraPreview");

    uvideos_ImageEncodeInit();
    camera_SetBrightnessL(4);

    camera_PrepareImageCaptureL(0, SensorFrameSize, g_sensorbuf);
    camera_CaptureImage();

    return 1;
}

INT32 uvideos_CameraStopPreview(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraStopPreview");
    vpp_AudioJpegEncClose();

    return 1;
}

VOID uvideos_CameraCapture(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraCapture");

    g_still_capture = TRUE;

    uvideos_ImageEncodeInit();

    camera_PrepareImageCaptureL(0, SensorFrameSize, g_sensorbuf);
    camera_CaptureImage();
}

VOID uvideos_CameraExitCapture(VOID)
{
    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "uvideos_CameraExitCapture");
    g_still_capture = FALSE;

    vpp_AudioJpegEncClose();
}

VOID uvideos_RegisterCallback(UVIDEOS_CALLBACK_T callback)
{
    uvideos_Send_Msg = callback;
}

VOID uvideos_TransportTask(VOID* param)
{
    Msg_t* msg;
    UINT32 evt[4];
    while(1)
    {
        msg = sxr_Wait(evt, g_uvideosMbx);
        switch(msg->B)
        {
            case UVIDEOS_MSG_START_PREVIEW:
                uvideos_CameraPowerUp();
                uvideos_CameraPreview();
                break;
            case UVIDEOS_MSG_FRAME_READY:
                uvideos_ImageEncode();
                break;
            case UVIDEOS_MSG_ENCODE_DONE:
                uvideos_ImageEncodeDone();
                break;
            case UVIDEOS_MSG_SEND_VIDEO_DATA:
                uvideos_VideoSend();
                break;
            default :
                break;
        }
    }
}

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T*
uvideos_StreamOpen(CONST UVIDEOS_STREAM_PARAMETERS_T*  cfg)
{
    HAL_USB_EP_DESCRIPTOR_T** epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T**)
             sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T*)*3);

    memset(g_UsbBulkIn, 0, sizeof(g_UsbBulkIn));

    if(g_uvideosIdTask == 0)
    {
        g_uvideosTask.TaskBody  = uvideos_TransportTask;
        g_uvideosTask.TaskExit  = 0;
        g_uvideosTask.Name      = "UVIDEOS TRANSPORT";
        g_uvideosTask.StackSize = 512;
        g_uvideosTask.Priority  = 51;
        g_uvideosMbx            = sxr_NewMailBox();
        g_uvideosIdTask         = sxr_NewTask(&g_uvideosTask);
        sxr_StartTask(g_uvideosIdTask, 0);
    }

    g_uvideosBoInEp  = HAL_USB_EP_DIRECTION_IN (cfg->inEpNum );

    epList[0] = uctls_NewBulkEpDescriptor(g_uvideosBoInEp, videosCallbackEp);
    epList[1] = 0;

    UVIDEOS_TRACE(UVIDEOS_INFO_TRC, 0, "UVIDEOS Stream Open\n");
    return uctls_NewInterfaceDescriptor(0x0e, 0x02, 0x00, 0x01,
                                        epList, 0, videosStreamCallback);
}
PRIVATE VOID uvideos_StreamClose(VOID)
{
    if(g_uvideosIdTask != 0)
    {
        sxr_StopTask(g_uvideosIdTask);
        sxr_FreeTask(g_uvideosIdTask);
        sxr_FreeMailBox(g_uvideosMbx);
    }
    g_uvideosIdTask = 0;
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UVIDEOS_STREAM_CALLBACK_T g_uvideosStreamCallback =
{
    .open  = uvideos_StreamOpen,
    .close = uvideos_StreamClose
};

