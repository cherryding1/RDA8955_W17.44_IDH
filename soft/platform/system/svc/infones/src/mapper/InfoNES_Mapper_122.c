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










void Map122_Init()
{

    MapperInit = Map122_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map122_Sram;


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




void Map122_Sram( WORD wAddr, BYTE byData )
{
    if ( wAddr == 0x6000 )
    {
        BYTE byChrBank0 = byData & 0x07;
        BYTE byChrBank1 = ( byData & 0x70 ) >> 4;

        byChrBank0 = ( byChrBank0 << 2 ) % ( NesHeader.byVRomSize << 3 );
        byChrBank1 = ( byChrBank1 << 2 ) % ( NesHeader.byVRomSize << 3 );

        PPUBANK[ 0 ] = VROMPAGE( byChrBank0 + 0 );
        PPUBANK[ 1 ] = VROMPAGE( byChrBank0 + 1 );
        PPUBANK[ 2 ] = VROMPAGE( byChrBank0 + 2 );
        PPUBANK[ 3 ] = VROMPAGE( byChrBank0 + 3 );
        PPUBANK[ 4 ] = VROMPAGE( byChrBank1 + 0 );
        PPUBANK[ 5 ] = VROMPAGE( byChrBank1 + 1 );
        PPUBANK[ 6 ] = VROMPAGE( byChrBank1 + 2 );
        PPUBANK[ 7 ] = VROMPAGE( byChrBank1 + 3 );
        InfoNES_SetupChr();
    }
}
