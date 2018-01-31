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







BYTE Map185_Dummy_Chr_Rom[ 0x400 ];




void Map185_Init()
{

    MapperInit = Map185_Init;


    MapperWrite = Map185_Write;


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


    for ( int nPage = 0; nPage < 0x400; nPage++ )
    {
        Map185_Dummy_Chr_Rom[ nPage ] = 0xff;
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map185_Write( WORD wAddr, BYTE byData )
{

    if ( byData & 0x03 )
    {
        PPUBANK[ 0 ] = VROMPAGE( 0 );
        PPUBANK[ 1 ] = VROMPAGE( 1 );
        PPUBANK[ 2 ] = VROMPAGE( 2 );
        PPUBANK[ 3 ] = VROMPAGE( 3 );
        PPUBANK[ 4 ] = VROMPAGE( 4 );
        PPUBANK[ 5 ] = VROMPAGE( 5 );
        PPUBANK[ 6 ] = VROMPAGE( 6 );
        PPUBANK[ 7 ] = VROMPAGE( 7 );
        InfoNES_SetupChr();
    }
    else
    {
        PPUBANK[ 0 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 1 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 2 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 3 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 4 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 5 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 6 ] = Map185_Dummy_Chr_Rom;
        PPUBANK[ 7 ] = Map185_Dummy_Chr_Rom;
        InfoNES_SetupChr();
    }
}
