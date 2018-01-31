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







BYTE    Map255_Reg[4];




void Map255_Init()
{

    MapperInit = Map255_Init;


    MapperWrite = Map255_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map255_Apu;


    MapperReadApu = Map255_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    InfoNES_Mirroring( 1 );

    Map255_Reg[0] = 0;
    Map255_Reg[1] = 0;
    Map255_Reg[2] = 0;
    Map255_Reg[3] = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map255_Write( WORD wAddr, BYTE byData )
{
    BYTE  byPrg = (wAddr & 0x0F80)>>7;
    int   nChr = (wAddr & 0x003F);
    int   nBank = (wAddr & 0x4000)>>14;

    if( wAddr & 0x2000 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }

    if( wAddr & 0x1000 )
    {
        if( wAddr & 0x0040 )
        {
            ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+2) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+3) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+2) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+3) % (NesHeader.byRomSize<<1));
        }
        else
        {
            ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+0) % (NesHeader.byRomSize<<1));
            ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+1) % (NesHeader.byRomSize<<1));
            ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+0) % (NesHeader.byRomSize<<1));
            ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+1) % (NesHeader.byRomSize<<1));
        }
    }
    else
    {
        ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+3) % (NesHeader.byRomSize<<1));
    }

    PPUBANK[ 0 ] = VROMPAGE((0x200*nBank+nChr*8+0) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 1 ] = VROMPAGE((0x200*nBank+nChr*8+1) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 2 ] = VROMPAGE((0x200*nBank+nChr*8+2) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 3 ] = VROMPAGE((0x200*nBank+nChr*8+3) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 4 ] = VROMPAGE((0x200*nBank+nChr*8+4) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 5 ] = VROMPAGE((0x200*nBank+nChr*8+5) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 6 ] = VROMPAGE((0x200*nBank+nChr*8+6) % (NesHeader.byVRomSize<<3));
    PPUBANK[ 7 ] = VROMPAGE((0x200*nBank+nChr*8+7) % (NesHeader.byVRomSize<<3));
}




void Map255_Apu( WORD wAddr, BYTE byData )
{
    if( wAddr >= 0x5800 )
    {
        Map255_Reg[wAddr&0x0003] = byData & 0x0F;
    }
}




BYTE Map255_ReadApu( WORD wAddr )
{
    if( wAddr >= 0x5800 )
    {
        return  Map255_Reg[wAddr&0x0003] & 0x0F;
    }
    else
    {
        return  wAddr>>8;
    }
}
