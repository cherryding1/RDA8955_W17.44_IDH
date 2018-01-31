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







BYTE  Map16_Regs[3];

BYTE  Map16_IRQ_Enable;
DWORD Map16_IRQ_Cnt;
DWORD Map16_IRQ_Latch;




void Map16_Init()
{

    MapperInit = Map16_Init;


    MapperWrite = Map16_Write;


    MapperSram = Map16_Write;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map16_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    Map16_Regs[ 0 ] = 0;
    Map16_Regs[ 1 ] = 0;
    Map16_Regs[ 2 ] = 0;

    Map16_IRQ_Enable = 0;
    Map16_IRQ_Cnt = 0;
    Map16_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map16_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0x000f )
    {
        case 0x0000:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 0 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0001:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 1 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0002:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 2 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0003:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 3 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0004:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 4 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0005:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 5 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0006:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 6 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0007:
            byData %= ( NesHeader.byVRomSize << 3 );
            PPUBANK[ 7 ] = VROMPAGE( byData );
            InfoNES_SetupChr();
            break;

        case 0x0008:
            byData <<= 1;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;

        case 0x0009:
            switch ( byData & 0x03 )
            {
                case 0x00:
                    InfoNES_Mirroring( 1 );
                    break;

                case 0x01:
                    InfoNES_Mirroring( 0 );
                    break;

                case 0x02:
                    InfoNES_Mirroring( 3 );
                    break;

                case 0x03:
                    InfoNES_Mirroring( 2 );
                    break;
            }
            break;

        case 0x000a:
            Map16_IRQ_Enable = byData & 0x01;
            Map16_IRQ_Cnt = Map16_IRQ_Latch;
            break;

        case 0x000b:
            Map16_IRQ_Latch = ( Map16_IRQ_Latch & 0xff00 ) | byData;
            break;

        case 0x000c:
            Map16_IRQ_Latch = ( (DWORD)byData << 8 ) | ( Map16_IRQ_Latch & 0x00ff );
            break;

        case 0x000d:

            break;
    }
}




void Map16_HSync()
{
    if ( Map16_IRQ_Enable )
    {

        if ( Map16_IRQ_Cnt <= 114 )
        {
            IRQ_REQ;
            Map16_IRQ_Cnt = 0;
            Map16_IRQ_Enable = 0;
        }
        else
        {
            Map16_IRQ_Cnt -= 114;
        }
    }
}
