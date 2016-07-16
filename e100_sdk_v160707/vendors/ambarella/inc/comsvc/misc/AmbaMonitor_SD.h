/**
 * @file AmbaMonitor_SD.h
 *
 * Amba sd card status monitor
 *
 * History:
 *    2014/12/11 - [cyweng] created file
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef AMBAMONITOR_SD_H_
#define AMBAMONITOR_SD_H_

/**
 * SD monitor init function
 *
 * @param Priority [in] - monitor task priority
 * @param CoreSelection [in] - core option for smp, Bit[0] = 1 means core #0 is selected
 * @return
 */
extern int AmbaMonitor_SDInit(UINT32 Priority,
                              UINT32 CoreSelection);

#endif /* AMBAMONITOR_SD_H_ */
