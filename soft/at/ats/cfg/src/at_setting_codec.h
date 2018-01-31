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

#ifndef __AT_SETTING_INTERNAL_HEADER
#define __AT_SETTING_INTERNAL_HEADER

#include <stdint.h>
#include <at_utils.h>
#include <at_cfg.h>

extern bool at_setting_decode(at_setting_t *setting, uint8_t *buffer, unsigned length);
extern bool at_setting_encode(const at_setting_t *setting, uint8_t **buffer, unsigned *length);

#endif /* __AT_SETTING_INTERNAL_HEADER */
