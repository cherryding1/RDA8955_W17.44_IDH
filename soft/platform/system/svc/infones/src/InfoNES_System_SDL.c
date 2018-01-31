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



#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

#include "fs.h"
#include "cos.h"
#include "sxr_sbx.h"
#include "ts.h"
#include "sxr_mem.h"
#include "hal_overlay.h"

#ifdef NES_MEM_ALLOC
extern BYTE *DRAM; //0XA0000
extern struct ApuEvent_t *ApuEventQueue;
extern BYTE *Map85_Chr_Ram;
extern BYTE *Map5_Wram;
BYTE *Nes_Buff_Pool = NULL;
#define     NES_HEAP_INDEX           5
#endif

typedef enum
{
    INFONES_MSG_START,
    INFONES_MSG_QUIT,
    INFONES_MSG_MAX
} INFONES_BODY_MSG_T ;

typedef INFONES_BODY_MSG_T MsgBody_t;
#include "itf_msg.h"

static TASK_HANDLE *InfoNES_handle;
COS_EVENT InfoNES_ev;


#define LCD_WIDTH_LIB     240
#define LCD_HEIGHT_lLIB   320                                       /* Detail Info: [Main LCD is 220x176]*/

char *InfoNES_UCS2Strcpy(char *strDestination, const char *strSource );

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg )
{
// sxs_fprintf(1<<21,"InfoNES_DebugPrint \n");
}


/*-------------------------------------------------------------------*/
/*  ROM image file information                                       */
/*-------------------------------------------------------------------*/

char szRomName[ 1024 ];
char szSaveName[ 1024 ];
unsigned char* g_load_from_sram = NULL;
unsigned int g_load_from_sram_size = NULL;
int nSRAM_SaveFlag;

/*-------------------------------------------------------------------*/
/*  Constants ( Linux specific )                                     */
/*-------------------------------------------------------------------*/
static const char * VERSION = "InfoNES v0.97J RC1";

/* Pad state */
DWORD dwPad1;
DWORD dwPad2;
DWORD dwSystem;

/*-------------------------------------------------------------------*/
/*  Function prototypes ( SDL specific )                             */
/*-------------------------------------------------------------------*/

int LoadSRAM();
int SaveSRAM();
void InfoNes_task_msg_handle(void) ;
/*
31 63 31
5 6 5
5a 94 ff
    */
/* Palette data */
#if 1
WORD NesPalette[ 64 ] =
{
    0x738e,
    0x20f1,
    0x15,
    0x4013,
    0x880e,
    0xa802,
    0xa000,
    0x7860,
    0x4160,
    0x200,
    0x280,
    0x1e2,
    0x19eb,
    0x0,
    0x0,
    0x0,
    0xbdf7,
    0x39d,
    0x21fd,
    0x801e,
    0xb817,
    0xe00b,
    0xd960,
    0xca61,
    0x8b80,
    0x480,
    0x560,
    0x487,
    0x411,
    0x0,
    0x0,
    0x0,
    0xffff,
    0x3dff,
    0x5c9f,
    0x447f,
    0xf3ff,
    0xfb96,
    0xfb8c,
    0xfce7,
    0xf5e7,
    0x8682,
    0x4ee9,
    0x5ff3,
    0x77b,
    0x0,
    0x0,
    0x0,
    0xffff,
    0xaf1f,
    0xc69f,
    0xd67f,
    0xfe1f,
    0xfe1b,
    0xfdf6,
    0xfef5,
    0xff14,
    0xe7f4,
    0xaf97,
    0xb7f9,
    0x9ffe,
    0x0,
    0x0,
    0x0,

};
#else
WORD NesPalette[ 64 ] =
{
    0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
    0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
    0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
    0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
    0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
    0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};
WORD NesPalette_RGB[ 64 ] ;

#endif

/*===================================================================*/
/*                                                                   */
/*           LoadSRAM() : Load a SRAM                                */
/*                                                                   */
/*===================================================================*/
int LoadSRAM()
{
    /*
     *  Load a SRAM
     *
     *  Return values
     *     0 : Normally
     *    -1 : SRAM data couldn't be read
     */

    INT32 fp;
    unsigned char pSrcBuf[ SRAM_SIZE ];
    unsigned char chData;
    unsigned char chTag;
    int nRunLen;
    int nDecoded;
    int nDecLen;
    int nIdx;

    // It doesn't need to save it
    nSRAM_SaveFlag = 0;

    // It is finished if the ROM doesn't have SRAM
    if ( !ROM_SRAM )
        return 0;

    // There is necessity to save it
    nSRAM_SaveFlag = 1;

    // The preparation of the SRAM file name
    InfoNES_UCS2Strcpy( szSaveName, szRomName );
    InfoNES_UCS2Strcpy( UCS2Strchr( szSaveName, L'.' ) , L"srm" );

    /*-------------------------------------------------------------------*/
    /*  Read a SRAM data                                                 */
    /*-------------------------------------------------------------------*/

    // Open SRAM file
    fp = FS_Open(szSaveName, FS_O_RDWR, 0x0);
    if ( fp < 0 )
        return -1;

    // Read SRAM data
    FS_Read(fp,pSrcBuf,SRAM_SIZE);

    // Close SRAM file
    FS_Close( fp );

    /*-------------------------------------------------------------------*/
    /*  Extract a SRAM data                                              */
    /*-------------------------------------------------------------------*/

    nDecoded = 0;
    nDecLen = 0;

    chTag = pSrcBuf[ nDecoded++ ];

    while ( nDecLen < 8192 )
    {
        chData = pSrcBuf[ nDecoded++ ];

        if ( chData == chTag )
        {
            chData = pSrcBuf[ nDecoded++ ];
            nRunLen = pSrcBuf[ nDecoded++ ];
            for ( nIdx = 0; nIdx < nRunLen + 1; ++nIdx )
            {
                SRAM[ nDecLen++ ] = chData;
            }
        }
        else
        {
            SRAM[ nDecLen++ ] = chData;
        }
    }

    // Successful
    return 0;
}

/*===================================================================*/
/*                                                                   */
/*           SaveSRAM() : Save a SRAM                                */
/*                                                                   */
/*===================================================================*/
int SaveSRAM()
{
    /*
     *  Save a SRAM
     *
     *  Return values
     *     0 : Normally
     *    -1 : SRAM data couldn't be written
     */

    INT32 fp;
    int nUsedTable[ 256 ];
    unsigned char chData;
    unsigned char chPrevData;
    unsigned char chTag;
    int nIdx;
    int nEncoded;
    int nEncLen;
    int nRunLen;
    unsigned char pDstBuf[ SRAM_SIZE ];

    if ( !nSRAM_SaveFlag )
        return 0;  // It doesn't need to save it

    /*-------------------------------------------------------------------*/
    /*  Compress a SRAM data                                             */
    /*-------------------------------------------------------------------*/

//  memset( nUsedTable, 0, sizeof( nUsedTable ));
    memset( nUsedTable, 0, 256);

    for ( nIdx = 0; nIdx < SRAM_SIZE; ++nIdx )
    {
        ++nUsedTable[ SRAM[ nIdx++ ] ];
    }
    for ( nIdx = 1, chTag = 0; nIdx < 256; ++nIdx )
    {
        if ( nUsedTable[ nIdx ] < nUsedTable[ chTag ] )
            chTag = nIdx;
    }

    nEncoded = 0;
    nEncLen = 0;
    nRunLen = 1;

    pDstBuf[ nEncLen++ ] = chTag;

    chPrevData = SRAM[ nEncoded++ ];

    while ( nEncoded < SRAM_SIZE && nEncLen < SRAM_SIZE - 133 )
    {
        chData = SRAM[ nEncoded++ ];

        if ( chPrevData == chData && nRunLen < 256 )
            ++nRunLen;
        else
        {
            if ( nRunLen >= 4 || chPrevData == chTag )
            {
                pDstBuf[ nEncLen++ ] = chTag;
                pDstBuf[ nEncLen++ ] = chPrevData;
                pDstBuf[ nEncLen++ ] = nRunLen - 1;
            }
            else
            {
                for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
                    pDstBuf[ nEncLen++ ] = chPrevData;
            }

            chPrevData = chData;
            nRunLen = 1;
        }

    }
    if ( nRunLen >= 4 || chPrevData == chTag )
    {
        pDstBuf[ nEncLen++ ] = chTag;
        pDstBuf[ nEncLen++ ] = chPrevData;
        pDstBuf[ nEncLen++ ] = nRunLen - 1;
    }
    else
    {
        for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
            pDstBuf[ nEncLen++ ] = chPrevData;
    }

    /*-------------------------------------------------------------------*/
    /*  Write a SRAM data                                                */
    /*-------------------------------------------------------------------*/

    // Open SRAM file
    fp = FS_Open(szSaveName,FS_O_RDWR, 0x0);
    if ( fp < 0 )
        return -1;

    // Write SRAM data
    FS_Write(fp, pDstBuf, nEncLen);

    // Close SRAM file
    FS_Close( fp );

    // Successful
    return 0;
}

char *InfoNES_UCS2Strcpy(char *strDestination, const char *strSource )
{

    int count =1;
    char *temp = strDestination;

    if( strDestination == NULL ||strSource == NULL  )
        return NULL;

    if( strSource == NULL )
    {
        if(strDestination)
        {
            *(strDestination + count -1 ) = '\0';
            *(strDestination + count  ) = '\0';
        }
        return temp;
    }

    while(  !((*(strSource + count ) == 0 ) &&  (*(strSource + count -1 ) == 0 )))
    {



        *(strDestination + count -1 ) = *(strSource + count -1 );
        *(strDestination + count  ) = *(strSource + count );
        count += 2;
    }

    *(strDestination + count -1 ) = '\0';
    *(strDestination + count  ) = '\0';


    return temp;
}


/* Read ROM image file */
int InfoNES_ReadRomFromSram( const unsigned char *loadSram )
{
    UINT32 read_len;
    unsigned char* p_loadSram = loadSram;
    /* Read ROM Header */
    read_len = sizeof( struct NesHeader_tag  );
    memcpy(&NesHeader,p_loadSram,read_len);
    p_loadSram = p_loadSram + read_len;
    sxs_fprintf(1<<21,"NesHeader.romsize: %d ,readlen %d\n",NesHeader.byRomSize,read_len);
    sxs_fprintf(1<<21,"NesHeader.vromsize: %d \n",NesHeader.byVRomSize);
    sxs_fprintf(1<<21,"NesHeader.info1: %d \n",NesHeader.byInfo1);
    sxs_fprintf(1<<21,"NesHeader.info2: %d \n",NesHeader.byInfo2);

    if( memcmp( NesHeader.byID, "NES\x1a", 4 )!=0)
    {
        /* not .nes file */
        sxs_fprintf(1<<21,"InfoNES_ReadRom byid err \n");
        return -1;
    }

    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if(NesHeader.byInfo1 & 4)
    {
        read_len = 512;
        memcpy(&SRAM[ 0x1000 ],p_loadSram,read_len);
        p_loadSram = p_loadSram + read_len;
    }
    /* Allocate Memory for ROM Image */
    read_len = NesHeader.byRomSize * 0x4000;
    //memcpy(ROM,p_loadSram,read_len);
    ROM = p_loadSram;
    p_loadSram = p_loadSram + read_len;
    if(NesHeader.byVRomSize>0)
    {
        /* Read VROM Image */
        read_len = NesHeader.byVRomSize * 0x2000;
        //memcpy(VROM,p_loadSram,read_len);
        VROM = p_loadSram;
        p_loadSram = p_loadSram + read_len;
    }
    if(p_loadSram > g_load_from_sram + g_load_from_sram_size)
    {
        sxs_fprintf(1<<21,"InfoNES_ReadRom fail \n");
        return -1;
    }
    else
    {
        sxs_fprintf(1<<21,"InfoNES_ReadRom Successful \n");

        /* Successful */
        return 0;
    }
}


/* Read ROM image file */
int InfoNES_ReadRom( const char *pszFileName )
{
    INT32 fp;
    INT32 retn;

    /* Open ROM file */
    fp=FS_Open(pszFileName, FS_O_RDWR, 0x0);

    sxs_fprintf(1<<21,"InfoNES_ReadRom: fp %d \n",fp);
    if(fp < 0) return -1;

    /* Read ROM Header */

    retn = FS_Read(fp, &NesHeader, sizeof( struct NesHeader_tag  ));

    sxs_fprintf(1<<21,"NesHeader.romsize: %d ,readlen %d\n",NesHeader.byRomSize,retn);
    sxs_fprintf(1<<21,"NesHeader.vromsize: %d \n",NesHeader.byVRomSize);
    sxs_fprintf(1<<21,"NesHeader.info1: %d \n",NesHeader.byInfo1);
    sxs_fprintf(1<<21,"NesHeader.info2: %d \n",NesHeader.byInfo2);

    if( memcmp( NesHeader.byID, "NES\x1a", 4 )!=0)
    {
        /* not .nes file */
        FS_Close( fp );
        sxs_fprintf(1<<21,"InfoNES_ReadRom byid err \n");
        return -1;
    }

    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if(NesHeader.byInfo1 & 4)
    {
        FS_Read(fp, &SRAM[ 0x1000 ], 512 );
    }
    /* Allocate Memory for ROM Image */
    ROM = (BYTE *)_sxr_HMalloc( NesHeader.byRomSize * 0x4000,3 );
    if(!ROM) return -1;
    /* Read ROM Image */
    FS_Read(fp, ROM, 0x4000*NesHeader.byRomSize );

    if(NesHeader.byVRomSize>0)
    {
        /* Allocate Memory for VROM Image */
        VROM = (BYTE *)_sxr_HMalloc( NesHeader.byVRomSize * 0x2000,3 );
        if(!VROM) return -1;

        /* Read VROM Image */
        FS_Read( fp,VROM, 0x2000* NesHeader.byVRomSize );
    }

    /* File close */
    FS_Close( fp );
    sxs_fprintf(1<<21,"InfoNES_ReadRom Successful \n");

    /* Successful */
    return 0;

}

/* Release a memory for ROM */
void InfoNES_ReleaseRom()
{
    if((ROM > g_load_from_sram)&&(ROM <= g_load_from_sram + g_load_from_sram_size))
    {
        ROM = NULL;
    }
    else
    {
        if(ROM)
        {
            sxr_Free(ROM);
            ROM=NULL;
        }
    }
    if((VROM > g_load_from_sram)&&(VROM <= g_load_from_sram + g_load_from_sram_size))
    {
        VROM = NULL;
    }
    else
    {
        if(VROM)
        {
            sxr_Free(VROM);
            VROM=NULL;
        }
    }
}

/* Transfer the contents of work frame on the screen */
static WORD *WorkFrame_Crop;//[ LCD_WIDTH_LIB * LCD_HEIGHT_lLIB ];
DWORD InfoNes_FrameSkip = 0;
UINT32 tim_ticks=0;
static UINT8 InfoNes_LcdResolution = 0;
UINT32 InfoNES_LoadFrame_Ex = 0;
void HAL_FUNC_INTERNAL InfoNES_LoadFrame()
{
#define NES_QQVGA
    int i = 0;
    int j = 0;
    int w,h;
    WORD *cur_p = WorkFrame_Crop;

    if( InfoNes_FrameSkip++ % 3 ) return;

    if(InfoNES_Menu())
    {
        sxs_fprintf(1<<21,"InfoNES_LoadFrame quit \n");
        return ;
    }

    if(InfoNes_LcdResolution == 0)   //qvga
    {
        w = 240;
        h = 320;
        for (i = 40; i< (h - 40) ; i++)
        {
            memcpy(WorkFrame_Crop+i*w , WorkFrame+(i-40)*NES_DISP_WIDTH+8 , w*2);
        }
    }
    else if (InfoNes_LcdResolution == 1)     //qqvga
    {
        w = 128;
        h = 160;
        for (i = 0; i< NES_DISP_HEIGHT ; i++)
        {
            for (j = 0 ; j < NES_DISP_WIDTH  ; j++)
            {
                if(j%2)
                {
                    if(i%3)
                    {
                        *(cur_p ++) =  *(WorkFrame+(i*NES_DISP_WIDTH)+j);
                    }
                }
            }
        }
    }

    gdi_layer_blt_previous(0,0,w-1,h-1);

    if( InfoNes_FrameSkip % 20 )
        sxr_Sleep(1);
}

/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
//  sxs_fprintf(1<<21,"InfoNES_PadState pad1 %x \n",dwPad1);

    *pdwPad1 = dwPad1;
    *pdwPad2 = dwPad2;
    *pdwSystem = dwSystem;
}


/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
    memcpy( dest, src, count );
    return dest;
}

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count )
{
    memset( dest, c, count);
    return dest;
}

/* Wait */
void InfoNES_Wait()
{

//  sxs_fprintf(1<<21,"InfoNES_Wait");
//  InfoNES_DebugPrint(__func__);
#if 0
    //if((InfoNes_FrameSkip%2048 )==0) {
    //      InfoNes_task_msg_handle();
    //  }
    if((InfoNes_FrameSkip%20)==10) tim_ticks = hal_TimGetUpTime();
    if((InfoNes_FrameSkip%20 )==0)
    {
        //sxs_fprintf(1<<21,"wait %d \n",hal_TimGetUpTime() - tim_ticks);
        //sxr_Sleep(1);
    }
#endif
}


/* Print system message */
void InfoNES_MessageBox(char *pszMsg, ...)
{
    //InfoNES_DebugPrint("s");
}


void InfoNES_getWorkFrame(void** addr,int* w,int* h)
{
    *addr= WorkFrame;
    *w = NES_DISP_WIDTH;
    *h = NES_DISP_HEIGHT;
}
void InfoNES_NotifyStart(void)
{
    Msg_t *msg;
    msg              = (Msg_t*) _sxr_HMalloc(sizeof(Msg_t),3);
    msg->H.Id        = 0;
    msg->B           = INFONES_MSG_START;
    sxr_Send(msg,COS_TaskMailBoxId(InfoNES_handle),SXR_SEND_MSG);
}
void InfoNes_SetCropWinAddr(void *p,UINT8 res)
{
    WorkFrame_Crop = (WORD*) p ;
    InfoNes_LcdResolution = res;
}
void InfoNES_NotifyQuit(void)
{
    Msg_t *msg;
    msg              = (Msg_t*) _sxr_HMalloc(sizeof(Msg_t),3);
    msg->H.Id        = 0;
    msg->B           = INFONES_MSG_QUIT;
    sxr_Send(msg,COS_TaskMailBoxId(InfoNES_handle),SXR_SEND_MSG);
}

int InfoNES_SetRom(char *filename,unsigned char *sram,unsigned int sram_size)
{
    memset(szRomName,0x0,1024);
    memset(szSaveName,0x0,1024);
    InfoNES_UCS2Strcpy( szRomName, filename );
    g_load_from_sram = sram;
    g_load_from_sram_size = sram_size;
    return 0;
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNes_mach_status = 0;
int InfoNES_Menu()
{
//if return -1 then quit
    return InfoNes_mach_status;
}

int InfoNES_Quit()
{
//if return -1 then quit
    InfoNes_mach_status = -1;
}


/*===================================================================*/
/*                                                                   */
/*                main() : Application main                          */
/*                                                                   */
/*===================================================================*/
void InfoNes_task_msg_handle(void)
{
    COS_EVENT ev;
    Msg_t *NewMsg=NULL;

    if(COS_IsEventAvailable(InfoNES_handle) == 0) {return ;}

    memset(&ev,0,sizeof(COS_EVENT));
    NewMsg = sxr_Wait((u32 *)&ev, COS_TaskMailBoxId(InfoNES_handle));
    if(NewMsg->B == INFONES_MSG_QUIT)
    {
        sxs_fprintf(1<<21,"InfoNES receive quit ...");
        InfoNes_mach_status = -1;

    }
    else
    {
        sxs_fprintf(1<<21,"InfoNes_task_msg_handle other msg");
    }

    sxr_Free(NewMsg);
    NewMsg = NULL;
}
void InfoNES_task_main(void *para)
{
    COS_EVENT ev;
    Msg_t *NewMsg=NULL;
    U32 mem_size = 0;
    /* Help Message */
    sxs_fprintf(1<<21, "Copyright (C) 1998-2006 Jay's Factory, ");
    sxs_fprintf(1<<21, "SDL Ports by mata.\n");
    sxs_fprintf(1<<21, "Usage: %s [NES format file]\n", VERSION);

    while(1)  /* main root for message processing */
    {
        memset(&ev,0,sizeof(COS_EVENT));
        NewMsg = sxr_Wait((u32 *)&ev, COS_TaskMailBoxId(InfoNES_handle));
        sxs_fprintf(1<<21, "InfoNES task main. Event group:%x", NewMsg->B);
        if (NewMsg!= NULL )
        {
            if (NewMsg->B == INFONES_MSG_START)
            {

                if(NewMsg)
                    sxr_Free(NewMsg);
                NewMsg = NULL;
                /* Load cassette */
                InfoNes_mach_status = 0;
                sxs_fprintf(1<<21,"------------before load rom.----------");
                InfoNES_MemStatus();

                //hal_OverlayLoad(HAL_OVERLAY_FAST_CPU_RAM_ID_0);

#ifdef NES_MEM_ALLOC
                mem_size = NES_DISP_WIDTH * NES_DISP_HEIGHT*sizeof(WORD)  /*WorkFrame*/
                           + sizeof(struct ApuEvent_t)*15000   /*ApuEventQueue*/
                           + 0x100 * 0x400                     /*Map85_Chr_Ram*/
                           + 0x2000 * 8                        /*Map5_Wram*/
                           + 0xa000;                           /*DRAM*/

                sxs_fprintf(1<<21,"------------malloc large mem: 0x%x.----------",mem_size);

                Nes_Buff_Pool = (BYTE *)_sxr_HMalloc(mem_size ,3 );
                if(Nes_Buff_Pool != NULL )
                {
                    sxs_fprintf(1<<21,"------------nessesary Mem malloced.----------");
                    memset(Nes_Buff_Pool,0x0,mem_size);
                    WorkFrame = (WORD *)Nes_Buff_Pool;
                    ApuEventQueue = (struct ApuEvent_t *)((BYTE *)WorkFrame + NES_DISP_WIDTH * NES_DISP_HEIGHT*sizeof(WORD));
                    Map85_Chr_Ram = (BYTE *)ApuEventQueue + sizeof(struct ApuEvent_t)*15000;
                    Map5_Wram = (BYTE *)Map85_Chr_Ram + 0x100 * 0x400;
                    DRAM = (BYTE *)Map5_Wram + 0x2000 * 8;

#endif

                    sxs_fprintf(1<<21,"------------start load Rom file.----------");
                    if(InfoNES_Load(szRomName)==0)
                    {
                        /* Load SRAM */
                        // LoadSRAM();

                        sxs_fprintf(1<<21,"------------after load rom.----------");
                        InfoNES_MemStatus();
                        /* MainLoop */
                        InfoNES_Main();
                        /* End */
                        // SaveSRAM();
#ifdef NES_MEM_ALLOC
                    }
                    else
                    {
                        sxs_fprintf(1<<21,"InfoNES InfoNES_Load fail. ");
                        mmi_InfoNESDispMessage(L"Nes load ROM fail.");
                    }
#endif
                }
                else
                {
                    sxs_fprintf(1<<21,"InfoNES Nes_Buff_Pool fail. ");
                    mmi_InfoNESDispMessage(L"Nes Nes_Buff_Pool fail.");
                }
#ifdef NES_MEM_ALLOC
                if(Nes_Buff_Pool)
                {
                    sxr_Free(Nes_Buff_Pool);
                    Nes_Buff_Pool = NULL;

                }
                WorkFrame=NULL;
                DRAM=NULL;
                Map85_Chr_Ram=NULL;
                Map5_Wram=NULL;
                ApuEventQueue=NULL;
#endif
                //hal_OverlayUnload(HAL_OVERLAY_FAST_CPU_RAM_ID_0);
                sxs_fprintf(1<<21,"InfoNES will quit ... ");
            }
            else
            {
                sxs_fprintf(1<<21,"InfoNES receive unknown msg");
                if(NewMsg)
                    sxr_Free(NewMsg);
                NewMsg = NULL;
            }
        }
    }
}

void InfoNES_task_create(void)
{

    InfoNES_handle = COS_CreateTask(InfoNES_task_main,
                                    NULL, NULL,
                                    4096,
                                    235,
                                    0, 0, "InfoNES task");

}


/* End of InfoNES_System_SDL.c */
