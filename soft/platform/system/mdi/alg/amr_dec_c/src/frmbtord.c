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





#include "frmbtord.tab"

short* table0__7[8] ;

/*
 *  Function        :   InitFrameBitOrder
 *--------------------------------------------------------------------------------
 *  Purpose         :   Inititate table0~table7 of reordering
 *  Entry           :
 *
 *  Return          :   void
 *  External Ref.   :
 *  Summary         :
 *  Flowchart       :
 *--------------------------------------------------------------------------------
 *  Note            :
 *--------------------------------------------------------------------------------
 */

void InitFrameBitOrder()
{
    int i ;
    for( i = 0 ; i < 16; i  ++ )
        bit_Byte_Number[i][1] = (bit_Byte_Number[i][0]+7)/8 ;

    table0__7[0] =(short*) BitOrderOfMR475 ;
    table0__7[1] =(short*) BitOrderOfMR515 ;
    table0__7[2] =(short*) BitOrderOfMR590 ;
    table0__7[3] =(short*) BitOrderOfMR670 ;
    table0__7[4] =(short*) BitOrderOfMR740 ;
    table0__7[5] =(short*) BitOrderOfMR795 ;
    table0__7[6] =(short*) BitOrderOfMR1020 ;
    table0__7[7] =(short*) BitOrderOfMR1220 ;
}






