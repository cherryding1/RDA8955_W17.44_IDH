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


#ifndef DEFINE_H
#define DEFINE_H
#define ENABLE_GRAY 0

#undef INT_MAX//sheen

#define INT_MAX 0x7FFFFFFF//(1<<31)
#define INT64_MAX (1<<30)
#define INT_MIN (-1<<30)
#define ENABLE_THREADS 0

#define restrict
#define ENABLE_SMALL 0
#define INT64_C __int64

#define snprintf sprintf


enum
{
    ENOMEM = 0,
    EINVAL   = 1,
    START_ENCODE  = 2,
    IN_ENCODE     = 3,
    END_ENCODE    = 4
};

#define ENABLE_H264_DECODER 1
#endif
