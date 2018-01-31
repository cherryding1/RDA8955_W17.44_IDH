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



#ifdef __cplusplus
extern "C" {
#endif

enum AmrFileType
{
    AMR_NB_SC,
    AMR_WB_SC,
    AMR_NB_MC,
    AMR_WB_MC,
    AMR_TYPE_NUM
};
enum ChannelStruct
{
    // l : left , r: right , c : central , s : surround , f : front , r : rear
    SINGLE_CHANNEL = 0 ,
    L_R = 1 ,//left , right
    L_R_C, //left , right , central
    FL_FR_RL_RR , // quadrophonic
    L_C_R_S,
    FL_FR_FC_SL_SR,
    L_LC_C_R_RC_S
};

#define MAXBYTESOFONFRAME 32
typedef struct tagAMRStruct
{
    enum AmrFileType        AmrType ;
    long                SampleRate ;
    //FILE*             fp ; // File pointer sheen
    int             fd ; // File handle
    short               nChannels ; // number of channels
    unsigned char*      ChannelBuffer[6] ;/*The buffer for channel ( most channel is 6 )
                                          The first bytes is the frame header */
    unsigned char       BufferPointer[MAXBYTESOFONFRAME*6] ; // the memory pointer when alloced
    enum ChannelStruct  Channel_Struct ; // channel information
    short               CurrMode[6] ;  // To store current mode of each frame
    unsigned char       AmrFileHead[24];//stotre the file head in this buffer
    int                 AmrFileHeadLength ;
} AMRStruct ;

//AMRStruct* OpenAmrFile(char* AmrName ) ;
AMRStruct* OpenAmrFile(char* AmrName , int *bytesConsume,unsigned int* pid , int isIdFile , int offsetForIdFile )  ;
AMRStruct* CreateAmrFile(char* AmrName , enum AmrFileType , enum ChannelStruct , int bCreateFile )  ;
short ReadOneFrameBlock( AMRStruct* AmrStruct, int *bytesLeft ) ;
short WriteOneFrameBlock( AMRStruct* AmrStruct  ) ;
short CloseAmrFile( AMRStruct* AmrStruct ) ;

AMRStruct* AmrDecInit(char *pBufIn, int *bytesConsume);
int AmrFrameDec(AMRStruct* AmrStruct, char *pBufIn, char *pBufOut, int *bytesLeft);
void AmrDecClose(AMRStruct* AmrStruct);


#ifdef __cplusplus
}
#endif
