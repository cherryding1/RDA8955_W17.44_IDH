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



# ifndef LIBMAD_DECODER_H
# define LIBMAD_DECODER_H

# include "stream.h"
# include "frame.h"
# include "synth.h"

enum mad_decoder_mode
{
    MAD_DECODER_MODE_SYNC  = 0,
    MAD_DECODER_MODE_ASYNC
};

enum mad_flow
{
    MAD_FLOW_CONTINUE = 0x0000,   /* continue normally */
    MAD_FLOW_STOP     = 0x0010,   /* stop decoding normally */
    MAD_FLOW_BREAK    = 0x0011,   /* stop decoding and signal an error */
    MAD_FLOW_IGNORE   = 0x0020    /* ignore the current frame */
};

struct syncs
{
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
};

struct mad_decoder
{
    void *error_data;
    int bad_last_frame;
    enum mad_decoder_mode mode;

    int options;

    struct
    {
        long pid;
        int in;
        int out;
    } async;

    struct syncs *sync;

//  struct {
//    struct mad_stream stream;
//    struct mad_frame frame;
//    struct mad_synth synth;
//  } *sync;

    void *cb_data;

    enum mad_flow (*input_func)(void *, struct mad_stream *);
    enum mad_flow (*header_func)(void *, struct mad_header const *);
    enum mad_flow (*filter_func)(void *,
                                 struct mad_stream const *, struct mad_frame *);
    enum mad_flow (*output_func)(void *,
                                 struct mad_header const *, struct mad_pcm *);
    enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
    enum mad_flow (*message_func)(void *, void *, unsigned int *);
};

void mad_decoder_init(struct mad_decoder *, void *,
                      enum mad_flow (*)(void *, struct mad_stream *),
                      enum mad_flow (*)(void *, struct mad_header const *),
//            enum mad_flow (*)(void *,
//                  struct mad_stream const *,
//                  struct mad_frame *),
//            enum mad_flow (*)(void *,
//                  struct mad_header const *,
//                  struct mad_pcm *),
                      enum mad_flow (*)(void *,
                                        struct mad_stream *,
                                        struct mad_frame *),
                      enum mad_flow (*)(void *, void *, unsigned int *));
int mad_decoder_finish(struct mad_decoder *);

# define mad_decoder_options(decoder, opts)  \
    ((void) ((decoder)->options = (opts)))
int run_sync_dwl(struct mad_decoder *decoder);
int mad_decoder_run(struct mad_decoder *, enum mad_decoder_mode);
int mad_decoder_message(struct mad_decoder *, void *, unsigned int *);
enum mad_flow error_default(void *data, struct mad_stream *stream,
                            struct mad_frame *frame);

# endif
