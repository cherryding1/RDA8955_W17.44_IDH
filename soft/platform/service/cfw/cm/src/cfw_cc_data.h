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









#if !defined(COMM_DATA_H)
#define COMM_DATA_H

#if  defined(__cplusplus)
extern  "C" {
#endif



#define CM_INVALID_TI  0xFF

//define the substate
#define SUBSTATE_IDLE              0
//for in MT_PROC state
#define SUBSTATE_MT_PROC_ATA        (1 << 0)
#define SUBSTATE_MT_PROC_CHLD0    (1 << 1)
#define SUBSTATE_MT_PROC_REJECT    (1 << 28)
#define SUBSTATE_MT_PROC_CHLD1       (1 << 2)
#define SUBSTATE_MT_PROC_CHLD2    (1 << 3)
//for in MEETING_HELD state
#define SUBSTATE_MEETINGHLD_CHLD0        (1 << 4)
#define SUBSTATE_MEETINGHLD_CHLD1        (1 << 5)
#define SUBSTATE_MEETINGHLD_CHLD1X      (1 << 6)
#define SUBSTATE_MEETINGHLD_CHLD2        (1 << 7)
#define SUBSTATE_MEETINGHLD_CHLD2X        (1 << 8)
#define SUBSTATE_MEETINGHLD_CHLD3        (1 << 9)
//for in IN_MEETING state
#define SUBSTATE_INMEETING_CHLD0    (1 << 10)
#define SUBSTATE_INMEETING_CHLD1    (1 << 11)
#define SUBSTATE_INMEETING_CHLD1X    (1 << 12)
#define SUBSTATE_INMEETING_CHLD2    (1 << 13)
#define SUBSTATE_INMEETING_CHLD2X    (1 << 14)
#define SUBSTATE_INMEETING_CHLD3    (1 << 15)
//for in HELD state
#define SUBSTATE_HELD_CHLD0    (1 << 16)
#define SUBSTATE_HELD_CHLD1    (1 << 17)
#define SUBSTATE_HELD_CHLD1X    (1 << 18)
#define SUBSTATE_HELD_CHLD2    (1 << 19)
#define SUBSTATE_HELD_CHLD2X    (1 << 20)
#define SUBSTATE_HELD_CHLD3    (1 << 21)
//for in ACTIVE state
#define SUBSTATE_ACTIVE_CHLD0    (1 << 22)
#define SUBSTATE_ACTIVE_CHLD1    (1 << 23)
#define SUBSTATE_ACTIVE_CHLD1X    (1 << 24)
#define SUBSTATE_ACTIVE_CHLD2    (1 << 25)
#define SUBSTATE_ACTIVE_CHLD3    (1 << 26)
#define SUBSTATE_ACTIVE_OR_MEETING_DTMF     (1 << 27)
//for in MO PROC state  hameina[+]
#define SUBSTATE_MO_PROC_DTMF   (1<<28)
#define SUBSTATE_PROC_EXPLICIT_CT   (1<<29)
typedef struct _CM_SM_INFO
{
    BOOL  bProgressIndFlag;      //TRUE: ProgressInd received
    BOOL  bAudioOnFlag;           //TRUE :AUDIO ON event received
    BOOL  bRelHeldCallFlag;      //indicate that release a held call
    BOOL bUsedAFlag;              //Used ATA to accept incoming call flag
    BOOL bUsedChldFlag;           //used CHLD=1,2 to accept incoming call flag
    BOOL bIsMpty;      //(TRUE,FALSE)
    BOOL nReleaseFlag;
    BOOL bMODisc;
    BOOL bNeedSplitFlag; //TRUE: need SplitMPTY after buildMPTY, otherwise,FALSE
    UINT8 nAddressType;
    UINT8 nIndex;
    UINT8 nTI;
    UINT8 nDir;         //0 MO call1, 1 MT call
    UINT8 nCallState;   //0~5
    UINT8 nCallMode;     //(0~9)
    UINT8 nCalledFuncFlag;  //0: nothing function is called
    // 1: CFW_CmInitiateSpeec is called
    // 2: CFW_CmAcceptSpeechCall is called
    // 4: CFW_CmCallHoldMultiparty is called
    // 8: CFW_CmReleaseCall is called
    // 16: CC_Release is called when received disconnect IND from network.
    UINT8 nHoldFunction;
    UINT8 nNumberSize;
    UINT8 bNeedRetrieve;
    UINT8 nCauseVal;  //add by Dhl

    VOID *pMsg;
    VOID *pMsgBackup;
    UINT32 nCurSubState;
    UINT32 nNextSubState;


    UINT8 nReDialNum;
    UINT8 pNumber[0];

} CM_SM_INFO ;

#if  defined(__cplusplus)
}
#endif

#endif // COMM_DATA_H
