/**
  * @file src/app/apps/gui/resource/connectedcam/gui_resource.h
  *
  * Header for GUI resource
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mw.h>
#include <AmbaDataType.h>
#include "clut.h"
#include "clut_ayuv4444.h"
#include "clut_ayuv8888.h"
#include "clut_argb8888.h"
#include "strings.h"
#include "bitmaps.h"
#include <applib.h>

#ifndef APP_GUI_RESOURCE_H_
#define APP_GUI_RESOURCE_H_


__BEGIN_C_PROTO__

/*************************************************************************
 * GUI resource pixel type
 ************************************************************************/
extern int gui_pixel_type;
#define GUI_RESOURCE_8BIT    (0)
#define GUI_RESOURCE_16BIT    (1)
#define GUI_RESOURCE_32BIT_AYUV    (2)
#define GUI_RESOURCE_32BIT_ARGB    (3)
#define GUI_RESOURCE_TYPE_NUM    (4)
extern int gui_tv_layout;

/*************************************************************************
 * GUI font resource
 ************************************************************************/
#if    defined(ENABLE_ARPHIC_LIB) || defined(ENABLE_FREETYPE_LIB)
#define GUI_VECTOR_FONT        (1)
#endif

#if defined(GUI_VECTOR_FONT)
#define GUI_FONT_H_DEFAULT    (36)
#else
#define GUI_FONT_H_DEFAULT    (54)
#endif


/*************************************************************************
 * GUI string resource
 ************************************************************************/


/*************************************************************************
 * GUI bitmap resource
 ************************************************************************/

/*************************************************************************
 * GUI default colors
 ************************************************************************/
extern UINT32 COLOR_BLACK;
extern UINT32 COLOR_RED;
extern UINT32 COLOR_GREEN;
extern UINT32 COLOR_BLUE;
extern UINT32 COLOR_MAGENTA;
extern UINT32 COLOR_LIGHTGRAY;
extern UINT32 COLOR_DARKGRAY;
extern UINT32 COLOR_YELLOW;
extern UINT32 COLOR_WHITE;
extern UINT32 COLOR_THUMB_BLUE;
extern UINT32 COLOR_THUMB_GRAY;
extern UINT32 COLOR_TEXT_BORDER;
extern UINT32 COLOR_MENU_BG;
extern UINT32 COLOR_WARNING;
extern UINT32 COLOR_CLR;

/*************************************************************************
 * GUI table resource
 ************************************************************************/
#define GUI_TABLE_STATIC    (0)    // Static GUI tables
#define GUI_TABLE_ROMFS        (1)    // GUI tables from ROMFS
#define GUI_TABLE_SOURCE    GUI_TABLE_STATIC

__END_C_PROTO__

#endif /* APP_GUI_RESOURCE_H__ */
