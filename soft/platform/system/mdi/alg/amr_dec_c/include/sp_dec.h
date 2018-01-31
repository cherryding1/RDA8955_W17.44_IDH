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




#ifndef _SP_DEC_H_
#define _SP_DEC_H_
/*
 * definition of enumerated types
 */

/*
 * definition of modes for decoder
 */
enum Mode { MR475 = 0,
            MR515,
            MR59,
            MR67,
            MR74,
            MR795,
            MR102,
            MR122,
            MRDTX,
            N_MODES     /* number of (SPC) modes */
          };

/* Declaration recieved frame types */
enum RXFrameType { RX_SPEECH_GOOD = 0,
                   RX_SPEECH_DEGRADED,
                   RX_ONSET,
                   RX_SPEECH_BAD,
                   RX_SID_FIRST,
                   RX_SID_UPDATE,
                   RX_SID_BAD,
                   RX_NO_DATA,
                   RX_N_FRAMETYPES     /* number of frame types */
                 };

/*
 * Function prototypes
 */

/*
 * initialize one instance of the speech decoder
 */
void* Speech_Decode_Frame_init ();

/*
 * free status struct
 */
void Speech_Decode_Frame_exit (void **st);

/*
 * Decodes one frame from encoded parameters
 */
void Speech_Decode_Frame (void *st, enum Mode mode, short *serial,
                          enum RXFrameType frame_type, short *synth);

/*
 * reset speech decoder
 */
int Speech_Decode_Frame_reset (void **st);

#endif

