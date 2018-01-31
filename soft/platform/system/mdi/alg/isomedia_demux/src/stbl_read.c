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
#include "mcip_debug.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "isomedia_dev.h"

void GetGhostNum(GF_StscEntry *ent, uint32 EntryIndex, uint32 count, GF_SampleTableBox *stbl)
{
    GF_StscEntry *nextEnt;
    GF_ChunkOffsetBox *stco;
    GF_ChunkLargeOffsetBox *co64;
    uint32 ghostNum = 1;

    if (!ent->nextChunk)
    {
        if (EntryIndex+1 == count)
        {
            //not specified in the spec, what if the last sample to chunk is no written?
            if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO)
            {
                stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
                ghostNum = (stco->entryCount > ent->firstChunk) ? (1 + stco->entryCount - ent->firstChunk) : 1;
            }
            else
            {
                co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
                ghostNum = (co64->entryCount > ent->firstChunk) ? (1 + co64->entryCount - ent->firstChunk) : 1;
            }
        }
        else
        {
            //this is an unknown case due to edit mode...
            nextEnt = (GF_StscEntry*)gf_list_get(stbl->SampleToChunk->entryList, EntryIndex+1);
            ghostNum = nextEnt->firstChunk - ent->firstChunk;
        }
    }
    else
    {
        ghostNum = (ent->nextChunk > ent->firstChunk) ? (ent->nextChunk - ent->firstChunk) : 1;
    }
    stbl->SampleToChunk->ghostNumber = ghostNum;
}

//Get the DTS of a sample
GF_Err stbl_GetSampleDTS(GF_TimeToSampleBox *stts, uint32 SampleNumber, uint32 *DTS, uint32 *sampleCount, uint32 *sampleDelta)
{
    uint32 i, j, count;
    GF_SttsEntry *ent;

    (*DTS) = 0;
    if (!stts || !SampleNumber) return GF_BAD_PARAM;

    ent = NULL;
    //use our cache
    count = gf_list_count(stts->entryList);
    if (stts->r_FirstSampleInEntry
            && (stts->r_FirstSampleInEntry <= SampleNumber)
            //this is for read/write access
            && (stts->r_currentEntryIndex < count) )
    {

        i = stts->r_currentEntryIndex;
    }
    else
    {
        i = stts->r_currentEntryIndex = 0;
        stts->r_FirstSampleInEntry = 1;
        stts->r_CurrentDTS = 0;
    }

    for (; i < count; i++)
    {
        ent = (GF_SttsEntry*)gf_list_get(stts->entryList, i);

        //in our entry
        if (ent->sampleCount + stts->r_FirstSampleInEntry >= 1 + SampleNumber)
        {
            j = SampleNumber - stts->r_FirstSampleInEntry;
            goto found;
        }

        //update our cache
        stts->r_CurrentDTS += ent->sampleCount * ent->sampleDelta;
        stts->r_currentEntryIndex += 1;
        stts->r_FirstSampleInEntry += ent->sampleCount;
    }
//  if (SampleNumber >= stts->r_FirstSampleInEntry + ent->sampleCount) return GF_BAD_PARAM;

    //no ent, this is really weird. Let's assume the DTS is then what is written in the table
    if (!ent || (i == count)) (*DTS) = stts->r_CurrentDTS;
    *sampleCount= ent->sampleCount;
    *sampleDelta = ent->sampleDelta;

    return GF_OK;

found:
    (*DTS) = stts->r_CurrentDTS + j * ent->sampleDelta;
    *sampleCount= ent->sampleCount;
    *sampleDelta = ent->sampleDelta;


    if (stts->r_FirstSampleInEntry == 1)
        stts->r_FirstSampleInEntry = 1;



    return GF_OK;
}

//Get the DTS count . add shenh
uint32 stbl_GetSampleDTScount(GF_TimeToSampleBox *stts)
{
    uint32  count;
    if (!stts ) return 0;

    count = gf_list_count(stts->entryList);

    return count;
}

//Get the DTS Delta. add shenh
uint32 stbl_GetSampleDTSdelta(GF_TimeToSampleBox *stts, uint32 SampleNumber)
{
    uint32 i, j, count;
    GF_SttsEntry *ent;

    if (!stts || !SampleNumber) return 0;

    ent = NULL;
    //use our cache
    count = gf_list_count(stts->entryList);
    if (stts->r_FirstSampleInEntry
            && (stts->r_FirstSampleInEntry <= SampleNumber)
            //this is for read/write access
            && (stts->r_currentEntryIndex < count) )
    {

        i = stts->r_currentEntryIndex;
    }
    else
    {
        i = stts->r_currentEntryIndex = 0;
        stts->r_FirstSampleInEntry = 1;
        stts->r_CurrentDTS = 0;
    }

    for (; i < count; i++)
    {
        ent = (GF_SttsEntry*)gf_list_get(stts->entryList, i);

        //in our entry
        if (ent->sampleCount + stts->r_FirstSampleInEntry >= 1 + SampleNumber)
        {
            j = SampleNumber - stts->r_FirstSampleInEntry;
            goto found;
        }

        if(!ent)return 0;
        //update our cache
        stts->r_CurrentDTS += ent->sampleCount * ent->sampleDelta;
        stts->r_currentEntryIndex += 1;
        stts->r_FirstSampleInEntry += ent->sampleCount;
    }
//  if (SampleNumber >= stts->r_FirstSampleInEntry + ent->sampleCount) return GF_BAD_PARAM;

    //no ent, this is really weird. Let's assume the DTS is then what is written in the table
    if (i == count) return 0;

found:

    if (stts->r_FirstSampleInEntry == 1)
        stts->r_FirstSampleInEntry = 1;

    return ent->sampleDelta;
}



//Set the RAP flag of a sample
GF_Err stbl_GetSampleRAP(GF_SyncSampleBox *stss, uint32 SampleNumber, uint8 *IsRAP, uint32 *prevRAP, uint32 *nextRAP)
{
    uint32 i;

    if (prevRAP) *prevRAP = 0;
    if (nextRAP) *nextRAP = 0;

    (*IsRAP) = 0;
    if (!stss || !SampleNumber) return GF_BAD_PARAM;

    if (stss->r_LastSyncSample && (stss->r_LastSyncSample < SampleNumber) )
    {
        i = stss->r_LastSampleIndex;
    }
    else
    {
        i = 0;
    }
    for (; i < stss->entryCount; i++)
    {
        //get the entry
        if (stss->sampleNumbers[i] == SampleNumber)
        {
            //update the cache
            stss->r_LastSyncSample = SampleNumber;
            stss->r_LastSampleIndex = i;
            (*IsRAP) = 1;
        }
        else if (stss->sampleNumbers[i] > SampleNumber)
        {
            if (nextRAP) *nextRAP = stss->sampleNumbers[i];
            return GF_OK;
        }
        if (prevRAP) *prevRAP = stss->sampleNumbers[i];
    }
    return GF_OK;
}


//shenh add for dynamic index read 2008.5.13
GF_Err stbl_GetSampleSizeFile(GF_SampleSizeBox *stsz, GF_FileDataMap *ptr, uint32 SampleNumber, uint32 *Size)
{
    uint32 SampleNumberInbuf=0;
    uint32 SampleRemainInbuf=0;
    uint32 LastRealInbuf=0;
    uint32 SampleRemainInFile=0;
    uint32 i=0;
    uint8 *p=NULL;
    uint8 tmp=0;

    uint32 readSampleNum=0;
    uint32 *readBufPos=NULL;
    uint32 fileOffset=0;
    uint8 memmoveID=0;

    if((!stsz) ||(!ptr) ||  (!SampleNumber) || (!Size))return GF_BAD_PARAM;

    if(stsz->sampleSize)
    {
        *Size=stsz->sampleSize;
        return GF_OK;
    }


    LastRealInbuf=stsz->firstRealInBuf+stsz->countInBuf-1;

    if(((SampleNumber-1) <=LastRealInbuf) && ((SampleNumber-1) >=stsz->firstRealInBuf ))
    {
        SampleNumberInbuf=SampleNumber-stsz->firstRealInBuf -1;//transform

        (*Size) = stsz->sizes[SampleNumberInbuf ];

        if((SampleNumberInbuf+1)>=(INDEX_BUF_SIZE>>1))
        {
            SampleRemainInbuf=stsz->countInBuf-SampleNumberInbuf;
            SampleRemainInFile=stsz->sampleCount-(stsz->firstRealInBuf+stsz->countInBuf);

            if( SampleRemainInFile>=(INDEX_BUF_SIZE-SampleRemainInbuf))
            {
                memcpy(stsz->sizes , stsz->sizes +SampleNumberInbuf, SampleRemainInbuf<<2  );//move and hold current Sample
                memmoveID=1;

                readBufPos=stsz->sizes+SampleRemainInbuf;
                readSampleNum=INDEX_BUF_SIZE-SampleRemainInbuf;
                fileOffset=stsz->offsetInFile+((LastRealInbuf+1)<<2);
                //diag_printf("stbl_GetSampleSizeFile 1: SampleNumber %d firstRealInBuf %d countInBuf %d SampleRemainInFile %d\n",SampleNumber,stsz->firstRealInBuf,stsz->countInBuf,SampleRemainInFile);
            }
            else if(SampleRemainInFile!=0)
            {
                memcpy(stsz->sizes , stsz->sizes +SampleNumberInbuf, SampleRemainInbuf<<2  );//move and hold current Sample
                memmoveID=1;

                readBufPos=stsz->sizes+SampleRemainInbuf;
                readSampleNum=SampleRemainInFile;
                fileOffset=stsz->offsetInFile+((LastRealInbuf+1)<<2);
                //diag_printf("stbl_GetSampleSizeFile 2: SampleNumber %d firstRealInBuf %d countInBuf %d SampleRemainInFile %d\n",SampleNumber,stsz->firstRealInBuf,stsz->countInBuf,SampleRemainInFile);
            }

        }
    }
    else
    {
        SampleRemainInFile=stsz->sampleCount -SampleNumber +1;

        if( SampleRemainInFile >= INDEX_BUF_SIZE)
        {
            readBufPos=stsz->sizes;
            readSampleNum=INDEX_BUF_SIZE;
            fileOffset=stsz->offsetInFile+((SampleNumber-1)<<2);
            SampleRemainInbuf=0;
            //diag_printf("stbl_GetSampleSizeFile 3: SampleNumber %d firstRealInBuf %d countInBuf %d SampleRemainInFile %d\n",SampleNumber,stsz->firstRealInBuf,stsz->countInBuf,SampleRemainInFile);
        }
        else
        {
            readBufPos=stsz->sizes;
            readSampleNum=SampleRemainInFile;
            fileOffset=stsz->offsetInFile+((SampleNumber-1)<<2);
            SampleRemainInbuf=0;
            //diag_printf("stbl_GetSampleSizeFile 4: SampleNumber %d firstRealInBuf %d countInBuf %d SampleRemainInFile %d\n",SampleNumber,stsz->firstRealInBuf,stsz->countInBuf,SampleRemainInFile);
        }
    }

    if(readSampleNum!=0)
    {

        if(gf_isom_fdm_get_data(ptr, (int8 *)readBufPos, readSampleNum<<2 , fileOffset)!=(readSampleNum<<2))
            return GF_IO_ERR;

        for (i = 0; i < readSampleNum; i++)
        {
            p=(uint8 *)(readBufPos+i);
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            tmp=*(p+1);
            *(p+1)=*(p+2);
            *(p+2)=tmp;
        }
        stsz->firstRealInBuf=SampleNumber-1;
        stsz->countInBuf=SampleRemainInbuf+readSampleNum;
        (*Size) = stsz->sizes[0];
    }
    else if(memmoveID==1)
    {
        stsz->firstRealInBuf=SampleNumber-1;
        stsz->countInBuf=SampleRemainInbuf;
    }

    return GF_OK;
}

//shenh add for dynamic index read 2008.5.13
GF_Err stbl_GetChunkOffsetFile(GF_ChunkOffsetBox *stco, GF_FileDataMap *ptr, uint32 ChunkNumber, u64 *offset)
{
    uint32 SampleNumberInbuf=0;
    uint32 SampleRemainInbuf=0;
    uint32 LastRealInbuf=0;
    uint32 SampleRemainInFile=0;
    uint32 i=0;
    uint8 *p=NULL;
    uint8 tmp=0;
    uint32 readSampleNum=0;
    uint32 *readBufPos=NULL;
    uint32 fileOffset=0;
    uint8 memmoveID=0;

    if((!stco) ||(!ptr) || (!ChunkNumber) || (!offset) ) return GF_BAD_PARAM;

    LastRealInbuf=stco->firstRealInBuf+stco->countInBuf -1;

    if(((ChunkNumber-1) <= LastRealInbuf) && ((ChunkNumber-1) >= stco->firstRealInBuf ))
    {
        SampleNumberInbuf=ChunkNumber-stco->firstRealInBuf -1;//transform

        (*offset) = stco->offsets[SampleNumberInbuf ];

        if((SampleNumberInbuf+1)>=(INDEX_BUF_SIZE>>1))
        {
            SampleRemainInbuf=stco->countInBuf-SampleNumberInbuf;
            SampleRemainInFile=stco->entryCount -(stco->firstRealInBuf+stco->countInBuf);

            if( SampleRemainInFile >= (INDEX_BUF_SIZE-SampleRemainInbuf ))
            {
                memcpy(stco->offsets , stco->offsets+SampleNumberInbuf, SampleRemainInbuf<<2  );//move and hold current Sample
                memmoveID=1;

                readBufPos=stco->offsets +SampleRemainInbuf;
                readSampleNum=INDEX_BUF_SIZE -SampleRemainInbuf;
                fileOffset=stco->offsetInFile+((LastRealInbuf +1 )<<2);
            }
            else if(SampleRemainInFile !=0)
            {
                memcpy(stco->offsets , stco->offsets+SampleNumberInbuf, SampleRemainInbuf<<2  );//move and hold current Sample
                memmoveID=1;

                readBufPos=stco->offsets +SampleRemainInbuf;
                readSampleNum=SampleRemainInFile;
                fileOffset=stco->offsetInFile+((LastRealInbuf +1 )<<2);
            }
        }
    }
    else
    {
        SampleRemainInFile=stco->entryCount -ChunkNumber +1;

        if( SampleRemainInFile >=INDEX_BUF_SIZE)
        {
            readBufPos=stco->offsets;
            readSampleNum=INDEX_BUF_SIZE;
            fileOffset=stco->offsetInFile+((ChunkNumber-1)<<2);
            SampleRemainInbuf=0;
        }
        else
        {
            readBufPos=stco->offsets;
            readSampleNum=SampleRemainInFile;
            fileOffset=stco->offsetInFile+((ChunkNumber-1)<<2);
            SampleRemainInbuf=0;
        }

    }

    if(readSampleNum!=0)
    {
        if(gf_isom_fdm_get_data(ptr, (int8 *)readBufPos, readSampleNum<<2 , fileOffset)!=(readSampleNum<<2))
            return GF_IO_ERR;

        for (i = 0; i < readSampleNum; i++)
        {
            p=(uint8 *)(readBufPos+i);
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            tmp=*(p+1);
            *(p+1)=*(p+2);
            *(p+2)=tmp;
        }
        stco->firstRealInBuf=ChunkNumber-1;
        stco->countInBuf=SampleRemainInbuf+readSampleNum;
        (*offset) = stco->offsets[0];
    }
    else if(memmoveID==1)
    {
        stco->firstRealInBuf=ChunkNumber-1;
        stco->countInBuf=SampleRemainInbuf;
    }

    return GF_OK;
}

//uint32 maxT=0;
GF_Err stbl_GetSampleSize(GF_MediaInformationBox *mInfo /*GF_SampleSizeBox *stsz*/, uint32 SampleNumber, uint32 *Size)//shenh change
{
    //shenh add
    GF_SampleSizeBox *stsz;
    GF_FileDataMap *ptr;
    s32 res;
    if(!mInfo)return GF_BAD_PARAM;
    stsz=mInfo->sampleTable->SampleSize;

    ptr=(GF_FileDataMap *)mInfo->dataHandler;
    //uint32 SampleNumberInbuf;

    //add end
//uint32 time;
//time_measure_start();

    if (!stsz ||!mInfo->dataHandler || !SampleNumber || SampleNumber > stsz->sampleCount) return GF_BAD_PARAM;

    (*Size) = 0;

    if (stsz->sampleSize && (stsz->type != GF_ISOM_BOX_TYPE_STZ2))
    {
        (*Size) = stsz->sampleSize;
    }
    else
    {
        //(*Size) = stsz->sizes[SampleNumber - 1];shenh

        res=stbl_GetSampleSizeFile(stsz, ptr,  SampleNumber, Size);
        if(res!=GF_OK) return res;
        /*
        if((SampleNumber-1) <(stsz->firstRealInBuf+stsz->countInBuf) && (SampleNumber-1) >=stsz->firstRealInBuf )
        {
            SampleNumberInbuf=SampleNumber-stsz->firstRealInBuf;//transform

            (*Size) = stsz->sizes[SampleNumberInbuf - 1 ];

            if(SampleNumberInbuf>=(INDEX_BUF_SIZE>>1))
            {
                memcpy(stsz->sizes , stsz->sizes+SampleNumberInbuf, INDEX_BUF_SIZE-SampleNumberInbuf  );
                stsz->firstRealInBuf=SampleNumber;

                if( stsz->sampleCount-SampleNumber>=SampleNumberInbuf)
                {
                    res=gf_isom_fdm_get_data(ptr, (char *)(stsz->sizes+INDEX_BUF_SIZE-SampleNumberInbuf), SampleNumberInbuf*4 , stsz->offsetInFile+SampleNumber);
                    if(res!=SampleNumberInbuf*4) return GF_IO_ERR;
                }
                else
                {
                    res=gf_isom_fdm_get_data(ptr, (char *)(stsz->sizes+INDEX_BUF_SIZE-SampleNumberInbuf), (stsz->sampleCount-SampleNumber)*4 , stsz->offsetInFile+SampleNumber);
                    if(res!=(stsz->sampleCount-SampleNumber)*4) return GF_IO_ERR;
                }
                stsz->countInBuf=INDEX_BUF_SIZE-SampleNumberInbuf+(res>>2);
            }
        }
        else
        {
            if( stsz->sampleCount-SampleNumber>=INDEX_BUF_SIZE)
            {
                res=gf_isom_fdm_get_data(ptr, (char *)stsz->sizes, INDEX_BUF_SIZE*4 , stsz->offsetInFile+SampleNumber-1);
                if(res!=INDEX_BUF_SIZE*4) return GF_IO_ERR;
            }
            else
            {
                res=gf_isom_fdm_get_data(ptr, (char *)stsz->sizes, (stsz->sampleCount-SampleNumber+1)*4 , stsz->offsetInFile+SampleNumber-1);
                if(res!=(stsz->sampleCount-SampleNumber+1)*4) return GF_IO_ERR;
            }
            (*Size) = stsz->sizes[0 ];
            stsz->firstRealInBuf=SampleNumber-1;
            stsz->countInBuf=(res>>2);
        }
        */
    }

//time=time_measure_end();
//if(maxT<time) maxT=time;
//diag_printf("78M t=%d maxt=%d\n",time/78,maxT/78);
    return GF_OK;
}

GF_Err stbl_GetSampleInfos(GF_MediaInformationBox *mInfo/*GF_SampleTableBox *stbl*/, uint32 sampleNumber, u64 *offset, uint32 *chunkNumber, uint32 *descIndex, uint8 *isEdited)//shenh change
{
    GF_Err e;
    uint32 i, j, k, offsetInChunk, size, count;
    GF_ChunkOffsetBox *stco;
    //GF_ChunkLargeOffsetBox *co64;
    GF_StscEntry *ent;
    GF_SampleTableBox *stbl;//shenh add
    GF_FileDataMap *ptr;//shenh add

    if(!mInfo) return GF_BAD_PARAM;//shenh add
    stbl=mInfo->sampleTable;//shenh add
    ptr=(GF_FileDataMap *)mInfo->dataHandler;//shenh add

    (*offset) = 0;
    (*chunkNumber) = (*descIndex) = 0;
    //(*isEdited) = 0;
    if (!stbl || !sampleNumber) return GF_BAD_PARAM;

    count = gf_list_count(stbl->SampleToChunk->entryList);
    if (count==stbl->SampleSize->sampleCount)//每个size对应一个offset，即每个chunk一个sample
    {
        ent = gf_list_get(stbl->SampleToChunk->entryList, sampleNumber-1);
        if (!ent) return GF_BAD_PARAM;
        (*descIndex) = ent->sampleDescriptionIndex;
        (*chunkNumber) = sampleNumber;
        //(*isEdited) = ent->isEdited;
        if ( stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO)
        {
            stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
            //(*offset) = (u64) stco->offsets[sampleNumber - 1];//shenh
            e=stbl_GetChunkOffsetFile(stco,  ptr,  sampleNumber, offset);
            if(e) return e;
        }
        else
        {
            /*shenh
            co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
            (*offset) = co64->offsets[sampleNumber - 1];
            */
            diag_printf("WARNING!!! GF_ISOM_BOX_TYPE_CO64 GF_NOT_SUPPORTED\n");
            return GF_NOT_SUPPORTED;
        }
        return GF_OK;
    }

    //check our cache
    if (stbl->SampleToChunk->firstSampleInCurrentChunk &&
            (stbl->SampleToChunk->firstSampleInCurrentChunk < sampleNumber))
    {

        i = stbl->SampleToChunk->currentIndex;
        ent = gf_list_get(stbl->SampleToChunk->entryList, i);
        ent = stbl->SampleToChunk->currentEntry;
        GetGhostNum(ent, i, count, stbl);
        k = stbl->SampleToChunk->currentChunk;
    }
    else
    {
        i = 0;
        stbl->SampleToChunk->currentIndex = 0;
        stbl->SampleToChunk->currentChunk = 1;
        stbl->SampleToChunk->firstSampleInCurrentChunk = 1;
        ent = (GF_StscEntry*)gf_list_get(stbl->SampleToChunk->entryList, 0);
        stbl->SampleToChunk->currentEntry = ent;
        GetGhostNum(ent, 0, count, stbl);
        k = stbl->SampleToChunk->currentChunk;
    }

    //first get the chunk
    for (; i < count; i++)
    {
        //browse from the current chunk we're browsing from index 1
        for (; k <= stbl->SampleToChunk->ghostNumber; k++)
        {
            //browse all the samples in this chunk
            for (j = 0; j < ent->samplesPerChunk; j++)
            {
                //ok, this is our sample
                if (stbl->SampleToChunk->firstSampleInCurrentChunk + j == sampleNumber )
                    goto sample_found;
            }
            //nope, get to next chunk
            stbl->SampleToChunk->firstSampleInCurrentChunk += ent->samplesPerChunk;
            stbl->SampleToChunk->currentChunk ++;
        }
        //not in this entry, get the next entry if not the last one
        if (i+1 != count)
        {
            ent = (GF_StscEntry*)gf_list_get(stbl->SampleToChunk->entryList, i+1);
            //update the GhostNumber
            GetGhostNum(ent, i+1, count, stbl);
            //update the entry in our cache
            stbl->SampleToChunk->currentEntry = ent;
            stbl->SampleToChunk->currentIndex = i+1;
            stbl->SampleToChunk->currentChunk = 1;
            k = 1;
        }
    }
    //if we get here, gasp, the sample was not found
    return GF_ISOM_INVALID_FILE;

sample_found:

    (*descIndex) = ent->sampleDescriptionIndex;
    (*chunkNumber) = ent->firstChunk + stbl->SampleToChunk->currentChunk - 1;
    //(*isEdited) = ent->isEdited;

    //ok, get the size of all the previous sample
    offsetInChunk = 0;

    if(mInfo->lastFirstSample==stbl->SampleToChunk->firstSampleInCurrentChunk &&
            mInfo->lastSampleNumber<=sampleNumber)
    {
        for (i = mInfo->lastSampleNumber; i < sampleNumber; i++)
        {
            e = stbl_GetSampleSize(mInfo, i, &size);
            if (e) return e;
            offsetInChunk += size;
        }

        offsetInChunk+=mInfo->lastOffset;
        mInfo->lastOffset=offsetInChunk;
        mInfo->lastSampleNumber=sampleNumber;
    }
    else
    {
        //warning, firstSampleInChunk is at least 1 - not 0
        for (i = stbl->SampleToChunk->firstSampleInCurrentChunk; i < sampleNumber; i++)
        {
            //e = stbl_GetSampleSize(stbl->SampleSize, i, &size);//shenh
            e = stbl_GetSampleSize(mInfo, i, &size);
            if (e) return e;
            offsetInChunk += size;
        }

        mInfo->lastFirstSample=stbl->SampleToChunk->firstSampleInCurrentChunk;
        mInfo->lastOffset=offsetInChunk;
        mInfo->lastSampleNumber=sampleNumber;
    }

    //OK, that's the size of our offset in the chunk
    //now get the chunk
    if ( stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO)
    {
        stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
        if (stco->entryCount < (*chunkNumber) ) return GF_ISOM_INVALID_FILE;
        //(*offset) = (u64) stco->offsets[(*chunkNumber) - 1] + (u64) offsetInChunk;shenh

        e=stbl_GetChunkOffsetFile(stco,  ptr,  (*chunkNumber) , offset);
        if(e) return e;
        (*offset)+=(u64) offsetInChunk;

    }
    else
    {
        /*
        co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
        if (co64->entryCount < (*chunkNumber) ) return GF_ISOM_INVALID_FILE;
        (*offset) = co64->offsets[(*chunkNumber) - 1] + (u64) offsetInChunk;
        */
        diag_printf("WARNING!!! GF_ISOM_BOX_TYPE_CO64 GF_NOT_SUPPORTED\n");
        return GF_NOT_SUPPORTED;
    }
    return GF_OK;
}

//shenh add.2008.10.25
GF_Err stbl_GetSampleOffset(GF_MediaInformationBox *mInfo, uint32 sampleNumber, u64 *offset)
{
    GF_Err e;
    int32 res;
    uint32 chunkNumber;
    uint32 i, k, offsetInChunk, size, count;
    GF_ChunkOffsetBox *stco;
    GF_SampleToChunkBox *sampleToChunk;
    GF_StscEntry *ent;
    GF_SampleTableBox *stbl;//shenh add
    GF_FileDataMap *ptr;//shenh add

    if(!mInfo) return GF_BAD_PARAM;//shenh add
    stbl=mInfo->sampleTable;//shenh add
    ptr=(GF_FileDataMap *)mInfo->dataHandler;//shenh add
    sampleToChunk=stbl->SampleToChunk;

    (*offset) = 0;
    chunkNumber = 0;
    if (!stbl || !sampleToChunk || !sampleNumber) return GF_BAD_PARAM;

    count = (sampleToChunk->entryList)?sampleToChunk->entryList->entryCount:0;

    if (count==stbl->SampleSize->sampleCount)//每个size对应一个offset，即每个chunk一个sample
    {
        chunkNumber = sampleNumber;
        if ( stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO)
        {
            stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
            e=stbl_GetChunkOffsetFile(stco,  ptr,  sampleNumber, offset);
            if(e) return e;
        }
        else
        {
            diag_printf("WARNING!!! GF_ISOM_BOX_TYPE_CO64 GF_NOT_SUPPORTED\n");
            return GF_NOT_SUPPORTED;
        }
        return GF_OK;
    }

    //check our cache
    if (sampleToChunk->firstSampleInCurrentChunk &&
            (sampleToChunk->firstSampleInCurrentChunk < sampleNumber))
    {
        i = sampleToChunk->currentIndex;
        ent = sampleToChunk->currentEntry;
        GetGhostNum(ent, i, count, stbl);
        k = sampleToChunk->currentChunk;
    }
    else
    {
        i = 0;
        sampleToChunk->currentIndex = 0;
        sampleToChunk->currentChunk = 1;
        sampleToChunk->firstSampleInCurrentChunk = 1;
        ent = (GF_StscEntry*)gf_list_get(sampleToChunk->entryList, 0);
        sampleToChunk->currentEntry = ent;
        GetGhostNum(ent, 0, count, stbl);
        k = 1;
    }

    //first get the chunk
    for (; i < count; i++)
    {
        //browse from the current chunk we're browsing from index 1
        for (; k <= sampleToChunk->ghostNumber; k++)
        {
            res=sampleToChunk->firstSampleInCurrentChunk + ent->samplesPerChunk;
            if (res > sampleNumber )
                goto sample_found;
            //nope, get to next chunk
            sampleToChunk->firstSampleInCurrentChunk =res;
            sampleToChunk->currentChunk ++;
        }

        //not in this entry, get the next entry if not the last one
        if (i+1 != count)
        {
            ent = (GF_StscEntry*)gf_list_get(sampleToChunk->entryList, i+1);
            //update the GhostNumber
            GetGhostNum(ent, i+1, count, stbl);
            //update the entry in our cache
            sampleToChunk->currentEntry = ent;
            sampleToChunk->currentIndex = i+1;
            sampleToChunk->currentChunk = 1;
            k = 1;
        }
    }
    //if we get here, gasp, the sample was not found
    return GF_ISOM_INVALID_FILE;

sample_found:

    chunkNumber = ent->firstChunk + sampleToChunk->currentChunk - 1;

    //ok, get the size of all the previous sample
    offsetInChunk = 0;

    if(mInfo->lastFirstSample==stbl->SampleToChunk->firstSampleInCurrentChunk &&
            mInfo->lastSampleNumber<=sampleNumber)
    {
        if(mInfo->lastSampleNumber+ 1 == sampleNumber)
        {
            offsetInChunk += mInfo->lastSampleSize;
        }
        else
        {
            for (i = mInfo->lastSampleNumber; i < sampleNumber; i++)
            {
                e = stbl_GetSampleSize(mInfo, i, &size);
                if (e) return e;
                offsetInChunk += size;
            }
        }

        offsetInChunk+=mInfo->lastOffset;
        mInfo->lastOffset=offsetInChunk;
        mInfo->lastSampleNumber=sampleNumber;
    }
    else
    {
        //warning, firstSampleInChunk is at least 1 - not 0
        for (i = stbl->SampleToChunk->firstSampleInCurrentChunk; i < sampleNumber; i++)
        {
            //e = stbl_GetSampleSize(stbl->SampleSize, i, &size);//shenh
            e = stbl_GetSampleSize(mInfo, i, &size);
            if (e) return e;
            offsetInChunk += size;
        }

        mInfo->lastFirstSample=stbl->SampleToChunk->firstSampleInCurrentChunk;
        mInfo->lastOffset=offsetInChunk;
        mInfo->lastSampleNumber=sampleNumber;
    }
    //OK, that's the size of our offset in the chunk
    //now get the chunk
    if ( stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO)
    {
        stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
        if (stco->entryCount < chunkNumber ) return GF_ISOM_INVALID_FILE;

        e=stbl_GetChunkOffsetFile(stco,  ptr,  chunkNumber , offset);
        if(e) return e;
        (*offset)+=(u64) offsetInChunk;

    }
    else
    {
        diag_printf("WARNING!!! GF_ISOM_BOX_TYPE_CO64 GF_NOT_SUPPORTED\n");
        return GF_NOT_SUPPORTED;
    }
    return GF_OK;
}


GF_Err Media_GetSampleDesc(GF_MediaBox *mdia, uint32 SampleDescIndex, GF_SampleEntryBox **out_entry, uint32 *dataRefIndex)
{
    GF_SampleDescriptionBox *stsd;
    GF_SampleEntryBox *entry = NULL;

    if (!mdia) return GF_ISOM_INVALID_FILE;

    stsd = mdia->information->sampleTable->SampleDescription;
    if (!stsd) return GF_ISOM_INVALID_FILE;
    if (!SampleDescIndex || (SampleDescIndex > gf_list_count(stsd->boxList)) ) return GF_BAD_PARAM;

    entry = (GF_SampleEntryBox*)gf_list_get(stsd->boxList, SampleDescIndex - 1);
    if (!entry) return GF_ISOM_INVALID_FILE;

    if (out_entry) *out_entry = entry;
    if (dataRefIndex) *dataRefIndex = entry->dataReferenceIndex;
    return GF_OK;
}

#endif

