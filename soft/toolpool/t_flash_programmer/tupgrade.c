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

#ifdef _MS_VC_VER_
#include "dsm_stdtype.h"
#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "dsm_shell.h"

#include "fat_base.h"
#include "fs.h"
#include "fs_base.h"
#include "fat_local.h"
#include "ml.h"
#else

#include "csw.h"
#include "dsm_cf.h"
#include "sxs_io.h"
#include "memd_m.h"
#include "drv_flash.h"
#include "calib_m.h"
#endif

#include "fs.h"
#include "tupgrade.h"
#include "string.h"

#ifdef _MS_VC_VER_
#define fmg_Printf
#define TSTDOUT 0
#define SUL_MemCopy8 memcpy
#define fmg_PrintfSetXY
#define sxs_fprintf CSW_TRACE
void hal_SysSoftReset()
{
    printf("reset!\n");
}
#define COS_FREE free
#define CSW_Free free
#define COS_MALLOC malloc
void DRV_FlashInit()
{
    printf("DRV_FlashInit!");
}
void DRV_FlashPowerUp()
{
    printf("DRV_FlashPowerUp!");
}
void csw_MemIint()
{
    printf("reset!\n");
}

#define CSW_FREE free
extern UINT32 ML_Init();
extern UINT32 ML_SetCodePage(UINT8 nCharset[12]);
UINT32 _boot_loader_magic_number = 0x20000;

#else
extern CONST MEMD_FLASH_LAYOUT_T g_memdFlashLayout;
extern void sxr_Sleep (u32 Period);
extern INT32 DSM_DevInit( VOID );
extern VOID csw_MemIint(VOID);
extern UINT32 fmg_Printf(CONST CHAR* fmt, ...);
extern VOID fmg_PrintfSetXY(UINT8 xPos, UINT8 yPos);

extern VOID hal_SysSoftReset(VOID);
extern UINT16 DSM_UnicodeLen(UINT8* pszUnicode);
extern INT32 VDS_Init(VOID);
extern BOOL hal_HstSendEvent(UINT32 ch);
extern int vsprintf(char *buf, const char *fmt, va_list ap);
extern CALIB_BUFFER_T _calib_start;
extern UINT32 _factory_start;
UINT32 _boot_loader_magic_number = 0;
#endif

#define MEMBER_OFFSETOF(TYPE, MEMBER) ((UINT32) &((TYPE *)0)->MEMBER)
#define MEMBER_SIZE(TYPE, MEMBER) (sizeof(((TYPE *)0)->MEMBER))

#define T_UP_DATA_TYPE_ERASE        1
#define T_UP_DATA_TYPE_DATA         2

#define T_UP_LINE_MAX_LEN              200
#define T_UP_BLOCK_LEN                    (1024*16)
#define T_UP_READ_LEN                      (1024*4)
#define T_UP_LINE_32BIT_LEN            10
#define  FILE_NAME_MAX_LEN             248
#define  FILE_PATH_MAX_LEN              8
#define  T_UP_FLASH_SEC_MAX_NUM  256
#define T_UPDATE_CFG_NAME             "/update.cfg"
#define T_UPDATE_KEY                        "T_UPDATE"
#define T_UPDATE_CFP_KEY                "T_CFP"
#define T_CLEAR_USER_DATA_KEY     "T_CLEAR_USER_DATA"

#define  T_UPDATE_AUD_SECT_1_LENGTH  0x2000
//#define T_UPDATE_AUDOFFSET            0x1388
//#define T_UPDATE_AUDLENGTH            0x960 //0x0978
//#define T_UPDATE_CALIB_SIZE            0x2000
#define T_UPDATE_OPT_LEN                 32

#define T_UPDATE_VERIFY_NAME           "/verify.txt"
#define T_UPDATE_MAKEFATINT32(a, b, c, d) MAKEINT32(MAKEINT16((a), (b)), MAKEINT16((c), (d)))

typedef struct
{
    UINT32 type;
    UINT32 addr;
    UINT8 data[T_UP_BLOCK_LEN+T_UP_LINE_MAX_LEN];
    UINT32 len;
} T_UP_DATA_INFO;

typedef struct
{
    UINT32 addr;
} T_UP_FLASH_INFO;

typedef struct t_up_info
{
    UINT8 *pbuff;
    UINT8 *pos;
    UINT32 read_size;
    UINT32 totalSize;
    UINT32 hasReadSize;
    UINT32 line_size;
} T_UP_INFO;

typedef struct
{
    UINT32 address;
    UINT32 magicNumber;
} T_UP_MAGIC_NUMBER;


T_UP_MAGIC_NUMBER g_magic_number[] =
{
    { .address = 0x000000, .magicNumber = 0xD9EF0045 },
    { .address = 0x000000,.magicNumber = 0x00000000 },
};

UINT32 g_maigic_sector = 0;

T_UP_INFO g_up_info = {NULL,NULL,0,0,0,0};

UINT32 g_code_size = 0;
UINT32 g_erase_count = 0;
T_UP_DATA_INFO *g_pdata_info = NULL;

T_UP_FLASH_INFO g_flash_info[T_UP_FLASH_SEC_MAX_NUM];
UINT32 g_flash_sec_count = 0;
INT32 g_t_up_log_fp = -1;
UINT32 g_tstReadSize = 0;
UINT32 g_aud_sect_2_Offset=0;
UINT32 g_aud_sect_2_length=0;

PRIVATE UINT32 T_UP_FlashGetSectorLimits(UINT32 sectorId, UINT32 *start, UINT32 *end)
{
    UINT32 aStart, aEnd;
    int bankNum = 0;
    int majSecIdx, minSecIdx;
    UINT32 sectSize;

    if (sectorId == g_memdFlashLayout.fullSize)
    {
        *start = g_memdFlashLayout.fullSize;
        *end = g_memdFlashLayout.fullSize;
        return 0;
    }
    else if (sectorId > g_memdFlashLayout.fullSize)
    {
        *start = 0;
        *end = 0;
        DSM_ASSERT(0, "Out of accessible flash space !!!!");
        return 0;
    }
    else
    {
        while ( (sectorId >= g_memdFlashLayout.bankLayout[bankNum+1].bankStartAddr) &&
                (bankNum < (g_memdFlashLayout.numberOfBank-1)) )
        {
            ++bankNum;
        }

        aStart = g_memdFlashLayout.bankLayout[bankNum].bankStartAddr;
        majSecIdx = 0;
        aEnd = aStart +
               (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);

        while (sectorId >= aEnd)
        {
            if (majSecIdx == 2)
            {
                *start = 0;
                *end = 0;
                DSM_ASSERT(0, "Out of accessible flash space !!!!");
                return 0;
            }
            else
            {
                ++majSecIdx;
                aStart = aEnd;
                aEnd = aStart +
                       (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                        g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);
            }
        }

        minSecIdx = 0;
        sectSize = g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1];
        aEnd = aStart + sectSize;

        while (sectorId >= aEnd)
        {
            aStart = aEnd;
            aEnd = aStart + sectSize;
            ++minSecIdx;
        }

        *start = aStart;
        *end = aEnd;
        return sectSize;
    }
}


UINT32 ML_Assic2Unicode(const UINT8* in, UINT32 in_len, UINT8** out, UINT32* out_len,UINT8 nCharset[12])
{
    UINT32 iUniLen = 0;
    UINT32 i;
    UINT8* p = NULL;

    nCharset = nCharset;
    if(NULL == in || NULL == out_len)
    {
        return 1;
    }

    iUniLen = (in_len+2)*2;
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
        p[2*i] = in[i];
        p[2*i + 1] = 0;
    }

    return 0;
}


INT32 T_UP_Trace(UINT32 id, CONST INT8 *fmt, ...)
{
    INT32 count;

    // static INT32 fp = -1;
    UINT8 log_name[32] = {'/',0,'t',0,'_',0,'u',0,'p',0,'d',0,'a',0,'t',0,'e',0,'.',0,'l',0,'o',0,'g',0,0,0,};
    char buf[512];
    va_list ap;

    va_start (ap, fmt);
    if((count = vsprintf(buf, fmt, ap)) > 510)
    {
        sxs_Raise (_MMI, "count %d overflow%x\n", count);
    }

    va_end (ap);

    DSM_StrCat(buf,"\n");

    if(-1 == g_t_up_log_fp)
    {
        g_t_up_log_fp = FS_Create(log_name,0);
        if(g_t_up_log_fp < 0)
        {
            return 0;
        }

    }


    if(g_t_up_log_fp >= 0)
    {
        FS_Write(g_t_up_log_fp,buf,strlen(buf));
        //  FS_Flush(g_t_up_log_fp);
    }
    return count;
}

UINT8* T_UP_StrChar(UINT8* str,UINT8 ch,UINT32 size)
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

UINT32 T_UP_Str2Hex(UINT8* str)
{
    UINT32 hex_value = 0;
    UINT32 tmp = 0;
    UINT8* p;
    UINT32 hex_x = 0;

    if(str == NULL)
    {
        return 0;
    }

    p = str;


    while(*p)
    {
        if(*p >= 'A' && *p <= 'F' )
        {
            hex_x = *p -'A' + 0x0a;
        }
        else if(*p >= 'a' && *p <= 'f' )
        {
            hex_x = *p -'a' + 0x0a;
        }
        else if(*p >= '0' && *p <= '9' )
        {
            hex_x = *p -'0';
        }
        hex_value = ((tmp <<4) | hex_x);
        tmp = hex_value;
        p++;
    }

    return hex_value;


}



INT32 T_UP_WriteToFlash(T_UP_DATA_INFO* pdata_info)
{
    INT32 result = 0;
    UINT32 writen = 0;
    INT32 ret = 0;
    UINT32 i;

    if(pdata_info->type == T_UP_DATA_TYPE_ERASE)
    {
        g_flash_info[g_flash_sec_count].addr = pdata_info->addr;
        g_flash_sec_count ++;

        result = memd_FlashErase((UINT8*)(pdata_info->addr),NULL);
        //result = DRV_EraseFlashSector(pdata_info->addr);
        if(result !=0 )
        {
            ret = -1;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:erase flash(0x%x) failed,err_code = %d.",pdata_info->addr,result);
            hal_HstSendEvent(0xEB000000 |pdata_info->addr);
        }
        else
        {
            fmg_PrintfSetXY(4,6);
            g_erase_count += 1;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:erase flash(0x%x) ok.",pdata_info->addr);
            hal_HstSendEvent(0x1B000000 |pdata_info->addr);
        }
        sxr_Sleep(20);
    }
    else
    {
#if 1

        for(i = 0; g_magic_number[i].magicNumber != 0 ; ++i)
        {
            if(g_magic_number[i].address == pdata_info->addr)
            {
                //SUL_MemCopy8(&(pdata_info->data[0]),&(g_magic_number[i].magicNumber),sizeof(g_magic_number[i].magicNumber));
                //T_UP_Trace(BASE_FFS_TS_ID,"TUP:addr = 0x%x,Migic = 0x%x.",pdata_info->addr,pdata_info->data[0]);
                g_maigic_sector |= 1<< i;
                break;
            }
        }
#else

        /*
        if((((UINT32)&_boot_loader_magic_number)&0x00FFFFFF) == pdata_info->addr)
        {
           SUL_MemCopy8(&(pdata_info->data[0]),&(g_magic_number[1].magicNumber),sizeof(g_magic_number[1].magicNumber));
           T_UP_Trace(BASE_FFS_TS_ID,"TUP:chage magic(_boot_loader_magic_number).addr = 0x%x,Migic = 0x%x.",pdata_info->addr,pdata_info->data[0]);
           hal_HstSendEvent(0x2B000000 |pdata_info->addr);
           hal_HstSendEvent(0x3B000000 |g_magic_number[1].magicNumber);
        }
        */
        /*else if(g_magic_number[0].address == pdata_info->addr)
        {
            SUL_MemCopy8(&(pdata_info->data[0]),&(g_magic_number[0].magicNumber),sizeof(g_magic_number[0].magicNumber));
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:change magic(const array).addr = 0x%x,Migic = 0x%x.",pdata_info->addr,pdata_info->data[0]);
            hal_HstSendEvent(0x4B000000 |pdata_info->addr);
            hal_HstSendEvent(0x5B000000 |g_magic_number[1].magicNumber);
        }*/

#endif
        result = DRV_WriteFlash(pdata_info->addr,pdata_info->data,pdata_info->len,&writen);
        if(result != 0 || writen != pdata_info->len)
        {
            hal_HstSendEvent(0xFB000000 |pdata_info->addr);
            fmg_Printf("TUP:write flash(0x%x,%d) failed,err_code = %d.\n",pdata_info->addr,pdata_info->len,result);
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:write flash(0x%x,%d) failed,err_code = %d.",pdata_info->addr,pdata_info->len,result);
            ret = -2;
        }
        else
        {
            g_code_size += pdata_info->len;
            hal_HstSendEvent(0x6B000000 |pdata_info->addr);
            hal_HstSendEvent(0x7B000000 |pdata_info->len);
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:write flash(0x%x) ok w_size = 0x%x.",pdata_info->addr,pdata_info->len);
        }
    }
    return ret;
}


INT32 t_up_GetFileName(UINT8* pLine,UINT8* pKeyStr,UINT8* pFileName)
{
    UINT32 statues = 0;
    UINT8* p = NULL;
    UINT32 name_size = 0;
    INT32 ret = 0;

    SUL_CharUpper(pLine);

    p = strstr(pLine,pKeyStr);
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


void t_up_ClearspecCh(UINT8* str)
{
    UINT32 vi=0,totalCnt=strlen(str);
    for(vi=0; vi<totalCnt; vi++)
    {
        if(*(str+vi)=='\n'||*(str+vi)=='\r' || *(str+vi)==0x20 || *(str+vi)==0x09)
        {
            *(str+vi) = '\0';
        }
    }
}

UINT32 t_up_GetLineSize(UINT8* str)
{
    // get line size.
    UINT8* p;
    UINT8* pfind = NULL;
    UINT32 line_size = 0;

    p = str;
    line_size = 32;
    do
    {
        pfind = strstr(p,"\n");
        if(!pfind)
        {
            break;
        }
        if(strlen(pfind) == 0)
        {
            break;
        }
        if(p[0] == '@' || p[0] == '#')
        {
            // do nothing.
        }
        else
        {
            line_size = (pfind - p + 1) < line_size ? (pfind - p + 1):line_size;
        }
        p = pfind + 1;
    }
    while(line_size == 32);
    return line_size;
}
void t_up_ReadLine(INT32 fhd,UINT8* buffer,UINT32 line_size)
{
    UINT8* pFindR = NULL;
    UINT8* pFindN = NULL;
    UINT8* pFind = NULL;
    UINT8 spare_size = 0;
    UINT8 szline[T_UP_LINE_MAX_LEN];

    *buffer = 0;

    if(g_up_info.pbuff == NULL)
    {
        g_up_info.pbuff= COS_MALLOC(T_UP_READ_LEN + T_UP_LINE_MAX_LEN);
        g_up_info.pos = g_up_info.pbuff;
        DSM_MemSet(g_up_info.pbuff,0,T_UP_READ_LEN + T_UP_LINE_MAX_LEN);
    }
    if(! g_up_info.totalSize)
    {
        g_up_info.totalSize = (INT32)FS_GetFileSize(fhd);
    }

    do
    {
        if(g_up_info.read_size == 0)
        {

            g_up_info.read_size = FS_Read(fhd,g_up_info.pbuff + spare_size,T_UP_READ_LEN);
            g_up_info.hasReadSize +=g_up_info.read_size;
            if(g_up_info.read_size == 0)
            {
                if(spare_size > 0)
                {
                    strcpy(buffer,szline);
                    spare_size = 0;
                    hal_HstSendEvent(0x990000aa);
                    T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLine:buffer !=NULL break,buffer:");
                    sxr_Sleep(10);
                }
                else
                {
                    hal_HstSendEvent(0x990000BB);
                    T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLine:buffer == NULL break.");
                }
                break;
            }
            g_up_info.pos = g_up_info.pbuff;
            g_up_info.read_size += spare_size;
            g_up_info.pbuff[g_up_info.read_size] = 0;
            if(g_up_info.line_size == 0)
            {
                g_up_info.line_size = t_up_GetLineSize(g_up_info.pbuff);
            }
        }

        pFindR  = T_UP_StrChar(g_up_info.pos,0x0D,line_size);
        pFindN  = T_UP_StrChar(g_up_info.pos,0x0A,line_size);

        pFind = pFindN;
        if(pFind != NULL)
        {
            if(pFindR  != NULL && pFindR < pFindN)
            {
                *pFindR = 0;
            }
            if(pFindN != NULL)
            {
                *pFindN  = 0;
            }

            if(pFind == g_up_info.pos)
            {
                g_up_info.read_size -= 1;
                g_up_info.pos += 1;

            }
            else
            {
                DSM_StrCpy(buffer,g_up_info.pos);
                g_up_info.read_size -= ((pFind - g_up_info.pos) + 1);
                g_up_info.pos = pFind + 1;
                break;
            }
        }
        else if(g_up_info.read_size > 0)
        {

            DSM_StrCpy(szline,g_up_info.pos);
            spare_size = DSM_StrLen(szline);
            DSM_MemSet(g_up_info.pbuff,0,T_UP_READ_LEN + T_UP_LINE_MAX_LEN);
            DSM_StrCpy(g_up_info.pbuff,szline);
            g_up_info.read_size = 0;

        }
    }
    while(1);
    //t_up_ClearspecCh(buffer);
}


void t_up_ReadLineExt(INT32 fhd,UINT8* buffer,UINT32 line_size,UINT8 filter_ch)
{
    UINT8* pfind;

    //T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:Begin.");
    while(1)
    {
        // T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:while begin.");
        memset(buffer,0,line_size);

        t_up_ReadLine(fhd,buffer,line_size);
        if(*buffer == 0)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:buffer == NULL break.");
            hal_HstSendEvent(0xBB000011);
            break;
        }
        //T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:buffer = %s.",buffer);

        pfind  = T_UP_StrChar(buffer,filter_ch,line_size);
        if(pfind != NULL)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:buffer pfind= %s.",pfind);
            *pfind = 0;
        }
        if(*buffer == 0)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:buffer = NULL continue.");
            hal_HstSendEvent(0xBB000012);
            continue;
        }
        else
        {
            //    T_UP_Trace(BASE_FFS_TS_ID,"t_up_ReadLineExt:buffer == %s break.",buffer);
            g_tstReadSize += strlen(buffer) < g_up_info.line_size ? g_up_info.line_size:strlen(buffer);
            //  hal_HstSendEvent(0xBB000022);
            break;
        }
    }
}

INT32 T_UP_Main(PF_T_UP_SHOWPROGRASS pFProgress)
{
    INT32 ret = 0;
    INT32 result;
    INT32 cfg_fd = -1;
    INT32 fd = -1;
    UINT8 szline[T_UP_LINE_MAX_LEN];
    UINT8 lod_name[FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN];
    UINT8 cfp_name[FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN];
    UINT8 opt[T_UPDATE_OPT_LEN];
    UINT32 uni_len = 0;
    UINT8* uni_name = NULL;
    UINT32 tmp_data;
    UINT32 progress = 0;
    UINT8* plod_name = NULL;
    UINT8* pcfp_name = NULL;
    UINT8* pclear_user_data_opt = NULL;
    UINT8* pcalibbuff = NULL;
    UINT32 calibsize = 0;
    UINT32 read_size = 0;
    UINT32 calib_base = 0;
    UINT32 calib_offset = 0;
    UINT32 writen_size = 0;
    UINT32 writen = 0;
    UINT32 i;
    UINT32 erase_addr;
    UINT32 sec_start;
    UINT32 sec_end;
    UINT32 bb_Offset=0;

    fmg_PrintfSetXY(2,6);
    fmg_Printf("t-upgrade ...\n");
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main begin.");
    hal_HstSendEvent(0x110000AA);

    bb_Offset=MEMBER_OFFSETOF(CALIB_BUFFER_T,bb);
    g_aud_sect_2_Offset=MEMBER_OFFSETOF(CALIB_BB_T,audio)+bb_Offset;    //  + T_UPDATE_AUD_SECT_1_LENGTH;
    g_aud_sect_2_length=MEMBER_SIZE(CALIB_BB_T,audio);
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:bb offset=0x%x,aud sect_2 offset=0x%x, length=0x%x.",bb_Offset,g_aud_sect_2_Offset,g_aud_sect_2_length);
    sxr_Sleep(20);
    // step_1: get update file name and cfp file name.

    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1 begin.");
        hal_HstSendEvent(0xAA00001A);
        result = ML_Assic2Unicode(T_UPDATE_CFG_NAME,  DSM_StrLen(T_UPDATE_CFG_NAME),&uni_name,&uni_len,NULL);
        if(result != 0)
        {
            ret = ERR_T_UP_TO_UNICODE_FAILED;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:local to uincode failed.err_code = %d.", result);
            hal_HstSendEvent(0xAA0000E1);
            DSM_ASSERT(0, "tupdate1 result = %d",result);
            goto  _func_end;
        }

        cfg_fd = FS_Open(uni_name,FS_O_RDONLY,0);
        if(cfg_fd < 0)
        {
            ret = ERR_T_UP_OPEN_FILE_FAILED;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:open cfg(0x%x) failed.err_code = %d.",uni_name,cfg_fd);
            hal_HstSendEvent(0xAA0000E2);
            DSM_ASSERT(0, "tupdate2 uniname = 0x%x",uni_name);
            goto  _func_end;
        }

        DSM_MemSet(lod_name,0x0,FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN);
        DSM_StrCpy(lod_name,"/");

        DSM_MemSet(cfp_name,0x0,FILE_NAME_MAX_LEN + FILE_PATH_MAX_LEN);
        DSM_StrCpy(cfp_name,"/");
        result = -1;
        do
        {
            if(NULL != plod_name &&
                    NULL != pcfp_name &&
                    NULL != pclear_user_data_opt)
            {
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1_3.");
                hal_HstSendEvent(0xAA000024);
                break;
            }

            DSM_MemSet(szline,0x0,T_UP_LINE_MAX_LEN);
            t_up_ReadLineExt(cfg_fd,szline,(T_UP_LINE_MAX_LEN-1),'#');
            if(0 == *szline)
            {
                //T_UP_Trace(BASE_FFS_TS_ID,"TUP:cfg file error,can't find the T_UPDATE line.");
                //  continue;
                break;
            }
            // update key
            result = t_up_GetFileName(szline,T_UPDATE_KEY,lod_name + 1);
            if(result == 0)
            {
                plod_name = lod_name;
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1_1.lod_name = %s.",lod_name);
                hal_HstSendEvent(0xAA000021);
                continue;
            }

            // cfp key
            result = t_up_GetFileName(szline,T_UPDATE_CFP_KEY,cfp_name + 1);
            if(result == 0)
            {
                pcfp_name = cfp_name;
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1_2.cfp_name = %s.",cfp_name);
                hal_HstSendEvent(0xAA000022);
                continue;
            }

            // clear user data key
            result = t_up_GetFileName(szline,T_CLEAR_USER_DATA_KEY,opt);
            if(result == 0)
            {
                pclear_user_data_opt = opt;
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1_3.clear_user_data_opt = %s.",pclear_user_data_opt);
                hal_HstSendEvent(0xAA000023);
                if(DSM_StrNCaselessCmp(pclear_user_data_opt,"YES",3) == 0)
                {
                    result = DRV_EraseAllUserDataSector();
                    if(result == ERR_SUCCESS)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:clear user data ok.");
                        hal_HstSendEvent(0xAA00002C);
                    }
                    else
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:clear user data error. err_code = %d",result);
                        hal_HstSendEvent(0xAA00002E);
                    }
                }
                else
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP: clear user data opt = %s.",pclear_user_data_opt);
                    hal_HstSendEvent(0xAA00002E);
                }
                continue;
            }

        }
        while(1);
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step1 end.");
        hal_HstSendEvent(0xAA0000E0);

    }

    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step2.");
    hal_HstSendEvent(0x110000BB);
    // step_2: open update file and copy the data to flash.
    if(NULL != plod_name)
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2_1.");
        hal_HstSendEvent(0xBB000011);

        // step_2_1: open update file,malloc for g_pdata_info.
        if(uni_name)
        {
            CSW_Free(uni_name);
            uni_name = NULL;
        }
        result = ML_Assic2Unicode(plod_name,  DSM_StrLen(plod_name),&uni_name,&uni_len,NULL);

        if(result != 0)
        {
            ret = ERR_T_UP_TO_UNICODE_FAILED;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:local to uincode failed.err_code = %d.", result);
            hal_HstSendEvent(0xBB0000E1);
            DSM_ASSERT(0, "tupdate3 result = %d",result);
            goto  _func_end;
        }

        g_up_info.pos = 0;
        g_up_info.totalSize = 0;
        g_up_info.read_size = 0;
        g_up_info.hasReadSize = 0;
        g_up_info.line_size = 0;

        fd = FS_Open(uni_name,FS_O_RDONLY,0);
        if(fd < 0)
        {
            ret = ERR_T_UP_OPEN_FILE_FAILED;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:open file failed.err_code = %d.", result);
            hal_HstSendEvent(0xBB0000E2);
            DSM_ASSERT(0, "tupdate4 uniname = 0x%x",uni_name);
            goto  _func_end;
        }

        g_pdata_info = COS_MALLOC(sizeof(T_UP_DATA_INFO));
        if(g_pdata_info == NULL)
        {
            ret = ERR_T_UP_MALLOC_FAILED;
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:malloc(0x%x) failed.\n",sizeof(T_UP_DATA_INFO));
            hal_HstSendEvent(0xBB0000E3);
            DSM_ASSERT(0, "tupdate5 malloc = NULL");
            goto  _func_end;
        }

        g_pdata_info->addr = 0;
        g_pdata_info->len = 0;

        //step_2_2:
        //   1)read the update file from t flash,
        //   2)erase sector
        //   3)convert ascci to hex
        //   4)write to flash.
        //   5)close file.
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2_2.");
        hal_HstSendEvent(0xBB000022);
        do
        {
            t_up_ReadLineExt(fd,szline,(T_UP_LINE_MAX_LEN-1),'#');

            if(szline[0] == '@')
            {
                if(g_pdata_info->len >= 4)
                {
                    result = T_UP_WriteToFlash(g_pdata_info);
                    if(result != 0)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:write update data to flash failed.err_code = %d.", result);
                        hal_HstSendEvent(0xBB0000E4);
                        ret = result == -1 ? ERR_T_UP_ERASE_FLASH_FAILED : ERR_T_UP_WRITE_FLASH_FAILED;
                        DSM_ASSERT(0, "tupdate6 result = %d",result);
                        goto _func_end;
                    }
                }
                g_pdata_info->len = 0;
                g_pdata_info->type =  T_UP_DATA_TYPE_ERASE;
                tmp_data= T_UP_Str2Hex(szline + 1);
                g_pdata_info->addr = (tmp_data & 0xffffff);
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2_2_1. line = %s",szline);
                hal_HstSendEvent(0xBE000000|g_pdata_info->addr);
                result = T_UP_WriteToFlash(g_pdata_info);
                if(result != 0)
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:erase flash failed.err_code = %d.", result);
                    hal_HstSendEvent(0xBB0000E5);
                    ret = result == -1 ? ERR_T_UP_ERASE_FLASH_FAILED : ERR_T_UP_WRITE_FLASH_FAILED;
                    DSM_ASSERT(0, "tupdate7 result = %d",result);
                    goto _func_end;
                }
                if(progress < 100)
                {
                    progress = g_tstReadSize*100/g_up_info.totalSize;
                    g_tstReadSize += (g_up_info.line_size + 1);
                    pFProgress(progress);
                    //progress ++;
                }
                g_pdata_info->type = T_UP_DATA_TYPE_DATA;
            }
            else if(szline[0] != 0)
            {
                if(g_pdata_info->len >= T_UP_BLOCK_LEN -4)
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2_2_21. line = %s",szline);
                    hal_HstSendEvent(0xBB000221);
                    result = T_UP_WriteToFlash(g_pdata_info);
                    if(result != 0)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:write update data to flash failed.err_code = %d.", result);
                        hal_HstSendEvent(0xBB0000E6);
                        ret = result == -1 ? ERR_T_UP_ERASE_FLASH_FAILED : ERR_T_UP_WRITE_FLASH_FAILED;
                        DSM_ASSERT(0, "tupdate7 result = %d",result);
                        goto _func_end;
                    }
                    else
                    {
                        g_pdata_info->addr += g_pdata_info->len;
                    }
                    g_pdata_info->len = 0;
                }
                tmp_data =  T_UP_Str2Hex(szline);
                SUL_MemCopy8((g_pdata_info->data + g_pdata_info->len),&tmp_data,4);
                g_pdata_info->len += 4;
            }
            else
            {
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2_2_3.");
                hal_HstSendEvent(0xBB000033);

                result = T_UP_WriteToFlash(g_pdata_info);
                if(result != 0)
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:write update data to flash failed.err_code = %d.", result);
                    hal_HstSendEvent(0xBB0000E6);
                    ret = result == -1 ? ERR_T_UP_ERASE_FLASH_FAILED : ERR_T_UP_WRITE_FLASH_FAILED;
                    DSM_ASSERT(0, "tupdate8 result = %d",result);
                    goto _func_end;
                }
                else
                {
                    g_pdata_info->addr += g_pdata_info->len;
                }
                g_pdata_info->len = 0;

                if(fd >= 0)
                {
                    result = FS_Close(fd);
                    if(result < 0)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:close update file failed. fd = 0x%x,err_code = 0x%x.",fd,result);
                    }
                    else
                    {

                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:close update file ok.",result);
                    }
                    fd = -1;
                }
                if(uni_name)
                {
                    CSW_Free(uni_name);
                    uni_name = NULL;
                }
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:code size = 0x%x,erase size = 0x%x.",g_code_size,g_erase_count);
                // fmg_Printf("TUP:code size = 0x%x,erase size = 0x%x.\n",g_code_size,g_erase_count);
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:copy update data complete.");
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step2 end.");
                hal_HstSendEvent(0xBB0000E0);
                // sxr_Sleep(20);
                break;
            }
        }
        while(1);
    }
    else
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:get update file name failed.");
        hal_HstSendEvent(0xBB0000EE);
        //  ret = ERR_T_UP_NOT_EXIST_LOD;
    }
    FS_Flush(g_t_up_log_fp);

    // step_3: open cfp file and replace the cfp data.
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step3 begin.");
    hal_HstSendEvent(0x110000CC);
    if(NULL != pcfp_name)
    {
        ret = 0;
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_1.");
        hal_HstSendEvent(0xCC000011);

        // step_3_1: open cfp file.if g_pdata_info is NULL,malloc for it .
        if(uni_name)
        {
            CSW_Free(uni_name);
            uni_name = NULL;
        }
        result = ML_Assic2Unicode(pcfp_name,  DSM_StrLen(pcfp_name),&uni_name,&uni_len,NULL);

        if(result != 0)
        {
            hal_HstSendEvent(0xCC0000E1);
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:cfp local to uincode failed.err_code = %d.", result);
            ret = ERR_T_UP_TO_UNICODE_FAILED;
            DSM_ASSERT(0, "tupdate9 result = %d",result);
            goto  _func_end;
        }

        g_up_info.pos = 0;
        g_up_info.totalSize = 0;
        g_up_info.read_size = 0;
        g_up_info.hasReadSize = 0;
        g_up_info.line_size = 0;
        fd = FS_Open(uni_name,FS_O_RDONLY,0);
        if(fd < 0)
        {
            hal_HstSendEvent(0xCC0000E2);
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:open cfp file failed.err_code = %d.", result);
            ret = ERR_T_UP_OPEN_FILE_FAILED;
            DSM_ASSERT(0, "tupdate10 uni_name = 0x%x.",uni_name);
            goto  _func_end;
        }
        else
        {
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_2.fd = %d.",fd);
            hal_HstSendEvent(0xC2000000|fd);
        }

        //sxr_Sleep(10);
        //step_3_2:
        //   1)read the cfp data file from t flash
        //   2)read calib data from flash.
        //   3)erase sector
        //   4)convert ascci to hex
        //   5)replace cfp.
        //   6)write to flash.
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3.");
        hal_HstSendEvent(0xCC000033);
        do
        {
            t_up_ReadLineExt(fd,szline,(T_UP_LINE_MAX_LEN-1),'#');
            if(szline[0] == '@')
            {
                tmp_data= T_UP_Str2Hex(szline + 1) ;
                calib_base = (tmp_data & 0xffffff);
                if(((UINT32)(&_calib_start)& 0xffffff) != calib_base)
                {
                    // tstcalib_start1 = (UINT32)_calib_start;
                    hal_HstSendEvent(((UINT32)(&_calib_start)& 0xffffff));
                    hal_HstSendEvent(calib_base);
                    hal_HstSendEvent(0xCC0000E9);
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:calib_base error.calib_base = 0x%x.",calib_base);
                    ret = ERR_T_UP_CALIB_ADDR_ERROR;
                    DSM_ASSERT(0, "tupdate11 calib  = 0x%x,error.",calib_base);
                }
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3_1.calib_base = 0x%x.",calib_base);
                hal_HstSendEvent(0xC1000000 | calib_base);
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:calib_base = 0x%x(0x%x).",calib_base,tmp_data);
                calibsize = (UINT32)(&_factory_start) - (UINT32)(&_calib_start);
                pcalibbuff = COS_MALLOC(calibsize + T_UP_LINE_MAX_LEN);
                if(NULL == pcalibbuff)
                {
                    hal_HstSendEvent(0xCC0000E3);
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:malloc for calib  failed.err_code = %d.", result);
                    ret = ERR_T_UP_MALLOC_FAILED;
                    DSM_ASSERT(0, "tupdate11 malloc = NULL.");
                    goto _func_end;
                }

                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3_2.calibsize = 0x%x.",calibsize);
                hal_HstSendEvent(0xC2000000 | calibsize);
                //read the cfp data file from t flash
                result = DRV_ReadFlash(calib_base,pcalibbuff,calibsize,&read_size);
                if(result != 0 || read_size != calibsize)
                {
                    hal_HstSendEvent(0xCC0000E4);
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:read calib  failed.err_code = %d.", result);
                    ret = ERR_T_UP_READ_FLASH_FAILED;
                    goto _func_end;
                }
                else
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:read calib  ok.balib_base = 0x%x,calibsize = 0x%x.", calib_base,calibsize);
                    hal_HstSendEvent(0xC2200000 | read_size);
                }
            }
            else if(szline[0] != 0)
            {
                UINT8 copyflag=0;
                tmp_data =  T_UP_Str2Hex(szline);
                if(( calib_offset < T_UPDATE_AUD_SECT_1_LENGTH ) ||
                        (calib_offset >= g_aud_sect_2_Offset && (calib_offset < (g_aud_sect_2_Offset + g_aud_sect_2_length))))
                {
                    copyflag=1;
                    SUL_MemCopy8((pcalibbuff + calib_offset),&tmp_data,4);
                }

                T_UP_Trace(BASE_FFS_TS_ID," %d : calib_offset = 0x%x,szline = %s,hex = 0x%x", copyflag,calib_offset,szline,tmp_data);
                calib_offset += 4;
            }
            else
            {

                erase_addr = calib_base;
                while( erase_addr < (calib_base + calibsize))
                {
                    result = memd_FlashErase((UINT8*)(erase_addr),NULL);
                    if(result != 0)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:erase calib sector failed.err_code = %d.", result);
                        hal_HstSendEvent(0xCC0000E5);
                        ret = ERR_T_UP_ERASE_FLASH_FAILED;
                        goto _func_end;
                    }
                    else
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3_4 erase calib sector ok.addr = 0x%x.",erase_addr);
                        hal_HstSendEvent(0xC4000000 | erase_addr);
                    }
                    sec_start = 0;
                    sec_end = 0;
                    T_UP_FlashGetSectorLimits(erase_addr,&sec_start,&sec_end);
                    erase_addr += (sec_end - sec_start);
                }


                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3_5. calib_size = 0x%x.",calibsize);
                hal_HstSendEvent(0xC5000000 | calibsize);
                result = DRV_WriteFlash(calib_base,pcalibbuff,calibsize,&writen_size);
                if(result != 0 || writen_size != calibsize)
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:write calib  failed.err_code = %d.", result);
                    hal_HstSendEvent(0xCC0000E6);
                    ret = ERR_T_UP_WRITE_FLASH_FAILED;
                    goto _func_end;
                }
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3_3_5. writen_size = 0x%x.",writen_size);
                hal_HstSendEvent(0xC6000000 | writen_size);
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:step3 end.");
                hal_HstSendEvent(0xCC0000E0);
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:copy cfp complete.");
                break;
            }
        }
        while(1);
    }
    else
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:get cfp file name failed.");
        hal_HstSendEvent(0xCC0000EE);
        // ret = ERR_T_UP_NOT_EXIST_CFG;
        // goto  _func_end;
    }
    for (i = 0; g_maigic_sector >> i; ++i)
    {
        if(g_maigic_sector & (1<<i))
        {
            if (g_magic_number[i].magicNumber != 0)
            {
                result = DRV_WriteFlash(g_magic_number[i].address,(CONST BYTE* )(&(g_magic_number[i].magicNumber)),sizeof(g_magic_number[i].magicNumber),&writen);
                if(result != 0 || writen != sizeof(g_magic_number[i].magicNumber))
                {
                    fmg_Printf("TUP:write flash(0x%x,%d) failed,err_code = %d.\n",g_magic_number[i].address,sizeof(g_magic_number[i].magicNumber),result);
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:write flash(0x%x,%d) failed,err_code = %d.",g_magic_number[i].address,sizeof(g_magic_number[i].magicNumber),result);
                    ret = -2;
                }
                else
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:write migic(0x%x) ok w_size = 0x%x,migic = 0x%x.",g_magic_number[i].address,sizeof(g_magic_number[i].magicNumber),g_magic_number[i].magicNumber);
                }
            }
        }
    }
// step_4: close file,free memory.

_func_end:
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step_4 begin.");
    hal_HstSendEvent(0x110000DD);
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step_4_1.");
    hal_HstSendEvent(0xDD000011);
    if(fd >= 0)
    {
        result = FS_Close(fd);
        if(result < 0)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:close file failed. fd = 0x%x,err_code = 0x%x.",fd,result);
        }
        else
        {

            T_UP_Trace(BASE_FFS_TS_ID,"TUP:close file ok.",result);
        }
        fd = -1;
    }
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step_4_2.");
    hal_HstSendEvent(0xDD000012);
    if(uni_name)
    {
        CSW_Free(uni_name);
        uni_name = NULL;
    }
    T_UP_Trace(BASE_FFS_TS_ID,"TUP:main step_4_3.");
    hal_HstSendEvent(0xDD000013);
    if(g_pdata_info)
    {
        COS_FREE(g_pdata_info);
        g_pdata_info = NULL;
    }

    if(pcalibbuff)
    {
        COS_FREE(pcalibbuff);
        pcalibbuff = NULL;
    }

    if(ret == 0)
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:main end(ok).");
        hal_HstSendEvent(0x110000E0);
    }
    else
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:main end(error).");
        hal_HstSendEvent(0x110000EE);
    }

    // close log fp.
    if(-1 != g_t_up_log_fp)
    {
        FS_Close(g_t_up_log_fp);
        g_t_up_log_fp = -1;
    }

    //sxr_Sleep(20);

    return ret;
}


INT32 T_UP_FsInit()
{
    INT32 err_code = 0x00;
    UINT32 fs_dev_count = 0;
    FS_DEV_INFO* fs_dev_info = NULL;
    DRV_FLASH_DEV_INFO flash_dev_info = {0,0,0};

    CSW_TRACE(BASE_FFS_TS_ID, TSTXT("\nBAL_TaskInit Start. \n"));
    hal_HstSendEvent(0x66000011);

    csw_MemIint();
    DRV_FlashPowerUp();

    DRV_FlashInit();


    ML_Init();
    ML_SetCodePage(ML_ISO8859_1);

    hal_HstSendEvent(0x66000012);
    // Get the flash information.
    err_code = DRV_FlashRegionMap( &flash_dev_info);
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_FFS_TS_ID,"DRV_FlashRegionMap OK.\n");
        hal_HstSendEvent(0x66000013);
    }
    else
    {
        CSW_TRACE(BASE_FFS_TS_ID,"DRV_FlashRegionMap ERROR, Error code: %d. \n",err_code);
        hal_HstSendEvent(0x660000E3);
        goto _func_end;
    }

    err_code = DSM_DevInit();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_FFS_TS_ID,"DSM_DevInit OK.\n");
        hal_HstSendEvent(0x66000014);
    }
    else
    {
        CSW_TRACE(BASE_FFS_TS_ID,"DSM_DevInit ERROR, Error code: %d. \n",err_code);
        hal_HstSendEvent(0x660000E4);
        goto _func_end;
    }

    err_code = FS_PowerOn();
    if(ERR_SUCCESS == err_code)
    {
        CSW_TRACE(BASE_FFS_TS_ID,"FS Power On Check OK.\n");
        hal_HstSendEvent(0x66000015);
    }
    else
    {
        CSW_TRACE(BASE_FFS_TS_ID,"FS Power On Check ERROR, Error code: %d. \n",err_code);
        hal_HstSendEvent(0x660000E5);
        goto _func_end;
    }

    // Get FS device table.
    err_code = FS_GetDeviceInfo(&fs_dev_count, &fs_dev_info);
    if(err_code != ERR_SUCCESS)
    {
        CSW_TRACE(BASE_FFS_TS_ID,"Device not register.\n");
        hal_HstSendEvent(0x66000016);
        fs_dev_count = 0;
    }
    else
    {
        hal_HstSendEvent(0x660000E6);
    }
    // Mount MMC0
    err_code = FS_MountRoot("MMC0");
    if(ERR_SUCCESS == err_code || ERR_FS_HAS_MOUNTED == err_code)
    {
        CSW_TRACE(BASE_FFS_TS_ID,"TUP: MountRoot(MMC0) ok.");
        hal_HstSendEvent(0x66000017);
    }
    else
    {
        CSW_TRACE(BASE_FFS_TS_ID,"TUP: MountRoot(MMC0) ERROR, Error code: %d.",err_code);
        hal_HstSendEvent(0x660000E7);
        goto _func_end;
    }


_func_end:
    if(0 == err_code)
    {
        hal_HstSendEvent(0x660000E0);
    }
    else
    {
        hal_HstSendEvent(0x660000EE);
    }
    return err_code;
}


#if 0
INT32 T_UP_Verify(void)
{
    INT32 ret = 0;
    INT32 result;
    INT32 fd = -1;
    UINT8 szline[T_UP_LINE_MAX_LEN];
    UINT32 uni_len = 0;
    UINT8* uni_name = NULL;
    UINT32 tmp_data;
    UINT8* buffer = NULL;
    UINT8* f_buffer = NULL;
    UINT8* p;
    UINT32 i,j;
    UINT32 addr;
    UINT32 read_sz = 0;
    UINT32 read_offset = 0;
    UINT32 r_size = 0;

    fmg_Printf("\nt-upgrade verify begin.\n\n");

    // step_1: open verify file.
    result = ML_LocalLanguage2UnicodeBigEnding(T_UPDATE_VERIFY_NAME,  DSM_StrLen(T_UPDATE_VERIFY_NAME),&uni_name,&uni_len,NULL);
    if(result != 0)
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity local to uincode failed.err_code = %d.", result);
        ret = ERR_T_UP_TO_UNICODE_FAILED;
        goto  _func_end;
    }

    fd = FS_Create(uni_name,0);
    if(fd < 0)
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity create file failed.err_code = %d.", result);
        ret = ERR_T_UP_OPEN_FILE_FAILED;
        goto  _func_end;
    }

    buffer = COS_MALLOC(T_UP_BLOCK_LEN);
    if(buffer == NULL)
    {
        fmg_Printf("TUP:verity malloc(0x%x) failed.\n",T_UP_BLOCK_LEN);
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity malloc(0x%x) failed.\n",T_UP_BLOCK_LEN);
        ret = ERR_T_UP_MALLOC_FAILED;
        goto  _func_end;
    }

    f_buffer = COS_MALLOC(T_UP_READ_LEN);
    if(f_buffer == NULL)
    {
        fmg_Printf("TUP:verity malloc(0x%x) failed.\n",T_UP_READ_LEN);
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity malloc(0x%x) failed.\n",T_UP_READ_LEN);
        ret = ERR_T_UP_MALLOC_FAILED;
        goto  _func_end;
    }

    for(i = 0; i < g_flash_sec_count; i++)
    {
        DSM_MemSet(szline,0,T_UP_LINE_MAX_LEN);
        DSM_StrPrint(szline,"@01%06x\n",g_flash_info[i].addr);

        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity:%s",szline);
        FS_Write(fd, (UINT8*)szline,DSM_StrLen(szline));
        read_offset = 0;
        //p = f_buffer;
        while(read_offset < T_UP_SEC_SIZE)
        {
            addr = g_flash_info[i].addr + read_offset;
            r_size = T_UP_SEC_SIZE - read_offset > T_UP_BLOCK_LEN ? T_UP_BLOCK_LEN: T_UP_SEC_SIZE - read_offset;
            result = DRV_ReadFlash(addr,buffer,r_size,&read_sz);
            if(result != 0)
            {
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity read flash failed.err_code = %d.", result);
                ret = ERR_T_UP_READ_FLASH_FAILED;
                goto  _func_end;
            }
            else
            {
                T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity:read flash size= 0x%x",read_sz);
            }
            read_offset += read_sz;
            j = 0;
            while(j*4 < read_sz)
            {
                DSM_MemSet(f_buffer,0,T_UP_READ_LEN);
                p = f_buffer;
                while(p - f_buffer  <= T_UP_READ_LEN - 9)
                {
                    tmp_data = T_UPDATE_MAKEFATINT32(*(buffer + j*4),*(buffer + j*4 + 1),*(buffer + j*4 + 2),*(buffer + j*4 +3));
                    DSM_StrPrint(p,"%08x\n",tmp_data);
                    j++;
                    p += DSM_StrLen(p);
                    if(j*4 >= read_sz)
                    {
                        T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity j = 0x%x, j*4 = 0x%x,break,",j,j*4);
                        break;
                    }
                    //T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity Line = %08x.",tmp_data);

                }
                result = FS_Write(fd,f_buffer,p - f_buffer);
                if(result != p - f_buffer)
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity write file failed.err_code = %d.", result);
                    ret = ERR_T_UP_WRITE_FILE_FAILED;
                    goto  _func_end;
                }
                else
                {
                    T_UP_Trace(BASE_FFS_TS_ID,"TUP:verity:Write size= 0x%x",p - f_buffer);
                }
                //sxr_Sleep(5);
            }
            // //sxr_Sleep(10);
        }
    }


_func_end:
    if(fd >= 0)
    {
        result = FS_Close(fd);
        if(result < 0)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:close file failed. fd = 0x%x,err_code = 0x%x.",fd,result);
        }
        else
        {

            T_UP_Trace(BASE_FFS_TS_ID,"TUP:close file ok.",result);
        }
    }
    if(uni_name)
    {
        CSW_Free(uni_name);
        uni_name = NULL;
    }

    T_UP_Trace(BASE_FFS_TS_ID,"TUP:verify end.");
    fmg_Printf("TUP:verify end.\n");

    if(buffer)
    {
        COS_FREE(buffer);
        buffer = NULL;
    }

    if(f_buffer)
    {
        COS_FREE(f_buffer);
        f_buffer = NULL;
    }
    //sxr_Sleep(20);

    return ret;
}


INT32 T_UP_GetFileList(UINT8* pFind, T_UP_FILE_LIST* pFileList,UINT32 uCount)
{
    INT32 result;
    INT32 fd = -1;
    UINT8 *uni_name = NULL;
    UINT32 uni_len = 0;
    UINT8 *local_name = NULL;
    UINT32 len = 0;
    FS_FIND_DATA find_data;
    UINT32 node_count = 0;
    INT32 ret = 0;

    result = ML_LocalLanguage2UnicodeBigEnding(pFind, DSM_StrLen(pFind),&uni_name,&uni_len,NULL);

    if(result != 0)
    {
        T_UP_Trace(BASE_FFS_TS_ID,"TUP:local to uincode failed.err_code = %d.", result);
        ret = ERR_T_UP_TO_UNICODE_FAILED;
        goto _func_end;
    }


    fd = FS_FindFirstFile(uni_name, &find_data);
    if(fd < 0)
    {
        ret = ERR_T_UP_NOT_EXIST_LOD;
        goto _func_end;
    }

    if(uni_name)
    {
        CSW_Free(uni_name);
        uni_name = NULL;
    }

    while(node_count < uCount)
    {
        result = ML_Unicode2LocalLanguage(find_data.st_name, DSM_UnicodeLen(find_data.st_name),&local_name,&len,NULL);
        if(result != 0)
        {
            T_UP_Trace(BASE_FFS_TS_ID,"TUP:local to uincode failed.err_code = %d.", result);
            ret = ERR_T_UP_TO_LOCAL_FAILED;
            break;
        }
        else
        {
            if(DSM_StrNCaselessCmp(local_name, "ramImage.lod",DSM_StrLen("local_name")) == 0)
            {
                // donoting.
            }
            else
            {
                DSM_StrCpy((UINT8*)pFileList[node_count].szFile,(UINT8*)local_name);
                node_count ++;
            }

            if(local_name)
            {
                CSW_Free(local_name);
                local_name = NULL;
            }

        }
        result = FS_FindNextFile(fd, &find_data);
        if(result != 0)
        {
            break;
        }
    }



_func_end:
    FS_Close(fd);
    return node_count;


}

#endif


