#ifndef _TIME_H
#define _TIME_H
#include "sys/time.h"
#define useconds_t UINT32
#define usleep(x) COS_Sleep(x/1000)
#endif


