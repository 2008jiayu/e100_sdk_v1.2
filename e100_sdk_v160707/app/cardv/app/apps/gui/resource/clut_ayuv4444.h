/**
  * @file src/app/apps/gui/resource/connectedcam/clut_ayuv4444.h
  *
  *  CLUT - Color definitions for AYUV 4444 format
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
#ifndef APP_CLUT_16BIT_H_
#define APP_CLUT_16BIT_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * GUI color definitions
 ************************************************************************/
/** Alpha part */
#define COLOR_16BIT_ALPHA_MASK        (0xF000)
#define COLOR_16BIT_ALPHA_OFFSET    (12)

/** color part */
#define COLOR_16BIT_COLOR_MASK        (0x0FFF)
#define COLOR_16BIT_COLOR_OFFSET    (0)

#define COLOR_16BIT_BLACK    (0xF188)
#define COLOR_16BIT_RED        (0xF56F)
#define COLOR_16BIT_GREEN    (0xF930)
#define COLOR_16BIT_BLUE    (0xF2F6)
#define COLOR_16BIT_MAGENTA    (0xF5BE)
#define COLOR_16BIT_LIGHTGRAY    (0xFC88)
#define COLOR_16BIT_DARKGRAY    (0xF888)
#define COLOR_16BIT_YELLOW    (0xFE1A)
#define COLOR_16BIT_WHITE    (0xFF88)
#define COLOR_16BIT_THUMB_BLUE    (0xF3B5)
#define COLOR_16BIT_THUMB_GRAY    (0xFA88)
#define COLOR_16BIT_TEXT_BORDER    (0xF588)
#define COLOR_16BIT_MENU_BG    (0xF988)
#define COLOR_16BIT_WARNING    (0xF46F)
#define COLOR_16BIT_CLR        (0x0188)

__END_C_PROTO__

#endif /* APP_CLUT_16BIT_H_ */
