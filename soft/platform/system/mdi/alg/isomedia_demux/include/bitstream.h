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

#ifndef _GF_BITSTREAM_H_
#define _GF_BITSTREAM_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    /*original stream data*/
    HANDLE *stream;
    /*stream file mode(0 file or 1 buffer).sheen*/
    uint8 mp4_fs_mode;
    /*file buffer length when mp4_fs_mode!=0. sheen*/
    uint32 mp4_fs_bufLen;
    /*file buffer read point. sheen*/
    uint8 *pMp4_fs_point;

    /*original data*/
    int8 *original;
    /*the size of our buffer*/
    u64 size;
    /*current position in BYTES*/
    u64 position;
    /*the byte readen/written */
    uint32 current;
    /*the number of bits in the current byte*/
    uint32 nbBits;
    /*the bitstream mode*/
    uint32 bsmode;
    void (*EndOfStream)(void *par);
    void *par;
} GF_BitStream ;

//add shenh
typedef struct
{
    /*original stream data*/
    uint8 *stream;
    /*the byte readen/written */
    uint8 current;
    /*the number of bits in the current byte*/
    uint32 nbBits;
} GF_BitToInt ;
//

/*!
 *  \brief data reading
 *
 *  Reads a data buffer
 *  \param bs the target bitstream
 *  \param data the data buffer to be filled
 *  \param nbBytes the amount of bytes to read
 *  \return the number of bytes actually read.
 *  \warning the data buffer passed must be large enough to hold the desired amount of bytes.
 */
uint32 gf_bs_read_data(GF_BitStream *bs, uint8 *data, uint32 nbBytes);

/*!
 *  \brief align char reading
 *
 *  Reads an integer coded on 8 bits starting at a byte boundary in the bitstream.
 *  \warning you must not use this function if the bitstream is not aligned
 *  \param bs the target bitstream
 *  \return the char value read.
 */
uint32 gf_bs_read_u8(GF_BitStream *bs);
/*!
 *  \brief align short reading
 *
 *  Reads an integer coded on 16 bits starting at a byte boundary in the bitstream.
 *  \warning you must not use this function if the bitstream is not aligned
 *  \param bs the target bitstream
 *  \return the short value read.
 */
uint32 gf_bs_read_u16(GF_BitStream *bs);
/*!
 *  \brief align 24-bit integer reading
 *
 *  Reads an integer coded on 24 bits starting at a byte boundary in the bitstream.
 *  \warning you must not use this function if the bitstream is not aligned
 *  \param bs the target bitstream
 *  \return the integer value read.
 */
uint32 gf_bs_read_u24(GF_BitStream *bs);
/*!
 *  \brief align integer reading
 *
 *  Reads an integer coded on 32 bits starting at a byte boundary in the bitstream.
 *  \warning you must not use this function if the bitstream is not aligned
 *  \param bs the target bitstream
 *  \return the integer value read.
 */
uint32 gf_bs_read_u32(GF_BitStream *bs);
uint32 gf_buf_read_u32(uint8*pBuf);//shenh add
/*!
 *  \brief align large integer reading
 *
 *  Reads an integer coded on 64 bits starting at a byte boundary in the bitstream.
 *  \warning you must not use this function if the bitstream is not aligned
 *  \param bs the target bitstream
 *  \return the large integer value read.
 */
u64 gf_bs_read_u64(GF_BitStream *bs);
GF_BitStream *gf_bs_from_file(/*GF_BitStream*/HANDLE *f, uint8 file_mode, uint32 data_Len, uint32 mode);//change shenh
u64 gf_bs_get_size(GF_BitStream *bs);
u64 gf_bs_get_position(GF_BitStream *bs);
GF_BitStream *gf_bs_new(uint8 *buffer, u64 size, uint32 mode);

void gf_bs_bit_to_int(GF_BitToInt *bs, s32 value, s32 nBits);


/*
mp4_fs_read. sheen
same as FS_Read.
*/
INT32 mp4_fs_read(GF_BitStream *bs, UINT8 *pBuf, UINT32 iLen);
/*
mp4_fs_seek. sheen
same as FS_Seek.
*/
INT32 mp4_fs_seek(GF_BitStream *bs, INT32 iOffset, UINT8 iOrigin);
/*
mp4_fs_getFileSize. sheen
same as FS_close.
*/
INT32 mp4_fs_getFileSize(GF_BitStream *bs);

#endif      /*_BITSTREAM_H_*/

#endif
