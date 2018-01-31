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



# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "mp3_global.h"

# include <stdlib.h>

# include "bit.h"
# include "stream.h"

/*
 * NAME:    stream->init()
 * DESCRIPTION: initialize stream struct
 */
void mad_stream_init(struct mad_stream *stream)
{
    stream->buffer     = 0;
    stream->bufend     = 0;
    stream->skiplen    = 0;

    stream->sync       = 0;
    stream->freerate   = 0;

    stream->this_frame = 0;
    stream->next_frame = 0;
    mad_bit_init(&stream->ptr, 0);

    mad_bit_init(&stream->anc_ptr, 0);
    stream->anc_bitlen = 0;

    stream->main_data  = 0;
    stream->md_len     = 0;

    stream->options    = 0;
    stream->error      = MAD_ERROR_NONE;
}

/*
 * NAME:    stream->finish()
 * DESCRIPTION: deallocate any dynamic memory associated with stream
 */
void mad_stream_finish(struct mad_stream *stream)
{
    if (stream->main_data)
    {
//    free(stream->main_data);
        stream->main_data = 0;
    }

    mad_bit_finish(&stream->anc_ptr);
    mad_bit_finish(&stream->ptr);
}

/*
 * NAME:    stream->buffer()
 * DESCRIPTION: set stream buffer pointers
 */
void mad_stream_buffer(struct mad_stream *stream,
                       unsigned char const *buffer, unsigned long length)
{
    stream->buffer = buffer;
    stream->bufend = buffer + length;

    stream->this_frame = buffer;
    stream->next_frame = buffer;

    stream->sync = 1;

    mad_bit_init(&stream->ptr, buffer);
}

#if 0
enum mad_flow decode_input_read_dwl(void *data, struct mad_stream *stream)
{
    struct player *player = data;
    struct input *input = &player->input;
    int len;

    if (input->eof)
        return MAD_FLOW_STOP;

    if (stream->next_frame)
    {
        memmove(input->data, stream->next_frame,
                input->length = &input->data[input->length] - stream->next_frame);
    }

    do
    {
        len = read(input->fd, input->data + input->length,
                   MPEG_BUFSZ - input->length);
    }
    while (len == -1 && errno == EINTR);

    if (len == -1)
    {
        error("input", ":read");
        return MAD_FLOW_BREAK;
    }
    else if (len == 0)
    {
        input->eof = 1;

        assert(MPEG_BUFSZ - input->length >= MAD_BUFFER_GUARD);

        while (len < MAD_BUFFER_GUARD)
            input->data[input->length + len++] = 0;
    }

    mad_stream_buffer(stream, input->data, input->length += len);

    return MAD_FLOW_CONTINUE;
}
#endif

/*
 * NAME:    stream->skip()
 * DESCRIPTION: arrange to skip bytes before the next frame
 */
void mad_stream_skip(struct mad_stream *stream, unsigned long length)
{
    stream->skiplen += length;
}

/*
 * NAME:    stream->sync()
 * DESCRIPTION: locate the next stream sync word
 */
int mad_stream_sync(struct mad_stream *stream)
{
    register unsigned char const *ptr, *end;

    ptr = mad_bit_nextbyte(&stream->ptr);
    end = stream->bufend;

    while (ptr < end - 1 &&
            !(ptr[0] == 0xff && (ptr[1] & 0xe0) == 0xe0))
        ++ptr;

    if (end - ptr < MAD_BUFFER_GUARD)
        return -1;

    mad_bit_init(&stream->ptr, ptr);

    return 0;
}

/*
 * NAME:    stream->errorstr()
 * DESCRIPTION: return a string description of the current error condition
 */
char const *mad_stream_errorstr(struct mad_stream const *stream)
{
    switch (stream->error)
    {
        case MAD_ERROR_NONE:       return "no error";

        case MAD_ERROR_BUFLEN:     return "input buffer too small (or EOF)";
        case MAD_ERROR_BUFPTR:     return "invalid (null) buffer pointer";

        case MAD_ERROR_NOMEM:      return "not enough memory";

        case MAD_ERROR_LOSTSYNC:   return "lost synchronization";
        case MAD_ERROR_BADLAYER:   return "reserved header layer value";
        case MAD_ERROR_BADBITRATE:     return "forbidden bitrate value";
        case MAD_ERROR_BADSAMPLERATE:  return "reserved sample frequency value";
        case MAD_ERROR_BADEMPHASIS:    return "reserved emphasis value";

        case MAD_ERROR_BADCRC:     return "CRC check failed";
        case MAD_ERROR_BADBITALLOC:    return "forbidden bit allocation value";
        case MAD_ERROR_BADSCALEFACTOR: return "bad scalefactor index";
        case MAD_ERROR_BADFRAMELEN:    return "bad frame length";
        case MAD_ERROR_BADBIGVALUES:   return "bad big_values count";
        case MAD_ERROR_BADBLOCKTYPE:   return "reserved block_type";
        case MAD_ERROR_BADSCFSI:   return "bad scalefactor selection info";
        case MAD_ERROR_BADDATAPTR:     return "bad main_data_begin pointer";
        case MAD_ERROR_BADPART3LEN:    return "bad audio data length";
        case MAD_ERROR_BADHUFFTABLE:   return "bad Huffman table select";
        case MAD_ERROR_BADHUFFDATA:    return "Huffman data overrun";
        case MAD_ERROR_BADSTEREO:  return "incompatible block_type for JS";
    }

    return 0;
}
