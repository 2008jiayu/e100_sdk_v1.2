/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_FIO.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Run Time Support Library for NAND flash
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_FIO_H_
#define _AMBA_RTSL_FIO_H_

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for FIO modules.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FIO_NAND               0
#define AMBA_FIO_SD                 1
#define AMBA_FIO_SDIO               2
#define AMBA_FIO_SD2                3

/*-----------------------------------------------------------------------------------------------*\
 * Flash IO interrupt definitions.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FIO_CMD_INT            0x0
#define AMBA_FIO_DMA_INT            0x1

/*-----------------------------------------------------------------------------------------------*\
 * Flash IO DMA error code definitions.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FIO_DMA_READ_ER        -1
#define AMBA_FIO_DMA_ADDR_ER        -2
#define AMBA_FIO_DMA_UNKN_ER        -99

/*-----------------------------------------------------------------------------------------------*\
 * Flash IO Multi-bits ECC Report Register: Error code definitions.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FIO_ECC_CORRECTED      -1
#define AMBA_FIO_ECC_FAILED         -2
#define AMBA_FIO_ECC_UNKN           -99

/*-----------------------------------------------------------------------------------------------*\
 * Flash memory current transaction.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_FLASH_TX_s_ {
    UINT32              FlashType;
    UINT32              Cmd;

    struct {
        AMBA_NAND_HOST_s    *pNandHost;
        AMBA_NAND_DEV_s     *pNandDev;

        UINT32          Bank;
        UINT32          AddrHi;
        UINT32          Addr;
        UINT32          Dest;
        UINT8           *pMain;
        UINT8           *pSpare;
        UINT32          MainLen;
        UINT32          SpareLen;
        UINT32          Area;
    } Arg;

    UINT32              DescXferID;
    UINT32              EccRpt;
    UINT32              FioDmaStatus;
} AMBA_FLASH_TX_s;

#define FLASH_TX_NAND_TYPE      0

/*-----------------------------------------------------------------------------------------------*\
 * An instance of the Ambarella FIO flash controller.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RTSL_FIO_CTRL_s_ {
    UINT32              FlagID;     /**< Event flag */

#if !defined(CONFIG_NAND_NONE)
    AMBA_NAND_HOST_s    NandHost;
#endif

    AMBA_FLASH_TX_s     FlashTx;
} AMBA_RTSL_FIO_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_FIO.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_RTSL_FIO_CTRL_s g_AmbaRTSL_FioCtrl;

/* Call back function when completed transactions */
extern void (*AmbaRTSL_FioCmdIsrCallBack)(void);
extern void (*AmbaRTSL_FioDmaIsrCallBack)(void);

int     AmbaRTSL_FioIsRandomMode(void);
void    AmbaRTSL_FioExitRandomMode(void);
void    *AmbaRTSL_FioGetBuf(void);
void    AmbaRTSL_FioMemcpyFromAlign(void *pDst, void *pSrc, UINT32 Len);
void    AmbaRTSL_FioMemcpyToAlign(void *pDst, void *pSrc, UINT32 Len);
void    AmbaRTSL_FioWaitCmdInt(void);
void    AmbaRTSL_FioWaitDmaInt(void);
void    AmbaRTSL_FioRegReset(UINT32 Module);
void    AmbaRTSL_FioDmaSetupDev2Mem(AMBA_FLASH_TX_s *pFlashTx);
void    AmbaRTSL_FioDmaSetupDev2MemDsm(AMBA_FLASH_TX_s *pFlashTx);
void    AmbaRTSL_FioDmaSetupMem2Dev(AMBA_FLASH_TX_s *pFlashTx);
void    AmbaRTSL_FioDmaSetupMem2DevDsm(AMBA_FLASH_TX_s *pFlashTx);
int     AmbaRTSL_FioDmaParseStatus(void);
int     AmbaRTSL_FioParseEccRpt(AMBA_NAND_DEV_s *pDev, UINT8 *pSpare, UINT32 Pages);
void    AmbaRTSL_FioCmdDoneIsr(int IntID);
void    AmbaRTSL_FioDmaDoneIsr(int IntID);
void    AmbaRTSL_FioResetAll(void);
void    AmbaRTSL_FioInit(void);

#endif  /* _AMBA_RTSL_FIO_H_ */

