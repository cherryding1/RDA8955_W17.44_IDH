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



# ifndef LIBMAD_HUFFMAN_H
# define LIBMAD_HUFFMAN_H

union huffquad
{
    struct
    {
        unsigned short final  :  1;
        unsigned short bits   :  3;
        unsigned short offset : 12;
    } ptr;
    struct
    {
        unsigned short final  :  1;
        unsigned short hlen   :  3;
        unsigned short v      :  1;
        unsigned short w      :  1;
        unsigned short x      :  1;
        unsigned short y      :  1;
    } value;
    unsigned short final    :  1;
};

union huffpair
{
    struct
    {
        unsigned short final  :  1;
        unsigned short bits   :  3;
        unsigned short offset : 12;
    } ptr;
    struct
    {
        unsigned short final  :  1;
        unsigned short hlen   :  3;
        unsigned short x      :  4;
        unsigned short y      :  4;
    } value;
    unsigned short final    :  1;
};

struct hufftable
{
    union huffpair const *table;
    unsigned short linbits;
    unsigned short startbits;
};

extern union huffquad const *const mad_huff_quad_table[2];
extern struct hufftable const mad_huff_pair_table[32];

# endif
