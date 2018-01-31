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










void Map72_Init()
{

    MapperInit = Map72_Init;


    MapperWrite = Map72_Write;


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
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map72_Write( WORD wAddr, BYTE byData )
{
    BYTE byBank = byData & 0x0f;

    if ( byData & 0x80 )
    {

        byBank <<= 1;
        byBank %= ( NesHeader.byRomSize << 1 );
        ROMBANK0 = ROMPAGE( byBank );
        ROMBANK1 = ROMPAGE( byBank + 1 );
    }
    else if ( byData & 0x40 )
    {

        byBank <<= 3;
        byBank %= ( NesHeader.byVRomSize << 3 );
        PPUBANK[ 0 ] = VROMPAGE( byBank );
        PPUBANK[ 1 ] = VROMPAGE( byBank + 1 );
        PPUBANK[ 2 ] = VROMPAGE( byBank + 2 );
        PPUBANK[ 3 ] = VROMPAGE( byBank + 3 );
        PPUBANK[ 4 ] = VROMPAGE( byBank + 4 );
        PPUBANK[ 5 ] = VROMPAGE( byBank + 5 );
        PPUBANK[ 6 ] = VROMPAGE( byBank + 6 );
        PPUBANK[ 7 ] = VROMPAGE( byBank + 7 );
        InfoNES_SetupChr();
    }
}
