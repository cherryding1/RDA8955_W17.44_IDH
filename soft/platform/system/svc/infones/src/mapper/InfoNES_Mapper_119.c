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







BYTE    Map119_Reg[8];
BYTE    Map119_Prg0, Map119_Prg1;
BYTE    Map119_Chr01, Map119_Chr23, Map119_Chr4, Map119_Chr5, Map119_Chr6, Map119_Chr7;
BYTE    Map119_WeSram;

BYTE    Map119_IRQ_Enable;
BYTE    Map119_IRQ_Counter;
BYTE    Map119_IRQ_Latch;




void Map119_Init()
{

    MapperInit = Map119_Init;


    MapperWrite = Map119_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map119_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 8; i++ )
    {
        Map119_Reg[i] = 0x00;
    }


    Map119_Prg0 = 0;
    Map119_Prg1 = 1;
    Map119_Set_CPU_Banks();


    Map119_Chr01 = 0;
    Map119_Chr23 = 2;
    Map119_Chr4  = 4;
    Map119_Chr5  = 5;
    Map119_Chr6  = 6;
    Map119_Chr7  = 7;
    Map119_Set_PPU_Banks();

    Map119_WeSram  = 0;         Map119_IRQ_Enable = 0;      Map119_IRQ_Counter = 0;
    Map119_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map119_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr & 0xE001 )
    {
        case  0x8000:
            Map119_Reg[0] = byData;
            Map119_Set_CPU_Banks();
            Map119_Set_PPU_Banks();
            break;
        case  0x8001:
            Map119_Reg[1] = byData;

            switch( Map119_Reg[0] & 0x07 )
            {
                case    0x00:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr01 = byData & 0xFE;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x01:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr23 = byData & 0xFE;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x02:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr4 = byData;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x03:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr5 = byData;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x04:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr6 = byData;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x05:
                    if( NesHeader.byVRomSize > 0 )
                    {
                        Map119_Chr7 = byData;
                        Map119_Set_PPU_Banks();
                    }
                    break;
                case    0x06:
                    Map119_Prg0 = byData;
                    Map119_Set_CPU_Banks();
                    break;
                case    0x07:
                    Map119_Prg1 = byData;
                    Map119_Set_CPU_Banks();
                    break;
            }
            break;
        case  0xA000:
            Map119_Reg[2] = byData;
            if( !ROM_FourScr )
            {
                if( byData & 0x01 ) InfoNES_Mirroring( 0 );
                else        InfoNES_Mirroring( 1 );
            }
            break;
        case  0xA001:
            Map119_Reg[3] = byData;
            break;
        case  0xC000:
            Map119_Reg[4] = byData;
            Map119_IRQ_Counter = byData;
            break;
        case  0xC001:
            Map119_Reg[5] = byData;
            Map119_IRQ_Latch = byData;
            break;
        case  0xE000:
            Map119_Reg[6] = byData;
            Map119_IRQ_Enable = 0;
            Map119_IRQ_Counter = Map119_IRQ_Latch;
            break;
        case  0xE001:
            Map119_Reg[7] = byData;
            Map119_IRQ_Enable = 1;
            break;
    }
}




void Map119_HSync()
{
    if( (  PPU_Scanline <= 239) )
    {
        if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
        {
            if( Map119_IRQ_Enable )
            {
                if( !(Map119_IRQ_Counter--) )
                {
                    Map119_IRQ_Counter = Map119_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}




void Map119_Set_CPU_Banks()
{
    if( Map119_Reg[0] & 0x40 )
    {
        ROMBANK0 = ROMLASTPAGE( 1 );
        ROMBANK1 = ROMPAGE( Map119_Prg1 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK2 = ROMPAGE( Map119_Prg0 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( Map119_Prg0 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK1 = ROMPAGE( Map119_Prg1 % ( NesHeader.byRomSize << 1 ) );
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }
}




void Map119_Set_PPU_Banks()
{
    if( Map119_Reg[0]&0x80 )
    {
        if(Map119_Chr4&0x40)    PPUBANK[ 0 ] = CRAMPAGE(Map119_Chr4&0x07);
        else            PPUBANK[ 0 ] = VROMPAGE(Map119_Chr4 % (NesHeader.byRomSize<<1));
        if(Map119_Chr5&0x40)    PPUBANK[ 1 ] = CRAMPAGE(Map119_Chr5&0x07);
        else            PPUBANK[ 1 ] = VROMPAGE(Map119_Chr5 % (NesHeader.byRomSize<<1));
        if(Map119_Chr6&0x40)    PPUBANK[ 2 ] = CRAMPAGE(Map119_Chr6&0x07);
        else            PPUBANK[ 2 ] = VROMPAGE(Map119_Chr6 % (NesHeader.byRomSize<<1));
        if(Map119_Chr7&0x40)    PPUBANK[ 3 ] = CRAMPAGE(Map119_Chr7&0x07);
        else            PPUBANK[ 3 ] = VROMPAGE(Map119_Chr7 % (NesHeader.byRomSize<<1));

        if((Map119_Chr01+0)&0x40)   PPUBANK[ 4 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
        else                PPUBANK[ 4 ] = VROMPAGE((Map119_Chr01+0) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr01+1)&0x40)   PPUBANK[ 5 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
        else                PPUBANK[ 5 ] = VROMPAGE((Map119_Chr01+1) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr23+0)&0x40)   PPUBANK[ 6 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
        else            PPUBANK[ 6 ] = VROMPAGE((Map119_Chr23+0) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr23+1)&0x40)   PPUBANK[ 7 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
        else            PPUBANK[ 7 ] = VROMPAGE((Map119_Chr23+1) % (NesHeader.byVRomSize<<3));
    }
    else
    {
        if((Map119_Chr01+0)&0x40)   PPUBANK[ 0 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
        else            PPUBANK[ 0 ] = VROMPAGE((Map119_Chr01+0) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr01+1)&0x40)   PPUBANK[ 1 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
        else            PPUBANK[ 1 ] = VROMPAGE((Map119_Chr01+1) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr23+0)&0x40)   PPUBANK[ 2 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
        else                PPUBANK[ 2 ] = VROMPAGE((Map119_Chr23+0) % (NesHeader.byVRomSize<<3));
        if((Map119_Chr23+1)&0x40)   PPUBANK[ 3 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
        else                PPUBANK[ 3 ] = VROMPAGE((Map119_Chr23+1) % (NesHeader.byVRomSize<<3));

        if(Map119_Chr4&0x40)    PPUBANK[ 4 ] = CRAMPAGE(Map119_Chr4&0x07);
        else            PPUBANK[ 4 ] = VROMPAGE(Map119_Chr4 % (NesHeader.byVRomSize<<3));
        if(Map119_Chr5&0x40)    PPUBANK[ 5 ] = CRAMPAGE(Map119_Chr5&0x07);
        else            PPUBANK[ 5 ] = VROMPAGE(Map119_Chr5 % (NesHeader.byVRomSize<<3));
        if(Map119_Chr6&0x40)    PPUBANK[ 6 ] = CRAMPAGE(Map119_Chr6&0x07);
        else            PPUBANK[ 6 ] = VROMPAGE(Map119_Chr6 % (NesHeader.byVRomSize<<3));
        if(Map119_Chr7&0x40)    PPUBANK[ 7 ] = CRAMPAGE(Map119_Chr7&0x07);
        else            PPUBANK[ 7 ] = VROMPAGE(Map119_Chr7 % (NesHeader.byVRomSize<<3));
    }
    InfoNES_SetupChr();
}

