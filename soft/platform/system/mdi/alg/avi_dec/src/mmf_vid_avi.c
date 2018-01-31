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


#if 0
#include "kal_release.h"
#include "kal_trace.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* task message communiction */
/* Buffer Management */
#include "app_buff_alloc.h"
#include "syscomp_config.h"
#include "task_config.h"        /* task creation */
#include "stacklib.h"           /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* event scheduler */
#include "stack_timer.h"        /* stack timer */
/* for tst inject message */
//#include "tst_sap.h" /* wufasong deleted 2007.06.16 */


/* global includes */
#include "l1audio.h"
#include "device.h"
#include "resource_audio.h"
#include "nvram_enums.h"
#include "nvram_struct.h"
#include "nvram_user_defs.h"
#include "nvram_data_items.h"
#include "custom_nvram_editor_data_item.h"

/* local includes */
//#include "med_main.h"
#include "med_global.h"
#include "aud_defs.h"
#include "med_struct.h"
#include "med_api.h"
#include "med_context.h"
#include "fat_fs.h"
#include "med_smalloc.h"
#include "med_utility.h"
#endif

//#include "cs_types.h"
#include "mmf_vid_avi.h"
#ifndef SHEEN_VC_DEBUG
#include "mmc.h"
#include "mcip_debug.h"
#endif

#if 0//__NO_LIMIT_AVI__   
#include "vid_main.h"
#endif

#if 0// __VID_TRACE_ON__
#include "med_trc.h"
#endif

#define _MJPG_

#ifdef _MJPG_

//#define MJPEGTRACE    0 // MJPEG trace power
/*========  CONSTANTS  =========*/
#define CACHE_TO_MEM
#ifdef FILE_CACHE_SIZE
#ifdef __MED_MJPG_AUDIO__
#define CHUNK_BUFFER_SIZE (FILE_CACHE_SIZE+MJPG_AUDIO_CHUNK_SIZE)
#define IDX_BUFFER_SIZE (4096)
#else
#define CHUNK_BUFFER_SIZE FILE_CACHE_SIZE
#endif
#else
#define CHUNK_BUFFER_SIZE (300*1024)
#endif

#define MAX_JUNK_COUNT 20
#define MAX_PARSING_TIME 1000  /* 1000*4.615ms~=4.6 sec */
/* HEADERBYTES: The number of bytes to reserve for the header */


//#include "mcip_debug.h"//shenh2007.9.12
//#include "oslmemory.h"//shenh2007.9.12
#define HEADERBYTES 512

#define AVI_MAX_LEN ((1<<26)*16-HEADERBYTES)

#define PAD_EVEN(x) ( ((x)+1) & ~1 )

#define ERR_EXIT(x) \
{ \
   if(hdrl_data!=0){ \
    /*  free(hdrl_data);*/ \
    /*  hdrl_data=0; */\
    } \
   avi_close(avi); \
   avi_errno = x; \
   diag_printf("[ALG_AVI]err avi_errno= %d, Line: %d\n", avi_errno, __LINE__);\
   return 0; \
}

/*==========  MACROS  ============*/
#define OUT4CC(s) \
   if(nhb<=HEADERBYTES-4) memcpy(avi_header+nhb,s,4); nhb += 4

#define OUTLONG(n) \
   if(nhb<=HEADERBYTES-4) ulong2str(avi_header+nhb,n); nhb += 4

#define OUTSHRT(n) \
   if(nhb<=HEADERBYTES-2) { \
      avi_header[nhb  ] = (n   )&0xff; \
      avi_header[nhb+1] = (n>>8)&0xff; \
   } \
   nhb += 2


/* The Flags in AVI File header */

#define AVIF_HASINDEX           0x00000010      /* Index at end of file */
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800      /* Use CKType to find key frames */
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000


/*========  GLOBAL VARIABLES  =========*/
int32 avi_errno = 0;

const int8 id_str[]="CSD Proprietary Format\n";

#ifdef __NO_LIMIT_AVI__
#define BUFFER_NO 2
#else
#define BUFFER_NO 1
#endif
//static avi_chunk_buffer_struct avi_chunk_buffer[BUFFER_NO];
//static avi_chunk_buffer_struct *active_chunk_buffer_p;
//static uint8 active_chunk_buffer_id;
#ifdef __NO_LIMIT_AVI__
static avi_chunk_buffer_struct avi_idx_buffer[BUFFER_NO];
static avi_chunk_buffer_struct *active_idx_buffer_p;
static uint8 active_idx_buffer_id;
/* seek hint for fast seek */
#define VID_AVI_SEEK_HINT_SIZE 2047
FS_FileLocationHint           vid_avi_seek_hint[VID_AVI_SEEK_HINT_SIZE];
#endif
static uint16 avi_cache_in_mem=0;
//int32 offset_pos_temp=0;
//uint8 data[512];



/************************
for dynamic index read shenh 2008.5.13
*************************/

#define INDEX_NUM 512 // video =INDEX_NUM*10 bytes audio= INDEX_NUM*14 bytes. max 65535 short

#define V_TYPE 0 //video index
#define A_TYPE 1 //audio index

#define VAL_POS 0 //index value
#define VAL_LEN 1
#define VAL_TOT 2
#define VAL_KEY 3//get key index

static unsigned int v_countInBuf=0;
static unsigned int a_countInBuf=0;

static unsigned int v_lastIndexNum=0;
static unsigned int a_lastIndexNum=0;

static unsigned int v_lastPosInFile=0;
static unsigned int a_lastPosInFile=0;

static unsigned int v_lastLen=0;
static unsigned int a_lastLen=0;

static unsigned int a_lastTot=0;

static unsigned int v_scale=0;
static unsigned int a_scale=0;

/**************************/



/*======    Utilities for writing an AVI File    ======*/

#if 0
/*************************************************************************
* FUNCTION
*   avi_alloc_chunk_buffer
*
* DESCRIPTION
*   This function is to allocate memory for chunk buffer.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static uint32 avi_alloc_chunk_buffer(uint32 len)
{
    uint8 i;
    uint32 buf_len=len/BUFFER_NO;
    for(i=0; i<BUFFER_NO; i++)
    {
        active_chunk_buffer_p=&(avi_chunk_buffer[i]);
        active_chunk_buffer_p->mem_start=(uint8*)malloc(buf_len);//shenh2007.9.15
        //ASSERT(active_chunk_buffer_p->mem_start!=NULL);
        active_chunk_buffer_p->len=buf_len;
        active_chunk_buffer_p->top_p=active_chunk_buffer_p->cur_p=active_chunk_buffer_p->mem_start;
        active_chunk_buffer_p->mem_end=active_chunk_buffer_p->mem_start+buf_len;
    }
    active_chunk_buffer_id=0;
    active_chunk_buffer_p=&(avi_chunk_buffer[0]);
    return len;
}

/*************************************************************************
* FUNCTION
*   avi_free_chunk_buffer
*
* DESCRIPTION
*   This function is to free the chunk buffer.
*
* PARAMETERS
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void avi_free_chunk_buffer(void)
{
    uint8 i;
    for(i=0; i<BUFFER_NO; i++)
    {
        active_chunk_buffer_p=&(avi_chunk_buffer[i]);
        free((void**)&(active_chunk_buffer_p->mem_start));
        active_chunk_buffer_p->len=0;
        active_chunk_buffer_p->cur_p=NULL;
    }
}

/*************************************************************************
* FUNCTION
*   avi_dump_chunk_buffer_to_file
*
* DESCRIPTION
*   This function is to dump data from chunk buffer to the file.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_dump_chunk_buffer_to_file(int32 fd)
{
    uint32 wlen;
    int32 result;
    avi_chunk_buffer_struct *buffer_p;
#ifdef __NO_LIMIT_AVI__
    uint8 no_active_chunk_buffer_id=!active_chunk_buffer_id;
#else
    uint8 no_active_chunk_buffer_id=active_chunk_buffer_id;
#endif
    if(fd<0)
        return -1;
    buffer_p=&(avi_chunk_buffer[no_active_chunk_buffer_id]);
    if((wlen=FS_Write(fd, buffer_p->mem_start, buffer_p->top_p-buffer_p->mem_start))!=buffer_p->top_p-buffer_p->mem_start)
        return -1;
    //VID_VALUE_TRACE(no_active_chunk_buffer_id, wlen, __LINE__);
    buffer_p->top_p=buffer_p->cur_p=buffer_p->mem_start;
    return (int32)wlen;
}
#endif

#ifdef __NO_LIMIT_AVI__

/*************************************************************************
* FUNCTION
*   avi_alloc_idx_buffer
*
* DESCRIPTION
*   This function is to allocate memory for idx buffer.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static uint32 avi_alloc_idx_buffer(uint32 len)
{
    uint8 i;
    uint32 buf_len=len/BUFFER_NO;
    for(i=0; i<BUFFER_NO; i++)
    {
        active_idx_buffer_p=&(avi_idx_buffer[i]);
        active_idx_buffer_p->mem_start=(uint8*)malloc(buf_len);
        //ASSERT(active_idx_buffer_p->mem_start!=NULL);
        active_idx_buffer_p->len=buf_len;
        active_idx_buffer_p->top_p=active_idx_buffer_p->cur_p=active_idx_buffer_p->mem_start;
        active_idx_buffer_p->mem_end=active_idx_buffer_p->mem_start+buf_len;
    }
    active_idx_buffer_id=0;
    active_idx_buffer_p=&(avi_idx_buffer[0]);
    return len;
}

/*************************************************************************
* FUNCTION
*   avi_free_idx_buffer
*
* DESCRIPTION
*   This function is to free the idx buffer.
*
* PARAMETERS
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void avi_free_idx_buffer(void)
{
    uint8 i;
    for(i=0; i<BUFFER_NO; i++)
    {
        active_idx_buffer_p=&(avi_idx_buffer[i]);
        free((void**)&(active_idx_buffer_p->mem_start));
        active_idx_buffer_p->len=0;
        active_idx_buffer_p->cur_p=NULL;
    }
}

/*************************************************************************
* FUNCTION
*   avi_dump_idx_buffer_to_file
*
* DESCRIPTION
*   This function is to dump data from idx buffer to the file.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_dump_idx_buffer_to_file(int32 fd)
{
    uint32 wlen;
    int32 result;
    avi_chunk_buffer_struct *buffer_p;
    uint8 no_active_idx_buffer_id=!active_idx_buffer_id;
    if(fd<0)
        return -1;
    buffer_p=&(avi_idx_buffer[no_active_idx_buffer_id]);
    if((wlen=FS_Write(fd, buffer_p->mem_start, buffer_p->top_p-buffer_p->mem_start))!=uffer_p->top_p-buffer_p->mem_start)
        return -1;
    VID_VALUE_TRACE(no_active_idx_buffer_id, wlen, __LINE__);
    buffer_p->top_p=buffer_p->cur_p=buffer_p->mem_start;
    return (int32)wlen;
}


#endif


#if 0
/*************************************************************************
* FUNCTION
*   avi_read_file_to_buffer
*
* DESCRIPTION
*   This function is to read data from the file to the chunk buffer.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_read_file_to_buffer(int32 fd, uint32 size)
{
    uint32 rlen;
    if(fd<0||size==0)
        return -1;
    rlen = FS_Read(fd, active_chunk_buffer_p->mem_start, size);
    if (rlen > 0)
    {
        active_chunk_buffer_p->top_p=active_chunk_buffer_p->mem_start+rlen;
        active_chunk_buffer_p->mem_end=active_chunk_buffer_p->mem_start+rlen;
        active_chunk_buffer_p->cur_p=active_chunk_buffer_p->mem_start;
    }
    else
        rlen = 0;
    return (int32)rlen;
}

/*************************************************************************
* FUNCTION
*   avi_set_array_to_buffer
*
* DESCRIPTION
*   This function is to set data pointer to the chunk buffer.
*
* PARAMETERS
*  uint32 len
*
* RETURNS
*   The allocated length.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_set_array_to_buffer(uint8 *data, uint32 len)
{
    if(data==0||len==0)
        return -1;
    active_chunk_buffer_p=&(avi_chunk_buffer[active_chunk_buffer_id]);
    active_chunk_buffer_p->mem_start=data;
    active_chunk_buffer_p->top_p=active_chunk_buffer_p->mem_start+len;
    active_chunk_buffer_p->mem_end=active_chunk_buffer_p->mem_start+len;
    active_chunk_buffer_p->cur_p=active_chunk_buffer_p->mem_start;
    active_chunk_buffer_p->len=len;
    return (int32)len;
}
#endif

/*************************************************************************
* FUNCTION
*   avi_read
*
* DESCRIPTION
*   This function is to read data with the given length from the given file handle.
*
* PARAMETERS
*   int32 fd
*  int8 *buf
*  uint32 len
* RETURNS
*   The length read.
*
* GLOBALS AFFECTED
*
*************************1************************************************/
#if 0//sheen
static uint32 avi_read(int32 fd, uint8 *buf, int32 len)
{
    //ASSERT(len!=0)
    if(avi_cache_in_mem)
    {
        if(active_chunk_buffer_p->cur_p+len<=active_chunk_buffer_p->mem_end)
        {
            memcpy(buf, active_chunk_buffer_p->cur_p, len);
            active_chunk_buffer_p->cur_p+=len;
            return len;
        }
        else if(active_chunk_buffer_p->cur_p<active_chunk_buffer_p->mem_end)
        {
            uint32 rlen;
            rlen=active_chunk_buffer_p->mem_end-active_chunk_buffer_p->cur_p;
            memcpy(buf, active_chunk_buffer_p->cur_p, rlen);
            active_chunk_buffer_p->cur_p+=rlen;
            return rlen;
        }
        else
            return 0;
    }
    else
    {
        // uint32 n = 0;
        uint32 r = 0;
        //int32 result;
#if 0
        while (r < len)
        {
            result=MMI_FS_Read(fd, buf + r, len - r, &n);
            TRACE_EF(g_sw_FMGR, "^^^^^^^^^^^^^ result = %d, n = %d, %d, %x", result, n , len - r, *((unsigned int *)(buf + r)) );

            r += n;
            if(result!=FS_NO_ERROR||n==0)
                break;
        }
#endif
#if 0
        while (r < len)
        {
            n=FS_Read(fd, buf + r, len - r);
            //TRACE_EF(g_sw_FMGR, "^^^^^^^^^^^^^ n = %d, %d, %x", n , len - r, *((unsigned int *)(buf + r)) );
            if (n > 0)
                r += n;
            else
                break;
        }
#endif
        //offset_pos_temp+=len;
        r=FS_Read(fd, buf, len);
        return r;
    }
}

/*************************************************************************
* FUNCTION
*   avi_seek
*
* DESCRIPTION
*   This function is to seek to the given position from the given file handle.
*
* PARAMETERS
*   int32 fd
*  int32 offset
*  int32 whence
* RETURNS
*   The length of the seek.
*
* GLOBALS AFFECTED
*
*****************************1********************************************/
static int32 avi_seek(int32 fd, int32 offset, int32 whence)
{
    int32 res;
    if(avi_cache_in_mem)
    {
        switch(whence)
        {
            case FS_SEEK_SET:
                if(offset<0 || offset>active_chunk_buffer_p->len)
                    return FS_PARAM_ERROR;
                active_chunk_buffer_p->cur_p=active_chunk_buffer_p->mem_start+offset;
                break;
            case FS_SEEK_CUR:
                if(active_chunk_buffer_p->cur_p+offset>active_chunk_buffer_p->mem_end ||
                        active_chunk_buffer_p->cur_p+offset<active_chunk_buffer_p->mem_start)
                    return FS_PARAM_ERROR;
                active_chunk_buffer_p->cur_p=active_chunk_buffer_p->cur_p+offset;
                break;
            case FS_SEEK_END:
                if(offset>0)
                    return FS_PARAM_ERROR;
                active_chunk_buffer_p->cur_p=active_chunk_buffer_p->mem_end+offset;
                break;
        }
        if(active_chunk_buffer_p->cur_p>active_chunk_buffer_p->top_p)
            active_chunk_buffer_p->top_p=active_chunk_buffer_p->cur_p;
        return (int32)(active_chunk_buffer_p->cur_p-active_chunk_buffer_p->mem_start);
    }
    else
    {
        /*
        if(whence==FS_SEEK_CUR)
            {offset_pos_temp+=offset;}
        else if(whence==FS_SEEK_SET)
            {
                offset_pos_temp=offset;
            }
        */
        res=(int32)FS_Seek((int32)fd, (int32)offset, (uint8)whence);
        return res;
    }
}
#endif

INT32 avi_fs_read(avi_struct *avi, UINT8 *pBuf, UINT32 iLen)
{
    INT32 res=0;

    if(avi->avi_fs_mode==0)
    {
#ifdef SHEEN_VC_DEBUG
        res=fread(pBuf, 1, iLen,(INT32)avi->fdes);
#else
        res=FS_Read((INT32)avi->fdes, pBuf, iLen);
#endif
    }
    else
    {
        if(avi->pAvi_fs_point+iLen<=(UINT8*)avi->fdes + avi->avi_fs_bufLen)
        {
            memcpy(pBuf, avi->pAvi_fs_point, iLen);
            avi->pAvi_fs_point+=iLen;
            res=iLen;
        }
        else if(avi->pAvi_fs_point-(UINT8*)avi->fdes < avi->avi_fs_bufLen)
        {
            res=(UINT8*)avi->fdes+ avi->avi_fs_bufLen- avi->pAvi_fs_point;
            memcpy(pBuf, avi->pAvi_fs_point, res);
            avi->pAvi_fs_point+=res;
        }
    }

    return res;
}

INT32 avi_fs_seek(avi_struct *avi, INT32 iOffset, UINT8 iOrigin)
{
    INT32 res=-1;

    if(avi->avi_fs_mode==0)
    {
#ifdef SHEEN_VC_DEBUG
        fseek((INT32)avi->fdes, iOffset, iOrigin);
        res=ftell(avi->fdes);
#else
        res=(INT32)FS_Seek((INT32)avi->fdes, (INT64)iOffset, iOrigin);
#endif
    }
    else
    {
        switch ( iOrigin )
        {
            case FS_SEEK_SET:
                res = 0 + iOffset;
                avi->pAvi_fs_point = res < 0 ? (UINT8*)avi->fdes : (res > avi->avi_fs_bufLen ? ((UINT8*)avi->fdes + avi->avi_fs_bufLen): ((UINT8*)avi->fdes + res));
                res = avi->pAvi_fs_point-(UINT8*)avi->fdes;
                break;

            case FS_SEEK_CUR:
                res = (INT32)(avi->pAvi_fs_point- (UINT8*)avi->fdes) + iOffset;
                avi->pAvi_fs_point = res < 0 ? (UINT8*)avi->fdes : (res > avi->avi_fs_bufLen ? ((UINT8*)avi->fdes + avi->avi_fs_bufLen): ((UINT8*)avi->fdes + res));
                res = avi->pAvi_fs_point-(UINT8*)avi->fdes;
                break;

            case FS_SEEK_END:
                res = avi->avi_fs_bufLen + iOffset;
                avi->pAvi_fs_point = res < 0 ? (UINT8*)avi->fdes : (res > avi->avi_fs_bufLen ? ((UINT8*)avi->fdes + avi->avi_fs_bufLen): ((UINT8*)avi->fdes + res));
                res = avi->pAvi_fs_point-(UINT8*)avi->fdes;
                break;

            default:
                res = -1;
        }

    }

    return res;
}

INT32 avi_fs_getFileSize(avi_struct *avi)
{
    INT32 res=0;

    if(avi->avi_fs_mode==0)
    {
#ifdef SHEEN_VC_DEBUG
        fseek((INT32)avi->fdes,0,SEEK_END);
        res=ftell(avi->fdes);
#else
        res=(INT32)FS_GetFileSize((INT32)avi->fdes);
#endif
    }
    else
    {
        res=avi->avi_fs_bufLen;
    }
    return res;
}

INT32 avi_fs_isEndOfFile(avi_struct *avi)
{
    INT32 res=0;

    if(avi->avi_fs_mode==0)
    {
#ifdef SHEEN_VC_DEBUG
        res=ftell(avi->fdes);
        fseek((INT32)avi->fdes,0,SEEK_END);
        if(ftell(avi->fdes)==res)
            res=1;
        else
        {
            fseek((INT32)avi->fdes,res,SEEK_SET);
            res=0;
        }


#else
        res=FS_IsEndOfFile((INT32)avi->fdes);
#endif
    }
    else
    {
        if(avi->pAvi_fs_point >= (uint8*)avi->fdes + avi->avi_fs_bufLen)
            res=1;
        else
            res=0;
    }
    return res;
}

#if 0
/*************************************************************************
* FUNCTION
*   avi_write
*
* DESCRIPTION
*   This function is to write data with the given length to the given file handle.
*
* PARAMETERS
*   int32 fd
*  int8 *buf
*  uint32 len
* RETURNS
*   The length written.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static uint32 avi_write (int32 fd, uint8 *buf, uint32 len)
{
    if(avi_cache_in_mem)
    {
        if(active_chunk_buffer_p->cur_p+len<=active_chunk_buffer_p->mem_end)
        {
            memcpy(active_chunk_buffer_p->cur_p, buf, len);
            active_chunk_buffer_p->cur_p+=len;
            if(active_chunk_buffer_p->cur_p>active_chunk_buffer_p->top_p)
                active_chunk_buffer_p->top_p=active_chunk_buffer_p->cur_p;
            return len;
        }
        else
            return 0;
    }
    else
    {
        uint32 n = 0;
        int32 result;

        n=FS_Write (fd, buf, len);
        if(n  <= 0)
        {
            avi_errno = AVI_ERR_CLOSE;
            return -1;
        }
        return n;
    }
}

#endif

#ifdef __NO_LIMIT_AVI__
/*************************************************************************
* FUNCTION
*   avi_write_idx
*
* DESCRIPTION
*   This function is to write index with the given length to the given file handle.
*
* PARAMETERS
*   int32 fd
*  int8 *buf
*  uint32 len
* RETURNS
*   The length written.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static uint32 avi_write_idx (int32 fd, uint8 *buf, uint32 len)
{
    if(avi_cache_in_mem)
    {
        if(active_idx_buffer_p->cur_p+len<=active_idx_buffer_p->mem_end)
        {
            memcpy(active_idx_buffer_p->cur_p, buf, len);
            active_idx_buffer_p->cur_p+=len;
            if(active_idx_buffer_p->cur_p>active_idx_buffer_p->top_p)
                active_idx_buffer_p->top_p=active_idx_buffer_p->cur_p;
            return len;
        }
        else
            return 0;
    }
    else
    {
        uint32 n = 0;
        int32 result;

        n=FS_Write (fd, buf, len);
        if(n  <= 0)
        {
            avi_errno = AVI_ERR_CLOSE;
            return -1;
        }
        return n;
    }
}
#endif

/*************************************************************************
* FUNCTION
*   ulong2str
*
* DESCRIPTION
*   Copy n into dst as a 4 byte, little endian number.
*   Should also work on big endian machines
*
* PARAMETERS
*   uint8 *dst
*  uint32 n
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if 0
static void ulong2str(uint8 *dst, uint32 n)
{
    dst[0] = (n    )&0xff;
    dst[1] = (n>> 8)&0xff;
    dst[2] = (n>>16)&0xff;
    dst[3] = (n>>24)&0xff;
}
#endif

/*************************************************************************
* FUNCTION
*   str2ulong
*
* DESCRIPTION
*   Convert a string of 4 bytes to a number,
*   also working on big endian machines
*
* PARAMETERS
*   uint8 *dst
*  uint32 n
*
* RETURNS
*   uint32
*
* GLOBALS AFFECTED
*
*********************1****************************************************/
static uint32 str2ulong(uint8 *str)
{
    /*
    uint8 temp[4];
    temp[0]=str [3];
    temp[1]=str [2];
    temp[2]=str [1];
    temp[3]=str [0];
    */
    return ( str[0] | (str[1]<<8) | (str[2]<<16) | (str[3]<<24) );
    //return *((uint32*)temp);
}

/*************************************************************************
* FUNCTION
*   str2ushort
*
* DESCRIPTION
*   Convert a string of 2 bytes to a number,
*   also working on big endian machines
*
* PARAMETERS
*   uint8 *dst
*  uint32 n
*
* RETURNS
*   uint32
*
* GLOBALS AFFECTED
*
*********************1****************************************************/
static uint32 str2ushort(uint8 *str)
{
    return ( str[0] | (str[1]<<8) );
}

/*************************************************************************
* FUNCTION
*   avi_sampsize
*
* DESCRIPTION
*   Calculate audio sample size from number of bits and number of channels.
*   This may have to be adjusted for eg. 12 bits and stereo
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_sampsize(avi_struct *avi)
{
    int32 s;
    s = ((avi->a_bits+7)/8)*avi->a_chans;
    if(s==0) s=1; /* avoid possible zero divisions */
    //if(s<4) s=4; /* avoid possible zero divisions */
    return s;
}

#if 0
/*************************************************************************
* FUNCTION
*   avi_add_chunk
*
* DESCRIPTION
*   To add a chunk (tag and data) to the AVI file
*
* PARAMETERS
*   avi_struct *avi
*  uint8 *tag
*  uint8 *data
*  uint32 length
* RETURNS
*   int32 , returns -1 on write error, 0 on success
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_add_chunk(avi_struct *avi, uint8 *tag, uint8 *data, uint32 length)
{
    uint8 c[8];

    memcpy(c,tag,4);
    ulong2str(c+4,length);

    /* Output tag, length and data, restore previous position
       if the write fails */

    length = PAD_EVEN(length);

#ifdef __NO_LIMIT_AVI__
    if( avi_write(avi->vi_fdes,c,8) != 8 ||
            avi_write(avi->vi_fdes,data,length) != length )
#else
    if( avi_write(avi->fdes,c,8) != 8 ||
            avi_write(avi->fdes,data,length) != length )
#endif
    {
        avi_errno = AVI_ERR_WRITE;
        return -1;
    }

    /* Update file position */

    avi->pos += 8 + length;

    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_add_index_entry
*
* DESCRIPTION
*   Add a index entry (=tag and data) to the AVI file
*
* PARAMETERS
*   avi_struct *avi
*  uint8 *tag
*  uint32 flags
*  uint32 pos
*  uint32 len
* RETURNS
*   int32, returns -1 on write error, 0 on success
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_add_index_entry(avi_struct *avi, uint8 *tag, uint32 flags, uint32 pos, uint32 len)
{
    void *ptr;

    if(avi->n_idx>=avi->max_idx)
    {
        /* Realloc */
        ptr=malloc((avi->max_idx+4096)*16);
        if (ptr && avi->idx)
        {
            memcpy(ptr,avi->idx, (avi->max_idx) * 16);
            free(avi->idx);
        }
        if(ptr == 0)
        {
            avi_errno = AVI_ERR_NO_MEM;
            return -1;
        }
        avi->max_idx += 4096;
        avi->idx = (uint8((*)[16]) ) ptr;
    }

    /* Add index entry */

    memcpy(avi->idx[avi->n_idx],tag,4);
    ulong2str(avi->idx[avi->n_idx]+ 4,flags);
    ulong2str(avi->idx[avi->n_idx]+ 8,pos);
    ulong2str(avi->idx[avi->n_idx]+12,len);

    /* Update counter */

    avi->n_idx++;

    if(len>avi->max_len) avi->max_len=len;

    return 0;
}
#endif

#ifdef __NO_LIMIT_AVI__
uint32 avi_extend_to_cluster_size(FS_HANDLE FileHandle, const WCHAR *FileName)
{
    uint32 drv,extendLength,fileLength;
    int clusterSize;
    int result;

    drv = (uint32)FileName[0];
    clusterSize = FS_GetClusterSize(drv);
    fileLength = (UINT32)FS_GetFileSize(FileHandle);

    extendLength =clusterSize - fileLength%clusterSize;
    kal_prompt_trace(MOD_MED, "clusterSize = %d , extendLength= %d, fileLength= %d", clusterSize, extendLength, fileLength);
    FS_Seek(FileHandle, (INT64)0, FS_SEEK_END);
    result = FS_Extend(FileHandle, extendLength);

    return extendLength;
}

int32 avi_write_idx_header(avi_struct *avi)
{
    uint32 idx_length=0;
    int32  FSErrorCode;
    uint8 size[12];
    uint32 wlen;

    //write "idx1 size" to idx file
    idx_length = (uint32)FS_GetFileSize(avi->idx_fdes);

    if (idx_length > 0)  //
    {
        FS_Seek(avi->idx_fdes, 0, FS_SEEK_SET);
        memcpy(size,"idx1",4);
        ulong2str(size+4,idx_length-8);
        wlen = FS_Write(avi->idx_fdes, (void *)size, 8);
    }

    avi->pos += idx_length;

    return idx_length-8;  //idx length

}

//Write 512 header to visual file
int32 avi_write_header(avi_struct *avi)
{
    uint32 wlen;

    int32 ret, njunk, sampsize, hasIndex, ms_per_frame, frate, flag;
    int32 hdrl_start, strl_start;
    int32 nhb;
    uint8 *avi_header =NULL;
    int32 movi_len;

    avi_set_audio(avi, 1, 8000, 16, WAVE_FORMAT_PCM, 128000);
    //add 512 header to temp buffer
    avi_header=(uint8*)malloc(HEADERBYTES);
    if (avi_header == NULL)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return -1;
    }

    /* Calculate length of movi list */
    movi_len = avi->pos - HEADERBYTES + 4;  // 4 is the length of "movi"
    kal_prompt_trace(MOD_MED, "avi->pos = %d , movi_len= %d, HEADERBYTES= %d", avi->pos, movi_len, HEADERBYTES);
    hasIndex=1;
    avi_write_idx_header(avi);

    /* Calculate Microseconds per frame */

    if(avi->fps < 0.001)
    {
        frate=0;
        ms_per_frame=0;
    }
    else
    {
        frate = (int32) (10000*avi->fps + 0.5);
        ms_per_frame=(int32) (1000000/avi->fps + 0.5);
    }

    /* Prepare the file header */

    nhb = 0;

    /* The RIFF header */

    OUT4CC ("RIFF");
    OUTLONG(avi->pos - 8);    /* # of bytes to follow */
    OUT4CC ("AVI ");

    /* Start the header list */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    hdrl_start = nhb;  /* Store start position */
    OUT4CC ("hdrl");

    /* The main AVI header */

    OUT4CC ("avih");
    OUTLONG(56);                 /* # of bytes to follow */
    OUTLONG(ms_per_frame);       /* Microseconds per frame */
    OUTLONG(0);
    OUTLONG(0);                  /* PaddingGranularity (whatever that might be) */
    /* Other sources call it 'reserved' */
    flag = AVIF_ISINTERLEAVED;
    if(hasIndex) flag |= AVIF_HASINDEX;
    if(hasIndex && avi->must_use_index) flag |= AVIF_MUSTUSEINDEX;
    OUTLONG(flag);               /* Flags */
    OUTLONG(avi->video_frames);  /* TotalFrames */
    OUTLONG(0);                  /* InitialFrames */
    if (avi->audio_bytes)
    { OUTLONG(2); }           /* Streams */
    else
    { OUTLONG(1); }           /* Streams */
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(avi->width);         /* Width */
    OUTLONG(avi->height);        /* Height */
    /* MS calls the following 'reserved': */
    OUTLONG(0);                  /* TimeScale:  Unit used to measure time */
    OUTLONG(0);                  /* DataRate:   Data rate of playback     */
    OUTLONG(0);                  /* StartTime:  Starting time of AVI data */
    OUTLONG(0);                  /* DataLength: Size of AVI data chunk    */


    /* Start the video stream list ---------------------------------- */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    strl_start = nhb;  /* Store start position */
    OUT4CC ("strl");

    /* The video stream header */

    OUT4CC ("strh");
    OUTLONG(64);                 /* # of bytes to follow */
    OUT4CC ("vids");             /* Type */
    OUT4CC (avi->compressor);    /* Handler */
    OUTLONG(0);                  /* Flags */
    OUTLONG(0);                  /* Reserved, MS says: wPriority, wLanguage */
    OUTLONG(0);                  /* InitialFrames */
    OUTLONG(10000);              /* Scale */
    OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
    OUTLONG(0);                  /* Start */
    OUTLONG(avi->video_frames);  /* Length */
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(-1);                 /* Quality */
    OUTLONG(0);                  /* SampleSize */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */

    /* The video stream format */

    OUT4CC ("strf");
    OUTLONG(40);                 /* # of bytes to follow */
    OUTLONG(40);                 /* Size */
    OUTLONG(avi->width);         /* Width */
    OUTLONG(avi->height);        /* Height */
    OUTSHRT(1); OUTSHRT(24);     /* Planes, Count */
    OUT4CC (avi->compressor);    /* Compression */
    OUTLONG(avi->width*avi->height*3);  /* SizeImage (in bytes?) */
    OUTLONG(0);                  /* XPelsPerMeter */
    OUTLONG(0);                  /* YPelsPerMeter */
    OUTLONG(0);                  /* ClrUsed: Number of colors used */
    OUTLONG(0);                  /* ClrImportant: Number of colors important */

    /* Finish stream list, i.e. put number of bytes in the list to proper pos */

    ulong2str(avi_header+strl_start-4,nhb-strl_start);

    if (avi->a_chans && avi->audio_bytes)
    {

        sampsize = avi_sampsize(avi);

        /* Start the audio stream list ---------------------------------- */

        OUT4CC ("LIST");
        OUTLONG(0);        /* Length of list in bytes, don't know yet */
        strl_start = nhb;  /* Store start position */
        OUT4CC ("strl");

        /* The audio stream header */

        OUT4CC ("strh");
        OUTLONG(64);            /* # of bytes to follow */
        OUT4CC ("auds");
        OUTLONG(0);             /* Format (Optionally) */

        OUTLONG(0);             /* Flags */
        OUTLONG(0);             /* Reserved, MS says: wPriority, wLanguage */
        OUTLONG(0);             /* InitialFrames */

        OUTLONG(sampsize/2);      /* Scale */
        OUTLONG(avi->mp3rate/16);
        OUTLONG(0);             /* Start */
        OUTLONG(avi->audio_bytes/sampsize);   /* Length */
        OUTLONG(0);             /* SuggestedBufferSize */
        OUTLONG(-1);            /* Quality */

        OUTLONG(sampsize);    /* SampleSize */

        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */

        /* The audio stream format */

        OUT4CC ("strf");
        OUTLONG(16);                   /* # of bytes to follow */
        OUTSHRT(avi->a_fmt);           /* Format */
        OUTSHRT(avi->a_chans);         /* Number of channels */
        OUTLONG(avi->a_rate);          /* SamplesPerSec */
        OUTLONG(avi->a_rate*sampsize);

        OUTSHRT(sampsize);           /* BlockAlign */


        OUTSHRT(avi->a_bits);          /* BitsPerSample */

        /* Finish stream list, i.e. put number of bytes in the list to proper pos */

        ulong2str(avi_header+strl_start-4,nhb-strl_start);

    }

    /* Finish header list */

    ulong2str(avi_header+hdrl_start-4,nhb-hdrl_start);

    /* Calculate the needed amount of junk bytes, output junk */

    njunk = HEADERBYTES - nhb - 8 - 12;

    if(njunk<=0)
    {
        //ASSERT(njunk>0);
    }

    OUT4CC ("JUNK");
    OUTLONG(njunk);
    memset(avi_header+nhb,0,njunk);

    kal_prompt_trace(MOD_MED,"%d", njunk);

    if(njunk > sizeof(id_str))
    {
        memcpy(avi_header+nhb, id_str, sizeof(id_str));
    }

    nhb += njunk;

    /* Start the movi list */

    OUT4CC ("LIST");
    OUTLONG(movi_len); /* Length of list in bytes */
    OUT4CC ("movi");

    //add 512 header to visual file
    FS_Seek(avi->vi_fdes, 0, FS_SEEK_SET);
    wlen = FS_Write(avi->vi_fdes, (void *)avi_header, 512);
    if(wlen != 512)
    {
        free((void**)&avi_header);
        avi_errno = AVI_ERR_CLOSE;
        return -1;
    }
    if(avi_header!=NULL)
    {
        free((void**)&avi_header);
    }
    return 0;
}


/*************************************************************************
* FUNCTION
*   avi_add_index_for_no_limit_entry
*
* DESCRIPTION
*   Add a index entry (=tag and data) to the AVI file,
*   returns -1 on write error, 0 on success
*
* PARAMETERS
*   avi_struct *avi
*  uint8 *tag
*  uint32 flags
*  uint32 pos
*  uint32 len
* RETURNS
*   int32
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_add_index_for_no_limit_entry(avi_struct *avi, uint8 *tag, uint32 flags, uint32 pos, uint32 len)
{
    uint8 idx[16];
    /* Add index entry */
    if(avi->idx_fdes<0)
        return -1;
    memcpy(&idx[0],tag,4);    //audio:01wb, visual:00db
    ulong2str(&idx[4],flags); //audio:0,      visual:1
    ulong2str(&idx[8],pos);   //offset-1
    ulong2str(&idx[12],len);   //size

    if(avi_write_idx(avi->idx_fdes, (void *)idx, 16) != 16)
    {
        avi_errno = AVI_ERR_WRITE;
        return -1;
    }
    avi->n_idx++;

    if(len>avi->max_len) avi->max_len=len;

    return 0;
}


#endif

#if 0
/*************************************************************************
* FUNCTION
*   avi_set_video
*
* DESCRIPTION
*   To set the video parameters.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 width
*  uint32 height
*  double fps
*  int8 *compressor
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void avi_set_video(avi_struct *avi, uint32 width, uint32 height, uint32 fps, int8 *compressor)
{
    /* only for writing */

    if(avi->mode==AVI_MODE_READ) return;

    avi->width  = width;
    avi->height = height;
    avi->fps    = fps;

    if(strncmp(compressor, "RGB", 3)==0)
    {
        memset(avi->compressor, 0, 4);
    }
    else
    {
        memcpy(avi->compressor,compressor,4);
    }

    avi->compressor[4] = 0;

}

/*************************************************************************
* FUNCTION
*   avi_set_audio
*
* DESCRIPTION
*   To set the audio parameters.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 channels
*  uint32 rate
*  uint32 bits
*  uint32 format
*  uint32 mp3rate
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void avi_set_audio(avi_struct *avi, uint32 channels, uint32 rate, uint32 bits, uint32 format, uint32 mp3rate)
{
    /* only for writing */

    if(avi->mode==AVI_MODE_READ) return;

    avi->a_chans = channels;
    avi->a_rate  = rate;
    avi->a_bits  = bits;
    avi->a_fmt   = format;
    avi->mp3rate = mp3rate;

}


/*************************************************************************
* FUNCTION
*   avi_flush_output_file
*
* DESCRIPTION
*   Write the header of an AVI file and close it.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32: returns 0 on success, -1 on write error.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_flush_output_file(avi_struct *avi)
{

    int32 ret, njunk, sampsize, hasIndex, ms_per_frame, frate, idxerror, flag;
    int32 movi_len, hdrl_start, strl_start;
    uint8 *avi_header;
    int32 nhb;

    avi_header=(uint8*)malloc(HEADERBYTES);
    if (avi_header == NULL)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return -1;
    }
    /* Calculate length of movi list */

    movi_len = avi->pos - HEADERBYTES + 4;

    idxerror = 0;
    ret = avi_add_chunk(avi,(uint8*)"idx1",(uint8*)avi->idx,avi->n_idx*16);
    hasIndex = (ret==0);
    if(ret)
    {
        idxerror = 1;
        avi_errno = AVI_ERR_WRITE_INDEX;
    }

    /* Calculate Microseconds per frame */

    if(avi->fps < 0.001)
    {
        frate=0;
        ms_per_frame=0;
    }
    else
    {
        frate = (int32) (10000*avi->fps + 0.5);
        ms_per_frame=(int32) (1000000/avi->fps + 0.5);
    }

    /* Prepare the file header */

    nhb = 0;

    /* The RIFF header */

    OUT4CC ("RIFF");
    OUTLONG(avi->pos - 8);    /* # of bytes to follow */
    OUT4CC ("AVI ");

    /* Start the header list */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    hdrl_start = nhb;  /* Store start position */
    OUT4CC ("hdrl");

    /* The main AVI header */

    OUT4CC ("avih");
    OUTLONG(56);                 /* # of bytes to follow */
    OUTLONG(ms_per_frame);       /* Microseconds per frame */
    OUTLONG(0);
    OUTLONG(0);                  /* PaddingGranularity (whatever that might be) */
    /* Other sources call it 'reserved' */
    flag = AVIF_ISINTERLEAVED;
    if(hasIndex) flag |= AVIF_HASINDEX;
    if(hasIndex && avi->must_use_index) flag |= AVIF_MUSTUSEINDEX;
    OUTLONG(flag);               /* Flags */
    OUTLONG(avi->video_frames);  /* TotalFrames */
    OUTLONG(0);                  /* InitialFrames */
    if (avi->audio_bytes)
    { OUTLONG(2); }           /* Streams */
    else
    { OUTLONG(1); }           /* Streams */
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(avi->width);         /* Width */
    OUTLONG(avi->height);        /* Height */
    /* MS calls the following 'reserved': */
    OUTLONG(0);                  /* TimeScale:  Unit used to measure time */
    OUTLONG(0);                  /* DataRate:   Data rate of playback     */
    OUTLONG(0);                  /* StartTime:  Starting time of AVI data */
    OUTLONG(0);                  /* DataLength: Size of AVI data chunk    */


    /* Start the video stream list ---------------------------------- */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    strl_start = nhb;  /* Store start position */
    OUT4CC ("strl");

    /* The video stream header */

    OUT4CC ("strh");
    OUTLONG(64);                 /* # of bytes to follow */
    OUT4CC ("vids");             /* Type */
    OUT4CC (avi->compressor);    /* Handler */
    OUTLONG(0);                  /* Flags */
    OUTLONG(0);                  /* Reserved, MS says: wPriority, wLanguage */
    OUTLONG(0);                  /* InitialFrames */
    OUTLONG(10000);              /* Scale */
    OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
    OUTLONG(0);                  /* Start */
    OUTLONG(avi->video_frames);  /* Length */
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(-1);                 /* Quality */
    OUTLONG(0);                  /* SampleSize */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */

    /* The video stream format */

    OUT4CC ("strf");
    OUTLONG(40);                 /* # of bytes to follow */
    OUTLONG(40);                 /* Size */
    OUTLONG(avi->width);         /* Width */
    OUTLONG(avi->height);        /* Height */
    OUTSHRT(1); OUTSHRT(24);     /* Planes, Count */
    OUT4CC (avi->compressor);    /* Compression */
    OUTLONG(avi->width*avi->height*3);  /* SizeImage (in bytes?) */
    OUTLONG(0);                  /* XPelsPerMeter */
    OUTLONG(0);                  /* YPelsPerMeter */
    OUTLONG(0);                  /* ClrUsed: Number of colors used */
    OUTLONG(0);                  /* ClrImportant: Number of colors important */

    /* Finish stream list, i.e. put number of bytes in the list to proper pos */

    ulong2str(avi_header+strl_start-4,nhb-strl_start);

    if (avi->a_chans && avi->audio_bytes)
    {

        sampsize = avi_sampsize(avi);

        /* Start the audio stream list ---------------------------------- */

        OUT4CC ("LIST");
        OUTLONG(0);        /* Length of list in bytes, don't know yet */
        strl_start = nhb;  /* Store start position */
        OUT4CC ("strl");

        /* The audio stream header */

        OUT4CC ("strh");
        OUTLONG(64);            /* # of bytes to follow */
        OUT4CC ("auds");
        OUTLONG(0);             /* Format (Optionally) */

        OUTLONG(0);             /* Flags */
        OUTLONG(0);             /* Reserved, MS says: wPriority, wLanguage */
        OUTLONG(0);             /* InitialFrames */

        OUTLONG(sampsize/2);      /* Scale */
        OUTLONG(avi->mp3rate/16);
        OUTLONG(0);             /* Start */
        OUTLONG(avi->audio_bytes/sampsize);   /* Length */
        OUTLONG(0);             /* SuggestedBufferSize */
        OUTLONG(-1);            /* Quality */

        OUTLONG(sampsize);    /* SampleSize */

        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */

        /* The audio stream format */

        OUT4CC ("strf");
        OUTLONG(16);                   /* # of bytes to follow */
        OUTSHRT(avi->a_fmt);           /* Format */
        OUTSHRT(avi->a_chans);         /* Number of channels */
        OUTLONG(avi->a_rate);          /* SamplesPerSec */
        OUTLONG(avi->a_rate*sampsize);

        OUTSHRT(sampsize);           /* BlockAlign */


        OUTSHRT(avi->a_bits);          /* BitsPerSample */

        /* Finish stream list, i.e. put number of bytes in the list to proper pos */

        ulong2str(avi_header+strl_start-4,nhb-strl_start);

    }

    /* Finish header list */

    ulong2str(avi_header+hdrl_start-4,nhb-hdrl_start);

    /* Calculate the needed amount of junk bytes, output junk */

    njunk = HEADERBYTES - nhb - 8 - 12;

    if(njunk<=0)
    {
        //ASSERT(njunk>0);
    }

    OUT4CC ("JUNK");
    OUTLONG(njunk);
    memset(avi_header+nhb,0,njunk);

    //kal_prompt_trace(MOD_MED,"%d", njunk);

    if(njunk > sizeof(id_str))
    {
        memcpy(avi_header+nhb, id_str, sizeof(id_str));
    }

    nhb += njunk;

    /* Start the movi list */

    OUT4CC ("LIST");
    OUTLONG(movi_len); /* Length of list in bytes */
    OUT4CC ("movi");

    /* Output the header, truncate the file to the number of bytes
       actually written, report an error if someting goes wrong */

    if ( avi_seek(avi->fdes,0,FS_SEEK_SET)<0 ||
            avi_write(avi->fdes,avi_header,HEADERBYTES)!=HEADERBYTES)
    {
        free((void**)&avi_header);
        avi_errno = AVI_ERR_CLOSE;
        return -1;
    }
    free((void**)&avi_header);

    if(idxerror) return -1;

    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_write_data
*
* DESCRIPTION
*   Add video or audio data to the file.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *data
*  uint32 length
*  int32 audio
*  int32 keyframe
* RETURNS
*   int32:    Return values:
*                   0    No error;
*                  -1    Error, avi_errno is set appropriatly;
*
* GLOBALS AFFECTED
*
*************************************************************************/
static int32 avi_write_data(avi_struct *avi, int8 *data, uint32 length, int32 audio, int32 keyframe)
{
    int32 n;

    /* Check for maximum file length */

    if ( (avi->pos + 8 + length + 8 + (avi->n_idx+1)*16) > AVI_MAX_LEN )
    {
        avi_errno = AVI_ERR_SIZELIM;
        return -1;
    }

    /* Add index entry */

#ifdef  __NO_LIMIT_AVI__
    if(audio)
        n = avi_add_index_for_no_limit_entry(avi,(uint8*)"01wb",0x00,avi->pos,length);
    else
        n = avi_add_index_for_no_limit_entry(avi,(uint8*)"00db",((keyframe)?0x10:0x0),avi->pos,length);
#else
    if(audio)
        n = avi_add_index_entry(avi,(uint8*)"01wb",0x00,avi->pos,length);
    else
        n = avi_add_index_entry(avi,(uint8*)"00db",((keyframe)?0x10:0x0),avi->pos,length);
#endif

    if(n) return -1;

    /* Output tag and data */

    if(audio)
        n = avi_add_chunk(avi,(uint8*)"01wb",(uint8*)data,length);
    else
        n = avi_add_chunk(avi,(uint8*)"00db",(uint8*)data,length);

    if (n) return -1;

    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_write_frame
*
* DESCRIPTION
*   Add video or audio data to the file.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *data
*  uint32 bytes
*  int32 keyframe
* RETURNS
*   int32:    Return values:
*                   0    No error;
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_write_frame(avi_struct *avi, int8 *data, uint32 bytes, int32 keyframe)
{
    uint32 pos;

    if(avi->mode==AVI_MODE_READ) { avi_errno = AVI_ERR_NOT_PERM; return -1; }

    pos = avi->pos;
    if( avi_write_data(avi,data,bytes,0,keyframe) ) return -1;
    avi->last_pos = pos;
    avi->last_len = bytes;
    avi->video_frames++;
    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_dup_frame
*
* DESCRIPTION
*   Add video or audio data to the file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_dup_frame(avi_struct *avi)
{
    if(avi->mode==AVI_MODE_READ) { avi_errno = AVI_ERR_NOT_PERM; return -1; }

    if(avi->last_pos==0) return 0; /* No previous real frame */
    if(avi_add_index_entry(avi,(uint8*)"00db",0x10,avi->last_pos,avi->last_len)) return -1;
    avi->video_frames++;
    avi->must_use_index = 1;
    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_write_audio
*
* DESCRIPTION
*   Add video or audio data to the file.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *data
*  uint32 bytes
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_write_audio(avi_struct *avi, int8 *data, uint32 bytes)
{
    if(avi->mode==AVI_MODE_READ) { avi_errno = AVI_ERR_NOT_PERM; return -1; }

    if( avi_write_data(avi,data,bytes,1,0) ) return -1;
    avi->audio_bytes += bytes;
    return 0;
}


/*************************************************************************
* FUNCTION
*   avi_append_audio
*
* DESCRIPTION
*   Add video or audio data to the file.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *data
*  uint32 bytes
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_append_audio(avi_struct *avi, int8 *data, uint32 bytes)
{

    int32 i;
    uint32 length, pos;
    uint8 c[4];

    if(avi->mode==AVI_MODE_READ) { avi_errno = AVI_ERR_NOT_PERM; return -1; }

    // update last index entry:
    --avi->n_idx;
    length = str2ulong((uint8*)avi->idx[avi->n_idx]+12);
    pos    = str2ulong((uint8*)avi->idx[avi->n_idx]+8);


    //update;
    ulong2str(avi->idx[avi->n_idx]+12,length+bytes);

    ++avi->n_idx;

    avi->audio_bytes += bytes;

    /* update chunk header */
    avi_seek(avi->fdes, pos+4, FS_SEEK_SET);
    ulong2str(c, length+bytes);
    avi_write(avi->fdes, c, 4);

    avi_seek(avi->fdes, pos+8+length, FS_SEEK_SET);

    i=PAD_EVEN(length + bytes);

    bytes = i - length;
    avi_write(avi->fdes, (uint8*)data, bytes);
    avi->pos = pos + 8 + i;

    return 0;
}
#endif

/*************************************************************************
* FUNCTION
*   avi_bytes_remain
*
* DESCRIPTION
*   Check the remaining avi size.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    the remaining size
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_bytes_remain(avi_struct *avi)
{
    if(avi->mode==AVI_MODE_READ) return 0;

    return ( AVI_MAX_LEN - (avi->pos + 8 + 16*avi->n_idx));
}

#if 0
/*************************************************************************
* FUNCTION
*   avi_bytes_written
*
* DESCRIPTION
*   Check the written avi size.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    the written size
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_bytes_written(avi_struct *avi)
{
    if(avi->mode==AVI_MODE_READ) return 0;

    return (avi->pos + 8 + 16*avi->n_idx);
}
#endif

#ifdef __NO_LIMIT_AVI__
/*************************************************************************
* FUNCTION
*   avi_bytes_written_for_idx
*
* DESCRIPTION
*   Check the written avi idx size.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    the written idx size
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_bytes_written_for_idx(avi_struct *avi)
{
    if(avi->mode==AVI_MODE_READ) return 0;

    return (avi->n_idx);
}


int32 avi_residual_bytes_in_active_buffer(avi_struct *avi)
{
    if(avi_cache_in_mem)
    {
        return (int32)(active_chunk_buffer_p->mem_end-active_chunk_buffer_p->top_p);
    }
    return 0;
}


int32 avi_residual_idx_in_active_buffer(avi_struct *avi)
{
    if(avi_cache_in_mem)
    {
        return (int32)((active_idx_buffer_p->mem_end-active_idx_buffer_p->top_p)>>4);
    }
    return 0;
}

#endif

#if 0
//Record avi with PCM end
/*************************************************************************
* FUNCTION
*   avi_open_output_file
*
* DESCRIPTION
*   Open an AVI File and write a bunch
*    of zero bytes as space for the header
*
* PARAMETERS
*   const uint16 * filename
*
* RETURNS
*   int32 : returns a pointer to avi_struct on success, a zero pointer on error
*
* GLOBALS AFFECTED
*
*************************************************************************/

avi_struct* avi_open_output_file(const uint16 * filename)
{
#ifdef __NO_LIMIT_AVI__
    avi_struct *avi;

    /* Allocate the avi_struct struct and zero it */
    avi = (avi_struct *) malloc(sizeof(avi_struct));
    if(avi==0)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return 0;
    }
    memset((void *)avi,0,sizeof(avi_struct));

    memcpy(vid_context_p->current_file_name, filename, MAX_FILE_NAME_LEN*ENCODE_BYTE);
    //Visual
    memcpy(vid_context_p->storing_path, filename, MAX_FILE_NAME_LEN*ENCODE_BYTE);
    med_remove_file_name(vid_context_p->storing_path);
    kal_wstrcat(vid_context_p->storing_path, AVI_VISUAL_FILE);
    avi->vi_fdes = FS_Open(vid_context_p->storing_path, VID_TEMP_FILE_ATTRIBUTES);
    if (avi->vi_fdes < 0)
    {
        avi_errno = AVI_ERR_OPEN;
        free(avi);
        avi=NULL;
        return 0;
    }
    FS_Seek(avi->vi_fdes, (int)HEADERBYTES, FS_SEEK_SET);

    //Idx
    med_remove_file_name(vid_context_p->storing_path);
    kal_wstrcat(vid_context_p->storing_path, AVI_IDX_FILE);

    avi->idx_fdes = FS_Open(vid_context_p->storing_path, VID_TEMP_FILE_ATTRIBUTES);
    if (avi->idx_fdes < 0)
    {
        avi_errno = AVI_ERR_OPEN;
        free(avi);
        avi=NULL;
        return 0;
    }
    FS_Seek(avi->idx_fdes, (int)8, FS_SEEK_SET);

    avi->pos  = HEADERBYTES;
    avi->mode = AVI_MODE_WRITE; /* open for writing */

#ifdef CACHE_TO_MEM
    avi_alloc_chunk_buffer(CHUNK_BUFFER_SIZE);
    avi_alloc_idx_buffer(IDX_BUFFER_SIZE);
#endif

    return avi;

#else /* limited AVI */

    avi_struct *avi;
    int32 i;
    uint8 *avi_header;


    /* Allocate the avi_struct struct and zero it */

    avi = (avi_struct *) malloc(sizeof(avi_struct));
    if(avi==0)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return 0;
    }
    memset((void *)avi,0,sizeof(avi_struct));


    avi->fdes = MMI_FS_Open(filename, FM_CREAT|FS_O_RDWR);//2007.9.13
    if (avi->fdes < 0)
    {
        avi_errno = AVI_ERR_OPEN;
        free(avi);
        avi=NULL;
        return 0;
    }

#ifdef CACHE_TO_MEM
    avi_alloc_chunk_buffer(CHUNK_BUFFER_SIZE);
#endif
    /* Write out HEADERBYTES bytes, the header will go here
       when we are finished with writing */
    avi_header=(uint8*)malloc(HEADERBYTES);
    if (avi_header == NULL)
    {
        FS_Close(avi->fdes);
        avi_errno = AVI_ERR_NO_MEM;
        free(avi);
        avi=NULL;
        return 0;
    }
    for (i=0; i<HEADERBYTES; i++) avi_header[i] = 0;
    i = avi_write(avi->fdes,avi_header,HEADERBYTES);
    free((void**)&avi_header);
    if (i != HEADERBYTES)
    {
        FS_Close(avi->fdes);
        avi_errno = AVI_ERR_WRITE;
        free(avi);
        avi=NULL;
        return 0;
    }

    avi->pos  = HEADERBYTES;
    avi->mode = AVI_MODE_WRITE; /* open for writing */

    return avi;
#endif
}


/*************************************************************************
* FUNCTION
*   avi_close_output_file
*
* DESCRIPTION
*   Close avi file and free all memory.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_close_output_file(avi_struct *avi)
{
#ifdef __NO_LIMIT_AVI__
    int32 ret;

#ifdef CACHE_TO_MEM
    avi_toggle_chunk_buffer();
    if(avi_dump_chunk_buffer_to_file(avi->vi_fdes)<0)
        ret=-1;
    avi_free_chunk_buffer();

    avi_toggle_idx_buffer();
    if(avi_dump_idx_buffer_to_file(avi->idx_fdes)<0)
        ret=-1;
    avi_free_idx_buffer();
#endif

    med_remove_file_name(vid_context_p->storing_path);
    kal_wstrcat(vid_context_p->storing_path, AVI_VISUAL_FILE);

    kal_prompt_trace(MOD_MED, "avi->pos = %d ", avi->pos);
    avi->pos += avi_extend_to_cluster_size(avi->vi_fdes, vid_context_p->storing_path);

    ret = avi_write_header(avi);

    //Close all temp files
    FS_Close(avi->vi_fdes);   //close visual file
    FS_Close(avi->idx_fdes);    //close idx file

    free(avi);
    avi=NULL;
    return 0;

#else /* limited AVI */
    int32 ret;

    ret = avi_flush_output_file(avi);

#ifdef CACHE_TO_MEM
    if(avi_dump_chunk_buffer_to_file(avi->fdes)<0)
        ret=-1;
    avi_free_chunk_buffer();
#endif
    /* Even if there happened an error, we first clean up */

    MMI_FS_Close(avi->fdes);
    if(avi->idx) free(avi->idx);
    if(avi->video_index) free(avi->video_index);
    if(avi->audio_index) free(avi->audio_index);
    free(avi);
    avi=NULL;
    return ret;
#endif
}
#endif

/*=========  Utilities for reading video and audio from an AVI File  =========*/

/*************************************************************************
* FUNCTION
*   avi_close
*
* DESCRIPTION
*   Close avi file and free all memory.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
* GLOBALS AFFECTED
*
*******************1******************************************************/
int32 avi_close(avi_struct *avi)
{
    int32 ret;

    /* If the file was open for writing, the header and index still have
       to be written */
#if 0
    if(avi->mode == AVI_MODE_READ_ARRAY)
        ret = avi_close_input_array(avi);
    else if(avi->mode == AVI_MODE_WRITE)
        ret = avi_close_output_file(avi);
    else
#endif
        ret = avi_close_input_file(avi);
    return ret;
}

/*************************************************************************
* FUNCTION
*   avi_set_mem_cache
*
* DESCRIPTION
*   This function is to set avi memory cache.
*
* PARAMETERS
*  uint16 cache
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void avi_set_mem_cache(uint16 cache)
{
    avi_cache_in_mem=cache;
}

#ifdef __NO_LIMIT_AVI__

/*************************************************************************
* FUNCTION
*   avi_toggle_chunk_buffer
*
* DESCRIPTION
*   This function is to toggle chunk double buffer.
*
* PARAMETERS
*  None
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void avi_toggle_chunk_buffer(void)
{
    if(BUFFER_NO>1)
    {
        active_chunk_buffer_id=!active_chunk_buffer_id;
    }
    active_chunk_buffer_p=&(avi_chunk_buffer[active_chunk_buffer_id]);
}

/*************************************************************************
* FUNCTION
*   avi_toggle_idx_buffer
*
* DESCRIPTION
*   This function is to toggle idx double buffer.
*
* PARAMETERS
*  None
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void avi_toggle_idx_buffer(void)
{
    if(BUFFER_NO>1)
    {
        active_idx_buffer_id=!active_idx_buffer_id;
    }
    active_idx_buffer_p=&(avi_idx_buffer[active_idx_buffer_id]);
}

#endif


/*************************************************************************
* FUNCTION
*   avi_open_input_file
*
* DESCRIPTION
*   Open the AVI file with the given file name.
*
* PARAMETERS
*   uint16 *filename
*  int32 getIndex : Get the index info or not.
*
* RETURNS
*   avi_struct:    Return values:
*                   0   Error;
* GLOBALS AFFECTED
*
*****************************1********************************************/
#ifdef SHEEN_VC_DEBUG
avi_struct *avi_open_input_file(FILE* fhd, uint8 file_mode, uint32 data_Len, int32 getIndex)
#else
avi_struct *avi_open_input_file(HANDLE fhd, uint8 file_mode, uint32 data_Len, int32 getIndex)
#endif
{
    avi_struct *avi=NULL;
    /* Create avi_struct structure */

    avi = (avi_struct *)mmc_MemMalloc(sizeof(avi_struct));
    if(avi==NULL)
    {
        avi_errno = AVI_ERR_NO_MEM;
        diag_printf("[ALG_AVI]  avi_open_input_file malloc struct error and return\n");
        return 0;
    }
    memset((void *)avi,0,sizeof(avi_struct));

    avi->mode = AVI_MODE_READ; /* open for reading */

    /* Open the file */
#if 1
    avi->fdes = fhd;//FS_Open(filename, FS_O_RDONLY, 0);
    /*init 3 avi fs parameters. sheen*/
    avi->avi_fs_mode=file_mode;
    avi->avi_fs_bufLen=data_Len;
    avi->pAvi_fs_point=(uint8*)fhd;
    diag_printf( "[ALG_AVI] open file %x avi->fdes=%x \n", fhd,avi->fdes);
    /*
    int32 len;
    uint8 tempbuf[12];
    memset(tempbuf, 0, 12);
    len=FS_Read(avi->fdes, tempbuf,12);
    */

#else
    memset(aviname_ucs2, 0x00, sizeof(aviname_ucs2));
    AnsiiToUnicodeString(aviname_ucs2, aviname_ascii);
    avi->fdes = MMI_FS_Open(aviname_ucs2, FS_READ_ONLY);
#endif

    if(avi->fdes < 0)
    {
        avi_errno = AVI_ERR_OPEN;
        //mmc_MemFreeAll();
        avi=NULL;
        diag_printf( "[ALG_AVI] avi open err \n");
        return 0;
    }
#if 0//shenh2007.9.13
    if(avi_cache_in_mem==1)
    {
        uint32 size;
        MMI_FS_GetFileSize(avi->fdes, &size);
        printf("Excute func : shenh elisa_avi_open_input_file,file size =  %d",size);
        if(size==0||size>CHUNK_BUFFER_SIZE)
        {
            avi_errno = AVI_ERR_NO_MEM;
            OslMfree(avi);
            avi=NULL;
            return 0;
        }
        avi_alloc_chunk_buffer(CHUNK_BUFFER_SIZE);
        avi_read_file_to_buffer(avi->fdes, size);
    }
#endif

    if(avi_parse_input_file(avi, getIndex)==0)

    {
        if(avi_errno==AVI_ERR_NO_IDX)//special process.
        {
            diag_printf( "[ALG_AVI] WARNING!avi_parse_input_file AVI_ERR_NO_IDX \n");
        }
        else
        {
            diag_printf( "[ALG_AVI] avi_parse_input_file error %d \n", avi_errno);
            return 0;
        }
    }
    return avi;
}


/*************************************************************************
* FUNCTION
*   avi_close_input_file
*
* DESCRIPTION
*   Close avi file and free all memory.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
* GLOBALS AFFECTED
*
**************************1***********************************************/
int32 avi_close_input_file(avi_struct *avi)
{

    /* Even if there happened an error, we first clean up */
#if 0
    if(avi_cache_in_mem==1)
    {
        avi_free_chunk_buffer();
    }

#ifdef __NO_LIMIT_AVI__
    MMI_FS_Close(avi->au_fdes);
#endif
#endif
    if(avi)
    {
        //avi->fdes==0
        //if(avi->fdes)
        //{
        //diag_printf("**********CLOSE FILE IN VIDEO AVI********avi->fdes:%d",avi->fdes);
        //FS_Close(avi->fdes);
        //}
        //if(avi->idx) free(avi->idx);
        //if(avi->video_index) free(avi->video_index);
        //if(avi->audio_index) free(avi->audio_index);
        //free(avi);
        //mmc_MemFreeAll();
        //avi=NULL;
    }
    return 0;
}

#if 0
/*************************************************************************
* FUNCTION
*   avi_open_fd
*
* DESCRIPTION
*   Open the AVI file with the given file handle.
*
* PARAMETERS
*   int32 fd : The given file handle.
*  int32 getIndex : Get the index info or not.
*
* RETURNS
*   avi_struct:    Return values:
*                   0   Error;
* GLOBALS AFFECTED
*
*************************************************************************/
avi_struct *avi_open_fd(int32 fd, int32 getIndex)
{
    avi_struct *avi=NULL;

    /* Create avi_struct structure */

    avi = (avi_struct *) malloc(sizeof(avi_struct));
    if(avi==NULL)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return 0;
    }
    memset((void *)avi,0,sizeof(avi_struct));

    avi->mode = AVI_MODE_READ; /* open for reading */

    avi->fdes = fd;

    avi_parse_input_file(avi, getIndex);

    return avi;
}

/*************************************************************************
* FUNCTION
*   avi_open_input_array
*
* DESCRIPTION
*   Open the AVI file with the given file name.
*
* PARAMETERS
*   void *data
*  uint32 len
*
* RETURNS
*   avi_struct:    Return values:
*                   0   Error;
* GLOBALS AFFECTED
*
*************************************************************************/

avi_struct *avi_open_input_array(uint8 *data, uint32 len, int32 getIndex)
{
    avi_struct *avi=NULL;

    /* Create avi_struct structure */

    avi = (avi_struct *) malloc(sizeof(avi_struct));
    if(avi==NULL)
    {
        avi_errno = AVI_ERR_NO_MEM;
        return 0;
    }
    memset((void *)avi,0,sizeof(avi_struct));

    avi->mode = AVI_MODE_READ_ARRAY; /* open for reading */

    /* Set the array data to chunk buffer */
    avi_set_array_to_buffer(data, len);

    if(avi_parse_input_file(avi, getIndex)==0)
    {
        return 0;
    }
    return avi;
}
/*************************************************************************
* FUNCTION
*   avi_close_input_array
*
* DESCRIPTION
*   Close avi array and free all memory.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int32:    Return values:
*                   0    No error;
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_close_input_array(avi_struct *avi)
{

    if(avi->idx) free(avi->idx);
    if(avi->video_index) free(avi->video_index);
    if(avi->audio_index) free(avi->audio_index);
    free(avi);
    avi=NULL;
    return 0;
}
#endif

/*
shenh add for part index read 2008.5.13
*/
int32 avi_part_index_read(avi_struct *avi, uint32 indexNum, uint16 av_type, uint16 val_type)
{
    int32 res=0;
    int32 chunkPass=0;
    uint32 beginNumInBuf=0;
    int32 readByte=0;
    uint8 temp[8];

    if(!v_scale || !a_scale)return -1;

    if(av_type==V_TYPE)
    {
        if(val_type!=VAL_KEY)
        {
            if((v_lastIndexNum+1 ==  indexNum) && (v_lastIndexNum!=0) && v_scale>1)
            {
                //FS_Seek(avi->fdes, v_lastPosInFile + PAD_EVEN(v_lastLen), FS_SEEK_SET);
                avi_fs_seek(avi, v_lastPosInFile + PAD_EVEN(v_lastLen), FS_SEEK_SET);

                chunkPass=0;

                do
                {
                    //if ((readByte=FS_Read(avi->fdes, temp, 8)) <= 0) return-1;
                    if ((readByte=avi_fs_read(avi, temp, 8)) <= 0) return-1;

                    if(strncasecmp(temp+2,avi->video_tag+2,2) == 0 || strncasecmp(temp+2,"dc",2) == 0)
                    {
                        v_lastPosInFile += PAD_EVEN(v_lastLen) + 8 + chunkPass;
                        v_lastLen=str2ulong((uint8*)(temp+4));
                        v_lastIndexNum++;
                        //FS_Seek(avi->fdes,  PAD_EVEN(v_lastLen), FS_SEEK_CUR);
                        avi_fs_seek(avi,  PAD_EVEN(v_lastLen), FS_SEEK_CUR);
                        chunkPass=0;
                    }
                    else if(strncasecmp(temp+2 , avi->audio_tag+2,2) == 0)
                    {
                        res=PAD_EVEN(str2ulong((uint8*)(temp+4)));
                        //FS_Seek(avi->fdes,  res, FS_SEEK_CUR);
                        avi_fs_seek(avi,  res, FS_SEEK_CUR);
                        chunkPass += res+8;
                    }
                    else if(strncasecmp(temp+1,avi->video_tag+2,2) == 0 || strncasecmp(temp+1,"dc",2) == 0)
                    {
                        if(chunkPass)
                            chunkPass--;
                        v_lastPosInFile += v_lastLen + 8 + chunkPass ;
                        v_lastLen= str2ulong((uint8*)(temp+3));
                        v_lastIndexNum++;
                        //FS_Seek(avi->fdes,  PAD_EVEN(v_lastLen)-1, FS_SEEK_CUR);
                        avi_fs_seek(avi,  PAD_EVEN(v_lastLen)-1, FS_SEEK_CUR);
                        chunkPass=0;
                    }
                    else if(strncasecmp(temp+1 , avi->audio_tag+2,2) == 0)
                    {
                        res=PAD_EVEN(str2ulong((uint8*)(temp+3)))-1;
                        //FS_Seek(avi->fdes,  res, FS_SEEK_CUR);
                        avi_fs_seek(avi,  res, FS_SEEK_CUR);
                        chunkPass += res+8;
                    }
                    else
                    {
                        diag_printf( "avi_part_index_read err indexNum=%d tag=%x", indexNum, *((int32*)temp));
                        return -1;
                    }


                }
                while( v_lastIndexNum!=indexNum && v_lastIndexNum<avi->video_frames );

                //diag_printf( "222 indexNum=%d v_lastIndexNum=%d v_lastPosInFile=%d  v_lastLen=%d \n",indexNum, v_lastIndexNum, v_lastPosInFile, v_lastLen);

            }
            else if(( (v_lastIndexNum!=indexNum) || indexNum==0 ) && v_scale>1)
            {

                beginNumInBuf=indexNum/v_scale;

                if(avi->video_index[beginNumInBuf].indexNum>indexNum)
                {
                    do
                    {
                        beginNumInBuf--;
                    }
                    while( beginNumInBuf!=0 && avi->video_index[beginNumInBuf].indexNum>indexNum );
                    //diag_printf( "000 indexNum=%d beginNumInBuf=%d video_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->video_index[beginNumInBuf].indexNum);
                }
                else if(avi->video_index[beginNumInBuf].indexNum<indexNum)
                {
                    do
                    {
                        beginNumInBuf++;
                    }
                    while(beginNumInBuf<v_countInBuf && avi->video_index[beginNumInBuf].indexNum<=indexNum );
                    beginNumInBuf--;
                    //diag_printf( "000 indexNum=%d beginNumInBuf=%d video_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->video_index[beginNumInBuf].indexNum);
                }

                if(indexNum==avi->video_index[beginNumInBuf].indexNum)
                {
                    v_lastIndexNum=avi->video_index[beginNumInBuf].indexNum;
                    v_lastPosInFile=avi->video_index[beginNumInBuf].pos;
                    v_lastLen=avi->video_index[beginNumInBuf].len;
                }
                else
                {
                    res=avi->video_index[beginNumInBuf].len;
                    //FS_Seek(avi->fdes,  avi->video_index[beginNumInBuf].pos + PAD_EVEN(res), FS_SEEK_SET);
                    avi_fs_seek(avi,  avi->video_index[beginNumInBuf].pos + PAD_EVEN(res), FS_SEEK_SET);
                    v_lastIndexNum=avi->video_index[beginNumInBuf].indexNum;
                    v_lastPosInFile=avi->video_index[beginNumInBuf].pos;
                    v_lastLen=res;
                    beginNumInBuf=v_lastIndexNum;

                    chunkPass=0;

                    do
                    {
                        //if ((readByte=FS_Read(avi->fdes, temp, 8)) <= 0)
                        if ((readByte=avi_fs_read(avi, temp, 8)) <= 0)
                        {
                            diag_printf( "err readByte=%d", readByte);
                            return-1;
                        }

                        if(strncasecmp(temp+2,avi->video_tag+2,2) == 0 || strncasecmp(temp+2,"dc",2) == 0)
                        {
                            v_lastPosInFile += PAD_EVEN(v_lastLen) + 8 + chunkPass;
                            v_lastLen= str2ulong((uint8*)(temp+4));
                            v_lastIndexNum++;
                            //FS_Seek(avi->fdes,  PAD_EVEN(v_lastLen) , FS_SEEK_CUR);
                            avi_fs_seek(avi,  PAD_EVEN(v_lastLen) , FS_SEEK_CUR);
                            chunkPass=0;
                        }
                        else if(strncasecmp(temp +2, avi->audio_tag+2,2) == 0)
                        {
                            res=PAD_EVEN(str2ulong((uint8*)(temp+4)));
                            //FS_Seek(avi->fdes,  res , FS_SEEK_CUR);
                            avi_fs_seek(avi,  res , FS_SEEK_CUR);
                            chunkPass +=res+ 8;
                        }
                        else if(strncasecmp(temp+1,avi->video_tag+2,2) == 0 || strncasecmp(temp+1,"dc",2) == 0)
                        {
                            if(chunkPass)
                                chunkPass--;
                            v_lastPosInFile += v_lastLen + 8 + chunkPass;
                            v_lastLen= str2ulong((uint8*)(temp+3));
                            v_lastIndexNum++;
                            //FS_Seek(avi->fdes,  PAD_EVEN(v_lastLen)-1 , FS_SEEK_CUR);
                            avi_fs_seek(avi,  PAD_EVEN(v_lastLen)-1 , FS_SEEK_CUR);
                            chunkPass=0;
                        }
                        else if(strncasecmp(temp +1, avi->audio_tag+2,2) == 0)
                        {
                            res=PAD_EVEN(str2ulong((uint8*)(temp+3)))-1;
                            //FS_Seek(avi->fdes,  res , FS_SEEK_CUR);
                            avi_fs_seek(avi,  res , FS_SEEK_CUR);
                            chunkPass +=res+ 8;
                        }
                        else
                        {
                            diag_printf( "err flag=%x %x %x %x pos=%d", temp[0],  temp[1],  temp[2],  temp[3], v_lastPosInFile);
                            return -1;
                        }

                        if(v_lastIndexNum-beginNumInBuf >= v_scale && ((uint32)*((uint32*)avi->compressor2)==AVI_FORMAT_MJPG))
                        {
                            diag_printf( "err v_lastIndexNum-beginNumInBuf=%d v_scale=%d ", v_lastIndexNum-beginNumInBuf,  v_scale);
                            return -1;
                        }

                        //diag_printf( "111 indexNum=%d tag=%x %x %x %x\n", indexNum, temp[0],temp[1],temp[2],temp[3]);
                    }
                    while( v_lastIndexNum!=indexNum && v_lastIndexNum<avi->video_frames);
                    //diag_printf( "111 indexNum=%d v_lastIndexNum=%d v_lastPosInFile=%d  v_lastLen=%d \n",indexNum, v_lastIndexNum, v_lastPosInFile, v_lastLen);
                }

            }
            else if(v_scale==1)
            {
                v_lastPosInFile=avi->video_index[indexNum].pos;
                v_lastLen=avi->video_index[indexNum].len;
            }

            if(val_type==VAL_POS)
            {
                res=v_lastPosInFile;
            }
            else if(val_type==VAL_LEN)
            {
                res=v_lastLen;
            }
            else
                res=-1;
        }
        else//find the neighbor key index
        {
            uint32 i;
            beginNumInBuf=indexNum/v_scale;

            if(avi->video_index[beginNumInBuf].indexNum>indexNum)
            {
                do
                {
                    beginNumInBuf--;
                }
                while( beginNumInBuf!=0 && avi->video_index[beginNumInBuf].indexNum>=indexNum );
                beginNumInBuf++;
                //diag_printf( "000 indexNum=%d beginNumInBuf=%d video_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->video_index[beginNumInBuf].indexNum);
            }
            else if(avi->video_index[beginNumInBuf].indexNum<indexNum)
            {
                do
                {
                    beginNumInBuf++;
                }
                while(beginNumInBuf<v_countInBuf && avi->video_index[beginNumInBuf].indexNum<indexNum );

                //diag_printf( "001 indexNum=%d beginNumInBuf=%d video_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->video_index[beginNumInBuf].indexNum);
            }

            i=beginNumInBuf;
            while(avi->video_index[i].key!=0x10 || avi->video_index[i].len==0)
            {
                i++;
                if(i>=v_countInBuf)
                    break;
            }

            if(i>=v_countInBuf)
            {
                i=beginNumInBuf;
                while(avi->video_index[i].key!=0x10 || avi->video_index[i].len==0)
                {
                    if(i==0)
                        break;
                    i--;
                }
            }

            if(avi->video_index[i].key==0x10)
            {
                v_lastIndexNum=avi->video_index[i].indexNum;
                v_lastPosInFile=avi->video_index[i].pos;
                v_lastLen=avi->video_index[i].len;

                res=v_lastIndexNum;
            }
            else
            {
                res=0;
            }
        }

//time=time_measure_end();
//if(maxT<time)maxT=time;
//diag_printf( ".............................time =%d max=%d \n",time/78, maxT/78);
    }
    else if(av_type==A_TYPE)
    {
        if( (a_lastIndexNum+1 ==  indexNum) && (a_lastIndexNum!=0) && a_scale>1)
        {
            //FS_Seek(avi->fdes,  a_lastPosInFile + PAD_EVEN(a_lastLen) , FS_SEEK_SET);
            avi_fs_seek(avi,  a_lastPosInFile + PAD_EVEN(a_lastLen) , FS_SEEK_SET);

            chunkPass=0;

            do
            {
                //if ((readByte=FS_Read(avi->fdes, temp, 8)) <= 0) return-1;
                if ((readByte=avi_fs_read(avi, temp, 8)) <= 0) return-1;

                if(strncasecmp(temp+2,avi->audio_tag+2,2) == 0)
                {
                    a_lastPosInFile += PAD_EVEN(a_lastLen) + 8 + chunkPass;
                    a_lastTot+=a_lastLen;
                    a_lastLen= str2ulong((uint8*)(temp+4));
                    a_lastIndexNum++;
                    //FS_Seek(avi->fdes,   PAD_EVEN(a_lastLen) , FS_SEEK_CUR);
                    avi_fs_seek(avi,   PAD_EVEN(a_lastLen) , FS_SEEK_CUR);
                    chunkPass=0;
                }
                else if(strncasecmp(temp+2 , avi->video_tag+2,2) == 0 || strncasecmp(temp+2,"dc",2) == 0)
                {
                    res=PAD_EVEN(str2ulong((uint8*)(temp+4)));
                    //FS_Seek(avi->fdes,   res , FS_SEEK_CUR);
                    avi_fs_seek(avi,   res , FS_SEEK_CUR);
                    chunkPass += res+8;
                }
                else if(strncasecmp(temp+1,avi->audio_tag+2,2) == 0)
                {
                    if(chunkPass)
                        chunkPass--;
                    a_lastPosInFile += a_lastLen + 8 + chunkPass;
                    a_lastTot+=a_lastLen;
                    a_lastLen= str2ulong((uint8*)(temp+3));
                    a_lastIndexNum++;
                    //FS_Seek(avi->fdes,   PAD_EVEN(a_lastLen)-1 , FS_SEEK_CUR);
                    avi_fs_seek(avi,   PAD_EVEN(a_lastLen)-1 , FS_SEEK_CUR);
                    chunkPass=0;
                }
                else if(strncasecmp(temp+1 , avi->video_tag+2,2) == 0 || strncasecmp(temp+1,"dc",2) == 0)
                {
                    res=PAD_EVEN(str2ulong((uint8*)(temp+3)))-1;
                    //FS_Seek(avi->fdes,   res , FS_SEEK_CUR);
                    avi_fs_seek(avi,   res , FS_SEEK_CUR);
                    chunkPass += res+8;
                }
                else
                {
                    diag_printf( "avi_part_index_read err indexNum=%d tag=%x", indexNum, *((int32*)temp));
                    return -1;
                }

            }
            while(a_lastIndexNum!=indexNum &&  a_lastIndexNum<avi->audio_chunks );
            //diag_printf( "222 indexNum=%d a_lastIndexNum=%d a_lastPosInFile=%d  a_lastLen=%d \n",indexNum, a_lastIndexNum, a_lastPosInFile, a_lastLen);
        }
        else if(((a_lastIndexNum!=indexNum)  || indexNum==0) && a_scale>1 )
        {
            beginNumInBuf=indexNum/a_scale;

            if(avi->audio_index[beginNumInBuf].indexNum>indexNum)
            {
                do
                {
                    beginNumInBuf--;
                }
                while( beginNumInBuf!=0 && avi->audio_index[beginNumInBuf].indexNum>indexNum );
                //diag_printf( "000 indexNum=%d beginNumInBuf=%d audio_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->audio_index[beginNumInBuf].indexNum);
            }
            else if(avi->audio_index[beginNumInBuf].indexNum<indexNum)
            {
                do
                {
                    beginNumInBuf++;
                }
                while(beginNumInBuf<a_countInBuf && avi->audio_index[beginNumInBuf].indexNum<=indexNum );
                beginNumInBuf--;
                //diag_printf( "000 indexNum=%d beginNumInBuf=%d audio_index[beginNumInBuf].indexNum=%d  \n",indexNum, beginNumInBuf, avi->audio_index[beginNumInBuf].indexNum);
            }

            if(indexNum==avi->audio_index[beginNumInBuf].indexNum)
            {
                a_lastIndexNum=avi->audio_index[beginNumInBuf].indexNum;
                a_lastPosInFile=avi->audio_index[beginNumInBuf].pos;
                a_lastLen=avi->audio_index[beginNumInBuf].len;
                a_lastTot=avi->audio_index[beginNumInBuf].tot;
            }
            else
            {
                res=avi->audio_index[beginNumInBuf].len;
                //FS_Seek(avi->fdes,   avi->audio_index[beginNumInBuf].pos + PAD_EVEN(res), FS_SEEK_SET);
                avi_fs_seek(avi,   avi->audio_index[beginNumInBuf].pos + PAD_EVEN(res), FS_SEEK_SET);
                a_lastIndexNum=avi->audio_index[beginNumInBuf].indexNum;
                a_lastPosInFile=avi->audio_index[beginNumInBuf].pos;
                a_lastLen=res;
                a_lastTot=avi->audio_index[beginNumInBuf].tot;
                beginNumInBuf=a_lastIndexNum;

                chunkPass=0;

                do
                {
                    //if ((readByte=FS_Read(avi->fdes, temp, 8)) <= 0) return-1;
                    if ((readByte=avi_fs_read(avi, temp, 8)) <= 0) return-1;

                    if(strncasecmp(temp+2,avi->audio_tag+2,2) == 0)
                    {
                        a_lastPosInFile += PAD_EVEN(a_lastLen) + 8 + chunkPass;
                        a_lastTot+=a_lastLen;
                        a_lastLen= str2ulong((uint8*)(temp+4));
                        a_lastIndexNum++;
                        //FS_Seek(avi->fdes,   PAD_EVEN(a_lastLen) , FS_SEEK_CUR);
                        avi_fs_seek(avi,   PAD_EVEN(a_lastLen) , FS_SEEK_CUR);
                        chunkPass=0;
                    }
                    else if(strncasecmp(temp+2 , avi->video_tag+2,2) == 0 || strncasecmp(temp+2,"dc",2) == 0)
                    {
                        res=PAD_EVEN(str2ulong((uint8*)(temp+4)));
                        //FS_Seek(avi->fdes,   res , FS_SEEK_CUR);
                        avi_fs_seek(avi,   res , FS_SEEK_CUR);
                        chunkPass +=res+ 8;
                    }
                    else if(strncasecmp(temp+1,avi->audio_tag+2,2) == 0)
                    {
                        if(chunkPass)
                            chunkPass--;
                        a_lastPosInFile += a_lastLen + 8 + chunkPass;
                        a_lastTot+=a_lastLen;
                        a_lastLen= str2ulong((uint8*)(temp+3));
                        a_lastIndexNum++;
                        //FS_Seek(avi->fdes,   PAD_EVEN(a_lastLen) -1, FS_SEEK_CUR);
                        avi_fs_seek(avi,   PAD_EVEN(a_lastLen) -1, FS_SEEK_CUR);
                        chunkPass=0;
                    }
                    else if(strncasecmp(temp+1 , avi->video_tag+2,2) == 0 || strncasecmp(temp+2,"dc",2) == 0)
                    {
                        res=PAD_EVEN(str2ulong((uint8*)(temp+3)))-1;
                        //FS_Seek(avi->fdes,   res , FS_SEEK_CUR);
                        avi_fs_seek(avi,   res , FS_SEEK_CUR);
                        chunkPass +=res+ 8;
                    }
                    else
                    {
                        diag_printf( "avi_part_index_read err indexNum=%d tag=%x", indexNum, *((int32*)temp));
                        return -1;
                    }

                    if(a_lastIndexNum-beginNumInBuf >= a_scale)
                    {
                        diag_printf( "avi_part_index_read err a_lastIndexNum-beginNumInBuf >= a_scale");
                        return -1;
                    }

                }
                while( a_lastIndexNum!=indexNum && a_lastIndexNum<avi->audio_chunks );
                //diag_printf( "111 indexNum=%d a_lastIndexNum=%d a_lastPosInFile=%d  a_lastLen=%d \n",indexNum, a_lastIndexNum, a_lastPosInFile, a_lastLen);
            }
        }
        else if(a_scale==1)
        {
            a_lastPosInFile=avi->audio_index[indexNum].pos;
            a_lastLen=avi->audio_index[indexNum].len;
            a_lastTot=avi->audio_index[indexNum].tot;
        }

        if(val_type==VAL_POS)
        {
            res=a_lastPosInFile;
        }
        else if(val_type==VAL_LEN)
        {
            res=a_lastLen;
        }
        else if(val_type==VAL_TOT)
        {
            res=a_lastTot;
        }
        else
            res=-1;
    }
    else
    {
        diag_printf( "avi_part_index_read err av_type=%d", av_type);
        return -1;
    }

    return res;
}


/*************************************************************************
* FUNCTION
*   avi_parse_input_file
*
* DESCRIPTION
*   Parse the AVI file.
*
* PARAMETERS
*   avi_struct *avi
*  int32 getIndex : Get the index info or not.
*
* RETURNS
*   int32:    Return values:
*                   0   No rrror;
* GLOBALS AFFECTED
*
**********************1***************************************************/
int32 avi_parse_input_file(avi_struct *avi, int32 getIndex)
{
    int32 res;
    int32 i, idx_type;
    int32 n;
    uint32 rate=0, scale=0;
    uint8 *hdrl_data=0;
    int32 hdrl_len = 0;
    int32 hdrl_pos=0;
    int32 hdrl_offset=0;
    uint32 nvi, nai, ioff;
    int32 tot;
    int32 lasttag = 0;
    int32 vids_strh_seen = 0;
    int32 vids_strf_seen = 0;
    int32 auds_strh_seen = 0;
    int32 auds_strf_seen = 0;
    uint32 num_stream = 0;
    uint8 *data;//[512];
    uint16 junk_count=0;
    // uint32 start_time, current_time;
    int32 index_pos=0;
    uint32 ext_indx_pos_vid=0;
    uint32 ext_indx_pos_aud=0;
    uint32 ext_indx_num_vid=0;
    uint32 ext_indx_num_aud=0;
    uint32 ext_indx_bsofst_vid=0;
    uint32 ext_indx_bsofst_aud=0;

    int32 offset, cpylen;
    int8 data_temp[16];
    int8 (*p_idx)[16];
    typedef int8  (*P_IDX_TYPE)[16];

    /* Read first 12 bytes and check that this is an AVI file */

    idx_type=-1;// -1 non idx

    if((data=(uint8*)mmc_MemMalloc(512))<=0) ERR_EXIT(AVI_ERR_NO_MEM)

        //if( avi_read((int32)avi->fdes,data,12) != 12 ) ERR_EXIT(AVI_ERR_READ)
        if( avi_fs_read(avi,data,12) != 12 ) ERR_EXIT(AVI_ERR_READ)

            if( strncmp((const int8 *)data  ,"RIFF",4) !=0 ||
                    strncmp((const int8 *)(data+8),"AVI ",4) !=0 ) ERR_EXIT(AVI_ERR_NO_AVI)

                /* Go through the AVI file and extract the header list,
                   the start position of the 'movi' list and an optionally
                   present idx1 tag */

                //kal_get_time(&start_time);

                while(1)
                {
                    //if( avi_read(avi->fdes,data,8) != 8 ) break; /* We assume it's EOF */
                    if( (res=avi_fs_read(avi,data,8)) != 8 )
                    {
                        diag_printf("[AVI] end of the file! res=%d");
                        res=avi_fs_seek(avi,0,FS_SEEK_CUR);
                        diag_printf("[AVI] cur pos res=%d",res);
                        break; /* We assume it's EOF */
                    }

                    n = str2ulong((uint8*)data+4);
                    n = PAD_EVEN(n);
                    /* check if parse too long */
                    //kal_get_time(&current_time);
                    //VID_VALUE_TRACE(start_time, current_time, __LINE__);
#if 0
                    if((current_time-start_time)>MAX_PARSING_TIME)
                    {

                        VID_VALUE_TRACE(start_time, current_time, __LINE__);
                        ERR_EXIT(AVI_ERR_SIZELIM)
                    }
#endif

                    if(strncasecmp(data,"LIST",4) == 0)
                    {

                        //if( avi_read(avi->fdes,data,4) != 4 ) ERR_EXIT(AVI_ERR_READ)
                        if( avi_fs_read(avi,data,4) != 4 ) ERR_EXIT(AVI_ERR_READ)
                            n -= 4;
                        if(strncasecmp(data,"hdrl",4) == 0)
                        {
                            hdrl_len = n;
                            //hdrl_data = (uint8 *) mmc_MemMalloc((int32)n);
                            //diag_printf( "shenh avi_parse_input_file hdrl_data malloc = %d \n",n);
                            //if(hdrl_data==0) ERR_EXIT(AVI_ERR_NO_MEM)

                            //avi->a_codecf_off = avi_seek(avi->fdes,0,FS_SEEK_CUR);
                            hdrl_pos= avi_fs_seek(avi,0,FS_SEEK_CUR);
                            avi->a_codecf_off =hdrl_pos;
                            avi->a_codech_off = avi->a_codecf_off;
                            avi->v_codech_off = avi->a_codecf_off;
                            avi->v_codecf_off = avi->a_codecf_off;

                            //if( avi_read(avi->fdes,(int8*)hdrl_data,n) != n ) ERR_EXIT(AVI_ERR_READ)
                            //if( avi_fs_read(avi,(int8*)hdrl_data,n) != n ) ERR_EXIT(AVI_ERR_READ)
                            if(avi_fs_seek(avi,n,FS_SEEK_CUR) !=hdrl_pos+n)ERR_EXIT(AVI_ERR_READ)
                            }
                        else if(strncasecmp(data,"movi",4) == 0)
                        {
                            //avi->movi_start = avi_seek(avi->fdes,0,FS_SEEK_CUR);
                            avi->movi_start = avi_fs_seek(avi,0,FS_SEEK_CUR);
                            //avi_seek(avi->fdes,n,FS_SEEK_CUR);

                            if(n>=0)
                            {
                                res=avi_fs_seek(avi,n,FS_SEEK_CUR);
                                if(res!=(avi->movi_start+n))
                                {
                                    diag_printf("[AVI]seek err res=%d \n",res);
                                    break;
                                }
                            }
                            else
                            {
                                res=avi_fs_seek(avi,0,FS_SEEK_END);
                                n=res-avi->movi_start;
                            }
                        }
                        else
                        {

                            //avi_seek(avi->fdes,n,FS_SEEK_CUR);
                            avi_fs_seek(avi,n,FS_SEEK_CUR);
                        }
                    }
                    else if(strncasecmp(data,"idx1",4) == 0)
                    {

                        idx_type=0;//0 use idx
                        //index_pos = FS_Seek(avi->fdes, 0, FS_SEEK_CUR);
                        index_pos = avi_fs_seek(avi, 0, FS_SEEK_CUR);
                        /* n must be a multiple of 16, but the reading does not
                        break if this is not the case */
                        avi->n_idx = avi->max_idx = n/16;
                        // avi->idx = (uint8((*)[16]) ) malloc(16);
                        // if(avi->idx==0) ERR_EXIT(AVI_ERR_NO_MEM)
                        //         if(avi_read(avi->fdes, (int8 *) avi->idx, n) != n ) ERR_EXIT(AVI_ERR_READ)
                        //if(FS_Seek(avi->fdes, n, FS_SEEK_CUR)-index_pos<n-1)
                        if(avi_fs_seek(avi, n, FS_SEEK_CUR)-index_pos<n-1)
                        {
                            diag_printf("[AVI]idx1 data area bad!process as non-idx.");
                            idx_type=-1;
                            index_pos=0;
                            avi->n_idx=0;
                        }
                    }
                    else
                    {
                        junk_count++;
                        if(junk_count>MAX_JUNK_COUNT)
                            break;
                        //avi_seek(avi->fdes,n,FS_SEEK_CUR);
                        avi_fs_seek(avi,n,FS_SEEK_CUR);
                    }
                }

    /*
    if(avi->n_idx==0)//add shenh
    {
        diag_printf("[AVI] not support non idx1 now!");
        ERR_EXIT(AVI_ERR_NO_IDX)
    }
    */
    if(idx_type==-1 || avi->n_idx<=0)//add shenh
    {
        diag_printf("[AVI] non idx1 AVI! idx_type=%d n_idx=%d \n",idx_type,avi->n_idx);
    }

    //if(!hdrl_data      ) ERR_EXIT(AVI_ERR_NO_HDRL)
    if(!avi->movi_start) ERR_EXIT(AVI_ERR_NO_MOVI)

        /* Interpret the header list */

        if(avi_fs_seek(avi, hdrl_pos, FS_SEEK_SET)!=hdrl_pos)
            ERR_EXIT(AVI_ERR_NO_HDRL)

            memset(data, 0, 512);
    if(hdrl_len<=512)
    {
        if( avi_fs_read(avi,data,hdrl_len) != hdrl_len ) ERR_EXIT(AVI_ERR_READ)
        }
    else
    {
        if( avi_fs_read(avi,data,512) != 512 ) ERR_EXIT(AVI_ERR_READ)
        }

    hdrl_offset=0;//first data offset from hdrl like i.
    hdrl_data=data;



    for(i=0; i<hdrl_len;)
    {
        /* List tags are completly ignored */
        if(((i-hdrl_offset) >=384) && ((hdrl_offset+512)<hdrl_len))//move last 128bytes to top.
        {
            int32 need;
            if((i-hdrl_offset)<512)
            {

                memcpy(hdrl_data, hdrl_data+(i-hdrl_offset), 512-(i-hdrl_offset));
                if(i-hdrl_offset <= hdrl_len-(hdrl_offset+512))
                    need=i-hdrl_offset;
                else
                    need=hdrl_len-(hdrl_offset+512);

                if( avi_fs_read(avi,hdrl_data+(512-(i-hdrl_offset)),need) != need ) ERR_EXIT(AVI_ERR_READ)

                    hdrl_offset+=(i-hdrl_offset);

            }
            else
            {

                if(avi_fs_seek(avi, i-(hdrl_offset+512), FS_SEEK_CUR)!=hdrl_pos+i)ERR_EXIT(AVI_ERR_READ)
                    if(512 <= hdrl_len-i)
                        need=512;
                    else
                        need=hdrl_len-i;

                if( avi_fs_read(avi,hdrl_data,need) != need ) ERR_EXIT(AVI_ERR_READ)
                    hdrl_offset=i;
            }

        }

        if(strncasecmp((int8*)hdrl_data+i-hdrl_offset,"LIST",4)==0) { i+= 12; continue; }

        n = str2ulong((uint8*)hdrl_data+i-hdrl_offset+4);
        n = PAD_EVEN(n);

        /* Interpret the tag and its args */
        if(strncasecmp(hdrl_data+i-hdrl_offset,"avih",4)==0)
        {
            /*
            diag_printf(  "[ALG_AVI] coolsand_avih \n");
            if(strncasecmp(hdrl_data+i+48,"coolsand",8) != 0)
            {
            ERR_EXIT(AVI_ERR_FORTMAT);
            }
            */
        }
        else if(strncasecmp(hdrl_data+i-hdrl_offset,"strh",4)==0)
        {
            i += 8;
            if(strncasecmp(hdrl_data+i-hdrl_offset,"vids",4) == 0 && !vids_strh_seen)
            {
                memcpy(avi->compressor,hdrl_data+i-hdrl_offset+4,4);
                avi->compressor[4] = 0;

                avi->v_codech_off += i+4;
                scale = str2ulong((uint8*)(hdrl_data+i-hdrl_offset+20));
                rate  = str2ulong((uint8*)(hdrl_data+i-hdrl_offset+24));

                if(scale!=0)
                    avi->fps =rate/scale;

                if(scale * avi->fps < rate)
                {
                    avi->fps++;
                }

                /*
                if (avi->fps>15)
                {
                diag_printf(  "[ALG_AVI] avi_parse_input_file avi_fps = %d, attention!!!\n", avi->fps);
                //ERR_EXIT(AVI_ERR_FORTMAT);
                }
                */
                avi->video_frames = str2ulong((uint8*)hdrl_data+i-hdrl_offset+32);
                if(avi->video_frames==0)
                {
                    if(avi->n_idx > 2)
                        avi->video_frames=(avi->n_idx)>>1;
                    else
                        avi->video_frames=200000;
                }
                avi->video_strn = num_stream;
                avi->max_len = 0;
                vids_strh_seen = 1;
                lasttag = 1; /* vids */
            }
            else if (strncasecmp (hdrl_data+i-hdrl_offset,"auds",4) ==0 && ! auds_strh_seen)
            {
                avi->audio_bytes = str2ulong((uint8*)hdrl_data+i-hdrl_offset+32)*avi_sampsize(avi);
                avi->audio_strn = num_stream;
                auds_strh_seen = 1;
                lasttag = 2; /* auds */

                avi->a_codech_off += i;

            }
            else
            {
                lasttag = 0;
            }

            num_stream++;
        }
        else if(strncasecmp(hdrl_data+i-hdrl_offset,"strf",4)==0)
        {
            i += 8;
            if(lasttag == 1)
            {
                avi->width  = str2ulong((uint8*)hdrl_data+i-hdrl_offset+4);
                avi->height = str2ulong((uint8*)hdrl_data+i-hdrl_offset+8);
                /*
                if (avi->width>=320||avi->height>=240)
                {
                diag_printf(  "[ALG_AVI] avi_parse_input_file avi->width = %d, avi->height=%d attention!!! \n",avi->width,avi->height);
                // ERR_EXIT(AVI_ERR_FORTMAT);
                }
                */
                vids_strf_seen = 1;
                avi->v_codecf_off += i+16;

                memcpy(avi->compressor2, hdrl_data+i-hdrl_offset+16, 4);
                //avi->compressor2[4] = 0;

            }
            else if(lasttag == 2)
            {
                avi->a_fmt   = str2ushort(hdrl_data+i -hdrl_offset );

                avi->a_codecf_off += i;

                avi->a_chans = str2ushort(hdrl_data+i-hdrl_offset+2);
                avi->a_rate  = str2ulong (hdrl_data+i-hdrl_offset+4);
                avi->mp3rate = 8*str2ulong((uint8*)hdrl_data+i-hdrl_offset+8);
                avi->a_bits  = str2ushort(hdrl_data+i-hdrl_offset+14);
                auds_strf_seen = 1;
            }

            //lasttag = 0;
        }
        else if(strncasecmp(hdrl_data+i-hdrl_offset,"indx",4)==0)//extension indx
        {
            i+=8;
            if(lasttag == 1)
            {
                ext_indx_pos_vid=str2ulong (hdrl_data+i-hdrl_offset+24);
            }
            else if(lasttag == 2)
            {
                ext_indx_pos_aud=str2ulong (hdrl_data+i-hdrl_offset+24);
            }
            idx_type=3;//indx
            diag_printf("[AVI] ext indx !");
        }
        else
        {
            i += 8;
            lasttag = 0;
        }

        i += n;
    }

    //free(hdrl_data);
    // hdrl_data=0;

    if(!vids_strh_seen || !vids_strf_seen) ERR_EXIT(AVI_ERR_NO_VIDS)
        if(idx_type==-1 && (res=avi_fs_getFileSize(avi)) >(200*1024*1024))
        {
            diag_printf("[AVI]too big non-idx file size=%d !!!\n", res);
            ERR_EXIT(AVI_ERR_NO_IDX)
        }
        else if(avi->width*avi->height > 320*240 && (res=avi_fs_getFileSize(avi)) >(200*1024*1024))
        {
            diag_printf("[AVI]too big resolution & file size w=%d h=%d !!!\n", avi->width, avi->height);
            ERR_EXIT(AVI_ERR_FORTMAT)
        }


    avi->video_tag[0] = avi->video_strn/10 + '0';
    avi->video_tag[1] = avi->video_strn%10 + '0';
    avi->video_tag[2] = 'd';
    avi->video_tag[3] = 'b';

    /* Audio tag is set to "99wb" if no audio present */
    if(!avi->a_chans) avi->audio_strn = 99;

    avi->audio_tag[0] = avi->audio_strn/10 + '0';
    avi->audio_tag[1] = avi->audio_strn%10 + '0';
    avi->audio_tag[2] = 'w';
    avi->audio_tag[3] = 'b';

    //avi_seek(avi->fdes,avi->movi_start,FS_SEEK_SET);
    avi_fs_seek(avi,avi->movi_start,FS_SEEK_SET);

    /* get index if wanted */


    if(!getIndex) return(avi->video_frames);
    //idx_type = 2;

    /* if the file has an idx1, check if this is relative
       to the start of the file or to the start of the movi list */
    /*
    //the following code is to add the index entry for some avi without the index part.
    //but for coolsand, it is not useful
    */

#if 0

    idx_type = 0;

    if(avi->idx)
    {
        int32 pos, len;

        /* Search the first videoframe in the idx1 and look where
           it is in the file */

        for(i=0; i<avi->n_idx; i++)
            if( strncasecmp(avi->idx[i],avi->video_tag,3)==0 ) break;
        if(i>=avi->n_idx) ERR_EXIT(AVI_ERR_NO_VIDS)

            pos = str2ulong((uint8*)avi->idx[i]+ 8);
        len = str2ulong((uint8*)avi->idx[i]+12);

        avi_seek(avi->fdes,pos,FS_SEEK_SET);
        if(avi_read(avi->fdes,data,8)!=8) ERR_EXIT(AVI_ERR_READ)
            if( strncasecmp(data,avi->idx[i],4)==0 && str2ulong((uint8*)data+4)==len )
            {
                idx_type = 1; /* Index from start of file */
            }
            else
            {
                avi_seek(avi->fdes,pos+avi->movi_start-4,FS_SEEK_SET);
                if(avi_read(avi->fdes,data,8)!=8) ERR_EXIT(AVI_ERR_READ)
                    if( strncasecmp(data,avi->idx[i],4)==0 && str2ulong((uint8*)data+4)==len )
                    {
                        idx_type = 2; /* Index from start of movi list */
                    }
            }
        /* idx_type remains 0 if neither of the two tests above succeeds */
    }




    if(idx_type == 0)
    {
        /* we must search through the file to get the index */

        avi_seek(avi->fdes, avi->movi_start, FS_SEEK_SET);

        avi->n_idx = 0;
        junk_count=0;
        while(1)
        {
            /* check if parse too long */
            kal_get_time(&current_time);
            //VID_VALUE_TRACE(start_time, current_time, __LINE__);
            if((current_time-start_time)>MAX_PARSING_TIME)
            {
                VID_VALUE_TRACE(start_time, current_time, __LINE__);
                ERR_EXIT(AVI_ERR_SIZELIM)
            }

            if( avi_read(avi->fdes,data,8) != 8 ) break;
            n = str2ulong((uint8*)data+4);

            /* The movi list may contain sub-lists, ignore them */

            if(strncasecmp(data,"LIST",4)==0)
            {
                avi_seek(avi->fdes,4,FS_SEEK_CUR);
                continue;
            }

            /* Check if we got a tag ##db, ##dc or ##wb */

            if( ( (data[2]=='d' || data[2]=='D') &&
                    (data[3]=='b' || data[3]=='B' || data[3]=='c' || data[3]=='C') )
                    || ( (data[2]=='w' || data[2]=='W') &&
                         (data[3]=='b' || data[3]=='B') ) )
            {
                if(avi_add_index_entry(avi,(uint8*)data,0,(uint32)avi_seek(avi->fdes,0,FS_SEEK_CUR)-8,n)) ERR_EXIT(AVI_ERR_NO_MEM)
                }
            else
            {
                junk_count++;
                if(junk_count>MAX_JUNK_COUNT)
                    break;
            }

            avi_seek(avi->fdes,PAD_EVEN(n),FS_SEEK_CUR);
        }
        idx_type = 1;
    }
#endif

    /* Now generate the video index and audio index arrays */

    // nvi = 0;
    // nai = 0;
    /*
    //for coolsand avi file, the video frame nubmer is alway  =  audio chunks
    // so the following code has been conceled only for coolsand avi!
    //
    */
#if 0
    for(i=0; i<avi->n_idx; i++)
    {
        if(strncasecmp(avi->idx[i],avi->video_tag,3) == 0) nvi++;
        if(strncasecmp(avi->idx[i],avi->audio_tag,4) == 0) nai++;
    }
#endif

    // nvi= nai = avi->n_idx>>1;
    //avi->video_frames = nvi;
    if(idx_type==3)
    {
        //vid indx
        if(avi_fs_seek(avi, ext_indx_pos_vid, FS_SEEK_SET)!=ext_indx_pos_vid)
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]seek ext_indx_pos_vid err!\n" );
            return 0;
        }
        if( avi_fs_read(avi,data,sizeof(_aviindex_chunk)) != sizeof(_aviindex_chunk) )
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]read ext_indx_pos_vid err!\n" );
            return 0;
        }
        if(data[10]!=0 || data[11]!=AVI_INDEX_OF_CHUNKS)//only support standard indx.
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]not stardard video indx  err!\n" );
            return 0;
        }
        ext_indx_num_vid= str2ulong((uint8*)data+12);
        ext_indx_bsofst_vid= str2ulong((uint8*)data+20);//only use 32bit offset
        ext_indx_pos_vid+=32;//update pos.
        //diag_printf("[ALG_AVI]indx_num_vid=%d indx_bsofst_vid=%d indx_pos_vid=%d\n",
        //  ext_indx_num_vid,ext_indx_bsofst_vid,ext_indx_pos_vid );

        //aud indx
        if(avi_fs_seek(avi, ext_indx_pos_aud, FS_SEEK_SET)!=ext_indx_pos_aud)
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]seek ext_indx_pos_aud err!\n" );
            return 0;
        }
        if( avi_fs_read(avi,data,sizeof(_aviindex_chunk)) != sizeof(_aviindex_chunk) )
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]read ext_indx_pos_aud err!\n" );
            return 0;
        }
        if(data[10]!=0 || data[11]!=AVI_INDEX_OF_CHUNKS)//only support standard indx.
        {
            avi_errno = AVI_ERR_NO_INDX;
            diag_printf("[ALG_AVI]not stardard audio indx  err!\n" );
            return 0;
        }
        ext_indx_num_aud= str2ulong((uint8*)data+12);
        ext_indx_bsofst_aud= str2ulong((uint8*)data+20);//only use 32bit offset
        ext_indx_pos_aud+=32;//update pos
        avi->audio_chunks=ext_indx_num_aud;
        //diag_printf("[ALG_AVI]indx_num_aud=%d indx_bsofst_aud=%d indx_pos_aud=%d\n",
        //  ext_indx_num_aud,ext_indx_bsofst_aud,ext_indx_pos_aud );

    }
    else if(idx_type!=-1)
    {
        if(avi->n_idx-(int32)avi->video_frames>0)
            avi->audio_chunks = avi->n_idx-(int32)avi->video_frames;
        else if(avi->n_idx>0)
            avi->audio_chunks=avi->n_idx;
        else
            avi->audio_chunks=avi->video_frames;
    }
    else
    {
        int avgAudChunkSize=0;

        //avi_seek(avi->fdes, avi->movi_start, FS_SEEK_SET);
        avi_fs_seek(avi, avi->movi_start, FS_SEEK_SET);
        junk_count=0;
        i=4;
        //get audio chunk average size
        while(1)
        {
            //if( avi_read(avi->fdes,data,8) != 8 ) ERR_EXIT(AVI_ERR_NO_IDX);
            if( avi_fs_read(avi,data,8) != 8 ) ERR_EXIT(AVI_ERR_NO_IDX);
            n = str2ulong((uint8*)data+4);

            /* The movi list may contain sub-lists, ignore them */
            if(strncasecmp(data,"LIST",4)==0)
            {
                //avi_seek(avi->fdes,4,FS_SEEK_CUR);
                avi_fs_seek(avi,4,FS_SEEK_CUR);
                continue;
            }

            /* Check if we got a tag ##db, ##dc or ##wb */

            if(  (data[2]=='w' || data[2]=='W') && (data[3]=='b' || data[3]=='B') )
            {
                avgAudChunkSize+=n;
                i--;
                if(i==0)break;
                //avi_seek(avi->fdes,PAD_EVEN(n),FS_SEEK_CUR);
                avi_fs_seek(avi,PAD_EVEN(n),FS_SEEK_CUR);
            }
            else if ( (data[2]=='d' || data[2]=='D') && (data[3]=='b' || data[3]=='B' || data[3]=='c' || data[3]=='C') )
            {
                //avi_seek(avi->fdes,PAD_EVEN(n),FS_SEEK_CUR);
                avi_fs_seek(avi,PAD_EVEN(n),FS_SEEK_CUR);
            }
            else if(  (data[1]=='w' || data[1]=='W') && (data[2]=='b' || data[2]=='B') )
            {
                n = str2ulong((uint8*)data+3);
                avgAudChunkSize+=n;
                i--;
                if(i==0)break;
                //avi_seek(avi->fdes,PAD_EVEN(n)-1,FS_SEEK_CUR);
                avi_fs_seek(avi,PAD_EVEN(n)-1,FS_SEEK_CUR);
            }
            else if ( (data[1]=='d' || data[1]=='D') && (data[2]=='b' || data[2]=='B' || data[2]=='c' || data[2]=='C') )
            {
                n = str2ulong((uint8*)data+3);
                //avi_seek(avi->fdes,PAD_EVEN(n)-1,FS_SEEK_CUR);
                avi_fs_seek(avi,PAD_EVEN(n)-1,FS_SEEK_CUR);
            }
            else
            {
                junk_count++;
                if(junk_count>MAX_JUNK_COUNT)
                    ERR_EXIT(AVI_ERR_NO_IDX);
                //avi_seek(avi->fdes,PAD_EVEN(n),FS_SEEK_CUR);
                avi_fs_seek(avi,PAD_EVEN(n),FS_SEEK_CUR);
            }
        }

        avgAudChunkSize>>=2;

        if(avi->video_frames && avi->mp3rate && scale && rate && avgAudChunkSize)
        {
#ifdef SHEEN_VC_DEBUG
            __int64 tmp;
            tmp=(__int64)avi->video_frames*(__int64)(avi->mp3rate>>3)*(__int64)scale/((__int64)rate*(__int64)avgAudChunkSize);
            avi->audio_chunks =(uint32)tmp;
#else
            long long tmp;
            tmp=(long long)avi->video_frames*(long long)(avi->mp3rate>>3)*(long long)scale/((long long)rate*(long long)avgAudChunkSize);
            avi->audio_chunks =(uint32)tmp;
#endif
        }
        else
        {
            diag_printf("[AVI]non idx1, get audio_chunks err! ");
            diag_printf("[AVI]avi->video_frames=%d avi->mp3rate=%d scale=%d rate=%d avgAudChunkSize=%d",
                        avi->video_frames , avi->mp3rate , scale ,  rate , avgAudChunkSize);
            ERR_EXIT(AVI_ERR_NO_IDX)
        }
    }

    diag_printf( "[ALG_AVI] avi_parsing  video_frames = %d, audio_chunks = %d \n",avi->video_frames,avi->audio_chunks);
    if(avi->video_frames==0) ERR_EXIT(AVI_ERR_NO_VIDS)
        //avi->video_index = (video_index_entry *) mmc_MemMalloc((int32)(nvi*sizeof(video_index_entry)));
        avi->video_index = (video_index_entry *) mmc_MemMalloc((int32)(INDEX_NUM*sizeof(video_index_entry)));//for part index read. shenh
    if(avi->video_index==0) ERR_EXIT(AVI_ERR_NO_MEM)
        memset(avi->video_index, 0, INDEX_NUM*sizeof(video_index_entry));

    if(avi->audio_chunks==0) ERR_EXIT(AVI_ERR_NO_VIDS)
        if(avi->audio_chunks)
        {
            //avi->audio_index = (audio_index_entry *) mmc_MemMalloc((int32)(nai*sizeof(audio_index_entry)));
            avi->audio_index = (audio_index_entry *) mmc_MemMalloc((int32)(INDEX_NUM*sizeof(audio_index_entry)));//for part index read. shenh
            if(avi->audio_index==0) ERR_EXIT(AVI_ERR_NO_MEM)
                memset(avi->audio_index, 0, INDEX_NUM*sizeof(audio_index_entry));
        }

    //make sure idx offset("movi" or file start)
    //idx_type = 0;
    if(idx_type==3)
    {
        //indx base ext_indx_bsofst_vid and ext_indx_bsofst_aud
    }
    else if(idx_type!=-1)
    {
        int32 pos, len;
        uint8 idxType[4];

        //get first index from idx1
        //if(avi_seek(avi->fdes, index_pos, FS_SEEK_SET)!=index_pos)ERR_EXIT(AVI_ERR_READ);
        if(avi_fs_seek(avi, index_pos, FS_SEEK_SET)!=index_pos)ERR_EXIT(AVI_ERR_READ);
        //if (avi_read(avi->fdes, data, 16) <= 0) ERR_EXIT(AVI_ERR_READ);
        if (avi_fs_read(avi, data, 16) <= 0) ERR_EXIT(AVI_ERR_READ);
        memcpy(idxType, data, 4);
        pos = str2ulong(data+ 8);
        len = str2ulong(data+12);

        //seek data with up index and compare them
        //if(avi_seek(avi->fdes, pos, FS_SEEK_SET)!=pos)ERR_EXIT(AVI_ERR_READ);
        if(avi_fs_seek(avi, pos, FS_SEEK_SET)!=pos)ERR_EXIT(AVI_ERR_READ);
        //if (avi_read(avi->fdes, data, 8) <= 0) ERR_EXIT(AVI_ERR_READ);
        if (avi_fs_read(avi, data, 8) <= 0) ERR_EXIT(AVI_ERR_READ);
        if( strncasecmp(data,idxType,4)==0 && str2ulong(data+4)==len )
        {
            idx_type = 1; /* Index from start of file */
        }
        else
        {
            //if(avi_seek(avi->fdes, avi->movi_start, FS_SEEK_SET)!=avi->movi_start)ERR_EXIT(AVI_ERR_READ);
            if(avi_fs_seek(avi, avi->movi_start, FS_SEEK_SET)!=avi->movi_start)ERR_EXIT(AVI_ERR_READ);
            //if (avi_read(avi->fdes, data, 8) <= 0) ERR_EXIT(AVI_ERR_READ);
            if (avi_fs_read(avi, data, 8) <= 0) ERR_EXIT(AVI_ERR_READ);
            if( strncasecmp(data,idxType,4)==0 && str2ulong(data+4)==len )
            {
                idx_type = 2; /* Index from start of movi list */
            }
            else
            {
                diag_printf("[AVI] idx1 index err!");
                ERR_EXIT(AVI_ERR_NO_IDX)
            }
        }
    }

    nvi = 0;
    nai = 0;
    tot = 0;
    ioff=0;
    if(idx_type==3)
    {
        /*********************
        for part index read. shenh
        **********************/
        v_countInBuf=0;
        a_countInBuf=0;

        v_lastIndexNum=0;
        a_lastIndexNum=0;

        v_lastPosInFile=0;
        a_lastPosInFile=0;

        v_lastLen=0;
        a_lastLen=0;

        a_lastTot=0;

        v_scale=0;
        a_scale=0;

        if(ext_indx_num_vid<=INDEX_NUM)
            v_scale=1;
        else
            v_scale=(ext_indx_num_vid/INDEX_NUM)+1;

        if(ext_indx_num_aud<=INDEX_NUM)
            a_scale=1;
        else
            a_scale=(ext_indx_num_aud/INDEX_NUM)+1;


        /************************/
        //read video indx
        avi_fs_seek(avi, ext_indx_pos_vid, FS_SEEK_SET);
        offset=512;//in data
        for(i=0; i<ext_indx_num_vid; i++)
        {

            if ( offset >= 512)
            {
                memset(data, 0, 512);
                avi_fs_read(avi, data, 512);
                offset=0;
            }

            if(v_countInBuf<INDEX_NUM)
            {
                if(v_scale==1)
                {
                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].key= ((((data[offset+ 7])&0x80)==0)?0x10:0);// size 31bit set key
                    avi->video_index[v_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_vid;
                    avi->video_index[v_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                    v_countInBuf++;

                }
                else if(((uint32)*((uint32*)avi->compressor2)==AVI_FORMAT_MJPG) && ((nvi % v_scale)==0))
                {
                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].key= ((((data[offset+ 7])&0x80)==0)?0x10:0);// size 31bit set key
                    avi->video_index[v_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_vid;
                    avi->video_index[v_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                    v_countInBuf++;

                }
                else if((uint32)*((uint32*)avi->compressor2)!=AVI_FORMAT_MJPG)//only recode key frame index for mpeg4
                {
                    if(((data[offset+ 7])&0x80)==0)//record the last key index
                    {
                        avi->video_index[v_countInBuf].indexNum=nvi;
                        avi->video_index[v_countInBuf].key= 0x10;
                        avi->video_index[v_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_vid;
                        avi->video_index[v_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                    }

                    if((nvi % v_scale)==0)
                    {
                        if(avi->video_index[v_countInBuf].indexNum==0)//record if no key index before
                        {
                            avi->video_index[v_countInBuf].indexNum=nvi;
                            avi->video_index[v_countInBuf].key= ((((data[offset+ 7])&0x80)==0)?0x10:0);// size 31bit set key
                            avi->video_index[v_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_vid;
                            avi->video_index[v_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                        }
                        v_countInBuf++;
                    }
                }
            }

            //diag_printf("%d %d %d",v_countInBuf-1, avi->video_index[v_countInBuf-1].pos,avi->video_index[v_countInBuf-1].len);

            nvi++;
            offset+=8;

        }

        //read audio indx
        avi_fs_seek(avi, ext_indx_pos_aud, FS_SEEK_SET);
        offset=512;//in data
        for(i=0; i<ext_indx_num_aud; i++ )
        {
            if ( offset >= 512)
            {
                memset(data, 0, 512);
                avi_fs_read(avi, data, 512);
                offset=0;
            }

            if(a_countInBuf<INDEX_NUM)
            {
                if(a_scale==1)
                {
                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_aud;
                    avi->audio_index[a_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;

                }
                else if((nai % a_scale)==0)
                {
                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong(data+offset)+ext_indx_bsofst_aud;
                    avi->audio_index[a_countInBuf].len = (str2ulong(data+offset+4)&0x7fffffff);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;

                }
            }
            tot += (str2ulong(data+offset+4)&0x7fffffff);

            nai++;
            offset+=8;
        }


        avi->audio_bytes = tot;

        diag_printf("[AVI] a_countInBuf=%d v_countInBuf=%d",a_countInBuf, v_countInBuf);

        /* Reposition the file */

        //avi_seek(avi->fdes,avi->movi_start,FS_SEEK_SET);
        avi_fs_seek(avi,avi->movi_start,FS_SEEK_SET);
        avi->video_pos = 0;
    }
    else if(idx_type!=-1)
    {
        //ioff = idx_type == 1 ? 8 : (uint32)(avi->movi_start+4);
        ioff =(( idx_type == 1 )? 8 : (uint32)(avi->movi_start+4));
        offset = index_pos & 0x1ff;
        //avi_seek(avi->fdes,(index_pos >> 9) << 9, FS_SEEK_SET);
        avi_fs_seek(avi,(index_pos >> 9) << 9, FS_SEEK_SET);
        //if (avi_read(avi->fdes, data, 512) <= 0) ERR_EXIT(AVI_ERR_READ);
        if (avi_fs_read(avi, data, 512) <= 0) ERR_EXIT(AVI_ERR_READ);
        p_idx =(P_IDX_TYPE)(data + offset);

        /*********************
        for part index read. shenh
        **********************/
        v_countInBuf=0;
        a_countInBuf=0;

        v_lastIndexNum=0;
        a_lastIndexNum=0;

        v_lastPosInFile=0;
        a_lastPosInFile=0;

        v_lastLen=0;
        a_lastLen=0;

        a_lastTot=0;

        v_scale=0;
        a_scale=0;

        if(avi->video_frames<=INDEX_NUM)
            v_scale=1;
        else
            v_scale=(avi->video_frames/INDEX_NUM)+1;

        if(avi->audio_chunks<=INDEX_NUM)
            a_scale=1;
        else
            a_scale=(avi->audio_chunks/INDEX_NUM)+1;


        /************************/
#if 0
        //follows are base avi->video_frames ==avi->audio_chunks && v_scale==a_scale
        offset_val=(v_scale+a_scale)*16;
        for(i=0; i<avi->n_idx; i+=(v_scale+a_scale), p_idx +=(v_scale+a_scale), offset += offset_val)
        {
            if ( offset + 32 < 512)
            {
                if(strncasecmp(p_idx[0],avi->video_tag,3) == 0)
                {
                    //avi->video_index[nvi].key = str2ulong((uint8*)avi->idx[i]+ 4);

                    //avi->video_index[nvi].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;//sehnh
                    // avi->video_index[nvi].len = str2ulong((uint8*)p_idx[0]+12);//shenh
                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                    v_countInBuf++;

                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[1]+ 8)+ioff;
                    tot +=avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[1]+12);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;

                    nvi+=v_scale;
                    nai+=a_scale;
                    //tot += str2ulong((uint8*)p_idx[1]+12);
                }
                else if(strncasecmp(p_idx[0],avi->audio_tag,4) == 0)
                {
                    /*shenh
                    avi->audio_index[nai].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->audio_index[nai].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[nai].tot = tot;
                    tot += avi->audio_index[nai].len;
                    */
                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    tot +=avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;

                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[1]+ 8)+ioff;
                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[1]+12);
                    v_countInBuf++;

                    nvi+=v_scale;
                    nai+=a_scale;
                }
            }
            else
            {
                cpylen =  512 - offset;
                if(cpylen>0)
                    memcpy(data_temp, p_idx[0], cpylen);

                if (avi_read(avi->fdes, data, 512) <= 0) ERR_EXIT(AVI_ERR_READ);

                if(cpylen>0)
                {
                    memcpy(data_temp + cpylen, data, 32 - cpylen);
                    p_idx = data_temp;
                }
                else
                    p_idx=data-cpylen;


                if(strncasecmp(p_idx[0],avi->video_tag,3) == 0)
                {
                    //avi->video_index[nvi].key = str2ulong((uint8*)avi->idx[i]+ 4);
                    //diag_printf("avi->video_index[%d].key = %d",nai,avi->video_index[nvi].key);

                    //avi->video_index[nvi].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;//shenh
                    //avi->video_index[nvi].len = str2ulong((uint8*)p_idx[0]+12);//shenh
                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                    v_countInBuf++;

                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[1]+ 8)+ioff;
                    tot +=avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[1]+12);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;
                    nvi+=v_scale;
                    nai+=a_scale;
                }
                else if(strncasecmp(p_idx[0],avi->audio_tag,4) == 0)
                {
                    /*shenh
                    avi->audio_index[nai].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->audio_index[nai].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[nai].tot = tot;
                    tot += avi->audio_index[nai].len;
                    */
                    avi->audio_index[a_countInBuf].indexNum=nai;
                    avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    tot +=avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[a_countInBuf].tot = tot;
                    a_countInBuf++;

                    avi->video_index[v_countInBuf].indexNum=nvi;
                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[1]+ 8)+ioff;
                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[1]+12);
                    v_countInBuf++;

                    nvi+=v_scale;
                    nai+=a_scale;
                }

                p_idx =data - cpylen;
                offset = - cpylen;

            }
        }
#else

        for(i=0; i<avi->n_idx; i++, p_idx ++, offset += 16)
        {

            if ( offset + 16 < 512)
            {
                if(strncasecmp(((uint8*)p_idx[0])+2,avi->video_tag+2,2) == 0 ||strncasecmp(((uint8*)p_idx[0])+2,"dc",2) == 0)
                {
                    //avi->video_index[nvi].key = str2ulong((uint8*)avi->idx[i]+ 4);

                    //avi->video_index[nvi].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;//sehnh
                    // avi->video_index[nvi].len = str2ulong((uint8*)p_idx[0]+12);//shenh


                    if(v_countInBuf<INDEX_NUM)
                    {
                        if(v_scale==1)
                        {
                            avi->video_index[v_countInBuf].indexNum=nvi;
                            avi->video_index[v_countInBuf].key= str2ulong((uint8*)p_idx[0]+ 4);
                            avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            v_countInBuf++;

                        }
                        else if(((uint32)*((uint32*)avi->compressor2)==AVI_FORMAT_MJPG) && ((nvi % v_scale)==0))
                        {
                            avi->video_index[v_countInBuf].indexNum=nvi;
                            avi->video_index[v_countInBuf].key=str2ulong((uint8*)p_idx[0]+ 4);
                            avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            v_countInBuf++;

                        }
                        else if((uint32)*((uint32*)avi->compressor2)!=AVI_FORMAT_MJPG)//only recode key frame index for mpeg4
                        {
                            if(str2ulong((uint8*)p_idx[0]+ 4)==0x10)//record the last key index
                            {
                                avi->video_index[v_countInBuf].indexNum=nvi;
                                avi->video_index[v_countInBuf].key=str2ulong((uint8*)p_idx[0]+ 4);
                                avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                                avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            }

                            if((nvi % v_scale)==0)
                            {
                                if(avi->video_index[v_countInBuf].indexNum==0)//record if no key index before
                                {
                                    avi->video_index[v_countInBuf].indexNum=nvi;
                                    avi->video_index[v_countInBuf].key=str2ulong((uint8*)p_idx[0]+ 4);
                                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                                }
                                v_countInBuf++;
                            }
                        }
                    }
                    nvi++;
                }
                else if(strncasecmp(((uint8*)p_idx[0])+2,avi->audio_tag+2,2) == 0)
                {
                    /*shenh
                    avi->audio_index[nai].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->audio_index[nai].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[nai].tot = tot;
                    tot += avi->audio_index[nai].len;
                    */
                    if(a_countInBuf<INDEX_NUM)
                    {
                        if(a_scale==1)
                        {
                            avi->audio_index[a_countInBuf].indexNum=nai;
                            avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            avi->audio_index[a_countInBuf].tot = tot;
                            a_countInBuf++;

                        }
                        else if((nai % a_scale)==0)
                        {
                            avi->audio_index[a_countInBuf].indexNum=nai;
                            avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            avi->audio_index[a_countInBuf].tot = tot;
                            a_countInBuf++;

                        }
                    }
                    tot += str2ulong((uint8*)p_idx[0]+12);

                    nai++;
                }
            }
            else
            {
                cpylen =  512 - offset;
                memcpy(data_temp, p_idx[0], cpylen);
                //if (avi_read(avi->fdes, data, 512) <= 0) ERR_EXIT(AVI_ERR_READ);
                if (avi_fs_read(avi, data, 512) <= 0) ERR_EXIT(AVI_ERR_READ);

                memcpy(data_temp + cpylen, data, 16 - cpylen);
                p_idx = (P_IDX_TYPE)data_temp;
                if(strncasecmp(((uint8*)p_idx[0])+2,avi->video_tag+2,2) == 0 ||strncasecmp(((uint8*)p_idx[0])+2,"dc",2) == 0)
                {
                    //avi->video_index[nvi].key = str2ulong((uint8*)avi->idx[i]+ 4);
                    //diag_printf("avi->video_index[%d].key = %d",nai,avi->video_index[nvi].key);

                    //avi->video_index[nvi].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;//shenh
                    //avi->video_index[nvi].len = str2ulong((uint8*)p_idx[0]+12);//shenh

                    if(v_countInBuf<INDEX_NUM)
                    {
                        if(v_scale==1)
                        {
                            avi->video_index[v_countInBuf].indexNum=nvi;
                            avi->video_index[v_countInBuf].key= str2ulong((uint8*)p_idx[0]+ 4);
                            avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            v_countInBuf++;

                        }
                        else if(((uint32)*((uint32*)avi->compressor2)==AVI_FORMAT_MJPG) && ((nvi % v_scale)==0))
                        {
                            avi->video_index[v_countInBuf].indexNum=nvi;
                            avi->video_index[v_countInBuf].key= str2ulong((uint8*)p_idx[0]+ 4);
                            avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            v_countInBuf++;

                        }
                        else if((uint32)*((uint32*)avi->compressor2)!=AVI_FORMAT_MJPG)//only recode key frame index for mpeg4
                        {
                            if(str2ulong((uint8*)p_idx[0]+ 4)==0x10)//record the last key index
                            {
                                avi->video_index[v_countInBuf].indexNum=nvi;
                                avi->video_index[v_countInBuf].key=str2ulong((uint8*)p_idx[0]+ 4);
                                avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                                avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            }

                            if((nvi % v_scale)==0)
                            {
                                if(avi->video_index[v_countInBuf].indexNum==0)//record if no key index before
                                {
                                    avi->video_index[v_countInBuf].indexNum=nvi;
                                    avi->video_index[v_countInBuf].key=str2ulong((uint8*)p_idx[0]+ 4);
                                    avi->video_index[v_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                                    avi->video_index[v_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                                }
                                v_countInBuf++;
                            }
                        }
                    }

                    nvi++;
                }
                else if(strncasecmp(((uint8*)p_idx[0])+2,avi->audio_tag+2,2) == 0)
                {
                    /*shenh
                    avi->audio_index[nai].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                    avi->audio_index[nai].len = str2ulong((uint8*)p_idx[0]+12);
                    avi->audio_index[nai].tot = tot;
                    tot += avi->audio_index[nai].len;
                    */

                    if(a_countInBuf<INDEX_NUM)
                    {
                        if(a_scale==1)
                        {
                            avi->audio_index[a_countInBuf].indexNum=nai;
                            avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            avi->audio_index[a_countInBuf].tot = tot;
                            a_countInBuf++;

                        }
                        else if((nai % a_scale)==0)
                        {
                            avi->audio_index[a_countInBuf].indexNum=nai;
                            avi->audio_index[a_countInBuf].pos = str2ulong((uint8*)p_idx[0]+ 8)+ioff;
                            avi->audio_index[a_countInBuf].len = str2ulong((uint8*)p_idx[0]+12);
                            avi->audio_index[a_countInBuf].tot = tot;
                            a_countInBuf++;

                        }
                    }
                    tot += str2ulong((uint8*)p_idx[0]+12);

                    nai++;
                }
                p_idx =(P_IDX_TYPE)(data - cpylen);
                offset = - cpylen;

            }
        }
#endif
        avi->audio_bytes = tot;

        diag_printf("[AVI] a_countInBuf=%d v_countInBuf=%d",a_countInBuf, v_countInBuf);

        /* Reposition the file */

        //avi_seek(avi->fdes,avi->movi_start,FS_SEEK_SET);
        avi_fs_seek(avi,avi->movi_start,FS_SEEK_SET);
        avi->video_pos = 0;
    }
    else
    {
        v_countInBuf=0;
        a_countInBuf=0;

        v_lastIndexNum=0;
        a_lastIndexNum=0;

        v_lastPosInFile=0;
        a_lastPosInFile=0;

        v_lastLen=0;
        a_lastLen=0;

        a_lastTot=0;

        v_scale=0;
        a_scale=0;

        if(avi->video_frames<=INDEX_NUM)
            v_scale=1;
        else
            v_scale=(avi->video_frames/INDEX_NUM)+1;

        if(avi->audio_chunks<=INDEX_NUM)
            a_scale=2;
        else
            a_scale=(avi->audio_chunks/INDEX_NUM)+1;

        //if((ioff=avi_seek(avi->fdes, avi->movi_start, FS_SEEK_SET))!=avi->movi_start)ERR_EXIT(AVI_ERR_NO_IDX);
        if((ioff=avi_fs_seek(avi, avi->movi_start, FS_SEEK_SET))!=avi->movi_start)ERR_EXIT(AVI_ERR_NO_IDX);
        junk_count=0;
        while(1)
        {
            //if( avi_read(avi->fdes,data,8) != 8 )
            if( avi_fs_read(avi,data,8) != 8 )
            {
                //if(FS_IsEndOfFile(avi->fdes)==1)
                if(avi_fs_isEndOfFile(avi)==1)
                {
                    diag_printf( "[AVI] file end!");
                    break;
                }
                else
                    ERR_EXIT(AVI_ERR_NO_IDX);
            }
            n = str2ulong((uint8*)data+4);
            ioff+=8;

            /* The movi list may contain sub-lists, ignore them */
            if(strncasecmp(data,"LIST",4)==0)
            {
                diag_printf( "[AVI] movi sub LIST!");
                ioff+=4;
                //if(avi_seek(avi->fdes,4,FS_SEEK_CUR)!=ioff)
                if(avi_fs_seek(avi,4,FS_SEEK_CUR)!=ioff)
                {
                    //if(FS_IsEndOfFile(avi->fdes)==1)
                    if(avi_fs_isEndOfFile(avi)==1)
                    {
                        diag_printf( "[AVI] file end!");
                        break;
                    }
                    else
                        ERR_EXIT(AVI_ERR_NO_IDX);
                }
                continue;
            }

            /* Check if we got a tag ##db, ##dc or ##wb */
            if(strncasecmp(data+2,avi->video_tag+2,2) == 0 ||strncasecmp(data+2,"dc",2) == 0)
            {
                junk_count=0;

                if(v_scale==1)
                {
                    if(v_countInBuf<INDEX_NUM)
                    {
                        avi->video_index[v_countInBuf].indexNum=nvi;
                        avi->video_index[v_countInBuf].pos = ioff;
                        avi->video_index[v_countInBuf].len = n;
                    }
                    v_countInBuf++;

                }
                else if((nvi % v_scale)==0)
                {
                    if(v_countInBuf<INDEX_NUM)
                    {
                        avi->video_index[v_countInBuf].indexNum=nvi;
                        avi->video_index[v_countInBuf].pos = ioff;
                        avi->video_index[v_countInBuf].len = n;
                    }
                    v_countInBuf++;

                }
                n=PAD_EVEN(n);
                nvi++;
            }
            else if(strncasecmp(data+2,avi->audio_tag+2,2) == 0)
            {
                junk_count=0;

                if(a_scale==1)
                {
                    if(a_countInBuf<INDEX_NUM)
                    {
                        avi->audio_index[a_countInBuf].indexNum=nai;
                        avi->audio_index[a_countInBuf].pos = ioff;
                        avi->audio_index[a_countInBuf].len = n;
                        avi->audio_index[a_countInBuf].tot = tot;
                    }
                    a_countInBuf++;

                }
                else if((nai % a_scale)==0)
                {
                    if(a_countInBuf<INDEX_NUM)
                    {
                        avi->audio_index[a_countInBuf].indexNum=nai;
                        avi->audio_index[a_countInBuf].pos =ioff;
                        avi->audio_index[a_countInBuf].len = n;
                        avi->audio_index[a_countInBuf].tot = tot;
                    }
                    a_countInBuf++;

                }
                tot += n;
                n=PAD_EVEN(n);
                nai++;
            }
            else if(strncasecmp(data+1,avi->video_tag+2,2) == 0 ||strncasecmp(data+1,"dc",2) == 0)
            {
                n = str2ulong((uint8*)data+3);
                junk_count=0;

                if(v_scale==1)
                {
                    if(v_countInBuf<INDEX_NUM)
                    {
                        avi->video_index[v_countInBuf].indexNum=nvi;
                        avi->video_index[v_countInBuf].pos = ioff-1;
                        avi->video_index[v_countInBuf].len = n;
                    }
                    v_countInBuf++;

                }
                else if((nvi % v_scale)==0)
                {
                    if(v_countInBuf<INDEX_NUM)
                    {
                        avi->video_index[v_countInBuf].indexNum=nvi;
                        avi->video_index[v_countInBuf].pos = ioff-1;
                        avi->video_index[v_countInBuf].len = n;
                    }
                    v_countInBuf++;

                }
                n=PAD_EVEN(n);
                n--;
                nvi++;
            }
            else if(strncasecmp(data+1,avi->audio_tag+2,2) == 0)
            {
                n = str2ulong((uint8*)data+3);
                junk_count=0;

                if(a_scale==1)
                {
                    if(a_countInBuf<INDEX_NUM)
                    {
                        avi->audio_index[a_countInBuf].indexNum=nai;
                        avi->audio_index[a_countInBuf].pos = ioff-1;
                        avi->audio_index[a_countInBuf].len = n;
                        avi->audio_index[a_countInBuf].tot = tot;
                    }
                    a_countInBuf++;

                }
                else if((nai % a_scale)==0)
                {
                    if(a_countInBuf<INDEX_NUM)
                    {
                        avi->audio_index[a_countInBuf].indexNum=nai;
                        avi->audio_index[a_countInBuf].pos =ioff-1;
                        avi->audio_index[a_countInBuf].len = n;
                        avi->audio_index[a_countInBuf].tot = tot;
                    }
                    a_countInBuf++;

                }
                n=PAD_EVEN(n);
                n--;
                tot += n;

                nai++;
            }
            else
            {
                diag_printf( "[AVI] err tag=%x", *(uint32*)data);
                junk_count++;
                if(junk_count>MAX_JUNK_COUNT)
                {
                    diag_printf( "[AVI] get index err!");
                    break;
                }
            }

            //avi_seek(avi->fdes, n, FS_SEEK_CUR);
            avi_fs_seek(avi, n, FS_SEEK_CUR);
            ioff+=n;
        }

        avi->video_frames=nvi;
        avi->audio_chunks=nai;

        avi->audio_bytes = tot;

        /* Reposition the file */

        //avi_seek(avi->fdes,avi->movi_start,FS_SEEK_SET);
        avi_fs_seek(avi,avi->movi_start,FS_SEEK_SET);
        avi->video_pos = 0;
    }


    /* free idx test */
    /*
       if(avi->idx)
       {
          free(avi->idx);
          avi->idx=NULL;
       }
    */
    return(avi->video_frames);
}

/*************************************************************************
* FUNCTION
*   avi_video_frames
*
* DESCRIPTION
*   Get the total frame number of the given AVI file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   uint32:    total frame number
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32 avi_video_frames(avi_struct *avi)
{
    return avi->video_frames;
}


/*************************************************************************
* FUNCTION
*   avi_video_width
*
* DESCRIPTION
*   Get the frame width of the given AVI file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   uint32:    the frame width
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32  avi_video_width(avi_struct *avi)
{
    return avi->width;
}

/*************************************************************************
* FUNCTION
*   avi_video_height
*
* DESCRIPTION
*   Get the frame height of the given AVI file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   uint32:    the frame height
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32  avi_video_height(avi_struct *avi)
{
    return avi->height;
}


/*************************************************************************
* FUNCTION
*   avi_frame_rate
*
* DESCRIPTION
*   Get the frame rate of the given AVI file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   double:    the frame rate
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32 avi_frame_rate(avi_struct *avi)
{
    return avi->fps;
}


/*************************************************************************
* FUNCTION
*   avi_video_compressor
*
* DESCRIPTION
*   Get the compressorof the given AVI file.
*
* PARAMETERS
*   avi_struct *avi
*
* RETURNS
*   int8*
*
* GLOBALS AFFECTED
*
*************************************************************************/
int8* avi_video_compressor(avi_struct *avi)
{
    return avi->compressor2;
}


/*************************************************************************
* FUNCTION
*   avi_frame_size
*
* DESCRIPTION
*   Get the frame data size of the given frame.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 frame: the given frame number
*
* RETURNS
*   uint32:    size
*
* GLOBALS AFFECTED
*
*****************************1********************************************/
uint32 avi_frame_size(avi_struct *avi, uint32 frame)
{
    int32 res;
    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->video_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(frame >= avi->video_frames) return 0;
    //return(avi->video_index[frame].len);//shenh
    if((res=avi_part_index_read(avi, frame, V_TYPE, VAL_LEN))==-1)
        return 0;

    return res;
}

/*************************************************************************
* FUNCTION
*   avi_audio_size
*
* DESCRIPTION
*   Get the audio data size of the given frame.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 frame: the given frame number
*
* RETURNS
*   uint32:    size
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32 avi_audio_size(avi_struct *avi, uint32 frame)//no use now shenh
{
    int32 res;
    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->audio_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(frame >= avi->audio_chunks) return 0;
// return(avi->audio_index[frame].len);shenh
    if((res=avi_part_index_read(avi, frame, A_TYPE, VAL_LEN))==-1)
        return 0;
    return res;
}

/*************************************************************************
* FUNCTION
*   avi_get_video_position
*
* DESCRIPTION
*   Get the video data position with the given frame number.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 frame: the given frame number
*
* RETURNS
*   int32:    position
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32 avi_get_video_position(avi_struct *avi, uint32 frame)//no use now shenh
{
    int32 res;
    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->video_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(frame >= avi->video_frames) return 0;
    // return(avi->video_index[frame].pos);shenh
    if((res=avi_part_index_read(avi, frame, V_TYPE, VAL_POS))==-1)
        return 0;

    return res;
}

//get neighbor key frame num for mpeg4. add sheen
uint32 avi_get_video_keyframe_num(avi_struct *avi, uint32 frame)
{
    int32 res;
    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->video_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(frame >= avi->video_frames) return 0;
    // return(avi->video_index[frame].pos);shenh
    if((res=avi_part_index_read(avi, frame, V_TYPE, VAL_KEY))==-1)
        return 0;

    return res;
}


/*************************************************************************
* FUNCTION
*   avi_set_video_position
*
* DESCRIPTION
*   Set the video frame position.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 frame: the given frame number
*  kal_bool seek: Need to seek to the given frame or not?
* RETURNS
*   int32:    0: No error
*
* GLOBALS AFFECTED
*
****************************1*********************************************/
int32 avi_set_video_position(avi_struct *avi, uint32 frame)
{
    // uint32 pos;

    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->video_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    avi->video_pos = frame;
    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_set_audio_bitrate
*
* DESCRIPTION
*   Set the audio bitrate.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 frame: the given bitrate
*
* RETURNS
*   int32:    0: No error
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_set_audio_bitrate(avi_struct *avi, uint32 bitrate)
{
    if(avi->mode==AVI_MODE_READ) { avi_errno = AVI_ERR_NOT_PERM; return -1; }

    avi->mp3rate = bitrate;
    return 0;
}

/*************************************************************************
* FUNCTION
*   avi_read_frame
*
* DESCRIPTION
*   Read the video frame data from the AVI file to the given buffer.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *vidbuf: the given buffer
*  int32 *keyframe
* RETURNS
*   int32:    0: No error
*
* GLOBALS AFFECTED
*
****************************1*********************************************/
int32  avi_read_frame(avi_struct *avi, int8 *vidbuf, uint32 bufSize, int32 *keyframe)
{
    int32 n;
    uint32 pos;
    int32 res;

    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->video_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(avi->video_pos >= avi->video_frames)
    {
        diag_printf("[AVI] video video_pos=%d >= video_frames=%d", avi->video_pos , avi->video_frames);
        return -1;
    }
    // n = avi->video_index[avi->video_pos].len;shenh
    if((res=avi_part_index_read(avi, avi->video_pos, V_TYPE, VAL_LEN))==-1)
    {
        diag_printf("[AVI] video len index err");
        return -1;
    }
    n=res;

    if(n>bufSize)
    {
        diag_printf("[AVI] video size=%d bigger than buffer size=%d", n, bufSize);
        n=bufSize;
        //return -1;
    }

    *keyframe = 0; //(avi->video_index[avi->video_pos].key==0x10) ? 1:0;

    // pos=avi->video_index[avi->video_pos].pos;shenh
    if((res=avi_part_index_read(avi, avi->video_pos, V_TYPE, VAL_POS))==-1)
    {
        diag_printf("[AVI] video pos index err");
        return -1;
    }
    pos=res;

    //avi_seek(avi->fdes, pos, FS_SEEK_SET);
    avi_fs_seek(avi, pos, FS_SEEK_SET);
    //if (avi_read((int32)avi->fdes,(uint8*)vidbuf,(int32)n) != n)
    if ((res=avi_fs_read(avi,(uint8*)vidbuf,(int32)n) )!= n)
    {
        avi_errno = AVI_ERR_READ;
        diag_printf("[AVI] video read err, pos=%d size=%d, res=%d",pos, n, res);
        return -1;
    }

    avi->video_pos++;

    return n;
}

/*************************************************************************
* FUNCTION
*   avi_set_audio_position
*
* DESCRIPTION
*   Set the audio position.
*
* PARAMETERS
*   avi_struct *avi
*  uint32 byte
*
* RETURNS
*   int32:    0: No error
*
* GLOBALS AFFECTED
*
**********************1***************************************************/
int32 avi_set_audio_position(avi_struct *avi, uint32 byteNum)//only this function use the audio index "tot" .shenh
{
    uint32 n0, n1, n;
    int32 res;
    // uint32 tot_temp;

    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->audio_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    /* Binary search in the audio chunks */

    n0 = 0;
    n1 = avi->audio_chunks;

    while(n0<n1-1)
    {
        n = (n0+n1)/2;
        /*shenh
        if(avi->audio_index[n].tot>byteNum)
           n1 = n;
        else
           n0 = n;
           */

        if((res=avi_part_index_read(avi, n, A_TYPE, VAL_TOT))==-1)
            return -1;

        if(res>byteNum)
            n1 = n;
        else
        {
            n0 = n;
            //tot_temp=res;
        }
    }

    avi->audio_posc = n0;
    // avi->audio_posb = byteNum - avi->audio_index[n0].tot;shenh
    if((res=avi_part_index_read(avi, n0, A_TYPE, VAL_TOT))==-1)
        return -1;
    avi->audio_posb = byteNum - res;

    return 0;
}

//set audio position from chunk number.add shenh
int32 avi_set_audio_position2(avi_struct *avi, uint32 chunkNum)
{
    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->audio_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    if(chunkNum < avi->audio_chunks)
        avi->audio_posc = chunkNum;
    else
        avi->audio_posc = 0;

    avi->audio_posb = 0;

    return 0;
}
/*************************************************************************
* FUNCTION
*   avi_read_audio
*
* DESCRIPTION
*   Read the audio data from the AVI file to the given buffer.
*
* PARAMETERS
*   avi_struct *avi
*  int8 *audbuf: the given buffer
*  int32 bytes
* RETURNS
*   int32:    0: No error
*
* GLOBALS AFFECTED
*
***************************1**********************************************/
int32 avi_read_audio(avi_struct *avi, int8 *audbuf, int32 bytes)
{
    int32 nr, pos, left, todo;
    int32 res;

    if(avi->mode==AVI_MODE_WRITE) { avi_errno = AVI_ERR_NOT_PERM; return -1; }
    if(!avi->audio_index)         { avi_errno = AVI_ERR_NO_IDX;   return -1; }

    nr = 0; /* total number of bytes read */

    while(bytes>0)
    {
        // left = avi->audio_index[avi->audio_posc].len - avi->audio_posb;shenh

        if((res=avi_part_index_read(avi, avi->audio_posc, A_TYPE, VAL_LEN))==-1)
            return -1;
        left = res - avi->audio_posb;

        if(left<=0)
        {
            if(avi->audio_posc>=avi->audio_chunks-1) return nr;
            avi->audio_posc++;
            avi->audio_posb = 0;
            continue;
        }
        if(bytes<left)
            todo = bytes;
        else
            todo = left;

        //  pos = avi->audio_index[avi->audio_posc].pos + avi->audio_posb;shenh
        if((res=avi_part_index_read(avi, avi->audio_posc, A_TYPE, VAL_POS))==-1)
            return -1;
        pos = res + avi->audio_posb;

#ifdef __NO_LIMIT_AVI__
        //avi_seek(avi->au_fdes, pos, FS_SEEK_SET);
        avi_fs_seek(avi, pos, FS_SEEK_SET);
        //if (avi_read(avi->au_fdes,audbuf+nr,todo) != todo)
        if (avi_fs_read(avi,audbuf+nr,todo) != todo)
#else
        //avi_seek(avi->fdes, pos, FS_SEEK_SET);
        avi_fs_seek(avi, pos, FS_SEEK_SET);
        //if (avi_read(avi->fdes,audbuf+nr,todo) != todo)
        if (avi_fs_read(avi,audbuf+nr,todo) != todo)
#endif
        {
            avi_errno = AVI_ERR_READ;
            return -1;
        }
        bytes -= todo;
        nr    += todo;
        avi->audio_posb += todo;
    }

    return nr;
}

#if 0
/* avi_read_data: Special routine for reading the next audio or video chunk
                  without having an index of the file. */

/*************************************************************************
* FUNCTION
*   avi_read_audio
*
* DESCRIPTION
*   Special routine for reading the next audio or video chunk
*                  without having an index of the file.
*
* PARAMETERS
*
* RETURNS
*   int32:  Return codes:
*
*    1 = video data read
*    2 = audio data read
*    0 = reached EOF
*   -1 = video buffer too small
*   -2 = audio buffer too small
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 avi_read_data(avi_struct *avi, int8 *vidbuf, uint32 max_vidbuf,
                    int8 *audbuf, uint32 max_audbuf,
                    uint32 *len)
{

    uint32 n;
    int8 data[8];

    if(avi->mode==AVI_MODE_WRITE) return 0;

    while(1)
    {
        /* Read tag and length */

        if( avi_read(avi->fdes,data,8) != 8 ) return 0;

        /* if we got a list tag, ignore it */

        if(strncasecmp(data,"LIST",4) == 0)
        {
            avi_seek(avi->fdes,4,FS_SEEK_CUR);
            continue;
        }

        n = PAD_EVEN(str2ulong((uint8*)data+4));

        if(strncasecmp(data,avi->video_tag,3) == 0)
        {
            *len = n;
            avi->video_pos++;
            if(n>max_vidbuf)
            {
                avi_seek(avi->fdes,n,FS_SEEK_CUR);
                return -1;
            }
            if(avi_read(avi->fdes,vidbuf,n) != n ) return 0;
            return 1;
        }
        else if(strncasecmp(data,avi->audio_tag,4) == 0)
        {
            *len = n;
            if(n>max_audbuf)
            {
                avi_seek(avi->fdes,n,FS_SEEK_CUR);
                return -2;
            }
            if(avi_read(avi->fdes,audbuf,n) != n ) return 0;
            return 2;
            break;
        }
        else if(avi_seek(avi->fdes,n,FS_SEEK_CUR)<0)  return 0;
    }
}
#endif

int32 avi_get_error(void)
{
    return avi_errno;
}

#ifdef __NO_LIMIT_AVI__
int32 avi_get_audio_format(uint32 audio_format, uint32 audio_rate)
{
    switch(audio_format)
    {
        case WAVE_FORMAT_UNKNOWN:       //0x0000
            return -1;
            break;

        case WAVE_FORMAT_PCM:       //0x0001
            if(audio_rate==8000)
                return MEDIA_FORMAT_PCM_8K;
            if(audio_rate==16000)
                return MEDIA_FORMAT_PCM_16K;
            break;
    }
    return -1;
}
#endif

#ifdef SHEEN_VC_DEBUG
//test
void main(void)
{
    FILE *pIn, *pOut;
    char *pBuf;
    avi_struct *avi_p;
    int i;
    int key_frame;
    int res;

    pIn=fopen("F:\\测试文件\\avi\\20120101_0026248.avi","rb");
    pOut=fopen("F:\\测试文件\\20120101_0026248.mjpg","wb");
    pBuf=malloc(100*1024);

    avi_p=avi_open_input_file(pIn, 0, 0, 1);

    //avi_get_video_keyframe_num(avi_p,200);

    i=0;
    while(1)
    {
        /*
        if(i==3)
        {
            i=avi_get_video_keyframe_num(avi_p, 266);

        }*/

        //avi_get_video_keyframe_num(avi_p,200);

        avi_set_video_position(avi_p, i);
        avi_frame_size(avi_p, i);
        res=avi_read_frame(avi_p, pBuf, 100*1024,  &key_frame);
        //if(res<4)break;
        //res=avi_read_audio(avi_p, pBuf, 40*1024);
#if 1//mjpg
        fwrite(&res,1,4,pOut);//write frame size
#endif
        fwrite(pBuf,1,res,pOut);

        printf("frame %d\n",i);
        i++;
        if(i>160)break;
    }

    fclose(pOut);
    fclose(pIn);
    free(pBuf);
}
#endif


#endif /* __MED_MJPG_MOD__ */


