#include "AmbaSSP.h"

#include "AmbaSensor.h"
#include "AmbaPLL.h"
#include "AmbaTest.h"

static void SensorUT_ShowCmdUsage(AMBA_SHELL_ENV_s *pEnv, int ArgCount, char *pArgVector[])
{
    AmbaPrint("Usage: t %s read <VinID> <SensorID> <Addr> - read sensor register", pArgVector[0]);
    AmbaPrint("       t %s write <VinID> <SensorID> <Addr> <Data> - write sensor register", pArgVector[0]);
    AmbaPrint("       t %s dump <VinID> <SensorID> <StartAddr> <Count> - dump sensor registers", pArgVector[0]);
    AmbaPrint("       t %s gain <VinID> <SensorID> <Gain*1000> - set sensor gain", pArgVector[0]);
    AmbaPrint("       t %s gain_test <VinID> <SensorID> <MinGain> <MaxGain> <Step> <Delay(ms)> - test gain", pArgVector[0]);
    AmbaPrint("       t %s shr <VinID> <SensorID> <ShrTime(us)> - set sensor shutter", pArgVector[0]);
    AmbaPrint("       t %s shr_test <VinID> <SensorID> <MinShr(us)> <MaxShr(us)> <Step> <Delay(ms)> - test shutter", pArgVector[0]);
    AmbaPrint("       t %s clk_si <VinID> <SensorID> <Clock(Hz)> - set sensor input clock", pArgVector[0]);
    AmbaPrint("       t %s hw_reset <VinID> <SensorID> - reset sensor via GPIO, for main VIN only", pArgVector[0]);
    AmbaPrint("       t %s detail <VinID> <SensorID> - print sensor detail information", pArgVector[0]);
    AmbaPrint("       t %s minfo <VinID> <SensorID> - print supported mode information", pArgVector[0]);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SensorUT_TestCmd
 *
 *  @Description:: Sensor test commands
 *
 *  @Input      ::
 *      pEnv:        Shell environment
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int SensorUT_TestCmd(struct _AMBA_SHELL_ENV_s_ *pEnv, int ArgCount, char *pArgVector[])
{
    AMBA_DSP_CHANNEL_ID_u Chan;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
    AMBA_SENSOR_MODE_INFO_s ModeInfo;
    AMBA_SENSOR_DEVICE_INFO_s DeviceInfo;

    float GainFactor, ExposureTime, Minshr, Maxshr, MinshrTime, MaxshrTime, Exp;
    UINT32 Factor, Agc, Dgc, ShutterCtrl, InputGain, InputShr, GR, GB, R, B;
    UINT16 Addr, Data, Count, i, Step, Delay, MinGain, MaxGain;

    AMBA_SENSOR_WB_GAIN_s DesiredWBFactor = {0};
    AMBA_SENSOR_WB_GAIN_s pActualWBFactor ={0};
    AMBA_SENSOR_WB_CTRL_s pWBGainCtrl = {0};

    AmbaPrint("SensorUT_TestCmd cmd: %s", pArgVector[1]);

    if (ArgCount < 4) {
        SensorUT_ShowCmdUsage(pEnv, ArgCount, pArgVector);
        return NG;
    }

    Chan.Bits.VinID = (UINT16)strtoul(pArgVector[2], (char**)NULL, 0);
    Chan.Bits.SensorID = (UINT16)strtoul(pArgVector[3], (char**)NULL, 0);

    if (Chan.Bits.VinID >= AMBA_NUM_VIN_CHANNEL) {
        AmbaPrint("VinID=%u is not valid!", Chan.Bits.VinID);
        return NG;
    }

    if (pAmbaSensorObj[Chan.Bits.VinID] == NULL) {
        AmbaPrint("Please hook up sensor first! (VinID=%u)", Chan.Bits.VinID);
        return NG;
    }

    if (ArgCount == 5 && strcmp(pArgVector[1],"read") == 0) {
        Addr = (UINT16)strtoul(pArgVector[4], (char**)NULL, 0);

        if(AmbaSensor_RegisterRead(Chan, Addr, &Data)==NG){
            AmbaPrint("doesn't support %s", pArgVector[1]);
        }else{
            AmbaPrint("Reg0x%x = 0x%x", Addr, Data);
        }
    } else if (ArgCount == 6 && strcmp(pArgVector[1],"write") == 0) {
        Addr = (UINT16)strtoul(pArgVector[4], (char**)NULL, 0);
        Data = (UINT16)strtoul(pArgVector[5], (char**)NULL, 0);

        AmbaSensor_RegisterWrite(Chan, Addr, Data);
    } else if (ArgCount == 6 && strcmp(pArgVector[1],"dump") == 0) {
        Addr = (UINT16)strtoul(pArgVector[4], (char**)NULL, 0);
        Count = (UINT16)strtoul(pArgVector[5], (char**)NULL, 0);
        
        if(AmbaSensor_RegisterRead(Chan, Addr, &Data)==NG){
            AmbaPrint("doesn't support %s", pArgVector[1]);
        }else{
            for(i = 0; i < Count; ++i) {
                AmbaSensor_RegisterRead(Chan, Addr + i, &Data);
                AmbaPrint("Reg0x%x = 0x%x", Addr + i, Data);
            }
        }
    } else if (ArgCount == 5 && strcmp(pArgVector[1],"gain") == 0) {
        InputGain = (UINT32)strtoul(pArgVector[4], (char**)NULL, 0);
        GainFactor = (float)InputGain / 1000.0;
        AmbaSensor_ConvertGainFactor(Chan, GainFactor, &Factor, &Agc, &Dgc);
        AmbaPrint("DesiredFactor:%f, DigitalGain:%u, AnalogGain:%u", GainFactor, Dgc, Agc);
        AmbaSensor_SetAnalogGainCtrl(Chan, Agc);
        AmbaSensor_SetDigitalGainCtrl(Chan, Dgc);
    } else if (ArgCount == 5 && strcmp(pArgVector[1],"hdr_gain") == 0) {
        InputGain = (UINT32)strtoul(pArgVector[4], (char**)NULL, 0);
        GainFactor = (float)InputGain / 1000.0;
        Chan.Bits.HdrID = 0x3;
        AmbaSensor_ConvertGainFactor(Chan, GainFactor, &Factor, &Agc, &Dgc);
        AmbaPrint("DesiredFactor:%f, DigitalGain:%u, AnalogGain:%u", GainFactor, Dgc, Agc);
        AmbaSensor_SetHdrAnalogGainCtrl(Chan, &Agc);
        AmbaSensor_SetHdrDigitalGainCtrl(Chan, &Dgc);
    } else if (ArgCount == 8 && strcmp(pArgVector[1],"wb") == 0) {
        R = (UINT32)strtoul(pArgVector[4], (char**)NULL, 0);
        GR = (UINT32)strtoul(pArgVector[5], (char**)NULL, 0);
        B = (UINT32)strtoul(pArgVector[6], (char**)NULL, 0);
        GB = (UINT32)strtoul(pArgVector[7], (char**)NULL, 0);
        DesiredWBFactor.R = (float)R / 1000.0;
        DesiredWBFactor.Gr = (float)GR / 1000.0;
        DesiredWBFactor.B = (float)B / 1000.0;
        DesiredWBFactor.Gb = (float)GB / 1000.0;
        AmbaSensor_ConvertWbGainFactor(Chan, DesiredWBFactor, &pActualWBFactor, &pWBGainCtrl);
        AmbaPrint("DesiredWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", DesiredWBFactor.R, DesiredWBFactor.Gr, DesiredWBFactor.Gb, DesiredWBFactor.B);
        AmbaPrint("WBGainCtrl R:%u, Gr:%u, Gb:%u, B:%u", pWBGainCtrl.R, pWBGainCtrl.Gr, pWBGainCtrl.Gb, pWBGainCtrl.B);
        AmbaPrint("ActualWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", pActualWBFactor.R, pActualWBFactor.Gr, pActualWBFactor.Gb, pActualWBFactor.B);
        AmbaSensor_SetHdrWbGainCtrl(Chan, &pWBGainCtrl);
    } else if (ArgCount == 8 && strcmp(pArgVector[1],"gain_test") == 0) {
        MinGain = (UINT16)strtoul(pArgVector[4], (char**)NULL, 0);
        MaxGain = (UINT16)strtoul(pArgVector[5], (char**)NULL, 0);
        Step = (UINT16)strtoul(pArgVector[6], (char**)NULL, 0);
        Delay = (UINT16)strtoul(pArgVector[7], (char**)NULL, 0);

        for(i = 0; i < (MaxGain - MinGain) * Step; ++i) {
            GainFactor = (float)MinGain + (float)i / (float)Step;
            AmbaSensor_ConvertGainFactor(Chan,GainFactor,&Factor,&Agc,&Dgc);
            AmbaPrint("DesiredFactor:%f, DigitalGain:0x%x, AnalogGain:0x%x",GainFactor,Dgc,Agc);
            AmbaSensor_SetAnalogGainCtrl(Chan,Agc);
            AmbaSensor_SetDigitalGainCtrl(Chan,Dgc);
            AmbaKAL_TaskSleep(Delay);
        }
    } else if (ArgCount == 5 && strcmp(pArgVector[1],"shr") == 0) {
        UINT8 ExposureFrames = 0;

        AmbaSensor_GetStatus(Chan, &SensorStatus);
        AmbaSensor_GetModeInfo(Chan, SensorStatus.ModeInfo.Mode, &ModeInfo);
        InputShr = (UINT32)strtoul(pArgVector[4], (char**)NULL, 0);
        ExposureTime = (float)InputShr / 1000000.0;
        AmbaSensor_ConvertShutterSpeed(Chan,ExposureTime,&ShutterCtrl);

        ExposureFrames = (ShutterCtrl / SensorStatus.ModeInfo.NumExposureStepPerFrame);
        ExposureFrames = (ShutterCtrl % SensorStatus.ModeInfo.NumExposureStepPerFrame) ? ExposureFrames+1 : ExposureFrames;
        AmbaSensor_SetSlowShutterCtrl(Chan, ExposureFrames);
        AmbaSensor_SetShutterCtrl(Chan,ShutterCtrl);

        AmbaPrint("ExposureTime:%f s, ShutterCtrl:%d, FrameLengthLines=%d, exp frame:%d",ExposureTime,ShutterCtrl, ModeInfo.FrameLengthLines, ShutterCtrl/ModeInfo.FrameLengthLines+1);
        
        
    } else if (ArgCount == 8 && strcmp(pArgVector[1],"shr_test") == 0) {
        Minshr = (UINT16)strtoul(pArgVector[4], (char**)NULL, 0);
        Maxshr = (UINT16)strtoul(pArgVector[5], (char**)NULL, 0);
        Step = (UINT16)strtoul(pArgVector[6], (char**)NULL, 0);
        Delay = (UINT16)strtoul(pArgVector[7], (char**)NULL, 0);
        
        AmbaSensor_GetStatus(Chan, &SensorStatus);
        AmbaSensor_GetModeInfo(Chan, SensorStatus.ModeInfo.Mode, &ModeInfo);
        
        MinshrTime = (float) Minshr/1000.0; //us -> ms
        MaxshrTime = (float) Maxshr/1000.0;
        
        for(i = 0; i < (MaxshrTime - MinshrTime) * Step; ++i) {
            
            Exp = (float)MinshrTime+ (float)i / (float)Step;
            ExposureTime = (float)Exp / 1000.0; //ms -> s
            AmbaSensor_ConvertShutterSpeed(Chan,ExposureTime,&ShutterCtrl);
            AmbaPrint("ExposureTime:%f s, ShutterCtrl:%d",ExposureTime,ShutterCtrl);
            AmbaSensor_ConvertShutterSpeed(Chan,ExposureTime,&ShutterCtrl);

            AmbaSensor_SetSlowShutterCtrl(Chan, ShutterCtrl/ModeInfo.FrameLengthLines+1); 
            AmbaSensor_SetShutterCtrl(Chan,ShutterCtrl);
            
            AmbaKAL_TaskSleep(Delay);
        }
    } else if (ArgCount == 5 && strcmp(pArgVector[1],"clk_si") == 0) {
        UINT32 SensorInputClk = (UINT32) strtoul(pArgVector[4], (char**)NULL, 0);

        if (Chan.Bits.VinID == 0) {
            AmbaPLL_SetSensorClk(SensorInputClk);
            AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);
        } else if (Chan.Bits.VinID == 1) {
            AmbaPLL_SetEnetClkConfig(1);
            AmbaPLL_SetEnetClk(SensorInputClk);
        }
        if (Chan.Bits.VinID <= 1)
            AmbaPrint("Set sensor clk=%u for Vin%d", SensorInputClk, Chan.Bits.VinID);
    } else if (ArgCount == 4 && strcmp(pArgVector[1],"hw_reset") == 0) {
        AmbaUserGPIO_SensorResetCtrl(0);
        AmbaKAL_TaskSleep(1);
        AmbaUserGPIO_SensorResetCtrl(1);
        AmbaKAL_TaskSleep(10);
    } else if (ArgCount == 4 && strcmp(pArgVector[1],"detail") == 0) {
        AmbaSensor_GetStatus(Chan, &SensorStatus);
        AmbaSensor_GetDeviceInfo(Chan, &DeviceInfo);
        AmbaSensor_GetModeInfo(Chan, SensorStatus.ModeInfo.Mode, &ModeInfo);
        AmbaSensor_GetCurrentGainFactor(Chan, &GainFactor);
        AmbaSensor_GetCurrentShutterSpeed(Chan, &ExposureTime);

        AmbaPrint("\n========== Sensor Device Info ==========");

        AmbaPrint(" full_view_width:\t%d", DeviceInfo.NumEffectivePixelCols);
        AmbaPrint(" full_view_height:\t%d", DeviceInfo.NumEffectivePixelRows);
        AmbaPrint(" sensor_cell_width:\t%.2f um", DeviceInfo.UnitCellWidth);
        AmbaPrint(" sensor_cell_height:\t%.2f um", DeviceInfo.UnitCellHeight);

        AmbaPrint("\n========== General Info ================");
        AmbaPrint(" Sensor mode:\t\t%d", SensorStatus.ModeInfo.Mode.Bits.Mode);

        AmbaPrint(" Main frame rate:\t%f fps", (double)SensorStatus.ModeInfo.FrameTime.FrameRate.TimeScale / SensorStatus.ModeInfo.FrameTime.FrameRate.NumUnitsInTick);
        AmbaPrint(" Sampling factor(hori):\t%2u:%u", SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum, SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen);
        AmbaPrint(" Sampling factor(vert):\t%2u:%u", SensorStatus.ModeInfo.InputInfo.VSubsample.FactorNum, SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen);

        AmbaPrint("\n========== Sensor Output Status ========");
        AmbaPrint(" Line_length_pck:\t%d (HB=%d)", SensorStatus.ModeInfo.LineLengthPck, SensorStatus.ModeInfo.LineLengthPck - SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width);
        AmbaPrint(" Frame_length_lines:\t%d (VB=%d)", SensorStatus.ModeInfo.FrameLengthLines, SensorStatus.ModeInfo.FrameLengthLines - SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height);

        AmbaPrint(" data bits:\t\t%d bits", SensorStatus.ModeInfo.OutputInfo.NumDataBits);
        AmbaPrint(" DSP Phase Shift:\t%d", SensorStatus.ModeInfo.OutputInfo.DspPhaseShift);

        AmbaPrint(" Source start_x:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartX);
        AmbaPrint(" Source start_y:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartY);
        AmbaPrint(" Source end_x:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartX + SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width * SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen - 1);
        AmbaPrint(" Source end_y:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartY + SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height * SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen - 1);
        AmbaPrint(" Source width:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width);
        AmbaPrint(" Source height:\t%d", SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height);

        AmbaPrint("\n========== Vin Window in Pixel Array ===");
        AmbaPrint(" start point:\t\t(%d, %d)", SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX, SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY);
        AmbaPrint(" end point:\t\t(%d, %d)", SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX + SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width - 1,
                  SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY + SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height - 1);
        AmbaPrint(" center point:\t(%d, %d)", SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX + SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width / 2,
                  SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY + SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height / 2);


        AmbaPrint("\n========== Actual Frame Timing =========");
        AmbaPrint(" Pixel/Bit clock:\t%9d Hz", SensorStatus.ModeInfo.FrameTime.InputClk);
        AmbaPrint(" Pixel rate (I/O):\t%9f pixel/sec", SensorStatus.ModeInfo.PixelRate);
        AmbaPrint(" Frame rate:\t\t%.6f frame/sec", (double) SensorStatus.ModeInfo.PixelRate / (SensorStatus.ModeInfo.LineLengthPck * SensorStatus.ModeInfo.FrameLengthLines));

        AmbaPrint(" Row time:\t\t%.6f us", SensorStatus.ModeInfo.RowTime * 1000000.0);
        AmbaPrint(" Vin HB time:\t\t%f us", 1000000.0 * (SensorStatus.ModeInfo.LineLengthPck - SensorStatus.ModeInfo.OutputInfo.OutputWidth) / SensorStatus.ModeInfo.PixelRate);
        AmbaPrint(" Vin VB time:\t\t%f us", 1000000.0 * (SensorStatus.ModeInfo.FrameLengthLines - SensorStatus.ModeInfo.OutputInfo.OutputHeight) * SensorStatus.ModeInfo.LineLengthPck / SensorStatus.ModeInfo.PixelRate);

        AmbaPrint("\n========== Gain Info ====================");
        AmbaPrint(" Gain factor: \t\t%.6f", GainFactor);
        AmbaPrint("\n========== Shutter Info ================");
        AmbaPrint(" Shutter Exposure Time: \t%.6f", ExposureTime);
    } else if (ArgCount == 4 && strcmp(pArgVector[1],"minfo") == 0) {
        AMBA_DSP_CHANNEL_ID_u Chan;
        AMBA_SENSOR_MODE_ID_u Mode;
        AMBA_SENSOR_MODE_INFO_s ModeInfo;

        AmbaPrint("ID\tViewAngle\tWidth\tHeight\tDataWidth  FrameRate\tRowTime\t   VBTime");
        AmbaPrint("\t\t\t\t\t\t\t\t  (ms)      (ms)");
        Chan.Data = 0;
        Mode.Bits.Mode = 0;

        while (AmbaSensor_GetModeInfo(Chan, Mode, &ModeInfo) == OK) {
            if (ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                AmbaPrint("%d\t%dx%d\t%d\t %d\t   %d\t  %6d/%d\t%f  %f",
                          Mode.Bits.Mode,
                          ModeInfo.InputInfo.PhotodiodeArray.Width,
                          ModeInfo.InputInfo.PhotodiodeArray.Height,
                          ModeInfo.OutputInfo.RecordingPixels.Width,
                          ModeInfo.OutputInfo.RecordingPixels.Height,
                          ModeInfo.OutputInfo.NumDataBits, ModeInfo.FrameTime.FrameRate.TimeScale,
                          ModeInfo.FrameTime.FrameRate.NumUnitsInTick,
                          ModeInfo.RowTime * 1000,
                          ModeInfo.RowTime * 1000 * (ModeInfo.FrameLengthLines - ModeInfo.OutputInfo.OutputHeight));
            } else {
                AmbaPrint("%d(HDR)\t%dx%d\t%d\t %d\t   %d\t  %6d/%d\t%f  %f",
                          Mode.Bits.Mode,
                          ModeInfo.InputInfo.PhotodiodeArray.Width,
                          ModeInfo.InputInfo.PhotodiodeArray.Height,
                          ModeInfo.OutputInfo.RecordingPixels.Width,
                          ModeInfo.OutputInfo.RecordingPixels.Height,
                          ModeInfo.OutputInfo.NumDataBits, ModeInfo.FrameTime.FrameRate.TimeScale,
                          ModeInfo.FrameTime.FrameRate.NumUnitsInTick,
                          ModeInfo.RowTime * 1000,
                          ModeInfo.RowTime * 1000 * (ModeInfo.FrameLengthLines - ModeInfo.OutputInfo.OutputHeight));
            }

            Mode.Bits.Mode++;
        }
    } else {
        SensorUT_ShowCmdUsage(pEnv, ArgCount, pArgVector);
        return NG;
    }

    return OK;
}

int SensorUT_TestAdd(void)
{
    AmbaPrint("Adding Sensor Unit Test");

    // hook command
    AmbaTest_RegisterCommand("sensor", SensorUT_TestCmd);

    return OK;
}

