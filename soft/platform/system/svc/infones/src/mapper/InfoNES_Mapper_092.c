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










void Map92_Init()
{

    MapperInit = Map92_Init;


    MapperWrite = Map92_Write;


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




void Map92_Write( WORD wAddr, BYTE byData )
{
    BYTE byRomBank;
    BYTE byChrBank;

    byData = (BYTE)wAddr & 0xff;
    byRomBank = ( byData & 0x0f ) << 1;
    byChrBank = byData & 0x0f;

    if ( wAddr >= 0x9000 )
    {
        switch ( byData & 0xf0 )
        {

            case 0xd0:
                byRomBank %= ( NesHeader.byRomSize << 1 );
                ROMBANK2 = ROMPAGE( byRomBank );
                ROMBANK3 = ROMPAGE( byRomBank + 1 );
                break;


            case 0xe0:
                byChrBank <<= 3;
                byChrBank %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 0 ] = VROMPAGE( byChrBank );
                PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
                PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
                PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
                PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
                PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
                PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
                PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
                InfoNES_SetupChr();
                break;
        }
    }
    else
    {
        switch ( byData & 0xf0 )
        {

            case 0xb0:
                byRomBank %= ( NesHeader.byRomSize << 1 );
                ROMBANK2 = ROMPAGE( byRomBank );
                ROMBANK3 = ROMPAGE( byRomBank + 1 );
                break;


            case 0x70:
                byChrBank <<= 3;
                byChrBank %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 0 ] = VROMPAGE( byChrBank );
                PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
                PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
                PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
                PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
                PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
                PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
                PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
                InfoNES_SetupChr();
                break;
        }
    }
}
