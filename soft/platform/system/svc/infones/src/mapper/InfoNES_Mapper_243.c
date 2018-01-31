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







BYTE Map243_Regs[4];




void Map243_Init()
{

    MapperInit = Map243_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map243_Apu;


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


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
        {
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        }
        InfoNES_SetupChr();
    }


    Map243_Regs[0] = Map243_Regs[1] = Map243_Regs[2] = Map243_Regs[3] = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map243_Apu( WORD wAddr, BYTE byData )
{
    if ( wAddr == 0x4100 )
    {
        Map243_Regs[0] = byData;
    }
    else if ( wAddr == 0x4101 )
    {
        switch ( Map243_Regs[0] & 0x07 )
        {
            case 0x02:
                Map243_Regs[1] = byData & 0x01;
                break;

            case 0x00:
            case 0x04:
            case 0x07:
                Map243_Regs[2] = ( byData & 0x01 ) << 1;
                break;


            case 0x05:
                ROMBANK0 = ROMPAGE( ( byData * 4 + 0 ) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK1 = ROMPAGE( ( byData * 4 + 1 ) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK2 = ROMPAGE( ( byData * 4 + 2 ) % ( NesHeader.byRomSize << 1 ) );
                ROMBANK3 = ROMPAGE( ( byData * 4 + 3 ) % ( NesHeader.byRomSize << 1 ) );
                break;

            case 0x06:
                Map243_Regs[3] = ( byData & 0x03 ) << 2;
                break;
        }


        if ( ( NesHeader.byVRomSize << 3 ) <= 64 )
        {
            BYTE chr_bank = ( Map243_Regs[2] + Map243_Regs[3] ) >> 1;

            PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
        else
        {
            BYTE chr_bank = Map243_Regs[1] + Map243_Regs[2] + Map243_Regs[3];

            PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( NesHeader.byVRomSize << 3 ) );
            PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
        }
    }
}
