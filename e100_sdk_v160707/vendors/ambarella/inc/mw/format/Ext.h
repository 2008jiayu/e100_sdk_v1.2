/**
 *  @file Ext.h
 *
 *  Common definitions for External format
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifndef __EXT_H__
#define __EXT_H__

/**
 * @defgroup EXT
 * @ingroup Format
 * @brief External Muxing/Demuxing Format implementation
 *
 * Implementing the External Muxing/Demuxing Format module.
 * 1. ExtMux just writes the raw frames to the stream by passing the frame information,
 *    and users can add their container format to the media.
 * 2. ExtDmx just asks the stream to feed the specified frames to FIFO via stream commands,
 *    so users must parse the media data from their custmoized container format.
 *
 * The ExtMux/ExtDmx module includes the following functions:
 * 1. Initiate ExtMux/ExtDmx module
 * 2. Create ExtMux/ExtDmx handler
 * 3. Delete ExtMux/ExtDmx handler
 */

/**
 * External format private information
 */
typedef struct {
    UINT8 Resv[4];      /**< Reserve */
} AMP_EXT_PRIV_INFO_s;

#endif

