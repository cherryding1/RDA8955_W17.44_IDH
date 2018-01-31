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

#ifndef AMP_TEST_H
#define AMP_TEST_H

#define MAX_PHYLINK_NUM 5
#define MAX_CONTROLLER  MAX_PHYLINK_NUM

typedef struct st_ampTestRemoteInfo
{
    u_int8      controllerNum;
    struct
    {
        u_int8  controlID;
        u_int8  controlType;
        u_int8  controlStatus;
    } controlInfo[MAX_CONTROLLER];

} ampTestRemoteInfo;

int AMPTestGetRemotePeerAddress(t_bdaddr *pAddr);

int AMPTestGetRemoteControlInfo(t_bdaddr address, ampTestRemoteInfo *pInfo);

int AMPTestWriteRemoteAMPInfo(t_bdaddr address, u_int8 controlID);

#endif