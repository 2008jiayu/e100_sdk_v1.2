/**
 * @file src/app/connected/applib/inc/player/still_decode/AppLibPlayer_Still_Single.h
 *
 * Decode and display single still with zooming and shifting features
 *
 * History:
 *    2013/11/28 - [phcheng] Create file
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
 * @defgroup AppLibPlayer_Still_Single      AppLibPlayer_Still_Single
 * @brief Decode and display an image with zooming and shifting features.
 *
 * Two steps of displaying an image.                        \n
 * 1. Load.                                                 \n
 *    Read a file and decode it.                            \n
 * 2. Show.                                                 \n
 *    Display a decoded image (or a part of it) on screen.  \n
 */

/**
 * @addtogroup AppLibPlayer_Still_Single
 * @ingroup StillDecode
 * @{
 */

#ifndef APPLIB_STILL_SINGLE_H_
#define APPLIB_STILL_SINGLE_H_

#include <applib.h>
#include <player/Decode.h>
#include <player/VideoDec.h>
#include <cfs/AmpCfs.h>
#include <player/decode_utility/ApplibPlayer_Common.h>

/**
 * Information to load a single still file.
 */
typedef struct _APPLIB_STILL_FILE_s_{
    /**
     * Filename. Full path of an image.
     */
#ifdef CONFIG_APP_ARD
    char Filename[MAX_FILENAME_LENGTH];
#else
    WCHAR Filename[MAX_FILENAME_LENGTH];
#endif
    /**
     * The decode source of file
     * 0: fullview
     * 1: thumbnail
     * 2: screennail
     * Comparison of resolution: fullview > screennail > thumbnail
     * Comparison of decode speed: thumbnail > screennail > fullview
     */
    UINT32 FileSource;
    /**
     * [Output] An identifier as an input to "AppLibStillSingle_Show()" to wait until the request is complete.
     */
    UINT32 *OutputWaitEventID;
    /**
     * Callback function that is invoked right before dealing with the first feed message of a specific channel.
     */
    int (*FeedBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last feed message of a specific channel.
     */
    int (*FeedEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right before dealing with the first decode message of a specific channel.
     */
    int (*DecodeBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last decode message of a specific channel.
     */
    int (*DecodeEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after finishing all the feed and decode messages.
     */
    int (*LoadEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
} APPLIB_STILL_FILE_s;

/**
 * Information to display an image on-screen.
 */
typedef struct _APPLIB_STILL_SINGLE_s_{
    /**
     * [Input] Displayed area (of digital channel) on screen.
     * The area is defined in a unique coordinate system which divides width and height of the screen into 10000 pieces.
     */
    AMP_AREA_s AreaDchanDisplayMain;
    /**
     * [Input] PIP (Picture In Picture) area of digital channel. Typically shows up when an image is enlarged.
     * PIP is a small window illustrating the entire image, with a frame indicating the displayed area relative to the entire image.
     * PIP is always in front of the displayed image. If you don't want to show PIP area, simply set AreaPIP.Width = 0 or AreaPIP.Height = 0.
     * The area is defined in a unique coordinate system which divides width and height of the screen into 10000 pieces.
     */
    AMP_AREA_s AreaDchanPIP;
    /**
     * [Input] Displayed area (of full channel) on screen.
     * The area is defined in a unique coordinate system which divides width and height of the screen into 10000 pieces.
     */
    AMP_AREA_s AreaFchanDisplayMain;
    /**
     * [Input] PIP (Picture In Picture) area of full channel. Typically shows up when an image is enlarged.
     * PIP is a small window illustrating the entire image, with a frame indicating the displayed area relative to the entire image.
     * PIP is always in front of the displayed image. If you don't want to show PIP area, simply set AreaPIP.Width = 0 or AreaPIP.Height = 0.
     * The area is defined in a unique coordinate system which divides width and height of the screen into 10000 pieces.
     */
    AMP_AREA_s AreaFchanPIP;
    /**
     * [Input] Number of pixels (of the original image) to shift along X-axis.
     * Area of the image outside of the window will be cropped.
     * ImageShiftX < 0: Image move to the left
     * ImageShiftX = 0: Image is centered horizontally
     * ImageShiftX > 0: Image move to the right
     */
    INT32 ImageShiftX;
    /**
     * [Input] Number of pixels (of the original image) to shift along Y-axis.
     * Area of the image outside of the window will be cropped.
     * ImageShiftY < 0: Image move up
     * ImageShiftY = 0: Image is centered vertically
     * ImageShiftY > 0: Image move down
     */
    INT32 ImageShiftY;
    /**
     * [Input] Magnification Factor.
     * Percentage of the size of displayed image relative to the original one that is centered in and stretched to the window.
     * Area of the image outside of the window will be cropped.
     * MagFactor < 100: Image become larger
     * MagFactor = 100: Image is centered in and stretched to the window, as long as it's not shifted.
     * MagFactor > 100: Image become smaller
     * For instance, setting MagFactor = 200 results in an image twice as large as the original one.
     */
    UINT32 MagFactor;
    /**
     * [Input] Rotate and flip setting.
     */
    AMP_ROTATION_e ImageRotate;
    /**
     * [Output] The eventual number of pixels (of the original image) shifted along X-axis.
     * When adjustment are made (AutoAdjust = 1), there may be difference between "OutputRealImageShiftX" and "ImageShiftX".
     * Otherwise, "OutputRealImageShiftX" and "ImageShiftX" are equal.
     */
    INT32 OutputRealImageShiftX;
    /**
     * [Output] The eventual number of pixels (of the original image) shifted along Y-axis.
     * When adjustment are made (AutoAdjust = 1), there may be difference between "OutputRealImageShiftY" and "ImageShiftY".
     * Otherwise, "OutputRealImageShiftY" and "ImageShiftY" are equal.
     */
    INT32 OutputRealImageShiftY;
    /**
     * [Output] An identifier as an input to "AppLibStillSingle_Show()" to wait until the request is complete.
     */
    UINT32 *OutputWaitEventID;
    /**
     * Callback function that is invoked right before dealing with the first rescale message of a specific channel.
     */
    int (*RescaleBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last rescale message of a specific channel.
     */
    int (*RescaleEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right before dealing with the first rescale message of all channels.
     */
    int (*RescaleAllChanBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last rescale message of all channels.
     */
    int (*RescaleAllChanEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right before dealing with the first display message of a specific channel.
     */
    int (*DisplayBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last display message of a specific channel (and waiting for DSP callback).
     */
    int (*DisplayWaitCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after receiving the DSP callback that the display request is done.
     */
    int (*DisplayEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right before dealing with the first display message of all channels.
     */
    int (*DisplayAllChanBeginCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after dealing with the last display message of all channels (and waiting for DSP callback).
     */
    int (*DisplayAllChanWaitCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after receiving the last DSP callback of all channels that the display request is done.
     */
    int (*DisplayAllChanEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
    /**
     * Callback function that is invoked right after finishing all the rescale and display messages.
     */
    int (*ShowEndCB)(AMP_STLDEC_HDLR_s *Hdlr, UINT32 EventID, void *Info);
} APPLIB_STILL_SINGLE_s;

/**
 * Initialize single still decoder which support \n
 * displaying, zooming and shifting a still in a window.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_Init(void);

/**
 * Load single still file.\n
 * A non-blocking function.
 *
 * @param [in] StillFile        Information to decode a file
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_Load(APPLIB_STILL_FILE_s *StillFile);

/**
 * Wait until the request with WaitEventID is complete.
 *
 * @param [in] WaitEventID      An identifier connected with the request
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_WaitLoad(UINT32 WaitEventID);

/**
 * Show single still on window.\n
 * A non-blocking function.
 *
 * @param [in] StillInfo        Information to display an image, such as location, size etc.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_Show(APPLIB_STILL_SINGLE_s *StillInfo);

/**
 * Wait until the request connected with the ID is complete.
 *
 * @param [in] WaitEventID      An identifier connected with the request.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_WaitShow(UINT32 WaitEventID);

/**
 * Show black on screen.\n
 * A non-blocking function.
 *
 * @param [in] OutputWaitEventID
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_ClearScreen(UINT32 *OutputWaitEventID);

/**
 * Wait until the request connected with the ID is complete.
 *
 * @param [in] WaitEventID      An identifier connected with the request.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_WaitClearScreen(UINT32 WaitEventID);

/**
 * Stop single still decoder.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_Stop(void);

/**
 * Stop single still decoder, release all the resources.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibStillSingle_Deinit(void);

#ifdef CONFIG_APP_ARD
/**
 * config single still decoder vout ar,
 * config_param:1 for lcd var, 0 will use default var
 * @return 0 - OK, others - Error
 */

extern void AppLibStillSingle_VoutAr_Config(int config_param);
#endif

#endif /* APPLIB_STILL_SINGLE_H_ */

/**
 * @}
 */     // End of group AppLibPlayer_Still_Single
