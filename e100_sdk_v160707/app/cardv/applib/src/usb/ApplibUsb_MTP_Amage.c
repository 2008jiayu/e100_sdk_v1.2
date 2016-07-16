/**
 * @file src/app/connected/applib/src/usb/ApplibUsb_MTP_Amage.c
 *
 *  Amage USB MTP class functions.
 *
 * History:
 *    2013/12/03 - [Martin Lai] created file
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

#include <applib.h>
#include <usb/AmbaUSB_API.h>
#include "ApplibUsb_MTP_Amage.h"
#include <AmbaCardManager.h>
#include <AmbaFS.h>
#include <AmbaPrintk.h>
#include <AmbaTUNE_TextHdlr.h>
#include <AmbaTUNE_USBHdlr.h>
#include "AmbaTUNE_HdlrManager.h"


//#define DEBUG_APPLIB_USB_MTP_AMAGE
#if defined(DEBUG_APPLIB_USB_MTP_AMAGE)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/* PIMA MTP names ... */
UINT8 MtpDeviceInfoVendorName[]  =      "Ambarella";
UINT8 MtpDeviceInfoProductName[] =      "A9 MTP Device";
UINT8 MtpDeviceInfoSerialNo[]    =      "0.0.0.1";
UINT8 MtpDeviceInfoVersion[]      =      "V1.0";

/* PIMA MTP storage names.  */
UINT8 MtpVolumeDescription[]       =   "A9 MTP Client Disk Volume";
UINT8 MtpVolumeLabel[]             =   "A9 MTP Client SD slot";
UINT8 ObjectPropDatasetDataBuffer[2];

MTP_CLASS_INFO_s ApplibUsbMtpClassInfo = {0};

#if 0
__no_init MTP_PARSE_OBJECT_INFO_s MtpParseObjectInfo;
#endif
MTP_OBJECT_INFO_s MtpObjectInfo __attribute__((section (".bss.noinit")));
MTP_OBJECT_s  MtpTmpObject __attribute__((section (".bss.noinit")));

static UINT16 DeviceSupportProp[] = {

        MTP_DEV_PROP_BATTERY_LEVEL,
        MTP_DEV_PROP_DATE_TIME,
        MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG,
        MTP_DEV_PROP_SYNCHRONIZATION_PARTNER,
        MTP_DEV_PROP_DEVICE_FRIENDLY_NAME,
        MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO,
        MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE,
        0
};

static UINT16 DeviceSupportCaptureFormat[] = {
        0
};

static UINT16 DeviceSupportImgFormat[] =  {

        MTP_OFC_UNDEFINED,
        MTP_OFC_ASSOCIATION,
        MTP_OFC_MP3,
        MTP_OFC_EXIF_JPEG,
        0
};

static UINT16 ObjectSupportProp[] = {

        /* Object format code : Undefined.  */
        MTP_OFC_UNDEFINED,

        /* NUmber of objects supported for this format.  */
        1,
        /* Mandatory objects for all formats.  */
        MTP_OBJECT_PROP_STORAGEID,
        MTP_OBJECT_PROP_OBJECT_FORMAT,
        MTP_OBJECT_PROP_PROTECTION_STATUS,
        MTP_OBJECT_PROP_OBJECT_SIZE,
        MTP_OBJECT_PROP_OBJECT_FILE_NAME,
        MTP_OBJECT_PROP_PARENT_OBJECT,
        MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        MTP_OBJECT_PROP_NAME,
        MTP_OBJECT_PROP_NON_CONSUMABLE,

        /* Object format code : Association.  */
        MTP_OFC_ASSOCIATION,

        /* NUmber of objects supported for this format.  */
        1,
        /* Mandatory objects for all formats.  */
        MTP_OBJECT_PROP_STORAGEID,
        MTP_OBJECT_PROP_OBJECT_FORMAT,
        MTP_OBJECT_PROP_PROTECTION_STATUS,
        MTP_OBJECT_PROP_OBJECT_SIZE,
        MTP_OBJECT_PROP_OBJECT_FILE_NAME,
        MTP_OBJECT_PROP_PARENT_OBJECT,
        MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        MTP_OBJECT_PROP_NAME,
        MTP_OBJECT_PROP_NON_CONSUMABLE,

        /* Object format code : Audio Clip.  */
        MTP_OFC_MP3,

        /* NUmber of objects supported for this format.  */
        1,
        /* Mandatory objects for all formats.  */
        MTP_OBJECT_PROP_STORAGEID,
        MTP_OBJECT_PROP_OBJECT_FORMAT,
        MTP_OBJECT_PROP_PROTECTION_STATUS,
        MTP_OBJECT_PROP_OBJECT_SIZE,
        MTP_OBJECT_PROP_OBJECT_FILE_NAME,
        MTP_OBJECT_PROP_PARENT_OBJECT,
        MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        MTP_OBJECT_PROP_NAME,
        MTP_OBJECT_PROP_NON_CONSUMABLE,

        /* Mandatory objects for all audio objects.  */
        MTP_OBJECT_PROP_ARTIST,
        MTP_OBJECT_PROP_TRACK,
        MTP_OBJECT_PROP_DATE_AUTHORED,
        MTP_OBJECT_PROP_GENRE,
        MTP_OBJECT_PROP_USE_COUNT,
        MTP_OBJECT_PROP_ALBUM_NAME,
        MTP_OBJECT_PROP_ALBUM_ARTIST,
        MTP_OBJECT_PROP_SAMPLE_RATE,
        MTP_OBJECT_PROP_NUMBER_OF_CHANNELS,
        MTP_OBJECT_PROP_AUDIO_WAVE_CODEC,
        MTP_OBJECT_PROP_AUDIO_BITRATE,

        /* Object format code : Windows Media Audio Clip.  */
        MTP_OFC_EXIF_JPEG,

        /* NUmber of objects supported for this format.  */
        1,
        /* Mandatory objects for all formats.  */
        MTP_OBJECT_PROP_STORAGEID,
        MTP_OBJECT_PROP_OBJECT_FORMAT,
        MTP_OBJECT_PROP_PROTECTION_STATUS,
        MTP_OBJECT_PROP_OBJECT_SIZE,
        MTP_OBJECT_PROP_OBJECT_FILE_NAME,
        MTP_OBJECT_PROP_PARENT_OBJECT,
        MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
        MTP_OBJECT_PROP_NAME,
        MTP_OBJECT_PROP_NON_CONSUMABLE,

        /* Mandatory objects for all image objects.  */
        MTP_OBJECT_PROP_WIDTH,
        MTP_OBJECT_PROP_HEIGHT,
        MTP_OBJECT_PROP_DATE_AUTHORED,

        0
    };

/* Device property dataset. Here we give the example of the Date/Time dataset.  */
#define DEVICE_PROP_DATE_TIME_DATASET_LENGTH 39
UINT8 DevicePropDataTimeDataset[] = {

    /* Device prop code : Date/Time.  */
        0x11, 0x50,                                         /* Prop code */
        0xff, 0xff,                                         /* String    */
        0x01,                                               /* GET/SET   */
        0x00,                                               /* Default value : empty string.  */
        0x10,                                               /* Current value : length of the unicode string.  */
        0x31, 0x00, 0x39, 0x00, 0x38, 0x00, 0x30, 0x00,     /* YYYY */
        0x30, 0x00, 0x31, 0x00,                             /* MM */
        0x30, 0x00, 0x31, 0x00,                             /* DD */
        0x54, 0x00,                                         /* T  */
        0x30, 0x00, 0x30, 0x00,                             /* HH */
        0x30, 0x00, 0x30, 0x00,                             /* MM */
        0x30, 0x00, 0x30, 0x00,                             /* SS */
        0x00, 0x00,                                         /* Unicode terminator.  */
        0x00                                                /* Form Flag : None.  */
};

/* Device property dataset. Here we give the example of the synchronization partner dataset.  */
#define DEVICE_PROP_SYNCHRONIZATION_PARTNER_DATASET_LENGTH 8
UINT8 DevicePropSynchronizationPartnerDataset[] = {

    /* Device prop code : Synchronization Partner.  */
        0x01, 0xD4,                                         /* Prop code */
        0xff, 0xff,                                         /* String    */
        0x01,                                               /* GET/SET   */
        0x00,                                               /* Default value : empty string.  */
        0x00,                                               /* Current value : empty string.  */
        0x00                                                /* Form Flag : None.  */
};

/* Device property dataset. Here we give the example of the device friendly name dataset.  */
#define DEVICE_PROP_DEVICE_FRIENDLY_NAME_DATASET_LENGTH 64
UINT8 DevicePropDeviceFriendlyNameDataset[] = {

    /* Device prop code : Device Friendly Name.  */
        0x02, 0xD4,                                         /* Prop code */
        0xff, 0xff,                                         /* String    */
        0x01,                                               /* GET/SET   */
        0x0E,                                               /* Default value.  Length of Unicode string. */
        0x41, 0x00, 0x6D, 0x00, 0x62, 0x00, 0x61, 0x00,     /* Unicode string.  */
        0x20, 0x00, 0x4D, 0x00, 0x54, 0x00, 0x50, 0x00,
        0x20, 0x00, 0x44, 0x00, 0x53, 0x00, 0x43, 0x00,
        0x20, 0x00,
        0x00, 0x00,                                         /* Unicode terminator. */
        0x0E,                                               /* Current value.  Length of Unicode string. */
        0x41, 0x00, 0x6D, 0x00, 0x62, 0x00, 0x61, 0x00,
        0x20, 0x00, 0x4D, 0x00, 0x54, 0x00, 0x50, 0x00,
        0x20, 0x00, 0x44, 0x00, 0x53, 0x00, 0x43, 0x00,     /* Unicode terminator.  */
        0x20, 0x00,
        0x00, 0x00,                                         /* Unicode terminator.  */
        0x00                                                /* Form Flag : None.  */
};

#define DEVICE_PROP_BATTERY_LEVEL_DATASET_LENGTH 11
UINT8 DevicePropBattteryLevelDataset[] = {

        0x01, 0x50,                                         /* Prop code */
        0x02, 0x00,                                         /* Data Type  */
        0x00,                                               /* GET/SET   */
        0x64,                                               /* Default value. */
        0x64,                                               /* Current value. */
        0x01,                                               /* Form Flag : Range.  */
        0x00,
        0x64,
        0x01
};


#define DEVICE_PROP_PERCEIVED_DEVICE_TYPE_DATASET_LENGTH 14
UINT8 DevicePropPrecivedDeviceTypeDataset[] = {

        0x07, 0xD4,                                         /* Prop code */
        0x06, 0x00,                                         /* Data Type  */
        0x00,                                               /* GET/SET   */
        0x01, 0x00, 0x00, 0x00,                             /* Default value. */
        0x01, 0x00, 0x00, 0x00,                             /* Current value. */
        0x00                                                /* Form Flag : None.  */
};

#define DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH 8
UINT8 DevicePropUseDeviceStageFlagDataset[] = {

        0x03, 0xD3,                                         /* Prop code */
        0x02, 0x00,                                         /* Data Type  */
        0x00,                                               /* GET/SET   */
        0x01,                                               /* Default value. */
        0x01,                                               /* Current value. */
        0x00,                                               /* Form Flag : Range.  */
};

#define IS_IMAGE_GROUP(x)   (x <= ApplibUsbMtpClassInfo.ImageNumber)

static VOID  MTP_UtilityShortPut(UINT8 * address, UINT16 value)
{
    *address++ =  (UINT8) (value & 0xff);
    *address = (UINT8) ((value >> 8) & 0xff);
    return;
}

static VOID  MTP_UtilityLongPut(UINT8 * address, UINT32 value)
{
    *address++ =  (UINT8) (value & 0xff);
    *address++ =  (UINT8) ((value >> 8) & 0xff);
    *address++ =  (UINT8) ((value >> 16) & 0xff);
    *address =      (UINT8) ((value >> 24) & 0xff);
    return;
}

static UINT32  MTP_UtilityStringLengthGet(UINT8 *string)
{
    UINT32  length =  0;

    length =  0;
    while (string[length]) {
        length++;
    }
    return(length);
}

VOID  MTP_UtilityUnicodeToString(UINT8 *source, UINT8 *destination)
{
    UINT32   StringLength;

    StringLength = (UINT32) *source++;

    while (StringLength--) {
        *destination++ = *source++;
        *source+=1;
    }
    *destination = 0;
    return;
}

VOID  MTP_UtilityStringToUnicode(UINT8 *source, UINT8 *destination)
{
    UINT32   StringLength;

    StringLength = MTP_UtilityStringLengthGet(source);

    if (StringLength == 0) {
        *destination = 0;
    } else {
        *destination++ = (UINT8) StringLength + 1;

        while (StringLength--) {
            *destination++ = *source++;
            *destination++ = 0;
        }
        *destination++ = 0;
        *destination++ = 0;
    }
    return;
}

static VOID  MTP_UtilityParseFileName(UINT8 *source, UINT8 *destination)
{
    UINT32   StringLength;
    UINT32   count = 0;

    StringLength = MTP_UtilityStringLengthGet(source);
    count = StringLength;

    if (StringLength == 0) {
        *destination = 0;
    } else {
        while (*(source+count-1) != '\\') {
            count--;
            if (count == 0) break;
        }
        if (count == 0) {
            AmbaPrint("NG");
            *destination = 0;
            return;
        }
        memcpy(destination, (char*)(source+count), (StringLength-count+1));
    }
    return;
}

#if 0

static UINT32 MTP_UtilityGetFileFormat(UINT32 HandleIndex)
{
    if (IS_IMAGE_GROUP(HandleIndex)) {
        return MTP_OFC_EXIF_JPEG;
    } else {
        return MTP_OFC_UNDEFINED;
    }
}

static VOID MTP_UtilityGetFullFileName(UINT32 HandleIndex, char* FullFileName)
{
    UINT32                  FileIndex = 0;
    AMBA_DCF_FILE_TYPE_e    FileType;

    if (IS_IMAGE_GROUP(HandleIndex)) {
        FileIndex = HandleIndex;
        FileType = AMBA_DCF_FILE_TYPE_IMAGE;
    } else {
        FileIndex = HandleIndex - ApplibUsbMtpClassInfo.ImageNumber;
        FileType = AMBA_DCF_FILE_TYPE_VIDEO;
    }
    AmbaDCF_GetFileName(FileType, FullFileName, FileIndex);
}

static UINT64   MTP_UtilityGetFileSize(UINT32 HandleIndex)
{
    UINT64   FileSize;
    char    FullFileName[64];

    MTP_UtilityGetFullFileName(HandleIndex, FullFileName);
    AmbaDCF_GetFileSize(FullFileName, &FileSize);

    return FileSize;
}

static INT32 MTP_ParseExifObject(char *FileName, MTP_PARSE_OBJECT_INFO_s * ParseObjectInfo)
{
    void                    *pFile = NULL;
    AMBA_FS_STAT              pStat;

    pFile = AmbaFS_fopen(FileName, "r");
    if (pFile == NULL) {
        AmbaPrint("File not exist");
        return NG;
    }

    AmbaDCF_GetFileType(FileName, &ParseObjectInfo->FileType);
    if (ParseObjectInfo->FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        if (AmbaMovParseKeyFrame(pFile, &ParseObjectInfo->MovInfo) != OK) {
            AmbaFS_fclose(pFile);
            AmbaPrint("Parse Mov error");
            return NG;
        }
    } else {
        AmbaFS_Stat(FileName, &pStat);
        memset(&ParseObjectInfo->ImgInfo, 0x0, sizeof(AMBA_EXIF_IMAGE_INFO_s));
        if (AmbaExifFullParse(pFile, &ParseObjectInfo->ImgInfo, &pStat) < 0) {
            AmbaFS_fclose(pFile);
            AmbaPrint("Parse Exif error");
            return NG;
        }

        /* Check sampling factor of SOF: Only support yuv420 and yuv422 */
        if (ParseObjectInfo->ImgInfo.YUVType != 0x21 && ParseObjectInfo->ImgInfo.YUVType != 0x22) {
            AmbaFS_fclose(pFile);
            AmbaPrint("PICDISP: Unsupported YUV fmt %x", ParseObjectInfo->ImgInfo.YUVType);
            return NG;
        }
    }
    AmbaFS_fclose(pFile);

    return OK;
}

static UINT32 MTP_UtilityGetParseHeight(UINT32 HandleIndex)
{
    MTP_PARSE_OBJECT_INFO_s *pParseObjectInfo = &MtpParseObjectInfo;
    char    FullFileName[64];

    memset(pParseObjectInfo, 0, sizeof(MTP_PARSE_OBJECT_INFO_s));
    MTP_UtilityGetFullFileName(HandleIndex, FullFileName);
    MTP_ParseExifObject(FullFileName, pParseObjectInfo);
    if (IS_IMAGE_GROUP(HandleIndex)) {
        return pParseObjectInfo->ImgInfo.Height;
    } else {
        return pParseObjectInfo->MovInfo.Height;
    }
}

static UINT32 MTP_UtilityGetParseWidth(UINT32 HandleIndex)
{
    MTP_PARSE_OBJECT_INFO_s *pParseObjectInfo = &MtpParseObjectInfo;
    char    FullFileName[64];

    memset(pParseObjectInfo, 0, sizeof(MTP_PARSE_OBJECT_INFO_s));
    MTP_UtilityGetFullFileName(HandleIndex, FullFileName);
    MTP_ParseExifObject(FullFileName, pParseObjectInfo);
    if (IS_IMAGE_GROUP(HandleIndex)) {
        return pParseObjectInfo->ImgInfo.Width;
    } else {
        return pParseObjectInfo->MovInfo.Width;
    }
}

static UINT32 MTP_UtilityGetObjectInfo(UINT32 HandleIndex, MTP_OBJECT_s **object)
{
    MTP_OBJECT_s *pMtpTmpObject = &MtpTmpObject;
    MTP_PARSE_OBJECT_INFO_s *pParseObjectInfo = &MtpParseObjectInfo;
    char    FullFileName[64], FileName[64];

    memset(pMtpTmpObject, 0, sizeof(MTP_OBJECT_s));
    memset(pParseObjectInfo, 0, sizeof(MTP_PARSE_OBJECT_INFO_s));

    MTP_UtilityGetFullFileName(HandleIndex, FullFileName);
    MTP_ParseExifObject(FullFileName, pParseObjectInfo);
    MTP_UtilityParseFileName((UINT8*)FullFileName, (UINT8*)FileName);

    MTP_UtilityStringToUnicode((UINT8*)FileName, pMtpTmpObject->ObjectFilename);
    pMtpTmpObject->ObjectLength = (UINT32)MTP_UtilityGetFileSize(HandleIndex);

    pMtpTmpObject->ObjectStorageId = ApplibUsbMtpClassInfo.DeviceInfo.StorageId;
    pMtpTmpObject->ObjectHandleId = HandleIndex;
    pMtpTmpObject->ObjectCompressedSize = pMtpTmpObject->ObjectLength;
    pMtpTmpObject->ObjectOffset = 0;

    if (IS_IMAGE_GROUP(HandleIndex)) {
        pMtpTmpObject->ObjectFormat = MTP_OFC_EXIF_JPEG;
        pMtpTmpObject->ObjectImageBitDepth = 24;
        pMtpTmpObject->ObjectImagePixHeight = pParseObjectInfo->ImgInfo.Height;
        pMtpTmpObject->ObjectImagePixWidth = pParseObjectInfo->ImgInfo.Width;
    } else {
        pMtpTmpObject->ObjectFormat = MTP_OFC_UNDEFINED;
        pMtpTmpObject->ObjectImageBitDepth = 0;
        pMtpTmpObject->ObjectImagePixHeight = pParseObjectInfo->MovInfo.Height;
        pMtpTmpObject->ObjectImagePixWidth = pParseObjectInfo->MovInfo.Width;
    }

    *object = &MtpTmpObject;

    return (OK);

}
#else
static UINT32 MTP_UtilityGetFileFormat(UINT32 HandleIndex)
{
  return 0;
}

static VOID MTP_UtilityGetFullFileName(UINT32 HandleIndex, char* FullFileName)
{

}

static UINT64   MTP_UtilityGetFileSize(UINT32 HandleIndex)
{
    return 0;
}



static UINT32 MTP_UtilityGetParseHeight(UINT32 HandleIndex)
{
    return 0;
}

static UINT32 MTP_UtilityGetParseWidth(UINT32 HandleIndex)
{
    return 0;
}

static UINT32 MTP_UtilityGetObjectInfo(UINT32 HandleIndex, MTP_OBJECT_s **object)
{
    return (OK);

}
#endif

static UINT32 MTP_ObjectHandlerCheck(UINT32  ObjectHandle, UINT32  *CallHandlerIndex)
{
    if (ObjectHandle <= (ApplibUsbMtpClassInfo.ImageNumber+ApplibUsbMtpClassInfo.OthersNumber)) {
            *CallHandlerIndex = ObjectHandle-1;
            return (OK);
    }

    return(MTP_RC_INVALID_OBJECT_HANDLE);
}

/* Operation code : DeleteObject 0x100B */
static UINT32 MTP_ObjectDelete(UINT32 ObjectHandle)
{

    UINT32        HandleIndex;
    UINT32        status;
    INT32         FsStatus;
    char         FullFileName[64];

    status = MTP_ObjectHandlerCheck(ObjectHandle, &HandleIndex);

    if (status == OK) {

        if (ApplibUsbMtpClassInfo.FpArray != NULL) {
            /* Yes, the handle is valid. The object pointer has been updated.  */
            /* The object may still be opened, try to close the handle first.  */
            FsStatus = AmbaFS_fclose(ApplibUsbMtpClassInfo.FpArray);
            ApplibUsbMtpClassInfo.FpArray = NULL;
            if (FsStatus < 0) {
                AmbaPrint("Failed to close %d handles", HandleIndex);
                return MTP_RC_STORE_NOT_AVAILABLE;
            }
        }
        MTP_UtilityGetFullFileName(ObjectHandle, FullFileName);
#if 0
        if (IS_IMAGE_GROUP(ObjectHandle)) {
            status = AmbaDCF_DeleteFile(AMBA_DCF_FILE_TYPE_IMAGE, FullFileName);
            ApplibUsbMtpClassInfo.ImageNumber--;
        } else {
            status = AmbaDCF_DeleteFile(AMBA_DCF_FILE_TYPE_VIDEO, FullFileName);
            ApplibUsbMtpClassInfo.OthersNumber--;
        }
#endif
        if (status != OK) {
            return MTP_RC_STORE_NOT_AVAILABLE;
        }

        ApplibUsbMtpClassInfo.ObjectNumberHandles--;
        return(OK);
    } else {
        return(MTP_RC_INVALID_OBJECT_HANDLE);
    }

}

/* Operation code : ResetDevice 0x1010 */
static UINT32 MTP_DeviceReset(void)
{
    return OK;
}

/* Operation code : GetDevicePropDesc 0x1014 */
static UINT32 MTP_PropDescGet(UINT32 DeviceProperty, UINT8 **DevicePropDataset, UINT32 *DevicePropDtasetLength)
{
    UINT32    status;

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME :
        *DevicePropDataset = DevicePropDataTimeDataset;
        *DevicePropDtasetLength = DEVICE_PROP_DATE_TIME_DATASET_LENGTH;
        status = OK;
        break;

    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG :
        *DevicePropDataset = DevicePropUseDeviceStageFlagDataset;
        *DevicePropDtasetLength = DEVICE_PROP_USE_DEVICE_STAGE_FLAG_DATASET_LENGTH;
        status = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER :
        *DevicePropDataset = DevicePropSynchronizationPartnerDataset;
        *DevicePropDtasetLength = DEVICE_PROP_SYNCHRONIZATION_PARTNER_DATASET_LENGTH;
        status = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME :
        *DevicePropDataset = DevicePropDeviceFriendlyNameDataset;
        *DevicePropDtasetLength = DEVICE_PROP_DEVICE_FRIENDLY_NAME_DATASET_LENGTH;
        status = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL :
        *DevicePropDataset = DevicePropBattteryLevelDataset;
        *DevicePropDtasetLength = DEVICE_PROP_BATTERY_LEVEL_DATASET_LENGTH;
        status = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE :
        *DevicePropDataset = DevicePropPrecivedDeviceTypeDataset;
        *DevicePropDtasetLength = DEVICE_PROP_PERCEIVED_DEVICE_TYPE_DATASET_LENGTH;
        status = OK;
        break;

    default :
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }
    return status;
}

/* Operation code : GetDevicePropValue 0x1015 */
static UINT32 MTP_PropValueGet(UINT32 DeviceProperty, UINT8 **DevicePorpValue, UINT32 *DevicePropValueLength)
{
    UINT32    status;

    status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;

    switch (DeviceProperty) {

    case MTP_DEV_PROP_DATE_TIME :
        *DevicePorpValue = DevicePropDataTimeDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 33;
        status = OK;
        break;

    case MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG :
        *DevicePorpValue = DevicePropUseDeviceStageFlagDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 1;
        status = OK;
        break;

    case MTP_DEV_PROP_SYNCHRONIZATION_PARTNER :
        *DevicePorpValue = DevicePropSynchronizationPartnerDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 1;
        status = OK;
        break;

    case MTP_DEV_PROP_DEVICE_FRIENDLY_NAME :
        *DevicePorpValue = DevicePropDeviceFriendlyNameDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 29;
        status = OK;
        break;

    case MTP_DEV_PROP_BATTERY_LEVEL :
        *DevicePorpValue = DevicePropBattteryLevelDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 1;
        status = OK;
        break;

    case MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE :
        *DevicePorpValue = DevicePropPrecivedDeviceTypeDataset + MTP_DATASET_VALUE_OFFSET;
        *DevicePropValueLength = 4;
        status = OK;
        break;

    default :
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
        break;
    }

    return(status);

}

/* Operation code : SetDeivcePropValue 0x1016 */
static UINT32 MTP_PropValueSet(UINT32 DeviceProperty, UINT8 *DevicePropValue, UINT32 DevicePropValueLength)
{
    UINT32    status;

    switch (DeviceProperty) {
    case MTP_DEV_PROP_DATE_TIME :
        memcpy (DevicePropDataTimeDataset + MTP_DATASET_VALUE_OFFSET, DevicePropValue, 0x10);
        status = OK;
        break;

    default :
        status = MTP_RC_DEVICE_PROP_NOT_SUPPORTED;
    }
    return(status);

}

static void MTP_ObjectClearAll(void)
{
    ApplibUsbMtpClassInfo.FpArray = NULL;

}

/* Operation code : FormatStore 0x100F */
static UINT32 MTP_StorageFormat(UINT32 StorageId)
{
    INT     FsStatus;
    CHAR    dummy='\0';

    /* Check the storage ID.  */
    if (StorageId == MTP_STORAGE_ID) {

        /* Format SD card with default FAT32 format */
        FsStatus = AmbaFS_Format('d',(const char *)&dummy);
        if (FsStatus < 0) {
            AmbaPrint("Failed to format");
            return (MTP_RC_ACCESS_DENIED);
        }

        /* Reset the handle counter.  */
        ApplibUsbMtpClassInfo.ObjectNumberHandles = 0;
        MTP_ObjectClearAll();

        /* Is there an error ?  */
        if (FsStatus == 0) {
            return(OK);
        } else {
            return(MTP_RC_ACCESS_DENIED);
        }
    } else {

        /* Error, wrong storage ID.  */
        return(MTP_RC_INVALID_STORAGE_ID);
    }
}

/* Operation code : GetStorageInfo 0x1005 */
static UINT32 MTP_StorageInfoGet(UINT32 StorageId, UINT32* MaxCapacityLow, UINT32* MaxCapacityHigh, UINT32* FreeSpaceLow, UINT32* FreeSpaceHigh)
{
    if (StorageId == ApplibUsbMtpClassInfo.DeviceInfo.StorageId) {
        *MaxCapacityLow = 0;
        *MaxCapacityHigh = 0;
        *FreeSpaceLow = 0;
        *FreeSpaceHigh = 0;
        return(OK);
    }
    else {
        return(MTP_RC_INVALID_STORAGE_ID);
    }
}

/* Operation code : GetNumObjects 0x1006 */
static UINT32 MTP_ObjectNumberGet(UINT32 ObjectFormatCode, UINT32 ObjectAssociation, UINT32 *ObjectNumber)
{
    *ObjectNumber =  ApplibUsbMtpClassInfo.ObjectNumberHandles;
    return(OK);
}

/* Operation code : GetObjectHandles 0x1007 */
static UINT32 MTP_ObjectHandlesGet (UINT32 ObjectHandlesFormatCode, UINT32 ObjectHandlesAssociation, UINT32 *ObjectHandlesArray, UINT32 ObjectHandlesMaxNumber, UINT32 *RequestNumber, USHORT *ResponseCode)
{
    UINT32   HandleIndex = 0;
    UINT32   FoundHandles = 0;
    UINT32   *ObjectHandlesArrayPtr = NULL;
    UINT32   TargetNum = 0;

    /* We start with no handles found.  */
    FoundHandles = 0;
    /* We store the handles in the array pointer, skipping the array count. */
    ObjectHandlesArrayPtr = ObjectHandlesArray + 1;

    if (ObjectHandlesFormatCode == MTP_OFC_EXIF_JPEG) {
        TargetNum = ApplibUsbMtpClassInfo.ImageNumber;
    } else if (ObjectHandlesFormatCode == MTP_OFC_UNDEFINED) {
        TargetNum = ApplibUsbMtpClassInfo.OthersNumber;
    } else if ((ObjectHandlesFormatCode == 0) || (ObjectHandlesFormatCode == 0xFFFFFFFF)) {
        TargetNum = ApplibUsbMtpClassInfo.OthersNumber + ApplibUsbMtpClassInfo.ImageNumber;
    }

    if (TargetNum > MTP_MAX_HANDLES) {
        TargetNum = MTP_MAX_HANDLES;
    }

    for (HandleIndex=0; HandleIndex<TargetNum; HandleIndex++) {
        /* We have a candicate.  Store the handle. */
        MTP_UtilityLongPut((UINT8 *) ObjectHandlesArrayPtr, HandleIndex+1);
        /* Next array container.  */
        ObjectHandlesArrayPtr++;
        /* We have found one handle more.  */
        FoundHandles++;
        /* Check if we are reaching the max array of handles.  */
        if (FoundHandles == ObjectHandlesMaxNumber) {
            /* Array is saturated. Store what we have found.  */
            MTP_UtilityLongPut((UINT8 *) ObjectHandlesArray, FoundHandles);
            return(OK);
        }
    }

    /* Array is populated. Store what we have found.  */
    MTP_UtilityLongPut((UINT8 *) ObjectHandlesArray, FoundHandles);

    /* And return to the Pima class.  */
    return(OK);

}

/* Operation code : GetObjectInfo 0x1008 */
static UINT32 MTP_ObjectInfoGet (UINT32 ObjectHandle,  MTP_OBJECT_s **object)
{
    UINT    status;
    UINT32   HandleIndex;

    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);

    MTP_UtilityGetObjectInfo(ObjectHandle, object);

    if (status == OK) {
        return(OK);
    } else {
        return(MTP_RC_INVALID_OBJECT_HANDLE);
    }

}

/* Operation code : GetObject 0x1009 */
/* Operation code : GetPartialObject 0x101B */
static UINT32 MTP_ObjectDataGet (UINT32 ObjectHandle, UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength, USHORT OpCode)

{
    UINT32          status;
    INT             StatusClose;
    UINT32          HandleIndex;
    UINT32          ReadLen;
    char            FullFileName[64];

    /* Check the object handle. It must be in the local array.  */
    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);

    if (status == OK) {
        /* We are either at the beginning of the transfer or continuing the transfer.
           Check of the filex array handle exist already.  */
        if (ApplibUsbMtpClassInfo.FpArray == NULL) {
            /* File not yet opened for this object.  Open the file. */
            MTP_UtilityGetFullFileName(ObjectHandle, FullFileName);
            ApplibUsbMtpClassInfo.FpArray = AmbaFS_fopen((const char*)FullFileName, "r");
            AmbaPrint("open %s",FullFileName);

            if (ApplibUsbMtpClassInfo.FpArray == NULL) {
                AmbaPrint("Failed to open %s",FullFileName);
                return(MTP_RC_OBJECT_NOT_OPENED);
            }
        }

        /* Read from the file into the media buffer.  */
        ReadLen =   AmbaFS_fread(ObjectBuffer,
                                    1,
                                    ObjectLengthRequested,
                                    ApplibUsbMtpClassInfo.FpArray);

        //AmbaPrint("req=%d B, read %d B @ 0x%8x", ObjectLengthRequested,ReadLen,ObjectOffset);

        *ObjectActualLength = ReadLen;

        if (ReadLen != 0) {
            status = OK;
        } else {
            status =  MTP_RC_ACCESS_DENIED;
        }

        /* Check if we have read the entire file.  */
        if (AmbaFS_feof(ApplibUsbMtpClassInfo.FpArray)) {
            /* This is the end of the transfer for the object. Close it.  */
            StatusClose =   AmbaFS_fclose(ApplibUsbMtpClassInfo.FpArray);
            ApplibUsbMtpClassInfo.FpArray = NULL;
            AmbaPrint("close file");

            if (StatusClose == 0 && status == OK) {
                return (OK);
            } else {
                StatusClose =  MTP_RC_ACCESS_DENIED;
                /* If status is error. we return status. If StatusClose is error we return StatusClose.  */
                if (status != OK) {
                    return(status);
                } else {
                    return(StatusClose);
                }
            }
        }
        return (OK);
    }

    return(MTP_RC_INVALID_OBJECT_HANDLE);

}

/* Operation code : SendObjectInfo 0x100C */
static UINT32 MTP_ObjectInfoSend (MTP_OBJECT_s *object, UINT32 StorageId, UINT32 ParentObjectHandle, UINT32 *ObjectHandle)
{
    UINT8               ObjectFilename[64];
    UINT32              status;
    INT                 FsStatus;
    INT32               DrvPathSize;

    if (ApplibUsbMtpClassInfo.ObjectNumberHandles < MTP_MAX_HANDLES) {
        DrvPathSize = MTP_UtilityStringLengthGet(ApplibUsbMtpClassInfo.DeviceInfo.RootPath);
        memcpy(ObjectFilename, &ApplibUsbMtpClassInfo.DeviceInfo.RootPath, DrvPathSize);
        MTP_UtilityUnicodeToString(object->ObjectFilename,(UINT8 *)ObjectFilename+DrvPathSize);

        if (object ->ObjectFormat == MTP_OFC_ASSOCIATION) {
            FsStatus = AmbaFS_Mkdir((const char *)ObjectFilename);
            if (FsStatus < 0) {
                status = MTP_RC_STORE_NOT_AVAILABLE;
                return (status);
            }
        } else {
            AmbaPrint("Not support create new object");
            return (MTP_RC_STORE_NOT_AVAILABLE);
        }

        /* We should never get here. */
        return(MTP_RC_STORE_FULL);

    }

    /* No more space for handle. Return storage full.  */
    return(MTP_RC_STORE_FULL);

}

/* Operation code : SendObject 0x100D */
static UINT32 MTP_ObjectDataSend (UINT32 ObjectHandle, UINT32 phase, UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength)
{

    // Not Support Object Write Now.

    return(MTP_RC_INVALID_OBJECT_HANDLE);

}

/* Operation code : GetObjectPropDesc 0x9802 */
static UINT32 MTP_ObjectPropDescGet (UINT32 ObjectProperty, UINT32 ObjectFormatCode, UINT8 **ObjectPropDataset, UINT32 *ObjectPropDatasetLength)
{
    UINT32                       status;
    UINT8                       *ObjectPropertyDatasetData;
    UINT32                       ObjectPropertyDatasetDataLength;

    /* Check the object format belongs to the list. 3 ategories : generic, audio, video */
    switch (ObjectFormatCode)
    {

        case    MTP_OFC_UNDEFINED                          :
        case    MTP_OFC_ASSOCIATION                        :
        case    MTP_OFC_EXIF_JPEG                      :
        case    MTP_OFC_MP3                                :
        case    MTP_OFC_ASF                                :
        case    MTP_OFC_WMA                                :
        case    MTP_OFC_WMV                                :
        case    MTP_OFC_ABSTRACT_AUDIO_ALBUM               :
        case    MTP_OFC_ABSTRACT_AUDIO_AND_VIDEO_PLAYLIST  :

            /* Set the pointer to the dataset_buffer.  */
            ObjectPropertyDatasetData = ObjectPropDatasetDataBuffer;

            /* Isolate the property. That will determine the dataset header.  */
            switch (ObjectProperty)
            {
                case    MTP_OBJECT_PROP_STORAGEID          :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_STORAGEID);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 12;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_OBJECT_FORMAT      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_OBJECT_FORMAT);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 3.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 3);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 12;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_PROTECTION_STATUS      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_PROTECTION_STATUS);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 4.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 4);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 7, 2);

                    /* Elements in Enum array.  Here we store only No protection and Read-Only protection values. This can be extended with
                       Read-only data and Non transferrable data. Spec talks about MTP vendor extension range as well. Not used here.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 11, 1);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 18;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_OBJECT_SIZE        :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_OBJECT_SIZE);

                    /* Data type is UINT64.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT64);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT64.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 12) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 18;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_OBJECT_FILE_NAME   :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_OBJECT_FILE_NAME);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_PARENT_OBJECT      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_PARENT_OBJECT);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 14;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER        :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER);

                    /* Data type is UINT128.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT128);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT128.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8, 0);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 12, 0);

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 16, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 20) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_NAME   :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_NAME);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is 2.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 2);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_NON_CONSUMABLE     :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_NON_CONSUMABLE);

                    /* Data type is UINT8.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT8);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT8.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is 2.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 2);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6, 2);

                    /* Elements in Enum array.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) =  0;
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9) =  1;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 15;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_ARTIST             :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_ARTIST);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_TRACK          :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_TRACK);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 3.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 3);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 12;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_USE_COUNT          :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_USE_COUNT);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 1.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 14;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_DATE_AUTHORED      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_DATE_AUTHORED);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is 3.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 3;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_GENRE              :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_GENRE);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_ALBUM_NAME     :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_ALBUM_NAME);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_ALBUM_ARTIST       :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_ALBUM_ARTIST);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_SAMPLE_RATE        :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_SAMPLE_RATE);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is 0KHZ.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0x00000000);

                    /* Maximum range in array is KHZ.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,0x0002EE00 );

                    /* Range step size is 32HZ.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17,0x00000020 );

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_NUMBER_OF_CHANNELS     :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_NUMBER_OF_CHANNELS);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 0);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 6) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 7, 3);

                    /* Elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 11, 1);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13, 2);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 20;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_AUDIO_WAVE_CODEC       :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_AUDIO_WAVE_CODEC);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 3);

                    /* Elements in Enum array.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 11, MTP_AUDIO_CODEC_WAVE_FORMAT_MPEGLAYER3);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 15, MTP_AUDIO_CODEC_WAVE_FORMAT_MPEG);
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 19, MTP_AUDIO_CODEC_WAVE_FORMAT_RAW_AAC1);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 28;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_AUDIO_BITRATE      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_AUDIO_BITRATE);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0x0000FA00);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is 1 bit per second.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0x00000001);

                    /* Maximum range in array is 1,500,000 bit per second.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,0x0016E360 );

                    /* Range step size is 1 bit per second.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17,0x00000001 );

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_DURATION           :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_DURATION);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag is 1.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 14;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_WIDTH      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_WIDTH);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is customer defined.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0);

                    /* Maximum range in array is customer defined.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13, 10000);

                    /* Range step size is customer defined.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17, 1);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_HEIGHT     :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_HEIGHT);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT16.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0x0000);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 0);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is customer defined.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0x0000);

                    /* Maximum range in array is customer defined.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13, 10000);

                    /* Range step size is customer defined.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17, 0x0000);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;


                case    MTP_OBJECT_PROP_SCAN_TYPE      :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_SCAN_TYPE);

                    /* Data type is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT16);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT16.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 2, 2);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 7, 8);

                    /* Elements in Enum array.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9,  0x0000);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 11, 0x0001);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13, 0x0002);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 15, 0x0003);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17, 0x0004);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 19, 0x0005);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 21, 0x0006);
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 23, 0x0007);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 29;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 2);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 1);

                    /* Elements in Enum array.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,  0x00000000);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 22;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_VIDEO_BITRATE :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_VIDEO_BITRATE);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 2);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0x00000000);

                    /* Maximum range in array is 0xFFFFFFFF.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,0xFFFFFFFF );

                    /* Range step size is 1.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17,0x00000001 );

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 2);

                    /* Form Flag ENUM.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 2;

                    /* Number of elements in Enum array.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 1);

                    /* Elements in Enum array.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,  0x00000000);

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 22;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_KEYFRAME_DISTANCE :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_KEYFRAME_DISTANCE);

                    /* Data type is UINT32.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_UINT32);

                    /* GetSet value is GET/SET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Default value is UINT32.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE, 0);

                    /* Group code is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 4, 2);

                    /* Form Flag RANGE.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 8) = 1;

                    /* Minimum range in array is 0.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 9, 0x00000000);

                    /* Maximum range in array is FFFFFFFF.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 13,0x0000FFFF );

                    /* Range step size is 1.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 17,0x00000001 );

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 26;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                case    MTP_OBJECT_PROP_ENCODING_PROFILE       :

                    /* Add the property code.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_CODE, MTP_OBJECT_PROP_ENCODING_PROFILE);

                    /* Data type is STRING.  */
                    MTP_UtilityShortPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_DATATYPE, MTP_TYPES_STR);

                    /* GetSet value is GETSET.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_GETSET) = MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET;

                    /* Store a empty Unicode string.   */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE) = 0;

                    /* Group code is NULL.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 1, 0);

                    /* Form Flag is not used.  */
                    *(ObjectPropertyDatasetData + MTP_OBJECT_PROPERTY_DATASET_VALUE + 5) = 0;

                    /* Set the length.  */
                    ObjectPropertyDatasetDataLength = 11;

                    /* We could create this property. */
                    status = OK;

                    /* Done here.  */
                    break;

                default :

                    /* Error, prop code is not valid.  */
                    status = MTP_RC_INVALID_OBJECT_PROP_CODE;

            }

        /* Check the status of the operation.  */
        if (status == OK)
        {

            /* The property exist and its dataset created.  Return its pointer to MTP.  */
            *ObjectPropDataset = ObjectPropertyDatasetData;

            /* And the length of the dataset.  */
            *ObjectPropDatasetLength = ObjectPropertyDatasetDataLength;

            /* Done here.  */
            return(OK);
        }
        else

            /* Done here. Return error.  */
            return(MTP_RC_INVALID_OBJECT_PROP_CODE);

        break;

        default :

            /* We get here when we have the wrong format code.  */
            return(MTP_RC_INVALID_OBJECT_FORMAT_CODE);
    }

}

/* Operation code : GetObjectPropValue 0x9803 */
static UINT32 MTP_ObjectPropValueGet (UINT32 ObjectHandle, UINT32 ObjectProperty, UINT8 **ObjectPropValue, UINT32 *ObjectPropValueLength)
{

    UINT32                       HandleIndex;
    UINT32                       status;
    UINT8                       *ObjectPropertyDatasetData;
    UINT32                       ObjectPropertyValueDataLength;

    /* Check the object handle. It must be in the local array.  */
    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);

    /* Does the object handle exist ?  */
    if (status == OK)
    {

        /* Set the pointer to the dataset_buffer.  */
        ObjectPropertyDatasetData = ObjectPropDatasetDataBuffer;

        /* Isolate the property. That will determine were we fetch the value.  We use the dataset storage area to build the value.  */
        switch (ObjectProperty)
        {
            case    MTP_OBJECT_PROP_STORAGEID          :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, ApplibUsbMtpClassInfo.DeviceInfo.StorageId);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_OBJECT_FORMAT      :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, MTP_UtilityGetFileFormat(ObjectHandle));

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_PROTECTION_STATUS      :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_OBJECT_SIZE        :
                {
                    UINT64 FileSize;

                    FileSize = MTP_UtilityGetFileSize(ObjectHandle);
                    /* Copy the value itself.  */
                    MTP_UtilityLongPut(ObjectPropertyDatasetData , (FileSize&0xFFFFFFFF));
                    MTP_UtilityLongPut(ObjectPropertyDatasetData  + 4, (FileSize>>32));
                }
                /* Set the length.  */
                ObjectPropertyValueDataLength = 8;

                /* We could create this property. */
                status = OK;


                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_OBJECT_FILE_NAME   :
                {
                    char FullFileName[64], FileName[64];

                    MTP_UtilityGetFullFileName(ObjectHandle, FullFileName);
                    MTP_UtilityParseFileName((UINT8*) FullFileName, (UINT8*) FileName);
                    /* Store the file name in unicode format.  */
                    MTP_UtilityStringToUnicode((UINT8*)FileName, ObjectPropertyDatasetData);
                }
                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2 + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_PARENT_OBJECT      :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER        :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, ObjectHandle);
                MTP_UtilityLongPut(ObjectPropertyDatasetData+4, 0);
                MTP_UtilityLongPut(ObjectPropertyDatasetData+8, 0);
                MTP_UtilityLongPut(ObjectPropertyDatasetData+12, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 16;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_NAME   :

                {
                    char FullFileName[64], FileName[64];

                    MTP_UtilityGetFullFileName(ObjectHandle, FullFileName);
                    MTP_UtilityParseFileName((UINT8*) FullFileName, (UINT8*) FileName);
                    /* Store the file name in unicode format.  */
                    MTP_UtilityStringToUnicode((UINT8*)FileName, ObjectPropertyDatasetData);
                }

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_NON_CONSUMABLE     :

                /* Copy the value itself.  */
                *ObjectPropertyDatasetData = 0;

                /* Set the length.  */
                ObjectPropertyValueDataLength = 1;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_ARTIST             :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_TRACK      :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_USE_COUNT      :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_DATE_AUTHORED      :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_GENRE              :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_ALBUM_NAME             :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_ALBUM_ARTIST               :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_SAMPLE_RATE        :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_NUMBER_OF_CHANNELS     :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_AUDIO_WAVE_CODEC       :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_AUDIO_BITRATE      :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_DURATION       :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_WIDTH          :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, MTP_UtilityGetParseWidth(ObjectHandle));

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_HEIGHT         :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, MTP_UtilityGetParseHeight(ObjectHandle));

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_SCAN_TYPE          :

                /* Copy the value itself.  */
                MTP_UtilityShortPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC         :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 2;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_VIDEO_BITRATE          :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS            :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_KEYFRAME_DISTANCE          :

                /* Copy the value itself.  */
                MTP_UtilityLongPut(ObjectPropertyDatasetData, 0);

                /* Set the length.  */
                ObjectPropertyValueDataLength = 4;

                /* We could create this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_ENCODING_PROFILE           :

                /* Store the file name in unicode format.  */
                MTP_UtilityStringToUnicode(0, ObjectPropertyDatasetData);

                /* Set the length.  First Unicode string data.  */
                ObjectPropertyValueDataLength = (UINT32) *(ObjectPropertyDatasetData) * 2  + 1;

                /* Done here.  */
                break;

            default :

                /* Error, prop code is not valid.  */
                status =  MTP_RC_INVALID_OBJECT_PROP_CODE;

        }

        /* Check the status of the operation.  */
        if (status == OK)
        {

            /* The property exist and its value created.  Return its pointer to MTP.  */
            *ObjectPropValue = ObjectPropertyDatasetData;

            /* And the length of the dataset.  */
            *ObjectPropValueLength = ObjectPropertyValueDataLength;

            /* Done here.  */
            return(OK);
        }
        else

            /* Done here. Return error.  */
            return(MTP_RC_INVALID_OBJECT_PROP_CODE);
    }
    else

        /* Done here. Return error.  */
        return(MTP_RC_INVALID_OBJECT_HANDLE);
}

/* Operation code : SetObjectPropValue 0x9804 */
static UINT32 MTP_ObjectPropValueSet (UINT32 ObjectHandle, UINT32 ObjectProperty, UINT8 *ObjectPropValue, UINT32 ObjectPropValueLength)
{

    UINT32                       HandleIndex;
    UINT32                       status;

    /* Check the object handle. It must be in the local array.  */
    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);


    /* Does the object handle exist ?  */
    if (status == OK)
    {

        /* Isolate the property.  This is SET. So the properties that are GET only will not be changed.  */
        switch (ObjectProperty)
        {
            case    MTP_OBJECT_PROP_STORAGEID          :
            case    MTP_OBJECT_PROP_OBJECT_FORMAT      :
            case    MTP_OBJECT_PROP_OBJECT_SIZE        :
            case    MTP_OBJECT_PROP_PARENT_OBJECT      :
            case    MTP_OBJECT_PROP_DURATION           :

                /* Object is write protected.  */
                status = MTP_RC_OBJECT_WRITE_PROTECTED;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_OBJECT_FILE_NAME   :

                /* We could set this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_NAME   :

                /* We could set this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_ARTIST             :

                /* We could set this property. */
                status = OK;

                break;

            case    MTP_OBJECT_PROP_DATE_AUTHORED      :

                /* We could set this property. */
                status = OK;

                /* Done here.  */
                break;

            case    MTP_OBJECT_PROP_GENRE              :

                /* We could set this property. */
                status = OK;

                /* Done here.  */
                break;

            default :

                /* Error, prop code is not valid.  */
                status = MTP_RC_INVALID_OBJECT_PROP_CODE;
        }


        /* Done here. Return status.  */
        return(status);
    }
    else

        /* Done here. Return error.  */
        return(MTP_RC_INVALID_OBJECT_HANDLE);
}

/* Operation code : GetObjectReferences 0x9810 */
static UINT32 MTP_ObjectReferenceGet (UINT32 ObjectHandle, UINT8 **ObjectReferenceArray, UINT32 *ObjectReferenceArrayLength)
{

    UINT32                       HandleIndex;
    UINT32                       status;
    UINT32                       ReferencesArray;

    /* Check the object handle. It must be in the local array.  */
    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);

    /* Does the object handle exist ?  */
    if (status == OK)
    {

        /* The property exist. Not sure what to do with references in this release.
           Here we simply create an empty array. */
           ReferencesArray = 0;

           /* Return its pointer to MTP.  */
        *ObjectReferenceArray = (UINT8 *) ReferencesArray;

        /* And the length of the dataset.  */
        *ObjectReferenceArrayLength = sizeof(UINT32);

        /* Done here.  */
        return(OK);

    }

    else

        /* Done here. Return error.  */
        return(MTP_RC_INVALID_OBJECT_HANDLE);
}

/* Operation code : SetObjectReferences 0x9811 */
static UINT32 MTP_ObjectReferenceSet (UINT32 ObjectHandle, UINT8 *ObjectReferenceArray, UINT32 ObjectReferenceArrayLength)
{

    UINT32                       HandleIndex;
    UINT32                       status;

    /* Check the object handle. It must be in the local array.  */
    status = MTP_ObjectHandlerCheck((UINT32)ObjectHandle, &HandleIndex);

    /* Does the object handle exist ?  */
    if (status == OK)
    {

        /* The property exist. Not sure what to do with references in this release. */

        /* Done here.  */
        return(OK);

    }

    else

        /* Done here. Return error.  */
        return(MTP_RC_INVALID_OBJECT_HANDLE);
}

/* Operation code : MTP_AmageCommunication 0x9999; Parameter 1: direction; Parameter 2: data size; Parameter 3: item_mode*/
static UINT32 MTP_AmageCommunication (UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32* length, UINT32* dir)
{
    TUNE_Initial_Config_s TuneInitialConfig;
    extern int USB_CheckProcessingDown(void);
    DBGMSG("[Applib - Usb MTP Amage] MTP_AmageCommunication");


    if(USB_CheckProcessingDown()==1){
        TuneInitialConfig.Usb.pBytePool = APPLIB_G_MMPL;
        TuneInitialConfig.Usb.StillTuningRawBuffer=&AppLibStillEnc_RawEncGetRawEncodeBuffer;
        TuneInitialConfig.Usb.StillTuningPreLoadDone=&AppLibStillEnc_RawEncPreLoadDone;
        TuneInitialConfig.Usb.StillItunerRawEncode=&AppLibStillEnc_RawEncode;
        AmbaTUNE_Change_Parser_Mode(USB_TUNE);
        if (0 != AmbaTUNE_Init(&TuneInitialConfig)) {
            AmbaPrint("%s() %d, call AmbaTUNE_Init() Fail", __func__, __LINE__);
        }
    }
    return USBHdlr_AmageCommunication(Parameter1, Parameter2, Parameter3, Parameter4, Parameter5, length, dir);
}

static UINT32 MTP_AmageDataGet(UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength)
{

    AMBA_DSP_IMG_MODE_CFG_s mode;
    TUNE_Save_Param_s Save_Param;

    DBGMSG("[Applib - Usb MTP Amage] MTP_AmageDataGet");
    memset(&mode,0x0 ,sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    memset(&Save_Param, 0x0, sizeof(Save_Param));

    Save_Param.USB.Buffer = ObjectBuffer;
    Save_Param.USB.Offset = ObjectOffset;
    Save_Param.USB.LengthRequested = ObjectLengthRequested;
    Save_Param.USB.ActualLength = ObjectActualLength;


   // return AmbaTUNE_Save_IDSP(&mode, (TUNE_Save_Param_s *) &Save_Param);
   return USBHdlr_Save_IDSP(&mode, (TUNE_USB_Save_Param_s *) &Save_Param);



}

static UINT32 MTP_AmageDataSend(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength)
{


    TUNE_Load_Param_s Load_Param;

    DBGMSG("[Applib - Usb MTP Amage] MTP_AmageDataSend 0x%x %d %d",ObjectBuffer,ObjectOffset,ObjectLength);

    memset(&Load_Param, 0x0, sizeof(Load_Param));

    Load_Param.USB.Buffer = ObjectBuffer;
    Load_Param.USB.Offset = ObjectOffset;
    Load_Param.USB.Length = ObjectLength;

    return USBHdlr_Load_IDSP((TUNE_USB_Load_Param_s *) &Load_Param);
}

static INT32 MTP_InitSys(void)
{
    INT32 ReturnValue = 0;

    ApplibUsbMtpClassInfo.DeviceInfo.DeviceInfoVendorName = MtpDeviceInfoVendorName;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceInfoProductName = MtpDeviceInfoProductName;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceInfoSerialNo = MtpDeviceInfoSerialNo;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceInfoVersion = MtpDeviceInfoVersion;
    ApplibUsbMtpClassInfo.DeviceInfo.VolumeDescription = MtpVolumeDescription;
    ApplibUsbMtpClassInfo.DeviceInfo.VolumeLabel = MtpVolumeLabel;
    strncpy((CHAR*)&ApplibUsbMtpClassInfo.DeviceInfo.RootPath, "c:\\", sizeof("c:\\")-1);
    ApplibUsbMtpClassInfo.DeviceInfo.StorageId = MTP_STORAGE_ID;
    ApplibUsbMtpClassInfo.DeviceInfo.StorageType = MTP_STC_FIXED_RAM;
    ApplibUsbMtpClassInfo.DeviceInfo.FileSystemType = MTP_FSTC_GENERIC_FLAT;
    ApplibUsbMtpClassInfo.DeviceInfo.AccessCapability = MTP_AC_RO_WITHOUT_OBJECT_DELETION;
    ApplibUsbMtpClassInfo.DeviceInfo.MaxCapacityLow = 0;
    ApplibUsbMtpClassInfo.DeviceInfo.MaxCapacityHigh = 0;
    ApplibUsbMtpClassInfo.DeviceInfo.FreeSpaceLow = 0;
    ApplibUsbMtpClassInfo.DeviceInfo.FreeSpaceHigh = 0;
    ApplibUsbMtpClassInfo.DeviceInfo.FreeSpaceImage = 0xFFFFFFFF;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceSupportProp = DeviceSupportProp;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceSupportCaptureFormat = DeviceSupportCaptureFormat;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceSupportImgFormat = DeviceSupportImgFormat;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectSupportProp = ObjectSupportProp;
    ApplibUsbMtpClassInfo.DeviceInfo.PropDescGet = MTP_PropDescGet;
    ApplibUsbMtpClassInfo.DeviceInfo.PropValueGet = MTP_PropValueGet;
    ApplibUsbMtpClassInfo.DeviceInfo.PropValueSet = MTP_PropValueSet;
    ApplibUsbMtpClassInfo.DeviceInfo.StorageFormat = MTP_StorageFormat;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectDelete = MTP_ObjectDelete;
    ApplibUsbMtpClassInfo.DeviceInfo.DeviceReset = MTP_DeviceReset;
    ApplibUsbMtpClassInfo.DeviceInfo.StorageInfoGet = MTP_StorageInfoGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectNumberGet = MTP_ObjectNumberGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectHandlesGet = MTP_ObjectHandlesGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectInfoGet = MTP_ObjectInfoGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectDataGet = MTP_ObjectDataGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectInfoSend = MTP_ObjectInfoSend;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectDataSend = MTP_ObjectDataSend;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectPropDescGet = MTP_ObjectPropDescGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectPropValueGet = MTP_ObjectPropValueGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectPropValueSet = MTP_ObjectPropValueSet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectReferenceGet = MTP_ObjectReferenceGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectReferenceSet = MTP_ObjectReferenceSet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectClearAll = MTP_ObjectClearAll;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectCustomCommand = MTP_AmageCommunication;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectCustomDataGet = MTP_AmageDataGet;
    ApplibUsbMtpClassInfo.DeviceInfo.ObjectCustomDataSend = MTP_AmageDataSend;

    AmbaUSB_Class_Mtp_SetInfo(&ApplibUsbMtpClassInfo.DeviceInfo);

    return ReturnValue;
}

static UINT32 MTP_LoadObject(void)
{
    INT32 ReturnValue = 0;

//    AmbaDCF_GetFileAmount(AMBA_DCF_FILE_TYPE_VIDEO, &ApplibUsbMtpClassInfo.OthersNumber);
//    AmbaDCF_GetFileAmount(AMBA_DCF_FILE_TYPE_IMAGE, &ApplibUsbMtpClassInfo.ImageNumber);

    if (MTP_MAX_HANDLES > (ApplibUsbMtpClassInfo.OthersNumber+ApplibUsbMtpClassInfo.ImageNumber)) {
        ApplibUsbMtpClassInfo.ObjectNumberHandles = ApplibUsbMtpClassInfo.OthersNumber + ApplibUsbMtpClassInfo.ImageNumber;
    } else if (MTP_MAX_HANDLES > ApplibUsbMtpClassInfo.ImageNumber) {
        ApplibUsbMtpClassInfo.ObjectNumberHandles = MTP_MAX_HANDLES;
        ApplibUsbMtpClassInfo.OthersNumber = MTP_MAX_HANDLES - ApplibUsbMtpClassInfo.ImageNumber;
    } else {
        ApplibUsbMtpClassInfo.ObjectNumberHandles = MTP_MAX_HANDLES;
        ApplibUsbMtpClassInfo.ImageNumber = MTP_MAX_HANDLES;
        ApplibUsbMtpClassInfo.OthersNumber = 0;
    }
    AmbaPrint("Total objects = %d", ApplibUsbMtpClassInfo.ObjectNumberHandles);
    AmbaPrint("Image objects = %d", ApplibUsbMtpClassInfo.ImageNumber);
    AmbaPrint("other objects = %d", ApplibUsbMtpClassInfo.OthersNumber);

    return ReturnValue;
}

/**
 *  @brief Init USB MTP for Amage
 *
 *  Init USB MTP for Amage
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibUSB_MTP_Amage_Init(void)
{
    INT32 ReturnValue = 0;

    MTP_InitSys();
    ReturnValue = MTP_LoadObject();

    return ReturnValue;
}


#define APPLIB_USB_AMAGE_TASK_STACK_SIZE  (12 * 1024)
#define APPLIB_USB_AMAGE_TASK_STACK_PRIORITY  (70)

int ApplibUsbAmage_Start(void)
{
    int ReturnValue = 0;
    USB_CLASS_INIT_s ClassConfig = {UDC_CLASS_NONE, 0, 0};

    DBGMSGc2(GREEN,"[Applib - Usb Amage] Start ");

    AppLibUSB_MTP_Amage_Init();

    /* Init USB custom device info.*/
    ReturnValue = AppLibUSB_Custom_SetDevInfo(APPLIB_USB_DEVICE_CLASS_MTP);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb Amage] AppLibUSB_Custom_SetDevInfo fail");
    }

    /* Hook USB Class*/
    ClassConfig.classID = UDC_CLASS_MTP;
    ClassConfig.ClassTaskPriority = APPLIB_USB_AMAGE_TASK_STACK_PRIORITY;
    ClassConfig.ClassTaskStackSize = APPLIB_USB_AMAGE_TASK_STACK_SIZE;
    ReturnValue = AmbaUSB_System_ClassHook(&ClassConfig);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb Amage] AmbaUSB_System_ClassHook fail");
    }
    //DBGMSGc2(GREEN,"[Applib - Usb Amage] Start Sleep %d ms", APPLIB_USB_DATA_CONN_DELAY);
    //AmbaKAL_TaskSleep(APPLIB_USB_DATA_CONN_DELAY);

    // pollo - 2014/06/16 - Fix issue: [Jira][Amba168] USB: support for delayed initialization, Item 2:
    // Application will not initialize the USB class immediately after the vbus detect.
    // It will wait for switching to MTP mode.
    // The lower layer should keep the host happy till the application initializes the USB class.
    // 1. Add AmbaUSB_System_SetDeviceDataConn() for Applications to enable/disable USB device data connect.
    // 2. Add AmbaUSB_System_SetDeviceDataConnWithVbus() for Applications to setup USB device data connect status when VBUS is detected.
    //    1: Data connect will be enabled when VBUS is detected. PC will recognize it immediately.
    //    0: Data connect will NOT be enabled when VBUS is detected. PC will not recognize it until AmbaUSB_System_SetDeviceDataConn(1) is called.
    ReturnValue = AmbaUSB_System_SetDeviceDataConn(1);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[Applib - Usb Amage] AppLibUSB_Custom_SetDevInfo fail");
    }
    return ReturnValue;
}

