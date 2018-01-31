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







BYTE    Map249_Spdata;
BYTE    Map249_Reg[8];

BYTE    Map249_IRQ_Enable;
BYTE    Map249_IRQ_Counter;
BYTE    Map249_IRQ_Latch;
BYTE    Map249_IRQ_Request;




void Map249_Init()
{

    MapperInit = Map249_Init;


    MapperWrite = Map249_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map249_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map249_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 8; i++ )
    {
        Map249_Reg[i] = 0x00;
    }


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


    Map249_IRQ_Enable = 0;      Map249_IRQ_Counter = 0;
    Map249_IRQ_Latch = 0;
    Map249_IRQ_Request = 0;

    Map249_Spdata = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map249_Write( WORD wAddr, BYTE byData )
{
    BYTE  byM0,byM1,byM2,byM3,byM4,byM5,byM6,byM7;

    switch( wAddr&0xFF01 )
    {
        case    0x8000:
        case    0x8800:
            Map249_Reg[0] = byData;
            break;
        case    0x8001:
        case    0x8801:
            switch( Map249_Reg[0] & 0x07 )
            {
                case    0x00:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 0 ] = VROMPAGE((byData&0xFE) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 1 ] = VROMPAGE((byData|0x01) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x01:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 2 ] = VROMPAGE((byData&0xFE) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 3 ] = VROMPAGE((byData|0x01) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x02:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 4 ] = VROMPAGE(byData % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x03:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 5 ] = VROMPAGE(byData % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x04:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 6 ] = VROMPAGE(byData % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x05:
                    if( Map249_Spdata == 1 )
                    {
                        byM0=byData&0x1;
                        byM1=(byData&0x02)>>1;
                        byM2=(byData&0x04)>>2;
                        byM3=(byData&0x08)>>3;
                        byM4=(byData&0x10)>>4;
                        byM5=(byData&0x20)>>5;
                        byM6=(byData&0x40)>>6;
                        byM7=(byData&0x80)>>7;
                        byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                    }
                    PPUBANK[ 7 ] = VROMPAGE(byData % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;
                case    0x06:
                    if( Map249_Spdata == 1 )
                    {
                        if( byData < 0x20 )
                        {
                            byM0=byData&0x1;
                            byM1=(byData&0x02)>>1;
                            byM2=(byData&0x04)>>2;
                            byM3=(byData&0x08)>>3;
                            byM4=(byData&0x10)>>4;
                            byM5=0;
                            byM6=0;
                            byM7=0;
                            byData=(byM7<<7)|(byM6<<6)|(byM5<<5)|(byM2<<4)|(byM1<<3)|(byM3<<2)|(byM4<<1)|byM0;
                        }
                        else
                        {
                            byData=byData-0x20;
                            byM0=byData&0x1;
                            byM1=(byData&0x02)>>1;
                            byM2=(byData&0x04)>>2;
                            byM3=(byData&0x08)>>3;
                            byM4=(byData&0x10)>>4;
                            byM5=(byData&0x20)>>5;
                            byM6=(byData&0x40)>>6;
                            byM7=(byData&0x80)>>7;
                            byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                        }
                    }
                    ROMBANK0 = ROMPAGE(byData % (NesHeader.byRomSize<<1));
                    break;
                case    0x07:
                    if( Map249_Spdata == 1 )
                    {
                        if( byData < 0x20 )
                        {
                            byM0=byData&0x1;
                            byM1=(byData&0x02)>>1;
                            byM2=(byData&0x04)>>2;
                            byM3=(byData&0x08)>>3;
                            byM4=(byData&0x10)>>4;
                            byM5=0;
                            byM6=0;
                            byM7=0;
                            byData=(byM7<<7)|(byM6<<6)|(byM5<<5)|(byM2<<4)|(byM1<<3)|(byM3<<2)|(byM4<<1)|byM0;
                        }
                        else
                        {
                            byData=byData-0x20;
                            byM0=byData&0x1;
                            byM1=(byData&0x02)>>1;
                            byM2=(byData&0x04)>>2;
                            byM3=(byData&0x08)>>3;
                            byM4=(byData&0x10)>>4;
                            byM5=(byData&0x20)>>5;
                            byM6=(byData&0x40)>>6;
                            byM7=(byData&0x80)>>7;
                            byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
                        }
                    }
                    ROMBANK1 = ROMPAGE(byData % (NesHeader.byRomSize<<1));
                    break;
            }
            break;
        case    0xA000:
        case    0xA800:
            Map249_Reg[2] = byData;
            if( !ROM_FourScr )
            {
                if( byData & 0x01 ) InfoNES_Mirroring( 0 );
                else        InfoNES_Mirroring( 1 );
            }
            break;
        case    0xA001:
        case    0xA801:
            Map249_Reg[3] = byData;
            break;
        case    0xC000:
        case    0xC800:
            Map249_Reg[4] = byData;
            Map249_IRQ_Counter = byData;
            Map249_IRQ_Request = 0;
            break;
        case    0xC001:
        case    0xC801:
            Map249_Reg[5] = byData;
            Map249_IRQ_Latch = byData;
            Map249_IRQ_Request = 0;
            break;
        case    0xE000:
        case    0xE800:
            Map249_Reg[6] = byData;
            Map249_IRQ_Enable = 0;
            Map249_IRQ_Request = 0;
            break;
        case    0xE001:
        case    0xE801:
            Map249_Reg[7] = byData;
            Map249_IRQ_Enable = 1;
            Map249_IRQ_Request = 0;
            break;
    }
}




void Map249_Apu( WORD wAddr, BYTE byData )
{
    if( wAddr == 0x5000 )
    {
        switch( byData )
        {
            case    0x00:
                Map249_Spdata = 0;
                break;
            case    0x02:
                Map249_Spdata = 1;
                break;
        }
    }
}




void Map249_HSync()
{
    if( (  PPU_Scanline <= 239) )
    {
        if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
        {
            if( Map249_IRQ_Enable && !Map249_IRQ_Request )
            {
                if( PPU_Scanline == 0 )
                {
                    if( Map249_IRQ_Counter )
                    {
                        Map249_IRQ_Counter--;
                    }
                }
                if( !(Map249_IRQ_Counter--) )
                {
                    Map249_IRQ_Request = 0xFF;
                    Map249_IRQ_Counter = Map249_IRQ_Latch;
                }
            }
        }
    }
    if( Map249_IRQ_Request )
    {
        IRQ_REQ;
    }
}

