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


#include "cs_types.h"
#include "string.h"
#include "fs.h"
//#include "dsm_cf.h"
#include "errorcode.h"
#include "ts.h"
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
#include "hal_boot_sector_reload.h"

#define  UPLOD_BUFF_SIZE   (320*90)
#define  UPLOD_LINE_MAX_SIZE   200
#define  FILE_NAME_MAX_LEN      248
#define  FILE_PATH_MAX_LEN      8
#define UPDATE_CFG_NAME "/t/update.cfg"

#define BAL_T_RAMRUN_KEY   "T_RAMRUN"
#define BAL_T_UPDATE_KEY   "T_UPDATE"

#define BAL_T_PATH "/T/"

extern VOID DSM_Dump(UINT8 *pBuff, UINT32 iTotalSize, INT32 iLineSize);
extern UINT16 DSM_UnicodeLen(UINT8 *pszUnicode);


typedef struct _uplod_info
{
    UINT8 *pbuff;
    UINT8 *pos;
    UINT32 read_size;
    UINT32 ramrun_totalSize;
    UINT32 hasRead_totalSize;
    UINT32 update_precent;
} _UPLOD_INFO;

_UPLOD_INFO g_uplod = {NULL, 0, 0};
static INT32 g_bal_log_fp = -1;


UINT32 bal_Assic2Unicode(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12])
{
    UINT32 iUniLen = 0;
    UINT32 i;
    UINT8 *p = NULL;

    nCharset = nCharset;
    if(NULL == in || NULL == out_len)
    {
        return 1;
    }


    iUniLen = (in_len + 2) * 2;
    p = CSW_Malloc(iUniLen);
    if(NULL == p)
    {
        *out = NULL;
        *out_len = 0;
        return 2;
    }
    else
    {
        *out = p;
        *out_len = iUniLen;
    }


    for(i = 0; i <= in_len; i++)
    {
        p[2 * i] = in[i];
        p[2 * i + 1] = 0;
    }

    return 0;
}


INT32 bal_FileTrace(UINT32 id, CONST INT8 *fmt, ...)
{
    INT32 count;

    // static INT32 fp = -1;
    UINT8 log_name[64] = {'/', 0, 't', 0, '/', 0, 't', 0, '_', 0, 'u', 0, 'p', 0, 'd', 0, 'a', 0, 't', 0, 'e', 0, '_', 0, '0', 0, '.', 0, 'l', 0, 'o', 0, 'g', 0, 0, 0,};
    char buf[512];
    va_list ap;

    va_start (ap, fmt);
    if((count = vsprintf(buf, fmt, ap)) > 510)
    {
        sxs_Raise (_MMI, "count %d overflow%x\n", count);
    }

    va_end (ap);

    strcat(buf, "\n");

    if(-1 == g_bal_log_fp)
    {
        g_bal_log_fp = FS_Create(log_name, 0);
        if(g_bal_log_fp < 0)
        {
            return 0;
        }

    }


    if(g_bal_log_fp >= 0)
    {
        FS_Write(g_bal_log_fp, buf, strlen(buf));
        //  FS_Flush(g_t_up_log_fp);
    }
    return count;
}

void bal_FileTraceClose()
{
    if(-1 != g_bal_log_fp)
    {
        FS_Close(g_bal_log_fp);
        g_bal_log_fp = -1;
    }
}

void bal_clearspecialCharacter(UINT8 *str)
{
    UINT32 vi = 0, totalCnt = strlen(str);
    for(vi = 0; vi < totalCnt; vi++)
    {
        if(*(str + vi) == '\n' || *(str + vi) == '\r' || *(str + vi) == 0x20 || *(str + vi) == 0x09)
        {
            *(str + vi) = '\0';
        }
    }
}

UINT8 *T_UP_StrChar(UINT8 *str, UINT8 ch, UINT32 size)
{
    UINT32 i;

    for (i = 0; i < size; i++)
    {
        if (*str == (char) ch)
            return __UNCONST(str);
        if (!*str)
            return NULL;
        str++;
    }
    return NULL;
    /* NOTREACHED */
}

void bal_ReadLine(INT32 fhd, UINT8 *buffer)
{
    UINT8 *pFind = NULL, *pFindR = NULL, *pFindN = NULL;
    UINT8 spare_size = 0;
    UINT8 szline[UPLOD_LINE_MAX_SIZE];

    //*buffer = 0;

    if(g_uplod.pbuff == NULL)
    {
        g_uplod.pbuff = COS_MALLOC(UPLOD_BUFF_SIZE + UPLOD_LINE_MAX_SIZE);
        g_uplod.pos = g_uplod.pbuff;
        memset(g_uplod.pbuff, 0, UPLOD_BUFF_SIZE + UPLOD_LINE_MAX_SIZE);
    }
    if(! g_uplod.ramrun_totalSize)
    {
        g_uplod.ramrun_totalSize = (UINT32)FS_GetFileSize(fhd);
        bal_FileTrace(BASE_BAL_TS_ID, TSTXT("bal_ReadLine , ramrun_totalSize = 0x%x."), g_uplod.ramrun_totalSize);
    }

    do
    {
        if(g_uplod.read_size == 0)
        {

            g_uplod.read_size = FS_Read(fhd, g_uplod.pbuff + spare_size, UPLOD_BUFF_SIZE);
            g_uplod.hasRead_totalSize += g_uplod.read_size;
            g_uplod.update_precent = (g_uplod.hasRead_totalSize * 100) / g_uplod.ramrun_totalSize;
            bal_FileTrace(BASE_BAL_TS_ID, TSTXT("bal_ReadLine:hasRead_totalSize = 0x%x,read_size = 0x%x.update_precent = %d."),
                          g_uplod.hasRead_totalSize,
                          g_uplod.read_size,
                          g_uplod.update_precent);

            if(g_uplod.read_size == 0)
            {
                if(spare_size > 0)
                {
                    strcpy(buffer, szline);
                    spare_size = 0;
                }
                break;
            }
            g_uplod.pos = g_uplod.pbuff;
            g_uplod.read_size += spare_size;
            g_uplod.pbuff[g_uplod.read_size] = 0;
        }

        pFindR  = T_UP_StrChar(g_uplod.pos, 0x0D, UPLOD_LINE_MAX_SIZE - 1);
        pFindN  = T_UP_StrChar(g_uplod.pos, 0x0A, UPLOD_LINE_MAX_SIZE - 1);

        pFind = pFindN > pFindR ? pFindN : pFindR;
        if(pFind != NULL)
        {
            if(pFindR  != NULL)
            {
                *pFindR = 0;
            }
            if(pFindN != NULL)
            {
                *pFindN  = 0;
            }

            if(pFind == g_uplod.pos)
            {
                g_uplod.read_size -= ((pFind - g_uplod.pos) + 1);
                g_uplod.pos = pFind + 1;
            }
            else
            {
                strcpy(buffer, g_uplod.pos);
                g_uplod.read_size -= ((pFind - g_uplod.pos) + 1);
                g_uplod.pos = pFind + 1;
                break;
            }
        }
        else if(g_uplod.read_size > 0)
        {
            //hal_DbgAssert(0);
            strcpy(szline, g_uplod.pos);
            spare_size = strlen(szline);
            memset(g_uplod.pbuff, 0, UPLOD_BUFF_SIZE + UPLOD_LINE_MAX_SIZE);
            strcpy(g_uplod.pbuff, szline);
            g_uplod.read_size = 0;

        }
    }
    while(1);
    bal_clearspecialCharacter(buffer);
}




INT32 bal_GetFileName(UINT8 *pLine, UINT8 *pKeyStr, UINT8 *pFileName)
{
    UINT32 statues = 0;
    UINT8 *p = NULL;
    UINT32 name_size = 0;
    INT32 ret = 0;

    SUL_CharUpper(pLine);

    p = strstr(pLine, pKeyStr);
    if(NULL == p)
    {
        return 1;
    }
    p += strlen(pKeyStr);
    while(*p && name_size < FILE_NAME_MAX_LEN)
    {
        switch(statues)
        {
            case 0: // find
                if(0x20 == *p || 0x09 == *p)
                {
                }
                else if ('=' == *p)
                {
                    statues = 1;
                }
                else
                {
                    return -2;
                }
                break;
            case 1: // =
                if(0x20 == *p || 0x09 == *p)
                {
                }
                else if(!(*p == 0x3a || *p == 0x22 ||
                          *p == 0x2a || *p == 0x3c ||
                          *p == 0x3e || *p == 0x3f ||
                          *p == 0x2f || *p == 0x5c ||
                          *p == 0x7c || (*p < 0x20 && *p != 0x05)))
                {
                    pFileName[name_size] = *p;
                    name_size ++;
                    statues = 2;
                }
                else
                {
                    return  -3;
                }
                break;
            case 2: // file name
                if(!(*p == 0x3a || *p == 0x22 ||
                        *p == 0x2a || *p == 0x3c ||
                        *p == 0x3e || *p == 0x3f ||
                        *p == 0x2f || *p == 0x5c ||
                        *p == 0x7c || (*p < 0x20 && *p != 0x05)))
                {
                    pFileName[name_size] = *p;
                    name_size ++;
                }
                else if(*p == 0)
                {
                    *pFileName = 0;
                    ret = 0;
                    goto func_end;
                }
                break;
            case 3: // end.
                ret = 0;
                goto func_end;
                break;
            default:
                break;
        }
        p++;
    }
func_end:

    if(ret == 0)
    {
        while(name_size >= 0)
        {
            if(pFileName[name_size] == 0x20 || pFileName[name_size] == 0x09)
            {
                pFileName[name_size] = 0;
                name_size --;
            }
            else
            {
                break;
            }
        }
    }
    return 0;

}


INT32 bal_updateSoftware(void(*updateCb)(UINT32 percents))
{
    /*open the information file and parse it*/
    UINT8 *unicodeName = NULL;
    //UINT8 *p = NULL;
    UINT32 outlen = 0, ret = 0, linecnt = 0, fillIdx = 0;
    INT32 cfghandle = -1, ramimgFhd = -1;
    INT32 *pmem = NULL;
    char *endp;
    UINT8 ramrun_name[FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN];
    static UINT32 currentPercent = 0;
    BOOT_SECTOR_RELOAD_CTX_T ctx;
    UINT8 szTmp[UPLOD_LINE_MAX_SIZE * 2];
    INT32 err_code = 0;
    INT32 ramrun_size = 0;
    UINT32 ramrun_file_size = 0;

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware: begin."));

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 1: Read cfg file."));
    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 1-1: Open cfg file."));
    err_code = bal_Assic2Unicode(UPDATE_CFG_NAME, strlen(UPDATE_CFG_NAME), &unicodeName, &outlen, NULL);
    if(err_code == ERR_SUCCESS)
    {
        cfghandle = FS_Open(unicodeName, FS_O_RDONLY, 0);
        if(cfghandle < 0)
        {
            ret = -2;
            bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:bal_Assic2Unicode failed,cfghandle = %d."), cfghandle);
            goto func_end;
        }
        else
        {
            bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:bal_Assic2Unicode ok,cfghandle = %d."), cfghandle);
        }
    }
    else
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:bal_Assic2Unicode failed,err_code = %d.\n"), err_code);
        ret = -1;
        goto func_end;
    }


    /********read cfg file info***************/
    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 1-2: Read cfg file."));
    memset(ramrun_name, 0x0, FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN);
    strcpy(ramrun_name, BAL_T_PATH);

    err_code = -1;
    do
    {
        memset(szTmp, 0x0, sizeof(szTmp));
        bal_ReadLine(cfghandle, szTmp);
        if(0 == *szTmp)
        {
            break;
        }
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:cfg line = %s.\n"), szTmp);
        err_code = bal_GetFileName(szTmp, BAL_T_RAMRUN_KEY, ramrun_name + strlen(BAL_T_PATH));
        if(err_code == 0)
        {
            bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware T_RAMRUN  = %s.\n"), ramrun_name);
            break;
        }

    }
    while(err_code == 1);

    if(err_code != 0)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:get ramrun name failed.err_code = %d.\n"), err_code);
        ret = -3;
        goto func_end;
    }

    if(unicodeName != NULL)
    {
        CSW_Free(unicodeName);
        unicodeName = NULL;
    }

    /********lod tramrun to ram***************/
    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2:Load tramrun to ram."));

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2-1:to unicode."));
    err_code = bal_Assic2Unicode(ramrun_name, strlen(ramrun_name), &unicodeName, &outlen, NULL);
    if(err_code != 0)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware: to unicode failed. ramrun_name = %s,err_code = %d,strlen = %d."), ramrun_name, err_code, strlen(ramrun_name));
        ret = -4;
        goto func_end;
    }
    else
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:to unitcode ok.ramrun_name = %s."), ramrun_name);
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2-2:open tramrun."));
    ramimgFhd = FS_Open(unicodeName, FS_O_RDONLY, 0);
    if(ramimgFhd < 0)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:open ramrun failed. ramimgFhd = %d."), ramimgFhd);
        ret = -5;
        goto func_end;
    }
    else
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:open ramrun ok. ramimgFhd = %d."), ramimgFhd);
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2-3:get tramrun file size."));
    ramrun_file_size = (UINT32)FS_GetFileSize(ramimgFhd);
    if(ramrun_file_size <= 0)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:get ramrun size failed.ramrun_size = %d."), ramrun_file_size);
        return -6;
    }
    else
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:get ramrun size ok.file size = 0x%x."), ramrun_file_size);
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2-4:malloc."));
    ramrun_size = ramrun_file_size / 2;
    pmem = COS_Malloc(ramrun_size, COS_MMI_HEAP);
    if(NULL == pmem)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:malloc failed. size = 0x%x."), ramrun_size);
        ret = -7;
        goto func_end;
    }
    else
    {
        memset(pmem, 0x0, ramrun_size);
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:malloc ok.pmem=%p,size = 0x%x"), pmem, ramrun_size);
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:step 2-5:load tramrun to ram."));
    if(ramimgFhd >= 0)
    {
        //clear the  global variable when read new file
        memset(&g_uplod, 0x0, sizeof(g_uplod));


        bal_ReadLine(ramimgFhd, szTmp);
        if(currentPercent != g_uplod.update_precent)
        {
            updateCb(g_uplod.update_precent);
        }
        currentPercent = g_uplod.update_precent;
        // CSW_TRACE(_CSW|TLEVEL(BASE_BAL_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTXT(TSTR("bal_updateSoftware:tmpContent is %s,total is %d,has read %d\n",0x08000034)),tmpContent,g_uplod.ramrun_totalSize,g_uplod.hasRead_totalSize);

        while(*szTmp != '\0')
        {

            if(*szTmp == '@')
            {
                //update last '@' address linecnt
                ctx.size = linecnt;

                if(fillIdx)
                {
                    hal_BootSectorFillReloadCtx(&ctx, fillIdx - 1);
                }

                //current '@' address is stored,when next time '@',update it
                ctx.mapAddress = strtoul(szTmp + 1, &endp, 16);
                ctx.storeAddress = (UINT32)pmem;
                bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:ctx.mapAddress=%p,ctx.storeAddress=%p,ctx.size=0x%x."), ctx.mapAddress , ctx.storeAddress, ctx.size );
                bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:fillIdx=0x%x."), fillIdx);
                fillIdx++;
                linecnt = 0;
            }
            else
            {
                outlen = strtoul(szTmp, &endp, 16);
                memcpy((pmem++) , &outlen, sizeof(ctx.storeAddress));
                linecnt++;
                //bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:outlen  is %p,g_uplod.update_precent is %d,%d.\n"), outlen,g_uplod.update_precent,currentPercent);
            }
            memset(szTmp, 0x0, sizeof(szTmp));
            bal_ReadLine(ramimgFhd, szTmp);
            // bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:tmpContent is %s,total is %d,has read %d.\n"),szTmp,g_uplod.ramrun_totalSize,g_uplod.hasRead_totalSize);

            if(currentPercent != g_uplod.update_precent)
            {
                updateCb(g_uplod.update_precent);
            }
            currentPercent = g_uplod.update_precent;
        }

        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:ctx.mapAddress = %p,ctx.storeAddress = %p,ctx.size = 0x%x."), ctx.mapAddress , ctx.storeAddress, ctx.size );
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:load end."));

        /*when at the end of the file,refresh the 3rd hal structure*/
        ctx.size = linecnt;
        hal_BootSectorFillReloadCtx(&ctx, fillIdx - 1);

        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:close ramimgFhd(%d)."), ramimgFhd);
        FS_Close(ramimgFhd);
        hal_BootSectorReloadEnable();

    }
    ret = 0;
func_end:

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:close cfghandle(%d)."), cfghandle);
    if(cfghandle >= 0)
    {
        FS_Close(cfghandle);
        cfghandle = -1;
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:free unicodeName(0x%x)."), unicodeName);
    if(unicodeName != NULL)
    {
        CSW_Free(unicodeName);
        unicodeName = NULL;
    }

    bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:free g_uplod.pbuff(0x%x)."), g_uplod.pbuff);
    if(g_uplod.pbuff)
    {
        COS_FREE(g_uplod.pbuff);
        g_uplod.pbuff = NULL;
    }

    if(ret == 0)
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:finish!system reset."));
        bal_FileTraceClose();
        DM_Reset();
    }
    else
    {
        bal_FileTrace(BASE_FFS_TS_ID, TSTXT("bal_updateSoftware:failed!ret = %d."), ret);
        bal_FileTraceClose();
        if(pmem)
        {
            COS_FREE(pmem);
            pmem = NULL;
        }
    }
    return ret;
}


