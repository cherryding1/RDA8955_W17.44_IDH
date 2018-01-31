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







BYTE Map25_Bank_Selector;
BYTE Map25_VBank[16];

BYTE Map25_IRQ_Count;
BYTE Map25_IRQ_State;
BYTE Map25_IRQ_Latch;




void Map25_Init()
{
    int nPage;


    MapperInit = Map25_Init;


    MapperWrite = Map25_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map25_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    for (nPage = 0; nPage < 16; nPage++)
        Map25_VBank[ nPage ] = 0x00;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map25_Write( WORD wAddr, BYTE byData )
{
    int nBank;

    switch ( wAddr )
    {
        case 0x8000:

            byData %= ( NesHeader.byRomSize << 1 );
            if ( Map25_Bank_Selector )
            {
                ROMBANK2 = ROMPAGE( byData );
            }
            else
            {
                ROMBANK0 = ROMPAGE( byData );
            }
            break;

        case 0x9000:

            switch (byData & 0x03)
            {
                case 0:
                    InfoNES_Mirroring( 1 );
                    break;
                case 1:
                    InfoNES_Mirroring( 0 );
                    break;
                case 2:
                    InfoNES_Mirroring( 2 );
                    break;
                case 3:
                    InfoNES_Mirroring( 3 );
                    break;
            }
            break;

        case 0x9002:


            Map25_Bank_Selector = byData & 0x02;
            break;

        case 0xa000:

            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        default:

            switch ( wAddr & 0xfffc)
            {
                case 0xb000:
                    Map25_VBank[ 0 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
                    nBank = 0 + ( wAddr & 0x0001 );
                    Map25_Sync_Vrom( nBank );
                    break;

                case 0xc000:
                    Map25_VBank[ 4 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
                    nBank = 2 + ( wAddr & 0x0001 );
                    Map25_Sync_Vrom( nBank );
                    break;

                case 0xd000:
                    Map25_VBank[ 8 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
                    nBank = 4 + ( wAddr & 0x0001 );
                    Map25_Sync_Vrom( nBank );
                    break;

                case 0xe000:
                    Map25_VBank[ 12 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
                    nBank = 6 + ( wAddr & 0x0001 );
                    Map25_Sync_Vrom( nBank );
                    break;

                case 0xf000:
                    switch ( wAddr & 0x0003 )
                    {
                        case 0:
                        case 1:
                            Map25_IRQ_Latch = byData;
                            break;
                        case 2:
                            Map25_IRQ_State = ( byData & 0x01 ) ? Map25_IRQ_State : 0x00;
                            Map25_IRQ_State = ( byData & 0x02 ) ? 0x01 : Map25_IRQ_State;
                            Map25_IRQ_Count = Map25_IRQ_Latch;
                            break;
                        case 3:
                            Map25_IRQ_State = ( Map25_IRQ_State << 1 ) | ( Map25_IRQ_State & 1 );
                            break;
                    }
                    break;
            }
            break;
    }
}




void Map25_Sync_Vrom( int nBank )
{
    BYTE byValue;

    byValue  = Map25_VBank[ ( nBank << 1 ) - ( nBank & 0x01 ) ];
    byValue |= Map25_VBank[ ( nBank << 1 ) - ( nBank & 0x01 ) + 2] << 4;
    byValue %= ( NesHeader.byVRomSize << 3 );
    PPUBANK[ nBank ] = VROMPAGE( byValue );
    InfoNES_SetupChr();
}




void Map25_HSync()
{

    if ( ( Map25_IRQ_State & 0x02 ) && ( ++Map25_IRQ_Count == 0 ) )
    {
        IRQ_REQ;
        Map25_IRQ_Count = Map25_IRQ_Latch;
    }
}
