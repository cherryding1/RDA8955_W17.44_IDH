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







BYTE    Map109_Reg;
BYTE    Map109_Chr0, Map109_Chr1, Map109_Chr2, Map109_Chr3;
BYTE    Map109_Chrmode0, Map109_Chrmode1;




void Map109_Init()
{

    MapperInit = Map109_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map109_Apu;


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


    Map109_Reg = 0;
    Map109_Chr0 = 0;
    Map109_Chr1 = 0;
    Map109_Chr2 = 0;
    Map109_Chr3 = 0;
    Map109_Chrmode0 = 0;
    Map109_Chrmode1 = 0;


    Map109_Set_PPU_Banks();


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map109_Apu( WORD wAddr, BYTE byData )
{
    switch( wAddr )
    {
        case  0x4100:
            Map109_Reg = byData;
            break;
        case  0x4101:
            switch( Map109_Reg )
            {
                case 0:
                    Map109_Chr0 = byData;
                    Map109_Set_PPU_Banks();
                    break;
                case 1:
                    Map109_Chr1 = byData;
                    Map109_Set_PPU_Banks();
                    break;
                case 2:
                    Map109_Chr2 = byData;
                    Map109_Set_PPU_Banks();
                    break;
                case 3:
                    Map109_Chr3 = byData;
                    Map109_Set_PPU_Banks();
                    break;
                case 4:
                    Map109_Chrmode0 = byData & 0x01;
                    Map109_Set_PPU_Banks();
                    break;
                case 5:
                    ROMBANK0 = ROMPAGE( ( ( byData & 0x07 ) + 0 ) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK1 = ROMPAGE( ( ( byData & 0x07 ) + 1 ) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK2 = ROMPAGE( ( ( byData & 0x07 ) + 2 ) % ( NesHeader.byRomSize << 1 ) );
                    ROMBANK3 = ROMPAGE( ( ( byData & 0x07 ) + 3 ) % ( NesHeader.byRomSize << 1 ) );
                    break;
                case 6:
                    Map109_Chrmode1 = byData & 0x07;
                    Map109_Set_PPU_Banks();
                    break;
                case 7:
                    if( byData & 0x01 ) InfoNES_Mirroring( 0 );
                    else        InfoNES_Mirroring( 1 );
                    break;
            }
            break;
    }
}




void Map109_Set_PPU_Banks()
{
    if ( NesHeader.byVRomSize > 0 )
    {
        PPUBANK[ 0 ] = VROMPAGE((Map109_Chr0) % (NesHeader.byVRomSize<<3) );
        PPUBANK[ 1 ] = VROMPAGE((Map109_Chr1|((Map109_Chrmode1<<3)&0x8)) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 2 ] = VROMPAGE((Map109_Chr2|((Map109_Chrmode1<<2)&0x8)) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 3 ] = VROMPAGE((Map109_Chr3|((Map109_Chrmode1<<1)&0x8)|(Map109_Chrmode0*0x10)) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 4 ] = VROMPAGE((NesHeader.byVRomSize<<3)-4);
        PPUBANK[ 5 ] = VROMPAGE((NesHeader.byVRomSize<<3)-3);
        PPUBANK[ 6 ] = VROMPAGE((NesHeader.byVRomSize<<3)-2);
        PPUBANK[ 7 ] = VROMPAGE((NesHeader.byVRomSize<<3)-1);
        InfoNES_SetupChr();
    }
}
