/**********************************************************************

  Audacity: A Digital Audio Editor

  DtmfGen.h

  Salvo Ventura
  Dec 2006

  An effect that generates DTMF tones

**********************************************************************/

#ifndef __AUDACITY_EFFECT_DTMF__
#define __AUDACITY_EFFECT_DTMF__

typedef struct
{
    unsigned short tone;
    unsigned int duration;
    unsigned int silencetime;
} DTMF_Tone_Struct;

typedef enum
{   
    DTMFGEN_STATE_IDLE,
    DTMFGEN_STATE_TONE,
    DTMFGEN_STATE_SILENCE,
}DTMFGEN_STATE;

typedef void (*DTMFGEN_HANDLER_T)(void);

extern void StartDtmfTone(unsigned short tone,unsigned int duration, unsigned int silencetime, unsigned int amplitudelevel, unsigned int fs, DTMFGEN_HANDLER_T handle  );

extern void StopDtmfTone();
extern bool DTMFGen_Process(short* framebuff, unsigned short framelen);

BOOL isDTMFGEN_Run();

#endif
