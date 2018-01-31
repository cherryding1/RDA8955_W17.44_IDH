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










void Map77_Init()
{

    MapperInit = Map77_Init;


    MapperWrite = Map77_Write;


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


    byVramWriteEnable = 1;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map77_Write( WORD wAddr, BYTE byData )
{
    BYTE byRomBank = byData & 0x07;
    BYTE byChrBank = ( byData & 0xf0 ) >> 4;


    byRomBank <<= 2;
    byRomBank %= ( NesHeader.byRomSize << 1 );

    ROMBANK0 = ROMPAGE( byRomBank );
    ROMBANK1 = ROMPAGE( byRomBank + 1 );
    ROMBANK2 = ROMPAGE( byRomBank + 2 );
    ROMBANK3 = ROMPAGE( byRomBank + 3 );


    byChrBank <<= 1;
    byChrBank %= ( NesHeader.byVRomSize << 3 );

    PPUBANK[ 0 ] = VROMPAGE( byChrBank );
    PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
    InfoNES_SetupChr();
}
