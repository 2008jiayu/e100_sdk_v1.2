/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaINT_Def.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for Vector Interrupt Controller (VIC) APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_INT_DEF_H_
#define _AMBA_INT_DEF_H_

/*-----------------------------------------------------------------------------------------------*\
 * Cortex-A9 VIC Interrupt IDs
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_VIC_INT_ID_e_ {
    /*-----------------------------------------------------------------------*\
     * VIC1[0:31]: ID0 - ID31
    \*-----------------------------------------------------------------------*/
    AMBA_VIC_INT_ID0_USB_VBUS_CONNECT = 0,
    AMBA_VIC_INT_ID1_VDSP_CODING_VOUT_TV,
    AMBA_VIC_INT_ID2_VDSP_CODING_VIN,
    AMBA_VIC_INT_ID3_VDSP_CODING_0,
    AMBA_VIC_INT_ID4_USB,
    AMBA_VIC_INT_ID5_USB_CHARGE_DETECT,
    AMBA_VIC_INT_ID6_SD2_CARD_DETECT,
    AMBA_VIC_INT_ID7_I2S_TX,
    AMBA_VIC_INT_ID8_I2S_RX,
    AMBA_VIC_INT_ID9_UART0,
    AMBA_VIC_INT_ID10_GPIO0,
    AMBA_VIC_INT_ID11_GPIO1,
    AMBA_VIC_INT_ID12_TIMER0,
    AMBA_VIC_INT_ID13_TIMER1,
    AMBA_VIC_INT_ID14_TIMER2,
    AMBA_VIC_INT_ID15_DMA,
    AMBA_VIC_INT_ID16_FLASH_CMD_DONE,
    AMBA_VIC_INT_ID17_FLASH_DMA_DONE,
    AMBA_VIC_INT_ID18_SD0,
    AMBA_VIC_INT_ID19_I2C0,
    AMBA_VIC_INT_ID20_SD2,
    AMBA_VIC_INT_ID21_WDT,
    AMBA_VIC_INT_ID22_IR_REMOTE,
    AMBA_VIC_INT_ID23_SD1_CARD_DETECT,
    AMBA_VIC_INT_ID24_SD0_CARD_DETECT,
    AMBA_VIC_INT_ID25_UART1,
    AMBA_VIC_INT_ID26_MOTOR,
    AMBA_VIC_INT_ID27_ETHERNET,
    AMBA_VIC_INT_ID28_USB_VBUS_DETECT_STATUS_CHANGE,
    AMBA_VIC_INT_ID29_GPIO3,
    AMBA_VIC_INT_ID30_GPIO2,
    AMBA_VIC_INT_ID31,

    /*-----------------------------------------------------------------------*\
     * VIC2[0:31]: ID32 - ID63
    \*-----------------------------------------------------------------------*/
    AMBA_VIC_INT_ID32_ETHERNET_POWER,
    AMBA_VIC_INT_ID33_DMA_FIOS_CHANNEL0,
    AMBA_VIC_INT_ID34_ADC_LEVEL_CHANGE,
    AMBA_VIC_INT_ID35_SPI0,
    AMBA_VIC_INT_ID36_I2C2,
    AMBA_VIC_INT_ID37_SPI1,
    AMBA_VIC_INT_ID38_SPI_SLAVE,
    AMBA_VIC_INT_ID39_USB_EHCI,
    AMBA_VIC_INT_ID40_HDMI_TX,
    AMBA_VIC_INT_ID41_FIOS_ECC,
    AMBA_VIC_INT_ID42_VOUT_TV,
    AMBA_VIC_INT_ID43_VOUT_LCD,
    AMBA_VIC_INT_ID44_USB_OHCI,
    AMBA_VIC_INT_ID45_NOR_SPI,
    AMBA_VIC_INT_ID46_VDSP_CODING_VOUT_LCD,
    AMBA_VIC_INT_ID47,
    AMBA_VIC_INT_ID48,
    AMBA_VIC_INT_ID49,
    AMBA_VIC_INT_ID50_GDMA,
    AMBA_VIC_INT_ID51_I2C1,
    AMBA_VIC_INT_ID52_SD1,
    AMBA_VIC_INT_ID53_IDSP_PIP_VSYNC_SLAVE,
    AMBA_VIC_INT_ID54_IDSP_PIP_FRAME_START,
    AMBA_VIC_INT_ID55_IDSP_PIP_VSYNC_MASTER,
    AMBA_VIC_INT_ID56_IDSP_PIP_LAST_PIXEL,
    AMBA_VIC_INT_ID57_IDSP_PIP_DELAYED_VSYNC,
    AMBA_VIC_INT_ID58_MAIN_DSP_PIP,
    AMBA_VIC_INT_ID59_TIMER3,
    AMBA_VIC_INT_ID60_TIMER4,
    AMBA_VIC_INT_ID61_TIMER5,
    AMBA_VIC_INT_ID62_TIMER6,
    AMBA_VIC_INT_ID63_TIMER7,

    /*-----------------------------------------------------------------------*\
     * VIC3[0:31]: ID64 - ID95
    \*-----------------------------------------------------------------------*/
    AMBA_VIC_INT_ID64_IDSP_VIN_VSYNC_MASTER,
    AMBA_VIC_INT_ID65_IDSP_VIN_VSYNC_SLAVE,
    AMBA_VIC_INT_ID66_IDSP_VIN_FRAME_START,
    AMBA_VIC_INT_ID67_IDSP_VIN_DELAYED_VSYNC,
    AMBA_VIC_INT_ID68_IDSP_VIN_LAST_PIXEL,
    AMBA_VIC_INT_ID69_SOFT_IRQ0,
    AMBA_VIC_INT_ID70_SOFT_IRQ1,
    AMBA_VIC_INT_ID71_SOFT_IRQ2,
    AMBA_VIC_INT_ID72_SOFT_IRQ3,
    AMBA_VIC_INT_ID73_SOFT_IRQ4,
    AMBA_VIC_INT_ID74_SOFT_IRQ5,
    AMBA_VIC_INT_ID75_SOFT_IRQ6,
    AMBA_VIC_INT_ID76_SOFT_IRQ7,
    AMBA_VIC_INT_ID77_SOFT_IRQ8,
    AMBA_VIC_INT_ID78_SOFT_IRQ9,
    AMBA_VIC_INT_ID79_SOFT_IRQ10,
    AMBA_VIC_INT_ID80_SOFT_IRQ11,
    AMBA_VIC_INT_ID81_SOFT_IRQ12,
    AMBA_VIC_INT_ID82_SOFT_IRQ13,
    AMBA_VIC_INT_ID83_AXI_SOFT_IRQ0,        /* AXI software interrupt 0 */
    AMBA_VIC_INT_ID84_AXI_SOFT_IRQ1,        /* AXI software interrupt 1 */
    AMBA_VIC_INT_ID85_L2CC,                 /* Cortex-A9 L2CC Combined Interrupt Output */
    AMBA_VIC_INT_ID86_CRYPTO_MD5,
    AMBA_VIC_INT_ID87_CRYPTO_DES,
    AMBA_VIC_INT_ID88_CRYPTO_AES,
    AMBA_VIC_INT_ID89_CRYPTO_SHA1,
    AMBA_VIC_INT_ID90,
    AMBA_VIC_INT_ID91_USB_DIGITAL_ID_CHANGE,
    AMBA_VIC_INT_ID92_CORTEX_PMU,           /* Cortex-A9 Performance Monitor Unit */
    AMBA_VIC_INT_ID93_L2CC_DECERRINTR,      /* Cortex-A9 Decode error received on master ports from L3 */
    AMBA_VIC_INT_ID94_L2CC_SLVERRINTR,      /* Cortex-A9 Slave error received on master ports from L3 */
    AMBA_VIC_INT_ID95_L2CC_ECNTRINTR,       /* Cortex-A9 Event Counter Overflow/Increment */

    AMBA_NUM_VIC_INTERRUPT,                 /* Total number of Interrupts supported */
} AMBA_VIC_INT_ID_e;

typedef enum _AMBA_VIC_INT_CONFIG_e_ {
    AMBA_VIC_INT_HIGH_LEVEL_TRIGGER = 0,    /* Corresponding interrupt is High level sensitive */
    AMBA_VIC_INT_LOW_LEVEL_TRIGGER,         /* Corresponding interrupt is Low level sensitive */
    AMBA_VIC_INT_RISING_EDGE_TRIGGER,       /* Corresponding interupt is rising-edge-triggered */
    AMBA_VIC_INT_FALLING_EDGE_TRIGGER,      /* Corresponding interupt is falling-edge-triggered */
    AMBA_VIC_INT_BOTH_EDGE_TRIGGER          /* Corresponding interupt is both-edge-triggered */
} AMBA_VIC_INT_CONFIG_e;

typedef void (*AMBA_VIC_ISR_f)(int IntID);

#endif /* _AMBA_INT_DEF_H_ */
