/**
 * @file src/app/connected/applib/src/display/ApplibCsc.c
 *
 * csc matrix utility
 *
 * History:
 *    2014/03/27 - [Eric Yen] created file
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

#include <display/Display.h>
#include <system/ApplibSys_Lcd.h>
#include <applib.h>
//#include <AmbaHDMI.h>

//#define DEBUG_APPLIB_CSC
#if defined(DEBUG_APPLIB_CSC)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif


/*************************************************************************
 * Csc Tables
 ************************************************************************/
typedef enum _APPLIB_CSC_TABLE_IDX_HDMI_e_ {
/* output to [255,0] become vivid since stretch out
 * R/G/B [0...255]
 *     Y[16...235]
 * Cb/Cr[16...240]
 *
 * HDMI TV should never use
   SD:
   1.16 -0.39 -0.81    + 135
   1.16  2.02     0    - 277
   1.16     0   1.6    - 223

   HD:
   1.16 -0.21 -0.53    + 76
   1.16  2.11     0    - 289
   1.16     0  1.79    - 249
 */
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_PCSD = 0,
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_PCHD,

/* PC-->PC or TV->TV domain
 * R/G/B[16...235]  R/ G/ B[0...255]
 *     Y[16...235] or
 * Cb/Cr[16...240]  Y/Cb/Cr[0...255]
 *
 * SD, YUV601
 * HD, YUV709
   SD:
   1  -0.34  -0.7    + 132
   1   1.73     0    - 222
   1      0  1.37    - 176

   HD:
   1  -0.18 -0.46    + 82
   1   1.82     0    - 232
   1      0  1.54    - 197
 */
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_ISD,
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_IHD,

/* src [255,0]->[235,16] ,
 * output to [235,16] become lighter since clamp to 16
 * LCD should never use
   SD:
   0.92  -0.31  -0.64   + 121
   0.92   1.59      0   - 204
   0.92      0    1.3   - 161

   HD:
   0.92 -0.16  -0.4    + 75
   0.92  1.67     0    - 213
   0.92     0  1.41    - 181
   { 0x1f5c03ae, 0x03ae1e66, 0x000006ae,
    0x000003ae, 0x004b0599, 0x7f4b7f2b,
    0x00eb0010, 0x00f00010, 0x00f00010 },

   0.92  - 0.18  - 0.46
   0.92    1.82    0.
   0.92    0.      1.54

 */
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_TVSD,
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_TVHD,

/* src [n,m]->[n,m]
   1  0  0
   0  1  0
   0  0  1
 */
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2YCbCrI = 6,
    APPLIB_CSC_TABLE_IDX_HDMI_RGB2RGBI       = 6,
    APPLIB_CSC_TABLE_IDX_HDMI_RGB2YCBCR_PCSD,
    APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2YCbCrI422
} APPLIB_CSC_TABLE_IDX_HDMI_e;

/* LCD shares same value with HDMI except clampping
 * However, LCD should 'not' use
 *  LCD_YCbCr2RGB_HDPC
 *  LCD_YCbCr2RGB_HDI
 *  LCD_YCbCr2RGB_HDTV and LCD_YCbCr2RGB_SDTV
 *  define for clarity
 */
typedef enum _APPLIB_CSC_TABLE_IDX_LCD_e_ {
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_PCSD = 0,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_PCHD,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_ISD,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_IHD,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_TVSD,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_TVHD,
    APPLIB_CSC_TABLE_IDX_LCD_KODA1,
    APPLIB_CSC_TABLE_IDX_LCD_KODA2,
    APPLIB_CSC_TABLE_IDX_LCD_KODA3,
    APPLIB_CSC_TABLE_IDX_LCD_CUSTOMIZE1,
    APPLIB_CSC_TABLE_IDX_LCD_CUSTOMIZE2,
    APPLIB_CSC_TABLE_IDX_LCD_YCBCR2YCBCRI = 11,
    APPLIB_CSC_TABLE_IDX_LCD_RGB2RGBI = 11,
    APPLIB_CSC_TABLE_IDX_LCD_RGB2YCBCR_PCSD
} APPLIB_CSC_TABLE_IDX_LCD_e;

typedef enum _APPLIB_CSC_TABLE_IDX_TV_e_ {
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_PCSD = 0,
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_PCHD,
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_ISD,    /* VO_CSC_ANALOG_SD */
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_IHD,    /* VO_CSC_ANALOG_HD */
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_TVSD,
    APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_TVHD,
} APPLIB_CSC_TABLE_IDX_TV_e;


#if 0   //original A7l csc
INT32 TvCsc[6][6] = {

    { 0x04000400, 0x00000400, 0x00000000,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x027102e9, 0x00c80271, 0x00c700c7,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    { 0x04000400, 0x00000400, 0x00000000,   /* VO_CSC_ANALOG_SD */
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x02710280, 0x00d40271, 0x00c700c7,   /* VO_CSC_ANALOG_HD */
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    { 0x04000370, 0x000e0400, 0x00000000,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x02710226, 0x00de0271, 0x00c700c7,
      0x00ff0000, 0x00ff0000, 0x00ff0000 }
    };

INT32 LCDCsc[13][9] = {
    { 0x1e6e04a7, 0x04a71cbf, 0x00000811,
      0x000004a7, 0x00870662, 0x7f217eeb,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x1f2504a7, 0x04a71dde, 0x00000872,
      0x000004a7, 0x004c072c, 0x7f077edf,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    { 0x1ea70400, 0x04001d35, 0x000006ed,
      0x00000400, 0x0084057b, 0x7f507f22,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x1f450400, 0x04001e2a, 0x00000743,
      0x00000400, 0x00520629, 0x7f3b7f1b,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    { 0x1eca0383, 0x03831d7d, 0x00000639,
      0x0000036f, 0x008604d0, 0x7f757f48,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x1f5a0383, 0x03831e63, 0x00000661,
      0x0000036f, 0x0058054b, 0x7f667f43,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },


    // 6 KODA customize1
    { 0x1a6705BB, 0x05BB17c7, 0x1efa0fb9,
      0x1dde05BB, 0x01850d14, 0x7e6a7df3,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // 7 KODA customize2
    { 0x1d2805BB, 0x05BB1af2, 0x1fbf0b5f,
      0x1f7805BB, 0x00ad0925, 0x7e9c7e4d,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // 8 KODA customize3
    { 0x1da804b9, 0x04b91bd6, 0x1fca095f,
      0x1f9004b9, 0x00af0789, 0x7efb7eb9,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // 9 customize4 slightly enhanced in luma
    { 0x1e6f04a8, 0x04a81cbf, 0x1fff0812,
      0x1ffe04a8, 0x00880662, 0x7f217eeb,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    /*
    10 customize 5 with Sat = 0.8 (Sat=2 is not fitting to our matrix)notice that
    p3 is great on objects but looks really strange on skin color.
    I think Sat=0.2 or Sat=0.4 will be better.
    */
    { 0x1a6705BB, 0x05BB17c7, 0x1efa0fb9,
      0x1dde05BB, 0x01850d14, 0x7e6a7df3,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // 11
    { 0x00000400, 0x00000000, 0x00000400,
      0x00000000, 0x00000400, 0x00000000,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // 12
    { 0x00740259, 0x1ead0132, 0x1f520200,
    0x1fad1e52, 0x00000200, 0x00800080,
    0x00ff0000, 0x00ff0000, 0x00ff0000 }
};

INT32 HDMICsc[9][9] = {
    { 0x1e6e04a7, 0x04a71cbf, 0x00000811,
      0x000004a7, 0x00870662, 0x7f217eeb,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },
    { 0x1f2504a7, 0x04a71dde, 0x00000872,
      0x000004a7, 0x004c072c, 0x7f077edf,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    { 0x1ea70400, 0x04001d35, 0x000006ed,
      0x00000400, 0x0084057b, 0x7f507f22,
      0x00eb0010, 0x00f00010, 0x00f00010 },
    { 0x1f450400, 0x04001e2a, 0x00000743,
      0x00000400, 0x00520629, 0x7f3b7f1b,
      0x00eb0010, 0x00f00010, 0x00f00010 },

    { 0x1eca0383, 0x03831d7d, 0x00000639,
      0x0000036f, 0x008604d0, 0x7f757f48,
      0x00eb0010, 0x00f00010, 0x00f00010 },
    { 0x1f4703ae, 0x03ae1e28, 0x00000747,
      0x000003ae, 0x00510628, 0x7f3a7f17,
      0x00eb0010, 0x00f00010, 0x00f00010 },

    { 0x00000400, 0x00000000, 0x00000400,
      0x00000000, 0x00000400, 0x00000000,
      0x00eb0010, 0x00f00010, 0x00f00010 },

    { 0x00740259, 0x1ead0132, 0x1f520200,
      0x1fad1e52, 0x00000200, 0x00800080,
      0x00ff0000, 0x00ff0000, 0x00ff0000 },

    // YUV422 output 6.6 in output
    { 0x00000400, 0x00000000, 0x00000400,
        0x00000000, 0x00000400, 0x00000000,
        0x0eb00100, 0x0f000100, 0x0f000100 },
};

//converting function
static void CscBitsToParam(int VoutType, INT32 *pCscParam)
{
#define GetFloatToInt(Val)  ((Val >= 0.0) ? ((INT32)(Val)) : (0 - (INT32)(-Val)))
#define GetIntToFloat(Val) ((Val > 0x1FFFFFFF) ? ((float)(0 - (float)(~Val))) : ((float)(Val)))
#define GetIntToFloat13(Val) ((Val > 0xFFF) ? ((float)(0 - (float)(~(Val | 0xFFFFF000)))) : ((float)(Val)))
#define GetIntToFloat15(Val) ((Val > 0x3FFF) ? ((float)(0 - (float)(~(Val | 0xFFFFC000)))) : ((float)(Val)))
    AMBA_DSP_VOUT_CSC_CONFIG_s CscMatrix = {0};
    switch (VoutType) {
    case 0: //lcd
        CscMatrix.Component[0].Coefficient[0] = GetIntToFloat13((pCscParam[0] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].Coefficient[1] = GetIntToFloat13(((pCscParam[0] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[0].Coefficient[2] = GetIntToFloat13((pCscParam[1] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].LowerBound = pCscParam[6] & 0xFFF;
        CscMatrix.Component[0].UpperBound = (pCscParam[6] & 0xFFF0000) >> 16;
        CscMatrix.Component[0].Constant = GetIntToFloat15(((pCscParam[4] & 0x7FFF0000) >> 16));
        CscMatrix.Component[1].Coefficient[0] = GetIntToFloat13(((pCscParam[1] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].Coefficient[1] = GetIntToFloat13((pCscParam[2] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[1].Coefficient[2] = GetIntToFloat13(((pCscParam[2] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].LowerBound = pCscParam[7] & 0xFFF;
        CscMatrix.Component[1].UpperBound = (pCscParam[7] & 0xFFF0000) >> 16;
        CscMatrix.Component[1].Constant = GetIntToFloat15((pCscParam[5] & 0x7FFF));
        CscMatrix.Component[2].Coefficient[0] = GetIntToFloat13((pCscParam[3] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].Coefficient[1] = GetIntToFloat13(((pCscParam[3] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[2].Coefficient[2] = GetIntToFloat13((pCscParam[4] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].LowerBound = pCscParam[8] & 0xFFF;
        CscMatrix.Component[2].UpperBound = (pCscParam[8] & 0xFFF0000) >> 16;
        CscMatrix.Component[2].Constant = GetIntToFloat15(((pCscParam[5] & 0x7FFF0000) >> 16));
        break;
    case 1: //analog
        CscMatrix.Component[0].Coefficient[0] = GetIntToFloat13((pCscParam[0] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].LowerBound = pCscParam[3] & 0x3FF;
        CscMatrix.Component[0].UpperBound = (pCscParam[3] & 0x3FF0000) >> 16;
        CscMatrix.Component[0].Constant = GetIntToFloat15(((pCscParam[1] & 0x7FFF0000) >> 16));
        CscMatrix.Component[1].Coefficient[0] = GetIntToFloat13(((pCscParam[0] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].LowerBound = pCscParam[4] & 0x3FF;
        CscMatrix.Component[1].UpperBound = (pCscParam[4] & 0x3FF0000) >> 16;
        CscMatrix.Component[1].Constant = GetIntToFloat15((pCscParam[2] & 0x7FFF));
        CscMatrix.Component[2].Coefficient[0] = GetIntToFloat13((pCscParam[1] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].LowerBound = pCscParam[5] & 0x3FF;
        CscMatrix.Component[2].UpperBound = (pCscParam[5] & 0x3FF0000) >> 16;
        CscMatrix.Component[2].Constant = GetIntToFloat15(((pCscParam[2] & 0x7FFF0000) >> 16));
        break;
    case 2: //hdmi 6.6
        CscMatrix.Component[0].Coefficient[0] = GetIntToFloat13((pCscParam[0] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].Coefficient[1] = GetIntToFloat13(((pCscParam[0] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[0].Coefficient[2] = GetIntToFloat13((pCscParam[1] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].LowerBound = pCscParam[6] & 0xFFF;
        CscMatrix.Component[0].UpperBound = (pCscParam[6] & 0xFFF0000) >> 16;
        CscMatrix.Component[0].Constant = GetIntToFloat15(((pCscParam[4] & 0x7FFF0000) >> 16)) / 16.0;
        CscMatrix.Component[1].Coefficient[0] = GetIntToFloat13(((pCscParam[1] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].Coefficient[1] = GetIntToFloat13((pCscParam[2] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[1].Coefficient[2] = GetIntToFloat13(((pCscParam[2] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].LowerBound = pCscParam[7] & 0xFFF;
        CscMatrix.Component[1].UpperBound = (pCscParam[7] & 0xFFF0000) >> 16;
        CscMatrix.Component[1].Constant = GetIntToFloat15((pCscParam[5] & 0x7FFF)) / 16.0;
        CscMatrix.Component[2].Coefficient[0] = GetIntToFloat13((pCscParam[3] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].Coefficient[1] = GetIntToFloat13(((pCscParam[3] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[2].Coefficient[2] = GetIntToFloat13((pCscParam[4] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].LowerBound = pCscParam[8] & 0xFFF;
        CscMatrix.Component[2].UpperBound = (pCscParam[8] & 0xFFF0000) >> 16;
        CscMatrix.Component[2].Constant = GetIntToFloat15(((pCscParam[5] & 0x7FFF0000) >> 16)) / 16.0;
        break;
    case 3: //hdmi 2.10
        CscMatrix.Component[0].Coefficient[0] = GetIntToFloat13((pCscParam[0] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].Coefficient[1] = GetIntToFloat13(((pCscParam[0] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[0].Coefficient[2] = GetIntToFloat13((pCscParam[1] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[0].LowerBound = pCscParam[6] & 0xFFF;
        CscMatrix.Component[0].UpperBound = (pCscParam[6] & 0xFFF0000) >> 16;
        CscMatrix.Component[0].Constant = GetIntToFloat15(((pCscParam[4] & 0x7FFF0000) >> 16));
        CscMatrix.Component[1].Coefficient[0] = GetIntToFloat13(((pCscParam[1] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].Coefficient[1] = GetIntToFloat13((pCscParam[2] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[1].Coefficient[2] = GetIntToFloat13(((pCscParam[2] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[1].LowerBound = pCscParam[7] & 0xFFF;
        CscMatrix.Component[1].UpperBound = (pCscParam[7] & 0xFFF0000) >> 16;
        CscMatrix.Component[1].Constant = GetIntToFloat15((pCscParam[5] & 0x7FFF));
        CscMatrix.Component[2].Coefficient[0] = GetIntToFloat13((pCscParam[3] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].Coefficient[1] = GetIntToFloat13(((pCscParam[3] & 0x1FFF0000) >> 16)) / 1024.0;
        CscMatrix.Component[2].Coefficient[2] = GetIntToFloat13((pCscParam[4] & 0x1FFF)) / 1024.0;
        CscMatrix.Component[2].LowerBound = pCscParam[8] & 0xFFF;
        CscMatrix.Component[2].UpperBound = (pCscParam[8] & 0xFFF0000) >> 16;
        CscMatrix.Component[2].Constant = GetIntToFloat15(((pCscParam[5] & 0x7FFF0000) >> 16));
        break;
    }
    AmbaPrint("    .Component[0] = { .Coefficient = { %f, %f, %f}, .Constant =  %f, .LowerBound = %u, .UpperBound = %u },",
              CscMatrix.Component[0].Coefficient[0], CscMatrix.Component[0].Coefficient[1], CscMatrix.Component[0].Coefficient[2],
              CscMatrix.Component[0].Constant, CscMatrix.Component[0].LowerBound, CscMatrix.Component[0].UpperBound);
    AmbaPrint("    .Component[1] = { .Coefficient = { %f, %f, %f}, .Constant =  %f, .LowerBound = %u, .UpperBound = %u },",
              CscMatrix.Component[1].Coefficient[0], CscMatrix.Component[1].Coefficient[1], CscMatrix.Component[1].Coefficient[2],
              CscMatrix.Component[1].Constant, CscMatrix.Component[1].LowerBound, CscMatrix.Component[1].UpperBound);
    AmbaPrint("    .Component[2] = { .Coefficient = { %f, %f, %f}, .Constant =  %f, .LowerBound = %u, .UpperBound = %u },",
              CscMatrix.Component[2].Coefficient[0], CscMatrix.Component[2].Coefficient[1], CscMatrix.Component[2].Coefficient[2],
              CscMatrix.Component[2].Constant, CscMatrix.Component[2].LowerBound, CscMatrix.Component[2].UpperBound);
//    typedef union _p4_u_ {
//        UINT32  Data;
//
//        struct {
//            INT32   CoefA8:             13;     /* [12:0] Signed 6.6 bits for HDMI Output Mode 2, Signed 2.10 bits otherwise */
//            INT32   Reserved:           3;      /* [15:13] Reserved */
//            INT32   ConstB0:            15;     /* [30:16] Signed 14 bits for HDMI Output Mode 2, Signed 10 bits otherwise */
//            INT32   Reserved1:          1;      /* [31] Reserved */
//        } Bits;
//    } p4_u;
//
//    p4_u MyParam4 = {0};
//
//    typedef union _p0_u_ {
//        UINT32  Data;
//
//        struct {
//            INT32   CoefA0:             13;     /* [12:0] Signed 6.6 bits for HDMI Output Mode 2, Signed 2.10 bits otherwise */
//            INT32   Reserved:           3;      /* [15:13] Reserved */
//            INT32   CoefA1:             13;     /* [28:16] Signed 6.6 bits for HDMI Output Mode 2, Signed 2.10 bits otherwise */
//            INT32   Reserved1:          3;      /* [31:29] Reserved */
//        } Bits;
//    } p0_u;
//    float MyUfloat0;
//    float MyUfloat1;
//    float MyUfloat2 = -221.7;
//    float MyUfloat3 = -2.77;
//    p0_u MyParam0 = {0};
//    AmbaPrint("0 %f (0x%x)", MyUfloat2, &MyUfloat2);
//    MyParam4.Bits.ConstB0 = (GetFloatToInt(MyUfloat2) << 4);
//    AmbaPrint("1 %x (0x%x)", MyParam4.Bits.ConstB0, &MyParam4);
//    MyUfloat1 = GetIntToFloat15(((MyParam4.Data & 0x7FFF0000) >> 16)) / 16.0;
//    AmbaPrint("2 %f (0x%x)", MyUfloat1, &MyUfloat1);
//
//    AmbaPrint("3 %f (0x%x)", MyUfloat3, &MyUfloat3);
//    MyParam0.Bits.CoefA0 = GetFloatToInt((MyUfloat3*1024));
//    AmbaPrint("4 %x (0x%x)", MyParam0.Bits.CoefA0, &MyParam0);
//    MyUfloat0 = GetIntToFloat13((MyParam0.Data & 0x1FFF)) / 1024.0;
//    AmbaPrint("5 %f (0x%x)", MyUfloat0, &MyUfloat0);
//    AmbaPrint("3 %f (0x%x)", MyUfloat3, &MyUfloat3);
}

int AmpUT_Display_cscP(struct _AMBA_SHELL_ENV_s_ *env, int argc, char **argv)
{
    int CscIdx;

    AmbaPrint("TvCsc");
    for (CscIdx=0; CscIdx<6; CscIdx++) {
        AmbaPrint("TvCsc[%d]", CscIdx);
        AmbaPrint("[%d] = { ", CscIdx);
        CscBitsToParam(1, &TvCsc[CscIdx][0]);
        AmbaPrint("},");
    }
    AmbaPrint("LCDCsc");
    for (CscIdx=0; CscIdx<13; CscIdx++) {
        AmbaPrint("[%d] = { ", CscIdx);
        CscBitsToParam(0, &LCDCsc[CscIdx][0]);
        AmbaPrint("},");
    }
    AmbaPrint("HDMICsc");
    for (CscIdx=0; CscIdx<9; CscIdx++) {
        AmbaPrint("[%d] = { ", CscIdx);
        if (CscIdx > 6)
            CscBitsToParam(2, &HDMICsc[CscIdx][0]);
        else
            CscBitsToParam(3, &HDMICsc[CscIdx][0]);
        AmbaPrint("},");
    }
    return 0;
}

#endif

static AMBA_DSP_VOUT_CSC_CONFIG_s TvCscTable[6] = {
    [0] = { //TVE_YCBCR2YPBPR_PCSD
        .Component[0] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
    },

    [1] = { //TVE_YCBCR2YPBPR_PCHD
        .Component[0] = { .Coefficient = { 0.727539, 0.000000, 0.000000}, .Constant =  200.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
    },

    [2] = { //TVE_YCBCR2YPBPR_ISD
        .Component[0] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
    },

    [3] = { //TVE_YCBCR2YPBPR_IHD
        .Component[0] = { .Coefficient = { 0.625000, 0.000000, 0.000000}, .Constant =  212.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
    },

    [4] = { //TVE_YCBCR2YPBPR_TVSD
        .Component[0] = { .Coefficient = { 0.859375, 0.000000, 0.000000}, .Constant =  14.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
    },

    [5] = { //TVE_YCBCR2YPBPR_TVHD
        .Component[0] = { .Coefficient = { 0.537109, 0.000000, 0.000000}, .Constant =  222.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.610352, 0.000000, 0.000000}, .Constant =  199.000000, .LowerBound = 0, .UpperBound = 255 },
    },
};

static AMBA_DSP_VOUT_CSC_CONFIG_s LcdCscTable[13] = {
    [0] = { //LCD_YCBCR2RGB_PCSD
        .Component[0] = { .Coefficient = { 1.163086, -0.391602, -0.812500}, .Constant =  135.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.163086, 2.016602, 0.000000}, .Constant =  -276.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.163086, 0.000000, 1.595703}, .Constant =  -222.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [1] = { //LCD_YCBCR2RGB_PCHD
        .Component[0] = { .Coefficient = { 1.163086, -0.212891, -0.532227}, .Constant =  76.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.163086, 2.111328, 0.000000}, .Constant =  -288.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.163086, 0.000000, 1.792969}, .Constant =  -248.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [2] = { //LCD_YCBCR2RGB_ISD
        .Component[0] = { .Coefficient = { 1.000000, -0.335938, -0.697266}, .Constant =  132.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.000000, 1.731445, 0.000000}, .Constant =  -221.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 1.370117}, .Constant =  -175.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [3] = { //LCD_YCBCR2RGB_IHD
        .Component[0] = { .Coefficient = { 1.000000, -0.181641, -0.458008}, .Constant =  82.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.000000, 1.815430, 0.000000}, .Constant =  -228.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 1.540039}, .Constant =  -196.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [4] = { //LCD_YCBCR2RGB_TVSD
        .Component[0] = { .Coefficient = { 0.877930, -0.301758, -0.626953}, .Constant =  134.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.877930, 1.555664, 0.000000}, .Constant =  -183.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.858398, 0.000000, 1.203125}, .Constant =  -138.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [5] = { //LCD_YCBCR2RGB_TVHD
        .Component[0] = { .Coefficient = { 0.877930, -0.161133, -0.402344}, .Constant =  88.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.877930, 1.594727, 0.000000}, .Constant =  -188.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.858398, 0.000000, 1.323242}, .Constant =  -153.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [6] = { //LCD_KODA1
        .Component[0] = { .Coefficient = { 1.432617, -1.398438, -2.054688}, .Constant =  389.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.432617, 3.930664, -0.254883}, .Constant =  -524.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.432617, -0.532227, 3.269531}, .Constant =  -405.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [7] = { //LCD_KODA2
        .Component[0] = { .Coefficient = { 1.432617, -0.709961, -1.262695}, .Constant =  173.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.432617, 2.842773, -0.062500}, .Constant =  -434.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.432617, -0.131836, 2.286133}, .Constant =  -355.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [8] = { //LCD_KODA3
        .Component[0] = { .Coefficient = { 1.180664, -0.584961, -1.040039}, .Constant =  175.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.180664, 2.342773, -0.051758}, .Constant =  -326.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.180664, -0.108398, 1.883789}, .Constant =  -260.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [9] = { //LCD_CUSTOMIZE1
        .Component[0] = { .Coefficient = { 1.164062, -0.390625, -0.812500}, .Constant =  136.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.164062, 2.017578, 0.000000}, .Constant =  -276.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.164062, -0.000977, 1.595703}, .Constant =  -222.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [10] = { //LCD_CUSTOMIZE2
        .Component[0] = { .Coefficient = { 1.432617, -1.398438, -2.054688}, .Constant =  389.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.432617, 3.930664, -0.254883}, .Constant =  -524.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.432617, -0.532227, 3.269531}, .Constant =  -405.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [11] = { //LCD_YCBCR2YCBCRI, LCD_RGB2RGBI
        .Component[0] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 0.000000, 1.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 0.000000, 0.000000, 1.000000}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [12] = { //LCD_RGB2YCBCR_PCSD
        .Component[0] = { .Coefficient = { 0.586914, 0.113281, 0.298828}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { -0.330078, 0.500000, -0.168945}, .Constant =  128.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { -0.418945, -0.080078, 0.500000}, .Constant =  128.000000, .LowerBound = 0, .UpperBound = 255 },
    },
};

static AMBA_DSP_VOUT_CSC_CONFIG_s HdmiCscTable[9] = {
    [0] = { //HDMI_YCBCR2RGB_PCSD
        .Component[0] = { .Coefficient = { 1.163086, -0.391602, -0.812500}, .Constant =  135.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.163086, 2.016602, 0.000000}, .Constant =  -276.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.163086, 0.000000, 1.595703}, .Constant =  -222.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [1] = { //HDMI_YCBCR2RGB_PCHD
        .Component[0] = { .Coefficient = { 1.163086, -0.212891, -0.532227}, .Constant =  76.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { 1.163086, 2.111328, 0.000000}, .Constant =  -288.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { 1.163086, 0.000000, 1.792969}, .Constant =  -248.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [2] = { //HDMI_YCBCR2RGB_ISD
        .Component[0] = { .Coefficient = { 1.000000, -0.335938, -0.697266}, .Constant =  132.000000, .LowerBound = 16, .UpperBound = 235 },
        .Component[1] = { .Coefficient = { 1.000000, 1.731445, 0.000000}, .Constant =  -221.000000, .LowerBound = 16, .UpperBound = 240 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 1.370117}, .Constant =  -175.000000, .LowerBound = 16, .UpperBound = 240 },
    },
    [3] = { //HDMI_YCBCR2RGB_IHD
        .Component[0] = { .Coefficient = { 1.000000, -0.181641, -0.458008}, .Constant =  82.000000, .LowerBound = 16, .UpperBound = 235 },
        .Component[1] = { .Coefficient = { 1.000000, 1.815430, 0.000000}, .Constant =  -228.000000, .LowerBound = 16, .UpperBound = 240 },
        .Component[2] = { .Coefficient = { 1.000000, 0.000000, 1.540039}, .Constant =  -196.000000, .LowerBound = 16, .UpperBound = 240 },
    },
    [4] = { //HDMI_YCBCR2RGB_TVSD
        .Component[0] = { .Coefficient = { 0.877930, -0.301758, -0.626953}, .Constant =  134.000000, .LowerBound = 16, .UpperBound = 235 },
        .Component[1] = { .Coefficient = { 0.877930, 1.555664, 0.000000}, .Constant =  -183.000000, .LowerBound = 16, .UpperBound = 240 },
        .Component[2] = { .Coefficient = { 0.858398, 0.000000, 1.203125}, .Constant =  -138.000000, .LowerBound = 16, .UpperBound = 240 },
    },
    [5] = { //HDMI_YCBCR2RGB_TVHD
        .Component[0] = { .Coefficient = { 0.919922, -0.179688, -0.459961}, .Constant =  81.000000, .LowerBound = 16, .UpperBound = 235 },
        .Component[1] = { .Coefficient = { 0.919922, 1.819336, 0.000000}, .Constant =  -232.000000, .LowerBound = 16, .UpperBound = 240 },
        .Component[2] = { .Coefficient = { 0.919922, 0.000000, 1.539062}, .Constant =  -197.000000, .LowerBound = 16, .UpperBound = 240 },
    },
    [6] = { //HDMI_YCBCR2YCbCrI, HDMI_RGB2RGBI
        .Component[0] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 16, .UpperBound = 235 },
        .Component[1] = { .Coefficient = { 0.000000, 1.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 16, .UpperBound = 240 },
        .Component[2] = { .Coefficient = { 0.000000, 0.000000, 1.000000}, .Constant =  0.000000, .LowerBound = 16, .UpperBound = 240 },
    },
    [7] = { //HDMI_RGB2YCBCR_PCSD
        .Component[0] = { .Coefficient = { 0.586914, 0.113281, 0.298828}, .Constant =  0.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[1] = { .Coefficient = { -0.330078, 0.500000, -0.168945}, .Constant =  8.000000, .LowerBound = 0, .UpperBound = 255 },
        .Component[2] = { .Coefficient = { -0.418945, -0.080078, 0.500000}, .Constant =  8.000000, .LowerBound = 0, .UpperBound = 255 },
    },
    [8] = { //HDMI_YCBCR2YCbCrI422
        .Component[0] = { .Coefficient = { 1.000000, 0.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 256, .UpperBound = 3760 },
        .Component[1] = { .Coefficient = { 0.000000, 1.000000, 0.000000}, .Constant =  0.000000, .LowerBound = 256, .UpperBound = 3840 },
        .Component[2] = { .Coefficient = { 0.000000, 0.000000, 1.000000}, .Constant =  0.000000, .LowerBound = 256, .UpperBound = 3840 },
    },
};


/*************************************************************************
 * Display APIs - Static
 ************************************************************************/



/*************************************************************************
 * Display APIs
 ************************************************************************/
/**
 *  @brief Get csc matrix
 *
 *  Get csc matrix
 *
 *  @param [in] VoutType Type of vout device @see AMBA_DSP_VOUT_TYPE_e
 *  @param [in] ColorSpaceIn Input color space @see AMBA_DSP_COLOR_SPACE_e
 *  @param [in] ColorSpaceOut Output color space @see AMBA_DSP_COLOR_SPACE_e
 *  @param [in] QRange Quantization range @see AMP_DISP_QUANTIZATION_RANGE_e
 *  @param [in] IsHDOutput Is output HD resolution, a.k.a. ActiveRowHeight >= 720
 *  @param [out] pOutMatrix Output csc matrix
 *
 *  @return frame rate
 */
int AppLibCsc_GetMatrix(AMP_DISP_DEV_IDX_e VoutType,
                        AMBA_DSP_COLOR_SPACE_e ColorSpaceIn,
                        AMBA_DSP_COLOR_SPACE_e ColorSpaceOut,
                        APPLIB_CSC_QUANTIZATION_RANGE_e QRangeIn,
                        APPLIB_CSC_QUANTIZATION_RANGE_e QRangeOut,
                        UINT8 IsHDOutput,
                        AMBA_DSP_VOUT_CSC_CONFIG_s *pOutMatrix)
{
    AMBA_DSP_VOUT_CSC_CONFIG_s *pCscSrc;

    if (pOutMatrix == NULL) {
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }

    switch ((UINT8)VoutType) {
    case AMP_DISP_LCD:
        if (ColorSpaceIn == ColorSpaceOut) {
            pCscSrc = &LcdCscTable[APPLIB_CSC_TABLE_IDX_LCD_YCBCR2YCBCRI];
        } else if ((ColorSpaceIn == AMBA_DSP_COLOR_SPACE_RGB) && (ColorSpaceOut == AMBA_DSP_COLOR_SPACE_YUV)) {
            pCscSrc = &LcdCscTable[APPLIB_CSC_TABLE_IDX_LCD_YCBCR2YCBCRI];
        } else if ((ColorSpaceIn == AMBA_DSP_COLOR_SPACE_YUV) && (ColorSpaceOut == AMBA_DSP_COLOR_SPACE_RGB)) {
            if (QRangeIn == QRangeOut) {
                pCscSrc = &LcdCscTable[APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_ISD];
            } else if (QRangeIn == APPLIB_CSC_QUANTIZATION_RANGE_LIMITED) {
                pCscSrc = &LcdCscTable[APPLIB_CSC_TABLE_IDX_LCD_YCBCR2RGB_PCSD];
            } else {
                return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
            }
        } else {
            return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
        }
        break;
    case AMP_DISP_CVBS:
        if (IsHDOutput == 1) {
            if (QRangeIn == QRangeOut) {
                pCscSrc = &TvCscTable[APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_IHD];
            } else if (QRangeIn == APPLIB_CSC_QUANTIZATION_RANGE_LIMITED) {
                return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
            } else {
                pCscSrc = &TvCscTable[APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_TVHD];
            }
        } else {
            if (QRangeIn == QRangeOut) {
                pCscSrc = &TvCscTable[APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_ISD];
            } else if (QRangeIn == APPLIB_CSC_QUANTIZATION_RANGE_LIMITED) {
                return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
            } else {
                pCscSrc = &TvCscTable[APPLIB_CSC_TABLE_IDX_TV_YCBCR2YPBPR_TVSD];
            }
        }
        break;
    case AMP_DISP_HDMI:
        if (ColorSpaceIn == ColorSpaceOut) {
            pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2YCbCrI];
//        } else if ((ColorSpaceIn == AMBA_DSP_COLOR_SPACE_YUV) && (ColorSpaceOut == AMBA_DSP_COLOR_SPACE_YUV422)) {
//            pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2YCbCrI422];
        } else if ((ColorSpaceIn == AMBA_DSP_COLOR_SPACE_RGB) && (ColorSpaceOut == AMBA_DSP_COLOR_SPACE_YUV)) {
            pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_RGB2YCBCR_PCSD];
        } else if ((ColorSpaceIn == AMBA_DSP_COLOR_SPACE_YUV) && (ColorSpaceOut == AMBA_DSP_COLOR_SPACE_RGB)) {
            if (IsHDOutput == 1) {
                if (QRangeIn == QRangeOut) {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_IHD];
                } else if (QRangeIn == APPLIB_CSC_QUANTIZATION_RANGE_LIMITED) {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_PCHD];
                } else {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_TVHD];
                }
            } else {
                if (QRangeIn == QRangeOut) {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_ISD];
                } else if (QRangeIn == APPLIB_CSC_QUANTIZATION_RANGE_LIMITED) {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_PCSD];
                } else {
                    pCscSrc = &HdmiCscTable[APPLIB_CSC_TABLE_IDX_HDMI_YCBCR2RGB_TVSD];
                }
            }
        } else {
            return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
        }
        break;
    default:
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
        break;
    }
    memcpy(pOutMatrix, pCscSrc, sizeof(AMBA_DSP_VOUT_CSC_CONFIG_s));
    return AMP_OK;
}
