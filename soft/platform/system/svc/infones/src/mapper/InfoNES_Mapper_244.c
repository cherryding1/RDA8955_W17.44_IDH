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










void Map244_Init()
{

    MapperInit = Map244_Init;


    MapperWrite = Map244_Write;


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


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map244_Write( WORD wAddr, BYTE byData )
{
    if( wAddr>=0x8065 && wAddr<=0x80A4 )
    {

        ROMBANK0 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+3) % (NesHeader.byRomSize<<1));
    }

    if( wAddr>=0x80A5 && wAddr<=0x80E4 )
    {

        ROMBANK0 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+3) % (NesHeader.byRomSize<<1));
    }
}
