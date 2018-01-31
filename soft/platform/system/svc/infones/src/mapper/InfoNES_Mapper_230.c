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







BYTE Map230_RomSw;




void Map230_Init()
{

    MapperInit = Map230_Init;


    MapperWrite = Map230_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    if( Map230_RomSw )
    {
        Map230_RomSw = 0;
    }
    else
    {
        Map230_RomSw = 1;
    }


    if( Map230_RomSw )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 14 );
        ROMBANK3 = ROMPAGE( 15 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( 16 );
        ROMBANK1 = ROMPAGE( 17 );
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map230_Write( WORD wAddr, BYTE byData )
{
    if( Map230_RomSw )
    {
        ROMBANK0 = ROMPAGE((((byData&0x07)<<1)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE((((byData&0x07)<<1)+1) % (NesHeader.byRomSize<<1));
    }
    else
    {
        if( byData & 0x20 )
        {
            ROMBANK0 = ROMPAGE((((byData&0x1F)<<1)+16) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((((byData&0x1F)<<1)+17) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((byData&0x1F)<<1)+16) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((byData&0x1F)<<1)+17) % (NesHeader.byRomSize<<1));
        }
        else
        {
            ROMBANK0 = ROMPAGE((((byData&0x1E)<<1)+16) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((((byData&0x1E)<<1)+17) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((((byData&0x1E)<<1)+18) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((((byData&0x1E)<<1)+19) % (NesHeader.byRomSize<<1));
        }
        if( byData & 0x40 )
        {
            InfoNES_Mirroring( 1 );
        }
        else
        {
            InfoNES_Mirroring( 0 );
        }
    }
}
