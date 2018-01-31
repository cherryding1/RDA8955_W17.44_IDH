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










void Map13_Init()
{

    MapperInit = Map13_Init;


    MapperWrite = Map13_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


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


    PPUBANK[ 0 ] = CRAMPAGE( 0 );
    PPUBANK[ 1 ] = CRAMPAGE( 1 );
    PPUBANK[ 2 ] = CRAMPAGE( 2 );
    PPUBANK[ 3 ] = CRAMPAGE( 3 );
    PPUBANK[ 4 ] = CRAMPAGE( 0 );
    PPUBANK[ 5 ] = CRAMPAGE( 1 );
    PPUBANK[ 6 ] = CRAMPAGE( 2 );
    PPUBANK[ 7 ] = CRAMPAGE( 3 );
    InfoNES_SetupChr();


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map13_Write( WORD wAddr, BYTE byData )
{

    ROMBANK0 = ROMPAGE((((byData&0x30)>>2)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1 = ROMPAGE((((byData&0x30)>>2)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2 = ROMPAGE((((byData&0x30)>>2)+2) % (NesHeader.byRomSize<<1));
    ROMBANK3 = ROMPAGE((((byData&0x30)>>2)+3) % (NesHeader.byRomSize<<1));


    PPUBANK[ 4 ] = CRAMPAGE(((byData&0x03)<<2)+0);
    PPUBANK[ 5 ] = CRAMPAGE(((byData&0x03)<<2)+1);
    PPUBANK[ 6 ] = CRAMPAGE(((byData&0x03)<<2)+2);
    PPUBANK[ 7 ] = CRAMPAGE(((byData&0x03)<<2)+3);
    InfoNES_SetupChr();
}
