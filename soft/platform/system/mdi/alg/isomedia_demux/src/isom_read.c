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

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "isomedia_dev.h"
//#include "../include/isomedia.h"

#define CHECK_SAMPLETABLE {\
if (!trak) return 0; \
if (!trak->Media) return 0; \
if (!trak->Media->information) return 0; \
if (!trak->Media->information->sampleTable) return 0; }

uint32 gf_isom_get_track_count(GF_ISOFile *movie)
{
    if (!movie || !movie->moov) return 0;

    if (! movie->moov->trackList)
    {
        movie->LastError = GF_ISOM_INVALID_FILE;
        return 0;
    }
    return gf_list_count(movie->moov->trackList);
}

boolean IsMP4Description(uint32 entryType)
{
    switch (entryType)
    {
        case GF_ISOM_BOX_TYPE_MP4S:
        case GF_ISOM_BOX_TYPE_MP4A:
        case GF_ISOM_BOX_TYPE_MP4V:
            return 1;
        default:
            return 0;
    }
}

uint32 gf_isom_get_track_by_id(GF_ISOFile *the_file, uint32 trackID)
{
    GF_TrackBox *trak;
    uint32 count;
    uint32 i;
    if (the_file == NULL) return 0;

    count = gf_isom_get_track_count(the_file);
    if (!count) return 0;
    for (i = 0; i < count; i++)
    {
        trak = gf_isom_get_track_from_file(the_file, i+1);
        if (!trak) return 0;
        if (!trak->Header) return 0;
        if (trak->Header->trackID == trackID) return i+1;
    }
    return 0;
}

uint32 gf_isom_get_media_type(GF_ISOFile *movie, uint32 trackNumber)
{
    GF_TrackBox *trak;
    trak = gf_isom_get_track_from_file(movie, trackNumber);
    if (!trak) return GF_BAD_PARAM;
    if (!trak->Media) return GF_BAD_PARAM;
    if (!trak->Media->handler) return GF_BAD_PARAM;
    return trak->Media->handler->handlerType;
}

uint32 gf_isom_get_media_subtype(GF_ISOFile *the_file, uint32 trackNumber, uint32 DescriptionIndex)
{
    GF_TrackBox *trak;
    GF_Box *entry;
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    if (!trak || !DescriptionIndex) return 0;
    CHECK_SAMPLETABLE;
    if(!trak->Media->information->sampleTable->SampleDescription) return 0;
    entry = gf_list_get(trak->Media->information->sampleTable->SampleDescription->boxList, DescriptionIndex-1);
    if (!entry) return 0;

    //filter MPEG sub-types
    //if (IsMP4Description(entry->type)) { //change shenh
    //  return GF_ISOM_SUBTYPE_MPEG4;
    //}
    if (entry->type==GF_ISOM_BOX_TYPE_MP4V) //add shenh
    {
        return GF_ISOM_BOX_TYPE_MP4V;
    }
    else if (entry->type==GF_ISOM_BOX_TYPE_MP4A) //add shenh
    {
        return GF_ISOM_BOX_TYPE_MP4A;
    }
    else if (entry->type==GF_ISOM_BOX_TYPE_MP4S) //add shenh
    {
        return GF_ISOM_BOX_TYPE_MP4S;
    }
    else if(entry->type==GF_ISOM_SUBTYPE_3GP_H263)//add shenh
    {
        return GF_ISOM_SUBTYPE_3GP_H263;
    }
    else if(entry->type==GF_ISOM_SUBTYPE_3GP_AMR)//add shenh
    {
        return GF_ISOM_SUBTYPE_3GP_AMR;
    }
    else if(entry->type==GF_ISOM_BOX_TYPE_MP3)//add shenh
    {
        return GF_ISOM_BOX_TYPE_MP3;
    }
    else if(entry->type==GF_ISOM_BOX_TYPE_AVC1)//add shenh
    {
        return GF_ISOM_BOX_TYPE_AVC1;
    }

    if (entry->type == GF_ISOM_BOX_TYPE_GNRV)
    {
        return ((GF_GenericVisualSampleEntryBox *)entry)->EntryType;
    }
    else if (entry->type == GF_ISOM_BOX_TYPE_GNRA)
    {
        return ((GF_GenericAudioSampleEntryBox *)entry)->EntryType;
    }
    else if (entry->type == GF_ISOM_BOX_TYPE_GNRM)
    {
        return ((GF_GenericSampleEntryBox *)entry)->EntryType;
    }
    return entry->type;
}

uint32 gf_isom_get_sample_count(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak;
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    CHECK_SAMPLETABLE;
    if(!trak->Media->information->sampleTable->SampleSize) return 0;
    return trak->Media->information->sampleTable->SampleSize->sampleCount;
}

//add shenh for seek sync
uint32 gf_isom_get_total_KeySamp_count(GF_ISOFile *the_file, uint32 trackNumber )
{
    GF_TrackBox *trak;

    trak = gf_isom_get_track_from_file(the_file, trackNumber);

    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SyncSample) return 0;

    return trak->Media->information->sampleTable->SyncSample->entryCount;
}

//last keysample from sampleNumber for seek sync shenh
uint32 gf_isom_get_keySampleNumber(GF_TrackBox *trak, uint32 sampleNumber )
{
    int i;
    uint32 keySampleNumber=0;
    uint32 *ptr;

    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SyncSample)
    {
        //return 0;
        return sampleNumber;
    }

    if((trak->Media->information->sampleTable->SyncSample->entryCount>0) &&
            (trak->Media->information->sampleTable->SyncSample->sampleNumbers!=NULL))
    {
        ptr=(uint32 *)trak->Media->information->sampleTable->SyncSample->sampleNumbers;

        for (i = 0; i < trak->Media->information->sampleTable->SyncSample->entryCount; i++)
        {
            if(ptr[i]-1>=sampleNumber)
            {
                if((ptr[i]-1-sampleNumber)<=(sampleNumber-keySampleNumber))
                {
                    keySampleNumber=ptr[i]-1;
                    break;
                }
                else
                    break;
            }
            keySampleNumber=ptr[i]-1 ;
        }
    }
    else
    {
        keySampleNumber=sampleNumber;
    }

    return keySampleNumber;
}

//next keysample from sampleNumber for audio video sync shenh
uint32 gf_isom_get_nextKey(GF_TrackBox *trak, uint32 sampleNumber )
{
    int i;
    static uint32 lastI=0;
    uint32 keySampleNumber=0;
    uint32 *ptr;
    uint32 entryCount;

    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SyncSample) return 0;

    ptr=trak->Media->information->sampleTable->SyncSample->sampleNumbers;
    entryCount=trak->Media->information->sampleTable->SyncSample->entryCount;

    if((entryCount>0) && (ptr!=NULL))
    {
        if(lastI>=entryCount)
        {
            lastI=0;
        }
        else if(ptr[lastI]-1>sampleNumber)
        {
            lastI=0;
        }

        i = lastI;

        for (; i < entryCount; i++)
        {
            if(ptr[i]-1>=sampleNumber)
            {
                keySampleNumber=ptr[i]-1;
                break;
            }
            lastI=i;
        }
    }

    return keySampleNumber;
}

u64 gf_isom_get_sample_FPS(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak;
    u64 duration ;
    uint32 timescale ;
    u64 count ;
    u64 FPS ;
#ifdef SHEEN_VC_DEBUG
    __int64 temp;
#else
    unsigned long long temp;
#endif
    trak = gf_isom_get_track_from_file(the_file, trackNumber);

    CHECK_SAMPLETABLE;
    if (! trak->Media->mediaHeader) return 0;
    if (!trak->Media->information->sampleTable->SampleSize) return 0;

    duration = trak->Media->mediaHeader->duration;
    timescale = trak->Media->mediaHeader->timeScale;
    count = trak->Media->information->sampleTable->SampleSize->sampleCount;
    if(!duration)return 0;
#ifdef SHEEN_VC_DEBUG
    temp=(((__int64 )count<<8)*(__int64 )timescale+(__int64 )(duration>>1))/(__int64 )duration;
#else
    temp=(((unsigned long long )count<<8)*(unsigned long long )timescale+(unsigned long long )(duration>>1))/(unsigned long long )duration;
#endif
    FPS=(u64)temp;
    return FPS;
}
uint32  gf_isom_get_sample_height(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak=NULL;
    GF_VisualSampleEntryBox *ptr=NULL;
    uint32 slotnum=0;
    GF_Box *a=NULL;
#if 0 //change shenh
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    if (!trak) return 0;
    return trak->Header->height/65536;
#else
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SampleDescription) return 0;
    if (!trak->Media->information->sampleTable->SampleDescription->boxList) return 0;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return 0;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type== GF_ISOM_BOX_TYPE_MP4V || a->type==GF_ISOM_BOX_TYPE_S263 ||
                a->type== GF_ISOM_BOX_TYPE_AVC1 || a->type== GF_ISOM_BOX_TYPE_JPEG)
        {
            ptr=(GF_VisualSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return 0;
    return ptr->Height;
#endif
}
uint32  gf_isom_get_sample_width(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak=NULL;
    GF_VisualSampleEntryBox *ptr=NULL;
    uint32 slotnum=0;
    GF_Box *a=NULL;
#if 0 //change shenh
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    if (!trak) return 0;
    return trak->Header->width/65536;
#else
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SampleDescription) return 0;
    if (!trak->Media->information->sampleTable->SampleDescription->boxList) return 0;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return 0;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type== GF_ISOM_BOX_TYPE_MP4V|| a->type==GF_ISOM_BOX_TYPE_S263 ||
                a->type== GF_ISOM_BOX_TYPE_AVC1 || a->type== GF_ISOM_BOX_TYPE_JPEG)
        {
            ptr=(GF_VisualSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return 0;
    return ptr->Width;
#endif
}

//add shenh
uint32 gf_isom_get_sample_totaltime(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak;
    u64 duration ;
    uint32 timescale ;
    //u64 count ;
    uint32 totaltime ;
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    if (!trak) return 0;
    if (!trak->Media) return 0;
    if (!trak->Media->mediaHeader) return 0;
    duration = trak->Media->mediaHeader->duration;
    timescale = trak->Media->mediaHeader->timeScale;
    if(timescale==0) return 0;
    //totaltime = (duration/timescale)*1000;//ms
    totaltime = (duration*1000)/timescale;//ms
    return totaltime;
}
//

//add shenh for audio
uint16  gf_isom_get_sample_bitspersample(GF_ISOFile *the_file, uint32 trackNumber)
{
    uint32 slotnum=0;
    GF_TrackBox *trak=NULL;
    GF_Box *a=NULL;
    GF_AudioSampleEntryBox *ptr=NULL;

    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SampleDescription) return 0;
    if (!trak->Media->information->sampleTable->SampleDescription->boxList) return 0;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return 0;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type== GF_ISOM_BOX_TYPE_MP4A || a->type==GF_ISOM_BOX_TYPE_SAMR || a->type==GF_ISOM_BOX_TYPE_MP3)
        {
            ptr=(GF_AudioSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return 0;
    return ptr->bitspersample;
}

uint16  gf_isom_get_sample_channel_count(GF_ISOFile *the_file, uint32 trackNumber)
{
    uint32 slotnum=0;
    GF_TrackBox *trak=NULL;
    GF_Box *a=NULL;
    GF_AudioSampleEntryBox *ptr=NULL;
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SampleDescription) return 0;
    if (!trak->Media->information->sampleTable->SampleDescription->boxList) return 0;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return 0;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type== GF_ISOM_BOX_TYPE_MP4A || a->type==GF_ISOM_BOX_TYPE_SAMR || a->type==GF_ISOM_BOX_TYPE_MP3)
        {
            ptr=(GF_AudioSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return 0;
    return ptr->channel_count;
}

uint16  gf_isom_get_sample_samplerate(GF_ISOFile *the_file, uint32 trackNumber)
{
    uint32 slotnum=0;
    GF_TrackBox *trak=NULL;
    GF_Box *a=NULL;
    GF_AudioSampleEntryBox *ptr=NULL;
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    CHECK_SAMPLETABLE;
    if (!trak->Media->information->sampleTable->SampleDescription) return 0;
    if (!trak->Media->information->sampleTable->SampleDescription->boxList) return 0;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return 0;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type== GF_ISOM_BOX_TYPE_MP4A || a->type==GF_ISOM_BOX_TYPE_SAMR || a->type==GF_ISOM_BOX_TYPE_MP3)
        {
            ptr=(GF_AudioSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return 0;
    return ptr->samplerate_hi;
}


//
uint32 gf_isom_fdm_get_data(GF_FileDataMap *ptr, int8 *buffer, uint32 bufferLength, u64 fileOffset)
{
    uint32 bytesRead;

    if(!ptr) return 0;
    //can we seek till that point ???
    if (fileOffset > gf_bs_get_size(ptr->bs)) return 0;

    //ouch, we are not at the previous location, do a seek
    if (ptr->curPos != fileOffset)
    {
        if (gf_bs_seek(ptr->bs, fileOffset) != GF_OK) return 0;
        ptr->curPos = fileOffset;
    }
    //read our data.
    bytesRead = gf_bs_read_data(ptr->bs, (uint8*)buffer, bufferLength);
    //update our cache
    if (bytesRead == bufferLength)
    {
        ptr->curPos += bytesRead;
    }
    else
    {
        //rewind to original (if seek fails, return 0 cause this means:
        // 1- no support for seek on the platform
        // 2- corrupted file for the OS
        //fflush(ptr->stream);
        gf_bs_seek(ptr->bs, ptr->curPos);
    }
    ptr->last_acces_was_read = 1;
    return bytesRead;
}

uint32 gf_isom_datamap_get_data(GF_DataMap *map, int8 *buffer, uint32 bufferLength, u64 Offset)
{
    if (!map || !buffer) return 0;
    return gf_isom_fdm_get_data((GF_FileDataMap *)map, buffer, bufferLength, Offset);
}

GF_Err gf_isom_datamap_open(GF_MediaBox *mdia, uint32 dataRefIndex, uint8 Edit)
{
    GF_DataEntryBox *ent;
    GF_MediaInformationBox *minf;
    uint32 SelfCont;
    GF_Err e = GF_OK;
    if ((mdia == NULL) || (! mdia->information) || !dataRefIndex)
        return GF_ISOM_INVALID_MEDIA;

    minf = mdia->information;
    if(!minf) return GF_BAD_PARAM;
    if(!minf->dataInformation) return GF_BAD_PARAM;
    if(!minf->dataInformation->dref) return GF_BAD_PARAM;
    if(!minf) return GF_BAD_PARAM;

    if (dataRefIndex > gf_list_count(minf->dataInformation->dref->boxList))
        return GF_BAD_PARAM;

    ent = (GF_DataEntryBox*)gf_list_get(minf->dataInformation->dref->boxList, dataRefIndex - 1);
    if (ent == NULL) return GF_ISOM_INVALID_MEDIA;

    //if the current dataEntry is the desired one, and not self contained, return
    if ((minf->dataEntryIndex == dataRefIndex) && (ent->flags != 1))
    {
        return GF_OK;
    }

    SelfCont = 0;
    switch (ent->type)
    {
        case GF_ISOM_BOX_TYPE_URL:
        case GF_ISOM_BOX_TYPE_URN:
            if (ent->flags == 1) SelfCont = 1;
            break;
        default:
            SelfCont = 1;
            break;
    }
    //if self-contained, assign the input file
    if(!mdia->mediaTrack) return GF_BAD_PARAM;
    if(!mdia->mediaTrack->moov) return GF_BAD_PARAM;
    if(!mdia->mediaTrack->moov->mov) return GF_BAD_PARAM;
    if(!mdia->information->dataHandler) return GF_BAD_PARAM;
    if(!mdia->information->dataHandler->bs) return GF_BAD_PARAM;

    if (SelfCont)
    {
        minf->dataHandler = mdia->mediaTrack->moov->mov->movieFileMap;
    }
    else
    {
        e = gf_isom_datamap_new((HANDLE)mdia->information->dataHandler->bs->stream, mdia->information->dataHandler->bs->mp4_fs_mode, mdia->information->dataHandler->bs->mp4_fs_bufLen, (GF_DataMap **)& mdia->information->dataHandler);
        if (e) return (e==GF_URL_ERROR) ? GF_ISOM_UNKNOWN_DATA_REF : e;
    }
    //OK, set the data entry index
    minf->dataEntryIndex = dataRefIndex;
    return GF_OK;
}


GF_Err Media_GetSample(GF_MediaBox *mdia, uint32 sampleNumber, GF_ISOSample **samp, uint32 *sIDX, boolean no_data, u64 *out_offset)
{
    GF_Err e;
    uint32 bytesRead;
    uint32 dataRefIndex, chunkNumber;
    u64 offset, new_size;
    uint8 isEdited;
    GF_SampleEntryBox *entry;
    uint32 DTS_sampleCount;
    uint32 DTS_sampleDelta;

    if (!mdia ) return GF_BAD_PARAM;
    if (!mdia->information) return GF_BAD_PARAM;
    if (!mdia->information->sampleTable) return GF_BAD_PARAM;
    if (!mdia->information->sampleTable->SampleSize) return GF_BAD_PARAM;

    //OK, here we go....
    if (sampleNumber > mdia->information->sampleTable->SampleSize->sampleCount) return GF_BAD_PARAM;

    //get the DTS
    e = stbl_GetSampleDTS(mdia->information->sampleTable->TimeToSample, sampleNumber, &(*samp)->DTS,&DTS_sampleCount,&DTS_sampleDelta);
    if (e) return e;

    (*samp)->CTS_Offset = 0;

    //the size
    /* change shenh, size have got and input
    e = stbl_GetSampleSize(mdia->information->sampleTable->SampleSize, sampleNumber, &(*samp)->dataLength);
    if (e) return e;
    */

    //the RAP
    //get is this frame key frame(Random Access Point)? no use. shenh change
    /*
    if (mdia->information->sampleTable->SyncSample) {
        e = stbl_GetSampleRAP(mdia->information->sampleTable->SyncSample, sampleNumber, &(*samp)->IsRAP, NULL, NULL);
        if (e) return e;
    } else {
        //if no SyncSample, all samples are sync (cf spec)
        (*samp)->IsRAP = 1;
    }*/

    //the data info
    if (!sIDX && !no_data) return GF_BAD_PARAM;
    if (!sIDX && !out_offset) return GF_OK;

    (*sIDX) = 0;
    e = stbl_GetSampleInfos(mdia->information/*->sampleTable*/, sampleNumber, &offset, &chunkNumber, sIDX, &isEdited);
    if (e) return e;

    //then get the DataRef
    e = Media_GetSampleDesc(mdia, *sIDX, &entry, &dataRefIndex);
    if (e) return e;

    // Open the data handler - check our mode, don't reopen in read only if this is
    //the same entry. In other modes we have no choice because the main data map is
    //divided into the original and the edition files
    if (!mdia->information->dataHandler || (mdia->information->dataEntryIndex != dataRefIndex))
    {
        e = gf_isom_datamap_open(mdia, dataRefIndex, isEdited);
        if (e) return e;
    }

    if (out_offset) *out_offset = offset;
    if (no_data) return GF_OK;

    /*and finally get the data, include padding if needed*/
    //(*samp)->data = (char *) malloc(sizeof(char) * ( (*samp)->dataLength ) );

    //check if we can get the sample (make sure we have enougth data...)
    new_size = gf_bs_get_size(mdia->information->dataHandler->bs);

    bytesRead =gf_isom_datamap_get_data(mdia->information->dataHandler, (*samp)->data, (*samp)->dataLength, offset);
    //if bytesRead != sampleSize, we have an IO err
    if (bytesRead < (*samp)->dataLength)
    {
        return GF_IO_ERR;
    }
    mdia->BytesMissing = 0;
    return GF_OK;
}

//creates a new empty sample
#if 0
GF_ISOSample *gf_isom_sample_new()
{
    //diag_printf("mmc_MemMalloc .... gf_isom_sample_new \n");
    GF_ISOSample *tmp = (GF_ISOSample *) mmc_MemMalloc(sizeof(GF_ISOSample));
    if (!tmp) return NULL;
    tmp->CTS_Offset = tmp->DTS = 0;
    tmp->dataLength = 0;
    tmp->IsRAP = 0;
    tmp->data = NULL;
    return tmp;
}
#endif

GF_ISOSample *gf_isom_get_sample(GF_ISOFile *the_file, GF_TrackBox *trak/*uint32 trackNumber*/, uint32 sampleNumber, uint32 *sampleDescriptionIndex, GF_ISOSample *samp)
{
    GF_Err e;
    uint32 descIndex;
    //GF_TrackBox *trak;
    //GF_ISOSample *samp;
    //trak = gf_isom_get_track_from_file(the_file, trackNumber);shenh change

    if (!trak) return NULL;

    if (!sampleNumber) return NULL;
    //samp = gf_isom_sample_new();
    //if (!samp) return NULL;
    e = Media_GetSample(trak->Media, sampleNumber, &samp, &descIndex, 0, NULL);
    if (e)
    {
        return NULL;
    }
    if (sampleDescriptionIndex) *sampleDescriptionIndex = descIndex;

    return samp;
}

/************************
shenh add 2008.10.17
for multiple sample read

uint32 startSampleNumber,  //witch sample begin
int8 *outbuf,
uint32 bufLenght, //how many bytes need?
uint32 *gotBytes, //real read bytes.
uint32 *gotCount //real sample count got.
*************************/

int32 gf_isom_get_multSample(GF_MediaInformationBox *mInfo, uint32 startSampleNumber, int8 *outbuf, uint32 bufLenght, uint32 *gotBytes, uint32 *gotCount)//shenh change
{
    GF_Err e;
    int32 res;
    uint32 i,  size;
    u64 offset;
    uint32 blocksize;
    uint32 startOffset;
    GF_SampleSizeBox *stsz;
    GF_FileDataMap *filedatamap;
    int8 *pOutbuf;
    int8 endread;

    if(!mInfo || !outbuf ||!gotBytes || !gotCount)
        return -1;

    filedatamap=(GF_FileDataMap *)mInfo->dataHandler;
    if(!filedatamap)return -1;
    if(!mInfo->sampleTable)return -1;
    stsz=(GF_SampleSizeBox *)mInfo->sampleTable->SampleSize;
    if(!stsz)return -1;

    (*gotBytes) = 0;
    (*gotCount) = 0;
    pOutbuf=outbuf;
    i=startSampleNumber;
    offset=0;

    while(1)
    {
        startOffset=0;
        blocksize=0;
        endread=0;
        while(1)
        {
            e=stbl_GetSampleSizeFile(stsz, filedatamap,  i, &size);
            res=(int32)(pOutbuf+size+blocksize-outbuf);

            if((e!=GF_OK) || res>bufLenght)
            {
                endread=1;
                break;
            }

            if(!offset)
            {
                //e=stbl_GetSampleInfos(mInfo, i, &offset, &chunkNumber, &sIDX, &isEdited);
                e=stbl_GetSampleOffset(mInfo, i, &offset);
                if(e!=GF_OK)
                {
                    endread=1;
                    break;
                }
            }

            mInfo->lastSampleSize= size;

            if(!startOffset)
                startOffset=offset;
            else if((startOffset+blocksize)!=offset)
                break;

            *gotCount+=1;
            blocksize+=size;

            offset=0;

            i+=1;

            if(res==bufLenght)
            {
                endread=1;
                break;
            }
        }


        if(blocksize>0)
        {
            if((res=gf_isom_fdm_get_data(filedatamap, pOutbuf, blocksize, startOffset))!=blocksize)//can not only use FS_Seek+FS_Read
            {
                if(res>0)
                {
                    blocksize=res;
                    endread=1;
                }
                else
                    break;
            }

            pOutbuf+=blocksize;
            *gotBytes+=blocksize;

            if(endread)
                break;
        }
        else
            break;
    }

    return 0;
}


#if 0
int32 gf_isom_get_multSample(GF_MediaInformationBox *mInfo, uint32 startSampleNumber, int8 *outbuf, uint32 bufLenght, uint32 *gotBytes, uint32 *gotCount)//shenh change
{
    GF_Err e;
    uint32 i, j, size, listcount;
    u64 offset;
    uint16 frameNum;
    uint32 totalsize;
    GF_ChunkOffsetBox *stco;
    GF_SampleSizeBox *stsz;
    GF_List * list;
    GF_FileDataMap *filedatamap;//shenh add
    HANDLE *stream;
    GF_SampleToChunkBox *SampleToChunk;
    uint32 samplesPerChunk;
    int8 *pOutbuf;
    int8 endread;

    diag_printf("gf_isom_get_multSample");

    if(!mInfo)return -1;
    filedatamap=(GF_FileDataMap *)mInfo->dataHandler;
    if(!filedatamap)return -1;
    if(!mInfo->sampleTable)return -1;
    SampleToChunk=mInfo->sampleTable->SampleToChunk;
    if(!SampleToChunk)return -1;
    list=SampleToChunk->entryList;
    if(!list)return -1;
    stco=(GF_ChunkOffsetBox*)mInfo->sampleTable->ChunkOffset;
    if(!stco)return -1;
    stsz=mInfo->sampleTable->SampleSize;
    if(!stsz)return -1;
    stream=filedatamap->bs->stream;

    (*gotBytes) = 0;
    (*gotCount) = 0;

    listcount = list->entryCount;
    diag_printf("entryCount=%d ", list->entryCount);

    if (listcount==stsz->sampleCount)//每个size对应一个offset，即每个chunk一个sample
    {
        pOutbuf=outbuf;
        for(i=startSampleNumber-1; i<listcount; i++)
        {
            e=stbl_GetSampleSizeFile(stsz, filedatamap,  i, &size);
            if(e!=GF_OK)return -1;

            if(pOutbuf+size-outbuf>bufLenght)
                break;

            e=stbl_GetChunkOffsetFile(stco,  filedatamap,  i, &offset);
            if(e!=GF_OK)return -1;

            if(FS_Seek((INT32)stream, offset, FS_SEEK_SET)!=offset)
                return -1;

            if(FS_Read((INT32)stream, pOutbuf, size)!=size)
                return -1;
            pOutbuf+=size;
            (*gotBytes) += size;
            (*gotCount) += 1;
        }
    }
    else
    {
        diag_printf("currentChunk=%d ", SampleToChunk->currentChunk);
        diag_printf("startSampleNumber=%d firstSampleInCurrentChunk=%d", startSampleNumber, SampleToChunk->firstSampleInCurrentChunk);
        if (SampleToChunk->firstSampleInCurrentChunk &&
                (SampleToChunk->firstSampleInCurrentChunk <= startSampleNumber))
        {
            i=SampleToChunk->currentIndex;

            for (; i < listcount; i++)
            {
                SampleToChunk->currentEntry=list->slots[i];
                samplesPerChunk=SampleToChunk->currentEntry->samplesPerChunk;
                if(SampleToChunk->firstSampleInCurrentChunk+samplesPerChunk>startSampleNumber)
                    break;
                SampleToChunk->currentChunk++;
                SampleToChunk->currentIndex++;
                SampleToChunk->firstSampleInCurrentChunk+=samplesPerChunk;
            }
            diag_printf("1 currentChunk=%d ", SampleToChunk->currentChunk);
        }
        else
        {
            SampleToChunk->firstSampleInCurrentChunk=1;
            SampleToChunk->currentChunk=1;
            SampleToChunk->currentIndex=0;

            for (i=0; i < listcount; i++)
            {
                SampleToChunk->currentEntry=list->slots[i];
                samplesPerChunk=SampleToChunk->currentEntry->samplesPerChunk;
                if(SampleToChunk->firstSampleInCurrentChunk+samplesPerChunk>startSampleNumber)
                    break;
                SampleToChunk->currentChunk++;
                SampleToChunk->currentIndex++;
                SampleToChunk->firstSampleInCurrentChunk+=samplesPerChunk;
            }
            diag_printf("2 currentChunk=%d ", SampleToChunk->currentChunk);
        }

        pOutbuf=outbuf;
        i=SampleToChunk->currentIndex;
        diag_printf("currentIndex=%d listcount=%d", SampleToChunk->currentIndex, listcount);
        for (; i < listcount; i++)
        {
            diag_printf("i=%d ", i);
            SampleToChunk->currentEntry=list->slots[i];
            samplesPerChunk=SampleToChunk->currentEntry->samplesPerChunk;
            e=stbl_GetChunkOffsetFile(stco,  filedatamap,  SampleToChunk->currentChunk-1, &offset);
            if(e!=GF_OK)return -1;

            size=0;
            frameNum=0;
            totalsize=0;
            endread=0;
            for(j=0; j<samplesPerChunk; j++)
            {
                e=stbl_GetSampleSizeFile(stsz, filedatamap,  SampleToChunk->firstSampleInCurrentChunk+j, &size);
                if(e!=GF_OK)return -1;

                if(SampleToChunk->firstSampleInCurrentChunk+j<startSampleNumber)
                {
                    offset+=size;
                }
                else
                {
                    if(pOutbuf+totalsize +size -outbuf>bufLenght)
                    {
                        endread=1;
                        break;
                    }

                    totalsize+=size;

                    frameNum++;
                }
            }

            if(totalsize)
            {
                if(FS_Seek((INT32)stream, offset, FS_SEEK_SET)!=offset)
                    return -1;

                if(FS_Read((INT32)stream, pOutbuf, totalsize)!=totalsize)
                    return -1;
                pOutbuf+=totalsize;

                (*gotBytes) += totalsize;
                (*gotCount) += frameNum;
                diag_printf("totalsize=%d frameNum=%d", totalsize, frameNum);
                if(endread)break;
            }
            else
                break;

            SampleToChunk->currentChunk++;
            SampleToChunk->currentIndex++;
            SampleToChunk->firstSampleInCurrentChunk+=samplesPerChunk;

        }


    }

    return 0;
}
#endif

GF_Err Media_GetESD(GF_MediaBox *mdia, uint32 sampleDescIndex, GF_ESD **out_esd, boolean true_desc_only)
{
    GF_ESD *esd;
    GF_MPEGSampleEntryBox *entry = NULL;
    GF_ESDBox *ESDa;
    GF_SampleDescriptionBox *stsd = NULL;

    if(!mdia) return GF_BAD_PARAM;
    if(!mdia->information) return GF_BAD_PARAM;
    if(!mdia->information->sampleTable) return GF_BAD_PARAM;

    stsd = mdia->information->sampleTable->SampleDescription;
    *out_esd = NULL;
    if (!stsd || !stsd->boxList || !sampleDescIndex || (sampleDescIndex > gf_list_count(stsd->boxList)) )
        return GF_BAD_PARAM;

    esd = NULL;
    entry = (GF_MPEGSampleEntryBox*)gf_list_get(stsd->boxList, sampleDescIndex - 1);
    if (! entry) return GF_ISOM_INVALID_MEDIA;

    *out_esd = NULL;
    ESDa = NULL;
    switch (entry->type)
    {
        case GF_ISOM_BOX_TYPE_MP4V:
            ESDa = ((GF_MPEGVisualSampleEntryBox*)entry)->esd;
            if (ESDa) esd = (GF_ESD *) ESDa->desc;
            break;
        case GF_ISOM_BOX_TYPE_MP4A:
            ESDa = ((GF_MPEGAudioSampleEntryBox*)entry)->esd;
            if (ESDa) esd = (GF_ESD *) ESDa->desc;
            break;
        case GF_ISOM_BOX_TYPE_MP4S:
            ESDa = entry->esd;
            if (ESDa) esd = (GF_ESD *) ESDa->desc;
            break;
        default: return GF_ISOM_INVALID_MEDIA;
    }

    if (true_desc_only)
    {
        if (!esd) return GF_ISOM_INVALID_MEDIA;
        *out_esd = esd;
        return GF_OK;
    }
    else
    {
        if (!esd && !*out_esd) return GF_ISOM_INVALID_MEDIA;
        if (*out_esd == NULL) *out_esd = esd;
    }
    return GF_OK;
}

GF_DecoderConfig *gf_isom_get_decoder_config(GF_ISOFile *the_file, uint32 trackNumber, uint32 StreamDescriptionIndex)
{
    GF_TrackBox *trak;
    GF_ESD *esd;
    GF_Descriptor *decInfo;
    trak = gf_isom_get_track_from_file(the_file, trackNumber);
    if (!trak) return NULL;
    //get the ESD (possibly emulated)
    Media_GetESD(trak->Media, StreamDescriptionIndex, &esd, 0);
    if (!esd) return NULL;
    decInfo = (GF_Descriptor *) esd->decoderConfig;
    esd->decoderConfig = NULL;
    //gf_odf_desc_del((GF_Descriptor *) esd);
    return (GF_DecoderConfig *)decInfo;
}

//add shenh for h.263
h263DecSpecStrc * gf_isom_get_h263_spec(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak=NULL;
    GF_3gpH263SampleEntryBox *ptr=NULL;
    uint32 slotnum=0;
    GF_Box *a=NULL;
    if(!the_file)return NULL;
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    if(!trak) return NULL;
    if(!trak->Media) return NULL;
    if(!trak->Media->information) return NULL;
    if(!trak->Media->information->sampleTable) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription) return NULL;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription->boxList->slots) return NULL;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type==GF_ISOM_BOX_TYPE_S263)
        {
            ptr=(GF_3gpH263SampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return NULL;

    return ptr->h263Sp;
}

//add shenh for amr
amrDecSpecStrc * gf_isom_get_amr_spec(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak=NULL;
    GF_3gpAmrSampleEntryBox *ptr=NULL;
    uint32 slotnum=0;
    GF_Box *a=NULL;
    if(!the_file)return NULL;
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    if(!trak) return NULL;
    if(!trak->Media) return NULL;
    if(!trak->Media->information) return NULL;
    if(!trak->Media->information->sampleTable) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription) return NULL;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription->boxList->slots) return NULL;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type==GF_ISOM_BOX_TYPE_SAMR)
        {
            ptr=(GF_3gpAmrSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return NULL;
    return ptr->amrSp;
}

//add shenh for h.264
GF_AVCConfig * gf_isom_get_avc_config(GF_ISOFile *the_file, uint32 trackNumber)
{
    GF_TrackBox *trak=NULL;
    GF_AVCVisualSampleEntryBox *ptr=NULL;
    uint32 slotnum=0;
    GF_Box *a=NULL;
    if(!the_file)return NULL;
    trak=gf_isom_get_track(the_file->moov,  trackNumber);
    if(!trak) return NULL;
    if(!trak->Media) return NULL;
    if(!trak->Media->information) return NULL;
    if(!trak->Media->information->sampleTable) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription) return NULL;
    slotnum=gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList);
    if(!slotnum) return NULL;
    if(!trak->Media->information->sampleTable->SampleDescription->boxList->slots) return NULL;
    while(slotnum!=0)
    {
        slotnum-=1;
        a=(GF_Box*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
        if(a->type==GF_ISOM_BOX_TYPE_AVC1)
        {
            ptr=(GF_AVCVisualSampleEntryBox*)(trak->Media->information->sampleTable->SampleDescription->boxList->slots[slotnum]);
            break;
        }
    }
    if(!ptr) return NULL;

    return ptr->avcC;
}

//no use now shenh
#if 0
GF_GenericSampleDescription *gf_isom_get_generic_sample_description(GF_ISOFile *movie, uint32 trackNumber, uint32 StreamDescriptionIndex)
{
    GF_GenericVisualSampleEntryBox *entry;
    GF_GenericAudioSampleEntryBox *gena;
    GF_GenericSampleEntryBox *genm;
    GF_TrackBox *trak;
    GF_GenericSampleDescription *udesc;
    trak = gf_isom_get_track_from_file(movie, trackNumber);
    if (!trak || !StreamDescriptionIndex) return NULL;

    entry = gf_list_get(trak->Media->information->sampleTable->SampleDescription->boxList, StreamDescriptionIndex-1);
    //no entry or MPEG entry:
    if (!entry || IsMP4Description(entry->type) ) return NULL;
    //if we handle the description return false
    //switch (entry->type) {
    //case GF_ISOM_BOX_TYPE_GNRA:
    gena = (GF_GenericAudioSampleEntryBox *)entry;
    //diag_printf("mmc_MemMalloc .... gf_isom_get_generic_sample_description 1\n");
    udesc = mmc_MemMalloc(sizeof(GF_GenericSampleDescription));
    if(!udesc)return NULL;
    memset(udesc, 0, sizeof(GF_GenericSampleDescription));
    if (gena->EntryType == GF_ISOM_BOX_TYPE_UUID)
    {
        memcpy(udesc->UUID, gena->uuid, sizeof(bin128));
    }
    else
    {
        udesc->codec_tag = gena->EntryType;
    }
    udesc->version = gena->version;
    udesc->revision = gena->revision;
    udesc->vendor_code = gena->vendor;
    udesc->SampleRate = gena->samplerate_hi;
    udesc->bitsPerSample = gena->bitspersample;
    udesc->NumChannels = gena->channel_count;
    if (gena->data_size)
    {
        udesc->extension_buf_size = gena->data_size;
        //diag_printf("mmc_MemMalloc .... gf_isom_get_generic_sample_description 2 \n");
        udesc->extension_buf = mmc_MemMalloc(sizeof(int8) * gena->data_size);
        if(!udesc->extension_buf )return NULL;
        memcpy(udesc->extension_buf, gena->data, gena->data_size);
    }
    return udesc;
//  case GF_ISOM_BOX_TYPE_GNRM:
//      genm = (GF_GenericSampleEntryBox *)entry;
//      udesc = malloc(sizeof(GF_GenericSampleDescription));
//      memset(udesc, 0, sizeof(GF_GenericSampleDescription));
//      if (genm->EntryType == GF_ISOM_BOX_TYPE_UUID) {
//          memcpy(udesc->UUID, genm->uuid, sizeof(bin128));
//      } else {
//          udesc->codec_tag = genm->EntryType;
//      }
//      if (genm->data_size) {
//          udesc->extension_buf_size = genm->data_size;
//          udesc->extension_buf = malloc(sizeof(char) * genm->data_size);
//          memcpy(udesc->extension_buf, genm->data, genm->data_size);
//      }
//      return udesc;
//  }
//  return NULL;
}
#endif

#endif


