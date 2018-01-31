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







BYTE    Map248_Reg[8];
BYTE    Map248_Prg0, Map248_Prg1;
BYTE    Map248_Chr01, Map248_Chr23, Map248_Chr4, Map248_Chr5, Map248_Chr6, Map248_Chr7;
BYTE    Map248_WeSram;

BYTE    Map248_IRQ_Enable;
BYTE    Map248_IRQ_Counter;
BYTE    Map248_IRQ_Latch;
BYTE    Map248_IRQ_Request;




void Map248_Init()
{

    MapperInit = Map248_Init;


    MapperWrite = Map248_Write;


    MapperSram = Map248_Sram;


    MapperApu = Map248_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map248_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 8; i++ )
    {
        Map248_Reg[i] = 0x00;
    }


    Map248_Prg0 = 0;
    Map248_Prg1 = 1;
    Map248_Set_CPU_Banks();


    Map248_Chr01 = 0;
    Map248_Chr23 = 2;
    Map248_Chr4  = 4;
    Map248_Chr5  = 5;
    Map248_Chr6  = 6;
    Map248_Chr7  = 7;
    Map248_Set_PPU_Banks();

    Map248_WeSram  = 0;         Map248_IRQ_Enable = 0;      Map248_IRQ_Counter = 0;
    Map248_IRQ_Latch = 0;
    Map248_IRQ_Request = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map248_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr & 0xE001 )
    {
        case  0x8000:
            Map248_Reg[0] = byData;
            Map248_Set_CPU_Banks();
            Map248_Set_PPU_Banks();
            break;
        case  0x8001:
            Map248_Reg[1] = byData;

            switch( Map248_Reg[0] & 0x07 )
            {
                case    0x00:
                    Map248_Chr01 = byData & 0xFE;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x01:
                    Map248_Chr23 = byData & 0xFE;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x02:
                    Map248_Chr4 = byData;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x03:
                    Map248_Chr5 = byData;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x04:
                    Map248_Chr6 = byData;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x05:
                    Map248_Chr7 = byData;
                    Map248_Set_PPU_Banks();
                    break;
                case    0x06:
                    Map248_Prg0 = byData;
                    Map248_Set_CPU_Banks();
                    break;
                case    0x07:
                    Map248_Prg1 = byData;
                    Map248_Set_CPU_Banks();
                    break;
            }
            break;
        case  0xA000:
            Map248_Reg[2] = byData;
            if( !ROM_FourScr )
            {
                if( byData & 0x01 )
                {
                    InfoNES_Mirroring( 0 );
                }
                else
                {
                    InfoNES_Mirroring( 1 );
                }
            }
            break;
        case 0xC000:
            Map248_IRQ_Enable=0;
            Map248_IRQ_Latch=0xBE;
            Map248_IRQ_Counter =0xBE;
            break;
        case 0xC001:
            Map248_IRQ_Enable=1;
            Map248_IRQ_Latch=0xBE;
            Map248_IRQ_Counter=0xBE;
            break;
    }
}




void Map248_Sram( WORD wAddr, BYTE byData )
{
    ROMBANK0 = ROMPAGE(((byData<<1)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1 = ROMPAGE(((byData<<1)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
}




void Map248_Apu( WORD wAddr, BYTE byData )
{
    Map248_Sram( wAddr, byData );
}




void Map248_HSync()
{
    if( (  PPU_Scanline <= 239) )
    {
        if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
        {
            if( Map248_IRQ_Enable )
            {
                if( !(Map248_IRQ_Counter--) )
                {
                    Map248_IRQ_Counter = Map248_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}




void Map248_Set_CPU_Banks()
{
    if( Map248_Reg[0] & 0x40 )
    {
        ROMBANK0 = ROMLASTPAGE( 1 );
        ROMBANK1 = ROMPAGE(Map248_Prg1 % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(Map248_Prg0 % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMLASTPAGE( 0 );
    }
    else
    {
        ROMBANK0 = ROMPAGE(Map248_Prg0 % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(Map248_Prg1 % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }
}




void Map248_Set_PPU_Banks()
{
    if( NesHeader.byRomSize > 0 )
    {
        if( Map248_Reg[0] & 0x80 )
        {
            PPUBANK[ 0 ] = VROMPAGE(Map248_Chr4 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE(Map248_Chr5 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE(Map248_Chr6 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE(Map248_Chr7 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE((Map248_Chr01+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE((Map248_Chr01+1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE((Map248_Chr23+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE((Map248_Chr23+1) % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
        }
        else
        {
            PPUBANK[ 0 ] = VROMPAGE((Map248_Chr01+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE((Map248_Chr01+1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE((Map248_Chr23+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE((Map248_Chr23+1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE(Map248_Chr4 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE(Map248_Chr5 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE(Map248_Chr6 % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE(Map248_Chr7 % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
        }
    }
}
