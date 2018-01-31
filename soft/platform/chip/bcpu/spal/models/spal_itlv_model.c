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




#include "global_macros.h"
#include "spalp_common_model.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define Mapping_on_Burst    114        /* length of data(in bits) mapping on burst */


//------------------------------------------------------------------------------
// LUTs
//------------------------------------------------------------------------------


// reordering and partition of coded block of 228 bits for TCH/HS
PRIVATE    UINT8 g_sppMatrixHs[]=
{
    0, 150,   1, 151,
    38, 188,  39, 189,
    76, 226,  77, 227,
    114,  14, 115,  15,
    152,  52, 153,  53,
    190,  90, 191,  91,
    18, 128,  19, 129,
    56, 166,  57, 167,
    94, 204,  95, 205,
    132,  32, 133,  33,
    170,  70, 171,  71,
    208, 108, 209, 109,
    8, 146,   9, 147,
    46, 184,  47, 185,
    84, 222,  85, 223,
    122,  10, 123,  11,
    160,  48, 161,  49,
    198,  86, 199,  87,
    28, 124,  29, 125,
    66, 162,  67, 163,
    104, 200, 105, 201,
    142,  30, 143,  31,
    180,  68, 181,  69,
    218, 106, 219, 107,
    4, 144,   5, 145,
    42, 182,  43, 183,
    80, 220,  81, 221,
    118,   6, 119,   7,
    156,  44, 157,  45,
    194,  82, 195,  83,
    22, 120,  23, 121,
    60, 158,  61, 159,
    98, 196,  99, 197,
    136,  24, 137,  25,
    174,  62, 175,  63,
    212, 100, 213, 101,
    12, 138,  13, 139,
    50, 176,  51, 177,
    88, 214,  89, 215,
    126,   2, 127,   3,
    164,  40, 165,  41,
    202,  78, 203,  79,
    34, 116,  35, 117,
    72, 154,  73, 155,
    110, 192, 111, 193,
    148,  26, 149,  27,
    186,  64, 187,  65,
    224, 102, 225, 103,
    16, 140,  17, 141,
    54, 178,  55, 179,
    92, 216,  93, 217,
    130,  20, 131,  21,
    168,  58, 169,  59,
    206,  96, 207,  97,
    36, 134,  37, 135,
    74, 172,  75, 173,
    112, 210, 113, 211
};

PRIVATE UINT16  InterleavTab15[1248] =
{
    0,463,890,1038,220,371,795,946,582,733,1160,63,490,641,277,428,
    852,1003,185,333,1223,120,547,698,1122,28,915,1066,242,390,817,968,
    610,761,1185,85,512,660,305,453,880,1031,204,355,782,1242,148,575,
    723,1150,50,474,625,1088,267,418,845,993,169,320,1207,113,537,688,
    1115,12,902,1050,232,383,807,958,594,745,1172,75,502,653,289,440,
    864,1015,197,345,1235,132,559,710,1134,40,927,1078,254,402,829,980,
    159,622,773,1197,97,524,672,1099,5,465,892,1043,216,367,794,942,
    587,735,1162,62,486,637,279,430,857,1005,181,332,1219,125,549,700,
    1127,24,914,1062,244,395,819,970,606,757,1184,87,514,665,301,452,
    876,1027,209,357,784,1247,144,571,722,1146,52,479,627,1090,266,414,
    841,992,171,322,1209,109,536,684,1111,17,904,1055,228,379,806,954,
    599,747,1174,74,498,649,291,442,869,1017,193,344,1231,137,561,712,
    1139,36,926,1074,256,407,831,982,158,618,769,1196,99,526,677,1101,
    7,458,894,1033,227,363,802,941,577,740,1152,70,485,645,284,420,
    859,998,189,328,1215,127,542,702,1117,35,922,1061,246,385,824,960,
    605,765,1180,92,504,667,309,448,887,1023,211,350,786,1237,155,567,
    730,1145,54,469,632,1080,274,413,849,988,176,312,1202,117,532,695,
    1107,19,906,1045,239,375,814,953,589,752,1164,82,497,657,296,432,
    871,1010,201,340,1227,139,554,714,1129,47,934,1073,258,397,836,972,
    166,617,777,1192,104,516,679,1094,9,460,899,1035,223,362,798,937,
    579,742,1157,66,481,644,286,425,861,1000,188,324,1214,129,544,707,
    1119,31,918,1057,251,387,826,965,601,764,1176,94,509,669,308,444,
    883,1022,213,352,791,1239,151,566,726,1141,59,471,634,1085,270,409,
    848,984,178,317,1204,116,528,691,1106,21,911,1047,235,374,810,949,
    591,754,1169,78,493,656,298,437,873,1012,200,336,1226,141,556,719,
    1131,43,930,1069,263,399,838,977,162,613,776,1188,106,521,681,1096,
    2,462,889,1040,219,370,797,945,584,732,1159,65,489,640,276,427,
    854,1002,184,335,1222,122,546,697,1124,27,917,1065,241,392,816,967,
    609,760,1187,84,511,662,304,455,879,1030,206,354,781,1244,147,574,
    725,1149,49,476,624,1087,269,417,844,995,168,319,1206,112,539,687,
    1114,14,901,1052,231,382,809,957,596,744,1171,77,501,652,288,439,
    866,1014,196,347,1234,134,558,709,1136,39,929,1077,253,404,828,979,
    161,621,772,1199,96,523,674,1098,4,467,891,1042,218,366,793,944,
    586,737,1161,61,488,636,281,429,856,1007,180,331,1218,124,551,699,
    1126,26,913,1064,243,394,821,969,608,756,1183,89,513,664,300,451,
    878,1026,208,359,783,1246,146,570,721,1148,51,478,629,1089,265,416,
    840,991,173,321,1211,108,535,686,1110,16,903,1054,230,378,805,956,
    598,749,1173,73,500,648,293,441,868,1019,192,343,1230,136,563,711,
    1138,38,925,1076,255,406,833,981,157,620,768,1195,101,525,676,1103,
    6,457,896,1032,226,365,801,940,576,739,1154,69,484,647,283,422,
    858,997,191,327,1217,126,541,704,1116,34,921,1060,248,384,823,962,
    604,767,1179,91,506,666,311,447,886,1025,210,349,788,1236,154,569,
    729,1144,56,468,631,1082,273,412,851,987,175,314,1201,119,531,694,
    1109,18,908,1044,238,377,813,952,588,751,1166,81,496,659,295,434,
    870,1009,203,339,1229,138,553,716,1128,46,933,1072,260,396,835,974,
    165,616,779,1191,103,518,678,1093,11,459,898,1037,222,361,800,936,
    581,741,1156,68,480,643,285,424,863,999,187,326,1213,131,543,706,
    1121,30,920,1056,250,389,825,964,600,763,1178,93,508,671,307,446,
    882,1021,215,351,790,1241,150,565,728,1140,58,473,633,1084,272,408,
    847,986,177,316,1203,115,530,690,1105,23,910,1049,234,373,812,948,
    593,753,1168,80,492,655,297,436,875,1011,199,338,1225,143,555,718,
    1133,42,932,1068,262,401,837,976,164,612,775,1190,105,520,683,1095,
    1,464,888,1039,221,369,796,947,583,734,1158,64,491,639,278,426,
    853,1004,183,334,1221,121,548,696,1123,29,916,1067,240,391,818,966,
    611,759,1186,86,510,661,303,454,881,1029,205,356,780,1243,149,573,
    724,1151,48,475,626,1086,268,419,843,994,170,318,1208,111,538,689,
    1113,13,900,1051,233,381,808,959,595,746,1170,76,503,651,290,438,
    865,1016,195,346,1233,133,560,708,1135,41,928,1079,252,403,830,978,
    160,623,771,1198,98,522,673,1100,3,466,893,1041,217,368,792,943,
    585,736,1163,60,487,638,280,431,855,1006,182,330,1220,123,550,701,
    1125,25,912,1063,245,393,820,971,607,758,1182,88,515,663,302,450,
    877,1028,207,358,785,1245,145,572,720,1147,53,477,628,1091,264,415,
    842,990,172,323,1210,110,534,685,1112,15,905,1053,229,380,804,955,
    597,748,1175,72,499,650,292,443,867,1018,194,342,1232,135,562,713,
    1137,37,924,1075,257,405,832,983,156,619,770,1194,100,527,675,1102,
    8,456,895,1034,225,364,803,939,578,738,1153,71,483,646,282,421,
    860,996,190,329,1216,128,540,703,1118,33,923,1059,247,386,822,961,
    603,766,1181,90,505,668,310,449,885,1024,212,348,787,1238,153,568,
    731,1143,55,470,630,1081,275,411,850,989,174,313,1200,118,533,693,
    1108,20,907,1046,237,376,815,951,590,750,1165,83,495,658,294,433,
    872,1008,202,341,1228,140,552,715,1130,45,935,1071,259,398,834,973,
    167,615,778,1193,102,517,680,1092,10,461,897,1036,224,360,799,938,
    580,743,1155,67,482,642,287,423,862,1001,186,325,1212,130,545,705,
    1120,32,919,1058,249,388,827,963,602,762,1177,95,507,670,306,445,
    884,1020,214,353,789,1240,152,564,727,1142,57,472,635,1083,271,410,
    846,985,179,315,1205,114,529,692,1104,22,909,1048,236,372,811,950,
    592,755,1167,79,494,654,299,435,874,1013,198,337,1224,142,557,717,
    1132,44,931,1070,261,400,839,975,163,614,774,1189,107,519,682,1097
};




//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//======================================================================
// spal_ItlvBusy
//----------------------------------------------------------------------
/// Check the status of the Interleaver
/// To be deleted after
///
/// @return BOOL. TRUE if interleaver is busy, FALSE otherwise.
///
//======================================================================
PUBLIC BOOL spal_ItlvBusy()
{
    // Driver
//    UINT32 pending;
//    pending = ((hwp_itlv->status) & ITLV_BUSY);
//    if (pending)
//        return TRUE;
//    else
    return FALSE;
}

//======================================================================
// spal_ItlvItlv
//----------------------------------------------------------------------
// Interleaving driver
//
// @param ItlvType UINT8. INPUT. ItlvType may take the following values:
// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
// TCH/AFS (speech, ratscch, sid_first).
// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
// - 2 : TYPE_1C. Valid for FACCH/H.
// - 3 : TYPE_2A. Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
// - 4 : TYPE_2B. Valid for TCH/AHS (sid_update).
// - 5 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
// and TCH/H2.4.
// .
// @param burst_offset UINT16. INPUT. Offset position for interleaving (see ITLV
//                      documentation).
// @param nb_bits UINT16. INPUT. Number of bits to interleave.
// @param bb_addr UINT32*. INPUT. Pointer to channel coded burst buffer.
// @param fb_addr UINT32*. OUTPUT. Output buffer pointer.
//
//======================================================================
PUBLIC VOID spal_ItlvItlv(UINT8 ItlvType, UINT16 burst_offset, UINT16 nb_bits, UINT32* bb_addr, UINT32* fb_addr)
{
    // Driver

//    /* Write Burst Base Address */
//    hwp_itlv->burst_base = (UINT32)bb_addr;
//    /* Write Frame Base Address */
//    hwp_itlv->frame_base = (UINT32)fb_addr;
//    /* Configurate the command register*/
//    hwp_itlv->command =
//        ITLV_ITLV_START |
//        ITLV_INT_MASK | // Interruption masked by default
//        ITLV_ITLV_TYPE(ItlvType) |
//        ITLV_BURST_OFFSET(burst_offset) |
//        ITLV_NB_BITS(nb_bits);
//    while (spp_ItlvBusy());
//    // Mandatory as it freezes the module
//    hwp_itlv->int_clear = ITLV_IT_CLEAR;

    UINT8* in;
    UINT8 writePos;
    UINT8* out;//UINT8 out[8][15];//

    UINT32 b, j, k;

    in=(UINT8*) fb_addr;
    writePos= (UINT8) burst_offset;

    out=(UINT8*)((bb_addr)) ;

    switch (ItlvType)
    {

        case ITLV_TYPE_1A: // Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,

            /*interleaving format: i(b,j)=c(n,k) */
            /*    ==>   out(b,j)=in(k)             */

            for (k = 0; k < 456; k++)
            {
                b = (k + writePos) % 8;
                j = 2 * ((49 * k) % 57) + ((k % 8) / 4);

                CLEARBIT( (&out[16*b]) , j);
                MOVEBIT(in, k, (&out[16*b]), j);
            }
            break;

        case ITLV_TYPE_1B:  //Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
            // BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.

            for (k = 0; k < 456; k++)
            {
                b = k % 4;
                j = 2 * ((49 * k) % 57) + (k % 8) / 4;

                CLEARBIT( (&out[16*b]) , j);
                MOVEBIT(in, k, (&out[16*b]), j);
            }


            break;

        case ITLV_TYPE_1C: //Valid for FACCH/H.

            for (k = 0; k < 456; k++)
            {
                // b = k % 8 - 4*((k % 8)/6);
                b = ((k % 8) - 4*((k % 8)/6) + writePos)%8;
                j = 2 * ((49 * k) % 57) + ((k % 8) / 4);
                CLEARBIT2(out,16*b, j);
                MOVEBIT2(in,k, out, 16*b ,j);
            }


            break;

        case ITLV_TYPE_2A: //Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
            for(j=0; j<Mapping_on_Burst; j=j+2)
            {
                for(b=0; b<2; b++)
                {
                    CLEARBIT((&out[16*((b + writePos) % 4)]), j);
                    MOVEBIT(in, g_sppMatrixHs[2*j+b%2], (&out[16*((b + writePos) % 4)]), j);

                }
            }

            for(j=1; j<Mapping_on_Burst; j=j+2)
            {
                for(b=2; b<4; b++)
                {
                    CLEARBIT((&out[16*((b + writePos) % 4)]), j);
                    MOVEBIT(in, g_sppMatrixHs[2*j+b%2], (&out[16*((b + writePos) % 4)]), j);

                }
            }

            break;
        case ITLV_TYPE_3:    //Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
            // and TCH/H2.4.
            for (k = 0; k < 456; k++)
            {
                //b = k % 19 + k / 114;
                b = (k % 19 + k / 114 + writePos) % 24;
                j = k % 19 + 19 * (k % 6);

                CLEARBIT( (&out[16*b]) , j);
                MOVEBIT(in, k, (&out[16*b]), j);

            }

            break;
        case ITLV_TYPE_D1: //MCS5,6
            for (k = 0; k < 1248; k++)
            {
                j = InterleavTab15[k];
                CLEARBIT((out), j);
                MOVEBIT(in, k, (out), j);
            }

            break;

        case ITLV_TYPE_D2: //MCS7

            for (k = 0; k < 1224; k++)
            {
                j = 306 * (k % 4) + 3 * (((44 * k) % 102) + (k / 4) % 2) + (k + 2 - (k / 408)) % 3;
                CLEARBIT((out), j);
                MOVEBIT(in, k, (out), j);
            }
            break;
        case ITLV_TYPE_D3: // MCS8, MCS9
            // data interleaving

            for (k = 0; k < 1224; k++)
            {
                j = 306 * (2 * (k / 612) + (k % 2)) + 3 * (((74 * k) % 102) + (k / 2) % 2)
                    + (k + 2 - (k / 204)) % 3;
                CLEARBIT((out), j);
                MOVEBIT(in, k, (out), j);
            }
            break;

        case ITLV_TYPE_H2: //MCS5_UL,MCS6_UL
            // header interleaving
            for (k = 0; k < 136; k++)
            {
                j = 34 * (k % 4) + 2 * ((11 * k) % 17) + (k % 8) / 4;
                CLEARBIT((out), j);
                MOVEBIT(in, k, (out), j);
            }
            break;

        case ITLV_TYPE_H4: //MCS7_UL,MCS8_UL, MCS9_UL

            // header interleaving
            for (k = 0; k < 160; k++)
            {
                j = 40 * (k % 4) + 2 * ((13 * (k / 8)) % 20) + ((k % 8) / 4);
                CLEARBIT((out), j);
                MOVEBIT(in, k, (out), j);
            }
            break;
    }

}


//======================================================================
// spal_ItlvDitlv
//----------------------------------------------------------------------
// Deinterleaving driver
//
// @param ItlvType UINT8. ItlvType may take the following values:
// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
// TCH/AFS (speech, ratscch, sid_first).
// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
// - 2 : TYPE_1C. Valid for FACCH/H.
// - 3 : TYPE_2. Valid for TCH/HS and TCH/AHS.
// - 4 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
// and TCH/H2.4.
// .
// @param burst_offset UINT16. INPUT. Offset position for deinterleaving (see ITLV
//                      documentation).
// @param nb_bits UINT16. INPUT. Number of bits to deinterleave.
// @param bb_addr UINT32*. INPUT. Pointer to equalized bursts buffer.
// @param fb_addr UINT32*. OUTPUT. Output buffer pointer.
//
//======================================================================
PUBLIC VOID spal_ItlvDitlv(UINT8 ItlvType, UINT16 burst_offset, UINT16 nb_bits, UINT32* bb_addr, UINT32* fb_addr)
{
    // Driver
//     /* Write Burst Base Address */
//     hwp_itlv->burst_base = (UINT32)bb_addr;
//     /* Write Frame Base Address */
//     hwp_itlv->frame_base = (UINT32)fb_addr;
//     /* Configurate the command register*/
//     hwp_itlv->command =
//         ITLV_DITLV_START |
//         ITLV_INT_MASK | // Interruption masked by default
//         ITLV_ITLV_TYPE(ItlvType) |
//         ITLV_BURST_OFFSET(burst_offset) |
//         ITLV_NB_BITS(nb_bits);
//     while (spp_ItlvBusy());
//
//     // Mandatory as it freezes the module
//     hwp_itlv->int_clear = ITLV_IT_CLEAR;



    // Software equivalent

//void CC_Deinterleave(UINT8 in[][114], UINT8* out, enum eIntrlvMode IM, UINT8 readPos)
//{
    UINT32 b, j, k;
    UINT8* in;
    UINT8 readPos;
    UINT8* out;

    in=(UINT8*) bb_addr;
    readPos= (UINT8) burst_offset;
    out=(UINT8*) fb_addr;


    switch(ItlvType)
    {
        case ITLV_TYPE_1A:
            for (k = 0; k < 456; k++)
            {
                b = (k + readPos) % 8;
                j = 2 * ((49 * k) % 57) + ((k % 8) / 4);
                out[k] = in[32*4*b+j];
            }
            break;
        case ITLV_TYPE_1B:

            for (k = 0; k < 456; k++)
            {
                b = k % 4;
                j = 2 * ((49 * k) % 57) + (k % 8) / 4;
                out[k] = in[32*4*b+j];
            }

            break;


        case ITLV_TYPE_1C:
            for (k = 0; k < 456; k++)
            {
                //b = ((k + readPos) % 8) - 4*((k % 8)/6);
                b = ((k % 8) - 4*((k % 8)/6) + readPos)%8;
                j = 2 * ((49 * k) % 57) + ((k % 8) / 4);
                out[k] = in[32*4*b+j];
            }
            break;

        // according to the table4,if we know b(BURST: 0,1,2,3) and j(0-113),
        // then we will know k with the formula k=MATRIX_HS[2*j+b%2]
        // then c(k)=i(b,j)
        case ITLV_TYPE_2A:
            for(j=0; j<Mapping_on_Burst; j=j+2)
            {
                for(b=0; b<2; b++)
                {
                    out[g_sppMatrixHs[2*j+b%2]] = in[32*4*((b + readPos) % 4)+j];
                }
            }
            for(j=1; j<Mapping_on_Burst; j=j+2)
            {
                for(b=2; b<4; b++)
                {
                    out[g_sppMatrixHs[2*j+b%2]] = in[32*4*((b + readPos) % 4)+j];
                }
            }
            break;
        case ITLV_TYPE_3:
            for (k = 0; k < 456; k++)
            {
                b = (k % 19 + k / 114 + readPos) % 24;
                j = k % 19 + 19 * (k % 6);
                out[k] = in[32*4*b+j];
            }
            break;

        case ITLV_TYPE_D1: //MCS 5,6
            for (k = 0; k < 1248; k++)
            {
                j = InterleavTab15[k];
                out[k] = in[j];
            }
            break;

        case ITLV_TYPE_D2: //MCS 7
            for (k = 0; k < 1224; k++)
            {
                j = 306 * (k % 4) + 3 * (((44 * k) % 102) + (k / 4) % 2) + (k + 2 - (k / 408)) % 3;
                out[k] = in[j];
            }
            break;

        case ITLV_TYPE_D3: //MCS 8,9
            for (k = 0; k < 1224; k++)
            {
                j = 306 * (2 * (k / 612) + (k % 2)) + 3 * (((74 * k) % 102) + (k / 2) % 2)
                    + (k + 2 - (k / 204)) % 3;
                out[k] = in[j];
            }
            break;

        case ITLV_TYPE_H1: //MCS 5,6
            for (k = 0; k < 100; k++)
            {
                j = 25 * (k % 4) + (17 * k) % 25;
                out[k] = in[j];
            }
            break;

        case ITLV_TYPE_H3: //MCS 7,8,9
            for (k = 0; k < 124; k++)
            {
                j = 31 * (k % 4) + (17 * k) % 31;
                out[k] = in[j];
            }
            break;

        default:
            for(k = 0; k < 372; k++)
            {
                out[k] = in[k];
            }
            break;


    }
}

//======================================================================
// spal_ItlvIntClear
//----------------------------------------------------------------------
// Clear the transfer done interrupt
//
//======================================================================
PUBLIC VOID spal_ItlvIntClear()
{
    // Driver
//    // Clear the transfer done interrupt
//    hwp_itlv->int_clear = ITLV_IT_CLEAR;
}





























































