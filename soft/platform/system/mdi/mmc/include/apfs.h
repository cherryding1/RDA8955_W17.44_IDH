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


#ifndef APFS_H
#define APFS_H


#include "cs_types.h"

#include "fs.h"
#include "mci.h"

extern PUBLIC MCI_ERR_T apfs_GetFileInformation (
    CONST INT32 FileHander,
    AudDesInfoStruct  * CONST DecInfo,
    CONST mci_type_enum FileType   );



extern PUBLIC MCI_ERR_T apfs_GetDurationTime(
    INT32 FileHander,
    mci_type_enum FileType,
    INT32 BeginPlayProgress,
    INT32 OffsetPlayProgress,
    MCI_ProgressInf* PlayInformation);

#endif


