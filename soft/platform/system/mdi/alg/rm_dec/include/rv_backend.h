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



#ifndef RV_BACKEND_H__
#define RV_BACKEND_H__

/* Unified RealVideo decoder backend interface */

#include "helix_types.h"
#include "helix_result.h"
#include "rv_format_info.h"
#include "rv_backend_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/* rv decoder backend interface */

typedef HX_RESULT (*rv_backend_init_fp) (void *prv10Init,
        void **global);
typedef HX_RESULT (*rv_backend_decode_fp) (UCHAR *pRV10Packets,
        UCHAR *pDecodedFrameBuffer,
        void *pInputParams,
        void *pOutputParams,
        void *global);
typedef HX_RESULT (*rv_backend_custom_message_fp) (UINT32 *msg_id,
        void *global);
typedef HX_RESULT (*rv_backend_hive_message_fp) (UINT32 *msg,
        void *global);
typedef HX_RESULT (*rv_backend_free_fp) (void* global);


/* The rv_backend struct contains the RealVideo decoder backend
 * function pointers. */

typedef struct rv_backend_struct
{
    rv_backend_init_fp            fpInit;
    rv_backend_decode_fp          fpDecode;
    rv_backend_custom_message_fp  fpCustomMessage;
    rv_backend_hive_message_fp    fpHiveMessage;
    rv_backend_free_fp            fpFree;
} rv_backend;


#ifdef __cplusplus
}
#endif

#endif /* RV_BACKEND_H__ */
