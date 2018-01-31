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



#ifndef WIN32
//#include <itk.h> sheen
#endif

#include <stdlib.h>
#include <string.h>
//#include <fcntl.h>

#ifdef WIN32
#include <memory.h>
#include <io.h>
#else
//#include <unistd.h> sheen
#endif
#include <stdio.h>
#include "amrfile.h"
#include "frmbtord.h"
#include "amr_typedef.h"
#ifdef WIN32
#define O_BINARY _O_BINARY
#define O_RDONLY _O_RDONLY
#endif



/*
 *  Function        :   OpenAmrFile
 *--------------------------------------------------------------------------------
 *  Purpose         :   Open a amr file
 *  Entry           :   AmrName -- Amr file name
 *
 *  Return          :   AmrStruct -- AMRStruct*
 *                      The struct to collect amr file information
 *
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */

/*
#ifndef WIN32
#include <time.h>
#include <itk.h>
#define O_BINARY
 0
#endif
*/

/* The start string of AMR file : Norrow-band single-channel, Norrow-band single-channel,
   Wide-band single-channel,  Wide-band multi-channel */
const char* amrStr[4] = { "#!AMR\n" , "#!AMR-WB\n" , "#!AMR_MC1.0\n" , "#!AMR-WB_MC1.0\n" } ;
AMRStruct* OpenAmrFile(char* AmrName , int *bytesConsume, unsigned int* pid , int isIdFile , int offsetForIdFile )
{
    //char buf[15] ; sheen
    //unsigned long bytes ;

    //int       fd = -1 ; sheen
    AMRStruct* AmrStruct = 0  ;
    enum AmrFileType AmrType ;
    short n ;
    *bytesConsume= 0;//sheen

    //FILE* fp  = 0  ;
    //fp = fopen( AmrName , "rb" ) ;
    //if( !fp )
    //  goto error ;

    if(!AmrName)//sheen
    {
        diag_printf("AmrName invalid!");
        return 0;
    }

#if 0//sheen
    if( !isIdFile  )
#ifdef WIN32
        fd = open( AmrName , O_RDONLY|O_BINARY ) ;

#else
        fd = open( AmrName , O_RDONLY ) ;
    else
    {
        fd = itkOpenIdFile( AmrName ,pid ,  O_RDONLY ) ;
        lseek( fd , offsetForIdFile , SEEK_SET ) ;
    }
#endif

    if( fd == -1 )
        goto error ;
#endif//sheen

    //bytes = fread( buf , 1 , 15 , fp ) ;
    //bytes = read( fd , buf , 15) ; sheen

    AmrType =  AMR_NB_SC ;

    while ( AmrType < AMR_TYPE_NUM )
    {
        //if( memcmp( buf , amrStr[AmrType] ,  strlen( amrStr[AmrType] ) ) == 0 ) sheen
        if( memcmp( AmrName , amrStr[AmrType] ,  strlen( amrStr[AmrType] ) ) == 0 )
            break ;
        AmrType ++  ;
    }

    //if( AmrType >= AMR_TYPE_NUM )
    //goto error ;
    if( AmrType >= AMR_TYPE_NUM )
    {
        diag_printf("AmrType invalid!");
        return 0;//sheen
    }

    //fseek( fp , strlen( amrStr[AmrType] )  , SEEK_SET );
#if 0//sheen
    if( !isIdFile  )
        lseek( fd , strlen( amrStr[AmrType] )  , SEEK_SET );
    else
        lseek( fd , strlen( amrStr[AmrType] ) + offsetForIdFile  , SEEK_SET );
#endif//sheen

    if( !isIdFile  )
        *bytesConsume= strlen( amrStr[AmrType] );
    else
        *bytesConsume= strlen( amrStr[AmrType] )+ offsetForIdFile;

    AmrStruct = (AMRStruct*)mmc_MemMalloc( sizeof( AMRStruct ) ) ;
    //if( !AmrStruct )
    //goto error ;
    if( !AmrStruct )//sheen
    {
        diag_printf("AmrStruct malloc fail!");
        return 0;
    }

    memset( AmrStruct , 0 , sizeof( AMRStruct ) ) ;
    AmrStruct->AmrType = AmrType ;
    //AmrStruct->fp  = fp ;
    //AmrStruct->fd  = fd ; //sheen
    if( AmrType >=  AMR_NB_MC )
    {
        char *p= (char*)&(AmrStruct->nChannels);//sheen
        //fread( &(AmrStruct->nChannels ) , 2 , 1 , fp )  ;
        //fread( &(AmrStruct->nChannels ) , 2 , 1 , fp )  ;
        //read( fd , &(AmrStruct->nChannels ) , 2  )  ; sheen
        //read( fd , &(AmrStruct->nChannels ) , 2  )  ; sheen
        *p= *(AmrName +(*bytesConsume) +2);
        *(p+1)= *(AmrName +(*bytesConsume) +3);
        *bytesConsume +=4;

        AmrStruct->Channel_Struct = AmrStruct->Channel_Struct ;
        if( AmrStruct->nChannels < 4 )
            AmrStruct->Channel_Struct ++ ;
    }
    else
    {
        AmrStruct->nChannels = 1  ;
        AmrStruct->Channel_Struct = SINGLE_CHANNEL ;
    }
    AmrStruct->ChannelBuffer[0]  = AmrStruct->BufferPointer ;
    for( n = 1  ; n <  AmrStruct->nChannels ; n ++ )
        AmrStruct->ChannelBuffer[n] = AmrStruct->ChannelBuffer[n-1] + MAXBYTESOFONFRAME ;

    if( (AmrType == AMR_WB_SC ) || (AmrType == AMR_WB_MC) )
        AmrStruct->SampleRate = 16000 ;
    else
        AmrStruct->SampleRate = 8000 ;

    InitFrameBitOrder() ;
    return AmrStruct ;
#if 0//sheen
error:
    if( fd )
        close( fd ) ;
    //if( fp )
    //  fclose( fp ) ;
    if( !AmrStruct )
        free( AmrStruct ) ;
    return 0 ;
#endif
}




/*
 *  Function        :   CreateAmrFile
 *--------------------------------------------------------------------------------
 *  Purpose         :   Create a new  a amr file for writing
 *  Entry           :   AmrName -- Amr file name
 *
 *  Return          :   AmrStruct -- AMRStruct*
 *                      The struct to collect amr file information
 *
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */

/* The start string of AMR file : Norrow-band single-channel, Norrow-band single-channel,
   Wide-band single-channel,  Wide-band multi-channel */
//const char* amrStr[4] = { "#!AMR\n" , "#!AMR-WB\n" , "#!AMR_MC1.0\n" , "#!AMR-WB_MC1.0\n" } ;
#if 0//sheen
AMRStruct* CreateAmrFile(char* AmrName , enum AmrFileType amrfiletype , enum ChannelStruct channelstruct , int bCreateFile )
{
    FILE* fp = 0  ;
    unsigned long bytes ;
    AMRStruct* AmrStruct = 0  ;
    if( bCreateFile )
    {
        fp = fopen( AmrName , "wb" ) ;
        if( !fp )
            goto error  ;
    }

    AmrStruct =(AMRStruct*) malloc( sizeof( AMRStruct ) ) ;
    if( ! AmrStruct )
        goto error  ;
    memset( AmrStruct , 0 , sizeof( AMRStruct ) ) ;

    strcpy( (char*)AmrStruct->AmrFileHead ,  amrStr[ amrfiletype] ) ;
    AmrStruct->AmrFileHeadLength = strlen( amrStr[ amrfiletype] ) ;
    if ( (amrfiletype == AMR_NB_MC)  || (amrfiletype == AMR_WB_MC) )
    {
        bytes = channelstruct ;
        memcpy( AmrStruct->AmrFileHead + strlen( amrStr[ amrfiletype] ),   &bytes , 4  );
        AmrStruct->AmrFileHeadLength += 4 ;
    }

    if( bCreateFile )
    {
        bytes = fwrite( AmrStruct->AmrFileHead , 1 , AmrStruct->AmrFileHeadLength , fp ) ;
        if( bytes != AmrStruct->AmrFileHeadLength )
            goto error;
    }

    AmrStruct->AmrType =  amrfiletype ;
    AmrStruct->fp = fp ;
    AmrStruct->Channel_Struct = channelstruct ;
    AmrStruct->nChannels = channelstruct + 1 ;
    AmrStruct->ChannelBuffer[0] = AmrStruct->BufferPointer ;
    for( bytes = 1 ; bytes <   AmrStruct->nChannels  ; bytes ++ )
        AmrStruct->ChannelBuffer[bytes] = AmrStruct->ChannelBuffer[bytes-1] + MAXBYTESOFONFRAME ;

    InitFrameBitOrder();
    return  AmrStruct ;
error:
    if( fp )
        fclose( fp ) ;
    if( AmrStruct )
        free( AmrStruct ) ;
    return 0 ;

}
#endif

/*
 *  Function        :   ReadOneFrameBlock
 *--------------------------------------------------------------------------------
 *  Purpose         :   Get the bytes  of one frame block  from amr file
 *  Entry           :   AmrStruct -- AMRStruct , The struct to collect amr file information
 *
 *  Return          :   short
 *                      1, if success
 *                      0, if Fail
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */

short ReadOneFrameBlock( AMRStruct* AmrStruct, int *bytesLeft)
{
    unsigned long bytes ;
    int frametype  ;
    int  nF  ;

    for(  nF  = 0 ; nF < AmrStruct->nChannels ;  nF ++ )
    {
        //bytes  = fread( AmrStruct->ChannelBuffer[nF] , 1 , 1 , AmrStruct->fp );
#if 0//sheen
        bytes  = read( AmrStruct->fd , AmrStruct->ChannelBuffer[nF] , 1   );
        if( bytes <= 0 )
            return 0 ;
#endif
        if(*bytesLeft <=0)
            return 0;
        *(char*)(AmrStruct->ChannelBuffer[nF])= *(char*)(AmrStruct->fd);
        *bytesLeft -=1;
        AmrStruct->fd+=1;

        frametype  =  (AmrStruct->ChannelBuffer[nF][0] >> 3 )&15 ;
//      printf( " Read frametype = %d and  bit_Byte_Number = %d\n" , frametype , bit_Byte_Number[frametype][1] ) ;
        //bytes  = fread( &AmrStruct->ChannelBuffer[nF][1] , 1 , bit_Byte_Number[frametype][1] , AmrStruct->fp );
#if 0//sheen
        bytes  = read(  AmrStruct->fd ,  &AmrStruct->ChannelBuffer[nF][1] ,  bit_Byte_Number[frametype][1] );
        if( bytes != bit_Byte_Number[frametype][1] )
        {
//          printf( " Error to Read");
            return 0 ;
        }
#endif
        bytes= bit_Byte_Number[frametype][1];
        if(*bytesLeft < bytes)
            return 0;
        memcpy(&AmrStruct->ChannelBuffer[nF][1], (void*)AmrStruct->fd, bytes);
        *bytesLeft -= bytes;
        AmrStruct->fd+=bytes;
        if (frametype>7) //skip frame,mode>7
            return 0;

    }
    return 1 ;
}




/*
 *  Function        :   WriteOneFrameBlock
 *--------------------------------------------------------------------------------
 *  Purpose         :   Write the bytes  of one frame block  from amr file
 *  Entry           :   AmrStruct -- AMRStruct , The struct to collect amr file information
 *
 *  Return          :   short
 *                      1, if success
 *                      0, if Fail
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */
#if 0//sheen
short WriteOneFrameBlock( AMRStruct* AmrStruct  )
{
    unsigned long bytes ;
    int frametype  ;
    int  nF  ;

    for(  nF  = 0 ; nF < AmrStruct->nChannels ;  nF ++ )
    {
        bytes  = fwrite( AmrStruct->ChannelBuffer[nF] , 1 , 1 , AmrStruct->fp );
        if( bytes <= 0 )
            return 0 ;

        frametype  =  (AmrStruct->ChannelBuffer[nF][0] >> 3 )&15 ;
//      printf( " frametype = %d and  bit_Byte_Number = %d\n" , frametype , bit_Byte_Number[frametype][1] ) ;
        bytes  = fwrite( &AmrStruct->ChannelBuffer[nF][1] , 1 , bit_Byte_Number[frametype][1] , AmrStruct->fp );
        if( bytes != bit_Byte_Number[frametype][1] )
        {
//          printf( "Error to write\n"  ) ;
            return 0 ;
        }
    }
    return 1 ;
}
#endif



/*
 *  Function        :   CloseAmrFile
 *--------------------------------------------------------------------------------
 *  Purpose         :   Close amrfile
 *  Entry           :   AmrStruct -- AMRStruct , The struct to collect amr file information
 *
 *  Return          :   short
 *                      1, if success
 *                      0, if Fail
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */


short CloseAmrFile( AMRStruct* AmrStruct )
{
#if 0//sheen
    int ret ;
    if(AmrStruct->fp)
        ret = fclose( AmrStruct->fp ) ;
    if( AmrStruct->fd )
        ret = close( AmrStruct->fd ) ;
#endif

#ifdef SHEEN_VC_DEBUG
    free( AmrStruct ) ;
    //return !ret   ;
    return 0;
#else
    return 0;
#endif
}

