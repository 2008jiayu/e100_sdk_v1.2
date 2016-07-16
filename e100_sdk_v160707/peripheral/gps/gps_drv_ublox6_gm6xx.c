/**
 * @file system/src/peripheral/gps/gps_drv_ublox_gm6xx.c
 *
 * This driver is for u-blox GM-6xx 6-series GPS receiver.
 *
 * History:
 *    2012/02/29 [E-John Lien] - created file
 *    2015/04/13 - [Bill Chou] porting to a12
 *
 * Copyright (C) 2009-2012, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <AmbaDataType.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaPrintk.h>
#include <AmbaUART.h>

#include <string.h>
#include <stdlib.h>

#include <gps.h>
#include <gps_dev.h>
#include <gps_dev_list.h>

#define GPS_GM6XX_TASK_SIZE       0x10000
#define GPS_GM6XX_TASK_PRIORITY    100
#define MAX_BUF_LINES           20
#define GPS_GM6XX_TASK_NAMEE          "GPS_GM6XX_TASK"
#define GPS_UART_CHANNEL    AMBA_UART_CHANNEL1
#define AMBA_GPS_UART_RX_RING_BUF_SIZE  1024
static UINT8 AmbaGPSUartRxRingBuf[AMBA_GPS_UART_RX_RING_BUF_SIZE] __attribute__((section (".bss.noinit"))) ;
#define MAX_GPS_RAW (80)

typedef struct _G_GPS_GM6XX_CFG_t_ {
    UINT8 TaskPriority;
    UINT32 TaskStackSize;
    void* TaskStack;
} G_GPS_GM6XX_CFG;

typedef struct _G_GPS_GM6XX_t_ {
    UINT8 Init;
    AMBA_KAL_TASK_t Task;
    AMBA_KAL_MUTEX_t Mutex;
} G_GPS_GM6XX_t;

static G_GPS_GM6XX_t G_Gps_Gm6xx = { 0 };

static UINT8 GPS_GM6XXStack[GPS_GM6XX_TASK_SIZE];

UINT8 str[2][MAX_GPS_RAW];
GPS_HANDLE gm6xx;

int gps_ublox_power_on(gps_dev_s  *dev)
{
    AMBA_UART_CONFIG_s UartConfig = {0};
    dev->status.mode = GPS_NORMAL_MODE;
    UartConfig.BaudRate = 9600;                               /* Baud Rate */
    UartConfig.DataBit = AMBA_UART_DATA_8_BIT;                  /* number of data bits */
    UartConfig.Parity = AMBA_UART_PARITY_NONE;                  /* parity */
    UartConfig.StopBit = AMBA_UART_STOP_1_BIT;                  /* number of stop bits */
    UartConfig.FlowCtrl = AMBA_UART_FLOW_CTRL_NONE;             /* flow control */

    UartConfig.MaxRxRingBufSize = sizeof(AmbaGPSUartRxRingBuf);    /* maximum receive ring-buffer size in Byte */
    UartConfig.pRxRingBuf = (UINT8 *) &(AmbaGPSUartRxRingBuf[0]);  /* pointer to the receive ring-buffer */
    AmbaUART_Config(AMBA_UART_CHANNEL1, &UartConfig);
    //uart_set_8n1(PRO_GPS_UART, 9600);
    AmbaPrint("power_on gps device, using UART %d", GPS_UART_CHANNEL);

    return 0;
}
static void Gps_Ublox_Task(UINT32 info)
{
    char input[MAX_GPS_RAW];
    int i = 0,j = 0, out = 0;
    UINT8 c;
    while (1){
        out = AmbaUART_Read((AMBA_UART_CHANNEL_e)GPS_UART_CHANNEL, 1, &c, 0);
        //c = uart_getchar(PRO_GPS_UART);
        if (out != 0) {
            if (c == '$') {
                i = 0;
            }
			if(i>=MAX_GPS_RAW){
				i = 0;
				memset(input,0,MAX_GPS_RAW);
				continue;
			}
            input[i] = c;
            i++;
            if (c == '\n') {
                if (input[0] == '$' ) {

					if(i>=MAX_GPS_RAW){
						i = 0;
						continue;
					}
					input[i] = '\0';
                    if(input[0]=='$'&&input[1]=='G'&&input[2]=='P'&&input[3]=='R'&&input[4]=='M'&&input[5]=='C')
                    {
                        AmbaPrint("%s",input);
                    }
                    memcpy(str[j], input, i);
                    gps_drv_data_update(gm6xx, str[j], i);
#if defined(DEBUG_GPS)
                    for (i = 0; i < MAX_GPS_RAW; i++) {
                        if (input[i] == '\n') {
                            input[i] = '\0';
                            break;
                        }
                    }
                    AmbaPrint("raw = %s \n", input);
#endif
					i=0;

                    j = (j==0)?1:0;
                }
            }
            if (i == MAX_GPS_RAW) {
                i = 0;
            }
        } else {
            AmbaKAL_TaskSleep(1);
        }
    }
}

int gps_ublox_init(gps_dev_s *dev)
{
    int Rval = 0;
    // create task
    Rval = AmbaKAL_TaskCreate(&G_Gps_Gm6xx.Task,
                              GPS_GM6XX_TASK_NAMEE,
                              GPS_GM6XX_TASK_PRIORITY ,
                              Gps_Ublox_Task,
                              0x0,
                              GPS_GM6XXStack,
                              GPS_GM6XX_TASK_SIZE,
                              AMBA_KAL_AUTO_START);
    if (Rval == OK) {
        AmbaPrintColor(GREEN, "[GPS GM6XX INIT] Create Task success");
    } else {
        AmbaPrintColor(RED, "[GPS GM6XX INIT] Create Task fail = %d", Rval);
        return NG;
    }

    G_Gps_Gm6xx.Init = 1;
    AmbaPrint("GPS GM6XX done");

    return 0;
}
int gps_ublox_detect(gps_dev_s *dev)
{
    // set uart 9600 bps N,8,1;
    AMBA_UART_CONFIG_s UartConfig = {0};
    dev->status.mode = GPS_NORMAL_MODE;
    UartConfig.BaudRate = 9600;                               /* Baud Rate */
    UartConfig.DataBit = AMBA_UART_DATA_8_BIT;                  /* number of data bits */
    UartConfig.Parity = AMBA_UART_PARITY_NONE;                  /* parity */
    UartConfig.StopBit = AMBA_UART_STOP_1_BIT;                  /* number of stop bits */
    UartConfig.FlowCtrl = AMBA_UART_FLOW_CTRL_NONE;             /* flow control */

    UartConfig.MaxRxRingBufSize = sizeof(AmbaGPSUartRxRingBuf);    /* maximum receive ring-buffer size in Byte */
    UartConfig.pRxRingBuf = (UINT8 *) &(AmbaGPSUartRxRingBuf[0]);  /* pointer to the receive ring-buffer */
    AmbaUART_Config(GPS_UART_CHANNEL, &UartConfig);
    return 1;
}

int gps_ublox_power_off(gps_dev_s *dev)
{
    dev->status.mode = GPS_POWER_OFF;
    AmbaPrint("power_off gps device");
    return 0;
}

int gps_ublox_resume(gps_dev_s *dev)
{
    dev->status.mode = GPS_NORMAL_MODE;
    AmbaPrint("resume gps device");
    return 0;
}

int gps_ublox_suspend(gps_dev_s *dev)
{
    dev->status.mode = GPS_POWER_SAVING_MODE;
    AmbaPrint("suspend gps device");
    return 0;
}

int gps_ublox_get_raw_data(gps_dev_s *dev, char *gps_raw_data)
{
    memcpy(gps_raw_data, str, MAX_GPS_RAW);
    return strlen(gps_raw_data);
}

/**
 * Initial device driver
 */
GPS_HANDLE gps_drv_init_ublox(void)
{
    gps_dev_s dev;

    strcpy(dev.name, "GPS_u-blox6_GM-6xx_DRIVER");
        dev.init        = gps_ublox_init;
        dev.power_on        = gps_ublox_power_on;
        dev.power_off       = gps_ublox_power_off;
        dev.resume      = gps_ublox_resume;
        dev.suspend         = gps_ublox_suspend;
    dev.detect      = gps_ublox_detect;
        dev.get_raw_data    = gps_ublox_get_raw_data;
    dev.i_type      = GPS_UART;

    dev.status.update_period    = 1000;
    dev.status.mode         = GPS_POWER_OFF;

    gm6xx = gps_drv_attach(&dev);

    return gm6xx;
}
