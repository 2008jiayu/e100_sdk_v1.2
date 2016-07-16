/**
 * @file src/app/connected/applib/inc/system/ApplibSys_Lens.h
 *
 * Header of Lens interface.
 *
 * History:
 *    2014/02/14 - [Martin Lai] created file
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
#ifndef APP_LENS_H_
#define APP_LENS_H_
/**
* @defgroup ApplibSys_Lens
* @brief Lens interface.
*
*
*/

/**
 * @addtogroup ApplibSys_Lens
 * @ingroup System
 * @{
 */
#include <mw.h>
#include <applib.h>

__BEGIN_C_PROTO__

/** Focus mode config id */
#define VIN_FMOD_MF    (0)      /**<VIN_FMOD_MF    */
#define VIN_FMOD_NORMAL    (1)  /**<VIN_FMOD_NORMAL*/
#define VIN_FMOD_MACRO    (2)   /**<VIN_FMOD_MACRO */
#define VIN_FMOD_LOCK    (3)    /**<VIN_FMOD_LOCK  */

#define LENS_OEM_TYPE       0   /**<LENS_OEM_TYPE   */
#define LENS_NONOEM_TYPE    1   /**<LENS_NONOEM_TYPE*/

/** Lens zoom definitions */
#define LENS_ZOOM_IN    (0)    /**<LENS_ZOOM_IN */
#define LENS_ZOOM_OUT    (1)   /**<LENS_ZOOM_OUT*/

/** Lens focus definitions */
#define LENS_FOCUS_ABSOLUTE    (0)  /**<LENS_FOCUS_ABSOLUTE*/
#define LENS_FOCUS_FAR         (1)  /**<LENS_FOCUS_FAR     */
#define LENS_FOCUS_NEAR        (2)  /**<LENS_FOCUS_NEAR    */

/*************************************************************************
 * Lens structure
 ************************************************************************/
/**
 *  This data structure describes the interface of a lens
 */
typedef struct _APPLIB_LENS_s_ {
    /** Module ID */
    UINT32 Id;
    UINT32 MeCapability; /**<Me Capability*/
    /** Module name */
    WCHAR Name[80];
    UINT32 ZoomCap; /**<Zoom Capacity*/
#define LENS_ZOOM_DISABLE   (0)    /**<LENS_ZOOM_DISABLE */
#define LENS_ZOOM_ENABLE    (1)    /**<LENS_ZOOM_ENABLE  */
    /** Module init interface */
    int (*Init)(void);
} APPLIB_LENS_s;

/*************************************************************************
 * Lens Internal APIs
 ************************************************************************/
/**
 *  Remove the Lens input device
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLens_Remove(void);

/**
 *  Attach the Lens input device and enable the device control.
 *
 *  @param [in] dev Device info
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLens_Attach(APPLIB_LENS_s *dev);

/*************************************************************************
 * Lens Public APIs
 ************************************************************************/

/**
 *  Clean Lens configuration
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLens_PreInit(void);

/**
 *  Initialize the lens
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibSysLens_Init(void);

/**
 *  @brief Check the capacity of zoom
 *
 *  Check the capacity of zoom
 *
 *  @return > 1 is supported, 0 is not supported
 */
extern int AppLibSysLens_CheckZoomCap(void);

/**
 *  Get lens' type
 *
 *  @return Lens' type
 */
extern int AppLibSysLens_GetType(void);

__END_C_PROTO__
/**
 * @}
 */
#endif /* APP_LENS_H_ */
