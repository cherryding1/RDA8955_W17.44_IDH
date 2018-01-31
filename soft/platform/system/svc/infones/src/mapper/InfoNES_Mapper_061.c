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










void Map61_Init()
{

    MapperInit = Map61_Init;


    MapperWrite = Map61_Write;


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
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map61_Write( WORD wAddr, BYTE byData )
{
    BYTE byBank;

    switch( wAddr & 0x30 )
    {
        case    0x00:
        case    0x30:
            ROMBANK0 = ROMPAGE((((wAddr&0x0F)<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((((wAddr&0x0F)<<2)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((wAddr&0x0F)<<2)+2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((wAddr&0x0F)<<2)+3) % (NesHeader.byRomSize<<1));
            break;
        case    0x10:
        case    0x20:
            byBank = ((wAddr & 0x0F)<<1)|((wAddr&0x20)>>4);

            ROMBANK0 = ROMPAGE(((byBank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((byBank<<1)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((byBank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((byBank<<1)+1) % (NesHeader.byRomSize<<1));
            break;
    }

    if( wAddr & 0x80 ) InfoNES_Mirroring( 0 );
    else           InfoNES_Mirroring( 1 );
}
