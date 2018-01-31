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










void Map0_Init()
{

    MapperInit = Map0_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    if ( NesHeader.byRomSize > 1 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 2 );
        ROMBANK3 = ROMPAGE( 3 );
    }
    else if ( NesHeader.byRomSize > 0 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 0 );
        ROMBANK3 = ROMPAGE( 1 );
    }
    else
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 0 );
        ROMBANK2 = ROMPAGE( 0 );
        ROMBANK3 = ROMPAGE( 0 );
    }


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map0_Write( WORD wAddr, BYTE byData )
{

}




void Map0_Sram( WORD wAddr, BYTE byData )
{

}




void Map0_Apu( WORD wAddr, BYTE byData )
{

}




BYTE Map0_ReadApu( WORD wAddr )
{

    return ( wAddr >> 8 );
}




void Map0_VSync()
{

}




void Map0_HSync()
{

#if 0
    // Frame IRQ
    FrameStep += STEP_PER_SCANLINE;
    if ( FrameStep > STEP_PER_FRAME && FrameIRQ_Enable )
    {
        FrameStep %= STEP_PER_FRAME;
        IRQ_REQ;
        APU_Reg[ 0x4015 ] |= 0x40;
    }
#endif
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 PPU Function                                            */
/*-------------------------------------------------------------------*/
void Map0_PPU( WORD wAddr )
{
    /*
     *  Dummy Callback at PPU
     *
     */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Rendering Screen Function                               */
/*-------------------------------------------------------------------*/
void Map0_RenderScreen( BYTE byMode )
{
    /*
     *  Dummy Callback at Rendering Screen
     *
     */
}
