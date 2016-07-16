/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCortexA9.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Cortex-A9 inside A12
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CORTEX_A9_H_
#define _AMBA_CORTEX_A9_H_

#define AMBA_CORTEX_A9_DRAM_BASE_ADDR               0x00000000      /* External DRAM Base Address 1GB */
#define AMBA_CORTEX_A9_DRAMC_PHY_BASE_ADDR          0xdffe0000      /* DRAMC Base Address 64KB */
#define AMBA_CORTEX_A9_AHB_PHY_BASE_ADDR            0xe0000000      /* AHB Mapped Peripherals 256MB */
#define AMBA_CORTEX_A9_APB_PHY_BASE_ADDR            0xe8000000      /* APB Mapped Peripherals 256MB */
#define AMBA_CORTEX_A9_DBGBUS_PHY_BASE_ADDR         0xec000000      /* Debug Bus Address */
#define AMBA_CORTEX_A9_AXI_PHY_BASE_ADDR            0xf0000000      /* AXI Bus Bass Address */

#define AMBA_CORTEX_A9_DRAM_CTRL_PHY_BASE_ADDR      AMBA_CORTEX_A9_DRAMC_PHY_BASE_ADDR
#define AMBA_CORTEX_A9_SCU_PHY_BASE_ADDR            (AMBA_CORTEX_A9_AXI_PHY_BASE_ADDR + 0x00000)
#define AMBA_CORTEX_A9_DDR_CTRL_PHY_BASE_ADDR		(AMBA_CORTEX_A9_DRAMC_PHY_BASE_ADDR + 0x0800)

#define AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR          0xa0000000      /* DRAM Virtual Base Address 1GB */

#define AMBA_CORTEX_A9_AHB_BASE_ADDR                0xe0000000      /* AHB Mapped Peripherals 256MB */
#define AMBA_CORTEX_A9_APB_BASE_ADDR                0xe8000000      /* APB Mapped Peripherals 256MB */
#define AMBA_CORTEX_A9_DBGBUS_BASE_ADDR             0xec000000      /* Debug Bus Addresss */
#define AMBA_CORTEX_A9_DRAMC_BASE_ADDR              0xef000000      /* DRAMC Bus Bass Address */
#define AMBA_CORTEX_A9_AXI_BASE_ADDR                0xf0000000      /* AXI Bus Bass Address */

#define AMBA_CORTEX_A9_DRAM_CTRL_BASE_ADDR          AMBA_CORTEX_A9_DRAMC_BASE_ADDR
#define AMBA_CORTEX_A9_DDR_CTRL_BASE_ADDR           (AMBA_CORTEX_A9_DRAMC_BASE_ADDR + 0x0800)

#define AMBA_CORTEX_A9_SCU_BASE_ADDR                (AMBA_CORTEX_A9_AXI_BASE_ADDR + 0x00000)
#define AMBA_CORTEX_A9_L2CC_BASE_ADDR               (AMBA_CORTEX_A9_AXI_BASE_ADDR + 0x02000)
#define AMBA_CORTEX_A9_CRYPTO_ENGINE0_BASE_ADDR     (AMBA_CORTEX_A9_AXI_BASE_ADDR + 0x20000)

#define AMBA_CORTEX_A9_FIO_DMA_FIFO_BASE_ADDR       (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x000000)
#define AMBA_CORTEX_A9_FIO_CTRL_BASE_ADDR           (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x001000)
#define AMBA_CORTEX_A9_FIO_DMA_CTRL_BASE_ADDR       (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x001080)
#define AMBA_CORTEX_A9_NAND_CTRL_BASE_ADDR          (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x001100)

#define AMBA_CORTEX_A9_SD0_BASE_ADDR                (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x002000)
#define AMBA_CORTEX_A9_VIC0_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x003000)
#define AMBA_CORTEX_A9_DMA_BASE_ADDR                (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x005000)
#define AMBA_CORTEX_A9_USB_DEVICE_BASE_ADDR         (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x006000)
#define AMBA_CORTEX_A9_AHB_CPU_ID_BASE_ADDR         (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x00b000)
#define AMBA_CORTEX_A9_SD1_BASE_ADDR                (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x00c000)
#define AMBA_CORTEX_A9_ETH_MAC_BASE_ADDR            (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x00e000)
#define AMBA_CORTEX_A9_ETH_DMA_BASE_ADDR            (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x00f000)
#define AMBA_CORTEX_A9_VIC1_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x010000)
#define AMBA_CORTEX_A9_FDMA_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x012000)
#define AMBA_CORTEX_A9_HDMI_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x013000)
#define AMBA_CORTEX_A9_GDMA_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x015000)
#define AMBA_CORTEX_A9_USB_EHCI_BASE_ADDR           (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x018000)
#define AMBA_CORTEX_A9_USB_OHCI_BASE_ADDR           (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x019000)
#define AMBA_CORTEX_A9_I2S1_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x01a000)
#define AMBA_CORTEX_A9_SCRATCHPAD_BASE_ADDR         (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x01b000)
#define AMBA_CORTEX_A9_VIC2_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x01c000)
#define AMBA_CORTEX_A9_SECURE_BASE_ADDR             (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x01d000)
#define AMBA_CORTEX_A9_SD2_BASE_ADDR                (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x01f000)
#define AMBA_CORTEX_A9_SPI0_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x020000)
#define AMBA_CORTEX_A9_SPI1_BASE_ADDR               (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x021000)
#define AMBA_CORTEX_A9_SPI_SLAVE_BASE_ADDR          (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x026000)
#define AMBA_CORTEX_A9_NOR_CTRL_BASE_ADDR              (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x031000)
#define AMBA_CORTEX_A9_UART1_BASE_ADDR              (AMBA_CORTEX_A9_AHB_BASE_ADDR + 0x032000)

#define AMBA_CORTEX_A9_I2C1_BASE_ADDR               (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x001000)
#define AMBA_CORTEX_A9_I2C0_BASE_ADDR               (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x003000)
#define AMBA_CORTEX_A9_STEPPER_BASE_ADDR            (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x004000)
#define AMBA_CORTEX_A9_UART0_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x005000)
#define AMBA_CORTEX_A9_IR_REMOTE_BASE_ADDR          (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x006000)
#define AMBA_CORTEX_A9_I2C2_BASE_ADDR               (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x007000)
#define AMBA_CORTEX_A9_PWM_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x008000)
#define AMBA_CORTEX_A9_GPIO0_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x009000)
#define AMBA_CORTEX_A9_GPIO1_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x00a000)
#define AMBA_CORTEX_A9_TIMER_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x00b000)
#define AMBA_CORTEX_A9_WDT_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x00c000)
#define AMBA_CORTEX_A9_GPIO2_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x00e000)
#define AMBA_CORTEX_A9_GPIO3_BASE_ADDR              (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x010000)

#define AMBA_CORTEX_A9_PWC_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x015020)
#define AMBA_CORTEX_A9_RTC_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x015020)
#define AMBA_CORTEX_A9_GPIO_PULL_CTRL_BASE_ADDR     (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x015000)
#define AMBA_CORTEX_A9_IO_MUX_BASE_ADDR             (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x016000)
#define AMBA_CORTEX_A9_ADC_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x01d000)

#define AMBA_CORTEX_A9_RCT_BASE_ADDR                (AMBA_CORTEX_A9_APB_BASE_ADDR + 0x4170000)

/*------------------------------------------------------------------------------------------------*\
 *  Following definitions are for Magic Codes of Warm-Boot
\*------------------------------------------------------------------------------------------------*/
#define AMBA_WARM_BOOT_MAGIC_CODE_SIZE              0x10
#define AMBA_RAM_WARM_BOOT_MAGIC_CODE_OFFSET        0x0E00

#define AMBA_RAM_WARM_BOOT_MAGIC_CODE_ADDR          (AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR + AMBA_RAM_WARM_BOOT_MAGIC_CODE_OFFSET)
#define AMBA_RAM_WARM_BOOT_MAGIC_CODE_PHY_ADDR      (AMBA_CORTEX_A9_DRAM_BASE_ADDR + AMBA_RAM_WARM_BOOT_MAGIC_CODE_OFFSET)
#define AMBA_ROM_WARM_BOOT_MAGIC_CODE_ADDR          (AMBA_RAM_WARM_BOOT_MAGIC_CODE_ADDR + AMBA_WARM_BOOT_MAGIC_CODE_SIZE)
#define AMBA_ROM_WARM_BOOT_MAGIC_CODE_PHY_ADDR      (AMBA_RAM_WARM_BOOT_MAGIC_CODE_PHY_ADDR + AMBA_WARM_BOOT_MAGIC_CODE_SIZE)

#endif /* _AMBA_CORTEX_A9_H_ */
