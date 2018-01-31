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

#include <cswtype.h>
#include <errorcode.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <cos.h>
#include <ts.h>
#include <string.h>

#include "cfw_emod_tsm.h"
#include "dm_audio.h"
#include <drv_prv.h>
#include <dm.h>
#include <pm.h>

#include "chip_id.h"

#include "tgt_m.h"
#include "memd_m.h"
#include "aud_m.h"
#include "csw_csp.h"
#include "calib_m.h"
#include "pal_imei.h"
#include "vois_m.h"

#ifndef RFM_FLASH_WDB
#define RFM_FLASH_WDB    0x02
#endif


extern HAL_AIF_STREAM_T audioStream;
extern AUD_LEVEL_T        audio_cfg;
extern AUD_ITF_T        audioItf;

static  UINT8 AudioMod;

//BOOL FS_Format(const unsigned char *image_name);




BOOL CFW_EmodGetBatteryInfo(CFW_EMOD_BATTERY_INFO *pBatInfo )
{
    UINT32 result = 0x00000000;
    SUL_MemSet8(pBatInfo, 0xff, SIZEOF(CFW_EMOD_BATTERY_INFO));
    result = PM_GetBatteryState((PM_BATTERY_INFO *)(&pBatInfo->nBatState));
    if(result)
    {
        result = PM_GetBatteryInfo((UINT8 *)(&pBatInfo->nChargeInfo.nBcs), (UINT8 *)(&pBatInfo->nChargeInfo.nBcl), (UINT16 *)(&pBatInfo->nChargeInfo.nMpc));
        if(result)
        {
            result = PM_BatteryChemistry((UINT8 *)&pBatInfo->nChemistry);
            //if(result)
        }
        else
            return result;
    }
    else
        return result;
    return TRUE;
}


void CFW_EmodAudioTestStart(UINT8 type)
{
    audioStream.startAddress = NULL;
    audioStream.length = 0;
    audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    audioStream.channelNb = HAL_AIF_MONO;
    audioStream.voiceQuality = TRUE;
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler = NULL;
    audioStream.endHandler = NULL;

    DM_StopTone();

    switch(audioItf)
    {
        case AUD_ITF_RECEIVER:
            AudioMod = DM_AUDIO_MODE_EARPIECE;
            break;
        case AUD_ITF_EAR_PIECE:
            AudioMod = DM_AUDIO_MODE_HANDSET;
            break;
        case AUD_ITF_LOUD_SPEAKER:
            AudioMod = DM_AUDIO_MODE_LOUDSPEAKER;
            break;
        default:
            AudioMod = DM_AUDIO_MODE_EARPIECE;
            break;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_EMOD_AudioTestStart()  AudioMod=%d!\n",0x081006d3)), AudioMod);
    switch(type)
    {
        case 0: // AUD_MODE_NORMAL
            audio_cfg.micLevel = AUD_MIC_ENABLE;
#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
            audio_cfg.spkLevel = AUD_SPK_VOL_15;                //wangrui modify for fixbug10475 20081202
#else
            audio_cfg.spkLevel = AUD_SPK_VOL_7;
#endif
            audio_cfg.sideLevel = AUD_SIDE_VOL_TEST;        //wangrui modify for fixbug10475 20081202
            DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
            break;

        case 1: // AUD_MODE_HEADSET
            audio_cfg.micLevel = AUD_MIC_ENABLE;
#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
            audio_cfg.spkLevel = AUD_SPK_VOL_15;                //wangrui modify for fixbug10475 20081202
#else
            audio_cfg.spkLevel = AUD_SPK_VOL_7;
#endif
            audio_cfg.sideLevel = AUD_SIDE_VOL_TEST;                //wangrui modify for fixbug10475 20081202
            DM_SetAudioMode(DM_AUDIO_MODE_HANDSET);
            break;

        case 2: // AUD_MODE_LOUDSPK
            audio_cfg.micLevel = AUD_MIC_ENABLE;
#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
            audio_cfg.spkLevel = AUD_SPK_VOL_12;
            audio_cfg.sideLevel = AUD_SIDE_VOL_14;
#else
            audio_cfg.spkLevel = AUD_SPK_VOL_4;
            audio_cfg.sideLevel = AUD_SIDE_VOL_6;
#endif
            DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
            break;

        default:
            break;
    }

    COS_Sleep(200);

    vois_TestModeSetup(audioItf, &audioStream, &audio_cfg, AUD_TEST_SIDE_TEST);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EMOD_AudioTestStart()  success!\n",0x081006d4)));

}

void CFW_EmodAudioTestEnd(void)
{
    audio_cfg.sideLevel = AUD_SIDE_MUTE;
    vois_TestModeSetup(audioItf, &audioStream, &audio_cfg, AUD_TEST_NO);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_EMOD_AudioTestEnd()  AudioMod=%d!\n",0x081006d5)), AudioMod);
    //  while(!DM_SetAudioMode(AudioMod));
    //  The return value of this function is always true. it is not necessary to call it in a loop;
    DM_SetAudioMode(AudioMod);
    COS_Sleep(200);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EMOD_AudioTestEnd()  success!\n",0x081006d6)));
}
extern BOOL g_tgtFactorySettingIsDirty ;

BOOL CFW_EmodClearUserInfo(void)
{

    // Todo
    return FALSE;

}

BOOL CFW_EmodSaveIMEI(UINT8 *pImei, UINT8 imeiLen, UINT8 nSimID)
{
    UINT8 nChar[9];
    UINT8 digit1, digit2;
    UINT32 i, j;
    if(nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_EmodGetIMEI()\r\n",0x081006d7)));
        return FALSE;
    }

    if (imeiLen != 15) return FALSE;

    // Construct IMEISV
    // TAC+SNR starts from the high 4 bits of nChar[0]
    digit2 = pImei[0] - '0';
    if (digit2 > 9) return FALSE;
    nChar[0] = digit2 << 4 | 0x03;
    for (i = 1; i + 1 < imeiLen; i += 2)
    {
        digit1 = pImei[i] - '0';
        digit2 = pImei[i + 1] - '0';
        if (digit1 > 9 || digit2 > 9) return FALSE;
        j = (i + 1) >> 1;
        nChar[j] = digit2 << 4 | digit1;
    }
#if 1
    // The first digit of SVN is re-used to save the last digit (the check digit) of IMEI,
    // which is not part of IMEISV
#else
    // The first digit of SVN is set to 0
    nChar[7] = (nChar[7] & 0x0f) | 0x00;
#endif
    // The second digit of SVN is set to 0
    nChar[8] = 0xf0;

    // Update factory settings
    BOOL result = pal_SaveFactoryImei(nSimID, nChar);
    return result;

}

VOID CFW_EmodGetIMEIBCD(UINT8 *pImei, UINT8 *pImeiLen, CFW_SIM_ID nSimID)
{
    UINT8 nChar[9];
    UINT8 *pChar;
    BOOL ret;

    if(nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_EmodGetIMEIBCD()\n",0x081006d8)));
        return ;
    }

    ret = pal_GetFactoryImei((UINT8)nSimID,(UINT8*)nChar);

    if (ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTXT(TSTR("Got IMEI-BCD: %08x%08x%02x\n",0x081006d9)),
                  (nChar[0] << 24) | (nChar[1] << 16) | (nChar[2] << 8) | nChar[3],
                  (nChar[4] << 24) | (nChar[5] << 16) | (nChar[6] << 8) | nChar[7],
                  nChar[8]);
        SUL_MemCopy8(pImei, nChar, 9);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("No valid IMEI-BCD\n",0x081006da)));
        SUL_MemSet8(pImei, 0, 9);
    }
    *pImeiLen = 9;
}

VOID CFW_EmodGetIMEI(UINT8 *pImei, UINT8 *pImeiLen, CFW_SIM_ID nSimID)
{
    UINT8 nChar[9];
    UINT8 len;
    UINT8 i;

    CFW_EmodGetIMEIBCD(nChar, &len, nSimID);
    CSW_ASSERT(len == sizeof(nChar));

    pImei[0] = ((nChar[0] & 0xf0) >> 4) + '0';
    for(i = 1; i < 8; i++)
    {
        pImei[2 * i - 1] = (nChar[i] & 0x0f) + '0';
        pImei[2 * i] = ((nChar[i] & 0xf0) >> 4) + '0';
    }
    *pImeiLen = 15;
}

void CFW_EmodReadBindSIM(CFW_EMOD_READ_BIND_SIM *pReadBindSim)
{
    UINT8 Restora[BIND_INFO_SIZE];
    UINT8 i = 0;
    UINT8 nNum = 0;
    UINT8 nCounter = 0;
    UINT32 result;
    BIND_SIM_NM *temp = NULL;
    CFW_EMOD_BIND_SIM *pBindSim = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodReadBindSIM()  1!\n",0x081006db)));
    temp = (BIND_SIM_NM *)CSW_SIM_MALLOC(SIZEOF(BIND_SIM_NM));
    result = Cfg_Emod_ReadSIMNm(temp);
    SUL_MemCopy8(Restora + 1, temp->simNo, 90);
    CSW_SIM_FREE(temp);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodReadBindSIM()  2!\n",0x081006dc)));
    if(result == MEMD_ERR_NO)
    {
        for(i = 0; i < 10; i++)
        {
            if(Restora[BIND_SIMNO + i * 9] != 0xff)
            {
                nNum++;
            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_EmodReadBindSIM()  number=%d!\n",0x081006dd)), nNum);
        if(nNum == 0)
            pReadBindSim->nNumber = 0;
        else
        {
            pBindSim = (CFW_EMOD_BIND_SIM *)CSW_TMS_MALLOC(nNum * (SIZEOF(CFW_EMOD_BIND_SIM)));
            for(i = 0; i < 10; i++)
            {
                if(Restora[BIND_SIMNO + i * 9] != 0xff)
                {
                    Emod_IMSItoASC(Restora + BIND_SIMNO + i * 9, pBindSim->pImsiNum, &pBindSim->nImsiLen);
                    pBindSim->nIndex = i + 1;
                    nCounter++;
                    pBindSim = (CFW_EMOD_BIND_SIM *)(&pBindSim->nIndex + 20);
                }
            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodReadBindSIM()  3!\n",0x081006de)));
        pReadBindSim->nNumber = nNum;
        pReadBindSim->nPointer = (UINT32)&pBindSim->nIndex - 20 * nCounter;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodReadBindSIM()  4!\n",0x081006df)));

}

void CFW_EmodReadBindNW(CFW_EMOD_READ_BIND_NW *pReadBindNw)
{
    UINT8 Restora[BIND_INFO_SIZE];
    UINT8 i = 0;
    UINT8 nNum = 0;
    UINT8 nCounter = 0;
    UINT32 result;
    BIND_NW_NM *temp = NULL;
    CFW_EMOD_BIND_NW *pBindNw = NULL;


    temp = (BIND_NW_NM *)CSW_SIM_MALLOC(SIZEOF(BIND_NW_NM));
    result = Cfg_Emod_ReadNWNm(temp);
    SUL_MemCopy8(Restora + 92, temp->nwNo, 15);
    CSW_SIM_FREE(temp);
    if(result == MEMD_ERR_NO)
    {
        for(i = 0; i < 5; i++)
        {
            if(Restora[BIND_NWNO + i * 3] != 0xff)
            {
                nNum++;
            }
        }
        if(nNum == 0)
            pReadBindNw->nNumber = 0;
        else
        {
            pBindNw = (CFW_EMOD_BIND_NW *)CSW_TMS_MALLOC(nNum * (SIZEOF(CFW_EMOD_BIND_NW)));
            for(i = 0; i < 5; i++)
            {
                if(Restora[BIND_NWNO + i * 3] != 0xff)
                {
                    SUL_MemCopy8(pBindNw->pNccNum, Restora + BIND_NWNO + i * 3, 3);
                    pBindNw->nIndex = i + 1;
                    nCounter++;
                    pBindNw = (CFW_EMOD_BIND_NW *)(&pBindNw->nIndex + 4);
                }
            }
        }
        pReadBindNw->nNumber = nNum;
        pReadBindNw->nPointer = (UINT32)&pBindNw->nIndex - 4 * nCounter;
    }
}

UINT32 CFW_EmodSetBindInfo(BOOL bSim, BOOL bNetWork)
{

    UINT32 ret;
    ret = Cfg_Emod_UpdateSign(bSim, bNetWork);
    if(ret == ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("success=%x.\n",0x081006e0)), ret);
        return(ERR_SUCCESS);
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("failure=%x..\n",0x081006e1)), ret);
        return ERR_CMS_TEMPORARY_FAILURE;
    }


}

void CFW_EmodGetBindInfo(BOOL *bSim, BOOL *bNetWork)
{

    if (ERR_SUCCESS == Cfg_Emod_ReadSign(bSim, bNetWork))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EMOD_GetBindInfo sucessful.\n",0x081006e2)));
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_EMOD_GetBindInfo sucessful and the bsim=%d,bNetWork=%d.\n",0x081006e3)), *bSim, *bNetWork);
    }
}

//*********************************************************************************
//discreption: this function used to bind sim to this phone
//---------------------------------------------------------------------------------
//params:   nIndex[in],         when pBindSimNum == null, range:1~10, delete an exist one
//                          when pBindSimNum != null,  0 means find a free location to save
//                                                  range1~10, means updata an exist one.
//                          can bind 10 sim cards totally.
//---------------------------------------------------------------------------------
//          pBindSimNum[in],    value of sim number, if this pointer is null, function will init this sim
//              number field according to the index. otherwise we will updata the sim number field.
//------------------------------------------------------------------------------------
//          nLen[in],           length of pBindSimNum. range 0~15
//*********************************************************************************
CFW_EMOD_UPDATE_RETURN CFW_EmodUpdateSimBind(UINT8 nIndex, UINT8 *pBindSimNum, UINT8 nLen)
{
    CFW_EMOD_UPDATE_RETURN nReturn;
    UINT32 j;
    UINT8   nIMSITemp[9];
    UINT8 simnm[90];
    nReturn.nErrCode = 0;
    nReturn.nIndex = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@11.\n",0x081006e4)));

    if(pBindSimNum == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@delete start .\n",0x081006e5)));
        if((nIndex < 1) || (nIndex > 10))
        {
            nReturn.nErrCode = ERR_INVALID_INDEX;
            return nReturn;
        }
        else
        {
            Cfg_Emod_ReadSIMNm((BIND_SIM_NM *)simnm);
            SUL_MemSet8(simnm + (nIndex - 1) * 9, 0xFF, 9);

            if(ERR_SUCCESS != Cfg_Emod_UpdateSIMNm((BIND_SIM_NM *)simnm))
            {
                nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
                return nReturn;
            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@delete end .\n",0x081006e6)));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@12.\n",0x081006e7)));
        if(nLen > 15)
        {
            nReturn.nErrCode = ERR_CFW_INVALID_PARAMETER;
            return nReturn;
        }

        Emod_ASCtoIMSI(pBindSimNum, nIMSITemp, nLen);

        if( ERR_SUCCESS != Cfg_Emod_ReadSIMNm((BIND_SIM_NM *)simnm))
        {
            nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
            return nReturn;
        }

        if(nIndex == 0)
        {
            j = 0;
            while((simnm[j] != 0xff) && (nReturn.nIndex < 10))
            {
                j += 9;
                nReturn.nIndex++;
            }
            if(nReturn.nIndex < 10)
            {
                SUL_MemCopy8(simnm + 9 * nReturn.nIndex, nIMSITemp, 9);
                nReturn.nIndex++;//hameina[+] 2007.7.26, output index range from 1~10

            }
            else if(nReturn.nIndex == 10)
            {
                UINT8 temp[81];
                SUL_MemCopy8(temp, simnm + 9, 81);
                SUL_MemCopy8(simnm, temp, 81);
                SUL_MemCopy8(simnm + 81, nIMSITemp, 9);

                nReturn.nIndex = 10;
            }

        }
        else
        {
            if((nIndex < 1) || ((nIndex > 10)))
            {
                nReturn.nErrCode = ERR_INVALID_INDEX;
                return nReturn;
            }
            else
            {
                SUL_MemCopy8(simnm + 9 * (nIndex - 1), nIMSITemp, 9);
                nReturn.nIndex = nIndex;

            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@13.\n",0x081006e8)));
        if(ERR_SUCCESS != Cfg_Emod_UpdateSIMNm((BIND_SIM_NM *)simnm))
        {
            nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
            return nReturn;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@14.\n",0x081006e9)));
    }
    return nReturn;

}

CFW_EMOD_UPDATE_RETURN CFW_EmodUpdateNwBind(UINT8 nIndex, UINT8 *pBindNwNum, UINT8 nLen)
{
    UINT32 j;
    CFW_EMOD_UPDATE_RETURN nReturn;
    UINT8 nwnm[15];
    nReturn.nErrCode = 0;
    nReturn.nIndex = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@11.\n",0x081006ea)));
    if(pBindNwNum == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@delete start .\n",0x081006eb)));
        if((nIndex < 1) || (nIndex > 5))
        {
            nReturn.nErrCode = ERR_INVALID_INDEX;
            return nReturn;
        }
        else
        {
            Cfg_Emod_ReadNWNm((BIND_NW_NM *)nwnm);
            SUL_MemSet8(nwnm + (nIndex - 1) * 3, 0xFF, 3);

            if(ERR_SUCCESS != Cfg_Emod_UpdateNWNm((BIND_NW_NM *)nwnm))
            {
                nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
                return nReturn;
            }

        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@delete end .\n",0x081006ec)));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@12.\n",0x081006ed)));
        if(nLen > 3)
        {
            nReturn.nErrCode = ERR_CFW_INVALID_PARAMETER;
            return nReturn;
        }

        if( ERR_SUCCESS != Cfg_Emod_ReadNWNm((BIND_NW_NM *)nwnm))
        {
            nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
            return nReturn;
        }

        if(nIndex == 0)
        {
            j = 0;
            while((nwnm[j] != 0xff) && (nReturn.nIndex < 5))
            {
                j += 3;
                nReturn.nIndex++;
            }
            if(nReturn.nIndex < 5)
            {
                SUL_MemCopy8(nwnm + 3 * nReturn.nIndex, pBindNwNum, 3);
                nReturn.nIndex += 1; //qiao+07.11.1 return logic index , which is increased one base on physical index

            }
            else if(nReturn.nIndex == 5)
            {
                UINT8 temp[12];
                SUL_MemCopy8(temp, nwnm + 3, 12);
                SUL_MemCopy8(nwnm, temp, 12);
                SUL_MemCopy8(nwnm + 12, pBindNwNum, 3);

                nReturn.nIndex = 5;
            }

        }
        else
        {
            if((nIndex < 1) || ((nIndex > 5)))
            {
                nReturn.nErrCode = ERR_INVALID_INDEX;
                return nReturn;
            }
            else
            {
                SUL_MemCopy8(nwnm + 3 * (nIndex - 1), pBindNwNum, nLen);
                nReturn.nIndex = nIndex;

            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@13.\n",0x081006ee)));
        if(ERR_SUCCESS != Cfg_Emod_UpdateNWNm((BIND_NW_NM *)nwnm))
        {
            nReturn.nErrCode = ERR_CMS_TEMPORARY_FAILURE;
            return nReturn;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("@@@@@@@@@@@14.\n",0x081006ef)));
    }
    return nReturn;
}

void CFW_EmodGetProductInfo(CFW_EMOD_LOCT_PRODUCT_INFO *pProduct)
{
#if 0
    UINT32 i = 0, j = 0;
    TGT_MOBILE_SN_T mobileSn;
    tgt_GetMobileSn(&mobileSn);
    while(i < 64)
    {
        if(mobileSn.mbSn[i] == 0xff)
        {
            pProduct->nBoardSNLen = i;
            SUL_MemCopy8(pProduct->nBoardSN, mobileSn.mbSn, i);
            i = 64;

        }
        i++;
    }

    while(j < 64)
    {
        if(mobileSn.mpSn[j] == 0xff)
        {
            pProduct->nPhoneSNLen = j;
            SUL_MemCopy8(pProduct->nPhoneSN, mobileSn.mpSn, j);
            j = 64;

        }
        j++;
    }
#else
    pProduct->nBoardSNLen = sizeof(pProduct->nBoardSN);
    tgt_GetMobileBoardSn(pProduct->nBoardSN, &pProduct->nBoardSNLen);
    pProduct->nPhoneSNLen = sizeof(pProduct->nPhoneSN);
    tgt_GetMobilePhoneSn(pProduct->nPhoneSN, &pProduct->nPhoneSNLen);
#endif
}

void CFW_EmodGetRFCalibrationInfo(CFW_EMOD_RF_CALIB_INFO *pRfCalib)
{
#if !(defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955))
    UINT32 size;
    CT_calibration_t *calib_p = NULL;
    calib_p = calib_GetPointers();
    size = SIZEOF(CT_calib_pcl2dbm_arfcn_g_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_pcl2dbm_arfcn_g_t LEN=%d.\n",0x081006f0)), size);
    size = SIZEOF(CT_calib_rf_chip_name_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_rf_chip_name_t LEN=%d.\n",0x081006f1)), size);
    size = SIZEOF(CT_calib_padac_profile_interp_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_profile_interp_t LEN=%d.\n",0x081006f2)), size);
    size = SIZEOF(CT_calib_padac_profile_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_profile_t LEN=%d.\n",0x081006f3)), size);
    size = SIZEOF(CT_calib_padac_profile_extrem_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_profile_extrem_t LEN=%d.\n",0x081006f4)), size);
    size = SIZEOF(CT_calib_padac_ramp_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_ramp_t LEN=%d.\n",0x081006f5)), size);
    size = SIZEOF(CT_calib_padac_ramp_swap_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_ramp_swap_t LEN=%d.\n",0x081006f6)), size);
    size = SIZEOF(CT_calib_padac_low_volt_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_low_volt_t LEN=%d.\n",0x081006f7)), size);
    size = SIZEOF(CT_calib_padac_low_dac_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_padac_low_dac_t LEN=%d.\n",0x081006f8)), size);
    size = SIZEOF(CT_calib_pa_t);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calib_pa_t LEN=%d.\n",0x081006f9)), size);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" ############CT_calibration=%x.\n",0x081006fa)), &calib_p->code_version);

    SUL_MemCopy8(pRfCalib->nLowDacLimit, calib_p->pa->palcust.low_dac_limit, 12);

    SUL_MemCopy8(pRfCalib->nLowVoltLimit, calib_p->pa->palcust.low_volt_limit, 12);

    SUL_MemCopy8(&pRfCalib->nName, &calib_p->pa->palcust.name, 4);

    SUL_MemCopy8(pRfCalib->nPcl2dbmArfcn_d, calib_p->pa->palcust.pcl2dbm_arfcn_d, 60);

    SUL_MemCopy8(pRfCalib->nPcl2dbmArfcn_g, calib_p->pa->palcust.pcl2dbm_arfcn_g, 60);

    SUL_MemCopy8(pRfCalib->nPcl2dbmArfcn_p, calib_p->pa->palcust.pcl2dbm_arfcn_p, 60);

    pRfCalib->nProfiledBmMax_dp = calib_p->pa->palcust.profile_dBm_max_dp;

    pRfCalib->nProfiledBmMax_g = calib_p->pa->palcust.profile_dBm_max_g;

    pRfCalib->nProfiledBmMin_dp = calib_p->pa->palcust.profile_dBm_min_dp;

    pRfCalib->nProfiledBmMin_g = calib_p->pa->palcust.profile_dBm_min_g;

    //  SUL_MemCopy8(pRfCalib->nProfile_dp,calib_p->pa->palcust.profile_dp, 2*1024);

    SUL_MemCopy8(pRfCalib->nProfileInterp_g, calib_p->pa->palcust.profile_interp_dp, 2 * 16);

    //  SUL_MemCopy8(pRfCalib->nProfile_g,calib_p->pa->palcust.profile_g, 2*1024);

    SUL_MemCopy8(pRfCalib->nProfileInterp_g, calib_p->pa->palcust.profile_interp_g, 2 * 16);

    SUL_MemCopy8(pRfCalib->nRampHigh, calib_p->pa->palcust.ramp_high, 2 * 32);


    SUL_MemCopy8(pRfCalib->nRampLow, calib_p->pa->palcust.ramp_low, 2 * 32);

    pRfCalib->nRampSwap_dp = calib_p->pa->palcust.ramp_swap_dp;

    pRfCalib->nRampSwap_g = calib_p->pa->palcust.ramp_swap_g;


    SUL_MemCopy8(pRfCalib->nRaram, calib_p->pa->param, 4 * 15);


    SUL_MemCopy8(pRfCalib->nTimes, calib_p->pa->times, 2 * 15);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" ############CT_calibration end.\n",0x081006fb)));
#endif
}

VOID Emod_IMSItoASC(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen)
{
    UINT8 i;
    OutPut[0] = ((InPut[1] & 0xF0 ) >> 4) + 0x30;
    for (i = 2; i < InPut[0] + 1 ; i++)
    {
        OutPut[2 * (i - 1) - 1] = (InPut[i] & 0x0F) + 0x30;
        OutPut[2 * (i - 1)]   = ((InPut[i] & 0xF0 ) >> 4) + 0x30;
    }

    *OutLen =  2 * InPut[0] - 1;
}

VOID Emod_ASCtoIMSI(UINT8 *InPut, UINT8 *OutPut, UINT8 InLen)
{
    UINT8 i;
    OutPut[0] = (InLen + 1) / 2;
    OutPut[1] = ((InPut[0] & 0x0f ) << 4) + 0x01;
    for (i = 2; i < OutPut[0] + 1; i++)
    {
        OutPut[i] = (InPut[2 * (i - 2) + 1] & 0x0f) + ((InPut[2 * (i - 1)] & 0x0f ) << 4);
    }
    if((OutPut[0] + 1) < 9)
    {
        for(i = (OutPut[0] + 1); i < 10; i++)
            OutPut[i] = 0xff;
    }
}




/* -------------------------------------------------- */
/* ----- XCV identifiers. */

enum
{
    DEFAULT_CALIB_XCV_AERO1PLUS = CT_CALIB_XCV_MASK,
    DEFAULT_CALIB_XCV_AERO2,
    DEFAULT_CALIB_XCV_SKY74117,
    DEFAULT_CALIB_XCV_RDA6210,
    DEFAULT_CALIB_XCV_QTY
};

/* -------------------------------------------------- */
/* ----- XCV names. */

#define DEFAULT_CALIB_XCV_NAMES_STR     "XCV SiLab Aero 1 Plus", \
                                        "XCV SiLab Aero 2", \
                                        "XCV Skyworks 74117", \
                                        "XCV RDA 6210"

/* -------------------------------------------------- */
/* ----- PA identifiers. */

enum
{
    DEFAULT_CALIB_PA_AWT6146 = CT_CALIB_PA_MASK,
    DEFAULT_CALIB_PA_RF7115,
    DEFAULT_CALIB_PA_TQM6M4002,
    DEFAULT_CALIB_PA_SKY77506,
    DEFAULT_CALIB_PA_SKY77318,
    DEFAULT_CALIB_PA_TQM6M4002_RDA,
    DEFAULT_CALIB_PA_RF3166,
    DEFAULT_CALIB_PA_QTY
};

/* -------------------------------------------------- */
/* ----- PA names. */

#define DEFAULT_CALIB_PA_NAMES_STR      "PA Anadigics AWT6146", \
                                        "PA RFMD RF71115", \
                                        "PA Triquint TQM6M4002", \
                                        "PA Skyworks 77506", \
                                        "PA Skyworks 77318", \
                                        "PA Triquint TQM6M4002_RDA", \
                                        "PA RFMD RF3166"

/* -------------------------------------------------- */
/* ----- SW identifiers. */

enum
{
    DEFAULT_CALIB_SW_LMSP7CHA = CT_CALIB_SW_MASK,
    DEFAULT_CALIB_SW_RF7115,
    DEFAULT_CALIB_SW_TQM6M4002,
    DEFAULT_CALIB_SW_SKY77506,
    DEFAULT_CALIB_SW_ASM4518806T,
    DEFAULT_CALIB_SW_TQM6M4002_RDA,
    DEFAULT_CALIB_SW_ASWMF46KAAACT,
    DEFAULT_CALIB_SW_ATB4532GC3T0,
    DEFAULT_CALIB_SW_LMSP33QA,
    DEFAULT_CALIB_SW_LRT613S5,
    DEFAULT_CALIB_SW_QTY
};

/* -------------------------------------------------- */
/* ----- SW names. */

#define DEFAULT_CALIB_SW_NAMES_STR      "SW LMSP7CHA", \
                                        "SW RFMD RF71115", \
                                        "SW Triquint TQM6M4002", \
                                        "SW Skyworks 77506", \
                                        "SW TDK ASM4518806t", \
                                        "SW Triquint TQM6M4002_RDA", \
                                        "SW SANYO ASWMF46KAAACT", \
                                        "SW PILKOR ATB4532GC3T0", \
                                        "SW MURATA LMSP33QA", \
                                        "SW KYOCERA LRT613S5"

/*
================================================================================
  Function   : CFW_EmodGetSoftVersion
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/

VOID CFW_EmodGetSoftVersion( UINT8 nMoudle, Soft_Version *pSV )
{
    // FIXME Implement
}


/*
================================================================================
  Function   : CFW_EmodGetHWVersion
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/

VOID CFW_EmodGetHWVersion( UINT8 nMoudle, HW_Version *pHWV )
{
}

UINT32 CFW_EmodGetBaseBandVersion(void)
{
    return hal_SysGetChipId(CFW_SYS_CHIP_PROD_ID);
}
