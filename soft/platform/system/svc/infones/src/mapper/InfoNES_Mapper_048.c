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







BYTE Map48_Regs[ 1 ];
BYTE Map48_IRQ_Enable;
BYTE Map48_IRQ_Cnt;




void Map48_Init()
{

    MapperInit = Map48_Init;


    MapperWrite = Map48_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map48_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


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


    Map48_Regs[ 0 ] = 0;
    Map48_IRQ_Enable = 0;
    Map48_IRQ_Cnt = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map48_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:

            if ( !Map48_Regs[ 0 ] )
            {
                if ( byData & 0x40 )
                {
                    InfoNES_Mirroring( 0 );
                }
                else
                {
                    InfoNES_Mirroring( 1 );
                }
            }

            ROMBANK0 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8001:

            ROMBANK1 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
            break;


        case 0x8002:
            PPUBANK[ 0 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( ( ( byData << 1 ) + 1 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0x8003:
            PPUBANK[ 2 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( ( ( byData << 1 ) + 1 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa000:
            PPUBANK[ 4 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa001:
            PPUBANK[ 5 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa002:
            PPUBANK[ 6 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xa003:
            PPUBANK[ 7 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc000:
            Map48_IRQ_Cnt = byData;
            break;

        case 0xc001:
            Map48_IRQ_Enable = byData & 0x01;
            break;

        case 0xe000:

            if ( byData & 0x40 )
            {
                InfoNES_Mirroring( 0 );
            }
            else
            {
                InfoNES_Mirroring( 1 );
            }
            Map48_Regs[ 0 ] = 1;
            break;
    }
}




void Map48_HSync()
{

    if ( Map48_IRQ_Enable )
    {
        if (  PPU_Scanline <= 239 )
        {
            if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
            {
                if ( Map48_IRQ_Cnt == 0xff )
                {
                    IRQ_REQ;
                    Map48_IRQ_Enable = 0;
                }
                else
                {
                    Map48_IRQ_Cnt++;
                }
            }
        }
    }
}
