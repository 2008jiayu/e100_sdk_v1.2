/**
 * @file src/app/connected/applib/src/usb/ApplibUsb_MTP_Amage.h
 *
 *  Header of Amage USB MTP class functions.
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

#include <AmbaFS.h>
#include <usb/AmbaUSB_API.h>


#ifndef APPLIB_USB_MTP_AMAGE_H_
#define APPLIB_USB_MTP_AMAGE_H_

#define MTP_STORAGE_ID                                            0x10001

/* Define PIMA Storage Types Codes.  */

#define MTP_STC_UNDEFINED                                        0x0000
#define MTP_STC_FIXED_ROM                                        0x0001
#define MTP_STC_REMOVABLE_ROM                                    0x0002
#define MTP_STC_FIXED_RAM                                        0x0003
#define MTP_STC_REMOVABLE_RAM                                    0x0004

/* Define PIMA File System Types Codes.  */

#define MTP_FSTC_UNDEFINED                                        0x0000
#define MTP_FSTC_GENERIC_FLAT                                    0x0001
#define MTP_FSTC_GENERIC_HIERARCHICAL                            0x0002
#define MTP_FSTC_DCF                                            0x0003

/* Define PIMA File System Access Types Codes.  */

#define MTP_AC_READ_WRITE                                        0x0000
#define MTP_AC_RO_WITHOUT_OBJECT_DELETION                        0x0001
#define MTP_AC_RO_WITH_OBJECT_DELETION                          0x0002

#define MTP_OFC_UNDEFINED                                       0x3000
#define MTP_OFC_ASSOCIATION                                     0x3001
#define MTP_OFC_SCRIPT                                          0x3002
#define MTP_OFC_EXECUTABLE                                      0x3003
#define MTP_OFC_TEXT                                            0x3004
#define MTP_OFC_HTML                                            0x3005
#define MTP_OFC_DPOF                                            0x3006
#define MTP_OFC_AIFF                                            0x3007
#define MTP_OFC_WAV                                             0x3008
#define MTP_OFC_MP3                                             0x3009
#define MTP_OFC_AVI                                             0x300A
#define MTP_OFC_MPEG                                            0x300B
#define MTP_OFC_ASF                                             0x300C
#define MTP_OFC_DEFINED                                         0x3800
#define MTP_OFC_EXIF_JPEG                                       0x3801
#define MTP_OFC_TIFF_EP                                         0x3802
#define MTP_OFC_FLASHPIX                                        0x3803
#define MTP_OFC_BMP                                             0x3804
#define MTP_OFC_CIFF                                            0x3805
#define MTP_OFC_UNDEFINED_2                                     0x3806
#define MTP_OFC_GIF                                             0x3807
#define MTP_OFC_JFIF                                            0x3808
#define MTP_OFC_CD                                              0x3809
#define MTP_OFC_PICT                                            0x380A
#define MTP_OFC_PNG                                             0x380B
#define MTP_OFC_UNDEFINED_3                                     0x380C
#define MTP_OFC_TIFF                                            0x380D
#define MTP_OFC_TIFF_IT                                         0x380E
#define MTP_OFC_JP2                                             0x380F
#define MTP_OFC_JPX                                             0x3810
#define MTP_OFC_UNDEFINED_FIRMWARE                              0xB802
#define MTP_OFC_WINDOWS_IMAGE_FORMAT                            0xB881
#define MTP_OFC_UNDEFINED_AUDIO                                 0xB900
#define MTP_OFC_WMA                                             0xB901
#define MTP_OFC_OGG                                             0xB902
#define MTP_OFC_AAC                                             0xB903
#define MTP_OFC_AUDIBLE                                         0xB904
#define MTP_OFC_FLAC                                            0xB906
#define MTP_OFC_UNDEFINED_VIDEO                                 0xB980
#define MTP_OFC_WMV                                             0xB981
#define MTP_OFC_MP4_CONTAINER                                   0xB982
#define MTP_OFC_MP2                                             0xB983
#define MTP_OFC_3GP_CONTAINER                                   0xB984
#define MTP_OFC_UNDEFINED_COLLECTION                            0xBA00
#define MTP_OFC_ABSTRACT_MULTIMEDIA_ALBUM                       0xBA01
#define MTP_OFC_ABSTRACT_IMAGE_ALBUM                            0xBA02
#define MTP_OFC_ABSTRACT_AUDIO_ALBUM                            0xBA03
#define MTP_OFC_ABSTRACT_VIDEO_ALBUM                            0xBA04
#define MTP_OFC_ABSTRACT_AUDIO_AND_VIDEO_PLAYLIST               0xBA05
#define MTP_OFC_ABSTRACT_CONTACT_GROUP                          0xBA06
#define MTP_OFC_ABSTRACT_MESSAGE_FOLDER                         0xBA07
#define MTP_OFC_ABSTRACT_CHAPTERED_PRODUCTION                   0xBA08
#define MTP_OFC_ABSTRACT_AUDIO_PLAYLIST                         0xBA09
#define MTP_OFC_ABSTRACT_VIDEO_PLAYLIST                         0xBA0A
#define MTP_OFC_ABSTRACT_MEDIACAST                              0xBA0B
#define MTP_OFC_WPL_PLAYLIST                                    0xBA10
#define MTP_OFC_M3U_PLAYLIST                                    0xBA11
#define MTP_OFC_MPL_PLAYLIST                                    0xBA12
#define MTP_OFC_ASX_PLAYLIST                                    0xBA13
#define MTP_OFC_PLS_PLAYLIST                                    0xBA14
#define MTP_OFC_UNDEFINED_DOCUMENT                              0xBA80
#define MTP_OFC_ABSTRACT_DOCUMENT                               0xBA81
#define MTP_OFC_XML_DOCUMENT                                    0xBA82
#define MTP_OFC_MICROSOFT_WORD_DOCUMENT                         0xBA83
#define MTP_OFC_MHT_COMPILED_HTML_DOCUMENT                      0xBA84
#define MTP_OFC_MICROSOFT_EXCEL_SPREADSHEET                     0xBA85
#define MTP_OFC_MICROSOFT_POWERPOINT_PRESENTATION               0xBA86
#define MTP_OFC_UNDEFINED_MESSAGE                               0xBB00
#define MTP_OFC_ABSTRACT_MESSAGE                                0xBB01
#define MTP_OFC_UNDEFINED_CONTACT                               0xBB80
#define MTP_OFC_ABSTRACT_CONTACT                                0xBB81
#define MTP_OFC_VCARD2                                          0xBB82


#define MTP_DEV_PROP_UNDEFINED                                  0x5000
#define MTP_DEV_PROP_BATTERY_LEVEL                              0x5001
#define MTP_DEV_PROP_FUNCTIONAL_MODE                            0x5002
#define MTP_DEV_PROP_IMAGE_SIZE                                 0x5003
#define MTP_DEV_PROP_COMPRESSION_SETTING                        0x5004
#define MTP_DEV_PROP_WHITE_BALANCE                              0x5005
#define MTP_DEV_PROP_RGB_GAIN                                   0x5006
#define MTP_DEV_PROP_F_NUMBER                                   0x5007
#define MTP_DEV_PROP_FOCAL_LENGTH                               0x5008
#define MTP_DEV_PROP_FOCUS_DISTANCE                             0x5009
#define MTP_DEV_PROP_FOCUS_MODE                                 0x500A
#define MTP_DEV_PROP_EXPOSURE_METERING_MODE                     0x500B
#define MTP_DEV_PROP_FLASH_MODE                                 0x500C
#define MTP_DEV_PROP_EXPOSURE_TIME                              0x500D
#define MTP_DEV_PROP_EXPOSURE_PROGRAM_MODE                      0x500E
#define MTP_DEV_PROP_EXPOSURE_INDEX                             0x500F
#define MTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION                 0x5010
#define MTP_DEV_PROP_DATE_TIME                                  0x5011
#define MTP_DEV_PROP_CAPTURE_DELAY                              0x5012
#define MTP_DEV_PROP_STILL_CAPTURE_MODE                         0x5013
#define MTP_DEV_PROP_CONTRAST                                   0x5014
#define MTP_DEV_PROP_SHARPNESS                                  0x5015
#define MTP_DEV_PROP_DIGITAL_ZOOM                               0x5016
#define MTP_DEV_PROP_EFFECT_MODE                                0x5017
#define MTP_DEV_PROP_BURST_NUMBER                               0x5018
#define MTP_DEV_PROP_BURST_INTERVAL                             0x5019
#define MTP_DEV_PROP_TIME_LAPSE_NUMBER                          0x501A
#define MTP_DEV_PROP_TIME_LAPSE_INTERVAL                        0x501B
#define MTP_DEV_PROP_FOCUS_METERING_MODE                        0x501C
#define MTP_DEV_PROP_UPLOAD_URL                                 0x501D
#define MTP_DEV_PROP_ARTIST                                     0x501E
#define MTP_DEV_PROP_COPYRIGHT_INFO                             0x501F
#define MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG                      0xD303
#define MTP_DEV_PROP_SYNCHRONIZATION_PARTNER                    0xD401
#define MTP_DEV_PROP_DEVICE_FRIENDLY_NAME                       0xD402
#define MTP_DEV_PROP_VOLUME                                     0xD403
#define MTP_DEV_PROP_SUPPORTED_FORMATS_ORDERED                  0xD404
#define MTP_DEV_PROP_DEVICE_ICON                                0xD405
#define MTP_DEV_PROP_PLAYBACK_RATE                              0xD410
#define MTP_DEV_PROP_PLAYBACK_OBJECT                            0xD411
#define MTP_DEV_PROP_PLAYBACK_CONTAINER                         0xD412
#define MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO             0xD406
#define MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE                      0xD407


#define MTP_OBJECT_PROP_STORAGEID                               0xDC01
#define MTP_OBJECT_PROP_OBJECT_FORMAT                           0xDC02
#define MTP_OBJECT_PROP_PROTECTION_STATUS                       0xDC03
#define MTP_OBJECT_PROP_OBJECT_SIZE                             0xDC04
#define MTP_OBJECT_PROP_ASSOCIATION_TYPE                        0xDC05
#define MTP_OBJECT_PROP_ASSOCIATION_DESC                        0xDC06
#define MTP_OBJECT_PROP_OBJECT_FILE_NAME                        0xDC07
#define MTP_OBJECT_PROP_DATE_CREATED                            0xDC08
#define MTP_OBJECT_PROP_DATE_MODIFIED                           0xDC09
#define MTP_OBJECT_PROP_KEYWORDS                                0xDC0A
#define MTP_OBJECT_PROP_PARENT_OBJECT                           0xDC0B
#define MTP_OBJECT_PROP_ALLOWED_FOLDER_CONTENTS                 0xDC0C
#define MTP_OBJECT_PROP_HIDDEN                                  0xDC0D
#define MTP_OBJECT_PROP_SYSTEM_OBJECT                           0xDC0E
#define MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER     0xDC41
#define MTP_OBJECT_PROP_SYNCID                                  0xDC42
#define MTP_OBJECT_PROP_PROPERTY_BAG                            0xDC43
#define MTP_OBJECT_PROP_NAME                                    0xDC44
#define MTP_OBJECT_PROP_CREATED_BY                              0xDC45
#define MTP_OBJECT_PROP_ARTIST                                  0xDC46
#define MTP_OBJECT_PROP_DATE_AUTHORED                           0xDC47
#define MTP_OBJECT_PROP_DESCRIPTION                             0xDC48
#define MTP_OBJECT_PROP_URL_REFERENCE                           0xDC49
#define MTP_OBJECT_PROP_LANGUAGE_LOCALE                         0xDC4A
#define MTP_OBJECT_PROP_COPYRIGHT_INFORMATION                   0xDC4B
#define MTP_OBJECT_PROP_SOURCE                                  0xDC4C
#define MTP_OBJECT_PROP_ORIGIN_LOCATION                         0xDC4D
#define MTP_OBJECT_PROP_DATE_ADDED                              0xDC4E
#define MTP_OBJECT_PROP_NON_CONSUMABLE                          0xDC4F
#define MTP_OBJECT_PROP_CORRUPT_UNPLAYABLE                      0xDC50
#define MTP_OBJECT_PROP_PRODUCER_SERIA_LNUMBER                  0xDC51
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_FORMAT            0xDC81
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_SIZE              0xDC82
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_HEIGHT            0xDC83
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_WIDTH             0xDC84
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_DURATION          0xDC85
#define MTP_OBJECT_PROP_REPRESENTATIVE_SAMPLE_DATA              0xDC86
#define MTP_OBJECT_PROP_WIDTH                                   0xDC87
#define MTP_OBJECT_PROP_HEIGHT                                  0xDC88
#define MTP_OBJECT_PROP_DURATION                                0xDC89
#define MTP_OBJECT_PROP_RATING                                  0xDC8A
#define MTP_OBJECT_PROP_TRACK                                   0xDC8B
#define MTP_OBJECT_PROP_GENRE                                   0xDC8C
#define MTP_OBJECT_PROP_CREDITS                                 0xDC8D
#define MTP_OBJECT_PROP_LYRICS                                  0xDC8E
#define MTP_OBJECT_PROP_SUBSCRIPTION_CONTENT_ID                 0xDC8F
#define MTP_OBJECT_PROP_PRODUCED_BY                             0xDC90
#define MTP_OBJECT_PROP_USE_COUNT                               0xDC91
#define MTP_OBJECT_PROP_SKIP_COUNT                              0xDC92
#define MTP_OBJECT_PROP_LAST_ACCESSED                           0xDC93
#define MTP_OBJECT_PROP_PARENTAL_RATING                         0xDC94
#define MTP_OBJECT_PROP_META_GENRE                              0xDC95
#define MTP_OBJECT_PROP_COMPOSER                                0xDC96
#define MTP_OBJECT_PROP_EFFECTIVE_RATING                        0xDC97
#define MTP_OBJECT_PROP_SUBTITLE                                0xDC98
#define MTP_OBJECT_PROP_ORIGINAL_RELEASE_DATE                   0xDC99
#define MTP_OBJECT_PROP_ALBUM_NAME                              0xDC9A
#define MTP_OBJECT_PROP_ALBUM_ARTIST                            0xDC9B
#define MTP_OBJECT_PROP_MOOD                                    0xDC9C
#define MTP_OBJECT_PROP_DRM_STATUS                              0xDC9D
#define MTP_OBJECT_PROP_SUB_DESCRIPTION                         0xDC9E
#define MTP_OBJECT_PROP_IS_CROPPED                              0xDCD1
#define MTP_OBJECT_PROP_IS_COLOUR_CORRECTED                     0xDCD2
#define MTP_OBJECT_PROP_IMAGE_BIT_DEPTH                         0xDCD3
#define MTP_OBJECT_PROP_FNUMBER                                 0xDCD4
#define MTP_OBJECT_PROP_EXPOSURE_TIME                           0xDCD5
#define MTP_OBJECT_PROP_EXPOSURE_INDEX                          0xDCD6
#define MTP_OBJECT_PROP_TOTAL_BITRATE                           0xDE91
#define MTP_OBJECT_PROP_BITRATE_TYPE                            0xDE92
#define MTP_OBJECT_PROP_SAMPLE_RATE                             0xDE93
#define MTP_OBJECT_PROP_NUMBER_OF_CHANNELS                      0xDE94
#define MTP_OBJECT_PROP_AUDIO_BITDEPTH                          0xDE95
#define MTP_OBJECT_PROP_SCAN_TYPE                               0xDE97
#define MTP_OBJECT_PROP_AUDIO_WAVE_CODEC                        0xDE99
#define MTP_OBJECT_PROP_AUDIO_BITRATE                           0xDE9A
#define MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC                      0xDE9B
#define MTP_OBJECT_PROP_VIDEO_BITRATE                           0xDE9C
#define MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS             0xDE9D
#define MTP_OBJECT_PROP_KEYFRAME_DISTANCE                       0xDE9E
#define MTP_OBJECT_PROP_BUFFER_SIZE                             0xDE9F
#define MTP_OBJECT_PROP_ENCODING_QUALITY                        0xDEA0
#define MTP_OBJECT_PROP_ENCODING_PROFILE                        0xDEA1
#define MTP_OBJECT_PROP_DISPLAY_NAME                            0xDCE0
#define MTP_OBJECT_PROP_BODY_TEXT                               0xDCE1
#define MTP_OBJECT_PROP_SUBJECT                                 0xDCE2
#define MTP_OBJECT_PROP_PRIORITY                                0xDCE3
#define MTP_OBJECT_PROP_GIVEN_NAME                              0xDD00
#define MTP_OBJECT_PROP_MIDDLE_NAMES                            0xDD01
#define MTP_OBJECT_PROP_FAMILY_NAME                             0xDD02
#define MTP_OBJECT_PROP_PREFIX                                  0xDD03
#define MTP_OBJECT_PROP_SUFFIX                                  0xDD04
#define MTP_OBJECT_PROP_PHONETIC_GIVEN_NAME                     0xDD05
#define MTP_OBJECT_PROP_PHONETIC_FAMILY_NAME                    0xDD06
#define MTP_OBJECT_PROP_EMAIL_PRIMARY                           0xDD07
#define MTP_OBJECT_PROP_EMAIL_PERSONAL_1                        0xDD08
#define MTP_OBJECT_PROP_EMAIL_PERSONAL_2                        0xDD09
#define MTP_OBJECT_PROP_EMAIL_BUSINESS_1                        0xDD0A
#define MTP_OBJECT_PROP_EMAIL_BUSINESS_2                        0xDD0B
#define MTP_OBJECT_PROP_EMAIL_OTHERS                            0xDD0C
#define MTP_OBJECT_PROP_PHONE_NUMBER_PRIMARY                    0xDD0D
#define MTP_OBJECT_PROP_PHONE_NUMBER_PERSONAL                   0xDD0E
#define MTP_OBJECT_PROP_PHONE_NUMBER_PERSONAL_2                 0xDD0F
#define MTP_OBJECT_PROP_PHONE_NUMBER_BUSINESS                   0xDD10
#define MTP_OBJECT_PROP_PHONE_NUMBER_BUSINESS_2                 0xDD11
#define MTP_OBJECT_PROP_PHONE_NUMBER_MOBILE                     0xDD12
#define MTP_OBJECT_PROP_PHONE_NUMBER_MOBILE_2                   0xDD13
#define MTP_OBJECT_PROP_FAX_NUMBER_PRIMARY                      0xDD14
#define MTP_OBJECT_PROP_FAX_NUMBER_PERSONAL                     0xDD15
#define MTP_OBJECT_PROP_FAX_NUMBER_BUSINESS                     0xDD16
#define MTP_OBJECT_PROP_PAGER_NUMBER                            0xDD17
#define MTP_OBJECT_PROP_PHONE_NUMBER_OTHERS                     0xDD18
#define MTP_OBJECT_PROP_PRIMARY_WEB_ADDRESS                     0xDD19
#define MTP_OBJECT_PROP_PERSONAL_WEB_ADDRESS                    0xDD1A
#define MTP_OBJECT_PROP_BUSINESS_WEB_ADDRESS                    0xDD1B
#define MTP_OBJECT_PROP_INSTANT_MESSENGER_ADDRESS               0xDD1C
#define MTP_OBJECT_PROP_INSTANT_MESSENGER_ADDRESS_2             0xDD1D
#define MTP_OBJECT_PROP_INSTANT_MESSENGER_ADDRESS_3             0xDD1E
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_FULL            0xDD1F
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_LINE_1          0xDD20
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_LINE_2          0xDD21
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_CITY            0xDD22
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_REGION          0xDD23
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_POSTAL_CODE     0xDD24
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_PERSONAL_COUNTRY         0xDD25
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_FULL            0xDD26
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_LINE_1          0xDD27
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_LINE_2          0xDD28
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_CITY            0xDD29
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_REGION          0xDD2A
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_POSTAL_CODE     0xDD2B
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_BUSINESS_COUNTRY         0xDD2C
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_FULL               0xDD2D
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_LINE_1             0xDD2E
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_LINE_2             0xDD2F
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_CITY               0xDD30
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_REGION             0xDD31
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_POSTAL_CODE        0xDD32
#define MTP_OBJECT_PROP_POSTAL_ADDRESS_OTHER_COUNTRY            0xDD33
#define MTP_OBJECT_PROP_ORGANIZATION_NAME                       0xDD34
#define MTP_OBJECT_PROP_PHONETIC_ORGANIZATION_NAME              0xDD35
#define MTP_OBJECT_PROP_ROLE                                    0xDD36
#define MTP_OBJECT_PROP_BIRTHDATE                               0xDD37
#define MTP_OBJECT_PROP_MESSAGE_TO                              0xDD40
#define MTP_OBJECT_PROP_MESSAGE_CC                              0xDD41
#define MTP_OBJECT_PROP_MESSAGE_BCC                             0xDD42
#define MTP_OBJECT_PROP_MESSAGE_READ                            0xDD43
#define MTP_OBJECT_PROP_MESSAGE_RECEIVED_TIME                   0xDD44
#define MTP_OBJECT_PROP_MESSAGE_SENDER                          0xDD45
#define MTP_OBJECT_PROP_ACTIVITY_BEGIN_TIME                     0xDD50
#define MTP_OBJECT_PROP_ACTIVITY_END_TIME                       0xDD51
#define MTP_OBJECT_PROP_ACTIVITY_LOCATION                       0xDD52
#define MTP_OBJECT_PROP_ACTIVITY_REQUIRED_ATTENDEES             0xDD54
#define MTP_OBJECT_PROP_ACTIVITY_OPTIONAL_ATTENDEES             0xDD55
#define MTP_OBJECT_PROP_ACTIVITY_RESOURCES                      0xDD56
#define MTP_OBJECT_PROP_ACTIVITY_ACCEPTED                       0xDD57
#define MTP_OBJECT_PROP_OWNER                                   0xDD5D
#define MTP_OBJECT_PROP_EDITOR                                  0xDD5E
#define MTP_OBJECT_PROP_WEBMASTER                               0xDD5F
#define MTP_OBJECT_PROP_URL_SOURCE                              0xDD60
#define MTP_OBJECT_PROP_URL_DESTINATION                         0xDD61
#define MTP_OBJECT_PROP_TIME_BOOKMARK                           0xDD62
#define MTP_OBJECT_PROP_OBJECT_BOOKMARK                         0xDD63
#define MTP_OBJECT_PROP_BYTE_BOOKMARK                           0xDD64
#define MTP_OBJECT_PROP_LAST_BUILD_DATE                         0xDD70
#define MTP_OBJECT_PROP_TIME_TO_LIVE                            0xDD71
#define MTP_OBJECT_PROP_MEDIA_GUID                              0xDD72

/* Define PIMA Response Codes.  */

#define MTP_RC_UNDEFINED                                        0x2000
#define MTP_RC_OK                                               0x2001
#define MTP_RC_GENERAL_ERROR                                    0x2002
#define MTP_RC_SESSION_NOT_OPEN                                 0x2003
#define MTP_RC_INVALID_TRANSACTION_ID                           0x2004
#define MTP_RC_OPERATION_NOT_SUPPORTED                          0x2005
#define MTP_RC_PARAMETER_NOT_SUPPORTED                          0x2006
#define MTP_RC_INCOMPLETE_TRANSFER                              0x2007
#define MTP_RC_INVALID_STORAGE_ID                               0x2008
#define MTP_RC_INVALID_OBJECT_HANDLE                            0x2009
#define MTP_RC_DEVICE_PROP_NOT_SUPPORTED                        0x200A
#define MTP_RC_INVALID_OBJECT_FORMAT_CODE                       0x200B
#define MTP_RC_STORE_FULL                                       0x200C
#define MTP_RC_OBJECT_WRITE_PROTECTED                           0x200D
#define MTP_RC_STORE_READ_ONLY                                  0x200E
#define MTP_RC_ACCESS_DENIED                                    0x200F
#define MTP_RC_NO_THUMBNAIL_PRESENT                             0x2010
#define MTP_RC_SELF_TEST_FAILED                                 0x2011
#define MTP_RC_PARTIAL_DELETION                                 0x2012
#define MTP_RC_STORE_NOT_AVAILABLE                              0x2013
#define MTP_RC_FORMAT_UNSUPPORTED                               0x2014
#define MTP_RC_NO_VALID_OBJECT_INFO                             0x2015
#define MTP_RC_INVALID_CODE_FORMAT                              0x2016
#define MTP_RC_UNKNOWN_VENDOR_CODE                              0x2017
#define MTP_RC_CAPTURE_ALREADY_TERMINATED                       0x2018
#define MTP_RC_DEVICE_BUSY                                      0x2019
#define MTP_RC_INVALID_PARENT_OBJECT                            0x201A
#define MTP_RC_INVALID_DEVICE_PROP_FORMAT                       0x201B
#define MTP_RC_INVALID_DEVICE_PROP_VALUE                        0x201C
#define MTP_RC_INVALID_PARAMETER                                0x201D
#define MTP_RC_SESSION_ALREADY_OPENED                           0x201E
#define MTP_RC_TRANSACTION_CANCELED                             0x201F
#define MTP_RC_DESTINATION_UNSUPPORTED                          0x2020
#define MTP_RC_OBJECT_ALREADY_OPENED                            0x2021
#define MTP_RC_OBJECT_ALREADY_CLOSED                            0x2022
#define MTP_RC_OBJECT_NOT_OPENED                                0x2023
#define MTP_RC_INVALID_OBJECT_PROP_CODE                         0xA801
#define MTP_RC_INVALID_OBJECT_PROP_FORMAT                       0xA802
#define MTP_RC_INVALID_OBJECT_PROP_VALUE                        0xA803
#define MTP_RC_INVALID_OBJECT_REFERENCE                         0xA804
#define MTP_RC_INVALID_DATASET                                  0xA806
#define MTP_RC_SPECIFICATION_BY_GROUP_UNSUPPORTED               0xA807
#define MTP_RC_SPECIFICATION_BY_DEPTH_UNSUPPORTED               0xA808
#define MTP_RC_OBJECT_TOO_LARGE                                 0xA809
#define MTP_RC_OBJECT_PROP_NOT_SUPPORTED                        0xA80A

/* Define PIMA object and thumb transfer phase.  */

#define MTP_OBJECT_TRANSFER_PHASE_ACTIVE                        0
#define MTP_OBJECT_TRANSFER_PHASE_COMPLETED                     1
#define MTP_OBJECT_TRANSFER_PHASE_COMPLETED_ERROR               2

/* Define PIMA MTP OBJECT PROPERTY DATASET.  */
#define MTP_OBJECT_PROPERTY_DATASET_CODE                        0x0000
#define MTP_OBJECT_PROPERTY_DATASET_DATATYPE                    0x0002
#define MTP_OBJECT_PROPERTY_DATASET_GETSET                      0x0004
#define MTP_OBJECT_PROPERTY_DATASET_VALUE                       0x0005

/* Define PIMA types.  */
#define MTP_TYPES_INT8                                          0x0001
#define MTP_TYPES_UINT8                                         0x0002
#define MTP_TYPES_INT16                                         0x0003
#define MTP_TYPES_UINT16                                        0x0004
#define MTP_TYPES_INT32                                         0x0005
#define MTP_TYPES_UINT32                                        0x0006
#define MTP_TYPES_INT64                                         0x0007
#define MTP_TYPES_UINT64                                        0x0008
#define MTP_TYPES_INT128                                        0x0009
#define MTP_TYPES_UINT128                                       0x000A
#define MTP_TYPES_AINT8                                         0x4001
#define MTP_TYPES_AUINT8                                        0x4002
#define MTP_TYPES_AINT16                                        0x4003
#define MTP_TYPES_AUINT16                                       0x4004
#define MTP_TYPES_AINT32                                        0x4005
#define MTP_TYPES_AUINT32                                       0x4006
#define MTP_TYPES_AINT64                                        0x4007
#define MTP_TYPES_AUINT64                                       0x4008
#define MTP_TYPES_AINT128                                       0x4009
#define MTP_TYPES_AUINT128                                      0x400A
#define MTP_TYPES_STR                                           0xFFFF

/* Define PIMA Event Codes.  */

#define MTP_EC_UNDEFINED                                        0x4000
#define MTP_EC_CANCEL_TRANSACTION                              0x4001
#define MTP_EC_OBJECT_ADDED                                     0x4002
#define MTP_EC_OBJECT_REMOVED                                   0x4003
#define MTP_EC_STORE_ADDED                                      0x4004
#define MTP_EC_STORE_REMOVED                                    0x4005
#define MTP_EC_DEVICE_PROP_CHANGED                             0x4006
#define MTP_EC_OBJECT_INFO_CHANGED                             0x4007
#define MTP_EC_DEVICE_INFO_CHANGED                             0x4008
#define MTP_EC_REQUEST_OBJECT_TRANSFER                         0x4009
#define MTP_EC_STORE_FULL                                        0x400A
#define MTP_EC_DEVICE_RESET                                      0x400B
#define MTP_EC_STORAGE_INFO_CHANGED                             0x400C
#define MTP_EC_CAPTURE_COMPLETE                                 0x400D
#define UMTP_EC_UNREPORTED_STATUS                               0x400E

/* Define PIMA Dataset equivalences.  */
#define MTP_OBJECT_PROPERTY_DATASET_VALUE_GET                   0x00
#define MTP_OBJECT_PROPERTY_DATASET_VALUE_GETSET                0x01

/* Define local Audio codecs.  */
#define MTP_AUDIO_CODEC_WAVE_FORMAT_MPEGLAYER3                  0x00000055
#define MTP_AUDIO_CODEC_WAVE_FORMAT_MPEG                        0x00000050
#define MTP_AUDIO_CODEC_WAVE_FORMAT_RAW_AAC1                    0x000000FF

#define MTP_DATASET_VALUE_OFFSET                                6
#define MTP_MAX_HANDLES                                          20
/* Define PIMA object info structure.  */

typedef struct _MTP_OBJECT_INFO_s_ {
    UINT8  FileName[MTP_MAX_FILENAME_SIZE];
    UINT32 format;
    UINT32 size;
    UINT32 BitDepth;
    UINT32 height;
    UINT32 width;
    UINT32 WriteProtect;
} MTP_OBJECT_INFO_s;

/* Structure of the Object property dataset.  This is specific to the local device. */
typedef struct _MTP_OBJECT_PROP_DATASET_s_ {
    UINT32      StorageId;
    UINT32      ObjectFormat;
    UINT32      ProtectionStatus;
    UINT32      ObjectSizeLow;
    UINT32      ObjectSizeHigh;
    UINT8       ObjectFileName[128];
    UINT32      ParentObject;
    UINT32      PersistentUniqueObjectIdentifier[4];
    UINT8       name[128];
    UINT8       NonConsumable;
    UINT8       artist[128];
    UINT32      track;
    UINT32      UseCount;
    UINT8       authored[16];
    UINT8       genre[128];
    UINT8       AlbumName[128];
    UINT8       AlbumArtist[128];
    UINT32      SampleRate;
    UINT32      NumOfChannels;
    UINT32      AudioWaveCodec;
    UINT32      AudioBitrate;
    UINT32      duration;
    UINT32      width;
    UINT32      height;
    UINT32      ScanType;
    UINT32      FourccCodec;
    UINT32      VideoBitrate;
    UINT32      FramePerThousandSeconds;
    UINT32      KeyframeDistance;
    UINT8       EncodingProfile[128];
} MTP_OBJECT_PROP_DATASET_s;


typedef struct _MTP_CLASS_INFO_s_ {
    UINT32                  ObjectNumberHandles;
    UINT32                  ImageNumber;
    UINT32                  OthersNumber;
    MTP_DEVICE_INFO_s       DeviceInfo;
    AMBA_FS_FILE            *FpArray;
} MTP_CLASS_INFO_s;

#if 0
typedef struct _MTP_PARSE_OBJECT_INFO_s_ {
    AMBA_DCF_FILE_TYPE_e FileType;
    AMBA_MOV_KEY_FRAME_INFO_s MovInfo;
    AMBA_EXIF_IMAGE_INFO_s ImgInfo;
} MTP_PARSE_OBJECT_INFO_s;
#endif

#endif
