/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSysCtrl.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Constants and Definitions for System Control
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SYS_CTRL_H_
#define _AMBA_SYS_CTRL_H_

typedef struct _AMBA_SYS_VERSION_INFO_s_ {
    UINT8   Major;              /* Major number */
    UINT8   Minor;              /* Minor number */
    UINT16  Svn;                /* SVN serial number */
    char    *pBuildDateTime;    /* pointer to the build date and time of SYS Lib */
} AMBA_SYS_VERSION_INFO_s;

typedef int (*AMBA_SYS_INFO_PRINT_f) (const char *pFmtData, ...);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSysInit.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SYS_VERSION_INFO_s AmbaSYS_VersionInfo;

AMBA_SYS_VERSION_INFO_s *AmbaGetSysVerInfo(void);

int AmbaSysIsWarmBoot(void);
void AmbaSysSoftReset(void);
void AmbaSysPowerDown(void);
void AmbaSysHotboot(UINT32 pattern);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaExceptionHandler.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaUserDefinedExceptionHandlerRegister(VOID_UINT32_IN_FUNCTION pUserDefinedExceptionHandler);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaUserSysCtrl.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaUserSysInitPreOS(void);                /* User defined System Initializations (before OS running) */
void AmbaUserSysInitPostOS(void);               /* User defined System Initializations (after OS running) */

#define BOOT_FROM_NOR       0x0
#define BOOT_FROM_NAND      0x1
#define BOOT_FROM_EMMC      0x2
#define BOOT_FROM_EEPROM    0x3

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRootTask.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaIdleTaskCallbackFunctionRegister(VOID_FUNCTION pCallbackFunction);
void AmbaIdleTaskIntervalTimeSet(UINT16 IntervalTime);

UINT32 AmbaCheckSysStatus(UINT32 SysStatusFlag);
UINT32 AmbaWaitSysStatus(UINT32 SysStatusFlag, UINT32 Options, UINT32 Timeout);
int AmbaResetAllSysStatus(void);

#define AMBA_SYS_STATUS_IDLE_TIME_OUT_FLAG          (1 << 0)    /* the flag of Idle time out: bit-0 */
#define AMBA_SYS_STATUS_TIMER_TIME_OUT_FLAG         (1 << 1)    /* the flag of System Timer time out: bit-1 */
#define AMBA_SYS_STATUS_HDMI_SINK_DISCONNECT_FLAG   (1 << 2)    /* the flag of HDMI HPD signal is at low voltage. Use DDC bus to read EDID is not allowed: bit-2 */
#define AMBA_SYS_STATUS_HDMI_SINK_POWER_OFF_FLAG    (1 << 3)    /* the flag of HDMI HPD signal is at high voltage. TMDS Receiver is not detected by RxSense: bit-3 */
#define AMBA_SYS_STATUS_HDMI_SINK_POWER_ON_FLAG     (1 << 4)    /* the flag of HDMI HPD signal is at high voltage. TMDS Receiver is detected by RxSense: bit-4 */
#define AMBA_SYS_STATUS_HDMI_CEC_TX_OK_FLAG         (1 << 5)    /* the flag of successful transmitting CEC frame: bit-5 */
#define AMBA_SYS_STATUS_HDMI_CEC_TX_FAIL_FLAG       (1 << 6)    /* the flag of failed transmitting CEC frame (after retry fail): bit-6 */
#define AMBA_SYS_STATUS_HDMI_CEC_RX_FLAG            (1 << 7)    /* the flag of successful receiving CEC frame (frame is available): bit-7 */
#define AMBA_SYS_STATUS_IR_DATA_AVAIL_FLAG          (1 << 8)    /* the flag of infraray remote data available : bit-8 */

#define HOTBOOT_MEM_ADDR        (AMBA_CORTEX_A9_DRAM_VIRT_BASE_ADDR + 0x800)
#define HOTBOOT_PATTERN_ADDR    HOTBOOT_MEM_ADDR + 0x4
#define HOTBOOT_VALID_ADDR      HOTBOOT_MEM_ADDR + 0x8
#define HOTBOOT_MAGIC0_ADDR     HOTBOOT_MEM_ADDR
#define HOTBOOT_MAGIC1_ADDR     HOTBOOT_MEM_ADDR + 0xc
#define HOTBOOT_MAGIC0          0x14cd78a0
#define HOTBOOT_MAGIC1          0x319837fb
#define GetHotbootValid()       (*(volatile UINT32 *)(HOTBOOT_VALID_ADDR))
#define SetHotbootValid(val)    *(volatile UINT32 *)(HOTBOOT_VALID_ADDR) = (val)
#define GetHotbootPattern()     (*(volatile UINT32 *)(HOTBOOT_PATTERN_ADDR))
#define SetHotbootPattern(val)  *(volatile UINT32 *)(HOTBOOT_PATTERN_ADDR) = (val)
#define GetHotbootMagic0()      (*(volatile UINT32 *)(HOTBOOT_MAGIC0_ADDR))
#define GetHotbootMagic1()      (*(volatile UINT32 *)(HOTBOOT_MAGIC1_ADDR))

/* The following bit fields is a mask for "active" loading partitions */

#define HOTBOOT_SYS              0x02

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSysInfo.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaSysShowTaskInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowMutexInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowSemaphoreInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowEventFlagInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowMsgQueueInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowBytePoolInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowBlockPoolInfo(AMBA_SYS_INFO_PRINT_f PrintFunc);
int AmbaSysShowIrqInfo(AMBA_SYS_INFO_PRINT_f PrintFunc, int IntID);
void AmbaSysResetIrqCount(void);

#endif  /* _AMBA_SYS_CTRL_H_ */
