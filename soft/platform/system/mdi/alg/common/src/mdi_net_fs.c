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



#include "mdi_alg_common.h"
#ifndef SHEEN_VC_DEBUG
#include "fs.h"
#include "mcip_debug.h"
#include "cos.h"
#include "string.h"
#endif

static INT32 net_fs_pos;//file offset from start position
static INT32 net_fs_size;//whole file bytes size.
#if 0
extern char * mmc_MemMalloc(int32 nsize);
extern VOID *memcpy(VOID *dest, CONST VOID *src, UINT32 n);
#endif

#ifdef SHEEN_VC_DEBUG
int temp_getfilesize(int handle)
{
    int res;
    fseek(handle,0,SEEK_END);
    res=ftell(handle);
    return;
}
#define COS_MALLOC malloc
#define COS_FREE free
#endif

#ifndef SHEEN_VC_DEBUG
#ifdef __J2ME_VM__
extern int localplayer_read_vstreamdata(void* handle,unsigned char  *buffer,long offset,long readsize);
extern int localplayer_query_wholecontentsize(void);

#else
int localplayer_read_vstreamdata(int handle,unsigned char  *buffer,long offset,long readsize)
{
    return 0;
}
int localplayer_query_wholecontentsize(void)
{
    return 10*1024*1024;
}
#endif
#else
int localplayer_read_vstreamdata(int handle,int offset, char* pout,int size)//debug
{
    int res;
    fseek(handle,offset,FS_SEEK_SET);
    res=fread(pout,1,size, handle);
    return res;
}
int localplayer_query_wholecontentsize(void)
{
    return 10*1024*1024;
}
#endif

/*
return first pack size need for head parse.
size from file start position
sheen
20120424
*/
INT32 http_fs_parsehead(INT32 handle)
{
    INT32 res=-1;
    INT32 offset=0;
    INT32 temp=0;
    while(1)
    {
        //find 3gp,mp4 moov
        if(localplayer_read_vstreamdata(handle,(unsigned char*)&temp,offset, 4)!=4)
        {
            res=-1;
            break;
        }
        res= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;

        if(localplayer_read_vstreamdata(handle,(unsigned char*)&temp,offset+4, 4)!=4)
        {
            res=-1;
            break;
        }

        offset+=res;

        res= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;
        res<<=8;
        temp>>=8;
        res|= temp&0xff;

        if(res=='moov')
        {
            res=offset+1024;//moov and 1k mdat
            break;
        }
    }

    return res;
}

/*
init http fs
sheen
20120424
*/
void http_fs_init(INT32 handle)
{
    net_fs_pos=0;
    net_fs_size=localplayer_query_wholecontentsize();//10*1024*1024;//temp_getfilesize(handle);
}

/*
read data from http temp file
sheen
20120424
*/
INT32 http_fs_read(INT32 handle, UINT8 *pBuf, UINT32 iLen)
{
    INT32 res=0;

    //read from temp download file or net
    res= localplayer_read_vstreamdata(handle, pBuf,net_fs_pos, iLen);

    if(res<=0)
    {
        res=0;
    }
    else
    {
        net_fs_pos+=res;
    }

    return res;
}

/*
set file offset.
sheen
20120424
*/
INT32 http_fs_seek(INT32 handle, INT32 iOffset, UINT8 iOrigin)
{
    //INT32 res=-1;

    switch ( iOrigin )
    {
        case FS_SEEK_SET:
            net_fs_pos=iOffset;
            break;
        case FS_SEEK_CUR:
            net_fs_pos+=iOffset;
            break;
        case FS_SEEK_END:
            net_fs_pos=net_fs_size -iOffset;
            break;
        default:
            return -1;
    }

    if(net_fs_pos<0)
        net_fs_pos = 0;
    else if(net_fs_pos>net_fs_size)
        net_fs_pos=net_fs_size;

    return net_fs_pos;
}

/*
get file size.
sheen
20120424
*/
INT32 http_fs_getFileSize(INT32 handle)
{
    return net_fs_size;
}

/*
is end of the file?
sheen
20120424
*/
INT32 http_fs_isEndOfFile(INT32 handle)
{
    INT32 res=0;

    if(net_fs_pos>=net_fs_size)
        res=1;
    else
        res=0;

    return res;
}


/****************************************************************
auidio/video stream system
*****************************************************************/

#define AVSTREAM_MIRROR_SIZE 1024
#define AVSTREAM_A_RINGBUF_SIZE (1024*8)
#define AVSTREAM_V_RINGBUF_SIZE (1024*80)
#define AVSTREAM_SPACE 4


/****************************************************************
follow functions uesed by data fill task
*****************************************************************/

/*
audio/video ringbuf system create.
avmode=0, create audio ringbuf with mirror area at head and tail,
avmode=1, create video ringbuf.
must be call before video player open, and fill the buf after create.
sheen
20120425
*/
avstreamstr *avstream_srv_create(INT32 avmode)
{
    avstreamstr *avstreamhdl=0;

    avstreamhdl=(avstreamstr*)COS_MALLOC(sizeof(avstreamstr));

    if(avstreamhdl)
    {
        memset(avstreamhdl, 0, sizeof(avstreamstr));
        if(avmode==0)//with mirror area
        {
            avstreamhdl->ringbufptr=(unsigned char*)COS_MALLOC(AVSTREAM_A_RINGBUF_SIZE+AVSTREAM_MIRROR_SIZE*2);
            avstreamhdl->avstreamhdl_agent=(avagentstr*)COS_MALLOC(sizeof(avagentstr));
            if(avstreamhdl->ringbufptr && avstreamhdl->avstreamhdl_agent)
            {
                memset(avstreamhdl->avstreamhdl_agent, 0, sizeof(avagentstr));
                avstreamhdl->ringbuflen=AVSTREAM_A_RINGBUF_SIZE;
                avstreamhdl->rdptr=avstreamhdl->wtptr=avstreamhdl->ringbufptr+AVSTREAM_MIRROR_SIZE;
                avstreamhdl->mirrorlen=AVSTREAM_MIRROR_SIZE;
                //set frame header info flag
                avstreamhdl->frminfoflag=1;//need frame header info before audio frame

                //set agent for first fill after create
                avstreamhdl->avstreamhdl_agent->ringbufptr=avstreamhdl->ringbufptr;
                avstreamhdl->avstreamhdl_agent->rdptr=avstreamhdl->rdptr;
                avstreamhdl->avstreamhdl_agent->wtptr=avstreamhdl->wtptr;
                avstreamhdl->avstreamhdl_agent->ringbuflen=avstreamhdl->ringbuflen;
                avstreamhdl->avstreamhdl_agent->mirrorlen=avstreamhdl->mirrorlen;
                avstreamhdl->avstreamhdl_agent->avid=0;//audio id
                avstreamhdl->avstreamhdl_agent->bytesneed=AVSTREAM_A_RINGBUF_SIZE+AVSTREAM_MIRROR_SIZE-AVSTREAM_SPACE;
                avstreamhdl->avstreamhdl_agent->bufupdate=1;
                avstreamhdl->avstreamhdl_agent->frminfoflag=avstreamhdl->frminfoflag;
            }
            else
            {
                COS_FREE(avstreamhdl->ringbufptr);
                COS_FREE(avstreamhdl->avstreamhdl_agent);
                COS_FREE(avstreamhdl);
                avstreamhdl=0;
//              avstreamhdl->avstreamhdl_agent=0;
            }
        }
        else//no mirror
        {
            avstreamhdl->ringbufptr=(unsigned char*)COS_MALLOC(AVSTREAM_V_RINGBUF_SIZE);
            avstreamhdl->avstreamhdl_agent=(avagentstr*)COS_MALLOC(sizeof(avagentstr));
            if(avstreamhdl->ringbufptr && avstreamhdl->avstreamhdl_agent)
            {
                memset(avstreamhdl->avstreamhdl_agent, 0, sizeof(avagentstr));
                avstreamhdl->ringbuflen=AVSTREAM_V_RINGBUF_SIZE;
                avstreamhdl->rdptr=avstreamhdl->wtptr=avstreamhdl->ringbufptr;
                avstreamhdl->mirrorlen=0;
                //set frame header info flag.
                avstreamhdl->frminfoflag=1;//need frame header info before video frame data.

                //set agent for first fill after create
                avstreamhdl->avstreamhdl_agent->ringbufptr=avstreamhdl->ringbufptr;
                avstreamhdl->avstreamhdl_agent->rdptr=avstreamhdl->rdptr;
                avstreamhdl->avstreamhdl_agent->wtptr=avstreamhdl->wtptr;
                avstreamhdl->avstreamhdl_agent->ringbuflen=avstreamhdl->ringbuflen;
                avstreamhdl->avstreamhdl_agent->mirrorlen=avstreamhdl->mirrorlen;
                avstreamhdl->avstreamhdl_agent->avid=1;//video id
                avstreamhdl->avstreamhdl_agent->bytesneed=AVSTREAM_V_RINGBUF_SIZE -AVSTREAM_SPACE;
                avstreamhdl->avstreamhdl_agent->bufupdate=1;
                avstreamhdl->avstreamhdl_agent->frminfoflag=avstreamhdl->frminfoflag;
            }
            else
            {
                COS_FREE(avstreamhdl->ringbufptr);
                COS_FREE(avstreamhdl->avstreamhdl_agent);
                COS_FREE(avstreamhdl);
                avstreamhdl=0;
//              avstreamhdl->avstreamhdl_agent=0;
            }
        }
    }

    return avstreamhdl;
}


/*
get audio or video ringbuf handle info.
avfillstr->bytesneed show data size remain need.
avfillstr->avid show audio(0) or video(1) stream need be fill.
sheen
20120426
*/
avfillstr* avstream_srv_getfillhdl(avinfostr* avinfohdl)
{
    diag_printf("gt in");
    if(!avinfohdl) return 0;

    //audio need first
    if(avinfohdl->avstreamhdl_a && avinfohdl->avstreamhdl_a->avstreamhdl_agent)
    {
        if(avinfohdl->avstreamhdl_a->avstreamhdl_agent->bufupdate==1)
        {
            diag_printf("gt 0 nd %d",avinfohdl->avstreamhdl_a->avstreamhdl_agent->bytesneed);
            return (avfillstr*)avinfohdl->avstreamhdl_a->avstreamhdl_agent;
        }
    }

    //video
    if(avinfohdl->avstreamhdl_v && avinfohdl->avstreamhdl_v->avstreamhdl_agent)
    {
        if(avinfohdl->avstreamhdl_v->avstreamhdl_agent->bufupdate==1)
        {
            diag_printf("gt 1 nd %d",avinfohdl->avstreamhdl_v->avstreamhdl_agent->bytesneed);
            return (avfillstr*)avinfohdl->avstreamhdl_v->avstreamhdl_agent;
        }
    }

    return 0;
}

/*
fill audio or video ringbuf
avfillhdl get by avstream_srv_getfillhdl.
avfillhdl->bytesneed show remain data size need.
srcbuf: out source buffer
srclen: out source buffer data length
return bytes size cosume from out buffer srcbuf, and avfillhdl->bytesneed  update.
func both valid with mirrorlen=!0 and mirrorlen=0.
sheen
20120425
*/
int avstream_srv_fillBuf(avfillstr* avfillhdl,unsigned char*srcbuf,int srclen)
{
    uint8* lftMroEnd;
    uint8* ritMroStart;
    int32 fillSize;
    int32 readSize=0;
    int32 consumesize=0;
    avagentstr* avagenthdl= (avagentstr*)avfillhdl;
    diag_printf("filbf in");
    if(!avagenthdl || !srcbuf)return 0;

    // left mirror end position
    lftMroEnd=avagenthdl->ringbufptr+avagenthdl->mirrorlen;
    ritMroStart=lftMroEnd+avagenthdl->ringbuflen;

    while(1)
    {
        if(avagenthdl->wtptr+AVSTREAM_SPACE<avagenthdl->rdptr)
        {
            fillSize=avagenthdl->rdptr-avagenthdl->wtptr-AVSTREAM_SPACE;
            if(avagenthdl->wtptr>=lftMroEnd)
            {
                //readSize=readSamp(avagenthdl->wtptr, fillSize);
                if(fillSize<=srclen)
                    readSize=fillSize;
                else
                    readSize=srclen;
                memcpy(avagenthdl->wtptr,srcbuf,readSize);
                avagenthdl->wtptr+=readSize;
                consumesize+=readSize;
                srcbuf+=readSize;
                srclen-=readSize;
            }
            else
            {
                if(fillSize<=lftMroEnd-avagenthdl->wtptr)
                {
                    readSize=fillSize;
                    memcpy(avagenthdl->wtptr, (ritMroStart+(avagenthdl->wtptr-avagenthdl->ringbufptr)), readSize);
                    avagenthdl->wtptr+=readSize;
                }
                else
                {
                    readSize=lftMroEnd-avagenthdl->wtptr;
                    memcpy(avagenthdl->wtptr, (ritMroStart+(avagenthdl->wtptr-avagenthdl->ringbufptr)), readSize);
                    avagenthdl->wtptr+=readSize;

                    //readSize=readSamp(avagenthdl->wtptr, fillSize-readSize);
                    if(fillSize-readSize<=srclen)
                        readSize=fillSize-readSize;
                    else
                        readSize=srclen;
                    memcpy(avagenthdl->wtptr,srcbuf,readSize);
                    avagenthdl->wtptr+=readSize;
                    consumesize+=readSize;
                    srcbuf+=readSize;
                    srclen-=readSize;
                }
            }

            break;
        }
        else if(avagenthdl->wtptr>=avagenthdl->rdptr)
        {
            if(avagenthdl->wtptr>=ritMroStart+avagenthdl->mirrorlen)
            {
                if(avagenthdl->rdptr>=avagenthdl->ringbufptr+AVSTREAM_SPACE)
                {
                    avagenthdl->wtptr=avagenthdl->ringbufptr;
                    continue;
                }
                else
                    break;
            }

            fillSize=ritMroStart+avagenthdl->mirrorlen-avagenthdl->wtptr;
            //readSize=readSamp(avagenthdl->wtptr, fillSize);
            if(fillSize<=srclen)
                readSize=fillSize;
            else
                readSize=srclen;
            memcpy(avagenthdl->wtptr,srcbuf,readSize);
            avagenthdl->wtptr+=readSize;
            consumesize+=readSize;
            srcbuf+=readSize;
            srclen-=readSize;

            if(readSize<fillSize)
                break;
        }
        else
            break;
    }

    avagenthdl->bytesneed -=consumesize;
    diag_printf("filbf %d sz %d csu %d",avagenthdl->avid,srclen,consumesize);
    return consumesize;
}

/*
fill ringbuf over.
call after n times avstream_srv_fillBuf.
sheen
20120425
*/
void avstream_srv_fillover(avfillstr* avfillhdl)
{
    avagentstr* agentptr= (avagentstr*)avfillhdl;
    diag_printf("filovr %d ",agentptr->avid);
    if(!agentptr)return;

    if(agentptr->bufupdate==1)
        agentptr->bufupdate=2;

}

/*
reset then get fill fresh data for seek or new start
sheen
20120427
*/
void avstream_srv_reset(avinfostr* avinfohdl)
{
    if(!avinfohdl)return;

    if(avinfohdl->avstreamhdl_a)
    {
        avinfohdl->avstreamhdl_a->rdptr=avinfohdl->avstreamhdl_a->wtptr=avinfohdl->avstreamhdl_a->ringbufptr+AVSTREAM_MIRROR_SIZE;
        //set agent and need more data
        avinfohdl->avstreamhdl_a->avstreamhdl_agent->rdptr=avinfohdl->avstreamhdl_a->rdptr;
        avinfohdl->avstreamhdl_a->avstreamhdl_agent->wtptr=avinfohdl->avstreamhdl_a->wtptr;
        avinfohdl->avstreamhdl_a->avstreamhdl_agent->bytesneed=AVSTREAM_A_RINGBUF_SIZE+AVSTREAM_MIRROR_SIZE-AVSTREAM_SPACE;
        avinfohdl->avstreamhdl_a->avstreamhdl_agent->stmrest=1;
        avinfohdl->avstreamhdl_a->avstreamhdl_agent->bufupdate=1;
    }

    if(avinfohdl->avstreamhdl_v)
    {
        avinfohdl->avstreamhdl_v->rdptr=avinfohdl->avstreamhdl_v->wtptr=avinfohdl->avstreamhdl_v->ringbufptr;
        //set agent and need more data
        avinfohdl->avstreamhdl_v->avstreamhdl_agent->rdptr=avinfohdl->avstreamhdl_v->rdptr;
        avinfohdl->avstreamhdl_v->avstreamhdl_agent->wtptr=avinfohdl->avstreamhdl_v->wtptr;
        avinfohdl->avstreamhdl_v->avstreamhdl_agent->bytesneed=AVSTREAM_V_RINGBUF_SIZE-AVSTREAM_SPACE;
        avinfohdl->avstreamhdl_v->avstreamhdl_agent->stmrest=1;
        avinfohdl->avstreamhdl_v->avstreamhdl_agent->bufupdate=1;
    }
    diag_printf("avstream_srv_reset");
}

/*
destroy all of a/v stream create by avstream_srv_create.
must be call after video player close .
sheen
20120426
*/
void avstream_srv_destroyall(avinfostr* avinfohdl)
{
    //free all memory
    if(avinfohdl)
    {
        if(avinfohdl->avstreamhdl_a)
        {
            if(avinfohdl->avstreamhdl_a->avstreamhdl_agent)
            {
                COS_FREE(avinfohdl->avstreamhdl_a->avstreamhdl_agent);
                avinfohdl->avstreamhdl_a->avstreamhdl_agent=0;
            }
            if(avinfohdl->avstreamhdl_a->ringbufptr)
            {
                COS_FREE(avinfohdl->avstreamhdl_a->ringbufptr);
                avinfohdl->avstreamhdl_a->ringbufptr=0;
            }

            COS_FREE(avinfohdl->avstreamhdl_a);
            avinfohdl->avstreamhdl_a=0;
        }

        if(avinfohdl->avstreamhdl_v)
        {
            if(avinfohdl->avstreamhdl_v->avstreamhdl_agent)
            {
                COS_FREE(avinfohdl->avstreamhdl_v->avstreamhdl_agent);
                avinfohdl->avstreamhdl_v->avstreamhdl_agent=0;
            }
            if(avinfohdl->avstreamhdl_v->ringbufptr)
            {
                COS_FREE(avinfohdl->avstreamhdl_v->ringbufptr);
                avinfohdl->avstreamhdl_v->ringbufptr=0;
            }

            COS_FREE(avinfohdl->avstreamhdl_v);
            avinfohdl->avstreamhdl_v=0;
        }
    }
}

/****************************************************************
func uesed by data fill task end
*****************************************************************/




/****************************************************************
follow func uesed by video player
*****************************************************************/

/*
check a/v's agent fill over or not, and update the new wtptr
sheen
20120426
*/
void avstream_clt_checkagent(avstreamstr* avstreamhdl)
{
    if(avstreamhdl->avstreamhdl_agent->bufupdate==2)
    {
        //data fill over and update wtptr
        avstreamhdl->wtptr=avstreamhdl->avstreamhdl_agent->wtptr;
        avstreamhdl->avstreamhdl_agent->bufupdate=0;

    }
    diag_printf("chk %d",avstreamhdl->avstreamhdl_agent->avid);
}

/*
send a/v's agent to get fil
sheen
20120426
*/
void avstream_clt_sendagent(avstreamstr* avstreamhdl)
{
    int bytesneed=0;

    //get bytesneed
    //rdptr right, wtptr left
    if(avstreamhdl->rdptr>avstreamhdl->wtptr+AVSTREAM_SPACE)
    {
        if(avstreamhdl->rdptr >avstreamhdl->ringbufptr+avstreamhdl->mirrorlen)
        {
            if(avstreamhdl->wtptr>=avstreamhdl->ringbufptr+avstreamhdl->mirrorlen- AVSTREAM_SPACE)
                bytesneed= avstreamhdl->rdptr -avstreamhdl->wtptr- AVSTREAM_SPACE;
            else
                bytesneed= avstreamhdl->rdptr -avstreamhdl->ringbufptr- avstreamhdl->mirrorlen;
        }
    }
    else if(avstreamhdl->rdptr<=avstreamhdl->wtptr)//rdptr left, wtptr right
    {
        if(avstreamhdl->rdptr>= avstreamhdl->ringbufptr+avstreamhdl->mirrorlen)
        {
            bytesneed=(avstreamhdl->ringbuflen+avstreamhdl->mirrorlen)- (avstreamhdl->wtptr-avstreamhdl->rdptr)- AVSTREAM_SPACE;
        }
        else
        {
            bytesneed=(avstreamhdl->ringbuflen+avstreamhdl->mirrorlen)-
                      (avstreamhdl->wtptr-(avstreamhdl->ringbufptr+avstreamhdl->mirrorlen))- AVSTREAM_SPACE;
        }
    }

    if(avstreamhdl->avstreamhdl_agent->bufupdate==0 && bytesneed>sizeof(avframeinfohdstr))
    {
        //tell current stream info
        avstreamhdl->avstreamhdl_agent->rdptr=avstreamhdl->rdptr;
        avstreamhdl->avstreamhdl_agent->wtptr=avstreamhdl->wtptr;
        avstreamhdl->avstreamhdl_agent->bytesneed= bytesneed;
        avstreamhdl->avstreamhdl_agent->bufupdate=1;
        avstreamhdl->avstreamhdl_agent->frminfoflag=avstreamhdl->frminfoflag;
        //send a msg to srv
    }
    diag_printf("snd %d %d %d",avstreamhdl->avstreamhdl_agent->avid,bytesneed,avstreamhdl->avstreamhdl_agent->bufupdate);
}

/*
get remain data length in ring buffer.
assume that one audio frame cnsume size will not bigger than mirror.
func both valid with mirrorlen=!0 and mirrorlen=0.
sheen
20120425
*/
int avstream_clt_getDataLen(avstreamstr* avstreamhdl)
{
    int dataInBuf;

    //avstream_clt_checkagent(avstreamhdl);

    //rdptr right, wtptr left
    if(avstreamhdl->rdptr>avstreamhdl->wtptr)
    {
        dataInBuf=avstreamhdl->ringbuflen+(avstreamhdl->mirrorlen<<1)-(avstreamhdl->rdptr-avstreamhdl->wtptr);

        if(avstreamhdl->wtptr>=avstreamhdl->ringbufptr+avstreamhdl->mirrorlen)
        {
            dataInBuf-=avstreamhdl->mirrorlen;
        }
        else
        {
            dataInBuf-=(avstreamhdl->wtptr-avstreamhdl->ringbufptr);
        }
    }
    else//rdptr left, wtptr right
    {
        dataInBuf=avstreamhdl->wtptr-avstreamhdl->rdptr;
    }

    //avstream_clt_sendagent(avstreamhdl);
    diag_printf("len %d %d",avstreamhdl->avstreamhdl_agent->avid,dataInBuf);
    return dataInBuf;
}

/*
get ringbuf rdptr.
assume that data cnsumeLen will not bigger than mirror.
func both valid with mirrorlen=!0 and mirrorlen=0.
sheen
20120425
*/
unsigned char* avstream_clt_getDataPos(avstreamstr* avstreamhdl)
{
    int8* startPos;
    uint8* ritMroStart;

    //avstream_clt_checkagent(avstreamhdl);

    // right mirror start position
    ritMroStart=avstreamhdl->ringbufptr+avstreamhdl->ringbuflen+avstreamhdl->mirrorlen;

    //rdptr start not in right mirror
    if(avstreamhdl->rdptr<ritMroStart)
    {
        startPos=avstreamhdl->rdptr;
    }
    else//rdptr start into the right mirror
    {
        uint8* lftMroPos;
        if(avstreamhdl->wtptr<avstreamhdl->rdptr)
        {
            lftMroPos=avstreamhdl->ringbufptr+(avstreamhdl->rdptr-ritMroStart);
            if(avstreamhdl->wtptr>=lftMroPos)
            {
                startPos=lftMroPos;
            }
            else
                startPos=avstreamhdl->rdptr;
        }
        else
        {
            startPos=avstreamhdl->rdptr;
        }
    }

    //avstream_clt_sendagent(avstreamhdl);

    return startPos;
}

/*
move rdptr start position.
assume that data cnsumeLen will not bigger than mirror.
func both valid with mirrorlen=!0 and mirrorlen=0.
sheen
20120425
*/
void avstream_clt_moveDataPos(avstreamstr* avstreamhdl,int consumeLen)
{
    uint8* ritMroStart;
    uint8* moveStartPos;

    //avstream_clt_checkagent(avstreamhdl);

    // right mirror start position
    ritMroStart=avstreamhdl->ringbufptr+avstreamhdl->ringbuflen+avstreamhdl->mirrorlen;
    //rdptr will move position
    moveStartPos=avstreamhdl->rdptr+consumeLen;

    if(moveStartPos<ritMroStart)
    {
        if(avstreamhdl->wtptr<avstreamhdl->rdptr)
        {
            avstreamhdl->rdptr=moveStartPos;
        }
        else if(avstreamhdl->wtptr>=moveStartPos)
        {
            avstreamhdl->rdptr=moveStartPos;
        }
        else
        {
            avstreamhdl->rdptr=avstreamhdl->wtptr;
        }
    }
    else
    {
        uint8* lftMroPos;
        if(avstreamhdl->wtptr<avstreamhdl->rdptr)
        {
            lftMroPos=avstreamhdl->ringbufptr+(moveStartPos-ritMroStart);
            if(avstreamhdl->wtptr>=lftMroPos)
            {
                avstreamhdl->rdptr=lftMroPos;
            }
            else
                avstreamhdl->rdptr=avstreamhdl->wtptr;
        }
        else if(avstreamhdl->wtptr>=moveStartPos)
        {
            avstreamhdl->rdptr=moveStartPos;
        }
        else
        {
            avstreamhdl->rdptr=avstreamhdl->wtptr;
        }
    }

    //avstream_clt_sendagent(avstreamhdl);
}

/*
copy data from ringbuf.
char* output,
unsigned int blkSize, //block size
unsigned int blkNum// block number
func both valid with mirrorlen=!0 and mirrorlen=0.
sheen
20120425
*/
int avstream_clt_getData(avstreamstr* avstreamhdl,unsigned char* output, unsigned int blkSize, unsigned int blkNum)
{
    int res;
    int getBlkNum;
    unsigned char *pTail;
    unsigned char *pOut;

    //avstream_clt_checkagent(avstreamhdl);

    pTail=avstreamhdl->ringbufptr+avstreamhdl->ringbuflen+(avstreamhdl->mirrorlen<<1);
    pOut=output;
    getBlkNum=0;
    while(avstream_clt_getDataLen(avstreamhdl)>=blkSize)
    {
        res=(int)(pTail-avstreamhdl->rdptr);
        if(res>=blkSize)
        {
            memcpy(pOut, avstreamhdl->rdptr, blkSize);
            avstream_clt_moveDataPos(avstreamhdl,blkSize);
        }
        else
        {
            memcpy(pOut, avstreamhdl->rdptr, res);
            avstream_clt_moveDataPos(avstreamhdl,res);

            memcpy(pOut+res, avstreamhdl->rdptr, blkSize-res);
            avstream_clt_moveDataPos(avstreamhdl,blkSize-res);
        }

        pOut+=blkSize;
        getBlkNum++;
        if(getBlkNum==blkNum)break;
    }

    //avstream_clt_sendagent(avstreamhdl);

    return getBlkNum;
}

/*
get frame data header from ringbuf.
avframeinfohdstr* header //output header data
find sync word 0xAAAAAAAA and output header.
return 1 success or 0 fail.
if find part or full of the syncword but the header data not enough, discard it whatever next fill.
sheen
20120824
*/
int avstream_clt_getheader(avstreamstr* avstreamhdl,avframeinfohdstr* header)
{
    avframeinfohdstr hd;
    unsigned char *pData= (unsigned char*)&hd;
    int size=sizeof(avframeinfohdstr);
    int findword=0;// find=4
    int res=0;

    while((res=avstream_clt_getData(avstreamhdl,pData, size, 1))==1)
    {
        if(hd.syncword==0xAAAAAAAA)
        {
            if(hd.samplesize < avstreamhdl->ringbuflen)
                break;
            else
            {
                pData=((unsigned char*)&hd)+4;
                findword=3;
                size= sizeof(avframeinfohdstr)-4;
                diag_printf("[avstream] fake syncword!");
            }
        }

        //continue find it
        while(findword!=4 && size!=0)
        {
            if(*pData!=0xAA)
                findword=0;
            else
                findword++;
            pData++;
            size--;
        }

        if(findword)
        {
            pData -= findword;
            memmove(&hd,pData,size+findword);
        }
        pData= ((unsigned char*)&hd)+size+findword;
        size= sizeof(avframeinfohdstr)- (size+findword);
        res=0;

    }

    if(res==1)
    {
        *header= hd;
        diag_printf("hd %d %d",avstreamhdl->avstreamhdl_agent->avid,header->timestamp);
    }
    else
        diag_printf("[avstream] %d miss syncword!",avstreamhdl->avstreamhdl_agent->avid);

    return res;
}

/*****************************************************
avstream flow explain

server, only fill data by audio/video agent.


1.  create and prestore before video player open.
    create audio stream, avinfostr->avstreamhdl_a =avstream_srv_create
    create video stream, avinfostr->avstreamhdl_v =avstream_srv_create

2.  prestore data,
    get audio/video fill handle avfillstr, avstream_srv_getfillhdl
    if(avfillstr->frminfoflag) fill frame header before each av frame data.
    fill data, avstream_srv_fillBuf
    fill data over, avstream_srv_fillover

3.  open player with avinfostr handle.
4.  get msg when playing
    audio or video avstream_srv_getfillhdl
    if(avfillstr->frminfoflag) fill frame header before each av frame data.
    avstream_srv_fillBuf
    avstream_srv_fillover

5.  seek or new data start
    avstream_srv_reset, find the key frame position.
    audio and video avstream_srv_getfillhdl
    if(avfillstr->frminfoflag) fill frame header before each av frame data.
    avstream_srv_fillBuf
    avstream_srv_fillover

6.  quit player
    avstream_srv_destroyall


client,     only read data.

******************************************************/

#if 0//test
int avfilehdl;;
int vfileoffset, vfileend;
int afileoffset,afileend;
char *dwldbuf;

void avstream_srv_filltest(avinfostr* avinfohdl)
{
    avfillstr * fillhdl;
    int res;

    //video stream include avframeinfohdstr
    fillhdl= avstream_srv_getfillhdl(avinfohdl);
    if(!fillhdl)return;

    if(fillhdl->avid==0 && fillhdl->bytesneed>0)
    {
        if(afileoffset <afileend)
        {
            FS_Seek(avfilehdl, afileoffset, FS_SEEK_SET);
            res=FS_Read(avfilehdl,dwldbuf,fillhdl->bytesneed);
            diag_printf("srv a need=%d res=%d offset=%d\n",fillhdl->bytesneed,res,afileoffset);
            afileoffset+=res;

            res=avstream_srv_fillBuf(fillhdl, dwldbuf, res);
            diag_printf("fill %d\n",res);
        }

        avstream_srv_fillover(fillhdl);
        return;
    }

    if(fillhdl->avid==1 && fillhdl->bytesneed>0 )
    {
        if(vfileoffset <vfileend)
        {
            res= fillhdl->bytesneed;
            if(vfileoffset+fillhdl->bytesneed >vfileend)
            {
                res= vfileend- vfileoffset;
            }

            FS_Seek(avfilehdl, vfileoffset, FS_SEEK_SET);
            res=FS_Read(avfilehdl,dwldbuf,res);
            diag_printf("srv v need=%d res=%d offset=%d\n",fillhdl->bytesneed,res,vfileoffset);
            vfileoffset+=res;

            res=avstream_srv_fillBuf(fillhdl, dwldbuf, res);
            diag_printf("fill %d\n",res);
        }
        avstream_srv_fillover(fillhdl);
        return;
    }
}

avinfostr* avstream_srv_testinit(int filehandle)
{
    avinfostr* avinfohdl;
    avfillstr * fillhdl;
    int res;
    avfilehdl=filehandle;
    dwldbuf= COS_MALLOC(80*1024);
    avinfohdl= COS_MALLOC(sizeof(avinfostr));
    memset(avinfohdl,0,sizeof(avinfostr));
    avinfohdl->avstreamhdl_a=avstream_srv_create(0);
    //avinfohdl->avstreamhdl_v=avstream_srv_create(1);

    avinfohdl->totaltime=120000;//ms

    /*
    //audio info amr
    avinfohdl->fmt_a=3;//audio format
    avinfohdl->samplerate_a=8000;
    avinfohdl->bits_a=16;
    avinfohdl->channels_a=1;
    avinfohdl->totalfrm_a=0;//total frame count or time/data length
    //video info 263
    avinfohdl->fmt_v=9;//video format
    avinfohdl->width_v=176;
    avinfohdl->height_v=144;
    avinfohdl->fps_v=25;
    avinfohdl->totalfrm_v=0;//total frame count or time/data length

    vfileoffset=0;
    afileoffset=2945324;
    */

    /*
        //audio info aac
        avinfohdl->fmt_a=2;//audio format
        avinfohdl->samplerate_a=44100;
        avinfohdl->bits_a=16;
        avinfohdl->channels_a=2;
        avinfohdl->totalfrm_a=0;//total frame count or time/data length
        //video info 264
        avinfohdl->fmt_v=15;//video format
        avinfohdl->width_v=320;
        avinfohdl->height_v=240;
        //avinfohdl->fps_v=2;
        avinfohdl->totalfrm_v=0;//total frame count or time/data length

        vfileoffset=0;
        afileoffset=52844;
        */

    /*
    avinfohdl->avtimelineflag=1;
    //audio info amr
    avinfohdl->fmt_a=3;//audio format
    avinfohdl->samplerate_a=8000;
    avinfohdl->bits_a=16;
    avinfohdl->channels_a=1;
    avinfohdl->totalfrm_a=0;//total frame count or time/data length
    avinfohdl->baseclock_a=22500;
    //video info 263
    avinfohdl->fmt_v=9;//video format
    avinfohdl->width_v=176;
    avinfohdl->height_v=144;
    avinfohdl->totalfrm_v=0;//total frame count or time/data length
    avinfohdl->baseclock_v=22500;
    avinfohdl->frmtimescale_v=1500;
    vfileoffset=0;
    //afileoffset=6606637;
    //afileend=6805597;
    //vfileend=6606637;
    afileoffset=6612629;//with syncword. both 2bad syncword in video(pos.370376,1002275) and audio(pos.6616457,6659753)
    afileend=6831485;
    vfileend=6612629;
    */

    /*
    avinfohdl->avtimelineflag=1;
    //DRA audio only
    avinfohdl->fmt_a=A_TYPE_DRA;//audio format
    avinfohdl->samplerate_a=48000;
    avinfohdl->bits_a=16;
    avinfohdl->channels_a=2;
    avinfohdl->totalfrm_a=0;//total frame count or time/data length
    avinfohdl->baseclock_a=22500;
    vfileoffset=0;
    afileoffset=0;//with syncword. both 2bad syncword in video(pos.370376,1002275) and audio(pos.6616457,6659753)
    afileend=300000;
    vfileend=0;
    */
    avinfohdl->avtimelineflag=1;
    //DRA audio only
    avinfohdl->fmt_a=A_TYPE_AMR;//audio format
    avinfohdl->samplerate_a=8000;
    avinfohdl->bits_a=16;
    avinfohdl->channels_a=1;
    avinfohdl->totalfrm_a=0;//total frame count or time/data length
    avinfohdl->baseclock_a=22500;
    vfileoffset=0;
    afileoffset=0;//with syncword. both 2bad syncword in video(pos.370376,1002275) and audio(pos.6616457,6659753)
    afileend=300000;
    vfileend=0;

    /*
        avinfohdl->avtimelineflag=0;
        //audio info amr
        avinfohdl->fmt_a=3;//audio format
        avinfohdl->samplerate_a=8000;
        avinfohdl->bits_a=16;
        avinfohdl->channels_a=1;
        avinfohdl->totalfrm_a=0;//total frame count or time/data length
        avinfohdl->baseclock_a=8000;
        //video info 263
        avinfohdl->fmt_v=9;//video format
        avinfohdl->width_v=176;
        avinfohdl->height_v=144;
        avinfohdl->totalfrm_v=0;//total frame count or time/data length
        avinfohdl->baseclock_v=90000;
        avinfohdl->frmtimescale_v=6000;
        vfileoffset=0;
        afileoffset=6606637;
        */

    avstream_srv_filltest(avinfohdl);
    avstream_srv_filltest(avinfohdl);

    return avinfohdl;
}

void avstreamstr_srv_testclose(avinfostr* avinfohdl)
{
    avstream_srv_destroyall( avinfohdl);
    COS_FREE(dwldbuf);
    COS_FREE(avinfohdl);
}

//#ifdef SHEEN_VC_DEBUG
#if 0

void *mmc_MemMalloc(int size)
{
    static i=0,tot=0;
    printf("i=%d size=%d tot=%d \n",i ,size, tot);
    i++;
    tot+=size;
    return malloc(size);
}

void main(void)
{
    FILE *fin=fopen("F:\\测试文件\\avstream.3gp","rb");
    FILE *fouta=fopen("F:\\测试文件\\avstream_a0.3gp","wb");
    FILE *foutv=fopen("F:\\测试文件\\avstream_v0.3gp","wb");
    char *frmbuf=malloc(80*1024);
    char *ptr;
    avinfostr *avinfohd;
    int res;
    avframeinfohdstr frmhd;
    int avover;

    avinfohd=avstream_srv_testinit(fin);


    while(1)
    {
        avover=0;

        res=avstream_clt_getDataLen(avinfohd->avstreamhdl_v);
        if(res>8)
        {
            avstream_clt_getData(avinfohd->avstreamhdl_v,&frmhd,8,1);
            avstream_srv_filltest(avinfohd);

            avstream_clt_getData(avinfohd->avstreamhdl_v,frmbuf,frmhd.samplesize,1);
            fwrite(frmbuf,1,frmhd.samplesize,foutv);
            avstream_srv_filltest(avinfohd);
            avover=1;
            printf("time=%d size=%d remain=%d\n",frmhd.timestamp,frmhd.samplesize,res-frmhd.samplesize-8);
        }

        res=avstream_clt_getDataLen(avinfohd->avstreamhdl_a);
        if(res>0)
        {
            ptr=avstream_clt_getDataPos(avinfohd->avstreamhdl_a);
            if(res>=64)
            {
                fwrite(ptr,1,64,fouta);
                avstream_clt_moveDataPos(avinfohd->avstreamhdl_a,64);
            }
            else
            {
                fwrite(ptr,1,res,fouta);
                avstream_clt_moveDataPos(avinfohd->avstreamhdl_a,res);
            }
            avstream_srv_filltest(avinfohd);
            avover=1;
            printf("audio remain=%d\n",res);
        }

        if(!avover)break;
    }

    avstream_srv_destroyall(avinfohd);
    fclose(fin);
    fclose(fouta);
    fclose(foutv);
    free(frmbuf);
}
#endif

#endif


