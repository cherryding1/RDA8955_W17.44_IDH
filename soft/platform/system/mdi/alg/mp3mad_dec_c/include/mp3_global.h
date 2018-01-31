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



# ifndef LIBMAD_GLOBAL_H
# define LIBMAD_GLOBAL_H

#ifdef SHEEN_VC_DEBUG
#include <stdlib.h>
#include <stdio.h>
#define inline __inline
#define mmc_MemMalloc malloc
#define mmc_MemFreeAll()
#endif
#define memset32(a,b,c) memset(a,b,(c<<2))

/* conditional debugging */

# if defined(DEBUG) && defined(NDEBUG)
#  error "cannot define both DEBUG and NDEBUG"
# endif

# if defined(DEBUG)
#  include <stdio.h>
# endif

/* conditional features */

# if defined(OPT_SPEED) && defined(OPT_ACCURACY)
#  error "cannot optimize for both speed and accuracy"
# endif

# if defined(OPT_SPEED) && !defined(OPT_SSO)
#  define OPT_SSO 1
# endif

# if defined(HAVE_UNISTD_H) && defined(HAVE_WAITPID) &&  \
    defined(HAVE_FCNTL) && defined(HAVE_PIPE) && defined(HAVE_FORK)
#  define USE_ASYNC
# endif

# if !defined(HAVE_ASSERT_H)
#  ifdef WIN32
#   define assert(x)
#  else
#   define assert(x)
#  endif
# endif

# endif
