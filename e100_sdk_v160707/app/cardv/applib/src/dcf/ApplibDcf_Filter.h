/**
 * @file src/app/connected/applib/src/dcf/ApplibDcf_Filter.h
 *
 * Header of DCF filter
 *
 * History:
 *    2013/11/6 - [YC Liao] created file
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
 * DCF Filter
 */
/**
 * The u32 difinition for index
 * base: bit 31~27 (magic number, to avoid app passes an incorrect id)
 * dnum: bit 26~17 (dnum <= 999, so 3FF is enough)
 * fnum: bit 16~0 (2FFF is enough for 9999, but user defined mode uses 5 digit)
 *  ---------------------------
 *  | base  |  dnum  |  fnum  |
 *  ---------------------------
 *  | 5bits | 10bits | 17bits |
 *  ---------------------------
 */
#define INDEX_BASE          0x40000000    /* File index basement */
#define DCF_BASE_MASK       0x0000001F
#define DCF_DNUM_MASK       0x000003FF
#define DCF_FNUM_MASK       0x0001FFFF
#define DCF_BASE_SHIFT      27
#define DCF_DNUM_SHIFT      17

#define DCF_DIR_PFX_LEN     0
#define DCF_DIR_NUM_LEN     3
#define DCF_DIR_SFX_LEN     5
#define DCF_DIR_SFX_STR     "MEDIA"

#define DCF_FILE_PFX_LEN    4
#define DCF_FILE_PFX_STR    "AMBA"
#define DCF_FILE_NUM_LEN    4
#define DCF_FILE_SFX_LEN    0
#define DCF_FILE_THM_LEN    4
#define DCF_FILE_THM_STR    "_thm"

#define DCF_DIR_NUM_MIN     100
#define DCF_DIR_NUM_MAX     999     // hard limit : 999
#define DCF_FILE_NUM_MIN    1
#define DCF_FILE_NUM_MAX    999     // hard limit: 9999

#define DCF_SEQ_NUM_LEN     2
#define DCF_SEQ_NUM_MAX     99

#define DCF_PATH_DELIMITER  '\\'

#define DCF_IS_DELIM(x) ((x == '\\') || (x == '/'))
#define DCF_IS_DIGIT(x) ((x >= '0')&&(x <= '9'))
#define DCF_IS_CHAR(x)  (((x >= 'a')&&(x <= 'z')) || ((x >= 'A')&&(x <= 'Z')) || (x == '_'))

extern AMP_DCF_FILTER_s g_AppLibDcfFlt1;

