#ifndef __TTSPLAYER_H
#define __TTSPLAYER_H

#include "tts.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TTS_FEATURES__
#define TTS_TASK_STACK_SIZE		1024	// 2k
#define TTS_TASK_PRIORITY          (5 + COS_PRI_PRV_BASE)
#endif
HANDLE g_hTTSTask;

#define TTSAPI		
#define ERRCODE				jtErrCode
#define JTTSPARAM			jtTTSParam
#define TTS_PLAYER_MAX_TEXTLEN	(1024 + 2)
//与引擎层相同的错误
#define TTS_PLAYER_ERR_NONE				jtTTS_ERR_NONE
#define TTS_PLAYER_ERR_PARAM			jtTTS_ERR_INPUT_PARAM
#define TTS_PLAYER_ERR_TOO_MORE_TEXT	jtTTS_ERR_TOO_MORE_TEXT
#define TTS_PLAYER_ERR_NOTINIT			jtTTS_ERR_NOT_INIT
#define TTS_PLAYER_ERR_OPENFILE			jtTTS_ERR_OPEN_DATA

//Player层独有的错误
typedef enum _TTS_PLAYER_ERR_MORE
{
	TTS_PLAYER_ERR_ALREADYINIT = 50,
	TTS_PLAYER_ERR_MEMORY,
	TTS_PLAYER_ERR_DONOTHING,
	TTS_PLAYER_ERR_PLAYING,
	TTS_PLAYER_ERR_AUDIO_DEVICE
}TTS_PLAYER_ERR_MORE;
//回调通知类型
typedef enum _TTS_PLAYER_NOTIFY
{
	TTS_PLAYER_NOTIFY_END,
	TTS_PLAYER_NOTIFY_ERROR
}TTS_PLAYER_NOTIFY;

//停止类型
typedef enum _TTS_PLAYER_PLAY_MODE
{
	TTS_PLAYER_PLAY_ASYNC,
	TTS_PLAYER_PLAY_SYNC
}jtTTSPlayMode;

//停止类型
typedef enum _TTS_PLAYER_STOP_MODE
{
	TTS_PLAYER_STOP_ASYNC,
	TTS_PLAYER_STOP_BLOCK
}jtTTSStopMode;
typedef enum _TTS_PLAYER_STATUS
{
	TTS_PLAYER_NOT_INIT,
	TTS_PLAYER_IDLE,
	TTS_PLAYER_SYNTHESIZING,
	TTS_PLAYER_PLAYING,
	TTS_PLAYER_PAUSE
}jtTTSPlayerStatus;
// user define callback type
typedef jtErrCode (* JTTSCALLBACKPROCEX)(
	long uMessage,				// [in] Message
	long lParam,				// [in] parameter 
	unsigned long wParam);		// [in] parameter 
	typedef void (*TTS_Finishicallback)(void); 
	
typedef S32	mdi_result;	
typedef void (*mdi_callback)(mdi_result result); 

//void TTS_PlayMain(void);
extern int ejTTS_Init();
extern int ejTTSPlayerToFile(char* pText,  size_t nSize);
extern UINT32  ejTTS_PlayToPCM(char* pText, size_t nSize, BOOL UCS2 );
UINT32 ejTTS_PlayToPCM_withCall(char* pText, size_t nSize, mdi_callback callback);

extern int ejTTSPlayerStop();
ERRCODE TTSAPI ejTTSPlayer_Play(jtTTSPlayMode ePlayMode, char *pszText, JTTSCALLBACKPROCEX lpfnCallback, unsigned long dwUserData);
ERRCODE TTSAPI ejTTSPlayer_PlayToFile(char *pszText, const char* pszFileName);

ERRCODE TTSAPI ejTTSPlayer_Stop(jtTTSStopMode eStopMode);
ERRCODE TTSAPI ejTTSPlayer_Resume(void);
ERRCODE TTSAPI ejTTSPlayer_Pause(void);

ERRCODE	TTSAPI ejTTSPlayer_GetStatus(int *piStatus);


ERRCODE TTSAPI ejTTSPlayer_SetParam(JTTSPARAM nParam, long lValue);
ERRCODE TTSAPI ejTTSPlayer_GetParam(JTTSPARAM nParam, long *plValue);
#ifdef __cplusplus
}
#endif

#endif	//__TTSPLAYER_H
