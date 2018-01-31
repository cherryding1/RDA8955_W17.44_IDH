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


#define MIN_OBJECT_SIZE     24
#define DATA_OBJECT_SIZE    50
#define WMA_MAX_DATA_REQUESTED  128

#define GetUnalignedWord( pb, w )   (w) = *( WORD*)(pb);

#define GetUnalignedDword( pb, dw ) (dw) = *( WORD*)(pb); (dw)|= ((DWORD)*(( WORD*)(pb)+1))<<16;

#define GetUnalignedQword( pb, qw ) (qw) = *( QWORD*)(pb);

//#define GetUnalignedDouble( pb, d ) (d) = *( double*)(pb);

#define GetUnalignedWordEx( pb, w )     GetUnalignedWord( pb, w ); (pb) += sizeof(WORD);
#define GetUnalignedDwordEx( pb, dw )   GetUnalignedDword( pb, dw ); (pb) += sizeof(DWORD);
#define GetUnalignedQwordEx( pb, qw )   GetUnalignedQword( pb, qw ); (pb) += sizeof(QWORD);
//#define GetUnalignedDoubleEx( pb, d )   GetUnalignedDouble( pb, d ); (pb) += sizeof(double);

#define LoadBYTE( b, p )    b = *(BYTE *)(p);  (p) += sizeof( BYTE )

#define LoadWORD( w, p )    GetUnalignedWordEx( p, w )
#define LoadDWORD( dw, p )  GetUnalignedDwordEx( p, dw )
#define LoadQWORD( qw, p )  GetUnalignedQwordEx( p, qw )

#define LoadGUID( g, p ) \
        { \
            LoadDWORD( (g).Data1, p ); \
            LoadWORD( (g).Data2, p ); \
            LoadWORD( (g).Data3, p ); \
            LoadBYTE( (g).Data4[0], p ); \
            LoadBYTE( (g).Data4[1], p ); \
            LoadBYTE( (g).Data4[2], p ); \
            LoadBYTE( (g).Data4[3], p ); \
            LoadBYTE( (g).Data4[4], p ); \
            LoadBYTE( (g).Data4[5], p ); \
            LoadBYTE( (g).Data4[6], p ); \
            LoadBYTE( (g).Data4[7], p ); \
        }

//

WMAERR WMA_LoadObjectHeader(tWMAFileHdrStateInternal *pInt, GUID *pObjectId, QWORD *pqwSize);
WMAERR WMA_LoadHeaderObject(tWMAFileHdrStateInternal *pInt, int isFull);
WMAERR WMA_LoadPropertiesObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize, int isFull);
WMAERR WMA_LoadAudioObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize, int isFull);
WMAERR WMA_LoadEncryptionObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize);
WMAERR WMA_LoadContentDescriptionObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize);

WMAERR WMA_ParseAsfHeader(tWMAFileHdrStateInternal *pInt, int isFull);


