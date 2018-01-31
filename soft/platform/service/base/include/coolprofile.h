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







#define u16 UINT16
#define u8  UINT8
#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define TBM_ENTRY(id) hal_PXTS_SendProfilingCode((id) & CPMASK)
#define TBM_EXIT(id) hal_PXTS_SendProfilingCode(((id) & CPMASK) | CPEXITFLAG)

extern void hal_PXTS_SendProfilingCode(u16 code);
