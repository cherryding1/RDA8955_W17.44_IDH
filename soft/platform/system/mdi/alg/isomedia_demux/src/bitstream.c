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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "tools.h"
#include "bitstream.h"
#include "isomedia_dev.h"
#ifndef SHEEN_VC_DEBUG
#include "cs_types.h"
#include "fs.h"
#endif
#include "mdi_alg_common.h"

GF_BitStream *bs_glob;//add shenh

enum
{
    GF_BITSTREAM_FILE_READ = GF_BITSTREAM_WRITE + 1,
    GF_BITSTREAM_FILE_WRITE,
    /*private mode if we own the buffer*/
    GF_BITSTREAM_WRITE_DYN
};

/*
INT32 mp4_fs_init(INT32 fd, UINT8 fileMode, UINT32 bufLen)
{
    mp4_fs_mode=fileMode;

    if(mp4_fs_mode==0)
    {
        pMp4_fs_point=0;
        mp4_fs_bufLen=0;
    }
    else
    {
        pMp4_fs_point=(UINT8*)fd;
        mp4_fs_bufLen=bufLen;
        if(!pMp4_fs_point || !mp4_fs_bufLen)
            return -1;
    }

    return 0;
}*/

INT32 mp4_fs_read(GF_BitStream *bs, UINT8 *pBuf, UINT32 iLen)
{
    INT32 res=0;

    if(bs->mp4_fs_mode==0)
    {
        res=FS_Read((INT32)bs->stream, pBuf, iLen);
    }
    else if(bs->mp4_fs_mode==1)
    {
        if(bs->pMp4_fs_point+iLen<=(UINT8*)bs->stream + bs->mp4_fs_bufLen)
        {
            memcpy(pBuf, bs->pMp4_fs_point, iLen);
            bs->pMp4_fs_point+=iLen;
            res=iLen;
        }
        else if(bs->pMp4_fs_point-(UINT8*)bs->stream < bs->mp4_fs_bufLen)
        {
            res=(UINT8*)bs->stream+ bs->mp4_fs_bufLen- bs->pMp4_fs_point;
            memcpy(pBuf, bs->pMp4_fs_point, res);
            bs->pMp4_fs_point+=res;
        }
    }
    else if(bs->mp4_fs_mode==2)
    {
        res=http_fs_read((INT32)bs->stream,pBuf,iLen);
    }

    return res;
}

INT32 mp4_fs_seek(GF_BitStream *bs, INT32 iOffset, UINT8 iOrigin)
{
    INT32 res=-1;

    if(bs->mp4_fs_mode==0)
    {
        res=(INT32)FS_Seek((INT32)bs->stream, (INT64)iOffset, iOrigin);
    }
    else if(bs->mp4_fs_mode==1)
    {
        switch ( iOrigin )
        {
            case FS_SEEK_SET:
                res = 0 + iOffset;
                bs->pMp4_fs_point = res < 0 ? (UINT8*)bs->stream : (res > bs->mp4_fs_bufLen ? ((UINT8*)bs->stream + bs->mp4_fs_bufLen): ((UINT8*)bs->stream + res));
                res = bs->pMp4_fs_point-(UINT8*)bs->stream;
                break;

            case FS_SEEK_CUR:
                res = (INT32)(bs->pMp4_fs_point- (UINT8*)bs->stream) + iOffset;
                bs->pMp4_fs_point = res < 0 ? (UINT8*)bs->stream : (res > bs->mp4_fs_bufLen ? ((UINT8*)bs->stream + bs->mp4_fs_bufLen): ((UINT8*)bs->stream + res));
                res = bs->pMp4_fs_point-(UINT8*)bs->stream;
                break;

            case FS_SEEK_END:
                res = bs->mp4_fs_bufLen + iOffset;
                bs->pMp4_fs_point = res < 0 ? (UINT8*)bs->stream : (res > bs->mp4_fs_bufLen ? ((UINT8*)bs->stream + bs->mp4_fs_bufLen): ((UINT8*)bs->stream + res));
                res = bs->pMp4_fs_point-(UINT8*)bs->stream;
                break;

            default:
                res = -1;
        }

    }
    else if(bs->mp4_fs_mode==2)
    {
        res=http_fs_seek((INT32)bs->stream, iOffset, iOrigin);
    }

    return res;
}

INT32 mp4_fs_getFileSize(GF_BitStream *bs)
{
    INT32 res=0;

    if(bs->mp4_fs_mode==0)
    {
#ifdef SHEEN_VC_DEBUG
        fseek(bs->stream,0,SEEK_END);
        res=ftell(bs->stream);
#else
        res=(UINT32)FS_GetFileSize((INT32)bs->stream);
#endif
    }
    else if(bs->mp4_fs_mode==1)
    {
        res=bs->mp4_fs_bufLen;
    }
    else if(bs->mp4_fs_mode==2)
    {
        res=http_fs_getFileSize((INT32)bs->stream);
    }
    return res;
}

INT32 mp4_fs_isEndOfFile(GF_BitStream *bs)
{
    INT32 res=0;

    if(bs->mp4_fs_mode==0)
    {
        res=FS_IsEndOfFile((INT32)bs->stream);
    }
    else if(bs->mp4_fs_mode==1)
    {
        if(bs->pMp4_fs_point >= (uint8*)bs->stream + bs->mp4_fs_bufLen)
            res=1;
        else
            res=0;
    }
    else if(bs->mp4_fs_mode==2)
    {
        res=http_fs_isEndOfFile((INT32)bs->stream);
    }
    return res;
}


/*
INT32 mp4_fs_close(GF_BitStream *bs)
{
    INT32 res=0;

    if(mp4_fs_mode==0)
    {
        res=FS_Close((INT32)bs->stream);
    }
    else
    {

    }
    return res;
}*/

uint8 BS_ReadByte(GF_BitStream*bs)
{
    uint8 tmp;
    if (bs->bsmode == GF_BITSTREAM_READ)
    {
        if (bs->position == bs->size)
        {
            if (bs->EndOfStream) bs->EndOfStream(bs->par);
            return 0;
        }
        return (uint8) bs->original[bs->position++];
    }
    /* change shenh
    if (!feof(bs->stream)) {
        bs->position++;
        return fgetc(bs->stream);
    }
    */
    //if (!FS_IsEndOfFile((INT32)bs->stream)) {
    if (!mp4_fs_isEndOfFile(bs))
    {
        bs->position++;
        //if(FS_Read((INT32)bs->stream, &tmp, 1)==1)
        if(mp4_fs_read(bs, &tmp, 1)==1)
            return tmp;
        else
            return 0;
    }
    return 0;
}

uint32 gf_bs_read_u8(GF_BitStream *bs)
{
    //assert(bs->nbBits==8);
    return (uint32) BS_ReadByte(bs);
}

uint32 gf_bs_read_u16(GF_BitStream *bs)
{
    uint32 ret;
    //assert(bs->nbBits==8);
    ret = BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs);
    return ret;
}


uint32 gf_bs_read_u24(GF_BitStream *bs)
{
    uint32 ret;
    //assert(bs->nbBits==8);
    ret = BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs);
    return ret;
}

uint32 gf_bs_read_u32(GF_BitStream *bs)
{
    uint32 ret;
    //assert(bs->nbBits==8);
    ret = BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs); ret<<=8;
    ret |= BS_ReadByte(bs);
    return ret;
}

uint32 gf_buf_read_u32(uint8*pBuf)//shenh add
{
    uint32 ret;
    //assert(bs->nbBits==8);
    ret = (*pBuf); ret<<=8;
    ret |=(*(pBuf+1)); ret<<=8;
    ret |= (*(pBuf+2)); ret<<=8;
    ret |= (*(pBuf+3));
    return ret;
}


u64 gf_bs_read_u64(GF_BitStream *bs)
{
#ifdef SHEEN_VC_DEBUG
    __int64 ret;
#else
    unsigned long long  ret;
#endif
    ret = gf_bs_read_u32(bs); ret<<=32;
    ret |= gf_bs_read_u32(bs);
    return ret;
}


void gf_bs_skip_bytes(GF_BitStream *bs, u64 nbBytes)
{
    if (!bs || !nbBytes) return;
    //gf_f64_seek(bs->stream, nbBytes, SEEK_CUR);change shenh
    //FS_Seek((INT32)bs->stream, nbBytes, FS_SEEK_CUR);
    mp4_fs_seek(bs, nbBytes, FS_SEEK_CUR);
    bs->position += nbBytes;
    return;
}

u64 gf_bs_available(GF_BitStream *bs)
{
    return ((bs->size > bs->position)?(bs->size - bs->position):0);
}

uint32 gf_bs_read_data(GF_BitStream *bs, uint8 *data, uint32 nbBytes)
{
    //u64 orig = bs->position;
    switch (bs->bsmode)
    {
        case GF_BITSTREAM_READ:
            memcpy(data, bs->original + bs->position, nbBytes);
            bs->position += nbBytes;
            return nbBytes;
        case GF_BITSTREAM_FILE_READ:
        case GF_BITSTREAM_FILE_WRITE:
            //fread(data, nbBytes, 1, bs->stream); change shenh
            //if(FS_Read((INT32)bs->stream, data, nbBytes)==nbBytes)
            if(mp4_fs_read(bs, data, nbBytes)==nbBytes)
            {
                bs->position += nbBytes;
                return nbBytes;
            }
            else
                return 0;
        default:
            return 0;
    }
}

#if 0 //shenh change
GF_BitStream *gf_bs_new(unsigned char *buffer, u64 BufferSize, uint32 mode)
{
    GF_BitStream *tmp;
    if ( (buffer && ! BufferSize)) return NULL;

    tmp = (GF_BitStream *)malloc(sizeof(GF_BitStream));
    if (!tmp) return NULL;
    memset(tmp, 0, sizeof(GF_BitStream));
    tmp->original = (char*)buffer;
    tmp->size = BufferSize;

    tmp->position = 0;
    tmp->current = 0;
    tmp->stream = NULL;
    tmp->nbBits = 8;
    tmp->bsmode = mode;
    tmp->current = 0;
    return tmp;
}
#endif

GF_BitStream *gf_bs_new(uint8 *buffer, u64 BufferSize, uint32 mode)
{
    GF_BitStream *tmp;
    if ( (buffer && ! BufferSize)) return NULL;

    //tmp = (GF_BitStream *)malloc(sizeof(GF_BitStream));//shenh change
    tmp= bs_glob;
    if (!tmp) return NULL;
    memset(tmp, 0, sizeof(GF_BitStream));
    tmp->original = (int8*)buffer;
    tmp->size = BufferSize;

    tmp->position = 0;
    tmp->current = 0;
    tmp->stream = NULL;
    tmp->nbBits = 8;
    tmp->bsmode = mode;
    tmp->current = 0;
    return tmp;
}

GF_BitStream *gf_bs_from_file(HANDLE *f, uint8 file_mode, uint32 data_Len, uint32 mode)
{
    GF_BitStream *tmp;
    //if (!f) return NULL;
    if (f<0) return NULL;
    //diag_printf("mmc_MemMalloc .... gf_bs_from_file \n");
    tmp = (GF_BitStream *)mmc_MemMalloc(sizeof(GF_BitStream));
    if (!tmp) return NULL;
    memset(tmp, 0, sizeof(GF_BitStream));
    /*switch to internal mode*/
    mode = (mode==GF_BITSTREAM_READ) ? GF_BITSTREAM_FILE_READ : GF_BITSTREAM_FILE_WRITE;
    tmp->bsmode = mode;
    tmp->current = 0;
    tmp->nbBits = (mode == GF_BITSTREAM_FILE_READ) ? 8 : 0;
    tmp->original = NULL;
    tmp->position = 0;
    tmp->stream = f;
    /*init the 3 parameters for mp4 fs.sheen*/
    tmp->mp4_fs_mode= file_mode;
    tmp->mp4_fs_bufLen= data_Len;
    tmp->pMp4_fs_point= (uint8*)f;

    /*get the size of this file (for read streams)*/
    /*change shenh
    tmp->position = gf_f64_tell(f);
    gf_f64_seek(f, 0, SEEK_END);
    tmp->size = gf_f64_tell(f);
    gf_f64_seek(f, tmp->position, SEEK_SET);
    */
    tmp->position = 0;
    //tmp->size =(u64)FS_GetFileSize((INT32)f);
    tmp->size =(u64)mp4_fs_getFileSize(tmp );
    //diag_printf("tmp->size %d\n", tmp->size);
    //FS_Seek((INT32)f, 0, FS_SEEK_SET);
    mp4_fs_seek(tmp, 0, FS_SEEK_SET);
    return tmp;
}

u64 gf_bs_get_size(GF_BitStream *bs)
{
    return bs->size;
}

u64 gf_bs_get_position(GF_BitStream *bs)
{
    return bs->position;
}



#define NO_OPTS

//static uint8 bit_mask[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
//static uint8 bits_mask[] = {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F};

uint8 gf_bs_read_bit(GF_BitStream *bs)
{
    if (bs->nbBits == 8)
    {
        bs->current = BS_ReadByte(bs);
        bs->nbBits = 0;
    }
#ifdef NO_OPTS
    {
        s32 ret;
        bs->current <<= 1;
        bs->nbBits++;
        ret = (bs->current & 0x100) >> 8;
        return (uint8) ret;
    }
#else
    return (uint8) (bs->current & bit_mask[bs->nbBits++]) ? 1 : 0;
#endif

}


uint32 gf_bs_read_int(GF_BitStream *bs, uint32 nBits)
{
    uint32 ret;

#ifndef NO_OPTS
    if (nBits + bs->nbBits <= 8)
    {
        bs->nbBits += nBits;
        ret = (bs->current >> (8 - bs->nbBits) ) & bits_mask[nBits];
        return ret;
    }
#endif
    ret = 0;
    while (nBits-- > 0)
    {
        ret <<= 1;
        ret |= gf_bs_read_bit(bs);
    }
    return ret;
}

GF_Err BS_SeekIntern(GF_BitStream *bs, u64 offset)
{
    //uint32 i;
    /*if mem, do it */
#if 0 //change shenh
    if ((bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_WRITE) || (bs->bsmode == GF_BITSTREAM_WRITE_DYN))
    {
        if (offset > 0xFFFFFFFF) return GF_IO_ERR;
        /*0 for write, read will be done automatically*/
        if (offset >= bs->size)
        {
            if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_WRITE) ) return GF_BAD_PARAM;
            /*in DYN, realloc ...*/
            bs->original = (char*)realloc(bs->original, (uint32) (offset + 1));
            for (i = 0; i < (uint32) (offset + 1 - bs->size); i++)
            {
                bs->original[bs->size + i] = 0;
            }
            bs->size = offset + 1;
        }
        bs->current = bs->original[offset];
        bs->position = offset;
        bs->nbBits = (bs->bsmode == GF_BITSTREAM_READ) ? 8 : 0;
        return GF_OK;
    }
#endif
    //gf_f64_seek(bs->stream, offset, SEEK_SET); change shenh
    //FS_Seek((INT32)bs->stream, offset, FS_SEEK_SET);
    mp4_fs_seek( bs, offset, FS_SEEK_SET);

    bs->position = offset;
    bs->current = 0;
    /*setup NbBits so that next acccess to the buffer will trigger read/write*/
    bs->nbBits = (bs->bsmode == GF_BITSTREAM_FILE_READ) ? 8 : 0;
    return GF_OK;
}

#if 0 //change shenh
void BS_WriteByte(GF_BitStream *bs, uint8 val)
{
    /*we don't allow write on READ buffers*/
    if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_FILE_READ) ) return;
    if (!bs->original && !bs->stream) return;
    /*we are in FILE mode, no pb for any realloc...*/
    fputc(val, bs->stream);
    /*check we didn't rewind the stream*/
    if (bs->size == bs->position) bs->size++;
    bs->position += 1;
}

void BS_WriteBit(GF_BitStream *bs, uint32 bit)
{
    bs->current <<= 1;
    bs->current |= bit;
    if (++ bs->nbBits == 8)
    {
        bs->nbBits = 0;
        BS_WriteByte(bs, (uint8) bs->current);
        bs->current = 0;
    }
}

void gf_bs_write_int(GF_BitStream *bs, s32 value, s32 nBits)
{
    value <<= sizeof (s32) * 8 - nBits;
    while (--nBits >= 0)
    {
        BS_WriteBit (bs, value < 0);
        value <<= 1;
    }
}
#endif

//add shenh
void gf_bs_bit_to_int(GF_BitToInt *bs, s32 value, s32 nBits)
{
    value <<= sizeof (s32) * 8 - nBits;
    while (--nBits >= 0)
    {
        bs->current <<= 1;
        bs->current |= (uint8)(value < 0);
        if (++ bs->nbBits == 8)
        {
            bs->nbBits = 0;
            *(bs->stream)=bs->current;
            bs->stream+=1;
            bs->current = 0;
        }
        value <<= 1;
    }
}

//

/*align return the num of bits read in READ mode, 0 in WRITE*/
uint8 gf_bs_align(GF_BitStream *bs)
{
    uint8 res = 8 - bs->nbBits;
    if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_FILE_READ))
    {
        if (res > 0)
        {
            gf_bs_read_int(bs, res);
        }
        return res;
    }
#if 0 //change shenh
    if (bs->nbBits > 0)
    {
        gf_bs_write_int (bs, 0, res);
        return res;
    }
    return 0;
#endif
    return 0;
}

/*seek from begining of stream: align before anything else*/
GF_Err gf_bs_seek(GF_BitStream *bs, u64 offset)
{
    /*warning: we allow offset = bs->size for WRITE buffers*/
    if (offset > bs->size) return GF_BAD_PARAM;

    gf_bs_align(bs);
    return BS_SeekIntern(bs, offset);
}

uint32 gf_bs_peek_bits(GF_BitStream *bs, uint32 numBits, uint32 byte_offset)
{
    u64 curPos;
    uint32 curBits, ret, current;

    if ( (bs->bsmode != GF_BITSTREAM_READ) && (bs->bsmode != GF_BITSTREAM_FILE_READ)) return 0;
    if (!numBits || (bs->size < bs->position + byte_offset)) return 0;

    /*store our state*/
    curPos = bs->position;
    curBits = bs->nbBits;
    current = bs->current;

    if (byte_offset) gf_bs_seek(bs, bs->position + byte_offset);
    ret = gf_bs_read_int(bs, numBits);

    /*restore our cache - position*/
    gf_bs_seek(bs, curPos);
    /*to avoid re-reading our bits ...*/
    bs->nbBits = curBits;
    bs->current = current;
    return ret;
}

boolean BS_IsAlign(GF_BitStream *bs)
{
    switch (bs->bsmode)
    {
        case GF_BITSTREAM_READ:
        case GF_BITSTREAM_FILE_READ:
            return ( (8 == bs->nbBits) ? 1 : 0);
        default:
            return !bs->nbBits;
    }
}

#if 0
uint32 gf_bs_write_data(GF_BitStream *bs, unsigned char *data, uint32 nbBytes)
{
    /*we need some feedback for this guy...*/
    u64 begin = bs->position;
    if (!nbBytes) return 0;

    if (BS_IsAlign(bs))
    {
        switch (bs->bsmode)
        {
            case GF_BITSTREAM_WRITE:
                if (bs->position+nbBytes > bs->size)
                    return 0;
                memcpy(bs->original + bs->position, data, nbBytes);
                bs->position += nbBytes;
                return nbBytes;
            case GF_BITSTREAM_WRITE_DYN:
                /*need to realloc ...*/
                if (bs->position+nbBytes > bs->size)
                {
                    if (bs->size + nbBytes > 0xFFFFFFFF)
                        return 0;
                    bs->original = (char*)realloc(bs->original, sizeof(uint32)*((uint32) bs->size + nbBytes));
                    if (!bs->original)
                        return 0;
                    bs->size += nbBytes;
                }
                memcpy(bs->original + bs->position, data, nbBytes);
                bs->position += nbBytes;
                return nbBytes;
            case GF_BITSTREAM_FILE_READ:
            case GF_BITSTREAM_FILE_WRITE:
                fwrite(data, nbBytes, 1, bs->stream);
                if (bs->size == bs->position) bs->size += nbBytes;
                bs->position += nbBytes;
                return nbBytes;
            default:
                return 0;
        }
    }

    while (nbBytes)
    {
        gf_bs_write_int(bs, (s32) *data, 8);
        data++;
        nbBytes--;
    }
    return (uint32) (bs->position - begin);
}
#endif

#if 0 //shenh change
void gf_bs_del(GF_BitStream *bs)
{
    if (!bs) return;
    /*if we are in dynamic mode (alloc done by the bitstream), free the buffer if still present*/
    if ((bs->bsmode == GF_BITSTREAM_WRITE_DYN) && bs->original) free(bs->original);
    free(bs);
}
#endif

void gf_bs_del(GF_BitStream *bs)
{
    //if (!bs) return;
    /*if we are in dynamic mode (alloc done by the bitstream), free the buffer if still present*/
    //if ((bs->bsmode == GF_BITSTREAM_WRITE_DYN) && bs->original) free(bs->original);
    //free(bs);
    memset(bs, 0, sizeof(GF_BitStream));
}

#endif

