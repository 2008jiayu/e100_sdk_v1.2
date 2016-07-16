/*-----------------------------------------------------------------------------------------------
*  @FileName       :: AmbaMemcpy_32to16.asm
*
*  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
*
*                     No part of this file may be reproduced, stored in a retrieval system,
*                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
*                     recording, or otherwise, without the prior consent of Ambarella Corporation.
*
*  @Description    :: Copy 4 byte aligned buffer to 2 byte aligned buffer.
*
\*-----------------------------------------------------------------------------------------------
*/
/*-----------------------------------------------------------------------------
 * Note:
 *   To call this assembly from a C routine, use the following
 *   syntax:
 *
 *     extern void AmbaMemcpy_32to16(unsigned long* src, // r0 register
 *                       unsigned short* dst,    // r1 register
 *                       unsigned int nbytes);   // r2 register
 *     void main(void)
 *     {
 *         unsigned long* src;      // aligned buffer
 *         unsigned short* dst;     / unaligned buffer
 *         unsigned short nbytes = 512; // Number of bytes to move, should be multiple of 32 byte
 *
 *         AmbaMemcpy_32to16((unsigned long*) src, (unsigned short*) dst, nbytes);
 *     }
 *-----------------------------------------------------------------------------
 */

.text
.global AmbaMemcpy_32to16
AmbaMemcpy_32to16:
    MOVS    r2, r2, lsr #5      /* number of times to do 8*32 bits loop */
    STMFD   sp!, {r4-r11}       /* save the working registers */

Octcopy:
    LDMIA   r0!, {r4-r11}       /* load 8x32bit words to registers */

    STRH    r4, [r1], #2        /* store lower half word to dst */
    MOV     r4, r4, lsr #16     /* shift upper half word to lower half word */
    STRH    r4, [r1], #2        /* store upper half word to dst */

    STRH    r5, [r1], #2
    MOV     r5, r5, lsr #16
    STRH    r5, [r1], #2

    STRH    r6, [r1], #2
    MOV     r6, r6, lsr #16
    STRH    r6, [r1], #2

    STRH    r7, [r1], #2
    MOV     r7, r7, lsr #16
    STRH    r7, [r1], #2

    STRH    r8, [r1], #2
    MOV     r8, r8, lsr #16
    STRH    r8, [r1], #2

    STRH    r9, [r1], #2
    MOV     r9, r9, lsr #16
    STRH    r9, [r1], #2

    STRH    r10, [r1], #2
    MOV     r10, r10, lsr #16
    STRH    r10, [r1], #2

    STRH    r11, [r1], #2
    MOV     r11, r11, lsr #16
    STRH    r11, [r1], #2

    SUBS    r2, r2, #1          /* decrement the counter */
    BNE     Octcopy             /* copy 32 bytes more */
    LDMFD   sp!, {r4-r11}       /* restore original registers */

Stop:
    BX      lr
