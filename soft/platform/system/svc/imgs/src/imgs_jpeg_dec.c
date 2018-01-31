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


#include "cs_types.h"

#include "stdio.h"
#include "string.h"

#include "sxr_mem.h"

#include "imgsp_debug.h"
#include "imgsp_jpeg_dec.h"

#include "fs.h"


PRIVATE CONST INT32 g_imgsAnd[33]= {0,1,3,7,0xf,0x1f,0x3f,0x7f,0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
                                    0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,0x7fffff,0xffffff,0x1ffffff,0x3ffffff,
                                    0x7ffffff,0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff,
                                   };
PRIVATE CONST INT32 g_imgsInverseZigZag[64]=
{
    0,1,8,16,9,2,3,10,
    17,24,32,25,18,11,4,5,
    12,19,26,33,40,48,41,34,
    27,20,13,6,7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63
};
PRIVATE CONST INT16  g_imgsAanScales[DCTSIZE2] =
{
    // precomputed values scaled up by 14 bits
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
    21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
    19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
    8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
    4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
};
PRIVATE CONST INT32 g_imgsCrRTab[256]=
{
    0xffffff4d  ,0xffffff4e  ,0xffffff4f  ,0xffffff51  ,0xffffff52  ,0xffffff54  ,0xffffff55  ,0xffffff56  ,
    0xffffff58  ,0xffffff59  ,0xffffff5b  ,0xffffff5c  ,0xffffff5d  ,0xffffff5f  ,0xffffff60  ,0xffffff62  ,
    0xffffff63  ,0xffffff64  ,0xffffff66  ,0xffffff67  ,0xffffff69  ,0xffffff6a  ,0xffffff6b  ,0xffffff6d  ,
    0xffffff6e  ,0xffffff70  ,0xffffff71  ,0xffffff72  ,0xffffff74  ,0xffffff75  ,0xffffff77  ,0xffffff78  ,
    0xffffff79  ,0xffffff7b  ,0xffffff7c  ,0xffffff7e  ,0xffffff7f  ,0xffffff80  ,0xffffff82  ,0xffffff83  ,
    0xffffff85  ,0xffffff86  ,0xffffff87  ,0xffffff89  ,0xffffff8a  ,0xffffff8c  ,0xffffff8d  ,0xffffff8e  ,
    0xffffff90  ,0xffffff91  ,0xffffff93  ,0xffffff94  ,0xffffff95  ,0xffffff97  ,0xffffff98  ,0xffffff9a  ,
    0xffffff9b  ,0xffffff9c  ,0xffffff9e  ,0xffffff9f  ,0xffffffa1  ,0xffffffa2  ,0xffffffa3  ,0xffffffa5  ,
    0xffffffa6  ,0xffffffa8  ,0xffffffa9  ,0xffffffaa  ,0xffffffac  ,0xffffffad  ,0xffffffaf  ,0xffffffb0  ,
    0xffffffb1  ,0xffffffb3  ,0xffffffb4  ,0xffffffb6  ,0xffffffb7  ,0xffffffb8  ,0xffffffba  ,0xffffffbb  ,
    0xffffffbd  ,0xffffffbe  ,0xffffffc0  ,0xffffffc1  ,0xffffffc2  ,0xffffffc4  ,0xffffffc5  ,0xffffffc7  ,
    0xffffffc8  ,0xffffffc9  ,0xffffffcb  ,0xffffffcc  ,0xffffffce  ,0xffffffcf  ,0xffffffd0  ,0xffffffd2  ,
    0xffffffd3  ,0xffffffd5  ,0xffffffd6  ,0xffffffd7  ,0xffffffd9  ,0xffffffda  ,0xffffffdc  ,0xffffffdd  ,
    0xffffffde  ,0xffffffe0  ,0xffffffe1  ,0xffffffe3  ,0xffffffe4  ,0xffffffe5  ,0xffffffe7  ,0xffffffe8  ,
    0xffffffea  ,0xffffffeb  ,0xffffffec  ,0xffffffee  ,0xffffffef  ,0xfffffff1  ,0xfffffff2  ,0xfffffff3  ,
    0xfffffff5  ,0xfffffff6  ,0xfffffff8  ,0xfffffff9  ,0xfffffffa  ,0xfffffffc  ,0xfffffffd  ,0xffffffff  ,
    0x0  ,0x1  ,0x3  ,0x4  ,0x6  ,0x7  ,0x8  ,0xa  ,
    0xb  ,0xd  ,0xe  ,0xf  ,0x11  ,0x12  ,0x14  ,0x15  ,
    0x16  ,0x18  ,0x19  ,0x1b  ,0x1c  ,0x1d  ,0x1f  ,0x20  ,
    0x22  ,0x23  ,0x24  ,0x26  ,0x27  ,0x29  ,0x2a  ,0x2b  ,
    0x2d  ,0x2e  ,0x30  ,0x31  ,0x32  ,0x34  ,0x35  ,0x37  ,
    0x38  ,0x39  ,0x3b  ,0x3c  ,0x3e  ,0x3f  ,0x40  ,0x42  ,
    0x43  ,0x45  ,0x46  ,0x48  ,0x49  ,0x4a  ,0x4c  ,0x4d  ,
    0x4f  ,0x50  ,0x51  ,0x53  ,0x54  ,0x56  ,0x57  ,0x58  ,
    0x5a  ,0x5b  ,0x5d  ,0x5e  ,0x5f  ,0x61  ,0x62  ,0x64  ,
    0x65  ,0x66  ,0x68  ,0x69  ,0x6b  ,0x6c  ,0x6d  ,0x6f  ,
    0x70  ,0x72  ,0x73  ,0x74  ,0x76  ,0x77  ,0x79  ,0x7a  ,
    0x7b  ,0x7d  ,0x7e  ,0x80  ,0x81  ,0x82  ,0x84  ,0x85  ,
    0x87  ,0x88  ,0x89  ,0x8b  ,0x8c  ,0x8e  ,0x8f  ,0x90  ,
    0x92  ,0x93  ,0x95  ,0x96  ,0x97  ,0x99  ,0x9a  ,0x9c  ,
    0x9d  ,0x9e  ,0xa0  ,0xa1  ,0xa3  ,0xa4  ,0xa5  ,0xa7  ,
    0xa8  ,0xaa  ,0xab  ,0xac  ,0xae  ,0xaf  ,0xb1  ,0xb2  ,
};
PRIVATE CONST INT32 g_imgsCbBTab[256]=
{
    0xffffff1d  ,0xffffff1f  ,0xffffff21  ,0xffffff22  ,0xffffff24  ,0xffffff26  ,0xffffff28  ,0xffffff2a  ,
    0xffffff2b  ,0xffffff2d  ,0xffffff2f  ,0xffffff31  ,0xffffff32  ,0xffffff34  ,0xffffff36  ,0xffffff38  ,
    0xffffff3a  ,0xffffff3b  ,0xffffff3d  ,0xffffff3f  ,0xffffff41  ,0xffffff42  ,0xffffff44  ,0xffffff46  ,
    0xffffff48  ,0xffffff49  ,0xffffff4b  ,0xffffff4d  ,0xffffff4f  ,0xffffff51  ,0xffffff52  ,0xffffff54  ,
    0xffffff56  ,0xffffff58  ,0xffffff59  ,0xffffff5b  ,0xffffff5d  ,0xffffff5f  ,0xffffff61  ,0xffffff62  ,
    0xffffff64  ,0xffffff66  ,0xffffff68  ,0xffffff69  ,0xffffff6b  ,0xffffff6d  ,0xffffff6f  ,0xffffff70  ,
    0xffffff72  ,0xffffff74  ,0xffffff76  ,0xffffff78  ,0xffffff79  ,0xffffff7b  ,0xffffff7d  ,0xffffff7f  ,
    0xffffff80  ,0xffffff82  ,0xffffff84  ,0xffffff86  ,0xffffff88  ,0xffffff89  ,0xffffff8b  ,0xffffff8d  ,
    0xffffff8f  ,0xffffff90  ,0xffffff92  ,0xffffff94  ,0xffffff96  ,0xffffff97  ,0xffffff99  ,0xffffff9b  ,
    0xffffff9d  ,0xffffff9f  ,0xffffffa0  ,0xffffffa2  ,0xffffffa4  ,0xffffffa6  ,0xffffffa7  ,0xffffffa9  ,
    0xffffffab  ,0xffffffad  ,0xffffffae  ,0xffffffb0  ,0xffffffb2  ,0xffffffb4  ,0xffffffb6  ,0xffffffb7  ,
    0xffffffb9  ,0xffffffbb  ,0xffffffbd  ,0xffffffbe  ,0xffffffc0  ,0xffffffc2  ,0xffffffc4  ,0xffffffc6  ,
    0xffffffc7  ,0xffffffc9  ,0xffffffcb  ,0xffffffcd  ,0xffffffce  ,0xffffffd0  ,0xffffffd2  ,0xffffffd4  ,
    0xffffffd5  ,0xffffffd7  ,0xffffffd9  ,0xffffffdb  ,0xffffffdd  ,0xffffffde  ,0xffffffe0  ,0xffffffe2  ,
    0xffffffe4  ,0xffffffe5  ,0xffffffe7  ,0xffffffe9  ,0xffffffeb  ,0xffffffed  ,0xffffffee  ,0xfffffff0  ,
    0xfffffff2  ,0xfffffff4  ,0xfffffff5  ,0xfffffff7  ,0xfffffff9  ,0xfffffffb  ,0xfffffffc  ,0xfffffffe  ,
    0x0  ,0x2  ,0x4  ,0x5  ,0x7  ,0x9  ,0xb  ,0xc  ,
    0xe  ,0x10  ,0x12  ,0x13  ,0x15  ,0x17  ,0x19  ,0x1b  ,
    0x1c  ,0x1e  ,0x20  ,0x22  ,0x23  ,0x25  ,0x27  ,0x29  ,
    0x2b  ,0x2c  ,0x2e  ,0x30  ,0x32  ,0x33  ,0x35  ,0x37  ,
    0x39  ,0x3a  ,0x3c  ,0x3e  ,0x40  ,0x42  ,0x43  ,0x45  ,
    0x47  ,0x49  ,0x4a  ,0x4c  ,0x4e  ,0x50  ,0x52  ,0x53  ,
    0x55  ,0x57  ,0x59  ,0x5a  ,0x5c  ,0x5e  ,0x60  ,0x61  ,
    0x63  ,0x65  ,0x67  ,0x69  ,0x6a  ,0x6c  ,0x6e  ,0x70  ,
    0x71  ,0x73  ,0x75  ,0x77  ,0x78  ,0x7a  ,0x7c  ,0x7e  ,
    0x80  ,0x81  ,0x83  ,0x85  ,0x87  ,0x88  ,0x8a  ,0x8c  ,
    0x8e  ,0x90  ,0x91  ,0x93  ,0x95  ,0x97  ,0x98  ,0x9a  ,
    0x9c  ,0x9e  ,0x9f  ,0xa1  ,0xa3  ,0xa5  ,0xa7  ,0xa8  ,
    0xaa  ,0xac  ,0xae  ,0xaf  ,0xb1  ,0xb3  ,0xb5  ,0xb7  ,
    0xb8  ,0xba  ,0xbc  ,0xbe  ,0xbf  ,0xc1  ,0xc3  ,0xc5  ,
    0xc6  ,0xc8  ,0xca  ,0xcc  ,0xce  ,0xcf  ,0xd1  ,0xd3  ,
    0xd5  ,0xd6  ,0xd8  ,0xda  ,0xdc  ,0xde  ,0xdf  ,0xe1  ,
};
PRIVATE CONST INT32 g_imgsCrGTab[256]=
{
    0x5b6900  ,0x5ab22e  ,0x59fb5c  ,0x59448a  ,0x588db8  ,0x57d6e6  ,0x572014  ,0x566942  ,
    0x55b270  ,0x54fb9e  ,0x5444cc  ,0x538dfa  ,0x52d728  ,0x522056  ,0x516984  ,0x50b2b2  ,
    0x4ffbe0  ,0x4f450e  ,0x4e8e3c  ,0x4dd76a  ,0x4d2098  ,0x4c69c6  ,0x4bb2f4  ,0x4afc22  ,
    0x4a4550  ,0x498e7e  ,0x48d7ac  ,0x4820da  ,0x476a08  ,0x46b336  ,0x45fc64  ,0x454592  ,
    0x448ec0  ,0x43d7ee  ,0x43211c  ,0x426a4a  ,0x41b378  ,0x40fca6  ,0x4045d4  ,0x3f8f02  ,
    0x3ed830  ,0x3e215e  ,0x3d6a8c  ,0x3cb3ba  ,0x3bfce8  ,0x3b4616  ,0x3a8f44  ,0x39d872  ,
    0x3921a0  ,0x386ace  ,0x37b3fc  ,0x36fd2a  ,0x364658  ,0x358f86  ,0x34d8b4  ,0x3421e2  ,
    0x336b10  ,0x32b43e  ,0x31fd6c  ,0x31469a  ,0x308fc8  ,0x2fd8f6  ,0x2f2224  ,0x2e6b52  ,
    0x2db480  ,0x2cfdae  ,0x2c46dc  ,0x2b900a  ,0x2ad938  ,0x2a2266  ,0x296b94  ,0x28b4c2  ,
    0x27fdf0  ,0x27471e  ,0x26904c  ,0x25d97a  ,0x2522a8  ,0x246bd6  ,0x23b504  ,0x22fe32  ,
    0x224760  ,0x21908e  ,0x20d9bc  ,0x2022ea  ,0x1f6c18  ,0x1eb546  ,0x1dfe74  ,0x1d47a2  ,
    0x1c90d0  ,0x1bd9fe  ,0x1b232c  ,0x1a6c5a  ,0x19b588  ,0x18feb6  ,0x1847e4  ,0x179112  ,
    0x16da40  ,0x16236e  ,0x156c9c  ,0x14b5ca  ,0x13fef8  ,0x134826  ,0x129154  ,0x11da82  ,
    0x1123b0  ,0x106cde  ,0xfb60c  ,0xeff3a  ,0xe4868  ,0xd9196  ,0xcdac4  ,0xc23f2  ,
    0xb6d20  ,0xab64e  ,0x9ff7c  ,0x948aa  ,0x891d8  ,0x7db06  ,0x72434  ,0x66d62  ,
    0x5b690  ,0x4ffbe  ,0x448ec  ,0x3921a  ,0x2db48  ,0x22476  ,0x16da4  ,0xb6d2  ,
    0x0  ,0xffff492e  ,0xfffe925c  ,0xfffddb8a  ,0xfffd24b8  ,0xfffc6de6  ,0xfffbb714  ,0xfffb0042  ,
    0xfffa4970  ,0xfff9929e  ,0xfff8dbcc  ,0xfff824fa  ,0xfff76e28  ,0xfff6b756  ,0xfff60084  ,0xfff549b2  ,
    0xfff492e0  ,0xfff3dc0e  ,0xfff3253c  ,0xfff26e6a  ,0xfff1b798  ,0xfff100c6  ,0xfff049f4  ,0xffef9322  ,
    0xffeedc50  ,0xffee257e  ,0xffed6eac  ,0xffecb7da  ,0xffec0108  ,0xffeb4a36  ,0xffea9364  ,0xffe9dc92  ,
    0xffe925c0  ,0xffe86eee  ,0xffe7b81c  ,0xffe7014a  ,0xffe64a78  ,0xffe593a6  ,0xffe4dcd4  ,0xffe42602  ,
    0xffe36f30  ,0xffe2b85e  ,0xffe2018c  ,0xffe14aba  ,0xffe093e8  ,0xffdfdd16  ,0xffdf2644  ,0xffde6f72  ,
    0xffddb8a0  ,0xffdd01ce  ,0xffdc4afc  ,0xffdb942a  ,0xffdadd58  ,0xffda2686  ,0xffd96fb4  ,0xffd8b8e2  ,
    0xffd80210  ,0xffd74b3e  ,0xffd6946c  ,0xffd5dd9a  ,0xffd526c8  ,0xffd46ff6  ,0xffd3b924  ,0xffd30252  ,
    0xffd24b80  ,0xffd194ae  ,0xffd0dddc  ,0xffd0270a  ,0xffcf7038  ,0xffceb966  ,0xffce0294  ,0xffcd4bc2  ,
    0xffcc94f0  ,0xffcbde1e  ,0xffcb274c  ,0xffca707a  ,0xffc9b9a8  ,0xffc902d6  ,0xffc84c04  ,0xffc79532  ,
    0xffc6de60  ,0xffc6278e  ,0xffc570bc  ,0xffc4b9ea  ,0xffc40318  ,0xffc34c46  ,0xffc29574  ,0xffc1dea2  ,
    0xffc127d0  ,0xffc070fe  ,0xffbfba2c  ,0xffbf035a  ,0xffbe4c88  ,0xffbd95b6  ,0xffbcdee4  ,0xffbc2812  ,
    0xffbb7140  ,0xffbaba6e  ,0xffba039c  ,0xffb94cca  ,0xffb895f8  ,0xffb7df26  ,0xffb72854  ,0xffb67182  ,
    0xffb5bab0  ,0xffb503de  ,0xffb44d0c  ,0xffb3963a  ,0xffb2df68  ,0xffb22896  ,0xffb171c4  ,0xffb0baf2  ,
    0xffb00420  ,0xffaf4d4e  ,0xffae967c  ,0xffaddfaa  ,0xffad28d8  ,0xffac7206  ,0xffabbb34  ,0xffab0462  ,
    0xffaa4d90  ,0xffa996be  ,0xffa8dfec  ,0xffa8291a  ,0xffa77248  ,0xffa6bb76  ,0xffa604a4  ,0xffa54dd2  ,
};
PRIVATE CONST INT32 g_imgsCbGTab[256]=
{
    0x2c8d00  ,0x2c34e6  ,0x2bdccc  ,0x2b84b2  ,0x2b2c98  ,0x2ad47e  ,0x2a7c64  ,0x2a244a  ,
    0x29cc30  ,0x297416  ,0x291bfc  ,0x28c3e2  ,0x286bc8  ,0x2813ae  ,0x27bb94  ,0x27637a  ,
    0x270b60  ,0x26b346  ,0x265b2c  ,0x260312  ,0x25aaf8  ,0x2552de  ,0x24fac4  ,0x24a2aa  ,
    0x244a90  ,0x23f276  ,0x239a5c  ,0x234242  ,0x22ea28  ,0x22920e  ,0x2239f4  ,0x21e1da  ,
    0x2189c0  ,0x2131a6  ,0x20d98c  ,0x208172  ,0x202958  ,0x1fd13e  ,0x1f7924  ,0x1f210a  ,
    0x1ec8f0  ,0x1e70d6  ,0x1e18bc  ,0x1dc0a2  ,0x1d6888  ,0x1d106e  ,0x1cb854  ,0x1c603a  ,
    0x1c0820  ,0x1bb006  ,0x1b57ec  ,0x1affd2  ,0x1aa7b8  ,0x1a4f9e  ,0x19f784  ,0x199f6a  ,
    0x194750  ,0x18ef36  ,0x18971c  ,0x183f02  ,0x17e6e8  ,0x178ece  ,0x1736b4  ,0x16de9a  ,
    0x168680  ,0x162e66  ,0x15d64c  ,0x157e32  ,0x152618  ,0x14cdfe  ,0x1475e4  ,0x141dca  ,
    0x13c5b0  ,0x136d96  ,0x13157c  ,0x12bd62  ,0x126548  ,0x120d2e  ,0x11b514  ,0x115cfa  ,
    0x1104e0  ,0x10acc6  ,0x1054ac  ,0xffc92  ,0xfa478  ,0xf4c5e  ,0xef444  ,0xe9c2a  ,
    0xe4410  ,0xdebf6  ,0xd93dc  ,0xd3bc2  ,0xce3a8  ,0xc8b8e  ,0xc3374  ,0xbdb5a  ,
    0xb8340  ,0xb2b26  ,0xad30c  ,0xa7af2  ,0xa22d8  ,0x9cabe  ,0x972a4  ,0x91a8a  ,
    0x8c270  ,0x86a56  ,0x8123c  ,0x7ba22  ,0x76208  ,0x709ee  ,0x6b1d4  ,0x659ba  ,
    0x601a0  ,0x5a986  ,0x5516c  ,0x4f952  ,0x4a138  ,0x4491e  ,0x3f104  ,0x398ea  ,
    0x340d0  ,0x2e8b6  ,0x2909c  ,0x23882  ,0x1e068  ,0x1884e  ,0x13034  ,0xd81a  ,
    0x8000  ,0x27e6  ,0xffffcfcc  ,0xffff77b2  ,0xffff1f98  ,0xfffec77e  ,0xfffe6f64  ,0xfffe174a  ,
    0xfffdbf30  ,0xfffd6716  ,0xfffd0efc  ,0xfffcb6e2  ,0xfffc5ec8  ,0xfffc06ae  ,0xfffbae94  ,0xfffb567a  ,
    0xfffafe60  ,0xfffaa646  ,0xfffa4e2c  ,0xfff9f612  ,0xfff99df8  ,0xfff945de  ,0xfff8edc4  ,0xfff895aa  ,
    0xfff83d90  ,0xfff7e576  ,0xfff78d5c  ,0xfff73542  ,0xfff6dd28  ,0xfff6850e  ,0xfff62cf4  ,0xfff5d4da  ,
    0xfff57cc0  ,0xfff524a6  ,0xfff4cc8c  ,0xfff47472  ,0xfff41c58  ,0xfff3c43e  ,0xfff36c24  ,0xfff3140a  ,
    0xfff2bbf0  ,0xfff263d6  ,0xfff20bbc  ,0xfff1b3a2  ,0xfff15b88  ,0xfff1036e  ,0xfff0ab54  ,0xfff0533a  ,
    0xffeffb20  ,0xffefa306  ,0xffef4aec  ,0xffeef2d2  ,0xffee9ab8  ,0xffee429e  ,0xffedea84  ,0xffed926a  ,
    0xffed3a50  ,0xffece236  ,0xffec8a1c  ,0xffec3202  ,0xffebd9e8  ,0xffeb81ce  ,0xffeb29b4  ,0xffead19a  ,
    0xffea7980  ,0xffea2166  ,0xffe9c94c  ,0xffe97132  ,0xffe91918  ,0xffe8c0fe  ,0xffe868e4  ,0xffe810ca  ,
    0xffe7b8b0  ,0xffe76096  ,0xffe7087c  ,0xffe6b062  ,0xffe65848  ,0xffe6002e  ,0xffe5a814  ,0xffe54ffa  ,
    0xffe4f7e0  ,0xffe49fc6  ,0xffe447ac  ,0xffe3ef92  ,0xffe39778  ,0xffe33f5e  ,0xffe2e744  ,0xffe28f2a  ,
    0xffe23710  ,0xffe1def6  ,0xffe186dc  ,0xffe12ec2  ,0xffe0d6a8  ,0xffe07e8e  ,0xffe02674  ,0xffdfce5a  ,
    0xffdf7640  ,0xffdf1e26  ,0xffdec60c  ,0xffde6df2  ,0xffde15d8  ,0xffddbdbe  ,0xffdd65a4  ,0xffdd0d8a  ,
    0xffdcb570  ,0xffdc5d56  ,0xffdc053c  ,0xffdbad22  ,0xffdb5508  ,0xffdafcee  ,0xffdaa4d4  ,0xffda4cba  ,
    0xffd9f4a0  ,0xffd99c86  ,0xffd9446c  ,0xffd8ec52  ,0xffd89438  ,0xffd83c1e  ,0xffd7e404  ,0xffd78bea  ,
    0xffd733d0  ,0xffd6dbb6  ,0xffd6839c  ,0xffd62b82  ,0xffd5d368  ,0xffd57b4e  ,0xffd52334  ,0xffd4cb1a  ,
};

CONST UINT8   g_imgsTable[5 * (MAXJSAMPLE+1) + CENTERJSAMPLE] MICRON_ALIGN(32)= //1408
{
#include "imgs_rangelist_data.tab"
};

CONST INT8* g_imgsSampleRangeLimit MICRON_ALIGN(32)= g_imgsTable+256;

//variables used in jpeg function
UINT8   *g_imgsLp;
UINT8   g_imgsYData[64*4];
UINT8   g_imgsUData[64];
UINT8   g_imgsVData[64];
INT16           g_imgsCompNum;
UINT8           g_imgsYDcIndex,g_imgsYAcIndex,g_imgsUVDcIndex,g_imgsUVAcIndex;
INT16           *g_imgsYQtTable,*g_imgsUQtTable,*g_imgsVQtTable;
//////////////////////////
INT16            (*imgs_QtTable)[64];
INT32              (*g_imgsHufMaxValue)[17],(*g_imgsValOffset)[17];
UINT32     (*g_imgsHuffCode)[256];
INT16            (*g_imgsLookNBits)[256],(*g_imgsLookSym)[256],(*g_imgsCodeLenTable)[17],(*g_imgsCodeValueTable)[256];
/////////////////////////
INT32               g_imgsBitPos,g_imgsCurByte;
UINT16  g_imgsRrun,g_imgsVvalue;
//INT16         MCU_lastcoef;
INT16           g_imgsYCoef,g_imgsUCoef,g_imgsVCoef;
BOOL            g_imgsIntervalFlag;
INT32             g_imgsInterval=0;
INT16           g_imgsRestart;
INT16           g_imgsImgWidth, g_imgsImgHeight;
INT16           g_imgsDownWidth,g_imgsDownHeight;
INT16           g_imgsSampRateYH,g_imgsSampRateYV;
INT32             g_imgsSizeI,g_imgsSizeJ;
INT16           g_imgsYLen,g_imgsVLen;
INT16 g_imgsYDctSize,g_imgsUVDctSize;
INT32 g_imgsVYuTemp, g_imgsHYuTemp, g_imgsVYVTemp, g_imgsHYVTemp;
VOID (*g_imgsPIDCT[2])(INT8* ,INT16* ,INT16*,INT16);
PROTECTED INT32         g_imgsFileHandle=-1;
INT8* g_imgsTempBuf;
INT32 (*g_imgsReadByte)();

//////////////////////////////////////////////function api
PRIVATE INT32  InitTag(UINT8* lpJpegBuf);
PRIVATE IMGS_ERR_T InitTable();
PRIVATE INT32  DecodeShort(INT8* RGB_buf);
PRIVATE INT32 DecodeMCUBlock();
PRIVATE INT32 HufBlock(INT16* coef,UINT8 dchufindex,UINT8 achufindex,INT16* pMCU);
PRIVATE INT32  DecodeElement(INT32 index);
//VOID ShrinkWorkingBitmap (UINT8 *a,UINT16 width,UINT16 height,UINT8 *b,UINT16 bx,UINT16 by);
PRIVATE VOID StoreBufferShort(INT8* rgb_buf);
PRIVATE VOID  make_huffutable(INT16 index);
//INT32  ReadByte();
PRIVATE INT32  ReadByteFile();
PRIVATE INT32  ReadByteBuf();

PRIVATE VOID  FreeMemory(INT16 kind);
PRIVATE VOID jpeg_idct_ifast (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos);
PRIVATE VOID jpeg_idct_1x1 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos);
PRIVATE VOID  jpeg_idct_2x2 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos);
PRIVATE VOID jpeg_idct_4x4 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos);

PRIVATE VOID  decide_qt_table(INT16 dct_size,INT16* qt,INT32 index);



// =============================================================================
// imgs_JpegDecode
// -----------------------------------------------------------------------------
/// Decode an encoded image, previously dumped into a buffer by #imgs_OpenImage,
/// into a frame buffer window passed as a parameter. The frame buffer window
/// data buffer is allocated by the #imgs_JpegDecode function, and contains
/// the full resolution image. Its region of interest is the full frame buffer
/// window.
/// @param fileName Encoded image. Can be either a file (kind=1) or a buffer
/// (kind = 0);
/// @param fbw Output frame buffer window.
/// @param callback User handler called at the end of the conversion.
/// @param kind of image: If 0, fileName is a pointer to a buffer holding the
/// image data. If kind = 1,  fileName is the name of a file to decode.
/// @return #IMGS_ERR_WRONG_HEAD, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_WRONG_CASE
/// or #IMGS_ERR_NO.
// =============================================================================
PROTECTED IMGS_ERR_T imgs_JpegDecode(PCSTR fileName, IMGS_FBW_T* fbw,
                                     IMGS_DEC_CALLBACK_T callback,UINT32 kind)
{
    INT32 funcret = 0;
    INT16 index;
    INT32  filelen;
    UINT32 headerLenght;
    //kind=0 jpeg in buf
    //kind=1 jpeg in file


    if(kind==1)
    {
        g_imgsFileHandle = FS_Open((UINT8*)fileName, FS_O_RDONLY, 0);
        if(g_imgsFileHandle<0)
        {
            return IMGS_ERR_FILE_OPEN;
        }

        g_imgsTempBuf =(INT8*)sxr_Malloc(TEMPBUFSIZE);
        if(g_imgsTempBuf==NULL)
        {
            FreeMemory(kind);
            FS_Close(g_imgsFileHandle);
            return  IMGS_ERR_OUT_OF_MEMORY;
        }

        filelen = FS_Read(g_imgsFileHandle, g_imgsTempBuf, TEMPBUFSIZE);

        g_imgsReadByte = ReadByteFile;
    }
    else
    {
        g_imgsTempBuf = (INT8*)fileName;
        g_imgsReadByte = ReadByteBuf;
        g_imgsFileHandle=0;
    }

    InitTable();

    headerLenght=InitTag(g_imgsTempBuf);//flag = 1 avi    flag=0 picture
    // This InitTag things set a lot of global variables for the
    // decoder.


    if(headerLenght<=100)
    {
        FreeMemory(kind);
        if(kind==1)
        {
            FS_Close(g_imgsFileHandle);
        }
        return IMGS_ERR_WRONG_HEAD;
    }


    for (index=0; index<=g_imgsCompNum; index++)
    {
        make_huffutable(index);
    }

    g_imgsVYuTemp = g_imgsSampRateYV>>1;
    g_imgsVYVTemp = g_imgsSampRateYV>>1;
    g_imgsHYuTemp = g_imgsSampRateYH>>1;
    g_imgsHYVTemp = g_imgsSampRateYH>>1;

// Those global variables need to be set to the output
// image size ...
    g_imgsDownWidth = g_imgsImgWidth;
    g_imgsDownHeight = g_imgsImgHeight;

    g_imgsYDctSize=8;
    g_imgsUVDctSize=8;

    // Allocate and configure the Frame Buffer Window fated to
    // hold the decoded image
    fbw->fb.buffer = sxr_Malloc(g_imgsImgWidth*g_imgsImgHeight*2); // Bytes, 16-bits/pixel
    IMGS_TRACE(TSTDOUT, 0, "Decode FB Buffer :%08x", (UINT32)fbw->fb.buffer);
    if (fbw->fb.buffer == NULL)
    {
        // Malloc failed
        IMGS_TRACE(TSTDOUT, 0, "Malloc for decoded FBW buffer failed !");
        return IMGS_ERR_OUT_OF_MEMORY;
    }
    fbw->fb.width = g_imgsImgWidth;
    fbw->fb.height = g_imgsImgHeight;
    fbw->fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
    fbw->roi.x = 0;
    fbw->roi.y = 0;
    fbw->roi.width = g_imgsImgWidth;
    fbw->roi.height = g_imgsImgHeight;

    ///decide qt table and IDCT function
    decide_qt_table(g_imgsYDctSize,g_imgsYQtTable,0);
    decide_qt_table(g_imgsUVDctSize,g_imgsUQtTable,1);

    if (g_imgsUQtTable!=g_imgsVQtTable)
    {
        decide_qt_table(g_imgsUVDctSize,g_imgsVQtTable,1);
    }

    g_imgsVLen=g_imgsVLen*g_imgsYDctSize>>3;
    g_imgsYLen=g_imgsYLen*g_imgsYDctSize>>3;
    //init
    g_imgsLp = (g_imgsTempBuf + headerLenght);

    //decode
    for (g_imgsSizeI=0; g_imgsSizeI<g_imgsImgHeight; g_imgsSizeI+=g_imgsVLen)
    {
        funcret = DecodeShort(((UINT8*)fbw->fb.buffer)+2*g_imgsSizeI*g_imgsImgWidth);//decode  g_imgsVLen*g_imgsImgWidth*3  [byte]

        if (funcret!=IMGS_ERR_NO)
        {
            break;
        }
    }

    //close file
    FreeMemory(kind);
    if(kind==1)
    {
        FS_Close(g_imgsFileHandle);
    }

    if (funcret!=IMGS_ERR_NO)
    {
        return IMGS_ERR_WRONG_CASE;
    }
    else
    {
        if (callback)
        {
            callback(fbw);
        }
        return IMGS_ERR_NO;
    }
}


PRIVATE INT32 InitTag(UINT8* lpJpegBuf)
{
    BOOL finish=FALSE;
    UINT8 id;
    UINT16  llength;
    INT16  i;
    UINT8 qt_table_index;
    UINT8 comnum;
    UINT8  *lptemp;
    //huffuman
    INT16  huftab1,huftab2;
    INT16  huftabindex;
    INT16  ccount;
    UINT8   comp_index[3];
    //INT16  SampRate_U_H,SampRate_U_V;
    //INT16  SampRate_V_H,SampRate_V_V;

    g_imgsLp=lpJpegBuf;
    if (*g_imgsLp!=0xff || *(g_imgsLp+1)!=0xd8)
    {
        return -1;
    }
    g_imgsLp=lpJpegBuf+2;
    while (!finish)
    {
        id=*(g_imgsLp+1);
        g_imgsLp+=2;
        switch (id)
        {
            case M_APP0:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                g_imgsLp+=llength;
                break;
            case M_DQT:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                qt_table_index=(*(g_imgsLp+2))&0x0f;
                lptemp=g_imgsLp+3;
                if(llength<80)
                {
                    for(i=0; i<64; i++)
                        imgs_QtTable[qt_table_index][g_imgsInverseZigZag[i]]=(INT16)*(lptemp++);
                }
                else
                {
                    for(i=0; i<64; i++)
                    {
                        imgs_QtTable[qt_table_index][g_imgsInverseZigZag[i]]=(INT16)*(lptemp++);
                    }
                    qt_table_index=(*(lptemp++))&0x0f;
                    for(i=0; i<64; i++)
                    {
                        imgs_QtTable[qt_table_index][g_imgsInverseZigZag[i]]=(INT16)*(lptemp++);
                    }

                }
                g_imgsLp+=llength;

                break;
            case M_SOF0:
            case M_SOF1:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                g_imgsImgHeight=MAKEAUINT16(*(g_imgsLp+4),*(g_imgsLp+3));
                g_imgsImgWidth=MAKEAUINT16(*(g_imgsLp+6),*(g_imgsLp+5));
                g_imgsCompNum=*(g_imgsLp+7);
                if((g_imgsCompNum!=1)&&(g_imgsCompNum!=3))
                    return IMGS_ERR_FUNC_FORMAT;
                if(g_imgsCompNum==3)
                {
                    comp_index[0]=*(g_imgsLp+8);
                    g_imgsSampRateYH=(*(g_imgsLp+9))>>4;
                    g_imgsSampRateYV=(*(g_imgsLp+9))&0x0f;
                    g_imgsYQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+10)];

                    comp_index[1]=*(g_imgsLp+11);
                    //SampRate_U_H=(*(g_imgsLp+12))>>4;
                    //SampRate_U_V=(*(g_imgsLp+12))&0x0f;
                    g_imgsUQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+13)];

                    comp_index[2]=*(g_imgsLp+14);
                    //SampRate_V_H=(*(g_imgsLp+15))>>4;
                    //SampRate_V_V=(*(g_imgsLp+15))&0x0f;
                    g_imgsVQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+16)];
                }
                else
                {
                    comp_index[0]=*(g_imgsLp+8);
                    g_imgsSampRateYH=(*(g_imgsLp+9))>>4;
                    g_imgsSampRateYV=(*(g_imgsLp+9))&0x0f;
                    g_imgsYQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+10)];

                    comp_index[1]=*(g_imgsLp+8);
                    //SampRate_U_H=1;
                    //SampRate_U_V=1;
                    g_imgsUQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+10)];

                    comp_index[2]=*(g_imgsLp+8);
                    //SampRate_V_H=1;
                    //SampRate_V_V=1;
                    g_imgsVQtTable=(INT16 *)imgs_QtTable[*(g_imgsLp+10)];
                }


                g_imgsYLen = g_imgsSampRateYH<<3;
                g_imgsVLen = g_imgsSampRateYV<<3;
                //Initialize_Fast_IDCT();
                g_imgsLp+=llength;
                break;
            case M_DHT:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                llength-=2;
                g_imgsLp+=2;

                while (llength>16)
                {
                    huftab1=(INT16)(*g_imgsLp)>>4;     //huftab1=0,1
                    huftab2=(INT16)(*g_imgsLp)&0x0f;   //huftab2=0,1
                    huftabindex=huftab1*2+huftab2;
                    if (huftabindex>3)
                    {
                        return IMGS_ERR_FUNC_FORMAT;
                        break;
                    }
                    g_imgsLp+=1;
                    ccount=0;

                    g_imgsCodeLenTable[huftabindex][0]=0;
                    for (i = 1; i <= 16; i++)
                    {
                        g_imgsCodeLenTable[huftabindex][i]=*g_imgsLp++;
                        ccount+=g_imgsCodeLenTable[huftabindex][i];
                    }
                    llength-=17;//1+16

                    for (i = 0; i < ccount; i++)
                        g_imgsCodeValueTable[huftabindex][i]=*g_imgsLp++;
                    llength-=ccount;

                }
                IMGS_ASSERT(llength==0, "InitTag");
                break;
            case M_DRI:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                g_imgsRestart=MAKEAUINT16(*(g_imgsLp+3),*(g_imgsLp+2));
                g_imgsLp+=llength;
                break;
            case M_SOS:
                llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                comnum=*(g_imgsLp+2);
                if(comnum!=g_imgsCompNum)
                    return IMGS_ERR_FUNC_FORMAT;
                lptemp=g_imgsLp+3;
                for (i=0; i<g_imgsCompNum; i++)
                {
                    if(*lptemp==comp_index[0])
                    {
                        g_imgsYDcIndex=(*(lptemp+1))>>4;    //Y
                        g_imgsYAcIndex=((*(lptemp+1))&0x0f)+2;
                    }
                    else
                    {
                        g_imgsUVDcIndex=(*(lptemp+1))>>4;   //U,V
                        g_imgsUVAcIndex=((*(lptemp+1))&0x0f)+2;
                    }
                    lptemp+=2;
                }
                g_imgsLp+=llength;
                finish=TRUE;
                break;
            case M_EOI:
                return IMGS_ERR_FUNC_FORMAT;
                break;
            default:
                if ((id&0xf0)!=0xd0)
                {
                    llength=MAKEAUINT16(*(g_imgsLp+1),*g_imgsLp);
                    if (llength==0)
                    {
                        return -1;
                    }
                    else if (llength>TEMPBUFSIZE && g_imgsFileHandle)
                    {
                        UINT32 templen=(UINT32)llength-TEMPBUFSIZE+((UINT32)g_imgsLp-(UINT32)lpJpegBuf);
                        INT32 filelen;
#ifndef  TESTONPC
                        FS_Seek(g_imgsFileHandle, (INT64)templen, FS_SEEK_CUR);
                        filelen = FS_Read(g_imgsFileHandle, g_imgsTempBuf, TEMPBUFSIZE);
#else
                        fseek(fileHandle,templen,SEEK_CUR);
                        fread(g_imgsTempBuf,1, TEMPBUFSIZE,fileHandle);
#endif
                        //usedlen+=(UINT32)g_imgsLp-(UINT32)lpJpegBuf+llength;
                        g_imgsLp=g_imgsTempBuf;
                    }
                    else
                        g_imgsLp+=llength;
                }
                else
                    g_imgsLp+=2;
                break;
        }  //switch
    } //while
    return (UINT32)g_imgsLp-(UINT32)lpJpegBuf;
}
/////////////////////////////////////////////////////////////////
PRIVATE IMGS_ERR_T InitTable()
{

    g_imgsImgWidth=g_imgsImgHeight=0;
    g_imgsSizeI = g_imgsSizeJ = 0;
    g_imgsInterval=0;
    g_imgsRrun=g_imgsVvalue=0;
    g_imgsBitPos=0;
    g_imgsCurByte=0;
    g_imgsIntervalFlag=FALSE;
    g_imgsRestart=0;
    g_imgsCompNum=0;
    g_imgsYCoef=g_imgsUCoef=g_imgsVCoef=0;

    g_imgsHuffCode = (UINT32(*)[256])sxr_Malloc(256<<4);
    if(g_imgsHuffCode==NULL)
        return IMGS_ERR_MALLOC;


    g_imgsHufMaxValue =(INT32(*)[17])sxr_Malloc(17<<4);
    if(g_imgsHufMaxValue==NULL)
        return IMGS_ERR_MALLOC;

    g_imgsValOffset=(INT32(*)[17])sxr_Malloc(17<<4);
    if(g_imgsValOffset==NULL)
        return IMGS_ERR_MALLOC;


    g_imgsLookNBits = (INT16(*)[256])sxr_Malloc(256<<3);
    if(g_imgsLookNBits==NULL)
        return IMGS_ERR_MALLOC;

    g_imgsLookSym = (INT16(*)[256])sxr_Malloc(256<<3);
    if(g_imgsLookSym==NULL)
        return IMGS_ERR_MALLOC;

    g_imgsCodeValueTable=(INT16(*)[256])sxr_Malloc(256<<3);
    if(g_imgsCodeValueTable==NULL)
        return IMGS_ERR_MALLOC;

    imgs_QtTable=(INT16(*)[64])sxr_Malloc(128*3);
    if(imgs_QtTable==NULL)
        return IMGS_ERR_MALLOC;

    g_imgsCodeLenTable=(INT16(*)[17])sxr_Malloc(17<<3);
    if(g_imgsCodeLenTable==NULL)
        return IMGS_ERR_MALLOC;

    memset(g_imgsLookNBits,0,256<<3);
    memset(g_imgsHuffCode,0,256<<4);
    return IMGS_ERR_NO;
}
PRIVATE VOID  FreeMemory(INT16 kind)
{
    if(g_imgsHuffCode)
        sxr_Free(g_imgsHuffCode);

    if(g_imgsHufMaxValue)
        sxr_Free(g_imgsHufMaxValue);

    if(g_imgsValOffset)
        sxr_Free(g_imgsValOffset);

    if(g_imgsLookNBits)
        sxr_Free(g_imgsLookNBits);

    if(g_imgsLookSym)
        sxr_Free(g_imgsLookSym);

    if(g_imgsCodeValueTable)
        sxr_Free(g_imgsCodeValueTable);

    if(imgs_QtTable)
        sxr_Free(imgs_QtTable);

    if(g_imgsCodeLenTable)
        sxr_Free(g_imgsCodeLenTable);

    if(g_imgsTempBuf && kind)
        sxr_Free(g_imgsTempBuf);

}

PRIVATE VOID  make_huffutable(INT16 index)
{
    INT32 p, i, l, numsymbols;
    INT32 code;
    INT32 ctr,lookbits;
    UINT32 si;

    //size
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        i = g_imgsCodeLenTable[index][l];
        while (i--)
            g_imgsHuffCode[index][p++] = l;
    }
    g_imgsHuffCode[index][p] = 0;
    numsymbols = p;
    //code
    code = 0;
    p = 0;
    si = g_imgsHuffCode[index][0];
    while (g_imgsHuffCode[index][p])
    {
        while (( g_imgsHuffCode[index][p]) == si)
        {
            g_imgsHuffCode[index][p++] = code;
            code++;
        }
        code <<= 1;
        si++;
    }


    //max  g_imgsValOffset
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        if (g_imgsCodeLenTable[index][l])
        {
            g_imgsValOffset[index][l] = p - g_imgsHuffCode[index][p];
            p += g_imgsCodeLenTable[index][l];
            g_imgsHufMaxValue[index][l] = g_imgsHuffCode[index][p-1]; // maximum code of length l
        }
        else
        {
            g_imgsHufMaxValue[index][l] = -1;   // -1 if no codes of this length
        }
    }

    //lookbits
    p = 0;
    for (l = 1; l <= HUFF_LOOKAHEAD; l++)
    {
        for (i = 1; i <= g_imgsCodeLenTable[index][l]; i++, p++)
        {
            // l = current code's length, p = its index in huffcode[] & huffval[].
            // Generate left-justified code followed by all possible bit sequences
            lookbits = g_imgsHuffCode[index][p] << (HUFF_LOOKAHEAD-l);
            for ( ctr= 1 << (HUFF_LOOKAHEAD-l); ctr > 0; ctr--)
            {
                g_imgsLookNBits[index][lookbits] = l;
                g_imgsLookSym[index][lookbits] = g_imgsCodeValueTable[index][p];
                lookbits++;
            }
        }
    }

}
PRIVATE VOID  decide_qt_table(INT16 dct_size,INT16* qt,INT32 index)
{
    INT32 i;
    switch(dct_size)
    {
        case 1:
            g_imgsPIDCT[index] = jpeg_idct_1x1;
            break;
        case 2:
            g_imgsPIDCT[index] = jpeg_idct_2x2;
            break;
        case 4:
            g_imgsPIDCT[index] = jpeg_idct_4x4;
            break;
        case 8:
            for (i=0; i<DCTSIZE2; i++)
            {
                qt[i]=(INT16)DESCALE(qt[i]*g_imgsAanScales[i],12);
            }
            g_imgsPIDCT[index] = jpeg_idct_ifast;
            break;
        default:
            i=0;//mmi_trace(TRUE, "MMC_JPEG_DEC::wrong dct size");//printf("wrong dct size\n");

    }
}




PRIVATE INT32 DecodeShort(INT8* RGB_buf)
{
    INT32 funcret = 0;

    for(g_imgsSizeJ=0; g_imgsSizeJ<g_imgsDownWidth; g_imgsSizeJ+=g_imgsYLen) //while((funcret=DecodeMCUBlock())==IMGS_ERR_NO)
    {

        if ((funcret=DecodeMCUBlock())!=IMGS_ERR_NO)
        {
            return funcret;
        }
        g_imgsInterval++;
        if((g_imgsRestart)&&(g_imgsInterval % g_imgsRestart==0))
            g_imgsIntervalFlag=TRUE;
        else
            g_imgsIntervalFlag=FALSE;

        StoreBufferShort(RGB_buf+2*g_imgsSizeJ);

    }

    return funcret;
}
PRIVATE VOID StoreBufferShort(INT8* rgb_buf)
{
    INT32 i,j;
    INT32 y,u,v;
    INT32 rr,gg,bb;
    INT32 y_row_start,uv_row_start;
    INT16 *lpbmp;
    INT32  temp_len=g_imgsYLen;
    CONST INT8* g_imgsRangeLimit=g_imgsSampleRangeLimit;

    INT32  tmp_vlen;

    if (g_imgsSizeI+g_imgsVLen<=g_imgsDownHeight)
    {
        tmp_vlen = g_imgsVLen;
    }
    else
    {
        tmp_vlen = g_imgsDownHeight-g_imgsSizeI;
    }

    lpbmp=(INT16*)rgb_buf;

    if (g_imgsSizeJ+g_imgsYLen<=g_imgsDownWidth)
    {
        for(i=0; i<tmp_vlen; i++, lpbmp = lpbmp+(g_imgsDownWidth-temp_len)) //换行
        {
            y_row_start = i*g_imgsYLen;
            uv_row_start = (i>>g_imgsVYuTemp)*g_imgsYDctSize;

            for(j=0; j<g_imgsYLen; j++) //列
            {
                y=g_imgsYData[y_row_start+j];
                u=g_imgsUData[uv_row_start+(j>>g_imgsHYuTemp)];
                v=g_imgsVData[uv_row_start+(j>>g_imgsHYVTemp)];
                {
                    rr =   g_imgsRangeLimit[y + g_imgsCrRTab[v]];
                    gg = g_imgsRangeLimit[y + ((INT32) RIGHT_SHIFT(g_imgsCbGTab[u] + g_imgsCrGTab[v], 16))];
                    bb =  g_imgsRangeLimit[y + g_imgsCbBTab[u]];

                    *lpbmp++=DRV_RGB2PIXEL565(rr,gg,bb);
                }
            }

        }
    }
    else
    {
        for(i=0; i<tmp_vlen; i++, lpbmp = lpbmp+(g_imgsDownWidth-temp_len)) //换行
        {
            y_row_start = i*g_imgsYLen;
            uv_row_start = (i>>g_imgsVYuTemp)*g_imgsYDctSize;

            for(j=0; j<g_imgsYLen; j++) //列
            {
                if (g_imgsSizeJ+j<g_imgsDownWidth)
                {
                    y=g_imgsYData[y_row_start+j];
                    u=g_imgsUData[uv_row_start+(j>>g_imgsHYuTemp)];
                    v=g_imgsVData[uv_row_start+(j>>g_imgsHYVTemp)];
                    {

                        rr =   g_imgsRangeLimit[y + g_imgsCrRTab[v]];
                        gg = g_imgsRangeLimit[y + ((INT32) RIGHT_SHIFT(g_imgsCbGTab[u] + g_imgsCrGTab[v], 16))];
                        bb =  g_imgsRangeLimit[y + g_imgsCbBTab[u]];

                        *lpbmp++=DRV_RGB2PIXEL565(rr,gg,bb);
                    }
                }
                else
                {
                    temp_len = g_imgsDownWidth-g_imgsSizeJ;
                    break;
                }
            }

        }
    }

}

///////////////////////////////////////////////////////////////////////////////
PRIVATE INT32 DecodeMCUBlock()
{
    INT16  i,j;
    INT32    funcret;
    INT16  MCUBuffer[64];

    if (g_imgsIntervalFlag)
    {
        IMGS_ASSERT(g_imgsBitPos<16, "DecodeMCUBlock");

        if(g_imgsBitPos<8)
        {
            g_imgsLp+=2;
        }
        g_imgsYCoef=g_imgsUCoef=g_imgsVCoef=0;
        g_imgsBitPos=0;
        g_imgsCurByte=0;
    }

    switch(g_imgsCompNum)
    {
        case 3:
            //Y DATA
            memset(MCUBuffer,0,128);
            if (g_imgsSampRateYV==1 && g_imgsSampRateYH==1)//444
            {

                HufBlock(&g_imgsYCoef,g_imgsYDcIndex,g_imgsYAcIndex,MCUBuffer);
                //if (funcret!=IMGS_ERR_NO)
                //  return funcret;
                g_imgsPIDCT[0](g_imgsYData,g_imgsYQtTable,MCUBuffer,g_imgsYDctSize);
            }
            else//422  411
            {
                INT16 tmplen=g_imgsYDctSize*g_imgsSampRateYH;
                for (i=0; i<g_imgsSampRateYV; i++)
                {
                    for(j=0; j<g_imgsSampRateYH; j++)
                    {
                        INT8* y_tmp=g_imgsYData+i*g_imgsYDctSize*g_imgsVLen+j*g_imgsYDctSize;
                        memset(MCUBuffer,0,128);
                        HufBlock(&g_imgsYCoef,g_imgsYDcIndex,g_imgsYAcIndex,MCUBuffer);
                        //if (funcret!=IMGS_ERR_NO)
                        //  return funcret;
                        g_imgsPIDCT[0](y_tmp,g_imgsYQtTable,MCUBuffer,tmplen);


                    }
                }
            }


            //U DATA
            memset(MCUBuffer,0,128);
            HufBlock(&g_imgsUCoef,g_imgsUVDcIndex,g_imgsUVAcIndex,MCUBuffer);
            //if (funcret!=IMGS_ERR_NO)
            //  return funcret;
            g_imgsPIDCT[1](g_imgsUData,g_imgsUQtTable,MCUBuffer,g_imgsUVDctSize);

            //V DATA
            memset(MCUBuffer,0,128);
            HufBlock(&g_imgsVCoef,g_imgsUVDcIndex,g_imgsUVAcIndex,MCUBuffer);
            //if (funcret!=IMGS_ERR_NO)
            //  return funcret;
            g_imgsPIDCT[1](g_imgsVData,g_imgsVQtTable,MCUBuffer,g_imgsUVDctSize);

            break;
        case 1:
            memset(MCUBuffer,0,128);
            funcret=HufBlock(&g_imgsYCoef,g_imgsYDcIndex,g_imgsYAcIndex,MCUBuffer);
            if (funcret!=IMGS_ERR_NO)
                return funcret;
            //(*idct_array[(MCU_lastcoef)&0x3f])(g_imgsYData);
            g_imgsPIDCT[0](g_imgsYData,g_imgsYQtTable,MCUBuffer,g_imgsYDctSize);
            break;
        default:
            return IMGS_ERR_FUNC_FORMAT;
    }
    return IMGS_ERR_NO;
}
//////////////////////////////////////////////////////////////////
PRIVATE INT32 HufBlock(INT16* coef,UINT8 dchufindex,UINT8 achufindex,INT16* pMCU)
{
    INT16 count=0;
    //INT32 funcret;

    //dc
    //HufTabIndex=dchufindex;
    DecodeElement(dchufindex);
    //if(funcret!=IMGS_ERR_NO)
    //  return funcret;
    g_imgsVvalue += *coef;
    *coef = g_imgsVvalue;
    *pMCU = g_imgsVvalue;
    //ac
    //HufTabIndex=achufindex;
    for(count=1;; count++)
    {

        DecodeElement(achufindex);

        //if(funcret!=IMGS_ERR_NO)
        //  return funcret;
        if ((g_imgsRrun==0)&&(g_imgsVvalue==0)) //EOB
        {
            //MCU_lastcoef = count;
            break;
        }
        else
        {
            count += g_imgsRrun;
            pMCU[g_imgsInverseZigZag[count]] = g_imgsVvalue;
        }
        if(count >= 63)
        {
            //MCU_lastcoef = count;
            break;
        }
    }

    return IMGS_ERR_NO;
}
/////////////////////////////////////////////////
PRIVATE INT32 DecodeElement(INT32 index)
{
    INT32 nb, look;
    INT32 code,sign,runsize;
    UINT16 valux;

    if (g_imgsBitPos < HUFF_LOOKAHEAD)
    {
        g_imgsCurByte = g_imgsCurByte<<8 | g_imgsReadByte();
        g_imgsCurByte=g_imgsCurByte&g_imgsAnd[g_imgsBitPos];
    }

    look = (g_imgsCurByte >> (g_imgsBitPos -  (HUFF_LOOKAHEAD)));
    if ((nb = g_imgsLookNBits[index][look]) != 0)
    {
        g_imgsBitPos -= nb;
        g_imgsCurByte=g_imgsCurByte & g_imgsAnd[g_imgsBitPos];
        valux = g_imgsLookSym[index][look];
    }
    else
    {
        while(g_imgsBitPos < 16)
        {
            g_imgsCurByte = g_imgsCurByte<<8 | g_imgsReadByte();
            g_imgsCurByte=g_imgsCurByte&g_imgsAnd[g_imgsBitPos];
        }
        nb = 9;//HUFF_LOOKAHEAD+1;
        g_imgsBitPos-=nb;
        code=(g_imgsCurByte >> g_imgsBitPos);
        g_imgsCurByte=g_imgsCurByte & g_imgsAnd[g_imgsBitPos];
        while (code > g_imgsHufMaxValue[index][nb])
        {
            code <<= 1;
            g_imgsBitPos--;
            code |= g_imgsCurByte >>g_imgsBitPos;
            g_imgsCurByte=g_imgsCurByte & g_imgsAnd[g_imgsBitPos];
            nb++;

            if (nb>16)
            {
                return IMGS_ERR_WRONG_DECOMPOSE;
            }
        }

        valux = g_imgsCodeValueTable[index][code+g_imgsValOffset[index][nb]];
    }

    g_imgsRrun=(INT16)(valux>>4);
    runsize=valux&0x0f;//bits of code

    if(runsize==0)//the num of continuous zeros
    {
        g_imgsVvalue=0;
        return IMGS_ERR_NO;
    }

    while(g_imgsBitPos < runsize)
    {
        g_imgsCurByte = g_imgsCurByte<<8 | g_imgsReadByte();
        g_imgsCurByte=g_imgsCurByte&g_imgsAnd[g_imgsBitPos];
    }

    g_imgsBitPos-=runsize;
    valux=g_imgsCurByte>>g_imgsBitPos;
    g_imgsCurByte=g_imgsCurByte&g_imgsAnd[g_imgsBitPos];

    sign=valux>>(runsize-1);
    if(!sign)
    {
        g_imgsVvalue=-((valux^0xffff)^(0xffff<<runsize));
    }
    else
        g_imgsVvalue=valux;
    return IMGS_ERR_NO;
}
///////////////////////////////////////////////////////////////////////////////
PRIVATE INT32  ReadByteBuf()
{
    INT32 i;
    i=*(g_imgsLp++);
    if(i==0xff)
    {
        g_imgsLp++;
    }
    g_imgsBitPos+=8;

    return i;
}
PRIVATE INT32  ReadByteFile()
{
    INT32 i;
    INT32 filelen;

    if ((UINT32)g_imgsLp-(UINT32)g_imgsTempBuf == TEMPBUFSIZE)
    {
        filelen = FS_Read(g_imgsFileHandle, g_imgsTempBuf, TEMPBUFSIZE);
        g_imgsLp = g_imgsTempBuf;
    }
    i=*(g_imgsLp++);
    if(i==0xff)
    {
        if ((UINT32)g_imgsLp-(UINT32)g_imgsTempBuf == TEMPBUFSIZE)
        {
            filelen = FS_Read(g_imgsFileHandle, g_imgsTempBuf, TEMPBUFSIZE);
            g_imgsLp = g_imgsTempBuf;
        }
        g_imgsLp++;
    }
    g_imgsBitPos+=8;

    return i;
}
///////////////////////////////////////////////////////////////////////
/////////////////////IDCT functions////////////////////////////////////////////
PRIVATE VOID jpeg_idct_ifast (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos)
{
    INT32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    INT32 tmp10, tmp11, tmp12, tmp13;
    INT32 z5, z10, z11, z12, z13;
    INT16* inptr=buf;
    INT16* quantptr=qt;
    CONST INT8* g_imgsRangeLimit=g_imgsSampleRangeLimit + CENTERJSAMPLE;

    INT32 * wsptr;
    INT8* outptr;
    INT32 ctr;
    INT32 workspace[DCTSIZE2];  // buffers data between passes



    // Pass 1: process columns from input, store INT32o work array.
    //inptr = buf;
    wsptr = workspace;
    for (ctr = DCTSIZE; ctr > 0; ctr--)
    {


        if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
                inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*4] == 0 &&
                inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*6] == 0 &&
                inptr[DCTSIZE*7] == 0)
        {
            // AC terms all zero
            tmp0 = (INT32) DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);

            wsptr[DCTSIZE*0] = tmp0;
            wsptr[DCTSIZE*1] = tmp0;
            wsptr[DCTSIZE*2] = tmp0;
            wsptr[DCTSIZE*3] = tmp0;
            wsptr[DCTSIZE*4] = tmp0;
            wsptr[DCTSIZE*5] = tmp0;
            wsptr[DCTSIZE*6] = tmp0;
            wsptr[DCTSIZE*7] = tmp0;

            inptr++;          // advance poINT32ers to next column
            wsptr++;
            quantptr++;
            continue;
        }

        tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
        tmp1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
        tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
        tmp3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
        // Even part
        tmp10 = tmp0 + tmp2;    // phase 3
        tmp11 = tmp0 - tmp2;

        tmp13 = tmp1 + tmp3;    // phases 5-3
        tmp12 = MULTIPLY8(tmp1 - tmp3, FIX_1_414213562) - tmp13; // 2*c4

        tmp0 = tmp10 + tmp13;   // phase 2
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        tmp4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
        tmp5 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
        tmp6 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
        tmp7 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

        z13 = tmp6 + tmp5;      // phase 6
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7 = z11 + z13;       // phase 5
        tmp11 = MULTIPLY8(z11 - z13, FIX_1_414213562); // 2*c4

        z5 = MULTIPLY8(z10 + z12, FIX_1_84775906555); // 2*c2
        tmp10 = MULTIPLY8(z12, FIX_1_082392200) - z5; // 2*(c2-c6)
        tmp12 = MULTIPLY8(z10, - FIX_2_613125930) + z5; // -2*(c2+c6)

        tmp6 = tmp12 - tmp7;    // phase 2
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        wsptr[DCTSIZE*0] = (INT32) (tmp0 + tmp7);
        wsptr[DCTSIZE*7] = (INT32) (tmp0 - tmp7);
        wsptr[DCTSIZE*1] = (INT32) (tmp1 + tmp6);
        wsptr[DCTSIZE*6] = (INT32) (tmp1 - tmp6);
        wsptr[DCTSIZE*2] = (INT32) (tmp2 + tmp5);
        wsptr[DCTSIZE*5] = (INT32) (tmp2 - tmp5);
        wsptr[DCTSIZE*4] = (INT32) (tmp3 + tmp4);
        wsptr[DCTSIZE*3] = (INT32) (tmp3 - tmp4);

        inptr++;            // advance poINT32ers to next column
        wsptr++;
        quantptr++;
    }

    // Pass 2: process rows from work array, store INT32o output array.
    // Note that we must descale the results by a factor of 8 == 23, */
    // and also undo the PASS1_BITS scaling.

    wsptr = workspace;
    for (ctr = 0; ctr < DCTSIZE; ctr++)
    {

        outptr = &outbuf[ctr*pos];

        // Even part
        tmp10 = ((DCTELEM) wsptr[0] + (DCTELEM) wsptr[4]);
        tmp11 = ((DCTELEM) wsptr[0] - (DCTELEM) wsptr[4]);

        tmp13 = ((DCTELEM) wsptr[2] + (DCTELEM) wsptr[6]);
        tmp12 = MULTIPLY8((DCTELEM) wsptr[2] - (DCTELEM) wsptr[6], FIX_1_414213562)
                - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part

        z13 = (DCTELEM) wsptr[5] + (DCTELEM) wsptr[3];
        z10 = (DCTELEM) wsptr[5] - (DCTELEM) wsptr[3];
        z11 = (DCTELEM) wsptr[1] + (DCTELEM) wsptr[7];
        z12 = (DCTELEM) wsptr[1] - (DCTELEM) wsptr[7];

        tmp7 = z11 + z13;       // phase 5
        tmp11 = MULTIPLY8(z11 - z13, FIX_1_414213562); // 2*c4

        z5 = MULTIPLY8(z10 + z12, FIX_1_84775906555); // 2*c2
        tmp10 = MULTIPLY8(z12, FIX_1_082392200) - z5; // 2*(c2-c6)
        tmp12 = MULTIPLY8(z10, - FIX_2_613125930) + z5; // -2*(c2+c6)

        tmp6 = tmp12 - tmp7;    // phase 2
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        // Final output stage: scale down by a factor of 8 and range-limit
        outptr[0] = g_imgsRangeLimit[IDESCALE(tmp0 + tmp7, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[7] = g_imgsRangeLimit[IDESCALE(tmp0 - tmp7, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[1] = g_imgsRangeLimit[IDESCALE(tmp1 + tmp6, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[6] = g_imgsRangeLimit[IDESCALE(tmp1 - tmp6, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[2] = g_imgsRangeLimit[IDESCALE(tmp2 + tmp5, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[5] = g_imgsRangeLimit[IDESCALE(tmp2 - tmp5, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[4] = g_imgsRangeLimit[IDESCALE(tmp3 + tmp4, PASS1_BITS+3)
                                     & RANGE_MASK];
        outptr[3] = g_imgsRangeLimit[IDESCALE(tmp3 - tmp4, PASS1_BITS+3)
                                     & RANGE_MASK];
        wsptr += DCTSIZE;       // advance poINT32er to next row
    }
}

PRIVATE VOID jpeg_idct_4x4 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos)
{
    INT32 tmp0, tmp2, tmp10, tmp12;
    INT32 z1, z2, z3, z4;
    INT16* inptr=buf;
    INT16* quantptr=qt;
    CONST INT8* g_imgsRangeLimit=g_imgsSampleRangeLimit + CENTERJSAMPLE;

    INT32 * wsptr;
    INT8* outptr;
    INT32 ctr;
    INT32 workspace[DCTSIZE*4]; // buffers data between passes


    // Pass 1: process columns from input, store INT32o work array.


    wsptr = workspace;
    for (ctr = DCTSIZE; ctr > 0; inptr++, quantptr++, wsptr++, ctr--)
    {
        // Don't bother to process column 4, because second pass won't use it
        if (ctr == DCTSIZE-4)
            continue;
        if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
                inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*5] == 0 &&
                inptr[DCTSIZE*6] == 0 && inptr[DCTSIZE*7] == 0)
        {
            // AC terms all zero; we need not examine term 4 for 4x4 output
            INT32 dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]) << PASS1_BITS;

            wsptr[DCTSIZE*0] = dcval;
            wsptr[DCTSIZE*1] = dcval;
            wsptr[DCTSIZE*2] = dcval;
            wsptr[DCTSIZE*3] = dcval;

            continue;
        }

        // Even part

        tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
        tmp0 <<= (CONST_BITS+1);

        z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
        z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

        tmp2 = MULTIPLY(z2, FIX_1_847759065) + MULTIPLY(z3, - FIX_0_765366865);

        tmp10 = tmp0 + tmp2;
        tmp12 = tmp0 - tmp2;

        // Odd part

        z1 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
        z2 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
        z3 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
        z4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);

        tmp0 = MULTIPLY(z1, - FIX_0_211164243) // sqrt(2) * (c3-c1)
               + MULTIPLY(z2, FIX_1_451774981) // sqrt(2) * (c3+c7)
               + MULTIPLY(z3, - FIX_2_172734803) // sqrt(2) * (-c1-c5)
               + MULTIPLY(z4, FIX_1_061594337); // sqrt(2) * (c5+c7)

        tmp2 = MULTIPLY(z1, - FIX_0_509795579) // sqrt(2) * (c7-c5)
               + MULTIPLY(z2, - FIX_0_601344887) // sqrt(2) * (c5-c1)
               + MULTIPLY(z3, FIX_0_899976223) // sqrt(2) * (c3-c7)
               + MULTIPLY(z4, FIX_2_562915447); // sqrt(2) * (c1+c3)

        // Final output stage

        wsptr[DCTSIZE*0] = (INT32) DESCALE(tmp10 + tmp2, CONST_BITS-PASS1_BITS+1);
        wsptr[DCTSIZE*3] = (INT32) DESCALE(tmp10 - tmp2, CONST_BITS-PASS1_BITS+1);
        wsptr[DCTSIZE*1] = (INT32) DESCALE(tmp12 + tmp0, CONST_BITS-PASS1_BITS+1);
        wsptr[DCTSIZE*2] = (INT32) DESCALE(tmp12 - tmp0, CONST_BITS-PASS1_BITS+1);
    }

    // Pass 2: process 4 rows from work array, store INT32o output array.

    wsptr = workspace;
    for (ctr = 0; ctr < 4; ctr++)
    {
        outptr = &outbuf[ctr*pos];
        // It's not clear whether a zero row test is worthwhile here ...


        // Even part

        tmp0 = ((INT32) wsptr[0]) << (CONST_BITS+1);

        tmp2 = MULTIPLY((INT32) wsptr[2], FIX_1_847759065)
               + MULTIPLY((INT32) wsptr[6], - FIX_0_765366865);

        tmp10 = tmp0 + tmp2;
        tmp12 = tmp0 - tmp2;

        // Odd part

        z1 = (INT32) wsptr[7];
        z2 = (INT32) wsptr[5];
        z3 = (INT32) wsptr[3];
        z4 = (INT32) wsptr[1];

        tmp0 = MULTIPLY(z1, - FIX_0_211164243) // sqrt(2) * (c3-c1)
               + MULTIPLY(z2, FIX_1_451774981) // sqrt(2) * (c3+c7)
               + MULTIPLY(z3, - FIX_2_172734803) // sqrt(2) * (-c1-c5)
               + MULTIPLY(z4, FIX_1_061594337); // sqrt(2) * (c5+c7)

        tmp2 = MULTIPLY(z1, - FIX_0_509795579) // sqrt(2) * (c7-c5)
               + MULTIPLY(z2, - FIX_0_601344887) // sqrt(2) * (c5-c1)
               + MULTIPLY(z3, FIX_0_899976223) // sqrt(2) * (c3-c7)
               + MULTIPLY(z4, FIX_2_562915447); // sqrt(2) * (c1+c3)

        // Final output stage
        outptr[0] = g_imgsRangeLimit[(INT32) DESCALE(tmp10 + tmp2, 19)   & RANGE_MASK];
        outptr[3] = g_imgsRangeLimit[(INT32) DESCALE(tmp10 - tmp2, 19)   & RANGE_MASK];
        outptr[1] = g_imgsRangeLimit[(INT32) DESCALE(tmp12 + tmp0, 19)   & RANGE_MASK];
        outptr[2] = g_imgsRangeLimit[(INT32) DESCALE(tmp12 - tmp0, 19)   & RANGE_MASK];


        wsptr += DCTSIZE;       // advance poINT32er to next row
    }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a reduced-size 2x2 output block.
 */

PRIVATE VOID  jpeg_idct_2x2 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos)
{
    INT32 tmp0, tmp10, z1;
    INT16* inptr=buf;
    INT16* quantptr=qt;
    CONST INT8* g_imgsRangeLimit=g_imgsSampleRangeLimit + CENTERJSAMPLE;
    INT32 * wsptr;
    INT8* outptr;
    INT32 ctr;
    INT32 workspace[DCTSIZE*2]; // buffers data between passes


    // Pass 1: process columns from input, store INT32o work array.
    wsptr = workspace;
    for (ctr = DCTSIZE; ctr > 0; inptr++, quantptr++, wsptr++, ctr--)
    {
        // Don't bother to process columns 2,4,6
        if (ctr == DCTSIZE-2 || ctr == DCTSIZE-4 || ctr == DCTSIZE-6)
            continue;
        if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*3] == 0 &&
                inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*7] == 0)
        {
            // AC terms all zero; we need not examine terms 2,4,6 for 2x2 output
            INT32 dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]) << PASS1_BITS;

            wsptr[DCTSIZE*0] = dcval;
            wsptr[DCTSIZE*1] = dcval;

            continue;
        }

        // Even part

        z1 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
        tmp10 = z1 << (CONST_BITS+2);

        // Odd part

        z1 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
        tmp0 = MULTIPLY(z1, - FIX_0_720959822); // sqrt(2) * (c7-c5+c3-c1)
        z1 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
        tmp0 += MULTIPLY(z1, FIX_0_850430095); // sqrt(2) * (-c1+c3+c5+c7)
        z1 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
        tmp0 += MULTIPLY(z1, - FIX_1_272758580); // sqrt(2) * (-c1+c3-c5-c7)
        z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
        tmp0 += MULTIPLY(z1, FIX_3_624509785); // sqrt(2) * (c1+c3+c5+c7)

        // Final output stage

        wsptr[DCTSIZE*0] = (INT32) DESCALE(tmp10 + tmp0, CONST_BITS-PASS1_BITS+2);
        wsptr[DCTSIZE*1] = (INT32) DESCALE(tmp10 - tmp0, CONST_BITS-PASS1_BITS+2);
    }

    // Pass 2: process 2 rows from work array, store INT32o output array.

    wsptr = workspace;
    for (ctr = 0; ctr < 2; ctr++)
    {
        outptr = &outbuf[ctr*pos];
        // It's not clear whether a zero row test is worthwhile here ...


        // Even part

        tmp10 = ((INT32) wsptr[0]) << (CONST_BITS+2);

        // Odd part

        tmp0 = MULTIPLY((INT32) wsptr[7], - FIX_0_720959822) // sqrt(2) * (c7-c5+c3-c1)
               + MULTIPLY((INT32) wsptr[5], FIX_0_850430095) // sqrt(2) * (-c1+c3+c5+c7)
               + MULTIPLY((INT32) wsptr[3], - FIX_1_272758580) // sqrt(2) * (-c1+c3-c5-c7)
               + MULTIPLY((INT32) wsptr[1], FIX_3_624509785); // sqrt(2) * (c1+c3+c5+c7)

        // Final output stage

        outptr[0] = g_imgsRangeLimit[(INT32) DESCALE(tmp10 + tmp0,
                                     CONST_BITS+PASS1_BITS+3+2)
                                     & RANGE_MASK];
        outptr[1] = g_imgsRangeLimit[(INT32) DESCALE(tmp10 - tmp0,
                                     CONST_BITS+PASS1_BITS+3+2)
                                     & RANGE_MASK];

        wsptr += DCTSIZE;       /* advance poINT32er to next row */
    }
}


//
// Perform dequantization and inverse DCT on one block of coefficients,
// producing a reduced-size 1x1 output block.
//

PRIVATE VOID jpeg_idct_1x1 (INT8* outbuf,INT16* qt,INT16* buf,INT16 pos)
{
    INT32 dcval;
    CONST INT8* g_imgsRangeLimit=g_imgsSampleRangeLimit + CENTERJSAMPLE;

    // We hardly need an inverse DCT routine for this: just take the
    // average pixel value, which is one-eighth of the DC coefficient.


    dcval = DEQUANTIZE(buf[0], qt[0]);
    dcval = (INT32) DESCALE((INT32) dcval, 3);

    outbuf[0] = g_imgsRangeLimit[dcval & RANGE_MASK];
}


