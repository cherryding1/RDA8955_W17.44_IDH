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














#ifndef FRMLIST_H__
#define FRMLIST_H__

#include "rvtypes.h"
#include "frame.h"


#ifdef DECODE_FRAME_LIST_MUTEX
#include "rv89thread.h" /* needed for the mutex in DecodedFrameList2 */
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* The DecodedFrameList class implements a doubly-linked list of */
/* DecodedFrame objects.  It uses the m_pPreviousFrame and m_pFutureFrame */
/* members of the DecodedFrame class to implement the links. */

struct DecodedFrameList
{
    struct DecodedFrame               *m_pHead;
    struct DecodedFrame               *m_pTail;
    /* m_pHead points to the first element in the list, and m_pTail */
    /* points to the last.  m_pHead->previous() and m_pTail->future() */
    /* are both NULL. */

    U32                         m_uLength;

    /*bool            isEmpty() { return !m_pHead; } */

};
void DecodedFrameList_Init(struct DecodedFrameList *t);

void DecodedFrameList_Delete(struct DecodedFrameList *t);

struct DecodedFrame  *DecodedFrameList_detachHead(struct DecodedFrameList *t);
/* Detach the first frame and return a pointer to it, */
/* or return NULL if the list is empty. */

void DecodedFrameList_append(struct DecodedFrameList *t,
                             struct DecodedFrame *pFrame);
/* Append the given frame to our tail */

void DecodedFrameList_insertList(struct DecodedFrameList *t,
                                 struct DecodedFrameList *src);
/* Move the given list to the beginning of our list. */


/* The DecodedFrameList2 class implements a doubly-linked list of */
/* DecodedFrame objects.  It uses the m_pPreviousFrame2 and m_pFutureFrame2 */
/* members of the DecodedFrame class to implement the links. */
/* It is not affects when a frame is decoded as a B frame, and thus */
/* has its reference frame pointers modified. */

struct DecodedFrameList2
{
    struct DecodedFrame               *m_pHead;
    struct DecodedFrame               *m_pTail;
    /* m_pHead points to the first element in the list, and m_pTail */
    /* points to the last.  m_pHead->previous() and m_pTail->future() */
    /* are both NULL. */

    U32                         m_uLength;
#ifdef DECODE_FRAME_LIST_MUTEX
    RV89Mutex*                  m_pMutex;
#endif
    /* protects the head & tail from HX threads operating in detachHead() and append() simultaneously */
    /*bool            isEmpty() { return !m_pHead; } */

    /* returns the length of the list */
};

void DecodedFrameList2_Init(struct DecodedFrameList2 *t);

void DecodedFrameList2_Delete(struct DecodedFrameList2 *t);
struct DecodedFrame*
DecodedFrameList2_detachHead(struct DecodedFrameList2 *t);
void
DecodedFrameList2_append(struct DecodedFrameList2 *t,
                         struct DecodedFrame *pFrame);


#ifdef __cplusplus
}
#endif

#endif /* FRMLIST_H__ */



