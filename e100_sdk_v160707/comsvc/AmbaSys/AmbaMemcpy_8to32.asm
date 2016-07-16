/*-----------------------------------------------------------------------------------------------
*  @FileName       :: AmbaMemcpy_8to32.asm
*
*  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
*
*                     No part of this file may be reproduced, stored in a retrieval system,
*                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
*                     recording, or otherwise, without the prior consent of Ambarella Corporation.
*
*  @Description    :: Copy unaligned buffer to 4 byte aligned buffer.
*-----------------------------------------------------------------------------------------------
*/

/*-----------------------------------------------------------------------------
 * Note:
 *   To call this assembly from a C routine, use the following
 *   syntax:
 *
 *     extern void AmbaMemcpy_8to32(unsigned long* dst,  /* r0 register  
 *                      unsigned char* src,  // r1 register  
 *                      unsigned intnbytes);     // r2 register  
 *     void main(void)
 *     {
 *         unsigned long* dst;       // aligned buffer 
 *         unsigned char* src;       // unaligned buffer 
 *         unsigned short nbytes = 512;  // Number of bytes to MOVe, should be multiple of 32 byte 
 *
 *        AmbaMemcpy_8to32((unsigned long*) dst, (unsigned char*) src, nbytes);
 *    }
 *-----------------------------------------------------------------------------
 */   

 .text
 .global AmbaMemcpy_8to32

AmbaMemcpy_8to32 :
    MOVS    r2, r2, lsr #5           /*number of times to do 8*32 bits loop */
    STMFD   sp!, {r4-r11}           /* save the working registers */

Octcopy :
    MOV     r4, #0                   /* initialize r4 to 0       */  
    LDRB    r3, [r1], #1             /* store 1st byte to r3     */
    ORR     r4, r4, r3               /* store 1st byte to r4     */
    LDRB    r3, [r1], #1             /* store 2nd byte to r3     */
    ORR     r4, r4, r3, lsl #8       /* store 1,2 byte to r4     */
    LDRB    r3, [r1], #1             /* store 3rd byte to r3     */
    ORR     r4, r4, r3, lsl #16      /* store 1,2,3 byte to r3   */
    LDRB    r3, [r1], #1             /* store 4th byte to r3     */
    ORR     r4, r4, r3, lsl #24      /* store 1,2,3,4 byte to r4 */

    MOV     r5, #0                  /* initialize r5 to 0      */ 
    LDRB    r3, [r1], #1            /* store 1st byte to r3    */
    ORR     r5, r5, r3              /* store 1st byte to r5    */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3    */
    ORR     r5, r5, r3, lsl #8      /* store 1,2 byte to r5    */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3    */
    ORR     r5, r5, r3, lsl #16     /* store 1,2,3 byte to r3  */
    LDRB    r3, [r1], #1            /* store 4th byte to r3    */
    ORR     r5, r5, r3, lsl #24     /* store 1,2,3,4 byte to r5*/

    MOV     r6, #0                  /* initialize r6 to 0       */ 
    LDRB    r3, [r1], #1            /* store 1st byte to r3     */
    ORR     r6, r6, r3              /* store 1st byte to r6     */  
    LDRB    r3, [r1], #1            /* store 2nd byte to r3     */
    ORR     r6, r6, r3, lsl #8      /* store 1,2 byte to r6     */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3     */
    ORR     r6, r6, r3, lsl #16     /* store 1,2,3 byte to r3   */
    LDRB    r3, [r1], #1            /* store 4th byte to r3     */
    ORR     r6, r6, r3, lsl #24     /* store 1,2,3,4 byte to r6 */

    MOV     r7, #0                  /* initialize r7 to 0      */ 
    LDRB    r3, [r1], #1            /* store 1st byte to r3    */
    ORR     r7, r7, r3              /* store 1st byte to r7    */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3    */
    ORR     r7, r7, r3, lsl #8      /* store 1,2 byte to r7    */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3    */
    ORR     r7, r7, r3, lsl #16     /* store 1,2,3 byte to r3  */
    LDRB    r3, [r1], #1            /* store 4th byte to r3    */
    ORR     r7, r7, r3, lsl #24     /* store 1,2,3,4 byte to r7*/

    MOV     r8, #0                  /* initialize r8 to 0      */  
    LDRB    r3, [r1], #1            /* store 1st byte to r3    */
    ORR     r8, r8, r3              /* store 1st byte to r8    */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3    */
    ORR     r8, r8, r3, lsl #8      /* store 1,2 byte to r8    */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3    */
    ORR     r8, r8, r3, lsl #16     /* store 1,2,3 byte to r3  */
    LDRB    r3, [r1], #1            /* store 4th byte to r3    */
    ORR     r8, r8, r3, lsl #24     /* store 1,2,3,4 byte to r8*/

    MOV     r9, #0                  /* initialize r9 to 0      */  
    LDRB    r3, [r1], #1            /* store 1st byte to r3    */
    ORR     r9, r9, r3              /* store 1st byte to r9    */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3    */
    ORR     r9, r9, r3, lsl #8      /* store 1,2 byte to r9    */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3    */
    ORR     r9, r9, r3, lsl #16     /* store 1,2,3 byte to r3  */
    LDRB    r3, [r1], #1            /* store 4th byte to r3    */
    ORR     r9, r9, r3, lsl #24     /* store 1,2,3,4 byte to r9*/

    MOV     r10, #0                 /* initialize r10 to 0      */ 
    LDRB    r3, [r1], #1            /* store 1st byte to r3     */
    ORR     r10, r10, r3            /* store 1st byte to r10    */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3     */
    ORR     r10, r10, r3, lsl #8    /* store 1,2 byte to r10    */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3     */
    ORR     r10, r10, r3, lsl #16   /* store 1,2,3 byte to r3   */
    LDRB    r3, [r1], #1            /* store 4th byte to r3     */
    ORR     r10, r10, r3, lsl #24   /* store 1,2,3,4 byte to r10*/

    MOV     r11, #0                 /* initialize r11 to 0     */  
    LDRB    r3, [r1], #1            /* store 1st byte to r3    */
    ORR     r11, r11, r3            /* store 1st byte to r11   */
    LDRB    r3, [r1], #1            /* store 2nd byte to r3    */
    ORR     r11, r11, r3, lsl #8    /* store 1,2 byte to r11   */
    LDRB    r3, [r1], #1            /* store 3rd byte to r3    */
    ORR     r11, r11, r3, lsl #16   /* store 1,2,3 byte to r3  */
    LDRB    r3, [r1], #1            /* store 4th byte to r3    */
    ORR     r11, r11, r3, lsl #24   /* store 1,2,3,4 byte to r11*/

    STMIA   r0!, {r4-r11}           /*store 8x32bit words to aligned buffer */

    SUBS    r2, r2, #1              /* decrement the counter */
    BNE     Octcopy                 /* copy 32 bytes more */
    LDMFD   sp!, {r4-r11}           /* restore original registers */

Stop:
    BX      lr
