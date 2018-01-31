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




#ifndef BLUETOOTH_TCSTERMINAL_API_DECLARED
#define BLUETOOTH_TCSTERMINAL_API_DECLARED


APIDECL1 int APIDECL2 TCS_TLRegister(u_int8 flag, t_TCS_callbacks *callbacks, t_bdaddr *GWBDAddress, u_int8 connectToGW, void (*callback)(int status));
APIDECL1 int APIDECL2 TCS_TLConnectToTL(t_bdaddr TLbdAddress, void (*callbackFunc)(t_bdaddr address, int status));
APIDECL1 int APIDECL2 TCS_TLConnectToGW(t_bdaddr GWbdAddress, void (*callbackFunc)(t_bdaddr address, int status));
APIDECL1 int APIDECL2 TCS_TLIntercomSetup(u_int16 *callHandle, t_bdaddr TLbdAddress, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_TLCordlessSetup(u_int16 *callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_TLRegisterRecall(u_int16 callHandle);
APIDECL1 int APIDECL2 TCS_TLConnect(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_TLReleaseComplete(u_int16 callHandle, t_TCS_callParams *callParams);

#define TCS_TLSetupAcknowledge      TCS_SetupAcknowledge
#define TCS_TLAlerting              TCS_Alerting
#define TCS_TLInformation           TCS_Information
#define TCS_TLCallProceeding        TCS_CallProceeding
#define TCS_TLConnectAcknowledge    TCS_ConnectAcknowledge
#define TCS_TLDisconnect            TCS_Disconnect
#define TCS_TLRelease               TCS_Release
#define TCS_TLStartDTMF             TCS_StartDTMF
#define TCS_TLStartDTMFAcknowledge  TCS_StartDTMFAcknowledge
#define TCS_TLStartDTMFReject       TCS_StartDTMFReject
#define TCS_TLStopDTMF              TCS_StopDTMF
#define TCS_TLStopDTMFAcknowledge   TCS_StopDTMFAcknowledge
#define TCS_TLProgress              TCS_Progress

APIDECL1 int APIDECL2 TCS_TLDisconnectFromGWorTL(void);
#define TCS_TLDisconnectFromTL TCS_TLDisconnectFromGWorTL
#define TCS_TLDisconnectFromGW TCS_TLDisconnectFromGWorTL




/*
APIDECL1 t_api APIDECL2 TCS_TLDisconnectFromTL();
APIDECL1 t_api APIDECL2 TCS_TLDisconnectFromGW();
*/


#endif /* BLUETOOTH_TCSTERMINAL_API_DECLARED */

