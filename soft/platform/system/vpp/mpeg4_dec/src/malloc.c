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



#include <string.h>
#include <stdlib.h>

#include "cs_types.h"
#include "sxr_ops.h"


#define MALLOC_CHUNK_SIZE 128

// =============================================================================
// MALLOC_CHUNK_T
// -----------------------------------------------------------------------------
/// List of all sxr_Malloc();
// =============================================================================

typedef struct MALLOC_CHUNK_STRUCT_T
{
    VOID*                         addr[MALLOC_CHUNK_SIZE];

    struct MALLOC_CHUNK_STRUCT_T* next;
} MALLOC_CHUNK_T ;


PRIVATE MALLOC_CHUNK_T* firstChunk = 0;

VOID* xvid_myMalloc(UINT32 size)
{
    VOID*            addr;
    MALLOC_CHUNK_T** currentChunk;
    UINT32           i;

    addr = sxr_Malloc(size);

    if(addr == 0)
    {
        return 0;
    }

    currentChunk = &firstChunk;

    while(*currentChunk)
    {
        for(i = 0; i < MALLOC_CHUNK_SIZE; ++i)
        {
            if((*currentChunk)->addr[i] == 0)
            {
                break;
            }
        }

        if(i != MALLOC_CHUNK_SIZE)
        {
            break;
        }

        currentChunk = &(*currentChunk)->next;
    }

    if(*currentChunk == 0)
    {
        *currentChunk = (MALLOC_CHUNK_T*)sxr_Malloc(sizeof(MALLOC_CHUNK_T));
        if(*currentChunk == 0)
        {
            sxr_Free(addr);
            return 0;
        }
        memset(*currentChunk, 0, sizeof(MALLOC_CHUNK_T));
        i = 0;
    }

    (*currentChunk)->addr[i] = addr;

    return addr;
}

VOID xvid_myFree(VOID* buffer)
{
    MALLOC_CHUNK_T* currentChunk;
    UINT32          i;

    currentChunk = firstChunk;

    while(currentChunk)
    {
        for(i = 0; i <  MALLOC_CHUNK_SIZE; ++i)
        {
            if(currentChunk->addr[i] == buffer)
            {
                break;
            }
        }

        if(i != MALLOC_CHUNK_SIZE)
        {
            break;
        }

        currentChunk = currentChunk->next;
    }

    if(currentChunk)
    {
        sxr_Free(buffer);
        currentChunk->addr[i] = 0;
    }
    else
    {
        // Error: double free ??
    }
}

VOID xvid_freeAll(VOID)
{
    MALLOC_CHUNK_T* currentChunk;
    MALLOC_CHUNK_T* nextChunk;
    UINT32          i;

    currentChunk = firstChunk;

    while(currentChunk)
    {
        for(i = 0; i <  MALLOC_CHUNK_SIZE; ++i)
        {
            if(currentChunk->addr[i] != 0)
            {
                sxr_Free(currentChunk->addr[i]);
            }
        }

        currentChunk = currentChunk->next;
    }

    currentChunk = firstChunk;

    while(currentChunk)
    {
        nextChunk    = currentChunk->next;
        sxr_Free(currentChunk);
        currentChunk = nextChunk;
    }
}

