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

#ifndef MMC_MEITU
#define MMC_MEITU

typedef enum
{
    MMC_MEITU_BLACKWHITE = 0,
    MMC_MEITU_EMBOSS,
    MMC_MEITU_GAUSSIANBLUR,
    MMC_MEITU_SKINSMOOTH,
    MMC_MEITU_PENCIL,
    MMC_MEITU_FEATHER,
    MMC_MEITU_EDGEFILTER,
    MMC_MEITU_LOMO,
} MeituFiltermode;

void MEITUIsr(void);
boolean getMeituFinish(void);
INT32 initVocMeitu(void);
void quitVocMeitu(void);
uint32 getMeituLen(void);
void  MMC_Meitu (UINT16 imgw, UINT16 imgh, char *bufin, char *bufout, MeituFiltermode filter_mode, UINT16 brightness, UINT16 contrast);

#endif