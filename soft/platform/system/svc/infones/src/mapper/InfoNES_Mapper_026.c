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







BYTE Map26_IRQ_Enable;
BYTE Map26_IRQ_Cnt;
BYTE Map26_IRQ_Latch;




void Map26_Init()
{

    MapperInit = Map26_Init;


    MapperWrite = Map26_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map26_HSync;


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


    Map26_IRQ_Enable = 0;
    Map26_IRQ_Cnt = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map26_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {

        case 0x8000:
            byData <<= 1;
            byData %= ( NesHeader.byRomSize << 1 );

            ROMBANK0 = ROMPAGE( byData + 0 );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;


        case 0xb003:
            switch ( byData & 0x7f )
            {
                case 0x08:
                case 0x2c:
                    InfoNES_Mirroring( 2 );
                    break;
                case 0x20:
                    InfoNES_Mirroring( 1 );
                    break;
                case 0x24:
                    InfoNES_Mirroring( 0 );
                    break;
                case 0x28:
                    InfoNES_Mirroring( 3 );
                    break;
            }
            break;


        case 0xc000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK2 = ROMPAGE( byData );
            InfoNES_SetupChr();
            break;


        case 0xd000:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 0 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xd001:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 2 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xd002:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 1 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xd003:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 3 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xe000:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 4 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xe001:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 6 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xe002:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 5 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0xe003:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 7 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;


        case 0xf000:
            Map26_IRQ_Latch = byData;
            break;

        case 0xf001:
            Map26_IRQ_Enable = byData & 0x01;
            break;

        case 0xf002:
            Map26_IRQ_Enable = byData & 0x03;

            if ( Map26_IRQ_Enable & 0x02 )
            {
                Map26_IRQ_Cnt = Map26_IRQ_Latch;
            }
            break;
    }
}




void Map26_HSync()
{

    if ( Map26_IRQ_Enable & 0x03 )
    {
        if ( Map26_IRQ_Cnt >= 0xfe )
        {
            IRQ_REQ;
            Map26_IRQ_Cnt = Map26_IRQ_Latch;
            Map26_IRQ_Enable = 0;
        }
        else
        {
            Map26_IRQ_Cnt++;
        }
    }
}
