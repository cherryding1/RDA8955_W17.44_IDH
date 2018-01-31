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

#ifndef _AT_CMD_ATA_H_
#define _AT_CMD_ATA_H_

VOID AT_ATA_CmdFunc_KEY(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_KEYHOLD(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_TCARD(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_LcdBackLightOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_LcdBackLightOff(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_KeyPadBackLightOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_KeyPadBackLightOff(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_LcdOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_LcdOff(AT_CMD_PARA *pParam);
#ifdef __MMI_FLASHLIGHT__
VOID AT_ATA_CmdFunc_FlashLightOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_FlashLightOff(AT_CMD_PARA *pParam);
#endif
VOID AT_ATA_CmdFunc_VibratorOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_VibratorOff(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_StartFM(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_StopFM(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_CameraCapture(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_GetSWVersion(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_GetCalibFlag(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_Keypad(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_Voltag(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_ChargeOn(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_ChargeOff(AT_CMD_PARA *pParam);
VOID AT_ATA_CmdFunc_Start(AT_CMD_PARA *pParam);
#ifdef __MMI_BT_SUPPORT__
VOID AT_ATA_CmdFunc_BTST(AT_CMD_PARA* pParam);
VOID AT_ATA_CmdFunc_BTST2(AT_CMD_PARA* pParam);
VOID AT_ATA_CmdFunc_BTIN(AT_CMD_PARA* pParam);
VOID AT_ATA_CmdFunc_BTSH(AT_CMD_PARA* pParam);
VOID AT_ATA_CmdFunc_PAIR(AT_CMD_PARA* pParam);
#endif
VOID AT_AUDIO_CmdFunc_MIC(AT_CMD_PARA *pParam);
VOID AT_AUDIO_CmdFunc_SPEAKER(AT_CMD_PARA *pParam);
VOID AT_AUDIO_CmdFunc_RECEIVER(AT_CMD_PARA *pParam);
VOID AT_AUDIO_CmdFunc_HEADSET(AT_CMD_PARA *pParam);
#endif

