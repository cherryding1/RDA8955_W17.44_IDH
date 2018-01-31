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







BYTE Map33_Regs[ 8 ];
BYTE Map33_Switch;

BYTE Map33_IRQ_Enable;
BYTE Map33_IRQ_Cnt;




void Map33_Init()
{

    MapperInit = Map33_Init;


    MapperWrite = Map33_Write;


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
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
            Map33_Regs[ nPage ] = nPage;
        }
        InfoNES_SetupChr();
    }
    else
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
        {
            Map33_Regs[ nPage ] = 0;
        }
    }


    Map33_Switch = 0;
    Map33_IRQ_Enable = 0;
    Map33_IRQ_Cnt = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map33_Write( WORD wAddr, BYTE byData )
{

    switch ( wAddr )
    {
        case 0x8000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            break;

        case 0x8001:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0x8002:
            Map33_Regs[ 0 ] = byData * 2;
            Map33_Regs[ 1 ] = byData * 2 + 1;

            PPUBANK[ 0 ] = VROMPAGE( Map33_Regs[ 0 ] % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( Map33_Regs[ 1 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x8003:
            Map33_Regs[ 2 ] = byData * 2;
            Map33_Regs[ 3 ] = byData * 2 + 1;

            PPUBANK[ 2 ] = VROMPAGE( Map33_Regs[ 2 ] % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( Map33_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa000:
            Map33_Regs[ 4 ] = byData;
            PPUBANK[ 4 ] = VROMPAGE( Map33_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa001:
            Map33_Regs[ 5 ] = byData;
            PPUBANK[ 5 ] = VROMPAGE( Map33_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa002:
            Map33_Regs[ 6 ] = byData;
            PPUBANK[ 6 ] = VROMPAGE( Map33_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa003:
            Map33_Regs[ 7 ] = byData;
            PPUBANK[ 7 ] = VROMPAGE( Map33_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc000:
            Map33_IRQ_Cnt = byData;
            break;

        case 0xc001:
        case 0xc002:
        case 0xe001:
        case 0xe002:
            if ( Map33_IRQ_Cnt == byData )
            {
                Map33_IRQ_Enable = 0xff;
            }
            else
            {
                Map33_IRQ_Enable = byData;
            }
            break;

        case 0xe000:
            if ( byData & 0x40 )
            {
                InfoNES_Mirroring( 0 );
            }
            else
            {
                InfoNES_Mirroring( 1 );
            }
            break;
    }
}




void Map33_HSync()
{

    if ( Map33_IRQ_Enable )
    {
        if ( Map33_IRQ_Enable == 0xff )
        {
            if ( PPU_Scanline == (WORD)( 0xff - Map33_IRQ_Cnt ) )
            {
                IRQ_REQ;
                Map33_IRQ_Cnt = 0;
                Map33_IRQ_Enable = 0;
            }
        }
        else
        {
            if ( Map33_IRQ_Cnt == 0xff )
            {
                IRQ_REQ;
                Map33_IRQ_Cnt = 0;
                Map33_IRQ_Enable = 0;
            }
            else
            {
                Map33_IRQ_Cnt++;
            }
        }
    }
}
