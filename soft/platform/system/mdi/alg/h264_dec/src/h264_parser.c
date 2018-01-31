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





#include "parser.h"
#include "h264_parser.h"

#include <assert.h>


int ff_h264_find_frame_end(H264Context *h, const uint8_t *buf, int buf_size)
{
    int i;
    uint32_t state;
    ParseContext *pc = &(h->s.parse_context);
//printf("first %02X%02X%02X%02X\n", buf[0], buf[1],buf[2],buf[3]);
//    mb_addr= pc->mb_addr - 1;
    state= pc->state;
    if(state>13)
        state= 7;

    for(i=0; i<buf_size; i++)
    {
        if(state==7)
        {
            for(; i<buf_size; i++)
            {
                if(!buf[i])
                {
                    state=2;
                    break;
                }
            }
        }
        else if(state<=2)
        {
            if(buf[i]==1)   state^= 5; //2->7, 1->4, 0->5
            else if(buf[i]) state = 7;
            else            state>>=1; //2->1, 1->0, 0->0
        }
        else if(state<=5)
        {
            int v= buf[i] & 0x1F;
            if(v==7 || v==8 || v==9)
            {
                if(pc->frame_start_found)
                {
                    i++;
found:
                    pc->state=7;
                    pc->frame_start_found= 0;
                    return i-(state&5);
                }
            }
            else if(v==1 || v==2 || v==5)
            {
                if(pc->frame_start_found)
                {
                    state+=8;
                    continue;
                }
                else
                    pc->frame_start_found = 1;
            }
            state= 7;
        }
        else
        {
            if(buf[i] & 0x80)
                goto found;
            state= 7;
        }
    }
    pc->state= state;
    return END_NOT_FOUND;
}

static int h264_parse(AVCodecParserContext *s,
                      AVCodecContext *avctx,
                      const uint8_t **poutbuf, int *poutbuf_size,
                      const uint8_t *buf, int buf_size)
{
    H264Context *h = s->priv_data;
    ParseContext *pc = &h->s.parse_context;
    int next;

    if(s->flags & PARSER_FLAG_COMPLETE_FRAMES)
    {
        next= buf_size;
    }
    else
    {
        next= ff_h264_find_frame_end(h, buf, buf_size);

        if (ff_combine_frame(pc, next, &buf, &buf_size) < 0)
        {
            *poutbuf = NULL;
            *poutbuf_size = 0;
            return buf_size;
        }

        if(next<0 && next != END_NOT_FOUND)
        {
            assert(pc->last_index + next >= 0 );
            ff_h264_find_frame_end(h, &pc->buffer[pc->last_index + next], -next); //update state
        }
    }

    *poutbuf = buf;
    *poutbuf_size = buf_size;
    return next;
}

static int h264_split(AVCodecContext *avctx,
                      const uint8_t *buf, int buf_size)
{
    int i;
    uint32_t state = -1;
    int has_sps= 0;

    for(i=0; i<=buf_size; i++)
    {
        if((state&0xFFFFFF1F) == 0x107)
            has_sps=1;
        /*        if((state&0xFFFFFF1F) == 0x101 || (state&0xFFFFFF1F) == 0x102 || (state&0xFFFFFF1F) == 0x105){
                }*/
        if((state&0xFFFFFF00) == 0x100 && (state&0xFFFFFF1F) != 0x107 && (state&0xFFFFFF1F) != 0x108 && (state&0xFFFFFF1F) != 0x109)
        {
            if(has_sps)
            {
                while(i>4 && buf[i-5]==0) i--;
                return i-4;
            }
        }
        if (i<buf_size)
            state= (state<<8) | buf[i];
    }
    return 0;
}


AVCodecParser h264_parser =
{
    { CODEC_ID_H264 },
    sizeof(H264Context),
    NULL,
    h264_parse,
#ifndef MEDIA_VOCVID_SUPPORT
    ff_parse_close,
#else
    NULL,
#endif// MEDIA_VOCVID_SUPPORT
    h264_split,
};
