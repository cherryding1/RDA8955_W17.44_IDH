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


#include "cs_types.h"


/* Type to use for aligned memory operations.
   This should normally be the biggest type supported by a single load
   and store.  */
#define op_t    unsigned  int
#define OPSIZ   (sizeof(op_t))

/* Optimal type for storing bytes in registers.  */
#define reg_char    char

#define MERGE(w0, sh_1, w1, sh_2) (((w0) >> (sh_1)) | ((w1) << (sh_2)))

/* Copy exactly NBYTES bytes from SRC_BP to DST_BP,
   without any assumptions about alignment of the pointers.  */
#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)               \
do {                                    \
    size_t __nbytes = (nbytes);                 \
    while (__nbytes > 0) {                      \
        unsigned char __x = ((unsigned char *) src_bp)[0];  \
        src_bp += 1;                        \
        __nbytes -= 1;                      \
        ((unsigned char *) dst_bp)[0] = __x;            \
        dst_bp += 1;                        \
    }                               \
} while (0)

/* Copy *up to* NBYTES bytes from SRC_BP to DST_BP, with
   the assumption that DST_BP is aligned on an OPSIZ multiple.  If
   not all bytes could be easily copied, store remaining number of bytes
   in NBYTES_LEFT, otherwise store 0.  */
/* extern void _wordcopy_fwd_aligned __P ((long int, long int, size_t)); */
/* extern void _wordcopy_fwd_dest_aligned __P ((long int, long int, size_t)); */
#define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)      \
do {                                    \
    if (src_bp % OPSIZ == 0)                    \
        _wordcopy_fwd_aligned(dst_bp, src_bp, (nbytes) / OPSIZ);\
    else                                \
        _wordcopy_fwd_dest_aligned(dst_bp, src_bp, (nbytes) / OPSIZ);\
    src_bp += (nbytes) & -OPSIZ;                    \
    dst_bp += (nbytes) & -OPSIZ;                    \
    (nbytes_left) = (nbytes) % OPSIZ;               \
} while (0)


/* Threshold value for when to enter the unrolled loops.  */
#define OP_T_THRES  16

/* _wordcopy_fwd_aligned -- Copy block beginning at SRCP to
   block beginning at DSTP with LEN `op_t' words (not LEN bytes!).
   Both SRCP and DSTP should be aligned for memory operations on `op_t's.  */
/* stream-lined (read x8 + write x8) */
static void _wordcopy_fwd_aligned( int dstp,  int srcp, size_t len)
{
    while (len > 7)
    {
        register op_t a0, a1, a2, a3, a4, a5, a6, a7;

        a0 = ((op_t *) srcp)[0];
        a1 = ((op_t *) srcp)[1];
        a2 = ((op_t *) srcp)[2];
        a3 = ((op_t *) srcp)[3];
        a4 = ((op_t *) srcp)[4];
        a5 = ((op_t *) srcp)[5];
        a6 = ((op_t *) srcp)[6];
        a7 = ((op_t *) srcp)[7];
        ((op_t *) dstp)[0] = a0;
        ((op_t *) dstp)[1] = a1;
        ((op_t *) dstp)[2] = a2;
        ((op_t *) dstp)[3] = a3;
        ((op_t *) dstp)[4] = a4;
        ((op_t *) dstp)[5] = a5;
        ((op_t *) dstp)[6] = a6;
        ((op_t *) dstp)[7] = a7;

        srcp += 8 * OPSIZ;
        dstp += 8 * OPSIZ;
        len -= 8;
    }
    while (len > 0)
    {
        *(op_t *)dstp = *(op_t *)srcp;

        srcp += OPSIZ;
        dstp += OPSIZ;
        len -= 1;
    }
}

/* _wordcopy_fwd_dest_aligned -- Copy block beginning at SRCP to
   block beginning at DSTP with LEN `op_t' words (not LEN bytes!).
   DSTP should be aligned for memory operations on `op_t's, but SRCP must
   *not* be aligned.  */
/* stream-lined (read x4 + write x4) */
static void _wordcopy_fwd_dest_aligned( int dstp,  int srcp,
                                        size_t len)
{
    op_t ap;
    int sh_1, sh_2;

    /* Calculate how to shift a word read at the memory operation
    aligned srcp to make it aligned for copy. */

    sh_1 = 8 * (srcp % OPSIZ);
    sh_2 = 8 * OPSIZ - sh_1;

    /* Make SRCP aligned by rounding it down to the beginning of the `op_t'
    it points in the middle of. */
    srcp &= -OPSIZ;
    ap = ((op_t *) srcp)[0];
    srcp += OPSIZ;

    while (len > 3)
    {
        op_t a0, a1, a2, a3;

        a0 = ((op_t *) srcp)[0];
        a1 = ((op_t *) srcp)[1];
        a2 = ((op_t *) srcp)[2];
        a3 = ((op_t *) srcp)[3];
        ((op_t *) dstp)[0] = MERGE(ap, sh_1, a0, sh_2);
        ((op_t *) dstp)[1] = MERGE(a0, sh_1, a1, sh_2);
        ((op_t *) dstp)[2] = MERGE(a1, sh_1, a2, sh_2);
        ((op_t *) dstp)[3] = MERGE(a2, sh_1, a3, sh_2);

        ap = a3;
        srcp += 4 * OPSIZ;
        dstp += 4 * OPSIZ;
        len -= 4;
    }
    while (len > 0)
    {
        register op_t a0;

        a0 = ((op_t *) srcp)[0];
        ((op_t *) dstp)[0] = MERGE(ap, sh_1, a0, sh_2);

        ap = a0;
        srcp += OPSIZ;
        dstp += OPSIZ;
        len -= 1;
    }
}

// =======================================================
// memcpy
// -------------------------------------------------------
/// Copy n bytes from src to dest
/// This implementation access to memory only with read 32 bits and write 32
/// bits. And use a buffer of 2 words for realign the destination
///
/// @param dstpp Destination buffer
/// @param srcpp Source buffer
/// @param len Copy size
// =======================================================
void *memcpy(void *dstpp, const void *srcpp, size_t len)
{
    unsigned  int dstp = ( int) dstpp;
    unsigned  int srcp = ( int) srcpp;

    /* Copy from the beginning to the end.  */

    /* If there not too few bytes to copy, use word copy.  */
    if (len >= OP_T_THRES)
    {
        /* Copy just a few bytes to make DSTP aligned.  */
        len -= (-dstp) % OPSIZ;
        BYTE_COPY_FWD(dstp, srcp, (-dstp) % OPSIZ);

        /* Copy whole pages from SRCP to DSTP by virtual address
           manipulation, as much as possible.  */

        /* PAGE_COPY_FWD_MAYBE (dstp, srcp, len, len); */

        /* Copy from SRCP to DSTP taking advantage of the known
           alignment of DSTP. Number of bytes remaining is put in the
           third argument, i.e. in LEN.  This number may vary from
           machine to machine. */

        WORD_COPY_FWD(dstp, srcp, len, len);

        /* Fall out and copy the tail. */
    }

    /* There are just a few bytes to copy.  Use byte memory operations. */
    BYTE_COPY_FWD(dstp, srcp, len);

    return dstpp;
}

void *memcpyb(void *dstpp, const void *srcpp, unsigned len)
{
    unsigned  int dstp = ( int) dstpp;
    unsigned  int srcp = ( int) srcpp;

    BYTE_COPY_FWD(dstp, srcp, len);

    return dstpp;
}
