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










void Map71_Init()
{

    MapperInit = Map71_Init;


    MapperWrite = Map71_Write;


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




void Map71_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf000 )
    {
        case 0x9000:
            if ( byData & 0x10 )
            {
                InfoNES_Mirroring( 2 );
            }
            else
            {
                InfoNES_Mirroring( 3 );
            }
            break;


        case 0xc000:
        case 0xd000:
        case 0xe000:
        case 0xf000:
            ROMBANK0 = ROMPAGE( ( ( byData << 1 ) + 0 ) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE( ( ( byData << 1 ) + 1 ) % ( NesHeader.byRomSize << 1 ) );
            break;
    }
}
