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







BYTE  Map65_IRQ_Enable;
DWORD Map65_IRQ_Cnt;
DWORD Map65_IRQ_Latch;




void Map65_Init()
{

    MapperInit = Map65_Init;


    MapperWrite = Map65_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map65_HSync;


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




void Map65_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:
            ROMBANK0 = ROMPAGE( byData % ( NesHeader.byRomSize << 1) );
            break;

        case 0x9000:
            if ( byData & 0x40 )
            {
                InfoNES_Mirroring( 1 );
            }
            else
            {
                InfoNES_Mirroring( 0 );
            }
            break;

        case 0x9003:
            Map65_IRQ_Enable = byData & 0x80;
            break;

        case 0x9004:
            Map65_IRQ_Cnt = Map65_IRQ_Latch;
            break;

        case 0x9005:
            Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0x00ff ) | ((DWORD)byData << 8 );
            break;

        case 0x9006:
            Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0xff00 ) | (DWORD)byData;
            break;


        case 0xb000:
            PPUBANK[ 0 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb001:
            PPUBANK[ 1 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb002:
            PPUBANK[ 2 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb003:
            PPUBANK[ 3 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb004:
            PPUBANK[ 4 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb005:
            PPUBANK[ 5 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb006:
            PPUBANK[ 6 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb007:
            PPUBANK[ 7 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;


        case 0xa000:
            ROMBANK1 = ROMPAGE( byData % ( NesHeader.byRomSize << 1) );
            break;

        case 0xc000:
            ROMBANK2 = ROMPAGE( byData % ( NesHeader.byRomSize << 1) );
            break;
    }
}




void Map65_HSync()
{

    if ( Map65_IRQ_Enable )
    {
        if ( Map65_IRQ_Cnt <= 113 )
        {
            IRQ_REQ;
            Map65_IRQ_Enable = 0;
            Map65_IRQ_Cnt = 0xffff;
        }
        else
        {
            Map65_IRQ_Cnt -= 113;
        }
    }
}
