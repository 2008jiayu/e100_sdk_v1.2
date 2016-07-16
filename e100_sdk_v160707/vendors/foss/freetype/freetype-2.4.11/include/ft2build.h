/***************************************************************************/
/*                                                                         */
/*  ft2build.h                                                             */
/*                                                                         */
/*    FreeType 2 build and setup macros.                                   */
/*    (Ambarella version)                                                  */
/*                                                                         */
/*  Copyright 1996-2001, 2006 by                                           */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* This file corresponds to the default `ft2build.h' file for            */
  /* FreeType 2.  It uses the `freetype' include root.                     */
  /*                                                                       */
  /* Note that specific platforms might use a different configuration.     */
  /* See builds/unix/ft2unix.h for an example.                             */
  /*                                                                       */
  /*************************************************************************/


#ifndef _FT2_BUILD_H_
#define _FT2_BUILD_H_

#define FT_CONFIG_CONFIG_H  "freetype/config/ftconfig.h"
#define FT_CONFIG_STANDARD_LIBRARY_H  "freetype/config/ftstdlib.h"
#define FT_CONFIG_OPTIONS_H  "freetype/config/ftoption.h"
#define FT_CONFIG_MODULES_H  "freetype/config/ftmodule.h"

#include "freetype/config/ftheader.h"

#endif /* __FT2_BUILD_AMBARELLA_H__ */


/* END */
