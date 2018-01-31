
#include "cs_types.h"
#include "string.h"

#include "hal_timers.h"
#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_speech.h"
#include "hal_overlay.h"
#include "cos.h"

#include "vois_m.h"


#include "aud_m.h"
#include "vpp_speech.h"
#include "calib_m.h"

#include "hal_map_engine.h"
#include "svcp_debug.h"
//#include "sxr_sbx.h"
//#include "sxr_tls.h"

#include "dtmf_decode.h"
#include "event.h"
#include "base_prv.h"


static struct{
   short            pDecodebuf[FRAME_BUF_SIZE * FRAME_BUF_NO];
   unsigned long   nDecodeLen;
   unsigned long    size;
   unsigned long   nPutLen;

   void             (*Callback)( char key );
   unsigned char         state;
   unsigned char         type;
   BOOL  bPreStart;
   
} kt_detect;

extern short dtmf_dt_flag;

static char  firstDtmf = 'X', secondDtmf = 'Y';

int dtmf_frame_count = 0;


short DecoderBuffer[DTMF_FRAME_SIZE];
void  reportDTMFChar(char chDtmf)
{
     SVC_TRACE (SVC_VOIS_TRC, 0, "reportDTMFChar key=%c",chDtmf);
      
}

 


int checkDtmfChar(short *pcm, int len)
{

	char		chDtmf;


	chDtmf = dtmf_check_character(pcm, len);
	
	//printf( "+GTDTMF: %c\n", chDtmf);



	if ((0 != chDtmf)&&(chDtmf>0)) {
		if (firstDtmf == 'X') 
		{
			if(secondDtmf == 'Y')
			{
				secondDtmf = chDtmf;
			}
			else if(secondDtmf == chDtmf)
			{
				if(dtmf_dt_flag == 1)
				{
					kt_detect.Callback(chDtmf);
					dtmf_dt_flag = 0;
				}
				firstDtmf = chDtmf;
				//printf("the dtmf_frame_count: %d\n",dtmf_frame_count);
			}
			else
			{
				firstDtmf = secondDtmf;
				secondDtmf = chDtmf;		
			}
		}
		else {
			
				firstDtmf = secondDtmf;
				secondDtmf = chDtmf;		
		}

	}
	else {

			firstDtmf = 'X';
			secondDtmf = 'Y';
	}

	dtmf_frame_count++;

	return 0;
}



char DTMF_Detect_Process( void )
{
       long  consumedLen;
   
       if(kt_detect.state != DTMF_STATE_RUN) 
            return -1;
   
	if( kt_detect.nPutLen< kt_detect.nDecodeLen) 
	{
	     consumedLen = kt_detect.size-kt_detect.nDecodeLen;
	     while (consumedLen >= DTMF_FRAME_SIZE)
	     {
                 memcpy(DecoderBuffer, kt_detect.pDecodebuf+ kt_detect.nDecodeLen, DTMF_FRAME_SIZE*sizeof(short));
		   checkDtmfChar(DecoderBuffer, DTMF_FRAME_SIZE);
                 kt_detect.nDecodeLen += DTMF_FRAME_SIZE;
                 consumedLen -= DTMF_FRAME_SIZE;
	     }

	     if(kt_detect.nPutLen+consumedLen> DTMF_FRAME_SIZE)
	     {
                 memcpy(DecoderBuffer, kt_detect.pDecodebuf+ kt_detect.nDecodeLen, consumedLen*sizeof(short));
                 memcpy(DecoderBuffer+consumedLen, kt_detect.pDecodebuf, (DTMF_FRAME_SIZE-consumedLen)*sizeof(short));
		   checkDtmfChar(DecoderBuffer, DTMF_FRAME_SIZE);
		   kt_detect.nDecodeLen = DTMF_FRAME_SIZE-consumedLen;
	   }
	}

      if( kt_detect.nPutLen> kt_detect.nDecodeLen) 
      {
           consumedLen = kt_detect.nPutLen-kt_detect.nDecodeLen;

	    while (consumedLen > DTMF_FRAME_SIZE)
	   {
                memcpy(DecoderBuffer, kt_detect.pDecodebuf+ kt_detect.nDecodeLen, DTMF_FRAME_SIZE*sizeof(short));
		  checkDtmfChar(DecoderBuffer, DTMF_FRAME_SIZE);
                kt_detect.nDecodeLen += DTMF_FRAME_SIZE;
                consumedLen -= DTMF_FRAME_SIZE;
	   }
     }
 
		
	return 1;

}


 void DTMF_DetectPushVoiceData( short *data, unsigned short len )
{
    unsigned long RemaingLen;

   if(kt_detect.state != DTMF_STATE_RUN) 
       return ;
   
   if(kt_detect.nPutLen< kt_detect.nDecodeLen)
   {
        RemaingLen = kt_detect.nDecodeLen-kt_detect.nPutLen;
	 if(RemaingLen<len) 
	 {
            memcpy(kt_detect.pDecodebuf+kt_detect.nPutLen, data,RemaingLen*sizeof(short));
            kt_detect.nPutLen += RemaingLen; 
	  }
	  else
	  {
            memcpy(kt_detect.pDecodebuf+kt_detect.nPutLen, data, len*sizeof(short));
            kt_detect.nPutLen += len; 
	 }

   }
   else
   {
	   if(kt_detect.nPutLen+len> kt_detect.size)
	   {
		  RemaingLen = kt_detect.size-kt_detect.nPutLen;
                memcpy(kt_detect.pDecodebuf+kt_detect.nPutLen, data,RemaingLen*sizeof(short));
		  kt_detect.nPutLen = 0;
          
		  if(kt_detect.nDecodeLen>= len-RemaingLen)
		  {
                    memcpy(kt_detect.pDecodebuf, data+RemaingLen,(len-RemaingLen)*sizeof(short));
                    kt_detect.nPutLen += len-RemaingLen;
		  }
          }
          else
         {
             memcpy(kt_detect.pDecodebuf+kt_detect.nPutLen, data,len*sizeof(short));
             kt_detect.nPutLen += len; 
         }//insure no overflow
   }

        COS_EVENT ev = { EV_DM_DTMF_DETECT_IND, 0, 0, 0 };
        COS_SendEvent(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

	
void DTMF_DetectStart(void (*callback)(char key))
{

   if(kt_detect.state != DTMF_STATE_IDLE) return;
   
    kt_detect.nDecodeLen = 0;
    kt_detect.nPutLen =0;
    kt_detect.size = FRAME_BUF_SIZE * FRAME_BUF_NO;

    if(callback == NULL)
        kt_detect.Callback = reportDTMFChar;
    else
        kt_detect.Callback = callback;
    
     dtmf_init_coeff();

     if(kt_detect.bPreStart)	
     {
          kt_detect.state = DTMF_STATE_RUN;
          kt_detect.bPreStart = FALSE;
     }
     else
     {
         hal_SwRequestClk(FOURCC_VOIS, HAL_SYS_FREQ_250M);

         kt_detect.state = DTMF_STATE_START; 
     }

}

void DTMF_DetectRun( unsigned char type)
{
     kt_detect.nDecodeLen = 0;
     kt_detect.nPutLen =0;
     kt_detect.type = type; //  SPEECH_STATE : VOIP_STATE 

     if(kt_detect.state == DTMF_STATE_IDLE) 
     {
          kt_detect.bPreStart = TRUE;
     }
     else
     {
         hal_SwRequestClk(FOURCC_VOIS, HAL_SYS_FREQ_250M);
         kt_detect.state = DTMF_STATE_RUN; 
     }
}


void DTMF_DetectStop( void )
{
    if(kt_detect.state != DTMF_STATE_IDLE)
    {
         kt_detect.state = DTMF_STATE_STOP;
         kt_detect.bPreStart = TRUE;
    }
}

void DTMF_DetectFinish( void )
{

    kt_detect.Callback = reportDTMFChar;

    kt_detect.state = DTMF_STATE_IDLE;
}

BOOL IsDTMF_Detect_Start()
{
    return  kt_detect.state != DTMF_STATE_IDLE;
} 
	
unsigned char DTMF_GetState(void)
{
    return kt_detect.state ;
}

//test code for simulator
# if 0
//void dtmf_demo_ysz(void)
void main(void )
{

	static short dtmfbuff[1000]={0};
	static short decodebuf[1200]={0};
	static short decodeLen = 0;

	FILE *src_pcm;
	

	//src_pcm = fopen("DTMF_0315_1859.trc.voc.pcm", "rb");
	//src_pcm = fopen("DTMF_0317_2021_NoDial.trc.voc.pcm","rb");
		src_pcm = fopen("DTMF_0317_2028_handfree.trc.voc.pcm", "rb");

	if (src_pcm == NULL)
	{
		printf("src error...");
		return;
	}

	DTMF_DetectStart(reportDTMFChar,SPEECH_STATE );

	//frame_length = FRAME;

	//dtmf_init_coeff();

	while (!feof(src_pcm))
	{
	


#if 0
		fread(dtmfbuff, sizeof(short), frame_length, src_pcm);

		
		/*
		len = 0;
		
		for (i=0; i<6; ++i) {
			if (QUEUE_BUFFER_EMPTY == Ecall_Queue_pop(dtmfbuff+len, ECALL_QUEUE_IN))
				break;
			len += 160;
		}
		*/
		len = frame_length;

		
		pos = 0;
		for (i=0; i<len; ++i) {
//yeshunzhou modify	
#if 0
			tmpV = dtmfbuff[i];
			if (tmpV < 0)
				tmpV = 0-tmpV;
			/*
			if (tmpV > dtmfValid) {
				pos++;
			}
			*/
			if (tmpV < dtmfValid) {
				continue;
			}
#endif			
			decodebuf[decodeLen++] = dtmfbuff[i];
			pos++;

		}

//yeshunzhou modify	
		if(pos < dtmfSamplePoint) break;
		
#if 0
		if (pos < (len/2) - 120)  {
			if (dtmfFilter) {
				if ((secondDtmf != 'Y') && (secondDtmf == firstDtmf)) {
					reportDTMFChar(secondDtmf);
					//hal_HstSendEvent(0xD7EF8888);
				}
			}
			else {
				if (secondDtmf != 'Y') {
					reportDTMFChar(secondDtmf);
					//hal_HstSendEvent(0xD7EF4444);
				}
				else if (firstDtmf != 'X') {
					reportDTMFChar(firstDtmf);
					//hal_HstSendEvent(0xD7EF5555);
				}
			}
			firstDtmf = 'X';
			secondDtmf = 'Y';
			decodeLen = 0;
			//break;
			continue;
		}
		if ((firstDtmf == secondDtmf) && (pos > (len-dtmfSamplePoint))) {
			//hal_HstSendEvent(0xD7EF6666);
			decodeLen = 0;
			//break;
			continue;
		}

#endif

#else
		fread(decodebuf, sizeof(short), FRAME_BUF_SIZE, src_pcm);

		DTMF_DetectPushVoiceData(decodebuf, FRAME_BUF_SIZE);
		DTMF_Detect_Process();
/*			
		for (i=0; i<6; ++i) {
			if (QUEUE_BUFFER_EMPTY == Ecall_Queue_pop(decodebuf+decodeLen, ECALL_QUEUE_IN))
				break;
			decodeLen += 160;
		}
*/		


	//	decodeLen += frame_length;
		
	//	if(decodeLen < dtmfSamplePoint) break;

#endif


	}
	
	DTMF_DetectStop();

	system("pause");

}
#endif


