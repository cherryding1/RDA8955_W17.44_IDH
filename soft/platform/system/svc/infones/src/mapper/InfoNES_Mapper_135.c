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







BYTE    Map135_Cmd;
BYTE    Map135_Chr0l, Map135_Chr1l, Map135_Chr0h, Map135_Chr1h, Map135_Chrch;




void Map135_Init()
{

    MapperInit = Map135_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map135_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map135_Cmd = 0;
    Map135_Chr0l = Map135_Chr1l = Map135_Chr0h = Map135_Chr1h = Map135_Chrch = 0;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    Map135_Set_PPU_Banks();


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map135_Apu( WORD wAddr, BYTE byData )
{
    switch( wAddr & 0x4101 )
    {
        case  0x4100:
            Map135_Cmd = byData & 0x07;
            break;
        case  0x4101:
            switch( Map135_Cmd )
            {
                case    0:
                    Map135_Chr0l = byData & 0x07;
                    Map135_Set_PPU_Banks();
                    break;
                case    1:
                    Map135_Chr0h = byData & 0x07;
                    Map135_Set_PPU_Banks();
                    break;
                case    2:
                    Map135_Chr1l = byData & 0x07;
                    Map135_Set_PPU_Banks();
                    break;
                case    3:
                    Map135_Chr1h = byData & 0x07;
                    Map135_Set_PPU_Banks();
                    break;
                case    4:
                    Map135_Chrch = byData & 0x07;
                    Map135_Set_PPU_Banks();
                    break;
                case    5:

                    ROMBANK0 = ROMPAGE( (((byData%0x07)<<2) + 0 ) % (NesHeader.byRomSize << 1) );
                    ROMBANK1 = ROMPAGE( (((byData%0x07)<<2) + 1 ) % (NesHeader.byRomSize << 1) );
                    ROMBANK2 = ROMPAGE( (((byData%0x07)<<2) + 2 ) % (NesHeader.byRomSize << 1) );
                    ROMBANK3 = ROMPAGE( (((byData%0x07)<<2) + 3 ) % (NesHeader.byRomSize << 1) );
                    break;
                case    6:
                    break;
                case    7:
                    switch( (byData>>1)&0x03 )
                    {
                        case  0: InfoNES_Mirroring( 2 ); break;
                        case  1: InfoNES_Mirroring( 0  ); break;
                        case  2: InfoNES_Mirroring( 1  ); break;
                        case  3: InfoNES_Mirroring( 2 ); break;
                    }
                    break;
            }
            break;
    }
}




void    Map135_Set_PPU_Banks()
{

    PPUBANK[ 0 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 1 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 2 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 3 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 4 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 5 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 6 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.byVRomSize << 3) );
    PPUBANK[ 7 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.byVRomSize << 3) );
    InfoNES_SetupChr();
}
