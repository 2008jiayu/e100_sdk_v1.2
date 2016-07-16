/*-------------------------------------------------------------------------------------------------------------------*\
*  @FileName       :: AmbaUSB_Hosst_API.h
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

#ifndef AMBAUSB_HOST_API_H
#define AMBAUSB_HOST_API_H

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaCardManager.h"
#include "StdUSB.h"
#include "ux_api.h"
#include "ux_host_class_pima.h"
#include "ux_hcd_ehci.h"

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host IRQ Onwer
\*-----------------------------------------------------------------------------------------------*/

typedef enum _USB_HOST_IRQ_OWNER_e_ {
	USB_HOST_IRQ_RTOS = 0,
	USB_HOST_IRQ_LINUX
} USB_HOST_IRQ_OWNER_e;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB CLASS
\*-----------------------------------------------------------------------------------------------*/

typedef enum _UHC_CLASS_e_ {
	UHC_CLASS_NONE,
	UHC_CLASS_HID,      /* HID */
	UHC_CLASS_STORAGE,  /* Storage */
	UHC_CLASS_MTP,      /* Mtp*/
	UHC_CLASS_VIDEO,    /* Video */
	UHC_CLASS_ISO,      /* Isochronous Test */
	UHC_CLASS_MIX_MSC_UVC,	/* enable both msc and uvc class*/
} UHC_CLASS_e;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host system Init parameters
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_HOST_SYSTEM_INIT_s_ {
	UINT8   *MemPoolPtr;                  // memory pool pointer
	UINT32  TotalMemSize;                 // Total memory size
	UINT32  EnumThreadStackSize;          // stack size of ux_host_stack_enum_thread
	UINT32  EnumThreadPriority;           // thread priority ux_host_stack_enum_thread
	UINT32  HcdThreadStackSize;           // stack size of ux_host_stack_hcd_thread
	UINT32  HcdThreadPriority;            // thread priority ux_host_stack_hcd_thread
	UINT32  EnumThreadAffinityMask;       // Task Affinity of ux_host_stack_enum_thread
	UINT32  HcdThreadAffinityMask;        // Task Affinity of ux_host_stack_hcd_thread
} USB_HOST_SYSTEM_INIT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host Class Hook parameters
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_HOST_CLASS_INIT_s_ {
	UHC_CLASS_e classID;
	UINT32      ClassTaskStackSize;
	UINT32      ClassTaskPriority;
	UINT32      ClassTaskAffinityMask;
} USB_HOST_CLASS_INIT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB PORT OWNER
\*-----------------------------------------------------------------------------------------------*/

typedef enum _USB0_PORT_OWNER_e_ {
	UDC_OWN_PORT = (0x1 << 0),  // USB0 = Device
	UHC_OWN_PORT = (0x1 << 1),  // USB0 = Host
} USB0_PORT_OWNER_e;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host Storage Class Media Information
\*-----------------------------------------------------------------------------------------------*/

typedef struct _USB_HOST_STORAGE_INFO_s_ {
	UINT32  present;
	INT32   format;
	UINT32  lun;
	UINT32  SectorSize;
	UINT32  lba;
	UINT32  wp;
} USB_HOST_STORAGE_INFO_s;

typedef struct UHC_TASKINFO_s_ {
	UINT32 Priority;
	UINT32 AffinityMask;
	UINT32 StackSize;
} UHC_TASKINFO_s;

typedef struct _USB_HOST_STORAGE_CB_s_ {
    void        (*MediaInsert)(void);
    void        (*MediaRemove)(void);
} USB_HOST_STORAGE_CB_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host Storage Class Media Information
\*-----------------------------------------------------------------------------------------------*/
typedef struct _USB_HOST_MTP_CB_s_ {
    void        (*MediaInsert)(void);
    void        (*MediaRemove)(void);
} USB_HOST_MTP_CB_s;
/*-----------------------------------------------------------------------------------------------*\
 * Define USB Host Video Class structures
\*-----------------------------------------------------------------------------------------------*/
typedef struct UVCH_FRAME_INFO_S {
	ULONG			index;
	ULONG			width;
	ULONG			height;
	ULONG			min_bitrate;
	ULONG			max_bitrate;
	ULONG			max_buffer_size;
	ULONG			default_frame_interval;
	ULONG			interval_type;
	ULONG			*interval;
} UVCH_FRAME_INFO;

typedef struct UVCH_H264_FRAME_INFO_S {
	ULONG			index;
	ULONG			width;
	ULONG			height;
	ULONG			SAR_width;
	ULONG			SAR_height;
	ULONG			profile;
	ULONG			level_idc;
	ULONG			support_usages;
	ULONG			capabilities;
	ULONG			SVC_capabilities;
	ULONG			MVC_capabilities;
	ULONG			min_bitrate;
	ULONG			max_bitrate;
	ULONG			default_frame_interval;
	ULONG			num_frame_interval;
	ULONG			*interval;
} UVCH_H264_FRAME_INFO;

typedef struct UVCH_FORMAT_INFO_S {
	ULONG				subtype;
	ULONG				index;
	ULONG				frame_count;
	UVCH_FRAME_INFO *frame;
} UVCH_FORMAT_INFO;

typedef struct UVCH_CONTROL_ITEM_STRUCT {
	UINT8  selector;
	UINT8  is_supported;
	UINT8  size;
	UINT8  rsvd;
	UINT32 buffer_size;
	UINT8 *max;
	UINT8 *min;
	UINT8 *def;
	UINT8 *cur;
} UVCH_CONTROL_ITEM;

typedef struct UVCH_DEVICE_INFO_STRUCT {
	ULONG  size;
	ULONG  it_id;
	ULONG  xu_id;
	ULONG  pu_id;
	ULONG  format_count;
	UVCH_FORMAT_INFO *format;
	/* UVC 1.5 */
	ULONG	eu_id;
	ULONG	uvc_ver;
} UVCH_DEVICE_INFO;

typedef struct _USB_HOST_UVC_CB_s_ {
    void        (*MediaInsert)(void);
    void        (*MediaRemove)(void);
} USB_HOST_UVC_CB_s;

/*-----------------------------------------------------------------------------------------------*\
 * Define USB HCD Test Mode.
\*-----------------------------------------------------------------------------------------------*/

#define USBH_TEST_MODE_J_STATE                           1
#define USBH_TEST_MODE_K_STATE                           2
#define USBH_TEST_MODE_SE0_NAK                           3
#define USBH_TEST_MODE_PACKET                            4
#define USBH_TEST_MODE_FORCE_ENABLE                      5
#define USBH_TEST_MODE_SUSPEND_RESUME                    6
#define USBH_TEST_MODE_ONE_STEP_GET_DESCRIPTOR           7
#define USBH_TEST_MODE_ONE_STEP_SET_FEATURE              8

/*-----------------------------------------------------------------------------------------------*\
 * Define USB HCD Extern functions.
\*-----------------------------------------------------------------------------------------------*/

extern UINT32 AmbaUSB_Host_System_Init(USB_HOST_SYSTEM_INIT_s *config);
extern void   AmbaUSB_System_Host_SetUsbOwner(USB_HOST_IRQ_OWNER_e owner, int update);
extern UINT32 AmbaUSB_Host_System_SetMemoryPool(TX_BYTE_POOL *cached_pool, TX_BYTE_POOL *noncached_pool);
extern UINT32 AmbaUSB_Host_Class_Hook(USB_HOST_CLASS_INIT_s *config);
extern UINT32 AmbaUSB_Host_Class_UnHook(UHC_CLASS_e ClassID);
extern void   AmbaUSB_Host_Init_SwitchUsbOwner(USB0_PORT_OWNER_e owner);
extern void   AmbaUSB_Host_Init_EnableISR(void);
extern void   AmbaUSB_Host_Init_DisableISR(void);

// Mass Storage Class
extern int    AmbaUSB_Host_Class_Storage_FileCopy(char *pFilePathSource, char *pFilePathDestin);
extern void   AmbaUSB_Host_Class_Storage_Thrughput(int slot, UINT32* bs_multi, UINT32 align);
extern void   AmbaUSB_Host_Class_Storage_SetSlotInfo(UINT32 slot);
extern USB_HOST_STORAGE_INFO_s* AmbaUSB_Host_Class_Storage_GetStatus(int UsbMedia);
extern int    AmbaUSB_Host_Class_Storage_Read(int UsbMedia, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
extern int    AmbaUSB_Host_Class_Storage_Write(int UsbMedia, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
extern UINT32 AmbaUSB_Host_Class_Storage_RegisterCallback(USB_HOST_STORAGE_CB_s *cb);

// Ambarella Iso Class
extern UINT32 AmbaUSB_Host_Class_Iso_Read(UINT8 *data_pointer, UINT32 len, void (*func)(UINT32 rval));
extern UINT32 AmbaUSB_Host_Class_Iso_Write(UINT8 *data_pointer, UINT32 len, void (*func)(UINT32 rval));
extern UINT32 AmbaUSB_Host_Class_Iso_GetFrame(void);
extern UINT32 AmbaUSB_Host_System_SetEhciOCPolarity(UINT32 polarity);

// Ambarella Mtp Class
extern UINT32 AmbaUSBH_Mtp_RegisterCallback(USB_HOST_MTP_CB_s *cb);
extern UINT32 AmbaUSBH_Mtp_DeviceInfoGet(USBH_MTP_DEVICE *Device);
extern UINT32 AmbaUSBH_Mtp_DeviceReset(void);
extern UINT32 AmbaUSBH_Mtp_SessionOpen(USBH_MTP_SESSION *Session);
extern UINT32 AmbaUSBH_Mtp_SessionClose(USBH_MTP_SESSION *Session);
extern UINT32 AmbaUSBH_Mtp_StorageIdsGet(USBH_MTP_SESSION *Session, UINT32 *IdsArray, UINT32 ArrayLength);
extern UINT32 AmbaUSBH_Mtp_StorageInfoGet(USBH_MTP_SESSION *Session, UINT32 StorageId, USBH_MTP_STORAGE *Storage);
extern UINT32 AmbaUSBH_Mtp_ObjectNumberGet(USBH_MTP_SESSION *Session, UINT32 StorageId, UINT32 FormatCode, UINT32 Association);
extern UINT32 AmbaUSBH_Mtp_ObjectHandlesGet(USBH_MTP_SESSION *Session,
                                            UINT32 *ObjectHandlesArray,
                                            UINT32 ObjectHandlesLength,
                                            UINT32 StorageId,
                                            UINT32 FormatCode,
                                            UINT32 ObjectHandleAssociation);
extern UINT32 AmbaUSBH_Mtp_ObjectInfoGet(USBH_MTP_SESSION *Session, UINT32 ObjectHandle, USBH_MTP_OBJECT *Object);
extern UINT32 AmbaUSBH_Mtp_ObjectInfoSend(USBH_MTP_SESSION *Session, UINT32 StorageId, UINT32 ParentObjectId, USBH_MTP_OBJECT *Object);
extern UINT32 AmbaUSBH_Mtp_ObjectGet(USBH_MTP_SESSION *Session,
                                     UINT32 ObjectHandle,
                                     USBH_MTP_OBJECT *Object,
                                     UINT8 *ObjectBuf,
                                     UINT32 ObjectBufLen,
                                     UINT32 *ObjectActualLen);
extern UINT32 AmbaUSBH_Mtp_ObjectSend(USBH_MTP_SESSION *Session,
                                      USBH_MTP_OBJECT *Object,
                                      UINT8 *ObjectBuf,
                                      UINT32 ObjectBufLen);
extern UINT32 AmbaUSBH_Mtp_ObjectCopy(USBH_MTP_SESSION *Session,
                                      UINT32 ObjectHandle,
                                      UINT32 StorageId,
                                      UINT32 ParentObjectId);
extern UINT32 AmbaUSBH_Mtp_ObjectMove(USBH_MTP_SESSION *Session,
                                      UINT32 ObjectHandle,
                                      UINT32 StorageId,
                                      UINT32 ParentObjectId);
extern UINT32 AmbaUSBH_Mtp_ObjectDelete(USBH_MTP_SESSION *Session, UINT32 ObjectHandle, UINT32 FormatCode);
extern UINT32 AmbaUSBH_Mtp_ObjectTransferAbort(USBH_MTP_SESSION *Session,
                                               UINT32 ObjectHandle,
                                               USBH_MTP_OBJECT *Object);
extern UINT32 AmbaUSBH_Mtp_ThumbGet(USBH_MTP_SESSION *Session,
                                    UINT32 ObjectHandle,
                                    UINT32 FormatCode,
                                    USBH_MTP_OBJECT *Object,
                                    UINT8 *ObjectBuf,
                                    UINT32 ObjectBufLen,
                                    UINT32 *ObjectActualLen);

extern UINT32 AmbaUSBH_Mtp_VendorCommand(USBH_MTP_SESSION *Session, UINT32 *params);
extern UINT32 AmbaUSBH_Mtp_VendorCommandSend(USBH_MTP_SESSION *Ssession,
	                                         USBH_MTP_COMMAND * Command,
                                             USBH_MTP_VENDOR_PAYLOAD *VendorPayload,
                                             UINT8 *Buffer,
                                             UINT32 Length);
extern UINT32 AmbaUSBH_Mtp_VendorCommandGet(USBH_MTP_SESSION *Session,
	                                        USBH_MTP_COMMAND * Command,
		                                    USBH_MTP_VENDOR_PAYLOAD *VendorPayload,
		                                    UINT8 *Buffer,
		                                    UINT32 Length,
		                                    UINT32 *ActualLength);
extern VOID AmbaUSBH_Mtp_GetResponseCode(USBH_MTP_COMMAND* Response);
extern VOID AmbaUSBH_Mtp_SetResponseCode(USBH_MTP_COMMAND* Response);


// Video Class
extern void   AmbaUSBH_Uvc_SetCurValues(ULONG unit,ULONG selector,ULONG value);
extern void   AmbaUSBH_Uvc_StreamingStart(void);
extern void   AmbaUSBH_Uvc_StreamingStop(void);
extern void   AmbaUSBH_Uvc_PrintDeviceValues(void);
extern void   AmbaUSBH_Uvc_ProbeAndCommit(ULONG formatIndex,ULONG frameIndex,ULONG fps);
extern void   AmbaUSBH_Uvc_GetCurrentProbeSetting(UINT32 *formatIndex,UINT32 *frameIndex, UINT32 *fps);
extern UINT32 AmbaUSBH_Uvc_GetDeviceInfo(UVCH_DEVICE_INFO *info);
extern UINT32 AmbaUSBH_Uvc_GetPuControlInfo(UVCH_CONTROL_ITEM *item, UVC_PROCESSING_UNIT_CONTROL_SELECTOR selector);
extern UINT32 AmbaUSBH_Uvc_GetItControlInfo(UVCH_CONTROL_ITEM *item, UVC_CAMERA_TERMINAL_CONTROL_SELECTOR selector);
extern UINT   AmbaUSBH_Uvc_Read(UINT8 *buffer, UINT32 len, void (*complete_func)(UINT32 len));
extern UINT   AmbaUSBH_Uvc_ReadBlock(UX_EHCI_ISO_REQUEST *request, UINT32 PacketNum, void (*complete_func)(UX_EHCI_ISO_REQUEST *request));
extern UINT   AmbaUSBH_Uvc_ReadAppend(UX_EHCI_ISO_REQUEST *request, UINT32 PacketNum, void (*complete_func)(UX_EHCI_ISO_REQUEST *request));
extern UINT   AmbaUSBH_Uvc_GetMaxPacketSize(void);
extern UINT32 AmbaUSBH_Uvc_RegisterCallback(USB_HOST_UVC_CB_s *cb);

// redefine APIs. Applications should call APIs below in the future.
#define AmbaUSB_HostSystemSetup        AmbaUSB_Host_System_Init
#define AmbaUSBH_System_SetMemoryPool  AmbaUSB_Host_System_SetMemoryPool
#define AmbaUSBH_System_ClassHook      AmbaUSB_Host_Class_Hook
#define AmbaUSBH_System_ClassUnHook    AmbaUSB_Host_Class_UnHook
#define AmbaUSBH_System_SetPhy0Owner   AmbaUSB_Host_Init_SwitchUsbOwner
#define AmbaUSBH_System_SetUsbOwner    AmbaUSB_System_Host_SetUsbOwner
#define AmbaUSBH_System_EnableISR      AmbaUSB_Host_Init_EnableISR
#define AmbaUSBH_System_DisableISR     AmbaUSB_Host_Init_DisableISR
#define AmbaUSBH_System_SetEhciOCPolarity AmbaUSB_Host_System_SetEhciOCPolarity
#define AmbaUSBH_Storage_SetSlotInfo   AmbaUSB_Host_Class_Storage_SetSlotInfo
#define AmbaUSBH_Storage_GetStatus     AmbaUSB_Host_Class_Storage_GetStatus
#define AmbaUSBH_Storage_FileCopy      AmbaUSB_Host_Class_Storage_FileCopy
#define AmbaUSBH_Storage_Thrughput     AmbaUSB_Host_Class_Storage_Thrughput
#define AmbaUSBH_Storage_Read          AmbaUSB_Host_Class_Storage_Read
#define AmbaUSBH_Storage_Write         AmbaUSB_Host_Class_Storage_Write

// new API naming here
UINT32 AmbaUSBH_System_IsOhciEnabled(void);
void   AmbaUSBH_System_SetOhciEnabled(UINT32 value);
void   AmbaUSBH_System_SetIsoSlopDelay(UINT32 slop);
// MSC
UINT32 AmbaUSBH_Msc_SetMainTaskInfo(UHC_TASKINFO_s *info);
UINT32 AmbaUSBH_Msc_GetMainTaskInfo(UHC_TASKINFO_s *info);

// HID
UINT32 AmbaUSBH_Hid_SetMainTaskInfo(UHC_TASKINFO_s *info);
UINT32 AmbaUSBH_Hid_GetMainTaskInfo(UHC_TASKINFO_s *info);
void  AmbaUSBH_Sysem_SetTestMode(UINT32 TestMode);

#endif

