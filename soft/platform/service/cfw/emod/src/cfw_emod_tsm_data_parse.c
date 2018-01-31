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
#include <itf_api.h>
#include <cfw.h>
#include <cfw_prv.h>
#include <cos.h>
#include <ts.h>
#include "sul.h"
#include "cfw_emod_tsm.h"

extern VOID *SUL_MemCopy8 (VOID  *dest, CONST VOID *src, UINT32 count);

BOOL ArrayCopare(UINT8 *Src1, UINT8 *Src2, UINT8 nSize)
{
    UINT8 i = 0;
    for(i = 0; i < nSize; i++)
    {
        if(Src1[i] != Src2[i])
            return TRUE;
    }
    return FALSE;
}

BOOL CurrCellDataParse(CFW_TSM_CURR_CELL_INFO *pCurrCellInfo, api_CurCellInfoInd_t *pCurrCellInfoInd)
{
    BOOL nPostMark = FALSE;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Arfcn=%d!\n",0x0810070a)), pCurrCellInfoInd->Arfcn);
    if(pCurrCellInfo->nTSM_Arfcn != pCurrCellInfoInd->Arfcn)
    {
        pCurrCellInfo->nTSM_Arfcn = pCurrCellInfoInd->Arfcn;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_Arfcn=%d!\n",0x0810070b)), pCurrCellInfo->nTSM_Arfcn);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Rla=%d!\n",0x0810070c)), pCurrCellInfoInd->Rla);
    if(pCurrCellInfo->nTSM_AvRxLevel != pCurrCellInfoInd->Rla)
    {
        pCurrCellInfo->nTSM_AvRxLevel = pCurrCellInfoInd->Rla;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Rla=%d!\n",0x0810070d)), pCurrCellInfoInd->Rla);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("BcchDec=%d!\n",0x0810070e)), pCurrCellInfoInd->BcchDec);
    if(pCurrCellInfo->nTSM_BCCHDec != pCurrCellInfoInd->BcchDec)
    {
        pCurrCellInfo->nTSM_BCCHDec = pCurrCellInfoInd->BcchDec;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_BCCHDec=%d!\n",0x0810070f)), pCurrCellInfo->nTSM_BCCHDec);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Ber=%d!\n",0x08100710)), pCurrCellInfoInd->Ber);
    if(pCurrCellInfo->nTSM_BER != pCurrCellInfoInd->Ber)
    {
        pCurrCellInfo->nTSM_BER = pCurrCellInfoInd->Ber;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_BER=%d!\n",0x08100711)), pCurrCellInfo->nTSM_BER);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Bsic=%d!\n",0x08100712)), pCurrCellInfoInd->Bsic);
    if(pCurrCellInfo->nTSM_Bsic != pCurrCellInfoInd->Bsic)
    {
        pCurrCellInfo->nTSM_Bsic = pCurrCellInfoInd->Bsic;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_Bsic=%d!\n",0x08100713)), pCurrCellInfo->nTSM_Bsic);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("C1=%x!\n",0x08100714)), pCurrCellInfoInd->C1);
    if(pCurrCellInfo->nTSM_C1Value != pCurrCellInfoInd->C1)
    {
        pCurrCellInfo->nTSM_C1Value = pCurrCellInfoInd->C1;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_C1Value=%x!\n",0x08100715)), pCurrCellInfo->nTSM_C1Value);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CellId=%d!\n",0x08100716)), pCurrCellInfoInd->CellId[0]);

#ifdef LTE_NBIOT_SUPPORT
	if(ArrayCopare(pCurrCellInfo->nTSM_CellID, pCurrCellInfoInd->CellId, 4))
	{
		SUL_MemCopy8(pCurrCellInfo->nTSM_CellID, pCurrCellInfoInd->CellId, 4);
		CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CellID=%d!\n",0x08100717)), pCurrCellInfo->nTSM_CellID[0]);
		nPostMark = TRUE;
	}
#else
	if(ArrayCopare(pCurrCellInfo->nTSM_CellID, pCurrCellInfoInd->CellId, 2))
	{
		SUL_MemCopy8(pCurrCellInfo->nTSM_CellID, pCurrCellInfoInd->CellId, 2);
		CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CellID=%d!\n",0x08100717)), pCurrCellInfo->nTSM_CellID[0]);
		nPostMark = TRUE;
	}
#endif //LTE_NBIOT_SUPPORT
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Lai=%d!\n",0x08100718)), pCurrCellInfoInd->Lai[0]);
    if(ArrayCopare(pCurrCellInfo->nTSM_LAI, pCurrCellInfoInd->Lai, 5))
    {
        SUL_MemCopy8(pCurrCellInfo->nTSM_LAI, pCurrCellInfoInd->Lai, 5);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_LAI=%d!\n",0x08100719)), pCurrCellInfo->nTSM_LAI[0]);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("MTxPower=%d!\n",0x0810071a)), pCurrCellInfoInd->MTxPower);
    if(pCurrCellInfo->nTSM_MaxTxPWR != pCurrCellInfoInd->MTxPower)
    {
        pCurrCellInfo->nTSM_MaxTxPWR = pCurrCellInfoInd->MTxPower;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_MaxTxPWR=%d!\n",0x0810071b)), pCurrCellInfo->nTSM_MaxTxPWR);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Nmo=%d!\n",0x0810071c)), pCurrCellInfoInd->Nmo);
    if(pCurrCellInfo->nTSM_NetMode != pCurrCellInfoInd->Nmo)
    {
        pCurrCellInfo->nTSM_NetMode = pCurrCellInfoInd->Nmo;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_NetMode=%d!\n",0x0810071d)), pCurrCellInfo->nTSM_NetMode);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Rac=%d!\n",0x0810071e)), pCurrCellInfoInd->Rac);
    if(pCurrCellInfo->nTSM_RAC != pCurrCellInfoInd->Rac)
    {
        pCurrCellInfo->nTSM_RAC = pCurrCellInfoInd->Rac;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_RAC=%d!\n",0x0810071f)), pCurrCellInfo->nTSM_RAC);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("Ta=%d!\n",0x08100720)), pCurrCellInfoInd->Ta);
    if(pCurrCellInfo->nTSM_TimeADV != pCurrCellInfoInd->Ta)
    {
        pCurrCellInfo->nTSM_TimeADV = pCurrCellInfoInd->Ta;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_TimeADV=%d!\n",0x08100721)), pCurrCellInfo->nTSM_TimeADV);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("AC=%d!\n",0x08100722)), pCurrCellInfoInd->AC[0]);
    if(ArrayCopare(pCurrCellInfo->nTSM_AC, pCurrCellInfoInd->AC, 2))
    {

        SUL_MemCopy8(pCurrCellInfo->nTSM_AC, pCurrCellInfoInd->AC, 2);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_AC=%d!\n",0x08100723)), pCurrCellInfo->nTSM_AC[0]);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("BsAgBlkRes=%d!\n",0x08100724)), pCurrCellInfoInd->BsAgBlkRes);
    if(pCurrCellInfo->nTSM_BsAgBlkRes != pCurrCellInfoInd->BsAgBlkRes)
    {
        pCurrCellInfo->nTSM_BsAgBlkRes = pCurrCellInfoInd->BsAgBlkRes;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_BsAgBlkRes=%d!\n",0x08100725)), pCurrCellInfo->nTSM_BsAgBlkRes);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("C2=%x!\n",0x08100726)), pCurrCellInfoInd->C2);
    if(pCurrCellInfo->nTSM_C2 != pCurrCellInfoInd->C2)
    {
        pCurrCellInfo->nTSM_C2 = pCurrCellInfoInd->C2;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_C2=%x!\n",0x08100727)), pCurrCellInfo->nTSM_C2);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CCCHconf=%d!\n",0x08100728)), pCurrCellInfoInd->CCCHconf);
    if(pCurrCellInfo->nTSM_CCCHconf != pCurrCellInfoInd->CCCHconf)
    {
        pCurrCellInfo->nTSM_CCCHconf = pCurrCellInfoInd->CCCHconf;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CCCHconf=%d!\n",0x08100729)), pCurrCellInfo->nTSM_CCCHconf);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CurrBand=%d!\n",0x0810072a)), pCurrCellInfoInd->CurrBand);
    if(pCurrCellInfo->nTSM_CurrBand != pCurrCellInfoInd->CurrBand)
    {
        pCurrCellInfo->nTSM_CurrBand = pCurrCellInfoInd->CurrBand;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CurrBand=%d!\n",0x0810072b)), pCurrCellInfo->nTSM_CurrBand);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CurrChanMode=%x!\n",0x0810072c)), pCurrCellInfoInd->CurrChanMode);
    if(pCurrCellInfo->nTSM_CurrChanMode != pCurrCellInfoInd->CurrChanMode)
    {
        pCurrCellInfo->nTSM_CurrChanMode = pCurrCellInfoInd->CurrChanMode;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CurrChanMode=%x!\n",0x0810072d)), pCurrCellInfo->nTSM_CurrChanMode);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CurrChanType=%d!\n",0x0810072e)), pCurrCellInfoInd->CurrChanType);
    if(pCurrCellInfo->nTSM_CurrChanType != pCurrCellInfoInd->CurrChanType)
    {
        pCurrCellInfo->nTSM_CurrChanType = pCurrCellInfoInd->CurrChanType;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_CurrChanType=%d!\n",0x0810072f)), pCurrCellInfo->nTSM_CurrChanType);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("MaxRetrans=%d!\n",0x08100730)), pCurrCellInfoInd->MaxRetrans);
    if(pCurrCellInfo->nTSM_MaxRetrans != pCurrCellInfoInd->MaxRetrans)
    {
        pCurrCellInfo->nTSM_MaxRetrans = pCurrCellInfoInd->MaxRetrans;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_MaxRetrans=%d!\n",0x08100731)), pCurrCellInfo->nTSM_MaxRetrans);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RxLevAccMin=%d!\n",0x08100732)), pCurrCellInfoInd->RxLevAccMin);
    if(pCurrCellInfo->nTSM_RxLevAccMin != pCurrCellInfoInd->RxLevAccMin)
    {
        pCurrCellInfo->nTSM_RxLevAccMin = pCurrCellInfoInd->RxLevAccMin;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_RxLevAccMin=%d!\n",0x08100733)), pCurrCellInfo->nTSM_RxLevAccMin);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RxQualFull=%d!\n",0x08100734)), pCurrCellInfoInd->RxQualFull);
    if(pCurrCellInfo->nTSM_RxQualFull != pCurrCellInfoInd->RxQualFull)
    {
        pCurrCellInfo->nTSM_RxQualFull = pCurrCellInfoInd->RxQualFull;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_RxQualFull=%d!\n",0x08100735)), pCurrCellInfo->nTSM_RxQualFull);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("RxQualSub=%d!\n",0x08100736)), pCurrCellInfoInd->RxQualSub);
    if(pCurrCellInfo->nTSM_RxQualSub != pCurrCellInfoInd->RxQualSub)
    {
        pCurrCellInfo->nTSM_RxQualSub = pCurrCellInfoInd->RxQualSub;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nTSM_RxQualSub=%d!\n",0x08100737)), pCurrCellInfo->nTSM_RxQualSub);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("SI7_8=%d!\n",0x08100738)), pCurrCellInfoInd->SI7_8);
    if(pCurrCellInfo->bTSM_SI7_8 != pCurrCellInfoInd->SI7_8)
    {
        pCurrCellInfo->bTSM_SI7_8 = pCurrCellInfoInd->SI7_8;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bTSM_SI7_8=%d!\n",0x08100739)), pCurrCellInfo->bTSM_SI7_8);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("EC=%d!\n",0x0810073a)), pCurrCellInfoInd->EC);
    if(pCurrCellInfo->bTSM_EC != pCurrCellInfoInd->EC)
    {
        pCurrCellInfo->bTSM_EC = pCurrCellInfoInd->EC;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bTSM_EC=%d!\n",0x0810073b)), pCurrCellInfo->bTSM_EC);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("ATT=%d!\n",0x0810073c)), pCurrCellInfoInd->ATT);
    if(pCurrCellInfo->bTSM_ATT != pCurrCellInfoInd->ATT)
    {
        pCurrCellInfo->bTSM_ATT = pCurrCellInfoInd->ATT;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("bTSM_ATT=%d!\n",0x0810073d)), pCurrCellInfo->bTSM_ATT);
        nPostMark = TRUE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_CURCELLINFO_IND  nPostMark=%d!\r\n",0x0810073e)) , nPostMark);
    return nPostMark;
}

BOOL NeighborCellDataParse(CFW_TSM_ALL_NEBCELL_INFO *pAllNeighborCellInfo, api_AdjCellInfoInd_t *pNeigborCellInfoInd)
{
    BOOL nPostMark = FALSE;
    UINT8 i;

    if(pNeigborCellInfoInd->AdjCellNb > 6)
        pNeigborCellInfoInd->AdjCellNb = 6;

    pAllNeighborCellInfo->nTSM_NebCellNUM = pNeigborCellInfoInd->AdjCellNb;
    for(i = 0; i < pNeigborCellInfoInd->AdjCellNb; i++)
    {
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_Arfcn != pNeigborCellInfoInd->AdjCellElem[i].Arfcn)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_Arfcn = pNeigborCellInfoInd->AdjCellElem[i].Arfcn;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_AvRxLevel != pNeigborCellInfoInd->AdjCellElem[i].Rla)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_AvRxLevel = pNeigborCellInfoInd->AdjCellElem[i].Rla;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_BCCHDec != pNeigborCellInfoInd->AdjCellElem[i].BcchDec)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_BCCHDec = pNeigborCellInfoInd->AdjCellElem[i].BcchDec;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_Bsic != pNeigborCellInfoInd->AdjCellElem[i].Bsic)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_Bsic = pNeigborCellInfoInd->AdjCellElem[i].Bsic;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_C1Value != pNeigborCellInfoInd->AdjCellElem[i].C1)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_C1Value = pNeigborCellInfoInd->AdjCellElem[i].C1;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_MaxTxPWR != pNeigborCellInfoInd->AdjCellElem[i].MTxPower)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_MaxTxPWR = pNeigborCellInfoInd->AdjCellElem[i].MTxPower;
            nPostMark = TRUE;
        }
        if(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_RxLevAM != pNeigborCellInfoInd->AdjCellElem[i].Ram)
        {
            pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_RxLevAM = pNeigborCellInfoInd->AdjCellElem[i].Ram;
            nPostMark = TRUE;
        }
        if(SUL_MemCompare(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI, pNeigborCellInfoInd->AdjCellElem[i].Lai, SIZEOF(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI)))
        {
            SUL_MemCopy8(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI, pNeigborCellInfoInd->AdjCellElem[i].Lai, SIZEOF(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_LAI));
            nPostMark = TRUE;
        }
        if(SUL_MemCompare(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID, pNeigborCellInfoInd->AdjCellElem[i].CellId, SIZEOF(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID)))
        {
            SUL_MemCopy8(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID, pNeigborCellInfoInd->AdjCellElem[i].CellId, SIZEOF(pAllNeighborCellInfo->nTSM_NebCell[i].nTSM_CellID));
            nPostMark = TRUE;
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("API_ADJCELLSINFO_IND  nPostMark=%d!\r\n",0x0810073f)) , nPostMark);
    return nPostMark;
}



