#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__
#include <cswtype.h>
#include "cfw.h"

#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
    typedef long         time_t;
#endif

#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
struct timespec {
        long long  tv_sec;                 /* seconds */
        long            tv_nsec;                /* nanoseconds */
};
#endif

struct timeval {
  long long    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif
