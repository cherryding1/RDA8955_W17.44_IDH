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




























#define RV_MSG_ID_Get_Decoder_API_Version           0


#define RVDecoder_API_Tromso_Major       2
#define RVDecoder_API_Tromso_Minor       0
/* The underlying codec changed over to the Tromso code base, in the */
/* first half of 2000.  Many of the ILVC custom messages are no longer */
/* applicable. */

#define RVDecoder_API_Margate_Major       3
#define RVDecoder_API_Margate_Minor       0
/* The underlying codec changed over to the Margate (RV9) code base, in the */
/* first half of 2002.  Many of the ILVC custom messages are no longer */
/* applicable. */

typedef struct
{
    ULONG32             message_id;
    /* message_id must be RV_MSG_ID_Get_Decoder_API_Version */

    ULONG32             version;
    /* The version is returned here */
} RVDecoder_API_Version_Message;



/* Define a structure and message id that can be used to change the */
/* dimensions of the output image.  This will fail if called before */
/* RV20toYUV420Init().  It will also fail if the new dimensions */
/* are not supported.  The new dimensions will take effect on the */
/* next call to RV20toYUV420Transform(). */

#define RV_MSG_ID_Change_Decoder_Output_Dimensions  1

typedef struct
{
    ULONG32             message_id;
    /* message_id must be RV_MSG_ID_Change_Decoder_Output_Dimensions */

    ULONG32             width;
    ULONG32             height;
    /* specifies the desired dimensions of the output image. */
} RVDecoder_Output_Dimensions_Message;


/* Define a message id that can be used to tell the decoder that it is */
/* starting a new video sequence.  This is intended to be called after */
/* a seek, for instance. */

#define RV_MSG_ID_Decoder_Start_Sequence    20
