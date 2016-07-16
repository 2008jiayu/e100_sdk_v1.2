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

#define ECL_RADAR_TASK_SIZE       0x10000
#define ECL_RADAR_TASK_PRIORITY     100
#define MAX_BUF_LINES               20
#define ECL_RADAR_TASK_NAMEE   "ECL_RADAR_TASK"
#define RADAR_UART_CHANNEL    AMBA_UART_CHANNEL1
#define ECL_RADAR_UART_RX_RING_BUF_SIZE  1024
static UINT8 AmbaGPSUartRxRingBuf[ECL_RADAR_UART_RX_RING_BUF_SIZE] __attribute__((section (".bss.noinit"))) ;
#define MAX_GPS_RAW (128)
static int error_count=0;
typedef struct _G_ECL_RADAR_CFG_t_ {
    UINT8  TaskPriority;
    UINT32 TaskStackSize;
    void*  TaskStack;
} G_ECL_RADAR_CFG;

typedef struct _G_ECL_RADAR_t_ {
    UINT8 Init;
    AMBA_KAL_TASK_t Task;
    AMBA_KAL_MUTEX_t Mutex;
} G_ECL_RADAR_t;

static G_ECL_RADAR_t G_ECL_Radar = { 0 };

static UINT8 ECL_RadarStack[ECL_RADAR_TASK_SIZE];

UINT8 str[2][MAX_GPS_RAW];
GPS_HANDLE ecl_radar;

int radar_ublox_power_on(gps_dev_s  *dev)
{
    // K_RADAR_CMD_T RadarEn;
    // {
    //     RadarEn.cmd_pack.flag = 0x0110;
    //     RadarEn.cmd_pack.size = 0x0002;
    //     RadarEn.cmd_pack.index = 0x0000;
    //     RadarEn.cmd = 0x0007;
    // }
    AMBA_UART_CONFIG_s UartConfig = {0};
    dev->status.mode = GPS_NORMAL_MODE;
    UartConfig.BaudRate = 115200;                               /* Baud Rate */
    UartConfig.DataBit = AMBA_UART_DATA_8_BIT;                  /* number of data bits */
    UartConfig.Parity = AMBA_UART_PARITY_NONE;                  /* parity */
    UartConfig.StopBit = AMBA_UART_STOP_1_BIT;                  /* number of stop bits */
    UartConfig.FlowCtrl = AMBA_UART_FLOW_CTRL_NONE;             /* flow control */

    UartConfig.MaxRxRingBufSize = sizeof(AmbaGPSUartRxRingBuf);    /* maximum receive ring-buffer size in Byte */
    UartConfig.pRxRingBuf = (UINT8 *) &(AmbaGPSUartRxRingBuf[0]);  /* pointer to the receive ring-buffer */
    AmbaUART_Config(AMBA_UART_CHANNEL1, &UartConfig);
    error_count=0;
    // AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_RADAR_CMD_T), (UINT8 *)&RadarEn, AMBA_KAL_WAIT_FOREVER);
    //uart_set_8n1(PRO_GPS_UART, 9600);
    AmbaPrint("power_on gps device, using UART %d", RADAR_UART_CHANNEL);
    return 0;
}

static void Radar_Ublox_Task(UINT32 info)
{
    UINT8 input[MAX_GPS_RAW];
    UINT8 input2[MAX_GPS_RAW];
    int i = 0,j = 0, out = 0;
    int i2 = 0,j2 = 0;
    UINT8 ObjHeadCount=0;
    UINT8 ObjHeadCount2=0;
    UINT8 c;
    UINT8 HeadTemp[6];
    UINT8 HeadTemp2[6];
    static UINT8 ObjFlg=0;
    static UINT8 StatusFlg=0;
    
    static int req_count=0;
    // static int wait_count=0;
    UINT8 loop;
    while (1){
        out = AmbaUART_Read(AMBA_UART_CHANNEL1, 1, &c, 0);
        // c = uart_getchar(AMBA_UART_CHANNEL1);
        if(out != 0) {
            // wait_count=0;
            if(ObjFlg==0x02)
            {
                 input[i]=c;
                 i++;
                 error_count=0;
                if(i>=sizeof(K_RADAR_OBJ_INFO))//(input[2]<<2|input[3]))
                {
                    // input[0]=10;
                    // input[1]=00;
                    // AmbaPrintAdas("OBJ-[%d][%d][%d]",input[0],input[1],ObjHeadCount);
                    // AmbaPrintAdas("$%02d%d%d&",4,4,1);
                    memcpy(str[j], input, sizeof(K_RADAR_OBJ_INFO));
                    radar_drv_obj_info_update(ecl_radar, str[j], sizeof(K_RADAR_OBJ_INFO));
                    j = (j==0)?1:0;
                    i = 0;
                    ObjFlg=0x00;
                    HeadTemp[0]=0x00;
                    memset(input,0,MAX_GPS_RAW); 
                    if(req_count++>=15)
                    {
                        K_RADAR_CMD_T RadarEn;
                        {
                            RadarEn.cmd_pack.flag = 0x0110;
                            RadarEn.cmd_pack.size = 0x0002;
                            RadarEn.cmd_pack.index = 0x0000;
                            RadarEn.cmd = 0x0007;
                        }   
                        req_count=0;               
                        AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_RADAR_CMD_T), (UINT8 *)&RadarEn, AMBA_KAL_WAIT_FOREVER); 
                    }                   
                } 
            }
            else if(ObjFlg==0)  //check obj head
            {
                if(c==0x01)
                {
                    if(HeadTemp[0]==0x01)
                    {
                        HeadTemp[1]=c;
                        ObjFlg=0x01;
                        ObjHeadCount=2;

                    }
                    else
                    {
                        HeadTemp[0]=c;
                    }
                }
                else
                {
                    HeadTemp[0]=0;
                }
            }
            else if(ObjFlg==0x01)
            {

                HeadTemp[ObjHeadCount]=c;
                ObjHeadCount++;

                if(ObjHeadCount>5)
                {
                    // AmbaPrint("OBJ-DATA11[%d][%d][%d]",HeadTemp[2],HeadTemp[3]);
                    // if(HeadTemp[2]==0x80&&HeadTemp[3]==0x00)
                    if(HeadTemp[2]==sizeof(K_RADAR_OBJ_INFO)&&HeadTemp[3]==0x00)
                    {
                        ObjFlg=0x02;                         
                        i=0;                     
                    }
                    else
                    {
                        ObjFlg=0x00;
                        HeadTemp[0]=0x00;
                    }
                }                
            }
            //=========================================================================================================
            if(StatusFlg==0x02)
            {
                 input2[i2]=c;
                 i2++;
                if(i>=sizeof(K_RADAR_STATUS_INFO))//(input[2]<<2|input[3]))
                {
                    memcpy(str[j2], input2, sizeof(K_RADAR_STATUS_INFO));
                     // AmbaPrint("OBJ-STATUS[%d][%d]",HeadTemp2[2],HeadTemp2[3]);
                    radar_drv_status_info_update(ecl_radar, str[j2], sizeof(K_RADAR_STATUS_INFO));
                    j2 = (j2==0)?1:0;
                    i2 = 0;
                    StatusFlg=0x00;
                    HeadTemp[0]=0x00;
                    memset(input2,0,MAX_GPS_RAW);
                    // AmbaPrint("OBJ-DATA");
                } 
            }
            else if(StatusFlg==0)  //check obj head
            {
                if(c==0xf0)
                {    
                    HeadTemp2[0]=c;
                }
                else if(c==0x1a)
                {
                    if(HeadTemp2[0]!=0xf0)
                    {
                        HeadTemp2[0]=0;
                    }
                    else
                    {
                        HeadTemp2[1]=c;
                        StatusFlg=0x01;
                        ObjHeadCount2=2;
                    }                  
                }
                else
                {
                    HeadTemp2[0]=0;
                }
            }
            else if(StatusFlg==0x01)
            {
                HeadTemp2[ObjHeadCount2]=c;
                ObjHeadCount2++;
               
                if(ObjHeadCount2>5)
                {
                    if(HeadTemp2[2]==sizeof(K_RADAR_STATUS_INFO)&&HeadTemp2[3]==0x00)
                    {
                        StatusFlg=0x02;  
                        i2=0;                     
                    }
                    else
                    {
                        StatusFlg=0x00;
                        HeadTemp2[0]=0x00;
                    }
                }                
            }
            //==========================================================================================================
            
        }
        else
        {
            if(error_count++>6000)//600000
            {
                K_RADAR_CMD_T RadarEn;
                {
                    RadarEn.cmd_pack.flag = 0x0110;
                    RadarEn.cmd_pack.size = 0x0002;
                    RadarEn.cmd_pack.index = 0x0000;
                    RadarEn.cmd = 0x0007;
                }   
                error_count=0;               
                AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_RADAR_CMD_T), (UINT8 *)&RadarEn, AMBA_KAL_WAIT_FOREVER);                
            }  
            
            if(error_count==1000)
            {
                memset(input,0,MAX_GPS_RAW); 
                memcpy(str[0], input, sizeof(K_RADAR_OBJ_INFO));
                radar_drv_obj_info_update(ecl_radar, str[0], sizeof(K_RADAR_OBJ_INFO));                
            }
                              
            AmbaKAL_TaskSleep(1);    
                 
        }
#if 0
            input[i]=c;
            i++;
            if(0x01==(input[0]&&0x01==input[1]))//objrct start flg
            {
                if(i>=MAX_GPS_RAW){
                    i = 0;
                    continue;
                }
                if(i==(sizeof(K_ADC_PACKAGE)+sizeof(K_RADAR_OBJ_INFO)-1))//(input[2]<<2|input[3]))
                {
                    // for(loop=0;loop<6;loop++)
                    // {
                    //     AmbaPrint("OBJDATA_GET[%d]",input[loop]);
                    // }
                    memcpy(str[j], &input[6], sizeof(K_RADAR_OBJ_INFO));
                    radar_drv_obj_info_update(ecl_radar, str[j], sizeof(K_RADAR_OBJ_INFO));
                    j = (j==0)?1:0;
                    i = 0;
                    memset(input,0,MAX_GPS_RAW);
                }                
            }
            else if(0x1a==(input[1]&&0xf0==input[0]))
            {
                if(i>=MAX_GPS_RAW){
                    i = 0;
                    continue;
                }
               
                if(i==(sizeof(K_ADC_PACKAGE)+sizeof(K_RADAR_STATUS_INFO)-1))
                {
                     AmbaPrint("OBJstatus_DATA_GET");
                    memcpy(str[j], &input[sizeof(K_ADC_PACKAGE)], sizeof(K_RADAR_STATUS_INFO));
                    radar_drv_status_info_update(ecl_radar, str[j],sizeof(K_RADAR_STATUS_INFO));
                    j = (j==0)?1:0;
                    i = 0;
                    memset(input,0,MAX_GPS_RAW);
                }     
            }
        } 
        else 
        {
            AmbaKAL_TaskSleep(1);
        }
        #endif
    }
}

int radar_ublox_init(gps_dev_s *dev)
{
    int Rval = 0;
    // create task
    Rval = AmbaKAL_TaskCreate(&G_ECL_Radar.Task,
                              ECL_RADAR_TASK_NAMEE,
                              ECL_RADAR_TASK_PRIORITY ,
                              Radar_Ublox_Task,
                              0x0,
                              ECL_RadarStack,
                              ECL_RADAR_TASK_SIZE,
                              AMBA_KAL_AUTO_START);
    if (Rval == OK) {
        AmbaPrintColor(GREEN, "[ECL RADAR MODEL INIT] Create Task success");
    } else {
        AmbaPrintColor(RED, "[ECL RADAR MODEL INIT]  Create Task fail = %d", Rval);
        return NG;
    }

    G_ECL_Radar.Init = 1;
    AmbaPrint("ECL RADAR MODEL Down");
    return 0;
}

int radar_ublox_detect(gps_dev_s *dev)
{
    // set uart 9600 bps N,8,1;
    AMBA_UART_CONFIG_s UartConfig = {0};
    dev->status.mode = GPS_NORMAL_MODE;
    UartConfig.BaudRate = 115200;                               /* Baud Rate */
    UartConfig.DataBit = AMBA_UART_DATA_8_BIT;                  /* number of data bits */
    UartConfig.Parity = AMBA_UART_PARITY_NONE;                  /* parity */
    UartConfig.StopBit = AMBA_UART_STOP_1_BIT;                  /* number of stop bits */
    UartConfig.FlowCtrl = AMBA_UART_FLOW_CTRL_NONE;             /* flow control */

    UartConfig.MaxRxRingBufSize = sizeof(AmbaGPSUartRxRingBuf);    /* maximum receive ring-buffer size in Byte */
    UartConfig.pRxRingBuf = (UINT8 *) &(AmbaGPSUartRxRingBuf[0]);  /* pointer to the receive ring-buffer */
    AmbaUART_Config(AMBA_UART_CHANNEL1, &UartConfig);
    return 1;
}

int radar_ublox_power_off(gps_dev_s *dev)
{

    K_RADAR_CMD_T RadarEn;
    {
        RadarEn.cmd_pack.flag = 0x0110;
        RadarEn.cmd_pack.size = 0x0002;
        RadarEn.cmd_pack.index = 0x0000;
        RadarEn.cmd = 0x0008;
    }
    dev->status.mode = GPS_POWER_OFF;
    AmbaPrint("power_off radar device");
    AmbaUART_Write(AMBA_UART_CHANNEL1, sizeof(K_RADAR_CMD_T), (UINT8 *)&RadarEn, AMBA_KAL_WAIT_FOREVER);
    return 0;
}

int radar_ublox_resume(gps_dev_s *dev)
{
    dev->status.mode = GPS_NORMAL_MODE;
    AmbaPrint("resume radar device");
    return 0;
}

int radar_ublox_suspend(gps_dev_s *dev)
{
    // UINT8 RadarEn[8]={0x01,0x10,0x00,0x02,0x00,0x00,0x00,0x08};//0x08-->off
    dev->status.mode = GPS_POWER_SAVING_MODE;
    // AmbaUART_Write(AMBA_UART_CHANNEL1, 8, &RadarEn, AMBA_KAL_WAIT_FOREVER);
    AmbaPrint("suspend radar device");
    return 0;
}

int radar_ublox_get_raw_data(gps_dev_s *dev, char *gps_raw_data)
{
    memcpy(gps_raw_data, str, MAX_GPS_RAW);
    return strlen(gps_raw_data);
}

/**
 * Initial device driver
 */
GPS_HANDLE radar_drv_init_ublox(void)
{
    gps_dev_s dev;
    strcpy(dev.name, "Ecl_radar_Uart_DRIVER");
    dev.init        = radar_ublox_init;
    dev.power_on    = radar_ublox_power_on;
    dev.power_off   = radar_ublox_power_off;
    dev.resume      = radar_ublox_resume;
    dev.suspend     = radar_ublox_suspend;
    dev.detect      = radar_ublox_detect;
    dev.get_raw_data = radar_ublox_get_raw_data;
    dev.i_type      = GPS_UART;

    dev.status.update_period    = 1000;
    dev.status.mode             = GPS_NORMAL_MODE;

     ecl_radar= gps_drv_attach(&dev);

    return ecl_radar;
}
