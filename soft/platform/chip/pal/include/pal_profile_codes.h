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

#ifndef COOL_PROFILE_PAL_PROFILE_CODES_H
#define COOL_PROFILE_PAL_PROFILE_CODES_H


#define CP_pal_FrameStart 0x0001
#define CP_pal_FrameEnd 0x0002
#define CP_pal_TimeTuning 0x0003
#define CP_pal_SynchronizationChange 0x0004
#define CP_pal_FrequencyTuning 0x0005
#define CP_pal_SetGsmCounters 0x0006
#define CP_pal_RxBufferCfg 0x0007
#define CP_pal_SetMonRxWin 0x0008
#define CP_pal_GetMonResult 0x0009
#define CP_pal_SetFchRxWin 0x000A
#define CP_pal_CloseFchRxWin 0x000B
#define CP_pal_GetFchResult 0x000C
#define CP_pal_SetSchRxWin 0x000D
#define CP_pal_GetSchResult 0x000E
#define CP_pal_SetIntRxWin 0x000F
#define CP_pal_GetIntResult 0x0010
#define CP_pal_SetNBurstRxWin 0x0011
#define CP_pal_GetNBurstResult 0x0012
#define CP_pal_GetNBlockResult 0x0013
#define CP_pal_GetUsf 0x0014
#define CP_pal_ABurstEncoding 0x0015
#define CP_pal_NblockEncoding 0x0016
#define CP_pal_SetTxWin 0x0017
#define CP_pal_WinResize 0x0018
#define CP_pal_SetTxWin_calib 0x0019
#define CP_pal_StartCiphering 0x001A
#define CP_pal_StopCiphering 0x001B
#define CP_pal_GprsCiphering 0x001C
#define CP_pal_StartTch 0x001D
#define CP_pal_ChangeTchMode 0x001E
#define CP_pal_RetrieveCurrentAMRCfg 0x001F
#define CP_pal_SetDtx 0x0020
#define CP_pal_GetDtx 0x0021
#define CP_pal_SetTchLoop 0x0022
#define CP_pal_StopTch 0x0023
#define CP_pal_FintSuspend 0x0024
#define CP_pal_FintResume 0x0025
#define CP_pal_GetElapsedTime 0x0026
#define CP_pal_RfWakeUp 0x0027
#define CP_pal_RfSleep 0x0028
#define CP_pal_ResourceActive 0x0029
#define CP_pal_ResourceInactive 0x002A
#define CP_pal_Sleep 0x002B
#define CP_pal_SetUsrVector 0x002C
#define CP_pal_SetUsrVectorMsk 0x002D
#define CP_pal_SleepDeep 0x002E
#define CP_pal_Debug 0x0030
#define CP_pal_Initialisation 0x00FF
#define CP_PAL_FRAME 0x3730
#define CP_pal_WinDebug 0x3731
#define CP_SkipFrameAllowed 0x3732



#endif
