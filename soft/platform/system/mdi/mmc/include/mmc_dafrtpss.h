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




#ifndef MMC_DAFRTPSS_H
#define MMC_DAFRTPSS_H


#include "cs_types.h"

#include "mci.h"

extern A2DP_codec_struct *A2DP_DAF_Open( SSHdl **ss_handle, A2DP_Callback pHandler,
        bt_a2dp_mp3_codec_cap_struct *daf_config_data,
        UINT8 *buf, UINT32 buf_len);
extern VOID A2DP_DAF_Close( SSHdl **ss_handle );
INT32 SSHdl_GetFreeSpace(SSHdl *hdl);
void SSHdl_GetWriteBuffer(SSHdl *hdl, UINT8 **buf, INT32 *buf_len);
void SSHdl_ShiftWritePointer(SSHdl *hdl, INT32 shamt);
INT32 SSHdl_GetDataCount(SSHdl *hdl);
void SSHdl_SniffNBytes(SSHdl *hdl, UINT32 offset, UINT32 N, UINT8 *dest);
UINT32 GET_UINT32(UINT8 *ptr);
void SSHdl_GetReadBuffer(SSHdl *hdl, UINT8 **buf, INT32 *buf_len);
void SSHdl_ShiftReadPointer(SSHdl *hdl, INT32 shamt);
#endif


