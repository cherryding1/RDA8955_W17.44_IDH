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










void Map227_Init()
{

    MapperInit = Map227_Init;


    MapperWrite = Map227_Write;


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
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 1 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map227_Write( WORD wAddr, BYTE byData )
{
    BYTE  byBank = ((wAddr&0x0100)>>4)|((wAddr&0x0078)>>3);

    if( wAddr & 0x0001 )
    {
        ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (NesHeader.byRomSize<<1));
    }
    else
    {
        if( wAddr & 0x0004 )
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

    if( !(wAddr & 0x0080) )
    {
        if( wAddr & 0x0200 )
        {
            ROMBANK2 = ROMPAGE((((byBank&0x1C)<<2)+14) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((byBank&0x1C)<<2)+15) % (NesHeader.byRomSize<<1));
        }
        else
        {
            ROMBANK2 = ROMPAGE((((byBank&0x1C)<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((byBank&0x1C)<<2)+1) % (NesHeader.byRomSize<<1));
        }
    }
    if( wAddr & 0x0002 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }
}
