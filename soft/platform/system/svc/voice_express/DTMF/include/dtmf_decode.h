/* dtmf_decode.h */ 
#ifndef   __DTMF_DECODE_
#define    __DTMF_DECODE_
#include "cs_types.h"

#define  FRAME_BUF_NO            12
#define  FRAME_BUF_MASK          (FRAME_BUF_NO-1)
#define  FRAME_BUF_SIZE          160
#define  DTMF_DETECTBUF_SIZE FRAME_BUF_NO*FRAME_BUF_SIZE
#define  DTMF_FRAME_SIZE         205

#define   DTMF_STATE_IDLE        0
#define   DTMF_STATE_START     1
#define   DTMF_STATE_RUN         2
#define   DTMF_STATE_STOP       3

#define  DTMF_DETECT_TYPE_VOISE   0
#define  DTMF_DETECT_TYPE_VOIP     1

void dtmf_init_coeff(void);
char dtmf_check_character(signed short *x, int len);

void DTMF_DetectStart(void (*callback)(char key));
bool IsDTMF_Detect_Start();
unsigned char DTMF_GetState(void);
void DTMF_DetectStop( void );
void DTMF_DetectRun( unsigned char type);
void DTMF_DetectFinish( void );
void DTMF_DetectPushVoiceData( short *data, unsigned short len );
char DTMF_Detect_Process( void );

#endif

