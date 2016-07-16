/**
 * @file NetUtility.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AMP_NETUTILITY_H_
#define AMP_NETUTILITY_H_

#include <mw.h>

/**
 * @defgroup NETUTILITY
 * @brief LINUX User psace Utilities
 *
 * NetUtility is to provide Linux user space utilities.\n
 * With NetUtility, RTOS could control Linux more easily.\n
 *
 */

/**
 * @addtogroup NETUTILITY
 * @{
 */

/**
 * Execte Linux user shell command without response.
 *
 * @param [in] sh_cmd shell command to be executed.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetUtility_luExecNoResponse(char *sh_cmd);

/**
 * Execte Linux user shell command without response.
 *
 * @param [in] sh_cmd shell command to be executed.
 * @param [out] Response response buffer for shell command.
 * @param [in] ResponseMaxLen maximal length of response buffer.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpNetUtility_luExec(char *sh_cmd, char *Response, int ResponseMaxLen);

/**
 * @}
 */

#endif /* AMP_NETUTILITY_H_ */
