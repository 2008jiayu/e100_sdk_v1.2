/**
 * @file src/app/connected/applib/inc/graphics/ApplibGraphics.h
 *
 * Header of Graphics Utilities
 *
 * History:
 *    2013/09/23 - [Martin Lai] created file
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

#ifndef APPLIB_GRAPHICS_H_
#define APPLIB_GRAPHICS_H_
/**
* @defgroup ApplibGraphics_Graphics ApplibGraphics_Graphics
* @brief Graphics define for graphics functions
*
* This is detailed description of Graphics
*/
/**
 * @addtogroup ApplibGraphics_Graphics
 * @ingroup ApplibGraphics
 * @{
 */

#include "ApplibGraphics_Common.h"
#include "ApplibGraphics_MainTask.h"
#include "graphics/DirectDraw/ApplibGraphics_DirectDraw.h"
__BEGIN_C_PROTO__

/*************************************************************************
 * Graphics definitions
 ************************************************************************/
#define GRAPH_CH_FCHAN    (0x01)            /**< GRAPH_CH_FCHAN                 */
#define GRAPH_CH_DCHAN    (0x02)            /**< GRAPH_CH_DCHAN                 */
#define GRAPH_CH_DUAL     (0x03)            /**< GRAPH_CH_DUAL                  */
#define GRAPH_CH_BLEND    (0x04)            /**< GRAPH_CH_BLEND                 */
#define GRAPH_CH_ALL      (0xFFFFFFFF)      /**< GRAPH_CH_ALL                   */

/*************************************************************************
 * Graphics enum
 ************************************************************************/
/**
 *  The definition of graphic plane.
 */
typedef enum _APPLIB_GRAPHIC_GPLANE_ID_e_ {
    GRAPH_GPLANE_0 = 0,                     /**< Graphic plane 0                */
    GRAPH_GPLANE_1,                         /**< Graphic plane 1                */
    GRAPH_GPLANE_2,                         /**< Graphic plane 2                */
    GRAPH_GPLANE_3,                         /**< Graphic plane 3                */
    GRAPH_GPLANE_4,                         /**< Graphic plane 4                */
    GRAPH_GPLANE_5,                         /**< Graphic plane 5                */
    GRAPH_GPLANE_6,                         /**< Graphic plane 6                */
    GRAPH_GPLANE_7,                         /**< Graphic plane 7                */
    GRAPH_GPLANE_8,                         /**< Graphic plane 8                */
    GRAPH_GPLANE_9,                         /**< Graphic plane 9                */
    GRAPH_GPLANE_10,                        /**< Graphic plane 10               */
    GRAPH_GPLANE_11,                        /**< Graphic plane 11               */
    GRAPH_GPLANE_12,                        /**< Graphic plane 12               */
    GRAPH_GPLANE_13,                        /**< Graphic plane 13               */
    GRAPH_GPLANE_14,                        /**< Graphic plane 14               */
    GRAPH_GPLANE_15,                        /**< Graphic plane 15               */
    GRAPH_GPLANE_NUM                        /**< Total number of gplane         */
} APPLIB_GRAPHIC_GPLANE_ID_e;

/**
 *  The definition of color format of encode
 */
typedef enum _APPLIB_GRAPH_ENCODE_FORMAT_e_ {
    ENCODE_FORMAT_YUV422 = 0,               /**< YUV422 format                  */
    ENCODE_FORMAT_YUV420,                   /**< YUV420 format                  */
    ENCODE_FORMAT_NUM                       /**< Total number of encode foamte  */
} APPLIB_GRAPH_ENCODE_FORMAT_e;

/**
 *  The definition of font type
 */
typedef enum _APPLIB_GRAPH_FONT_TYPE_e_ {
    FONT_TYPE_BMP = 0,                      /**< BMP font                       */
    FONT_TYPE_CUSTOMIZED,                   /**< Customized font                */
    FONT_TYPE_NUM                           /**< Total number of font type      */
} APPLIB_GRAPH_FONT_TYPE_e;

/*************************************************************************
 * Graphics Struct
 ************************************************************************/
/**
 * Initial struct for graphics
 */
typedef struct _APPLIB_GRAPH_FONT_FUNCTION_s_ {
    int (*Init_f)(void);                                        /**< Init Font                              */
    UINT32 (*GetFontSize_f)(const char* FontFn);                /**< Get Font size                          */
    int (*Load_f)(UINT8 *FontBuffer);                           /**< Load Font data to FontBuffer           */
    int (*Draw_f)( const APPLIB_GRAPHIC_RENDER_s *render,
                   APPLIB_FONT_DRAW_CONFIG_s drawConfig);       /**< Draw function                          */
    UINT32 (*GetStrWidth_f)(UINT32 StrHeight, UINT16* Str);     /**< Get String Width function              */
    UINT32 (*GetStrHeight_f)(UINT32 StrHeight, UINT16 *Str);    /**< Get String Height function             */
} APPLIB_GRAPH_FONT_FUNCTION_s;

/**
 * Initial struct for graphics
 */
typedef struct _APPLIB_GRAPH_FONT_CONFIG_s_ {
    const char* FontFileName;                                   /**< the binary filename of font             */
    UINT32 FontSize;                                            /**< the file size of font                   */
    void *FontBuffer;                                           /**< the font data buffer address            */
    APPLIB_GRAPH_FONT_TYPE_e FontType;                          /**< the type of font                        */
    APPLIB_GRAPH_FONT_FUNCTION_s FontFunc;
} APPLIB_GRAPH_FONT_CONFIG_s;

/**
 * Initial struct for graphics
 */
typedef struct _APPLIB_GRAPH_INIT_CONFIG_s_ {
    APPLIB_GRAPH_FONT_CONFIG_s Font;                            /**< the configures of font                  */
    const char* ClutFileName;                                   /**< the binary filename of clut             */
    const char* BMPFileName;                                    /**< the binary filename of BMP              */
    const char* StringFileName;                                 /**< the binary filename of string           */
    UINT8 DchanEnable;                                          /**< the enable / disable flag of D-chan     */
    UINT8 FchanEnable;                                          /**< the enable / disable flag of F-chan     */
    UINT8 BlendEnable;                                          /**< the enable / disable flag of blending   */
} APPLIB_GRAPH_INIT_CONFIG_s;

/*************************************************************************
 * Graphics APIs
 ************************************************************************/
/**
 *  @brief Init graphics module
 *
 *  Init graphics module
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraph_Init(void);

/**
 *  @brief Configure graphics module
 *
 *  Configure graphics module
 *
 *  @return
 *  @see
 */
extern void AppLibGraph_SetDefaultConfig(APPLIB_GRAPH_INIT_CONFIG_s initConfig);

/**
 *  @brief Set OSD size
 *
 *  Set OSD size
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *  @param [in] width The width of OSD
 *  @param [in] height The height of OSD
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraph_SetOsdSize(UINT32 graphChannelId, int width, int height);

/**
 *  @brief Set pixel format
 *
 *  Set pixel format
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *  @param [in] format Pixel format
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_SetPixelFormat(UINT32 graphChannelId, AMP_DISP_OSD_FORMAT_e format);

/**
 *  @brief Update the window of graphic
 *
 *  Update the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_SetWindowConfig(UINT32 graphChannelId);

/**
 *  @brief Set max object numbers
 *
 *  To set the total number of object
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] objectNum total number of object
 *
 *  @return 0 Success
 *  @see
 */
extern int AppLibGraph_SetMaxObjectNum(UINT32 graphChannelId, UINT32 objectNum);

/**
 *  @brief Set GUI's layout
 *
 *  Set GUI's layout
 *
 *  @param[in] graphChannelId The specific channel id
 *  @param[in] layoutId The layout id for the specific channel
 *  @param[in] *uiObjTable[] The gui table
 *  @param[in] langIdx The language id of gui table
 *
 *  @return 0 Success
 *  @see APPLIB_GRAPHIC_UIOBJ_s
 */
extern int AppLibGraph_SetGUILayout(UINT32 graphChannelId, UINT32 layoutId, APPLIB_GRAPHIC_UIOBJ_s *uiObjTable[], UINT32 langIdx);

/**
 *  @brief Active the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_ActivateWindow(UINT32 graphChannelId);

/**
 *  @brief Enable draw the objecf
 *
 *  Enable draw the objecf
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_EnableDraw(UINT32 graphChannelId);

/**
 *  @brief Disable draw the objecf
 *
 *  Disable draw the objecf
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_DisableDraw(UINT32 graphChannelId);

/**
 *  @brief Deactive the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_DeactivateWindow(UINT32 graphChannelId);

/**
 *  @brief Flush the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibGraph_FlushWindow(UINT32 graphChannelId);

/**
 *  @brief Get the width of the string
 *
 *  Get the width of the string
 *
 *  @param[in] strSize the string height
 *  @param[in] *str the specific string
 *
 *  @return 0 Success
 *  @see
 */
extern UINT32 AppLibGraph_GetStringWidth( UINT32 graphChannelId, UINT32 guiId, UINT32 strSize);

/**
 *  @brief Retrive GUI object's information
 *
 *  Retrive GUI object's information
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] areaInfo the gui's area info
 *
 *  @return
 *  @see
 */
extern void AppLibGraph_RetrieveObjInfo(UINT32 graphChannelId, UINT32 guiId, AMP_AREA_s *areaInfo);

/**
 *  @brief Update BMP
 *
 *  Let user can change the specific BMP manually
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] bmpId the modified BMP id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdateBMP(UINT32 graphChannelId, UINT32 guiId, UINT32 bmpId);

/**
 *  @brief Update string
 *
 *  Let user can change the specific string manually
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] strId the modified string id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdateString(UINT32 graphChannelId, UINT32 guiId, UINT32 strId);

/**
 *  @brief Update string's context in BIN
 *
 *  Change string's context
 *
 *  @param[in] langIdx the language id of the specific string
 *  @param[in] strId the specific string id
 *  @param[in] *str the updated sting
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdateStringContext(UINT32 langIdx, UINT32 strId, UINT16 *str);

/**
 *  @brief Update GUI object's color
 *
 *  Change GUI object's color
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] foreColor the fore color of the gui id
 *  @param[in] backColor the back color of the gui id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdateColor(UINT32 graphChannelId, UINT32 guiId, UINT32 foreColor, UINT32 backColor);

/**
 *  @brief Update GUI object's position
 *
 *  Change GUI object's position
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] left X coordinate of Graphic obj's Bottom left
 *  @param[in] bottom Y coordinate of Graphic obj's Bottom right
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdatePosition(UINT32 graphChannelId, UINT32 guiId, UINT32 left, UINT32 bottom);

/**
 *  @brief Update GUI object's size
 *
 *  Change GUI object's size
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] width the gui's modified width
 *  @param[in] height the gui's modified height
 *  @param[in] strSize the string's modified height
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_UpdateSize(UINT32 graphChannelId, UINT32 guiId, UINT32 width, UINT32 height, UINT32 strSize);

/**
 *  @brief Show shape
 *
 *  Show shape (including rectangle, line, circle)
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] uiObj the shape object
 *
 *  @return 0 Success
 *  @see
 */
extern int AppLibGraph_ShowShape(UINT32 graphChannelId, APPLIB_GRAPHIC_UIOBJ_s *uiObj);

/**
 *  @brief Show GUI objects
 *
 *  Set the specific gui object visible
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the specific object id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
extern int AppLibGraph_Show(UINT32 graphChannelId, UINT32 guiId);

/**
 *  @brief Hide GUI objects
 *
 *  Set the specific gui object un-visible
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the specific object id
 *
 *  @return 0 Success, <0 Fail
 *  @see AppLibGraph_HideAll
 */
extern int AppLibGraph_Hide(UINT32 graphChannelId, UINT32 guiId);

/**
 *  @brief Hide All
 *
 *  Hide all objects on the specific channel
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return 0 Success
 *  @see AppLibGraph_Hide
 */
extern int AppLibGraph_HideAll(UINT32 graphChannelId);

/**
 *  @brief Draw all objects on the canvas
 *
 *  Draw all objects on the canvas
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return 0 Success
 *  @see
 */
extern int AppLibGraph_Draw(UINT32 graphChannelId);

/**
 *  @brief Save the graphic as BMP file
 *
 *  Save the graphic as BMP file
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return
 *  @see
 */
extern void AppLibGraph_SaveAsBMP(UINT32 graphChannelId);

/**
 *  @brief Init stamp
 *
 *  Init stamp
 *
 *  @return
 *  @see
 */
extern void AppLibGraph_InitStamp(void);

/**
 *  @brief Reset stamp
 *
 *  Reset stamp
 *
 *  @return
 *  @see
 */
extern void AppLibGraph_ResetStamp(void);

/**
 *  @brief Add a new stamp area
 *
 *  Add a new stamp area
 *
 *  @param[in] stampArea stamp area config
 *  @param[in] encodeFormat the encode format
 *
 *  @return
 *  @see
 */
extern UINT8 AppLibGraph_AddStampArea(AMP_AREA_s stampArea, APPLIB_GRAPH_ENCODE_FORMAT_e encodeFormat);

/**
 *  @brief Update a specific stamp area
 *
 *  Update a specific stamp area
 *
 *  @param[in] stampAreaId stamp area Id
 *  @param[in] stampArea stamp area config
 *  @param[in] encodeFormat the encode format
 *
 *  @return
 */
extern void AppLibGraph_UpdateStampArea(UINT8 stampAreaId, AMP_AREA_s stampArea, APPLIB_GRAPH_ENCODE_FORMAT_e encodeFormat);

/**
 *  @brief Pass a action to DirectDraw library
 *
 *  Pass a action to DirectDraw library. Like to get OSD buffer or to flush OSD buffer.
 *
 *  @param[in] structure of DirectDraw content
 *
 *  @return
 */
extern void AppLibGraph_DirectDrawAct(APPLIB_GRAPHIC_DIRECTDRAW_CNT_s *ddCnt);
#ifdef CONFIG_APP_ARD
extern int AppLibGraph_GetFontType(void);
#endif
__END_C_PROTO__

#endif /* APPLIB_GRAPHICS_H_ */

/**
 * @}
 */

