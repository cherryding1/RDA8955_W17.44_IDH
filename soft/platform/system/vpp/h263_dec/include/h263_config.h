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










#ifndef H263_CONFIG_H
#define H263_CONFIG_H

#include "cs_types.h"
#include "string.h"
#include "hal_debug.h"

#ifdef NON_ANSI_COMPILER
#define _ANSI_ARGS_(x) ()
#else
#define _ANSI_ARGS_(x) x
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

/************
internal ram start address
************/
//#define PROFILE_DEBUG

//#define USE_INTERNAL_SRAM

#ifdef USE_INTERNAL_SRAM
/*
    internal SRAM
    0x80c00000 - 0x80c0ffff  total 64kB SRAM
    0x80c00000- 0x80c068b0 be used 26kB
    0x80c0fdb0 -0x80c0ffff be used 592 bytes
    0x80c068b0-0x80c0fdaf can be use 38143 bytes

    new position
    start: 0x80c06920
    end: 0x80c0eefc
*/
#define START_OFFSET 0x70

#define YUV2RGB_START 0x80c068b0+START_OFFSET //size 2048*3 bytes
#define RESAMPLE_START 0x80c080b0+START_OFFSET //size uncertain (128x96 image=1408 bytes)

#define IDCTCLIP_START 0x80c08bb0+START_OFFSET  //size 2048 bytes
#define CLP_START 0x80c093b0+START_OFFSET //size 1024 bytes

#define DCT3DTAB0_START 0x80c097b0+START_OFFSET // size 448 bytes
#define DCT3DTAB1_START 0x80c09970+START_OFFSET // size 384 bytes
#define DCT3DTAB2_START 0x80c09af0+START_OFFSET // size 480 bytes
#define INTRA_DCT3DTAB0_START 0x80c09cd0+START_OFFSET // size 448 bytes
#define INTRA_DCT3DTAB1_START 0x80c09e90+START_OFFSET // size 384 bytes
#define INTRA_DCT3DTAB2_START 0x80c0a010+START_OFFSET // size 480 bytes

#define STORE_CODE_START 0x80c0a1f0+START_OFFSET // size 1536 bytes
#define ZIGZAG_SCAN_START 0x80c0a7f0+START_OFFSET // size 64 bytes
#define MQ_CHROMA_QP_TABLE_START 0x80c0a830+START_OFFSET // size 32 bytes
#define INBFR_START 0x80c0a850+START_OFFSET // size 16 bytes
#define BLOCK_START 0x80c0a860+START_OFFSET // size 768 bytes . blk_cnt=6
#define BLOCK_TEMP_START 0x80c0ab60+START_OFFSET // size 128 bytes .  for temp block

#if 0
#define MCBPCTABINTRA_START 0x80c0abe0 // size 128 bytes 
#define CBPYTAB_START 0x80c0aca0 // size 192 bytes 

#define TMNMVTAB0_START 0x80c0ad60 // size 56 bytes 
#define TMNMVTAB1_START 0x80c0ad98 // size 384 bytes 
#define TMNMVTAB2_START 0x80c0af18 // size 492 bytes 

#define MCBPCTAB0_START 0x80c0b104 // size 1024 bytes 
#define MCBPCTAB1_START 0x80c0b504 // size 32 bytes 
#endif

#endif

#endif
