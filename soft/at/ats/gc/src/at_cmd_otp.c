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
#include "at_module.h"
#include "at_cfg.h"
#include "at_cmd_gc.h"
#include "csw.h"
#if !defined(CHIP_HAS_AP) && !defined(__MODEM_NO_AP_)

typedef enum
{
    /// No error occured
    MEMD_ERR_NO = 0,

    /// An error occurred during the erase operation
    MEMD_ERR_ERASE = -10,

    /// An error occurred during the write operation
    MEMD_ERR_WRITE,

    /// This error occurs when a command requiring
    /// sector aligned addresses as parameters is called with unaligned addresses.
    /// (Those are
    /// #memd_FlashErrors memd_FlashErase, #memd_FlashLock and #memd_FlashUnlock)
    MEMD_ERR_ALIGN,

    /// An erase or write operation was attempted on a protected sector
    MEMD_ERR_PROTECT,
    /// erase suspend.
    MEMD_ERR_SUSPEND,
    /// device is busy.
    MEMD_ERR_BUSY,

} MEMD_ERR_T;

extern BOOL hal_HstSendEvent(UINT32 ch);
#define SECTOR_SIZE 4096
extern MEMD_ERR_T memd_FlashRead(UINT8 *flashAddress,
                                 UINT32  byteSize,
                                 UINT32* pReadByteSize,
                                 UINT8*  buffer);
extern MEMD_ERR_T memd_FlashWrite(UINT8 *flashAddress,
                                  UINT32 byteSize,
                                  UINT32 * pWrittenByteSize,
                                  CONST UINT8* buffer);
extern MEMD_ERR_T memd_FlashErase(UINT8 *startFlashAddress, UINT8 *endFlashAddress);
extern int memd_FlashCodeSectionOtp(UINT8 protect_section, BOOL otp_val);
#define hal_flash_Erase memd_FlashErase
#define hal_flash_Read memd_FlashRead
#define hal_flash_Write memd_FlashWrite
//
// AT+OTP= n ( 1 < n < 5)
// AT+OTP=n,1 ( 1 < n < 5)
VOID AT_GC_CmdFunc_OTP(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iCount   = 0;
    UINT16 iSize = 0;
    UINT8 protect_section = 0;
    UINT8 otp_val = 0;
    UINT8 opt_type = 0;
    BOOL botp_val = FALSE;
    UINT32 cri_status;
    UINT32 RSize = 0;
    UINT8* addr;
    UINT8* p;
    UINT8 is_dirty = 0;
    UINT32 i,j;
    UINT8* pbuff = NULL;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    //DBG_ASSERT(0,"OTP COMMAND");
    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }
    else
    {
        if (pParam->iType == AT_CMD_SET)
        {
            eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

            if (eResult == ERR_SUCCESS)
            {
                switch (iCount)
                {
                case 0:
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    hal_HstSendEvent( 0x22000001);
                    break;
                case 1:
                    hal_HstSendEvent( 0x22000002);
                    iSize = sizeof(UINT16);
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &opt_type, &iSize);
                    if(eResult != ERR_SUCCESS)
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        break;
                    }

                    if(opt_type >= 1)
                    {
                        pbuff = CSW_Malloc(SECTOR_SIZE*16);
                        cri_status = hal_SysEnterCriticalSection();
                        hal_HstSendEvent( 0x22000023);
                        for(i = 0; i < opt_type; i ++)
                        {
                            addr = (UINT8*)(i*SECTOR_SIZE*16);
                            eResult = hal_flash_Read(addr,SECTOR_SIZE*16,&RSize,pbuff);
                            if(eResult == ERR_SUCCESS)
                            {
                                hal_HstSendEvent( 0x22000024);
                                hal_HstSendEvent((UINT32)addr);
                            }
                            else
                            {
                                hal_HstSendEvent( 0x22000025);
                                hal_HstSendEvent((UINT32)addr);
                                continue;
                            }
                            eResult = hal_flash_Erase(addr,addr + SECTOR_SIZE*16);
                            if(eResult == ERR_SUCCESS)
                            {
                                hal_HstSendEvent( 0x22000026);
                                hal_HstSendEvent((UINT32)addr);
                            }
                            else
                            {
                                hal_HstSendEvent( 0x22000027);
                                hal_HstSendEvent((UINT32)addr);
                                continue;
                            }
                            is_dirty = 0;
                            p = (UINT8*) ((UINT32)addr |0x88000000);
                            while((((UINT32)p&0x00ffffff) - (UINT32)addr) < SECTOR_SIZE*16)
                            {
                                if(*p != 0xff)
                                {
                                    hal_HstSendEvent( 0x22000227);
                                    is_dirty = 1;
                                    break;
                                }
                                p++;
                            }
                            if(is_dirty == 1)
                            {
                                hal_HstSendEvent( 0x22002227);
                                continue;
                            }

                            eResult = hal_flash_Write(addr,SECTOR_SIZE*16,&RSize,pbuff);
                            if(eResult == ERR_SUCCESS)
                            {
                                hal_HstSendEvent( 0x22000028);
                                hal_HstSendEvent((UINT32)addr);
                            }
                            else
                            {
                                hal_HstSendEvent( 0x22000029);
                                hal_HstSendEvent((UINT32)addr);
                                continue;
                            }
                            p = (UINT8*) ((UINT32)addr |0x88000000);
                            j = 0;
                            while((((UINT32)p&0x00ffffff) - (UINT32)addr) < SECTOR_SIZE*16)
                            {
                                if(*p != pbuff[j])
                                {
                                    hal_HstSendEvent( 0x22000229);
                                    is_dirty = 1;
                                    break;
                                }
                                j++;
                                p++;
                            }
                            if(is_dirty == 1)
                            {
                                hal_HstSendEvent( 0x22002229);
                                continue;
                            }
                        }
                        hal_HstSendEvent( 0x2200002A);
                        if(pbuff)
                        {
                            CSW_Free(pbuff);
                        }
                        DM_Reset();
                        hal_SysExitCriticalSection(cri_status);
                    }
                    break;
                case 2:
                    iSize = sizeof(UINT16);
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &protect_section, &iSize);
                    if(eResult != ERR_SUCCESS)
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        break;
                    }
                    iSize = sizeof(UINT16);
                    eResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &otp_val, &iSize);
                    if(eResult != ERR_SUCCESS)
                    {
                        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        break;
                    }
                    botp_val = otp_val == 0 ? FALSE: TRUE;

                    eResult = memd_FlashCodeSectionOtp(protect_section,botp_val);
                    if(eResult == 0)
                    {
                        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
                    }
                    else
                    {
                        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                    break;

                default:
                    AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    break;
                }
            }
            else
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    return;
}

#else
VOID AT_GC_CmdFunc_OTP(AT_CMD_PARA *pParam)
{
    pParam = pParam;
}
#endif

// ///////////////////////////////////////////////////////////////////////////////
// The end of the file
// ///////////////////////////////////////////////////////////////////////////////

