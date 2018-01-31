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














#if defined(_WIN32)
/* <windows.h> is needed for registry and ini file access */
#include "hxtypes.h"
#include <windows.h>
#endif /* defined(_WIN32) */

#include "beginhdr.h"
#include "frmlist.h"
#include "rvdecoder.h"

#if defined(DEBUG_FRAMELIST)
#include <stdio.h>
extern FILE_HDL g_framelist;
#endif


/* TBD: the mutexes are C++ only, so DECODE_FRAME_LIST_MUTEX will not work */


void DecodedFrameList_Init(struct DecodedFrameList *t)
{
    t->m_pHead = t->m_pTail = 0;
    t->m_uLength = 0;
}



void DecodedFrameList_Delete(struct DecodedFrameList *t)
{
    struct DecodedFrame *pCurr = t->m_pHead;
    while (pCurr)
    {
        struct DecodedFrame *pNext = pCurr->m_pFutureFrame;;
        DecodedFrame_Delete(pCurr);
        pCurr = pNext;
    }

    t->m_pHead = t->m_pTail = 0;
    t->m_uLength = 0;
}

struct DecodedFrame*
DecodedFrameList_detachHead(struct DecodedFrameList *t)
{
    struct DecodedFrame *pHead = t->m_pHead;
    if (pHead)
    {
        t->m_uLength--;
#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
        fprintf(g_framelist, "%x detachHead : %d (%d)\n",
                (U32)t, pHead->m_uFrameNumber,
                t->m_uLength);
#endif
        t->m_pHead = t->m_pHead->m_pFutureFrame;
        if (t->m_pHead)
            t->m_pHead->m_pPreviousFrame = 0;
        else
            t->m_pTail = 0;
    }
    return pHead;
}

void
DecodedFrameList_append(struct DecodedFrameList *t,
                        struct DecodedFrame *pFrame)
{
    pFrame->m_pPreviousFrame = t->m_pTail;
    pFrame->m_pFutureFrame = 0;
    if (t->m_pTail)
        t->m_pTail->m_pFutureFrame = pFrame;
    else
        t->m_pHead = pFrame;

    t->m_pTail = pFrame;
    t->m_uLength++;
#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
    fprintf(g_framelist, "%x append     : %d (%d)\n",
            (U32)t, pFrame->m_uFrameNumber,
            t->m_uLength);
#endif
}

void
DecodedFrameList_insertList(struct DecodedFrameList *t,
                            struct DecodedFrameList *src)
{
    if (src->m_pHead)
    {
        src->m_pTail->m_pFutureFrame = t->m_pHead;
        if (t->m_pHead)
            t->m_pHead->m_pPreviousFrame = src->m_pTail;
        t->m_pHead = src->m_pHead;

        if (!t->m_pTail)
            t->m_pTail = src->m_pTail;

        src->m_pHead = src->m_pTail = 0;
    }
}

void DecodedFrameList2_Init(struct DecodedFrameList2 *t)
{
    t->m_pHead = t->m_pTail = 0;
    t->m_uLength = 0;
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_makeMutex(&(t->m_pMutex));
    /* Make a mutex to protect members from concurrent access */
    /* by RMA threads. Typically: One thread enters the decoder through Decode() */
    /* and then detachHead() here, setting m_pTail to NULL.  Meanwhile another  */
    /* RMA thread (usually the renderer) enters the decoder through  */
    /* ReleaseFrame() and then here through append() and *uses* m_pTail right  */
    /* after it was set to NULL by the other thread. */
#endif
}



void DecodedFrameList2_Delete(struct DecodedFrameList2 *t)
{
    t->m_pHead = t->m_pTail = 0;
    t->m_uLength = 0;
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_deleteMutex(t->m_pMutex);
#endif
}

struct DecodedFrame*
DecodedFrameList2_detachHead(struct DecodedFrameList2 *t)
{
    struct DecodedFrame *pHead;
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_lockMutex(t->m_pMutex);
#endif
    pHead = t->m_pHead;
    if (pHead)
    {
        t->m_uLength--;
#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
        fprintf(g_framelist, "%x detachHead2 : %d (%d)\n",
                (U32)t, pHead->m_uFrameNumber,
                t->m_uLength);
#endif
        t->m_pHead = t->m_pHead->m_pFutureFrame2;
        if (t->m_pHead)
            t->m_pHead->m_pPreviousFrame2 = 0;
        else
            t->m_pTail = 0;
    }
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_unlockMutex(t->m_pMutex);
#endif
    return pHead;
}

void
DecodedFrameList2_append(struct DecodedFrameList2 *t,
                         struct DecodedFrame *pFrame)
{
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_lockMutex(t->m_pMutex);
#endif
    pFrame->m_pPreviousFrame2=t->m_pTail;
    pFrame->m_pFutureFrame2=0;
    if (t->m_pTail)
        t->m_pTail->m_pFutureFrame2=pFrame;
    else
        t->m_pHead = pFrame;

    t->m_pTail = pFrame;
    t->m_uLength++;
#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
    fprintf(g_framelist, "%x append2     : %d (%d)\n",
            (U32)t, pFrame->m_uFrameNumber,
            t->m_uLength);
#endif
#ifdef DECODE_FRAME_LIST_MUTEX
    Decoder_unlockMutex(t->m_pMutex);
#endif
}





