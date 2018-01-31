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


#ifndef _BOT_H_
#define _BOT_H_
#include "cs_types.h"

#include "usbmscp.h"



//*****************************************************************************
// D E F I N E S
//*****************************************************************************

#define MAX_BOT_STALL_COUNT 2

#define MAX_CBWCB_SIZE 16

// Command Block Wrapper Signature 'USBC'
#define CBW_SIGNATURE               0x43425355
#define CBW_FLAGS_DATA_IN           0x80
#define CBW_FLAGS_DATA_OUT          0x00

//
// Command Status Wrapper Signature 'USBS'
//
#define CSW_SIGNATURE               0x53425355
#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02


//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************

typedef struct _CSW_
{
    UINT32   dCSWSignature;
    UINT32   dCSWTag;
    UINT32   dCSWDataResidue;
    UINT8   bCSWStatus;
} __attribute__((packed))CSW, *PCSW;


//
// Command Block Wrapper
//
typedef struct _CBW
{
    UINT32   dCBWSignature;
    UINT32   dCBWTag;
    UINT32   dCBWDataTransferLength;
    UINT8   bmCBWFlags;
    UINT8   bCBWLUN;
    UINT8   bCBWCBLength;
    UINT8   CBWCB[MAX_CBWCB_SIZE];
} __attribute__((packed))CBW, *PCBW;

// __attribute__ ((packed))
// Command Status Wrapper
//


//*****************************************************************************
//
// F U N C T I O N    P R O T O T Y P E S
//
// Direction, TRUE = Data-In, else Data-Out
//*****************************************************************************
//
UINT32 BOT_DataTransfer(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData, BOOL Direction );

UINT32 BOT_GetMaxLUN(PUSBMSC_DEVICE pUsbDevice, UINT8* pLun);

#endif // _BOT_H_

// EOF
