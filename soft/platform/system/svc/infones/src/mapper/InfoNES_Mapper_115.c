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







BYTE    Map115_Reg[8];
BYTE    Map115_Prg0, Map115_Prg1, Map115_Prg2, Map115_Prg3;
BYTE    Map115_Prg0L, Map115_Prg1L;
BYTE    Map115_Chr0, Map115_Chr1, Map115_Chr2, Map115_Chr3;
BYTE    Map115_Chr4, Map115_Chr5, Map115_Chr6, Map115_Chr7;

BYTE    Map115_IRQ_Enable;
BYTE    Map115_IRQ_Counter;
BYTE    Map115_IRQ_Latch;

BYTE    Map115_ExPrgSwitch;
BYTE    Map115_ExChrSwitch;




void Map115_Init()
{

    MapperInit = Map115_Init;


    MapperWrite = Map115_Write;


    MapperSram = Map115_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map115_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 8; i++ )
    {
        Map115_Reg[i] = 0x00;
    }

    Map115_Prg0 = Map115_Prg0L = 0;
    Map115_Prg1 = Map115_Prg1L = 1;
    Map115_Prg2 = ( NesHeader.byRomSize << 1 ) - 2;
    Map115_Prg3 = ( NesHeader.byRomSize << 1 ) - 1;

    Map115_ExPrgSwitch = 0;
    Map115_ExChrSwitch = 0;


    Map115_Set_CPU_Banks();


    if ( NesHeader.byVRomSize > 0 )
    {
        Map115_Chr0 = 0;
        Map115_Chr1 = 1;
        Map115_Chr2 = 2;
        Map115_Chr3 = 3;
        Map115_Chr4 = 4;
        Map115_Chr5 = 5;
        Map115_Chr6 = 6;
        Map115_Chr7 = 7;
        Map115_Set_PPU_Banks();
    }
    else
    {
        Map115_Chr0 = Map115_Chr2 = Map115_Chr4 = Map115_Chr5 = Map115_Chr6 = Map115_Chr7 = 0;
        Map115_Chr1 = Map115_Chr3 = 1;
    }

    Map115_IRQ_Enable = 0;
    Map115_IRQ_Counter = 0;
    Map115_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map115_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr & 0xE001 )
    {
        case  0x8000:
            Map115_Reg[0] = byData;
            Map115_Set_CPU_Banks();
            Map115_Set_PPU_Banks();
            break;
        case  0x8001:
            Map115_Reg[1] = byData;
            switch( Map115_Reg[0] & 0x07 )
            {
                case    0x00:
                    Map115_Chr0 = byData & 0xFE;
                    Map115_Chr1 = Map115_Chr0+1;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x01:
                    Map115_Chr2 = byData & 0xFE;
                    Map115_Chr3 = Map115_Chr2+1;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x02:
                    Map115_Chr4 = byData;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x03:
                    Map115_Chr5 = byData;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x04:
                    Map115_Chr6 = byData;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x05:
                    Map115_Chr7 = byData;
                    Map115_Set_PPU_Banks();
                    break;
                case    0x06:
                    Map115_Prg0 = Map115_Prg0L = byData;
                    Map115_Set_CPU_Banks();
                    break;
                case    0x07:
                    Map115_Prg1 = Map115_Prg1L = byData;
                    Map115_Set_CPU_Banks();
                    break;
            }
            break;
        case  0xA000:
            Map115_Reg[2] = byData;
            if ( !ROM_FourScr )
            {
                if( byData & 0x01 ) InfoNES_Mirroring( 0 );
                else        InfoNES_Mirroring( 1 );
            }
            break;
        case  0xA001:
            Map115_Reg[3] = byData;
            break;
        case  0xC000:
            Map115_Reg[4] = byData;
            Map115_IRQ_Counter = byData;
            Map115_IRQ_Enable = 0xFF;
            break;
        case  0xC001:
            Map115_Reg[5] = byData;
            Map115_IRQ_Latch = byData;
            break;
        case  0xE000:
            Map115_Reg[6] = byData;
            Map115_IRQ_Enable = 0;
            break;
        case  0xE001:
            Map115_Reg[7] = byData;
            Map115_IRQ_Enable = 0xFF;
            break;
    }
}




void Map115_Sram( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case  0x6000:
            Map115_ExPrgSwitch = byData;
            Map115_Set_CPU_Banks();
            break;
        case  0x6001:
            Map115_ExChrSwitch = byData&0x1;
            Map115_Set_PPU_Banks();
            break;
    }
}




void Map115_HSync()
{
    if( (  PPU_Scanline <= 239) )
    {
        if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
        {
            if( Map115_IRQ_Enable )
            {
                if( !(Map115_IRQ_Counter--) )
                {
                    Map115_IRQ_Counter = Map115_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}




void Map115_Set_CPU_Banks()
{
    if( Map115_ExPrgSwitch & 0x80 )
    {
        Map115_Prg0 = ((Map115_ExPrgSwitch<<1)&0x1e);
        Map115_Prg1 = Map115_Prg0+1;

        ROMBANK0 = ROMPAGE( Map115_Prg0 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK2 = ROMPAGE( ( Map115_Prg0+2 ) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK3 = ROMPAGE( ( Map115_Prg1+2 ) % ( NesHeader.byRomSize << 1 ) );
    }
    else
    {
        Map115_Prg0 = Map115_Prg0L;
        Map115_Prg1 = Map115_Prg1L;
        if( Map115_Reg[0] & 0x40 )
        {
            ROMBANK0 = ROMPAGE( ( NesHeader.byRomSize << 1 ) - 2 );
            ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.byRomSize << 1 ) );
            ROMBANK2 = ROMPAGE( Map115_Prg0 % ( NesHeader.byRomSize << 1 ) );
            ROMBANK3 = ROMPAGE( ( NesHeader.byRomSize << 1 ) - 1 );
        }
        else
        {
            ROMBANK0 = ROMPAGE( Map115_Prg0 % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.byRomSize << 1 ) );
            ROMBANK2 = ROMPAGE( ( NesHeader.byRomSize << 1 ) - 2 );
            ROMBANK3 = ROMPAGE( ( NesHeader.byRomSize << 1 ) - 1 );
        }
    }
}




void Map115_Set_PPU_Banks()
{
    if ( NesHeader.byVRomSize > 0 )
    {
        if( Map115_Reg[0] & 0x80 )
        {
            PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
        }
        else
        {
            PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
        }
    }
}

