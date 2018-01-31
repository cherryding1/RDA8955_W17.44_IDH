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



#define SECTION_0_START            (((RAM_I_MIN)+0x0000)/2)
#define SECTION_1_START            (((RAM_I_MIN)+0x0200)/2)

// start of the images
#define SECTION_MAIN_START          SECTION_0_START
#define SECTION_HI_START            SECTION_1_START
#define SECTION_BYE_START           SECTION_1_START

// end of the images
#define SECTION_MAIN_END            (((RAM_I_MIN)+0x0100)/2)
#define SECTION_HI_END              (((RAM_I_MIN)+0x0280)/2)
#define SECTION_BYE_END             (((RAM_I_MIN)+0x0280)/2)

// size of the images
#define SECTION_MAIN_SIZE           (SECTION_MAIN_END - SECTION_MAIN_START)
#define SECTION_HI_SIZE             (SECTION_HI_END   - SECTION_HI_START)
#define SECTION_BYE_SIZE            (SECTION_BYE_END  - SECTION_BYE_START)
