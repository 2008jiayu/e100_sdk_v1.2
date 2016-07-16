#ifndef UX_HOST_CLASS_VIDEO_H
#define UX_HOST_CLASS_VIDEO_H

#include <StdUSB.h>
#include <AmbaUSB_Host_API.h>

#ifndef UX_HOST_CLASS_VIDEO_THREAD_STACK_SIZE
#define UX_HOST_CLASS_VIDEO_THREAD_STACK_SIZE             UX_THREAD_STACK_SIZE
#endif

#ifndef UX_THREAD_PRIORITY_VIDEO
#define UX_THREAD_PRIORITY_VIDEO                         70
#endif

typedef struct _USB_HOST_VIDEO_INFO_s_ {
	ULONG   unit;
	ULONG   selector;
	ULONG   value;
} USB_HOST_VIDEO_INFO_s;

typedef struct _USB_HOST_VIDEO_PROBE_INFO_s_ {
	ULONG   formatIndex;
	ULONG   frameIndex;
	ULONG   fps;
} USB_HOST_VIDEO_PROBE_INFO_s;

enum {
	HOST_VIDEO_SET_CUR              = 0x00000001,
	HOST_VIDEO_STREAMING_START      = 0x00000002,
	HOST_VIDEO_STREAMING_STOP       = 0x00000004,
	HOST_VIDEO_PRINT_DEVICE_VALUES  = 0x00000010,
	HOST_VIDEO_PROBING_AND_COMMIT   = 0x00000020,
	HOST_VIDEO_GET_DEVICE_INFO	    = 0x00000040,
	HOST_VIDEO_REQUEST_DONE	        = 0x80000000,
	HOST_VIDEO_ALL                  = 0x00000077,
};

/* Define Video Class main constants.  */

#define UVCH_THREAD_PRIORITY_CLASS           70
#define UX_HOST_CLASS_VIDEO_CLASS                           0xE
#define UX_HOST_CLASS_VIDEO_SUBCLASS_CONTROL                1
#define UX_HOST_CLASS_VIDEO_SUBCLASS_STREAMING              2

/* Define Video Class interface descriptor subclasses.  */
#define UX_HOST_CLASS_VIDEO_SC_UNDEFINED                    0x00
#define UX_HOST_CLASS_VIDEO_SC_CONTROL                      0x01
#define UX_HOST_CLASS_VIDEO_SC_STREAMING                    0x02
#define UX_HOST_CLASS_VIDEO_SC_INTERFACE_COLLECTION         0x03

#if 0
/* Define Video Class specific request codes.  */
#define UX_HOST_CLASS_VIDEO_REQUEST_CODE_UNDEFINED          0x00
#define UX_HOST_CLASS_VIDEO_SET_CUR                         0x01
#define UX_HOST_CLASS_VIDEO_GET_CUR                         0x81
#define UX_HOST_CLASS_VIDEO_GET_MIN                         0x82
#define UX_HOST_CLASS_VIDEO_GET_MAX                         0x83
#define UX_HOST_CLASS_VIDEO_GET_RES                         0x84
#define UX_HOST_CLASS_VIDEO_GET_LEN                         0x85
#define UX_HOST_CLASS_VIDEO_GET_INFO                        0x86
#define UX_HOST_CLASS_VIDEO_GET_DEF                         0x87
#endif

/* Define Video Class external terminal types.  */
#define UX_HOST_CLASS_VIDEO_EXTERNAL_VENDOR_SPECIFIC        0x0400
#define UX_HOST_CLASS_VIDEO_EXTERNAL_CONNECTOR              0X0401
#define UX_HOST_CLASS_VIDEO_EXTERNAL_SVIDEO_CONNECTOR       0X0402
#define UX_HOST_CLASS_VIDEO_EXTERNAL_COMPONENT_CONNECTOR    0X0403

#define UVCH_INTERFACE_DESCRIPTOR_ENTRIES            8
#define UVCH_INTERFACE_DESCRIPTOR_LENGTH             8

#define UVCH_OT_DESC_ENTRIES      8
#define UVCH_OT_DESC_LENGTH       9

#define UVCH_FEATURE_UNIT_DESCRIPTOR_ENTRIES         7
#define UVCH_FEATURE_UNIT_DESCRIPTOR_LENGTH          7

#define UVCH_STREAMING_INTERFACE_DESCRIPTOR_ENTRIES  6
#define UVCH_STREAMING_INTERFACE_DESCRIPTOR_LENGTH   6

#define UVCH_STREAMING_ENDPOINT_DESCRIPTOR_ENTRIES   6
#define UVCH_STREAMING_ENDPOINT_DESCRIPTOR_LENGTH    6

#define UVCH_PU_DESC_ENTRIES       7
#define UVCH_PU_DESC_LENGTH        8

#define UVCH_EU_DESC_ENTRIES       7
#define UVCH_EU_DESC_LENGTH        7

#define UVCH_IT_DESC_ENTRIES       11
#define UVCH_IT_DESC_LENGTH        15

#define UVCH_XU_DESC_ENTRIES       10
#define UVCH_XU_DESC_LENGTH        22

#define UVCH_VS_INPUT_HEADER_DESCRIPTOR_ENTRIES      12
#define UVCH_VS_INPUT_HEADER_DESCRIPTOR_LENGTH       13

#define UVCH_MJPEG_FORMAT_DESCRIPTOR_ENTRIES         11
#define UVCH_MJPEG_FORMAT_DESCRIPTOR_LENGTH          11

#define UVCH_H264_FORMAT_DESCRIPTOR_ENTRIES          32
#define UVCH_H264_FORMAT_DESCRIPTOR_LENGTH           52

#define UVCH_UNCOMPRESSED_FORMAT_DESCRIPTOR_ENTRIES  15
#define UVCH_UNCOMPRESSED_FORMAT_DESCRIPTOR_LENGTH   27

#define UVCH_FRAME_DESCRIPTOR_ENTRIES                12
#define UVCH_FRAME_DESCRIPTOR_LENGTH                 26

#define UVCH_H264_FRAME_DESCRIPTOR_ENTRIES           19
#define UVCH_H264_FRAME_DESCRIPTOR_LENGTH            44

/* Define Video Class specific interface descriptor.  */

#define UVCH_MAX_CHANNEL                             8
#define UVCH_NAME_LENGTH                             64

typedef struct UVCH_INTF_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFormatType;
	ULONG           bNrChannels;
	ULONG           bSubframeSize;
	ULONG           bBitResolution;
	ULONG           bSamFreqType;
} UVCH_INTF_DESC;

/* Define Video Class specific output terminal interface descriptor.  */
typedef struct UVCH_OT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bTerminalID;
	ULONG           wTerminalType;
	ULONG           bAssocTerminal;
	ULONG           bSourceID;
	ULONG           iTerminal;
} UVCH_OT_DESC;

/* Define Video Class streaming interface descriptor.  */
typedef struct UVCH_STREAMING_INTF_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubtype;
	ULONG           bTerminalLink;
	ULONG           bDelay;
	ULONG           wFormatTag;
} UVCH_STREAMING_INTF_DESC;


/* Define Video Class specific streaming endpoint descriptor.  */
typedef struct UVCH_STREAMING_ENDPOINT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubtype;
	ULONG           bmAttributes;
	ULONG           bLockDelayUnits;
	ULONG           wLockDelay;
} UVCH_STREAMING_ENDPOINT_DESC;

/* Define Video Class specific processing unit interface descriptor.  */
typedef struct UVCH_PU_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bUnitID;
	ULONG           bSourceID;;
	ULONG           wMaxMultiplier;
	ULONG           bControlSize;
} UVCH_PU_DESC;

/* Define Video Class specific encode unit interface descriptor.  */
typedef struct UVCH_EU_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bUnitID;
	ULONG           bSourceID;;
	ULONG           iEncode;
	ULONG           bControlSize;
} UVCH_EU_DESC;

/* Define Video Class specific input terminal(Camera) interface descriptor.  */
typedef struct UVCH_IT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bTerminalID;
	ULONG           wTerminalType;
	ULONG           bAssocTerminal;
	ULONG           iTerminal;
	ULONG           wObjectiveFocalLengthMin;
	ULONG           wObjectiveFocalLengthMax;
	ULONG           wOcularFocalLength;
	ULONG           bControlSize;
} UVCH_IT_DESC;


/* Define Video Class specific extension unit descriptor.  */

typedef struct UVCH_XU_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bUnitID;
	ULONG           guidExtensionCode0;
	ULONG           guidExtensionCode1;
	ULONG           guidExtensionCode2;
	ULONG           guidExtensionCode3;
	ULONG           bNumControls;
	ULONG           bNrInPins;
} UVCH_XU_DESC;



/* Define Video Class specific VS Interface Input Header descriptor.  */

typedef struct UVCH_VS_INPUT_HEADER_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bNumFormats;
	ULONG           wTotalLength;
	ULONG           bEndpointAddress;
	ULONG           bmInfo;
	ULONG           bTerminalLink;
	ULONG           bStillCaptureMethod;
	ULONG           bTriggerSupport;
	ULONG           bTriggerUsage;
	ULONG           bControlSize;
	// Currently not support bmaControls
} UVCH_VS_INPUT_HEADER_DESC;

/* Define Video Class specific MJPEG Video Format descriptor.  */

typedef struct UVCH_MJPEG_FORMAT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFormatIndex;
	ULONG           bNumFrameDescriptors;
	ULONG           bmFlags;
	ULONG           bDefaultFrameIndex;
	ULONG           bAspectRatioX;
	ULONG           bAspectRatioY;
	ULONG           bmInterlaceFlags;
	ULONG           bCopyProtect;
} UVCH_MJPEG_FORMAT_DESC;

/* Define Video Class specific Umncompressed Video Format descriptor.  */

typedef struct UVCH_UNCOMPRESSED_FORMAT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFormatIndex;
	ULONG           bNumFrameDescriptors;
	ULONG           guidFormat0;
	ULONG           guidFormat1;
	ULONG           guidFormat2;
	ULONG           guidFormat3;
	ULONG           bBitsPerPixel;
	ULONG           bDefaultFrameIndex;
	ULONG           bAspectRatioX;
	ULONG           bAspectRatioY;
	ULONG           bmInterlaceFlags;
	ULONG           bCopyProtect;
} UVCH_UNCOMPRESSED_FORMAT_DESC;

/* Define Video Class specific H264 Video Format descriptor.  */

typedef struct UVCH_H264_FORMAT_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFormatIndex;
	ULONG           bNumFrameDescriptors;
	ULONG			bDefaultFrameIndex;
	ULONG			bMaxCodecConfigDelay;
	ULONG			bmSupportedSliceModes;
	ULONG			bmSupportedSyncFrameTypes;
	ULONG			bResolutionScaling;
	ULONG			Reserved1;
	ULONG			bmSupportedRateControlModes;
	ULONG			wMaxMBperSecOneResolutionNoScalability;
	ULONG			wMaxMBperSecTwoResolutionsNoScalability;
	ULONG			wMaxMBperSecThreeResolutionsNoScalability;
	ULONG			wMaxMBperSecFourResolutionsNoScalability;
	ULONG			wMaxMBperSecOneResolutionTemporalScalability;
	ULONG			wMaxMBperSecTwoResolutionsTemporalScalablility;
	ULONG			wMaxMBperSecThreeResolutionsTemporalScalability;
	ULONG			wMaxMBperSecFourResolutionsTemporalScalability;
	ULONG			wMaxMBperSecOneResolutionTemporalQualityScalability;
	ULONG			wMaxMBperSecTwoResolutionsTemporalQualityScalability;
	ULONG			wMaxMBperSecThreeResolutionsTemporalQualityScalablity;
	ULONG			wMaxMBperSecFourResolutionsTemporalQualityScalability;
	ULONG			wMaxMBperSecOneResolutionsTemporalSpatialScalability;
	ULONG			wMaxMBperSecTwoResolutionsTemporalSpatialScalability;
	ULONG			wMaxMBperSecThreeResolutionsTemporalSpatialScalability;
	ULONG			wMaxMBperSecFourResolutionsTemporalSpatialScalability;
	ULONG			wMaxMBperSecOneResolutionFullScalability;
	ULONG			wMaxMBperSecTwoResolutionsFullScalability;
	ULONG			wMaxMBperSecThreeResolutionsFullScalability;
	ULONG			wMaxMBperSecFourResolutionsFullScalability;
} UVCH_H264_FORMAT_DESC;

/* Define Video Class specific Video Frame descriptor.  */
typedef struct UVCH_FRAME_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFrameIndex;
	ULONG           bmCapabilities;
	ULONG           wWidth;
	ULONG           wHeight;
	ULONG           dwMinBitRate;
	ULONG           dwMaxBitRate;
	ULONG           dwMaxVideoFrameBufferSize;
	ULONG           dwDefaultFrameInterval;
	ULONG           bFrameIntervalType;
	// Currently not the following items
} UVCH_FRAME_DESC;

/* Define Video Class specific H264 Video Frame descriptor.  */
typedef struct UVCH_H264_FRAME_DESC_STRUCT {
	ULONG           bLength;
	ULONG           bDescriptorType;
	ULONG           bDescriptorSubType;
	ULONG           bFrameIndex;
	ULONG			wWidth;
	ULONG			wHeight;
	ULONG			wSARwidth;
	ULONG			wSARheight;
	ULONG			wProfile;
	ULONG			bLevelIDC;
	ULONG			wConstrainedToolset;
	ULONG			bmSupportedUsages;
	ULONG			bmCapabilities;
	ULONG			bmSVCCapabilities;
	ULONG			bmMVCCapabilities;
	ULONG			dwMinBitRate;
	ULONG			dwMaxBitRate;
	ULONG			dwDefaultFrameInterval;
	ULONG			bNumFrameIntervals;
	// Currently not the following items
} UVCH_H264_FRAME_DESC;

/* Define Video Class instance structure.  */
typedef struct UVCH_INSTANCE_STRUCT {
	struct UVCH_INSTANCE_STRUCT *next_instance;
	UX_HOST_CLASS   *ux_host_class;
	UX_DEVICE       *ux_device;
	UX_INTERFACE    *streaming_interface;
	UX_INTERFACE    *streaming_interface_active;
	ULONG           vc_interface_number;
	ULONG           vs_interface_number;
	UX_ENDPOINT     *streaming_endpoint;
	struct UVCH_TRANSFER_REQUEST_STRUCT *head_transfer_request;
	struct UVCH_TRANSFER_REQUEST_STRUCT *tail_transfer_request;
	UINT            state;
	ULONG           terminal_link;
	ULONG           type;
	UCHAR *         config_desc;
	ULONG           config_desc_length;
	TX_THREAD       control_thread;
	TX_SEMAPHORE    control_semaphore;
	VOID            *control_thread_stack;
	ULONG           it_id;
	ULONG           xu_id;
	ULONG           pu_id;
	ULONG           format_count;
	UVCH_FORMAT_INFO *format_lut;
	ULONG			uvc_ver;
	/* UVC 1.5 */
	ULONG			eu_id;
} UVCH_INSTANCE;

/* Define Video Class isochronous USB transfer request structure.  */

typedef struct UVCH_TRANSFER_REQUEST_STRUCT {
	ULONG           status;
	UCHAR *         data_pointer;
	ULONG           requested_length;
	ULONG           actual_length;
	VOID            (*completion_function) (struct UVCH_TRANSFER_REQUEST_STRUCT *);
	TX_SEMAPHORE    semaphore;
	VOID            *class_instance;
	UINT            completion_code;
	struct UVCH_TRANSFER_REQUEST_STRUCT *next_request;
	UX_TRANSFER     ux_request;
} UVCH_TRANSFER_REQUEST;

/* Define Video Class control request structure.  */

typedef struct UVCH_CONTROL_REQUEST_STRUCT {
	ULONG   selector;
	ULONG   size;
	INT     set_cur;
	INT     get_cur;
	INT     get_min;
	INT     get_max;
	INT     get_res;
	INT     get_len;
	INT     get_info;
	INT     get_def;
} UVCH_CONTROL_REQUEST;

/* Define Video Class probe control structure.  */

typedef struct UVCH_PROBE_CONTROL_STRUCT {
	USHORT  bmHint;
	UCHAR   bFormatIndex;
	UCHAR   bFrameIndex;
	ULONG   dwFrameInterval;
	USHORT  wKeyFrameRate;
	USHORT  wPFrameRate;
	USHORT  wCompQuality;
	USHORT  wCompWindowSize;
	USHORT  wDelay;
	ULONG   dwMaxVideoFrameSize;
	ULONG   dwMaxPayloadTransferSize;
} UVCH_PROBE_CONTROL;

/* Define Video Class probe control structure.  */

typedef struct UVCH_INPUT_TERMINAL_STRUCT {
	UCHAR   scanning_mode;
	UCHAR   ae_mode;
	UCHAR   ae_priority;
	ULONG   exposure_time_absolute;
	CHAR    exposure_time_relative;
	USHORT  focus_absolute;
	UCHAR   focus_relative[2];
	UCHAR   focus_auto;
	USHORT  iris_absolute;
	UCHAR   iris_relative;
	USHORT  zoom_absolute;
	UCHAR   zoom_relative[3];
	ULONG   pantilt_absolute[2];
	UCHAR   pantilt_relative[4];
	SHORT   roll_absolute;
	UCHAR   roll_relative[2];
	UCHAR   privacy;
}  UVCH_INPUT_TERMINAL;

typedef struct UVCH_PROCESSING_UNIT_STRUCT {
	USHORT  backlight;
	SHORT   brightness;
	USHORT  contrast;
	USHORT  gain;
	UCHAR   power_line_frequency;
	SHORT   hue;
	USHORT  saturation;
	USHORT  sharpness;
	USHORT  gamma;
	USHORT  wb_temperature;
	UCHAR   wb_temperature_auto;
	USHORT  wb_component[2];
	UCHAR   wb_component_auto;
	USHORT  digital_multiplier;
	USHORT  digital_multiplier_limit;
	UCHAR   hue_auto;
	UCHAR   analog_video_standard;
	UCHAR   analog_lock_status;
}  UVCH_PROCESSING_UNIT;

typedef struct UVC_ENCODE_UNIT_PROFILE_TOOLSET_STRUCT {
	USHORT	profile;
	USHORT	toolset;
	UCHAR	settings;
} UVC_ENCODE_UNIT_PROFILE_TOOLSET;

typedef struct UVC_ENCODE_UNIT_SYNC_REF_FRAME_STRUCT {
	UCHAR	sync_frame_type;
	USHORT	sync_frame_interval;
	UCHAR	gradual_decoder_refresh;
} UVC_ENCODE_UNIT_SYNC_REF_FRAME;

typedef struct UVCH_ENCODING_UNIT_STRUCT {
	USHORT							select_layer;
	USHORT							resolution[2];		// wWidth and wHeight
	UVC_ENCODE_UNIT_PROFILE_TOOLSET	profile;
	ULONG							frame_interval;
	USHORT							slice_mode[2];
	UCHAR							rate_control_mode;
	ULONG							average_bit_rate;
	ULONG							cpb_size;
	ULONG							peak_bit_rate;
	USHORT							quantization_params[3];
	UCHAR							qp_range[2];
	UVC_ENCODE_UNIT_SYNC_REF_FRAME	sync_ref_frame;
	UCHAR							ltr_buffer[2];
	UCHAR							ltr_picture[2];
	USHORT							ltr_validation;
	ULONG							sei_payload_type[2];
	UCHAR							priority;
	UCHAR							start_or_stop_layer;
	UCHAR							leve_idc_limit;
	USHORT							error_resiliency;
}  UVCH_ENCODING_UNIT;

/* Define Video Class channel/value control structures.  */
typedef struct UX_HOST_CLASS_VIDEO_CONTROL_STRUCT {
	ULONG  bmcontrol_bit_in_descriptor; // Used in all request except set_cur
	ULONG  selector;                    // Used in set_cur
	ULONG  request;
} UX_HOST_CLASS_VIDEO_CONTROL;

/* Define Video Class function prototypes.  */

UINT    _uvch_activate(UX_HOST_CLASS_COMMAND *command);
UINT    _uvch_set_configuration(UVCH_INSTANCE *video);
UINT    _uvch_deactivate(UX_HOST_CLASS_COMMAND *command);
UINT    _uvch_descriptor_get(UVCH_INSTANCE *video);
UINT    _uvch_controls_list_get(UVCH_INSTANCE *video);
UINT    _uvch_device_type_get(UVCH_INSTANCE *video);
UINT    _uvch_endpoints_get(UVCH_INSTANCE *video);
UINT    _uvch_max_stream_packet_size_get(UVCH_INSTANCE *video);
UINT    _uvch_entry(UX_HOST_CLASS_COMMAND *command);
UINT    _uvch_read(UVCH_INSTANCE *instance,
				UX_EHCI_ISO_REQUEST *request,
				UINT32 PacketNumbers,
				void (*complete_func)(UX_EHCI_ISO_REQUEST *request),
				UINT32 append);
UINT    _uvch_streaming_terminal_get(UVCH_INSTANCE *video);
UINT    _uvch_transfer_request(UVCH_INSTANCE *video, UVCH_TRANSFER_REQUEST *video_transfer_request);
VOID    _uvch_transfer_request_completed(UX_TRANSFER *transfer_request);
UINT    _uvch_write(UVCH_INSTANCE *video, UVCH_TRANSFER_REQUEST *video_transfer_request);
UINT    _uvch_streaming_interface_get(UVCH_INSTANCE *video);
void    _uvch_streaming_memory_free(UVCH_INSTANCE *video);
UINT    _uvch_control_probe(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control, UVCH_PROBE_CONTROL* probe);
UINT    _uvch_control_commit(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control,UVCH_PROBE_CONTROL* probe_control, ULONG selector);
VOID    _uvch_print_probe_value(UVCH_PROBE_CONTROL* probe, ULONG request);

UINT    _uvch_alternate_setting_locate(UVCH_INSTANCE *video, UVCH_PROBE_CONTROL* probe_ctrl,UINT *alternate_setting);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_device_data.c
\*-----------------------------------------------------------------------------------------------*/
UVCH_INPUT_TERMINAL* uhc_uvc_get_it_data(ULONG request);
UVCH_PROCESSING_UNIT* uhc_uvc_get_pu_data(ULONG request);
UVCH_ENCODING_UNIT*   uhc_uvc_get_eu_data(ULONG request);
UVCH_PROBE_CONTROL* uhc_uvc_get_probe_data(ULONG request);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_control_input_terminal.c
\*-----------------------------------------------------------------------------------------------*/
UINT    _uvch_it_get_value(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control, UVCH_INPUT_TERMINAL* InputTerminal);
UINT    _uvch_it_desc_data_get(UVCH_INSTANCE *video, INT* pControl_size, UCHAR** bmControls);
VOID    _uvch_it_print_values(UVCH_INPUT_TERMINAL* InputTerminal,ULONG request);
void    _uvch_it_bitmap_reset(void);
void    _uvch_it_bitmap_set(ULONG bit_field);
INT     _uvch_it_is_request_supported(ULONG bmControl_bit, ULONG request);
UINT32  _uvch_it_fill_control_item(UVCH_CONTROL_ITEM *item, UVC_CAMERA_TERMINAL_CONTROL_SELECTOR selector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_control_processing_unit.c
\*-----------------------------------------------------------------------------------------------*/
void    _uvch_pu_set_value(UVCH_INSTANCE *video,ULONG selector,ULONG value);
VOID    _uvch_pu_print_values(UVCH_PROCESSING_UNIT* ProcessingUnit, ULONG request);
UINT    _uvch_pu_desc_data_get(UVCH_INSTANCE *video, INT* pControl_size, UCHAR** bmControls);
void    _uvch_pu_bitmap_set(ULONG bit_field);
void    _uvch_pu_bitmap_reset(void);
UINT    _uvch_pu_get_device_control_value(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control, UVCH_PROCESSING_UNIT* ProcessingUnit);
INT     _uvch_pu_is_request_supported(ULONG bmControl_bit,ULONG request);
UINT32  _uvch_pu_fill_control_item(UVCH_CONTROL_ITEM *item, UVC_PROCESSING_UNIT_CONTROL_SELECTOR selector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_control_encoding_unit.c
\*-----------------------------------------------------------------------------------------------*/
void    _uvch_eu_set_value(UVCH_INSTANCE *video, ULONG selector, ULONG *value);
VOID    _uvch_eu_print_values(UVCH_ENCODING_UNIT* EncodingUnit, ULONG request);
UINT    _uvch_eu_desc_data_get(UVCH_INSTANCE *video, INT* pControl_size, UCHAR** bmControls, UCHAR** bmControlsRuntime);
void    _uvch_eu_bitmap_set(ULONG bit_field, ULONG runtime);
void    _uvch_eu_bitmap_reset(void);
UINT    _uvch_eu_get_device_control_value(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control, UVCH_ENCODING_UNIT* EncodingUnit);
INT     _uvch_eu_is_request_supported(ULONG bmControl_bit,ULONG request);
UINT32  _uvch_eu_fill_control_item(UVCH_CONTROL_ITEM *item, UVC_ENCODING_UNIT_CONTROL_SELECTOR selector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_device_data.c
\*-----------------------------------------------------------------------------------------------*/
UINT    _uvch_xu_get_value(UVCH_INSTANCE *video, UX_HOST_CLASS_VIDEO_CONTROL *video_control);
UINT    _uvch_xu_desc_data_get(UVCH_INSTANCE *video, INT* pControl_size, UCHAR** bmControls);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_device_initialize.c
\*-----------------------------------------------------------------------------------------------*/
VOID    _uvch_device_initialize(UVCH_INSTANCE *video);
char    *_uvch_get_control_request_string(ULONG request);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in ux_host_class_video_thread.c
\*-----------------------------------------------------------------------------------------------*/
TX_EVENT_FLAGS_GROUP *_uvch_get_control_thread_event_flag(void);
VOID    _uvch_set_cur(ULONG unit,ULONG selector,ULONG value);
VOID    _uvch_streaming_start(void);
VOID    _uvch_streaming_stop(void);
VOID    _uvch_print_device_values(void);
VOID    _uvch_probe_and_commit(ULONG formatIndex,ULONG frameIndex,ULONG fps);
VOID _uvch_get_current_probe_setting(ULONG *formatIndex,ULONG *frameIndex,ULONG *fps);
VOID    _uvch_control_thread_entry(ULONG class_address);
UINT32  _uvch_get_device_info(UVCH_DEVICE_INFO *info);
UVCH_INSTANCE *_uvch_get_instance(void);
#endif


