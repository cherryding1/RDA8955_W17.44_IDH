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







BYTE    Map96_Reg[2];




void Map96_Init()
{

    MapperInit = Map96_Init;


    MapperWrite = Map96_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map96_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map96_Reg[0] = Map96_Reg[1] = 0;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    Map96_Set_Banks();
    InfoNES_Mirroring( 3 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map96_Write( WORD wAddr, BYTE byData )
{
    ROMBANK0 = ROMPAGE((((byData & 0x03)<<2)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1 = ROMPAGE((((byData & 0x03)<<2)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2 = ROMPAGE((((byData & 0x03)<<2)+2) % (NesHeader.byRomSize<<1));
    ROMBANK3 = ROMPAGE((((byData & 0x03)<<2)+3) % (NesHeader.byRomSize<<1));

    Map96_Reg[0] = (byData & 0x04) >> 2;
    Map96_Set_Banks();
}




void Map96_PPU( WORD wAddr )
{
    if( (wAddr & 0xF000) == 0x2000 )
    {
        Map96_Reg[1] = (wAddr>>8)&0x03;
        Map96_Set_Banks();
    }
}




void Map96_Set_Banks()
{
    PPUBANK[ 0 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+0);
    PPUBANK[ 1 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+1);
    PPUBANK[ 2 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+2);
    PPUBANK[ 3 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+3);
    PPUBANK[ 4 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+0);
    PPUBANK[ 5 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+1);
    PPUBANK[ 6 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+2);
    PPUBANK[ 7 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+3);
    InfoNES_SetupChr();
}

