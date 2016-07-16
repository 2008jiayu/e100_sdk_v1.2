/**
 * @file app/cardv/applib/inc/graphics/ApplibGraphics_Common.h
 *
 * ApplibGraphics_Common.h only for Amba Graphics AppLib common internal use
 *
 * History:
 *    2015/06/24 - [Yuchi Wei] created file from 'src/graphics/ApplibGraphics_Common.h'
 *                             merged 'src/graphics/graphics.h'
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

#ifndef _GRAPHICS_COMMON_H_
#define _GRAPHICS_COMMON_H_

#include <AmbaDataType.h>
#include <AmbaPrint.h>      // AmbaPrint
#include <AmbaPrintk.h>     // AmbaPrintColor
#if 1
#include <AmbaKAL.h>    // AMBA_KAL_MUTEX_t
#include <AmbaROM.h>    // AMBA_ROM_SYS_DATA
#include <AmbaCache.h>  // AmbaCache_Clean
#include <util.h>
#include <comsvc/ApplibComSvc_MemMgr.h>

#include <graphics/render/ApplibGraphics_Render.h>
#include <graphics/canvas/ApplibGraphics_Canvas.h>
#include <graphics/font/ApplibGraphics_Font.h>
#include <graphics/stamp/ApplibGraphics_Stamp.h>

#include <graphics/UIObj/ApplibGraphics_UIObj.h>
#endif
/*************************************************************************
 * Enum
 ************************************************************************/
/**
 *  The definition of boolean.
 */
typedef enum _BOOLEAN_e_ {
    BOOLEAN_FALSE   = 0,                /**< False    */
    BOOLEAN_TRUE    = 1                 /**< True     */
} BOOLEAN_e;

/**
 *  The definition of return type.
 */
typedef enum _RETURN_TYPE_e_ {
    RETURN_SUCCESS  = 0,                /**< Success   */
    RETURN_FAIL     = -1                /**< Fail       */
} RETURN_TYPE_e;

/**
 *  The definition of debug level.
 */
typedef enum _DEBUG_LEVEL_e_ {
    DEBUG_ERR       = 1 << 0,           /**< Print error messages   */
    DEBUG_WARNING   = 1 << 1,           /**< Print warning messages */
    DEBUG_ONLY      = 1 << 2,           /**< Print debug messages   */
} DEBUG_LEVEL_e;

/**
 *  The definition of debug level.
 */
typedef enum _DEBUG_MODULE_e_ {
    DEBUG_MODULE_NONE       = 0,
    DEBUG_MODULE_GRAPHICS   = 1 << 0,
    DEBUG_MODULE_MAINTASK   = 1 << 1,
    DEBUG_MODULE_SHAPE      = 1 << 2,
    DEBUG_MODULE_BITMAP     = 1 << 3,
    DEBUG_MODULE_STING      = 1 << 4,
    DEBUG_MODULE_FONT       = 1 << 5,
    DEBUG_MODULE_STAMP      = 1 << 6,
    DEBUG_MODULE_CANVAS     = 1 << 7,
    DEBUG_MODULE_RENDER     = 1 << 8,
    DEBUG_MODULE_UIOBJ      = 1 << 9,
} DEBUG_MODULE_e;

#if 1
typedef enum _FILE_SOURCE_e_ {
    FILE_SOURCE_FS = 0,
    FILE_SOURCE_ROM,
    FILE_SOURCE_NUM,
} FILE_SOURCE_e;

typedef enum _BUFFER_TYPE_e_ {
    BUFFER_TYPE_WORKING = 0,
    BUFFER_TYPE_DISPLAY_1ST,
    BUFFER_TYPE_DISPLAY_2ND,
    BUFFER_TYPE_CLUT,
} BUFFER_TYPE_e;

/*************************************************************************
 * Struct
 ************************************************************************/
typedef struct _APPLIB_GRAPH_BMPFILE_HEADER_t_ {
    unsigned int    file_size;
    unsigned int    reserved;
    unsigned int    image_offset;
    unsigned int    header_size;
    unsigned int    xres;
    unsigned int    yres;
    unsigned short  num_of_planes;
    unsigned short  bits_per_pix;
    unsigned int    compression;
    unsigned int    bit_map_size;
    unsigned int    hor_res;
    unsigned int    vert_res;
    unsigned int    num_of_colors;
    unsigned int    num_sig_colors;
} APPLIB_GRAPH_BMPFILE_HEADER_t;

typedef struct _WORKING_BUFFER_INFO_s_ {
    UINT32 Width;
    UINT32 Height;
    void *Address;
    void *RawAddress;
} WORKING_BUFFER_INFO_s;

typedef struct _DISPLAY_BUFFER_INFO_s_ {
    UINT32 Width;
    UINT32 Height;
    void *Address[2];
    void *RawAddress[2];
} DISPLAY_BUFFER_INFO_s;

typedef struct _OSD_INFO_s_ {
    AMP_DISP_OSD_FORMAT_e PixelFormat;
    AMP_OSD_HDLR_s *Hdlr;
    AMP_OSD_CLUT_CFG_s *ClutCfg;
} OSD_INFO_s;

typedef struct _WINDOW_INFO_s_ {
    int WindowId;
} WINDOW_INFO_s;

typedef struct _GRAPH_INFO_s_ {
    UINT8 EnableDraw;
    UINT8 BufferIdx:4;
    UINT8 EnableSwScale:4;
    APPLIB_GRAPHIC_UIOBJ_s **pGuiTable;
    UINT32 ObjectMaxAmout;
    DISPLAY_BUFFER_INFO_s DispalyBuffer;
    WORKING_BUFFER_INFO_s WorkingBuffer;
    APPLIB_GRAPHIC_RENDER_s Render;
    APPLIB_CANVAS_s Canvas;
    APPLIB_GRAPHIC_OBJ_s *ObjList;
    UINT32 Resolution;
    void *BitmapBuffer;
#ifdef CONFIG_APP_ARD
    void *StringBuffer[256];
	UINT8 ActiveLangId;
#else
	void *StringBuffer;
#endif
} GRAPH_INFO_s;

typedef struct _GRAPH_CHAN_s_ {
    BOOLEAN_e PathActived;
    GRAPH_INFO_s GraphicInfo;
    OSD_INFO_s OsdInfo;
    WINDOW_INFO_s WindowInfo;
} GRAPH_CHAN_s;
#endif

/*************************************************************************
 * Variable
 ************************************************************************/
extern DEBUG_MODULE_e DebugModule;
extern DEBUG_LEVEL_e DebugLevel;

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Function
 ************************************************************************/
/**
 *  @brief The print function of Graphics module.
 *
 *  The print function of Graphics module.
 *
 *  @param [in] Level Debug level
 *  @param [in] Message Debug message
 *
 *  @return
 *  @see DEBUG_LEVEL_e
 */
extern void GraphicsPrint(DEBUG_LEVEL_e Level, DEBUG_MODULE_e Module, const char *Message, ...);

/**
 *  @brief Set the debugging level for Graphics module.
 *
 *  Set the debugging level for Graphics module.
 *
 *  @param [in] Level Debug level
 *
 *  @return
 *  @see DEBUG_LEVEL_e
 */
extern void GraphicsPrint_SetLevel(DEBUG_LEVEL_e Level);

/**
 *  @brief Set the debugging module for Graphics module.
 *
 *  Set the debugging module for Graphics module.
 *
 *  @param [in] Module Debug module
 *
 *  @return
 *  @see DEBUG_MODULE_e
 */
extern void GraphicsPrint_SetModule(DEBUG_MODULE_e Module);

#endif // _GRAPHICS_COMMON_H_