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



#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

#include "portab.h"

void *xvid_malloc(size_t size,
                  uint8_t alignment);
void xvid_free(void *mem_ptr);
#endif                          /* _MEM_ALIGN_H_ */

