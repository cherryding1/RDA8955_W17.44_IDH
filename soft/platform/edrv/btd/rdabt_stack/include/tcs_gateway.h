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




#ifndef BLUETOOTH_TCSGATEWAY_API_DECLARED
#define BLUETOOTH_TCSGATEWAY_API_DECLARED


APIDECL1 int APIDECL2 TCS_GWRegister(u_int8 flag, t_TCS_callbacks *callbacks);
APIDECL1 int APIDECL2 TCS_GWCordlessSetup(u_int16 *callHandle, t_bdaddr *TLbdAddress, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWIntercomSetup(u_int16 *callHandle, t_bdaddr TLbdAddress, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWConnect(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWReleaseComplete(u_int16 callHandle, t_TCS_callParams *callParams);

#define TCS_GWSetupAcknowledge      TCS_SetupAcknowledge
#define TCS_GWAlerting              TCS_Alerting
#define TCS_GWInformation           TCS_Information
#define TCS_GWCallProceeding        TCS_CallProceeding
#define TCS_GWConnectAcknowledge    TCS_ConnectAcknowledge
#define TCS_GWDisconnect            TCS_Disconnect
#define TCS_GWRelease               TCS_Release
#define TCS_GWStartDTMF             TCS_StartDTMF
#define TCS_GWStartDTMFAcknowledge  TCS_StartDTMFAcknowledge
#define TCS_GWStartDTMFReject       TCS_StartDTMFReject
#define TCS_GWStopDTMF              TCS_StopDTMF
#define TCS_GWStopDTMFAcknowledge   TCS_StopDTMFAcknowledge
#define TCS_GWProgress              TCS_Progress
APIDECL1 int APIDECL2 TCS_GWDisconnectFromTL(t_bdaddr TLAddress);

#endif /* BLUETOOTH_TCSTERMINAL_API_DECLARED */

