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







#include "vpp_hr_typedefs.h"
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_mathhalf_macro.h"

/***************************************************************************
 *
 *   FUNCTION NAME: saturate
 *
 *   PURPOSE:
 *
 *     Limit the 32 bit input to the range of a 16 bit word.
 *
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   KEYWORDS: saturation, limiting, limit, saturate, 16 bits
 *
 *************************************************************************/

/*static INT16 saturate(INT32 L_var1)
{
  INT16 swOut;

  if (L_var1 > SW_MAX)
  {
    swOut = SW_MAX;
  }
  else if (L_var1 < SW_MIN)
  {
    swOut = SW_MIN;
  }
  else
    swOut = (INT16) L_var1;        // automatic type conversion
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: abs_s
 *
 *   PURPOSE:
 *
 *     Take the absolute value of the 16 bit input.  An input of
 *     -0x8000 results in a return value of 0x7fff.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0x0000 0000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Take the absolute value of the 16 bit input.  An input of
 *     -0x8000 results in a return value of 0x7fff.
 *
 *   KEYWORDS: absolute value, abs
 *
 *************************************************************************/

/*INT16 abs_s(INT16 var1)
{
  INT16 swOut;

  if (var1 == SW_MIN)
  {
    swOut = SW_MAX;
  }
  else
  {
    if (var1 < 0)
      swOut = -var1;
    else
      swOut = var1;
  }
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: add
 *
 *   PURPOSE:
 *
 *     Perform the addition of the two 16 bit input variable with
 *     saturation.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform the addition of the two 16 bit input variable with
 *     saturation.
 *
 *     swOut = var1 + var2
 *
 *     swOut is set to 0x7fff if the operation results in an
 *     overflow.  swOut is set to 0x8000 if the operation results
 *     in an underflow.
 *
 *   KEYWORDS: add, addition
 *
 *************************************************************************/

/*INT16 add(INT16 var1, INT16 var2)
{
  INT32 L_sum;
  INT16 swOut;

  L_sum = (INT32) var1 + var2;
  swOut = saturate(L_sum);
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: divide_s
 *
 *   PURPOSE:
 *
 *     Divide var1 by var2.  Note that both must be positive, and
 *     var1 >=  var2.  The output is set to 0 if invalid input is
 *     provided.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     In the case where var1==var2 the function returns 0x7fff.  The output
 *     is undefined for invalid inputs.  This implementation returns zero
 *     and issues a warning via stdio if invalid input is presented.
 *
 *   KEYWORDS: divide
 *
 *************************************************************************/

/*INT16 divide_s(INT16 var1, INT16 var2)
{
  INT32 L_div;
  INT16 swOut;

  if (var1 < 0 || var2 < 0 || var1 > var2)
  {
    // undefined output for invalid input into divide_s
    return (0);
  }

  if (var1 == var2)
    return (0x7fff);

  L_div = ((0x00008000L * (INT32) var1) / (INT32) var2);
  swOut = saturate(L_div);
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: extract_h
 *
 *   PURPOSE:
 *
 *     Extract the 16 MS bits of a 32 bit INT32.  Return the 16 bit
 *     number as a INT16.  This is used as a "truncation" of a fractional
 *     number.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *   KEYWORDS: assign, truncate
 *
 *************************************************************************/

/*INT16 extract_h(INT32 L_var1)
{
  INT16 var2;

  var2 = (INT16) (0x0000ffffL & (L_var1 >> 16));
  return (var2);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: extract_l
 *
 *   PURPOSE:
 *
 *     Extract the 16 LS bits of a 32 bit INT32.  Return the 16 bit
 *     number as a INT16.  The upper portion of the input INT32
 *     has no impact whatsoever on the output.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *
 *   KEYWORDS: extract, assign
 *
 *************************************************************************/

/*INT16 extract_l(INT32 L_var1)
{
  INT16 var2;

  var2 = (INT16) (0x0000ffffL & L_var1);
  return (var2);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_abs
 *
 *   PURPOSE:
 *
 *     Take the absolute value of the 32 bit input.  An input of
 *     -0x8000 0000 results in a return value of 0x7fff ffff.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *
 *
 *   KEYWORDS: absolute value, abs
 *
 *************************************************************************/
/*INT32 L_abs(INT32 L_var1)
{
  INT32 L_Out;

  if (L_var1 == LW_MIN)
  {
    L_Out = LW_MAX;
  }
  else
  {
    if (L_var1 < 0)
      L_Out = -L_var1;
    else
      L_Out = L_var1;
  }
  return (L_Out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_add
 *
 *   PURPOSE:
 *
 *     Perform the addition of the two 32 bit input variables with
 *     saturation.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *     L_var2
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform the addition of the two 32 bit input variables with
 *     saturation.
 *
 *     L_Out = L_var1 + L_var2
 *
 *     L_Out is set to 0x7fff ffff if the operation results in an
 *     overflow.  L_Out is set to 0x8000 0000 if the operation
 *     results in an underflow.
 *
 *   KEYWORDS: add, addition
 *
 *************************************************************************/

/*INT32 L_add (INT32 L_var1, INT32 L_var2)
{
    INT32 L_var_out;

    L_var_out = L_var1 + L_var2;

    if (((L_var1 ^ L_var2) & LW_MIN) == 0)
    {
        if ((L_var_out ^ L_var1) & LW_MIN)
        {
            L_var_out = (L_var1 < 0) ? LW_MIN : LW_MAX;
            //Overflow = 1;
        }
    }

    return (L_var_out);
}*/
/*INT32 L_add(INT32 L_var1, INT32 L_var2)
{

  INT32 L_Sum,
         L_SumLow,
         L_SumHigh;

  L_Sum = L_var1 + L_var2;

  if ((L_var1 > 0 && L_var2 > 0) || (L_var1 < 0 && L_var2 < 0))
  {

    // an overflow is possible

    L_SumLow = (L_var1 & 0xffff) + (L_var2 & 0xffff);
    L_SumHigh = ((L_var1 >> 16) & 0xffff) + ((L_var2 >> 16) & 0xffff);
    if (L_SumLow & 0x10000)
    {
      // carry into high word is set
      L_SumHigh += 1;
    }

    // update sum only if there is an overflow or underflow
    //------------------------------------------------------

    if ((0x10000 & L_SumHigh) && !(0x8000 & L_SumHigh))
      L_Sum = LW_MIN;                  // underflow
    else if (!(0x10000 & L_SumHigh) && (0x8000 & L_SumHigh))
      L_Sum = LW_MAX;                  // overflow
  }

  return (L_Sum);

}
*/



/***************************************************************************
 *
 *   FUNCTION NAME: L_deposit_h
 *
 *   PURPOSE:
 *
 *     Put the 16 bit input into the 16 MSB's of the output INT32.  The
 *     LS 16 bits are zeroed.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff 0000.
 *
 *
 *   KEYWORDS: deposit, assign, fractional assign
 *
 *************************************************************************/

/*INT32 L_deposit_h(INT16 var1)
{
  INT32 L_var2;

  L_var2 = (INT32) var1 << 16;
  return (L_var2);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_deposit_l
 *
 *   PURPOSE:
 *
 *     Put the 16 bit input into the 16 LSB's of the output INT32 with
 *     sign extension i.e. the top 16 bits are set to either 0 or 0xffff.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0xffff 8000 <= L_var1 <= 0x0000 7fff.
 *
 *   KEYWORDS: deposit, assign
 *
 *************************************************************************/

/*INT32 L_deposit_l(INT16 var1)
{
  INT32 L_Out;

  L_Out = var1;
  return (L_Out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_mac
 *
 *   PURPOSE:
 *
 *     Multiply accumulate.  Fractionally multiply two 16 bit
 *     numbers together with saturation.  Add that result to the
 *     32 bit input with saturation.  Return the 32 bit result.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var3
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *     Fractionally multiply two 16 bit numbers together with
 *     saturation.  The only numbers which will cause saturation on
 *     the multiply are 0x8000 * 0x8000.
 *
 *     Add that result to the 32 bit input with saturation.
 *     Return the 32 bit result.
 *
 *     Please note that this is not a true multiply accumulate as
 *     most processors would implement it.  The 0x8000*0x8000
 *     causes and overflow for this instruction.  On most
 *     processors this would cause an overflow only if the 32 bit
 *     input added to it were positive or zero.
 *
 *   KEYWORDS: mac, multiply accumulate
 *
 *************************************************************************/

/*INT32 L_mac(INT32 L_var3, INT16 var1, INT16 var2)
{
  return (L_add(L_var3, L_MULT(var1, var2)));
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_msu
 *
 *   PURPOSE:
 *
 *     Multiply and subtract.  Fractionally multiply two 16 bit
 *     numbers together with saturation.  Subtract that result from
 *     the 32 bit input with saturation.  Return the 32 bit result.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var3
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *     Fractionally multiply two 16 bit numbers together with
 *     saturation.  The only numbers which will cause saturation on
 *     the multiply are 0x8000 * 0x8000.
 *
 *     Subtract that result from the 32 bit input with saturation.
 *     Return the 32 bit result.
 *
 *     Please note that this is not a true multiply accumulate as
 *     most processors would implement it.  The 0x8000*0x8000
 *     causes and overflow for this instruction.  On most
 *     processors this would cause an overflow only if the 32 bit
 *     input added to it were negative or zero.
 *
 *   KEYWORDS: mac, multiply accumulate, msu
 *
 *************************************************************************/

/*INT32 L_msu(INT32 L_var3, INT16 var1, INT16 var2)
{
  return (L_SUB(L_var3, (L_MULT(var1, var2))));
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_mult
 *
 *   PURPOSE:
 *
 *     Perform a fractional multipy of the two 16 bit input numbers
 *     with saturation.  Output a 32 bit number.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *     Multiply the two the two 16 bit input numbers. If the
 *     result is within this range, left shift the result by one
 *     and output the 32 bit number.  The only possible overflow
 *     occurs when var1==var2==-0x8000.  In this case output
 *     0x7fff ffff.
 *
 *   KEYWORDS: multiply, mult, mpy
 *
 *************************************************************************/

/*INT32 L_mult(INT16 var1, INT16 var2)
{
  INT32 L_product;

  if (var1 == SW_MIN && var2 == SW_MIN)
    L_product = LW_MAX;                // overflow
  else
  {
    L_product = (INT32) var1 *var2; // integer multiply

    L_product = L_product << 1;
  }
  return (L_product);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_negate
 *
 *   PURPOSE:
 *
 *     Negate the 32 bit input. 0x8000 0000's negated value is
 *     0x7fff ffff.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0001 <= L_var1 <= 0x7fff ffff.
 *
 *   KEYWORDS: negate, negative
 *
 *************************************************************************/

/*INT32 L_negate(INT32 L_var1)
{
  INT32 L_Out;

  if (L_var1 == LW_MIN)
    L_Out = LW_MAX;
  else
    L_Out = -L_var1;
  return (L_Out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_shift_r
 *
 *   PURPOSE:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.  This is just like shift_r above except that the
 *     input/output is 32 bits as opposed to 16.
 *
 *     if var2 is positve perform a arithmetic left shift
 *     with saturation (see L_shl() above).
 *
 *     If var2 is zero simply return L_var1.
 *
 *     If var2 is negative perform a arithmetic right shift (L_shr)
 *     of L_var1 by (-var2)+1.  Add the LS bit of the result to
 *     L_var1 shifted right (L_shr) by -var2.
 *
 *     Note that there is no constraint on var2, so if var2 is
 *     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
 *     This is the reason the L_shl function is used.
 *
 *
 *   KEYWORDS:
 *
 *************************************************************************/

/*INT32 L_shift_r(INT32 L_var1, INT16 var2)
{
  INT32 L_Out,
         L_rnd;

  if (var2 < -31)
  {
    L_Out = 0;
  }
  else if (var2 < 0)
  {
    // right shift
    L_rnd = L_shl(L_var1, var2 + 1) & 0x1;
    L_Out = L_ADD(L_shl(L_var1, var2), L_rnd);
  }
  else
    L_Out = L_shl(L_var1, var2);

  return (L_Out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_shl
 *
 *   PURPOSE:
 *
 *     Arithmetic shift left (or right).
 *     Arithmetically shift the input left by var2.   If var2 is
 *     negative then an arithmetic shift right (L_shr) of L_var1 by
 *     -var2 is performed.
 *
 *   INPUTS:
 *
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Arithmetically shift the 32 bit input left by var2.  This
 *     operation maintains the sign of the input number. If var2 is
 *     negative then an arithmetic shift right (L_shr) of L_var1 by
 *     -var2 is performed.  See description of L_shr for details.
 *
 *     Equivalent to the Full-Rate GSM ">> n" operation.  Note that
 *     ANSI-C does not guarantee operation of the C ">>" or "<<"
 *     operator for negative numbers.
 *
 *   KEYWORDS: shift, arithmetic shift left,
 *
 *************************************************************************/

/*INT32 L_shl(INT32 L_var1, INT16 var2)
{

  INT32 L_Mask,
         L_Out;
  int    i,
         iOverflow = 0;

  if (var2 == 0 || L_var1 == 0)
  {
    L_Out = L_var1;
  }
  else if (var2 < 0)
  {
    if (var2 <= -31)
    {
      if (L_var1 > 0)
        L_Out = 0;
      else
        L_Out = 0xffffffffL;
    }
    else
      //L_Out = L_shr(L_var1, -var2);
      L_Out = L_SHR_V(L_var1, -var2);

  }
  else
  {

    if (var2 >= 31)
      iOverflow = 1;

    else
    {

      if (L_var1 < 0)
        L_Mask = LW_SIGN;              // sign bit mask
      else
        L_Mask = 0x0;
      L_Out = L_var1;
      for (i = 0; i < var2 && !iOverflow; i++)
      {
        // check the sign bit
        L_Out = (L_Out & 0x7fffffffL) << 1;
        if ((L_Mask ^ L_Out) & LW_SIGN)
          iOverflow = 1;
      }
    }

    if (iOverflow)
    {
      // saturate
      if (L_var1 > 0)
        L_Out = LW_MAX;
      else
        L_Out = LW_MIN;
    }
  }

  return (L_Out);
}*/

/*INT32 L_shl (INT32 L_var1, INT16 var2)//EFR
{
    INT32 L_var_out;

    if (var2 <= 0)
    {
        L_var_out = L_shr (L_var1, -var2);
    }
    else
    {
        for (; var2 > 0; var2--)
        {
            if (L_var1 > (INT32) 0X3fffffffL)
            {
                //Overflow = 1;
                L_var_out = LW_MAX;
                break;
            }
            else
            {
                if (L_var1 < (INT32) 0xc0000000L)
                {
                    //Overflow = 1;
                    L_var_out = LW_MIN;
                    break;
                }
            }
            L_var1 *= 2;
            L_var_out = L_var1;
        }
    }

    return (L_var_out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_shr
 *
 *   PURPOSE:
 *
 *     Arithmetic shift right (or left).
 *     Arithmetically shift the input right by var2.   If var2 is
 *     negative then an arithmetic shift left (shl) of var1 by
 *     -var2 is performed.
 *
 *   INPUTS:
 *
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Arithmetically shift the input right by var2.  This
 *     operation maintains the sign of the input number. If var2 is
 *     negative then an arithmetic shift left (shl) of L_var1 by
 *     -var2 is performed.  See description of L_shl for details.
 *
 *     The input is a 32 bit number, as is the output.
 *
 *     Equivalent to the Full-Rate GSM ">> n" operation.  Note that
 *     ANSI-C does not guarantee operation of the C ">>" or "<<"
 *     operator for negative numbers.
 *
 *   KEYWORDS: shift, arithmetic shift right,
 *
 *************************************************************************/

/*INT32 L_shr(INT32 L_var1, INT16 var2)
{

  INT32 L_Mask,
         L_Out;

  if (var2 == 0 || L_var1 == 0)
  {
    L_Out = L_var1;
  }
  else if (var2 < 0)
  {
    // perform a left shift
    //----------------------
    if (var2 <= -31)
    {
      // saturate /
      if (L_var1 > 0)
        L_Out = LW_MAX;
      else
        L_Out = LW_MIN;
    }
    else
      L_Out = L_shl(L_var1, -var2);
  }
  else
  {

    if (var2 >= 31)
    {
      if (L_var1 > 0)
        L_Out = 0;
      else
        L_Out = 0xffffffffL;
    }
    else
    {
      L_Mask = 0;

      if (L_var1 < 0)
      {
        L_Mask = ~L_Mask << (32 - var2);
      }

      L_var1 >>= var2;
      L_Out = L_Mask | L_var1;
    }
  }
  return (L_Out);
}*/
/*INT32 L_shr (INT32 L_var1, INT16 var2)//EFR
{
    INT32 L_var_out;

    if (var2 < 0)
    {
        L_var_out = L_shl (L_var1, -var2);

    }
    else
    {
        if (var2 >= 31)
        {
            L_var_out = (L_var1 < 0L) ? -1 : 0;
        }
        else
        {
            if (L_var1 < 0)
            {
                L_var_out = ~((~L_var1) >> var2);
            }
            else
            {
                L_var_out = L_var1 >> var2;
            }
        }
    }

    return (L_var_out);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: L_sub
 *
 *   PURPOSE:
 *
 *     Perform the subtraction of the two 32 bit input variables with
 *     saturation.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *     L_var2
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform the subtraction of the two 32 bit input variables with
 *     saturation.
 *
 *     L_Out = L_var1 - L_var2
 *
 *     L_Out is set to 0x7fff ffff if the operation results in an
 *     overflow.  L_Out is set to 0x8000 0000 if the operation
 *     results in an underflow.
 *
 *   KEYWORDS: sub, subtraction
 *
 *************************************************************************/
/*INT32 L_sub(INT32 L_var1, INT32 L_var2)
{
  INT32 L_Sum;

  // check for overflow
  if ((L_var1 > 0 && L_var2 < 0) || (L_var1 < 0 && L_var2 > 0))
  {
    if (L_var2 == LW_MIN)
    {
      L_Sum = L_ADD(L_var1, LW_MAX);
      L_Sum = L_ADD(L_Sum, 1);
    }
    else
      L_Sum = L_ADD(L_var1, -L_var2);
  }
  else
  {                                    // no overflow possible
    L_Sum = L_var1 - L_var2;
  }
  return (L_Sum);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME:mac_r
 *
 *   PURPOSE:
 *
 *     Multiply accumulate and round.  Fractionally multiply two 16
 *     bit numbers together with saturation.  Add that result to
 *     the 32 bit input with saturation.  Finally round the result
 *     into a 16 bit number.
 *
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var3
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Fractionally multiply two 16 bit numbers together with
 *     saturation.  The only numbers which will cause saturation on
 *     the multiply are 0x8000 * 0x8000.
 *
 *     Add that result to the 32 bit input with saturation.
 *     Round the 32 bit result by adding 0x0000 8000 to the input.
 *     The result may overflow due to the add.  If so, the result
 *     is saturated.  The 32 bit rounded number is then shifted
 *     down 16 bits and returned as a INT16.
 *
 *     Please note that this is not a true multiply accumulate as
 *     most processors would implement it.  The 0x8000*0x8000
 *     causes and overflow for this instruction.  On most
 *     processors this would cause an overflow only if the 32 bit
 *     input added to it were positive or zero.
 *
 *   KEYWORDS: mac, multiply accumulate, macr
 *
 *************************************************************************/

/*INT16 mac_r(INT32 L_var3, INT16 var1, INT16 var2)
{
  //return (roundo(L_ADD(L_var3, (L_MULT(var1, var2)))));
  return (roundo(L_MAC(L_var3, var1, var2)));

}*/

/***************************************************************************
 *
 *   FUNCTION NAME:  msu_r
 *
 *   PURPOSE:
 *
 *     Multiply subtract and round.  Fractionally multiply two 16
 *     bit numbers together with saturation.  Subtract that result from
 *     the 32 bit input with saturation.  Finally round the result
 *     into a 16 bit number.
 *
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *     L_var3
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var2 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Fractionally multiply two 16 bit numbers together with
 *     saturation.  The only numbers which will cause saturation on
 *     the multiply are 0x8000 * 0x8000.
 *
 *     Subtract that result from the 32 bit input with saturation.
 *     Round the 32 bit result by adding 0x0000 8000 to the input.
 *     The result may overflow due to the add.  If so, the result
 *     is saturated.  The 32 bit rounded number is then shifted
 *     down 16 bits and returned as a INT16.
 *
 *     Please note that this is not a true multiply accumulate as
 *     most processors would implement it.  The 0x8000*0x8000
 *     causes and overflow for this instruction.  On most
 *     processors this would cause an overflow only if the 32 bit
 *     input added to it were positive or zero.
 *
 *   KEYWORDS: mac, multiply accumulate, macr
 *
 *************************************************************************/

/*INT16 msu_r(INT32 L_var3, INT16 var1, INT16 var2)
{
  //return (roundo(L_SUB(L_var3, (L_MULT(var1, var2)))));
  return (ROUNDO(L_MSU(L_var3, var1, var2)));

}*/

/***************************************************************************
 *
 *   FUNCTION NAME: mult
 *
 *   PURPOSE:
 *
 *     Perform a fractional multipy of the two 16 bit input numbers
 *     with saturation and truncation.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform a fractional multipy of the two 16 bit input
 *     numbers.  If var1 == var2 == -0x8000, output 0x7fff.
 *     Otherwise output var1*var2 >> 15.  The output is a
 *     16 bit number.
 *
 *   KEYWORDS: mult, mulitply, mpy
 *
 *************************************************************************/

/*INT16 mult(INT16 var1, INT16 var2)
{
  INT32 L_product;
  INT16 swOut;

  L_product = L_MULT(var1, var2);
  swOut = EXTRACT_H(L_product);
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: mult_r
 *
 *   PURPOSE:
 *
 *     Perform a fractional multipy and round of the two 16 bit
 *     input numbers with saturation.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     This routine is defined as the concatenation of the multiply
 *     operation and the round operation.
 *
 *     The fractional multiply (L_mult) produces a saturated 32 bit
 *     output.  This is followed by a an add of 0x0000 8000 to the
 *     32 bit result.  The result may overflow due to the add.  If
 *     so, the result is saturated.  The 32 bit rounded number is
 *     then shifted down 16 bits and returned as a INT16.
 *
 *
 *   KEYWORDS: multiply and round, round, mult_r, mpyr
 *
 *************************************************************************/


/*INT16 mult_r(INT16 var1, INT16 var2)
{
  INT16 swOut;

  swOut = ROUNDO(L_MULT(var1, var2));
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: negate
 *
 *   PURPOSE:
 *
 *     Negate the 16 bit input. 0x8000's negated value is 0x7fff.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8001 <= swOut <= 0x0000 7fff.
 *
 *   KEYWORDS: negate, negative, invert
 *
 *************************************************************************/

/*INT16 negate(INT16 var1)
{
  INT16 swOut;

  if (var1 == SW_MIN)
    swOut = SW_MAX;
  else
    swOut = -var1;
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: norm_l
 *
 *   PURPOSE:
 *
 *     Get normalize shift count:
 *
 *     A 32 bit number is input (possiblly unnormalized).  Output
 *     the positive (or zero) shift count required to normalize the
 *     input.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0 <= swOut <= 31
 *
 *
 *
 *   IMPLEMENTATION:
 *
 *     Get normalize shift count:
 *
 *     A 32 bit number is input (possiblly unnormalized).  Output
 *     the positive (or zero) shift count required to normalize the
 *     input.
 *
 *     If zero in input, return 0 as the shift count.
 *
 *     For non-zero numbers, count the number of left shift
 *     required to get the number to fall into the range:
 *
 *     0x4000 0000 >= normlzd number >= 0x7fff ffff (positive number)
 *     or
 *     0x8000 0000 <= normlzd number < 0xc000 0000 (negative number)
 *
 *     Return the number of shifts.
 *
 *     This instruction corresponds exactly to the Full-Rate "norm"
 *     instruction.
 *
 *   KEYWORDS: norm, normalization
 *
 *************************************************************************/

/*INT16 norm_l(INT32 L_var1)
{

  INT16 swShiftCnt;

  if (L_var1 != 0)
  {
    if (!(L_var1 & LW_SIGN))
    {

      // positive input
      for (swShiftCnt = 0; !(L_var1 <= LW_MAX && L_var1 >= 0x40000000L);
           swShiftCnt++)
      {
        L_var1 = L_var1 << 1;
      }

    }
    else
    {
      // negative input
      for (swShiftCnt = 0;
           !(L_var1 >= LW_MIN && L_var1 < (INT32) 0xc0000000L);
           swShiftCnt++)
      {
        L_var1 = L_var1 << 1;
      }
    }
  }
  else
  {
    swShiftCnt = 0;
  }
  return (swShiftCnt);
}*/
INT16 norm_l (INT32 L_var1)
{
    INT16 var_out;

    if (L_var1 == 0)
    {
        var_out = 0;
    }
    else
    {
        if (L_var1 == (INT32) 0xffffffffL)
        {
            var_out = 31;
        }
        else
        {
            if (L_var1 < 0)
            {
                L_var1 = ~L_var1;
            }
            for (var_out = 0; L_var1 < (INT32) 0x40000000L; var_out++)
            {
                L_var1 <<= 1;
            }
        }
    }

    return (var_out);
}
/***************************************************************************
 *
 *   FUNCTION NAME: norm_s
 *
 *   PURPOSE:
 *
 *     Get normalize shift count:
 *
 *     A 16 bit number is input (possiblly unnormalized).  Output
 *     the positive (or zero) shift count required to normalize the
 *     input.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0 <= swOut <= 15
 *
 *
 *
 *   IMPLEMENTATION:
 *
 *     Get normalize shift count:
 *
 *     A 16 bit number is input (possiblly unnormalized).  Output
 *     the positive (or zero) shift count required to normalize the
 *     input.
 *
 *     If zero in input, return 0 as the shift count.
 *
 *     For non-zero numbers, count the number of left shift
 *     required to get the number to fall into the range:
 *
 *     0x4000 >= normlzd number >= 0x7fff (positive number)
 *     or
 *     0x8000 <= normlzd number <  0xc000 (negative number)
 *
 *     Return the number of shifts.
 *
 *     This instruction corresponds exactly to the Full-Rate "norm"
 *     instruction.
 *
 *   KEYWORDS: norm, normalization
 *
 *************************************************************************/

INT16 norm_s(INT16 var1)
{

    short  swShiftCnt;
    INT32 L_var1;

    L_var1 = L_DEPOSIT_H(var1);
    swShiftCnt = norm_l(L_var1);
    return (swShiftCnt);
}

/***************************************************************************
 *
 *   FUNCTION NAME: round
 *
 *   PURPOSE:
 *
 *     Round the 32 bit INT32 into a 16 bit INT16 with saturation.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     32 bit long signed integer (INT32) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform a two's complement round on the input INT32 with
 *     saturation.
 *
 *     This is equivalent to adding 0x0000 8000 to the input.  The
 *     result may overflow due to the add.  If so, the result is
 *     saturated.  The 32 bit rounded number is then shifted down
 *     16 bits and returned as a INT16.
 *
 *
 *   KEYWORDS: round
 *
 *************************************************************************/

/*INT16 roundo(INT32 L_var1)
{
  INT32 L_Prod;

  L_Prod = L_ADD(L_var1, 0x00008000L); // round MSP
  return (EXTRACT_H(L_Prod));
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: shift_r
 *
 *   PURPOSE:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *
 *   IMPLEMENTATION:
 *
 *     Shift and round.  Perform a shift right. After shifting, use
 *     the last bit shifted out of the LSB to round the result up
 *     or down.
 *
 *     If var2 is positive perform a arithmetic left shift
 *     with saturation (see shl() above).
 *
 *     If var2 is zero simply return var1.
 *
 *     If var2 is negative perform a arithmetic right shift (shr)
 *     of var1 by (-var2)+1.  Add the LS bit of the result to var1
 *     shifted right (shr) by -var2.
 *
 *     Note that there is no constraint on var2, so if var2 is
 *     -0xffff 8000 then -var2 is 0x0000 8000, not 0x0000 7fff.
 *     This is the reason the shl function is used.
 *
 *
 *   KEYWORDS:
 *
 *************************************************************************/

/*INT16 shift_r(INT16 var1, INT16 var2)
{
  INT16 swOut,
         swRnd;

  if (var2 >= 0)
    swOut = shl(var1, var2);
  else
  {
    // right shift
    if (var2 < -15)
    {
      swOut = 0;
    }
    else
    {
      swRnd = shl(var1, var2 + 1) & 0x1;
      //swOut = ADD_SAT16(shl(var1, var2), swRnd);
      swOut = ADD(shl(var1, var2), swRnd);
    }
  }
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: shl
 *
 *   PURPOSE:
 *
 *     Arithmetically shift the input left by var2.
 *
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     If Arithmetically shift the input left by var2.  If var2 is
 *     negative then an arithmetic shift right (shr) of var1 by
 *     -var2 is performed.  See description of shr for details.
 *     When an arithmetic shift left is performed the var2 LS bits
 *     are zero filled.
 *
 *     The only exception is if the left shift causes an overflow
 *     or underflow.  In this case the LS bits are not modified.
 *     The number returned is 0x8000 in the case of an underflow or
 *     0x7fff in the case of an overflow.
 *
 *     The shl is equivalent to the Full-Rate GSM "<< n" operation.
 *     Note that ANSI-C does not guarantee operation of the C ">>"
 *     or "<<" operator for negative numbers - it is not specified
 *     whether this shift is an arithmetic or logical shift.
 *
 *   KEYWORDS: asl, arithmetic shift left, shift
 *
 *************************************************************************/

/*INT16 shl(INT16 var1, INT16 var2)
{
  INT16 swOut;
  INT32 L_Out;
  if (var2 == 0 || var1 == 0)
  {
    swOut = var1;
  }
  else if (var2 < 0)
  {
    // perform a right shift
    //-----------------------
    if (var2 <= -15)
    {
      if (var1 < 0)
        swOut = (INT16) 0xffff;
      else
        swOut = 0x0;
    }
    else
      //swOut = shr(var1, -var2);
      swOut = SHR(var1, -var2);
  }
  else
  {
    // var2 > 0
    if (var2 >= 15)
    {
      if (var1 > 0)// saturate
        swOut = SW_MAX;
      else
        swOut = SW_MIN;
    }
    else
    {
      L_Out = (INT32) var1 *(1 << var2);
      swOut = (INT16) L_Out;       // copy low portion to swOut, overflow could have hpnd
      if (swOut != L_Out)
      {
        if (var1 > 0)
          swOut = SW_MAX;              // saturate
        else
          swOut = SW_MIN;              // saturate
      }
    }
  }
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: shr
 *
 *   PURPOSE:
 *
 *     Arithmetic shift right (or left).
 *     Arithmetically shift the input right by var2.   If var2 is
 *     negative then an arithmetic shift left (shl) of var1 by
 *     -var2 is performed.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Arithmetically shift the input right by var2.  This
 *     operation maintains the sign of the input number. If var2 is
 *     negative then an arithmetic shift left (shl) of var1 by
 *     -var2 is performed.  See description of shl for details.
 *
 *     Equivalent to the Full-Rate GSM ">> n" operation.  Note that
 *     ANSI-C does not guarantee operation of the C ">>" or "<<"
 *     operator for negative numbers.
 *
 *   KEYWORDS: shift, arithmetic shift right,
 *
 *************************************************************************/

/*INT16 shr(INT16 var1, INT16 var2)
{

  INT16 swMask,
         swOut;

  if (var2 == 0 || var1 == 0)
    swOut = var1;

  else if (var2 < 0)
  {
    // perform an arithmetic left shift
    //----------------------------------
    if (var2 <= -15)
    {
      // saturate
      if (var1 > 0)
        swOut = SW_MAX;
      else
        swOut = SW_MIN;
    }
    else
      swOut = SHL_gle(var1, -var2);
  }
  else
  {
    // positive shift count
    //----------------------
    if (var2 >= 15)
    {
      if (var1 < 0)
        swOut = (INT16) 0xffff;
      else
        swOut = 0x0;
    }
    else
    {
      // take care of sign extension
      //-----------------------------
      swMask = 0;
      if (var1 < 0)
      {
        swMask = ~swMask << (16 - var2);
      }
      var1 >>= var2;
      swOut = swMask | var1;

    }
  }
  return (swOut);
}*/

/***************************************************************************
 *
 *   FUNCTION NAME: sub
 *
 *   PURPOSE:
 *
 *     Perform the subtraction of the two 16 bit input variable with
 *     saturation.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range 0xffff 8000 <= var2 <= 0x0000 7fff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swOut
 *                     16 bit short signed integer (INT16) whose value
 *                     falls in the range
 *                     0xffff 8000 <= swOut <= 0x0000 7fff.
 *
 *   IMPLEMENTATION:
 *
 *     Perform the subtraction of the two 16 bit input variable with
 *     saturation.
 *
 *     swOut = var1 - var2
 *
 *     swOut is set to 0x7fff if the operation results in an
 *     overflow.  swOut is set to 0x8000 if the operation results
 *     in an underflow.
 *
 *   KEYWORDS: sub, subtraction
 *
 *************************************************************************/
/*INT16 sub(INT16 var1, INT16 var2)
{
  INT32 L_diff;
  INT16 swOut;

  L_diff = (INT32) var1 - var2;
  swOut = saturate(L_diff);

  return (swOut);
}*/
