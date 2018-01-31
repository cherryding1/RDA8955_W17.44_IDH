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










void Map66_Init()
{
    int nPage;


    MapperInit = Map66_Init;


    MapperWrite = Map66_Write;


    MapperSram = Map66_Write;


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


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 0 );
}




void Map66_Write( WORD wAddr, BYTE byData )
{
    BYTE byRom;
    BYTE byVRom;

    byRom  = ( byData >> 4 ) & 0x0F;
    byVRom = byData & 0x0F;


    byRom <<= 1;
    byRom %= NesHeader.byRomSize;
    byRom <<= 1;

    ROMBANK0 = ROMPAGE( byRom );
    ROMBANK1 = ROMPAGE( byRom + 1 );
    ROMBANK2 = ROMPAGE( byRom + 2 );
    ROMBANK3 = ROMPAGE( byRom + 3 );


    byVRom <<= 3;
    byVRom %= ( NesHeader.byVRomSize << 3 );

    PPUBANK[ 0 ] = VROMPAGE( byVRom );
    PPUBANK[ 1 ] = VROMPAGE( byVRom + 1 );
    PPUBANK[ 2 ] = VROMPAGE( byVRom + 2 );
    PPUBANK[ 3 ] = VROMPAGE( byVRom + 3 );
    PPUBANK[ 4 ] = VROMPAGE( byVRom + 4 );
    PPUBANK[ 5 ] = VROMPAGE( byVRom + 5 );
    PPUBANK[ 6 ] = VROMPAGE( byVRom + 6 );
    PPUBANK[ 7 ] = VROMPAGE( byVRom + 7 );
    InfoNES_SetupChr();
}
