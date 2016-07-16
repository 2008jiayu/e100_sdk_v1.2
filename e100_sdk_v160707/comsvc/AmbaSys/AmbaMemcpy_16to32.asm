/**-----------------------------------------------------------------------------------------------
  @FileName       :: AmbaMemcpy_16to32.asm

  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.

                     No part of this file may be reproduced, stored in a retrieval system,
                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
                     recording, or otherwise, without the prior consent of Ambarella Corporation.

  @Description    :: Copy 2 byte aligned buffer to 4 byte aligned buffer.
-----------------------------------------------------------------------------------------------
 */

.text
.global AmbaMemcpy_16to32 
AmbaMemcpy_16to32:
    MOVS    r2, r2, lsr #5          /* number of times to do 8*32 bits loop */
    STMFD   sp!, {r4-r11}           /* save the working registers */

Octcopy:
    LDRH    r4, [r1], #2            /* store lower half word to r4 */
    LDRH    r3, [r1], #2            /* store upper half word to r3 */
    ORR     r4, r4, r3, lsl #16     /* shift upper half word to lower half word */
 
    LDRH    r5, [r1], #2            /* Repeat for the rest of the data */
    LDRH    r3, [r1], #2
    ORR     r5, r5, r3, lsl #16

    LDRH    r6, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r6, r6, r3, lsl #16

    LDRH    r7, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r7, r7, r3, lsl #16

    LDRH    r8, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r8, r8, r3, lsl #16

    LDRH    r9, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r9, r9, r3, lsl #16

    LDRH    r10, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r10, r10, r3, lsl #16

    LDRH    r11, [r1], #2
    LDRH    r3, [r1], #2
    ORR     r11, r11, r3, lsl #16

    STMIA   r0!, {r4-r11}           /* store 8x32bit words to aligned buffer */

    SUBS    r2, r2, #1              /* decrement the counter */
    BNE     Octcopy                /* copy 32 bytes more */
    LDMFD   sp!, {r4-r11}           /* restore original registers */

Stop:
    BX      lr
