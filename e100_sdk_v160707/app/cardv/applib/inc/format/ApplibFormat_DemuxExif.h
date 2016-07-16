/**
 * @file src/app/connected/applib/inc/format/ApplibFormat_DemuxExif.h
 *
 * Header of exif demux
 *
 * History:
 *    2014/08/25 - [phcheng] created file
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

#ifndef APPLIB_DEMUX_EXIF_H_
#define APPLIB_DEMUX_EXIF_H_
/**
* @defgroup ApplibFormat_DemuxExif
* @brief exif demux
*
*
*/

/**
 * @addtogroup ApplibFormat_DemuxExif
 * @ingroup Format
 * @{
 */
__BEGIN_C_PROTO__

/**
 *   Initialization of exif demuxer.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatDemuxExif_Init(void);

/**
 *  Feed image into raw buffer
 *
 *  @param [in] codecHdlr       Pointer to the still decode handler.
 *  @param [in] Fn              File name. Full path of an image.
 *  @param [in] ImageSource     The decode source of file. 0: fullview  1: thumbnail  2: screennail.
 *  @param [in] RawBuf          Address of raw buffer.
 *  @param [in] SizeRawBuf      Size of raw buffer.
 *  @param [in] MPOImage        MPO Image.
 *  @param [in] MPOIdx          MPO Index.
 *  @param [out] ImageWidth     Width of the image.
 *  @param [out] ImageHeight    Height of the image.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibFormatDemuxExif_Feed(void* codecHdlr,
        char* Fn,
        UINT8 ImageSource,
        void* RawBuf,
        UINT32 SizeRawBuf,
        UINT8 MPOImage,
        UINT8 MPOIdx,
        UINT32 *ImageWidth,
        UINT32 *ImageHeight);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_DEMUX_EXIF_H_ */
