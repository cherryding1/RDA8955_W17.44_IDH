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







BYTE    Map57_Reg;




void Map57_Init()
{

    MapperInit = Map57_Init;


    MapperWrite = Map57_Write;


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


    Map57_Reg = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map57_Write( WORD wAddr, BYTE byData )
{
    BYTE byChr;

    switch( wAddr )
    {
        case  0x8000:
        case  0x8001:
        case  0x8002:
        case  0x8003:
            if( byData & 0x40 )
            {
                byChr = (byData&0x03)+((Map57_Reg&0x10)>>1)+(Map57_Reg&0x07);

                PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (NesHeader.byVRomSize<<3));
                PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (NesHeader.byVRomSize<<3));
                InfoNES_SetupChr();
            }
            break;
        case  0x8800:
            Map57_Reg = byData;

            if( byData & 0x80 )
            {
                ROMBANK0 = ROMPAGE((((byData & 0x40)>>6)*4+8+0) % (NesHeader.byRomSize<<1));
                ROMBANK1 = ROMPAGE((((byData & 0x40)>>6)*4+8+1) % (NesHeader.byRomSize<<1));
                ROMBANK2 = ROMPAGE((((byData & 0x40)>>6)*4+8+2) % (NesHeader.byRomSize<<1));
                ROMBANK3 = ROMPAGE((((byData & 0x40)>>6)*4+8+3) % (NesHeader.byRomSize<<1));
            }
            else
            {
                ROMBANK0 = ROMPAGE((((byData & 0x60)>>5)*2+0) % (NesHeader.byRomSize<<1));
                ROMBANK1 = ROMPAGE((((byData & 0x60)>>5)*2+1) % (NesHeader.byRomSize<<1));
                ROMBANK2 = ROMPAGE((((byData & 0x60)>>5)*2+0) % (NesHeader.byRomSize<<1));
                ROMBANK3 = ROMPAGE((((byData & 0x60)>>5)*2+1) % (NesHeader.byRomSize<<1));
            }

            byChr = (byData&0x07)+((byData&0x10)>>1);

            PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (NesHeader.byVRomSize<<3));
            PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (NesHeader.byVRomSize<<3));
            InfoNES_SetupChr();

            if( byData & 0x08 ) InfoNES_Mirroring( 0 );
            else        InfoNES_Mirroring( 1 );

            break;
    }
}

