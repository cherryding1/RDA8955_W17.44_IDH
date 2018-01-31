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





#ifndef FFMPEG_EVAL_H
#define FFMPEG_EVAL_H

#if LIBAVCODEC_VERSION_INT < ((52<<16)+(0<<8)+0)
/**
 * @deprecated Use ff_eval2 instead
 */
double ff_eval(char *s, double *const_value, const char **const_name,
               double (**func1)(void *, double), const char **func1_name,
               double (**func2)(void *, double, double), char **func2_name,
               void *opaque);
#endif

/**
 * Parses and evaluates an expression.
 * Note, this is significantly slower than ff_parse_eval()
 * @param s expression as a zero terminated string for example "1+2^3+5*5+sin(2/3)"
 * @param func1 NULL terminated array of function pointers for functions which take 1 argument
 * @param func2 NULL terminated array of function pointers for functions which take 2 arguments
 * @param const_name NULL terminated array of zero terminated strings of constant identifers for example {"PI", "E", 0}
 * @param func1_name NULL terminated array of zero terminated strings of func1 identifers
 * @param func2_name NULL terminated array of zero terminated strings of func2 identifers
 * @param error pointer to a char* which is set to an error message if something goes wrong
 * @param const_value a zero terminated array of values for the identifers from const_name
 * @param opaque a pointer which will be passed to all functions from func1 and func2
 * @return the value of the expression
 */
double ff_eval2(const char *s, double *const_value, const char **const_name,
                double (**func1)(void *, double), const char **func1_name,
                double (**func2)(void *, double, double), char **func2_name,
                void *opaque, const char **error);

typedef struct ff_expr_s AVEvalExpr;

/**
 * Parses a expression.
 * @param s expression as a zero terminated string for example "1+2^3+5*5+sin(2/3)"
 * @param func1 NULL terminated array of function pointers for functions which take 1 argument
 * @param func2 NULL terminated array of function pointers for functions which take 2 arguments
 * @param const_name NULL terminated array of zero terminated strings of constant identifers for example {"PI", "E", 0}
 * @param func1_name NULL terminated array of zero terminated strings of func1 identifers
 * @param func2_name NULL terminated array of zero terminated strings of func2 identifers
 * @param error pointer to a char* which is set to an error message if something goes wrong
 * @return AVEvalExpr which must be freed with ff_eval_free by the user when it is not needed anymore
 *         NULL if anything went wrong
 */
AVEvalExpr * ff_parse(const char *s, const char **const_name,
                      double (**func1)(void *, double), const char **func1_name,
                      double (**func2)(void *, double, double), char **func2_name,
                      const char **error);
/**
 * Evaluates a previously parsed expression.
 * @param const_value a zero terminated array of values for the identifers from ff_parse const_name
 * @param opaque a pointer which will be passed to all functions from func1 and func2
 * @return the value of the expression
 */
double ff_parse_eval(AVEvalExpr * e, double *const_value, void *opaque);
void ff_eval_free(AVEvalExpr * e);

#endif /* FFMPEG_EVAL_H */
