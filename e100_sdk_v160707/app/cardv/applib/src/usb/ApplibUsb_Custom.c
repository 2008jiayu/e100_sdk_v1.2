/**
 * @file src/app/connected/applib/src/usb/ApplibUsb_Custom.c
 *
 *  USB custom device info
 *
 * History:
 *    2013/11/29 - [Martin Lai] created file
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
#include <AmbaDataType.h>
#include <usb/AmbaUSB_API.h>
#include <AmbaCardManager.h>


// define MTP VENDOR REQUEST
#define MTP_VENDOR_REQUEST         0x54

/*-----------------------------------------------------------------------------------------------*\
   Multiple languages are supported on the device, to add
   a language besides English, the unicode language code must
   be appended to the LanguageIdFramework array and the length
   adjusted accordingly.
\*-----------------------------------------------------------------------------------------------*/

UINT8 LangID[] = {

    /* English. */
    0x09, 0x04
};

/*-----------------------------------------------------------------------------------------------*\
 * Mass Storage Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/

UINT8 __attribute__((aligned(32))) MscDescFs[] = {

    /* Device descriptor */
    0x12,       // this descriptor size
    0x01,       // device descriptor type
    0x10, 0x01, // Spec version
    0x00,       // class code
    0x00,       // subclass code
    0x00,       // procotol code
    0x40,       // max packet size
    0xec, 0x08, // VID
    0x10, 0x00, // PID
    0x00, 0x00, // Device release num
    0x01,       // Manufacturer string index
    0x02,       // Product string index
    0x03,       // device serial number index
    0x01,       // number of possible configuration

    /* Configuration descriptor */
    0x09,       // this descriptor size
    0x02,       // configuration descriptor type
    0x20, 0x00, // total length
    0x01,       // config number of interface
    0x01,       // config value
    0x00,       // config index
    0xc0,       // attribute
    0x32,       // max power unit=2mA

    /* Interface descriptor */
    0x09,       // this interface descriptor size
    0x04,       // interface descriptor type
    0x00,       // interface number
    0x00,       // alternative settings number
    0x02,       // endpoint number of this interface
    0x08,       // class code
    0x06,       // subclass code
    0x50,       // protocol
    0x00,       // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,       // this endpoint descriptor size
    0x05,       // endpoint descriptor type
    0x01,       // EP1 address
    0x02,       // endpoint attribute = BULK
    0x40, 0x00, // max packet size
    0x00,       // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,       // this descriptor size
    0x05,       // endpoint descriptor type
    0x82,       // EP2 address
    0x02,       // endpoint attribute = BULK
    0x40, 0x00, // max packet size
    0x00        // interval
};

UINT8 __attribute__((aligned(32))) MscDescHs[] = {

    /* Device descriptor */
    0x12,       // this descriptor size
    0x01,       // device descriptor type
    0x00, 0x02, // Spec version
    0x00,       // class code
    0x00,       // subclass code
    0x00,       // procotol code
    0x40,       // max packet size
    0x0a, 0x07, // VID
    0x26, 0x40, // PID
    0x01, 0x00, // Device release num
    0x01,       // Manufacturer string index
    0x02,       // Product string index
    0x03,       // device serial number index
    0x01,       // number of possible configuration

    /* Device qualifier descriptor */
    0x0a,       // this descriptor size
    0x06,       // descriptor type
    0x00, 0x02, // Spec version
    0x00,       // class code
    0x00,       // subclass code
    0x00,       // procotol code
    0x40,       // max packet size
    0x01,       // configuration number
    0x00,       // reserved

    /* Configuration descriptor */
    0x09,       // this descriptor size
    0x02,       // descriptor type
    0x20, 0x00, // total length
    0x01,       // config number of interface
    0x01,       // config value
    0x00,       // config index
    0xc0,       // attribute
    0x32,       // max power unit=2mA

    /* Interface descriptor */
    0x09,       // this descriptor size
    0x04,       // descriptor type
    0x00,       // interface number
    0x00,       // alternative settings number
    0x02,       // endpoint number of this interface
    0x08,       // class code
    0x06,       // subclass code
    0x50,       // protocol
    0x00,       // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,       // this descriptor size
    0x05,       // type
    0x01,       // EP1 address
    0x02,       // endpoint attribute = BULK
    0x00, 0x02, // max packet size
    0x00,       // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,       // this descriptor size
    0x05,       // type
    0x82,       // EP2 address
    0x02,       // endpoint attribute = BULK
    0x00, 0x02, // max packet size
    0x00        // interval
};

/* String Device Framework :
 Byte 0 and 1 : Word containing the language ID : 0x0904 for US
 Byte 2       : Byte containing the index of the descriptor
 Byte 3       : Byte containing the length of the descriptor string
*/
UINT8 __attribute__((aligned(32))) AppLibUSBMscStr[] = {

    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04, 0x01, 0x0c,
    0x41, 0x6d, 0x62, 0x61,0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x49, 0x6e, 0x63,

    /* Product string descriptor : Index 2 */
    0x09, 0x04, 0x02, 0x0c,
    0x41, 0x39, 0x20, 0x50, 0x6c, 0x61, 0x74, 0x66,
    0x6f, 0x72, 0x6d, 0x20,

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31
};

/*-----------------------------------------------------------------------------------------------*\
 * MTP/PTP Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/
UINT8 __attribute__((aligned(32))) MtpDescFs[] = {

    /* Device descriptor */
    0x12,           // this descriptor size
    0x01,           // device descriptor type
    0x10, 0x01,     // Spec version
    0x00,           // class code
    0x00,           // subclass code
    0x00,           // procotol code
    0x08,           // max packet size
    0xE8, 0x04,     // VID
    0xC5, 0x68,     // PID
    0x00, 0x00,     // Device release num
    0x00,           // Manufacturer string index
    0x00,           // Product string index
    0x00,           // device serial number index
    0x01,           // number of possible configuration

    /* Configuration descriptor */
    0x09,           // this descriptor size
    0x02,           // descriptor type
    0x27, 0x00,     // total length
    0x01,           // config number of interface
    0x01,           // config value
    0x00,           // config index
    0xc0,           // attribute
    0x32,           // max power unit=2mA

    /* Interface descriptor */
    0x09,           // this descriptor size
    0x04,           // descriptor type
    0x00,           // interface number
    0x00,           // alternative settings number
    0x03,           // endpoint number of this interface
    0x06,           // class code
    0x01,           // subclass code
    0x01,           // protocol
    0x00,           // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,           // this descriptor size
    0x05,           // type
    0x02,           // EP2 address
    0x02,           // endpoint attribute = BULK
    0x40, 0x00,     // max packet size
    0x00,           // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,           // this descriptor size
    0x05,           // type
    0x81,           // EP1 address
    0x02,           // endpoint attribute = BULK
    0x40, 0x00,     // max packet size
    0x00,           // interval

    /* Endpoint descriptor (Interrupt In) */
    0x07,           // this descriptor size
    0x05,           // type
    0x83,           // EP3 address
    0x03,           // endpoint attribute = INTERRUPT
    0x40, 0x00,     // max packet size
    0x04            // interval
};

UINT8 __attribute__((aligned(32))) MtpDescHs[] = {

    /* Device descriptor */
    0x12,           // this descriptor size
    0x01,           // device descriptor type
    0x00, 0x02,     // Spec version
    0x00,           // class code
    0x00,           // subclass code
    0x00,           // procotol code
    0x40,           // max packet size
    0xE8, 0x04,     // VID
    0xC5, 0x68,     // PID
    0x01, 0x00,     // Device release num
    0x01,           // Manufacturer string index
    0x02,           // Product string index
    0x03,           // device serial number index
    0x01,           // number of possible configuration

    /* Device qualifier descriptor */
    0x0a,           // this descriptor size
    0x06,           // descriptor type
    0x00, 0x02,     // Spec version
    0x00,           // class code
    0x00,           // subclass code
    0x00,           // procotol code
    0x40,           // max packet size
    0x01,           // configuration number
    0x00,           // reserved

    /* Configuration descriptor */
    0x09,           // this descriptor size
    0x02,           // descriptor type
    0x27, 0x00,     // total length
    0x01,           // config number of interface
    0x01,           // config value
    0x00,           // config index
    0xc0,           // attribute
    0x32,           // max power unit=2mA

    /* Interface descriptor */
    0x09,           // this descriptor size
    0x04,           // descriptor type
    0x00,           // interface number
    0x00,           // alternative settings number
    0x03,           // endpoint number of this interface
    0x06,           // class code
    0x01,           // subclass code
    0x01,           // protocol
    0x00,           // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,           // this descriptor size
    0x05,           // type
    0x01,           // EP1 address
    0x02,           // endpoint attribute = BULK
    0x00, 0x02,     // max packet size
    0x00,           // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,           // this descriptor size
    0x05,           // type
    0x82,           // EP2 address
    0x02,           // endpoint attribute = BULK
    0x00, 0x02,     // max packet size
    0x00,           // interval

    /* Endpoint descriptor (Interrupt In) */
    0x07,           // this descriptor size
    0x05,           // type
    0x83,           // EP3 address
    0x03,           // endpoint attribute = INTERRUPT
    0x40, 0x00,     // max packet size
    0x04            // interval
};

/* String Device Framework :
 Byte 0 and 1 : Word containing the language ID : 0x0904 for US or 0x0000 for none.
 Byte 2       : Byte containing the index of the descriptor
 Byte 3       : Byte containing the length of the descriptor string

 The last string entry can be the optional Microsoft String descriptor.
*/

UINT8 __attribute__((aligned(32))) MtpStr[] = {

    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04, 0x01, 0x0c,
    0x41, 0x6d, 0x62, 0x61,0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x49, 0x6e, 0x63,

    /* Product string descriptor : Index 2 */
    0x09, 0x04, 0x02, 0x0c,
    0x41, 0x39, 0x20, 0x50, 0x6c, 0x61, 0x74, 0x66,
    0x6f, 0x72, 0x6d, 0x20,

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31,

    /* Microsoft OS string descriptor : Index 0xEE. String is MSFT100.
       The last byte is the vendor code used to filter Vendor specific commands.
       The vendor commands will be executed in the class.
       This code can be anything but must not be 0x66 or 0x67 which are PIMA class commands.  */
    0x00, 0x00, 0xEE, 0x08,
    0x4D, 0x53, 0x46, 0x54,
    0x31, 0x30, 0x30,
    MTP_VENDOR_REQUEST

};


/*-----------------------------------------------------------------------------------------------*\
 * PictBridge Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/

UINT8 __attribute__((aligned(32))) PictBridgeDescFs[] = {

    /* Device descriptor */
    0x12,               // this descriptor size
    0x01,               // device descriptor type
    0x10, 0x01,         // Spec version
    0x00,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x20,               // max packet size
    0xA9, 0x04,         // VID
    0xB6, 0x32,         // PID
    0x00, 0x00,         // Device release num
    0x01,               // Manufacturer string index
    0x02,               // Product string index
    0x03,               // device serial number index
    0x01,               // number of possible configuration

    /* Configuration descriptor */
    0x09,               // this descriptor size
    0x02,               // descriptor type
    0x27, 0x00,         // total length
    0x01,               // config number of interface
    0x01,               // config value
    0x00,               // config index
    0xc0,               // attribute
    0x32,               // max power unit=2mA

    /* Interface descriptor */
    0x09,               // this descriptor size
    0x04,               // descriptor type
    0x00,               // interface number
    0x00,               // alternative settings number
    0x03,               // endpoint number of this interface
    0x06,               // class code
    0x01,               // subclass code
    0x01,               // protocol
    0x00,               // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,               // this descriptor size
    0x05,               // type
    0x01,               // EP1 address
    0x02,               // endpoint attribute = BULK
    0x40, 0x00,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,               // this descriptor size
    0x05,               // type
    0x82,               // EP2 address
    0x02,               // endpoint attribute = BULK
    0x40, 0x00,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Interrupt) */
    0x07,               // this descriptor size
    0x05,               // type
    0x83,               // EP3 address
    0x03,               // endpoint attribute = INTERRUPT
    0x20, 0x00,         // max packet size
    0x60                // interval
};

UINT8 __attribute__((aligned(32))) PictBridgeDescHs[] = {

    /* Device descriptor */
    0x12,               // this descriptor size
    0x01,               // device descriptor type
    0x00, 0x02,         // Spec version
    0x00,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x40,               // max packet size
    0xA9, 0x04,         // VID
    0xB6, 0x32,         // PID
    0x01, 0x00,         // Device release num
    0x01,               // Manufacturer string index
    0x02,               // Product string index
    0x03,               // device serial number index
    0x01,               // number of possible configuration

    /* Device qualifier descriptor */
    0x0a,               // this descriptor size
    0x06,               // device descriptor type
    0x00, 0x02,         // Spec version
    0x00,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x40,               // max packet size
    0x01,               // configuration number
    0x00,               // reserved

    /* Configuration descriptor */
    0x09,               // this descriptor size
    0x02,               // device descriptor type
    0x27, 0x00,         // total length
    0x01,               // config number of interface
    0x01,               // config value
    0x00,               // config index
    0xc0,               // attribute
    0x32,               // max power unit=2mA

    /* Interface descriptor */
    0x09,               // this descriptor size
    0x04,               // device descriptor type
    0x00,               // interface number
    0x00,               // alternative settings number
    0x03,               // endpoint number of this interface
    0x06,               // class code
    0x01,               // subclass code
    0x01,               // protocol
    0x00,               // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,               // this descriptor size
    0x05,               // type
    0x01,               // EP1 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,               // this descriptor size
    0x05,               // type
    0x82,               // EP2 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Interrupt) */
    0x07,               // this descriptor size
    0x05,               // type
    0x83,               // EP3 address
    0x03,               // endpoint attribute = Interrupt
    0x20, 0x00,         // max packet size
    0x60                // interval
};



/* String Device Framework :
     Byte 0 and 1 : Word containing the language ID : 0x0904 for US
     Byte 2       : Byte containing the index of the descriptor
     Byte 3       : Byte containing the length of the descriptor string
*/
UINT8 __attribute__((aligned(32))) PictBridgeStr[] = {

    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04, 0x01, 0x0c,
    0x41, 0x6d, 0x62, 0x61,0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x49, 0x6e, 0x63,

    /* Product string descriptor : Index 2 */
    0x09, 0x04, 0x02, 0x0c,
    0x41, 0x39, 0x20, 0x50, 0x6c, 0x61, 0x74, 0x66,
    0x6f, 0x72, 0x6d, 0x20,

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31
};

/*-----------------------------------------------------------------------------------------------*\
 * CDC-ACM Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/

UINT8 __attribute__((aligned(32))) CdcAcmDescFs[] = {
    /* Device descriptor */
    0x12,               // this descriptor size
    0x01,               // device descriptor type
    0x10, 0x01,         // Spec version
    0x02,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x20,               // max packet size
    0x55, 0x42,         // VID
    0x52, 0x00,         // PID
    0x00, 0x00,         // Device release num
    0x01,               // Manufacturer string index
    0x02,               // Product string index
    0x03,               // device serial number index
    0x01,               // number of possible configuration

    /* Configuration descriptor */
    0x09,               // this descriptor size
    0x02,               // descriptor type
    0x4B, 0x00,         // total length
    0x02,               // config number of interface
    0x01,               // config value
    0x00,               // config index
    0xc0,               // attribute
    0x32,               // max power unit=2mA

    /* Interface association descriptor. 8 bytes.  */
    0x08,               // this descriptor size
    0x0b,               // descriptor type
    0x00,               // the first iad interface
    0x02,               // number of iad interface
    0x02,               // Function Class
    0x02,               // Function SubClass
    0x00,               // Function Protocal
    0x00,               // Function

    /* Interface descriptor */
    0x09,               // this descriptor size
    0x04,               // descriptor type
    0x00,               // interface number
    0x00,               // alternative settings number
    0x01,               // endpoint number of this interface
    0x02,               // class code
    0x02,               // subclass code
    0x01,               // protocol
    0x00,               // interface index

    /* Header functional descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x00,               // descriptor subtype
    0x10,               // bcdCDC Low Byte
    0x01,               // bcdCDC High Byte

    /* Call Managment Functional Descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x01,               // descriptor subtype
    0x00,               // capabilities
    0x01,               // Data interface Number

    /* ACM Functional Descriptor */
    0x04,               // this descriptor size
    0x24,               // descriptor type
    0x02,               // descriptor subtype
    0x02,               // capabilities

    /* Union Functional Descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x06,               // descriptor subtype
    0x00,               // Master Interface
    0x01,               // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    0x07,               // this descriptor size
    0x05,               // type
    0x83,               // EP3 address
    0x03,               // endpoint attribute = Interrupt
    0x20, 0x00,         // max packet size
    0x10,               // interval

    /* Interface descriptor 1 */
    0x09,               // this descriptor size
    0x04,               // descriptor type
    0x01,               // interface number
    0x00,               // alternative settings number
    0x02,               // endpoint number of this interface
    0x0A,               // class code
    0x00,               // subclass code
    0x00,               // protocol
    0x00,               // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,               // this descriptor size
    0x05,               // type
    0x01,               // EP1 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,               // this descriptor size
    0x05,               // type
    0x82,               // EP2 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00                // interval
};

UINT8 __attribute__((aligned(32))) CdcAcmDescHs[] = {
    /* Device descriptor */
    0x12,               // this descriptor size
    0x01,               // device descriptor type
    0x00, 0x02,         // Spec version
    0x02,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x40,               // max packet size
    0x55, 0x42,         // VID
    0x52, 0x00,         // PID
    0x00, 0x00,         // Device release num
    0x01,               // Manufacturer string index
    0x02,               // Product string index
    0x03,               // device serial number index
    0x01,               // number of possible configuration

    /* Device qualifier descriptor */
    0x0a,               // this descriptor size
    0x06,               // device descriptor type
    0x00, 0x02,         // Spec version
    0x02,               // class code
    0x00,               // subclass code
    0x00,               // procotol code
    0x40,               // max packet size
    0x01,               // configuration number
    0x00,               // reserved

    /* Configuration descriptor */
    0x09,               // this descriptor size
    0x02,               // descriptor type
    0x4B, 0x00,         // total length
    0x02,               // config number of interface
    0x01,               // config value
    0x00,               // config index
    0xc0,               // attribute
    0x32,               // max power unit=2mA

    /* Interface association descriptor. 8 bytes.  */
    0x08,               // this descriptor size
    0x0b,               // descriptor type
    0x00,               // the first iad interface
    0x02,               // number of iad interface
    0x02,               // Function Class
    0x02,               // Function SubClass
    0x00,               // Function Protocal
    0x00,               // Function

    /* Interface descriptor */
    0x09,               // this descriptor size
    0x04,               // descriptor type
    0x00,               // interface number
    0x00,               // alternative settings number
    0x01,               // endpoint number of this interface
    0x02,               // class code
    0x02,               // subclass code
    0x01,               // protocol
    0x00,               // interface index

    /* Header functional descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x00,               // descriptor subtype
    0x10,               // bcdCDC Low Byte
    0x01,               // bcdCDC High Byte

    /* Call Managment Functional Descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x01,               // descriptor subtype
    0x00,               // capabilities
    0x01,               // Data interface Number

    /* ACM Functional Descriptor */
    0x04,               // this descriptor size
    0x24,               // descriptor type
    0x02,               // descriptor subtype
    0x02,               // capabilities

    /* Union Functional Descriptor */
    0x05,               // this descriptor size
    0x24,               // descriptor type
    0x06,               // descriptor subtype
    0x00,               // Master Interface
    0x01,               // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    0x07,               // this descriptor size
    0x05,               // type
    0x83,               // EP3 address
    0x03,               // endpoint attribute = Interrupt
    0x20, 0x00,         // max packet size
    0x10,               // interval

    /* Interface descriptor 1 */
    0x09,               // this descriptor size
    0x04,               // descriptor type
    0x01,               // interface number
    0x00,               // alternative settings number
    0x02,               // endpoint number of this interface
    0x0A,               // class code
    0x00,               // subclass code
    0x00,               // protocol
    0x00,               // interface index

    /* Endpoint descriptor (Bulk Out) */
    0x07,               // this descriptor size
    0x05,               // type
    0x01,               // EP1 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00,               // interval

    /* Endpoint descriptor (Bulk In) */
    0x07,               // this descriptor size
    0x05,               // type
    0x82,               // EP2 address
    0x02,               // endpoint attribute = BULK
    0x00, 0x02,         // max packet size
    0x00                // interval
};

UINT8 __attribute__((aligned(32))) CdcAcmStr[] = {
    /* Manufacturer string descriptor : Index 1 Amba */
    0x09, 0x04, 0x01, 0x04,
    0x41, 0x6d, 0x62, 0x61,

    /* Product string descriptor : Index 2 Amba cdcacm class*/
    0x09, 0x04, 0x02, 0x11,
    0x41, 0x6d, 0x62, 0x61, 0x20, 0x63, 0x64, 0x63,
    0x61, 0x63, 0x6d, 0x20, 0x63, 0x6c, 0x61, 0x73,
    0x73,

    /* Serial Number string descriptor : Index 3 0001 */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31
};

/*-----------------------------------------------------------------------------------------------*\
 * SIMPLE Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/

UINT8 __attribute__((aligned(32))) SimpleDescFs[] = {
    /* Device descriptor */
    0x12,           // this descriptor size
    0x01,           // device descriptor type
    0x10, 0x01,     // Spec version
    0,              // class code
    0,              // subclass code
    0,              // procotol code
    0x40,           // max packet size
    0x55, 0x42,     // VID
    0x01, 0x00,     // PID
    0, 0,           // Device release num
    0x01,           // Manufacturer string index
    0x02,           // Product string index
    0x03,           // device serial number index
    0x01,           // number of possible configuration

    /* Configuration descriptor */
    0x09,           // this descriptor size
    0x02,           // descriptor type
    0, 0,           // total length
    0x01,           // config number of interface
    0x01,           // config value
    0x00,           // config index
    0xc0,           // attribute
    0x32,           // max power unit=2mA

    /* interface0 alt0 */
    0x09,           // descriptor size
    0x04,           // descriptor type
    0,              // interface number
    0,              // alternate setting
    2,              // number of endpoint
    0xFF,           // interface class
    0xFF,           // interface sub-class
    0,              // interface protocol
    0,              // interface string id

    /* Endpoint descriptor (Bulk-In-0) */
    0x07,           // this descriptor size
    0x05,           // type
    0x82,           // EP address
    0x02,           // endpoint attribute = Bulk
    64, 0,          // max packet size
    0x0,            // polling interval

    /* Endpoint descriptor (Bulk-Out-0) */
    0x07,           // this descriptor size
    0x5,           // type
    0x01,           // EP address
    0x02,           // endpoint attribute = Bulk
    64, 0,          // max packet size
    0x0,            // polling interval
};

UINT8 __attribute__((aligned(32))) SimpleDescHs[] = {
    /* Device descriptor */
    0x12,           // this descriptor size
    0x01,           // device descriptor type
    0x00, 0x02,     // Spec version
    0,              // class code
    0,              // subclass code
    0,              // procotol code
    0x40,           // max packet size
    0x55, 0x42,     // VID
    0x01, 0x00,     // PID
    0, 0,           // Device release num
    0x01,           // Manufacturer string index
    0x02,           // Product string index
    0x03,           // device serial number index
    0x01,           // number of possible configuration

    /* Device qualifier descriptor */
    0x0A,           // this descriptor size
    0x06,           // device descriptor type
    0x00, 0x02,     // Spec version
    0,              // class code
    0,              // subclass code
    0,              // procotol code
    64,             // max packet size
    0x01,           // configuration number
    0x00,           // reserved

    /* Configuration descriptor */
    0x09,           // this descriptor size
    0x02,           // descriptor type
    0,0,            // total length
    0x01,           // number of interface
    0x01,           // config index
    0x00,           // string index
    0xC0,           // attribute
    0x32,           // max power unit=2mA

    /* interface0 alt0 */
    0x09,           // descriptor size
    0x04,           // descriptor type
    0,              // interface number
    0,              // alternate setting
    2,              // number of endpoint
    0xFF,           // interface class
    0xFF,           // interface sub-class
    0,              // interface protocol
    0,              // interface string id

    /* Endpoint descriptor (Bulk-In-0) */
    0x07,           // this descriptor size
    0x05,           // type
    0x82,           // EP address
    0x02,           // endpoint attribute = Bulk
    0x00, 0x02,     // max packet size
    0x0,                      // polling interval

    /* Endpoint descriptor (Bulk-Out-0) */
    0x07,           // this descriptor size
    0x05,           // type
    0x01,           // EP address
    0x02,           // endpoint attribute = Bulk
    0x00, 0x02,     // max packet size
    0x0,            // polling interval
};

UINT8 __attribute__((aligned(32))) SimpleStr[] = {
    /* Manufacturer string descriptor : Index 1 Amba */
    0x09, 0x04, // LANGID
    0x01, // Index
    0x04, // Length
    0x41, 0x6d, 0x62, 0x61,

    /* Product string descriptor : Index 2 Amba SIMPLE Class */
    0x09, 0x04, // LANGID
    0x02, // Index
    17,   // Length
    'A', 'm', 'b', 'a', ' ', 'S', 'I', 'M',
    'P', 'L', 'E', ' ', 'C', 'l', 'a', 's',
    's',

    /* Serial Number string descriptor : Index 3 0001 */
    0x09, 0x04,
    0x03,
    0x04,
    0x30, 0x30, 0x30, 0x31
};

CLASS_STACK_INIT_INFO_s UsbCustomInfo[APPLIB_USB_NUM_DEVICE_CLASS] = {
    [APPLIB_USB_DEVICE_CLASS_NONE] = {
        .DescFrameworkFs = NULL,
        .DescFrameworkHs = NULL,
        .StrFramework    = NULL,
        .LangIDFramework = NULL,
        .DescSizeFs      = 0,
        .DescSizeHs      = 0,
        .StrSize         = 0,
        .LangIDSize      = 0,
    },
    [APPLIB_USB_DEVICE_CLASS_MSC] = {
        .DescFrameworkFs = MscDescFs,
        .DescFrameworkHs = MscDescHs,
        .StrFramework    = AppLibUSBMscStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = sizeof(MscDescFs),
        .DescSizeHs      = sizeof(MscDescHs),
        .StrSize         = sizeof(AppLibUSBMscStr),
        .LangIDSize      = sizeof(LangID),
    },
    [APPLIB_USB_DEVICE_CLASS_MTP] = {
        .DescFrameworkFs = MtpDescFs,
        .DescFrameworkHs = MtpDescHs,
        .StrFramework    = MtpStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = sizeof(MtpDescFs),
        .DescSizeHs      = sizeof(MtpDescHs),
        .StrSize         = sizeof(MtpStr),
        .LangIDSize      = sizeof(LangID),
    },
#if 0
    [APPLIB_USB_DEVICE_CLASS_PICT] = {
        .DescFrameworkFs = PictBridgeDescFs,
        .DescFrameworkHs = PictBridgeDescHs,
        .StrFramework    = PictBridgeStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = sizeof(PictBridgeDescFs),
        .DescSizeHs      = sizeof(PictBridgeDescHs),
        .StrSize         = sizeof(PictBridgeStr),
        .LangIDSize      = sizeof(LangID),
    },
    [APPLIB_USB_DEVICE_CLASS_CDC_ACM] = {
        .DescFrameworkFs = CdcAcmDescFs,
        .DescFrameworkHs = CdcAcmDescHs,
        .StrFramework    = CdcAcmStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = sizeof(CdcAcmDescFs),
        .DescSizeHs      = sizeof(CdcAcmDescHs),
        .StrSize         = sizeof(CdcAcmStr),
        .LangIDSize      = sizeof(LangID),
    },
    [APPLIB_USB_DEVICE_CLASS_CUSTOM] = {
        .DescFrameworkFs = NULL,
        .DescFrameworkHs = NULL,
        .StrFramework    = NULL,
        .LangIDFramework = NULL,
        .DescSizeFs      = 0,
        .DescSizeHs      = 0,
        .StrSize         = 0,
        .LangIDSize      = 0,
    },
    [APPLIB_USB_DEVICE_CLASS_STREAM] = {
        .DescFrameworkFs = NULL,
        .DescFrameworkHs = NULL,
        .StrFramework    = NULL,
        .LangIDFramework = NULL,
        .DescSizeFs      = 0,
        .DescSizeHs      = 0,
        .StrSize         = 0,
    },
    [APPLIB_USB_DEVICE_CLASS_SIMPLE] = {
        .DescFrameworkFs = SimpleDescFs,
        .DescFrameworkHs = SimpleDescHs,
        .StrFramework    = SimpleStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = sizeof(SimpleDescFs),
        .DescSizeHs      = sizeof(SimpleDescHs),
        .StrSize         = sizeof(SimpleStr),
        .LangIDSize      = sizeof(LangID),
    },
#endif
};
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUSB_Custom_SetDevInfo
 *
 *  @Description:: Init USB custom device info
 *
 *  @Input      ::
 *          APPLIB_USB_CLASS_e:
 *
 *  @Output     :: none
 *
 *  @Return        ::
 *          UINT32 : UX_SUCCESS/UX_ERROR
\*-----------------------------------------------------------------------------------------------*/
UINT32 AppLibUSB_Custom_SetDevInfo(APPLIB_USB_DEVICE_CLASS_e class)
{
    return AmbaUSB_Descriptor_Init(&UsbCustomInfo[class]);
}
