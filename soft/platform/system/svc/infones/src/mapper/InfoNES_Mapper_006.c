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







BYTE Map6_IRQ_Enable;
DWORD Map6_IRQ_Cnt;
BYTE Map6_Chr_Ram[ 0x2000 * 4 ];




void Map6_Init()
{
    int nPage;


    MapperInit = Map6_Init;


    MapperWrite = Map6_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map6_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map6_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 14 );
    ROMBANK3 = ROMPAGE( 15 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( nPage = 0; nPage < 8; ++nPage )
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }
    else
    {
        for ( nPage = 0; nPage < 8; ++nPage )
        {
            PPUBANK[ nPage ] = Map6_VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map6_Apu( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {

        case 0x42fe:
            if ( byData & 0x10 )
            {
                InfoNES_Mirroring( 2 );
            }
            else
            {
                InfoNES_Mirroring( 3 );
            }
            break;

        case 0x42ff:
            if ( byData & 0x10 )
            {
                InfoNES_Mirroring( 0 );
            }
            else
            {
                InfoNES_Mirroring( 1 );
            }
            break;

        case 0x4501:
            Map6_IRQ_Enable = 0;
            break;

        case 0x4502:
            Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0xff00 ) | (DWORD)byData;
            break;

        case 0x4503:
            Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0x00ff ) | ( (DWORD)byData << 8 );
            Map6_IRQ_Enable = 1;
            break;
    }
}




void Map6_Write( WORD wAddr, BYTE byData )
{
    BYTE byPrgBank = ( byData & 0x3c ) >> 2;
    BYTE byChrBank = byData & 0x03;


    byPrgBank <<= 1;
    byPrgBank %= ( NesHeader.byRomSize << 1 );

    ROMBANK0 = ROMPAGE( byPrgBank );
    ROMBANK1 = ROMPAGE( byPrgBank + 1 );


    PPUBANK[ 0 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 0 * 0x400 ];
    PPUBANK[ 1 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 1 * 0x400 ];
    PPUBANK[ 2 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 2 * 0x400 ];
    PPUBANK[ 3 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 3 * 0x400 ];
    PPUBANK[ 4 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 4 * 0x400 ];
    PPUBANK[ 5 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 5 * 0x400 ];
    PPUBANK[ 6 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 6 * 0x400 ];
    PPUBANK[ 7 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 7 * 0x400 ];
    InfoNES_SetupChr();
}




void Map6_HSync()
{
    if ( Map6_IRQ_Enable )
    {
        Map6_IRQ_Cnt += 133;
        if ( Map6_IRQ_Cnt >= 0xffff )
        {
            IRQ_REQ;
            Map6_IRQ_Cnt = 0;
        }
    }
}
