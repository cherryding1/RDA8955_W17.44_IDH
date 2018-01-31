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



#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H
#define GOEP_LOCAL_MTU     (0X3800)
/********************************** Base Types ***************************************************/
typedef unsigned char   u_int8;       /* unsigned integer,  8 bits long */
typedef unsigned short  u_int16;     /* unsigned integer, 16 bits long */
typedef unsigned int    u_int32;       /* unsigned integer, 32 bits long */
//typedef unsigned char boolean;   /* boolean, true=1, false=0 */

typedef signed char     s_int8;         /* unsigned integer,  8 bits long */
typedef signed short    s_int16;       /* unsigned integer,  8 bits long */
typedef signed long     s_int32;        /* unsigned integer,  8 bits long */

typedef unsigned int    u_int;

/********************************** Host Stacks ***************************************************/
/* All configurable host stack parameters should be set in this file, with a descriptive comment */
/* All preprocessor symbols should be defined as 1 or 0, to include or not include the feature */
/* All timer values are in tenths of a second, i.e. 400 - 40 seconds */

/****************** Features ***************/
#define RDABT_SSP_SUPPORT        1
#define RDABT_LE_SUPPORT        0
#define RDABT_EIR_SUPPORT        0
/* for bt 3.0 */
#define AMP_MANAGER_SUPPORT                 0
#define PAL_IN_HOST                         0

#define RDABT_THREAD_EVENT_SUPPORT        0        // event for call bt profile from other thread
#define COMBINED_HOST                       0

/* memory pool */
#define MEMORY_POOL_SUPPORT         0
#define MEMORY_POOL_SIZE            256
#define HOST_MEM_COLLECT        0

/******************* Manager ***************/
/* The local and remote names are truncated to the defines below - see GAP spec. */
#define MGR_MAX_REMOTE_NAME_LEN         30
#define MGR_MAX_LOCAL_NAME_LEN          48

/****************** L2CAP ****************/
#define L2CAP_CONFIG_VALUES_BELOW_HCI       1
#define L2CAP_FRAGMENT_ACL_SUPPORT          1
#define L2CAP_RETRANS_SUPPORT                   0

/*************** Test and Debug *************/
/* Debug option */
#define pDEBUG                              1//0524 

#if pDEBUG
#define pDEBUGLEVEL                         pLOGNOTICE
#define pDEBUGLAYERS                        pLOGALWAYS
#endif

/*************************************** platform sepcific defines ************************************************/
#ifdef APIDECL1
#undef APIDECL1
#endif

#ifdef APIDECL2
#undef APIDECL2
#endif

#define APIDECL1
#define APIDECL2

#define PLATFORMSTRING "COOLSAND"

/* define endian of platform */

#define BTHOST_BIGENDIAN 0

#define NO_SEC_CHECK

#endif /* PLATFORM_CONFIG_H */

