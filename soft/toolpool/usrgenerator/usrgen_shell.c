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

#ifdef _USERGEN_SHELL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs_types.h"

#include "dsm_cf.h"
#include "ml.h"
#include "fs.h"
#include "usrgen.h"
#include "memd_m.h"
#include "memdp.h"
#include "dsm_dev_driver.h"
#include "dsm_config.h"
#include "vds_local.h"
#include "vds_cd.h"
#include "dsm_config.h"
#include "tgt_app_cfg.h"

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
int verbose = 0;  /* extra output when running */

extern char usrgen_version_string[]; /*version string */
extern UINT8* g_pFlashImg;
#ifdef MMI_ON_WIN32
#else
extern   VOID memd_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize);
#endif

UINT32 g_iUsrSecCnt = 0;
UINT32 g_iCfgSecCnt = 0;
UINT32 g_iRemainSecCnt = 0;


// =============================================================================
//  FUNCTIONS
// =============================================================================
static UINT32 usrgen_OutputText(UINT16 nIndex, PCSTR fmt, ...)
{
    if((nIndex == 0xffff)||(nIndex == 0xff)||(verbose > 0)) {
        char uart_buf[256];
        va_list ap;
        va_start (ap, fmt);
        vsnprintf(uart_buf, sizeof(uart_buf),(const char*)fmt, ap);
        va_end (ap);
        if(nIndex == 0xffff) {
            fprintf(stderr, "%s\n", uart_buf);
        } else {
            fprintf(stdout, "%s\n", uart_buf);
        }
    }
    return 0;
}


static UINT32 usrgen_check()
{
    UINT32 i,j;
    UINT32 cfg_sec_cnt = 0;
    UINT32 sec_cnt;
    UINT32 sec_sz;
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;
    UINT32 pCnt = 0;
    DSM_CONFIG_T* dsm_config = (DSM_CONFIG_T*)tgt_GetDsmCfg();

    memd_FlashGetUserFieldInfo(&iBaseAddr,&iTotalSize,&iSectorSize,&iBlockSize);

    sec_cnt = iTotalSize/iSectorSize;
    g_iUsrSecCnt = sec_cnt;
    sec_sz = iSectorSize;

    usrgen_OutputText(0xff, (PCSTR)"usrgen_check: sec_cnt=%d, sec_sz=%d\n", sec_cnt,sec_sz);

    for(i = 0; i < dsm_config->dsmPartitionNumber; i++)
    {
        if(dsm_config->dsmPartitionInfo[i].eDevType == DSM_MEM_DEV_FLASH)
        {
            cfg_sec_cnt += dsm_config->dsmPartitionInfo[i].uSecCnt;
        }
    }
    g_iCfgSecCnt = cfg_sec_cnt + 1;
    if(cfg_sec_cnt+1 > sec_cnt)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_check: sec_cnt=%d, cfg_sec_cnt+1=%d\n", sec_cnt,cfg_sec_cnt+1);
        return -1;
    }


    for(i = 0; i < dsm_config->dsmPartitionNumber; i++)
    {
        if(dsm_config->dsmPartitionInfo[i].eDevType != DSM_MEM_DEV_FLASH)
        {
            continue;
        }
        for(j = 0; j < dsm_config->dsmPartitionInfo[i].uSecCnt; j++)
        {
            pCnt += sec_sz;
        }

        usrgen_OutputText(0xff, (PCSTR)"usrgen_check: make partiton(%s) sec_cnt = %d.\n", dsm_config->dsmPartitionInfo[i].szPartName,j);

    }

    usrgen_OutputText(0xff, (PCSTR)"usrgen_check: remain sec_cnt = %d.\n", (((sec_cnt*sec_sz) - pCnt)/sec_sz));

    return (UINT32)(sec_cnt*sec_sz);

}

UINT32 usrgen_writeImage(FILE *fp)
{
    UINT32 iBaseAddr = 0;
    UINT32 iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 0;

    if(fp == NULL) {
        return -1;
    }

    if(NULL == g_pFlashImg) {
        return -2;
    }
#ifdef MMI_ON_WIN32
    iBaseAddr = 0;
    iSectorSize = 0;
    iBlockSize = 0;
    iTotalSize = USERGEN_FLASH_SECTORCOUNT*USERGEN_FLASH_SECTORSIZE;
#else
    memd_FlashGetUserFieldInfo(&iBaseAddr,&iTotalSize,&iSectorSize,&iBlockSize);
#endif

    fwrite(g_pFlashImg,iTotalSize, 1, fp);

    return 0;
}
UINT32 usrgen_writelodfile(FILE *fp)
{
    #define MAKEUINT(a1,a2,a3,a4) ((a1|(a2<<8)) |((a3|(a4<<8)) << 16))
    UINT32 iBaseAddr = 0;
    UINT32 iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 0;
    UINT32 iOffset = 0;
    UINT8 line_buff[32];
    UINT8* p;
    UINT32 iData;
    UINT32 iCheckSum = 0;;
    if(fp == NULL) {
        return -1;
    }
    if(NULL == g_pFlashImg) {
        return -2;
    }
#ifdef MMI_ON_WIN32
    iBaseAddr = 0;
    iSectorSize = 0;
    iBlockSize = 0;
    iTotalSize = USERGEN_FLASH_SECTORCOUNT*USERGEN_FLASH_SECTORSIZE;
#else
    memd_FlashGetUserFieldInfo(&iBaseAddr,&iTotalSize,&iSectorSize,&iBlockSize);
#endif

    for(iOffset = 0; iOffset < iTotalSize; iOffset += 4)
    {
        p = g_pFlashImg + iOffset;
        memset(line_buff,0,sizeof(line_buff));
        if(iOffset % iSectorSize == 0)
        {
            sprintf((char*)line_buff,"@08%x\r\n",(unsigned int)((iBaseAddr & 0xffffff) + iOffset));
            fwrite(line_buff,strlen((char*)line_buff), 1, fp);
        }
        iData = MAKEUINT(p[0],p[1],p[2],p[3]);
        iCheckSum += iData;
        memset(line_buff,0,sizeof(line_buff));
        sprintf((char*)line_buff,"%08x\r\n",(unsigned int)iData);
        fwrite(line_buff,strlen((char*)line_buff), 1, fp);
    }
    memset(line_buff,0,sizeof(line_buff));
    sprintf((char*)line_buff,"#checksum=%08x\r\n",(unsigned int)iCheckSum);
    fwrite(line_buff,strlen((char*)line_buff), 1, fp);
    return 0;
}


UINT32  usrgen_make_user_data(FILE *bin_fp,FILE *lod_fp)
{
    INT32 result;
    UINT32 iactSize = 0;
    UINT8 pDirT[] = {'/',0,'t',0,0,0};
    // UINT8 pDirHome[] = {'/',0,'h',0,'o',0,'m',0,'e',0,0,0};
    // UINT8 pDirWap[] = {'/',0,'_',0,'w',0,'a',0,'p',0,'_',0,'m',0,'m',0,'s',0,0,0};

    if((iactSize = usrgen_check()) == -1) {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:vds check fail.\n");
        return 1;
    }

    result = usrgen_fs_init();

    if(0 !=result)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:usrgen_fs_init fail.result = %d\n",result);
        return 2;
    }

    result = FS_MakeDir(pDirT,0);
    if(0 !=result)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:make dir(t) fail.\n");
        return 3;
    }
    /*
    result = FS_MakeDir(pDirHome,0);
    if(result != ERR_SUCCESS &&
       result != ERR_FS_FILE_EXIST)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:make dir(home) fail.\n");
        return 4;
    }

    result = FS_MakeDir(pDirWap,0);
    if(result != ERR_SUCCESS &&
       result != ERR_FS_FILE_EXIST)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:make dir(_wap_mms) fail.\n");
        return 5;
    }
    */
#ifndef CHIP_IS_MODEM
    result = usrgen_nvram();
    if(0 !=result)
    {
        usrgen_OutputText(0xffff,(PCSTR) "usrgen_make_user_data:usrgen_nvram fail.\n");
        return 6;
    }

    result = usrgen_wap_mms();
    if(0 !=result)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:usrgen_wap_mms fail.\n");
        return 7;
    }
#endif
    result = usrgen_prefab_file();
    if(0 !=result)
    {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen_make_user_data:usrgen_prefab_file fail.\n");
        return 8;
    }

    result = usrgen_writeImage(bin_fp);
    if(0 !=result)
    {
        usrgen_OutputText(0xffff,(PCSTR) "usrgen_make_user_data:usrgen_writeImage fail.\n");

        return 9;
    }
    result = usrgen_writelodfile(lod_fp);
    if(0 !=result)
    {
        usrgen_OutputText(0xffff,(PCSTR) "usrgen_make_user_data:usrgen_writeImage fail.\n");
        return 9;
    }

    usrgen_OutputText(0xffff,(PCSTR) "usrgen_make_user_data:User Data Sectors:0x%x\n",
                      g_iUsrSecCnt);
    usrgen_OutputText(0xffff,(PCSTR) "usrgen_make_user_data:User configure Sectors:0x%x\n",
                      g_iCfgSecCnt);
    DRV_FlashPowerDown();
    return 0;
}

int main (int argc,char *argv[])
{

    int arg;
    char fnambin[32]="user.bin";
    char fnamlod[32]="user.lod";
    char *pfnambin = fnambin;
    char *pfnamlod = fnamlod;
    FILE *bin_fp;
    FILE *lod_fp;
    UINT32 error_no = 0;

    usrgen_OutputText(0xff,  (PCSTR)"%s",(char*)usrgen_version_string);

    for (arg = 1; arg < argc; arg++)
    {
        if (argv[arg][0]=='-')
        {
            if (argv[arg][1]=='o')
            {
                pfnambin = argv[arg+1];
                arg++;
            }
            if (argv[arg][1]=='v')
            {
                verbose++;
            }
        }
    }

    if(pfnambin == NULL) {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen:out file not defined!\n");
        exit(1);
    }

    if ((bin_fp = fopen (pfnambin, "wb+")) == NULL) {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen:Can't open bin file %s\n", pfnambin);
        exit(1);
    }
     if ((lod_fp = fopen (pfnamlod, "wb+")) == NULL) {
        usrgen_OutputText(0xffff, (PCSTR)"usrgen:Can't open lod file %s\n", pfnamlod);
        exit(1);
    }

    if((error_no = usrgen_make_user_data(bin_fp,lod_fp)) != 0) {
        usrgen_OutputText(0xffff, (PCSTR)"error code %d\n", error_no);
    }
    fclose(bin_fp);
    fclose(lod_fp);
    return error_no;
}
#endif
