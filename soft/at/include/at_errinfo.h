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

#ifndef __AT_ERRINFO_H__
#define __AT_ERRINFO_H__

#include <stdint.h>

const uint8_t *at_GetResultCodeInfo(int code);
const uint8_t *at_GetCmeCodeInfo(int code);
const uint8_t *at_GetCmsCodeInfo(int code);

// The above functions relay on an ordered array. The following function
// will check whether the array is ordered.
void at_ErrCodeInfoCheck(void);

#endif
