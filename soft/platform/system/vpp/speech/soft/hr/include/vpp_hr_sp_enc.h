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

#ifndef __SP_ENC
#define __SP_ENC

#include "vpp_hr_typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                           Function Prototypes                           |
 |_________________________________________________________________________|
*/

void   vpp_hr_encode(HAL_SPEECH_PCM_HALF_BUF_T EncInput, HAL_SPEECH_ENC_OUT_T* EncOutput);
//void   speechEncoder(INT16 pswSpeechIn[], INT16 pswFrmCodes[]);

//INT32 error_free (UINT16* EncoderIn, UINT16* EncoderOut);
//INT32 error_free_initial(HAL_SPEECH_ENC_OUT_T EdIfaceIn, HAL_SPEECH_DEC_IN_T* EdIfaceOutput);
//INT32 error_free_initial (UINT16* EncoderIn, UINT16* EncoderOut);
//INT32 error_free_interface (UINT16* EncoderIn, UINT16* EncoderOut);
#endif
