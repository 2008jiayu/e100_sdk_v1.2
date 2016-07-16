#ifndef OSD_H_
#define OSD_H_

#include        "mw.h"
#include        "Display.h"

/**
 * @defgroup OSD
 * @brief On screen display apis
 *
 * OSD command set provide API to setup buffer of on screen display.\n
 * User should prepare buffer with data in specified format.\n
 * DSP will take blend the buffer with video plan.
 *
 */

/**
 * @addtogroup OSD
 * @{
 */

/** The osd formate */
typedef enum _AMP_DISP_OSD_FORMAT_e_ {
    AMP_OSD_8BIT_CLUT_MODE = 0, /**< 8-bit color look-up table mode */

    /* 16-bit direct mode */
    AMP_OSD_16BIT_VYU_RGB_565 = 1, /**< VYU_RGB_565            */
    AMP_OSD_16BIT_UYV_BGR_565, /**< UYV_BGR_565            */
    AMP_OSD_16BIT_AYUV_4444, /**< AYUV_4444              */
    AMP_OSD_16BIT_RGBA_4444, /**< RGBA_4444              */
    AMP_OSD_16BIT_BGRA_4444, /**< BGRA_4444              */
    AMP_OSD_16BIT_ABGR_4444, /**< ABGR_4444              */
    AMP_OSD_16BIT_ARGB_4444, /**< ARGB_4444              */
    AMP_OSD_16BIT_AYUV_1555, /**< AYUV_1555              */
    AMP_OSD_16BIT_YUV_1555, /**< YUV_1555,  MSB ignored */
    AMP_OSD_16BIT_RGBA_5551, /**< RGBA_5551              */
    AMP_OSD_16BIT_BGRA_5551, /**< BGRA_5551              */
    AMP_OSD_16BIT_ABGR_1555, /**< ABGR_1555              */
    AMP_OSD_16BIT_ARGB_1555, /**< ARGB_1555              */

    /* 32-bit direct mode */
    AMP_OSD_32BIT_AYUV_8888 = 27, /**< AYUV_8888 */
    AMP_OSD_32BIT_RGBA_8888, /**< RGBA_8888 */
    AMP_OSD_32BIT_BGRA_8888, /**< BGRA_8888 */
    AMP_OSD_32BIT_ABGR_8888, /**< ABGR_8888 */
    AMP_OSD_32BIT_ARGB_8888 /**< ARGB_8888 */
} AMP_DISP_OSD_FORMAT_e;

/** The osd HW_Rescaler list */
typedef enum _AMP_DISP_OSD_HW_RESCALER_TYPE_e_ {
    HW_OSD_RESCALER_NONE = 0, /**< no scale */
    HW_OSD_RESCALER_INT, /**< scale to N times */
    HW_OSD_RESCALER_ANY, /**< scale to any size (aligned)*/
    HW_OSD_RESCALER_UP /**< only scale up */
} AMP_DISP_OSD_HW_RESCALER_TYPE_e;

/** For 8 bit color clut config */
typedef struct _AMP_OSD_CLUT_CFG_s_ {
    UINT8 *Clut; /**< color look up table in format AYUV8888. It must be a point point to u8[4*256].*/
} AMP_OSD_CLUT_CFG_s;

/** Osd buffer config */
typedef struct _AMP_OSD_BUFFER_CFG_s {
    AMP_DISP_OSD_FORMAT_e PixelFormat; /**< osd buffer color format */
    UINT32 BufWidth; /**< osd buffer width */
    UINT32 BufHeight; /**< osd buffer height */
    UINT32 BufPitch; /**< osd buffer pitch */
    UINT8 *BufAddr; /**< Present buffer */
} AMP_OSD_BUFFER_CFG_s;

/** Transparent color config */
typedef struct _AMP_OSD_TRANSPARENT_CFG_s_ {
    UINT8 Enable; /**< enable transparent */
    UINT16 Color; /**< Only one transparent color */
} AMP_OSD_TRANSPARENT_CFG_s;

#define HW_OSD_DEFAULT_GBLEND               0xFF
/** Osd totally structure config */
typedef struct _AMP_OSD_CFG_s_ {
    AMP_OSD_BUFFER_CFG_s BufCfg; /**< Osd buffer config */
    AMP_OSD_CLUT_CFG_s ColorLookupTable; /**< color look up table address */
    AMP_OSD_TRANSPARENT_CFG_s TransparentColor; /**< Transparent color config */
    AMP_DISP_OSD_HW_RESCALER_TYPE_e HwScalerType; /**< What kind of HW scaler */
    UINT8 OsdBufRepeatField; /**< Repeat osd buffer for top field and bottom field in interlaced mode */
    UINT8 GlobalBlend; /**< Decide the all osd transparent color */
} AMP_OSD_CFG_s;

/** Osd handler for user */
typedef struct _AMP_OSD_HDLR_s_ {
    void *Ctx; /**< Pointer to codec context */
} AMP_OSD_HDLR_s;

/** Osd mini module intial parameters */
typedef struct _AMP_OSD_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr; /**< Buffer start address for osd module to create osd hanlder array */
    UINT32 MemoryPoolSize; /**< Size of Buffer */
    UINT32 MaxOsdInterHdlr; /**< The number of osd handler user want */
} AMP_OSD_INIT_CFG_s;

/** Osd active handler info */
typedef struct _AMP_OSD_HDLR_INFO_s_ {
    AMP_OSD_HDLR_s *OsdHdlr; /**< Active osd handler instance */
    AMP_DISP_WINDOW_HDLR_s *WindowHdlr; /**< Active window handler instance */
} AMP_OSD_HDLR_INFO_s;

/**
 * Get default intial config to initial osd mini module.
 *
 * Give some default value to user reference.
 *
 * @param[in] Cfg - Initial config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetDefaultInitCfg(AMP_OSD_INIT_CFG_s *cfg);

/**
 * Intial osd mini module.
 *
 * Arrange osd handler memory and inital other flags.
 *
 * @param[in] Cfg - Initial config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_Init(AMP_OSD_INIT_CFG_s *cfg);

/**
 * Get default config to create osd handler.
 *
 * Give some default vaule to user reference.
 *
 * @param[out] Cfg - Osd handler config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetDefaultCfg(AMP_OSD_CFG_s *cfg);

/**
 * Create a osd handler.
 *
 * User need register a osd handler to hanlde their osd related.
 *
 * @param[in] Cfg - Osd handler config parameters
 * @param[out] OsdHdlr - Osd handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_Create(AMP_OSD_CFG_s *cfg,
                         AMP_OSD_HDLR_s **osdHdlr);

/**
 * Delete a osd handler.
 *
 * Clean the osd handler info and check out the handler.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_Delete(AMP_OSD_HDLR_s *osdHdlr);

/**
 * Set/Update osd info to a osd handler.
 *
 * Update the osd info in osd handler, and would take effect immediately if osd active.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_SetCfg(AMP_OSD_HDLR_s *osdHdlr,
                         AMP_OSD_CFG_s *cfg);

/**
 * Get osd info of the osd handler.
 *
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[out] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetCfg(AMP_OSD_HDLR_s *osdHdlr,
                         AMP_OSD_CFG_s *cfg);

/**
 * Set/Update osd bufferi nfo to a osd handler.
 *
 * Update the osd buffer info in osd handler, and would take effect immediately if osd active.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_SetBufferCfg(AMP_OSD_HDLR_s *osdHdlr,
                               AMP_OSD_BUFFER_CFG_s *cfg);

/**
 * Get osd buffer info of the osd handler.
 *
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[out] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetBufferCfg(AMP_OSD_HDLR_s *osdHdlr,
                               AMP_OSD_BUFFER_CFG_s *cfg);

/**
 * Set/Update osd clut info to a osd handler.
 *
 * Update the clut info in osd handler, and would take effect immediately if osd active.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_SetClutCfg(AMP_OSD_HDLR_s *osdHdlr,
                             AMP_OSD_CLUT_CFG_s *cfg);

/**
 * Get osd clut info of the osd handler.
 *
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[out] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetClutCfg(AMP_OSD_HDLR_s *osdHdlr,
                             AMP_OSD_CLUT_CFG_s *cfg);

/**
 * Set/Update osd transparent info to a osd handler.
 *
 * Update the osd transparent info in osd handler, and would take effect immediately if osd active.
 * Only could set one color.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_SetTransparentCfg(AMP_OSD_HDLR_s *osdHdlr,
                                    AMP_OSD_TRANSPARENT_CFG_s *cfg);

/**
 * Get osd transparant info of the sod handler.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[out] Cfg - Osd config parameters
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetTransparentCfg(AMP_OSD_HDLR_s *osdHdlr,
                                    AMP_OSD_TRANSPARENT_CFG_s *cfg);

/**
 * Acitive osd handler, show osd on corresponding window on device.
 *
 * For now, one device only can have one osd window.
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] WindowHdlr - Window handler instance for user to show osd.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_ActivateOsdWindow(AMP_OSD_HDLR_s *osdHdlr,
                                    AMP_DISP_WINDOW_HDLR_s *windowHdlr);

/**
 * Deacitive osd handler, disable osd on corresponding window on device.
 *
 *
 * @param[in] OsdHdlr - Osd handler instance for user
 * @param[in] WindowHdlr - Window handler instance for user to disable osd.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_DeActivateOsdWindow(AMP_OSD_HDLR_s *osdHdlr,
                                      AMP_DISP_WINDOW_HDLR_s *windowHdlr);

/**
 * Get osd active handler system info.
 *
 * @param[in] Channel - The channel that osd show on
 * @param[out] OutputInfo - return osd active handler and window handler
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpOsd_GetHdlrInfo(int channel,
                              AMP_OSD_HDLR_INFO_s *outputInfo);

/**
 * @}
 */

#endif /* OSD_H_ */
