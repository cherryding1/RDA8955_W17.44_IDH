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







BYTE Map42_IRQ_Cnt;
BYTE Map42_IRQ_Enable;




void Map42_Init()
{

    MapperInit = Map42_Init;


    MapperWrite = Map42_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map42_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = ROMPAGE( 0 );


    ROMBANK0 = ROMLASTPAGE( 3 );
    ROMBANK1 = ROMLASTPAGE( 2 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map42_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xe003 )
    {

        case 0xe000:
            SRAMBANK = ROMPAGE( ( byData & 0x0f ) % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0xe001:
            if ( byData & 0x08 )
            {
                InfoNES_Mirroring( 0 );
            }
            else
            {
                InfoNES_Mirroring( 1 );
            }
            break;

        case 0xe002:
            if ( byData & 0x02 )
            {
                Map42_IRQ_Enable = 1;
            }
            else
            {
                Map42_IRQ_Enable = 0;
                Map42_IRQ_Cnt = 0;
            }
            break;
    }
}




void Map42_HSync()
{

    if ( Map42_IRQ_Enable )
    {
        if ( Map42_IRQ_Cnt < 215 )
        {
            Map42_IRQ_Cnt++;
        }
        if ( Map42_IRQ_Cnt == 215 )
        {
            IRQ_REQ;
            Map42_IRQ_Enable = 0;
        }
    }
}
