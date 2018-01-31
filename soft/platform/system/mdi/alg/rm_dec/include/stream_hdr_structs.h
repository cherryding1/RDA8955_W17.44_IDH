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



#ifndef STREAM_HDR_STRUCTS_H
#define STREAM_HDR_STRUCTS_H

#include "helix_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* map an ASM rule to some other property */
typedef struct rm_rule_map_struct
{
    UINT32  ulNumRules;
    UINT32* pulMap;
} rm_rule_map;

/* rm multistream header -- surestream */
typedef struct rm_multistream_hdr_struct
{
    UINT32             ulID;            /* unique identifier for this header */
    rm_rule_map rule2SubStream;  /* mapping of ASM rule number to substream */
    UINT32             ulNumSubStreams; /* number of substreams */
} rm_multistream_hdr;

#define RM_MULTIHEADER_OBJECT 0x4D4C5449 /* 'MLTI' */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef STREAM_HDR_STRUCTS_H */
