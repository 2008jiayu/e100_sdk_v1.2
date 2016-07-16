/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_USB.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: USB RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_USB_H_
#define _AMBA_RTSL_USB_H_

#include "AmbaDataType.h"
#include "AmbaINT_Def.h"

#define AMBA_RTSL_ISR_USB                          AMBA_VIC_INT_ID4_USB
#define AMBA_RTSL_ISR_USB_TRIGGER_LEVEL          AMBA_VIC_INT_HIGH_LEVEL_TRIGGER

#define AMBA_RTSL_ISR_HCD_EHCI                    AMBA_VIC_INT_ID39_USB_EHCI
#define AMBA_RTSL_ISR_HCD_EHCI_TRIGGER_LEVEL    AMBA_VIC_INT_HIGH_LEVEL_TRIGGER

#define AMBA_RTSL_ISR_HCD_OHCI                    AMBA_VIC_INT_ID44_USB_OHCI
#define AMBA_RTSL_ISR_HCD_OHCI_TRIGGER_LEVEL    AMBA_VIC_INT_HIGH_LEVEL_TRIGGER

#define AMBA_RTSL_DMA_FIX             0x00000000

#define USB_RTSL_DEV_SPD_HI      0   // 00 (RW) - PHY CLK = 30 or 60 MHz
#define USB_RTSL_DEV_SPD_FU      1   // 01 (RW) - PHY CLK = 30 or 60 MHz
#define USB_RTSL_DEV_SPD_LO      2   // 10 (RW) - PHY CLK = 6 MHz
#define USB_RTSL_DEV_SPD_FU48    3   // 11 (RW) - PHY CLK = 48 MHz

#define USB_RTSL_DEV_SELF_POWER              1
#define USB_RTSL_DEV_REMOTE_WAKEUP           1
#define USB_RTSL_DEV_PHY_8BIT                1
#define USB_RTSL_DEV_UTMI_DIR_UNI            0
#define USB_RTSL_DEV_UTMI_DIR_BI             1
#define USB_RTSL_DEV_HALT_ACK                0
#define USB_RTSL_DEV_HALT_STALL              1
#define USB_RTSL_DEV_SET_DESC_STALL          0
#define USB_RTSL_DEV_SET_DESC_ACK            1
#define USB_RTSL_DEV_CSR_PRG                 1

#define USB_RTSL_DEV_RESUME_WAKEUP           1
#define USB_RTSL_DEV_RX_DMA_ON               1
#define USB_RTSL_DEV_TX_DMA_ON               1
#define USB_RTSL_DEV_LITTLE_ENDN             0
#define USB_RTSL_DEV_DESC_UPD_PYL            0
#define USB_RTSL_DEV_DESC_UPD_PKT            1
#define USB_RTSL_DEV_THRESH_ON               1
#define USB_RTSL_DEV_BURST_ON                1
#define USB_RTSL_DEV_DMA_MODE                1
#define USB_RTSL_DEV_SET_SOFT_DISCON         1
#define USB_RTSL_DEV_SET_FLUSH_RXFIFO        1
#define USB_RTSL_DEV_SET_NAK                 1
#define USB_RTSL_DEV_SET_CSR_DONE            1
#define USB_RTSL_DEV_BURST_LEN               7

typedef enum _USB_PHY0_OWNER_e_ {
    USB_PHY0_DEVICE,  // Phy0 = Device
    USB_PHY0_HOST,  // Phy0 = Host
    USB_PHY0_UNDEF,
} USB_PHY0_OWNER_e;

typedef enum _USB_PORT_STATE_e_ {
    USB_ON,         /* Enable USB port */
    USB_OFF,        /* Disable USB port */
    USB_SUSPEND,    /* Force USB port into suspend state */
    USB_ALWAYS_ON   /* Enable USB port  & force USB to never suspend */
} USB_PORT_STATE_e;

typedef enum _AMBA_USB_DEV_INT_e_ {
    USB_DEV_INT_SC,
    USB_DEV_INT_SI,
    USB_DEV_INT_ES,
    USB_DEV_INT_UR,
    USB_DEV_INT_US,
    USB_DEV_INT_SOF,
    USB_DEV_INT_ENUM,
    USB_DEV_INT_RMTWKP,
    USB_DEV_INT_ALL
} AMBA_USB_DEV_INT_e;

typedef union _USB_DEV_INT_STATUS_u_ {
    UINT32  Data;

    struct {
        UINT32  SetConfig:          1;      /* [0] SetConfiguration command is received (WC) */
        UINT32  SetInterface:       1;      /* [1] SetInterface command is received (WC) */
        UINT32  Empty:              1;      /* [2] USB bus idle state has been detected for 3ms (WC) */
        UINT32  UsbReset:           1;      /* [3] A reset is detected on the USB (WC) */
        UINT32  UsbSuspend:         1;      /* [4] A suspend is detected on the USB (WC) */
        UINT32  Sof:                1;      /* [5] A SOF token is detected on the USB (WC) */
        UINT32  EnumDone:           1;      /* [6] Speed enumeration is completed (WC) */
        UINT32  RemoteWakeUp:       1;      /* [7] A Set/Clear Feature (Remote Wakeup) is received (WC) */
        UINT32  Reserved:           24;     /* [31:8] */
    } Bits;
} USB_DEV_INT_STATUS_u;

typedef union _USB_ENDPOINT_INT_STATUS_u_ {
    UINT32  Data;

    struct {
        UINT32  InEndpoint0:        1;      /* [0] Set when event on In Endpoint 0 (WC) */
        UINT32  InEndpoint1:        1;      /* [1] Set when event on In Endpoint 1 (WC) */
        UINT32  InEndpoint2:        1;      /* [2] Set when event on In Endpoint 2 (WC) */
        UINT32  InEndpoint3:        1;      /* [3] Set when event on In Endpoint 3 (WC) */
        UINT32  InEndpoint4:        1;      /* [4] Set when event on In Endpoint 4 (WC) */
        UINT32  InEndpoint5:        1;      /* [5] Set when event on In Endpoint 5 (WC) */
        UINT32  Reserved:           10;     /* [15:6] */
        UINT32  OutEndpoint0:       1;      /* [16] Set when event on Out Endpoint 0 (WC) */
        UINT32  OutEndpoint1:       1;      /* [17] Set when event on Out Endpoint 1 (WC) */
        UINT32  OutEndpoint2:       1;      /* [18] Set when event on Out Endpoint 2 (WC) */
        UINT32  OutEndpoint3:       1;      /* [19] Set when event on Out Endpoint 3 (WC) */
        UINT32  OutEndpoint4:       1;      /* [20] Set when event on Out Endpoint 4 (WC) */
        UINT32  OutEndpoint5:       1;      /* [21] Set when event on Out Endpoint 5 (WC) */
        UINT32  Reserved1:          10;     /* [31:22] */
    } Bits;
} USB_ENDPOINT_INT_STATUS_u;

int AmbaRTSL_USBIsrHook(UINT32 IntID, UINT32 Priority, UINT32 IntConfig, void (*AMBA_ISR_f)(int IntID));
int AmbaRTSL_USBIsrEnable(UINT32 IntID);
int AmbaRTSL_USBIsrDisable(UINT32 IntID);
int AmbaRTSL_USBIsIsrEnable(UINT32 IntID);
unsigned int AmbaRTSL_USBGetBaseAddress(void);
unsigned int AmbaRTSL_USBGetEhciBaseAddress(void);
unsigned int AmbaRTSL_USBGetOhciBaseAddress(void);
void AmbaRTSL_USBSetEhciOCPolarity(UINT32 polarity);
UINT32 AmbaRTSL_USBGetPinLevel(UINT32 GpioPinID);
int AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_OWNER_e owner);
USB_PHY0_OWNER_e AmbaRTSL_USBGetPhy0Owner(void);
UINT32 AmbaRTSL_USBPhys2Virt(UINT32 phys);
UINT32 AmbaRTSL_USBVirt2Phys(UINT32 virt);
UINT32 AmbaRTSL_UsbGetDevConfig(void);
void AmbaRTSL_UsbSetDevConfig(UINT32 config);
void AmbaRTSL_UsbSetDevConfigSpd(UINT32 speed);
void AmbaRTSL_UsbSetDevConfigRwkp(UINT32 Remotewakeup);
void AmbaRTSL_UsbSetDevConfigSp(UINT32 SlefPowered);
void AmbaRTSL_UsbSetDevConfigSs(UINT32 SyncFrame);
void AmbaRTSL_UsbSetDevConfigPyType(UINT32 PhyType);
void AmbaRTSL_UsbSetDevConfigRev(void);
void AmbaRTSL_UsbSetDevConfigStatus(UINT32 status);
void AmbaRTSL_UsbSetDevConfigPe(UINT32 PhyError);
void AmbaRTSL_UsbSetDevConfigToFull(UINT32 TimeoutFullSpeed);
void AmbaRTSL_UsbSetDevConfigToHigh(UINT32 TimeoutHighSpeed);
void AmbaRTSL_UsbSetDevConfigHaltStatus(UINT32 HaltStatus);
void AmbaRTSL_UsbSetDevConfigDynProg(UINT32 DynamicProgram);
void AmbaRTSL_UsbSetDevConfigSetDesc(UINT32 SetDescriptor);
/*-----------------------------------------------------------------------------------------------*\
 * USB_dev_control Register
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetDevCtl(void);
void AmbaRTSL_UsbSetDevCtl(UINT32 control);
void AmbaRTSL_UsbSetDevCtlRes(UINT32 ResumeSignal);
void AmbaRTSL_UsbSetDevCtlRde(UINT32 EnableRxDma);
UINT32 AmbaRTSL_UsbGetDevCtlRde(void);
void AmbaRTSL_UsbSetDevCtlTde(UINT32 EnableTxDma);
void AmbaRTSL_UsbSetDevCtlDu(UINT32 DescriptorUpdate);
void AmbaRTSL_UsbSetDevCtlBe(UINT32 BigEndian);
void AmbaRTSL_UsbSetDevCtlBf(UINT32 BufferFillMode);
void AmbaRTSL_UsbSetDevCtlThe(UINT32 ThresholdEnable);
void AmbaRTSL_UsbSetDevCtlBren(UINT32 BurstEnable);
void AmbaRTSL_UsbSetDevCtlMode(UINT32 UsbMode);
void AmbaRTSL_UsbSetDevCtlSd(UINT32 SoftDisconnect);
void AmbaRTSL_UsbSetDevCtlScale(UINT32 ScaleDown);
void AmbaRTSL_UsbSetDevCtlDevnak(UINT32 DeviceNak);
void AmbaRTSL_UsbSetDevCtlCsrdone(UINT32 CsrDone);
void AmbaRTSL_UsbSetDevCtlSrxflush(UINT32 RxFifoFlush);
void AmbaRTSL_UsbSetDevCtlBrlen(UINT32 BurstLength);
void AmbaRTSL_UsbSetDevCtlThlen(UINT32 ThresholdLength);
/*-----------------------------------------------------------------------------------------------*\
 * USB_dev_status Register
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetDevStatus(void);
UINT32 AmbaRTSL_UsbGetDevStatusCfg(void);
UINT32 AmbaRTSL_UsbGetDevStatusIntf(void);
UINT32 AmbaRTSL_UsbGetDevStatusAlt(void);
UINT32 AmbaRTSL_UsbGetDevStatusSusp(void);
UINT32 AmbaRTSL_UsbGetDevStatusEnum_spd(void);
UINT32 AmbaRTSL_UsbGetDevStatusRxfifo_empty(void);
UINT32 AmbaRTSL_UsbGetDevStatusPhy_err(void);
UINT32 AmbaRTSL_UsbGetDevStatusRmwk_sts(void);
UINT32 AmbaRTSL_UsbGetDevStatusTs(void);
/*-----------------------------------------------------------------------------------------------*\
 * USB_DevIntStatus status/mask Register
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetDevIntStatus(void);
void AmbaRTSL_UsbSetDevIntStatus(UINT32 InterruptStatus);
UINT32 AmbaRTSL_UsbGetDevIntSc(void);
UINT32 AmbaRTSL_UsbGetDevIntSi(void);
UINT32 AmbaRTSL_UsbGetDevIntES(void);
UINT32 AmbaRTSL_UsbGetDevIntUR(void);
UINT32 AmbaRTSL_UsbGetDevIntUS(void);
UINT32 AmbaRTSL_UsbGetDevIntSOF(void);
UINT32 AmbaRTSL_UsbGetDevIntENUM(void);
UINT32 AmbaRTSL_UsbGetDevIntRMTWKP(void);
void AmbaRTSL_UsbClearDevIntSc(void);
void AmbaRTSL_UsbClearDevIntSi(void);
void AmbaRTSL_UsbClearDevIntES(void);
void AmbaRTSL_UsbClearDevIntUR(void);
void AmbaRTSL_UsbClearDevIntUS(void);
void AmbaRTSL_UsbClearDevIntSOF(void);
void AmbaRTSL_UsbClearDevIntENUM(void);
void AmbaRTSL_UsbClearDevIntRMTWKP(void);
UINT32 AmbaRTSL_UsbGetDevIntMask(void);
void AmbaRTSL_UsbSetDevIntMask(UINT32 InterruptMask);
void AmbaRTSL_UsbSetDevIntMaskSc(UINT32 SetConfig);
void AmbaRTSL_UsbSetDevIntMaskSi(UINT32 SetInterface);
void AmbaRTSL_UsbSetDevIntMaskES(UINT32 BusIdle);
void AmbaRTSL_UsbSetDevIntMaskUR(UINT32 UsbReset);
void AmbaRTSL_UsbSetDevIntMaskUS(UINT32 UsbSuspend);
void AmbaRTSL_UsbSetDevIntMaskSOF(UINT32 SOF);
void AmbaRTSL_UsbSetDevIntMaskENUM(UINT32 EnumDone);
void AmbaRTSL_UsbSetDevIntMaskRMTWKP(UINT32 RemoteWakeup);
/*-----------------------------------------------------------------------------------------------*\
 * USB_endpoint_int_status/mask
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInt(void);
void AmbaRTSL_UsbSetEpInt(UINT32 interrupt);
UINT32 AmbaRTSL_UsbGetInEpIntStatus(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetOutEpIntStatus(UINT32 endpoint);
void AmbaRTSL_UsbClrInEpInt(UINT32 endpoint);
void AmbaRTSL_UsbClrOutEpInt(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetidMaskInt(void);
UINT32 AmbaRTSL_UsbGetInEpMaskInt(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetOutEpMaskInt(UINT32 endpoint);
void AmbaRTSL_UsbEnInEpInt(UINT32 endpoint);
void AmbaRTSL_UsbEnOutEpInt(UINT32 endpoint);
void AmbaRTSL_UsbDisInEpInt(UINT32 endpoint);
void AmbaRTSL_UsbDisOutEpInt(UINT32 endpoint);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_ctrl_in/out Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInCtrl(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStall(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInNAK(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInPollDemand(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInCtrl(UINT32 endpoint, UINT32 control);
void AmbaRTSL_UsbSetEpInStall(UINT32 endpoint, UINT32 stall);
void AmbaRTSL_UsbSetEpInCtrlF(UINT32 endpoint, UINT32 FlushTxFifo);
void AmbaRTSL_UsbSetEpInPollDemand(UINT32 endpoint, UINT32 PollDemand);
void AmbaRTSL_UsbSetEpInEndPointType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEpInNAK(UINT32 endpoint, UINT32 nak);
void AmbaRTSL_UsbClearEpInNAK(UINT32 endpoint, UINT32 ClearNak);
void AmbaRTSL_UsbSetEpInRxReady(UINT32 endpoint, UINT32 RxReady);
void AmbaRTSL_UsbSetEpInCtrlSendNull(UINT32 endpoint, UINT32 SendNull);
UINT32 AmbaRTSL_UsbGetEpOutCtrl(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStall(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutNAK(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutPollDemand(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutCtrl(UINT32 endpoint, UINT32 control);
void AmbaRTSL_UsbSetEpOutStall(UINT32 endpoint, UINT32 stall);
void AmbaRTSL_UsbSetEpOutCtrlF(UINT32 endpoint, UINT32 FlushTxFifo);
void AmbaRTSL_UsbSetEpOutSnoop(UINT32 endpoint, UINT32 snoop);
void AmbaRTSL_UsbSetEpOutPollDemand(UINT32 endpoint, UINT32 PollDemand);
void AmbaRTSL_UsbSetEpOutEndPointType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEpOutNAK(UINT32 endpoint, UINT32 nak);
void AmbaRTSL_UsbClearEpOutNAK(UINT32 endpoint, UINT32 ClearNak);
void AmbaRTSL_UsbSetEpOutRxReady(UINT32 endpoint, UINT32 RxReady);
void AmbaRTSL_UsbSetEpOutCtrlSendNull(UINT32 endpoint, UINT32 SendNull);
void AmbaRTSL_UsbSetEpOutCtrlCloseDesc(UINT32 endpoint, UINT32 CloseDescriptor);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_status_in/out Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInStatus(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusIn(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusBna(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusHe(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusTdc(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusRxpktsz(UINT32 endpo);
UINT32 AmbaRTSL_UsbGetEpInStatusIsoindone(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusRcs(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusRss(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusTxempty(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInStatus(UINT32 endpoint, UINT32 status);
void AmbaRTSL_UsbClrEpInStatusIn(UINT32 endpoint, UINT32 in);
void AmbaRTSL_UsbClrEpInStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable);
void AmbaRTSL_UsbClrEpInStatusHe(UINT32 endpoint, UINT32 AhbError);
void AmbaRTSL_UsbClrEpInStatusTdc(UINT32 endpoint, UINT32 TxDmaComplete);
void AmbaRTSL_UsbSetEpInStatusRxpktsz(UINT32 endpoint, UINT32 RxPacketSize);
void AmbaRTSL_UsbSetEpInStatusIsoindone(UINT32 endpoint, UINT32 IsoInDone);
void AmbaRTSL_UsbClrEpInStatusRcs(UINT32 endpoint, UINT32 RxClearStall);
void AmbaRTSL_UsbClrEpInStatusRss(UINT32 endpoint, UINT32 RxSetStall);
void AmbaRTSL_UsbClrEpInStatusTxempty(UINT32 endpoint, UINT32 TxFifoEmpty);
UINT32 AmbaRTSL_UsbGetEpOutStatus(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusOut(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusBna(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusHe(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusTdc(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusRxpktsz(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusIsoindone(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusRcs(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusRss(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusTxempty(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusCdcclear(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutStatus(UINT32 endpoint, UINT32 status);
void AmbaRTSL_UsbClrEpOutStatusOut(UINT32 endpoint, UINT32 out);
void AmbaRTSL_UsbClrEpOutStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable);
void AmbaRTSL_UsbClrEpOutStatusHe(UINT32 endpoint, UINT32 AhbError);
void AmbaRTSL_UsbClrEpOutStatusTdc(UINT32 endpoint, UINT32 TxDmaComplete);
void AmbaRTSL_UsbSetEpOutStatusRxpktsz(UINT32 endpoint, UINT32 RxPacketSize);
void AmbaRTSL_UsbClrEpOutStatusRcs(UINT32 endpoint, UINT32 RxClearStall);
void AmbaRTSL_UsbClrEpOutStatusRss(UINT32 endpoint, UINT32 RxSetStall);
void AmbaRTSL_UsbClrEpOutStatusTxempty(UINT32 endpoint, UINT32 TxFifoEmpty);
void AmbaRTSL_UsbClrEpOutStatusCdcclear(UINT32 endpoint, UINT32 CloseDescriptorClear);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_buffsize_in Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInBuffsize(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInBuffsizeBufsz(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInBuffsizeIsoinpid(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInBuff(UINT32 endpoint, UINT32 buffer);
void AmbaRTSL_UsbSetEpInBuffsize(UINT32 endpoint, UINT32 size);
void AmbaRTSL_UsbSetEpInBuffIsoinpid(UINT32 endpoint, UINT32 IsoInPid);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_packet_fm_out Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpOutFmout(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutFmoutFrameno(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutFmoutIsooutpid(UINT32 endpoint);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_max_pktsz Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInMaxpksz(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInMaxpksz(UINT32 endpoint, UINT32 size);
UINT32 AmbaRTSL_UsbGetEpOutMaxpksz(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutMaxpksz(UINT32 endpoint, UINT32 size);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_setup_buffptr Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpOutSetupbuf(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutSetupbuf(UINT32 endpoint, UINT32 ptr);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_end[n]_desptr_out Registers
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEpInDesptr(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInDesptr(UINT32 endpoint, UINT32 ptr);
UINT32 AmbaRTSL_UsbGetEpOutDesptr(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutDesptr(UINT32 endpoint, UINT32 ptr);
/*-----------------------------------------------------------------------------------------------*\
 *  USB_udc20 Endpoint Register
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaRTSL_UsbGetEp20(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20LogicalID(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20EndPointDir(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20EndPointType(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20ConfigID(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20Interface(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20AltSetting(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEp20MaxPacketSize(UINT32 endpoint);
void AmbaRTSL_UsbSetEp20(UINT32 endpoint, UINT32 value);
void AmbaRTSL_UsbSetEp20LogicalID(UINT32 endpoint, UINT32 LogicalID);
void AmbaRTSL_UsbSetEp20EndPointDir(UINT32 endpoint, UINT32 direction);
void AmbaRTSL_UsbSetEp20EndPointType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEp20ConfigID(UINT32 endpoint, UINT32 ConfigID);
void AmbaRTSL_UsbSetEp20Interface(UINT32 endpoint, UINT32 InterfaceID);
void AmbaRTSL_UsbSetEp20AltSetting(UINT32 endpoint, UINT32 alternate);
void AmbaRTSL_UsbSetEp20MaxPacketSize(UINT32 endpoint, UINT32 MaxPacketSize);
UINT32 AmbaRTSL_UsbGetPhyCtrl0(void);
UINT32 AmbaRTSL_UsbGetPhyCtrl1(void);
UINT32 AmbaRTSL_UsbGetDpPinState(void);
UINT32 AmbaRTSL_UsbGetChargerDetect(void);
void AmbaRTSL_UsbSetPhyCtrl0(UINT32 value);
void AmbaRTSL_UsbSetPhyCtrl1(UINT32 value);
void AmbaRTSL_UsbEnableDataContactDetect(void);
void AmbaRTSL_UsbDisableDataContactDetect(void);
void AmbaRTSL_UsbEnableDataSrcVoltage(void);
void AmbaRTSL_UsbDisableDataSrcVoltage(void);
void AmbaRTSL_UsbEnableDatDetectVoltage(void);
void AmbaRTSL_UsbDisableDatDetectVoltage(void);
void AmbaRTSL_UsbSetChargeSrcSelect(UINT32 value);
void AmbaRTSL_UsbVbusVicConfig(void);
UINT32 AmbaRTSL_UsbVbusVicRawStaus(void);
void AmbaRTSL_UsbClrDevInt(AMBA_USB_DEV_INT_e IntID);
void AmbaRTSL_UsbSetDevMaskInt(AMBA_USB_DEV_INT_e IntID, UINT32 Mask);
void AmbaRTSL_RctSetUsb0Commononn(UINT32 value);
void AmbaRTSL_RctUsbDeviceSoftReset(void);
void AmbaRTSL_RctUsbHostSoftReset(void);
void AmbaRTSL_RctSetUsbPortState(USB_PORT_STATE_e PortState);
USB_PORT_STATE_e AmbaRTSL_RctGetUsbPortState(void);
void AmbaRTSL_RctSetUsbHostPortState(USB_PORT_STATE_e PortState);
USB_PORT_STATE_e AmbaRTSL_RctGetUsbHostPortState(void);
#endif
