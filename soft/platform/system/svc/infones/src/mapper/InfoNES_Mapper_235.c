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










void Map235_Init()
{

    MapperInit = Map235_Init;


    MapperWrite = Map235_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    for( int i = 0; i < 0x2000; i++ )
    {
        DRAM[i] = 0xFF;
    }


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map235_Write( WORD wAddr, BYTE byData )
{
    BYTE  byPrg = ((wAddr&0x0300)>>3)|(wAddr&0x001F);
    BYTE  byBus = 0;

    if( (NesHeader.byRomSize<<1) == 64*2 )
    {
        switch( wAddr & 0x0300 )
        {
            case    0x0000: break;
            case    0x0100: byBus = 1; break;
            case    0x0200: byBus = 1; break;
            case    0x0300: byBus = 1; break;
        }
    }
    else if( (NesHeader.byRomSize<<1) == 128*2 )
    {
        switch( wAddr & 0x0300 )
        {
            case    0x0000: break;
            case    0x0100: byBus = 1; break;
            case    0x0200: byPrg = (byPrg&0x1F)|0x20; break;
            case    0x0300: byBus = 1; break;
        }
    }
    else if( (NesHeader.byRomSize<<1) == 192*2 )
    {
        switch( wAddr & 0x0300 )
        {
            case    0x0000: break;
            case    0x0100: byBus = 1; break;
            case    0x0200: byPrg = (byPrg&0x1F)|0x20; break;
            case    0x0300: byPrg = (byPrg&0x1F)|0x40; break;
        }
    }
    else if( (NesHeader.byRomSize<<1) == 256*2 )
    {
    }

    if( wAddr & 0x0800 )
    {
        if( wAddr & 0x1000 )
        {
            ROMBANK0 = ROMPAGE(((byPrg<<2)+2) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((byPrg<<2)+3) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (NesHeader.byRomSize<<1));
        }
        else
        {
            ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
        }
    }
    else
    {
        ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (NesHeader.byRomSize<<1));
    }

    if( byBus )
    {
        ROMBANK0 = DRAM;
        ROMBANK1 = DRAM;
        ROMBANK2 = DRAM;
        ROMBANK3 = DRAM;
    }

    if( wAddr & 0x0400 )
    {
        InfoNES_Mirroring( 3 );
    }
    else if( wAddr & 0x2000 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }
}

