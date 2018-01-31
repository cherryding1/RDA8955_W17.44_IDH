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







BYTE    Map252_Reg[9];
BYTE    Map252_IRQ_Enable;
BYTE    Map252_IRQ_Counter;
BYTE    Map252_IRQ_Latch;
BYTE    Map252_IRQ_Occur;
int Map252_IRQ_Clock;




void Map252_Init()
{

    MapperInit = Map252_Init;


    MapperWrite = Map252_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map252_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 9; i++ )
    {
        Map252_Reg[i] = i;
    }

    Map252_IRQ_Enable = 0;
    Map252_IRQ_Counter = 0;
    Map252_IRQ_Latch = 0;
    Map252_IRQ_Clock = 0;
    Map252_IRQ_Occur = 0;


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




void Map252_Write( WORD wAddr, BYTE byData )
{
    if( (wAddr & 0xF000) == 0x8000 )
    {
        ROMBANK0 = ROMPAGE( byData % (NesHeader.byRomSize<<1));
        return;
    }
    if( (wAddr & 0xF000) == 0xA000 )
    {
        ROMBANK1 = ROMPAGE( byData % (NesHeader.byRomSize<<1));
        return;
    }
    switch( wAddr & 0xF00C )
    {
        case 0xB000:
            Map252_Reg[0] = (Map252_Reg[0] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 0 ] = VROMPAGE(Map252_Reg[0] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xB004:
            Map252_Reg[0] = (Map252_Reg[0] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 0 ] = VROMPAGE(Map252_Reg[0] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xB008:
            Map252_Reg[1] = (Map252_Reg[1] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 1 ] = VROMPAGE(Map252_Reg[1] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xB00C:
            Map252_Reg[1] = (Map252_Reg[1] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 1 ] = VROMPAGE(Map252_Reg[1] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;

        case 0xC000:
            Map252_Reg[2] = (Map252_Reg[2] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 2 ] = VROMPAGE(Map252_Reg[2] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xC004:
            Map252_Reg[2] = (Map252_Reg[2] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 2 ] = VROMPAGE(Map252_Reg[2] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xC008:
            Map252_Reg[3] = (Map252_Reg[3] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 3 ] = VROMPAGE(Map252_Reg[3] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xC00C:
            Map252_Reg[3] = (Map252_Reg[3] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 3 ] = VROMPAGE(Map252_Reg[3] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;

        case 0xD000:
            Map252_Reg[4] = (Map252_Reg[4] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 4 ] = VROMPAGE(Map252_Reg[4] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xD004:
            Map252_Reg[4] = (Map252_Reg[4] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 4 ] = VROMPAGE(Map252_Reg[4] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xD008:
            Map252_Reg[5] = (Map252_Reg[5] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 5 ] = VROMPAGE(Map252_Reg[5] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xD00C:
            Map252_Reg[5] = (Map252_Reg[5] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 5 ] = VROMPAGE(Map252_Reg[5] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;

        case 0xE000:
            Map252_Reg[6] = (Map252_Reg[6] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 6 ] = VROMPAGE(Map252_Reg[6] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xE004:
            Map252_Reg[6] = (Map252_Reg[6] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 6 ] = VROMPAGE(Map252_Reg[6] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xE008:
            Map252_Reg[7] = (Map252_Reg[7] & 0xF0) | (byData & 0x0F);
            PPUBANK[ 7 ] = VROMPAGE(Map252_Reg[7] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;
        case 0xE00C:
            Map252_Reg[7] = (Map252_Reg[7] & 0x0F) | ((byData & 0x0F) << 4);
            PPUBANK[ 7 ] = VROMPAGE(Map252_Reg[7] % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();
            break;

        case 0xF000:
            Map252_IRQ_Latch = (Map252_IRQ_Latch & 0xF0) | (byData & 0x0F);
            Map252_IRQ_Occur = 0;
            break;
        case 0xF004:
            Map252_IRQ_Latch = (Map252_IRQ_Latch & 0x0F) | ((byData & 0x0F) << 4);
            Map252_IRQ_Occur = 0;
            break;

        case 0xF008:
            Map252_IRQ_Enable = byData & 0x03;
            if( Map252_IRQ_Enable & 0x02 )
            {
                Map252_IRQ_Counter = Map252_IRQ_Latch;
                Map252_IRQ_Clock = 0;
            }
            Map252_IRQ_Occur = 0;
            break;

        case 0xF00C:
            Map252_IRQ_Enable = (Map252_IRQ_Enable & 0x01) * 3;
            Map252_IRQ_Occur = 0;
            break;
    }
}




void Map252_HSync()
{
    if( Map252_IRQ_Enable & 0x02 )
    {
        if( Map252_IRQ_Counter == 0xFF )
        {
            Map252_IRQ_Occur = 0xFF;
            Map252_IRQ_Counter = Map252_IRQ_Latch;
            Map252_IRQ_Enable = (Map252_IRQ_Enable & 0x01) * 3;
        }
        else
        {
            Map252_IRQ_Counter++;
        }
    }
    if( Map252_IRQ_Occur )
    {
        IRQ_REQ;
    }
}

