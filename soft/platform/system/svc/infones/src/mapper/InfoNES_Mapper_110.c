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







BYTE    Map110_Reg0, Map110_Reg1;




void Map110_Init()
{

    MapperInit = Map110_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map110_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    Map110_Reg0 = 0;
    Map110_Reg1 = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map110_Apu( WORD wAddr, BYTE byData )
{
    switch( wAddr )
    {
        case  0x4100:
            Map110_Reg1 = byData & 0x07;
            break;
        case  0x4101:
            switch( Map110_Reg1 )
            {
                case    5:
                    ROMBANK0 = ROMPAGE( ((byData << 2) + 0) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK1 = ROMPAGE( ((byData << 2) + 1) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK2 = ROMPAGE( ((byData << 2) + 2) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK3 = ROMPAGE( ((byData << 2) + 3) % ( NesHeader.byRomSize << 1 ) );
                    break;

                case    0:
                    Map110_Reg0 = byData & 0x01;
                    PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;

                case    2:
                    Map110_Reg0 = byData;
                    PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;

                case    4:
                    Map110_Reg0 = Map110_Reg0 | (byData<<1);
                    PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;

                case    6:
                    Map110_Reg0 = Map110_Reg0 | (byData<<2);
                    PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.byVRomSize<<3));
                    PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.byVRomSize<<3));
                    InfoNES_SetupChr();
                    break;

                default:
                    break;
            }
            break;
        default:
            break;
    }
}
