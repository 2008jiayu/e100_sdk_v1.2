/**
 * @file inc/mw/player/stilldec.h
 *
 * Amba still decoder header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef STILLDEC_H_
#define STILLDEC_H_

#include "Decode.h"
#include <display/Display.h>

/**
 * @defgroup StillDec
 * @brief Still image decoder
 *
 * Still decode codec provide functions that operated on single frame.\n
 * Including: Idr/I frame decode, jpeg decode, yuv rescale, yuv display, yuv blending.\n
 */

/**
 * @addtogroup StillDec
 * @ingroup Codec
 * @{ 
 */

/**
 * decode callback event
 */
/*
 typedef enum _AMP_STLDEC_CALLBACK_EVENT_e_{
 //dec still
 AMP_STLDEC_CALLBACK_EVENT_DECODE_DONE,         ///< event decode done
 AMP_STLDEC_CALLBACK_EVENT_SCRIPT_DONE,         ///< event decode done
 AMP_STLDEC_CALLBACK_EVENT_CAPTURE_DONE,                ///< event pb capture done
 AMP_STLDEC_CALLBACK_EVENT_YUV2YUV_DONE,                ///< event yuv to yuv done
 AMP_STLDEC_CALLBACK_EVENT_BLEND_DONE,          ///< event blend done
 AMP_STLDEC_CALLBACK_EVENT_ERROR,                       ///< event still decode runtime error
 }AMP_STLDEC_CALLBACK_EVENT_e;
 */

/**
 * AMP_STLDEC_CFG_s
 */
typedef struct _AMP_STILLDEC_CFG_s_ {
    /** background color */
    AMP_YUV_COLOR_s BgColor;

    /** callback function pointer of still decode event */
    AMP_CALLBACK_f CbCodecEvent;

    /** buffer for still raw file */
    UINT8 *RawBuf;

    /** buffer size of the raw buffer */
    UINT32 RawBufSize;

    /** DSP working area address, 0 = no change */
    UINT8 *DspWorkBufAddr;

    /** DSP working area size, 0 = no change */
    UINT32 DspWorkBufSize;
//TBD
} AMP_STILLDEC_CFG_s;

/**
 * AMP_STLDEC_INIT_CFG_s
 */
typedef struct _AMP_STILLDEC_INIT_CFG_s_ {
    /** max hdlr supported on the module */
    UINT32 MaxHdlr;

    /** task info for decode task */
    AMP_TASK_INFO_s TaskInfo;

    /** working buffer for still decode module */
    UINT8 *Buf;

    /** working buffer size for still decode module */
    UINT32 BufSize;
} AMP_STILLDEC_INIT_CFG_s;

/**
 * AMP_STLDEC_DECODE_s
 */
typedef struct _AMP_STILLDEC_DECODE_s_ {
    // over all field
    /** number of file to decode */
    UINT32 NumFile;

    /** decode output buffer */
    UINT8* DecodeBuf;

    /** size of decode output buffer */
    UINT32 SizeDecodeBuf;

    /** pitch in decode buffer for last decoded image */
    UINT32 *DecodedImgPitch;
    // info for each decode

    /** u32 array which's size is numFile for store error state for each decode */
    UINT32 *DecodeState;

    /** AMP_AREA_s array. null to skip scaling. */
    AMP_AREA_s *CropFromDecodedBuf;

    /** AMP_YUV_BUFFER_s array */
    AMP_YUV_BUFFER_s *RescaleDest;

    /** rotation on scaling */
    AMP_ROTATION_e *Rotate;

    /** [Output] The address of decoded Y data of LAST file */
    UINT8* OutAddrY;

    /** [Output] The address of decoded UV data of LAST file */
    UINT8* OutAddrUV;

    /** [Output] The color format of LAST file. */
    AMP_COLOR_FORMAT_e OutColorFmt;
} AMP_STILLDEC_DECODE_s;

typedef struct _AMP_STILLDEC_RESCALE_s_ {
    UINT32 NumBuf;              ///< number of buffer to scale
    AMP_YUV_BUFFER_s *Src;      ///< scaling source
    AMP_YUV_BUFFER_s *Dest;     ///< scaling target
    AMP_ROTATION_e *Rotate;     ///< rotate on scaling
    UINT8 *LumaGain;            ///< luma gain: 1 ~ 255
} AMP_STILLDEC_RESCALE_s;

typedef struct _AMP_STILLDEC_BLEND_s_ {
    AMP_YUV_BUFFER_s *Src1;   ///< scaling source
    AMP_YUV_BUFFER_s *Src2;   ///< scaling source
    AMP_YUV_BUFFER_s *Dest;  ///< scaling target
    UINT8 *AlphaMap;         ///< pointer to alpha map
    UINT32 GlobalAlpha;         ///< take effect when alphaMap = NULL
} AMP_STILLDEC_BLEND_s;

typedef struct _AMP_STILLDEC_DISPLAY_s_ {
    AMP_DISP_CHANNEL_IDX_e Vout; ///< which vout to display on
    AMP_YUV_BUFFER_s *Buf; ///< buf to display, size of AOI must be the same as vout size
} AMP_STILLDEC_DISPLAY_s;

/**
 * Event info for AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT event
 */
typedef struct _AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT_s_ {
    AMP_DISP_CHANNEL_IDX_e Channel; /**< Displayed channel. 0: DCHAN, 1: FCHAN */
    void *VoutYAddr; /**< Luma address of displayed Vout buffer */
    void *VoutUVAddr; /**< Chroma address of displayed Vout buffer */
} AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT_s;

/**
 * Event info for AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT event
 * Information of an array of LCD Vout buffers allocated by DSP
 */
typedef struct _AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT_s_ {
    char*   Address;        /**< Start address of the first buffer */
    UINT32  BufferNumber;   /**< Number of buffers in Vout buffer array */
    UINT32  BufferSize;     /**< Total size of the Vout buffer array */
    UINT16  ImagePitch;     /**< Pitch of the image in Vout buffer */
    UINT16  ImageWidth;     /**< Width of the image in Vout buffer */
    UINT16  ImageHeight;    /**< Height of the image in Vout buffer */
    UINT8   CurrentId;      /**< Index of the buffer currently displayed on-screen */
} AMP_DEC_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT_s;

/**
 * Callback for AMP_DEC_EVENT_JPEG_DEC_YUV_DISP_REPORT event
 */
typedef int (*AMP_DEC_EVENT_HANDLER_f)(void *Hdlr,
                                       UINT32 Event,
                                       void* EventInfo);

/**
 * To get the default value of initial config for video decode module
 *
 * @param [out] cfg - default module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_GetInitDefaultCfg(AMP_STILLDEC_INIT_CFG_s *cfg);

/**
 * To set the initial config for still decode module
 *
 * @param [in] cfg - module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Init(AMP_STILLDEC_INIT_CFG_s *cfg);

/**
 * To get default config of still decode codec
 * 
 * @param [out] cfg - default codec config
 * 
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_GetDefaultCfg(AMP_STILLDEC_CFG_s *cfg);

/**
 * To create still decode codec
 *
 * @param [in] cfg - codec config
 * @param [out] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Create(AMP_STILLDEC_CFG_s *cfg,
                              AMP_STLDEC_HDLR_s **hdlr);

/**
 * To delete stll decode codec
 *
 * @param [in] hdlr - codec handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Delete(AMP_STLDEC_HDLR_s *hdlr);

/**
 * To decode raw data into YUV data
 *
 * @param [in] hdlr - codec handler
 * @param [in] decode - decode info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Decode(AMP_STLDEC_HDLR_s *hdlr,
                              AMP_STILLDEC_DECODE_s *decode);

/**
 * To rescale a YUV buffer
 * the limitation is it should be align the value which depending dsp hw limitation
 *
 * @param [in] hdlr - codec handler
 * @param [in] rescale - rescale info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Rescale(AMP_STLDEC_HDLR_s *hdlr,
                               AMP_STILLDEC_RESCALE_s *rescale);

/**
 * To display the buffer on Vout
 *
 * @param [in] hdlr - codec handler
 * @param [in] display - display info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Display(AMP_STLDEC_HDLR_s *hdlr,
                               AMP_STILLDEC_DISPLAY_s *display);

/**
 * To blend two YUV buffer with the same height and pitch
 *
 * @param [in] hdlr - codec handler
 * @param [in] blend - blending info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpStillDec_Blend(AMP_STLDEC_HDLR_s *hdlr,
                             AMP_STILLDEC_BLEND_s *blend);

/**
 * @}
 */
#endif
