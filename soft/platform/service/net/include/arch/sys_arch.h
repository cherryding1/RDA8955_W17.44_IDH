/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef __TCPIP_ARCH_SYS_H__
#define __TCPIP_ARCH_SYS_H__
#include "lwip/opt.h"
#include "cos.h"
#include "hal_sys.h"

extern int errno;
#define set_errno(err) errno = (err)

typedef struct
{
	u8_t sem_id;
	u8_t is_vail;
	u8_t is_timeout;
	u8_t wait_count;  //if > 0 someone is wait for this sem
}sys_sem_t, *sys_sem_ptr;

typedef struct
{
	u8_t id;
    u8_t is_vail;
}sys_mbox_t,*sys_mbox_ptr;

typedef struct {
    u8_t id;
    u8_t user_id;
} sys_mutex_t;

#define  sys_thread_t  HANDLE
#define  sys_prot_t		u32_t

#define sys_msleep COS_Sleep

#define SYS_ARCH_DECL_PROTECT(x) UINT32 x
#define SYS_ARCH_PROTECT(x)		   x = hal_SysEnterCriticalSection()
#define SYS_ARCH_UNPROTECT(x)	  hal_SysExitCriticalSection(x)

#endif /* __TCPIP_ARCH_SYS_H__ */


