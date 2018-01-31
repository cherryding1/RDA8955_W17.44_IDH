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

#ifndef _UVIDEOS_STREAM_M_H_
#define _UVIDEOS_STREAM_M_H_

/* request code */
#define USBVIDEO_RC_UNDEFINED                       0x00
#define USBVIDEO_SET_CUR                                0x01
#define USBVIDEO_GET_CUR                                0x81
#define USBVIDEO_GET_MIN                                0x82
#define USBVIDEO_GET_MAX                                0x83
#define USBVIDEO_GET_RES                                0x84
#define USBVIDEO_GET_LEN                                0x85
#define USBVIDEO_GET_INFO                               0x86
#define USBVIDEO_GET_DEF                                0x87

/* videostreaming interface control selectors */
#define USBVIDEO_VS_US_CONTROL_UNDEFINED            0x0000
#define USBVIDEO_VS_PROBE_CONTROL                   0x0100
#define USBVIDEO_VS_COMMIT_CONTROL                  0x0200
#define USBVIDEO_VS_STILL_PROBE_CONTROL             0x0300
#define USBVIDEO_VS_STILL_COMMIT_CONTROL                0x0400
#define USBVIDEO_VS_STILL_IMAGE_TRIGGER_CONTROL     0x0500
#define USBVIDEO_VS_STREAM_ERROR_CODE_CONTROL       0x0600
#define USBVIDEO_VS_GENERATE_KEY_FRAME_CONTROL      0x0700
#define USBVIDEO_VS_UPDATE_FRAME_SEGMENT_CONTROL    0x0800
#define USBVIDEO_VS_SYNC_DELAY_CONTROL              0x0900

typedef enum
{
    /* msg between media and USB */
    UVIDEOS_MSG_MED_START_SUCCESS_CNF,
    UVIDEOS_MSG_MED_START_FAIL_CNF,
    UVIDEOS_MSG_MED_CHANGE_VIDEO_IND,
    UVIDEOS_MSG_MED_STILL_CAPTURE_IND,
    UVIDEOS_MSG_MED_START_IND,
    UVIDEOS_MSG_MED_DEVICE_STILL_CNF,
    UVIDEOS_MSG_MED_PAUSE_CNF,
    UVIDEOS_MSG_MED_RESUME_SUCCESS_CNF,
    UVIDEOS_MSG_MED_RESUME_FAIL_CNF,
    UVIDEOS_MSG_MED_STOP_IND,
    UVIDEOS_MSG_MED_STOP_CNF,
    UVIDEOS_MSG_MED_ABORT_IND,

    /* msg used in internal USB driver */
    UVIDEOS_MSG_USB_ENUM_DONE,
    UVIDEOS_MSG_USB_COMPLETE_VIDEO,
    UVIDEOS_MSG_USB_DMA_DONE,
    UVIDEOS_MSG_USB_CHANGE_SIZE,
    UVIDEOS_MSG_USB_STILL_CAPTURE,
    UVIDEOS_MSG_USB_COMPLETE_STILL,
    UVIDEOS_MSG_USB_STILL_DMA_DONE,
    UVIDEOS_MSG_USB_STILL_DMA_PARTIAL,
    UVIDEOS_MSG_USB_INCALL_DMA_DONE,
    UVIDEOS_MSG_USB_INCALL_TIMEOUT,
    UVIDEOS_MSG_USB_INCALL_CHANGE_SIZE,
    UVIDEOS_MSG_USB_ABORT
} UVIDEOS_MSG_TYPE;

typedef void (*UVIDEOS_CALLBACK_T)(UVIDEOS_MSG_TYPE uvideos_msg_type,UINT32 param1, UINT32 param2);

PUBLIC VOID uvideos_CameraGetInfo (VOID);
PUBLIC UINT32 uvideos_CameraPowerUp (VOID);
PUBLIC VOID uvideos_CameraPowerdown (VOID);
PUBLIC INT32 uvideos_CameraPreview(VOID);
PUBLIC INT32 uvideos_CameraStopPreview(VOID);
PUBLIC VOID uvideos_CameraCapture(VOID);
PUBLIC VOID uvideos_CameraExitCapture(VOID);
PUBLIC VOID uvideos_RegisterCallback(UVIDEOS_CALLBACK_T callback);

#endif //_UVIDEOS_STREAM_M_H_