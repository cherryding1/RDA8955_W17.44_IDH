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







BYTE  Map73_IRQ_Enable;
DWORD Map73_IRQ_Cnt;




void Map73_Init()
{

    MapperInit = Map73_Init;


    MapperWrite = Map73_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map73_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    Map73_IRQ_Enable = 0;
    Map73_IRQ_Cnt = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map73_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:
            Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xfff0 ) | ( byData & 0x0f );
            break;

        case 0x9000:
            Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xff0f ) | ( ( byData & 0x0f ) << 4 );
            break;

        case 0xa000:
            Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xf0ff ) | ( ( byData & 0x0f ) << 8 );
            break;

        case 0xb000:
            Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0x0fff ) | ( ( byData & 0x0f ) << 12 );
            break;

        case 0xc000:
            Map73_IRQ_Enable = byData;
            break;


        case 0xf000:
            byData <<= 1;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;
    }
}




void Map73_HSync()
{

#if 1
    if ( Map73_IRQ_Enable & 0x02 )
    {
        if ( ( Map73_IRQ_Cnt += STEP_PER_SCANLINE ) > 0xffff )
        {
            Map73_IRQ_Cnt &= 0xffff;
            IRQ_REQ;
            Map73_IRQ_Enable = 0;
        }
    }
#else
    if ( Map73_IRQ_Enable )
    {
        if ( Map73_IRQ_Cnt > 0xffff - 114 )
        {
            IRQ_REQ;
            Map73_IRQ_Enable = 0;
        }
        else
        {
            Map73_IRQ_Cnt += 114;
        }
    }
#endif
}
