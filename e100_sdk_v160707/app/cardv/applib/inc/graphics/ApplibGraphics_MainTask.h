/**
 * @file app/connected/applib/inc/graphics/inc/ApplibGraphics_MainTask.h
 *
 * ApplibGraphics_MainTask.h only for Amba Graphics main task internal use
 *
 * History:
 *    2015/06/24 - [Yuchi Wei] created file from 'src/graphics/inc/ApplibGraphics_MainTask.h'
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

#ifndef _GRAPHICS_MAINTASK_H_
#define _GRAPHICS_MAINTASK_H_

#include "ApplibGraphics_Common.h"

/*************************************************************************
 * Enum
 ************************************************************************/
/**
 * The definition of channel.
 */
typedef enum _GRAPHICS_CHANNEL_e_ {
    GRAPHICS_CHANNEL_F = 0,
    GRAPHICS_CHANNEL_D,
    GRAPHICS_CHANNEL_BLEND,
    GRAPHICS_CHANNEL_NUM,
} GRAPHICS_CHAN_e;

/*************************************************************************
 * Struct
 ************************************************************************/

/*************************************************************************
 * Variable
 ************************************************************************/

/*************************************************************************
 * Define
 ************************************************************************/
#define GRAPHICS_INIT_NONE      (0)
#define GRAPHICS_INIT_MW_OSD    (1 << 1)
#define GRAPHICS_INIT_MW_WINDOW (1 << 2)
#define GRAPHICS_INIT_FONT      (1 << 3)
#define GRAPHICS_INIT_GRAPHICS  (1 << 4)

/*************************************************************************
 * Function
 ************************************************************************/

#endif // _GRAPHICS_MAINTASK_H_
