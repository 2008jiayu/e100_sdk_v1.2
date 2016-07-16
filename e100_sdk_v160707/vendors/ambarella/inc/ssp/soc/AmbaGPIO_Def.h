/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaGPIO_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for GPIO APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_GPIO_DEF_H_
#define _AMBA_GPIO_DEF_H_

#define GPIO_ALT_FUNC(FuncNo, PinNo)        (((FuncNo) << 12) | PinNo)
#define GPIO_IS_ALT_FUNC(PinNo)             (((PinNo) >> 12) ? 1 : 0)
#define GPIO_GET_ALT_FUNC(PinNo)            ((PinNo) >> 12)

typedef enum _AMBA_GPIO_PIN_ID_e_ {
    /* Group 0 */
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,

    /* Group 1 */
    GPIO_PIN_32,
    GPIO_PIN_33,
    GPIO_PIN_34,
    GPIO_PIN_35,
    GPIO_PIN_36,
    GPIO_PIN_37,
    GPIO_PIN_38,
    GPIO_PIN_39,
    GPIO_PIN_40,
    GPIO_PIN_41,
    GPIO_PIN_42,
    GPIO_PIN_43,
    GPIO_PIN_44,
    GPIO_PIN_45,
    GPIO_PIN_46,
    GPIO_PIN_47,
    GPIO_PIN_48,
    GPIO_PIN_49,
    GPIO_PIN_50,
    GPIO_PIN_51,
    GPIO_PIN_52,
    GPIO_PIN_53,
    GPIO_PIN_54,
    GPIO_PIN_55,
    GPIO_PIN_56,
    GPIO_PIN_57,
    GPIO_PIN_58,
    GPIO_PIN_59,
    GPIO_PIN_60,
    GPIO_PIN_61,
    GPIO_PIN_62,
    GPIO_PIN_63,

    /* Group 2 */
    GPIO_PIN_64,
    GPIO_PIN_65,
    GPIO_PIN_66,
    GPIO_PIN_67,
    GPIO_PIN_68,
    GPIO_PIN_69,
    GPIO_PIN_70,
    GPIO_PIN_71,
    GPIO_PIN_72,
    GPIO_PIN_73,
    GPIO_PIN_74,
    GPIO_PIN_75,
    GPIO_PIN_76,
    GPIO_PIN_77,
    GPIO_PIN_78,
    GPIO_PIN_79,
    GPIO_PIN_80,
    GPIO_PIN_81,
    GPIO_PIN_82,
    GPIO_PIN_83,
    GPIO_PIN_84,
    GPIO_PIN_85,
    GPIO_PIN_86,
    GPIO_PIN_87,
    GPIO_PIN_88,
    GPIO_PIN_89,
    GPIO_PIN_90,
    GPIO_PIN_91,
    GPIO_PIN_92,
    GPIO_PIN_93,
    GPIO_PIN_94,
    GPIO_PIN_95,

    /* Group 3 */
    GPIO_PIN_96,
    GPIO_PIN_97,
    GPIO_PIN_98,
    GPIO_PIN_99,
    GPIO_PIN_100,
    GPIO_PIN_101,
    GPIO_PIN_102,
    GPIO_PIN_103,
    GPIO_PIN_104,
    GPIO_PIN_105,
    GPIO_PIN_106,
    GPIO_PIN_107,
    GPIO_PIN_108,
    GPIO_PIN_109,
    GPIO_PIN_110,
    GPIO_PIN_111,
    GPIO_PIN_112,
    GPIO_PIN_113,

    AMBA_NUM_GPIO_PIN,  /* Total Number of GPIO logical pins */

    GPIO_PIN_0_SD_HS_SEL                    = GPIO_ALT_FUNC(1, GPIO_PIN_0),
    GPIO_PIN_1_EHCI_APP_PRT_OVCURR0         = GPIO_ALT_FUNC(1, GPIO_PIN_1),
    GPIO_PIN_1_UART_AHB_RX                  = GPIO_ALT_FUNC(2, GPIO_PIN_1),
    GPIO_PIN_1_SSIS_SCLK                    = GPIO_ALT_FUNC(3, GPIO_PIN_1),
    GPIO_PIN_1_SC_C0                        = GPIO_ALT_FUNC(4, GPIO_PIN_1),
    GPIO_PIN_2_EHCI_APP_PRT_OVCURR1         = GPIO_ALT_FUNC(1, GPIO_PIN_2),
    GPIO_PIN_2_UART_AHB_TX                  = GPIO_ALT_FUNC(2, GPIO_PIN_2),
    GPIO_PIN_2_SSISRXD                      = GPIO_ALT_FUNC(3, GPIO_PIN_2),
    GPIO_PIN_2_SC_C1                        = GPIO_ALT_FUNC(4, GPIO_PIN_2),
    GPIO_PIN_3_EHCI_PRT_PWR_0               = GPIO_ALT_FUNC(1, GPIO_PIN_3),
    GPIO_PIN_3_UART_AHB_CTS_N               = GPIO_ALT_FUNC(2, GPIO_PIN_3),
    GPIO_PIN_3_SSIS_TXD                     = GPIO_ALT_FUNC(3, GPIO_PIN_3),
    GPIO_PIN_3_SC_C2                        = GPIO_ALT_FUNC(4, GPIO_PIN_3),
    GPIO_PIN_4_EHCI_PRT_PWR_1               = GPIO_ALT_FUNC(1, GPIO_PIN_4),
    GPIO_PIN_4_UART_AHB_RTS_N               = GPIO_ALT_FUNC(2, GPIO_PIN_4),
    GPIO_PIN_4_SSIS_EN                      = GPIO_ALT_FUNC(3, GPIO_PIN_4),
    GPIO_PIN_4_SC_C3                        = GPIO_ALT_FUNC(4, GPIO_PIN_4),
    GPIO_PIN_5_PWM_1                        = GPIO_ALT_FUNC(1, GPIO_PIN_5),
    GPIO_PIN_5_IDSP_PIP_IOPAD_MASTER_HSYNC  = GPIO_ALT_FUNC(2, GPIO_PIN_5),
    GPIO_PIN_5_VIN_STRIG0                   = GPIO_ALT_FUNC(3, GPIO_PIN_5),
    GPIO_PIN_5_SC_D0                        = GPIO_ALT_FUNC(4, GPIO_PIN_5),
    GPIO_PIN_5_UART_AHB_CTS_N               = GPIO_ALT_FUNC(5, GPIO_PIN_5),
    GPIO_PIN_6_PWM_2                        = GPIO_ALT_FUNC(1, GPIO_PIN_6),
    GPIO_PIN_6_IDSP_PIP_IOPAD_MASTER_VSYNC  = GPIO_ALT_FUNC(2, GPIO_PIN_6),
    GPIO_PIN_6_VIN_STRIG_1                  = GPIO_ALT_FUNC(3, GPIO_PIN_6),
    GPIO_PIN_6_SC_D1                        = GPIO_ALT_FUNC(4, GPIO_PIN_6),
    GPIO_PIN_6_UART_AHB_RTS_N               = GPIO_ALT_FUNC(5, GPIO_PIN_6),
    GPIO_PIN_7_SC_A0                        = GPIO_ALT_FUNC(1, GPIO_PIN_7),
    GPIO_PIN_7_SSI1_SCLK                    = GPIO_ALT_FUNC(2, GPIO_PIN_7),
    GPIO_PIN_7_NORSPI_CLK                   = GPIO_ALT_FUNC(3, GPIO_PIN_7),
    GPIO_PIN_7_PWM_0                        = GPIO_ALT_FUNC(4, GPIO_PIN_7),
    GPIO_PIN_7_SDXC_CMD                     = GPIO_ALT_FUNC(5, GPIO_PIN_7),
    GPIO_PIN_8_SC_A1                        = GPIO_ALT_FUNC(1, GPIO_PIN_8),
    GPIO_PIN_8_SSI1_TXD                     = GPIO_ALT_FUNC(2, GPIO_PIN_8),
    GPIO_PIN_8_NORSPI_DQ_0                  = GPIO_ALT_FUNC(3, GPIO_PIN_8),
    GPIO_PIN_8_PWM_1                        = GPIO_ALT_FUNC(4, GPIO_PIN_8),
    GPIO_PIN_8_SDXC_CD                      = GPIO_ALT_FUNC(5, GPIO_PIN_8),
    GPIO_PIN_9_SC_A2                        = GPIO_ALT_FUNC(1, GPIO_PIN_9),
    GPIO_PIN_9_SSI1_RXD                     = GPIO_ALT_FUNC(2, GPIO_PIN_9),
    GPIO_PIN_9_NORSPI_DQ_1                  = GPIO_ALT_FUNC(3, GPIO_PIN_9),
    GPIO_PIN_9_PWM_2                        = GPIO_ALT_FUNC(4, GPIO_PIN_9),
    GPIO_PIN_9_SDXC_WP                      = GPIO_ALT_FUNC(5, GPIO_PIN_9),
    GPIO_PIN_10_SC_A3                       = GPIO_ALT_FUNC(1, GPIO_PIN_10),
    GPIO_PIN_10_SSI1_EN0                    = GPIO_ALT_FUNC(2, GPIO_PIN_10),
    GPIO_PIN_10_NORSPI_DQ_2                 = GPIO_ALT_FUNC(3, GPIO_PIN_10),
    GPIO_PIN_10_PWM_3                       = GPIO_ALT_FUNC(4, GPIO_PIN_10),
    GPIO_PIN_10_SDXC_D_0                    = GPIO_ALT_FUNC(5, GPIO_PIN_10),
    GPIO_PIN_11_SC_B0                       = GPIO_ALT_FUNC(1, GPIO_PIN_11),
    GPIO_PIN_11_SSI1_EN1                    = GPIO_ALT_FUNC(2, GPIO_PIN_11),
    GPIO_PIN_11_NORSPI_DQ_3                 = GPIO_ALT_FUNC(3, GPIO_PIN_11),
    GPIO_PIN_11_SDXC_D_1                    = GPIO_ALT_FUNC(5, GPIO_PIN_11),
    GPIO_PIN_12_SC_B1                       = GPIO_ALT_FUNC(1, GPIO_PIN_12),
    GPIO_PIN_12_SSI1_EN2                    = GPIO_ALT_FUNC(2, GPIO_PIN_12),
    GPIO_PIN_12_NORSPI_EN_0                 = GPIO_ALT_FUNC(3, GPIO_PIN_12),
    GPIO_PIN_12_NORSPI_DQ_2                 = GPIO_ALT_FUNC(4, GPIO_PIN_12),
    GPIO_PIN_12_SDXC_D_2                    = GPIO_ALT_FUNC(5, GPIO_PIN_12),
    GPIO_PIN_13_SC_B2                       = GPIO_ALT_FUNC(1, GPIO_PIN_13),
    GPIO_PIN_13_SSI1_EN3                    = GPIO_ALT_FUNC(2, GPIO_PIN_13),
    GPIO_PIN_13_NORSPI_EN_1                 = GPIO_ALT_FUNC(3, GPIO_PIN_13),
    GPIO_PIN_13_NORSPI_DQ_3                 = GPIO_ALT_FUNC(4, GPIO_PIN_13),
    GPIO_PIN_13_SDXC_D_3                    = GPIO_ALT_FUNC(5, GPIO_PIN_13),
    GPIO_PIN_14_SC_B3                       = GPIO_ALT_FUNC(1, GPIO_PIN_14),
    GPIO_PIN_14_PWM_3                       = GPIO_ALT_FUNC(2, GPIO_PIN_14),
    GPIO_PIN_14_NORSPI_EN_2                 = GPIO_ALT_FUNC(3, GPIO_PIN_14),
    GPIO_PIN_14_SDXC_D_4                    = GPIO_ALT_FUNC(5, GPIO_PIN_14),
    GPIO_PIN_15_SC_C0                       = GPIO_ALT_FUNC(1, GPIO_PIN_15),
    GPIO_PIN_15_UART_AHB_RX                 = GPIO_ALT_FUNC(2, GPIO_PIN_15),
    GPIO_PIN_15_SSIS_SCLK                   = GPIO_ALT_FUNC(3, GPIO_PIN_15),
    GPIO_PIN_15_SDXC_D5                     = GPIO_ALT_FUNC(5, GPIO_PIN_15),
    GPIO_PIN_16_SC_C1                       = GPIO_ALT_FUNC(1, GPIO_PIN_16),
    GPIO_PIN_16_UART_AHB_TX                 = GPIO_ALT_FUNC(2, GPIO_PIN_16),
    GPIO_PIN_16_SSIS_RXD                    = GPIO_ALT_FUNC(3, GPIO_PIN_16),
    GPIO_PIN_16_ENET_CRS                    = GPIO_ALT_FUNC(4, GPIO_PIN_16),
    GPIO_PIN_16_SDXC_D6                     = GPIO_ALT_FUNC(5, GPIO_PIN_16),
    GPIO_PIN_17_SC_C2                       = GPIO_ALT_FUNC(1, GPIO_PIN_17),
    GPIO_PIN_17_UART_AHB_CTS_N              = GPIO_ALT_FUNC(2, GPIO_PIN_17),
    GPIO_PIN_17_SSIS_TXD                    = GPIO_ALT_FUNC(3, GPIO_PIN_17),
    GPIO_PIN_17_ENET_RXD_2                  = GPIO_ALT_FUNC(4, GPIO_PIN_17),
    GPIO_PIN_17_SDXC_D7                     = GPIO_ALT_FUNC(5, GPIO_PIN_17),
    GPIO_PIN_18_SC_C3                       = GPIO_ALT_FUNC(1, GPIO_PIN_18),
    GPIO_PIN_18_UART_AHB_RTS_N              = GPIO_ALT_FUNC(2, GPIO_PIN_18),
    GPIO_PIN_18_SSIS_EN                     = GPIO_ALT_FUNC(3, GPIO_PIN_18),
    GPIO_PIN_18_ENET_RXD_3                  = GPIO_ALT_FUNC(4, GPIO_PIN_18),
    GPIO_PIN_18_SDXC_CLK                    = GPIO_ALT_FUNC(5, GPIO_PIN_18),
    GPIO_PIN_19_SC_D0                       = GPIO_ALT_FUNC(1, GPIO_PIN_19),
    GPIO_PIN_19_UART_AHB_RX                 = GPIO_ALT_FUNC(2, GPIO_PIN_19),
    GPIO_PIN_19_SSIS_SCLK                   = GPIO_ALT_FUNC(3, GPIO_PIN_19),
    GPIO_PIN_19_ENET_COL                    = GPIO_ALT_FUNC(4, GPIO_PIN_19),
    GPIO_PIN_19_PWM_0                       = GPIO_ALT_FUNC(5, GPIO_PIN_19),
    GPIO_PIN_20_SC_D1                       = GPIO_ALT_FUNC(1, GPIO_PIN_20),
    GPIO_PIN_20_UART_AHB_TX                 = GPIO_ALT_FUNC(2, GPIO_PIN_20),
    GPIO_PIN_20_SSIS_RXD                    = GPIO_ALT_FUNC(3, GPIO_PIN_20),
    GPIO_PIN_20_ENET_TX_CLK                 = GPIO_ALT_FUNC(4, GPIO_PIN_20),
    GPIO_PIN_20_PWM_1                       = GPIO_ALT_FUNC(5, GPIO_PIN_20),
    GPIO_PIN_21_SC_D2                       = GPIO_ALT_FUNC(1, GPIO_PIN_21),
    GPIO_PIN_21_UART_AHB_CTS_N              = GPIO_ALT_FUNC(2, GPIO_PIN_21),
    GPIO_PIN_21_SSIS_TXD                    = GPIO_ALT_FUNC(3, GPIO_PIN_21),
    GPIO_PIN_21_ENET_TX_ER                  = GPIO_ALT_FUNC(4, GPIO_PIN_21),
    GPIO_PIN_21_PWM_2                       = GPIO_ALT_FUNC(5, GPIO_PIN_21),
    GPIO_PIN_22_SC_D3                       = GPIO_ALT_FUNC(1, GPIO_PIN_22),
    GPIO_PIN_22_UART_AHB_RTS_N              = GPIO_ALT_FUNC(2, GPIO_PIN_22),
    GPIO_PIN_22_SSIS_EN                     = GPIO_ALT_FUNC(3, GPIO_PIN_22),
    GPIO_PIN_22_ENET_TXD_2                  = GPIO_ALT_FUNC(4, GPIO_PIN_22),
    GPIO_PIN_22_PWM_3                       = GPIO_ALT_FUNC(5, GPIO_PIN_22),
    GPIO_PIN_23_SC_E0                       = GPIO_ALT_FUNC(1, GPIO_PIN_23),
    GPIO_PIN_23_SSI0_EN2                    = GPIO_ALT_FUNC(2, GPIO_PIN_23),
    GPIO_PIN_23_NORSPI_EN_3                 = GPIO_ALT_FUNC(3, GPIO_PIN_23),
    GPIO_PIN_23_ENET_TXD_3                  = GPIO_ALT_FUNC(4, GPIO_PIN_23),
    GPIO_PIN_23_PWM_1                       = GPIO_ALT_FUNC(5, GPIO_PIN_23),
    GPIO_PIN_24_TM11_CLK                    = GPIO_ALT_FUNC(1, GPIO_PIN_24),
    GPIO_PIN_24_ENET_2ND_REF_CLK            = GPIO_ALT_FUNC(4, GPIO_PIN_24),
    GPIO_PIN_25_TM12_CLK                    = GPIO_ALT_FUNC(1, GPIO_PIN_25),
    GPIO_PIN_25_IDSP_PIP_IOPAD_MASTER_HSYNC = GPIO_ALT_FUNC(3, GPIO_PIN_25),
    GPIO_PIN_25_ENET_MDC                    = GPIO_ALT_FUNC(4, GPIO_PIN_25),
    GPIO_PIN_26_TM13_CLK                    = GPIO_ALT_FUNC(1, GPIO_PIN_26),
    GPIO_PIN_26_SSI0_EN3                    = GPIO_ALT_FUNC(2, GPIO_PIN_26),
    GPIO_PIN_26_IDSP_PIP_IOPAD_MASTER_VSYNC = GPIO_ALT_FUNC(3, GPIO_PIN_26),
    GPIO_PIN_26_ENET_MDIO                   = GPIO_ALT_FUNC(4, GPIO_PIN_26),
    GPIO_PIN_27_IDC0CLK                     = GPIO_ALT_FUNC(1, GPIO_PIN_27),
    GPIO_PIN_28_IDC0DATA                    = GPIO_ALT_FUNC(1, GPIO_PIN_28),
    GPIO_PIN_29_IDC1CLK                     = GPIO_ALT_FUNC(1, GPIO_PIN_29),
    GPIO_PIN_29_NOTSPI_DQ_2                 = GPIO_ALT_FUNC(3, GPIO_PIN_29),
    GPIO_PIN_29_NOTSPI_EN_2                 = GPIO_ALT_FUNC(4, GPIO_PIN_29),
    GPIO_PIN_30_IDC1DATA                    = GPIO_ALT_FUNC(1, GPIO_PIN_30),
    GPIO_PIN_30_NOTSPI_DQ_3                 = GPIO_ALT_FUNC(3, GPIO_PIN_30),
    GPIO_PIN_30_NOTSPI_EN_3                 = GPIO_ALT_FUNC(4, GPIO_PIN_30),
    GPIO_PIN_31_IDC2CLK                     = GPIO_ALT_FUNC(1, GPIO_PIN_31),
    GPIO_PIN_31_VIN_STRIG0                  = GPIO_ALT_FUNC(2, GPIO_PIN_31),
    GPIO_PIN_32_IDC2DATA                    = GPIO_ALT_FUNC(1, GPIO_PIN_32),
    GPIO_PIN_32_VIN_STRIG1                  = GPIO_ALT_FUNC(2, GPIO_PIN_32),
    GPIO_PIN_33_IR_IN                       = GPIO_ALT_FUNC(1, GPIO_PIN_33),
    GPIO_PIN_34_SSI0_SCLK                   = GPIO_ALT_FUNC(1, GPIO_PIN_34),
    GPIO_PIN_34_NOTSPI_CLK                  = GPIO_ALT_FUNC(2, GPIO_PIN_34),
    GPIO_PIN_34_UART_AHB_RX                 = GPIO_ALT_FUNC(3, GPIO_PIN_34),
    GPIO_PIN_34_SSIS_SCLK                   = GPIO_ALT_FUNC(4, GPIO_PIN_34),
    GPIO_PIN_35_SSI0_TXD                    = GPIO_ALT_FUNC(1, GPIO_PIN_35),
    GPIO_PIN_35_NORSPI_DQ_0                 = GPIO_ALT_FUNC(2, GPIO_PIN_35),
    GPIO_PIN_35_UART_AHB_TX                 = GPIO_ALT_FUNC(3, GPIO_PIN_35),
    GPIO_PIN_35_SSIS_RXD                    = GPIO_ALT_FUNC(4, GPIO_PIN_35),
    GPIO_PIN_36_SSI0_RXD                    = GPIO_ALT_FUNC(1, GPIO_PIN_36),
    GPIO_PIN_36_NORSPI_DQ_1                 = GPIO_ALT_FUNC(2, GPIO_PIN_36),
    GPIO_PIN_36_UART_AHB_CTS_N              = GPIO_ALT_FUNC(3, GPIO_PIN_36),
    GPIO_PIN_36_SSIS_TXD                    = GPIO_ALT_FUNC(4, GPIO_PIN_36),
    GPIO_PIN_37_SSI0_EN0                    = GPIO_ALT_FUNC(1, GPIO_PIN_37),
    GPIO_PIN_37_NORSPI_EN_0                 = GPIO_ALT_FUNC(2, GPIO_PIN_37),
    GPIO_PIN_37_UART_AHB_RTS_N              = GPIO_ALT_FUNC(3, GPIO_PIN_37),
    GPIO_PIN_37_SSIS_EN                     = GPIO_ALT_FUNC(4, GPIO_PIN_37),
    GPIO_PIN_38_SSI0_EN1                    = GPIO_ALT_FUNC(1, GPIO_PIN_38),
    GPIO_PIN_38_NORSPI_EN_1                 = GPIO_ALT_FUNC(2, GPIO_PIN_38),
    GPIO_PIN_39_UART0RX                     = GPIO_ALT_FUNC(1, GPIO_PIN_39),
    GPIO_PIN_39_UART_AHB_RX                 = GPIO_ALT_FUNC(2, GPIO_PIN_39),
    GPIO_PIN_40_UART0TX                     = GPIO_ALT_FUNC(1, GPIO_PIN_40),
    GPIO_PIN_40_UART_AHB_TX                 = GPIO_ALT_FUNC(2, GPIO_PIN_40),
    GPIO_PIN_41_I2S_CLK                     = GPIO_ALT_FUNC(1, GPIO_PIN_41),
    GPIO_PIN_42_I2S_SI                      = GPIO_ALT_FUNC(1, GPIO_PIN_42),
    GPIO_PIN_43_I2S_SO                      = GPIO_ALT_FUNC(1, GPIO_PIN_43),
    GPIO_PIN_44_I2S_WS                      = GPIO_ALT_FUNC(1, GPIO_PIN_44),
    GPIO_PIN_45_CLK_AU                      = GPIO_ALT_FUNC(1, GPIO_PIN_45),
    GPIO_PIN_46_RMII_ENET_TXEN              = GPIO_ALT_FUNC(1, GPIO_PIN_46),
    GPIO_PIN_46_SC_A0                       = GPIO_ALT_FUNC(2, GPIO_PIN_46),
    GPIO_PIN_46_MII_ENET_TXEN               = GPIO_ALT_FUNC(3, GPIO_PIN_46),
    GPIO_PIN_46_SSI1_SCLK                   = GPIO_ALT_FUNC(4, GPIO_PIN_46),
    GPIO_PIN_46_NORSPI_CLK                  = GPIO_ALT_FUNC(5, GPIO_PIN_46),
    GPIO_PIN_47_RMII_ENET_TXD_0             = GPIO_ALT_FUNC(1, GPIO_PIN_47),
    GPIO_PIN_47_SC_A1                       = GPIO_ALT_FUNC(2, GPIO_PIN_47),
    GPIO_PIN_47_MII_ENET_TXD                = GPIO_ALT_FUNC(3, GPIO_PIN_47),
    GPIO_PIN_47_SSI1_TXD                    = GPIO_ALT_FUNC(4, GPIO_PIN_47),
    GPIO_PIN_47_NORSPI_DQ_0                 = GPIO_ALT_FUNC(5, GPIO_PIN_47),
    GPIO_PIN_48_RMII_ENET_TXD_1             = GPIO_ALT_FUNC(1, GPIO_PIN_48),
    GPIO_PIN_48_SC_A2                       = GPIO_ALT_FUNC(2, GPIO_PIN_48),
    GPIO_PIN_48_MII_ENET_TXD_1              = GPIO_ALT_FUNC(3, GPIO_PIN_48),
    GPIO_PIN_48_SSI1_RXD                    = GPIO_ALT_FUNC(4, GPIO_PIN_48),
    GPIO_PIN_48_NORSPI_DQ_1                 = GPIO_ALT_FUNC(5, GPIO_PIN_48),
    GPIO_PIN_49_RMII_ENET_RXD_0             = GPIO_ALT_FUNC(1, GPIO_PIN_49),
    GPIO_PIN_49_SC_A3                       = GPIO_ALT_FUNC(2, GPIO_PIN_49),
    GPIO_PIN_49_MII_ENET_RXD_0              = GPIO_ALT_FUNC(3, GPIO_PIN_49),
    GPIO_PIN_49_SSI1_EN0                    = GPIO_ALT_FUNC(4, GPIO_PIN_49),
    GPIO_PIN_49_NORSPI_EN_0                 = GPIO_ALT_FUNC(5, GPIO_PIN_49),
    GPIO_PIN_50_RMII_ENET_RXD_1             = GPIO_ALT_FUNC(1, GPIO_PIN_50),
    GPIO_PIN_50_SC_B0                       = GPIO_ALT_FUNC(2, GPIO_PIN_50),
    GPIO_PIN_50_MII_ENET_RXD_1              = GPIO_ALT_FUNC(3, GPIO_PIN_50),
    GPIO_PIN_50_SSI1_EN1                    = GPIO_ALT_FUNC(4, GPIO_PIN_50),
    GPIO_PIN_50_NORSPI_EN_1                 = GPIO_ALT_FUNC(5, GPIO_PIN_50),
    GPIO_PIN_51_RMII_ENET_RXER              = GPIO_ALT_FUNC(1, GPIO_PIN_51),
    GPIO_PIN_51_SC_B1                       = GPIO_ALT_FUNC(2, GPIO_PIN_51),
    GPIO_PIN_51_MII_ENET_RXER               = GPIO_ALT_FUNC(3, GPIO_PIN_51),
    GPIO_PIN_51_SSI1_EN2                    = GPIO_ALT_FUNC(4, GPIO_PIN_51),
    GPIO_PIN_51_NORSPI_EN_2                 = GPIO_ALT_FUNC(5, GPIO_PIN_51),
    GPIO_PIN_52_RMII_ENET_CRS_DV            = GPIO_ALT_FUNC(1, GPIO_PIN_52),
    GPIO_PIN_52_SC_B2                       = GPIO_ALT_FUNC(2, GPIO_PIN_52),
    GPIO_PIN_52_MII_ENET_CRS_DV             = GPIO_ALT_FUNC(3, GPIO_PIN_52),
    GPIO_PIN_52_SSI1_EN3                    = GPIO_ALT_FUNC(4, GPIO_PIN_52),
    GPIO_PIN_52_NORSPI_DQ_2                 = GPIO_ALT_FUNC(5, GPIO_PIN_52),
    GPIO_PIN_53_ENET_REF_CLK                = GPIO_ALT_FUNC(1, GPIO_PIN_53),
    GPIO_PIN_53_SC_B3                       = GPIO_ALT_FUNC(2, GPIO_PIN_53),
    GPIO_PIN_53_ENET_RX_CLK                 = GPIO_ALT_FUNC(3, GPIO_PIN_53),
    GPIO_PIN_53_NORSPI_DQ_3                 = GPIO_ALT_FUNC(5, GPIO_PIN_53),
    GPIO_PIN_54_NAND_WP                     = GPIO_ALT_FUNC(2, GPIO_PIN_54),
    GPIO_PIN_55_NAND_CE                     = GPIO_ALT_FUNC(2, GPIO_PIN_55),
    GPIO_PIN_55_NORSPI_CLK                  = GPIO_ALT_FUNC(3, GPIO_PIN_55),
    GPIO_PIN_56_NAND_RB                     = GPIO_ALT_FUNC(2, GPIO_PIN_56),
    GPIO_PIN_56_NORSPI_DQ_4                 = GPIO_ALT_FUNC(3, GPIO_PIN_56),
    GPIO_PIN_57_SD_CLK                      = GPIO_ALT_FUNC(2, GPIO_PIN_57),
    GPIO_PIN_58_SD_CMD                      = GPIO_ALT_FUNC(2, GPIO_PIN_58),
    GPIO_PIN_59_SD_CD                       = GPIO_ALT_FUNC(2, GPIO_PIN_59),
    GPIO_PIN_60_SD_WP                       = GPIO_ALT_FUNC(2, GPIO_PIN_60),
    GPIO_PIN_61_NAND_RE                     = GPIO_ALT_FUNC(2, GPIO_PIN_61),
    GPIO_PIN_61_NORSPI_DQ_5                 = GPIO_ALT_FUNC(3, GPIO_PIN_61),
    GPIO_PIN_62_NAND_WE                     = GPIO_ALT_FUNC(2, GPIO_PIN_62),
    GPIO_PIN_62_NORSPI_DQ_6                 = GPIO_ALT_FUNC(3, GPIO_PIN_62),
    GPIO_PIN_63_NAND_ALE                    = GPIO_ALT_FUNC(2, GPIO_PIN_63),
    GPIO_PIN_63_NORSPI_DQ_7                 = GPIO_ALT_FUNC(3, GPIO_PIN_63),
    GPIO_PIN_64_NAND_D_0                    = GPIO_ALT_FUNC(2, GPIO_PIN_64),
    GPIO_PIN_64_NORSPI_EN_0                 = GPIO_ALT_FUNC(3, GPIO_PIN_64),
    GPIO_PIN_65_NAND_D_1                    = GPIO_ALT_FUNC(2, GPIO_PIN_65),
    GPIO_PIN_65_NORSPI_EN_1                 = GPIO_ALT_FUNC(3, GPIO_PIN_65),
    GPIO_PIN_66_NAND_D_2                    = GPIO_ALT_FUNC(2, GPIO_PIN_66),
    GPIO_PIN_66_NORSPI_EN_2                 = GPIO_ALT_FUNC(3, GPIO_PIN_66),
    GPIO_PIN_67_NAND_D_3                    = GPIO_ALT_FUNC(2, GPIO_PIN_67),
    GPIO_PIN_67_NORSPI_EN_3                 = GPIO_ALT_FUNC(3, GPIO_PIN_67),
    GPIO_PIN_68_NAND_D_4                    = GPIO_ALT_FUNC(2, GPIO_PIN_68),
    GPIO_PIN_68_NORSPI_DQ_0                 = GPIO_ALT_FUNC(3, GPIO_PIN_68),
    GPIO_PIN_69_NAND_D_5                    = GPIO_ALT_FUNC(2, GPIO_PIN_69),
    GPIO_PIN_69_NORSPI_DQ_1                 = GPIO_ALT_FUNC(3, GPIO_PIN_69),
    GPIO_PIN_70_NAND_D_6                    = GPIO_ALT_FUNC(2, GPIO_PIN_70),
    GPIO_PIN_70_NORSPI_DQ_2                 = GPIO_ALT_FUNC(3, GPIO_PIN_70),
    GPIO_PIN_71_NAND_D_7                    = GPIO_ALT_FUNC(2, GPIO_PIN_71),
    GPIO_PIN_71_NORSPI_DQ_3                 = GPIO_ALT_FUNC(3, GPIO_PIN_71),
    GPIO_PIN_72_NAND_CLE                    = GPIO_ALT_FUNC(2, GPIO_PIN_72),
    GPIO_PIN_73_SD_D_0                      = GPIO_ALT_FUNC(2, GPIO_PIN_73),
    GPIO_PIN_74_SD_D_1                      = GPIO_ALT_FUNC(2, GPIO_PIN_74),
    GPIO_PIN_75_SD_D_2                      = GPIO_ALT_FUNC(2, GPIO_PIN_75),
    GPIO_PIN_76_SD_D_3                      = GPIO_ALT_FUNC(2, GPIO_PIN_76),
    GPIO_PIN_77_SD_D_4                      = GPIO_ALT_FUNC(2, GPIO_PIN_77),
    GPIO_PIN_77_SC_C0                       = GPIO_ALT_FUNC(4, GPIO_PIN_77),
    GPIO_PIN_77_SSIS_SC                     = GPIO_ALT_FUNC(5, GPIO_PIN_77),
    GPIO_PIN_78_SD_D_5                      = GPIO_ALT_FUNC(2, GPIO_PIN_78),
    GPIO_PIN_78_SC_C1                       = GPIO_ALT_FUNC(4, GPIO_PIN_78),
    GPIO_PIN_78_SSIS_RX                     = GPIO_ALT_FUNC(5, GPIO_PIN_78),
    GPIO_PIN_79_SD_D_6                      = GPIO_ALT_FUNC(2, GPIO_PIN_79),
    GPIO_PIN_79_SC_C2                       = GPIO_ALT_FUNC(4, GPIO_PIN_79),
    GPIO_PIN_79_SSIS_TX                     = GPIO_ALT_FUNC(5, GPIO_PIN_79),
    GPIO_PIN_80_SD_D_7                      = GPIO_ALT_FUNC(2, GPIO_PIN_80),
    GPIO_PIN_80_SC_C3                       = GPIO_ALT_FUNC(4, GPIO_PIN_80),
    GPIO_PIN_80_SSIS_EN                     = GPIO_ALT_FUNC(5, GPIO_PIN_80),
    GPIO_PIN_81_SDIO_CLK                    = GPIO_ALT_FUNC(2, GPIO_PIN_81),
    GPIO_PIN_82_SDIO_CMD                    = GPIO_ALT_FUNC(2, GPIO_PIN_82),
    GPIO_PIN_83_SDIO_D_0                    = GPIO_ALT_FUNC(2, GPIO_PIN_83),
    GPIO_PIN_83_SC_D0                       = GPIO_ALT_FUNC(4, GPIO_PIN_83),
    GPIO_PIN_83_SSIS_SCLK                   = GPIO_ALT_FUNC(5, GPIO_PIN_83),
    GPIO_PIN_84_SDIO_D_1                    = GPIO_ALT_FUNC(2, GPIO_PIN_84),
    GPIO_PIN_84_SC_D1                       = GPIO_ALT_FUNC(4, GPIO_PIN_84),
    GPIO_PIN_84_SSIS_RXD                    = GPIO_ALT_FUNC(5, GPIO_PIN_84),
    GPIO_PIN_85_SDIO_D_2                    = GPIO_ALT_FUNC(2, GPIO_PIN_85),
    GPIO_PIN_85_SC_D2                       = GPIO_ALT_FUNC(4, GPIO_PIN_85),
    GPIO_PIN_85_SSIS_TXD                    = GPIO_ALT_FUNC(5, GPIO_PIN_85),
    GPIO_PIN_86_SDIO_D_3                    = GPIO_ALT_FUNC(2, GPIO_PIN_86),
    GPIO_PIN_86_SC_D3                       = GPIO_ALT_FUNC(4, GPIO_PIN_86),
    GPIO_PIN_86_SSIS_EN                     = GPIO_ALT_FUNC(5, GPIO_PIN_86),
    GPIO_PIN_87_SDIO_CD                     = GPIO_ALT_FUNC(2, GPIO_PIN_87),
    GPIO_PIN_88_SDIO_WP                     = GPIO_ALT_FUNC(2, GPIO_PIN_88),
    GPIO_PIN_89_HDMITX_HPD                  = GPIO_ALT_FUNC(1, GPIO_PIN_89),
    GPIO_PIN_90_HDMITX_CEC                  = GPIO_ALT_FUNC(1, GPIO_PIN_90),
    GPIO_PIN_90_ENET_2ND_REF_CLK            = GPIO_ALT_FUNC(2, GPIO_PIN_90),
    GPIO_PIN_91_VIN_SVSYNC                  = GPIO_ALT_FUNC(1, GPIO_PIN_91),
    GPIO_PIN_91_IDSP_PIP_IOPAD_MASTER_HSYNC = GPIO_ALT_FUNC(2, GPIO_PIN_91),
    GPIO_PIN_92_VIN_SHSYNC                  = GPIO_ALT_FUNC(1, GPIO_PIN_92),
    GPIO_PIN_92_IDSP_PIP_IOPAD_MASTER_VSYNC = GPIO_ALT_FUNC(2, GPIO_PIN_92),
    GPIO_PIN_93_VD0_OUT_0                   = GPIO_ALT_FUNC(1, GPIO_PIN_93),
    GPIO_PIN_94_VD0_OUT_1                   = GPIO_ALT_FUNC(1, GPIO_PIN_94),
    GPIO_PIN_95_VD0_OUT_2                   = GPIO_ALT_FUNC(1, GPIO_PIN_95),
    GPIO_PIN_96_VD0_OUT_3                   = GPIO_ALT_FUNC(1, GPIO_PIN_96),
    GPIO_PIN_97_VD0_OUT_4                   = GPIO_ALT_FUNC(1, GPIO_PIN_97),
    GPIO_PIN_98_VD0_OUT_5                   = GPIO_ALT_FUNC(1, GPIO_PIN_98),
    GPIO_PIN_99_VD0_OUT_6                   = GPIO_ALT_FUNC(1, GPIO_PIN_99),
    GPIO_PIN_100_VD0_OUT_7                  = GPIO_ALT_FUNC(1, GPIO_PIN_100),
    GPIO_PIN_101_VD0_OUT_8                  = GPIO_ALT_FUNC(1, GPIO_PIN_101),
    GPIO_PIN_102_VD0_OUT_9                  = GPIO_ALT_FUNC(1, GPIO_PIN_102),
    GPIO_PIN_103_VD0_OUT_10                 = GPIO_ALT_FUNC(1, GPIO_PIN_103),
    GPIO_PIN_104_VD0_OUT_11                 = GPIO_ALT_FUNC(1, GPIO_PIN_104),
    GPIO_PIN_105_VD0_OUT_12                 = GPIO_ALT_FUNC(1, GPIO_PIN_105),
    GPIO_PIN_106_VD0_OUT_13                 = GPIO_ALT_FUNC(1, GPIO_PIN_106),
    GPIO_PIN_107_VD0_OUT_14                 = GPIO_ALT_FUNC(1, GPIO_PIN_107),
    GPIO_PIN_108_VD0_OUT_15                 = GPIO_ALT_FUNC(1, GPIO_PIN_108),
    GPIO_PIN_109_VD0_CLK                    = GPIO_ALT_FUNC(1, GPIO_PIN_109),
    GPIO_PIN_110_VD0_VSYNC                  = GPIO_ALT_FUNC(1, GPIO_PIN_110),
    GPIO_PIN_111_VD0_HSYNC                  = GPIO_ALT_FUNC(1, GPIO_PIN_111),
    GPIO_PIN_112_VD0_HVLD                   = GPIO_ALT_FUNC(1, GPIO_PIN_112),
    GPIO_PIN_113_PWM_0                      = GPIO_ALT_FUNC(1, GPIO_PIN_113)

} AMBA_GPIO_PIN_ID_e;

typedef enum _AMBA_GPIO_GROUP_e_ {
    AMBA_GPIO_GROUP0,       /* GPIO 0~31    */
    AMBA_GPIO_GROUP1,       /* GPIO 32~63   */
    AMBA_GPIO_GROUP2,       /* GPIO 64~95   */
    AMBA_GPIO_GROUP3,       /* GPIO 96~113  */

    AMBA_NUM_GPIO_GROUP
} AMBA_GPIO_GROUP_e;

typedef enum _AMBA_GPIO_INT_CONFIG_e_ {
    GPIO_INT_HIGH_LEVEL_TRIGGER = 0,    /* Corresponding interrupt is High level sensitive */
    GPIO_INT_LOW_LEVEL_TRIGGER,         /* Corresponding interrupt is Low level sensitive */
    GPIO_INT_RISING_EDGE_TRIGGER,       /* Corresponding interupt is rising edge-triggered */
    GPIO_INT_FALLING_EDGE_TRIGGER,      /* Corresponding interupt is falling edge-triggered */
    GPIO_INT_BOTH_EDGE_TRIGGER          /* Corresponding interupt is edge-triggered */
} AMBA_GPIO_INT_CONFIG_e;

/*-----------------------------------------------------------------------------------------------*\
 * GPIO line status structures
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_GPIO_PIN_CONFIG_e_ {
    AMBA_GPIO_CONFIG_INPUT = 0,         /* Pin is configured as input */
    AMBA_GPIO_CONFIG_OUTPUT,            /* Pin is configured as output */
    AMBA_GPIO_CONFIG_ALTERNATE          /* Pin is configured as alternate function */
} AMBA_GPIO_PIN_CONFIG_e;

typedef enum _AMBA_GPIO_PIN_LEVEL_e_ {
    AMBA_GPIO_LEVEL_LOW = 0,            /* Pin level state is low */
    AMBA_GPIO_LEVEL_HIGH,               /* Pin level state is high */
    AMBA_GPIO_LEVEL_UNKNOWN             /* Pin level state is unknown (due to pin mask is disabled) */
} AMBA_GPIO_PIN_LEVEL_e;

typedef enum _AMBA_GPIO_PIN_PULL_CTRL_e_ {
    AMBA_GPIO_PULL_DISABLE = 0,         /* Disable Internal Pull Control */
    AMBA_GPIO_PULL_DOWN,                /* Internal Pull Down */
    AMBA_GPIO_PULL_UP,                  /* Internal Pull Up */
} AMBA_GPIO_PIN_PULL_CTRL_e;

typedef enum _AMBA_GPIO_PIN_DRIVE_STRENGTH_e_ {
    AMBA_GPIO_DRIVE_STRENGTH_3MA = 0,   /* Pin drive strength is 3mA */
    AMBA_GPIO_DRIVE_STRENGTH_6MA,       /* Pin drive strength is 6mA */
    AMBA_GPIO_DRIVE_STRENGTH_12MA,      /* Pin drive strength is 12mA */
    AMBA_GPIO_DRIVE_STRENGTH_18MA       /* Pin drive strength is 18mA */
} AMBA_GPIO_PIN_DRIVE_STRENGTH_e;

typedef struct _AMBA_GPIO_PIN_INFO_s_ {
    AMBA_GPIO_PIN_CONFIG_e          Config;
    AMBA_GPIO_PIN_LEVEL_e           Level;
    AMBA_GPIO_PIN_PULL_CTRL_e       PullStatus;
    AMBA_GPIO_PIN_DRIVE_STRENGTH_e  PinDriveStrength;
    UINT8   IntEnable;
} AMBA_GPIO_PIN_INFO_s;

typedef struct _AMBA_GPIO_PIN_GROUP_CONFIG_s_ {
    struct {
        UINT32  Function[3];               /* GPIO or alternate function */
        UINT32  Direction;              /* Input or output */
        UINT32  Mask;                   /* Read-only or read-writeable */
        UINT32  State;                  /* Logical zero or logical one */
        UINT32  PullEnable;             /* Disable/enable internal resistance */
        UINT32  PullSelect;             /* Internal resistance is pull-down or pull-up */
        UINT32  DriveStrength[2];       /* Driving strength is 3mA/6mA/12mA/18mA */
    } PinGroup[AMBA_NUM_GPIO_GROUP];
} AMBA_GPIO_PIN_GROUP_CONFIG_s;

typedef void (*AMBA_GPIO_ISR_f)(AMBA_GPIO_PIN_ID_e GpioPinID);

#endif /* _AMBA_GPIO_DEF_H_ */
