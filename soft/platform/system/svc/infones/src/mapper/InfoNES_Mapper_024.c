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







BYTE Map24_IRQ_Count;
BYTE Map24_IRQ_State;
BYTE Map24_IRQ_Latch;




void Map24_Init()
{

    MapperInit = Map24_Init;


    MapperWrite = Map24_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map24_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map24_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:

            ROMBANK0 = ROMPAGE( ( byData + 0 ) % ( NesHeader.byRomSize << 1) );
            ROMBANK1 = ROMPAGE( ( byData + 1 ) % ( NesHeader.byRomSize << 1) );
            break;

        case 0xb003:

            switch ( byData & 0x0c )
            {
                case 0x00:
                    InfoNES_Mirroring( 1 );
                    break;
                case 0x04:
                    InfoNES_Mirroring( 0 );
                    break;
                case 0x08:
                    InfoNES_Mirroring( 3 );
                    break;
                case 0x0c:
                    InfoNES_Mirroring( 2 );
                    break;
            }
            break;

        case 0xC000:
            ROMBANK2 = ROMPAGE( byData % ( NesHeader.byRomSize << 1) );
            break;

        case 0xD000:
            PPUBANK[ 0 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD001:
            PPUBANK[ 1 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD002:
            PPUBANK[ 2 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD003:
            PPUBANK[ 3 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xE000:
            PPUBANK[ 4 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xE001:
            PPUBANK[ 5 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xE002:
            PPUBANK[ 6 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xE003:
            PPUBANK[ 7 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xF000:
            Map24_IRQ_Latch = byData;
            break;

        case 0xF001:
            Map24_IRQ_State = byData & 0x03;
            if(Map24_IRQ_State & 0x02)
            {
                Map24_IRQ_Count = Map24_IRQ_Latch;
            }
            break;

        case 0xF002:
            if(Map24_IRQ_State & 0x01)
            {
                Map24_IRQ_State |= 0x02;
            }
            else
            {
                Map24_IRQ_State &= 0x01;
            }
            break;
    }
}




void Map24_HSync()
{

    if(Map24_IRQ_State & 0x02)
    {
        if(Map24_IRQ_Count == 0xFF)
        {
            IRQ_REQ;
            Map24_IRQ_Count = Map24_IRQ_Latch;
        }
        else
        {
            Map24_IRQ_Count++;
        }
    }
}
