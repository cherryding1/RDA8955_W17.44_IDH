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







BYTE  Map40_IRQ_Enable;
DWORD Map40_Line_To_IRQ;




void Map40_Init()
{

    MapperInit = Map40_Init;


    MapperWrite = Map40_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map40_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = ROMPAGE( 6 );


    Map40_IRQ_Enable = 0;
    Map40_Line_To_IRQ = 0;


    ROMBANK0 = ROMPAGE( 4 );
    ROMBANK1 = ROMPAGE( 5 );
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 7 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map40_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xe000 )
    {
        case 0x8000:
            Map40_IRQ_Enable = 0;
            break;

        case 0xa000:
            Map40_IRQ_Enable = 1;
            Map40_Line_To_IRQ = 37;
            break;

        case 0xc000:
            break;

        case 0xe000:

            ROMBANK2 = ROMPAGE ( ( byData & 0x07 ) % ( NesHeader.byRomSize << 1 ) );
            break;
    }
}




void Map40_HSync()
{

    if ( Map40_IRQ_Enable )
    {
        if ( ( --Map40_Line_To_IRQ ) <= 0 )
        {
            IRQ_REQ;
        }
    }
}


