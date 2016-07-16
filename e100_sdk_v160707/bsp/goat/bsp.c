 /**
  * @file bsp.c
  *
  * BSP
  *
  * History:
  *    2015.10.14 - [Yuchi Wei] created file
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "bsp.h"
#include "AmbaSD_Def.h"
#include "AmbaNAND_Def.h"
#include "AmbaGPIO.h"
#include "AmbaUART.h"
#include "AmbaAudio_CODEC.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaPrintk.h"
#include "AmbaSD.h"


/** GPIO config */
AMBA_GPIO_PIN_GROUP_CONFIG_s GpioPinGrpConfig = {
    .PinGroup[AMBA_GPIO_GROUP0] = {
        .Function = {
            0xf800001e,
            0x00878000,
            0x07000000
        },
        .Direction = 0x003843e1,
        .Mask = 0xffffffff,
        .State = 0x000841e0,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    .PinGroup[AMBA_GPIO_GROUP1] = {
        .Function = {
            0x00bfffff,
            0xffc80000,
            0x00000000
       },
        .Direction = 0x00000000,
        .Mask = 0xffffffff,
        .State = 0x00000000,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
    .PinGroup[AMBA_GPIO_GROUP2] = {
        .Function = {
            0xfe000031,
            0x01ffffff,
            0x00000000
        },
        .Direction = 0x00000000,
        .Mask = 0xffffffff,
        .State = 0x00000000,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
#else
    .PinGroup[AMBA_GPIO_GROUP1] = {
        .Function = {
            0x000bffff,
            0xff800000,
            0x00000000
       },
        .Direction = 0x00340000,
        .Mask = 0xffffffff,
        .State = 0x00100000,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
    .PinGroup[AMBA_GPIO_GROUP2] = {
        .Function = {
            0xfe000000,
            0x01ffffff,
            0x00000000
        },
        .Direction = 0x00000000,
        .Mask = 0xffffffff,
        .State = 0x00000000,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
#endif
    .PinGroup[AMBA_GPIO_GROUP3] = {
        .Function = {
            0x0003ffff,
            0x00000000,
            0x00000000
        },
        .Direction = 0x00000000,
        .Mask = 0xffffffff,
        .State = 0x00000000,
        .PullEnable = 0x00000000,
        .PullSelect = 0x00000000,
        .DriveStrength = {
            [0] = 0xffffffff,
            [1] = 0x0
        },
    },
};
#if 0
AMBA_EXP_GPIO_CONFIG_s ExpGpioPinPortConfig = {
    .Port[AMBA_EXP_GPIO_PORT0] = {  /* EX_GPIO00 - EX_GPIO07 */
        .IoDirection = 0x30,
        .PinPolarity = 0x00,
        .PinLevel    = 0x87,
    },

    .Port[AMBA_EXP_GPIO_PORT1] = {  /* EX_GPIO10 - EX_GPIO17 */
        .IoDirection = 0xF7,
        .PinPolarity = 0x00,
        .PinLevel    = 0x08,
    },
};
#endif

const int AmbaFW_PartitionSize[AMBA_NUM_FW_PARTITION] = {
    /* 2KB fixed size. Can not be changed. */
    [AMBA_PARTITION_BOOTSTRAP]                    = AMBA_BOOTSTRAPE_SIZE,

    /* Firmware (System code) partitions */
    [AMBA_PARTITION_TABLE]                        = AMBA_PARTITION_TABLE_SIZE,
    [AMBA_PARTITION_BOOTLOADER]                   = AMBA_BOOTLOADER_SIZE,
    [AMBA_PARTITION_SD_FIRMWARE_UPDATE]           = AMBA_SD_FWUPDATE_SIZE,
    [AMBA_PARTITION_SYS_SOFTWARE]                 = AMBA_SYSTEM_SOFTWARE_SIZE,

    /* Firmware (Read-Only, uCode and ROM) partitions */
    [AMBA_PARTITION_DSP_uCODE]                    = AMBA_DSP_SIZE,
    [AMBA_PARTITION_SYS_ROM_DATA]                 = AMBA_ROM_SIZE,

    [AMBA_PARTITION_LINUX_KERNEL]                 = AMBA_LINUX_SIZE,
    [AMBA_PARTITION_LINUX_ROOT_FS]                = AMBA_ROOTFS_SIZE,
    [AMBA_PARTITION_LINUX_HIBERNATION_IMAGE]      = AMBA_HIBERNATION_SIZE,

    /* Media partitions */
    [AMBA_PARTITION_STORAGE0]                     = MP_STORAGE1_SIZE,
    [AMBA_PARTITION_STORAGE1]                     = MP_STORAGE2_SIZE,
    [AMBA_PARTITION_VIDEO_REC_INDEX]              = MP_INDEX_SIZE,
    [AMBA_PARTITION_USER_SETTING]                 = MP_PREFERENCE_SIZE,
    [AMBA_PARTITION_CALIBRATION_DATA]             = MP_CALIBRATION_SIZE,
};

/**
 * User defined idle function
 *
 * @return none
 */
void AmbaUserIdleFunction(void)
{
#if 0
    AmbaUserGPIO_LedCtrl(1);
    AmbaSysTimer_Wait(1000, 1000);   /* wait for a while */
    AmbaUserGPIO_LedCtrl(0);
#else
    static UINT32 LedFlag = 0;

    LedFlag ^= 1;
    AmbaUserGPIO_LedCtrl(LedFlag);
#endif
}

/**
 * User defined external GPIO initialization
 *
 * @return none
 */
void AmbaUserExpGPIO_Init(void)
{
//    AmbaExpGPIO_Init(&ExpGpioPinPortConfig);
    AmbaUserGPIO_LedCtrl(1);    /* Turn on the LED */
}

/**
 * LED control
 *
 * @param [in] LedFlag Led Flag
 *
 * @return none
 */
void AmbaUserGPIO_LedCtrl(UINT32 LedFlag)
{
//    AmbaExpGPIO_ConfigOutput(EX_GPIO_PIN_07, LedFlag ^ 1);
}

/**
 * Sensor Reset Control
 *
 * @param [in] ResetFlag Sensor reset
 *
 * @return none
 */
void AmbaUserGPIO_SensorResetCtrl(UINT32 ResetFlag)
{
    AmbaGPIO_ConfigOutput(GPIO_PIN_21, ResetFlag);
}

/**
 * SD power control
 *
 * @param [in] EnableFlag Enable SD power
 *
 * @return none
 */
void AmbaUserGPIO_SDPowerCtrl(UINT32 EnableFlag)
{
}

/**
 * SD voltage
 *
 * @param [in] Level Level
 *
 * @return none
 */
void AmbaUserSD_VoltageCtrl(UINT32 Level)
{
    AMBA_GPIO_PIN_LEVEL_e GpioLevel = (Level ==  SD_POWER_CTRL_33V) ?  AMBA_GPIO_LEVEL_LOW :
                                      (Level ==  SD_POWER_CTRL_18V) ?  AMBA_GPIO_LEVEL_HIGH :
                                      AMBA_GPIO_LEVEL_LOW;
    AmbaRTSL_GpioConfigOutput(GPIO_PIN_0, GpioLevel);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserSD_PowerCtrl
 *
 *  @Description::
 *
 *  @Input      ::
 *      Enable:
 *      Level :
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaUserSD_PowerCtrl(UINT32 Enable)
{
    if(Enable == 1){
        /*Power on SD module*/
        AmbaGPIO_ConfigOutput(GPIO_PIN_52, AMBA_GPIO_LEVEL_HIGH);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_57_SD_CLK);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_58_SD_CMD);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_73_SD_D_0);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_74_SD_D_1);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_75_SD_D_2);
        AmbaGPIO_ConfigAltFunc(GPIO_PIN_76_SD_D_3);
    }else{
        /*Power off SD and Config CLK/CMD/DAT/ as gpio output 0*/
        AmbaGPIO_ConfigOutput(GPIO_PIN_57, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_58, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_73, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_74, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_75, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_76, AMBA_GPIO_LEVEL_LOW);
        AmbaGPIO_ConfigOutput(GPIO_PIN_52, AMBA_GPIO_LEVEL_LOW);
    }
}

/**
 * LCD backlight control
 *
 * @param [in] LcdFlag LCD flag
 *
 * @return none
 */
void AmbaUserGPIO_LcdCtrl(UINT32 LcdFlag)
{
    if (LcdFlag == 0)
        AmbaGPIO_ConfigOutput(GPIO_PIN_53, AMBA_GPIO_LEVEL_LOW);
    else
        AmbaGPIO_ConfigOutput(GPIO_PIN_53, AMBA_GPIO_LEVEL_HIGH);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserConsole_Write
 *
 *  @Description:: Write string to console
 *
 *  @Input      ::
 *      StringSize: Number of characters
 *      StringBuf:  Character buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   Number of written characters
\*-----------------------------------------------------------------------------------------------*/
int AmbaUserConsole_Write(int StringSize, char *StringBuf, UINT32 TimeOut)
{
    AmbaUART_Write(AMBA_UART_CHANNEL0, StringSize, (UINT8 *)StringBuf, TimeOut);
    return StringSize;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserConsole_Read
 *
 *  @Description:: Read string from console
 *
 *  @Input      ::
 *      StringSize: Number of characters
 *      StringBuf:  Character buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   Number of read characters
\*-----------------------------------------------------------------------------------------------*/
int AmbaUserConsole_Read(int StringSize, char *StringBuf, UINT32 TimeOut)
{
    AMBA_UART_CHANNEL_e UartChanNo = AMBA_UART_CHANNEL0;
    return AmbaUART_Read(UartChanNo, StringSize, (UINT8 *)StringBuf, TimeOut);
}

/**
 * Audio codec initialization
 *
 * @return none
 */
void AmbaUserAudioCodec_Init(void)
{
    /* Audio CODEC Setting */
    AmbaGPIO_ConfigOutput(GPIO_PIN_49, AMBA_GPIO_LEVEL_LOW);
    AmbaKAL_TaskSleep(1);
    AmbaGPIO_ConfigOutput(GPIO_PIN_49, AMBA_GPIO_LEVEL_HIGH);
    AmbaKAL_TaskSleep(1);
    AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_0);
    AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_0, I2S);
    AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_0, 48000);
    /* A12 dragonfly bub has only MIC-IN now */
    AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_0, AMBA_AUDIO_CODEC_MIC_IN);
    AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_0, AMBA_AUDIO_CODEC_SPEAKER_OUT);
    AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_0, 0);
}

/**
 *  Get BSP(HW) version
 *
 *  @return Bsp version ID
 */
BSP_GOAT_VERSDION_ID_e AmbaUser_GetVersion(void)
{
    static BSP_GOAT_VERSDION_ID_e BspVersionID = AMBA_BSP_GOAT_VERSION_UNKNOWN;

    if (BspVersionID == AMBA_BSP_GOAT_VERSION_UNKNOWN) {
        AMBA_GPIO_PIN_INFO_s pinInfo1={0};
        AMBA_GPIO_PIN_INFO_s pinInfo2={0};
        AmbaGPIO_ConfigInput(GPIO_HW_VER1);
        AmbaGPIO_ConfigInput(GPIO_HW_VER2);
        AmbaGPIO_GetPinInfo(GPIO_HW_VER1, &pinInfo1);
        AmbaGPIO_GetPinInfo(GPIO_HW_VER2, &pinInfo2);
        if ((pinInfo1.Level==AMBA_GPIO_LEVEL_LOW) && (pinInfo2.Level==AMBA_GPIO_LEVEL_LOW)) {
            BspVersionID = AMBA_BSP_GOAT_VERSION_V00;
        } else {
            BspVersionID = AMBA_BSP_GOAT_VERSION_UNKNOWN;// default value
        }
    }

    return BspVersionID;
}

/**
 * Check usb is slave or host
 *
 * @return 1 slave, 0 host
 */
int AmbaUser_CheckIsUsbSlave(void)
{
    UINT32 RegVal;
    /** Check POC_29 */
    RegVal = *((UINT32 *) 0xec170034);

    return (RegVal & 0x20000000)?1:0;
}

/**
 * set SD delay control
 */
void AmbaUserSD_Adjust(void)
{
#if (ENABLE_SD_DLY_CTRL == 1)
    AmbaSD_DelayCtrlReset();
    AmbaSD_DelayCtrlAdjustPhy(SD_DLY_CTRL_RND_DLY, 1024000000 / SD_DLY_CTRL_CLK_SPEED, 1, 0);
#endif
}

typedef struct{
    char *pName;
    int RoundDelay;
    int Speed;
    AMBA_SD_CID Cid;
}SD_ADJUST_T;

#define CARD_NUM  (2)
static SD_ADJUST_T cards[CARD_NUM]={
    {"Kingston_Uhs_8Gb_A1",4096,SD_DLY_CTRL_CLK_SPEED,{65,50,{'S','D','8','G','B'},48,948885,13,8}},
    {"Kingston_Uhs_16Gb_Red",2320,SD_DLY_CTRL_CLK_SPEED,{39,72,{'S','D','1','6','G'},48,30535183,13,8}}
};

void AmbaUserSD_DelayCtrl(AMBA_SD_HOST *pHost, AMBA_SD_CARD *pCard, AMBA_SD_CID CID)
{
    int i;
    for(i=0;i<CARD_NUM;i++){
        if((cards[i].Cid.Mid == pCard->Cid.Mid)&&
          (cards[i].Cid.Oid == pCard->Cid.Oid)&&
          (cards[i].Cid.Prv == pCard->Cid.Prv)&&
          /*(cards[i].Cid.Psn == pCard->Cid.Psn)&& */
          (cards[i].Cid.MDateYear == pCard->Cid.MDateYear)&&
          (cards[i].Cid.MDateMth == pCard->Cid.MDateMth)&&
          (memcmp(cards[i].Cid.Pnm,pCard->Cid.Pnm,5)==0)){
            AmbaSD_DelayCtrlReset();
            AmbaSD_DelayCtrlAdjustPhy(cards[i].RoundDelay, 1024000000 / cards[i].Speed, 1, 0);
            AmbaPrint("%s,RoundDelay=%d",cards[i].pName,cards[i].RoundDelay);
            break;
          }
    }
}

#if 0
/**
 * poll far sensor connected state
 */
int AmbaUserFarSensor_PollConnectedState(void)
{
    UINT16 devide_slave_id = 0x6c;
    int rval = 0;

    AmbaSbrg_DriverInit();
    if (AmbaSbrg_Config(0x3, 0x1, &devide_slave_id) == 0)
        rval = OK;
    else
        rval = NG;
    return rval;
}
#endif
/**
 *  gps module detect
 */
int AmbaUserGps_Detect(void)
{
    AMBA_GPIO_PIN_LEVEL_e level;
    #ifndef ECL_RADAR_MODEL
    AMBA_GPIO_PIN_INFO_s pinInfo;
     AmbaGPIO_ConfigInput(GPIO_PIN_17);
     AmbaGPIO_GetPinInfo(GPIO_PIN_17, &pinInfo);
    level =pinInfo.Level;
    #else
    level=0;
    #endif
    return level;
}




