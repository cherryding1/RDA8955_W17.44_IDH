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



#include "wmaudio_type.h"



#define MAX_SAMPLES 2048        // maximum buffer size when retrieving PCM samples
#define STRING_SIZE 255         // default string size
#define WMADEC_VOC_INBUF_SIZE 4096      // i/o buffer size (reading in data from file)
#define WMADEC_VOC_PrevOutput_SIZE 4096

#define NUM_BARK_BAND 25
#define NBITS_PACKET_CNT    4

#define WMAB_TRUE 1
#define WMAB_FALSE 0

typedef void * tHWMAFileState;
//typedef unsigned long int U32;
typedef long int I32;
//typedef unsigned short U16;
typedef I32 Int;
typedef I32 Bool; // In Win32, boolean is an "int" == 4 bytes. Keep it this way to reduce problems.
typedef I32 WMARESULT;
// SUCCESS codes
static const WMARESULT WMA_OK               = 0x00000000;
//static const WMARESULT WMA_S_FALSE          = 0x00000001;
//static const WMARESULT WMA_S_BUFUNDERFLOW   = 0x00000002;
//static const WMARESULT WMA_S_NEWPACKET      = 0x00000003;
//static const WMARESULT WMA_S_NO_MORE_FRAME  = 0x00000004;
//static const WMARESULT WMA_S_DEFAULT        = 0x00000005;
//static const WMARESULT WMA_S_SWITCHCHMODE   = 0x00000006; // Internal error, corrected internally

// ERROR codes
//static const WMARESULT WMA_E_FAIL           = 0x80004005;
//static const WMARESULT WMA_E_OUTOFMEMORY    = 0x8007000E;
//static const WMARESULT WMA_E_INVALIDARG     = 0x80070057;
static const WMARESULT WMA_E_NOTSUPPORTED   = 0x80040000;
//static const WMARESULT WMA_E_LOSTPACKET     = 0x80040001;
//static const WMARESULT WMA_E_BROKEN_FRAME   = 0x80040002;//0x8402
//static const WMARESULT WMA_E_BUFFEROVERFLOW = 0x80040003;
//static const WMARESULT WMA_E_ONHOLD         = 0x80040004;
//ON_HOLD: the decoder can't continue operating because of sth, most
//       likely no more data for flushing, getbits or peekbits. all functions
//       stop *immediately*.

//static const WMARESULT WMA_E_BUFFERUNDERFLOW =0x80040005;
//static const WMARESULT WMA_E_INVALIDRECHDR  = 0x80040006;
//static const WMARESULT WMA_E_SYNCERROR      = 0x80040007;
//static const WMARESULT WMA_E_NOTIMPL        = 0x80040008;
//static const WMARESULT WMA_E_FLUSHOVERFLOW  = 0x80040009; // Too much input data provided to flush




int32  LOG2_VOC(uint32 i);
//tWMAFileStatus WMAFileDecodeCreate (tHWMAFileState* phstate);
//tWMAFileStatus WMAFileDecodeInit (tHWMAFileState phstate);
//tWMAFileStatus WMAFileDecodeClose (tHWMAFileState phstate);
//void* hal_VocGetPointer(int32 vocLocalAddr);

tWMAFileStatus WMAFileGetInfo (tHWMAFileState hstate);
tWMAFileStatus WMAFileDecodeInit (tHWMAFileState hstate);


