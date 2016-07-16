
/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Device Video Class                                                  */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
#ifndef __ux_device_class_uvc_h__
#define __ux_device_class_uvc_h__

#include <StdUSB.h>

#define UX_SLAVE_CLASS_UVC_CLASS          0x0E

#define UVC_10_HEADER_EOH	0x80
#define UVC_10_HEADER_SCR	0x08
#define UVC_10_HEADER_PTS	0x04
#define UVC_10_HEADER_EOF	0x02
#define UVC_10_HEADER_FID	0x01
#define UVC_10_PACKET_SIZE	512

#define UVC_FRAME_TYPE_OFFSET_FS 168
#define UVC_FRAME_TYPE_OFFSET_HS 178
#define UVC_FRAME_LENGTH         0x32

#define UVC_FORMAT_INDEX_MJPEG		  0x01

#define UVC_FRAME_INDEX_320x240		  0x01
#define UVC_FRAME_INDEX_640x480		  0x02
#define UVC_FRAME_INDEX_1280x720	  0x03
#define UVC_FRAME_INDEX_1920x1080	  0x04

// -------------------------------------------------
// ------ Input Terminal ---------------------------
// -------------------------------------------------
#define UVC_IT_CONTROL_SIZE             0x03

#define UVC_AE_MODE_MANUAL              0x01
#define UVC_AE_MODE_AUTO                0x02
#define UVC_AE_MODE_SHUTTER_PRIORITY    0x04
#define UVC_AE_MODE_APERTURE_PRIORITY   0x08
#define UVC_AE_MODE_W_LENGTH            0x01

// -------------------------------------------------
// ------ Processing Uint --------------------------
// -------------------------------------------------
#define UVC_PU_CONTROL_SIZE      0x02

#define UVC_PU_POWERLINE_DISABLE 0x0
#define UVC_PU_POWERLINE_50HZ    0x01
#define UVC_PU_POWERLINE_60HZ    0x02

#define UDC_UVC_EVENT_CLEAR_STALL 0x01
#define UDC_UVC_EVENT_SET_FEATURE 0x02

/* Define Slave uvc Class structure.  */

typedef struct UX_SLAVE_CLASS_UVC_STRUCT
{
    UX_SLAVE_INTERFACE          *ux_slave_class_uvc_interface;
    TX_SEMAPHORE                class_thread_semaphore;
    VOID                        *event_thread_stack;
    TX_SEMAPHORE                event_thread_semaphore;
    TX_THREAD                   event_thread;
    UINT32                      event_id;
    UINT32                      event_endpoint_id;
} UX_SLAVE_CLASS_UVC;

typedef struct uvc_probe_control_s {
	UINT16	bmHint;
	UINT8	bFormatIndex;
	UINT8	bFrameIndex;
	UINT32	dwFrameInterval;
	UINT16	wKeyFrameRate;
	UINT16	wPFrameRate;
	UINT16 	wCompQuality;
	UINT16	wCompWindowSize;
	UINT16	wDelay;
	UINT32	dwMaxVideoFrameSize;
	UINT32	dwMaxPayloadTransferSize;
} uvc_probe_control_t;

typedef struct uvc_host_config_s {
	UINT32 width;
	UINT32 height;
    UINT32 mjpeg_quality;
    UINT32 framerate;
} uvc_host_config_t;

typedef struct uvc_pu_control_setting_s {
	int control_index;
	int maximum_value;
	int minimum_value;
	int default_value;
	int current_value;
} uvc_pu_control_setting_t;

typedef struct uvc_it_control_setting_s {
	int control_index;
	int maximum_value;
	int minimum_value;
	int default_value;
	int current_value;
} uvc_it_control_setting_t;

#define UVC_GET_SUPPORT		0x01
#define UVC_SET_SUPPORT		0x02

typedef struct usb_setup_reques_s {
    UINT8 bmRequestType;
    UINT8 bRequest;
    UINT16 wValue;
    UINT16 wIndex;
    UINT16 wLength;
} usb_setup_request_t;

typedef struct udc_uvc_event_callback_s {
    UINT32 (*encode_start)(void);
    UINT32 (*encode_stop)(void);
	UINT32 (*pu_set)(UINT32 index);
	UINT32 (*it_set)(UINT32 index);
} udc_uvc_event_callback_t;

VOID    _ux_device_class_uvc_thread(ULONG storage_instance);
UINT    _ux_device_class_uvc_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_reset(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_activate(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_probe_control(UX_SLAVE_TRANSFER *transfer_request, usb_setup_request_t *sreq);
UINT    _ux_device_class_uvc_commit_control(UX_SLAVE_TRANSFER *transfer_request, usb_setup_request_t *sreq);
UINT 	_ux_device_class_uvc_payload_send(UINT8 *buffer, UINT32 size, UINT32 timeout, UINT32 nocopy);
VOID    _udc_uvc_event_thread(ULONG uvc_class);

void  udc_uvc_config_probe_control(UCHAR *data_pointer, ULONG request_length);
uvc_probe_control_t *udc_uvc_get_probe_control(void);
void  udc_uvc_config_mjpeg_resolution(UINT8 format_idx, UINT8 frame_idx);
void  udc_uvc_copy_probe_control(uvc_probe_control_t *control, UINT8 *out);
void  udc_uvc_decode_probe_control(UINT8 *data, uvc_probe_control_t *control);
void  uvc_send_control_null_packet(UX_SLAVE_TRANSFER *transfer);
void  uvc_stall_controll_endpoint(void);
UINT  udc_uvc_set_context(UX_SLAVE_CLASS_UVC *ctx);
UX_SLAVE_CLASS_UVC *udc_uvc_get_context(void);
void  udc_uvc_notify_encode_start(void);
void  udc_uvc_notify_encode_stop(void);
void  udc_uvc_notify_pu_set(UINT32 index);
void  udc_uvc_notify_it_set(UINT32 index);

// utility functions exposed
UINT32 udc_uvc_get_mjpeg_quality(void);
UINT32 udc_uvc_set_mjpeg_quality(UINT32 quality);
UINT32 udc_uvc_register_callback(udc_uvc_event_callback_t *cbs);
UINT32 udc_uvc_get_host_config(uvc_host_config_t *config);

UINT udc_uvc_set_input_terminal_id(UINT32 id);
UINT udc_uvc_set_processing_unit_id(UINT32 id);
UINT udc_uvc_set_extension_unit_id(UINT32 id);

UINT32 udc_uvc_get_brightness_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_brightness_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_get_sharpness_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_sharpness_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_get_saturation_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_saturation_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_get_hue_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_hue_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_get_contrast_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_contrast_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_get_powerline_setting(uvc_pu_control_setting_t *s);
UINT32 udc_uvc_set_powerline_setting(uvc_pu_control_setting_t *s);

UINT32 AmbaUSB_System_GetConnectSpeed(void);

#endif

