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

#ifndef _MCI_MIDI_H__
#define _MCI_MIDI_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef SHEEN_VC_DEBUG

typedef unsigned char uint8;
typedef unsigned char UINT8;
typedef unsigned char U8;
typedef unsigned short uint16;
typedef unsigned short U16;
typedef int INT32;
typedef unsigned int uint32;
typedef int int32;
typedef short int16;
#ifdef WIN32
typedef __int64 INT64;
#else
typedef long long INT64;
#endif
typedef FILE* HANDLE;
typedef char BOOL;
#define FALSE 0;
#define TRUE 1
#define inline
#define diag_printf printf

typedef struct _PlayInfromation
{
    INT32 PlayProgress;
    INT32 reserve;
} MCI_PlayInf;

char *pcmbuf_overlay;

#define FS_SEEK_SET 0
#define FS_SEEK_CUR 1
#define FS_SEEK_END 2

#endif

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

#define  MAX_TRACK   16 //sheen
#define  MAX_PGM_MLC   16 //instrument malloc num,sheen

#define MIDIPOOL_TEMP  0
#define MIDIPOOL_PERMANENT  1
#define MIDIPOOL_SPECIAL  2 //for evlist, sheen
//#define MIDIPOOL_NUMPOOLS  2 sheen
#define MIDIPOOL_NUMPOOLS  3


#define ALIGN_TYPE      int

typedef struct midi_pool_struct * midi_pool_ptr;

typedef struct midi_pool_struct
{
    midi_pool_ptr  next;    /* next in list of pools */
    size_t bytes_used;      /* how many bytes already used within pool */
    size_t bytes_left;      /* bytes still available in this pool */
} midi_pool_hdr;


//#define  RUN_ON_GREENSTONE
#define FAST_DECAY
//#define HANDLE   sint32
/* Bits in modes: */
#define MODES_16BIT     (1<<0)
#define MODES_UNSIGNED  (1<<1)
#define MODES_LOOPING   (1<<2)
#define MODES_PINGPONG  (1<<3)
#define MODES_REVERSE   (1<<4)
#define MODES_SUSTAIN   (1<<5)
#define MODES_ENVELOPE  (1<<6)


/* A hack to delay instrument loading until after reading the
   entire MIDI file. */
#define MAGIC_LOAD_INSTRUMENT ((MidInstrument *) (-1))

#define SPECIAL_PROGRAM -1

#define PE_MONO     0x01  /* versus stereo */
#define PE_SIGNED   0x02  /* versus unsigned */
#define PE_16BIT    0x04  /* versus 8-bit */

/* Midi events */
#define ME_NONE     0
#define ME_NOTEON   1
#define ME_NOTEOFF  2
#define ME_KEYPRESSURE  3
#define ME_MAINVOLUME   4
#define ME_PAN      5
#define ME_SUSTAIN  6
#define ME_EXPRESSION   7
#define ME_PITCHWHEEL   8
#define ME_PROGRAM  9
#define ME_TEMPO    10
#define ME_PITCH_SENS   11

#define ME_ALL_SOUNDS_OFF   12
#define ME_RESET_CONTROLLERS    13
#define ME_ALL_NOTES_OFF    14
#define ME_TONE_BANK    15

#define ME_LYRIC    16

#define ME_EOT      99

/* Causes the instrument's default panning to be used. */
#define NO_PANNING -1

/* Voice status options: */
#define VOICE_FREE 0
#define VOICE_ON 1
#define VOICE_SUSTAINED 2
#define VOICE_OFF 3
#define VOICE_DIE 4

/* Voice panned options: */
#define PANNED_MYSTERY 0
#define PANNED_LEFT 1
#define PANNED_RIGHT 2
#define PANNED_CENTER 3
/* Anything but PANNED_MYSTERY only uses the left volume */

#define ISDRUMCHANNEL(s, c) (((s)->drumchannels & (1<<(c))))

/* Audio format flags (defaults to LSB byte order)
 */
#define MID_AUDIO_U8        0x0008  /* Unsigned 8-bit samples */
#define MID_AUDIO_S8        0x8008  /* Signed 8-bit samples */
#define MID_AUDIO_U16LSB    0x0010  /* Unsigned 16-bit samples */
#define MID_AUDIO_S16LSB    0x8010  /* Signed 16-bit samples */
#define MID_AUDIO_U16MSB    0x1010  /* As above, but big-endian byte order */
#define MID_AUDIO_S16MSB    0x9010  /* As above, but big-endian byte order */
#define MID_AUDIO_U16       MID_AUDIO_U16LSB
#define MID_AUDIO_S16       MID_AUDIO_S16LSB

typedef enum
{
    MID_SONG_TEXT = 0,
    MID_SONG_COPYRIGHT = 1
} MidSongMetaId;

#define MID_VIBRATO_SAMPLE_INCREMENTS 32

/* Maximum polyphony. */
#define MID_MAX_VOICES  48
/* In percent. */
#define DEFAULT_AMPLIFICATION   70

/* Default polyphony */
#define DEFAULT_VOICES  24

/* 1000 here will give a control ratio of 22:1 with 22 kHz output.
   Higher CONTROLS_PER_SECOND values allow more accurate rendering
   of envelopes and tremolo. The cost is CPU time. */
#define CONTROLS_PER_SECOND 1000

/* 9 here is MIDI channel 10, which is the standard percussion channel.
   Some files (notably C:\WINDOWS\CANYON.MID) think that 16 is one too.
   On the other hand, some files know that 16 is not a drum channel and
   try to play music on it. This is now a runtime option, so this isn't
   a critical choice anymore. */
#define DEFAULT_DRUMCHANNELS ((1<<9) | (1<<15))
#define FRACTION_BITS 12
#define GUARD_BITS 3
#define AMP_BITS (15-GUARD_BITS)
#define MAX_AMP_VALUE ((1<<(AMP_BITS+1))-1)


#define SWEEP_SHIFT 16
#define RATE_SHIFT 5
/* Vibrato and tremolo Choices of the Day */
#define SWEEP_TUNING 38
#define MAX_DIE_TIME 20
#define MAX_AMPLIFICATION 800
#define MAX_CONTROL_RATIO 255
#define DEFAULT_PROGRAM 0

/* change FRACTION_BITS above, not these */
#define INTEGER_MASK (0xFFFFFFFF << FRACTION_BITS)
#define FRACTION_MASK (~ INTEGER_MASK)
#define SINE_CYCLE_LENGTH 1024


#define XCHG_SHORT(x) ((((x)&0xFF)<<8) | (((x)>>8)&0xFF))
# define XCHG_LONG(x) ((((x)&0xFF)<<24) | \
              (((x)&0xFF00)<<8) | \
              (((x)&0xFF0000)>>8) | \
              (((x)>>24)&0xFF))

#define SWAPLE16(x) x
#define SWAPLE32(x) x
#define SWAPBE16(x) XCHG_SHORT(x)
#define SWAPBE32(x) XCHG_LONG(x)
#define s32tou16l s32tou16
#define s32tou16b s32tou16x
#define s32tos16l s32tos16
#define s32tos16b s32tos16x
/////////////////////////////////////////////////////////////////
typedef sint16 sample_t;
typedef sint32 final_volume_t;
#ifndef SHEEN_VC_DEBUG
#define  DEBUG_MSG     diag_printf
typedef  INT32  MidIStream;

#else
#define DEBUG_MSG       printf
typedef  FILE*  MidIStream;
#endif

typedef struct _MidSample MidSample;
struct _MidSample
{
    sint32
    loop_start, loop_end, data_length,
                sample_rate, low_vel, high_vel, low_freq, high_freq, root_freq;
    sint32 envelope_rate[6], envelope_offset[6];
    //float volume;
    sint32 volume_fix;//q12  zouying adds this on 2009-11-19
    sample_t *data;
    sint32
    tremolo_sweep_increment, tremolo_phase_increment,
                             vibrato_sweep_increment, vibrato_control_ratio;
    uint8 tremolo_depth, vibrato_depth, modes;
    sint8 panning, note_to_use;
};

typedef struct _MidChannel MidChannel;
struct _MidChannel
{
    int bank, program, volume, sustain, panning, pitchbend, expression;
    int mono; /* one note only on this channel -- not implemented yet */
    int pitchsens;
    /* chorus, reverb... Coming soon to a 300-MHz, eight-way superscalar
       processor near you */
    //float pitchfactor; /* precomputed pitch bend factor to save some fdiv's */
    sint32 pitch_fix;//q30  zouying adds this variable for fixed point computation.
};

typedef struct _MidVoice MidVoice;
struct _MidVoice
{
    uint8 status, channel, note, velocity;
    MidSample *sample;
    sint32
    orig_frequency, frequency,
                    sample_offset, sample_increment,
                    envelope_volume, envelope_target, envelope_increment,
                    tremolo_sweep, tremolo_sweep_position,
                    tremolo_phase, tremolo_phase_increment,
                    vibrato_sweep, vibrato_sweep_position;

    final_volume_t left_mix, right_mix;

    //float  left_amp, right_amp, tremolo_volume;
    sint32  lamp_fix,ramp_fix,tremolo_volume_fix;//q12  zouying adds this on 2009-11-19

    sint32 vibrato_sample_increment[MID_VIBRATO_SAMPLE_INCREMENTS];
    int
    vibrato_phase, vibrato_control_ratio, vibrato_control_counter,
                   envelope_stage, control_counter, panning, panned;

};

typedef struct _MidInstrument MidInstrument;
struct _MidInstrument
{
    uint8 samples;
    MidSample *sample;
};

typedef struct _MidToneBankElement MidToneBankElement;
struct _MidToneBankElement
{
    //char *name;  //remove by zouying
    int note, amp, pan, strip_loop, strip_envelope, strip_tail;
};

typedef struct _MidToneBank MidToneBank;
struct _MidToneBank
{
    MidToneBankElement *tone;
    MidInstrument *instrument[128];
};

typedef struct _MidEvent MidEvent;
struct _MidEvent
{
    sint32 time;
    uint8 channel, type, a, b;
};

typedef struct _MidEventList MidEventList;
struct _MidEventList
{
    MidEvent event;
    void *next;
};

typedef struct _MidEventListShort MidEventListShort;
struct _MidEventListShort //sheen
{
    uint16 trackNum;//sign which track's event. also sign enable read or write.
    int16 endFlag;//track end flag,0=have data,MAGIC_EOT=end
    MidEvent event;
    void *pre;
    void *next;
};

struct _MidSong
{
    MidIStream fileStrm;//sheen
    int playing;
    sint32 rate;
    sint32 totalSample;//total time with sample, sheen
    sint16 format;//0=one track, 1=two or more tracks and play simultaneously, 2=more and play sequentially, sheen
    sint16 tracksNum;//valid tracks number, sheen
    sint32 divisions;//sheen
    sint32 encoding;
    int bytes_per_sample;
    //float master_volume;
    sint32 master_volume_fix;//q12  zouying adds this on 2009-11-19
    sint32 amplification;
    //MidDLSPatches *patches;
    MidToneBank *tonebank;//MidToneBank *tonebank[128];
    MidToneBank *drumset;//MidToneBank *drumset[128];
    MidInstrument *default_instrument;
    int default_program;
    void (*write) (void *dp, sint32 * lp, sint32 c);
    int buffer_size;
    //sample_t *resample_buffer;
    //sint32 *common_buffer;
    /* These would both fit into 32 bits, but they are often added in
       large multiples, so it's simpler to have two roomy ints */
    /* samples per MIDI delta-t */
    sint32 sample_increment;
    sint32 sample_correction;
    MidChannel channel[16];
    MidVoice voice[MID_MAX_VOICES];
    int voices;
    sint32 drumchannels;
    sint32 control_ratio;
    sint32 lost_notes;
    sint32 cut_notes;
    sint32 samples;
    MidEvent *events;//reuse evlist memmory space. sheen
    MidEvent *current_event;
    //MidEventList *evlist; sheen
    sint32 current_sample;
    sint32 event_count;
    sint32 *at;//point to every tracks at. sheen
    sint32 groomed_event_count;
    char *meta_data[8];
};


typedef struct _MidSong MidSong;
typedef struct _MidSongOptions MidSongOptions;
struct _MidSongOptions
{
    sint32 rate;      /* DSP frequency -- samples per second */
    uint16 format;    /* Audio data format */
    uint8 channels;   /* Number of channels: 1 mono, 2 stereo */
    uint16 buffer_size;   /* Sample buffer size in samples */
};

typedef enum
{
    MSG_MMC_MIDI_PLAY=0,
    MSG_MMC_MIDI_STOP=1,
    MSG_MMC_MID_SBC_VOC=2,
    MSG_MMC_MIDI_ERROR =3,

} MIDIDEC_USER_MSG;

typedef enum
{
    MIDIDEC_ERROR_NO=0,
    MIDIDEC_ERROR_FORMAT=1,
    MIDIDEC_ERROR_OUTOF_SPACE =2,

} MIDIDEC_ERROR_TYPE;

///////////////////////////////////////////////////////////////////
int load_missing_instruments(MidSong *song);
//void free_instruments(MidSong *song);
//int set_default_instrument(MidSong *song1, char *name);

int div_320_320_lshift(sint32 divd,sint32 divs,sint32 loop);
sint32  mult32_r1(sint32 x,sint32 y,sint32 r);

MidInstrument *load_instrument_dls(MidSong *song, int drum, int bank, int instrument);

void mix_voice(MidSong *song, sint32 *buf, int v, sint32 c);
int recompute_envelope(MidSong *song, int v);
void apply_envelope_to_amp(MidSong *song, int v);

MidEvent *read_midi_file(MidIStream stream, MidSong *song);
sample_t *resample_voice(MidSong *song, int v, sint32 *countptr);

/* Core Library Functions
 * ======================
 */

/* Initialize the library. If config_file is NULL
 * search for configuratin file in default directories
 */
int mid_init (char *config_file);

/* Initialize the library without reading any
 * configuratin file
 */
int mid_init_no_config (void);

/* Shutdown the library
 */
void mid_exit (void);


/* Input Stream Functions
 * ======================
 */

/* Create input stream from a file name
 */
//  extern MidIStream *mid_istream_open_file (const char *file);

/* Create input stream from a file pointer
 */
//MidIStream *mid_istream_open_fp (FILE * fp, int autoclose);

/* Create input stream from memory
 */
MidIStream mid_istream_open_mem (void *mem, size_t size,
                                 int autofree);


/* Read data from input stream
 */
size_t mid_istream_read (MidIStream stream, void *ptr, size_t size,
                         size_t nmemb);

/* Skip data from input stream
 */
void mid_istream_skip (MidIStream  stream, size_t len);



/* Load MIDI song
 */
MidSong *mid_song_load (MidIStream stream,
                        MidSongOptions * options);

/* Load MIDI song with specified DLS pathes
 */
MidSong *mid_song_load_dls (MidIStream  stream,MidSongOptions * options);

/* Set song amplification value
 */
void mid_song_set_volume (MidSong * song, int volume);

/* Seek song to the start position and initialize conversion
 */
void mid_song_start (MidSong * song);

/* Read WAVE data
 */
size_t mid_song_read_wave (MidSong * song, void *ptr, size_t size);

/* Seek song to specified offset in millseconds
 */
void mid_song_seek (MidSong * song, uint32 ms);

/* Get total song time in millseconds
 */
uint32 mid_song_get_total_time (MidSong * song);

/* Get current song time in millseconds
 */
uint32 mid_song_get_time (MidSong * song);

/* Get song meta data. Return NULL if no meta data found
 */
char *mid_song_get_meta (MidSong * song, MidSongMetaId what);

/* Destroy song
 */
void mid_song_free (MidSong * song);



void s32tos8(void *dp, sint32 *lp, sint32 c);
void s32tou8(void *dp, sint32 *lp, sint32 c);
void s32tos16(void *dp, sint32 *lp, sint32 c);
void s32tou16(void *dp, sint32 *lp, sint32 c);
void s32tos16x(void *dp, sint32 *lp, sint32 c);
void s32tou16x(void *dp, sint32 *lp, sint32 c);

size_t  midi_Fread(void *dest, size_t unitlen, size_t size, MidIStream fp);
size_t  midi_Fwrite(void *dest, size_t unitlen, size_t size, MidIStream fp);
MidIStream midi_Fopen(char* filename ,const char* format);
sint32 midi_FCloseFile(MidIStream fp);
//void *safe_malloc(size_t count);
//void midi_freespace(void* p);
void  midi_make_huffutable();
int  midi_ReadByte();
int midi_DecodeElement();
int  get_last();
void skip_to(MidSong *song, sint32 until_time);


//void *safe_malloc(size_t count);
//void midi_freespace(void* p,size_t count);
void *midi_alloc_small(int pool_id, size_t sizeofobject);
int midi_free_pool (int pool_id);
void midi_free_all();
///////////////////interfaces//////////////////////////////////////////////////////////
int32 Audio_MidiPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);
int32 Audio_MidiPause (void);
int32 Audio_MidiResume ( HANDLE fhd);
int32 Audio_MidiGetID3 (char * pFileName) ;
int32 Audio_MidiStop (void);
int32 Audio_MidiUserMsg(int32 nMsg);
int32 Audio_MidiGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV);
MIDIDEC_ERROR_TYPE  MMC_Midi2RawBuffer(INT32 *pBuffer,  INT32 len, UINT8 loop);
MidInstrument *load_instrument_from_array(MidSong *song, int num, int percussion,
        int panning, int amp, int note_to_use,
        int strip_loop, int strip_envelope,
        int strip_tail, MidInstrument *pReUseInstrument);//MidInstrument *pReUseInstrument, add sheen
uint32 midi_GetDurationTime(
    HANDLE FileHander,
    INT32 filesize,
    INT32 BeginPlayProgress,
    INT32 OffsetPlayProgress        );

PUBLIC MCI_ERR_T MMC_MidGetFileInformation (CONST INT32 FileHander,
        AudDesInfoStruct  * CONST DecInfo  );

#endif

