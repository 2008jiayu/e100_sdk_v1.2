#include <system/ApplibSys_Gyro.h>
#include <apps/apps.h>

static short Gyro_grade = 9800;
int AppGyro_Init(void)
{
    return 0;
}
int AppGyro_DetectHandle(void)
{
    short acc_std=0;
    acc_std=AppLibSysGyro_getdata();
    if(-1!=acc_std)
    {
        if (acc_std > Gyro_grade)
        {
            AmbaPrintColor(BLUE,"urgent event:send msg:AMSG_CMD_GSENSOR_EVEN");
            AppLibComSvcHcmgr_SendMsg(AMSG_CMD_EVENT_RECORD, 0, 0);
            return;
        }

    }

}

int AppGyro_set_event_sensitivity(UINT16 sens)
{
    if(sens == 1)
        Gyro_grade = 4800;
    else if(sens == 2)
        Gyro_grade = 9800;
    else if(sens == 3)
        Gyro_grade = 13500;
    else
        Gyro_grade = 0;
    return 0;
}

int AppGyro_Register(void)
{
    APPLIB_GYRO_s Dev = {0};
    
    Dev.Id = 0;
    WCHAR DevName[] = {'G','Y','R','O','\0'};
    w_strcpy(Dev.Name, DevName);
    Dev.Enable = 0;
    Dev.Init = AppGyro_Init;
    Dev.DetectHandle= AppGyro_DetectHandle;
    Dev.event_sensitivity= AppGyro_set_event_sensitivity;

    AppLibSysGyro_Attach(&Dev);

    return 0;
}
