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




#ifndef MMC_MOTION_DETECT_H
#define MMC_MOTION_DETECT_H

//sensitive level
typedef enum
{
    SENSE_LOW,
    SENSE_MID,
    SENSE_HIGH
} MD_LEVEL;

int motion_detect_open(MD_LEVEL sensLevel, void(*md_callback)(int32));
int motion_detect_close();
void do_motion_detect(void);


#endif

