/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_Communicate.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Communication APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_COMMUNICATE_H_
#define _AMBA_B5_COMMUNICATE_H_

#define AMBA_B5_REG_READ    0
#define AMBA_B5_REG_WRITE   1

#define AMBA_B5_REG_ADDR_NO_INCREMENT   0
#define AMBA_B5_REG_ADDR_INCREMENT      1

#define AMBA_B5_PWM_POLLING_START       6   /* in msec, for replay_times=6 and symbol_clk=400KHz */
#define AMBA_B5_PWM_POLLING_TIMEOUT     100

typedef enum _AMBA_B5_DATA_WIDTH_e_ {
    AMBA_B5_DATA_WIDTH_8BIT = 0,
    AMBA_B5_DATA_WIDTH_16BIT,
    AMBA_B5_DATA_WIDTH_32BIT,
    AMBA_B5_DATA_WIDTH_RESERVED
} AMBA_B5_DATA_WIDTH_e;

typedef enum _AMBA_B5N_I2C_SLAVE_ADDRESS_e_ {
    AMBA_B5N_I2C_SLAVE_ADDR_00_WRITE = 0xA8,  /* B5N I2CS ID[1:0] = 2'b00 */
    AMBA_B5N_I2C_SLAVE_ADDR_00_READ  = 0xA9,  /* B5N I2CS ID[1:0] = 2'b00 */
    AMBA_B5N_I2C_SLAVE_ADDR_01_WRITE = 0xAA,  /* B5N I2CS ID[1:0] = 2'b01 */
    AMBA_B5N_I2C_SLAVE_ADDR_01_READ  = 0xAB,  /* B5N I2CS ID[1:0] = 2'b01 */
    AMBA_B5N_I2C_SLAVE_ADDR_10_WRITE = 0xAC,  /* B5N I2CS ID[1:0] = 2'b10 */
    AMBA_B5N_I2C_SLAVE_ADDR_10_READ  = 0xAD,  /* B5N I2CS ID[1:0] = 2'b10 */
    AMBA_B5N_I2C_SLAVE_ADDR_11_WRITE = 0xAE,  /* B5N I2CS ID[1:0] = 2'b11 */
    AMBA_B5N_I2C_SLAVE_ADDR_11_READ  = 0xAF,  /* B5N I2CS ID[1:0] = 2'b11 */
} AMBA_B5N_I2C_SLAVE_ADDRESS_e;

typedef union _AMBA_B5_COMMAND_u_ {
    UINT16  Data;

    struct {
        UINT16  IsAPB:          1;      /* [0]: 0 = AHB, 1 = APB */
        UINT16  DataWidth:      2;      /* [2:1]: Transfer Size: 0:Byte, 1:Halfword, 2:Word, 3: Special command */
        UINT16  DataSize:       7;      /* [9:3]: Transfer byte count by size */
        UINT16  AddrInc:        1;      /* [10]: 0 = Fixed address 1 = increment */
        UINT16  ReadOrWrite:    1;      /* [11]: 0 = Read, 1 = Write */
        UINT16  ChipID:         4;      /* [15:12]: Chip ID */
    } Bits;
} AMBA_B5_COMMAND_u;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_Communicate.c
\*---------------------------------------------------------------------------*/
extern AMBA_B5_CHIP_INTERFACE_e AmbaB5_ChipInterface;
extern UINT8 AmbaB5_DisablePollingMsg;

int AmbaB5_CommInit(void);

int AmbaB5_RegWrite(UINT16 ChipIdData, UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pTxDataBuf);
int AmbaB5_RegRead(UINT16 ChipIdData, UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pRxDataBuf);

int AmbaB5N_RegWrite(UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pTxDataBuf);
int AmbaB5N_RegRead(UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pRxDataBuf);
int AmbaB5F_RegWrite(AMBA_B5_CHIP_ID_u ChipID, UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pTxDataBuf);
int AmbaB5F_RegRead(AMBA_B5_CHIP_ID_u ChipID, UINT32 RegAddr, UINT8 IsIncAddr, AMBA_B5_DATA_WIDTH_e DataWidth, int DataSize, void *pRxDataBuf[4]);

void AmbaB5_SetPwmBrokenFlag(UINT8 Value);
UINT8 AmbaB5_GetPwmBrokenFlag(void);
UINT8 AmbaB5_GetConnectionStatus(void);

#endif /* _AMBA_B5_COMMUNICATE_H_ */
