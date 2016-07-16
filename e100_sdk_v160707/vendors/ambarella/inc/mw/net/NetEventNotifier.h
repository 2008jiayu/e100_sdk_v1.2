/**
 * @file NetEventNotifier.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AMP_NETEVENTNOTIFIER_H_
#define AMP_NETEVENTNOTIFIER_H_

#include <mw.h>

/**
 * @defgroup NETEVENTNOTIFIER
 * @brief RTOS MW Event Notifier module
 *
 * NetEventNotifier is used to exchange event between RTOS and Linux user space programs.\n
 * With NetEventNotifier, Linux program could send/get event from/to RTOS.\n
 *
 */

/**
 * @addtogroup NETEVENTNOTIFIER
 * @{
 */

/**
 * Event Data Block
 */
typedef struct _AMP_NETEVENTNOTIFIER_DATABLK_s_ {
    UINT32 event; /**< event type */
    UINT32 param_len;  /**< valid length of extra data */
    UINT8 *param;  /**< extra data for event */
} AMP_NETEVENTNOTIFIER_DATABLK_s;

/**
 * NetEventNotifier default init config
 */
typedef struct _AMP_NETEVENTNOTIFIER_INIT_CFG_s_ {
    AMP_CALLBACK_f cbEvent;
} AMP_NETEVENTNOTIFIER_INIT_CFG_s;

/**
 * to get neteventnotifier package version info
 *
 * @return Version Number of NetEventNotifier Package
 */
extern int AmpNetEventNotifier_GetVer(void);

/**
 * Get default configuration for init.
 *
 * @param [out] defaultCfg the init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_NETEVENTNOTIFIER_INIT_CFG_s
 */
extern int AmpNetEventNotifier_GetInitDefaultCfg(AMP_NETEVENTNOTIFIER_INIT_CFG_s *defaultCfg);

/**
 * module init function
 *
 * @param [in] InitCfg the init config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_NETEVENTNOTIFIER_INIT_CFG_s
 */
extern int AmpNetEventNotifier_init(AMP_NETEVENTNOTIFIER_INIT_CFG_s *InitCfg);

/**
 * Notify Linux APP.
 *
 * @param [in] notify Type for notification. (see AMP_NETFIFO_NOTIFY_TYPE_e)
 * @param [in] param extra info for the notification.
 * @param [in] param_len valid length of extra info. Pass 0 if there is no need.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetEventNotifier_SendNotify(UINT32 event, UINT8 *param, UINT32 param_len);

/**
 * Register Callback for Linux APP event.
 *
 * The parameters for the call back are specified as:\n
 * hdlr: not used in this callback, will be set as NULL.\n
 * event: the event id.\n
 * info: extra info for event. It will be pass as AMP_NETEVENTNOTIFIER_DATABLK_s.\n
 *
 * callback should return as follow:\n
 * return 0 - OK, others - AMP_ER_CODE_e\n
 *
 * @param [in] cbEvent the callback function for Event.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetEventNotifier_RegEventCb(AMP_CALLBACK_f cbEvent);

/**
 * @}
 */

#endif /* AMP_NETEVENTNOTIFIER_H_ */
