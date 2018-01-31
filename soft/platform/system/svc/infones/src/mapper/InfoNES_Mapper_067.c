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







BYTE Map67_IRQ_Enable;
BYTE Map67_IRQ_Cnt;
BYTE Map67_IRQ_Latch;




void Map67_Init()
{

    MapperInit = Map67_Init;


    MapperWrite = Map67_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map67_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    PPUBANK[ 0 ] = VROMPAGE( 0 );
    PPUBANK[ 1 ] = VROMPAGE( 1 );
    PPUBANK[ 2 ] = VROMPAGE( 2 );
    PPUBANK[ 3 ] = VROMPAGE( 3 );
    PPUBANK[ 4 ] = VROMPAGE( ( NesHeader.byVRomSize << 3 ) - 4 );
    PPUBANK[ 5 ] = VROMPAGE( ( NesHeader.byVRomSize << 3 ) - 3 );
    PPUBANK[ 6 ] = VROMPAGE( ( NesHeader.byVRomSize << 3 ) - 2 );
    PPUBANK[ 7 ] = VROMPAGE( ( NesHeader.byVRomSize << 3 ) - 1 );
    InfoNES_SetupChr();


    Map67_IRQ_Enable = 0;
    Map67_IRQ_Cnt = 0;
    Map67_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map67_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf800 )
    {

        case 0x8800:
            byData <<= 1;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 0 ] = VROMPAGE( byData + 0 );
            PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0x9800:
            byData <<= 1;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 2 ] = VROMPAGE( byData + 0 );
            PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0xa800:
            byData <<= 1;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 4 ] = VROMPAGE( byData + 0 );
            PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0xb800:
            byData <<= 1;
            byData %= ( NesHeader.byVRomSize << 3 );

            PPUBANK[ 6 ] = VROMPAGE( byData + 0 );
            PPUBANK[ 7 ] = VROMPAGE( byData + 1 );
            InfoNES_SetupChr();
            break;

        case 0xc800:
            Map67_IRQ_Cnt = Map67_IRQ_Latch;
            Map67_IRQ_Latch = byData;
            break;

        case 0xd800:
            Map67_IRQ_Enable = byData & 0x10;
            break;

        case 0xe800:
            switch ( byData & 0x03 )
            {
                case 0:
                    InfoNES_Mirroring( 1 );
                    break;
                case 1:
                    InfoNES_Mirroring( 0 );
                    break;
                case 2:
                    InfoNES_Mirroring( 3 );
                    break;
                case 3:
                    InfoNES_Mirroring( 2 );
                    break;
            }
            break;


        case 0xf800:
            byData <<= 1;
            byData %= ( NesHeader.byRomSize << 1 );

            ROMBANK0 = ROMPAGE( byData + 0 );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;
    }
}




void Map67_HSync()
{

    if ( Map67_IRQ_Enable )
    {
        if (  PPU_Scanline <= 239 )
        {
            if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
            {
                if ( --Map67_IRQ_Cnt == 0xf6 )
                {
                    Map67_IRQ_Cnt = Map67_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}
