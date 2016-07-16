/**
 *  @file PrecMux.h
 *
 *  PRE-RECORD mux format header
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2013/07/05 |clchan      |Created        |
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
#ifndef __PREC_MUX_H__
#define __PREC_MUX_H__

#include <format/Format.h>

/**
 * @defgroup PREC
 * @ingroup Format
 * @brief pre-record mux flow implementation
 *
 * The pre-record mux manager/handler includes the following functions
 * 1. Initiate pre-record mux manager function.
 * 2. Create pre-record mux handler function.
 * 3. Delete pre-record mux handler function.
 *
 */

/**
 * @addtogroup PREC
 * @{
 */

typedef struct {
    AMP_MUX_FORMAT_HDLR_s Hdlr; /**< must be the first member */
    AMBA_KAL_MUTEX_t Mutex;
    UINT32 Length;  /**< the pre-record length (ms) */
    AMP_MEDIA_TRACK_INFO_s Log; /**< the current progress, for pre-check the default video track */
    BOOL8 Open;
    UINT8 master;
    UINT8 limitedend_speedup;
} PREC_MUX_HDLR_s;


typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_BLOCK_POOL_t HdlrBpl;
    BOOL8 Init;
} PREC_MUX_MGR_s;

/**
 * Prec muxer initial configure.
 */
typedef struct {
    UINT8 *Buffer;          /**< The buffer of the Prec muxer manager. */
    UINT32 BufferSize;      /**< The buffer size of the Prec muxer manager. */
    UINT8 MaxHdlr;          /**< The max number of the handler of the Prec muxer manager. */
} PREC_MUX_INIT_CFG_s;

/**
 * Prec muxer configure.
 */
typedef struct {
    UINT32 Length;          /**< The length(ms) of the pre-record muxer. */
} PREC_MUX_CFG_s;

/**
 * Get the default configure of pre-record muxer manager.
 * @param [out] config the buffer to get default configure.
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_GetInitDefaultCfg(PREC_MUX_INIT_CFG_s *config);

/**
 * Get the required buffer size
 * @param [in] maxHdlr the number of pre-record muxers
 * @return the required buffer size
 */
extern UINT32 AppLibPrecMux_GetRequiredBufferSize(UINT8 maxHdlr);

/**
 * Initiate the core of pre-record muxer manager.
 * @param [in] config the configure for initiating the prec muxer manager.
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_Init(PREC_MUX_INIT_CFG_s *config);

/**
 * Get the default pre-record muxer configure.
 * @param [out] config the buffer to get default configure.
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_GetDefaultCfg(PREC_MUX_CFG_s *config);

/**
 * Create a pre-record mux handler.
 * @param [in] config the configure for creating the prec muxer handler.
 * @param [out] hdlr the double pointer to get the handler.
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_Create(PREC_MUX_CFG_s *config, AMP_MUX_FORMAT_HDLR_s **hdlr);

/**
 * Delete a pre-record muxer.
 * @param [in] hdlr the muxer handler that want to delete.
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_Delete(AMP_MUX_FORMAT_HDLR_s *hdlr);

/**
 * Set the pre-record length
 * @param [in] hdlr the muxer handler
 * @param [in] length the pre-record length (ms)
 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */
extern int AppLibPrecMux_SetPrecLength(AMP_MUX_FORMAT_HDLR_s *hdlr, UINT32 length);
/**
 * Set the pre-record splite state
 * @param [in] splite state

 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */

extern int  AppLibPrecMux_SetSpliteState(int splite_state);

/**
 * get the pre-record splite state
 * @param [in] splite state

 * @return 0 - OK, others - APPLIB_ER_CODE_e
 * @see APPLIB_ER_CODE_e
 */

extern int  AppLibPrecMux_getSpliteState(void);

/**
 * get the pre-record end
 * @param [in]

 *
 *
 */

extern void AppLibPrecMux_setprecend(int end);

/**
 * clear the pre-record buffer when event end
 * @param [in]

 *
 *
 */

extern void AppLibPrecMux_fifo_clear(void);
/**
 * set premux master or not
 * @param [in]

 *
 *
 */

extern void AppLibPrecMux_master_set(AMP_MUX_FORMAT_HDLR_s *hdlr,UINT8 master);


/**
 * @}
 */

#endif

