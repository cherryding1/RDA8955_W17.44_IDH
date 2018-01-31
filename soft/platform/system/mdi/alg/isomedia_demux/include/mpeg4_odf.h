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

#ifndef _GF_MPEG4_ODF_H_
#define _GF_MPEG4_ODF_H_

#include "list.h"
#include "bitstream.h"
#include "isomedia_dev.h"
#include "isomedia.h"

/***************************************
            SLConfig Tag
***************************************/
enum
{
    SLPredef_Null = 0x01,
    SLPredef_MP4 = 0x02,
    SLPredef_SkipSL = 0xF0
};


/***************************************
            Descriptors Tag
***************************************/
enum
{
    GF_ODF_OD_TAG           = 0x01,
    GF_ODF_IOD_TAG          = 0x02,
    GF_ODF_ESD_TAG          = 0x03,
    GF_ODF_DCD_TAG          = 0x04,
    GF_ODF_DSI_TAG          = 0x05,
    GF_ODF_SLC_TAG          = 0x06,
    GF_ODF_CI_TAG           = 0x07,
    GF_ODF_SCI_TAG          = 0x08,
    GF_ODF_IPI_PTR_TAG      = 0x09,
    GF_ODF_IPMP_PTR_TAG     = 0x0A,
    GF_ODF_IPMP_TAG         = 0x0B,
    GF_ODF_QOS_TAG          = 0x0C,
    GF_ODF_REG_TAG          = 0x0D,

    /*FILE FORMAT RESERVED IDs - NEVER CREATE / USE THESE DESCRIPTORS*/
    GF_ODF_ESD_INC_TAG      = 0x0E,
    GF_ODF_ESD_REF_TAG      = 0x0F,
    GF_ODF_ISOM_IOD_TAG     = 0x10,
    GF_ODF_ISOM_OD_TAG      = 0x11,
    GF_ODF_ISOM_IPI_PTR_TAG = 0x12,
    /*END FILE FORMAT RESERVED*/

    GF_ODF_EXT_PL_TAG       = 0x13,
    GF_ODF_PL_IDX_TAG       = 0x14,

    GF_ODF_ISO_BEGIN_TAG    = 0x15,
    GF_ODF_ISO_END_TAG      = 0x3F,

    GF_ODF_CC_TAG           = 0x40,
    GF_ODF_KW_TAG           = 0x41,
    GF_ODF_RATING_TAG       = 0x42,
    GF_ODF_LANG_TAG         = 0x43,
    GF_ODF_SHORT_TEXT_TAG   = 0x44,
    GF_ODF_TEXT_TAG         = 0x45,
    GF_ODF_CC_NAME_TAG      = 0x46,
    GF_ODF_CC_DATE_TAG      = 0x47,
    GF_ODF_OCI_NAME_TAG     = 0x48,
    GF_ODF_OCI_DATE_TAG     = 0x49,
    GF_ODF_SMPTE_TAG        = 0x4A,

    GF_ODF_SEGMENT_TAG      = 0x4B,
    GF_ODF_MEDIATIME_TAG    = 0x4C,

    GF_ODF_IPMP_TL_TAG      = 0x60,
    GF_ODF_IPMP_TOOL_TAG    = 0x61,

    GF_ODF_ISO_RES_BEGIN_TAG    = 0x62,
    GF_ODF_ISO_RES_END_TAG      = 0xBF,

    GF_ODF_USER_BEGIN_TAG   = 0xC0,

    /*internal descriptor for mux input description*/
    GF_ODF_MUXINFO_TAG      = GF_ODF_USER_BEGIN_TAG,
    /*internal descriptor for bifs config input description*/
    GF_ODF_BIFS_CFG_TAG     = GF_ODF_USER_BEGIN_TAG + 1,
    /*internal descriptor for UI config input description*/
    GF_ODF_UI_CFG_TAG       = GF_ODF_USER_BEGIN_TAG + 2,
    /*internal descriptor for TextConfig description*/
    GF_ODF_TEXT_CFG_TAG     = GF_ODF_USER_BEGIN_TAG + 3,
    GF_ODF_TX3G_TAG         = GF_ODF_USER_BEGIN_TAG + 4,


    GF_ODF_USER_END_TAG     = 0xFE,


    GF_ODF_OCI_BEGIN_TAG    = 0x40,
    GF_ODF_OCI_END_TAG      = (GF_ODF_ISO_RES_BEGIN_TAG - 1),

    GF_ODF_EXT_BEGIN_TAG    = 0x80,
    GF_ODF_EXT_END_TAG      = 0xFE,
};
/*
typedef struct
{
    uint8 configurationVersion;
    uint8 AVCProfileIndication;
    uint8 profile_compatibility;
    uint8 AVCLevelIndication;
    uint8 nal_unit_size;

    GF_List *sequenceParameterSets;
    GF_List *pictureParameterSets;
} GF_AVCConfig;
 */

#endif

#endif

