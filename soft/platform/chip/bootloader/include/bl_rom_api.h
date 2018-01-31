/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BL_ROM_API_H_
#define _BL_ROM_API_H_

void *memcpy(void *dest, const void *src, unsigned n);
void *memset(void *s, int c, unsigned n);
int memcmp(const void *s1, const void *s2, unsigned n);

void mon_Event(unsigned evt);
void boot_InvalidICache(void);
void boot_FlushDCache(unsigned char invalidate);

#endif
