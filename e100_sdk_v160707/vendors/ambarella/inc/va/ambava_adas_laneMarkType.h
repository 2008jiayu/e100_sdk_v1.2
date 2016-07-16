/**
 * rtos\vendors\ambarella\inc\va\ambava_adas_laneMarkType.h
 *
 * Header of AMBA ADAS interface.
 *
 * Copyright (C) 2004-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _AMBAVA_ADAS_LANE_MARK_TYPE_H_
#define _AMBAVA_ADAS_LANE_MARK_TYPE_H_

#define AMBAVA_ADAS_NUM_PATTERN_TYPES 5

typedef enum
{
    AMBA_ADAS_LTUNKNOWN = 0,
    AMBA_ADAS_LTSINGLE = 1,
    AMBA_ADAS_LTDOUBLE = 2,
    AMBA_ADAS_LTROADSIDELEFT = 3,
    AMBA_ADAS_LTROADSIDERIGHT = 4
} AMBA_ADAS_LINETYPE;

typedef enum _AMBA_ADAS_LINESOLIDITY_
{
    AMBA_ADAS_LSUNKNOWN = 0,
    AMBA_ADAS_LSSOLID = 1,
    AMBA_ADAS_LSDASHED = 2
} AMBA_ADAS_LINESOLIDITY;

typedef enum _AMBA_ADAS_LINECOLOR_
{
    AMBA_ADAS_LCUNKNOWN = 0,
    AMBA_ADAS_LCWHITE = 1,
    AMBA_ADAS_LCYELLOW = 2
} AMBA_ADAS_LINECOLOR;

/** Type of road lines */
typedef struct _AMBA_ADAS_LANEMARKTYPE_
{
    AMBA_ADAS_LINETYPE Type;
    AMBA_ADAS_LINESOLIDITY Solidity;
    AMBA_ADAS_LINECOLOR Color;
} AMBA_ADAS_LANEMARKTYPE;


#endif

