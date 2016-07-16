/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSD_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella SD Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SD_DEF_H_
#define _AMBA_SD_DEF_H_

typedef struct _AMBA_SD_CSD_ {
    UINT8   CsdStructure;
    UINT8   SpecVers;
    UINT8   TAAC;
    UINT8   NSAC;
    UINT8   TranSpeed;
    UINT16  CCC;
    UINT8   ReadBLLen;
    UINT8   ReadBLPartial;
    UINT8   WriteBlkMisalign;
    UINT8   ReadBlkMisalign;
    UINT8   DsrImp;
    UINT32  CSize;
    UINT8   VddReadCurrMin;
    UINT8   VddReadCurrMax;
    UINT8   VddWriteCurrMin;
    UINT8   VddWriteCurrMax;
    UINT8   CSizeMult;
    /** The 'erase' field is different for different specs */
    union {
        /** MMC system specification version 3.1 */
        struct {
            UINT8   EraseGrpSize;
            UINT8   EraseGrpMult;
        } MmcV31;
        /** MMC system specification version 2.2 */
        struct {
            UINT8   SectorSize;
            UINT8   EraseGrpSize;
        } MmcV22;
        /** SD-Memory specification version 1.01) */
        struct {
            UINT8   EraseBlkEN;
            UINT8   SectorSize;
        } SDMem;
    } Erase;
    UINT8   WpGrpSize;
    UINT8   WpGrpEnable;
    UINT8   DefaultEcc;
    UINT8   R2WFactor;
    UINT8   WriteBLLen;
    UINT8   WriteBLPartial;
    UINT8   FileFormatGrp;
    UINT8   Copy;
    UINT8   PermWriteProtect;
    UINT8   TmpWriteProtect;
#define SD_CSD_PERM_WP  0x4
#define SD_CSD_TMP_WP   0x2
#define HW_WP       0x1
    UINT8   FileFormat;
} AMBA_SD_CSD;

typedef struct _AMBA_SD_EXT_CSD_ {
    /** Modes segment */
    struct {
        UINT8   Rsv0[177];
        UINT8   BootBusWidth;
        UINT8   Rsv7;
        UINT8   BootConfig;
        UINT8   Rsv6;
        UINT8   ErasedMemoryContent;
        UINT8   Rsv1;
        UINT8   BusWidth;
        UINT8   Rsv2;
        UINT8   HSTiming;
        UINT8   Rsv3;
        UINT8   PowerClass;
        UINT8   Rsv4;
        UINT8   CmdSetRev;
        UINT8   Rsv5;
        UINT8   CmdSet;
    } Modes;

    /** Properties segment */
    struct {
        UINT8   ExtCsdRev;
        UINT8   Rsv0;
        UINT8   CsdStructure;
        UINT8   Rsv1;
        UINT8   CardType;
#define HIGH_SPEED_MMC_26MHZ    0x1
#define HIGH_SPEED_MMC_52MHZ    0x2
        UINT8   Rsv2[3];
        UINT8   PwrCL52195;
        UINT8   PwrCL26195;
        UINT8   PwrCL52360;
        UINT8   PwrCL26360;
        UINT8   Rsv3;
        UINT8   MinPerfRD4_26;
        UINT8   MinPerfWR4_26;
        UINT8   MinPerfRD8_26_4_52;
        UINT8   MinPerfWR8_26_4_52;
        UINT8   MinPerfRD8_52;
        UINT8   MinPerfWR8_52;
        UINT8   Rsv4;
        UINT32  SecCnt;
        UINT8   Rsv5[288];
        UINT8   SCmdSet;
        UINT8   Rsv6[7];
    } Properties;
} AMBA_SD_EXT_CSD;

/*-----------------------------------------------------------------------------------------------*\
 * CARD definitions                                                                              *
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * The CID register value stored on the SD/MMC memory card.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SD_CID_ {
    UINT8   Mid;        /**< Manufacturer ID */
    UINT16  Oid;        /**< OEM/Application ID */
    char    Pnm[5];     /**< Product name */
    UINT8   Prv;        /**< Product revision */
    UINT32  Psn;        /**< Product serial number */
    UINT8   MDateYear;      /**< Manufacture date (year) */
    UINT8   MDateMth;      /**< Manufacture date (month) */
} AMBA_SD_CID;

/**
 * Status related to SD Memory Card proprietary features.
 */
typedef struct _AMBA_SD_SD_STATUS_ {
    UINT16  DatBusWidthSecureMode;
#define DAT_BUS_WIDTH_1BIT  0x0
#define DAT_BUS_WIDTH_4BIT  0x2
#define SD_CARD_IN_SECURED_MODE 0x1 /**< If card is in secure mode op. */
    UINT16  SdCardType;     /**< Type of SD memory card */
#define SD_CARD_TYPE_REGULAR    0x0
#define SD_CARD_TYPE_SDROM  0x1
#define SD_CARD_TYPE_OTP    0x2
    UINT32  SizeOFProtectedArea;    /**< Size of protected area */
    UINT8   SpeedClass;
#define SD_SPEED_CLASS_0    0x0
#define SD_SPEED_CLASS_2    0x1
#define SD_SPEED_CLASS_4    0x2
#define SD_SPEED_CLASS_6    0x3
#define SD_SPEED_CLASS_10   0x4
    UINT8   PerformanceMove;
    UINT8   AuSize;
    UINT16  EraseSize;
    UINT8   EraseTimeoutOffset;
    UINT8   Rsv[50];
} AMBA_SD_SD_STATUS;

/**
 * Unpacked data structure for storing the SCR register content.
 */
typedef struct _AMBA_SD_SCR_ {
    UINT8   ScrStructure;       /**< SCR structure */
    UINT8   SdSpec;             /**< SD Mem Card - Spec. Version */
    UINT8   DataStatAfterErase; /**< Data status after release */
    UINT8   SdSecurity;         /**< SD security support */
    UINT8   SdBusWidth;         /**< SD bus width */
    UINT8   SdSpec3;            /**< SD Mem Card - Spec. Version3 */
    UINT8   CmdSupport;         /**< Support bits of CMD20 and CMD23 */
#define SD_SUPPORT_CMD20    0x1
#define SD_SUPPORT_CMD23    0x2
} AMBA_SD_SCR;

/** Switch command (spec 3.x) command) result */
#define SD_UHS_DDR50_SUPPORT   0x10
#define SD_UHS_SDR104_SUPPORT  0x8
#define SD_UHS_SDR50_SUPPORT   0x4
#define SD_UHS_SDR25_SUPPORT   0x2
#define SD_UHS_SDR12_SUPPORT   0x1

#define SD_DRV_TYPE_D_SUPPORT   0x8
#define SD_DRV_TYPE_C_SUPPORT   0x4
#define SD_DRV_TYPE_A_SUPPORT   0x2
#define SD_DRV_TYPE_B_SUPPORT   0x1

#define SD_CUR_800MA_SUPPORT   0x8
#define SD_CUR_600MA_SUPPORT   0x4
#define SD_CUR_400MA_SUPPORT   0x2
#define SD_CUR_200MA_SUPPORT   0x1

#define SD_UHS_DDR50_MODE   0x4
#define SD_UHS_SDR104_MODE  0x3
#define SD_UHS_SDR50_MODE   0x2
#define SD_UHS_SDR25_MODE   0x1
#define SD_UHS_SDR12_MODE   0x0

#define SD_DRV_STG_TYPE_D   0x3
#define SD_DRV_STG_TYPE_C   0x2
#define SD_DRV_STG_TYPE_A   0x1
#define SD_DRV_STG_TYPE_B   0x0

#define SD_CUR_800MA        0x3
#define SD_CUR_600MA        0x2
#define SD_CUR_400MA        0x1
#define SD_CUR_200MA        0x0

typedef struct __AMBA_SD_SWITCH_CMD__ {
    UINT32 Mode;
    UINT32 DrvStg;  /**< Driver Strength for UHS-I mode */
    UINT32 CurLimit;    /**< Current limit for UHS-I mode */
} AMBA_SD_SWITCH_CMD;

/**
 * MMC/SD (logical) device.
 */
typedef struct _AMBA_SD_CARD_ {
    UINT8   ID;     /**< Card ID, 0-SD */
#define SD_CARD                 0

    UINT32  Ocr;                /**< Operation condition Register */
    AMBA_SD_CID     Cid;    /**< Card Identification Register */
    AMBA_SD_CSD     Csd;    /**< Card Specific Register */
    AMBA_SD_EXT_CSD ExtCsd;
    UINT8   Rsv[16];
    UINT16  Rca;            /**< Relative Card Address */
    UINT32  Dsr;            /**< Driver Stage Register */
    AMBA_SD_SCR Scr;    /**< SD card Configuration Register */

    /* SDIO-specific variables */
    UINT8   Nio;            /**< Number of IO functions */
    UINT8   MemoryPresent;  /**< Memory present */
    UINT16  FNBlkSZ[8];     /**< Function block sizes */
    UINT32  CisPtr[8];      /**< CIS pointers */

    /* Other card info */
    UINT32  TaacNS;         /**< Data access time in ns */
    UINT32  TaacClks;       /**< Data access time in clks */
    UINT32  MaxDtr;         /**< Maximum data transfer rate */
    UINT64  Capacity;       /**< Card capacity */
    INT32   MMC4BusTest;    /**< Mask for MMC4 bus test */
#define MMC4_BUS_TEST_FAIL_8    0x2
#define MMC4_BUS_TEST_FAIL_4    0x1
    INT32   DesiredClock;   /**< Desired SD_CLK in Khz */
#define SD_CLK_DECREASE 4000000 /**< Reduce 4Mhz one time */

    INT32   ActualClock;    /**< Actual clock of current card */
    INT32   FClock;     /**< User defined SD_CLK */
    INT32   BusWidth;      /**< Data bus width */
    INT32   HighSpeed;      /**< High speed (MMC 4.0) */
    UINT32  Phase;          /**< phase steps for tuning command */
    UINT16  Vdd;            /**< Current working voltage */
#define MAX_SD_INSTANCE 2
    INT32   Format[MAX_SD_INSTANCE];     /**< File system format ID */
    UINT32  Hcs;        /**< Host capacity support */
    UINT32  Ccs;        /**< 0: Standard, 1:High capacity */
    UINT32  Uhs;        /**< UHS-I mode 1.8V supported */
    UINT32  Mode;       /**< Current data bus mode */
#define SD_SDR_MODE 0x0
#define SD_DDR_MODE 0x1
    /** Switch command (spec 3.x) command) result */
    AMBA_SD_SWITCH_CMD  SwitchCmd;
    UINT32  DrvStg;     /**< Driver Strength for UHS-I mode */
    UINT32  CurLimit;   /**< Current limit for UHS-I mode */
    UINT32  AddrMode;  /**< Address mode: byte/sector */
#define SD_ADDR_MODE_BYTE   0
#define SD_ADDR_MODE_SECTOR 1
    UINT32  MoviNand;

    /* States */
    UINT32  InitLoop;   /**< Initialization loop count */
    UINT8   PseudoEject;/**< State: card is psudo eject */
    UINT8   Present;    /**< State: card is present */
    UINT8   WP;         /**< State: write protect */
    UINT8   Suspend;    /**< State: card is suspend */
    UINT8   IsInit;     /**< State: card is init */
    UINT8   IsMmc;      /**< State: card is MMC (mutually exclusive with SD) */
    UINT8   IsSdmem;    /**< State: card is SDMem (could be combo with SDIO) */
    UINT8   IsSdio;     /**< State: card is SDIO (could be combo with SDMem) */
    UINT32  Status;     /**< State: status */
#define SD_STATE_INA        0xff
#define SD_STATE_IDLE       0
#define SD_STATE_READY      1
#define SD_STATE_IDENT      2
#define SD_STATE_STBY       3
#define SD_STATE_TRAN       4
#define SD_STATE_DATA       5
#define SD_STATE_RCV        6
#define SD_STATE_PRG        7
#define SD_STATE_DIS        8
    UINT8   IsBusy;
    UINT8   SccCtrl;    /**< Is in SDXC speed class ctrl */
    UINT32  RawScr[2];
    AMBA_SD_SD_STATUS       Ssr;
#define SAVE_RESP_NUM       5
    struct CmdResp_s {
        UINT32 OpCode;
        UINT32 Resp[4];
    } CmdResp[SAVE_RESP_NUM];
} AMBA_SD_CARD;

/* ======================================================================== */
/* ------------------------------------------------------------------------ */
/* HOST controller definitions                                              */
/* ------------------------------------------------------------------------ */
/* ======================================================================== */

/**
 * Basic configuration of the host controller.
 */
typedef struct _AMBA_SD_IOS_ {
    UINT32  DesiredClock;   /**< The desired clock rate - in KHZ */
#define SD_INIT_CLK     300
    UINT32  ActualClock;    /**< The actual clock rate - in HZ */
    UINT16  Vdd;            /**< VDD value */
#define SD_VDD_150  0   /**< 1.50 V */
#define SD_VDD_155  1   /**< 1.55 V */
#define SD_VDD_160  2   /**< 1.60 V */
#define SD_VDD_165  3   /**< 1.65 V */
#define SD_VDD_170  4   /**< 1.70 V */
#define SD_VDD_180  5   /**< 1.80 V */
#define SD_VDD_190  6   /**< 1.90 V */
#define SD_VDD_200  7   /**< 2.00 V */
#define SD_VDD_210  8   /**< 2.10 V */
#define SD_VDD_220  9   /**< 2.20 V */
#define SD_VDD_230  10  /**< 2.30 V */
#define SD_VDD_240  11  /**< 2.40 V */
#define SD_VDD_250  12  /**< 2.50 V */
#define SD_VDD_260  13  /**< 2.60 V */
#define SD_VDD_270  14  /**< 2.70 V */
#define SD_VDD_280  15  /**< 2.80 V */
#define SD_VDD_290  16  /**< 2.90 V */
#define SD_VDD_300  17  /**< 3.00 V */
#define SD_VDD_310  18  /**< 3.10 V */
#define SD_VDD_320  19  /**< 3.20 V */
#define SD_VDD_330  20  /**< 3.30 V */
#define SD_VDD_340  21  /**< 3.40 V */
#define SD_VDD_350  22  /**< 3.50 V */
#define SD_VDD_360  23  /**< 3.60 V */

    UINT8   BusMode;    /**< command output mode */
#define SD_BUSMODE_OPENDRAIN    1
#define SD_BUSMODE_PUSHPULL 2

    UINT8   PowerMode;  /**< power supply mode */
#define SD_POWER_OFF    0
#define SD_POWER_UP     1
#define SD_POWER_ON     2
} AMBA_SD_IOS;

/**
 * FIXME: the judgement of enabling Ambalink is changed from compiling time to runtime.
 * MAX_SD_SLOT and SD_INSTANCES should do the same change.
 */
#define MAX_SD_SLOT         2
#define SD_INSTANCES        2

//#define MAX_SD_HOST         SD_INSTANCES
#define MAX_SD_HOST         1

/**
 * SD/MMC command abstraction. This data structure is used when sending a
 * a command to the SD host controller.
 */
typedef struct _AMBA_SD_COMMAND_ {
    UINT32      OpCode;     /**< SD/MMC command code */
    UINT32      Arg;        /**< Argument */
    UINT32      Resp[4];    /**< Response from card */
    UINT32      Expect;     /**< Expected response type */
#define SD_EXPECT_NONE      0
#define SD_EXPECT_R1        1
#define SD_EXPECT_R1B       2
#define SD_EXPECT_R2        3
#define SD_EXPECT_R3        4
#define SD_EXPECT_R4        5
#define SD_EXPECT_R5        6
#define SD_EXPECT_R5B       7
#define SD_EXPECT_R6        8
#define SD_EXPECT_R7        9
    UINT32      Retries;    /**< Max number of retries */
    UINT32      Timeout;    /**< Timeout to wait for command/data */
    INT32       Error;      /**< Command error code */
} AMBA_SD_COMMAND;

/**
 * Data transfer. This data structure is used when sending a command to the
 * SD host controller and a data is expected to transferred on the DAT line
 * in association with the command.
 */
typedef struct _AMBA_SD_DATA_ {
    UINT8       *pBuf;      /**< System memory buffer */
    UINT8       *pAlignBuf; /**< Alignment Buffer  */
    UINT8       Misalign;
    UINT32      TimeoutNS;  /**< Data timeout (in ns, max 80ms) */
    UINT32      TimeoutClks;/**< Data timeout (in clocks) */
    UINT32      BlkSize;    /**< Data block size */
    UINT32      Blocks;     /**< Number of blocks */
    INT32       Error;      /**< Data error */
    UINT32      Flags;      /**< Data transfer flags */
#define SD_DATA_READ        (0 << 1)    /**< Data read (from card) */
#define SD_DATA_WRITE       (1 << 1)    /**< Data write (to card) */
#define SD_DATA_STREAM      (1 << 2)    /**< Stream access (ACMD12) */
#define SD_DATA_1BIT_WIDTH  (0 << 3)    /**< 1 DAT line */
#define SD_DATA_4BIT_WIDTH  (1 << 3)    /**< 4 DAT lines */
#define SD_DATA_8BIT_WIDTH  (2 << 3)    /**< 8 DAT lines */
#define SD_DATA_HIGH_SPEED  (1 << 5)    /**< High speed mode */
    UINT32      BytesXfered;    /**< The actual bytes transferred */
} AMBA_SD_DATA;

/**
 * This data structure represents a requst for a transaction to be issued by
 * the host controller to the card.
 */
typedef struct _AMBA_SD_REQUEST_ {
    AMBA_SD_COMMAND *pCmd;  /**< transaction command */
    AMBA_SD_COMMAND *pStop; /**< transaction end command */
    AMBA_SD_DATA    *pData; /**< data to be transferred */
} AMBA_SD_REQUEST;

/**
 * Host controller properties and state.
 */
typedef struct _AMBA_SD_HOST_ {
    void *pAmba_SDReg;   /**< SDHC register base address */

    UINT32  FreqMin;
    UINT32  FreqMax;
    UINT32  FreqMaxUhs;
#define SD_UHS_MAX_FREQ 100000000
    UINT32  OcrAvail;
    UINT32  SupportPllScaler;
    UINT32  SupportSdxc;
    int     FixCd;
    INT32   ID;     /**< SD host instance ID */
#define SD_HOST_0   0
#define SD_HOST_1   1
#define SD_HOST_0_E 2
#define SD_HOST_2   3
#define MAX_SD_HOST_ID 4
    /* host specific block data */
    UINT32  MaxSegSize; /**< see blk_queue_max_segment_size */
    UINT16  MaxHwSegs;  /**< see blk_queue_max_hw_segments */
    UINT16  MaxPhysSegs;/**< see blk_queue_max_phys_segments */
    UINT16  MaxSectors; /**< see blk_queue_max_sectors */
    UINT16  Unused;

    /* private data */
    AMBA_SD_IOS     Ios;    /**< current io bus settings */
    /** cards controlled by host */
    AMBA_SD_CARD    Card;

    INT32   Cards;              /**< total number of active cards */
    UINT16  Rca;                /**< the current active card's RCA */
    AMBA_SD_REQUEST *pMrq;  /**< saved request */

    INT32   FlgID;              /**< event flag to use with ISR */
    UINT32  RegBase;            /**< SDHC register base address */
    UINT32  IrqNumber;          /**< IRQ number */

    void    (*SetPwr33V)(void);
    void    (*SetPwr18V)(void);
    int     (*SetPll)(UINT32);
    UINT32  (*GetPll)(void);

} AMBA_SD_HOST;

/**
 * The switch function status is the returned data block that contains function
 * and current consumption information. The block length is predefined to
 * 512 bits and the use of SET_BLK_LEN command is not necessary.
 */
typedef struct _AMBA_SD_SW_FUNC_STATUS_ {
    UINT16 MaxCurrent;
    UINT16 SupportGroup6;
    UINT16 SupportGroup5;
    UINT16 SupportGroup4;
    UINT16 SupportGroup3;
    UINT16 SupportGroup2;
    UINT16 SupportGroup1;
    UINT8  StatusGroup6 : 4;
    UINT8  StatusGroup5 : 4;
    UINT8  StatusGroup4 : 4;
    UINT8  StatusGroup3 : 4;
    UINT8  StatusGroup2 : 4;
    UINT8  StatusGroup1 : 4;
    UINT8  StructVersion;
    UINT16 BusyGroup6;
    UINT16 BusyGroup5;
    UINT16 BusyGroup4;
    UINT16 BusyGroup3;
    UINT16 BusyGroup2;
    UINT16 BusyGroup1;
    UINT8  Rsv1[34];
} __attribute__((packed))AMBA_SD_SW_FUNC_STATUS;

typedef struct _AMBA_SD_SW_FUNC_SWITCH_ {
    UINT8 Rsv2;
    UINT8 Rsv1;
    UINT8 CurLimit;
    UINT8 DrvStg;
    UINT8 CmdSys;
    UINT8 AccessMode;
} __attribute__((packed))AMBA_SD_SW_FUNC_SWITCH;

typedef struct _AMBA_SD_LOCK_ {
    UINT8 Operation;
    UINT8 PwdsLen;
    UINT8 *pPWData;
} AMBA_SD_LOCK;

void AmbaUserSD_PowerCtrl(UINT32 Enable) __attribute__((weak));
void AmbaUserSDIO_PowerCtrl(UINT32 Enable) __attribute__((weak));

void AmbaUserSD_VoltageCtrl(UINT32 Level) __attribute__((weak));
void AmbaUserSDIO_VoltageCtrl(UINT32 Level) __attribute__((weak));
#define SD_POWER_CTRL_33V   0
#define SD_POWER_CTRL_18V   1

void AmbaUserSD_DelayCtrl(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_CID CID) __attribute__((weak));

typedef enum _AMBA_SD_DRIVING_STRENGTH_VALUE_e_ {
    AMBA_SD_DRIVING_STRENGTH_3MA = 0,
    AMBA_SD_DRIVING_STRENGTH_12MA,
    AMBA_SD_DRIVING_STRENGTH_6MA,
    AMBA_SD_DRIVING_STRENGTH_18MA
} AMBA_SD_DRIVING_STRENGTH_VALUE_e;

typedef enum _AMBA_SD_DRIVING_STRENGTH_TYPE_e_ {
    AMBA_SD_DRIVING_STRENGTH_CLK = 0,
    AMBA_SD_DRIVING_STRENGTH_DATA,
    AMBA_SD_DRIVING_STRENGTH_CMD,
    AMBA_SD_DRIVING_STRENGTH_CDWP,

    AMBA_SD_DRIVING_STRENGTH_ALL
} AMBA_SD_DRIVING_STRENGTH_TYPE_e;

typedef enum _AMBA_SD_PHY_PARAM_e_ {
    AMBA_SD_PHY_AUTO_CMD_BYPASS_EN = 0,
    AMBA_SD_PHY_CLK_OUT_BYPASS,
    AMBA_SD_PHY_RESET,
    AMBA_SD_PHY_CLK270_ALONE,
    AMBA_SD_PHY_RX_CLK_POL,
    AMBA_SD_PHY_DATA_CMD_BYPASS,
    AMBA_SD_PHY_DLL_BYPASS
} AMBA_SD_PHY_PARAM_e;

typedef enum _AMBA_SD_PHY_SBC_e_ {
    AMBA_SD_PHY_SBC_PWRDWNSHFT = 0,
    AMBA_SD_PHY_SBC_FORCELOCK,
    AMBA_SD_PHY_SBC_LOCKRANGE,
    AMBA_SD_PHY_SBC_COARSEDLY,
    AMBA_SD_PHY_SBC_ENABLEDLL
} AMBA_SD_PHY_SBC_e;

typedef enum _AMBA_SD_PHY_SEL_e_ {
    AMBA_SD_PHY_SEL0 = 0,
    AMBA_SD_PHY_SEL1,
    AMBA_SD_PHY_SEL2,
    AMBA_SD_PHY_SEL3
} AMBA_SD_PHY_SEL_e;


#endif  /* _AMBA_SD_DEF_H_ */
