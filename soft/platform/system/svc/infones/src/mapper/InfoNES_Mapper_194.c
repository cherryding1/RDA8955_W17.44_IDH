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










void Map194_Init()
{

    MapperInit = Map194_Init;


    MapperWrite = Map194_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( (NesHeader.byRomSize<<1) - 4 );
    ROMBANK1 = ROMPAGE( (NesHeader.byRomSize<<1) - 3 );
    ROMBANK2 = ROMPAGE( (NesHeader.byRomSize<<1) - 2 );
    ROMBANK3 = ROMPAGE( (NesHeader.byRomSize<<1) - 1 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map194_Write( WORD wAddr, BYTE byData )
{
    SRAMBANK = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
}
