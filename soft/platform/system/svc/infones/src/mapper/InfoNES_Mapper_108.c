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










void Map108_Init()
{

    MapperInit = Map108_Init;


    MapperWrite = Map108_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = ROMPAGE( 0 );


    ROMBANK0 = ROMPAGE( 0xC % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( 0xD % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( 0xE % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( 0xF % ( NesHeader.byRomSize << 1 ) );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map108_Write( WORD wAddr, BYTE byData )
{

    SRAMBANK = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
}
