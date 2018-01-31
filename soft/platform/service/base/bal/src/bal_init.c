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











#include "csw.h"
#include "stdkey.h"
#include "event.h"
#include "base_prv.h"
#include "drv_prv.h"
#include "csw_ver.h"
#include "csw_map.h"
#include "vds_api.h"
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#include "vds_cache.h"
#include "factory.h"
#endif
#include "drv_flash.h"
#include "fs.h"
#include "cfw_nv.h"
#include "chip_id.h"
#include "pmd_m.h"
#include "sxs_io.h"
#include "sxr_tksd.h"
#include "hal_host.h"
#include "hal_lps.h"
#include <fs/sf_api.h>

extern BOOL DRV_FlashInit(void);
extern VOID BAL_CFWApsTask (  VOID *pData);
extern VOID csw_MemIint() ;
extern BOOL PM_CheckValidPowerOnVoltage(UINT16 nVoltage);
extern HANDLE COS_CreateTask_Prv(PTASK_ENTRY pTaskEntry, PVOID pParameter, PVOID pStackAddr, UINT16 nStackSize,
                                 UINT8 nPriority, UINT16 nCreationFlags, UINT16 nTimeSlice, PCSTR pTaskName);
extern VOID BAL_SetMMIDefaultValue(VOID);
extern INT32 VDS_Init();
extern INT32 CFW_CfgInit();
extern INT32 SMS_DM_Init();
extern INT32 DSM_DevInit();
extern VOID fsTace_task_create();
extern UINT32 TM_PowerOnPatch();
extern VOID memd_Init(VOID);

#ifndef ONLY_MMI_SUPPORT
extern BOOL BAL_ATInit(VOID);
#endif
#ifdef WIFI_SUPPORT
extern VOID wifi_task_init(VOID);
#endif


#define MAX_BAL_TASK_NUM      4

HANDLE g_BalTasks[MAX_BAL_TASK_NUM];

MMI_Default_Value g_MMI_Default_Value;

VOLATILE BOOL g_BalFlashInitProgramDone = FALSE;


VOID CSW_CheckMMIDefaultValue(VOID)
{
    if((MINLEAST_VOLTAGE > g_MMI_Default_Value.nMinVol) || (MAXLEAST_VOLTAGE < g_MMI_Default_Value.nMinVol))
        g_MMI_Default_Value.nMinVol = DEFAULTLEAST_VOLTAGE ;
    if((MIN_MMI_MEMORY_SIZE > g_MMI_Default_Value.nMemorySize) || (MAX_MMI_MEMORY_SIZE < g_MMI_Default_Value.nMemorySize))
        g_MMI_Default_Value.nMemorySize = DEFAULT_MMI_MEMORY_SIZE ;
}

HANDLE BAL_TH(UINT8 pri)
{
    if(pri == COS_BK_TASK_PRI)
    {
        return g_BalTasks[3];
    }
    else
    {
        return g_BalTasks[BAL_TASK_NUM(pri)];
    }
}

BOOL BAL_TaskInit()
{
    UINT8 i = 0;
    INT32 err_code = 0x00;
    UINT32 fs_dev_count = 0;
    FS_DEV_INFO *fs_dev_info = NULL;
    UINT32 fs_root_dev_count = 0;

    BAL_InitSysFreq();

    csw_RegisterYourself();

    // hal_fastOSTimInit();
    //enable lps timer align to paging
#ifdef HAL_FORCE_LPS_OS_ALIGN
    hal_LpsForceOsTimAlignEnable(TRUE);
#endif
    BAL_DevHandlerInit();
    TS_Init_Inner();

    // TS_SetOutputMask(CSW_TS_ID, 1);
    // TS_SetOutputMask(CFW_SIM_TS_ID, 1);
    // TS_SetOutputMask(CFW_NW_TS_ID, 1);
    // TS_SetOutputMask(CFW_SMS_TS_ID, 1);
    // TS_SetOutputMask(CFW_CC_TS_ID, 1);
    // TS_SetOutputMask(CFW_SS_TS_ID, 1);
    // TS_SetOutputMask(CFW_PM_TS_ID, 1);
    // TS_SetOutputMask(BASE_FFS_TS_ID, 1);
    // TS_SetOutputMask(BASE_TM_TS_ID, 1);
    // TS_SetOutputMask(CFW_SHELL_TS_ID, 1);
    // TS_SetOutputMask(CFW_AOM_TS_ID, 1);

    // open shell trace
    sxs_IoCtx.TraceBitMap[TGET_ID(_CSW)] |= (1 << 1);

    BAL_SetMMIDefaultValue();

    // CSW_CheckMMIDefaultValue() ;
    CSW_CheckMMIDefaultValue(); // add wys 2007-06-20

    csw_MemIint();
    PM_CheckValidPowerOnVoltage(3000);

    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("\nBAL_TaskInit Start. \n",0x0800000a)));

    DRV_FlashPowerUp();
    //mabo deleted,20070813
    //DRV_FlashInit(CSW_DVR_FLASH_INIT_PARA) ;
    //romuald added 20080425
    DRV_FlashInit();

    // ML should be initialized before VDS_Init
    ML_Init();
    ML_SetCodePage(ML_ISO8859_1);

    memd_Init();
    // No flash read access is allowed when flash is being programmed
    sxr_EnterScSchedule();
    UINT32 status = hal_SysEnterCriticalSection();

    err_code = VDS_Init();   // Initialize VDS. added bye nie. 20070322
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("VDS_Init() OK.\n",0x0800000b));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("VDS_Init() ERROR, Error code: %d.\n",0x0800000c), err_code);
    }

    hal_SysExitCriticalSection(status);
    sxr_ExitScSchedule();

    err_code = DSM_DevInit();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("DSM_DevInit OK.\n",0x0800000d));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("DSM_DevInit ERROR, Error code: %d. \n",0x0800000e), err_code);
    }

    // spiffs init
    sf_config_t sf_config = {
        .phys_start = SPIFFS_FLASH_START,
        .phys_size = SPIFFS_FLASH_SIZE,
        .format_enable = true,
        .check_enable = true,
    };
    err_code = sf_init(&sf_config);
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("SPIFFS init Check OK.\n", 0x08000030));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("SPIFFS init Check ERROR.\n", 0x08000031));
    }

    err_code = CFW_CfgInit();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CfgInit OK.\n",0x08000011)));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_CfgInit ERROR, Error code: 0x%08x \n",0x08000012)), err_code);
    }

#ifdef LTE_NBIOT_SUPPORT
    extern void nvm_init(void);
    nvm_init();
#endif

    err_code = FS_PowerOn();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("FS Power On Check OK.\n",0x08000015));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("FS Power On Check ERROR, Error code: %d. \n",0x08000016), err_code);
    }

    // Get FS device table.
    err_code = FS_GetDeviceInfo(&fs_dev_count, &fs_dev_info);
    if(err_code != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("Device not register.\n",0x08000017));
        fs_dev_count = 0;
    }

    // Check the state of root device ,if not format, format it.
    // Mount root device.
    for(i = 0; i < fs_dev_count; i++)
    {
        // format the flash device.
        if(fs_dev_info[i].dev_type == FS_DEV_TYPE_FLASH)
        {
            err_code = FS_HasFormatted(fs_dev_info[i].dev_name, FS_TYPE_FAT);

            if(ERR_FS_HAS_FORMATED == err_code)
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s has formated.\n",0x08000018), fs_dev_info[i].dev_name);
            }
            else if(ERR_FS_NOT_FORMAT == err_code)
            {

                CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s not format.\n",0x08000019), fs_dev_info[i].dev_name);
                err_code = FS_Format(fs_dev_info[i].dev_name, FS_TYPE_FAT, 0);
                if(ERR_SUCCESS == err_code)
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s format ok.\n",0x0800001a), fs_dev_info[i].dev_name);
                }
                else if(ERR_FS_NOT_FORMAT == err_code)
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("The flash device %s format error.Error code:%d.\n",0x0800001b), fs_dev_info[i].dev_name, err_code);
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("The flash device %s has formated error.Error code:%d.\n",0x0800001c), fs_dev_info[i].dev_name, err_code);

            }

            // Mount root device.
            if(TRUE == fs_dev_info[i].is_root)
            {
                if(fs_root_dev_count > 0)
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("The FS root device too more:%d.\n",0x0800001d), fs_root_dev_count);
                }
                else
                {
                    err_code = FS_MountRoot(NULL);
                    if(ERR_SUCCESS == err_code)
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("FS MountRoot(%s) OK.\n",0x0800001e), fs_dev_info[i].dev_name);
                        fs_root_dev_count ++;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("FS MountRoot(%s) ERROR, Error code: %d. \n",0x0800001f), fs_dev_info[i].dev_name, err_code);
                        hal_HstSendEvent(0xaaaa000e);
                        hal_DbgAssert("mount root failed.err_code = %d", err_code);
                    }
                }
            }
        }
    }
    g_BalFlashInitProgramDone = TRUE;
#if !defined(_T_UPGRADE_PROGRAMMER) && !defined(CHIP_HAS_AP)
    err_code = FACTORY_Init();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("FS FACTORY_Init OK.\n",0x08000020));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("FS FACTORY_Init error, Error code: %d. \n",0x08000021), err_code);
    }
#endif
#if !defined(_T_UPGRADE_PROGRAMMER) && defined(REDUNDANT_DATA_REGION)

    err_code = Remain_Init();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("Remain_Init OK.\n",0x08000020));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("Remain_Init error, Error code: %d. \n",0x08000021), err_code);
    }
#endif

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    err_code = VDS_CacheInit();   // Initialize VDS Cache.
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTR("VDS_CacheInit() OK.\n",0x08000022));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTR("VDS_CacheInit() ERROR, Error code: %d.\n",0x08000023), err_code);
    }
#endif
    //#ifdef SIM_SWITCH_ENABLE
#if 0
    //
    //Add by lixp at 070407
    //SimSwitch(UINT8 SimNum)
    //#define SIM_1 1 //simcard 1
    //#define SIM_2 2 //simcard 2
    //
    UINT8 nSIMCARD_INDEX = 0x00;
    err_code = CFW_CfgGetSimSwitch(&nSIMCARD_INDEX);
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_CfgGetSimSwitch OK.\n",0x08000024)));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_CfgGetSimSwitch ERROR, Error code: 0x%08x \n",0x08000025)), err_code);
    }

    SimSwitch(nSIMCARD_INDEX);
    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("nSIMCARD_INDEX is %d\n",0x08000026)), nSIMCARD_INDEX);
#endif

    // CFW NV Initialize.
    err_code = NV_Init();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_BAL_TS_ID, "Cfw Nv init OK.\n");
    }
    else
    {
        CSW_TRACE(BASE_BAL_TS_ID, "Cfw Nv init Error. err_code = %d\n",err_code);
    }
    err_code = SMS_DM_Init();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("SMS_DM_Init OK.\n",0x08000013)));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("SMS_DM_Init ERROR, Error code: %d.\n",0x08000014)), err_code);
    }

#ifdef FSTRACE_SUPPORT
    fsTace_task_create();
#endif

    TM_PowerOnPatch();

    // Start application tasks

    // TODO: Should task scheduling be disabled? If yes, how about the interrupts?
    // Note that sxr_Sleep should be banned when task scheduling is disabled.
    //sxr_EnterScSchedule();

    for(i = 0; i < MAX_BAL_TASK_NUM; i++)
        g_BalTasks[i] = HNULL;

    g_BalTasks[BAL_TASK_NUM(BAL_SYS_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_SysTask, NULL, NULL,
            BAL_SYS_TASK_STACK_SIZE, BAL_SYS_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_SysTask");

#if 0
    g_BalTasks[BAL_TASK_NUM(BAL_CFW_ADV_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_CFWApsTask, NULL, NULL,
            BAL_CFW_ADV_TASK_STACK_SIZE, BAL_CFW_ADV_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_CFWApsTask");
#endif

    g_BalTasks[BAL_TASK_NUM(BAL_DEV_MONITOR_TASK_PRIORITY)] = COS_CreateTask_Prv(BAL_DevMonitorTask, NULL, NULL,
            BAL_DEV_MONITOR_TASK_STACK_SIZE, BAL_DEV_MONITOR_TASK_PRIORITY, COS_CREATE_DEFAULT, 0, "BAL_DevMonitorTask");

    g_BalTasks[3] = COS_CreateTask_Prv(BAL_BackgroundTask, NULL, NULL,
                                       BAL_BG_TASK_STACK_SIZE, COS_BK_TASK_PRI, COS_CREATE_DEFAULT, 0, "BAL_BackgroundTask");

    BAL_ApplicationInit();

#if (defined(COMPRESS_3264)||defined(COMPRESS_6464) ||defined(COMPRESS_3232) ||defined(COMPRESS_3232_NO_OVERLAY))&&!defined(MMI_ON_WIN32)
    while(!cdfu_is_inited())
    {
        COS_Sleep(50);
    }
#endif
#if (defined(COMPRESS_3232) && !defined(MMI_ON_WIN32))
    extern BOOL is_cdfu_overlay_3_1_decompressed();
    while(!is_cdfu_overlay_3_1_decompressed())
    {
        COS_Sleep(50);
    }
#endif
#ifndef ONLY_MMI_SUPPORT
    BAL_ATInit();
#endif

#ifdef IOT_SUPPORT
    IOT_Init();
#endif

#ifdef WIFI_SUPPORT
    wifi_task_init();
#endif

    //sxr_ExitScSchedule();

    return TRUE;
}


extern void cdfu_init(void);
extern void cdfu_overlay_2_1(void);
extern void cdfu_overlay_3_1(void);

VOID BAL_DecompressCode(VOID)
{
#ifndef MMI_ON_WIN32

    while (!g_BalFlashInitProgramDone)
    {
        COS_Sleep(5);
    }

#if defined(COMPRESS_OPTIMIZE_FLASH_SIZE) || defined(COMPRESS_3264) || defined(COMPRESS_6464) || defined(COMPRESS_3232)  ||defined(COMPRESS_3232_NO_OVERLAY)
    cdfu_init();
#elif defined(COMPRESS_SLIMPDA6464)
    cdfu_init();
    cdfu_overlay_2_1();
    cdfu_overlay_3_1();
#endif
#if ((defined(COMPRESS_3264)&&defined(COMPRESS_3264_USE_OVERLAY)))
    cdfu_overlay_3_2() ;
#elif defined(COMPRESS_3232)
    cdfu_overlay_3_1() ;
#endif
#endif // !MMI_ON_WIN32
}


BOOL IsCFWEventAvail(VOID)
{
    if( sxr_SbxHot( 11) )
        return TRUE;
    else
        return FALSE;
}

