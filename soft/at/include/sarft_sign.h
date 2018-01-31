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


#ifndef _SARFT_SIGN_
#define _SARFT_SIGN_
extern int sarft_sign_errno;

/**
 * function: get lib version.
 * called by: gprs module application software.
 * implemented by: this sign lib implement this interface.
 * warning: Don't modify the data pointed by the pointer which
 *          sarft_get_version() return.
 */
const char *sarft_get_version();

/**
 *function: sign data with recieved randnum
 *called by: gprs module application software.
 *implemented by:this lib implement this interface.
 * @data[in]: the data need to be sign.
 * @data_len[in]: the size of data to be signed in byte.
 * @randnum[in]:  the randnum recieved from stb
 * @randnum_len[in]: the length of randnum
 * @cid[in]:stb id, that recieved from stb when initialize boot
 * @gmid[in]:gprs module id
 * @seed[in]:key seed that recieved from stb when initialize boot
 * @signres[in]:is the address that save the sign result.
 * @sign_len[in,out]: [in] is the size in byte that can save the sign result.
 *                    [out] is the actually size of the sign result in byte.
 * @return:if successfully return 0, otherwise return nonzero. When return
 *         nonzero, the caller may call sarft_error_str function with
 *         sarft_sign_errno to error description.
**/
int sarft_sign_data(unsigned char *data, int data_len, unsigned char *randnum,
                    int randnum_len, const unsigned char *cid, const unsigned char *gmid,
                    unsigned char *seed, unsigned char *signres, int *sign_len);

/**
 *function: initialize the lib,
 *called by: gprs module application software.
 *implemented by:this lib implement this interface.
 * @errorno[in]: sarft_sign_errno.
 * @return: error description.
 * warning: Don't modify the data pointed by the pointer which sarft_error_str
 *          return.
**/
const char *sarft_error_str(int errorno);
#endif
