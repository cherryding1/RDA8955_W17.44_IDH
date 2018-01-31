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







BYTE Map188_Dummy[ 0x2000 ];




void Map188_Init()
{

    MapperInit = Map188_Init;


    MapperWrite = Map188_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = Map188_Dummy;


    if ( ( NesHeader.byRomSize << 1 ) > 16 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 14 );
        ROMBANK3 = ROMPAGE( 15 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }


    Map188_Dummy[ 0 ] = 0x03;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map188_Write( WORD wAddr, BYTE byData )
{

    if ( byData )
    {
        if ( byData & 0x10 )
        {
            byData = ( byData & 0x07 ) << 1;
            ROMBANK0 = ROMPAGE( ( byData + 0 ) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE( ( byData + 1 ) % ( NesHeader.byRomSize << 1 ) );
        }
        else
        {
            byData <<= 1;
            ROMBANK0 = ROMPAGE( ( byData + 16 ) % ( NesHeader.byRomSize << 1 ) );
            ROMBANK1 = ROMPAGE( ( byData + 17 ) % ( NesHeader.byRomSize << 1 ) );
        }
    }
    else
    {
        if ( ( NesHeader.byRomSize << 1 ) == 0x10 )
        {
            ROMBANK0 = ROMPAGE( 14 );
            ROMBANK1 = ROMPAGE( 15 );
        }
        else
        {
            ROMBANK0 = ROMPAGE( 16 );
            ROMBANK1 = ROMPAGE( 17 );
        }
    }
}
