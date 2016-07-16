/**
 * @file system/include/peripheral/gps_dev_list.h
 *
 * List of GPS device drivers
 *
 * History:
 *    2009/11/19 - [Jack Huang] created the file
 *    2015/04/13 - [Bill Chou] porting to a12
 *
 * Copyright (C) 2009-2010, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __GPS_DEV_LIST_H__
#define __GPS_DEV_LIST_H__

__BEGIN_C_PROTO__

//#ifdef __GPS_DEV_IMPL__
#include <gps.h>
/*
#ifdef  CONFIG_GPS_PROGIN_SR92
extern GPS_HANDLE gps_drv_init_progin(void);
#endif
*/
#ifdef  CONFIG_GPS_UBLOX6_GM6XX
extern GPS_HANDLE gps_drv_init_ublox(void);
#endif

#ifdef CONFIG_ECL_RADAR_MODEL
extern  GPS_HANDLE radar_drv_init_ublox(void);
#endif
/*
#ifdef  CONFIG_GPS_GOTOP_GAM_2222_MTR
extern GPS_HANDLE gps_drv_init_gotop(void);
#endif
*/
//#endif /*   __GPS_DEV_IMPL__ */

__END_C_PROTO__

#endif /* __GPS_DEV_LLIST_H__*/
