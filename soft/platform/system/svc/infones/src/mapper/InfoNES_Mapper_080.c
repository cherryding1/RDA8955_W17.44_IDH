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










void Map80_Init()
{

    MapperInit = Map80_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map80_Sram;


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




void Map80_Sram( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {

        case 0x7ef0:
            byData &= 0x7f;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 0 ] = VROMPAGE( byData );
            PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0x7ef1:
            byData &= 0x7f;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 2 ] = VROMPAGE( byData );
            PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0x7ef2:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 4 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x7ef3:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 5 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x7ef4:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 6 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x7ef5:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 7 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;


        case 0x7ef6:
            if ( byData & 0x01 )
            {
                InfoNES_Mirroring( 1 );
            }
            else
            {
                InfoNES_Mirroring( 0 );
            }


        case 0x7efa:
        case 0x7efb:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            break;

        case 0x7efc:
        case 0x7efd:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0x7efe:
        case 0x7eff:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK2 = ROMPAGE( byData );
            break;
    }
}
