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







BYTE  Map49_Regs[ 3 ];
DWORD Map49_Prg0, Map49_Prg1;
DWORD Map49_Chr01, Map49_Chr23;
DWORD Map49_Chr4, Map49_Chr5, Map49_Chr6, Map49_Chr7;

BYTE Map49_IRQ_Enable;
BYTE Map49_IRQ_Cnt;
BYTE Map49_IRQ_Latch;




void Map49_Init()
{

    MapperInit = Map49_Init;


    MapperWrite = Map49_Write;


    MapperSram = Map49_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map49_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map49_Prg0 = 0;
    Map49_Prg1 = 1;
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    Map49_Chr01 = 0;
    Map49_Chr23 = 2;
    Map49_Chr4  = 4;
    Map49_Chr5  = 5;
    Map49_Chr6  = 6;
    Map49_Chr7  = 7;

    for ( int nPage = 0; nPage < 8; ++nPage )
    {
        PPUBANK[ nPage ] = VROMPAGE( nPage );
    }
    InfoNES_SetupChr();


    Map49_Regs[ 0 ] = Map49_Regs[ 1 ] = Map49_Regs[ 2 ] = 0;
    Map49_IRQ_Enable = 0;
    Map49_IRQ_Cnt = 0;
    Map49_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map49_Sram( WORD wAddr, BYTE byData )
{
    if ( Map49_Regs[ 2 ] & 0x80 )
    {
        Map49_Regs[ 1 ] = byData;
        Map49_Set_CPU_Banks();
        Map49_Set_PPU_Banks();
    }
}




void Map49_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xe001 )
    {
        case 0x8000:
            if ( ( byData & 0x40 ) != ( Map49_Regs[ 0 ] & 0x40 ) )
            {
                Map49_Set_CPU_Banks();
            }
            if ( ( byData & 0x80 ) != ( Map49_Regs[ 0 ] & 0x80 ) )
            {
                Map49_Regs[ 0 ] = byData;
                Map49_Set_PPU_Banks();
            }
            Map49_Regs[ 0 ] = byData;
            break;

        case 0x8001:
            switch ( Map49_Regs[ 0 ] & 0x07 )
            {

                case 0x00:
                    Map49_Chr01 = byData & 0xfe;
                    Map49_Set_PPU_Banks();
                    break;

                case 0x01:
                    Map49_Chr23 = byData & 0xfe;
                    Map49_Set_PPU_Banks();
                    break;

                case 0x02:
                    Map49_Chr4 = byData;
                    Map49_Set_PPU_Banks();
                    break;

                case 0x03:
                    Map49_Chr5 = byData;
                    Map49_Set_PPU_Banks();
                    break;

                case 0x04:
                    Map49_Chr6 = byData;
                    Map49_Set_PPU_Banks();
                    break;

                case 0x05:
                    Map49_Chr7 = byData;
                    Map49_Set_PPU_Banks();
                    break;


                case 0x06:
                    Map49_Prg0 = byData;
                    Map49_Set_CPU_Banks();
                    break;

                case 0x07:
                    Map49_Prg1 = byData;
                    Map49_Set_CPU_Banks();
                    break;
            }
            break;

        case 0xa000:
            if ( !ROM_FourScr )
            {
                if ( byData & 0x01 )
                {
                    InfoNES_Mirroring( 0 );
                }
                else
                {
                    InfoNES_Mirroring( 1 );
                }
            }
            break;

        case 0xa001:
            Map49_Regs[ 2 ] = byData;
            break;

        case 0xc000:
            Map49_IRQ_Cnt = byData;
            break;

        case 0xc001:
            Map49_IRQ_Latch = byData;
            break;

        case 0xe000:
            Map49_IRQ_Enable = 0;
            break;

        case 0xe001:
            Map49_IRQ_Enable = 1;
            break;
    }
}




void Map49_HSync()
{

    if ( Map49_IRQ_Enable )
    {
        if (  PPU_Scanline <= 239 )
        {
            if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
            {
                if ( !( Map49_IRQ_Cnt-- ) )
                {
                    Map49_IRQ_Cnt = Map49_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}




void Map49_Set_CPU_Banks()
{
    DWORD dwBank0, dwBank1, dwBank2, dwBank3;

    if ( Map49_Regs[ 1 ] & 0x01 )
    {
        if ( Map49_Regs[ 0 ] & 0x40 )
        {
            dwBank0 = ( ( ( NesHeader.byRomSize << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank2 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank3 = ( ( ( NesHeader.byRomSize << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
        }
        else
        {
            dwBank0 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank2 = ( ( ( NesHeader.byRomSize << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
            dwBank3 = ( ( ( NesHeader.byRomSize << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
        }
    }
    else
    {
        dwBank0 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 0;
        dwBank1 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 1;
        dwBank2 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 2;
        dwBank3 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 3;
    }


    ROMBANK0 = ROMPAGE( dwBank0 % ( NesHeader.byRomSize << 1) );
    ROMBANK1 = ROMPAGE( dwBank1 % ( NesHeader.byRomSize << 1) );
    ROMBANK2 = ROMPAGE( dwBank2 % ( NesHeader.byRomSize << 1) );
    ROMBANK3 = ROMPAGE( dwBank3 % ( NesHeader.byRomSize << 1) );
}




void Map49_Set_PPU_Banks()
{
    Map49_Chr01 = ( Map49_Chr01 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
    Map49_Chr23 = ( Map49_Chr23 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
    Map49_Chr4 = ( Map49_Chr4 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
    Map49_Chr5 = ( Map49_Chr5 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
    Map49_Chr6 = ( Map49_Chr6 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
    Map49_Chr7 = ( Map49_Chr7 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );


    if ( Map49_Regs[ 0 ] & 0x80 )
    {
        PPUBANK[ 0 ] = VROMPAGE( Map49_Chr4 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 1 ] = VROMPAGE( Map49_Chr5 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 2 ] = VROMPAGE( Map49_Chr6 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 3 ] = VROMPAGE( Map49_Chr7 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 4 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 5 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 6 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 7 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
        InfoNES_SetupChr();
    }
    else
    {
        PPUBANK[ 0 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 1 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 2 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 3 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 4 ] = VROMPAGE( Map49_Chr4 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 5 ] = VROMPAGE( Map49_Chr5 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 6 ] = VROMPAGE( Map49_Chr6 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 7 ] = VROMPAGE( Map49_Chr7 % ( NesHeader.byVRomSize << 3 ) );
        InfoNES_SetupChr();
    }
}
