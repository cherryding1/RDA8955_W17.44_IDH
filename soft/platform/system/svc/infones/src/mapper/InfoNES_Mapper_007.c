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










void Map7_Init()
{

    MapperInit = Map7_Init;


    MapperWrite = Map7_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map7_Write( WORD wAddr, BYTE byData )
{
    BYTE byBank;


    byBank = ( byData & 0x07 ) << 2;
    byBank %= ( NesHeader.byRomSize << 1 );

    ROMBANK0 = ROMPAGE( byBank );
    ROMBANK1 = ROMPAGE( byBank + 1 );
    ROMBANK2 = ROMPAGE( byBank + 2 );
    ROMBANK3 = ROMPAGE( byBank + 3 );


    InfoNES_Mirroring( byData & 0x10 ? 2 : 3 );
}
