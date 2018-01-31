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







BYTE Map99_Coin;




void Map99_Init()
{

    MapperInit = Map99_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map99_Apu;


    MapperReadApu = Map99_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    if ( NesHeader.byRomSize > 1 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 2 );
        ROMBANK3 = ROMPAGE( 3 );
    }
    else if ( NesHeader.byRomSize > 0 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 0 );
        ROMBANK3 = ROMPAGE( 1 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 0 );
        ROMBANK2 = ROMPAGE( 0 );
        ROMBANK3 = ROMPAGE( 0 );
    }


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }

    Map99_Coin = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map99_Apu( WORD wAddr, BYTE byData )
{
    if( wAddr == 0x4016 )
    {
        if( byData & 0x04 )
        {
            PPUBANK[ 0 ] = VROMPAGE(  8 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE(  9 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 10 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 11 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 12 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 13 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 14 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 15 % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else
        {
            PPUBANK[ 0 ] = VROMPAGE( 0 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 1 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 2 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 3 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 4 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 5 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 6 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 7 % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }

    if( wAddr == 0x4020 )
    {
        Map99_Coin = byData;
    }
}




BYTE Map99_ReadApu( WORD wAddr )
{
    if( wAddr == 0x4020 )
    {
        return Map99_Coin;
    }
    return ( wAddr >> 8 );
}
