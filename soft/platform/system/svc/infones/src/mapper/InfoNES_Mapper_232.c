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







BYTE Map232_Regs[2];




void Map232_Init()
{

    MapperInit = Map232_Init;


    MapperWrite = Map232_Write;


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


    Map232_Regs[0] = 0x0C;
    Map232_Regs[1] = 0x00;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map232_Write( WORD wAddr, BYTE byData )
{
    if ( wAddr == 0x9000 )
    {
        Map232_Regs[0] = (byData & 0x18)>>1;
    }
    else if ( 0xA000 <= wAddr  )
    {
        Map232_Regs[1] = byData & 0x03;
    }

    ROMBANK0= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+0) % (NesHeader.byRomSize<<1));
    ROMBANK1= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+1) % (NesHeader.byRomSize<<1));
    ROMBANK2= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+0) % (NesHeader.byRomSize<<1));
    ROMBANK3= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+1) % (NesHeader.byRomSize<<1));
}
