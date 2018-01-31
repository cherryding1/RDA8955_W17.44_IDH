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







BYTE    Map236_Bank, Map236_Mode;




void Map236_Init()
{

    MapperInit = Map236_Init;


    MapperWrite = Map236_Write;


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


    Map236_Bank = Map236_Mode = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map236_Write( WORD wAddr, BYTE byData )
{
    if( wAddr >= 0x8000 && wAddr <= 0xBFFF )
    {
        Map236_Bank = ((wAddr&0x03)<<4)|(Map236_Bank&0x07);
    }
    else
    {
        Map236_Bank = (wAddr&0x07)|(Map236_Bank&0x30);
        Map236_Mode = wAddr&0x30;
    }

    if( wAddr & 0x20 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }

    switch( Map236_Mode )
    {
        case  0x00:
            Map236_Bank |= 0x08;
            ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (NesHeader.byRomSize<<1));
            break;
        case  0x10:
            Map236_Bank |= 0x37;
            ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (NesHeader.byRomSize<<1));
            break;
        case  0x20:
            Map236_Bank |= 0x08;
            ROMBANK0 = ROMPAGE((((Map236_Bank&0xFE)<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((((Map236_Bank&0xFE)<<1)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((Map236_Bank&0xFE)<<1)+2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((Map236_Bank&0xFE)<<1)+3) % (NesHeader.byRomSize<<1));
            break;
        case  0x30:
            Map236_Bank |= 0x08;
            ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.byRomSize<<1));
            break;
    }
}
