#ifndef __PTHREAD_H__
#define __PTHREAD_H__
#include "lwip/sys.h"
#include "sxr_mutx.h"

#define pthread_mutex_t sys_mutex_t
#define pthread_cond_t sys_mutex_t
#define PTHREAD_MUTEX_INITIALIZER {.id = 0xff}
#define pthread_mutex_lock(a) do {if ((sys_mutex_t *)(a)->id == 0xff) sys_mutex_new(a); sys_mutex_lock(a);} while(0);
#define pthread_mutex_unlock sys_mutex_unlock

#define pthread_t HANDLE
#endif
