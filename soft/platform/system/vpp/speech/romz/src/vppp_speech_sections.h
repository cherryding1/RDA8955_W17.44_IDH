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



#define RAM_I_START               (RAM_I_MIN/2)
#define RAM_I_END                 (RAM_I_MAX/2)

#define SECTION_1_START ( 664/2 + RAM_I_START)
#define SECTION_2_START (4120/2 + RAM_I_START)
#define SECTION_3_START (6448/2 + RAM_I_START)
#define SECTION_4_START (8560/2 + RAM_I_START)


#define SECTION_COMMON_SIZE        (SECTION_1_START - RAM_I_START)
#define SECTION_VOCODER_SIZE       (RAM_I_END       - SECTION_1_START)
#define SECTION_AMR_SPE_SIZE       (SECTION_3_START - SECTION_2_START)
#define SECTION_AMR_ENC_SIZE       (SECTION_4_START - SECTION_3_START)
#define SECTION_AMR_DEC_SIZE       (RAM_I_END       - SECTION_4_START)
#define SECTION_MSDF_SIZE                 16


// start of different image sections in extern RAM

#define SECTION1_IMAGE_START       (0)

#define EFR_AMR_START              (SECTION1_IMAGE_START + 0*SECTION_VOCODER_SIZE)
#define FR_START                   (SECTION1_IMAGE_START + 1*SECTION_VOCODER_SIZE)
#define HR_START                   (SECTION1_IMAGE_START + 2*SECTION_VOCODER_SIZE)


#define SECTION2_IMAGE_START       (SECTION1_IMAGE_START + 3*SECTION_VOCODER_SIZE)

#define AMR_START                  (SECTION2_IMAGE_START + 0*SECTION_AMR_SPE_SIZE)
#define EFR_START                  (SECTION2_IMAGE_START + 1*SECTION_AMR_SPE_SIZE)

#define SECTION3_IMAGE_START       (SECTION2_IMAGE_START + 2*SECTION_AMR_SPE_SIZE)

#define AMR475_ENC_START           (SECTION3_IMAGE_START + 0*SECTION_AMR_ENC_SIZE)
#define AMR515_ENC_START           (SECTION3_IMAGE_START + 1*SECTION_AMR_ENC_SIZE)
#define AMR59_ENC_START            (SECTION3_IMAGE_START + 2*SECTION_AMR_ENC_SIZE)
#define AMR67_ENC_START            (SECTION3_IMAGE_START + 3*SECTION_AMR_ENC_SIZE)
#define AMR74_ENC_START            (SECTION3_IMAGE_START + 4*SECTION_AMR_ENC_SIZE)
#define AMR795_ENC_START           (SECTION3_IMAGE_START + 5*SECTION_AMR_ENC_SIZE)
#define AMR102_ENC_START           (SECTION3_IMAGE_START + 6*SECTION_AMR_ENC_SIZE)
#define AMR122_ENC_START           (SECTION3_IMAGE_START + 7*SECTION_AMR_ENC_SIZE)


#define SECTION4_IMAGE_START       (SECTION3_IMAGE_START + 8*SECTION_AMR_ENC_SIZE)

#define AMR475_DEC_START           (SECTION4_IMAGE_START + 0*SECTION_AMR_DEC_SIZE)
#define AMR515_DEC_START           (SECTION4_IMAGE_START + 1*SECTION_AMR_DEC_SIZE)
#define AMR59_DEC_START            (SECTION4_IMAGE_START + 2*SECTION_AMR_DEC_SIZE)
#define AMR67_DEC_START            (SECTION4_IMAGE_START + 3*SECTION_AMR_DEC_SIZE)
#define AMR74_DEC_START            (SECTION4_IMAGE_START + 4*SECTION_AMR_DEC_SIZE)
#define AMR795_DEC_START           (SECTION4_IMAGE_START + 5*SECTION_AMR_DEC_SIZE)
#define AMR102_DEC_START           (SECTION4_IMAGE_START + 6*SECTION_AMR_DEC_SIZE)
#define AMR122_DEC_START           (SECTION4_IMAGE_START + 7*SECTION_AMR_DEC_SIZE)

#define SECTION5_IMAGE_START       (SECTION4_IMAGE_START + 8*SECTION_AMR_DEC_SIZE)
#define MSDF_START                 (SECTION5_IMAGE_START + 0*SECTION_MSDF_SIZE)


// stack pointers for AMR

#define SP16_ADDR                  0x1000 - 1
#define SP32_ADDR                  0x4800 - 2

// inout buffers memory mapping of AMR encoder
#define  PC_START               0x4//0x0  

