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







BYTE    Map226_Reg[2];




void Map226_Init()
{

    MapperInit = Map226_Init;


    MapperWrite = Map226_Write;


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


    Map226_Reg[0] = 0;
    Map226_Reg[1] = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map226_Write( WORD wAddr, BYTE byData )
{
    if( wAddr & 0x0001 )
    {
        Map226_Reg[1] = byData;
    }
    else
    {
        Map226_Reg[0] = byData;
    }

    if( Map226_Reg[0] & 0x40 )
    {
        InfoNES_Mirroring( 1 );
    }
    else
    {
        InfoNES_Mirroring( 0 );
    }

    BYTE  byBank = ((Map226_Reg[0]&0x1E)>>1)|((Map226_Reg[0]&0x80)>>3)|((Map226_Reg[1]&0x01)<<5);

    if( Map226_Reg[0] & 0x20 )
    {
        if( Map226_Reg[0] & 0x01 )
        {
            ROMBANK0 = ROMPAGE(((byBank<<2)+2) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((byBank<<2)+3) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (NesHeader.byRomSize<<1));
        }
        else
        {
            ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((byBank<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((byBank<<2)+1) % (NesHeader.byRomSize<<1));
        }
    }
    else
    {
        ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (NesHeader.byRomSize<<1));
    }
}
