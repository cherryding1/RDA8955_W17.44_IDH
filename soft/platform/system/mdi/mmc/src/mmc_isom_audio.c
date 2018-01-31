
#include "global.h"

#ifdef MP4_3GP_SUPPORT

#include "cswtype.h"
#include "cs_types.h"
#include "cpu_share.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "hal_aif.h"
#include "aud_m.h"
#include "di.h"
#include "mci.h"
#include "mmc_timer.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_tls.h"
#include "mmc_isom_video.h"
#include "event.h"
#include "hal_timers.h"
#include "lcdd_m.h"
#include "hal_overlay.h"
#include "mcip_debug.h"
#include "mmc_audiompeg.h"

#ifdef MPEG4_SUPPORT
#include "xvid_decraw.h"
#endif

#ifdef H263_SUPPORT
#include "h263_global.h"
#endif
#include "mmi_jpeg_dec.h"
#include "hal_gouda.h"
#include "hal_speech.h"
#include "vpp_speech.h"
#include "hal_sys.h"
#include "mdi_alg_common.h"
#include "dm.h"


//for mp4
typedef enum
{
	//Video player state.
	VID_PLAY_STATE_IDLE=0,
	VID_PLAY_STATE_PLAYING,
	VID_PLAY_STATE_PAUSE,

	//invalid after.
	VID_PLAY_STATE_INVALID
}VID_PLAY_STATE;

#define AAC_HEAD_SIZE 7
#define AMR_HEAD "#!AMR\n"
#define AMR_HEAD_SIZE 6

#define MAX_SLOW_NUM 2
#define AAC_FRAME_SAMPLE 1024
#define AMR_FRAME_SAMPLE 160
#define MP3_FRAME_SAMPLE_1152 1152
#define MP3_FRAME_SAMPLE_576 576
#define DRA_FRAME_SAMPLE_1024 1024
#define PCM_FRAME_SAMPLE 1024//user define

#define PCM_BUF_AAC_FRAME_NUM (1*2)
#define PCM_BUF_AMR_FRAME_NUM (2*2)
#define PCM_BUF_MP3_FRAME_NUM (1*2)
#define PCM_BUF_PCM_FRAME_NUM (2*2)//user define
#define PCM_BUF_COOK_FRAME_NUM (2*2)
#define PCM_BUF_DRA_FRAME_NUM (2*2)

#define EXTRA_PCM_BUF_NUM 3 //use above half num as unit.

#define MAX_A_SAMPLEBUF_SIZE (1024*2*3)
#define MAX_A_SAMPLEBUF_MIRROR_SIZE 1536 //1024
#define MAX_A_SAMPLEBUF_PIPE_SIZE (1024*6)

#define PIPE_SPACE 4
#define V_FPS_SCALE_BIT 8

static unsigned char aacHead[AAC_HEAD_SIZE];
static volatile uint16 play_state=VID_PLAY_STATE_INVALID;

extern HAL_AIF_STREAM_T audioStream;
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T    audioItf;

extern MPEG_PLAY MpegPlayer;
extern const uint8 silence_frame475[];

static MPEG_INPUT  *my_input = &MpegPlayer.MpegInput;
static INT16 g_SbcEncEnable=0;

static volatile uint32 RX_BUF_SWAP_ID;//pingpang

static IsomPlayGlobal *pIsomGlob;

#define VOC_WORK_NON 0
#define VOC_WORK_AUD 1

static vpp_AudioJpeg_DEC_IN_T vocDecAud;

static vpp_AudioJpeg_DEC_OUT_T vocDecStatus;
#define VOC_WORK_CONT (0x2000000)//total time=xxx ms
static volatile uint8 VOC_OPEN;
static volatile uint8 VOC_SETTING;
static volatile uint8 VOC_WORK;

static volatile int16 VOC_AUD;
static volatile uint16 AUD_DEC_FRM_NUM;
static volatile uint8 PCM_ISR_FLAG;
static volatile int16 AUD_DATA_REQ;//0 =no need, 1= need, 2=reposition && need
static volatile int16 SEND_EVENT_NUM;//avoid mail box full
static uint32 DTS_FLG;

typedef struct
{
	volatile uint8 done;// 0=need dec, 1=dec done
	volatile uint8 *pBufPos;
	volatile uint32 timestamp;//record with pcm sample num.
}aud_extra_pcmbuf;

//static volatile uint8 AUD_EXTRA_BUF_DONE;// 0=need dec, 1=dec done
static aud_extra_pcmbuf aud_extra_pcmbuf_state[EXTRA_PCM_BUF_NUM];//more, pingpang or only one
static volatile uint8 A_EXBF_RD_ID;// will read from
static volatile uint8 A_EXBF_WT_ID;// will write to.
//static int8 DISPLAY_STATE;// 1=in 0=out
static int8 AV_DEC_STATE;// 1=in 0=out
static volatile uint32 pPcmBufPos;

static INT32 *G_VidSBCConstY=NULL;
extern const INT32 G_VppSBCConstX[];

static volatile uint16 AUD_FADE_IN_FLAG;//zero frame num
static volatile uint16 AUD_FADE_OUT_FLAG;// 3 step. 3=pcm isr check and process fade out. 2=fade out will be play. 1=fade out over.

extern VOID mmc_MemCheck(BOOL bprintinfo);
extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeAll();
extern int mmc_MemGetFree();
static void vid_display(void) ;


#define WRITE_AAC_HEAD(outbuf, sampLen)\
{\
	aacHead[3]&=0xfc;\
	aacHead[4]=0;\
	aacHead[5]&=0x1f;\
	aacHead[5]|=(((sampLen)&0x07)<<5);\
	aacHead[4]=(((sampLen)&0x07f8)>>3);\
	aacHead[3]|=(((sampLen)&0x1800)>>11);\
	memcpy((outbuf), aacHead, AAC_HEAD_SIZE);\
}

static uint32 tick1, tick2, totalTa,totalTv, totalTx, counta, countv, countx;//test for time

/*
get remain data lenght in pipe buffer.
assume that data cnsumeLen will not bigger than mirror.
*/
static int getDataLen()
{
	int dataInBuf;

	if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
	{
		avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
		return avstream_clt_getDataLen(avinfohdl->avstreamhdl_a);
	}

	//a_SampStart right, a_SampEnd left
	if(pIsomGlob->a_SampStart>pIsomGlob->a_SampEnd)
	{
		dataInBuf=pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1)-(pIsomGlob->a_SampStart-pIsomGlob->a_SampEnd);

		if(pIsomGlob->a_SampEnd>=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen)
		{
			dataInBuf-=pIsomGlob->a_SampBufMirrorLen;
		}
		else
		{
			dataInBuf-=(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf);
		}
	}
	else//a_SampStart left, a_SampEnd right
	{
		dataInBuf=pIsomGlob->a_SampEnd-pIsomGlob->a_SampStart;
	}

	return dataInBuf;
}

/*
get sample start position.
assume that data cnsumeLen will not bigger than mirror.
*/
static char* getDataPos()
{
	int8* startPos;
	uint8* ritMroStart;

	if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
	{
		avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
		return avstream_clt_getDataPos(avinfohdl->avstreamhdl_a);
	}

	// right mirror start position
	ritMroStart=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+pIsomGlob->a_SampBufMirrorLen;

	//sample start not in right mirror
	if(pIsomGlob->a_SampStart<ritMroStart)
	{
		startPos=pIsomGlob->a_SampStart;
	}
	else//sample start into the right mirror
	{
		uint8* lftMroPos;
		if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
		{
			lftMroPos=pIsomGlob->a_SampBuf+(pIsomGlob->a_SampStart-ritMroStart);
			if(pIsomGlob->a_SampEnd>=lftMroPos)
			{
				startPos=lftMroPos;
			}
			else
				startPos=pIsomGlob->a_SampStart;
		}
		else
		{
			startPos=pIsomGlob->a_SampStart;
		}
	}

	return startPos;
}

/*
move sample start position.
assume that data cnsumeLen will not bigger than mirror.
*/
static void moveDataPos(int consumeLen)
{
	uint8* ritMroStart;
	uint8* moveStartPos;

	if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
	{
		avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
		avstream_clt_moveDataPos(avinfohdl->avstreamhdl_a,consumeLen);
		return;
	}

	// right mirror start position
	ritMroStart=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+pIsomGlob->a_SampBufMirrorLen;
	//sample start will move position
	moveStartPos=pIsomGlob->a_SampStart+consumeLen;

	if(moveStartPos<ritMroStart)
	{
		if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
		{
			pIsomGlob->a_SampStart=moveStartPos;
		}
		else if(pIsomGlob->a_SampEnd>=moveStartPos)
		{
			pIsomGlob->a_SampStart=moveStartPos;
		}
		else
		{
			pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
		}
	}
	else
	{
		uint8* lftMroPos;
		if(pIsomGlob->a_SampEnd<pIsomGlob->a_SampStart)
		{
			lftMroPos=pIsomGlob->a_SampBuf+(moveStartPos-ritMroStart);
			if(pIsomGlob->a_SampEnd>=lftMroPos)
			{
				pIsomGlob->a_SampStart=lftMroPos;
			}
			else
				pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
		}
		else if(pIsomGlob->a_SampEnd>=moveStartPos)
		{
			pIsomGlob->a_SampStart=moveStartPos;
		}
		else
		{
			pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd;
		}
	}
}

/*
copy data from buffer.
char* output,
unsigned int blkSize, //block size
unsigned int blkNum// block number
*/
static  int getData(unsigned char* output, unsigned int blkSize, unsigned int blkNum)
{
	int res;
	int getBlkNum;
	unsigned char *pTail;
	unsigned char *pOut;

	if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
	{
		avinfostr* avinfohdl= (avinfostr*)pIsomGlob->filehdl;
		return avstream_clt_getData(avinfohdl->avstreamhdl_a, output, blkSize, blkNum);
	}

	pTail=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1);
	pOut=output;
	getBlkNum=0;
	while(getDataLen()>=blkSize)
	{
		res=(int)(pTail-pIsomGlob->a_SampStart);
		if(res>=blkSize)
		{
			memcpy(pOut, pIsomGlob->a_SampStart, blkSize);
			moveDataPos(blkSize);
		}
		else
		{
			memcpy(pOut, pIsomGlob->a_SampStart, res);
			moveDataPos(res);

			memcpy(pOut+res, pIsomGlob->a_SampStart, blkSize-res);
			moveDataPos(blkSize-res);
		}

		pOut+=blkSize;
		getBlkNum++;
		if(getBlkNum==blkNum)break;
	}

	return getBlkNum;
}

/*
read data from bitstream and fill the out buffer full.
there have a temp buffer for bitstream,the size=mirror size
assume that the sample data will not bigger than mirror.
*/
static int readSamp(char *pBuf, int size)
{
	int res;
	int readSize;
	int8* pOut;
	uint8 useTmp;
	int8* pSamp;
	uint32 sampSize,gotbytes, gotcount;

	readSize=0;
	pOut=pBuf;

	if(pIsomGlob->a_SampTmpRemain>0)
	{
		if(pIsomGlob->a_SampTmpRemain>=size)
		{
			memcpy(pOut, pIsomGlob->a_SampTmpBufp, size);
			readSize=size;
		}
		else
		{
			memcpy(pOut, pIsomGlob->a_SampTmpBufp, pIsomGlob->a_SampTmpRemain);
			readSize=pIsomGlob->a_SampTmpRemain;
		}

		pOut+=readSize;
		pIsomGlob->a_SampTmpRemain-=readSize;
		pIsomGlob->a_SampTmpBufp+=readSize;
	}

	useTmp=0;
	while(readSize<size)
	{
		if(pIsomGlob->a_current_sample_num<pIsomGlob->a_total_sample_num)
		{
			if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
			{
				if(pIsomGlob->a_type== A_TYPE_AAC)
				{
					if((res = stbl_GetSampleSize(pIsomGlob->a_info , pIsomGlob->a_current_sample_num+1, &sampSize))!=GF_OK)
					{
						diag_printf("[VID] AUD GetSampleSize ERR! res=%d sample_num=%d \n",res, pIsomGlob->a_current_sample_num);
						break;
					}

					if(readSize+AAC_HEAD_SIZE+sampSize<=size)
					{
						useTmp=0;
						pSamp=pOut;
					}
					else if(AAC_HEAD_SIZE+sampSize<=pIsomGlob->a_SampBufMirrorLen)
					{
						useTmp=1;
						pSamp=pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
					}
					else
					{
						diag_printf("[VID] AUD ERR! sample size=%d bigger than a_SampBufMirrorLen=%d",sampSize, pIsomGlob->a_SampBufMirrorLen);
						break;
					}

					WRITE_AAC_HEAD(pSamp, AAC_HEAD_SIZE+sampSize);
					pSamp+=AAC_HEAD_SIZE;

					res=gf_isom_get_multSample((GF_MediaInformationBox *)pIsomGlob->a_info,
												pIsomGlob->a_current_sample_num+1,
												pSamp,
												sampSize,
												&gotbytes, &gotcount);

					if(res || (gotbytes<=0))
					{
						diag_printf("[VID] AUD get_multSample. res=%d gotbytes=%d sampSize=%d sample_num=%d",res, gotbytes, sampSize, pIsomGlob->a_current_sample_num);
						break;
					}

					if(useTmp==0)
					{
						readSize+=(AAC_HEAD_SIZE+gotbytes);
						pOut+=(AAC_HEAD_SIZE+gotbytes);
					}
					else
					{
						pIsomGlob->a_SampTmpRemain=(AAC_HEAD_SIZE+gotbytes);
						res=size-readSize;
						if(res>pIsomGlob->a_SampTmpRemain)
							res=pIsomGlob->a_SampTmpRemain;
						memcpy(pOut, pIsomGlob->a_SampTmpBufp, res);
						pIsomGlob->a_SampTmpRemain-=res;
						pIsomGlob->a_SampTmpBufp+=res;
						pOut+=res;
						readSize+=res;
					}
					pIsomGlob->a_current_sample_num+=gotcount;

				}
				else if(pIsomGlob->a_type== A_TYPE_AMR || pIsomGlob->a_type== A_TYPE_MP3)
				{
					if((res = stbl_GetSampleSize(pIsomGlob->a_info , pIsomGlob->a_current_sample_num+1, &sampSize))!=GF_OK)
					{
						diag_printf("[VID] AUD GetSampleSize ERR! res=%d sample_num=%d \n",res, pIsomGlob->a_current_sample_num);
						break;
					}

					if(readSize+sampSize<=size)
					{
						useTmp=0;
						pSamp=pOut;
						sampSize=size-readSize;
					}
					else if(sampSize<=pIsomGlob->a_SampBufMirrorLen)
					{
						useTmp=1;
						pSamp=pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
						sampSize=pIsomGlob->a_SampBufMirrorLen;
					}
					else
					{
						diag_printf("[VID] AUD ERR! sample size=%d bigger than a_SampBufMirrorLen=%d",sampSize, pIsomGlob->a_SampBufMirrorLen);
						break;
					}

					res=gf_isom_get_multSample((GF_MediaInformationBox *)pIsomGlob->a_info,
												pIsomGlob->a_current_sample_num+1,
												pSamp,
												sampSize,
												&gotbytes, &gotcount);

					if(res || (gotbytes<=0))
					{
						diag_printf("[VID]AUD get_multSample res=%d gotbytes=%d sampSize=%d sample_num=%d",res, gotbytes, sampSize, pIsomGlob->a_current_sample_num);
						break;
					}

					if(useTmp==0)
					{
						readSize+=gotbytes;
						pOut+=gotbytes;
					}
					else
					{
						pIsomGlob->a_SampTmpRemain=gotbytes;
						res=size-readSize;
						if(res>pIsomGlob->a_SampTmpRemain)
							res=pIsomGlob->a_SampTmpRemain;
						memcpy(pOut, pIsomGlob->a_SampTmpBufp, res);
						pIsomGlob->a_SampTmpRemain-=res;
						pIsomGlob->a_SampTmpBufp+=res;
						pOut+=res;
						readSize+=res;
					}
					pIsomGlob->a_current_sample_num+=gotcount;

				}
			}
			else
			{
avi_audio:
        if((res=avi_read_audio(pIsomGlob->avi_p, pOut, size-readSize))<=0)
				{
					diag_printf(  "[VID]avi_read_audio res =%d audio_posc=%d", res, pIsomGlob->avi_p->audio_posc);

					if (pIsomGlob->avi_p->audio_chunks <= pIsomGlob->avi_p->audio_posc+1) {
            	res=0;
					} else {
  					  pIsomGlob->a_current_sample_num=pIsomGlob->avi_p->audio_posc+1;
  						res =  avi_set_audio_position2(pIsomGlob->avi_p, pIsomGlob->a_current_sample_num);
  						if(res!=0)
  						{
  							diag_printf("[VID] avi_set_audio_position2 err! res=%d ", res);
  						//res=ISOM_ERR_AUDIO_READ_FAIL;
  						  break;
  						}
              goto avi_audio;
					}
        }
				readSize+=res;
				pIsomGlob->a_current_sample_num=pIsomGlob->avi_p->audio_posc+1;
				break;
			}
		}
		else
		{
			break;
		}
	}

	return readSize;
}

static void fillSampBuf()
{
	uint8* lftMroEnd;
	uint8* ritMroStart;
	int32 fillSize;
	int32 readSize=0;

	if(pIsomGlob->med_type==MCI_TYPE_AVSTRM)
	{
    		avinfostr *avinfohdl= (avinfostr*)pIsomGlob->filehdl;

   		avstream_clt_checkagent(avinfohdl->avstreamhdl_a);
		return;
	}

	// left mirror end position
	lftMroEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
	ritMroStart=lftMroEnd+pIsomGlob->a_SampBufPipeLen;

	while(1)
	{
		if(pIsomGlob->a_SampEnd+PIPE_SPACE<pIsomGlob->a_SampStart)
		{
			fillSize=pIsomGlob->a_SampStart-pIsomGlob->a_SampEnd-PIPE_SPACE;
			if(pIsomGlob->a_SampEnd>=lftMroEnd)
			{
				readSize=readSamp(pIsomGlob->a_SampEnd, fillSize);
				pIsomGlob->a_SampEnd+=readSize;

			}
			else
			{
				if(fillSize<=lftMroEnd-pIsomGlob->a_SampEnd)
				{
					readSize=fillSize;
					memcpy(pIsomGlob->a_SampEnd, ritMroStart+(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf), readSize);
					pIsomGlob->a_SampEnd+=readSize;
				}
				else
				{
					readSize=lftMroEnd-pIsomGlob->a_SampEnd;
					memcpy(pIsomGlob->a_SampEnd, ritMroStart+(pIsomGlob->a_SampEnd-pIsomGlob->a_SampBuf), readSize);
					pIsomGlob->a_SampEnd+=readSize;

					readSize=readSamp(pIsomGlob->a_SampEnd, fillSize-readSize);
					pIsomGlob->a_SampEnd+=readSize;
				}
			}

			break;
		}
		else if(pIsomGlob->a_SampEnd>=pIsomGlob->a_SampStart)
		{
			if(pIsomGlob->a_SampEnd>=ritMroStart+pIsomGlob->a_SampBufMirrorLen)
			{
				if(pIsomGlob->a_SampStart>=pIsomGlob->a_SampBuf+PIPE_SPACE)
				{
					pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf;
					continue;
				}
				else
					break;
			}

			fillSize=ritMroStart+pIsomGlob->a_SampBufMirrorLen-pIsomGlob->a_SampEnd;
			readSize=readSamp(pIsomGlob->a_SampEnd, fillSize);
			pIsomGlob->a_SampEnd+=readSize;

			if(readSize<fillSize)
				break;
		}
		else
			break;
	}
}

static void get_amr_mode(uint8 *pbuf, uint32 *pmode, uint32 *pframesize )
{
	//read amr ring header mode.
	//mode 8,15 and other will replace with 475 silence data and drop original data
      switch((pbuf[0] >> 3) & 0xF)
        {
            case 0:
                *pframesize = 13;
                *pmode = HAL_AMR475_DEC;
		//diag_printf("[VID] AMR mode 0, 4.75");
                break;
            case 1:
               *pframesize = 14;
                *pmode = HAL_AMR515_DEC;
		//diag_printf("[VID] AMR mode 1, 5.15");
                break;
            case 2:
               *pframesize = 16;
                *pmode = HAL_AMR59_DEC;
		//diag_printf("[VID] AMR mode 2, 5.9");
                break;
            case 3:
               *pframesize = 18;
                *pmode = HAL_AMR67_DEC;
		//diag_printf("[VID] AMR mode 3, 6.7");
                break;
            case 4:
               *pframesize = 20;
                *pmode = HAL_AMR74_DEC;
		//diag_printf("[VID] AMR mode 4, 7.4");
                break;
            case 5:
               *pframesize = 21;
                *pmode = HAL_AMR795_DEC;
		//diag_printf("[VID] AMR mode 5, 7.95");
                break;
            case 6:
               *pframesize = 27;
                *pmode = HAL_AMR102_DEC;
		//diag_printf("[VID] AMR mode 6, 10.2");
                break;
            case 7:
               *pframesize = 32;
                *pmode = HAL_AMR122_DEC;
		//diag_printf("[VID] AMR mode 7, 12.2");
                break;
	     case 8:
               *pframesize = 6;
                *pmode = HAL_INVALID;
		diag_printf("[VID] AMR mode 8, SID");
                break;
            case 15:
               *pframesize = 1;
                *pmode = HAL_INVALID;
		diag_printf("[VID] AMR mode 15, NO DATA");
                break;

            default:
	        //pIsomGlob->open_audio=0;
               *pframesize = 1;//move position 1byte
                *pmode = HAL_INVALID;
                diag_printf("[VID] WARNING! AMR head=%x , unsupport now!!! Audio Turn off!",pbuf[0]);
                break;
        }

}

/*
audio linear fade in
process couple of sample each time(assume 2 channel)
int16 *pPcm	//buffer in
uint32 smpNum	// total sample count
*/
static void audFadeIn(int16 *pPcm, uint32 smpNum)
{
	int32 i;
	int32 pcm1,pcm2;
	int32 scale,scale2;
//int32 tt1,tt2;
//tt1=hal_TimGetUpTime();

	//32767=0x7FFF
	if(pPcm && smpNum>2)
	{
		smpNum>>=1;
		smpNum<<=1;
		scale=(32767+(smpNum>>1))/smpNum;
		if(scale==0)scale=1;
		for(i=0;i<smpNum;i+=2)
		{
			pcm1=(int32)*pPcm;
			pcm2=(int32)*(pPcm+1);
			scale2=i*scale;
			if(scale2>32767)break;
			*pPcm++=(int16)((pcm1*scale2)>>15);// 32767
			*pPcm++=(int16)((pcm2*scale2)>>15);// 32767
			//if(i>smpNum-64)
			//diag_printf("%d %d %d %d",pcm1,pcm2,*(pPcm-2),*(pPcm-1));
		}
	}
//tt2=hal_TimGetUpTime();
//diag_printf("fade in time=%d",(tt2-tt1));//*1000/16384
}

/*
audio linear fade out
process couple of sample each time(assume 2 channel)
int16 *pPcm	//buffer in
uint32 smpNum	// total sample count
*/

static void audFadeOut(int16 *pPcm, uint32 smpNum)
{
	int32 i;
	int32 pcm1,pcm2;
	int32 scale,scale2;
//int32 tt1,tt2;
//tt1=hal_TimGetUpTime();

	//32767=0x7FFF
	if(pPcm && smpNum>2)
	{
		smpNum>>=1;
		smpNum<<=1;
		scale=(32767+(smpNum>>1))/smpNum;
		if(scale==0)scale=1;
		scale2=32767;
		for(i=0;i<smpNum;i+=2)
		{
			scale2=scale2-scale;

			if(scale2>0)
			{
				pcm1=(int32)*pPcm;
				pcm2=(int32)*(pPcm+1);
				*pPcm++=(int16)((pcm1*scale2)>>15);// 32767
				*pPcm++=(int16)((pcm2*scale2)>>15);// 32767
			}
			else
			{
				*pPcm++=0;
				*pPcm++=0;
			}

			//if(i>smpNum-64)
			//diag_printf("%d %d %d %d",pcm1,pcm2,*(pPcm-2),*(pPcm-1));
		}
	}
//tt2=hal_TimGetUpTime();
//diag_printf("fade out time=%d",(tt2-tt1));//*1000/16384
}

static void fillAudioDataFun(void)
{
	fillSampBuf();

	if(pIsomGlob->a_type== A_TYPE_PCM && AUD_DEC_FRM_NUM>0)
	{
		int res;
		while(getDataLen()>0)
		{
			if(AUD_FADE_OUT_FLAG==0)
				res=getData((unsigned char*)pPcmBufPos, pIsomGlob->a_frameBytes, 1);
			else
			{
				moveDataPos(pIsomGlob->a_frameBytes);
				res=1;
			}

			if(res<=0)break;

			AUD_DEC_FRM_NUM-=(uint16)res;

			if((res>0)&&(pIsomGlob->a_channels==2)&&(audioItf==AUD_ITF_LOUD_SPEAKER))
			{
				uint32 i;
				INT16 *pPcm=(INT16 *)pPcmBufPos;
				uint32 sNum=pIsomGlob->a_frame_sample*2;

				for(i=0;i<sNum;i+=2)
				{
					pPcm[i]=pPcm[i+1]=(pPcm[i]>>1)+(pPcm[i+1]>>1);
				}
			}

			pPcmBufPos+=pIsomGlob->a_frameBytes;
			fillSampBuf();

			if(AUD_DEC_FRM_NUM==0)break;
		}

		if(AUD_DEC_FRM_NUM>0)
		{
			memset((void*)pPcmBufPos, 0, pIsomGlob->a_frameBytes*AUD_DEC_FRM_NUM);
			AUD_DEC_FRM_NUM=0;
			diag_printf("fillAudioDataFun pcm over");
		}
	}
}

static void audio_voc_dec()
{

	if(pIsomGlob->a_type != A_TYPE_AMR)
	{
		vocDecAud.inStreamBufAddr=(INT32 *)getDataPos();
		vocDecAud.outStreamBufAddr=(INT32 *)pPcmBufPos;
		vocDecAud.audioItf=audioItf;
		if(vpp_AudioJpegDecScheduleOneFrame(&vocDecAud)!=HAL_ERR_NO)//aac 1024ֽڻ
		{
			VOC_WORK=VOC_WORK_NON;
			diag_printf("[VID]voc call err!");
		}
	}
	else
	{
		HAL_SPEECH_DEC_IN_T *pVocDecAmr=vpp_SpeechGetRxCodBuffer();
		get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));
		if(pIsomGlob->a_amr_mode != HAL_INVALID)
		{
			getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
			pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
		}
		else
		{
			//write silence data
			moveDataPos(pIsomGlob->a_codeframeBytes);
			memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
			pVocDecAmr->codecMode= HAL_AMR475_DEC;
		}

		// Set all the preprocessing modules
		vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
		if(vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC) != HAL_ERR_NO)
		{
			VOC_WORK=VOC_WORK_NON;
			diag_printf("[VID]voc call err!");
		}
	}
}


static void aud_pcm_isr(int flag)
{
	long long syncF;
	uint8 pcmUpdate=0;

	if(VOC_AUD == 0|| pIsomGlob->open_audio==2)
	{
		pIsomGlob->a_current_frame_num += pIsomGlob->a_HalfPcmBufFrame;
	}

  if(flag == 1)
  {
        //diag_printf("1 %d %d",pIsomGlob->a_current_frame_num,pIsomGlob->v_current_frame_num);
		if(pIsomGlob->med_type!=MCI_TYPE_RM)
		{
			if(DTS_FLG ==0xFFFFFFFF)
			{
				if(pIsomGlob->med_type!=MCI_TYPE_AVSTRM)
				{
						pIsomGlob->current_time= ((long long)pIsomGlob->a_frame_sample*(long long)pIsomGlob->a_current_frame_num*1000)/(long long)pIsomGlob->a_sample_rate;
				}
			}
			else
			{
				pIsomGlob->current_time= ((long long)pIsomGlob->a_frame_sample*(long long)pIsomGlob->a_current_frame_num*1000+ (long long)(pIsomGlob->a_sample_rate>>1))/(long long)pIsomGlob->a_sample_rate;
			//	if(DTS_SampDelta)
			//		VID_DEC_REQ= ((long long)pIsomGlob->current_time*(long long)pIsomGlob->v_timeScale - (long long)DTS_FLG*1000 )/((long long)DTS_SampDelta*1000);
			}
		}
  }

	if(AUD_FADE_OUT_FLAG == 3)
	{
		//fade out  last frame
  		uint8* srcAddr = (flag == 0) ? pIsomGlob->a_PcmBufHalfPos : pIsomGlob->a_PcmBuf;
			audFadeOut((int16*) (srcAddr + pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame-1)),
			    pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
	}

  uint8* bufAddr = (flag == 0) ? pIsomGlob->a_PcmBuf : pIsomGlob->a_PcmBufHalfPos;

	if(pIsomGlob->a_type!= A_TYPE_PCM)
	{
		if(aud_extra_pcmbuf_state[A_EXBF_RD_ID].done==1)
		{
			if(AUD_FADE_OUT_FLAG==0)
			{
				memcpy(bufAddr, (VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
				
				/*
				if((my_input->bt_sco==TRUE) &&(pIsomGlob->a_type != A_TYPE_AMR))
				{
							g_VidBTDataAddress = pIsomGlob->a_PcmBuf;
							if(pIsomGlob->a_channels==2)
							{
								int i;
								INT16 *PCM_p=(INT16 *)g_VidBTDataAddress;
								for(i=0;i< g_VidBtOutputPcmBufSize ;i=i+2)
								{
									PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);
								}
							}

							g_VidBtNout=ResampleOneFrame(g_VidBTDataAddress,g_VidBT8kPcmBuffer+g_VidBtcounterframe*g_VidBtNumOfOutSample);
							g_VidBtcounterframe++;
							if(g_VidBtcounterframe >= g_VidBtTotalFrame)
								g_VidBtcounterframe = 0;
				          		MCI_TRACE (TSTDOUT,0,"g_VidBtNout55555 = %d",g_VidBtNout);
				}
				*/
			}
			  
			aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
			A_EXBF_RD_ID += 1;
			if(A_EXBF_RD_ID >= EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;

			if(VOC_WORK == VOC_WORK_NON && VOC_SETTING == 0 && getDataLen()>0)
			{
                VOC_WORK  = VOC_WORK_AUD;
	            pPcmBufPos= (uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                audio_voc_dec();
			}
			pcmUpdate = 1;
		}
		else
		{
			if(VOC_WORK == VOC_WORK_NON && VOC_SETTING == 0 && getDataLen()>0)
			{
                VOC_AUD++;
                VOC_WORK  = VOC_WORK_AUD;
	            pPcmBufPos= (uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
                audio_voc_dec();
			}
			else if(getDataLen()>0)
			{
				//if(VOC_AUD!=0)
				//{
					//AUD_DEC_MIN=0;
				//}
				VOC_AUD++;
			}
			pcmUpdate = 0;
		}
	}
	else
	{
		pPcmBufPos=(uint32)bufAddr;
		AUD_DEC_FRM_NUM=(uint16)pIsomGlob->a_HalfPcmBufFrame;
		pcmUpdate=1;

	}

	if(AUD_FADE_OUT_FLAG > 0)
	{
		memset((uint8*)bufAddr, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
		if(AUD_FADE_OUT_FLAG>1)AUD_FADE_OUT_FLAG--;
		pcmUpdate=1;
	}

	if(SEND_EVENT_NUM == 0 )
	{
		SEND_EVENT_NUM = 1;
		mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
		//diag_printf("0 pcm send");
	}
	
	if(pcmUpdate==0)
	{
		memset(bufAddr, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
		diag_printf("[VID]pcm isr, aud no data");
	}
}

static void aud_pcmHalf_isr ()
{
	long long syncF;
	uint8 pcmUpdate = 0;

	PCM_ISR_FLAG = 0;
    aud_pcm_isr(PCM_ISR_FLAG);
}

static void aud_pcmEnd_isr ()
{
	long long syncF;
	uint8 pcmUpdate = 0;

	PCM_ISR_FLAG=1;
    aud_pcm_isr(PCM_ISR_FLAG);
}


static void vid_update(void )
{

}


static void aud_voc_isr(HAL_VOC_IRQ_STATUS_T * status )
{
	int16 av_check = 0;
	
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle = MpegPlayer.MpegInput.audio_config_handle ;

    if(VOC_WORK==VOC_WORK_AUD)
    {
    	if(pIsomGlob->a_type != A_TYPE_AMR)
    	{
    		vpp_AudioJpegDecStatus(&vocDecStatus);
    		if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type ==1)&&(pIsomGlob->a_type != A_TYPE_AMR) )      //BT_A2DP_sbc
    				my_ss_handle->PutData(my_ss_handle, (UINT8*) hal_VocGetPointer(SBC_Out_put_addr), vocDecStatus.ImgWidth);

    		if(vocDecStatus.ErrorStatus)
    		{
    			if((uint8*)pPcmBufPos +pIsomGlob->a_frameBytes<= aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame)
    			{
    				memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
    				diag_printf("[VId]aud err. output_len=%d consume %d", vocDecStatus.output_len,vocDecStatus.consumedLen);
    			}
    			else
    			{
    				diag_printf("[VId]WARNING!aud err overflow, a_frameBytes=%d a_PcmFrameTempBuf=%x end=%x", pIsomGlob->a_frameBytes,
    					(uint8*)pPcmBufPos, aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
    		    }
    		}
    		moveDataPos((int)vocDecStatus.consumedLen);
    	}
    	else
    	{
    		// or 0x20000000 and unuse cache for get output data update.
    		memcpy((VOID*)pPcmBufPos,(VOID*)(((UINT32)( vpp_SpeechGetRxPcmBuffer())|0x20000000) + RX_BUF_SWAP_ID*HAL_SPEECH_FRAME_SIZE_PCM_BUF),
    			    pIsomGlob->a_frameBytes);
    		RX_BUF_SWAP_ID = 1 - RX_BUF_SWAP_ID;
    	}

    	AUD_DEC_FRM_NUM += 1;
    	if(AUD_DEC_FRM_NUM >= pIsomGlob->a_HalfPcmBufFrame)
    	{
    		AUD_DEC_FRM_NUM=0;
    		if(VOC_AUD>0)
    		{
    			if(AUD_FADE_OUT_FLAG==0)
    			{
    				if(PCM_ISR_FLAG==0)
    					memcpy(pIsomGlob->a_PcmBuf, (VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
    				else
    					memcpy(pIsomGlob->a_PcmBufHalfPos, (VOID*)((uint32)(aud_extra_pcmbuf_state[A_EXBF_RD_ID].pBufPos)|0x20000000), pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
    			}

    			aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
    			A_EXBF_WT_ID+=1;
    			if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;

    			aud_extra_pcmbuf_state[A_EXBF_RD_ID].done=0;
    			A_EXBF_RD_ID+=1;
    			if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;

    			VOC_AUD--;
    		}
    		else
    		{
    			aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
    			A_EXBF_WT_ID+=1;
    			if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;
    		}
    	}

    	//mute frame
    	if(AUD_FADE_IN_FLAG>1)
    	{
    		memset((uint8*)pPcmBufPos, 0, pIsomGlob->a_frameBytes);
    		AUD_FADE_IN_FLAG--;
    	}
    	else if(AUD_FADE_IN_FLAG==1)
    	{
    		audFadeIn((int16*)pPcmBufPos, pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
    		AUD_FADE_IN_FLAG=0;
    	}
    }

	//audio check
	if(pIsomGlob->open_audio == 2 && pIsomGlob->a_type != A_TYPE_COOK && pIsomGlob->a_type != A_TYPE_PCM)
	{
		if(getDataLen() > 0)
		{
			if((VOC_AUD > 0 )||(aud_extra_pcmbuf_state[A_EXBF_WT_ID].done==0))
			{
			//diag_printf("isr %d 1",VOC_WORK);
				VOC_WORK=VOC_WORK_AUD;

				if(AUD_DEC_FRM_NUM!=0)
				{
					pPcmBufPos+=pIsomGlob->a_frameBytes;
				}
				else
				{
					pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[A_EXBF_WT_ID].pBufPos);
				}
                audio_voc_dec();
	
				return;
			}
		}
		else
		{
			//AUD_DATA_REQ=0;
			if(AUD_DATA_REQ==1)AUD_DATA_REQ=2;
			//diag_printf( "[VID] voc isr, aud no data");
		}

		AUD_DEC_FRM_NUM=0;
		VOC_AUD=0;
	}

	VOC_WORK = VOC_WORK_NON;

}


//#define DEBUG_PRINT
static int32 aud_av_dec_frame(void)
{
    int32 res;

    if(play_state != VID_PLAY_STATE_PLAYING)
    {
        if(play_state == VID_PLAY_STATE_PAUSE)
        {
            pIsomGlob->a_Pause_DecReq = 1;
            diag_printf("[VID] aud_av_dec_frame pause state! \n");
            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
            return ISOM_ERR_OK;
        }
        else
        {
            diag_printf("[VID] aud_av_dec_frame play_state= %d not match\n",play_state);
            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);
            return ISOM_ERR_PLAY_STATE_FAIL;
        }
    }
    
	fillAudioDataFun();
	
	//diag_printf("[VID]pIsomGlob->current_time：%ld\n", pIsomGlob->current_time);
	if(pIsomGlob->current_time >= pIsomGlob->total_time)
	{
		diag_printf("[VID] audio only play time over!");
		mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
	}
    
	return ISOM_ERR_OK;
}


static int32 aud_export_info_from_parse(GF_ISOFile *file,uint8 open_audio)
{
	GF_DecoderConfig *temp_dcfg=NULL;
	u32 trackID, i,  m_type, m_stype;
	u32 track_count=0;
	u32 av_track=0;

	pIsomGlob->v_type= AV_TYPE_INVALID;
	pIsomGlob->a_type= AV_TYPE_INVALID;

	track_count = gf_isom_get_track_count(file);

	if(track_count<1)
	{
		//diag_printf("[VID] unsupport only video or only audio! track_count=%d\n", track_count);
		diag_printf("[VID] error track_count=%d\n", track_count);
		return ISOM_ERR_UNSUPPORT;
	}


	for(i=0;i<track_count/*MAX_TRACK_NUM*/;i++)
	{
		//trackID = gf_isom_get_track_by_id(file, i+1);
		trackID=i+1;

		m_type = gf_isom_get_media_type(file, trackID);//vide soun
		m_stype = gf_isom_get_media_subtype(file, trackID, 1);

		if(m_type==GF_ISOM_MEDIA_VISUAL)
		{
			diag_printf("[VID]v m_type=%x trackID=%d\n",m_type,trackID);
			if(pIsomGlob->v_type != AV_TYPE_INVALID)
				continue;
		}
		else if(m_type==GF_ISOM_MEDIA_AUDIO)
		{
			diag_printf("[VID]a m_type=%x trackID=%d\n",m_type,trackID);
			if(pIsomGlob->a_type != AV_TYPE_INVALID)
				continue;
		}
		else
		{
			diag_printf("[VID]discard m_type=%x trackID=%d\n",m_type,trackID);
			continue;
		}

		temp_dcfg=NULL;

		if ((m_stype==GF_ISOM_BOX_TYPE_MP4V) ||(m_stype==GF_ISOM_BOX_TYPE_MP4A))
		    temp_dcfg = gf_isom_get_decoder_config(file, trackID, 1);//esds

		if(m_type==GF_ISOM_MEDIA_VISUAL)
		{
			pIsomGlob->v_trackID=trackID;
			pIsomGlob->v_trak= gf_isom_get_track_from_file(file,trackID);
			if(!pIsomGlob->v_trak)
			{
				diag_printf("[VID]invalid gf_isom_get_track_from_file\n");
				continue;
			}

			if(pIsomGlob->v_trak->Media)
			{
				if(pIsomGlob->v_trak->Media->information)
				{
					pIsomGlob->v_info=(GF_MediaInformationBox *)pIsomGlob->v_trak->Media->information;
					pIsomGlob->v_info->lastSampleNumber=0;
					pIsomGlob->v_info->lastFirstSample=0;
					pIsomGlob->v_info->lastOffset=0;

					if(!pIsomGlob->v_info->dataHandler)
					{
						if(pIsomGlob->v_trak->Media->mediaTrack)
						{
							if(pIsomGlob->v_trak->Media->mediaTrack->moov)
							{
								if(pIsomGlob->v_trak->Media->mediaTrack->moov->mov)
								{
									if(pIsomGlob->v_trak->Media->mediaTrack->moov->mov->movieFileMap)
										pIsomGlob->v_info->dataHandler = pIsomGlob->v_trak->Media->mediaTrack->moov->mov->movieFileMap;
									else
									{
										diag_printf("[VID]invalid pIsomGlob->v_trak->Media 5\n");
										continue;
									}
								}
								else
								{
									diag_printf("[VID]invalid pIsomGlob->v_trak->Media 4\n");
									continue;
								}
							}
							else
							{
								diag_printf("[VID]invalid pIsomGlob->v_trak->Media 3\n");
								continue;
							}
						}
						else
						{
							diag_printf("[VID]invalid pIsomGlob->v_trak->Media 2\n");
							continue;
						}
					}
				}
				else
				{
					diag_printf("[VID]invalid pIsomGlob->v_trak->Media 1\n");
					continue;
				}
			}
			else
			{
				diag_printf("[VID]invalid pIsomGlob->v_trak->Media 0\n");
				continue;
			}

 			if(pIsomGlob->v_info->sampleTable)
 			{
 				if(pIsomGlob->v_info->sampleTable->TimeToSample)
 				{
					 if(stbl_GetSampleDTScount(pIsomGlob->v_info->sampleTable->TimeToSample)>1)
					 {
					 	DTS_FLG=0;
						if(pIsomGlob->v_trak->Media->mediaHeader)
						{
							pIsomGlob->v_timeScale=pIsomGlob->v_trak->Media->mediaHeader->timeScale;
						}
						else
						{
							diag_printf("[VID]invalid mediaHeader");
							continue;
						}
					 }
					 else
					 {
					 	DTS_FLG=0xFFFFFFFF;
						pIsomGlob->v_timeScale=0;
					 }
 				}
				else
				{
					diag_printf("[VID]invalid TimeToSample");
					continue;
				}
 			}
			else
			{
				diag_printf("[VID]invalid sampleTable");
				continue;
			}

			pIsomGlob->total_time=gf_isom_get_sample_totaltime(file, pIsomGlob->v_trackID);
			pIsomGlob->v_fps= gf_isom_get_sample_FPS(file, pIsomGlob->v_trackID);//fps<<V_FPS_SCALE_BIT
			pIsomGlob->v_height= gf_isom_get_sample_height(file, pIsomGlob->v_trackID);
			pIsomGlob->v_width= gf_isom_get_sample_width(file, pIsomGlob->v_trackID);
			pIsomGlob->v_total_sample_num = gf_isom_get_sample_count(file, pIsomGlob->v_trackID);//total sample(frame) Num
			pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num;//will change
			pIsomGlob->v_total_keyframe_num=gf_isom_get_total_KeySamp_count(file, pIsomGlob->v_trackID);

		}
		else if (m_type==GF_ISOM_MEDIA_AUDIO)
		{
			pIsomGlob->a_trackID=trackID;
			pIsomGlob->a_trak= gf_isom_get_track_from_file(file,trackID);
			if(!pIsomGlob->a_trak)
			{
				diag_printf("[VID]invalid gf_isom_get_track_from_file\n");
				continue;
			}

			if(pIsomGlob->a_trak->Media)
			{
				if(pIsomGlob->a_trak->Media->information)
				{
					pIsomGlob->a_info=(GF_MediaInformationBox *)pIsomGlob->a_trak->Media->information;
					pIsomGlob->a_info->lastSampleNumber=0;
					pIsomGlob->a_info->lastFirstSample=0;
					pIsomGlob->a_info->lastOffset=0;

					if(!pIsomGlob->a_info->dataHandler)
					{
						if(pIsomGlob->a_trak->Media->mediaTrack)
						{
							if(pIsomGlob->a_trak->Media->mediaTrack->moov)
							{
								if(pIsomGlob->a_trak->Media->mediaTrack->moov->mov)
								{
									if(pIsomGlob->a_trak->Media->mediaTrack->moov->mov->movieFileMap)
										pIsomGlob->a_info->dataHandler = pIsomGlob->a_trak->Media->mediaTrack->moov->mov->movieFileMap;
									else
									{
										diag_printf("[VID]invalid Media 5\n");
										continue;
									}
								}
								else
								{
									diag_printf("[VID]invalid Media 4\n");
									continue;
								}
							}
							else
							{
								diag_printf("[VID]invalid Media 3\n");
								continue;
							}
						}
						else
						{
							diag_printf("[VID]invalid Media 2\n");
							continue;
						}
					}
				}
				else
				{
					diag_printf("[VID]invalid Media 1\n");
					continue;
				}
			}
			else
			{
				diag_printf("[VID]invalid Media 0\n");
				continue;
			}

			pIsomGlob->a_sample_rate=gf_isom_get_sample_samplerate(file, pIsomGlob->a_trackID);
			pIsomGlob->a_bit=gf_isom_get_sample_bitspersample(file, pIsomGlob->a_trackID);
			if (temp_dcfg)
				pIsomGlob->a_bitrate=temp_dcfg->maxBitrate;
			pIsomGlob->a_channels=gf_isom_get_sample_channel_count(file, pIsomGlob->a_trackID);
			pIsomGlob->a_total_sample_num = gf_isom_get_sample_count(file, pIsomGlob->a_trackID);//total sample(chunk) Num
			//pIsomGlob->a_total_frame_num=pIsomGlob->a_total_sample_num;//will change
			pIsomGlob->a_total_frame_num=0;

		}

		if (temp_dcfg)
		{
			pIsomGlob->aacPara.is_aac=-1;
			pIsomGlob->aacPara.aac_type=0;

			switch (temp_dcfg->streamType)
			{
				case GF_STREAM_VISUAL:
					switch (temp_dcfg->objectTypeIndication) {
			        		//Extracting MPEG-4 Visual stream to m4v ".m4v"
					case 0x20:
						{
							//memcpy(pIsomGlob->v_SampleBuf,temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength);
							//pIsomGlob->v_SampleBuf_BytesRemain+=temp_dcfg->decoderSpecificInfo->dataLength;
							pIsomGlob->v_decSpeInfo=temp_dcfg->decoderSpecificInfo;
							diag_printf("[VID] MPEG4");
						}
						break;
			        		//Extracting MPEG-4 AVC-H264 stream to h264".h264"
					case 0x21:
			    			//avccfg = gf_isom_avc_config_get(file, pIsomGlob->v_trackID, 1);
						break;
			       		//Extracting MPEG-1 Visual stream to h264".m1v"
					case 0x6A:
						break;
			        		//Extracting MPEG-2 Visual stream to h264".m2v"
					case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65:
						break;
			        		//Extracting JPEG image".jpg"
					case 0x6C:
						break;
			        		//Extracting PNG image".png"
					case 0x6D:
						break;
					default:
						diag_printf("[VID]invalid objectTypeIndication %x\n",temp_dcfg->objectTypeIndication);
						continue;
					}
					break;
				case GF_STREAM_AUDIO:
					switch (temp_dcfg->objectTypeIndication)
					{
				       		 //Extracting MPEG-2 AAC".aac"
						case 0x66:
						case 0x67:
						case 0x68:
							pIsomGlob->aacPara.is_aac = A_TYPE_MPEG2_AAC;
							pIsomGlob->aacPara.aac_type = temp_dcfg->objectTypeIndication - 0x66;
							gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &pIsomGlob->a_cfg);
							diag_printf("[VID] MPEG2_AAC");
							break;
				        		//Extracting MPEG-4 AAC ".aac"
						case 0x40:
							pIsomGlob->aacPara.is_aac = A_TYPE_MPEG4_AAC;
							gf_m4a_get_config(temp_dcfg->decoderSpecificInfo->data, temp_dcfg->decoderSpecificInfo->dataLength, &pIsomGlob->a_cfg);
							pIsomGlob->aacPara.aac_type = pIsomGlob->a_cfg.base_object_type - 1;
							diag_printf("[VID] MPEG4_AAC");
							break;
				        		//Extracting MPEG-1/2 Audio (MP3)".mp3"
						case 0x69:
						case 0x6B:
							break;
						default:
							diag_printf("[VID] invalid temp_dcfg->objectTypeIndication=%x \n",temp_dcfg->objectTypeIndication);
				            		continue;
					}
				    	break;
				default:
					diag_printf("[VID] invalid temp_dcfg->streamType=%x \n",temp_dcfg->streamType);
					continue;
			}

			if (temp_dcfg->streamType==GF_STREAM_AUDIO && pIsomGlob->aacPara.is_aac != -1)
			{
				GF_BitToInt bs;
				memset(aacHead, 0, AAC_HEAD_SIZE);
				bs.stream=aacHead;
				bs.current=0;
				bs.nbBits=0;

				gf_bs_bit_to_int(&bs, 0xFFF, 12);/*sync*/
				gf_bs_bit_to_int(&bs, (pIsomGlob->aacPara.is_aac==A_TYPE_MPEG2_AAC) ? 1 : 0, 1);/*mpeg2 aac*/
				gf_bs_bit_to_int(&bs, 0, 2); /*layer*/
				gf_bs_bit_to_int(&bs, 1, 1); /* protection_absent*/
				gf_bs_bit_to_int(&bs, pIsomGlob->aacPara.aac_type, 2);
				gf_bs_bit_to_int(&bs, pIsomGlob->a_cfg.base_sr_index, 4);
				gf_bs_bit_to_int(&bs, 0, 1);
				gf_bs_bit_to_int(&bs, pIsomGlob->a_cfg.nb_chan, 3);
				gf_bs_bit_to_int(&bs, 0, 4);
				//gf_bs_bit_to_int(&bs, AAC_HEAD_SIZE+dataLength, 13);
				gf_bs_bit_to_int(&bs, 0, 13);//13bit will be rewrite when every sample read
				gf_bs_bit_to_int(&bs, 0x7FF, 11);
				gf_bs_bit_to_int(&bs, 0, 2);
			}
			//gf_odf_desc_del((GF_Descriptor *) dcfg);
		}
#ifdef H263_SUPPORT
		else if(m_stype==GF_ISOM_SUBTYPE_3GP_H263) //add shenh
		{
			//return ISOM_ERR_UNSUPPORT;//change shenh
			pIsomGlob->h263Sp=gf_isom_get_h263_spec(file, pIsomGlob->v_trackID);
			if(!pIsomGlob->h263Sp) return ISOM_ERR_UNSUPPORT;
			diag_printf("[VID] H263");
		}
#endif
		else if(m_stype==GF_ISOM_SUBTYPE_AVC_H264)
		{
			pIsomGlob->h264Cfg=gf_isom_get_avc_config(file, pIsomGlob->v_trackID);
			if(!pIsomGlob->h264Cfg) return ISOM_ERR_UNSUPPORT;
			diag_printf("[VID] H264");
		}
		else if(m_stype==GF_ISOM_SUBTYPE_3GP_JPEG)
		{
			diag_printf("[VID] JPEG");
		}
		else if(m_stype==GF_ISOM_SUBTYPE_3GP_AMR)//add shenh
		{
			//return ISOM_ERR_UNSUPPORT;//change shenh
			pIsomGlob->amrSp=gf_isom_get_amr_spec(file, pIsomGlob->a_trackID);
			if(!pIsomGlob->amrSp) return ISOM_ERR_UNSUPPORT;
			diag_printf("[VID] AMR");
		}
		else if(m_stype==GF_ISOM_BOX_TYPE_MP3)
		{
			diag_printf("[VID] MP3");
		}
		else
		{
			/*
			udesc = gf_isom_get_generic_sample_description(file, trackID, 1);
			if (udesc) {
				dsi = udesc->extension_buf; udesc->extension_buf = NULL;
				dsi_size = udesc->extension_buf_size;
			}
			if (udesc) free(udesc);
			*/
			diag_printf("[VID] invalid  m_stype=%x \n",m_stype);
			continue;//change shenh
		}

		if(m_type==GF_ISOM_MEDIA_VISUAL)
		{
			if(m_stype==GF_ISOM_BOX_TYPE_MP4V)
				pIsomGlob->v_type=V_TYPE_MPEG4;
			else if(m_stype==GF_ISOM_BOX_TYPE_S263)
				pIsomGlob->v_type=V_TYPE_H263;
			else if(m_stype==GF_ISOM_SUBTYPE_AVC_H264)
				pIsomGlob->v_type=V_TYPE_H264;
			else if(m_stype==GF_ISOM_BOX_TYPE_JPEG)
				pIsomGlob->v_type=V_TYPE_MJPEG;

		}
		else if (m_type==GF_ISOM_MEDIA_AUDIO)
		{
			if(m_stype==GF_ISOM_BOX_TYPE_MP4A)
				pIsomGlob->a_type=A_TYPE_AAC;
			else if(m_stype==GF_ISOM_BOX_TYPE_SAMR)
				pIsomGlob->a_type=A_TYPE_AMR;
			else if(m_stype==GF_ISOM_BOX_TYPE_MP3)
				pIsomGlob->a_type=A_TYPE_MP3;
		}

		av_track++;

		if(av_track>=2)
		{
			if(DTS_FLG !=0xFFFFFFFF)
			{//check invalid
				long long totalDuration1, totalDuration2, totalDuration3;
				long long timeScale1, timeScale2, timeScale3;
				//mvhd
				totalDuration1= file->moov->mvhd->duration;
				timeScale1= file->moov->mvhd->timeScale;
				//mdhd video
				totalDuration2= pIsomGlob->v_trak->Media->mediaHeader->duration;
				timeScale2 =pIsomGlob->v_trak->Media->mediaHeader->timeScale;
				//mdhd audio
				totalDuration3= pIsomGlob->a_trak->Media->mediaHeader->duration;
				timeScale3 =pIsomGlob->a_trak->Media->mediaHeader->timeScale;

				if(timeScale1 && timeScale2 && timeScale3)
				{
					if( ABS((totalDuration2/timeScale2) - (totalDuration3/timeScale3)) -ABS((totalDuration1/timeScale1) - (totalDuration3/timeScale3)) >3 )
					{
						long long sampleCount;
						DTS_FLG = 0xFFFFFFFF;//not use DTS
						pIsomGlob->v_timeScale=0;

						diag_printf("[VID]check totalDuration & timeScale for DTS. Old fps=%d",pIsomGlob->v_fps>>8);
						diag_printf("[VID]mvhd dur=%d scale=%d",(uint32)totalDuration1,(uint32)timeScale1);
						diag_printf("[VID]mdhd vid dur=%d scale=%d",(uint32) totalDuration2,(uint32) timeScale2);
						diag_printf("[VID]mdhd aud dur=%d scale=%d ",(uint32)totalDuration3,(uint32)timeScale3);

						if(totalDuration1 && pIsomGlob->v_trak->Media->information->sampleTable->SampleSize)
						{
							sampleCount= pIsomGlob->v_trak->Media->information->sampleTable->SampleSize->sampleCount;
							pIsomGlob->v_fps= ((sampleCount<<8)*timeScale1+(totalDuration1>>1))/totalDuration1;
						}
						diag_printf("[VID]Not use DTS and new fps=%d",pIsomGlob->v_fps>>8);

					}
				}
			}

			return ISOM_ERR_OK;
		}
	}

	if(pIsomGlob->v_type== AV_TYPE_INVALID && open_audio !=2)
	{
		diag_printf("[VID] unsupport  invalid video track!\n");
		return ISOM_ERR_UNSUPPORT;
	}
	else if(pIsomGlob->a_type== AV_TYPE_INVALID && open_audio !=0)
	{
		diag_printf("[VID] WARNING  invalid audio track!\n");
	}

	return ISOM_ERR_OK;
}

static int32 parse_media_type(HANDLE handle,  uint8 mode, uint32 Len)
{
   int32 iRet = 0;
   /******************************************************************
                     parse  file
    ******************************************************************/
	switch(pIsomGlob->med_type)
	{
		case MCI_TYPE_MP4:
		case MCI_TYPE_3GP:
			iRet= gf_isom_open_file( handle, mode, Len, &pIsomGlob->file);
			if (iRet)
			{
		    	diag_printf( "[VID] Error opening file \n");
				iRet=ISOM_ERR_OPEN_PARSE_FILE_FAIL;
				return iRet;
			}

  /******************************************************************
                     export  file
   ******************************************************************/
			if( (iRet=aud_export_info_from_parse(pIsomGlob->file, 2))!=ISOM_ERR_OK)
			{
				diag_printf("[VID] gf_media_export_info err res=%d \n", iRet);
				return iRet;
			}

		 	DTS_FLG=0xFFFFFFFF;
			pIsomGlob->v_timeScale=0;
			break;
            
		case MCI_TYPE_MJPG:
			pIsomGlob->avi_p=NULL;
			pIsomGlob->avi_p=avi_open_input_file(handle, mode, Len, 1);
			if(pIsomGlob->avi_p==NULL)
			{
				diag_printf( "[VID]##ERROR## avi_open_input_file error %d \n",avi_get_error());
				iRet=ISOM_ERR_OPEN_FILE_FAIL;
				return iRet;
			}

			if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_MJPG)
			{
				diag_printf("[VID] MJPEG");
				pIsomGlob->v_type=V_TYPE_MJPEG;
			}
			else if(((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_XVID)
				|| ((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_XVID2))
			{
				diag_printf("[VID] XVID");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DX50)
			{
				diag_printf("[VID] DX50");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DIV3)
			{
				diag_printf("[VID] DIV3");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if(((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_DIVX)
				|| ((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_divx))
			{
				diag_printf("[VID] DIVX");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_PM4V)
			{
				diag_printf("[VID] PM4V DIV4");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_FMP4)
			{
				diag_printf("[VID] FMP4");
				pIsomGlob->v_type=V_TYPE_MPEG4;
			}
			else if((uint32)*((uint32*)pIsomGlob->avi_p->compressor2)==AVI_FORMAT_AVC1)
			{
				diag_printf("[VID] AVC1");
				pIsomGlob->v_type=V_TYPE_H264;
			}
			else
			{
				iRet=ISOM_ERR_UNSUPPORT;
				diag_printf("[VID] unsupport video type=%x \n",*(uint32*)pIsomGlob->avi_p->compressor2);
				return iRet;
			}

			if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_AMR)
			{
				pIsomGlob->a_type=A_TYPE_AMR;
				diag_printf("[VID] AMR");
			}
			else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_AMR_2)
			{
				//same as AMR, only FCC different.
				pIsomGlob->a_type=A_TYPE_AMR;
				diag_printf("[VID] AMR 2");
			}
			else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_MP3)
			{
				pIsomGlob->a_type=A_TYPE_MP3;
				diag_printf("[VID] MP3");
			}
			else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_MP2)
			{
				pIsomGlob->a_type=A_TYPE_MP3;
				diag_printf("[VID] MP2");
			}
			else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_PCM)
			{
				pIsomGlob->a_type=A_TYPE_PCM;
				diag_printf("[VID] PCM");
			}
			else if(pIsomGlob->avi_p->a_fmt==AVI_FORMAT_ADPCM)
			{
				pIsomGlob->a_type=A_TYPE_ADPCM;
				iRet=ISOM_ERR_UNSUPPORT;
				diag_printf("[VID] unsupport audio ADPCM now \n");
				return iRet;
			}
			else
			{
				iRet=ISOM_ERR_UNSUPPORT;
				diag_printf("[VID] unsupport audio type=%x \n",pIsomGlob->avi_p->a_fmt);
				return iRet;
			}
            
			pIsomGlob->a_channels=(uint16)pIsomGlob->avi_p->a_chans;
			pIsomGlob->a_sample_rate=pIsomGlob->avi_p->a_rate;
			pIsomGlob->a_bit=pIsomGlob->avi_p->a_bits;
			pIsomGlob->a_bitrate=pIsomGlob->avi_p->mp3rate;
			pIsomGlob->a_total_sample_num=pIsomGlob->avi_p->audio_chunks;
			pIsomGlob->a_total_frame_num=0;
			pIsomGlob->v_total_sample_num=pIsomGlob->avi_p->video_frames;
			pIsomGlob->v_total_frame_num=pIsomGlob->v_total_sample_num;
			pIsomGlob->v_fps=pIsomGlob->avi_p->fps;
			if(pIsomGlob->v_fps)
			{
				pIsomGlob->total_time=pIsomGlob->v_total_sample_num*1000/pIsomGlob->v_fps;
			}
			pIsomGlob->v_fps<<=V_FPS_SCALE_BIT;
			pIsomGlob->v_width=pIsomGlob->avi_p->width;
			pIsomGlob->v_height=pIsomGlob->avi_p->height;
			break;

		default:
			iRet=ISOM_ERR_UNSUPPORT;
			diag_printf("[VID] unsupport media type\n");
			return iRet;
	}
    
    return ISOM_ERR_OK;
}

static int32 parse_audio_type()
{
    int32 iRet = 0;
    
	switch(pIsomGlob->a_type)
	{
		case A_TYPE_AAC:
			pIsomGlob->a_vocDecType=MMC_AAC_DECODE;
			if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
			{
				if(pIsomGlob->a_channels!=pIsomGlob->a_cfg.nb_chan)
				{
					if(pIsomGlob->a_cfg.nb_chan!=0)
						pIsomGlob->a_channels=pIsomGlob->a_cfg.nb_chan;
					diag_printf("[VID] real channels pIsomGlob->a_cfg.nb_chan=%d !!!\n",pIsomGlob->a_cfg.nb_chan);
				}

				if(pIsomGlob->a_sample_rate!=pIsomGlob->a_cfg.base_sr)
				{
					if(pIsomGlob->a_cfg.base_sr!=0)
						pIsomGlob->a_sample_rate=pIsomGlob->a_cfg.base_sr;
					diag_printf("[VID] real a_sample_rate pIsomGlob->a_cfg.sbr_sr=%d !!!\n",pIsomGlob->a_cfg.base_sr);
				}
			}

			pIsomGlob->a_frame_sample=AAC_FRAME_SAMPLE;
			pIsomGlob->a_frameBytes=(AAC_FRAME_SAMPLE*pIsomGlob->a_channels<<1);
			pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_AAC_FRAME_NUM>>1);
			pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
			pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

			if(!pIsomGlob->a_sample_rate)
			{
				diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
				iRet = ISOM_ERR_PARAM_ERROR;
				return iRet;
			}
			pIsomGlob->a_frame_Period=((AAC_FRAME_SAMPLE*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
			pIsomGlob->a_AacDecHdl= AACInitDecoder();
			if(!pIsomGlob->a_AacDecHdl)
			{
				diag_printf("[VID] aac decoder init err!\n");
				iRet = ISOM_ERR_AUDIO_DEC_FAIL;
				return iRet;
			}
			break;
            
		case A_TYPE_AMR:
			pIsomGlob->a_vocDecType=MMC_AMR_DECODE;

			if(pIsomGlob->a_channels!=1 || pIsomGlob->a_sample_rate!=8000)
			{
				if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
				{
					if(pIsomGlob->amrSp->mode_set & 0x0080)
					{
						pIsomGlob->a_channels=1;
						pIsomGlob->a_sample_rate=8000;
						diag_printf("[VID] pIsomGlob->amrSp->mode_set=%x \n", pIsomGlob->amrSp->mode_set);
					}
					else
					{
						pIsomGlob->a_channels=1;
						pIsomGlob->a_sample_rate=8000;
						diag_printf("[VID] WARNING! pIsomGlob->amrSp->mode_set=%x \n", pIsomGlob->amrSp->mode_set);
					}
				}
				else
				{
					diag_printf("[VID] warning!amr a_channels=%d a_sample_rate=%d\n", pIsomGlob->a_channels, pIsomGlob->a_sample_rate);
					pIsomGlob->a_channels=1;
					pIsomGlob->a_sample_rate=8000;
				}
			}

			pIsomGlob->a_frame_sample=AMR_FRAME_SAMPLE;
			pIsomGlob->a_frameBytes=(AMR_FRAME_SAMPLE*pIsomGlob->a_channels<<1);
			pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_AMR_FRAME_NUM>>1);
			pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
			pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

			if(!pIsomGlob->a_sample_rate)
			{
				diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
				iRet = ISOM_ERR_PARAM_ERROR;
				return iRet;
			}
			pIsomGlob->a_frame_Period=((AMR_FRAME_SAMPLE*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
			break;
            
		case A_TYPE_MP3:
			pIsomGlob->a_vocDecType=MMC_MP3_DECODE;

			if(pIsomGlob->a_sample_rate>=32000)
				pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_1152;
			else
				pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_576;
			pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels<<1);
			pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_MP3_FRAME_NUM>>1);
			pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
			pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

			if(!pIsomGlob->a_sample_rate)
			{
				diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
				iRet = ISOM_ERR_PARAM_ERROR;
				return iRet;
			}
			pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
			iRet = MMF_Mp3_OpenDecoder();
			if(iRet != 0)
			{
				diag_printf("[VID] mp3 decoder init err!\n");
				iRet = ISOM_ERR_AUDIO_DEC_FAIL;
				return iRet;
			}
			break;
            
		case A_TYPE_PCM:
		case A_TYPE_ADPCM:
			//no voc decoder, input pcm directly.
			pIsomGlob->a_frame_sample=PCM_FRAME_SAMPLE;
			pIsomGlob->a_frameBytes=(pIsomGlob->a_frame_sample*pIsomGlob->a_channels<<1);
			pIsomGlob->a_HalfPcmBufFrame=(PCM_BUF_PCM_FRAME_NUM>>1);
			pIsomGlob->a_SampBufPipeLen=MAX_A_SAMPLEBUF_PIPE_SIZE;
			pIsomGlob->a_SampBufMirrorLen=MAX_A_SAMPLEBUF_MIRROR_SIZE;

			if(!pIsomGlob->a_sample_rate)
			{
				diag_printf("[VID] err pIsomGlob->a_sample_rate=%d !!!\n",pIsomGlob->a_sample_rate);
				iRet = ISOM_ERR_PARAM_ERROR;
				return iRet;
			}
			pIsomGlob->a_frame_Period=((pIsomGlob->a_frame_sample*1000)+(pIsomGlob->a_sample_rate>>1))/pIsomGlob->a_sample_rate;
			break;

		default:
			diag_printf("[VID] invalid audio! ");
			iRet = ISOM_ERR_UNSUPPORT;
			return iRet;
	}
	
    return ISOM_ERR_OK;
}

static int32 allocate_buffer()
{
    int32 iRet = 0;
    int32 iLen = 0;
    int32 i = 0;
    
    if(pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1)>mmc_MemGetFree()-32)
	{
		iRet = ISOM_ERR_MEM_INSUFFICIENT;
		return iRet;
	}
    
	pIsomGlob->a_SampBuf=(uint8*)mmc_MemMalloc(pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1));
	if(pIsomGlob->a_SampBuf)
	{
		memset(pIsomGlob->a_SampBuf, 0, pIsomGlob->a_SampBufPipeLen+(pIsomGlob->a_SampBufMirrorLen<<1));
		pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
	}
	else
	{
		iRet = ISOM_ERR_MALLOC_FAIL;
		return iRet;
	}

	if(pIsomGlob->a_SampBufMirrorLen > mmc_MemGetFree()-32)
	{
		iRet = ISOM_ERR_MEM_INSUFFICIENT;
		return iRet;
	}
	pIsomGlob->a_SampTmpBuf=(uint8*)mmc_MemMalloc(pIsomGlob->a_SampBufMirrorLen);
	if(pIsomGlob->a_SampTmpBuf)
	{
		memset(pIsomGlob->a_SampTmpBuf, 0, pIsomGlob->a_SampBufMirrorLen);
		pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
	}
	else
	{
		iRet = ISOM_ERR_MALLOC_FAIL;
		return iRet;
	}
	
	//one more pIsomGlob->a_frameBytes for avoid overflow
	if(pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1)+pIsomGlob->a_frameBytes>mmc_MemGetFree()-32)
	{
		iRet = ISOM_ERR_MEM_INSUFFICIENT;
		return iRet;
    }
    
	//malloc enough memory avoid the fake parameters
	if(pIsomGlob->a_channels==1)
	{
		if(pIsomGlob->a_type==A_TYPE_MP3 && pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576)
		{
			iLen=(pIsomGlob->a_frameBytes<<2)*pIsomGlob->a_HalfPcmBufFrame;
			pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(iLen<<1);
			pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(iLen*EXTRA_PCM_BUF_NUM);
		}
		else
		{
			iLen=(pIsomGlob->a_frameBytes<<1)*(pIsomGlob->a_HalfPcmBufFrame);
			pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(iLen<<1);
			pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(iLen*EXTRA_PCM_BUF_NUM);
		}
	}
	else if(pIsomGlob->a_channels==2)
	{
		if(pIsomGlob->a_type==A_TYPE_MP3 && pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576)
		{
			iLen=(pIsomGlob->a_frameBytes<<1)*(pIsomGlob->a_HalfPcmBufFrame);
			pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(iLen<<1);
			pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(iLen*EXTRA_PCM_BUF_NUM);
		}
		else
		{
			iLen=pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame);
			pIsomGlob->a_PcmBuf=(uint8 *)mmc_MemMalloc(iLen<<1);
			pIsomGlob->a_ExtraPcmBuf=(uint8 *)mmc_MemMalloc(iLen*EXTRA_PCM_BUF_NUM);
		}
	}

	diag_printf("pIsomGlob->a_PcmBuf=%x %x res=%d",pIsomGlob->a_PcmBuf,pIsomGlob->a_ExtraPcmBuf, iLen);

	if(pIsomGlob->a_PcmBuf && pIsomGlob->a_ExtraPcmBuf)
	{
		memset(pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1));
		pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
	}
	else
	{
		iRet = ISOM_ERR_MALLOC_FAIL;
		return iRet;
	}

	//init extra buf
	for(i=0; i<EXTRA_PCM_BUF_NUM; i++)
	{
		aud_extra_pcmbuf_state[i].done=0;
		aud_extra_pcmbuf_state[i].pBufPos=pIsomGlob->a_ExtraPcmBuf+iLen*i;
		diag_printf("pBufPos=%x ",aud_extra_pcmbuf_state[i].pBufPos);
	}
    
	A_EXBF_RD_ID=0;
	A_EXBF_WT_ID=0;

    return 0;
}

static int32 init_voc()
{
    int32 iRet = 0;
	/*open voc*/
	VOC_OPEN = 0;
	VOC_AUD  = 0;
	VOC_WORK = VOC_WORK_NON;
	VOC_SETTING = 0;
	
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
  
	if(pIsomGlob->a_type != A_TYPE_AMR)
	{
		if((iRet=vpp_AudioJpegDecOpen(aud_voc_isr))!=HAL_ERR_NO)// will make a ISR.
		{
			diag_printf("[VID]##WARNING## vpp_AudioJpegDecOpen err res=%d \n" ,iRet);
			iRet=ISOM_ERR_VOC_OPEN_FAIL;
			return iRet;
		}
		vocDecAud.reset=1;
		vocDecAud.mode = pIsomGlob->a_vocDecType;
		vocDecAud.EQ_Type=-1;
		vocDecAud.BsbcEnable = 0;
		
		G_VidSBCConstY = (INT32*)mmc_MemMalloc(468*4);
		if( G_VidSBCConstY==NULL)
		{
				//mmc_MemFreeAll();
			//	g_VidBT8kPcmBuffer = NULL;
				diag_printf("[VID]##WARNING## G_VidSBCConstY vpp_AudioJpegDecOpen mmc_MemMalloc error!\n");
				return ISOM_ERR_MALLOC_FAIL;
		}

		if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
		{
		    vocDecAud.BsbcEnable = 1;
		}
	}
	else
	{
		VPP_SPEECH_AUDIO_CFG_T vppCfg;
		HAL_SPEECH_DEC_IN_T *pVocDecAmr=NULL;
		if ((iRet = vpp_SpeechOpen(aud_voc_isr, VPP_SPEECH_WAKEUP_SW_DEC)) == HAL_ERR_RESOURCE_BUSY)
		{
			diag_printf("[VID]##WARNING## vpp_SpeechOpen err res=%d \n" ,iRet);
			iRet=ISOM_ERR_VOC_OPEN_FAIL;
			return iRet;
		}

		// VPP audio config
		vppCfg.echoEsOn  = 0;
		vppCfg.echoEsVad = 0;
		vppCfg.echoEsDtd = 0;
		vppCfg.echoExpRel = 0;
		vppCfg.echoExpMu  = 0;
		vppCfg.echoExpMin = 0;
		vppCfg.encMute = VPP_SPEECH_MUTE;
		vppCfg.decMute = VPP_SPEECH_UNMUTE;
		vppCfg.sdf = NULL;
		vppCfg.mdf = NULL;
		vppCfg.if1 = 1;// amr ring full mode must set 1.

		vpp_SpeechAudioCfg(&vppCfg);
        vpp_SpeechSetOutAlgGain(0);

		pVocDecAmr = vpp_SpeechGetRxCodBuffer();
		if(pVocDecAmr == NULL)
		{
			diag_printf("[VID]##ERROR## vpp_SpeechGetRxCodBuffer NULL!\n");
			iRet = ISOM_ERR_VOC_OPEN_FAIL;
			return iRet;
		}

		pVocDecAmr->bfi = 0;
		pVocDecAmr->sid = 0;
		pVocDecAmr->taf = 0;
		pVocDecAmr->ufi = 0;
		pVocDecAmr->reserved = 0;
        pVocDecAmr->dtxOn    = 0;
		pVocDecAmr->codecMode= HAL_INVALID;
		pVocDecAmr->decFrameType = 0;
		memset(pVocDecAmr->decInBuf, 0, HAL_SPEECH_FRAME_SIZE_COD_BUF);
	}

    return ISOM_ERR_OK;
}


static void dbg_print()
{
	diag_printf("[VID] a_type=%x \n",pIsomGlob->a_type);
	diag_printf("[VID] a_trackID=%d \n",pIsomGlob->a_trackID);
	diag_printf("[VID] a_total_sample_num=%d \n",pIsomGlob->a_total_sample_num);
	diag_printf("[VID] a_total_frame_num=%d \n",pIsomGlob->a_total_frame_num);
	diag_printf("[VID] a_bitrate=%d \n",pIsomGlob->a_bitrate);
	diag_printf("[VID] a_sample_rate=%d \n",pIsomGlob->a_sample_rate);
	diag_printf("[VID] a_frame_Period=%d \n",pIsomGlob->a_frame_Period);
	diag_printf("[VID] a_bit=%d \n",pIsomGlob->a_bit);
	diag_printf("[VID] a_channels=%d \n",pIsomGlob->a_channels);
	diag_printf("[VID] a_frame_sample=%d \n",pIsomGlob->a_frame_sample);
	diag_printf("[VID] total_time = %d\n", pIsomGlob->total_time);
}

static void open_error()
{
    if(VOC_OPEN)
	{
		if(pIsomGlob->a_type != A_TYPE_AMR)
		{
			vpp_AudioJpegDecClose();
		}
		else
		{
			vpp_SpeechClose();
		}
		VOC_OPEN=0;

	}
    
	if(pIsomGlob)
	{
		//destroy media box
		if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
		{
			gf_isom_close_file(pIsomGlob->file);
		}

		else if(pIsomGlob->med_type==MCI_TYPE_MJPG)
		{
			if(pIsomGlob->avi_p)
			{
				avi_close_input_file(pIsomGlob->avi_p);
            }
		}

		//destroy audio decoder
		if(pIsomGlob->a_AacDecHdl)
		{
			AACFreeDecoder(pIsomGlob->a_AacDecHdl);
			pIsomGlob->a_AacDecHdl=0;
		}
		else if(pIsomGlob->a_type == A_TYPE_MP3)
		{
			MMF_Mp3_CloseDecoder();
		}
		else if(pIsomGlob->a_AmrStruct)
		{
			AmrDecClose(pIsomGlob->a_AmrStruct);
			pIsomGlob->a_AmrStruct=0;
		}
	}
	//mmc_MemFreeAll();
}


int32 mmc_audio_isom_open_file( HANDLE file_handle,  uint8 file_mode, uint32 data_Len, int32 audio_path, 
	uint8 open_audio, mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32))
{

	int32 iRet = 0;
    int32 i = 0;
     
     bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
     bt_a2dp_sbc_codec_cap_struct *sbc_config_handle =  &my_audio_config_handle->codec_cap.sbc;

    diag_printf("[VID] mmc_audio_isom_open_file \n");
    pIsomGlob=0;
    /*for voc*/
    VOC_OPEN=0;
    VOC_AUD=0;
    VOC_WORK=VOC_WORK_NON;
    VOC_SETTING=0;

	if(play_state != VID_PLAY_STATE_INVALID)
	{
		diag_printf("[VID] mmc_vid_isom_open_file err play_state= %d \n",play_state);
		iRet = ISOM_ERR_PLAY_STATE_FAIL;
		goto open_error;
	}

    if(!vid_play_finish_callback)
	{
        iRet=ISOM_ERR_PARAM_ERROR;
		goto open_error;
	}

	pIsomGlob = (IsomPlayGlobal*)mmc_MemMalloc(sizeof(IsomPlayGlobal));
    if(!pIsomGlob)
    {
        iRet = ISOM_ERR_MALLOC_FAIL;
		goto open_error;
    }
    memset(pIsomGlob, 0, sizeof(IsomPlayGlobal));
	pIsomGlob->med_type = type;
	pIsomGlob->filehdl = file_handle;
	pIsomGlob->a_audio_path = audio_path;
	pIsomGlob->open_audio   = open_audio;

    
	DTS_FLG = 0xFFFFFFFF;
    pIsomGlob->vid_play_finish_callback = vid_play_finish_callback;
	if(vid_draw_panel_callback)
	{
		pIsomGlob->vid_draw_panel_callback = vid_draw_panel_callback;
	}

	diag_printf("[VID] david file_mode=%d",file_mode);
    
    iRet = parse_media_type(file_handle, file_mode, data_Len );
    if(iRet)
    {
       goto open_error;   
    }
    
    iRet = parse_audio_type();
    if(iRet)
    {
       goto open_error;   
    }
    
	if(pIsomGlob->a_sample_rate)
	{
		pIsomGlob->total_time=(UINT32)((long long)pIsomGlob->a_total_sample_num * 1000 * (long long)pIsomGlob->a_frame_sample/pIsomGlob->a_sample_rate);
			//diag_printf("[VID]file_mode=%d",file_mode);
	}
	//pIsomGlob->a_sync_distance_frame_num=SYNC_TIME_OFFSET*pIsomGlob->a_sample_rate/1000;
    dbg_print();

    iRet = allocate_buffer();
    if(iRet)
    {
       goto open_error;   
    }

    iRet = init_voc();
    if(iRet)
    {
       goto open_error;   
    }
    
    VOC_OPEN=1;
    
	//fillAudioData = NULL;//not read audio in video decoder.
	mmc_MemCheck(TRUE);
	//pIsomGlob->bSkipFrame = FALSE;
	//pIsomGlob->v_current_frame_num= -1;//0=the first frame
	AV_DEC_STATE = 0;
	//DISPLAY_STATE=0;
	//AUD_DEC_MIN = 2;//default
	//VID_ERR_FRAME_REC=0;
	//VID_SET_MODE=0;
	AUD_DATA_REQ = 0;

	//reduce charge current for video player.
	//pmd_ReduceChargeCurrent(1);	

	play_state = VID_PLAY_STATE_IDLE;

	diag_printf("[VID] mmc_audio_isom_open_file ok!\n", iRet);
	return ISOM_ERR_OK;

open_error:
	diag_printf("[VID] ##WARNING## mmc_vid_isom_open_file err res=%d\n", iRet);
    open_error();
	return iRet;
}

int32 audio_frame_sample()
{
    int32 sample = 0;
    
    if(pIsomGlob->a_type== A_TYPE_AAC)
       sample= AAC_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_AMR)
	sample= AMR_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_MP3)
    {
        if(pIsomGlob->a_sample_rate>=32000)
            sample= MP3_FRAME_SAMPLE_1152;
        else
            sample= MP3_FRAME_SAMPLE_576;
    }
    else if(pIsomGlob->a_type== A_TYPE_PCM)
        sample= PCM_FRAME_SAMPLE;
    else if(pIsomGlob->a_type== A_TYPE_ADPCM)
	 sample=1;
    else if(pIsomGlob->a_type== A_TYPE_COOK)
        sample= pIsomGlob->a_frame_sample;
    else if(pIsomGlob->a_type== A_TYPE_RA)
	 sample=1;
    else if(pIsomGlob->a_type== A_TYPE_DRA)
	sample= DRA_FRAME_SAMPLE_1024;
    else
    {
        diag_printf("[VID] err pIsomGlob->a_type=%d !!!\n",pIsomGlob->a_type);
        return ISOM_ERR_UNSUPPORT;
    }

    return sample;
}

int32 init_audio_stream()
{
    int32 iRet = 0;
    
    audioStream.startAddress = (UINT32*)pIsomGlob->a_PcmBuf;
    audioStream.length       = pIsomGlob->a_frameBytes*(pIsomGlob->a_HalfPcmBufFrame<<1);
    audioStream.channelNb    = pIsomGlob->a_channels;
    audioStream.voiceQuality = FALSE;
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler = aud_pcmHalf_isr;
    audioStream.endHandler  = aud_pcmEnd_isr;

    switch (pIsomGlob->a_sample_rate)
    {
        case 48000:
            audioStream.sampleRate = HAL_AIF_FREQ_48000HZ;
            break;
        case 44100:
            audioStream.sampleRate = HAL_AIF_FREQ_44100HZ;
            break;
        case 32000:
            audioStream.sampleRate = HAL_AIF_FREQ_32000HZ;
            break;
        case 24000:
            audioStream.sampleRate = HAL_AIF_FREQ_24000HZ;
            break;
        case 22050:
            audioStream.sampleRate = HAL_AIF_FREQ_22050HZ;
            break;
        case 16000:
            audioStream.sampleRate = HAL_AIF_FREQ_16000HZ;
            break;
        case 12000:
            audioStream.sampleRate = HAL_AIF_FREQ_12000HZ;
            break;
        case 11025:
            audioStream.sampleRate = HAL_AIF_FREQ_11025HZ;
            break;
        case 8000:
            audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            break;
        default:
            diag_printf( "##WARNING## invalid a_sample_rate=%d\n",pIsomGlob->a_sample_rate);
            return ISOM_ERR_INVALID_FORMAT;
    }

	diag_printf( "[VID]open audio, audioItf=%d",audioItf);
	
	#if 0
	if((MpegPlayer.MpegInput.bt_sco==TRUE)&&(pIsomGlob->a_type != A_TYPE_AMR))
	{
		g_VidBtNumOfOutSample=ResampleInit(pIsomGlob->a_sample_rate, HAL_AIF_FREQ_8000HZ, pIsomGlob->a_frameBytes*pIsomGlob->a_bit/8, pIsomGlob->a_channels);
		g_VidBtTotalFrame = pIsomGlob->a_HalfPcmBufFrame;
		if(g_VidBT8kPcmBuffer == NULL)
	   	g_VidBT8kPcmBuffer=(INT16 *)mmc_MemMalloc(g_VidBtTotalFrame*g_VidBtNumOfOutSample*pIsomGlob->a_bit);

		if(!g_VidBT8kPcmBuffer)
		{
		    diag_printf( "[AUD] g_VidBT8kPcmBuffer allocate error!");
		    return ISOM_ERR_MALLOC_FAIL;
		}
		memset(g_VidBT8kPcmBuffer, 0, VID_BT_SCO_8K_OUTBUFF_SIZE);

		g_VidBtOutputPcmBufSize =pIsomGlob->a_frameBytes;
		//g_VidBtNumOfOutSample=ResampleInit(pIsomGlob->a_frame_sample, HAL_AIF_FREQ_8000HZ, VID_BT_SCO_8K_OUTBUFF_SIZE>>1, audioStream.channelNb);
		//pIsomGlob->a_sample_rate = HAL_AIF_FREQ_8000HZ;
		audioStream.startAddress  = (UINT32*) g_VidBT8kPcmBuffer;//pAudioPlayer->MpegOutput.pcm.Buffer;
		audioStream.length        = g_VidBtTotalFrame*g_VidBtNumOfOutSample*pIsomGlob->a_bit/8;
		audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
		audioStream.voiceQuality   = TRUE;
		audioItf=AUD_ITF_BLUETOOTH;
	}
	else if((MpegPlayer.MpegInput.bt_sco==TRUE)&&(pIsomGlob->a_type == A_TYPE_AMR))
	{
		audioItf=AUD_ITF_BLUETOOTH;
		audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
		audioStream.channelNb     = HAL_AIF_MONO;//HAL_AIF_STEREO;//HAL_AIF_MONO;
		audioStream.voiceQuality   = TRUE;
	}
	#endif
	
    if((iRet = aud_StreamStart(audioItf, &audioStream, &audio_cfg))!=AUD_ERR_NO)
    {
        diag_printf( "[VID]##WARNING## first aud_StreamStart fail res= %d \n",iRet);
	      aud_StreamStop(audioItf);
        if((iRet=aud_StreamStart(audioItf, &audioStream, &audio_cfg))!=AUD_ERR_NO)
        {
            diag_printf( "[VID]##ERROR## aud_StreamStart res= %d \n",iRet);
            return ISOM_ERR_PCM_OPEN_FAIL;
        }
    }

    return ISOM_ERR_OK;
}


int32 mmc_audio_isom_play (uint16 lcd_start_x,  uint16 lcd_start_y)
{
    int i,j;
    //int32 res;
    unsigned long long temp;
    uint32 aud_frame_samp;
    int32 iRet = 0;
    SSHdl *my_ss_handle = MpegPlayer.MpegInput.ss_handle;
    bt_a2dp_audio_cap_struct * my_audio_config_handle=   MpegPlayer.MpegInput.audio_config_handle ;
    bt_a2dp_sbc_codec_cap_struct *sbc_config_handle =  &my_audio_config_handle->codec_cap.sbc;
    
    totalTa=0;
    totalTv=0;
    totalTx=0;
    counta=0;
    countv=0;
    countx=0;

    diag_printf( "[VID] mmc_audio_isom_play");
    if(play_state!=VID_PLAY_STATE_IDLE)
    {
        diag_printf("[VID] mmc_vid_isom_play err play_state= %d \n",play_state);
        iRet = ISOM_ERR_PLAY_STATE_FAIL;
        goto play_error;
    }

    aud_frame_samp = audio_frame_sample();
    if(aud_frame_samp == ISOM_ERR_UNSUPPORT)
    {
        goto play_error;       
    }
    
    if(!pIsomGlob->total_time)
    {
        diag_printf("[VID] err v_total_frame_num=%d total_time=%d !!!\n",pIsomGlob->v_total_frame_num,pIsomGlob->total_time);
        iRet = ISOM_ERR_PARAM_ERROR;
        goto play_error;
    }
    
    MpegPlayer.MpegInput.bt_sco = FALSE;//sound quality is terrible, always close sco.
    #if 0
    if (is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))
      {
        int iRet = 0;
        iRet = sbc_init(&pIsomGlob->sbc, 0L);
        if(iRet != 0)
        {
            diag_printf("[VID] sbc_init error !####################### iRet = %d\n", iRet);
            goto play_error;
        }

        diag_printf("[VID] ####################### sbc_init ok!");
       pIsomGlob->sbc.mono2dual = DUAL2MONO;//2:dual->mono.
	     g_SbcEncEnable =0;//not enc sbc in first half pcm buf,avoid error ch or samplerate.
       if (sbc_config_handle->alloc_method  == 0x01)
         pIsomGlob->sbc.allocation= SBC_AM_LOUDNESS;//
       else if (sbc_config_handle->alloc_method  == 0x02 )
         pIsomGlob->sbc.allocation= SBC_AM_SNR;//SBC_AM_LOUDNESS;//
       else
         diag_printf("[VID] WARNING! ##WARNING##ErrorSBC alloc_method!\n");
       if( sbc_config_handle->channel_mode == 0x01 )
         pIsomGlob->sbc.mode = SBC_MODE_JOINT_STEREO; // joint stereo
       else if( sbc_config_handle->channel_mode == 0x02 )
         pIsomGlob->sbc.mode = SBC_MODE_STEREO; // stereo
       else if( sbc_config_handle->channel_mode == 0x04 )
         pIsomGlob->sbc.mode = SBC_MODE_DUAL_CHANNEL; // dual
       else if( sbc_config_handle->channel_mode == 0x08 )
         pIsomGlob->sbc.mode= SBC_MODE_MONO; // mono
       else
         diag_printf("[VID] WARNING! ##WARNING##ErrorSBC channel mode!\n");
      }
    #endif
   
    temp=((unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->a_sample_rate)/ ((unsigned long long)aud_frame_samp*1000);
	if(pIsomGlob->a_type== A_TYPE_ADPCM || pIsomGlob->a_type== A_TYPE_RA)
	{
		temp = 0;
	}
    
    pIsomGlob->a_current_frame_num = (uint32)temp;

	if(pIsomGlob->med_type == MCI_TYPE_MP4 || pIsomGlob->med_type == MCI_TYPE_3GP)
	{
		pIsomGlob->a_SampStart = pIsomGlob->a_SampEnd = pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
		if(pIsomGlob->a_type == A_TYPE_AMR)
		{
			pIsomGlob->a_SampTmpBufp = pIsomGlob->a_SampTmpBuf;
			pIsomGlob->a_SampTmpRemain = 0;
		}
		else
		{
			pIsomGlob->a_SampTmpBufp = pIsomGlob->a_SampTmpBuf;
			pIsomGlob->a_SampTmpRemain = 0;
		}
	}
	else
	{
		pIsomGlob->a_SampStart=pIsomGlob->a_SampEnd=pIsomGlob->a_SampBuf+pIsomGlob->a_SampBufMirrorLen;
		if(pIsomGlob->a_type== A_TYPE_AMR && pIsomGlob->a_current_sample_num!=0)
		{
    		pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
    		pIsomGlob->a_SampTmpRemain=0;
		}
		else
		{
			pIsomGlob->a_SampTmpBufp=pIsomGlob->a_SampTmpBuf;
			pIsomGlob->a_SampTmpRemain=0;
		}

		iRet=avi_set_audio_position2(pIsomGlob->avi_p, pIsomGlob->a_current_sample_num);
		if(iRet != 0)
		{
			diag_printf("[VID] avi_set_audio_position2 err! res=%d ", iRet);
			iRet=ISOM_ERR_AUDIO_READ_FAIL;
			goto play_error;
		}

		if(pIsomGlob->a_type== A_TYPE_AMR && pIsomGlob->a_current_sample_num ==0)
		{
			fillSampBuf();
			//move amr header
			moveDataPos(AMR_HEAD_SIZE);
		}
	}

	vocDecAud.reset = 1;

	A_EXBF_RD_ID = 0;
	A_EXBF_WT_ID = 0;

    for(i = 0; i < EXTRA_PCM_BUF_NUM; i++)
	{
		aud_extra_pcmbuf_state[i].done = 1;
	}
    
    VOC_AUD = 0;
	AUD_FADE_IN_FLAG = (pIsomGlob->a_HalfPcmBufFrame + 2);//zero pcm num
	
    diag_printf( "[VID] play111");

	for(i = 0; i < (2 + EXTRA_PCM_BUF_NUM);i++)
    {
        fillSampBuf();
        
        /*check AMR mode*/
        if(pIsomGlob->a_type == A_TYPE_AMR && i==0)
        {
            uint8 *pH;
            pH = getDataPos();
        }

        if(getDataLen() <= 0)
        {
            continue;
        }

		if(i==0)
		{
			pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
		}
		else if(i==1)
		{
			pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
		}
		else
		{
			pPcmBufPos=(uint32)(aud_extra_pcmbuf_state[i-2].pBufPos);
		}

        if(pIsomGlob->a_type!= A_TYPE_PCM && pIsomGlob->a_type!= A_TYPE_ADPCM)
        {
            tick1 = hal_TimGetUpTime();
            AUD_DEC_FRM_NUM=0;

            diag_printf( "[VID] play222");
            if(VOC_WORK == VOC_WORK_NON )
            {
                VOC_WORK = VOC_WORK_AUD;
		        aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=0;
				if(pIsomGlob->a_type!= A_TYPE_AMR)
				{
                    vocDecAud.inStreamBufAddr = (INT32 *)getDataPos();
                    vocDecAud.outStreamBufAddr = (INT32 *)pPcmBufPos;
                    vocDecAud.audioItf = audioItf;
            
                    if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))      //BT_A2DP_sbc
				 	{

        			      int mode =SBC_MODE_JOINT_STEREO;
        		          vocDecAud.BsbcEnable = 1;
        
        			      if( sbc_config_handle->channel_mode == 0x01 )
        			         mode = SBC_MODE_JOINT_STEREO; // joint stereo
        			      else if( sbc_config_handle->channel_mode == 0x02 )
        			        mode = SBC_MODE_STEREO; // stereo
        			      else if( sbc_config_handle->channel_mode == 0x04 )
        			        mode = SBC_MODE_DUAL_CHANNEL; // dual
        			      else if( sbc_config_handle->channel_mode == 0x08 )
        			        mode= SBC_MODE_MONO; // mono
        			      else
        			          diag_printf("[VID] WARNING! ##WARNING##ErrorSBC channel mode!\n");
        
        				   vocDecAud.Source_width = mode;
        
        			      if( sbc_config_handle->alloc_method  == 0x01 )
        			           vocDecAud.Source_height= SBC_AM_LOUDNESS;//
        			      else if( sbc_config_handle->alloc_method  == 0x02 )
        			           vocDecAud.Source_height= SBC_AM_SNR;//SBC_AM_LOUDNESS;//
        			      else
        			          diag_printf("[VID] WARNING! ##WARNING##ErrorSBC alloc_method!\n");
        				   vocDecAud.Cut_width =    SBC_BIT_POOL;
        				   vocDecAud.inStreamUBufAddr = (INT32*)G_VppSBCConstX;
        				   vocDecAud.inStreamVBufAddr = G_VidSBCConstY;
				 	}

		       		vpp_AudioJpegDecScheduleOneFrame(&vocDecAud);
		           	vocDecAud.reset=0;
				}
		        else
				{
					HAL_SPEECH_DEC_IN_T *pVocDecAmr = vpp_SpeechGetRxCodBuffer();
					get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));
					if(i==0)
					{
						int32 dropNum=pIsomGlob->a_HalfPcmBufFrame;
						//make start with silence
						while(dropNum)
						{
							moveDataPos(pIsomGlob->a_codeframeBytes);//drop one frame
							get_amr_mode( getDataPos(), &(pIsomGlob->a_amr_mode), &(pIsomGlob->a_codeframeBytes));
							dropNum--;
						}
						memset((void*)pPcmBufPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
						aud_extra_pcmbuf_state[A_EXBF_WT_ID].done=1;
						A_EXBF_WT_ID+=1;
						if(A_EXBF_WT_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_WT_ID=0;
						VOC_WORK = VOC_WORK_NON;
					}
					else
					{
						if(pIsomGlob->a_amr_mode != HAL_INVALID)
						{
							getData((U8*)pVocDecAmr->decInBuf, pIsomGlob->a_codeframeBytes, 1);
							pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
						}
						else
						{
							//write silence data
							moveDataPos(pIsomGlob->a_codeframeBytes);
							memcpy(pVocDecAmr->decInBuf, silence_frame475, 13);
							pVocDecAmr->codecMode= HAL_AMR475_DEC;
						}

						pVocDecAmr->dtxOn = 0;
						//pVocDecAmr->codecMode= pIsomGlob->a_amr_mode;
						pVocDecAmr->decFrameType = 0;
						pVocDecAmr->bfi = 0;
						pVocDecAmr->sid = 0;
						pVocDecAmr->taf = 0;
						pVocDecAmr->ufi = 0;
						pVocDecAmr->reserved= 0;
						RX_BUF_SWAP_ID=0;

						// Set all the preprocessing modules
						vpp_SpeechSetEncDecPocessingParams(/*audioItf*/AUD_ITF_LOUD_SPEAKER, audio_cfg.spkLevel, 0);
						vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC);
					}
				}
            }

            j = 0;
            while(VOC_WORK )
            {
                j++;
                if(j>=VOC_WORK_CONT)
                {
                    diag_printf("[VID]AUD VOC_WORK_CONT end! \n");
                    iRet=ISOM_ERR_AUDIO_DEC_FAIL;
                    goto play_error;
                }
            }

            diag_printf("A_EXBF_WT_ID %d A_EXBF_RD_ID %d",A_EXBF_WT_ID,A_EXBF_RD_ID);

			if(i<2)
			{
				A_EXBF_RD_ID+=1;
				if(A_EXBF_RD_ID>=EXTRA_PCM_BUF_NUM)A_EXBF_RD_ID=0;
			}

            tick2=hal_TimGetUpTime();
            diag_printf( "[VID]voc aud dec time=%d frame num=%d", (tick2-tick1)*1000/16384, pIsomGlob->a_HalfPcmBufFrame);
	        if(pIsomGlob->a_type!= A_TYPE_AMR)
			{
				diag_printf( "[VID] AUD real channel=%d SampleRate=%d BitRate=%d output_len=%d streamStatus=%d",
					vocDecStatus.nbChannel,
					vocDecStatus.SampleRate,
					vocDecStatus.BitRate,
					vocDecStatus.output_len,
					vocDecStatus.streamStatus);

				if(vocDecStatus.nbChannel !=pIsomGlob->a_channels)
				{
					diag_printf( "[VID] warning! AUD channel not match");
					if(vocDecStatus.nbChannel==1)
					{
						pIsomGlob->a_channels=1;
						pIsomGlob->a_frameBytes>>=1;
						pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
					}
					else if(vocDecStatus.nbChannel==2)
					{
						pIsomGlob->a_channels=2;
						pIsomGlob->a_frameBytes<<=1;
						pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
					}
					else
					{
						iRet=ISOM_ERR_AUDIO_DEC_FAIL;
						goto play_error;
					}
				}
				else if(vocDecStatus.SampleRate !=pIsomGlob->a_sample_rate)
				{
					diag_printf( "[VID] warning! AUD SampleRate not match");
					if(vocDecStatus.SampleRate>=8000 && vocDecStatus.SampleRate<=48000)
						pIsomGlob->a_sample_rate=vocDecStatus.SampleRate;
					else
					{
						iRet=ISOM_ERR_AUDIO_DEC_FAIL;
						goto play_error;
					}

					if(pIsomGlob->a_type==A_TYPE_MP3 )
					{
						if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_576 && vocDecStatus.SampleRate>=32000)
						{
							pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_1152;
							pIsomGlob->a_frameBytes<<=1;
							pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
						}
						else if( pIsomGlob->a_frame_sample==MP3_FRAME_SAMPLE_1152 && vocDecStatus.SampleRate<=24000)
						{
							pIsomGlob->a_frame_sample=MP3_FRAME_SAMPLE_576;
							pIsomGlob->a_frameBytes>>=1;
							pIsomGlob->a_PcmBufHalfPos=pIsomGlob->a_PcmBuf+pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame;
						}
					}
				}
				else if(vocDecStatus.output_len !=pIsomGlob->a_frameBytes)
				{
					diag_printf( "[VID] warning! AUD output_len not match");
					iRet=ISOM_ERR_AUDIO_DEC_FAIL;
					goto play_error;
				}
			}
        }
        else if(pIsomGlob->a_type == A_TYPE_PCM)
        {
            if(i==0)
                pPcmBufPos=(uint32)pIsomGlob->a_PcmBuf;
            else if(i==1)
            {
                pPcmBufPos=(uint32)pIsomGlob->a_PcmBufHalfPos;
                i=2;
            }
            AUD_DEC_FRM_NUM=pIsomGlob->a_HalfPcmBufFrame;
            while(AUD_DEC_FRM_NUM)
            {
                iRet=getData(((unsigned char*)pPcmBufPos)+(pIsomGlob->a_HalfPcmBufFrame-(unsigned int)AUD_DEC_FRM_NUM)*pIsomGlob->a_frameBytes,
                                pIsomGlob->a_frameBytes, (unsigned int)AUD_DEC_FRM_NUM);
                if(iRet>0)
                {
                    if((pIsomGlob->a_channels==2)&&(audioItf==AUD_ITF_LOUD_SPEAKER))
                    {
                        INT16 *pPcm=(INT16 *)(((unsigned char*)pPcmBufPos)+(pIsomGlob->a_HalfPcmBufFrame-(unsigned int)AUD_DEC_FRM_NUM)*pIsomGlob->a_frameBytes);
                        uint32 sNum=pIsomGlob->a_frame_sample*2*iRet;

                        for(j=0;j<sNum;j+=2)
                        {
                            pPcm[j]=pPcm[j+1]=(pPcm[j]>>1)+(pPcm[j+1]>>1);
                        }
                    }

                    AUD_DEC_FRM_NUM-=(uint16)iRet;
                    fillSampBuf();
                }
                else
                {
                    diag_printf( "[VID] warning! AUD data end!");
                    AUD_DEC_FRM_NUM=0;
                    break;
                }
            }
        }
    }
	
  //  LAST_SYNC_AUD_FRM_NUM=pIsomGlob->a_current_frame_num;
    AUD_DEC_FRM_NUM=0;
    PCM_ISR_FLAG=0;
    VOC_WORK=VOC_WORK_NON;
    VOC_AUD=0;

	diag_printf("A_EXBF_WT_ID %d A_EXBF_RD_ID %d",A_EXBF_WT_ID,A_EXBF_RD_ID);
	//diag_printf(" %d %d %d",aud_extra_pcmbuf_state[0].done,aud_extra_pcmbuf_state[1].done,aud_extra_pcmbuf_state[2].done);

	//fillAudioData=NULL;//not read audio in video decoder.

    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER)
    {
        // Initial cfg
        diag_printf("audioItf=%d invalid. set to AUD_ITF_LOUD_SPEAKER");
        audioItf = AUD_ITF_LOUD_SPEAKER;
    }
    iRet = init_audio_stream();
    if(iRet != 0)
    {
        goto play_error;
    }

    AUD_DATA_REQ = 1;
    play_state   = VID_PLAY_STATE_PLAYING;
    diag_printf("[VID] mmc_audio_isom_play OK!");
    return ISOM_ERR_OK;

play_error:
    diag_printf("[VID] ##WARNING## mmc_vid_isom_play err res= %d \n",iRet);
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
    return iRet;
}

int32 mmc_audio_isom_seek ( uint32	time, int16 time_mode, int16 lcd_start_x, int16	lcd_start_y)
{
    int32 res;
    int32 used_bytes;
   // unsigned long long temp;

    if((play_state!=VID_PLAY_STATE_IDLE) && (play_state!=VID_PLAY_STATE_PAUSE) )
    {
        diag_printf("[VID] mmc_vid_isom_seek err play_state= %d \n",play_state);
        return  ISOM_ERR_PLAY_STATE_FAIL;
    }

    if(time_mode==SEEK_TIME_MODE_OFFSET)
    {
        pIsomGlob->current_time += time;
    }
    else if(time_mode==SEEK_TIME_MODE_ABSOLUTE)//default
    {
        pIsomGlob->current_time = time;
    }
    diag_printf("[VID] v_lcd_start_x=%d v_lcd_end_x=%d",pIsomGlob->v_lcd_start_x, pIsomGlob->v_lcd_end_x);
    diag_printf("[VID] v_lcd_start_y=%d v_lcd_end_y=%d",pIsomGlob->v_lcd_start_y, pIsomGlob->v_lcd_end_y);
    diag_printf("[VID] seek time=%d \n",pIsomGlob->current_time);

    if(pIsomGlob->current_time < 0)
    {
        pIsomGlob->current_time = 0;
    }
    else if(pIsomGlob->current_time > pIsomGlob->total_time)
    {
        pIsomGlob->current_time = pIsomGlob->total_time;
    }

    ///temp=(unsigned long long)pIsomGlob->current_time*(unsigned long long)pIsomGlob->v_fps/(1000<<V_FPS_SCALE_BIT);
	play_state=VID_PLAY_STATE_IDLE;
	diag_printf("[VID] mmc_audio_isom_seek audio OK!");
    
	return ISOM_ERR_OK;
}

int32 mmc_audio_isom_pause (void)
{
	int res;
    int32 iRet = 0;

	if(play_state!=VID_PLAY_STATE_PLAYING)
	{
		mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
		diag_printf("[VID]##WARNING##mmc_vid_isom_pause play_state= %d \n",play_state);
		return  ISOM_ERR_PLAY_STATE_FAIL;
	}
    
	AUD_FADE_OUT_FLAG = 3;
	int32 i = 0;
	//fade out
	while(AUD_FADE_OUT_FLAG != 1)
	{
		sxr_Sleep(320);//about 20ms
		if(i>20)
			break;
		i++;
	}

	if((res = aud_StreamPause(audioItf, TRUE)) != AUD_ERR_NO)
	{
		diag_printf("[VID] ##WARNING##aud_StreamPause res=%d",res);
	}
	AUD_DATA_REQ = 0;
	AUD_FADE_OUT_FLAG = 0;
	
	diag_printf("[VID]mmc_audio_isom_pause audio_Pause_Pos= %x \n",pIsomGlob->a_Pause_Pos);
	play_state = VID_PLAY_STATE_PAUSE;

	//wait voc work over
	i = 0;
	while(AV_DEC_STATE  || VOC_WORK )
	{
		i++;
		if(i>=VOC_WORK_CONT)
		{
			diag_printf("[VID]PAUSE WARNING! AUD VOC_WORK_CONT end! \n");
			break;
		}
		sxr_Sleep(1600);
	}

	return ISOM_ERR_OK;
}

int32 mmc_audio_isom_resume (void)
{
	int32 iRet = 0;
    
    diag_printf("[VID]mmc_audio_isom_resume \n");
	if(play_state!=VID_PLAY_STATE_PAUSE)
	{
		diag_printf("[VID]##WARNING##mmc_vid_isom_resume play_state= %d \n",play_state);
		iRet=ISOM_ERR_PLAY_STATE_FAIL;
		goto resume_error;
	}

	if(pIsomGlob->a_Pause_DecReq==1)
	{
		pIsomGlob->a_Pause_DecReq=0;
	}
    
	int32 i,j;
	uint8 rd_id = A_EXBF_RD_ID;

	AUD_FADE_IN_FLAG = (pIsomGlob->a_HalfPcmBufFrame+2);//zero pcm num
	//mute current pcm play buffer
	if(PCM_ISR_FLAG == 0)
	{
		memset((uint8*)pIsomGlob->a_PcmBufHalfPos, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
	}
	else
	{
		memset((uint8*)pIsomGlob->a_PcmBuf, 0, pIsomGlob->a_frameBytes*pIsomGlob->a_HalfPcmBufFrame);
	}
    
	//mute or fade in extern buffer
	for(i = 0; i < EXTRA_PCM_BUF_NUM; i++)
	{
		if(aud_extra_pcmbuf_state[rd_id].done==1)
		{
			j=0;
			while(j<pIsomGlob->a_HalfPcmBufFrame)
			{
				if(AUD_FADE_IN_FLAG>1)
				{
					memset((uint8*)(aud_extra_pcmbuf_state[rd_id].pBufPos+ pIsomGlob->a_frameBytes*j), 0, pIsomGlob->a_frameBytes);
					AUD_FADE_IN_FLAG--;
				}
				else if(AUD_FADE_IN_FLAG==1)
				{
					audFadeIn((int16*)(aud_extra_pcmbuf_state[rd_id].pBufPos+ pIsomGlob->a_frameBytes*j),
						pIsomGlob->a_frame_sample*pIsomGlob->a_channels);
					    AUD_FADE_IN_FLAG=0;
				}
				j++;
			}
		}
		else
		{
			break;
		}
		rd_id+=1;
		if(rd_id>=EXTRA_PCM_BUF_NUM)rd_id=0;
	}

	if((iRet=aud_StreamPause(audioItf, FALSE))!=AUD_ERR_NO)
	{
		diag_printf( "##WARNING## aud_StreamPause err result= %d \n",iRet);
		iRet=ISOM_ERR_PCM_OPEN_FAIL;
		goto resume_error;
	}
    
	AUD_DATA_REQ=1;
	SEND_EVENT_NUM=0;
	//DISPLAY_SEND_EVENT_NUM=0;

	play_state=VID_PLAY_STATE_PLAYING;
	diag_printf("[VID] mmc_audio_isom_resume");

	return ISOM_ERR_OK;
resume_error:

	mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
	diag_printf("[VID] mmc_vid_isom_resume err res=%d\n", iRet);
	return iRet;

}


int32 mmc_audio_isom_stop(void)
{
	int iRet = 0;
	//int i;

	if(play_state >= VID_PLAY_STATE_INVALID)
	{
		diag_printf("[VID]mmc_vid_isom_stop play_state=%d ERR!", play_state);
		return ISOM_ERR_PLAY_STATE_FAIL;
	}
	else if( play_state==VID_PLAY_STATE_IDLE)
	{
		diag_printf("[VID] mmc_vid_isom_stop play_state= %d \n",play_state);
		return ISOM_ERR_OK;
	}

	if(pIsomGlob)
	{
		AUD_FADE_OUT_FLAG=3;
		int32 i=0;
		//fade out
		while(AUD_FADE_OUT_FLAG !=1)
		{
			sxr_Sleep(320);//about 20ms
			if(i>20)
				break;
			i++;
		}

		if((iRet = aud_StreamStop(audioItf))!=AUD_ERR_NO)
		{
			diag_printf("[VID] ##WARNING##aud_StreamStop res=%d", iRet);
		}
		AUD_DATA_REQ=0;
		AUD_FADE_OUT_FLAG=0;
		PCM_ISR_FLAG=0;
		pIsomGlob->a_Pause_DecReq=0;
	}

	diag_printf("[VID] mmc_audio_isom_stop");

	if(countv)
		diag_printf("[VID] avg  a_read,dec =%d ms ", (totalTa*1000+countv*(16384/2))/(countv*16384));
	if(countv)
		diag_printf("[VID] avg v_dec=%d ms ", (totalTv*1000+countv*(16384/2))/(countv*16384));

	play_state=VID_PLAY_STATE_IDLE;

	//wait voc work over
	int32 i=0;
	while(AV_DEC_STATE || VOC_WORK )
	{
		i++;
		if(i>=VOC_WORK_CONT)
		{
			diag_printf("[VID]STOP WARNING! AUD VOC_WORK_CONT end! \n");
			break;
		}
		sxr_Sleep(1600);
	}
    
	return ISOM_ERR_OK;
}

int32 mmc_audio_isom_close_file (void)
{
	int32 res;
	if(play_state!=VID_PLAY_STATE_IDLE)
	{
		diag_printf("[VID] mmc_vid_isom_close_file err play_state= %d \n",play_state);
		mmc_vid_isom_stop();
	}

	if(VOC_OPEN)
	{
		if(pIsomGlob->a_type != A_TYPE_AMR)
		{
			vpp_AudioJpegDecClose();
		}
		else
		{
			vpp_SpeechClose();
		}
		
		VOC_OPEN=0;
	}

	if(pIsomGlob)
	{
		//destroy media box
		if(pIsomGlob->med_type==MCI_TYPE_MP4 || pIsomGlob->med_type==MCI_TYPE_3GP)
		{
			gf_isom_close_file(pIsomGlob->file);
		}
		else
		{
			if(pIsomGlob->avi_p)
				avi_close_input_file(pIsomGlob->avi_p);
		}
	}
	play_state=VID_PLAY_STATE_INVALID;

	//wait video work over
	while(AV_DEC_STATE )
	{
		sxr_Sleep(1600);
		diag_printf("[VID]AV_DEC_STATE busy!");
	}

	//avstreamstr_srv_testclose(pIsomGlob->filehdl);//test
	mmc_MemFreeAll();
	pIsomGlob=NULL;
	//VID_SET_MODE=0;

	diag_printf("[VID]  mmc_audio_isom_close_file  \n");

	return ISOM_ERR_OK;
}

int32 mmc_audio_isom_get_info( IsomPlayInfo* pInfo)
{
	if(play_state>=VID_PLAY_STATE_INVALID)
	{
		diag_printf("[VID]mmc_vid_isom_get_info play_state=%d ERR!", play_state);
		return ISOM_ERR_PLAY_STATE_FAIL;
	}

	/*get info*/
	pInfo->total_time=(pIsomGlob->total_time<500)?0:(pIsomGlob->total_time-500);
	pInfo->current_time= pIsomGlob->current_time;
	pInfo->total_frame_num=pIsomGlob->v_total_frame_num;
	pInfo->image_height=pIsomGlob->v_height;
	pInfo->image_width=pIsomGlob->v_width;
       pInfo->aud_channel_no = pIsomGlob->a_channels;
       pInfo->aud_sample_rate = pIsomGlob->a_sample_rate;
       pInfo->a_type   =  pIsomGlob->a_type;
     switch(pIsomGlob->a_type)
	{
       case A_TYPE_AAC:
       case A_TYPE_AMR:
       case A_TYPE_MP3:
	case A_TYPE_PCM:
	case A_TYPE_COOK:
       pInfo->track = MDI_VIDEO_TRACK_A_ONLY;
      		break;
	default:
		pInfo->track = MDI_VIDEO_TRACK_NONE;
		break;
    	}

     switch(pIsomGlob->v_type)
	{

       case V_TYPE_H263:
       case V_TYPE_MPEG4:
       case V_TYPE_MJPEG:


      		 if(pInfo->track == MDI_VIDEO_TRACK_A_ONLY)
			pInfo->track =  MDI_VIDEO_TRACK_AV;
               else
		     pInfo->track =  MDI_VIDEO_TRACK_V_ONLY;
      		break;
	default:
		break;
    	}

	return ISOM_ERR_OK;
}

int32 Audio_IsomUserMsg(COS_EVENT *pVidEv)
{
    int32 res;

	AV_DEC_STATE=1;

    switch(pVidEv->nEventId)
    {
        case MSG_VID_AUD_DEC_REQ:
            SEND_EVENT_NUM=0;

            res=aud_av_dec_frame();

            if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] UserMsg vid_audio_dec_frame err=%d \n", res);
            }

            break;

        case MSG_VID_IMG_DEC_REQ:
            SEND_EVENT_NUM=0;

            res=aud_av_dec_frame();

            if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                diag_printf("[VID] UserMsg vid_video_dec_frame err=%d \n", res);
            }

            break;

        case EV_TIMER:

            switch(pVidEv->nParam1)
            {
                case MMC_VID_PLY_TIMER_ID:

                    if(SEND_EVENT_NUM==0)
                    {
                        res = aud_av_dec_frame();

                        if(res!=ISOM_ERR_OK && res!=ISOM_ERR_PLAY_STATE_FAIL)
                        {
                            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                            diag_printf("[VID] UserMsg vid_video_dec_frame err=%d \n", res);
                        }
                    }
                    break;

                default:

                    break;
            }
            break;

        case MSG_VID_DISPLAY_REQ:
            //vid_display();
            break;

        case MSG_VID_STOP_REQ:

            if(pIsomGlob->open_audio)
                aud_StreamStop(audioItf);

            COS_KillTimerEX(GetMCITaskHandle(MBOX_ID_MMC), MMC_VID_PLY_TIMER_ID);

            pIsomGlob->current_time=0;
            pIsomGlob->vid_play_finish_callback(0);
            
            break;

        default:
            break;
    }

	AV_DEC_STATE=0;

    return ISOM_ERR_OK;

}


#endif

