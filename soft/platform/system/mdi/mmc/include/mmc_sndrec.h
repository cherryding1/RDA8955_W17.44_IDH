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

#ifndef _MMC_SNDREC_H_
#define _MMC_SNDREC_H_
#include"cs_types.h"
#include"fs.h"
#include"mci.h"

#define SNDRECBUF_LEN (8000)
#define SNDRECBUF_LEN_AMR475 (900)
#define SNDRECBUF_LEN_AMR515 (1200)
#define SNDRECBUF_LEN_AMR59 (1200)
#define SNDRECBUF_LEN_AMR67 (750)
#define SNDRECBUF_LEN_AMR74 (750)
#define SNDRECBUF_LEN_AMR795 (750)
#define SNDRECBUF_LEN_AMR102 (1050)
#define SNDRECBUF_LEN_AMR122 (1200)


typedef enum
{
    SNDREC_DATA_PART_HALF,
    SNDREC_DATA_PART_END
} SNDREC_DATA_PART;

void sndRec_msgHandle(SNDREC_DATA_PART part);
void sndrec_halfHandler();
void sndrec_endHandler();

extern INT32 Mmc_sndRecStart(HANDLE fhd,U8 quality,mci_type_enum format, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);
extern INT32 Mmc_sndRecStop(void);
extern INT32 Mmc_sndRecPause(void);
extern INT32 Mmc_sndRecResume(void);
extern INT32 Mmc_FmRecStart(HANDLE fhd,mci_type_enum format);

//sound record and play
INT32 Mmc_SoundStartRecord(HANDLE fhd);
INT32 Mmc_SoundStopRecord(void);

#endif //_MMC_SNDREC_H_
