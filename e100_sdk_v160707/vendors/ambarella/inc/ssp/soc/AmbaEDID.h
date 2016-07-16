/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaEDID.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for VESA Enahnced EDID Standard
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_EDID_H_
#define _AMBA_EDID_H_

/*-----------------------------------------------------------------------------------------------*\
 * VESA Enahnced Display Data Channel (E-DDC) Standard
\*-----------------------------------------------------------------------------------------------*/
#define DDC_ADDR_SET_SEGMENT_POINTER        (0x60)  /* I2C slave address to write segment pointer */
#define DDC_ADDR_SET_WORD_OFFSET            (0xA0)  /* I2C slave address to write word offset of the segment */
#define DDC_ADDR_GET_EDID_BLOCK             (0xA1)  /* I2C slave address to read one EDID block  */

/*-----------------------------------------------------------------------------------------------*\
 * VESA Enhanced Extended Display Identification Data (E-EDID) Standard
\*-----------------------------------------------------------------------------------------------*/
#define EDID_NUM_BLOCKS                     (256)   /* Maximum number of EDID blocks (E-DDC) */
#define EDID_BLOCK_SIZE                     (128)
#define EDID_SEGMENT_SIZE                   (2 * EDID_BLOCK_SIZE)

/*-----------------------------------------------------------------------------------------------*\
 * CEA 861 Extension Block
\*-----------------------------------------------------------------------------------------------*/
#define HDMI_VSDB_HDMI_VIDEO_PRESENT        0x20
#define HDMI_VSDB_I_LATENCY_FIELDS_PRESENT  0x40
#define HDMI_VSDB_LATENCY_FIELDS_PRESENT    0x80

typedef enum _EDID_CEA861_CEA_DATA_BLOCK_TYPE_e_ {
    EDID_CEA861_TAG_AUDIO                   = 1,    /* Tag Code of Audio Data Block */
    EDID_CEA861_TAG_VIDEO                   = 2,    /* Tag Code of Video Data Block */
    EDID_CEA861_TAG_VENDOR                  = 3,    /* Tag Code of Vendor Specific Data Block (VSDB) */
    EDID_CEA861_TAG_SPEAKER                 = 4,    /* Tag Code of Speaker Allocation Data Block */
    EDID_CEA861_TAG_VESA_DTC                = 5,    /* Tag Code of VESA DTC Data Block */
    EDID_CEA861_TAG_EXTENDED                = 7     /* Use Extended Tag */
} EDID_CEA861_CEA_DATA_BLOCK_TYPE_e;

#define EDID_CEA861_YUV_422_SUPPORTED       0x10    /* Set if sink supports YCbCr 4:2:2 in addition to RGB */
#define EDID_CEA861_YUV_444_SUPPORTED       0x20    /* Set if sink supports YCbCr 4:4:4 in addition to RGB */
#define EDID_CEA861_BASIC_AUDIO_SUPPORTED   0x40    /* Set if sink supports basic audio (2 channel LPCM, fs 32/44.1/48 kHz) */
#define EDID_CEA861_UNDERSCAN_IT_BY_DEFAULT 0x80    /* Set if sink underscans IT video formats by default */

typedef  struct _EDID_CEA861_EXT_V3_FORMAT_s_ {
    UINT8 Tag;                              /* 00h: Tag (0x02) */
    UINT8 RevisionNumber;                   /* 01h: Revision Number (0x03) */
    UINT8 OffsetDTD;                        /* 02h: Byte offset of Detailed Timing Descriptors */
    UINT8 NumOfDTD;                         /* 03h: The number of Detailed Timing Descriptors with some indication flags */
    UINT8 Data[123];                        /* 04h-7Eh: Data Block Collection & DTDs & Padding */
    UINT8 Checksum;                         /* 7Fh: Checksum */
}  EDID_CEA861_EXT_V3_FORMAT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Base EDID Version 1 Revision 3 Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _EDID_EXTENSION_TAG_e_ {
    EDID_EXT_CEA                            = 0x02, /* CEA 861 Series Extension */
    EDID_EXT_VTB                            = 0x10, /* Video Timing Block Extension */
    EDID_EXT_DI                             = 0x40, /* Display Information Extension */
    EDID_EXT_LS                             = 0x50, /* Localized String Extension */
    EDID_EXT_DPVL                           = 0x60, /* Digital Packet Video Link Extension */
    EDID_EXT_BLOCK_MAP                      = 0xF0, /* Extension Block Map */
    EDID_EXT_MANUFACTURER                   = 0xFF  /* Extensions defined by the display manufacturer */
} EDID_EXTENSION_TAG_e;

typedef struct _EDID_FORMAT_s_ {
    UINT8 HeaderInfo[8];                    /* 00h-07h: Header = (00 FF FF FF FF FF FF 00)h */
    UINT16 IdManufacturerName;              /* 08h-09h: ISA 3-character ID code */
    UINT16 IdProductCode;                   /* 0Ah-0Bh: Vendor assigned code */
    UINT32 IdSerialNumber;                  /* 0Ch-0Fh: 32-bit Serial Number */
    UINT8 WeekOfManufacture;                /* 10h: Week number or Model Year Flag */
    UINT8 YearOfManufacture;                /* 11h: Manufacture Year of Model Year */
    UINT8 VersionNumber;                    /* 12h: EDID structure version number */
    UINT8 RevisionNumber;                   /* 13h: EDID structure revision number */
    UINT8 VideoInputDefinition;             /* 14h: Information for host to configure its video output */
    UINT8 HorizontalScreenSize;             /* 15h: Listed in cm. */
    UINT8 VerticalScreenSize;               /* 16h: Listed in cm. */
    UINT8 Gamma;                            /* 17h: Factory Default Value (FFh if stored in extension block) */
    UINT8 FeatureSupport;                   /* 18h: The field used to indicate support for various display features */
    UINT8 LowOrderBitsRedGreen;             /* 19h: Red-x   [1:0], Red-y   [1:0], Green-x [1:0], Green-y [1:0] */
    UINT8 LowOrderBitsBlueWhite;            /* 1Ah: Blue-x  [1:0], Blue-y  [1:0], White-x [1:0], White-y [1:0] */
    UINT8 HighOrderBitsRedX;                /* 1Bh: Red-x   [9:2] */
    UINT8 HighOrderBitsRedY;                /* 1Ch: Red-y   [9:2] */
    UINT8 HighOrderBitsGreenX;              /* 1Dh: Green-x [9:2] */
    UINT8 HighOrderBitsGreenY;              /* 1Eh: Green-y [9:2] */
    UINT8 HighOrderBitsBlueX;               /* 1Fh: Blue-x  [9:2] */
    UINT8 HighOrderBitsBlueY;               /* 20h: Blue-y  [9:2] */
    UINT8 HighOrderBitsWhiteX;              /* 21h: White-x [9:2] */
    UINT8 HighOrderBitsWhiteY;              /* 22h: White-y [9:2] */
    UINT8 EstablishedTimings[3];            /* 23h-25h: Optional indication of Factory Supported Video Timing Modes */
    UINT16 StandardTimings[8];              /* 26h-35h: Optional timing identifier of Factory Supported Video Timing Modes */
    UINT8 Descriptor[4][18];                /* 36h-7Dh: 18 Byte Detailed timing descriptor/Display descriptor/Dummy descriptor */
    UINT8 ExtensionBlockCount;              /* 7Eh: Number of 128-byte EDID Extension blocks to follow */
    UINT8 Checksum;                         /* 7Fh: The 1-byte makes the sum of all 128 bytes in this EDID block equal zero */
} EDID_FORMAT_s;

#endif /* _AMBA_EDID_H_ */
