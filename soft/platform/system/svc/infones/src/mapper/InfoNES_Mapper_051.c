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







int     Map51_Mode, Map51_Bank;




void Map51_Init()
{

    MapperInit = Map51_Init;


    MapperWrite = Map51_Write;


    MapperSram = Map51_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map51_Bank = 0;
    Map51_Mode = 1;


    Map51_Set_CPU_Banks();


    for ( int nPage = 0; nPage < 8; ++nPage )
        PPUBANK[ nPage ] = CRAMPAGE( nPage );
    InfoNES_SetupChr();


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map51_Write( WORD wAddr, BYTE byData )
{
    Map51_Bank = (byData & 0x0f) << 2;
    if( 0xC000 <= wAddr && wAddr <= 0xDFFF )
    {
        Map51_Mode = (Map51_Mode & 0x01) | ((byData & 0x10) >> 3);
    }
    Map51_Set_CPU_Banks();
}




void Map51_Sram( WORD wAddr, BYTE byData )
{
    if( wAddr>=0x6000 )
    {
        Map51_Mode = ((byData & 0x10) >> 3) | ((byData & 0x02) >> 1);
        Map51_Set_CPU_Banks();
    }
}




void Map51_Set_CPU_Banks()
{
    switch(Map51_Mode)
    {
        case 0:
            InfoNES_Mirroring( 1 );
            SRAMBANK = ROMPAGE((Map51_Bank|0x2c|3) % (NesHeader.byRomSize<<1));
            ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((Map51_Bank|0x0c|2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((Map51_Bank|0x0c|3) % (NesHeader.byRomSize<<1));
            break;
        case 1:
            InfoNES_Mirroring( 1 );
            SRAMBANK = ROMPAGE((Map51_Bank|0x20|3) % (NesHeader.byRomSize<<1));
            ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((Map51_Bank|0x00|2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((Map51_Bank|0x00|3) % (NesHeader.byRomSize<<1));
            break;
        case 2:
            InfoNES_Mirroring( 1 );
            SRAMBANK = ROMPAGE((Map51_Bank|0x2e|3) % (NesHeader.byRomSize<<1));
            ROMBANK0 = ROMPAGE((Map51_Bank|0x02|0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((Map51_Bank|0x02|1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((Map51_Bank|0x0e|2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((Map51_Bank|0x0e|3) % (NesHeader.byRomSize<<1));
            break;
        case 3:
            InfoNES_Mirroring( 0 );
            SRAMBANK = ROMPAGE((Map51_Bank|0x20|3) % (NesHeader.byRomSize<<1));
            ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((Map51_Bank|0x00|2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((Map51_Bank|0x00|3) % (NesHeader.byRomSize<<1));
            break;
    }
}

