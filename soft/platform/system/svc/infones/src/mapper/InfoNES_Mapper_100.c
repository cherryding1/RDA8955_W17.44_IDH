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







BYTE    Map100_Reg[8];
BYTE    Map100_Prg0, Map100_Prg1, Map100_Prg2, Map100_Prg3;
BYTE    Map100_Chr0, Map100_Chr1, Map100_Chr2, Map100_Chr3;
BYTE    Map100_Chr4, Map100_Chr5, Map100_Chr6, Map100_Chr7;

BYTE    Map100_IRQ_Enable;
BYTE    Map100_IRQ_Cnt;
BYTE    Map100_IRQ_Latch;




void Map100_Init()
{

    MapperInit = Map100_Init;


    MapperWrite = Map100_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map100_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        Map100_Chr0 = 0;
        Map100_Chr1 = 1;
        Map100_Chr2 = 2;
        Map100_Chr3 = 3;
        Map100_Chr4 = 4;
        Map100_Chr5 = 5;
        Map100_Chr6 = 6;
        Map100_Chr7 = 7;
        Map100_Set_PPU_Banks();
    }
    else
    {
        Map100_Chr0 = Map100_Chr2 = Map100_Chr4 = Map100_Chr5 = Map100_Chr6 = Map100_Chr7 = 0;
        Map100_Chr1 = Map100_Chr3 = 1;
    }


    Map100_IRQ_Enable = 0;
    Map100_IRQ_Cnt = 0;
    Map100_IRQ_Latch = 0;
    for( int i = 0; i < 8; i++ ) { Map100_Reg[ i ] = 0x00; }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map100_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xE001 )
    {
        case 0x8000:
            Map100_Reg[0] = byData;
            break;

        case 0x8001:
            Map100_Reg[1] = byData;

            switch ( Map100_Reg[0] & 0xC7 )
            {
                case 0x00:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr0 = byData&0xFE;
                        Map100_Chr1 = Map100_Chr0+1;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x01:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr2 = byData&0xFE;
                        Map100_Chr3 = Map100_Chr2+1;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x02:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr4 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x03:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr5 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x04:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr6 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x05:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr7 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x06:
                    Map100_Prg0 = byData;
                    Map100_Set_CPU_Banks();
                    break;

                case 0x07:
                    Map100_Prg1 = byData;
                    Map100_Set_CPU_Banks();
                    break;

                case 0x46:
                    Map100_Prg2 = byData;
                    Map100_Set_CPU_Banks();
                    break;

                case 0x47:
                    Map100_Prg3 = byData;
                    Map100_Set_CPU_Banks();
                    break;

                case 0x80:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr4 = byData&0xFE;
                        Map100_Chr5 = Map100_Chr4+1;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x81:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr6 = byData&0xFE;
                        Map100_Chr7 = Map100_Chr6+1;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x82:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr0 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x83:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr1 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x84:
                    if ( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr2 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

                case 0x85:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map100_Chr3 = byData;
                        Map100_Set_PPU_Banks();
                    }
                    break;

            }
            break;

        case 0xA000:
            Map100_Reg[2] = byData;
            if ( !ROM_FourScr )
            {
                if( byData & 0x01 ) InfoNES_Mirroring( 0 );
                else            InfoNES_Mirroring( 1 );
            }
            break;

        case 0xA001:
            Map100_Reg[3] = byData;
            break;

        case 0xC000:
            Map100_Reg[4] = byData;
            Map100_IRQ_Cnt = byData;
            break;

        case 0xC001:
            Map100_Reg[5] = byData;
            Map100_IRQ_Latch = byData;
            break;

        case 0xE000:
            Map100_Reg[6] = byData;
            Map100_IRQ_Enable = 0;
            break;

        case 0xE001:
            Map100_Reg[7] = byData;
            Map100_IRQ_Enable = 0xFF;
            break;
    }
}




void Map100_HSync()
{
    if( (  PPU_Scanline <= 239) )
    {
        if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
        {
            if( Map100_IRQ_Enable )
            {
                if( !(Map100_IRQ_Cnt--) )
                {
                    Map100_IRQ_Cnt = Map100_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}




void Map100_Set_CPU_Banks()
{
    ROMBANK0 = ROMPAGE( Map100_Prg0 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( Map100_Prg1 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( Map100_Prg2 % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( Map100_Prg3 % ( NesHeader.byRomSize << 1 ) );
}




void Map100_Set_PPU_Banks()
{
    if ( NesHeader.byVRomSize > 0 )
    {
        PPUBANK[ 0 ] = VROMPAGE( Map100_Chr0 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 1 ] = VROMPAGE( Map100_Chr1 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 2 ] = VROMPAGE( Map100_Chr2 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 3 ] = VROMPAGE( Map100_Chr3 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 4 ] = VROMPAGE( Map100_Chr4 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 5 ] = VROMPAGE( Map100_Chr5 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 6 ] = VROMPAGE( Map100_Chr6 % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 7 ] = VROMPAGE( Map100_Chr7 % ( NesHeader.byVRomSize << 3 ) );
        InfoNES_SetupChr();
    }
}

