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






#include "cs_types.h"
#include "chip_id.h"
#include "gsm.h"
// chip/include
#include "global_macros.h"
#include "excor.h"
// spal public header
#include "spal_excor.h"
#include "spal_mem.h"
// spal private header
#include "spalp_private.h"
#include "spalp_debug.h"

#if ( (CHIP_EDGE_SUPPORTED==1) || (defined CHIP_DIE_8955) )

PROTECTED CONST SPAL_COMPLEX_T LUT_8PSK[8] =
{
    {-SVAL_INVSQRT,  SVAL_INVSQRT}, // 0
    {-SVAL_NEGONE ,  SVAL_ZERO   }, // 1
    { SVAL_ZERO   ,  SVAL_ONE    }, // 2
    { SVAL_INVSQRT,  SVAL_INVSQRT}, // 3
    { SVAL_ZERO   , -SVAL_NEGONE }, // 4
    {-SVAL_INVSQRT, -SVAL_INVSQRT}, // 5
    { SVAL_INVSQRT, -SVAL_INVSQRT}, // 6
    { SVAL_ONE    ,  SVAL_ZERO   }  // 7
};


PROTECTED CONST SPAL_COMPLEX_T PSK8_ROTATE[16] =
{
    { SVAL_ONE    ,  SVAL_ZERO   },
    { SVAL_COS3PI8, -SVAL_SIN3PI8},
    {-SVAL_INVSQRT, -SVAL_INVSQRT},
    {-SVAL_SIN3PI8,  SVAL_COS3PI8},
    { SVAL_ZERO   ,  SVAL_ONE    },
    { SVAL_SIN3PI8,  SVAL_COS3PI8},
    { SVAL_INVSQRT, -SVAL_INVSQRT},
    {-SVAL_COS3PI8, -SVAL_SIN3PI8},
    {-SVAL_NEGONE ,  SVAL_ZERO   },
    {-SVAL_COS3PI8,  SVAL_SIN3PI8},
    { SVAL_INVSQRT,  SVAL_INVSQRT},
    { SVAL_SIN3PI8, -SVAL_COS3PI8},
    { SVAL_ZERO   ,  -SVAL_NEGONE},
    {-SVAL_SIN3PI8, -SVAL_COS3PI8},
    {-SVAL_INVSQRT,  SVAL_INVSQRT},
    { SVAL_COS3PI8,  SVAL_SIN3PI8}
};

PROTECTED CONST SPAL_COMPLEX_T GMSK_ROTATE[16] =
{
    { SVAL_ONE    ,  SVAL_ZERO   },
    { SVAL_ZERO   , -SVAL_NEGONE },
    {-SVAL_NEGONE ,  SVAL_ZERO   },
    { SVAL_ZERO   ,  SVAL_ONE    }
};


//--------------------------------------------------------------------------------
// Excor
//--------------------------------------------------------------------------------


//=============================================================================
// spal_XcorActive
//-----------------------------------------------------------------------------
/// Check if XCOR is activated
///
//=============================================================================

PRIVATE BOOL spal_ExcorActive()
{
    if (((hwp_excor->status) & EXCOR_STATUS_MASK))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


//=============================================================================
// spal_XcorBmmlZf()
//-----------------------------------------------------------------------------
/// Calculate the partial metrics used for the branch metrics and the ZF.
///
/// @param taps INT16*. address of input taps array.
/// @param metrics INT16. array containing the metrics.
///
//=============================================================================
// compute the BMML or partial ZF (two taps)
PUBLIC VOID spal_XcorBmmlZf( INT16* taps,
                             INT16* metric,
                             INT8             fracBits)
{

    UINT8 i;
    UINT8 excor_tmp0,excor_tmp1;
    SPAL_COMPLEX_T  localTable[8];
    SPAL_COMPLEX_T  *localTable_pt;

    SPAL_COMPLEX_T * pm         = (SPAL_COMPLEX_T *) &metric[2];
    SPAL_COMPLEX_T * taps_Cmp   = (SPAL_COMPLEX_T *) taps ;
    SPAL_COMPLEX_T * metric_Cmp = (SPAL_COMPLEX_T *) (MEM_ACCESS_UNCACHED(metric));

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorBmmlZf);

    localTable_pt = (SPAL_COMPLEX_T* )(MEM_ACCESS_UNCACHED(localTable));

    for (i=0; i < 8; i++)
    {
        localTable_pt[i].i = LUT_8PSK[i].i;
        localTable_pt[i].q = LUT_8PSK[i].q;
    }

    //////addr_reg0: taps_Cmp;  addr_reg1: LUT_8PSK; addr_reg2: pm

    hwp_excor->addr0 = (UINT32)taps_Cmp;        // RD address
    hwp_excor->addr1 = (UINT32)localTable;      // RD address
    hwp_excor->addr2 = (UINT32)pm;          // WR address

    hwp_excor->ctrl = EXCOR_CMD(EXCOR_BMMLZF) |EXCOR_NB_ILOOP(0) |
                      EXCOR_NB_OLOOP(ALL_TAP_LEN/2) |EXCOR_SHIFT_BIT(fracBits) ;

    // re-arrange the main branch matric
    excor_tmp0 = 0;
    excor_tmp1 = STATE_LEN;

    while (spal_ExcorActive()) ;

    for (i = 0; i< STATE_LEN; i++)
    {
        metric_Cmp[excor_tmp0].i = metric_Cmp[excor_tmp1].i;
        metric_Cmp[excor_tmp0].q= metric_Cmp[excor_tmp1].q;
        excor_tmp0 += STATE_LEN;
        excor_tmp1 += STATE_LEN;
    }

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorBmmlZf);
}

//=============================================================================
// spal_XcorComplexPower16
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param seq INT16*.INPUT. input sequence.
// @param out INT16*.OUTPUT. output sequence (may be null).
// @param len UINT16 .INPUT. The length of the input sequence.
// @param frac_bits INT8.INPUT.Rescale parameter.
// @return INT32.Power of the complex sequence
//
//=============================================================================
PUBLIC UINT32 spal_XcorComplexPower16(    INT16 *seq,
        INT16 *out,
        UINT16 len,
        INT8 fracBits)
{
#define MAX_ComplexPowerLen    200 //MAX length for complex power
    UINT16 i;
    UINT32 allPwr;
    SPAL_COMPLEX_T* SeqCmp = (SPAL_COMPLEX_T*) seq;

    UINT32 tmp_data;
    UINT32 tmp_out[MAX_ComplexPowerLen] ;
    UINT32 *pt0;
    UINT16 *pt1;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexPower);

    ///////addr_reg0: SeqCmp;  addr_reg2: out
    hwp_excor->addr0 = (UINT32)SeqCmp;      // RD address
    hwp_excor->addr2 = (UINT32)tmp_out;     // WR address

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_COMPPOW) |EXCOR_NB_ILOOP(len) |
                          EXCOR_NB_OLOOP(0) |EXCOR_SHIFT_BIT(fracBits) ;

    while (spal_ExcorActive()) ;

    allPwr = hwp_excor->data1;
    allPwr = allPwr >> fracBits;

    //convert from 32 bit tmp_out to 16 bit out;
    if(out)
    {
        pt0 = MEM_ACCESS_UNCACHED(tmp_out);
        pt1 = (UINT16 *)out;
        for (i = len; i > 0; i--)
        {
            tmp_data = *(pt0++);
            tmp_data = tmp_data & 0xffff;
            *(pt1++)    = (UINT16)tmp_data;
        }
    }

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexPower);

    return (allPwr);
}



//=============================================================================
// spal_XcorComplexPower32
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param seq INT16*.INPUT. input sequence.
// @param out UINT32*.OUTPUT. output sequence (may be null).
// @param len UINT16 .INPUT. The length of the input sequence.
// @param frac_bits INT8.INPUT.Rescale parameter.
// @return INT32.Power of the complex sequence
//
//=============================================================================
PUBLIC UINT32 spal_XcorComplexPower32 (    INT16 *seq,
        UINT32 *out,
        UINT16 len,
        INT8 fracBits)
{

    UINT32 allPwr;
    SPAL_COMPLEX_T* SeqCmp = (SPAL_COMPLEX_T*) seq;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexPower);

    ///////addr_reg0: SeqCmp;  addr_reg2: out
    hwp_excor->addr0 = (UINT32)SeqCmp;      // RD address
    hwp_excor->addr2 = (UINT32)out;     // WR address

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_COMPPOW) |EXCOR_NB_ILOOP(len) |
                          EXCOR_NB_OLOOP(0) |EXCOR_SHIFT_BIT(fracBits) ;

    while (spal_ExcorActive()) ;

    allPwr = hwp_excor->data1;
    allPwr = allPwr >> fracBits;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexPower);

    return (allPwr);
}


//======================================================================
// spal_XcorDerotate
//----------------------------------------------------------------------
// This function is used for 8PSK or GMSK signal phase derotate
//
// @param in        INT16*  INPUT.  Pointer to the input sequence.
// @param out       INT16*  OUTPUT. Pointer to the output sequence.
// @param Length    UINT16            INPUT.  Length of the input sequence.
// @param fracBits  INT8              INPUT.  Rescale parameter.
// @param ModType   UINT8             INPUT.  Defines the modulation type.
//
//======================================================================
PUBLIC VOID spal_XcorDerotate ( INT16* in,
                                INT16* out,
                                UINT16 Length,
                                UINT8 ModType )
{

    INT16 i;
    SPAL_COMPLEX_T* inCmp = (SPAL_COMPLEX_T*) in;
    SPAL_COMPLEX_T* outCmp = (SPAL_COMPLEX_T*) out;
    SPAL_COMPLEX_T   localTable[16];
    SPAL_COMPLEX_T* pTable = (ModType == GMSK) ? (SPAL_COMPLEX_T*)GMSK_ROTATE : (SPAL_COMPLEX_T*)PSK8_ROTATE;
    UINT8    modulo = (ModType == GMSK) ? 0x3 : 0xf;

    SPAL_COMPLEX_T *localTable_pt;
    SPAL_COMPLEX_T *pTable_pt;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDerotate);

    localTable_pt =    (SPAL_COMPLEX_T* )(MEM_ACCESS_UNCACHED(localTable));
    pTable_pt  =   (SPAL_COMPLEX_T* )(pTable);

    for (i=0; i < 16; i++)
    {
        localTable_pt[i].i= pTable_pt[i].i;
        localTable_pt[i].q= pTable_pt[i].q;
    }

    ///////addr_reg0: inCmp; addr_reg1: pTable; addr_reg2: outCmp
    ///////data_reg0: modulo

    hwp_excor->addr0 = (UINT32)inCmp;       // RD address
    hwp_excor->addr1 = (UINT32)localTable;      // RD address
    hwp_excor->addr2 = (UINT32)outCmp;      // WR address

    hwp_excor->data0 = (UINT32)modulo;

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_DEROTATE) |EXCOR_NB_ILOOP(Length) |
                          EXCOR_NB_OLOOP(0) |EXCOR_SHIFT_BIT(SVAL_FRAC_BITS) ;

    while (spal_ExcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDerotate);

}



//======================================================================
// spal_XcorDcIqCompensate
//----------------------------------------------------------------------
// This function is used in order to calculate from the 13 correlations
//
// @param inOut          INT16*    INPUT/OUTPUT.   Input/Output Data Buffer
// @param dc_compens     INT16*    INPUT.          Dc compensate
// @param iq_compens     INT16*    INPUT.          Iq compensate
// @param length         UINT16    INPUT.          Length of the Input/Output sequence.
// @param fracBits       INT8      INPUT           shift performed on the outut samples
//
//======================================================================
PUBLIC VOID spal_XcorDcIqCompensate(     INT16* inOut,
        INT16* dc_compens,
        INT16* iq_compens,
        UINT16 length,
        INT8 fracBits)
{


    UINT32 tmp0,tmp1;
    SPAL_COMPLEX_T* inOutCmp      = (SPAL_COMPLEX_T*) inOut;
    SPAL_COMPLEX_T* dc_compensCmp = (SPAL_COMPLEX_T*) dc_compens;
    SPAL_COMPLEX_T* iq_compensCmp = (SPAL_COMPLEX_T*) iq_compens;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDcIqCompensate);
    ///////addr_reg0: inOutCmp; addr_reg1: inOutCmp;
    ///////data_reg0: {dc_compensCmp.q, dc_compensCmp.i} ; data_reg1: {2048+iq_compensCmp.i, 2048-iq_compensCmp.i}

    hwp_excor->addr0 = (UINT32)inOutCmp;        // RD address
    hwp_excor->addr2 = (UINT32)inOutCmp;        // WR address

    tmp0 =  dc_compensCmp->q;
    tmp1 =  dc_compensCmp->i;
    tmp0 =  (tmp0 & 0xffff)<<16;
    tmp0 =  tmp0 | (tmp1 & 0xffff);
    hwp_excor->data0 = tmp0;

    tmp0 =  iq_compensCmp->i;
    tmp1 = tmp0 + 2048;
    tmp1 = (tmp1 & 0xffff) <<16;
    tmp0 = 2048 - tmp0;
    tmp0 = tmp0 & 0xffff;
    tmp1 = tmp1 | tmp0;
    hwp_excor->data1 = tmp1;

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_DCCOMP) |EXCOR_NB_ILOOP(length) |
                          EXCOR_NB_OLOOP(0) |EXCOR_SHIFT_BIT(fracBits) ;

    while (spal_ExcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDcIqCompensate);


}



//=============================================================================
// spal_XcorSrecPSK8
//-----------------------------------------------------------------------------
/// ReconstructS a reference 8-PSK burst from channel taps and estimated/known hard bits.
///
/// @param taps     INT16*.  INPUT.  Pointer to the channel taps.
/// @param size     UINT8.   INPUT.  Length of the reconstructed burst.
/// @param HardBits UINT8*.  INPUT.  Pointert to the Hard decision bits. to be reconstructed.
/// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
/// @param chTapNb  UINT8.   INPUT.  Number of the channel taps.
/// @param regShift UINT8.   INPUT.  Rescale parameter.
///
//=============================================================================
PUBLIC VOID spal_XcorSrecPSK8(    INT16* taps,
                                  UINT8 size,
                                  UINT32* HardBits,
                                  UINT32* refBurst,
                                  UINT8 chTapNb,
                                  UINT8 regShift)
{
    SPAL_COMPLEX_T  localTable[8];
    UINT32 temp;
    INT16 i;
    SPAL_COMPLEX_T  *localTable_pt;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorSrecPSK8);

    localTable_pt = (SPAL_COMPLEX_T* )(MEM_ACCESS_UNCACHED(localTable));

    for (i=0; i < 8; i++)
    {
        localTable_pt[i].i = LUT_8PSK[i].i;
        localTable_pt[i].q = LUT_8PSK[i].q;
    }

    /////addr_reg0: taps last address; addr_reg1: LUT_8PSK; addr_reg2: refBurst; addr_reg3: HardBits;
    hwp_excor->addr0 = (UINT32)(&taps[2*(chTapNb-1)]);
    hwp_excor->addr1 = (UINT32)localTable;
    hwp_excor->addr2 = (UINT32)refBurst;
    hwp_excor->addr3 = (UINT32)HardBits;

    temp = size-chTapNb;

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_SRECPSK8) |EXCOR_NB_ILOOP(chTapNb) |
                          EXCOR_NB_OLOOP(temp) |EXCOR_SHIFT_BIT(regShift) ;

    while (spal_ExcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorSrecPSK8);

}



//=============================================================================
// spal_XcorComplexConvolution
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param Xseq INT16.INPUT.The first input sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence.
// @param Yseq INT16.INPUT.The second input sequence.
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence (16 or 32-bit complex).
// @param Zoff UINT16.INPUT. Offset in the ouptut sequence
// @param Zlen UINT16.INPUT. Max length of the output sequence (discard if negative)
// @param frac_bits INT8.INPUT.Rescale parameter.
//
//=============================================================================
PUBLIC VOID spal_XcorComplexConvolution (    INT16* Xseq,
        UINT16 Xlen,
        INT16* Yseq,
        UINT16 Ylen,
        INT16* Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits   )
{
    VOLATILE UINT32 tmp0;
    SPAL_COMPLEX_T *XseqCmp =  (SPAL_COMPLEX_T*)Xseq;
    SPAL_COMPLEX_T *YseqCmp =  (SPAL_COMPLEX_T*)Yseq;
    SPAL_COMPLEX_T *ZseqCmp =  (SPAL_COMPLEX_T*)Zseq;

    UINT16             OutLoopLen = Xlen + Ylen - 1 - Zoff;

    //               "config registers"
    UINT16            regLoopExt = ((OutLoopLen > Zlen) && (Zlen > 0)) ? Zlen : OutLoopLen;
    UINT16            regPtrIdx  = Zoff + 1;
    UINT16            regMinLen  = (Xlen < Ylen) ? Xlen : Ylen;
    UINT16            regAllLen  = Xlen + Ylen;
    UINT16            regYLen    = Ylen;
    INT8              regShift   = fracBits;

    SPAL_COMPLEX_T * regPtrX    = XseqCmp - Ylen;
    SPAL_COMPLEX_T * regPtrY    = YseqCmp - 1;
    SPAL_COMPLEX_T * regPtrZ    = ZseqCmp;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexConvolution);

    ////addr_reg0: regPtrX;  addr_reg1: regPtrY; addr_reg2: regPtrZ
    ////data_reg0: {regAllLen,regMinLen}, data_reg1: {regYLen,regPtrIdx}
    hwp_excor->addr0 = (UINT32)regPtrX;         // RD address
    hwp_excor->addr1     = (UINT32)regPtrY;        // RD address
    hwp_excor->addr2     = (UINT32)regPtrZ;         // WR address

    tmp0 = regAllLen & 0xffff;
    tmp0 = tmp0 <<16;
    tmp0 = tmp0 | (regMinLen &  0xffff);
    hwp_excor->data0 = tmp0;             // {regAllLen,regMinLen}

    tmp0 = regYLen & 0xffff;
    tmp0 = tmp0 <<16;
    tmp0 = tmp0 | (regPtrIdx &  0xffff);
    hwp_excor->data1 = tmp0;             // {regYLen,regPtrIdx}

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_COMPCONV) |EXCOR_NB_ILOOP(0) |
                          EXCOR_NB_OLOOP(regLoopExt) |EXCOR_SHIFT_BIT(regShift) ;

    while (spal_ExcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexConvolution);


}



//====================================================================================
//    spal_XcorComplexMatrixMultiply
//
//    This function is used to compute double convolution in the DC IQ estimation function.
//
//  @param inVector  SPAL_COMPLEX_T*  input vector
//  @param inMatrix  SPAL_COMPLEX_T*  input matrix
//  @param outMatrix SPAL_COMPLEX_T*  output matrix (multiplication result)
//  @param columnLen UINT8             number of input matrix columns
//  @param rowLen    UINT8             number of input matrix rows
//  @param fracBits  UINT8             fractional bits of the data format
//
//
//====================================================================================

PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply(
    INT16* inVector,
    INT16* inMatrix,
    INT16* outMatrix,
    UINT8   columnLen,
    UINT8   rowLen,
    INT8    fracBits)
{
    SPAL_CMPLX32_T Accu;

    SPAL_COMPLEX_T* inVectCmp  =  (SPAL_COMPLEX_T*) inVector;
    SPAL_COMPLEX_T* outMatrCmp =  (SPAL_COMPLEX_T*) outMatrix;
    SPAL_COMPLEX_T* outPtr = (SPAL_COMPLEX_T*)(MEM_ACCESS_UNCACHED(&outMatrCmp[columnLen-1]));

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexMatrixMultiply);

    ////addr_reg0: inMatrCmp;  addr_reg1: inVectCmp; addr_reg2: outMatrCmp
    if ( ( (columnLen == 0) || (rowLen == 0)) && (outMatrix != NULL) )
    {
        *outMatrix = 0;
        *(outMatrix+1) = 0;
        Accu.i = 0;
        Accu.q = 0;
        return Accu;
    }

    hwp_excor->addr0 = (UINT32)inMatrix;             // RD address
    hwp_excor->addr1 = (UINT32)inVectCmp;            // RD address
    hwp_excor->addr2 = (UINT32)outMatrCmp;           // WR address
    hwp_excor->ctrl  = EXCOR_CMD(EXCOR_COMPMATRIMUL) |EXCOR_NB_ILOOP(rowLen) |
                       EXCOR_NB_OLOOP(columnLen) |EXCOR_SHIFT_BIT(fracBits) ;

    while (spal_ExcorActive()) ;

    //output last Accu
    if (outMatrix)
    {
        Accu.i = (INT32)(outPtr->i);
        Accu.q = (INT32)(outPtr->q);
    }
    else
    {
        Accu.i = (INT32)(hwp_excor->data1);
        Accu.q = (INT32)(hwp_excor->data3);
    }

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexMatrixMultiply);

    return (Accu);

}


//=============================================================================
// spal_XcorFastConvolution
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence, 2(Ylen -1) zreos added orignal vector .
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param frac_bits0 INT8.INPUT.Rescale parameter.
// @param frac_bits1 INT8.INPUT.Rescale parameter.
//=============================================================================
PUBLIC VOID spal_XcorFastConvolution (    INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT16  Xlen,
        UINT16  Ylen,
        UINT8 fracBits0,
        UINT8 fracBits1)
{

    spal_InvalidateDcache();
    hwp_excor->addr0 = (UINT32)Xseq;         // RD address
    hwp_excor->addr1     = (UINT32)Yseq;        // RD address
    hwp_excor->addr2     = (UINT32)Zseq;         // WR address

    hwp_excor->ctrl_fast     =  (fracBits1&0xFF)<< 16 ;

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_FASTCONV) |EXCOR_NB_ILOOP(Ylen) |
                          EXCOR_NB_OLOOP(Xlen) |EXCOR_SHIFT_BIT(fracBits0) ;

    while (spal_ExcorActive()) ;

}
//====================================================================================
//    spal_XcorFastMatrixMultiply
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param rowALen UINT8             number of input matrix A  rows
//  @param  columnBLen    UINT8     number of input matrix B column
//  @param  LoopABLen    UINT8      loop number of input matrix A,B calculation
//  @param fracBits0  UINT8            Rescale bit
//  @param fracBits1  UINT8            Rescale bit
//====================================================================================

PUBLIC VOID spal_XcorFastMatrixMultiply(
    INT16* inVectorA,
    INT16* inVectorB,
    INT16* outMatrix,
    UINT8   rowALen,
    UINT8   columnBLen,
    UINT8   LoopABLen,
    UINT8    fracBits0,
    UINT8    fracBits1)
{

    spal_InvalidateDcache();

    hwp_excor->addr0 = (UINT32)inVectorA;             // RD address
    hwp_excor->addr1 = (UINT32)inVectorB;            // RD address
    hwp_excor->addr2 = (UINT32)outMatrix;           // WR address

    hwp_excor->ctrl_fast  = EXCOR_LOOP_NUM_A(rowALen)|EXCOR_LOOP_NUM_B(columnBLen)|EXCOR_LOOP_NUM_AB(LoopABLen)|((fracBits1&0xFF)<< 16);

    hwp_excor->ctrl  = EXCOR_CMD(EXCOR_FASTMATRIMUL) |EXCOR_SHIFT_BIT(fracBits0) ;

    while (spal_ExcorActive()) ;

}
//====================================================================================
//    spal_XcorFastMatrixMultiplyS
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param ctrl  UINT32           ctrl register configuration value.
//====================================================================================

PUBLIC  VOID spal_XcorFastMatrixMultiplyS(INT16* inVectorA,
        INT16* inVectorB,
        INT16* outMatrix,
        UINT32  ctrl)
{

    spal_InvalidateDcache();

    hwp_excor->addr0 = (UINT32)inVectorA;             // RD address
    hwp_excor->addr1 = (UINT32)inVectorB;            // RD address
    hwp_excor->addr2 = (UINT32)outMatrix;           // WR address

    hwp_excor->ctrl  = ctrl;

    while (spal_ExcorActive());
}

//====================================================================================
//    spal_XcorFastMatrixMultiplyInit
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param ctrl  UINT32           ctrl register configuration value.
//====================================================================================
PUBLIC  VOID spal_XcorFastMatrixMultiplyInit(INT16* inVectorA,
        INT16* inVectorB,
        INT16* outMatrix,
        UINT32   ctrl)
{
    spal_InvalidateDcache();

    hwp_excor->addr0 = (UINT32)inVectorA;             // RD address
    hwp_excor->addr1 = (UINT32)inVectorB;            // RD address
    hwp_excor->addr2 = (UINT32)outMatrix;           // WR address

    hwp_excor->ctrl  = ctrl ;

}
//====================================================================================
//    spal_XcorFastMatrixMultiplySet
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param rowALen UINT8             number of input matrix A  rows
//  @param  columnBLen    UINT8     number of input matrix B column
//  @param  LoopABLen    UINT8      loop number of input matrix A,B calculation
//  @param fracBits0  UINT8            Rescale bit
//  @param fracBits1  UINT8            Rescale bit
//====================================================================================

PUBLIC  VOID spal_XcorFastMatrixMultiplySet(INT16* inVectorA,
        INT16* inVectorB,
        INT16* outMatrix,
        UINT8   rowALen,
        UINT8   columnBLen,
        UINT8   LoopABLen,
        UINT8    fracBits0,
        UINT8    fracBits1)
{
    spal_InvalidateDcache();
    hwp_excor->addr0 = (UINT32)inVectorA;             // RD address
    hwp_excor->addr1 = (UINT32)inVectorB;            // RD address
    hwp_excor->addr2 = (UINT32)outMatrix;           // WR address
    hwp_excor->ctrl_fast  = EXCOR_LOOP_NUM_A(rowALen)|EXCOR_LOOP_NUM_B(columnBLen)|EXCOR_LOOP_NUM_AB(LoopABLen)|((fracBits1&0xFF)<< 16);
    hwp_excor->ctrl  = EXCOR_CMD(EXCOR_FASTMATRIMUL) |EXCOR_SHIFT_BIT(fracBits0) ;

}

//=============================================================================
// spal_XcorFastConvolutionSet
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param ctrl UINT32.INPUT. ctrl register configuration.
//=============================================================================

PUBLIC  VOID spal_XcorFastConvolutionSet (INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT32 ctrl
                                         )
{
    spal_InvalidateDcache();

    hwp_excor->addr0 = (UINT32)Xseq;         // RD address
    hwp_excor->addr1     = (UINT32)Yseq;        // RD address
    hwp_excor->addr2     = (UINT32)Zseq;         // WR address

    hwp_excor->ctrl     = ctrl ;

    while (spal_ExcorActive()) ;

}

//=============================================================================
// spal_XcorFastConvolutionInit
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param ctrl UINT32.INPUT. ctrl register configuration.
//=============================================================================

PUBLIC  VOID spal_XcorFastConvolutionInit (INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT32 ctrl)
{
    spal_InvalidateDcache();
    hwp_excor->addr0 = (UINT32)Xseq;         // RD address
    hwp_excor->addr1     = (UINT32)Yseq;        // RD address
    hwp_excor->addr2     = (UINT32)Zseq;         // WR address
    hwp_excor->ctrl     = ctrl;
}


//====================================================================================
//    spal_ExcorIrRecombine
//
//    combine ir data
//
//  @param inBuf0 UINT8*  input buffer 0
//  @param inOutPps0 UINT32*  inout buffer 0's puncture table and output combined puncture table
//  @param inBuf1 UINT8*  input buffer 1
//  @param inBuf1_2nd UINT8*  input buffer 1, 2nd part in ir buf
//  @param inPps1 UINT32*  inout buffer 1's puncture table
//  @param flag SPAL_IR_MCS6PAD_COMBINE_T  input mcs6pad mode
//  @param len    UINT8  comb length
//  @param outBuf UINT8* out buffer
//
//====================================================================================
PUBLIC VOID spal_ExcorIrRecombine( UINT8*  inBuf0,
                                   UINT32* inOutPps0,
                                   UINT8*  inBuf1,
                                   UINT8*  inBuf1_2nd,
                                   UINT32* inPps1,
                                   SPAL_IR_MCS6PAD_COMBINE_T flag,
                                   UINT16  len,
                                   UINT8*  outBuf)
{
    UINT32 psIdx0,psIdx1,psIdx2;
    INT16 joff,j,j1;
    UINT32 tmp,tmp1;
    UINT32 tmp2;
    UINT32 *pt0;
    UINT32 *pt1;
    UINT16 *pt2;
    UINT32 save_idx2;
    UINT8  save_data0,save_data1,save_data2;
    save_data0 = save_data1 = save_data2 = 0;
    SPAL_PROFILE_FUNCTION_ENTER(spal_ExcorIrRecombine);

    if (flag == IR_COMBINE_MCS6PAD_TO_MCS8)
        joff = 144;// 144=1836-1692(conv enc output len)
    else if (flag == IR_COMBINE_MCS8_TO_MCS6PAD)
        joff = -144;
    else
        joff = 0;
    psIdx0=0;
    psIdx1=0;
    psIdx2=0;
    j=0;
    // if joff<0, count punct bits for buffer 0 and out buffer
    pt0 = inOutPps0;
    tmp2= *pt0++;
    for(j1=joff; j1<0;)
    {
        if (tmp2 & 1)
            outBuf[psIdx2++] = inBuf0[psIdx0++];
        if (tmp2 & 2)
            outBuf[psIdx2++] = inBuf0[psIdx0++];
        if (tmp2 & 4)
            outBuf[psIdx2++] = inBuf0[psIdx0++];
        if (tmp2 & 8)
            outBuf[psIdx2++] = inBuf0[psIdx0++];
        tmp2 = tmp2>>4;
        j+=4;
        j1+=4;
        if((j & 0x1f) == 0)
            tmp2 = *pt0++;
    }

    // if joff>0, count punct bits for buffer 1(IR buf)
    pt0 = inPps1;
    tmp2= *pt0++;
    for(j1=0; j1<joff;)
    {
        if (tmp2 & 1)
            psIdx1++;
        if (tmp2 & 2)
            psIdx1++;
        if (tmp2 & 4)
            psIdx1++;
        if (tmp2 & 8)
            psIdx1++;
        tmp2 = tmp2>>4;
        j1+=4;
        if((j1 & 0x1f) == 0)
            tmp2 = *pt0++;
    }
    save_idx2  = psIdx2;

    if ( (save_idx2&3) !=0)
    {
        save_data0 = outBuf[save_idx2-1];
        save_data1 = outBuf[save_idx2-2];
        save_data2 = outBuf[save_idx2-3];
    }
    /////addr_reg0: pos0 address; addr_reg1: pos1 address; addr_reg2: inbuf0 address; addr_reg3: inbuf1 address;
    /////addr_reg4: outbuf address; addr_reg5: inbuf1_second address;
    hwp_excor->addr0 = (UINT32)(inOutPps0 + (j>>5));
    hwp_excor->addr1 = (UINT32)(inPps1 + (j1>>5));
    hwp_excor->addr2 = (UINT32)(inBuf0 + psIdx0);
    hwp_excor->addr3 = (UINT32)(inBuf1 + (psIdx1>>1));
    hwp_excor->addr4 = (UINT32)(outBuf + psIdx2);
    hwp_excor->addr5 = (UINT32)(inBuf1_2nd);
    ///////data_reg0-3: compress table, 8 bit data, from 0 to 15
    hwp_excor->data0 = (UINT32)0x1c140c04;  //28,20,12,4
    hwp_excor->data1 = (UINT32)0x70503828;  //112,80,56,40
    hwp_excor->data2 = (UINT32)0x9c948c84;  //156,148,140,132
    hwp_excor->data3 = (UINT32)0xf0d2b8a8;  //240,210,184,168
    ///////data_reg4[3:0]: psidx0, data_reg4[7:4]: psidx2, data_reg4[18:8]: psidx1,
    hwp_excor->data4 = EXCOR_IRCOM_PSIDX0(psIdx0) | EXCOR_IRCOM_PSIDX1(psIdx1) | EXCOR_IRCOM_PSIDX2(psIdx2);
    ////////tmp:  j[4:0] and j1[4:0]
    /////// tmp1: loop num /32
    tmp = ((j&0x1f)>>1)|((j1&0x1f)<<3);
    tmp1= ((len - j)>>5) + 2;//huangxuan changed @20130711
    if(psIdx0 != psIdx1)
        tmp1 = tmp1*2-2;
    //////data_reg5  threshold of switching from inBuf1 to inBuf1_2nd
    hwp_excor->data5 = 624;
    hwp_excor->ctrl  = EXCOR_CMD(EXCOR_IR_COMB) |EXCOR_NB_ILOOP(tmp)| EXCOR_NB_OLOOP(tmp1);

    spal_InvalidateDcache();

    while (spal_ExcorActive()) ;

    if ((save_idx2&0x3)!=0)
    {
        outBuf[save_idx2-1] = save_data0;
        outBuf[save_idx2-2] = save_data1;
        outBuf[save_idx2-3] = save_data2;
    }

// punc table combine

    if (inPps1)
    {
        if (flag == IR_COMBINE_MCS6PAD_TO_MCS8)
        {
            pt1 = inOutPps0;
            pt2 = (UINT16 *)inPps1;
            pt2 = pt2 + 9;
            for (j = 53; j >0; j--)
            {
                tmp2  = (*pt1) | (*(pt2+1)<<16) | (*pt2);
                *pt1++= tmp2;
                pt2  += 2;
            }
        }
        else if (flag == IR_COMBINE_MCS8_TO_MCS6PAD)
        {
            pt1 = inOutPps0 + 4;
            pt2 = (UINT16 *)inPps1;
            tmp2 = (*pt1) | ((*pt2)<<16);
            *pt1++= tmp2;
            pt2++;
            for (j = 53; j >0; j--)
            {
                tmp2  = (*pt1) | (*(pt2+1)<<16) | (*pt2);
                *pt1++= tmp2;
                pt2  += 2;
            }
        }
        else
        {
            pt0 = inOutPps0;
            pt1 = inPps1;
            for (j= 58; j > 0; j--)
            {
                tmp2 = (*pt0) | (*pt1++);
                *pt0++ = tmp2;
            }

        }
    }

    SPAL_PROFILE_FUNCTION_EXIT(spal_ExcorIrRecombine);
}




//======================================================================
// spal_ExcorIQShift
//----------------------------------------------------------------------
// This function is used perform i/q shift
//
// @param IQ_ADDR   INT32*  INPUT.  Pointer to the input sequence. iq packed
// @param len       UINT16            INPUT.  Length of the input sequence.
// @param fracBits  INT8              INPUT.  Rescale parameter. Pos: >>, neg: <<
//
//======================================================================

PUBLIC VOID spal_ExcorIQShift(UINT32* IQ_ADDR,UINT16 len,INT8 fracBits)
{
    INT8 iq_shift_bits;
    iq_shift_bits = fracBits + 16;
    hwp_excor->addr0 = (UINT32)IQ_ADDR;    // RD address
    hwp_excor->addr1 = (UINT32)IQ_ADDR;   // WR address
    hwp_excor->ctrl  = EXCOR_CMD(EXCOR_IQ_SHIFT) |EXCOR_NB_ILOOP(0) |
                       EXCOR_NB_OLOOP(len+1) |EXCOR_SHIFT_BIT(iq_shift_bits) ;
    //spal_InvalidateDcache();// inout should be uncached
    while (spal_ExcorActive()) ;
}


//======================================================================
// spal_XcorEgprsRotate
//----------------------------------------------------------------------
// This function is used for 8PSK signal phase rotate
//
// @param in        INT16*  INPUT.  Pointer to the input sequence.
// @param out       INT16*  OUTPUT. Pointer to the output sequence.
// @param offset   UINT8 INPUT. offset for phase rotation
// @param Length    UINT16            INPUT.  Length of the input sequence.
//
//======================================================================
PUBLIC VOID spal_ExcorEgprsRotate ( INT16* in,
                                    INT16* out,
                                    UINT8 offset,
                                    UINT16 Length)
{

    INT16 i;
    SPAL_COMPLEX_T* inCmp = (SPAL_COMPLEX_T*) in;
    SPAL_COMPLEX_T* outCmp = (SPAL_COMPLEX_T*) out;
    SPAL_COMPLEX_T   localTable[16];
    SPAL_COMPLEX_T* pTable = (SPAL_COMPLEX_T*)PSK8_ROTATE;
    UINT8    modulo = 0xf;

    SPAL_COMPLEX_T *localTable_pt;
    SPAL_COMPLEX_T *pTable_pt;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDerotate);

    localTable_pt =    (SPAL_COMPLEX_T* )(MEM_ACCESS_UNCACHED(localTable));
    pTable_pt  =   (SPAL_COMPLEX_T* )(pTable);

    for (i=0; i < 16; i++)
    {
        localTable_pt[i].i= pTable_pt[(i+offset)&0xF].i;
        localTable_pt[i].q= -pTable_pt[(i+offset)&0xF].q;
    }

    ///////addr_reg0: inCmp; addr_reg1: pTable; addr_reg2: outCmp
    ///////data_reg0: modulo

    hwp_excor->addr0 = (UINT32)inCmp;       // RD address
    hwp_excor->addr1 = (UINT32)localTable;      // RD address
    hwp_excor->addr2 = (UINT32)outCmp;      // WR address

    hwp_excor->data0 = (UINT32)modulo;

    hwp_excor->ctrl     = EXCOR_CMD(EXCOR_DEROTATE) |EXCOR_NB_ILOOP(Length) |
                          EXCOR_NB_OLOOP(0) |EXCOR_SHIFT_BIT(SVAL_FRAC_BITS) ;

    while (spal_ExcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDerotate);

}


#endif
