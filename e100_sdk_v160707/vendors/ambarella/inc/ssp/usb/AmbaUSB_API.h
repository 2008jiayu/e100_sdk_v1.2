/*-------------------------------------------------------------------------------------------------------------------*\
*  @FileName       :: AmbaUSB_API.h
*
*  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
*
*                     No part of this file may be reproduced, stored in a retrieval system,
*                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
*                     recording, or otherwise, without the prior consent of Ambarella Corporation.
*
*  @Description    :: AmbaUSB.lib external API.
*
*  @History        ::
*      Date        Name        Comments
*      03/13/2013  ycliao      Created
*
\*-----------------------------------------------------------------------------------------------*/

#ifndef AMBAUSB_API_H
#define AMBAUSB_API_H

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaCardManager.h"
#include "AmbaGPIO_Def.h"

#define MTP_MAX_FILENAME_SIZE                  128
#define MTP_UNICODE_MAX_LENGTH                 256
#define MTP_DATE_TIME_STRING_MAX_LENGTH        64

#define USB_CONNECT_SPEED_FULL     0
#define USB_CONNECT_SPEED_HIGH     1

/*-----------------------------------------------------------------------------------------------*\
 * Define USB IRQ Onwer
\*-----------------------------------------------------------------------------------------------*/

typedef enum _USB_IRQ_OWNER_e_ {
	USB_IRQ_RTOS = 0,
	USB_IRQ_LINUX
} USB_IRQ_OWNER_e;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB CLASS
\*-----------------------------------------------------------------------------------------------*/

typedef enum _UDC_CLASS_e_ {
	UDC_CLASS_NONE,
	UDC_CLASS_MSC,      /* mass storage class */
	UDC_CLASS_MTP,      /* ptp class */
	UDC_CLASS_PICT,
	UDC_CLASS_CDC_ACM,  // CDC-ACM class with single instances
	UDC_CLASS_CUSTOM,
	UDC_CLASS_STREAM,
	UDC_CLASS_HID,
	UDC_CLASS_SIMPLE,
	UDC_CLASS_UVC,
	UDC_CLASS_MIX_STG,
	UDC_CLASS_CDC_ACM_MULTI, // CDC-ACM class with multiple instances
	UDC_CLASS_NUM
} UDC_CLASS_e;

typedef struct UDC_TASKINFO_s_ {
	UINT32 Priority;
	UINT32 AffinityMask;
	UINT32 StackSize;
} UDC_TASKINFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB system Init parameters
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_SYSTEM_INIT_s_ {
	UINT8   *MemPoolPtr;
	UINT32  TotalMemSize;
} USB_SYSTEM_INIT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Class Hook parameters
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_CLASS_INIT_s_ {
	UDC_CLASS_e classID;
	UINT32      ClassTaskStackSize;
	UINT32      ClassTaskPriority;
	UINT32      ClassTaskAffinityMask;
} USB_CLASS_INIT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Vbus connect/disconnect callback function
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_DEV_VBUS_CB_s_ {
	void        (*VbusConnectCB)(void);
	void        (*VbusDisconnectCB)(void);
	void        (*SystemStart)(void);
	void        (*SystemRelease)(void);
	void        (*SystemConfigured)(UINT16 index);
	void        (*SystemSuspended)(void);
	void        (*SystemResumed)(void);
	void        (*SystemReset)(void);
} USB_DEV_VBUS_CB_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define Customer Descriptor
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_DEV_INFO_s_ {
	/* device descritpor */
	UINT16      idVendor;
	UINT16      idProduct;
	UINT16      bcdDevice;
	UINT16      LangID;
	UINT8       *StrFramework;
	UINT32      StrFrameWorkSize;
	/* configuration1 descritpor */
	UINT8       MaxPowerConfig1;
	UINT8       AttributesConfig1;
	/* ToDo : configuration2 descritpor */
	UINT8       MaxPowerConfig2;
	UINT8       AttributesConfig2;
} USB_DEV_INFO_s;

typedef struct _CLASS_STACK_INIT_INFO_s_ {
	UINT8                                   *DescFrameworkFs;
	UINT8                                   *DescFrameworkHs;
	UINT8                                   *StrFramework;
	UINT8                                   *LangIDFramework;
	UINT32                                  DescSizeFs;
	UINT32                                  DescSizeHs;
	UINT32                                  StrSize;
	UINT32                                  LangIDSize;
} CLASS_STACK_INIT_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define Interface Vendor request info structure.
   Support ONLY Interface target!! The Interface number in index field should match USB descriptor.
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_VENDOR_REQUEST_INFO_s_ {
	UDC_CLASS_e classID;
	INT     (*InterfaceVendorRequestFunc)(UINT32 RequestType, UINT32 request, UINT32 value, UINT32 index, UINT32 len);
} USB_VENDOR_REQUEST_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define MTP info Structure.
\*-----------------------------------------------------------------------------------------------*/

enum _USB_MTP_VENSOR_OP_e_ {
	MTP_VENDOR_OP_SEND = 0,
	MTP_VENDOR_OP_RECEIVE,
	MTP_VENDOR_OP_RESPONSE
};

typedef struct _USB_MTP_RESPONSE_s_ {
	UINT32 ResponseCode;
	UINT32 parmCnt;
	UINT32 parm[3];
} USB_MTP_RESPONSE_s;

typedef struct _USB_MTP_VENDOR_OP_s_ {
	UINT32              operation;
	UINT8               *BufferPtr;
	UINT32              length;
	UINT32              ActualLength;
	USB_MTP_RESPONSE_s  response;
} USB_MTP_VENDOR_OP_s;

typedef struct _USB_MTP_CMD_s_ {
	UINT32 OpCode;
	UINT32 TransactionID;
	UINT32 parmCnt;
	UINT32 parm[5];
} USB_MTP_CMD_s;

typedef struct _MTP_OBJECT_s_ {
	UINT32                   ObjectStorageId;
	UINT32                   ObjectFormat;
	UINT32                   ObjectProtectionStatus;
	UINT32                   ObjectCompressedSize;
	UINT32                   ObjectThumbFormat;
	UINT32                   ObjectThumbCompressedSize;
	UINT32                   ObjectThumbPixWidth;
	UINT32                   ObjectThumbPixHeight;
	UINT32                   ObjectImagePixWidth;
	UINT32                   ObjectImagePixHeight;
	UINT32                   ObjectImageBitDepth;
	UINT32                   ObjectParentObject;
	UINT32                   ObjectAssociationType;
	UINT32                   ObjectAssociationDesc;
	UINT32                   ObjectSequenceNumber;
	UINT8                    ObjectFilename[MTP_UNICODE_MAX_LENGTH];
	UINT8                    ObjectCaptureDate[MTP_DATE_TIME_STRING_MAX_LENGTH];
	UINT8                    ObjectModificationDate[MTP_DATE_TIME_STRING_MAX_LENGTH];
	UINT8                    ObjectKeywords[MTP_UNICODE_MAX_LENGTH];
	UINT32                   ObjectState;
	UINT32                   ObjectOffset;
	UINT32                   ObjectTransferStatus;
	UINT32                   ObjectHandleId;
	UINT32                   ObjectLength;
	UINT8                    *ObjectBuffer;
} MTP_OBJECT_s;

typedef struct _MTP_DEVICE_INFO_s_ {
	UINT32                      StorageId;
	UINT32                      StorageType;
	UINT32                      FileSystemType;
	UINT32                      AccessCapability;
	UINT32                      MaxCapacityLow;
	UINT32                      MaxCapacityHigh;
	UINT32                      FreeSpaceLow;
	UINT32                      FreeSpaceHigh;
	UINT32                      FreeSpaceImage;
	UINT8                       *DeviceInfoVendorName;
	UINT8                       *DeviceInfoProductName;
	UINT8                       *DeviceInfoSerialNo;
	UINT8                       *DeviceInfoVersion;
	UINT8                       *VolumeDescription;
	UINT8                       *VolumeLabel;
	UINT16                      *DeviceSupportProp;
	UINT16                      *DeviceSupportCaptureFormat;
	UINT16                      *DeviceSupportImgFormat;
	UINT16                      *ObjectSupportProp;
	UINT16                      *OperationSupportList;
	UINT32                      (*PropDescGet)(UINT32 DeviceProperty, UINT8 **DevicePropDataset, UINT32 *DevicePropDtasetLength);
	UINT32                      (*PropValueGet)(UINT32 DeviceProperty, UINT8 **DevicePorpValue, UINT32 *DevicePropValueLength);
	UINT32                      (*PropValueSet)(UINT32 DeviceProperty, UINT8 *DevicePropValue, UINT32 DevicePropValueLength);
	UINT32                      (*StorageFormat)(UINT32 StorageId);
	UINT32                      (*ObjectDelete)(UINT32 ObjectHandle);
	UINT32                      (*DeviceReset)(void);
	UINT32                      (*StorageInfoGet)(UINT32 StorageId, UINT32* MaxCapacityLow, UINT32* MaxCapacityHigh, UINT32* FreeSpaceLow, UINT32* FreeSpaceHigh);
	UINT32                      (*ObjectNumberGet)(UINT32 ObjectFormatCode, UINT32 ObjectAssociation, UINT32 *ObjectNumber);
	UINT32                      (*ObjectHandlesGet)(UINT32 ObjectHandlesFormatCode, UINT32 ObjectHandlesAssociation, UINT32 *ObjectHandlesArray, UINT32 ObjectHandlesMaxNumber, UINT32 *RequestNumber, USHORT *ResponseCode);
	UINT32                      (*ObjectInfoGet)(UINT32 ObjectHandle,  MTP_OBJECT_s **object);
	UINT32                      (*ObjectDataGet)(UINT32 ObjectHandle, UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength, USHORT OpCode);
	UINT32                      (*ObjectInfoSend)(MTP_OBJECT_s *object, UINT32 StorageId, UINT32 ParentObjectHandle, UINT32 *ObjectHandle);
	UINT32                      (*ObjectDataSend)(UINT32 ObjectHandle, UINT32 phase, UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength);
	UINT32                      (*ObjectPropDescGet)(UINT32 ObjectProperty, UINT32 ObjectFormatCode, UINT8 **ObjectPropDataset, UINT32 *ObjectPropDatasetLength);
	UINT32                      (*ObjectPropValueGet)(UINT32 ObjectHandle, UINT32 ObjectProperty, UINT8 **ObjectPropValue, UINT32 *ObjectPropValueLength);
	UINT32                      (*ObjectPropValueSet)(UINT32 ObjectHandle, UINT32 ObjectProperty, UINT8 *ObjectPropValue, UINT32 ObjectPropValueLength);
	UINT32                      (*ObjectReferenceGet)(UINT32 ObjectHandle, UINT8 **ObjectReferenceArray, UINT32 *ObjectReferenceArrayLength);
	UINT32                      (*ObjectReferenceSet)(UINT32 ObjectHandle, UINT8 *ObjectReferenceArray, UINT32 ObjectReferenceArrayLength);
	UINT32                      (*ObjectPropListGet)(UINT32 *params, UINT8 **PropList, UINT32 *PropListLength);
	UINT32                      (*ObjectCustomCommand)(UINT32 Parameter1, UINT32 Parameter2, UINT32 Parameter3, UINT32 Parameter4, UINT32 Parameter5, UINT32 *DataLength, UINT32 *direction);
	UINT32                      (*ObjectCustomDataGet)(UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength);
	UINT32                      (*ObjectCustomDataSend)(UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength);
	UINT32                      (*VendorCmdProcess)(USB_MTP_CMD_s *VendorCmdInfo);
	void                        (*ObjectClearAll)(void);
	UINT8                       RootPath[MTP_MAX_FILENAME_SIZE];
	VOID						(*TransferCanceled)(void);
	UINT32                      (*ObjectThumbGet)(UINT32 ObjectHandle, UINT8 *ObjectBuffer, UINT32 ObjectOffset,UINT32 ObjectLengthRequested, UINT32 *ObjectActualLength);
} MTP_DEVICE_INFO_s;

typedef struct _MTP_EVENT_s_ {
	UINT32  MTP_EVENT_CODE;
	UINT32  MTP_EVENT_SESSION_ID;       /*Do not need to assign a value*/
	UINT32  MTP_EVENT_TRANSACTION_ID;  /*Do not need to assign a value*/
	UINT32  MTP_EVENT_PARAMETER_1;
	UINT32  MTP_EVENT_PARAMETER_2;
	UINT32  MTP_EVENT_PARAMETER_3;
} MTP_EVENT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define PICTBRIDGE info Structure.
\*-----------------------------------------------------------------------------------------------*/

typedef struct _PICT_JOB_INFO_s_ {
	UINT32  JobinfoQuality;
	UINT32  JobinfoPapertype;
	UINT32  JobinfoPapersize;
	UINT32  JobinfoFiletype;
	UINT32  JobinfoDateprint;
	UINT32  JobinfoFilenameprint;
	UINT32  JobinfoImageoptimize;
	UINT32  JobinfoLayout;
	UINT32  JobinfoFixedsize;
	UINT32  JobinfoCropping;
	UINT32  ObjectFormat;
	UINT32  ObjectSize;
	UINT8   *FileName;
	UINT8   *date;
	UINT8   *buff;
} PICT_JOB_INFO_s;

typedef struct _PICT_JOB_OP_s_ {
	UINT32  (*ReadObjectData)(UINT8 *FileName, UINT8 *ObjectBuffer, UINT32 ObjectOffset, UINT32 ObjectLength, UINT32 *ActualLength);
	UINT32  (*JobEndReason)(UINT32 reason);
} PICT_JOB_OP_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define MASS STORAGE CLASS FS info Structure.
\*-----------------------------------------------------------------------------------------------*/

typedef struct _MSC_FSLIB_OP_s_ {
	INT32 (*read) (int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
	INT32 (*write) (int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
	INT32 (*GetMediaInfo) (int SlotID, AMBA_SCM_STATUS_s *pStatus);
} MSC_FSLIB_OP_s;

#define UDC_MSC_CBW_PROCESS_DONE         0 // Callback has processed the CBW
#define UDC_MSC_CBW_PROCESS_CONTINUE     1 // Callback has not processed the CBW

// MSC CSW Status
#define UDC_MSC_CSW_STS_GOOD        0x00 // Good Status
#define UDC_MSC_CSW_STS_CMD_FAILED  0x01 // Command Failed
#define UDC_MSC_CSW_STS_PHASE_ERR   0x02 // Phase Error
#define UDC_MSC_CSW_STS_SENDED      0xff // status data has sended
#define UDC_MSC_CSW_STS_USB_ERR     0xfe // usb send/receive error

typedef UINT32 (*UDC_MSC_CBK_PROCESSCBW)(UINT8 *cbwcb, UINT32 cbwcb_length, UINT8 *status);
UINT32 AmbaUSB_Class_Msc_SetCbwCallback(UDC_MSC_CBK_PROCESSCBW pfnCallBackFunc );
UINT32 AmbaUSB_Class_Msc_SendBulkInData(UINT8 *data, UINT32 length);
UINT32 AmbaUSB_Class_Msc_RecvBulkOutData(UINT8 *data, UINT32 length, UINT32 *bytes_read);

/*-----------------------------------------------------------------------------------------------*\
 * Define UVC Structures.
\*-----------------------------------------------------------------------------------------------*/
typedef struct USBD_UVC_PU_CONTROL_SETTING_s_ {
	int ControlIndex;
	int MaximumValue;
	int MinimumValue;
	int DefaultValue;
	int CurrentValue;
} USBD_UVC_PU_CONTROL_SETTING_s;

typedef struct USBD_UVC_IT_CONTROL_SETTING_s_ {
	int ControlIndex;
	int MaximumValue;
	int MinimumValue;
	int DefaultValue;
	int CurrentValue;
} USBD_UVC_IT_CONTROL_SETTING_s;

typedef struct USBD_UVC_EVENT_CALLBACK_s_ {
	UINT32 (*EncodeStart)(void);
	UINT32 (*EncodeStop)(void);
	UINT32 (*PuSet)(UINT32 index);
	UINT32 (*ItSet)(UINT32 index);
} USBD_UVC_EVENT_CALLBACK_s;

typedef struct USBD_UVC_HOST_CONFIG_s_ {
	UINT32 Width;
	UINT32 Height;
	UINT32 MjpegQuality;
	UINT32 Framerate;
} USBD_UVC_HOST_CONFIG_s;

UINT32 AmbaUSBD_UVC_RegisterCallback(USBD_UVC_EVENT_CALLBACK_s *cbs);
UINT32 AmbaUSBD_UVC_GetHostConfig(USBD_UVC_HOST_CONFIG_s *config);
UINT32 AmbaUSBD_UVC_BulkSend(UINT8 *Buffer, UINT32 Size, UINT32 Timeout);
UINT32 AmbaUSBD_UVC_BulkSendEx(UINT8 *Buffer, UINT32 Size, UINT32 Timeout, UINT32 NoCopy);
UINT32 AmbaUSBD_UVC_IsoSend(UINT8 *Buffer, UINT32 Size, UINT32 Timeout, UINT32 NocCpy);
UINT32 AmbaUSBD_UVC_SetInputTerminalAttribute(UINT32 attribute);
UINT32 AmbaUSBD_UVC_SetProcessingUintAttribute(UINT32 attribute);
UINT32 AmbaUSBD_UVC_GetInputTerminalAttribute(void);
UINT32 AmbaUSBD_UVC_GetProcessingUintAttribute(void);
UINT32 AmbaUSBD_UVC_GetBrightnessSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetBrightnessSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_GetSaturationSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetSaturationSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_GetContrastSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetContrastSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_GetHueSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetHueSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_GetSharpnessSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetSharpnessSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_GetPowerLineSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);
UINT32 AmbaUSBD_UVC_SetPowerLineSetting(USBD_UVC_PU_CONTROL_SETTING_s *s);

/*-----------------------------------------------------------------------------------------------*\
 * Define HID CLASS parameter Structure.
\*-----------------------------------------------------------------------------------------------*/

#define USBD_HID_EVENT_BUFFER_LENGTH 32

typedef struct USBD_HID_EVENT_s_ {
	UINT32    Id;
	UINT32    Type;
	UINT8     Buffer[USBD_HID_EVENT_BUFFER_LENGTH];
	UINT32    Length;
} USBD_HID_EVENT_s;

typedef UINT32 (*UsbdHidReportCallback)(USBD_HID_EVENT_s *);

typedef struct USBD_HID_PARAMETER_s_ {
	UINT8    *ReportAddress;
	UINT32    ReportLength;
	UsbdHidReportCallback ReportCallback; // get called when set_report is called from HOST.
} USBD_HID_PARAMETER_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB MESSAGE Structure
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_HOST_QUEUE_DATA_s_ {

	UINT32  data0;
	UINT32  data1;
	UINT32  data2;
	UINT32  data3;
    UINT32  data4;
    UINT32  data5;
    UINT32  data6;
    UINT32  data7;
} USB_HOST_QUEUE_DATA_s;

typedef struct _USB_HOST_MESSAGEQ_s_ {
	AMBA_KAL_MSG_QUEUE_t        QID;
	USB_HOST_QUEUE_DATA_s       Queue[5];
} USB_HOST_MESSAGEQ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB MESSAGE BASE OFFSET
\*-----------------------------------------------------------------------------------------------*/

#define USB_MSG_SYSTEM_DEVICE_BASE  0x01
#define USB_MSG_SYSTEM_HOST_BASE    0x02
#define USB_MSG_CLASS_DEVICE_BASE   0x03
#define USB_MSG_CLASS_HOST_BASE     0x04
#define USB_MSG_TEST_BASE            0x05

#define USB_MSG_SYSTEM_DEVICE(x)       ((USB_MSG_SYSTEM_DEVICE_BASE<<24) + x)
#define USB_MSG_SYSTEM_HOST(x)          ((USB_MSG_SYSTEM_HOST_BASE<<24) + x)
#define USB_MSG_CLASS_DEVICE(x)         ((USB_MSG_CLASS_DEVICE_BASE<<24) + x)
#define USB_MSG_CLASS_HOST(x)           ((USB_MSG_CLASS_HOST_BASE<<24) + x)
#define USB_MSG_TEST(x)                  ((USB_MSG_TEST_BASE<<24) + x)

/*-----------------------------------------------------------------------------------------------*\
 * Define USB MESSAGE SYSTEM_DEVICE, SYSTEM_HOST, CLASS_DEVICE, CLASS_HOST
\*-----------------------------------------------------------------------------------------------*/

#define USB_MSG_OVERCURRENT        USB_MSG_SYSTEM_HOST(0x0001)

/*-----------------------------------------------------------------------------------------------*\
 * Define USB A9 DCD Extern functions.
\*-----------------------------------------------------------------------------------------------*/

// device system APIs
extern UINT32 AmbaUSB_System_SetMemoryPool(TX_BYTE_POOL *cached_pool, TX_BYTE_POOL *noncached_pool);
extern UINT32 AmbaUSB_System_DeviceSystemSetup(USB_SYSTEM_INIT_s* config);
extern UINT32 AmbaUSB_System_HostSystemSetup(USB_SYSTEM_INIT_s* config);
extern UINT32 AmbaUSB_System_ClassHook(USB_CLASS_INIT_s* config);
extern UINT32 AmbaUSB_System_ClassUnHook(USB_CLASS_INIT_s* config);
extern UINT32 AmbaUSB_System_RegisterVbusCallback(USB_DEV_VBUS_CB_s *VbusCb);
extern UINT32 AmbaUSB_System_SetVbusPin(AMBA_GPIO_PIN_ID_e pin);
extern AMBA_GPIO_PIN_ID_e AmbaUSB_System_GetVbusPin(void);
extern void   AmbaUSB_System_SetConnect(UINT32 enable);
extern void   AmbaUSBD_System_SetSoftwareConnect(UINT32 enable, UINT32 connect);
extern UINT32 AmbaUSB_System_IsConfigured(void);
extern UINT32 AmbaUSB_System_GetVbusStatus(void);
extern UINT32 AmbaUSB_System_GetConnectSpeed(void);
extern UINT32 AmbaUSB_System_ChargeDetection(void);
extern UINT32 AmbaUSB_System_DataContactDetection(UINT32 t1, UINT32 t2);
extern UINT32 AmbaUSB_System_RegisterQueueMsg(USB_HOST_MESSAGEQ_s *msg);
extern void   AmbaUSB_System_Config_Vbus_Pin(void);
extern UINT32 AmbaUSB_System_Vbus_Pin_Status(void);
extern VOID  *AmbaUSB_System_GetDriverContext(void);
extern UINT32 AmbaUSB_System_GetDeviceDataConnWithVbus(void);
extern UINT32 AmbaUSB_System_SetDeviceDataConnWithVbus(UINT32 value);
extern UINT32 AmbaUSB_System_SetDeviceDataConn(UINT32 value);
extern char  *AmbaUSB_System_GetVcsVersion(void);
extern void   AmbaUSB_System_SetUsbOwner(USB_IRQ_OWNER_e owner, int update);
extern UINT32 AmbaUSB_System_RegisterVendorRequest(USB_VENDOR_REQUEST_INFO_s *info);
extern UINT32 AmbaUSB_System_ControlRead(UINT8 *buff, UINT32 size);
extern UINT32 AmbaUSB_System_ControlWrite(UINT8 *buff, UINT32 size);
extern char  *AmbaUSB_System_GetUxErrorString(UINT32 ux_code);
extern UINT32 AmbaUSB_System_SetDeviceConfigDetectTimeout(UINT32 value);
extern VOID   AmbaUSB_System_SetBulkInTimeout(UINT32 value);

// device driver APIs
extern void   AmbaUSB_Drv_DumpSlaveRegisters(void);

// descriptor APIs
extern UINT32 AmbaUSB_Descriptor_SetCustomize(UDC_CLASS_e UdcClass, USB_DEV_INFO_s *desc);
extern UINT32 AmbaUSB_Descriptor_Init(CLASS_STACK_INIT_INFO_s *info);

// Mass Storage Class
extern UINT32 AmbaUSB_Class_Msc_Init(MSC_FSLIB_OP_s *op);
extern UINT32 AmbaUSB_Class_Msc_SetInfo(UINT8 *vendor, UINT8 *product, UINT8 *revision);
extern UINT32 AmbaUSB_Class_Msc_SetProp(UINT8 drive, UINT8 removal, UINT8 wp, UINT8 type);
extern UINT32 AmbaUSB_Class_Msc_Mount(UINT8 drive);
extern UINT32 AmbaUSB_Class_Msc_UnMount(UINT8 drive);

// MTP Class
extern UINT32 AmbaUSB_Class_Mtp_AddEvent(MTP_EVENT_s* event);
extern UINT32 AmbaUSB_Class_Mtp_SetInfo(MTP_DEVICE_INFO_s *DeviceInfo);
extern UINT32 AmbaUSB_Class_Mtp_VendorOperation(USB_MTP_VENDOR_OP_s *VendorOperation);

// Pictbridge Class
extern UINT32 AmbaUSB_Class_Pictbridge_SetInfo(UINT8 *vendor, UINT8 *product, UINT8 *sn);
extern UINT32 AmbaUSB_Class_Pictbridge_RegisterCb(PICT_JOB_OP_s *cb);
extern UINT32 AmbaUSB_Class_Pictbridge_CheckCapability(UINT32 capability);
extern UINT32 AmbaUSB_Class_Pictbridge_ConfigJob(PICT_JOB_INFO_s *config);
extern UINT32 AmbaUSB_Class_Pictbridge_IsPrinterServiceReady(void);
extern UINT32 AmbaUSB_Class_Pictbridge_AbortJob(void);
extern UINT32 AmbaUSB_Class_Pictbridge_ContinueJob(void);

// CDC-ACM Class
extern UINT32 AmbaUSB_Class_CDC_ACM_TerminalOpen(void);
extern UINT32 AmbaUSB_Class_CDC_ACM_Write(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength);
extern UINT32 AmbaUSB_Class_CDC_ACM_Read(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength);

// CDC-ACM multiple instances Class
extern UINT32 AmbaUSB_Class_CDC_ACM_Multi_TerminalOpen(UINT32 instance_id);
extern UINT32 AmbaUSB_Class_CDC_ACM_Multi_Write(UINT32 instance_id, UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength);
extern UINT32 AmbaUSB_Class_CDC_ACM_Multi_Read(UINT32 instance_id, UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength);

// HID class
extern UINT32 AmbaUSBD_Hid_Init(void);
extern UINT32 AmbaUSBD_Hid_SetParameter(USBD_HID_PARAMETER_s *parameter);
extern UINT32 AmbaUSBD_Hid_SendReport(USBD_HID_EVENT_s *event);

// Ambarella Simple class
extern UINT32 AmbaUSB_Class_Simple_Init(void);
extern UINT32 AmbaUSB_Class_Simple_Write(UINT8 *Buffer, UINT32 RequestLength, UINT32 *ActualLength, UINT32 EndpointAddress, UINT32 Timeout);
extern UINT32 AmbaUSB_Class_Simple_Read(UINT8 *Buffer, UINT32 RequestLength, UINT32 *ActualLength, UINT32 EndpointAddress, UINT32 Timeout);

// Ambarella Stream class
extern UINT32 AmbaUSB_Class_Stream_Write(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength, ULONG timeout);
extern UINT32 AmbaUSB_Class_Stream_Read(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength, ULONG timeout);
extern UINT32 AmbaUSB_Class_Stream_Write_Native(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength, ULONG timeout);
extern UINT32 AmbaUSB_Class_Stream_Read_Native(UINT8 *buffer, UINT32 RequestLength, UINT32 *ActualLength, ULONG timeout);

// debug APIs from USBX class
extern UINT32 AmbaUSB_Class_Mtp_EnableDebug(void);
extern UINT32 AmbaUSB_Class_Mtp_DisableDebug(void);
extern UINT32 AmbaUSB_Class_Mtp_IsDebugEnabled(void);

// redefine APIs. Applications should call APIs below in the future.
#define AmbaUSB_DeviceSystemSetup            AmbaUSB_System_DeviceSystemSetup
#define AmbaUSB_System_Init                  AmbaUSB_System_DeviceSystemSetup
#define AmbaUSBD_System_SetMemoryPool        AmbaUSB_System_SetMemoryPool
#define AmbaUSBD_System_ClassHook            AmbaUSB_System_ClassHook
#define AmbaUSBD_System_ClassUnHook          AmbaUSB_System_ClassUnHook
#define AmbaUSBD_System_SetConnect           AmbaUSB_System_SetConnect
#define AmbaUSBD_System_RegisterVbusCallback AmbaUSB_System_RegisterVbusCallback
#define AmbaUSBD_System_DumpRegisters        AmbaUSB_Drv_DumpSlaveRegisters
#define AmbaUSBD_System_GetVbusStatus        AmbaUSB_System_GetVbusStatus
#define AmbaUSBD_System_IsConfigured         AmbaUSB_System_IsConfigured
#define AmbaUSBD_System_GetConnectSpeed      AmbaUSB_System_GetConnectSpeed
#define AmbaUSBD_System_DataContactDetection AmbaUSB_System_DataContactDetection
#define AmbaUSBD_System_ChargeDetection      AmbaUSB_System_ChargeDetection
#define AmbaUSBD_System_GetDataConnWithVbus  AmbaUSB_System_GetDeviceDataConnWithVbus
#define AmbaUSBD_System_SetDataConnWithVbus  AmbaUSB_System_SetDeviceDataConnWithVbus
#define AmbaUSBD_System_SetDataConn          AmbaUSB_System_SetDeviceDataConn
#define AmbaUSBD_System_SetUsbOwner          AmbaUSB_System_SetUsbOwner
#define	AmbaUSBD_System_RegisterVendorRequest AmbaUSB_System_RegisterVendorRequest
#define AmbaUSBD_System_ControlRead          AmbaUSB_System_ControlRead
#define AmbaUSBD_System_ControlWrite         AmbaUSB_System_ControlWrite
#define AmbaUSBD_System_GetVbusPinStatus     AmbaUSB_System_Vbus_Pin_Status
#define AmbaUSBD_System_GetVbusPin           AmbaUSB_System_GetVbusPin
#define AmbaUSBD_System_SetBulkInTimeout     AmbaUSB_System_SetBulkInTimeout
#define AmbaUSBD_Descriptor_Init             AmbaUSB_Descriptor_Init
#define AmbaUSBD_Descriptor_SetCusomize      AmbaUSB_Descriptor_SetCustomize
#define AmbaUSBD_Msc_Init                    AmbaUSB_Class_Msc_Init
#define AmbaUSBD_Msc_SetInfo                 AmbaUSB_Class_Msc_SetInfo
#define AmbaUSBD_Msc_SetProp                 AmbaUSB_Class_Msc_SetProp
#define AmbaUSBD_Msc_Mount                   AmbaUSB_Class_Msc_Mount
#define AmbaUSBD_Msc_UnMount                 AmbaUSB_Class_Msc_UnMount
#define AmbaUSBD_Msc_SetCbwCallback          AmbaUSB_Class_Msc_SetCbwCallback
#define AmbaUSBD_Msc_SendBulkInData          AmbaUSB_Class_Msc_SendBulkInData
#define AmbaUSBD_Msc_RecvBulkOutData         AmbaUSB_Class_Msc_RecvBulkOutData
#define AmbaUSBD_Mtp_AddEvent                AmbaUSB_Class_Mtp_AddEvent
#define AmbaUSBD_Mtp_SetInfo                 AmbaUSB_Class_Mtp_SetInfo
#define AmbaUSBD_Mtp_VendorOperation         AmbaUSB_Class_Mtp_VendorOperation
#define AmbaUSBD_Mtp_EnableDebug             AmbaUSB_Class_Mtp_EnableDebug
#define AmbaUSBD_Mtp_DisableDebug            AmbaUSB_Class_Mtp_DisableDebug
#define AmbaUSBD_Mtp_IsDebugEnabled          AmbaUSB_Class_Mtp_IsDebugEnabled
#define AmbaUSBD_Pictbridge_SetInfo          AmbaUSB_Class_Pictbridge_SetInfo
#define AmbaUSBD_Pictbridge_RegisterCb       AmbaUSB_Class_Pictbridge_RegisterCb
#define AmbaUSBD_Pictbridge_CheckCapability  AmbaUSB_Class_Pictbridge_CheckCapability
#define AmbaUSBD_Pictbridge_ConfigJob        AmbaUSB_Class_Pictbridge_ConfigJob
#define AmbaUSBD_Pictbridge_IsPrinterServiceReady AmbaUSB_Class_Pictbridge_IsPrinterServiceReady
#define AmbaUSBD_Pictbridge_AbortJob         AmbaUSB_Class_Pictbridge_AbortJob
#define AmbaUSBD_Pictbridge_ContinueJob      AmbaUSB_Class_Pictbridge_ContinueJob
#define AmbaUSBD_CDC_ACM_TerminalOpen        AmbaUSB_Class_CDC_ACM_TerminalOpen
#define AmbaUSBD_CDC_ACM_Write               AmbaUSB_Class_CDC_ACM_Write
#define AmbaUSBD_CDC_ACM_Read                AmbaUSB_Class_CDC_ACM_Read
#define AmbaUSBD_CDC_ACM_Multi_TerminalOpen  AmbaUSB_Class_CDC_ACM_Multi_TerminalOpen
#define AmbaUSBD_CDC_ACM_Multi_Write         AmbaUSB_Class_CDC_ACM_Multi_Write
#define AmbaUSBD_CDC_ACM_Multi_Read          AmbaUSB_Class_CDC_ACM_Multi_Read
#define AmbaUSBD_Simple_Init                 AmbaUSB_Class_Simple_Init
#define AmbaUSBD_Simple_Write                AmbaUSB_Class_Simple_Write
#define AmbaUSBD_Simple_Read                 AmbaUSB_Class_Simple_Read
#define AmbaUSBD_Stream_Write                AmbaUSB_Class_Stream_Write
#define AmbaUSBD_Stream_Read                 AmbaUSB_Class_Stream_Read
#define AmbaUSBD_Stream_NativeWrite          AmbaUSB_Class_Stream_Write_Native
#define AmbaUSBD_Stream_NativeRead           AmbaUSB_Class_Stream_Read_Native

// new API naming here

// System - ISR task
UINT32 AmbaUSBD_System_SetIsrTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_System_GetIsrTaskInfo(UDC_TASKINFO_s *info);

// System - VBUS Timer task
UINT32 AmbaUSBD_System_SetVbusTimerTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_System_GetVbusTimerTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_System_SuspendVbusDetection(void);
UINT32 AmbaUSBD_System_ResumeVbusDetection(void);

// MSC
UINT32 AmbaUSBD_Msc_SetReadCacheTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_Msc_GetReadCacheTaskInfo(UDC_TASKINFO_s *info);
void   AmbaUSBD_Msc_EnableDebug(void);
void   AmbaUSBD_Msc_DisableDebug(void);

// MTP
UINT32 AmbaUSBD_Mtp_SetReadCacheTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_Mtp_GetReadCacheTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_Mtp_SetSupportedEvents(UINT16 *list);

// Pictbridge
UINT32 AmbaUSBD_Pictbridge_SetMainTaskInfo(UDC_TASKINFO_s *info);
UINT32 AmbaUSBD_Pictbridge_GetMainTaskInfo(UDC_TASKINFO_s *info);

#endif
