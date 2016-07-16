/**
 * @file src/app/connected/applib/inc/graphics/bmp/ApplibGraphics_Bmp.h
 *
 * ApplibGraphics_ambaBmp include Amba BMP related
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

#ifndef _APPLIB_GRAPHICS_BMP_H_
#define _APPLIB_GRAPHICS_BMP_H_

/**
* @defgroup ApplibGraphics_BMP ApplibGraphics_BMP
* @brief BMP define for graphics functions
*
* This is detailed description of BMP
*/
/**
 * @addtogroup ApplibGraphics_BMP
 * @ingroup GraphicsObj
 * @{
 */

#include <display/Osd.h>

/** bitmap give transparency color */
#define APPLIB_GRAPHIC_BMP_BMF2_HAS_TRANS   0x80

/*************************************************************************
 * BMP Enums
 ************************************************************************/
/**
 * BMP Encode Type
 */
typedef enum _APPLIB_BMP_ENCODE_TYPE_e_ {
    ENCODE_BMP_BMF2_RAW = 0x00,                 /**< bitmap is not encoded.                 */
    ENCODE_BMP_BMF2_AMBARLE,                    /**< bitmap is RLE encoded.                 */
    ENCODE_BMP_BMF2_AVCHDRLE,                   /**< bitmap is AVCHD RLE encoded.           */
    ENCODE_BMP_BMF2_STDRLE,                     /**< bitmap is STD RLE encoded.             */
    ENCODE_BMP_BMF2_ANM,                        /**< bitmap is animation.                   */
    ENCODE_BMP_END                              /**< bitmap encode end.                     */
} APPLIB_BMP_ENCODE_TYPE_e;

/**
 * BMP Loading Status
 */
typedef enum _APPLIB_BMP_STATUS_e_ {
    BMP_STATUS_INIT = 0x00,                     /**< bitmap is inited                       */
    BMP_STATUS_LOADED,                          /**< bitmap is loaded                       */
    BMP_INIT_SATAUS_END                         /**< bitmap status end                      */
} APPLIB_BMP_STATUS_e;

/*************************************************************************
 * BMP Structures
 ************************************************************************/
/**
 * BMP Header Format
 */
typedef struct _APPLIB_GRAPHIC_BMP_BIN_HEADER_s_ {
    UINT16  Magic;                              /**< Magic number                           */
    UINT16  Version;                            /**< Version                                */
    UINT16  HeaderSize;                         /**< sizeof (APPLIB_GRAPHIC_BMP_BIN_HEADER_s)   */
    UINT16  DescSize;                           /**< sizeof (APPLIB_GRAPHIC_BMP_BIN_DESC_s)     */
    UINT16  ResNum;                             /**< number of resolution (>= 1)            */
    UINT16  BmpNum;                             /**< number of bmp (>= 1)                   */
    UINT16  PixelFmt;                           /**< pixel format                           */
    UINT16  Reversed[9];                        /**< future use                             */
} APPLIB_GRAPHIC_BMP_BIN_HEADER_s;

/**
 * BMP Format
 */
typedef struct _APPLIB_GRAPHIC_BMP_s_ {
    APPLIB_BMP_ENCODE_TYPE_e  Flags;            /**< combination of flags above             */
    UINT8  Bits;                                /**< 1, 2, 4, 8, 16, or 24                  */
    UINT16 Width;                               /**< in pixels                              */
    UINT16 Height;                              /**< in pixels                              */
    AMP_DISP_OSD_HW_RESCALER_TYPE_e  Pxf;       /**< bitmap pixel format                    */
    UINT8  Reserve;                             /**< reserve byte                           */
    UINT32 TColor;                              /**< transparent color for > 8bpp bitmaps   */
    UINT8  *Ptr;                                /**< bitmap data pointer                    */
} APPLIB_GRAPHIC_BMP_s;

/**
 * BMP Descritor Format
 */
typedef struct _APPLIB_GRAPHIC_BMP_BIN_DESC_s {
    UINT32 Offset;                              /**< file position                          */
    UINT32 Size;                                /**< bmp size                               */
    APPLIB_BMP_STATUS_e Flags;                  /**< internal flags for loader              */
    UINT16 Count;                               /**< internal count for loader              */
    APPLIB_GRAPHIC_BMP_s *BmpPtr;               /**< internal pointer for loader            */
} APPLIB_GRAPHIC_BMP_BIN_DESC_s;

/**
 * BMP Info Format
 */
typedef struct _APPLIB_GRAPHIC_BMP_BIN_INFO_s_ {
    char BinFileName[64];                       /**< The file name of BMP.bin               */
    UINT32 ResIdx;                              /**< Resolution index of this BMP BIN INFO  */
    UINT32 BmpNum;                              /**< Number of BMPs of this resolution      */
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *DescTab;     /**< BMP Description Ptr table array        */
} APPLIB_GRAPHIC_BMP_BIN_INFO_s;

/**
 * Attribute Format for BMP GUI Object
 */
typedef struct _APPLIB_GRAPHIC_BMP_CNT_t_ {
    UINT32 Left;                                /**< X coordinate of GUI obj's top left     */
    UINT32 Bottom;                              /**< Y coordinate of GUI obj's top left     */
    UINT32 ResIdx;                              /**< resolution index of GUI obj's in Bin file */
    UINT32 BMPIdx;                              /**< BMP index of Graphic obj's in Bin file */
    APPLIB_GRAPHIC_BMP_BIN_INFO_s *BmpInfo;     /**< Bin file header of this GUI obj        */
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *BmpDescPtr;  /**< Bin file description of this GUI obj   */
} APPLIB_GRAPHIC_BMP_CNT_s;

/*************************************************************************
 * BMP Functions
 ************************************************************************/
/**
 *  @brief Get the specific BMP size from ROM
 *
 *  Get the specific BMP size of the specific resoltuion in BIN from ROM
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] ResIdx Indicated which resolution in BMP bin file is going to calculate.
 *  @param [in] BMPIdx Returned total BMP number in BMP bin file.
 *  @param [out] BMPSize Returned the specific BMP size of the specific resolution
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibBMP_GetOneBMPSize( const char *fileName,
                                    UINT32 ResIdx,
                                    UINT32 BMPIdx,
                                    UINT32 *BMPSize);
/**
 *  @brief Get BMP information from ROM
 *
 *  Get the specific resoltuion BMP information in BIN from ROM
 *  This Buffer includes needed index/header of the BMP bin
 *  BMP buffer structure:
 *      APPLIB_GRAPHIC_BMP_BIN_INFO_t     BMP bin info
 *      APPLIB_GRAPHIC_BMP_BIN_DESC_s     BMP desc Array(Index)
 *      APPLIB_GRAPHIC_BMP_s              BMP data Array(BMPbuffer)
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx Indicated which resolution in BMP bin file is going to calculate.
 *  @param [out] bmpTotalNum Returned total BMP number in BMP bin file.
 *  @param [out] bufferSize Returned BMP buffer size
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibBMP_GetTotalBMPSize( const char *fileName,
                                      UINT32 resIdx,
                                      UINT32 *bmpTotalNum,
                                      UINT32 *bufferSize);
/**
 *  @brief Get one BMP
 *
 *  Load a BMP data from BMP BIN
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx the specific resolution in BMP BIN
 *  @param [in] bmpIdx the specific BMP index in BMP BIN
 *  @param [out] bmpBuf the wanted BMP buffer
 *  @param [out] bmp the wanted BMP data
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibBMP_LoadBMP( const char *fileName,
                              UINT32 resIdx,
                              UINT32 bmpIdx,
                              void *bmpBuf,
                              APPLIB_GRAPHIC_BMP_s **bmp);
/**
 *  @brief Initialize one resolution of BMPs in BMP.bin.
 *
 *  Construct indexes in Buf, the index includes APPLIB_GRAPHIC_BMP_BIN_HEADER_s & APPLIB_GRAPHIC_BMP_BIN_INFO_t
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx the specific resolution in BMP BIN
 *  @param [in] bmpBuf the BMP buffer
 *  @param [in] loadBMP load the whole BMP from BIN or not
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibBMP_InitBMPBuffer( const char *fileName,
                                    UINT32 resIdx,
                                    void *bmpBuf,
                                    UINT8 loadBMP);

#endif /* _APPLIB_GRAPHICS_BMP_H_ */

/**
 * @}
 */

