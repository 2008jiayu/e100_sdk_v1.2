/**
 * @file src/app/connected/applib/inc/player/still_decode/ApplibPlayer_Thumb_Basic.h
 *
 * The apis provide basic thumbnail for 6 scene on display with focus feature
 *
 * History:
 *    2013/07/29 - [cyweng] Create file
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
 * @defgroup ApplibPlayer_Thumb_Basic       ApplibPlayer_Thumb_Basic
 * @brief Decode and display 6 images tiled on screen.
 *
 * Designed for thumbnail mode.                             \n
 * The displayed area of each image is user-defined.        \n
 * There're two sets of user-defined areas to distinguish
 * a focused file form others.
 */

/**
 * @addtogroup ApplibPlayer_Thumb_Basic
 * @ingroup StillDecode
 * @{
 */

#ifndef APPLIB_THUMB_BASIC_H_
#define APPLIB_THUMB_BASIC_H_

#include <applib.h>
#include <player/Decode.h>
#include <player/VideoDec.h>
#include <cfs/AmpCfs.h>
#include <player/decode_utility/ApplibPlayer_Common.h>
/**
 *
 * applib thumb basic table
 *
 */
typedef struct _APPLIB_THUMB_BASIC_TABLE_s_ {
    /** Number of scenes in basic thumbnail mode */
    UINT8 NumScenes;
    /** Display information of each scene */
    APPLIB_STILL_DISP_DESC_s *AreaNormal;
    /** Display information of each scene when it's focused */
    APPLIB_STILL_DISP_DESC_s *AreaFocused;
} APPLIB_THUMB_BASIC_TABLE_s;

/**
 *
 * applib thumb basic file
 *
 */
typedef struct _APPLIB_THUMB_BASIC_FILE_s_ {
    /** filename */
    char Filename[MAX_FILENAME_LENGTH];
    /** if the file is focused */
    UINT8 Focused;
    /** the source of file, indicate to show thumbnail / fullview / screennail */
    UINT32 FileSource;
} APPLIB_THUMB_BASIC_FILE_s;

/**
 * Initialize basic thumbnail, support to display thumbnail on at most 2 window.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibThmBasic_Init(void);

/**
 * Show black on screen.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibThmBasic_ClearScreen(void);

/**
 * Show thumbnail on window.
 *
 * @param [in] LocactionInfo    The location table
 * @param [in] NumFiles         Number of file to show
 * @param [in] Files            Array pointer to files to show
 * @param [in] Decoded          The files have been decoded and there's no need to decode them again
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibThmBasic_Show(APPLIB_THUMB_BASIC_TABLE_s *LocactionInfo,
                               UINT8 NumFiles,
                               APPLIB_THUMB_BASIC_FILE_s *Files,
                               UINT8 Decoded);

/**
 * Deinit basic thumbnail.
 *
 * @return 0 - OK, others - Error
 */
extern int AppLibThmBasic_Deinit(void);

#endif /* APPLIB_THUMB_BASIC_H_ */

/**
 * @}
 */     // End of group ApplibPlayer_Thumb_Basic
