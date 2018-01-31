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










void Map62_Init()
{

    MapperInit = Map62_Init;


    MapperWrite = Map62_Write;


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




void Map62_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr & 0xFF00 )
    {
        case  0x8100:
            ROMBANK0 = ROMPAGE((byData+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((byData+1) % (NesHeader.byRomSize<<1));
            break;
        case  0x8500:
            ROMBANK0 = ROMPAGE(byData % (NesHeader.byRomSize<<1));
            break;
        case  0x8700:
            ROMBANK1 = ROMPAGE(byData % (NesHeader.byRomSize<<1));
            break;

        default:
            PPUBANK[ 0 ] = VROMPAGE((byData+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE((byData+1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE((byData+2) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE((byData+3) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE((byData+4) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE((byData+5) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE((byData+6) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE((byData+7) % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
    }
}
