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

#ifdef CFW_GPRS_SUPPORT
#include <csw.h>

#ifndef _CFW_GPRS_TOOL_H_
#define _CFW_GPRS_TOOL_H_

#include <cfw_config_prv.h>

#if (CFW_NW_DUMP_ENABLE==0) && (CFW_NW_SRV_ENABLE==1)

#include "cfw_gprs_data.h"
#include <cswtype.h>
#include <cfw.h>
#include "sul.h"

typedef struct _CFW_GPRS_SSMQOS
{
    UINT8 Length;

    struct
    {
        UINT8 ReliabilityClass : 3;
        // ms to network
        // 0 : subscribed reliability class
        // network to ms and ms to network
        // 1 : acknowledged GTP, LLC and RLC
        // 2 : unacknowledged GTP, ack LLC and RLC
        // 3,6 : unack GTP and LLC, ack RLC
        // 4 : unack GTP, LLC and RLC
        // 5 : unack GTP, LLC and RLC, unprotected data
        UINT8 DelayClass : 3;
        // ms to network
        // 0 : subscribed delay class
        // ms to network and network to MS
        // 1 : delay class 1
        // 2 : delay class 2
        // 3 : delay class 3
        // 4,5,6 : delay class 4(best effort)
        UINT8 Spare0 : 2; // 0 : spare
        UINT8 PrecedenceClass : 3;
        // ms to network
        // 0 : subscribed precedence
        // ms to network and network to ms
        // 1 : high prioity
        // 2,4,5,6 : normal priority
        // 3 : low priority
        UINT8   Spare1 : 1; // 0 : spare
        UINT8   PeakThroughput : 4;
        // ms to network
        // 0 : subscribed peak throughput
        // ms to network and network to ms
        // 1,10..14 : up to 1000 octet/s
        // 2 : up to 2000 octet/s
        // 3 : up to 4000 octet/s
        // 4 : up to 8000 octet/s
        // 5 : up to 16000 octet/s
        // 6 : up to 32000 octet/s
        // 7 : up to 64000 octet/s
        // 8 : up to 128000 octet/s
        // 9 : up to 256000 octet/s
        UINT8   MeanThroughput : 5;
        // ms to network
        // 0 : subscribed mean throughput
        // Ms to network and network to ms
        // 1 : 100 octet/h
        // 2 : 200 octet/h
        // 3 : 500 octet/h
        // 4 : 1000 octet/h
        // 5 : 2000 octet/h
        // 6 : 5000 octet/h
        // 7 : 10000 octet/h
        // 8 : 20000 octet/h
        // 9 : 50000 octet/h
        // 10 : 100000 octet/h
        // 11 : 200000 octet/h
        // 12 : 500000 octet/h
        // 13 : 1000000 octet/h
        // 14 : 2000000 octet/h
        // 15 : 5000000 octet/h
        // 16 : 10000000 octet/h
        // 17 : 20000000 octet/h
        // 18 : 50000000 octet/h
        // 19..29,31 : best effort
        UINT8 Spare2 : 3; // 0 : spare
        UINT8 DeliveryOfErroneousSDU : 3;
        // ms to network
        // 0 : subscribed delivery of erroneous SDUs
        // ms to network and network to ms
        // 1 : no detect
        // 2 : erroneous SDUs are delivered
        // 3 : erroneous SDUs are not delivered
        UINT8 DeliveryOrder : 2;
        // ms to network
        // 0 : subscribed delivery order
        // ms to network and network to ms
        // 1 : with delivery order
        // 2 : without delivery order
        UINT8 TrafficClass : 3;
        // ms to network
        // 0 : subscribed tranffic class
        // ms to network and network to ms
        // 1 : conversational class
        // 2 : streaming class
        // 3 : interactive class
        // 4 : background class
        UINT8 MaximumSDUSize;
        // ms to network
        // 0 : subscribed maximum SDU size
        // ms to network and network to ms
        // 1..150 : 10..1500 octets
        // 151 : 1502 octets
        // 152 : 1510 octets
        // 153 : 1520 octets
        UINT8 MaximumBitRateForUplink;
        // ms to network
        // 0 : subscribed maximum bit rate for uplink
        // ms to network and network to ms
        // 1 : 1kbps
        // 63 : 1kbps to 63kbps
        // 64 : b4kpbs+
        // 127 : 64kpbs to 568kbps
        // 128 : 576kbps+
        // 254 : 576kbps to 8640kbps
        // 255 : 0kbps
        UINT8 MaximumBitRateForDownlink;  // see MaximumBitRateForUplink
        UINT8 SDUErrorRatio : 4;
        // ms to network
        // 0 : subscribed SDU error ratio
        // ms to network and network to ms
        // 1 : 1 * 10-2
        // 2 : 7 * 10-3
        // 3 : 1 * 10-3
        // 4 : 1 * 10-4
        // 5 : 1 * 10-5
        // 6 : 1 * 10-6
        // 7 : 1 * 10-1
        UINT8 ResidualBER : 4;
        // ms to network
        // 0 : subscribed residual BER
        // ms to network and network to ms
        // 1 : 5 * 10-2
        // 2 : 1 * 10-2
        // 3 : 5 * 10-3
        // 4 : 4 * 10-3
        // 5 : 1 * 10-3
        // 6 : 1 * 10-4
        // 7 : 1 * 10-5
        // 8 : 1 * 10-6
        // 9 : 6 * 10-8
        UINT8 TrafficHandlingPriority : 2;
        // ms to network
        // 0 : subscribed traffic handling priority
        // ms to network and network to ms
        // 1 : priority level 1
        // 2 : priority level 2
        // 3 : priority level 3
        UINT8 TransferDelay : 6;
        // ms to network
        // 0 : subscribed transfer delay
        // ms to network and network to ms
        // 1 : 10 ms
        // 15 : 10 to 150ms
        // 16 : 200ms+
        // 31 200 to 950ms
        // 32: 1000ms+
        // 62 : 1000ms to 4000ms
        UINT8 GuaranteedBitRateForUplink;   // see MaximumBitRateForUplink
        UINT8 GuaranteedBitRateForDownlink; // see MaximumBitRateForUplink
        UINT8                       bSrcStatDescriptor: 4;
        UINT8                       bSignalingIndication: 1;
        UINT8                       bSpare: 3;              /* octet 13 */
        UINT8                       bExtMaxBitRateDL;       /* octet 14 */
        UINT8                       bExtGuaranteedBitRateDL;/* octet 15 */
    } QOS;
} CFW_GPRS_SSMQOS; // refer 3GPP 24008

// qos to api covert
//[in] Qos
//[out]ApiQos
void CFW_GprsQos2Api(CFW_GPRS_QOS sQos, UINT8 ApiQos[11], UINT8 *pQosLength);
void CFW_GprsQos2ApiUtms(CFW_GPRS_QOS sQos, UINT8 ApiQos[14], UINT8 *pQosLength);

// qos to api covert
//[in]ApiQos
//[out] pQos
void CFW_GprsApi2Qos(CFW_GPRS_QOS *pQos, UINT8 ApiQos[11], UINT8 QosLength);
//[in]pPdpCXT
//[out]pPdpAddLen
//[out]pPdpAdd
void CFW_GprsPdpAddr2Api(CFW_GPRS_PDPCONT_INFO *pPdpCXT, UINT8 *pPdpAddLen, UINT8 *pPdpAdd);
//[out]pPdpCXT
//[in]pPdpAddLen
//[in]pPdpAdd
void CFW_GprsPdpApi2Addr(CFW_GPRS_PDPCONT_INFO *pPdpCXT, UINT8 pPdpAddLen, UINT8 *pPdpAdd);

UINT8 CFW_GprsGetFreeCid(CFW_SIM_ID nSimID);

struct CurrentAppCng
{
    BOOL CurAppSet;// true: can be set; false: can't be set;
    HANDLE CurrentAppTaskHandle;
};

#define CFW_GPRS_TRACE CFW_GPRS_TS_ID
#define CFW_GPRS_TRACE_DATA  (_CSW| TLEVEL(9))
//#define CFW_GPRS_TRACE_DATA  (_CSW)

#define CFW_Dump_SXS(id, pdata, size) SXS_DUMP(id, 0, pdata, size)


HAO GprsMTAoProc(CFW_EV *pEvent);
#ifdef LTE_NBIOT_SUPPORT
UINT32 CFW_GprsSendCtxCfg(UINT8 nSimID );
UINT32 CFW_GprsSetCtxEQos(UINT8 nCid, CFW_EQOS* pQos,UINT8 nSimID );
UINT32 CFW_GprsGetCtxEQos(UINT8 nCid, CFW_EQOS* pQos,CFW_SIM_ID nSimID );
UINT32 CFW_GprsSetCtxTFT(UINT8 nCid, CFW_TFT_SET  *pTft, UINT8 nSimID );
UINT32 CFW_GprsGetCtxTFT(UINT8 nCid, CFW_TFT_SET **pTft, UINT8 nSimID );
UINT8  CFW_GprsPfMatching(CFW_SIM_ID nSimID, UINT8 LinkEbi, UINT16 nDataSize, UINT8 *pData);
HAO    EpsMTAoProc(CFW_EV *pEvent);
UINT32 CFW_GprsSecPdpAct(UINT8 nState, UINT8 nCid, UINT8 nPCid, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_GprsCtxEpsModify(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID);
VOID   CFW_GprsEQos2Api(CFW_EQOS* pQos, UINT8 ApiQos[13], UINT8 *Length);
VOID   CFW_GprsApi2EQoS(CFW_EQOS *pQos, UINT8 ApiQos[13], UINT8 Length);
UINT8  CFW_GprsAddrCompare(UINT8* Addr1, UINT8* Addr2, UINT8* Mask, UINT8 MaskBits);
VOID*  CFW_GprsTftPfMalloc(VOID);
VOID*  CFW_GprsPfCpntMalloc(VOID);
VOID   CFW_GprsTftPfFree(CFW_TFT_PF* pTftPf);
UINT8  CFW_GprsGetIPv6Prefix(UINT8* Mask);
UINT8  CFW_GprsTftHasEbi(CFW_TFT_PF *tftPf,UINT8 Ebi);
UINT8  CFW_GprsTftHasUpDir(CFW_TFT_PF *pfHead);
UINT8  CFW_GprsNewTftHasSamePriority(CFW_TFT_PF *newTftPf);
UINT8  CFW_GprsTftPfHasSameIDInAct(CFW_TFT_PF *newPfHead,CFW_TFT_PF *existPf,UINT8 Ebi);
UINT8  CFW_GprsTftFindOldSamePrio(CFW_TFT_PF *newPfHead, CFW_TFT_PF *existPf, CFW_TFT_PARSE *pTftParse);
UINT8  CFW_GprsTftMsgParseWithComponent(CFW_TFT* pTft, UINT8 Ebi, CFW_TFT_PF **retTftPf);
UINT8  CFW_GprsTftDelPfMsgParse(CFW_TFT* pTft, UINT8 Ebi, CFW_TFT_PF **retTftPf);
UINT8  CFW_GprsTftParsePf(UINT8 pfListBegin,UINT8* tft,PF_HEAD *pfHead,CFW_TFT_PF *newTftPf);
UINT8  CFW_GprsPfAttriIsValid(CFW_PF_CPNT *cpnt);
VOID   CFW_GprsTftFreeAll(CFW_TFT_PF **pfHead);
VOID*  CFW_GprsGetCid2IndexInfo(UINT8 nCid, UINT8 nSimID);


CFW_TFT_PF *CFW_GprsTftClone(CFW_TFT_PF *orignal);
CFW_TFT_PF *CFW_GprsTftDeleteSameEbi(CFW_TFT_PF **tftPf,UINT8 Ebi);
CFW_TFT_PF *CFW_GprsTftFindAndDelSameID(CFW_TFT_PF *newPfHead,CFW_TFT_PF *existPf,CFW_TFT_PF **tftPf,UINT8 Ebi);
CFW_TFT_PF *CFW_GprsTftInsertPf(CFW_TFT_PF **head,CFW_TFT_PF *newTftPf);


UINT32 CFW_SendPduData(UINT8 nNsapi, UINT16 nDataSize, UINT8 *pData, UINT8 rai, UINT16 type_of_user_data,CFW_SIM_ID nSimID, UINT8 v_dataNum);
#endif //LTE_NBIOT_SUPPORT

extern HANDLE CFW_bal_GetTaskHandle(UINT8 nTaskId);

#endif
#endif
#endif

