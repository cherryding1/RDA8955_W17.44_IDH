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







BYTE    Map234_Reg[2];




void Map234_Init()
{

    MapperInit = Map234_Init;


    MapperWrite = Map234_Write;


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


    Map234_Reg[0] = 0;
    Map234_Reg[1] = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map234_Write( WORD wAddr, BYTE byData )
{
    if( wAddr >= 0xFF80 && wAddr <= 0xFF9F )
    {
        if( !Map234_Reg[0] )
        {
            Map234_Reg[0] = byData;
            Map234_Set_Banks();
        }
    }

    if( wAddr >= 0xFFE8 && wAddr <= 0xFFF7 )
    {
        Map234_Reg[1] = byData;
        Map234_Set_Banks();
    }
}




void Map234_Set_Banks()
{
    BYTE byPrg, byChr;

    if( Map234_Reg[0] & 0x80 )
    {
        InfoNES_Mirroring( 0 );
    }
    else
    {
        InfoNES_Mirroring( 1 );
    }
    if( Map234_Reg[0] & 0x40 )
    {
        byPrg = (Map234_Reg[0]&0x0E)|(Map234_Reg[1]&0x01);
        byChr = ((Map234_Reg[0]&0x0E)<<2)|((Map234_Reg[1]>>4)&0x07);
    }
    else
    {
        byPrg = Map234_Reg[0]&0x0F;
        byChr = ((Map234_Reg[0]&0x0F)<<2)|((Map234_Reg[1]>>4)&0x03);
    }


    ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (NesHeader.byRomSize<<1));
    ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (NesHeader.byRomSize<<1));


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


