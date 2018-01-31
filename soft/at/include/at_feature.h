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



#ifndef __AT_FEATURE_H__
#define __AT_FEATURE_H__

// Extended to support MAL layer. 1 means enabled, 0 disabled.
#define MAL_EXTENDED 1

// A patch switch. defined as 1, means compatible with <cr><lf> suffix command line; 0 means use the standard format of <cr> suffix.
#define LF_PATCH 1

// Unit test switch.
#define AT_UT 0

#endif
