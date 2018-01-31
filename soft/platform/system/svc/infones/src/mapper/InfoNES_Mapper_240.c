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










void Map240_Init()
{

    MapperInit = Map240_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map240_Apu;


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


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map240_Apu( WORD wAddr, BYTE byData )
{
    if( wAddr>=0x4020 && wAddr<0x6000 )
    {

        ROMBANK0 = ROMPAGE((((byData&0xF0)>>2)+0) % (NesHeader.byRomSize<<1));
        ROMBANK1 = ROMPAGE((((byData&0xF0)>>2)+1) % (NesHeader.byRomSize<<1));
        ROMBANK2 = ROMPAGE((((byData&0xF0)>>2)+2) % (NesHeader.byRomSize<<1));
        ROMBANK3 = ROMPAGE((((byData&0xF0)>>2)+3) % (NesHeader.byRomSize<<1));


        PPUBANK[ 0 ] = VROMPAGE((((byData&0x0F)<<3)+0) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 1 ] = VROMPAGE((((byData&0x0F)<<3)+1) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 2 ] = VROMPAGE((((byData&0x0F)<<3)+2) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 3 ] = VROMPAGE((((byData&0x0F)<<3)+3) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 4 ] = VROMPAGE((((byData&0x0F)<<3)+4) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 5 ] = VROMPAGE((((byData&0x0F)<<3)+5) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 6 ] = VROMPAGE((((byData&0x0F)<<3)+6) % (NesHeader.byVRomSize<<3));
        PPUBANK[ 7 ] = VROMPAGE((((byData&0x0F)<<3)+7) % (NesHeader.byVRomSize<<3));
        InfoNES_SetupChr();
    }
}
