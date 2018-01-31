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







BYTE Map64_Cmd;
BYTE Map64_Prg;
BYTE Map64_Chr;




void Map64_Init()
{

    MapperInit = Map64_Init;


    MapperWrite = Map64_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMLASTPAGE( 0 );
    ROMBANK1 = ROMLASTPAGE( 0 );
    ROMBANK2 = ROMLASTPAGE( 0 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    Map64_Cmd = 0x00;
    Map64_Prg = 0x00;
    Map64_Chr = 0x00;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map64_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:

            Map64_Cmd = byData & 0x0f;
            Map64_Prg = ( byData & 0x40 ) >> 6;
            Map64_Chr = ( byData & 0x80 ) >> 7;
            break;

        case 0x8001:
            switch ( Map64_Cmd )
            {
                case 0x00:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 4 ] = VROMPAGE( byData );
                        PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
                    }
                    else
                    {
                        PPUBANK[ 0 ] = VROMPAGE( byData );
                        PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x01:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 6 ] = VROMPAGE( byData );
                        PPUBANK[ 7 ] = VROMPAGE( byData + 1 );
                    }
                    else
                    {
                        PPUBANK[ 2 ] = VROMPAGE( byData );
                        PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x02:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 0 ] = VROMPAGE( byData );
                    }
                    else
                    {
                        PPUBANK[ 4 ] = VROMPAGE( byData );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x03:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 1 ] = VROMPAGE( byData );
                    }
                    else
                    {
                        PPUBANK[ 5 ] = VROMPAGE( byData );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x04:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 2 ] = VROMPAGE( byData );
                    }
                    else
                    {
                        PPUBANK[ 6 ] = VROMPAGE( byData );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x05:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    if ( Map64_Chr )
                    {
                        PPUBANK[ 3 ] = VROMPAGE( byData );
                    }
                    else
                    {
                        PPUBANK[ 7 ] = VROMPAGE( byData );
                    }
                    InfoNES_SetupChr();
                    break;

                case 0x06:

                    byData %= ( NesHeader.byRomSize << 1 );
                    if ( Map64_Prg )
                    {
                        ROMBANK1 = ROMPAGE( byData );
                    }
                    else
                    {
                        ROMBANK0 = ROMPAGE( byData );
                    }
                    break;

                case 0x07:

                    byData %= ( NesHeader.byRomSize << 1 );
                    if ( Map64_Prg )
                    {
                        ROMBANK2 = ROMPAGE( byData );
                    }
                    else
                    {
                        ROMBANK1 = ROMPAGE( byData );
                    }
                    break;

                case 0x08:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 1 ] = VROMPAGE( byData );
                    InfoNES_SetupChr();
                    break;

                case 0x09:

                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 3 ] = VROMPAGE( byData );
                    InfoNES_SetupChr();
                    break;

                case 0x0f:

                    byData %= ( NesHeader.byRomSize << 1 );
                    if ( Map64_Prg )
                    {
                        ROMBANK0 = ROMPAGE( byData );
                    }
                    else
                    {
                        ROMBANK2 = ROMPAGE( byData );
                    }
                    break;
            }
            break;

        default:
            switch ( wAddr & 0xf000 )
            {
                case 0xa000:

                    InfoNES_Mirroring( byData & 0x01 );
                    break;

                default:
                    break;
            }
            break;
    }
}
