/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCardManager.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for simple card manager APIs.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_SCM_H_
#define _AMBA_SCM_H_

#ifndef SCM_MAX_CARDCTR
#define SCM_MAX_CARDCTR                 26
#endif

#ifndef SCM_MAX_EVENT_HANDLER
#define SCM_MAX_EVENT_HANDLER           8
#endif

#ifndef SCM_TASK_PRIORITY
#define SCM_TASK_PRIORITY               64
#endif

#ifndef SCM_MSG_QUE_MAX
#define SCM_MSG_QUE_MAX                 10
#endif

/* The following are 'slot' ID definitions. */
#define SCM_SLOT_FL0                    0   /* NAND Flash:        (A) */
#define SCM_SLOT_FL1                    1   /* NAND Flash:        (B) */
#define SCM_SLOT_SD0                    2   /* SD/MMC controller: (C) */
#define SCM_SLOT_SD1                    3   /* SD/MMC controller: (D) */
#define SCM_SLOT_RD                     4   /* Ramdisk:           (E) */
#define SCM_SLOT_SD0_E                  4   /* SD/MMC controller: (E) */
#define SCM_SLOT_IDX                    5   /* Media Partition:   (F) */
#define SCM_SLOT_PRF                    6   /* Media Partition:   (G) */
#define SCM_SLOT_CAL                    7   /* Media Partition:   (H) */
#define SCM_SLOT_USB                    8   /* USB HOST:          (I) */
#define SCM_SLOT_USB                    8   /* USB HOST:          (I) */
#define SCM_SLOT_USB                    8   /* USB controller     (I) */
#define SCM_SLOT_SDIO                   11  /* SD/SDIO:           (L) */
#define SCM_SLOT_RF1                    24  /* DSP-ROMFS:         (Y) */
#define SCM_SLOT_RF2                    25  /* ROM-ROMFS:         (Z) */

#define SCM_SLOT_CARDS                  8   /* Number of active cards */

/* Supported events (of interest) */
#define SCM_CARD_INSERTED               (1 << 0)   /* Card inserted into slot event */
#define SCM_CARD_EJECTED                (1 << 1)   /* Card ejected from slot event */
#define SCM_CARD_SUSPENDED              (1 << 2)   /* Card in slot suspended event */
#define SCM_CARD_RESUMED                (1 << 3)   /* Card in slot resumed event */
#define SCM_CARD_IO_ERROR               (1 << 4)   /* Card IO error! */
#define SCM_FATAL_ERROR                 (1 << 5)   /* Card/Controller fatal error! */
#define SCM_CARD_HOT_IN                 (1 << 6)   /* Hot-plug insertion */
#define SCM_CARD_HOT_OUT                (1 << 7)   /* Hot-plug ejection */
#define SCM_CARD_MOUNT                  (1 << 8)   /* Card mount event */
#define SCM_CARD_UNMOUNT                (1 << 9)   /* Card unmount event */

/*-----------------------------------------------------------------------------------------------*\
 * Card slot status containing useful information for applications
 * interested in querying a slot.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SCM_STATUS_ {
    UINT8   CardPresent;        /* Whether a card is present */
    UINT8   BusWidth;           /* Data bus width */
    UINT8   WriteProtect;       /* Whether card is Write protected */
    UINT8   OS;                 /* Local/remote OS */

    UINT32  CardType;           /* Type of card */
    UINT32  ManfID;             /* Manufacture ID */
    UINT32  Version;            /* Version */
    char    Name[64];           /* Name of card (if any) */

    int     Format;             /* Format of card */
    UINT32  SecsCnt;            /* Number of sectors */
    UINT16  SecSize;            /* Sector size */
    char    Speed[16];          /* Current card speed */

    void    *Extra;             /* pointer to extra infomation */
} AMBA_SCM_STATUS_s;

/* Card types could be combo cards! */
#define SCM_CARD_MEM            0x1     /* Memory card */
#define SCM_CARD_IO             0x2     /* IO card */
#define SCM_CARD_MECH           0x4     /* Mechanical (not solid state) */
#define SCM_CARD_SD             0x8     /* SD card */
#define SCM_CARD_MMC            0x10    /* MMC card */
#define SCM_CARD_SDHC           0x20    /* SDHC card */
#define SCM_CARD_SDXC           0x40    /* SDXC card */

#define OS_LOCAL                0
#define OS_REMOTE               1

/*-----------------------------------------------------------------------------------------------*\
 * Meta data describing basic information about a lot.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SCM_SLOT_INFO_ {
    int     Slot;
    char    DriveLetter;
    char    Name[64];
} AMBA_SCM_SLOT_INFO;

/*-----------------------------------------------------------------------------------------------*\
 * Data structure containing service functions that need to be provided to the
 * SCM by the device drivers.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SCM_DEV_ {
    char Name[64];
	int  Slot;
    /*
     * The following functions are device-specific implementations
     * of various functions that users of the scardmgr can operate on.
     */
    int (*Read)(int Id, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
    int (*Write)(int Id, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);

    int (*IoStatSet)(int Id, int StatisticEnable);
    int (*IoStatGet)(int Id, UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt, UINT32 *pRlvl, UINT32 *pWlvl);
    int (*IoStat2ndSet)(int Id, int StatisticEnable);
    int (*IoStat2ndGet)(int Id, UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt, UINT32 *pRlvl, UINT32 *pWlvl);

    int (*Reinit)(int Id, int Mode);
    int (*Insert)(int Id);
    int (*Eject)(int Id);
    int (*Sync)(int Id);
    int (*GetStatus)(int Id, AMBA_SCM_STATUS_s *pStatus);
} AMBA_SCM_DEV_s;

/*-----------------------------------------------------------------------------------------------*\
 * Function prototype for event call-back function.
\*-----------------------------------------------------------------------------------------------*/
typedef void (*AmbaSCM_EeventHandler)(int Slot, int EID);

/*-----------------------------------------------------------------------------------------------*\
 * An object used by the SCM module and task.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_SCM_CTRL_ {
    /* Map a slot ID to a controller device driver handler. */
    struct {
        int             Slot;
        AMBA_SCM_DEV_s  *pDevCtrl;
    } CardCtrl[SCM_MAX_CARDCTR];

    /* Table of event handnlers */
    AmbaSCM_EeventHandler   EvtHandler[SCM_MAX_EVENT_HANDLER];

    AMBA_KAL_SEM_t          MutexSem;
    AMBA_KAL_SEM_t          HandlerMutexSem;
} AMBA_SCM_CTRL_s;

void AmbaSCM_DeviceRegister(int Slot, AMBA_SCM_DEV_s *pDevice);
void AmbaSCM_DeviceUnregister(int Slot);
void AmbaSCM_Register(AmbaSCM_EeventHandler Handler);
void AmbaSCM_Unregister(AmbaSCM_EeventHandler Handler);
void AmbaSCM_DispatchEvent(int Slot, int Eid);
int AmbaSCM_GetValidSlots(void);
int AmbaSCM_GetAllSlotsInfo(AMBA_SCM_SLOT_INFO *pInfo, int NumInfo);
int AmbaSCM_GetSlotStatus(int SlotID, AMBA_SCM_STATUS_s *pStatus);
int AmbaSCM_IoStatSet(int SlotID, int Enable);
int AmbaSCM_IoStatGet(int SlotID, UINT64 * pRb, UINT64 * pWb, UINT32 * pRt, UINT32 * pWt, UINT32 * pRlvl, UINT32 * pWlvl);
int AmbaSCM_IoStat2ndSet(int SlotID, int enable);
int AmbaSCM_IoStat2ndGet(int SlotID, UINT64 * pRb, UINT64 * pWb, UINT32 * pRt, UINT32 * pWt, UINT32 * pRlvl, UINT32 * pWlvl);
int AmbaSCM_Read(int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaSCM_Write(int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaMSC_Read(int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaMSC_Write(int SlotID, UINT8 *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaSCM_Reinit(int SlotID, int InitMode);
int AmbaSCM_Insert(int SlotID);
int AmbaSCM_Eject(int SlotID);
int AmbaSCM_Init(void);

#endif /* _AMBA_SCM_H_ */
