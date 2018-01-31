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

#include "wmaudio_parse.h"

#define WMA_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
          const GUID name; \
          const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }


#define WMA_IsEqualGUID(rguid1, rguid2) (!my_memcmp((void *)rguid1, (void *)rguid2))

int  my_memcmp(void *rguid1, void *rguid2)
{
    int8 *cmp_p1,*cmp_p2;
    int cmp_count;

    cmp_p1=(int8 *)(rguid1);
    cmp_p2=(int8 *)(rguid2);

    cmp_count=  sizeof(GUID);
    while((*cmp_p1==*cmp_p2)&&(--cmp_count)&&(*cmp_p1!='\0')&&(*cmp_p2!='\0'))
    {
        cmp_p1++;
        cmp_p2++;
    }
    if((*cmp_p1=='\0')||(*cmp_p2=='\0')||(cmp_count==0))
        return 0;
    else
        return 1;
}

/***************************************************************************/



WMA_DEFINE_GUID(CLSID_CAsfHeaderObjectV0,
                0x75b22630,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

WMA_DEFINE_GUID(CLSID_CAsfPropertiesObjectV2,
                0x8cabdca1, 0xa947, 0x11cf, 0x8e, 0xe4, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

WMA_DEFINE_GUID(CLSID_CAsfStreamPropertiesObjectV1,
                0xb7dc0791, 0xa9b7, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

WMA_DEFINE_GUID(CLSID_CAsfContentDescriptionObjectV0,
                0x75b22633,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

WMA_DEFINE_GUID(CLSID_CAsfDataObjectV0,
                0x75b22636,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c);

WMA_DEFINE_GUID(CLSID_AsfXNullErrorMaskingStrategy,
                0x20fb5700,0x5b55,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

WMA_DEFINE_GUID(CLSID_AsfXAcmAudioErrorMaskingStrategy,
                0xbfc3cd50,0x618f,0x11cf,0x8b,0xb2,0x00,0xaa,0x00,0xb4,0xe2,0x20);

WMA_DEFINE_GUID(CLSID_AsfXSignatureAudioErrorMaskingStrategy,
                0x49f1a440, 0x4ece, 0x11d0, 0xa3, 0xac, 0x0, 0xa0, 0xc9, 0x3, 0x48, 0xf6);

WMA_DEFINE_GUID(CLSID_AsfXStreamTypeAcmAudio,
                0xf8699e40,0x5b4d,0x11cf,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b);

WMA_DEFINE_GUID(CLSID_CAsfContentEncryptionObject,
                0x2211b3fb, 0xbd23, 0x11d2, 0xb4, 0xb7, 0x0, 0xa0, 0xc9, 0x55, 0xfc, 0x6e);

WMA_DEFINE_GUID( CLSID_CAsfExtendedContentDescObject,
                 0xd2d0a440, 0xe307, 0x11d2, 0x97, 0xf0, 0x0, 0xa0, 0xc9, 0x5e, 0xa8, 0x50);

WMA_DEFINE_GUID(CLSID_CAsfMarkerObjectV0,
                0xf487cd01, 0xa951, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65);

WMA_DEFINE_GUID( CLSID_CAsfLicenseStoreObject,
                 0x8fa7857b, 0xddc0, 0x11d3, 0xb6, 0xbd, 0x0, 0xc0, 0x4f, 0x61, 0xd, 0x62);

WMA_DEFINE_GUID( CLSID_CAsfStreamPropertiesObjectV2,
                 0x818d0d47, 0x43, 0x4451, 0x92, 0x37, 0xda, 0xef, 0xbc, 0x48, 0x4f, 0x50);

WMA_DEFINE_GUID( CLSID_AsfXStreamTypeDRMAuxData,
                 0xd5c82c81, 0x8264, 0x4abd, 0xa7, 0x14, 0xcd, 0x4f, 0x85, 0x54, 0xc7, 0x2e);


