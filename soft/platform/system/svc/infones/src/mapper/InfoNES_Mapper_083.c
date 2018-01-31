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







BYTE Map83_Regs[3];
DWORD Map83_Chr_Bank;
DWORD Map83_IRQ_Cnt;
BYTE Map83_IRQ_Enabled;




void Map83_Init()
{

    MapperInit = Map83_Init;


    MapperWrite = Map83_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map83_Apu;


    MapperReadApu = Map83_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map83_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    Map83_Regs[0] = Map83_Regs[1] = Map83_Regs[2] = 0;


    if ( ( NesHeader.byRomSize << 1 ) >= 32 )
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMPAGE( 30 );
        ROMBANK3 = ROMPAGE( 31 );
        Map83_Regs[ 1 ] = 0x30;
    }
    else
    {
        ROMBANK0 = ROMPAGE( 0 );
        ROMBANK1 = ROMPAGE( 1 );
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
    }


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    Map83_IRQ_Enabled = 0;
    Map83_IRQ_Cnt = 0;
    Map83_Chr_Bank = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




BYTE Map83_ReadApu( WORD wAddr )
{
    if ( ( wAddr & 0x5100 ) == 0x5100 )
    {
        return Map83_Regs[2];
    }
    else
    {
        return wAddr >> 8;
    }
}




void Map83_Apu( WORD wAddr, BYTE byData )
{
    switch(wAddr)
    {
        case 0x5101:
        case 0x5102:
        case 0x5103:
            Map83_Regs[2] = byData;
            break;
    }
}




void Map83_Write( WORD wAddr, BYTE byData )
{

    switch( wAddr )
    {
        case 0x8000:
        case 0xB000:
        case 0xB0FF:
        case 0xB1FF:
            Map83_Regs[0] = byData;
            Map83_Chr_Bank = (byData & 0x30) << 4;

            ROMBANK0 = ROMPAGE( (byData*2+0) % (NesHeader.byRomSize << 1) );
            ROMBANK1 = ROMPAGE( (byData*2+1) % (NesHeader.byRomSize << 1) );
            ROMBANK2 = ROMPAGE( (((byData&0x30)|0x0F)*2+0) % (NesHeader.byRomSize << 1) );
            ROMBANK3 = ROMPAGE( (((byData&0x30)|0x0F)*2+1) % (NesHeader.byRomSize << 1) );
            break;

        case 0x8100:
            if ( NesHeader.byVRomSize <= 32 )
            {
                Map83_Regs[1] = byData;
            }
            if ((byData & 0x03) == 0x00)
            {
                InfoNES_Mirroring( 1 );
            }
            else if((byData & 0x03) == 0x01)
            {
                InfoNES_Mirroring( 0 );
            }
            else if((byData & 0x03) == 0x02)
            {
                InfoNES_Mirroring( 3 );
            }
            else
            {
                InfoNES_Mirroring( 2 );
            }
            break;

        case 0x8200:
            Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0xFF00 ) | (DWORD)byData;
            break;

        case 0x8201:
            Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0x00FF ) | ((DWORD)byData << 8);
            Map83_IRQ_Enabled = byData;
            break;

        case 0x8300:
            ROMBANK0 = ROMPAGE( byData % (NesHeader.byRomSize << 1) );
            break;

        case 0x8301:
            ROMBANK1 = ROMPAGE( byData % (NesHeader.byRomSize << 1) );
            break;

        case 0x8302:
            ROMBANK2 = ROMPAGE( byData % (NesHeader.byRomSize << 1) );
            break;

        case 0x8310:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 0 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 0 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 1 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8311:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 1 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 2 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 3 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8312:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 2 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8313:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 3 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8314:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 4 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8315:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 5 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8316:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 6 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8317:
            if ((Map83_Regs[1] & 0x30) == 0x30)
            {
                PPUBANK[ 7 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            else if ( (Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
            {
                PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (NesHeader.byVRomSize << 3) );
                PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (NesHeader.byVRomSize << 3) );
                InfoNES_SetupChr();
            }
            break;

        case 0x8318:
            ROMBANK0 = ROMPAGE( (((Map83_Regs[0]&0x30)|byData)*2+0) % (NesHeader.byRomSize << 1) );
            ROMBANK1 = ROMPAGE( (((Map83_Regs[0]&0x30)|byData)*2+1) % (NesHeader.byRomSize << 1) );
            break;
    }
}




void Map83_HSync()
{
    if (Map83_IRQ_Enabled)
    {
        if (Map83_IRQ_Cnt <= 114)
        {
            IRQ_REQ;
            Map83_IRQ_Enabled = 0;
        }
        else
        {
            Map83_IRQ_Cnt -= 114;
        }
    }
}
