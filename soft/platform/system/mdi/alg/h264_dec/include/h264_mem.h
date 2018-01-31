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





#ifndef FFMPEG_MEM_H
#define FFMPEG_MEM_H

#ifdef __ICC
#define DECLARE_ALIGNED(n,t,v)      t v __attribute__ ((aligned (n)))
#define DECLARE_ASM_CONST(n,t,v)    const t __attribute__ ((aligned (n))) v
#elif defined(__GNUC__)
#define DECLARE_ALIGNED(n,t,v)      t v __attribute__ ((aligned (n)))
#define DECLARE_ASM_CONST(n,t,v)    static const t v attribute_used __attribute__ ((aligned (n)))
#elif defined(_MSC_VER)
#define DECLARE_ALIGNED(n,t,v)      __declspec(align(n)) t v
#define DECLARE_ASM_CONST(n,t,v)    __declspec(align(n)) static const t v
#elif defined(HAVE_INLINE_ASM)
#error The asm code needs alignment, but we do not know how to do it for this compiler.
#else
#define DECLARE_ALIGNED(n,t,v)      t v
#define DECLARE_ASM_CONST(n,t,v)    static const t v
#endif

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define av_malloc_attrib __attribute__((__malloc__))
#else
#define av_malloc_attrib
#endif

#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 2)
#define av_alloc_size(n) __attribute__((alloc_size(n)))
#else
#define av_alloc_size(n)
#endif

/**
 * Allocate a block of \p size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU).
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if it cannot allocate
 * it.
 * @see av_mallocz()
 */
void *av_malloc(unsigned int size) av_malloc_attrib av_alloc_size(1);

/**
 * Allocate or reallocate a block of memory.
 * If \p ptr is NULL and \p size > 0, allocate a new block. If \p
 * size is zero, free the memory block pointed by \p ptr.
 * @param size Size in bytes for the memory block to be allocated or
 * reallocated.
 * @param ptr Pointer to a memory block already allocated with
 * av_malloc(z)() or av_realloc() or NULL.
 * @return Pointer to a newly reallocated block or NULL if it cannot
 * reallocate or the function is used to free the memory block.
 * @see av_fast_realloc()
 */
void *av_realloc(void *ptr, unsigned int size) av_alloc_size(2);

/**
 * Free a memory block which has been allocated with av_malloc(z)() or
 * av_realloc().
 * @param ptr Pointer to the memory block which should be freed.
 * @note ptr = NULL is explicitly allowed.
 * @note It is recommended that you use av_freep() instead.
 * @see av_freep()
 */
void av_free(void *ptr);

/**
 * Allocate a block of \p size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU) and
 * set to zeroes all the bytes of the block.
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if it cannot allocate
 * it.
 * @see av_malloc()
 */
void *av_mallocz(unsigned int size) av_malloc_attrib av_alloc_size(1);

/**
 * Duplicate the string \p s.
 * @param s String to be duplicated.
 * @return Pointer to a newly allocated string containing a
 * copy of \p s or NULL if it cannot be allocated.
 */
char *av_strdup(const char *s) av_malloc_attrib;

/**
 * Free a memory block which has been allocated with av_malloc(z)() or
 * av_realloc() and set to NULL the pointer to it.
 * @param ptr Pointer to the pointer to the memory block which should
 * be freed.
 * @see av_free()
 */
void av_freep(void *ptr);

int h264_memory_init();//sheen
void h264_memory_free_all();//sheen

#endif /* FFMPEG_MEM_H */
