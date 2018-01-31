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

#include "at.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cfg.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "key_defs.h"
#include "hal_pwm.h"
#include "hal_key.h"
#include "camera_m.h"
#include "hal_host.h"
#include "hal_gouda.h"
#include "cpu_share.h"
#include "mmc_camera.h"

#ifdef AT_CAMERA_SUPPORT

volatile BOOL gCamCaptureFlag = FALSE;
volatile BOOL gCamPowerOnFlag = FALSE;

U8 *gCamBuffer = NULL;
U32 gCamBufferSize = 0;

#define AT_CAM_WIDTH 128
#define AT_CAM_HEIGHT 160

#define AT_IMAGE_WIDTH 480
#define AT_IMAGE_HEIGHT 640

#define ERROR_RETURN(pResult, reason, nDLCI)           \
    do                                                 \
    {                                                  \
        pResult = AT_CreateRC(CMD_FUNC_FAIL,           \
                              CMD_RC_ERROR,            \
                              reason,                  \
                              CMD_ERROR_CODE_TYPE_CME, \
                              0,                       \
                              NULL,                    \
                              0,                       \
                              nDLCI);                  \
        AT_Notify2ATM(pResult, nDLCI);                 \
        if (pResult != NULL)                           \
        {                                              \
            AT_FREE(pResult);                          \
            pResult = NULL;                            \
        }                                              \
        return;                                        \
    \
} while (0)
#define OK_RETURN(pResult, pString, nDLCI)             \
    do                                                 \
    {                                                  \
        pResult = AT_CreateRC(CMD_FUNC_SUCC,           \
                              CMD_RC_OK,               \
                              CMD_ERROR_CODE_OK,       \
                              CMD_ERROR_CODE_TYPE_CME, \
                              0,                       \
                              pString,                 \
                              strlen(pString),         \
                              nDLCI);                  \
        AT_Notify2ATM(pResult, nDLCI);                 \
        if (pResult != NULL)                           \
        {                                              \
            AT_FREE(pResult);                          \
            pResult = NULL;                            \
        }                                              \
        return;                                        \
    \
} while (0)

VOID AT_Cam_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)

{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

VOID AT_Cam_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

#define TFLASH_ROOT "/t"
#define TFLASH_ROOT_PATH "/t/"

static INT AT_INIT_TCard(void)

{
    INT32 iRet = 0;
    UINT8 tflash[20] = {0};
    INT i = 0;
    AnsiiToUnicodeString(tflash, TFLASH_ROOT);

    iRet = FS_MakeDir(tflash, 0);

    if (iRet != ERR_SUCCESS)
    {
        if (iRet != ERR_FS_FILE_EXIST)
        {
            AT_TC(g_sw_SPEC, "[MMI_InitTFlash]tfalsh dir isn't exist ! create dir & mount failed! ");
            return;
        }
        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] tfalsh dir has exist ! ");
    }

    iRet = FS_HasFormatted("mmc0", FS_TYPE_FAT);

    switch (iRet)
    {
    case ERR_FS_OPEN_DEV_FAILED: //打不开设备（卡没插上）
        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] Can't open device(Tflash card)!. ");
        return 0;

    case ERR_FS_UNKNOWN_FILESYSTEM: //不支持的文件系统，需要格式化
        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] ERR_FS_UNKNOWN_FILESYSTEM : File system is not supported.");
        return 1;

    case ERR_FS_NOT_SUPPORT: //不支持的文件系统，需要格式化
        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] ERR_FS_NOT_SUPPORT  : not supported fat32.");
        return 1;

    case ERR_FS_NOT_FORMAT: //卡没格式化，需要格式化
        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] ERR_FS_NOT_FORMAT : No format!");
        return 1;

    case ERR_FS_HAS_FORMATED:

        AT_TC(g_sw_SPEC, "[MMI_InitTFlash] tfalsh FS_HAS_FORMATED,"
                         " g_tflashCardExist =%d",
              1);

        for (i = 0; i < 3; i++)
        {
            AT_TC(g_sw_SPEC, "[MMI_InitTFlash] mount tflash card >>>>>>%d<<<<<<", i + 1);

            iRet = FS_Mount("mmc0", tflash, 0, FS_TYPE_FAT);

            if (iRet == ERR_SUCCESS)
            {
                AT_TC(g_sw_SPEC, "[MMI_InitTFlash] tflash dir has mounted successfully !"
                                 " g_hasMountedTFlashFS= %d",
                      1);

                break;
            }
        }

        return 1;

    default:
        AT_TC(g_sw_SPEC, "mount tfalsh : unknown Error!");
        break;
    }

    return 0;
}

void savefiletoTcard()
{
    INT32 iFd = -1;
    UINT16 uONSize = 0;
    UINT32 iResult = ERR_SUCCESS;
    UINT8 g_recName[80] = {
        0,
    };
    static INT32 iTcardRet = 0;
    // mount T card
    if (TRUE != iTcardRet)
    {
        AT_TC(g_sw_SPEC, "+CAM : Tcard mount \n", iTcardRet);
        iTcardRet = AT_INIT_TCard();
    }

    // Create the print file.
    if (TRUE == iTcardRet)
    {
        // Make the print file name with path.
        INT32 uiULen = 0;
        UINT8 g_szTestResultFile[60] = {
            0,
        };
        UINT8 *recName = NULL;
        TM_SYSTEMTIME sSystemTime;
        TM_GetSystemTime(&sSystemTime);
        SUL_StrPrint(g_szTestResultFile, "/t/%d-%d-%d-%d-%d-%d.jpg",
                     sSystemTime.uYear,
                     sSystemTime.uMonth,
                     sSystemTime.uDay,
                     sSystemTime.uHour,
                     sSystemTime.uMinute,
                     sSystemTime.uSecond);
        AnsiiToUnicodeString(g_recName, g_szTestResultFile);
        if (ERR_SUCCESS == iResult)
        {
            AT_TC(g_sw_GC, "record file open [%s] (%d)", g_szTestResultFile, uiULen);

            iFd = FS_Open(g_recName, (FS_O_RDWR | FS_O_CREAT), 0);

            if (iFd < 0)
            {
                AT_TC(g_sw_GC, "[%s]openfailing.h=%d", g_szTestResultFile, iFd);
                iFd = FS_Create((PCSTR)g_recName, 0);
            }

            iResult = MCI_CamSavePhoto(iFd);
            FS_Close(iFd);
            AT_TC(g_sw_SPEC, "FS_Write = %d.", iResult);
        }
        else
        {
            AT_TC(g_sw_SPEC, "Local2Unicode fail.err code = %d.", iResult);
        }
    }
    else
    {
        MCI_CamSavePhoto(-1);
        AT_TC(g_sw_SPEC, "+CAM :need mount Tcard\n");
    }

    AT_TC(g_sw_GC, "================");
}
UINT32 AT_hex2ascii(UINT8 *pInput, UINT32 nInputLen, UINT8 *pOutput)
{
    UINT32 i;
    UINT8 *pBuffer = pOutput;
    for (i = 0; i < nInputLen; i++)
    {
        UINT8 high4bits = pInput[i] >> 4;
        UINT8 low4bits = pInput[i] & 0x0F;

        if (high4bits < 0x0A)
            *pOutput++ = high4bits + '0'; // 0 - 9
        else                              // 0x0A - 0x0F
            *pOutput++ = high4bits - 0x0A + 'A';

        if (low4bits < 0x0A)
            *pOutput++ = low4bits + '0'; // 0 - 0x09
        else                             // 0x0A - 0x0F
            *pOutput++ = low4bits - 0x0A + 'A';
    }
    return pOutput - pBuffer;
}

extern UINT16 uart_SendDataToTe(UINT8 *pBuf, UINT16 len);

VOID AT_read_cambuffer(UINT8 *pInput, UINT32 nInputLen)
{
    if (gCamBuffer)
    {
        AT_FREE(gCamBuffer);
    }
    //gCamBuffer = (UINT8 *)AT_MALLOC(nInputLen * 2);
    gCamBuffer = (UINT8 *)AT_MALLOC(nInputLen);
    if (gCamBuffer)
    {
        //gCamBufferSize = AT_hex2ascii(pInput, nInputLen, gCamBuffer);
        gCamBufferSize = nInputLen;
        memcpy(gCamBuffer, pInput, gCamBufferSize);
    }
}

//========================================================
//VOID AT_EMOD_CmdFunc_CTTS(AT_CMD_PARA* pParam)
//Get ip address,without parameter
//========================================================
extern BOOL camera_ImageIsConvert(VOID);

VOID AT_EMOD_CmdFunc_Cam(AT_CMD_PARA *pParam)
{
    INT32 iRet = ERR_SUCCESS;
    PAT_CMD_RESULT pResult;
    UINT8 camId = 0xff;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 nOutputStr[16] = {
        0,
    };
    if (NULL == pParam)
    {
        goto Cam_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto Cam_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_TEST:
            SUL_StrCat(nOutputStr, "+cam: (0-3)");
            AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_READ:
            if (gCamPowerOnFlag)
            {
                SUL_StrPrint(nOutputStr, "+cam: %d", 1);
            }
            else
            {
                SUL_StrPrint(nOutputStr, "+cam: %d", 0);
            }
            AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            return;
        case AT_CMD_SET:
        {
            UINT8 uParaCount;
            UINT8 mode;
            UINT32 nRet = ERR_SUCCESS;
            UINT16 uSize;
            UINT8 i = 0;
            UINT8 arrRes[100];
            UINT32 uStat = CFW_CcGetCallStatus(nSim);

            if (uStat != 0x00) //CC_STATE_NULL
            {
                AT_Sprintf(arrRes, "+CAM:in  calling %d. ", uStat);
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }

            iRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);

            if (ERR_SUCCESS != iRet)
                goto Cam_ERROR;

            if (uParaCount < 1)
                goto Cam_ERROR;

            uSize = 1;
            nRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);

            if (nRet != ERR_SUCCESS)
            {
                AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }

#ifdef AT_SOUND_RECORDER_SUPPORT
            AT_AUDIO_StopAll();
#endif

            if (mode == 0)
            {
                if (!gCamPowerOnFlag)
                {
                    UINT32 ret = 0xdd;
                    mci_LcdInit(0xFFFF);
                    lcdd_SetBrightness(7);
                    MCI_LcdSizeSet(AT_CAM_WIDTH, AT_CAM_HEIGHT);
                    camera_ImageIsConvert();
                    MCI_CamSetPara(CAM_PARAM_IMAGESIZE, 0x24);
                    set_mmc_camera_preview();
                    ret = MCI_CamPowerUp(0);
                    hal_HstSendEvent(0xf3f3f3f3);
                    hal_HstSendEvent(ret);
                    if (ret == 0)
                    {
                        CAM_PREVIEW_STRUCT preStru = {
                            0,
                        };
                        preStru.start_x = 0;
                        preStru.start_y = 0;
                        preStru.end_x = AT_CAM_WIDTH - 1;
                        preStru.end_y = AT_CAM_HEIGHT - 1;
                        preStru.image_width = AT_IMAGE_WIDTH;
                        preStru.image_height = AT_IMAGE_HEIGHT;

                        preStru.preview_rotate = 0;
                        preStru.nightmode = 0;
                        preStru.imageQuality = 1;
                        preStru.contrast = 0;
                        preStru.specialEffect = 0;
                        preStru.brightNess = 0;
                        preStru.banding = 0;
                        preStru.whiteBlance = 0;
                        preStru.exposure = 0;
                        preStru.addFrame = 0;
                        preStru.factor = 0;
                        preStru.flashenable = 0;
                        MCI_CamPreviewOpen(&preStru);
                        gCamCaptureFlag = FALSE;
                        gCamPowerOnFlag = TRUE;
                        gCamBuffer = NULL;
                        gCamBufferSize = 0;
                    }
                    else
                    {
                        gCamCaptureFlag = FALSE;
                        gCamPowerOnFlag = FALSE;
                        if (gCamBuffer)
                        {
                            AT_FREE(gCamBuffer);
                        }
                        gCamBuffer = NULL;
                        gCamBufferSize = 0;
                        lcdd_SetBrightness(0);
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                }
                else
                {
                    goto Cam_ERROR;
                }
            }
            else if (mode == 1)
            {
                if (gCamPowerOnFlag)
                {
                    CAM_PREVIEW_STRUCT data;
                    CSW_SYS_FREQ_T curr_sys_freq = 0;
                    data.image_width = AT_IMAGE_WIDTH;
                    data.image_height = AT_IMAGE_HEIGHT;
                    set_mmc_camera_preview();
                    //MCI_CamCapture(&data);
                    COS_Sleep(10);
                    MCI_CamPlayBack();

                    curr_sys_freq = csw_GetResourceActivity(CSW_LP_RESOURCE_UNUSED_1);
                    csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, CSW_SYS_FREQ_250M);
                    csw_SetVOCFreqActivity(CSW_VOC_FREQ_250M);
                    savefiletoTcard();
                    csw_SetVOCFreqActivity(CSW_VOC_FREQ_13M);
                    csw_SetResourceActivity(CSW_LP_RESOURCE_UNUSED_1, curr_sys_freq);
                    if (gCamBuffer == NULL)
                    {
                        gCamCaptureFlag = FALSE;
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                    gCamCaptureFlag = TRUE;
                }
                else
                {
                    goto Cam_ERROR;
                }
            }
            else if (mode == 2)
            {
                if (gCamCaptureFlag)
                {
                    U32 size = 0;
                    U32 size_add = 0x1000 >> 1;
                    U8 *sendbuffer = (UINT8 *)AT_MALLOC(0x1000);
                    if (sendbuffer)
                    {
                        COS_Sleep(2000);
                        /*  
                  while (gCamBufferSize - size)
                    {
                        if (gCamBufferSize > size)
                        {
                            U16 send_size = 0;
                            if (gCamBufferSize > (size + 0x1000))
                            {
                                send_size = 0x1000;
                                at_CmdRespInfoNText(pParam->engine,gCamBuffer + size, send_size);
                                COS_Sleep(10);
                                size += 0x1000;
                            }
                            else
                            {
                                send_size = gCamBufferSize - size;
                                at_CmdRespInfoNText(pParam->engine,gCamBuffer + size, send_size);
                                COS_Sleep(10);
                                size += gCamBufferSize - size;
                            }
                        }
                        else
                        {
                            size = gCamBufferSize;
                        }
                    }
                    */

                        while (gCamBufferSize - size)
                        {
                            if (gCamBufferSize > size)
                            {
                                U16 send_size = 0;
                                if (gCamBufferSize > (size + size_add))
                                {
                                    send_size = 0x1000;
                                    send_size = AT_hex2ascii(gCamBuffer + size, size_add, sendbuffer);
                                    at_CmdRespOutputNText(pParam->engine, sendbuffer, send_size);
                                    COS_Sleep(10);
                                    size += size_add;
                                }
                                else
                                {
                                    send_size = (gCamBufferSize - size) * 2;
                                    send_size = AT_hex2ascii(gCamBuffer + size, (gCamBufferSize - size), sendbuffer);
                                    at_CmdRespOutputNText(pParam->engine, sendbuffer, send_size);
                                    COS_Sleep(10);
                                    size += gCamBufferSize - size;
                                }
                            }
                            else
                            {
                                size = gCamBufferSize;
                            }
                        }

                        AT_FREE(sendbuffer);
                    }
                    else
                    {
                        if (gCamBuffer)
                        {
                            AT_FREE(gCamBuffer);
                        }
                        gCamBuffer = NULL;
                        gCamCaptureFlag = FALSE;
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
                    }
                }
                else
                {
                    goto Cam_ERROR;
                }
            }
            else if (mode == 3)
            {
                if (gCamPowerOnFlag)
                {
                    gCamCaptureFlag = FALSE;
                    gCamPowerOnFlag = FALSE;
                    if (gCamBuffer)
                    {
                        AT_FREE(gCamBuffer);
                    }
                    gCamBuffer = NULL;
                    gCamBufferSize = 0;
                    Lily_Camera_PowerOff();
                    lcdd_SetBrightness(0);
                    //lcdd_Close();
                }
                else
                {
                    goto Cam_ERROR;
                }
            }
            else
            {
                goto Cam_ERROR;
            }
            if (ERR_SUCCESS == nRet)
                AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 30, NULL, 0, pParam->nDLCI);
            else
                AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        default:
            break;
        }
    }

Cam_ERROR:
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}

#endif
