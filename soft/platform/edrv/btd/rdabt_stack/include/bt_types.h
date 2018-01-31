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



#ifndef RDABT_HOST_TYPES_H
#define RDABT_HOST_TYPES_H
#include "platform_config.h"
/* API function return code */
typedef int t_api;

typedef u_int16 t_channelId;

/* header sizes to reserve at each layer */

#define RDABT_HOST_BUF_HCI_HEADER_PRESENT 0x0001
#define RDABT_HOST_BUF_USE_RESV_ACL_MEM   0x0002
/* is this packet a packet allocated on the way out or way in? */
/* 1 is a packet from the queues */
#define RDABT_HOST_BUF_OUTGOING_PACKET    0x0004

#define RDABT_HOST_BUF_URGENCY_PACKET       0x0010

#define RDABT_HANDLE_FLAG_PICONET_BROADCAST 0x8000
#define RDABT_HANDLE_FLAG_ACTIVE_BROADCAST  0x4000
#define RDABT_HANDLE_FLAG_START_FRAGMENT  0x2000
#define RDABT_HANDLE_FLAG_CONTINUE_FRAGMENT  0x1000


/* data transport buffer */
typedef struct st_t_dataBuf
{
    u_int8 *buf;
    u_int8 transport;
    u_int16 dataLen;
    u_int16 bufLen;
} t_DataBuf;

/*
    NB:  The bytes in the bdaddr structure are ordered with the LSB at bytes[0].
    e.g. u_int8 bytes[] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA } represents the
    device address 0xAABBCCDDEEFF.  This is the order which the HCI PDU parsing uses.
    Similar ordering is used for link keys.
*/
#define SIZE_OF_BDADDR 6
typedef struct
{
    u_int8 bytes[SIZE_OF_BDADDR];
} t_bdaddr;

#define LINKKEY_SIZE 16

#define MAX_PIN_LEN 16
#define DEFAULT_FLUSH_TIMEOUT        0x0000
#define MAX_FLUSH_TIMEOUT            0x07FF
#define DEFAULT_LINK_SUP_TIMEOUT     0x7D00
#define MAX_DEFAULT_LINK_SUP_TIMEOUT 0xFFFF

#ifndef RDABT_COMBINED_STACK_COMMON_TYPES
#define RDABT_COMBINED_STACK_COMMON_TYPES 1
typedef u_int32 t_classDevice;
#endif

#ifndef BD_ADDR_CMP /* An optimised version may be defined in platform_operations.h */
#    define BD_ADDR_CMP(x,y) !pMemcmp(((x).bytes), ((y).bytes), SIZE_OF_BDADDR)
#endif


#ifndef BD_ADDR_TO_UINT8_CPY
#define  BD_ADDR_TO_UINT8_CPY(x,y) pMemcpy((x),((y).bytes),SIZE_OF_BDADDR)
#endif


/*
    This is used as a callback identifier - it is passed into a service primitive, and the callback
    carries this value to identify the service primitive which it is paired with
*/
typedef void *tid_t;

/* Spec defined release reasons */

#define RDABT_USER_ENDED_CONNECTION 0x13
#define RDABT_LOW_RESOURCES         0x14
#define RDABT_ABOUT_TO_POWER_OFF        0x15
#define RDABT_TERMINATED_BY_LOCAL_HOST 0x16

#endif /* HOST_TYPES_H */

