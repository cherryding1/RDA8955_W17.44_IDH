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
#include "amrfile.h"
#include "amr_typedef.h"
//#include "cnst.h"
//#include "frame.h"
#include "frmbtord.h"
//#include "interf_enc.h"
#include "interf_dec.h"
//#include "mode.h"
#include "sp_dec.h"


/*
 *  Function        :   ParseOneFrameBits
 *--------------------------------------------------------------------------------
 *  Purpose         :   Get the bits of one frame ,1bit to 1 byte
 *  Entry           :   serial -- Word16* , The buffer to fill frame bits in
 *                          The length is SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)
 *                      AmrStruct -- AMRStruct*  , The amr file manager
 *                      nChannel--short , # of channel
 *
 *  Return          :   short
 *                      1, if success
 *                      0, if Fail
 *  External Ref.   :   CurrMode--int , to store current mode
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */

#if 0//sheen
enum RXFrameType { RX_SPEECH_GOOD = 0,
                   RX_SPEECH_DEGRADED,
                   RX_ONSET,
                   RX_SPEECH_BAD,
                   RX_SID_FIRST,
                   RX_SID_UPDATE,
                   RX_SID_BAD,
                   RX_NO_DATA,
                   RX_N_FRAMETYPES     /* number of frame types */
                 };
#endif

short ParseOneFrameBits( Word16* serial, AMRStruct* AmrStruct, short nChannel )
{
    Word16* serialTmp ;
    unsigned char* buf;
    int frametype , quality , mode , RX_Type ;
    int i ;
    if( nChannel >= AmrStruct->nChannels  )
        return 1 ;
    buf = AmrStruct->ChannelBuffer[ nChannel] ;
    frametype  = (buf[0] >>3 ) &15 ;
    //quality = (buf[0] >> 2 )&1 ;
    quality =1; //ignore quality bit;

    if(  (frametype >=0 ) && (frametype<=7)  )
    {
        short* tableX ;
        int bit = 0 ;
        int k ;
        int allbytes ;

        if( quality == 1 )
            RX_Type = RX_SPEECH_GOOD ;
        else
            RX_Type = RX_SPEECH_DEGRADED;
        mode = frametype ;
        serial[0] =  RX_Type ; // RX Type
        serial[1+MAX_SERIAL_SIZE] = mode ; //Mode indication
        AmrStruct->CurrMode[ nChannel ] = mode ;

        serialTmp = serial + 1 ;
        tableX = table0__7[mode] ;
        allbytes = bit_Byte_Number[mode][0]/8;
        buf ++ ;
        for( i = 0 ; i <  allbytes ; i ++ )
        {
            serialTmp [ tableX[bit++] ] =  (buf[i] >> 7) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 6) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 5) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 4) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 3) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 2) & 1  ;
            serialTmp [ tableX[bit++] ] = (buf[i] >> 1) & 1  ;
            serialTmp [ tableX[bit++] ] = buf[i] & 1 ;
        }
        for( k = 7 ; k > 7-bit_Byte_Number[mode][0]%8 ; k -- )
            serialTmp [ tableX[bit++] ] = (buf[i] >> k) & 1  ;
        return 1 ;
    }
    else if( frametype == 8 ) //SID Frame
    {
        int STI;
        int bit = 0 ;
        //printf("Encounter SID\n") ;
        STI = buf[4]& 0x10 ;
        if(STI==0 )
            RX_Type = RX_SID_FIRST ;
        else
            RX_Type = RX_SID_UPDATE ;
        mode = (buf[4]>>1)&7; //((buf[4]>>3)&1)|((buf[4]>>1)&2)|((buf[4]<<1)&4) ;
#ifdef DEBUG_AMR
        if( AmrStruct->CurrMode[ nChannel ] != mode )
            printf("Why not equal\n");
#endif
        serial[0] =  RX_Type ; // RX Type
        serial[1+MAX_SERIAL_SIZE] = mode ; //Mode indication
        AmrStruct->CurrMode[ nChannel ] = mode ;
        serialTmp = serial + 1 ;
        for( i = 0 ; i < 4 ; i ++ )
        {
            serialTmp [ bit++ ] = (buf[i] >> 7) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 6) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 5) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 4) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 3) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 2) & 1  ;
            serialTmp [ bit++ ] = (buf[i] >> 1) & 1  ;
            serialTmp [ bit++ ] = buf[i] & 1 ;
        }
        serialTmp [ bit++ ] =  (buf[4] >> 7) & 1  ;
        serialTmp [ bit++ ] = (buf[4] >> 6) & 1  ;
        serialTmp [ bit++ ] = (buf[4] >> 5) & 1  ;
        return 1 ;
    }
    else if( frametype == 15 )
    {
        serial[0] = RX_NO_DATA;
        serial[1+MAX_SERIAL_SIZE] = AmrStruct->CurrMode[ nChannel ] ;
        return 1 ;
    }
    else
    {
        return 0 ;
        while (1 )
        {
            diag_printf("Error\n" ) ;
            i = 0 ;
            i ++   ;
        }
    }
}



/*
 *  Function        :   CombineOneFrameBits
 *--------------------------------------------------------------------------------
 *  Purpose         :   Get the bits of one frame ,1bit to 1 byte
 *  Entry           :   serial -- Word16* , The buffer to fill frame bits in
 *                          The length is SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)
 *                      AmrStruct -- AMRStruct*  , The amr file manager
 *                      nChannel--short , # of channel
 *
 *  Return          :   short
 *                      1, if success
 *                      0, if Fail
 *  External Ref.   :   CurrMode--int , to store current mode
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */


enum TXFrameType { TX_SPEECH_GOOD = 0,
                   TX_SID_FIRST,
                   TX_SID_UPDATE,
                   TX_NO_DATA,
                   TX_SPEECH_DEGRADED,
                   TX_SPEECH_BAD,
                   TX_SID_BAD,
                   TX_ONSET,
                   TX_N_FRAMETYPES     /* number of frame types */
                 };


short CombineOneFrameBits(
    Word16* serial,
    AMRStruct* AmrStruct,
    enum TXFrameType tx_type,
    enum Mode mode,
    short nChannel
)
{
    unsigned char* buf ;
    short bit , i ;
    short* table ;
    Word16* serialTmp ;

    if( nChannel >= AmrStruct->nChannels )
        return 0 ;
    buf =  AmrStruct->ChannelBuffer [ nChannel ] ;
    switch( tx_type )
    {
        case TX_SPEECH_GOOD :
        case TX_SPEECH_DEGRADED:
        case TX_SPEECH_BAD:
            if( tx_type == TX_SPEECH_GOOD )
                buf[0] = (mode << 3)|4 ;
            else
                buf[0] = ( mode << 3) ;
            bit = 1 ;
            i = 0 ;
            table = table0__7[ mode ] ;
//          serialTmp = serial + 1 ;
            serialTmp = serial ;
            while(  i < bit_Byte_Number[mode][0] )
            {
                buf[bit]  = serialTmp[table[i++]]<<7 ;
                buf[bit] |= serialTmp[table[i++]]<<6 ;
                buf[bit] |= serialTmp[table[i++]]<<5 ;
                buf[bit] |= serialTmp[table[i++]]<<4 ;
                buf[bit] |= serialTmp[table[i++]]<<3 ;
                buf[bit] |= serialTmp[table[i++]]<<2 ;
                buf[bit] |= serialTmp[table[i++]]<<1 ;
                buf[bit] |= serialTmp[table[i++]] ;
                bit ++ ;
            }
            break;

        case TX_SID_FIRST:
            buf[0] = (8 << 3)|4 ;
            buf[4] = mode<<1 ;
            buf[1] = buf[2] = buf[3] = 0 ;
            break;
        case TX_SID_UPDATE:
        case TX_SID_BAD:
            if( tx_type == TX_SID_UPDATE )
                buf[0] = (8 << 3)|4 ;
            else
                buf[0] = (8 << 3);
            buf[4] = (mode<<1)|0x10 ;
            bit = 1 ;
            for( i = 0 ; i < 3 ; i ++ )
            {
                buf[i] = (serial[bit++]<<7) ;
                buf[i] |= (serial[bit++]<<6) ;
                buf[i] |= (serial[bit++]<<5) ;
                buf[i] |= (serial[bit++]<<4) ;
                buf[i] |= (serial[bit++]<<3) ;
                buf[i] |= (serial[bit++]<<2) ;
                buf[i] |= (serial[bit++]<<1) ;
                buf[i] |= serial[bit++] ;
            }
            buf[4] |= serial[bit++]<<7 ;
            buf[4] |= serial[bit++]<<6 ;
            buf[4] |= serial[bit++]<<5 ;
            break;
        case TX_NO_DATA:
            buf[0] = (15<<1) ;
            break;
        case TX_ONSET: //I don't know this type
        default:
            return 0 ;

    }
    return 1 ;
}