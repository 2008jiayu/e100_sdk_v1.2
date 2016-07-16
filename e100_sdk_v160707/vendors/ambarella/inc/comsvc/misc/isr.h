 /**
  * @file inc/comsvc/isr.h
  *
  * ISR hook service header
  *
  * History:
  *    2013/03/02/ - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef __ISR_H__
#define __ISR_H__


/**
 * ISR hook
 *
 * @param [in] IrqNo IRQ number
 * @param [in] Func ISR
 *
 * @return 0 - success, -1 - fail
 */
extern int AmpIsr_Hook(UINT32 IrqNo, void (*Func)(int IrqNo));

/**
 * ISR Control
 *
 * @param [in] IrqNo IRQ number
 * @param [in] Enable Enable or disable
 * @param [in] Func ISR
 *
 * @return 0 - success 1 - fail
 */
extern int AmpIsr_Control(UINT32 IrqNo, UINT32 Enable, void (*Func)(int IrqNo));


/**
 * @}
 */

#endif

