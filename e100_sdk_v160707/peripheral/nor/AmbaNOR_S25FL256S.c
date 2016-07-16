/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNAND_ S25FL128.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND device Info
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaNOR_Def.h"

const AMBA_NOR_DEV_INFO_s AmbaNOR_DevInfo = {
    "Spansion S25FL128_256S_PG2K",    /* NOR_NAME */

    /*--- DevInfo ---*/
    0x1,   /* Manufactuer ID  */
    0x18,     /* Devic ID */
    32 * 1024 * 1024 ,     /* NOR_TOTAL_BYTE_SIZE */
    512,   /* NOR_PROGRAM_PAGE_SIZE */
    256 * 1024,   /* NOR_BLOCKS_ERASE_SIZE */
    70000000,      /* NOR SPI FREQUENCY_Mhz */
    32,      /* NOR_EXT_ADDR_BITS */
    0,      /* NOR_LSBFirst */

    /*--- DevCMDInfo ---*/
    0xBB,   /* NOR DDRIO READ CMD*/
    0x2,    /* NOR Program Page CMD*/
    0x1,    /* NOR Write Regster CMD*/
    0x90,    /* NOR ReadID CMD*/
    0x0,     /* NOR Enable Reset */
    0xF0,    /* NOR Reset CMD*/
    0x5,    /* NOR Read Status0 CMD */
    0x7,    /* NOR Read Status1 CMD */
    0x30,   /* NOR Clear Status CMD */
    0x6,    /* NOR Write Enable CMD*/
    0x4,    /* NOR Write Disable CMD*/
    0xD8,   /* NOR Erase BLOCK CMD*/
    0x60,   /* NOR Erase Chip CMD*/
    0x4    /* NOR READ Dummy Cycles*/
};
