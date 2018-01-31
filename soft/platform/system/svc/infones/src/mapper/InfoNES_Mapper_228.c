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










void Map228_Init()
{

    MapperInit = Map228_Init;


    MapperWrite = Map228_Write;


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




void Map228_Write( WORD wAddr, BYTE byData )
{
    BYTE  byPrg = (wAddr&0x0780)>>7;

    switch( (wAddr&0x1800)>>11 )
    {
        case  1:
            byPrg |= 0x10;
            break;
        case  3:
            byPrg |= 0x20;
            break;
    }

    if( wAddr & 0x0020 )
    {
        byPrg <<= 1;
        if( wAddr & 0x0040 )
        {
            byPrg++;
        }
        ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
    }
    else
    {
        ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (NesHeader.byRomSize<<1));
    }

    BYTE byChr = ((wAddr&0x000F)<<2)|(byData&0x03);

    PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (NesHeader.byVRomSize<<3));
    InfoNES_SetupChr();

    if( wAddr & 0x2000 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }
}
