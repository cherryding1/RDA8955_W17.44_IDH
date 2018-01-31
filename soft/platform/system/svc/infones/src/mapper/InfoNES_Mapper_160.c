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







BYTE Map160_IRQ_Enable;
BYTE Map160_IRQ_Cnt;
BYTE Map160_IRQ_Latch;
BYTE Map160_Refresh_Type;




void Map160_Init()
{

    MapperInit = Map160_Init;


    MapperWrite = Map160_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map160_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    Map160_IRQ_Enable = 0;
    Map160_IRQ_Cnt = 0;
    Map160_IRQ_Latch = 0;
    Map160_Refresh_Type = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map160_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {

        case 0x8000:
            ROMBANK0 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8001:
            ROMBANK1 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8002:
            ROMBANK2 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x9000:
            if ( byData == 0x2b )
            {
                Map160_Refresh_Type = 1;
            }
            else if ( byData == 0xa8 )
            {
                Map160_Refresh_Type = 2;
            }
            else if ( byData == 0x1f )
            {
                Map160_Refresh_Type = 3;
            }
            else if ( byData == 0x7c )
            {
                Map160_Refresh_Type = 4;
            }
            else if ( byData == 0x18 )
            {
                Map160_Refresh_Type = 5;
            }
            else if ( byData == 0x60 )
            {
                Map160_Refresh_Type = 6;
            }
            else
            {
                Map160_Refresh_Type = 0;
            }

            PPUBANK[ 0 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9001:
            PPUBANK[ 1 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9002:
            if ( Map160_Refresh_Type == 2 && byData != 0xe8 )
            {
                Map160_Refresh_Type = 0;
            }
            PPUBANK[ 2 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9003:
            PPUBANK[ 3 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9004:
            PPUBANK[ 4 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9005:
            PPUBANK[ 5 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9006:
            PPUBANK[ 6 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x9007:
            PPUBANK[ 7 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc000:
            Map160_IRQ_Cnt = Map160_IRQ_Latch;
            Map160_IRQ_Enable = Map160_IRQ_Latch;
            break;

        case 0xc001:
            Map160_IRQ_Latch = byData;
            break;

        case 0xc002:
            Map160_IRQ_Enable = 0;
            break;

        case 0xc003:
            Map160_IRQ_Cnt = byData;
            break;
    }
}




void Map160_HSync()
{

    if ( PPU_Scanline == 0 || PPU_Scanline == 239 )
    {
        if ( Map160_Refresh_Type == 1 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x58 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x59 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x5a % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x5b % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x58 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x59 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x5a % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x5b % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else if ( Map160_Refresh_Type == 2 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x78 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x79 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x7a % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x7b % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x78 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x79 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x7a % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x7b % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else if ( Map160_Refresh_Type == 3 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x7c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x7d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x7e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x7f % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else if ( Map160_Refresh_Type == 5 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x70 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x71 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x72 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x73 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x74 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x75 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x76 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x77 % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else if ( Map160_Refresh_Type == 6 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x5c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x5d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x5e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x5f % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }

    if ( PPU_Scanline == 64 )
    {
        if ( Map160_Refresh_Type == 4 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x6c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x6d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x6e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x6f % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }

    if ( PPU_Scanline == 128 )
    {
        if ( Map160_Refresh_Type == 4 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x68 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x69 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x6a % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x6b % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else if ( Map160_Refresh_Type == 5 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x74 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x75 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x76 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x77 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x74 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x75 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x76 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x77 % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }

    if ( PPU_Scanline == 160 )
    {
        if ( Map160_Refresh_Type == 6 )
        {
            PPUBANK[ 0 ] = VROMPAGE( 0x60 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( 0x61 % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( 0x5e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( 0x5f % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }

    if ( Map160_IRQ_Enable )
    {
        if ( Map160_IRQ_Cnt == 0xff )
        {
            IRQ_REQ;
            Map160_IRQ_Cnt = 0;
            Map160_IRQ_Enable = 0;
        }
        else
        {
            Map160_IRQ_Cnt++;
        }
    }
}
