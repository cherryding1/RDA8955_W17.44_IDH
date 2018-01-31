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

#if !defined(PB_UTILS_HEADER)
#define PB_UTILS_HEADER

#include <pb.h>

typedef struct pb_array_decode_param_s pb_array_decode_param_t;
struct pb_array_decode_param_s
{
    void *data;                  // pointer to the address of first submsg
    unsigned cnt;                // max submsg count, for check
    unsigned idx;                // currrent submsg index
    const pb_field_t *subfields; // submsg description
    bool (*conv)(pb_array_decode_param_t *param);
    void *pbdata; // memory to hold pbdata
};

typedef struct pb_array_encode_param_s pb_array_encode_param_t;
struct pb_array_encode_param_s
{
    const void *data;            // pointer to the address of first submsg
    unsigned cnt;                // max submsg count, for loop
    const pb_field_t *subfields; // submsg description
    bool (*conv)(unsigned idx, const pb_array_encode_param_t *param);
    void *pbdata; // memory to hold pbdata
};

bool pb_array_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool pb_array_encode_callback(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);

typedef struct pb_string_decode_param_s pb_string_decode_param_t;
struct pb_string_decode_param_s
{
    void *data;
    unsigned cnt;
};

typedef struct pb_string_encode_param_s pb_string_encode_param_t;
struct pb_string_encode_param_s
{
    const void *data;
    unsigned cnt;
};

bool pb_string_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool pb_string_encode_callback(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);

#endif /* PB_UTILS_HEADER */
