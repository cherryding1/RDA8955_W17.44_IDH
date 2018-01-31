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


#ifndef CHIP_HAS_AP

#include "sxs_cfg.h"
#include "hal_timers.h"
#include "itf_msgc.h"
#include "fs.h"
#include "cfw.h"
#include "csw.h"
#include "fstraces.h"
#include "sxs_io.h"
#include "cs_types.h"
#include "dbg.h"
#include "hal_host.h"
#include "base_prv.h"
#include "dsm_cf.h"
#include "bal_fstrace.h"

// global variable

INT32 g_tracefileHandle;

UINT8 *g_trace_Data = NULL;

PRIVATE INT32 _fsTraceFileOpen(CONST CHAR *name,  UINT32 iFlag, UINT32 iMode)
{
    INT32 result;
    INT32 fd = -1;
    UINT32 uni_len = 0;
    UINT8 *uni_name = NULL;
    if((result = ML_LocalLanguage2UnicodeBigEnding(name,  DSM_StrLen(name), &uni_name, &uni_len, NULL) ) != ERR_SUCCESS)
    {
        if(uni_name)
        {
            CSW_Free(uni_name);
            uni_name = NULL;
        }
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("fstrace:%s:%u:ML_LocalLanguage2UnicodeBigEnding ret %d",0x08000001), __FUNCTION__, __LINE__, result);
        return -1;
    }

    fd = FS_Open(uni_name, iFlag, iMode);

    if(uni_name)
    {
        CSW_Free(uni_name);
        uni_name = NULL;
    }

    return fd;

}


PRIVATE INT32 _fsTraceFileClose(INT32 fd)
{
    if(fd < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("%s:%d: 0x%x not a FILE pointer",0x08000002), __FUNCTION__, __LINE__, fd);
        return -1;
    }
    return FS_Close(fd);
}

// =============================================================================
// _fsTraceSetLevel
// -----------------------------------------------------------------------------
/// set trace level
// =============================================================================
PRIVATE VOID _fsTraceSetLevel(void)
{
    UINT16 traceLevel[SXS_NB_ID] = {0, };

    UINT16 cnt                         = 0;
    UINT8 targetTraceLevelFileName[30] = "/t/csd_fstracelevel.tfg";

    INT32 iResult                      = -1;
    INT32 fhd;
    UINT8 tmpTransfer = 0, *tmpTransfer1;
    traceLevel[__MMI] = 0xffff;
    traceLevel[__CSW] = 0xffff;

    CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("fsTrace_setTraceLevel OK.%d,%d\n",0x08000003)), __MMI, SXS_NB_ID);

    if((fhd = _fsTraceFileOpen(targetTraceLevelFileName, FS_O_RDONLY, 0)) >= 0)
    {
        iResult = FS_Read(fhd, (UINT8 *)(traceLevel), SXS_NB_ID * 2);

        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("fsTrace_setTraceLevel read result is %d\n",0x08000004)), iResult);

        cnt = 0;
        while (cnt < SXS_NB_ID)
        {

            /******CONVERT H8 AND L8,START ****/
            tmpTransfer1 = (UINT8 *)(&traceLevel[cnt]);
            tmpTransfer = *tmpTransfer1;
            *tmpTransfer1 = *(tmpTransfer1 + 1);
            *(tmpTransfer1 + 1) = tmpTransfer;
            /******CONVERT H8 AND L8, END ****/

            CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("fstraces_setLevel(0x%x 0x%x",0x08000005)), TID(cnt), traceLevel[cnt]);
            fstraces_setLevel(TID(cnt), traceLevel[cnt]);
            ++cnt;
        }

        iResult = _fsTraceFileClose(fhd);
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(1), TSTXT(TSTR("fsTrace_setTraceLevel close result is %d\n",0x08000006)), iResult);

    }
    else  // set default level
    {
        while (cnt < SXS_NB_ID)
        {
            fstraces_setLevel(TID(cnt), traceLevel[cnt]);
            ++cnt;
        }
    }

}

// =============================================================================
// bal_fsTraceProcess
// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer and output to tFlash card.
// =============================================================================
PUBLIC VOID bal_fsTraceProcess(VOID)
{
    INT32 trace_len, result = 0;

    //  hal_HstSendEvent(199);

    if(g_trace_Data == NULL)
    {
        return;
    }

    do
    {
        trace_len = fstraces_Read(g_trace_Data, MAX_TRACE_LEN);
        if ((trace_len > 0) && (g_tracefileHandle >= 0))
        {
            result = FS_Write(g_tracefileHandle, g_trace_Data, trace_len);
            if(result == ERR_FS_DISK_FULL)
            {
                FS_Truncate(g_tracefileHandle, 0);
            }
            else if(result < 0)
            {
                bal_fstraceStop();
                return;
            }
        }
    }
    while (trace_len > 0);

    COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_CFW_TRACE_TO_FLASH_TIMER_ID, COS_TIMER_MODE_SINGLE, FS_TRACE_TIMER_CNT);

}

// =============================================================================
// bal_fstraceStart
// -----------------------------------------------------------------------------
/// start tFlash trace recorsing
// =============================================================================
PUBLIC VOID  bal_fstraceStart(VOID)
{
    UINT8 targetTraceName[20] = "/t/fstrace.bin";

    if(g_trace_Data != NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("bal_fstraceStart:fs trace already opened!",0x08000007)));
        return;
    }

    if((g_tracefileHandle = _fsTraceFileOpen(targetTraceName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0)) < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTXT(TSTR("bal_fstraceStart:file %s open fail, ret %x",0x08000008)), targetTraceName,  g_tracefileHandle);
        return;
    }

    if((g_trace_Data = CSW_Malloc(MAX_TRACE_LEN)) == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("bal_fstraceStart:malloc fail!",0x08000009)));
        _fsTraceFileClose(g_tracefileHandle);
        return;
    }

    _fsTraceSetLevel();

    fstraces_Open();

    COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , PRV_CFW_TRACE_TO_FLASH_TIMER_ID, COS_TIMER_MODE_SINGLE, FS_TRACE_TIMER_CNT);

}


// =============================================================================
// bal_fstraceStop
// -----------------------------------------------------------------------------
/// stop tFlash trace recorsing
// =============================================================================
PUBLIC VOID  bal_fstraceStop(VOID)
{

    fstraces_Close();
    COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_CFW_TRACE_TO_FLASH_TIMER_ID);
    _fsTraceFileClose(g_tracefileHandle);
    CSW_Free(g_trace_Data);
    g_trace_Data = NULL;
}

#endif // !CHIP_HAS_AP

