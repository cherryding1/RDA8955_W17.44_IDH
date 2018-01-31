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







BYTE Map32_Saved;




void Map32_Init()
{

    MapperInit = Map32_Init;


    MapperWrite = Map32_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    Map32_Saved = 0x00;


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




void Map32_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf000 )
    {
        case 0x8000:

            byData %= ( NesHeader.byRomSize << 1 );

            if ( Map32_Saved & 0x02 )
            {
                ROMBANK2 = ROMPAGE( byData );
            }
            else
            {
                ROMBANK0 = ROMPAGE( byData );
            }
            break;

        case 0x9000:
            Map32_Saved = byData;

            InfoNES_Mirroring( byData & 0x01 );
            break;

        case 0xa000:

            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0xb000:

            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ wAddr & 0x0007 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        default:
            break;
    }
}
