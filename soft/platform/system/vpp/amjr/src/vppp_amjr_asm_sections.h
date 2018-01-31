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


#define SECTION_1_START            (((RAM_I_MIN)+0x0200)/2)
#define SECTION_2_START            (((RAM_I_MIN)+0x1200)/2)

// start of different image sections
#define SECTION_JPEG_ENC_START      SECTION_1_START
#define SECTION_AMR_ENC_START       SECTION_2_START
#define SECTION_MP3_ENC_START       SECTION_2_START

// size of different image sections
#define SECTION_JPEG_ENC_SIZE       ((((RAM_I_MIN)+0x1200)/2) - SECTION_JPEG_ENC_START)
#define SECTION_AMR_ENC_SIZE        ((((RAM_I_MIN)+0x2200)/2) - SECTION_AMR_ENC_START)
#define SECTION_MP3_ENC_SIZE        ((((RAM_I_MIN)+0x2000)/2) - SECTION_MP3_ENC_START)
