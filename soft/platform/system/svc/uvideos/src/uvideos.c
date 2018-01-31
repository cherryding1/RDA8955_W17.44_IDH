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

#include "uvideos_m.h"
#include "uctls_m.h"
#include "uctls_callback.h"
#include "sxr_ops.h"
#include "string.h"
#include "hal_usb_descriptor.h"
#include "camera_m.h"


PUBLIC CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T g_uvideosIad =
{
    .bLength = sizeof g_uvideosIad,
    .bDescriptorType = 0x0b,//USB_DT_INTERFACE_ASSOCIATION
    .bFirstInterface = 0x00,
    .bInterfaceCount = 0x02,
    .bFunctionClass = 0x0e,//CC_VIDEO
    .bFunctionSubClass = 0x03,//SC_VIDEO_INTERFACE_COLLECTION
    .bFunctionProtocol = 0x00,//PC_PROTOCOL_UNDEFINED
    .iFunction  = 4,//STRING_INTERFACE
};

/*Scriptor length*/
#define USBVIDEO_VC_INTERFACE_LENGTH                13
#define USBVIDEO_VC_CT_LENGTH                       17
#define USBVIDEO_VC_OT_LENGTH                       9
#define USBVIDEO_VC_PU_LENGTH                       11

#define USBVIDEO_VS_INTERFACE_LENGTH                14
#define USBVIDEO_VS_MJPEG_FORMAT_LENGTH         11
#define USBVIDEO_VS_MJPEG_FRAME_LENGTH          38
#define USBVIDEO_VS_STILL_LENGTH                    6
#define USBVIDEO_VS_COLOR_MATCHING_LENGTH       6

EXPORT CAM_SIZE_T SensorFrameSize;
EXPORT UINT16 SensorFrameWidth;
EXPORT UINT16 SensorFrameHeight;

/***********************************************
    video class specific descriptor structure
************************************************/
/* class specific VC interface descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bcdUSB[2];
    UINT8    wTotalLength[2];
    UINT8    dwClockFrequency[4];
    UINT8    bInCollection;
    UINT8    baInterfaceNr;
} USBVideo_Class_VC_If_Dscr;

/* class specific VC input(camera) terminal descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bTerminalID;
    UINT8    wTerminalType[2];
    UINT8    bAssocTerminal;
    UINT8    iTerminal;
    UINT8    wObjectiveFocalLengthMin[2];
    UINT8    wObjectiveFocalLengthMax[2];
    UINT8    wOcularFocalLength[2];
    UINT8    bControlSize;
    UINT8    bmControls[2];
} USBVideo_CT_Dscr;

/* class specific VC output terminal descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bTerminalID;
    UINT8    wTerminalType[2];
    UINT8    bAssocTerminal;
    UINT8    bSourceID;
    UINT8    iTerminal;
} USBVideo_OT_Dscr;

/* class specific VC processing unit descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bUintID;
    UINT8    bSourceID;
    UINT8    wMaxMultiplier[2];
    UINT8    bControlSize;
    UINT8    bmControls[2];
    UINT8    iProcessing;
    //UINT8    bmVideoStandards;
} USBVideo_PU_Dscr;

/* class specific VS interface descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bNumFormats;
    UINT8    wTotalLength[2];
    UINT8    bEndpointAddress;
    UINT8    bmInfo;
    UINT8    bTerminalLink;
    UINT8    bStillCaptureMethod;
    UINT8    bTriggerSupport;
    UINT8    bTriggerUsage;
    UINT8    bControlSize;
    UINT8    bmaControls;
} USBVideo_Class_VS_If_Dscr;

/* MJPEG video format descriptor */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bFormatIndex;
    UINT8    bNumFrameDescriptors;
    UINT8    bmFlags;
    UINT8    bDefaultFrameIndex;
    UINT8    bAspectRatioX;
    UINT8    bAspectRatioY;
    UINT8    bmInterlaceFlags;
    UINT8    bCopyProtect;
} USBVideo_MJPEG_Format_Dscr;

/* MJPEG video frame descriptor */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bFrameIndex;
    UINT8    bmCapabilities;
    UINT16    wWidth;
    UINT16    wHeight;
    UINT32    dwMinBitRate;
    UINT32    dwMaxBitRate;
    UINT32    dwMaxVideoFrameBufferSize;
    UINT32    dwDefaultFrameInterval;
    UINT8    bFrameIntervalType;
    UINT32    dwMinFrameInterval;
    UINT32    dwMaxFrameInterval;
    UINT32    dwFrameIntervalStep;
} __attribute__((packed))USBVideo_MJPEG_Frame_Dscr;

/* still size structure */
/* Note that the UINT8[2] array can not modify to kal_uint16
    It is bacause align issue */
typedef struct
{
    UINT8  wWidth[2];
    UINT8  wHeight[2];
} USBVideo_Still_Size;

/* Still image frame descriptor */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bEndpointAddress;
    UINT8    bNumImageSizePatterns;
    USBVideo_Still_Size *p_image_patterns;
    UINT8    bNumCompressionPattern;
    UINT8    *p_compression_patterns;
} USBVideo_Still_Frame_Dscr;

/* Color matching descriptor */
typedef struct
{
    UINT8    bLength;
    UINT8    bDescriptorType;
    UINT8    bDescriptorSubType;
    UINT8    bColorPrimaries;
    UINT8    bTransferCharacteristics;
    UINT8    bMatrixCoefficients;
} USBVideo_Color_Dscr;

//Class-specific VS Frame Descriptor
struct usb_cs_VS_frame_descriptor
{
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bDescriptorSubtype;

    UINT8 bFrameIndex;
    UINT8 bmCapabilities;
    UINT16 wWidth;
    UINT16 wHeight;
    UINT32 dwMinBitRate;
    UINT32 dwMaxBitRate;
    UINT32 dwMaxVideoFrameBufSize;
    UINT32 dwDefaultFrameInterval;
    UINT8 bFrameIntervalType;
    UINT32 dwMinFrameInterval;
    UINT32 dwMaxFrameInterval;
    UINT32 dwFrameIntervalStep;
} __attribute__ ((packed));


#define VS_FRAME_MJPEG 0x07
static struct usb_cs_VS_frame_descriptor
    web_cam_cs_VS_frame1 =
{
#if 1
    .bLength = sizeof web_cam_cs_VS_frame1,
    .bDescriptorType = 0x24,
    .bDescriptorSubtype = VS_FRAME_MJPEG,
    .bFrameIndex = 0x01, //First (and only) format descriptor
    .bmCapabilities = 0x00, //Still images using capture method 1 are supported at this frame setting.D1: Fixed frame-rate.
    .wWidth = 0x00a0, //Width of frame is 160 pixels
    .wHeight= 0x0078, //Height of frame is 120 pixels.
    .dwMinBitRate = 0x0000947c, //Min bit rate in bits/s
    .dwMaxBitRate = 0x000debfe, //Max bit rate in bits/s
    .dwMaxVideoFrameBufSize = 0x0000c000, //Maximum video or still frame size, in bytes.
    .dwDefaultFrameInterval = 0x0007a120,//Default frame interval is 666666ns (15fps).
    .bFrameIntervalType = 0x00, //Continuous frame interval
    .dwMinFrameInterval = 0x0007a120, //Minimum frame interval is 666666ns (15fps)
    .dwMaxFrameInterval = 0x0007a120, //Maximum frame interval is 666666ns (15fps).
    .dwFrameIntervalStep = 0x00000000,//No frame interval step supported.
#else
    0x26,0x24,0x07,0x01,0x00,0xa0,0x00,0x78,0x00,0x7c,0x94,0x00,0x00,0xfe,
    0xeb,0x0d,0x00,0x00,0xc0,0x00,0x00,0x20,0xa1,0x07,0x00,0x00,0x20,0xa1,
    0x07,0x00,0x20,0xa1,0x07,0x00,0x00,0x00,0x00,0x00
#endif

};

static struct usb_cs_VS_frame_descriptor
    web_cam_cs_VS_frame2=
{
#if 1
    .bLength = sizeof web_cam_cs_VS_frame2,
    .bDescriptorType = 0x24,
    .bDescriptorSubtype = VS_FRAME_MJPEG,

    .bFrameIndex = 0x02,
    .bmCapabilities = 0x00,
    .wWidth = 0x0140,//320
    .wHeight = 0x00f0,//240
    .dwMinBitRate = 0x00009480,
    .dwMaxBitRate = 0x000dec00,
    .dwMaxVideoFrameBufSize = 0x0000c000,
    .dwDefaultFrameInterval = 0x0007a120,
    .bFrameIntervalType = 0x00,
    .dwMinFrameInterval = 0x0007a120,
    .dwMaxFrameInterval = 0x0007a120,
    .dwFrameIntervalStep = 0x00000000,
#else
    0x26,0x24,0x07,0x02,0x00,0x40,0x01,0xf0,0x00,0x80,0x94,0x00,0x00,0x00,
    0xec,0x0d,0x00,0x00,0xc0,0x00,0x00,0x20,0xa1,0x07,0x00,0x00,0x20,0xa1,
    0x07,0x00,0x20,0xa1,0x07,0x00,0x00,0x00,0x00,0x00
#endif

};

struct usb_cs_VS_still_image_frame_descriptor
{
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bDescriptorSubtype;

    UINT8 bEndpointAddress;
    UINT8 bNumImageSizePatterns;
    UINT16 wWidth1;
    UINT16 wHeight1;
    UINT16 wWidth2;
    UINT16 wHeight2;
    UINT16 wWidth3;
    UINT16 wHeight3;
    UINT8 bNumCompression;
    UINT8 bCompression1;
    UINT8 bCompression2;

} __attribute__ ((packed));

static struct usb_cs_VS_still_image_frame_descriptor
    web_cam_cs_VS_frame_still_image =
{
#if 1
    .bLength = sizeof web_cam_cs_VS_frame_still_image,
    .bDescriptorType = 0x24,
    .bDescriptorSubtype = 0x03,

    .bEndpointAddress = 0x00,//If method 2 of still image capture is used, this field shall be set to zero
    .bNumImageSizePatterns = 0x03,//Number of Image Size patterns of this format:
    .wWidth1 = 0x00a0,
    .wHeight1 = 0x0078,
    .wWidth2 = 0x0140,
    .wHeight2 = 0x00f0,
    .wWidth3 = 0x0280,
    .wHeight3 = 0x01e0,
    .bNumCompression = 0x02,//Number of Compression pattern of this format:
    .bCompression1 = 0x03,
    .bCompression2 = 0x02,
#else
    0x14,0x24,0x03,0x00,0x03,0xa0,0x00,0x78,0x00,0x40,0x01,0xf0,0x00,0x80,
    0x02,0xe0,0x01,0x02,0x03,0x02
#endif
};


PRIVATE CONST UVIDEOS_CFG_T* g_uvideosConfig;

PRIVATE UINT32 uvideoVCdescConstructor(UINT8* buffer)
{
    UINT8* p_desc;
    UINT32 if_desc_index = 0;
    UINT32 total_size;

    /* class-specific VC interface descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_Class_VC_If_Dscr));
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bLength = USBVIDEO_VC_INTERFACE_LENGTH;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bDescriptorSubType = 0x01;//USBVIDEO_VC_HEADER;
    //((USBVideo_Class_VC_If_Dscr*)p_desc)->bcdUSB[0] = 0x10; /* video spec rev is 1.1 */
    //((USBVideo_Class_VC_If_Dscr*)p_desc)->bcdUSB[1] = 0x01;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bcdUSB[0] = 0x00; /* video spec rev is 1.0 */
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bcdUSB[1] = 0x01;
    total_size = sizeof(USBVideo_Class_VC_If_Dscr)+sizeof(USBVideo_CT_Dscr)+sizeof(USBVideo_OT_Dscr)+sizeof(USBVideo_PU_Dscr);
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->wTotalLength[0] = total_size&0xff;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->wTotalLength[1] = (total_size>>8)&0xff;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[0] = 1000&0xff;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[1] = (1000>>8)&0xff;
//  ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[2] = (USBVIDEO_CLOCK_FREQUENCY>>16)&0xff;
//  ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[3] = (USBVIDEO_CLOCK_FREQUENCY>>24)&0xff;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[2] = 0x00;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->dwClockFrequency[3] = 0x00;
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->bInCollection = 1; /* number of VS interface*/
    ((USBVideo_Class_VC_If_Dscr*)p_desc)->baInterfaceNr = 1; /* VS interface number */
    memcpy(&buffer[if_desc_index], p_desc, USBVIDEO_VC_INTERFACE_LENGTH);
    if_desc_index += USBVIDEO_VC_INTERFACE_LENGTH;
    sxr_Free(p_desc);

    /* input (camera) terminal descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_CT_Dscr));
    ((USBVideo_CT_Dscr*)p_desc)->bLength = USBVIDEO_VC_CT_LENGTH;
    ((USBVideo_CT_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_CT_Dscr*)p_desc)->bDescriptorSubType = 0x02;//USBVIDEO_VC_INPUT_TERMINAL;
    ((USBVideo_CT_Dscr*)p_desc)->bTerminalID = 1;//USBVIDEO_CT_ID;
    ((USBVideo_CT_Dscr*)p_desc)->wTerminalType[0] = 0x0201&0xff;
    ((USBVideo_CT_Dscr*)p_desc)->wTerminalType[1] = (0x0201>>8)&0xff;
    ((USBVideo_CT_Dscr*)p_desc)->bAssocTerminal = 0x0; /* no association*/
    ((USBVideo_CT_Dscr*)p_desc)->iTerminal = 2;
    ((USBVideo_CT_Dscr*)p_desc)->wObjectiveFocalLengthMin[0] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->wObjectiveFocalLengthMin[1] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->wObjectiveFocalLengthMax[0] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->wObjectiveFocalLengthMax[1] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->wOcularFocalLength[0] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->wOcularFocalLength[1] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->bControlSize = 0x2;
    ((USBVideo_CT_Dscr*)p_desc)->bmControls[0] = 0;
    ((USBVideo_CT_Dscr*)p_desc)->bmControls[1] = 0;
    memcpy(&buffer[if_desc_index ], p_desc, USBVIDEO_VC_CT_LENGTH);
    if_desc_index += USBVIDEO_VC_CT_LENGTH;
    sxr_Free(p_desc);

    /* output terminal descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_OT_Dscr));
    ((USBVideo_OT_Dscr*)p_desc)->bLength = USBVIDEO_VC_OT_LENGTH;
    ((USBVideo_OT_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_OT_Dscr*)p_desc)->bDescriptorSubType = 0x03;//USBVIDEO_VC_OUTPUT_TERMINAL;
    ((USBVideo_OT_Dscr*)p_desc)->bTerminalID = 2;//USBVIDEO_OT_ID;
    ((USBVideo_OT_Dscr*)p_desc)->wTerminalType[0] = 0x0101&0xff;
    ((USBVideo_OT_Dscr*)p_desc)->wTerminalType[1] = (0x0101>>8)&0xff;
    ((USBVideo_OT_Dscr*)p_desc)->bAssocTerminal = 0x0; /* no association*/
    ((USBVideo_OT_Dscr*)p_desc)->bSourceID = 3;//USBVIDEO_PU_ID;
    ((USBVideo_OT_Dscr*)p_desc)->iTerminal = 0;
    memcpy(&buffer[if_desc_index ], p_desc, USBVIDEO_VC_OT_LENGTH);
    if_desc_index += USBVIDEO_VC_OT_LENGTH;
    sxr_Free(p_desc);

    /* processing unit descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_PU_Dscr));
    ((USBVideo_PU_Dscr*)p_desc)->bLength = USBVIDEO_VC_PU_LENGTH;
    ((USBVideo_PU_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_PU_Dscr*)p_desc)->bDescriptorSubType = 0x05;//USBVIDEO_VC_PROCESSING_UNIT;
    ((USBVideo_PU_Dscr*)p_desc)->bUintID = 3;//USBVIDEO_PU_ID;
    ((USBVideo_PU_Dscr*)p_desc)->bSourceID = 1;//USBVIDEO_CT_ID;
    ((USBVideo_PU_Dscr*)p_desc)->wMaxMultiplier[0] = 0;
    ((USBVideo_PU_Dscr*)p_desc)->wMaxMultiplier[1] = 0;
    ((USBVideo_PU_Dscr*)p_desc)->bControlSize = 0x2;
    ((USBVideo_PU_Dscr*)p_desc)->bmControls[0] = 0;
    ((USBVideo_PU_Dscr*)p_desc)->bmControls[1] = 0;
    ((USBVideo_PU_Dscr*)p_desc)->iProcessing = 0;
    //((USBVideo_PU_Dscr*)p_desc)->bmVideoStandards = 0;  /* for 1.1 revision */

    memcpy(&buffer[if_desc_index ], p_desc, USBVIDEO_VC_PU_LENGTH);
    if_desc_index += USBVIDEO_VC_PU_LENGTH;
    sxr_Free(p_desc);

    return total_size;


}
PRIVATE UINT32 uvideoVSdescConstructor(UINT8* buffer)
{
    UINT8* p_desc;
    UINT32 if_desc_index = 0;
    UINT32 total_length;
    UINT8 video_size_num = 2;
    //UINT8 still_size_num = 3;
    //UINT8 compression_num = 2;

    /* class specific VS interface descriptor, input header descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_Class_VS_If_Dscr));
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bLength = 14;//USBVIDEO_VS_INTERFACE_LENGTH;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bDescriptorSubType = 0x01;//USBVIDEO_VS_INPUT_HEADER;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bNumFormats = 0x01; /* one format descriptor for MJPEG*/
    total_length = sizeof(USBVideo_Class_VS_If_Dscr)+sizeof(USBVideo_MJPEG_Format_Dscr)
                   +sizeof(web_cam_cs_VS_frame1)+sizeof(web_cam_cs_VS_frame2)//+(video_size_num*sizeof(USBVideo_MJPEG_Frame_Dscr))
                   +sizeof(web_cam_cs_VS_frame_still_image)//+USBVIDEO_VS_STILL_LENGTH + (sizeof(USBVideo_Still_Size)*still_size_num) + (sizeof(UINT8)*compression_num)
                   +sizeof(USBVideo_Color_Dscr);
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->wTotalLength[0] = total_length&0xff;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->wTotalLength[1] = (total_length>>8)&0xff;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bEndpointAddress = (0x80|2);//(USB_EP_DIR_IN|ep_tx_id);   /*InPipe*/
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bmInfo = 0; /* No dynamic format change support */
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bTerminalLink = 2;//USBVIDEO_OT_ID;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bStillCaptureMethod = 2;//USBVIDEO_STILL_CAPTURE_METHOD_2;
#ifdef __USBVIDEO_DEVICE_TRIGGER_SUPPORT__
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bTriggerSupport = 1;//USBVIDEO_HARDWARE_TRIGGER_SUPPORT;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bTriggerUsage = 0;//USBVIDEO_HARDWARE_TRIGGER_STILL;
#else
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bTriggerSupport = 0;//USBVIDEO_HARDWARE_TRIGGER_NOT_SUPPORT;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bTriggerUsage = 0;
#endif
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bControlSize = 1;
    ((USBVideo_Class_VS_If_Dscr*)p_desc)->bmaControls = 0;
    memcpy(&buffer[if_desc_index], p_desc, USBVIDEO_VS_INTERFACE_LENGTH);
    if_desc_index +=USBVIDEO_VS_INTERFACE_LENGTH;
    sxr_Free(p_desc);

    /* MJPEG video format descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_MJPEG_Format_Dscr));
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bLength = 11;//USBVIDEO_VS_MJPEG_FORMAT_LENGTH;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bDescriptorSubType = 0x06;//USBVIDEO_VS_FORMAT_MJPEG;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bFormatIndex = 1;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bNumFrameDescriptors = video_size_num;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bmFlags = 1;//USBVIDEO_FIXED_SIZE_SAMPLES;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bDefaultFrameIndex = 0x02;//(UINT8)g_USBVideo.camera_param->usbvideo_get_default_video_index();
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bAspectRatioX = 0;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bAspectRatioY = 0;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bmInterlaceFlags = 0;
    ((USBVideo_MJPEG_Format_Dscr*)p_desc)->bCopyProtect = 0;
    memcpy(&buffer[if_desc_index], p_desc, USBVIDEO_VS_MJPEG_FORMAT_LENGTH);
    if_desc_index += USBVIDEO_VS_MJPEG_FORMAT_LENGTH;
    sxr_Free(p_desc);
#if 0
    /* MJPEG frame descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_MJPEG_Frame_Dscr)*video_size_num);
    //frame1
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bLength = USBVIDEO_VS_MJPEG_FRAME_LENGTH;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bDescriptorSubType = 0x07;//USBVIDEO_VS_FRAME_MJPEG;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bFrameIndex = 0x01;//(UINT8)p_video_size_param->type;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bmCapabilities = 0;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->wWidth = 0x00a0; //Width of frame is 160 pixels
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->wHeight = 0x0078; //Height of frame is 120 pixels

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwMinBitRate = 0x0000947c; //Min bit rate in bits/s
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwMaxBitRate = 0x000debfe; //Max bit rate in bits/s

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwMaxVideoFrameBufferSize = 0x0000c000;

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwDefaultFrameInterval = 0x0007a120;

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->bFrameIntervalType = 0;
    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwMinFrameInterval = 0x0007a120;

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwMaxFrameInterval = 0x0007a120;

    ((USBVideo_MJPEG_Frame_Dscr*)p_desc)->dwFrameIntervalStep = 0x00000000;

    memcpy(&buffer[if_desc_index], p_desc, USBVIDEO_VS_MJPEG_FRAME_LENGTH);
    if_desc_index +=USBVIDEO_VS_MJPEG_FRAME_LENGTH;
    //TODO,add frame2
    sxr_Free(p_desc);
#endif
    /* MJPEG frame descriptor */
    if (SensorFrameSize == 0)
    {
        return 0;
    }
    else if(SensorFrameSize == CAM_NPIX_QVGA)
    {
        web_cam_cs_VS_frame1.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame1.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame2.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame2.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth1 = SensorFrameWidth;
        web_cam_cs_VS_frame_still_image.wHeight1 = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth2 = SensorFrameWidth>>1;
        web_cam_cs_VS_frame_still_image.wHeight2 = SensorFrameHeight>>1;
    }
    else if(SensorFrameSize == CAM_NPIX_VGA)
    {
        web_cam_cs_VS_frame1.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame1.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame2.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame2.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth1 = SensorFrameWidth;
        web_cam_cs_VS_frame_still_image.wHeight1 = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth2 = SensorFrameWidth>>1;
        web_cam_cs_VS_frame_still_image.wHeight2 = SensorFrameHeight>>1;
    }
    else
    {
        web_cam_cs_VS_frame1.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame1.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame2.wWidth = SensorFrameWidth;
        web_cam_cs_VS_frame2.wHeight = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth1 = SensorFrameWidth;
        web_cam_cs_VS_frame_still_image.wHeight1 = SensorFrameHeight;
        web_cam_cs_VS_frame_still_image.wWidth2 = SensorFrameWidth>>1;
        web_cam_cs_VS_frame_still_image.wHeight2 = SensorFrameHeight>>1;
    }

    memcpy(&buffer[if_desc_index], &web_cam_cs_VS_frame1, sizeof web_cam_cs_VS_frame1);
    if_desc_index += sizeof web_cam_cs_VS_frame1;

    memcpy(&buffer[if_desc_index], &web_cam_cs_VS_frame2, sizeof web_cam_cs_VS_frame2);
    if_desc_index += sizeof web_cam_cs_VS_frame2;

    /* still image frame descriptor */
    memcpy(&buffer[if_desc_index], &web_cam_cs_VS_frame_still_image, sizeof web_cam_cs_VS_frame_still_image);
    if_desc_index += sizeof web_cam_cs_VS_frame_still_image;

    /* color matching descriptor */
    p_desc = sxr_Malloc(sizeof(USBVideo_Color_Dscr));
    ((USBVideo_Color_Dscr*)p_desc)->bLength = USBVIDEO_VS_COLOR_MATCHING_LENGTH;
    ((USBVideo_Color_Dscr*)p_desc)->bDescriptorType = 0x24;//USBVIDEO_CS_INTERFACE;
    ((USBVideo_Color_Dscr*)p_desc)->bDescriptorSubType = 0x0d;//USBVIDEO_VS_COLORFORMAT;
    ((USBVideo_Color_Dscr*)p_desc)->bColorPrimaries = 0;
    ((USBVideo_Color_Dscr*)p_desc)->bTransferCharacteristics = 0;
    ((USBVideo_Color_Dscr*)p_desc)->bMatrixCoefficients = 0;
    memcpy(&buffer[if_desc_index], p_desc, USBVIDEO_VS_COLOR_MATCHING_LENGTH);
    if_desc_index += USBVIDEO_VS_COLOR_MATCHING_LENGTH;
    sxr_Free(p_desc);

    return total_length;

}

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T** uvideos_Open(CONST UVIDEOS_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceDesc;

    if(config == 0)
    {
        return 0;
    }

    interfaceDesc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*5);

    g_uvideosConfig = config;

    //Open Control itf
    interfaceDesc[0] = config->controlCfg.controlCallback
                       ->open(&config->controlCfg.controlParameters);
    //Open other VC desc
    interfaceDesc[1] = (HAL_USB_INTERFACE_DESCRIPTOR_T*)hal_UsbDescriptorNewCSInterfaceDescriptor(uvideoVCdescConstructor, 0);
    //Open Stream itf
    interfaceDesc[2] = config->streamCfg.streamCallback
                       ->open(&config->streamCfg.streamParameters);
    //Open other VS desc
    interfaceDesc[3] = (HAL_USB_INTERFACE_DESCRIPTOR_T*)hal_UsbDescriptorNewCSInterfaceDescriptor(uvideoVSdescConstructor, 0);

    interfaceDesc[4] = 0;

    return(interfaceDesc);
}

PRIVATE VOID uvideos_Close(VOID)
{
    // Close Control
    g_uvideosConfig->controlCfg.controlCallback->close();
    // Close Stream
    g_uvideosConfig->streamCfg.streamCallback->close();

}

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_uvideosCallback =
{
    .open  = (HAL_USB_INTERFACE_DESCRIPTOR_T** (*)
    (CONST UCTLS_SERVICE_CFG_T*))                 uvideos_Open,
    .close =                                      uvideos_Close
};

