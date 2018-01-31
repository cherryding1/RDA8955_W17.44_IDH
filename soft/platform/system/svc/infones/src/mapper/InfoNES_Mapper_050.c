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







BYTE Map50_IRQ_Enable;




void Map50_Init()
{

    MapperInit = Map50_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map50_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map50_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = ROMPAGE( 15 % ( NesHeader.byRomSize << 1 ) );


    ROMBANK0 = ROMPAGE( 8 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( 9 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( 0 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( 11 % ( NesHeader.byRomSize << 1 ) );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }


    Map50_IRQ_Enable = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map50_Apu( WORD wAddr, BYTE byData )
{
    if ( ( wAddr & 0xE060 ) == 0x4020 )
    {
        if( wAddr & 0x0100 )
        {
            Map50_IRQ_Enable = byData & 0x01;
        }
        else
        {
            BYTE byDummy;

            byDummy = ( byData & 0x08 ) | ( ( byData & 0x01 ) << 2 ) | ( ( byData & 0x06 ) >> 1 );
            ROMBANK2 = ROMPAGE( byDummy % ( NesHeader.byRomSize << 1 ) );
        }
    }
}




void Map50_HSync()
{

    if ( Map50_IRQ_Enable )
    {
        if ( PPU_Scanline == 21 )
        {
            IRQ_REQ;
        }
    }
}
