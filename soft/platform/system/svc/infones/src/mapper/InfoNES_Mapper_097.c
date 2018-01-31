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










void Map97_Init()
{

    MapperInit = Map97_Init;


    MapperWrite = Map97_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMLASTPAGE( 0 );
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 1 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map97_Write( WORD wAddr, BYTE byData )
{

    if ( wAddr < 0xc000 )
    {
        BYTE byPrgBank = byData & 0x0f;

        byPrgBank <<= 1;
        byPrgBank %= ( NesHeader.byRomSize << 1 );

        ROMBANK2 = ROMPAGE( byPrgBank );
        ROMBANK3 = ROMPAGE( byPrgBank + 1 );

        if ( ( byData & 0x80 ) == 0 )
        {
            InfoNES_Mirroring( 0 );
        }
        else
        {
            InfoNES_Mirroring( 1 );
        }
    }
}
