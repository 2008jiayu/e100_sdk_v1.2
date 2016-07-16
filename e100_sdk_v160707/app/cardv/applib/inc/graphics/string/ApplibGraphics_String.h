/**
 * @file src/app/connected/applib/inc/graphics/string/ApplibGraphics_String.h
 *
 * ApplibGraphics_String include Amba string module related
 *
 * History:
 *    2013/12/06 - [Eric Yen] created file
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
#ifndef _APPLIB_GRAPHICS_STRING_H_
#define _APPLIB_GRAPHICS_STRING_H_

/**
* @defgroup ApplibGraphics_String ApplibGraphics_String
* @brief String define for graphics functions
*
* This is detailed description of String
*/
/**
 * @addtogroup ApplibGraphics_String
 * @ingroup GraphicsObj
 * @{
 */

#include <wchar.h>
#include <graphics/obj/ApplibGraphics_Obj.h>
#include <graphics/font/ApplibGraphics_Font.h>

/*************************************************************************
 * Shape definition
 ************************************************************************/
#define APPLIB_GRAPHIC_STR_BIN_INFO_INIT         0x0        /**< String is inited */
#define APPLIB_GRAPHIC_STR_BIN_INFO_LOAD         0x1        /**< String is loaded */

/*************************************************************************
 * Shape Enum
 ************************************************************************/
/**
 * String algnment description
 */
typedef enum _APPLIB_GRAPHIC_STRING_ALIGN_e_ {
    APPLIB_GRAPHIC_STRING_ALIGN_TL = 0,                     /**< top-left position                          */
    APPLIB_GRAPHIC_STRING_ALIGN_TM,                         /**< top-middle position                        */
    APPLIB_GRAPHIC_STRING_ALIGN_TR,                         /**< top-right postion                          */
    APPLIB_GRAPHIC_STRING_ALIGN_ML,                         /**< middle-right position                      */
    APPLIB_GRAPHIC_STRING_ALIGN_MM,                         /**< middle-middle postion                      */
    APPLIB_GRAPHIC_STRING_ALIGN_MR,                         /**< middle-right position                      */
    APPLIB_GRAPHIC_STRING_ALIGN_BL,                         /**< bottom-left position                       */
    APPLIB_GRAPHIC_STRING_ALIGN_BM,                         /**< bottom-middle position                     */
    APPLIB_GRAPHIC_STRING_ALIGN_BR,                         /**< bottom-right position                      */
} APPLIB_GRAPHIC_STRING_ALIGN_e;

/**
 * Graphic content shadow position description
 */
typedef enum _APPLIB_GRAPHIC_STRING_SHADOW_POSITION_e_ {
    APPLIB_GRAPHIC_STRING_SHADOW_TL = 0,                    /**< top-left shadow                            */
    APPLIB_GRAPHIC_STRING_SHADOW_TM,                        /**< top-middle shadow                          */
    APPLIB_GRAPHIC_STRING_SHADOW_TR,                        /**< top-right shadow                           */
    APPLIB_GRAPHIC_STRING_SHADOW_ML,                        /**< middle-left shadow                         */
    APPLIB_GRAPHIC_STRING_SHADOW_MM,                        /**< middle-middle shadow                       */
    APPLIB_GRAPHIC_STRING_SHADOW_MR,                        /**< middle-right shadow                        */
    APPLIB_GRAPHIC_STRING_SHADOW_BL,                        /**< bottom-left shadow                         */
    APPLIB_GRAPHIC_STRING_SHADOW_BM,                        /**< bottom-middle shadow                       */
    APPLIB_GRAPHIC_STRING_SHADOW_BR,                        /**< bottom-right shadow                        */
} APPLIB_GRAPHIC_STRING_SHADOW_POSITION_e;

/*************************************************************************
 * Shape Struct
 ************************************************************************/
/**
 * Header struct in BIN code of string
 */
typedef struct _APPLIB_GRAPHIC_STR_BIN_HEADER_s_ {
    UINT16  Magic;                                          /**< Magic number                               */
    UINT16  Version;                                        /**< Version                                    */
    UINT16  HeaderSize;                                     /**< sizeof (APPLIB_GRAPHIC_STR_BIN_HEADER_s)   */
    UINT16  LangNum;                                        /**< number of languages (>= 1)                 */
    UINT16  MsgNum;                                         /**< number of messages (>= 1)                  */
    UINT16  Reversed[11];                                   /**< future use                                 */
} APPLIB_GRAPHIC_STR_BIN_HEADER_s;

/**
 * Description struct in BIN code of string
 */
typedef struct _APPLIB_GRAPHIC_STR_BIN_DESC_t_ {
    UINT32 Offset;                                          /**< file position                              */
    UINT32 Size;                                            /**< string size                                */
    UINT16 Flags;                                           /**< internal flags for loader                  */
    UINT16 Count;                                           /**< internal count for loader                  */
    WCHAR *Ptr;                                             /**< internal pointer for loader                */
} APPLIB_GRAPHIC_STR_BIN_DESC_s;

/**
 * Info struct in BIN code of string
 */
typedef struct _APPLIB_GRAPHIC_STR_BIN_INFO_t_ {
    char BinFileName[64];                                   /**< The file name of BMP.bin                   */
    UINT16 LangIdx;                                         /**< Index of languages                         */
    UINT16 MsgNum;                                          /**< number of messages (>= 1)                  */
    APPLIB_GRAPHIC_STR_BIN_DESC_s *DescTable;               /**< [Str0 Str1 Str2 ...]                       */
} APPLIB_GRAPHIC_STR_BIN_INFO_s;

/**
 * Graphic content shadow description
 */
typedef struct _APPLIB_GRAPHIC_STRING_SHADOW_t_ {
    UINT8 Enable;                                           /**< Enable                                     */
    APPLIB_GRAPHIC_STRING_SHADOW_POSITION_e Postion;        /**< Postion                                    */
    UINT32 Distance;                                        /**< Distance                                   */
    UINT32 Color;                                           /**< Color                                      */
} APPLIB_GRAPHIC_STRING_SHADOW_s;

/**
 * Graphic content string description
 */
typedef struct _APPLIB_GRAPHIC_STR_CNT_t_ {
    UINT32 Left;                                            /**< X coordinate of Graphic obj's Bottom left      */
    UINT32 Top;                                             /**< Y coordinate of Graphic obj's Bottom left      */
    UINT32 Width;                                           /**< String box width                               */
    UINT32 Height;                                          /**< String box height                              */
    UINT32 StrSize;                                         /**< font height(size)                              */
    UINT32 ColorBack;                                       /**< Background color                               */
    UINT32 ColorFore;                                       /**< Foreground color                               */
    APPLIB_FONT_s *FontAttr;                        /**< Fonts desc structure                           */
    APPLIB_GRAPHIC_STRING_ALIGN_e Alignment;                /**< Fonts alignment                                */
    UINT32 LangIdx;                                         /**< Language index of Graphic obj's in Bin file    */
    UINT32 MsgIdx;                                          /**< Message index of Graphic obj's in Bin file     */
    APPLIB_GRAPHIC_STRING_SHADOW_s Shadow;                  /**< Shadow settings of this Graphic obj            */
    APPLIB_GRAPHIC_STR_BIN_INFO_s *StrInfo;                 /**< Bin file heaser info of this Graphic obj       */
    APPLIB_GRAPHIC_STR_BIN_DESC_s *StrDesc;                 /**< String description info of this Graphic obj    */
} APPLIB_GRAPHIC_STR_CNT_s;

/*************************************************************************
 * String Function
 ************************************************************************/
/**
 *  @brief Calculate the total size of string from rom file system
 *
 *  Calculate needed Str buffer size for some language from String.bin in ROMFS
 *  This Buffer includes needed index/header of the String bin
 *
 *  @param [in] *fileName Indicated which .bin file generated by AmbaGUIGen
 *  @param [in] langIdx Indicate which language is going to calculate
 *  @param [out] *msgNum Returned message number of that language
 *  @param [out] *strBufSize Returned needed Str buffer size
 *  @param [out] *tmpBufSize Returned needed string tmp buffer size
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibStr_CalcLangSizeFromROMFS(const char *fileName,
                                           UINT32 langIdx,
                                           UINT32 *msgNum,
                                           UINT32 *strBufSize,
                                           UINT32 *tmpBufSize);

/**
 *  @brief Initialize one resolution of BMPs in BMP.bin.
 *
 *  Initialize one language of strings in str.bin.
 *  Construct indexes in strBuf, the index includes content of APPLIB_GRAPHIC_BMP_BIN_INFO_t
 *
 *  @param [in] *fileName Indicated which .bin file generated by AmbaGUIGen
 *  @param [in] langIdx Specify which language to init
 *  @param [in] *strBuf An allocated buffer for loading string, and string index,
 *  @param [in] *tmpBuf An allocated buffer for temporary used when loading messages,
 *                      can be released immediately after function called.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibStr_InitFromROMFS(const char *fileName,
                                      UINT32 langIdx,
                                      void *strBuf,
                                      void *tmpBuf);

#endif /* _APPLIB_GRAPHICS_STRING_H_ */

/**
 * @}
 */

