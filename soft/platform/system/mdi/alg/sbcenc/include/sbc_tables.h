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




static const int sbc_offset4[4][4] =
{
    { -1, 0, 0, 0 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 }
};


static const int sbc_offset8[4][8] =
{
    { -2, 0, 0, 0, 0, 0, 0, 1 },
    { -3, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 }
};

#define SP4(val) ASR(val, SCALE_PROTO4_TBL)
#define SA4(val) ASR(val, SCALE_ANA4_TBL)
#define SP8(val) ASR(val, SCALE_PROTO8_TBL)
#define SA8(val) ASR(val, SCALE_ANA8_TBL)
#define SS4(val) ASR(val, SCALE_SPROTO4_TBL)
#define SS8(val) ASR(val, SCALE_SPROTO8_TBL)
#define SN4(val) ASR(val, SCALE_NPROTO4_TBL)
#define SN8(val) ASR(val, SCALE_NPROTO8_TBL)

/*static const int32_t _sbc_proto_4[20] = {
    SP4(0x02cb3e8c), SP4(0x22b63dc0), SP4(0x002329cc), SP4(0x053b7548),
    SP4(0x31eab940), SP4(0xec1f5e60), SP4(0xff3773a8), SP4(0x0061c5a7),
    SP4(0x07646680), SP4(0x3f239480), SP4(0xf89f23a8), SP4(0x007a4737),
    SP4(0x00b32807), SP4(0x083ddc80), SP4(0x4825e480), SP4(0x0191e578),
    SP4(0x00ff11ca), SP4(0x00fb7991), SP4(0x069fdc58), SP4(0x4b584000)
};*/
static const int32_t _sbc_proto_4[20] =
{
    (0x00000596), (0x0000456c), (0x00000046), (0x00000a76),
    (0x000063d5), (0xffffd83e), (0xfffffe6e), (0x000000c3),
    (0x00000ec8), (0x00007e47), (0xfffff13e), (0x000000f4),
    (0x00000166), (0x0000107b), (0x0000904b), (0x00000323),
    (0x000001fe), (0x000001f6), (0x00000d3f), (0x000096b0)
};


/*static const int32_t _anamatrix4[4] = {
    SA4(0x2d413cc0), SA4(0x3b20d780), SA4(0x40000000), SA4(0x187de2a0)
}; right shift 17*/
static const int32_t _anamatrix4[4] =
{
    (0x000016a0), (0x00001d90), (0x00002000), (0x00000c3e)
};


/*static const int32_t _sbc_proto_8[40] = {
    SP8(0x02e5cd20), SP8(0x22d0c200), SP8(0x006bfe27), SP8(0x07808930),
    SP8(0x3f1c8800), SP8(0xf8810d70), SP8(0x002cfdc6), SP8(0x055acf28),
    SP8(0x31f566c0), SP8(0xebfe57e0), SP8(0xff27c437), SP8(0x001485cc),
    SP8(0x041c6e58), SP8(0x2a7cfa80), SP8(0xe4c4a240), SP8(0xfe359e4c),
    SP8(0x0048b1f8), SP8(0x0686ce30), SP8(0x38eec5c0), SP8(0xf2a1b9f0),
    SP8(0xffe8904a), SP8(0x0095698a), SP8(0x0824a480), SP8(0x443b3c00),
    SP8(0xfd7badc8), SP8(0x00d3e2d9), SP8(0x00c183d2), SP8(0x084e1950),
    SP8(0x4810d800), SP8(0x017f43fe), SP8(0x01056dd8), SP8(0x00e9cb9f),
    SP8(0x07d7d090), SP8(0x4a708980), SP8(0x0488fae8), SP8(0x0113bd20),
    SP8(0x0107b1a8), SP8(0x069fb3c0), SP8(0x4b3db200), SP8(0x00763f48)
};right shift 16*/
static const int32_t _sbc_proto_8[40] =
{
    (0x000002e5), (0x000022d0), (0x0000006b), (0x00000780),
    (0x00003f1c), (0xfffff881), (0x0000002c), (0x0000055a),
    (0x000031f5), (0xffffebfe), (0xffffff27), (0x00000014),
    (0x0000041c), (0x00002a7c), (0xffffe4c4), (0xfffffe35),
    (0x00000048), (0x00000686), (0x000038ee), (0xfffff2a1),
    (0xffffffe8), (0x00000095), (0x00000824), (0x0000443b),
    (0xfffffd7b), (0x000000d3), (0x000000c1), (0x0000084e),
    (0x00004810), (0x0000017f), (0x00000105), (0x000000e9),
    (0x000007d7), (0x00004a70), (0x00000488), (0x00000113),
    (0x00000107), (0x0000069f), (0x00004b3d), (0x00000076)
};


/*static const int32_t sbc_proto_4_40m0[] = {
    SS4(0x00000000), SS4(0xffa6982f), SS4(0xfba93848), SS4(0x0456c7b8),
    SS4(0x005967d1), SS4(0xfffb9ac7), SS4(0xff589157), SS4(0xf9c2a8d8),
    SS4(0x027c1434), SS4(0x0019118b), SS4(0xfff3c74c), SS4(0xff137330),
    SS4(0xf81b8d70), SS4(0x00ec1b8b), SS4(0xfff0b71a), SS4(0xffe99b00),
    SS4(0xfef84470), SS4(0xf6fb4370), SS4(0xffcdc351), SS4(0xffe01dc7)
};*/
static const int32_t sbc_proto_4_40m0[] =
{
    (0x00000000), (0xfffffa69), (0xffffba93), (0x0000456c),
    (0x00000596), (0xffffffb9), (0xfffff589), (0xffff9c2a),
    (0x000027c1), (0x00000191), (0xffffff3c), (0xfffff137),
    (0xffff81b8), (0x00000ec1), (0xffffff0b), (0xfffffe99),
    (0xffffef84), (0xffff6fb4), (0xfffffcdc), (0xfffffe01)
};



/*static const int32_t sbc_proto_4_40m1[] = {
    SS4(0xffe090ce), SS4(0xff2c0475), SS4(0xf694f800), SS4(0xff2c0475),
    SS4(0xffe090ce), SS4(0xffe01dc7), SS4(0xffcdc351), SS4(0xf6fb4370),
    SS4(0xfef84470), SS4(0xffe99b00), SS4(0xfff0b71a), SS4(0x00ec1b8b),
    SS4(0xf81b8d70), SS4(0xff137330), SS4(0xfff3c74c), SS4(0x0019118b),
    SS4(0x027c1434), SS4(0xf9c2a8d8), SS4(0xff589157), SS4(0xfffb9ac7)
};*/
static const int32_t sbc_proto_4_40m1[] =
{
    (0xfffffe09), (0xfffff2c0), (0xffff694f), (0xfffff2c0),
    (0xfffffe09), (0xfffffe01), (0xfffffcdc), (0xffff6fb4),
    (0xffffef84), (0xfffffe99), (0xffffff0b), (0x00000ec1),
    (0xffff81b8), (0xfffff137), (0xffffff3c), (0x00000191),
    (0x000027c1), (0xffff9c2a), (0xfffff589), (0xffffffb9)
};



/*static const int32_t sbc_proto_8_80m0[] = {
    SS8(0x00000000), SS8(0xfe8d1970), SS8(0xee979f00), SS8(0x11686100),
    SS8(0x0172e690), SS8(0xfff5bd1a), SS8(0xfdf1c8d4), SS8(0xeac182c0),
    SS8(0x0d9daee0), SS8(0x00e530da), SS8(0xffe9811d), SS8(0xfd52986c),
    SS8(0xe7054ca0), SS8(0x0a00d410), SS8(0x006c1de4), SS8(0xffdba705),
    SS8(0xfcbc98e8), SS8(0xe3889d20), SS8(0x06af2308), SS8(0x000bb7db),
    SS8(0xffca00ed), SS8(0xfc3fbb68), SS8(0xe071bc00), SS8(0x03bf7948),
    SS8(0xffc4e05c), SS8(0xffb54b3b), SS8(0xfbedadc0), SS8(0xdde26200),
    SS8(0x0142291c), SS8(0xff960e94), SS8(0xff9f3e17), SS8(0xfbd8f358),
    SS8(0xdbf79400), SS8(0xff405e01), SS8(0xff7d4914), SS8(0xff8b1a31),
    SS8(0xfc1417b8), SS8(0xdac7bb40), SS8(0xfdbb828c), SS8(0xff762170)
};*/
static const int32_t sbc_proto_8_80m0[] =
{
    (0x00000000), (0xfffffa34), (0xffffba5e), (0x000045a1),
    (0x000005cb), (0xffffffd6), (0xfffff7c7), (0xffffab06),
    (0x00003676), (0x00000394), (0xffffffa6), (0xfffff54a),
    (0xffff9c15), (0x00002803), (0x000001b0), (0xffffff6e),
    (0xfffff2f2), (0xffff8e22), (0x00001abc), (0x0000002e),
    (0xffffff28), (0xfffff0fe), (0xffff81c6), (0x00000efd),
    (0xffffff13), (0xfffffed5), (0xffffefb6), (0xffff7789),
    (0x00000508), (0xfffffe58), (0xfffffe7c), (0xffffef63),
    (0xffff6fde), (0xfffffd01), (0xfffffdf5), (0xfffffe2c),
    (0xfffff050), (0xffff6b1e), (0xfffff6ee), (0xfffffdd8)
};


/*static const int32_t sbc_proto_8_80m1[] = {
    SS8(0xff7c272c), SS8(0xfcb02620), SS8(0xda612700), SS8(0xfcb02620),
    SS8(0xff7c272c), SS8(0xff762170), SS8(0xfdbb828c), SS8(0xdac7bb40),
    SS8(0xfc1417b8), SS8(0xff8b1a31), SS8(0xff7d4914), SS8(0xff405e01),
    SS8(0xdbf79400), SS8(0xfbd8f358), SS8(0xff9f3e17), SS8(0xff960e94),
    SS8(0x0142291c), SS8(0xdde26200), SS8(0xfbedadc0), SS8(0xffb54b3b),
    SS8(0xffc4e05c), SS8(0x03bf7948), SS8(0xe071bc00), SS8(0xfc3fbb68),
    SS8(0xffca00ed), SS8(0x000bb7db), SS8(0x06af2308), SS8(0xe3889d20),
    SS8(0xfcbc98e8), SS8(0xffdba705), SS8(0x006c1de4), SS8(0x0a00d410),
    SS8(0xe7054ca0), SS8(0xfd52986c), SS8(0xffe9811d), SS8(0x00e530da),
    SS8(0x0d9daee0), SS8(0xeac182c0), SS8(0xfdf1c8d4), SS8(0xfff5bd1a)
};*/
static const int32_t sbc_proto_8_80m1[] =
{
    (0xfffffdf0), (0xfffff2c0), (0xffff6984), (0xfffff2c0),
    (0xfffffdf0), (0xfffffdd8), (0xfffff6ee), (0xffff6b1e),
    (0xfffff050), (0xfffffe2c), (0xfffffdf5), (0xfffffd01),
    (0xffff6fde), (0xffffef63), (0xfffffe7c), (0xfffffe58),
    (0x00000508), (0xffff7789), (0xffffefb6), (0xfffffed5),
    (0xffffff13), (0x00000efd), (0xffff81c6), (0xfffff0fe),
    (0xffffff28), (0x0000002e), (0x00001abc), (0xffff8e22),
    (0xfffff2f2), (0xffffff6e), (0x000001b0), (0x00002803),
    (0xffff9c15), (0xfffff54a), (0xffffffa6), (0x00000394),
    (0x00003676), (0xffffab06), (0xfffff7c7), (0xffffffd6)
};


/*static const int32_t _anamatrix8[8] = {
    SA8(0x3b20d780), SA8(0x187de2a0), SA8(0x3ec52f80), SA8(0x3536cc40),
    SA8(0x238e7680), SA8(0x0c7c5c20), SA8(0x2d413cc0), SA8(0x40000000)
}; right shift 17 */
static const int32_t _anamatrix8[8] =
{
    (0x00001d90), (0x00000c3e), (0x00001f62), (0x00001a9b),
    (0x000011c7), (0x0000063e), (0x000016a0), (0x00002000)
};



/*static const int32_t synmatrix4[8][4] = {
    { SN4(0x05a82798), SN4(0xfa57d868), SN4(0xfa57d868), SN4(0x05a82798) },
    { SN4(0x030fbc54), SN4(0xf89be510), SN4(0x07641af0), SN4(0xfcf043ac) },
    { SN4(0x00000000), SN4(0x00000000), SN4(0x00000000), SN4(0x00000000) },
    { SN4(0xfcf043ac), SN4(0x07641af0), SN4(0xf89be510), SN4(0x030fbc54) },
    { SN4(0xfa57d868), SN4(0x05a82798), SN4(0x05a82798), SN4(0xfa57d868) },
    { SN4(0xf89be510), SN4(0xfcf043ac), SN4(0x030fbc54), SN4(0x07641af0) },
    { SN4(0xf8000000), SN4(0xf8000000), SN4(0xf8000000), SN4(0xf8000000) },
    { SN4(0xf89be510), SN4(0xfcf043ac), SN4(0x030fbc54), SN4(0x07641af0) }
};*/
static const int32_t synmatrix4[8][4] =
{
    { (0x0000b504), (0xffff4afb), (0xffff4afb), (0x0000b504) },
    { (0x000061f7), (0xffff137c), (0x0000ec83), (0xffff9e08) },

    { (0x00000000), (0x00000000), (0x00000000), (0x00000000) },
    { (0xffff9e08), (0x0000ec83), (0xffff137c), (0x000061f7) },

    { (0xffff4afb), (0x0000b504), (0x0000b504), (0xffff4afb) },
    { (0xffff137c), (0xffff9e08), (0x000061f7), (0x0000ec83) },

    { (0xffff0000), (0xffff0000), (0xffff0000), (0xffff0000) },
    { (0xffff137c), (0xffff9e08), (0x000061f7), (0x0000ec83) }
};



/*static const int32_t synmatrix8[16][8] = {
    { SN8(0x05a82798), SN8(0xfa57d868), SN8(0xfa57d868), SN8(0x05a82798),
      SN8(0x05a82798), SN8(0xfa57d868), SN8(0xfa57d868), SN8(0x05a82798) },
    { SN8(0x0471ced0), SN8(0xf8275a10), SN8(0x018f8b84), SN8(0x06a6d988),
      SN8(0xf9592678), SN8(0xfe70747c), SN8(0x07d8a5f0), SN8(0xfb8e3130) },
    { SN8(0x030fbc54), SN8(0xf89be510), SN8(0x07641af0), SN8(0xfcf043ac),
      SN8(0xfcf043ac), SN8(0x07641af0), SN8(0xf89be510), SN8(0x030fbc54) },
    { SN8(0x018f8b84), SN8(0xfb8e3130), SN8(0x06a6d988), SN8(0xf8275a10),
      SN8(0x07d8a5f0), SN8(0xf9592678), SN8(0x0471ced0), SN8(0xfe70747c) },
    { SN8(0x00000000), SN8(0x00000000), SN8(0x00000000), SN8(0x00000000),
      SN8(0x00000000), SN8(0x00000000), SN8(0x00000000), SN8(0x00000000) },
    { SN8(0xfe70747c), SN8(0x0471ced0), SN8(0xf9592678), SN8(0x07d8a5f0),
      SN8(0xf8275a10), SN8(0x06a6d988), SN8(0xfb8e3130), SN8(0x018f8b84) },
    { SN8(0xfcf043ac), SN8(0x07641af0), SN8(0xf89be510), SN8(0x030fbc54),
      SN8(0x030fbc54), SN8(0xf89be510), SN8(0x07641af0), SN8(0xfcf043ac) },
    { SN8(0xfb8e3130), SN8(0x07d8a5f0), SN8(0xfe70747c), SN8(0xf9592678),
      SN8(0x06a6d988), SN8(0x018f8b84), SN8(0xf8275a10), SN8(0x0471ced0) },
    { SN8(0xfa57d868), SN8(0x05a82798), SN8(0x05a82798), SN8(0xfa57d868),
      SN8(0xfa57d868), SN8(0x05a82798), SN8(0x05a82798), SN8(0xfa57d868) },
    { SN8(0xf9592678), SN8(0x018f8b84), SN8(0x07d8a5f0), SN8(0x0471ced0),
      SN8(0xfb8e3130), SN8(0xf8275a10), SN8(0xfe70747c), SN8(0x06a6d988) },
    { SN8(0xf89be510), SN8(0xfcf043ac), SN8(0x030fbc54), SN8(0x07641af0),
      SN8(0x07641af0), SN8(0x030fbc54), SN8(0xfcf043ac), SN8(0xf89be510) },
    { SN8(0xf8275a10), SN8(0xf9592678), SN8(0xfb8e3130), SN8(0xfe70747c),
      SN8(0x018f8b84), SN8(0x0471ced0), SN8(0x06a6d988), SN8(0x07d8a5f0) },
    { SN8(0xf8000000), SN8(0xf8000000), SN8(0xf8000000), SN8(0xf8000000),
      SN8(0xf8000000), SN8(0xf8000000), SN8(0xf8000000), SN8(0xf8000000) },
    { SN8(0xf8275a10), SN8(0xf9592678), SN8(0xfb8e3130), SN8(0xfe70747c),
      SN8(0x018f8b84), SN8(0x0471ced0), SN8(0x06a6d988), SN8(0x07d8a5f0) },
    { SN8(0xf89be510), SN8(0xfcf043ac), SN8(0x030fbc54), SN8(0x07641af0),
      SN8(0x07641af0), SN8(0x030fbc54), SN8(0xfcf043ac), SN8(0xf89be510) },
    { SN8(0xf9592678), SN8(0x018f8b84), SN8(0x07d8a5f0), SN8(0x0471ced0),
      SN8(0xfb8e3130), SN8(0xf8275a10), SN8(0xfe70747c), SN8(0x06a6d988) }
};*/
static const int32_t synmatrix8[16][8] =
{
    {
        (0x0000b504), (0xffff4afb), (0xffff4afb), (0x0000b504),
        (0x0000b504), (0xffff4afb), (0xffff4afb), (0x0000b504)
    },

    {
        (0x00008e39), (0xffff04eb), (0x000031f1), (0x0000d4db),
        (0xffff2b24), (0xffffce0e), (0x0000fb14), (0xffff71c6)
    },

    {
        (0x000061f7), (0xffff137c), (0x0000ec83), (0xffff9e08),
        (0xffff9e08), (0x0000ec83), (0xffff137c), (0x000061f7)
    },

    {
        (0x000031f1), (0xffff71c6), (0x0000d4db), (0xffff04eb),
        (0x0000fb14), (0xffff2b24), (0x00008e39), (0xffffce0e)
    },

    {
        (0x00000000), (0x00000000), (0x00000000), (0x00000000),
        (0x00000000), (0x00000000), (0x00000000), (0x00000000)
    },

    {
        (0xffffce0e), (0x00008e39), (0xffff2b24), (0x0000fb14),
        (0xffff04eb), (0x0000d4db), (0xffff71c6), (0x000031f1)
    },

    {
        (0xffff9e08), (0x0000ec83), (0xffff137c), (0x000061f7),
        (0x000061f7), (0xffff137c), (0x0000ec83), (0xffff9e08)
    },

    {
        (0xffff71c6), (0x0000fb14), (0xffffce0e), (0xffff2b24),
        (0x0000d4db), (0x000031f1), (0xffff04eb), (0x00008e39)
    },

    {
        (0xffff4afb), (0x0000b504), (0x0000b504), (0xffff4afb),
        (0xffff4afb), (0x0000b504), (0x0000b504), (0xffff4afb)
    },

    {
        (0xffff2b24), (0x000031f1), (0x0000fb14), (0x00008e39),
        (0xffff71c6), (0xffff04eb), (0xffffce0e), (0x0000d4db)
    },

    {
        (0xffff137c), (0xffff9e08), (0x000061f7), (0x0000ec83),
        (0x0000ec83), (0x000061f7), (0xffff9e08), (0xffff137c)
    },

    {
        (0xffff04eb), (0xffff2b24), (0xffff71c6), (0xffffce0e),
        (0x000031f1), (0x00008e39), (0x0000d4db), (0x0000fb14)
    },

    {
        (0xffff0000), (0xffff0000), (0xffff0000), (0xffff0000),
        (0xffff0000), (0xffff0000), (0xffff0000), (0xffff0000)
    },

    {
        (0xffff04eb), (0xffff2b24), (0xffff71c6), (0xffffce0e),
        (0x000031f1), (0x00008e39), (0x0000d4db), (0x0000fb14)
    },

    {
        (0xffff137c), (0xffff9e08), (0x000061f7), (0x0000ec83),
        (0x0000ec83), (0x000061f7), (0xffff9e08), (0xffff137c)
    },

    {
        (0xffff2b24), (0x000031f1), (0x0000fb14), (0x00008e39),
        (0xffff71c6), (0xffff04eb), (0xffffce0e), (0x0000d4db)
    }
};
