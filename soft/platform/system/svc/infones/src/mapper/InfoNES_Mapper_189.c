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







BYTE Map189_Regs[ 1 ];
BYTE Map189_IRQ_Cnt;
BYTE Map189_IRQ_Latch;
BYTE Map189_IRQ_Enable;




void Map189_Init()
{

    MapperInit = Map189_Init;


    MapperWrite = Map189_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map189_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map189_HSync;


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
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }


    Map189_IRQ_Cnt = 0;
    Map189_IRQ_Latch = 0;
    Map189_IRQ_Enable = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map189_Apu( WORD wAddr, BYTE byData )
{
    if ( wAddr >= 0x4100 && wAddr <= 0x41FF )
    {
        byData = ( byData & 0x30 ) >> 4;
        ROMBANK0 = ROMPAGE( ( byData * 4 + 0 ) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK1 = ROMPAGE( ( byData * 4 + 1 ) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK2 = ROMPAGE( ( byData * 4 + 2 ) % ( NesHeader.byRomSize << 1 ) );
        ROMBANK3 = ROMPAGE( ( byData * 4 + 3 ) % ( NesHeader.byRomSize << 1 ) );
    }
}




void Map189_Write( WORD wAddr, BYTE byData )
{

    switch( wAddr )
    {
        case 0x8000:
            Map189_Regs[0] = byData;
            break;

        case 0x8001:
            switch( Map189_Regs[0] )
            {
                case 0x40:
                    PPUBANK[ 0 ] = VROMPAGE( ( byData + 0 ) % ( NesHeader.byVRomSize << 3 ) );
                    PPUBANK[ 1 ] = VROMPAGE( ( byData + 1 ) % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x41:
                    PPUBANK[ 2 ] = VROMPAGE( ( byData + 0 ) % ( NesHeader.byVRomSize << 3 ) );
                    PPUBANK[ 3 ] = VROMPAGE( ( byData + 1 ) % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x42:
                    PPUBANK[ 4 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x43:
                    PPUBANK[ 5 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x44:
                    PPUBANK[ 6 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x45:
                    PPUBANK[ 7 ] = VROMPAGE( byData % ( NesHeader.byVRomSize << 3 ) );
                    InfoNES_SetupChr();
                    break;

                case 0x46:
                    ROMBANK2 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
                    break;

                case 0x47:
                    ROMBANK1 = ROMPAGE( byData % ( NesHeader.byRomSize << 1 ) );
                    break;
            }
            break;

        case 0xC000:
            Map189_IRQ_Cnt = byData;
            break;

        case 0xC001:
            Map189_IRQ_Latch = byData;
            break;

        case 0xE000:
            Map189_IRQ_Enable = 0;
            break;

        case 0xE001:
            Map189_IRQ_Enable = 1;
            break;
    }
}




void Map189_HSync()
{

    if ( Map189_IRQ_Enable )
    {
        if (  PPU_Scanline <= 239 )
        {
            if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
            {
                if ( !( --Map189_IRQ_Cnt ) )
                {
                    Map189_IRQ_Cnt = Map189_IRQ_Latch;
                    IRQ_REQ;
                }
            }
        }
    }
}
