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










void Map233_Init()
{

    MapperInit = Map233_Init;


    MapperWrite = Map233_Write;


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




void Map233_Write( WORD wAddr, BYTE byData )
{
    if( byData & 0x20 )
    {
        ROMBANK0 = ROMPAGE((((byData&0x1F)<<1)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE((((byData&0x1F)<<1)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE((((byData&0x1F)<<1)+0) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE((((byData&0x1F)<<1)+1) % (NesHeader.byRomSize<<1));
    }
    else
    {
        ROMBANK0 = ROMPAGE((((byData&0x1E)<<1)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE((((byData&0x1E)<<1)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE((((byData&0x1E)<<1)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE((((byData&0x1E)<<1)+3) % (NesHeader.byRomSize<<1));
    }

    if( (byData&0xC0) == 0x00 )
    {
        InfoNES_Mirroring( 5 );
    }
    else if( (byData&0xC0) == 0x40 )
    {
        InfoNES_Mirroring( 1 );
    }
    else if( (byData&0xC0) == 0x80 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 2 );
    }
}
