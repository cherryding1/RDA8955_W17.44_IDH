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



#ifndef RM_ALLOCATOR_H
#define RM_ALLOCATOR_H

/* Includes */
#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rm_allocator_block_struct
{
    BYTE*  pPtr;
    UINT32 ulSize;
    UINT32 ulRefCount;
} rm_allocator_block;

typedef struct rm_allocator_struct
{
    void*               pUserMem;
    rm_malloc_func_ptr  fpMalloc;
    rm_free_func_ptr    fpFree;
    UINT32              ulNumBlocks;
    rm_allocator_block* pBlock;
} rm_allocator;

/*
 * rm_allocator_init()
 *
 * This should be called before using the rm_allocator
 * struct to create any buffers.
 */
HX_RESULT rm_allocator_init(rm_allocator*      pAlloc,
                            void*              pUserMem,
                            rm_malloc_func_ptr fpMalloc,
                            rm_free_func_ptr   fpFree);

/*
 * rm_allocator_preallocate()
 *
 * This function can be called to pre-allocate ulNumBuffers,
 * each of size ulBufferSize. Each of these pre-allocated
 * buffers have a ref count of 0, meaning they are
 * currently unused.
 */
HX_RESULT rm_allocator_preallocate(rm_allocator* pAlloc,
                                   UINT32        ulNumBuffers,
                                   UINT32        ulBufferSize);

/*
 * rm_allocator_get_buffer()
 *
 * This function is called to return a buffer of size ulSize
 * in the out parameter ppBuf. Internally, the buffer may be
 * allocated or it may be returned from the pool of
 * unused buffers (those with a ref count of 0). If the user
 * wants to force the allocation of a buffer (and NOT use
 * previously used buffer, then set bNoReUse to TRUE).
 */
HX_RESULT rm_allocator_get_buffer(rm_allocator* pAlloc,
                                  UINT32        ulSize,
                                  HXBOOL        bNoReUse,
                                  BYTE**        ppBuf);

/*
 * rm_allocator_addref_buffer()
 *
 * This function is called when an object needs to
 * increase the ref count on a buffer. For instance, suppose
 * an rm_allocator is shared between objects A and B.
 * Object A creates a buffer, passes it to object B,
 * which processes it and then no longer needs it.
 * Object A would call rm_allocator_get_buffer() to
 * get a buffer from the pool. At this point the buffer
 * would have a ref count of 2. It would then tranfer
 * the buffer to object B, which would call
 * rm_allocator_addref_buffer() to place an additional
 * ref on the buffer, giving it a ref count of 3. Then
 * when object A no longer needs the buffer, it would call
 * rm_allocator_release_buffer(), so the ref count would
 * go back to 2. After object B no longer needs the buffer, it would
 * called rm_allocator_release_buffer(), which would cause
 * the buffer to have a ref count of 1, and thus make
 * it available to be chosen in rm_allocator_get_buffer().
 */
HX_RESULT rm_allocator_addref_buffer(rm_allocator* pAlloc,
                                     BYTE*         pBuf);

/*
 * rm_allocator_release_buffer()
 *
 * This function is called to when the user is finished with
 * buffer pBuf which was created with rm_allocator_get_buffer().
 * Buffers not created with rm_allocator_get_buffer()
 * should NOT be passed into rm_allocator_release_buffer.
 */
HX_RESULT rm_allocator_release_buffer(rm_allocator* pAlloc,
                                      BYTE*         pBuf);

/*
 * rm_allocator_cleanup()
 *
 * This function is called to when the user is finished with
 * all buffers. If there are still non-zero ref counts
 * on any buffers in the pool, then the buffers are
 * still deleted, but an error is returned.
 */
void rm_allocator_cleanup(rm_allocator* pAlloc);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RM_ALLOCATOR_H */
