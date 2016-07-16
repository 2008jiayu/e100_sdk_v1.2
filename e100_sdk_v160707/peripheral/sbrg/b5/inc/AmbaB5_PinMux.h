/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPinMux.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PIN MUX APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PIN_MUX_H_
#define _AMBA_B5_PIN_MUX_H_

#define AMBA_B5_NUM_MULTIFUNC_PIN           37  /* Total Number of multi-function pins */

#define PIN_ALT_FUNC(PinNo, FuncNo)         (((FuncNo) << 8) | PinNo)

typedef enum _AMBA_B5_PIN_ID_e_ {
    AMBA_B5_PIN_GPIO0                       = PIN_ALT_FUNC(0, 0),
    AMBA_B5_PIN_GPIO1                       = PIN_ALT_FUNC(1, 0),
    AMBA_B5_PIN_GPIO2                       = PIN_ALT_FUNC(2, 0),
    AMBA_B5_PIN_GPIO3                       = PIN_ALT_FUNC(3, 0),
    AMBA_B5_PIN_GPIO4                       = PIN_ALT_FUNC(4, 0),
    AMBA_B5_PIN_GPIO5                       = PIN_ALT_FUNC(5, 0),
    AMBA_B5_PIN_GPIO6                       = PIN_ALT_FUNC(6, 0),
    AMBA_B5_PIN_GPIO7                       = PIN_ALT_FUNC(7, 0),
    AMBA_B5_PIN_GPIO8                       = PIN_ALT_FUNC(8, 0),
    AMBA_B5_PIN_GPIO9                       = PIN_ALT_FUNC(9, 0),
    AMBA_B5_PIN_GPIO10                      = PIN_ALT_FUNC(10, 0),
    AMBA_B5_PIN_GPIO11                      = PIN_ALT_FUNC(11, 0),
    AMBA_B5_PIN_GPIO12                      = PIN_ALT_FUNC(12, 0),
    AMBA_B5_PIN_GPIO13                      = PIN_ALT_FUNC(13, 0),
    AMBA_B5_PIN_GPIO14                      = PIN_ALT_FUNC(14, 0),
    AMBA_B5_PIN_GPIO15                      = PIN_ALT_FUNC(15, 0),
    AMBA_B5_PIN_GPIO16                      = PIN_ALT_FUNC(16, 0),
    AMBA_B5_PIN_GPIO17                      = PIN_ALT_FUNC(17, 0),
    AMBA_B5_PIN_GPIO18                      = PIN_ALT_FUNC(18, 0),
    AMBA_B5_PIN_GPIO19                      = PIN_ALT_FUNC(19, 0),
    AMBA_B5_PIN_GPIO20                      = PIN_ALT_FUNC(20, 0),
    AMBA_B5_PIN_GPIO21                      = PIN_ALT_FUNC(21, 0),
    AMBA_B5_PIN_GPIO22                      = PIN_ALT_FUNC(22, 0),
    AMBA_B5_PIN_GPIO23                      = PIN_ALT_FUNC(23, 0),
    AMBA_B5_PIN_GPIO24                      = PIN_ALT_FUNC(24, 0),
    AMBA_B5_PIN_GPIO25                      = PIN_ALT_FUNC(25, 0),
    AMBA_B5_PIN_GPIO26                      = PIN_ALT_FUNC(26, 0),
    AMBA_B5_PIN_GPIO27                      = PIN_ALT_FUNC(27, 0),
    AMBA_B5_PIN_GPIO28                      = PIN_ALT_FUNC(28, 0),
    AMBA_B5_PIN_GPIO29                      = PIN_ALT_FUNC(29, 0),
    AMBA_B5_PIN_GPIO30                      = PIN_ALT_FUNC(30, 0),
    AMBA_B5_PIN_GPIO31                      = PIN_ALT_FUNC(31, 0),

    AMBA_B5_PIN_I2C1_CLK0                   = PIN_ALT_FUNC(0, 1),   /* Clock pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_DATA0                  = PIN_ALT_FUNC(1, 1),   /* Data pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_CLK1                   = PIN_ALT_FUNC(2, 1),   /* Clock pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_DATA1                  = PIN_ALT_FUNC(3, 1),   /* Data pin of the 2nd I2C master */
    AMBA_B5_PIN_SPI_CLK                     = PIN_ALT_FUNC(4, 1),   /* Clock pin of SPI master */
    AMBA_B5_PIN_SPI_EN0                     = PIN_ALT_FUNC(5, 1),   /* 1st Slave Select pin of SPI master */
    AMBA_B5_PIN_SPI_MOSI                    = PIN_ALT_FUNC(6, 1),   /* Output Data pin of SPI master */
    AMBA_B5_PIN_SPI_MISO                    = PIN_ALT_FUNC(7, 1),   /* Input Data pin of SPI master */
    AMBA_B5_PIN_I2C0_CLK                    = PIN_ALT_FUNC(8, 1),   /* Clock pin of the 1st I2C master */
    AMBA_B5_PIN_I2C0_DATA                   = PIN_ALT_FUNC(9, 1),   /* Data pin of the 1st I2C master */
    AMBA_B5_PIN_VIN0_VSYNC                  = PIN_ALT_FUNC(10, 1),  /* Output VSYNC pin of VIN0 */
    AMBA_B5_PIN_VIN0_HSYNC                  = PIN_ALT_FUNC(11, 1),  /* Output HSYNC pin of VIN0 */
    AMBA_B5_PIN_VIN1_VSYNC                  = PIN_ALT_FUNC(12, 1),  /* Output VSYNC pin of VIN1 */
    AMBA_B5_PIN_VIN1_HSYNC                  = PIN_ALT_FUNC(13, 1),  /* Output HSYNC pin of VIN1 */
    AMBA_B5_PIN_VIN2_VSYNC0                 = PIN_ALT_FUNC(14, 1),  /* Output VSYNC pin of VIN2 */
    AMBA_B5_PIN_VIN2_HSYNC0                 = PIN_ALT_FUNC(15, 1),  /* Output HSYNC pin of VIN2 */
    AMBA_B5_PIN_VIN3_VSYNC                  = PIN_ALT_FUNC(16, 1),  /* Output VSYNC pin of VIN3 */
    AMBA_B5_PIN_VIN3_HSYNC                  = PIN_ALT_FUNC(17, 1),  /* Output HSYNC pin of VIN3 */
    AMBA_B5_PIN_B5F_0_RESET                 = PIN_ALT_FUNC(18, 1),  /* Broadcasting of B5N Reset Input */
    AMBA_B5_PIN_B5F_1_RESET                 = PIN_ALT_FUNC(19, 1),  /* Broadcasting of B5N Reset Input */
    AMBA_B5_PIN_B5F_2_RESET                 = PIN_ALT_FUNC(20, 1),  /* Broadcasting of B5N Reset Input */
    AMBA_B5_PIN_B5F_3_RESET                 = PIN_ALT_FUNC(21, 1),  /* Broadcasting of B5N Reset Input */
    AMBA_B5_PIN_PLL_OBSV_CORE0              = PIN_ALT_FUNC(27, 1),  /* Core PLL Frequency Observer */
    AMBA_B5_PIN_PLL_OBSV_PHY                = PIN_ALT_FUNC(28, 1),  /* PHY PLL Frequency Observer */
    AMBA_B5_PIN_PLL_OBSV_SENSOR             = PIN_ALT_FUNC(29, 1),  /* Sensor PLL Frequency Observer */
    AMBA_B5_PIN_PLL_OBSV_VO                 = PIN_ALT_FUNC(32, 1),  /* Video PLL Frequency Observer */
    AMBA_B5_PIN_PLL_OBSV_CORE1              = PIN_ALT_FUNC(33, 1),  /* Core PLL Frequency Observer */

    AMBA_B5_PIN_SPI_EN1                     = PIN_ALT_FUNC(0, 2),   /* 2nd Slave Select pin of SPI master */
    AMBA_B5_PIN_SPI_EN2                     = PIN_ALT_FUNC(1, 2),   /* 3rd Slave Select pin of SPI master */
    AMBA_B5_PIN_SPI_EN3                     = PIN_ALT_FUNC(2, 2),   /* 4th Slave Select pin of SPI master */
    AMBA_B5_PIN_I2C1_CLK2                   = PIN_ALT_FUNC(4, 2),   /* Clock pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_DATA2                  = PIN_ALT_FUNC(5, 2),   /* Data pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_CLK3                   = PIN_ALT_FUNC(6, 2),   /* Clock pin of the 2nd I2C master */
    AMBA_B5_PIN_I2C1_DATA3                  = PIN_ALT_FUNC(7, 2),   /* Data pin of the 2nd I2C master */
    AMBA_B5_PIN_UART_RX                     = PIN_ALT_FUNC(8, 2),   /* RX pin of UART */
    AMBA_B5_PIN_UART_TX                     = PIN_ALT_FUNC(9, 2),   /* TX pin of UART */
    AMBA_B5_PIN_CLK_SI                      = PIN_ALT_FUNC(32, 2),  /* Sensor Clock Output pin */
    AMBA_B5_PIN_CFG_SPI_CLK                 = PIN_ALT_FUNC(33, 2),  /* Clock pin of SPI slave */
    AMBA_B5_PIN_CFG_SPI_EN                  = PIN_ALT_FUNC(34, 2),  /* 1st Slave Select pin of SPI slave */
    AMBA_B5_PIN_CFG_SPI_MOSI                = PIN_ALT_FUNC(35, 2),  /* Output Data pin of SPI slave */
    AMBA_B5_PIN_CFG_SPI_MISO                = PIN_ALT_FUNC(36, 2),  /* Input Data pin of SPI slave */

    AMBA_B5_PIN_PWM_LS_TX_OUT0              = PIN_ALT_FUNC(0, 3),   /* One wire PWM TX pin to the B5F0 */
    AMBA_B5_PIN_PWM_LS_TX_OUT1              = PIN_ALT_FUNC(1, 3),   /* One wire PWM TX pin to the B5F1 */
    AMBA_B5_PIN_PWM_LS_TX_OUT2              = PIN_ALT_FUNC(2, 3),   /* One wire PWM TX pin to the B5F2 */
    AMBA_B5_PIN_PWM_LS_TX_OUT3              = PIN_ALT_FUNC(3, 3),   /* One wire PWM TX pin to the B5F3 */
    AMBA_B5_PIN_VIN2_VSYNC1                 = PIN_ALT_FUNC(10, 3),  /* Output VSYNC pin of VIN2 */
    AMBA_B5_PIN_VIN2_HSYNC1                 = PIN_ALT_FUNC(11, 3),  /* Output HSYNC pin of VIN2 */
    AMBA_B5_PIN_I2C_SLAVE_CLK               = PIN_ALT_FUNC(33, 3),  /* Clock pin of I2C slave */
    AMBA_B5_PIN_I2C_SLAVE_DATA              = PIN_ALT_FUNC(34, 3),  /* Data pin of I2C slave */

    AMBA_B5_PIN_FPGA_MPHY_TX_BURST          = PIN_ALT_FUNC(4, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_PROTDORDY      = PIN_ALT_FUNC(5, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_DATANCTRL      = PIN_ALT_FUNC(6, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL_CLK     = PIN_ALT_FUNC(7, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_PHYDIRDY       = PIN_ALT_FUNC(8, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL0        = PIN_ALT_FUNC(23, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL1        = PIN_ALT_FUNC(24, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL2        = PIN_ALT_FUNC(25, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL3        = PIN_ALT_FUNC(26, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL4        = PIN_ALT_FUNC(28, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL5        = PIN_ALT_FUNC(29, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL6        = PIN_ALT_FUNC(30, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL7        = PIN_ALT_FUNC(31, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL8        = PIN_ALT_FUNC(10, 4),
    AMBA_B5_PIN_FPGA_MPHY_TX_SYMBOL9        = PIN_ALT_FUNC(11, 4),
    AMBA_B5_PIN_PWM_LS_RX_IN                = PIN_ALT_FUNC(33, 4),  /* One wire PWM RX pin of B5F */

    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOLERR      = PIN_ALT_FUNC(4, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_PHYDORDY       = PIN_ALT_FUNC(5, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_DATANCTRL      = PIN_ALT_FUNC(6, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL_CLK     = PIN_ALT_FUNC(7, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_END            = PIN_ALT_FUNC(8, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_READY          = PIN_ALT_FUNC(9, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_STANDBY        = PIN_ALT_FUNC(10, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_START          = PIN_ALT_FUNC(11, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL0        = PIN_ALT_FUNC(22, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL1        = PIN_ALT_FUNC(23, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL2        = PIN_ALT_FUNC(24, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL3        = PIN_ALT_FUNC(25, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL4        = PIN_ALT_FUNC(26, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL5        = PIN_ALT_FUNC(27, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL6        = PIN_ALT_FUNC(28, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL7        = PIN_ALT_FUNC(29, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL8        = PIN_ALT_FUNC(30, 5),
    AMBA_B5_PIN_FPGA_MPHY_RX_SYMBOL9        = PIN_ALT_FUNC(31, 5),
} AMBA_B5_PIN_ID_e;

typedef union _AMBA_B5_PIN_FUNC_u_ {
    AMBA_B5_PIN_ID_e   Data;        /* this is an 16-bit data */

    struct {
        UINT8   PinID;              /* Pin number */
        UINT8   AltFunc;            /* Alternate function */
    } Bits;
} AMBA_B5_PIN_FUNC_u;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_PinMux.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaB5N_PinMuxConfig(AMBA_B5_PIN_CONFIG_s *pPinConfig);
int AmbaB5F_PinMuxConfig(AMBA_B5_CHIP_ID_u ChipID, AMBA_B5_PIN_CONFIG_s *pPinConfig);
int AmbaB5N_PinMuxSetFunc(AMBA_B5_PIN_FUNC_u PinFunc);
int AmbaB5F_PinMuxSetFunc(AMBA_B5_CHIP_ID_u ChipID, AMBA_B5_PIN_FUNC_u PinFunc);

#endif /* _AMBA_B5_PIN_MUX_H_ */
