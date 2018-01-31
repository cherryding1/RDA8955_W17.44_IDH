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







BYTE Map117_IRQ_Line;
BYTE Map117_IRQ_Enable1;
BYTE Map117_IRQ_Enable2;




void Map117_Init()
{

    MapperInit = Map117_Init;


    MapperWrite = Map117_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map117_HSync;


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


    Map117_IRQ_Line = 0;
    Map117_IRQ_Enable1 = 0;
    Map117_IRQ_Enable2 = 1;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map117_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {


        case 0x8000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            break;

        case 0x8001:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0x8002:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK2 = ROMPAGE( byData );
            break;


        case 0xA000:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 0 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA001:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 1 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA002:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 2 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA003:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 3 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA004:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 4 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA005:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 5 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA006:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 6 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xA007:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 7 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xc001:
        case 0xc002:
        case 0xc003:
            Map117_IRQ_Enable1 = Map117_IRQ_Line = byData;
            break;

        case 0xe000:
            Map117_IRQ_Enable2 = byData & 0x01;
            break;
    }
}




void Map117_HSync()
{
    if ( Map117_IRQ_Enable1 && Map117_IRQ_Enable2 )
    {
        if ( Map117_IRQ_Line == PPU_Scanline )
        {
            Map117_IRQ_Enable1 = 0x00;
            IRQ_REQ;
        }
    }
}
