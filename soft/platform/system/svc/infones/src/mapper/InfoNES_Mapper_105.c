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







BYTE    Map105_Init_State;
BYTE    Map105_Write_Count;
BYTE    Map105_Bits;
BYTE    Map105_Reg[4];

BYTE    Map105_IRQ_Enable;
int Map105_IRQ_Counter;




void Map105_Init()
{

    MapperInit = Map105_Init;


    MapperWrite = Map105_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map105_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    Map105_Reg[0] = 0x0C;
    Map105_Reg[1] = 0x00;
    Map105_Reg[2] = 0x00;
    Map105_Reg[3] = 0x10;

    Map105_Bits = 0;
    Map105_Write_Count = 0;

    Map105_IRQ_Counter = 0;
    Map105_IRQ_Enable = 0;
    Map105_Init_State = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map105_Write( WORD wAddr, BYTE byData )
{
    WORD reg_num = (wAddr & 0x7FFF) >> 13;

    if( byData & 0x80 )
    {
        Map105_Bits = Map105_Write_Count = 0;
        if( reg_num == 0 )
        {
            Map105_Reg[reg_num] |= 0x0C;
        }
    }
    else
    {
        Map105_Bits |= (byData & 1) << Map105_Write_Count++;
        if( Map105_Write_Count == 5)
        {
            Map105_Reg[reg_num] = Map105_Bits & 0x1F;
            Map105_Bits = Map105_Write_Count = 0;
        }
    }

    if( Map105_Reg[0] & 0x02 )
    {
        if( Map105_Reg[0] & 0x01 )
        {
            InfoNES_Mirroring( 0 );
        }
        else
        {
            InfoNES_Mirroring( 1 );
        }
    }
    else
    {
        if( Map105_Reg[0] & 0x01 )
        {
            InfoNES_Mirroring( 3 );
        }
        else
        {
            InfoNES_Mirroring( 2 );
        }
    }

    switch( Map105_Init_State )
    {
        case 0:
        case 1:
            Map105_Init_State++;
            break;

        case 2:
            if(Map105_Reg[1] & 0x08)
            {
                if (Map105_Reg[0] & 0x08)
                {
                    if (Map105_Reg[0] & 0x04)
                    {
                        ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK2 = ROMPAGE( 30 % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK3 = ROMPAGE( 31 % ( NesHeader.byRomSize << 1 ) );
                    }
                    else
                    {
                        ROMBANK0 = ROMPAGE( 16 % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK1 = ROMPAGE( 17 % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( NesHeader.byRomSize << 1 ) );
                        ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( NesHeader.byRomSize << 1 ) );
                    }
                }
                else
                {
                    ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 16) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 17) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 18) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 19) % ( NesHeader.byRomSize << 1 ) );
                }
            }
            else
            {
                ROMBANK0 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 0) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK1 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 1) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK2 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 2) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK3 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 3) % ( NesHeader.byRomSize << 1 ) );
            }

            if( Map105_Reg[1] & 0x10 )
            {
                Map105_IRQ_Counter = 0;
                Map105_IRQ_Enable = 0;
            }
            else
            {
                Map105_IRQ_Enable = 1;
            }
            break;

        default:
            break;
    }
}




void Map105_HSync()
{
    if( !PPU_Scanline )
    {
        if( Map105_IRQ_Enable )
        {
            Map105_IRQ_Counter += 29781;
        }
        if( ((Map105_IRQ_Counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000 )
        {
            IRQ_REQ;
        }
    }
}

