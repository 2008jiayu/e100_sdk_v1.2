/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaLCD.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for LCD driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaLCD.h"

extern AMBA_LCD_OBJECT_s AmbaLCD_WdF9648wObj;
extern AMBA_LCD_OBJECT_s AmbaLCD_T20P52Obj;
extern AMBA_LCD_OBJECT_s AmbaLCD_T27P05Obj;
extern AMBA_LCD_OBJECT_s AmbaLCD_SSD2828Obj;

AMBA_LCD_OBJECT_s *pAmbaLcdObj[] = {
    0,
};
