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







BYTE    Map191_Reg[8];
BYTE    Map191_Prg0, Map191_Prg1;
BYTE    Map191_Chr0, Map191_Chr1, Map191_Chr2, Map191_Chr3;
BYTE    Map191_Highbank;




void Map191_Init()
{

    MapperInit = Map191_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map191_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 8; i++ )
    {
        Map191_Reg[i] = 0x00;
    }


    Map191_Prg0 = 0;
    Map191_Set_CPU_Banks();


    Map191_Chr0 = 0;
    Map191_Chr1 = 0;
    Map191_Chr2 = 0;
    Map191_Chr3 = 0;
    Map191_Highbank = 0;
    Map191_Set_PPU_Banks();


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map191_Apu( WORD wAddr, BYTE byData )
{
    switch( wAddr )
    {
        case  0x4100:
            Map191_Reg[0]=byData;
            break;
        case  0x4101:
            Map191_Reg[1]=byData;
            switch( Map191_Reg[0] )
            {
                case    0:
                    Map191_Chr0=byData&7;
                    Map191_Set_PPU_Banks();
                    break;
                case    1:
                    Map191_Chr1=byData&7;
                    Map191_Set_PPU_Banks();
                    break;
                case    2:
                    Map191_Chr2=byData&7;
                    Map191_Set_PPU_Banks();
                    break;
                case    3:
                    Map191_Chr3=byData&7;
                    Map191_Set_PPU_Banks();
                    break;
                case    4:
                    Map191_Highbank=byData&7;
                    Map191_Set_PPU_Banks();
                    break;
                case    5:
                    Map191_Prg0=byData&7;
                    Map191_Set_CPU_Banks();
                    break;
                case    7:
                    if( byData & 0x02 ) InfoNES_Mirroring( 0 );
                    else        InfoNES_Mirroring( 1 );
                    break;
            }
            break;
    }
}




void    Map191_Set_CPU_Banks()
{
    ROMBANK0 = ROMPAGE( ((Map191_Prg0<<2) + 0 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( ((Map191_Prg0<<2) + 1 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( ((Map191_Prg0<<2) + 2 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( ((Map191_Prg0<<2) + 3 ) % ( NesHeader.byRomSize << 1 ) );
};




void    Map191_Set_PPU_Banks()
{
    if ( NesHeader.byVRomSize > 0 )
    {
        PPUBANK[ 0 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 1 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+1 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 2 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+2 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 3 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+3 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 4 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+0 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 5 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+1 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 6 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+2 ) % ( NesHeader.byVRomSize << 3 ) );
        PPUBANK[ 7 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+3 ) % ( NesHeader.byVRomSize << 3 ) );
        InfoNES_SetupChr();
    }
}
