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

#ifndef VIDEO_API_H
#define VIDEO_API_H

#include "cs_types.h"
//#include "mmc_video.h"
#include "mmc_isom_video.h"
#include "mci.h"
/*
typedef struct {
    uint8 padA;
    uint16 padB;
    int16   result;
    uint16  image_width;
    uint16  image_height;
    uint32  total_frame_num;
    uint32  total_time;
    uint32   current_time;
    uint16  seq_num;
    uint16 media_type;
}
mci_vid_file_ready_ind_struct;

typedef struct {
    uint8 padA;
    uint16 padB;
    uint16   result;
    uint16   seq_num;
}
mci_vid_play_finish_ind_struct;

typedef struct {
    uint8 padA;
    uint16 padB;
    uint16   result;
    uint16   seq_num;
}
mci_vid_seek_done_ind_struct;
*/
typedef struct
{
    /*set play mode*/
    uint16   zoom_width;
    uint16   zoom_height;
    int16   lcd_start_x;
    int16   lcd_start_y;
    int16 cutX;
    int16 cutY;
    int16 cutW;
    int16 cutH;
    uint16   rotate;//0=no, 1=90clockwise 2=270clockwise
} mci_vid_set_mode_ind_struct;


/*
LILY_VideoOpenFile

OutputPath: audio path
open_audio: 1=play audio, 0=no
fhd: file handle or buffer address.
file_mode: 0 file mode, 1 buffer mode
data_Len: data length in fhd buffer when file_mode==1
type: media type

sheen
*/
int32  LILY_VideoOpenFile (HANDLE fhd, uint8 file_mode, uint32 data_Len, int OutputPath,uint8 open_audio,mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32));

int32  LILY_VideoSeek (uint32 seektime, int32 time_mode, int16 startX, int16 startY);

int32  LILY_VideoPlay (uint16 lcd_start_x, uint16 lcd_start_y);

int32  LILY_VideoStop (void);

int32  LILY_VideoPause (void) ;

int32  LILY_VideoResume (void);

int32  LILY_VideoClose (void);

int32  LILY_VideoGetInfo (mci_vid_file_ready_ind_struct *pFileInfo);

int32  LILY_VideoSetInfo(mci_vid_set_mode_ind_struct *pSetMode);

#endif
