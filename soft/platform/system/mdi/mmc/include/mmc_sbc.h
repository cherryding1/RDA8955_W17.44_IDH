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




#ifndef MMC_SBC_H
#define MMC_SBC_H


#include "cs_types.h"

#include "mci.h"
//#include "l1audio.h"


#define SBC_ALLOCATION_SUPPORT      0x1 // alloc_method: loudness only



#define SBC_SYNCWORD 0x9C
#define SBC_BIT_POOL        32

/* sampling frequency */
#define SBC_FS_16   0x00
#define SBC_FS_32   0x01
#define SBC_FS_44   0x02
#define SBC_FS_48   0x03

/* nrof_blocks */
#define SBC_NB_4    0x00
#define SBC_NB_8    0x01
#define SBC_NB_12   0x02
#define SBC_NB_16   0x03

/* channel mode */
#define SBC_CM_MONO     0x00
#define SBC_CM_DUAL_CHANNEL 0x01
#define SBC_CM_STEREO       0x02
#define SBC_CM_JOINT_STEREO 0x03

#define MONO        0x00
#define DUAL_CHANNEL    0x01
#define STEREO      0x02
#define JOINT_STEREO    0x03

/* allocation mode */
#define SBC_AM_LOUDNESS     0x00
#define SBC_AM_SNR      0x01

#define LOUDNESS        0x00
#define SNR     0x01

#define PREFFERED_ALLOCATION_MODE       LOUDNESS

/* subbands */
#define SBC_SB_4    0x00
#define SBC_SB_8    0x01

#define SB_NUM      8







typedef struct
{
    UINT32 sampling_frequency;  /* in kHz */
    UINT32 blocks;
    UINT32 channels;
    UINT32 subbands;
    UINT32 channel_mode;
    UINT32 allocation_method;
    UINT32 bitpool;

    UINT8 sbc_frame_header_data;

} SBC_Frame_State;




typedef struct
{

    UINT32 Crc:8;
    UINT32 Bitpool: 8;
    UINT32 Subbands: 1; // fixed
    UINT32 Allocation_method: 1;
    UINT32 ChannelMode: 2;
    UINT32 Blocks: 2; // fixed
    UINT32 Sample_rate: 2; // fixed
    UINT32 Sync: 8; // fixed
} sbcHeaderStruct;



typedef struct
{

    SSHdl       ss_handle;
    sbcHeaderStruct  config_header;

    A2DP_Callback   callback;
    UINT16         aud_id;
//   Media_Format     aud_fmt;

    UINT32       current_bit_rate;
    UINT32       current_frame_size;

    SBC_Frame_State  frame_state;



    UINT32       total_payload_size;

    UINT8        min_bit_pool;
    UINT8        max_bit_pool;


} sbc_struct;

typedef struct
{
    A2DP_codec_struct  *a2dp_codec_hdl;
    sbc_struct  *sbc_ss_handle;
    A2DP_Callback callback;
} a2dp_sbc_strm_struct;



extern A2DP_codec_struct *SBC_Open( SSHdl **ss_handle,  A2DP_Callback pHandler,
                                    bt_a2dp_sbc_codec_cap_struct *sbc_config_data,
                                    UINT8 *buf,   UINT32 buf_len );

extern VOID SBC_Close( SSHdl **ss_handle );
extern void  get_sbc_ss_handle(SSHdl  **handle  );
#endif


