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







BYTE Map46_Regs[ 4 ];




void Map46_Init()
{

    MapperInit = Map46_Init;


    MapperWrite = Map46_Write;


    MapperSram = Map46_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map46_Regs[ 0 ] = Map46_Regs[ 1 ] = Map46_Regs[ 2 ] = Map46_Regs[ 3 ] = 0;
    Map46_Set_ROM_Banks();


    InfoNES_Mirroring( 1 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map46_Sram( WORD wAddr, BYTE byData )
{

    Map46_Regs[ 0 ] = byData & 0x0f;
    Map46_Regs[ 1 ] = ( byData & 0xf0 ) >> 4;
    Map46_Set_ROM_Banks();
}




void Map46_Write( WORD wAddr, BYTE byData )
{

    Map46_Regs[ 2 ] = byData & 0x01;
    Map46_Regs[ 3 ] = ( byData & 0x70 ) >> 4;
    Map46_Set_ROM_Banks();
}




void Map46_Set_ROM_Banks()
{

    ROMBANK0 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 0 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 1 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 2 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 3 ) % ( NesHeader.byRomSize << 1 ) );


    PPUBANK[ 0 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 0 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 1 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 2 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 3 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 4 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 5 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 6 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 7 ) % ( NesHeader.byVRomSize << 3 ) );
    InfoNES_SetupChr();
}
