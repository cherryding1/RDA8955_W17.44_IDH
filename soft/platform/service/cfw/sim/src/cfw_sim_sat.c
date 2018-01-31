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



#include <cswtype.h>
#include <errorcode.h>
//#include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"
#include "cfw_sim.h"
#include "cfw_sim_sat.h"

extern UINT32 sms_tool_CheckDcs(UINT8 *pDcs);

extern UINT32 _SetTimerExpirationDownloadData(UINT8 *pData, UINT32 nID, CFW_SIM_ID nSimID);
BOOL CFW_GetSATIndFormate(VOID);
UINT32 _ResetSimTimer(UINT8 nTimeInterval, UINT8 nTimeUnit, CFW_SIM_ID nSimID);

//This array is used to store Command details object of the last proactive command for every SIM
UINT8 CommandDetails[CFW_SIM_COUNT][5]= {[0 ... CFW_SIM_COUNT -1] = {0x81,0x03,0x01,0x01}};
//This array is used to store device identities object of the last proactive command for every SIM
UINT8 DeviceIdentities[CFW_SIM_COUNT][4]= {[0 ... CFW_SIM_COUNT -1] = {0x82,0x02,0x82,0x81}};

UINT8 CreateTerminalResponse(UINT8* TerminalResponse, UINT8 nResult, CFW_SIM_ID nSimID)
{
    memcpy(TerminalResponse, CommandDetails[nSimID], 5);
    memcpy(TerminalResponse + 5, DeviceIdentities[nSimID], 4);
    TerminalResponse[5 + 4] = 0x83;
    TerminalResponse[5 + 4 + 1] = 0x01;
    TerminalResponse[5 + 4 + 2] = nResult;
//  CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("------CreateTerminalResponse------",0x08100d7b));
//  CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, DeviceIdentities, 8, 16);
    return 5 + 4 + 3;
}

const UINT8 TerminalResponse[RESPONSEBASE] =
{
    // Command details
    0x81,       // Command Detail tag            Tag=0x01 or 0x81
    0x03,       // Length                          0x03
    // Command Number Modify by lixp at 20081005 for SIM SAT bug.
    // 协议说明3GPP 11.14 6.5.1  Command numbers may take any hexadecimal value between '01' and 'FE'.
    // 目前经验证有些卡，必须是0x00才能正常执行Response的操作。
    // 所以目前定义的是0x00 和 0x01.通常首先尝试0x00,如果失败则尝试0x01.
    // 按照协议，应该是由SIM卡指定Command numbers，ME记录该值，不过实践发现，有些情况使用SIM卡返回的Command numbers
    // 会出现异常，所以仍然按照0x00,0x01来尝试。
    0x00,       // 0x01,
    0x00,       // Type of command               This byte need to be updated.???
    0x00,       // Command Qualifier             ???

    // Device identities
    0x82,       // Device identities tag         Tag=0x01 or 0x81
    0x02,       // Length                        0x02
    0x82,       // Source device identity        ME=0x82
    0x81,       // Destination device identity   SIM=0x81

    // Result
    0x83,       // Result tag                    tag=0x03 or 0x83
    0x01,       // Length                        0x01,    ??????
    0x00        // General result              0x0n or 0x1n successful.???
    //0x2n or 0x3n fail.
    //0x00,    //Additional information
};

// #define ComType          0x03
// #define ComResult        0x0B
#if 0
const UINT8 TerminalResponse1[RESPONSEBASE] =
{
    // Command details
    0x81,       // Command Detail tag            Tag=0x01 or 0x81
    0x03,       // Length                          0x03
    // Command Number Modify by lixp at 20081005 for SIM SAT bug.
    // 协议说明3GPP 11.14 6.5.1  Command numbers may take any hexadecimal value between '01' and 'FE'.
    // 目前经验证有些卡，必须是0x00才能正常执行Response的操作。
    // 所以目前定义的是0x00 和 0x01.通常首先尝试0x00,如果失败则尝试0x01.
    0x01,       // Command Number
    0x00,       // Type of command               This byte need to be updated.???
    0x00,       // Command Qualifier             ???

    // Device identities
    0x82,       // Device identities tag         Tag=0x01 or 0x81
    0x02,       // Length                        0x02
    0x82,       // Source device identity        ME=0x82
    0x81,       // Destination device identity   SIM=0x81

    // Result
    0x83,       // Result tag                    tag=0x03 or 0x83
    0x01,       // Length                        0x01,    ??????
    0x00        // General result                 0x0n or 0x1n successful.???
    // 0x2n or 0x3n fail.
};

const UINT8 ResponseDuration[RESPONSEDURA] =
{
    // Duration
    0x84,       // Duration tag                  tag=0x04 or 0x84
    0x02,       // Length                        0x02
    0x00,       // Time unit                     00-Minutes,01-Seconds,02-Tenths of seconds
    0x00        // Time innterval                 00-Reserved,01-1 unit,02-2 units...FF-255
};
#endif
// #define TextLength        0x01
// #define TextScheme        0x02
const UINT8 ResponseText[RESPONSETEXT] =
{
    // Text string
    0x0D,       // Text string tag                0x0d or 0x8d
    0x00,       // Length                         00
    0x00        // Data coding scheme              00
};

const UINT8 ResponseTextEX[RESPONSETEXT + 1] =
{
    // Text string
    0x0D,       // Text string tag                0x0d or 0x8d
    0x81,
    0x00,       // Length                         00
    0x00        // Data coding scheme              00
};

const UINT8 ResponseUSSD[RESPONSEUSSD] =
{
    // USSD
    0x0d,       // USSD string tag                0x0d or 0x8d
    0x00,       // Length                         00
    0x00        // Data coding scheme             00
};

const UINT8 ResponseUSSDEX[RESPONSEUSSD + 1] =
{
    // USSD
    0x0d,       // USSD string tag                0x0d or 0x8d
    0x81,
    0x00,       // Length                         00
    0x00        // Data coding scheme             00
};

const UINT8 ResponseItem[RESPONSEITEM] =
{
    // Item identifier
    0x90,       // Item identifier tag            tag=0x10 or 0x90
    0x01,       // Length                            0x01
    0x00        // Identifier of item chosen  This byte need to be updated.???
};

// #define ItemSelection    0x02
const UINT8 MenuSelection[] =
{
    0xD3,       // Menu Selection Tag             Tag=0xD3
    0x07,       // Length                         This byte need to be updated.???
    // This can occupy the two byte.
    // Device identities
    0x82,       // Device identities tag          Tag=0x02 or 0x82
    0x02,       // Length                         0x02
    0x01,       // Source device identity         Keypad=0x82
    0x81,       // Destination device identity    SIM=0x81

    // Item identifier
    0x90,       // Item identifier tag            tag=0x10 or 0x90
    0x01,       // Length                         0x01
    0x00        // Identifier of item chosen       This byte need to be updated.???
    // 0x2n or 0x3n fail.
};

const UINT8 EventDownload[] =
{
    0xD6,       // Event download tag             Tag=0xD6
    0x07,       // Length                         This byte need to be updated.???
    // This can occupy the two byte.
    // Device identities
    0x19,       // Event list tag
    0x01,       // Length                         User activity length
    0x04,       // User activity
    0x82,       // Destination device identity
    0x02,       // Length = '02'
    0x82,       // Source device identity         ME
    0x81        // Destination device identity    SIM
};

UINT8 CallControlDownload[] =
{
    0xD4,       // Call control tag          Tag=0xD4
    0x00,       // Length                          0x03

    // Device identities
    0x82,       // Device identities tag         Tag=0x01 or 0x81
    0x02,       // Length                        0x02
    0x82,       // Source device identity        ME=0x82
    0x81        // Destination device identity   SIM=0x81
};

const UINT8 SMPPDownSelection[] =
{
    0xD1,       // SMS TAG Selection Tag             Tag=0xD1
    0x00,       // Length                         This byte need to be updated.???
    // This can occupy the two byte.
    // Device identities
    0x02,       // Device identities tag          Tag=0x02 or 0x82
    0x02,       // Length                         0x02
    0x83,       // Source device identity         Network=0x83
    0x81,       // Destination device identity    SIM=0x81

    // Address identifier
    0x06        // Address identifier tag            tag=0x06 or 0x86
};

const UINT8 SMPPDownSelectionEx[] =
{
    0xD1,       // SMS TAG Selection Tag             Tag=0xD1
    0x81,
    0x00,       // Length                         This byte need to be updated.???
    // This can occupy the two byte.
    // Device identities
    0x02,       // Device identities tag          Tag=0x02 or 0x82
    0x02,       // Length                         0x02
    0x83,       // Source device identity         Network=0x83
    0x81,       // Destination device identity    SIM=0x81

    // Address identifier
    0x06        // Address identifier tag            tag=0x06 or 0x86
};

// #define ItemNumber    0x08        //if Length occupy two byte,this value should add one.
const UINT8 TimerExpirationDownload[] =
{
    0xD7,       // Event download tag             Tag=0xD6
    0x0c,       // Length                         This byte need to be updated.???
    // Device identities
    0x82,       // Destination device identity
    0x02,       // Length = '02'
    0x82,       // Source device identity         ME
    0x81,       // Destination device identity    SIM
    /*
    Byte(s) Description Length
    1   Timer identifier tag    1
    2   Length='01' 1
    3   Timer identifier    1
    */
    0xa4,
    0X01,
    0X01,
    /*
    Byte(s) Description Length
    1   Timer value tag 1
    2   Length='03' 1
    3 - 5   Timer value 3
    */
    0xa5,
    0x03,
    0x00,
    0x00,
    0x52
};

// #define MCC_MNC        0x02
// #define LAC            0x05
// #define CELLID        0x07
const UINT8 ResponseLocalInfo[RESPONSELOCAL] =
{
    0x93,       // Local Information Tag           Tag=0x13 or 0x93
    0x07,       // Length
    0x00,       // Mobile country&Network codes    Three octets
    0x00,       // (MCC & MNC)
    0x00,
    0x00,       // Location Area code(LAC)         Two octets
    0x00,
    0x00,       // Cell Identify Value(Cell ID)    Two octets
    0x00
};

// #define IMEI        0x02
const UINT8 ResponseIMEI[RESPONSEIMEI] =
{
    0x94,       // IMEI Tag                        Tag=0x14 or 0x94
    0x08,       // Length
    0x00,       // IMEI                            eight octets
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

// #define MEASUREMENT        0x02
const UINT8 ResponseMeasure[RESPONSEMEASURE] =
{
    0x96,       // Network measurement Tag    Tag=0x16 or 0x96
    0x10,       // Length
    0x00,       // Measurement results        sixteen octets
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

// #define DateTime    0x02
const UINT8 ResponseDateTime[RESPONSEDATETIME] =
{
    0xA6,        // Date-Time and Zone tag     Tag=0x26 or 0xA6
    0x07,       // Length
    0x00,       // Date Time and Time zone    seven octets
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

// #define Language    0x02
const UINT8 ResponseLanguage[RESPONSELANGUAGE] =
{
    0xAD,       // Language tag            Tag=0x2D or 0xAD
    0x02,       // Length
    0x00,       // Language                Two octets
    0x00
};

// #define MEStatus    0x02
// #define TimingAdv    0x03
const UINT8 ResponseTimingAdvance[RESPONSETADVANCE] =
{
    0xAE,       // Timing Advance tag     Tag=0x2E or 0xAE
    0x02,       // Length
    0x00,       // ME Status              00:ME is in the idle status,01:ME is not in the idle status
    0x00        // Timing Advance          Value
};

// #define CHANNELSTATUS        0x02
const UINT8 ResponseChannel[RESPONSECHANNEL] =
{
    0xB8,       // Channel status Tag            Tag=0x38 or 0xB8
    0x02,       // Length
    0x00,       // Channel status                Two octets
    0x00
};


// #define CardID_Length    0x01
// #define CardID            0x02
const UINT8 ResponseCardID[RESPONSECARDID] =
{
    0xBA,       // Card reader identify tag    Tag=0x3A or 0xBA
    0x00,       // Length,the ID need to be dynamic allocation
    0x00
};

UINT8 nCommondNum[68][CFW_SIM_COUNT]               = { {0x00,}, {0x00,} };

const UINT8 ResponseCardStatus[RESPONSECARDSTATUS] =
{
    0xA0,       // Card reader status tag    Tag=0x20 or 0xA0
    0x01,       // Length
    0x00
};

//return the type of length, 1 or 2 byte
//nLength is output parameter, it will be length of the current object
UINT8 getLengthTLV(UINT8 *pInput, UINT8 *nLength)
{
    UINT8 nIndex = 1;
    if(pInput[nIndex] == 0x81)
        nIndex++;
    *nLength = pInput[nIndex];  //length of one Item
    //*nLength += nIndex + 1;
    return nIndex;
}

UINT8 getCommandTag(UINT8 *pInput)
{
    return pInput[0];
}

UINT8 getCommandDetail(UINT8 *input, struct CommandDetail  *comdetail)
{
    UINT8 offset = 0;
    comdetail->DetailsTag = *(input + offset++);
    comdetail->Length = *(input + offset++);
    comdetail->Number = *(input + offset++);
    comdetail->Type = *(input + offset++);
    comdetail->Qualifier  = *(input + offset);

    if((comdetail ->DetailsTag & 0x7F) == COMMAND_DETAILS_TAG)
    {
        if(comdetail->Length != 0x03)
            return 0;
        return comdetail->Length + 2;
    }
    else
        return 0;
}

UINT8 getDeviceIden(UINT8 *input, struct DeviceIden  *device)
{
    UINT8 offset = 0;
    device->IdentifiesTag = *(input + offset++);
    device->Length = *(input + offset++);
    device->Source = *(input + offset++);
    device->Destination  = *(input + offset);

    if((device ->IdentifiesTag & 0x7F) == DEVICE_IDENTITY_TAG)
    {
        if(device->Length != 0x02)
            return 0;
        return device->Length + 2;
    }
    else
        return 0;
}

UINT8 getCellBroadcastPage(UINT8 *input, struct CellBroadcastPage  *cbp)
{
    UINT8 offset = 0;
    cbp->CellBroadcastPageTag = *(input + offset++);
    cbp->Length = *(input + offset++);
    cbp->CellBroadcastPage  = input + offset;

    if((cbp ->CellBroadcastPageTag & 0x7F) == CELL_BROADCAST_PAGE_TAG)
    {
        if(cbp->Length != 0x58)
            return 0;
        return cbp->Length + 2;
    }
    else
        return 0;
}

UINT8 getDuration(UINT8 *input, struct Duration  *duration)
{

    UINT8 offset = 0;
    duration->DurationTag = *(input + offset++);
    duration->Length =  *(input + offset++);
    duration->TimeUnit = *(input + offset++);
    duration->TimeInterval  =  *(input + offset);

    if((duration ->DurationTag & 0x7F) == DURATION_TAG)
    {
        if(duration->Length != 0x02)
            return 0;
        return duration->Length + 2;
    }
    else
        return 0;
}

UINT8 getItemIdentifier(UINT8 *input, struct ItemIdentifier  *itemiden)
{

    UINT8 offset = 0;
    itemiden->ItemIdenTag = *(input + offset++);
    itemiden->Length =  *(input + offset++);
    itemiden->Identifier = *(input + offset);

    if((itemiden ->ItemIdenTag & 0x7F) == ITEM_IDENTIFIER_TAG)
    {
        if(itemiden->Length != 0x01)
            return 0;
        return itemiden->Length + 2;
    }
    else
        return 0;
}

UINT8 getResponseLength(UINT8 *input, struct ResponseLength *reslen)
{
    UINT8 offset = 0;
    reslen->ResponseLenTag = *(input + offset++);
    reslen->Length =  *(input + offset++);
    reslen->MiniLen =  *(input + offset++);
    reslen->MaxLen = *(input + offset);

    if((reslen ->ResponseLenTag & 0x7F) == RESPONSE_LENGTH_TAG)
    {
        if(reslen->Length != 0x02)
            return 0;
        return reslen->Length + 2;
    }
    else
        return 0;
}

UINT8 getTone(UINT8 *input, struct Tone *tone)
{
    UINT8 offset = 0;
    tone->ToneTag = *(input + offset++);
    tone->Length =  *(input + offset++);
    tone->Tone = *(input + offset);

    if((tone ->ToneTag & 0x7F) == TONE_TAG)
    {
        if(tone->Length != 0x01)
            return 0;
        return tone->Length + 2;
    }
    else
        return 0;
}

UINT8 getLocationInfo(UINT8 *input, struct LocationInfo *localinfo)
{
    UINT8 offset = 0;
    localinfo->LocationInfoTag = *(input + offset++);
    localinfo->Length =  *(input + offset++);
    localinfo->MCCandMNC = input + offset;
    offset += 3;
    localinfo->LACInfo = input + offset;
    offset += 2;
    localinfo->CellIDInfo = input + offset;

    if((localinfo ->LocationInfoTag & 0x7F) == LOCATION_INFO_TAG)
    {
        if(localinfo->Length != 0x07)
            return 0;
        return localinfo->Length + 2;
    }
    else
        return 0;
}

UINT8 getIMEI(UINT8 *input, struct IMEIInfo *imei)
{
    UINT8 offset = 0;
    imei->IMEITag = *(input + offset++);
    imei->Length =  *(input + offset++);
    imei->IMEIInfo = input + offset;

    if((imei ->IMEITag & 0x7F) == IMEI_TAG)
    {
        if(imei->Length != 0x08)
            return 0;
        return imei->Length + 2;
    }
    else
        return 0;
}

UINT8 getHelpRequest(UINT8 *input, struct HelpRequest *helpreq)
{
    UINT8 offset = 0;
    helpreq->HelpRequestTag = *(input + offset++);
    helpreq->Length =  *(input + offset);

    if((helpreq ->HelpRequestTag & 0x7F) == HELP_REQUEST_TAG)
    {
        if(helpreq->Length != 0)
            return 0;
        return helpreq->Length + 2;
    }
    else
        return 0;
}

UINT8 getNetWorkMeasResult(UINT8 *input, struct NetWorkMeasResult *network)
{
    UINT8 offset = 0;
    network->NetWorkMeasResultTag = *(input + offset++);
    network->Length =  *(input + offset++);
    network->NetWorkMeasResult = input + offset;

    if((network ->NetWorkMeasResultTag & 0x7F) == NETWORK_MEASURE_RESULTS_TAG)
    {
        if(network->Length != 0x10)
            return 0;
        return network->Length + 2;
    }
    else
        return 0;
}

UINT8 getItemsNextActionIndicator(UINT8 *input, struct ItemsNAI *itemsnai)
{
    UINT8 offset = 0;
    itemsnai->ItemsNAITAG = *(input + offset++);
    itemsnai->Length = *(input + offset++);
    itemsnai->ItemsNAIList = input + offset;

    if((itemsnai ->ItemsNAITAG & 0x7F) == ITEMS_NEXT_ACTION_INDICATOR_TAG)
    {
        if(itemsnai->Length == 0)
            return 0;
        return itemsnai->Length + 2;
    }
    else
        return 0;
}

UINT8 getCause(UINT8 *input, struct CauseInfo *cause)
{
    UINT8 offset = 0;
    cause->CauseTag = *(input + offset++);
    cause->Length =  *(input + offset++);
    cause->CauseInfo = input + offset;

    if((cause ->CauseTag & 0x7F) == CAUSE_TAG)
    {
        if((cause->Length == 0x01) || (cause->Length > 31))
            return 0;
        return cause->Length + 2;
    }
    else
        return 0;
}

UINT8 getLocationStatus(UINT8 *input, struct LocationStatus *localstatus)
{
    UINT8 offset = 0;
    localstatus->LocationStatusTag = *(input + offset++);
    localstatus->Length =  *(input + offset++);
    localstatus->LocationStatus = *(input + offset++);

    if((localstatus ->LocationStatusTag & 0x7F) == LOCATION_STATUS_TAG)
    {
        if(localstatus->Length != 0x01)
            return 0;
        return localstatus->Length + 2;
    }
    else
        return 0;
}

UINT8 getTransactionIden(UINT8 *input, struct TransactionIden *traniden)
{
    UINT8 offset = 0;
    traniden->TransactionIdenTag = *(input + offset++);
    traniden->Length =  *(input + offset++);
    traniden->TransactionIdenList = input + offset;

    if((traniden ->TransactionIdenTag & 0x7F) == TRANSACTION_IDENTIFIER_TAG)
    {
        if(traniden->Length == 0)
            return 0;
        return traniden->Length + 2;
    }
    else
        return 0;
}

UINT8 getBCCHChannelList(UINT8 *input, struct BCCHChannelList *bcch)
{
    UINT8 offset = 0;
    bcch->BCCHChannelListTag = *(input + offset++);
    bcch->Length =  *(input + offset++);
    bcch->BCCHChannelList = input + offset;

    if((bcch ->BCCHChannelListTag & 0x7F) == BCCH_CHANNEL_LIST_TAG)
    {
        if(bcch->Length == 0)
            return 0;
        return bcch->Length + 2;
    }
    else
        return 0;
}

UINT8 getIconIdentifier(UINT8 *input, struct IconIden *iconiden)
{
    UINT8 offset = 0;
    iconiden->IconIdentifierTag = *(input + offset++);
    iconiden->Length =  *(input + offset++);
    iconiden->IconQualifier = *(input + offset++);
    iconiden->IconIdentifier = *(input + offset++);

    if((iconiden ->IconIdentifierTag & 0x7F) == ICON_IDENTIFIER_TAG)
    {
        if(iconiden->Length != 0x02)
            return 0;
        return iconiden->Length + 2;
    }
    else
        return 0;
}

UINT8 getItemIconIdenList(UINT8 *input, struct ItemIconIdenList *itemlist)
{
    UINT8 offset = 0;
    itemlist->ItemIconIdenListTag = *(input + offset++);
    itemlist->Length =  *(input + offset++);
    itemlist->IconListQualifier = *(input + offset++);
    itemlist->IconIdenList = input + offset;

    if((itemlist ->ItemIconIdenListTag & 0x7F) == ITEM_ICON_IDENTIFIER_LIST_TAG)
    {
        if(itemlist->Length == 0)
            return 0;
        return itemlist->Length + 2;
    }
    else
        return 0;
}

UINT8 getCardReaderStatus(UINT8 *input, struct CardReaderStatus *cardstatus)
{
    UINT8 offset = 0;
    cardstatus->CardReaderStatusTag = *(input + offset++);
    cardstatus->Length =  *(input + offset++);
    cardstatus->CardReaderStatus = *(input + offset);

    if((cardstatus ->CardReaderStatusTag & 0x7F) == CARD_READER_STATUS_A_TAG)
    {
        if(cardstatus->Length != 0x01)
            return 0;
        return cardstatus->Length + 2;
    }
    else
        return 0;
}

UINT8 getCardATR(UINT8 *input, struct CardATR *atr)
{
    UINT8 offset = 0;
    atr->CardATRTag = *(input + offset++);
    atr->Length =  *(input + offset++);
    atr->ATR = input + offset;

    if((atr ->CardATRTag & 0x7F) == CARD_ATR_A_TAG)
    {
        if(atr->Length == 0)
            return 0;
        return atr->Length + 2;
    }
    else
        return 0;
}

UINT8 getTimerIden(UINT8 *input, struct TimerIden *timer)
{
    UINT8 offset = 0;
    timer->TimerIdenTag = *(input + offset++);
    timer->Length =  *(input + offset++);
    timer->Identifier = *(input + offset);

    if((timer ->TimerIdenTag & 0x7F) == TIMER_IDENTIFIER_TAG)
    {
        if(timer->Length != 0x01)
            return 0;
        return timer->Length + 2;
    }
    else
        return 0;
}

UINT8 getTimerValue(UINT8 *input, struct TimerValue *timer)
{
    UINT8 offset = 0;
    timer->TimerValueTag = *(input + offset++);
    timer->Length =  *(input + offset++);
    timer->TimerValue = input + offset;

    if((timer ->TimerValueTag & 0x7F) == TIMER_VALUE_TAG)
    {
        if(timer->Length != 0x03)
            return 0;
        return timer->Length + 2;
    }
    else
        return 0;
}

UINT8 getDateTimeZone(UINT8 *input, struct DateTimeZone *dtz)
{
    UINT8 offset = 0;
    dtz->DateTimeZoneTag = *(input + offset++);
    dtz->Length =  *(input + offset++);
    dtz->DateTimeZone = input + offset;

    if((dtz ->DateTimeZoneTag & 0x7F) == DATE_TIME_ZONE_TAG)
    {
        if(dtz->Length != 0x07)
            return 0;
        return dtz->Length + 2;
    }
    else
        return 0;
}

UINT8 getBCRepeatInd(UINT8 *input, struct BCRepeatInd *bcri)
{
    UINT8 offset = 0;
    bcri->BCRepeatIndTag = *(input + offset++);
    bcri->Length =  *(input + offset++);
    bcri->BCRepeatIndValue = *(input + offset);

    if((bcri ->BCRepeatIndTag & 0x7F) == BC_REPEAT_INDICATOR_TAG)
    {
        if(bcri->Length != 0x01)
            return 0;
        return bcri->Length + 2;
    }
    else
        return 0;
}

UINT8 getImmediateRSP(UINT8 *input, struct ImmediateRSP *imrsp)
{
    UINT8 offset = 0;
    imrsp->ImmediateRSPTag = *(input + offset++);
    imrsp->Length =  *(input + offset);

    if((imrsp ->ImmediateRSPTag & 0x7F) == IMMEDIATE_RESPONSE_TAG)
    {
        if(imrsp->Length != 0)
            return 0;
        return imrsp->Length + 2;
    }
    else
        return 0;
}

UINT8 getLanguage(UINT8 *input, struct LanguageInfo *lang)
{
    UINT8 offset = 0;
    lang->LanguageTag = *(input + offset++);
    lang->Length =  *(input + offset++);
    lang->LanguageInfo =  input + offset;

    if((lang ->LanguageTag & 0x7F) == LANGUAGE_TAG)
    {
        if(lang->Length != 0x02)
            return 0;
        return lang->Length + 2;
    }
    else
        return 0;
}

UINT8 getTimingAdvance(UINT8 *input, struct TimingAdvanceInfo *timing)
{
    UINT8 offset = 0;
    timing->TimingAdvanceTag = *(input + offset++);
    timing->Length =  *(input + offset++);
    timing->MEStatusInfo = *(input + offset++);
    timing->TimingAdvanceInfo =  *(input + offset);

    if((timing ->TimingAdvanceTag & 0x7F) == TIMING_ADVANCE_TAG)
    {
        if(timing->Length != 0x02)
            return 0;
        return timing->Length + 2;
    }
    else
        return 0;
}

UINT8 getBrowserIden(UINT8 *input, struct BrowserIden *browser)
{
    UINT8 offset = 0;
    browser->BrowserIdenTag = *(input + offset++);
    browser->Length =  *(input + offset++);
    browser->BrowserIdentifier =  *(input + offset);

    if((browser ->BrowserIdenTag & 0x7F) == BROWSER_IDENTITY_C_TAG)
    {
        if(browser->Length != 0x01)
            return 0;
        return browser->Length + 2;
    }
    else
        return 0;
}

UINT8 getBrowserTerminationCause(UINT8 *input, struct BrowserTerminationCause *browser)
{
    UINT8 offset = 0;
    browser->BrowserTerminationCauseTag = *(input + offset++);
    browser->Length =  *(input + offset++);
    browser->BrowserTerminationCause =  *(input + offset);

    if((browser ->BrowserTerminationCauseTag & 0x7F) == BROWSER_TERMINATION_CAUSE_C_TAG)
    {
        if(browser->Length != 0x01)
            return 0;
        return browser->Length + 2;
    }
    else
        return 0;
}

UINT8 getBearerDes(UINT8 *input, struct BearerDes *des)
{
    UINT8 offset = 0;
    des->BearerDesTag = *(input + offset++);
    des->Length =  *(input + offset++);
    des->Type =  *(input + offset++);
    des->BearerParameters =  input + offset;

    if((des ->BearerDesTag & 0x7F) == BEARER_DESCRIPTION_E_TAG)
    {
        if(des->Length == 0)
            return 0;
        return des->Length + 2;
    }
    else
        return 0;
}

UINT8 getChannelDataLength(UINT8 *input, struct ChannelDataLength *datalen)
{
    UINT8 offset = 0;
    datalen->ChannelDataLengthTag = *(input + offset++);
    datalen->Length =  *(input + offset++);
    datalen->ChannelDataLength =  *(input + offset);

    if((datalen ->ChannelDataLengthTag & 0x7F) == CHANNEL_DATA_LENGTH_E_TAG)
    {
        if(datalen->Length != 0x01)
            return 0;
        return datalen->Length + 2;
    }
    else
        return 0;
}

UINT8 getBufferSize(UINT8 *input, struct BufferSize *buffersize)
{
    UINT8 offset = 0;
    buffersize->BufferSizeTag = *(input + offset++);
    buffersize->Length =  *(input + offset++);
    buffersize->BufferSize =  input + offset;

    if((buffersize ->BufferSizeTag & 0x7F) == BUFFER_SIZE_E_TAG)
    {
        if(buffersize->Length != 0x02)
            return 0;
        return buffersize->Length + 2;
    }
    else
        return 0;
}

UINT8 getChannelStatus(UINT8 *input, struct ChannelStatusInfo *status)
{
    UINT8 offset = 0;
    status->ChannelStatusTag = *(input + offset++);
    status->Length =  *(input + offset++);
    status->ChannelStatusInfo =  input + offset;

    if((status->ChannelStatusTag & 0x7F) == CHANNEL_STATUS_E_TAG)
    {
        if(status->Length != 0x02)
            return 0;
        return status->Length + 2;
    }
    else
        return 0;
}

UINT8 getCardReaderIden(UINT8 *input, struct CardReaderIden *cardreader)
{
    UINT8 offset = 0;
    cardreader->CardReaderIdenTag = *(input + offset++);
    cardreader->Length =  *(input + offset++);
    cardreader->IdenCardReader =  input + offset;

    if((cardreader ->CardReaderIdenTag & 0x7F) == CARD_READER_IDENTIFIER_A_TAG)
    {
        if(cardreader->Length == 0)
            return 0;
        return cardreader->Length + 2;
    }
    else
        return 0;
}

UINT8 getOtherAddress(UINT8 *input, struct OtherAddress *address)
{
    UINT8 offset = 0;
    address->OtherAddressTag = *(input + offset++);
    address->Length =  *(input + offset++);
    address->Type =  *(input + offset++);
    address->OtherAddress =  input + offset;

    if((address ->OtherAddressTag & 0x7F) == OTHER_ADDRESS_E_TAG)
    {
        if(address->Length == 0)
            return 0;
        return address->Length + 2;
    }
    else
        return 0;
}

UINT8 getTransportLevel(UINT8 *input, struct TransportLevel *level)
{
    UINT8 offset = 0;
    level->TransportLevelTag = *(input + offset++);
    level->Length =  *(input + offset++);
    level->Type =  *(input + offset++);
    level->PortNumber =  input + offset;

    if((level ->TransportLevelTag & 0x7F) == SIM_ME_INTERFACE_TRANSPORT_LEVEL_E_TAG)
    {
        if(level->Length != 0x03)
            return 0;
        return level->Length + 2;
    }
    else
        return 0;
}

UINT8 getNetworkAccessName(UINT8 *input, struct NetworkAccessName *name)
{
    UINT8 offset = 0;
    name->NetworkAccessNameTag = *(input + offset++);
    name->Length =  *(input + offset++);
    name->NetworkAccessName =  input + offset;

    if((name ->NetworkAccessNameTag & 0x7F) == NETWORK_ACCESS_NAME_TAG)
    {
        if(name->Length == 0)
            return 0;
        return name->Length + 2;
    }
    else
        return 0;
}

UINT8 getAddress(UINT8 *input, struct Address *address)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    address->AddType = size;

#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        address->AddressTag = temp->Tag;
        address->Length = temp->Len;
        address->TONandNPI = temp->OneByte;
        address->DiallingNumber = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        address->AddressTag = temp->Tag;
        address->Length = temp->Len;
        address->TONandNPI = temp->OneByte;
        address->DiallingNumber = temp->String;
    }
#else
    length += size + 1;
    address->AddressTag = *(input);
    address->Length = *(input + size++);
    address->TONandNPI = *(input + size++);
    address->DiallingNumber = input + size;
#endif
    if((address ->AddressTag & 0x7F) == ADDRESS_TAG)
    {
//  if(address->Length == 0)
//      return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getAlphaIden(UINT8 *input, struct AlphaIden *alphaiden)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    alphaiden->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        alphaiden->AlphaTag = temp->Tag;
        alphaiden->Length = temp->Len;
        alphaiden->AlphaIdentifier = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        alphaiden->AlphaTag = temp->Tag;
        alphaiden->Length = temp->Len;
        alphaiden->AlphaIdentifier = temp->String;
    }
#else
    length += size + 1;
    alphaiden->AlphaTag = *(input);
    alphaiden->Length = *(input + size++);
    alphaiden->AlphaIdentifier = input + size;
#endif
    if((alphaiden ->AlphaTag & 0x7F) == ALPHA_IDENTIFIER_TAG)
    {
        //if(alphaiden->Length == 0)
        //  return 0;
        return length;  //Return the size of present Object including Tag and length
    }
    else
        return 0;       //present object is not ALPHA_IDENTIFIER_TAG
}

UINT8 getSubAddress(UINT8 *input, struct SubAddress *subaddress)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    subaddress->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        subaddress->SubAddressTag = temp->Tag;
        subaddress->Length = temp->Len;
        subaddress->SubAddress = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        subaddress->SubAddressTag = temp->Tag;
        subaddress->Length = temp->Len;
        subaddress->SubAddress = temp->String;
    }
#else
    length += size + 1;
    subaddress->SubAddressTag = *(input);
    subaddress->Length = *(input + size++);
    subaddress->SubAddress = input + size;
#endif
    if((subaddress ->SubAddressTag & 0x7F) == SUBADDRESS_TAG)
    {
        //if(subaddress->Length == 0)
        //    return 0;
        return length;  //Return the size of present Object including Tag and length
    }
    else
        return 0;       //present object is not SUBADDRESS_TAG
}

UINT8 getCapabilityCP(UINT8 *input, struct CapabilityCP *capabilitycp)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    capabilitycp->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        capabilitycp->CapabilityCPTag = temp->Tag;
        capabilitycp->Length = temp->Len;
        capabilitycp->CapabilityCP = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        capabilitycp->CapabilityCPTag = temp->Tag;
        capabilitycp->Length = temp->Len;
        capabilitycp->CapabilityCP = temp->String;
    }
#else
    length += size + 1;
    capabilitycp->CapabilityCPTag = *(input);
    capabilitycp->Length = *(input + size++);
    capabilitycp->CapabilityCP = input + size;
#endif
    if((capabilitycp ->CapabilityCPTag & 0x7F) == CAPABILITY_CONFIG_PARAM_TAG)
    {
        if(capabilitycp->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getItem(UINT8 *input, struct Item *item)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    item->AddType = size;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("length = %d,size = %d",0x08100d7c), length, size);
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        item->ItemTag = temp->Tag;
        item->Length = temp->Len;
        item->Identifier = temp->OneByte;
        item->TextString = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        item->ItemTag = temp->Tag;
        item->Length = temp->Len;
        item->Identifier = temp->OneByte;
        item->TextString = temp->String;
    }
#else
    length += size + 1;
    item->ItemTag = *(input);
    item->Length = *(input + size++);
    item->Identifier = *(input + size++);
    item->TextString = input + size;
#endif
    if((item ->ItemTag & 0x7F) == ITEM_TAG)
    {
        if(item->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getResult(UINT8 *input, struct Result *result)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    result->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *)(input);
        result->ResultTag = temp->Tag;
        result->Length = temp->Len;
        result->GeneralResult = temp->OneByte;
        if(result->Length - 1 > 0)
            result->AdditionalInfo = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *)(input);
        result->ResultTag = temp->Tag;
        result->Length = temp->Len;
        result->GeneralResult = temp->OneByte;
        if(result->Length - 1 > 0)
            result->AdditionalInfo = temp->String;
    }
#else
    length += size + 1;
    result->ResultTag = *(input);
    result->Length = *(input + size++);
    result->GeneralResult = *(input + size++);
    if(result->Length - 1 > 0)
        result->AdditionalInfo = input + size;
#endif
    if((result ->ResultTag & 0x7F) == RESULT_TAG)
    {
        if(result->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getSMSTPDU(UINT8 *input, struct SMSTPDU *smstpdu)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    smstpdu->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        smstpdu->SMSTPDUTag = temp->Tag;
        smstpdu->Length = temp->Len;
        smstpdu->SMSTPDU = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        smstpdu->SMSTPDUTag = temp->Tag;
        smstpdu->Length = temp->Len;
        smstpdu->SMSTPDU = temp->String;
    }
#else
    length += size + 1;
    smstpdu->SMSTPDUTag = *(input);
    smstpdu->Length = *(input + size++);
    smstpdu->SMSTPDU = input + size;
#endif
    if((smstpdu ->SMSTPDUTag & 0x7F) == SMS_TPDU_TAG)
    {
        if(smstpdu->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getSSString(UINT8 *input, struct SSString *ssstring)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    ssstring->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        ssstring->SSStringTag = temp->Tag;
        ssstring->Length = temp->Len;
        ssstring->TONandNPI = temp->OneByte;
        ssstring->SSorUSSDString = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        ssstring->SSStringTag = temp->Tag;
        ssstring->Length = temp->Len;
        ssstring->TONandNPI = temp->OneByte;
        ssstring->SSorUSSDString = temp->String;
    }
#else
    length += size + 1;
    ssstring->SSStringTag = *(input);
    ssstring->Length = *(input + size++);
    ssstring->TONandNPI =  *(input + size++);
    ssstring->SSorUSSDString =  input + size;
#endif
    if((ssstring ->SSStringTag & 0x7F) == SS_STRING_TAG)
    {
        if(ssstring->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getTextString(UINT8 *input, struct TextString *textstring)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    textstring->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        textstring->TextStringTag = temp->Tag;
        textstring->Length = temp->Len;
        textstring->DataCodingScheme = temp->OneByte;
        textstring->TextString = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        textstring->TextStringTag = temp->Tag;
        textstring->Length = temp->Len;
        textstring->DataCodingScheme = temp->OneByte;
        textstring->TextString = temp->String;
    }
#else
    length += size + 1;
    textstring->TextStringTag = *(input);
    textstring->Length = *(input + size++);
    textstring->DataCodingScheme = *(input + size++);
    textstring->TextString = input + size;
#endif
    if((textstring ->TextStringTag & 0x7F) == TEXT_STRING_TAG)
    {
        if(textstring->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getUSSDString(UINT8 *input, struct USSDString *ussdstring)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    ussdstring->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        ussdstring->USSDStringTag = temp->Tag;
        ussdstring->Length = temp->Len;
        ussdstring->DataCodingScheme = temp->OneByte;
        ussdstring->USSDString = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        ussdstring->USSDStringTag = temp->Tag;
        ussdstring->Length = temp->Len;
        ussdstring->DataCodingScheme = temp->OneByte;
        ussdstring->USSDString = temp->String;
    }
#else
    length += size + 1;
    ussdstring->USSDStringTag = *(input);
    ussdstring->Length = *(input + size++);
    ussdstring->DataCodingScheme =  *(input + size++);
    ussdstring->USSDString = input + size;
#endif
    if((ussdstring ->USSDStringTag & 0x7F) == USSD_STRING_TAG)
    {
        if(ussdstring->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getFileList(UINT8 *input, struct FileList *filelist)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    filelist->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        filelist->FileListTag = temp->Tag;
        filelist->Length = temp->Len;
        filelist->Numberoffiles = temp->OneByte;
        filelist->Files = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        filelist->FileListTag = temp->Tag;
        filelist->Length = temp->Len;
        filelist->Numberoffiles = temp->OneByte;
        filelist->Files = temp->String;
    }
#else
    length += size + 1;
    filelist->FileListTag = *(input);
    filelist->Length = *(input + size++);
    filelist->Numberoffiles = *(input + size++);
    filelist->Files = input + size;
#endif
    if((filelist ->FileListTag & 0x7F) == FILE_LIST_TAG)
    {
        if(filelist->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getDefaultText(UINT8 *input, struct DefaultText *deftxt)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    deftxt->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LBS *temp = (struct _T2LBS *) (input);
        deftxt->DefaultTextTag = temp->Tag;
        deftxt->Length = temp->Len;
        deftxt->DataCodingScheme = temp->OneByte;
        deftxt->TextString = temp->String;
    }
    else
    {
        struct _T1LBS *temp = (struct _T1LBS *) (input);
        deftxt->DefaultTextTag = temp->Tag;
        deftxt->Length = temp->Len;
        deftxt->DataCodingScheme = temp->OneByte;
        deftxt->TextString = temp->String;
    }
#else
    length += size + 1;
    deftxt->DefaultTextTag = *(input);
    deftxt->Length = *(input + size++);
    deftxt->DataCodingScheme = *(input + size++);
    deftxt->TextString = input + size;
#endif
    if((deftxt ->DefaultTextTag & 0x7F) == DEFAULT_TEXT_TAG)
    {
        if(deftxt->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}


UINT8 getEventList(UINT8 *input, struct EventList *eventlist)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    eventlist->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        eventlist->EventListTag = temp->Tag;
        eventlist->Length = temp->Len;
        eventlist->EventList = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        eventlist->EventListTag = temp->Tag;
        eventlist->Length = temp->Len;
        eventlist->EventList = temp->String;
    }
#else
    length += size + 1;
    eventlist->EventListTag = *(input);
    eventlist->Length = *(input + size++);
    eventlist->EventList = input + size;
#endif
#if 0
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("eventlist->EventListTag = 0x%x",0x08100d7d), eventlist->EventListTag);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("eventlist->Length = 0x%x",0x08100d7e), eventlist->Length);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("eventlist->EventList =",0x08100d7f));
    UINT8 i = 0;
    for(i = 0; i < eventlist->Length; i++)
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("%x",0x08100d80), eventlist->EventList[i]);
#endif
    if((eventlist ->EventListTag & 0x7F) == EVENT_LIST_TAG)
    {
        if(eventlist->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getCallControlReqAct(UINT8 *input, struct CallControlReqAct *callcontrolreqact)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    callcontrolreqact->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        callcontrolreqact->CallControlReqActTag = temp->Tag;
        callcontrolreqact->Length = temp->Len;
        callcontrolreqact->CallControlReqAct = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        callcontrolreqact->CallControlReqActTag = temp->Tag;
        callcontrolreqact->Length = temp->Len;
        callcontrolreqact->CallControlReqAct = temp->String;
    }
#else
    length += size + 1;
    callcontrolreqact->CallControlReqActTag = *(input);
    callcontrolreqact->Length = *(input + size++);
    callcontrolreqact->CallControlReqAct = input + size;
#endif
    if((callcontrolreqact ->CallControlReqActTag & 0x7F) == CALL_CONTROL_REQURESTED_ACTION_TAG)
    {
        if(callcontrolreqact->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}



UINT8 getRAPDU(UINT8 *input, struct RAPDU *rapdu)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    rapdu->AddType = size;
    length += size + 1;

    rapdu->RAPDUTag = *(input);
    rapdu->Length = *(input + size++);
    if(rapdu->Length > 2)
        rapdu->RAPDUdata = input + size;

    rapdu->SW1 = *(input + rapdu->Length - 2);
    rapdu->SW2 = *(input + rapdu->Length - 1);
    if((rapdu ->RAPDUTag & 0x7F) == R_APDU_A_TAG)
    {
        if(rapdu->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getCAPDU(UINT8 *input, struct CAPDU *capdu)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    capdu->AddType = size;
    length += size + 1;
    capdu->CAPDUTag = *(input);
    capdu->Length = *(input + size++);
    capdu->CLA = *(input + size++);
    capdu->INS = *(input + size++);
    capdu->P1 = *(input + size++);
    capdu->P2 = *(input + size++);
    if(capdu->Length == 5)
    {
        capdu->Lc = 0;
        capdu->data = 0;
        capdu->Le = *(input + size);
    }
    else
    {
        capdu->Lc = *(input + size++);
        capdu->data = input + size;
        capdu->Le = *(input + capdu->Length - 1);
    }
    if((capdu ->CAPDUTag & 0x7F) == C_APDU_A_TAG)
    {
        if(capdu->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getATCommand(UINT8 *input, struct ATCommand *atcommand)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    atcommand->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        atcommand->ATCommandTag = temp->Tag;
        atcommand->Length = temp->Len;
        atcommand->ATCommandString = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        atcommand->ATCommandTag = temp->Tag;
        atcommand->Length = temp->Len;
        atcommand->ATCommandString = temp->String;
    }
#else
    length += size + 1;
    atcommand->ATCommandTag = *(input);
    atcommand->Length = *(input + size++);
    atcommand->ATCommandString = input + size;
#endif
    if((atcommand ->ATCommandTag & 0x7F) == AT_COMMAND_TAG)
    {
        if(atcommand->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 BuildATResponse(UINT8 *input, UINT8 *output)
{
    if(input == NULL)
        return 0;
    UINT8 i;
    UINT32 length = SUL_Strlen(input);
    *output++ = AT_RESPONSE_B_TAG;
    if(length > 127)
        *output++ = 0x81;

    for(i = 0; i < length; i++)
    {
        *output++ = *input++;
    }
    return i;
}

UINT8 getDTMFString(UINT8 *input, struct DTMFString *dtmf)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    dtmf->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        dtmf->DTMFStringTag = temp->Tag;
        dtmf->Length = temp->Len;
        dtmf->DTMFString = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        dtmf->DTMFStringTag = temp->Tag;
        dtmf->Length = temp->Len;
        dtmf->DTMFString = temp->String;
    }
#else
    length += size + 1;
    dtmf->DTMFStringTag = *(input);
    dtmf->Length = *(input + size++);
    dtmf->DTMFString = input + size;
#endif
    if((dtmf ->DTMFStringTag & 0x7F) == DTMF_STRING_TAG)
    {
        if(dtmf->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getURL(UINT8 *input, struct URL *url)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    url->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        url->URLTag = temp->Tag;
        url->Length = temp->Len;
        url->URL = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        url->URLTag = temp->Tag;
        url->Length = temp->Len;
        url->URL = temp->String;
    }
#else
    length += size + 1;
    url->URLTag = *(input);
    url->Length = *(input + size++);
    url->URL = input + size;
#endif
    if((url ->URLTag & 0x7F) == URL_C_E_TAG)
    {
        if(url->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

//'00' = SMS ;
//'01' = CSD ;
//'02' = USSD ;
//'03' = GPRS ;
//'04' to 'FF' = RFU.
UINT8 getBearer(UINT8 *input, struct Bearer *bearer)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    bearer->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        bearer->BearerTag = temp->Tag;
        bearer->Length = temp->Len;
        bearer->ListBearers = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        bearer->BearerTag = temp->Tag;
        bearer->Length = temp->Len;
        bearer->ListBearers = temp->String;
    }
#else
    length += size + 1;
    bearer->BearerTag = *(input);
    bearer->Length = *(input + size++);
    bearer->ListBearers = input + size;
#endif
    if((bearer ->BearerTag & 0x7F) == BEARER_C_TAG)
    {
        if(bearer->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getProvisoinFileRef(UINT8 *input, struct ProvisoinFileRef *profileref)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    profileref->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        profileref->ProvisionFileRefTag = temp->Tag;
        profileref->Length = temp->Len;
        profileref->Path = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        profileref->ProvisionFileRefTag = temp->Tag;
        profileref->Length = temp->Len;
        profileref->Path = temp->String;
    }
#else
    length += size + 1;
    profileref->ProvisionFileRefTag = *(input);
    profileref->Length = *(input + size++);
    profileref->Path = input + size;
#endif
    if((profileref ->ProvisionFileRefTag & 0x7F) == PROVISIONING_REFER_FILE_C_TAG)
    {
        if(profileref->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT8 getChannelData(UINT8 *input, struct ChannelData *channeldata)
{
    UINT8 length = 0;
    UINT8 size = getLengthTLV(input, &length);
    channeldata->AddType = size;
#ifdef __GCC__
    if(size == ADDRESS2BYTE)
    {
        struct _T2LS *temp = (struct _T2LS *)(input);
        channeldata->ChannelDataTag = temp->Tag;
        channeldata->Length = temp->Len;
        channeldata->ChannelDataString = temp->String;
    }
    else
    {
        struct _T1LS *temp = (struct _T1LS *)(input);
        channeldata->ChannelDataTag = temp->Tag;
        channeldata->Length = temp->Len;
        channeldata->ChannelDataString = temp->String;
    }
#else
    length += size + 1;
    channeldata->ChannelDataTag = *(input);
    channeldata->Length = *(input + size++);
    channeldata->ChannelDataString = input + size;
#endif
    if((channeldata ->ChannelDataTag & 0x7F) == CHANNEL_DATA_E_TAG)
    {
        if(channeldata->Length == 0)
            return 0;
        return length;
    }
    else
        return 0;
}

UINT32 fetchSetupMenu(CFW_SAT_MENU_RSP **pMenuItemList, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI       = 0;
    UINT8 lengthI = 0;
    UINT8 lengthINAI = 0;
    UINT8 lengthII = 0;
    UINT8 lengthIIIL = 0;

    UINT8 *pOffset = pInput;
    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_MENU_RSP));

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("fetchSetupMenu",0x08100d81));
    if((pInput == NULL) || nLength == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("fetchSetupMenu (pInput == NULL) || pInput == NULL) pInput =%x,nLength = %d",0x08100d82), pInput, nLength);
        return ERR_PARAM_ILLEGAL;
    }
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;

    struct AlphaIden alphaiden;
    if((lengthAI = getAlphaIden(pOffset, &alphaiden)) == 0)
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthAI;
    nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);

    struct Item item;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL),"nLength = %d, pOffset = %x, pInput = %x(%d)",nLength, pOffset, pInput, pOffset - pInput);
    if(nLength <= pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    UINT8 i = 0;
    UINT8 *pItemOffset = pOffset;
    UINT8 ItemNum  = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize = %x",0x08100d83), nMemsize);

    while(nLength > pOffset - pInput)
    {
        if((lengthI = getItem(pOffset, &item)) == 0)
        {
            if((*pOffset & 0x7F) != ITEM_TAG)
                break;
            if(ItemNum == 1)
            {
                *pMenuItemList = (CFW_SAT_MENU_RSP *)CSW_SIM_MALLOC(nMemsize);
                pOffset = (UINT8 *)(*pMenuItemList) + (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_MENU_RSP));
                //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("*pMenuItemList start address = %x",0x08100d84), *pMenuItemList);
                //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize = %x",0x08100d85), nMemsize);

                (*pMenuItemList)->nAlphaLen = alphaiden.Length;
                (*pMenuItemList)->pAlphaStr = pOffset;
                SUL_MemCopy8((*pMenuItemList)->pAlphaStr, alphaiden.AlphaIdentifier, alphaiden.Length);
                pOffset += MEM_ALIGN_SIZE(alphaiden.Length);

                (*pMenuItemList)->pItemList = NULL;
                return ERR_SUCCESS;
            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pOffset = %x,lengthI = %d",0x08100d86), pOffset, lengthI);
        ItemNum++;

        pOffset += lengthI;
        nMemsize += (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST)) + (UINT8) MEM_ALIGN_SIZE(item.Length - 1);
        i += item.Length;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nMemsize = %d,item.Length = %d",0x08100d87), nMemsize, item.Length);
    }

    struct ItemsNAI itemnai;
    if(nLength > pOffset - pInput)
    {
        lengthINAI = getItemsNextActionIndicator(pOffset, &itemnai);
        if(lengthINAI != 0)
        {
            pOffset += lengthINAI;
            nMemsize += MEM_ALIGN_SIZE(itemnai.Length);
        }
    }

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
    {
        lengthII = getIconIdentifier(pOffset, &iconiden);
        if(lengthII != 0)
            pOffset += lengthII;
    }

    struct ItemIconIdenList itemiconidenlist;
    if(nLength > pOffset - pInput)
    {
        lengthIIIL = getItemIconIdenList(pOffset, &itemiconidenlist);
        if(lengthIIIL != 0)
            nMemsize += MEM_ALIGN_SIZE(itemiconidenlist.Length - 1);
    }
    *pMenuItemList = (CFW_SAT_MENU_RSP *)CSW_SIM_MALLOC(nMemsize);
    pOffset = (UINT8 *)(*pMenuItemList) + (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_MENU_RSP));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("*pMenuItemList start address = %x",0x08100d88), *pMenuItemList);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize = %x",0x08100d89), nMemsize);

    (*pMenuItemList)->nAlphaLen = alphaiden.Length;
    (*pMenuItemList)->pAlphaStr = pOffset;
    SUL_MemCopy8((*pMenuItemList)->pAlphaStr, alphaiden.AlphaIdentifier, alphaiden.Length);
    pOffset += MEM_ALIGN_SIZE(alphaiden.Length);

    (*pMenuItemList)->pItemList = (CFW_SAT_ITEM_LIST *)pOffset;

    CFW_SAT_ITEM_LIST *pList;
    if(ItemNum == 0)
        pList = NULL;
    else
        pList = (CFW_SAT_ITEM_LIST *)pOffset;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "1pList = %x, ItemNum = %d", pList, ItemNum);
    for(i = 0; i < ItemNum; i++)
    {
        lengthI = getItem(pItemOffset, &item);
        pItemOffset += lengthI;
        pList->nItemID = item.Identifier;
        pList->nItemLen = item.Length - 1;
        pList->pItemStr = pOffset + (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST));
        //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pList->nItemID = 0x%x",0x08100d8b), pList->nItemID);
        SUL_MemCopy8(pList->pItemStr, item.TextString, item.Length - 1);
        //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIZE = %x",0x08100d8c), (UINT8)SIZEOF(CFW_SAT_ITEM_LIST) + MEM_ALIGN_SIZE(item.Length - 1));
        pOffset += (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST)) + MEM_ALIGN_SIZE(item.Length - 1);
        //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pOffset = %x",0x08100d8d), pOffset);
        if( i == ItemNum - 1)
            pList->pNextItem = NULL;
        else
            pList->pNextItem = (CFW_SAT_ITEM_LIST *)pOffset;

        pList = (CFW_SAT_ITEM_LIST *)pOffset;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("End address = %x",0x08100d8e), pOffset);
    //  pList = (*pMenuItemList)->pItemList;
    //  while(pList != NULL)
    //  {
    //      CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pList->nItemID = %x",0x08100d8f), pList->nItemID);
    //      pList=pList->pNextItem;
    //  }
    return ERR_SUCCESS;
}

UINT32 fetchSelectItem(CFW_SAT_MENU_RSP **pMenuItemList, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI  = 0;
    UINT8 lengthI = 0;
    UINT8 lengthITI = 0;
    UINT8 lengthINAI = 0;
    UINT8 lengthII = 0;
    UINT8 lengthIIIL = 0;

    UINT8 *pOffset = pInput;
    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_MENU_RSP));

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("fetchSelectItem",0x08100d90));
    if((pInput == NULL) || nLength == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("fetchSelectItem (pInput == NULL) || pInput == NULL) pInput =%x,nLength = %d",0x08100d91), pInput, nLength);
        return ERR_PARAM_ILLEGAL;
    }
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    struct AlphaIden alphaiden;
    if((lengthAI = getAlphaIden(pOffset, &alphaiden)) != 0)
    {
        pOffset += lengthAI;
        nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("alphaiden.Length = %d,lengthAI = %d",0x08100d92), alphaiden.Length, lengthAI);
    }

    struct Item item;
    if(nLength < pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    UINT8 i = 0;
    UINT8 *pItemOffset = pOffset;
    UINT8 ItemNum  = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize = %x",0x08100d93), nMemsize);

    while(nLength > pOffset - pInput)
    {
        if((lengthI = getItem(pOffset, &item)) == 0)
            break;
        ItemNum++;
        pOffset += lengthI;
        nMemsize += (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST)) + (UINT8) MEM_ALIGN_SIZE(item.Length - 1);
        i += item.Length;
        //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nMemsize = %d,item.Length = %d",0x08100d94), nMemsize, item.Length);
    }

    struct ItemsNAI itemnai;
    if(nLength > pOffset - pInput)
    {
        lengthINAI = getItemsNextActionIndicator(pOffset, &itemnai);
        if(lengthINAI != 0)
        {
            pOffset += lengthINAI;
            nMemsize += MEM_ALIGN_SIZE(itemnai.Length);
        }
    }

    struct ItemIdentifier itemiden;
    if(nLength > pOffset - pInput)
    {
        lengthITI = getItemIdentifier(pOffset, &itemiden);
        if(lengthITI != 0)
        {
            pOffset += lengthITI;
        }
    }
    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
    {
        lengthII = getIconIdentifier(pOffset, &iconiden);
        if(lengthII != 0)
            pOffset += lengthII;
    }

    struct ItemIconIdenList itemiconidenlist;
    if(nLength > pOffset - pInput)
    {
        lengthIIIL = getItemIconIdenList(pOffset, &itemiconidenlist);
        if(lengthIIIL != 0)
            nMemsize += MEM_ALIGN_SIZE(itemiconidenlist.Length - 1);
    }
    *pMenuItemList = (CFW_SAT_MENU_RSP *)CSW_SIM_MALLOC(nMemsize);
    pOffset = (UINT8 *)(*pMenuItemList) + (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_MENU_RSP));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("*pMenuItemList start address = %x",0x08100d95), *pMenuItemList);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nMemsize = %x",0x08100d96), nMemsize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("lengthAI = %d",0x08100d97), lengthAI);

    if(lengthAI != 0)
    {
        (*pMenuItemList)->nAlphaLen = alphaiden.Length;
        (*pMenuItemList)->pAlphaStr = pOffset;
        SUL_MemCopy8((*pMenuItemList)->pAlphaStr, alphaiden.AlphaIdentifier, alphaiden.Length);
        pOffset += MEM_ALIGN_SIZE(alphaiden.Length);
    }
    else
    {
        (*pMenuItemList)->nAlphaLen = 0;
        (*pMenuItemList)->pAlphaStr = NULL;
    }
    if(ItemNum == 0)
        (*pMenuItemList)->pItemList = NULL;
    else
    {
        (*pMenuItemList)->pItemList = (CFW_SAT_ITEM_LIST *)pOffset;

        CFW_SAT_ITEM_LIST *pList = (CFW_SAT_ITEM_LIST *)pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("1pList = %x",0x08100d98), pList);
        for(i = 0; i < ItemNum; i++)
        {
            lengthI = getItem(pItemOffset, &item);
            pItemOffset += lengthI;
            pList->nItemID = item.Identifier;
            pList->nItemLen = item.Length - 1;
            pList->pItemStr = pOffset + (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST));
            //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pList->nItemID = 0x%x",0x08100d99), pList->nItemID);
            SUL_MemCopy8(pList->pItemStr, item.TextString, item.Length - 1);
            //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIZE = %x",0x08100d9a), (UINT8)SIZEOF(CFW_SAT_ITEM_LIST) + MEM_ALIGN_SIZE(item.Length - 1));
            pOffset += (UINT8)MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_ITEM_LIST)) + MEM_ALIGN_SIZE(item.Length - 1);
            //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pOffset = %x",0x08100d9b), pOffset);
            if( i == ItemNum - 1)
                pList->pNextItem = NULL;
            else
                pList->pNextItem = (CFW_SAT_ITEM_LIST *)pOffset;
            pList = (CFW_SAT_ITEM_LIST *)pOffset;
        }
    }
    //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("End address = %x",0x08100d9c), pOffset);
    //pList = (*pMenuItemList)->pItemList;
    //while(pList != NULL)
    //{
    //  CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pList->nItemID = %x",0x08100d9d), pList->nItemID);/
    //  pList=pList->pNextItem;
    //}
    return ERR_SUCCESS;
}

UINT32 fetchDisplayText(CFW_SAT_DTEXT_RSP **pDTextResp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthTS = 0;
    UINT8 lengthII  = 0;
    UINT8 lengthIRSP = 0;
    UINT8 *pOffset = pInput;
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;

    struct TextString textstring;
    if((lengthTS = getTextString(pInput, &textstring)) == 0)
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthTS;

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
    {
        lengthII = getIconIdentifier(pOffset, &iconiden);
        if(lengthII != 0)
            pOffset += lengthII;
    }
    struct ImmediateRSP immrsp;
    if(nLength > pOffset - pInput)
    {
        lengthIRSP = getImmediateRSP(pOffset, &immrsp);
    }
    UINT16 nMemSize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_DTEXT_RSP)) + textstring.Length - 1;
    *pDTextResp = (CFW_SAT_DTEXT_RSP *)(CSW_SIM_MALLOC(nMemSize));
    if (*pDTextResp == NULL)
        return ERR_NO_MORE_MEMORY;
    SUL_ZeroMemory8(*pDTextResp, (SIZEOF(CFW_SAT_DTEXT_RSP) + textstring.Length - 1));

    (*pDTextResp)->nTextSch      = textstring.DataCodingScheme;
    (*pDTextResp)->nTextLen      = textstring.Length - 1;
    (*pDTextResp)->pTextStr      = (UINT8 *)(*pDTextResp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_DTEXT_RSP));
    SUL_MemCopy8((*pDTextResp)->pTextStr, textstring.TextString, textstring.Length - 1);

    return ERR_SUCCESS;
}

UINT32 fetchMoreTime(UINT8 *pInput, UINT8 nLength)
{
    return ERR_SUCCESS;
}

UINT32 fetchGetInput(CFW_SAT_INPUT_RSP **pInputResp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthTS = 0;
    UINT8 lengthRL = 0;
    UINT8 lengthDT = 0;
    UINT8 lengthII = 0;
    UINT8 *pOffset = pInput;

    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;

    struct TextString textstring;
    if((lengthTS = getTextString(pOffset, &textstring)) == 0)
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthTS;

    struct ResponseLength responselength;
    if(nLength < pOffset - pInput)
        return ERR_PARAM_ILLEGAL;
    if((lengthRL = getResponseLength(pOffset, &responselength)) == 0)
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthRL;

    struct DefaultText defaulttext;
    if(nLength > pOffset - pInput)
    {
        if((lengthDT = getDefaultText(pOffset, &defaulttext)) != 0)
        {
            pOffset += lengthDT;
        }
    }
    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
    {
        lengthII = getIconIdentifier(pOffset, &iconiden);
    }
    UINT16 nMemSize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_INPUT_RSP));
    nMemSize += MEM_ALIGN_SIZE(textstring.Length) + defaulttext.Length;

    *pInputResp = (CFW_SAT_INPUT_RSP*)(CSW_SIM_MALLOC(nMemSize));
    if (*pInputResp == NULL)
        return ERR_NO_MORE_MEMORY;
    SUL_ZeroMemory8(*pInputResp, nMemSize);
    pOffset = (UINT8*)(*pInputResp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_INPUT_RSP));


    (*pInputResp)->nTextSch      = textstring.DataCodingScheme;
    (*pInputResp)->nTextLen      = textstring.Length;
    (*pInputResp)->pTextStr      = pOffset;
    SUL_MemCopy8((*pInputResp)->pTextStr, textstring.TextString, textstring.Length);
    pOffset += MEM_ALIGN_SIZE(textstring.Length);

    (*pInputResp)->nResMinLen    = responselength.MiniLen;
    (*pInputResp)->nResMaxLen    = responselength.MaxLen;

    if (lengthDT != 0)
    {
        (*pInputResp)->nDefTextLen = defaulttext.Length;
        (*pInputResp)->nDefTextSch = defaulttext.DataCodingScheme;
        (*pInputResp)->pDefTextStr = pOffset;
        SUL_MemCopy8((*pInputResp)->pDefTextStr, defaulttext.TextString, defaulttext.Length);
    }
    else
    {
        (*pInputResp)->nDefTextLen = 0;
        (*pInputResp)->nDefTextSch = 0;
        (*pInputResp)->pDefTextStr = NULL;
    }
    return ERR_SUCCESS;
}

UINT32 fetchSendSMS(CFW_SAT_SMS_RSP **pSmsResp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI = 0;
    UINT8 lengthA = 0;
    UINT8 lengthSMS = 0;
    UINT8 lengthII = 0;
    UINT8 *pOffset = pInput;

    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_SMS_RSP));

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchSendSMS: *pOffset = 0x%x\n",0x08100d9e), *pOffset);
    struct AlphaIden alphaiden;
    lengthAI = getAlphaIden(pOffset, &alphaiden);
    if(lengthAI != 0)
    {
        pOffset += lengthAI;
        if(alphaiden.Length != 0)
            nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("alphaiden.Length = %x",0x08100d9f),alphaiden.Length);
    }
    else
        alphaiden.Length = 0;

    struct Address address;
    if(nLength > pOffset - pInput)
    {
        lengthA = getAddress(pOffset, &address);
        if(lengthA != 0)
        {
            pOffset += lengthA;
            if(address.Length  > 1)
                nMemsize += MEM_ALIGN_SIZE(address.Length - 1); // one byte TON and NPI
        }
        else
            address.Length = 0;
    }

    struct SMSTPDU smstpdu;
    if(nLength <= pOffset - pInput)
        return ERR_PARAM_ILLEGAL;
    if((lengthSMS = getSMSTPDU(pOffset, &smstpdu)) == 0)
        return ERR_PARAM_ILLEGAL;

    nMemsize += MEM_ALIGN_SIZE(smstpdu.Length);
    pOffset += lengthSMS;

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
        lengthII = getIconIdentifier(pOffset, &iconiden);

    *pSmsResp = (CFW_SAT_SMS_RSP *)CSW_SIM_MALLOC(nMemsize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SMS PDUString Mem size=%d,*pSmsResp  = %x\n",0x08100da1), nMemsize, (*pSmsResp));
    if (*pSmsResp == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8(*pSmsResp, nMemsize);

    pOffset = (UINT8 *)(*pSmsResp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_SMS_RSP));
    if(alphaiden.Length  != 0)
    {
        (*pSmsResp)->nAlphaLen = alphaiden.Length;
        (*pSmsResp)->pAlphaStr = pOffset;
        SUL_MemCopy8((*pSmsResp)->pAlphaStr, alphaiden.AlphaIdentifier, alphaiden.Length);
        pOffset += MEM_ALIGN_SIZE(alphaiden.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: AlphaIdentifier Length=%d",0x08100da2), (*pSmsResp)->nAlphaLen);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: (*pSmsResp)->pAlphaStr=%x\n",0x08100da3), (*pSmsResp)->pAlphaStr);
    }

    if(address.Length  > 1)
    {
        (*pSmsResp)->nAddrLen  = address.Length - 1;    //one byte TON and NPI
        (*pSmsResp)->nAddrType = address.TONandNPI;
        (*pSmsResp)->pAddrStr = pOffset;
        SUL_MemCopy8((*pSmsResp)->pAddrStr, address.DiallingNumber, address.Length - 1);    //one byte TON and NPI
        pOffset += MEM_ALIGN_SIZE(address.Length - 1);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: nAddrLen=%d\n",0x08100da4), (*pSmsResp)->nAddrLen);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: nAddrType=%d\n",0x08100da5),  (*pSmsResp)->nAddrType);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS:  (*pSmsResp)->pAddrStr =%x\n",0x08100da6), (*pSmsResp)->pAddrStr);
    }

    (*pSmsResp)->nPDULen = smstpdu.Length;
    (*pSmsResp)->pPDUStr = pOffset;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: Address Length=%d\n",0x08100da7),  address.Length - 1);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: (*pSmsResp)->pPDUStr=%x\n",0x08100da8), (*pSmsResp)->pPDUStr);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("-->Fetch SMS: SMSTPDU Length=%d\n",0x08100da9),  smstpdu.Length);
    SUL_MemCopy8((*pSmsResp)->pPDUStr, smstpdu.SMSTPDU, smstpdu.Length);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("(*pSmsResp)->pPDUStr[2] =%x",0x08100daa), (*pSmsResp)->pPDUStr[2]);
#if 0
    // hameina [+] 2008.1SAT
    // check the DCS and tpdu, if dcs is 7 bits and all the tpdu bit7 is 0, it means the DCS is not equal to the data,
    // we need to transfer the 8 bit data to 7 bit.

    UINT8 uDCSOffsize;
    UINT8 uVPLen;
    UINT8 uVPF;
    UINT8 UdataOff   = 0;
    UINT8 buff7[140] = { 0, };
    UINT8 buff7len   = 0;

    // MTI+MR+DALEN/2+DATYPE+PID + DCS
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("1",0x08100dab));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("(*pSmsResp)->pPDUStr[2] =%x",0x08100dac), (*pSmsResp)->pPDUStr[2]);
    uDCSOffsize = 2 + ((*pSmsResp)->pPDUStr[2] + 1) / 2 + 1 + 2;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("2",0x08100dad));

    // bit3,4 of MTI is vpf
    uVPF = ((*pSmsResp)->pPDUStr[0] >> 3) & 3;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("3",0x08100dae));

    if (!uVPF)  // NOT present
        uVPLen = 0;
    else if (uVPF == 2) // relative format
        uVPLen = 1;
    else
        uVPLen = 7;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("4",0x08100daf));

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("uDCSOffsize=%d,pPDUPacketTmp[uDCSOffsize]=0x%02x, uVPLen=%d\n",0x08100db0),
              uDCSOffsize, (*pSmsResp)->pPDUStr[uDCSOffsize], uVPLen);

    // dcs offsize + udl+1-->userdata
    // cause VPF is 0, it means vp field is not present, so when test, we don't count this, othersize VP will take 1~7 octs.
    UdataOff = uDCSOffsize + 2 + uVPLen;

    // check if match the condition 1, DCS is 0
    UINT8 nDCS = (*pSmsResp)->pPDUStr[uDCSOffsize];
    sms_tool_CheckDcs(&nDCS);
    if (!nDCS)
    {
        UINT8 uLoop = 0;

        // here need to check whether all the bit7 of userdata is 0,
        for (uLoop = 0; uLoop < (*pSmsResp)->pPDUStr[UdataOff - 1]; uLoop++)
        {
            if (((*pSmsResp)->pPDUStr[UdataOff + uLoop] >> 7) & 1)
                break;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("uLoop=%d, pSmsResp->pPDUStr[UdataOff-1]=%d\n",0x08100db1), uLoop,
                  (*pSmsResp)->pPDUStr[UdataOff - 1]);

        // check if match the condition 2: all the bit 7 of user data is 0.
        if(0)// (uLoop && (uLoop == pSmsResp->pPDUStr[UdataOff - 1]))
        {
            buff7len = SUL_Encode7Bit(&((*pSmsResp)->pPDUStr[UdataOff]), buff7, (*pSmsResp)->pPDUStr[UdataOff - 1]);
            SUL_MemCopy8(&((*pSmsResp)->pPDUStr[UdataOff]), buff7, buff7len);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("UdataOff=%d, buff7len= %d\n",0x08100db2), UdataOff, buff7len);

            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen before = %d\n",0x08100db3), (*pSmsResp)->nPDULen);
            (*pSmsResp)->nPDULen +=  (buff7len - (*pSmsResp)->pPDUStr[UdataOff - 1]);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen end = %d\n",0x08100db4), (*pSmsResp)->nPDULen);

        }
    }
#if 0
    // 8 bit data, change to 7 bit data
    //else if (4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0xf)) // padding for UMCC mobile mailbox
    else if ((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0f)) || (0xf2 == (pSmsResp->pPDUStr[uDCSOffsize]))) // padding for UMCC mobile mailbox
    {
        //if ((2 == pSmsResp->pPDUStr[UdataOff - 1] && pSmsResp->pPDUStr[UdataOff] == 0x4d) ||(0xf2 == (pSmsResp->pPDUStr[uDCSOffsize])))  // && pSmsResp->pPDUStr[UdataOff+1]== 0x50
        if ((2 == pSmsResp->pPDUStr[UdataOff - 1] && pSmsResp->pPDUStr[UdataOff] == 0x4d) )  // && pSmsResp->pPDUStr[UdataOff+1]== 0x50
        {
            // change DCS to 0, 7  bit decode
            pSmsResp->pPDUStr[uDCSOffsize] = 0;

            // encode user data to 7 bit data
            buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
            SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("UdataOff=%d, buff7len= %d\n",0x08100db5), UdataOff, buff7len);
        }
    }
#endif
    else if(0xF2 == (*pSmsResp)->pPDUStr[uDCSOffsize])
    {
        // change DCS to 0, 7  bit decode
        (*pSmsResp)->pPDUStr[uDCSOffsize] = 0;

        // encode user data to 7 bit data
        buff7len = SUL_Encode7Bit(&(*pSmsResp)->pPDUStr[UdataOff], buff7, (*pSmsResp)->pPDUStr[UdataOff - 1]);
        SUL_MemCopy8(&(*pSmsResp)->pPDUStr[UdataOff], buff7, buff7len);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("UdataOff=%d, buff7len= %d\n",0x08100db6), UdataOff, buff7len);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen before = %d\n",0x08100db7), (*pSmsResp)->nPDULen);
        (*pSmsResp)->nPDULen +=  (buff7len - (*pSmsResp)->pPDUStr[UdataOff - 1]);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen end = %d\n",0x08100db8), (*pSmsResp)->nPDULen);
    }
#if 0 //Remove by Lixp For  Madagascar stk sms issue at 20140707
    else if((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0F)) && (0x00 == pSmsResp->pPDUStr[uDCSOffsize - 1]))
    {
        // change DCS to 0, 7  bit decode
        pSmsResp->pPDUStr[uDCSOffsize] = 0;

        // encode user data to 7 bit data
        buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
        SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("UdataOff=%d, buff7len= %d\n",0x08100db9), UdataOff, buff7len);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen before = %d\n",0x08100dba), pSmsResp->nPDULen);
        pSmsResp->nPDULen +=  (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSmsResp->nPDULen end = %d\n",0x08100dbb), pSmsResp->nPDULen);
    }
#endif
#endif

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) (*pSmsResp)->pPDUStr, (UINT16)smstpdu.Length, 16);
    return ERR_SUCCESS;
}


UINT32 fetchCallSetup(CFW_SAT_CALL_RSP **pCallResp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAIUC = 0;
    UINT8 lengthAICSU = 0;
    UINT8 lengthA = 0;
    UINT8 lengthCCP = 0;
    UINT8 lengthSA = 0;
    UINT8 lengthD = 0;
    UINT8 lengthIIUC = 0;
    UINT8 lengthIICSU = 0;
    UINT8 *pOffset = pInput;

    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;

    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_CALL_RSP));

    struct AlphaIden alphaidenUC;
    lengthAIUC = getAlphaIden(pOffset, &alphaidenUC);
    pOffset += lengthAIUC;
    if(lengthAIUC != 0)
        nMemsize += MEM_ALIGN_SIZE(alphaidenUC.Length);

    if(nLength < pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    struct Address address;
    if((lengthA = getAddress(pOffset, &address)) == 0)
        return ERR_PARAM_ILLEGAL;
    nMemsize += MEM_ALIGN_SIZE(address.Length - 1); //one byte TON and NPI
    pOffset += lengthA;

    struct CapabilityCP capabilitycp;
    if(nLength > pOffset - pInput)
    {
        lengthCCP = getCapabilityCP(pOffset, &capabilitycp);
        if(lengthCCP != 0)
        {
            pOffset += lengthCCP;
            nMemsize += MEM_ALIGN_SIZE(capabilitycp.Length);
        }
    }
    struct SubAddress subaddress;
    if(nLength > pOffset - pInput)
    {
        lengthSA = getSubAddress(pOffset, &subaddress);
        if(lengthSA != 0)
        {
            pOffset += lengthSA;
            nMemsize += MEM_ALIGN_SIZE(subaddress.Length);
        }
    }

    struct Duration duration;
    if(nLength > pOffset - pInput)
    {
        lengthD = getDuration(pOffset, &duration);
        if(lengthD != 0)
        {
            pOffset += lengthD;
        }
    }
    struct IconIden iconidenUC;
    if(nLength > pOffset - pInput)
    {
        lengthIIUC = getIconIdentifier(pOffset, &iconidenUC);
        if(lengthIIUC != 0)
        {
            pOffset += lengthIIUC;
        }
    }
    struct AlphaIden alphaidenCSU;
    if(nLength > pOffset - pInput)
    {
        lengthAICSU = getAlphaIden(pOffset, &alphaidenCSU);
        if(lengthAICSU != 0)
        {
            pOffset += lengthAICSU;
            nMemsize += MEM_ALIGN_SIZE(alphaidenCSU.Length);
        }
    }
    struct IconIden iconidenCSU;
    if(nLength > pOffset - pInput)
    {
        lengthIICSU = getIconIdentifier(pOffset, &iconidenCSU);
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: nMemsize = %d\n",0x08100dbc), nMemsize);

    *pCallResp = (CFW_SAT_CALL_RSP *)CSW_SIM_MALLOC(nMemsize);
    if (*pCallResp == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8(*pCallResp, nMemsize);
    pOffset =  (UINT8 *)(*pCallResp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_CALL_RSP));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: pOffset = %x\n",0x08100dbd), pOffset);

    if(lengthAIUC != 0)
    {
        (*pCallResp)->nAlphaConfirmLen = alphaidenUC.Length;
        (*pCallResp)->pAlphaConfirmStr = pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: pAlphaConfirmStr = %x\n",0x08100dbe), (*pCallResp)->pAlphaConfirmStr);

        SUL_MemCopy8((*pCallResp)->pAlphaConfirmStr, alphaidenUC.AlphaIdentifier, alphaidenUC.Length);
        pOffset += MEM_ALIGN_SIZE(alphaidenUC.Length);
    }
    else
    {
        (*pCallResp)->nAlphaConfirmLen = 0;
        (*pCallResp)->pAlphaConfirmStr = NULL;
    }

    if((address.Length -1) != 0)
    {
        (*pCallResp)->nAddrLen  = address.Length - 1;
        (*pCallResp)->nAddrType = address.TONandNPI;
        (*pCallResp)->pAddrStr  = pOffset;
        SUL_MemCopy8((*pCallResp)->pAddrStr, address.DiallingNumber, address.Length - 1);
        pOffset += MEM_ALIGN_SIZE(address.Length - 1);
    }
    else
    {
        (*pCallResp)->nAddrLen = 0;
        (*pCallResp)->nAddrType = 0;
        (*pCallResp)->pAddrStr = NULL;
    }

    if(lengthCCP != 0)
    {
        (*pCallResp)->nCapabilityCfgLen = capabilitycp.Length;
        (*pCallResp)->pCapabilityCfgStr = pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: pCapabilityCfgStr = %x\n",0x08100dbf), (*pCallResp)->pCapabilityCfgStr);

        SUL_MemCopy8((*pCallResp)->pCapabilityCfgStr, capabilitycp.CapabilityCP, capabilitycp.Length);
        pOffset += MEM_ALIGN_SIZE(capabilitycp.Length);
    }
    else
    {
        (*pCallResp)->nCapabilityCfgLen = 0;
        (*pCallResp)->pCapabilityCfgStr = NULL;
    }

    if(lengthSA != 0)
    {
        (*pCallResp)->nSubAddrLen = subaddress.Length;
        (*pCallResp)->pSubAddrStr = pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: pSubAddrSt = %x\n",0x08100dc0), (*pCallResp)->pSubAddrStr);

        SUL_MemCopy8((*pCallResp)->pSubAddrStr, subaddress.SubAddress, subaddress.Length);
        pOffset += MEM_ALIGN_SIZE(subaddress.Length);
    }
    else
    {
        (*pCallResp)->nSubAddrLen = 0;
        (*pCallResp)->pSubAddrStr = NULL;
    }

    if(lengthD != 0)
    {
        (*pCallResp)->nTuint     = duration.TimeUnit;
        (*pCallResp)->nTinterval = duration.TimeInterval;
    }
    else
    {
        (*pCallResp)->nTuint     = 0;
        (*pCallResp)->nTinterval = 0;
    }

    if(lengthAICSU != 0)
    {
        (*pCallResp)->nAlphaSetupLen = alphaidenCSU.Length;
        (*pCallResp)->pAlphaSetupStr = pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("fetchCallSetup: pAlphaSetupStr = %x\n",0x08100dc1), (*pCallResp)->pAlphaSetupStr);

        SUL_MemCopy8((*pCallResp)->pAlphaSetupStr, alphaidenCSU.AlphaIdentifier, alphaidenCSU.Length);
    }
    else
    {
        (*pCallResp)->nAlphaSetupLen = 0;
        (*pCallResp)->pAlphaSetupStr = NULL;
    }
    return ERR_SUCCESS;
}



#if 0
UINT32 fetchSetupEventList(CFW_SAT_RESPONSE *pSimSatResponseData,
                           SIM_SAT_PARAM *pG_SimSat, CFW_EV *pEvent, UINT8 nSimID)
{
    //  api_SimFetchInd_t *pSimFetchInd = (api_SimFetchInd_t *)pEvent->nParam1;
    //        UINT8 *pOffset = pSimFetchInd->Data;

    SAT_BASE_RSP *pBaseResp = (SAT_BASE_RSP *)CSW_SIM_MALLOC(SIZEOF(SAT_BASE_RSP));
    if (pBaseResp == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8(pBaseResp, (SIZEOF(SAT_BASE_RSP)));
    pBaseResp->nComID        = pG_SimSat->nCurCmd;
    pBaseResp->nComQualifier = pG_SimSat->nCmdQualifier;

    if (pG_SimSat->pSatComData != NULL)
    {
        if(pG_SimSat->nCurCmd == SIM_SAT_SELECT_ITEM_COM)
        {
            CFW_SAT_MENU_RSP *p = (CFW_SAT_MENU_RSP *)pG_SimSat->pSatComData;
            CSW_SIM_FREE(p->pItemList);
        }
        CSW_SIM_FREE(pG_SimSat->pSatComData);
    }
    pG_SimSat->pSatComData = pBaseResp;

    // #ifdef CFW_MULTI_SIM
    // CFW_SatResponse(pSimSatResponseData->nSAT_CmdType, 0x00, 0x00, NULL, 0x00, nRUti,nSimID);
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    SAT_BASE_RSP *pBase = (SAT_BASE_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pBase == NULL))
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;

    // SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, pSimTerminalResponesReq->NbByte);
#if 0
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponseData->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    // pSimTerminalResponesReq->TerminalRespData[ComType -1]   = 0x02;

    pSimTerminalResponesReq->TerminalRespData[ComType]   = 0x05;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = 0x00;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_CmdType    %x\n",0x08100dc2), pSimSatResponseData->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier   %x\n",0x08100dc3), pBase->nComQualifier);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status     %x\n",0x08100dc4), pSimSatResponseData->nSAT_Status);

    // CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq, sizeof(api_SimTerminalResponseReq_t), 16);
    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID))
        return ERR_SCL_QUEUE_FULL;

    // CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pSimSatResponseData->nSAT_CmdType, 0, nUTI|(nSimID<<24), 0x0);
    // #else
    // CFW_SatResponse(pSimSatResponseData->nSAT_CmdType, 0x00, 0x00, NULL, 0x00, nRUti);
    // CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pSimSatResponseData->nSAT_CmdType, 0, nUTI, 0x0);
    // #endif
    return ERR_SUCCESS;
}

#else

UINT32 fetchSetupEventList(CFW_SAT_EVENTLIST_RSP **pEventList, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthEL = 0;
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;

    struct EventList eventlist;
    if((lengthEL = getEventList(pInput, &eventlist)) == 0)
    {
        if((*pInput & 0x7F) != EVENT_LIST_TAG)
            return ERR_PARAM_ILLEGAL;
    }
    UINT8 nSize = SIZEOF(CFW_SAT_EVENTLIST_RSP);
    *pEventList = (CFW_SAT_EVENTLIST_RSP *)CSW_SIM_MALLOC(nSize);
    if(*pEventList == NULL)
        return ERR_NO_MORE_MEMORY;
    SUL_MemSet8(*pEventList, 0, nSize);
    if(lengthEL != 0)
    {
        (*pEventList)->nNumEvent = eventlist.Length;
        SUL_MemCopy8((*pEventList)->nEventList, eventlist.EventList, eventlist.Length);
    }
    return ERR_SUCCESS;
}
#endif

UINT32 fetchRefresh(CFW_SAT_FILELIST_RSP **pFileList,
                    UINT8 *pInput, UINT8 nLength, UINT8 nCmdQualifier, CFW_SIM_ID nSimID)
{
    UINT8 lengthFL = 0;
    UINT8 *pOffset = pInput;

    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    if((nCmdQualifier == REFRESH_FILE_CHANGE) ||(nCmdQualifier == REFRESH_SIM_INIT_FILE_CHANGE))
    {
        struct FileList filelist;
        lengthFL = getFileList(pOffset, &filelist);
        //'00' =SIM Initialization and Full File Change Notification;
        //'01' = File Change Notification;
        //'02' = SIM Initialization and File Change Notification;
        //'03' = SIM Initialization;
        //'04' = SIM Reset;
        //'05' to 'FF' = reserved values.
        //For the refresh modes "File Change Notification" and "SIM Initialization and File Change Notification",
        //the SIM shall supply a File List data object, indicating which EFs need to be refreshed.
        //For other modes, inclusion of a File List is optional, and the ME shall ignore it.
        if (lengthFL == 0)
            return ERR_PARAM_ILLEGAL;
        else
        {
            UINT8 memsize = SIZEOF(CFW_SAT_FILELIST_RSP) + filelist.Numberoffiles;
            *pFileList = (CFW_SAT_FILELIST_RSP *)CSW_SIM_MALLOC(memsize);
            if (*pFileList == NULL)
                return ERR_NO_MORE_MEMORY;

            SUL_ZeroMemory8(*pFileList, memsize);
            (*pFileList)->nMode = nCmdQualifier;
            UINT8 k = filelist.Numberoffiles;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "k = %d, filelist.Files[0] = %02x", k, filelist.Files[0]);
            if((filelist.Files[0] == 0x3F) && ( k > 0))
            {
                INT8 i = filelist.Length - 1;
                INT8 j = 0;
                for(; i > 3 ; i--)  //i should be bigger than 4
                {
                    INT8 j = i - 1;
                    UINT16 EFFile = (filelist.Files[j -1] << 8) | filelist.Files[j];
                    UINT16 EFPath = (filelist.Files[j - 3] << 8) | filelist.Files[j - 2];

                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "EFFile = %04x, EFPath = %04x", EFFile, EFPath);
                    UINT16 FileID = CFW_GetStackSimFileID(EFFile, EFPath, nSimID);
                    if(0xFFFF == FileID)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "FileID(0x%04x) cannot be found!", EFFile);
                        break;
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "FileID = 0x%02x", FileID);
                    (*pFileList)->nFiles[k-1] = FileID;
                    k--;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "i = %d!", i);
                    i -= 2; //remove last file id

                    while((i > 3) && ( filelist.Files[i - 1] != 0x3F))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "filelist.Files[i - 1]= %x!",filelist.Files[i - 1]);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "--i = %d!", i);
                        i--;
                    }
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "k = %d", k);
                if(k != 0)
                {
                    for(j = k; j < filelist.Numberoffiles; j++)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "(*pFileList)->nFiles[%d] = %02x", k, (*pFileList)->nFiles[k] );
                        (*pFileList)->nFiles[j - k] = (*pFileList)->nFiles[j];
                    }
                }
                (*pFileList)->nNumFiles = filelist.Numberoffiles - k;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "(*pFileList)->nNumFiles  = %d", (*pFileList)->nNumFiles );
            }
            else
            {
                (*pFileList)->nNumFiles = 0;
            }
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_REFRESH_IND: File Change, nCmdQualifier = %d",0x08100dc5),nCmdQualifier);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====(*pFileList)->nNumFiles = %d===",(*pFileList)->nNumFiles);
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (*pFileList)->nFiles, (*pFileList)->nNumFiles, 16);
    }
    else
    {
        *pFileList = (CFW_SAT_FILELIST_RSP*)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_FILELIST_RSP));
        if (*pFileList == NULL)
            return ERR_NO_MORE_MEMORY;
        (*pFileList)->nMode= nCmdQualifier;
        (*pFileList)->nNumFiles = 0;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_REFRESH_IND: nCmdQualifier = %d",0x08100dc6),nCmdQualifier);
    }
#if 0
    UINT8 nIndex = 0;
    UINT8 nLength = 0;
    UINT8 nNumOfFiles = 0x00;
    UINT8 *pFiles = NULL;

    /*
    '00' =SIM Initialization and Full File Change Notification;
    '01' = File Change Notification;
    '02' = SIM Initialization and File Change Notification;
    '03' = SIM Initialization;
    '04' = SIM Reset;
    '05' to 'FF' = reserved values.
    */
    if( 0x00 == nCmdQualifier)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_FRESH_IND    nComQualifier 0x00 re attachment NW\n",0x08100dc7));
        //TODO
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);

        //UINT8 nUTI = 0x00;
        // CFW_GetFreeUTI(0, &nUTI);

        UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x01;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n",0x08100dc8), nRet, nSimID, gSatFreshComm[nSimID]);
        return ERR_SUCCESS;

    }
    else if( 0x01 == nCmdQualifier)
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {

            if (pOffset[++nIndex] == 0x81)
                nIndex++;

            nLength     = pOffset[nIndex++];
            nNumOfFiles   = pOffset[nIndex++];

            pFiles = (UINT8 *)CSW_SIM_MALLOC(nLength);

            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);

            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);
            //UINT8 nUTI = 0x00;
            //CFW_GetFreeUTI(0, &nUTI);

            UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x11;

            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n",0x08100dc9), nRet, nSimID, gSatFreshComm[nSimID]);

            //CFW_SetComm(CFW_ENABLE_COMM,1,22,nSimID);
            return ERR_SUCCESS;
        }
        else
            return ERR_PARAM_ILLEGAL;
    }
    else if( 0x02 == nCmdQualifier)
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {
            if (pOffset[++nIndex] == 0x81)
                nIndex++;

            nLength     = pOffset[nIndex++];
            nNumOfFiles   = pOffset[nIndex++];

            pFiles = (UINT8 *)CSW_SIM_MALLOC(nLength);

            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);

            // UINT8 nUTI = 0x00;
            //CFW_GetFreeUTI(0, &nUTI);
            UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x21;

            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n",0x08100dca), nRet, nSimID, gSatFreshComm[nSimID]);
            return ERR_SUCCESS;
        }
        else
            return ERR_PARAM_ILLEGAL;
    }
    else if( 0x03 == nCmdQualifier)
    {
        //Add by Lixp at 20130721
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);

        //UINT8 nUTI = 0x00;
        //CFW_GetFreeUTI(0, &nUTI);
        UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x31;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n",0x08100dcb), nRet, nSimID, gSatFreshComm[nSimID]);
        return ERR_SUCCESS;
    }
    else if( 0x04 == nCmdQualifier)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SimInit[%d] %d\n",0x08100dcc), nSimID);
        gSatFreshComm[nSimID] = 0x41;
        CFW_SimInit(0, nSimID );
        return ERR_SUCCESS;
    }
    else
    {
        //Reserved VALUES
    }
#endif
    return ERR_SUCCESS;
}

UINT32 fetchPollInterval(CFW_SAT_POLL_INTERVAL_RSP **pPIResp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthD = 0x00;
    UINT8 *pOffset = pInput;
    struct Duration duration;
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    lengthD = getDuration(pOffset, &duration);
    if(lengthD == 0)
        return ERR_PARAM_ILLEGAL;
    *pPIResp = (CFW_SAT_POLL_INTERVAL_RSP *)(CSW_SIM_MALLOC(SIZEOF(CFW_SAT_POLL_INTERVAL_RSP)));
    if (*pPIResp == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8((*pPIResp), (SIZEOF(CFW_SAT_POLL_INTERVAL_RSP)));
    (*pPIResp)->nTimeUnit     = duration.TimeUnit;
    (*pPIResp)->nTimeInterval = duration.TimeInterval;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nTimeUnit 0x%x,nTimeInterval 0x%x\n",0x08100dcd), duration.TimeUnit, duration.TimeInterval);
    return ERR_SUCCESS;
}


UINT32 fetchSendSS(CFW_SAT_SS_RSP **pSSRsp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI = 0;
    UINT8 lengthII = 0;
    UINT8 lengthUSSD = 0;
    UINT8 *pOffset = pInput;

    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_SS_RSP));
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    struct AlphaIden alphaiden;
    lengthAI = getAlphaIden(pOffset, &alphaiden);
    if(lengthAI != 0)
    {
        pOffset += lengthAI;
        nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
    }

    struct SSString ss;
    if(nLength <= pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    if((lengthUSSD = getSSString(pOffset, &ss)) == 0)
        return ERR_PARAM_ILLEGAL;
    nMemsize += MEM_ALIGN_SIZE(ss.Length - 1);      //one byte scheme
    pOffset += lengthUSSD;

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
        lengthII = getIconIdentifier(pOffset, &iconiden);

    (*pSSRsp) = (CFW_SAT_SS_RSP *)CSW_SIM_MALLOC(nMemsize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SS Send Mem size=%d\n",0x08100dce), nMemsize);
    if ((*pSSRsp) == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8((*pSSRsp) , nMemsize);
    pOffset =  (UINT8 *) (*pSSRsp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_USSD_RSP));
    if(lengthAI != 0)
    {
        (*pSSRsp) ->nAlphaLength = alphaiden.Length;
        (*pSSRsp) ->nAlphaIdentifier = pOffset;
        SUL_MemCopy8((*pSSRsp) ->nAlphaIdentifier, alphaiden.AlphaIdentifier, alphaiden.Length);
        pOffset += MEM_ALIGN_SIZE(alphaiden.Length);
    }
    else
    {
        (*pSSRsp) ->nAlphaLength = 0;
        (*pSSRsp) ->nAlphaIdentifier = NULL;
    }

    (*pSSRsp) ->nSSLength = ss.Length - 1;
    (*pSSRsp) ->nTONNPI = ss.TONandNPI;
    (*pSSRsp) ->pSSStr =  pOffset;
    SUL_MemCopy8( (*pSSRsp) ->pSSStr, ss.SSorUSSDString, ss.Length - 1);

    (*pSSRsp) ->nIconQualifier = iconiden.IconQualifier;
    (*pSSRsp) ->nIconIdentifier = iconiden.IconIdentifier;

    return ERR_SUCCESS;
}


UINT32 fetchSendUSSD(CFW_SAT_USSD_RSP **pUSSDRsp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI = 0;
    UINT8 lengthII = 0;
    UINT8 lengthUSSD = 0;
    UINT8 *pOffset = pInput;

    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_USSD_RSP));
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    struct AlphaIden alphaiden;
    lengthAI = getAlphaIden(pOffset, &alphaiden);
    if(lengthAI != 0)
    {
        pOffset += lengthAI;
        nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
    }

    struct USSDString ussd;
    if(nLength <= pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    if((lengthUSSD = getUSSDString(pOffset, &ussd)) == 0)
        return ERR_PARAM_ILLEGAL;
    nMemsize += MEM_ALIGN_SIZE(ussd.Length - 1);        //one byte scheme
    pOffset += lengthUSSD;

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
        lengthII = getIconIdentifier(pOffset, &iconiden);

    (*pUSSDRsp) = (CFW_SAT_USSD_RSP *)CSW_SIM_MALLOC(nMemsize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD Send Mem size=%d\n",0x08100dcf), nMemsize);

    if ((*pUSSDRsp) == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_ZeroMemory8((*pUSSDRsp) , nMemsize);
    pOffset =  (UINT8 *) (*pUSSDRsp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_USSD_RSP));

    if(lengthAI != 0)
    {
        (*pUSSDRsp) ->nAlphaLength = alphaiden.Length;
        (*pUSSDRsp) ->nAlphaIdentifier = pOffset;
        SUL_MemCopy8( (*pUSSDRsp) ->nAlphaIdentifier, alphaiden.AlphaIdentifier, alphaiden.Length);
        pOffset += MEM_ALIGN_SIZE(alphaiden.Length);
    }
    else
    {
        (*pUSSDRsp) ->nAlphaLength = 0;
        (*pUSSDRsp) ->nAlphaIdentifier = NULL;
    }

    (*pUSSDRsp) ->nUSSDLength = ussd.Length - 1;
    (*pUSSDRsp) ->nCodeScheme = ussd.DataCodingScheme;
    (*pUSSDRsp) ->pUSSDStr =  pOffset;
    SUL_MemCopy8( (*pUSSDRsp) ->pUSSDStr, ussd.USSDString, ussd.Length - 1);

    (*pUSSDRsp) ->nIconQualifier = iconiden.IconQualifier;
    (*pUSSDRsp) ->nIconIdentifier = iconiden.IconIdentifier;

    return ERR_SUCCESS;
}

UINT32 fetchPlayTone(CFW_SAT_PLAYTONE_RSP** pPlayTone, UINT8 *pInput, UINT8 nLength)
{
    UINT8* pOffset = pInput;
    UINT8 lengthAI = 0;
    UINT8 lengthT = 0;
    UINT8 lengthD = 0;
    UINT8 lengthII = 0;

    UINT16 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_PLAYTONE_RSP));

    struct AlphaIden alphaiden;
    if((lengthAI = getAlphaIden(pOffset, &alphaiden)) != 0)
    {
        pOffset += lengthAI;
        nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("alphaiden.Length = %d,lengthAI = %d",0x08100dd0), alphaiden.Length, lengthAI);
    }

    struct Tone tone;
    if((lengthT = getTone(pOffset,&tone)) != 0)
        pOffset += lengthT;

    struct Duration duration;
    if((lengthD = getDuration(pOffset,&duration)) != 0)
        pOffset += lengthD;

    struct IconIden iconiden;
    if((lengthII = getIconIdentifier(pOffset,&iconiden)) != 0)
        pOffset += lengthII;

    (*pPlayTone) = (CFW_SAT_PLAYTONE_RSP *)CSW_SIM_MALLOC(nMemsize);
    if((*pPlayTone) == NULL)
        return ERR_NO_MORE_MEMORY;

    if(lengthAI != 0)
    {
        (*pPlayTone)->AlphaIDLen = lengthAI;
        SUL_MemCopy8((*pPlayTone)->AlphaIDString,alphaiden.AlphaIdentifier, alphaiden.Length);
    }

    if(lengthT != 0)
        (*pPlayTone)->Tone = tone.Tone;

    if(lengthD != 0)
    {
        (*pPlayTone)->TimeUnit = duration.TimeUnit;
        (*pPlayTone)->TimeInterval = duration.TimeInterval;
    }

    if(lengthII != 0)
    {
        (*pPlayTone)->IconIdentifier =iconiden.IconIdentifier;
        (*pPlayTone)->IconQualifier =iconiden.IconQualifier;
    }

    return ERR_SUCCESS;
}


UINT32 fetchLaunchBrowser(CFW_SAT_BROWSER_RSP **pBrowserRsp, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthBI = 0;
    UINT8 lengthURL = 0;
    UINT8 lengthB = 0;
    UINT8 lengthPFR = 0;
    UINT8 lengthTS = 0;
    UINT8 lengthAI = 0;
    UINT8 lengthII = 0;
    UINT8 nMemsize = 0;
    UINT8 *pOffset = pInput;

    if((nLength == 0) ||(pInput == NULL))
        return ERR_PARAM_ILLEGAL;
    nMemsize =  MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_BROWSER_RSP));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("FetchLaunchBrowser: SIZEOF(CFW_SAT_BROWSER_RSP) = %d",0x08100dd1),nMemsize);

    struct BrowserIden browser;
    lengthBI = getBrowserIden(pOffset, &browser);
    if(lengthBI != 0)
        pOffset += lengthBI;

    if(nLength <= pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    struct URL url;
    lengthURL = getURL(pOffset, &url);
//    if(lengthURL == 0)                //3GPP test case allow this is NULL
//        return ERR_PARAM_ILLEGAL;
    if(lengthURL != 0)
    {
        pOffset += lengthURL;
        nMemsize += MEM_ALIGN_SIZE(url.Length);
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("----------url---------",0x08100dd2));
//    UINT8 k = 0;
//    for(k= 0;k<url.Length;k++)
//        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("%x --%c",0x08100dd3),url.URL[k], url.URL[k]);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("FetchLaunchBrowser: MEM_ALIGN_SIZE(url.Length) = %d,nMemsize = %d",0x08100dd4),MEM_ALIGN_SIZE(url.Length),nMemsize);

    struct Bearer bearer;
    if(nLength > pOffset - pInput)
    {
        lengthB = getBearer(pOffset, &bearer);
        if(lengthB != 0)
        {
            pOffset += lengthB;
            nMemsize += MEM_ALIGN_SIZE(bearer.Length);
        }
    }

    struct ProvisoinFileRef provfileref;
    UINT8 FileRef[255]= {0};
    UINT8 num = 0;
    UINT8 i = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("getProvisoinFileRef  nLength = %d,pOffset - pInput = %d",0x08100dd5),nLength,pOffset - pInput);
    while(nLength > pOffset - pInput)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("getProvisoinFileRef  num = %d",0x08100dd6),num);
        lengthPFR = getProvisoinFileRef(pOffset, &provfileref);
        if(lengthPFR != 0)
        {
            pOffset += lengthPFR;
            nMemsize += MEM_ALIGN_SIZE(provfileref.Length) + SIZEOF(VOID*);
            //the last member of CFW_SAT_BROWSER_RSP is array, Here add one pointer;
            num++;
            FileRef[i] = provfileref.Length;
            SUL_MemCopy8(FileRef + i + 1, provfileref.Path, provfileref.Length);
            i += provfileref.Length + 1;    //FileRef devide to several groups, one group include one byte store the length and PathOfFiles;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("getProvisoinFileRef list done!",0x08100dd7));
            break;
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("the number of getProvisoinFileRef = %d,nMemsize = %d",0x08100dd8),num,nMemsize);

    struct TextString textstring;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("FetchLaunchBrowser: nLength = %d,pOffset - pInput = %d",0x08100dd9),nLength,pOffset - pInput);
    if(nLength > pOffset - pInput)
    {
        lengthTS = getTextString(pOffset, &textstring);
        if(lengthTS != 0)
        {
            pOffset += lengthTS;
            nMemsize += MEM_ALIGN_SIZE(textstring.Length - 1);
        }
    }

    struct AlphaIden alphaiden;
    if(nLength > pOffset - pInput)
    {
        lengthAI = getAlphaIden(pOffset, &alphaiden);
        if(lengthAI != 0)
        {
            pOffset += lengthAI;
            nMemsize += alphaiden.Length;
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("FetchLaunchBrowser: nLength = %d,pOffset - pInput =%d",0x08100dda),nLength,pOffset - pInput);
    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
    {
        lengthII = getIconIdentifier(pOffset, &iconiden);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("FetchLaunchBrowser: lengthII = %d",0x08100ddb),lengthII);
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("FetchLaunchBrowser: nMemsize = %d",0x08100ddc),nMemsize);
    *pBrowserRsp = (CFW_SAT_BROWSER_RSP *)CSW_SIM_MALLOC(nMemsize);
    if(*pBrowserRsp == NULL)
        return ERR_NO_MORE_MEMORY;

    pOffset = (UINT8 *)(*pBrowserRsp) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_BROWSER_RSP));

    if(lengthBI != 0)
        (*pBrowserRsp)->nBrowserIdentity = browser.BrowserIdentifier;
    else
        (*pBrowserRsp)->nBrowserIdentity = 0;

    if(num != 0)
    {
        (*pBrowserRsp)->nProvisioningFileRefNum = num;
        UINT8 j = 0;
        pOffset = pOffset + num * SIZEOF(VOID*);
        for(i = 0; i < num; i++)
        {
            (*pBrowserRsp)->pProvisioningFileRef[i] = pOffset;
            SUL_MemCopy8((*pBrowserRsp)->pProvisioningFileRef[i], &FileRef[j + 1], FileRef[j]);
            pOffset += MEM_ALIGN_SIZE(FileRef[j]);
            j += FileRef[j] + 1;
        }
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (*pBrowserRsp)->pProvisioningFileRef[0], 50, 16);
    }
    else
    {
        (*pBrowserRsp)->nProvisioningFileRefNum = 0;
        (*pBrowserRsp)->pProvisioningFileRef[0] = NULL;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("getProvisoinFileRef url.Length = %d",0x08100ddd),url.Length);

    if(lengthURL != 0)
    {
        (*pBrowserRsp)->nURLLength = url.Length;
        (*pBrowserRsp)->pURL = pOffset;
        SUL_MemCopy8((*pBrowserRsp)->pURL, url.URL, url.Length);
        pOffset += MEM_ALIGN_SIZE(url.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pURL: pOffset = %x",0x08100dde),pOffset);
        //for(k= 0;k<url.Length;k++)
        //    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("%x -- %c",0x08100ddf),(*pBrowserRsp)->pURL[k],(*pBrowserRsp)->pURL[k]);
    }
    else
    {
        (*pBrowserRsp)->nURLLength = 0;
        (*pBrowserRsp)->pURL = 0;
    }

    if(lengthB != 0)
    {
        (*pBrowserRsp)->nBearerLength = bearer.Length;
        (*pBrowserRsp)->pListBearer = pOffset;
        SUL_MemCopy8((*pBrowserRsp)->pListBearer, bearer.ListBearers, bearer.Length);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pListBearer: pOffset = %x",0x08100de0),pOffset);

        pOffset += MEM_ALIGN_SIZE(bearer.Length);
    }
    else
    {
        (*pBrowserRsp)->nBearerLength = 0;
        (*pBrowserRsp)->pListBearer = NULL;
    }

    if(lengthTS != 0)
    {
        (*pBrowserRsp)->nCodeScheme = textstring.DataCodingScheme;
        (*pBrowserRsp)->nTextLength = textstring.Length - 1;
        (*pBrowserRsp)->nTextString = pOffset;
        SUL_MemCopy8((*pBrowserRsp)->nTextString, textstring.TextString, textstring.Length - 1);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pTextString: pOffset = %x",0x08100de1),pOffset);
        for(i = 0; i<textstring.Length - 1; i++)
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("%x -- %c",0x08100de2),textstring.TextString[i],textstring.TextString[i]);
        pOffset += MEM_ALIGN_SIZE(textstring.Length - 1);
    }
    else
    {
        (*pBrowserRsp)->nCodeScheme = 0;
        (*pBrowserRsp)->nTextLength = 0;
        (*pBrowserRsp)->nTextString = NULL;
    }

    if(lengthAI != 0)
    {
        (*pBrowserRsp)->nAlphaLength = alphaiden.Length;
        (*pBrowserRsp)->nAlphaIdentifier = pOffset;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pAlphaIdentifier: pOffset = %x,alphaiden.Length =%d",0x08100de3),pOffset,alphaiden.Length);
        SUL_MemCopy8((*pBrowserRsp)->nAlphaIdentifier, alphaiden.AlphaIdentifier, alphaiden.Length);
        for(i = 0; i<alphaiden.Length; i++)
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("%x -- %c",0x08100de4),(UINT8)(*pBrowserRsp)->nAlphaIdentifier[i],(*pBrowserRsp)->nAlphaIdentifier[i]);
        //pOffset += alphaiden.Length - 1;
    }
    else
    {
        (*pBrowserRsp)->nAlphaLength = 0;
        (*pBrowserRsp)->nAlphaIdentifier = NULL;
    }

    if(lengthII != 0)
    {
        (*pBrowserRsp)->nIconQualifier = iconiden.IconQualifier;
        (*pBrowserRsp)->nIconIdentifier = iconiden.IconIdentifier;
    }
    else
    {
        (*pBrowserRsp)->nIconQualifier = 0xFF;
        (*pBrowserRsp)->nIconIdentifier = 0xFF;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("*pBrowserRsp = %x, nMemsize = %d",0x08100de5),*pBrowserRsp, nMemsize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Last address of pBrowserRsp = %x,  alphaiden.Length -1 = %d",0x08100de6),(*pBrowserRsp)->nAlphaIdentifier, alphaiden.Length-1);
    return ERR_SUCCESS;
}

#if 0
UINT32 fetchTimerManagement(CFW_SAT_RESPONSE *pSimSatResponseData,
                            SIM_SAT_PARAM *pG_SimSat, UINT8 *pInput, UINT8 nSimID, UINT8 nLength)
{
    UINT8 lengthTI = 0;
    UINT8 lengthTV = 0;
    UINT8 *pOffset = pInput;

    UINT8 nIndex        = 0;
    UINT8 nTimerID = 0x00;
    UINT8 nHour = 0x00;
    UINT8 nMinute = 0x00;
    UINT8 nSecond = 0x00;
    UINT32 nTimerValue = 0x00;

    SAT_BASE_RSP *pBaseResp = (SAT_BASE_RSP *)CSW_SIM_MALLOC(SIZEOF(SAT_BASE_RSP));
    if(pBaseResp == NULL)
        return ERR_NO_MORE_MEMORY;
    SUL_ZeroMemory8(pBaseResp, (SIZEOF(SAT_BASE_RSP)));
    pBaseResp->nComID        = pG_SimSat->nCurCmd;
    pBaseResp->nComQualifier = pG_SimSat->nCmdQualifier;

    if (pG_SimSat->pSatComData != NULL)
    {
#if 0
        if(pG_SimSat->nCurCmd == SIM_SAT_SELECT_ITEM_COM)
        {
            CFW_SAT_MENU_RSP *p = (CFW_SAT_MENU_RSP *)pG_SimSat->pSatComData;
            CSW_SIM_FREE(p->pItemList);
        }
#endif
        CSW_SIM_FREE(pG_SimSat->pSatComData);
    }

    pG_SimSat->pSatComData = pBaseResp;

    /*
        if(nLength == 0)
            return ERR_PARAM_ILLEGAL;
        struct TimerIden timeriden;
        if((lengthTI = getTimerIden(pOffset,&timeriden)) == 0)
            return ERR_PARAM_ILLEGAL;
        pOffset += lengthTI;
        if(nLength > pOffset - pInput)
        {
            struct TimerValue timervalue;
            lengthTV = getTimerValue(pOffset, &timervalue)
        }
        nTimerID = timeriden.Identifier;
        nHour = timervalue.TimerValue[2];
        nMinute = timervalue.TimerValue[1];
        nSecond = timervalue.TimerValue[0];
    */

    if ((pOffset[nIndex] == 0x24) || (pOffset[nIndex] == 0xA4))
    {
        nTimerID = _GetTimerIdentifier(&pOffset[nIndex]) ;
        nIndex += 3;
    }
    if ((pOffset[nIndex] == 0x25) || (pOffset[nIndex] == 0xA5))
    {
        nTimerValue = _GetTimerValue(&pOffset[nIndex]);
        nHour = nTimerValue >> 16;
        nMinute = nTimerValue >> 8;
        nSecond = nTimerValue & 0xFF;
    }

    if(nTimerID)
    {
        gSATTimerValue[nTimerID - 1][nSimID] = nTimerValue ;
    }
    BOOL bRet = FALSE;
    /*
    -   TIMER MANAGEMENT
    bits 1 to 2     00 = start
            01 = deactivate
            10 = get current value
            11 = RFU
    bits 3 to 8 RFU
    */

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier	  %d\n",0x08100de7), pBaseResp->nComQualifier);
    if( 0x00 == pBaseResp->nComQualifier )
    {
        bRet = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1, \
                              COS_TIMER_MODE_SINGLE, (nHour * 60 * 60 + nMinute * 60 + nSecond) * 1000 MILLI_SECOND);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("COS_SetTimerEX	  %d\n",0x08100de8), bRet);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nTimerID	  %d\n",0x08100de9), nTimerID);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Second	  %d\n",0x08100dea), (nHour * 60 * 60 + nMinute * 60 + nSecond));
    }
    else if( 0x01 == pBaseResp->nComQualifier )
    {
        bRet = COS_KillTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("COS_KillTimerEX	  %d\n",0x08100deb), bRet);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nTimerID	  %d\n",0x08100dec), nTimerID);
    }

    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    SAT_BASE_RSP *pBase = (SAT_BASE_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pBase == NULL))
        return ERR_NO_MORE_MEMORY;

    memset(pSimTerminalResponesReq, 0x00, sizeof(api_SimTerminalResponseReq_t));
    pSimTerminalResponesReq->NbByte = RESPONSEBASE;

    // SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponseData->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    // pSimTerminalResponesReq->TerminalRespData[ComType -1]   = 0x02;

    pSimTerminalResponesReq->TerminalRespData[ComType] = SIM_SAT_TIMER_MANAGEMENT_IND;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pBaseResp->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_CmdType    %x\n",0x08100ded), pSimSatResponseData->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier   %x\n",0x08100dee), pBase->nComQualifier);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status	   %x\n",0x08100def), pSimSatResponseData->nSAT_Status);

    if(  pBaseResp->nComQualifier == 1)
    {
        //a5 03 00 00 52
        pSimTerminalResponesReq->NbByte += 8;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 0] = 0x04;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 1] = 0x01;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 2] = 0x01;

        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 3] = 0xa5;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 4] = 0x03;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 5] = 0x00;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 6] = 0x00;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 7] =
            (UINT8)sxs_ExistTimer(HVY_TIMER_IN + PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1) / 16384;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimTerminalResponesReq->NbByte   %x\n",0x08100df0), pSimTerminalResponesReq->NbByte);

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID | C_DETAIL, (UINT8 *) pSimTerminalResponesReq, sizeof(api_SimTerminalResponseReq_t), 16);
    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID))
        return ERR_SCL_QUEUE_FULL;
    return ERR_SUCCESS;
}
#else

UINT32 fetchTimerManagement(CFW_SAT_TIMER_RSP **pTimer, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthTI = 0;
    UINT8 lengthTV = 0;
    UINT8 *pOffset = pInput;

    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    struct TimerIden timeriden;
    if((lengthTI = getTimerIden(pOffset, &timeriden)) == 0)
        return ERR_PARAM_ILLEGAL;
    if((timeriden.Identifier == 0) && (timeriden.Identifier >= 9))
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthTI;
    struct TimerValue timervalue;
    SUL_ZeroMemory8(&timervalue, SIZEOF(timervalue));

    if(nLength > pOffset - pInput)
        lengthTV = getTimerValue(pOffset, &timervalue);

    (*pTimer) = (CFW_SAT_TIMER_RSP *)CSW_SIM_MALLOC(SIZEOF(CFW_SAT_TIMER_RSP));
    if((*pTimer) == NULL)
        return ERR_NO_MORE_MEMORY;

    (*pTimer)->nTimerID = timeriden.Identifier;
    (*pTimer)->nHour = timervalue.TimerValue[2];
    (*pTimer)->nMinute = timervalue.TimerValue[1];
    (*pTimer)->nSecond = timervalue.TimerValue[0];

    return ERR_SUCCESS;
}

#endif

UINT32 fetchDTMF(CFW_SAT_SEND_DTMF_RSP **pDTMF, UINT8 *pInput, UINT8 nLength)
{
    UINT8 lengthAI = 0;
    UINT8 lengthII = 0;
    UINT8 lengthDTMF = 0;

    UINT8 nMemsize = MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_SEND_DTMF_RSP));

    UINT8 *pOffset = pInput;
    if(nLength == 0)
        return ERR_PARAM_ILLEGAL;
    struct AlphaIden alphaiden;
    lengthAI = getAlphaIden(pOffset, &alphaiden);
    if(lengthAI != 0)
    {
        pOffset += lengthAI;
        nMemsize += MEM_ALIGN_SIZE(alphaiden.Length);
    }

    if(nLength < pOffset - pInput)
        return ERR_PARAM_ILLEGAL;

    struct DTMFString dtmf;
    lengthDTMF = getDTMFString(pOffset, &dtmf);
    if(lengthDTMF == 0)
        return ERR_PARAM_ILLEGAL;
    pOffset += lengthDTMF;
    nMemsize += dtmf.Length;

    struct IconIden iconiden;
    if(nLength > pOffset - pInput)
        lengthII = getIconIdentifier(pOffset, &iconiden);

    *pDTMF = (CFW_SAT_SEND_DTMF_RSP *)CSW_SIM_MALLOC(nMemsize);
    if((*pDTMF) == NULL)
        return ERR_NO_MORE_MEMORY;
    pOffset = (UINT8 *)(*pDTMF) + MEM_ALIGN_SIZE(SIZEOF(CFW_SAT_SEND_DTMF_RSP));
    if(lengthAI != 0)
    {
        (*pDTMF)->nAlphaLen = alphaiden.Length;
        (*pDTMF)->pAlphaStr = pOffset;
        SUL_MemCopy8((*pDTMF)->pAlphaStr, alphaiden.AlphaIdentifier, alphaiden.Length);
        pOffset += MEM_ALIGN_SIZE((*pDTMF)->nAlphaLen);
    }
    else
    {
        (*pDTMF)->nAlphaLen = 0;
        (*pDTMF)->pAlphaStr = NULL;
    }
    if(dtmf.Length > 0)
    {
        (*pDTMF)->nDTMFLen = dtmf.Length;
        (*pDTMF)->pDTMFStr = pOffset;
        SUL_MemCopy8((*pDTMF)->pDTMFStr, dtmf.DTMFString, dtmf.Length);
    }
    else
    {
        (*pDTMF)->nDTMFLen = 0;
        (*pDTMF)->pDTMFStr = NULL;
    }
    return ERR_SUCCESS;
}

#if 0
UINT32 sendSetupMenuRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                        SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SimTerminalResponseReq_t));
    CFW_SAT_MENU_RSP *pMenu = (CFW_SAT_MENU_RSP *)(pG_SimSat->pSetupMenu);
    if ((pSimTerminalResponesReq == NULL) || (pMenu == NULL))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pMenu = %x, pSimTerminalResponesReq = %x\n",0x08100df1), pMenu, pSimTerminalResponesReq);
        return ERR_NO_MORE_MEMORY;
    }

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    pSimTerminalResponesReq->TerminalRespData[ComType]   = SIM_SAT_SETUP_MENU_COM;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pMenu->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;
    CSW_TC_MEMBLOCK(CFW_API_TS_ID, (UINT8 *) pSimTerminalResponesReq->TerminalRespData, pSimTerminalResponesReq->NbByte, 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}
#endif

UINT32 sendResponsePDU(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = pSimSatResponse->nSAT_InputLen;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sendResponsePDU: nSAT_InputLen = %d",0x08100df2), pSimSatResponse->nSAT_InputLen);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("sendResponsePDU: Length = %d",0x08100df3), SUL_Strlen(pSimSatResponse->nSAT_InputStr));

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, pSimSatResponse->nSAT_InputStr,
                 pSimTerminalResponesReq->NbByte);

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *)pSimTerminalResponesReq->TerminalRespData,
                    pSimTerminalResponesReq->NbByte, 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("sendResponsePDU  ERR_SUCCESS",0x08100df4));
    return ERR_SUCCESS;
}


UINT32 sendSelectItemRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                         SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    CFW_SAT_MENU_RSP *pMenu = (CFW_SAT_MENU_RSP *)(pG_SimSat->pSatComData);
    if(pMenu == NULL)
        return ERR_PARAM_ILLEGAL;

    //CFW_SAT_MENU_RSP* p = pMenu->pItemList;
    //while(p != NULL)
    //{
    //  CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("p->nComID = %d",0x08100df5),p ->nComID);
    //    p = p->pItemList;
    //}
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SIM_SAT_SELECT_ITEM_IND: pMenu = %x, pSimTerminalResponesReq = %x\n",0x08100df6), pMenu, pSimTerminalResponesReq);

    if ((pSimTerminalResponesReq == NULL) || (pMenu == NULL))
        return ERR_NO_MORE_MEMORY;

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SIM_SAT_SELECT_ITEM_IND: pG_SimSat->nCmdNum = %d,  nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID] = %d\n",0x08100df7), \
              pG_SimSat->nCmdNum, nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID]);

    pSimTerminalResponesReq->TerminalRespData[ComType]   = SIM_SAT_SELECT_ITEM_IND;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pMenu->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SIM_SAT_SELECT_ITEM_IND: status = %x, Item ID = %d\n",0x08100df8),  \
              pSimTerminalResponesReq->TerminalRespData[ComResult], pSimSatResponse->nSAT_ItemID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SELECT_ITEM_IND: Command number = %x\n",0x08100df9),  pSimTerminalResponesReq->TerminalRespData[ComType - 1]);

    //if(( 0x00 == pSimSatResponse->nSAT_Status) || ( 0x13/*Request help infor*/ == pSimSatResponse->nSAT_Status) )
    if(0x00 == pSimSatResponse->nSAT_Status)
    {
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseItem, RESPONSEITEM);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + ItemSelection) =
            pSimSatResponse->nSAT_ItemID;
        pSimTerminalResponesReq->NbByte = RESPONSEBASE + RESPONSEITEM;
    }
    else
    {
        pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL),"===========pSimTerminalResponesReq->NbByte=%d=========\n",pSimTerminalResponesReq->NbByte);
    //UINT8     i;
    //for(i = 0; i<  pSimTerminalResponesReq->NbByte; i++)
    //  CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("%02x",0x08100dfb),pSimTerminalResponesReq->TerminalRespData[i]);
    //  CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("=====================\n",0x08100dfc));
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *)(pSimTerminalResponesReq->TerminalRespData), pSimTerminalResponesReq->NbByte, 16);

    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendGetInKeyRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    CFW_SAT_INPUT_RSP *pGetInput = (CFW_SAT_INPUT_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pGetInput == NULL))
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE; // no string input
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);

#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pGetInput->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID)|TDB|TNB_ARG(1), TSTR("satresponseproc:get inkey %d\n",0x08100dfd), pSimSatResponse->nSAT_InputLen);
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *)(pSimSatResponse->nSAT_InputStr), 3, 16);
    if (pSimSatResponse->nSAT_InputLen > 0)
    {
        pSimTerminalResponesReq->NbByte = RESPONSEBASE + RESPONSETEXT + pSimSatResponse->nSAT_InputLen - 1; // remove the sch byte

        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseText, RESPONSETEXT);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + TextLength) = pSimSatResponse->nSAT_InputLen;  // add scheme
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + TextScheme) = pSimSatResponse->nSAT_InputSch;

        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + RESPONSETEXT,
                     pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen - 1);
    }

    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendGetInputRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    CFW_SAT_INPUT_RSP *pGetInput = (CFW_SAT_INPUT_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pGetInput == NULL))
        return ERR_NO_MORE_MEMORY;
    pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pGetInput->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    if (pSimSatResponse->nSAT_InputLen != 0)
    {
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE) = TEXT_STRING_TAG;
        pSimTerminalResponesReq->NbByte += pSimSatResponse->nSAT_InputLen + 2;

        if(pSimSatResponse->nSAT_InputLen < 0x7f)
        {
            *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + 1) = pSimSatResponse->nSAT_InputLen;
            *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + TextScheme) = pSimSatResponse->nSAT_InputSch;

            SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + RESPONSETEXT,
                         pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen - 1);
        }
        else
        {
            *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + 1) = 0x81;
            *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + 2) = pSimSatResponse->nSAT_InputLen;
            *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + TextScheme + 1) = pSimSatResponse->nSAT_InputSch;

            SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + RESPONSETEXT + 1,
                         pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen - 1);

            pSimTerminalResponesReq->NbByte++;
        }
        //CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimTerminalResponesReq->TerminalRespData, pSimTerminalResponesReq->NbByte, 16);
    }

    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

#if 0
UINT32 sendSMSPPDownload(CFW_SAT_RESPONSE *pSimSatResponse,
                         SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_SMSPP_DOWNLOAD[0xD1] command ERR_NO_MORE_MEMORY \n",0x08100dfe));
        return ERR_NO_MORE_MEMORY;
    }
    api_SmsPPReceiveInd_t *pSMSPP = (api_SmsPPReceiveInd_t *)(pSimSatResponse->nSAT_InputStr);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSMSPP->DataLen %x,pSMSPP->AddressLen %x\n",0x08100dff), pSMSPP->DataLen,
              pSMSPP->AddressLen);
    if(( 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen ) < 0x80)
    {
        SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, SMPPDownSelection, 7);
        //Device identities tag Length: 4
        /*
          Byte(s) Description Length
          1   Device identities tag   1
          2   Length = '02'   1
          3   Source device identity  1
          4   Destination device identity 1
        */
        //Address Length: 2
        /*
          Byte(s) Description Length
          1   Address tag 1
          2 to (Y 1)+2    Length (X)  Y
        */

        //SMS TPDU tag: 2
        /*
           Byte(s) Description Length
           1   SMS TPDU tag    1
           2 to (Y 1)+2    Length (X)  Y
         */
        pSimTerminalEnvelopeReq->EnvelopeData[1] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSMSPP->AddressLen %x pSMSPP->DataLen %x\n",0x08100e00), pSMSPP->AddressLen,
                  pSMSPP->DataLen);

        //SMS-PP download tag: 2
        /*
          Description Section M/O Min Length
          SMS-PP download tag 13.1    M   Y   1
          Length (A+B+C)  -   M   Y   1 or 2
        */
        pSimTerminalEnvelopeReq->NbByte = pSimTerminalEnvelopeReq->EnvelopeData[1] + 2;

        /*
           A: Device identities
           B: Address
           C: SMS TPDU (SMS-DELIVER)
         */
        pSimTerminalEnvelopeReq->EnvelopeData[7] = pSMSPP->AddressLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[8]), pSMSPP->Address, pSMSPP->AddressLen);

        pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen] = 0x0B;
        pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen + 1] = pSMSPP->DataLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen + 2]), pSMSPP->Data,
                     pSMSPP->DataLen);
    }
    else
    {
        SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, SMPPDownSelectionEx, 8);

        //Device identities tag Length: 4
        /*
          Byte(s) Description Length
          1   Device identities tag   1
          2   Length = '02'   1
          3   Source device identity  1
          4   Destination device identity 1
        */

        //Address Length: 2
        /*
          Byte(s) Description Length
          1   Address tag 1
          2 to (Y 1)+2    Length (X)  Y
        */

        //SMS TPDU tag: 2
        /*
           Byte(s) Description Length
           1   SMS TPDU tag    1
           2 to (Y 1)+2    Length (X)  Y
         */
        if( pSMSPP->DataLen < 0x80 )
            pSimTerminalEnvelopeReq->EnvelopeData[2] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen;
        else
            pSimTerminalEnvelopeReq->EnvelopeData[2] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen + 1;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSMSPP->AddressLen %x pSMSPP->DataLen %x\n",0x08100e01), pSMSPP->AddressLen,
                  pSMSPP->DataLen);

        //SMS-PP download tag: 2
        /*
          Description Section M/O Min Length
          SMS-PP download tag 13.1    M   Y   1
          Length (A+B+C)  -   M   Y   1 or 2
        */
        pSimTerminalEnvelopeReq->NbByte = pSimTerminalEnvelopeReq->EnvelopeData[2] + 3;

        /*
           A: Device identities
           B: Address
           C: SMS TPDU (SMS-DELIVER)
         */
        pSimTerminalEnvelopeReq->EnvelopeData[8] = pSMSPP->AddressLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9]), pSMSPP->Address, pSMSPP->AddressLen);

        pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen] = 0x0B;

        if(pSMSPP->DataLen < 0x80)
        {
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 1] = pSMSPP->DataLen;
            SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 2]), pSMSPP->Data,
                         pSMSPP->DataLen);
        }
        else
        {
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 1] = 0x81;
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 2] = pSMSPP->DataLen;
            SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 3]), pSMSPP->Data,
                         pSMSPP->DataLen);
        }

    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0xD1 CMD TERMINALENVELOPE_REQ \n",0x08100e02));

    // CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData, (UINT16) pSimTerminalEnvelopeReq->NbByte, 16);
    if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID))
        return ERR_SCL_QUEUE_FULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0xD1 command OK!!! \n",0x08100e03));
    return ERR_SUCCESS;
}
#else
UINT32 sendSMSPPDownload(CFW_SAT_RESPONSE *pSimSatResponse,
                         SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_SMSPP_DOWNLOAD[0xD1] command ERR_NO_MORE_MEMORY \n",0x08100e04));
        return ERR_NO_MORE_MEMORY;
    }
    api_SmsPPReceiveInd_t *pSMSPP = (api_SmsPPReceiveInd_t *)(pSimSatResponse->nSAT_InputStr);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SMSPP DataLen %x, SMSPP AddressLen %x\n",0x08100e05), pSMSPP->DataLen, pSMSPP->AddressLen);
#if 0
//====================================================
    if(( 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen ) < 0x80)
    {
        SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, SMPPDownSelection, 7);
        pSimTerminalEnvelopeReq->EnvelopeData[1] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSMSPP->AddressLen %x pSMSPP->DataLen %x\n",0x08100e06), pSMSPP->AddressLen,
                  pSMSPP->DataLen);

        pSimTerminalEnvelopeReq->NbByte = pSimTerminalEnvelopeReq->EnvelopeData[1] + 2;

        pSimTerminalEnvelopeReq->EnvelopeData[7] = pSMSPP->AddressLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[8]), pSMSPP->Address, pSMSPP->AddressLen);

        pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen] = 0x0B;
        pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen + 1] = pSMSPP->DataLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[8 + pSMSPP->AddressLen + 2]), pSMSPP->Data,
                     pSMSPP->DataLen);
    }
    else
    {
        SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, SMPPDownSelectionEx, 8);
        if( pSMSPP->DataLen < 0x80 )
            pSimTerminalEnvelopeReq->EnvelopeData[2] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen;
        else
            pSimTerminalEnvelopeReq->EnvelopeData[2] = 4 + 2 + 2 + pSMSPP->AddressLen + pSMSPP->DataLen + 1;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSMSPP->AddressLen %x pSMSPP->DataLen %x\n",0x08100e07), pSMSPP->AddressLen,
                  pSMSPP->DataLen);

        pSimTerminalEnvelopeReq->NbByte = pSimTerminalEnvelopeReq->EnvelopeData[2] + 3;

        pSimTerminalEnvelopeReq->EnvelopeData[8] = pSMSPP->AddressLen;
        SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9]), pSMSPP->Address, pSMSPP->AddressLen);

        pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen] = 0x0B;

        if(pSMSPP->DataLen < 0x80)
        {
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 1] = pSMSPP->DataLen;
            SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 2]), pSMSPP->Data,
                         pSMSPP->DataLen);
        }
        else
        {
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 1] = 0x81;
            pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 2] = pSMSPP->DataLen;
            SUL_MemCopy8(&(pSimTerminalEnvelopeReq->EnvelopeData[9 + pSMSPP->AddressLen + 3]), pSMSPP->Data,
                         pSMSPP->DataLen);
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("====================================================",0x08100e08));
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData, (UINT16) pSimTerminalEnvelopeReq->NbByte, 16);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("====================================================",0x08100e09));
//====================================================
#endif

    //Build SMSPP Download data
    UINT8 i = 0;
    pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0xD1;
    //Count the data length of SMSPPDownload, 4 for device identities,
    UINT8 nLength = 4;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("0 : nLength = %d, pSMSPP->AddressLen = %d",0x08100e0a), nLength, pSMSPP->AddressLen);
    if(pSMSPP->AddressLen != 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("1 : nLength = %d",0x08100e0b), nLength);
        nLength += 2 + pSMSPP->AddressLen;  // 2 for Address tag, length
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("2 : nLength = %d",0x08100e0c), nLength);
        if(pSMSPP->AddressLen > 0x7F)
        {
            nLength++;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("3 : nLength = %d",0x08100e0d), nLength);
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Address : nLength = %d, i = %d",0x08100e0e), nLength, i);
    if(pSMSPP->DataLen != 0)
    {
        nLength += 2 + pSMSPP->DataLen;     // 2 for SMS TPDU tag and length
        if(pSMSPP->DataLen > 0x7F)
            nLength++;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SMSPP : nLength = %d, i = %d",0x08100e0f), nLength, i);
    if(nLength > 0x7F)
        pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x81;
    pSimTerminalEnvelopeReq->EnvelopeData[i++] = nLength;

    pSimTerminalEnvelopeReq->EnvelopeData[i++] = DEVICE_IDENTITY_TAG;   //create device object
    pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x02;
    pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x83;
    pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x81;
    if(pSMSPP->AddressLen != 0)
    {
        //Create Address object
        pSimTerminalEnvelopeReq->EnvelopeData[i++] = ADDRESS_TAG;   //create address object
        if(pSMSPP->AddressLen > 0x7F)
            pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x81;
        pSimTerminalEnvelopeReq->EnvelopeData[i++] = pSMSPP->AddressLen;
        memcpy(pSimTerminalEnvelopeReq->EnvelopeData + i, pSMSPP->Address, pSMSPP->AddressLen);
        i += pSMSPP->AddressLen;
    }

    if(pSMSPP->DataLen != 0)
    {
        //Create Address object
        pSimTerminalEnvelopeReq->EnvelopeData[i++] = SMS_TPDU_TAG;  //create smspp object
        if(pSMSPP->DataLen > 0x7F)
            pSimTerminalEnvelopeReq->EnvelopeData[i++] = 0x81;
        pSimTerminalEnvelopeReq->EnvelopeData[i++] = pSMSPP->DataLen;
        memcpy(pSimTerminalEnvelopeReq->EnvelopeData + i, pSMSPP->Data, pSMSPP->DataLen);
    }
    pSimTerminalEnvelopeReq->NbByte = pSMSPP->DataLen + i;
    if(pSimTerminalEnvelopeReq->NbByte != nLength + 2)
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("WARNING: the NbByte is not equal to (nLength + 2)",0x08100e10));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SMSPP : pSMSPP->DataLen = %d, i = %d",0x08100e11), pSMSPP->DataLen, i);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimTerminalEnvelopeReq->NbByte = 0x%x",0x08100e12), pSimTerminalEnvelopeReq->NbByte);
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData, (UINT16) pSimTerminalEnvelopeReq->NbByte, 16);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SMSPPDownload success(0xD1)",0x08100e13));

    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);
    return ERR_SUCCESS;
}
#endif
UINT32 sendEventDownload(CFW_SAT_RESPONSE *pSimSatResponse,     UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalEnvelopeReq->NbByte = 0x09;
    SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, EventDownload, pSimTerminalEnvelopeReq->NbByte);
    //pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber] = pSimSatResponse->nSAT_ItemID; // Select the second item
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_EVENT_DOWNLOAD[0xD6] CMD TERMINALENVELOPE_REQ \n",0x08100e14));
    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_EVENT_DOWNLOAD[0xD6] command OK!!! \n",0x08100e15));
    return ERR_SUCCESS;
}

UINT32 sendTimerExpiration(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
        return ERR_NO_MORE_MEMORY;
    pSimTerminalEnvelopeReq->NbByte = 0x0e;

    SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, TimerExpirationDownload, pSimTerminalEnvelopeReq->NbByte);

    _SetTimerExpirationDownloadData(pSimTerminalEnvelopeReq->EnvelopeData, pSimSatResponse->nSAT_InputStr[0], nSimID);
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData,  pSimTerminalEnvelopeReq->NbByte, 16);

    //pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber] = pSimSatResponse->nSAT_ItemID; // Select the second item
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_TIMER_EXPIRATION[0xD7] CMD TERMINALENVELOPE_REQ \n",0x08100e16));
    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);
    return ERR_SUCCESS;
}
UINT32 sendPollIntervalRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                           SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    CFW_SAT_POLL_INTERVAL_RSP *pBase = (CFW_SAT_POLL_INTERVAL_RSP *)(pG_SimSat->pSatComData);

    if ((pSimTerminalResponesReq == NULL) || (pBase == NULL))
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE + 4;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    // 次数有些特别，目前发现一张特别的卡会上该Fetch Ind,单独先进行Command number 0x01的处理
    // 如果失败再进行0x00的处理
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pBase->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    pSimTerminalResponesReq->TerminalRespData[0x0c] = 0x84;
    pSimTerminalResponesReq->TerminalRespData[0x0d] = 0x02;
    pSimTerminalResponesReq->TerminalRespData[0x0e] = pBase->nTimeUnit;
    pSimTerminalResponesReq->TerminalRespData[0x0f] = pBase->nTimeInterval;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_CmdType = %x\n",0x08100e17), pSimSatResponse->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier = %x\n",0x08100e18), pBase->nComQualifier);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status = %x\n",0x08100e19), pSimSatResponse->nSAT_Status);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendUSSDRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                   SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    // USSD String
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));

    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    //pSimTerminalResponesReq->TerminalRespData[ComQual]   = pBase->nComQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD - nSAT_CmdType    %x\n",0x08100e1a), pSimSatResponse->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USSD - nSAT_Status     %x\n",0x08100e1b), pSimSatResponse->nSAT_Status);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "pSimSatResponse->nSAT_InputLen = %x", pSimSatResponse->nSAT_InputLen);
    if((pSimSatResponse->nSAT_Status < 0x10) && (pSimSatResponse->nSAT_InputLen != 0))
    {
#if 1
        UINT8* pOffset = pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE;
        *pOffset = 0x8D;				//TEXT_STRING_TAG;     //pOffset point to TEXT_STRING_TAG
        pOffset++;                      //pOffset  point to an offset of the length of TEXT_STRING
		if(pSimSatResponse->nSAT_InputLen > 0x7F)
		{
            *(pOffset) = 0x81;
            pOffset++;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====pOffset = %x ====", pOffset);
		}
		*(pOffset) = pSimSatResponse->nSAT_InputLen;
        SUL_MemCopy8(pOffset + 1, pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen);
		//convert CBS dcs to SMS dcs
		UINT8* pDCS = pOffset + 1;
		UINT8 nDCS = *pDCS;
		if((nDCS&0xF0) == 0)
		{
		   nDCS = 0;
		}
		else if(nDCS == 0x10)
		{
		   nDCS = 0;
		}
		else if(nDCS == 0x11)
		{
		    nDCS = 0x08;
		}
		else if((nDCS&0xE0) == 0x20)
		{
			nDCS = 0;
		}
		else if((nDCS >> 6) == 0x01)
		{
		   nDCS = nDCS & 0x3F;
		}
		else if((nDCS >> 4) == 9)
		{
		    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====the nDCS(%x) is the same as SMS ====", nDCS);
		}
		else if((nDCS >> 4) == 0x0F)
		{
		    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====the nDCS(%x) is the same as SMS ====", nDCS);
		}
	    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====nDCS = %x ====", nDCS);
		*pDCS = nDCS;
        pSimTerminalResponesReq->NbByte = pOffset - pSimTerminalResponesReq->TerminalRespData + pSimSatResponse->nSAT_InputLen + 1;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====pSimTerminalResponesReq->TerminalRespData = %x ====", pSimTerminalResponesReq->TerminalRespData);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====pSimTerminalResponesReq->NbByte = %x ====", pSimTerminalResponesReq->NbByte);
#else
        UINT8 nActLen = pSimSatResponse->nSAT_InputLen - 1;     //minus one byte of dcs
        UINT8 nDcs = pSimSatResponse->nSAT_InputStr[0];
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "nDcs = %x, nActLen = %d", nDcs, nActLen);
        if((nDcs >> 4) == 0x0F)
        {
            if(((nDcs >> 2) & 0x03) == 0)
            {
                nActLen = ((nActLen << 3) + 7)/ 7;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====nActLen = %d ====", nActLen);
            }
        }
        else if((nDcs >> 6) == 0)
        {
            if(((nDcs >> 2) & 0x03) == 0)
            {
                nActLen = ((nActLen << 3) + 7)/ 7;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "----nActLen = %d ====", nActLen);
            }
        }

        UINT8* pOffset = pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE;
        *pOffset = TEXT_STRING_TAG;     //pOffset point to TEXT_STRING_TAG
        pOffset++;                      //pOffset  point to an offset of the length of TEXT_STRING
        if(nActLen + 1 > 0x7F)		    //the length of TEXT String include dcs
        {
            *(pOffset) = 0x81;
            pOffset++;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "====pOffset = %x ====", pOffset);
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "nActLen = %d, pSimSatResponse->nSAT_InputLen - 1 = %d", nActLen, pSimSatResponse->nSAT_InputLen - 1);
        if(nActLen == pSimSatResponse->nSAT_InputLen - 1)
            SUL_MemCopy8(pOffset + 1, pSimSatResponse->nSAT_InputStr + 1, pSimSatResponse->nSAT_InputLen - 1);
        else
        {
            *(pOffset + 1) = (nDcs & 0xF0) + 4;
            nActLen = SUL_Decode7Bit(pSimSatResponse->nSAT_InputStr + 1, pOffset + 2, pSimSatResponse->nSAT_InputLen - 1);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "SUL_Decode7Bit = %d ====", nActLen);
        }
        nActLen++;      //Plus one byte Dcs.
        *(pOffset) = nActLen;
        pSimTerminalResponesReq->NbByte = pOffset - pSimTerminalResponesReq->TerminalRespData + 1 + nActLen;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "pSimTerminalResponesReq->NbByte = %d ====", pSimTerminalResponesReq->NbByte);
#endif
    }

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq->TerminalRespData, pSimTerminalResponesReq->NbByte, 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendCallControl(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_CALL_CONTROL[0xD4] command ERR_NO_MORE_MEMORY \n",0x08100e1c));
        return ERR_NO_MORE_MEMORY;
    }
    pSimTerminalEnvelopeReq->NbByte = pSimSatResponse->nSAT_InputLen + sizeof(CallControlDownload);
    CallControlDownload[1] = 4 + pSimSatResponse->nSAT_InputLen;
    SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, CallControlDownload, sizeof(CallControlDownload));
    SUL_MemCopy8((pSimTerminalEnvelopeReq->EnvelopeData + sizeof(CallControlDownload)), pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen);

    //pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber] = pSimSatResponse->nSAT_ItemID; // Select the second item
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_CALL_CONTROL[0xD4] sizeof(CallControlDownload) %d \n",0x08100e1d), sizeof(CallControlDownload));
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData, pSimTerminalEnvelopeReq->NbByte, 16);
    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_CALL_CONTROL[0xD4] command OK!!! \n",0x08100e1e));
    return ERR_SUCCESS;
}

UINT32 sendMenuSelection(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_MENU_SELECTION[0xD3] command ERR_NO_MORE_MEMORY \n",0x08100e1f));
        return ERR_NO_MORE_MEMORY;
    }
    pSimTerminalEnvelopeReq->NbByte = 0x09;

    SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, MenuSelection, pSimTerminalEnvelopeReq->NbByte);
    pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber] = pSimSatResponse->nSAT_ItemID; // Select the second item
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_MENU_SELECTION[0xD3] CMD TERMINALENVELOPE_REQ \n",0x08100e20));
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("=====================\n",0x08100e21));

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalEnvelopeReq->EnvelopeData, pSimTerminalEnvelopeReq->NbByte, 16);
    if((pSimSatResponse->nSAT_InputStr != NULL) && (pSimSatResponse->nSAT_InputLen > 0))
    {
        //ADD Help Request
        pSimTerminalEnvelopeReq->NbByte = 0x09 + 2; //Help Request is two size
        pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber + 1] = 0x15; // Help Request tag is 0x15
        pSimTerminalEnvelopeReq->EnvelopeData[ItemNumber + 2] = 0x00; // Help Request Length fill 0x00

        pSimTerminalEnvelopeReq->EnvelopeData[TextLength] = 0x07 + 2; //add help len
    }
    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_MENU_SELECTION[0xD3] command OK!!! \n",0x08100e22));
    return ERR_SUCCESS;
}

UINT32 sendEnvelopePDU(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID)
{
    api_SimTerminalEnvelopeReq_t *pSimTerminalEnvelopeReq =
        (api_SimTerminalEnvelopeReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalEnvelopeReq_t));
    if (pSimTerminalEnvelopeReq == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SAT PDU mode command ERR_NO_MORE_MEMORY \n",0x08100e23));
        return ERR_NO_MORE_MEMORY;
    }
    pSimTerminalEnvelopeReq->NbByte = pSimSatResponse->nSAT_InputLen;

    SUL_MemCopy8(pSimTerminalEnvelopeReq->EnvelopeData, pSimSatResponse->nSAT_InputStr,
                 pSimTerminalEnvelopeReq->NbByte);
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimTerminalEnvelopeReq->EnvelopeData, pSimTerminalEnvelopeReq->NbByte, 16);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SAT PDU mode envelop InputLen[%d]  CMD TERMINALENVELOPE_REQ \n",0x08100e24),
              pSimSatResponse->nSAT_InputLen);

    CFW_SendSclMessage(API_SIM_TERMINALENVELOPE_REQ, pSimTerminalEnvelopeReq, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0xFE command OK!!! \n",0x08100e25));
    return ERR_SUCCESS;
}

UINT32 sendProvideLocalInfoRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                               SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    if((pSimSatResponse == NULL) ||(pSimSatResponse->nSAT_InputStr == NULL))
        return ERR_PARAM_ILLEGAL;

    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));

    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    SUL_MemSet8(pSimTerminalResponesReq, 0x00, sizeof(api_SimTerminalResponseReq_t));
    pSimTerminalResponesReq->NbByte = RESPONSEBASE;

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pG_SimSat->nCmdQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pBase->nComQualifier   %x\n",0x08100e26), pG_SimSat->nCmdQualifier);
    if (pG_SimSat->nCmdQualifier == 0x00)
    {
        SAT_LOCALINFO_RSP *pLocalInfo = (SAT_LOCALINFO_RSP *)pSimSatResponse->nSAT_InputStr;
        pSimTerminalResponesReq->NbByte += RESPONSELOCAL;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseLocalInfo, RESPONSELOCAL);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + MCC_MNC) =
            (pLocalInfo->nMCC_MNC_Code[0] & 0x0F) + ((pLocalInfo->nMCC_MNC_Code[1] << 4) & 0xF0);;
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + MCC_MNC + 1) =
            (pLocalInfo->nMCC_MNC_Code[2] & 0x0F) + ((pLocalInfo->nMCC_MNC_Code[5] << 4) & 0xF0);
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + MCC_MNC + 2) =
            (pLocalInfo->nMCC_MNC_Code[3] & 0x0F) + ((pLocalInfo->nMCC_MNC_Code[4] << 4) & 0xF0);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + LAC) =
            (UINT8)(pLocalInfo->nLocalAreaCode >> 8);
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + LAC + 1) =
            (UINT8)(pLocalInfo->nLocalAreaCode & 0xFF);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + CellID) =
            (UINT8)(pLocalInfo->nCellIdentify >> 8);
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + CellID + 1) =
            (UINT8)(pLocalInfo->nCellIdentify & 0xFF);
    }
    else if (pG_SimSat->nCmdQualifier == 0x01)      //Provide IMEI
    {
        pSimTerminalResponesReq->NbByte += RESPONSEIMEI;
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE] = IMEI_TAG;     //IMEI tag
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + 1] = 0x08;     //Length of IMEI
        UINT8 nType = 0x02;
        UINT8 i = 0;
        UINT8 j = 2;
        UINT8* p = pSimSatResponse->nSAT_InputStr;
        p[pSimSatResponse->nSAT_InputLen - 1] = '0';        //the last digital should be zero.

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimSatResponse->nSAT_InputLen=%d",0x08100e27),pSimSatResponse->nSAT_InputLen);
        if((pSimSatResponse->nSAT_InputLen & 0x01) == 0x1)  //odd number
        {
            nType |= 0x08;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("odd number =%d",0x08100e28), nType);
        }
        pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + j++] = ((p[i++] - '0') << 4) | nType;
        while(i < pSimSatResponse->nSAT_InputLen)
        {
            pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + j++] = ((p[i+1] - '0') << 4) | (p[i] - '0');
            i += 2;
        }
        if((nType & 0x08) == 0)
        {
            pSimTerminalResponesReq->TerminalRespData[RESPONSEBASE + j - 1] |= 0xF0;
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("==============IMEI INFO===============",0x08100e29));
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq->TerminalRespData, pSimTerminalResponesReq->NbByte, 16);
    }
    else if (pG_SimSat->nCmdQualifier == 0x02)
    {
        SAT_MEASURE_RSP *pMeasure = (SAT_MEASURE_RSP *)pSimSatResponse->nSAT_InputStr;
        pSimTerminalResponesReq->NbByte += RESPONSEMEASURE;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseMeasure, RESPONSEMEASURE);
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + Measurement, pMeasure->nMeasure, 16);
    }
    else if (pG_SimSat->nCmdQualifier == 0x03)
    {
        SAT_DATETIME_RSP *pDatetime = (SAT_DATETIME_RSP *)pSimSatResponse->nSAT_InputStr;
        pSimTerminalResponesReq->NbByte += RESPONSEDATETIME;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseDateTime, RESPONSEDATETIME);
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + DateTime, pDatetime->nDateTimeZone, 7);
    }
    else if (pG_SimSat->nCmdQualifier == 0x04)
    {
        SAT_LANGUAGE_RSP *pLanguage = (SAT_LANGUAGE_RSP *)pSimSatResponse->nSAT_InputStr;
        pSimTerminalResponesReq->NbByte += RESPONSELANGUAGE;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseLanguage, RESPONSELANGUAGE);
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + Language, pLanguage->nLanguageCode, 2);
    }
    else if (pG_SimSat->nCmdQualifier == 0x05)
    {
        SAT_TIMINGADV_RSP *pTimingAdv = (SAT_TIMINGADV_RSP *)pSimSatResponse->nSAT_InputStr;
        pSimTerminalResponesReq->NbByte += RESPONSETADVANCE;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE,
                     ResponseTimingAdvance, RESPONSETADVANCE);
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + MEStatus) = pTimingAdv->nME_Status;
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + TimingAdvance) = pTimingAdv->nTimingAdv;
    }
    // CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq, sizeof(api_SimTerminalResponseReq_t), 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendGetReadStatusRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                            SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));

    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif

    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pG_SimSat->nCmdQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    if (pG_SimSat->nCmdQualifier == 0x00) // Card reader status
    {
        pSimTerminalResponesReq->NbByte += RESPONSECARDSTATUS;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE,
                     ResponseCardStatus, RESPONSECARDSTATUS);
        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + CardStatus) =
            pSimSatResponse->nSAT_InputLen;
    }
    else if (pG_SimSat->nCmdQualifier == 0x01)  // Card reader identify
    {
        pSimTerminalResponesReq->NbByte += RESPONSECARDID;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseCardStatus, RESPONSECARDID);

        *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + CardID_Length) =
            pSimSatResponse->nSAT_InputLen;
        SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + CardID,
                     pSimSatResponse->nSAT_InputStr, pSimSatResponse->nSAT_InputLen);
    }
    else
        return ERR_PARAM_ILLEGAL;

    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

UINT32 sendGetChannelStatusRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                               SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    //SAT_CHSTATUS_RSP* pChannelStatus = NULL;

    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE + RESPONSELOCAL;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pG_SimSat->nCmdQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE, ResponseLocalInfo, RESPONSECHANNEL);
    // *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + RESPONSECHANNEL) =
    //  (pChannelStatus->nChannelID) | (pChannelStatus->nStatus << 7);
    // *(pSimTerminalResponesReq->TerminalRespData + RESPONSEBASE + RESPONSECHANNEL + 1) = pChannelStatus->nType;
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

#if 0
UINT32 sendDisplayTextRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                          SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pG_SimSat->nCmdQualifier;

    if(pSimSatResponse != NULL)
    {
        pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
        pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nSAT_CmdType    %x,ComQual:0x%x\n",0x08100e2a), pSimSatResponse->nSAT_CmdType, pG_SimSat->nCmdQualifier);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status     %x\n",0x08100e2b), pSimSatResponse->nSAT_Status);
    }
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}
#endif

#if 0
UINT32 sendPlayToneRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));

    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;

    pSimTerminalResponesReq->NbByte = RESPONSEBASE;
    SUL_MemCopy8(pSimTerminalResponesReq->TerminalRespData, TerminalResponse, RESPONSEBASE);
#if 1
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] =
        nCommondNum[pSimSatResponse->nSAT_CmdType - 1][nSimID];
#else
    pSimTerminalResponesReq->TerminalRespData[ComType - 1] = pG_SimSat->nCmdNum;
#endif
    // pSimTerminalResponesReq->TerminalRespData[ComType -1]   = 0x02;
    pSimTerminalResponesReq->TerminalRespData[ComType]   = pSimSatResponse->nSAT_CmdType;
    pSimTerminalResponesReq->TerminalRespData[ComQual]   = pG_SimSat->nCmdQualifier;
    pSimTerminalResponesReq->TerminalRespData[ComResult] = pSimSatResponse->nSAT_Status;

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_CmdType    %x\n",0x08100e2c), pSimSatResponse->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nSAT_Status     %x\n",0x08100e2d), pSimSatResponse->nSAT_Status);

    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq, sizeof(api_SimTerminalResponseReq_t), 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}
#endif

UINT32 sendCommonRsp(CFW_SAT_RESPONSE *pSimSatResponse,UINT8 nSimID)
{
    api_SimTerminalResponseReq_t *pSimTerminalResponesReq =
        (api_SimTerminalResponseReq_t *)CFW_MemAllocMessageStructure(sizeof(api_SimTerminalResponseReq_t));
    if (pSimTerminalResponesReq == NULL)
        return ERR_NO_MORE_MEMORY;
    if(pSimSatResponse->nSAT_CmdType != CommandDetails[nSimID][3])
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("WARNING: Command ID is not equal to response!",0x08100e2e));
        CommandDetails[nSimID][0] = pSimSatResponse->nSAT_CmdType;
    }
    pSimTerminalResponesReq->NbByte = CreateTerminalResponse(pSimTerminalResponesReq->TerminalRespData,
                                      pSimSatResponse->nSAT_Status, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("send SAT Response: nSAT_CmdType = 0x%x\n",0x08100e2f), pSimSatResponse->nSAT_CmdType);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("send SAT Response: nSAT_Status = 0x%x\n",0x08100e30), pSimSatResponse->nSAT_Status);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" pSimTerminalResponesReq->NbByte = %d\n",0x08100e31), pSimTerminalResponesReq->NbByte);
    CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, (UINT8 *) pSimTerminalResponesReq->TerminalRespData, pSimTerminalResponesReq->NbByte, 16);
    CFW_SendSclMessage(API_SIM_TERMINALRESPONSE_REQ, pSimTerminalResponesReq, nSimID);
    return ERR_SUCCESS;
}

//pack GSM 7 bit default alphabet to 8 bit
//des, destination buffer
//src, source buffer, store data will be packed
//size, how many data will be packed.
UINT8 packGSM7bit(UINT8*des,UINT8* src,UINT8 size)
{
    UINT8 i = 0;
    UINT8 j = 0;
    UINT8 k = 0;
    if(size == 0)
        return 0;
    else if(size == 1)
    {
        *des = *src;
        return 1;
    }

    for(i = 0; i<size - 1; i++)
    {
        des[k] = ((UINT8)src[i]>>j++);
        des[k+1] = ((src[i+1]&(0xFF>>(8-j)))<<(8-j));
        des[k]=des[k] | des[k+1];
        k++;
        if(j == 7)
        {
            i++;
            j = 0;
        }
    }

    if(j != 0)
    {
        des[k]=src[i]>>j;
        k++;
    }
    else
    {
        if(i == size - 1)
            des[k++] = src[i];
    }
    return k;
}

UINT32 processFetchInd(CFW_EV *pEvent, UINT32 nUTI, CFW_SIM_ID nSimID)
{
    UINT8 nIndex        = 0;
    UINT8 nLength       = 0;

    UINT32 nReturnValue = 0;
    UINT32  nTransID = 0;
    nTransID = nUTI | (nSimID << 24);
    SIM_SAT_PARAM *pG_SimSat;
    CFW_CfgSimGetSatParam(&pG_SimSat, nSimID);

    api_SimFetchInd_t *pSimFetchInd = (api_SimFetchInd_t *)pEvent->nParam1;

    UINT8 *pOffset = pSimFetchInd->Data;
//Test ...
#if 0
    extern UINT8 bSatSimInit;
    //refresh
    //UINT8  nTest[] = {0xd0, 0x12, 0x81, 0x03, 0x01, 0x01, 0x02, 0x82, 0x02, 0x81, 0x82, 0x12, 0x07, 0x01, 0x3f, 0x00, 0x7f, 0x10, 0x6f, 0x3a};
    //setup menu
    //UINT8  nTest[] = {0xd0, 0x0d, 0x81, 0x03, 0x01, 0x25, 0x00, 0x82, 0x02, 0x81, 0x82, 0x85, 0x00, 0x8f, 0x00};
    if(bSatSimInit == 0)
    {
        pOffset = nTest;
    }
#endif
    nIndex = getLengthTLV(pOffset, &nLength);
    nLength += nIndex + 1;  //nLength is equal to total number of IND

    struct CommandDetail comdetail;
    getCommandDetail(pOffset + nIndex + 1, &comdetail);
    pG_SimSat->nCmdQualifier = comdetail.Qualifier;
    pG_SimSat->nCurCmd       = comdetail.Type;
    pG_SimSat->nCmdNum     = comdetail.Number;

    CommandDetails[nSimID][0] = comdetail.DetailsTag;
    CommandDetails[nSimID][1] = 3;
    CommandDetails[nSimID][2] = comdetail.Number;
    CommandDetails[nSimID][3] = comdetail.Type;
    CommandDetails[nSimID][4] = comdetail.Qualifier;

    DeviceIdentities[nSimID][0] =  pOffset[nIndex + 1 + 5]; //get Device Identities tag
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("<<< ComType =  %x,Cmd Num = %x, CmdQuallifier = %d>>>",0x08100e32), \
              comdetail.Type, comdetail.Number, comdetail.Qualifier);
    nCommondNum[pG_SimSat->nCurCmd - 1][nSimID] = pG_SimSat->nCmdNum;

    BOOL bRet = CFW_GetSATIndFormate();
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SM_RESP_FETCH: SAT mode =  %d,Cmd Type = 0x%x\n",0x08100e33), bRet, pG_SimSat->nCurCmd);
    if (pOffset[1] == 0x81)
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pOffset, pOffset[2] + 3, 16);
    else
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pOffset, pOffset[1] + 2, 16);

    if( CFW_SAT_IND_PDU == bRet )
    {
        UINT8 *pPDUData;
        pPDUData =  (UINT8 *)CSW_SIM_MALLOC(pSimFetchInd->Length);
        if(pPDUData != NULL)
        {
            SUL_MemCopy8(pPDUData, pOffset, pSimFetchInd->Length);
            if (pG_SimSat->pSatComData != NULL)
            {
                CSW_SIM_FREE(pG_SimSat->pSatComData);
                pG_SimSat->pSatComData = NULL;
            }
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, (UINT32)pPDUData, pSimFetchInd->Length, nTransID, 0);
            return ERR_SUCCESS;
        }
        else
        {
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
            return ERR_NO_MORE_MEMORY;
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Got API_SIM_FETCH_IND The command ID = [0x%x]\n",0x08100e34), pG_SimSat->nCurCmd);
    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nCmdNum : %x\n",0x08100e35), pG_SimSat->nCmdNum);

    nIndex  += 4 + 5 + 1;
    if (SIM_SAT_SETUP_MENU_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_MENU_RSP *pMenuItemList = NULL;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" pOffset + nIndex = %x\n",0x08100e36), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e37), nLength,  nLength - nIndex);
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pOffset + nIndex, nLength - 11, 16);

        nReturnValue = fetchSetupMenu(&pMenuItemList, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pMenuItemList->nComID = SIM_SAT_SETUP_MENU_IND;
            pMenuItemList->nComQualifier = pG_SimSat->nCmdQualifier;
            if(pG_SimSat->pSetupMenu != NULL)
                CSW_SIM_FREE(pG_SimSat->pSetupMenu);
            pG_SimSat->pSetupMenu = pMenuItemList;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("EV_CFW_SAT_CMDTYPE_IND = 0x25\n",0x08100e38));
        }
        else
        {
            pG_SimSat->pSetupMenu = NULL;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_FETCH_IND: ERROR\n",0x08100e39));
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
        }
        return nReturnValue;
    }
    else if ( SIM_SAT_SELECT_ITEM_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_MENU_RSP *pMenuItemList = NULL;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" pOffset + nIndex = %x\n",0x08100e3a), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e3b), nLength,  nLength - nIndex);
        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pOffset + nIndex, nLength - 11, 16);
        nReturnValue = fetchSelectItem(&pMenuItemList, pOffset + nIndex, nLength - nIndex);
        //nReturnValue = fetchSelectItem(pG_SimSat, pEvent);

        if(nReturnValue == ERR_SUCCESS)
        {
            pMenuItemList->nComID = pG_SimSat->nCurCmd ;
            pMenuItemList->nComQualifier = pG_SimSat->nCmdQualifier;

            if(pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pMenuItemList;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("EV_CFW_SAT_CMDTYPE_IND = 0x24\n",0x08100e3c));
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("API_SIM_FETCH_IND: ERROR\n",0x08100e3d));
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
        }
        return nReturnValue;
    }
    else if ( SIM_SAT_SEND_SMS_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_SMS_RSP *pSmsResp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SEND_SMS_IND: nIndex = %d\n",0x08100e3e), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e3f), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e40), nLength,  nLength - nIndex);
        nReturnValue = fetchSendSMS(&pSmsResp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pSmsResp->nComID        = pG_SimSat->nCurCmd;
            pSmsResp->nComQualifier = pG_SimSat->nCmdQualifier;

            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pSmsResp;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nComQualifier = %d\n",0x08100e41), pSmsResp->nComQualifier);
            if(pSmsResp->nComQualifier == 0x01)     //Need packing by ME
            {
                UINT8* pData = pSmsResp->pPDUStr;
                UINT8 TP_VPF= (pData[0] >> 3) & 0x03;   //Validity-Period-Format
                UINT8 lengthOfVPF = 0;
                if(pSmsResp->nPDULen < 4)           //the value of nPDULen must be large than 4 here, so skip following process...
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "The value of nPDULen(%d) must be large than 4 here", pSmsResp->nPDULen);
                    goto SendEvent;
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Validity-Period-Format = %d\n",0x08100e42), TP_VPF);
                if(TP_VPF == 0)     //not present
                    lengthOfVPF = 0;
                else if(TP_VPF == 2)    //present-relative format
                    lengthOfVPF = 1;
                else                    //present-enhanced format or present-absolute format
                    lengthOfVPF = 7;

                UINT8 TP_SRR= (pData[0] >> 7);          //Status-Report-Request
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Status-Report-Request = %d\n",0x08100e43), TP_SRR);

                UINT8 lengthOfAddress = pData[2];
                lengthOfAddress = (lengthOfAddress + 1) >> 1;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Length of Address = %d\n",0x08100e44), lengthOfAddress);

                UINT8 typeOfAddress = pData[3];
                CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pData + 4, lengthOfAddress, 16);

                UINT8 nOffset = 1 + 1 + (2 + lengthOfAddress);          //The length from SMS PDU to TP_PID
                if(pSmsResp->nPDULen < nOffset + 1 + 1 + lengthOfVPF)   //The length from SMS PDU to TP_UDL
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "The value of nPDULen(%d) must include the last mandatory item(TP_UDL), so skip following process...", pSmsResp->nPDULen);
                    goto SendEvent;
                }
                UINT8* pTP_PID = (pData + nOffset);
                UINT8* pTP_DCS = (pData + nOffset + 1);
                UINT8 TP_DCS = *pTP_DCS;
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Data-Coding-Scheme = %x\n",0x08100e45), TP_DCS);
                //pTP_UD is equal to Address of pData plus one byte of first byte, one byte of MR, DA, one byte of PID, DCS, VP,UDL
                UINT8 TP_UDL = *(pData + nOffset + 1 + 1 + lengthOfVPF);
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("User-Data-Length = %d\n",0x08100e46), TP_UDL);
                if((TP_UDL == 0)||(pSmsResp->nPDULen < nOffset + 1 + 1 + lengthOfVPF + 1))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "There is no user data!(TP_UDL = %d, nPDULen = %d)",TP_UDL ,pSmsResp->nPDULen);
                    goto SendEvent;
                }
                UINT8* pTP_UD = pData + nOffset + 1 + 1 + lengthOfVPF + 1;
                if(TP_DCS == 0)             //GSM 7 bit default alphabet with no message class
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "GSM 7 bit default alphabet with no message class!");
                    goto packedData;
                }
                else if((TP_DCS & 0xC0) == 0)       //group 00xx
                {
                    if((TP_DCS & 0x20) != 0)        //text is compressed
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "Text is compressed");
                        goto SendEvent;
                    }
                    if((TP_DCS & 0x10) == 0x10)  //bit 4 set to 1
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "Message Class = %d", TP_DCS&0x03);
                    else
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "No Message Class!");
                    UINT8 code = ((TP_DCS >> 2) & 0x03);
                    if(code == 0)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "GSM 7 bit default alphabet!");
	                    goto packedData;
                	}
                    else if(code == 1)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "8 bit data!");
                    else if(code == 2)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "UCS2(16 bit)");
                    else
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "Reserved!");
                    goto SendEvent;
                }
                else if((TP_DCS & 0xF0) == 0xF0)        //group 1111
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "Message Class = %d", TP_DCS&0x03);

                    if((TP_DCS & 0x08) != 0)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "WARNING: Bit 3 should be set to 0, but it is 1!");
                    UINT8 code = ((TP_DCS >> 2) & 0x03);
                    if(code == 0)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "GSM 7 bit default alphabet!");
					    goto packedData;
					}
                    else if(code == 1)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "8 bit data!");
                    else if(code == 2)
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "UCS2(16 bit)");
                    else
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "Reserved!");
					    goto SendEvent;

packedData:
                    {
                        UINT8 PDU[255] = {0};
						//check data whether it is 7bit or not
						for(UINT8 i = 0; i < TP_UDL; i++)
						{
							if((pTP_UD[i] >> 7) != 0)
                                goto SendEvent;
						}
                        UINT8 lengthOfUD = packGSM7bit(PDU, pTP_UD, TP_UDL);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("lengthOfUD = %d",0x08100e4a),lengthOfUD);
                        pSmsResp->nPDULen -= TP_UDL - lengthOfUD;
                        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, PDU, lengthOfUD, 16);

                        //for Testing
                        //==================================================================
                        //UINT8 Buffer1[140];
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("----------Buffer ---------",0x08100e4b));
                        //lengthOfUD = SUL_Encode7Bit(pTP_UD, Buffer1, TP_UDL);
                        //CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, Buffer1, lengthOfUD, 16);
                        //==================================================================

                        //store the compressed data, and change dcs
                        memcpy(pTP_UD, PDU, lengthOfUD);
                        //*pTP_DCS = (TP_DCS & 0xF0) + 4;
                        //*pTP_DCS = 0xF2;  //testing
                        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "---------DCS = %02x, TP_PID= %02x-------",*pTP_DCS, *pTP_PID);
                    }
                }
            }
SendEvent:
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);

        return nReturnValue;
    }
    else if ((SIM_SAT_DISPLAY_TEXT_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_GET_INKEY_IND == pG_SimSat->nCurCmd))
    {
        CFW_SAT_DTEXT_RSP *pDTextResp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_DISPLAY_TEXT_IND: nIndex = %d\n",0x08100e4d), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e4e), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e4f), nLength,  nLength - nIndex);
        nReturnValue = fetchDisplayText(&pDTextResp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pDTextResp->nComID        = pG_SimSat->nCurCmd;
            pDTextResp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pDTextResp;
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pG_SimSat->nCurCmd = %x\n",0x08100e50), pG_SimSat->nCurCmd);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pDTextResp->nComID  = %x\n",0x08100e51),  pDTextResp->nComID );
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID, 0xF0);
        return nReturnValue;
    }
    else if ((SIM_SAT_MORE_TIME_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_POLLING_OFF_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_POWER_OFF_CARD_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_POWER_ON_CARD_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_GET_READER_STATUS_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_GET_CHANNEL_STATUS_IND == pG_SimSat->nCurCmd) ||
             (SIM_SAT_PROVIDE_LOCAL_INFO_IND == pG_SimSat->nCurCmd) )
    {

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_MORE_TIME_IND: nIndex = %d\n",0x08100e52), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e53), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e54), nLength,  nLength - nIndex);
        //All of above command have the same structure, and no special data, so call fetchMore Time function.
        //nReturnValue = fetchMoreTime(pOffset + nIndex, nLength - nIndex);

        if (pG_SimSat->pSatComData != NULL)
            CSW_SIM_FREE(pG_SimSat->pSatComData);

        CFW_SAT_BASE_RSP *pBase = CSW_SIM_MALLOC(SIZEOF(CFW_SAT_BASE_RSP));
        if(pBase == NULL)
        {
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
            return ERR_NO_MORE_MEMORY;
        }
        else
        {
            pBase->nComID = pG_SimSat->nCurCmd;
            pBase->nComQualifier = pG_SimSat->nCmdQualifier;
            pG_SimSat->pSatComData = pBase;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
            return ERR_SUCCESS;
        }
    }
    else if ( SIM_SAT_GET_INPUT_IND == pG_SimSat->nCurCmd) // GetInput
    {
        CFW_SAT_INPUT_RSP *pInputResp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_GET_INPUT_IND: nIndex = %d\n",0x08100e55), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e56), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e57), nLength,  nLength - nIndex);
        nReturnValue = fetchGetInput(&pInputResp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pInputResp->nComID        = pG_SimSat->nCurCmd;
            pInputResp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);

            pG_SimSat->pSatComData = pInputResp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if ( SIM_SAT_CALL_SETUP_IND == pG_SimSat->nCurCmd) // set up call
    {
        CFW_SAT_CALL_RSP *pCallResp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_CALL_SETUP_IND: nIndex = %d\n",0x08100e58), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e59), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e5a), nLength,  nLength - nIndex);

        nReturnValue = fetchCallSetup(&pCallResp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pCallResp->nComID        = pG_SimSat->nCurCmd;
            pCallResp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pCallResp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if ( SIM_SAT_SETUP_EVENT_LIST_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_EVENTLIST_RSP *pEventList;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SETUP_EVENT_LIST_IND: nIndex = %d\n",0x08100e5b), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e5c), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e5d), nLength,  nLength - nIndex);
        nReturnValue = fetchSetupEventList(&pEventList, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pEventList->nComID = pG_SimSat->nCurCmd;
            pEventList->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pEventList;

            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("pEventList->nEventList=",0x08100e5e));
            UINT8 i = 0;
            for(i = 0; i < pEventList->nNumEvent; i++)
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("%x",0x08100e5f), pEventList->nEventList[i]);

            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if ( SIM_SAT_REFRESH_IND == pG_SimSat->nCurCmd) // Refresh
    {
        CFW_SAT_FILELIST_RSP *pFileList;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_REFRESH_IND: nIndex = %d\n",0x08100e60), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e61), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e62), nLength,  nLength - nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_REFRESH_IND: pG_SimSat->nCmdQualifier = %d\n",0x08100e63), pG_SimSat->nCmdQualifier);
        nReturnValue = fetchRefresh(&pFileList, pOffset + nIndex, nLength - nIndex, pG_SimSat->nCmdQualifier, nSimID);
        if(nReturnValue == ERR_SUCCESS)
        {
            pFileList->nComID = pG_SimSat->nCurCmd;
            pFileList->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pFileList;
#ifdef _DROPCARD_AUTOPIN_
            if(pFileList->nComQualifier == REFRESH_SIM_RESET)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL), "===REFRESH_SIM_RESET===");
                CFW_SimResetEx(nUTI, nSimID,TRUE);
                return nReturnValue;
            }
#endif
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, pG_SimSat->nCmdQualifier, nTransID, 0x0);
            CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("SIM_SAT_REFRESH_IND: Send Refresh command to MMI",0x08100e64));
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if (SIM_SAT_POLL_INTERVAL_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_POLL_INTERVAL_RSP *pPIResp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_POLL_INTERVAL_IND: nIndex = %d\n",0x08100e65), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e66), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e67), nLength,  nLength - nIndex);
        nReturnValue =  fetchPollInterval(&pPIResp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pPIResp->nComID        = pG_SimSat->nCurCmd;
            pPIResp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            _ResetSimTimer(pPIResp->nTimeInterval, pPIResp->nTimeUnit, nSimID);
            pG_SimSat->pSatComData = pPIResp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if( SIM_SAT_SEND_SS_IND == pG_SimSat->nCurCmd) // Send SS
    {
        CFW_SAT_SS_RSP *pSSRsp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SEND_SS_IND: nIndex = %d\n",0x08100e68), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e69), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e6a), nLength,  nLength - nIndex);

        nReturnValue =  fetchSendSS(&pSSRsp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pSSRsp->nComID        = pG_SimSat->nCurCmd;
            pSSRsp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);

            pG_SimSat->pSatComData = pSSRsp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);

        return nReturnValue;
    }
    else if( SIM_SAT_SEND_USSD_IND == pG_SimSat->nCurCmd) // Send USSD
    {
        CFW_SAT_USSD_RSP *pUSSDRsp;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SEND_USSD_IND: nIndex = %d\n",0x08100e6b), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e6c), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e6d), nLength,  nLength - nIndex);

        nReturnValue =  fetchSendUSSD(&pUSSDRsp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pUSSDRsp->nComID        = pG_SimSat->nCurCmd;
            pUSSDRsp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pUSSDRsp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);

        return nReturnValue;
    }
    if(SIM_SAT_PLAY_TONE_IND == pG_SimSat->nCurCmd)
    {
        CFW_SAT_PLAYTONE_RSP* pPlayTone;
        nReturnValue =  fetchPlayTone(&pPlayTone, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pPlayTone;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;

    }
    else if( SIM_SAT_LAUNCH_BROWSER_IND == pG_SimSat->nCurCmd) // Launch Browser
    {
#if 0
//following for test
//-----No session already launched
        /* Test case 1
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL empty
        Alpha Identifier    "Default URL"
        */
//UINT8* nTestLaunchBrowser = "D0188103011500820281823100050B44656661756C742055524C";

        /*  Test case 2
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    empty
        */
//UINT8* nTestLaunchBrowser = "D01F8103011500820281823112687474703A2F2F7878782E7979792E7A7A7A0500";

        /*  Test case 3
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            Browser Identity    default
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)

        */
//UINT8* nTestLaunchBrowser = "D0208103011500820281823001003112687474703A2F2F7878782E7979792E7A7A7A";

        /*  Test case 4
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
            Bearer  GPRS
        Gateway/Proxy id
            DCSunpacked, 8 bits data
            Text string:    abc.def.ghi.jkl (different from the default IP address)
        */
//UINT8* nTestLaunchBrowser = "D0328103011500820281823112687474703A2F2F7878782E7979792E7A7A7A3201030D10046162632E6465662E6768692E6A6B6C";

//-----Interaction with current session
        /*  Test case 1
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  use the existing browser
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Defined URL"
        */
//UINT8* nTestLaunchBrowser = "D02A8103011502820281823112687474703A2F2F7878782E7979792E7A7A7A050B446566696E65642055524C";

        /*  Test case 2
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  close the existing browser session and launch new browser session
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Defined URL"
        */
//UINT8* nTestLaunchBrowser = "D02A8103011503820281823112687474703A2F2F7878782E7979792E7A7A7A050B446566696E65642055524C";

////-----icons support
        /*  Test case 1
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  use the existing browser
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Not self explan."
        Icon identifier:
            Icon qualifier: not self-explanatory
            Icon identifier:    record 1 in EF(IMG)
        */
//UINT8* nTestLaunchBrowser = "D0338103011502820281823112687474703A2F2F7878782E7979792E7A7A7A05104E6F742073656C66206578706C616E2E1E020101";

        /*  Test case 2
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  use the existing browser
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Self explan."
        Icon identifier:
            Icon qualifier: self-explanatory
            Icon identifier:    record 1 in EF(IMG)

        */
//UINT8* nTestLaunchBrowser = "D02F8103011502820281823112687474703A2F2F7878782E7979792E7A7A7A050C53656C66206578706C616E2E1E020001";

//support of Text Attribute--- This is for UCAT, this version doesn't supported.
        /* Test case 1
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Defined URL 1"
        Text Attribute
            Formatting position:    0
            Formatting length:  13
            Formatting mode:    Left Alignment, Normal Font, Bold Off, Italic Off, Underline Off, Strikethrough Off
            Colour: Dark Green Foreground, Bright Yellow Background
        */
//UINT8* nTestLaunchBrowser = "D0328103011500820281823112687474703A2F2F7878782E7979792E7A7A7A050D446566696E65642055524C2031D004000D00B4";

        /* Test case 2
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier        "Defined URL 2"
        */
//UINT8* nTestLaunchBrowser = "D02C8103011500820281823112687474703A2F2F7878782E7979792E7A7A7A050D446566696E65642055524C2032";

        /*  Test case 3
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  launch browser, if not already launched
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier    "Defined URL 1"
        Text Attribute
            Formatting position:    0
            Formatting length:  13
            Formatting mode:    Center Alignment, Normal Font, Bold Off, Italic Off, Underline Off, Strikethrough Off
        Colour: Dark Green Foreground, Bright Yellow Background
        */
//UINT8* nTestLaunchBrowser = "D0328103011500820281823112687474703A2F2F7878782E7979792E7A7A7A050D446566696E65642055524C2031D004000D01B4";

//-----LAUNCH BROWSER (UCS2 Display in Chinese)
        /*
        Command details
            Command number: 1
            Command type:   LAUNCH BROWSER
            Command qualifier:  use the existing browser
        Device identities
            Source device:  UICC
            Destination device: ME
            URL http://xxx.yyy.zzz (Note: this URL shall be different from the default URL, but it can be reached from the gateway defined by default in the browser parameters of the mobile)
        Alpha Identifier
            Data coding scheme: UCS2 (16 bits)
            Text:   "你好"
        */
//UINT8* nTestLaunchBrowser = "D0248103011502820281823112687474703A2F2F7878782E7979792E7A7A7A0505804F60597D";
        UINT8 TestInd[511] = {0};
        SUL_ascii2hex(nTestLaunchBrowser, TestInd);
        pSimFetchInd->Length = SUL_Strlen(nTestLaunchBrowser)>>1;
        pOffset = TestInd;

        nIndex = getLengthTLV(pOffset, &nLength);
        nLength += nIndex + 1;

        struct CommandDetail comdetail;
        getCommandDetail(pOffset + nIndex + 1, &comdetail);
        pG_SimSat->nCmdQualifier = comdetail.Qualifier;
        pG_SimSat->nCurCmd       = comdetail.Type;
        pG_SimSat->nCmdNum     = comdetail.Number;

        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("<<< ComType =  %x,Cmd Num = %x, CmdQuallifier = %d>>>",0x08100e6e), \
                  comdetail.Type, comdetail.Number, comdetail.Qualifier);

        nCommondNum[pG_SimSat->nCurCmd - 1][nSimID] = pG_SimSat->nCmdNum;
        nIndex  += 4 + 5 + 1;
#endif
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_LAUNCH_BROWSER_IND: nIndex = %d\n",0x08100e6f), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e70), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e71), nLength,  nLength - nIndex);
        CFW_SAT_BROWSER_RSP *pBrowserRsp;
        nReturnValue =  fetchLaunchBrowser(&pBrowserRsp, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pBrowserRsp->nComID = pG_SimSat->nCurCmd;
            pBrowserRsp->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pBrowserRsp;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if(SIM_SAT_TIMER_MANAGEMENT_IND  == pG_SimSat->nCurCmd)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_TIMER_MANAGEMENT_IND: nIndex = %d\n",0x08100e72), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e73), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e74), nLength,  nLength - nIndex);
        CFW_SAT_TIMER_RSP *pTimer;
        nReturnValue = fetchTimerManagement(&pTimer, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pTimer->nComID = pG_SimSat->nCurCmd;
            pTimer->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pTimer;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else if(SIM_SAT_SEND_DTMF_IND  == pG_SimSat->nCurCmd)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM_SAT_SEND_DTMF_IND: nIndex = %d\n",0x08100e75), nIndex);
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR(" *(pOffset + nIndex) = %x\n",0x08100e76), *( pOffset + nIndex));
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("nLength = %d, maxlen = %d\n",0x08100e77), nLength,  nLength - nIndex);
        CFW_SAT_SEND_DTMF_RSP *pDTMF;
        nReturnValue = fetchDTMF(&pDTMF, pOffset + nIndex, nLength - nIndex);
        if(nReturnValue == ERR_SUCCESS)
        {
            pDTMF->nComID = pG_SimSat->nCurCmd;
            pDTMF->nComQualifier = pG_SimSat->nCmdQualifier;
            if (pG_SimSat->pSatComData != NULL)
                CSW_SIM_FREE(pG_SimSat->pSatComData);
            pG_SimSat->pSatComData = pDTMF;
            CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
        }
        else
            CFW_PostNotifyEvent(EV_CFW_SAT_RESPONSE_RSP, nReturnValue, 0, nTransID , 0xF0);
        return nReturnValue;
    }
    else
    {
        SAT_BASE_RSP *pBaseResp = (SAT_BASE_RSP *)CSW_SIM_MALLOC(SIZEOF(SAT_BASE_RSP));
        if (pBaseResp == NULL)
            return ERR_NO_MORE_MEMORY;

        pBaseResp->nComID = pG_SimSat->nCurCmd;
        pBaseResp->nComQualifier = pG_SimSat->nCmdQualifier;
        CSW_TRACE(_CSW|TLEVEL(CFW_SAT_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("<<<<This Command cannot be processed = 0x%x>>>>\n",0x08100e78), pG_SimSat->nCurCmd);

        if (pG_SimSat->pSatComData != NULL)
            CSW_SIM_FREE(pG_SimSat->pSatComData);
        pG_SimSat->pSatComData = pBaseResp;
        CFW_PostNotifyEvent(EV_CFW_SAT_CMDTYPE_IND, pG_SimSat->nCurCmd, 0, nTransID, 0x0);
    }
    return ERR_SUCCESS;
}

