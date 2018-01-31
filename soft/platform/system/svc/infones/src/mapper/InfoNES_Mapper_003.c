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










void Map3_Init()
{
    int nPage;


    MapperInit = Map3_Init;


    MapperWrite = Map3_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    if ( ( NesHeader.byRomSize << 1 ) > 2 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 2 );
        ROMBANK3 = ROMPAGE( 3 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 0 );
        ROMBANK3 = ROMPAGE( 1 );
    }


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( nPage = 0; nPage < 8; ++nPage )
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }



    K6502_Set_Int_Wiring( 1, 1 );
}




void Map3_Write( WORD wAddr, BYTE byData )
{
    DWORD dwBase;


    byData %= NesHeader.byVRomSize;
    dwBase = ( (DWORD)byData ) << 3;

    PPUBANK[ 0 ] = VROMPAGE( dwBase + 0 );
    PPUBANK[ 1 ] = VROMPAGE( dwBase + 1 );
    PPUBANK[ 2 ] = VROMPAGE( dwBase + 2 );
    PPUBANK[ 3 ] = VROMPAGE( dwBase + 3 );
    PPUBANK[ 4 ] = VROMPAGE( dwBase + 4 );
    PPUBANK[ 5 ] = VROMPAGE( dwBase + 5 );
    PPUBANK[ 6 ] = VROMPAGE( dwBase + 6 );
    PPUBANK[ 7 ] = VROMPAGE( dwBase + 7 );

    InfoNES_SetupChr();
}
