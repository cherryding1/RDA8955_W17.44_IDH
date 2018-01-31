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


























#ifndef BEGINHDR_H__
#define BEGINHDR_H__

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/rv89combo_c/cdeclib/beginhdr.h,v 1.3 2005/04/27 19:35:00 hubbe Exp $ */

/* This file provides a crude "pre-include" mechanism.  Anything declared */
/* here is guaranteed to be included into every source file in the project, */
/* with the caveats noted below. */
/* The "guarantee" depends on a programming practice whereby every ".h" */
/* file in the project begins with a #include of this file.  Said #include */
/* should occur after system includes (i.e., #include <....>), and before */
/* any other project includes (i.e., #include "..."). */
/* */
/* Note that it is not necessary to include this file in any .c or .cpp files. */
/* As a matter of good programming practice, and perhaps correctness, such */
/* files should #include at least one meaningful ".h" file, and will thus */
/* include this file indirectly. */
/* */
/* RV89Combo-specific notes: */
/* */
/* We lied when we said this file is included everywhere.  This file is */
/* rv89combo-specific, and is thus included below the RV line in the RealVideo/Pia */
/* architecture.  We do not include this file in the RealVideo (though possibly */
/* I could imagine using it to configure the RealVideo RealVideo, since that */
/* RealVideo is more closely coupled with the codec than the other RealVideos), */
/* nor in the performance counter library.  We also do not include this */
/* file in "interface" files, such as trommsg.h, which are distributed */
/* to third parties for use in calling the codec. */


#include "rvcfg.h"


#endif /* BEGINHDR_H__ */
