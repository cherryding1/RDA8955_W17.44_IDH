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

#include "cs_types.h"



uint32 LILY_FM_RDA5800_init (uint8 scl,uint8 sda) ;

uint32 LILY_FM_RDA5800_close(void);

uint32 LILY_FM_RDA5800_open(uint32 bUseLastFreq);

uint32 LILY_FM_RDA5800_tune(uint32 iFrequency,uint32 isStereo);

uint32 LILY_FM_RDA5800_mute(uint32 isMute);

uint32 LILY_FM_RDA5800_setVolume(uint32 iVol);

uint32 LILY_FM_RDA5800_seek(uint32 isSeekUp);

uint32 LILY_FM_RDA5800_I2S(void);

uint32 LILY_FM_RDA5800_STOPI2S(void);




