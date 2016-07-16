/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVerImpl.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: the module of Ambarella Network Support
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/17/2014  C.Y.        Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <AmbaDataType.h>
#include <AmbaVer.h>
#include <AmbaPrint.h>


static AMBA_VerInfo_s G_Amba_VerInfo[] = {
    {   .Key    =   "libapp.a"        },
    {   .Key    =   "libapplib.a"     },
    {   .Key    =   "libaudio.a"                    },
    {   .Key    =   "libaudio_sys.a"                },
    {   .Key    =   "libbsp.a"                      },
    {   .Key    =   "libbst_ddr3.a"                 },
    {   .Key    =   "libbst_lpddr2.a"               },
    {   .Key    =   "libcomsvc_fwupdate.a"          },
    {   .Key    =   "libcomsvc_misc.a"              },
    {   .Key    =   "libcomsvc_shell.a"             },
    {   .Key    =   "libcomsvc_sys.a"               },
    {   .Key    =   "libcomsvc_test.a"              },
    {   .Key    =   "libcomsvc_tune.a"              },
    {   .Key    =   "libcomsvc_image.a"              },
    {   .Key    =   "libdsp_dspkernel.a"            },
    {   .Key    =   "libdsp_imgkernel.a"            },
    {   .Key    =   "libfs.a"                       },
    {   .Key    =   "libfwprog.a"                   },
    {   .Key    =   "libimg_adj.a"                  },
    {   .Key    =   "libimg_ae.a"                   },
    {   .Key    =   "libimg_awb.a"                  },
    {   .Key    =   "libimg_calib.a"                },
    {   .Key    =   "libimg_ituner.a"               },
    {   .Key    =   "libimg_proc.a"                 },
    {   .Key    =   "libimg_utils.a"                },
    {   .Key    =   "libimg_schdlr.a"               },
    {   .Key    =   "libimg_encmonitor.a"           },
    {   .Key    =   "libkal.a"                      },
    {   .Key    =   "liblink.a"                     },
    {   .Key    =   "libmw_cfs.a"                   },
    {   .Key    =   "libmw_dataflow.a"              },
    {   .Key    =   "libmw_dspflow.a"               },
    {   .Key    =   "libmw_net.a"                   },
    {   .Key    =   "libmw_unittest.a"              },
    {   .Key    =   "libperipheral.a"               },
    {   .Key    =   "libprfile.a"                   },
    {   .Key    =   "libsoc.a"                      },
    {   .Key    =   "libsoc_bld.a"                  },
    {   .Key    =   "libsoc_bst_ddr3.a"             },
    {   .Key    =   "libsoc_bst_lpddr2.a"           },
    {   .Key    =   "libsoc_non_os.a"               },
    {   .Key    =   "libsoc_test.a"                 },
    {   .Key    =   "libssp_unittest.a"             },
    {   .Key    =   "libthreadx.a"                  },
    {   .Key    =   "libusb.a"                      },
    {   .Key    =   "libusb_mtp.a"                  },
    {   .Key    =   "libusb_msc.a"                  },
    {   .Key    =   "libusb_hid.a"                  },
    {   .Key    =   "libusb_cdcacm.a"               },
    {   .Key    =   "libusb_simple.a"               },
    {   .Key    =   "libusb_stream.a"               },
    {   .Key    =   "libusb_dfu.a"                  },
    {   .Key    =   "libusb_uvc.a"                  },
    {   .Key    =   "libusb_deviceclasses.a"        },
    {   .Key    =   "libusb_driver.a"               },
    {   .Key    =   "libusb_hostclasses.a"          },
    {   .Key    =   "libusb_hostcontroller.a"       },
    {   .Key    =   "libusb_pictbridge.a"           },
    {   .Key    =   "libamboot_bld.a"               },
#ifdef CONFIG_BT_AMBA
    {   .Key    =   "libbt_amba.a"                  },
#endif
#ifdef CONFIG_BUILD_GPL_LIB
    {   .Key    =   "libgpl.a"                      },
#endif
#ifdef CONFIG_BUILD_FOSS_LIB
    {   .Key    =   "libfoss.a"                     },
#endif
};

static UINT32 G_Amba_VerInfo_Count = sizeof(G_Amba_VerInfo) / sizeof(AMBA_VerInfo_s);

typedef void (*Amba_VerFunc_Set)(AMBA_VerInfo_s *pVerInfo);
#define DECLARE_WEAK_VF(f) extern void f(AMBA_VerInfo_s *pVerInfo) __attribute__((weak));

extern const char *pAmbaVer_LinkVer_Target;
extern const char *pAmbaVer_LinkVer_Machine;
extern const char *pAmbaVer_LinkVer_Date;
extern const char *pAmbaVer_LinkLibs[];
extern unsigned int AmbaVer_LinkLibsNum;

DECLARE_WEAK_VF(AmbaVer_SetApp);
DECLARE_WEAK_VF(AmbaVer_SetAppLib);
DECLARE_WEAK_VF(AmbaVer_SetAudio);
DECLARE_WEAK_VF(AmbaVer_SetAudio_sys);
DECLARE_WEAK_VF(AmbaVer_SetBsp);
DECLARE_WEAK_VF(AmbaVer_SetComsvcFwupdate);
DECLARE_WEAK_VF(AmbaVer_SetComsvcMisc);
DECLARE_WEAK_VF(AmbaVer_SetComsvcShell);
DECLARE_WEAK_VF(AmbaVer_SetComsvcSys);
DECLARE_WEAK_VF(AmbaVer_SetComsvcTest);
DECLARE_WEAK_VF(AmbaVer_SetComsvcImage);
DECLARE_WEAK_VF(AmbaVer_SetComsvcTune);
DECLARE_WEAK_VF(AmbaVer_SetDspDspKernel);
DECLARE_WEAK_VF(AmbaVer_SetDspImgKernel);
DECLARE_WEAK_VF(AmbaVer_SetFs);
DECLARE_WEAK_VF(AmbaVer_SetFwprog);
DECLARE_WEAK_VF(AmbaVer_SetImgAdj);
DECLARE_WEAK_VF(AmbaVer_SetImgAe);
DECLARE_WEAK_VF(AmbaVer_SetImgAwb);
DECLARE_WEAK_VF(AmbaVer_SetImgCalib);
DECLARE_WEAK_VF(AmbaVer_SetImgItuner);
DECLARE_WEAK_VF(AmbaVer_SetImgProc);
DECLARE_WEAK_VF(AmbaVer_SetImgSchdlr);
DECLARE_WEAK_VF(AmbaVer_SetImgEncMonitor);
DECLARE_WEAK_VF(AmbaVer_SetImgUtils);
DECLARE_WEAK_VF(AmbaVer_SetKal);
DECLARE_WEAK_VF(AmbaVer_SetLink);
DECLARE_WEAK_VF(AmbaVer_SetMwCfs);
DECLARE_WEAK_VF(AmbaVer_SetMwDataFlow);
DECLARE_WEAK_VF(AmbaVer_SetMwDspFlow);
DECLARE_WEAK_VF(AmbaVer_SetMwNet);
DECLARE_WEAK_VF(AmbaVer_SetMwUnitTest);
DECLARE_WEAK_VF(AmbaVer_SetPeripheral);
DECLARE_WEAK_VF(AmbaVer_SetPrfile);
DECLARE_WEAK_VF(AmbaVer_SetSoc);
DECLARE_WEAK_VF(AmbaVer_SetSocBld);
DECLARE_WEAK_VF(AmbaVer_SetSocBstDdr3);
DECLARE_WEAK_VF(AmbaVer_SetSocBstLpddr2);
DECLARE_WEAK_VF(AmbaVer_SetSocNonOs);
DECLARE_WEAK_VF(AmbaVer_SetSocTest);
DECLARE_WEAK_VF(AmbaVer_SetSspUnitTest);
DECLARE_WEAK_VF(AmbaVer_SetThreadx);
DECLARE_WEAK_VF(AmbaVer_SetUsb)
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassMtp);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassMsc);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassHid);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassSimple);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassStream);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassCdcAcm);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassDfu);
DECLARE_WEAK_VF(AmbaVer_SetUsbDeviceClassUvc);
DECLARE_WEAK_VF(AmbaVer_SetUsbDirver);
DECLARE_WEAK_VF(AmbaVer_SetUsbHostClasses);
DECLARE_WEAK_VF(AmbaVer_SetUsbHostController);
DECLARE_WEAK_VF(AmbaVer_SetUsbPictbridge);
DECLARE_WEAK_VF(AmbaVer_SetAmbootBld);
#ifdef CONFIG_BT_AMBA
DECLARE_WEAK_VF(AmbaVer_SetBT_Amba);
#endif
#ifdef CONFIG_BUILD_GPL_LIB
DECLARE_WEAK_VF(AmbaVer_SetGPL);
#endif
#ifdef CONFIG_BUILD_FOSS_LIB
DECLARE_WEAK_VF(AmbaVer_SetFOSS);
#endif

static void AmbaVer_InitSingle(Amba_VerFunc_Set Verfunc, const char *ModuleName)
{
    int i, found = 0;

    if (Verfunc) {
	    for (i = 0; i < G_Amba_VerInfo_Count; i++) {
            if (strcmp(G_Amba_VerInfo[i].Key, ModuleName) == 0) {
                found = 1;
				break;
            }
	    }

        if (found) {
		    Verfunc(&G_Amba_VerInfo[i]);
        }
    }
}

static void AmbaVer_Show(AMBA_VerInfo_s *pVerInfo)
{
	AmbaPrint("Module:          %s", pVerInfo->Key);
	AmbaPrint("Built Machine:   %s", pVerInfo->MachStr);

	if (pVerInfo->DateStr != NULL)
		if (strcmp(pVerInfo->DateStr, ""))
			AmbaPrint("Built Date:      %s", pVerInfo->DateStr);

	AmbaPrint("Commit count:    %d", pVerInfo->CiCount);
	AmbaPrint("Commit ID:       %s", pVerInfo->CiIdStr);
}

/* ----------------------------------------------------------------------------------------------------------------- */

void AmbaVer_Init(void)
{
	AmbaVer_InitSingle(AmbaVer_SetApp,					"libapp.a");
	AmbaVer_InitSingle(AmbaVer_SetAppLib,				"libapplib.a");
	AmbaVer_InitSingle(AmbaVer_SetAudio,				"libaudio.a");
	AmbaVer_InitSingle(AmbaVer_SetAudio_sys,			"libaudio_sys.a");
	AmbaVer_InitSingle(AmbaVer_SetBsp,					"libbsp.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcFwupdate,		"libcomsvc_fwupdate.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcMisc,			"libcomsvc_misc.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcShell,			"libcomsvc_shell.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcSys,			"libcomsvc_sys.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcTest,			"libcomsvc_test.a");
	AmbaVer_InitSingle(AmbaVer_SetComsvcTune,           "libcomsvc_tune.a");
	AmbaVer_InitSingle(AmbaVer_SetDspDspKernel,			"libdsp_dspkernel.a");
	AmbaVer_InitSingle(AmbaVer_SetDspImgKernel,			"libdsp_imgkernel.a");
	AmbaVer_InitSingle(AmbaVer_SetFs,					"libfs.a");
	AmbaVer_InitSingle(AmbaVer_SetFwprog,				"libfwprog.a");
	AmbaVer_InitSingle(AmbaVer_SetImgAdj,				"libimg_adj.a");
	AmbaVer_InitSingle(AmbaVer_SetImgAe,				"libimg_ae.a");
	AmbaVer_InitSingle(AmbaVer_SetImgAwb,				"libimg_awb.a");
	AmbaVer_InitSingle(AmbaVer_SetImgCalib,				"libimg_calib.a");
	AmbaVer_InitSingle(AmbaVer_SetImgItuner,			"libimg_ituner.a");
	AmbaVer_InitSingle(AmbaVer_SetImgProc,				"libimg_proc.a");
	AmbaVer_InitSingle(AmbaVer_SetImgSchdlr,			"libimg_schdlr.a");
	AmbaVer_InitSingle(AmbaVer_SetImgEncMonitor,        "libimg_encmonitor.a");
	AmbaVer_InitSingle(AmbaVer_SetImgUtils,             "libimg_utils.a");
	AmbaVer_InitSingle(AmbaVer_SetKal,					"libkal.a");
	AmbaVer_InitSingle(AmbaVer_SetLink,					"liblink.a");
	AmbaVer_InitSingle(AmbaVer_SetMwCfs,				"libmw_cfs.a");
	AmbaVer_InitSingle(AmbaVer_SetMwDataFlow,			"libmw_dataflow.a");
	AmbaVer_InitSingle(AmbaVer_SetMwDspFlow,			"libmw_dspflow.a");
	AmbaVer_InitSingle(AmbaVer_SetMwNet,				"libmw_net.a");
	AmbaVer_InitSingle(AmbaVer_SetMwUnitTest,			"libmw_unittest.a");
	AmbaVer_InitSingle(AmbaVer_SetPeripheral,			"libperipheral.a");
	AmbaVer_InitSingle(AmbaVer_SetPrfile,				"libprfile.a");
	AmbaVer_InitSingle(AmbaVer_SetSoc,					"libsoc.a");
	AmbaVer_InitSingle(AmbaVer_SetSocBld,				"libsoc_bld.a");
	AmbaVer_InitSingle(AmbaVer_SetSocBstDdr3,			"libsoc_bst_ddr3.a");
	AmbaVer_InitSingle(AmbaVer_SetSocBstLpddr2,			"libsoc_bst_lpddr2.a");
	AmbaVer_InitSingle(AmbaVer_SetSocNonOs,				"libsoc_non_os.a");
	AmbaVer_InitSingle(AmbaVer_SetSocTest,				"libsoc_test.a");
	AmbaVer_InitSingle(AmbaVer_SetSspUnitTest,			"libssp_unittest.a");
	AmbaVer_InitSingle(AmbaVer_SetThreadx,				"libthreadx.a");
	AmbaVer_InitSingle(AmbaVer_SetUsb,					"libusb.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassMtp,	"libusb_mtp.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassMsc,	"libusb_msc.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassHid,	"libusb_hid.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassCdcAcm,	"libusb_cdcacm.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassSimple,	"libusb_simple.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassStream,	"libusb_stream.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassDfu,	"libusb_dfu.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDeviceClassUvc,	"libusb_uvc.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbDirver,			"libusb_driver.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbHostClasses,		"libusb_hostclasses.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbHostController,	"libusb_hostcontroller.a");
	AmbaVer_InitSingle(AmbaVer_SetUsbPictbridge,		"libusb_pictbridge.a");
	AmbaVer_InitSingle(AmbaVer_SetAmbootBld,			"libamboot_bld.a");
#ifdef CONFIG_BT_AMBA
	AmbaVer_InitSingle(AmbaVer_SetBT_Amba,			"libbt_amba.a");
#endif
#ifdef CONFIG_BUILD_GPL_LIB
    AmbaVer_InitSingle(AmbaVer_SetGPL,                  "libgpl.a");
#endif
#ifdef CONFIG_BUILD_FOSS_LIB
    AmbaVer_InitSingle(AmbaVer_SetFOSS,                 "libfoss.a");
#endif
}

UINT32 AmbaVer_GetCount(void)
{
	return G_Amba_VerInfo_Count;
}

const char *AmbaVer_GetKey(int idx)
{
	return G_Amba_VerInfo[idx].Key;
}

AMBA_VerInfo_s *AmbaVer_GetVerInfo(const char *ModuleName)
{
	int i;

	for (i = 0; i < G_Amba_VerInfo_Count; i++) {
		if (strcmp(G_Amba_VerInfo[i].Key, ModuleName) == 0)
			return &G_Amba_VerInfo[i];
	}

	return NULL;
}

void AmbaVer_ShowVerInfo(const char *ModuleName)
{
	int i;

	for (i = 0; i < G_Amba_VerInfo_Count; i++) {
		if (strcmp(G_Amba_VerInfo[i].Key, ModuleName) == 0)
			break;
	}

	if (i < G_Amba_VerInfo_Count)
		AmbaVer_Show(&G_Amba_VerInfo[i]);
}

void AmbaVer_ShowVerCompactInfo(const char *ModuleName)
{
	int i;

	for (i = 0; i < G_Amba_VerInfo_Count; i++) {
		if (strcmp(G_Amba_VerInfo[i].Key, ModuleName) == 0)
			break;
	}

	if (i < G_Amba_VerInfo_Count)
		AmbaPrint("[%7s] : %s", G_Amba_VerInfo[i].CiIdStr, G_Amba_VerInfo[i].Key);
}


void AmbaVer_ShowAllVerInfo(void)
{
	int i;

	for (i = 0; i < AmbaVer_LinkLibsNum; i++) {
		AmbaVer_ShowVerInfo(pAmbaVer_LinkLibs[i]);
	}
}

void AmbaVer_ShowAllVerCompactInfo(void)
{
	int i;

	for (i = 0; i < AmbaVer_LinkLibsNum; i++) {
		AmbaVer_ShowVerCompactInfo(pAmbaVer_LinkLibs[i]);
	}
}

/* ----------------------------------------------------------------------------------------------------------------- */

void AmbaVer_ShowLinkVerInfo(void)
{
	AmbaPrint("Built %s by %s at %s", pAmbaVer_LinkVer_Target, pAmbaVer_LinkVer_Machine, pAmbaVer_LinkVer_Date);
}

void AmbaVer_GetLinkVerInfo(const char **pLinkTarget, const char **pLinkMachine, const char **pLinkDate)
{
	*pLinkTarget	= pAmbaVer_LinkVer_Target;
	*pLinkMachine	= pAmbaVer_LinkVer_Machine;
	*pLinkDate		= pAmbaVer_LinkVer_Date;
}

