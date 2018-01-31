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

#include <pb_util.h>
#include <pb_decode.h>
#include <pb_encode.h>

bool pb_string_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    pb_string_decode_param_t *param = (pb_string_decode_param_t *)*arg;

    if (stream->bytes_left > param->cnt)
        return false;
    if (!pb_read(stream, param->data, stream->bytes_left))
        return false;
    return true;
}

bool pb_string_encode_callback(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    const pb_string_encode_param_t *param = (const pb_string_encode_param_t *)*arg;

    if (!pb_encode_tag_for_field(stream, field))
        return false;
    return pb_encode_string(stream, param->data, param->cnt);
}

bool pb_array_encode_callback(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    const pb_array_encode_param_t *param = (const pb_array_encode_param_t *)*arg;
    for (int idx = 0; idx < param->cnt; idx++)
    {
        if (!param->conv(idx, param))
            return false;
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        if (!pb_encode_submessage(stream, param->subfields, param->pbdata))
            return false;
    }
    return true;
}

bool pb_array_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    pb_array_decode_param_t *param = (pb_array_decode_param_t *)*arg;
    unsigned idx = param->idx++;

    if (idx >= param->cnt)
        return false;
    if (!pb_decode(stream, param->subfields, param->pbdata))
        return false;
    if (!param->conv(param))
        return false;
    return true;
}
