#ifndef DISPLAY_H_
#define DISPLAY_H_

/**
 * @defgroup Display
 *
 * @brief video output controller
 *
 * Display module is module used to control output device.\n
 * In most cast, we provide two vout channel: Digital channel and Full functional channel.\n
 *
 */

/**
 * @addtogroup Display
 * @{
 */
#include    "mw.h"
#include    "AmbaSensor.h"
#include    "AmbaHDMI.h"

/** The type of device */
typedef enum _AMP_DISP_DEV_IDX_e_ {
    AMP_DISP_LCD = 0, /**< disp hdlr on lcd */
    AMP_DISP_CVBS, /**< disp hdlr on cvbs */
    AMP_DISP_HDMI, /**< disp hdlr on hdmi */
    AMP_DSIP_NONE /**< disp hdlr on nothing */
} AMP_DISP_DEV_IDX_e;

/** The mode of LCD */
typedef enum _AMP_DISP_LCD_MODE_e_ {
    AMP_DISP_LCD_MODE_DEFAULT = 0xFFFF, /**< Default mode */
} AMP_DISP_LCD_MODE_e;

/** The mode of HDMI */
typedef enum _AMP_DISP_HDMI_MODE_e_ {
    /* Code Short Name     ID    Aspect Ratio              HxV @ Frate */
    AMP_DISP_ID_RESERVED = 0, /**<  N/A */
    AMP_DISP_ID_DMT0659, /**<  4:3,            640x480p @ 59.94/60Hz */
    AMP_DISP_ID_480P, /**<  4:3,            720x480p @ 59.94/60Hz */
    AMP_DISP_ID_480P_WIDE, /**< 16:9,            720x480p @ 59.94/60Hz */
    AMP_DISP_ID_720P, /**< 16:9,           1280x720p @ 59.94/60Hz */
    AMP_DISP_ID_1080I, /**< 16:9,          1920x1080i @ 59.94/60Hz */
    AMP_DISP_ID_480I, /**<  4:3,      720(1440)x480i @ 59.94/60Hz */
    AMP_DISP_ID_480I_WIDE, /**< 16:9,      720(1440)x480i @ 59.94/60Hz */
    AMP_DISP_ID_240P, /**<  4:3,      720(1440)x240p @ 59.94/60Hz */
    AMP_DISP_ID_240P_WIDE, /**< 16:9,      720(1440)x240p @ 59.94/60Hz */
    AMP_DISP_ID_480I4X, /**<  4:3,         (2880)x480i @ 59.94/60Hz */
    AMP_DISP_ID_480I4X_WIDE, /**< 16:9,         (2880)x480i @ 59.94/60Hz */
    AMP_DISP_ID_240P4X, /**<  4:3,         (2880)x240p @ 59.94/60Hz */
    AMP_DISP_ID_240P4X_WIDE, /**< 16:9,         (2880)x240p @ 59.94/60Hz */
    AMP_DISP_ID_480P2X, /**<  4:3,           1440x480p @ 59.94/60Hz */
    AMP_DISP_ID_480P2X_WIDE, /**< 16:9,           1440x480p @ 59.94/60Hz */
    AMP_DISP_ID_1080P, /**< 16:9,          1920x1080p @ 59.94/60Hz */
    AMP_DISP_ID_576P, /**<  4:3,            720x576p @ 50Hz */
    AMP_DISP_ID_576P_WIDE, /**< 16:9,            720x576p @ 50Hz */
    AMP_DISP_ID_720P50, /**< 16:9,           1280x720p @ 50Hz */
    AMP_DISP_ID_1080I25, /**< 16:9,          1920x1080i @ 50Hz (V-Total 1125) */
    AMP_DISP_ID_576I, /**<  4:3,      720(1440)x576i @ 50Hz */
    AMP_DISP_ID_576I_WIDE, /**< 16:9,      720(1440)x576i @ 50Hz */
    AMP_DISP_ID_288P, /**<  4:3,      720(1440)x288p @ 50Hz */
    AMP_DISP_ID_288P_WIDE, /**< 16:9,      720(1440)x288p @ 50Hz */
    AMP_DISP_ID_576I4X, /**<  4:3,         (2880)x576i @ 50Hz */
    AMP_DISP_ID_576I4X_WIDE, /**< 16:9,         (2880)x576i @ 50Hz */
    AMP_DISP_ID_288P4X, /**<  4:3,         (2880)x288p @ 50Hz */
    AMP_DISP_ID_288P4X_WIDE, /**< 16:9,         (2880)x288p @ 50Hz */
    AMP_DISP_ID_576P2X, /**<  4:3,           1440x576p @ 50Hz */
    AMP_DISP_ID_576P2X_WIDE, /**< 16:9,           1440x576p @ 50Hz */
    AMP_DISP_ID_1080P50, /**< 16:9,          1920x1080p @ 50Hz */
    AMP_DISP_ID_1080P24, /**< 16:9,          1920x1080p @ 23.98/24Hz */
    AMP_DISP_ID_1080P25, /**< 16:9,          1920x1080p @ 25Hz */
    AMP_DISP_ID_1080P30, /**< 16:9,          1920x1080p @ 29.97/30Hz */
    AMP_DISP_ID_480P4X, /**<  4:3,         (2880)x480p @ 59.94/60Hz */
    AMP_DISP_ID_480P4X_WIDE, /**< 16:9,         (2880)x480p @ 59.94/60Hz */
    AMP_DISP_ID_576P4X, /**<  4:3,         (2880)x576p @ 50Hz */
    AMP_DISP_ID_576P4X_WIDE, /**< 16:9,         (2880)x576p @ 50Hz */
    AMP_DISP_ID_1080I25_AS, /**< 16:9,          1920x1080i @ 50Hz (V-Total 1250) */
    AMP_DISP_ID_1080I50, /**< 16:9,          1920x1080i @ 100Hz */
    AMP_DISP_ID_720P100, /**< 16:9,           1280x720p @ 100Hz */
    AMP_DISP_ID_576P100, /**<  4:3,            720x576p @ 100Hz */
    AMP_DISP_ID_576P100_WIDE, /**< 16:9,            720x576p @ 100Hz */
    AMP_DISP_ID_576I50, /**<  4:3,      720(1440)x576i @ 100Hz */
    AMP_DISP_ID_576I50_WIDE, /**< 16:9,      720(1440)x576i @ 100Hz */
    AMP_DISP_ID_1080I60, /**< 16:9,          1920x1080i @ 119.88/120Hz */
    AMP_DISP_ID_720P120, /**< 16:9,           1280x720p @ 119.88/120Hz */
    AMP_DISP_ID_480P119, /**<  4:3,            720x480p @ 119.88/120Hz */
    AMP_DISP_ID_480P119_WIDE, /**< 16:9,            720x480p @ 119.88/120Hz */
    AMP_DISP_ID_480I59, /**<  4:3,      720(1440)x480i @ 119.88/120Hz */
    AMP_DISP_ID_480I59_WIDE, /**< 16:9,      720(1440)x480i @ 119.88/120Hz */
    AMP_DISP_ID_576P200, /**<  4:3,            720x576p @ 200Hz */
    AMP_DISP_ID_576P200_WIDE, /**< 16:9,            720x576p @ 200Hz */
    AMP_DISP_ID_576I100, /**<  4:3,      720(1440)x576i @ 200Hz */
    AMP_DISP_ID_576I100_WIDE, /**< 16:9,      720(1440)x576i @ 200Hz */
    AMP_DISP_ID_480P239, /**<  4:3,            720x480p @ 239.76/240Hz */
    AMP_DISP_ID_480P239_WIDE, /**< 16:9,            720x480p @ 239.76/240Hz */
    AMP_DISP_ID_480I119, /**<  4:3,      720(1440)x480i @ 239.76/240Hz */
    AMP_DISP_ID_480I119_WIDE, /**< 16:9,      720(1440)x480i @ 239.76/240Hz */
    AMP_DISP_ID_720P24, /**< 16:9,           1280x720p @ 23.98/24Hz */
    AMP_DISP_ID_720P25, /**< 16:9,           1280x720p @ 25Hz */
    AMP_DISP_ID_720P30, /**< 16:9,           1280x720p @ 29.97/30Hz */
    AMP_DISP_ID_1080P120, /**< 16:9,          1920x1080p @ 119.88/120Hz */
    AMP_DISP_ID_1080P100, /**< 16:9,          1920x1080p @ 100Hz */

    AMP_DISP_ID_HDMI_EXT = 128, /**<VICs after this offset are all the extended resolution formats */
    AMP_DISP_ID_2160P30, /**< 16:9,          3840x2160p @ 29.97/30Hz */
    AMP_DISP_ID_2160P25, /**< 16:9,          3840x2160p @ 25Hz */
    AMP_DISP_ID_2160P24, /**< 16:9,          3840x2160p @ 23.98/24Hz */
    AMP_DISP_ID_2160P24_SMPTE, /**< 16:9,          4096x2160p @ 24Hz (SMPTE) */

    AMP_DISP_NUM_ID, /**<Total Number of pre-defined video ID codes */
} AMP_DISP_HDMI_MODE_e;

/** The system type of device */
typedef enum _AMP_DISP_VOUT_SYSTEM_e_ {
    AMP_DISP_NTSC = 0, /**< NTSC */
    AMP_DISP_PAL /**< PAL/SECAM */
} AMP_DISP_VOUT_SYSTEM_e;

/** The type of channel */
typedef enum _AMP_DISP_CHANNEL_IDX_e_ {
    AMP_DISP_CHANNEL_DCHAN = 0, /**< VOUT channel 0, for digital output only(A9) */
    AMP_DISP_CHANNEL_FCHAN, /**< VOUT channel 1, for full functional output only(A9)*/
    AMP_DISP_CHANNEL_NUM /**< Total number of VOUT channels */
} AMP_DISP_CHANNEL_IDX_e;

/** The source of window */
typedef enum _AMP_DISP_WINDOW_SRC_e_ {
    AMP_DISP_OSD = 0, /**< OSD window source */
    AMP_DISP_ENC, /**< ENC window source */
    AMP_DISP_DEC, /**< DEC window source */
    AMP_DISP_DEFIMG, /**< default image window source */
    AMP_DISP_BACKGROUND_COLOR /**< background window source */
} AMP_DISP_WINDOW_SRC_e;

/** LCD back light control */
typedef enum _AMP_DISP_LCD_BACKLIGHT_e_ {
    AMP_DISP_LCD_BACKLIGHT_ON_INIT = 0, /**< Turn on LCD back light when device start */
    AMP_DISP_LCD_BACKLIGHT_ON_WINDOW,   /**< Turn on LCD back light when enabling display window */
    AMP_DISP_LCD_BACKLIGHT_IGNORE,      /**< Do not control LCD backlight in mw_display */
} AMP_DISP_LCD_BACKLIGHT_e;

typedef struct _AMP_DISP_CUSTOM_CFG_s_ {
    UINT8 EnCustomCfg;
    union {
        struct {
            AMBA_HDMI_VIDEO_FRAME_LAYOUT_e      FrameLayout;    ///< 2D/3D video transmission
            AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e    PixelFormat;    ///< RGB/YCbCr color space 
            AMBA_HDMI_QUANTIZATION_RANGE_e      QuantRange;     ///< Quantization rage of RGB/YCC 
            HDMI_AUDIO_SAMPLE_RATE_e            SampleRate;     ///< Audio sample rate 
            HDMI_AUDIO_CHANNEL_ALLOC_e          SpeakerAlloc;   ///< Audio channel/speaker allocation ID 
            HDMI_AUDIO_CLOCK_FREQUENCY_e        OverSample;     ///< Audio clock oversampling ratio 
        } HDMI;
        struct {
            AMP_DISP_LCD_BACKLIGHT_e            BackLight;      ///< LCD back light control
        } LCD;
    } Cfg;
    UINT8  LowDelayEnable;
    UINT16 LowDelayTime;
} AMP_DISP_CUSTOM_CFG_s;

/** Device setup config */
typedef struct _AMP_DISP_DEV_CFG_s_ {
    AMP_DISP_CHANNEL_IDX_e Channel; /**< The channel device use */
    AMP_DISP_DEV_IDX_e DeviceId; /**< Device type */
    UINT32 DeviceMode; /**< Resolution */
    UINT32 DeviceAr; /**< Device aspect ratio */
    AMP_DISP_CUSTOM_CFG_s CustomCfg; /**< customize cfg, set only if you do not want default value */
} AMP_DISP_DEV_CFG_s;

/** Window handler config */
typedef struct _AMP_DISP_WINDOW_CFG_s_ {
    AMP_DISP_WINDOW_SRC_e Source; /**< The source of window */
    /** information to fetch source. For OSD, it will be osd hdlr. For Enc it will be vin ch
     For Dec, it will be video/still dec hdlr */
    union {
        struct {
            void *OsdHdlr;
        } Osd;
        struct {
            /**
             source from which vin channel
             */
            AMBA_DSP_CHANNEL_ID_u VinCh;
            /**
             blend table used when overlap with other enc windows
             */
            AMP_2D_BUFFER_s BlendTable;

            /**
             ViewZoneID indicate the view zoone of the big input. defualt 0.
             */
            UINT8 ViewZoneID;
            /**
             rotate the source
             */
            AMP_ROTATION_e Rotate;
        } Enc;
        struct {
            /**
             decoder handler
             */
            void *DecHdlr;
            /**
             rotate the source
             */
            AMP_ROTATION_e Rotate;
        } Dec;
        struct {
            AMP_YUV_BUFFER_s *Image;
            UINT8 FieldRepeat;
        } DefImage;
    } SourceDesc;
    AMP_AREA_s CropArea; /**< The crop area of window source buffer */
    AMP_AREA_s TargetAreaOnPlane; /**< Window display position in device */
    UINT32 Layer; /**< The small number of layer would be put at the bottom */
} AMP_DISP_WINDOW_CFG_s;

/** Window handler for user */
typedef struct _AMP_DISP_WINDOW_HDLR_s_ {
    void *Ctx; /**< Pointer to codec context */
} AMP_DISP_WINDOW_HDLR_s;

/** CSC config structure */
typedef struct _AMP_DISP_CSC_CONFIG_s_ {
    float CscMatrix[9]; /**< the 3x3 matrix */
    INT16 Constant[3]; /**< Constants offset */
    UINT16 Clamp[6]; /**< Clamping values */
} AMP_DISP_CSC_CONFIG_s;

/** Display module entire config */
typedef struct _AMP_DISP_CFG_s_ {
    AMP_DISP_DEV_CFG_s Device; /**< Device info */
    UINT32 MaxNumWindow; /**, The max number of window handler set from upper layer */
    //? TBD add frame rate
    //? TBD remove type
    AMP_DISP_VOUT_SYSTEM_e SystemType; /**< vout system type */
    UINT32 ScreenRotate; /**< Flag for device screen rotation */
    //TBD add 3d mode enum
    UINT8 Out3dMode; /**< 3D output mode */
    AMP_CALLBACK_f CbCfgUpdated; /**< Callback for vout change use */
    AMP_CALLBACK_f CbCodecEvent; /**< Callback for vout change use */
} AMP_DISP_CFG_s;

typedef struct _AMP_DISP_FRAME_RATE_s_ {
    UINT8 Interlace; /**< 1 - Interlace; 0 - Progressive */
    UINT32 TimeScale; /**< time scale */
    UINT32 NumUnitsInTick; /**< Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
} AMP_DISP_FRAME_RATE_s;

/** Vout info from driver or dsp */
typedef struct _AMP_DISP_VOUT_INFO_s_ {
    UINT8 Enable; /**< Device enabled */
    UINT16 VoutWidth; /**< Device width */
    UINT16 VoutHeight; /**< Device height */
    AMP_DISP_FRAME_RATE_s FrameRate; /**< Frame rate */
    float DevPixelAr; /**< dev pixel aspect ratio (pixel width:pixel height) = (Ar : 1)*/
    UINT32 Out3dMode; /**< 3D mode programed on device */
    UINT32 VoutReady; /**< Vout is ready and could take command again */
} AMP_DISP_VOUT_INFO_s;

/** Display handler for user */
typedef struct _AMP_DISP_HDLR_s_ {
    void *Ctx; /**< Pointer to codec context */
} AMP_DISP_HDLR_s;

/** Display module intial parameters */
typedef struct _AMP_DISP_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr; /**< Buffer start address for display module to create display hanlder array */
    UINT32 MemoryPoolSize; /**< Size of Buffer */
    UINT32 MaxDeviceInterHdlr; /**< The number of device handler user want */
    UINT32 MaxWindowInterHdlr; /**< The number of window handler user want (Totally window handler number)*/
    UINT32 MaxHdmiCheck; /**< The maximum check hdmi times */
} AMP_DISP_INIT_CFG_s;

/** Device basic info */
typedef struct _AMP_DISP_INFO_s_ {
    AMP_DISP_VOUT_INFO_s DeviceInfo; /**< Device vout info */
    AMP_DISP_WINDOW_HDLR_s *WindowHdlr; /**< Active window handler instance */
} AMP_DISP_INFO_s;

/**
 * Get default intial config to initial display module.
 *
 * Give some default vaule to user reference
 *
 * @param[out] Cfg - Initial config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetDefaultInitCfg(AMP_DISP_INIT_CFG_s *cfg);

/**
 * Intial display module.
 *
 * Arrange display handler memory and inital other flags.
 *
 * @param[in] Cfg - Initial config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Init(AMP_DISP_INIT_CFG_s *cfg);

/**
 * Get default config to create display handler.
 *
 * Give some default vaule to user reference
 *
 * @param[out] Cfg - Display handler config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetDefaultCfg(AMP_DISP_CFG_s *cfg);

/**
 * Create a display handler.
 *
 * A device would need a display handler, and would arrange window hanlder memoey
 *
 * @param[in] Cfg - Display handler config parameters
 * @param[out] DispHdlr - Display handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Create(AMP_DISP_CFG_s *cfg,
                             AMP_DISP_HDLR_s **dispHdlr);

/**
 * Delete a display handler.
 *
 * Clean the display handler structure
 *
 * @param[in] DispHdlr - Display handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Delete(AMP_DISP_HDLR_s *dispHdlr);

/**
 * Set/Update device info to a display handler.
 *
 * Update the device info in display handler, and would take effect when display start
 *
 * @param[in] DispHdlr - Display handler instance for user
 * @param[in] DevCfg - Device config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_SetDeviceCfg(AMP_DISP_HDLR_s *dispHdlr,
                                   AMP_DISP_DEV_CFG_s *devCfg);

/**
 * Get device info to a display handler.
 *
 *
 * @param[in] DispHdlr - Display handler instance for user
 * @param[out] DevCfg - Device config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetDeviceCfg(AMP_DISP_HDLR_s *dispHdlr,
                                   AMP_DISP_DEV_CFG_s *devCfg);

/**
 * Get window default config parameters
 *
 * Give some default value to user reference
 *
 * @param[out] Cfg - Window config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetDefaultWindowCfg(AMP_DISP_WINDOW_CFG_s *cfg);

/**
 * Creat a window handler
 *
 * User need hold a window handler to tell others where to show on display
 *
 * @param[in] DispHdlr - The display handler
 * @param[in] Cfg - Window config parameters
 * @param[out] WindowHdlr - Window handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_CreateWindow(AMP_DISP_HDLR_s *dispHdlr,
                                   AMP_DISP_WINDOW_CFG_s *cfg,
                                   AMP_DISP_WINDOW_HDLR_s **windowHdlr);

/**
 * Delete a window handler
 *
 * Clean the window handler structure
 *
 * @param[in] WindowHdlr - The window handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_DeleteWindow(AMP_DISP_WINDOW_HDLR_s *windowHdlr);

/**
 * Set/Update the window handler info
 *
 * Update the window info and send event to app to preocess if window is active.
 *
 * @param[in] WindowHdlr - The window handler instance for user
 * @param[in] Cfg - The new window config paramters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_SetWindowCfg(AMP_DISP_WINDOW_HDLR_s *windowHdlr,
                                   AMP_DISP_WINDOW_CFG_s *cfg);

/**
 * Get a window handler info
 *
 * Give the info of window handler to user
 *
 * @param[in] WindowHdlr - The window handler instance for user
 * @param[out] Cfg - Window handler config info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetWindowCfg(AMP_DISP_WINDOW_HDLR_s *windowHdlr,
                                   AMP_DISP_WINDOW_CFG_s *cfg);

/**
 * Acitive window on device. Take effect after AmpDisplay_UpdateWindowState
 *
 * @param[in] WindowHdlr - Window handler instance for user to show osd.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_SetWindowActivateFlag(AMP_DISP_WINDOW_HDLR_s *windowHdlr,
                                            UINT8 activeFlag);

/**
 * update window state on a display. AmpDisplay_SetWindowCfg and AmpDisplay_ActivateWindow take effect after this function invoked.
 *
 * @param [in] DispHdlr - The display windows work on
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Update(AMP_DISP_HDLR_s *dispHdlr);

// use AmpDisplay_SetWindowActivateFlag and AmpDisplay_Update to replace these two function
//extern int AmpDisplay_ActivateVideoWindow(AMP_DISP_WINDOW_HDLR_s *windowHdlr);
//extern int AmpDisplay_DeactivateVideoWindow(AMP_DISP_WINDOW_HDLR_s *windowHdlr);

/**
 * Setup the device
 *
 * Tell the driver to let device on, and return the call back event to app
 *
 * @param[in] DispHdlr - The display handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Start(AMP_DISP_HDLR_s *dispHdlr);

/**
 * Disable the device
 *
 * Tell the driver to shutdown the device, and return the call back event to app
 *
 * @param[in] DispHdlr - The display handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_Stop(AMP_DISP_HDLR_s *dispHdlr);

/**
 * Get a display device & system info set to dsp
 *
 * Return the basic device info and others to user
 *
 * @param[in] DispHdlr - The window handler instance for user
 * @param[out] OutputInfo - Device vout info
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetInfo(AMP_DISP_HDLR_s *dispHdlr,
                              AMP_DISP_INFO_s *outputInfo);

/**
 *
 * @param [in] dispHdlr - The display handler instance to set value
 * @param [in] cscCfg - csc config to set
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_SetVoutCsc(AMP_DISP_HDLR_s *dispHdlr,
                                 AMP_DISP_CSC_CONFIG_s *cscCfg);

/**
 *
 * @param [in] dispHdlr - The display handler instance to get value
 * @param [out] cscCfg - csc config
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_GetVoutCsc(AMP_DISP_HDLR_s *dispHdlr,
                                 AMP_DISP_CSC_CONFIG_s *cscCfg);

/**
 * Set max lcd size for display module. system need the information form some feature before init.
 *
 * @param width [in] - max vout width
 * @param Height [in] - max vout height
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDisplay_SetMaxVout0Size(UINT32 width, UINT32 height);

/**
 * @}
 */

#endif /* DISPLAY_H_ */
