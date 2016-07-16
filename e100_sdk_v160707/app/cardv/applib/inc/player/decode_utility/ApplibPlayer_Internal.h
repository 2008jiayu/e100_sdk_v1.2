/**
 * @file src/app/connected/applib/inc/player/decode_utility/ApplibPlayer_Internal.h
 *
 * Common functions used internally by player module in application library
 *
 * History:
 *    2013/12/04 - [phcheng] Create file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup ApplibPlayer_Internal          ApplibPlayer_Internal
 * @brief Common functions and definitions only used by player modules.
 *
 * Composed of the following.\n
 * 1. Functions of computing aspect ratio.\n
 * 2. Calculating displayed area in a buffer.\n
 * 3. Drawing in a buffer.
 */

/**
 * @addtogroup ApplibPlayer_Internal
 * @ingroup DecodeUtility
 * @{
 */

#ifndef _APPLIB_PLYR_INTERNAL_H_
#define _APPLIB_PLYR_INTERNAL_H_

#include <applib.h>
//#include <mw.h>
//#include <common/common.h>

#if 0 // Apply fixed-point approach when the calculation by floating point is too slow to endure
typedef INT32 APPLIB_FIX32; ///< 32 bits fixed-point signed number with "FIX32_FRACTION" fraction bits
typedef UINT32 APPLIB_UFIX32; ///< 32 bits fixed-point unsigned number with "FIX32_FRACTION" fraction bits

#define FIX32_FRACTION 16 ///< Number of fraction bits of fixed-point type "APPLIB_FIX32"
#define UINT32_TO_UFIX32(x) ((APPLIB_UFIX32)((x) << FIX32_FRACTION)) ///< 0 <= x <= 65535
#define UFIX32_TO_UINT32(x) ((UINT32)((x) >> FIX32_FRACTION))
#define INT32_TO_FIX32(x) ((APPLIB_FIX32)((x) << FIX32_FRACTION)) ///< -32768 <= x <= 32767
#define FIX32_TO_INT32(x) ((INT32)((x) >> FIX32_FRACTION))
#endif

#define ASPECT_RATIO(height, width) (((height) << 16) / (width)) ///< Calculate aspect ratio. Using integer and shifting to imitate floating-point operations.
#define ASPECT_RATIO_INVERSE(ar) ((0xFFFFFFFF) / (ar)) ///< Calculate multiplicative inverse of the aspect ratio derived from macro ASPECT_RATIO. It's an approximation of (1 << 32)/(ar) to avoid "<< 32" overflow

/**
 * Calculation parameter. Information for calculating parameters of rescaling when displaying image
 */
typedef struct _APPLIB_DISP_SIZE_CAL_s_ {
    /**
     * [Input] Width of the image in main/cache buffer BEFORE rotation.         \n
     * Pixels may not be square when the image is storerd in a cache buffer.
     */
    UINT32 ImageWidth;
    /**
     * [Input] Height of the image in main/cache buffer BEFORE rotation.        \n
     * Pixels may not be square when the image is storerd in a cache buffer.
     */
    UINT32 ImageHeight;
    /**
     * [Input] The aspect ratio of the original image BEFORE rotation.          \n
     * A fixed point number with 16 binary fractional digits.
     * ImageAr = ASPECT_RATIO(ImageH, ImageW)
     */
    UINT32 ImageAr;
    /** [Input] Ratate option of the image */
    AMP_ROTATION_e ImageRotate;
    /**
     * [Input] Aspect ratio of the screen measured by physical length (not pixels). \n
     * An aspect ratio of 4:3, for example, would be 0x0403.
     */
    UINT32 DeviceAr;
    /** [Input] Width of the screen (number of pixels). */
    UINT32 DeviceWidth;
    /** [Input] Height of the screen (number of pixels). */
    UINT32 DeviceHeight;
    /** [Input] Width, in pixels on screen, of the window (a portion of the screen to display image). */
    UINT32 WindowWidth;
    /** [Input] Height, in pixels on screen, of the window (a portion of the screen to display image). */
    UINT32 WindowHeight;
    /**
     * [Input] How many pixels of the geometric center of image are to shift along x-axis.                              \n
     * The pixels are square ones of the original image.                                                                \n
     * Area of the image outside of the window will be cropped.                                                         \n
     * Set ImageShiftX = 0 to keep position along x-axis after the image is centered in and stretched to the window.    \n
     * Set ImageShiftX > 0 to shift the image right.
     * Set ImageShiftX < 0 to shift the image left.
     */
    INT32 ImageShiftX;
    /**
     * [Input] How many pixels of the geometric center of image are to shift along y-axis.                              \n
     * The pixels are square ones of the original image.                                                                \n
     * Area of the image outside of the window will be cropped.                                                         \n
     * Set ImageShiftY = 0 to keep position along y-axis after the image is centered in and stretched to the window.    \n
     * Set ImageShiftY > 0 to shift the image down.
     * Set ImageShiftY < 0 to shift the image up.
     */
    INT32 ImageShiftY;
    /**
     *  [Input] Magnification Factor. Percentage of displayed image relative to                                         \n
     *  the original one that is centered in and stretched to the window.                                               \n
     *  Area of the image outside of the window will be cropped.                                                        \n
     *  Set MagFactor = 100 to get the original image.                                                                  \n
     *  Set MagFactor = 200 to get an image twice as large as the original one.
     */
    UINT32 MagFactor;
    /**
     *  [Input] Adjust the position of the image automatically      \n
     *  when zoom and shift are applied.                            \n
     *  0: Don't Adjust image                                       \n
     *  1: adjust image
     */
    UINT8 AutoAdjust;
    /** [Output] X coordinate relative to upper-left corner of the window. */
    UINT32 OutputOffsetX;
    /** [Output] Y coordinate relative to upper-left corner of the window. */
    UINT32 OutputOffsetY;
    /** [Output] Width, in pixels on screen, of the image. */
    UINT32 OutputWidth;
    /** [Output] Height, in pixels on screen, of the image. */
    UINT32 OutputHeight;
    /** [Output] X coordinate of AOI of the source image in YUV buffer. */
    double OutputSrcImgOffsetX;
    /** [Output] Y coordinate of AOI of the source image in YUV buffer. */
    double OutputSrcImgOffsetY;
    /** [Output] Width of AOI of the source image in YUV buffer. */
    double OutputSrcImgWidth;
    /** [Output] Height of AOI of the source image in YUV buffer. */
    double OutputSrcImgHeight;
    /**
     *  [Output] The eventual number of pixels (of the original image) shifted along X-axis.    \n
     *  When adjustment are made (AutoAdjust = 1), there may be difference                      \n
     *  between "OutputRealImageShiftX" and "ImageShiftX".                                      \n
     *  Otherwise, "OutputRealImageShiftX" and "ImageShiftX" are equal.
     */
    INT32 OutputRealImageShiftX;
    /**
     *  [Output] The eventual number of pixels (of the original image) shifted along Y-axis.    \n
     *  When adjustment are made (AutoAdjust = 1), there may be difference                      \n
     *  between "OutputRealImageShiftY" and "ImageShiftY".                                      \n
     *  Otherwise, "OutputRealImageShiftY" and "ImageShiftY" are equal.
     */
    INT32 OutputRealImageShiftY;
} APPLIB_DISP_SIZE_CAL_s;

/**
 * Calculation parameter array  \n
 * It's used when calculation of multiple channels have to be consistent.
 */
typedef struct _APPLIB_DISP_SIZE_CAL_ARRAY_s_ {
    /**
     * Array of pointers to calculation parameters of all channels. \n
     * A channel will be excluded when its pointer = NULL.
     */
    APPLIB_DISP_SIZE_CAL_s* Cal[DISP_CH_NUM];
} APPLIB_DISP_SIZE_CAL_ARRAY_s;

/**
 * Information for drawing
 */
typedef struct _APPLIB_DRAW_CLOLR_s_ {
    /** Y value of pen's color. 0 ~ 255 */
    UINT8 PenColorY;
    /** U value of pen's color. 0 ~ 255 */
    UINT8 PenColorU;
    /** V value of pen's color. 0 ~ 255 */
    UINT8 PenColorV;
    /** Alpha value (for blending) of pen's color. 0 ~ 255 */
    UINT8 PenColorAlpha;
} APPLIB_DRAW_COLOR_s;

/**
 * Information for drawing a frame
 */
typedef struct _APPLIB_DRAW_FRAME_CONFIG_s_ {
    /** Target of drawing. */
    AMP_YUV_BUFFER_s TargetBuffer;
    /** Color of frame. */
    APPLIB_DRAW_COLOR_s FrameColor;
    /** Thickness of the frame (number of pixels). */
    UINT32 Thickness;
} APPLIB_DRAW_FRAME_CONFIG_s;

/**
 * Information for calculating location and size of PIP frame
 */
typedef struct _APPLIB_PIP_FRAME_CAL_s_ {
    /** Buffer width of image source. */
    UINT32 ImageSrcWidth;
    /** Buffer height of image source. */
    UINT32 ImageSrcHeight;
    /** Displayed area of image source. */
    AMP_AREA_s ImageSrcAOI;
    /** PIP AOI in Vout buffer. */
    AMP_AREA_s ImagePipAOI;
    /** Rotate and flip setting. */
    AMP_ROTATION_e ImageRotate;
    /** PIP frame AOI in Vout buffer. It's a subset of PIP area. */
    AMP_AREA_s OutputPipFrameAOI;
} APPLIB_PIP_FRAME_CAL_s;

/**
 * Calculate location and size of a image in order to display original aspect ratio on screen.\n
 * Resize the image to fit in the window, and center it horizontally and vertically. Zoom and \n
 * shift the image afterwards.
 *
 * @param [in] param            Include input and output of the function
 *
 * @return 0 - OK, others - Error
 */
extern int Applib_DisplaySizeCal(APPLIB_DISP_SIZE_CAL_s *param);

/**
 * Calculate location and size of a image in multiple channel. Force the shift of a image in    \n
 * different channel to be consistent.                                                          \n
 * Exclude channel with its address in "param" equal to NULL. If all channel are NULL, simply   \n
 * assign input shift to output shift.
 *
 * @param [in,out] param        Calculation parameter for all channels
 * @param [in] InputShiftX      Requested shift on the X-axis
 * @param [in] InputShiftY      Requested Shift on the Y-axis
 * @param [out] OutputShiftX    Eventual shift on the X-axis
 * @param [out] OutputShiftY    Eventual shift on the Y-axis
 *
 * @return 0 - OK, others - Error
 */
extern int Applib_DisplaySizeCal_MultiChannel(
    APPLIB_DISP_SIZE_CAL_ARRAY_s *param,
    const INT32 InputShiftX,
    const INT32 InputShiftY,
    INT32 *OutputShiftX,
    INT32 *OutputShiftY);

/**
 * Draw a solid rectangle in a YUV buffer.
 *
 * @param [in] TargetBuffer     Target of drawing
 * @param [in] RectColor        Color of rectangle
 *
 * @return 0 - OK, others - Error
 */
extern int Applib_Draw_Rectangle(const AMP_YUV_BUFFER_s *TargetBuffer, const APPLIB_DRAW_COLOR_s *RectColor);

/**
 * Draw a frame in a YUV buffer
 *
 * @param [in] param            Include target buffer, frame color, and frame size
 *
 * @return 0 - OK, others - Error
 */
extern int Applib_Draw_Frame(APPLIB_DRAW_FRAME_CONFIG_s *param);

/**
 * Calculate location and size of a pip frame.
 *
 * @param [in] param            Include input and output of the function
 *
 * @return 0 - OK, others - Error
 */
extern int Applib_PipFrameSizeCal(APPLIB_PIP_FRAME_CAL_s *param);

/**
 * Set color of YUV buffer to black.                                                    \n
 * YUV buffer is composed of Y buffer and UV buffer. Both buffers have (BufSize) bytes. \n
 * YUV value of black color: Y = 0, U = 128, V = 128
 *
 * @param [in] StartLumaAddr    Start address of Y. Set each byte of the buffer to 0.
 * @param [in] StartChromaAddr  Start address of UV. Set each byte of the buffer to 128.
 * @param [in] YBufSize         The size, in bytes, of Y buffer
 * @param [in] UvBufSize        The size, in bytes, of UV buffer
 *
 * @return None
 */
extern void AppLib_SetYuvBuf_Black(UINT8* StartLumaAddr, UINT8* StartChromaAddr, const UINT32 YBufSize, const UINT32 UvBufSize);

/**
 * @}
 */     // End of group ApplibPlayer_Internal

#endif /* _APPLIB_PLYR_INTERNAL_H_ */
