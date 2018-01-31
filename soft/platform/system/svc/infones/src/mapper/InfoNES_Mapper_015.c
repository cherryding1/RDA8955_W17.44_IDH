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










void Map15_Init()
{

    MapperInit = Map15_Init;


    MapperWrite = Map15_Write;


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




void Map15_Write( WORD wAddr, BYTE byData )
{
    BYTE byBank;

    switch ( wAddr )
    {
        case 0x8000:

            InfoNES_Mirroring( byData & 0x20 ? 0 : 1);


            byBank = byData & 0x1f;
            byBank %= ( NesHeader.byRomSize << 1 );
            byBank <<= 1;

            ROMBANK0 = ROMPAGE( byBank );
            ROMBANK1 = ROMPAGE( byBank + 1 );
            ROMBANK2 = ROMPAGE( byBank + 2 );
            ROMBANK3 = ROMPAGE( byBank + 3 );
            break;

        case 0x8001:

            byData &= 0x3f;
            byData %= ( NesHeader.byRomSize << 1 );
            byData <<= 1;

            ROMBANK2 = ROMPAGE( byData );
            ROMBANK3 = ROMPAGE( byData + 1 );
            break;

        case 0x8002:

            byBank = byData & 0x3f;
            byBank %= ( NesHeader.byRomSize << 1 );
            byBank <<= 1;
            byBank += ( byData & 0x80 ? 1 : 0 );

            ROMBANK0 = ROMPAGE( byBank );
            ROMBANK1 = ROMPAGE( byBank );
            ROMBANK2 = ROMPAGE( byBank );
            ROMBANK3 = ROMPAGE( byBank );
            break;

        case 0x8003:

            InfoNES_Mirroring( byData & 0x20 ? 0 : 1);


            byData &= 0x1f;
            byData %= ( NesHeader.byRomSize << 1 );
            byData <<= 1;

            ROMBANK2 = ROMPAGE( byData );
            ROMBANK3 = ROMPAGE( byData + 1 );
            break;
    }
}
