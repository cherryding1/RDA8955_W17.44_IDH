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

#ifndef COOL_PROFILE_SPC_PROFILE_CODES_H
#define COOL_PROFILE_SPC_PROFILE_CODES_H


#define CP_spc_RxProcess 0x4000
#define CP_spc_CCHRxProcess 0x4001
#define CP_spc_TCHRxProcess 0x4002
#define CP_spc_CheckLoopMode 0x4003
#define CP_spc_UpdateSpeechDec 0x4004
#define CP_spc_BfiFilter 0x4005
#define CP_spc_SetFacch 0x4006
#define CP_spc_AmrDecode 0x4007
#define CP_spc_AmrRatscch 0x4008
#define CP_spc_FcchProcess 0x4009
#define CP_spc_TxProcess 0x400A
#define CP_spc_CCHTxProcess 0x400B
#define CP_spc_TCHTxProcess 0x400C
#define CP_spc_RACHTxProcess 0x400D
#define CP_spc_UpdateSpeechEnc 0x400E
#define CP_spc_DataFacch 0x400F
#define CP_spc_DataEncode 0x4010
#define CP_spc_AmrEncode 0x4011
#define CP_spc_UpdateOnFint 0x4012
#define CP_spc_UpdateAmrCfg 0x4013
#define CP_spc_ResetAudio 0x4014
#define CP_spc_ResetCodec 0x4015
#define CP_spc_AudioProcess 0x4016
#define CP_spc_AudioSpkUpd 0x4017
#define CP_spc_Debug 0x4018
#define CP_spc_EgprsCCHRxProcess 0x4019
#define CP_spc_EgprsCCHTxProcess 0x401A
#define CP_spc_RxProcessExt 0x401D
#define CP_spc_EgprsBlockDecode 0x401E
#define CP_BB_RX_WIN_MONIT 0x6F10
#define CP_BB_RX_WIN_FCCH 0x6F11
#define CP_BB_RX_WIN_SCH 0x6F12
#define CP_BB_RX_WIN_NBURST 0x6F13
#define CP_BB_RX_WIN_INTRF 0x6F14
#define CP_BB_RX_WIN_UNKNOWN 0x6F15
#define CP_DSP_SCH_DECODING_COMPLETE 0x6F20
#define CP_DSP_PCH_READY 0x6F21
#define CP_DSP_USF_READY 0x6F22
#define CP_DSP_EQUALIZATION_COMPLETE 0x6F23
#define CP_DSP_TASK_COMPLETE 0x6F24
#define CP_DSP_NB_DECODING_COMPLETE 0x6F25
#define CP_BCPU_RUNNING 0x7700
#define CP_FRAME 0x7701
#define CP_RF_IFC 0x7702
#define CP_MAILBOX 0x7703
#define CP_FCCH 0x7704
#define CP_SCH 0x7705
#define CP_NBURST 0x7706
#define CP_MONITORING 0x7707
#define CP_INTERF 0x7708
#define CP_DEBUG 0x7709
#define CP_SPC_SCHED_HOOK 0x770A
#define CP_TCU0 0x770B
#define CP_TCU1 0x770C



#endif
