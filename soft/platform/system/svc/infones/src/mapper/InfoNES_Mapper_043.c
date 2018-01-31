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







DWORD Map43_IRQ_Cnt;
BYTE Map43_IRQ_Enable;




void Map43_Init()
{

    MapperInit = Map43_Init;


    MapperWrite = Map43_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map43_Apu;


    MapperReadApu = Map43_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map43_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = ROMPAGE( 2 );


    ROMBANK0 = ROMPAGE( 1 );
    ROMBANK1 = ROMPAGE( 0 );
    ROMBANK2 = ROMPAGE( 4 );
    ROMBANK3 = ROMPAGE( 9 );


    Map43_IRQ_Enable = 1;
    Map43_IRQ_Cnt = 0;


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




BYTE Map43_ReadApu( WORD wAddr )
{
    if ( 0x5000 <= wAddr && wAddr < 0x6000 )
    {
        return ROM[ 0x2000*8 + 0x1000 + (wAddr - 0x5000) ];
    }
    return (BYTE)(wAddr >> 8);
}




void Map43_Apu( WORD wAddr, BYTE byData )
{
    if( (wAddr&0xF0FF) == 0x4022 )
    {
        switch( byData&0x07 )
        {
            case    0x00:
            case    0x02:
            case    0x03:
            case    0x04:
                ROMBANK2 = ROMPAGE( 4 );
                break;
            case    0x01:
                ROMBANK2 = ROMPAGE( 3 );
                break;
            case    0x05:
                ROMBANK2 = ROMPAGE( 7 );
                break;
            case    0x06:
                ROMBANK2 = ROMPAGE( 5 );
                break;
            case    0x07:
                ROMBANK2 = ROMPAGE( 6 );
                break;
        }
    }
}




void Map43_Write( WORD wAddr, BYTE byData )
{
    if( wAddr == 0x8122 )
    {
        if( byData&0x03 )
        {
            Map43_IRQ_Enable = 1;
        }
        else
        {
            Map43_IRQ_Cnt = 0;
            Map43_IRQ_Enable = 0;
        }
    }
}




void Map43_HSync()
{
    if( Map43_IRQ_Enable )
    {
        Map43_IRQ_Cnt += 114;
        if( Map43_IRQ_Cnt >= 4096 )
        {
            Map43_IRQ_Cnt -= 4096;
            IRQ_REQ;
        }
    }
}
