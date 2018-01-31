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



#ifndef RV_DECODE_H__
#define RV_DECODE_H__

/* Simple unified decoder frontend for RealVideo */

#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"
#include "rm_error.h"
#include "rv_format_info.h"
#include "rvmsg.h"
#include "rv_backend.h"
#include "rv_backend_types.h"
#include "rvstruct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The rv_decode struct contains the RealVideo decoder frontend
 * state variables and backend instance pointer. */

typedef struct rv_decode_struct
{
    rm_error_func_ptr          fpError;
    /* User defined error function.                                   */

    void*                      pUserError;
    /* User defined parameter for error function.                     */

    rm_malloc_func_ptr         fpMalloc;
    /* User defined malloc function.                                  */

    rm_free_func_ptr           fpFree;
    /* User defined free function.                                    */

    void*                      pUserMem;
    /* User defined parameter for malloc and free functions.          */

    UINT32                     ulSPOExtra;
    /* The SPO Extra bits. Opaque data appended to bitstream header.  */

    UINT32                     ulStreamVersion;
    /* The stream version. Opaque data following the SPO Extra bits.  */

    UINT32                     ulMajorBitstreamVersion;
    /* The major bitstream version. Indicates RV7, RV8, RV9, etc.     */

    UINT32                     ulMinorBitstreamVersion;
    /* The minor bitstream version. Indicates minor revision or RAW.  */

    UINT32                     ulNumResampledImageSizes;
    /* The number of RPR sizes. Optional RV7 and RV8 opaque data.     */

    UINT32                     ulEncodeSize;
    /* The maximum encoded frame dimensions. Optional RV9 opaque data.*/

    UINT32                     ulLargestPels;
    /* The maximum encoded frame width.                               */

    UINT32                     ulLargestLines;
    /* The maximum encoded frame height.                              */

    UINT32                     pDimensions[2*(8+1)];
    /* Table of encoded dimensions, including RPR sizes.              */

    UINT32                     ulOutSize;
    /* The maximum size of the output frame in bytes.                 */

    UINT32                     ulECCMask;
    /* Mask for identifying ECC packets.                              */

    UINT32                     bInputFrameIsReference;
    /* Identifies whether input frame is a key frame or not.          */

    UINT32                     ulInputFrameQuant;
    /* The input frame quantization parameter.                        */

    rv_format_info            *pBitstreamHeader;
    /* The bitstream header.                                          */

    rv_frame                  *pInputFrame;
    /* Pointer to the input frame struct.                             */

    void                      *pDecodeState;
    /* Pointer to decoder backend state.                              */

    rv_backend                *pDecode;
    /* Decoder backend function pointers.                             */

    rv_backend_init_params     pInitParams;
    /* Initialization parameters for the decoder backend.             */

    rv_backend_in_params       pInputParams;
    /* The decoder backend input parameter struct.                    */

    rv_backend_out_params      pOutputParams;
    /* The decoder backend output parameter struct.                   */

} rv_decode;

/* rv_decode_create()
 * Creates RV decoder frontend struct, copies memory utilities.
 * Returns struct pointer on success, NULL on failure. */
rv_decode* rv_decode_create(void* pUserError, rm_error_func_ptr fpError);

rv_decode* rv_decode_create2(void* pUserError, rm_error_func_ptr fpError,
                             void* pUserMem, rm_malloc_func_ptr fpMalloc,
                             rm_free_func_ptr fpFree);

/* rv_decode_destroy()
 * Deletes decoder backend instance, followed by frontend. */
void rv_decode_destroy(rv_decode* pFrontEnd);

/* rv_decode_init()
 * Reads bitstream header, selects and initializes decoder backend.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT rv_decode_init(rv_decode* pFrontEnd, rv_format_info* pHeader);

/* rv_decode_stream_input()
 * Reads frame header and fills decoder input parameters struct. If there
 * is packet loss and ECC packets exist, error correction is attempted.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT rv_decode_stream_input(rv_decode* pFrontEnd, rv_frame* pFrame);

/* rv_decode_stream_decode()
 * Calls decoder backend to decode issued frame and produce an output frame.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT rv_decode_stream_decode(rv_decode* pFrontEnd, UINT8* pOutput);

/* rv_decode_stream_flush()
 * Flushes the latency frame from the decoder backend after the last frame
 * is delivered and decoded before a pause or the end-of-file.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT rv_decode_stream_flush(rv_decode* pFrontEnd, UINT8* pOutput);

/* rv_decode_custom_message()
 * Sends a custom message to the decoder backend.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT rv_decode_custom_message(rv_decode* pFrontEnd, RV_Custom_Message_ID *pMsg_id);

/* rv_decode_ext_maxbuf_sizes()
 * Sends a custom message to the decoder backend.
 * Returns maximum memory size of video buffer used to decode this stream  on success,
 * zero result indicates that external buffers not used
 * negative result indicates failure.
 * note : used only after rv_decode_init() ! */
INT32 rv_decode_ext_maxbuf_sizes(rv_decode* pFrontEnd);

/* rv_decode_ext_getnextfree_idx()
 * Sends a custom message to the decoder backend.
 * Returns abstract internal index in memory allocation table that used for video buffesrs
 * zero result indicates that internal allocation table is full, or not used
 * negative result indicates failure.
 * note : used only after rv_decode_init() ! */
INT32 rv_decode_ext_getnextfree_idx(rv_decode* pFrontEnd);

/* rv_decode_ext_getsize_idx()
 * Sends a custom message to the decoder backend.
 * Returns size in bytes of internal array needed to allocate for this abstract internal index
 * zero result indicates that internal allocation is done, or not used
 * negative result indicates failure.
 * note : used only after rv_decode_init() ! */
INT32 rv_decode_ext_getsize_idx(rv_decode* pFrontEnd, INT32 idx);

/* rv_decode_ext_setptr_idx()
 * Sends a custom message to the decoder backend.
 * Sets internal pointer to a external allocated buffer
 * Returns one on succes
 * zero result indicates that internal poiner already set or not needed
 * negative result indicates failure.
 * note : used only after rv_decode_init() ! */
INT32 rv_decode_ext_setptr_idx(rv_decode* pFrontEnd, INT32 idx, void* ptr);

/* rv_decode_ext_exestart_seq()
 * Sends a custom message to the decoder backend. Starts execution of internal start sequence
 * Returns zero on success, negative result indicates failure.
 * note : used only after rv_decode_init() ! when used external buffer allocation mechanism*/

HX_RESULT rv_decode_ext_exestart_seq(rv_decode* pFrontEnd);


/**************** Accessor Functions *******************/
/* rv_decode_max_output_size()
 * Returns maximum size of YUV 4:2:0 output buffer in bytes. */
UINT32 rv_decode_max_output_size(rv_decode* pFrontEnd);

/* rv_decode_get_output_size()
 * Returns size of most recent YUV 4:2:0 output buffer in bytes. */
UINT32 rv_decode_get_output_size(rv_decode* pFrontEnd);

/* rv_decode_get_output_dimensions()
 * Returns width and height of most recent YUV output buffer. */
HX_RESULT rv_decode_get_output_dimensions(rv_decode* pFrontEnd, UINT32* pWidth,
        UINT32* pHeight);

/* rv_decode_frame_valid()
 * Checks decoder output parameters to see there is a valid output frame.
 * Returns non-zero value if a valid output frame exists, else zero. */
UINT32 rv_decode_frame_valid(rv_decode* pFrontEnd);

/* rv_decode_more_frames()
 * Checks decoder output parameters to see if more output frames can be
 * produced without additional input frames.
 * Returns non-zero value if more frames can be
 * produced without additional input, else zero. */
UINT32 rv_decode_more_frames(rv_decode* pFrontEnd);

/* rv_decode_is_key_frame()
 * Checks decoder output parameters to see if the current frame is key frame
 * or not.
 * Returns non-zero value if the current frame is key frame, else zero. */
UINT32 rv_decode_is_key_frame(rv_decode* pFrontEnd);

/* rv_decode_frame_info()
 * Fills frame allocation data  Y U V planes pointers and other , used for output creation.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_frame_info(rv_decode* pFrontEnd, struct RV_Image* pImageInfo);

HX_RESULT
rv_decode_frame_info_YUV420(rv_decode* pFrontEnd, struct RV_Image* pImageInfo, RV_MSG_GetDisplayed_Image_Info* pMsg);


#ifdef __cplusplus
}
#endif

#endif /* RV_DECODE_H__ */
