/**
  * @file src/app/apps/gui/resource/connectedcam/clut_ayuv8888.h
  *
  *  CLUT - Color definitions for AYUV 8888 format
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
#ifndef APP_CLUT_AYUV32BIT_H_
#define APP_CLUT_AYUV32BIT_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * GUI color definitions
 ************************************************************************/
/** Alpha part */
#define COLOR_AYUV32BIT_ALPHA_MASK        (0xFF000000)
#define COLOR_AYUV32BIT_ALPHA_OFFSET    (24)

/** color part */
#define COLOR_AYUV32BIT_COLOR_MASK        (0x00FFFFFF)
#define COLOR_AYUV32BIT_COLOR_OFFSET    (0)

#define COLOR_AYUV32BIT_BLACK    (0xFF008080)
#define COLOR_AYUV32BIT_RED        (0xFF4C54F0)
#define COLOR_AYUV32BIT_GREEN    (0xFF952B15)
#define COLOR_AYUV32BIT_BLUE    (0xFF1DEF67)
#define COLOR_AYUV32BIT_MAGENTA    (0xFF4CBDCD)
#define COLOR_AYUV32BIT_LIGHTGRAY    (0xFFC68080)
#define COLOR_AYUV32BIT_DARKGRAY    (0xFF7A8080)
#define COLOR_AYUV32BIT_YELLOW    (0xFFE11094)
#define COLOR_AYUV32BIT_WHITE    (0xFFEB8080)
#define COLOR_AYUV32BIT_THUMB_BLUE    (0xFF5C9F69)
#define COLOR_AYUV32BIT_THUMB_GRAY    (0xFF9D8080)
#define COLOR_AYUV32BIT_TEXT_BORDER    (0xFF538080)
#define COLOR_AYUV32BIT_MENU_BG    (0xFF958080)
#define COLOR_AYUV32BIT_WARNING    (0xFF405BF0)
#define COLOR_AYUV32BIT_CLR        (0x00008080)

__END_C_PROTO__

#endif /* APP_CLUT_AYUV32BIT_H_ */
