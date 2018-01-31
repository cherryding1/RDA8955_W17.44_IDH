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























#include <itf_api.h>

#ifndef _CFW_EMOD_TSM_IN_H_
#define _CFW_EMOD_TSM_IN_H_


#define BIND_INFO_SIZE          SIZEOF(bind_info_t)


#define FACTORY_IMEISV          4
#define FACTORY_MB_SN           13
#define FACTORY_MP_SN           77
#define BIND_SIMSIGN            0
#define BIND_SIMNO          1
#define BIND_NWSIGN         91
#define BIND_NWNO           92


// =============================================================================
// bind_info_t
// -----------------------------------------------------------------------------
/// Define the banding info of the SIM Card.
// =============================================================================
typedef struct
{
    UINT8 sim_Sign;     // The sign that indicates if band SIM card
    UINT8 simNo[10][9]; // It means that can band up-to 10 SIM cards and each SIM card number ties up 9 bytes spaces
    UINT8 nw_Sign;      // The sign that indicates if band network
    UINT8 nwNo[5][3];   // It means that can band up-to 5 networks and each network has 3 identifiers
} bind_info_t;


typedef struct _SIM_TSM_STATE
{
    UINT8 nPreState;
    UINT8 nCurrState;
    UINT8 nNextState;
    UINT8 pad;
} CFW_TSM_STATE;

typedef struct _CFW_TSM_GET_INFO
{
    CFW_TSM_FUNCTION_SELECT SelecNum;
    CFW_TSM_STATE           nState;
    BOOL                    bClose;
    UINT8                   pad[3];
} CFW_TSM_GET_INFO;

/******************************tsm data prase*************************************************************/
BOOL CurrCellDataParse(CFW_TSM_CURR_CELL_INFO *pCurrCellInfo, api_CurCellInfoInd_t *pCurrCellInfoInd);
BOOL NeighborCellDataParse(CFW_TSM_ALL_NEBCELL_INFO *pAllNeighborCellInfo, api_AdjCellInfoInd_t *pNeigborCellInfoInd);



/*******************************************rsp************************************************************/
UINT32 CFW_EmodOutfieldTestProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_EmodSyncInfoTestProc (HAO hAo, CFW_EV *pEvent);

/**********************************************************************************************************/


#define CFW_TSM_IDLE                        0x01
#define CFW_TSM_REC_IND                     0x02

VOID Emod_ASCtoIMSI(UINT8 *InPut, UINT8 *OutPut, UINT8 InLen);
VOID Emod_IMSItoASC(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen);
#endif // _CFW_EMOD_TSM_IN_H_
