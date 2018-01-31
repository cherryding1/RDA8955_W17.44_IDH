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





#include <stdio.h>
#include "amr_typedef.h"
//#include "n_proc.h"
//#include "cnst.h"
//#include "mode.h"
//#include "frame.h"
//#include "strfunc.h"
//#include "sp_dec.h"
//#include "d_homing.h"

#include "frmbtord.h"
#include "amrfile.h"
//#include "wavefile.h"
#include "interf_dec.h"
//#include "interf_enc.h"
#include "rom_dec.h"

#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)

static Speech_Decode_FrameState *speech_decoder_state=NULL;//sheen
static Word16 *serial;//[SERIAL_FRAMESIZE];   /* coded bits                    */

extern short ParseOneFrameBits( Word16* serial, AMRStruct* AmrStruct, short nChannel ) ;
/*
 *  Function        :   DecodeAmrToWav
 *--------------------------------------------------------------------------------
 *  Purpose         :   Decode AMR to wave
 *  Entry           :   AmrName -- char* , The input Amr file file name
 *                      WavName -- char* , The output wave sample file name ( no head )
 *
 *  Return          :   static void
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */
#if 0//sheen
int decodeOneAmrFile( char* AmrName , char* WavNAme )
{

    typedef void Speech_Decode_FrameState ;
#ifdef _IMPROVE_SPEED
    extern unsigned long GetWindowsTickCount() ;
    unsigned long t1 , t2 ;
#endif

    AMRStruct*  AmrStruct = 0 ;
    WAVStruct*  WavStruct = 0 ;
    Speech_Decode_FrameState *speech_decoder_state = NULL;

    Word16 serial[SERIAL_FRAMESIZE];   /* coded bits                    */
    Word16* synth0[6];        // L_FRAME    /* Synthesis                     */
    Word16* synthBuffer = 0  ;
    Word16* synth ;
    Word32 frame;

    Word16  reset_flag = 0;
    Word16  reset_flag_old = 1;
    Word16  i , n ;


//  int rxframetypeMode = 1 ;           /* use RX frame type codes       */
    enum Mode mode = (enum Mode)0;
    enum RXFrameType rx_type = (enum RXFrameType)0;
    enum TXFrameType tx_type = (enum TXFrameType)0;

    AmrStruct = OpenAmrFile( AmrName , 0 , 0 , 0 )  ;
    if( !AmrStruct )
        goto error ;
    synthBuffer = (Word16* ) malloc( AmrStruct->nChannels * L_FRAME * sizeof( Word16 ) ) ;
    if( !synthBuffer )
        goto error ;
    for( n = 0 ;  n < AmrStruct->nChannels ; n ++ )
        synth0[ n ] = synthBuffer + n*L_FRAME ;
    WavStruct = CreateWaveFile( WavNAme, AmrStruct->nChannels , 16 , AmrStruct->SampleRate ) ;
    if( !WavStruct )
        goto error ;
    speech_decoder_state = Decoder_Interface_init() ;
    if (!speech_decoder_state)
        //exit(-1);
        goto error ;


    /*-----------------------------------------------------------------------*
    * process serial bitstream frame by frame                               *
    *-----------------------------------------------------------------------*/

#ifdef _IMPROVE_SPEED
    t1 =  GetWindowsTickCount() ;
#endif

    frame = 0;
    while ( ReadOneFrameBlock( AmrStruct )  )
    {
        ++frame;
#ifdef DEBUG
        if ( (frame%50) == 0)
        {
            diag_printf ("frame=%d\n  ", frame);
        }
#endif
        for( n = 0 ; n < AmrStruct->nChannels ; n ++ )
        {
            synth = synth0[n] ;
            if( ParseOneFrameBits(  serial, AmrStruct, n ) == 0 )
                break ;
#if 0
            rx_type = (enum RXFrameType)serial[0];
            mode = (enum Mode) serial[1+MAX_SERIAL_SIZE];
            if (rx_type == RX_NO_DATA)
                mode = speech_decoder_state->prev_mode;
            else
                speech_decoder_state->prev_mode = mode;

            /* if homed: check if this frame is another homing frame */
            if (reset_flag_old == 1)
                /* only check until end of first subframe */
                reset_flag = decoder_homing_frame_test_first(&serial[1], mode);

            /* produce encoder homing frame if homed & input=decoder homing frame */
            if ((reset_flag != 0) && (reset_flag_old != 0))
            {
                for (i = 0; i < L_FRAME; i++)
                    synth[i] = EHF_MASK;
            }
            else
                /* decode frame */
                Speech_Decode_Frame(speech_decoder_state, mode, &serial[1],
                                    rx_type, synth);
#endif

            Decoder_Interface_Decode( speech_decoder_state , serial , synth , 0 );
#if 0
            /* if not homed: check whether current frame is a homing frame */
            if (reset_flag_old == 0)
                /* check whole frame */
                reset_flag = decoder_homing_frame_test(&serial[1], mode);

            /* reset decoder if current frame is a homing frame */
            if (reset_flag != 0)
                Speech_Decode_Frame_reset(speech_decoder_state);
            reset_flag_old = reset_flag;
#endif
        }
        WriteSeparateWaveFile( WavStruct , synth0 , AmrStruct->SampleRate/50 , AmrStruct->nChannels ) ;
        printf("frame %d\n",frame);
    }

#ifdef _IMPROVE_SPEED
    t2 = GetWindowsTickCount() ;
    printf(" Frame num is %d \n  decode time is %ld  \n", frame ,  t2-t1 ) ;
#endif

    /*-----------------------------------------------------------------------*
    * Close down speech decoder                                             *
    *-----------------------------------------------------------------------*/
//  Speech_Decode_Frame_exit(&speech_decoder_state);
    Decoder_Interface_exit(speech_decoder_state);
    CloseWaveFile( WavStruct );
    CloseAmrFile( AmrStruct );
    free(synthBuffer );

    return 1;

error:
    if( WavStruct )
        CloseWaveFile( WavStruct );
    if( AmrStruct )
        CloseAmrFile( AmrStruct );
    if( synthBuffer )
        free(synthBuffer );
    if(speech_decoder_state )
        Decoder_Interface_exit(speech_decoder_state);
    return 0 ;
}
#endif//sheen

//sheen
AMRStruct* AmrDecInit(char *pBufIn, int *bytesConsume)
{
    AMRStruct* AmrStruct = 0  ;
    AmrStruct= OpenAmrFile(pBufIn,bytesConsume,0,0,0);
    if(!AmrStruct)
    {
        diag_printf("OpenAmrFile err!");
        goto error;
    }
    speech_decoder_state = NULL;
    speech_decoder_state = Decoder_Interface_init() ;
    if (!speech_decoder_state)
    {
        diag_printf("Decoder_Interface_init err!");
        goto error ;
    }
    serial= (Word16*)mmc_MemMalloc(SERIAL_FRAMESIZE* sizeof(Word16));   /* coded bits                    */
    if(!serial)
    {
        diag_printf("serial malloc fail!");
        goto error ;
    }

    return AmrStruct;
error:

    if( AmrStruct )
        CloseAmrFile( AmrStruct );
    if(speech_decoder_state )
        Decoder_Interface_exit(speech_decoder_state);
#ifdef SHEEN_VC_DEBUG
    if(serial)
        free(serial);
#endif

    return 0;
}

//sheen
int AmrFrameDec(AMRStruct* AmrStruct, char *pBufIn, char *pBufOut, int *bytesLeft)
{
    Word16* synth0[6];        // L_FRAME    /* Synthesis                     */
    Word16  n ;
    Word16* synth ;

    if(!AmrStruct || !pBufIn|| !pBufOut|| !bytesLeft)
        return 0;


    for( n = 0 ;  n < AmrStruct->nChannels ; n ++ )
        synth0[ n ] = (Word16*)(pBufOut + n*L_FRAME );

    AmrStruct->fd= (int)pBufIn;//buf pos reuse

    if(ReadOneFrameBlock( AmrStruct, bytesLeft ) !=1)
        return 0;

    for( n = 0 ; n < AmrStruct->nChannels ; n ++ )
    {
        synth = synth0[n] ;
        if( ParseOneFrameBits(  serial, AmrStruct, n ) == 0 )
            break ;

        Decoder_Interface_Decode( speech_decoder_state , serial , synth , 0 );

        //for 2 channels
        /*
        long bytes ;
        unsigned char buf0[48] ;
        unsigned char* buf ;
        unsigned char* samplesTmp[12] ;
        long bytesEachChannel;
        int n , k ;

        if( nChannel != WavStruct->foramt.wChannels )
            return 0 ;
        memcpy( samplesTmp , samples , nChannel*sizeof( unsigned char* ) ) ;
        bytesEachChannel = WavStruct->foramt.wBitsPerSample/8 ;

        buf = buf0 ;
        for( n = 0 ; n < sampleNum ; n ++  )
        {
            for( k = 0 ; k < nChannel ; k ++ )
            {
                memcpy( buf , samplesTmp[k]  , bytesEachChannel ) ;
                samplesTmp[k] += bytesEachChannel ;
                buf += bytesEachChannel ;
            }
            buf = buf0 ;
            bytes = fwrite( buf , 1 , WavStruct->foramt.wBlockAlign , WavStruct->fp ) ;
            WavStruct->data.chunkSize += bytes ;
        }
        */
    }

    return 1;
}

//sheen
void AmrDecClose(AMRStruct* AmrStruct)
{
    if(speech_decoder_state)
        Decoder_Interface_exit(speech_decoder_state);
    CloseAmrFile( AmrStruct );

    return;
}

#ifdef SHEEN_VC_DEBUG
void main (int argc, char *argv[])
{
    FILE *fIn,*fOut;
    char *pBufIn,*pBufOut;
    AMRStruct*  AmrStruct = 0 ;
    int consume=0,bytesleft=0;
    int i;

    fIn= fopen("F:\\测试文件\\amr\\out_mode_7.amr","rb");
    fOut= fopen("F:\\测试文件\\amr\\test.pcm","wb");
    pBufIn= malloc(32);
    pBufOut= malloc(L_FRAME*2);

    fread(pBufIn,1,6,fIn);
    AmrStruct= AmrDecInit(pBufIn, &consume);
    memmove(pBufIn,pBufIn +consume,6- consume);
    bytesleft=6- consume;
    consume=0;
    i=0;

    while(1)
    {
        bytesleft += fread(pBufIn,1,32- bytesleft,fIn);
        if(bytesleft<32)
            break;
        AmrFrameDec(AmrStruct,pBufIn,pBufOut,&bytesleft);
        fwrite(pBufOut,2,L_FRAME,fOut);
        printf("frame %d\n",i);
        i++;
    }

    AmrDecClose(AmrStruct);

    return;
}
#endif
