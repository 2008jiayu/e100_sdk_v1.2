/**
  * @file src/app/apps/gui/resource/connectedcam/clut.h
  *
  *  CLUT Table - 8 bit and include 16 bit
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
#ifndef APP_CLUT_H_
#define APP_CLUT_H_

__BEGIN_C_PROTO__

/*************************************************************************
 * GUI color definitions
 ************************************************************************/
/** Color info */
/**
* 8-bit color info is index of RGB clut
**/
#define COLOR_8BIT_BLACK        (71)
#define COLOR_8BIT_RED            (175)
#define COLOR_8BIT_GREEN        (208)
#define COLOR_8BIT_BLUE            (80)
#define COLOR_8BIT_MAGENTA        (253)
#define COLOR_8BIT_LIGHTGRAY        (21)
#define COLOR_8BIT_DARKGRAY        (41)
#define COLOR_8BIT_YELLOW        (232)
#define COLOR_8BIT_WHITE        (1)
#define COLOR_8BIT_THUMB_BLUE        (103)
#define COLOR_8BIT_THUMB_GRAY        (32)
#define COLOR_8BIT_TEXT_BORDER        (52)
#define COLOR_8BIT_MENU_BG        (34)
#define COLOR_8BIT_WARNING        (180)
#define COLOR_8BIT_CLR            (0) // Specified transparency color!!

extern UINT8 clut[256 * 3];
extern UINT8 blending[256];

__END_C_PROTO__

#endif /* APP_CLUT_H_ */
