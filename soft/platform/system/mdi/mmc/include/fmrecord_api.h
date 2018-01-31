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



uint32 LILY_FmrRecordStart (int8 *file_name_p) ;    //  MCI_MEDIA_PLAY_REQ,

uint32 LILY_FmrRecordPause(void);

uint32 LILY_FmrRecordResume(void);

uint32 LILY_FmrRecordStop(void);
int32 FmrRecordStart(int32 volume, int32 play_style, char * filename);

