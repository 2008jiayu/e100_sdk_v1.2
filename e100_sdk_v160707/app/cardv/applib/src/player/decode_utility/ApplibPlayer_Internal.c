/**
 * @file src/app/connected/applib/src/player/decode_utility/ApplibPlayer_Internal.c
 *
 * Common functions used by player module in application library
 *
 * History:
 *    2013/12/04 - [phcheng] created file
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

#include <player/decode_utility/ApplibPlayer_Internal.h>

#define CONVERT_TO_FLOAT(x)     ((x) * 1.0)
//#define FLOAT_TO_INT32(x)     ((x) >= 0 ? (INT32)((x) + 0.5) : (INT32)((x) - 0.5))
//#define FLOAT_TO_UINT32(x)    ((x) > 0 ? (UINT32)((x) + 0.5) : 0) /** A more precise yet slower approach */
#define FLOAT_TO_UINT32(x)      ((UINT32)(x)) /** A faster yet less precise approach */

/**
 * Stretch, fit, and center the whole image in the window.
 *
 * @param [in] param                Display configuration
 * @param [in] isTransformation     The shape will transform after rotation
 */
static void Applib_DisplaySizeCal_Basic(
        APPLIB_DISP_SIZE_CAL_s *param,
        const UINT8 isTransformation)
{
    // The OBSERVED aspect ratio of the displayed image AFTER rotation. It may be different from
    // the aspect ratio of the image in Vout buffer, since pixels of screen may not be square.
    // "ar" is a fixed-point variable scaled by the macro "ASPECT_RATIO"
    UINT32 ar = 0;
    // The OBSERVED aspect ratio of the window. It may be different from the aspect ratio of the window
    // in Vout buffer for the same reason of "ar".
    // The window is a fraction of the screen, sometimes the entire screen, in which the image is displayed.
    // "war" is a fixed-point variable scaled by the macro "ASPECT_RATIO"
    UINT32 war = 0;

    // Step 1-1: Get war, the observed aspect ratio of the window.
    war = ASPECT_RATIO(GET_VAR_Y(param->DeviceAr)*param->WindowHeight*param->DeviceWidth/param->DeviceHeight,
                        GET_VAR_X(param->DeviceAr)*param->WindowWidth);

    // Step 1-2: Get ar, the observed aspect ratio of the displayed image AFTER rotation.
    if (param->ImageAr != VAR_ANY) {
        if (isTransformation != 0)
            ar = ASPECT_RATIO_INVERSE(param->ImageAr); // Swap height and width
        else
            ar = param->ImageAr;
    } else if ( (param->ImageWidth != 0) && (param->ImageHeight != 0) ) {
        // When aspect ratio of the image is not defined, generate aspect ratio from ImageWidth and ImageHeight instead.
        // The value may not be correct since pixels in YUV buffer are not necessarily square.
        if (isTransformation != 0)
            ar = ASPECT_RATIO(param->ImageWidth, param->ImageHeight); // Swap height and width
        else
            ar = ASPECT_RATIO(param->ImageHeight, param->ImageWidth);
    } else {
        // The aspect ratio of the image is not determined. Use "war" as default.
        ar = war;
    }

    // Step 1-3: Resize the image to fit in the window, and center it horizontally and vertically.
    // Determine the location and size of the image, when the magnitudes are measure in pixels of the screen.
    // Note that the location is represented by coordinate relative to the upper-left corner of the window.
    if (war == ar) {
        // Case 1: Aspect ratios of image and window are equal.
        // The image fits entirely in the window.
        param->OutputWidth = param->WindowWidth;
        param->OutputHeight = param->WindowHeight;
        param->OutputOffsetX = 0;
        param->OutputOffsetY = 0;
    } else if (war > ar) {
        // Case 2: Aspect ratio of window is larger than that of image.
        // Widths of image and window are equal. The image is vertically centered.
        // Be aware that ar may be up to about 50 (panorama for example) scaled by a factor of 65536.
        // As a result, overflow may occur if we didn't convert WindowHeight to float
        param->OutputWidth = param->WindowWidth;
        param->OutputHeight = (UINT32) ((CONVERT_TO_FLOAT(param->WindowHeight) * ar) / war);
        param->OutputOffsetX = 0;
        param->OutputOffsetY = (param->WindowHeight - param->OutputHeight) >> 1; // Replace "/ 2" by ">> 1"
    } else {
        // Case 3: Aspect ratio of window is smaller than that of image.
        // Heights of image and window are equal. The image is horizontally centered.
        // Be aware that war may be up to about 1000 scaled by a factor of 65536.
        // As a result, overflow may occur if we didn't convert WindowWidth to float
        param->OutputWidth = (UINT32) ((CONVERT_TO_FLOAT(param->WindowWidth) * war) / ar);
        param->OutputHeight = param->WindowHeight;
        param->OutputOffsetX = (param->WindowWidth - param->OutputWidth) >> 1; // Replace "/ 2" by ">> 1"
        param->OutputOffsetY = 0;
    }

    // Step 1-4: Set AOI of image source
    // Show entire image
    param->OutputSrcImgOffsetX = 0;
    param->OutputSrcImgOffsetY = 0;
    param->OutputSrcImgWidth = param->ImageWidth;
    param->OutputSrcImgHeight = param->ImageHeight;

    // Step 1-5: Set image shift
    // Currently there's no shift
    param->OutputRealImageShiftX = 0;
    param->OutputRealImageShiftY = 0;
}

/**
 * When auto adjustment is applied, center the image when certain conditions are met.
 *
 * @param [out] outputShiftX        Shift distance on X-axis
 * @param [out] outputShiftY        Shift distance on Y-axis
 * @param [in] outputWidth          Image width after zooming and before trimming by window
 * @param [in] outputHeight         Image height after zooming and before trimming by window
 * @param [in] windowWidth          Window width
 * @param [in] windowHeight         Window height
 */
static void Applib_DisplaySizeCal_AutoCenter(
        INT32* outputShiftX,
        INT32* outputShiftY,
        const INT32 outputWidth,
        const INT32 outputHeight,
        const INT32 windowWidth,
        const INT32 windowHeight)
{
    // Case 1: Imgae width  < Window width.  The image should be centered horizontally.
    // Case 2: Imgae height < Window height. The image should be centered vertically.
    if (outputWidth < windowWidth) {
            (*outputShiftX) = 0; // Center horizontally
    }
    if (outputHeight < windowHeight) {
            (*outputShiftY) = 0; // Center vertically
    }
}

/**
 * When auto adjustment is applied, adjust the position of the image
 *
 * @param [out] outputOffsetX       Image offset in the window on X-axis
 * @param [out] outputOffsetY       Image offset in the window on Y-axis
 * @param [in] outputWidth          Image width after zooming and before trimming by window
 * @param [in] outputHeight         Image height after zooming and before trimming by window
 * @param [in] windowWidth          Window width
 * @param [in] windowHeight         Window height
 */
static void Applib_DisplaySizeCal_AutoBoundary(
        INT32* outputOffsetX,
        INT32* outputOffsetY,
        const INT32 outputWidth,
        const INT32 outputHeight,
        const INT32 windowWidth,
        const INT32 windowHeight)
{
    // Declare variables for auto adjustment
    INT32 rightEdgeDistance = 0;
    INT32 leftEdgeDistance = 0;
    INT32 bottomEdgeDistance = 0;
    INT32 topEdgeDistance = 0;

    rightEdgeDistance = (*outputOffsetX) + outputWidth - windowWidth;
    leftEdgeDistance = (*outputOffsetX);
    bottomEdgeDistance = (*outputOffsetY) + outputHeight - windowHeight;
    topEdgeDistance = (*outputOffsetY);
    // Restrict the space between image and window
    // Case 1: Imgae width  >= Window width.  There should be no left margin and right maigin.
    // Case 2: Imgae width  <  Window width.  The image should not overstep left edge and right edge of the window.
    // Case 3: Imgae height >= Window height. There should be no top margin and bottom maigin.
    // Case 4: Imgae height <  Window height. The image should not overstep top and bottom of the window.
        if (rightEdgeDistance > 0 && leftEdgeDistance > 0) {
            (*outputOffsetX) -= MIN(rightEdgeDistance, leftEdgeDistance);
        }
        else if (rightEdgeDistance < 0 && leftEdgeDistance < 0) {
            (*outputOffsetX) -= MAX(rightEdgeDistance, leftEdgeDistance);
        }

        if (bottomEdgeDistance > 0 && topEdgeDistance > 0) {
            (*outputOffsetY) -= MIN(bottomEdgeDistance, topEdgeDistance);
        }
        else if (bottomEdgeDistance < 0 && topEdgeDistance < 0) {
            (*outputOffsetY) -= MAX(bottomEdgeDistance, topEdgeDistance);
        }
}

/**
 * Calculate the area (to display image) in the window
 *
 * @param [out] param               Display configuration
 * @param [in] outputOffsetX        Image offset in the window on X-axis
 * @param [in] outputOffsetY        Image offset in the window on Y-axis
 * @param [in] outputWidth          Image width after zooming and before cropping by window
 * @param [in] outputHeight         Image height after zooming and before cropping by window
 * @param [in] windowWidth          Window width
 * @param [in] windowHeight         Window height
 */
static void Applib_DisplaySizeCal_WindowAoi(
        APPLIB_DISP_SIZE_CAL_s *param,
        const INT32 outputOffsetX,
        const INT32 outputOffsetY,
        const INT32 outputWidth,
        const INT32 outputHeight,
        const INT32 windowWidth,
        const INT32 windowHeight)
{
    param->OutputOffsetX = (UINT32) ( MAX(outputOffsetX, 0) );
    param->OutputOffsetY = (UINT32) ( MAX(outputOffsetY, 0) );
    param->OutputWidth   = (UINT32) ( MIN(windowWidth - outputOffsetX, outputWidth)     // Trim right side of the image outside of the window
                                    + MIN(outputOffsetX, 0) );                          // Trim left side of the image outside of the window
    param->OutputHeight  = (UINT32) ( MIN(windowHeight - outputOffsetY, outputHeight)   // Trim bottom of the image outside of the window
                                    + MIN(outputOffsetY, 0) );                          // Trim top of the image outside of the window
}

/**
 * Calculate the AOI (Area Of Interest) of image source.\n
 * Remove the area of image outside of the window.\n
 * When the image is cropped, the AOI of the corresponding image source (in YUV buffer) has to be reconfigured.\n
 * Offset of the AOI in YUV buffer. 0 <= X < ImageWidth, 0 <= Y < ImageHeight.\n
 * Width and height of the AOI in YUV buffer. 0 <= Width <= imageWidth, 0 <= Height <= imageHeight.
 *
 * @param [out] param               Display configuration
 * @param [in] outputOffsetX        Image offset in the window on X-axis
 * @param [in] outputOffsetY        Image offset in the window on Y-axis
 * @param [in] outputWidth          Image width after zooming and before cropping by window
 * @param [in] outputHeight         Image height after zooming and before cropping by window
 * @param [in] windowWidth          Window width
 * @param [in] windowHeight         Window height
 * @param [in] imageWidth           Image source buffer width
 * @param [in] imageHeight          Image source buffer height
 */
static void Applib_DisplaySizeCal_ImageAoi(
        APPLIB_DISP_SIZE_CAL_s *param,
        const INT32 outputOffsetX,
        const INT32 outputOffsetY,
        const INT32 outputWidth,
        const INT32 outputHeight,
        const INT32 windowWidth,
        const INT32 windowHeight,
        const INT32 imageWidth,
        const INT32 imageHeight)
{
#if 0
    /** Calculate by integer */
    // "distX1" is the distance (before trimming) form image left edge to window left edge. The sign implies direction.
    // No need to check the unwanted case when (distanceX1 >= outputWidth) which is ruled out in Step 3-0
    const INT32 distX1 = MAX( (-outputOffsetX), 0 );
    // "distX2" is the distance (before trimming) form window right edge to image right edge The sign implies direction.
    // No need to check the unwanted case when (distanceX2 >= outputWidth) which is ruled out in Step 3-0
    const INT32 distX2 = MAX( (outputWidth + outputOffsetX - windowWidth), 0 );
    // "distY1" is the distance (before trimming) form image top to window top. The sign implies direction.
    // No need to check the unwanted case when (distanceY1 >= outputHeight) which is ruled out in Step 3-0
    const INT32 distY1 = MAX( (-outputOffsetY), 0 );
    // "distY2" is the distance (before trimming) form window bottom to image bottom. The sign implies direction.
    // No need to check the unwanted case when (distanceY2 >= outputHeight) which is ruled out in Step 3-0
    const INT32 distY2 = MAX( (outputHeight + outputOffsetY - windowHeight), 0 );
    // The width of the area in buffer that is to displayed when the image rotate 0 or 180 degree
    const INT32 srcW1 = ((INT32)param->OutputWidth)  * imageWidth  / outputWidth;
    // The width of the area in buffer that is to displayed when the image rotate 90 or 270 degree
    const INT32 srcW2 = ((INT32)param->OutputHeight) * imageWidth  / outputHeight;
    // The height of the area in buffer that is to displayed when the image rotate 0 or 180 degree
    const INT32 srcH1 = ((INT32)param->OutputHeight) * imageHeight / outputHeight;
    // The height of the area in buffer that is to displayed when the image rotate 90 or 270 degree
    const INT32 srcH2 = ((INT32)param->OutputWidth)  * imageHeight / outputWidth;
    switch (param->ImageRotate) {
        case AMP_ROTATE_0:                  /* No rotation */
            param->OutputSrcImgOffsetX = (UINT32) ( distX1 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = (UINT32) ( distY1 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = (UINT32) ( srcW1 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH1 );
            break;
        case AMP_ROTATE_0_HORZ_FLIP:        /* No rotation and horizontal flip */
            param->OutputSrcImgOffsetX = (UINT32) ( distX2 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = (UINT32) ( distY1 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = (UINT32) ( srcW1 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH1 );
            break;
        case AMP_ROTATE_90:                 /* Clockwise 90 degree */
            param->OutputSrcImgOffsetX = (UINT32) ( distY1 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = (UINT32) ( distX2 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = (UINT32) ( srcW2 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH2 );
            break;
        case AMP_ROTATE_90_VERT_FLIP:       /* Clockwise 90 degree and vertical flip*/
            param->OutputSrcImgOffsetX = (UINT32) ( distY2 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = (UINT32) ( distX2 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = (UINT32) ( srcW2 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH2 );
            break;
        case AMP_ROTATE_180:                /* Clockwise 180 degree */
            param->OutputSrcImgOffsetX = (UINT32) ( distX2 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = (UINT32) ( distY2 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = (UINT32) ( srcW1 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH1 );
            break;
        case AMP_ROTATE_180_HORZ_FLIP:      /* Clockwise 180 degree and horizontal flip */
            param->OutputSrcImgOffsetX = (UINT32) ( distX1 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = (UINT32) ( distY2 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = (UINT32) ( srcW1 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH1 );
            break;
        case AMP_ROTATE_270:                /* Clockwise 270 degree */
            param->OutputSrcImgOffsetX = (UINT32) ( distY2 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = (UINT32) ( distX1 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = (UINT32) ( srcW2 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH2 );
            break;
        case AMP_ROTATE_270_VERT_FLIP:      /* Clockwise 270 degree and vertical flip */
            param->OutputSrcImgOffsetX = (UINT32) ( distY1 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = (UINT32) ( distX1 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = (UINT32) ( srcW2 );
            param->OutputSrcImgHeight  = (UINT32) ( srcH2 );
            break;
        default:
            // Do nothing. This situation has been excluded in the beginning
            break;
    }
#else
    /** Calculate by floating point */
    // "distX1" is the distance (before trimming) form image left edge to window left edge. The sign implies direction.
    // No need to check the unwanted case when (distanceX1 >= outputWidth) which is ruled out in Step 3-0
    const double distX1 = MAX( (-outputOffsetX), 0 );
    // "distX2" is the distance (before trimming) form window right edge to image right edge The sign implies direction.
    // No need to check the unwanted case when (distanceX2 >= outputWidth) which is ruled out in Step 3-0
    const double distX2 = MAX( (outputWidth + outputOffsetX - windowWidth), 0 );
    // "distY1" is the distance (before trimming) form image top to window top. The sign implies direction.
    // No need to check the unwanted case when (distanceY1 >= outputHeight) which is ruled out in Step 3-0
    const double distY1 = MAX( (-outputOffsetY), 0 );
    // "distY2" is the distance (before trimming) form window bottom to image bottom. The sign implies direction.
    // No need to check the unwanted case when (distanceY2 >= outputHeight) which is ruled out in Step 3-0
    const double distY2 = MAX( (outputHeight + outputOffsetY - windowHeight), 0 );
    // The width of the area in buffer that is to displayed when the image rotate 0 or 180 degree
    const double srcW1 = CONVERT_TO_FLOAT(param->OutputWidth)  * imageWidth  / outputWidth;
    // The width of the area in buffer that is to displayed when the image rotate 90 or 270 degree
    const double srcW2 = CONVERT_TO_FLOAT(param->OutputHeight) * imageWidth  / outputHeight;
    // The height of the area in buffer that is to displayed when the image rotate 0 or 180 degree
    const double srcH1 = CONVERT_TO_FLOAT(param->OutputHeight) * imageHeight / outputHeight;
    // The height of the area in buffer that is to displayed when the image rotate 90 or 270 degree
    const double srcH2 = CONVERT_TO_FLOAT(param->OutputWidth)  * imageHeight / outputWidth;
    switch (param->ImageRotate) {
        case AMP_ROTATE_0:                  /* No rotation */
            param->OutputSrcImgOffsetX = ( distX1 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = ( distY1 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = ( srcW1 );
            param->OutputSrcImgHeight  = ( srcH1 );
            break;
        case AMP_ROTATE_0_HORZ_FLIP:        /* No rotation and horizontal flip */
            param->OutputSrcImgOffsetX = ( distX2 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = ( distY1 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = ( srcW1 );
            param->OutputSrcImgHeight  = ( srcH1 );
            break;
        case AMP_ROTATE_90:                 /* Clockwise 90 degree */
            param->OutputSrcImgOffsetX = ( distY1 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = ( distX2 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = ( srcW2 );
            param->OutputSrcImgHeight  = ( srcH2 );
            break;
        case AMP_ROTATE_90_VERT_FLIP:       /* Clockwise 90 degree and vertical flip*/
            param->OutputSrcImgOffsetX = ( distY2 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = ( distX2 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = ( srcW2 );
            param->OutputSrcImgHeight  = ( srcH2 );
            break;
        case AMP_ROTATE_180:                /* Clockwise 180 degree */
            param->OutputSrcImgOffsetX = ( distX2 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = ( distY2 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = ( srcW1 );
            param->OutputSrcImgHeight  = ( srcH1 );
            break;
        case AMP_ROTATE_180_HORZ_FLIP:      /* Clockwise 180 degree and horizontal flip */
            param->OutputSrcImgOffsetX = ( distX1 * imageWidth  / outputWidth  );
            param->OutputSrcImgOffsetY = ( distY2 * imageHeight / outputHeight );
            param->OutputSrcImgWidth   = ( srcW1 );
            param->OutputSrcImgHeight  = ( srcH1 );
            break;
        case AMP_ROTATE_270:                /* Clockwise 270 degree */
            param->OutputSrcImgOffsetX = ( distY2 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = ( distX1 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = ( srcW2 );
            param->OutputSrcImgHeight  = ( srcH2 );
            break;
        case AMP_ROTATE_270_VERT_FLIP:      /* Clockwise 270 degree and vertical flip */
            param->OutputSrcImgOffsetX = ( distY1 * imageWidth  / outputHeight );
            param->OutputSrcImgOffsetY = ( distX1 * imageHeight / outputWidth  );
            param->OutputSrcImgWidth   = ( srcW2 );
            param->OutputSrcImgHeight  = ( srcH2 );
            break;
        default:
            // Do nothing. This situation has been excluded in the beginning
            break;
    }
#endif
}

/**
 * Zoom and shift the image. Afterwards, trim the image and return the results.
 *
 * @param [in] param                Display configuration
 * @param [in] isTransformation     The shape will transform after rotation
 *
 * @return 0 - OK, others - Error
 */
static int Applib_DisplaySizeCal_Advanced(
        APPLIB_DISP_SIZE_CAL_s *param,
        const UINT8 isTransformation)
{
    // We have to deal with negative numbers during calculation
    INT32 outputOffsetX = (INT32)param->OutputOffsetX;
    INT32 outputOffsetY = (INT32)param->OutputOffsetY;
    INT32 outputWidth = (INT32)param->OutputWidth;
    INT32 outputHeight = (INT32)param->OutputHeight;
    // Declare variables for shifting and zooming image
    const INT32 outputWidthHalf = outputWidth >> 1; // Replace "/ 2" by ">> 1"
    const INT32 outputHeightHalf = outputHeight >> 1; // Replace "/ 2" by ">> 1"
    const INT32 windowWidth = (INT32)param->WindowWidth;
    const INT32 windowHeight = (INT32)param->WindowHeight;
    const INT32 MagFactor = (INT32)param->MagFactor; // Warning: Extremely high/low MagFactor may cause computational inaccuracy.
    const INT32 imageWidth = (INT32)param->ImageWidth; // Image width in YUV buffer.
    const INT32 imageHeight = (INT32)param->ImageHeight; // Image height in YUV buffer.
    INT32 outputShiftX = 0;
    INT32 outputShiftY = 0;

    /**
     * Part 2: Advanced display.
     * The "standardized image" from previous step is now magnified and shifted.
     * Conceptually, we shift the image first, and then magnify it from the new center.
     */
    // Step 2-0: Check necessary informations.
    if ((param->MagFactor == MAGNIFICATION_FACTOR_BASE) &&
        (param->ImageShiftX == 0) &&
        (param->ImageShiftY == 0)) {
        // If there's no zooming and shifting, do nothing and return.
        return 0; // Not an error
    }
    if ((param->OutputWidth == 0) || (param->OutputHeight == 0)) {
        // This is a situation that may happen if we assign extreme but legal values to "ar" or "war".
        // The OutputHeight or OutputWidth will be too small to show up.
        // For example, assuming DeviceWidth = 1920, DeviceHeight = 1080, DeviceAr = VAR_16x9, ar = (0.4 << 16) = 26214,
        // WindowHeight = 1000, WindowWidth = 2, we can get war = (500 << 16) = 32768000.
        // Since war > ar in this case, compute OutputHeight = (UINT32)1000.0*26214/32768000 = (UINT32)0.799988 = 0
        // Note that even if the image won't show, this situation is NOT an error.
        // TODO: If the image from the previous example is magnified 10 times larger, we should not return here.
        return 0; // Not an error
    }
    if ((param->ImageWidth == 0) || (param->ImageHeight == 0)) {
        // This is a situation that ImageAr is given but ImageWidth or ImageHeight is not.
        // Width and height of the image soure are needed to calculate AOI in the buffer and shift distance on screen.
        // However, given ImageAr we can still display the whole image. Hence, this is NOT an error but we should return here.
        AmbaPrint("[Applib - StillDec] <DisplaySizeCal> No zoom and shift applied. ImageWidth and ImageHeight are needed.");
        return 0; // Not an error
    }

    // TODO: We do the following calculations based on the assumption that the image source in YUV buffer has exactly
    //       the same size as the original image. If the image source was rescaled form the original image without
    //       retaining original size, further informations would be needed to get the right answer. In other words,
    //       We need enough informations to convert locations between the image source and the original image.

    // Step 2-1: Use INT32 to store temporary results
    if (isTransformation != 0) {
        outputShiftX = param->ImageShiftX * outputHeight / imageHeight;
        outputShiftY = param->ImageShiftY * outputWidth / imageWidth;
    }
    else {
        outputShiftX = param->ImageShiftX * outputWidth / imageWidth;
        outputShiftY = param->ImageShiftY * outputHeight / imageHeight;
    }

//#define DEBUG_STILL_SHOW
#ifdef DEBUG_STILL_SHOW
INT32 centerX = imageWidth/2-param->ImageShiftX;
INT32 centerY = imageHeight/2-param->ImageShiftY;
AmbaPrintColor(RED,"[Start] centerX=%d,centerY=%d",centerX,centerY);
#endif
    // Step 2-2: Resize and shift the image
    // Shift and magnify the "standardized image" based on magnification factor.
    // Conceptually, we shift the image with its distance measured by number of original pixels of the image,
    // and afterwards we enlarge the image from the center of the WINDOW (not the image).
    outputWidth = outputWidth * MagFactor / MAGNIFICATION_FACTOR_BASE;
    outputHeight = outputHeight * MagFactor / MAGNIFICATION_FACTOR_BASE;
    if (param->AutoAdjust > 0) {
        Applib_DisplaySizeCal_AutoCenter(
            &outputShiftX, &outputShiftY,
            outputWidth, outputHeight,
            windowWidth, windowHeight);
    }
    // Calculate offsets after shifting and zooming
    outputOffsetX += outputWidthHalf + (outputShiftX - outputWidthHalf) * MagFactor / MAGNIFICATION_FACTOR_BASE;
    outputOffsetY += outputHeightHalf + (outputShiftY  - outputHeightHalf) * MagFactor / MAGNIFICATION_FACTOR_BASE;
#ifdef DEBUG_STILL_SHOW
AmbaPrintColor(RED,"OffsetX=%d,OffsetY=%d,OutputWidth=%d,OutputHeight=%d",outputOffsetX,outputOffsetY,outputWidth,outputHeight);
#endif

    // Step 2-3: When auto adjustment is applied, adjust the position of the image
    // Set output value in "outputOffsetX", "outputOffsetY"
    if (param->AutoAdjust > 0) {
        Applib_DisplaySizeCal_AutoBoundary(
            &outputOffsetX, &outputOffsetY,
            outputWidth, outputHeight,
            windowWidth, windowHeight);
    }

    /**
     * Part 3: Trim image and return results.
     * After magnification and shifting, trim the area outside of the window.
     */
    // Step 3-0: Check if the image "disappeared"
    // If the image moved entirely out of the window, retuen a 0-sized image to make it invisable.
    if ((outputOffsetX >= windowWidth) ||       // Image moved beyond the right edge of the window
        (outputOffsetX + outputWidth <= 0) ||   // Image moved beyond the left edge of the window
        (outputOffsetY >= windowHeight) ||      // Image moved beyond the bottom of the window
        (outputOffsetY + outputHeight <= 0)) {  // Image moved beyond the top of the window
        param->OutputWidth = 0;
        param->OutputHeight = 0;
        param->OutputOffsetX = 0;
        param->OutputOffsetY = 0;
        return 0;
    }

    // Step 3-1: Calculate the area (to display image) in the window
    // Set output value in "param->OutputOffsetX", "param->OutputOffsetY", "param->OutputWidth", "param->OutputHeight"
    Applib_DisplaySizeCal_WindowAoi(param,
            outputOffsetX, outputOffsetY,
            outputWidth, outputHeight,
            windowWidth, windowHeight);
#ifdef DEBUG_STILL_SHOW
AmbaPrintColor(RED,"OffsetX=%d,OffsetY=%d,OutputWidth=%d,OutputHeight=%d",param->OutputOffsetX,param->OutputOffsetY,param->OutputWidth,param->OutputHeight);
#endif

    // Step 3-2: Calculate the AOI (Area Of Interest) of image source
    // Set output value in "param->OutputSrcImgOffsetX", "param->OutputSrcImgOffsetY", "param->OutputSrcImgWidth", "param->OutputSrcImgHeight"
    Applib_DisplaySizeCal_ImageAoi(param,
            outputOffsetX, outputOffsetY,
            outputWidth, outputHeight,
            windowWidth, windowHeight,
            imageWidth, imageHeight);
#ifdef DEBUG_STILL_SHOW
AmbaPrintColor(RED,"SrcImgX=%d,SrcImgY=%d,SrcImgWidth=%d,SrcImgHeight=%d",param->OutputSrcImgOffsetX,param->OutputSrcImgOffsetY,param->OutputSrcImgWidth,param->OutputSrcImgHeight);
centerX = (windowWidth/2-param->OutputOffsetX)*param->OutputSrcImgWidth/param->OutputWidth+param->OutputSrcImgOffsetX;
centerY = (windowHeight/2-param->OutputOffsetY)*param->OutputSrcImgHeight/param->OutputHeight+param->OutputSrcImgOffsetY;
AmbaPrintColor(RED,"[End] centerX=%d,centerY=%d",centerX,centerY);
#endif

    // Step 3-3: Return distance shifted eventually
    // Since shift may be changed during auto adjustment, derive it by calculating distance between center of image and center of window.
    outputShiftX = outputOffsetX + ((outputWidth - windowWidth) >> 1); // Replace "/ 2" by ">> 1"
    outputShiftY = outputOffsetY + ((outputHeight - windowHeight) >> 1); // Replace "/ 2" by ">> 1"
    param->OutputRealImageShiftX = outputShiftX * imageWidth / outputWidth;
    param->OutputRealImageShiftY = outputShiftY * imageHeight / outputHeight;

    return 0; // Success
}

int Applib_DisplaySizeCal(APPLIB_DISP_SIZE_CAL_s *param)
{
    // Whether the outline of image will change afer rotation.
    // 0: The outline won't change, 1: The outline will change
    UINT8 isTransformation = 0;

    /**
     * Part 1: Basic display.
     * Get a "standardized image" which is stretched, fitted, and centered in \n
     * the window without changing the original aspect ratio.
     */
    // Whether the shape of the image change after rotation.
    // It happends when the angle of rotation is either 90 or 270 degrees.
    switch (param->ImageRotate) {
        case AMP_ROTATE_0:                  /* No rotation */
        case AMP_ROTATE_0_HORZ_FLIP:        /* No rotation and horizontal flip */
        case AMP_ROTATE_180:                /* Clockwise 180 degree */
        case AMP_ROTATE_180_HORZ_FLIP:      /* Clockwise 180 degree and horizontal flip */
            isTransformation = 0;
            break;
        case AMP_ROTATE_90:                 /* Clockwise 90 degree */
        case AMP_ROTATE_90_VERT_FLIP:       /* Clockwise 90 degree and vertical flip*/
        case AMP_ROTATE_270:                /* Clockwise 270 degree */
        case AMP_ROTATE_270_VERT_FLIP:      /* Clockwise 270 degree and vertical flip */
            isTransformation = 1;
            break;
        default:
            AmbaPrint("[Applib - StillDec] <DisplaySizeCal> Cannot recognize the rotate setting \"%d\".", param->ImageRotate);
            return -1; // Error
    }

    // Stretch, fit, and center the whole image in the window.
    // Set output value in "param"
    Applib_DisplaySizeCal_Basic(param, isTransformation);

    /**
     * Part 2: Zoom and shift the image.\n
     * Part 3: Trim image and return results.
     */
    // Set output value in "param"
    return Applib_DisplaySizeCal_Advanced(param, isTransformation);
}

int Applib_DisplaySizeCal_MultiChannel(
        APPLIB_DISP_SIZE_CAL_ARRAY_s *param,
        const INT32 InputShiftX,
        const INT32 InputShiftY,
        INT32 *OutputShiftX,
        INT32 *OutputShiftY)
{
    int i = 0; // Counter
    // Set initial return value
    *OutputShiftX = InputShiftX;
    *OutputShiftY = InputShiftY;
    // Calculate the area in buffer to be displayed. The eventual shift of LCD and TV should be equal
    for (i = 0; i < DISP_CH_NUM; ++i) {
        if (param->Cal[i] != NULL) { // Channel is available
            // Get previous shift
            param->Cal[i]->ImageShiftX = *OutputShiftX;
            param->Cal[i]->ImageShiftY = *OutputShiftY;
            Applib_DisplaySizeCal(param->Cal[i]);
            // Set current shift
            *OutputShiftX = param->Cal[i]->OutputRealImageShiftX;
            *OutputShiftY = param->Cal[i]->OutputRealImageShiftY;
        }
    }
    // If the shift of different channel are not consistent, calculate again.
    // Don't have to calculate the last element, since the absolute value of shift can only decrease after each calculation.
    for (i = 0; i < DISP_CH_NUM - 1; ++i) {
        if (param->Cal[i] != NULL) { // Channel is available
            if ((param->Cal[i]->ImageShiftX != *OutputShiftX) || (param->Cal[i]->ImageShiftY != *OutputShiftY)) {
                param->Cal[i]->ImageShiftX = *OutputShiftX;
                param->Cal[i]->ImageShiftY = *OutputShiftY;
                if (Applib_DisplaySizeCal(param->Cal[i]) != 0) {
                    return -1; // Error
                }
                // Don't have to set current shift, the shift won't change.
                // Theoretically, the shift won't change
                if ((*OutputShiftX != param->Cal[i]->ImageShiftX) ||
                    (*OutputShiftY != param->Cal[i]->ImageShiftY)) {
                    AmbaPrintColor(BLUE, "[Applib - StillDec] %s:%u Unexpected change of shift (%d, %d) -> (%d, %d)!",
                        __FUNCTION__, __LINE__, *OutputShiftX, *OutputShiftY, param->Cal[i]->ImageShiftX, param->Cal[i]->ImageShiftY);
                }
            }
        }
    }
    return 0;
}

static UINT8 Applib_AlphaBlendCal(const UINT8 X, const UINT8 Y, const UINT8 Alpha)
{
    UINT32 uX = X;
    UINT32 uY = Y;
    UINT32 uAlpha = Alpha;
    if (X > Y) {
        // Replace "/ 256" by ">> 8"
        return (UINT8) (uY + (((uX - uY + 1) * uAlpha) >> 8));
    }
    else if (X < Y) {
        return (UINT8) (uY - (((uY - uX + 1) * uAlpha) >> 8));
    }
    else { // X == Y
        return X;
    }
}

int Applib_Draw_Rectangle(
        const AMP_YUV_BUFFER_s *TargetBuffer,
        const APPLIB_DRAW_COLOR_s *RectColor)
{
    UINT32 x, y; // Counter representing offsets in the buffer
    UINT8 *lumaAddr; // Address in luma buffer
    UINT8 *chromaAddr, *prevChromaAddr; // Address in chroma buffer
    // Align X to avoid decode error. U and V value are stored alternately, so start address must be even.
    const UINT32 targetX = ALIGN_2(TargetBuffer->AOI.X);
    // Align Y in case of 4:2:0 format in which chroma values are shared by two rows.
    const UINT32 targetY = ALIGN_2(TargetBuffer->AOI.Y);
    // We have to align Width to a multiple of two to get full chroma info, since U and V value are stored alternately.
    // Use "& 0xFFFFFFFE" instead of "ALIGN_2", so as to avoid situation that X and Width are both increased by 1 (may cause overflow of buffer)
    const UINT32 targetWidth = (TargetBuffer->AOI.Width) & 0xFFFFFFFE;
    // Align Height in case of 4:2:0 format in which chroma values are shared by two rows.
    const UINT32 targetHeight = (TargetBuffer->AOI.Height) & 0xFFFFFFFE;
    const UINT8 colorY = RectColor->PenColorY;
    const UINT8 colorU = RectColor->PenColorU;
    const UINT8 colorV = RectColor->PenColorV;

#define MOD_2(x) (((UINT32)x) & 0x1)

    if (targetWidth == 0 || targetHeight == 0) {
        return -1; // Error: The size is too small to show
    }

    // Start luma address
    // We assume that LumaAddr and ChromaAddr are already aligned.
    lumaAddr = TargetBuffer->LumaAddr + targetX + (targetY * TargetBuffer->Pitch);
    // Start chroma address
    if (TargetBuffer->ColorFmt == AMP_YUV_422) { // 4:2:2 format
        chromaAddr = TargetBuffer->ChromaAddr + targetX + (targetY * TargetBuffer->Pitch);
    }
    else { // 4:2:0 format
        // "targetY" divided by 2. Replace "/ 2" by ">> 1"
        chromaAddr = TargetBuffer->ChromaAddr + targetX + ((targetY >> 1) * TargetBuffer->Pitch);
    }

    // Draw rectangle by setting colors for each row
    for (y = 0; y < targetHeight; ++y) {
        if (RectColor->PenColorAlpha == 255) { // No blending. All rows will have the same value
            // Set luma value
            memset(lumaAddr, colorY, targetWidth);
            // Set chroma value
            if (y == 0) { // The first row
                for (x = 0; x < targetWidth; ++x) { // U and V values are arranged alternately
                    // Note that chromaAddr is even
                    chromaAddr[x] = colorU;
                    ++x; // Don't have to check x < targetWidth, since targetWidth is even.
                    chromaAddr[x] = colorV;
                }
            }
            else { // Other rows. Copy the content of previous row
                if ((TargetBuffer->ColorFmt == AMP_YUV_422) ||                      // 4:2:2 format, calculate each row
                    (TargetBuffer->ColorFmt == AMP_YUV_420 && (MOD_2(y) == 0))) {   // 4:2:0 format, calculate rows with even "y"
                    memcpy(chromaAddr, prevChromaAddr, targetWidth);
                }
            }
        }
        else { // Blending. Cannot copy previous row.
            for (x = 0; x < targetWidth; ++x) {
                // Set luma value
                lumaAddr[x] = Applib_AlphaBlendCal(colorY, lumaAddr[x], RectColor->PenColorAlpha);
                // Set chroma value
                if ((TargetBuffer->ColorFmt == AMP_YUV_422) ||                      // 4:2:2 format, calculate each row
                    (TargetBuffer->ColorFmt == AMP_YUV_420 && (MOD_2(y) == 0))) {   // 4:2:0 format, calculate rows with even "y"
                    if (MOD_2(x) == 0) {
                        // x is even, hence chromaAddr[x] stores U value
                        chromaAddr[x] = Applib_AlphaBlendCal(colorU, chromaAddr[x], RectColor->PenColorAlpha);
                    }
                    else {
                        // x is odd, hence chromaAddr[x] stores V value
                        chromaAddr[x] = Applib_AlphaBlendCal(colorV, chromaAddr[x], RectColor->PenColorAlpha);
                    }
                }
            }
        }

        // Start chroma address of previous row.
        prevChromaAddr = chromaAddr;
        // Move to next row
        lumaAddr += TargetBuffer->Pitch;
        if (TargetBuffer->ColorFmt == AMP_YUV_422) { // 4:2:2 format
            chromaAddr += TargetBuffer->Pitch;
        }
        else if (y != (y & 0xFFFFFFFE)) { // 4:2:0 format, y is odd
            chromaAddr += TargetBuffer->Pitch;
        }
    }
    return 0; // Success
}

int Applib_Draw_Frame(APPLIB_DRAW_FRAME_CONFIG_s *param)
{
    // Align X to avoid decode error. U and V value are stored alternately, so start address must be even.
    const UINT32 targetX = ALIGN_2(param->TargetBuffer.AOI.X);
    // Align Y in case of 4:2:0 format in which chroma values are shared by two rows.
    const UINT32 targetY = ALIGN_2(param->TargetBuffer.AOI.Y);
    // We have to align Width to a multiple of two to get full chroma info, since U and V value are stored alternately.
    // Use "& 0xFFFFFFFE" instead of "ALIGN_2", so as to avoid situation that X and Width are both increased by 1 (may cause overflow of buffer)
    const UINT32 targetWidth = (param->TargetBuffer.AOI.Width) & 0xFFFFFFFE;
    // Align Height in case of 4:2:0 format in which chroma values are shared by two rows.
    const UINT32 targetHeight = (param->TargetBuffer.AOI.Height) & 0xFFFFFFFE;
    AMP_YUV_BUFFER_s targetBuffer = param->TargetBuffer;
    const APPLIB_DRAW_COLOR_s frameColor = param->FrameColor;
    const UINT32 Thickness = ALIGN_2(param->Thickness);

    if (targetWidth == 0 || targetHeight == 0) {
        AmbaPrint("[Applib - StillDec] <Draw> The frame is too small to draw!");
        return -1; // Error: The size is too small to show
    }
    if (((Thickness << 1) >= targetWidth) || // Replace "* 2" by "<< 1"
        ((Thickness << 1) >= targetHeight)) {
        // Frame is too thick and turn into a rectangle
        // Draw a rectangle instead
        return Applib_Draw_Rectangle(&targetBuffer, &frameColor);
    }

    // Draw top side of the frame
    targetBuffer.AOI.Height = Thickness;
    Applib_Draw_Rectangle(&targetBuffer, &frameColor);

    // Draw bottom side of the frame
    targetBuffer.AOI.Y = targetY + targetHeight - Thickness;
    Applib_Draw_Rectangle(&targetBuffer, &frameColor);

    // Draw left side of the frame
    targetBuffer.AOI.Y = targetY + Thickness;
    targetBuffer.AOI.Height = targetHeight - (Thickness << 1);
    targetBuffer.AOI.Width = Thickness;
    Applib_Draw_Rectangle(&targetBuffer, &frameColor);

    // Draw right side of the frame
    targetBuffer.AOI.X = targetX + targetWidth - Thickness;
    Applib_Draw_Rectangle(&targetBuffer, &frameColor);

    return 0; // Success
}

int Applib_PipFrameSizeCal(APPLIB_PIP_FRAME_CAL_s *param)
{
    const UINT32 IW = param->ImageSrcWidth;
    const UINT32 IH = param->ImageSrcHeight;
    const UINT32 PX = param->ImagePipAOI.X;
    const UINT32 PY = param->ImagePipAOI.Y;
    const UINT32 PW = param->ImagePipAOI.Width;
    const UINT32 PH = param->ImagePipAOI.Height;
    // "distX1" is the distance (in source buffer) between image AOI left edge and image left edge.
    const INT32 distX1 = param->ImageSrcAOI.X;
    // "distX2" is the distance (in source buffer) between image AOI right edge and image right edge.
    const INT32 distX2 = param->ImageSrcWidth - param->ImageSrcAOI.X - param->ImageSrcAOI.Width;
    // "distY1" is the distance (in source buffer) between image AOI top and image top.
    const INT32 distY1 = param->ImageSrcAOI.Y;
    // "distY2" is the distance (in source buffer) between image AOI bottom and image bottom.
    const INT32 distY2 = param->ImageSrcHeight - param->ImageSrcAOI.Y - param->ImageSrcAOI.Height;
    // The width of PIP frame when the image rotate 0 or 180 degree
    const INT32 FW1 = param->ImageSrcAOI.Width  * param->ImagePipAOI.Width  / param->ImageSrcWidth;
    // The width of PIP frame when the image rotate 90 or 270 degree
    const INT32 FW2 = param->ImageSrcAOI.Height * param->ImagePipAOI.Width  / param->ImageSrcHeight;
    // The height of PIP frame when the image rotate 0 or 180 degree
    const INT32 FH1 = param->ImageSrcAOI.Height * param->ImagePipAOI.Height / param->ImageSrcHeight;
    // The height of PIP frame when the image rotate 90 or 270 degree
    const INT32 FH2 = param->ImageSrcAOI.Width  * param->ImagePipAOI.Height / param->ImageSrcWidth;
    switch (param->ImageRotate) {
        case AMP_ROTATE_0:                  /* No rotation */
            param->OutputPipFrameAOI.X = PX + distX1 * PW / IW;
            param->OutputPipFrameAOI.Y = PY + distY1 * PH / IH;
            param->OutputPipFrameAOI.Width = FW1;
            param->OutputPipFrameAOI.Height = FH1;
            break;
        case AMP_ROTATE_0_HORZ_FLIP:        /* No rotation and horizontal flip */
            param->OutputPipFrameAOI.X = PX + distX2 * PW / IW;
            param->OutputPipFrameAOI.Y = PY + distY1 * PH / IH;
            param->OutputPipFrameAOI.Width = FW1;
            param->OutputPipFrameAOI.Height = FH1;
            break;
        case AMP_ROTATE_90:                 /* Clockwise 90 degree */
            param->OutputPipFrameAOI.X = PX + distY2 * PW / IH;
            param->OutputPipFrameAOI.Y = PY + distX1 * PH / IW;
            param->OutputPipFrameAOI.Width = FW2;
            param->OutputPipFrameAOI.Height = FH2;
            break;
        case AMP_ROTATE_90_VERT_FLIP:       /* Clockwise 90 degree and vertical flip*/
            param->OutputPipFrameAOI.X = PX + distY2 * PW / IH;
            param->OutputPipFrameAOI.Y = PY + distX2 * PH / IW;
            param->OutputPipFrameAOI.Width = FW2;
            param->OutputPipFrameAOI.Height = FH2;
            break;
        case AMP_ROTATE_180:                /* Clockwise 180 degree */
            param->OutputPipFrameAOI.X = PX + distX2 * PW / IW;
            param->OutputPipFrameAOI.Y = PY + distY2 * PH / IH;
            param->OutputPipFrameAOI.Width = FW1;
            param->OutputPipFrameAOI.Height = FH1;
            break;
        case AMP_ROTATE_180_HORZ_FLIP:      /* Clockwise 180 degree and horizontal flip */
            param->OutputPipFrameAOI.X = PX + distX1 * PW / IW;
            param->OutputPipFrameAOI.Y = PY + distY2 * PH / IH;
            param->OutputPipFrameAOI.Width = FW1;
            param->OutputPipFrameAOI.Height = FH1;
            break;
        case AMP_ROTATE_270:                /* Clockwise 270 degree */
            param->OutputPipFrameAOI.X = PX + distY1 * PW / IH;
            param->OutputPipFrameAOI.Y = PY + distX2 * PH / IW;
            param->OutputPipFrameAOI.Width = FW2;
            param->OutputPipFrameAOI.Height = FH2;
            break;
        case AMP_ROTATE_270_VERT_FLIP:      /* Clockwise 270 degree and vertical flip */
            param->OutputPipFrameAOI.X = PX + distY1 * PW / IH;
            param->OutputPipFrameAOI.Y = PY + distX1 * PH / IW;
            param->OutputPipFrameAOI.Width = FW2;
            param->OutputPipFrameAOI.Height = FH2;
            break;
        default:
            // Do nothing.
            break;
    }

    return 0; // Success
}

void AppLib_SetYuvBuf_Black(
        UINT8* StartLumaAddr,
        UINT8* StartChromaAddr,
        const UINT32 YBufSize,
        const UINT32 UvBufSize)
{
    // (Y, U, V) = (0, 0, 0) turns out black color
    // Set Y to 0
    memset(StartLumaAddr, 0, YBufSize);
    // Set U and V to 128
    memset(StartChromaAddr, 128, UvBufSize);
}

