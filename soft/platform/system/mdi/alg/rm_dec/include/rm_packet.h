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



#ifndef RM_PACKET_H
#define RM_PACKET_H

#include "helix_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/*
 * Packet struct
 *
 * Users are strongly encouraged to use the
 * accessor functions below to retrieve information
 * from the packet, since the definition of this
 * struct may change in the future.
 */
typedef struct rm_packet_struct
{
    UINT32 ulTime;
    UINT16 usStream;
    UINT16 usASMFlags;
    BYTE   ucASMRule;
    BYTE   ucLost;
    UINT16 usDataLen;
    BYTE*  pData;
} rm_packet;

/*
 * Packet Accessor functions
 */
UINT32 rm_packet_get_timestamp(rm_packet* packet);
UINT32 rm_packet_get_stream_number(rm_packet* packet);
UINT32 rm_packet_get_asm_flags(rm_packet* packet);
UINT32 rm_packet_get_asm_rule_number(rm_packet* packet);
UINT32 rm_packet_get_data_length(rm_packet* packet);
BYTE*  rm_packet_get_data(rm_packet* packet);
HXBOOL rm_packet_is_lost(rm_packet* packet);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RM_PACKET_H */
