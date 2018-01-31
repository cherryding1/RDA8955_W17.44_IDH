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


#if (CSW_EXTENDED_API == 1)

#include "mci.h"

// Init
BOOL    MCI_TaskInit(VOID)
{
    return TRUE;
}

// Video Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value)
{
    return 0;
}
UINT32  MCI_VideoRecordPause(VOID)
{
    return 0;
}
UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara)
{
    return 0;
}
UINT32  MCI_VideoRecordPreviewStop(VOID)
{
    return 0;
}
UINT32  MCI_VideoRecordResume(VOID)
{
    return 0;
}
UINT32  MCI_VideoRecordStart(char *pfilename)
{
    return 0;
}
UINT32  MCI_VideoRecordStop(VOID)
{
    return 0;
}
UINT32  MCI_DisplayVideoInterface (VOID)
{
    return 0;
}
VOID vdorec_cancel_saveFile(VOID) {}

// Camera
VOID set_mmc_camera_preview() {}
VOID set_multicaputure(INT32 i) {}

#endif // CSW_EXTENDED_API


