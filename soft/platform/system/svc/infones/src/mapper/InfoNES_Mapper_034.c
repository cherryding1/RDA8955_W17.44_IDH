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










void Map34_Init()
{

    MapperInit = Map34_Init;


    MapperWrite = Map34_Write;


    MapperSram = Map34_Sram;


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




void Map34_Write( WORD wAddr, BYTE byData )
{

    byData <<= 2;
    byData %= ( NesHeader.byRomSize << 1);

    ROMBANK0 = ROMPAGE( byData );
    ROMBANK1 = ROMPAGE( byData + 1 );
    ROMBANK2 = ROMPAGE( byData + 2 );
    ROMBANK3 = ROMPAGE( byData + 3 );
}




void Map34_Sram( WORD wAddr, BYTE byData )
{
    switch(wAddr)
    {

        case 0x7ffd:
            byData <<= 2;
            byData %= ( NesHeader.byRomSize << 1 );

            ROMBANK0 = ROMPAGE( byData );
            ROMBANK1 = ROMPAGE( byData + 1 );
            ROMBANK2 = ROMPAGE( byData + 2 );
            ROMBANK3 = ROMPAGE( byData + 3 );
            break;


        case 0x7ffe:
            byData <<= 2;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 0 ] = VROMPAGE( byData );
            PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
            PPUBANK[ 2 ] = VROMPAGE( byData + 2 );
            PPUBANK[ 3 ] = VROMPAGE( byData + 3 );
            InfoNES_SetupChr();
            break;


        case 0x7fff:
            byData <<= 2;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 4 ] = VROMPAGE( byData );
            PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
            PPUBANK[ 6 ] = VROMPAGE( byData + 2 );
            PPUBANK[ 7 ] = VROMPAGE( byData + 3 );
            InfoNES_SetupChr();
            break;
    }
}
