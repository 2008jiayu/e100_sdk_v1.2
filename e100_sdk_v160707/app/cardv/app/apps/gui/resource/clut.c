/**
  * @file src/app/apps/gui/resource/connectedcam/clut.c
  *
  *  CLUT Table
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
#include "gui_resource.h"
#include "clut.h"
#include <AmbaUtility.h>


static UINT32 clut_AYUV[256] = {0}; /* CLUT of AYUV color format */

UINT8 clut[256*3] = {
    255,   0, 255,    // 0    Clear
    255, 255, 255,    // 000    White
    252, 252, 252,
    250, 250, 250,
    248, 248, 248,
    246, 246, 246,
    244, 244, 244,    // 005
    242, 242, 242,
    239, 239, 239,
    237, 237, 237,
    234, 234, 234,
    232, 232, 232,    // 010
    229, 229, 229,
    226, 226, 226,
    223, 223, 223,
    220, 220, 220,
    217, 217, 217,    // 015
    214, 214, 214,
    211, 211, 211,
    208, 208, 208,
    204, 204, 204,
    201, 201, 201,    // 020
    198, 198, 198,    // 021    Light gray
    194, 194, 194,
    190, 190, 190,
    187, 187, 187,
    183, 183, 183,    // 025
    180, 180, 180,
    176, 176, 176,
    172, 172, 172,
    168, 168, 168,
    165, 165, 165,    // 030
    161, 161, 161,
    157, 157, 157,    // 032    Thumb gray
    153, 153, 153,
    149, 149, 149,    // 034    Menu background
    145, 145, 145,    // 035
    141, 141, 141,
    137, 137, 137,
    133, 133, 133,
    129, 129, 129,
    126, 126, 126,    // 040
    122, 122, 122,    // 041    Dark gray
    118, 118, 118,
    114, 114, 114,
    110, 110, 110,
    106, 106, 106,    // 045
    102, 102, 102,
    100, 100, 100,
     98,  98,  98,
     94,  94,  94,
     90,  90,  90,    // 050
     87,  87,  87,
     83,  83,  83,    // 052    Text border
     79,  79,  79,
     75,  75,  75,
     72,  72,  72,    // 055
     68,  68,  68,
     65,  65,  65,
     61,  61,  61,
     57,  57,  57,
     54,  54,  54,    // 060
     51,  51,  51,
     47,  47,  47,
     44,  44,  44,
     35,  35,  35,
     29,  29,  29,    // 065
     23,  23,  23,
     18,  18,  18,
     13,  13,  13,
      9,   9,   9,
      5,   5,   5,    // 070
      0,   0,   0,    // 071    Black
      9,  79, 210,
     16,  89, 213,
     24, 100, 217,
     34, 113, 221,    // 075
     43, 126, 225,
     52, 137, 229,
     59, 147, 232,
     64, 154, 234,
      0,   0, 255,    // 080    Blue
     40,  40, 180,
     40,  40, 177,
     41,  40, 172,
     41,  40, 165,
     42,  40, 158,    // 085
     43,  40, 151,
     43,  40, 142,
     44,  40, 134,
     45,  40, 125,
     46,  40, 117,    // 090
     47,  41, 110,
     48,  42, 103,
     49,  43,  98,
     50,  45,  93,
     51,  47,  90,    // 095
     54,  54,  90,
     55,  58,  93,
     56,  63,  98,
     57,  69, 106,
     58,  76, 114,    // 100
     59,  83, 125,
     60,  90, 136,
     61,  98, 148,    // 103    Thumb blue
     62, 106, 160,
     63, 114, 173,    // 105
     65, 122, 185,
     66, 130, 198,
     68, 137, 209,
     71, 145, 220,
     73, 152, 229,    // 110
     76, 158, 237,
     80, 164, 243,
     84, 170, 245,
     86, 173, 247,
     89, 176, 249,    // 115
     95, 182, 249,
    102, 187, 249,
    109, 193, 249,
    117, 199, 249,
    124, 205, 249,    // 120
    132, 210, 249,
    139, 215, 249,
    152, 223, 249,
    158, 226, 249,
    162, 229, 249,    // 125
    166, 230, 249,
    168, 231, 249,
    169, 232, 249,
    168, 230, 246,
    166, 227, 241,    // 130
    163, 223, 235,
    159, 217, 228,
    154, 211, 220,
    148, 204, 212,
    142, 196, 203,    // 135
    135, 189, 194,
    128, 181, 186,
    120, 174, 178,
    113, 167, 170,
    106, 161, 164,    // 140
     99, 156, 158,
     92, 152, 154,
     85, 150, 151,
     80, 150, 150,
     74, 152, 151,    // 145
     69, 155, 153,
     63, 160, 157,
     57, 166, 162,
     52, 172, 169,
     46, 180, 176,    // 150
     40, 188, 183,
     35, 197, 191,
     29, 206, 200,
     24, 215, 208,
     19, 223, 216,    // 155
     15, 231, 224,
     10, 239, 231,
      7, 245, 237,
      3, 250, 242,
    255, 180, 180,    // 160
    255, 174, 174,
    255, 165, 165,
    255, 155, 155,
    255, 144, 144,
    255, 132, 132,    // 165
    255, 118, 118,
    255,  91,  91,
    255,  77,  77,
    255,  63,  63,
    255,  50,  50,    // 170
    255,  38,  38,
    255,  26,  26,
    255,  17,  17,
    255,   8,   8,
    255,   0,   0,    // 175    Red
    253,   0,   0,
    245,   0,   0,
    237,   0,   0,
    227,   0,   0,
    215,   0,   0,    // 180    Warning
    203,   0,   0,
    191,   1,   1,
    178,   5,   5,
    164,   9,   9,
    151,  14,  14,    // 185
    138,  19,  19,
    126,  23,  23,
    114,  28,  28,
    103,  32,  32,
     94,  35,  35,    // 190
     89,  32,  32,
    180, 255, 180,
    173, 255, 173,
    165, 255, 165,
    154, 255, 154,    // 195
    143, 255, 143,
    130, 255, 130,
    116, 255, 116,
    102, 255, 102,
     87, 255,  87,    // 200
     73, 255,  73,
     59, 255,  59,
     46, 255,  46,
     33, 255,  33,
     22, 255,  22,    // 205
     13, 255,  13,
      5, 255,   5,
      0, 255,   0,    // 208    Green
      0, 249,   0,
      0, 241,   0,    // 210
      0, 231,   0,
      0, 220,   0,
      0, 208,   0,
      0, 195,   0,
      4, 181,   4,    // 215
     13, 154,  13,
     18, 141,  18,
     23, 128,  23,
     27, 116,  27,
     31, 104,  31,    // 220
     35,  95,  35,
     38,  86,  38,
     33,  83,  33,
    255, 128,   0,
    255, 139,   0,    // 225
    255, 154,   0,
    255, 172,   0,
    255, 192,   0,
    255, 211,   0,
    255, 230,   0,    // 230
    255, 245,   0,
    255, 255,   0,    // 232    Yellow
    255, 255,  15,
    255, 255,  36,
    255, 255,  62,    // 235
    255, 255,  90,
    255, 255, 118,
    255, 255, 143,
    255, 255, 165,
    200, 180, 255,    // 240
    190, 159, 255,
    174, 127, 255,
    157,  91, 255,
    140,  54, 255,
    130,  22, 255,    // 245
    128,   0, 255,
    138,   0, 255,
    159,   0, 255,
    186,   0, 255,
    213,   0, 255,    // 250
    238,   0, 255,
    255,  80, 170,
    186,   0, 186,    // 253    Magenta
      0,   0,   0
};

UINT8 blending[256] = {
     0, 15, 15, 15, 15, 15, 15, 15,        // 0   -   7
    15, 15, 15, 15, 15, 15, 15, 15,        // 8   -  15
    15, 15, 15, 15, 15, 15, 15, 15,        // 16  -  23
    15, 15, 15, 15, 15, 15, 15, 15,        // 24  -  31
    15, 15, 15, 15, 15, 15, 15, 15,        // 32  -  39
    15, 15, 15, 15, 15, 15, 15, 15,        // 40  -  47
    15, 15, 15, 15, 15, 15, 15, 15,        // 48  -  55
    15, 15, 15, 15, 15, 15, 15, 15,        // 56  -  63
    15, 15, 15, 15, 15, 15, 15, 15,        // 64  -  71
    15, 15, 15, 15, 15, 15, 15, 15,        // 72  -  79
    15, 15, 15, 15, 15, 15, 15, 15,        // 80  -  87
    15, 15, 15, 15, 15, 15, 15, 15,        // 88  -  95
    15, 15, 15, 15, 15, 15, 15, 15,        // 96  - 103
    15, 15, 15, 15, 15, 15, 15, 15,        // 104 - 111
    15, 15, 15, 15, 15, 15, 15, 15,        // 112 - 119
    15, 15, 15, 15, 15, 15, 15, 15,        // 120 - 127
    15, 15, 15, 15, 15, 15, 15, 15,        // 128 - 135
    15, 15, 15, 15, 15, 15, 15, 15,        // 136 - 143
    15, 15, 15, 15, 15, 15, 15, 15,        // 144 - 151
    15, 15, 15, 15, 15, 15, 15, 15,        // 152 - 159
    15, 15, 15, 15, 15, 15, 15, 15,        // 160 - 167
    15, 15, 15, 15, 15, 15, 15, 15,        // 168 - 175
    15, 15, 15, 15, 15, 15, 15, 15,        // 176 - 183
    15, 15, 15, 15, 15, 15, 15, 15,        // 184 - 191
    15, 15, 15, 15, 15, 15, 15, 15,        // 192 - 199
    15, 15, 15, 15, 15, 15, 15, 15,        // 200 - 207
    15, 15, 15, 15, 15, 15, 15, 15,        // 208 - 215
    15, 15, 15, 15, 15, 15, 15, 15,        // 216 - 223
    15, 15, 15, 15, 15, 15, 15, 15,        // 224 - 231
    15, 15, 15, 15, 15, 15, 15, 15,        // 232 - 239
    15, 15, 15, 15, 15, 15, 15, 15,        // 240 - 247
    15, 15, 15, 15, 15, 15, 15, 15        // 248 - 255
};

#if 0
static void print_clutAYUV(void)
{
    UINT32 entryNum;
    UINT32 i;

    entryNum = sizeof(blending);

    AmbaPrintColor(GREEN,"UINT8 clut_AYUV[256*4] = {");
    for (i=0;i<entryNum-1;i++) {
        AmbaPrintColor(GREEN,"%3d, %3d, %3d, %3d, // %3d",clut_AYUV[i] & 0XFF,
                            (clut_AYUV[i] >> 8) & 0XFF,
                            (clut_AYUV[i] >> 16) & 0XFF,
                            (clut_AYUV[i] >> 24) & 0XFF, i);
    }
    AmbaPrintColor(GREEN,"%3d, %3d, %3d, %3d}; // %3d",clut_AYUV[i] & 0XFF,
                            (clut_AYUV[i] >> 8) & 0XFF,
                            (clut_AYUV[i] >> 16) & 0XFF,
                            (clut_AYUV[i] >> 24) & 0XFF, i);
}
#endif

static int save_clutAYUV_as_file(void)
{
#if 0
    FF_FILE *pfile;
    char *fn = "C:\\clutAYUV.c";
    char clutEntry[32];
    UINT32 clutEntrySize;
    UINT32 entryNum;
    UINT32 i;

    clutEntrySize = sizeof(clutEntry);
    entryNum = sizeof(blending);

    pfile = ff_fopen(fn, "w+");
    if (pfile == NULL) {
        AmbaPrintColor(GREEN, "Open file '%s' fail.",fn);
        return -1;
    }

    snprintf(clutEntry,clutEntrySize,"UINT8 clut_AYUV[256*4] = {\n");
    ff_fwrite(clutEntry, strlen(clutEntry), 1, pfile);
    for (i=0;i<entryNum-1;i++) {
        snprintf(clutEntry,clutEntrySize,"    %3d, %3d, %3d, %3d, // %3d\n",
                            clut_AYUV[i] & 0XFF,
                            (clut_AYUV[i] >> 8) & 0XFF,
                            (clut_AYUV[i] >> 16) & 0XFF,
                            (clut_AYUV[i] >> 24) & 0XFF, i);
        ff_fwrite(clutEntry, strlen(clutEntry), 1, pfile);
    }

    snprintf(clutEntry,clutEntrySize,"    %3d, %3d, %3d, %3d // %3d\n};\n",
                            clut_AYUV[i] & 0XFF,
                            (clut_AYUV[i] >> 8) & 0XFF,
                            (clut_AYUV[i] >> 16) & 0XFF,
                            (clut_AYUV[i] >> 24) & 0XFF, i);
    ff_fwrite(clutEntry, strlen(clutEntry), 1, pfile);
    ff_fclose(pfile);
#endif
    return 0;
}

static int save_clutAYUV_as_bin_file(void)
{
    AMBA_FS_FILE *File = NULL;
    char *fn = "C:\\clutAYUV.bin";
    WCHAR mode[3] = {'w','b','\0'};
    WCHAR TempFn[64] = {0};

    AmbaUtility_Ascii2Unicode("C:\\clutAYUV.bin", TempFn);

    File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);
    if (File == NULL) {
        AmbaPrintColor(GREEN, "Open file '%s' fail.",fn);
        return -1;
    }



    AmbaFS_fwrite(clut_AYUV, sizeof(clut_AYUV), 1, File);
    AmbaFS_fclose(File);

    return 0;
}


static UINT8 clip(UINT8 value, UINT8 max, UINT8 min)
{
    UINT8 clippedValue;

    if (value > max) {
        clippedValue = max;
    } else if (value < min) {
        clippedValue = min;
    } else {
        clippedValue = value;
    }

    return clippedValue;
}


/**
* Calculate the entry of CLUT / palette
*/
static UINT32 cal_clut_entry(UINT8 *ptr, UINT8 blend_factor, UINT8 yuv)
{
    UINT32 table;
    UINT8  tab[3];

    if (!yuv) {
        /* tab[0] = 0.299 * ptr[0] + 0.587 * ptr[1] + 0.114 * ptr[2]; */
        tab[0] = (5016387*ptr[0]+9848226*ptr[1]+1912603*ptr[2]) >> 24;

        /* tab[1] = (ptr[2] - tab[0]) * 0.565; */
        if (tab[0] >= ptr[2]) {
            tab[1] = ((128 << 24)-(tab[0]-ptr[2])*9479127) >> 24 ;
        } else {
            tab[1] = ((ptr[2]-tab[0])*9479127+(128 << 24)) >> 24;
        }


        /* tab[2] = (ptr[0] - tab[0]) * 0.713; */
        if (tab[0] >= ptr[0]) {
            tab[2] = ((128 << 24)-(tab[0]-ptr[0])*11962155) >> 24 ;
        } else {
            tab[2] = ((ptr[0]-tab[0])*11962155+(128 << 24)) >> 24;
        }

#if 0   /* 16 ~ 235 */
        tab[0] = clip(tab[0], 235, 16);
        tab[1] = clip(tab[1], 240, 16);
        tab[2] = clip(tab[2], 240, 16);
#else   /* 0 ~ 255 */
        tab[0] = clip(tab[0], 255, 0);
        tab[1] = clip(tab[1], 255, 0);
        tab[2] = clip(tab[2], 255, 0);
#endif
        /* adjust alpha value*/
        if (blend_factor) {
            blend_factor = 255;
        }

        table  = ((blend_factor & 0xff) << 24) +
                ((tab[0] & 0xff) << 16) +
                ((tab[1] & 0xff) << 8) +
                (tab[2] & 0xff);
        } else {
        table = ((blend_factor & 0xff) << 24) +
                ((ptr[0] & 0xff) << 16) +
                ((ptr[1] & 0xff) << 8) +
                (ptr[2] & 0xff);
    }

    return (table);
}

int convert_ARGB_To_AYUV(void)
{
    UINT8 *ptrRGB;
    UINT32 entryNum;
    UINT32 i;

    ptrRGB = clut;
    entryNum = sizeof(blending);

    for (i=0;i<entryNum;i++) {
        clut_AYUV[i] = cal_clut_entry(ptrRGB, blending[i], 0);
        ptrRGB += 3;
    }

    //print_clutAYUV();
    save_clutAYUV_as_file();
    save_clutAYUV_as_bin_file();

    return 0;
}

