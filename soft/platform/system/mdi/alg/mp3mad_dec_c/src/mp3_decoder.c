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


# include "stream.h"
# include "frame.h"
# include "synth.h"
# include "decoder.h"

#if 0
#include "cswtype.h"
#include "ts.h"


//API for cool profile

#define u16 UINT16
#define u8   UINT8
extern void hal_PXTS_SendProfilingCode(u16 code);
#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define CP_ENTRY(id) hal_PXTS_SendProfilingCode((id) & CPMASK)
#define CP_EXIT(id) hal_PXTS_SendProfilingCode(((id) & CPMASK) | CPEXITFLAG)

#endif


void mad_decoder_init(struct mad_decoder *decoder, void *data,
                      enum mad_flow (*input_func)(void *,
                              struct mad_stream *),
                      enum mad_flow (*header_func)(void *,
                              struct mad_header const *),
//            enum mad_flow (*filter_func)(void *,
//                         struct mad_stream const *,
//                         struct mad_frame *),
//            enum mad_flow (*output_func)(void *,
//                         struct mad_header const *,
//                         struct mad_pcm *),
                      enum mad_flow (*error_func)(void *,
                              struct mad_stream *,
                              struct mad_frame *),
                      enum mad_flow (*message_func)(void *,
                              void *, unsigned int *))
{
    decoder->mode         = -1;

    decoder->options      = 0;

    decoder->async.pid    = 0;
    decoder->async.in     = -1;
    decoder->async.out    = -1;

    decoder->sync         = 0;

    decoder->cb_data      = data;

    decoder->input_func   = input_func;
    decoder->header_func  = header_func;
    decoder->filter_func  =0; // filter_func;
    decoder->output_func  =0; // output_func;
    decoder->error_func   = error_func;
    decoder->message_func = message_func;
}

int mad_decoder_finish(struct mad_decoder *decoder)
{
# if defined(USE_ASYNC)
    if (decoder->mode == MAD_DECODER_MODE_ASYNC && decoder->async.pid)
    {
        pid_t pid;
        int status;

        close(decoder->async.in);

        do
            pid = waitpid(decoder->async.pid, &status, 0);
        while (pid == -1 && errno == EINTR);

        decoder->mode = -1;

        close(decoder->async.out);

        decoder->async.pid = 0;
        decoder->async.in  = -1;
        decoder->async.out = -1;

        if (pid == -1)
            return -1;

        return (!WIFEXITED(status) || WEXITSTATUS(status)) ? -1 : 0;
    }
# endif

    return 0;
}

# if defined(USE_ASYNC)
static
enum mad_flow send_io(int fd, void const *data, size_t len)
{
    char const *ptr = data;
    ssize_t count;

    while (len)
    {
        do
            count = write(fd, ptr, len);
        while (count == -1 && errno == EINTR);

        if (count == -1)
            return MAD_FLOW_BREAK;

        len -= count;
        ptr += count;
    }

    return MAD_FLOW_CONTINUE;
}

static
enum mad_flow receive_io(int fd, void *buffer, size_t len)
{
    char *ptr = buffer;
    ssize_t count;

    while (len)
    {
        do
            count = read(fd, ptr, len);
        while (count == -1 && errno == EINTR);

        if (count == -1)
            return (errno == EAGAIN) ? MAD_FLOW_IGNORE : MAD_FLOW_BREAK;
        else if (count == 0)
            return MAD_FLOW_STOP;

        len -= count;
        ptr += count;
    }

    return MAD_FLOW_CONTINUE;
}

static
enum mad_flow receive_io_blocking(int fd, void *buffer, size_t len)
{
    int flags, blocking;
    enum mad_flow result;

    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        return MAD_FLOW_BREAK;

    blocking = flags & ~O_NONBLOCK;

    if (blocking != flags &&
            fcntl(fd, F_SETFL, blocking) == -1)
        return MAD_FLOW_BREAK;

    result = receive_io(fd, buffer, len);

    if (flags != blocking &&
            fcntl(fd, F_SETFL, flags) == -1)
        return MAD_FLOW_BREAK;

    return result;
}

static
enum mad_flow send(int fd, void const *message, unsigned int size)
{
    enum mad_flow result;

    /* send size */

    result = send_io(fd, &size, sizeof(size));

    /* send message */

    if (result == MAD_FLOW_CONTINUE)
        result = send_io(fd, message, size);

    return result;
}

static
enum mad_flow receive(int fd, void **message, unsigned int *size)
{
    enum mad_flow result;
    unsigned int actual;

    if (*message == 0)
        *size = 0;

    /* receive size */

    result = receive_io(fd, &actual, sizeof(actual));

    /* receive message */

    if (result == MAD_FLOW_CONTINUE)
    {
        if (actual > *size)
            actual -= *size;
        else
        {
            *size  = actual;
            actual = 0;
        }

        if (*size > 0)
        {
            if (*message == 0)
            {
                *message = malloc(*size);
                if (*message == 0)
                    return MAD_FLOW_BREAK;
            }

            result = receive_io_blocking(fd, *message, *size);
        }

        /* throw away remainder of message */

        while (actual && result == MAD_FLOW_CONTINUE)
        {
            char sink[256];
            unsigned int len;

            len = actual > sizeof(sink) ? sizeof(sink) : actual;

            result = receive_io_blocking(fd, sink, len);

            actual -= len;
        }
    }

    return result;
}

static
enum mad_flow check_message(struct mad_decoder *decoder)
{
    enum mad_flow result;
    void *message = 0;
    unsigned int size;

    result = receive(decoder->async.in, &message, &size);

    if (result == MAD_FLOW_CONTINUE)
    {
        if (decoder->message_func == 0)
            size = 0;
        else
        {
            result = decoder->message_func(decoder->cb_data, message, &size);

            if (result == MAD_FLOW_IGNORE ||
                    result == MAD_FLOW_BREAK)
                size = 0;
        }

        if (send(decoder->async.out, message, size) != MAD_FLOW_CONTINUE)
            result = MAD_FLOW_BREAK;
    }

    if (message)
        free(message);

    return result;
}
# endif


enum mad_flow error_default(void *data, struct mad_stream *stream,
                            struct mad_frame *frame)
{
    int *bad_last_frame = data;

    switch (stream->error)
    {
        case MAD_ERROR_BADCRC:
            if (*bad_last_frame)
                mad_frame_mute(frame);
            else
                *bad_last_frame = 1;

            return MAD_FLOW_IGNORE;

        default:
            return MAD_FLOW_CONTINUE;
    }
}



int mad_decoder_message(struct mad_decoder *decoder,
                        void *message, unsigned int *len)
{

    return -1;

}
