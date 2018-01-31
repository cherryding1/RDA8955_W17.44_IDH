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










void Map89_Init()
{

    MapperInit = Map89_Init;


    MapperWrite = Map89_Write;


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




void Map89_Write( WORD wAddr, BYTE byData )
{
    if ( ( wAddr & 0xFF00 ) == 0xC000 )
    {
        BYTE byPrgBank = (byData & 0x70) >> 4;
        BYTE byChrBank = ((byData & 0x80) >> 4) | (byData & 0x07);


        ROMBANK0 = ROMPAGE( (byPrgBank*2+0) % (NesHeader.byRomSize << 1) );
        ROMBANK1 = ROMPAGE( (byPrgBank*2+1) % (NesHeader.byRomSize << 1) );

        PPUBANK[ 0 ] = VROMPAGE( (byChrBank*8+0) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 1 ] = VROMPAGE( (byChrBank*8+1) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 2 ] = VROMPAGE( (byChrBank*8+2) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 3 ] = VROMPAGE( (byChrBank*8+3) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 4 ] = VROMPAGE( (byChrBank*8+4) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 5 ] = VROMPAGE( (byChrBank*8+5) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 6 ] = VROMPAGE( (byChrBank*8+6) % (NesHeader.byVRomSize << 3) );
        PPUBANK[ 7 ] = VROMPAGE( (byChrBank*8+7) % (NesHeader.byVRomSize << 3) );
        InfoNES_SetupChr();

        if ( byData & 0x08 )
        {
            InfoNES_Mirroring( 2 );
        }
        else
        {
            InfoNES_Mirroring( 3 );
        }
    }
}
