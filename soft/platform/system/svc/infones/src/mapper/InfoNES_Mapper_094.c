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










void Map94_Init()
{

    MapperInit = Map94_Init;


    MapperWrite = Map94_Write;


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


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map94_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xfff0 )
    {

        case 0xff00:
            byData = ( byData & 0x1c ) >> 2;
            byData <<= 1;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;
    }
}
