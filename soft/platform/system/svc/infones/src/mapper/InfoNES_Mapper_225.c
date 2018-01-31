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










void Map225_Init()
{

    MapperInit = Map225_Init;


    MapperWrite = Map225_Write;


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


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map225_Write( WORD wAddr, BYTE byData )
{
    BYTE byPrgBank = (wAddr & 0x0F80) >> 7;
    BYTE byChrBank = wAddr & 0x003F;

    PPUBANK[ 0 ] = VROMPAGE(((byChrBank<<3)+0) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE(((byChrBank<<3)+1) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE(((byChrBank<<3)+2) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE(((byChrBank<<3)+3) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE(((byChrBank<<3)+4) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE(((byChrBank<<3)+5) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE(((byChrBank<<3)+6) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE(((byChrBank<<3)+7) % ( NesHeader.byVRomSize << 3 ) );
    InfoNES_SetupChr();

    if( wAddr & 0x2000 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }

    if( wAddr & 0x1000 )
    {
        if( wAddr & 0x0040 )
        {
            ROMBANK0 = ROMPAGE(((byPrgBank<<2)+2) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE(((byPrgBank<<2)+3) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK2 = ROMPAGE(((byPrgBank<<2)+2) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK3 = ROMPAGE(((byPrgBank<<2)+3) % ( NesHeader.byRomSize << 1 ) );
        }
        else
        {
            ROMBANK0 = ROMPAGE(((byPrgBank<<2)+0) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE(((byPrgBank<<2)+1) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK2 = ROMPAGE(((byPrgBank<<2)+0) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK3 = ROMPAGE(((byPrgBank<<2)+1) % ( NesHeader.byRomSize << 1 ) );
        }
    }
    else
    {
        ROMBANK0 = ROMPAGE(((byPrgBank<<2)+0) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK1 = ROMPAGE(((byPrgBank<<2)+1) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK2 = ROMPAGE(((byPrgBank<<2)+2) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK3 = ROMPAGE(((byPrgBank<<2)+3) % ( NesHeader.byRomSize << 1 ) );
    }
}
