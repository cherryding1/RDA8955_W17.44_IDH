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


#define SECTION_1_START            (((RAM_I_MIN)+0x300)/2)
#define SECTION_2_START            (((RAM_I_MIN)+0xd00)/2)

// start of different image sections
#define SECTION_JPEG_DEC_START      SECTION_1_START
#define SECTION_ZOOM_DEC_START      SECTION_1_START
#define SECTION_MP12_DEC_START      SECTION_1_START
#define SECTION_AAC_DEC_START       SECTION_2_START
#define SECTION_MP3_DEC_START       SECTION_2_START
#define SECTION_AMR_DEC_START       SECTION_2_START

// size of different image sections
#define SECTION_JPEG_DEC_SIZE       ((((RAM_I_MIN)+0x0c00)/2) - SECTION_JPEG_DEC_START)
#define SECTION_ZOOM_DEC_SIZE       ((((RAM_I_MIN)+0x0c00)/2) - SECTION_JPEG_DEC_START)
#define SECTION_MP12_DEC_SIZE       ((((RAM_I_MIN)+0x0700)/2) - SECTION_MP12_DEC_START)
#define SECTION_AAC_DEC_SIZE        ((((RAM_I_MIN)+0x2400)/2) - SECTION_AAC_DEC_START)
#define SECTION_MP3_DEC_SIZE        ((((RAM_I_MIN)+0x2400)/2) - SECTION_MP3_DEC_START)
#define SECTION_AMR_DEC_SIZE        ((((RAM_I_MIN)+0x1900)/2) - SECTION_AMR_DEC_START)
