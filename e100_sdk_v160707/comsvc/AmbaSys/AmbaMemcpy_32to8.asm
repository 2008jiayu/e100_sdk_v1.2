/*-----------------------------------------------------------------------------------------------
*  @FileName       :: AmbaMemcpy_32to8.asm
*
*  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
*
*                     No part of this file may be reproduced, stored in a retrieval system,
*                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
*                     recording, or otherwise, without the prior consent of Ambarella Corporation.
*
*  @Description    :: Copy 4 byte aligneed buffer to unaligned buffer.
*
*-----------------------------------------------------------------------------------------------
*/

/*-----------------------------------------------------------------------------
 *Note:
 *  To call this assembly from a C routine, use the following
 *  syntax:
 *
 *    extern void AmbaMemcpy_32to8(unsigned long* src,  // r0 register  
 *                     unsigned char* dst,  // r1 register  
 *                     unsigned int nbytes);    // r2 register  
 *    void main(void)
 *    {
 *        unsigned long* src;       // aligned buffer 
 *        unsigned char* dst;       // unaligned    buffer 
 *        unsigned short nbytes = 512;  // Number of bytes to MOVe, should be multiple of 32 byte 
 *
 *        AmbaMemcpy_32to8((unsigned long*) src, (unsigned char*) dst, nbytes);
 *    }
 *-----------------------------------------------------------------------------
 */

.text
.global AmbaMemcpy_32to8
 
AmbaMemcpy_32to8:
    MOVS    r2, r2, lsr #5          /* number of times to do 8*32 bits loop */
    STMFD   sp!, {r4-r11}           /* save the working registers */

Octcopy:
    LDMIA   r0!, {r4-r11}           /* load 8x32bit words to registers */

    STRB    r4, [r1], #1            /* store 1st byte to dst            */
    MOV     r4, r4, lsr #8          /* shift 2nd byte to lower byte     */
    STRB    r4, [r1], #1            /* store 2nd byte to dst            */    
    MOV     r4, r4, lsr #8          /* shift 3rd byte to lower half word*/
    STRB    r4, [r1], #1            /* store 3rd byte to dst            */ 
    MOV     r4, r4, lsr #8          /* shift 4th byte to lower half word*/
    STRB    r4, [r1], #1            /* store 4th byte to dst            */
                                       
    STRB    r5, [r1], #1             /*store 1st byte to dst            */    
    MOV     r5, r5, lsr #8           /*shift 2nd byte to lower byte     */
    STRB    r5, [r1], #1             /*store 2nd byte to dst            */
    MOV     r5, r5, lsr #8           /*shift 3rd byte to lower half word*/
    STRB    r5, [r1], #1             /*store 3rd byte to dst            */
    MOV     r5, r5, lsr #8           /*shift 4th byte to lower half word*/
    STRB    r5, [r1], #1             /*store 4th byte to dst            */

    STRB    r6, [r1], #1             /*store 1st byte to dst            */ 
    MOV     r6, r6, lsr #8           /*shift 2nd byte to lower byte     */
    STRB    r6, [r1], #1             /*store 2nd byte to dst            */
    MOV     r6, r6, lsr #8           /*shift 3rd byte to lower half word*/
    STRB    r6, [r1], #1             /*store 3rd byte to dst            */
    MOV     r6, r6, lsr #8           /*shift 4th byte to lower half word*/
    STRB    r6, [r1], #1             /*store 4th byte to dst            */

    STRB    r7, [r1], #1             /*store 1st byte to dst            */           
    MOV     r7, r7, lsr #8           /*shift 2nd byte to lower byte     */
    STRB    r7, [r1], #1             /*store 2nd byte to dst            */
    MOV     r7, r7, lsr #8           /*shift 3rd byte to lower half word*/
    STRB    r7, [r1], #1             /*store 3rd byte to dst            */
    MOV     r7, r7, lsr #8           /*shift 4th byte to lower half word*/
    STRB    r7, [r1], #1             /*store 4th byte to dst            */

    STRB    r8, [r1], #1             /*store 1st byte to dst             */  
    MOV     r8, r8, lsr #8           /*shift 2nd byte to lower byte      */
    STRB    r8, [r1], #1             /*store 2nd byte to dst             */
    MOV     r8, r8, lsr #8           /*shift 3rd byte to lower half word */
    STRB    r8, [r1], #1             /*store 3rd byte to dst             */
    MOV     r8, r8, lsr #8           /*shift 4th byte to lower half word */
    STRB    r8, [r1], #1             /*store 4th byte to dst             */

    STRB    r9, [r1], #1             /*store 1st byte to dst             */ 
    MOV     r9, r9, lsr #8           /*shift 2nd byte to lower byte      */
    STRB    r9, [r1], #1             /*store 2nd byte to dst             */
    MOV     r9, r9, lsr #8           /*shift 3rd byte to lower half word */
    STRB    r9, [r1], #1             /*store 3rd byte to dst             */
    MOV     r9, r9, lsr #8           /*shift 4th byte to lower half word */
    STRB    r9, [r1], #1             /*store 4th byte to dst             */

    STRB    r10, [r1], #1            /*store 1st byte to dst            */       
    MOV     r10, r10, lsr #8         /*shift 2nd byte to lower byte     */
    STRB    r10, [r1], #1            /*store 2nd byte to dst            */
    MOV     r10, r10, lsr #8         /*shift 3rd byte to lower half word*/
    STRB    r10, [r1], #1            /*store 3rd byte to dst            */
    MOV     r10, r10, lsr #8         /*shift 4th byte to lower half word*/
    STRB    r10, [r1], #1            /*store 4th byte to dst            */

    STRB    r11, [r1], #1            /*store 1st byte to dst             */     
    MOV     r11, r11, lsr #8         /*shift 2nd byte to lower byte      */
    STRB    r11, [r1], #1            /*store 2nd byte to dst             */
    MOV     r11, r11, lsr #8         /*shift 3rd byte to lower half word */
    STRB    r11, [r1], #1            /*store 3rd byte to dst             */
    MOV     r11, r11, lsr #8         /*shift 4th byte to lower half word */
    STRB    r11, [r1], #1            /*store 4th byte to dst             */

    SUBS    r2, r2, #1              /* decrement the counter */
    BNE     Octcopy                 /* copy 32 bytes more */ 
    LDMFD   sp!, {r4-r11}           /* restore original registers */

Stop:
    BX      lr
