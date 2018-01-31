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

#ifndef _GF_ISOMEDIA_H_
#define _GF_ISOMEDIA_H_


#include "mpeg4_odf.h"

enum
{
    /*reserved, internal use in the lib. Indicates the track complies to MPEG-4 system
    specification, and the usual OD framework tools may be used*/
    GF_ISOM_SUBTYPE_MPEG4       = GF_FOUR_CHAR_INT( 'M', 'P', 'E', 'G' ),

    /*reserved, internal use in the lib. Indicates the track is of GF_ISOM_SUBTYPE_MPEG4
    but it is encrypted.*/
    GF_ISOM_SUBTYPE_MPEG4_CRYP  = GF_FOUR_CHAR_INT( 'E', 'N', 'C', 'M' ),

    /*AVC/H264 media type - not listed as an MPEG-4 type, ALTHOUGH this library automatically remaps
    GF_AVCConfig to MPEG-4 ESD*/
    GF_ISOM_SUBTYPE_AVC_H264        = GF_FOUR_CHAR_INT( 'a', 'v', 'c', '1' ),

    /*3GPP(2) extension subtypes*/
    GF_ISOM_SUBTYPE_3GP_H263        = GF_FOUR_CHAR_INT( 's', '2', '6', '3' ),
    GF_ISOM_SUBTYPE_3GP_JPEG        = GF_FOUR_CHAR_INT( 'j', 'p', 'e', 'g' ),
    GF_ISOM_SUBTYPE_3GP_AMR     = GF_FOUR_CHAR_INT( 's', 'a', 'm', 'r' ),
    GF_ISOM_SUBTYPE_3GP_AMR_WB  = GF_FOUR_CHAR_INT( 's', 'a', 'w', 'b' ),
    GF_ISOM_SUBTYPE_3GP_EVRC        = GF_FOUR_CHAR_INT( 's', 'e', 'v', 'c' ),
    GF_ISOM_SUBTYPE_3GP_QCELP   = GF_FOUR_CHAR_INT( 's', 'q', 'c', 'p' ),
    GF_ISOM_SUBTYPE_3GP_SMV     = GF_FOUR_CHAR_INT( 's', 's', 'm', 'v' )
};

#endif
#endif

