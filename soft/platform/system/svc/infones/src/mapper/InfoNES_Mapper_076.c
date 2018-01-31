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







BYTE Map76_Reg;




void Map76_Init()
{

    MapperInit = Map76_Init;


    MapperWrite = Map76_Write;


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




void Map76_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:
            Map76_Reg = byData;
            break;

        case 0x8001:
            switch ( Map76_Reg & 0x07 )
            {
                case 0x02:
                    byData <<= 1;
                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 0 ] = VROMPAGE( byData );
                    PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
                    InfoNES_SetupChr();
                    break;

                case 0x03:
                    byData <<= 1;
                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 2 ] = VROMPAGE( byData );
                    PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
                    InfoNES_SetupChr();
                    break;

                case 0x04:
                    byData <<= 1;
                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 4 ] = VROMPAGE( byData );
                    PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
                    InfoNES_SetupChr();
                    break;

                case 0x05:
                    byData <<= 1;
                    byData %= ( NesHeader.byVRomSize << 3 );
                    PPUBANK[ 6 ] = VROMPAGE( byData );
                    PPUBANK[ 7 ] = VROMPAGE( byData + 1 );
                    InfoNES_SetupChr();
                    break;

                case 0x06:
                    byData %= ( NesHeader.byRomSize << 1 );
                    ROMBANK0 = ROMPAGE( byData );
                    break;

                case 0x07:
                    byData %= ( NesHeader.byRomSize << 1 );
                    ROMBANK1 = ROMPAGE( byData );
                    break;
            }
            break;
    }
}
