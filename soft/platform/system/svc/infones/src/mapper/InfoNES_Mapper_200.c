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










void Map200_Init()
{

    MapperInit = Map200_Init;


    MapperWrite = Map200_Write;


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


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map200_Write( WORD wAddr, BYTE byData )
{

    ROMBANK0 = ROMPAGE((((wAddr&0x07)<<1)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1 = ROMPAGE((((wAddr&0x07)<<1)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2 = ROMPAGE((((wAddr&0x07)<<1)+0) % (NesHeader.byRomSize<<1));
    ROMBANK3 = ROMPAGE((((wAddr&0x07)<<1)+1) % (NesHeader.byRomSize<<1));


    PPUBANK[0] = VROMPAGE((((wAddr&0x07)<<3)+0) % (NesHeader.byVRomSize<<3));
    PPUBANK[1] = VROMPAGE((((wAddr&0x07)<<3)+1) % (NesHeader.byVRomSize<<3));
    PPUBANK[2] = VROMPAGE((((wAddr&0x07)<<3)+2) % (NesHeader.byVRomSize<<3));
    PPUBANK[3] = VROMPAGE((((wAddr&0x07)<<3)+3) % (NesHeader.byVRomSize<<3));
    PPUBANK[4] = VROMPAGE((((wAddr&0x07)<<3)+4) % (NesHeader.byVRomSize<<3));
    PPUBANK[5] = VROMPAGE((((wAddr&0x07)<<3)+5) % (NesHeader.byVRomSize<<3));
    PPUBANK[6] = VROMPAGE((((wAddr&0x07)<<3)+6) % (NesHeader.byVRomSize<<3));
    PPUBANK[7] = VROMPAGE((((wAddr&0x07)<<3)+7) % (NesHeader.byVRomSize<<3));
    InfoNES_SetupChr();

    if ( wAddr & 0x01 )
    {
        InfoNES_Mirroring( 1 );
    }
    else
    {
        InfoNES_Mirroring( 0 );
    }
}
