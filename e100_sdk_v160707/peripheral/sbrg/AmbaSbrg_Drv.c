#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaPrintk.h"

#define AMBA_SBRG_DEV_IMPL
#include "AmbaSbrg_DevDrv.h"
#include "AmbaSbrg_DevList.h"


typedef struct _AMBA_SBRG_OBJ_s_ {
	UINT8 sbrg_chan_id;
	UINT8 valid;
} AMBA_SBRG_OBJ_s;


#define SBRG_DEV_MAX_NUM	4
static AMBA_SBRG_DEV_s G_sbrg[SBRG_DEV_MAX_NUM];
//static __ARMCC_ALIGN(4) sbrg_obj_t G_sbrg_st[SBRG_DEV_MAX_NUM] __GNU_ALIGN(4);
static AMBA_SBRG_OBJ_s G_sbrg_st[SBRG_DEV_MAX_NUM];



static UINT8 AmbaSbrg_GetSbrgIndex(UINT8 sbrg_chan_id)
{
	UINT8 i = 0;
	for (i = 0; i < SBRG_DEV_MAX_NUM; i++) {
		if (G_sbrg_st[i].sbrg_chan_id == sbrg_chan_id)
			return i;
	}

	return 0xff;
}

/**
 * Initilzation of default LCD device driver at the system bootup.
 * The default device driver is selected menually at menuconfig
 */
int AmbaSbrg_DriverInit(void)
{
#ifdef CONFIG_SBRIDGE_S2
	sbrg_drv_init_s2();
#endif

#ifdef CONFIG_SBRIDGE_S3D
	sbrg_drv_init_s3d();
#endif

#ifdef CONFIG_SBRIDGE_TI913914
	Ds90ub91x_DrvDevInit();
#endif
	return OK;
}

/**
 * Fully remove the SBridge device driver.
 */
int AmbaSbrg_DriverRemove(UINT8 sbrg_chan_id)
{
	UINT8 rval = 0;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff)
		return OK;

	memset(&G_sbrg_st[rval], 0x0, sizeof(AMBA_SBRG_OBJ_s));
	memset(&G_sbrg[rval], 0x0, sizeof(AMBA_SBRG_DEV_s));

	return OK;
}

/**
 * Attach the SBridge device driver and enable the device control.
 */
int AmbaSbrg_DriverAttach(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_s *sbrg)
{
	UINT8 i = 0;
	if (sbrg == NULL)
		return NG;

	AmbaSbrg_DriverRemove(sbrg_chan_id);

	while (i < SBRG_DEV_MAX_NUM) {
		if (G_sbrg_st[i].valid == 0) {
			G_sbrg_st[i].sbrg_chan_id = sbrg_chan_id;
			G_sbrg_st[i].valid = 1;
			memset(&G_sbrg[i], 0x0, sizeof(AMBA_SBRG_DEV_s));
			memcpy(&G_sbrg[i], sbrg, sizeof(AMBA_SBRG_DEV_s));
			break;
		} else {
			i++;
		}
	}

	return OK;
}

/**
 * reset function for SBridge
 */
int AmbaSbrg_Reset(UINT8 sbrg_chan_id, UINT16 mode)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].reset != NULL) {
		return (G_sbrg[rval].reset(&mode));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
* Configure the top function for SBridge
*/
int AmbaSbrg_Config(UINT8 sbrg_chan_id, UINT16 sub_module_id, void *cfg)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].config != NULL) {
		return (G_sbrg[rval].config(&sub_module_id, cfg));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Update the registers
 */
int AmbaSbrg_WriteReg(UINT8 sbrg_chan_id, UINT16 addr, UINT16 data, UINT16 size)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].write_reg != NULL) {
		return (G_sbrg[rval].write_reg(addr, data, size));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Update the registers in the burst mode.
 */
int AmbaSbrg_BurstWriteReg(UINT8 sbrg_chan_id, UINT16 addr, UINT16 data, UINT16 size)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].burst_write_reg != NULL) {
		return (G_sbrg[rval].burst_write_reg(addr, data, size));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Read the register value
 */
int AmbaSbrg_ReadReg(UINT8 sbrg_chan_id, UINT16 addr, UINT8 *data, UINT16 size)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].read_reg != NULL) {
		return (G_sbrg[rval].read_reg(addr, data, size));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Update the registers (extention)
 */
int AmbaSbrg_WriteDevice(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_ACCESS_s *param)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].write_dev != NULL) {
		return (G_sbrg[rval].write_dev(param));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}


/**
 * Read the register value (extention)
 */
int AmbaSbrg_ReadDev(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_ACCESS_s *param)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].read_dev != NULL) {
		return (G_sbrg[rval].read_dev(param));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Dump information for SBridge
 */
int AmbaSbrg_GetInfo(UINT8 sbrg_chan_id)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].info != NULL) {
		return (G_sbrg[rval].info(&G_sbrg[rval]));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}


/**
 * Get device module
 */
int AmbaSbrg_GetDevice(UINT8 sbrg_chan_id, char *sbrg_name)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].name != NULL) {
		strcpy(sbrg_name, G_sbrg[rval].name);
		return OK;
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Enable device module
 */
int AmbaSbrg_Enable(UINT8 sbrg_chan_id)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].enable != NULL) {
		return (G_sbrg[rval].enable());
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Disable device module
 */
int AmbaSbrg_Disable(UINT8 sbrg_chan_id)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].disable != NULL) {
		return (G_sbrg[rval].disable());
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

/**
 * Capture one frame only
 */
int AmbaSbrg_CaptureOneFrame(UINT8 sbrg_chan_id, void *func)
{
	UINT8 rval;

	rval = AmbaSbrg_GetSbrgIndex(sbrg_chan_id);

	if (rval == 0xff) {
		AmbaPrint("SBRG - Can not find the sbrg ID!!");
		return NG;
	}

	if (G_sbrg[rval].capture_one_frm != NULL) {
		return (G_sbrg[rval].capture_one_frm(func));
	} else {
		AmbaPrint("SBRG - NULL function is called!!");
		return NG;
	}
}

