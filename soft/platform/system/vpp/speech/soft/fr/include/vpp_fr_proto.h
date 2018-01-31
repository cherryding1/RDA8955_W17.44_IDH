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





#ifndef PROTO_H
#define PROTO_H

#if __cplusplus
#       define  NeedFunctionPrototypes  1
#endif

#       define  NeedFunctionPrototypes  1

#undef  P       /* gnu stdio.h actually defines this...         */
#undef  P0
#undef  P1
#undef  P2
#undef  P3
#undef  P4
#undef  P5
#undef  P6
#undef  P7
#undef  P8

#if NeedFunctionPrototypes

#       define  P( protos )     protos

#       define  P0()                            (void)
#       define  P1(x, a)                        (a)
#       define  P2(x, a, b)                     (a, b)
#       define  P3(x, a, b, c)                  (a, b, c)
#       define  P4(x, a, b, c, d)               (a, b, c, d)
#       define  P5(x, a, b, c, d, e)            (a, b, c, d, e)
#       define  P6(x, a, b, c, d, e, f)         (a, b, c, d, e, f)
#       define  P7(x, a, b, c, d, e, f, g)      (a, b, c, d, e, f, g)
#       define  P8(x, a, b, c, d, e, f, g, h)   (a, b, c, d, e, f, g, h)

#else /* !NeedFunctionPrototypes */

#       define  P( protos )     ( /* protos */ )

#       define  P0()                            ()
#       define  P1(x, a)                        x a;
#       define  P2(x, a, b)                     x a; b;
#       define  P3(x, a, b, c)                  x a; b; c;
#       define  P4(x, a, b, c, d)               x a; b; c; d;
#       define  P5(x, a, b, c, d, e)            x a; b; c; d; e;
#       define  P6(x, a, b, c, d, e, f)         x a; b; c; d; e; f;
#       define  P7(x, a, b, c, d, e, f, g)      x a; b; c; d; e; f; g;
#       define  P8(x, a, b, c, d, e, f, g, h)   x a; b; c; d; e; f; g; h;

#endif  /* !NeedFunctionPrototypes */

#endif  /* PROTO_H */
