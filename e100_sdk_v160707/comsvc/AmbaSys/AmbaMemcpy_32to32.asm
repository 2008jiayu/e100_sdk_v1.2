/*-----------------------------------------------------------------------------------------------
*  @FileName       :: AmbaMemcpy_32to32.asm
*
*  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
*
*                     No part of this file may be reproduced, stored in a retrieval system,
*                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
*                     recording, or otherwise, without the prior consent of Ambarella Corporation.
*
*  @Description    :: Copy 4 byte aligned buffer to 4 byte aligned buffer.
*-----------------------------------------------------------------------------------------------
*/
/*------------------------------------------------------------------------------
*  Note:
*    To call this assembly from a C routine, use the following
*    syntax:
*
*      extern void AmbaMemcpy_32to32(unsigned long* dst, // r0 register  
*                        unsigned long* src, // r1 register  
*                        unsigned int nbytes);   // r2 register  
*      void main(void)
*      {
*          unsigned long* dst;    // aligned buffer 
*          unsigned long* src;    // unaligned buffer 
*          unsigned int nbytes = 512; // Number of bytes to move, should be multiple of 32 byte 
* 
*          AmbaMemcpy_32to32((unsigned long*) dst, (unsigned long*) src, nbytes);
*      }
* ------------------------------------------------------------------------------
*/    
.text
.global AmbaMemcpy_32to32 

AmbaMemcpy_32to32:
    MOVS    r2, r2, lsr #5      /* number of times to do 8*32 bits loop */
    STMFD   sp!, {r4-r11}       /* save the working registers */

Octcopy:
    LDMIA   r1!, {r4-r11}       /* load 8x32bit words to registers */
    STMIA   r0!, {r4-r11}       /* store 8x32bit words to aligned buffer */

    SUBS    r2, r2, #1          /* decrement the counter */
    BNE     Octcopy             /* copy 32 bytes more */
    LDMFD   sp!, {r4-r11}       /* restore original registers */

Stop:
    BX      lr
