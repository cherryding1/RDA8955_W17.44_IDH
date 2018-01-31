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


#if 1
//#include "project_config.h"

//#include "host_config.h"
//#include "host_types.h"
//#include "host_error.h"
//#include "papi.h"
#include "mmi_trace.h"


#include "cos.h"
#include "bt.h"
#include "papi.h"
#include "rdabt_main.h"
#include "hal_dma.h"
#include "hal_debug.h"
#include "hal_sys.h"
#include "coolprofile.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack_config.h"
#include "dbg.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "sxr_mem.hp"
#include "sxr_sbx.hp"
#include "edrvp_debug.h"

void RDABT_Platform_Init(void)
{
}


#if pDEBUG
static const char* LOGLAYER[]=
{
    "pLOGSYSTEM",
    "pLOGHCI",
    "pLOGMANAGER",
    "pLOGL2CAP",
    "pLOGRFCOMM",
    "pLOGSDP",
    "pLOGOBEX",
    "pLOGTCS",
    "pLOGA2DP",
    "pALWAYS"
};

static const char* LOGLEVEL[]=
{
//    "pLOGDATA",
    "pLOGFATAL",
    "pLOGCRITICAL",
    "pLOGERROR",
    "pLOGWARNING",
    "pLOGNOTICE",
    "pLOGDEBUG"
};


#include "mmi_trace.h"


int RDA_Debug_PrintEX(int logLevel,int logLayer, char *formatString, ... )
{
    va_list args;
    char buff[512];
    //mmi_trace(g_sw_BT,"RDA_Debug_PrintEX \n");

    //  return RDABT_NOERROR;
#if 1
    if(pLOGHCI==logLayer)
        return RDABT_NOERROR;

#if 1
    if(pLOGSYSTEM==logLayer)
        return RDABT_NOERROR;
    if(pLOGMANAGER==logLayer)
        return RDABT_NOERROR;
    if(pLOGSDP==logLayer)
        return RDABT_NOERROR;
    if(pLOGRFCOMM==logLayer)
        return RDABT_NOERROR;
    if(pLOGL2CAP==logLayer)
        return RDABT_NOERROR;
    //if(pLOGOBEX==logLayer)
    // return RDABT_NOERROR;
    //if(pLOGHFP==logLayer)
    // return RDABT_NOERROR;
#endif
    va_start(args,formatString);
    vsprintf(buff, formatString, args);
    va_end(args);
    SXS_TRACE(_SXR | TLEVEL(1), buff);
    //mmi_trace(g_sw_BT, buff);


    return RDABT_NOERROR;
#endif
}



int RDA_Debug_Print(int logLevel, char *formatString, ... )
{
    va_list args;
    char buff[512];

    va_start(args,formatString);
    vsprintf(buff, formatString, args);
    va_end(args);

    SXS_TRACE(_SXR | TLEVEL(1), buff);
    //mmi_trace(g_sw_BT, buff);

    return RDABT_NOERROR;
}


int pDebugDumpFunc(int logLevel, u_int8 *buffer, u_int32 len)
{
    return pDebugDumpFuncEX(logLevel,pLOGALWAYS,buffer,len);
}

int pDebugDumpFuncEX(int logLevel, int logLayer, u_int8 *buffer, u_int32 len)
{
    return RDABT_NOERROR;
}

int pDebugCheckFunc(void)
{
    return RDABT_NOERROR;
}

#endif
//#define  BT_MEM_OVERWRITE_DEBUG
#ifdef BT_MEM_OVERWRITE_DEBUG
UINT32 Current_ptr_mem1058=0;
uint32 size_proc=1058;
#endif
void *pMalloc(size_t size)
{
    UINT32  nCallerAdd;
    COS_GET_RA(&nCallerAdd);
#ifdef BT_MEM_OVERWRITE_DEBUG
    if(size==size_proc)
        size=size+20;
#endif
    if (size ==0)
    {
        kal_prompt_trace(1,"#################pMalloc size = %p\n",size);
        return NULL;
    }

    void *p = COS_MALLOC(size);
    if (p == NULL)
    {
        kal_prompt_trace(1,"#################pMalloc retuen address = %p\n",p);
        return NULL;
    }

    memset(p,0,size);
    COS_UpdateMemCallAdd(p,nCallerAdd);
#ifdef BT_MEM_OVERWRITE_DEBUG
    if((size==(size_proc+20))&&(Current_ptr_mem1058==0))
    {
        Current_ptr_mem1058=(UINT32)p;
#ifdef ENABLE_PAGESPY		
        COS_PageProtectDisable(COS_PAGE_NUM_4);
        COS_PageProtectSetup(COS_PAGE_NUM_4, COS_PAGE_WRITE_TRIGGER, (UINT32)(p+size_proc), (UINT32)((p+size_proc+19)));
        COS_PageProtectEnable(COS_PAGE_NUM_4);
#endif
    }

    // kal_prompt_trace(1,"pmm=%d,C=%x",size,Current_ptr_mem1058);

#endif

    //mmi_trace(g_sw_BT,"pM s=%x  %x\n",size,p);
    return p;
}




void pFree(void *buffer)
{

#ifdef BT_MEM_OVERWRITE_DEBUG
    //kal_prompt_trace(1,"pff=%x,C=%x",(UINT32)buffer,Current_ptr_mem1058);

    if(Current_ptr_mem1058==(UINT32)buffer)
    {
        kal_prompt_trace(1,"pff=%x,C=%x",(UINT32)buffer,Current_ptr_mem1058);
#ifdef ENABLE_PAGESPY		
        COS_PageProtectDisable(COS_PAGE_NUM_4);
#endif
        Current_ptr_mem1058=0;
    }

#endif

    COS_FREE(buffer);
    buffer=NULL;

}



#if 0
#define pMemcpy(dest,  src,  n) memcpy(dest,src,n)

void *pMemcpy(void *dest,  void *src, size_t n)
{
    memcpy(dest,src,n);
}




void *pMemset(void *s, int c, size_t n)
{
    memset(s,c,n);
}

int pMemcmpp(void *buf1, void *buf2, size_t numbytes)
{
    int i;
    i++;
}

size_t pStrlen(const char *s)
{
    return strlen(s);
}

int pStrncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1,s2,n);
}

int pStrcmp(const char *s1, const char *s2)
{
    return strcmp(s1,s2);
}

char *pStrcpy(char *s1, const char *s2)
{
    return strcpy(s1,s2);
}

char *pStrncpy(char *s1, const char *s2, size_t n)
{
    return strncpy(s1,s2,n);
}

#endif

/*
 * PAPI TIMER SECTION
 * no used in MTK
 */


t_pTimer pTimerCreate_base(u_int32 nMillisecondes,  u_int8 flag)//flag 0 for task 1 for a2dp
{
    // u_int32 CallerAdd;
    // SXS_GET_RA(&CallerAdd);
    //mmi_trace(g_sw_BT,"pTimerCreate_base,flag=%d",flag);


    TASK_HANDLE * hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);

    sxs_StartTimer(((nMillisecondes<<14)/1000), BT_TIMER_PAPI+flag+1, NULL, TRUE, COS_TaskMailBoxId(hTask));

    return (t_pTimer)(flag+1);
}


int pTimerCancel_base(t_pTimer timerHandle)
{
    //u_int32 CallerAdd;
    // SXS_GET_RA(&CallerAdd);
    //mmi_trace(0,"pTimerCancel -- callbackFunc =0x%x,timerHandle =%d, CallerAdd = 0x%x,current ticks=%d\n",bt_timer_evt[timerHandle-1].callbackfun,timerHandle,CallerAdd,hal_TimGetUpTime());
    TASK_HANDLE * hTask = (TASK_HANDLE *)GetMmiTaskHandle(MOD_BT);
    sxs_StopTimer(BT_TIMER_PAPI+timerHandle,NULL,COS_TaskMailBoxId(hTask));
    return 0;
}



t_pTimer pTimerCreate(u_int32 timeoutTenths, void(*callbackFunc)(void *), void *callbackArg, u_int8 flags)
{
    return 0;
}

int pTimerSet(unsigned int timeout, void(*callbackFunc)(void *), void *callbackParm, int *timerHandle)
{
    return 0;
}

int pTimerCancel(t_pTimer timerHandle)
{
    return 0;
}


/*
void *pMemcpy(void *dest, void *src, size_t numbytes)
{

    if(numbytes < 128)
        return memcpy(dest, src, numbytes);
    else
    {
        HAL_DMA_CFG_T transferCfg;
        // Use a DMA transfer as it will be faster than the CPU.
        transferCfg.srcAddr         = (UINT8*)src;
        transferCfg.dstAddr         = (UINT8*)dest;
        transferCfg.alterDstAddr    = NULL;
        transferCfg.pattern         = 0;
        transferCfg.transferSize    = (UINT32)numbytes;
        transferCfg.mode            = HAL_DMA_MODE_NORMAL;
        transferCfg.userHandler     = NULL;
        // Start the DMA
        while (HAL_ERR_NO != hal_DmaStart(&transferCfg))
        {
            sxr_Sleep(10);
        }

        // Wait for the DMA to finish
        // (hal_OverlayLoad is a blocking function).
        while (!hal_DmaDone())
        {
            sxr_Sleep(10);
        }
    }
}*/
#endif

t_pTimer pTimerCreateMs(u_int32 timeoutTenths, void(*callbackFunc)(void *), void *callbackArg, u_int8 flags)
{}
int pSendEvent(u_int16 event_id, u_int8 *msg)
{}
u_int32 RDABT_Enter_CriticalSection()
{
    return hal_SysEnterCriticalSection();
}

void RDABT_Exit_CriticalSection(u_int32 status)
{
    hal_SysExitCriticalSection(status);
}

static  UINT8 g_btMemSemaphore = 0xff;

void btmem_SemFree(void)
{
    sxr_ReleaseSemaphore(g_btMemSemaphore);
}
void btmem_SemTake(void)
{
    if(g_btMemSemaphore==0xff)
    {
        UINT32 state=hal_SysEnterCriticalSection();
        g_btMemSemaphore=sxr_NewSemaphore(1);
        hal_SysExitCriticalSection(state);
    }

    sxr_TakeSemaphore(g_btMemSemaphore);
}

