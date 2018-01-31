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

#include <cfw_config_prv.h>

#if (CFW_NW_DUMP_ENABLE==0) && (CFW_NW_SRV_ENABLE==1)
#include <cfw.h>
#include <cfw_prv.h>
#include "cfw_gprs_tool.h"

// qos to api covert
//[in] Qos
//[out]ApiQos
//[out] pQosLength: qos length for api

void CFW_GprsQos2Api(CFW_GPRS_QOS sQos, UINT8 ApiQos[11], UINT8 *pQosLength)
{
    CFW_GPRS_SSMQOS TempQos;

    TempQos.Length = 11;

    SUL_MemSet8(&TempQos.QOS, 0x00, (UINT32)TempQos.Length);

    //TempQos.QOS.ReliabilityClass = sQos.nReliability;//modify by wuys 2008-01-21
    //TempQos.QOS.DelayClass = sQos.nDelay;//modify by wuys 2008-01-21
    //TempQos.QOS.Spare0 = 0;
    //TempQos.QOS.PrecedenceClass = sQos.nPrecedence;//modify by wuys 2008-01-21
    //TempQos.QOS.Spare1 = 0;
    //TempQos.QOS.PeakThroughput = sQos.nPeak;//modify by wuys 2008-01-21
    //TempQos.QOS.MeanThroughput = sQos.nMean;//modify by wuys 2008-01-21
    TempQos.QOS.ReliabilityClass = 3;//modify by wuys 2008-01-21
    TempQos.QOS.DelayClass = 0;//modify by wuys 2008-01-21
    TempQos.QOS.Spare0 = 0;
    TempQos.QOS.PrecedenceClass = 0;//modify by wuys 2008-01-21
    TempQos.QOS.Spare1 = 0;
    TempQos.QOS.PeakThroughput = 0;//modify by wuys 2008-01-21
    TempQos.QOS.MeanThroughput = 31;//modify by wuys 2008-01-21
    TempQos.QOS.Spare2 = 0;
    TempQos.QOS.DeliveryOfErroneousSDU = 0;
    TempQos.QOS.DeliveryOrder = 2;
    TempQos.QOS.TrafficClass = 0;
    TempQos.QOS.MaximumSDUSize = 0x00;
    TempQos.QOS.MaximumBitRateForUplink = 0x00;
    TempQos.QOS.MaximumBitRateForDownlink = 0x00;
    TempQos.QOS.SDUErrorRatio = 0;
    TempQos.QOS.ResidualBER = 0;
    TempQos.QOS.TrafficHandlingPriority = 0;
    TempQos.QOS.TransferDelay = 0;
    TempQos.QOS.GuaranteedBitRateForUplink = 0;
    TempQos.QOS.GuaranteedBitRateForDownlink = 0;

    //SUL_MemSet8(&TempQos.QOS, 0x00, (UINT32)TempQos.Length);
    //TempQos.QOS.MeanThroughput = 0x1F;//modify by wuys 2010-09-20, refer to Nokia

    SUL_MemCopy8(ApiQos, &TempQos.QOS, (UINT32)TempQos.Length);

    *pQosLength = TempQos.Length;
}

// qos to api covert
//[in] Qos
//[out]ApiQos
//[out] pQosLength: qos length for api
#ifdef __MULTI_RAT__

void CFW_GprsQos2ApiUtms(CFW_GPRS_QOS sQos, UINT8 ApiQos[14], UINT8 *pQosLength)
{
    CFW_GPRS_SSMQOS TempQos;
    TempQos.Length = 14;
    SUL_MemSet8(&TempQos.QOS, 0x00, (UINT32)TempQos.Length);
#if 0
    TempQos.QOS.ReliabilityClass = 3;       //0 1 1 Unacknowledged GTP and LLC; Acknowledged RLC, Protected data
    TempQos.QOS.DelayClass = 0;             //0 1 1 Delay class 3
    TempQos.QOS.Spare0 = 0;
    TempQos.QOS.PrecedenceClass = 0;        // 0 0 1   High priority
    TempQos.QOS.Spare1 = 0;
    TempQos.QOS.PeakThroughput = 0;         // 1 0 0 1     Up to 256 000 octet/s
    TempQos.QOS.MeanThroughput = 31;        //  1 0 0 1 0    50 000 000 octet/h
    TempQos.QOS.Spare2 = 0;
    TempQos.QOS.DeliveryOfErroneousSDU = 0; //  0 1 0       Erroneous SDUs are delivered ('yes')
    TempQos.QOS.DeliveryOrder = 2;          //   1 0     Without delivery order ('no')
    TempQos.QOS.TrafficClass = sQos.nTrafficClass;
    TempQos.QOS.MaximumSDUSize = 0x00;      //            1 0 0 1 1 0 0 1     1520 octets
    TempQos.QOS.MaximumBitRateForUplink = 0x00; //     1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.MaximumBitRateForDownlink =  0x00;  //      1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.SDUErrorRatio = 0;          //   0 0 0 1       1*10-2
    TempQos.QOS.ResidualBER = 0;            // 0 0 0 1     5*10-2
    TempQos.QOS.TrafficHandlingPriority = 0;//   0 1    Priority level 1
    TempQos.QOS.TransferDelay = 0;          //      0 0 0 0 0 1   The Transfer delay is binary coded in 6 bits, using a granularity of 10 ms
    TempQos.QOS.GuaranteedBitRateForUplink = 0x00;// 1 1 1 1 1 1 1 0     giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.GuaranteedBitRateForDownlink =  0x00;//     1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.bSrcStatDescriptor = 0;     //   0 0 0 0      unknown
    TempQos.QOS.bSignalingIndication = 0;   //  0      Not optimised for signalling traffic
    TempQos.QOS.bSpare = 0;
    TempQos.QOS.bExtMaxBitRateDL = 0;       //  0 0 0 0 0 0 0 0  Use the value indicated by the Maximum bit rate for downlink in octet 9.
    TempQos.QOS.bExtGuaranteedBitRateDL = 0;//  0 0 0 0 0 0 0 0   Use the value indicated by the Guaranteed bit rate for downlink in octet 13.
#endif
    TempQos.QOS.ReliabilityClass = sQos.nReliability;
    TempQos.QOS.DelayClass = sQos.nDelay;           //0 1 1 Delay class 3
    TempQos.QOS.Spare0 = 0;
    TempQos.QOS.PrecedenceClass = sQos.nPrecedence;         // 0 0 1   High priority
    TempQos.QOS.Spare1 = 0;
    TempQos.QOS.PeakThroughput = sQos.nPeak;        // 1 0 0 1     Up to 256 000 octet/s
    TempQos.QOS.MeanThroughput = sQos.nMean;        //  1 0 0 1 0    50 000 000 octet/h
    TempQos.QOS.Spare2 = 0;
    TempQos.QOS.DeliveryOfErroneousSDU = sQos.nDeliveryOfErroneousSDU;  //  0 1 0       Erroneous SDUs are delivered ('yes')
    TempQos.QOS.DeliveryOrder = sQos.nDeliveryOrder;            //   1 0     Without delivery order ('no')
    TempQos.QOS.TrafficClass = sQos.nTrafficClass;
    TempQos.QOS.MaximumSDUSize = sQos.nMaximumSDUSize;      //            1 0 0 1 1 0 0 1     1520 octets
    TempQos.QOS.MaximumBitRateForUplink = sQos.nMaximumbitrateUL; //       1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.MaximumBitRateForDownlink =  sQos.nMaximumbitrateDL;    //      1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.SDUErrorRatio = sQos.nSDUErrorRatio;            //   0 0 0 1       1*10-2
    TempQos.QOS.ResidualBER = sQos.nResidualBER;            // 0 0 0 1     5*10-2
    TempQos.QOS.TrafficHandlingPriority = sQos.nTrafficHandlingPriority;//   0 1    Priority level 1
    TempQos.QOS.TransferDelay = sQos.nTransferDelay;            //      0 0 0 0 0 1   The Transfer delay is binary coded in 6 bits, using a granularity of 10 ms
    TempQos.QOS.GuaranteedBitRateForUplink = sQos.nGuaranteedbitrateUL;// 1 1 1 1 1 1 1 0    giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.GuaranteedBitRateForDownlink =  sQos.nGuaranteedbitrateDL;//        1 1 1 1 1 1 1 0   giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments
    TempQos.QOS.bSrcStatDescriptor = 0;     //   0 0 0 0      unknown
    TempQos.QOS.bSignalingIndication = 0;   //  0      Not optimised for signalling traffic
    TempQos.QOS.bSpare = 0;
    TempQos.QOS.bExtMaxBitRateDL = 0;       //  0 0 0 0 0 0 0 0  Use the value indicated by the Maximum bit rate for downlink in octet 9.
    TempQos.QOS.bExtGuaranteedBitRateDL = 0;//  0 0 0 0 0 0 0 0   Use the value indicated by the Guaranteed bit rate for downlink in octet 13.
    SUL_MemCopy8(ApiQos, &TempQos.QOS, (UINT32)TempQos.Length);
    *pQosLength = TempQos.Length;
}
#endif
// qos to api covert
//[in]ApiQos
//[out] pQos
void CFW_GprsApi2Qos(CFW_GPRS_QOS *pQos, UINT8 ApiQos[11], UINT8 QosLength)
{
    CFW_GPRS_SSMQOS TempQos;
    SUL_MemCopy8(&TempQos.QOS, ApiQos, (UINT32)QosLength);
    TempQos.Length = QosLength;
    pQos->nDelay = TempQos.QOS.DelayClass;
    pQos->nMean = TempQos.QOS.MeanThroughput;
    pQos->nPeak = TempQos.QOS.PeakThroughput;
    pQos->nPrecedence = TempQos.QOS.PrecedenceClass;
    pQos->nReliability = TempQos.QOS.ReliabilityClass;

}
#ifdef LTE_NBIOT_SUPPORT
// CFW Bit Rate to EPS QoS
UINT8 CFW_GprsBitRate2EQoS(UINT32 BitRate)
{
    UINT8 ret = 0;
    /*
    In UE to network direction and in network to UE direction:
    0 0 0 0 0 0 0 1     The maximum bit rate is binary coded in 8 bits, using a granularity of 1 kbps
            to          giving a range of values from 1 kbps to 63 kbps in 1 kbps increments.
    0 0 1 1 1 1 1 1

    0 1 0 0 0 0 0 0     The maximum bit rate is 64 kbps + ((the binary coded value in 8 bits - 01000000) * 8 kbps)
            to          giving a range of values from 64 kbps to 568 kbps in 8 kbps increments.
    0 1 1 1 1 1 1 1

    1 0 0 0 0 0 0 0     The maximum bit rate is 576 kbps + ((the binary coded value in 8 bits - 10000000) * 64 kbps)
            to          giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments.
    1 1 1 1 1 1 1 0
    */
    if ( BitRate < 64)
    {
        ret = (UINT8)BitRate;
    }
    else if ( BitRate < 576 )
    {
        ret = (UINT8)(((BitRate - 64)>>3) + 0x40);
    }
    else
    {
        ret = (UINT8)(((BitRate - 576)>>6) + 0x80);
    }

    return ret;
}
UINT8 CFW_GprsBitRate2EQoS_Ext1(UINT32 BitRate)
{
    UINT8 ret = 0;
    /*
    Guaranteed/Maximum bit rate for uplink/downlink (extended):
    
    In UE to network direction and in network to UE direction:
    0 0 0 0 0 0 0 0 Use the value indicated by the maximum bit rate for uplink in octet 4.

                        For all other values: ignore the value indicated by the maximum bit rate for uplink in octet 4
                        and use the following value:
    0 0 0 0 0 0 0 1 The maximum bit rate is 8600 kbps + ((the binary coded value in 8 bits) * 100 kbps),
            to          giving a range of values from 8700 kbps to 16000 kbps in 100 kbps increments.
    0 1 0 0 1 0 1 0

    0 1 0 0 1 0 1 1 The maximum bit rate is 16 Mbps + ((the binary coded value in 8 bits - 01001010) * 1 Mbps),
            to          giving a range of values from 17 Mbps to 128 Mbps in 1 Mbps increments.
    1 0 1 1 1 0 1 0

    1 0 1 1 1 0 1 1 The maximum bit rate is 128 Mbps + ((the binary coded value in 8 bits - 10111010) * 2 Mbps),
            to          giving a range of values from 130 Mbps to 256 Mbps in 2 Mbps increments.
    1 1 1 1 1 0 1 0

    */
    if ( BitRate < 16000 )
    {
        ret = (UINT8)((BitRate - 8600)/100);
    }
    else if ( BitRate < 128000 )
    {
        ret = (UINT8)((BitRate - 16000)/1000 + 0x4A);
    }
    else
    {
        ret = (UINT8)((BitRate - 128000)/2000 + 0xBA);
    }
    
    return ret;
}
UINT8 CFW_GprsBitRate2EQoS_Ext2(UINT32 BitRate)
{
    UINT8 ret = 0;
    /*
    Guaranteed/Maximum bit rate for uplink/downlink (extended-2)
    Bits
    8 7 6 5 4 3 2 1
    In UE to network direction and in network to UE direction:
    0 0 0 0 0 0 0 0 Use the value indicated by the Maximum bit rate for uplink in octet 4 and octet 8.

                        For all other values: Ignore the value indicated by the Maximum bit rate for upink in octet 4 and
                        octet 8 and use the following value:
    0 0 0 0 0 0 0 1 The maximum bit rate is 256 Mbps + ((the binary coded value in 8 bits) * 4 Mbps),
    0 0 1 1 1 1 0 1 giving a range of values from 260 Mbps to 500 Mbps in 4 Mbps increments.

    0 0 1 1 1 1 1 0 The maximum bit rate is 500 Mbps + ((the binary coded value in 8 bits - 00111101) * 10 Mbps),
    1 0 1 0 0 0 0 1 giving a range of values from 510 Mbps to 1500 Mbps in 10 Mbps increments.

    1 0 1 0 0 0 1 0 The maximum bit rate is 1500 Mbps + ((the binary coded value in 8 bits - 10100001) * 100 Mbps),
    1 1 1 1 0 1 1 0 giving a range of values from 1600 Mbps to 10 Gbps in 100 Mbps increments.

    */ 
    if ( BitRate < 500000 )
    {
        ret = (UINT8)((BitRate - 256000)/4000);
    }
    else if ( BitRate < 1500000 )
    {
        ret = (UINT8)((BitRate - 500000)/10000 + 0x3D);
    }
    else
    {
        ret = (UINT8)((BitRate - 1500000)/100000 + 0xA1);
    }
    
    return ret;
}
// CFW Bit Rate to EPS QoS
UINT32 CFW_GprsEQoS2BitRate(UINT8 Value)
{
    UINT32 ret = 0;

    /* Guaranteed/Maximum bit rate for uplink/DownLink:
    
       In UE to network direction and in network to UE direction:
       0 0 0 0 0 0 0 1     The maximum bit rate is binary coded in 8 bits, using a granularity of 1 kbps
               to          giving a range of values from 1 kbps to 63 kbps in 1 kbps increments.
       0 0 1 1 1 1 1 1
       
       0 1 0 0 0 0 0 0     The maximum bit rate is 64 kbps + ((the binary coded value in 8 bits - 01000000) * 8 kbps)
               to          giving a range of values from 64 kbps to 568 kbps in 8 kbps increments.
       0 1 1 1 1 1 1 1
       
       1 0 0 0 0 0 0 0     The maximum bit rate is 576 kbps + ((the binary coded value in 8 bits - 10000000) * 64 kbps)
               to          giving a range of values from 576 kbps to 8640 kbps in 64 kbps increments.
       1 1 1 1 1 1 1 0
    */
    
    if ( (Value & 0xC0) == 0x00 )
    {
        ret = Value;
    }
    else if( (Value & 0xC0) == 0x40 )
    {
        ret = 64 + (Value & 0x3F) * 8;
    }
    else
    {
        ret = 576 + (Value & 0x7F) * 64;

        if( Value == 0xFF) ret = 0;
    }

    return ret;
}
UINT32 CFW_GprsEQoS2BitRate_Ext1(UINT8 Value)
{
    UINT32 ret = 0;
    /*
    Guaranteed/Maximum bit rate for uplink/downlink (extended):
    
    In UE to network direction and in network to UE direction:
    0 0 0 0 0 0 0 0 Use the value indicated by the maximum bit rate for uplink in octet 4.

                        For all other values: ignore the value indicated by the maximum bit rate for uplink in octet 4
                        and use the following value:
    0 0 0 0 0 0 0 1 The maximum bit rate is 8600 kbps + ((the binary coded value in 8 bits) * 100 kbps),
            to          giving a range of values from 8700 kbps to 16000 kbps in 100 kbps increments.
    0 1 0 0 1 0 1 0

    0 1 0 0 1 0 1 1 The maximum bit rate is 16 Mbps + ((the binary coded value in 8 bits - 01001010) * 1 Mbps),
            to          giving a range of values from 17 Mbps to 128 Mbps in 1 Mbps increments.
    1 0 1 1 1 0 1 0

    1 0 1 1 1 0 1 1 The maximum bit rate is 128 Mbps + ((the binary coded value in 8 bits - 10111010) * 2 Mbps),
            to          giving a range of values from 130 Mbps to 256 Mbps in 2 Mbps increments.
    1 1 1 1 1 0 1 0

    */

    if ( Value == 0x00 )
    {
        /*Same as Maximum bit rate for uplink*/;
    }
    else if( Value < 0x4B )
    {
        ret = 8600 + Value * 100;
    }
    else if( Value < 0xBB )
    {
        ret = 16000 + (Value - 0x4A) * 1000;
    }
    else if( Value < 0xFB )
    {
        ret = 128000 + (Value - 0xBA) * 2000;
    }

    return ret;
}
UINT32 CFW_GprsEQoS2BitRate_Ext2(UINT8 Value)
{
    UINT32 ret = 0;
    /*
    Guaranteed/Maximum bit rate for uplink/downlink (extended-2)
    Bits
    8 7 6 5 4 3 2 1
    In UE to network direction and in network to UE direction:
    0 0 0 0 0 0 0 0 Use the value indicated by the Maximum bit rate for uplink in octet 4 and octet 8.

                        For all other values: Ignore the value indicated by the Maximum bit rate for upink in octet 4 and
                        octet 8 and use the following value:
    0 0 0 0 0 0 0 1 The maximum bit rate is 256 Mbps + ((the binary coded value in 8 bits) * 4 Mbps),
    0 0 1 1 1 1 0 1 giving a range of values from 260 Mbps to 500 Mbps in 4 Mbps increments.

    0 0 1 1 1 1 1 0 The maximum bit rate is 500 Mbps + ((the binary coded value in 8 bits - 00111101) * 10 Mbps),
    1 0 1 0 0 0 0 1 giving a range of values from 510 Mbps to 1500 Mbps in 10 Mbps increments.

    1 0 1 0 0 0 1 0 The maximum bit rate is 1500 Mbps + ((the binary coded value in 8 bits - 10100001) * 100 Mbps),
    1 1 1 1 0 1 1 0 giving a range of values from 1600 Mbps to 10 Gbps in 100 Mbps increments.

    */    
    if ( Value == 0x00 )
    {
        /*Same as Maximum bit rate for uplink*/;
    }
    else if( Value < 0x3E )
    {
        ret = 256000 + Value * 4000;
    }
    else if( Value < 0xA2 )
    {
        ret = 500000 + (Value - 0x3D) * 10000;
    }
    else if( Value < 0xF7 )
    {
        ret = 1500000 + (Value - 0xA1) * 100000;
    }

    return ret;
}

// CFW to API covert
void CFW_GprsEQos2Api(CFW_EQOS* pQos, UINT8 ApiQos[13], UINT8 *Length)
{
    SUL_MemSet8(ApiQos, 0x00, 13);
    
    ApiQos[0] = pQos->nQci;

    if (pQos->nDlGbr == 0 && pQos->nDlMbr == 0 && pQos->nUlGbr == 0 && pQos->nUlMbr == 0)
    {
        *Length = 1;
        return;
    }

    *Length = 5;

    /* Maximum bit rate for uplink */
    if ( pQos->nUlMbr <= 8640 )
    {
        ApiQos[1] = CFW_GprsBitRate2EQoS(pQos->nUlMbr);
    }
    else if ( pQos->nUlMbr <= 256000 )
    {
        *Length = 9;
        ApiQos[1] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[5] = CFW_GprsBitRate2EQoS_Ext1(pQos->nUlMbr);
    }
    else if ( pQos->nUlMbr <= 10000000 )
    {
        *Length = 13;
        ApiQos[1] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[5] = CFW_GprsBitRate2EQoS_Ext1(256000);
        ApiQos[9] = CFW_GprsBitRate2EQoS_Ext2(pQos->nUlMbr);
    }

    /* Maximum bit rate for downlink */
    if ( pQos->nDlMbr <= 8640 )
    {
        ApiQos[2] = CFW_GprsBitRate2EQoS(pQos->nDlMbr);
    }
    else if ( pQos->nDlMbr <= 256000 )
    {
        *Length = 9;
        ApiQos[2] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[6] = CFW_GprsBitRate2EQoS_Ext1(pQos->nDlMbr);
    }
    else if ( pQos->nDlMbr <= 10000000 )
    {
        *Length = 13;
        ApiQos[2] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[6] = CFW_GprsBitRate2EQoS_Ext1(256000);
        ApiQos[10] = CFW_GprsBitRate2EQoS_Ext2(pQos->nDlMbr);
    }
    
    /* Guaranteed bit rate for uplink */
    if ( pQos->nUlGbr <= 8640 )
    {
        ApiQos[3] = CFW_GprsBitRate2EQoS(pQos->nUlGbr);
    }
    else if ( pQos->nUlGbr <= 256000 )
    {
        *Length = 9;
        ApiQos[3] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[7] = CFW_GprsBitRate2EQoS_Ext1(pQos->nUlGbr);
    }
    else if ( pQos->nUlGbr <= 10000000 )
    {
        *Length = 13;
        ApiQos[3] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[7] = CFW_GprsBitRate2EQoS_Ext1(256000);
        ApiQos[11] = CFW_GprsBitRate2EQoS_Ext2(pQos->nUlGbr);
    }

    /* Guaranteed bit rate for downlink */
    if ( pQos->nDlGbr <= 8640 )
    {
        ApiQos[4] = CFW_GprsBitRate2EQoS(pQos->nDlGbr);
    }
    else if ( pQos->nDlGbr <= 256000 )
    {
        *Length = 9;
        ApiQos[4] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[8] = CFW_GprsBitRate2EQoS_Ext1(pQos->nDlGbr);
    }
    else if ( pQos->nDlGbr <= 10000000 )
    {
        *Length = 13;
        ApiQos[4] = CFW_GprsBitRate2EQoS(8640);
        ApiQos[8] = CFW_GprsBitRate2EQoS_Ext1(256000);
        ApiQos[12] = CFW_GprsBitRate2EQoS_Ext2(pQos->nDlGbr);
    }        
}

// API to CFW coverts
void CFW_GprsApi2EQoS(CFW_EQOS *pQos, UINT8 ApiQos[13], UINT8 Length)
{
    /* Save QCI */
    pQos->nQci = ApiQos[0];

    if (Length < 5)
        return;

    pQos->nUlMbr = CFW_GprsEQoS2BitRate(ApiQos[1]);
    pQos->nDlMbr = CFW_GprsEQoS2BitRate(ApiQos[2]);
    pQos->nUlGbr = CFW_GprsEQoS2BitRate(ApiQos[3]);
    pQos->nDlGbr = CFW_GprsEQoS2BitRate(ApiQos[4]);

    /* Extended Value not existed */
    if (Length < 9)
        return;
    
    pQos->nUlMbr = CFW_GprsEQoS2BitRate_Ext1(ApiQos[5]);
    pQos->nDlMbr = CFW_GprsEQoS2BitRate_Ext1(ApiQos[6]);
    pQos->nUlGbr = CFW_GprsEQoS2BitRate_Ext1(ApiQos[7]);
    pQos->nDlGbr = CFW_GprsEQoS2BitRate_Ext1(ApiQos[8]);

    /* Extended 2 Value not existed */
    if (Length < 13)
        return;

    pQos->nUlMbr = CFW_GprsEQoS2BitRate_Ext2(ApiQos[9]);
    pQos->nDlMbr = CFW_GprsEQoS2BitRate_Ext2(ApiQos[10]);
    pQos->nUlGbr = CFW_GprsEQoS2BitRate_Ext2(ApiQos[11]);
    pQos->nDlGbr = CFW_GprsEQoS2BitRate_Ext2(ApiQos[12]);

}

// IP Address Compare - Packet Filter
UINT8 CFW_GprsAddrCompare(UINT8* Addr1, UINT8* Addr2, UINT8* Mask, UINT8 MaskBits)
{
    UINT8 Index   = 0;
    UINT8 Bytes   = MaskBits/8;
    UINT8 Bits    = MaskBits%8;
    UINT8 nMask   = 0xFF;

    for ( Index = 0; Index < Bytes; Index ++ )
    {
        if ( (Addr1[Index] & Mask[Index]) != (Addr2[Index] & Mask[Index]) )
        {
            return FALSE;
        }
    }

    if  ( Bits != 0 )
    {
        nMask = (nMask<<(8-Bits));
        
        if ( (Addr1[Bytes] & Mask[Bytes] & nMask) != (Addr2[Bytes] & Mask[Bytes] & nMask) )
        {
            return FALSE;
        }
    }

    return TRUE;
}

UINT8 CFW_GprsTftHasEbi(CFW_TFT_PF *tftPf,UINT8 Ebi)
{
    UINT8 found = FALSE;
    
    while((tftPf != NULL)
        &&(FALSE == found))
    {
        if(Ebi == tftPf->uEBI)
            found = TRUE;
        tftPf = tftPf->pNext;
    }
    
    return found;
}

UINT8 CFW_GprsTftFindSameId(CFW_TFT_PF *existPf,UINT8 *flagArr,UINT8 *IdArr,UINT8 Ebi)
{
    UINT8 idCnt = 0;

    while(existPf != NULL)
    {
        if(Ebi == existPf->uEBI)
        {
            if(1 == flagArr[existPf->uPF_ID])
                IdArr[idCnt++] = existPf->uPF_ID;
            
            flagArr[existPf->uPF_ID] = 1;
        }
        existPf = existPf->pNext;
    }

    return idCnt;
}

CFW_TFT_PF * CFW_GprsTftDeleteSameId(CFW_TFT_PF **tftPf,UINT8 Id,UINT8 Ebi)
{
    CFW_TFT_PF *current = NULL;
    CFW_TFT_PF *prev = NULL;

    if(NULL == *tftPf)
        return NULL;

    current = *tftPf;
    prev = *tftPf;

    //delete head
    if((current->uPF_ID == Id)
        &&(current->uEBI == Ebi))
    {
        prev = current->pNext;
        CFW_GprsTftPfFree(current);
        *tftPf = prev;
        return *tftPf;
    }

    current = current->pNext;
    
    while((current != NULL)
        &&((current->uPF_ID != Id)||(current->uEBI != Ebi)))
    {
        prev = current;
        current = current->pNext;
    }

    if(NULL != current)
    {
        prev->pNext = current->pNext;
        CFW_GprsTftPfFree(current);
    }

    return *tftPf;
    
}

CFW_TFT_PF * CFW_GprsTftDeleteSameEbi(CFW_TFT_PF **tftPf,UINT8 Ebi)
{
    CFW_TFT_PF *current = NULL;
    CFW_TFT_PF *prev = NULL;
    CFW_TFT_PF *retHead = NULL;

    if(NULL == *tftPf)
        return NULL;

    current = *tftPf;
    prev = *tftPf;

    while((current != NULL)
        &&(current->uEBI == Ebi))
    {
        prev = current;
        current = current->pNext;
        CFW_GprsTftPfFree(prev);
    }

    if(NULL != current)
    {
        CFW_TFT_PF *pFree;
        retHead = current;
        prev = current;

        current = current->pNext;
        while(current != NULL)      
        {
            if(current->uEBI == Ebi)
            {
                pFree = current;
                prev->pNext = current->pNext;
                current = prev->pNext;  
                
                CFW_GprsTftPfFree(pFree); 
            }
            else
            {
                prev = current;
                current = current->pNext;               
            }
        }

        *tftPf = retHead;
    }

    return retHead;
    
}


UINT8 CFW_GprsTftHasUpDir(CFW_TFT_PF *pfHead)
{
    CFW_TFT_PF *pf = pfHead;

    while(pf != NULL)
    {
        if(pf->uPF_Dir & 0x02)
            break;
        pf = pf->pNext;
    }

    if(pf != NULL)
        return TRUE;

    return FALSE;
}

UINT8 CFW_GprsTftPfHasSameID(CFW_TFT_PF *pfHead,UINT8 *flagArr)
{
    CFW_TFT_PF *pf = pfHead;

    while(pf != NULL)
    {
        if(1 == flagArr[pf->uPF_ID])
            break;
        
        flagArr[pf->uPF_ID] = 1;

        pf = pf->pNext;
    }

    if(pf != NULL)
        return TRUE;

    return FALSE;
}

void CFW_GprsStorePfSyntSamePrioEbi(UINT8 *EbiDefault,UINT8 *EbiNum,UINT8 Ebi)
{
    UINT8 num = 0;
    while(num < *EbiNum)
    {
        if(EbiDefault[*EbiNum] != Ebi)
            num++;
        else
            break;
    }
    
    if((num < *EbiNum)
        ||(0 == *EbiNum))
    {
        EbiDefault[*EbiNum] = Ebi;
        *EbiNum = *EbiNum + 1;
    }
}

UINT8 CFW_GprsNewTftHasSamePriority(CFW_TFT_PF *newTftPf)
{
    UINT8 flagArr[256] = {0};
    CFW_TFT_PF *pf = newTftPf;

    while(pf != NULL)
    {
        if(1 == flagArr[pf->uPF_Pri])
            break;
        
        flagArr[pf->uPF_Pri] = 1;

        pf = pf->pNext;
    }

    if(pf != NULL)
        return TRUE;

    return FALSE;   
}


UINT8 CFW_GprsCheckPfHeadMismatch(UINT8 pos,CFW_TFT* pTft,PF_HEAD *pfHead)
{
    if((pos + 2) >= pTft->uLength)
        return E_CFW_TFT_ERR_OP_SYNT;

    pfHead = (PF_HEAD *)(pTft->uTFT + pos);
        
    if(((pos + 2) + pfHead->length) >= pTft->uLength)
            return E_CFW_TFT_ERR_OP_SYNT;
    
    return E_CFW_TFT_ERR_NO;
}

UINT8 CFW_PfComponentIsInValidType(UINT8 pfComponentType)
{
    UINT8 valid = FALSE;
    
    switch(pfComponentType)
    {
        case IPV4_REMOTE_ADDRESS_TYPE :
        case IPV4_LOCAL_ADDRESS_TYPE :
        case IPV6_REMOTE_ADDRESS_TYPE :
        case IPV6_REMOTE_ADDRESS_PREFIX_LENGTH_TYPE :
        case IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE :
        case PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE :
        case SINGLE_LOCAL_PORT_TYPE :
        case LOCAL_PORT_RANGE_TYPE :
        case SINGLE_REMOTE_PORT_TYPE :
        case REMOTE_PORT_RANGE_TYPE :
        case SECURITY_PARAMETER_INDEX_TYPE :
        case TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE :
        case FLOW_LABEL_TYPE :
            valid = TRUE;
            break;
        default:
            break;
    }

    return valid;
}

UINT8 CFW_GprsGetPfTypeLength(UINT8 type)
{

/*  IPv4 remote address type:               8 octet (a four octet IPv4 address field and a four octet IPv4 address mask field)
    IPv4 local address type:                8 octet (a four octet IPv4 address field and a four octet IPv4 address mask field)
    IPv6 remote address type:               32 octet (a sixteen octet IPv6 address field and a sixteen octet IPv6 address mask field)
    IPv6 remote address/prefix length type: 17 octet(a sixteen octet IPv6 address field and one octet prefix length field)
    IPv6 local address/prefix length type:  17 octet(a sixteen octet IPv6 address field and one octet prefix length field)
    Protocol identifier/Next header type:   one octet
    Single local port type:                 two octet
    Local port range type:                  4 octet (a two octet port range low limit field and a two octet port range high limit field)
    Single remote port type:                two octet
    Remote port range type:                 4 octet (a two octet port range low limit field and a two octet port range high limit field)
    Security parameter index type:          four octet
    Type of service/Traffic class type:     2 octet (a one octet Type-of-Service/Traffic Class field and a one octet Type-of-Service/Traffic Class mask field)
    Flow label type:                        three octet
*/



    UINT8 length = 0;
    switch(type)
    {
        case IPV4_REMOTE_ADDRESS_TYPE :
        case IPV4_LOCAL_ADDRESS_TYPE :
            length = 8;
            break;
        case IPV6_REMOTE_ADDRESS_TYPE :
            length = 32;
            break;  
        case IPV6_REMOTE_ADDRESS_PREFIX_LENGTH_TYPE :
        case IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE :            
            length = 17;
            break;      
        case PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE :         
            length = 1;
            break;  
        case SINGLE_LOCAL_PORT_TYPE :
        case SINGLE_REMOTE_PORT_TYPE:   
        case TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE:
            length = 2;
            break;  
        case LOCAL_PORT_RANGE_TYPE :    
        case REMOTE_PORT_RANGE_TYPE :
        case SECURITY_PARAMETER_INDEX_TYPE:
            length = 4;
            break;
        case FLOW_LABEL_TYPE:
            length = 3;
            break;
        default:
            //s_TftPfTypeError = TRUE;
            break;          
    }

    return length;
}

UINT8 CFW_GprsPfComponentConfictCheck(CFW_CPNT_CFLT *conflictFlag,UINT8 type)
{
    UINT8 errorCode = FALSE;
//  #define PF_COMPOENENT_ERROR_TYPE (0)

    switch(type)
    {
        case IPV4_REMOTE_ADDRESS_TYPE:
        case IPV6_REMOTE_ADDRESS_TYPE:
        {
            if(0 == conflictFlag->remoteAddr)
                conflictFlag->remoteAddr = type;
            else if(conflictFlag->remoteAddr != type)
                errorCode = TRUE;
            break;
        }

        case SINGLE_LOCAL_PORT_TYPE:
        case LOCAL_PORT_RANGE_TYPE:
        {
            if(0 == conflictFlag->locPortType)
                conflictFlag->locPortType = type;
            else if(conflictFlag->locPortType != type)
                errorCode = TRUE;
            break;
        }

        case SINGLE_REMOTE_PORT_TYPE:
        case REMOTE_PORT_RANGE_TYPE:
        {
            if(0 == conflictFlag->remotePortType)
                conflictFlag->remotePortType = type;
            else if(conflictFlag->remotePortType != type)
                errorCode = TRUE;
            break;          
        }
        default:
            break;
    }

    return errorCode;
}

CFW_TFT_PF * CFW_GprsTftFindAndDelSameID(CFW_TFT_PF *newPfHead,CFW_TFT_PF *existPf,CFW_TFT_PF **tftPf,UINT8 Ebi)
{
    UINT8 flagArr[16] = {0};
    UINT8 sameIdCnt = 0;
    UINT8 sameIdArr[16] = {0};
    CFW_TFT_PF *retTftPf = NULL;
    
    sameIdCnt = CFW_GprsTftPfHasSameID(newPfHead,flagArr);

    sameIdCnt = CFW_GprsTftFindSameId(existPf,flagArr,sameIdArr,Ebi);

    retTftPf = *tftPf;
    while(sameIdCnt != 0)
    {
        retTftPf = CFW_GprsTftDeleteSameId(tftPf,sameIdArr[sameIdCnt - 1],Ebi);
        *tftPf = retTftPf;
        sameIdCnt--;
    }
    return retTftPf;
}

UINT8 CFW_GprsTftPfHasSameIDInAct(CFW_TFT_PF *newPfHead,CFW_TFT_PF *existPf,UINT8 Ebi)
{
    UINT8 flagArr[16] = {0};
    UINT8 ret = FALSE;
    CFW_TFT_PF *pf = existPf;    
    
    CFW_GprsTftPfHasSameID(newPfHead,flagArr);

    while(pf != NULL)
    {
        if(pf->uEBI == Ebi)
        {
            if(1 == flagArr[pf->uPF_ID])
            {
                ret = TRUE;
                break;
            }
            
            flagArr[pf->uPF_ID] = 1;
        }

        pf = pf->pNext;
    }

    return ret;
}

UINT8 CFW_GprsTftFindOldSamePrio(CFW_TFT_PF *newPfHead,
                                                 CFW_TFT_PF *existPf,
                                                 CFW_TFT_PARSE *pTftParse)
{
    UINT8 flagArr[256] = {0};
    CFW_TFT_PF *pf = newPfHead;
    UINT8 errorCode = E_CFW_TFT_ERR_NO;
    UINT8 LinkEbi = pTftParse->LinkEbi;

    while(pf != NULL)
    {   
        flagArr[pf->uPF_Pri] = 1;
        pf = pf->pNext;
    }

    pf = existPf;

    while(pf != NULL)
    {
        if(1 == flagArr[pf->uPF_Pri] && pf->uEBI != pTftParse->Ebi)
        {
            if(pf->uEBI != LinkEbi)
            {
                CFW_GprsStorePfSyntSamePrioEbi(pTftParse->EbiDeact, &pTftParse->EbiDeactNum, pf->uEBI);
                errorCode = E_CFW_TFT_ERR_SAME_PRIO_NOT_DEFAULT;
            }
            else
            {
                errorCode = E_CFW_TFT_ERR_SAME_PRIO_DEFAULT;
                break;
            }       
        }
        else
        {
            flagArr[pf->uPF_Pri] = 1;
        }
        pf = pf->pNext;
    }   

    return errorCode;   
}

UINT8 CFW_GprsTftMsgParseWithComponent(CFW_TFT* pTft, UINT8 Ebi, CFW_TFT_PF **retTftPf)
{
    TFT_HEAD_BIT *headBit;
    CFW_TFT_PF *newTftPf = NULL;
    CFW_TFT_PF *prevTftPf = NULL;   
    CFW_TFT_PF *newTftPfHead = NULL;
    PF_HEAD *pfHead = NULL;
    UINT8 errorCode = E_CFW_TFT_ERR_NO;
    UINT8 pfCnt = 0;
    UINT8 *tft = NULL;
    UINT8 pos = 0;
    
    headBit = (TFT_HEAD_BIT *)pTft->uTFT;

    newTftPf = CFW_GprsTftPfMalloc();
    newTftPf->Cpnt = NULL; 
    newTftPfHead = newTftPf;
    prevTftPf = newTftPf;

    tft = pTft->uTFT;
    pos = 1;
    while(pfCnt < headBit->numOfPf)
    {
        /*
        b)Syntactical errors in TFT operations:
        2)in active or 6)in modify
          When there are other types of syntactical errors in the coding of the TFT IE,
          such as a mismatch between the number of packet filters subfield, 
          and the number of packet filters in the packet filter list

          the UE shall reject the modification request with
          ESM cause #42 "syntactical error in the TFT operation".       
        */
        if(E_CFW_TFT_ERR_NO != CFW_GprsCheckPfHeadMismatch(pos,pTft,pfHead))
        {
            errorCode = E_CFW_TFT_ERR_OP_SYNT;
            break;
        }

        pfHead = (PF_HEAD *)(pTft->uTFT + pos);
        
        errorCode = CFW_GprsTftParsePf(pos,tft,pfHead,newTftPf);
        
        if(errorCode != E_CFW_TFT_ERR_NO)
            break;

        newTftPf->uEBI = Ebi;
        //set to next pf position
        pos += 3;
        pos += pfHead->length;
        
        pfCnt++;

        if(pfCnt < headBit->numOfPf)
        {
            newTftPf = CFW_GprsTftPfMalloc();
            newTftPf->Cpnt = NULL;         
            prevTftPf->pNext = newTftPf;            
            prevTftPf = prevTftPf->pNext;
        }
    }   

    *retTftPf = newTftPfHead;

    return errorCode;
}

UINT8 CFW_GprsTftDelPfMsgParse(CFW_TFT* pTft, UINT8 Ebi, CFW_TFT_PF **retTftPf)
{
    TFT_HEAD_BIT *headBit;
    CFW_TFT_PF *newTftPf = NULL;
    CFW_TFT_PF *prevTftPf = NULL;   
    CFW_TFT_PF *newTftPfHead = NULL;
    UINT8 pfCnt = 0;
    UINT8 *tft = NULL;
    
    headBit = (TFT_HEAD_BIT *)pTft->uTFT;

    /*
    b)  Syntactical errors in packet filters:
    6)  When there are other types of syntactical errors in the coding of the TFT IE,
        such as a mismatch between the number of packet filters subfield, 
        and the number of packet filters in the packet filter list

        In cases 1 and 3 the UE shall reject the activation request with ESM cause 
        #45 "syntactical errors in packet filter(s)".
    */  
    if((headBit->numOfPf + 1) > pTft->uLength)  
        return E_CFW_TFT_ERR_OP_SYNT;
    /*
    for jira R8910-370 
    b)  yntactical errors in TFT operations:    
    5)TFT operation = "Delete packet filters from existing TFT" with
    a packet filter list also including packet filters in addition to
    the packet filter identifiers.
    */    
    if((headBit->Ebit == 0)
        &&((headBit->numOfPf + 1) < pTft->uLength))
    {        
        return E_CFW_TFT_ERR_OP_SYNT;    
    }

    newTftPf = CFW_GprsTftPfMalloc();
    newTftPf->Cpnt = NULL;
    newTftPfHead = newTftPf;
    prevTftPf = newTftPf;

    tft = pTft->uTFT;

    while(pfCnt < headBit->numOfPf)
    {
        newTftPf->uPF_ID = (tft[pfCnt+1] & 0x0F);
        
        pfCnt++;

        if(pfCnt < headBit->numOfPf)
        {
            newTftPf = CFW_GprsTftPfMalloc();
            newTftPf->Cpnt = NULL;         
            prevTftPf->pNext = newTftPf;            
            prevTftPf = prevTftPf->pNext;
        }
    }   

    *retTftPf = newTftPfHead;

    return E_CFW_TFT_ERR_NO;
}

UINT8 CFW_GprsTftParsePf(UINT8 pfListBegin,UINT8* tft,PF_HEAD *pfHead,CFW_TFT_PF *newTftPf)
{
    UINT8 pos = 0;
    UINT8 length = 0;
    UINT8 errorCode = E_CFW_TFT_ERR_NO;
    PF_HEAD_BIT *headBit = NULL;
    CFW_PF_CPNT *newComponent = NULL;
    CFW_PF_CPNT *prev = NULL;
    CFW_CPNT_CFLT conflictFlag;
    
    headBit = (PF_HEAD_BIT *)&(pfHead->idAndDir);
    newTftPf->uPF_Dir = headBit->dir;
    newTftPf->uPF_ID = headBit->id;
    
    newTftPf->uPF_Pri = pfHead->prior;

    //TBD
    if(pfHead->length == 0)
        return E_CFW_TFT_ERR_PF_SYNT;
    
    newComponent = CFW_GprsPfCpntMalloc();
    newTftPf->Cpnt = newComponent;
    prev = newComponent;

    pos = pfListBegin + 3;
    length = 0;

    SUL_MemSet8((UINT8 *)(&conflictFlag),0x00,sizeof(CFW_CPNT_CFLT));

    while(length < pfHead->length)
    {
        /*
        d)Syntactical errors in packet filters
        3)When there are other types of syntactical errors in the coding of packet filters, 
          such as the use of a reserved value for a packet filter component identifier

          UE shall reject the modification request with
          ESM cause #45 "syntactical errors in packet filter(s)"
        */
        newComponent->uCpntType = tft[pos];
        if(FALSE == CFW_PfComponentIsInValidType(newComponent->uCpntType))
        {
            errorCode = E_CFW_TFT_ERR_PF_SYNT;
            break;
        }
        /*
        c)Semantic errors in packet filters
        1)When a packet filter consists of conflicting packet filter components 
          which would render the packet filter ineffective, 
          i.e. no IP packet will ever fit this packet filter. 
          How the UE determines a semantic error in a packet filter is
          outside the scope of the present document

          The UE shall reject the modification request with
          ESM cause #44 "semantic errors in packet filter(s)"
        */
        if(CFW_GprsPfComponentConfictCheck(&conflictFlag,newComponent->uCpntType))
        {
            errorCode = E_CFW_TFT_ERR_PF_SEMA;
            break;
        }           

        newComponent->uCpntLen = CFW_GprsGetPfTypeLength(newComponent->uCpntType);

        /*
        b)Syntactical errors in TFT operations:
        2)in active or 
        6)in modify     
          When there are other types of syntactical errors in the coding of the TFT IE,
          such as a mismatch between the number of packet filters subfield, 
          and the number of packet filters in the packet filter list

          the UE shall reject the modification request with
          ESM cause #42 "syntactical error in the TFT operation".       
        */
        if(((length + 1) + newComponent->uCpntLen) > pfHead->length)
        {
            errorCode = E_CFW_TFT_ERR_OP_SYNT;
            break;
        }

        SUL_MemCopy8(newComponent->uCpntCont,tft+pos+1,newComponent->uCpntLen);

        length += 1;
        length += newComponent->uCpntLen;

        pos += (newComponent->uCpntLen + 1);

        if(length < pfHead->length)
        {
            //pos += length;

            newComponent = CFW_GprsPfCpntMalloc();
            prev->pNext = newComponent;
            prev = prev->pNext;
        }
    }
    
    /*
    for jira R8910-377
    TS 23.060 15.3.2
    */
    if((E_CFW_TFT_ERR_NO == errorCode)
        &&(FALSE == CFW_GprsPfAttriIsValid(newTftPf->Cpnt)))
     errorCode = E_CFW_TFT_ERR_PF_SEMA;

    return errorCode;
}
/*
Remote Address and Subnet Mask
Protocol Number(IPv4)/Next Header(IPv6)
Local Port Range
Remote Port Range
TOS(IPv4)/Traffic Class(IPv6) and MasK
at least one should be specified.
*/
UINT8 CFW_GprsPfAttriIsValid(CFW_PF_CPNT *cpnt)
{
    UINT8 coexistBitMap = 0;
    CFW_PF_CPNT *pfCpnt = cpnt;
    
    while(pfCpnt != NULL)
    {
        switch(pfCpnt->uCpntType)
        {
            case IPV4_REMOTE_ADDRESS_TYPE :
            case IPV6_REMOTE_ADDRESS_TYPE :
            case IPV6_REMOTE_ADDRESS_PREFIX_LENGTH_TYPE :
                coexistBitMap |= 1<<7;
                break;
            case PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE :
                coexistBitMap |= 1<<6;
                break;
            case IPV4_LOCAL_ADDRESS_TYPE :
            case IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE :
                coexistBitMap |= 1<<5;
                break;
            case LOCAL_PORT_RANGE_TYPE :
                coexistBitMap |= 1<<4;
                break;
            case REMOTE_PORT_RANGE_TYPE :
                coexistBitMap |= 1<<3;
                break;  
            case SECURITY_PARAMETER_INDEX_TYPE :
                coexistBitMap |= 1<<2;
                break;              
            case TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE :
                coexistBitMap |= 1<<1;
                break;
            case FLOW_LABEL_TYPE :
                coexistBitMap |= 1<<0;
                break;
            default:
                break;

        }
        pfCpnt = pfCpnt->pNext;
    }

    //exist flow label,check 0101,1100
    if(((coexistBitMap & 0x01) != 0x0)
        &&(((coexistBitMap & 0x5C) != 0x0)))
        return FALSE;
    
    //exist IPSec SPI,check 0001,1000
    if(((coexistBitMap & 0x04) != 0x0)
        &&(((coexistBitMap & 0x18) != 0x0)))
        return FALSE;
    
    //0xDA :1101,1010
    if((coexistBitMap & 0xDA) != 0x0)
        return TRUE;
    else
        return FALSE;
}

CFW_TFT_PF *CFW_GprsTftInsertPf(CFW_TFT_PF **head,CFW_TFT_PF *newTftPf)
{
    CFW_TFT_PF * currentPf = NULL;
    CFW_TFT_PF *prevPf = NULL;

    if(NULL == *head)
    {
        *head = newTftPf;   
        newTftPf->pNext = NULL; 
    }
    else
    {
        currentPf = *head;
        
        //insert head
        if(currentPf->uPF_Pri >= newTftPf->uPF_Pri)
        {
            newTftPf->pNext = *head;
            *head = newTftPf;
        }
        else
        {
            prevPf = *head;     
            currentPf = prevPf->pNext;
            while(currentPf != NULL)
            {
                if(currentPf->uPF_Pri < newTftPf->uPF_Pri)
                {
                    prevPf = currentPf;
                    currentPf = prevPf->pNext;
                }
                else
                {
                    prevPf->pNext = newTftPf;
                    newTftPf->pNext = currentPf;
                    break;
                }
            }

            //insert to tail
            if(NULL == currentPf)
            {
                prevPf->pNext = newTftPf;
                newTftPf->pNext = NULL;
            }
        }
    }

    return *head;
}

/* TFT Packet Filter Clone */
CFW_TFT_PF *CFW_GprsTftClone(CFW_TFT_PF *orignal)
{
    CFW_TFT_PF *origTftClone = NULL;
    CFW_TFT_PF *newClone     = NULL;
    CFW_TFT_PF *prevClone    = NULL;

    CFW_PF_CPNT* NewCpnt  = NULL;
    CFW_PF_CPNT* HeadCpnt = NULL;
    CFW_PF_CPNT* PrevCpnt = NULL;

    while(orignal != NULL)
    {
        newClone = CFW_GprsTftPfMalloc();
        SUL_MemCopy8(newClone,orignal,sizeof(CFW_TFT_PF));        
        newClone->Cpnt = NULL;     

        /* Clone CPNT */
        HeadCpnt = orignal->Cpnt;
        
        while(HeadCpnt != NULL)
        {
            NewCpnt = CFW_GprsPfCpntMalloc();
            SUL_MemCopy8(NewCpnt, HeadCpnt, sizeof(CFW_PF_CPNT));

            if(newClone->Cpnt == NULL)
            {
                newClone->Cpnt = NewCpnt;
                PrevCpnt = NewCpnt;
            }
            else
            {
                PrevCpnt->pNext = NewCpnt;
                PrevCpnt = NewCpnt;
            }

            HeadCpnt = HeadCpnt->pNext;
        }

        if( origTftClone == NULL )
        {
            origTftClone = newClone;
            prevClone    = newClone;
        }
        else
        {
            prevClone->pNext = newClone;
            prevClone = newClone;
        }
        
        orignal = orignal->pNext;
    }

    return origTftClone;
}

// TFT Packet Filter Free all
void CFW_GprsTftFreeAll(CFW_TFT_PF **pfHead)
{
    CFW_TFT_PF *next = NULL;
    while(*pfHead != NULL)
    {
        next = (*pfHead)->pNext;
        CFW_GprsTftPfFree(*pfHead);
        *pfHead = next;
    }
    *pfHead = NULL;
}


// Malloc TFT Packet Filter
VOID* CFW_GprsTftPfMalloc(VOID)
{
    CFW_TFT_PF* pCpnt = CSW_GPRS_MALLOC(sizeof( CFW_TFT_PF ));

    pCpnt->pNext = NULL;
    
    return pCpnt;
}

// Free TFT Packet Filter
void CFW_GprsTftPfFree(CFW_TFT_PF* pTftPf)
{
    CFW_PF_CPNT* pCpnt = NULL;

    while ( pTftPf->Cpnt != NULL )
    {
        pCpnt = pTftPf->Cpnt;
        pTftPf->Cpnt = pTftPf->Cpnt->pNext;

        CSW_GPRS_FREE(pCpnt);
    }

    CSW_GPRS_FREE(pTftPf);
}

// Malloc TFT Packet Filter Component
VOID* CFW_GprsPfCpntMalloc(VOID)
{
    CFW_PF_CPNT* pCpnt = CSW_GPRS_MALLOC(sizeof( CFW_PF_CPNT ));

    pCpnt->pNext = NULL;
    
    return pCpnt;
}

UINT8 CFW_GprsGetIPv6Prefix(UINT8* Mask)
{
    UINT8 Prefix = 0;
    UINT8 Index = 0;
    UINT8 Pos = 0;

    for(Index = 0; Index < 16; Index ++)
    {
        if( Mask[Index] != 0xFF )
        {
            Prefix = Index * 8;

            for(Pos = 0; Pos < 8; Pos ++)
            {
                if ( (Mask[Index] >> (7-Pos)) & 0x01)
                    Prefix ++;
                else
                    break;
            }
                  
        }
    }

    return Prefix;
}
#endif
//[in]
//[out]
//[out]
void CFW_GprsPdpAddr2Api(CFW_GPRS_PDPCONT_INFO *pPdpCXT, UINT8 *pPdpAddLen, UINT8 *pPdpAdd)
{
    *pPdpAddLen = pPdpCXT->nPdpAddrSize + 2;
    *pPdpAdd = 0x01;
    *(pPdpAdd + 1) = 0x21;
    SUL_MemCopy8(pPdpAdd + 2, pPdpCXT->pPdpAddr, pPdpCXT->nPdpAddrSize);
}

void CFW_GprsPdpApi2Addr(CFW_GPRS_PDPCONT_INFO *pPdpCXT, UINT8 pPdpAddLen, UINT8 *pPdpAdd)
{
    pPdpCXT->nPdpAddrSize = pPdpAddLen - 2;
    SUL_MemCopy8(pPdpCXT->pPdpAddr, pPdpAdd + 2, pPdpCXT->nPdpAddrSize);
}


UINT8 CFW_GprsGetFreeCid(CFW_SIM_ID nSimID)
{
    HAO    hSm;
    GPRS_SM_INFO *GetProc;
    UINT8 nAllIndex = 0;
    UINT8 i = 0;
    UINT8 j = 0;

    CFW_SetServiceId(CFW_GPRS_SRV_ID);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, 0) ) != HNULL) && (i < 7))
    {
        GetProc = (GPRS_SM_INFO *)CFW_GetAoUserData(hSm);
        if (GetProc )
        {
            if((GetProc->Cid2Index.nCid != 0xff) && (GetProc->Cid2Index.nCid >= 1) && (GetProc->Cid2Index.nCid <= 7))
            {
                nAllIndex |= ( 1 << (GetProc->Cid2Index.nCid - 1));
            }
            i++;
        }
    }

    while(j < 7)
    {
        if(nAllIndex & (1 << j))
            j++;
        else
            return (j + 1);
    }

    return 0xff;//???????????
}
#endif
#endif

