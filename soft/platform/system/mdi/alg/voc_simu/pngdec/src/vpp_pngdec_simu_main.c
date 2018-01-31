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



#include <stdio.h>
#include <string.h>

#include "voc2_define.h"
#include "voc2_library.h"
#include "voc2_simulator.h"

#include "vpp_pngdec.h"
#include "vppp_pngdec_asm.h"
#include "vppp_pngdec_asm_common.h"
#include "vppp_pngdec_asm_map.h"
#include "vppp_pngdec_asm_sections.h"




#define TIMES_TO_RUN    10
#define IS_BINARY_SIM   FALSE // FALSE, TRUE


FILE *InFileHandle;
FILE *OutFileHandle;
FILE *DebugFileHandle;

int FileLen;



UINT32 ImgWidth;
UINT32 ImgHeigh;

#pragma pack(1)
/*BMP文件头*/
typedef struct
{
    UINT16 bfType;          //文件类型，2Bytes，值为ox4D42(字符BM)
    UINT32 bfSize;         // 4Bytes,整个文件的大小，单位为字节
    UINT16 bfReserved1;     //保留，2Bytes，置位为0
    UINT16 bfReserved2;     //保留，2Bytes，置位为0
    UINT32 bfOffBits;      //从文件头开始到实际图像数据之间的字节偏移量，4Bytes
} BITMAPFILEHEADER;


/*BMP文件信息头*/
typedef struct
{
    UINT32 biSize;             //BMP信息头大小(字节数)，4Bytes
    INT32 biWidth;             //图像的宽度，以像素为单位，4Bytes
    INT32 biHeight;            //图像的高度，以像素为单位，4Bytes
    UINT16 biPlanes;            //说明目标设备位面数，固定值为1，2Bytes
    UINT16 biBitCount;          //每像素比特数，2Bytes，其值为：1、4、8、16、24、32
    UINT32 biCompression;      //图像数据压缩方法，4Bytes
    UINT32 biSizeImage;        //图像的大小(字节数)，4Bytes，该数必须是4的整数倍，当使用BI_RGB格式时，可置为0
    INT32 biXPelsPerMeter;     //水平分辨率，4Bytes，单位：像素/米
    INT32 biYPelsPerMeter;     //垂直分辨率，4Bytes，单位：像素/米
    UINT32 biClrUsed;          //实际使用的色表中的颜色索引数,4Bytes
    UINT32 biClrImportant;     //对图像显示有重要影响的颜色索引的数目,4Bytes
} BITMAPINFOHEADER;
#pragma pack()


BITMAPFILEHEADER *mypbmfh;
BITMAPINFOHEADER *mypbmih;

unsigned char RGB565Pal[16] = {0x00, 0xf8, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00,
                               0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                              };//rgb565 需要写此16字节

// ******************************
//            inpout data
// ******************************

INT32 g_vppPngDecInbuf[1*1024*1024] =
{
    0
};

// ******************************
//            Outpout data
// ******************************

INT32 g_vppPngDecOutbuf[1*1024*1024] =
{
    0
};

// ******************************
//            Outpout data
// ******************************

INT32 g_vppALphaDecOutbuf[1*1024*1024] =
{
    0
};

// ******************************
//            inpout data
// ******************************

INT32 g_vppBkgrdInbuf[1*1024*1024] =
{
    0//#include "test123.tab"
};

INT32 g_vppWindowbuf[64*1024] =
{
    0xcdcdcdcd
};

VPP_PNGDEC_CFG_T g_vppPngDecCfg;
UINT32 g_count = 0;

void VoC_BCPU_IRQ_handler() {;}

void VoC_XCPU_IRQ_handler()
{
    VPP_PNGDEC_STATUS_T status;

    // get status
    vpp_PngDecStatus(&status);

    // check for errors
    if(status.errorStatus != 0)
    {
        printf("Exiting On Error : Unknown Error !!!");
        return;
    }

    g_count++;

    if (g_count < TIMES_TO_RUN)
    {
        // ----------------
        // config
        // ----------------
        g_vppPngDecCfg.mode         =  0;
        g_vppPngDecCfg.reset        =  1;

        g_vppPngDecCfg.FileContent=g_vppPngDecInbuf;
        g_vppPngDecCfg.FileSize=FileLen;
        g_vppPngDecCfg.PNGOutBuffer=g_vppPngDecOutbuf;
        g_vppPngDecCfg.OutBufferSize=1*1024*1024*4;
        g_vppPngDecCfg.AlphaOutBuffer=g_vppALphaDecOutbuf;
        g_vppPngDecCfg.AlphaBufferSize=1*1024*1024*4;
        g_vppPngDecCfg.BkgrdBuffer=g_vppBkgrdInbuf;
        g_vppPngDecCfg.BkgrdClipX1=0;
        g_vppPngDecCfg.BkgrdClipX2=511;
        g_vppPngDecCfg.BkgrdClipY1=0;
        g_vppPngDecCfg.BkgrdClipY2=511;
        g_vppPngDecCfg.BkgrdOffsetX=20;
        g_vppPngDecCfg.BkgrdOffsetY=20;
        g_vppPngDecCfg.BkgrdWidth=512;
        g_vppPngDecCfg.BkgrdHeight=512;
        g_vppPngDecCfg.ZoomWidth=0;
        g_vppPngDecCfg.ZoomHeight=0;
        g_vppPngDecCfg.DecMode=0;
        g_vppPngDecCfg.alpha_blending_to_transparentColor=0;

        g_vppPngDecCfg.ImgWidth=&ImgWidth;
        g_vppPngDecCfg.ImgHeigh=&ImgHeigh;

        g_vppPngDecCfg.Window=g_vppWindowbuf;


        vpp_PngDecCfg(&g_vppPngDecCfg);
        vpp_PngDecSchedule();
    }

    VoC_counter_stats_minmax(g_count);
    printf("frame:%d\n",g_count);

    return;
}

// WARNING : this is just a trick to pass the main entry to hal
// in voc lib simulation. On the target this function should
// correspond to VoC_XCPU_IRQ_handler
VOID vocMainEntryFct(HAL_VOC_IRQ_STATUS_T* status)
{
    vpp_PngDecMain();
}
/*
  unsigned int CRCtable[256]=
  {
    0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
    0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
    0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
    0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
    0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
    0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
    0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
    0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
    0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
    0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
    0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
    0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
    0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
    0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
    0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
    0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
    0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
    0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
    0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
    0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
    0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
    0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
    0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
    0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
    0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
    0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
    0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
    0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
    0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
    0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
    0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
    0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
    0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
    0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
    0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
    0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
    0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
    0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
    0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
    0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
    0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
    0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
    0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
    0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
    0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
    0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
    0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
    0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
    0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
    0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
    0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
    0x2d02ef8dUL
  };
      unsigned short lbase_c[31] = { // Length codes 257..285 base
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};

      unsigned short lext_c[31] = { // Length codes 257..285 extra
        16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18,
        19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 201, 196};

      unsigned short dbase_c[32] = { // Distance codes 0..29 base
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577, 0, 0};

       unsigned short dext_c[32] = { // Distance codes 0..29 extra
        16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
        23, 23, 24, 24, 25, 25, 26, 26, 27, 27,
        28, 28, 29, 29, 64, 64};


*/

/*

typedef struct {
    unsigned char op;           // operation, extra bits, table bits
    unsigned char bits;         // bits in this part of the code
    unsigned short val;         // offset in table or code value
} strmcode;

    static const strmcode lenfix[512] = {
        {96,7,0},{0,8,80},{0,8,16},{20,8,115},{18,7,31},{0,8,112},{0,8,48},
        {0,9,192},{16,7,10},{0,8,96},{0,8,32},{0,9,160},{0,8,0},{0,8,128},
        {0,8,64},{0,9,224},{16,7,6},{0,8,88},{0,8,24},{0,9,144},{19,7,59},
        {0,8,120},{0,8,56},{0,9,208},{17,7,17},{0,8,104},{0,8,40},{0,9,176},
        {0,8,8},{0,8,136},{0,8,72},{0,9,240},{16,7,4},{0,8,84},{0,8,20},
        {21,8,227},{19,7,43},{0,8,116},{0,8,52},{0,9,200},{17,7,13},{0,8,100},
        {0,8,36},{0,9,168},{0,8,4},{0,8,132},{0,8,68},{0,9,232},{16,7,8},
        {0,8,92},{0,8,28},{0,9,152},{20,7,83},{0,8,124},{0,8,60},{0,9,216},
        {18,7,23},{0,8,108},{0,8,44},{0,9,184},{0,8,12},{0,8,140},{0,8,76},
        {0,9,248},{16,7,3},{0,8,82},{0,8,18},{21,8,163},{19,7,35},{0,8,114},
        {0,8,50},{0,9,196},{17,7,11},{0,8,98},{0,8,34},{0,9,164},{0,8,2},
        {0,8,130},{0,8,66},{0,9,228},{16,7,7},{0,8,90},{0,8,26},{0,9,148},
        {20,7,67},{0,8,122},{0,8,58},{0,9,212},{18,7,19},{0,8,106},{0,8,42},
        {0,9,180},{0,8,10},{0,8,138},{0,8,74},{0,9,244},{16,7,5},{0,8,86},
        {0,8,22},{64,8,0},{19,7,51},{0,8,118},{0,8,54},{0,9,204},{17,7,15},
        {0,8,102},{0,8,38},{0,9,172},{0,8,6},{0,8,134},{0,8,70},{0,9,236},
        {16,7,9},{0,8,94},{0,8,30},{0,9,156},{20,7,99},{0,8,126},{0,8,62},
        {0,9,220},{18,7,27},{0,8,110},{0,8,46},{0,9,188},{0,8,14},{0,8,142},
        {0,8,78},{0,9,252},{96,7,0},{0,8,81},{0,8,17},{21,8,131},{18,7,31},
        {0,8,113},{0,8,49},{0,9,194},{16,7,10},{0,8,97},{0,8,33},{0,9,162},
        {0,8,1},{0,8,129},{0,8,65},{0,9,226},{16,7,6},{0,8,89},{0,8,25},
        {0,9,146},{19,7,59},{0,8,121},{0,8,57},{0,9,210},{17,7,17},{0,8,105},
        {0,8,41},{0,9,178},{0,8,9},{0,8,137},{0,8,73},{0,9,242},{16,7,4},
        {0,8,85},{0,8,21},{16,8,258},{19,7,43},{0,8,117},{0,8,53},{0,9,202},
        {17,7,13},{0,8,101},{0,8,37},{0,9,170},{0,8,5},{0,8,133},{0,8,69},
        {0,9,234},{16,7,8},{0,8,93},{0,8,29},{0,9,154},{20,7,83},{0,8,125},
        {0,8,61},{0,9,218},{18,7,23},{0,8,109},{0,8,45},{0,9,186},{0,8,13},
        {0,8,141},{0,8,77},{0,9,250},{16,7,3},{0,8,83},{0,8,19},{21,8,195},
        {19,7,35},{0,8,115},{0,8,51},{0,9,198},{17,7,11},{0,8,99},{0,8,35},
        {0,9,166},{0,8,3},{0,8,131},{0,8,67},{0,9,230},{16,7,7},{0,8,91},
        {0,8,27},{0,9,150},{20,7,67},{0,8,123},{0,8,59},{0,9,214},{18,7,19},
        {0,8,107},{0,8,43},{0,9,182},{0,8,11},{0,8,139},{0,8,75},{0,9,246},
        {16,7,5},{0,8,87},{0,8,23},{64,8,0},{19,7,51},{0,8,119},{0,8,55},
        {0,9,206},{17,7,15},{0,8,103},{0,8,39},{0,9,174},{0,8,7},{0,8,135},
        {0,8,71},{0,9,238},{16,7,9},{0,8,95},{0,8,31},{0,9,158},{20,7,99},
        {0,8,127},{0,8,63},{0,9,222},{18,7,27},{0,8,111},{0,8,47},{0,9,190},
        {0,8,15},{0,8,143},{0,8,79},{0,9,254},{96,7,0},{0,8,80},{0,8,16},
        {20,8,115},{18,7,31},{0,8,112},{0,8,48},{0,9,193},{16,7,10},{0,8,96},
        {0,8,32},{0,9,161},{0,8,0},{0,8,128},{0,8,64},{0,9,225},{16,7,6},
        {0,8,88},{0,8,24},{0,9,145},{19,7,59},{0,8,120},{0,8,56},{0,9,209},
        {17,7,17},{0,8,104},{0,8,40},{0,9,177},{0,8,8},{0,8,136},{0,8,72},
        {0,9,241},{16,7,4},{0,8,84},{0,8,20},{21,8,227},{19,7,43},{0,8,116},
        {0,8,52},{0,9,201},{17,7,13},{0,8,100},{0,8,36},{0,9,169},{0,8,4},
        {0,8,132},{0,8,68},{0,9,233},{16,7,8},{0,8,92},{0,8,28},{0,9,153},
        {20,7,83},{0,8,124},{0,8,60},{0,9,217},{18,7,23},{0,8,108},{0,8,44},
        {0,9,185},{0,8,12},{0,8,140},{0,8,76},{0,9,249},{16,7,3},{0,8,82},
        {0,8,18},{21,8,163},{19,7,35},{0,8,114},{0,8,50},{0,9,197},{17,7,11},
        {0,8,98},{0,8,34},{0,9,165},{0,8,2},{0,8,130},{0,8,66},{0,9,229},
        {16,7,7},{0,8,90},{0,8,26},{0,9,149},{20,7,67},{0,8,122},{0,8,58},
        {0,9,213},{18,7,19},{0,8,106},{0,8,42},{0,9,181},{0,8,10},{0,8,138},
        {0,8,74},{0,9,245},{16,7,5},{0,8,86},{0,8,22},{64,8,0},{19,7,51},
        {0,8,118},{0,8,54},{0,9,205},{17,7,15},{0,8,102},{0,8,38},{0,9,173},
        {0,8,6},{0,8,134},{0,8,70},{0,9,237},{16,7,9},{0,8,94},{0,8,30},
        {0,9,157},{20,7,99},{0,8,126},{0,8,62},{0,9,221},{18,7,27},{0,8,110},
        {0,8,46},{0,9,189},{0,8,14},{0,8,142},{0,8,78},{0,9,253},{96,7,0},
        {0,8,81},{0,8,17},{21,8,131},{18,7,31},{0,8,113},{0,8,49},{0,9,195},
        {16,7,10},{0,8,97},{0,8,33},{0,9,163},{0,8,1},{0,8,129},{0,8,65},
        {0,9,227},{16,7,6},{0,8,89},{0,8,25},{0,9,147},{19,7,59},{0,8,121},
        {0,8,57},{0,9,211},{17,7,17},{0,8,105},{0,8,41},{0,9,179},{0,8,9},
        {0,8,137},{0,8,73},{0,9,243},{16,7,4},{0,8,85},{0,8,21},{16,8,258},
        {19,7,43},{0,8,117},{0,8,53},{0,9,203},{17,7,13},{0,8,101},{0,8,37},
        {0,9,171},{0,8,5},{0,8,133},{0,8,69},{0,9,235},{16,7,8},{0,8,93},
        {0,8,29},{0,9,155},{20,7,83},{0,8,125},{0,8,61},{0,9,219},{18,7,23},
        {0,8,109},{0,8,45},{0,9,187},{0,8,13},{0,8,141},{0,8,77},{0,9,251},
        {16,7,3},{0,8,83},{0,8,19},{21,8,195},{19,7,35},{0,8,115},{0,8,51},
        {0,9,199},{17,7,11},{0,8,99},{0,8,35},{0,9,167},{0,8,3},{0,8,131},
        {0,8,67},{0,9,231},{16,7,7},{0,8,91},{0,8,27},{0,9,151},{20,7,67},
        {0,8,123},{0,8,59},{0,9,215},{18,7,19},{0,8,107},{0,8,43},{0,9,183},
        {0,8,11},{0,8,139},{0,8,75},{0,9,247},{16,7,5},{0,8,87},{0,8,23},
        {64,8,0},{19,7,51},{0,8,119},{0,8,55},{0,9,207},{17,7,15},{0,8,103},
        {0,8,39},{0,9,175},{0,8,7},{0,8,135},{0,8,71},{0,9,239},{16,7,9},
        {0,8,95},{0,8,31},{0,9,159},{20,7,99},{0,8,127},{0,8,63},{0,9,223},
        {18,7,27},{0,8,111},{0,8,47},{0,9,191},{0,8,15},{0,8,143},{0,8,79},
        {0,9,255}
    };

    static const strmcode distfix[32] = {
        {16,5,1},{23,5,257},{19,5,17},{27,5,4097},{17,5,5},{25,5,1025},
        {21,5,65},{29,5,16385},{16,5,3},{24,5,513},{20,5,33},{28,5,8193},
        {18,5,9},{26,5,2049},{22,5,129},{64,5,0},{16,5,2},{23,5,385},
        {19,5,25},{27,5,6145},{17,5,7},{25,5,1537},{21,5,97},{29,5,24577},
        {16,5,4},{24,5,769},{20,5,49},{28,5,12289},{18,5,13},{26,5,3073},
        {22,5,193},{64,5,0}
    };

*/

int main(int argc, char **argv)
{

    int i;
    VPP_PNGDEC_STATUS_T status;

    unsigned char *filename=NULL;

    int filenamelen;

    for (i=0; i<RAM_X_SIZE/2; i++)
    {
        RAM_X[i]=0xcccccccc;//0xfffffffe;//0x55555555;//0xaaaaaaaa;
    }

    for (i=0; i<RAM_Y_SIZE/2; i++)
    {
        RAM_Y[i]=0xcccccccc;//0xfffffffe;//0x55555555;//0xbbbbbbbb;
    }
    for (i=0; i<RAM_E_SIZE/2; i++)
    {
        RAM_EXTERN[i]=0xcccccccc;
    }

    for (i=0; i<1*1024*1024; i++)
    {
        g_vppBkgrdInbuf[i]=0xffffffff;
    }

    for (i=0; i<64*1024; i++)
    {
        g_vppWindowbuf[i]=0x00cd00cd;
    }

    /*
        {
            int i;
            unsigned short temp_var;
        OutFileHandle = fopen("vpp_Constx_lenfix_distfix.con", "wb");
        if (!OutFileHandle)
        {
            printf("File Open Error!:%s\n",argv[2]);
            return -1;
        }

        for (i=0;i<512;i++)
        {
            temp_var=lenfix[i].op;
            fwrite(&temp_var,2,1,OutFileHandle);
        }
        for (i=0;i<32;i++)
        {
            temp_var=distfix[i].op;
            fwrite(&temp_var,2,1,OutFileHandle);
        }

        for (i=0;i<512;i++)
        {
            temp_var=lenfix[i].bits;
            fwrite(&temp_var,2,1,OutFileHandle);
        }
        for (i=0;i<32;i++)
        {
            temp_var=distfix[i].bits;
            fwrite(&temp_var,2,1,OutFileHandle);
        }

        for (i=0;i<512;i++)
        {
            fwrite(&lenfix[i].val,2,1,OutFileHandle);
        }
        for (i=0;i<32;i++)
        {
            fwrite(&distfix[i].val,2,1,OutFileHandle);
        }

    //  fwrite(CRCtable,1,sizeof(CRCtable),OutFileHandle);
    //  fwrite(lbase_c,1,sizeof(lbase_c),OutFileHandle);
    //  fwrite(lext_c,1,sizeof(lext_c),OutFileHandle);
    //  fwrite(dbase_c,1,sizeof(dbase_c),OutFileHandle);
    //  fwrite(dext_c,1,sizeof(dext_c),OutFileHandle);



        fflush(OutFileHandle);
        fclose(OutFileHandle);


        }


    */


    /* open input file */
    InFileHandle = fopen(argv[1], "rb");

    if (!InFileHandle)
    {
        printf("File Open Error!:%s\n",argv[1]);
        return -1;
    }



    filenamelen=strlen(argv[1]);

    filename= malloc(filenamelen+4+1);

    memcpy(filename,argv[1],filenamelen);


    filename[filenamelen-4]='_';
    filename[filenamelen-3]='V';
    filename[filenamelen-2]='o';
    filename[filenamelen-1]='C';

    filename[filenamelen]='.';
    filename[filenamelen+1]='b';
    filename[filenamelen+2]='m';
    filename[filenamelen+3]='p';
    filename[filenamelen+4]=0x00;


    /* open output file */
    OutFileHandle = fopen(filename, "wb");
    if (!OutFileHandle)
    {
        printf("File Open Error!:%s\n",filename);
        return -1;
    }


    filename= malloc(filenamelen+1);

    memcpy(filename,argv[1],filenamelen+1);


    filename[filenamelen-3]='d';
    filename[filenamelen-2]='b';
    filename[filenamelen-1]='g';


    DebugFileHandle = fopen(filename, "rb");
    if (!DebugFileHandle)
    {
        printf("File Open Error!:%s\n",filename);
        return -1;
    }



    FileLen=fread(((char *)g_vppPngDecInbuf)+3,1,4*1024*1024,InFileHandle);

    printf("FileLen:%d\n",FileLen);

    // ----------------
    // run
    // ----------------
    vpp_PngDecOpen((IS_BINARY_SIM)?NULL:vocMainEntryFct, VPP_PNGDEC_WAKEUP_SW0);


    vpp_PngDecStatus(&status);

    if (status.errorStatus!=0)
    {
        printf("PNG Decoder Error!");
        while(1);
    }

    vpp_PngDecClose();

    mypbmfh = ( BITMAPFILEHEADER *)malloc(14);
    mypbmih = (BITMAPINFOHEADER *)malloc(40);
    //bmp header

    mypbmfh->bfType = 0x4d42;
    mypbmfh->bfReserved1 = 0;
    mypbmfh->bfReserved2 = 0;
    if(g_vppPngDecCfg.DecMode==0)
        mypbmfh->bfSize = 70 + g_vppPngDecCfg.BkgrdWidth*g_vppPngDecCfg.BkgrdHeight*2;
    else if(g_vppPngDecCfg.DecMode==1)
        mypbmfh->bfSize = 70 + ImgWidth*ImgHeigh*2;
    else if(g_vppPngDecCfg.DecMode==2)
        mypbmfh->bfSize = 70 + ImgWidth*ImgHeigh*4;
    mypbmfh->bfOffBits = 70;


    mypbmih->biSize = 40;
    if(g_vppPngDecCfg.DecMode==0)
    {
        mypbmih->biWidth =g_vppPngDecCfg.BkgrdWidth;
        mypbmih->biHeight =-((long)g_vppPngDecCfg.BkgrdHeight);//加-号可以使bmp图像上下倒转显示方式（原本bmp存储时上下倒转）
        mypbmih->biBitCount = 16;
        mypbmih->biCompression = 3;//rgb565 写0颜色不正确。
        mypbmih->biSizeImage= g_vppPngDecCfg.BkgrdWidth*g_vppPngDecCfg.BkgrdHeight*2;
    }
    else if(g_vppPngDecCfg.DecMode==1)
    {
        mypbmih->biWidth =ImgWidth;
        mypbmih->biHeight =-((long)ImgHeigh);//加-号可以使bmp图像上下倒转显示方式（原本bmp存储时上下倒转）
        mypbmih->biBitCount = 16;
        mypbmih->biCompression = 3;//rgb565 写0颜色不正确。
        mypbmih->biSizeImage= ImgWidth*ImgHeigh*2;
    }
    else if(g_vppPngDecCfg.DecMode==2)
    {
        mypbmih->biWidth =ImgWidth;
        mypbmih->biHeight =-((long)ImgHeigh);//加-号可以使bmp图像上下倒转显示方式（原本bmp存储时上下倒转）
        mypbmih->biBitCount = 32;
        mypbmih->biCompression = 0;//rgb565 写0颜色不正确。
        mypbmih->biSizeImage= ImgWidth*ImgHeigh*4;
    }

    mypbmih->biPlanes = 1;
    mypbmih->biXPelsPerMeter= 0;
    mypbmih->biYPelsPerMeter= 0;
    mypbmih->biClrUsed= 0;
    mypbmih->biClrImportant= 0;

    fwrite(mypbmfh, 14, 1, OutFileHandle);
    fflush(OutFileHandle);
    fwrite(mypbmih, 40, 1, OutFileHandle);
    fflush(OutFileHandle);
    fwrite(&RGB565Pal, 16, 1, OutFileHandle);//rgb565 需要写此16字节
    fflush(OutFileHandle);



    if(g_vppPngDecCfg.DecMode==0)
        fwrite(g_vppPngDecCfg.BkgrdBuffer,mypbmih->biSizeImage,1,OutFileHandle);
    else if(g_vppPngDecCfg.DecMode==1)
    {
        fwrite(g_vppPngDecCfg.PNGOutBuffer,mypbmih->biSizeImage,1,OutFileHandle);
        //fwrite(g_vppPngDecCfg.AlphaOutBuffer,ImgWidth*ImgHeigh,1,fAlpha);
    }
    else if(g_vppPngDecCfg.DecMode==2)
        fwrite(g_vppPngDecCfg.PNGOutBuffer,mypbmih->biSizeImage,1,OutFileHandle);


    /*
        {
            FILE *RAM_FileHandle;

            RAM_FileHandle = fopen("RAM_X_VoC", "wb");
            if (!RAM_FileHandle)
            {
                printf("RAM_X Open Error ");
                return -1;
            }

            fwrite(RAM_X,1,20480,RAM_FileHandle);


            fclose(RAM_FileHandle);

            RAM_FileHandle = fopen("RAM_Y_VoC", "wb");
            if (!RAM_FileHandle)
            {
                printf("RAM_Y Open Error ");
                return -1;
            }

            fwrite(RAM_Y,1,20480,RAM_FileHandle);


            fclose(RAM_FileHandle);

            RAM_FileHandle = fopen("WINDOW_VoC", "wb");
            if (!RAM_FileHandle)
            {
                printf("RAM_Y Open Error ");
                return -1;
            }

            fwrite(g_vppWindowbuf,1,64*1024,RAM_FileHandle);


            fclose(RAM_FileHandle);





        }
    */

    /* close files */

    if (OutFileHandle)
    {
        fclose(OutFileHandle);
    }

    if (InFileHandle)
    {
        fclose(InFileHandle);
    }

    if (DebugFileHandle)
    {
        fclose(DebugFileHandle);
    }

    VoC_counter_print(g_count);

    return 0;
}

