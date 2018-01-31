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


#include "wmaudio.h"
//#include "wmaudio_type.h"
#include "wmaudio_parse.h"
#include "wmaguids.h"


//extern unsigned char WMA_TEST[38568] ;
uint8 pDataBuffer[WMA_MAX_DATA_REQUESTED];

tWMA_U32 WMAFileCBGetData(tHWMAFileState state, tWMA_U32 offset,tWMA_U32 num_bytes, uint8 **ppData)
{
#if 1
    tWMA_U32 iBytesRead = 0;
//  unsigned char * myinputtest;
    tWMAFileHdrStateInternal *pInput =  state;

//   myinputtest=WMA_INPUT_BUFF;
//    myinputtest+=offset;

    FS_Seek(*(INT32 *)pInput->pInput,(INT64)offset,SEEK_SET);
    iBytesRead = FS_Read(*(INT32 *)pInput->pInput,pDataBuffer,num_bytes);
//  memcpy(pDataBuffer,myinputtest,num_bytes);


    iBytesRead = num_bytes;
    *ppData = pDataBuffer;

//  *ppData = pDataBuffer+offset;
    return iBytesRead;
#else
    tWMA_U32 iBytesRead = 0;
    uint8 * myinputtest, *s_cpypoint, *c_cpypoint;
    int i=0;
    unsigned int * bufset;
    tWMAFileHdrStateInternal *pInt =state;

    myinputtest=(*pInt).pInput;
    myinputtest+=offset;
    s_cpypoint=myinputtest;

    bufset=(unsigned int *)pDataBuffer;

    for(i=0; i<32; ++i)
        bufset[i]=0;

    c_cpypoint=pDataBuffer;

    for(i=0; i<(int)num_bytes; ++i)
        *c_cpypoint++ =*s_cpypoint++;

    iBytesRead = num_bytes;

    *ppData = pDataBuffer;
//  printf("p = %8x\n",*ppData);
    return iBytesRead;
#endif
}


DWORD HnsQWORDtoMsDWORD (QWORD qw)
{
    DWORD msLo, msHi;
//   float i =125.126;
//     i*=1265.22;
    msLo = qw.dwLo/10000;
//   msHi = (DWORD)(qw.dwHi  * (float) 429496.7296);
    msHi = (DWORD)(qw.dwHi * 429496+qw.dwHi * 7296/10000);
//    msHi=0;
    return  (msLo + msHi);
}



WMAERR WMA_LoadObjectHeader(tWMAFileHdrStateInternal *pInt,
                            GUID *pObjectId,
                            QWORD *pqwSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbWanted = MIN_OBJECT_SIZE;
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    pInt->currPacketOffset += cbActual;

    LoadGUID(*pObjectId, pData);
    LoadQWORD(*pqwSize, pData);

    return WMAERR_OK;
}



WMAERR WMA_LoadHeaderObject(tWMAFileHdrStateInternal *pInt,
                            int isFull)
{
    GUID objectId;
    QWORD qwSize;
    DWORD cHeaders;
    BYTE align;
    BYTE arch;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;
//  char *cmp_p1,*cmp_p2;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbWanted = MIN_OBJECT_SIZE + sizeof(DWORD) + 2*sizeof(BYTE);
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    pInt->currPacketOffset += cbActual;

    LoadGUID(objectId, pData);
    LoadQWORD(qwSize, pData);
    LoadDWORD(cHeaders, pData);
    LoadBYTE(align, pData);
    LoadBYTE(arch, pData);

    if(!WMA_IsEqualGUID(&objectId, &CLSID_CAsfHeaderObjectV0)|| align != 1|| arch != 2)
    {
        return WMAERR_INVALIDHEADER;
    }


    pInt->cbHeader = qwSize.dwLo;

    return WMAERR_OK;
}


/*************there is something wrong in this function*********************************/

WMAERR
WMA_LoadPropertiesObject(tWMAFileHdrStateInternal *pInt,
                         DWORD cbSize,
                         int isFull)
{
    GUID mmsId;
    QWORD qwTotalSize;
    QWORD qwCreateTime;
    QWORD qwPackets;
    QWORD qwPlayDuration;
    QWORD qwSendDuration;
    QWORD qwPreroll;
    DWORD dwFlags;
    DWORD dwMinPacketSize;
    DWORD dwMaxPacketSize;
    DWORD dwMaxBitrate;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }
    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = sizeof(GUID) + 6*sizeof(QWORD) + 4*sizeof(DWORD);
    if(cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }

    LoadGUID(mmsId, pData);
    LoadQWORD(qwTotalSize, pData);
    LoadQWORD(qwCreateTime, pData);
    LoadQWORD(qwPackets, pData);
    LoadQWORD(qwPlayDuration, pData);
    LoadQWORD(qwSendDuration, pData);
    LoadQWORD(qwPreroll, pData);
    LoadDWORD(dwFlags, pData);
    LoadDWORD(dwMinPacketSize, pData);
    LoadDWORD(dwMaxPacketSize, pData);
    LoadDWORD(dwMaxBitrate, pData);

    if(dwMinPacketSize != dwMaxPacketSize
            || (qwPackets.dwLo == 0 && qwPackets.dwHi == 0))
    {
        return WMAERR_FAIL;
    }

    pInt->cbPacketSize = dwMaxPacketSize;
    pInt->cPackets     = qwPackets.dwLo;
    pInt->msDuration   = HnsQWORDtoMsDWORD(qwPlayDuration);
    pInt->msPreroll    = qwPreroll.dwLo;


    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}

WMAERR
WMA_LoadAudioObject(tWMAFileHdrStateInternal *pInt,
                    DWORD cbSize,
                    int isFull)
{
    GUID streamType;
    GUID ecStrategy;
    QWORD qwOffset;
    DWORD cbTypeSpecific;
    DWORD cbErrConcealment;
    WORD wStreamNum;
    DWORD dwJunk;
    DWORD nBlocksPerObject;
    AsfXAcmAudioErrorMaskingData *pScramblingData;
    // WAVEFORMATEX *pFmt;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    WORD  tw;
    DWORD tdw;
    const BYTE *tp;


    DWORD cbObjectOffset = 0;



    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 2*sizeof(GUID) + sizeof(QWORD) + 3*sizeof(DWORD) + sizeof(WORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadGUID(streamType, pData);
    LoadGUID(ecStrategy, pData);
    LoadQWORD(qwOffset, pData);
    LoadDWORD(cbTypeSpecific, pData);
    LoadDWORD(cbErrConcealment, pData);
    LoadWORD(wStreamNum, pData);
    LoadDWORD(dwJunk, pData);

    wStreamNum &= 0x7F;

    if( !WMA_IsEqualGUID( &CLSID_AsfXStreamTypeAcmAudio, &streamType ) )
    {

        /* Skip over the rest */
        pInt->currPacketOffset += cbSize;
        return WMAERR_OK;
    }

    /* Type specific */
    pInt->wAudioStreamId = wStreamNum; //Amit

    if(cbTypeSpecific > 0)
    {
        cbWanted = cbTypeSpecific;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        tp = pData;
        LoadWORD (tw ,tp);

        switch(tw)
        {
            case WAVE_FORMAT_WMAUDIO2:

                if(cbTypeSpecific < 28 /*sizeof(WMAUDIO2WAVEFORMAT)*/)
                {
                    return WMAERR_FAIL;
                }

                pInt->nVersion         = 2;
                tp = pData +  4; LoadDWORD(tdw,tp);
                pInt->nSamplesPerSec   = tdw;
                tp = pData +  8; LoadDWORD(tdw,tp);
                pInt->nAvgBytesPerSec  = tdw;
                tp = pData + 12; LoadWORD (tw ,tp);
                pInt->nBlockAlign      = tw;
                tp = pData +  2; LoadWORD (tw ,tp);
                pInt->nChannels        = tw;
                tp = pData + 22; LoadWORD (tw ,tp);
                tp = pData + 18; LoadDWORD(tdw,tp);
                pInt->nSamplesPerBlock = tdw;
                pInt->nEncodeOpt       = tw;

                break;
            case WAVE_FORMAT_MSAUDIO1:
                if(cbTypeSpecific < 22 /*sizeof(MSAUDIO1WAVEFORMAT)*/)
                {
                    return WMAERR_FAIL;
                }

                pInt->nVersion         = 1;
                tp = pData +  4; LoadDWORD(tdw,tp);
                pInt->nSamplesPerSec   = tdw;
                tp = pData +  8; LoadDWORD(tdw,tp);
                pInt->nAvgBytesPerSec  = tdw;
                tp = pData + 12; LoadWORD (tw ,tp);
                pInt->nBlockAlign      = tw;
                tp = pData +  2; LoadWORD (tw ,tp);
                pInt->nChannels        = tw;
                tp = pData + 20; LoadWORD (tw,tp);
                pInt->nEncodeOpt       = tw;
                tp = pData + 18; LoadWORD (tw,tp);
                pInt->nSamplesPerBlock = tw;

                break;
            default:
                // unknown...
                return WMAERR_FAIL;
        }
    }

    /* Error concealment - this can get as big as 400!!! */

    if(cbErrConcealment > 0)
    {
        if(WMA_IsEqualGUID(&CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy))
        {
            cbWanted = 9;//sizeof(AsfXSignatureAudioErrorMaskingData);
        }
        else if(WMA_IsEqualGUID(&CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy))
        {
//            cbWanted = sizeof(AsfXAcmAudioErrorMaskingData);

            cbWanted = 8;
        }
        else
        {
            return WMAERR_FAIL;
        }

        if(cbObjectOffset + cbWanted > cbSize)
        {

            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {

            return WMAERR_BUFFERTOOSMALL;
        }

        cbObjectOffset += cbErrConcealment; // Skip over the rest - cbActual;

        if(WMA_IsEqualGUID(&CLSID_AsfXSignatureAudioErrorMaskingStrategy, &ecStrategy))
        {

            pInt->cbAudioSize = ((AsfXSignatureAudioErrorMaskingData *)pData)->maxObjectSize;


        }
        else if(WMA_IsEqualGUID(&CLSID_AsfXAcmAudioErrorMaskingStrategy, &ecStrategy))
        {

            pScramblingData = (AsfXAcmAudioErrorMaskingData *)pData;


            pInt->cbAudioSize = (DWORD)(pScramblingData->virtualPacketLen*pScramblingData->span);

//            pInt->cbAudioSize = (DWORD)((*(WORD*)(pData+1)) * ((WORD)(*pData)));


            if (pScramblingData->span >1)
                return WMAERR_FAIL;



        }
        else
        {

            return WMAERR_FAIL;
        }
    }

    nBlocksPerObject = pInt->cbAudioSize/pInt->nBlockAlign;
    pInt->cbAudioSize = nBlocksPerObject*pInt->nSamplesPerBlock*pInt->nChannels*2;

    /* use all */
    pInt->currPacketOffset += cbSize;
    return WMAERR_OK;
}


WMAERR WMA_LoadEncryptionObject(tWMAFileHdrStateInternal *pInt,
                                DWORD cbSize)
{
    DWORD cbBlock;

    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    BYTE *sData,*cDaTa;
    int i=0;


    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    pInt->cbSecretData = 0;


    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(pInt->cbSecretData, pData);

    if(pInt->cbSecretData)
    {
        cbWanted = pInt->cbSecretData;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

//       memcpy(pInt->pbSecretData, pData, (int)cbActual);

        cDaTa=pInt->pbSecretData;
        sData =pData;

        for(i=0; i<(int)cbActual; ++i)
            *cDaTa++ =*sData++;
//  iBytesRead = num_bytes;

    }


    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

//       memcpy(pInt->pbType, pData, (int)cbActual);

        cDaTa=pInt->pbType;
        sData =pData;

        for(i=0; i<(int)cbActual; ++i)
            *cDaTa++ =*sData++;


    }


    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

//        memcpy(pInt->pbKeyID, pData, (int)cbActual);
        cDaTa=pInt->pbKeyID;
        sData =pData;

        for(i=0; i<(int)cbActual; ++i)
            *cDaTa++ =*sData++;

    }


    cbWanted = sizeof(DWORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(cbBlock, pData);

    if(cbBlock)
    {
        cbWanted = cbBlock;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        while (cbWanted>0)
        {
            cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                        pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
            cbObjectOffset += cbActual;
            cbWanted -= cbActual;
        }

    }

    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}



WMAERR
WMA_LoadContentDescriptionObject(tWMAFileHdrStateInternal *pInt,
                                 DWORD cbSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 5*sizeof(WORD);
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadWORD(pInt->cbCDTitle, pData);
    LoadWORD(pInt->cbCDAuthor, pData);
    LoadWORD(pInt->cbCDCopyright, pData);
    LoadWORD(pInt->cbCDDescription, pData);
    LoadWORD(pInt->cbCDRating, pData);

    pInt->cbCDOffset = pInt->currPacketOffset + cbObjectOffset;


    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


WMAERR
WMA_LoadLicenseStoreObject(tWMAFileHdrStateInternal *pInt,
                           DWORD cbSize)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbDone;
    DWORD cbActual;

    DWORD m_dwFlag;

    DWORD cbObjectOffset = 0;


    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }

    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 8;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadDWORD(m_dwFlag, pData);
    LoadDWORD(pInt->m_dwLicenseLen, pData);

    cbWanted = pInt->m_dwLicenseLen;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_NOTDONE;
    }
//    pInt->m_pLicData = malloc(cbWanted);

    cbDone = 0;
    while (cbWanted)
    {
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
//        memcpy(pInt->m_pLicData + cbDone, pData, cbActual);

        cbObjectOffset += cbActual;
        cbWanted -= cbActual;
        cbDone   += cbActual;
        if(cbActual == 0)
        {
            return WMAERR_FAIL;
        }
    }

//    pInt->cbCDOffset = pInt->currPacketOffset + cbObjectOffset;


    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


WMAERR  WMA_LoadMarkerObject(tWMAFileHdrStateInternal *pInt, DWORD cbSize, int iIndex)
{
    BYTE *pData;
    DWORD cbWanted;
    DWORD cbActual;

    DWORD cbObjectOffset = 0;

    GUID    m_gMarkerStrategy;
    WORD    m_wAlignment;
    WORD    m_wNameLen;
    WORD    tw;
    unsigned int i, j;


    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }


    cbSize -= MIN_OBJECT_SIZE;

    cbWanted = 24;
    if(cbObjectOffset + cbWanted > cbSize)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
    if(cbActual != cbWanted)
    {
        return WMAERR_BUFFERTOOSMALL;
    }
    cbObjectOffset += cbActual;

    LoadGUID (m_gMarkerStrategy, pData);
    LoadDWORD(pInt->m_dwMarkerNum, pData);
    LoadWORD (m_wAlignment, pData);
    LoadWORD (m_wNameLen, pData);

    //pInt->m_pMarkers = (MarkerEntry *) malloc(sizeof(MarkerEntry)*pInt->m_dwMarkerNum);

//NQF+
    if( pInt->m_dwMarkerNum == 0)
    {

        pInt->currPacketOffset += cbSize;
        return WMAERR_OK;

    }
    else if ( iIndex < 0 )      //for query number of Markers
    {


        pInt->currPacketOffset += cbSize;
        return WMAERR_OK;

    }
    else if ( iIndex >= (int) pInt -> m_dwMarkerNum)
    {

        pInt->currPacketOffset += cbSize;
        return WMAERR_BUFFERTOOSMALL;  //NQF_temp
    }


    for (j = 0; j <= (unsigned int) iIndex; j++)
    {
        cbWanted = 18;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        LoadQWORD(pInt->m_pMarker->m_qOffset, pData);
        LoadQWORD(pInt->m_pMarker->m_qtime, pData);
        LoadWORD (pInt->m_pMarker->m_wEntryLen, pData);

        cbWanted = pInt->m_pMarker->m_wEntryLen;
        if(cbObjectOffset + cbWanted > cbSize)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbActual = WMAFileCBGetData((tHWMAFileState *)pInt,
                                    pInt->currPacketOffset + cbObjectOffset, cbWanted, &pData);
        if(cbActual != cbWanted)
        {
            return WMAERR_BUFFERTOOSMALL;
        }
        cbObjectOffset += cbActual;

        LoadDWORD(pInt->m_pMarker->m_dwSendTime, pData);
        LoadDWORD(pInt->m_pMarker->m_dwFlags, pData);
        LoadDWORD(pInt->m_pMarker->m_dwDescLen, pData);

        //pInt->m_pMarkers[j].m_pwDescName = (WORD *)malloc(sizeof(WORD)*pInt->m_pMarkers[j].m_dwDescLen);

        for (i=0; i<pInt->m_pMarker->m_dwDescLen && i < DESC_NAME_MAX_LENGTH; i++)
        {
            LoadWORD(tw, pData);
            pInt->m_pMarker->m_pwDescName[i] = tw;
        }
    }


    pInt->currPacketOffset += cbSize;

    return WMAERR_OK;
}


WMAERR WMA_ParseAsfHeader(tWMAFileHdrStateInternal *pInt,
                          int isFull)
{
    WMAERR wmarc;
    GUID objId;
    QWORD qwSize;

    if(pInt == NULL)
    {
        return WMAERR_INVALIDARG;
    }



    pInt->currPacketOffset = 0;

    pInt->cbCDOffset      = 0;
    pInt->cbCDTitle       = 0;
    pInt->cbCDAuthor      = 0;
    pInt->cbCDCopyright   = 0;
    pInt->cbCDDescription = 0;
    pInt->cbCDRating      = 0;

    wmarc = WMA_LoadHeaderObject(pInt, isFull);

    if(wmarc != WMAERR_OK)
    {
        return wmarc;
    }
    pInt->cbFirstPacketOffset = pInt->cbHeader += DATA_OBJECT_SIZE;



    while(pInt->currPacketOffset < pInt->cbFirstPacketOffset)
    {

        wmarc = WMA_LoadObjectHeader(pInt, &objId, &qwSize);


        if(wmarc != WMAERR_OK)
        {
            return wmarc;
        }

        if(WMA_IsEqualGUID(&CLSID_CAsfPropertiesObjectV2, &objId))
        {
            wmarc = WMA_LoadPropertiesObject(pInt, qwSize.dwLo,
                                             isFull);

            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if( WMA_IsEqualGUID( &CLSID_CAsfStreamPropertiesObjectV1, &objId )
                 || WMA_IsEqualGUID( &CLSID_CAsfStreamPropertiesObjectV2, &objId ) )
        {
            wmarc = WMA_LoadAudioObject(pInt, qwSize.dwLo,
                                        isFull);



            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfContentEncryptionObject, &objId))
        {
            wmarc = WMA_LoadEncryptionObject(pInt, qwSize.dwLo);

            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfContentDescriptionObjectV0, &objId))
        {
            wmarc = WMA_LoadContentDescriptionObject(pInt, qwSize.dwLo);

            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else if(WMA_IsEqualGUID(&CLSID_CAsfExtendedContentDescObject, &objId))
        {

            pInt->currPacketOffset += qwSize.dwLo - MIN_OBJECT_SIZE;
            wmarc = WMAERR_OK;

        }

        else if(WMA_IsEqualGUID(&CLSID_CAsfLicenseStoreObject, &objId))
        {
            wmarc = WMA_LoadLicenseStoreObject(pInt, qwSize.dwLo);

            if(wmarc != WMAERR_OK)
            {
                return wmarc;
            }
        }
        else
        {

            pInt->currPacketOffset += qwSize.dwLo - MIN_OBJECT_SIZE;
        }

    }


    return WMAERR_OK;
}



